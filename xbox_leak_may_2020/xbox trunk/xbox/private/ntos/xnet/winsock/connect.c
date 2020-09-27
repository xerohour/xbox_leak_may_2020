/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    connect.c

Abstract:

    Implementation of connection related Winsock APIs:
        bind
        connect
        listen
        accept
        getsockname
        getpeername

Revision History:

    06/02/2000 davidx
        Created it.

--*/

#include "precomp.h"


int WSAAPI
bind(
    SOCKET s,                          
    const struct sockaddr* name,   
    int namelen                        
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const struct sockaddr_in* sin = (const struct sockaddr_in*) name;
    WinsockApiPrologSockLock_(bind, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen >= SOCKADDRLEN &&
        sin->sin_family == AF_INET &&
        !IS_BCAST_IPADDR(sin->sin_addr.s_addr) &&
        !IS_MCAST_IPADDR(sin->sin_addr.s_addr));

    if (IsPcbBound(pcb)) {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    err = PcbBind(pcb, sin->sin_addr.s_addr, sin->sin_port);
    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


int WSAAPI
connect(
    SOCKET s,                          
    const struct sockaddr* name,  
    int namelen                        
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const struct sockaddr_in* sin = (const struct sockaddr_in*) name;
    IPADDR dstaddr;
    IPPORT dstport;
    WinsockApiPrologSockLock_(connect, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen >= SOCKADDRLEN &&
        sin->sin_family == AF_INET);

    if (IsPcbConnected(pcb) && IsTcb(pcb)) {
        WinsockApiGotoExit_(WSAEISCONN);
    }

    dstaddr = sin->sin_addr.s_addr;
    dstport = sin->sin_port;
    if (IsDgramPcb(pcb)) {
        err = PcbConnectDgram(pcb, dstaddr, dstport);
    } else if (pcb->nonblocking) {
        err = TcbConnect((TCB*) pcb, dstaddr, dstport, FALSE);

        // For nonblocking socket, we'll return WSAEWOULDBLOCK
        // error code but the operation proceeds.
        if (NT_SUCCESS(err)) err = NETERR_WOULDBLOCK;
    } else {
        err = TcbConnect((TCB*) pcb, dstaddr, dstport, FALSE);
        if (NT_SUCCESS(err)) {
            // For blocking socket, we need to wait here for
            // the operation to complete.
            err = PcbWaitForEvent(pcb, PCBEVENT_CONNECT, 0);
            pcb->connectSelected = 1;
        }
    }

    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


int WSAAPI
listen(
    SOCKET s,    
    int backlog  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockApiPrologSockLock_(listen, SOCKET_ERROR);

    WinsockApiParamCheck_(IsTcb(pcb));

    if (IsPcbConnected(pcb)) {
        WinsockApiGotoExit_(WSAEISCONN);
    }

    if (!IsPcbBound(pcb)) {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    err = TcbListen((TCB*) pcb, backlog);
    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


SOCKET WSAAPI
accept(
    SOCKET s, 
    struct sockaddr* addr,  
    int* addrlen  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    TCB* tcb;
    TCB* newtcb = NULL;
    WinsockApiPrologSockLock_(accept, INVALID_SOCKET);

    WinsockApiParamCheck_(
        IsTcb(pcb) &&
        (addr == NULL ||
         addrlen != NULL && *addrlen >= SOCKADDRLEN));

    if (!IsTcpListenState(pcb)) {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    // Check if we have any pending connection requests
    tcb = (TCB*) pcb;
    if (!TcbHasPendingConnReq(tcb)) {
        // For nonblocking socket, return an error code
        // if we don't have any pending connection requests.
        if (tcb->nonblocking) {
            WinsockApiGotoExit_(WSAEWOULDBLOCK);
        }

        // For blocking sockets, wait until there
        // is a connection request.
        err = PcbWaitForEvent(pcb, PCBEVENT_ACCEPT, 0);
    }

    err = TcbAccept(tcb, &newtcb);
    MapNtStatusToWinsockError_(err);

    if (newtcb && addr && addrlen) {
        struct sockaddr_in* sin;

        sin = (struct sockaddr_in*) addr;
        sin->sin_family = AF_INET;
        sin->sin_port = newtcb->dstport;
        sin->sin_addr.s_addr = newtcb->dstaddr;
        ZeroMem(sin->sin_zero, sizeof(sin->sin_zero));
        *addrlen = SOCKADDRLEN;
    }

    WinsockApiExitSockUnlock_((SOCKET) newtcb, INVALID_SOCKET);
}


int WSAAPI
getsockname(
    SOCKET s,                    
    struct sockaddr* name,  
    int* namelen            
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    struct sockaddr_in* sin;
    WinsockApiPrologSockLock_(getsockname, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen != NULL &&
        *namelen >= SOCKADDRLEN);

    if (!IsPcbBound(pcb)) {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    sin = (struct sockaddr_in*) name;
    ZeroMem(sin, SOCKADDRLEN);
    sin->sin_family = AF_INET;
    sin->sin_port = pcb->srcport;
    sin->sin_addr.s_addr = pcb->srcaddr;

    *namelen = SOCKADDRLEN;
    err = NO_ERROR;

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}


int WSAAPI
getpeername(
    SOCKET s,                    
    struct sockaddr* name,  
    int* namelen            
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    struct sockaddr_in* sin;
    WinsockApiPrologSockLock_(getpeername, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen != NULL &&
        *namelen >= SOCKADDRLEN);

    if (!IsPcbConnected(pcb)) {
        WinsockApiGotoExit_(WSAENOTCONN);
    }

    sin = (struct sockaddr_in*) name;
    sin->sin_family = AF_INET;
    sin->sin_port = pcb->dstport;
    sin->sin_addr.s_addr = pcb->dstaddr;
    ZeroMem(sin->sin_zero, sizeof(sin->sin_zero));

    *namelen = SOCKADDRLEN;
    err = NO_ERROR;

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

