/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    pcb.h

Abstract:

    Protocol control blocks

Revision History:

    06/01/2000 davidx
        Created it.

--*/

#ifndef _PCB_H
#define _PCB_H

//------------------------------------------------------------------------
// Protocol control block for TCP/UDP/RAW sockets
//------------------------------------------------------------------------

typedef struct _RECVREQ RECVREQ;
typedef struct _RECVBUF RECVBUF;
typedef struct _SENDREQ SENDREQ;
typedef struct _SENDBUF SENDBUF;
typedef struct _TCPSENDBUF TCPSENDBUF;
typedef struct _PCBOPT PCBOPT;
typedef struct _PCB PCB;
typedef struct _TCB TCB;

// Socket options

typedef struct _PCBOPT {
    UINT sendTimeout;
    UINT recvTimeout;
    UINT maxSendBufsize;
    UINT maxRecvBufsize;
    BYTE ipTtl;
    BYTE ipTos;
    BYTE mcastTtl;
    BYTE unused;
    UINT ipoptlen;
    BYTE* ipopts;
    LINGER linger;

    struct {
        UINT nonblocking:1;
        UINT broadcast:1;
        UINT reuseAddr:1;
        UINT exclusiveAddr:1;
        UINT noNagle:1;
        UINT ipDontFrag:1;
        UINT ipHdrIncl:1;
        UINT noMcastLoopback:1;
        UINT connectSelected:1;
    };
};

// Per-socket multicast group membership information

typedef struct _PcbMcastGroup {
    IPADDR mcastaddr;
    IPADDR ifaddr;
    IfInfo* ifp;
} PcbMcastGroup;

typedef struct _PcbMcastData {
    IPADDR mcastIfAddr;     // outgoing multicast interface address
    IfInfo* mcastIfp;       // and pointer

    // Variable number of multicast groups
    UINT groupCount;
    PcbMcastGroup mcastGroups[1];
} PcbMcastData;

// Protocol control block for TCP/UDP/RAW sockets

typedef struct _PCB {
    //
    // Doubly-linked list pointers
    //  must be the first field!
    //
    LIST_ENTRY links;

    //
    // Magic cookie for debugging purposes and to indicate
    // whether the PCB is being exclusively accessed.
    //
    LONG magicCookie;

    #define ACTIVE_PCB_COOKIE '+BCP'
    #define BUSY_PCB_COOKIE   '*BCP'
    #define CLOSED_PCB_COOKIE '-bcp'

    //
    // PCB type (SOCK_STREAM/SOCK_DGRAM/SOCK_RAW) and protocol number
    //
    BYTE type;
    BYTE protocol;

    //
    // Current TCP connection state
    //
    BYTE tcpstate;

    //
    // PCB flags
    //
    BYTE flags;

    #define PCBFLAG_SEND_SHUTDOWN   0x01    // cannot send data
    #define PCBFLAG_RECV_SHUTDOWN   0x02    // cannot receive data
    #define PCBFLAG_BOUND           0x04    // bound to local address
    #define PCBFLAG_CONNECTED       0x08    // connected to remote address
    #define PCBFLAG_NETPOOL         0x10    // allocated from our private pool
    #define PCBFLAG_REVIVABLE       0x20    // a revivable TCP connection
    #define PCBFLAG_BOTH_SHUTDOWN   (PCBFLAG_SEND_SHUTDOWN|PCBFLAG_RECV_SHUTDOWN)

    #define IsPcbSendShutdown(_pcb) ((_pcb)->flags & PCBFLAG_SEND_SHUTDOWN)
    #define IsPcbRecvShutdown(_pcb) ((_pcb)->flags & PCBFLAG_RECV_SHUTDOWN)
    #define IsPcbBound(_pcb)        ((_pcb)->flags & PCBFLAG_BOUND)
    #define IsPcbConnected(_pcb)    ((_pcb)->flags & PCBFLAG_CONNECTED)

    //
    // Socket wait event flags
    //
    #define PCBEVENT_READ       0x0001  // = FD_READ
    #define PCBEVENT_WRITE      0x0002  // = FD_WRITE
    #define PCBEVENT_ACCEPT     0x0008  // = FD_ACCEPT
    #define PCBEVENT_CONNECT    0x0010  // = FD_CONNECT
    #define PCBEVENT_CLOSE      0x0020  // = FD_CLOSE
    #define PCBEVENT_CONNRESET  0x8000
    #define PCBEVENT_ALL        (-1)

    INT eventFlags;

    //
    // Socket notification event object
    //
    KEVENT blockEvent;

    #define GetPcbWaitEvent(_pcb) (&(_pcb)->blockEvent)

    //
    // Protocol flags (see XP1_* constants in winsock2.h)
    //
    INT protocolFlags;

    // Check if a PCB is for a datagram socket
    #define IsDgramPcb(_pcb) ((_pcb)->protocolFlags & XP1_CONNECTIONLESS)
    #define IsTcb(_pcb) (!IsDgramPcb(_pcb))

    //
    // Pending receive I/O request and receive buffers
    //
    RECVREQ* overlappedRecvs;
    LIST_ENTRY recvbuf;
    ULONG recvbufSize;

    #define HasOverlappedRecv(_pcb) ((_pcb)->overlappedRecvs != NULL)
    #define PcbGetOverlappedRecv(_pcb) ((_pcb)->overlappedRecvs)
    
    #define IsPcbRecvBufFull(_pcb) ((_pcb)->recvbufSize >= (_pcb)->maxRecvBufsize)
    #define IsDgramRecvBufEmpty(_pcb) IsListEmpty(&(_pcb)->recvbuf)
    #define IsTcpRecvBufEmpty(_tcb) ((_tcb)->recvbufSize == 0)
    #define IsPcbRecvBufEmpty(_pcb) \
            (IsDgramPcb(_pcb) ? IsDgramRecvBufEmpty(_pcb) : IsTcpRecvBufEmpty(_pcb))

    //
    // Pending send I/O request and send buffers
    //
    SENDREQ* overlappedSends;
    LIST_ENTRY sendbuf;
    ULONG sendbufSize;

    #define HasOverlappedSend(_pcb) ((_pcb)->overlappedSends != NULL)

    #define IsPcbSendBufEmpty(_pcb) IsListEmpty(&(_pcb)->sendbuf)
    #define IsPcbSendBufFull(_pcb) ((_pcb)->sendbufSize >= (_pcb)->maxSendBufsize)

    //
    // Socket address pairs: dstaddr, srcaddr, dstport, srcport
    //
    union {
        IpAddrPair addrpair;
        IpAddrPair;
    };
    IPADDR bindSrcAddr;

    //
    // Cached route table entry
    // Interface that the socket was explicitly bound to
    //
    RTE* rte;
    IfInfo* bindIfp;

    //
    // Multicast group membership information
    //  (for UDP and RAW sockets only)
    //
    PcbMcastData* mcastData;

    // 
    // Socket error status
    // For TCP socket, this indicates whether the connection
    // has been reset and; and if so, how the connection got reset.
    //
    NTSTATUS errStatus;

    #define PcbGetErrStatus(_pcb) ((_pcb)->errStatus)

    //
    // Socket options
    //
    union {
        PCBOPT options;
        PCBOPT;
    };
};

//
// Allocate and free a PCB structure
//
PCB* PcbCreate(INT type, INT protocol, BYTE allocFlag);
NTSTATUS PcbClose(PCB* pcb, BOOL force);
VOID PcbCloseAll();

//
// Allocate and free socket IP options
//
INLINE VOID PcbFreeIpOpts(PCB* pcb) {
    if (pcb->flags & PCBFLAG_NETPOOL) {
        XnetFree(pcb->ipopts);
    } else {
        SysFree(pcb->ipopts);
    }
    pcb->ipopts = NULL;
    pcb->ipoptlen = 0;
}

INLINE BYTE* PcbSetIpOpts(PCB* pcb, const VOID* opts, UINT optlen) {
    if (pcb->flags & PCBFLAG_NETPOOL) {
        pcb->ipopts = (BYTE*) XnetAlloc0(optlen, PTAG_SOPTS);
    } else {
        pcb->ipopts = (BYTE*) SysAlloc0(optlen, PTAG_SOPTS);
    }

    if (pcb->ipopts) {
        pcb->ipoptlen = optlen;
        CopyMem(pcb->ipopts, opts, optlen);
    }
    return pcb->ipopts;
}

//
// TCP connection block
//
typedef DWORD TCPSEQ;
typedef struct _TCB {
    //
    // Common protocol control block
    //  must be the first field!
    PCB;

    //
    // Pending connection requests
    //
    TCB* parent;
    LIST_ENTRY listenq;
    UINT backlog, maxBacklog;

    #define IsPendingConnReqTcb(_tcb) ((_tcb)->parent != NULL)
    #define TcbHasPendingConnReq(_tcb) \
            ((_tcb)->backlog ? TcbHasConnectedPendingConnReq(_tcb) : FALSE)
    
    //
    // TCP timers
    //  delayedAcks is the number of delay-acked segments;
    //  all other timers are measured in 500ms ticks.
    //
    // NOTE: Be careful when you add or remove fields between
    // delayedAcks and rtt_tick (inclusive). See comments in
    // function TcbReset().
    //
    UINT delayedAcks;
    UINT synTimer;          // connection establishment timer
    UINT xmitTimer;         // retransmission & persist timer
    UINT timeWaitTimer;     // TIME-WAIT & linger timer

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
    WORD dupacks;           // number of duplicate ACKs received
    BYTE persistFlag;       // are we probing send window?
    BYTE fastRexmitFlag;    // in fast retransmit / fast recovery mode?

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

    #define SRTT_SHIFT      3
    #define SRTT_SCALE      (1 << SRTT_SHIFT)
    #define RTTVAR_SHIFT    2
    #define RTTVAR_SCALE    (1 << RTTVAR_SHIFT)

    //
    // This points to the next send buffer that has been
    // queued up but has yet to be sent
    //
    TCPSENDBUF* sendbufNext;
};

//
// TCP states
//
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

// Idle states:
//  CLOSED, LISTEN
#define IsTcpIdleState(_tcb) ((_tcb)->tcpstate <= TCPST_LISTEN)
#define IsTcpListenState(_tcb) ((_tcb)->tcpstate == TCPST_LISTEN)

// Synchronized states:
//  ESTABLISHED, FIN_WAIT_1, FIN_WAIT_2, CLOSING
//  TIME_WAIT, CLOSE_WAIT, LAST_ACK
#define IsTcpSyncState(_tcb) ((_tcb)->tcpstate >= TCPST_ESTABLISHED)

// FIN has been received from the peer, no more incoming data
#define IsFINReceived(_tcb) ((_tcb)->tcpstate >= TCPST_CLOSING)

// FIN has been sent to the peer, no more outgoing data
#define IsFINSent(_tcb) ((_tcb)->tcpstate >= TCPST_FIN_WAIT_1 && \
                         (_tcb)->tcpstate != TCPST_CLOSE_WAIT)

// Maximum TCP window size (no window scaling option)
#define MAX_TCP_WNDSIZE 0xffff

// Increment the congestion window size for a TCP connection
INLINE VOID TcbIncrementCwnd(TCB* tcb, UINT incr) {
    if ((tcb->snd_cwnd += incr) > MAX_TCP_WNDSIZE)
        tcb->snd_cwnd = MAX_TCP_WNDSIZE;
}

// Reset the slow-start threshold for a TCP connection
INLINE VOID TcbResetSsthresh(TCB* tcb) {
    // NOTE: unsigned arithmetic
    tcb->snd_ssthresh = (tcb->snd_nxt - tcb->snd_una) >> 1;
    if (tcb->snd_ssthresh < 2*tcb->snd_mss)
        tcb->snd_ssthresh = 2*tcb->snd_mss;
}

//------------------------------------------------------------------------
// Data structures for transmitting and receiving data
//------------------------------------------------------------------------

//
// We use WSAOVERLAPPED structure to keep track of
// the status of an overlapped I/O request. When
// the request is pending, the fields are interpreted
// as follows:
//  Internal - pointer to the I/O request structure
//  InternalHigh - unused
//  Offset - unused
//  OffsetHigh - status code = NETERR_PENDING
//
// After a request is completed, the fields are
// interpreted differently:
//  Internal - unused
//  InternalHigh - I/O request completion flags
//  Offset - number of bytes actually transferred
//  OffsetHigh - status code != NETERR_PENDING
//
#define _ioreq      Internal
#define _ioflags    InternalHigh
#define _ioxfercnt  Offset
#define _iostatus   OffsetHigh

// A pending receive request

typedef struct _PcbOverlappedReq {
    PCB* pcb;
    PRKEVENT overlappedEvent;
    WSAOVERLAPPED* overlapped;
} PcbOverlappedReq;

typedef struct _RECVREQ {
    PcbOverlappedReq;   // must be the first field
    DWORD flags;
    DWORD* bytesRecv;
    BYTE* buf;
    UINT buflen;
    struct sockaddr_in* fromaddr;
};

// Complete an overlapped receive request
VOID PcbCompleteOverlappedSendRecv(PcbOverlappedReq* req, NTSTATUS status);

#define PcbCompleteOverlappedRecv(_recvreq, _status) \
        PcbCompleteOverlappedSendRecv((PcbOverlappedReq*) (_recvreq), _status)

// Clear all overlapped receive requests
#define PcbClearOverlappedRecvs(_pcb, _status) { \
            if ((_pcb)->overlappedRecvs) { \
                PcbCompleteOverlappedRecv((_pcb)->overlappedRecvs, _status); \
            } \
        }

// Set the sender's address for received data packet
#define SetRecvReqFromAddr(_recvreq, _fromaddr, _fromport) { \
            if ((_recvreq)->fromaddr) { \
                (_recvreq)->fromaddr->sin_port = (_fromport); \
                (_recvreq)->fromaddr->sin_addr.s_addr = (_fromaddr); \
            } \
        }

// Buffered incoming data

typedef struct _RECVBUF {
    LIST_ENTRY links;   // !! must be the first field
    UINT datalen;
    union {
        // For TCP sockets, points to the start of
        // the remaining data in this buffer.
        struct {
            TCPSEQ seqnext;
            WORD dataoffset;
            BYTE tcpflags;
        };

        // For UDP/RAW sockets, remembers the sender
        // of the datagram.
        struct {
            IPADDR fromaddr;
            IPPORT fromport;
        };
    };
};

// A send request

typedef struct _SENDREQ {
    PcbOverlappedReq;   // must be the first field
    WSABUF* bufs;
    UINT bufcnt;
    UINT sendtotal;
    struct sockaddr_in* toaddr;
};

// Queue up an overlapped send request
NTSTATUS PcbQueueOverlappedSend(PCB* pcb, SENDREQ* sendreq);

// Complete an overlapped send request
#define PcbCompleteOverlappedSend(_sendreq, _status) \
        PcbCompleteOverlappedSendRecv((PcbOverlappedReq*) (_sendreq), _status)

// Clear all overlapped send requests
#define PcbClearOverlappedSends(_pcb, _status) { \
            if ((_pcb)->overlappedSends) { \
                PcbCompleteOverlappedSend((_pcb)->overlappedSends, _status); \
            } \
        }

// Dequeue the overlapped send request
INLINE SENDREQ* PcbGetOverlappedSend(PCB* pcb) {
    SENDREQ* sendreq = pcb->overlappedSends;
    pcb->overlappedSends = NULL;
    return sendreq;
}

// Buffer outgoing data (common to UDP and TCP)

typedef struct _SENDBUF {
    LIST_ENTRY links;   // !! must be the first field
    LONG refcount;
    UINT datalen;
    PCB* pcb;
};

// Get the pointer to the Packet that contains a SENDBUF. For our
// outgoing packets (either UDP/RAW datagrams or TCP segments), we
// store a SENDBUF structure right after the packet header.
#define GetSendbufPkt(_sendbuf) \
        ((Packet*) ((BYTE*) (_sendbuf) - PKTHDRLEN))

#define IsSendbufActive(_sendbuf) ((_sendbuf)->refcount > 1)
#define SendbufIncRef(_sendbuf) ((_sendbuf)->refcount += 1)
#define SendbufDecRef(_sendbuf) ((_sendbuf)->refcount -= 1)
#define SendbufRelease(_sendbuf) { \
            if (SendbufDecRef(_sendbuf) == 0) { \
                XnetFreePacket(GetSendbufPkt(_sendbuf)); \
            } \
        }

// TCP send buffers

typedef struct _TCPSENDBUF {
    SENDBUF;
    BYTE* pktdata;      // saved packet data pointer and length
    UINT pktdatalen;    //
    TCPSEQ seq;         // starting sequence number for this TCP segment
    TCPSEQ seqnext;     // the first seqno after this segment
    WORD retries;       // number of times this segment has been rexmitted
    BYTE tcpflags;      // TCP segment flags
    BYTE tcphdrlen;     // TCP segment header length
    UINT firstSendTime; // the time this segment was first sent
};

#define TcbSendbufNil(_tcb) ((TCPSENDBUF*) &(_tcb)->sendbuf)
#define TcbFirstSendbuf(_tcb) ((TCPSENDBUF*) (_tcb)->sendbuf.Flink)
#define TcbSendbufFlink(_sendbuf) ((TCPSENDBUF*) (_sendbuf)->links.Flink)
#define TcbHasPendingSend(_tcb) ((_tcb)->sendbufNext != TcbSendbufNil(_tcb))

// Loop through all transmission buffers for a TCP connection block

#define LOOP_THRU_TCB_SENDBUF(_tcb, _sendbuf) { \
            TCPSENDBUF* _next; \
            _sendbuf = TcbFirstSendbuf(_tcb); \
            for (; _sendbuf != TcbSendbufNil(_tcb); _sendbuf = _next) { \
                _next = TcbSendbufFlink(_sendbuf);

#define END_TCB_SENDBUF_LOOP \
            } \
        }

#define TcbRecvbufNil(_tcb) ((RECVBUF*) &(_tcb)->recvbuf)
#define TcbLastRecvbuf(_tcb) ((RECVBUF*) (_tcb)->recvbuf.Blink)
#define TcbRecvbufBlink(_recvbuf) ((RECVBUF*) (_recvbuf)->links.Blink)
#define TcbRecvbufFlink(_recvbuf) ((RECVBUF*) (_recvbuf)->links.Flink)

//------------------------------------------------------------------------
// Global PCB list
//------------------------------------------------------------------------

extern LIST_ENTRY PcbList;
extern ULONG PcbCount;

#define LOOP_THRU_PCB_LIST(_pcb) { \
            PCB* _next; \
            _pcb = (PCB*) PcbList.Flink; \
            for (; _pcb != (PCB*) &PcbList; _pcb = _next) { \
                _next = (PCB*) _pcb->links.Flink;

#define END_PCB_LIST_LOOP \
            } \
        }
//
// Insert a PCB into the global list
//
INLINE VOID PcbInsertToList(PCB* pcb) {
    KIRQL irql = RaiseToDpc();
    InsertHeadList(&PcbList, &pcb->links);
    LowerFromDpc(irql);
}

#define IsPcbListEmpty() IsListEmpty(&PcbList)

//
// Block the current thread until the specified
// socket event is signalled
//
NTSTATUS PcbWaitForEvent(PCB* pcb, INT eventMask, UINT timeout);
INT PcbCheckSelectEvents(PCB* pcb, INT eventMasks, INT setwait);
INLINE VOID PcbClearSelectEvents(PCB* pcb) {
    pcb->eventFlags = 0;
}

//
// Flush a socket's receive buffers
//
INLINE VOID PcbFlushRecvBuffers(PCB* pcb) {
    while (!IsListEmpty(&pcb->recvbuf)) {
        VOID* head = RemoveHeadList(&pcb->recvbuf);
        XnetFree(head);
    }
    pcb->recvbufSize = 0;
}

//
// Public PCB functions
//
NTSTATUS PcbBind(PCB* pcb, IPADDR srcaddr, IPPORT srcport);
NTSTATUS PcbConnectDgram(PCB* pcb, IPADDR dstaddr, IPPORT dstport);
NTSTATUS PcbSendDgram(PCB* tcb, SENDREQ* sendreq);
NTSTATUS PcbRecvDgram(PCB* pcb, RECVREQ* recvreq);
NTSTATUS PcbShutdownDgram(PCB* pcb, BYTE flags);
NTSTATUS PcbUpdateBufferSize(PCB* pcb, INT sendBufsize, INT recvBufsize);
NTSTATUS PcbSetMcastIf(PCB* pcb, IPADDR ifaddr);
NTSTATUS PcbChangeMcastGroup(PCB* pcb, IPADDR mcastaddr, IPADDR ifaddr, BOOL add);

NTSTATUS TcbConnect(TCB* tcb, IPADDR dstaddr, IPPORT dstport, BOOL synAck);
NTSTATUS TcbListen(TCB* tcb, INT backlog);
NTSTATUS TcbAccept(TCB* tcb, TCB** newtcb);
NTSTATUS TcbSend(TCB* tcb, SENDREQ* sendreq);
NTSTATUS TcbRecv(TCB* tcb, RECVREQ* recvreq);
NTSTATUS TcbShutdown(TCB* tcb, BYTE flags, BOOL apicall);
BOOL TcbHasConnectedPendingConnReq(TCB* tcb);

#endif // !_PCB_H

