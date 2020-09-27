// mmping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "connection.h"
#include "httprequest.h"

void PrintRequestHeaders(CHttpRequest *pHttpRequest)
{
	LPTSTR pBuf = NULL;
	DWORD dwBufLen = 0;

	pHttpRequest->GetRequestHeaders(NULL, &dwBufLen);
	
	if(dwBufLen)
	{
		DWORD dwCurrentIndex = 0;
		INT	n = 0;

		pBuf = new TCHAR[dwBufLen];
		pHttpRequest->GetRequestHeaders(pBuf, &dwBufLen);
		_tprintf(TEXT("Request headers...\n"), dwBufLen);

		for(dwCurrentIndex = 0; dwCurrentIndex < dwBufLen; ++dwCurrentIndex)
		{
			_tprintf(TEXT("   %s\n"), pBuf + dwCurrentIndex);
			dwCurrentIndex += _tcslen(pBuf + dwCurrentIndex);
		}
	}

	pBuf ? delete [] pBuf : 0;
}

void PrintResponseHeaders(CHttpRequest *pHttpRequest)
{
	LPTSTR pBuf = NULL;
	DWORD dwBufLen = 0;

	pHttpRequest->GetResponseHeaders(NULL, &dwBufLen);
	
	if(dwBufLen)
	{
		DWORD dwCurrentIndex = 0;
		INT	n = 0;

		pBuf = new TCHAR[dwBufLen];
		pHttpRequest->GetResponseHeaders(pBuf, &dwBufLen);
		_tprintf(TEXT("Response headers...\n"), dwBufLen);

		for(dwCurrentIndex = 0; dwCurrentIndex < dwBufLen; ++dwCurrentIndex)
		{
			_tprintf(TEXT("   %s\n"), pBuf + dwCurrentIndex);
			dwCurrentIndex += _tcslen(pBuf + dwCurrentIndex);
		}
	}

	pBuf ? delete [] pBuf : 0;
}

int main(int argc, char* argv[])
{
	CConnection *pConnection;
	CHttpRequest *pHttpRequest;
	XMATCH_SESSION SessionInfo;
	DWORD dwExpectedSize = 0, dwActualSize = 0, dwOriginalID = 0, dwModifiedID = 0;
	BYTE *pRetrievedData = NULL;

	pConnection = new CConnection;
	pConnection->AddRef();
	pConnection->EstablishConnection(MMPING_AGENT_NAME, TEXT("darrenan3"), INTERNET_DEFAULT_HTTP_PORT);
	_tprintf(TEXT("Established connection...\n"));

	pHttpRequest = new CHttpRequest(pConnection);
	pHttpRequest->Initialize(TEXT("POST"), TEXT("/xmatch/xmatchhost.srf"));
	_tprintf(TEXT("Initialized request...\n"));

	if(!pHttpRequest->AddHeaders("Content-Type: application/xmatch\r\n"))
	{
		_tprintf(TEXT("AddHeaders failed with %u...\n"), GetLastError());
	}
	else
	{
		_tprintf(TEXT("AddHeaders succeeded...\n"));
	}

	SessionInfo.dwMessageLength = sizeof(XMATCH_SESSION);
	SessionInfo.dwProtocolVersion = 1;
	SessionInfo.dwTitleID = 12345;
	SessionInfo.dwSessionID = 0;
	SessionInfo.dwPublicAvailable = 10;
	SessionInfo.dwPrivateAvailable = 0;
	SessionInfo.dwNumAttributes = 0;

	pHttpRequest->SendRequest(&SessionInfo, sizeof(XMATCH_SESSION));
	_tprintf(TEXT("Sent request...\n"));

	PrintRequestHeaders(pHttpRequest);
	PrintResponseHeaders(pHttpRequest);

	pHttpRequest->GetContentLength(&dwExpectedSize);

	_tprintf(TEXT("Retrieved content length as %u\n"), dwExpectedSize);

	dwActualSize = dwExpectedSize;

	pRetrievedData = new BYTE[dwExpectedSize];
	pHttpRequest->DownloadDataToBuffer(pRetrievedData, &dwActualSize);

	if(dwActualSize == sizeof(DWORD))
	{
		_tprintf(TEXT("Retrieved data:\n"));
		dwOriginalID = *((DWORD *) pRetrievedData);
		_tprintf(TEXT("   SessionID: %u\n"), dwOriginalID);
	}
	else
	{
		_tprintf(TEXT("Retrieved data was too large (%u bytes)\n"), dwActualSize);
		goto Exit;
	}

	delete [] pRetrievedData;

// 888888888888

	delete pHttpRequest;

	pHttpRequest = new CHttpRequest(pConnection);
	pHttpRequest->Initialize(TEXT("POST"), TEXT("/xmatch/docroot/xmatchhost.srf"));
	_tprintf(TEXT("Initialized request...\n"));

	if(!pHttpRequest->AddHeaders("Content-Type: application/xmatch\r\n"))
	{
		_tprintf(TEXT("AddHeaders failed with %u...\n"), GetLastError());
	}
	else
	{
		_tprintf(TEXT("AddHeaders succeeded...\n"));
	}

	SessionInfo.dwMessageLength = sizeof(XMATCH_SESSION);
	SessionInfo.dwProtocolVersion = 1;
	SessionInfo.dwTitleID = 12345;
	SessionInfo.dwSessionID = dwOriginalID;
	SessionInfo.dwPublicAvailable = 11;
	SessionInfo.dwPrivateAvailable = 0;
	SessionInfo.dwNumAttributes = 0;

	pHttpRequest->SendRequest(&SessionInfo, sizeof(XMATCH_SESSION));
	_tprintf(TEXT("Sent request...\n"));

	PrintRequestHeaders(pHttpRequest);
	PrintResponseHeaders(pHttpRequest);

	pHttpRequest->GetContentLength(&dwExpectedSize);

	_tprintf(TEXT("Retrieved content length as %u\n"), dwExpectedSize);

	dwActualSize = dwExpectedSize;

	pRetrievedData = new BYTE[dwExpectedSize];
	pHttpRequest->DownloadDataToBuffer(pRetrievedData, &dwActualSize);

	if(dwActualSize == sizeof(DWORD))
	{
		_tprintf(TEXT("Retrieved data:\n"));
		dwModifiedID = *((DWORD *) pRetrievedData);
		_tprintf(TEXT("   SessionID: %u\n"), dwModifiedID);
	}
	else
	{
		_tprintf(TEXT("Retrieved data was too large (%u bytes)\n"), dwActualSize);
		goto Exit;
	}

	delete [] pRetrievedData;


	// Determine call type...
	// 1) Game host
	// 2) Game search

	// Determine options...
	// 1) Destination address
	// 2) Destination port (optional)
	// 3) Protocol version
	// 4) Title id

	// Game host options...
	// 5) Host address (required for game host, ignored for game search)
	// 6) Open public slots
	// 7) Open private slots
	// 8) Attributes (maybe this will allow you to point to a file that will contain the binary data)

	// Game search options...
	// 9) Query id (optional)
	// 10) Search procedure index
	// 11) Parameters (maybe this will allow you to point to a file that will contain the binary data)

Exit:
	pRetrievedData ? delete [] pRetrievedData : 0;
	pHttpRequest ? delete pHttpRequest : 0;
	pConnection ? delete pConnection : 0;

	return 0;
}

