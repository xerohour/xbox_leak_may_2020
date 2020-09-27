/*----------------------------------------------------------------------------
 *  Copyright © 1997 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 10/24/00
 *  Developer: Sean Wohlgemuth
 *----------------------------------------------------------------------------
 */

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#include <stdio.h>
#include "winsock2.h"
#include "svrsim.h"

//#define TRACEDEBUG

#ifdef TRACEDEBUG
#ifdef _XBOX
extern "C" {ULONG DebugPrint(PCHAR Format, ...);}
#else
#define DebugPrint printf
#endif
#else
#define DebugPrint(a)
#endif

//**********
//********** constructor
//**********
svrsim::svrsim():
m_fListening(false),
m_dwcResponse(0l),
m_fNetInit(true)
{
	strcpy(m_rgchResponse,"HTTP/1.1 200 OK\r\nDate: Tue, 24 Oct 2000 20:42:18 GMT\r\nLast-Modified: Tue, 24 Oct 2000 19:19:37 GMT\r\nContent-Length: 0\r\n\r\n");
	m_dwcResponse=strlen(m_rgchResponse);

	InitializeCriticalSection(&m_cs);
	m_shutdown=CreateEventA(NULL,true,false,"listen_shutdown_event");

	//Initialize XNET
	int err=0;
	#ifdef _XBOX
	err=XnetInitialize(NULL,true);
	if (0!=err)
		m_fNetInit=false;
	#endif

	if (true==m_fNetInit){
		//Initialize winsock
		WSADATA wsadata;
		WORD wVersionRequested = MAKEWORD( 2, 2 );
		err = WSAStartup(wVersionRequested, &wsadata);
		if (0!=err){
			m_fNetInit=false;
			#ifdef _XBOX
			XnetCleanup();
			#endif
		}//endif
	}//endif

};//endmethod

//**********
//********** destructor
//**********
svrsim::~svrsim(){

	//Fire shutdown event and give 1 second
	SetEvent(m_shutdown);
	Sleep(1000);

	//Release critical section
	DeleteCriticalSection(&m_cs);

	//Release shutdown event
	CloseHandle(m_shutdown);

	//Cleanup winsock
	WSACleanup();

	//Cleanup XNET
	#ifdef _XBOX
	XnetCleanup();
	#endif

}//endmethod

//**********
//********** public method
//**********
HRESULT svrsim::SetResponse(const char* pszFilename){

	HRESULT hr=S_OK;//return value
	FILE* pf=NULL;//file pointer

	//initialize memory
	memset(m_rgchResponse,0,sizeof(m_rgchResponse));

	//open response file
	pf=fopen(pszFilename,"rb");
	if (NULL==pf){
		return E_INVALIDARG;
	}//endif

	//store response file in memory
	EnterCriticalSection(&m_cs);
	m_dwcResponse=fread(m_rgchResponse,sizeof(char),sizeof(m_rgchResponse),pf);
	if (0==feof(pf)){
		hr=E_FAIL;
		LeaveCriticalSection(&m_cs);
		goto cleanup;
	}//endif
	LeaveCriticalSection(&m_cs);
		
cleanup:

	//cleanup
	if (NULL!=pf){
		fclose(pf);
	}//endif
	pf=NULL;

	//return
	return hr;

}//endmethod

//**********
//********** public method
//**********
HRESULT svrsim::SetResponse(const char* rgchBuffer, DWORD dwcBuffer){

	EnterCriticalSection(&m_cs);
	memcpy(m_rgchResponse,rgchBuffer,dwcBuffer);
	m_dwcResponse=dwcBuffer;
	LeaveCriticalSection(&m_cs);

	return S_OK;
}//endmethod

//**********
//********** public method
//**********
HRESULT svrsim::Listen(){

	//Network initialized properly?
	if (false==m_fNetInit) return E_FAIL;

	//BUGBUG: race condition possible but unlikely
	if (false==m_fListening){
		m_fListening=true;
	}else{
		return E_FAIL;
	}//endif

	SOCKET s;//listen socket
	SOCKET c;//accept socket
	struct sockaddr_in server;//listen struct
	struct sockaddr_in client;//accept struct
	int iLength;//temp length var
	unsigned long ulParam=1l; //ioctlsocket param
	int err=0;//winsock err code
	struct linger l_linger; //linger structure for setting hard close
	HRESULT hr=S_OK;

	//create listen socket
	s=socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == s){
		return E_FAIL;
	}//endif

	//configure listen socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons((short)80);

	//bind socket
	if (SOCKET_ERROR==bind(s,(struct sockaddr*)&server,sizeof(server))){
		hr=E_FAIL;
		goto cleanup;
	}//endif

	//set socket to listen state
	if (SOCKET_ERROR==listen(s,SOMAXCONN)){
		hr=E_FAIL;
		goto cleanup;
	}//endif

	//set socket to non-blocking
	ulParam=1;
	if (SOCKET_ERROR == ioctlsocket(s,FIONBIO,&ulParam)){
		hr=E_FAIL;
		goto cleanup;
	}//endif

	err=0;
	iLength=sizeof(struct sockaddr);
	do{
		//loop accepting connection or until shutdown event is fired
		do{
			//accept connection
			c=accept(s,(struct sockaddr*)&client,&iLength);
			if (INVALID_SOCKET==c)
				err=WSAGetLastError();
			else
				err=0;
		}while((WSAEWOULDBLOCK==err)&&(WAIT_TIMEOUT==WaitForSingleObject(m_shutdown,100)));

		//break out if shutdown was signalled
		if ((INVALID_SOCKET == c)||(WAIT_OBJECT_0==WaitForSingleObject(m_shutdown,0))){
			break;
		}//endif

		//Debug: print out connection name
		#ifdef TRACEDEBUG
		struct hostent* hostptr;
		hostptr=gethostbyaddr((char*)&(client.sin_addr),4,AF_INET);
		DebugPrint("connection: %s\n",(*hostptr).h_name);
		#endif

		//set accept socket to blocking
		ulParam=0;
		if (SOCKET_ERROR == ioctlsocket(c,FIONBIO,&ulParam)){
			hr=E_FAIL;
			goto cleanup;
		}//endif

		//set 2 second linger
		l_linger.l_onoff=1;
		l_linger.l_linger=2;
		if (SOCKET_ERROR == setsockopt(c,SOL_SOCKET,SO_LINGER,(char *)&l_linger,sizeof(l_linger))){
			hr=E_FAIL;
			goto cleanup;
		}//endif

		//process socket
		if FAILED(ProcessClient(&c)){
			hr=E_FAIL;
			break;
		}//endif
	}while(INVALID_SOCKET!=c);

cleanup:

	//cleanup accept socket
	if (INVALID_SOCKET!=c)
		closesocket(c);

	//cleanup listen socket
	if (INVALID_SOCKET!=s)
		closesocket(s);
	
	//Reset event and set listening state
	ResetEvent(m_shutdown);
	m_fListening=false;

	return hr;

}//endmethod
	
//**********
//********** public method
//**********
HRESULT svrsim::Stop(){

	//If not listing then duh!
	if (false==m_fListening){
		return S_OK;
	}//endif

	//Fire shutdown event
	SetEvent(m_shutdown);
		
	return S_OK;
}//endmethod

//**********
//********** private method
//**********
HRESULT svrsim::ProcessClient(SOCKET* ps){

	HRESULT hr=S_OK;//return value

	//validate parameters
	if (NULL==ps){
		return E_INVALIDARG;
	}//endif

	//local dereference
	SOCKET s=*ps;

	//local buffer
	char rgchBuffer[1024];
	int icRecv=0;

	//loop through data and ignore
	while (true){
		icRecv=recv(s,rgchBuffer,sizeof(rgchBuffer),0);

		//socket closed?
		if ((0==icRecv)||(SOCKET_ERROR==icRecv)){
			hr=E_FAIL;
			break;
		}//endif

		//end of data?
		if ('\n'==rgchBuffer[icRecv-1]){
			break;
		}//endif

	}//endif

	//still connected? send response
	if SUCCEEDED(hr){
		EnterCriticalSection(&m_cs);
		send(s,m_rgchResponse,m_dwcResponse,0);
		LeaveCriticalSection(&m_cs);
	}//endif

	//close socket
	closesocket(s);

	return hr;

}//endmethod

#ifndef _XBOX

//**********
//********** entry point
//**********

int __cdecl main(int argc, char *argv[ ]){

	HRESULT hr=S_OK;
	svrsim oSimulator;

	//check for proper number of command line params
	if (argc!=2){
		goto usage;
	}//endif

	//load the response file
	if (0!=_strnicmp(argv[1],"default\0",8))
		hr=oSimulator.SetResponse(argv[1]);

	//listen for connections
	if SUCCEEDED(hr){
		hr=oSimulator.Listen();
	}//endif

	//failure
	if FAILED(hr)
		return -1;

	//success
	return 0;

usage:

	printf("usage: %s responsefile | default\n",argv[0]);
	printf("example: %s default.txt\n",argv[0]); 
	printf("example: %s default\n",argv[0]); 

	return -1;
}//endmethod	
#endif