#include "stdafx.h"

CTimer::CTimer (CTimeable *obj, size_t milliseconds)
	:m_targetObj(obj), m_milliseconds(milliseconds), 
	 m_timerAbort(NULL), m_timerThread(NULL), m_i64ExpireTSCValue(0)
{
	if (milliseconds <= 0)
		return;

	// Moved this above the beginthreadex to eliminate a potential race condition
	m_timerAbort = CreateEvent (NULL, false, false, NULL);
	if (m_timerAbort == NULL)
		throw _T("Failed to create timer event");

	DWORD result = _beginthreadex (NULL, 0, &TimerEntry, this, 0, (UINT*)&result);
	if (result == 0)
		throw _T("Failed to create timer thread");

	m_timerThread = (HANDLE) result;
}

CTimer::~CTimer ()
{
	if (m_milliseconds && m_timerAbort && m_timerThread)
	{
		SetEvent (m_timerAbort);
		WaitForSingleObject (m_timerThread, INFINITE);
	}
	CloseHandle (m_timerAbort);
	CloseHandle (m_timerThread);
}

unsigned __stdcall CTimer::TimerEntry (LPVOID pParam)
{
	CTimer *timer = (CTimer *) pParam;
	timer->TimerMain ();
	_endthreadex (0); 
	return 0;
}

void CTimer::TimerMain ()
{
	DWORD result;

#if defined( _XBOX ) // An XBOX-specific routine that uses the Time Stamp Counter for accuracy
	LARGE_INTEGER liLocalTSC;

	_asm
	{
		rdtsc
		mov liLocalTSC.LowPart, eax       ; Preserve the Least Significant 32 bits
		mov liLocalTSC.HighPart, edx      ; Preserve the Most Significant 32 bits
	}

	m_i64ExpireTSCValue = liLocalTSC.QuadPart + gi64CPUSpeed*(__int64)(m_milliseconds/1000);
	// Now wait for either an abort, or a period of time to elapse
	for (;;)
	{
		result = WaitForSingleObject (m_timerAbort, 2000);
		if (result == WAIT_TIMEOUT)
		{
			_asm
			{
				rdtsc
				mov liLocalTSC.LowPart, eax       ; Preserve the Least Significant 32 bits
				mov liLocalTSC.HighPart, edx      ; Preserve the Most Significant 32 bits
			}
			if (liLocalTSC.QuadPart >= m_i64ExpireTSCValue) // The proper number of seconds has elapsed, so fire off the timer event
			{
				m_targetObj->OnTimer ();
				break;
			}
		}
		else // Must be an abort
			break;
	}

#else // Regular Win32 
	result = WaitForSingleObject (m_timerAbort, m_milliseconds);

	if (result == WAIT_TIMEOUT)
		m_targetObj->OnTimer ();
#endif
}
