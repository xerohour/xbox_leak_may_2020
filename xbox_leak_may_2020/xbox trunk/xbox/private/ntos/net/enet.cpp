// ---------------------------------------------------------------------------------------
// enet.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(Arp,          0);
DefineTag(ArpWarn,      TAG_ENABLE);

// ---------------------------------------------------------------------------------------
// CXnEnet - External Functions
// ---------------------------------------------------------------------------------------

NTSTATUS CXnEnet::EnetInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    NTSTATUS status = NicInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

    SetInitFlag(INITF_ENET);

    KeInitializeDpc(&_dpcEnet, &EnetDpc, this);

#ifdef XNET_FEATURE_ARP
    _paeLast = _aae;
    _timerArp.Init((PFNTIMER)ArpTimer);
#endif

    return(NETERR_OK);
}

void CXnEnet::EnetTerm()
{
    TCHECK(UDPC);

    EnetStop();

    if (TestInitFlag(INITF_ENET))
    {
        KeRemoveQueueDpc(&_dpcEnet);

        if (!_pqXmit.IsEmpty())
        {
            TraceSz1(Warning, "Enet shutdown with %d packet(s) queued for transmit", _pqXmit.Count());
            _pqXmit.Discard(this);
        }

#ifdef XNET_FEATURE_ARP

        CArpEntry * pae = _aae;
        UINT        cae = dimensionof(_aae);

        for (; cae > 0; --cae, ++pae)
        {
            if (!pae->_pqWait.IsEmpty())
            {
                TraceSz2(Warning, "Enet shutdown with %d packet(s) queued for ARP %s",
                         pae->_pqWait.Count(), pae->_ipa.Str());
                pae->_pqWait.Discard(this);
            }
        }

        TimerSet(&_timerArp, TIMER_INFINITE);

#endif
    }

    SetInitFlag(INITF_ENET_TERM);

    NicTerm();
}

void CXnEnet::EnetXmit(CPacket * ppkt, CIpAddr ipaNext)
{
    ICHECK(ENET, UDPC|SDPC);

    if (!ppkt->TestFlags(PKTF_XMIT_FRAME))
    {
        Assert(ppkt->IsIp());
        *((CIpAddr *)ppkt->GetPv() - 1) = ipaNext;
    }

    if (ppkt->TestFlags(PKTF_XMIT_PRIORITY))
        _pqXmit.InsertHead(ppkt);
    else
        _pqXmit.InsertTail(ppkt);

    ppkt->ClearFlags(PKTF_XMIT_PRIORITY);

    EnetQueuePush();
}

#ifdef XNET_FEATURE_ARP

void CXnEnet::EnetXmitArp(CIpAddr ipa)
{
    ICHECK(ENET, UDPC|SDPC);

    _ipaCheck = ipa;

    if (_ipaCheck)
    {
        ArpXmit(ARP_OP_REQUEST, _ipaCheck, _ipaCheck, NULL);
    }
}

#endif

void CXnEnet::EnetStop()
{
    TCHECK(UDPC);

    if (TestInitFlag(INITF_ENET) && !TestInitFlag(INITF_ENET_STOP))
    {
        SetInitFlag(INITF_ENET_STOP);
    }

    NicStop();
}

// ---------------------------------------------------------------------------------------
// Virtual callbacks
// ---------------------------------------------------------------------------------------

void CXnEnet::EnetRecv(CPacket * ppkt, UINT uiType)
{
    ICHECK(ENET, UDPC|SDPC);

    Assert(ppkt->IsEnet());

    if (uiType == ENET_TYPE_IP)
    {
        ppkt->SetType(PKTF_TYPE_IP);
        IpRecv(ppkt);
    }
#ifdef XNET_FEATURE_ARP
    else if (uiType == ENET_TYPE_ARP)
    {
        ArpRecv(ppkt);
    }
#endif
    else
    {
        TraceSz1(pktRecv, "[DISCARD] No support for Ethernet type %04X", NTOHS((WORD)uiType));
    }
}

// ---------------------------------------------------------------------------------------
// CXnEnet - Internal Functions
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ARP

void CXnEnet::ArpXmit(WORD wOp, CIpAddr ipaTarget, CIpAddr ipaSender, CEnetAddr * peaTarget)
{
    ICHECK(ENET, UDPC|SDPC);

    CPacket *   ppkt;
    CEnetHdr *  pEnetHdr;
    CArpMsg *   pArpMsg;
    
    ppkt = PacketAlloc(PTAG_CArpPacket,
                       PKTF_POOLALLOC|PKTF_TYPE_ENET|PKTF_XMIT_FRAME|PKTF_XMIT_PRIORITY,
                       sizeof(CArpMsg));
    if (ppkt == NULL)
    {
        TraceSz(Warning, "Out of memory allocating ARP packet");
        return;
    }

    pEnetHdr = ppkt->GetEnetHdr();
    pArpMsg = (CArpMsg *)ppkt->GetPv();

    if (peaTarget)
    {
        pEnetHdr->_eaDst = *peaTarget;
        pArpMsg->_eaTarget = *peaTarget;
    }
    else
    {
        pEnetHdr->_eaDst.SetBroadcast();
        pArpMsg->_eaTarget.SetZero();
    }

    pEnetHdr->_eaSrc    = _ea;
    pEnetHdr->_wType    = ENET_TYPE_ARP;
    pArpMsg->_wHrd      = ARP_HWTYPE_ENET;
    pArpMsg->_wPro      = ENET_TYPE_IP;
    pArpMsg->_bHln      = sizeof(CEnetAddr);
    pArpMsg->_bPln      = sizeof(CIpAddr);
    pArpMsg->_wOp       = wOp;
    pArpMsg->_eaSender  = _ea;
    pArpMsg->_ipaSender = ipaSender;
    pArpMsg->_ipaTarget = ipaTarget;

    TraceSz5(Arp, "%s (%s) is %s %s (%s)",
             ipaSender.Str(), _ea.Str(), peaTarget ? "replying to" : "broadcasting request for",
             ipaTarget.Str(), pArpMsg->_eaTarget.Str());

    EnetXmit(ppkt);
}

CXnEnet::CArpEntry * CXnEnet::ArpLookup(CIpAddr ipa, ArpResolve eResolve)
{
    ICHECK(ENET, UDPC|SDPC);
    Assert(ipa.IsValidUnicast());

    CArpEntry * pae;
    CArpEntry * paeRetryEnd;
    CArpEntry * paeHash;
    UINT        uiHash;

    Assert(ipa != 0);

    if (_paeLast->_ipa == ipa)
    {
        Assert(!_paeLast->IsFree());
        return(_paeLast);
    }

    // Get the hash bucket for the specified address

    uiHash = ARP_HASH(ipa);
    pae    = &_aae[uiHash];

    // Found the target address in the cache via a direct hash hit

    if (pae->_ipa == ipa)
    {
        goto found;
    }

    // No direct hash hit, try linear search

    paeHash     = pae;
    paeRetryEnd = pae + ARP_HASH_RETRY;

    while (++pae < paeRetryEnd)
    {
        if (pae->_ipa == ipa)
            goto found;
    }

    if (eResolve == eNone)
    {
        return(NULL);
    }

    // The target IP address is not in the cache:
    //  send out an ARP request if specified;
    //  and make a new cache entry for the target
    
    // Check to see if the hash bucket is free

    pae = paeHash;

    if (!pae->IsFree())
    {
        while (++pae < paeRetryEnd)
        {
            if (pae->IsFree())
                break;
        }

        // Couldn't find a free entry
        //  fall back and try to find a non-busy entry

        if (pae == paeRetryEnd)
        {
            pae = paeHash;

            while (++pae < paeRetryEnd)
            {
                if (!pae->IsBusy())
                    break;
            }

            if (pae == paeRetryEnd)
            {
                // Too bad: couldn't find either a free or non-busy entry
                //  emit a warning and give up

                return(NULL);
            }

            // This entry is currently used for another address:
            // we'll just bump it off here.

            TraceSz1(Arp, "Bumped entry for %s", pae->_ipa.Str());
        }
    }

    TraceSz1(Arp, "Adding entry for %s", ipa.Str());

    pae->_ipa = ipa;
    Assert(pae->_pqWait.IsEmpty());

    if (eResolve == eSendRequest)
    {
        pae->_wState = ARP_STATE_BUSY + cfgEnetArpReqRetries;
        pae->_dwTick = TimerSetRelative(&_timerArp, cfgEnetArpRexmitTimeoutInSeconds * TICKS_PER_SECOND);
        ArpXmit(ARP_OP_REQUEST, ipa, _ipa, NULL);
    }
    else
    {
        // Remaining initialization of this entry happens in the caller which
        // changes the state to ARP_STATE_IDLE
        pae->_wState = ARP_STATE_BUSY;
    }

found:
    Assert(pae && !pae->IsFree());
    _paeLast = pae;
    return(pae);
}

void CXnEnet::ArpTimer(CTimer * pt)
{
    ICHECK(ENET, UDPC|SDPC);

    CArpEntry * pae;
    UINT        iae;
    DWORD       dwTickNow = TimerTick();
    DWORD       dwTickArp = TIMER_INFINITE;

    for (iae = 0, pae = _aae; iae < ARP_CACHE_SIZE; ++iae, ++pae)
    {
        if (pae->IsFree())
            continue;

        if (dwTickNow >= pae->_dwTick)
        {
            if (pae->_wState > ARP_STATE_BUSY)
            {
                TraceSz1(ArpWarn, "ArpTimer: %s didn't respond to request.  Trying again.", pae->_ipa.Str());
                pae->_dwTick = dwTickNow + cfgEnetArpRexmitTimeoutInSeconds * TICKS_PER_SECOND;
                pae->_wState -= 1;
                ArpXmit(ARP_OP_REQUEST, pae->_ipa, _ipa, NULL);
            }
            else if (pae->_wState == ARP_STATE_BUSY)
            {
                TraceSz3(ArpWarn, "ArpTimer: %s is unreachable.  Discarding %d waiting packet%s.",
                         pae->_ipa.Str(), pae->_pqWait.Count(), pae->_pqWait.Count() == 1 ? "" : "s");
                pae->_pqWait.Complete(this);
                pae->_dwTick = dwTickNow + (cfgEnetArpNegCacheTimeoutInMinutes * 60 * TICKS_PER_SECOND);
                pae->_wState = ARP_STATE_BAD;
            }
            else
            {
                TraceSz1(Arp, "%s has timed out", pae->_ipa.Str());
                pae->_dwTick = TIMER_INFINITE;
                pae->_wState = ARP_STATE_FREE;
                pae->_ipa    = 0;
            }
        }

        if (dwTickArp > pae->_dwTick)
            dwTickArp = pae->_dwTick;
    }

    Assert(pt == &_timerArp);
    TimerSet(pt, dwTickArp);
}

void CXnEnet::ArpRecv(CPacket * ppkt)
{
    ICHECK(ENET, UDPC|SDPC);

    Assert(ppkt->IsEnet());

    CEnetHdr *  pEnetHdr = ppkt->GetEnetHdr();
    CArpMsg *   pArpMsg  = (CArpMsg *)ppkt->GetPv();
    UINT        cbMsg    = ppkt->GetCb();
    WORD        wOp;
    CIpAddr     ipaSender;
    CEnetAddr   eaSender;
    CIpAddr     ipaTarget;
    CArpEntry * pae;
    ArpResolve  eResolve;

    TraceSz5(pktRecv, "[ARP %s %s %s %s %s]",
             pArpMsg->_wOp == ARP_OP_REQUEST ? "Request" :
             pArpMsg->_wOp == ARP_OP_REPLY ? "Reply" : "???",
             pArpMsg->_ipaSender.Str(), pArpMsg->_eaSender.Str(),
             pArpMsg->_ipaTarget.Str(), pArpMsg->_eaTarget.Str());

    if (_ea.IsEqual(pEnetHdr->_eaSrc))
    {
        // We received an ARP packet from someone who has the same Ethernet address
        // as us.  Issue a warning and discard the packet.  There's not a lot we
        // can do at this point.

        TraceSz(pktWarn, "[DISCARD] ARP sender has conflicting Ethernet address!");
        return;
    }

    if (cbMsg < sizeof(CArpMsg))
    {
        TraceSz1(pktRecv, "[DISCARD] ARP packet is too small (%d)", cbMsg);
        return;
    }

    wOp       = pArpMsg->_wOp;
    ipaSender = pArpMsg->_ipaSender;
    eaSender  = pArpMsg->_eaSender;
    ipaTarget = pArpMsg->_ipaTarget;

    if (    pArpMsg->_wHrd != ARP_HWTYPE_ENET && pArpMsg->_wHrd != ARP_HWTYPE_802
        ||  pArpMsg->_wPro != ENET_TYPE_IP
        ||  pArpMsg->_bHln != sizeof(CEnetAddr)
        ||  pArpMsg->_bPln != sizeof(CIpAddr)
        ||  wOp != ARP_OP_REQUEST && wOp != ARP_OP_REPLY
        ||  !ipaSender.IsValidUnicast()
        ||  !ipaTarget.IsValidUnicast()
        ||  eaSender.IsMulticast())
    {
        TraceSz9(pktRecv, "[DISCARD] ARP message is invalid (%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                 pArpMsg->_wHrd != ARP_HWTYPE_ENET && pArpMsg->_wHrd != ARP_HWTYPE_802,
                 pArpMsg->_wPro != ENET_TYPE_IP,
                 pArpMsg->_bHln != sizeof(CEnetAddr),
                 pArpMsg->_bPln != sizeof(CIpAddr),
                 wOp != ARP_OP_REQUEST && wOp != ARP_OP_REPLY,
                 !ipaSender.IsValidUnicast(),
                 !ipaTarget.IsValidUnicast(),
                 eaSender.IsMulticast(),
                 eaSender.IsBroadcast());
        return;
    }

    // Check to see if we have an existing entry for the sender
    // in our ARP cache. If we're the target and there is no
    // existing entry, then we'll create a new entry.
    // This assumes that communication will likely be bidirectional.

    if (ipaSender == _ipaCheck)
    {
        TraceSz(Arp, "Auto-IP collision detected");
        _ipaCheck = 0;
        IpRecvArp(&eaSender);
    }

    // If we don't have an IP address yet, there is nothing more to do

    if (_ipa == 0)
        return;

    if (ipaSender != _ipa)
    {
        eResolve = (ipaTarget == _ipa) ? eCreateEntry : eNone;

        pae = ArpLookup(ipaSender, eResolve);

        if (pae)
        {
            if (pae->_wState != ARP_STATE_IDLE)
            {
                TraceSz2(Arp, "%s resolved to %s", pae->_ipa.Str(), eaSender.Str());
            }

            pae->_wState = ARP_STATE_IDLE;
            pae->_dwTick = TimerSetRelative(&_timerArp, cfgEnetArpPosCacheTimeoutInMinutes * 60 * TICKS_PER_SECOND);
            pae->_eaNext = eaSender;

            if (!pae->_pqWait.IsEmpty())
            {
                TraceSz3(Arp, "Sending %d packet%s waiting for %s", pae->_pqWait.Count(),
                         pae->_pqWait.Count() == 1 ? "" : "s", pae->_ipa.Str());

                _pqXmit.InsertHead(&pae->_pqWait);

                EnetQueuePush();
            }
        }
    }
    
    // If we're the target and the packet is an ARP request,
    // then send out an ARP reply.

    if (ipaTarget == _ipa && wOp == ARP_OP_REQUEST)
    {
        TraceSz(pktRecv, "[REPLY] Replying to ARP request");
        ArpXmit(ARP_OP_REPLY, ipaSender, _ipa, &eaSender);
    }
    else if (wOp == ARP_OP_REQUEST)
    {
        TraceSz(pktRecv, "[DISCARD] ARP request not for me");
        return;
    }
    else
    {
        TraceSz(pktRecv, "[ARPDONE] ARP reply processed");
        return;
    }
}

#endif

// ---------------------------------------------------------------------------------------
// EnetPush
// ---------------------------------------------------------------------------------------

void CXnEnet::EnetPush()
{
    ICHECK(ENET, UDPC|SDPC);

    CPacket *   ppkt;
    CIpAddr     ipaNext;

#ifdef XNET_FEATURE_ARP
    CArpEntry * pae;
#endif

    while (!_pqXmit.IsEmpty() && NicXmitReady())
    {
        ppkt = _pqXmit.RemoveHead();

        if (TestInitFlag(INITF_ENET_STOP))
        {
            TraceSz(pktWarn, "[DISCARD] Network is down");
            goto complete;
        }

        if (ppkt->TestFlags(PKTF_XMIT_FRAME))
        {
            ppkt->ClearFlags(PKTF_XMIT_FRAME);
            NicXmit(ppkt);
            continue;
        }

        Assert(ppkt->IsIp());

        ipaNext = *((CIpAddr *)ppkt->GetPv() - 1);

        if (ipaNext == 0)
        {
            // ipaNext set to zero from the caller means that this packet should be discarded
            // silently.  The purpose is to prevent calling back to the sender in the middle
            // of transmitting the packet to break potential recursion.
            goto complete;
        }

        if (ipaNext.IsBroadcast())
        {
            CEnetAddr eaNext;
            eaNext.SetBroadcast();
            EnetFillAndXmit(ppkt, &eaNext);
            continue;
        }

        if (ipaNext.IsLoopback() && ipaNext != IPADDR_LOOPBACK)
        {
            TraceSz1(pktWarn, "[DISCARD] Can't send to %s", ipaNext.Str());
            goto complete;
        }

        if (ipaNext == IPADDR_LOOPBACK || ipaNext == _ipa)
        {
            TraceSz1(pktRecv, "\n[LOOPBACK][%d]", ppkt->GetCb());
            ppkt->SetFlags(PKTF_RECV_LOOPBACK);
            IpRecv(ppkt);
            ppkt->ClearFlags(PKTF_RECV_LOOPBACK);
            ppkt->Complete(this);
            continue;
        }

#ifdef XNET_FEATURE_ARP

        pae = ArpLookup(ipaNext, eSendRequest);

        if (pae == NULL)
        {
            TraceSz(pktWarn, "[DISCARD] ARP cache is full");
            goto complete;
        }

        if (pae->IsIdle())
        {
            EnetFillAndXmit(ppkt, &pae->_eaNext);
            continue;
        }

        if (pae->IsBad())
        {
            TraceSz1(pktWarn, "[DISCARD] %s is unreachable", pae->_ipa.Str());
            goto complete;
        }

        Assert(pae->IsBusy());
        pae->_pqWait.InsertTail(ppkt);
        continue;

#else
        TraceSz1(pktWarn, "[DISCARD] %s is unreachable", ipaNext);
        goto complete;
#endif

complete:
        ppkt->Complete(this);
    }
}

void CXnEnet::EnetFillAndXmit(CPacket * ppkt, CEnetAddr * peaNext)
{
    ICHECK(ENET, UDPC|SDPC);
    Assert(ppkt->IsIp());
    Assert(peaNext);

    CEnetHdr * pEnetHdr = ppkt->GetEnetHdr();
    pEnetHdr->_eaDst = *peaNext;
    pEnetHdr->_eaSrc = _ea;
    pEnetHdr->_wType = ENET_TYPE_IP;

    NicXmit(ppkt);
}

void CXnEnet::EnetQueuePush()
{
    ICHECK(ENET, UDPC|SDPC);
    KeInsertQueueDpc(&_dpcEnet, NULL, NULL);
}

void CXnEnet::EnetDpc(PKDPC, void * pEnet, void *, void *)
{
    ((CXnEnet *)pEnet)->EnetPush();
}
