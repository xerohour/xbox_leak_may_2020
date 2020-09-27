/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    netutil.h

Abstract:

    Common functions and macros shared by the networking code

Revision History:

    03/27/2000 davidx
        Created it.

--*/

#ifndef _NETUTIL_H
#define _NETUTIL_H

//
// Misc. macros
//
#define SizeofWSTR(ws)  ((wcslen(ws) + 1) * sizeof(WCHAR))
#define SizeofSTR(s)    ((strlen(s) + 1) * sizeof(CHAR))
#define ARRAYCOUNT(a)   (sizeof(a) / sizeof(a[0]))
#define ROUNDUP4(count) (((UINT)(count) + 3) & ~3)
#define ROUNDUP8(count) (((UINT)(count) + 7) & ~7)
#define ZeroMem(p, n)   memset(p, 0, n)
#define CopyMem         memcpy
#define MoveMem         memmove
#define EqualMem(d,s,n) (memcmp(d, s, n) == 0)

//
// Memory allocation and deallocation functions
//

#ifdef _XNET_SERVICE

// allocate from the process heap
INLINE VOID* MAlloc(SIZE_T size) {
    return (VOID*) LocalAlloc(LMEM_FIXED, size);
}

INLINE VOID* MAlloc0(SIZE_T size) {
    return (VOID*) LocalAlloc(LPTR, size);
}

// free to the process heap
INLINE VOID Free(VOID* ptr) {
    LocalFree((HLOCAL) (ptr));
}

#endif // _XNET_SERVICE

//
// General net error codes:
//  facility = win32 + winsock error code
//
#define NETERR(_err)        HRESULT_FROM_WIN32(_err)
#define NETERR_OK           STATUS_SUCCESS
#define NETERR_PARAM        NETERR(WSAEINVAL)
#define NETERR_MEMORY       NETERR(WSAENOBUFS)
#define NETERR_SYSCALL      NETERR(WSASYSCALLFAILURE)
#define NETERR_FAULT        NETERR(WSAEFAULT)
#define NETERR_CANCELLED    NETERR(WSAECANCELLED)
#define NETERR_PENDING      NETERR(WSA_IO_PENDING)
#define NETERR_WOULDBLOCK   NETERR(WSAEWOULDBLOCK)
#define NETERR_MSGSIZE      NETERR(WSAEMSGSIZE)
#define NETERR_TIMEOUT      NETERR(WSAETIMEDOUT)
#define NETERR_NOTIMPL      NETERR(ERROR_CALL_NOT_IMPLEMENTED)
#define NETERR_UNREACHABLE  NETERR(WSAEHOSTUNREACH)
#define NETERR_NETDOWN      NETERR(WSAENETDOWN)
#define NETERR_ADDRINUSE    NETERR(WSAEADDRINUSE)
#define NETERR_ADDRCONFLICT NETERR(ERROR_DHCP_ADDRESS_CONFLICT)
#define NETERR_CONNRESET    NETERR(WSAECONNRESET)

// XBox-specific net error codes:
#define NETERR_HARDWARE     0x801f0001  // hardware not responding
#define NETERR_DISCARDED    0x801f0002  // packet discarded
#define NETERR_REASSEMBLY   0x801f0003  // IP datagram reassembly failed

//
// Raise IRQL to DISPATCH_LEVEL and restore it
//
#define RaiseToDpc KeRaiseIrqlToDpcLevel
#define LowerFromDpc KeLowerIrql

// Check if a doubly-linked list head is NULL (uninitialized)
#define IsListNull(_list) ((_list)->Flink == NULL)

#ifndef _NTSYSTEM_

//
// Pseudo-random number generator
//  range: 0 to 0x7fffffff
//
extern ULONG XnetRandSeed;

VOID XnetInitRandSeed();

INLINE ULONG XnetRand() {
    return RtlRandom(&XnetRandSeed);
}

INLINE ULONG XnetRandScaled(ULONG maxval) {
    return RtlRandom(&XnetRandSeed) % (maxval+1);
}

// Check if an IP address is a valid non-loopback unicast address
INLINE XnetIsValidUnicastAddr(IPADDR addr) {
    return (addr != 0) &&
           !IS_BCAST_IPADDR(addr) &&
           !IS_MCAST_IPADDR(addr) &&
           !IS_LOOPBACK_IPADDR(addr);
}

// Convert an IP address to an ASCII character string and vice versa
CHAR* IpAddrToString(IPADDR ipaddr, CHAR* buf, INT buflen);
BOOL IpAddrFromString(const CHAR* str, IPADDR* addr);

INLINE CHAR* IPADDRSTR(IPADDR ipaddr) {
    static CHAR buf[16];
    return IpAddrToString(ipaddr, buf, sizeof(buf));
}

// Return the default subnet mask for a given IP address
IPADDR XnetGetDefaultSubnetMask(IPADDR ipaddr);

// Check if a subnet mask is valid (i.e. of the form 111...000)
INLINE BOOL XnetIsValidSubnetMask(IPADDR addrmask) {
    addrmask = ~NTOHL(addrmask);
    return (addrmask & (addrmask+1)) == 0;
}

//
// Compute and set the checksum field of some message header
//
UINT tcpipxsum(UINT xsum, const VOID* buf, UINT buflen);
#define COMPUTE_CHECKSUM(_field, _buf, _len) \
        (_field) = 0; \
        (_field) = (WORD) ~tcpipxsum(0, _buf, _len)

//
// Make a copy of the specified packet
//
Packet* XnetCopyPacket(Packet* pkt, UINT extraHdr);

//
// Deferenece an XAPI event handle to get
// a pointer to the kernel-mode event object
//
INLINE PRKEVENT GetKernelEventObject(HANDLE hEvent) {
    NTSTATUS status;
    PRKEVENT kEvent;

    if (!hEvent) return NULL;
    status = ObReferenceObjectByHandle(
                hEvent,
                ExEventObjectType,
                (VOID**) &kEvent);

    return NT_SUCCESS(status) ? kEvent : NULL;
}

//
// Wait for a kernel event object to be signalled
//
INLINE NTSTATUS WaitKernelEventObject(PRKEVENT kEvent, UINT timeout) {
    LARGE_INTEGER waittime;
    NTSTATUS status;

    waittime.QuadPart = Int32x32To64(timeout, -10000);
    status = KeWaitForSingleObject(
                    kEvent,
                    UserRequest,
                    UserMode,
                    FALSE,
                    timeout ? &waittime : NULL);

    return (status == STATUS_SUCCESS) ? NETERR_OK : NETERR_TIMEOUT;
}

//
// Signal a kernel event object
//
INLINE VOID SetKernelEvent(PRKEVENT kEvent) {
    KeSetEvent(kEvent, EVENT_INCREMENT, FALSE);
}

#endif // !_NTSYSTEM_

//
// Pool tags
//
#define PTAG_POOL   '!TEN'
#define PTAG_PKT    '0TEN'
#define PTAG_ENET   '1TEN'
#define PTAG_ARP    '2TEN'
#define PTAG_LPBK   '3TEN'
#define PTAG_MCAST  '4TEN'
#define PTAG_RTE    '5TEN'
#define PTAG_PCB    '6TEN'
#define PTAG_TCB    '7TEN'
#define PTAG_RREQ   '8TEN'
#define PTAG_RBUF   '9TEN'
#define PTAG_DHCP   'aTEN'
#define PTAG_DNS    'bTEN'
#define PTAG_SOPTS  'cTEN'
#define PTAG_NIC    'dTEN'

#endif // !_NETUTIL_H

