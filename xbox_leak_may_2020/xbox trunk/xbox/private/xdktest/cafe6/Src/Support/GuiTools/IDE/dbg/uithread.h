///////////////////////////////////////////////////////////////////////////////
//  UITHREAD.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UIThreads class
//

#ifndef __UITHREAD_H__
#define __UITHREAD_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "dbgxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif


// BEGIN_CLASS_HELP
// ClassName: CThreadInfo
// BaseClass: none
// Category: Debug
// END_CLASS_HELP

class CThreadInfo
{
public:
	BOOL bHasFocus;
	CString strID;
	int intSuspend;
	int intPriority;
	CString strLocation;
};


///////////////////////////////////////////////////////////////////////////////
//  UIThreads class

// BEGIN_CLASS_HELP
// ClassName: UIThreads
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIThreads : public UIDialog
	{

// base class overrides
public:	
	UIWND_COPY_CTOR (UIThreads, UIDialog) ;
	virtual CString ExpectedTitle (void) const{return GetLocString(IDSS_THREAD_TITLE);}
	virtual BOOL Activate(void);


	// Internal Utilities
	private:
	CString ThreadToString(DWORD thread);

	// Utilities
	public:
		BOOL FreezeThread(DWORD thread);
		BOOL ThawThread(DWORD thread);
		BOOL SetCurrentThread(DWORD thread);
		DWORD  GetCurrentThread(void);
		BOOL GetThreadInfo(CThreadInfo& ThreadInfo);
		BOOL SetThreadFocus(LPCSTR szThreadID);
		BOOL SuspendThread(LPCSTR szThreadID);
		BOOL ResumeThread(LPCSTR szThreadID);
		BOOL SelectThread(LPCSTR szThreadID);
	};

#endif // __UITHREAD_H__
