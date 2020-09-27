// ---------------------------------------------------------------------------------------
// ipdns.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(dns, 0);
DefineTag(dnsWarn, TAG_ENABLE);
DefineTag(dnsDump, 0);

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_DNS

struct CDnsName
{
    // Definitions -----------------------------------------------------------------------

    #define DNSMAXLABEL     63

    // External --------------------------------------------------------------------------

    INLINE          CDnsName() { _ab[0] = 0; }
    BOOL            InitFromString(const char * psz);
    BYTE *          InitFromLabels(BYTE * pbSrcBeg, BYTE * pbSrcEnd, BYTE * pbLabBeg);
    void            GenerateString(char * psz, UINT cb);
    BOOL            IsEqual(CDnsName * pdnsname);
    UINT            GetSize();

#if DBG
    char *          Str();
#endif

    // Data ------------------------------------------------------------------------------

    BYTE            _ab[255];           // Buffer of octet-prefixed labels
};

struct CDnsReg
{
    // Definitions -----------------------------------------------------------------------

    // External --------------------------------------------------------------------------

    INLINE CDnsReg() {}

    // Data ------------------------------------------------------------------------------

    CDnsReg *       _pdnsregNext;       // Next CDnsReg in linked list
    CTimer          _timer;             // Timer for scheduling timouts
    CDnsName *      _pdnsname;          // Parsed DNS name of host
    BYTE            _bRetry;            // Retry count
    BYTE            _fPending;          // TRUE while request in progress
    WORD            _wId;               // DNS message unique identifier
    PRKEVENT        _pEvent;            // Event reference (may be NULL)
    XNDNS           _xndns;             // XNDNS structure returned to client
};

struct CDnsHdr
{
    // Definitions -----------------------------------------------------------------------

    #define DNSF_RESPONSE       BIT(15) // 1 if response; 0 if query
    #define DNSF_QOP_MASK       0x7800  // 4 bits of query opcode
    #define DNSF_QOP_SQRY       0x0000  // Standard query opcode
    #define DNSF_QOP_IQRY       0x0800  // Inverse query opcode
    #define DNSF_QOP_STAT       0x1000  // Server status request
    #define DNSF_AA             BIT(10) // Authoritative answer
    #define DNSF_TC             BIT(9)  // Truncation
    #define DNSF_RD             BIT(8)  // Recursion desired
    #define DNSF_RA             BIT(7)  // Recursion available
    #define DNSF_ROP_MASK       0x000F  // 4 bits of response opcode
    #define DNSF_ROP_OK         0x0000  // No error response code
    #define DNSF_ROP_FMTERR     0x0001  // Format error response code
    #define DNSF_ROP_SRVFAIL    0x0002  // Server failure response code
    #define DNSF_ROP_NAMEERR    0x0003  // Name error response code
    #define DNSF_ROP_NOTIMPL    0x0004  // Not implemented response code
    #define DNSF_ROP_REFUSED    0x0005  // Refused response code

    #define DNSRT_A             1       // a host address
    #define DNSRT_NS            2       // an authoritative name server
    #define DNSRT_MD            3       // a mail destination (Obsolete - use MX)
    #define DNSRT_MF            4       // a mail forwarder (Obsolete - use MX)
    #define DNSRT_CNAME         5       // the canonical name for an alias
    #define DNSRT_SOA           6       // marks the start of a zone of authority
    #define DNSRT_MB            7       // a mailbox domain name (EXPERIMENTAL)
    #define DNSRT_MG            8       // a mail group member (EXPERIMENTAL)
    #define DNSRT_MR            9       // a mail rename domain name (EXPERIMENTAL)
    #define DNSRT_NULL          10      // a null RR (EXPERIMENTAL)
    #define DNSRT_WKS           11      // a well known service description
    #define DNSRT_PTR           12      // a domain name pointer
    #define DNSRT_HINFO         13      // host information
    #define DNSRT_MINFO         14      // mailbox or mail list information
    #define DNSRT_MX            15      // mail exchange
    #define DNSRT_TXT           16      // text strings

    #define DNSQT_AXFR          252     // a transfer of an entire zone
    #define DNSQT_MAILB         253     // mailbox-related records (MB, MG or MR)
    #define DNSQT_MAILA         254     // mail agent RRs (Obsolete - see MX)
    #define DNSQT_ALL           255     // all records

    #define DNSRC_IN            1       // the Internet
    #define DNSRC_CS            2       // the CSNET class (Obsolete)
    #define DNSRC_CH            3       // the CHAOS class
    #define DNSRC_HS            4       // the Hesiod class
    #define DNSRC_ANY           255     // any class

    // External --------------------------------------------------------------------------

    INLINE CDnsHdr() {}

    // Data ------------------------------------------------------------------------------

    WORD            _wId;               // DNS message unique identifier
    WORD            _wFlags;            // See DNSF_* above
    WORD            _cQd;               // Number of entries in questions section
    WORD            _cAn;               // Number of entries in answers section
    WORD            _cNs;               // Number of entries in authority section
    WORD            _cAr;               // Number of entries in additional section
    
};

// ---------------------------------------------------------------------------------------
// CDnsName
// ---------------------------------------------------------------------------------------

BOOL CDnsName::InitFromString(const char * psz)
{
    BYTE * pb = _ab;
    UINT cb = strlen(psz);

    if ((cb + 2) > sizeof(_ab))
    {
        TraceSz2(dnsWarn, "CDnsName::InitFromString - '%s': String is too %s",
                 psz, cb == 0 ? "small" : "big");
        goto err;
    }

    // Copy the entire string into the buffer offset by one byte.  Prepend a dot for
    // the first label, and copy the NULL string terminator into the buffer.

    _ab[0] = '.';
    memcpy(&_ab[1], psz, cb + 1);

    // Parse each label and prepend its size, overwriting each dot.

    while (*pb != 0)
    {
        // Find the next dot or end of string

        BYTE * pbDot = pb + 1;
        while (*pbDot != 0 && *pbDot != '.')
            ++pbDot;

        // The maximum size of a label is CB_DNSMAXLABEL

        UINT cbLabel = pbDot - (pb + 1);

        if (cbLabel > DNSMAXLABEL)
        {
            *pbDot = 0;
            TraceSz2(dnsWarn, "CDnsName::InitFromString - '%s': Label '%s' is too big",
                     psz, pb + 1);
            goto err;
        }

        if (cbLabel == 0 && *pbDot != 0)
        {
            TraceSz1(dnsWarn, "CDnsName::InitFromString - '%s': Empty label must be the last", psz);
            goto err;
        }

        // Prepend the length of the label and continue at the next dot

        *pb = (BYTE)cbLabel;
        pb  = pbDot;
    }

    // A valid name must contain at least one non-zero label

    if (_ab[0] == 0)
    {
        TraceSz1(dnsWarn, "CDnsName::InitFromString - '%s': Zero labels parsed", psz);
        goto err;
    }

    return(TRUE);

err:
    _ab[0] = 0;
    return(FALSE);
}

BYTE * CDnsName::InitFromLabels(BYTE * pbSrcBeg, BYTE * pbSrcEnd, BYTE * pbLabOrg)
{
    BYTE *  pbLabBeg    = pbLabOrg;
    BYTE *  pbDst       = _ab;
    BYTE *  pbDstEnd    = pbDst + sizeof(_ab);
    BYTE *  pbSrc;
    BYTE *  pbSrcHi     = NULL;
    BYTE    bLen;
    WORD    wLen;
    
chaselabel:

    if (pbLabBeg < pbSrcBeg || pbLabBeg >= pbSrcEnd)
    {
        TraceSz3(dnsWarn, "CDnsName::InitFromLabels - Label at %08lX is out of range [%08lX %08lX)",
                 pbLabBeg, pbSrcBeg, pbSrcEnd);
        goto err;
    }

    pbSrc = pbLabBeg;

nextlabel:

    bLen  = *pbSrc++;

    if ((bLen & 0xC0) == 0xC0)
    {
        // This is a pointer.  Compute the offset from pbSrcBeg and continue there.

        if (pbSrcEnd - pbSrc == 0)
        {
            TraceSz2(dnsWarn, "CDnsName::InitFromLabels - Label at %08lX has pointer spanning boundary (%08lX)",
                     pbLabBeg, pbSrcEnd);
            goto err;
        }

        wLen = ((bLen & 0x3F) << 8) | (*pbSrc++);

        // If this is the first pointer we've chased, then the result of this function
        // will be the current pbSrc pointer.

        if (pbSrcHi == NULL)
        {
            pbSrcHi = pbSrc;
        }

        // Compute the address of the chased label, and limit the pbSrcEnd so that the
        // range of acceptable addresses ends just before the current label.  Then jump
        // back to continue scanning.  Note that boundary conditions are checked later.

        pbSrcEnd = pbLabBeg;
        pbLabBeg = pbSrcBeg + wLen;

        goto chaselabel;
    }

    if ((bLen & 0xC0) != 0)
    {
        TraceSz3(dnsWarn, "CDnsName::InitFromLabels - Label at %08lX has bogus length byte (%02X) at %08lX",
                 pbLabBeg, bLen, pbSrc - 1);
        goto err;
    }

    if (pbDstEnd - pbDst <= bLen)
    {
        TraceSz1(dnsWarn, "CDnsName::InitFromLabels - Label at %08lX is too large", pbLabOrg);
        goto err;
    }

    *pbDst++ = bLen;

    if (bLen > 0)
    {
        memcpy(pbDst, pbSrc, bLen);
        pbDst += bLen;
        pbSrc += bLen;
        goto nextlabel;
    }

    // Finished copying the entire DNS name.  Return a pointer to just after the initial set of
    // bytes consumed.  This will be pbSrcHi if a pointer was chased, else it will be the current
    // pbSrc.

    return(pbSrcHi ? pbSrcHi : pbSrc);

err:
    _ab[0] = 0;
    return(NULL);
}

UINT CDnsName::GetSize()
{
    BYTE * pb = _ab;
    UINT   cb = 1;

    while (*pb != 0)
    {
        cb += *pb + 1;
        pb += *pb + 1;
    }

    return(cb);
}

BOOL CDnsName::IsEqual(CDnsName * pdnsname)
{
    BYTE * pb1 = _ab;
    BYTE * pb2 = pdnsname->_ab;

    while (*pb1 != 0 && *pb2 != 0)
    {
        if (*pb1 != *pb2)
        {
            return(FALSE);
        }

        if (_strnicmp((char *)(pb1 + 1), (char *)(pb2 + 1), *pb1) != 0)
        {
            return(FALSE);
        }

        pb1 += *pb1 + 1;
        pb2 += *pb2 + 1;
    }

    return(TRUE);
}

#if DBG

char * CDnsName::Str()
{
    #define DNSNAME_STR_BUFFS    4
    #define DNSNAME_BUF_SIZE     256
    static char g_chBufDnsName[DNSNAME_STR_BUFFS * DNSNAME_BUF_SIZE];
    static LONG g_lBufIndexDnsName = 0;
    char * pch = &g_chBufDnsName[(InterlockedIncrement(&g_lBufIndexDnsName) % DNSNAME_STR_BUFFS) * DNSNAME_BUF_SIZE];
    char * pchDst = pch;
    BYTE * pb = _ab;
    while (*pb != 0)
    {
        if (pch != pchDst)
        {
            *pchDst++ = '.';
        }

        memcpy(pchDst, pb + 1, *pb);
        pchDst += *pb;
        pb += *pb + 1;
    }
    *pchDst = 0;
    return(pch);
}

#endif

// ---------------------------------------------------------------------------------------
// CXnIp (DNS)
// ---------------------------------------------------------------------------------------

INT CXnIp::IpDnsLookup(const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns)
{
    ICHECK(IP, USER);

    CDnsName dnsname;

    if (!dnsname.InitFromString(pszHost))
    {
        TraceSz1(dnsWarn, "%s is not a valid DNS domain name", pszHost);
        return(WSAEFAULT);
    }

    UINT        cbDnsName   = dnsname.GetSize();
    CDnsReg *   pdnsreg     = (CDnsReg *)PoolAllocZ(sizeof(CDnsReg) + cbDnsName, PTAG_CDnsReg);

    if (pdnsreg == NULL)
    {
        TraceSz(dnsWarn, "Out of memory allocating CDnsReg");
        return(WSAENOBUFS);
    }

    if (hEvent != NULL)
    {
        pdnsreg->_pEvent = EvtFromHandle(hEvent);

        if (pdnsreg->_pEvent == NULL)
        {
            TraceSz1(dnsWarn, "CXnIp::IpDnsLookup - Error referencing WSAEVENT %08lX", hEvent);
            PoolFree(pdnsreg);
            return(WSASYSCALLFAILURE);
        }
    }

    memcpy(pdnsreg + 1, dnsname._ab, cbDnsName);

    pdnsreg->_wId = HTONS(GetNextDgramId());
    pdnsreg->_pdnsname = (CDnsName *)(pdnsreg + 1);
    pdnsreg->_timer.Init((PFNTIMER)IpDnsTimer);
    pdnsreg->_fPending = TRUE;
    pdnsreg->_xndns.iStatus = WSAEINPROGRESS;

    *ppxndns = &pdnsreg->_xndns;

    RaiseToDpc();

    pdnsreg->_pdnsregNext = _pdnsreg;
    _pdnsreg = pdnsreg;

    IpXmitDns(pdnsreg);

    return(0);
}

INT CXnIp::IpDnsRelease(XNDNS * pxndns)
{
    ICHECK(IP, USER);

    RaiseToDpc();

    CDnsReg ** ppdnsreg = &_pdnsreg;
    CDnsReg *  pdnsreg;

    for (; (pdnsreg = *ppdnsreg) != NULL; ppdnsreg = &pdnsreg->_pdnsregNext)
        if (pxndns == &pdnsreg->_xndns)
            break;

    if (pdnsreg == NULL)
    {
        TraceSz1(Warning, "XNDNS %08lX is not valid", pxndns);
        return(WSAEINVAL);
    }

    TimerSet(&pdnsreg->_timer, TIMER_INFINITE);

    if (pdnsreg->_pEvent)
    {
        EvtDereference(pdnsreg->_pEvent);
    }

    *ppdnsreg = pdnsreg->_pdnsregNext;

    PoolFree(pdnsreg);

    return(0);
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpDnsTimer
// ---------------------------------------------------------------------------------------

void CXnIp::IpDnsTimer(CTimer * pt)
{
    ICHECK(IP, UDPC|SDPC);

    CDnsReg * pdnsreg = (CDnsReg *)((BYTE *)pt - offsetof(CDnsReg, _timer));

    pdnsreg->_bRetry += 1;

    if (pdnsreg->_bRetry >= cfgDnsRetries)
    {
        TraceSz3(dns, "IpDnsTimer %04X '%s': Maximum retries exceeded (%d)",
                 NTOHS(pdnsreg->_wId), pdnsreg->_pdnsname->Str(), cfgDnsRetries);
        IpDnsSignal(pdnsreg, WSAETIMEDOUT);
        return;
    }

    IpXmitDns(pdnsreg);
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpXmitDns
// ---------------------------------------------------------------------------------------

void CXnIp::IpXmitDns(CDnsReg * pdnsreg)
{
    ICHECK(IP, UDPC|SDPC);

    if (_options._dnsServerCount == 0)
    {
        TraceSz2(dnsWarn, "IpXmitDns %04X '%s': No DNS servers configured",
                 NTOHS(pdnsreg->_wId), pdnsreg->_pdnsname->Str());
        IpDnsSignal(pdnsreg, WSAHOST_NOT_FOUND);
        return;
    }

    CIpAddr ipaDst = _options._dnsServers[pdnsreg->_bRetry % _options._dnsServerCount];

    TraceSz4(dns, "IpXmitDns %04X '%s' to %s (try #%d)",
             NTOHS(pdnsreg->_wId), pdnsreg->_pdnsname->Str(), ipaDst.Str(), pdnsreg->_bRetry + 1);

    UINT cbDnsName = pdnsreg->_pdnsname->GetSize();
    UINT cbDnsMsg  = sizeof(CDnsHdr) + cbDnsName + 2 * sizeof(WORD);

    CPacket * ppkt = PacketAlloc(PTAG_CDnsPacket, PKTF_TYPE_UDP|PKTF_XMIT_INSECURE|PKTF_POOLALLOC, cbDnsMsg);

    if (ppkt == NULL)
    {
        TraceSz(dnsWarn, "Out of memory allocating DNS packet");
        return;
    }

    CUdpHdr *   pudphdr = ppkt->GetUdpHdr();
    CDnsHdr *   pdnshdr = (CDnsHdr *)(pudphdr + 1);
    BYTE *      pb      = (BYTE *)(pdnshdr + 1);

    pudphdr->_ipportSrc = DNS_CLIENT_PORT;
    pudphdr->_ipportDst = DNS_SERVER_PORT;
    pudphdr->_wLen      = HTONS(sizeof(CUdpHdr) + cbDnsMsg);

    memset(pdnshdr, 0, sizeof(CDnsHdr));
    pdnshdr->_wId       = pdnsreg->_wId;
    pdnshdr->_wFlags    = HTONS(DNSF_RD | DNSF_QOP_SQRY);
    pdnshdr->_cQd       = HTONS(1);

    memcpy(pb, pdnsreg->_pdnsname->_ab, cbDnsName);
    pb                 += cbDnsName;
    *(WORD *)pb         = HTONS(DNSRT_A);
    pb                 += sizeof(WORD);
    *(WORD *)pb         = HTONS(DNSRC_IN);

#ifdef XNET_FEATURE_TRACE
    if (Tag(dnsDump))
    {
        UINT dns = Tag(dns);
        Tag(dns) = Tag(dnsDump);
        IpDnsDump(pdnshdr, cbDnsMsg);
        Tag(dns) = dns;
    }
#endif

    TimerSetRelative(&pdnsreg->_timer, cfgDnsTimeoutInSeconds * TICKS_PER_SECOND);
    IpFillAndXmit(ppkt, ipaDst, IPPROTOCOL_UDP);
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpRecvDns
// ---------------------------------------------------------------------------------------

void CXnIp::IpRecvDns(CPacket * ppkt, CUdpHdr * pUdpHdr, UINT cbLen)
{
    ICHECK(IP, SDPC);

    if (ppkt->TestFlags(PKTF_RECV_BROADCAST) || ppkt->GetIpHdr()->_ipaDst.IsBroadcast())
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet received via broadcast");
        return;
    }

    CDnsHdr * pdnshdr = (CDnsHdr *)(pUdpHdr + 1);

    if (cbLen < sizeof(CDnsHdr))
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet is too small");
        return;
    }

    WORD wFlags = NTOHS(pdnshdr->_wFlags);

    if (    (wFlags & DNSF_RESPONSE) == 0
        ||  (wFlags & DNSF_QOP_MASK) != DNSF_QOP_SQRY
        ||  pdnshdr->_cQd != HTONS(1))
    {
        TraceSz3(pktWarn, "[DISCARD] DNS packet has invalid response (%d,%d,%d)",
                 (wFlags & DNSF_RESPONSE) == 0,
                 (wFlags & DNSF_QOP_MASK) != DNSF_QOP_SQRY,
                 pdnshdr->_cQd != HTONS(1));
        return;
    }

    CDnsReg * pdnsreg = _pdnsreg;

    for (; pdnsreg != NULL; pdnsreg = pdnsreg->_pdnsregNext)
        if (pdnsreg->_wId == pdnshdr->_wId)
            break;
    
    if (pdnsreg == NULL)
    {
        TraceSz1(pktWarn, "[DISCARD] Unrecognized DNS packet with ID %04X", NTOHS(pdnshdr->_wId));
        return;
    }

    if (!pdnsreg->_fPending)
    {
        TraceSz2(pktRecv, "[DISCARD] Ignoring duplicate DNS packet with ID %04X (iStatus=%d)",
                 NTOHS(pdnsreg->_wId), pdnsreg->_xndns.iStatus);
        return;
    }

#ifdef XNET_FEATURE_TRACE
    if (Tag(dnsDump))
    {
        UINT dns = Tag(dns);
        Tag(dns) = Tag(dnsDump);
        IpDnsDump(pdnshdr, cbLen);
        Tag(dns) = dns;
    }
#endif

    if ((wFlags & DNSF_ROP_MASK) == DNSF_ROP_NAMEERR)
    {
        IpDnsSignal(pdnsreg, WSAHOST_NOT_FOUND);
        return;
    }
    else if ((wFlags & DNSF_ROP_MASK) != DNSF_ROP_OK)
    {
        TraceSz1(pktWarn, "[DISCARD] DNS packet with RCODE %d", wFlags & DNSF_ROP_MASK);
        return;
    }

    // Scan the Answer section for DNSRT_A/DNSRC_IN records

    BYTE *      pbBeg = (BYTE *)pdnshdr;
    BYTE *      pbEnd = pbBeg + cbLen;
    BYTE *      pb    = (BYTE *)(pdnshdr + 1);
    CDnsName    dnsname;
    WORD        wType, wClass, wRdLen;
    DWORD       dwTtl;

    // But first, skip over the Question section and verify that the name is correct

    if (pb - pbEnd == 0)
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet has no Question section");
        return;
    }

    pb = dnsname.InitFromLabels(pbBeg, pbEnd, pb);

    if (pb == NULL)
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet has invalid Question name");
        return;
    }
        
    if (pbEnd - pb < sizeof(WORD) + sizeof(WORD))
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet has too small Question section");
        return;
    }

    wType  = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
    wClass = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);

    if (!dnsname.IsEqual(pdnsreg->_pdnsname))
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet has wrong query name in Question section");
        return;
    }

    // Now pb points to the Answer section.  Parse all the answers to find DNSRT_A records.

    WORD cAn = NTOHS(pdnshdr->_cAn);
    UINT cina = 0;

    for (; cAn > 0; --cAn)
    {
        pb = dnsname.InitFromLabels(pbBeg, pbEnd, pb);

        if (pb == NULL)
        {
            TraceSz(pktWarn, "[DISCARD] DNS packet has invalid Answer name");
            return;
        }

        if (pbEnd - pb < sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(WORD))
        {
            TraceSz(pktWarn, "[DISCARD] DNS packet ended prematurely while parsing Answer section (1)");
            return;
        }

        wType  = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
        wClass = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
        dwTtl  = NTOHL(*(DWORD *)pb);   pb += sizeof(DWORD);
        wRdLen = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);

        if (pbEnd - pb < wRdLen)
        {
            TraceSz(pktWarn, "[DISCARD] DNS packet ended prematurely while parsing Answer section (2)");
            return;
        }

        if (wType == DNSRT_A && wClass == DNSRC_IN && wRdLen == sizeof(CIpAddr))
        {
            pdnsreg->_xndns.aina[cina++] = *(IN_ADDR *)pb;

            if (cina == dimensionof(pdnsreg->_xndns.aina))
            {
                TraceSz(pktWarn, "DNS packet had more IN/A answers than XNDNS has room");
                break;
            }
        }

        pb += wRdLen;
    }

    if (cina == 0)
    {
        TraceSz(pktWarn, "[DISCARD] DNS packet had no A/IN answer records");
        return;
    }

    // Signal successful completion.

    pdnsreg->_xndns.cina = cina;
    IpDnsSignal(pdnsreg, 0);
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpDnsSignal
// ---------------------------------------------------------------------------------------

void CXnIp::IpDnsSignal(CDnsReg * pdnsreg, INT iStatus)
{
    ICHECK(IP, UDPC|SDPC);

    Assert(pdnsreg->_fPending);
    Assert(pdnsreg->_xndns.iStatus == WSAEINPROGRESS);
    Assert(iStatus != WSAEINPROGRESS);

    pdnsreg->_fPending = FALSE;
    pdnsreg->_xndns.iStatus = iStatus;
    TimerSet(&pdnsreg->_timer, TIMER_INFINITE);

    if (pdnsreg->_pEvent)
    {
        EvtSet(pdnsreg->_pEvent, EVENT_INCREMENT);
    }
}

// ---------------------------------------------------------------------------------------
// CXnIp::IpDnsDump
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_TRACE

void CXnIp::IpDnsDump(CDnsHdr * pdnshdr, UINT cbDnsMsg)
{
    ICHECK(IP, USER|UDPC|SDPC);

    Assert(cbDnsMsg >= sizeof(CDnsHdr));

    WORD wFlags = NTOHS(pdnshdr->_wFlags);

    TraceSz14(dns, "[DNS %s %d/%s %d/%s%s%s%s%s %04X %d/Qd %d/An %d/Ns %d/Ar]",
              (wFlags & DNSF_RESPONSE) ? "RESP" : "QUERY",
              (wFlags & DNSF_QOP_MASK),
              (wFlags & DNSF_QOP_MASK) == DNSF_QOP_SQRY ? "SQRY" :
              (wFlags & DNSF_QOP_MASK) == DNSF_QOP_IQRY ? "IQRY" :
              (wFlags & DNSF_QOP_MASK) == DNSF_QOP_STAT ? "STAT" : "????",
              (wFlags & DNSF_ROP_MASK),
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_OK ? "OK" :
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_FMTERR ? "FMTERR" :
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_SRVFAIL ? "SRVFAIL" :
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_NAMEERR ? "NAMEERR" :
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_NOTIMPL ? "NOTIMPL" :
              (wFlags & DNSF_ROP_MASK) == DNSF_ROP_REFUSED ? "REFUSED" : "????",
              (wFlags & DNSF_AA) ? " AA" : "",
              (wFlags & DNSF_TC) ? " TC" : "",
              (wFlags & DNSF_RD) ? " RD" : "",
              (wFlags & DNSF_RA) ? " RA" : "",
              NTOHS(pdnshdr->_wId),
              NTOHS(pdnshdr->_cQd),
              NTOHS(pdnshdr->_cAn),
              NTOHS(pdnshdr->_cNs),
              NTOHS(pdnshdr->_cAr));

    BYTE * pbBeg = (BYTE *)pdnshdr;
    BYTE * pbEnd = pbBeg + cbDnsMsg;
    BYTE * pbSec = (BYTE *)(pdnshdr + 1);

    if (NTOHS(pdnshdr->_cQd) != 0)
        pbSec = IpDnsDumpSection(pbBeg, pbEnd, pbSec, NTOHS(pdnshdr->_cQd), "Qd");
    if (pbSec && NTOHS(pdnshdr->_cAn) != 0)
        pbSec = IpDnsDumpSection(pbBeg, pbEnd, pbSec, NTOHS(pdnshdr->_cAn), "An");
    if (pbSec && NTOHS(pdnshdr->_cNs) != 0)
        pbSec = IpDnsDumpSection(pbBeg, pbEnd, pbSec, NTOHS(pdnshdr->_cNs), "Ns");
    if (pbSec && NTOHS(pdnshdr->_cAr) != 0)
        pbSec = IpDnsDumpSection(pbBeg, pbEnd, pbSec, NTOHS(pdnshdr->_cAr), "Ar");
}

char * DnsTypeToStr(WORD wType)
{
    switch (wType)
    {
        case DNSRT_A:       return("A");
        case DNSRT_NS:      return("NS");
        case DNSRT_MD:      return("MD");
        case DNSRT_MF:      return("MF");
        case DNSRT_CNAME:   return("CNAME");
        case DNSRT_SOA:     return("SOA");
        case DNSRT_MB:      return("MB");
        case DNSRT_MG:      return("MG");
        case DNSRT_MR:      return("MR");
        case DNSRT_NULL:    return("NULL");
        case DNSRT_WKS:     return("WKS");
        case DNSRT_PTR:     return("PTR");
        case DNSRT_HINFO:   return("HINFO");
        case DNSRT_MINFO:   return("MINFO");
        case DNSRT_MX:      return("MX");
        case DNSRT_TXT:     return("TXT");
        case DNSQT_AXFR:    return("AXFR");
        case DNSQT_MAILB:   return("MAILB");
        case DNSQT_MAILA:   return("MAILA");
        case DNSQT_ALL:     return("ALL");
        default:            return("?");
    }
}

char * DnsClassToStr(WORD wClass)
{
    switch (wClass)
    {
        case DNSRC_IN:      return("IN");
        case DNSRC_CS:      return("CS");
        case DNSRC_CH:      return("CH");
        case DNSRC_HS:      return("HS");
        case DNSRC_ANY:     return("ANY");
        default:            return("?");
    }
}

BYTE * CXnIp::IpDnsDumpSection(BYTE * pbBeg, BYTE * pbEnd, BYTE * pb, UINT cSec, char * pszSec)
{
    CDnsName    dnsname1, dnsname2;
    UINT        iSec;
    WORD        wType, wClass, wRdLen;
    DWORD       dwTtl;
    
    for (iSec = 0; iSec < cSec; ++iSec)
    {
        if (pb - pbEnd == 0)
            goto err;

        pb = dnsname1.InitFromLabels(pbBeg, pbEnd, pb);
        if (pb == NULL)
            goto err;

        if (strcmp(pszSec, "Qd") == 0)
        {
            if (pbEnd - pb < sizeof(WORD) + sizeof(WORD))
                goto err;

            wType  = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
            wClass = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);

            TraceSz7(dns, "  %s.%d: %-32s %d/%s %d/%s",
                     pszSec, iSec, dnsname1.Str(), wType, DnsTypeToStr(wType),
                     wClass, DnsClassToStr(wClass));
        }
        else
        {
            if (pbEnd - pb < sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(WORD))
                goto err;

            wType  = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
            wClass = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);
            dwTtl  = NTOHL(*(DWORD *)pb);   pb += sizeof(DWORD);
            wRdLen = NTOHS(*(WORD *)pb);    pb += sizeof(WORD);

            if (pbEnd - pb < wRdLen)
                goto err;

            if (wType == DNSRT_A && wClass == DNSRC_IN && wRdLen == sizeof(CIpAddr))
            {
                TraceSz8(dns, "  %s.%d: %-32s %d/%s %d/%s %s",
                         pszSec, iSec, dnsname1.Str(), wType, DnsTypeToStr(wType),
                         wClass, DnsClassToStr(wClass), ((CIpAddr *)pb)->Str());
            }
            else if (   wRdLen > 0
                     && (   wType == DNSRT_NS
                         || wType == DNSRT_MD
                         || wType == DNSRT_MF
                         || wType == DNSRT_CNAME
                         || wType == DNSRT_MB
                         || wType == DNSRT_MG
                         || wType == DNSRT_MR
                         || wType == DNSRT_PTR)
                     && dnsname2.InitFromLabels(pbBeg, pb + wRdLen, pb) == pb + wRdLen)
            {
                TraceSz8(dns, "  %s.%d: %-32s %d/%s %d/%s %s",
                         pszSec, iSec, dnsname1.Str(), wType, DnsTypeToStr(wType),
                         wClass, DnsClassToStr(wClass), dnsname2.Str());
            }
            else
            {
                TraceSz8(dns, "  %s.%d: %-32s %d/%s %d/%s [+%d bytes]",
                         pszSec, iSec, dnsname1.Str(), wType, DnsTypeToStr(wType),
                         wClass, DnsClassToStr(wClass), wRdLen);
                IpDnsDumpHex(pb, wRdLen);
            }

            pb += wRdLen;
        }

    }
        
    return(pb);

err:
    TraceSz2(dns, "  %s.%d: <Error processing record>", pszSec, iSec);
    return(NULL);
}

void CXnIp::IpDnsDumpHex(BYTE * pb, UINT cb)
{
    char * pch;
    char ach[256];
    UINT ib, ibEnd;
    BYTE b;

    while (cb > 0)
    {
        pch = ach;

        ibEnd = 16;
        if (ibEnd > cb)
            ibEnd = cb;

        for (ib = 0; ib < ibEnd; ++ib, ++pb)
        {
            b = (*pb) >> 4;
            *pch++ = (b < 10) ? '0' + b : 'A' + b - 10;
            b = (*pb) & 0xF;
            *pch++ = (b < 10) ? '0' + b : 'A' + b - 10;
            *pch++ = (ib == 7) ? '-' : ' ';
        }

        pb -= ibEnd;

        if (ibEnd < 16)
        {
            memset(pch, ' ', 3 * (16 - ibEnd));
            pch += 3 * (16 - ibEnd);
        }

        *pch++ = ' ';
        *pch++ = ' ';

        for (ib = 0; ib < ibEnd; ++ib, ++pb)
        {
            b = *pb;
            if (b >= 32 && b < 127)
                *pch++ = b;
            else
                *pch++ = '.';
        }

        *pch = 0;

        TraceSz1(dns, "    %s", ach);

        cb -= ibEnd;
    }
}

#endif

// ---------------------------------------------------------------------------------------

#endif
