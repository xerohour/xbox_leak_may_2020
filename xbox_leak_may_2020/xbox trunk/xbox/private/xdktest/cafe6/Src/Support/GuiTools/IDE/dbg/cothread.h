///////////////////////////////////////////////////////////////////////////////
//  COTHREAD.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the COThreads class
//

#ifndef __COTHREAD_H__
#define __COTHREAD_H__

#include "uithread.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COThreads class

// BEGIN_CLASS_HELP
// ClassName: COThreads
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COThreads

	{
	// Data
	private:
		UIThreads uithread;

	// Utilities
	public:
		BOOL FreezeThread(DWORD thread);
		BOOL ThawThread(DWORD thread);			
		BOOL SetCurrentThread(DWORD thread);
		DWORD  GetCurrentThread(void);
		BOOL CurrentThreadIs(DWORD thread);
		BOOL VerifyThreadInfo(CThreadInfo& ThreadInfo);
		BOOL SetThreadFocus(LPCSTR szThreadID);
		BOOL VerifyThreadExitCode(LPCSTR szThreadID, int intExitCode);
		BOOL SuspendThread(LPCSTR szThreadID);
		BOOL ResumeThread(LPCSTR szThreadID);
	};

#endif // __COTHREAD_H__
