// FriendEnumTask.h: interface for the CFriendEnumTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

class CFriendEnumTask : public CAsyncTask  
{
public:
	BOOL IsFriendListUpToDate();
	DWORD GetLatestFriendList(XONLINE_FRIEND **ppFriendList);
	HRESULT TaskContinue();
	void SetUserIndex(DWORD dwUserIndex);
	BOOL StartTask();
	CFriendEnumTask();
	virtual ~CFriendEnumTask();

protected:
	DWORD m_dwFriendsCount;
	DWORD m_dwUserIndex;
	XONLINE_FRIEND *m_pFriendArray;
};
