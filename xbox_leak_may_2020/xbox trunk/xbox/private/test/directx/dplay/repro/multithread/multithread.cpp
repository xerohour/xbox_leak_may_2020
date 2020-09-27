//-----------------------------------------------------------------------------
// File: dpmemtest.cpp
//
// Desc: Basic executable to measure dynamic DirectPlay memory usage
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define INITGUID
#include <xtl.h>

DWORD WINAPI MyThreadProc(LPVOID *pParam)
{
	SOCKET	LocalSock = (SOCKET) pParam;
	CHAR	LocalBuff[100];

	while(TRUE)
	{
		recv(LocalSock, LocalBuff, sizeof(LocalBuff), 0);
	}

}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: everyone knows what main does...
//-----------------------------------------------------------------------------
void __cdecl main()
{
    SOCKADDR_IN				MySockAddr = { AF_INET };
	WSADATA					WSAData;
	HANDLE					MyThread;
	SOCKET					MySock;
	CHAR					MyBuff[100];

	XnetInitialize(NULL, TRUE);

	WSAStartup(MAKEWORD(2, 2), &WSAData);

	MySock = 0;
	MySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    MySockAddr.sin_port        = htons(12345);
    MySockAddr.sin_addr.s_addr = INADDR_ANY;
	
    bind(MySock, (const struct sockaddr *) &MySockAddr, sizeof(SOCKADDR_IN));

	MyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) MyThreadProc, (LPVOID) MySock, 0, NULL);

	while(TRUE)
	{
		recv(MySock, MyBuff, sizeof(MyBuff), 0);
	}

	WSACleanup();

	XnetCleanup();

	while(TRUE)
	{
		// Loop to infinity
	}
	
	__asm int 3;

}


