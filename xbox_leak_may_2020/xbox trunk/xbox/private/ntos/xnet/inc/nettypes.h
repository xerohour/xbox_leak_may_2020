/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    nettypes.h

Abstract:

    Common type definitions for the networking code

Revision History:

    05/04/2000 davidx
        Created it.

--*/

#ifndef _NETTYPES_H
#define _NETTYPES_H

//
// C inline function declaration
//
#define INLINE __inline

//
// Private function declaration
//  don't do it for debug build
//
#if DBG
#define PRIVATE
#else
#define PRIVATE static
#endif

//
// Map from bit index to bit mask
//
#define BIT(n) (1u << (n))

//
// System pool allocation functions
//
INLINE VOID* SysAlloc(SIZE_T size, ULONG tag) {
    return ExAllocatePoolWithTag(size, tag);
}

INLINE VOID* SysAlloc0(SIZE_T size, ULONG tag) {
    VOID* p = ExAllocatePoolWithTag(size, tag);
    if (p) { memset(p, 0, size); }
    return p;
}

INLINE VOID SysFree(VOID* ptr) {
    if (ptr) { ExFreePool(ptr); }
}

//
// Private pool allocation functions
//
NTSTATUS XnetPoolInit();
VOID XnetPoolCleanup();
VOID XnetPoolDump();
VOID* XnetAlloc(SIZE_T size, ULONG tag);
VOID XnetFree(VOID* ptr);
INLINE VOID* XnetAlloc0(SIZE_T size, ULONG tag) {
    VOID* p = XnetAlloc(size, tag);
    if (p) { memset(p, 0, size); }
    return p;
}

extern VOID* (*XnetUncachedAllocProc)(SIZE_T, ULONG);
extern VOID (*XnetUncachedFreeProc)(VOID*);
#ifdef DVTSNOOPBUG
NTSTATUS XnetUncachedPoolInit();
VOID XnetUncachedPoolCleanup();
#endif


//------------------------------------------------------------------------
// IP address and port types
//------------------------------------------------------------------------

// Byte-order swapping macros
//  - equivalent to htons/htonl/ntohs/ntohl functions
//  - assumes we're on a little-endian machine

#define HTONL(l) \
        ((((l) >> 24) & 0x000000FFL) | \
         (((l) >>  8) & 0x0000FF00L) | \
         (((l) <<  8) & 0x00FF0000L) | \
         (((l) << 24)              ))

#define HTONS(s) ((WORD) \
        ((((s) >> 8) & 0x00FF) | \
         (((s) << 8) & 0xFF00)))

#define NTOHL HTONL
#define NTOHS HTONS

// IPv4 address in network byte order
typedef DWORD IPADDR;

#define IPADDRLEN                sizeof(IPADDR)
#define IPADDR_ANY               0
#define IPADDR_BCAST             0xffffffff
#define IPADDR_LOOPBACK          HTONL(0x7f000001)
#define IPADDR_ALLHOSTS          HTONL(0xe0000001)
#define IPADDR_ALLROUTERS        HTONL(0xe0000002)
#define CLASSA_NETMASK           HTONL(0xff000000)
#define CLASSB_NETMASK           HTONL(0xffff0000)
#define CLASSC_NETMASK           HTONL(0xffffff00)
#define CLASSD_NETMASK           HTONL(0xf0000000)
#define CLASSD_NETID             HTONL(0xe0000000)
#define LOOPBACK_NETID           HTONL(0x7f000000)
#define IS_MCAST_IPADDR          IS_CLASSD_IPADDR
#define IS_BCAST_IPADDR(addr)    ((addr) == IPADDR_BCAST)
#define IS_CLASSA_IPADDR(addr)   (((addr) & HTONL(0x80000000)) == 0)
#define IS_CLASSB_IPADDR(addr)   (((addr) & HTONL(0xc0000000)) == HTONL(0x80000000))
#define IS_CLASSC_IPADDR(addr)   (((addr) & HTONL(0xe0000000)) == HTONL(0xc0000000))
#define IS_CLASSD_IPADDR(addr)   (((addr) & CLASSD_NETMASK) == CLASSD_NETID)
#define IS_LOOPBACK_IPADDR(addr) (((addr) & CLASSA_NETMASK) == LOOPBACK_NETID)

// TCP/UDP port number in network byte order
typedef WORD IPPORT;

// Source and destination address/port combination
typedef struct _IpAddrPair {
    IPADDR dstaddr;
    IPADDR srcaddr;
    IPPORT dstport;
    IPPORT srcport;
} IpAddrPair;

//------------------------------------------------------------------------
// Network data packet
//------------------------------------------------------------------------

typedef struct _IfInfo IfInfo;
typedef struct _Packet Packet;
typedef struct _IpHeader IpHeader;
typedef struct _RouteTableEntry RTE;
typedef VOID (*PktCompletionProc)(Packet* pkt, NTSTATUS status);

typedef struct _Packet {
    Packet* nextpkt;
        // Points to the next packet in the queue

    WORD pktflags;
        // Packet flags

    WORD iphdrOffset;
        // Offset to where IP datagram header starts

    BYTE* data;
        // Points to where actual data starts

    UINT datalen;
        // Specifies amount of data in the packet

    union {
        IPADDR nexthop;
        IfInfo* recvifp;
        UINT_PTR ifdata;
    };
        // This field has different meaning depending on
        // whether the packet is an incoming packet or
        // an outgoing packet:
        //  for an outgoing packet -
        //      initially contains the next hop IP address
        //      later used as scratch space by the interface driver 
        //  for an incoming packet -
        //      a pointer to the receiving interface

    PktCompletionProc completionCallback;
        // Before passing a packet along for processing,
        // the caller can optionally set up a packet completion
        // routine. When the callee is done with the packet,
        // it'll calls CompletePacket to dispose of the packet.
        // CompletePacket will call the completion routine
        // if it's present. Otherwise, it simply frees the
        // packet memory buffer.
        //
        // !! IMPORTANT NOTE:
        // The callee must not reuse a packet for other purposes
        // if the completion routine is present. Specifically,
        // when an interface driver passes a packet to the upper
        // layer protocols, the upper layer protocol must not
        // reuse the same packet for transmission if the packet
        // completion routine is set.

    BYTE buf[1];
        // Variable size data buffer
} Packet;

//
// Packet header size
//  NOTE: Change the packet fields with care. It should be
//  kept to be a multiple of 8 bytes.
//
#define PKTHDRLEN offsetof(Packet, buf)

// Convenience macros for getting a typecasted pointer
// to the beginning of the packet data buffer and to
// the current packet data.
#define GETPKTBUF(_pkt, _type) ((_type *) (_pkt)->buf)
#define GETPKTDATA(_pkt, _type) ((_type *) (_pkt)->data)

// Figure out where the IP header data starts
#define GETPKTIPHDR(_pkt) ((IpHeader*) ((BYTE*) (_pkt) + (_pkt)->iphdrOffset))
#define SETPKTIPHDR(_pkt, _iphdr) (_pkt)->iphdrOffset = (WORD) ((BYTE*) (_iphdr) - (BYTE*) (_pkt))

//
// bit 0: whether the packet memory came from our private pool
// bit 1: whether the packet is used by the debug monitor stack
// bit 2: physically contiguous memory allocated for DMA transfer
// bit 3: received as link-layer broadcast or multicast packet
//
#define PKTFLAG_NETPOOL BIT(0)  // came from our private pool
#define PKTFLAG_DBGMON  BIT(1)  // used by the debug monitor stack
#define PKTFLAG_DMA     BIT(2)  // receive-only
#define PKTFLAG_MCAST   BIT(3)  // receive-only
#define PKTFLAG_REXMIT  BIT(4)  // send-only: for retransmitted packets
#define PKTFLAG_UNCACHED BIT(5) // temporary hack to workaround nv2a bug

// Allocate and free a network packet
Packet* XnetAllocPacket(UINT size, UINT pktflags);
INLINE VOID XnetFreePacket(Packet* pkt) {
    #ifdef DVTSNOOPBUG
    if (pkt->pktflags & PKTFLAG_UNCACHED) {
        XnetUncachedFreeProc(pkt);
    } else
    #endif
    if (pkt->pktflags & PKTFLAG_NETPOOL) {
        XnetFree(pkt);
    } else {
        SysFree(pkt);
    }
}

// Call this to dispose a packet when we're done using it
INLINE VOID XnetCompletePacket(Packet* pkt, NTSTATUS status) {
    if (pkt->completionCallback) {
        // If the packet has an associated completion function, call it.
        pkt->completionCallback(pkt, status);
    } else {
        // Otherwise, just free it back to the packet pool
        XnetFreePacket(pkt);
    }
}

// Set packet completion function
INLINE VOID XnetSetPacketCompletion(Packet* pkt, PktCompletionProc proc) {
    pkt->completionCallback = proc;
}

//
// Calculate the free space in a packet before and after
// the current data buffer:
//  pkt             pkt->data  ... + pkt->datalen
//   |               |                |
//   v  header       v                v
//  |xxxxxxxxxxx     xxxxxxxxxxxxxxxxx          |
//              space                   space
//              before                  after
//
#define PktSpaceBefore(_pkt) \
        ((_pkt)->data - (_pkt)->buf)

//------------------------------------------------------------------------
// A singly-linked packet queue
//------------------------------------------------------------------------

typedef struct _PacketQueue {
    Packet* head;
    Packet* tail;
} PacketQueue;

// Initialize a packet queue
#define PktQInit(_q) ((_q)->head = (_q)->tail = NULL)

// Determine if a packet queue is empty
#define PktQIsEmpty(_q) ((_q)->head == NULL)

// Insert a packet to the end of a packet queue
INLINE VOID PktQInsertTail(PacketQueue* q, Packet* pkt) {
    if (q->tail)
        q->tail->nextpkt = pkt;
    else
        q->head = pkt;

    q->tail = pkt;
    pkt->nextpkt = NULL;
}

// Insert a packet to the beginning of a packet queue
INLINE VOID PktQInsertHead(PacketQueue* q, Packet* pkt) {
    if ((pkt->nextpkt = q->head) == NULL)
        q->tail = pkt;
    q->head = pkt;
}

// Remove a packet at the head of a packet queue
//  !!! The queue must not be empty when this is called.
INLINE Packet* PktQRemoveHead(PacketQueue* q) {
    Packet* pkt = q->head;

    if ((q->head = pkt->nextpkt) == NULL)
        q->tail = NULL;
    else
        pkt->nextpkt = NULL;

    return pkt;
}

// Join two queues together:
//  q2 is inserted before q1
//  and we assume q2 isn't empty
INLINE VOID PktQJoinBefore(PacketQueue* q1, PacketQueue* q2) {
    if (!q1->tail) q1->tail = q2->tail;
    q2->tail->nextpkt = q1->head;
    q1->head = q2->head;
}

#endif // !_NETTYPES_H

