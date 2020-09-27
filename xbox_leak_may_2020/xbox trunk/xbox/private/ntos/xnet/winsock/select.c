/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    select.c

Abstract:

    Implementation of asynchronous notification related Winsock APIs:
        select
        WSAGetOverlappedResult
        WSACancelOverlappedIO

Revision History:

    06/02/2000 davidx
        Created it.

--*/

#include "precomp.h"

//
// Count the total number of socket handles
//
#define SOCKETS_IN_SET(_set) ((_set) ? ((_set)->fd_count & 0xffff) : 0)

//
// Information about sockets that was passed to the select calls
//
typedef struct _SELECTINFO {
    SOCKET s;
    fd_set* fdset;
    INT eventMasks;
    PCB* pcb;
    INT pcbMasks;
} SELECTINFO;

//
// Select event masks
//
#define SELECT_READ_EVENTS (PCBEVENT_READ|PCBEVENT_ACCEPT|PCBEVENT_CLOSE|PCBEVENT_CONNRESET)
#define SELECT_WRITE_EVENTS (PCBEVENT_WRITE|PCBEVENT_CONNECT)
#define SELECT_EXCEPT_EVENTS (PCBEVENT_CONNRESET)

//
// Number of KWAIT_BLOCKs allocated on the stack for select() calls.
//
#define SELECT_STACK_KWAIT_BLOCKS 3


PRIVATE INT
SockLockSelectSockets(
    fd_set* fdset,
    SELECTINFO* selinfo,
    INT offset,
    INT eventMasks
    )

/*++

Routine Description:

    Lock the socket handles that was passed to the select API

Arguments:

    fdset - Points to the socket set
    selinfo - Points to an array of SELECTINFO structures
        for storing the locked socket information
    eventMasks - Specifies the interested events

Return Value:

    Winsock error code

--*/

{
    INT i, count;

    // Nothing to do if the set is empty
    count = SOCKETS_IN_SET(fdset);

    for (i=0; i < count; i++) {
        SOCKET s = fdset->fd_array[i];
        INT j, k = offset + i;

        selinfo[k].s = s;
        selinfo[k].fdset = fdset;
        selinfo[k].eventMasks = eventMasks;

        // Check to see if the socket is already used
        // in the same select call
        for (j=0; j < k && selinfo[j].s != s; j++)
            ;

        if (j == k) {
            //
            // The socket isn't seen already
            //
            selinfo[k].pcbMasks = eventMasks;
            selinfo[k].pcb = SockLock(s);
            if (!selinfo[k].pcb)
                return GetLastError();
        } else {
            //
            // The socket is already seen
            //
            selinfo[j].pcbMasks |= eventMasks;
            selinfo[k].pcbMasks = 0;
            selinfo[k].pcb = selinfo[j].pcb;
        }
    }

    return NO_ERROR;
}


int WSAAPI
select(
    int nfds,
    fd_set* readfds,
    fd_set* writefds,
    fd_set* exceptfds,
    const struct timeval* timeout  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    SELECTINFO tempinfo;
    PRKEVENT tempevent;
    KWAIT_BLOCK tempWaitBlocks[SELECT_STACK_KWAIT_BLOCKS];
    SELECTINFO* selinfo;
    PRKEVENT* events;
    INT index, rdcnt, rwcnt, selcnt = 0;
    PCB* pcb;
    LARGE_INTEGER waittime;
    LARGE_INTEGER* pwait;
    PKWAIT_BLOCK waitBlockArray = tempWaitBlocks;

    WinsockApiProlog_(select, SOCKET_ERROR);

    // Count the total number of sockets
    // (ignore the input nfds parameter)
    rdcnt = SOCKETS_IN_SET(readfds);
    rwcnt = rdcnt + SOCKETS_IN_SET(writefds);
    nfds = rwcnt + SOCKETS_IN_SET(exceptfds);

    if (nfds == 0) {
        WinsockApiReturnError_(WSAEINVAL, SOCKET_ERROR);
    }

    if (nfds == 1) {
        // Use temporary stack buffers for the special case
        // where there is only one socket. This saves us from
        // two extra memory allocations.
        events = &tempevent;
        selinfo = &tempinfo;
        ZeroMem(selinfo, sizeof(SELECTINFO));
    } else {
        selinfo = (SELECTINFO*) MAlloc0(nfds*sizeof(SELECTINFO));
        events = (PRKEVENT*) MAlloc(nfds*sizeof(PRKEVENT));
        if (!selinfo || !events) {
            nfds = 0;
            WinsockApiGotoExit_(WSAENOBUFS);
        }
    }

    // Lock all the socket handles
    if ((err = SockLockSelectSockets(readfds, selinfo, 0, SELECT_READ_EVENTS)) != 0 ||
        (err = SockLockSelectSockets(writefds, selinfo, rdcnt, SELECT_WRITE_EVENTS)) != 0 ||
        (err = SockLockSelectSockets(exceptfds, selinfo, rwcnt, SELECT_EXCEPT_EVENTS)) != 0) {
        goto exit;
    }

    // Compute the wait time in 100ns unit
    if (timeout) {
        pwait = &waittime;
        waittime.QuadPart = Int32x32To64(timeout->tv_sec, -10000000) +
                            Int32x32To64(timeout->tv_usec, -10);
    } else {
        pwait = NULL;
    }

    // Check if we to wait:
    // if we do, set up the socket event flags
    if (!pwait || pwait->QuadPart) {
        INT waitCount = 0;
        for (index=0; index < nfds; index++) {
            pcb = selinfo[index].pcb;
            if (selinfo[index].pcbMasks) {
                if (PcbCheckSelectEvents(pcb, selinfo[index].pcbMasks, -1)) break;
                events[waitCount++] = GetPcbWaitEvent(pcb);
            }
        }

        if (index == nfds) {
            if (waitCount > SELECT_STACK_KWAIT_BLOCKS) {
                waitBlockArray = (PKWAIT_BLOCK) MAlloc0(waitCount * sizeof(KWAIT_BLOCK));
                if (!waitBlockArray) {
                    WinsockApiGotoExit_(WSAENOBUFS);
                }
            }

            err = KeWaitForMultipleObjects(
                        waitCount,
                        events,
                        WaitAny,
                        UserRequest,
                        UserMode,
                        FALSE,
                        pwait,
                        waitBlockArray);

            if ((err < 0 || err >= waitCount) && err != STATUS_TIMEOUT) {
                WinsockApiGotoExit_(WSAEFAULT);
            }
        }
    }

    // Determine which socket events are ready
    // and return appropriate information

    if (readfds) { FD_ZERO(readfds); }
    if (writefds) { FD_ZERO(writefds); }
    if (exceptfds) { FD_ZERO(exceptfds); }

    for (index=selcnt=0; index < nfds; index++) {
        if (PcbCheckSelectEvents(
                    selinfo[index].pcb,
                    selinfo[index].eventMasks,
                    0)) {
            FD_SET(selinfo[index].s, selinfo[index].fdset);
            selcnt++;
        }
    }

    err = NO_ERROR;

exit:
    for (index=0; index < nfds; index++) {
        pcb = selinfo[index].pcb;
        if (pcb && selinfo[index].pcbMasks) {
            PcbClearSelectEvents(pcb);
            SockUnlock(pcb);
        }
    }
    if (waitBlockArray != tempWaitBlocks) { Free(waitBlockArray); }
    if (selinfo != &tempinfo) { Free(selinfo); }
    if (events != &tempevent) { Free(events); }

    WinsockApiCheckError_(SOCKET_ERROR);
    return selcnt;
}


BOOL WSAAPI
WSAGetOverlappedResult(
    SOCKET s,                      
    LPWSAOVERLAPPED overlapped,  
    LPDWORD byteCount,          
    BOOL fWait,                    
    LPDWORD flags              
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockApiPrologSockLock_(WSAGetOverlappedResult, FALSE);

    WinsockApiParamCheck_(
        overlapped != NULL &&
        overlapped->hEvent != NULL &&
        byteCount != NULL &&
        flags != NULL);

    //
    // Check if we need to wait for the I/O request to complete
    //
    if (overlapped->_iostatus == NETERR_PENDING && fWait) {
        KIRQL irql;
        WaitForSingleObject(overlapped->hEvent, INFINITE);

        irql = RaiseToDpc();
        if (overlapped->_iostatus == NETERR_PENDING) {
            err = overlapped->_ioxfercnt ? NETERR_OK : NETERR_CANCELLED;
            PcbCompleteOverlappedSendRecv(
                (PcbOverlappedReq*) overlapped->_ioreq,
                err);
        }
        LowerFromDpc(irql);
    }

    //
    // If the I/O request was completed,
    // return the completion status information
    //
    if ((err = overlapped->_iostatus) != NETERR_PENDING && NT_SUCCESS(err)) {
        *byteCount = overlapped->_ioxfercnt;
        *flags = overlapped->_ioflags;
    }

    if (err == NETERR_PENDING) {
        WinsockApiGotoExit_(WSA_IO_INCOMPLETE);
    } else {
        MapNtStatusToWinsockError_(err);
    }
    
    WinsockApiExitSockUnlock_(TRUE, FALSE);
}


INT WSAAPI
WSACancelOverlappedIO(
    SOCKET s
    )

/*++

Routine Description:

    Cancel out any outstanding overlapped I/O requests on a socket

Arguments:

    s - Specifies the socket handle

Return Value:

    0 if successful, SOCKET_ERROR if there is an error

--*/

{
    KIRQL irql;
    WinsockApiPrologSockLock_(WSACancelOverlappedIO, SOCKET_ERROR);

    irql = RaiseToDpc();
    PcbClearOverlappedRecvs(pcb, NETERR_CANCELLED);
    PcbClearOverlappedSends(pcb, NETERR_CANCELLED);
    LowerFromDpc(irql);
    WinsockApiGotoExit_(NO_ERROR);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

