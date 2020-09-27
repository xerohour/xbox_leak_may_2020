
#ifndef _WINSOCKP_H
#define _WINSOCKP_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


// ---------------------------------------------------------------------------------------
// XNet Configuration
// ---------------------------------------------------------------------------------------

#define XNET_STARTUP_MANUAL_CONFIG      0x80    // XNetStartup won't configure to defaults

#include <pshpack1.h>

typedef struct {
    BYTE        abSeed[20];                     // Last random seed
    IN_ADDR     ina;                            // Static IP address (0 for DHCP)
    IN_ADDR     inaMask;                        // Static IP subnet mask
    IN_ADDR     inaGateway;                     // Static gateway IP address
    IN_ADDR     inaDnsPrimary;                  // Static primary DNS server IP address
    IN_ADDR     inaDnsSecondary;                // Static secondary DNS server IP address
    char        achDhcpHostName[64];            // Host name for DHCP ("" means no host name)
    char        achPppUserName[64];             // User name for PPPoE ("" means no PPPoE)
    char        achPppPassword[64];             // User password for PPPoE
    char        achPppServer[64];               // Server name for PPPoE ("" ok)
    BYTE        abReserved[192];                // Reserved (makes structure 492 bytes)
    DWORD       dwSigEnd;                       // XNET_CONFIG_PARAMS_SIGEND
} XNetConfigParams;

typedef int __farf__XNetConfigParams[sizeof(XNetConfigParams) == 492];

#define XNET_CONFIG_PARAMS_SIGEND               'XBCP'

#define XNET_CONFIG_NORMAL              0x0000  // Configure normally
#define XNET_CONFIG_TROUBLESHOOT        0x0001  // Configure for troubleshooter in DASH
#define XNET_CONFIG_TROUBLESHOOT_PPPOE  0x0002  // Attempt to discover PPPoE concentrators

typedef struct {
    DWORD       dwFlags;                        // See XNET_STATUS_* below
    IN_ADDR     ina;                            // IP address
    IN_ADDR     inaMask;                        // IP subnet mask
    IN_ADDR     inaGateway;                     // Gateway IP address
    IN_ADDR     inaDnsPrimary;                  // Primary DNS server IP address
    IN_ADDR     inaDnsSecondary;                // Secondary DNS server IP address
    IN_ADDR     inaDhcpServer;                  // IP address of DHCP server
    char        achPppServer[64][4];            // PPPoE concentrators discovered
} XNetConfigStatus;

#define XNET_STATUS_PENDING             0x0001  // XNet configuration is not yet complete
#define XNET_STATUS_PPPOE_DISCOVERED    0x0002  // One or more PPPoE concentrators found
#define XNET_STATUS_PPPOE_CONFIGURED    0x0004  // Credentials accepted by PPPoE concentrator
#define XNET_STATUS_PPPOE_REJECTED      0x0008  // Credentials rejected by PPPoE concentrator
#define XNET_STATUS_PPPOE_NORESPONSE    0x0010  // PPPoE concentrator did not respond
#define XNET_STATUS_DHCP_CONFIGURED     0x0020  // DHCP server provided IP configuration
#define XNET_STATUS_DHCP_REJECTED       0x0040  // DHCP server rejected request
#define XNET_STATUS_DHCP_GATEWAY        0x0080  // DHCP server provided default gateway
#define XNET_STATUS_DHCP_DNS            0x0100  // DHCP server provided DNS servers
#define XNET_STATUS_DHCP_NORESPONSE     0x0200  // DHCP server did not respond
#define XNET_STATUS_DNS_CONFIGURED      0x0400  // DNS server successfully resolved test lookup
#define XNET_STATUS_DNS_FAILED          0x0800  // DNS server failed to resolve test lookup
#define XNET_STATUS_DNS_NORESPONSE      0x1000  // DNS server did not respond to test lookup
#define XNET_STATUS_PING_SUCCESSFUL     0x2000  // ICMP echo to Internet test server successful
#define XNET_STATUS_PING_NORESPONSE     0x4000  // ICMP echo to Internet test server did not respond

#include <poppack.h>

#ifdef __cplusplus
extern "C" {
#endif

INT     WSAAPI XnInit(void ** ppXn, char * pchXbox, const XNetStartupParams * pxnsp, BOOL fWsa, WORD wVersionRequired, LPWSADATA lpWSAData);
INT     WSAAPI XnTerm(void ** ppXn, BOOL fWsa);

INT     WSAAPI XNetConfig(const XNetConfigParams * pxncp, DWORD dwFlags);
INT     WSAAPI XNetGetConfigStatus(XNetConfigStatus * pxncs);

HANDLE  WSAAPI XNetOpenConfigVolume();
BOOL    WSAAPI XNetLoadConfigSector(HANDLE hVolume, UINT iSector, BYTE * pbData, UINT cbData);
BOOL    WSAAPI XNetSaveConfigSector(HANDLE hVolume, UINT iSector, const BYTE * pbData, UINT cbData);
BOOL    WSAAPI XNetCloseConfigVolume(HANDLE hVolume);

BOOL    WSAAPI XNetLoadConfigParams(XNetConfigParams * pxncp);
BOOL    WSAAPI XNetSaveConfigParams(const XNetConfigParams * pxncp);

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------
// Hal Api List
// ---------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NT_INCLUDED
    NTSTATUS HalNtOpenFile(char * pszXbox, PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);
    NTSTATUS HalNtCreateFile(char * pszXbox, PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions);
    NTSTATUS HalNtReadFile(char * pszXbox, HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset);
    NTSTATUS HalNtWriteFile(char * pszXbox, HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset);
    NTSTATUS HalNtClose(char * pszXbox, HANDLE Handle);

    #define HALNTAPILIST() \
        HALAPI(NTSTATUS, NtOpenFile, (PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions), (HALXBOX FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions)) \
        HALAPI(NTSTATUS, NtCreateFile, (PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions), (HALXBOX FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions)) \
        HALAPI(NTSTATUS, NtReadFile, (HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset), (HALXBOX FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset)) \
        HALAPI(NTSTATUS, NtWriteFile, (HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset), (HALXBOX FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset)) \
        HALAPI(NTSTATUS, NtClose, (HANDLE Handle), (HALXBOX Handle))
#else
    #define HALNTAPILIST()
    typedef LONG NTSTATUS;
#endif

HANDLE   HalCreateFile(char * pszXbox, LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
NTSTATUS HalExQueryNonVolatileSetting(char * pszXbox, ULONG ValueIndex, ULONG * Type, VOID * Value, ULONG ValueLength, ULONG * ResultLength);
NTSTATUS HalExSaveNonVolatileSetting(char * pszXbox, ULONG ValueIndex, ULONG Type, const VOID * Value, ULONG ValueLength);
NTSTATUS ExQueryNonVolatileSetting(ULONG ValueIndex, ULONG * Type, VOID * Value, ULONG ValueLength, ULONG * ResultLength);
NTSTATUS ExSaveNonVolatileSetting(ULONG ValueIndex, ULONG Type, const VOID * Value, ULONG ValueLength);

#define HALAPILIST() \
    HALAPI(HANDLE,   CreateFile, (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile), (HALXBOX lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)) \
    HALAPI(NTSTATUS, ExQueryNonVolatileSetting, (ULONG ValueIndex, ULONG * Type, VOID * Value, ULONG ValueLength, ULONG * ResultLength), (HALXBOX ValueIndex, Type, Value, ValueLength, ResultLength)) \
    HALAPI(NTSTATUS, ExSaveNonVolatileSetting, (ULONG ValueIndex, ULONG Type, const VOID * Value, ULONG ValueLength), (HALXBOX ValueIndex, Type, Value, ValueLength)) \
    HALNTAPILIST()

#ifdef _XBOX
    #define HALXBOX
#else
    #define HALXBOX _achXbox,
#endif

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------
// XNet Api List
// ---------------------------------------------------------------------------------------

#define XNETAPILIST() \
    XNETAPI_(INT,       XnInit, (void ** ppXn, char * pchXbox, const XNetStartupParams * pxnsp, BOOL fWsa, WORD wVersionRequired, LPWSADATA lpWSAData), (ppXn, pchXbox, pxnsp, fWsa, wVersionRequired, lpWSAData)) \
    XNETAPI_(INT,       XnTerm, (void ** ppXn, BOOL fWsa), (ppXn, fWsa)) \
    XNETAPI_(INT,       XNetStartup, (const XNetStartupParams * pxnsp), (pxnsp)) \
    XNETAPI_(INT,       XNetCleanup, (), ()) \
    XNETAPI_(HANDLE,    XNetOpenConfigVolume, (), ()) \
    XNETAPI_(BOOL,      XNetLoadConfigSector, (HANDLE hVolume, UINT iSector, BYTE * pbData, UINT cbData), (hVolume, iSector, pbData, cbData)) \
    XNETAPI_(BOOL,      XNetSaveConfigSector, (HANDLE hVolume, UINT iSector, const BYTE * pbData, UINT cbData), (hVolume, iSector, pbData, cbData)) \
    XNETAPI_(BOOL,      XNetCloseConfigVolume, (HANDLE hVolume), (hVolume)) \
    XNETAPI_(BOOL,      XNetLoadConfigParams, (XNetConfigParams * pxncp), (pxncp)) \
    XNETAPI_(BOOL,      XNetSaveConfigParams, (const XNetConfigParams * pxncp), (pxncp)) \
    XNETAPI (INT,       XNetConfig, (const XNetConfigParams * pxncp, DWORD dwFlags), (pxncp, dwFlags)) \
    XNETAPI (INT,       XNetGetConfigStatus, (XNetConfigStatus * pxncs), (pxncs)) \
    XNETAPI (INT,       XNetRandom, (BYTE * pb, UINT cb), (pb, cb)) \
    XNETAPI (INT,       XNetCreateKey, (XNKID * pxnkid, XNKEY * pxnkey), (pxnkid, pxnkey)) \
    XNETAPI (INT,       XNetRegisterKey, (const XNKID * pxnkid, const XNKEY * pxnkey), (pxnkid, pxnkey)) \
    XNETAPI (INT,       XNetUnregisterKey, (const XNKID * pxnkid), (pxnkid)) \
    XNETAPI (INT,       XNetXnAddrToInAddr, (const XNADDR * pxna, const XNKID * pxnkid, IN_ADDR * pina), (pxna, pxnkid, pina)) \
    XNETAPI (INT,       XNetServerToInAddr, (const IN_ADDR ina, DWORD dwServiceId, IN_ADDR * pina), (ina, dwServiceId, pina)) \
    XNETAPI (INT,       XNetInAddrToXnAddr, (const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid), (ina, pxna, pxnkid)) \
    XNETAPI (INT,       XNetInAddrToString, (const IN_ADDR ina, char * pchBuf, INT cchBuf), (ina, pchBuf, cchBuf)) \
    XNETAPI (INT,       XNetUnregisterInAddr, (const IN_ADDR ina), (ina)) \
    XNETAPI (INT,       XNetDnsLookup, (const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns), (pszHost, hEvent, ppxndns)) \
    XNETAPI (INT,       XNetDnsRelease, (XNDNS * pxndns), (pxndns)) \
    XNETAPI (INT,       XNetQosListen, (const XNKID * pxnkid, const BYTE * pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags), (pxnkid, pb, cb, dwBitsPerSec, dwFlags)) \
    XNETAPI (INT,       XNetQosXnAddr, (UINT cxnqos, const XNADDR * apxna[], const XNKID * apxnkid[], const XNKEY * apxnkey[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos), (cxnqos, apxna, apxnkid, apxnkey, dwFlags, hEvent, ppxnqos)) \
    XNETAPI (INT,       XNetQosServer, (UINT cxnqos, const IN_ADDR aina[], const DWORD adwServiceId[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos), (cxnqos, aina, adwServiceId, dwFlags, hEvent, ppxnqos)) \
    XNETAPI (INT,       XNetQosRelease, (XNQOS * pxnqos), (pxnqos)) \
    XNETAPI (DWORD,     XNetGetTitleXnAddr, (XNADDR * pxna), (pxna)) \
    XNETAPI (DWORD,     XNetGetDebugXnAddr, (XNADDR * pxna), (pxna)) \
    XNETAPI_(DWORD,     XNetGetEthernetLinkStatus, (), ()) \

#define SOCKAPILIST() \
    XNETAPI_(INT,       WSAStartup, (WORD wVersionRequired, LPWSADATA lpWSAData), (wVersionRequired, lpWSAData)) \
    XNETAPI_(INT,       WSACleanup, (), ()) \
    XNETAPI (SOCKET,    socket, (IN int af, IN int type, IN int protocol), (af, type, protocol)) \
    XNETAPI (int,       closesocket, (IN SOCKET s), (s)) \
    XNETAPI (int,       shutdown, (IN SOCKET s, IN int how), (s, how)) \
    XNETAPI (int,       ioctlsocket, (SOCKET s, long cmd, u_long* argp), (s, cmd, argp)) \
    XNETAPI (int,       setsockopt, (SOCKET s, int level, int optname, const char* optval, int optlen), (s, level, optname, optval, optlen)) \
    XNETAPI (int,       getsockopt, (SOCKET s, int level, int optname, char * optval, int * optlen), (s, level, optname, optval, optlen)) \
    XNETAPI (int,       getsockname, (SOCKET s, struct sockaddr * name, int * namelen), (s, name, namelen)) \
    XNETAPI (int,       getpeername, (SOCKET s, struct sockaddr * name, int * namelen), (s, name, namelen)) \
    XNETAPI (int,       bind, (SOCKET s, const struct sockaddr * name, int namelen), (s, name, namelen)) \
    XNETAPI (int,       connect, (SOCKET s, const struct sockaddr * name, int namelen), (s, name, namelen)) \
    XNETAPI (int,       listen, (SOCKET s, int backlog), (s, backlog)) \
    XNETAPI (SOCKET,    accept, (SOCKET s, struct sockaddr * addr, int * addrlen), (s, addr, addrlen)) \
    XNETAPI (int,       select, (int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout), (nfds, readfds, writefds, exceptfds, timeout)) \
    XNETAPI (BOOL,      WSAGetOverlappedResult, (SOCKET s, LPWSAOVERLAPPED overlapped, LPDWORD byteCount, BOOL fWait, LPDWORD flags), (s, overlapped, byteCount, fWait, flags)) \
    XNETAPI (INT,       WSACancelOverlappedIO, (SOCKET s), (s)) \
    XNETAPI (int,       recv, (SOCKET s, char* buf, int len, int flags), (s, buf, len, flags)) \
    XNETAPI (int,       WSARecv, (SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesRecv, LPDWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc), (s, bufs, bufcnt, bytesRecv, flags, overlapped, completionproc)) \
    XNETAPI (int,       recvfrom, (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr * from, int * fromlen), (s, buf, len, flags, from, fromlen)) \
    XNETAPI (int,       WSARecvFrom, (SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesRecv, LPDWORD flags, struct sockaddr * fromaddr, LPINT fromlen, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc), (s, bufs, bufcnt, bytesRecv, flags, fromaddr, fromlen, overlapped, completionproc)) \
    XNETAPI (int,       send, (SOCKET s, const char* buf, int len, int flags), (s, buf, len, flags)) \
    XNETAPI (int,       WSASend, (SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesSent, DWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc), (s, bufs, bufcnt, bytesSent, flags, overlapped, completionproc)) \
    XNETAPI (int,       sendto, (SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen), (s, buf, len, flags, to, tolen)) \
    XNETAPI (int,       WSASendTo, (SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesSent, DWORD flags, const struct sockaddr* toaddr, int tolen, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc), (s, bufs, bufcnt, bytesSent, flags, toaddr, tolen, overlapped, completionproc)) \
    XNETAPI_(ULONG,     inet_addr, (const char * pch), (pch)) \
    XNETAPI_(u_long,    htonl, (u_long hostlong), (hostlong)) \
    XNETAPI_(u_short,   htons, (u_short hostshort), (hostshort)) \
    XNETAPI_(u_long,    ntohl, (u_long netlong), (netlong)) \
    XNETAPI_(u_short,   ntohs, (u_short netshort), (netshort)) \
    XNETAPI_(int,       WSAGetLastError, (), ()) \
    XNETAPI_(void,      WSASetLastError, (int error), (error)) \
    XNETAPI_(WSAEVENT,  WSACreateEvent, (), ()) \
    XNETAPI_(BOOL,      WSACloseEvent, (WSAEVENT hEvent), (hEvent)) \
    XNETAPI_(BOOL,      WSASetEvent, (WSAEVENT hEvent), (hEvent)) \
    XNETAPI_(BOOL,      WSAResetEvent, (WSAEVENT hEvent), (hEvent)) \
    XNETAPI_(DWORD,     WSAWaitForMultipleEvents, (DWORD cEvents, const WSAEVENT * lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable), (cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable)) \
    XNETAPI_(int,       __WSAFDIsSet, (SOCKET fd, fd_set * set), (fd, set)) \

// If the compiler chokes on the following macro expansion, it means that one or more
// of the prototypes defined in this file is out of sync with the prototype in XNETAPILIST
// or SOCKAPILIST.

#ifdef __cplusplus
extern "C" {
#endif

#undef  XNETAPI
#define XNETAPI(ret, fname, arglist, paramlist) XBOXAPI ret WSAAPI fname arglist;
#undef  XNETAPI_
#define XNETAPI_(ret, fname, arglist, paramlist) XBOXAPI ret WSAAPI fname arglist;

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------
// CXNet
// ---------------------------------------------------------------------------------------

#ifdef __cplusplus

class CXNet
{
    friend class CXn;

public:

    // Constructor -----------------------------------------------------------------------

    #ifdef _XBOX
        __forceinline CXNet(char * pszXbox = NULL) {}
        #define GetAchXbox() NULL
        #define _WSAAPI_ WSAAPI
    #else
        __forceinline CXNet(char * pszXbox = NULL) { _pXn = NULL; SetAchXbox(pszXbox); }
        __forceinline char *    GetAchXbox() { return(_achXbox); }
        __forceinline void      SetAchXbox(char * pszXbox) { _achXbox[sizeof(_achXbox) - 1] = 0; strncpy(_achXbox, pszXbox ? pszXbox : "xb1@Lan", sizeof(_achXbox) - 1); }
        __forceinline CXn *     GetXn() { return(_pXn); }
        __forceinline CXn **    GetXnRef() { return(&_pXn); }
        __forceinline void      SetXn(CXn * pXn) { _pXn = pXn; }
        #define _WSAAPI_ CXNet::
    #endif

    // API -------------------------------------------------------------------------------

    #undef  XNETAPI
    #undef  XNETAPI_
    #undef  HALAPI

    #ifdef _XBOX
        #define XNETAPI(ret, fname, arglist, paramlist) __forceinline ret fname arglist { return(::fname paramlist); }
        #define XNETAPI_(ret, fname, arglist, paramlist) __forceinline ret fname arglist { return(::fname paramlist); }
        #define HALAPI(ret, fname, arglist, paramlist) __forceinline ret fname arglist { return(::fname paramlist); }
    #else
        #define XNETAPI(ret, fname, arglist, paramlist) ret fname arglist;
        #define XNETAPI_(ret, fname, arglist, paramlist) ret fname arglist;
        #define HALAPI(ret, fname, arglist, paramlist)  __forceinline ret fname arglist { return(Hal##fname paramlist); }
    #endif

    XNETAPILIST()
    SOCKAPILIST()
    HALAPILIST()

    // Data ------------------------------------------------------------------------------

private:

    #ifndef _XBOX
        CXn *   _pXn;                       // Pointer to CXn instance of this stack
        char    _achXbox[64];               // Name of this virtual xbox
    #endif

};

#endif

// ---------------------------------------------------------------------------------------
// Simulated Network Address Translator
// ---------------------------------------------------------------------------------------

#if defined(__cplusplus) && !defined(_XBOX)

#define NAT_ASSIGN_MINIMAL          0x00
#define NAT_ASSIGN_AGGRESSIVE       0x01

#define NAT_FILTER_NONE             0x00
#define NAT_FILTER_ADDRESS          0x01
#define NAT_FILTER_PORT             0x02
#define NAT_FILTER_ADDRESS_PORT     0x03

#define NAT_RECV_OUTER_TO_INNER     0x01    // Packet from outer going to inner
#define NAT_RECV_INNER_TO_OUTER     0x02    // Packet from inner going to outer
#define NAT_RECV_INNER_TO_INNER     0x03    // Packet from inner going to inner

typedef void (* PFNNATRECV)(void * pvNat, void * pvPkt, UINT cbPkt, DWORD dwFlags, IN_ADDR inaOrig, WORD wPortOrig);

struct NATCREATE
{
    XNetStartupParams * _pxnsp;             // Pointer to startup params (or NULL)
    XNetConfigParams    _xncpInner;         // Inner network configuration
    XNetConfigParams    _xncpOuter;         // Outer network configuration
    char                _achXboxInner[64];  // Inner virtual xbox name
    char                _achXboxOuter[64];  // Outer virtual xbox name
    IN_ADDR             _inaBase;           // Base IP address to give to DHCP clients
    IN_ADDR             _inaLast;           // Last IP address to give to DHCP clients
    DWORD               _dwLeaseTime;       // Default DHCP lease time in seconds
    UINT                _iAssign;           // One of NAT_ASSIGN_* constants
    UINT                _iFilter;           // One of NAT_FILTER_* constants
    UINT                _iTimeout;          // Inactivity timeout in seconds
    UINT                _iNatPortBase;      // Base NAT port to assign
    UINT                _cNatPort;          // Number of NAT ports to assign
    PFNNATRECV          _pfnNatRecv;        // Hook function to intercept packets
};

#ifdef __cplusplus
extern "C" {
#endif

void *  WSAAPI XNetNatCreate(NATCREATE * pNatCreate);
void    WSAAPI XNetNatDelete(void * pvNat);
void    WSAAPI XNetNatXmit(void * pvNat, void * pvPkt, BOOL fDiscard);

#ifdef __cplusplus
}
#endif

#endif

#endif  /* _WINSOCKP_H */
