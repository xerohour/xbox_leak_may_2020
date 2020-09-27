// MatchSearchTask.cpp: implementation of the CMatchSearchTask class.
//
//////////////////////////////////////////////////////////////////////

#include "MatchSearchTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatchSearchTask::CMatchSearchTask()
{
	m_dwResultsCount = 10;
	m_dwResultsLen = 10 * (sizeof(XMATCH_SEARCHRESULT) + 100);
	m_dwProcedureIndex = 1;
	m_fFindFromSessionID = FALSE;
	memset(&m_SessionID, 0, sizeof(XNKID));
	m_pSearchAttributes = NULL;
	m_dwSearchAttributesCount = 0;
	m_rgpReturnedResults = NULL;
	m_dwReturnedResultsCount = 0;
	m_fResultsAvailable = FALSE;
}

CMatchSearchTask::~CMatchSearchTask()
{

}

BOOL CMatchSearchTask::StartTask()
{
	HRESULT hr = S_OK;

	// If this object has been used for a previous search task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}

	if(m_fFindFromSessionID)
	{
		// Find from a sessionID
		m_hrLastResult = XOnlineMatchSessionFindFromID(m_SessionID, m_hWorkEvent, &m_hTask);

		if(FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CMatchSearchTask: XOnlineMatchSessionFindFromID failed", m_hrLastResult);
			return FALSE;
		}
	}
	else
	{
		// Create a search with attributes
		m_hrLastResult = XOnlineMatchSearch(m_dwProcedureIndex, m_dwResultsCount, m_dwSearchAttributesCount, m_pSearchAttributes, m_dwResultsLen, m_hWorkEvent, &m_hTask);

		if(FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CMatchSearchTask: XOnlineMatchSearch failed", m_hrLastResult);
			return FALSE;
		}
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	m_fResultsAvailable = FALSE;

	return TRUE;
}

HRESULT CMatchSearchTask::GetSearchResults(PXMATCH_SEARCHRESULT **prgpSearchResults, DWORD *pdwResultsCount)
{
	if(!m_fResultsAvailable)
	{
		m_hrLastResult = XOnlineMatchSearchGetResults(m_hTask, &m_rgpReturnedResults, &m_dwReturnedResultsCount);
		
		if(FAILED(m_hrLastResult))
			return m_hrLastResult;
	}
	
	if(prgpSearchResults)
		*prgpSearchResults = m_rgpReturnedResults;
	
	if(pdwResultsCount)
		*pdwResultsCount = m_dwReturnedResultsCount;

	return m_hrLastResult;
}

void CMatchSearchTask::SetSearchAttributes(PXONLINE_ATTRIBUTE pAttributes, DWORD dwAttributesCount)
{
	if(!pAttributes && (dwAttributesCount > 0))
		return;

	m_dwSearchAttributesCount = dwAttributesCount;
	m_pSearchAttributes = pAttributes;
}

void CMatchSearchTask::SetResultsSize(DWORD dwResultsCount, DWORD dwResultsLen)
{
	m_dwResultsCount = dwResultsCount;
	m_dwResultsLen = dwResultsLen;
}

void CMatchSearchTask::SetSessionIDForSearch(XNKID *pSessionID)
{
	if(!pSessionID)
		return;

	memcpy(&m_SessionID, pSessionID, sizeof(XNKID));

	m_fFindFromSessionID = TRUE;

	return;
}

void CMatchSearchTask::SetSearchProcIndex(DWORD dwProcIndex)
{
	m_dwProcedureIndex = dwProcIndex;
}
