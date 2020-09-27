///////////////////////////////////////////////////////////////////////////////
//  UITHREAD.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the UIThreads class
//

#include "stdafx.h"
#include "uithread.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vcpp32.h"
#include "..\shl\uwbframe.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::Activate(void)
// Description: Open the Threads dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIThreads::Activate(void)
{
	UIWB.DoCommand(IDM_RUN_THREADS, DC_MNEMONIC);
	if (WaitAttachActive(5000)) {
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIThreads::ThreadToString(DWORD thread)
// Description: Convert a thread ID value to a string. For example, the thread value 0x95 is converted to the string 00000095.
// Return: A CString that contains the thread value converted to a string.
// Param: thread An unsigned long that contains the thread value.
// END_HELP_COMMENT
CString UIThreads::ThreadToString(DWORD thread)
	{
	CString sThread;
	sprintf(sThread.GetBuffer(255), "%.8x", thread);
	return sThread;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::FreezeThread(DWORD thread)
// Description: Suspend the specified thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to suspend.
// END_HELP_COMMENT
BOOL UIThreads::FreezeThread(DWORD thread)
	{
	CString sListItem;
	CString sThread = ThreadToString(thread);
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_THREAD_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_THREAD_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_THREAD_LIST), x, sListItem);
		if (sListItem.Find(sThread) != -1 ) { break; };   // is thread in list item?
	};
	if ( x > limit )  return FALSE;      //thread not found
	MST.WListItemClk(GetLabel(VCPP32_DLG_THREAD_LIST), x);
	EXPECT(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_THREAD_SUSPEND)));
	MST.WButtonClick(GetLabel(VCPP32_DLG_THREAD_SUSPEND));
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::ThawThread(DWORD thread)
// Description: Resume the specified thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to resume.
// END_HELP_COMMENT
BOOL UIThreads::ThawThread(DWORD thread)
	{
	CString sListItem;
	CString sThread = ThreadToString(thread);
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_THREAD_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_THREAD_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_THREAD_LIST), x, sListItem);
		if (sListItem.Find(sThread) != -1 ) { break; };   // is thread in list item?
	};
	if ( x > limit )  return FALSE;      //thread not found
	MST.WListItemClk(GetLabel(VCPP32_DLG_THREAD_LIST), x);
	EXPECT(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_THREAD_RESUME)));
	MST.WButtonClick(GetLabel(VCPP32_DLG_THREAD_RESUME));
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::SetCurrentThread(DWORD thread)
// Description: Set the specified thread to be the current thread.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: thread An unsigned long that contains the thread ID of the thread to activate.
// END_HELP_COMMENT
BOOL UIThreads::SetCurrentThread(DWORD thread)
	{
	CString sListItem;
	CString sThread = ThreadToString(thread);
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_THREAD_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_THREAD_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_THREAD_LIST), x, sListItem);
		if (sListItem.Find(sThread) != -1 ) { break; };   // is thread in list item?
	};
	if ( x > limit )  return FALSE;      //thread not found
	MST.WListItemClk(GetLabel(VCPP32_DLG_THREAD_LIST), x);
	EXPECT(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_THREAD_FOCUS)));
	MST.WButtonClick(GetLabel(VCPP32_DLG_THREAD_FOCUS));
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: INT  UIThreads::GetCurrentThread(void)
// Description: Get the thread ID of the currently active thread.
// Return: An unsigned long that specifies the currently active thread.
// END_HELP_COMMENT
DWORD  UIThreads::GetCurrentThread(void)   //assumes first item in listbox is current thread.
	{
	CString sListItem;
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_THREAD_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_THREAD_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_THREAD_LIST), x, sListItem);
		if (sListItem[0]=='*' ) { break; };   // this thread has the focus
	};
	ASSERT(x <= limit);
	CString sThread= sListItem.Mid(1,8);
	return strtoul(sThread,NULL,10);
	};


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::GetThreadInfo(CThreadInfo& ThreadInfo)
// Description: Gets info about the specified thread from the Threads dlg.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: ThreadInfo - A CThreadInfo object that receives information about the thread. The strID field of this object specifies for which thread to get info.
// END_HELP_COMMENT
BOOL UIThreads::GetThreadInfo(CThreadInfo& ThreadInfo)

{
	char szThisFunction[] = "UIThreads::GetThreadInfo";

	// verify threads dlg is up.
	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in %s: threads dlg isn't active.", szThisFunction);
		return FALSE;
	}

	// stores the line of info for a thread in the threads list.
	CString strThreadInfo;
	int intThreadCount = MST.WListCount("@1");

	// search each thread info line in the list until we find the one containing the specified ID.
	for(int intThread = 1; intThread <= intThreadCount; intThread++)
	
	{
		// get the line containing the next thread's info.
		MST.WListItemText("@1", intThread, strThreadInfo);
		
		// verify that the string is long enough to contain a valid thread id 
		// (8 hex digits, prepended by 1 char for the focus state);
		if(strThreadInfo.GetLength() < 9)
		{
			LOG->RecordInfo("ERROR in %s: thread info string %d not long enough to specify id: \"%s\".", 
				szThisFunction, intThread, strThreadInfo);
			return FALSE;
		}

		// check if we found the matching thread id.
		if(strThreadInfo.Mid(1, 8) == ThreadInfo.strID)
			break;
	}
	
	// verify that we found the specified thread in the list.
	if(intThread > intThreadCount)
	{
		LOG->RecordInfo("ERROR in %s: thread with ID %s does not exist.", szThisFunction, ThreadInfo.strID);
		return FALSE;
	}

	// determine the focus state.
	if(strThreadInfo[0] == '*')
		ThreadInfo.bHasFocus = TRUE;
	else
		ThreadInfo.bHasFocus = FALSE;

	// truncate the string to the left of the suspend count.
	strThreadInfo = strThreadInfo.Right(strThreadInfo.GetLength() - 10);

	// make sure the string is long enough to hold the suspend count.
	if(strThreadInfo.GetLength() < 1)
	{
		LOG->RecordInfo("ERROR in %s: thread info string not long enough to specify suspend count: \"%s\".", 
			szThisFunction, strThreadInfo);
		return FALSE;
	}

	// find the space that terminates the suspend count.
	int iSpace = strThreadInfo.Find(" ");

	// make sure we found it.
	if(iSpace == -1)
	{
		LOG->RecordInfo("ERROR in %s: couldn't find space that terminates suspend count: \"%s\".", 
			szThisFunction, strThreadInfo);
		return FALSE;
	}

	// extract the suspend count.
	ThreadInfo.intSuspend = atoi(strThreadInfo.Mid(0, iSpace));	
	// truncate the string to the left of the priority.
	strThreadInfo = strThreadInfo.Right(strThreadInfo.GetLength() - iSpace - 1);
	strThreadInfo.TrimLeft();
	// find the space that terminates the priority count.
	iSpace = strThreadInfo.Find(" ");

	// make sure we found it.
	if(iSpace == -1)
	{
		LOG->RecordInfo("ERROR in %s: couldn't find space that terminates priority: \"%s\".", 
			szThisFunction, strThreadInfo);
		return FALSE;
	}

	// extract the priority.
	ThreadInfo.intPriority = atoi(strThreadInfo.Mid(0, iSpace));	
	// truncate the string to the left of the location.
	strThreadInfo = strThreadInfo.Right(strThreadInfo.GetLength() - iSpace - 1);
	strThreadInfo.TrimLeft();
	// the remaining string should be the location name (trim the end just in case).
	strThreadInfo.TrimRight();
	ThreadInfo.strLocation = strThreadInfo;
	
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::SetThreadFocus(LPCSTR szThreadID)
// Description: Selects a thread in the Threads dlg and clicks the Set Focus button.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to set focus to.
// END_HELP_COMMENT
BOOL UIThreads::SetThreadFocus(LPCSTR szThreadID)

{
	char szThisFunction[] = "UIThreads::SetThreadFocus";

	// select the thread.
	if(!SelectThread(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't select thread with ID '%s'.", szThisFunction, szThreadID);
		return FALSE;
	}

	// set the focus to it.
	if(MST.WButtonEnabled(GetLocString(IDSS_THREAD_SET_FOCUS)))
		MST.WButtonClick(GetLocString(IDSS_THREAD_SET_FOCUS));
	else
	{
		LOG->RecordInfo("ERROR in %s: couldn't find '%s' button in threads dlg.",
			szThisFunction, GetLocString(IDSS_THREAD_SET_FOCUS));
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::SuspendThread(LPCSTR szThreadID)
// Description: Selects a thread in the Threads dlg and clicks the Suspend button.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to suspend.
// END_HELP_COMMENT
BOOL UIThreads::SuspendThread(LPCSTR szThreadID)

{
	char szThisFunction[] = "UIThreads::SuspendThread";

	// select the thread.
	if(!SelectThread(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't select thread with ID '%s'.", szThisFunction, szThreadID);
		return FALSE;
	}

	// suspend it.
	if(MST.WButtonEnabled(GetLocString(IDSS_THREAD_SUSPEND)))
		MST.WButtonClick(GetLocString(IDSS_THREAD_SUSPEND));
	else
	{
		LOG->RecordInfo("ERROR in %s: couldn't find '%s' button in threads dlg.",
			szThisFunction, GetLocString(IDSS_THREAD_SUSPEND));
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::ResumeThread(LPCSTR szThreadID)
// Description: Selects a thread in the Threads dlg and clicks the Resume button.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to resume.
// END_HELP_COMMENT
BOOL UIThreads::ResumeThread(LPCSTR szThreadID)

{
	char szThisFunction[] = "UIThreads::ResumeThread";

	// select the thread.
	if(!SelectThread(szThreadID))
	{
		LOG->RecordInfo("ERROR in %s: couldn't select thread with ID '%s'.", szThisFunction, szThreadID);
		return FALSE;
	}

	// resume it.
	if(MST.WButtonEnabled(GetLocString(IDSS_THREAD_RESUME)))
		MST.WButtonClick(GetLocString(IDSS_THREAD_RESUME));
	else
	{
		LOG->RecordInfo("ERROR in %s: couldn't find '%s' button in threads dlg.",
			szThisFunction, GetLocString(IDSS_THREAD_RESUME));
		return FALSE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIThreads::SelectThread(LPCSTR szThreadID)
// Description: Selects a thread in the Threads dlg.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szThreadID - A string specifying the ID of the thread to select.
// END_HELP_COMMENT
BOOL UIThreads::SelectThread(LPCSTR szThreadID)

{
	char szThisFunction[] = "UIThreads::SelectThread";

	// verify threads dlg is up.
	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in %s: threads dlg isn't active.", szThisFunction);
		return FALSE;
	}

	// stores the line of info for a thread in the threads list.
	CString strThreadInfo;
	int intThreadCount = MST.WListCount("@1");

	// search each thread info line in the list until we find the one containing the ID.
	for(int intThread = 1; intThread <= intThreadCount; intThread++)
	
	{
		// get the line containing the next thread's info.
		MST.WListItemText("@1", intThread, strThreadInfo);
		
		// check if we found the matching thread ID.
		if(strThreadInfo.Find(szThreadID) != -1)
		{
			// select the thread in the dlg.
			MST.WListItemClk("@1", intThread);
			return TRUE;
		}
	}

	LOG->RecordInfo("ERROR in %s: couldn't find thread with ID \"%s\".", szThisFunction, szThreadID);
	return FALSE;
}
