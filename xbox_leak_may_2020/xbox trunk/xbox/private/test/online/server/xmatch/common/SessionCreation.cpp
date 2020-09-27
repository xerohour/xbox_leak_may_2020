// SessionCreation.cpp: implementation of the CSessionCreation class.
//
//////////////////////////////////////////////////////////////////////

#include "SessionCreation.h"

//////////////////////////////////////////////////////////////////////
// Helper function
//////////////////////////////////////////////////////////////////////

DWORD CreateDeleteModifySession(LPTSTR szServer, LPTSTR szObject, XMATCH_SESSION *pSessionInfo, DWORD dwInfoSize)
{
	CConnection *pConnection;
	CHttpRequest *pHttpRequest;
	DWORD dwExpectedSize = 0, dwActualSize = 0, dwSessionID = 0, dwStatusCode = 0;
	BYTE *pRetrievedData = NULL;
	BOOL fFailed = FALSE;

	pConnection = new CConnection;
	pConnection->AddRef();
	pConnection->EstablishConnection(MMTEST_AGENT_NAME, szServer, INTERNET_DEFAULT_HTTP_PORT);

	pHttpRequest = new CHttpRequest(pConnection);
	pHttpRequest->Initialize(TEXT("POST"), szObject);
	pHttpRequest->AddHeaders("Content-Type: application/xmatch\r\n");
	pHttpRequest->SendRequest(pSessionInfo, dwInfoSize);
	pHttpRequest->GetStatusCode(&dwStatusCode);

	if(dwStatusCode != 200)
	{
		_tprintf(TEXT("Session creation returned failure (status %u)\n"), dwStatusCode);
		fFailed = TRUE;
	}

	pHttpRequest->GetContentLength(&dwExpectedSize);

	dwActualSize = dwExpectedSize;

	if(dwExpectedSize)
	{
		
		pRetrievedData = new BYTE[dwExpectedSize];
		pHttpRequest->DownloadDataToBuffer(pRetrievedData, &dwActualSize);
		
		// If this is an error return code, display it
		if(fFailed)
		{
			if((dwActualSize != 4) && (dwActualSize != 8))
			{
				_tprintf(TEXT("Received unexpected error message of size %u\n"), dwActualSize);
			}
			else
			{
				switch(*((DWORD *) pRetrievedData))
				{
				case X_REQ_ERROR_INVALID_SESSION_ID:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_SESSION_ID returned\n"));
					break;
				case X_REQ_ERROR_INVALID_TITLE_ID:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_TITLE_ID returned\n"));
					break;
				case X_REQ_ERROR_INVALID_DATA_TYPE:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_DATA_TYPE returned\n"));
					break;
				case X_REQ_ERROR_REQUEST_TOO_SMALL:
					_tprintf(TEXT("X_REQ_ERROR_REQUEST_TOO_SMALL returned\n"));
					break;
				case X_REQ_ERROR_REQUEST_TRUNCATED:
					_tprintf(TEXT("X_REQ_ERROR_REQUEST_TRUNCATED returned\n"));
					break;
				case X_REQ_ERROR_INVALID_SEARCH_REQ:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_SEARCH_REQ returned\n"));
					break;
				case X_REQ_ERROR_INVALID_OFFSET:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_OFFSET returned\n"));
					break;
				case X_REQ_ERROR_INVALID_ATTR_TYPE:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_ATTR_TYPE returned\n"));
					break;
				case X_REQ_ERROR_INVALID_VERSION:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_VERSION returned\n"));
					break;
				case X_REQ_ERROR_OVERFLOW:
					_tprintf(TEXT("X_REQ_ERROR_OVERFLOW returned\n"));
					break;
				case X_REQ_ERROR_INVALID_RESULT_COL:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_RESULT_COL returned\n"));
					break;
				case X_REQ_ERROR_INVALID_STRING:
					_tprintf(TEXT("X_REQ_ERROR_INVALID_STRING returned\n"));
					break;
				default:
					_tprintf(TEXT("Unknown error returned\n"));
					break;
				}
			}
		}
		// If this was a session modification or deletion, then we aren't expecting a new session ID to be returned
		else if((dwInfoSize >= sizeof(XMATCH_SESSION)) && (pSessionInfo->dwSessionID))
		{
			_tprintf(TEXT("Retrieved data was of unexpected size (%u bytes)\n"), dwActualSize);
			if(dwActualSize < 100)
				_tprintf(TEXT("   %s\n"), pRetrievedData);
		}
		// If this was a new session creation, then we're expecting a new session ID
		else
		{
			
			if(dwActualSize == sizeof(DWORD))
			{
				_tprintf(TEXT("Retrieved data:\n"));
				dwSessionID = *((DWORD *) pRetrievedData);
				_tprintf(TEXT("   SessionID: %u\n"), dwSessionID);
				
			}
			else
			{
				_tprintf(TEXT("Retrieved data was of unexpected size (%u bytes)\n"), dwActualSize);
				if(dwActualSize < 100)
					_tprintf(TEXT("   %s\n"), pRetrievedData);
			}
		}
		
		pRetrievedData ? delete [] pRetrievedData : 0;
		pRetrievedData = NULL;
	}

	pRetrievedData ? delete [] pRetrievedData : 0;
	pHttpRequest ? delete pHttpRequest : 0;
	pConnection ? delete pConnection : 0;

	return dwSessionID;

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSessionCreation::CSessionCreation()
{
	m_fMsgLenOverride = FALSE;
	m_fNumAttribOverride = FALSE;
	m_pBuffer = NULL;
	m_dwBufferLen = 0;
	m_dwTotalAttribs = 0;
	m_dwTotalAttribLen = 0;
	m_pAttribList = NULL;
}

CSessionCreation::~CSessionCreation()
{
	PATTRIB_LIST pCurr = m_pAttribList, pTemp = NULL;

	// Release the attributes list
	while(pCurr)
	{
		pTemp = pCurr;
		pCurr = pTemp->pNext;

		pTemp->pAttribBuffer ? delete [] pTemp->pAttribBuffer : 0;
		delete pTemp;
	}

	// Release the request buffer
	m_pBuffer ? delete [] m_pBuffer : 0;
}

BOOL CSessionCreation::Initialize(DWORD dwBufferLen)
{
	PATTRIB_LIST pCurr = m_pAttribList, pTemp = NULL;

	// Release the attributes list
	while(pCurr)
	{
		pTemp = pCurr;
		pCurr = pTemp->pNext;

		pTemp->pAttribBuffer ? delete [] pTemp->pAttribBuffer : 0;
		delete pTemp;
	}

	// Reset the attribute tracking variables
	m_dwTotalAttribs = 0;
	m_dwTotalAttribLen = 0;
	m_pAttribList = NULL;

	// Reset the override variables
	m_fMsgLenOverride = FALSE;
	m_fNumAttribOverride = FALSE;

	// Release the request buffer and allocate a new one
	m_pBuffer ? delete [] m_pBuffer : 0;

	if(!dwBufferLen)
	{
		m_dwBufferLen = 0;
		return TRUE;
	}

	m_pBuffer = new BYTE[dwBufferLen];
	if(!m_pBuffer)
		return FALSE;

	m_dwBufferLen = dwBufferLen;

	// Set default values only if the session request is of valid size
	if(dwBufferLen >= sizeof(XMATCH_SESSION))
	{
		((XMATCH_SESSION *) m_pBuffer)->dwMessageLength = sizeof(XMATCH_SESSION);
		((XMATCH_SESSION *) m_pBuffer)->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
		((XMATCH_SESSION *) m_pBuffer)->dwTitleID = 1;
		((XMATCH_SESSION *) m_pBuffer)->dwSessionID = 0;
		memset(((XMATCH_SESSION *) m_pBuffer)->bHostAddress, 0xAA, 32 * sizeof(BYTE));
		((XMATCH_SESSION *) m_pBuffer)->dwPublicAvailable = 10;
		((XMATCH_SESSION *) m_pBuffer)->dwPrivateAvailable = 0;
		((XMATCH_SESSION *) m_pBuffer)->dwNumAttributes = 0;
	}

	return TRUE;
}
    
BOOL CSessionCreation::SetProtocolVersion(DWORD dwProtocolVersion)
{
	if(!m_pBuffer || m_dwBufferLen < (2 * sizeof(DWORD)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwProtocolVersion = dwProtocolVersion;

	return TRUE;
}

BOOL CSessionCreation::SetMessageLength(DWORD dwMessageLength)
{
	if(!m_pBuffer || m_dwBufferLen < (1 * sizeof(DWORD)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwMessageLength = dwMessageLength;
	m_fMsgLenOverride = TRUE;

	return TRUE;
}

BOOL CSessionCreation::SetTitleID(DWORD dwTitleID)
{
	if(!m_pBuffer || m_dwBufferLen < (3 * sizeof(DWORD)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwTitleID = dwTitleID;

	return TRUE;
}

BOOL CSessionCreation::SetSessionID(DWORD dwSessionID)
{
	if(!m_pBuffer || m_dwBufferLen < (4 * sizeof(DWORD)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwSessionID = dwSessionID;

	return TRUE;
}

BOOL CSessionCreation::SetPublicAvailable(DWORD dwPublicAvailable)
{
	if(!m_pBuffer || m_dwBufferLen < (5 * sizeof(DWORD) + 32 * sizeof(BYTE)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwPublicAvailable = dwPublicAvailable;

	return TRUE;

}

BOOL CSessionCreation::SetPrivateAvailable(DWORD dwPrivateAvailable)
{
	if(!m_pBuffer || m_dwBufferLen < (6 * sizeof(DWORD) + 32 * sizeof(BYTE)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwPrivateAvailable = dwPrivateAvailable;

	return TRUE;
}

BOOL CSessionCreation::SetNumAttributes(DWORD dwNumAttributes)
{
	if(!m_pBuffer || m_dwBufferLen < (7 * sizeof(DWORD) + 32 * sizeof(BYTE)))
		return FALSE;

	((XMATCH_SESSION *) m_pBuffer)->dwNumAttributes = dwNumAttributes;
	m_fNumAttribOverride = TRUE;

	return TRUE;
}

DWORD CSessionCreation::SendRequest(LPTSTR szServer, LPTSTR szObject)
{
	PATTRIB_LIST pCurrentAttrib = NULL;
	PBYTE pAttribIndex = NULL;
	DWORD dwCopiedAttribLen = 0, dwCurrentOffset = 0;

	// Update the header values unless they were overridden
	if(m_dwBufferLen >= sizeof(XMATCH_SESSION))
	{
		if(!m_fNumAttribOverride)
		{
			((XMATCH_SESSION *) m_pBuffer)->dwNumAttributes = m_dwTotalAttribs;
		}

		if(!m_fMsgLenOverride)
		{
			m_dwBufferLen = sizeof(XMATCH_SESSION) + m_dwTotalAttribs * sizeof(DWORD) + m_dwTotalAttribLen;
			((XMATCH_SESSION *) m_pBuffer)->dwMessageLength = m_dwBufferLen;
		}
	}

	// Add each attribute to the final buffer;
	pCurrentAttrib = m_pAttribList;
	for(DWORD i = 0; i < m_dwTotalAttribs; ++i)
	{
		pAttribIndex = m_pBuffer + sizeof(XMATCH_SESSION) + i * sizeof(DWORD);

		dwCurrentOffset = sizeof(XMATCH_SESSION) + m_dwTotalAttribs * sizeof(DWORD) + dwCopiedAttribLen;
		*((DWORD *) pAttribIndex) = dwCurrentOffset;

		memcpy(m_pBuffer + dwCurrentOffset, pCurrentAttrib->pAttribBuffer, pCurrentAttrib->dwAttribLen);
		dwCopiedAttribLen += pCurrentAttrib->dwAttribLen;
		pCurrentAttrib = pCurrentAttrib->pNext;
	}

	return CreateDeleteModifySession(szServer, szObject, (XMATCH_SESSION *) m_pBuffer, m_dwBufferLen);
}

DWORD CSessionCreation::SendRawRequest(LPTSTR szServer, LPTSTR szObject)
{
	return CreateDeleteModifySession(szServer, szObject, (XMATCH_SESSION *) m_pBuffer, m_dwBufferLen);
}

BOOL CSessionCreation::AddAttribute(DWORD dwAttribID, DWORD dwAttribLen, LPBYTE pBuffer)
{
	PATTRIB_LIST pAttribList = NULL;
	PBYTE pAttribIndex = NULL;
	DWORD dwNewHeaderAndOffsetLen = 0, dwNewTotalAttribLen = 0;
	
	if(!m_pBuffer || m_dwBufferLen < sizeof(XMATCH_SESSION))
		return FALSE;

	// Add the size of the length-prefix
	if(dwAttribID & X_ATTRIBUTE_DATATYPE_STRING)
	{
		if(dwAttribLen > MAXWORD)
			return FALSE;
		
		dwNewTotalAttribLen += sizeof(WORD);
	}
	else if(dwAttribID & X_ATTRIBUTE_DATATYPE_BLOB)
	{
		dwNewTotalAttribLen += sizeof(DWORD);
	}
	
	// Add the size of attributes and their attribute ID's
	dwNewTotalAttribLen += m_dwTotalAttribLen + sizeof(DWORD) + dwAttribLen;
	
	// Add the size of the attribute offsets required
	dwNewHeaderAndOffsetLen += sizeof(XMATCH_SESSION) + ((m_dwTotalAttribs + 1) * sizeof(DWORD));
	
	// Verify there is enough room to accomodate the new attribute
	if(dwNewHeaderAndOffsetLen + dwNewTotalAttribLen > m_dwBufferLen)
		return FALSE;
	
	// Allocate a new attribute and copies the attribute data into it
	pAttribList = new ATTRIB_LIST;
	
	// Insertion varies depending on type
	if(dwAttribID & X_ATTRIBUTE_DATATYPE_STRING)
	{
		pAttribList->dwAttribLen = dwAttribLen + 1 * sizeof(DWORD) + 1 * sizeof(WORD);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the string length prefix
		*((WORD *) pAttribIndex) = (WORD) (0x0000FFFF & dwAttribLen);
		pAttribIndex += sizeof(WORD);
		
		// Copy the string value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	else if(dwAttribID & X_ATTRIBUTE_DATATYPE_BLOB)
	{
		pAttribList->dwAttribLen = dwAttribLen + 2 * sizeof(DWORD);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the string length prefix
		*((DWORD *) pAttribIndex) = dwAttribLen;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the string value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	else
	{
		pAttribList->dwAttribLen = dwAttribLen + 1 * sizeof(DWORD);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the integer value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	
	// Insert the attribute entry into the list
	pAttribList->pNext = m_pAttribList;
	m_pAttribList = pAttribList;
	
	// Increment the length tracking variables
	++m_dwTotalAttribs;
	m_dwTotalAttribLen = dwNewTotalAttribLen;
	
	return TRUE;
}

BOOL CSessionCreation::AddPlayerAttribute(DWORD dwAttribID, LONGLONG qwPlayerID, DWORD dwAttribLen, LPBYTE pBuffer)
{
	PATTRIB_LIST pAttribList = NULL;
	PBYTE pAttribIndex = NULL;
	DWORD dwNewHeaderAndOffsetLen = 0, dwNewTotalAttribLen = 0;

	if(!m_pBuffer || m_dwBufferLen < sizeof(XMATCH_SESSION))
		return FALSE;

	// Add the size of the length-prefix
	if(dwAttribID & X_ATTRIBUTE_DATATYPE_STRING)
	{
		if(dwAttribLen > MAXWORD)
			return FALSE;
		
		dwNewTotalAttribLen += sizeof(WORD);
	}
	else if(dwAttribID & X_ATTRIBUTE_DATATYPE_BLOB)
	{
		dwNewTotalAttribLen += sizeof(DWORD);
	}
	
	// Add the size of attributes and their attribute ID's
	dwNewTotalAttribLen += m_dwTotalAttribLen + sizeof(DWORD) + dwAttribLen + sizeof(LONGLONG);
	
	// Add the size of the attribute offsets required
	dwNewHeaderAndOffsetLen += sizeof(XMATCH_SESSION) + ((m_dwTotalAttribs + 1) * sizeof(DWORD));
	
	// Verify there is enough room to accomodate the new attribute
	if(dwNewHeaderAndOffsetLen + dwNewTotalAttribLen > m_dwBufferLen)
		return FALSE;
	
	// Allocate a new attribute and copies the attribute data into it
	pAttribList = new ATTRIB_LIST;
	
	// Insertion varies depending on type
	if(dwAttribID & X_ATTRIBUTE_DATATYPE_STRING)
	{
		pAttribList->dwAttribLen = dwAttribLen + 1 * sizeof(DWORD) + 1 * sizeof(WORD) + sizeof(LONGLONG);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);

		// Copy the player ID
		*((LONGLONG *) pAttribIndex) = qwPlayerID;
		pAttribIndex += sizeof(LONGLONG);

		// Copy the string length prefix
		*((WORD *) pAttribIndex) = (WORD) (0x0000FFFF & dwAttribLen);
		pAttribIndex += sizeof(WORD);
		
		// Copy the string value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	else if(dwAttribID & X_ATTRIBUTE_DATATYPE_BLOB)
	{
		pAttribList->dwAttribLen = dwAttribLen + 2 * sizeof(DWORD) + sizeof(LONGLONG);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the player ID
		*((LONGLONG *) pAttribIndex) = qwPlayerID;
		pAttribIndex += sizeof(LONGLONG);

		// Copy the string length prefix
		*((DWORD *) pAttribIndex) = dwAttribLen;
		pAttribIndex += sizeof(DWORD);
		
		// Copy the string value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	else
	{
		pAttribList->dwAttribLen = dwAttribLen + 1 * sizeof(DWORD) + sizeof(LONGLONG);
		pAttribList->pAttribBuffer = new BYTE[pAttribList->dwAttribLen];
		pAttribIndex = pAttribList->pAttribBuffer;
		
		// Copy the attribute ID
		*((DWORD *) pAttribIndex) = dwAttribID;
		pAttribIndex += sizeof(DWORD);

		// Copy the player ID
		*((LONGLONG *) pAttribIndex) = qwPlayerID;
		pAttribIndex += sizeof(LONGLONG);

		// Copy the integer value
		memcpy(pAttribIndex, pBuffer, dwAttribLen);
	}
	
	// Insert the attribute entry into the list
	pAttribList->pNext = m_pAttribList;
	m_pAttribList = pAttribList;
	
	// Increment the length tracking variables
	++m_dwTotalAttribs;
	m_dwTotalAttribLen = dwNewTotalAttribLen;
	
	return TRUE;	
}

BOOL CSessionCreation::SetHostAddress(LPBYTE pHostAddress)
{
	if(!m_pBuffer || m_dwBufferLen < (4 * sizeof(DWORD) + 32 * sizeof(BYTE)))
		return FALSE;
	
	memcpy(((XMATCH_SESSION *) m_pBuffer)->bHostAddress, pHostAddress, 32 * sizeof(BYTE));
	
	return TRUE;
}
