// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "Connection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VOID CALLBACK ConnCallback(HINTERNET hInternet, DWORD *pContext, DWORD dwStatus, LPVOID lpvStatusInfo, DWORD dwStatusInfoLen)
{
	CConnection *pConnection = (CConnection *) pContext;

	switch(dwStatus)
	{
//	case INTERNET_STATUS_RESOLVING_NAME:
//	case INTERNET_STATUS_NAME_RESOLVED:
//	case INTERNET_STATUS_CONNECTING_TO_SERVER:
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		pConnection->m_hConnectEvent ? SetEvent(pConnection->m_hConnectEvent) : 0;
		break;

//	case INTERNET_STATUS_SENDING_REQUEST:
//	case INTERNET_STATUS_REQUEST_SENT:
//	case INTERNET_STATUS_RECEIVING_RESPONSE:
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		pConnection->m_hResponseEvent ? SetEvent(pConnection->m_hResponseEvent) : 0;
		break;
//	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
//	case INTERNET_STATUS_PREFETCH:
//	case INTERNET_STATUS_CLOSING_CONNECTION:
//	case INTERNET_STATUS_CONNECTION_CLOSED:
//	case INTERNET_STATUS_HANDLE_CREATED:
//	case INTERNET_STATUS_HANDLE_CLOSING:
//	case INTERNET_STATUS_DETECTING_PROXY:
//	case INTERNET_STATUS_REQUEST_COMPLETE:
//	case INTERNET_STATUS_REDIRECT:
//	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
//	case INTERNET_STATUS_USER_INPUT_REQUIRED:
//	case INTERNET_STATUS_STATE_CHANGE:
	default:
		break;
	}
}

CConnection::CConnection()
{
	m_hInternet = NULL;
	m_hConnection = NULL;
	m_dwService = INTERNET_SERVICE_HTTP;
	m_fAsync = FALSE;
	m_hConnectEvent = NULL;
	m_hResponseEvent = NULL;
}

CConnection::~CConnection()
{
	if(m_hConnection)
		InternetCloseHandle(m_hConnection);

	if(m_hInternet)
		InternetCloseHandle(m_hInternet);

	if(m_hConnectEvent)
		CloseHandle(m_hConnectEvent);

	if(m_hResponseEvent)
		CloseHandle(m_hResponseEvent);
}

BOOL CConnection::EstablishConnection(LPTSTR szAgentName, LPTSTR szServer, INTERNET_PORT nPort)
{
	DWORD dwFlags = m_fAsync ? INTERNET_FLAG_ASYNC : 0;

	m_hInternet = InternetOpen(szAgentName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, dwFlags | INTERNET_FLAG_DONT_CACHE);
	if(!m_hInternet)
		return FALSE;
	
	if(m_fAsync)
	{
		if(InternetSetStatusCallback(m_hInternet, (INTERNET_STATUS_CALLBACK) ConnCallback) == INTERNET_INVALID_STATUS_CALLBACK)
			return FALSE;

		m_hConnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(!m_hConnectEvent)
			return FALSE;

		m_hResponseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(!m_hResponseEvent)
			return FALSE;
	}

	m_hConnection = InternetConnect(m_hInternet, szServer, nPort, NULL, NULL, m_dwService, 0, m_fAsync ? (DWORD) this : 0);
	if(!m_hConnection)
		return FALSE;

	m_fConnectInitiated = TRUE;

	return TRUE;
}

CConnection::SetService(DWORD dwService)
{
	m_dwService = dwService;
}

CConnection::SetAsync(BOOL fAsync)
{
	m_fAsync = fAsync;
}

BOOL CConnection::WaitForConnect(DWORD dwWaitTime)
{
	DWORD dwWaitResult = WAIT_TIMEOUT;
	BOOL fConnected = FALSE;

	if(!m_hConnectEvent)
		return FALSE;

	dwWaitResult = WaitForSingleObject(m_hConnectEvent, dwWaitTime);
	switch(dwWaitResult)
	{
	case WAIT_TIMEOUT:
		fConnected = FALSE;
	case WAIT_OBJECT_0:
		fConnected = TRUE;
	default:
		fConnected = FALSE;
	}

	return fConnected;
}

BOOL CConnection::WaitForResponse(DWORD dwWaitTime)
{
	DWORD dwWaitResult = WAIT_TIMEOUT;
	BOOL fResponseReceived = FALSE;

	if(!m_hResponseEvent)
		return FALSE;

	dwWaitResult = WaitForSingleObject(m_hResponseEvent, dwWaitTime);
	switch(dwWaitResult)
	{
	case WAIT_TIMEOUT:
		fResponseReceived = FALSE;
	case WAIT_OBJECT_0:
		fResponseReceived = TRUE;
	default:
		fResponseReceived = FALSE;
	}

	return fResponseReceived;
}

CConnection::AddRef()
{
	++m_dwRefCount;
}

CConnection::Release()
{
	--m_dwRefCount;
	if(!m_dwRefCount)
	{
		delete this;
	}
}
