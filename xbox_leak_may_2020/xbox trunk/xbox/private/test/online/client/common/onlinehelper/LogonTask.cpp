// LogonTask.cpp: implementation of the CLogonTask class.
//
//////////////////////////////////////////////////////////////////////

#include "LogonTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogonTask::CLogonTask()
{
	m_pUsersArray = new XONLINE_USER[XONLINE_MAX_STORED_ONLINE_USERS];
	memset(m_pUsersArray, 0, sizeof(XONLINE_USER) * XONLINE_MAX_STORED_ONLINE_USERS);
	memset(m_dwServices, 0, sizeof(m_dwServices));
	m_dwUsersCount = 0;
	m_dwServicesCount = 0;
	m_fDefaultUsers = TRUE;
}

CLogonTask::~CLogonTask()
{
	delete [] m_pUsersArray;
}

BOOL CLogonTask::StartTask()
{
	// If this object has been used for a previous logon task, clean up the old task first
	if(m_hTask)
	{
		XOnlineTaskClose(m_hTask);
		m_hTask = NULL;
	}

	// By default, just grab the first four users on the box and log in with them
	if(m_fDefaultUsers)
	{
		memset(m_pUsersArray, 0, sizeof(XONLINE_USER) * XONLINE_MAX_STORED_ONLINE_USERS);
		m_hrLastResult = XOnlineGetUsers(m_pUsersArray, &m_dwUsersCount);
		if (FAILED(m_hrLastResult))
		{
			LOGTASKINFOHR(m_hLog, "CLogonTask: Couldn't get users from harddrive", m_hrLastResult);
			return FALSE;
		}
	}


	// Start login
	m_hrLastResult = XOnlineLogon(m_pUsersArray, m_dwServicesCount ? m_dwServices : NULL, m_dwServicesCount, m_hWorkEvent, &m_hTask);
	if (FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CLogonTask: XOnlineLogon failed", m_hrLastResult);
        return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

BOOL CLogonTask::SetServices(DWORD *pdwServices, DWORD dwServicesCount)
{
	if(dwServicesCount > MAX_LOGON_SERVICES)
		return FALSE;

	memcpy(m_dwServices, pdwServices, sizeof(DWORD) * dwServicesCount);

	m_dwServicesCount = dwServicesCount;

	return TRUE;
}

HRESULT CLogonTask::TaskContinue()
{
	// Only process this if the task hasn't already failed
	if(XONLINETASK_STATUS_AVAILABLE(m_hrLastResult) && (m_hrLastResult != XONLINE_S_LOGON_CONNECTION_ESTABLISHED))
		return m_hrLastResult;

	CAsyncTask::TaskContinue();

	// Only get the final result when the task first completes
	if(!XONLINETASK_STATUS_AVAILABLE(m_hrLastResult))
		return m_hrLastResult;

	return m_hrLastResult;
}

DWORD CLogonTask::GetUsersCount()
{
	DWORD dwUserIndex = 0;
	PXONLINE_USER pCurrUser = NULL;

	m_dwUsersCount = 0;
	for(dwUserIndex = 0,pCurrUser = XOnlineGetLogonUsers();((pCurrUser != NULL) && (dwUserIndex < XONLINE_MAX_LOGON_USERS)); ++pCurrUser, ++dwUserIndex)
	{
		if(pCurrUser->xuid.qwUserID != 0)
			++m_dwUsersCount;
	}

	return m_dwUsersCount;
}

BOOL CLogonTask::IsTaskComplete()
{
	return (XONLINETASK_STATUS_AVAILABLE(m_hrLastResult) && (m_hrLastResult != XONLINE_S_LOGON_CONNECTION_ESTABLISHED));
}

BOOL CLogonTask::IsConnectionEstablished()
{
	return (m_hrLastResult == XONLINE_S_LOGON_CONNECTION_ESTABLISHED);
}

BOOL CLogonTask::SetUsers(PXONLINE_USER pUsersArray)
{
	if(!pUsersArray)
		return FALSE;

	m_fDefaultUsers = FALSE;

	memcpy(m_pUsersArray, pUsersArray, sizeof(XONLINE_USER) * XONLINE_MAX_STORED_ONLINE_USERS);

	return TRUE;
}

void CLogonTask::UseDefaultUsers()
{
	m_fDefaultUsers = TRUE;
}
