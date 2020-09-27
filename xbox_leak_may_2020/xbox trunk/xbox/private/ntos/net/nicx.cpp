// ---------------------------------------------------------------------------------------
// nicx.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#ifdef XNET_FEATURE_XBOX

// ---------------------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------------------

KINTERRUPT CXnNic::s_InterruptObject;

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#define HWADDR0(_hwaddr) ((const DWORD *)((BYTE *)(_hwaddr)))[0]
#define HWADDR1(_hwaddr) ((const WORD *)((BYTE *)(_hwaddr)))[2]

DefineTag(nicStats,     TAG_ENABLE);        // Trace Nic stats
DefineTag(nicStatsAll,  0);                 // Trace all Nic stats

// ---------------------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------------------

void CXnNic::NicStartXmitRecv()
{
    ICHECK(NIC, USER|UDPC|SDPC);

    PNICCSR pCsr    = PNicCsr();
    pCsr->rx_poll   = RXPOLL_EN | _dwRxPollFreq;
    pCsr->tx_en     = TXEN_ENABLE;
    pCsr->rx_en     = RXEN_ENABLE;
    pCsr->mode      = MODE_TXDM|MODE_RXDM;
}

void CXnNic::NicStopXmitRecv()
{
    ICHECK(NIC, USER|UDPC|SDPC);

    PNICCSR pCsr = PNicCsr();
    int timeout;

    // Turn off the transmitter and receiver

    pCsr->rx_poll = 0;
    pCsr->rx_en = 0;
    pCsr->tx_en = 0;

    // Wait for a max of 5msec until both the transmitter and receiver are idle

    for (timeout=500; timeout--; )
    {
        if (    !(pCsr->rx_sta & RXSTA_BUSY)
            &&  !(pCsr->tx_sta & TXSTA_BUSY))
            break;

        KeStallExecutionProcessor(10);
    }

    // Ensure there is no active DMA transfer in progress

    pCsr->mode = MODE_DISABLE_DMA;

    for (timeout=500; timeout--; )
    {
        if (pCsr->mode & MODE_DMA_IDLE)
            break;

        KeStallExecutionProcessor(10);
    }

    pCsr->mode = 0;
}

BOOLEAN CXnNic::NicIsr(PKINTERRUPT, PVOID pvContext)
{
    return(((CXnNic *)pvContext)->NicProcessIsr());
}

BOOLEAN CXnNic::NicProcessIsr()
{
    Assert(!TestInitFlag(INITF_NIC_STOP));

    // Disable interrupts

    PNicCsr()->intr_mk = 0;

    // Queue the dpc to handle the interrupt

    KeInsertQueueDpc(&_dpc, NULL, NULL);

#ifdef XNET_FEATURE_STATS
    _NicStats.isrCount += 1;
#endif

    return(TRUE);
}

void CXnNic::NicDpc(PKDPC dpc, void * pvContext, void * pvArg1, void * pvArg2)
{
    ((CXnNic *)pvContext)->NicProcessDpc();
}

void CXnNic::NicProcessDpc()
{
    ICHECK(NIC, SDPC);

    PNICCSR pCsr = PNicCsr();

    STATINC(dpcCount);

    while (1)
    {
        DWORD dwMintr = pCsr->mintr;
        DWORD dwIntr  = pCsr->intr;

        if (dwIntr == 0)
            break;

        // Process the MII interrupt before acknowledging it to prevent the auto-poll
        // of the PHY interfering with reading of the current link state

        if (dwIntr & INTR_MINT)
        {
            NicMiiInterrupt(dwMintr, FALSE);
        }

        // Acknowledge interrupts (MII interrupts first)

        pCsr->mintr = dwMintr;
        pCsr->intr  = dwIntr;

        // We always call these two interrupt handlers no matter what the interrupt
        // bits say.  They will do nothing if there isn't anything to receive or transmit.

        NicRecvInterrupt();
        NicXmitInterrupt();

        // If a receive frame was missed, tell the Nic to restart RX polling

        if (dwIntr & INTR_MISS)
        {
            STATINC(rxMissedFrames);
            pCsr->mode = MODE_RXDM;
        }

    }

    // Re-enable interrupts

    pCsr->intr_mk = INTR_ALL;
}

void CXnNic::NicMiiInterrupt(DWORD dwMintr, BOOL fInit)
{
    ICHECK(NIC, USER|UDPC|SDPC);

    PNICCSR pCsr        = PNicCsr();
    DWORD   dwMiics     = pCsr->mii_cs;
    DWORD   dwLinkState = PhyGetLinkState(!fInit);

    if (fInit || (dwLinkState != _dwLinkState))
    {
        TraceSz3(Warning, "+Ethernet link status: %s %dMbps %s-duplex",
            (dwLinkState & XNET_ETHERNET_LINK_ACTIVE) ? "up" : "down",
            (dwLinkState & XNET_ETHERNET_LINK_100MBPS) ? 100 :
                (dwLinkState & XNET_ETHERNET_LINK_10MBPS) ? 10 : 0,
            (dwLinkState & XNET_ETHERNET_LINK_FULL_DUPLEX) ? "full" :
                (dwLinkState & XNET_ETHERNET_LINK_HALF_DUPLEX) ? "half" : "?");

        // NOTE: When the link was up before, we need to stop  both Tx and Rx and then set
        // Rx polling frequency and Tx duplex mode according to the link status.

        if (!fInit)
        {
            NicStopXmitRecv();
        }

        _dwRxPollFreq = (dwLinkState & XNET_ETHERNET_LINK_10MBPS) ? RXPOLL_FREQ_10MPS : RXPOLL_FREQ_100MPS;

        if (dwLinkState & XNET_ETHERNET_LINK_FULL_DUPLEX)
            pCsr->tx_cntl &= ~TXCNTL_HDEN;
        else
            pCsr->tx_cntl |=  TXCNTL_HDEN;

        if (!fInit)
        {
            NicStartXmitRecv();
        }

        if (fInit && (dwLinkState & XNET_ETHERNET_LINK_ACTIVE))
        {
            SetInitFlag(INITF_CONNECTED_BOOT);
        }

        _dwLinkState = dwLinkState;
    }
}

void CXnNic::NicRecvInterrupt()
{
    ICHECK(NIC, UDPC|SDPC);

    CPacket     pkt;
    void *      pv;
    UINT        cb;
    CEnetHdr *  pEnetHdr;
    UINT        uiFlags;
    UINT        uiType;
    RecvDesc *  prd;
    DWORD       dwFlagsCount;

    prd = _prdPtr;

    while (1)
    {
        dwFlagsCount = prd->_dwFlagsCount;

        if (dwFlagsCount & RXDESC_OWN)
            break;
        
        if ((dwFlagsCount & RXDESC_REND) == 0)
        {
            STATINC(rxEndOfFrameErrors);
            goto nextframe;
        }

        if (dwFlagsCount & RXDESC_ERR)
        {
            STATINC_(dwFlagsCount & RXDESC_OFOL,    rxOverFlowErrors);
            STATINC_(dwFlagsCount & RXDESC_CRC,     rxCrcErrors);
            STATINC_(dwFlagsCount & RXDESC_LFER,    rxLengthErrors);
            STATINC_(dwFlagsCount & RXDESC_MAX,     rxMaxFrameErrors);
            STATINC_(dwFlagsCount & RXDESC_LCOL,    rxLateCollisions);
            STATINC_(dwFlagsCount & RXDESC_RUNT,    rxRunts);
            STATINC_(dwFlagsCount & RXDESC_FRAM,    rxFramingErrors);

            // Accept an error frame if RXDESC_FRAM is the only error bit turned on.
            // For all other errors we discard the frame.

            if ((dwFlagsCount & (RXDESC_OFOL|RXDESC_CRC|RXDESC_LFER|RXDESC_MAX|RXDESC_LCOL|RXDESC_RUNT|RXDESC_FRAM)) != RXDESC_FRAM)
                goto nextframe;
        }

        // Until NicStart is called we drop all incoming frames because the upper
        // layers of the stack are still initializing.

        if (!TestInitFlag(INITF_NIC_2))
            goto nextframe;

        pv       = VirAddr(prd->_dwPhyAddr);
        cb       = (dwFlagsCount & 0xFFFF);
        pEnetHdr = (CEnetHdr *)pv;
        uiFlags  = PKTF_TYPE_ENET;
        uiType   = pEnetHdr->_wType;

        PushPktRecvTags(pEnetHdr->_eaDst.IsBroadcast());

        if ((dwFlagsCount & (RXDESC_ERR|RXDESC_FRAM|RXDESC_EXTRA)) == (RXDESC_ERR|RXDESC_FRAM|RXDESC_EXTRA))
        {
            STATINC(rxExtraByteErrors);

            if (cb > 0)
            {
                cb -= 1;
            }
        }

        if (cb < ENET_DATA_MINSIZE)
        {
            STATINC(rxMinSizeErrors);
            goto nextframepop;
        }

        STATINC(rxGoodFrames);

        if (NTOHS((WORD)uiType) <= ENET_DATA_MAXSIZE)
        {
            CIeeeHdr * pIeeeHdr = (CIeeeHdr *)pEnetHdr;
            BYTE abEnetHdr[sizeof(CEnetAddr) * 2];

            if (!pIeeeHdr->IsEnetFrame())
            {
                TraceSz(pktRecv, "[DISCARD] IEEE frame type not supported");
                goto nextframepop;
            }

            // Convert the link header into a standard CEnetHdr

            uiType = pIeeeHdr->_wTypeIeee;
            pv     = (BYTE *)pv + (sizeof(CIeeeHdr) - sizeof(CEnetHdr));
            cb    -= (sizeof(CIeeeHdr) - sizeof(CEnetHdr));

            memcpy(abEnetHdr, pIeeeHdr, sizeof(abEnetHdr));
            memcpy(pv, abEnetHdr, sizeof(abEnetHdr));

            pEnetHdr = (CEnetHdr *)pv;
            pEnetHdr->_wType = (WORD)uiType;
        }

        pv  = (BYTE *)pv + sizeof(CEnetHdr);
        cb -= sizeof(CEnetHdr);

        TraceSz4(pktRecv, "[ENET %s %s %04X][%d]",
                 pEnetHdr->_eaDst.Str(), pEnetHdr->_eaSrc.Str(), NTOHS((WORD)uiType), cb);

        if (pEnetHdr->_eaDst.IsBroadcast())
        {
            uiFlags |= PKTF_RECV_BROADCAST;
        }
        else if (!pEnetHdr->_eaDst.IsEqual(_ea))
        {
#ifdef XNET_FEATURE_XBDM_SERVER
            if (!pEnetHdr->_eaDst.IsEqual(_eaClient))
#endif
            {
                TraceSz(pktWarn, "[DISCARD] Frame should not have been accepted by Nic hardware");
                goto nextframepop;
            }
        }

#ifdef XNET_FEATURE_XBDM_SERVER
        if ((uiFlags & PKTF_RECV_BROADCAST) || (pEnetHdr->_eaDst.IsEqual(_eaClient)))
        {
            if (_pXbdmClient)
            {
                _pXbdmClient->EnetRecv(uiFlags, pv, cb, uiType);
            }
            else if ((uiFlags & PKTF_RECV_BROADCAST) == 0)
            {
                TraceSz(pktRecv, "[DISCARD] Title stack not attached");
            }

            if ((uiFlags & PKTF_RECV_BROADCAST) == 0)
                goto nextframepop;
        }
#endif

        pkt.Init(uiFlags, pv, cb, NULL);
        EnetRecv(&pkt, uiType);

    nextframepop:

        PopPktRecvTags();

    nextframe:

        prd->_dwFlagsCount = RXDESC_OWN | (NIC_FRAME_SIZE - NIC_FRAME_ALIGNMENT - 1);
        prd = PrdNext(prd);
    }

    _prdPtr = prd;
}

void CXnNic::NicXmitInterrupt()
{
    ICHECK(NIC, UDPC|SDPC);

    XmitDesc *  pxd;
    CPacket *   ppkt;
    DWORD       dwFlagsCount;
    
    while (1)
    {
        ppkt = _pqXmit.GetHead();

        if (ppkt == NULL)
            break;

        pxd = _pxdBusy;

        if (ppkt->TestFlags(PKTF_XMIT_DUALPAGE))
        {
            Assert((pxd->_dwFlagsCount & TXDESC_TEND) == 0);
            pxd = PxdNext(pxd);
        }

        dwFlagsCount = pxd->_dwFlagsCount;

        Assert(dwFlagsCount & TXDESC_TEND);

        if (dwFlagsCount & TXDESC_OWN)
            break;

        _pqXmit.RemoveHead();
        _pxdBusy  = PxdNext(pxd);
        _cxdBusy -= (1 + !!ppkt->TestFlags(PKTF_XMIT_DUALPAGE));
        MmLockUnlockBufferPages(ppkt->GetEnetHdr(), sizeof(CEnetHdr) + ppkt->GetCb(), TRUE);

        if (dwFlagsCount & TXDESC_ERR)
        {
            STATINC_(dwFlagsCount & TXDESC_UFLO,    txUnderflowErrors);
            STATINC_(dwFlagsCount & TXDESC_LCOL,    txLateCollisions);
            STATINC_(dwFlagsCount & TXDESC_LCAR,    txLostCarriers);
            STATINC_(dwFlagsCount & TXDESC_DEF,     txDefers);
            STATINC_(dwFlagsCount & TXDESC_EXDEF,   txExcessiveDefers);
            STATINC_(dwFlagsCount & TXDESC_RTRY,    txRetryErrors);
        }
        else
        {
            STATINC(txGoodFrames);
        }

#ifdef XNET_FEATURE_XBDM_SERVER
        if (ppkt->TestFlags(PKTF_XMIT_XBDMCLIENT))
        {
            CXbdmPacket * ppktXbdm = (CXbdmPacket *)ppkt;

            if (_pXbdmClient)
                _pXbdmClient->XmitComplete(ppktXbdm->_pvPkt);
            else
                TraceSz(Warning, "XBDM client detached with active packets in the Xmit queue");
                
            _pqClient.InsertTail(ppktXbdm);
            continue;
        }
#endif

        ppkt->ClearFlags(PKTF_XMIT_DUALPAGE);
        ppkt->Complete(this);
    }

    if (_fXmitFull && _cxdBusy < _cxdPool)
    {
        _fXmitFull = FALSE;

#ifdef XNET_FEATURE_XBDM_SERVER
        if (_pXbdmClient)
            _pXbdmClient->EnetPush();
#endif

        EnetPush();
    }
}

BOOL CXnNic::NicXmitReady()
{
    ICHECK(NIC, UDPC|SDPC);

#ifdef XNET_FEATURE_XBDM_CLIENT
    if (_pXbdmServer)
    {
        return(_pXbdmServer->XmitReady());
    }
#endif

    BOOL fReady = (_cxdBusy < _cxdPool);

    if (!fReady)
    {
        _fXmitFull = TRUE;
    }

    return(fReady);
}

void CXnNic::NicXmit(CPacket * ppkt)
{
    ICHECK(NIC, UDPC|SDPC);

#ifdef XNET_FEATURE_XBDM_CLIENT
    if (_pXbdmServer)
    {
        _pXbdmServer->Xmit(ppkt, ppkt->GetEnetHdr(), ppkt->GetCb() + sizeof(CEnetHdr));
    }
    else
#endif
    {
        Assert(_cxdBusy < _cxdPool);
        Assert(ppkt->GetCb() <= ENET_DATA_MAXSIZE);

        void *      pv      = ppkt->GetEnetHdr();
        UINT        cb      = ppkt->GetCb() + sizeof(CEnetHdr);
        XmitDesc *  pxd     = _pxdFree;
        XmitDesc *  pxd0;
        UINT        cb0;

        TraceSz4(pktXmit, "[ENET dst=%s src=%s %04X][%d]",
                 ((CEnetHdr *)pv)->_eaDst.Str(), ((CEnetHdr *)pv)->_eaSrc.Str(),
                 NTOHS(((CEnetHdr *)pv)->_wType), ppkt->GetCb());

        _pqXmit.InsertTail(ppkt);

        MmLockUnlockBufferPages(pv, cb, FALSE);

        pxd->_dwPhyAddr = MmGetPhysicalAddress(pv);
        cb0 = PAGE_SIZE - (pxd->_dwPhyAddr & (PAGE_SIZE - 1));

        if (cb <= cb0)
        {
            // Entire frame is in a single physical page.  Only need one transmit destriptor.
            // Notice that the length in the _dwFlagsCount is the actual length minus one.
            // That is an odd requirement of the nVidia hardware.

            ppkt->ClearFlags(PKTF_XMIT_DUALPAGE);

            pxd->_dwFlagsCount = TXDESC_OWN | TXDESC_TEND | (cb - 1);
        }
        else
        {
            // The frame straddles a page boundary.  Use two transmit descriptors.  We set
            // up the second descriptor before the first one to prevent the Nic from trying
            // to transmit the frame before we are done setting up the two descriptors.

            ppkt->SetFlags(PKTF_XMIT_DUALPAGE);

            pxd0 = pxd;
            pxd  = PxdNext(pxd0);
            _cxdBusy += 1;

            pxd->_dwPhyAddr     = MmGetPhysicalAddress((BYTE *)pv + cb0);
            pxd->_dwFlagsCount  = TXDESC_OWN | TXDESC_TEND | (cb - cb0 - 1);
            pxd0->_dwFlagsCount = TXDESC_OWN | (cb0 - 1);
        }

        _pxdFree = PxdNext(pxd);
        _cxdBusy += 1;

        if (!TestInitFlag(INITF_NIC_STOP))
        {
            // Tell the Nic to check the transmit ring
            PNICCSR pCsr = PNicCsr();
            pCsr->mode = MODE_TXDM;
        }
    }
}

void CXnNic::NicTimer()
{
    ICHECK(NIC, SDPC);

#ifdef XNET_FEATURE_XBDM_CLIENT
    if (_pXbdmServer)
        return;
#endif

    // This function gets called every 200ms from the main timer.  Occasionally, the
    // transmitter will stall with frames on the queue.  If there are any frames queued,
    // we set the MODE_TXDM bit on the Nic hardware to tell it to check the queue.

    if (_cxdBusy > 0)
    {
        PNICCSR pCsr = PNicCsr();
        pCsr->mode = MODE_TXDM;
    }

    // In case we miss a receive interrupt, process any received frames now

    if ((_prdPtr->_dwFlagsCount & RXDESC_OWN) == 0)
    {
        NicRecvInterrupt();
    }
}

NTSTATUS CXnNic::NicInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    NTSTATUS status = BaseInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

#ifdef XNET_FEATURE_XBDM_CLIENT

    _pXbdmServer = (CXbdmServer *)KeGetCurrentPrcb()->DmEnetFunc;

    if (_pXbdmServer)
    {
        if (!_pXbdmServer->IsValidServer())
        {
            TraceSz(Warning, "XBDM.DLL is incompatible with this version of XNET.");
            return(NETERR_SYSCALL);
        }

        BOOL fLinkIsUp = FALSE;

        status = _pXbdmServer->InitClient(cfgEnetReceiveQueueLength, cfgEnetTransmitQueueLength,
                                          &_ea, &fLinkIsUp);

        if (!NT_SUCCESS(status))
            return(status);

        SetInitFlag(INITF_NIC);

        if (fLinkIsUp)
        {
            SetInitFlag(INITF_CONNECTED_BOOT);
        }

        return(NETERR_OK);
    }

#elif !defined(XNET_FEATURE_XBDM_SERVER)

    // For xnets.lib (secure library), we cannot have XBDM owning the network stack.
    // If we find that it is running, we tell it to release the NIC hardware now.
    // Note that this code will execute on the retail box, but will have no effect
    // because pXbdmServer will be NULL always.  This code sequence makes it possible
    // to run an XBE linked with xnets.lib on a devkit.

    CXbdmServer * pXbdmServer = (CXbdmServer *)KeGetCurrentPrcb()->DmEnetFunc;

    if (pXbdmServer && pXbdmServer->IsValidServer())
        pXbdmServer->NicStop();

#endif

    SetInitFlag(INITF_NIC);

    StatInit();

    PNICCSR pCsr = PNicCsr();

    _ulIntrVector = HalGetInterruptVector(XPCICFG_NIC_IRQ, &_irqlIntr);

    KeInitializeDpc(&_dpc, NicDpc, this);
    KeInitializeInterrupt(&s_InterruptObject, NicIsr, this, _ulIntrVector,
                          _irqlIntr, LevelSensitive, TRUE);

    NicStopXmitRecv();

    // Reset buffer management

    pCsr->mode = MODE_RESET_BUFFERS;
    KeStallExecutionProcessor(10);
    pCsr->mode = 0;
    KeStallExecutionProcessor(10);

    pCsr->mintr_mk  = 0;
    pCsr->intr_mk   = 0;
    pCsr->pm_cntl   = 0;
    pCsr->swtr_cntl = 0;
    pCsr->tx_poll   = 0;
    pCsr->rx_poll   = 0;
    pCsr->tx_sta    = pCsr->tx_sta;
    pCsr->rx_sta    = pCsr->rx_sta;
    pCsr->mintr     = pCsr->mintr;
    pCsr->intr      = pCsr->intr;

    _crdPool = cfgEnetReceiveQueueLength;

    if (_crdPool > (PAGE_SIZE/2)/sizeof(RecvDesc))
        _crdPool = (PAGE_SIZE/2)/sizeof(RecvDesc);

    _cxdPool = cfgEnetTransmitQueueLength + 1;

    if (_cxdPool > (PAGE_SIZE/2)/sizeof(XmitDesc))
        _cxdPool = (PAGE_SIZE/2)/sizeof(XmitDesc);

    XmitDesc * pxd = NULL;
    UINT cbDma = PAGE_SIZE + (_crdPool * NIC_FRAME_SIZE);

#ifdef XNET_FEATURE_XBDM_SERVER
    // Try allocating the DMA memory in the first 64K if possible under XBDM.
    pxd = (XmitDesc *)HalDmaAlloc(cbDma, 64 * 1024);
#endif

    if (pxd == NULL)
        pxd = (XmitDesc *)HalDmaAlloc(cbDma);

    if (pxd == NULL)
    {
        TraceSz(Warning, "Out of memory allocating DMA receive buffers");
        return(NETERR_MEMORY);
    }

    // Clear the entire first page where the transmit and receive descriptors go

    memset(pxd, 0, PAGE_SIZE);

    // Compute the offset between virtual and physical memory for these descriptors

    _dwPhyOff = MmGetPhysicalAddress(pxd) - (DWORD_PTR)pxd;

    // Fill in the transmit descriptor pointers

    _pxdFirst = pxd;
    _pxdBusy  = pxd;
    _pxdFree  = pxd;
    _pxdLast  = pxd + _cxdPool - 1;

    // Decrement _cxdPool so that we can assume that two transmit descriptors are
    // available whenever _cxdBusy < _cxdPool.  This makes for an easier check later.

    _cxdPool -= 1;

    RecvDesc * prd = (RecvDesc *)((BYTE *)pxd + (PAGE_SIZE/2));

    _prdFirst = prd;
    _prdPtr   = prd;
    _prdLast  = prd + _crdPool - 1;

    // Fill in the RecvDesc to point to the frame buffers.  Notice that we bias the
    // frame pointer by NIC_FRAME_ALIGNMENT (2 bytes).  This is because the CEnetHdr
    // is 14 bytes long, and we'd like the start of the IP packet to be on a four-byte
    // boundary.

    DWORD dwPhyAddr = (DWORD_PTR)pxd + _dwPhyOff + PAGE_SIZE + NIC_FRAME_ALIGNMENT;

    for (; prd <= _prdLast; ++prd, dwPhyAddr += NIC_FRAME_SIZE)
    {
        prd->_dwPhyAddr = dwPhyAddr;
        prd->_dwFlagsCount = RXDESC_OWN | (NIC_FRAME_SIZE - NIC_FRAME_ALIGNMENT - 1);
    }

#ifdef XNET_FEATURE_XBDM_SERVER

    // Allocate and enqueue enough CXbdmPacket structures to satisfy the worst case
    // where the client has completely filled the transmit queue.

    CXbdmPacket * ppktXbdm = (CXbdmPacket *)SysAllocZ((_cxdPool + 1) * sizeof(CXbdmPacket), PTAG_CXbdmPacket);

    if (ppktXbdm == NULL)
    {
        TraceSz(Warning, "Out of memory allocating CXbdmPacket");
        return(NETERR_MEMORY);
    }

    _ppktXbdm = ppktXbdm;

    for (int cPkt = _cxdPool + 1; cPkt > 0; --cPkt, ++ppktXbdm)
    {
        _pqClient.InsertTail(ppktXbdm);
    }

#endif

    ULONG ulType, ulSize;
    status = ExQueryNonVolatileSetting(XC_FACTORY_ETHERNET_ADDR, &ulType, (BYTE *)&_ea, sizeof(CEnetAddr), &ulSize);

    if (!NT_SUCCESS(status) || ulSize != sizeof(CEnetAddr))
    {
        // If we failed to read Ethernet address from non-volatile memory,
        // pick a random even address among the first 64 addresses of
        // the 00-50-f2 address block. This is so that we can at least boot
        // on the manufacturing line and start communicating with the test server.

        TraceSz(Warning, "**************************************************************************");
        TraceSz2(Warning, "Unable to read Ethernet address from EEPROM (status=%08X,ulSize=%d).", status, ulSize);
        TraceSz(Warning, "Run the recovery CD to repair.  Continuing with a random Ethernet address.");
        TraceSz(Warning, "**************************************************************************");

        BYTE bAddr;
        Rand(&bAddr, sizeof(bAddr));
        _ea._ab[1] = 0x50;
        _ea._ab[2] = 0xF2;
        _ea._ab[5] = (BYTE)(bAddr & 0x3E);
    }

    _dwRxPollFreq = RXPOLL_FREQ_100MPS;

#ifdef XNET_FEATURE_XBDM_SERVER

    if (_ea._ab[5] & 1)
    {
        TraceSz(Warning, "***************************************************************************");
        TraceSz1(Warning, "Ethernet address of this Development Kit is incorrect (%s).", _ea.Str());
        TraceSz(Warning, "The last bit of the address should not be set.  Clearing last bit and");
        TraceSz(Warning, "continuing.  There may be conflicts with other network devices as a result.");
        TraceSz(Warning, "***************************************************************************");
        
        _ea._ab[5] &= ~1;
    }

    _eaClient = _ea;    // Title always gets EEPROM address
    _ea._ab[5] |= 1;    // Debug always gets next address

    // Enable unicast reception for the Ethernet address of the title stack

    pCsr->uni0 = HWADDR0(_eaClient._ab);
    pCsr->uni1 = HWADDR1(_eaClient._ab);

    // Enable multicast reception for the Ethernet address of the debug stack

    pCsr->mult_mk0 = 0xFFFFFFFF;
    pCsr->mult_mk1 = 0xFFFF;
    pCsr->mult0    = HWADDR0(_ea._ab);
    pCsr->mult1    = HWADDR1(_ea._ab);

#else

    // Enable unicast reception for the Ethernet address of the title stack

    pCsr->uni0 = HWADDR0(_ea._ab);
    pCsr->uni1 = HWADDR1(_ea._ab);

    // Disable multicast reception

    pCsr->mult_mk0 = 0xFFFFFFFF;
    pCsr->mult_mk1 = 0xFFFF;
    pCsr->mult0    = 0xFFFFFFFF;
    pCsr->mult1    = 0xFFFF;

#endif

    // Setup transmitter and receiver
    // NOTE: nVidia NIC somehow expects the maximum
    // receive buffer size is 1518 instead of 1514.
    Assert(NIC_FRAME_SIZE - NIC_FRAME_ALIGNMENT > 1518);
    pCsr->rx_cntl_1 = 1518;
    pCsr->rx_cntl_0 = RXCNTL_DEFAULT;
    pCsr->tx_cntl = TXCNTL_DEFAULT;

    // Randomly generate a backoff control timeout (single byte), but don't let it
    // be zero because that means no seed (aggressive retry).
    BYTE bBackoff;
    Rand(&bBackoff, sizeof(bBackoff));
    bBackoff += (bBackoff == 0);
    pCsr->bkoff_cntl = (BKOFFCNTL_DEFAULT & 0xFFFFFF00) | bBackoff;

    pCsr->tx_def = TXDEF_DEFAULT;
    pCsr->rx_def = RXDEF_DEFAULT;

    pCsr->tx_dadr = PhyAddr(_pxdFirst);
    pCsr->rx_dadr = PhyAddr(_prdFirst);
    pCsr->dlen = ((_crdPool-1) << 16) | (_cxdPool); // _cxdPool already decremented earlier
    pCsr->rx_fifo_wm = RXFIFOWM_DEFAULT;
    pCsr->tx_fifo_wm = TXFIFOWM_DEFAULT;

    // Enable MII auto-polling interrupt (delay auto-poll enable until after PhyInitialize)
    pCsr->mii_cs = MIICS_DEFAULT & ~MIICS_APEN;
    pCsr->mii_tm = MIITM_DEFAULT;
    KeStallExecutionProcessor(50);

    // Initialize the PHY
    status = PhyInitialize(FALSE, NULL);
    if (!NT_SUCCESS(status))
        return(status);

    pCsr->mii_cs |= MIICS_APEN;
    KeStallExecutionProcessor(50);

    NicMiiInterrupt(0, TRUE);
    NicStartXmitRecv();

    pCsr->mintr = pCsr->mintr;
    pCsr->intr = pCsr->intr;
    pCsr->mintr_mk = MINTR_MAPI;
    pCsr->intr_mk = INTR_ALL;

    if (!KeConnectInterrupt(&s_InterruptObject))
    {
        TraceSz(Warning, "Failed to connect NIC interrupt");
        return(STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT);
    }

    _HalShutdownReg.NotificationRoutine = (PHAL_SHUTDOWN_NOTIFICATION)HalShutdownNotification;
    HalRegisterShutdownNotification(&_HalShutdownReg, TRUE);
    SetInitFlag(INITF_NIC_1);

#ifdef XNET_FEATURE_XBDM_SERVER
    
    KeGetCurrentPrcb()->DmEnetFunc = &_XbdmServer;

#endif

    return(NETERR_OK);
}

void CXnNic::NicStart()
{
    ICHECK(NIC, USER);

    BaseStart();

    Assert(!TestInitFlag(INITF_NIC_STOP));

#ifdef XNET_FEATURE_XBDM_CLIENT
    if (_pXbdmServer)
    {
        _pXbdmServer->AttachClient(&_XbdmClient);
        return;
    }
#endif

    SetInitFlag(INITF_NIC_2);
}

void CXnNic::NicFlush()
{
    TCHECK(UDPC|SDPC);

#ifdef XNET_FEATURE_XBDM_CLIENT
    if (_pXbdmServer)
        return;
#endif

    if (!TestInitFlag(INITF_NIC) || TestInitFlag(INITF_NIC_STOP))
        return;

    // Wait up to half a second for packets queued for transmit to go

    EnetPush();

    PNICCSR pCsr = PNicCsr();

    for (UINT cTimeout = 10000; cTimeout > 0; --cTimeout)
    {
        pCsr->mode = MODE_TXDM;
        NicXmitInterrupt();

        if (_pqXmit.IsEmpty())
            break;

        KeStallExecutionProcessor(50);
    }
}

void CXnNic::NicStop()
{
    TCHECK(UDPC|SDPC);

    if (TestInitFlag(INITF_NIC) && !TestInitFlag(INITF_NIC_STOP))
    {
        if (TestInitFlag(INITF_NIC_1))
        {
            HalRegisterShutdownNotification(&_HalShutdownReg, FALSE);
        }

#ifdef XNET_FEATURE_XBDM_CLIENT
        if (_pXbdmServer)
        {
            _pXbdmServer->DetachClient();
        }
        else
#endif
        {
            if (s_InterruptObject.Connected)
            {
                KeDisconnectInterrupt(&s_InterruptObject);
            }

            KeRemoveQueueDpc(&_dpc);
        }

        SetInitFlag(INITF_NIC_STOP);
    }

    BaseStop();
}

void CXnNic::NicTerm()
{
    TCHECK(UDPC);

    NicStop();

    SetInitFlag(INITF_NIC_TERM);

    if (TestInitFlag(INITF_NIC))
    {
        StatTerm();

        if (!_pqXmit.IsEmpty())
        {
            TraceSz1(Warning, "Nic shutdown with %d packet(s) queued for transmit", _pqXmit.Count());
        }

        while (!_pqXmit.IsEmpty())
        {
            CPacket * ppkt = _pqXmit.RemoveHead();
            MmLockUnlockBufferPages(ppkt->GetEnetHdr(), sizeof(CEnetHdr) + ppkt->GetCb(), TRUE);
            Assert(!ppkt->TestFlags(PKTF_XMIT_XBDMCLIENT));
            PacketFree(ppkt);
        }

        if (_pxdFirst)
        {
            HalDmaFree(_pxdFirst);
        }

#ifdef XNET_FEATURE_XBDM_SERVER
        if (_ppktXbdm)
        {
            SysFree(_ppktXbdm);
        }

        KeGetCurrentPrcb()->DmEnetFunc = NULL;
        Assert(_pXbdmClient == NULL);
#endif

        Assert(!s_InterruptObject.Connected);
    }

    BaseTerm();
}

void CXnNic::HalShutdownNotification(HAL_SHUTDOWN_REGISTRATION * pHalShutdownReg)
{
    KIRQL kirql = ::KeRaiseIrqlToDpcLevel();
    CXnNic * pXnNic = (CXnNic *)((BYTE *)pHalShutdownReg - offsetof(CXnNic, _HalShutdownReg));
    ((CXnIp *)pXnNic)->SecRegShutdown(FALSE);
    pXnNic->NicFlush();
    pXnNic->NicStop();
    ::KeLowerIrql(kirql);
}

// ---------------------------------------------------------------------------------------
// Nic Statistics
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_STATS

void CXnNic::StatInit()
{
    _timerStats.Init((PFNTIMER)StatTimer);
}

void CXnNic::StatTerm()
{
    TimerSet(&_timerStats, TIMER_INFINITE);
}

void CXnNic::StatInc(UINT iStat)
{
    ICHECK(NIC, UDPC|SDPC);

    ULONG * pul = (ULONG *)((BYTE *)&_NicStats + iStat);

    *pul += 1;

#ifdef XNET_FEATURE_TRACE

    if (    Tag(nicStats)
        &&  (   Tag(nicStatsAll)
             || (iStat > offsetof(NICSTATS, rxGoodFrames))))
    {
        if (!_timerStats.IsActive())
        {
            TimerSetRelative(&_timerStats, 5 * TICKS_PER_SECOND);
        }
    }

#endif
}

void CXnNic::StatTimer(CTimer * pt)
{
    ICHECK(NIC, UDPC|SDPC);

    #define DUMPSTAT(n) if (_NicStatsLast.n != _NicStats.n) { TraceSz3(nicStats, "%5d [%5d] %s", _NicStats.n - _NicStatsLast.n, _NicStats.n, #n); }

    TraceSz7(nicStats, "[tx %d] [txq %d/%d] [rx %d] [rxq %d] [isr %d] [dpc %d]",
             _NicStats.txGoodFrames, _cxdBusy, _cxdPool, _NicStats.rxGoodFrames, _crdPool,
             _NicStats.isrCount, _NicStats.dpcCount);

    DUMPSTAT(txUnderflowErrors);
    DUMPSTAT(txLateCollisions);
    DUMPSTAT(txLostCarriers);
    DUMPSTAT(txDefers);
    DUMPSTAT(txExcessiveDefers);
    DUMPSTAT(txRetryErrors);
    DUMPSTAT(rxMinSizeErrors);
    DUMPSTAT(rxFramingErrors);
    DUMPSTAT(rxOverFlowErrors);
    DUMPSTAT(rxCrcErrors);
    DUMPSTAT(rxLengthErrors);
    DUMPSTAT(rxMaxFrameErrors);
    DUMPSTAT(rxLateCollisions);
    DUMPSTAT(rxRunts);
    DUMPSTAT(rxExtraByteErrors);
    DUMPSTAT(rxMissedFrames);
    DUMPSTAT(rxEndOfFrameErrors);
    
    _NicStatsLast = _NicStats;
}

#endif

// ---------------------------------------------------------------------------------------
// CXbdmClient
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBDM_CLIENT

INLINE CXnNic * CXnNic::CXbdmClient::GetXnNic()
{
    return((CXnNic *)((BYTE *)this - offsetof(CXnNic, _XbdmClient)));
}

void CXnNic::CXbdmClient::XmitComplete(void * pvPkt)
{
    ((CPacket *)pvPkt)->Complete(GetXnNic());
}

void CXnNic::CXbdmClient::EnetRecv(UINT uiFlags, void * pv, UINT cb, UINT uiType)
{
    CPacket pkt;

    pkt.Init(uiFlags, pv, cb, NULL);

    GetXnNic()->PushPktRecvTags(pkt.GetEnetHdr()->_eaDst.IsBroadcast());
    GetXnNic()->EnetRecv(&pkt, uiType);
    GetXnNic()->PopPktRecvTags();
}

void CXnNic::CXbdmClient::EnetPush()
{
    GetXnNic()->EnetPush();
}

DWORD CXnNic::CXbdmClient::GetXnAddr(XNADDRXBDM * pxnaXbdm)
{
    XNADDR xnaddr;
    DWORD dwFlags = ((CXnIp *)GetXnNic())->IpGetXnAddr(&xnaddr);
    memcpy(pxnaXbdm->abEnet, xnaddr.abEnet, sizeof(pxnaXbdm->abEnet));
    pxnaXbdm->ina = xnaddr.ina;
    return(dwFlags);
}

DWORD CXnNic::NicGetOtherXnAddr(XNADDR * pxna)
{
    XNADDRXBDM xnaddrXbdm = { 0 };
    DWORD dwFlags = _pXbdmServer ? _pXbdmServer->GetXnAddr(&xnaddrXbdm) : XNET_GET_XNADDR_NONE;
    memset(pxna, 0, sizeof(*pxna));
    memcpy(pxna->abEnet, xnaddrXbdm.abEnet, sizeof(pxna->abEnet));
    pxna->ina = xnaddrXbdm.ina;
    return(dwFlags);
}

#endif

// ---------------------------------------------------------------------------------------
// CXbdmServer
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBDM_SERVER

INLINE CXnNic * CXnNic::CXbdmServer::GetXnNic()
{
    return((CXnNic *)((BYTE *)this - offsetof(CXnNic, _XbdmServer)));
}

void CXnNic::CXbdmServer::NicStop()
{
    Assert(GetXnNic()->_pXbdmClient == NULL);
    KIRQL irql = ::KeRaiseIrqlToDpcLevel();
    GetXnNic()->NicStop();
    KeGetCurrentPrcb()->DmEnetFunc = NULL;
    ::KeLowerIrql(irql);
}

NTSTATUS CXnNic::CXbdmServer::InitClient(UINT cfgRecvQ, UINT cfgXmitQ, CEnetAddr * pea, BOOL * pfLinkIsUp)
{
    Assert(GetXnNic()->_pXbdmClient == NULL);
    *pea = GetXnNic()->_eaClient;
    *pfLinkIsUp = GetXnNic()->TestInitFlag(INITF_CONNECTED_BOOT);
    return(NETERR_OK);
}

void CXnNic::CXbdmServer::AttachClient(CXbdmClient * pXbdmClient)
{
    Assert(GetXnNic()->_pXbdmClient == NULL);
    KIRQL irql = ::KeRaiseIrqlToDpcLevel();
    GetXnNic()->_pXbdmClient = pXbdmClient;
    ::KeLowerIrql(irql);
}

void CXnNic::CXbdmServer::DetachClient()
{
    Assert(KeGetCurrentIrql() == DISPATCH_LEVEL);

    CXnNic * pXnNic = GetXnNic();
    PNICCSR pCsr = PNicCsr();
    UINT cTimeout = 0;

    while (!pXnNic->_pqXmit.IsEmpty())
    {
        pCsr->mode = MODE_TXDM;
        pXnNic->NicXmitInterrupt();
        KeStallExecutionProcessor(50);
        cTimeout++;
        AssertSz(cTimeout != 20000, "Taking too long to flush transmit queue");
    }

    pXnNic->_pXbdmClient = NULL;
}

void CXnNic::CXbdmServer::Xmit(void * pvPkt, void * pv, UINT cb)
{
    CXnNic * pXnNic = GetXnNic();
    Assert(pXnNic->_pXbdmClient != NULL);
    Assert(!pXnNic->_pqClient.IsEmpty());
    CXbdmPacket * ppktXbdm = (CXbdmPacket *)pXnNic->_pqClient.RemoveHead();
    ppktXbdm->Init(PKTF_XMIT_XBDMCLIENT, (BYTE *)pv + sizeof(CEnetHdr), cb - sizeof(CEnetHdr), NULL);
    ppktXbdm->_pvPkt = pvPkt;
    pXnNic->NicXmit(ppktXbdm);
}

BOOL CXnNic::CXbdmServer::XmitReady()
{
    Assert(GetXnNic()->_pXbdmClient != NULL);
    BOOL fReady = GetXnNic()->NicXmitReady();
    return(fReady);
}

DWORD CXnNic::CXbdmServer::GetXnAddr(XNADDRXBDM * pxnaXbdm)
{
    XNADDR xnaddr;
    DWORD dwFlags = ((CXnIp *)GetXnNic())->IpGetXnAddr(&xnaddr);
    memcpy(pxnaXbdm->abEnet, xnaddr.abEnet, sizeof(pxnaXbdm->abEnet));
    pxnaXbdm->ina = xnaddr.ina;
    return(dwFlags);
}

DWORD CXnNic::NicGetOtherXnAddr(XNADDR * pxna)
{
    XNADDRXBDM xnaddrXbdm = { 0 };
    DWORD dwFlags = _pXbdmClient ? _pXbdmClient->GetXnAddr(&xnaddrXbdm) : XNET_GET_XNADDR_NONE;
    memset(pxna, 0, sizeof(*pxna));
    memcpy(pxna->abEnet, xnaddrXbdm.abEnet, sizeof(pxna->abEnet));
    pxna->ina = xnaddrXbdm.ina;
    return(dwFlags);
}

#endif

// ---------------------------------------------------------------------------------------

#endif

