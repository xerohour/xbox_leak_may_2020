// HttpRequest.cpp: implementation of the CHttpRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "HttpRequest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpRequest::CHttpRequest(CConnection *pConnection)
{
	m_pConnection = pConnection;
	m_pConnection->AddRef();
	m_hHttpRequest = NULL;

}

CHttpRequest::~CHttpRequest()
{
	m_pConnection->Release();
	m_pConnection = NULL;

	if(m_hHttpRequest)
		InternetCloseHandle(m_hHttpRequest);
}

BOOL CHttpRequest::AddHeaders(LPSTR szHeaders)
{

	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;


	return HttpAddRequestHeaders(m_hHttpRequest, szHeaders, -1L, 0);
}

BOOL CHttpRequest::Initialize(LPTSTR szVerb, LPTSTR szObjectName)
{
	if(!m_pConnection->m_hConnection)
		return FALSE;

	if(m_hHttpRequest)
	{
		InternetCloseHandle(m_hHttpRequest);
		m_hHttpRequest = NULL;
	}

	m_hHttpRequest = HttpOpenRequest(m_pConnection->m_hConnection, szVerb, szObjectName, NULL, NULL, NULL, 0, (DWORD) m_pConnection);
	if(!m_hHttpRequest)
		return FALSE;

	return TRUE;
}

BOOL CHttpRequest::SendRequest(LPVOID pData, DWORD dwDataSize)
{
	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return HttpSendRequest(m_hHttpRequest, NULL, 0, pData, dwDataSize);
}

BOOL CHttpRequest::WaitForResponse(DWORD dwWaitTime)
{
	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return m_pConnection->WaitForResponse(dwWaitTime);
}

BOOL CHttpRequest::GetResponseHeaders(LPVOID pBuffer, LPDWORD pdwBufferLen)
{
	DWORD dwHeaderIndex = 0;

	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_RAW_HEADERS, pBuffer, pdwBufferLen, &dwHeaderIndex);
}

BOOL CHttpRequest::QueryDataAvailable(LPDWORD pdwDataAvailable)
{
	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return InternetQueryDataAvailable(m_hHttpRequest, pdwDataAvailable, 0, 0);
}

BOOL CHttpRequest::ReadData(LPVOID pBuffer, DWORD dwBytesToRead, LPDWORD pdwBytesRead)
{
	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return InternetReadFile(m_hHttpRequest, pBuffer, dwBytesToRead, pdwBytesRead);
}

BOOL CHttpRequest::GetContentLength(DWORD *pdwContentLength)
{
	LPTSTR pBuf = NULL;
	DWORD dwBufferSize = 0;
	BOOL fRet = TRUE;
	INT n = 0;

	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_CONTENT_LENGTH, NULL, &dwBufferSize, 0);
	
	if(!dwBufferSize)
		return FALSE;
	
	pBuf = new TCHAR[dwBufferSize];
	HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_CONTENT_LENGTH, pBuf, &dwBufferSize, 0);
	
	n = _ttoi(pBuf);
	
	*pdwContentLength = (DWORD) n;
	
	delete [] pBuf;
	
	return TRUE;
}

BOOL CHttpRequest::GetRequestHeaders(LPVOID pBuffer, LPDWORD pdwBufferLen)
{
	DWORD dwHeaderIndex = 0;

	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	return HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_FLAG_REQUEST_HEADERS | HTTP_QUERY_RAW_HEADERS, pBuffer, pdwBufferLen, &dwHeaderIndex);
}

BOOL CHttpRequest::DownloadDataToBuffer(LPBYTE pBuffer, LPDWORD pdwActualSize)
{
	LPBYTE pReadBuffer = pBuffer, pTempBuffer = NULL;
	DWORD dwBytesAvailable = 0, dwBytesRead = 0, dwTotalBytesRead = 0;
	BOOL fOverflowing = FALSE, fRet = TRUE;

	if(!InternetQueryDataAvailable(m_hHttpRequest, &dwBytesAvailable, 0, 0))
	{
		_tprintf(TEXT("QueryData returned unexpected error %u\n"), GetLastError());
		fRet = FALSE;
		goto Exit;
	}
	
	while(dwBytesAvailable)
	{	
		if(!fOverflowing)
		{
			if(dwTotalBytesRead + dwBytesAvailable > *pdwActualSize)
			{
				// If there's just a small bit of data left, read that last part to our buffer
				if(*pdwActualSize > dwTotalBytesRead)
				{
					dwBytesAvailable = *pdwActualSize - dwTotalBytesRead;

//					_tprintf(TEXT("Reading last %u bytes that will fit into buffer\n"), dwBytesAvailable);

					if(!InternetReadFile(m_hHttpRequest, pReadBuffer, dwBytesAvailable, &dwBytesRead))
					{
						_tprintf(TEXT("QueryData returned unexpected error %u\n"), GetLastError());
						fRet = FALSE;
						goto Exit;
					}

					dwBytesAvailable = 0;
					if(!InternetQueryDataAvailable(m_hHttpRequest, &dwBytesAvailable, 0, 0))
					{
						_tprintf(TEXT("QueryData returned unexpected error %u\n"), GetLastError());
						fRet = FALSE;
						goto Exit;
					}

					continue;
				}
				// If we alrady exhausted our input buffer, then start reading from the
				else
				{
					fOverflowing = TRUE;
					pTempBuffer = new BYTE[dwBytesAvailable]; 
					pReadBuffer = pTempBuffer;
				}
			}
		}
		else
		{
			pTempBuffer = new BYTE[dwBytesAvailable]; 
			pReadBuffer = pTempBuffer;
		}

		dwBytesRead = 0;

		if(!InternetReadFile(m_hHttpRequest, pReadBuffer, dwBytesAvailable, &dwBytesRead))
		{
			_tprintf(TEXT("QueryData returned unexpected error %u\n"), GetLastError());
			fRet = FALSE;
			goto Exit;
		}
		
//		_tprintf(TEXT("Read %u bytes\n"), dwBytesRead);

		dwTotalBytesRead += dwBytesRead;

		// If we're already overflowing, then release the buffer that was used for this read
		if(fOverflowing)
		{
			delete [] pTempBuffer;
			pTempBuffer = NULL;
		}
		// Otherwise, move the buffer pointer to point to the location for the next read
		else
		{
			pReadBuffer += dwBytesRead;
		}

		dwBytesAvailable = 0;
		if(!InternetQueryDataAvailable(m_hHttpRequest, &dwBytesAvailable, 0, 0))
		{
			_tprintf(TEXT("QueryData returned unexpected error %u\n"), GetLastError());
			fRet = FALSE;
			goto Exit;
		}
	}

Exit:

//	_tprintf(TEXT("Read %u bytes total\n"), dwTotalBytesRead);

	pTempBuffer ? delete [] pTempBuffer : 0;
	*pdwActualSize = dwTotalBytesRead;
	return fRet;
}

BOOL CHttpRequest::GetStatusCode(LPDWORD pdwStatusCode)
{
	LPTSTR pBuf = NULL;
	DWORD dwBufferSize = 0;
	BOOL fRet = TRUE;
	INT n = 0;

	if((!m_pConnection->m_hConnection) && (!m_hHttpRequest))
		return FALSE;

	HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_STATUS_CODE, NULL, &dwBufferSize, 0);

	pBuf = new TCHAR[dwBufferSize];
	HttpQueryInfo(m_hHttpRequest, HTTP_QUERY_STATUS_CODE, pBuf, &dwBufferSize, 0);

	n = _ttoi(pBuf);

	*pdwStatusCode = (DWORD) n;

	delete [] pBuf;

	return TRUE;
}
