// HttpRequest.h: interface for the CHttpRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(HTTPREQUEST_H)
#define HTTPREQUEST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "connection.h"

class CHttpRequest  
{
public:
	BOOL GetStatusCode(LPDWORD pdwStatusCode);
	BOOL DownloadDataToBuffer(LPBYTE pBuffer, LPDWORD pdwActualSize);
	BOOL GetRequestHeaders(LPVOID pBuffer, LPDWORD pdwBufferLen);
	BOOL GetContentLength(DWORD *pdwContentLength);
	BOOL ReadData(LPVOID pBuffer, DWORD dwBytesToRead, LPDWORD pdwBytesRead);
	BOOL QueryDataAvailable(LPDWORD pdwDataAvailable);
	BOOL GetResponseHeaders(LPVOID pBuffer, LPDWORD pdwBufferLen);
	BOOL WaitForResponse(DWORD dwWaitTime);
	BOOL SendRequest(LPVOID pData, DWORD dwDataSize);
	BOOL AddHeaders(LPSTR szHeaders);
	BOOL Initialize(LPTSTR szVerb, LPTSTR szObjectName);
	CHttpRequest(CConnection *pConnection);
	virtual ~CHttpRequest();

private:
	HINTERNET m_hHttpRequest;
	CConnection * m_pConnection;
};

#endif // !defined(HTTPREQUEST_H)
