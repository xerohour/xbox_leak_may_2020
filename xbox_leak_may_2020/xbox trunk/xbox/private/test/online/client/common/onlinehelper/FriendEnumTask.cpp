// FriendEnumTask.cpp: implementation of the CFriendEnumTask class.
//
//////////////////////////////////////////////////////////////////////

#include "FriendEnumTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFriendEnumTask::CFriendEnumTask()
{
	m_pFriendArray = new XONLINE_FRIEND[MAX_FRIENDS];
	memset(m_pFriendArray, 0, sizeof(XONLINE_FRIEND) * MAX_FRIENDS);
	m_dwUserIndex = 0;
	m_dwFriendsCount = 0;
}

CFriendEnumTask::~CFriendEnumTask()
{
	delete [] m_pFriendArray;
}

BOOL CFriendEnumTask::StartTask()
{
	HRESULT hr = S_OK;

	// Start enumeration
	m_hrLastResult = XOnlineFriendsEnumerate(m_dwUserIndex, m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendEnumTask: XOnlineFriendsEnumerate failed", m_hrLastResult);
        return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

void CFriendEnumTask::SetUserIndex(DWORD dwUserIndex)
{
	m_dwUserIndex = dwUserIndex;
}

HRESULT CFriendEnumTask::TaskContinue()
{
	CAsyncTask::TaskContinue();

	// Only get the final result when the task first completes
	if(XONLINETASK_STATUS_AVAILABLE(m_hrLastResult) && (XONLINE_S_NOTIFICATION_UPTODATE != m_hrLastResult))
	{
		LOGTASKINFO(m_hLog, "CFriendEnumTask: XOnlineFriendsEnumerate task failed asynchronously");
	}

	return m_hrLastResult;
}

DWORD CFriendEnumTask::GetLatestFriendList(XONLINE_FRIEND **ppFriendList)
{

	m_dwFriendsCount = XOnlineFriendsGetLatest(m_dwUserIndex, MAX_FRIENDS, m_pFriendArray);

	if(ppFriendList)
		*ppFriendList = m_pFriendArray;

	return m_dwFriendsCount;
}

BOOL CFriendEnumTask::IsFriendListUpToDate()
{
	return (XONLINE_S_NOTIFICATION_UPTODATE == m_hrLastResult);
}
