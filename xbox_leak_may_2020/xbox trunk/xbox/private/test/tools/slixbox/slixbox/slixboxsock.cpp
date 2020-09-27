/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixboxsock.cpp

Abstract:

    winsock routines used on SliXbox

Author:

    Jason Gould (a-jasgou) June 2000

--*/

#include "slixboxsock.h"


SOCKET ggSock;

HANDLE			gDoRead			= INVALID_HANDLE_VALUE;
HANDLE			gHasRead		= INVALID_HANDLE_VALUE;
HANDLE			gDoQuit			= INVALID_HANDLE_VALUE;
HANDLE			gReadThread 	= INVALID_HANDLE_VALUE;
int				gToRecv			= 0;
SOCKET			gReadSock		= INVALID_SOCKET;
char			gReadData[256];
DWORD WINAPI ReadThread(LPVOID pParam);


SOCKET ssInitServer()
{
	SOCKET sock;
    XNetStartupParams  XNetParams;
	WSADATA wsadata;
	sockaddr_in name;

	DebugPrint("XNetStartup()\n");
    ZeroMemory(&XNetParams, sizeof(XNetParams));
    XNetParams.cfgSizeOfStruct = sizeof(XNetParams);
    XNetParams.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	XNetStartup(&XNetParams);
	DebugPrint("WSAStartup(2, ..)\n");
	WSAStartup(2, &wsadata);

	DebugPrint("Getting socket()\n");
	ggSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(ggSock == INVALID_SOCKET) {
		DebugPrint("ggSock == INVALID_SOCKET), %d\n", WSAGetLastError());
		return 1;
	}

    name.sin_family = AF_INET;
    name.sin_port = 53331;
    name.sin_addr.s_addr = htonl(INADDR_ANY); // (u_long) 0; //(u_long)0x9D370D57?

	DebugPrint("bind()\n");
    if(SOCKET_ERROR == bind(ggSock, (LPSOCKADDR)&name, sizeof(SOCKADDR))) {
		DebugPrint("bind returned SOCKET_ERROR, %d\n", WSAGetLastError());
		return 2;
	}

ListenLine:
	DebugPrint("listen()\n");
	if (SOCKET_ERROR == listen(ggSock, 1)) {
		DebugPrint("listen returned SOCKET_ERROR, %d\n", WSAGetLastError());
		goto ListenLine;
		return 3;
	}

	DebugPrint("accept()\n");
	sock = accept(ggSock, NULL, NULL);
	if(sock == INVALID_SOCKET) {
		DebugPrint("gSock == INVALID_SOCKET, %d\n", WSAGetLastError());
		return 4;
	}

/*	DebugPrint("ReadThread()\n");
	DWORD ThreadID;
	gDoRead  = CreateEvent(NULL, TRUE, FALSE, NULL);
    gHasRead = CreateEvent(NULL, TRUE, FALSE, NULL);
	gDoQuit  = CreateEvent(NULL, TRUE, FALSE, NULL);
	gReadThread   = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadID);
*/
	return sock;
}

/*
int ssSend(SOCKET hsock, TRANSFERDATA * sockdata, int tosend)
{
	tosend = sockdata->size;
	int sent = 0;
	int temp;
	while(sent < tosend) {
		temp = send(hsock, &(sockdata->all[sent]), tosend - sent, 0);
		if(temp == SOCKET_ERROR || temp <= 0) {
			DebugPrint("send error: %d\n", WSAGetLastError());
			return 0;
		}
		sent += temp;
	}
	return sent;
}
*/

/*
int ssCloseTCP(SOCKET hsock, TRANSFERDATA * inbuf, int len)
{
	int ret;
	int bytesdone = 0;

	shutdown(hsock, 1);
	ret = 1;
	if(inbuf) {
		if(SOCKET_ERROR == recv(hsock, (char*)inbuf, len, 0)) {
			DebugPrint("recv error: %d\n", WSAGetLastError());
		}
	}
	
	SetEvent(gDoQuit);
//	if(WaitForSingleObject(gHasRead, 100))
//		TerminateThread(gReadThread, 1);

	ret = closesocket(hsock);

	WSACleanup();
	XnetCleanup();	
	return ret;
}
*/

/*
int ssRecv(SOCKET hsock, TRANSFERDATA  * sockdata, int maxrecv, int timeout)
{
	int received = 0;
	gToRecv = maxrecv;
	gReadSock = hsock;
	SetEvent(gDoRead);
	ResetEvent(gHasRead);
	if(WAIT_OBJECT_0 == WaitForSingleObject(gHasRead, timeout)) {
		memcpy((void*)sockdata, (void*)gReadData, sizeof(sockdata));
		return sockdata->size;
	}
	return 0;
}
*/
/*
DWORD WINAPI ReadThread(LPVOID pParam)
{
	int received = 0;
	int temp;
	int torecv;
	SOCKET hsock;
	
	HANDLE events[] = {gDoRead, gDoQuit, gHasRead};

	while(1) {
//		temp = WaitForMultipleObjects(2, events, FALSE, INFINITE);
//		if(WAIT_OBJECT_0 + 1 == temp) { //if we're supposed to quit...
//			SetEvent(gHasRead);
//			break;	//quit
//		}
//		DebugPrint("gDoRead received.\n");
//		ResetEvent(gDoRead);
		memset(gReadData, 0, sizeof(gReadData));

		torecv = gToRecv;
		hsock = gReadSock;

		received = 0;
		do {
			temp = recv(hsock, &gReadData[received], torecv - received, 0);
			DebugPrint("recv() returned %d!", temp);
			torecv = gReadData[0];
			if(temp == 0 || temp == SOCKET_ERROR) {
				listen(hsock, 1);
				accept(hsock, NULL, NULL);
			}
			received += temp;
		} while (received < torecv);
		SetEvent(gHasRead);
	}
	return 0;
}
*/
