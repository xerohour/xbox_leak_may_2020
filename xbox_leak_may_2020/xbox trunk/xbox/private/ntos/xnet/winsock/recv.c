/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    recv.c

Abstract:

    Implementation of data reception related Winsock APIs:
        recv
        recvfrom
        WSARecv
        WSARecvFrom

Revision History:

    06/02/2000 davidx
        Created it.

--*/

#include "precomp.h"

// Disable unreference label warning
#pragma warning(disable:4102)


PRIVATE NTSTATUS
SockRecv(
    PCB* pcb,
    RECVREQ* recvreq
    )

/*++

Routine Description:

    Internal function for receiving data from a socket

Arguments:

    pcb - Points to the protocol control block
    recvreq - Describes the receive user request

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    WSAOVERLAPPED* overlapped;
    WSAOVERLAPPED overlappedTemp;

    if (IsTcb(pcb)) {
        if (!IsPcbConnected(pcb)) {
            return NETERR(WSAENOTCONN);
        }
    } else {
        if (!IsPcbBound(pcb)) {
            return NETERR(WSAEINVAL);
        }
    }

    if (IsPcbRecvShutdown(pcb)) {
        return !NT_SUCCESS(PcbGetErrStatus(pcb)) ?
                    PcbGetErrStatus(pcb) :
                    NETERR(WSAESHUTDOWN);
    }

    // Since all our sockets are overlapped, we don't
    // enforce the Win32 behavior that the input socket
    // must be a non-overlapped socket.

    if ((overlapped = recvreq->overlapped) != NULL) {
        recvreq->overlappedEvent = GetKernelEventObject(overlapped->hEvent);
        if (!recvreq->overlappedEvent)
            return NETERR(WSASYSCALLFAILURE);
    } else
        recvreq->overlappedEvent = NULL;

    // Check if we have any buffered data waiting to be read
    if (IsPcbRecvBufEmpty(pcb)) {
        if (overlapped) {
            // Overlapped call
            KeClearEvent(recvreq->overlappedEvent);
        } else if (pcb->nonblocking) {
            // Nonoverlapped call and socket is nonblocking:
            // just return WOULDBLOCK error code.
            //
            // Note: For TCP socket, if FIN has been received
            // we should return success with bytesRecv set to 0.
            if (IsDgramPcb(pcb) || !IsFINReceived(pcb))
                return NETERR(WSAEWOULDBLOCK);
        } else {
            // Blocking call - prepare to wait
            recvreq->overlapped = &overlappedTemp;
            recvreq->overlappedEvent = GetPcbWaitEvent(pcb);
            KeClearEvent(recvreq->overlappedEvent);
        }
    }

    status = IsDgramPcb(pcb) ?
                PcbRecvDgram(pcb, recvreq) :
                TcbRecv((TCB*) pcb, recvreq);

    if (status == NETERR_PENDING) {
        if (!overlapped) {
            // A blocking call is still in progress
            WaitKernelEventObject(recvreq->overlappedEvent, pcb->recvTimeout);

            if (overlappedTemp._iostatus == NETERR_PENDING) {
                KIRQL irql = RaiseToDpc();
                if (overlappedTemp._iostatus == NETERR_PENDING) {
                    // We can use recvreq directly here because
                    // blocking recv call is treated as a special case
                    // in PcbQueueOverlappedRecv (in tcp\pcb.c).
                    PcbCompleteOverlappedRecv(recvreq, NETERR_TIMEOUT);
                }
                LowerFromDpc(irql);
            }

            *recvreq->bytesRecv = overlappedTemp._ioxfercnt;
            recvreq->flags = overlappedTemp._ioflags;
            status = overlappedTemp._iostatus;
        }
    } else {
        if (overlapped) {
            // An overlapped call was completed immediately
            overlapped->_ioflags = recvreq->flags;
            overlapped->_ioxfercnt = *recvreq->bytesRecv;
            overlapped->_iostatus = status;

            // It would seem to be a waste to signal the event here.
            // But that's win2k behavior.
            KeSetEvent(recvreq->overlappedEvent, 0, FALSE);
            ObDereferenceObject(recvreq->overlappedEvent);
        }
    }

    return status;
}


int WSAAPI
recv(
    SOCKET s,       
    char* buf,  
    int len,        
    int flags       
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    RECVREQ recvreq;
    DWORD count;
    WinsockApiPrologSockLock_(recv, SOCKET_ERROR);

    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        flags == 0);

    recvreq.buf = (BYTE*) buf;
    recvreq.buflen = len;
    recvreq.flags = flags;
    recvreq.bytesRecv = &count;
    recvreq.fromaddr = NULL;
    recvreq.overlapped = NULL;

    err = SockRecv(pcb, &recvreq);
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(count, SOCKET_ERROR);
}


//
// Verify buffers passed to WSARecv API
// NOTE: we do not support more than 1 receive buffers.
//
INLINE BOOL CheckRecvWsaBuf(WSABUF* bufs, UINT bufcnt) {
    return (bufcnt == 1 &&
            bufs != NULL &&
            (bufs->len > 0 && bufs->buf != NULL || bufs->len == 0));
}


int WSAAPI
WSARecv(
    SOCKET s,                                               
    LPWSABUF bufs,                                     
    DWORD bufcnt,                                    
    LPDWORD bytesRecv,                           
    LPDWORD flags,                                        
    LPWSAOVERLAPPED overlapped,                           
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    RECVREQ recvreq;
    WinsockApiPrologSockLock_(WSARecv, SOCKET_ERROR);

    WinsockApiParamCheck_(
        CheckRecvWsaBuf(bufs, bufcnt) &&
        bytesRecv != NULL &&
        flags != NULL && *flags == 0 &&
        completionproc == NULL);

    recvreq.buf = (BYTE*) bufs->buf;
    recvreq.buflen = bufs->len;
    recvreq.flags = *flags;
    recvreq.bytesRecv = bytesRecv;
    recvreq.fromaddr = NULL;
    recvreq.overlapped = overlapped;

    err = SockRecv(pcb, &recvreq);
    *flags = recvreq.flags;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


int WSAAPI
recvfrom(
    SOCKET s,                   
    char FAR* buf,              
    int len,                    
    int flags,                  
    struct sockaddr* from,  
    int* fromlen            
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    DWORD count;
    INT err;
    WSABUF wsabuf;

    WinsockApiPrologLight_(recvfrom);
    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        (from == NULL ||
         fromlen != NULL && *fromlen >= SOCKADDRLEN));

    wsabuf.len = len;
    wsabuf.buf = buf;
    err = WSARecvFrom(s, &wsabuf, 1, &count, (DWORD*) &flags, from, fromlen, NULL, NULL);
    return (err == NO_ERROR) ? count : SOCKET_ERROR;
}


int WSAAPI
WSARecvFrom(
    SOCKET s,
    LPWSABUF bufs,
    DWORD bufcnt,
    LPDWORD bytesRecv,
    LPDWORD flags,
    struct sockaddr* fromaddr,
    LPINT fromlen,                                        
    LPWSAOVERLAPPED overlapped,                           
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    RECVREQ recvreq;
    WinsockApiPrologSockLock_(WSARecvFrom, SOCKET_ERROR);

    WinsockApiParamCheck_(
        CheckRecvWsaBuf(bufs, bufcnt) &&
        bytesRecv != NULL &&
        (fromaddr == NULL ||
         fromlen != NULL && *fromlen >= SOCKADDRLEN) &&
        flags != NULL && *flags == 0 &&
        completionproc == NULL);

    // Winsock documentation on this call is extremely confusing
    // regarding the correct behavior for connection-oriented sockets
    // Here I've taken the liberty to treat WSARecvFrom the same way
    // as WSARecv for such cases.
    if (IsTcb(pcb)) {
        VERBOSE_("WSARecvFrom called on stream socket!");
    }

    recvreq.buf = (BYTE*) bufs->buf;
    recvreq.buflen = bufs->len;
    recvreq.flags = *flags;
    recvreq.bytesRecv = bytesRecv;
    recvreq.overlapped = overlapped;
    recvreq.fromaddr = (struct sockaddr_in*) fromaddr;

    if (fromaddr) {
        ZeroMem(fromaddr, SOCKADDRLEN);
        *fromlen = SOCKADDRLEN;
        recvreq.fromaddr->sin_family = AF_INET;
    }

    err = SockRecv(pcb, &recvreq);
    *flags = recvreq.flags;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

