///////////////////////////////////////////////////////////////////////////////
//  COTHREAD.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the COThreads class
//

#include "stdafx.h"
#include "cothread.h"
#include "testxcpt.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "..\shl\uioutput.h"
#include "mstwrap.h"
#include "..\..\testutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::FreezeThread(int thread)
// Description: Suspend the specified thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to suspend.
// END_HELP_COMMENT
BOOL COThreads::FreezeThread(DWORD thread)
	{
	EXPECT(uithread.Activate());
	EXPECT(uithread.FreezeThread(thread));
	EXPECT(uithread.OK());
	// verify really done, log warning return FALSE if not
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::ThawThread(int thread)			
// Description: Resume the specified thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to resume.
// END_HELP_COMMENT
BOOL COThreads::ThawThread(DWORD thread)			
	{
	EXPECT(uithread.Activate());
	EXPECT(uithread.ThawThread(thread));
	EXPECT(uithread.OK());
	// verify really done, log warning return FALSE if not
	return TRUE;
	};


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::SetCurrentThread(int thread)
// Description: Set the specified thread to be the current thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to activate.
// END_HELP_COMMENT
BOOL COThreads::SetCurrentThread(DWORD thread)

{
	const char* const THIS_FUNCTION = "COThreads::SetCurrentThread()";

	if(!uithread.Activate())
	{
		LOG->RecordInfo("ERROR in %s: could not open Threads dialog.", THIS_FUNCTION);
		return FALSE;
	}
	
	if(!uithread.SetCurrentThread(thread))
	{
		LOG->RecordInfo("ERROR in %s: could not give focus to thread %d.", THIS_FUNCTION, thread);
		uithread.Cancel();
		return FALSE;
	}

	if(uithread.OK())
	{
		LOG->RecordInfo("ERROR in %s: could not OK Threads dialog.", THIS_FUNCTION);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: INT COThreads::GetCurrentThread(void)
// Description: Get the thread ID of the currently active thread.
// Return: An unsigned long that specifies the currently active thread.
// END_HELP_COMMENT
DWORD  COThreads::GetCurrentThread(void)
	{
	EXPECT(uithread.Activate());
	DWORD temp = uithread.GetCurrentThread();
	EXPECT(uithread.OK());
	return temp;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::CurrentThreadIs(int thread)
// Description: Determine if the current thread ID matches the given thread ID.
// Return: A Boolean value that indicates whether the current thread ID matches the given thread ID.
// Param: thread An unsigned long that contains the thread ID of the thread.
// END_HELP_COMMENT
BOOL COThreads::CurrentThreadIs(DWORD thread)
	{
	EXPECT(uithread.Activate());
	DWORD temp = uithread.GetCurrentThread();
	if (temp!=thread)
	{
		// log warning
		LOG->RecordInfo( (LPCSTR) "The Current Thread is %ul, expected %ul", temp, thread);
	}
	EXPECT(uithread.OK());
	return thread==temp;
	};


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::VerifyThreadInfo(int intThread, CThreadInfo& ThreadInfo)
// Description: Determines if the debugger knows the correct information about a thread.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: ThreadInfo - A CThreadInfo reference that contains the expected information about the thread.
// END_HELP_COMMENT
BOOL COThreads::VerifyThreadInfo(CThreadInfo& ThreadInfo)

{
	char szThisFunc[] = "COThreads::VerifyThreadInfo";

	// activate the threads dlg.
	if(!uithread.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate threads dlg.", szThisFunc);
		return FALSE;
	}

	CThreadInfo ThreadInfoActual;
	// set the ID field so UIThreads::GetThreadInfo() knows which thread to get info for.
	ThreadInfoActual.strID = ThreadInfo.strID;

	// get the info for the thread from the threads dlg.
	if(!uithread.GetThreadInfo(ThreadInfoActual))
	{
		LOG->RecordInfo("ERROR in %s: couldn't get info for thread ID '%s'.", szThisFunc, ThreadInfo.strID);
		uithread.Cancel();
		return FALSE;
	}

	// verify each piece of thread info.
	if(ThreadInfoActual.bHasFocus != ThreadInfo.bHasFocus)
	{
		LOG->RecordInfo("ERROR in %s: expected focus for thread ID '%s' didn't match actual focus.",
						szThisFunc, ThreadInfo.strID);
		uithread.Cancel();
		return FALSE;
	}

	if(ThreadInfoActual.intSuspend != ThreadInfo.intSuspend)
	{
		LOG->RecordInfo("ERROR in %s: expected suspend count for thread ID '%s' was %d. Actual suspend count was %d.",
						szThisFunc, ThreadInfo.strID, ThreadInfo.intSuspend, ThreadInfoActual.intSuspend);
		uithread.Cancel();
		return FALSE;
	}

	if(ThreadInfoActual.intPriority != ThreadInfo.intPriority)
	{
		LOG->RecordInfo("ERROR in %s: expected priority for thread ID '%s' was %d. Actual priority was %d.",
						szThisFunc, ThreadInfo.strID, ThreadInfo.intPriority, ThreadInfoActual.intPriority);
		uithread.Cancel();
		return FALSE;
	}

	if(ThreadInfoActual.strLocation != ThreadInfo.strLocation)
	{
		LOG->RecordInfo("ERROR in %s: expected location for thread ID '%s' was '%s'. Actual location was '%s'.", 
						szThisFunc, ThreadInfo.strID, ThreadInfo.strLocation, ThreadInfoActual.strLocation);
		uithread.Cancel();
		return FALSE;
	}

	// close the threads dlg without risking any changes (cancel it).
	if(uithread.Cancel() != NULL)
	{
		LOG->RecordInfo("ERROR in %s: couldn't close threads dlg.", szThisFunc);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::SetThreadFocus(LPCSTR szThreadID)
// Description: Sets debugger focus to a specified thread.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to set focus to.
// END_HELP_COMMENT
BOOL COThreads::SetThreadFocus(LPCSTR szThreadID)

{
	char szThisFunc[] = "COThreads::SetThreadFocus";

	// activate the threads dlg.
	if(!uithread.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate threads dlg.", szThisFunc);
		return FALSE;
	}

	// set the thread focus.
	if(!uithread.SetThreadFocus(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't set focus on thread with ID '%s'.", szThisFunc, szThreadID);
		uithread.Cancel();
		return FALSE;
	}

	// ok the the threads dlg.
	if(uithread.OK() != NULL)
	{
		LOG->RecordInfo("ERROR in %s: couldn't OK threads dlg.", szThisFunc);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::VerifyThreadExitCode(LPCSTR szThreadID, int intExitCode)
// Description: Verifies that the exit code of the specified thread is as expected.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the thread ID of the thread in question.
// Param: intExitCode - An integer specifying the expected exit code.
// END_HELP_COMMENT
BOOL COThreads::VerifyThreadExitCode(LPCSTR szThreadID, int intExitCode)

{
	char szThisFunc[] = "COThreads::VerifyThreadExitCode";
	
	if((CString)szThreadID == "")
	{
		LOG->RecordInfo("ERROR in %s: expected thread ID is blank.", szThisFunc);
		return FALSE;
	}

	UIOutput uioutput;

	// TODO(michma - 2/8/98): UIOutput::Activate() works but does not return TRUE.
	// activate the output window.
	/*
	if(!uioutput.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate output window.", szThisFunc);
		return FALSE;
	}
	*/
	uioutput.Activate();

	// select all the contents of the output window.
	UIWB.DoCommand(ID_EDIT_SELECT_ALL, DC_MNEMONIC);
	// copy it to the clipboard and load it into a string.
	MST.DoKeys("^(c)");
	CString strClip;
	GetClipText(strClip);
	// convert the exit code to the proper string formats.
	char szExitCode[12];
	sprintf(szExitCode, "%d", intExitCode);
	// convert the string id to the proper string format (uppercase and with leading 0's truncated).
	CString strThreadID = szThreadID;
	strThreadID.MakeUpper();
	int iNonZero = strThreadID.FindOneOf("123456789ABCDEF");
	strThreadID = strThreadID.Mid(iNonZero);
	// create the string that we are searching for in the output window.
	CString strThreadExitCode = strThreadID + " " + GetLocString(IDSS_THREAD_EXIT_CODE) + " " + szExitCode;
	
	// verify that the output window contains the correct thread exit code string.
	if(strClip.Find(strThreadExitCode) == -1)
	{
		LOG->RecordInfo("ERROR in %s: could not find '%s' in '%s'.", szThisFunc, strThreadExitCode, strClip);
		return FALSE;
	}

	return TRUE; 
}


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::SuspendThread(LPCSTR szThreadID)
// Description: Suspends a thread.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to suspend.
// END_HELP_COMMENT
BOOL COThreads::SuspendThread(LPCSTR szThreadID)

{
	char szThisFunc[] = "COThreads::SuspendThread";

	// activate the threads dlg.
	if(!uithread.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate threads dlg.", szThisFunc);
		return FALSE;
	}

	// suspend the thread.
	if(!uithread.SuspendThread(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't suspend thread with ID '%s'.", szThisFunc, szThreadID);
		uithread.Cancel();
		return FALSE;
	}

	// ok the the threads dlg.
	if(uithread.OK() != NULL)
	{
		LOG->RecordInfo("ERROR in %s: couldn't OK threads dlg.", szThisFunc);
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COThreads::ResumeThread(LPCSTR szThreadID)
// Description: Resumes a thread.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to resume.
// END_HELP_COMMENT
BOOL COThreads::ResumeThread(LPCSTR szThreadID)

{
	char szThisFunc[] = "COThreads::ResumeThread";

	// activate the threads dlg.
	if(!uithread.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate threads dlg.", szThisFunc);
		return FALSE;
	}

	// resume the thread.
	if(!uithread.ResumeThread(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't resume thread with ID '%s'.", szThisFunc, szThreadID);
		uithread.Cancel();
		return FALSE;
	}

	// ok the the threads dlg.
	if(uithread.OK() != NULL)
	{
		LOG->RecordInfo("ERROR in %s: couldn't OK threads dlg.", szThisFunc);
		return FALSE;
	}

	return TRUE;
}

