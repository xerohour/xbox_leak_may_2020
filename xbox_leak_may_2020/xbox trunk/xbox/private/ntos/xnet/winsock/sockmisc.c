/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    sockmisc.c

Abstract:

    Miscellaneous Winsock functions:
        htonl
        htons
        ntohl
        ntohs
        inet_addr
        inet_ntoa
        WSAGetLastError
        WSASetLastError
        WSACreateEvent
        WSACloseEvent
        WSASetEvent
        WSAResetEvent
        WSAWaitForMultipleEvents
        __WSAFDIsSet

Revision History:

    05/30/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// Converts a u_long from host to network byte order
//
u_long WSAAPI htonl(IN u_long hostlong) {
    return HTONL(hostlong);
}

//
// Converts a u_short from host to network byte order 
//
u_short WSAAPI htons(IN u_short hostshort) {
    return HTONS(hostshort);
}

// 
// Converts a u_long from network order to host byte order 
//
u_long WSAAPI ntohl(IN u_long netlong) {
    return NTOHL(netlong);
}

//
// Converts a u_short from network byte order to host byte order
//
u_short WSAAPI ntohs(IN u_short netshort) {
    return NTOHS(netshort);
}

//
// Converts a string containing an (Ipv4) Internet Protocol
// dotted address into a proper address for the IN_ADDR structure.
//
unsigned long WSAAPI inet_addr(IN const char* cp) {
    IPADDR addr;

    WinsockApiPrologLight_(inet_addr);
    WinsockApiParamCheck_(cp != NULL);

    return IpAddrFromString(cp, &addr) ? addr : INADDR_NONE;
}

//
// Converts an (Ipv4) Internet network address into
// a string in Internet standard dotted format.
//
char* WSAAPI inet_ntoa(IN struct in_addr in) {
    WinsockApiProlog_(inet_ntoa, NULL);

    if (!SockAllocThreadbuf(&tlsData->strbuf, 16)) {
        WinsockApiReturnError_(WSAENOBUFS, NULL);
    }

    return IpAddrToString(in.s_addr, tlsData->strbuf.data, tlsData->strbuf.size);
}

//
// Gets the error status for the last operation that failed
//
int WSAAPI WSAGetLastError() {
    return GetLastError();
}

//
// Sets the error code that can be retrieved
// through the WSAGetLastError function
//
void WSAAPI WSASetLastError(IN int error) {
    SetLastError(error);
}

//
// Creates a new event object
//
WSAEVENT WSAAPI WSACreateEvent() {
    return CreateEvent(NULL, TRUE, FALSE, NULL);
}

//
// Closes an open event object handle
//
BOOL WSAAPI WSACloseEvent(IN WSAEVENT hEvent) {
    return CloseHandle(hEvent);
}

//
// Sets the state of the specified event object to signaled
//
BOOL WSAAPI WSASetEvent(IN WSAEVENT hEvent) {
    return SetEvent(hEvent);
}

//
// Resets the state of the specified event object to nonsignaled
//
BOOL WSAAPI WSAResetEvent(IN WSAEVENT hEvent) {
    return ResetEvent(hEvent);
}

//
// Returns either when one or all of the specified event objects
// are in the signaled state, or when the time-out interval expires
//
DWORD WSAAPI
WSAWaitForMultipleEvents(
    IN DWORD cEvents,
    IN const WSAEVENT* lphEvents,
    IN BOOL fWaitAll,
    IN DWORD dwTimeout,
    IN BOOL fAlertable
    )
{
    return WaitForMultipleObjectsEx(
                cEvents,
                lphEvents,
                fWaitAll,
                dwTimeout,
                fAlertable);
}

//
// Determines if a specific socket is a contained in an FD_SET
//
int WSAAPI __WSAFDIsSet(SOCKET fd, fd_set* set) {
    int i = (set->fd_count & 0xffff);
    while (i--){
        if (set->fd_array[i] == fd) return 1;
    }
    return 0;
}

