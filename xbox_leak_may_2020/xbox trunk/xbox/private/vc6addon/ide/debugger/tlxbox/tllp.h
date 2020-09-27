
/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    tllp.h

Abstract:

	Simple wrapper classes for events and mutexes.

Author:

	Matthew Hendel (math)
	
--*/


#ifndef _TLLP_H_
#define _TLLP_H_

#ifdef __cplusplus

#define DEBUG_DEADLOCK_TIMEOUT	INFINITE  // 90 * 1000		// 90 sec

class TEvent
{
	HANDLE	m_Event;

  public:

	TEvent(
		)
	{
		m_Event = NULL;
	}

	~TEvent(
		)
	{
		Close ();
	}

	BOOL
	Create(
		LPSECURITY_ATTRIBUTES	lpEventAttributes = NULL,
	    BOOL					fManualReset = FALSE,
	    BOOL					fInitialState = FALSE,
	    LPCTSTR					lpName = NULL
	    )
	{
		m_Event = CreateEvent (lpEventAttributes, fManualReset, fInitialState,
							   lpName);

		return (m_Event != NULL);
	}

	BOOL
	Open(
		ULONG	dwAccess,
		BOOL	fInheritHandles,
		LPCTSTR	lpName
		)
	{
		assert (!m_Event);
		m_Event = OpenEvent (dwAccess, fInheritHandles, lpName);

		return (m_Event != NULL);
	}

	BOOL
	Close(
		)
	{
		BOOL	fSucc = CloseHandle (m_Event);
		m_Event = NULL;

		return fSucc;
	}

	ULONG
	Wait(
		ULONG	TimeOut = INFINITE
		);

	BOOL
	Set(
		)
	{
		assert (m_Event);
		return SetEvent (m_Event);
	}

	BOOL
	Pulse(
		)
	{
		assert (m_Event);
		return PulseEvent (m_Event);
	}

	BOOL
	Reset(
		)
	{
		assert (m_Event);
		return ResetEvent (m_Event);
	}

};


class TMutex
{
	HANDLE	m_hMutex;

  public:

	TMutex(
		)
	{
		m_hMutex = NULL;
	}

	~TMutex(
		)
	{
		Close ();
	}

	BOOL
	Create(
		LPSECURITY_ATTRIBUTES	lpMutexAttributes = NULL,
		BOOL					fInitialOwner = FALSE,
		LPCTSTR					lpName = NULL
		)
	{
		assert (m_hMutex == NULL);

		m_hMutex = CreateMutex (lpMutexAttributes, fInitialOwner, lpName);

		return (m_hMutex != NULL);
	}
		

	BOOL
	Open(
		ULONG	DesiredAccess,
		BOOL	fInheritHandles,
		LPCTSTR	lpName
		)
	{
		m_hMutex = OpenMutex (DesiredAccess, fInheritHandles, lpName);

		return (m_hMutex != NULL);
	}

	BOOL
	Close(
		)
	{
		BOOL	fSucc = CloseHandle (m_hMutex);
		m_hMutex = NULL;

		return fSucc;
	}
		
	ULONG
	Wait(
		ULONG	TimeOut = INFINITE
		);

	BOOL
	Release(
		)
	{
		return ReleaseMutex (m_hMutex);
	}

};

extern "C"
#endif // __cplusplus

BOOL WINAPI TlDllMain(HINSTANCE, DWORD, DWORD);

#endif // _TLLP_H_

