// LogonTask.h: interface for the CLogonTask class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AsyncTask.h"

#define MAX_LOGON_SERVICES 10

class CLogonTask : public CAsyncTask  
{
public:
	void UseDefaultUsers();
	BOOL SetUsers(PXONLINE_USER pUsersArray);
	BOOL IsConnectionEstablished();
	BOOL IsTaskComplete();
	DWORD GetUsersCount();
	HRESULT TaskContinue();
	BOOL SetServices(DWORD *paServices, DWORD dwServicesCount);
	BOOL StartTask();
	CLogonTask();
	virtual ~CLogonTask();

protected:
	DWORD m_dwServicesCount;
	DWORD m_dwServices[MAX_LOGON_SERVICES];
	DWORD m_dwUsersCount;
	XONLINE_USER *m_pUsersArray;
private:
	BOOL m_fDefaultUsers;
};
