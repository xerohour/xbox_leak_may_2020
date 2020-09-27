// ---------------------------------------------------------------------------------------
// xnetp.cpp
//
// Implementation of private XNet API's
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(NatPort, 0);
DefineTag(NatFlow, 0);

// ---------------------------------------------------------------------------------------
// Reading and Writing Configuration Sectors
// ---------------------------------------------------------------------------------------

HANDLE _WSAAPI_ XNetOpenConfigVolume()
{
    HANDLE              hVolume;
    OBJECT_ATTRIBUTES   oa;
    OBJECT_STRING       os;
    IO_STATUS_BLOCK     statusBlock;
    NTSTATUS            status;

    RtlInitObjectString(&os, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &os, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&hVolume, SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE, &oa, &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_ALERT);

    if (!NT_SUCCESS(status))
    {
        TraceSz1(Warning, "XNetOpenConfigVolume: NtOpenFile returned %08lX", status);
        hVolume = INVALID_HANDLE_VALUE;
    }

    return(hVolume);
}

BOOL _WSAAPI_ XNetLoadConfigSector(HANDLE hVolume, UINT iSector, BYTE * pbData, UINT cbData)
{
    BYTE                abSector[XBOX_HD_SECTOR_SIZE];
    PXBOX_CONFIG_SECTOR pxbcs = (PXBOX_CONFIG_SECTOR)abSector;
    LARGE_INTEGER       liOffset;
    IO_STATUS_BLOCK     statusBlock;
    NTSTATUS            status;

    Assert(hVolume != INVALID_HANDLE_VALUE);
    Assert(cbData == sizeof(pxbcs->Data));

    if (iSector >= XBOX_NUM_CONFIG_SECTORS)
    {
        TraceSz2(Warning, "XNetSaveConfigSector (%d): iSector must be between 0 and %d",
                 iSector, XBOX_NUM_CONFIG_SECTORS);
        return(FALSE);
    }

    liOffset.QuadPart = (XBOX_CONFIG_SECTOR_INDEX + iSector) * XBOX_HD_SECTOR_SIZE;

    status = NtReadFile(hVolume, 0, NULL, NULL, &statusBlock, abSector, sizeof(abSector), &liOffset);

    if (!NT_SUCCESS(status))
    {
        TraceSz2(Warning, "XNetLoadConfigSector (%d): NtReadFile returned %08lX", iSector, status);
        return(FALSE);
    }

    if (    pxbcs->SectorBeginSignature != XBOX_CONFIG_SECTOR_BEGIN_SIGNATURE
        ||  pxbcs->SectorEndSignature != XBOX_CONFIG_SECTOR_END_SIGNATURE
        ||  pxbcs->Version < XBOX_CONFIG_VERSION
        ||  pxbcs->SectorCount < XBOX_CONFIG_SECTOR_COUNT)
    {
        return(FALSE);
    }

    memcpy(pbData, pxbcs->Data, sizeof(pxbcs->Data));
    return(TRUE);
}

BOOL _WSAAPI_ XNetSaveConfigSector(HANDLE hVolume, UINT iSector, const BYTE * pbData, UINT cbData)
{
    BYTE                abSector[XBOX_HD_SECTOR_SIZE];
    PXBOX_CONFIG_SECTOR pxbcs = (PXBOX_CONFIG_SECTOR)abSector;
    LARGE_INTEGER       liOffset;
    IO_STATUS_BLOCK     statusBlock;
    NTSTATUS            status;

    Assert(hVolume != INVALID_HANDLE_VALUE);
    Assert(cbData == sizeof(pxbcs->Data));

    if (iSector >= XBOX_NUM_CONFIG_SECTORS)
    {
        TraceSz2(Warning, "XNetSaveConfigSector (%d): iSector must be between 0 and %d",
                 iSector, XBOX_NUM_CONFIG_SECTORS);
        return(FALSE);
    }

    liOffset.QuadPart = (XBOX_CONFIG_SECTOR_INDEX + iSector) * XBOX_HD_SECTOR_SIZE;

    pxbcs->SectorBeginSignature = XBOX_CONFIG_SECTOR_BEGIN_SIGNATURE;
    pxbcs->SectorEndSignature = XBOX_CONFIG_SECTOR_END_SIGNATURE;
    pxbcs->Version = XBOX_CONFIG_VERSION;
    pxbcs->SectorCount = XBOX_CONFIG_SECTOR_COUNT;

    memcpy(pxbcs->Data, pbData, sizeof(pxbcs->Data));
    
    // Recompute the checksum of the configuration sector

    pxbcs->Checksum = 0;
    pxbcs->Checksum = ~XConfigChecksum(pxbcs, sizeof(*pxbcs));

    // Write the config sector back to the disk

    status = NtWriteFile(hVolume, 0, NULL, NULL, &statusBlock, abSector, sizeof(abSector), &liOffset);

    if (!NT_SUCCESS(status))
    {
        TraceSz2(Warning, "XNetSaveConfigSector (%d): NtWriteFile returned %08lX", iSector, status);
        return(FALSE);
    }

    return(TRUE);
}

BOOL _WSAAPI_ XNetCloseConfigVolume(HANDLE hVolume)
{
    if (hVolume != INVALID_HANDLE_VALUE)
    {
        NtClose(hVolume);
    }

    return(TRUE);
}

BOOL _WSAAPI_ XNetLoadConfigParams(XNetConfigParams * pxncp)
{
    HANDLE hVolume = XNetOpenConfigVolume();

    if (hVolume != INVALID_HANDLE_VALUE)
    {
        pxncp->dwSigEnd = 0;

        XNetLoadConfigSector(hVolume, 0, (BYTE *)pxncp, sizeof(XNetConfigParams));

        if (pxncp->dwSigEnd != XNET_CONFIG_PARAMS_SIGEND)
        {
            memset(pxncp, 0, sizeof(pxncp));
            pxncp->dwSigEnd = XNET_CONFIG_PARAMS_SIGEND;
        }

        XNetCloseConfigVolume(hVolume);
    }

    return(TRUE);
}

BOOL _WSAAPI_ XNetSaveConfigParams(const XNetConfigParams * pxncp)
{
    BOOL   fResult = FALSE;
    HANDLE hVolume = XNetOpenConfigVolume();

    if (hVolume != INVALID_HANDLE_VALUE)
    {
        Assert(pxncp->dwSigEnd == XNET_CONFIG_PARAMS_SIGEND);

        fResult = XNetSaveConfigSector(hVolume, 0, (BYTE *)pxncp, sizeof(XNetConfigParams));

        XNetCloseConfigVolume(hVolume);
    }

    return(fResult);
}

// ---------------------------------------------------------------------------------------
// XoBase Support
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ONLINE

void CXoBase::XnSetXoBase(CXoBase * pXoBase)
{
    GetXn()->IpSetXoBase(pXoBase);
}

void CXoBase::XnLogon(IN_ADDR inaLogon, ULONGLONG * pqwUserId, WSAEVENT hEventLogon)
{
    GetXn()->IpLogon(CIpAddr(inaLogon.s_addr), pqwUserId, hEventLogon);
}

DWORD CXoBase::XnLogonGetStatus(SGADDR * psgaddr)
{
    return(GetXn()->IpLogonGetStatus(psgaddr));
}

BOOL CXoBase::XnLogonGetQFlags(UINT iUserId, ULONGLONG * pqwUserId, DWORD * pdwQFlags, DWORD * pdwSeqQFlags)
{
    return(GetXn()->IpLogonGetQFlags(iUserId, pqwUserId, pdwQFlags, pdwSeqQFlags));
}

BOOL CXoBase::XnLogonSetQFlags(UINT iUserId, DWORD dwQFlagsSet, DWORD dwQFlagsClr)
{
    return(GetXn()->IpLogonSetQFlags(iUserId, dwQFlagsSet, dwQFlagsClr));
}

BOOL CXoBase::XnLogonSetPState(UINT iUserId, DWORD dwPState, const XNKID * pxnkid, UINT cbData, BYTE * pbData)
{
    return(GetXn()->IpLogonSetPState(iUserId, dwPState, pxnkid, cbData, pbData));
}

void CXoBase::XnLogoff()
{
    GetXn()->IpLogoff();
}

void CXoBase::XnSetEventTimer(BYTE * pbEventTimer, WSAEVENT hEvent, DWORD dwTimeout)
{
    GetXn()->IpSetEventTimer((CEventTimer *)pbEventTimer, hEvent, dwTimeout);
}

void CXoBase::XnRaiseToDpc(BOOL fRaise)
{
    GetXn()->IpRaiseToDpc(fRaise);
}

#ifdef XNET_FEATURE_ASSERT

void * CXoBase::XnLeakAdd(CLeakInfo * pli, void * pv, UINT cb, ULONG tag)
{
    return(GetXn()->LeakAdd(pli, pv, cb, tag));
}

void * CXoBase::XnLeakDel(CLeakInfo * pli, void * pv)
{
    return(GetXn()->LeakDel(pli, pv));
}

void CXoBase::XnLeakTerm(CLeakInfo * pli)
{
    GetXn()->LeakTerm(pli);
}

#endif

#endif

// ---------------------------------------------------------------------------------------
// Simulated Network Address Translator
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_WINDOWS

#include <pshpack1.h>

struct CDhcpHdr
{
    // Definitions -----------------------------------------------------------------------

    #define DHCP_OP_BOOTREQUEST         1
    #define DHCP_OP_BOOTREPLY           2

    #define DHCP_HWTYPE_ETHERNET        1
    #define DHCP_FLAGS_BROADCAST        0x0008
    #define DHCP_MAGIC_COOKIE           0x63538263

    #define DHCP_TYPE_DISCOVER          1
    #define DHCP_TYPE_OFFER             2
    #define DHCP_TYPE_REQUEST           3
    #define DHCP_TYPE_DECLINE           4
    #define DHCP_TYPE_ACK               5
    #define DHCP_TYPE_NAK               6
    #define DHCP_TYPE_RELEASE           7
    #define DHCP_TYPE_INFORM            8

    #define DHCP_OPT_PAD                0
    #define DHCP_OPT_SUBNET_MASK        1
    #define DHCP_OPT_ROUTERS            3
    #define DHCP_OPT_DNS_SERVERS        6
    #define DHCP_OPT_DOMAIN_NAME        15
    #define DHCP_OPT_REQUESTED_IPADDR   50
    #define DHCP_OPT_IPADDR_LEASE_TIME  51
    #define DHCP_OPT_FIELD_OVERLOAD     52
    #define DHCP_OPT_MESSAGE_TYPE       53
    #define DHCP_OPT_SERVERID           54
    #define DHCP_OPT_PARAM_REQUEST_LIST 55
    #define DHCP_OPT_T1_INTERVAL        58
    #define DHCP_OPT_T2_INTERVAL        59
    #define DHCP_OPT_CLIENTID           61
    #define DHCP_OPT_END                255

    #define DHCP_OVERLOAD_FILE          1
    #define DHCP_OVERLOAD_SERVER        2

    // Functions -------------------------------------------------------------------------

    BOOL        GetOpt(UINT cbLen, BYTE bOpt, UINT cbOpt, BOOL fVariable, BYTE ** ppbOpt, UINT * pcbOpt);
    static BOOL GetOpt(BYTE * pb, UINT cb, BYTE bOpt, UINT cbOpt, BOOL fVar, BYTE ** ppbOpt, UINT * pcbOpt, BYTE * pbOver);

    // Data ------------------------------------------------------------------------------

    BYTE        _bOp;               // DHCP_OP_*
    BYTE        _bHwType;           // DHCP_HWTYPE_*
    BYTE        _bHwLen;            // hardware address length
    BYTE        _bHops;             // relay hops
    DWORD       _dwXid;             // random transaction identifier
    WORD        _wSecs;             // seconds since address acquisition process began
    WORD        _wFlags;            // DHCP_FLAGS_*
    CIpAddr     _ipaC;              // IP address requested by client
    CIpAddr     _ipaY;              // IP address assigned to client
    CIpAddr     _ipaS;              // IP address of DHCP server
    CIpAddr     _ipaG;              // IP address of gateway in trans-router booting
    CEnetAddr   _ea;                // Ethernet Address of client
    BYTE        _abHwPad[10];       // Pad hardware address to full 16 bytes
    char        _achServer[64];     // Null-terminated server hostname
    char        _achFile[128];      // Null-terminated name of bootfile
    DWORD       _dwMagicCookie;     // DHCP_MAGIC_COOKIE

};

#include <poppack.h>

BOOL CDhcpHdr::GetOpt(UINT cbLen, BYTE bOpt, UINT cbOpt, BOOL fVar, BYTE ** ppbOpt, UINT * pcbOpt)
{
    BYTE bOver = 0;

    if (GetOpt((BYTE *)(this + 1), cbLen - sizeof(CDhcpHdr), bOpt, cbOpt, fVar, ppbOpt, pcbOpt, &bOver))
    {
        return(TRUE);
    }

    if (bOver & DHCP_OVERLOAD_FILE)
    {
        if (GetOpt((BYTE *)_achFile, sizeof(_achFile), bOpt, cbOpt, fVar, ppbOpt, pcbOpt, NULL))
        {
            return(TRUE);
        }
    }

    if (bOver & DHCP_OVERLOAD_SERVER)
    {
        if (GetOpt((BYTE *)_achServer, sizeof(_achServer), bOpt, cbOpt, fVar, ppbOpt, pcbOpt, NULL))
        {
            return(TRUE);
        }
    }

    return(FALSE);
}

BOOL CDhcpHdr::GetOpt(BYTE * pb, UINT cb, BYTE bOpt, UINT cbOpt, BOOL fVar, BYTE ** ppbOpt, UINT * pcbOpt, BYTE * pbOver)
{
    BYTE b1, b2;

    while (cb > 0)
    {
        b1  = *pb++;
        cb -= 1;

        if (b1 == DHCP_OPT_END)
            break;

        if (b1 == DHCP_OPT_PAD)
            continue;

        if (cb == 0)
        {
            TraceSz1(Warning, "CDhcpHdr::GetOpt(%d) - Premature end of option list", bOpt);
            break;
        }

        b2  = *pb++;
        cb -= 1;

        if (b2 == 0 || b2 > cb)
        {
            TraceSz3(Warning, "CDhcpHdr::GetOpt(%d) - Option length (%d) is invalid (cb %d)", bOpt, b2, cb);
            break;
        }

        if (b1 == DHCP_OPT_FIELD_OVERLOAD && pbOver && *pbOver == 0 && b2 == 1)
        {
            *pbOver = *pb;
        }

        if (b1 == bOpt)
        {
            if (    (!fVar && b2 != cbOpt)
                ||  ( fVar && (b2 < cbOpt || (b2 % cbOpt) != 0)))
            {
                TraceSz4(Warning, "CDhcpHdr::GetOpt(%d) - Option length (%d) is invalid (%d%s)",
                         bOpt, b2, cbOpt, fVar ? " variable" : "");
                break;
            }

            *ppbOpt = pb;

            if (pcbOpt)
            {
                *pcbOpt = b2;
            }

            return(TRUE);
        }

        pb += b2;
        cb -= b2;
    }

    return(FALSE);
}

class CXnNat
{
    // Definitions -----------------------------------------------------------------------

    struct CDhcpLease
    {
        CEnetAddr       _ea;                // Ethernet address of client
        CIpAddr         _ipa;               // IP address assigned to client
        FILETIME        _ftExpire;          // Expiration time of the lease
    };

    struct CNatFilt
    {
        CIpAddr         _ipaOuter;          // IP address of server on outer lan
        CIpPort         _ipportOuter;       // IP port of server on outer lan
        DWORD           _dwTick;            // Tick of last packet activity through this filter
    };

    struct CNatPort
    {
        LIST_ENTRY      _leHash;            // Link into hash table bucket
        LIST_ENTRY      _leLru;             // Link into LRU list
        CIpAddr         _ipaInner;          // IP address of client on inner lan
        CIpPort         _ipportInner;       // IP port of client on inner lan
        CIpPort         _ipportNat;         // IP port assigned by NAT
        CIpAddr         _ipaOuter;          // IP address of server on outer lan (aggressive) 
        CIpPort         _ipportOuter;       // IP port of server on outer lan (aggressive)
        CNatFilt *      _pNatFilt;          // Vector of sorted CNatFilt structures
        UINT            _cNatFilt;          // Count of CNatFilt structures in _pNatFilt in use
        UINT            _cNatFiltAlloc;     // Count of CNatFilt structures in _pNatFilt
        DWORD           _dwTick;            // Tick of last packet activity through this mapping
    };

    class CXnIpNat : public CXnIp
    {

    public:

        HAL_DECLARE_NEW_DELETE(CXnIpNat)

        BOOL    Init(CXnNat * pXnNat, XNetStartupParams * pxnsp, XNetConfigParams * pxncp, char * pszXbox);
        void    Stop();
        void    Term();
        void    UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
        void    TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen);
        void    SockReset(CIpAddr ipa) {}
        BOOL    IsGateway(CIpPort ipport);
        void    IpXmitIp(CPacket * ppkt) { CXnIp::IpXmitIp(ppkt); }

        CIpAddr GetIpa()                { return(_ipa); }
        CIpAddr GetIpaMask()            { return(_ipaMask); }
        CIpAddr GetIpaDns(UINT iDns)    { return(iDns < _options._dnsServerCount ? _options._dnsServers[iDns] : 0); }

    private:

        CXnNat *    _pXnNat;

    };

public:

    HAL_DECLARE_NEW_DELETE(CXnNat)

    BOOL        Init(NATCREATE * pNatCreate);
    void        Term();
    void        Recv(CXnIpNat * pXnIpNat, CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    void        RecvDhcp(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    void        RecvInner(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    void        RecvOuter(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    BOOL        IsGateway(CIpPort ipport, CXnIpNat * pXnIpNat);
    CNatPort *  NatPortLookup(CIpAddr ipaInner, CIpPort ipportInner, CIpAddr ipaOuter, CIpPort ipportOuter);
    CNatPort *  NatPortLookup(CIpPort ipportNat);
    BOOL        NatPortExpire(CNatPort * pNatPort);
    void        NatPortFree(CNatPort * pNatPort);
    CNatFilt *  NatFiltLookup(CNatPort * pNatPort, CIpAddr ipaOuter, CIpPort ipportOuter, BOOL fAlloc);

private:

    CXnIpNat *      _pXnIpNatInner;         // Inner network stack
    CXnIpNat *      _pXnIpNatOuter;         // Outer network stack
    DWORD           _dwIpaBase;             // Base IP address (host order) of DHCP server
    DWORD           _dwIpaLast;             // Last IP address (host order) of DHCP server
    DWORD           _dwIpaNext;             // Next IP address (host order) of DHCP server
    DWORD           _dwLeaseTime;           // Lease time in seconds for DHCP server
    CDhcpLease *    _pDhcpLease;            // Vector of CDhcpLease structures
    UINT            _cDhcpLease;            // Count of CDhcpLease structures in use
    UINT            _cDhcpLeaseAlloc;       // Count of CDhcpLease structures allocated
    UINT            _iAssign;               // See NAT_ASSIGN_*
    UINT            _iFilter;               // See NAT_FILTER_*
    UINT            _iTimeout;              // NAT timeout interval in ticks
    LIST_ENTRY *    _pleHash;               // Vector of hash buckets of CNatPort structures
    UINT            _cBucket;               // Number of buckets in the hash table
    LIST_ENTRY      _leLru;                 // CNatPort structures in LRU order
    CNatPort **     _ppNatPort;             // Mapping from ipportNat to CNatPort
    UINT            _iNatPortBase;          // Base ipportNat to assign
    UINT            _iNatPortLast;          // Last ipportNat to assign
    UINT            _iNatPortNext;          // Next ipportNat to assign
    UINT            _cNatPortFree;          // Count of free ipportNat
    BOOL            _fInitialized;          // TRUE when CXnNat::Init has completed

};

BOOL CXnNat::CXnIpNat::Init(CXnNat * pXnNat, XNetStartupParams * pxnsp, XNetConfigParams * pxncp, char * pszXbox)
{
    _pXnNat = pXnNat;

    XNetInitParams xnip;
    xnip.pxnp = (XNetParams *)pxnsp;
    xnip.pszXbox = pszXbox;
    HalQueryTsc(&xnip.liTime);

    NTSTATUS status = IpInit(&xnip);

    if (!NT_SUCCESS(status))
    {
        TraceSz1(Warning, "CXnNat::CXnIpNat::Init - IpInit failed (%08lX)", status);
        return(FALSE);
    }

    cfgFlags |= XNET_STARTUP_BYPASS_SECURITY;

    INT err = IpConfig(pxncp, XNET_CONFIG_NORMAL);

    if (err != 0)
    {
        TraceSz1(Warning, "CXnNat::CXnIpNat::Init - IpConfig failed (%08lX)", err);
        return(FALSE);
    }

    IpStart();

    return(TRUE);    
}

void CXnNat::CXnIpNat::Stop()
{
    if (TestInitFlag(INITF_HAL))
    {
        RaiseToDpc();
        IpStop();
    }
}

void CXnNat::CXnIpNat::Term()
{
    if (TestInitFlag(INITF_HAL))
    {
        RaiseToDpc();
        IpTerm();
    }

    _pXnNat = NULL;
}

void CXnNat::CXnIpNat::UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    _pXnNat->Recv(this, ppkt, pIpHdr, pUdpHdr, cbLen);
}

void CXnNat::CXnIpNat::TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen)
{
    _pXnNat->Recv(this, ppkt, pIpHdr, (CUdpHdr *)pTcpHdr, cbLen);
}

BOOL CXnNat::CXnIpNat::IsGateway(CIpPort ipport)
{
    return(_pXnNat->IsGateway(ipport, this));
}

BOOL CXnNat::Init(NATCREATE * pNatCreate)
{
    _dwIpaBase     = NTOHL(pNatCreate->_inaBase.s_addr);
    _dwIpaLast     = NTOHL(pNatCreate->_inaLast.s_addr);
    _dwIpaNext     = _dwIpaBase;
    _dwLeaseTime   = pNatCreate->_dwLeaseTime;
    _iAssign       = pNatCreate->_iAssign;
    _iFilter       = pNatCreate->_iFilter;
    _iTimeout      = (pNatCreate->_iTimeout ? pNatCreate->_iTimeout : 60) * TICKS_PER_SECOND;
    _iNatPortBase  = pNatCreate->_iNatPortBase ? pNatCreate->_iNatPortBase : 1000;
    _iNatPortLast  = _iNatPortBase + (pNatCreate->_cNatPort ? pNatCreate->_cNatPort : 1000) - 1;
    _iNatPortNext  = _iNatPortBase;

    if (_iAssign < NAT_ASSIGN_MINIMAL && _iAssign > NAT_ASSIGN_AGGRESSIVE)
        _iAssign = NAT_ASSIGN_MINIMAL;
    if (_iFilter < NAT_FILTER_NONE || _iFilter > NAT_FILTER_ADDRESS_PORT)
        _iFilter = NAT_FILTER_NONE;

    if (_iNatPortLast > 0xFFFF)
        _iNatPortLast = 0xFFFF;

    _cNatPortFree  = _iNatPortLast - _iNatPortBase + 1;
    _cBucket       = _cNatPortFree * 160 / 100;

    static DWORD s_adwPrimes[] = { 37,59,89,139,227,359,577,929,1499,2423,3919,6337,10253,16573,
        26821,43391,70207,113591,183797,297377,481171,778541,1259701,2038217,3297913,5336129,
        8633983,13970093,22604069,36574151,59178199,95752333,154930511,250682837,405613333,
        656296153,1061909479,1718205583,2780115059,0xFFFFFFFF};

    DWORD * pdwPrime = s_adwPrimes;
    for (; _cBucket < *pdwPrime; pdwPrime++) ;
    _cBucket = *pdwPrime;

    _pleHash       = (LIST_ENTRY *)HalAllocZ(_cBucket * sizeof(LIST_ENTRY), PTAG_CNatPort);
    _ppNatPort     = (CNatPort **)HalAllocZ(_cNatPortFree * sizeof(CNatPort *), PTAG_CNatPort);
    _pXnIpNatInner = new CXnIpNat;
    _pXnIpNatOuter = new CXnIpNat;

    if (_pleHash == NULL || _ppNatPort == NULL || _pXnIpNatInner == NULL || _pXnIpNatOuter == NULL)
    {
        TraceSz(Warning, "CXnNat::Init - Out of memory");
        return(FALSE);
    }

    InitializeListHead(&_leLru);

    LIST_ENTRY *    ple     = _pleHash;
    UINT            cBucket = _cBucket;

    for (; cBucket > 0; --cBucket, ++ple)
    {
        InitializeListHead(ple);
    }

    if (!_pXnIpNatInner->Init(this, pNatCreate->_pxnsp, &pNatCreate->_xncpInner, pNatCreate->_achXboxInner))
    {
        return(FALSE);
    }

    if (!_pXnIpNatOuter->Init(this, pNatCreate->_pxnsp, &pNatCreate->_xncpOuter, pNatCreate->_achXboxOuter))
    {
        return(FALSE);
    }

    _fInitialized = TRUE;

    return(TRUE);
}

void CXnNat::Term()
{
    if (_pXnIpNatInner)
    {
        _pXnIpNatInner->Stop();
    }

    if (_pXnIpNatOuter)
    {
        _pXnIpNatOuter->Stop();
    }

    if (_pXnIpNatInner)
    {
        _pXnIpNatInner->Term();
        delete _pXnIpNatInner;
        _pXnIpNatInner = NULL;
    }

    if (_pXnIpNatOuter)
    {
        _pXnIpNatOuter->Term();
        delete _pXnIpNatOuter;
        _pXnIpNatOuter = NULL;
    }

    if (!IsListNull(&_leLru))
    {
        while (!IsListEmpty(&_leLru))
        {
            CNatPort * pNatPort = (CNatPort *)((BYTE *)_leLru.Flink - offsetof(CNatPort, _leLru));
            NatPortFree(pNatPort);
        }
    }

    if (_ppNatPort)
    {
        HalFree(_ppNatPort);
    }

    if (_pDhcpLease)
    {
        HalFree(_pDhcpLease);
    }
}

void CXnNat::Recv(CXnIpNat * pXnIpNat, CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (!_fInitialized)
        return;

    if (pXnIpNat == _pXnIpNatOuter)
        RecvOuter(ppkt, pIpHdr, pUdpHdr, cbLen);
    else if (pIpHdr->_bProtocol == IPPROTOCOL_UDP && pUdpHdr->_ipportDst == DHCP_SERVER_PORT)
        RecvDhcp(ppkt, pIpHdr, pUdpHdr, cbLen);
    else
        RecvInner(ppkt, pIpHdr, pUdpHdr, cbLen);
}

void CXnNat::RecvDhcp(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (cbLen < sizeof(CDhcpHdr))
    {
        TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - DHCP payload is too small");
        return;
    }

    CDhcpHdr * pDhcpHdr = (CDhcpHdr *)(pUdpHdr + 1);

    if (    pDhcpHdr->_bOp != DHCP_OP_BOOTREQUEST
        ||  pDhcpHdr->_bHwType != DHCP_HWTYPE_ETHERNET
        ||  pDhcpHdr->_bHwLen != sizeof(CEnetAddr)
        ||  pDhcpHdr->_dwMagicCookie != DHCP_MAGIC_COOKIE)
    {
        TraceSz4(pktWarn, "[DISCARD] CXnNat::RecvDhcp - DHCP request is invalid (%d,%d,%d,%d)",
                 (pDhcpHdr->_bOp != DHCP_OP_BOOTREQUEST),
                 (pDhcpHdr->_bHwType != DHCP_HWTYPE_ETHERNET),
                 (pDhcpHdr->_bHwLen != sizeof(CEnetAddr)),
                 (pDhcpHdr->_dwMagicCookie != DHCP_MAGIC_COOKIE));
        return;
    }

    BYTE * pbOpt;
    UINT   cbOpt;
    BYTE   bType;

    if (!pDhcpHdr->GetOpt(cbLen, DHCP_OPT_MESSAGE_TYPE, 1, FALSE, &pbOpt, NULL))
    {
        TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - DHCP_OPT_MESSAGE_TYPE not found");
        return;
    }

    bType = *pbOpt;

    if (bType != DHCP_TYPE_DISCOVER && bType != DHCP_TYPE_REQUEST)
    {
        TraceSz1(pktWarn, "[DISCARD] CXnNat::RecvDhcp - DHCP_OPT_MESSAGE_TYPE (%d) not supported", bType);
        return;
    }

    if (_pXnIpNatOuter->GetIpa() == 0)
    {
        TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - NAT hasn't acquired outer IP address yet");
        return;
    }

    CDhcpLease * pDhcpLease = _pDhcpLease;
    UINT         cDhcpLease = _cDhcpLease;

    for (; cDhcpLease > 0; ++pDhcpLease, --cDhcpLease)
    {
        if (pDhcpLease->_ea.IsEqual(pDhcpHdr->_ea))
            break;
    }

    if (cDhcpLease == 0)
    {
        if (bType != DHCP_TYPE_DISCOVER)
        {
            TraceSz1(pktWarn, "[DISCARD] CXnNat::RecvDhcp - No lease found for %s", pDhcpHdr->_ea.Str());
            return;
        }

        for (; _dwIpaNext <= _dwIpaLast; ++_dwIpaNext)
        {
            if (CIpAddr(HTONL(_dwIpaNext)).IsValidAddr())
                break;
        }

        if (_dwIpaNext > _dwIpaLast)
        {
            TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - Out of IP addresses");
            return;
        }

        if (_cDhcpLease == _cDhcpLeaseAlloc)
        {
            UINT         cDhcpLeaseAlloc = _cDhcpLeaseAlloc + 64;
            CDhcpLease * pDhcpLease      = (CDhcpLease *)HalAllocZ(cDhcpLeaseAlloc * sizeof(CDhcpLease), PTAG_CDhcpLease);

            if (pDhcpLease == NULL)
            {
                TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - Out of memory allocating CDhcpLease vector");
                return;
            }

            if (_cDhcpLease > 0)
            {
                memcpy(pDhcpLease, _pDhcpLease, _cDhcpLease * sizeof(CDhcpLease));
                HalFree(_pDhcpLease);
            }

            _pDhcpLease      = pDhcpLease;
            _cDhcpLeaseAlloc = cDhcpLeaseAlloc;
        }

        pDhcpLease       = &_pDhcpLease[_cDhcpLease];
        pDhcpLease->_ea  = pDhcpHdr->_ea;
        pDhcpLease->_ipa = HTONL(_dwIpaNext);
        _dwIpaNext      += 1;
        _cDhcpLease     += 1;
    }

    if (bType == DHCP_TYPE_REQUEST)
    {
        if (!pDhcpHdr->GetOpt(cbLen, DHCP_OPT_REQUESTED_IPADDR, sizeof(CIpAddr), FALSE, &pbOpt, NULL))
        {
            TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - REQUESTED_IPADDR not supplied");
            return;
        }

        if (*(CIpAddr *)pbOpt != pDhcpLease->_ipa)
        {
            TraceSz2(pktWarn, "[DISCARD] CXnNat::RecvDhcp - REQUESTED_IPADDR is %s, expecting %s",
                     *(CIpAddr *)pbOpt, pDhcpLease->_ipa);
            return;
        }
    }

    CPacket * ppktReply = _pXnIpNatInner->PacketAlloc(PTAG_CDhcpPacket,
        PKTF_TYPE_UDP|PKTF_XMIT_INSECURE|PKTF_POOLALLOC, sizeof(CDhcpHdr) + 128);

    if (ppktReply == NULL)
    {
        TraceSz(pktWarn, "[DISCARD] CXnNat::RecvDhcp - Out of memory allocating CDhcpPacket");
        return;
    }

    CIpHdr *    pIpHdrReply     = ppktReply->GetIpHdr();
    CUdpHdr *   pUdpHdrReply    = (CUdpHdr *)(pIpHdrReply + 1);
    CDhcpHdr *  pDhcpHdrReply   = (CDhcpHdr *)(pUdpHdrReply + 1);
    BYTE *      pbOptReply      = (BYTE *)(pDhcpHdrReply + 1);

    pUdpHdrReply->_ipportSrc = DHCP_SERVER_PORT;
    pUdpHdrReply->_ipportDst = DHCP_CLIENT_PORT;
    
    memset(pDhcpHdrReply, 0, sizeof(CDhcpHdr));
    pDhcpHdrReply->_bOp = DHCP_OP_BOOTREPLY;
    pDhcpHdrReply->_bHwType = DHCP_HWTYPE_ETHERNET;
    pDhcpHdrReply->_bHwLen = sizeof(CEnetAddr);
    pDhcpHdrReply->_dwXid = pDhcpHdr->_dwXid;
    pDhcpHdrReply->_wFlags = pDhcpHdr->_wFlags;
    pDhcpHdrReply->_ipaY = pDhcpLease->_ipa;
    pDhcpHdrReply->_ea = pDhcpLease->_ea;
    pDhcpHdrReply->_dwMagicCookie = DHCP_MAGIC_COOKIE;

    *pbOptReply++ = DHCP_OPT_MESSAGE_TYPE;
    *pbOptReply++ = 1;
    *pbOptReply++ = bType == DHCP_TYPE_DISCOVER ? DHCP_TYPE_OFFER : DHCP_TYPE_ACK;

    *pbOptReply++ = DHCP_OPT_IPADDR_LEASE_TIME;
    *pbOptReply++ = sizeof(DWORD);
    *(DWORD *)pbOptReply = HTONL(_dwLeaseTime);
    pbOptReply += sizeof(DWORD);

    *pbOptReply++ = DHCP_OPT_SERVERID;
    *pbOptReply++ = sizeof(CIpAddr);
    *(CIpAddr *)pbOptReply = _pXnIpNatInner->GetIpa();
    pbOptReply += sizeof(CIpAddr);

    if (pDhcpHdr->GetOpt(cbLen, DHCP_OPT_PARAM_REQUEST_LIST, 1, TRUE, &pbOpt, &cbOpt))
    {
        for (; cbOpt > 0; ++pbOpt, --cbOpt)
        {
            switch (*pbOpt)
            {
                case DHCP_OPT_SUBNET_MASK:
                    *pbOptReply++ = DHCP_OPT_SUBNET_MASK;
                    *pbOptReply++ = sizeof(CIpAddr);
                    *(CIpAddr *)pbOptReply = _pXnIpNatInner->GetIpaMask();
                    pbOptReply += sizeof(CIpAddr);
                    break;

                case DHCP_OPT_ROUTERS:
                    *pbOptReply++ = DHCP_OPT_ROUTERS;
                    *pbOptReply++ = sizeof(CIpAddr);
                    *(CIpAddr *)pbOptReply = _pXnIpNatInner->GetIpa();
                    pbOptReply += sizeof(CIpAddr);
                    break;

                case DHCP_OPT_DNS_SERVERS:
                {
                    CIpAddr *   pipa = (CIpAddr *)(pbOptReply + 2);
                    UINT        iDns;

                    for (iDns = 0; iDns < MAX_DEFAULT_DNSSERVERS; ++iDns)
                    {
                        CIpAddr ipaDns = _pXnIpNatOuter->GetIpaDns(iDns);

                        if (ipaDns == 0)
                            break;

                        *pipa++ = ipaDns;
                    }

                    if (iDns > 0)
                    {
                        *pbOptReply++ = DHCP_OPT_DNS_SERVERS;
                        *pbOptReply++ = iDns * sizeof(CIpAddr);
                        pbOptReply += iDns * sizeof(CIpAddr);
                    }
                    break;
                }
            }
        }
    }

    *pbOptReply++ = DHCP_OPT_END;

    ppktReply->SetCb(pbOptReply - (BYTE *)pIpHdrReply);
    pUdpHdrReply->_wLen = NTOHS(pbOptReply - (BYTE *)pUdpHdrReply);

    _pXnIpNatInner->IpFillAndXmit(ppktReply, pIpHdr->_ipaSrc ? pIpHdr->_ipaSrc : IPADDR_BROADCAST, IPPROTOCOL_UDP);
}

void CXnNat::RecvInner(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (    ppkt->TestFlags(PKTF_RECV_BROADCAST|PKTF_RECV_LOOPBACK)
        ||  ppkt->GetHdrOptLen() > 0
        ||  pIpHdr->_ipaDst == _pXnIpNatInner->GetIpa())
        return;

    CIpAddr     ipaInner    = pIpHdr->_ipaSrc;
    CIpPort     ipportInner = pUdpHdr->_ipportSrc;
    CIpAddr     ipaOuter    = pIpHdr->_ipaDst;
    CIpPort     ipportOuter = pUdpHdr->_ipportDst;
    DWORD       dwTick      = _pXnIpNatInner->TimerTick();

    CNatPort * pNatPort = NatPortLookup(ipaInner, ipportInner, ipaOuter, ipportOuter);

    if (pNatPort == NULL)
    {
        TraceSz5(Warning, "[DISCARD] Cannot allocate CNatPort for outbound %s [%s:%d %s:%d]",
                 pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                 ipaInner.Str(), NTOHS(ipportInner), ipaOuter.Str(), NTOHS(ipportOuter));
        return;
    }

    pNatPort->_dwTick = dwTick;

    if (_iFilter)
    {
        CNatFilt * pNatFilt = NatFiltLookup(pNatPort, ipaOuter, ipportOuter, TRUE);

        if (pNatFilt == NULL)
        {
            TraceSz5(Warning, "[DISCARD] Out of memory allocating CNatFilt for outbound %s [%s:%d %s:%d]",
                     pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                     ipaInner.Str(), NTOHS(ipportInner), ipaOuter.Str(), NTOHS(ipportOuter));
            return;
        }

        pNatFilt->_dwTick = dwTick;
    }

    // Copy the packet, change source address and port, and forward to _pXnIpNatOuter

    CPacket * ppktNat = _pXnIpNatOuter->PacketAlloc(PTAG_CNatPacket,
        PKTF_TYPE_IP|PKTF_POOLALLOC|PKTF_XMIT_INSECURE, ppkt->GetCb() - sizeof(CIpHdr));

    if (ppktNat == NULL)
    {
        TraceSz5(pktWarn, "[DISCARD] Out of memory allocating CNatPacket for outbound %s [%s:%d %s:%d]",
                 pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                 ipaInner.Str(), NTOHS(ipportInner), ipaOuter.Str(), NTOHS(ipportOuter));
        return;
    }

    ppktNat->SetType((WORD)(pIpHdr->_bProtocol == IPPROTOCOL_UDP ? PKTF_TYPE_UDP : PKTF_TYPE_TCP));

    CIpHdr *  pIpHdrNat  = ppktNat->GetIpHdr();
    CUdpHdr * pUdpHdrNat = (CUdpHdr *)(pIpHdrNat + 1);

    Assert(ppkt->GetCb() == ppktNat->GetCb());
    memcpy(pIpHdrNat, pIpHdr, ppkt->GetCb());

    pIpHdrNat->_ipaSrc = _pXnIpNatOuter->GetIpa();
    pUdpHdrNat->_ipportSrc = pNatPort->_ipportNat;

    TraceSz10(NatFlow, ">> %s [%s:%d %s:%d] -> [%s:%d %s:%d] (+%d)",
              pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
              pIpHdr->_ipaSrc.Str(), NTOHS(pUdpHdr->_ipportSrc),
              pIpHdr->_ipaDst.Str(), NTOHS(pUdpHdr->_ipportDst),
              pIpHdrNat->_ipaSrc.Str(), NTOHS(pUdpHdrNat->_ipportSrc),
              pIpHdrNat->_ipaDst.Str(), NTOHS(pUdpHdrNat->_ipportDst),
              cbLen);

    _pXnIpNatOuter->IpXmit(ppktNat);
}

void CXnNat::RecvOuter(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (ppkt->TestFlags(PKTF_RECV_BROADCAST|PKTF_RECV_LOOPBACK) || ppkt->GetHdrOptLen() > 0)
        return;

    CIpAddr     ipaOuter    = pIpHdr->_ipaSrc;
    CIpPort     ipportOuter = pUdpHdr->_ipportSrc;
    CIpAddr     ipaNat      = pIpHdr->_ipaDst;
    CIpPort     ipportNat   = pUdpHdr->_ipportDst;
    DWORD       dwTick      = _pXnIpNatInner->TimerTick();

    CNatPort * pNatPort = NatPortLookup(ipportNat);

    if (pNatPort == NULL)
    {
        TraceSz5(NatPort, "!! No port mapping found for inbound %s [%s:%d %s:%d]",
                 pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                 ipaOuter.Str(), NTOHS(ipportOuter), ipaNat.Str(), NTOHS(ipportNat));
        return;
    }

    pNatPort->_dwTick = dwTick;
    RemoveEntryList(&pNatPort->_leLru);
    InsertTailList(&_leLru, &pNatPort->_leLru);

    if (_iFilter)
    {
        CNatFilt * pNatFilt = NatFiltLookup(pNatPort, ipaOuter, ipportOuter, FALSE);

        if (pNatFilt == NULL)
        {
            TraceSz10(NatPort, "!! %s filter discarding inbound %s [%s:%d %s:%d] -> [%s:%d %s:%d]",
                      _iFilter == NAT_FILTER_ADDRESS ? "Addr" : _iFilter == NAT_FILTER_PORT ? "Port" : "AddrPort",
                      pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                      ipaOuter.Str(), NTOHS(ipportOuter),
                      ipaNat.Str(), NTOHS(ipportNat),
                      ipaOuter.Str(), NTOHS(ipportOuter),
                      pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner));
            return;
        }

        pNatFilt->_dwTick = dwTick;
    }

    // Copy the packet, change source address and port, and forward to _pXnIpNatOuter

    CPacket * ppktNat = _pXnIpNatInner->PacketAlloc(PTAG_CNatPacket,
        PKTF_TYPE_IP|PKTF_POOLALLOC|PKTF_XMIT_INSECURE, ppkt->GetCb() - sizeof(CIpHdr));

    if (ppktNat == NULL)
    {
        TraceSz5(pktWarn, "[DISCARD] Out of memory allocating CNatPacket for inbound %s [%s:%d %s:%d]",
                 pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
                 ipaOuter.Str(), NTOHS(ipportOuter), ipaNat.Str(), NTOHS(ipportNat));
        return;
    }

    ppktNat->SetType((WORD)(pIpHdr->_bProtocol == IPPROTOCOL_UDP ? PKTF_TYPE_UDP : PKTF_TYPE_TCP));

    CIpHdr *  pIpHdrNat  = ppktNat->GetIpHdr();
    CUdpHdr * pUdpHdrNat = (CUdpHdr *)(pIpHdrNat + 1);

    Assert(ppkt->GetCb() == ppktNat->GetCb());
    memcpy(pIpHdrNat, pIpHdr, ppkt->GetCb());

    pIpHdrNat->_ipaDst = pNatPort->_ipaInner;
    pUdpHdrNat->_ipportDst = pNatPort->_ipportInner;

    TraceSz10(NatFlow, "<< %s [%s:%d %s:%d] -> [%s:%d %s:%d] (+%d)",
              pIpHdr->_bProtocol == IPPROTOCOL_UDP ? "UDP" : "TCP",
              pIpHdr->_ipaSrc.Str(), NTOHS(pUdpHdr->_ipportSrc),
              pIpHdr->_ipaDst.Str(), NTOHS(pUdpHdr->_ipportDst),
              pIpHdrNat->_ipaSrc.Str(), NTOHS(pUdpHdrNat->_ipportSrc),
              pIpHdrNat->_ipaDst.Str(), NTOHS(pUdpHdrNat->_ipportDst),
              cbLen);

    _pXnIpNatInner->IpXmitIp(ppktNat);
}

CXnNat::CNatPort * CXnNat::NatPortLookup(CIpAddr ipaInner, CIpPort ipportInner, CIpAddr ipaOuter, CIpPort ipportOuter)
{
    CNatPort * pNatPort;
    
    while (!IsListEmpty(&_leLru))
    {
        pNatPort = (CNatPort *)((BYTE *)_leLru.Flink - offsetof(CNatPort, _leLru));

        if (!NatPortExpire(pNatPort))
            break;
    }

    DWORD dwHash = ipaInner ^ (ipportInner << 3);

    if (_iAssign == NAT_ASSIGN_AGGRESSIVE)
    {
        dwHash ^= (ipaOuter << 13);
        dwHash ^= (ipaOuter >> 19);
        dwHash ^= (ipportOuter << 7);
    }
    else
    {
        ipaOuter = 0;
        ipportOuter = 0;
    }

    LIST_ENTRY * pleHead = &_pleHash[dwHash % _cBucket];

    for (LIST_ENTRY * ple = pleHead->Flink; ple != pleHead; ple = ple->Flink)
    {
        pNatPort = (CNatPort *)ple;

        if (    pNatPort->_ipaInner == ipaInner
            &&  pNatPort->_ipportInner == ipportInner
            &&  pNatPort->_ipaOuter == ipaOuter
            &&  pNatPort->_ipportOuter == ipportOuter)
        {
            return(pNatPort);
        }
    }

    if (_cNatPortFree == 0)
    {
        return(NULL);
    }

    pNatPort = (CNatPort *)HalAllocZ(sizeof(CNatPort), PTAG_CNatPort);

    if (pNatPort != NULL)
    {
        while (1)
        {
            if (_iNatPortNext > _iNatPortLast)
                _iNatPortNext = _iNatPortBase;

            if (_ppNatPort[_iNatPortNext - _iNatPortBase] == NULL)
                break;

            _iNatPortNext += 1;
        }

        pNatPort->_ipaInner     = ipaInner;
        pNatPort->_ipportInner  = ipportInner;
        pNatPort->_ipportNat    = HTONS((WORD)_iNatPortNext);
        pNatPort->_ipaOuter     = ipaOuter;
        pNatPort->_ipportOuter  = ipportOuter;
        pNatPort->_dwTick       = _pXnIpNatInner->TimerTick();

        InsertTailList(pleHead, &pNatPort->_leHash);
        InsertTailList(&_leLru, &pNatPort->_leLru);

        _ppNatPort[_iNatPortNext - _iNatPortBase] = pNatPort;

        _cNatPortFree -= 1;

        TraceSz5(NatPort, "** Port mapping %d [%s:%d %s:%d] allocated",
                 NTOHS(pNatPort->_ipportNat), pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner),
                 pNatPort->_ipaOuter.Str(), NTOHS(pNatPort->_ipportOuter));
    }

    return(pNatPort);
}

CXnNat::CNatPort * CXnNat::NatPortLookup(CIpPort ipportNat)
{
    UINT iNatPort = NTOHS(ipportNat);

    if (iNatPort < _iNatPortBase || iNatPort > _iNatPortLast)
        return(NULL);
    else
        return(_ppNatPort[iNatPort - _iNatPortBase]);
}

BOOL CXnNat::NatPortExpire(CNatPort * pNatPort)
{
    if (pNatPort->_dwTick + _iTimeout > _pXnIpNatInner->TimerTick())
    {
        return(FALSE);
    }
    
    TraceSz5(NatPort, "** Port mapping %d [%s:%d %s:%d] has timed out",
             NTOHS(pNatPort->_ipportNat), pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner),
             pNatPort->_ipaOuter.Str(), NTOHS(pNatPort->_ipportOuter));

    NatPortFree(pNatPort);
    return(TRUE);
}

void CXnNat::NatPortFree(CNatPort * pNatPort)
{
    UINT iNatPort = NTOHS(pNatPort->_ipportNat);

    TraceSz5(NatPort, "** Port mapping %d [%s:%d %s:%d] freed",
             iNatPort, pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner),
             pNatPort->_ipaOuter.Str(), NTOHS(pNatPort->_ipportOuter));

    Assert(iNatPort >= _iNatPortBase && iNatPort <= _iNatPortLast);
    Assert(_ppNatPort[iNatPort - _iNatPortBase] == pNatPort);
    _ppNatPort[iNatPort - _iNatPortBase] = NULL;

    RemoveEntryList(&pNatPort->_leHash);
    RemoveEntryList(&pNatPort->_leLru);

    if (pNatPort->_pNatFilt)
    {
        HalFree(pNatPort->_pNatFilt);
    }

    HalFree(pNatPort);

    _cNatPortFree += 1;
}

CXnNat::CNatFilt * CXnNat::NatFiltLookup(CNatPort * pNatPort, CIpAddr ipaOuter, CIpPort ipportOuter, BOOL fAlloc)
{
    CNatFilt *  pNatFilt;
    UINT        idx, idxIns;
    DWORD       dwTimeout = _pXnIpNatInner->TimerTick() - _iTimeout;

    if ((_iFilter & NAT_FILTER_ADDRESS) == 0)
    {
        ipaOuter = 0;
    }

    if ((_iFilter & NAT_FILTER_PORT) == 0)
    {
        ipportOuter = 0;
    }

    for (int iLo = 0, iHi = (int)pNatPort->_cNatFilt, iMid; iLo < iHi; )
    {
        iMid     = (iLo + iHi) >> 1;
        pNatFilt = &pNatPort->_pNatFilt[iMid];

        if (pNatFilt->_ipaOuter == ipaOuter && pNatFilt->_ipportOuter == ipportOuter)
        {
            if (!fAlloc && pNatFilt->_dwTick <= dwTimeout)
            {
                return(NULL);
            }

            return(pNatFilt);
        }

        if (pNatFilt->_ipaOuter < ipaOuter || (pNatFilt->_ipaOuter == ipaOuter && pNatFilt->_ipportOuter < ipportOuter))
            iLo = iMid + 1;
        else
            iHi = iMid;
    }

    idxIns = iLo;

    if (!fAlloc)
    {
        return(NULL);
    }

    if (pNatPort->_cNatFilt > 0 && pNatPort->_cNatFilt == pNatPort->_cNatFiltAlloc)
    {
        CNatFilt * pNatFiltDst = pNatPort->_pNatFilt;
        CNatFilt * pNatFiltEnd = pNatFiltDst + pNatPort->_cNatFilt;

        pNatFilt = pNatFiltDst;

        for (idx = 0; pNatFilt < pNatFiltEnd; ++pNatFilt)
        {
            if (pNatFilt->_dwTick > dwTimeout)
            {
                if (pNatFiltDst < pNatFilt)
                {
                    memcpy(pNatFiltDst, pNatFilt, sizeof(CNatFilt));

                    TraceSz7(NatPort, "** Port filter  %d [%s:%d %s:%d] <--> [%s:%d] has timed out",
                             NTOHS(pNatPort->_ipportNat), pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner),
                             pNatPort->_ipaOuter.Str(), NTOHS(pNatPort->_ipportOuter),
                             pNatFilt->_ipaOuter.Str(), NTOHS(pNatFilt->_ipportOuter));
                }

                pNatFiltDst += 1;
                idx += 1;
            }
            else
            {
                pNatPort->_cNatFilt -= 1;
                if (idxIns > idx)
                    idxIns -= 1;
            }
        }
    }

    if (pNatPort->_cNatFilt == pNatPort->_cNatFiltAlloc)
    {
        UINT cNatFiltAlloc = pNatPort->_cNatFiltAlloc + 8;

        pNatFilt = (CNatFilt *)HalAlloc(cNatFiltAlloc * sizeof(CNatFilt), PTAG_CNatFilt);

        if (pNatFilt == NULL)
        {
            return(NULL);
        }

        if (pNatPort->_cNatFilt > 0)
        {
            memcpy(pNatFilt, pNatPort->_pNatFilt, pNatPort->_cNatFilt * sizeof(CNatFilt));
            HalFree(pNatPort->_pNatFilt);
        }

        pNatPort->_pNatFilt = pNatFilt;
        pNatPort->_cNatFiltAlloc = cNatFiltAlloc;
    }

    Assert(idxIns <= pNatPort->_cNatFilt);
    Assert(idxIns <  pNatPort->_cNatFiltAlloc);

    pNatFilt = &pNatPort->_pNatFilt[idxIns];

    if (idxIns < pNatPort->_cNatFilt)
    {
        memmove(pNatFilt + 1, pNatFilt, (pNatPort->_cNatFilt - idxIns) * sizeof(CNatFilt));
    }

    pNatFilt->_ipaOuter     = ipaOuter;
    pNatFilt->_ipportOuter  = ipportOuter;
    pNatFilt->_dwTick       = _pXnIpNatInner->TimerTick();

    pNatPort->_cNatFilt    += 1;

    TraceSz7(NatPort, "** Port filter  %d [%s:%d %s:%d] <--> [%s:%d] allocated",
             NTOHS(pNatPort->_ipportNat), pNatPort->_ipaInner.Str(), NTOHS(pNatPort->_ipportInner),
             pNatPort->_ipaOuter.Str(), NTOHS(pNatPort->_ipportOuter),
             pNatFilt->_ipaOuter.Str(), NTOHS(pNatFilt->_ipportOuter));

    return(pNatFilt);
}

BOOL CXnNat::IsGateway(CIpPort ipport, CXnIpNat * pXnIpNat)
{
    return(pXnIpNat == _pXnIpNatInner || ipport == DNS_CLIENT_PORT);
}

// ---------------------------------------------------------------------------------------
// XNetNatCreate / XNetNatDelete
// ---------------------------------------------------------------------------------------

void * WSAAPI XNetNatCreate(NATCREATE * pNatCreate)
{
    CXnNat * pXnNat = new CXnNat;

    if (pXnNat == NULL)
    {
        TraceSz(Warning, "XNetNatCreate - Out of memory allocating CXnNat");
    }
    else if (!pXnNat->Init(pNatCreate))
    {
        XNetNatDelete(pXnNat);
        pXnNat = NULL;
    }

    return(pXnNat);
}

void WSAAPI XNetNatDelete(void * pvNat)
{
    CXnNat * pXnNat = (CXnNat *)pvNat;
    pXnNat->Term();
    delete pXnNat;
}

void WSAAPI XNetNatXmit(void * pvNat, void * pvPkt, BOOL fDiscard)
{
//@@@    ((CXnNat *)pvNat)->NatXmit(pvPkt, fDiscard);
}


#endif
