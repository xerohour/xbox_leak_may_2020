///////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"  //Precompiled header
#include "../connection.h"  //Local
#include <crtdbg.h>  //Debugging

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////

BaseConnection::BaseConnection(HANDLE hShutdownEvent/*=NULL*/, UINT uicWriteBuffers/*=DEFAULTBUFFERS*/, INT icWriteBufferSize/*=DEFAULTBUFFERSIZE*/, INT icReadBufferSize/*=-1*/):
m_hShutdownEvent(hShutdownEvent),
m_uicEvents(1),
m_rgWriteBuffer(NULL),
m_rgWriteOverlapped(NULL),
m_uicWriteBuffers(uicWriteBuffers),
m_icWriteBufferSize(icWriteBufferSize),
m_uiWriteIndex(0),
m_icReadBufferSize(icReadBufferSize),
m_s(INVALID_SOCKET)
{
	//Create array of buffers
	m_rgWriteBuffer=new WSABUF[m_uicWriteBuffers];
	_ASSERT(NULL!=m_rgWriteBuffer);

	//Create array of overlapped objects
	m_rgWriteOverlapped=new WSAOVERLAPPEDEX[m_uicWriteBuffers];
	_ASSERT(NULL!=m_rgWriteOverlapped);

	//Populate array of buffers and initialize overlapped objects
	for (UINT ui=0; ui<m_uicWriteBuffers; ui++){

		//Populate array of buffers
		m_rgWriteBuffer[ui].buf=NULL;
		m_rgWriteBuffer[ui].buf=new CHAR[m_icWriteBufferSize];
		m_rgWriteBuffer[ui].len=0;
		_ASSERT(NULL!=m_rgWriteBuffer[ui].buf);

		//Initialize array of overlapped objects
		m_rgWriteOverlapped[ui].hEvent=NULL;
		m_rgWriteOverlapped[ui].hEvent=WSACreateEvent();
		_ASSERT(NULL!=m_rgWriteOverlapped[ui].hEvent);
		m_rgWriteOverlapped[ui].fPending=false;

	}//endfor 

	if (NULL!=m_hShutdownEvent){
		m_uicEvents=2;
		m_rghEvents[1]=m_hShutdownEvent;
	}//endif

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////

BaseConnection::~BaseConnection(){

	//Cleanup array of buffers and overlapped objects
	for (UINT ui=0; ui<m_uicWriteBuffers; ui++){

		//Cleanup array of buffers
		if (NULL!=m_rgWriteBuffer[ui].buf){
			delete [] m_rgWriteBuffer[ui].buf;
			m_rgWriteBuffer[ui].buf=NULL;
		}//endif

		//Cleanup array of overlapped objects
		if (NULL!=m_rgWriteOverlapped[ui].hEvent){
			WSACloseEvent(m_rgWriteOverlapped[ui].hEvent);
			m_rgWriteOverlapped[ui].hEvent=NULL;
		}//endif
	}//endfor

	//Deallocate array of overlapped objects
	if (NULL!=m_rgWriteOverlapped){
		delete [] m_rgWriteOverlapped;
		m_rgWriteOverlapped=NULL;
	}//endif 

	//Deallocate array of buffers
	if (NULL!=m_rgWriteBuffer){
		delete [] m_rgWriteBuffer;
		m_rgWriteBuffer=NULL;
	}//endif

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// ResetBuffers - Reset overlapped buffers
/////////////////////////////////////////////////////////////////////////////

HRESULT BaseConnection::ResetBuffers(){

	//Reset any existing buffers
	for (UINT ui=0; ui<m_uicWriteBuffers; ui++){
		m_rgWriteBuffer[ui].len=0;
		m_rgWriteOverlapped[ui].fPending=false;
		WSAResetEvent(m_rgWriteOverlapped[ui].hEvent);
	}//endfor
	m_uiWriteIndex=0;

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// SendToFlush - Throw overlapped buffer to winsock
/////////////////////////////////////////////////////////////////////////////

HRESULT BaseConnection::SendToFlush(const struct sockaddr FAR *lpTo/*=NULL*/, int iToLen/*=0*/){

	//Check if connection is established
	if (INVALID_SOCKET==m_s){
		return E_FAIL;
	}//endif

	//Initialize number of bytes sent
	DWORD dwBytesSent=0L;

	//Reset overlapped object's wsa event
	WSAResetEvent(m_rgWriteOverlapped[m_uiWriteIndex].hEvent);

	//Send data
	if (0!=WSASendTo(m_s,&m_rgWriteBuffer[m_uiWriteIndex],1,&dwBytesSent,0,lpTo,iToLen,&m_rgWriteOverlapped[m_uiWriteIndex],NULL)){

		//Pending?
		if (WSA_IO_PENDING!=WSAGetLastError()){
			return E_FAIL;	
		}//endif

		//Mark buffer as submitted
		m_rgWriteOverlapped[m_uiWriteIndex].fPending=true;

		//Set new current buffer
		if (++m_uiWriteIndex>=m_uicWriteBuffers){
			m_uiWriteIndex=0;
		}//endif

	}else{

		//All data sent?
		if (dwBytesSent!=m_rgWriteBuffer[m_uiWriteIndex].len){

			//Should have gone to WSA_IO_PENDING!
			return E_FAIL;

		}//endif

		//Data was sent; set buffer offset back to 0
		m_rgWriteBuffer[m_uiWriteIndex].len=0;

	}//endif

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// SendToFlush - Send to buffer memory
/////////////////////////////////////////////////////////////////////////////

HRESULT BaseConnection::SendTo(const char* rgchBuf, int iLen, BOOL fFlush/*=TRUE*/, DWORD dwTimeout/*=WSA_INFINITE*/, const struct sockaddr FAR *lpTo/*=NULL*/, int iToLen/*=0*/){

	//Check parameters
	if ((NULL==rgchBuf)||(iLen<=0)||(iLen>m_icWriteBufferSize)){
		return E_INVALIDARG;
	}//endif

	while (TRUE){

		//Was buffer pending?
		if (true==m_rgWriteOverlapped[m_uiWriteIndex].fPending){

			//Populate event array
			m_rghEvents[0]=m_rgWriteOverlapped[m_uiWriteIndex].hEvent;

			//Wait for buffer to unlock or for shutdown event to fire
			DWORD dwErr=WSAWaitForMultipleEvents(m_uicEvents,m_rghEvents,false,dwTimeout,false);
			if ((WSA_WAIT_FAILED==dwErr)||(WSA_WAIT_TIMEOUT==dwErr)||(WSA_WAIT_EVENT_0+1==dwErr)){
				return E_FAIL;
			}//endif

			//Reuse the buffer and set the offset back to 0; data was sent
			m_rgWriteOverlapped[m_uiWriteIndex].fPending=false;
			m_rgWriteBuffer[m_uiWriteIndex].len=0;

		}//endif

		//Can we write to this buffer or do we need to flush/and or get new buffer?
		if ((INT)m_rgWriteBuffer[m_uiWriteIndex].len+iLen>m_icWriteBufferSize){
			if FAILED(SendToFlush(lpTo,iToLen)){
				return E_FAIL;
			}//endif
			continue;  //check buffer now
		}//endif

		//Copy data to memory buffer
		memcpy(m_rgWriteBuffer[m_uiWriteIndex].buf+m_rgWriteBuffer[m_uiWriteIndex].len,rgchBuf,iLen);
		m_rgWriteBuffer[m_uiWriteIndex].len+=iLen;

		//Force a flush? (time critical)
		if (TRUE==fFlush){
			if FAILED(SendToFlush(lpTo,iToLen)){
				return E_FAIL;
			}//endif
		}//endif

		break;  //success

	}//endwhile

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Receive - Receives data on an open socket
/////////////////////////////////////////////////////////////////////////////

HRESULT BaseConnection::ReceiveFrom(char* rgchBuf, DWORD dwLen, DWORD* pdwLen, DWORD dwTimeout/*=WSA_INFINITE*/,struct sockaddr FAR *lpFrom/*=NULL*/, int* piFromLen/*=0*/){

	//Check parameters
	if ((NULL==rgchBuf)||(dwLen<0)||(NULL==pdwLen)){
		return E_INVALIDARG;  //Invalid parameters
	}//endif

	//check if connection is established
	if (INVALID_SOCKET==m_s){
		return E_FAIL;
	}//endif

	//wrap w/ wsa buffer
	WSABUF Buffer;
	Buffer.buf=(char*)rgchBuf;
	Buffer.len=dwLen;

	//create io completion event
	WSAOVERLAPPED overlapped;
	overlapped.hEvent=WSACreateEvent();
	if (NULL==overlapped.hEvent){
		return E_FAIL;
	}//endif

	//populate event array
	UINT uicEvents=1;
	WSAEVENT rghEvents[2];
	rghEvents[0]=overlapped.hEvent;
	if (NULL!=m_hShutdownEvent){
		uicEvents=2;
		rghEvents[1]=m_hShutdownEvent;
	}//endif

	HRESULT hr=S_OK;
	DWORD dwFlags=0L;

	*pdwLen=0;

	//receive data
	if (0!=WSARecvFrom(m_s,&Buffer,1,pdwLen,&dwFlags,lpFrom,piFromLen,&overlapped,NULL)){

		//blocks
		if (WSA_IO_PENDING==WSAGetLastError()){
				
			//wait for completion event or shutdown
			DWORD dwErr=WSAWaitForMultipleEvents(uicEvents,rghEvents,false,dwTimeout,false);
			if ((WSA_WAIT_FAILED==dwErr)||(1==dwErr)||(WSA_WAIT_TIMEOUT==dwErr)){
				hr=E_FAIL;
			}//endif

			//success?
			if SUCCEEDED(hr){

				//get bytes received
				DWORD dwFlags=0L;
				if (FALSE==WSAGetOverlappedResult(m_s,&overlapped,pdwLen,FALSE,&dwFlags)){
					hr=E_FAIL;
				}//endif

			}//endif

		}else{

			//other error
			hr=E_FAIL;

		}//endif

	}//endif

	//close event
	if (FALSE==WSACloseEvent(overlapped.hEvent)){
		hr=E_FAIL;
	}//endif

	return hr;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

HRESULT BaseConnection::WaitForAllPendingBuffersToFlush(DWORD dwTimeout){

	//Initiate buffer flush
	if FAILED(SendToFlush()){
		return E_FAIL;
	}//endif

	//Allocate event array
	HANDLE* rghEvents=NULL;
	rghEvents=new HANDLE[m_uicWriteBuffers];
	_ASSERT(NULL!=rghEvents);
	if (NULL==rghEvents){
		return E_FAIL;
	}//endif

	UINT uicEvents=0;
	BOOL fTimeout=FALSE;

	//Count the number of pending buffers
	for (UINT ui=0; ui<m_uicWriteBuffers; ui++){
		if (true==m_rgWriteOverlapped[ui].fPending){
			rghEvents[uicEvents++]=m_rgWriteOverlapped[ui].hEvent;
		}//endif
	}//endfor

	//If we have pending buffers wait for them
	if (uicEvents>0){
		if (WSA_WAIT_TIMEOUT==WSAWaitForMultipleEvents(uicEvents,rghEvents,TRUE,dwTimeout,FALSE)){
			fTimeout=TRUE;
		}//endif
	}//endif

	//Deallocate event array
	if (NULL!=rghEvents){
		delete [] rghEvents;
		rghEvents=NULL;
	}//endif

	//Did we timeout?
	if (TRUE==fTimeout){
		return E_FAIL;
	}//endif

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Constructor 
/////////////////////////////////////////////////////////////////////////////

TCPClientConnection::TCPClientConnection(HANDLE hShutdownEvent/*=NULL*/, UINT uicWriteBuffers/*=DEFAULTBUFFERS*/, INT icWriteBufferSize/*=DEFAULTBUFFERSIZE*/, INT icReadBufferSize/*=-1*/):
BaseConnection(hShutdownEvent,uicWriteBuffers,icWriteBufferSize,icReadBufferSize){

	//create close event
	m_hCloseEvent=WSACreateEvent();

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////

TCPClientConnection::~TCPClientConnection(){

	//Close socket
	Close();

	//close event
	WSACloseEvent(m_hCloseEvent);

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Connect - Establishes a connection on an out bound socket
/////////////////////////////////////////////////////////////////////////////

HRESULT TCPClientConnection::Connect(DWORD dwIP, WORD wPort, DWORD dwTimeout/*=WSA_INFINITE*/){

	//already connected?
	if (INVALID_SOCKET!=m_s){
		return E_FAIL;
	}//endif

	//create socket
	m_s = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET==m_s){
		return E_FAIL;
	}//endif

	//Set the send buffer size to 0; forcing our local buffers to be used preventing any bad-performance copy
	int iBufferSize=0;
	if (0!=setsockopt(m_s,SOL_SOCKET,SO_SNDBUF,(char*)&iBufferSize,sizeof(iBufferSize))){
		return E_FAIL;
	}//endif

	//If user has requested a different receive buffer size, set it
	if (-1!=m_icReadBufferSize){
		iBufferSize=m_icReadBufferSize;
		if (0!=setsockopt(m_s,SOL_SOCKET,SO_RCVBUF,(char*)&iBufferSize,sizeof(iBufferSize))){
			return E_FAIL;
		}//endif
	}//endif 

	//create address
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(wPort);
	sin.sin_addr.s_addr = dwIP;

	//create event array
	WSAEVENT rghEvents[2];
	rghEvents[0]=WSACreateEvent();
	if (NULL==rghEvents[0]){
		return E_FAIL;
	}//endif

	//populate event array
	UINT uicEvents=1;
	if (NULL!=m_hShutdownEvent){
		uicEvents=2;
		rghEvents[1]=m_hShutdownEvent;
	}//endif

	//register for network event
	if (SOCKET_ERROR==WSAEventSelect(m_s,rghEvents[0],FD_CONNECT)){
		return E_FAIL;
	}//endif

	//connect
	HRESULT hr=S_OK;
	if (SOCKET_ERROR==WSAConnect(m_s,(const SOCKADDR*) &sin, sizeof(sin),NULL,NULL,NULL,NULL)){

		//blocks
		if (WSAEWOULDBLOCK==WSAGetLastError()){

			//wait for network event or shutdown
			DWORD dwErr=WSAWaitForMultipleEvents(uicEvents,rghEvents,false,dwTimeout,false);
			if ((WSA_WAIT_FAILED==dwErr)||(1==dwErr)||(WSA_WAIT_TIMEOUT==dwErr)){
				hr=E_FAIL;
			}//endif

			//success?
			if SUCCEEDED(hr){

				//get network event
				WSANETWORKEVENTS NetworkEvents;
				if (SOCKET_ERROR==WSAEnumNetworkEvents(m_s,rghEvents[0],&NetworkEvents)){
					hr=E_FAIL;
				}//endif

				//success?
				if SUCCEEDED(hr){

					//check network event error code
					if ((((NetworkEvents.lNetworkEvents&FD_CONNECT)==FD_CONNECT)) && (NetworkEvents.iErrorCode[FD_CONNECT_BIT]!=0))
					{
						hr=E_FAIL;
					}//endif

				}//endif

			}//endif

		}else{

			//error other than blocking occurred
			hr=E_FAIL;

		}//endif

	}//endif

	//unregister for network event
	if (SOCKET_ERROR==WSAEventSelect(m_s,rghEvents[0],0)){
		hr=E_FAIL;
	}//endif

	//close event
	if (FALSE==WSACloseEvent(rghEvents[0])){
		hr=E_FAIL;
	}//endif

	//register for socket close event
	WSAResetEvent(m_hCloseEvent);
	if (SOCKET_ERROR==WSAEventSelect(m_s,m_hCloseEvent,FD_CLOSE)){
		hr=E_FAIL;
	}//endif

	//Reset buffers
	if (FAILED(ResetBuffers())){
		hr=E_FAIL;
	}//endif

	return hr;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// WaitForDisconnect - wait for socket disconnect
/////////////////////////////////////////////////////////////////////////////

bool TCPClientConnection::WaitForDisconnect(int iTimeout){

	//create event array
	UINT uicEvents=1;
	WSAEVENT l_rgEvents[2];	
	l_rgEvents[0]=m_hCloseEvent;
	if (NULL!=m_hShutdownEvent){
		uicEvents=2;
		l_rgEvents[1]=m_hShutdownEvent;
	}//endif

	//wait for event or timeout
	DWORD dwErr=WSAWaitForMultipleEvents(uicEvents,l_rgEvents,false,iTimeout,false);
	if ((WSA_WAIT_TIMEOUT!=dwErr)||(1==dwErr)){
		Close();
	}//endif
	
	//if connection is gone set retval to false
	if (INVALID_SOCKET==m_s){
		return false;
	}//endif

	return true;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Close - Closes an established connection
/////////////////////////////////////////////////////////////////////////////

HRESULT TCPClientConnection::Close(bool fHard/*=false*/){

	//Socket closed?
	if (INVALID_SOCKET==m_s){
		return S_FALSE;  //Already closed
	}//endif

	if (true==fHard){
		//Hard disconnect!
		LINGER linger;
		linger.l_onoff=1;
		linger.l_linger=0;
		setsockopt(m_s,SOL_SOCKET,SO_LINGER,(const char *)&linger,sizeof(linger));
	}else{
		bool f=true;
		setsockopt(m_s,SOL_SOCKET,SO_DONTLINGER,(const char *)&f,sizeof(f));
		SendToFlush();
	}//endif

	//Close socket
	closesocket(m_s);

	//Set internal state
	m_s=INVALID_SOCKET;

	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////

UDPConnection::UDPConnection(HANDLE hShutdownEvent/*=NULL*/, UINT uicWriteBuffers/*=DEFAULTBUFFERS*/, INT icWriteBufferSize/*=DEFAULTBUFFERSIZE*/, INT icReadBufferSize/*=-1*/):
BaseConnection(hShutdownEvent,uicWriteBuffers,icWriteBufferSize,icReadBufferSize)
{
	//Socket setup
	m_s = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_s){
		_ASSERT(INVALID_SOCKET!=m_s);
		return;
	}//endif

	//Set the send buffer size to 0; forcing our local buffers to be used preventing any bad-performance copy
	INT iBufferSize=0;
	if (0!=setsockopt(m_s,SOL_SOCKET,SO_SNDBUF,(char*)&iBufferSize,sizeof(iBufferSize))){
		_ASSERT(0);
		return;
	}//endif

	//If user has requested a different receive buffer size, set it
	if (-1==m_icReadBufferSize){
		iBufferSize=m_icReadBufferSize;
		if (0!=setsockopt(m_s,SOL_SOCKET,SO_RCVBUF,(char*)&iBufferSize,sizeof(iBufferSize))){
			_ASSERT(0);
			return;
		}//endif
	}//endif 

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////

UDPConnection::~UDPConnection(){

	//Hard disconnect!
	LINGER linger;
	linger.l_onoff=1;
	linger.l_linger=0;
	setsockopt(m_s,SOL_SOCKET,SO_LINGER,(const char *)&linger,sizeof(linger));
	closesocket(m_s);

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Bind - Bind socket to port
/////////////////////////////////////////////////////////////////////////////

HRESULT UDPConnection::Bind(WORD wPort){

	//Socket address
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(wPort);
	sin.sin_addr.s_addr = INADDR_ANY;

	//Bind socket
	if (SOCKET_ERROR==bind(m_s,(struct sockaddr *)&sin,sizeof(sin))){
		return E_FAIL;  //Unable to bind socket
	}//endif
	
	//Success
	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// InternetAddress - Helper function converting names or ips to DWORD
/////////////////////////////////////////////////////////////////////////////

DWORD InternetAddress(const char* pszHost){

	DWORD dwIP=INADDR_NONE;

	//Get IP address
	dwIP = ::inet_addr(pszHost);
	if (dwIP==INADDR_NONE){
		HOSTENT* pHostEnt= ::gethostbyname(pszHost);
		if (NULL!=pHostEnt){
			dwIP = *((LPDWORD) pHostEnt->h_addr);
		}//endif
	}//endif

	return dwIP;  //Return IP address

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// WinsockUP - Helper function to bring up winsock layer
/////////////////////////////////////////////////////////////////////////////

HRESULT WinsockUP(){
	
	WORD wVersionRequested;  //Version of winsock to request
	WSADATA wsaData;  //Version of winsock the system supports
	int err;  //WSAStartup return value
 
	//Sets version
	wVersionRequested=MAKEWORD( 2, 2 );
 
	//WSAStartup
	err=WSAStartup(wVersionRequested, &wsaData);
	if (err!=0){
		return E_FAIL;  //Unable to start winsock
	}//endif
  
	//Check version
	if (LOBYTE(wsaData.wVersion)!=2 ||
		HIBYTE(wsaData.wVersion)!=2){
		WSACleanup();
		return E_FAIL;  //Wrong version of winsock 
	}//endif

	//Success
	return S_OK;

}//endmethod

/////////////////////////////////////////////////////////////////////////////
// WinsockDOWN - Helper function to bring down winsock layer
/////////////////////////////////////////////////////////////////////////////

HRESULT WinsockDOWN(){
	
	int err;  //WSACleanup return value

	//WSACleanup
	err=WSACleanup();
	if (err!=0){
		return E_FAIL;  //Unable to bring down winsock
	}//endif

	//Success
	return S_OK;

}//endmethod

