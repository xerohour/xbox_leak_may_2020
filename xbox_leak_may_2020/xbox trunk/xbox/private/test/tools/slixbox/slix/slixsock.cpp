/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixsock.cpp

Abstract:

    network routines used by slix2k

Author:

    Jason Gould (a-jasgou) June 2000

--*/
#include <windowsx.h>
#include <winsock2.h>

#include "slixsock.h"

#include <stdio.h>



//HANDLE			gDoRead			= INVALID_HANDLE_VALUE;	//event, tells read thread to read
//HANDLE			gHasRead		= INVALID_HANDLE_VALUE;	//event, tells main thread read has occurred
//HANDLE			gDoQuit			= INVALID_HANDLE_VALUE;	//event, tells read thread to quit
//HANDLE			gReadThread 	= INVALID_HANDLE_VALUE;	//handle to read thread
//int				gToRecv			= 0;					//num of bytes for read thread to read
//SOCKET			gReadSock		= INVALID_SOCKET;		//socket for read thread to read from
//char			gReadData[256];							//buffer to read data to



/*************************************************************************************
Function:   GetAddr
Purpose:	get an ip address from a text ip address or computer name
Params:     szHost --- pointer to string that contains host name or "#.#.#.#" format
Return:     0 if it failed, non-zero if an address was found
*************************************************************************************/
unsigned long GetAddr(char * szHost)
{
	unsigned long ret = INADDR_ANY;
	LPHOSTENT host;
	if(!*szHost)
		return ret;
	ret = inet_addr(szHost);	//check first for "#.#.#.#" format...
	if((ret == INADDR_NONE) && strcmp(szHost, "255.255.255.255"))	//if that didn't work,
	{
		host = gethostbyname(szHost);	//try to get it by name...
		if(!host) {						//if that didn't work,
			return INADDR_ANY;				//return 0
		}
		ret = *(unsigned long *)(host->h_addr_list[0]);	//return the address from hostname
	}	
	return ret;	//return the address from "#.#.#.#" format
}

/*************************************************************************************
Function:   GetPort
Purpose:	get the port number of a particular service
Params:     service --- either a number in a string, or a service name
Return:     the port number of the service
*************************************************************************************/
unsigned short GetPort (char * service)
{
	LPSERVENT servent;
	if(isdigit(service[0]))
		return atoi(service);

	servent = getservbyname(service, "tcp");
	if(!servent)
		return 0;
	return servent->s_port;
}

void DoBox(char * text, ...) {
	char buf[256];
	vsprintf(buf, text, (char*)((&text) + 1));
	MessageBox(NULL, buf, buf, MB_OK);
}

/*************************************************************************************
Function:   ssConnectTCP
Purpose:	initialize the tcp connections, connect to the server, start read thread
Params:     szDestination --- the computer name to connect to
            szService --- the service to use (for slix, should be "53331"
Return:     INVALID_SOCKET if it failed, socket value if it worked
*************************************************************************************/
SOCKET ssConnectTCP(char * szDestination, char * szService)
{
	SOCKET hsock;
	WSADATA wsadata;
	int ret;
	SOCKADDR_IN RemoteName;

	ret = WSAStartup(2, &wsadata);	//initialize Winsock
	if(ret != 0) {
		DoBox("WSAStartup, error %d", WSAGetLastError());
		return 0;
	}

	hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//create a socket
	if(hsock==INVALID_SOCKET) {
		DoBox("socket, error %d", WSAGetLastError());
		return 0;
	}

	RemoteName.sin_addr.S_un.S_addr = GetAddr(szDestination);	//get address of server
	if(RemoteName.sin_addr.S_un.S_addr == INADDR_NONE) {
		DoBox("GetAddr, error %d", WSAGetLastError());
//		closesocket(hsock);
		return 0;
	}

	RemoteName.sin_port = GetPort(szService);	//get port for the address
	if(RemoteName.sin_port==0) {
		DoBox("GetPort, error %d", WSAGetLastError());
//		closesocket(hsock);
		return 0;
	}
    RemoteName.sin_port = 53331;
	
	RemoteName.sin_family = PF_INET;
	ret = connect(hsock, (LPSOCKADDR)&RemoteName, sizeof(SOCKADDR));  //connect to server
	if(ret == SOCKET_ERROR) {
		DoBox("connect, error %d", WSAGetLastError());
//		closesocket(hsock);
		return 0;
	}

	int tRuE = TRUE;
	setsockopt(hsock, IPPROTO_TCP, TCP_NODELAY, (char*)&tRuE, 1);

	tRuE = 5;
	setsockopt(hsock, IPPROTO_TCP, SO_RCVTIMEO, (char*)&tRuE, 4);


//	DWORD ThreadID;
//	gDoRead  = CreateEvent(NULL, TRUE, FALSE, NULL);	//init DoRead event
  //  gHasRead = CreateEvent(NULL, TRUE, FALSE, NULL);	//init HasRead event
	//gDoQuit  = CreateEvent(NULL, TRUE, FALSE, NULL);	//init DoQuit event
	
//	gReadThread   = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadID);	//begin the read thread
//	/if(gReadThread == INVALID_HANDLE_VALUE) {
//		DoBox("CreateThread, error %d", WSAGetLastError());
//		closesocket(hsock);
//		return 0;
//	}

	return hsock;	
}

/*************************************************************************************
Function:   ssCloseTCP
Purpose:	close the tcp connection, 
Params:     [IN] hsock --- the socket to close
			[OPT OUT] inbuf --- buffer to receive data if there is a transfer left to do
			[OPT IN OUT] len --- (IN) max buffer size, (OUT) num of bytes received
Return:     0 if everything closed properly, non-0 if an error happened
*************************************************************************************/
int ssCloseTCP(SOCKET hsock, TRANSFERDATA * inbuf, int * len)
{
	int ret;
	int bytesdone = 0;

	if(inbuf) {						//if we can,
		ret = (len ? *len : 256);			
		ret = recv(hsock, (char*)inbuf, ret, 0);	//receive the last piece of data...
		if(len) *len = ret;			
	}

	if(WSAIsBlocking()) {			//cancel blocking calls
		WSACancelBlockingCall();	
	}

	shutdown(hsock, 1);				//shut down the socket

//	SetEvent(gDoQuit);				//tell the Read Thread to quit
//	if(WaitForSingleObject(gHasRead, 100))	//give the read thread some time to quit...
//		TerminateThread(gReadThread, 1);	//if it's stuck, terminate it (that sounds cruel, doesn't it?)

	ret = closesocket(hsock);		//close our socket

	WSACleanup();					//and close down winsock, if necessary
	return ret;
}


/*************************************************************************************
Function:   ssSend
Purpose:	send data!
Params:     [IN] hsock --- the socket to send through
			[IN] sockdata --- pointer to the data to send
			[IN] tosend --- the number of bytes from sockdata to send
Return:     number of bytes sent (0 would imply an error then...)
*************************************************************************************/
int ssSend(SOCKET hsock, TRANSFERDATA * sockdata, int tosend)
{
	tosend = sockdata->size;
	int sent = 0;
	int temp;
	while(sent < tosend) {
		temp = send(hsock, &(sockdata->all[sent]), tosend, 0);
		if(temp == SOCKET_ERROR) {
			DoBox("send failed, error %d", WSAGetLastError());
			return 0;
		}
		sent += temp;
	}
	return sent;
}


/*************************************************************************************
Function:   ssRecv
Purpose:	receive data
Params:     [IN]  hsock --- the socket to read from
			[OUT] sockdata --- buffer to receive data into
			[IN]  maxrecv --- the max number of bytes to receive
			[OPT] timeout --- the number of miliseconds to wait
Return:     number of bytes received (should be > 2)
*************************************************************************************/
/*
int ssRecv(SOCKET hsock, TRANSFERDATA * sockdata, int maxrecv, int timeout)
{
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

/*************************************************************************************
Function:   ReadThread
Purpose:	the thread to read stuff from slixbox
Params:     [UNUSED] pParam
Return:     irrelevant
*************************************************************************************/
/*DWORD WINAPI ReadThread(LPVOID pParam)
{
	int received = 0;
	int temp;
	int torecv;
	SOCKET hsock;
	
	HANDLE events[] = {gDoRead, gDoQuit, gHasRead};	//various events we use

	while(1) {
		temp = WaitForMultipleObjects(2, events, FALSE, INFINITE);	//wait for DoRead or DoQuit
		if(WAIT_OBJECT_0 + 1 == temp) {							//if we're supposed to quit...
			SetEvent(gHasRead);									//say that we're quitting
			break;												//quit	
		}
		printf("gDoRead received.\n");

		torecv = gToRecv;									//the amount to receive
		hsock = gReadSock;									//the socket to read from

		memset(gReadData, 0, sizeof(gReadData));			//we're reading, so clean the buffer

		received = 0;
		do {
			temp = recv(hsock, &gReadData[received], torecv - received, 0);	//receive already!
			printf("recv received %d\n", temp);

			torecv = gReadData[0];				//the num of bytes we should have received
			if(temp == 0 || temp == SOCKET_ERROR) {		//if there was a problem, 
				break;									//go back to main loop
			}
			received += temp;					//we received data!
		} while (received < torecv);			//receive until we've got it all
		SetEvent(gHasRead);						//we've read something...
	}

	return 0;		//we will get here if gDoQuit ever gets set.
}
*/
