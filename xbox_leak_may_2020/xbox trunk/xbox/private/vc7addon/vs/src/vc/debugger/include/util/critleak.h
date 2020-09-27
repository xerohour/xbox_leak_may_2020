
#pragma once

#include "util\debug.h"

#ifdef _DEBUG

extern "C" void * _ReturnAddress();


class dbgCRITICAL_SECTION
{
public:
	void
	Initialize(
		)
	{
		InitializeCriticalSection(&m_cs);
		m_ulLockCount = 0;
		m_dwOwningThread = 0;
	}

	void
	Enter( 
		)
	{
		EnterCriticalSection(&m_cs);
		if (m_ulLockCount == 0) {
			m_dwOwningThread = GetCurrentThreadId();
		}
		Assert(m_dwOwningThread == GetCurrentThreadId());
		++m_ulLockCount;
	}

//	BOOL
//	TryEnter(
//		)
//	{
//		if (TryEnterCriticalSection(&m_cs)) {
//			// Same as code for Enter().
//			if (m_ulLockCount == 0) {
//				m_dwOwningThread = GetCurrentThreadId();
//			}
//			Assert(m_dwOwningThread == GetCurrentThreadId());
//			++m_ulLockCount;
//		} else {
//			Assert(!IsOwnedByThisThread());
//		}
//	}

	void
	Leave(
	   )
	{
		--m_ulLockCount;
		Assert(m_dwOwningThread == GetCurrentThreadId());
		if (m_ulLockCount == 0) {
			m_dwOwningThread = 0;
		}
		LeaveCriticalSection(&m_cs);
	}

	void
	Delete(
	    )
	{
		Assert(m_dwOwningThread == 0);
		Assert(m_ulLockCount == 0);
		DeleteCriticalSection(&m_cs);
	}

	// This isn't suggested for anything other than debugging use.
	//	Its return result can be incorrect by the time its
	//	evaluated.
	bool
	IsOwned
		() const
	{
		return (m_ulLockCount > 0);
	}

	// This should always be safe and give consistent results.
	bool
	IsOwnedByThisThread(
		) const
	{
		return (m_dwOwningThread == GetCurrentThreadId());
	}

	operator LPCRITICAL_SECTION(
			) 
	{ 
		return &m_cs; 
	};


private:

	CRITICAL_SECTION m_cs;
	ULONG m_ulLockCount;
	DWORD m_dwOwningThread;
};

#define CRITICAL_SECTION dbgCRITICAL_SECTION

#define	InitializeCriticalSection(s)	InitializeCriticalSection_dbg(__FILE__,__LINE__,(dbgCRITICAL_SECTION*) s,_ReturnAddress())
#define	DeleteCriticalSection		DeleteCriticalSection_dbg
#define	LeaveCriticalSection(s)		LeaveCriticalSection_dbg((dbgCRITICAL_SECTION*) s,__FILE__,__LINE__)

EXTERN_C void 
WINAPI 
InitializeCriticalSection_dbg( 
	const char*, 
	int, 
	dbgCRITICAL_SECTION* lpSec, 
	void*pRet 
	);

EXTERN_C void 
WINAPI 
DeleteCriticalSection_dbg( 
	dbgCRITICAL_SECTION* lpsec 
	);



EXTERN_C void 
WINAPI 
LeaveCriticalSection_dbg(
	dbgCRITICAL_SECTION* lpsec, 
	const char *, 
	int
	);
EXTERN_C void 
WINAPI 
EnterCriticalSection_dbg(
	dbgCRITICAL_SECTION* lpsec , 
	const char *, 
	int
	);

EXTERN_C void 
WINAPI 
EnterCriticalSectionEx_dbg(
	dbgCRITICAL_SECTION* lpsec, 
	const char *, 
	int
	);

BOOL
WINAPI
IsCriticalSectionOwned(
	dbgCRITICAL_SECTION* CriticalSection
	);

BOOL
WINAPI
IsCriticalSectionUnOwned(
	dbgCRITICAL_SECTION CriticalSection
	);

#ifdef CRITS_NO_REENTRY 
#define	EnterCriticalSection(s)		EnterCriticalSection_dbg((dbgCRITICAL_SECTION*)s,__FILE__,__LINE__)
#define	EnterCriticalSectionEx(s)	EnterCriticalSectionEx_dbg((dbgCRITICAL_SECTION*)s,__FILE__,__LINE__)
#else
#define	EnterCriticalSection(s)		EnterCriticalSectionEx_dbg((dbgCRITICAL_SECTION*)s,__FILE__,__LINE__)
#define	EnterCriticalSectionEx(s)	EnterCriticalSectionEx_dbg((dbgCRITICAL_SECTION*)s,__FILE__,__LINE__)
#endif

#else
#define	EnterCriticalSectionEx(s) EnterCriticalSection(s)
#endif
