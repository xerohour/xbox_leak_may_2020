// MatchSearchTask.h: interface for the CMatchSearchTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CMatchSearchTask : public CAsyncTask  
{
public:
	void SetSearchProcIndex(DWORD dwProcIndex);
	void SetSessionIDForSearch(XNKID *pSessionID);
	void SetResultsSize(DWORD dwResultsCount, DWORD dwResultsLen);
	void SetSearchAttributes(PXONLINE_ATTRIBUTE pAttributes, DWORD dwAttributesCount);
	HRESULT GetSearchResults(PXMATCH_SEARCHRESULT **prgpSearchResults, DWORD *pdwResultsCount);
	BOOL StartTask();
	CMatchSearchTask();
	virtual ~CMatchSearchTask();

protected:
	BOOL m_fResultsAvailable;
	PXMATCH_SEARCHRESULT *m_rgpReturnedResults;
	DWORD m_dwReturnedResultsCount;
	XNKID m_SessionID;
	BOOL m_fFindFromSessionID;
	DWORD m_dwResultsLen;
	DWORD m_dwResultsCount;
	DWORD m_dwProcedureIndex;
	DWORD m_dwSearchAttributesCount;
	PXONLINE_ATTRIBUTE m_pSearchAttributes;
};
