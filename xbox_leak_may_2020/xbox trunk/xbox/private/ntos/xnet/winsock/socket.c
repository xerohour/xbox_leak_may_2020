/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    socket.c

Abstract:

    Implement the following Winsock APIs:
        socket
        closesocket
        shutdown

Revision History:

    06/01/2000 davidx
        Created it.

--*/

#include "precomp.h"

SOCKET WSAAPI
socket(
    IN int af,
    IN int type,
    IN int protocol
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    const WSAPROTOCOL_INFOW* protoinfo;
    PCB* pcb;

    WinsockApiProlog_(socket, INVALID_SOCKET);

    err = MatchTcpipProtocol(af, type, protocol, &protoinfo);
    WinsockApiCheckError_(INVALID_SOCKET);

    if (type == 0) type = protoinfo->iSocketType;
    if (protocol == 0) protocol = protoinfo->iProtocol;

    pcb = PcbCreate(type, protocol, 0);
    if (!pcb) {
        WinsockApiReturnError_(WSAENOBUFS, INVALID_SOCKET);
    }

    // Initialize socket information here
    pcb->protocolFlags = protoinfo->dwServiceFlags1;

    PcbInsertToList(pcb);
    return (SOCKET) pcb;
}


int WSAAPI
closesocket(
    IN SOCKET s
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    WinsockApiPrologSockLock_(closesocket, SOCKET_ERROR);

    err = pcb ? PcbClose(pcb, FALSE) : NETERR_PARAM;
    MapNtStatusToWinsockError_(err);

    // NOTE: PcbClose will always close the socket.
    // So there is no need for us to call SockUnlock here.

    WinsockApiCheckError_(SOCKET_ERROR);
    return NO_ERROR;
}


int WSAAPI
shutdown(
    IN SOCKET s,
    IN int how
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    BYTE flags;
    WinsockApiPrologSockLock_(shutdown, SOCKET_ERROR);

    WinsockApiParamCheck_(
        how == SD_SEND ||
        how == SD_RECEIVE ||
        how == SD_BOTH);

    switch (how) {
    case SD_SEND:
        flags = PCBFLAG_SEND_SHUTDOWN;
        break;

    case SD_RECEIVE:
        flags = PCBFLAG_RECV_SHUTDOWN;
        break;

    default:
        flags = PCBFLAG_SEND_SHUTDOWN|PCBFLAG_RECV_SHUTDOWN;
        break;
    }

    if (IsDgramPcb(pcb)) {
        err = PcbShutdownDgram(pcb, flags);
    } else {
        if (!IsPcbConnected(pcb)) {
            WinsockApiGotoExit_(WSAENOTCONN);
        }
        err = TcbShutdown((TCB*) pcb, flags, TRUE);
    }

    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

