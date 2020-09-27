/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#define DEFAULTBUFFERSIZE 1024
#define DEFAULTBUFFERS 1

struct WSAOVERLAPPEDEX: public WSAOVERLAPPED {
	bool fPending;
};//endstruct

class BaseConnection{
	
public:

	//Constructor - This form is for establishing a new connection
	BaseConnection(HANDLE hShutdownEvent=NULL, UINT uicWriteBuffers=DEFAULTBUFFERS, INT icWriteBufferSize=DEFAULTBUFFERSIZE, INT icReadBufferSize=-1);

	//Destructor
	~BaseConnection();

	//Wait for all overlapped async buffers to be in non-pending state
	HRESULT WaitForAllPendingBuffersToFlush(DWORD dwTimeout);

protected:

	//Send data down connection
	HRESULT SendTo(const char* rgchBuf, int iLen, BOOL fFlush=TRUE, DWORD dwTimeout=WSA_INFINITE, const struct sockaddr FAR *lpTo=NULL, int iToLen=0);

	//Receive data off the connection
	HRESULT ReceiveFrom(char* rgchBuf, DWORD dwLen, DWORD* pdwLen, DWORD dwTimeout=WSA_INFINITE, struct sockaddr FAR *lpFrom=NULL, int* piFromLen=0);

	//Reset overlapped buffers
	HRESULT BaseConnection::ResetBuffers();

	//Flush buffers to wire
	HRESULT SendToFlush(const struct sockaddr FAR *lpTo=NULL, int iToLen=0);

protected:

	SOCKET m_s; //Overlapped socket
	HANDLE m_hShutdownEvent;  //Shutdown event
	INT m_icReadBufferSize;  //Internal buffer read size

private:

	WSAEVENT m_rghEvents[2]; //Can hold one overlapped event and a shutdown event
	UINT m_uicEvents; //Number of WSA wait event (depends if shutdownevent was provided)

	UINT m_uicWriteBuffers; //Number of write buffers
	INT m_icWriteBufferSize; //Max size of a write buffer
	WSABUF* m_rgWriteBuffer; //Array of WSABUFs
	WSAOVERLAPPEDEX* m_rgWriteOverlapped; //Array of overlapped events
	UINT m_uiWriteIndex; //Current write buffer to write to
};

/////////////////////////////////////////////////////////////////////////////
// Network Connection Wrapper Class
//
// Construct any class with the optional hShutdownEvent parameter so that you 
// can cancel ANY outstanding network operation.  These class methods will
// block until each operation is complete OR the hShutdownEvent has is thrown.
// All blocking methods perform smart sleep states until network action can
// be completed or the hShutdownEvent is thrown.
/////////////////////////////////////////////////////////////////////////////

class TCPClientConnection:public BaseConnection{
	
public:

	//Constructor - This form is for establishing a new connection
	TCPClientConnection(HANDLE hShutdownEvent=NULL, UINT uicWriteBuffers=DEFAULTBUFFERS, INT icWriteBufferSize=DEFAULTBUFFERSIZE, INT icReadBufferSize=0);

	//Destructor
	~TCPClientConnection();

	//Establish connection
	HRESULT Connect(DWORD dwIP, WORD wPortD, DWORD dwTimeout=WSA_INFINITE);

	//Wait for socket disconnect or timeout
	bool WaitForDisconnect(int iTimeout);

	//Close connection
	HRESULT Close(bool fHard=false);
	//Send data down connection

	//Send data down connection
	HRESULT Send(const char* rgchBuf, int iLen, BOOL fFlush=TRUE, DWORD dwTimeout=WSA_INFINITE){
		return SendTo(rgchBuf,iLen,fFlush,dwTimeout);
	}//endmethod

	//Receive data off the connection
	HRESULT Receive(char* rgchBuf, DWORD dwLen, DWORD* pdwLen, DWORD dwTimeout=WSA_INFINITE){
		return ReceiveFrom(rgchBuf,dwLen,pdwLen,dwTimeout);
	}//endmethod

private:

	WSAEVENT m_hCloseEvent; //Close event

};//endclass

/////////////////////////////////////////////////////////////////////////////
// UDPConnection - UDP based connection wrapper class used to send or receive
// data.  Use Bind() method only for establishing a port bind for 
// ReceiveFrom().
/////////////////////////////////////////////////////////////////////////////

class UDPConnection:public BaseConnection{

public:

	//Constructor - use INADDR_ANY for receiving any source on specified port
	UDPConnection(HANDLE hShutdownEvent=NULL, UINT uicWriteBuffers=DEFAULTBUFFERS, INT icWriteBufferSize=DEFAULTBUFFERSIZE, INT icReadBufferSize=0);

	//Destructor
	~UDPConnection();

	//Bind socket to port
	HRESULT Bind(WORD wPort);

	//Send data down connection
	HRESULT SendTo(const char* rgchBuf, int iLen, BOOL fFlush=TRUE, DWORD dwTimeout=WSA_INFINITE, const struct sockaddr FAR *lpTo=NULL, int iToLen=0){
		return BaseConnection::SendTo(rgchBuf,iLen,fFlush,dwTimeout,lpTo,iToLen);
	}//endmethod

	//Receive data off the connection
	HRESULT ReceiveFrom(char* rgchBuf, DWORD dwLen, DWORD* pdwLen, DWORD dwTimeout=WSA_INFINITE, struct sockaddr FAR *lpFrom=NULL, int* piFromLen=0){
		return BaseConnection::ReceiveFrom(rgchBuf,dwLen,pdwLen,dwTimeout,lpFrom,piFromLen);
	}//endmethod

};//endclass

/////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// InternetAddress - Helper function converting names or ips to DWORD
DWORD InternetAddress(const char* pszHost);

// WinsockUP - Helper function to bring up winsock layer
HRESULT WinsockUP();

// WinsockDOWN - Helper function to bring down winsock layer
HRESULT WinsockDOWN();
	
#endif