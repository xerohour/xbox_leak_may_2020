// OfferingEnumerateTask.h: interface for the COfferingEnumerate class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class COfferingEnumerateTask : public CAsyncTask 
{
public:
	void SetEnumParams(PXONLINEOFFERING_ENUM_PARAMS pEnumParams);
	void SetUserIndex(DWORD dwUserIndex);
	void SetTitleSpecificDataMaxSize(DWORD dwTitleSpecificDataMaxSize);
	HRESULT GetResults(PXONLINEOFFERING_INFO **prgpOfferingInfo, DWORD *pdwReturnedResults, BOOL *pfMoreResults);
	BOOL StartTask();
	COfferingEnumerateTask();
	virtual ~COfferingEnumerateTask();

protected:
	DWORD m_dwTitleSpecificDataMaxSize;
	XONLINEOFFERING_ENUM_PARAMS m_EnumParams;
	DWORD m_dwUserIndex;
};
