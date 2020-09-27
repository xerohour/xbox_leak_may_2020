// FriendOpTask.h: interface for the CFriendOpTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CFriendOpTask : public CAsyncTask  
{
public:
	BOOL AnswerGameInvite(DWORD dwUserIndex, PXONLINE_FRIEND pToFriend, XONLINE_GAMEINVITE_ANSWER_TYPE Answer);
	BOOL RevokeGameInvite(DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendsListCount, PXONLINE_FRIEND pToFriendList);
	BOOL GameInvite(DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendListCount,  PXONLINE_FRIEND pToFriendList);
	BOOL StartTask();
	HRESULT TaskContinue();
	BOOL AnswerFriendRequest(DWORD dwUserIndex, PXONLINE_FRIEND pFriend, XONLINE_REQUEST_ANSWER_TYPE Answer);
	BOOL RemoveFriend(DWORD dwUserIndex, PXONLINE_FRIEND pFriend);
	BOOL RequestFriend(DWORD dwUserIndex, XUID xuidFriend);
	CFriendOpTask();
	virtual ~CFriendOpTask();

};
