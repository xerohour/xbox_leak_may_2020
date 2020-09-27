////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __SOCKCORE_H_
#define __SOCKCORE_H_


#define XASSERT(x) if( !(x) ) _asm int 3

	#include <xtl.h>

#define FD_READ_BIT      0
#define FD_READ          (1 << FD_READ_BIT)

#define FD_WRITE_BIT     1
#define FD_WRITE         (1 << FD_WRITE_BIT)

#define FD_OOB_BIT       2
#define FD_OOB           (1 << FD_OOB_BIT)

#define FD_ACCEPT_BIT    3
#define FD_ACCEPT        (1 << FD_ACCEPT_BIT)

#define FD_CONNECT_BIT   4
#define FD_CONNECT       (1 << FD_CONNECT_BIT)

#define FD_CLOSE_BIT     5
#define FD_CLOSE         (1 << FD_CLOSE_BIT)

#define FD_QOS_BIT       6
#define FD_QOS           (1 << FD_QOS_BIT)

#define FD_GROUP_QOS_BIT 7
#define FD_GROUP_QOS     (1 << FD_GROUP_QOS_BIT)

#define FD_ROUTING_INTERFACE_CHANGE_BIT 8
#define FD_ROUTING_INTERFACE_CHANGE     (1 << FD_ROUTING_INTERFACE_CHANGE_BIT)

#define FD_ADDRESS_LIST_CHANGE_BIT 9
#define FD_ADDRESS_LIST_CHANGE     (1 << FD_ADDRESS_LIST_CHANGE_BIT)

#define FD_MAX_EVENTS    10
#define FD_ALL_EVENTS    ((1 << FD_MAX_EVENTS) - 1)



/////////////////////////////////////////////////////////////////////////////
// CAsyncSocket

class CAsyncSocket
{
protected:
	HANDLE	m_hInstSOCK;
	int		m_nSockTerm;
	HANDLE	m_hSocketWindow;
	SOCKET	m_hSocket;

	void SocketTerm(void);

// Construction
public:
	CAsyncSocket();
	~CAsyncSocket();
	BOOL SocketInit(WSADATA* lpwsaData = NULL);

	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		LPCTSTR lpszSocketAddress = NULL);

	operator SOCKET() const
		{ return m_hSocket; }

	BOOL Attach(SOCKET hSocket, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
	SOCKET Detach();

	BOOL GetPeerName(LPSTR& rPeerAddress, UINT& rPeerPort);
	BOOL GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
		{ return (SOCKET_ERROR != getpeername(m_hSocket, lpSockAddr, lpSockAddrLen)); }

	BOOL GetSockName(LPSTR& rSocketAddress, UINT& rSocketPort);
	BOOL GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
		{ return (SOCKET_ERROR != getsockname(m_hSocket, lpSockAddr, lpSockAddrLen)); }

	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET)
		{ return (SOCKET_ERROR != setsockopt(m_hSocket, nLevel, nOptionName, (LPCSTR)lpOptionValue, nOptionLen)); }
	BOOL GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel = SOL_SOCKET)
		{ return (SOCKET_ERROR != getsockopt(m_hSocket, nLevel, nOptionName, (LPSTR)lpOptionValue, lpOptionLen)); }

//	static CAsyncSocket* PASCAL FromHandle(SOCKET hSocket)
//		{ return LookupHandle(hSocket, FALSE); }

//	static CAsyncSocket* PASCAL LookupHandle(SOCKET hSocket, BOOL f)
//		{ return FALSE; }

	static int PASCAL GetLastError()
		{ return WSAGetLastError(); }


// Operations
public:

	virtual BOOL Accept(CAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);

	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind (const SOCKADDR* lpSockAddr, int nSockAddrLen)
		{ return (SOCKET_ERROR != bind(m_hSocket, lpSockAddr, nSockAddrLen)); }

	virtual void Close();

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
		{ return ConnectHelper(lpSockAddr, nSockAddrLen); }

	BOOL IOCtl(long lCommand, DWORD* lpArgument)
		{ return (SOCKET_ERROR != ioctlsocket(m_hSocket, lCommand, lpArgument)); }

	BOOL Listen(int nConnectionBacklog=5)
		{ return (SOCKET_ERROR != listen(m_hSocket, nConnectionBacklog)); }

	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

	int ReceiveFrom(void* lpBuf, int nBufLen, LPSTR& rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	int ReceiveFrom(void* lpBuf, int nBufLen,	SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0)
		{ return ReceiveFromHelper(lpBuf, nBufLen, lpSockAddr, lpSockAddrLen, nFlags); }

	enum { receives = 0, sends = 1, both = 2 };
	BOOL ShutDown(int nHow = sends)
		{ return (SOCKET_ERROR != shutdown(m_hSocket,nHow)); }

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
	int SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0)
		{ return SendToHelper(lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags); }


	BOOL AsyncSelect(long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

// Overridable callbacks
protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

// Implementation
public:
//	static CAsyncSocket* PASCAL LookupHandle(SOCKET hSocket, BOOL bDead = FALSE);
//	static void PASCAL AttachHandle(SOCKET hSocket, CAsyncSocket* pSocket, BOOL bDead = FALSE);
//	static void PASCAL DetachHandle(SOCKET hSocket, BOOL bDead = FALSE);
//	static void PASCAL KillSocket(SOCKET hSocket, CAsyncSocket* pSocket);
//	static void PASCAL DoCallBack(WPARAM wParam, LPARAM lParam);

	BOOL Socket(int nSocketType=SOCK_STREAM, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

#ifdef _DEBUG
//	virtual void AssertValid() const;
#endif

protected:
	friend class CSocketWnd;

	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);
};

/////////////////////////////////////////////////////////////////////////////
// CSocket

class CSocket : public CAsyncSocket
{
//	DECLARE_DYNAMIC(CSocket);
private:
	CSocket(const CSocket& rSrc);         // no implementation
	void operator=(const CSocket& rSrc);  // no implementation

// Construction
public:
	CSocket();
	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM, LPCTSTR lpszSocketAddress = NULL)
		{ return CAsyncSocket::Create(nSocketPort, nSocketType, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE, lpszSocketAddress); }

// Attributes
public:
	BOOL IsBlocking()
		{ return (m_pbBlocking != NULL); }

//	static CSocket* PASCAL FromHandle(SOCKET hSocket)
//		{ return (CSocket*)CAsyncSocket::LookupHandle(hSocket, FALSE); }

	BOOL Attach(SOCKET hSocket)
		{ return Attach(hSocket); }

// Operations
public:
	void CancelBlockingCall();

// Overridable callbacks
protected:
//	virtual BOOL OnMessagePending();

// Implementation
public:
	int m_nTimeOut;

	virtual ~CSocket();

	static int PASCAL ProcessAuxQueue();

	virtual BOOL Accept(CAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);
	virtual void Close();
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendChunk(const void* lpBuf, int nBufLen, int nFlags);

protected:
	BOOL* m_pbBlocking;
	int m_nConnectError;

	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);
//
//	static void PASCAL AuxQueueAdd(UINT message, WPARAM wParam, LPARAM lParam);
//
//	virtual BOOL PumpMessages(UINT uStopFlag);
//
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//#endif
};


#endif // __SOCKCORE_H_

/////////////////////////////////////////////////////////////////////////////
