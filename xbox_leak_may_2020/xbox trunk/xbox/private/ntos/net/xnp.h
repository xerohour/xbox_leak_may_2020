// ---------------------------------------------------------------------------------------
// xnp.h
//
// Private header file for the XNet Secure Networking Library
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef _XNP_H
#define _XNP_H

// ---------------------------------------------------------------------------------------
// To Do List
//
// @@@ Probing support
// @@@ DHCP rewrite (sync with XP client, gratuitous ARP, host name support, yank auto-ip)
// @@@ PPPoE support
// @@@ Network troubleshooter support
// ---------------------------------------------------------------------------------------

#pragma code_seg("XNET")
#pragma const_seg("XNET_RD")

// ---------------------------------------------------------------------------------------
// System Includes
// ---------------------------------------------------------------------------------------

#ifndef DBG
#define DBG 0
#endif

#ifdef _XBOX
    extern "C" {
        #include <ntos.h>
        #include <init.h>
        #include <hal.h>
        #include <nturtl.h>
        #include <xtl.h>
        #include <xboxp.h>
        #include <xdbg.h>
        #include <phy.h>
        #include <xbeimage.h>
        #include <ldr.h>
    };
#else
    #include <nt.h>
    #include <ntrtl.h>
    #include <nturtl.h>
    #include <windows.h>
    #include <winsockx.h>
    #include <xboxp.h>
#endif

#include <winsockp.h>

// ---------------------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------------------

#if DBG
    #define XNET_FEATURE_ASSERT
    #define XNET_FEATURE_TRACE
    #define XNET_FEATURE_STATS
#endif

#if defined(XNET_BUILD_LIBXS)
    #define XNET_FEATURE_XBOX
    #if DBG
        #pragma message("Building xnetsd.lib")
        #define XNET_FEATURE_XBDM_CLIENT
    #else
        #pragma message("Building xnets.lib")
    #endif
#elif defined(XNET_BUILD_LIBX)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_DNS
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_INSECURE
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_XBDM_CLIENT
    #define XNET_FEATURE_XBOX
    #if DBG
        #pragma message("Building xnetd.lib")
    #else
        #pragma message("Building xnet.lib")
    #endif
#elif defined(XNET_BUILD_LIBOS)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_DNS
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_ONLINE
    #define XNET_FEATURE_QOS
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_SG
    #define XNET_FEATURE_XBOX
    #if DBG
        #pragma message("Building xnetosd.lib")
        #define XNET_FEATURE_XBDM_CLIENT
    #else
        #pragma message("Building xnetos.lib")
    #endif
#elif defined(XNET_BUILD_LIBO)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_DNS
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_INSECURE
    #define XNET_FEATURE_ONLINE
    #define XNET_FEATURE_QOS
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_SG
    #define XNET_FEATURE_XBDM_CLIENT
    #define XNET_FEATURE_XBOX
    #if DBG
        #pragma message("Building xnetod.lib")
    #else
        #pragma message("Building xneto.lib")
    #endif
#elif defined(XNET_BUILD_LIBM)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_INSECURE
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_XBDM_SERVER
    #define XNET_FEATURE_XBOX
    #if DBG
        #pragma message("Building xnetmd.lib")
    #else
        #pragma message("Building xnetm.lib")
    #endif
#elif defined(XNET_BUILD_LIBXW)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_DNS
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_FRAG_LOOPBACK
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_INSECURE
    #define XNET_FEATURE_QOS
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_SG
    #define XNET_FEATURE_WINDOWS
    #define XNET_FEATURE_VIRTUAL
    #if DBG
        #pragma message("Building xnetwd.lib")
        #define XNET_FEATURE_VMEM
    #else
        #pragma message("Building xnetw.lib")
    #endif
#elif defined(XNET_BUILD_LIBOW)
    #define XNET_FEATURE_ARP
    #define XNET_FEATURE_DHCP
    #define XNET_FEATURE_DNS
    #define XNET_FEATURE_FRAG
    #define XNET_FEATURE_FRAG_LOOPBACK
    #define XNET_FEATURE_ICMP
    #define XNET_FEATURE_INSECURE
    #define XNET_FEATURE_ONLINE
    #define XNET_FEATURE_QOS
    #define XNET_FEATURE_ROUTE
    #define XNET_FEATURE_SG
    #define XNET_FEATURE_WINDOWS
    #define XNET_FEATURE_VIRTUAL
    #if DBG
        #pragma message("Building xnetwd.lib")
        #define XNET_FEATURE_VMEM
    #else
        #pragma message("Building xnetw.lib")
    #endif
#else
    #error "Don't know which xnet library to build"
#endif

#ifdef XNET_FEATURE_ARP
#pragma message("  XNET_FEATURE_ARP")
#endif

#ifdef XNET_FEATURE_ASSERT
#pragma message("  XNET_FEATURE_ASSERT")
#endif

#ifdef XNET_FEATURE_DHCP
#pragma message("  XNET_FEATURE_DHCP")
#endif

#ifdef XNET_FEATURE_DNS
#pragma message("  XNET_FEATURE_DNS")
#endif

#ifdef XNET_FEATURE_FRAG
#pragma message("  XNET_FEATURE_FRAG")
#endif

#ifdef XNET_FEATURE_FRAG_LOOPBACK
#pragma message("  XNET_FEATURE_FRAG_LOOPBACK")
#endif

#ifdef XNET_FEATURE_ICMP
#pragma message("  XNET_FEATURE_ICMP")
#endif

#ifdef XNET_FEATURE_INSECURE
#pragma message("  XNET_FEATURE_INSECURE")
#endif

#ifdef XNET_FEATURE_ONLINE
#pragma message("  XNET_FEATURE_ONLINE")
#endif

#ifdef XNET_FEATURE_QOS
#pragma message("  XNET_FEATURE_QOS")
#endif

#ifdef XNET_FEATURE_ROUTE
#pragma message("  XNET_FEATURE_ROUTE")
#endif

#ifdef XNET_FEATURE_SG
#pragma message("  XNET_FEATURE_SG")
#endif

#ifdef XNET_FEATURE_TRACE
#pragma message("  XNET_FEATURE_TRACE")
#endif

#ifdef XNET_FEATURE_VIRTUAL
#pragma message("  XNET_FEATURE_VIRTUAL")
#endif

#ifdef XNET_FEATURE_VMEM
#pragma message("  XNET_FEATURE_VMEM")
#endif

#ifdef XNET_FEATURE_WINDOWS
#pragma message("  XNET_FEATURE_WINDOWS")
#endif

#ifdef XNET_FEATURE_XBDM_CLIENT
#pragma message("  XNET_FEATURE_XBDM_CLIENT")
#endif

#ifdef XNET_FEATURE_XBDM_SERVER
#pragma message("  XNET_FEATURE_XBDM_SERVER")
#endif

#ifdef XNET_FEATURE_XBOX
#pragma message("  XNET_FEATURE_XBOX")
#endif

// ---------------------------------------------------------------------------------------
// Debugging
// ---------------------------------------------------------------------------------------

#include <xn.h>

ExternTag(LeakWarn);
ExternTag(pktXmit);
ExternTag(pktRecv);
ExternTag(pktBroadcast);
ExternTag(pktWarn);
ExternTag(secStat);
ExternTag(sock);
ExternTag(sockWarn);

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_VIRTUAL
#define VIRTUAL                 virtual
#define NOVTABLE
#define VPURE                   = 0
#else
#define NOVTABLE                __declspec(novtable)
#define VIRTUAL
#define VPURE
#endif

#define XNETPTAGLIST() \
    XNETPTAG(CXnHal,            'NET0') \
    XNETPTAG(CXnBase,           'NET1') \
    XNETPTAG(CXnNic,            'NET2') \
    XNETPTAG(CXnEnet,           'NET3') \
    XNETPTAG(CXnIp,             'NET4') \
    XNETPTAG(CXnSock,           'NET5') \
    XNETPTAG(CDhcpOptions,      'NET6') \
    XNETPTAG(CRouteEntry,       'NET7') \
    XNETPTAG(CSocket,           'NET8') \
    XNETPTAG(CUdpRecvBuf,       'NET9') \
    XNETPTAG(CTcpSocket,        'NETa') \
    XNETPTAG(CTcpRecvBuf,       'NETb') \
    XNETPTAG(CRecvReq,          'NETc') \
    XNETPTAG(CXmitReq,          'NETd') \
    XNETPTAG(Pool,              'NETe') \
    XNETPTAG(PoolStart,         'NETf') \
    XNETPTAG(PoolEnd,           'NETg') \
    XNETPTAG(select,            'NETh') \
    XNETPTAG(CArpPacket,        'NETi') \
    XNETPTAG(CDhcpPacket,       'NETj') \
    XNETPTAG(CIcmpEchoPacket,   'NETk') \
    XNETPTAG(CFragPacket,       'NETl') \
    XNETPTAG(CTcpPacket,        'NETm') \
    XNETPTAG(CUdpPacket,        'NETn') \
    XNETPTAG(CKeyReg,           'NETo') \
    XNETPTAG(CSecReg,           'NETp') \
    XNETPTAG(CKeyExPacket,      'NETq') \
    XNETPTAG(CPacket,           'NETr') \
    XNETPTAG(CXbdmPacket,       'NETs') \
    XNETPTAG(CDnsReg,           'NETt') \
    XNETPTAG(CDnsPacket,        'NETu') \
    XNETPTAG(CSecMsgPacket,     'NETv') \
    XNETPTAG(CXnNat,            'NETw') \
    XNETPTAG(CXnIpNat,          'NETx') \
    XNETPTAG(CDhcpLease,        'NETy') \
    XNETPTAG(CNatPacket,        'NETz') \
    XNETPTAG(CNatPort,          'NETA') \
    XNETPTAG(CNatFilt,          'NETB') \
    XNETPTAG(QosData,           'NETC') \

#undef  XNETPTAG
#define XNETPTAG(_name, _tag) PTAG_##_name = NTOHL_(_tag),

enum { XNETPTAGLIST() };

#define INITF_HAL               0x00000001
#define INITF_HAL_1             0x00000002
#define INITF_HAL_2             0x00000004
#define INITF_HAL_STOP          0x00000008
#define INITF_HAL_TERM          0x00000010
#define INITF_BASE              0x00000020
#define INITF_BASE_1            0x00000040
#define INITF_BASE_2            0x00000080
#define INITF_BASE_STOP         0x00000100
#define INITF_BASE_TERM         0x00000200
#define INITF_NIC               0x00000400
#define INITF_NIC_1             0x00000800
#define INITF_NIC_2             0x00001000
#define INITF_NIC_STOP          0x00002000
#define INITF_NIC_TERM          0x00004000
#define INITF_ENET              0x00008000
#define INITF_ENET_1            0x00010000
#define INITF_ENET_2            0x00020000
#define INITF_ENET_STOP         0x00040000
#define INITF_ENET_TERM         0x00080000
#define INITF_IP                0x00100000
#define INITF_IP_1              0x00200000
#define INITF_IP_2              0x00400000
#define INITF_IP_STOP           0x00800000
#define INITF_IP_TERM           0x01000000
#define INITF_SOCK              0x02000000
#define INITF_SOCK_1            0x04000000
#define INITF_SOCK_2            0x08000000
#define INITF_SOCK_STOP         0x10000000
#define INITF_SOCK_TERM         0x20000000
#define INITF_CONNECTED_BOOT    0x80000000

#define NETERR(_err)            HRESULT_FROM_WIN32(_err)
#define NETERR_OK               STATUS_SUCCESS
#define NETERR_PARAM            NETERR(WSAEINVAL)
#define NETERR_MEMORY           NETERR(WSAENOBUFS)
#define NETERR_SYSCALL          NETERR(WSASYSCALLFAILURE)
#define NETERR_FAULT            NETERR(WSAEFAULT)
#define NETERR_CANCELLED        NETERR(WSAECANCELLED)
#define NETERR_PENDING          NETERR(WSA_IO_PENDING)
#define NETERR_WOULDBLOCK       NETERR(WSAEWOULDBLOCK)
#define NETERR_MSGSIZE          NETERR(WSAEMSGSIZE)
#define NETERR_TIMEOUT          NETERR(WSAETIMEDOUT)
#define NETERR_NOTIMPL          NETERR(ERROR_CALL_NOT_IMPLEMENTED)
#define NETERR_UNREACHABLE      NETERR(WSAEHOSTUNREACH)
#define NETERR_NETDOWN          NETERR(WSAENETDOWN)
#define NETERR_ADDRINUSE        NETERR(WSAEADDRINUSE)
#define NETERR_ADDRCONFLICT     NETERR(ERROR_DHCP_ADDRESS_CONFLICT)
#define NETERR_CONNRESET        NETERR(WSAECONNRESET)

#define IPPROTOCOL_SECMSG       0
#define IPPROTOCOL_ICMP         1
#define IPPROTOCOL_TCP          6
#define IPPROTOCOL_UDP          17

#define DNS_SERVER_PORT         HTONS_(53)
#define DNS_CLIENT_PORT         HTONS_(53)
#define DHCP_CLIENT_PORT        HTONS_(68)
#define DHCP_SERVER_PORT        HTONS_(67)
#define ESPUDP_CLIENT_PORT      HTONS_(3074)

#define dimensionof(x)          (sizeof((x))/sizeof(*(x)))
#define BIT(n)                  (1u << (n))
#define ROUNDUP4(count)         (((count) + 3) & ~3)
#define ROUNDUP8(count)         (((count) + 7) & ~7)
#define IsListNull(_ple)        ((_ple)->Flink == NULL)

// ---------------------------------------------------------------------------------------
// XNetParams
// ---------------------------------------------------------------------------------------

#define XNETPARAMDEFS() \
    DEFINE_PARAM_( 0, cfgSizeOfStruct,                         0, 0,   0) \
    DEFINE_PARAM_( 1, cfgFlags,                                0, 0, 255) \
    DEFINE_PARAM_( 2, cfgPrivatePoolSizeInPages,              12, 4, 255) \
    DEFINE_PARAM_( 3, cfgEnetReceiveQueueLength,               8, 4, 255) \
    DEFINE_PARAM_( 4, cfgIpFragMaxSimultaneous,                4, 1, 255) \
    DEFINE_PARAM_( 5, cfgIpFragMaxPacketDiv256,                8, 4, 255) \
    DEFINE_PARAM_( 6, cfgSockMaxSockets,                      64, 1, 255) \
    DEFINE_PARAM_( 7, cfgSockDefaultRecvBufsizeInK,           16, 1,  63) \
    DEFINE_PARAM_( 8, cfgSockDefaultSendBufsizeInK,           16, 1,  63) \
    DEFINE_PARAM_( 9, cfgKeyRegMax,                            4, 1, 255) \
    DEFINE_PARAM_(10, cfgSecRegMax,                           32, 1, 255) \
    DEFINE_PARAM_(11, cfgQosDataLimitDiv4,                   128, 0, 255) \
    DEFINE_PARAM (    cfgEnetTransmitQueueLength,             10, 4, 255) \
    DEFINE_PARAM (    cfgEnetArpReqRetries,                    2, 1, 255) \
    DEFINE_PARAM (    cfgEnetArpRexmitTimeoutInSeconds,        1, 1, 255) \
    DEFINE_PARAM (    cfgEnetArpPosCacheTimeoutInMinutes,     10, 1, 255) \
    DEFINE_PARAM (    cfgEnetArpNegCacheTimeoutInMinutes,      1, 1, 255) \
    DEFINE_PARAM (    cfgIpFragTimeoutInSeconds,              60, 1, 255) \
    DEFINE_PARAM (    cfgIpDefaultTos,                         0, 0, 255) \
    DEFINE_PARAM (    cfgIpDefaultTtl,                        64, 1, 255) \
    DEFINE_PARAM (    cfgAutoIpMaxAttempts,                   10, 1, 255) \
    DEFINE_PARAM (    cfgAutoIpRexmitTimeoutInSeconds,         2, 1, 255) \
    DEFINE_PARAM (    cfgAutoIpRetriesPerAttempt,              2, 1, 255) \
    DEFINE_PARAM (    cfgDhcpRetryCount,                       3, 1, 255) \
    DEFINE_PARAM (    cfgDhcpRetryMaxTimeoutInSeconds,        10, 1, 255) \
    DEFINE_PARAM (    cfgDhcpDefaultLeaseTimeInDays,           0, 0, 255) \
    DEFINE_PARAM (    cfgDhcpRenewMinTimeoutInSeconds,        60, 1, 255) \
    DEFINE_PARAM (    cfgDhcpDeclineWaitInSeconds,            10, 1, 255) \
    DEFINE_PARAM (    cfgSockMaxListenBacklog,                 5, 1, 255) \
    DEFINE_PARAM (    cfgSockMaxSendBufsizeInK,               63, 1,  63) \
    DEFINE_PARAM (    cfgSockMaxRecvBufsizeInK,               63, 1,  63) \
    DEFINE_PARAM (    cfgSockSegmentLifetimeInSeconds,        30, 1, 255) \
    DEFINE_PARAM (    cfgSockConnectTimeoutInSeconds,         20, 1, 255) \
    DEFINE_PARAM (    cfgSockLingerTimeoutInSeconds,         120, 1, 255) \
    DEFINE_PARAM (    cfgSockRexmitRetries,                    5, 1, 255) \
    DEFINE_PARAM (    cfgSockRexmitTimeoutInSeconds,           3, 1, 255) \
    DEFINE_PARAM (    cfgSockRexmitMinTimeoutInSeconds,        1, 1, 255) \
    DEFINE_PARAM (    cfgSockRexmitMaxTimeoutInSeconds,        8, 1, 255) \
    DEFINE_PARAM (    cfgSockPersistTimeoutInSeconds,          5, 1, 255) \
    DEFINE_PARAM (    cfgKeyExXbToXbRexmitRetries,             9, 1, 255) \
    DEFINE_PARAM (    cfgKeyExXbToXbRexmitTimeoutInSeconds,    1, 1, 255) \
    DEFINE_PARAM (    cfgKeyExXbToSgRexmitRetries,             4, 1, 255) \
    DEFINE_PARAM (    cfgKeyExXbToSgRexmitTimeoutInSeconds,    2, 1, 255) \
    DEFINE_PARAM (    cfgDnsRetries,                           4, 1, 255) \
    DEFINE_PARAM (    cfgDnsTimeoutInSeconds,                  2, 1, 255) \
    DEFINE_PARAM (    cfgSecRegVisitInSeconds,                 5, 1, 255) \
    DEFINE_PARAM (    cfgSecRegPulseTimeoutInSeconds,         20, 1, 255) \
    DEFINE_PARAM (    cfgSecRegTimeoutInSecondsDiv10,         13, 1, 255) \

#define DEFINE_PARAM_(idx, name, def, min, max) BYTE name;
#define DEFINE_PARAM(name, def, min, max) BYTE name;

#include <pshpack1.h>
struct XNetParams { XNETPARAMDEFS() };
#include <poppack.h>

struct XNetInitParams
{
    XNetParams *    pxnp;
    char *          pszXbox;
    LARGE_INTEGER   liTime;
    BYTE            abSeed[20];
};

// ---------------------------------------------------------------------------------------
// CXnVoid
// ---------------------------------------------------------------------------------------

class NOVTABLE CXnVoid
{
    VIRTUAL void VoidInit() {};
};

// ---------------------------------------------------------------------------------------
// CXnHal
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_WINDOWS
#include "halw.h"
#endif

#ifdef XNET_FEATURE_XBOX
#include "halx.h"
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

// ---------------------------------------------------------------------------------------
// Crypto
// ---------------------------------------------------------------------------------------

#include <xcrypt.h>

#ifdef XNET_FEATURE_ASSERT
#define XCRYPT_VERIFY(x)    do { BOOL __fOK__ = (x); if (!__fOK__) AssertSz(FALSE, "XCRYPT_SERVICE failed"); } while (0)
#else
#define XCRYPT_VERIFY(x)    x
#endif

// ---------------------------------------------------------------------------------------
// CTimer
// ---------------------------------------------------------------------------------------

typedef void (CXnVoid::*PFNPKTFREE)(class CPacket * ppkt);
typedef void (CXnVoid::*PFNTIMER)(class CTimer * pt);

class CTimer
{
    friend class CXnBase;

    // Definitions -------------------------------------------------------------------

    #define TICKS_PER_SECOND    5       // 200 ms per tick (25 years before DWORD wraps)
    #define MSEC_PER_TICK       (1000 / TICKS_PER_SECOND)
    #define TIMER_INFINITE      0xFFFFFFFF

    // External ----------------------------------------------------------------------

public:

    INLINE void Init(PFNTIMER pfn)  { _le.Flink = NULL; _pfn = pfn; _dwTick = TIMER_INFINITE; }
    INLINE BOOL IsActive()          { return(_le.Flink != NULL); }
    INLINE BOOL IsNull()            { return(_pfn == NULL); }

    // Data --------------------------------------------------------------------------

private:

    LIST_ENTRY  _le;
    DWORD       _dwTick;
    PFNTIMER    _pfn;

};

struct CEventTimer : public CTimer
{
    PRKEVENT    _pEvent;
};

C_ASSERT(sizeof(CEventTimer) == CBEVENTTIMER);

// ---------------------------------------------------------------------------------------
// CXnBase
// ---------------------------------------------------------------------------------------

struct CLeak
{
    void *  _pv;
    UINT    _cb;
    ULONG   _tag;
};

class NOVTABLE CXnBase : public CXnHal
{
    // Definitions -----------------------------------------------------------------------

public:

    #define MAX_SMALL_BLOCKS 5

private:

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnBase)

    NTSTATUS        BaseInit(XNetInitParams * pxnip);
    void            BaseStart();
    void            BaseStop();
    void            BaseTerm();

    void *          LeakAdd(CLeakInfo * pli, void * pv, UINT cb, ULONG tag);
    void *          LeakDel(CLeakInfo * pli, void * pv);
    BOOL            LeakFind(CLeakInfo * pli, void * pv, CLeak ** ppLeak);
    void            LeakTerm(CLeakInfo * pli);

    void *          SysAlloc(size_t size, ULONG tag);
    void *          SysAllocZ(size_t size, ULONG tag);
    void            SysFree(void * pv);

    void *          PoolAlloc(size_t size, ULONG tag);
    void *          PoolAllocZ(size_t size, ULONG tag);
    void            PoolFree(void * pv);

    CPacket *       PacketAlloc(ULONG tag, UINT uiFlags, UINT cbPayload = 0, UINT cbPkt = 0, PFNPKTFREE pfn = NULL);
    void            PacketFree(CPacket * ppkt);

    void            Rand(BYTE * pb, UINT cb);
    ULONG           RandLong();
    INLINE ULONG    RandScaled(ULONG ulMax);

    void            TimerSet(CTimer * pt, DWORD dwTick);
    DWORD           TimerSetRelative(CTimer * pt, DWORD dwTicksFromNow);
    INLINE DWORD    TimerTick()         { TCHECK(UDPC|SDPC); return(_dwTick); }

    void            PushPktRecvTags(BOOL fBroadcast);
    void            PopPktRecvTags();

    VIRTUAL void    SecRegProbe() VPURE;
    VIRTUAL void    NicTimer() VPURE;

    // Internal --------------------------------------------------------------------------

private:

    void            RandInit(XNetInitParams * pxnip);

    void            PoolDump();

    static void     TimerDpc(PKDPC pkdpc, void * pvContext, void * pvParam1, void * pvParam2);
    void            TimerPush();

    // Data ------------------------------------------------------------------------------

public:

    XNETPARAMDEFS()

    void *          _pvPoolStart;
    void *          _pvPoolEnd;
    LIST_ENTRY      _aleFree[MAX_SMALL_BLOCKS + 1];
    BYTE            _abRandRc4Struct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
    KDPC            _dpcTimer;
    KTIMER          _timerTimer;
    LIST_ENTRY      _leTimers;
    DWORD           _dwTickKe;
    DWORD           _dwTick;

#ifdef XNET_FEATURE_ASSERT
    CLeakInfo       _leakinfo;
#endif

#ifdef XNET_FEATURE_TRACE
    BYTE            _abPushPop[32];
    UINT            _cbPushPop;
    UINT            _cPushPopBroadcast;
    UINT            _pktRecvPushPop;
    UINT            _pktWarnPushPop;
#endif

private:

    static const XNetParams s_XNetParamsDef;
    static const XNetParams s_XNetParamsMin;
    static const XNetParams s_XNetParamsMax;

};

INLINE void * CXnBase::SysAlloc(size_t size, ULONG tag)
{
    ICHECK(BASE, USER|UDPC);
    void * pv = HalAlloc(size, tag);
#ifdef XNET_FEATURE_ASSERT
    LeakAdd(&_leakinfo, pv, size, tag);
#endif
    return(pv);
}

INLINE void * CXnBase::SysAllocZ(size_t size, ULONG tag)
{
    ICHECK(BASE, USER|UDPC);
    void * pv = HalAllocZ(size, tag);
#ifdef XNET_FEATURE_ASSERT
    LeakAdd(&_leakinfo, pv, size, tag);
#endif
    return(pv);
}

INLINE void CXnBase::SysFree(void * pv)
{
    ICHECK(BASE, USER|UDPC|SDPC);
#ifdef XNET_FEATURE_ASSERT
    LeakDel(&_leakinfo, pv);
#endif
    HalFree(pv);
}

INLINE ULONG CXnBase::RandScaled(ULONG ulMax)
{
    ICHECK(BASE, USER|UDPC|SDPC);
    return(RandLong() % (ulMax + 1));
}

#ifndef XNET_FEATURE_TRACE

INLINE void CXnBase::PushPktRecvTags(BOOL fBroadcast) {}
INLINE void CXnBase::PopPktRecvTags() {}

#endif

// ---------------------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------------------

extern "C" UINT tcpipxsum(UINT xsum, const void * buf, UINT buflen);
const char * DbgVerifyList(LIST_ENTRY * ple, LIST_ENTRY * pleRequire);
char * XnAddrStr(const XNADDR * pxnaddr);
void XnInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf);

// Ethernet Address ----------------------------------------------------------------------

#include <pshpack1.h>

struct CEnetAddr
{
    BYTE _ab[6];

    INLINE BOOL IsEqual(const CEnetAddr & ea) const { return(memcmp(_ab, ea._ab, sizeof(_ab)) == 0); }
    INLINE BOOL IsEqual(const BYTE * pb) const { return(memcmp(_ab, pb, sizeof(_ab)) == 0); }
    INLINE void SetBroadcast()                { *(DWORD *)&_ab[0] = 0xFFFFFFFF; *(WORD *)&_ab[4] = 0xFFFF; }
    INLINE void SetZero()                     { *(DWORD *)&_ab[0] = 0x00000000; *(WORD *)&_ab[4] = 0x0000; }
    INLINE BOOL IsBroadcast() const           { return(*(DWORD *)&_ab[0] == 0xFFFFFFFF && *(WORD *)&_ab[4] == 0xFFFF); }
    INLINE BOOL IsMulticast() const           { return((_ab[0] & 1) != 0); }
    INLINE BOOL IsZero() const                { return(*(DWORD *)&_ab[0] == 0 && *(WORD *)&_ab[4] == 0); }
    char * Str() const;
};

#include <poppack.h>

// Ethernet Link Headers -----------------------------------------------------------------

#define ENET_LINK_MAXSIZE   ROUNDUP4(sizeof(CIeeeHdr))
#define ENET_DATA_MAXSIZE   1500
#define ENET_DATA_MINSIZE   42
#define ENET_TYPE_IP        HTONS_(0x0800)
#define ENET_TYPE_ARP       HTONS_(0x0806)

#include <pshpack1.h>

struct CEnetHdr
{
    CEnetAddr       _eaDst;
    CEnetAddr       _eaSrc;
    WORD            _wType;
};

struct CIeeeHdr : public CEnetHdr
{
    BYTE            _bDsap;         // 0xAA
    BYTE            _bSsap;         // 0xAA
    BYTE            _bControl;      // 0x03
    BYTE            _bOrgCode[3];   // 0x00 0x00 0x00
    WORD            _wTypeIeee;

    INLINE BOOL IsEnetFrame()   { return(*(DWORD *)&_bDsap == 0x0003AAAA && *(WORD *)&_bOrgCode[1] == 0); }
    INLINE BOOL IsValid()       { return(NTOHS(_wType) > ENET_DATA_MAXSIZE || IsEnetFrame()); }
};

#include <poppack.h>

// IP Address in Network Byte Order ------------------------------------------------------

struct CIpAddr                      // IP Address in Network Byte Order
{
    union
    {
        BYTE        _ab[4];
        DWORD       _dw;
    };

    #define IPADDR_BROADCAST            CIpAddr(HTONL_(0xFFFFFFFF))
    #define IPADDR_LOOPBACK             CIpAddr(HTONL_(0x7F000001))
    #define IPADDR_ALLHOSTS             CIpAddr(HTONL_(0xE0000001))
    #define IPADDR_ALLROUTERS           CIpAddr(HTONL_(0xE0000002))
    #define IPADDR_CLASSA_NETMASK       CIpAddr(HTONL_(0xFF000000))
    #define IPADDR_CLASSB_NETMASK       CIpAddr(HTONL_(0xFFFF0000))
    #define IPADDR_CLASSC_NETMASK       CIpAddr(HTONL_(0xFFFFFF00))
    #define IPADDR_CLASSD_NETMASK       CIpAddr(HTONL_(0xF0000000))
    #define IPADDR_CLASSD_NETID         CIpAddr(HTONL_(0xE0000000))
    #define IPADDR_LOOPBACK_NETID       CIpAddr(HTONL_(0x7F000000))
    #define IPADDR_SECURE_NETID         CIpAddr(HTONL_(0x00000000))
    #define IPADDR_SECURE_NETMASK       CIpAddr(HTONL_(0xFF000000))
    #define IPADDR_SECURE_DEFAULT       CIpAddr(HTONL_(0x00000001))

    INLINE operator DWORD () const { return(_dw); }
    INLINE DWORD operator = (DWORD & dw) { return(_dw = dw); }
    INLINE CIpAddr(DWORD dw) { _dw = dw; }
    INLINE CIpAddr(WORD wUniq, BYTE bSlot) { Assert(wUniq > 0); _dw = HTONL((wUniq << 8) | bSlot); Assert(IsSecure()); }
    INLINE CIpAddr() {}

    INLINE BOOL IsBroadcast() const { return(_dw == IPADDR_BROADCAST); }
    INLINE BOOL IsMulticast() const { return(IsClassD() && !IsBroadcast()); }
    INLINE BOOL IsLoopback() const { return((_dw & IPADDR_CLASSA_NETMASK) == IPADDR_LOOPBACK_NETID); }
    INLINE BOOL IsSecure() const { return(_dw && (_dw & IPADDR_SECURE_NETMASK) == IPADDR_SECURE_NETID); }
    INLINE UINT SecureSlot() const { Assert(IsSecure()); return(_dw >> 24); }
    INLINE BOOL IsClassA() const { return((_dw & HTONL_(0x80000000)) == HTONL_(0x00000000)); }
    INLINE BOOL IsClassB() const { return((_dw & HTONL_(0xC0000000)) == HTONL_(0x80000000)); }
    INLINE BOOL IsClassC() const { return((_dw & HTONL_(0xE0000000)) == HTONL_(0xC0000000)); }
    INLINE BOOL IsClassD() const { return((_dw & HTONL_(0xF0000000)) == HTONL_(0xE0000000)); }
    INLINE BOOL IsValidMask() const { DWORD dw = ~NTOHL(_dw); return((dw & (dw + 1)) == 0); }
    BOOL IsValidUnicast() const;
    BOOL IsValidAddr() const;
    CIpAddr DefaultMask() const;
    char * Str() const;
};

// IP Port in Network Byte Order ---------------------------------------------------------

struct CIpPort
{
    WORD            _w;

    INLINE operator WORD () { return (_w); }
    INLINE WORD operator = (WORD & w) { return(_w = w); }
    INLINE CIpPort(WORD w) { _w = w; }
    INLINE CIpPort() {}
};

// IP Header in Network Byte Order -------------------------------------------------------

#include <pshpack1.h>

struct CIpHdr
{
    BYTE            _bVerHdr;       // Version & header length
    BYTE            _bTos;          // Type of service
    WORD            _wLen;          // Total length including IP header
    WORD            _wId;           // Identification
    WORD            _wFragOff;      // Flags and fragment offset
    BYTE            _bTtl;          // Time to live
    BYTE            _bProtocol;     // Protocol
    WORD            _wChecksum;     // Header checksum
    CIpAddr         _ipaSrc;        // Source address
    CIpAddr         _ipaDst;        // Destination address

    INLINE UINT     GetHdrLen()         { return((_bVerHdr & 0xF) << 2); }
    INLINE void     SetHdrLen(UINT cb)  { _bVerHdr = (BYTE)(0x40 | (cb >> 2)); } 
    INLINE UINT     GetOptLen()         { return(GetHdrLen() - sizeof(CIpHdr)); }
    INLINE UINT     GetLen()            { return(NTOHS(_wLen)); }
    INLINE UINT     VerifyHdrLen()      { return((_bVerHdr & 0xF0) == 0x40 ? GetHdrLen() : 0); }

};

#include <poppack.h>

#define MAXIPHDRLEN         (15*4)
#define DONT_FRAGMENT       BIT(14)
#define MORE_FRAGMENTS      BIT(13)
#define FRAGOFFSET_MASK     0x1FFF

#define TraceIpHdr(tag, pIpHdr, cbDat) \
    TraceSz9(tag, "[IP %s %s (%d %d %04X %04X) %d]{%d}[%d]", \
             (pIpHdr)->_ipaDst.Str(), (pIpHdr)->_ipaSrc.Str(), (pIpHdr)->_bTos, (pIpHdr)->_bTtl, \
             NTOHS((pIpHdr)->_wId), NTOHS((pIpHdr)->_wFragOff), (pIpHdr)->_bProtocol, \
             (pIpHdr)->GetOptLen(), (cbDat))

// UDP/TCP Pseudo Header in Network Byte Order -------------------------------------------

#include <pshpack1.h>

struct CPseudoHeader
{
    CIpAddr         _ipaSrc;
    CIpAddr         _ipaDst;
    BYTE            _bZero;
    BYTE            _bProtocol;
    WORD            _wLen;
};

#include <poppack.h>

// UDP Header in Network Byte Order ------------------------------------------------------

#include <pshpack1.h>

struct CUdpHdr
{
    CIpPort         _ipportSrc;     // Source port
    CIpPort         _ipportDst;     // Destination port
    WORD            _wLen;          // Datagram length including UDP header
    WORD            _wChecksum;     // Datagram checksum

    INLINE UINT GetLen()            { return(NTOHS(_wLen)); }
};

#include <poppack.h>

#define TraceUdpHdr(tag, pSocket, pUdpHdr, cbDat) \
        TraceSz5(tag, "[%X.u] [UDP %d %d][%d] %s", (pSocket), \
                  NTOHS((pUdpHdr)->_ipportDst), NTOHS((pUdpHdr)->_ipportSrc), \
                  (cbDat), DataToString((BYTE *)(pUdpHdr) + sizeof(CUdpHdr), cbDat)) \

// TCP Header in Network Byte Order ------------------------------------------------------

#include <pshpack1.h>

struct CTcpHdr
{
    // Definitions -----------------------------------------------------------------------

    #define TCPSEQ  DWORD

    // External --------------------------------------------------------------------------

    INLINE void SetHdrLen(UINT cb)  { _bHdrLen = (BYTE)(ROUNDUP4(cb) << 2); }
    INLINE UINT GetHdrLen()         { return((_bHdrLen & 0xF0) >> 2); }

    // Data ------------------------------------------------------------------------------

    CIpPort         _ipportSrc;     // Source port
    CIpPort         _ipportDst;     // Destination port
    TCPSEQ          _dwSeqNum;      // Sequence number
    TCPSEQ          _dwAckNum;      // Ack number
    BYTE            _bHdrLen;       // Header length (number of DWORDs)
    BYTE            _bFlags;        // Flags
    WORD            _wWindow;       // Window
    WORD            _wChecksum;     // Checksum
    WORD            _wUrgent;       // Urgent pointer

};

#include <poppack.h>

// Return the sequence numbers in a TCP segment (in host byte order)
#define SEG_SEQ(_tcphdr) NTOHL((_tcphdr)->_dwSeqNum)
#define SEG_ACK(_tcphdr) NTOHL((_tcphdr)->_dwAckNum)
#define SEG_WIN(_tcphdr) NTOHS((_tcphdr)->_wWindow)
#define SEG_URG(_tcphdr) NTOHS((_tcphdr)->_wUrgent)

// Return the length of a TCP segment (counting SYN and FIN)
#define SEG_LEN(_tcphdr, _datalen) \
        ((_datalen) + \
         (((_tcphdr)->_bFlags & TCP_SYN) ? 1 : 0) + \
         (((_tcphdr)->_bFlags & TCP_FIN) ? 1 : 0))

// convenience macro for checking segment flags
#define ISTCPSEG(_tcphdr, _flag) ((_tcphdr)->_bFlags & TCP_##_flag)

#define MAXTCPHDRLEN (4*15)
// TCP segment flag bits
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20

// Control flags
#define TCP_CONTROLS (TCP_SYN|TCP_FIN|TCP_RST) 

//
// TCP option tags
//
#define TCPOPT_EOL              0x00
#define TCPOPT_NOP              0x01
#define TCPOPT_MAX_SEGSIZE      0x02
#define TCPOPT_WINDOW_SCALE     0x03
#define TCPOPT_SACK_PERMITTED   0x04
#define TCPOPT_SACK             0x05
#define TCPOPT_TIMESTAMP        0x08

char * DataToString(BYTE * pb, UINT cb);

#define TraceTcpHdr(tag, pTcpSocket, pTcpHdr, cbOpt, cbDat) \
        TraceSz16(tag, "[%X.t] [TCP %d %d %08lX %08lX %d %d%s%s%s%s%s%s]{%d}[%d] %s", (pTcpSocket), \
                  NTOHS((pTcpHdr)->_ipportDst), NTOHS((pTcpHdr)->_ipportSrc), \
                  NTOHL((pTcpHdr)->_dwSeqNum), NTOHL((pTcpHdr)->_dwAckNum), \
                  NTOHS((pTcpHdr)->_wWindow), NTOHS((pTcpHdr)->_wUrgent), \
                  ((pTcpHdr)->_bFlags & TCP_FIN) ? " FIN" : "", \
                  ((pTcpHdr)->_bFlags & TCP_SYN) ? " SYN" : "", \
                  ((pTcpHdr)->_bFlags & TCP_RST) ? " RST" : "", \
                  ((pTcpHdr)->_bFlags & TCP_PSH) ? " PSH" : "", \
                  ((pTcpHdr)->_bFlags & TCP_ACK) ? " ACK" : "", \
                  ((pTcpHdr)->_bFlags & TCP_URG) ? " URG" : "", \
                  (cbOpt), (cbDat), DataToString((BYTE *)(pTcpHdr) + sizeof(CTcpHdr) + (cbOpt), cbDat)) \

// ESP Header in Network Byte Order ------------------------------------------------------

#include <pshpack1.h>

struct CEspHdr : public CUdpHdr
{
    DWORD           _dwSpi;         // Security parameters index
    DWORD           _dwSeq;         // Sequence number
};

struct CEspTail
{
    BYTE            _bPadLen;       // Length of padding
    BYTE            _bNextHeader;   // Protocol
    BYTE            _abHash[12];    // HMAC-SHA-1-96 digest
};

#include <poppack.h>

#define ESP_OVERHEAD_1      (sizeof(CIpHdr) + sizeof(CEspHdr) + (sizeof(CEspTail) - offsetof(CEspTail, _abHash)))
#define ESP_OVERHEAD_8      (offsetof(CEspTail, _abHash))
#define ESP_MSS(n,p)        (((((n) - ESP_OVERHEAD_1) / 8) * 8) - XC_SERVICE_DES_BLOCKLEN - (p) - ESP_OVERHEAD_8)

// ---------------------------------------------------------------------------------------
// CPacket
// ---------------------------------------------------------------------------------------

class CPacket
{
    friend class CPacketQueue;

    // Definitions -----------------------------------------------------------------------

public:

    #define PKTF_RECV_BROADCAST     0x0001      // Packet was received via link-level broadcast
    #define PKTF_RECV_LOOPBACK      0x0002      // Packet was received via local loopback
    #define PKTF_IHL_MASK           0x003C      // Number of DWORD's of IP header options
    #define PKTF_POOLALLOC          0x0040      // Packet was allocated with pool allocator
    #define PKTF_CRYPT              0x0080      // Packet is encrypted on the wire
    #define PKTF_TYPE_MASK          0x0700      // PKTF_TYPE_* mask
    #define PKTF_XMIT_FRAME         0x0800      // Tells ENET layer to send packet without ARP
    #define PKTF_XMIT_PRIORITY      0x1000      // Tells ENET layer to queue packet before others
    #define PKTF_XMIT_DUALPAGE      0x2000      // Used by NIC layer during transmit
    #define PKTF_XMIT_XBDMCLIENT    0x4000      // Used by NIC layer during transmit
    #define PKTF_XMIT_INSECURE      0x8000      // Tells IP layer to bypass security

    #define PKTF_IHL_SHIFT          2
    #define PKTF_TYPE_SHIFT         8

    #define PKTFT(i)                ((i) << PKTF_TYPE_SHIFT)

    #define PKTF_TYPE_IP            PKTFT(0)    // [ENET]^[IP][Payload]
    #define PKTF_TYPE_IP_ESP        PKTFT(1)    // [ENET]^[IP][ESP][Payload][ESPT]
    #define PKTF_TYPE_UDP           PKTFT(2)    // [ENET]^[IP][UDP][Payload]
    #define PKTF_TYPE_UDP_ESP       PKTFT(3)    // [ENET]^[IP][ESP][UDP][Payload][ESPT]
    #define PKTF_TYPE_TCP           PKTFT(4)    // [ENET]^[IP][TCP][Payload]
    #define PKTF_TYPE_TCP_ESP       PKTFT(5)    // [ENET]^[IP][ESP][TCP][Payload][ESPT]
    #define PKTF_TYPE_ENET          PKTFT(6)    // [ENET]^[Payload]
    #define PKTF_TYPE_ESP           PKTFT(1)    // Bit common to all [ESP] types

    #define PKTI_ESP                0           // [ESP]
    #define PKTI_UDP_TCP            1           // [UDP]/[TCP]
    #define PKTI_SIZE               2           // Size of frame excluding [ENET] and [Payload]
    #define PKTI_MAX                3           // Number of PKTI_* defined

    // Allocate / Deallocate -------------------------------------------------------------

public:

    INLINE void Init(UINT uiFlags, void * pv, UINT cb, PFNPKTFREE pfn)
    {
        _pv         = pv;
        _cb         = (WORD)cb;
        _wFlags     = (WORD)uiFlags;
        _ppktNext   = NULL;
        _pfn        = pfn;
#if DBG
        _fAllocated = FALSE;
        DbgSetSpy();
#endif
    }

    INLINE void Complete(CXnVoid * pXnVoid)
    {
        (pXnVoid->*_pfn)(this);
    }

    // Member Functions ------------------------------------------------------------------

public:

    INLINE WORD        GetFlags()              { return(_wFlags); }
    INLINE void        SetFlags(WORD wFlags)   { _wFlags |= wFlags; IFDBG(DbgSetSpy();) }
    INLINE void        ClearFlags(WORD wFlags) { _wFlags &= ~wFlags; IFDBG(DbgSetSpy();) }
    INLINE BOOL        TestFlags(WORD wFlags)  { return(!!(_wFlags & wFlags)); }
    INLINE WORD        GetType()               { return(_wFlags & PKTF_TYPE_MASK); }
    INLINE void        SetType(WORD wFlags)    { _wFlags = (_wFlags & ~PKTF_TYPE_MASK) | (wFlags & PKTF_TYPE_MASK); IFDBG(DbgSetSpy();) }

    INLINE void *      GetPv()                 { return(_pv); }
    INLINE void        SetPv(void * pv)        { _pv = pv; }
    INLINE UINT        GetCb()                 { return(_cb); }
    INLINE void        SetCb(UINT cb)          { _cb = (WORD)cb; }
    INLINE void        SetPfn(PFNPKTFREE pfn)  { _pfn = pfn; }
    INLINE UINT        GetHdrOptLen()          { Assert(PKTF_IHL_SHIFT == 2); return(_wFlags & PKTF_IHL_MASK); }
    INLINE void        SetHdrOptLen(UINT cb)   { Assert(PKTF_IHL_SHIFT == 2); Assert(ROUNDUP4(cb) == cb); _wFlags = (_wFlags & ~PKTF_IHL_MASK) | cb; }

    INLINE BOOL        IsEnet()                { return(GetType() == PKTF_TYPE_ENET); }
    INLINE BOOL        IsIp()                  { return(GetType() <  PKTF_TYPE_ENET); }
    INLINE BOOL        IsEsp()                 { return(TestFlags(PKTF_TYPE_ESP)); }
    INLINE BOOL        IsCrypt()               { Assert(IsEsp()); return(TestFlags(PKTF_CRYPT)); }
    INLINE BOOL        IsUdp()                 { return(TestFlags(PKTF_TYPE_UDP)); }
    INLINE BOOL        IsTcp()                 { return(TestFlags(PKTF_TYPE_TCP)); }

           void *      GetHdr(int iHdr);
    INLINE CEnetHdr *  GetEnetHdr()            { return((CEnetHdr *)((BYTE *)_pv - sizeof(CEnetHdr))); }
    INLINE CIpHdr *    GetIpHdr()              { Assert(IsIp()); return((CIpHdr *)_pv); }
    INLINE CEspHdr *   GetEspHdr()             { Assert(IsEsp()); return((CEspHdr *)GetHdr(PKTI_ESP)); }
    INLINE CUdpHdr *   GetUdpHdr()             { Assert(IsUdp()); return((CUdpHdr *)GetHdr(PKTI_UDP_TCP)); }
    INLINE CTcpHdr *   GetTcpHdr()             { Assert(IsTcp()); return((CTcpHdr *)GetHdr(PKTI_UDP_TCP)); }
    INLINE CEspTail *  GetEspTail()            { Assert(IsEsp()); return((CEspTail *)((BYTE *)_pv + _cb - sizeof(CEspTail))); }
    
    INLINE CPacket *   GetNextPkt()            { return(_ppktNext); }

    // Data ------------------------------------------------------------------------------

private:

    void *              _pv;                // [LINK]^[Payload]
    WORD                _cb;                // Size of [Payload]
    WORD                _wFlags;            // See PKTF_* above
    CPacket *           _ppktNext;          // Next packet in packet queue
    PFNPKTFREE          _pfn;               // Function which frees the packet

#if DBG

    struct CSpy
    {
        CEnetHdr *      _pEnetHdr;          // Pointer to CEnetHdr (if any)
        CIpHdr *        _pIpHdr;            // Pointer to CIpHdr (if any)
        CEspHdr *       _pEspHdr;           // Pointer to CEspHdr (if any)
        CUdpHdr *       _pUdpHdr;           // Pointer to CUdpHdr (if any)
        CTcpHdr *       _pTcpHdr;           // Pointer to CTcpHdr (if any)
        CEspTail *      _pEspTail;          // Pointer to CEspTail (if any)
    };

private:

    const char *        _pchSpy;            // String describing packet format
    CSpy                _spy;               // Spy structure
    BOOL                _fAllocated;        // Packet was allocated via PacketAlloc

public:

    void                DbgSetSpy();
    void                SetAllocated()      { _fAllocated = TRUE; }
    BOOL                IsAllocated()       { return(_fAllocated); }
    void                Validate();

#else

public:

    INLINE void         Validate()          {}

#endif

public:

    static const BYTE         _abPktTypeInfo[];
    static const char * const _aszPktTypeName[];

};

// ---------------------------------------------------------------------------------------
// CPacketQueue
// ---------------------------------------------------------------------------------------

class CPacketQueue
{

public:

    void                InsertHead(CPacket * ppkt);
    void                InsertTail(CPacket * ppkt);
    void                InsertHead(CPacketQueue * ppq);
    CPacket *           RemoveHead();
    void                Dequeue(CPacket * ppktDequeue);
    void                Complete(CXnBase * pXnBase);
    void                Discard(CXnBase * pXnBase);
    UINT                Count();

    INLINE void         Init()      { _ppktHead = NULL; _ppktTail = NULL; }
    INLINE BOOL         IsEmpty()   { return(_ppktHead == NULL); }
    INLINE CPacket *    GetHead()   { return(_ppktHead); }

private:

    CPacket *           _ppktHead;
    CPacket *           _ppktTail;

};

// ---------------------------------------------------------------------------------------
// CXnNic
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_WINDOWS
#include "nicw.h"
#endif

#ifdef XNET_FEATURE_XBOX
#include "nicx.h"
#endif

// ---------------------------------------------------------------------------------------
// CXnEnet
// ---------------------------------------------------------------------------------------

class NOVTABLE CXnEnet : public CXnNic
{
    friend void CXnNic::EnetRecv(CPacket * ppkt, UINT uiType);
    friend void CXnNic::EnetPush();

    // Definitions -----------------------------------------------------------------------

#ifdef XNET_FEATURE_ARP

    #define ARP_STATE_FREE      0           // entry is free
    #define ARP_STATE_IDLE      1           // entry is good
    #define ARP_STATE_BAD       2           // target is unreachable
    #define ARP_STATE_BUSY      3           // entry is being resolved

    #define ARP_OP_REQUEST      HTONS_(1)   // opcodes
    #define ARP_OP_REPLY        HTONS_(2)

    #define ARP_HWTYPE_ENET     HTONS_(1)   // hardware address spaces
    #define ARP_HWTYPE_802      HTONS_(6)

    enum ArpResolve
    {
        eNone,
        eCreateEntry,
        eSendRequest
    };

    #define ARP_CACHE_SIZE      64          // cache parametes (modulo should be prime)
    #define ARP_HASH_RETRY      5
    #define ARP_HASH_MODULO     (ARP_CACHE_SIZE-ARP_HASH_RETRY)
    #define ARP_HASH(_addr)     ((_addr) % ARP_HASH_MODULO)

    struct CArpEntry
    {
        CIpAddr         _ipa;
        DWORD           _dwTick;
        CPacketQueue    _pqWait;
        CEnetAddr       _eaNext;
        WORD            _wState;

        INLINE CArpEntry() {}
        INLINE BOOL IsFree()   { return(_wState == ARP_STATE_FREE); }
        INLINE BOOL IsIdle()   { return(_wState == ARP_STATE_IDLE); }
        INLINE BOOL IsBad()    { return(_wState == ARP_STATE_BAD); }
        INLINE BOOL IsBusy()   { return(_wState >= ARP_STATE_BUSY); }
    };

    #include <pshpack1.h>

    struct CArpMsg
    {
        WORD            _wHrd;      // hardware address space
        WORD            _wPro;      // protocol address space (ENET_TYPE_IP)
        BYTE            _bHln;      // hardware address length (6)
        BYTE            _bPln;      // protocol address length (4)
        WORD            _wOp;       // opcode
        CEnetAddr       _eaSender;  // sender's hardware address
        CIpAddr         _ipaSender; // sender's protocol address
        CEnetAddr       _eaTarget;  // target's hardware address
        CIpAddr         _ipaTarget; // target's protocol address
    };

    #include <poppack.h>

#endif

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnEnet)

    void            EnetXmit(CPacket * ppkt, CIpAddr ipaNext = 0);
    void            EnetXmitArp(CIpAddr ipa);
    void            EnetPush();

protected:

    NTSTATUS        EnetInit(XNetInitParams * pxnip);
    INLINE void     EnetStart()     { NicStart(); }
    void            EnetStop();
    void            EnetTerm();

    VIRTUAL void    IpRecv(CPacket * ppkt) VPURE;
    VIRTUAL void    IpRecvArp(CEnetAddr * pea) VPURE;

    VIRTUAL void    EnetRecv(CPacket * ppkt, UINT uiType);
    VIRTUAL void    EnetQueuePush();

    // Internal --------------------------------------------------------------------------

private:

#ifdef XNET_FEATURE_ARP

    void            ArpXmit(WORD wOp, CIpAddr ipaTarget, CIpAddr ipaSender, CEnetAddr * peaTarget);
    CArpEntry *     ArpLookup(CIpAddr ipa, ArpResolve eResolve);
    void            ArpTimer(CTimer * pt);
    void            ArpRecv(CPacket * ppkt);

#endif

    void            EnetFillAndXmit(CPacket * ppkt, CEnetAddr * peaNext);

    static void     EnetDpc(PKDPC, void *, void *, void *);
    void            EnetProcessDpc();

    // Data ------------------------------------------------------------------------------
    
public:

    CIpAddr         _ipa;                   // IP address of this interface
    CIpAddr         _ipaMask;               // IP address mask
    CIpAddr         _ipaSubnet;             // IP subnet address

private:

    CPacketQueue    _pqXmit;                // Transmit packet queue
    KDPC            _dpcEnet;               // DPC

#ifdef XNET_FEATURE_ARP

    CIpAddr         _ipaCheck;              // IP address we are checking for uniqueness
    CArpEntry *     _paeLast;               // Last entry fetched
    CArpEntry       _aae[ARP_CACHE_SIZE];   // Arp cache
    CTimer          _timerArp;              // Arp timer

#endif

};    

// ---------------------------------------------------------------------------------------
// CRouteEntry
// ---------------------------------------------------------------------------------------

class CRouteEntry
{
    friend class CXnIp;

    // Definitions -----------------------------------------------------------------------

    #define RTEF_ORPHAN         0x0001             // the route entry is orphaned
    #define RTEF_HOST           0x0002             // host route
    #define RTEF_DEFAULT        0x0004             // default gateway route
    #define RTEF_LOCAL          0x0008             // local route

    #define RTE_DEFAULT_METRIC  1

    // External --------------------------------------------------------------------------

private:

    HAL_DECLARE_NEW_DELETE(CRouteEntry)

    void            AddRef()                    { InterlockedIncrement(&_cRefs); }

    // Internal --------------------------------------------------------------------------

    INLINE void     SetFlags(WORD wFlags)       { _wFlags |= wFlags; }
    INLINE void     ClearFlags(WORD wFlags)     { _wFlags &= ~wFlags; }
    INLINE BOOL     TestFlags(WORD wFlags)      { return(!!(_wFlags & wFlags)); }

    INLINE BOOL     IsOrphan()                  { return(TestFlags(RTEF_ORPHAN)); }
    INLINE BOOL     IsLocal()                   { return(TestFlags(RTEF_LOCAL)); }
    INLINE BOOL     IsHost()                    { return(TestFlags(RTEF_HOST)); }

    // Data ------------------------------------------------------------------------------

    LIST_ENTRY      _le;                        // Doubly linked list entry
    LONG            _cRefs;                     // Reference count
    WORD            _wFlags;                    // See RTEF_*
    WORD            _wMetric;                   // Route metrics (smaller is higher priority)
    CIpAddr         _ipaDst;                    // Destination IP address
    CIpAddr         _ipaMask;                   // Destination IP address mask
    CIpAddr         _ipaNext;                   // Next hop IP address

};

// ---------------------------------------------------------------------------------------
// CXnIp
// ---------------------------------------------------------------------------------------

struct CDnsReg;
struct CDnsHdr;

class NOVTABLE CXnIp : public CXnEnet
{
    friend void CXnBase::SecRegProbe();
    friend void CXnEnet::IpRecv(CPacket * ppkt);
    friend void CXnEnet::IpRecvArp(CEnetAddr * pea);

    #define MAX_DEFAULT_GATEWAYS 4
    #define MAX_DEFAULT_DNSSERVERS 4

    typedef DWORD DHCPTIME;

    struct CSecReg;

    struct CDhcpOptions
    {
        INLINE CDhcpOptions() {}
        INT         _recvMsgType;           // type message received
        CIpAddr     _dhcpmask;              // subnet mask
        CIpAddr     _dhcpServer;            // dhcp server address
        DHCPTIME    _t1time;                // when to enter renewing state
        DHCPTIME    _t2time;                // when to enter rebinding state
        DHCPTIME    _exptime;               // lease expiration time
        UINT        _gatewayCount;          // number of gateways
        CIpAddr     _gateways[MAX_DEFAULT_GATEWAYS];
        UINT        _dnsServerCount;
        CIpAddr     _dnsServers[MAX_DEFAULT_DNSSERVERS];
    };

    struct CQosPkt : public CPacket
    {
        void *          _pvQos;             // Pointer to CQosReq or CQosListen owning this packet
        BOOL            _fQosReq;           // TRUE if packet belongs to CQosReq; FALSE for CQosListen
        BOOL            _fInXmit;           // TRUE if packet is being transmitted
    };

    struct CQosInfo
    {
        LIST_ENTRY      _le;                // Link into _leXmit{Wait} or _leOpen{Wait}
        XNADDR          _xnaddr;            // XNet Address (XbToXb)
        XNKID           _xnkid;             // Key identifier (XbToXb)
        XNKEY           _xnkey;             // Key exchange key (XbToXb)
        CSecReg *       _pSecReg;           // Pointer to CSecReg (XbToSg)
        BYTE            _bState;            // Current state of this Qos item
        BYTE            _bRetry;            // Retries remaining at current state
        CIpPort         _ipportDst;         // Destination port
        CIpAddr         _ipaDst;            // Destination IP address
        UINT            _cbQos;             // Size of Qos data
        BYTE *          _pbQos;             // Qos data buffer
    };

    struct CQosReq
    {
        CQosReq *       _pQosReqNext;       // Next CQosReq item in the list
        BOOL            _fXbToSg;           // TRUE if XB to SG; FALSE if XB to XB
        PRKEVENT        _pEvent;            // Event to signal whenever progress is made
        DWORD           _dwFlags;           // XNET_QOS_* flags from request
        BYTE            _abNonce[8];        // Nonce for this request
        LIST_ENTRY      _leXmit;            // Queue of CQosInfo ready to xmit
        LIST_ENTRY      _leWait;            // Queue of CQosInfo waiting for response
        UINT            _iQosInfo;          // Index of CQosInfo to process next
        UINT            _cQosInfo;          // Count of CQosInfo items
        CQosInfo *      _pQosInfo;          // Array of CQosInfo items
        CQosPkt *       _pQosPkt1;          // Pointer to first Qos packet in packet pair
        CQosPkt *       _pQosPkt2;          // Pointer to second Qos packet in packet pair
        BYTE *          _pbQosData;         // Pointer to Qos data buffer for all CQosInfo
        XNQOS           _xnqos;             // User-visible XNQOS structure
    };

    struct CQosReg
    {
        DWORD           _dwBitsPerSec;      // Bandwidth limit for responses
        CQosPkt *       _pQosPkt1;          // Pointer to first Qos packet in packet pair
        CQosPkt *       _pQosPkt2;          // Pointer to second Qos packet in packet pair
        UINT            _cbQos;             // Size of Qos data
        BYTE            _abQos[1];          // Qos data buffer
    };

    struct CKeyReg
    {
        // External ----------------------------------------------------------------------

        INLINE CKeyReg() {}

        // Data --------------------------------------------------------------------------

        XNKID           _xnkid;             // key identifier
        BYTE            _abKeySha[16];      // key exchange SHA auth key
        BYTE            _abKeyDes[8];       // key exchange DES key
        BYTE            _abDhX[CBDHG1];     // diffie-hellman X value
        BYTE            _abDhGX[CBDHG1];    // diffie-hellman g^X value
        CQosReg *       _pQosReg;           // Pointer to CQosReg if listening
    };

    #include <pshpack1.h>

    struct CKeyExXbToXb : public CKeyExHdr
    {
        XNKID           _xnkid;             // key identifier of key-exchange-key
        DWORD           _dwSpiInit;         // SPI of the initiator
        DWORD           _dwSpiResp;         // SPI of the responder
        BYTE            _abNonceInit[8];    // Nonce of the initiator
        BYTE            _abNonceResp[8];    // Nonce of the responder
        LARGE_INTEGER   _liTime;            // Increasing time-value of the sender
        BYTE            _abIv[8];           // Initialization vector for DES encryption
        XNADDR          _xnaddrInit;        // XNADDR of the initiator (encrypted)
        XNADDR          _xnaddrResp;        // XNADDR of the responder (encrypted)
    };

    struct CKeyExNatOpen : public CKeyExHdr
    {
        // Definitions -------------------------------------------------------------------
        
        #define KNOF_XBTOXB_KEYEX   0x0001  // Request occuring for key exchange
        #define KNOF_XBTOXB_PROBE   0x0002  // Request occuring for probing

        // Data --------------------------------------------------------------------------

        XNKID           _xnkid;             // key identifier of key-exhcange-key
        DWORD           _dwCtx;             // Context identifier from the sender
        BYTE            _abNonce[8];        // Nonce from the sender
        CIpAddr         _ipaDst;            // IP address of the sender
        CIpPort         _ipportDst;         // IP port of the sender
        WORD            _wFlags;            // See KNOF_* above
        BYTE            _abHash[20];        // SHA hash of this message (including header)
    };

    struct CKeyExQos : public CKeyExHdr
    {
        DWORD           _dwReserved;        // Placeholder
    };

    #include <poppack.h>

    struct CSecReg
    {
        // Definitions -------------------------------------------------------------------

        #define SR_STATE_IDLE       0       // key exchange not started by either side
        #define SR_STATE_INITSENT   1       // initiator packet has been sent
        #define SR_STATE_RESPSENT   2       // response packet has been sent
        #define SR_STATE_INITWAIT   3       // initiator waiting for responder to synchronize
        #define SR_STATE_READY      4       // key exchange is complete

        #define SRF_SYSTEMLINK      0x0001  // Other side is an XB via system link
        #define SRF_ONLINEPEER      0x0002  // Other side is an XB that is online
        #define SRF_ONLINESERVER    0x0004  // Other side is an SG
        #define SRF_SECMSG_DELETE   0x0008  // SECMSG_TYPE_DELETE has been sent
        #define SRF_OWNED           0x0010  // This side owns this CSecReg

        // External ----------------------------------------------------------------------

        INLINE      CSecReg() {}
        INLINE      IsXmitReady() { return(_bState >= SR_STATE_INITWAIT); }
        INLINE      IsRecvReady() { return(_bState >= SR_STATE_RESPSENT); }
        char *      CSecReg::Str();

        INLINE BOOL TestFlags(WORD wFlags)  { return(!!(_wFlags & wFlags)); }
        INLINE void SetFlags(WORD wFlags)   { _wFlags |= wFlags; }
        INLINE void ClearFlags(WORD wFlags) { _wFlags &= ~wFlags; }

        // Data --------------------------------------------------------------------------

        BYTE            _wFlags;                // See SRF_* above
        BYTE            _bState;                // See SR_STATE_* above
        BYTE            _bRetry;                // Number of retries remaining
        DWORD           _dwSpiRecv;             // SPI assigned by this side
        DWORD           _dwSpiXmit;             // SPI assigned by other side
        DWORD           _dwSeqRecv;             // Base sequence of receive window
        DWORD           _dwSeqMask;             // Bit mask of sequence numbers received
        DWORD           _dwSeqXmit;             // Highest sequence number transmitted
        DWORD           _dwTickRecv;            // TimerTick when last packet recv
        DWORD           _dwTickXmit;            // TimerTick when last packet xmit
        DWORD           _dwTickPulse;           // TimerTick when last pulse xmit
        DWORD           _dwTickPulseTimeout;    // Outbound inactivity before sending pulse
        DWORD           _dwTickTimeout;         // Inbound inactivity before timing out connection
        CPacketQueue    _pqWait;                // Queue of packets waiting
        CTimer          _timer;                 // Timer for scheduling timeouts
        BYTE            _abIv[8];               // Next IV to use for encrypted packets
        BYTE            _abNonceInit[8];        // Nonce generated by the initiator
        BYTE            _abNonceResp[8];        // Nonce generated by the responder
        BYTE            _abKeyShaRecv[16];      // Negotiated incoming SHA auth key
        BYTE            _abKeyDesRecv[24];      // Negotiated incoming DES crypt key
        WORD            _cbKeyDesRecv;          // Size of incoming DES crypt key
        BYTE            _abKeyShaXmit[16];      // Negotiated outgoing SHA auth key
        BYTE            _abKeyDesXmit[24];      // Negotiated outgoing DES crypt key
        WORD            _cbKeyDesXmit;          // Size of outgoing DES crypt key
        CIpAddr         _ipaDst;                // Destination IP address for ONLINEPEER or ONLINESERVER
        CIpPort         _ipportDst;             // Destination port for ONLINEPEER or ONLINESERVER
        LARGE_INTEGER   _liTime;                // [XB] Highest time value, [SG] Kerberos time
        union {
            struct {
                CKeyReg *       _pKeyReg;       // [XB] Pointer to corresponding CKeyReg
                XNADDR          _xnaddr;        // [XB] XNADDR of the destination
            };
            struct {
                DWORD           _dwServiceId;   // [SG] Service Identifier
                SGADDR          _sgaddr;        // [SG] SGADDR assigned by SG
                CIpAddr         _ipaNat;        // [SG] NAT'd source IP address as seen by SG
                CIpPort         _ipportNat;     // [SG] NAT'd source port as seen by SG
            };
        };
    };

    // External --------------------------------------------------------------------------

public:

    INLINE          CXnIp() {};

    void            IpXmit(CPacket * ppkt, CRouteEntry ** pprte = NULL);
    void            IpFillAndXmit(CPacket * ppkt, CIpAddr ipaDst, BYTE bProtocol, CRouteEntry ** pprte = NULL);
    void            IpFillHdr(CPacket * ppkt, CIpAddr ipaDst, BYTE bProtocol);
    void            IpSetAddress(CIpAddr ipa, CIpAddr ipaMask);

    BOOL            IpDecrypt(CPacket * ppkt, CIpAddr ipaDst);

    INT             IpCreateKey(XNKID * pxnkid, XNKEY * pxnkey);
    INT             IpRegisterKey(const XNKID * pxnkid, const XNKEY * pxnkey);
    INT             IpUnregisterKey(const XNKID * pxnkid);
    INT             IpXnAddrToInAddr(const XNADDR * pxna, const XNKID * pxnkid, CIpAddr * pipa);
    INT             IpServerToInAddr(const CIpAddr ipa, DWORD dwServiceId, CIpAddr * pipa);
    INT             IpInAddrToXnAddr(const CIpAddr ipa, XNADDR * pxna, XNKID * pxnkid);
    INT             IpUnregisterInAddr(const CIpAddr ipa);
    DWORD           IpGetXnAddr(XNADDR * pxna);

    void            RouteRelease(CRouteEntry * prte);
    INLINE WORD     GetNextDgramId()    { return((WORD)InterlockedIncrement(&_lNextDgramId)); }

#ifdef XNET_FEATURE_DNS
    INT             IpDnsLookup(const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns);
    INT             IpDnsRelease(XNDNS * pxndns);
#endif

#ifndef XNET_FEAUTRE_QOS
    INT             IpQosListen(const XNKID * pxnkid, const BYTE * pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags);
    INT             IpQosXnAddr(UINT cxnqos, const XNADDR * apxna[], const XNKID * apxnkid[], const XNKEY * apxnkey[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
    INT             IpQosServer(UINT cxnqos, const IN_ADDR aina[], const DWORD adwServiceId[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
    INT             IpQosRelease(XNQOS * pxnqos);
#endif

    void            SecRegSetOwned(CIpAddr ipa);
    void            SecRegShutdown(BOOL fOnlineOnly);
    INLINE BYTE     SecRegRexmitRetries(CSecReg * pSecReg);
    INLINE BYTE     SecRegRexmitTimeoutInSeconds(CSecReg * pSecReg);

#ifdef XNET_FEATURE_ONLINE
    INLINE CXoBase *IpGetXoBase() { return(_pXoBase); }
    void            IpSetXoBase(CXoBase * pXoBase);
    void            IpRaiseToDpc(BOOL fRaise);
#endif

#ifdef XNET_FEATURE_SG
    void            IpLogon(CIpAddr ipaLogon, ULONGLONG * pqwUserId, WSAEVENT hEventLogon);
    DWORD           IpLogonGetStatus(SGADDR * psgaddr);
    BOOL            IpLogonGetQFlags(UINT iUserId, ULONGLONG * pqwUserId, DWORD * pdwQFlags, DWORD * pdwSeqQFlags);
    BOOL            IpLogonSetQFlags(UINT iUserId, DWORD dwQFlags, DWORD dwSeqQFlags);
    BOOL            IpLogonSetPState(UINT iUserId, DWORD dwPState, const XNKID * pxnkid, UINT cbData, BYTE * pbData);
    void            IpLogoff();
#endif

    void            IpSetEventTimer(CEventTimer * pEventTimer, WSAEVENT hEvent, DWORD dwTimeout);
    void            EventTimer(CTimer * pt);

protected:

    NTSTATUS        IpInit(XNetInitParams * pxnip);
    INT             IpConfig(const XNetConfigParams * pxncp, DWORD dwFlags);
    INT             IpGetConfigStatus(XNetConfigStatus * pxncs);
    INLINE void     IpStart()       { EnetStart(); }
    void            IpStop();
    void            IpTerm();

    VIRTUAL void    IpRecv(CPacket * ppkt);
    VIRTUAL void    IpRecvArp(CEnetAddr * pea);

    VIRTUAL void    UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen) VPURE;
    VIRTUAL void    TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen) VPURE;
    VIRTUAL void    SockReset(CIpAddr ipa) VPURE;

#ifdef XNET_FEATURE_WINDOWS
    VIRTUAL void    EnetXmit(CPacket * ppkt, CIpAddr ipaNext = 0) { CXnEnet::EnetXmit(ppkt, ipaNext); }
    INLINE  void    IpXmitIp(CPacket * ppkt) { IpXmitIp(ppkt, NULL); }
    VIRTUAL BOOL    IsGateway(CIpPort ipport) { return(FALSE); }
#else
    INLINE  BOOL    IsGateway(CIpPort ipport) { return(FALSE); }
#endif

    // Internal --------------------------------------------------------------------------

private:
    
    void            IpXmitEsp(CPacket * ppkt, CSecReg * pSecReg, CRouteEntry ** pprte);
    void            IpXmitIp(CPacket * ppkt, CRouteEntry ** pprte);

    NTSTATUS        RouteInit();
    void            RouteTerm();
    CRouteEntry *   RouteLookup(CIpAddr ipaDst);
    void            RouteAdd(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext, WORD wFlags, WORD wMetric);
    void            RouteRedirect(CIpAddr ipaDst, CIpAddr ipaOldGateway, CIpAddr ipNewGateway);
    void            RouteDelete(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext);
    void            RouteListOrphan();
    void            RouteInvalidateCache();
    void            RouteEntryOrphan(CRouteEntry * prte);
    void            RouteListDump();

    void            FragTerm();
    void            FragRecv(CPacket * ppkt, CIpHdr * pIpHdr, UINT cbHdrLen, UINT cbLen);
    void            FragTimer(CTimer * pt);
    void            FragFree(CPacket * ppkt);

    void            IcmpRecv(CPacket* ppkt, CIpHdr * pIpHdr, void * pvData, UINT cbData);

    NTSTATUS        DhcpInit();
    INT             DhcpConfig(const XNetConfigParams * pxncp);
    void            DhcpTerm();
    void            DhcpTimer(CTimer * pt);
    void            DhcpRecv(CPacket * ppkt, CUdpHdr * pUdpHdr, UINT cbLen);
    void            DhcpNotifyAddressConflict();
    BOOL            DhcpForceAutonet(CIpAddr addr, CIpAddr mask);
    void            DhcpTimerSetRelative(UINT minWait, UINT maxWait);
    void            DhcpTimerSetAbsolute(DHCPTIME dhcptime);
    void            DhcpComputeTimeout();
    void            DhcpChangeState(INT state);
    void            DhcpResetInterface();
    NTSTATUS        DhcpSendMessage(CPacket * ppkt, UINT msglen, BOOL broadcast);
    BYTE *          DhcpFillMessageHeader(BYTE * buf, INT msgtype);
    void            DhcpComputeSecsSinceStart();
    NTSTATUS        DhcpSendDiscover();
    NTSTATUS        DhcpSendRequest();
    NTSTATUS        DhcpSendInform();
    NTSTATUS        DhcpSendRelease();
    NTSTATUS        DhcpProcessOffer(CIpAddr yiaddr, CDhcpOptions * param);
    void            DhcpAddOrRemoveGateways(BOOL doDelete);
    void            DhcpSetDefaultGateways();
    void            DhcpUseOptionParams(CDhcpOptions * param);
    NTSTATUS        DhcpProcessAck(CIpAddr yiaddr, CDhcpOptions * param);
    NTSTATUS        DhcpProcessNak(CIpAddr dhcpServer);
    void            DhcpSelectAutonetAddr();
    BYTE *          DhcpAppendLeaseTimeOption(BYTE * option);
    BOOL            DhcpValidateOffer(CIpAddr yiaddr, CDhcpOptions * param);
    BOOL            ActiveDhcpAddr();
    BOOL            ActiveAutonetAddr();
    BOOL            ActiveStaticAddr();
    BOOL            ActiveNoAddr();
    NTSTATUS        DhcpParseOptionParams(CDhcpOptions * param, const BYTE* buf, UINT buflen, BYTE* overload);
    DHCPTIME        DhcpTime();

    void            IpXmitDns(CDnsReg * pdnsreg);
    void            IpRecvDns(CPacket * ppkt, CUdpHdr * pUdpHdr, UINT cbLen);
    void            IpDnsTimer(CTimer * pt);
    void            IpDnsSignal(CDnsReg * pdnsreg, INT iStatus);
    void            IpDnsDump(CDnsHdr * pdnshdr, UINT cbDnsMsg);
    BYTE *          IpDnsDumpSection(BYTE * pbBeg, BYTE * pbEnd, BYTE * pb, UINT cSec, char * pszSec);
    void            IpDnsDumpHex(BYTE * pb, UINT cb);

    void            IpRecvUdp(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    void            IpRecvTcp(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbLen);
    void            IpRecvEsp(CPacket * ppkt, CIpHdr * pIpHdr, CEspHdr * pEspHdr, UINT cbLen);
    void            IpRecvKeyEx(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExHdr * pKeyExHdr, UINT cbKeyEx);
    void            IpRecvKeyExXbToXb(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExXbToXb * pKeyExXbToXb, UINT cbKeyEx);
    void            IpRecvKeyExSgToXb(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExSgToXbResp * pKeyExSgToXbResp, UINT cbKeyEx);
    void            IpRecvKeyExNatOpen(CPacket * ppkt, CIpAddr ipaSrc, CIpPort ipportSrc, CKeyExNatOpen * pKeyExNatOpen, UINT cbKeyEx);
    void            IpRecvSecMsg(CPacket * ppkt, CSecReg * pSecReg, DWORD dwSeq, CSecMsgHdr * pSecMsgHdr, UINT cb);

    void            IpXmitKeyEx(CSecReg * pSecReg);
    void            IpXmitKeyExXbToXb(CSecReg * pSecReg, BOOL fInhibitNatOpen = FALSE);
    void            IpFillKeyExXbToXb(CSecReg * pSecReg, CKeyExXbToXb * pKeyExXbToXb);
    void            IpXmitKeyExXbToSg(CSecReg * pSecReg);
    void            IpXmitSecMsg(CSecReg * pSecReg, WORD wType, void * pv1 = NULL, UINT cb1 = 0, void * pv2 = 0, UINT cb2 = 0);
    void            IpXmitSecMsgDelete(CSecReg * pSecReg, DWORD dwReason);
    void            IpXmitSecMsgXbToSgPulse(CSecReg * pSecReg);

    CKeyReg *       KeyRegLookup(const XNKID * pxnkid);

    void            CryptDes(DWORD dwOp, BYTE * pbKey, UINT cbKey, BYTE * pbIv, BYTE * pb, UINT cb);

    CSecReg *       SecRegLookup(const XNADDR * pxna, const XNKID * pxnkid);
    CSecReg *       SecRegLookup(const CIpAddr ipa, DWORD dwServiceId, const XOKERBINFO * pxokerbinfo);
    CSecReg *       SecRegLookup(DWORD dwSpiRecv);
    CSecReg *       SecRegAlloc();
    CSecReg *       SecRegAlloc(const XNADDR * pxna, CKeyReg * pKeyReg);
    CSecReg *       SecRegAlloc(const CIpAddr ipa, DWORD dwServiceId);
    void            SecRegFree(CSecReg * ppSecReg);

    void            SecRegEnqueue(CSecReg * pSecReg, CPacket * ppkt);
    void            SecRegXmitQueue(CSecReg * pSecReg);
    void            SecRegSetIdle(CSecReg * pSecReg);
    void            SecRegSetTicks(CSecReg * pSecReg);
    void            SecRegSetKey(CSecReg * pSecReg, BYTE * pbKeyHmac, UINT cbKeyHmac, BYTE * pbDhX, UINT cbDhX, BYTE * pbDhGY, UINT cbDhGY, BOOL fInitiator);
    void            SecRegTimer(CTimer * pt);
    void            SecRegProbe();

    // Data ------------------------------------------------------------------------------

protected:

#ifdef XNET_FEATURE_DHCP
    CDhcpOptions    _options;
#endif

private:

    LONG            _lNextDgramId;

#ifdef XNET_FEATURE_ROUTE
    LIST_ENTRY      _leRouteList;
    CIpAddr         _ipaDstLast;
    CRouteEntry *   _prteLast;
#endif

#ifdef XNET_FEATURE_FRAG
    CPacketQueue    _pqFrag;
    UINT            _cFrag;
#endif

#ifdef XNET_FEATURE_DHCP
    INT             _state;             // current state of the interface
    INT             _flags;             // misc. flags (see constants below)
    CIpAddr         _activeaddr;        // active IP address
    CIpAddr         _activemask;        // active IP mask
    CIpAddr         _dhcpaddr;          // last DHCP address
    CIpAddr         _autonetaddr;       // last autonet address
    DHCPTIME        _acqtime;           // when lease acquisition started
    DWORD           _xid;               // XID for the next outgoing message
    UINT            _retries;           // number of retries for the current message
    UINT            _initRetryCount;    // how many retries into INIT state
    UINT            _secsSinceStart;    // secs since the address-req process started
    CTimer          _timerDhcp;
    DWORD           _dwXidNext;
#endif

#ifdef XNET_FEATURE_DNS
    CDnsReg *       _pdnsreg;           // Active DNS lookups
#endif

#if defined(XNET_FEATURE_SG) && defined(XNET_FEATURE_INSECURE)
    BYTE            _abDhXNull[CBDHG1]; // Diffie-hellman X for insecure XbToSg
    BYTE            _abDhGXNull[CBDHG1];// Diffie-hellman g^X for insecure XbToSg
    BYTE            _abKeyNull[16];     // Session key for insecure XbToSg
#endif

    BYTE            _abKeyShaLan[16];   // LAN broadcast SHA auth key
    BYTE            _abKeyDesLan[24];   // LAN broadcast DES crypt key (3des)
    CKeyReg *       _pKeyReg;           // Vector of CKeyReg structures
    CSecReg *       _pSecReg;           // Vector of CSecReg structures
    CSecReg *       _pSecRegProbe;      // Next CSecReg to process in SecRegProbeTimer
    WORD            _cKeyReg;           // Number of registered key slots in use
    WORD            _wSecRegUniq;       // Number cycled to generate next secure IP address
    UINT            _cSecRegProbeNumer; // Probe timer numer (cfgSecRegMax per tick)
    UINT            _cSecRegProbeDenom; // Probe timer denom (ticks per CSecReg visit)
    LARGE_INTEGER   _liTime;            // Highest system time sent during key exchange

#ifdef XNET_FEATURE_ONLINE
    CXoBase *       _pXoBase;           // Pointer to CXoBase implemented by the Online code
#endif

#ifdef XNET_FEATURE_SG
    UINT            _uiLogonState;      // One of the XN_LOGON_* states
    CSecReg *       _pSecRegLogon;      // Pointer to CSecReg from IpLogon
    PRKEVENT        _pEventLogon;       // Pointer to cracked WSAEVENT from IpLogon
    XOUSERINFO      _axouserinfo[4];    // Vector of XOUSERINFO
    BYTE            _abXbToSgPulse[4];  // Changes to transmit on next XbToSg pulse
    DWORD           _dwSeqXbToSg;       // Sequence number of latest XbToSg pulse with changes
    DWORD           _dwSeqSgToXb;       // Sequence number to acknowledge in next XbToSg pulse
#endif

};

INLINE BYTE CXnIp::SecRegRexmitRetries(CSecReg * pSecReg)
{
#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINESERVER))
    {
        return(cfgKeyExXbToSgRexmitRetries);
    }
#endif

    return(cfgKeyExXbToXbRexmitRetries);
}

INLINE BYTE CXnIp::SecRegRexmitTimeoutInSeconds(CSecReg * pSecReg)
{
#ifdef XNET_FEATURE_SG
    if (pSecReg->TestFlags(SRF_ONLINESERVER))
    {
        return(cfgKeyExXbToSgRexmitTimeoutInSeconds);
    }
#endif

    return(cfgKeyExXbToXbRexmitTimeoutInSeconds);
}

// ---------------------------------------------------------------------------------------
// CSocket
// ---------------------------------------------------------------------------------------

class CRecvReq;
class CRecvBuf;
class CSendReq;
class CSendBuf;

class CSocket
{
    // Definitions -----------------------------------------------------------------------

    #define SOCKF_POOLALLOC             0x00000001      // [ANY] Socket allocated with pool allocator
    #define SOCKF_TCP                   0x00000002      // [ANY] Socket is CTcpSocket
    #define SOCKF_NOMORE_XMIT           0x00000004      // [ANY] No more transmission of data allowed
    #define SOCKF_NOMORE_RECV           0x00000008      // [ANY] No more receiving of data allowed
    #define SOCKF_BOUND                 0x00000010      // [ANY] Socket is bound to a local address
    #define SOCKF_CONNECTED             0x00000020      // [ANY] Socket is connected
    #define SOCKF_REVIVABLE             0x00000040      // [TCP] Socket is revivable
    #define SOCKF_CONNECT_SELECTED      0x00000080      // [TCP] Select has been called for connect event
    #define SOCKF_PERSIST               0x00000100      // [TCP] Socket is persisting
    #define SOCKF_FAST_RETRANSMIT       0x00000200      // [TCP] Socket is in fast retransmit mode
    #define SOCKF_EVENT_READ            0x00000400      // [ANY] FD_READ event
    #define SOCKF_EVENT_WRITE           0x00000800      // [ANY] FD_WRITE event
    #define SOCKF_EVENT_ACCEPT          0x00001000      // [ANY] FD_ACCEPT event
    #define SOCKF_EVENT_CONNECT         0x00002000      // [ANY] FD_CONNECT event
    #define SOCKF_EVENT_CLOSE           0x00004000      // [ANY] FD_CLOSE event
    #define SOCKF_EVENT_RESET           0x00008000      // [ANY] Socket has been reset
    #define SOCKF_OPT_BROADCAST         0x00010000      // [UDP] Broadcast is allowed
    #define SOCKF_OPT_NONBLOCKING       0x00020000      // [ANY] Nonblocking socket
    #define SOCKF_OPT_REUSEADDR         0x00040000      // [ANY] Address reuse is allowed
    #define SOCKF_OPT_EXCLUSIVEADDR     0x00080000      // [ANY] Address reuse prohibited
    #define SOCKF_OPT_NONAGLE           0x00100000      // [TCP] Don't buffer data
    #define SOCKF_OWNED                 0x00200000      // [ANY] Socket belongs to user (socket() & accept())
    #define SOCKF_CLOSED                0x00400000      // [ANY] Socket is closed but not freed yet
    #define SOCKF_LINGERING             0x00800000      // [ANY] Socket is lingering
    #define SOCKF_INSECURE              0x01000000      // [ANY] Socket is insecure

    #define SOCKF_EVENT_MASK            (SOCKF_EVENT_READ|SOCKF_EVENT_WRITE|SOCKF_EVENT_ACCEPT|SOCKF_EVENT_CONNECT|SOCKF_EVENT_CLOSE|SOCKF_EVENT_RESET)
    #define SOCKF_OPT_MASK              (SOCKF_OPT_NONBLOCKING|SOCKF_OPT_BROADCAST|SOCKF_OPT_REUSEADDR|SOCKF_OPT_EXCLUSIVEADDR|SOCKF_OPT_NONAGLE)

    #define SOCK_LOCK_ACTIVE            '+COS'
    #define SOCK_LOCK_BUSY              '*COS'
    #define SOCK_LOCK_CLOSED            '-cos'

    // External --------------------------------------------------------------------------

public:

    INLINE void         Init(CXnBase * pXnBase, BOOL fTcp, BOOL fPoolAlloc);
    INLINE void         Enqueue(LIST_ENTRY * pleHead);
    INLINE void         Dequeue();
    INLINE LIST_ENTRY * GetFlink();
    static CSocket *    Lock(SOCKET s);
    INLINE void         Unlock();
    INLINE void         SetClosed();
    INLINE BOOL         IsLingering();
    INLINE PRKEVENT     GetEvent();
    INLINE void         SignalEvent(DWORD dwFlags);
    INLINE BOOL         TestFlags(DWORD dwFlags);
    INLINE DWORD        GetFlags(DWORD dwMask = 0xFFFFFFFF);
    INLINE void         SetFlags(DWORD dwFlags);
    INLINE void         SetFlags(DWORD dwFlags, DWORD dwMask);
           void         SetFlagsAndOr(DWORD dwAnd, DWORD dwOr);
    INLINE void         ClearFlags(DWORD dwFlags);
    INLINE BOOL         IsTcp();
    INLINE BOOL         IsUdp();
    INLINE const char * TypeStr();
    INLINE BOOL         HasRecvReq();
    INLINE CRecvReq *   GetRecvReq();
    INLINE void         SetRecvReq(CRecvReq * pRecvReq);
    INLINE BOOL         HasRecvBuf();
    INLINE CRecvBuf *   GetRecvBufFirst();
    INLINE CRecvBuf *   GetRecvBufNext(CRecvBuf * pRecvBuf);
    INLINE CRecvBuf *   GetRecvBufLast();
    INLINE CRecvBuf *   GetRecvBufPrev(CRecvBuf * pRecvBuf);
    INLINE CRecvBuf *   DequeueRecvBuf();
    INLINE void         DequeueRecvBuf(CRecvBuf * pRecvBuf);
    INLINE void         EnqueueRecvBuf(CRecvBuf * pRecvBuf);
           void         InsertRecvBuf(CRecvBuf * pRecvBuf, CRecvBuf * pRecvBufPrev);
    INLINE BOOL         IsRecvBufFull();
    INLINE BOOL         IsRecvBufEmpty();
    INLINE BOOL         IsUdpRecvBufEmpty();
    INLINE BOOL         IsTcpRecvBufEmpty();
    INLINE UINT         GetCbRecvBuf();
    INLINE void         SetCbRecvBuf(UINT cb);
    INLINE BOOL         HasSendReq();
    INLINE CSendReq *   GetSendReq();
    INLINE void         SetSendReq(CSendReq * pSendReq);
    INLINE BOOL         HasSendBuf();
    INLINE CSendBuf *   GetSendBufFirst();
    INLINE CSendBuf *   GetSendBufNext(CSendBuf * pSendBuf);
    INLINE CSendBuf *   DequeueSendBuf();
    INLINE void         DequeueSendBuf(CSendBuf * pSendBuf);
    INLINE void         EnqueueSendBuf(CSendBuf * pSendBuf);
    INLINE BOOL         IsSendBufFull();
    INLINE BOOL         IsSendBufEmpty();
    INLINE UINT         GetCbSendBuf();
    INLINE void         SetCbSendBuf(UINT cb);
    INLINE UINT         IncCbSendBuf(UINT cb);
    INLINE UINT         DecCbSendBuf(UINT cb);
    INLINE NTSTATUS     GetStatus();
    INLINE void         SetStatus(NTSTATUS status);

    // Data ------------------------------------------------------------------------------

private:

    LIST_ENTRY          _le;
    LONG                _lLock;
    DWORD               _dwFlags;
    KEVENT              _Event;
    CRecvReq *          _pRecvReq;
    LIST_ENTRY          _leRecvBufs;
    UINT                _cbRecvBuf;
    CSendReq *          _pSendReq;
    LIST_ENTRY          _leSendBufs;
    UINT                _cbSendBuf;
    NTSTATUS            _status;

public:

    CIpAddr             _ipaDst;
    CIpPort             _ipportDst;
    CIpPort             _ipportSrc;
    CRouteEntry *       _prte;
    UINT                _uiSendTimeout;
    UINT                _uiRecvTimeout;
    UINT                _cbMaxSendBuf;
    UINT                _cbMaxRecvBuf;

#ifdef XNET_FEATURE_ASSERT
private:
    CXnBase *           _pXnBase;
public:
    INLINE CXnBase *    GetXnBase() { return(_pXnBase); }
#endif

};

// ---------------------------------------------------------------------------------------
// CSockReq
// ---------------------------------------------------------------------------------------

class CSockReq
{
    // Definitions -----------------------------------------------------------------------

    // We use the WSAOVERLAPPED structure to keep track of the status of an overlapped I/O
    // request. When the request is pending, the fields are interpreted as follows:
    //      Internal     - pointer to the I/O request structure
    //      InternalHigh - unused
    //      Offset       - unused
    //      OffsetHigh   - status code = NETERR_PENDING
    // After a request is completed, the fields are interpreted differently:
    //      Internal     - unused
    //      InternalHigh - I/O request completion flags
    //      Offset       - number of bytes actually transferred
    //      OffsetHigh   - status code != NETERR_PENDING

    #define _ioreq      Internal
    #define _ioflags    InternalHigh
    #define _ioxfercnt  Offset
    #define _iostatus   OffsetHigh

    // Data ------------------------------------------------------------------------------

public:

    PRKEVENT            _pEvent;
    WSAOVERLAPPED *     _pWsaOverlapped;

};

// ---------------------------------------------------------------------------------------
// CRecvReq
// ---------------------------------------------------------------------------------------

class CRecvReq : public CSockReq
{

    // External --------------------------------------------------------------------------

public:

    INLINE void SetFromAddrPort(CIpAddr ipa, CIpPort ipport)
    {
        if (fromaddr)
        {
            fromaddr->sin_addr.s_addr = ipa;
            fromaddr->sin_port = ipport;
        }
    }

    INLINE UINT GetCbBuf()
    {
        return(buflen);
    }

    INLINE BYTE * GetPbBuf()
    {
        return(buf);
    }

    // Data ------------------------------------------------------------------------------

public:

    DWORD               flags;
    DWORD *             bytesRecv;
    BYTE *              buf;
    UINT                buflen;
    struct sockaddr_in* fromaddr;

};

// ---------------------------------------------------------------------------------------
// CRecvBuf
// ---------------------------------------------------------------------------------------

class CRecvBuf
{
    friend class CSocket;

    // External --------------------------------------------------------------------------

public:

    INLINE void         Init(CXnBase * pXnBase);
    INLINE UINT         GetCbBuf();
    INLINE void         SetCbBuf(UINT cb);
    INLINE UINT         IncCbBuf(UINT cb);
    INLINE UINT         DecCbBuf(UINT cb);

    // Data ------------------------------------------------------------------------------

private:

    LIST_ENTRY          _le;
    UINT                _cbBuf;

#ifdef XNET_FEATURE_ASSERT
    CXnBase *           _pXnBase;
public:
    INLINE CXnBase *    GetXnBase() { return(_pXnBase); }
#endif

};

INLINE void CRecvBuf::Init(CXnBase * pXnBase)
{
    ICHECK_(pXnBase, SOCK, USER|UDPC|SDPC);
#ifdef XNET_FEATURE_ASSERT
    _pXnBase = pXnBase;
#endif
}

INLINE UINT CRecvBuf::GetCbBuf()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_cbBuf);
}

INLINE void CRecvBuf::SetCbBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cbBuf = cb;
}

INLINE UINT CRecvBuf::IncCbBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbBuf += cb);
}

INLINE UINT CRecvBuf::DecCbBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbBuf -= cb);
}

// ---------------------------------------------------------------------------------------
// CUdpRecvBuf
// ---------------------------------------------------------------------------------------

class CUdpRecvBuf : public CRecvBuf
{

    // Data ------------------------------------------------------------------------------

public:

    CIpAddr             fromaddr;
    CIpPort             fromport;
};

// ---------------------------------------------------------------------------------------
// CTcpRecvBuf
// ---------------------------------------------------------------------------------------

class CTcpRecvBuf : public CRecvBuf
{

    // Data ------------------------------------------------------------------------------

public:

    TCPSEQ              seqnext;
    WORD                dataoffset;
    BYTE                tcpflags;

};

// ---------------------------------------------------------------------------------------
// CSendReq
// ---------------------------------------------------------------------------------------

class CSendReq : public CSockReq
{

    // Data ------------------------------------------------------------------------------

public:

    WSABUF *            bufs;
    UINT                bufcnt;
    UINT                sendtotal;
    struct sockaddr_in* toaddr;

};

// ---------------------------------------------------------------------------------------
// CSendBuf
// ---------------------------------------------------------------------------------------

class CSendBuf : public CPacket
{
    friend class CSocket;

    // External --------------------------------------------------------------------------

public:

    INLINE void         Init(CSocket * pSocket, UINT cbData, UINT cRefs);
    INLINE void         AddRef();
    INLINE LONG         Release();
    INLINE BOOL         IsActive();
    INLINE UINT         GetCbBuf();
    INLINE void         SetCbBuf(UINT cb);
    INLINE UINT         DecCbBuf(UINT cb);
    INLINE CSocket *    GetSocket();

#ifdef XNET_FEATURE_ASSERT
    INLINE CXnBase *    GetXnBase()     { return(_pSocket->GetXnBase()); }
#endif

    // Data ------------------------------------------------------------------------------

private:

    LIST_ENTRY          _le;
    CSocket *           _pSocket;
    UINT                _cbBuf;
    LONG                _cRefs;

};

INLINE void CSendBuf::Init(CSocket * pSocket, UINT cbBuf, UINT cRefs)
{
    ICHECK_(pSocket->GetXnBase(), SOCK, USER|UDPC|SDPC);
    _pSocket    = pSocket;
    _cbBuf      = cbBuf;
    _cRefs      = cRefs;
}

INLINE void CSendBuf::AddRef()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cRefs += 1;
}

INLINE LONG CSendBuf::Release()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(--_cRefs);
}

INLINE BOOL CSendBuf::IsActive()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cRefs > 1);
}

INLINE UINT CSendBuf::GetCbBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbBuf);
}

INLINE void CSendBuf::SetCbBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cbBuf = cb;
}

INLINE UINT CSendBuf::DecCbBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cbBuf -= cb;
}

INLINE CSocket * CSendBuf::GetSocket()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pSocket);
}

// ---------------------------------------------------------------------------------------
// CTcpSendBuf
// ---------------------------------------------------------------------------------------

class CTcpSendBuf : public CSendBuf
{

    // Data ------------------------------------------------------------------------------

public:

    TCPSEQ              seq;            // starting sequence number for this TCP segment
    TCPSEQ              seqnext;        // the first seqno after this segment
    WORD                retries;        // number of times this segment has been rexmitted
    BYTE                tcpflags;       // TCP segment flags
    BYTE                tcphdrlen;      // TCP segment header length
    UINT                firstSendTime;  // the time this segment was first sent

};

// ---------------------------------------------------------------------------------------
// CSocket Inlines
// ---------------------------------------------------------------------------------------

INLINE void CSocket::Init(CXnBase * pXnBase, BOOL fTcp, BOOL fPoolAlloc)
{
    ICHECK_(pXnBase, SOCK, USER|UDPC|SDPC);

#ifdef XNET_FEATURE_ASSERT
    _pXnBase        = pXnBase;
#endif

    _lLock          = SOCK_LOCK_ACTIVE;
    _dwFlags        = (fPoolAlloc ? SOCKF_POOLALLOC : 0) | (fTcp ? SOCKF_TCP : 0);
    _cbMaxSendBuf   = pXnBase->cfgSockDefaultSendBufsizeInK * 1024;
    _cbMaxRecvBuf   = pXnBase->cfgSockDefaultRecvBufsizeInK * 1024;

    InitializeListHead(&_le);
    InitializeListHead(&_leRecvBufs);
    InitializeListHead(&_leSendBufs);
    EvtInit(EvtRef(_Event));
}

INLINE void CSocket::Enqueue(LIST_ENTRY * pleHead)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListNull(&_le));
    AssertList(pleHead);
    InsertTailList(pleHead, &_le);
}

INLINE void CSocket::Dequeue()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListNull(&_le));
    RemoveEntryList(&_le);
}

INLINE void CSocket::Unlock()
{
    ICHECK_(GetXnBase(), SOCK, USER);
    Assert(_lLock == SOCK_LOCK_BUSY);
    _lLock = SOCK_LOCK_ACTIVE;
}

INLINE void CSocket::SetClosed()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _lLock = SOCK_LOCK_CLOSED;
}

INLINE BOOL CSocket::IsLingering()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(TestFlags(SOCKF_LINGERING));
}

INLINE PRKEVENT CSocket::GetEvent()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(EvtRef(_Event));
}

INLINE void CSocket::SignalEvent(DWORD dwFlags)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);

    if (TestFlags(dwFlags))
    {
        EvtSet(EvtRef(_Event), EVENT_INCREMENT);
    }
}

INLINE DWORD CSocket::GetFlags(DWORD dwMask)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_dwFlags & dwMask);
}

INLINE void CSocket::SetFlags(DWORD dwFlags)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    SetFlagsAndOr(0xFFFFFFFF, dwFlags);
}

INLINE void CSocket::SetFlags(DWORD dwFlags, DWORD dwMask)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    SetFlagsAndOr(~dwMask, dwFlags & dwMask);
}

INLINE void CSocket::ClearFlags(DWORD dwFlags)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    SetFlagsAndOr(~dwFlags, 0);
}

INLINE BOOL CSocket::TestFlags(DWORD dwFlags)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(!!(_dwFlags & dwFlags));
}

INLINE BOOL CSocket::IsTcp()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(TestFlags(SOCKF_TCP));
}

INLINE BOOL CSocket::IsUdp()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(!IsTcp());
}

INLINE const char * CSocket::TypeStr()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(IsUdp() ? "UDP" : "TCP");
}

INLINE BOOL CSocket::HasRecvReq()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pRecvReq != NULL);
}

INLINE CRecvReq * CSocket::GetRecvReq()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pRecvReq);
}

INLINE void CSocket::SetRecvReq(CRecvReq * pRecvReq)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _pRecvReq = pRecvReq;
}

INLINE BOOL CSocket::HasRecvBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(!IsListEmpty(&_leRecvBufs));
}

INLINE CRecvBuf * CSocket::GetRecvBufNext(CRecvBuf * pRecvBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);

    if (pRecvBuf == NULL)
        pRecvBuf = (CRecvBuf *)_leRecvBufs.Flink;
    else
        pRecvBuf = (CRecvBuf *)pRecvBuf->_le.Flink;

    return(pRecvBuf == (CRecvBuf *)&_leRecvBufs ? NULL : pRecvBuf);
}

INLINE CRecvBuf * CSocket::GetRecvBufFirst()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetRecvBufNext(NULL));
}

INLINE CRecvBuf * CSocket::GetRecvBufPrev(CRecvBuf * pRecvBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(offsetof(CRecvBuf, _le) == 0);

    if (pRecvBuf == NULL)
        pRecvBuf = (CRecvBuf *)_leRecvBufs.Blink;
    else
        pRecvBuf = (CRecvBuf *)pRecvBuf->_le.Blink;

    return(pRecvBuf == (CRecvBuf *)&_leRecvBufs ? NULL : pRecvBuf);
}

INLINE CRecvBuf * CSocket::GetRecvBufLast()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetRecvBufPrev(NULL));
}

INLINE CRecvBuf * CSocket::DequeueRecvBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListEmpty(&_leRecvBufs));
    Assert(offsetof(CRecvBuf, _le) == 0);
    CRecvBuf * pRecvBuf = (CRecvBuf *)RemoveHeadList(&_leRecvBufs);
    return(pRecvBuf);
}

INLINE void CSocket::DequeueRecvBuf(CRecvBuf * pRecvBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListEmpty(&_leRecvBufs));
    AssertListEntry(&_leRecvBufs, &pRecvBuf->_le);
    RemoveEntryList(&pRecvBuf->_le);
}

INLINE void CSocket::EnqueueRecvBuf(CRecvBuf * pRecvBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(pRecvBuf != NULL);
    AssertList(&_leRecvBufs);
    InsertTailList(&_leRecvBufs, &pRecvBuf->_le);
}

INLINE BOOL CSocket::IsRecvBufFull()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbRecvBuf >= _cbMaxRecvBuf);
}

INLINE BOOL CSocket::IsUdpRecvBufEmpty()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    Assert(IsUdp());
    return(IsListEmpty(&_leRecvBufs));
}

INLINE BOOL CSocket::IsTcpRecvBufEmpty()
{ 
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    Assert(IsTcp());
    return(_cbRecvBuf == 0);
}

INLINE BOOL CSocket::IsRecvBufEmpty()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(IsUdp() ? IsUdpRecvBufEmpty() : IsTcpRecvBufEmpty());
}

INLINE UINT CSocket::GetCbRecvBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbRecvBuf);
}

INLINE void CSocket::SetCbRecvBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cbRecvBuf = cb;
}

INLINE BOOL CSocket::HasSendReq()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_pSendReq != NULL);
}

INLINE CSendReq * CSocket::GetSendReq()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pSendReq);
}

INLINE void CSocket::SetSendReq(CSendReq * pSendReq)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _pSendReq = pSendReq;
}

INLINE BOOL CSocket::HasSendBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(!IsListEmpty(&_leSendBufs));
}

INLINE CSendBuf * CSocket::GetSendBufNext(CSendBuf * pSendBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    LIST_ENTRY * ple = (pSendBuf == NULL) ? _leSendBufs.Flink : pSendBuf->_le.Flink;
    return(ple == &_leSendBufs ? NULL : (CSendBuf *)((BYTE *)ple - offsetof(CSendBuf, _le)));
}

INLINE CSendBuf * CSocket::GetSendBufFirst()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetSendBufNext(NULL));
}

INLINE CSendBuf * CSocket::DequeueSendBuf()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListEmpty(&_leSendBufs));
    AssertList(&_leSendBufs);
    LIST_ENTRY * ple = RemoveHeadList(&_leSendBufs);
    return((CSendBuf *)((BYTE *)ple - offsetof(CSendBuf, _le)));
}

INLINE void CSocket::DequeueSendBuf(CSendBuf * pSendBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(!IsListEmpty(&_leSendBufs));
    AssertListEntry(&_leSendBufs, &pSendBuf->_le);
    RemoveEntryList(&pSendBuf->_le);
}

INLINE void CSocket::EnqueueSendBuf(CSendBuf * pSendBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(pSendBuf != NULL);
    AssertList(&_leSendBufs);
    InsertTailList(&_leSendBufs, &pSendBuf->_le);
}

INLINE BOOL CSocket::IsSendBufFull()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_cbSendBuf >= _cbMaxSendBuf);
}

INLINE BOOL CSocket::IsSendBufEmpty()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(IsListEmpty(&_leSendBufs));
}

INLINE UINT CSocket::GetCbSendBuf()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_cbSendBuf);
}

INLINE void CSocket::SetCbSendBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _cbSendBuf = cb;
}

INLINE UINT CSocket::IncCbSendBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbSendBuf += cb);
}

INLINE UINT CSocket::DecCbSendBuf(UINT cb)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_cbSendBuf -= cb);
}

INLINE NTSTATUS CSocket::GetStatus()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_status);
}

INLINE void CSocket::SetStatus(NTSTATUS status)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _status = status;
}

INLINE LIST_ENTRY * CSocket::GetFlink()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_le.Flink);
}

// ---------------------------------------------------------------------------------------
// CTcpSocket
// ---------------------------------------------------------------------------------------

class CTcpSocket : public CSocket
{
    // Definitions -----------------------------------------------------------------------

    #define TCPST_CLOSED            0
    #define TCPST_LISTEN            1
    #define TCPST_SYN_SENT          2
    #define TCPST_SYN_RECEIVED      3
    #define TCPST_ESTABLISHED       4
    #define TCPST_FIN_WAIT_1        5
    #define TCPST_FIN_WAIT_2        6
    #define TCPST_CLOSING           7
    #define TCPST_TIME_WAIT         8
    #define TCPST_CLOSE_WAIT        9
    #define TCPST_LAST_ACK          10
    #define TCPST_MAX               TCPST_LAST_ACK

    #define UDP_DEFAULT_MSS         ESP_MSS(576, sizeof(CUdpHdr))
    #define UDP_MAXIMUM_MSS         ESP_MSS(ENET_DATA_MAXSIZE, sizeof(CUdpHdr))

    #define TCP_MINIMUM_MSS         (MAXIPHDRLEN+MAXTCPHDRLEN+8-sizeof(CIpHdr)-sizeof(CTcpHdr))
    #define TCP_DEFAULT_MSS         ESP_MSS(576, sizeof(CTcpHdr))
    #define TCP_MAXIMUM_MSS         ESP_MSS(ENET_DATA_MAXSIZE, sizeof(CTcpHdr))
    #define MAX_TCP_WNDSIZE         0xFFFF

    #define MIN_TEMP_PORT           1024
    #define MAX_TEMP_PORT           4998
    #define TEMP_PORT_COUNT         (MAX_TEMP_PORT-MIN_TEMP_PORT+1)

    #define SEQ_LT(a, b)            ((INT) ((a) - (b)) < 0)
    #define SEQ_LE(a, b)            ((INT) ((a) - (b)) <= 0)
    #define SEQ_GT(a, b)            ((INT) ((a) - (b)) > 0)
    #define SEQ_GE(a, b)            ((INT) ((a) - (b)) >= 0)

    #define SRTT_SHIFT              3
    #define SRTT_SCALE              (1 << SRTT_SHIFT)
    #define RTTVAR_SHIFT            2
    #define RTTVAR_SCALE            (1 << RTTVAR_SHIFT)

    // External --------------------------------------------------------------------------

public:

    INLINE void                 TcpInit(CXnBase * pXnBase);
    INLINE static CTcpSocket *  TimerToSocket(CTimer * pt);
    INLINE BOOL                 HasParent();
    INLINE CTcpSocket *         GetParent();
    INLINE BOOL                 IsChild();
           void                 EnqueueChild(CTcpSocket * pTcpSocketChild);
           void                 DequeueChild(CTcpSocket * pTcpSocketChild);
    INLINE CTcpSocket *         GetNextChild(CTcpSocket * pTcpSocketPrev);
    INLINE CTcpSocket *         GetFirstChild();
    INLINE CTcpSocket *         GetPrevChild(CTcpSocket * pTcpSocketNext);
    INLINE CTcpSocket *         GetLastChild();
    INLINE BOOL                 HasConnectedChild();
           CTcpSocket *         GetConnectedChild();
           CTcpSocket *         DequeueConnectedChild();
    INLINE UINT                 GetBacklog();
    INLINE UINT                 GetBacklogMax();
    INLINE void                 SetBacklogMax(UINT cBacklogMax);
    INLINE UINT                 IncDupAcks();
    INLINE void                 SetDupAcks(UINT cDupAcks);
    INLINE UINT                 GetState();
    INLINE void                 SetState(UINT uiState);
    INLINE BOOL                 IsIdleState();
    INLINE BOOL                 IsListenState();
    INLINE BOOL                 IsSyncState();
    INLINE BOOL                 IsFinReceived();
    INLINE BOOL                 IsFinSent();
    INLINE void                 IncrementCongestionWindow(UINT uiIncr);
    INLINE void                 ResetSlowStartThresh();
    INLINE void                 GetLinger(LINGER * pLinger);
    INLINE void                 SetLinger(LINGER * pLinger);
    INLINE BOOL                 GetLingerOnOff();
    INLINE void                 SetLingerOnOff(BOOL fOn);
    INLINE UINT                 GetLingerTimeout();
    INLINE CTimer *             GetTimer();
    INLINE CTcpSendBuf *        GetSendBufPtr();
    INLINE void                 SetSendBufPtr(CTcpSendBuf * pTcpSendBuf);

    // Data ------------------------------------------------------------------------------

private:

    CTcpSocket *            _pTcpSocketParent;
    LIST_ENTRY              _le;
    CTimer                  _timer;
    BYTE                    _bState;
    BYTE                    _bBacklog;
    BYTE                    _bBacklogMax;
    BYTE                    _bDupAcks;
    LINGER                  _Linger;
    CTcpSendBuf *           _pTcpSendBufPtr;

public:

    //
    // TCP timers
    //  delayedAcks is the number of delay-acked segments;
    //
    // NOTE: Be careful when you add or remove fields between
    // delayedAcks and rtt_tick (inclusive). See comments in
    // function TcpReset().
    //
    UINT delayedAcks;
    UINT dwTickSyn;        // connection establishment timer
    UINT dwTickXmit;       // retransmission & persist timer
    UINT dwTickWait;       // TIME-WAIT & linger timer

    //
    // Send/receive state information
    //
    TCPSEQ snd_isn;         // initial send sequence number
    TCPSEQ snd_una;         // sent but not yet acknowledged
    TCPSEQ snd_nxt;         // buffered but not yet sent
    TCPSEQ snd_end;         // not yet buffered
    TCPSEQ snd_wl1;         // seq and ack numbers of the last ACK 
    TCPSEQ snd_wl2;         // that was used to update snd_wnd
    UINT snd_wnd;           // send window
    UINT snd_cwnd;          // congestion window
    UINT snd_ssthresh;      // slow-start threshold
    UINT snd_mss;           // send MSS
    TCPSEQ rcv_isn;         // initial receive sequence number
    TCPSEQ rcv_nxt;         // next expected receive sequence number
    UINT rcv_wnd;           // receive window
    UINT rcv_mss;           // receive MSS
    UINT rcv_swsthresh;     // receive SWS avoidance threshold

    //
    // Round-trip time measurements
    //
    INT srtt_8;             // smoothed round-trip time, in 1/8 ticks
    INT rttvar_4;           // round-trip time variance, in 1/4 ticks
    UINT RTO;               // retransmission timeout, in ticks
    TCPSEQ rtt_seq;         // the seqno of the segment being timed
    UINT rtt_tick;          // when the timed segment was sent

};

INLINE CTcpSocket * CTcpSocket::TimerToSocket(CTimer * pt)
{
    return((CTcpSocket *)((BYTE *)pt - offsetof(CTcpSocket, _timer)));
}

INLINE BOOL CTcpSocket::HasParent()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pTcpSocketParent != NULL);
}

INLINE CTcpSocket * CTcpSocket::GetParent()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(_pTcpSocketParent != NULL);
    return(_pTcpSocketParent);
}

INLINE BOOL CTcpSocket::IsChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pTcpSocketParent != NULL);
}

INLINE CTcpSocket * CTcpSocket::GetNextChild(CTcpSocket * pTcpSocketPrev)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    LIST_ENTRY * ple = pTcpSocketPrev ? pTcpSocketPrev->_le.Flink : _le.Flink;
    return(ple == &_le ? NULL : (CTcpSocket *)((BYTE *)ple - offsetof(CTcpSocket, _le)));
}

INLINE CTcpSocket * CTcpSocket::GetFirstChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetNextChild(NULL));
}
    
INLINE CTcpSocket * CTcpSocket::GetPrevChild(CTcpSocket * pTcpSocketNext)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    LIST_ENTRY * ple = pTcpSocketNext ? pTcpSocketNext->_le.Blink : _le.Blink;
    return(ple == &_le ? NULL : (CTcpSocket *)((BYTE *)ple - offsetof(CTcpSocket, _le)));
}

INLINE CTcpSocket * CTcpSocket::GetLastChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetPrevChild(NULL));
}

INLINE BOOL CTcpSocket::HasConnectedChild()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(GetConnectedChild() != NULL);
}

INLINE UINT CTcpSocket::GetBacklog()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_bBacklog);
}

INLINE UINT CTcpSocket::GetBacklogMax()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_bBacklogMax);
}

INLINE void CTcpSocket::SetBacklogMax(UINT cBacklogMax)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _bBacklogMax = (BYTE)cBacklogMax;
}

INLINE UINT CTcpSocket::IncDupAcks()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(++_bDupAcks);
}

INLINE void CTcpSocket::SetDupAcks(UINT cDupAcks)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(cDupAcks <= 255);
    _bDupAcks = (BYTE)cDupAcks;
}

INLINE UINT CTcpSocket::GetState()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_bState);
}

INLINE void CTcpSocket::SetState(UINT uiState)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    Assert(uiState <= TCPST_MAX);
    _bState = (BYTE)uiState;
}

INLINE BOOL CTcpSocket::IsIdleState()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    // CLOSED or LISTEN
    return(_bState <= TCPST_LISTEN);
}

INLINE BOOL CTcpSocket::IsListenState()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(_bState == TCPST_LISTEN);
}

INLINE BOOL CTcpSocket::IsSyncState()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    // ESTABLISHED, FIN_WAIT_1, FIN_WAIT_2, CLOSING, TIME_WAIT, CLOSE_WAIT, LAST_ACK
    return(_bState >= TCPST_ESTABLISHED);
}

INLINE BOOL CTcpSocket::IsFinReceived()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    // CLOSING, TIME_WAIT, CLOSE_WAIT, LAST_ACK
    return(_bState >= TCPST_CLOSING);
}

INLINE BOOL CTcpSocket::IsFinSent()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    // FIN_WAIT_1, FIND_WAIT_2, CLOSING, TIME_WAIT, LAST_ACK
    return(_bState >= TCPST_FIN_WAIT_1 && _bState != TCPST_CLOSE_WAIT);
}

INLINE void CTcpSocket::IncrementCongestionWindow(UINT uiIncr)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    snd_cwnd += uiIncr;
    if (snd_cwnd > MAX_TCP_WNDSIZE)
        snd_cwnd = MAX_TCP_WNDSIZE;
}

INLINE void CTcpSocket::ResetSlowStartThresh()
{
    snd_ssthresh = (snd_nxt - snd_una) >> 1;
    if (snd_ssthresh < 2*snd_mss)
        snd_ssthresh = 2*snd_mss;
}

INLINE void CTcpSocket::GetLinger(LINGER * pLinger)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    *pLinger = _Linger;
}

INLINE void CTcpSocket::SetLinger(LINGER * pLinger)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _Linger = *pLinger;
}

INLINE BOOL CTcpSocket::GetLingerOnOff()
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);
    return(!!_Linger.l_onoff);
}

INLINE void CTcpSocket::SetLingerOnOff(BOOL fOn)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _Linger.l_onoff = !!fOn;
}

INLINE UINT CTcpSocket::GetLingerTimeout()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_Linger.l_linger);
}

INLINE CTimer * CTcpSocket::GetTimer()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(&_timer);
}

INLINE CTcpSendBuf * CTcpSocket::GetSendBufPtr()
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    return(_pTcpSendBufPtr);
}

INLINE void CTcpSocket::SetSendBufPtr(CTcpSendBuf * pTcpSendBuf)
{
    ICHECK_(GetXnBase(), SOCK, UDPC|SDPC);
    _pTcpSendBufPtr = pTcpSendBuf;
}

//
// Socket address length
//
#define SOCKADDRLEN ((INT) sizeof(SOCKADDR_IN))

//------------------------------------------------------------------------
// Prolog and epilog code for Winsock API functions
//------------------------------------------------------------------------

#if DBG

ExternTag(sock);

#define WinsockApiProlog_(_apiname, _result) \
        static const CHAR* fname_ = #_apiname; \
        INT err = (!this || SockGetRefs() <= 0) ? WSANOTINITIALISED : NO_ERROR; \
        WinsockApiCheckError_(_result) \
        TCHECK(USER); \

#define WinsockApiPrologLight_(_apiname) \
        static const CHAR* fname_ = #_apiname; \
        TCHECK(USER); \

#define WinsockApiPrologTrivial_(_apiname) \
        static const CHAR* fname_ = #_apiname; \

#define WinsockApiWarnError_(_err) { \
            if ((_err) != WSA_IO_PENDING && \
                (_err) != WSA_IO_INCOMPLETE && \
                (_err) != WSAETIMEDOUT && \
                (_err) != WSAEWOULDBLOCK) { \
                TraceSz2(Warning, "%s failed: %d", fname_, _err); \
            } \
        }

#define WinsockApiParamCheck_(_exp) { \
            if (!(_exp)) { \
                TraceSz2(Warning, "%s: %s", fname_, #_exp); \
                Rip("bad winsock parameters\n"); \
            } \
        }

#else // !DBG

#define WinsockApiProlog_(_apiname, _result) \
        INT err = (!this || SockGetRefs() <= 0) ? WSANOTINITIALISED : NO_ERROR; \
        WinsockApiCheckError_(_result) \
        TCHECK(USER); \

#define WinsockApiPrologLight_(_apiname)
#define WinsockApiPrologTrivial_(_apiname)
#define WinsockApiWarnError_(_err)
#define WinsockApiParamCheck_(_exp)

#endif // !DBG

#define WinsockApiReturnError_(_err, _result) { \
            WinsockApiWarnError_(_err); \
            SetLastError(_err); \
            return (_result); \
        }

#define WinsockApiCheckError_(_result) { \
            if (err != NO_ERROR) { \
                WinsockApiReturnError_(err, _result); \
            } \
        }

#define WinsockApiPrologSockLock_(_apiname, _result) \
        WinsockApiProlog_(_apiname, _result); \
        CSocket * pSocket = CSocket::Lock(s); \
        if (!pSocket) return (_result)

#define WinsockApiExitSockUnlock_(_resultOk, _resultErr) \
        exit: pSocket->Unlock(); \
        WinsockApiCheckError_(_resultErr); \
        return (_resultOk)

#define WinsockApiGotoExit_(_err) \
        { err = (_err); goto exit; }

#define MapNtStatusToWinsockError_(_err) \
        ((_err) = NT_SUCCESS(_err) ? NO_ERROR : RtlNtStatusToDosError(_err))

#define XNetInitCheck(_fname, _err) \
    if (this == NULL) \
    { \
        TraceSz1(Warning, "%s: You must call XNetStartup or WSAStartup before calling this function.", #_fname); \
        return(_err); \
    } \

#ifdef XNET_FEATURE_ASSERT
    #define XNetParamCheck(_fname, _pcheck, _err) \
        if (!(_pcheck)) \
        { \
            TraceSz2(Warning, "%s: %s", #_fname, #_pcheck); \
            Rip("bad XNet parameters\n"); \
            return(_err); \
        }
    #define XNetEnter(_fname, _pcheck) \
        const char * __pszFName__ = #_fname; \
        XNetInitCheck(_fname, WSANOTINITIALISED); \
        XNetParamCheck(_fname, _pcheck, WSAEFAULT)
    #define XNetLeave(_err) \
        if ((_err) != 0) { TraceSz2(Warning, "%s failed: %d", __pszFName__, (_err)); }
#else
    #define XNetParamCheck(_fname, _pcheck, _err)
    #define XNetEnter(fname, pcheck) XNetInitCheck(fname, WSANOTINITIALISED) 
    #define XNetLeave(_err)
#endif

//
// Stop TCP fast retransmit/recovery mode
//
INLINE VOID TcpStopFastRexmitMode(CTcpSocket* pTcpSocket) {
    pTcpSocket->ClearFlags(SOCKF_FAST_RETRANSMIT);
    pTcpSocket->SetDupAcks(0);
}

// ---------------------------------------------------------------------------------------
// CXnSock
// ---------------------------------------------------------------------------------------

class NOVTABLE CXnSock : public CXnIp
{
    friend void CTcpSocket::TcpInit(CXnBase * pXnBase);
    friend void CXnIp::UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    friend void CXnIp::TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen);
    friend void CXnIp::SockReset(CIpAddr ipa);

    // Definitions -----------------------------------------------------------------------

    //
    // Information about sockets that was passed to the select calls
    //
    typedef struct _SELECTINFO {
        SOCKET s;
        fd_set* fdset;
        INT eventMasks;
        CSocket* pSocket;
        INT pSocketMasks;
    } SELECTINFO;

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnSock)

    NTSTATUS            SockInit(XNetInitParams * pxnip);
    void                SockStart()     { IpStart(); }
    void                SockStop()      { IpStop(); }
    void                SockTerm();

    LONG                XNetGetRefs()   { return(_cRefs); }
    LONG                XNetAddRef()    { return(++_cRefs); }
    LONG                XNetRelease()   { return(--_cRefs); }

    LONG                SockGetRefs()   { return(_cRefsSock); }
    LONG                SockAddRef()    { return(++_cRefsSock); }
    LONG                SockRelease()   { return(--_cRefsSock); }
    void                SockShutdown();

    #undef  XNETAPI
    #define XNETAPI(ret, fname, arglist, paramlist) ret fname arglist;
    #undef  XNETAPI_
    #define XNETAPI_(ret, fname, arglist, paramlist)
    #undef  XNETAPIV
    #define XNETAPIV(ret, fname, arglist, paramlist)

    SOCKAPILIST()

    // Internal --------------------------------------------------------------------------

private:

    VIRTUAL void        UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen);
    VIRTUAL void        TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen);
    VIRTUAL void        SockReset(CIpAddr ipa);

    INLINE CSocket *    GetFirstSocket();
    INLINE CSocket *    GetNextSocket(CSocket * pSocketPrev);
    CSocket *           SockAlloc(BOOL fTcp, BOOL fPoolAlloc);
    NTSTATUS            SockClose(CSocket * pSocket, BOOL fForce);
    void                SockCleanup(CSocket * pSocket);
    void                SockFree(CSocket * pSocket);
    CSocket *           SockFindMatch(CIpPort toport, CIpAddr fromaddr, CIpPort fromport, BYTE type);
    NTSTATUS            SockUpdateBufferSize(CSocket* pSocket, INT sendBufsize, INT recvBufsize);
    NTSTATUS            SockBind(CSocket* pSocket, CIpPort ipportBind);
    NTSTATUS            SockWaitForEvent(CSocket* pSocket, INT eventMask, UINT timeout);
    INT                 SockCheckSelectEvents(CSocket * pSocket, INT eventMasks, INT setwait);
    void                SockReqComplete(CSocket * pSocket, CSockReq * pSockReq, NTSTATUS status);
    NTSTATUS            RecvReqEnqueue(CSocket * pSocket, CRecvReq * pRecvReq);
    NTSTATUS            SockQueueOverlappedSend(CSocket * pSocket, CSendReq * sendreq);
    NTSTATUS            SockRead(CSocket * pSocket, CRecvReq * recvreq);
    INT                 SockLockSelectSockets(fd_set* fdset, SELECTINFO* selinfo, INT offset, INT eventMasks);
    NTSTATUS            SockSend(CSocket* pSocket, CSendReq* sendreq);
    void                SockInsertToList(CSocket * pSocket);
    void                SockReleaseSendBuf(CSendBuf * pSendBuf);
    void                SockFlushRecvBuffers(CSocket* pSocket);
    VOID                TcpDisassociateChild(CTcpSocket * childTcp);
    VOID                TcpTrimBacklog(CTcpSocket* pTcpSocket, UINT cBacklogMax);
    BOOL                TcpClose(CTcpSocket* pTcpSocket, BOOL forceful);
    NTSTATUS            TcpShutdown(CTcpSocket* pTcpSocket, DWORD dwFlags, BOOL apicall);
    CTcpSocket *        TcpReset(CTcpSocket* pTcpSocket, NTSTATUS status);
    CTcpSocket *        TcpCloneChild(CTcpSocket * pTcpSocket);
    NTSTATUS            TcpListen(CTcpSocket* pTcpSocket, INT backlog);
    NTSTATUS            TcpConnect(CTcpSocket* pTcpSocket, CIpAddr dstaddr, CIpPort dstport, BOOL synAck);
    void                TcpSetState(CTcpSocket* pTcpSocket, BYTE state, const CHAR* caller);
    void                TcpSlideRecvWindow(CTcpSocket* pTcpSocket, UINT datalen);
    UINT                TcpCopyRecvData(CTcpSocket* pTcpSocket, CRecvReq* recvreq);
    NTSTATUS            TcpRead(CTcpSocket* pTcpSocket, CRecvReq* recvreq);
    void                TcpUpdateRTO(CTcpSocket* pTcpSocket);
    BOOL                TcpUpdatePersistFlag(CTcpSocket* pTcpSocket);
    void                TcpUpdateSndUna(CTcpSocket* pTcpSocket, TCPSEQ ack);
    BOOL                TcpValidateSeqs(CTcpSocket* pTcpSocket, TCPSEQ oldseq0, TCPSEQ oldseq1, TCPSEQ* newseq0, TCPSEQ* newseq1);
    BOOL                TcpVerifyRecvBuf(CTcpSocket* pTcpSocket);
    UINT                TcpCorrectMisorderedSegments(CTcpSocket* pTcpSocket, TCPSEQ seq, CTcpRecvBuf * recvbuf);
    void                TcpBufferMisorderedSegment(CTcpSocket* pTcpSocket, UINT seq, CTcpRecvBuf * recvbuf);
    UINT                TcpCompletePendingRecvReqFast(CTcpSocket* pTcpSocket, BYTE tcpflags, BYTE * pbData, UINT cbData);
    CTcpSocket *        TcpRecvData(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, BYTE * pbData, UINT cbData);
    void                TcpProcessFIN(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, TCPSEQ seqfin);
    BOOL                TcpParseOptions(CTcpSocket * pTcpSocket, CTcpHdr * tcphdr, struct _TcpOptions * opts);
    BOOL                TcpAcceptConnReqPassive(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr, CIpAddr ipaSrc);
    void                TcpConnectionEstablished(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr);
    BOOL                TcpAcceptConnReqActive(CTcpSocket* pTcpSocket, CTcpHdr* tcphdr);
    void                TcpSendSegment(CTcpSocket* pTcpSocket, CTcpSendBuf* sendbuf, BOOL fRetransmit);
    BOOL                TcpStartOutput(CTcpSocket* pTcpSocket);
    CTcpSocket *        TcpXmitTimeout(CTcpSocket* pTcpSocket);
    void                TcpDoFastRexmit(CTcpSocket* pTcpSocket);
    void                TcpQueueSegment(CTcpSocket* pTcpSocket, CPacket* pkt, BYTE tcpflags, UINT datalen);
    NTSTATUS            TcpSend(CTcpSocket* pTcpSocket, CSendReq * sendreq, UINT uiFlags);
    TCPSEQ              TcpSndNxt(CTcpSocket* pTcpSocket);
    VOID                TcpResetPeer(CTcpSocket* pTcpSocket);
    void                TcpEmitRST(CTcpSocket * pTcpSocket, CIpAddr ipaDst, CIpPort ipportDst, CIpPort ipportSrc, TCPSEQ seq, TCPSEQ ack, BYTE flags);
    NTSTATUS            TcpEmitSYN(CTcpSocket* pTcpSocket, BOOL synAck);
    void                TcpEmitACK(CTcpSocket* pTcpSocket);
    NTSTATUS            TcpEmitFIN(CTcpSocket * pTcpSocket);
    void                NeedSendACKNow(CTcpSocket * pTcpSocket);
    void                TcpTimer(CTimer * pt);
    CPacket *           TcpPacketAlloc(CTcpSocket * pTcpSocket, CIpAddr ipaDst, UINT uiFlags, UINT cbDat = 0, UINT cbPkt = 0, PFNPKTFREE pfn = NULL);
    void                TcpPacketFree(CPacket * ppkt);
    NTSTATUS            UdpConnect(CSocket* pSocket, CIpAddr dstaddr, CIpPort dstport);
    void                UdpRecvData(CSocket* pSocket, CIpAddr fromaddr, CIpPort fromport, BYTE * pbData, UINT cbData);
    NTSTATUS            UdpRead(CSocket* pSocket, CRecvReq* recvreq);
    NTSTATUS            UdpSend(CSocket* pSocket, CSendReq* sendreq, UINT uiFlags);
    void                UdpPacketFree(CPacket * ppkt);
    NTSTATUS            UdpShutdown(CSocket* pSocket, DWORD dwFlags);

    // Data ------------------------------------------------------------------------------

private:

    LONG                _cRefs;
    LONG                _cRefsSock;
    ULONG               _cSockets;
    LIST_ENTRY          _leSockets;
    LIST_ENTRY          _leDeadSockets;
    UINT                _ipportTempNext;

};

INLINE CSocket * CXnSock::GetNextSocket(CSocket * pSocketPrev)
{
    ICHECK(SOCK, UDPC|SDPC);
    LIST_ENTRY * ple = pSocketPrev ? pSocketPrev->GetFlink() : _leSockets.Flink;
    return(ple == &_leSockets ? NULL : (CSocket *)ple);
}

INLINE CSocket * CXnSock::GetFirstSocket()
{
    ICHECK(SOCK, UDPC|SDPC);
    return(GetNextSocket(NULL));
}

// ---------------------------------------------------------------------------------------
// CTcpSocket Inlines (Requires CSocket Declared)
// ---------------------------------------------------------------------------------------

INLINE void CTcpSocket::TcpInit(CXnBase * pXnBase)
{
    ICHECK_(pXnBase, SOCK, USER|UDPC|SDPC);

    _Linger.l_linger = pXnBase->cfgSockLingerTimeoutInSeconds;
    _Linger.l_onoff  = FALSE;
    _pTcpSendBufPtr  = NULL;
    snd_mss          = TCP_DEFAULT_MSS;
    rcv_mss          = TCP_DEFAULT_MSS;
    snd_cwnd         = MAX_TCP_WNDSIZE;
    snd_ssthresh     = MAX_TCP_WNDSIZE;
    srtt_8           = 0;
    rttvar_4         = (UINT)(pXnBase->cfgSockRexmitTimeoutInSeconds * TICKS_PER_SECOND);
    RTO              = rttvar_4;

    Assert(IsListNull(&_le) || IsListEmpty(&_le));
    InitializeListHead(&_le);

    Assert(!_timer.IsActive());
    _timer.Init((PFNTIMER)CXnSock::TcpTimer);
}

// ---------------------------------------------------------------------------------------
// CXn
// ---------------------------------------------------------------------------------------

class NOVTABLE CXn : public CXnSock
{

public:

    // External --------------------------------------------------------------------------

    #undef  XNETAPI
    #define XNETAPI(ret, fname, arglist, paramlist) ret fname arglist;
    #undef  XNETAPI_
    #define XNETAPI_(ret, fname, arglist, paramlist)
    #undef  XNETAPIV
    #define XNETAPIV(ret, fname, arglist, paramlist)

    XNETAPILIST()

};

#ifdef XNET_FEATURE_XBOX
extern CXn * g_pXn;
#define GetXn()      g_pXn
#define GetXnRef()  &g_pXn
#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_VIRTUAL
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_VIRTUAL

INLINE void CXnBase::SecRegProbe() { ((CXnIp *)this)->SecRegProbe(); }
INLINE void CXnBase::NicTimer() { ((CXnNic *)this)->NicTimer(); }
INLINE void CXnNic::EnetRecv(CPacket * ppkt, UINT uiType) { ((CXnEnet *)this)->EnetRecv(ppkt, uiType); }
INLINE void CXnNic::EnetPush() { ((CXnEnet *)this)->EnetPush(); }
INLINE void CXnEnet::IpRecv(CPacket * ppkt) { ((CXnIp *)this)->IpRecv(ppkt); }
INLINE void CXnEnet::IpRecvArp(CEnetAddr * pea) { ((CXnIp *)this)->IpRecvArp(pea); }
INLINE void CXnIp::UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen) { ((CXnSock *)this)->UdpRecv(ppkt, pIpHdr, pUdpHdr, cbLen); }
INLINE void CXnIp::TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen) { ((CXnSock *)this)->TcpRecv(ppkt, pIpHdr, pTcpHdr, cbHdrLen, cbLen); }
INLINE void CXnIp::SockReset(CIpAddr ipa) { ((CXnSock *)this)->SockReset(ipa); }

#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_FRAG
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_FRAG

INLINE void CXnIp::FragTerm() {}

#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_ROUTE
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_ROUTE

INLINE NTSTATUS CXnIp::RouteInit() { return(NETERR_OK); }
INLINE void     CXnIp::RouteTerm() {}
INLINE void     CXnIp::RouteAdd(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext, WORD wFlags, WORD wMetric) {}
INLINE void     CXnIp::RouteDelete(CIpAddr ipaDst, CIpAddr ipaMask, CIpAddr ipaNext) {}
INLINE void     CXnIp::RouteRedirect(CIpAddr ipaDst, CIpAddr ipaOldGateway, CIpAddr ipNewGateway) {}
INLINE void     CXnIp::RouteRelease(CRouteEntry * prte) {}
INLINE void     CXnIp::RouteListOrphan() {}

#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_DHCP
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_DHCP

INLINE NTSTATUS CXnIp::DhcpInit() { return(NETERR_OK); }
INLINE void     CXnIp::DhcpTerm() {}
INLINE void     CXnIp::DhcpTimer(CTimer * pt) {}
INLINE void     CXnIp::IpRecvArp(CEnetAddr *) {}

#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_TRACE
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_TRACE

INLINE void CXnSock::TcpSetState(CTcpSocket* pTcpSocket, BYTE state, const CHAR* caller)
{
    pTcpSocket->SetState(state);
}

#endif

// ---------------------------------------------------------------------------------------
// XNET_FEATURE_ASSERT
// ---------------------------------------------------------------------------------------

#ifndef XNET_FEATURE_ASSERT

INLINE void *   CXnBase::LeakAdd(CLeakInfo * pli, void * pv, UINT cb, ULONG tag) { return(pv); }
INLINE void *   CXnBase::LeakDel(CLeakInfo * pli, void * pv) { return(pv); }

#endif

// ---------------------------------------------------------------------------------------

#endif

