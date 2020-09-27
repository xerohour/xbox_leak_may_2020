/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixboxsock.cpp

Abstract:

    winsock routines used on SliXbox

Author:

    Jason Gould (a-jasgou) June 2000

--*/

#include "USBXBsock.h"
#include  "draw.h"


SOCKET			ggSock			= INVALID_SOCKET;;
SOCKET			gEventSock	= INVALID_SOCKET;
HANDLE			gDoRead			= INVALID_HANDLE_VALUE;
HANDLE			gHasRead		= INVALID_HANDLE_VALUE;
HANDLE			gDoQuit			= INVALID_HANDLE_VALUE;
HANDLE			gReadThread 	= INVALID_HANDLE_VALUE;
int				gToRecv			= 0;
SOCKET			gReadSock		= INVALID_SOCKET;
char			gReadData[256];
DWORD WINAPI	ReadThread(LPVOID pParam);

#define XBoxEventsPortNumber 53335

SOCKET ssInitEventServer()
{
	sockaddr_in name;
	SOCKET sock;

	// create a socket
	DebugPrint("Getting socket()\n");
	memset(&name,0x00,sizeof(sockaddr_in)); // clear structure
	gEventSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(gEventSock == INVALID_SOCKET) {
		DebugPrint("gEventSock == INVALID_SOCKET), %d\n", WSAGetLastError());
		return 1;
	}

    name.sin_family = AF_INET;
    name.sin_port = htons(XBoxEventsPortNumber);
    name.sin_addr.s_addr = htonl(INADDR_ANY); // (u_long) 0; //(u_long)0x9D370D57?

	DebugPrint("bind()\n");
    if(SOCKET_ERROR == bind(gEventSock, (LPSOCKADDR)&name, sizeof(SOCKADDR))) 
	{
		closesocket(gEventSock);	
		DebugPrint("bind returned SOCKET_ERROR, %d\n", WSAGetLastError());
		return 2;
	}

	DebugPrint("listen()\n");
	if (SOCKET_ERROR == listen(gEventSock, 1)) 
	{
		closesocket(gEventSock);
		DebugPrint("listen returned SOCKET_ERROR, %d\n", WSAGetLastError());
		return 3;
	}

	DebugPrint("USBXBAccept()\n");
	sock = accept(gEventSock, NULL, NULL);
	if(sock == INVALID_SOCKET) 
	{
		closesocket(gEventSock);
		DebugPrint("gSock == INVALID_SOCKET, %d\n", WSAGetLastError());
		return 4;
	}
	DebugPrint("Returning from accept with socket = %x\n", sock);

	return sock;
}

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
    name.sin_port = htons(XPortNumber);
    name.sin_addr.s_addr = htonl(INADDR_ANY); // (u_long) 0; //(u_long)0x9D370D57?

	DebugPrint("bind()\n");
    if(SOCKET_ERROR == bind(ggSock, (LPSOCKADDR)&name, sizeof(SOCKADDR))) {
		DebugPrint("bind returned SOCKET_ERROR, %d\n", WSAGetLastError());
		return 2;
	}

	// now get local IP address and display a banner
	WCHAR Banner[50];
    XNADDR xnaddr;
    UINT iTimeout = 500;    // Five seconds maximum just in case

	while(XNetGetTitleXnAddr(&xnaddr)== 0 && iTimeout-- > 0) Sleep(10);

	wsprintf(Banner,L"IP Address = %d.%d.%d.%d",
				xnaddr.ina.S_un.S_un_b.s_b1,
				xnaddr.ina.S_un.S_un_b.s_b2,
				xnaddr.ina.S_un.S_un_b.s_b3,
				xnaddr.ina.S_un.S_un_b.s_b4
				);
	

	drCls();
	drPrintf(40,40,Banner);
	drShowScreen();

ListenLine:
	DebugPrint("listen()\n");
	if (SOCKET_ERROR == listen(ggSock, 1)) {
		DebugPrint("listen returned SOCKET_ERROR, %d\n", WSAGetLastError());
		goto ListenLine;
		return 3;
	}

	DebugPrint("USBXBAccept()\n");
	sock = accept(ggSock, NULL, NULL);
	if(sock == INVALID_SOCKET) {
		DebugPrint("gSock == INVALID_SOCKET, %d\n", WSAGetLastError());
		return 4;
	}
	DebugPrint("Returning from accept with socket = %x\n", sock);

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
