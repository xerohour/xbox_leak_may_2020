// FriendOpTask.cpp: implementation of the CFriendOpTask class.
//
//////////////////////////////////////////////////////////////////////

#include "FriendOpTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFriendOpTask::CFriendOpTask()
{

}

CFriendOpTask::~CFriendOpTask()
{

}

BOOL CFriendOpTask::RequestFriend(DWORD dwUserIndex, XUID xuidFriend)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsRequest(dwUserIndex, xuidFriend);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsRequest failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}

BOOL CFriendOpTask::RemoveFriend(DWORD dwUserIndex, PXONLINE_FRIEND pFriend)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsRemove(dwUserIndex, pFriend);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsRemove failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}

BOOL CFriendOpTask::AnswerFriendRequest(DWORD dwUserIndex, PXONLINE_FRIEND pFriend, XONLINE_REQUEST_ANSWER_TYPE Answer)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsAnswerRequest(dwUserIndex, pFriend, Answer);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsAnswerRequest failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}

HRESULT CFriendOpTask::TaskContinue()
{
	CAsyncTask::TaskContinue();

	return m_hrLastResult;
}

BOOL CFriendOpTask::StartTask()
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsStartup(m_hWorkEvent, &m_hTask);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsStartup failed", m_hrLastResult);
        return FALSE;
	}

	// Mark the task as running so TaskContinue calls will be processed
	m_hrLastResult = XONLINETASK_S_RUNNING;

	return TRUE;
}

BOOL CFriendOpTask::GameInvite(DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendListCount, PXONLINE_FRIEND pToFriendList)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsGameInvite(dwUserIndex, SessionID, dwFriendListCount, pToFriendList);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsGameInvite failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}

BOOL CFriendOpTask::RevokeGameInvite(DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendsListCount, PXONLINE_FRIEND pToFriendList)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsRevokeGameInvite(dwUserIndex, SessionID, dwFriendsListCount, pToFriendList);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsRevokeGameInvite failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}

BOOL CFriendOpTask::AnswerGameInvite(DWORD dwUserIndex, PXONLINE_FRIEND pToFriend, XONLINE_GAMEINVITE_ANSWER_TYPE Answer)
{
	HRESULT hr = S_OK;

	m_hrLastResult = XOnlineFriendsAnswerGameInvite(dwUserIndex, pToFriend, Answer);
	if(FAILED(m_hrLastResult))
	{
		LOGTASKINFOHR(m_hLog, "CFriendOpTask: XOnlineFriendsAnswerRequest failed", m_hrLastResult);
        return FALSE;
	}

	return TRUE;
}
