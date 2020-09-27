//==================================================================================
// xmclasses.cpp: implementation of various helper classes for
//                handling asynchronous XMatch requests
//==================================================================================
#include "xmclasses.h"

//
// BUGBUG: Dummy stubs to let it compile
//

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionCreate(
    IN XNKID SessionID,
    IN DWORD dwNumAttributes,
    IN DWORD dwAttributesLen,
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE* phSession
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetInt(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN ULONGLONG qwAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetString(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN LPCWSTR pwszAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSetBlob(
    IN XONLINETASK_HANDLE hSession,
    IN DWORD dwAttributeID,
    IN ULONGLONG qwUserPUID,
    IN DWORD dwAttributeLength,
    IN PVOID pvAttributeValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionAddPlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionRemovePlayer(
    IN XONLINETASK_HANDLE hSession,
    IN ULONGLONG qwUserPUID
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionSend(
    IN XONLINETASK_HANDLE hSession
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionGetID (
    IN XONLINETASK_HANDLE hSession,
    OUT XNKID* pSessionID
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionDelete(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phDelete
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionFindFromID(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchCreate(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumParameters,
    IN DWORD dwParametersLen,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendInt(
    IN XONLINETASK_HANDLE hSearch,
    IN ULONGLONG qwParameterValue
    )
{
    return S_OK;
}

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendString(
    IN XONLINETASK_HANDLE hSearch,
    IN LPCWSTR pwszParameterValue
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchAppendBlob(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwParameterLen,
    IN LPVOID pvParameterValue
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchSend(
    IN XONLINETASK_HANDLE hSearch
    )
{
    return S_OK;
}
    
XBOXAPI
HRESULT 
WINAPI
XOnlineMatchSearchGetResults(
    IN XONLINETASK_HANDLE hSearch,
    OUT PXMATCH_SEARCHRESULT **prgpSearchResults,
    OUT DWORD *pdwReturnedResults
    )
{
    return S_OK;
}
 
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchGetAttribute(
    IN XONLINETASK_HANDLE hSearch,
    IN DWORD dwSearchResultIndex,
    IN DWORD dwAttributeIndex,
    OUT DWORD *pdwAttributeID,
    OUT VOID *pAttributeValue,
    OUT DWORD *pcbAttributeValue
    )
{
    return S_OK;
}

//==================================================================================
// CXMatchSearchRequest
//==================================================================================
CXMatchSearchRequest::CXMatchSearchRequest(DWORD dwProcedure, DWORD dwNumParameters, DWORD dwParametersLen, DWORD dwResultsLen)
{
	HRESULT hr = S_OK;

	m_hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hWorkEvent)
	{
		m_hTaskHandle = NULL;
		return;
	}

	hr = XOnlineMatchSearchCreate(dwProcedure, MAX_STRESS_SEARCH_RESULTS, dwNumParameters, dwParametersLen, dwResultsLen, m_hWorkEvent, &m_hTaskHandle);
	if(FAILED(hr))
	{
		CloseHandle(m_hWorkEvent);
		m_hWorkEvent = NULL;
		m_hTaskHandle = NULL;
		return;
	}
}

BOOL CXMatchSearchRequest::AddParameters(PPARAMETER pParameterArray, DWORD dwParameterCount)
{
	HRESULT hr = S_OK;
	DWORD dwParameterIndex = 0;
	BOOL fRet = TRUE;

	if(!pParameterArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwParameterIndex = 0; dwParameterIndex < dwParameterCount; ++dwParameterIndex)
	{
		switch (pParameterArray[dwParameterIndex].dwParameterType & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSearchAppendInt(m_hTaskHandle, pParameterArray[dwParameterIndex].Value.qwValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSearchAppendString(m_hTaskHandle, (LPWSTR) pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSearchAppendBlob(m_hTaskHandle, strlen((LPSTR)pParameterArray[dwParameterIndex].Value.pvValue),
					pParameterArray[dwParameterIndex].Value.pvValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
}

HRESULT CXMatchSearchRequest::GetResults(PXMATCH_SEARCHRESULT** pppSearchResults, DWORD *pdwReturnedResults)
{
	if(!m_hTaskHandle)
		return E_FAIL;

	return XOnlineMatchSearchGetResults(m_hTaskHandle, pppSearchResults, pdwReturnedResults);
}

HRESULT CXMatchSearchRequest::GetAttribute(DWORD dwSearchResultIndex, DWORD dwAttributeIndex, DWORD *pdwAttributeID,
    VOID *pAttributeValue, DWORD *pcbAttributeValue)
{
	if(!m_hTaskHandle)
		return E_FAIL;

	return XOnlineMatchSearchGetAttribute(m_hTaskHandle, dwSearchResultIndex, dwAttributeIndex, pdwAttributeID,
		pAttributeValue, pcbAttributeValue);
}

HRESULT CXMatchSearchRequest::StartRequest()
{
	if(!m_hTaskHandle)
		return E_FAIL;

	m_nCurrentState = STATE_INPROGRESS;

	return XOnlineMatchSearchSend(m_hTaskHandle);
}

//==================================================================================
// CXMatchSessionRequest
//==================================================================================
CXMatchSessionRequest::CXMatchSessionRequest(XNKID SessionID, DWORD dwNumAttributes, DWORD dwAttributesLen)
{
	HRESULT hr = S_OK;

	m_hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_hWorkEvent)
	{
		m_hTaskHandle = NULL;
		return;
	}

	hr = XOnlineMatchSessionCreate(SessionID, dwNumAttributes, dwAttributesLen, m_hWorkEvent, &m_hTaskHandle);
	if(FAILED(hr))
	{
		CloseHandle(m_hWorkEvent);
		m_hWorkEvent = NULL;
		m_hTaskHandle = NULL;
		return;
	}
}

BOOL CXMatchSessionRequest::AddAttributes(PATTRIBUTE pAttributeArray, DWORD dwAttributeCount)
{
	HRESULT hr = S_OK;
	DWORD dwAttributeIndex = 0;
	BOOL fRet = TRUE;

	if(!pAttributeArray)
	{
		fRet = FALSE;
		goto Exit;
	}

	for (dwAttributeIndex = 0; dwAttributeIndex < dwAttributeCount; ++dwAttributeIndex)
	{
		switch (pAttributeArray[dwAttributeIndex].dwAttributeID & X_ATTRIBUTE_DATATYPE_MASK)
		{
			case X_ATTRIBUTE_DATATYPE_INTEGER:
			{
				hr = XOnlineMatchSessionSetInt(m_hTaskHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].UserID, pAttributeArray[dwAttributeIndex].Value.qwValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_STRING:
			{
				hr = XOnlineMatchSessionSetString(m_hTaskHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].UserID, (LPWSTR) pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}

			case X_ATTRIBUTE_DATATYPE_BLOB:
			{
				hr = XOnlineMatchSessionSetBlob(m_hTaskHandle, pAttributeArray[dwAttributeIndex].dwAttributeID,
					pAttributeArray[dwAttributeIndex].UserID, strlen((LPSTR)pAttributeArray[dwAttributeIndex].Value.pvValue),
					pAttributeArray[dwAttributeIndex].Value.pvValue );
				if (FAILED(hr))
				{
					fRet = FALSE;
					goto Exit;
				}
				break;
			}
		}
	}

Exit:

	return fRet;
}

HRESULT CXMatchSessionRequest::GetSessionID(XNKID *pSessionID)
{
	if(!m_hTaskHandle)
		return E_FAIL;

	return XOnlineMatchSessionGetID (m_hTaskHandle, pSessionID);
}

HRESULT CXMatchSessionRequest::GetKeyExchangeKey(BYTE *pKeyExchangeKey, DWORD *pdwKeySize)
{
	return E_NOTIMPL;
}

HRESULT CXMatchSessionRequest::StartRequest()
{
	if(!m_hTaskHandle)
		return E_FAIL;

	m_nCurrentState = STATE_INPROGRESS;

	return XOnlineMatchSessionSend(m_hTaskHandle);
}
