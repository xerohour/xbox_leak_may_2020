// ----------------------------------------------------------------------------
// nicw.cpp
//
// Copyright (C) Microsoft Corporation
// ----------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#ifdef XNET_FEATURE_WINDOWS

#include <vlan.h>

// ----------------------------------------------------------------------------
// CXnNic - External
// ----------------------------------------------------------------------------

NTSTATUS CXnNic::NicInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    NTSTATUS status = BaseInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

    SetInitFlag(INITF_NIC_1);

    RecvDesc *  prd;
    BYTE *      pb;
    LONG        cb;

    prd = (RecvDesc *)SysAllocZ(cfgEnetReceiveQueueLength * sizeof(RecvDesc), PTAG_CXnNic);

    if (prd == NULL)
    {
        TraceSz(Warning, "NicInit: Unable to allocate memory");
        return(NETERR_MEMORY);
    }

    KeInitializeDpc(&_dpcRecv, DpcCallback, this);

    _cxdPool   = cfgEnetTransmitQueueLength;
    _crdPool   = cfgEnetReceiveQueueLength;
    _prdFirst  = prd;
    _prdRead   = prd;
    _prdWrite  = prd;
    _prdLast   = prd + cfgEnetReceiveQueueLength - 1;

    SetInitFlag(INITF_NIC|INITF_CONNECTED_BOOT);

    return(NETERR_OK);
}

INT CXnNic::NicVLanAttach()
{
    if (!_fVLanInit)
    {
        if (!VLanInit())
        {
            TraceSz(Warning, "NicVLanAttach: VLanInit failed");
            return(WSASYSCALLFAILURE);
        }

        _fVLanInit = TRUE;
    }

    Assert(!_fVLanAttach);

    if (!VLanAttach(_achXbox, _ea._ab, this))
    {
        TraceSz1(Warning, "NicVLanAttach: VLanAttach '%s' failed", _achXbox);
        return(WSASYSCALLFAILURE);
    }

    _fVLanAttach = TRUE;

    return(0);
}

void CXnNic::NicVLanDetach(BOOL fTerm)
{
    if (_fVLanAttach)
    {
        VLanDetach(_ea._ab);
        _fVLanAttach = FALSE;
    }

    if (fTerm && _fVLanInit)
    {
        VLanTerm();
        _fVLanInit = FALSE;
    }
}

INT CXnNic::NicConfig(const XNetConfigParams * pxncp)
{
    return(NicVLanAttach());
}

void CXnNic::NicStart()
{
    ICHECK(NIC, USER);

    BaseStart();

    Assert(!TestInitFlag(INITF_NIC_STOP));

    SetInterrupts(TRUE);
}

void CXnNic::NicFlush()
{
    if (!TestInitFlag(INITF_NIC) || TestInitFlag(INITF_NIC_STOP))
        return;

    // Wait up to half a second for packets queued for transmit to go

    EnetPush();

    for (UINT cTimeout = 500; cTimeout > 0; --cTimeout)
    {
        XmitPush();

        if (_pqXmit.IsEmpty())
            break;

        Sleep(1);
    }
}

void CXnNic::NicStop()
{
    TCHECK(UDPC);

    if (TestInitFlag(INITF_NIC) && !TestInitFlag(INITF_NIC_STOP))
    {
        SetInterrupts(FALSE);
        KeLowerIrql(PASSIVE_LEVEL);
        NicVLanDetach(FALSE);
        KeRaiseIrqlToDpcLevel();
        SetInitFlag(INITF_NIC_STOP);
    }

    BaseStop();
}

void CXnNic::NicTerm()
{
    TCHECK(UDPC);

    NicStop();

    SetInitFlag(INITF_NIC_TERM);

    NicVLanDetach(TRUE);

    if (TestInitFlag(INITF_NIC_1))
    {
        Assert(_dpcRecv.DpcListEntry.Flink == NULL);

        while (_cxdBusy > 0)
        {
            Assert((UINT)_pqXmit.Count() == (UINT)_cxdBusy);
            TraceSz1(Warning, "Nic shutdown with %d packet(s) queued for transmit", _cxdBusy);
            _pqXmit.Discard(this);
        }

        if (_prdFirst)
        {
            SysFree(_prdFirst);
        }
    }

    BaseTerm();
}

BOOL CXnNic::NicXmitReady()
{
    ICHECK(NIC, UDPC|SDPC);

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

    Assert(_cxdBusy < _cxdPool);
    Assert(ppkt->GetCb() <= ENET_DATA_MAXSIZE);

    _pqXmit.InsertTail(ppkt);
    _cxdBusy += 1;

    XmitPush();
}

// ---------------------------------------------------------------------------------------
// CXnNic - Internal
// ---------------------------------------------------------------------------------------

void CXnNic::XmitPush()
{
    ICHECK(NIC, UDPC|SDPC);

    if (!_fIntr)
    {
        TraceSz(Warning, "CXnNic::XmitPush - Interrupts are disabled");
        return;
    }

    while (_cxdBusy > 0)
    {
        CPacket * ppkt = _pqXmit.GetHead();

        if (!VLanXmit((BYTE *)ppkt->GetEnetHdr(), sizeof(CEnetHdr) + ppkt->GetCb()))
        {
            TraceSz(Warning, "CXnNic::XmitPush - VLanXmit failed");
        }

        _pqXmit.RemoveHead();
        _cxdBusy -= 1;
        ppkt->Complete(this);
    }

    if (_fXmitFull && _cxdBusy < _cxdPool)
    {
        _fXmitFull = FALSE;
        EnetPush();
    }
}

void CXnNic::NicRecvFrame(void * pv, DWORD cbDat)
{
    ICHECK(NIC, USER|UDPC|SDPC);

    Assert(cbDat >= sizeof(CEnetHdr));
    Assert(cbDat <= sizeof(CEnetHdr) + ENET_DATA_MAXSIZE);

    if (_crdBusy == _crdPool)
    {
        TraceSz(Warning, "CXnNic::NicRecvFrame - Receive queue is full");
    }
    else if (cbDat <  sizeof(CEnetHdr) || cbDat > sizeof(CIeeeHdr) + ENET_DATA_MAXSIZE)
    {
        TraceSz1(Warning, "CXnNic::NicRecvFrame - Invalid frame size (%ld bytes)", cbDat);
    }
    else
    {
        HalEnterDpc();

        if (_fIntr)
        {
            RecvDesc * prd = _prdWrite;
            _prdWrite = NextPrd(prd);
            prd->cbDat = cbDat;
            memcpy(prd->rgbBuf, pv, cbDat);
            _crdBusy += 1;

            KeInsertQueueDpc(&_dpcRecv, NULL, NULL);
        }

        HalLeaveDpc();
    }
}

void CXnNic::RecvPush()
{
    ICHECK(NIC, UDPC|SDPC);

    if (!_fIntr)
    {
        TraceSz(Warning, "CXnNic::RecvPush - Interrupts are disabled");
        return;
    }

    while (_crdBusy > 0)
    {
        CPacket     pkt;
        RecvDesc *  prd      = _prdRead;
        void *      pv       = prd->rgbBuf;
        UINT        cb       = prd->cbDat;
        CEnetHdr *  pEnetHdr = (CEnetHdr *)pv;
        UINT        uiFlags  = PKTF_TYPE_ENET;
        UINT        uiType   = pEnetHdr->_wType;

        PushPktRecvTags(pEnetHdr->_eaDst.IsBroadcast());

        if (NTOHS((WORD)uiType) <= ENET_DATA_MAXSIZE)
        {
            CIeeeHdr * pIeeeHdr = (CIeeeHdr *)pEnetHdr;
            BYTE abEnetHdr[sizeof(CEnetAddr) * 2];

            if (!pIeeeHdr->IsEnetFrame())
            {
                TraceSz(pktRecv, "[DISCARD] IEEE frame type not supported");
                goto nextframe;
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
            TraceSz(pktWarn, "[DISCARD] Frame should not have been accepted by Nic hardware");
            goto nextframe;
        }

#ifdef XNET_FEATURE_VMEM
        if (VMemIsEnabled())
        {
            void * pvNew = VMemAlloc(cb + sizeof(CEnetHdr));
            Assert(pvNew != NULL);
            memcpy(pvNew, (BYTE *)pv - sizeof(CEnetHdr), cb + sizeof(CEnetHdr));
            pv = (BYTE *)pvNew + sizeof(CEnetHdr);
        }
#endif

        pkt.Init(uiFlags, pv, cb, NULL);
        EnetRecv(&pkt, uiType);

#ifdef XNET_FEATURE_VMEM

        if (VMemIsEnabled())
        {
            VMemFree((BYTE *)pv - sizeof(CEnetHdr));
        }

#endif

    nextframe:
        
        _prdRead  = NextPrd(prd);
        _crdBusy -= 1;

        PopPktRecvTags();
    }
}

void CXnNic::DpcCallback(PRKDPC, void * pthis, void *, void *)
{
    ((CXnNic *)pthis)->RecvPush();
}

void CXnNic::SetInterrupts(BOOL fEnable)
{
    ICHECK(NIC, USER|UDPC|SDPC);

    RaiseToDpc();

    if (!!fEnable != !!_fIntr)
    {
        _fIntr = fEnable;

        if (fEnable)
        {   
            if (_crdBusy > 0)
            {
                KeInsertQueueDpc(&_dpcRecv, NULL, NULL);
            }
        }
        else
        {
            KeRemoveQueueDpc(&_dpcRecv);
        }
    }
}

CXnNic::RecvDesc * CXnNic::NextPrd(RecvDesc * prd)
{
    TCHECK(USER|UDPC|SDPC);
    return((prd == _prdLast) ? _prdFirst : prd + 1);
}

// ---------------------------------------------------------------------------------------
// VLanRecv
// ---------------------------------------------------------------------------------------

void WINAPI VLanRecv(BYTE * pb, UINT cb, void * pvArg)
{
    ((CXnNic *)pvArg)->NicRecvFrame(pb, cb);
}

#endif
