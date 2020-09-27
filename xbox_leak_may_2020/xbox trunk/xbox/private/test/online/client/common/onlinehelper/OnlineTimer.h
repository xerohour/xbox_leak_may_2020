// OnlineTimer.h: interface for the COnlineTimer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>

class COnlineTimer  
{
public:
	void SetAllowedTime(DWORD dwAllowedTime);
	DWORD GetTimeRemaining();
	void Start();
	BOOL HasTimeExpired();
	COnlineTimer::COnlineTimer(const COnlineTimer &);
	COnlineTimer();
	virtual ~COnlineTimer();
protected:
	DWORD m_dwAllowedTime;
	DWORD m_dwStartTick;
	DWORD m_dwLastTick;
	BOOL m_fStarted;
};
