// MatchSessionCreateTask.cpp: implementation of the CMatchSessionCreateTask class.
//
//////////////////////////////////////////////////////////////////////

#include "MatchSessionCreateTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatchSessionCreateTask::CMatchSessionCreateTask()
{
	memset(&m_SessionID, 0, sizeof(m_SessionID));
	memset(&m_SessionKey, 0, sizeof(m_SessionKey));
	m_fSessionIDSet = FALSE;
	m_pAttributes = NULL;
	m_dwAttributesCount = 0;
	m_dwPublicAvailable = 0;
	m_dwPrivateAvailable = 0;
	m_dwPublicCurrent = 0;
	m_dwPrivateCurrent = 0;
	m_fDeleteSessionOnClose = TRUE;
	m_fSessionCreated = FALSE;
}

CMatchSessionCreateTask::~CMatchSessionCreateTask()
{
	COnlineTimer DeleteTimer;
	XONLINETASK_HANDLE hDeleteTask = NULL;
	HRESULT hr = S_OK;
	HANDLE hDeleteEvent = NULL;

	if(m_fSessionCreated && m_fDeleteSessionOnClose)
	{
		hDeleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if(!hDeleteEvent)
			goto Exit;

		hr = XOnlineMatchSessionDelete(m_SessionID, hDeleteEvent, &hDeleteTask);
		if(FAILED(hr))
			goto Exit;

		DeleteTimer.SetAllowedTime(10000);
		DeleteTimer.Start();

		// Pump task for 10 seconds
		do
		{
			if(DeleteTimer.HasTimeExpired())
				break;

			WaitForSingleObject(hDeleteEvent, DeleteTimer.GetTimeRemaining());

			hr = XOnlineTaskContinue(hDeleteTask);
		} while(!XONLINETASK_STATUS_AVAILABLE(hr));
		
		if(!XONLINETASK_STATUS_AVAILABLE(hr))
			goto Exit;		
	}

Exit:

	if(hDeleteTask)
		XOnlineTaskClose(hDeleteTask);

	if(hDeleteEvent)
		CloseHandle(hDeleteEvent);

	return;
}

BOOL CMatchSessionCreateTask::StartTask()
{
	HRESULT hr = S_OK;

	// If this object has been used for a previous match task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}

	if(m_fSessionIDSet)
	{
		// Modify an existing session
		m_hrLastResult = XOnlineMatchSessionUpdate(m_SessionID, m_dwPublicCurrent, m_dwPublicAvailable, m_dwPrivateCurrent, m_dwPrivateAvailable, m_dwAttributesCount, m_pAttributes, m_hWorkEvent, &m_hTask);
		if(FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CMatchSessionCreateTask: XOnlineMatchSessionUpdate failed", m_hrLastResult);
			return FALSE;
		}
	}
	else
	{
		// Create a new session
		m_hrLastResult = XOnlineMatchSessionCreate(m_dwPublicCurrent, m_dwPublicAvailable, m_dwPrivateCurrent, m_dwPrivateAvailable, m_dwAttributesCount, m_pAttributes, m_hWorkEvent, &m_hTask);
		if(FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CMatchSessionCreateTask: XOnlineMatchSessionCreate failed", m_hrLastResult);
			return FALSE;
		}
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

void CMatchSessionCreateTask::SetAttributes(PXONLINE_ATTRIBUTE pAttributes, DWORD dwAttributesCount)
{
	m_pAttributes = pAttributes;
	m_dwAttributesCount = dwAttributesCount;
}

HRESULT CMatchSessionCreateTask::TaskContinue()
{
	HRESULT hr = S_OK;

	// Only process this if the task hasn't already completed
	if(XONLINETASK_STATUS_AVAILABLE(m_hrLastResult))
		return m_hrLastResult;

	CAsyncTask::TaskContinue();

	// Only process this if the task hasn't already completed
	if(XONLINETASK_STATUS_AVAILABLE(m_hrLastResult))
	{
		if(FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CMatchSessionCreateTask: Session creation failed asynchronously", m_hrLastResult);
			return m_hrLastResult;
		}

		hr = XOnlineMatchSessionGetInfo(m_hTask, &m_SessionID, &m_SessionKey);
		if(hr != S_OK)
		{
			LOGTASKINFOHR(m_hLog, "CMatchSessionCreateTask: Failed getting session ID", m_hrLastResult);

			return m_hrLastResult;
		}

		m_fSessionIDSet = TRUE;
		m_fSessionCreated = TRUE;
	}

	return m_hrLastResult;
}

void CMatchSessionCreateTask::GetSessionInfo(XNKID *pSessionID, XNKEY *pSessionKey)
{
	if(pSessionID)
		memcpy(pSessionID, &m_SessionID, sizeof(XNKID));

	if(pSessionKey)
		memcpy(pSessionKey, &m_SessionKey, sizeof(XNKEY));
}

void CMatchSessionCreateTask::SetAvailableSlots(DWORD dwPublicCurrent, DWORD dwPublicAvailable, DWORD dwPrivateCurrent, DWORD dwPrivateAvailable)
{
	m_dwPublicCurrent = dwPublicCurrent;
	m_dwPrivateCurrent = dwPrivateCurrent;
	m_dwPublicAvailable = dwPublicAvailable;
	m_dwPrivateAvailable = dwPrivateAvailable;
}

void CMatchSessionCreateTask::SetSessionID(XNKID *pSessionID)
{
	if(pSessionID)
	{
		memcpy(&m_SessionID, pSessionID, sizeof(m_SessionID));

		// If the session ID is non-zero, then the match operation will be an
		// update of an existing session
		for(WORD i = 0; i < sizeof(pSessionID->ab) / sizeof(BYTE); i++)
		{
			if(pSessionID->ab[i])
				break;
		}
		
		// If this is true, we have a session update
		if(i < sizeof(pSessionID->ab) / sizeof(BYTE))
		{
			m_fSessionIDSet = TRUE;
		}
		else
		{
			m_fSessionIDSet = FALSE;
		}
	}
}

void CMatchSessionCreateTask::KeepSessionAfterClose()
{
	m_fDeleteSessionOnClose = FALSE;
}

BOOL CMatchSessionCreateTask::IsSessionCreated()
{
	return m_fSessionCreated;
}
