// MatchSessionCreateTask.h: interface for the CMatchSessionCreateTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"
#include "OnlineTimer.h"

class CMatchSessionCreateTask : public CAsyncTask  
{
public:
	BOOL IsSessionCreated();
	void KeepSessionAfterClose();
	void SetSessionID(XNKID *pSessionID);
	void SetAvailableSlots(DWORD dwPublicCurrent, DWORD dwPublicAvailable, DWORD dwPrivateCurrent, DWORD dwPrivateAvailable);
	void GetSessionInfo(XNKID *pSessionID, XNKEY *pSessionKey);
	HRESULT TaskContinue();
	void SetAttributes(PXONLINE_ATTRIBUTE pAttributes, DWORD dwAttributesCount);
	BOOL StartTask();
	CMatchSessionCreateTask();
	virtual ~CMatchSessionCreateTask();

protected:
	BOOL m_fSessionCreated;
	BOOL m_fDeleteSessionOnClose;
	DWORD m_dwPrivateCurrent;
	DWORD m_dwPublicCurrent;
	DWORD m_dwPrivateAvailable;
	DWORD m_dwPublicAvailable;
	DWORD m_dwAttributesCount;
	PXONLINE_ATTRIBUTE m_pAttributes;
	BOOL m_fSessionIDSet;
	XNKID m_SessionID;
	XNKEY m_SessionKey;
};

