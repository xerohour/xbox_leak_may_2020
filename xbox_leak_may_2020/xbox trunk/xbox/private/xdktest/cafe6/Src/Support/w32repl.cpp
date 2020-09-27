///////////////////////////////////////////////////////////////////////////////
// 
// File: W32Repl.cpp
//
// Description: This file provides the implementations for overriddene Win32 
//              API calls.
///////////////////////////////////////////////////////////////////////////////
#include "w32repl.h"

#undef Sleep
#undef WaitForInputIdle

TARGET_DATA UINT g_nSleepModifier;		// Increases sleeps by this percentage.  100 = default behavior.
TARGET_DATA BOOL g_bInfiniteIdleWaits;	// If true, WaitForInputIdles are INFINITE.

VOID CAFE_Sleep(DWORD cMilliseconds)
{
	if (g_nSleepModifier == 100)	// Don't bother with the rest if we're at default.
		::Sleep(cMilliseconds);
	else
	{
		DWORD adj_msec = (g_nSleepModifier * cMilliseconds) / 100; // Not concerned about overload, but it's theoretically possible, though unlikely with DWORDs.

		::Sleep(adj_msec);	
	}
}

DWORD CAFE_WaitForInputIdle(HANDLE hProcess, DWORD dwTimeout)
{
	return ::WaitForInputIdle(hProcess, g_bInfiniteIdleWaits? INFINITE: dwTimeout);
}