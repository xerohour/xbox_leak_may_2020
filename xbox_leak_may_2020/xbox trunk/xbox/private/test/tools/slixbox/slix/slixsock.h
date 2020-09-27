/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    log.cpp

Abstract:

    declares networking functions for Slix2k to communicate with slixbox

Author:

    Jason Gould (a-jasgou) June 2000

--*/
//SlixSock.h, declares functions for sending data for SliX-Win2k

#ifndef __slixsock_h_
#define __slixsock_h_

//header in c:\, because it is shared between XBox and Hardware projects...
//it sets up a bunch of structures for data formatting for transmitting stuff
//#include "\slixtransfer.h" 

#include "..\slixtransfer.h" 

#include <winsock2.h>

SOCKET ssConnectTCP(char * szDestination, char * szService);  //log on to the server at szDestination, using szService
int ssCloseTCP(SOCKET hsock, TRANSFERDATA * inbuf, int * len = NULL);  //log off of the server
int ssSend(SOCKET hsock, TRANSFERDATA * sockdata, int tosend); //send "tosend" bytes from sockdata through hsock
int ssRecv(SOCKET hsock, TRANSFERDATA * sockdata, int maxrecv, int timeout = RECV_TIMEOUT);//recv at most "maxrecv" bytes to sockdata through hsock

DWORD WINAPI ReadThread(LPVOID pParam);
#endif
