// ---------------------------------------------------------------------------------------
// ip.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(secStat, 0);
DefineTag(tcpRetrans, 0);
DefineTag(pktPreAuth, 0);
DefineTag(keyExDrop, 0);

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#define ESPUDP_REPLAY_WINDOW        1024                // How far ahead sender can get

// ---------------------------------------------------------------------------------------
// CXnIp External
// ---------------------------------------------------------------------------------------

NTSTATUS CXnIp::IpInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    NTSTATUS status = EnetInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

    SetInitFlag(INITF_IP);

    status = RouteInit();
    if (!NT_SUCCESS(status))
        return(status);

    status = DhcpInit();
    if (!NT_SUCCESS(status))
        return(status);

    _pKeyReg = (CKeyReg *)SysAllocZ(cfgKeyRegMax * sizeof(CKeyReg), PTAG_CKeyReg);

    if (_pKeyReg == NULL)
    {
        TraceSz(Warning, "IpInit - Out of memory allocating CKeyReg vector");
        return(WSAENOBUFS);
    }

    _pSecReg = (CSecReg *)SysAllocZ(cfgSecRegMax * sizeof(CSecReg), PTAG_CSecReg);

    if (_pSecReg == NULL)
    {
        TraceSz(Warning, "IpInit - Out of memory allocating CSecReg vector");
        return(WSAENOBUFS);
    }

#if defined(XNET_FEATURE_XBOX) && !defined(XNET_FEATURE_XBDM_SERVER)

    // Generate the SHA and 3DES keys for LAN broadcast.  We do this by concatenating two digests:
    //
    //      HMAC(HMAC(ROM-LAN-KEY, CERT-LAN-KEY), 0 | CERT-LAN-KEY) |
    //      HMAC(HMAC(ROM-LAN-KEY, CERT-LAN-KEY), 1 | CERT-LAN-KEY)
    //
    // This produces 40 bytes of digest.  The first 16 bytes are used as a SHA key, and the
    // remaining 24 bytes as the 3DES key.

    BYTE    abCert[1 + XBEIMAGE_CERTIFICATE_KEY_LENGTH];
    BYTE    abHash[XC_SERVICE_DIGEST_SIZE * 2];

    memcpy(&abCert[1], XeImageHeader()->Certificate->LANKey, XBEIMAGE_CERTIFICATE_KEY_LENGTH);

    abCert[0] = 0;
    XcHMAC((BYTE *)*XboxLANKey, XBOX_KEY_LENGTH, abCert, sizeof(abCert), NULL, 0, &abHash[0]);
    abCert[0] = 1;
    XcHMAC((BYTE *)*XboxLANKey, XBOX_KEY_LENGTH, abCert, sizeof(abCert), NULL, 0, &abHash[XC_SERVICE_DIGEST_SIZE]);

    Assert(sizeof(abHash) == sizeof(_abKeyShaLan) + sizeof(_abKeyDesLan));
    memcpy(_abKeyShaLan, &abHash[0], sizeof(_abKeyShaLan));
    memcpy(_abKeyDesLan, &abHash[sizeof(_abKeyShaLan)], sizeof(_abKeyDesLan));
    XcDESKeyParity(_abKeyDesLan, sizeof(_abKeyDesLan));

#endif

    Rand((BYTE *)&_lNextDgramId, sizeof(_lNextDgramId));
    Rand((BYTE *)&_wSecRegUniq, sizeof(_wSecRegUniq));
    KeQuerySystemTime(&_liTime);

    _cSecRegProbeDenom = cfgSecRegVisitInSeconds * TICKS_PER_SECOND;

#if defined(XNET_FEATURE_SG) && defined(XNET_FEATURE_INSECURE)
    Rand(_abDhXNull, sizeof(_abDhXNull));
    XcModExp((DWORD *)_abDhGXNull, (DWORD *)g_abOakleyGroup1Base,
             (DWORD *)_abDhXNull, (DWORD *)g_abOakleyGroup1Mod, CBDHG1 / sizeof(DWORD));
#endif

    return(NETERR_OK);
}

void CXnIp::IpStop()
{
    TCHECK(UDPC);

    if (TestInitFlag(INITF_IP) && !TestInitFlag(INITF_IP_STOP))
    {
        SecRegShutdown(FALSE);
        NicFlush();
        SetInitFlag(INITF_IP_STOP);
    }

    EnetStop();
}

void CXnIp::IpTerm()
{
    TCHECK(UDPC);

    IpStop();

    if (TestInitFlag(INITF_IP))
    {
        DhcpTerm();
        RouteTerm();
        FragTerm();

        if (_pKeyReg)
        {
            CKeyReg * pKeyReg = &_pKeyReg[_cKeyReg - 1];
            UINT      cKeyReg = _cKeyReg;

            for (; cKeyReg > 0; --pKeyReg, --cKeyReg)
            {
                TraceSz1(Warning, "IpTerm - XNKID %s was not unregistered before shutdown",
                         HexStr(pKeyReg->_xnkid.ab, sizeof(pKeyReg->_xnkid.ab)));
                IpUnregisterKey(&pKeyReg->_xnkid);
            }

            SysFree(_pKeyReg);
        }

        if (_pSecReg)
        {
            CSecReg * pSecReg = _pSecReg;
            UINT      cSecReg = cfgSecRegMax;

            for (; cSecReg > 0; ++pSecReg, --cSecReg)
            {
                if (pSecReg->_dwSpiRecv)
                {
                    SecRegFree(pSecReg);
                }
            }

            SysFree(_pSecReg);
        }
    }

    SetInitFlag(INITF_IP_TERM);

    EnetTerm();
}

// ---------------------------------------------------------------------------------------
// IpConfig
// ---------------------------------------------------------------------------------------

INT CXnIp::IpConfig(const XNetConfigParams * pxncp, DWORD dwFlags)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    INT err = NicConfig(pxncp);

#ifdef XNET_FEATURE_DHCP
    if (err == 0)
    {
        err = DhcpConfig(pxncp);
    }
#endif

    return(err);
}

INT CXnIp::IpGetConfigStatus(XNetConfigStatus * pxncs)
{
    ICHECK(IP, USER);
    TraceSz(Warning, "IpGetConfigStatus not yet implemented");
    memset(pxncs, 0, sizeof(XNetConfigStatus));
    pxncs->dwFlags = XNET_STATUS_PENDING;
    return(0);
}

// ---------------------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------------------

void CXnIp::IpSetAddress(CIpAddr ipa, CIpAddr ipaMask)
{
    ICHECK(IP, USER|UDPC|SDPC);

    RaiseToDpc();

    if (ipa == 0)
    {
        ipaMask = 0;
    }
    else if (!ipa.IsValidUnicast())
    {
        TraceSz4(Warning, "IpSetAddress - Cannot set IP address to %s (reason %d/%d/%d)",
                 ipa.Str, ipa.IsBroadcast(), ipa.IsMulticast(), ipa.IsLoopback());
        ipa = 0;
        ipaMask = 0;
    }
    else if (ipaMask == 0 || !ipaMask.IsValidMask())
    {
        ipaMask = ipa.DefaultMask();
    }

    _ipa        = ipa;
    _ipaMask    = ipaMask;
    _ipaSubnet  = (ipa & ipaMask);

    if (ipa)
    {
#if DBG
        TraceSz(Warning, "+\n-------------------------------------------------------------------------");
        #ifdef XNET_FEATURE_XBDM_SERVER
        TraceSz3(Warning, "+XBOX DEBUG IP: %s / %s [%s]", ipa.Str(), ipaMask.Str(), _ea.Str());
        #else
        TraceSz3(Warning, "+XBOX TITLE IP: %s / %s [%s]", ipa.Str(), ipaMask.Str(), _ea.Str());
        #endif
        TraceSz(Warning, "+-------------------------------------------------------------------------\n");
#endif

        // Add a route for the local subnet

        RouteAdd(_ipaSubnet, _ipaMask, _ipa, RTEF_LOCAL, RTE_DEFAULT_METRIC);
    }
    else
    {
        // We've lost our subnet so clear the route list

        RouteListOrphan();
    }
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpRecv
// ---------------------------------------------------------------------------------------

void CXnIp::IpRecv(CPacket * ppkt)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(ppkt->IsIp());

    CIpHdr *        pIpHdr;
    CIpAddr         ipaDst;
    CIpAddr         ipaSrc;
    UINT            cbHdrLen;
    UINT            cbLen;

    if (ppkt->GetCb() < sizeof(CIpHdr))
    {
        TraceSz(pktWarn, "[DISCARD] Ethernet frame smaller than IP header");
        return;
    }

    pIpHdr = ppkt->GetIpHdr();
    cbHdrLen = pIpHdr->_bVerHdr;
    cbLen = pIpHdr->GetLen();

    if ((cbHdrLen & 0xF0) != 0x40)
    {
        TraceSz1(pktWarn, "[DISCARD] IP version (%d) is not IPv4", cbHdrLen >> 4);
        return;
    }

    cbHdrLen = (cbHdrLen & 0x0F) << 2;

    if (cbHdrLen < sizeof(CIpHdr) || cbHdrLen > cbLen || cbLen > ppkt->GetCb())
    {
        TraceSz3(pktWarn, "[DISCARD] IP header length is bad (%d,%d,%d)",
                cbHdrLen < sizeof(CIpHdr), cbHdrLen > cbLen, cbLen > ppkt->GetCb());
        return;
    }

    // Change the size of the packet to match the size specified in the IP header.  An
    // ethernet frame, especially a small one, can sometimes be longer than the IP
    // packet length.  We've already checked above that the ethernet frame is at least
    // as big as cbLen.

    ppkt->SetCb(cbLen);

    ipaDst = pIpHdr->_ipaDst;
    ipaSrc = pIpHdr->_ipaSrc;

    if (cbHdrLen > sizeof(CIpHdr))
    {
        TraceSz9(pktRecv, "[IP %s %s (%d %d %04X %04X) %d]{%d}[%d]",
                 ipaDst.Str(), ipaSrc.Str(), pIpHdr->_bTos, pIpHdr->_bTtl,
                 NTOHS(pIpHdr->_wId), NTOHS(pIpHdr->_wFragOff), pIpHdr->_bProtocol,
                 cbHdrLen - sizeof(CIpHdr), cbLen - cbHdrLen);
        TraceSz(pktRecv, "IP header options are not supported.  Continuing as if no options.");
        ppkt->SetHdrOptLen(cbHdrLen - sizeof(CIpHdr));
    }
    else
    {
        TraceSz8(pktRecv, "[IP %s %s (%d %d %04X %04X) %d][%d]",
                 ipaDst.Str(), ipaSrc.Str(), pIpHdr->_bTos, pIpHdr->_bTtl,
                 NTOHS(pIpHdr->_wId), NTOHS(pIpHdr->_wFragOff), pIpHdr->_bProtocol,
                 cbLen - cbHdrLen);
    }

    if (tcpipxsum(0, pIpHdr, cbHdrLen) != 0xffff)
    {
        TraceSz(pktWarn, "[DISCARD] IP header checksum failed");
        return;
    }

    Assert(!_ipa.IsBroadcast());
    Assert(!_ipa.IsMulticast());
    Assert(!_ipa.IsLoopback());

    if (ipaDst == 0)
    {
        TraceSz(pktWarn, "[DISCARD] Destination address is zero");
        return;
    }

    if (ppkt->TestFlags(PKTF_RECV_BROADCAST) && !ipaDst.IsBroadcast())
    {
        TraceSz(pktRecv, "[DISCARD] IP-level unicast via link-level broadcast");
        return;
    }

    if (ipaSrc.IsBroadcast() || (ipaSrc == 0 && !IsGateway(0)))
    {
        TraceSz1(pktRecv, "[DISCARD] Source address is %s", ipaSrc == 0 ? "zero" : "broadcast");
        return;
    }

    if (!ppkt->TestFlags(PKTF_RECV_LOOPBACK))
    {
        if (ipaDst.IsLoopback() || ipaSrc.IsLoopback() || ipaSrc == _ipa)
        {
            TraceSz3(pktRecv, "[DISCARD] IP loopback addresses received via link-level (%d,%d,%d)",
                    ipaDst.IsLoopback(), ipaSrc.IsLoopback(), ipaSrc == _ipa);
            return;
        }

        if (!ppkt->TestFlags(PKTF_RECV_BROADCAST) && !ipaDst.IsBroadcast() && !IsGateway(0))
        {
            if (_ipa != ipaDst && (ipaDst != IPADDR_SECURE_DEFAULT || pIpHdr->_bProtocol != IPPROTOCOL_UDP))
            {
            #if DBG
                BYTE bProtocol = pIpHdr->_bProtocol;
                CUdpHdr * pUdpHdr = (bProtocol == IPPROTOCOL_UDP || bProtocol == IPPROTOCOL_TCP) ? (CUdpHdr *)(pIpHdr + 1) : NULL;
                TraceSz6(pktWarn, "[DISCARD] %s packet from %s:%d to %s:%d but my IP is %s",
                         bProtocol == IPPROTOCOL_UDP ? "UDP" : bProtocol == IPPROTOCOL_TCP ? "TCP " : "IP",
                         ipaSrc.Str(), pUdpHdr ? NTOHS(pUdpHdr->_ipportSrc) : bProtocol,
                         ipaDst.Str(), pUdpHdr ? NTOHS(pUdpHdr->_ipportDst) : bProtocol,
                         _ipa ? _ipa.Str() : "not yet acquired");
            #endif
                return;
            }
        }
    }

    if (pIpHdr->_wFragOff & HTONS(MORE_FRAGMENTS|FRAGOFFSET_MASK))
    {
#ifdef XNET_FEATURE_FRAG
    #ifdef XNET_FEATURE_FRAG_LOOPBACK
        if (ppkt->TestFlags(PKTF_RECV_BROADCAST))
    #else
        if (ppkt->TestFlags(PKTF_RECV_BROADCAST|PKTF_RECV_LOOPBACK))
    #endif
        {
            TraceSz1(pktWarn, "[DISCARD] Fragmented packet received via %s",
                    ppkt->TestFlags(PKTF_RECV_BROADCAST) ? "broadcast" : "loopback");
            return;
        }
        else
        {
            FragRecv(ppkt, pIpHdr, cbHdrLen, cbLen);
            return;
        }
#else
        TraceSz(pktWarn, "[DISCARD] No support for fragmented packets");
        return;
#endif
    }

    BYTE * pb = (BYTE *)pIpHdr + cbHdrLen;
    cbLen -= cbHdrLen;

    if (pIpHdr->_bProtocol == IPPROTOCOL_UDP)
    {
        ppkt->SetType(PKTF_TYPE_UDP);
        IpRecvUdp(ppkt, pIpHdr, (CUdpHdr *)pb, cbLen);
        return;
    }

    if (pIpHdr->_bProtocol == IPPROTOCOL_TCP)
    {
        ppkt->SetType(PKTF_TYPE_TCP);
        IpRecvTcp(ppkt, pIpHdr, (CTcpHdr *)pb, cbLen);
        return;
    }

    #ifdef XNET_FEATURE_ICMP

    if (pIpHdr->_bProtocol == IPPROTOCOL_ICMP)
    {
        IcmpRecv(ppkt, pIpHdr, pb, cbLen);
        return;
    }

    #endif

    TraceSz1(pktWarn, "[DISCARD] No support for protocol %d", pIpHdr->_bProtocol);
    return;
}

void CXnIp::IpRecvUdp(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(ppkt->IsUdp());

    if (    cbLen < sizeof(CUdpHdr)
        ||  pUdpHdr->GetLen() != cbLen
        ||  pUdpHdr->_ipportDst == 0
        ||  pUdpHdr->_ipportSrc == 0)
    {
        TraceSz4(pktWarn, "[DISCARD] UDP header is invalid (%d,%d,%d,%d)",
                 cbLen < sizeof(CUdpHdr), pUdpHdr->GetLen() != cbLen,
                 pUdpHdr->_ipportDst == 0, pUdpHdr->_ipportSrc == 0);
        return;
    }

    if (!ppkt->IsEsp())
    {
        if (pUdpHdr->_ipportDst == ESPUDP_CLIENT_PORT && !IsGateway(pUdpHdr->_ipportDst))
        {
            IpRecvEsp(ppkt, pIpHdr, (CEspHdr *)pUdpHdr, cbLen);
            return;
        }

        if (pIpHdr->_ipaDst == IPADDR_SECURE_DEFAULT)
        {
            TraceSz5(pktWarn, "[DISCARD] UDP packet from %s:%d to %s:%d but my IP is %s",
                     pIpHdr->_ipaSrc.Str(), NTOHS(pUdpHdr->_ipportSrc),
                     pIpHdr->_ipaDst.Str(), NTOHS(pUdpHdr->_ipportDst),
                     _ipa ? _ipa.Str() : "not yet acquired");
            return;
        }

        if (!ppkt->TestFlags(PKTF_RECV_LOOPBACK))
        {
            if (pUdpHdr->_wChecksum)
            {
                CPseudoHeader ph;
                ph._ipaSrc    = pIpHdr->_ipaSrc;
                ph._ipaDst    = pIpHdr->_ipaDst;
                ph._bZero     = 0;
                ph._bProtocol = IPPROTOCOL_UDP;
                ph._wLen      = pUdpHdr->_wLen;

                Assert(cbLen == NTOHS(ph._wLen));

                if (tcpipxsum(tcpipxsum(0, &ph, sizeof(CPseudoHeader)), pUdpHdr, cbLen) != 0xFFFF)
                {
                    TraceSz(pktWarn, "[DISCARD] UDP header checksum failed");
                    return;
                }
            }

#ifdef XNET_FEATURE_DHCP
            if (pUdpHdr->_ipportDst == DHCP_CLIENT_PORT)
            {
                TraceSz(pktRecv, "[DHCPRECV]");
                DhcpRecv(ppkt, pUdpHdr, cbLen - sizeof(CUdpHdr));
                return;
            }
#endif

#ifdef XNET_FEATURE_DNS
            if (pUdpHdr->_ipportDst == DNS_CLIENT_PORT && !IsGateway(pUdpHdr->_ipportDst))
            {
                TraceSz(pktRecv, "[DNSRECV]");
                IpRecvDns(ppkt, pUdpHdr, cbLen - sizeof(CUdpHdr));
                return;
            }
#endif

            // This is an insecure packet.  If we are compiled for online, some sockets
            // can accept insecure packets, so we'll pass them up to UpdRecv and let it
            // decide.  Otherwise, unless we are compiled insecure and the client has
            // requested bypassing security, the insecure UDP packet stops right here.
            
#if !defined(XNET_FEATURE_ONLINE)
    #ifdef XNET_FEATURE_INSECURE
            if (!(cfgFlags & XNET_STARTUP_BYPASS_SECURITY))
    #endif
            {
                TraceSz3(pktWarn, "[DISCARD] Insecure UDP packet on port %d from %s:%d",
                         NTOHS(pUdpHdr->_ipportDst), pIpHdr->_ipaSrc.Str(),
                         NTOHS(pUdpHdr->_ipportSrc));
                return;
            }
#endif
        }
    }

    UdpRecv(ppkt, pIpHdr, pUdpHdr, cbLen - sizeof(CUdpHdr));
}

void CXnIp::IpRecvTcp(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbLen)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(ppkt->IsTcp());

    if (ppkt->TestFlags(PKTF_RECV_BROADCAST) || pIpHdr->_ipaDst.IsBroadcast())
    {
        TraceSz(pktRecv, "[DISCARD] TCP packet with broadcast destination address");
        return;
    }

    UINT cbHdrLen;

    if (    cbLen < sizeof(CTcpHdr)
        || (cbHdrLen = pTcpHdr->GetHdrLen()) < sizeof(CTcpHdr)
        ||  cbHdrLen > cbLen
        ||  pTcpHdr->_ipportDst == 0
        ||  pTcpHdr->_ipportSrc == 0)
    {
        TraceSz5(pktWarn, "[DISCARD] TCP header is invalid (%d,%d,%d,%d,%d)",
                 cbLen < sizeof(CTcpHdr), cbHdrLen < sizeof(CTcpHdr),
                 cbHdrLen > cbLen, pTcpHdr->_ipportDst == 0, pTcpHdr->_ipportSrc == 0);
        return;
    }

    if (!ppkt->TestFlags(PKTF_TYPE_ESP|PKTF_RECV_LOOPBACK))
    {
        CPseudoHeader ph;
        ph._ipaSrc    = pIpHdr->_ipaSrc;
        ph._ipaDst    = pIpHdr->_ipaDst;
        ph._bZero     = 0;
        ph._bProtocol = IPPROTOCOL_TCP;
        ph._wLen      = HTONS((WORD)cbLen);

        if (tcpipxsum(tcpipxsum(0, &ph, sizeof(CPseudoHeader)), pTcpHdr, cbLen) != 0xFFFF)
        {
            TraceSz(pktWarn, "[DISCARD] TCP header checksum failed");
            return;
        }

        // This is an insecure packet.  If we are compiled for online, some sockets
        // can accept insecure packets, so we'll pass them up to TcpRecv and let it
        // decide.  Otherwise, unless we are compiled insecure and the client has
        // requested bypassing security, the insecure TCP packet stops right here.
            
#if !defined(XNET_FEATURE_ONLINE)
    #ifdef XNET_FEATURE_INSECURE
        if (!(cfgFlags & XNET_STARTUP_BYPASS_SECURITY))
    #endif
        {
            TraceSz3(pktWarn, "[DISCARD] Insecure TCP packet on port %d from %s:%d",
                     NTOHS(pTcpHdr->_ipportDst), pIpHdr->_ipaSrc.Str(), NTOHS(pTcpHdr->_ipportSrc));
            return;
        }
#endif
    }

    TcpRecv(ppkt, pIpHdr, pTcpHdr, cbHdrLen, cbLen - cbHdrLen);
}

void CXnIp::IpRecvEsp(CPacket * ppkt, CIpHdr * pIpHdr, CEspHdr * pEspHdr, UINT cbLen)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(ppkt->IsUdp());
    Assert(pEspHdr->_ipportDst == ESPUDP_CLIENT_PORT);

    if (cbLen < sizeof(CEspHdr))
    {
        TraceSz(pktWarn, "[DISCARD] ESPUDP header length exceeds packet length");
        return;
    }

    if (pEspHdr->_dwSpi == 0)
    {
        if (ppkt->TestFlags(PKTF_RECV_BROADCAST|PKTF_RECV_LOOPBACK))
        {
            TraceSz1(pktWarn, "[DISCARD] KeyEx packet received via %s",
                     ppkt->TestFlags(PKTF_RECV_BROADCAST) ? "broadcast" : "loopback");
            return;
        }

        IpRecvKeyEx(ppkt, pIpHdr->_ipaSrc, pEspHdr->_ipportSrc,
                    (CKeyExHdr *)&pEspHdr->_dwSeq, cbLen - offsetof(CEspHdr, _dwSeq));
        return;
    }

    if ((cbLen & 3) != 0)
    {
        TraceSz1(pktWarn, "[DISCARD] ESPUDP packet is not four-byte aligned (%d)", cbLen);
        return;
    }

    ppkt->SetType(PKTF_TYPE_IP|PKTF_TYPE_ESP);

    CSecReg *   pSecReg     = NULL;
    CIpAddr     ipa         = pEspHdr->_dwSpi;
    BOOL        fBroadcast  = ipa.IsBroadcast();
    DWORD       dwSeq       = NTOHL(pEspHdr->_dwSeq);
    DWORD       dwBit       = 0;
    CEspTail *  pEspTail    = ppkt->GetEspTail();
    BYTE *      pb          = (BYTE *)(pEspHdr + 1);
    BYTE *      pbKeySha;
    UINT        cbKeySha;
    BYTE *      pbKeyDes;
    UINT        cbKeyDes;

    if (!!fBroadcast != !!pIpHdr->_ipaDst.IsBroadcast())
    {
        TraceSz2(pktWarn, "[DISCARD] ESPUDP packet has dwSpi %08X but ipaDst %s",
                 pEspHdr->_dwSpi, pIpHdr->_ipaDst.Str());
        return;
    }

    if (fBroadcast)
    {
        pbKeySha = _abKeyShaLan;
        cbKeySha = sizeof(_abKeyShaLan);
        pbKeyDes = _abKeyDesLan;
        cbKeyDes = sizeof(_abKeyDesLan);

        if (dwSeq != 0xFFFFFFFF)
        {
            TraceSz1(pktWarn, "[DISCARD] ESPUDP broadcast packet has invalid dwSeq (%08lX)", dwSeq);
            return;
        }
    }
    else
    {
        pSecReg = SecRegLookup(ipa);

        if (pSecReg == NULL)
        {
            TraceSz1(pktWarn, "[DISCARD] Secure packet sent to unregistered address (%s)",
                     ipa.Str());
            return;
        }

        if (!pSecReg->IsRecvReady())
        {
            TraceSz1(pktWarn, "[DISCARD] Secure packet to %s before key exchange is complete",
                     ipa.Str());
            return;
        }

        pbKeySha = pSecReg->_abKeyShaRecv;
        cbKeySha = sizeof(pSecReg->_abKeyShaRecv);
        pbKeyDes = pSecReg->_abKeyDesRecv;
        cbKeyDes = pSecReg->_cbKeyDesRecv;

        if (dwSeq < pSecReg->_dwSeqRecv)
        {
            TraceSz3(pktWarn, "[DISCARD] Secure packet to %s has dwSeq %d less than window base %d",
                     ipa.Str(), dwSeq, pSecReg->_dwSeqRecv);
            return;
        }

        dwBit = dwSeq - pSecReg->_dwSeqRecv;

        if (dwBit > ESPUDP_REPLAY_WINDOW)
        {
            TraceSz4(pktWarn, "[DISCARD] Secure packet to %s has dwSeq %d outside window (%d to %d)",
                     ipa.Str(), dwSeq, pSecReg->_dwSeqRecv, pSecReg->_dwSeqRecv + ESPUDP_REPLAY_WINDOW);
            return;
        }

        if (dwBit < 32 && (pSecReg->_dwSeqMask & (1 << dwBit)))
        {
            TraceSz2(pktWarn, "[DISCARD] Secure packet to %s has dwSeq %d which is replayed",
                     ipa.Str(), dwSeq);
            return;
        }
    }

    if (cbKeyDes)
    {
        ppkt->SetFlags(PKTF_CRYPT);

        if (cbLen < sizeof(CEspHdr) + ROUNDUP8(XC_SERVICE_DES_BLOCKLEN + offsetof(CEspTail, _abHash)) + sizeof(pEspTail->_abHash))
        {
            TraceSz1(pktWarn, "[DISCARD] ESPUDP crypt packet is too small (%d)", cbLen);
            return;
        }
    }
    else
    {
        if (cbLen < sizeof(CEspHdr) + ROUNDUP4(sizeof(CEspTail)))
        {
            TraceSz1(pktWarn, "[DISCARD] ESPUDP auth packet is too small (%d)", cbLen);
            return;
        }
    }

    cbLen -= sizeof(CEspHdr) + sizeof(pEspTail->_abHash);

    TraceSz4(pktRecv, "[ESP %s #%d]%s[%d][ESPT]",
             ipa.Str(), dwSeq, ppkt->TestFlags(PKTF_CRYPT) ? "[IV]" : "",
             cbLen - (ppkt->TestFlags(PKTF_CRYPT) ? XC_SERVICE_DES_BLOCKLEN : 0) - offsetof(CEspTail, _abHash));

    // Authenicate the packet from the [ESP] header to just before the _abHash in [ESPT]

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];
    Assert(sizeof(pEspTail->_abHash) <= sizeof(abHash));
    XcHMAC(pbKeySha, cbKeySha, (BYTE *)&pEspHdr->_dwSpi, (sizeof(CEspHdr) - sizeof(CUdpHdr)) + cbLen,
           NULL, 0, abHash);

    if (memcmp(pEspTail->_abHash, abHash, sizeof(pEspTail->_abHash)) != 0)
    {
        TraceSz1(pktWarn, "[DISCARD] Secure packet to %s failed to authenticate", ipa.Str());
        return;
    }

    if (cbKeyDes)
    {
        // Decrypt the packet from just after the [ESP] header to just before the _abHash in [ESPT]

        if (cbLen != ROUNDUP8(cbLen))
        {
            TraceSz1(pktWarn, "[DISCARD] Secure crypt packet has invalid payload size (%d)", cbLen);
            return;
        }

        CryptDes(XC_SERVICE_DECRYPT, pbKeyDes, cbKeyDes, pb, pb + XC_SERVICE_DES_BLOCKLEN, cbLen);

        pb += XC_SERVICE_DES_BLOCKLEN;
        cbLen -= XC_SERVICE_DES_BLOCKLEN;
    }

    cbLen -= offsetof(CEspTail, _abHash);

    if (    pEspTail->_bNextHeader != IPPROTOCOL_UDP
        &&  pEspTail->_bNextHeader != IPPROTOCOL_TCP
        &&  pEspTail->_bNextHeader != IPPROTOCOL_SECMSG)
    {
        TraceSz2(pktWarn, "[DISCARD] Secure packet to %s failed bNextHeader test (%d)",
                ipa.Str(), pEspTail->_bNextHeader);
        return;
    }

    if (cbLen < pEspTail->_bPadLen)
    {
        TraceSz3(pktWarn, "[DISCARD] Secure packet to %s failed bPadLen test (%d/%d)",
                 ipa.Str(), cbLen, pEspTail->_bPadLen);
        return;
    }

    if (pEspTail->_bPadLen > 0)
    {
        UINT    cbPad = pEspTail->_bPadLen;
        BYTE *  pbPad = (BYTE *)pEspTail;

        // Verify that the padding is the series of bytes 1, 2, 3, ...

        while (cbPad > 0 && *--pbPad == (BYTE)cbPad)
            cbPad--;
    
        if (cbPad)
        {
            TraceSz1(pktWarn, "[DISCARD] Secure packet to %s failed padding test", ipa.Str());
            return;
        }

        cbLen -= pEspTail->_bPadLen;
    }

    if (!fBroadcast)
    {
        if (dwBit < 32)
        {
            // Sequence number is within the current window.  Just set the bit.

            Assert((pSecReg->_dwSeqMask & (1 << dwBit)) == 0);
            pSecReg->_dwSeqMask |= (1 << dwBit);
        }
        else
        {
            // Sequence number is beyond the edge of the window.  Slide window so that the
            // edge is at the sequence number.

            if (dwBit >= 63)
                pSecReg->_dwSeqMask = 0x80000000;
            else
                pSecReg->_dwSeqMask = 0x80000000 | (pSecReg->_dwSeqMask >> (dwBit - 31));

            pSecReg->_dwSeqRecv += dwBit - 31;
        }

        if (pSecReg->_bState != SR_STATE_READY)
        {
            Assert(pSecReg->_bState == SR_STATE_INITWAIT || pSecReg->_bState == SR_STATE_RESPSENT);

            // We have successfully authenticated a packet while in the INITWAIT or RESPSENT
            // state.  That means the other side definitely knows the keys.  Go into the
            // ready state and release any pending packets.

#ifdef XNET_FEATURE_SG

            if (pSecReg->TestFlags(SRF_ONLINEPEER))
            {
                // Capture the address of the sender of this packet.  This will be used
                // as the return address for packets sent on this security association.

                Assert(pIpHdr->_ipaSrc != 0 && pEspHdr->_ipportSrc != 0);

                pSecReg->_ipaDst    = pIpHdr->_ipaSrc;
                pSecReg->_ipportDst = pEspHdr->_ipportSrc;
            }

#endif

            pSecReg->_bState = SR_STATE_READY;
            TimerSet(&pSecReg->_timer, TIMER_INFINITE);
            SecRegXmitQueue(pSecReg);
        }

        // Change the return address in the IP header to the secure address of the sender

        pIpHdr->_ipaSrc = CIpAddr(pSecReg->_dwSpiRecv);

        // Remember the last time a packet was received on this security association

        pSecReg->_dwTickRecv = TimerTick();
    }

    pIpHdr->_bProtocol = pEspTail->_bNextHeader;

    if (pIpHdr->_bProtocol == IPPROTOCOL_UDP)
    {
        ppkt->SetType(PKTF_TYPE_ESP|PKTF_TYPE_UDP);
        IpRecvUdp(ppkt, pIpHdr, (CUdpHdr *)pb, cbLen);
    }
    else if (pIpHdr->_bProtocol == IPPROTOCOL_TCP)
    {
        ppkt->SetType(PKTF_TYPE_ESP|PKTF_TYPE_TCP);
        IpRecvTcp(ppkt, pIpHdr, (CTcpHdr *)pb, cbLen);
    }
    else
    {
        Assert(pIpHdr->_bProtocol == IPPROTOCOL_SECMSG);
        ppkt->SetType(PKTF_TYPE_ESP);
        IpRecvSecMsg(ppkt, pSecReg, dwSeq, (CSecMsgHdr *)pb, cbLen);
    }
}

void CXnIp::IpRecvKeyEx(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExHdr * pKeyExHdr, UINT cbKeyEx)
{
    ICHECK(IP, UDPC|SDPC);

    if (cbKeyEx < sizeof(CKeyExHdr) || pKeyExHdr->_cbEnt < sizeof(CKeyExHdr) || pKeyExHdr->_cbEnt > cbKeyEx)
    {
        TraceSz5(pktWarn, "[DISCARD] KeyEx from %s:%d header entry is invalid (%d,%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), cbKeyEx < sizeof(CKeyExHdr),
                 pKeyExHdr->_cbEnt < sizeof(CKeyExHdr), pKeyExHdr->_cbEnt > cbKeyEx);
        return;
    }

    if (pKeyExHdr->_wType == KEYEX_TYPE_XBTOXB_INIT || pKeyExHdr->_wType == KEYEX_TYPE_XBTOXB_RESP)
    {
        IpRecvKeyExXbToXb(ppkt, ipaSrc, ipportSrc, (CKeyExXbToXb *)pKeyExHdr, cbKeyEx);
        return;
    }

#ifdef XNET_FEATURE_SG

    if (pKeyExHdr->_wType == KEYEX_TYPE_SGTOXB_RESP && ipaSrc != 0 && ipportSrc != 0)
    {
        IpRecvKeyExSgToXb(ppkt, ipaSrc, ipportSrc, (CKeyExSgToXbResp *)pKeyExHdr, cbKeyEx);
        return;
    }

    if (    (pKeyExHdr->_wType == KEYEX_TYPE_NATOPEN_INIT && ipaSrc == 0 && ipportSrc == 0)
        ||  (pKeyExHdr->_wType == KEYEX_TYPE_NATOPEN_RESP && ipaSrc != 0 && ipportSrc != 0))
    {
        IpRecvKeyExNatOpen(ppkt, ipaSrc, ipportSrc, (CKeyExNatOpen *)pKeyExHdr, cbKeyEx);
        return;
    }

#endif

    TraceSz3(pktWarn, "[DISCARD] KeyEx from %s:%d header type (%04X) is invalid",
             ipaSrc.Str(), NTOHS(ipportSrc), pKeyExHdr->_wType);
    return;
}

void CXnIp::IpRecvKeyExXbToXb(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExXbToXb * pKeyExXbToXb, UINT cbKeyEx)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(cbKeyEx >= pKeyExXbToXb->_cbEnt);

    if (pKeyExXbToXb->_cbEnt != sizeof(CKeyExXbToXb))
    {
        TraceSz4(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d entry has an incorrect size (%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), pKeyExXbToXb->_cbEnt, sizeof(CKeyExXbToXb));
        return;
    }

#ifdef XNET_FEATURE_SG

    if (XNetXnKidIsOnlinePeer(&pKeyExXbToXb->_xnkid) && _pSecRegLogon == NULL)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d ignoring request from online-peer while offline",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

#endif

    CKeyReg * pKeyReg = NULL;
    CSecReg * pSecReg = NULL;

    if (pKeyExXbToXb->_wType == KEYEX_TYPE_XBTOXB_INIT)
    {
        pKeyReg = KeyRegLookup(&pKeyExXbToXb->_xnkid);

        if (pKeyReg == NULL)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbInit from %s:%d to unregistered XNKID %s",
                     ipaSrc.Str(), NTOHS(ipportSrc), HexStr(pKeyExXbToXb->_xnkid.ab, sizeof(pKeyExXbToXb->_xnkid.ab)));
            return;
        }
    }
    else
    {
        pSecReg = SecRegLookup(pKeyExXbToXb->_dwSpiInit);

        if (pSecReg == NULL)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to unregistered address %s",
                     ipaSrc.Str(), NTOHS(ipportSrc), CIpAddr(pKeyExXbToXb->_dwSpiInit).Str());
            return;
        }

    #ifdef XNET_FEATURE_SG

        if (ipaSrc == 0 || ipportSrc == 0)
        {
            TraceSz2(pktWarn, "[DISCARD] KeyExXbToXbResp received via SG forwarding (%d,%d)",
                     ipaSrc == 0, ipportSrc == 0);
            return;
        }

        if (!pSecReg->TestFlags(SRF_SYSTEMLINK|SRF_ONLINEPEER))
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to non peer-to-peer address %s",
                     ipaSrc.Str(), NTOHS(ipportSrc), CIpAddr(pSecReg->_dwSpiRecv).Str());
            return;
        }

    #endif

        if (pKeyExXbToXb->_liTime.QuadPart <= pSecReg->_liTime.QuadPart)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to %s was replayed",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->Str());
            return;
        }

        if (pSecReg->_bState != SR_STATE_INITSENT && pSecReg->_bState != SR_STATE_INITWAIT)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to %s while not in ISENT or IWAIT state",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->Str());
            return;
        }

        Assert(sizeof(pSecReg->_abNonceInit) == sizeof(pKeyExXbToXb->_abNonceInit));

        if (memcmp(pSecReg->_abNonceInit, pKeyExXbToXb->_abNonceInit, sizeof(pSecReg->_abNonceInit)) != 0)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to %s has incorrect nonce",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->Str());
            return;
        }

        pKeyReg = pSecReg->_pKeyReg;

        if (memcmp(pKeyReg->_xnkid.ab, pKeyExXbToXb->_xnkid.ab, sizeof(pKeyReg->_xnkid.ab)) != 0)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXbResp from %s:%d to %s has incorrect xnkid",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->Str());
            return;
        }
    }

    // Advance to the next entry.  It must contain the diffie-hellman g^X value.

    cbKeyEx -= pKeyExXbToXb->_cbEnt;
    CKeyExHdr * pKeyExDh = (CKeyExHdr *)((BYTE *)pKeyExXbToXb + pKeyExXbToXb->_cbEnt);

    if (    cbKeyEx < sizeof(CKeyExHdr)
        ||  pKeyExDh->_wType != KEYEX_TYPE_DH_GX
        ||  pKeyExDh->_cbEnt != sizeof(CKeyExHdr) + CBDHG1
        ||  pKeyExDh->_cbEnt > cbKeyEx)
    {
        TraceSz6(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d DH entry is invalid (%d,%d,%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), cbKeyEx < sizeof(CKeyExHdr),
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_wType != KEYEX_TYPE_DH_GX,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_cbEnt != sizeof(CKeyExHdr) + CBDHG1,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_cbEnt > cbKeyEx);
        return;
    }

    // Advance to the next entry.  It must contain the HMAC-SHA digest of the previous entries,
    // and it must also be the last entry.

    cbKeyEx -= pKeyExDh->_cbEnt;
    CKeyExHdr * pKeyExSha = (CKeyExHdr *)((BYTE *)pKeyExDh + pKeyExDh->_cbEnt);

    if (    cbKeyEx < sizeof(CKeyExHdr)
        ||  pKeyExSha->_wType != KEYEX_TYPE_HMAC_SHA
        ||  pKeyExSha->_cbEnt != sizeof(CKeyExHdr) + XC_SERVICE_DIGEST_SIZE
        ||  pKeyExSha->_cbEnt != cbKeyEx)
    {
        TraceSz6(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d SHA entry is invalid (%d,%d,%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), cbKeyEx < sizeof(CKeyExHdr),
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExSha->_wType != KEYEX_TYPE_HMAC_SHA,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExSha->_cbEnt != sizeof(CKeyExHdr) + XC_SERVICE_DIGEST_SIZE,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExSha->_cbEnt != cbKeyEx);
        return;
    }

    // Authenticate the key exchange message (all entries except the last) using the key-exchange-key
    // corresponding to the XNKID in the message.

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];

    XcHMAC(pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha), (BYTE *)pKeyExXbToXb,
           (BYTE *)pKeyExSha - (BYTE *)pKeyExXbToXb, NULL, 0, abHash);

    Assert(pKeyExSha->_cbEnt == sizeof(CKeyExHdr) + sizeof(abHash));

    if (memcmp((BYTE *)(pKeyExSha + 1), abHash, sizeof(abHash)) != 0)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d failed to authenticate",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

    // Decrypt the portion of the first entry which contains the XNADDR structures

    CryptDes(XC_SERVICE_DECRYPT, pKeyReg->_abKeyDes, sizeof(pKeyReg->_abKeyDes),
             pKeyExXbToXb->_abIv, pKeyExXbToXb->_abIv + XC_SERVICE_DES_BLOCKLEN,
             sizeof(CKeyExXbToXb) - offsetof(CKeyExXbToXb, _abIv) - XC_SERVICE_DES_BLOCKLEN);

    // Verify that the sender knows the current XNADDR of this stack

    XNADDR xnaddr, * pxnaddr;
    IpGetXnAddr(&xnaddr);

    pxnaddr = (pKeyExXbToXb->_wType == KEYEX_TYPE_XBTOXB_INIT) ? &pKeyExXbToXb->_xnaddrResp : &pKeyExXbToXb->_xnaddrInit;

    if (memcmp(&xnaddr, pxnaddr, sizeof(XNADDR)) != 0)
    {
        TraceSz4(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d target XNADDR mismatch; got %s expected %s",
                 ipaSrc.Str(), NTOHS(ipportSrc), XnAddrStr(pxnaddr), XnAddrStr(&xnaddr));
        return;
    }

    // Verify that the sender's XNADDR is valid

    pxnaddr = (pKeyExXbToXb->_wType == KEYEX_TYPE_XBTOXB_INIT) ? &pKeyExXbToXb->_xnaddrInit : &pKeyExXbToXb->_xnaddrResp;

    if (XNetXnKidIsSystemLink(&pKeyExXbToXb->_xnkid))
    {
        CEnetHdr * pEnetHdr = ppkt->GetEnetHdr();

        if (!pEnetHdr->_eaSrc.IsEqual(pxnaddr->abEnet))
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d source XNADDR failed system-link test (%d)",
                     ipaSrc.Str(), NTOHS(ipportSrc), !pEnetHdr->_eaSrc.IsEqual(pxnaddr->abEnet));
            return;
        }
    }

#ifdef XNET_FEATURE_SG

    if (XNetXnKidIsOnlinePeer(&pKeyExXbToXb->_xnkid))
    {
        if (    !CIpAddr(pxnaddr->ina.s_addr).IsValidUnicast()
            ||  !CIpAddr(pxnaddr->inaOnline.s_addr).IsValidUnicast()
            ||  pxnaddr->wPortOnline == 0)
        {
            TraceSz5(pktWarn, "[DISCARD] KeyExXbToXb from %s:%d source XNADDR failed online-peer tests (%d,%d,%d)",
                     ipaSrc.Str(), NTOHS(ipportSrc), !CIpAddr(pxnaddr->ina.s_addr).IsValidUnicast(),
                     !CIpAddr(pxnaddr->inaOnline.s_addr).IsValidUnicast(), pxnaddr->wPortOnline == 0);
            return;
        }
    }
                    
#endif

    if (pKeyExXbToXb->_wType == KEYEX_TYPE_XBTOXB_INIT)
    {
        // Try to find an existing CSecReg given the XNADDR of the initiator and the XNKID

        pSecReg = SecRegLookup(&pKeyExXbToXb->_xnaddrInit, &pKeyExXbToXb->_xnkid);

        if (pSecReg == NULL)
        {
            // Not found.  Allocate a new CSecReg.

            pSecReg = SecRegAlloc(&pKeyExXbToXb->_xnaddrInit, pKeyReg);

            if (pSecReg == NULL)
            {
                TraceSz2(pktWarn, "[DISCARD] KeyExXbToXbInit from %s:%d reached CSecReg limit",
                         ipaSrc.Str(), NTOHS(ipportSrc));
                return;
            }
        }
        else if (pKeyExXbToXb->_liTime.QuadPart <= pSecReg->_liTime.QuadPart)
        {
            TraceSz2(pktWarn, "[DISCARD] KeyExXbToXbInit from %s:%d is obsolete or replayed",
                     ipaSrc.Str(), NTOHS(ipportSrc));
            return;
        }
#ifdef XNET_FEATURE_SG
        else if (pSecReg->TestFlags(SRF_ONLINEPEER))
        {
            // Update the XNADDR for this CSecReg.  Is is possible that the online information
            // has changed since the last time the peer contacted us.

            pSecReg->_xnaddr = pKeyExXbToXb->_xnaddrInit;
        }
#endif

        pSecReg->_liTime = pKeyExXbToXb->_liTime;

        // If we are in the INITSENT state, then we have simultaneous initiator packets
        // being sent.  To resolve this, the side which has the highest ethernet address
        // is the initiator and the other side becomes the responder.

        if (pSecReg->_bState == SR_STATE_INITSENT)
        {
            if (memcmp(_ea._ab, pKeyExXbToXb->_xnaddrInit.abEnet, sizeof(CEnetAddr)) > 0)
            {
                // This side wins.  The other side will run the same computation and will
                // respond to our initiator packet.  We just discard theirs.

                TraceSz2(pktRecv, "[DISCARD] KeyExXbToXbInit from %s:%d simultaneous initiators",
                         ipaSrc.Str(), NTOHS(ipportSrc));
                return;
            }
        }

        // If we are in the RESPSENT state, this could be a retransmit of the initiator packet.
        // We verify this by checking the nonce of the initiator that we remembered from before.

        if (    pSecReg->_bState != SR_STATE_RESPSENT
            ||  memcmp(pSecReg->_abNonceInit, pKeyExXbToXb->_abNonceInit, sizeof(pSecReg->_abNonceInit)) != 0)
        {
            // This is a new key exchange initiation sequence.  Reset the security association.

            SecRegSetIdle(pSecReg);

            pSecReg->_bState    = SR_STATE_RESPSENT;
            pSecReg->_bRetry    = SecRegRexmitRetries(pSecReg);
            pSecReg->_dwSpiXmit = pKeyExXbToXb->_dwSpiInit;

            // Remember the nonce of the initiator in case we need to retransmit this reponse.
            // Generate a random nonce for the responder.

            Assert(sizeof(pSecReg->_abNonceInit) == sizeof(pKeyExXbToXb->_abNonceInit));
            memcpy(pSecReg->_abNonceInit, pKeyExXbToXb->_abNonceInit, sizeof(pSecReg->_abNonceInit));
            Rand(pSecReg->_abNonceResp, sizeof(pSecReg->_abNonceResp));

            // Generate the SHA and DES keys for this security association

            SecRegSetKey(pSecReg, pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha),
                         pKeyReg->_abDhX, sizeof(pKeyReg->_abDhX),
                         (BYTE *)(pKeyExDh + 1), pKeyExDh->_cbEnt - sizeof(CKeyExHdr),
                         FALSE);

            // Set the retransmission timer.  This side is responsible for retransmitting
            // the key exchange response until it gets a secure packet from the other side.

            TimerSet(&pSecReg->_timer, TimerTick() + SecRegRexmitTimeoutInSeconds(pSecReg) * TICKS_PER_SECOND);
        }

#ifdef XNET_FEATURE_SG
        if (pSecReg->TestFlags(SRF_ONLINEPEER) && ipaSrc != 0 && ipportSrc != 0)
        {
            // Capture the return address information from the initiator packet if the packet
            // arrived directly (versus being forwarded through the security gateways).

            pSecReg->_ipaDst    = ipaSrc;
            pSecReg->_ipportDst = ipportSrc;
        }
#endif

        // Send a KeyEx XbToXb response packet back to the initiator

        IpXmitKeyExXbToXb(pSecReg);
    }
    else
    {
#if DBG
        if (Tag(keyExDrop) && pSecReg->_bRetry == SecRegRexmitRetries(pSecReg))
        {
            // Drop the first key-exchange response for testing purposes
            TraceSz(pktWarn, "[DISCARD] Dropping first KeyExXbToXbResp (debug only)");
            return;
        }
#endif

        pSecReg->_liTime = pKeyExXbToXb->_liTime;

#ifdef XNET_FEATURE_SG
        if (pSecReg->TestFlags(SRF_ONLINEPEER))
        {
            // Capture the return address information from the responder packet.  Responses are
            // always sent directly (as opposed to being forwarded through the security gateways).

            Assert(ipaSrc != 0 && ipportSrc != 0);

            pSecReg->_ipaDst    = ipaSrc;
            pSecReg->_ipportDst = ipportSrc;
        }
#endif

        // If we are in the INITWAIT state, this could be a retransmit of the responder packet.
        // We verify this by checking the nonce of the responder that we remembered from before.

        if (    pSecReg->_bState != SR_STATE_INITWAIT
            ||  memcmp(pSecReg->_abNonceResp, pKeyExXbToXb->_abNonceResp, sizeof(pSecReg->_abNonceResp)) != 0)
        {
            // This is the first time we've gotten a response from the other side to this
            // key exchange initiation.  Remember the nonce of the responder and the dwSpiXmit.

            Assert(sizeof(pSecReg->_abNonceResp) == sizeof(pKeyExXbToXb->_abNonceResp));
            memcpy(pSecReg->_abNonceResp, pKeyExXbToXb->_abNonceResp, sizeof(pSecReg->_abNonceResp));
            pSecReg->_dwSpiXmit = pKeyExXbToXb->_dwSpiResp;

            // Generate the SHA and DES keys for this security association

            SecRegSetKey(pSecReg, pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha),
                         pKeyReg->_abDhX, sizeof(pKeyReg->_abDhX),
                         (BYTE *)(pKeyExDh + 1), pKeyExDh->_cbEnt - sizeof(CKeyExHdr),
                         TRUE);

            // Switch to the INITWAIT state.  In this state, we know that the other side has computed
            // the same keys that we have, so we stop retransmitting the initiator packet.

            pSecReg->_bState = SR_STATE_INITWAIT;
            TimerSet(&pSecReg->_timer, TIMER_INFINITE);
        }

        // We must send a response.  If there are waiting packets, send those now.
        // Otherwise, send a SECMSG_TYPE_PULSE to let the other side know we're alive.

        if (pSecReg->_pqWait.IsEmpty())
            IpXmitSecMsg(pSecReg, SECMSG_TYPE_PULSE);
        else
            SecRegXmitQueue(pSecReg);
    }
}

#ifdef XNET_FEATURE_SG

void CXnIp::IpRecvKeyExNatOpen(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExNatOpen * pKeyExNatOpen, UINT cbKeyEx)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(pKeyExNatOpen->_wType == KEYEX_TYPE_NATOPEN_INIT || pKeyExNatOpen->_wType == KEYEX_TYPE_NATOPEN_RESP);
    Assert(cbKeyEx >= pKeyExNatOpen->_cbEnt);

    if (pKeyExNatOpen->_cbEnt != sizeof(CKeyExNatOpen))
    {
        TraceSz4(pktWarn, "[DISCARD] KeyExNatOepn from %s:%d entry has an incorrect size (%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), pKeyExNatOpen->_cbEnt, sizeof(CKeyExNatOpen));
        return;
    }

    CKeyReg * pKeyReg = KeyRegLookup(&pKeyExNatOpen->_xnkid);

    if (pKeyReg == NULL)
    {
        TraceSz3(pktWarn, "[DISCARD] KeyExNatOpen from %s:%d to unregistered XNKID %s",
                 ipaSrc.Str(), NTOHS(ipportSrc), HexStr(pKeyExNatOpen->_xnkid.ab, sizeof(pKeyExNatOpen->_xnkid.ab)));
        return;
    }

    // Authenticate the message including the header using the key-exchange-key corresponding to
    // the XNKID in the message.

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];

    XcHMAC(pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha), (BYTE *)pKeyExNatOpen,
           offsetof(CKeyExNatOpen, _abHash), NULL, 0, abHash);

    Assert(sizeof(pKeyExNatOpen->_abHash) == sizeof(abHash));

    if (memcmp(abHash, pKeyExNatOpen->_abHash, sizeof(abHash)) != 0)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExNatOpen from %s:%d failed to authenticate",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

    if (pKeyExNatOpen->_wType == KEYEX_TYPE_NATOPEN_INIT)
    {
        Assert(ipaSrc == 0 && ipportSrc == 0);

        // Reply to this request by sending a CKeyExNatOpen packet to the destination address
        // specified.  We change the type to KEYEX_TYPE_NATOPEN_RESP and recompute the abHash
        // to prove to the requester that we know the key-exhcange-key associated with the xnkid.

        CPacket * ppktXmit = PacketAlloc(PTAG_CKeyExPacket, PKTF_TYPE_UDP|PKTF_POOLALLOC, sizeof(DWORD) + sizeof(CKeyExNatOpen));

        if (ppktXmit == NULL)
        {
            TraceSz(Warning, "IpRecvKeyExNatOpen - Out of memory allocating packet");
            return;
        }

        CEnetHdr *      pEnetHdr          = ppktXmit->GetEnetHdr();
        CIpHdr *        pIpHdr            = ppktXmit->GetIpHdr();
        CEspHdr *       pEspHdr           = (CEspHdr *)(pIpHdr + 1);
        CKeyExNatOpen * pKeyExNatOpenXmit = (CKeyExNatOpen *)&pEspHdr->_dwSeq;

        IpFillHdr(ppktXmit, pKeyExNatOpen->_ipaDst, IPPROTOCOL_UDP);

        pIpHdr->_ipaSrc     = _ipa;
        pEspHdr->_wLen      = NTOHS(sizeof(CUdpHdr) + sizeof(DWORD) + sizeof(CKeyExNatOpen));
        pEspHdr->_ipportSrc = ESPUDP_CLIENT_PORT;
        pEspHdr->_ipportDst = pKeyExNatOpen->_ipportDst;
        pEspHdr->_wChecksum = 0;
        pEspHdr->_dwSpi     = 0;

        memcpy(pKeyExNatOpenXmit, pKeyExNatOpen, offsetof(CKeyExNatOpen, _abHash));
        pKeyExNatOpenXmit->_wType = KEYEX_TYPE_NATOPEN_RESP;

        Assert(sizeof(pKeyExNatOpenXmit->_abHash) == XC_SERVICE_DIGEST_SIZE);
        XcHMAC(pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha), (BYTE *)pKeyExNatOpenXmit,
               offsetof(CKeyExNatOpen, _abHash), NULL, 0, pKeyExNatOpenXmit->_abHash);

        TraceSz3(secStat, "Sending KeyExNatOpen RESP/%04X to %s:%d",
                 pKeyExNatOpenXmit->_wFlags, pIpHdr->_ipaDst.Str(), NTOHS(pEspHdr->_ipportDst));

        IpXmitIp(ppktXmit, NULL);
        return;
    }

    if (pKeyExNatOpen->_wType == KEYEX_TYPE_NATOPEN_RESP)
    {
        Assert(ipaSrc != 0 && ipportSrc != 0);

        if (pKeyExNatOpen->_wFlags & KNOF_XBTOXB_KEYEX)
        {
            // This message is the response from a NatOpen request we sent via the SG forwarding
            // mechanism.  Lookup the CSecReg associated from the _dwCtx.

            CSecReg * pSecReg = SecRegLookup(pKeyExNatOpen->_dwCtx);

            Assert(sizeof(pSecReg->_abNonceInit) == sizeof(pKeyExNatOpen->_abNonce));

            if (     pSecReg == NULL
                ||   pSecReg->_bState != SR_STATE_INITSENT
                ||  !pSecReg->TestFlags(SRF_ONLINEPEER)
                ||   memcmp(pSecReg->_abNonceInit, pKeyExNatOpen->_abNonce, sizeof(pSecReg->_abNonceInit)) != 0)
            {
                TraceSz6(pktWarn, "[DISCARD] KeyExNatOpen from %s:%d is obsolete or invalid (%d,%d,%d,%d)",
                         ipaSrc.Str(), NTOHS(ipportSrc), pSecReg == NULL,
                         pSecReg && pSecReg->_bState != SR_STATE_INITSENT,
                         pSecReg && !pSecReg->TestFlags(SRF_ONLINEPEER),
                         pSecReg && memcmp(pSecReg->_abNonceInit, pKeyExNatOpen->_abNonce, sizeof(pSecReg->_abNonceInit)) != 0);
                return;
            }

            // Capture the return address information from the sender, and retransmit the key exchange
            // initiator packet.  The hope is that the sender will have programmed his NAT to allow
            // this packet to get through.  It is still not guaranteed to get through, however, if our
            // NAT is agressively assigning ports and the other side is port filtering.

            pSecReg->_ipaDst    = ipaSrc;
            pSecReg->_ipportDst = ipportSrc;

            // Retransmit the key exchange initiator packet right now (instead of waiting for the
            // timeout to expire).  The second argument tells IpXmitKeyExXbToXb to not send a
            // CKeyExNatOpen message this time.  Sending it now could cause a storm of packets
            // flowing if communication is one-sided (i.e. this side can receive but not
            // transmit).

            IpXmitKeyExXbToXb(pSecReg, TRUE);
            return;
        }

        //@@@ Handle KNOF_XBTOXB_PROBE here
    }

    TraceSz4(pktWarn, "[DISCARD] KeyExNatOpen from %s:%d type %04X is invalid (wFlags %04X)",
             ipaSrc.Str(), NTOHS(ipportSrc), pKeyExNatOpen->_wType, pKeyExNatOpen->_wFlags);
    return;
}

void CXnIp::IpRecvKeyExSgToXb(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExSgToXbResp * pKeyExSgToXbResp, UINT cbKeyEx)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(pKeyExSgToXbResp->_wType == KEYEX_TYPE_SGTOXB_RESP);
    Assert(cbKeyEx >= pKeyExSgToXbResp->_cbEnt);

    if (pKeyExSgToXbResp->_cbEnt != sizeof(CKeyExSgToXbResp))
    {
        TraceSz4(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d entry has an incorrect size (%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), pKeyExSgToXbResp->_cbEnt, sizeof(CKeyExSgToXbResp));
        return;
    }

    CSecReg * pSecReg = SecRegLookup(pKeyExSgToXbResp->_dwSpiInit);

    if (pSecReg == NULL)
    {
        TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d to unregistered address %s",
                 ipaSrc.Str(), NTOHS(ipportSrc), CIpAddr(pKeyExSgToXbResp->_dwSpiInit).Str());
        return;
    }

    if (!pSecReg->TestFlags(SRF_ONLINESERVER))
    {
        TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d to non online-server address %s",
                 ipaSrc.Str(), NTOHS(ipportSrc), CIpAddr(pKeyExSgToXbResp->_dwSpiInit).Str());
        return;
    }

    if (pSecReg->_bState != SR_STATE_INITSENT)
    {
        TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d to %s while not in ISENT state",
                 ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->Str());
        return;
    }

    Assert(sizeof(pSecReg->_abNonceInit) == sizeof(pKeyExSgToXbResp->_abNonceInit));

    if (memcmp(pSecReg->_abNonceInit, pKeyExSgToXbResp->_abNonceInit, sizeof(pSecReg->_abNonceInit)) != 0)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d has incorrect nonce",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

    // Advance to the next entry.  It must contain the diffie-hellman g^X value.

    cbKeyEx -= pKeyExSgToXbResp->_cbEnt;
    CKeyExHdr * pKeyExDh = (CKeyExHdr *)((BYTE *)pKeyExSgToXbResp + pKeyExSgToXbResp->_cbEnt);

    if (    cbKeyEx < sizeof(CKeyExHdr)
        ||  pKeyExDh->_wType != KEYEX_TYPE_DH_GX
        ||  pKeyExDh->_cbEnt != sizeof(CKeyExHdr) + CBDHG1
        ||  pKeyExDh->_cbEnt > cbKeyEx)
    {
        TraceSz6(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d DH entry is invalid (%d,%d,%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc),
                 cbKeyEx < sizeof(CKeyExHdr),
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_wType != KEYEX_TYPE_DH_GX,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_cbEnt != sizeof(CKeyExHdr) + CBDHG1,
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExDh->_cbEnt > cbKeyEx);
        return;
    }

    // Advance to the next entry.  It must contain the KERB_APREP and it must also be the last entry.

    cbKeyEx -= pKeyExDh->_cbEnt;
    CKeyExHdr * pKeyExApRep = (CKeyExHdr *)((BYTE *)pKeyExDh + pKeyExDh->_cbEnt);

    if (cbKeyEx < sizeof(CKeyExHdr) || pKeyExApRep->_cbEnt != cbKeyEx)
    {
        TraceSz4(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d ApRep entry is invalid (%d,%d)",
                 ipaSrc.Str(), NTOHS(ipportSrc), cbKeyEx < sizeof(CKeyExHdr),
                 cbKeyEx >= sizeof(CKeyExHdr) && pKeyExApRep->_cbEnt != cbKeyEx);
        return;
    }

    // Crack the ApRep 

    BYTE *  pbKeyHmac;          // Key to use to compute HMAC-SHA of key-exchange message
    UINT    cbKeyHmac;          // Size of pbKeyHmac
    BYTE *  pbShaApRep;         // HMAC-SHA from the AP reply
    UINT    cbShaApRep;         // Size of pbShaApRep
    BYTE *  pbDhX;              // Diffie-hellman X
    UINT    cbDhX;              // Size of pbDhX

#ifdef XNET_FEATURE_ONLINE

    XOKERBINFO *    pxokerbinfo;
    BYTE            abShaApRep[XC_SERVICE_DIGEST_SIZE];

    if (pKeyExApRep->_wType == KEYEX_TYPE_KERB_APREP)
    {
        pxokerbinfo = _pXoBase ? _pXoBase->XoKerbGetInfo(pSecReg->_dwServiceId) : NULL;

        if (pxokerbinfo == NULL)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d cannot find XOKERBINFO for service %08lX",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->_dwServiceId);
            return;
        }

        if (!_pXoBase->XoKerbCrackApRep(pSecReg->_dwServiceId, &pSecReg->_liTime, abShaApRep,
                                        sizeof(abShaApRep), (BYTE *)(pKeyExApRep + 1),
                                        pKeyExApRep->_cbEnt - sizeof(CKeyExHdr)))
        {
            TraceSz2(Warning, "[DISCARD] KeyExSgToXbResp from %s:%d XoKerbCrackApRep failed",
                     ipaSrc.Str(), NTOHS(ipportSrc));
            return;
        }

        pbKeyHmac  = pxokerbinfo->_abKey;
        cbKeyHmac  = sizeof(pxokerbinfo->_abKey);
        pbShaApRep = abShaApRep;
        cbShaApRep = sizeof(abShaApRep);
        pbDhX      = pxokerbinfo->_abDhX;
        cbDhX      = sizeof(pxokerbinfo->_abDhX);

        goto ApRepDone;
    }
#endif

#ifdef XNET_FEATURE_INSECURE

    if (pKeyExApRep->_wType == KEYEX_TYPE_NULL_APREP)
    {
        CKeyExNullApRep * pKeyExNullApRep = (CKeyExNullApRep *)pKeyExApRep;

        if (pKeyExNullApRep->_cbEnt != sizeof(CKeyExNullApRep))
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d NULL_APREP has invalid size (%d bytes)",
                     ipaSrc.Str(), NTOHS(ipportSrc), pKeyExApRep->_cbEnt);
            return;
        }

        if (pSecReg->_dwServiceId != 0)
        {
            TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d NULL_APREP unexpected for service %08lX",
                     ipaSrc.Str(), NTOHS(ipportSrc), pSecReg->_dwServiceId);
            return;
        }

        pbKeyHmac   = _abKeyNull;
        cbKeyHmac   = sizeof(_abKeyNull);
        pbShaApRep  = pKeyExNullApRep->_abSha;
        cbShaApRep  = sizeof(pKeyExNullApRep->_abSha);
        pbDhX       = _abDhXNull;
        cbDhX       = sizeof(_abDhXNull);

        goto ApRepDone;
    }

#endif

    TraceSz3(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d APREP type (%d) not supported",
             ipaSrc.Str(), NTOHS(ipportSrc), pKeyExApRep->_wType);
    return;

ApRepDone:

    // Authenticate the key exchange message (all entries except the last).

    BYTE abSha[XC_SERVICE_DIGEST_SIZE];

    XcHMAC(pbKeyHmac, cbKeyHmac, (BYTE *)pKeyExSgToXbResp, (BYTE *)pKeyExApRep - (BYTE *)pKeyExSgToXbResp,
           NULL, 0, abSha);

    Assert(cbShaApRep <= sizeof(abSha));

    if (memcmp(abSha, pbShaApRep, cbShaApRep) != 0)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d failed to authenticate",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

    if (pKeyExSgToXbResp->_dwFlags & SXRF_ENCRYPT_DES)
        pSecReg->_cbKeyDesRecv = pSecReg->_cbKeyDesXmit = XC_SERVICE_DES_KEYSIZE;
    else if (pKeyExSgToXbResp->_dwFlags & SXRF_ENCRYPT_3DES)
        pSecReg->_cbKeyDesRecv = pSecReg->_cbKeyDesXmit = XC_SERVICE_DES3_KEYSIZE;
    else
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExSgToXbResp from %s:%d unrecognized encryption",
                 ipaSrc.Str(), NTOHS(ipportSrc));
        return;
    }

#if DBG
    if (Tag(keyExDrop) && pSecReg->_bRetry == SecRegRexmitRetries(pSecReg))
    {
        // Drop the first key-exchange response for testing purposes
        TraceSz(pktWarn, "[DISCARD] Dropping first KeyExSgToXbResp (debug only)");
        return;
    }
#endif

    pSecReg->_dwSpiXmit           = pKeyExSgToXbResp->_dwSpiResp;
    pSecReg->_dwTickPulseTimeout  = pKeyExSgToXbResp->_wXbToSgPulseTimeoutInSecs * TICKS_PER_SECOND;
    pSecReg->_dwTickTimeout       = pKeyExSgToXbResp->_wXbToSgTimeoutInSecs * TICKS_PER_SECOND;
    pSecReg->_sgaddr              = pKeyExSgToXbResp->_sgaddrInit;
    pSecReg->_ipaNat              = pKeyExSgToXbResp->_inaInit.s_addr;
    pSecReg->_ipportNat           = pKeyExSgToXbResp->_wPortInit;

    Assert(sizeof(pSecReg->_abNonceResp) == sizeof(pKeyExSgToXbResp->_abNonceResp));
    memcpy(pSecReg->_abNonceResp, pKeyExSgToXbResp->_abNonceResp, sizeof(pSecReg->_abNonceResp));

    SecRegSetKey(pSecReg, pbKeyHmac, cbKeyHmac, pbDhX, cbDhX, (BYTE *)(pKeyExDh + 1),
                 pKeyExDh->_cbEnt - sizeof(CKeyExHdr), TRUE);

    pSecReg->_bState = SR_STATE_INITWAIT;
    TimerSet(&pSecReg->_timer, TIMER_INFINITE);

    if (pSecReg == _pSecRegLogon)
    {
        Assert(_uiLogonState == XN_LOGON_STATE_PENDING);

        _uiLogonState = XN_LOGON_STATE_ONLINE;

        if (_pEventLogon)
        {
            EvtSet(_pEventLogon, EVENT_INCREMENT);
        }
    }

    // We must respond with to the initiator.  If there are waiting packets, send those now.
    // Otherwise, send a SECMSG_TYPE_PULSE to let the other side know we're alive.

    if (pSecReg->_pqWait.IsEmpty())
        IpXmitSecMsg(pSecReg, SECMSG_TYPE_PULSE);
    else
        SecRegXmitQueue(pSecReg);
}

#endif

void CXnIp::IpRecvSecMsg(CPacket * ppkt, CSecReg * pSecReg, DWORD dwSeq, CSecMsgHdr * pSecMsgHdr, UINT cb)
{
    ICHECK(IP, UDPC|SDPC);
    
    if (cb < sizeof(CSecMsgHdr) || cb != pSecMsgHdr->_cbEnt)
    {
        TraceSz3(pktWarn, "[DISCARD] SecMsg to %s has wrong size (%d,%d)",
                 pSecReg->Str(), cb < sizeof(CSecMsgHdr),
                 cb >= sizeof(CSecMsgHdr) && cb != pSecMsgHdr->_cbEnt);
        return;
    }

    TraceSz3(pktRecv, "[SecMsg %s %04X]{+%d}", pSecReg->Str(), pSecMsgHdr->_wType, pSecMsgHdr->_cbEnt - sizeof(CSecMsgHdr));

    if (pSecMsgHdr->_wType == SECMSG_TYPE_DELETE)
    {
        if (cb != sizeof(CSecMsgDelete))
        {
            TraceSz2(pktWarn, "[DISCARD] SecMsg SECMSG_TYPE_DELETE to %s has incorrect payload size (%d)",
                     pSecReg->Str(), cb);
            return;
        }

        // The other side has disconnected from us.  Reset the CSecReg so that KeyEx will
        // be reinitiated next time a packet is transmitted.

        if (!pSecReg->TestFlags(SRF_OWNED))
            SecRegFree(pSecReg);
        else
            SecRegSetIdle(pSecReg);

        return;
    }

    if (pSecMsgHdr->_wType == SECMSG_TYPE_PULSE)
    {
        if (cb != sizeof(CSecMsgPulse))
        {
            TraceSz2(pktWarn, "[DISCARD] SecMsg SECMSG_TYPE_PULSE to %s has incorrect payload size (%d)",
                     pSecReg->Str(), cb);
            return;
        }

        // Nothing else to do with this message.  It was only sent to let us know that the
        // other side is still alive.

        return;
    }

#ifdef XNET_FEATURE_SG

    if (pSecMsgHdr->_wType == SECMSG_TYPE_SGTOXB_PULSE)
    {
        if (cb < sizeof(CSecMsgSgToXbPulse))
        {
            TraceSz2(pktWarn, "[DISCARD] SecMsg SECMSG_TYPE_SGTOXB_PULSE to %s has incorrect payload size (%d)",
                     pSecReg->Str(), cb);
            return;
        }

        if (pSecReg == _pSecRegLogon)
        {
            CSecMsgSgToXbPulse * pSecMsgSgToXbPulse = (CSecMsgSgToXbPulse *)pSecMsgHdr;

            if (pSecMsgSgToXbPulse->_dwSeqAck && pSecMsgSgToXbPulse->_dwSeqAck >= _dwSeqXbToSg)
            {
                _dwSeqXbToSg = 0;
                *(DWORD *)_abXbToSgPulse = 0;
            }

            cb -= sizeof(CSecMsgSgToXbPulse);

            if (cb > 0)
            {
                BYTE * pb = (BYTE *)(pSecMsgSgToXbPulse + 1);

                while (cb > 0)
                {
                    BYTE b = *pb++;
                    cb -= 1;

                    if ((b & ~(SGPULSE_USER_INDEX_MASK|SGPULSE_QFLAGS_CHANGE)) != 0)
                    {
                        TraceSz2(pktWarn, "SecMsg SECMSG_TYPE_SGTOXB_PULSE to %s has bad control byte (%02X)",
                                 pSecReg->Str(), b);
                        break;
                    }

                    if (b & SGPULSE_QFLAGS_CHANGE)
                    {
                        if (cb < 2*sizeof(DWORD))
                        {
                            TraceSz3(pktWarn, "SecMsg SECMSG_TYPE_SGTOXB_PULSE to %s ends prematurely (cb=%d,cbReq=%d)",
                                     pSecReg->Str(), cb, 2*sizeof(DWORD));
                            break;
                        }

                        DWORD dwQFlags = *(DWORD *)pb;
                        pb += sizeof(DWORD);
                        cb -= sizeof(DWORD);

                        DWORD dwSeqQFlags = *(DWORD *)pb;
                        pb += sizeof(DWORD);
                        cb -= sizeof(DWORD);

                        XOUSERINFO * pxouserinfo = &_axouserinfo[b & SGPULSE_USER_INDEX_MASK];

                        if (pxouserinfo->_dwSeqQFlags < dwSeqQFlags)
                        {
                            pxouserinfo->_dwSeqQFlags = dwSeqQFlags;
                            pxouserinfo->_dwQFlags    = dwQFlags;

                            if (_pEventLogon)
                            {
                                EvtSet(_pEventLogon, EVENT_INCREMENT);
                            }
                        }
                    }
                }

                _dwSeqSgToXb = dwSeq;
            }
        }

        return;
    }

    if (pSecMsgHdr->_wType == SECMSG_TYPE_XBTOXB_FORWARD)
    {
        if (cb < sizeof(CSecMsgXbToXbForward))
        {
            TraceSz2(pktWarn, "[DISCARD] SecMsg SECMSG_TYPE_XBTOXB_FORWARD to %s has incorrect payload size (%d)",
                     pSecReg->Str(), cb);
            return;
        }

        if (pSecReg != _pSecRegLogon)
        {
            TraceSz4(pktWarn, "[DISCARD] SecMsg SECMSG_TYPE_XBTOXB_FORWARD to %s received %s%s",
                     pSecReg->Str(), _pSecRegLogon ? "from wrong SG (expected " : "while offline",
                     _pSecRegLogon ? _pSecRegLogon->Str() : "", _pSecRegLogon ? ")" : "");
            return;
        }

        CSecMsgXbToXbForward * pSecMsgXbToXbForward = (CSecMsgXbToXbForward *)pSecMsgHdr;

        Assert(pSecReg->TestFlags(SRF_ONLINESERVER));
        Assert(memcmp(&pSecMsgXbToXbForward->_sgaddr, &pSecReg->_sgaddr, sizeof(SGADDR)) == 0);

        IpRecvKeyEx(ppkt, 0, 0, (CKeyExHdr *)(pSecMsgXbToXbForward + 1), cb - sizeof(CSecMsgXbToXbForward));
        return;
    }

#endif

    TraceSz3(pktWarn, "[DISCARD] SecMsg to %s has unrecognized type (%d, cb=%d)",
             pSecReg->Str(), pSecMsgHdr->_wType, pSecMsgHdr->_cbEnt - sizeof(CSecMsgHdr));
    return;
}

// ---------------------------------------------------------------------------------------
// CXnIp (Xmit)
// ---------------------------------------------------------------------------------------

void CXnIp::IpXmit(CPacket * ppkt, CRouteEntry ** pprte)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(ppkt->IsIp());

    ppkt->Validate();

    CIpHdr * pIpHdr = ppkt->GetIpHdr();
    CIpAddr  ipaDst = pIpHdr->_ipaDst;

    if (ipaDst.IsLoopback() || ipaDst == _ipa)
    {
        Assert(!ppkt->IsEsp());
        pIpHdr->_ipaSrc = ipaDst;
        IpXmitIp(ppkt, pprte);
        return;
    }

    if (ppkt->IsEsp())
    {
        if (ipaDst.IsBroadcast())
        {
            IpXmitEsp(ppkt, NULL, NULL);
            return;
        }

        CSecReg * pSecReg = SecRegLookup(ipaDst);
        
        if (pSecReg == NULL)
        {
            TraceSz1(pktWarn, "[DISCARD] %s is not a registered secure address", ipaDst.Str());
            EnetXmit(ppkt, 0);
            return;
        }

        if (pSecReg->IsXmitReady())
        {
            IpXmitEsp(ppkt, pSecReg, pprte);
            return;
        }

#ifdef XNET_FEATURE_SG

        if (pSecReg->TestFlags(SRF_ONLINESERVER) && _uiLogonState != XN_LOGON_STATE_ONLINE)
        {
            TraceSz2(pktWarn, "[DISCARD] Secure packet to %s %s", ipaDst.Str(),
                     _uiLogonState == XN_LOGON_STATE_IDLE ? "before XOnlineLogon" :
                     _uiLogonState == XN_LOGON_STATE_PENDING ? "before XOnlineLogon has connected" :
                     "after XOnlineLogon connection lost");
            EnetXmit(ppkt, 0);
            return;
        }

#endif

        SecRegEnqueue(pSecReg, ppkt);
        return;
    }

#ifdef XNET_FEATURE_INSECURE
    if (cfgFlags & XNET_STARTUP_BYPASS_SECURITY)
    {
        ppkt->SetFlags(PKTF_XMIT_INSECURE);
    }
#endif

#if defined(XNET_FEATURE_INSECURE) || defined(XNET_FEATURE_ICMP) || defined(XNET_FEATURE_DHCP) || defined(XNET_FEATURE_DNS) || defined(XNET_FEATURE_ONLINE)

    if (ppkt->TestFlags(PKTF_XMIT_INSECURE))
    {
        pIpHdr->_ipaSrc = _ipa;
        IpXmitIp(ppkt, pprte);
        return;
    }

#endif

    TraceSz1(pktWarn, "[DISCARD] %s is not a registered secure address", ipaDst.Str());
    EnetXmit(ppkt, 0);
}

void CXnIp::IpXmitEsp(CPacket * ppkt, CSecReg * pSecReg, CRouteEntry ** pprte)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(ppkt->IsIp());

    ppkt->Validate();

    CIpHdr *    pIpHdr   = ppkt->GetIpHdr();
    CEspHdr *   pEspHdr  = ppkt->GetEspHdr();
    CEspTail *  pEspTail = ppkt->GetEspTail();
    CEnetHdr *  pEnetHdr = ppkt->GetEnetHdr();
    BYTE *      pbKeySha;
    UINT        cbKeySha;
    BYTE *      pbKeyDes;
    UINT        cbKeyDes;
    BYTE *      pb;
    UINT        cb;

    if (pSecReg == NULL)
    {
        ppkt->SetFlags(PKTF_XMIT_FRAME);

        pEnetHdr->_eaSrc = _ea;
        pEnetHdr->_eaDst.SetBroadcast();
        pEnetHdr->_wType = ENET_TYPE_IP;

        pIpHdr->_ipaSrc = IPADDR_SECURE_DEFAULT;
        Assert(pIpHdr->_ipaDst.IsBroadcast());

        pEspHdr->_ipportDst = ESPUDP_CLIENT_PORT;
        pEspHdr->_dwSpi     = 0xFFFFFFFF;
        pEspHdr->_dwSeq     = 0xFFFFFFFF;

        pbKeySha = _abKeyShaLan;
        cbKeySha = sizeof(_abKeyShaLan);
        pbKeyDes = _abKeyDesLan;
        cbKeyDes = sizeof(_abKeyDesLan);
    }
    else
    {
#ifdef XNET_FEATURE_SG
        if (pSecReg->TestFlags(SRF_ONLINESERVER|SRF_ONLINEPEER))
        {
            pIpHdr->_ipaSrc = _ipa;
        }
        else
#endif
        {
            pIpHdr->_ipaSrc = IPADDR_SECURE_DEFAULT;

            ppkt->SetFlags(PKTF_XMIT_FRAME);

            pEnetHdr->_eaDst = *(CEnetAddr *)pSecReg->_xnaddr.abEnet;
            pEnetHdr->_eaSrc = _ea;
            pEnetHdr->_wType = ENET_TYPE_IP;
        }

        Assert(pSecReg->_ipaDst != 0 && pSecReg->_ipportDst != 0);

        pIpHdr->_ipaDst     = pSecReg->_ipaDst;
        pEspHdr->_ipportDst = pSecReg->_ipportDst;
        pEspHdr->_dwSpi     = pSecReg->_dwSpiXmit;
        pEspHdr->_dwSeq     = HTONL(++pSecReg->_dwSeqXmit);

        pbKeySha = pSecReg->_abKeyShaXmit;
        cbKeySha = sizeof(pSecReg->_abKeyShaXmit);
        pbKeyDes = pSecReg->_abKeyDesXmit;
        cbKeyDes = pSecReg->_cbKeyDesXmit;

        pSecReg->_dwTickXmit = TimerTick();
    }

    Assert(!!(cbKeyDes != 0) == !!ppkt->IsCrypt());
    Assert(pIpHdr->GetOptLen() == 0);

    pb = (BYTE *)(pEspHdr + 1);
    cb = ppkt->GetCb() - sizeof(CIpHdr) - sizeof(CEspHdr) - sizeof(pEspTail->_abHash);

    Assert(cb == ROUNDUP4(cb));

    ppkt->Validate();

    pEspTail->_bNextHeader = pIpHdr->_bProtocol;
    pIpHdr->_bProtocol     = IPPROTOCOL_UDP;
    pEspHdr->_ipportSrc    = ESPUDP_CLIENT_PORT;
    pEspHdr->_wLen         = HTONS(ppkt->GetCb() - sizeof(CIpHdr));

    if (ppkt->IsCrypt())
    {
        // Encrypt the packet from just after the [ESP] header to just before the _abHash in [ESPT]

        if (pSecReg == NULL)
            Rand(pb, XC_SERVICE_DES_BLOCKLEN);
        else
            memcpy(pb, pSecReg->_abIv, XC_SERVICE_DES_BLOCKLEN);

        Assert(cb == ROUNDUP8(cb));

        CryptDes(XC_SERVICE_ENCRYPT, pbKeyDes, cbKeyDes,
                 pb, pb + XC_SERVICE_DES_BLOCKLEN, cb - XC_SERVICE_DES_BLOCKLEN);

        if (pSecReg)
        {
            // Remember the last DES block to initialize the next packet's IV.  This logically
            // extends the cipher-block-chaining and prevents us from having to call Rand
            // for each packet.

            memcpy(pSecReg->_abIv, pb + cb - XC_SERVICE_DES_BLOCKLEN, XC_SERVICE_DES_BLOCKLEN);
        }
    }

    // Authenicate the packet from the [ESP] header to just before the _abHash in [ESPT]

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];
    Assert(sizeof(pEspTail->_abHash) <= sizeof(abHash));

    XcHMAC(pbKeySha, cbKeySha, (BYTE *)&pEspHdr->_dwSpi, (sizeof(CEspHdr) - sizeof(CUdpHdr)) + cb,
           NULL, 0, abHash);
    memcpy(pEspTail->_abHash, abHash, sizeof(pEspTail->_abHash));

    TraceSz4(pktXmit, "[ESP %s #%d]%s[%d][ESPT]",
             CIpAddr(pEspHdr->_dwSpi).Str(), NTOHL(pEspHdr->_dwSeq),
             ppkt->TestFlags(PKTF_CRYPT) ? "[IV]" : "",
             cb - offsetof(CEspTail, _abHash) - (ppkt->TestFlags(PKTF_CRYPT) ? XC_SERVICE_DES_BLOCKLEN : 0));

    IpXmitIp(ppkt, pprte);
}

void CXnIp::IpXmitKeyEx(CSecReg * pSecReg)
{
    if (pSecReg->_bState == SR_STATE_IDLE)
    {
        pSecReg->_bState = SR_STATE_INITSENT;
        pSecReg->_bRetry = SecRegRexmitRetries(pSecReg);
        SecRegSetTicks(pSecReg);
        Rand(pSecReg->_abNonceInit, sizeof(pSecReg->_abNonceInit));
        TimerSet(&pSecReg->_timer, TimerTick() + SecRegRexmitTimeoutInSeconds(pSecReg) * TICKS_PER_SECOND);
    }

#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINESERVER))
    {
        IpXmitKeyExXbToSg(pSecReg);
        return;
    }
#endif

    IpXmitKeyExXbToXb(pSecReg);
}

void CXnIp::IpXmitKeyExXbToXb(CSecReg * pSecReg, BOOL fInhibitNatOpen)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(pSecReg->_bState == SR_STATE_INITSENT || pSecReg->_bState == SR_STATE_RESPSENT);
    Assert(pSecReg->_bState != SR_STATE_INITSENT || pSecReg->TestFlags(SRF_OWNED));

#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINEPEER) && _uiLogonState != XN_LOGON_STATE_ONLINE)
    {
        TraceSz2(pktWarn, "[DISCARD] KeyExXbToXb%s to online-peer %s while offline",
                 pSecReg->_bState == SR_STATE_INITSENT ? "Init" : "Resp", pSecReg->Str());
        return;
    }
#endif

    UINT cbKeyEx = sizeof(CKeyExXbToXb) + sizeof(CKeyExHdr) + CBDHG1 + sizeof(CKeyExHdr) + XC_SERVICE_DIGEST_SIZE;

    CPacket * ppkt = PacketAlloc(PTAG_CKeyExPacket, PKTF_TYPE_UDP|PKTF_POOLALLOC, sizeof(DWORD) + cbKeyEx);

    if (ppkt == NULL)
    {
        TraceSz(Warning, "IpXmitKeyExXbToXb - Out of memory allocating packet");
        return;
    }

    CEnetHdr *  pEnetHdr  = ppkt->GetEnetHdr();
    CIpHdr *    pIpHdr    = ppkt->GetIpHdr();
    CEspHdr *   pEspHdr   = (CEspHdr *)(pIpHdr + 1);

    IpFillHdr(ppkt, 0, IPPROTOCOL_UDP);

#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINEPEER))
    {
        pIpHdr->_ipaSrc = _ipa;

        if (pSecReg->_ipaDst != 0)
        {
            // We've received a packet directly from the other side at least once, so we'll
            // send the response back there.

            Assert(pSecReg->_ipaDst != 0 && pSecReg->_ipportDst != 0);

            pIpHdr->_ipaDst     = pSecReg->_ipaDst;
            pEspHdr->_ipportDst = pSecReg->_ipportDst;
        }
        else if (pSecReg->_xnaddr.inaOnline.s_addr == _pSecRegLogon->_ipaNat)
        {
            // The other side has the same IP address as this side from the perspective
            // of the security gateways.  This implies that the two sides are behind the same
            // NAT, so we should address packets to the inner IP address behind the NAT (i.e.
            // the actual IP address of the xbox).

            Assert(pSecReg->_xnaddr.ina.s_addr != 0);

            pIpHdr->_ipaDst     = pSecReg->_xnaddr.ina.s_addr;
            pEspHdr->_ipportDst = ESPUDP_CLIENT_PORT;
        }
        else
        {
            // Our best guess is to send a packet to the return IP address and port that the
            // security gateway the other side is connected to is reporting as its return address.
            // Depending on how strict a NAT device is in front of the xbox, if any, this
            // packet may or may not make it through.

            Assert(pSecReg->_xnaddr.inaOnline.s_addr != 0 && pSecReg->_xnaddr.wPortOnline != 0);

            pIpHdr->_ipaDst     = pSecReg->_xnaddr.inaOnline.s_addr;
            pEspHdr->_ipportDst = pSecReg->_xnaddr.wPortOnline;
        }

        if (    pSecReg->_bState == SR_STATE_INITSENT
            &&  pSecReg->_bRetry < cfgKeyExXbToXbRexmitRetries
            &&  pSecReg->_xnaddr.inaOnline.s_addr != _pSecRegLogon->_ipaNat
            && !fInhibitNatOpen)
        {
            // This is a retransmission of a key exchange initiator.  In case the other side
            // is behind a NAT (other than ours) that is not letting our packet through,
            // we'll transmit a CKeyExNatOpen message via the SG forwarding mechanism.  This
            // is guaranteed to arrive because every xbox that is online is maintaining a
            // bidirectional connection to a security gateway, and all security gateways have
            // direct access to each other.

            CKeyReg *       pKeyReg = pSecReg->_pKeyReg;
            CKeyExNatOpen   KeyExNatOpen;

            KeyExNatOpen._wType = KEYEX_TYPE_NATOPEN_INIT;
            KeyExNatOpen._cbEnt = sizeof(CKeyExNatOpen);
            KeyExNatOpen._xnkid = pSecReg->_pKeyReg->_xnkid;
            KeyExNatOpen._dwCtx = pSecReg->_dwSpiRecv;

            Assert(sizeof(KeyExNatOpen._abNonce) == sizeof(pSecReg->_abNonceInit));
            memcpy(KeyExNatOpen._abNonce, pSecReg->_abNonceInit, sizeof(KeyExNatOpen._abNonce));

            KeyExNatOpen._ipaDst    = _pSecRegLogon->_ipaNat;
            KeyExNatOpen._ipportDst = _pSecRegLogon->_ipportNat;
            KeyExNatOpen._wFlags    = KNOF_XBTOXB_KEYEX;

            Assert(sizeof(KeyExNatOpen._abHash) == XC_SERVICE_DIGEST_SIZE);
            XcHMAC(pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha), (BYTE *)&KeyExNatOpen,
                   offsetof(CKeyExNatOpen, _abHash), NULL, 0, KeyExNatOpen._abHash);

            TraceSz3(secStat, "Sending KeyExNatOpen INIT/%04X packet to %s via SGADDR %s",
                     KeyExNatOpen._wFlags, pSecReg->Str(),
                     HexStr(pSecReg->_xnaddr.abOnline, sizeof(pSecReg->_xnaddr.abOnline)));

            IpXmitSecMsg(_pSecRegLogon, SECMSG_TYPE_XBTOXB_FORWARD,
                         pSecReg->_xnaddr.abOnline, sizeof(pSecReg->_xnaddr.abOnline),
                         &KeyExNatOpen, sizeof(CKeyExNatOpen));
        }
    }
    else
#endif
    {
        ppkt->SetFlags(PKTF_XMIT_FRAME);

        pEnetHdr->_eaDst    = *(CEnetAddr *)pSecReg->_xnaddr.abEnet;
        pEnetHdr->_eaSrc    = _ea;
        pEnetHdr->_wType    = ENET_TYPE_IP;

        pIpHdr->_ipaSrc     = IPADDR_SECURE_DEFAULT;
        pIpHdr->_ipaDst     = IPADDR_SECURE_DEFAULT;
        pEspHdr->_ipportDst = ESPUDP_CLIENT_PORT;
    }

    // Fill in the UDP header plus the SPI = 0 indicator of the ESP header

    pEspHdr->_wLen      = NTOHS(sizeof(CUdpHdr) + sizeof(DWORD) + cbKeyEx);
    pEspHdr->_ipportSrc = ESPUDP_CLIENT_PORT;
    pEspHdr->_wChecksum = 0;
    pEspHdr->_dwSpi     = 0;

    IpFillKeyExXbToXb(pSecReg, (CKeyExXbToXb *)&pEspHdr->_dwSeq);

    TraceSz4(secStat, "Sending KeyExXbToXb%s packet to %s via %s:%d",
             pSecReg->_bState == SR_STATE_INITSENT ? "Init" : "Resp", pSecReg->Str(),
             pIpHdr->_ipaDst.Str(), NTOHS(pEspHdr->_ipportDst));

    IpXmitIp(ppkt, NULL);
}

void CXnIp::IpFillKeyExXbToXb(CSecReg * pSecReg, CKeyExXbToXb * pKeyExXbToXb)
{
    memset(pKeyExXbToXb, 0, sizeof(CKeyExXbToXb));

    CKeyReg * pKeyReg = pSecReg->_pKeyReg;

    pKeyExXbToXb->_cbEnt   = sizeof(CKeyExXbToXb);
    pKeyExXbToXb->_xnkid   = pKeyReg->_xnkid;
    pKeyExXbToXb->_liTime  = _liTime;
    _liTime.QuadPart      += 1;

    if (pSecReg->_bState == SR_STATE_INITSENT)
    {
        pKeyExXbToXb->_wType     = KEYEX_TYPE_XBTOXB_INIT;
        pKeyExXbToXb->_dwSpiInit = pSecReg->_dwSpiRecv;

        Assert(sizeof(pKeyExXbToXb->_abNonceInit) == sizeof(pSecReg->_abNonceInit));
        memcpy(pKeyExXbToXb->_abNonceInit, pSecReg->_abNonceInit, sizeof(pKeyExXbToXb->_abNonceInit));

        IpGetXnAddr(&pKeyExXbToXb->_xnaddrInit);
        pKeyExXbToXb->_xnaddrResp = pSecReg->_xnaddr;
    }
    else
    {
        pKeyExXbToXb->_wType     = KEYEX_TYPE_XBTOXB_RESP;
        pKeyExXbToXb->_dwSpiInit = pSecReg->_dwSpiXmit;
        pKeyExXbToXb->_dwSpiResp = pSecReg->_dwSpiRecv;

        Assert(sizeof(pKeyExXbToXb->_abNonceInit) == sizeof(pSecReg->_abNonceInit));
        memcpy(pKeyExXbToXb->_abNonceInit, pSecReg->_abNonceInit, sizeof(pKeyExXbToXb->_abNonceInit));
        Assert(sizeof(pKeyExXbToXb->_abNonceResp) == sizeof(pSecReg->_abNonceResp));
        memcpy(pKeyExXbToXb->_abNonceResp, pSecReg->_abNonceResp, sizeof(pKeyExXbToXb->_abNonceResp));

        pKeyExXbToXb->_xnaddrInit = pSecReg->_xnaddr;
        IpGetXnAddr(&pKeyExXbToXb->_xnaddrResp);
    }

    // Encrypt the portion of this entry which contains the XNADDR structures to prevent
    // observers from seeing any addressing information

    Rand(pKeyExXbToXb->_abIv, sizeof(pKeyExXbToXb->_abIv));
    CryptDes(XC_SERVICE_ENCRYPT, pKeyReg->_abKeyDes, sizeof(pKeyReg->_abKeyDes),
             pKeyExXbToXb->_abIv, pKeyExXbToXb->_abIv + XC_SERVICE_DES_BLOCKLEN,
             sizeof(CKeyExXbToXb) - offsetof(CKeyExXbToXb, _abIv) - XC_SERVICE_DES_BLOCKLEN);

    // Fill in the DH key exchange entry next

    CKeyExHdr * pKeyExDh = (CKeyExHdr *)((BYTE *)pKeyExXbToXb + pKeyExXbToXb->_cbEnt);

    pKeyExDh->_wType = KEYEX_TYPE_DH_GX;
    pKeyExDh->_cbEnt = sizeof(CKeyExHdr) + CBDHG1;

    Assert(sizeof(pKeyReg->_abDhGX) == CBDHG1);
    memcpy(pKeyExDh + 1, pKeyReg->_abDhGX, CBDHG1);

    // Fill in the HMAC_SHA key exchange entry

    CKeyExHdr * pKeyExSha = (CKeyExHdr *)((BYTE *)pKeyExDh + pKeyExDh->_cbEnt);

    pKeyExSha->_wType = KEYEX_TYPE_HMAC_SHA;
    pKeyExSha->_cbEnt = sizeof(CKeyExHdr) + XC_SERVICE_DIGEST_SIZE;
    XcHMAC(pKeyReg->_abKeySha, sizeof(pKeyReg->_abKeySha),
           (BYTE *)pKeyExXbToXb, (BYTE *)pKeyExSha - (BYTE *)pKeyExXbToXb,
           NULL, 0, (BYTE *)(pKeyExSha + 1));
}

#ifdef XNET_FEATURE_SG

void CXnIp::IpXmitKeyExXbToSg(CSecReg * pSecReg)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(pSecReg->_bState == SR_STATE_INITSENT);

    BYTE *  pbKeyHmac;
    UINT    cbKeyHmac;
    BYTE *  pbDhGX;
    UINT    cbDhGX;
    UINT    cbApReqMax;
    DWORD   dwUserPerm;
    
#ifdef XNET_FEATURE_INSECURE

    if (pSecReg->_dwServiceId == 0)
    {
        pbKeyHmac  = _abKeyNull;
        cbKeyHmac  = sizeof(_abKeyNull);
        pbDhGX     = _abDhGXNull;
        cbDhGX     = sizeof(_abDhGXNull);
        cbApReqMax = sizeof(CKeyExNullApReq); 
        dwUserPerm = 0xE0A06020;

        goto ServiceDone;
    }

#endif

#ifdef XNET_FEATURE_ONLINE
    {
        XOKERBINFO * pxokerbinfo = _pXoBase ? _pXoBase->XoKerbGetInfo(pSecReg->_dwServiceId) : NULL;

        if (pxokerbinfo != NULL)
        {
            pbKeyHmac  = pxokerbinfo->_abKey;
            cbKeyHmac  = sizeof(pxokerbinfo->_abKey);
            pbDhGX     = pxokerbinfo->_abDhGX;
            cbDhGX     = sizeof(pxokerbinfo->_abDhGX);
            cbApReqMax = sizeof(CKeyExHdr) + pxokerbinfo->_cbApReqMax;
            dwUserPerm = pxokerbinfo->_dwUserPerm;
            
            goto ServiceDone;
        }
    }

#endif

    TraceSz2(Warning, "IpXmitKeyExXbToSgInit to %s service %08lX is invalid",
             pSecReg->Str(), pSecReg->_dwServiceId);
    return;

ServiceDone:

    UINT cbKeyEx = sizeof(CKeyExXbToSgInit) + sizeof(CKeyExHdr) + CBDHG1 + cbApReqMax;

    CPacket * ppkt = PacketAlloc(PTAG_CKeyExPacket, PKTF_TYPE_UDP|PKTF_POOLALLOC, sizeof(DWORD) + cbKeyEx);

    if (ppkt == NULL)
    {
        TraceSz1(Warning, "IpXmitKeyExXbToSgInit to %s - Out of memory allocating packet", pSecReg->Str());
        return;
    }

    CIpHdr *    pIpHdr    = ppkt->GetIpHdr();
    CEspHdr *   pEspHdr   = (CEspHdr *)(pIpHdr + 1);
    CKeyExHdr * pKeyExHdr = (CKeyExHdr *)&pEspHdr->_dwSeq;

    // Fill in the IP header

    IpFillHdr(ppkt, pSecReg->_ipaDst, IPPROTOCOL_UDP);
    pIpHdr->_ipaSrc = _ipa;

    // Fill in the UDP header plus the SPI = 0 indicator of the ESP header

    pEspHdr->_ipportDst = ESPUDP_CLIENT_PORT;
    pEspHdr->_ipportSrc = ESPUDP_CLIENT_PORT;
    pEspHdr->_wChecksum = 0;
    pEspHdr->_dwSpi     = 0;

    // Fill in the first key exchange entry

    CKeyExXbToSgInit * pKeyExXbToSgInit = (CKeyExXbToSgInit *)pKeyExHdr;

    pKeyExXbToSgInit->_wType   = KEYEX_TYPE_XBTOSG_INIT;
    pKeyExXbToSgInit->_cbEnt   = sizeof(CKeyExXbToSgInit);
    pKeyExXbToSgInit->_dwFlags = 0;

#ifdef XNET_FEATURE_SG
    if (_pSecRegLogon == pSecReg)
    {
        pKeyExXbToSgInit->_dwFlags |= XSIF_CONNECTION_SERVICE;
    }
#endif

    pKeyExXbToSgInit->_dwSpiInit = pSecReg->_dwSpiRecv;
    pKeyExXbToSgInit->_dwUserPerm = dwUserPerm;

    Assert(sizeof(pKeyExXbToSgInit->_abNonceInit) == sizeof(pSecReg->_abNonceInit));
    memcpy(pKeyExXbToSgInit->_abNonceInit, pSecReg->_abNonceInit, sizeof(pKeyExXbToSgInit->_abNonceInit));

    // Fill in the DH key exchange entry next

    CKeyExHdr * pKeyExDh = (CKeyExHdr *)((BYTE *)pKeyExHdr + pKeyExHdr->_cbEnt);

    pKeyExDh->_wType = KEYEX_TYPE_DH_GX;
    pKeyExDh->_cbEnt = sizeof(CKeyExHdr) + CBDHG1;

    Assert(cbDhGX == CBDHG1);
    memcpy(pKeyExDh + 1, pbDhGX, CBDHG1);

    // Fill in the APREQ key exchange entry

    CKeyExHdr * pKeyExApReq = (CKeyExHdr *)((BYTE *)pKeyExDh + pKeyExDh->_cbEnt);

    BYTE abSha[XC_SERVICE_DIGEST_SIZE];

    XcHMAC(pbKeyHmac, cbKeyHmac, (BYTE *)pKeyExXbToSgInit, (BYTE *)pKeyExApReq - (BYTE *)pKeyExXbToSgInit,
           NULL, 0, abSha);

#ifdef XNET_FEATURE_INSECURE

    if (pSecReg->_dwServiceId == 0)
    {
        CKeyExNullApReq * pKeyExNullApReq = (CKeyExNullApReq *)pKeyExApReq;

        pKeyExNullApReq->_wType  = KEYEX_TYPE_NULL_APREQ;
        pKeyExNullApReq->_cbEnt  = sizeof(CKeyExNullApReq);
        pKeyExNullApReq->_liTime = _liTime;
        _liTime.QuadPart += 1;

        Assert(sizeof(pKeyExNullApReq->_abSha) == sizeof(abSha));
        memcpy(pKeyExNullApReq->_abSha, abSha, sizeof(pKeyExNullApReq->_abSha));

        memset(&pKeyExNullApReq->_AuthData, 0, sizeof(pKeyExNullApReq->_AuthData));
        pKeyExNullApReq->_AuthData.SetCb(sizeof(pKeyExNullApReq->_AuthData));
        pKeyExNullApReq->_AuthData.wAuthDataVersion = XONLINE_AUTHDATA_VERSION;
        pKeyExNullApReq->_AuthData.dwAuthDataSize = sizeof(XKERB_AD_XBOX);

        #ifdef XNET_FEATURE_ONLINE
        if (_pXoBase)
        {
            _pXoBase->XoKerbGetAuthData(&pKeyExNullApReq->_AuthData);
        }
        else
        #endif
        {
            memset(&pKeyExNullApReq->_AuthData.qwXboxID, 0xAA, sizeof(pKeyExNullApReq->_AuthData.qwXboxID));
            memcpy(&pKeyExNullApReq->_AuthData.qwXboxID, _ea._ab, sizeof(_ea));

            XOUSERINFO * pxouserinfo    = _axouserinfo;
            XOUSERINFO * pxouserinfoEnd = _axouserinfo + dimensionof(_axouserinfo);
            XUID *       pxuid          = pKeyExNullApReq->_AuthData.users;

            Assert(dimensionof(_axouserinfo) == dimensionof(pKeyExNullApReq->_AuthData.users));

            for (; pxouserinfo < pxouserinfoEnd; ++pxouserinfo, ++pxuid)
            {
                pxuid->qwUserID = pxouserinfo->_qwUserId;
            }
        }

        goto ApReqDone;
    }

#endif

#ifdef XNET_FEATURE_ONLINE

    {
        UINT cbApReq = cbApReqMax - sizeof(CKeyExHdr);

        if (!_pXoBase->XoKerbBuildApReq(pSecReg->_dwServiceId, &pSecReg->_liTime, abSha, sizeof(abSha),
                                    (BYTE *)(pKeyExApReq + 1), &cbApReq))
        {
            TraceSz1(Warning, "IpXmitKeyExXbToSgInit to %s - XoKerbBuildApReq failed", pSecReg->Str());
            return;
        }

        pKeyExApReq->_wType = KEYEX_TYPE_KERB_APREQ;
        pKeyExApReq->_cbEnt = sizeof(CKeyExHdr) + cbApReq;
        goto ApReqDone;
    }

#endif

ApReqDone:

    // Shrink the size of the packet if the ApReq is smaller than the space allocated

    Assert(cbApReqMax >= pKeyExApReq->_cbEnt);

    UINT cbExtra = cbApReqMax - pKeyExApReq->_cbEnt;
    ppkt->SetCb(ppkt->GetCb() - cbExtra);
    pEspHdr->_wLen = HTONS(sizeof(CUdpHdr) + sizeof(DWORD) + cbKeyEx - cbExtra);
    pIpHdr->_wLen = HTONS(pIpHdr->GetLen() - cbExtra);

    TraceSz3(secStat, "Sending KeyExXbToSgInit packet to %s via %s:%d",
             pSecReg->Str(), pIpHdr->_ipaDst.Str(), NTOHS(pEspHdr->_ipportDst));

    IpXmitIp(ppkt, NULL);
}

#endif

void CXnIp::IpXmitSecMsg(CSecReg * pSecReg, WORD wType, void * pv1, UINT cb1, void * pv2, UINT cb2)
{
    ICHECK(IP, UDPC|SDPC);

    TraceSz3(secStat, "Sending SecMsg packet to %s (%04X, cb=%d)", pSecReg->Str(), wType, cb1+cb2);

    CPacket * ppkt = PacketAlloc(PTAG_CSecMsgPacket, PKTF_POOLALLOC|PKTF_TYPE_ESP|PKTF_CRYPT, sizeof(CSecMsgHdr) + cb1 + cb2);

    if (ppkt == NULL)
    {
        TraceSz(Warning, "IpXmitSecMsg - Out of memory allocating packet");
        return;
    }

    // Fill in the IP header

    IpFillHdr(ppkt, pSecReg->_dwSpiRecv, IPPROTOCOL_SECMSG);

    // Fill in the payload

    CSecMsgHdr * pSecMsgHdr = (CSecMsgHdr *)((BYTE *)ppkt->GetEspHdr() + sizeof(CEspHdr) + XC_SERVICE_DES_BLOCKLEN);

    pSecMsgHdr->_wType = wType;
    pSecMsgHdr->_cbEnt = sizeof(CSecMsgHdr) + cb1 + cb2;

    if (cb1 > 0)
    {
        memcpy(pSecMsgHdr + 1, pv1, cb1);
    }

    if (cb2 > 0)
    {
        memcpy((BYTE *)(pSecMsgHdr + 1) + cb1, pv2, cb2);
    }

    TraceSz2(pktXmit, "[SecMsg %04X]{+%d}", wType, cb1 + cb2);

    IpXmitEsp(ppkt, pSecReg, NULL);
}

void CXnIp::IpXmitSecMsgDelete(CSecReg * pSecReg, DWORD dwReason)
{
    ICHECK(IP, UDPC|SDPC);

    IpXmitSecMsg(pSecReg, SECMSG_TYPE_DELETE, &dwReason, sizeof(dwReason));
}

#ifdef XNET_FEATURE_SG

void CXnIp::IpXmitSecMsgXbToSgPulse(CSecReg * pSecReg)
{
    ICHECK(IP, UDPC|SDPC);

    BYTE                    ab[sizeof(CSecMsgXbToSgPulse) + dimensionof(_axouserinfo) * (1 + sizeof(DWORD) + sizeof(XNKID) + 1 + sizeof(_axouserinfo[0]._abData))];
    DWORD                   dwSeqXmit           = pSecReg->_dwSeqXmit;
    CSecMsgXbToSgPulse *    pSecMsgXbToSgPulse  = (CSecMsgXbToSgPulse *)ab;
    BYTE *                  pb                  = &ab[sizeof(CSecMsgXbToSgPulse)];

    if (pSecReg == _pSecRegLogon)
    {
        pSecMsgXbToSgPulse->_dwSeqAck = _dwSeqSgToXb;

        if (*(DWORD *)_abXbToSgPulse != 0)
        {
            XOUSERINFO * pxouserinfo    = _axouserinfo;
            XOUSERINFO * pxouserinfoEnd = _axouserinfo + dimensionof(_axouserinfo);
            BYTE *       pbXbToSgPulse  = _abXbToSgPulse;

            for (; pxouserinfo < pxouserinfoEnd; ++pxouserinfo, ++pbXbToSgPulse)
            {
                BYTE b = *pbXbToSgPulse;

                if (b != 0)
                {
                    Assert((b &  (XBPULSE_USER_INDEX_MASK)) == pxouserinfo - _axouserinfo);
                    Assert((b & ~(XBPULSE_USER_INDEX_MASK|XBPULSE_STATE_CHANGE|XBPULSE_XNKID_CHANGE|XBPULSE_TDATA_CHANGE)) == 0);
                    Assert((b &  (XBPULSE_STATE_CHANGE|XBPULSE_XNKID_CHANGE|XBPULSE_TDATA_CHANGE)) != 0);

                    *pb++ = b;

                    if (b & XBPULSE_STATE_CHANGE)
                    {
                        *(DWORD *)pb = pxouserinfo->_dwPState;
                        pb += sizeof(DWORD);
                    }

                    if (b & XBPULSE_XNKID_CHANGE)
                    {
                        *(XNKID *)pb = pxouserinfo->_xnkid;
                        pb += sizeof(XNKID);
                    }

                    if (b & XBPULSE_TDATA_CHANGE)
                    {
                        Assert(pxouserinfo->_cbData <= sizeof(pxouserinfo->_abData));

                        *pb++ = (BYTE)pxouserinfo->_cbData;

                        if (pxouserinfo->_cbData > 0)
                        {
                            memcpy(pb, pxouserinfo->_abData, pxouserinfo->_cbData);
                            pb += pxouserinfo->_cbData;
                        }
                    }
                }
            }
        }
    }
    else
    {
        pSecMsgXbToSgPulse->_dwSeqAck = 0;
    }

    Assert(pb - ab <= sizeof(ab));

    IpXmitSecMsg(pSecReg, SECMSG_TYPE_XBTOSG_PULSE, &ab[sizeof(CSecMsgHdr)], pb - ab - sizeof(CSecMsgHdr));

    if (pSecReg->_dwSeqXmit > dwSeqXmit)
    {
        Assert(pSecReg->_dwSeqXmit == dwSeqXmit + 1);

        pSecReg->_dwTickPulse = TimerTick();

        if (pSecReg == _pSecRegLogon)
        {
            _dwSeqSgToXb = 0;

            if (*(DWORD *)_abXbToSgPulse != 0)
            {
                _dwSeqXbToSg = pSecReg->_dwSeqXmit;
            }
        }
    }
}

#endif

void CXnIp::IpXmitIp(CPacket * ppkt, CRouteEntry ** pprte)
{
    ICHECK(IP, UDPC|SDPC);
    Assert(ppkt->IsIp());
    
    CIpHdr *        pIpHdr   = ppkt->GetIpHdr();
    CIpAddr         ipaDst   = pIpHdr->_ipaDst;
    UINT            cbHdrLen = pIpHdr->GetHdrLen();
    UINT            cbLen    = ppkt->GetCb();
    UINT            uiChecksum;
    CPseudoHeader   ph;

    Assert(pIpHdr->_ipaSrc != 0 || _ipa == 0);
    Assert(pIpHdr->_ipaDst != 0);

    TraceIpHdr(pktXmit, pIpHdr, ppkt->GetCb() - pIpHdr->GetHdrLen());

    pIpHdr->_wChecksum = 0;
    pIpHdr->_wChecksum = (WORD)~tcpipxsum(0, pIpHdr, cbHdrLen);

    if (ppkt->IsUdp() || ppkt->IsEsp())
    {
        CUdpHdr * pUdpHdr   = (CUdpHdr *)((BYTE *)pIpHdr + cbHdrLen);
        ph._ipaSrc          = pIpHdr->_ipaSrc;
        ph._ipaDst          = ipaDst;
        ph._bZero           = 0;
        ph._bProtocol       = IPPROTOCOL_UDP;
        ph._wLen            = HTONS(cbLen - cbHdrLen);
        pUdpHdr->_wChecksum = 0;
        uiChecksum          = ~tcpipxsum(tcpipxsum(0, &ph, sizeof(ph)), pUdpHdr, cbLen - cbHdrLen);
        pUdpHdr->_wChecksum = uiChecksum - (uiChecksum == 0);
    }
#ifdef XNET_FEATURE_INSECURE
    else if (ppkt->IsTcp())
    {
        CTcpHdr * pTcpHdr   = (CTcpHdr *)((BYTE *)pIpHdr + cbHdrLen);
        ph._ipaSrc          = pIpHdr->_ipaSrc;
        ph._ipaDst          = ipaDst;
        ph._bZero           = 0;
        ph._bProtocol       = IPPROTOCOL_TCP;
        ph._wLen            = HTONS(cbLen - cbHdrLen);
        pTcpHdr->_wChecksum = 0;
        pTcpHdr->_wChecksum = ~tcpipxsum(tcpipxsum(0, &ph, sizeof(ph)), pTcpHdr, cbLen - cbHdrLen);
    }
#endif

#ifdef XNET_FEATURE_ROUTE

    if (    !ppkt->TestFlags(PKTF_XMIT_FRAME)
        &&   ipaDst != _ipa
        &&  !ipaDst.IsLoopback()
        &&  !ipaDst.IsBroadcast())
    {
        CRouteEntry * prte = pprte ? *pprte : NULL;

        Assert(!prte || prte->_ipaDst == (ipaDst & prte->_ipaMask));

        if (prte == NULL || prte->IsOrphan())
        {
            if (prte)
            {
                RouteRelease(prte);
            }

            prte = RouteLookup(ipaDst);

            if (pprte)
            {
                *pprte = prte;
            }

            if (prte == NULL)
            {
                TraceSz1(pktWarn, "[DISCARD] No route found to %s", ipaDst.Str());
                EnetXmit(ppkt, 0);
                return;
            }
        }

        if (!prte->IsLocal())
        {
            ipaDst = prte->_ipaNext;
        }

        if (prte && pprte == NULL)
        {
            RouteRelease(prte);
        }
    }

#endif

#ifdef XNET_FEATURE_TRACE
    if (Tag(tcpRetrans) && ppkt->IsTcp() && ppkt->IsEsp() && ppkt->IsCrypt() && !ppkt->TestFlags(PKTF_XMIT_PRIORITY))
    {
        // This causes encrypted TCP packets to fail on first transmit, and it used to
        // test the decrypt-before-retransmit code path.

        ppkt->ClearFlags(PKTF_XMIT_FRAME);
        ipaDst = 0;
    }
#endif

    EnetXmit(ppkt, ipaDst);
    return;
}

// --------------------------------------------------------------------------------------
// CXnIp (Xmit Helpers)
// --------------------------------------------------------------------------------------

void CXnIp::IpFillAndXmit(CPacket * ppkt, CIpAddr ipaDst, BYTE bProtocol, CRouteEntry ** pprte)
{
    ICHECK(IP, USER|UDPC|SDPC);
    IpFillHdr(ppkt, ipaDst, bProtocol);
    RaiseToDpc();
    IpXmit(ppkt, pprte);
}

void CXnIp::IpFillHdr(CPacket * ppkt, CIpAddr ipaDst, BYTE bProtocol)
{
    ICHECK(IP, USER|UDPC|SDPC);
    Assert(ppkt->IsIp());
    Assert(ppkt->GetHdrOptLen() == 0);

    CIpHdr * pIpHdr = ppkt->GetIpHdr();
    pIpHdr->SetHdrLen(sizeof(CIpHdr));
    pIpHdr->_bTos       = cfgIpDefaultTos;
    pIpHdr->_wLen       = HTONS((WORD)ppkt->GetCb());
    pIpHdr->_wId        = HTONS(GetNextDgramId());
    pIpHdr->_wFragOff   = 0;
    pIpHdr->_bTtl       = cfgIpDefaultTtl;
    pIpHdr->_bProtocol  = bProtocol;
    pIpHdr->_ipaSrc     = 0;
    pIpHdr->_ipaDst     = ipaDst;
}

// ---------------------------------------------------------------------------------------
// CXnIp (Frag)
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_FRAG

class CFragPacket : public CPacket
{
    friend class CXnIp;

    CTimer      _timer;         // reassembly timeout timer
    CIpAddr     _ipaSrc;        // source IP address
    CIpAddr     _ipaDst;        // destination IP address
    DWORD       _dwProtoId;     // protocol and datagram ID
    UINT        _cbHdrLen;      // length of reassembled datagram header
    UINT        _cbLen;         // length of reassembled datagram payload
    UINT        _cBitRecv;      // number of chunks of payload received so far
    UINT        _cBitTotal;     // total number of chunks of payload expected
    UINT        _iBitEnd;       // highest bit position filled so far
    BYTE        _abBits[1];     // vector of bit flags for which payload fragments have arrived

    INLINE void SetBit(UINT iBit)
    {
        BYTE * pb = &_abBits[iBit / 8];
        UINT iMask = (1 << (iBit % 8));
        _cBitRecv += !(*pb & iMask);
        *pb |= iMask;
    }
};

void CXnIp::FragTerm()
{
    TCHECK(UDPC);

    while (_cFrag > 0)
    {
        FragFree(_pqFrag.GetHead());
    }
}

void CXnIp::FragFree(CPacket * ppkt)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(ppkt != NULL);
    Assert(_cFrag > 0);
    Assert(_cFrag == _pqFrag.Count());

    _pqFrag.Dequeue(ppkt);
    _cFrag -= 1;

    Assert(_cFrag == _pqFrag.Count());

    TimerSet(&((CFragPacket *)ppkt)->_timer, TIMER_INFINITE);

#ifdef XNET_FEATURE_FRAG_LOOPBACK
    ppkt->ClearFlags(PKTF_RECV_LOOPBACK);
#endif

    PacketFree(ppkt);
}

void CXnIp::FragTimer(CTimer * pt)
{
    ICHECK(IP, UDPC|SDPC);

    CFragPacket * ppktFrag = (CFragPacket *)((BYTE *)pt - offsetof(CFragPacket, _timer));
    TraceSz3(Warning, "FragTimer - Packet reassembly timeout [IP %s %s (%08lX)]",
             ppktFrag->_ipaDst.Str(), ppktFrag->_ipaSrc.Str(), ppktFrag->_dwProtoId);
    FragFree(ppktFrag);
}

void CXnIp::FragRecv(CPacket * ppkt, CIpHdr * pIpHdr, UINT cbHdrLen, UINT cbLen)
{
    ICHECK(IP, UDPC|SDPC);

    CFragPacket *   ppktFrag;
    DWORD           dwProtoId;
    UINT            uiFragOff;
    BOOL            fMoreFrag;
    UINT            cbPkt;
    UINT            ibEnd;
    UINT            iBit, iBitEnd;

    uiFragOff  = NTOHS(pIpHdr->_wFragOff);
    fMoreFrag  = !!(uiFragOff & MORE_FRAGMENTS);
    uiFragOff &= (uiFragOff & FRAGOFFSET_MASK);
    dwProtoId  = (NTOHS(pIpHdr->_wId) << 16) | pIpHdr->_bProtocol;
    ppktFrag   = (CFragPacket *)_pqFrag.GetHead();

    for (; ppktFrag; ppktFrag = (CFragPacket *)ppktFrag->GetNextPkt())
    {
        if (    ppktFrag->_dwProtoId == dwProtoId
            &&  ppktFrag->_ipaSrc == pIpHdr->_ipaSrc
            &&  ppktFrag->_ipaDst == pIpHdr->_ipaDst)
            break;
    }
    
    if (ppktFrag == NULL)
    {
        if (_cFrag >= cfgIpFragMaxSimultaneous)
        {
            TraceSz(pktWarn, "[DISCARD] Too many fragmented IP datagrams needing reassembly");
            return;
        }

        cbPkt  = offsetof(CFragPacket, _abBits);
        cbPkt += ((cfgIpFragMaxPacketDiv256 * 256) / 8 + 7) / 8;
        cbPkt  = ROUNDUP4(cbPkt);

        ppktFrag = (CFragPacket *)PacketAlloc(PTAG_CFragPacket, PKTF_TYPE_ENET|PKTF_POOLALLOC,
                                              cfgIpFragMaxPacketDiv256 * 256,
                                              cbPkt, (PFNPKTFREE)FragFree);

        if (ppktFrag == NULL)
        {
            TraceSz(pktWarn, "[DISCARD] Out of memory allocating CFragPacket");
            return;
        }

#ifdef XNET_FEATURE_FRAG_LOOPBACK
        if (ppkt->TestFlags(PKTF_RECV_LOOPBACK))
        {
            ppktFrag->SetFlags(PKTF_RECV_LOOPBACK);
        }
#endif

        ppktFrag->_timer.Init((PFNTIMER)FragTimer);
        ppktFrag->_ipaSrc    = pIpHdr->_ipaSrc;
        ppktFrag->_ipaDst    = pIpHdr->_ipaDst;
        ppktFrag->_dwProtoId = dwProtoId;
        ppktFrag->_cbHdrLen  = 0;
        ppktFrag->_cbLen     = 0;
        ppktFrag->_cBitRecv  = 0;
        ppktFrag->_cBitTotal = 0;
        ppktFrag->_iBitEnd   = 0;
        memset(ppktFrag->_abBits, 0, cbPkt - offsetof(CFragPacket, _abBits));
        TimerSet(&ppktFrag->_timer, TimerTick() + cfgIpFragTimeoutInSeconds * TICKS_PER_SECOND);

        Assert(_cFrag == _pqFrag.Count());
        _pqFrag.InsertTail(ppktFrag);
        _cFrag += 1;
        Assert(_cFrag == _pqFrag.Count());
    }

    if (uiFragOff == 0)
    {
        if (ppktFrag->_cbHdrLen > 0)
        {
            if (ppktFrag->_cbHdrLen != cbHdrLen)
            {
                TraceSz2(pktWarn, "[DISCARD] Duplicate first fragment and header sizes don't match (%d vs. %d)",
                         ppktFrag->_cbHdrLen, cbHdrLen);
                goto fragfree;
            }

            TraceSz(pktWarn, "[DISCARD] Ignoring duplicate first fragment");
            return;
        }

        memcpy((BYTE *)ppktFrag->GetPv() + MAXIPHDRLEN - cbHdrLen, pIpHdr, cbHdrLen);
        ppktFrag->_cbHdrLen = cbHdrLen;
    }

    cbLen -= cbHdrLen;

    if (cbLen > 0)
    {
        ibEnd   = (uiFragOff * 8) + cbLen;
        iBit    = uiFragOff;
        iBitEnd = iBit + (cbLen + 7) / 8;

        if (MAXIPHDRLEN + ibEnd > ppktFrag->GetCb())
        {
            TraceSz1(pktWarn, "[DISCARD] Maximum reassembly size exceeded (%d)", ibEnd);
            goto fragfree;
        }

        if (fMoreFrag)
        {
            if ((cbLen % 8) != 0)
            {
                TraceSz1(pktWarn, "[DISCARD] Fragment with MF flag has non 8-byte multiple payload (%d)", cbLen);
                goto fragfree;
            }
        }
        else
        {
            if (ppktFrag->_cbLen == 0)
            {
                if (ppktFrag->_iBitEnd >= iBitEnd)
                {
                    TraceSz(pktWarn, "[DISCARD] Fragments received past end of last fragment");
                    goto fragfree;
                }

                ppktFrag->_cbLen     = ibEnd;
                ppktFrag->_cBitTotal = iBitEnd;
            }
            else if (ppktFrag->_cbLen != ibEnd)
            {
                TraceSz2(pktWarn, "[DISCARD] Last fragment was duplicated and has a different size (%d vs. %d)",
                         ppktFrag->_cbLen, ibEnd);
                goto fragfree;
            }
        }

        memcpy((BYTE *)ppktFrag->GetPv() + MAXIPHDRLEN + (uiFragOff * 8), (BYTE *)pIpHdr + cbHdrLen, cbLen);

        if (ppktFrag->_cBitTotal && iBitEnd > ppktFrag->_cBitTotal)
        {
            TraceSz(pktWarn, "[DISCARD] Fragment received past end of last fragment");
            goto fragfree;
        }

        if (ppktFrag->_iBitEnd < iBitEnd)
            ppktFrag->_iBitEnd = iBitEnd;

        for (; iBit < iBitEnd; ++iBit)
        {
            ppktFrag->SetBit(iBit);
            Assert(ppktFrag->_cBitTotal == 0 || ppktFrag->_cBitRecv <= ppktFrag->_cBitTotal);
        }
    }

    if (ppktFrag->_cbHdrLen && ppktFrag->_cbLen && ppktFrag->_cBitRecv == ppktFrag->_cBitTotal)
    {
        ppktFrag->SetPv((BYTE *)ppktFrag->GetPv() + MAXIPHDRLEN - ppktFrag->_cbHdrLen);
        ppktFrag->SetCb(ppktFrag->_cbHdrLen + ppktFrag->_cbLen);
        ppktFrag->SetType(PKTF_TYPE_IP);

        pIpHdr              = ppktFrag->GetIpHdr();
        pIpHdr->_wLen       = HTONS((WORD)ppktFrag->GetCb());
        pIpHdr->_wFragOff   = 0;
        pIpHdr->_wChecksum  = 0;
        pIpHdr->_wChecksum  = (WORD)~tcpipxsum(0, pIpHdr, ppktFrag->_cbHdrLen);

        TraceSz(pktRecv, "Fragment processing complete.  Processing reassembled packet.");
        IpRecv(ppktFrag);
        goto fragfree;
    }

    TraceSz4(pktRecv, "[FRAGHOLD] Waiting for more fragments (cbHdrLen=%ld,cBitRecv=%ld,cBitTotal=%ld,cbLen=%ld)",
             ppktFrag->_cbHdrLen, ppktFrag->_cBitRecv, ppktFrag->_cBitTotal, ppktFrag->_cbLen);
    return;

fragfree:
    FragFree(ppktFrag);
    return;
}

#endif // XNET_FEATURE_FRAG

// ---------------------------------------------------------------------------------------
// CXnIp (Route)
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ROUTE

NTSTATUS CXnIp::RouteInit()
{
    TCHECK(USER);

    InitializeListHead(&_leRouteList);
    Assert(_ipaDstLast == 0);
    Assert(_prteLast == NULL);

    return(NETERR_OK);
}

void CXnIp::RouteTerm()
{
    TCHECK(UDPC);
    RouteListOrphan();
}

void CXnIp::RouteInvalidateCache()
{
    ICHECK(IP, UDPC|SDPC);
    _ipaDstLast = 0;
    _prteLast = NULL;
}

void CXnIp::RouteListOrphan()
{
    ICHECK(IP, UDPC|SDPC);

    if (!IsListNull(&_leRouteList))
    {
        CRouteEntry * prte = (CRouteEntry *)_leRouteList.Flink;
        CRouteEntry * prteNext;

        while (prte != (CRouteEntry *)&_leRouteList)
        {
            prteNext = (CRouteEntry *)prte->_le.Flink;
            RouteEntryOrphan(prte);
            prte = prteNext;
        }
    }
}

void CXnIp::RouteEntryOrphan(CRouteEntry * prte)
{
    ICHECK(IP, UDPC|SDPC);

    RouteInvalidateCache();

    AssertListEntry(&_leRouteList, &prte->_le);
    RemoveEntryList(&prte->_le);

    prte->SetFlags(RTEF_ORPHAN);
    RouteRelease(prte);
}

CRouteEntry * CXnIp::RouteLookup(CIpAddr ipaDst)
{
    ICHECK(IP, UDPC|SDPC);

    CRouteEntry *    prte;
    CRouteEntry *    prteNext;
    CRouteEntry *    prteFound;

    if (ipaDst == 0)
    {
        return(NULL);
    }

    if (ipaDst == _ipaDstLast)
    {
        // _prteLast may be NULL if the last successful lookup for this IP address
        // failed to find any route for it.  RouteAdd will invalidate the cache
        // and force a slow lookup.  Until then, quickly keep responding that
        // there is no route entry for this IP address.

        if (_prteLast)
            _prteLast->AddRef();

        return(_prteLast);
    }

    prte = (CRouteEntry *)_leRouteList.Flink;
    prteFound = NULL;

    while (prte != (CRouteEntry *)&_leRouteList)
    {
        prteNext = (CRouteEntry *)prte->_le.Flink;

        if ((ipaDst & prte->_ipaMask) == prte->_ipaDst)
        {
            prteFound = prte;
            prte->AddRef();
            break;
        }

        prte = prteNext;
    }

    _ipaDstLast = ipaDst;
    _prteLast = prteFound;

    return(prteFound);
}

void CXnIp::RouteAdd(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext, WORD wFlags, WORD wMetric)
{
    ICHECK(IP, UDPC|SDPC);

    CRouteEntry *    prte;
    CRouteEntry *    prteNext;
    CRouteEntry *    prteNew;

    // Validate input parameters:
    //  - address mask must be of the form 111...000
    //      be careful about the byte order
    //  - next hop address cannot be a broadcast address

    ipaDst = ipaDst & ipaMask;

    if (    (!ipaDst.IsValidUnicast() && !(wFlags & RTEF_DEFAULT))
        ||  !ipaNext.IsValidUnicast()
        ||  !ipaMask.IsValidMask())
    {
        TraceSz1(Warning, "Bad route to %s:", ipaDst.Str());
        TraceSz1(Warning, "  mask = %s", ipaMask.Str());
        TraceSz1(Warning, "  nexthop = %s", ipaNext.Str());
        return;
    }

    // Find out if the specified route is already in the table
    
    AssertList(&_leRouteList);

    prte    = (CRouteEntry *)_leRouteList.Flink;
    prteNew = NULL;

    while (prte != (CRouteEntry *)&_leRouteList)
    {
        prteNext = (CRouteEntry *)prte->_le.Flink;

        if (    prte->_ipaDst == ipaDst
            &&  prte->_ipaMask == ipaMask
            &&  (ipaMask != 0 || prte->_ipaNext == ipaNext))
        {
            // Update the existing route information
            prteNew = prte;
            RemoveEntryList(&prte->_le);
            break;
        }

        prte = prteNext;
    }
    
    if (prteNew == NULL)
    {
        prteNew = (CRouteEntry *)PoolAllocZ(sizeof(CRouteEntry), PTAG_CRouteEntry);
        
        if (prteNew == NULL)
        {
            TraceSz(Warning, "CXnIp::RouteAdd - Out of memory allocating route entry");
            return;
        }

        prteNew->_cRefs = 1;
    }

    RouteInvalidateCache();

    prteNew->_wFlags  = wFlags;
    prteNew->_wMetric = wMetric;
    prteNew->_ipaDst  = ipaDst;
    prteNew->_ipaMask = ipaMask;
    prteNew->_ipaNext = ipaNext;

    // Insert the new route into the table
    //  sorted by the mask length and route metric

    AssertList(&_leRouteList);

    prte = (CRouteEntry *)_leRouteList.Flink;

    while (prte != (CRouteEntry *)&_leRouteList)
    {
        prteNext = (CRouteEntry *)prte->_le.Flink;

        if (    ipaMask > prte->_ipaMask
            ||  (   ipaMask == prte->_ipaMask
                 && wMetric < prte->_wMetric))
        {
            break;
        }

        prte = prteNext;
    }

    prteNew->_le.Flink = (LIST_ENTRY *)prte;
    prteNew->_le.Blink = prte->_le.Blink;
    prte->_le.Blink->Flink = (LIST_ENTRY *)prteNew;
    prte->_le.Blink = (LIST_ENTRY *)prteNew;

    AssertList(&_leRouteList);
}

void CXnIp::RouteDelete(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext)
{
    ICHECK(IP, UDPC|SDPC);

    CRouteEntry *    prte;
    CRouteEntry *    prteNext;

    RaiseToDpc();

    AssertList(&_leRouteList);

    prte = (CRouteEntry *)_leRouteList.Flink;

    while (prte != (CRouteEntry *)&_leRouteList)
    {
        prteNext = (CRouteEntry *)prte->_le.Flink;

        if (    prte->_ipaDst == ipaDst
            &&  prte->_ipaMask == ipaMask
            &&  prte->_ipaNext == ipaNext)
        {
            RouteEntryOrphan(prte);
            break;
        }

        prte = prteNext;
    }
}

void CXnIp::RouteRedirect(CIpAddr ipaDst, CIpAddr ipaOldGateway, CIpAddr ipaNewGateway)
{
    ICHECK(IP, SDPC);

    CRouteEntry * prte;

    // The redirected destination must be a unicast address
    if (!ipaDst.IsValidUnicast())
        return;

    // Validate the new gateway address:
    //  must be a unicast address on the same subnet
    //  through which the redirect message arrived
    if (    !ipaNewGateway.IsValidUnicast()
        ||  _ipaSubnet != (ipaNewGateway & _ipaMask))
        return;

    // Find the current route to the destination
    // and see if the sender of the redirect message
    // is the current next-hop gateway

    prte = RouteLookup(ipaDst);

    if (prte == NULL)
    {
        return;
    }
    else if (prte->_ipaNext != ipaOldGateway)
    {
        RouteRelease(prte);
        return;
    }

    // Add a new host route
    RouteAdd(ipaDst, 0xFFFFFFFF, ipaNewGateway, RTEF_HOST, RTE_DEFAULT_METRIC);
}

void CXnIp::RouteRelease(CRouteEntry * prte)
{
    ICHECK(IP, USER|UDPC|SDPC);

    if (InterlockedDecrement(&prte->_cRefs) == 0)
    {
        Assert(prte->IsOrphan());
        PoolFree(prte);
    }
}

#if DBG

void CXnIp::RouteListDump()
{
    ICHECK(IP, USER|UDPC|SDPC);

    RaiseToDpc();

    TraceSz(Verbose, "\n*** Route table:\n");

    CRouteEntry * prte = (CRouteEntry *)_leRouteList.Flink;

    while (prte != (CRouteEntry *)&_leRouteList)
    {
        TraceSz6(Verbose, "%s/%s/%s %02X %d %d", prte->_ipaDst.Str(), prte->_ipaMask.Str(),
                 prte->_ipaNext.Str(), prte->_wFlags, prte->_wMetric, prte->_cRefs);
        prte = (CRouteEntry *)prte->_le.Flink;
    }
}

#endif

#endif

// ---------------------------------------------------------------------------------------
// CXnIp (Register / Unregister Key)
// ---------------------------------------------------------------------------------------

INT CXnIp::IpRegisterKey(const XNKID * pxnkid, const XNKEY * pxnkey)
{
    ICHECK(IP, USER);

#ifdef XNET_FEATURE_SG
    if (!XNetXnKidIsSystemLink(pxnkid) && !XNetXnKidIsOnlinePeer(pxnkid))
#else
    if (!XNetXnKidIsSystemLink(pxnkid))
#endif
    {
        TraceSz1(Warning, "IpRegisterKey - XNKID %s is not peer-to-peer", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));
        return(WSAEINVAL);
    }

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];
    BYTE abDhX[CBDHG1];
    BYTE abDhGX[CBDHG1];

    // HMAC the key with the LAN keys as a precaution against the title not keeping
    // its key private (by sending it in cleartext over the network, for example).
    // This means that even if you intercept an XNKEY you still need to figure
    // out the LAN key to make use of it.

    XcHMAC(_abKeyShaLan, sizeof(_abKeyShaLan), (BYTE *)pxnkid->ab, sizeof(pxnkid->ab),
           (BYTE *)pxnkey->ab, sizeof(pxnkey->ab), abHash);

    // Generate a diffie-hellman X and g^X for use in key-exchange

    Rand(abDhX, sizeof(abDhX));
    XcModExp((DWORD *)abDhGX, (DWORD *)g_abOakleyGroup1Base, (DWORD *)abDhX,
             (DWORD *)g_abOakleyGroup1Mod, CBDHG1 / sizeof(DWORD));

    RaiseToDpc();

    CKeyReg * pKeyReg = KeyRegLookup(pxnkid);

    if (pKeyReg != NULL)
    {
        TraceSz1(Warning, "IpRegisterKey - XNKID %s is already registered", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));
        return(WSAEALREADY);
    }

    if (_cKeyReg == cfgKeyRegMax)
    {
        TraceSz1(Warning, "IpRegisterKey - Reached config limit for key registrations (%d)", cfgKeyRegMax);
        return(WSAENOMORE);
    }

    pKeyReg = &_pKeyReg[_cKeyReg++];
    pKeyReg->_xnkid = *pxnkid;
    memcpy(pKeyReg->_abKeySha, abHash, sizeof(pKeyReg->_abKeySha));
    memcpy(pKeyReg->_abKeyDes, &abHash[sizeof(abHash) - sizeof(pKeyReg->_abKeyDes)], sizeof(pKeyReg->_abKeyDes));
    memcpy(pKeyReg->_abDhX, abDhX, sizeof(pKeyReg->_abDhX));
    memcpy(pKeyReg->_abDhGX, abDhGX, sizeof(pKeyReg->_abDhGX));
    XcDESKeyParity(pKeyReg->_abKeyDes, sizeof(pKeyReg->_abKeyDes));
    pKeyReg->_pQosReg = NULL;

    TraceSz1(secStat, "XNKID %s: Registered", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));

    return(0);
}

INT CXnIp::IpCreateKey(XNKID * pxnkid, XNKEY * pxnkey)
{
    ICHECK(IP, USER|UDPC);
    Rand((BYTE *)pxnkid, sizeof(XNKID));
    Rand((BYTE *)pxnkey, sizeof(XNKEY));
    pxnkid->ab[0] &= ~XNET_XNKID_MASK;
    pxnkid->ab[0] |=  XNET_XNKID_SYSTEM_LINK;
    return(0);
}

INT CXnIp::IpUnregisterKey(const XNKID * pxnkid)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    CKeyReg * pKeyReg = KeyRegLookup(pxnkid);

    if (pKeyReg == NULL)
    {
        TraceSz1(Warning, "IpUnregisterKey - XNKID %s is not registered", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));
        return(WSAEINVAL);
    }

#if 0
//@@@
    if (pKeyReg->_pbQos)
    {
        SysFree(pKeyReg->_pbQos);
        pKeyReg->_pbQos = NULL;
        pKeyReg->_cbQos = 0;
    }
#endif

    TraceSz1(secStat, "XNKID %s: Unregistered", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));

    CKeyReg *   pKeyRegLast = &_pKeyReg[--_cKeyReg];
    UINT        cSecReg     = cfgSecRegMax;
    CSecReg *   pSecReg     = _pSecReg;

    // Move the last entry to this spot to keep the table contiguous

    *pKeyReg = *pKeyRegLast;

    // Free any CSecReg that points at the deleted key entry.  Update any CSecReg that
    // pointed to the last CKeyReg that was just moved.

    for (; cSecReg > 0; ++pSecReg, --cSecReg)
    {
        if (pSecReg->_dwSpiRecv)
        {
            if (pSecReg->_pKeyReg == pKeyReg)
            {
                if (pSecReg->IsXmitReady() && !pSecReg->TestFlags(SRF_SECMSG_DELETE))
                {
                    pSecReg->SetFlags(SRF_SECMSG_DELETE);
                    IpXmitSecMsgDelete(pSecReg, SECMSG_DELETE_UNREGISTER);
                }

                SecRegFree(pSecReg);
            }
            else if (pSecReg->_pKeyReg == pKeyRegLast)
            {
                pSecReg->_pKeyReg = pKeyReg;
            }
        }
    }

    return(0);
}

// ---------------------------------------------------------------------------------------
// CXnIp (KeyRegLookup / KeyRegKey)
// ---------------------------------------------------------------------------------------

CXnIp::CKeyReg * CXnIp::KeyRegLookup(const XNKID * pxnkid)
{
    ICHECK(IP, UDPC|SDPC);

    CKeyReg *   pKeyReg = _pKeyReg;
    UINT        cKeyReg = _cKeyReg;

    for (; cKeyReg-- > 0; ++pKeyReg)
    {
        if (memcmp(pKeyReg->_xnkid.ab, pxnkid->ab, sizeof(XNKID)) == 0)
        {
            return(pKeyReg);
        }
    }

    return(NULL);
}

// ---------------------------------------------------------------------------------------
// CXnIp (DES Encrypt / Decrypt)
// ---------------------------------------------------------------------------------------

void CXnIp::CryptDes(DWORD dwOp, BYTE * pbKey, UINT cbKey, BYTE * pbIv, BYTE * pb, UINT cb)
{
    Assert(cbKey == XC_SERVICE_DES_KEYSIZE || cbKey == XC_SERVICE_DES3_KEYSIZE);
    BYTE abDesTable[XC_SERVICE_DES3_TABLESIZE];
    BYTE abFeedback[XC_SERVICE_DES_BLOCKLEN];

    XcKeyTable(cbKey == XC_SERVICE_DES_KEYSIZE ? XC_SERVICE_DES_CIPHER : XC_SERVICE_DES3_CIPHER,
               abDesTable, pbKey);
    memcpy(abFeedback, pbIv, XC_SERVICE_DES_BLOCKLEN);
    XcBlockCryptCBC(cbKey == XC_SERVICE_DES_KEYSIZE ? XC_SERVICE_DES_CIPHER : XC_SERVICE_DES3_CIPHER,
                    cb, pb, pb, abDesTable, dwOp, abFeedback);
}

// ---------------------------------------------------------------------------------------
// CXnIp (InAddr)
// ---------------------------------------------------------------------------------------

INT CXnIp::IpXnAddrToInAddr(const XNADDR * pxna, const XNKID * pxnkid, CIpAddr * pipa)
{
    ICHECK(IP, USER);

    // The InAddr for the local host is the loopback address

    if (_ea.IsEqual(pxna->abEnet))
    {
        *pipa = IPADDR_LOOPBACK;
        return(0);
    }

#ifdef XNET_FEATURE_SG

    if (XNetXnKidIsOnlinePeer(pxnkid) && pxna->inaOnline.s_addr == 0)
    {
        TraceSz2(Warning, "IpXnAddrToInAddr - XNKID %s is online-peer but XNADDR %s is system-link",
                 HexStr(pxnkid->ab, sizeof(pxnkid->ab)), XnAddrStr(pxna));
        return(WSAEINVAL);
    }

#endif

    RaiseToDpc();

    // See if we have a CSecReg already for this XNADDR and XNKID

    CSecReg * pSecReg = SecRegLookup(pxna, pxnkid);

    if (pSecReg == NULL)
    {
        CKeyReg * pKeyReg = KeyRegLookup(pxnkid);

        if (pKeyReg == NULL)
        {
            TraceSz1(Warning, "IpXnAddrToInAddr - XNKID %s is not registered", HexStr(pxnkid->ab, sizeof(pxnkid->ab)));
            return(WSAEINVAL);
        }

        pSecReg = SecRegAlloc(pxna, pKeyReg);

        if (pSecReg == NULL)
        {
            return(WSAENOMORE);
        }
    }
    else
    {
        // Update the XNADDR of the CSecReg in case it has been updated with new
        // online information.

        pSecReg->_xnaddr = *pxna;
    }

    pSecReg->SetFlags(SRF_OWNED);

    *pipa = CIpAddr(pSecReg->_dwSpiRecv);

    return(0);
}

INT CXnIp::IpServerToInAddr(const CIpAddr ipa, DWORD dwServiceId, CIpAddr * pipa)
{
    ICHECK(IP, USER);

    if (!ipa.IsValidUnicast())
    {
        TraceSz1(Warning, "IpServerToInAddr - %s is not a valid unicast IP address", ipa.Str());
        return(WSAEINVAL);
    }

    RaiseToDpc();

#ifdef XNET_FEATURE_SG

    XOKERBINFO * pxokerbinfo;

#ifdef XNET_FEATURE_INSECURE

    if (dwServiceId == 0)
    {
        pxokerbinfo = NULL;
        goto ServiceDone;
    }

#endif

#ifdef XNET_FEATURE_ONLINE

    pxokerbinfo = _pXoBase ? _pXoBase->XoKerbGetInfo(dwServiceId) : NULL;

    if (pxokerbinfo != NULL)
        goto ServiceDone;

#endif

#endif

    TraceSz1(Warning, "IpServerToInAddr - dwServiceId %08lX is not available", dwServiceId);
    return(WSAEINVAL);

#ifdef XNET_FEATURE_SG

ServiceDone:

    // See if we have a CSecReg already for this XNADDR and XOKERBINFO

    CSecReg * pSecReg = SecRegLookup(ipa, dwServiceId, pxokerbinfo);

    if (pSecReg == NULL)
    {
        pSecReg = SecRegAlloc(ipa, dwServiceId);

        if (pSecReg == NULL)
        {
            return(WSAENOMORE);
        }
    }

    *pipa = CIpAddr(pSecReg->_dwSpiRecv);

    return(0);

#endif

}

INT CXnIp::IpInAddrToXnAddr(const CIpAddr ipa, XNADDR * pxna, XNKID * pxnkid)
{
    ICHECK(IP, USER|UDPC|SDPC);

    if (pxna)
    {
        memset(pxna, 0, sizeof(XNADDR));
    }

    if (pxnkid)
    {
        memset(pxnkid, 0, sizeof(XNKID));
    }

    if (ipa == IPADDR_LOOPBACK)
    {
        if (pxna)
        {
            IpGetXnAddr(pxna);
        }

        return(0);
    }

    RaiseToDpc();

    CSecReg * pSecReg = ipa.IsSecure() ? SecRegLookup(ipa) : NULL;

    if (pSecReg == NULL)
    {
        TraceSz1(Warning, "IpInAddrToXnAddr - %s is not a registered secure address", ipa.Str());
        return(WSAEINVAL);
    }

#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINESERVER))
    {
        TraceSz1(Warning, "IpInAddrToXnAddr - %s is a secure address to a server.  Cannot convert to XNADDR.", ipa.Str());
        return(WSAEINVAL);
    }
#endif

    if (pxna)
    {
        *pxna = pSecReg->_xnaddr;
    }

    if (pxnkid)
    {
        *pxnkid = pSecReg->_pKeyReg->_xnkid;
    }

    return(0);
}

INT CXnIp::IpUnregisterInAddr(const CIpAddr ipa)
{
    ICHECK(IP, USER|UDPC|SDPC);
    
    RaiseToDpc();

    CSecReg * pSecReg = ipa.IsSecure() ? SecRegLookup(ipa) : NULL;

    if (pSecReg == NULL)
    {
        TraceSz1(Warning, "IpUnregisterInAddr - %s is not a registered secure address", ipa.Str());
        return(WSAEINVAL);
    }
    
#ifdef XNET_FEATURE_SG

    if (pSecReg == _pSecRegLogon)
    {
        TraceSz1(Warning, "IpUnregisterInAddr - %s cannot be manually unregistered", ipa.Str());
        return(WSAEINVAL);
    }

#endif

    if (pSecReg->_bState == SR_STATE_RESPSENT)
    {
        // The other side is in initiating key-exchange with us, so just forget that we have
        // seen this secure address.

        pSecReg->ClearFlags(SRF_OWNED);
    }
    else
    {
        // Shutdown the security association and let the other side know if necessary.

        if (pSecReg->IsXmitReady() && !pSecReg->TestFlags(SRF_SECMSG_DELETE))
        {
            IpXmitSecMsgDelete(pSecReg, SECMSG_DELETE_SHUTDOWN);
        }

        SecRegFree(pSecReg);
    }

    return(0);
}

// ---------------------------------------------------------------------------------------
// CXnIp (SecRegLookup / SecRegAlloc / SecRegFree)
// ---------------------------------------------------------------------------------------

CXnIp::CSecReg * CXnIp::SecRegLookup(const XNADDR * pxna, const XNKID * pxnkid)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg *   pSecReg = _pSecReg;
    UINT        cSecReg = cfgSecRegMax;

    for (; cSecReg > 0; ++pSecReg, --cSecReg)
    {
        if (pSecReg->_dwSpiRecv == 0 || pSecReg->TestFlags(SRF_ONLINESERVER))
            continue;

        if (memcmp(pSecReg->_xnaddr.abEnet, pxna->abEnet, sizeof(CEnetAddr)) != 0)
            continue;

        if (memcmp(&pSecReg->_pKeyReg->_xnkid, pxnkid, sizeof(XNKID)) != 0)
            continue;

        return(pSecReg);
    }

    return(NULL);
}

#ifdef XNET_FEATURE_SG

CXnIp::CSecReg * CXnIp::SecRegLookup(const CIpAddr ipa, DWORD dwServiceId, const XOKERBINFO * pxokerbinfo)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg *   pSecReg = _pSecReg;
    UINT        cSecReg = cfgSecRegMax;

    for (; cSecReg > 0; ++pSecReg, --cSecReg)
    {
        if (pSecReg->_dwSpiRecv == 0 || !pSecReg->TestFlags(SRF_ONLINESERVER))
            continue;

        if (pSecReg->_ipaDst != ipa)
            continue;

        if (pSecReg->_dwServiceId == dwServiceId)
            return(pSecReg);

#ifdef XNET_FEATURE_ONLINE
        XOKERBINFO * pxokerbinfoReg = _pXoBase ? _pXoBase->XoKerbGetInfo(pSecReg->_dwServiceId) : NULL;
        if (pxokerbinfoReg && pxokerbinfo->_dwTicketId == pxokerbinfoReg->_dwTicketId)
            return(pSecReg);
#endif
    }

    return(NULL);
}

#endif

CXnIp::CSecReg * CXnIp::SecRegLookup(DWORD dwSpiRecv)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg * pSecReg = NULL;

    if (_pSecReg && CIpAddr(dwSpiRecv).IsSecure())
    {
        UINT uiSlot = CIpAddr(dwSpiRecv).SecureSlot();

        if (uiSlot < cfgSecRegMax)
        {
            pSecReg = &_pSecReg[uiSlot];

            if (pSecReg->_dwSpiRecv != dwSpiRecv)
            {
                pSecReg = NULL;
            }
        }
    }

    return(pSecReg);
}

CXnIp::CSecReg * CXnIp::SecRegAlloc()
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg *   pSecReg = _pSecReg;
    UINT        cSecReg = cfgSecRegMax;

    for (; cSecReg > 0; ++pSecReg, --cSecReg)
    {
        if (pSecReg->_dwSpiRecv == 0)
            break;
    }

    if (cSecReg == 0)
    {
        TraceSz1(Warning, "Reached config limit for secure address registrations (%d)", cfgSecRegMax);
        return(NULL);
    }

    pSecReg->_timer.Init((PFNTIMER)SecRegTimer);

    if (++_wSecRegUniq == 0)
        ++_wSecRegUniq;

    CIpAddr ipa(_wSecRegUniq, (BYTE)(pSecReg - _pSecReg));

    pSecReg->_dwSpiRecv = ipa;

    TraceSz1(secStat, "Allocated %s", pSecReg->Str());

    return(pSecReg);
}

CXnIp::CSecReg * CXnIp::SecRegAlloc(const XNADDR * pxna, CKeyReg * pKeyReg)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg * pSecReg = SecRegAlloc();

    if (pSecReg != NULL)
    {
        pSecReg->_wFlags       = SRF_SYSTEMLINK;
        pSecReg->_xnaddr       = *pxna;
        pSecReg->_pKeyReg      = pKeyReg;
        pSecReg->_cbKeyDesRecv = XC_SERVICE_DES_KEYSIZE;
        pSecReg->_cbKeyDesXmit = XC_SERVICE_DES_KEYSIZE;
        pSecReg->_ipaDst       = IPADDR_SECURE_DEFAULT;
        pSecReg->_ipportDst    = ESPUDP_CLIENT_PORT;

#ifdef XNET_FEATURE_SG
        // For an online peer we don't know for sure the address to send packets until the
        // key exchange process is complete.  During key exchange, the _ipaDst, _ipportDst,
        // and _ipportSrc fields of the CSecReg are updated as return address information
        // becomes available.

        if (XNetXnKidIsOnlinePeer(&pKeyReg->_xnkid))
        {
            pSecReg->_wFlags    = SRF_ONLINEPEER;
            pSecReg->_ipaDst    = 0;
            pSecReg->_ipportDst = 0;
        }
#endif
    }

    return(pSecReg);
}

#ifdef XNET_FEATURE_SG

CXnIp::CSecReg * CXnIp::SecRegAlloc(const CIpAddr ipa, DWORD dwServiceId)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg * pSecReg = SecRegAlloc();

    if (pSecReg != NULL)
    {
        pSecReg->_wFlags       = SRF_ONLINESERVER|SRF_OWNED;
        pSecReg->_dwServiceId  = dwServiceId;
        pSecReg->_cbKeyDesRecv = XC_SERVICE_DES3_KEYSIZE;
        pSecReg->_cbKeyDesXmit = XC_SERVICE_DES3_KEYSIZE;
        pSecReg->_ipaDst       = ipa;
        pSecReg->_ipportDst    = ESPUDP_CLIENT_PORT;
    }

    return(pSecReg);
}

#endif

void CXnIp::SecRegFree(CSecReg * pSecReg)
{
    ICHECK(IP, UDPC|SDPC);

    if (pSecReg->_dwSpiRecv)
    {
        TraceSz1(secStat, "Deallocated %s", pSecReg->Str());
        SecRegSetIdle(pSecReg);
        memset(pSecReg, 0, sizeof(CSecReg));
    }
}

// ---------------------------------------------------------------------------------------
// CXnIp (SecReg state machine)
// ---------------------------------------------------------------------------------------

void CXnIp::SecRegEnqueue(CSecReg * pSecReg, CPacket * ppkt)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(     pSecReg->_bState == SR_STATE_IDLE
           ||   pSecReg->_bState == SR_STATE_INITSENT
           ||   pSecReg->_bState == SR_STATE_RESPSENT);

    if (pSecReg->_bState == SR_STATE_IDLE)
    {
        IpXmitKeyEx(pSecReg);
    }

    // Insert the packet into the wait queue until we make it into the ready state

    pSecReg->_pqWait.InsertTail(ppkt);

    TraceSz2(secStat, "Packet enqueued to %s (%d waiting)", pSecReg->Str(), pSecReg->_pqWait.Count());
}

void CXnIp::SecRegXmitQueue(CSecReg * pSecReg)
{
    if (!pSecReg->_pqWait.IsEmpty())
    {
        TraceSz3(secStat, "Sending %d waiting packet%s to %s",
                 pSecReg->_pqWait.Count(), pSecReg->_pqWait.Count() == 1 ? "" : "s", pSecReg->Str());

        do
        {
            CPacket * ppkt = pSecReg->_pqWait.RemoveHead();
            IpXmit(ppkt, NULL);
        }
        while (!pSecReg->_pqWait.IsEmpty());
    }
}

void CXnIp::SecRegShutdown(BOOL fOnlineOnly)
{
    ICHECK(IP, UDPC|SDPC);

    CSecReg *   pSecReg = _pSecReg;
    UINT        cSecReg = cfgSecRegMax;

    for (; cSecReg > 0; ++pSecReg, --cSecReg)
    {
        if (pSecReg->_dwSpiRecv == 0)
            continue;

        if (fOnlineOnly && !pSecReg->TestFlags(SRF_ONLINEPEER|SRF_ONLINESERVER))
            continue;

        if (pSecReg->IsXmitReady() && !pSecReg->TestFlags(SRF_SECMSG_DELETE))
        {
            pSecReg->SetFlags(SRF_SECMSG_DELETE);
            IpXmitSecMsgDelete(pSecReg, SECMSG_DELETE_SHUTDOWN);
        }

        if (fOnlineOnly)
        {
            SecRegFree(pSecReg);
        }
    }
}

void CXnIp::SecRegSetIdle(CSecReg * pSecReg)
{
    ICHECK(IP, UDPC|SDPC);

    // Tell the sockets layer that this secure IP address has being disconnected.
    // Any TCP socket connected to this address will be reset.

    SockReset(CIpAddr(pSecReg->_dwSpiRecv));

    pSecReg->_wFlags   &= SRF_SYSTEMLINK|SRF_ONLINEPEER|SRF_ONLINESERVER|SRF_OWNED;
    pSecReg->_bState    = SR_STATE_IDLE;
    pSecReg->_bRetry    = 0;
    pSecReg->_dwSeqXmit = 0;
    pSecReg->_dwSeqRecv = 0;
    pSecReg->_dwSeqMask = 0;
    memset(pSecReg->_abNonceInit, 0, sizeof(pSecReg->_abNonceInit));
    memset(pSecReg->_abNonceResp, 0, sizeof(pSecReg->_abNonceResp));
    Rand(pSecReg->_abIv, sizeof(pSecReg->_abIv));
    TimerSet(&pSecReg->_timer, TIMER_INFINITE);
    pSecReg->_pqWait.Complete(this);
    SecRegSetTicks(pSecReg);

#ifdef XNET_FEATURE_SG
    if (pSecReg->_wFlags & SRF_ONLINEPEER)
    {
        pSecReg->_ipaDst    = 0;
        pSecReg->_ipportDst = 0;
    }

    if (_pSecRegLogon == pSecReg && _uiLogonState != XN_LOGON_STATE_IDLE)
    {
        _uiLogonState = XN_LOGON_STATE_OFFLINE;

        if (_pEventLogon)
        {
            EvtSet(_pEventLogon, EVENT_INCREMENT);
        }
    }

#endif
}

void CXnIp::SecRegSetTicks(CSecReg * pSecReg)
{
    ICHECK(IP, UDPC|SDPC);

    pSecReg->_dwTickRecv         = TimerTick();
    pSecReg->_dwTickXmit         = TimerTick();
    pSecReg->_dwTickPulse        = TimerTick();
    pSecReg->_dwTickPulseTimeout = cfgSecRegPulseTimeoutInSeconds * TICKS_PER_SECOND;
    pSecReg->_dwTickTimeout      = cfgSecRegTimeoutInSecondsDiv10 * 10 * TICKS_PER_SECOND;
}

void CXnIp::SecRegSetOwned(CIpAddr ipa)
{
    ICHECK(IP, USER|UDPC|SDPC);

    if (ipa.IsSecure())
    {
        RaiseToDpc();

        CSecReg * pSecReg = SecRegLookup(ipa);

        if (pSecReg != NULL)
        {
            pSecReg->SetFlags(SRF_OWNED);
        }
    }
}

void CXnIp::SecRegSetKey(CSecReg * pSecReg, BYTE * pbKeyHmac, UINT cbKeyHmac, BYTE * pbDhX, UINT cbDhX,
                         BYTE * pbDhGY, UINT cbDhGY, BOOL fInitiator)
{
    ICHECK(IP, UDPC|SDPC);

    BYTE abDhGXY[CBDHG1];

    // Generate the diffie-hellman g^XY mod p = (g^Y)^X mod p

    Assert(cbDhX == CBDHG1);
    Assert(cbDhGY == CBDHG1);
    XcModExp((DWORD *)abDhGXY, (DWORD *)pbDhGY, (DWORD *)pbDhX, (DWORD *)g_abOakleyGroup1Mod, CBDHG1 / sizeof(DWORD));

    BYTE * pbKeyOut = fInitiator ? pSecReg->_abKeyShaXmit : pSecReg->_abKeyShaRecv;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            XcHMAC(pbKeyHmac, cbKeyHmac, abDhGXY, sizeof(abDhGXY), pSecReg->_abNonceInit,
                   sizeof(pSecReg->_abNonceInit) + sizeof(pSecReg->_abNonceResp), pbKeyOut);
            pbKeyOut += XC_SERVICE_DIGEST_SIZE;
            abDhGXY[0] += 1;
        }

        pbKeyOut = fInitiator ? pSecReg->_abKeyShaRecv : pSecReg->_abKeyShaXmit;
    }

    XcDESKeyParity(pSecReg->_abKeyDesXmit, sizeof(pSecReg->_abKeyDesXmit));
    XcDESKeyParity(pSecReg->_abKeyDesRecv, sizeof(pSecReg->_abKeyDesRecv));
}

void CXnIp::SecRegTimer(CTimer * pt)
{
    ICHECK(IP, SDPC);

    CSecReg * pSecReg = (CSecReg *)((BYTE *)pt - offsetof(CSecReg, _timer));

    Assert(pSecReg->_bState == SR_STATE_INITSENT || pSecReg->_bState == SR_STATE_RESPSENT);

    TraceSz2(secStat, "KeyEx timeout to %s (bRetry %d)", pSecReg->Str(), pSecReg->_bRetry);

    if (pSecReg->_bRetry == 0)
    {
        TraceSz3(secStat, "Failed KeyEx to %s (%d packet%s flushed)",
                 pSecReg->Str(), pSecReg->_pqWait.Count(),
                 pSecReg->_pqWait.Count() == 1 ? "" : "s");

        if (!pSecReg->TestFlags(SRF_OWNED))
            SecRegFree(pSecReg);
        else
            SecRegSetIdle(pSecReg);
    }
    else
    {
        pSecReg->_bRetry -= 1;
        TimerSet(&pSecReg->_timer, TimerTick() + SecRegRexmitTimeoutInSeconds(pSecReg) * TICKS_PER_SECOND);
        IpXmitKeyEx(pSecReg);
    }
}

void CXnIp::SecRegProbe()
{
    ICHECK(IP, SDPC);

    _cSecRegProbeNumer += cfgSecRegMax;

    if (_cSecRegProbeNumer < _cSecRegProbeDenom)
        return;

    DWORD dwTickNow = TimerTick();

    UINT cSecReg = _cSecRegProbeNumer / _cSecRegProbeDenom;
    _cSecRegProbeNumer -= cSecReg * _cSecRegProbeDenom;
    Assert(_cSecRegProbeNumer < _cSecRegProbeDenom);

    CSecReg * pSecReg     = _pSecRegProbe ? _pSecRegProbe : _pSecReg;
    CSecReg * pSecRegLast = _pSecReg + cfgSecRegMax;

    for (; cSecReg > 0; --cSecReg, ++pSecReg)
    {
        if (pSecReg == pSecRegLast)
        {
            // We've run off the end of the CSecReg vector.  Start back at the beginning.

            pSecReg = _pSecReg;
        }

        if (pSecReg->_dwSpiRecv == 0)
        {
            // This CSecReg is not in use.  Go on to the next one.

            continue;
        }

        if (pSecReg->_bState < SR_STATE_INITWAIT)
        {
            // This CSecReg is in the middle of key exchange.  That process handles its
            // own timeouts.  Go on to the next one.

            continue;
        }

        if (pSecReg->_dwTickRecv <= dwTickNow - pSecReg->_dwTickTimeout)
        {
            // This CSecReg hasn't received a packet in a long enough time that the other
            // side should be considered down.

            TraceSz2(secStat, "Timeout after %d secs of no incoming packets from %s",
                     (dwTickNow - pSecReg->_dwTickRecv) / TICKS_PER_SECOND, pSecReg->Str());

            if (!pSecReg->TestFlags(SRF_OWNED))
                SecRegFree(pSecReg);
            else
                SecRegSetIdle(pSecReg);

            continue;
        }

        DWORD dwTickPulse = dwTickNow - pSecReg->_dwTickPulseTimeout;

        if (pSecReg->_dwTickPulse <= dwTickPulse)
        {
            // This CSecReg hasn't sent a pulse in a long enough time that now might be
            // a good time to send it.  We only need to send it if there have been no
            // packets transmitted recently, or if we have something to say.

#ifdef XNET_FEATURE_SG
            if (pSecReg->TestFlags(SRF_ONLINESERVER))
            {
                if (    pSecReg->_dwTickXmit <= dwTickPulse
                    || (pSecReg == _pSecRegLogon && (_dwSeqSgToXb || *(DWORD *)_abXbToSgPulse)))
                {
                    TraceSz3(secStat, "Sending pulse to %s (last xmit/pulse %d/%d secs ago)",
                             pSecReg->Str(), (dwTickNow - pSecReg->_dwTickXmit) / TICKS_PER_SECOND,
                             (dwTickNow - pSecReg->_dwTickPulse) / TICKS_PER_SECOND);
                    IpXmitSecMsgXbToSgPulse(pSecReg);
                }
                continue;
            }
#endif
            if (pSecReg->_dwTickXmit <= dwTickPulse)
            {
                TraceSz2(secStat, "Sending pulse to %s (last xmit/pulse %d/%d secs ago)",
                         pSecReg->Str(), (dwTickNow - pSecReg->_dwTickXmit) / TICKS_PER_SECOND);
                IpXmitSecMsg(pSecReg, SECMSG_TYPE_PULSE);
                pSecReg->_dwTickPulse = dwTickNow;
                continue;
            }
        }
    }

    _pSecRegProbe = pSecReg;
}

#ifdef XNET_FEATURE_TRACE

char * CXnIp::CSecReg::Str()
{
    #define SECREG_STR_BUFFS    32
    #define SECREG_BUF_SIZE     24
    static char g_chBufSecReg[SECREG_STR_BUFFS * SECREG_BUF_SIZE];
    static LONG g_lBufIndexSecReg = 0;
    char * pch = &g_chBufSecReg[(InterlockedIncrement(&g_lBufIndexSecReg) % SECREG_STR_BUFFS) * SECREG_BUF_SIZE];
    XnInAddrToString(*(IN_ADDR *)&_dwSpiRecv, pch, SECREG_BUF_SIZE);
    strcat(pch, _bState == SR_STATE_IDLE ? "/IDLE" : _bState == SR_STATE_INITSENT ? "/ISENT" :
           _bState == SR_STATE_RESPSENT ? "/RSENT" : "/READY");
    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// CXnIp (IpDecrypt)
// ---------------------------------------------------------------------------------------

BOOL CXnIp::IpDecrypt(CPacket * ppkt, CIpAddr ipaDst)
{
    ICHECK(IP, USER|UDPC|SDPC);

    Assert(ppkt->IsEsp() && ppkt->IsCrypt());
    
    CSecReg * pSecReg = SecRegLookup(ipaDst);

    if (pSecReg == NULL)
    {
        TraceSz1(pktWarn, "IpDecrypt: %s is not a valid secure address", ipaDst.Str());
        return(FALSE);
    }

    if (!pSecReg->IsXmitReady())
    {
        TraceSz1(pktWarn, "IpDecrypt: %s is not ready for transmit", ipaDst.Str());
        return(FALSE);
    }

    // Check that the packet authenticates with the transmit SHA key

    CEspHdr *   pEspHdr     = ppkt->GetEspHdr();
    CEspTail *  pEspTail    = ppkt->GetEspTail();

    BYTE abHash[XC_SERVICE_DIGEST_SIZE];
    Assert(sizeof(pEspTail->_abHash) <= sizeof(abHash));

    XcHMAC(pSecReg->_abKeyShaXmit, sizeof(pSecReg->_abKeyShaXmit),
           (BYTE *)&pEspHdr->_dwSpi,
           (BYTE *)&pEspTail->_abHash - (BYTE *)&pEspHdr->_dwSpi,
           NULL, 0, abHash);

    if (memcmp(pEspTail->_abHash, abHash, sizeof(pEspTail->_abHash)) != 0)
    {
        TraceSz(pktWarn, "IpDecrypt: packet failed to authenticate");
        return(FALSE);
    }

    // Decrypt the packet with the transmit DES key

    BYTE * pb = (BYTE *)(pEspHdr + 1);
    CryptDes(XC_SERVICE_DECRYPT, pSecReg->_abKeyDesXmit, pSecReg->_cbKeyDesXmit,
             pb, pb + XC_SERVICE_DES_BLOCKLEN, (BYTE *)&pEspTail->_abHash - pb);

    return(TRUE);
}

// ---------------------------------------------------------------------------------------
// CXnIp (GetXnAddr)
// ---------------------------------------------------------------------------------------

DWORD CXnIp::IpGetXnAddr(XNADDR * pxna)
{
    ICHECK(IP, USER|UDPC|SDPC);

    RaiseToDpc();

    memset(pxna, 0, sizeof(XNADDR));
    pxna->ina.s_addr = _ipa;
    memcpy(pxna->abEnet, _ea._ab, sizeof(_ea));

    DWORD dwFlags = 0;

#ifdef XNET_FEATURE_SG

    if (_uiLogonState == XN_LOGON_STATE_ONLINE && _pSecRegLogon != NULL)
    {
        pxna->inaOnline.s_addr = _pSecRegLogon->_ipaNat;
        pxna->wPortOnline = _pSecRegLogon->_ipportNat;
        Assert(sizeof(pxna->abOnline) == sizeof(SGADDR));
        memcpy(pxna->abOnline, &_pSecRegLogon->_sgaddr, sizeof(pxna->abOnline));
        dwFlags |= XNET_GET_XNADDR_ONLINE;
    }

#endif

#ifdef XNET_FEATURE_DHCP

    if (_options._gatewayCount > 0)
        dwFlags |= XNET_GET_XNADDR_GATEWAY;

    if (_options._dnsServerCount > 0)
        dwFlags |= XNET_GET_XNADDR_DNS;

    if (ActiveDhcpAddr())
        dwFlags |= XNET_GET_XNADDR_DHCP;
    else if (ActiveAutonetAddr())
        dwFlags |= XNET_GET_XNADDR_AUTO;
    else if (ActiveStaticAddr())
        dwFlags |= XNET_GET_XNADDR_STATIC;
    else if (ActiveNoAddr())
        dwFlags |= XNET_GET_XNADDR_ETHERNET;

#else

    // For now on the XBOX if we are not configured for using DHCP there is no
    // way to acquire an IP address.  This is the common codepath for the secure
    // xnets.lib to take for the XTL 1.0 release.

    Assert(_ipa == 0);

    dwFlags |= XNET_GET_XNADDR_ETHERNET;

#endif

    return(dwFlags);
}

// ---------------------------------------------------------------------------------------
// Online Support
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ONLINE

void CXnIp::IpSetXoBase(CXoBase * pXoBase)
{
    ICHECK(IP, USER);

    RaiseToDpc();

    if (pXoBase == NULL)
    {
        IpLogoff();
    }

    _pXoBase = pXoBase;
}

#endif

#ifdef XNET_FEATURE_SG

void CXnIp::IpLogon(CIpAddr ipaLogon, ULONGLONG * pqwUserId, WSAEVENT hEventLogon)
{
    ICHECK(IP, USER);

    RaiseToDpc();

    Assert(_uiLogonState == XN_LOGON_STATE_IDLE);

    _pEventLogon = hEventLogon ? EvtFromHandle(hEventLogon) : NULL;

    if (_pEventLogon == NULL && hEventLogon != NULL)
    {
        TraceSz1(Warning, "IpLogon - invalid hEvent %08lX", hEventLogon);
    }

    if (ipaLogon != 0)
    {
        _pSecRegLogon = SecRegLookup(ipaLogon);

        if (_pSecRegLogon == NULL || !_pSecRegLogon->TestFlags(SRF_ONLINESERVER))
        {
            TraceSz1(Warning, "IpLogon - %s is not a valid secure address to an SG", ipaLogon.Str());
            _pSecRegLogon = NULL;
        }
    }

    if (_pSecRegLogon == NULL)
    {
        // This logon is not connecting to an SG providing connection services.  Just mark
        // the state machine as online.

        _uiLogonState = XN_LOGON_STATE_ONLINE;

        if (_pEventLogon)
        {
            EvtSet(_pEventLogon, EVENT_INCREMENT);
        }
    }
    else
    {
        _uiLogonState = XN_LOGON_STATE_PENDING;

        if (pqwUserId != NULL)
        {
            XOUSERINFO * pxouserinfo    = _axouserinfo;
            XOUSERINFO * pxouserinfoEnd = pxouserinfo + dimensionof(_axouserinfo);

            for (; pxouserinfo < pxouserinfoEnd; ++pxouserinfo, ++pqwUserId)
            {
                pxouserinfo->_qwUserId = *pqwUserId;
            }
        }

        // Kick off key exchange to the SG providing connection services.

        Assert(_pSecRegLogon->_bState == SR_STATE_IDLE);
        IpXmitKeyEx(_pSecRegLogon);
    }
}

DWORD CXnIp::IpLogonGetStatus(SGADDR * psgaddr)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    if (psgaddr != NULL)
    {
        if (_pSecRegLogon != NULL && _uiLogonState == XN_LOGON_STATE_ONLINE)
            *psgaddr = _pSecRegLogon->_sgaddr;
        else
            memset(psgaddr, 0, sizeof(SGADDR));
    }

    return(_uiLogonState);
}

BOOL CXnIp::IpLogonGetQFlags(UINT iUserId, ULONGLONG * pqwUserId, DWORD * pdwQFlags, DWORD * pdwSeqQFlags)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    if (iUserId < dimensionof(_axouserinfo))
    {
        XOUSERINFO * pxouserinfo = &_axouserinfo[iUserId];

        if (pqwUserId != NULL)
        {
            *pqwUserId = pxouserinfo->_qwUserId;
        }

        if (pdwQFlags != NULL)
        {
            *pdwQFlags = pxouserinfo->_dwQFlags;
        }

        if (pdwSeqQFlags != NULL)
        {
            *pdwSeqQFlags = pxouserinfo->_dwSeqQFlags;
        }

        return(TRUE);
    }

    return(FALSE);
}

BOOL CXnIp::IpLogonSetQFlags(UINT iUserId, DWORD dwQFlags, DWORD dwSeqQFlags)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    if (iUserId < dimensionof(_axouserinfo))
    {
        XOUSERINFO * pxouserinfo = &_axouserinfo[iUserId];

        if (pxouserinfo->_dwSeqQFlags < dwSeqQFlags)
        {
            pxouserinfo->_dwSeqQFlags = dwSeqQFlags;
            pxouserinfo->_dwQFlags    = dwQFlags;

            if (_pEventLogon)
            {
                EvtSet(_pEventLogon, EVENT_INCREMENT);
            }
        }

        return(TRUE);
    }

    return(FALSE);
}

BOOL CXnIp::IpLogonSetPState(UINT iUserId, DWORD dwPState, const XNKID * pxnkid, UINT cbData, BYTE * pbData)
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    if (iUserId < dimensionof(_axouserinfo) && cbData <= sizeof(_axouserinfo[0]._abData))
    {
        XOUSERINFO * pxouserinfo = &_axouserinfo[iUserId];
        BYTE *       pb          = &_abXbToSgPulse[iUserId];

        if (pxouserinfo->_dwPState != dwPState)
        {
            pxouserinfo->_dwPState = dwPState;

            *pb |= (BYTE)iUserId | XBPULSE_STATE_CHANGE;
        }

        if (memcmp(&pxouserinfo->_xnkid, pxnkid, sizeof(XNKID)) != 0)
        {
            pxouserinfo->_xnkid = *pxnkid;

            *pb |= (BYTE)iUserId | XBPULSE_XNKID_CHANGE;
        }

        if (cbData != pxouserinfo->_cbData || memcmp(pxouserinfo->_abData, pbData, cbData) != 0)
        {
            pxouserinfo->_cbData = cbData;

            if (cbData > 0)
            {
                memcpy(pxouserinfo->_abData, pbData, cbData);
            }

            *pb |= (BYTE)iUserId | XBPULSE_TDATA_CHANGE;
        }

        return(TRUE);
    }

    return(FALSE);
}

void CXnIp::IpLogoff()
{
    ICHECK(IP, USER|UDPC);

    RaiseToDpc();

    _uiLogonState = XN_LOGON_STATE_IDLE;
    _pSecRegLogon = NULL;

    if (_pEventLogon)
    {
        EvtDereference(_pEventLogon);
        _pEventLogon = NULL;
    }

    memset(&_axouserinfo, 0, sizeof(_axouserinfo));
    *(DWORD *)_abXbToSgPulse = 0;
    _dwSeqXbToSg = 0;
    _dwSeqSgToXb = 0;

    SecRegShutdown(TRUE);
}

#endif

// ---------------------------------------------------------------------------------------
// CXnIp::IpSetEventTimer / CXnIp::EventTimer
// ---------------------------------------------------------------------------------------

void CXnIp::IpSetEventTimer(CEventTimer * pEventTimer, WSAEVENT hEvent, DWORD dwTimeout)
{
    ICHECK(IP, USER);

    RaiseToDpc();

    if (pEventTimer->IsNull())
    {
        pEventTimer->Init((PFNTIMER)EventTimer);
    }

    // Release the reference to the existing event.

    if (pEventTimer->_pEvent != NULL)
    {
        EvtDereference(pEventTimer->_pEvent);
        pEventTimer->_pEvent = NULL;
    }

    if (dwTimeout != TIMER_INFINITE)
    {
        // Compute the number of milliseconds until our timer next fires.

        DWORD dwTickKe = KeQueryTickCount() - _dwTickKe;

        dwTickKe = MSEC_PER_TICK - min(MSEC_PER_TICK, dwTickKe);

        // In order to compensate for edge effects between the kernel timer and our timer
        // we add 25 ms to the requested timeout.  This will guarantee that we don't set
        // the event prematurely with respect to the kernel millisecond timer.

        dwTimeout += 25;

        // Now compute the number of whole timer ticks we need to wait beyond the partial
        // tick we are in right now.  Note that we round up to the next timer tick.

        if (dwTimeout < dwTickKe)
        {
            dwTimeout = 0;
        }
        else
        {
            dwTimeout -= dwTickKe;
            dwTimeout += MSEC_PER_TICK - 1;
            dwTimeout  = (dwTimeout * TICKS_PER_SECOND) / 1000;
        }

        // Finally, add in the current timer tick plus one, which is the tick of the next
        // firing of our timer.

        dwTimeout += TimerTick() + 1;

        // Add a reference to the event handle

        pEventTimer->_pEvent = EvtFromHandle(hEvent);

        if (pEventTimer->_pEvent == NULL)
        {
            TraceSz1(Warning, "IpSetEventTimer - invalid hEvent %08lX", hEvent);
        }
    }

    // Set or cancel the timer

    TimerSet(pEventTimer, dwTimeout);
}

void CXnIp::EventTimer(CTimer * pt)
{
    ICHECK(IP, SDPC);

    CEventTimer * pEventTimer = (CEventTimer *)pt;

    if (pEventTimer->_pEvent != NULL)
    {
        EvtSet(pEventTimer->_pEvent, EVENT_INCREMENT);
        EvtDereference(pEventTimer->_pEvent);
        pEventTimer->_pEvent = NULL;
    }
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpRaiseToDpc
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ONLINE

void CXnIp::IpRaiseToDpc(BOOL fRaise)
{
    ICHECK(IP, USER|UDPC);

    Assert(KeGetCurrentIrql() == (fRaise ? PASSIVE_LEVEL : DISPATCH_LEVEL));

    if (fRaise)
        KeRaiseIrqlToDpcLevel();
    else
        KeLowerIrql(PASSIVE_LEVEL);
}

#endif
