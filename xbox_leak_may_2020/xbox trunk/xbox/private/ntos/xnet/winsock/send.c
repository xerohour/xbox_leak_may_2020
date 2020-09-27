/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    send.c

Abstract:

    Implementation of data transmission related Winsock APIs:
        send
        sendto
        WSASend
        WSASendTo

Revision History:

    06/02/2000 davidx
        Created it.

--*/

#include "precomp.h"

// Disable unreferenced label warning
#pragma warning(disable: 4102)


PRIVATE NTSTATUS
SockSend(
    PCB* pcb,
    SENDREQ* sendreq
    )

/*++

Routine Description:

    Internal function for sending out data from a socket

Arguments:

    pcb - Points to the protocol control block
    sendreq - Points to send user request information

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    WSAOVERLAPPED* overlapped;

    if (!IsPcbConnected(pcb) && !sendreq->toaddr)
        return NETERR(WSAENOTCONN);

    if (IsPcbSendShutdown(pcb)) {
        return !NT_SUCCESS(PcbGetErrStatus(pcb)) ?
                    PcbGetErrStatus(pcb) :
                    NETERR(WSAESHUTDOWN);
    }

    if ((overlapped = sendreq->overlapped) != NULL) {
        sendreq->overlappedEvent = GetKernelEventObject(overlapped->hEvent);
        if (!sendreq->overlappedEvent)
            return NETERR(WSASYSCALLFAILURE);
    }

    // Check if we have room in the send buffer
    if (IsPcbSendBufFull(pcb)) {
        if (overlapped) {
            // Queue the overlapped send request.
            KeClearEvent(sendreq->overlappedEvent);
            status = PcbQueueOverlappedSend(pcb, sendreq);

            // The overlapped request was successfully queued up
            if (status == NETERR_PENDING)
                return status;

            // The overlapped send request wasn't queued
            // because of an error.
            if (!NT_SUCCESS(status)) goto exit;

            // The send buffer has opened up and
            // the overlapped send request wasn't queued.
        } else if (pcb->nonblocking || HasOverlappedSend(pcb)) {
            return NETERR_WOULDBLOCK;
        } else {
            status = PcbWaitForEvent(pcb, PCBEVENT_WRITE, pcb->sendTimeout);
            if (!NT_SUCCESS(status)) return status;
        }
    }

    status = IsDgramPcb(pcb) ?
                PcbSendDgram(pcb, sendreq) :
                TcbSend((TCB*) pcb, sendreq);

exit:
    if (overlapped) {
        // An overlapped call was completed immediately
        overlapped->_ioflags = 0;
        overlapped->_ioxfercnt = sendreq->sendtotal;
        overlapped->_iostatus = status;

        // It would seem to be a waste to signal the event here.
        // But that's win2k behavior.
        KeSetEvent(sendreq->overlappedEvent, 0, FALSE);
        ObDereferenceObject(sendreq->overlappedEvent);
    }

    return status;
}


int WSAAPI
send(
    SOCKET s,              
    const char* buf,  
    int len,               
    int flags              
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WSABUF wsabuf;
    SENDREQ sendreq;
    WinsockApiPrologSockLock_(send, SOCKET_ERROR);

    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        (flags  == 0));

    wsabuf.len = len;
    wsabuf.buf = (char*) buf;
    sendreq.overlapped = NULL;
    sendreq.bufs = &wsabuf;
    sendreq.bufcnt = 1;
    sendreq.sendtotal = len;
    sendreq.toaddr = NULL;

    err = SockSend(pcb, &sendreq);
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(sendreq.sendtotal, SOCKET_ERROR);
}


//
// Count the total number of bytes to send
//
INLINE UINT SockCountSendTotal(WSABUF* bufs, UINT bufcnt) {
    UINT total = 0;
    while (bufcnt--) {
        total += bufs->len;
        bufs++;
    }
    return total;
}

INLINE BOOL SockCheckSendWsaBuf(WSABUF* bufs, UINT bufcnt) {
    if (bufs == NULL || bufcnt == 0)
        return FALSE;

    while (bufcnt--) {
        if (bufs->len && bufs->buf == NULL)
            return FALSE;
        bufs++;
    }
    return TRUE;
}

int WSAAPI
WSASend(
    SOCKET s,
    LPWSABUF bufs,
    DWORD bufcnt,
    LPDWORD bytesSent,
    DWORD flags, 
    LPWSAOVERLAPPED overlapped,                           
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    SENDREQ sendreq;
    WinsockApiPrologSockLock_(WSASend, SOCKET_ERROR);

    WinsockApiParamCheck_(
        SockCheckSendWsaBuf(bufs, bufcnt) &&
        bytesSent != NULL &&
        flags == 0 &&
        completionproc == NULL);

    sendreq.overlapped = overlapped;
    sendreq.bufs = bufs;
    sendreq.bufcnt = bufcnt;
    sendreq.sendtotal = SockCountSendTotal(bufs, bufcnt);
    sendreq.toaddr = NULL;

    err = SockSend(pcb, &sendreq);
    if (NT_SUCCESS(err)) *bytesSent = sendreq.sendtotal;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


int WSAAPI
sendto(
    SOCKET s,                        
    const char* buf,            
    int len,                         
    int flags,                       
    const struct sockaddr* to,  
    int tolen                        
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    DWORD count;
    INT err;
    WSABUF wsabuf;

    WinsockApiPrologLight_(sendto);
    WinsockApiParamCheck_(len > 0 && buf != NULL || len == 0);

    wsabuf.len = len;
    wsabuf.buf = (char*) buf;
    err = WSASendTo(s, &wsabuf, 1, &count, flags, to, tolen, NULL, NULL);
    return (err == NO_ERROR) ? count : SOCKET_ERROR;
}


int WSAAPI
WSASendTo(
    SOCKET s,                   
    LPWSABUF bufs,           
    DWORD bufcnt,       
    LPDWORD bytesSent, 
    DWORD flags,             
    const struct sockaddr* toaddr,               
    int tolen,              
    LPWSAOVERLAPPED overlapped,        
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    struct sockaddr_in* sin = (struct sockaddr_in*) toaddr;
    SENDREQ sendreq;
    WinsockApiPrologSockLock_(WSASendTo, SOCKET_ERROR);

    WinsockApiParamCheck_(
        SockCheckSendWsaBuf(bufs, bufcnt) &&
        bytesSent != NULL &&
        flags == 0 &&
        (toaddr == NULL ||
         tolen >= SOCKADDRLEN && sin->sin_family == AF_INET) &&
        completionproc == NULL);

    sendreq.overlapped = overlapped;
    sendreq.bufs = bufs;
    sendreq.bufcnt = bufcnt;
    sendreq.sendtotal = SockCountSendTotal(bufs, bufcnt);

    if (IsTcb(pcb) || toaddr == NULL) {
        // For TCP sockets, WSASendTo is equivalent to WSASend.
        // We simply ignore lpTo and iToLen parameters.
        //
        // Also, if toaddr parameter is NULL, we treat WSASendTo
        // the same way as WSASend.

        sendreq.toaddr = NULL;
        err = SockSend(pcb, &sendreq);
    } else {
        // Must do this check because downstream code
        // doesn't expect the destination address to be 0.
        // Also, we consider sending to UDP port 0 an error.
        if (sin->sin_addr.s_addr == 0 ||
            sin->sin_port == 0 && pcb->type == SOCK_DGRAM) {
            WinsockApiGotoExit_(WSAEADDRNOTAVAIL);
        }

        // Is this socket allowed to send broadcast
        // datagrams on this socket?
        if (IS_BCAST_IPADDR(sin->sin_addr.s_addr) && !pcb->broadcast) {
            WinsockApiGotoExit_(WSAEACCES);
        }

        // If the socket is not bound, bind it here
        if (!IsPcbBound(pcb)) {
            err = PcbBind(pcb, 0, 0);
            if (!NT_SUCCESS(err)) {
                MapNtStatusToWinsockError_(err);
                goto exit;
            }
        }

        // Send to the new destination
        sendreq.toaddr = sin;
        err = SockSend(pcb, &sendreq);
    }
        
    if (NT_SUCCESS(err)) *bytesSent = sendreq.sendtotal;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

