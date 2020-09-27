//==================================================================================
// xmclasses.h: header for XMatch request helper classes
//==================================================================================

#if !defined(XMCLASSES_H)
#define XMCLASSES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xtl.h>
#include <xdbg.h>
#include <xonline.h>
#include "asyncrequest.h"

#define MAX_STRESS_SEARCH_RESULTS 10

//==================================================================================
// Structures
//==================================================================================
typedef struct
{
	ULONGLONG	UserID;
	DWORD		dwAttributeID;
	union
	{
		ULONGLONG		qwValue;
		VOID			*pvValue;
	} Value;
} ATTRIBUTE, *PATTRIBUTE;

typedef struct
{
	DWORD			dwParameterType;
	union
	{
		ULONGLONG		qwValue;
		VOID			*pvValue;
	} Value;
} PARAMETER, *PPARAMETER;

typedef enum
{
    xmatchWorking = 0,
    xmatchCancelled,
    xmatchDone

} XMATCH_STATE;

//==================================================================================
// CXMatchSearchRequest
//==================================================================================
class CXMatchSearchRequest : public CXOnlineAsyncRequest
{
public:
	CXMatchSearchRequest(DWORD dwProcedure, DWORD dwNumParameters, DWORD dwParametersLen, DWORD dwResultsLen);
	BOOL AddParameters(PPARAMETER pParameterArray, DWORD dwParameterCount);
	HRESULT GetResults(PXMATCH_SEARCHRESULT** pppSearchResults, DWORD *pdwReturnedResults);
	HRESULT GetAttribute(DWORD dwSearchResultIndex, DWORD dwAttributeIndex, DWORD *pdwAttributeID, VOID *pAttributeValue,
		DWORD *pcbAttributeValue);
	HRESULT StartRequest();
};

//==================================================================================
// CXMatchSessionRequest
//==================================================================================
class CXMatchSessionRequest : public CXOnlineAsyncRequest
{
public:
	CXMatchSessionRequest(XNKID SessionID, DWORD dwNumAttributes, DWORD dwAttributesLen);
	BOOL AddAttributes(PATTRIBUTE pAttributeArray, DWORD dwAttributeCount);
	HRESULT GetSessionID(XNKID *pSessionID);
	HRESULT GetKeyExchangeKey(BYTE *pKeyExchangeKey, DWORD *pdwKeySize);
	HRESULT StartRequest();
};

#endif // !defined(XMCLASSES_H)
