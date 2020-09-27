/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcpimpl.h

Abstract:

    TCP implementation header file

Revision History:

    05/31/2000 davidx
        Created it.

--*/

#ifndef _TCPIMPL_H
#define _TCPIMPL_H

//
// Sequence number related functions and macros
//
INLINE TCPSEQ TcbGetIsn() {
    // Get the initial sequence number for a new TCP connection.
    // The return value is a 32-bit integer bound to
    // a real-time clock and whose value is incremented
    // every 4 microseconds.
    LARGE_INTEGER time;

    KeQuerySystemTime(&time);
    return (TCPSEQ) (time.QuadPart / 40);
}

#define SEQ_LT(a, b)    ((INT) ((a) - (b)) < 0)
#define SEQ_LE(a, b)    ((INT) ((a) - (b)) <= 0)
#define SEQ_GT(a, b)    ((INT) ((a) - (b)) > 0)
#define SEQ_GE(a, b)    ((INT) ((a) - (b)) >= 0)

// Return the sequence numbers in a TCP segment (in host byte order)
#define SEG_SEQ(_tcphdr) NTOHL((_tcphdr)->seqnum)
#define SEG_ACK(_tcphdr) NTOHL((_tcphdr)->acknum)
#define SEG_WIN(_tcphdr) NTOHS((_tcphdr)->window)
#define SEG_URG(_tcphdr) NTOHS((_tcphdr)->urgent)

// Return the length of a TCP segment (counting SYN and FIN)
#define SEG_LEN(_tcphdr, _datalen) \
        ((_datalen) + \
         (((_tcphdr)->flags & TCP_SYN) ? 1 : 0) + \
         (((_tcphdr)->flags & TCP_FIN) ? 1 : 0))

//
// Internal function prototypes
//
VOID PcbCleanup(PCB* pcb, BOOL revivable);
VOID PcbSetupIpHeader(PCB* pcb, Packet* pkt, IpAddrPair* addrpair);
PCB* PcbFindMatch(IPADDR, IPPORT, IPADDR, IPPORT, BYTE, BYTE);
VOID TcbInit(TCB* tcb);
BOOL TcbClose(TCB* tcb, BOOL force);
BOOL TcbStartOutput(TCB* tcb);
TCB* TcbReset(TCB* tcb, NTSTATUS status);
TCB* TcbCloneChild(TCB* tcb);
TCB* TcbXmitTimeout(TCB* tcb);
VOID TcbDoFastRexmit(TCB* tcb);

VOID TcbSendSegment(TCB* tcb, TCPSENDBUF* sendbuf);
NTSTATUS TcbEmitSYN(TCB* tcb, BOOL synAck);
NTSTATUS TcbEmitFIN(TCB* tcb);
VOID TcbEmitACK(TCB* tcb);
VOID TcbEmitRST(IfInfo*, IpAddrPair*, TCPSEQ, TCPSEQ, BYTE);

// Return the effective snd_nxt value for a TCP connection
// NOTE: we cannot use TCB.snd_nxt field directly here
// because the way we're doing persisting causes snd_nxt
// to go past the send window.
INLINE TCPSEQ TcbSndNxt(TCB* tcb) {
    TCPSEQ sndmax = tcb->snd_wl2 + tcb->snd_wnd;
    return (IsTcpSyncState(tcb) && SEQ_GT(tcb->snd_nxt, sndmax)) ?
                sndmax :
                tcb->snd_nxt;
}

// Send a RST segment to the TCP connection peer
INLINE VOID TcbResetPeer(TCB* tcb) {
    TcbEmitRST(tcb->bindIfp,
               &tcb->addrpair,
               TcbSndNxt(tcb),
               tcb->rcv_nxt,
               TCP_ACK);
}

// Multicast related functions
VOID PcbCleanupMcastData(PCB* pcb);
BOOL PcbCheckMcastGroup(PCB* pcb, IPADDR mcastaddr);
IfInfo* PcbGetMcastIf(PCB* pcb);
NTSTATUS PcbSendMcastDgram(PCB* pcb, Packet* pkt, IPADDR dstaddr);

// Forcefully delete PCB and TCB
#define PcbDelete(_pcb) PcbClose(_pcb, TRUE)
#define TcbDelete(_tcb) PcbClose((PCB*) (_tcb), TRUE)

// Is this a TCP socket lingering after being closed?
#define TcbIsLingering(_tcb) ((_tcb)->magicCookie == CLOSED_PCB_COOKIE)

// Calculate the checksum for an outgoing TCP/UDP packet
#define CALC_TCPUDP_CHECKSUM(_pkt, _addrpair, _protocol, _checksum) { \
            PseudoHeader _pseudohdr; \
            _pseudohdr.srcaddr = (_addrpair)->srcaddr; \
            _pseudohdr.dstaddr = (_addrpair)->dstaddr; \
            _pseudohdr.zero = 0; \
            _pseudohdr.protocol = (_protocol); \
            _pseudohdr.length = HTONS((_pkt)->datalen); \
            _checksum = tcpipxsum(0, &_pseudohdr, sizeof(_pseudohdr)); \
            _checksum = ~tcpipxsum(_checksum, (_pkt)->data, (_pkt)->datalen); \
        }

// Set up the TCP header information in an outgoing TCB segment
INLINE VOID
FILL_TCPHEADER(
    Packet* pkt,
    IpAddrPair* addrpair,
    TCPSEQ seq,
    TCPSEQ ack,
    BYTE hdrlen,
    BYTE flags,
    WORD wnd
    )
{
    TcpHeader* tcphdr;
    UINT checksum;
    tcphdr = GETPKTDATA(pkt, TcpHeader);
    tcphdr->srcport = addrpair->srcport;
    tcphdr->dstport = addrpair->dstport;
    tcphdr->seqnum = HTONL(seq);
    tcphdr->acknum = HTONL(ack);
    SETTCPHDRLEN(tcphdr, hdrlen);
    tcphdr->flags = flags;
    tcphdr->window = HTONS(wnd);
    tcphdr->checksum = tcphdr->urgent = 0;
    CALC_TCPUDP_CHECKSUM(pkt, addrpair, IPPROTOCOL_TCP, checksum);
    tcphdr->checksum = (WORD) checksum;
}

// Set up the UDP header information in an outgoing UDP datagram
INLINE VOID FILL_UDPHEADER(Packet* pkt, IpAddrPair* addrpair) {
    UdpHeader* udphdr;
    UINT checksum;
    udphdr = GETPKTDATA(pkt, UdpHeader);
    udphdr->srcport = addrpair->srcport;
    udphdr->dstport = addrpair->dstport;
    udphdr->length = (WORD) HTONS(pkt->datalen);
    udphdr->checksum = 0;
    CALC_TCPUDP_CHECKSUM(pkt, addrpair, IPPROTOCOL_UDP, checksum);
    udphdr->checksum = (WORD) (checksum ? checksum : ~checksum);
}

// Signal PCB events
#define PcbSignalEvent(_pcb, _eventMask) { \
            if ((_pcb)->eventFlags & (_eventMask)) { \
                SetKernelEvent(GetPcbWaitEvent(_pcb)); \
            } \
        }

// Queue up a receive user request on a socket
NTSTATUS PcbQueueOverlappedRecv(PCB* pcb, RECVREQ* recvreq);

//
// Change TCP connection state:
//
#if DBG
VOID TcbSetState(TCB* tcb, BYTE state, const CHAR* caller);
#else
#define TcbSetState(_tcb, _state, _caller) \
        (_tcb)->tcpstate = (_state)
#endif

//
// Global variable declarations
//
extern UINT cfgMSL;
extern UINT cfgMaxDelayedAcks;
extern UINT cfgConnectTimeout;
extern UINT cfgMaxXmitRetries;
extern UINT cfgMinRexmitTimeout;
extern UINT cfgMaxRexmitTimeout;
extern LIST_ENTRY DeadTcbList;

//
// Setting various TCP timers
//
INLINE VOID TcbSetSynTimer(TCB* tcb, UINT ticks) {
    tcb->synTimer = ticks;
}

INLINE VOID TcbSetTimeWaitTimer(TCB* tcb, UINT ticks) {
    if (tcb->timeWaitTimer == 0 || tcb->timeWaitTimer > ticks)
        tcb->timeWaitTimer = ticks;
}

//
// Stop TCP fast retransmit/recovery mode
//
INLINE VOID TcbStopFastRexmitMode(TCB* tcb) {
    tcb->fastRexmitFlag = 0;
    tcb->dupacks = 0;
}

//
// TCP statistics
//
typedef struct _TCPSTATS {
    UINT rexmitTimeouts;
    UINT fastRexmits;
    UINT persists;
    UINT misordered;
} TCPSTATS;

extern TCPSTATS TcpStats;

//
// Figure out the default source address that we should
// use for a TCP or UDP connection, given the outgoing route.
//
INLINE IPADDR PcbGetDefaultSrcAddr(RTE* rte) {
    return ((rte->flags & RTEFLAG_HOST) &&
            (rte->nexthop == IPADDR_LOOPBACK)) ?
                rte->dstaddr :
                rte->ifp->ipaddr;
}

#endif // !_TCPIMPL_H

