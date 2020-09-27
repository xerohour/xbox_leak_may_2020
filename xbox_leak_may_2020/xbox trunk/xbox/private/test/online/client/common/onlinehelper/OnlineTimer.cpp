// OnlineTimer.cpp: implementation of the COnlineTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "OnlineTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COnlineTimer::COnlineTimer()
{
	m_dwAllowedTime = 0;
	m_dwStartTick = 0;
	m_dwLastTick = 0;
	m_fStarted = FALSE;
}

COnlineTimer::~COnlineTimer()
{

}

COnlineTimer::COnlineTimer(const COnlineTimer &OnlineTimer)
{
	m_dwAllowedTime = OnlineTimer.m_dwAllowedTime;
	m_dwLastTick = OnlineTimer.m_dwLastTick;
	m_dwStartTick = OnlineTimer.m_dwStartTick;
	m_fStarted = OnlineTimer.m_fStarted;
}

COnlineTimer& COnlineTimer::operator = (const COnlineTimer& OnlineTimer)
{
	m_dwAllowedTime = OnlineTimer.m_dwAllowedTime;
	m_dwLastTick = OnlineTimer.m_dwLastTick;
	m_dwStartTick = OnlineTimer.m_dwStartTick;
	m_fStarted = OnlineTimer.m_fStarted;

	return *this;
}

DWORD COnlineTimer::GetTimeRemaining()
{
	DWORD dwElapsedTime = 0;

	// If the timer was was never called, then always return
	// that there is 0 time remaining
	if(!m_fStarted)
		return 0;

	m_dwLastTick = GetTickCount();

	if(m_dwLastTick < m_dwStartTick)
	{
		// Handle tick wraparound
		dwElapsedTime = MAXDWORD - m_dwStartTick + m_dwLastTick;
	}
	else
	{
		dwElapsedTime = m_dwLastTick - m_dwStartTick;
	}

	if(dwElapsedTime > m_dwAllowedTime)
		return 0;

	return m_dwAllowedTime - dwElapsedTime;
}

BOOL COnlineTimer::HasTimeExpired()
{
	return (GetTimeRemaining() == 0);	
}

void COnlineTimer::Start()
{
	m_fStarted = TRUE;
	m_dwStartTick = GetTickCount();
}

void COnlineTimer::SetAllowedTime(DWORD dwAllowedTime)
{
	m_dwAllowedTime = dwAllowedTime;
}
