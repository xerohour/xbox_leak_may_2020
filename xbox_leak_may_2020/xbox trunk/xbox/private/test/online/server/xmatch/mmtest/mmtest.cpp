// mmtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "connection.h"
#include "httprequest.h"
#include "sessioncreation.h"

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

BOOL ParseSearchResults(BYTE *pBuffer, DWORD dwBufferSize)
{
	XMATCH_SEARCHRESULT *pCurrentResult = NULL;
	DWORD dwResultCounter = 1, dwRemainingBuffer = dwBufferSize, dwCurrentResultLength = 0;
	BYTE *pResultsIndex = pBuffer;
	BOOL fRet = TRUE;

	_tprintf(TEXT("Results:\n"));
	_tprintf(TEXT("  TotalSize %u\n"), dwBufferSize);

	while(dwRemainingBuffer >= sizeof(XMATCH_SEARCHRESULT))
	{
		_tprintf(TEXT("   Search result %u\n"), dwResultCounter);

		pCurrentResult = (XMATCH_SEARCHRESULT *) pResultsIndex;
		dwCurrentResultLength = pCurrentResult->dwResultLength;
		_tprintf(TEXT("      Result length: %u\n"), dwCurrentResultLength);
		_tprintf(TEXT("      Session ID: %u\n"), pCurrentResult->dwSessionID);
		_tprintf(TEXT("      HostAddress1: 0x%08x%08x\n"),
			*((DWORD *) &(pCurrentResult->bHostAddress[0])),
			*((DWORD *) &(pCurrentResult->bHostAddress[4])));
		_tprintf(TEXT("      HostAddress2: 0x%08x%08x\n"),
			*((DWORD *) &(pCurrentResult->bHostAddress[8])),
			*((DWORD *) &(pCurrentResult->bHostAddress[12])));
		_tprintf(TEXT("      HostAddress3: 0x%08x%08x\n"),
			*((DWORD *) &(pCurrentResult->bHostAddress[16])),
			*((DWORD *) &(pCurrentResult->bHostAddress[20])));
		_tprintf(TEXT("      HostAddress4: 0x%08x%08x\n"),
			*((DWORD *) &(pCurrentResult->bHostAddress[24])),
			*((DWORD *) &(pCurrentResult->bHostAddress[28])));
		_tprintf(TEXT("      Public Available: %u\n"), pCurrentResult->dwPublicAvailable);
		_tprintf(TEXT("      Private Available: %u\n"), pCurrentResult->dwPrivateAvailable);
		_tprintf(TEXT("      Num Attributes: %u\n"), pCurrentResult->dwNumAttributes);

		++dwResultCounter;
		if(pCurrentResult->dwResultLength > dwRemainingBuffer)
		{
			_tprintf(TEXT("Current result is larger than available data!\n"));
			dwRemainingBuffer = 0;
			break;
		}
		dwRemainingBuffer -= pCurrentResult->dwResultLength;
	}

	if(dwRemainingBuffer)
	{
		_tprintf(TEXT("%u bytes of unused data present at end of results list!\n"), dwRemainingBuffer);
		fRet = FALSE;
	}

	return fRet;
}


DWORD QuerySession(LPTSTR szServer, LPTSTR szObject, XMATCH_SEARCH *pSearchInfo, DWORD dwInfoSize, BYTE **ppBuffer)
{
	CConnection *pConnection;
	CHttpRequest *pHttpRequest;
	DWORD dwExpectedSize = 0, dwActualSize = 0, dwStatusCode = 0;
	BOOL fFailed = FALSE;

	pConnection = new CConnection;
	pConnection->AddRef();
	pConnection->EstablishConnection(MMTEST_AGENT_NAME, szServer, INTERNET_DEFAULT_HTTP_PORT);

	pHttpRequest = new CHttpRequest(pConnection);
	pHttpRequest->Initialize(TEXT("POST"), szObject);
	pHttpRequest->AddHeaders("Content-Type: application/xmatch\r\n");
	pHttpRequest->SendRequest(pSearchInfo, dwInfoSize);
	pHttpRequest->GetStatusCode(&dwStatusCode);

	if(dwStatusCode != 200)
	{
		_tprintf(TEXT("Query returned failure (status %u)\n"), dwStatusCode);
		dwActualSize = 0;
		fFailed = TRUE;
	}

	pHttpRequest->GetContentLength(&dwExpectedSize);

	if(dwExpectedSize)
	{
		// We received a content length, header so use that
		_tprintf(TEXT("Retrieved content length as %u\n"), dwExpectedSize);
		dwActualSize = dwExpectedSize;
		*ppBuffer = new BYTE[dwExpectedSize];
	}
	else
	{
		// If we didn't get a content length, then just return the first 10 results
		_tprintf(TEXT("Server didn't return a content length.  Will only allow 10 results.\n"));
		dwActualSize = sizeof(XMATCH_SEARCHRESULT) * 10;
	    *ppBuffer = new BYTE[sizeof(XMATCH_SEARCHRESULT) * 10];
	}


	pHttpRequest->DownloadDataToBuffer(*ppBuffer, &dwActualSize);
		
	if(fFailed)
	{
		if((dwActualSize != 4) && (dwActualSize != 8))
		{
			_tprintf(TEXT("Received unexpected error message of size %u\n"), dwActualSize);
		}
		else
		{
			switch(*((DWORD *) *ppBuffer))
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
	
	else if(dwExpectedSize && (dwActualSize != dwExpectedSize))
	{
		_tprintf(TEXT("Unexpected data size returned (%u != %u)\n"), dwActualSize, dwExpectedSize);
		if(dwActualSize > dwExpectedSize)
		{
			dwActualSize = dwExpectedSize;
		}
	}
	
	pHttpRequest ? delete pHttpRequest : 0;
	pConnection ? delete pConnection : 0;

	return dwActualSize;
}

void RunSessionCreationTests(LPTSTR szServerName)
{
	BYTE Buffer[sizeof(XMATCH_SESSION) + 500], Blob[100], *pResults = NULL;
	XMATCH_SESSION *pSessionInfo = (XMATCH_SESSION *) Buffer;
	XMATCH_SEARCH *pSearchInfo = (XMATCH_SEARCH *) Buffer;
	BYTE *pAttribPointer = NULL, *pParamPointer = NULL, *pLargeBuffer = NULL;
	DWORD dwSessionID = 0, dwResultsSize = 0;
	INT nIntAttribute = 0;

	memset(Blob, 'z', 100 * sizeof(BYTE));

	CSessionCreation *pCreateRequest = NULL;

/*
	// ------------------------------------------
	// Message length longer than available bytes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Message length longer than available bytes\n"));

	pCreateRequest = new CSessionCreation();
	pCreateRequest->Initialize(sizeof(XMATCH_SESSION));

	((XMATCH_SESSION *) pCreateRequest->m_pBuffer)->dwMessageLength = sizeof(XMATCH_SESSION) + 10;
	
	pCreateRequest->SendRawRequest(szServerName, CREATESESSION_OBJECT);
	delete pCreateRequest;
	pCreateRequest = NULL;

	// ------------------------------------------
	// Message length only one byte
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Message length only one byte\n"));

	Buffer[0] = 255;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, (XMATCH_SESSION *) Buffer, 1);

	// ------------------------------------------
	// Title ID unknown
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Title ID unknown\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 0xAAAAAAAA;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Max title ID
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Max title ID\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 0xFFFFFFFF;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Message length shorter than sizeof(XMATCH_SESSION)
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Message length shorter than sizeof(XMATCH_SESSION)\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Unknown protocol version
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Unknown protocol version\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION + 1;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// No public or private slots available
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("No public or private slots available\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Use attib offset inside session header
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Use attib offset inside session header\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Use attib offset beyond available
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Use attib offset beyond available\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = 10000;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Use attrib offset equal to current offset
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Use attrib offset equal to current offset\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION);

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Indicate more attributes than available
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Indicate more attributes than available\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with no attributes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with no attributes\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search where returned session will have no attributes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have no attributes\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 5;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with no attributes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with no attributes\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with no attributes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with no attributes\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 5;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a global session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global session integer attribute\n"));

	pCreateRequest = new CSessionCreation();
	pCreateRequest->Initialize(500);

	nIntAttribute = SAMPLE_ATTRIB1_INT;
	pCreateRequest->AddAttribute(X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID) ,sizeof(DWORD), (BYTE *) &nIntAttribute);
	
	dwSessionID = pCreateRequest->SendRequest(szServerName, CREATESESSION_OBJECT);
	_tprintf(TEXT("Session %u created\n"), dwSessionID);
	delete pCreateRequest;
	pCreateRequest = NULL;

  
	// ------------------------------------------
	// Search where returned session will have a global session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a global session integer attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 7;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with global session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with global session integer attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with global session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with global session integer attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 7;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD) + strlen(SAMPLE_ATTRIB1_STRING) + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = strlen(SAMPLE_ATTRIB1_STRING) + 1;

	pAttribPointer += sizeof(WORD);
	strcpy((char *) pAttribPointer, SAMPLE_ATTRIB1_STRING);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Search where returned session will have a global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a global session string attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 8;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with global session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with global session string attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 8;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;

	
	// ------------------------------------------
	// Create session with a global session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global session blob attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 100;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = 100;

	pAttribPointer += sizeof(DWORD);
	memcpy((char *) pAttribPointer, Blob, 100);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Search where returned session will have a global session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a global session blob attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 9;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with global session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with global session blob attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with global session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with global session blob attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 9;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a title-specific session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific session integer attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_INTEGER |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = SAMPLE_ATTRIB1_INT;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Search where returned session will have a title-specific session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a title-specific session integer attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 7;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with title-specific session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with title-specific session integer attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with title-specific session integer attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with title-specific session integer attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 7;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a title-specific session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD) + strlen(SAMPLE_ATTRIB1_STRING) + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = strlen(SAMPLE_ATTRIB1_STRING) + 1;

	pAttribPointer += sizeof(WORD);
	strcpy((char *) pAttribPointer, SAMPLE_ATTRIB1_STRING);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Search where returned session will have a title-specific session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a title-specific session string attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 8;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with title-specific session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with title-specific session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with title-specific session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with title-specific session string attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 8;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a title-specific session blob attribute attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific session blob attribute attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 100;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = 100;

	pAttribPointer += sizeof(DWORD);
	memcpy((char *) pAttribPointer, Blob, 100);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Search where returned session will have a title-specific session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search where returned session will have a title-specific session blob attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 9;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
		ParseSearchResults(pResults, dwResultsSize);
	else
		_tprintf(TEXT("FAIL:  Search didn't return expected session!\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Delete session with title-specific session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Delete session with title-specific session blob attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));


	// ------------------------------------------
	// Search for deleted session with title-specific session blob attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search for deleted session with title-specific session blob attribute\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 9;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = dwSessionID;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		_tprintf(TEXT("FAIL:  Search returned deleted session!\n"));
		ParseSearchResults(pResults, dwResultsSize);
	}
	else
		_tprintf(TEXT("Search didn't report deleted session\n"));

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Create session with a global user integer attribute without PUID
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global user integer attribute without PUID\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_INTEGER |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = SAMPLE_ATTRIB1_INT;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with a global user integer attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global user integer attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 1 * sizeof(LONGLONG);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_INTEGER |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((DWORD *) pAttribPointer) = SAMPLE_ATTRIB1_INT;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with a global user string attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global user string attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + 1 * sizeof(LONGLONG) + sizeof(WORD) + strlen(SAMPLE_ATTRIB1_STRING) + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((WORD *) pAttribPointer) = strlen(SAMPLE_ATTRIB1_STRING) + 1;

	pAttribPointer += sizeof(WORD);
	strcpy((char *) pAttribPointer, SAMPLE_ATTRIB1_STRING);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with a global user blob attribute attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global user blob attribute attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 1 * sizeof(LONGLONG) + 100;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((DWORD *) pAttribPointer) = 100;

	pAttribPointer += sizeof(DWORD);
	memcpy((char *) pAttribPointer, Blob, 100);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with a title-specific user integer attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific user integer attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 1 * sizeof(LONGLONG);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_INTEGER |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((DWORD *) pAttribPointer) = SAMPLE_ATTRIB1_INT;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with a title-specific user string attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific user string attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + 1 * sizeof(LONGLONG) + sizeof(WORD) + strlen(SAMPLE_ATTRIB1_STRING) + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((WORD *) pAttribPointer) = strlen(SAMPLE_ATTRIB1_STRING) + 1;

	pAttribPointer += sizeof(WORD);
	strcpy((char *) pAttribPointer, SAMPLE_ATTRIB1_STRING);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	// ------------------------------------------
	// Create session with a title-specific user blob attribute attribute for non-existent user
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a title-specific user blob attribute attribute for non-existent user\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + 1 * sizeof(LONGLONG) + 100;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC | X_ATTRIBUTE_TYPE_USER | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((LONGLONG *) pAttribPointer) = SAMPLE_PUID1;

	pAttribPointer += sizeof(LONGLONG);
	*((DWORD *) pAttribPointer) = 100;

	pAttribPointer += sizeof(DWORD);
	memcpy((char *) pAttribPointer, Blob, 100);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with an empty global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with an empty global session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD) + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = 1;

	pAttribPointer += sizeof(WORD);
	*((BYTE *) pAttribPointer) = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with a zero-length global session string attribute
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a zero-length global session string attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with a zero-length global session blob attribute 
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a zero-length global session blob attribute\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with a string attribute whose length goes beyond the number of bytes available
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a string attribute whose length goes beyond the number of bytes available\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = 0xFFFF;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);
*/
	// ------------------------------------------
	// Create session with a blob attribute whose length goes beyond the number of bytes available
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a blob attribute whose length goes beyond the number of bytes available\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = Buffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = 0xFFFFFFFF;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);
/*
	// ------------------------------------------
	// Create session with a global session string attribute of maximum size
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global session string attribute of maximum size\n"));

	pLargeBuffer = new BYTE[sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD) + MAXWORD];
	pSessionInfo = (XMATCH_SESSION *) pLargeBuffer;

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 2 * sizeof(DWORD) + sizeof(WORD) + MAXWORD;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = pLargeBuffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_STRING |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((WORD *) pAttribPointer) = MAXWORD;

	// Build a string of all a's
	pAttribPointer += sizeof(WORD);
	memset((char *) pAttribPointer, 'a', MAXWORD - 1);
	pAttribPointer[MAXWORD - 1] = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	pSessionInfo = (XMATCH_SESSION *) Buffer;
	pLargeBuffer ? delete [] pLargeBuffer : 0;

	// ------------------------------------------
	// Create session with a global session blob attribute of larger than MAXWORD size
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with a global session blob attribute of larger than MAXWORD size\n"));

	pLargeBuffer = new BYTE[sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + MAXWORD + 1];
	pSessionInfo = (XMATCH_SESSION *) pLargeBuffer;

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength =
		sizeof(XMATCH_SESSION) + 3 * sizeof(DWORD) + MAXWORD + 1;
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 1;

	pAttribPointer = pLargeBuffer + sizeof(XMATCH_SESSION);
	*((DWORD *) pAttribPointer) = sizeof(XMATCH_SESSION) + sizeof(DWORD);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) =
		X_ATTRIBUTE_SCOPE_GLOBAL | X_ATTRIBUTE_TYPE_SESSION | X_ATTRIBUTE_DATATYPE_BLOB |
		(X_ATTRIBUTE_ID_MASK & SAMPLE_ATTRIB1_ID);

	pAttribPointer += sizeof(DWORD);
	*((DWORD *) pAttribPointer) = MAXWORD;

	// Build a blob with all 0xAA's
	pAttribPointer += sizeof(DWORD);
	memset((char *) pAttribPointer, 0xAA, MAXWORD + 1);

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);

	pSessionInfo = (XMATCH_SESSION *) Buffer;
	pLargeBuffer ? delete [] pLargeBuffer : 0;
*/
}

void RunSessionDeletionTests(LPTSTR szServerName)
{
	BYTE Buffer[sizeof(XMATCH_SESSION) + 500];
	XMATCH_SESSION *pSessionInfo = (XMATCH_SESSION *) Buffer;
	DWORD dwSessionID = 0;

	// ------------------------------------------
	// Try to delete an invalid session
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Try to delete an invalid session\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 1;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Create session with no attibutes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with no attibutes\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Try to delete a valid session
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Try to delete a valid session\n"));

	memset(pSessionInfo->bHostAddress, 0xAA, 32);
	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, DELETESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Try to modify a deleted session
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Try to modify a deleted session\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	if(CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength))
	{
		_tprintf(TEXT("Shouldn't have received a new session ID!\n"));
	}

}

void RunSessionModificationTests(LPTSTR szServerName)
{
	BYTE Buffer[sizeof(XMATCH_SESSION) + 500], *pAttribPointer = NULL;;
	XMATCH_SESSION *pSessionInfo = (XMATCH_SESSION *) Buffer;
	DWORD dwSessionID = 0;

	memset(pSessionInfo->bHostAddress, 0xAA, 32);


	// ------------------------------------------
	// Try to modify a session that doesn't exist
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Try to modify a session that doesn't exist\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0x55555555;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));

	// ------------------------------------------
	// Create session with no attibutes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with no attibutes\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Modify session to decrease the number of public players to 0
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Modify session to decrease the number of public players to 0\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	if(CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength))
	{
		_tprintf(TEXT("Shouldn't have received a new session ID!\n"));
	}


	// ------------------------------------------
	// Modify session to increase the number of public players above 0
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Modify session to increase the number of public players above 0\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 1;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	if(CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength))
	{
		_tprintf(TEXT("Shouldn't have received a new session ID!\n"));
	}


	// ------------------------------------------
	// Create session with no attibutes
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Create session with no attibutes\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 10;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength);


	// ------------------------------------------
	// Modify session to decrease the number of private players to 0
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Modify session to decrease the number of private players to 0\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	if(CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength))
	{
		_tprintf(TEXT("Shouldn't have received a new session ID!\n"));
	}


	// ------------------------------------------
	// Modify session to increase the number of private players above 0
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Modify session to increase the number of private players above 0\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = dwSessionID;
	pSessionInfo->dwPublicAvailable = 0;
	pSessionInfo->dwPrivateAvailable = 1;
	pSessionInfo->dwNumAttributes = 0;

	if(CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, pSessionInfo->dwMessageLength))
	{
		_tprintf(TEXT("Shouldn't have received a new session ID!\n"));
	}

	return;
}

void RunSessionSearchTests(LPTSTR szServerName)
{
	BYTE Buffer[sizeof(XMATCH_SEARCH) + 500], *pParamPointer = NULL, Blob[100];
	BYTE *pResults = NULL;
	XMATCH_SEARCH *pSearchInfo = (XMATCH_SEARCH *) Buffer;
	DWORD dwResultsSize = 0, dwSessionID = 0;

	memset(Blob, 0x0F, 100 * sizeof(BYTE));

	XMATCH_SESSION *pSessionInfo = (XMATCH_SESSION *) Buffer;

	memset(pSessionInfo->bHostAddress, 0xAA, 32);


	// ------------------------------------------
	// Basic session creation for title 1
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Basic session creation for title 1\n"));

	pSessionInfo->dwMessageLength = sizeof(XMATCH_SESSION);
	pSessionInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSessionInfo->dwTitleID = 1;
	pSessionInfo->dwSessionID = 0;
	pSessionInfo->dwPublicAvailable = 10;
	pSessionInfo->dwPrivateAvailable = 0;
	pSessionInfo->dwNumAttributes = 0;

	dwSessionID = CreateDeleteModifySession(szServerName, CREATESESSION_OBJECT, pSessionInfo, sizeof(XMATCH_SESSION));

	// ------------------------------------------
	// Search with invalid session ID as parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with invalid session ID as parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 5;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = 0;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;

	// ------------------------------------------
	// Search with 0 parameters using a stored procedure that doesn't exist
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with 0 parameters using a stored procedure that doesn't exist\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 1000;
	pSearchInfo->dwNumParameters = 0;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;

	// ------------------------------------------
	// Search with 0 parameters
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with 0 parameters\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 0;
	pSearchInfo->dwNumParameters = 0;

	_tprintf(TEXT("Calling QuerySession\n"));
	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);
	_tprintf(TEXT("Returned from QuerySession\n"));

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;

	// ------------------------------------------
	// Search with 1 integer parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with 1 integer parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 1;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 12345;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with 1 string parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with 1 string parameter\n"));

	pSearchInfo->dwMessageLength =
		sizeof(XMATCH_SEARCH) + 2 * sizeof(DWORD) + 1 * sizeof(WORD) + strlen(SAMPLE_PARAM1_STRING) + 1;
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 2;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_STRING;

	pParamPointer += sizeof(DWORD);
	*((WORD *) pParamPointer) = strlen(SAMPLE_PARAM1_STRING) + 1;

	pParamPointer += sizeof(WORD);
	strcpy((char *) pParamPointer, SAMPLE_PARAM1_STRING);

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with 1 blob parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with 1 blob parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD) + 100 * sizeof(BYTE);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 3;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_BLOB;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 100;

	pParamPointer += sizeof(DWORD);
	memcpy((char *) pParamPointer, Blob, 100 * sizeof(BYTE));

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with integer stored proc and string parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with integer stored proc and string parameter\n"));

	pSearchInfo->dwMessageLength =
		sizeof(XMATCH_SEARCH) + 2 * sizeof(DWORD) + 1 * sizeof(WORD) + strlen(SAMPLE_PARAM1_STRING) + 1;
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 1;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_STRING;

	pParamPointer += sizeof(DWORD);
	*((WORD *) pParamPointer) = strlen(SAMPLE_PARAM1_STRING) + 1;

	pParamPointer += sizeof(WORD);
	strcpy((char *) pParamPointer, SAMPLE_PARAM1_STRING);

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with integer stored proc and blob parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with integer stored proc and blob parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD) + 100 * sizeof(BYTE);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 1;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_BLOB;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 100;

	pParamPointer += sizeof(WORD);
	memcpy((char *) pParamPointer, Blob, 100 * sizeof(BYTE));

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with string stored proc and integer parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with string stored proc and integer parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 2;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 12345;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with string stored proc and blob parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with string stored proc and blob parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD) + 100 * sizeof(BYTE);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 2;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_BLOB;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 100;

	pParamPointer += sizeof(DWORD);
	memcpy((char *) pParamPointer, Blob, 100 * sizeof(BYTE));

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with blob stored proc and integer parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with blob stored proc and integer parameter\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 3;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_INTEGER;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 12345;

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;


	// ------------------------------------------
	// Search with blob stored proc and string parameter
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Search with blob stored proc and string parameter\n"));

	pSearchInfo->dwMessageLength =
		sizeof(XMATCH_SEARCH) + 2 * sizeof(DWORD) + 1 * sizeof(WORD) + strlen(SAMPLE_PARAM1_STRING) + 1;
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 3;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_STRING;

	pParamPointer += sizeof(DWORD);
	*((WORD *) pParamPointer) = strlen(SAMPLE_PARAM1_STRING) + 1;

	pParamPointer += sizeof(WORD);
	strcpy((char *) pParamPointer, SAMPLE_PARAM1_STRING);

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;

	// ------------------------------------------
	// Repro case for 5008
	// ------------------------------------------
	_tprintf(TEXT("\n"));
	_tprintf(TEXT("Repro case for 5008\n"));

	pSearchInfo->dwMessageLength = sizeof(XMATCH_SEARCH) + 3 * sizeof(DWORD) + 100 * sizeof(BYTE);
	pSearchInfo->dwProtocolVersion = X_MATCH_PROTOCOL_VERSION;
	pSearchInfo->dwTitleID = 1;
	pSearchInfo->dwQueryID = 1;
	pSearchInfo->dwProcedureIndex = 1;
	pSearchInfo->dwNumParameters = 1;

	pParamPointer = Buffer + sizeof(XMATCH_SEARCH);
	*((DWORD *) pParamPointer) = sizeof(XMATCH_SEARCH) + sizeof(DWORD);

	pParamPointer += sizeof(DWORD);
	*((DWORD *) pParamPointer) = X_ATTRIBUTE_DATATYPE_BLOB;

	pParamPointer += sizeof(DWORD);
	*((INT *) pParamPointer) = 100;

	pParamPointer += sizeof(WORD);
	memcpy((char *) pParamPointer, Blob, 100 * sizeof(BYTE));

	dwResultsSize = QuerySession(szServerName, QUERYSESSION_OBJECT, pSearchInfo, pSearchInfo->dwMessageLength, &pResults);

	if(dwResultsSize && pResults)
	{
		ParseSearchResults(pResults, dwResultsSize);
	}

	pResults ? delete [] pResults : 0;
	pResults = NULL;
}

int main(int argc, char* argv[])
{
	TCHAR szServerName[50] = DEFAULT_SERVER;

	if(argc > 1)
	{
#ifdef UNICODE
		mbstowcs(szServerName, argv[1], strlen(argv[1]) + 1);
#else
		strcpy(szServerName, argv[1]);
#endif
	}

	RunSessionCreationTests(szServerName);

//	RunSessionDeletionTests(szServerName);

//	RunSessionModificationTests(szServerName);

//	RunSessionSearchTests(szServerName);

	return 0;
}
