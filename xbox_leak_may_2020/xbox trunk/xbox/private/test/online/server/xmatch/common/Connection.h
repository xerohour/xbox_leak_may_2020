// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CONNECTION_H)
#define CONNECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

class CConnection  
{
friend VOID CALLBACK ConnCallback(HINTERNET, DWORD *, DWORD, LPVOID, DWORD);
friend class CHttpRequest;

public:
	Release();
	AddRef();
	BOOL WaitForResponse(DWORD dwWaitTime);
	BOOL WaitForConnect(DWORD dwWaitTime);
	SetAsync(BOOL fAsync);
	SetService(DWORD dwService);
	BOOL EstablishConnection(LPTSTR szAgentName, LPTSTR szServer, INTERNET_PORT nPort);
	CConnection();
	virtual ~CConnection();

private:
	BOOL m_fConnectInitiated;
	HANDLE m_hResponseEvent;
	HANDLE m_hConnectEvent;
	DWORD m_dwService;
	DWORD m_dwRefCount;
	HINTERNET m_hConnection;
	HINTERNET m_hInternet;
	BOOL m_fAsync;
};

#endif // !defined(CONNECTION_H)
