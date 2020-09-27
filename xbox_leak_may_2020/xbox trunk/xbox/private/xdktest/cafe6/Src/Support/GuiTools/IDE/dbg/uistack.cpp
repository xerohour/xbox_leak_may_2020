///////////////////////////////////////////////////////////////////////////////
//  UISTACK.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the UIStack class
//

#include "stdafx.h"
#include "uistack.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "Strings.h"
#include "..\sym\reg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: CString UIStack::GetFunction(int level /* 0 */)
// Description: Get the call stack description string at the given call stack level (index).
// Return: A CString that contains the call stack description string from the given call stack level.
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// END_HELP_COMMENT
CString UIStack::GetFunction(int level /* 0 */)
	{
	char buffer[10];
	CString count=itoa(level,buffer,10);
	CString Keys = "^{HOME}{DOWN "+count+"}+{END}^(c)";
	MST.DoKeys(Keys);
	CString cstr;
	GetClipText(cstr);
	return cstr;
	};

// BEGIN_HELP_COMMENT
// Function: CString UIStack::GetAllFunctions(void)
// Description: Get all the call stack frame descriptions for all frames on the call stack.
// Return: A CString that contains a newline-delimited list of call stack frames.
// END_HELP_COMMENT
CString UIStack::GetAllFunctions(void)
	{
	//
	//	Reverse selection sequence to workaround ORCA #171
//	CString Keys = "^{HOME}^(+{END})^(c)";
	CString Keys = "^{END}^(+{HOME})^(c)";

	MST.DoKeys(Keys);
	CString cstr;
	GetClipText(cstr);
	return cstr;
	};
#include "costack.h"
// BEGIN_HELP_COMMENT
// Function: int UIStack::NavigateStack(int level /* 0 */)
// Description: Navigate to the specified call stack frame & checks if the line number is correct.
// Return: A int value that indicates success. -1 if no source file is avail; 0 if not success (line number does not match). >0 - line number
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// END_HELP_COMMENT
int UIStack::NavigateStack(int level /* 0 */)
{
	CString str=GetFunction(level); //puts the cursor to the "level" frame
	// turn off automatic msg handling so we can detect find source and return appropriately.
	// emmang@xbox - SetHandleMsgs(FALSE) does not appear to be working properly,
	// if FindSource dialog is thrown up, m_pTarget	still handles it (with ESC)
	UIWB.m_pTarget->SetHandleMsgs(FALSE);
	MST.DoKeys("{ENTER}");
	// wait up to 2 seconds to cancel Find Source dlg
	if(WaitForWndWithCtrl(IDSS_CANCEL)  /* = 3000*/)// MST.WFndWndWait(GetLocString(IDSS_FS_TITLE), FW_DEFAULT, 2))
	{
		MST.WButtonClick(GetLocString(IDSS_CANCEL)); //didn't find source
		LOG->RecordInfo("NavigateStack - frame no:%d '%s'. No source file available", level, str);
		UIWB.m_pTarget->SetHandleMsgs(TRUE);
	}
	else
	{ // verification of the line number we ended up on
		UIWB.m_pTarget->SetHandleMsgs(TRUE);
		Frame frame( str );
		if(frame.GetLineNumber( str ))
		{
			int nline = UIWB.GetEditorCurPos(GECP_LINE); /*GetLineNumber()*/
			if(nline == atoi(str) )
				return nline;
			else
			{
				LOG->RecordInfo("ERROR: NavigateStack - frame no:%d '%s'. expected line %s, hit line %d", level, frame, str, nline);
				return 0;
			}
		}
		else
			LOG->RecordInfo("NavigateStack - frame no:%d '%s' - no line number", level, frame);
	}
	return -1;
};


// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::GoToFrame(int level)
// Description: Go to to the specified call stack frame.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: level An integer that contains the 0-based index into the call stack.
// END_HELP_COMMENT
BOOL UIStack::GoToFrame(int level)

	{
	char szkeys[64];
	if(level<0)level=0;
	sprintf(szkeys, "^({HOME}){DOWN %d}", level);
	MST.DoKeyshWnd(HWnd(), szkeys);

	// verification here
	return TRUE;
	};


// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::IsValid(void) const
// Description: Determine if the call stack window is a valid window.
// Return: A Boolean that indicates whether the call stack window is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIStack::IsValid(void) const
	{
	return UIWindow::IsValid();
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::AttachActive(void)
// Description: Attach to the call stack window if it's the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIStack::AttachActive(void)
	{
	HWND hwnd;

    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_STACK_WND_TITLE), FW_FULL, 2))
		return Attach(hwnd);
	else
		return Attach(FindFirstChild(UIWB.HWnd(), IDW_CALLS_WIN));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::Find(void)
// Description: Find the call stack window and attach to it
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIStack::Find(void)
	{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
	return Attach(UIWB.GetToolbar(GetID(),pkgId));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::IsActive(void)
// Description: Determine if the call stack window is active.
// Return: A Boolean value that indicates whether the call stack window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIStack::IsActive(void)
	{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIStack::Activate(void)
// Description: Bring up the call stack window.
// Return: A Boolean value that indicates whether the call stack window is active (TRUE) or not.
// END_HELP_COMMENT
#include "guitarg.h"
BOOL UIStack::Activate(void)
{
	UIWB.DoCommand(IDM_VIEW_STACK, DC_MNEMONIC);
	// we're hitting a timing problem where the callstack window doesn't come up fast enough so we
	// grab text from the wrong window to verify function information (usually the src or asm windows).
	Sleep(100);
	return AttachActive();
}

