/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    xnetp.h

Abstract:

    Prototypes shared by XBox net implementation

Revision History:

    05/17/2000 davidx
        Created it.

--*/

#ifndef _XNETP_H
#define _XNETP_H

// Put all xnet code in its own section
#pragma code_seg("XNET")
#pragma const_seg("XNET_RD")

#define _NTDRIVER_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stddef.h>
#include <ntos.h>
#include <nturtl.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xtl.h>
#include <winsockp.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "nettypes.h"
#include "debug.h"
#include "ip.h"
#include "tcp.h"
#include "interface.h"
#include "netutil.h"

//
// TCP/UDP module
//

NTSTATUS TcpInitialize();
VOID TcpCleanup();
VOID TcpReceivePacket(Packet* pkt);
VOID TcpSourceQuench(const IpHeader* iphdr);
VOID UdpReceivePacket(Packet* pkt);
VOID RawReceivePacket(Packet* pkt);
VOID UdpSendPacketInternal(Packet*, IpAddrPair*, IfInfo*);

// TCP tick count is increment every half second (500ms)
#define SLOW_TCP_TIMER_FREQ 2
extern UINT TcpTickCount;

//
// IP module
//

NTSTATUS IpInitialize();
VOID IpCleanup();
VOID IpTimerProc();

NTSTATUS IpSendPacket(Packet* pkt, IPADDR dstaddr, IfInfo* ifp, RTE** rte);
VOID IpSendPacketInternal(Packet*, IPADDR, IPADDR, BYTE, IfInfo*);
VOID IcmpSendError(Packet* pkt, UINT type, UINT code);
VOID IpReceivePacket(Packet* pkt);

DWORD IpGetBestAddress(IPADDR* addr);
VOID IfSetIpAddr(IfInfo*, IPADDR, IPADDR);
NTSTATUS IfGetDefaultDnsServers(IfInfo*, CHAR*, UINT, IPADDR*, UINT*);

// Return the identifier for the next outgoing IP datagram
// Note that we use the same global ID space for all interfaces.
extern WORD IpNextDgramId;

// Fill in the information in an IP datagram header
#define FILL_IPHEADER(_iphdr, _hdrlen, _tos, _len, _df, _ttl, _proto, _srcaddr, _dstaddr) { \
            (_iphdr)->ver_hdrlen = (BYTE) (IPVER4 | (_hdrlen) >> 2); \
            (_iphdr)->tos = (_tos); \
            (_iphdr)->length = HTONS(_len); \
            (_iphdr)->id = HTONS(IpNextDgramId); IpNextDgramId++; \
            (_iphdr)->fragoffset = (WORD) (_df); \
            (_iphdr)->ttl = (BYTE) (_ttl); \
            (_iphdr)->protocol = (_proto); \
            (_iphdr)->srcaddr = (_srcaddr); \
            (_iphdr)->dstaddr = (_dstaddr); \
            COMPUTE_CHECKSUM((_iphdr)->hdrxsum, (_iphdr), (_hdrlen)); \
        }

// Allocate packet to hold the specified amount of
// protocol header information as well as data
INLINE Packet* XnetAllocIpPacket(UINT ipoptlen, UINT datalen) {
    // Reserve enough space at the beginning of the packet buffer
    // to hold the link-layer frame header. Then add the IP header
    // and option length.
    UINT hdrlen = MAXLINKHDRLEN + IPHDRLEN + ROUNDUP4(ipoptlen);
    Packet* pkt;

    if ((pkt = XnetAllocPacket(hdrlen+datalen, PKTFLAG_NETPOOL)) != NULL) {
        pkt->iphdrOffset = PKTHDRLEN + MAXLINKHDRLEN;
        pkt->data += hdrlen;
        pkt->datalen = datalen;
    }
    return pkt;
}

//
// DHCP module
//

NTSTATUS DhcpInitialize(IfInfo* ifp);
VOID DhcpCleanup(IfInfo* ifp);
VOID DhcpTimerProc(IfInfo* ifp);
VOID DhcpReceivePacket(IfInfo* ifp, Packet* pkt);
VOID DhcpNotifyAddressConflict(IfInfo* ifp);
VOID DhcpSetDefaultGateways(IfInfo* ifp);
DWORD DhcpGetActiveAddressType(IfInfo* ifp);
NTSTATUS DhcpWaitForAddress(IfInfo* ifp);
NTSTATUS DhcpGetDefaultDnsServers(IfInfo*, CHAR*, UINT, IPADDR*, UINT*);

//
// Network interface modules
//

NTSTATUS EnetInitialize(IfInfo** newifp);
NTSTATUS IpQueueLoopbackPacket(Packet* pkt, BOOL copyflag);

//
// DNS module
//

NTSTATUS DnsInitialize();
VOID DnsCleanup();
VOID DnsNotifyDefaultServers(IfInfo* ifp);

typedef struct hostent* (*DnsLookupCallback)(UINT size, VOID* param);
NTSTATUS DnsLookupByName(const char* name, DnsLookupCallback callback, VOID* param);
NTSTATUS DnsLookupByAddr(IPADDR addr, DnsLookupCallback callback, VOID* param);

//
// Configuration parameters
//
extern UINT cfgXnetPoolSize;
extern UINT cfgRecvQLength;
extern UINT cfgXmitQLength;
extern UINT cfgMaxReassemblySize;
extern UINT cfgMaxReassemblyDgrams;
extern BYTE cfgDefaultTtl;
extern BYTE cfgDefaultTos;
extern UINT cfgDefaultSendBufsize;
extern UINT cfgDefaultRecvBufsize;
extern UINT cfgMaxSendRecvBufsize;
extern UINT cfgMaxSockets;
extern UINT cfgXnetConfigFlags;

extern UINT defaultPacketAllocFlag;

// Check if XNET is running inside the debug monitor
#define XnetInsideDbgmon() (defaultPacketAllocFlag & PKTFLAG_DBGMON)

//
// Maximum number of default gateways and DNS servers
//
#define MAX_DEFAULT_DNSSERVERS 4
#define MAX_DEFAULT_GATEWAYS 4

//
// Perf counters
//
#ifdef DEVKIT

extern ULONGLONG IpTotalBytesSent;
extern ULONGLONG IpTotalBytesRecv;
extern ULONGLONG TcpTotalBytesSent;
extern ULONGLONG TcpTotalBytesRecv;
extern ULONGLONG UdpTotalBytesSent;
extern ULONGLONG UdpTotalBytesRecv;
extern ULONG IpTotalDgramsSent;
extern ULONG IpTotalDgramsRecv;
extern ULONG UdpTotalDgramsSent;
extern ULONG UdpTotalDgramsRecv;
extern ULONG TcpTotalSegmentsSent;
extern ULONG TcpTotalSegmentsRecv;

#define XnetIncrementPerfCount(counter, incr) ((counter) += (incr))

#else // !DEVKIT

#define XnetIncrementPerfCount(counter, incr)

#endif // !DEVKIT

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_XNETP_H

