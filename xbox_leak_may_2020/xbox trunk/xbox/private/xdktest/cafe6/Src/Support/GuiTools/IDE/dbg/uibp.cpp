///////////////////////////////////////////////////////////////////////////////
//  UIBP.CPP
//
//  Created by :            Date :
//      MichMa              01/14/94
//
//  Description :
//      Implementation of the UIBreakpoints class
//

#include "stdafx.h"
#include "uibp.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "guiv1.h"
#include "..\shl\uitabdlg.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::Activate(void)
// Description: Open the Breakpoints dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBreakpoints::Activate(void)
{
	UIWB.DoCommand(IDM_RUN_SETBREAK, DC_MNEMONIC);  
//	UIWB.DoCommand(IDM_RUN_SETBREAK, DC_ACCEL);    // Temporary for new BP dialog

	Attach(MST.WFndWndWait(GetLocString(IDSS_BP_TITLE), FW_DEFAULT, 10));   // give it 10 seconds to come up.
	return (this != NULL);
}

///BOOL UIBreakpoints::SetType(int type)
///	{
///	if(!(IsActive() && MST.WComboEnabled(GetLabel(VCPP32_ID_SETBREAK_ACTION)))) return FALSE;
///	MST.WComboItemClk(GetLabel(VCPP32_ID_SETBREAK_ACTION), type);
///	return TRUE;
///	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetLocation(LPCSTR location)
// Description: Set the location for the breakpoint in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: location A pointer to a string that specifies the location for the breakpoint. The location string is in the form: {function, source filename, EXE filename} .line.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetLocation(LPCSTR location)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	if( uitd.SetOption(UIBP_LOCATION_EDIT, UIBP_LOCATION_EDIT, location) != ERROR_SUCCESS ) return FALSE;

//	if( !MST.WEditEnabled(GetLabel(UIBP_LOCATION_EDIT, uitd.GetCurrentPage())) ) return FALSE;
//	MST.WEditSetText(GetLabel(UIBP_LOCATION_EDIT, uitd.GetCurrentPage()), location);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetLocationExpression(LPCSTR expression, int expr_type, BOOL fForceType /* TRUE */)
// Description: Set the expression to break on for a location breakpoint in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to break on. The expression string is in the form: {function, source filename, EXE filename} expression.
// Param: expr_type A value that specifies the type of expression. This parameter is NYI.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetLocationExpression(LPCSTR expression, int expr_type)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_LOCATION_EDIT,8);

	uibpcond.Activate();
	if(!uibpcond.SetExpression(expression, expr_type)) return FALSE;

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetDataExpression(LPCSTR expression, int expr_type)
// Description: Set the expression to break on for a data breakpoint in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to break on. The expression string is in the form: {function, source filename, EXE filename} expression.
// Param: expr_type A value that specifies the type of expression. This parameter is NYI.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetDataExpression(LPCSTR expression, int expr_type)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_DATA_EXPRESSION,8);
	// REVIEW (michma): the control-id is correct but GetLabel
	// claims that no label definition precedes the control definition.
	//if(!MST.WEditEnabled(GetLabel(UIBP_DATA_EXPRESSION)) ) return FALSE;
	if(!MST.WEditEnabled("@1")) return FALSE;
	MST.WEditSetText("@1", expression);

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetExprLength(LPCSTR length)
// Description: Set the size of the data to watch for an expression in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: length A pointer to a string that contains the size of the data to watch.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetExprLength(LPCSTR length)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_NUMBER_OF_ELEMENTS,8);
	MST.WEditSetText("@2", length);

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetLocationExprLength(LPCSTR length)
// Description: Set the size of the data to watch for an expression in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: length A pointer to a string that contains the size of the data to watch.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetLocationExprLength(LPCSTR length)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_LOCATION_EDIT,8);
	uibpcond.Activate();
	if(!uibpcond.SetLength(length)) return FALSE;

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetPassCount(LPCSTR passcount)
// Description: Set the pass count for a breakpoint in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: passcount A pointer to a string that contains the pass count.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetPassCount(LPCSTR passcount)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_LOCATION_EDIT,8);
	uibpcond.Activate();
	if(!uibpcond.SetPassCount(passcount)) return FALSE;

	return TRUE;
	/* Passcounts removed from Version 2.0, will be in version 3.0
	if(!(IsActive() && MST.WEditEnabled(GetLabel(VCPP32_ID_SETBREAK_PASSCNT)))) return FALSE;
	MST.WEditSetText(GetLabel(VCPP32_ID_SETBREAK_PASSCNT), passcount);
	return TRUE;
	*/
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetWndProc(LPCSTR location)
// Description: Set the window procedure for a message breakpoint in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: location A pointer to a string that contains the window procedure.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetWndProc(LPCSTR location)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_WINDPROC_COMBO,8);
	MST.WComboSetText("@1", location);
	return TRUE;
	}
	
	
// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetMessage(LPCSTR message, int type)
// Description: Set the message to break when received in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: message A pointer to a string that contains the name of the message.
// Param: type A value that specifies the type of message. This parameter is NYI.
// END_HELP_COMMENT
BOOL UIBreakpoints::SetMessage(LPCSTR message, int type)
	{
	if(!(IsActive()) ) return FALSE;
	UITabbedDialog uitd;
	uitd.AttachActive();
	uitd.ShowPage(UIBP_MESSAGE_COMBO,8);
	MST.WComboSetText("@2", message);
	return TRUE;
	}	


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::SetCurrentBreakpoint(int bp /* 0 */)
// Description: Select the breakpoint specified in the list of breakpoints in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bp An integer that specifies the 0-based index into the list of breakpoints. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIBreakpoints::SetCurrentBreakpoint(int bp /* 0 */)
	{
	if(!(IsActive() && MST.WListEnabled(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
	MST.WListItemClk(GetLabel(UIBP_BREAKPOINTS_LIST), bp + 1);
	return TRUE;
	}


///BOOL UIBreakpoints::Add()
///	{
///	if(!(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_ID_SETBREAK_ADD)))) return FALSE;
///	int count = GetCount();
///	MST.WButtonClick(GetLabel(VCPP32_ID_SETBREAK_ADD));
///	return GetCount() == (count + 1);
///	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::Remove(int bp /* 0 */)	
// Description: Remove the specified breakpoint from the list of breakpoints in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bp An integer that specifies the 0-based index into the list of breakpoints for the breakpoint to remove. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIBreakpoints::Remove(int bp /* 0 */)	
	{
	if (!SetCurrentBreakpoint(bp))
		return FALSE;
	if(!(IsActive() && MST.WButtonEnabled(GetLabel(UIBP_REMOVE_BTN)))) return FALSE;
	int count = GetCount();
	MST.WButtonClick(GetLabel(UIBP_REMOVE_BTN));
	return GetCount() == (count - 1);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::Enable(int bp /* 0 */)
// Description: Enable the specified breakpoint in the list of breakpoints in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bp An integer that specifies the 0-based index into the list of breakpoints for the breakpoint to remove. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIBreakpoints::Enable(int bp /* 0 */)
	{
	if (!SetCurrentBreakpoint(bp))
		return FALSE;
	if( !IsActive() ) return FALSE;

	CListBox clist;
	if( !clist.Attach( MST.WGetFocus() ) ) return FALSE;
	DWORD checkbox = clist.GetItemData(bp);
   	if( !clist.Detach() ) return FALSE;
	if( checkbox == 0 )
		MST.DoKeys(" ");

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::Disable(int bp /* 0 */)
// Description: Disable the specified breakpoint in the list of breakpoints in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bp An integer that specifies the 0-based index into the list of breakpoints for the breakpoint to remove. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIBreakpoints::Disable(int bp /* 0 */)
	{
	if (!SetCurrentBreakpoint(bp))
		return FALSE;

	CListBox clist;
	DWORD checkbox;
	if( !clist.Attach( MST.WGetFocus() ) ) return FALSE;
	checkbox = clist.GetItemData(bp);
	if( !clist.Detach() ) return FALSE;
	if( checkbox != 0 )
		MST.DoKeys(" ");

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::ClearAll()
// Description: Remove all breakpoints from the list of breakpoints in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBreakpoints::ClearAll()
{
	if(!(IsActive() && MST.WListEnabled(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
//	MST.DoKeys("^{Home}^+{End}");
//	MST.DoKeys("^{End}");
//	MST.DoKeys("^+{Home}");
//	if( !MST.WButtonEnabled(GetLabel(UIBP_REMOVE_BTN)) ) return FALSE;
//	MST.WButtonClick(GetLabel(UIBP_REMOVE_BTN));

	// Review: YS: When multiple selection is avalable we'll do SelectAll() & Remove()
	int count = GetCount();
	if( count > 1 ) 	  // New BP dialog: the "new breakpoint" line gets count bigger by 1
		for( int i = 0; i < count - 1; i++ )
		{
			if( !(SetCurrentBreakpoint() && MST.WButtonEnabled(GetLabel(UIBP_REMOVE_BTN))) ) return FALSE;
			MST.WButtonClick(GetLabel(UIBP_REMOVE_BTN));
		}
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::BreakpointIsSet(LPCSTR expr, LPCSTR source /* NULL */)
// Description: Determine if the breakpoint specified is set in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates whether the breakpoint is set (TRUE) or not.
// Param: expr A pointer to a string that contains all or part of the expected expression for the breakpoint.
// Param: source A pointer to a string that contains all or part of the expected source filename for the breakpoint. NULL means use expression only. (Default value is NULL.)
// END_HELP_COMMENT
BOOL UIBreakpoints::BreakpointIsSet(LPCSTR expr, LPCSTR source /* NULL */)
	
{
	if(!(IsActive() && MST.WListEnabled(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
	int count = GetCount();
	CString cstr;

	for (int i = 0; i < count - 1; i++)
	{
		cstr = GetBreakpointStr(i);

   		if((cstr.Find(expr) != -1) && ((!source) || (cstr.Find(source) != -1)))
			break;
	}
	
	return (i < count - 1);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::BreakpointIsEnabled(LPCSTR expr, LPCSTR source /* NULL */)
// Description: Determine if the specified breakpoint is enabled in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A Boolean value that indicates whether the breakpoint is enabled (TRUE) or not.
// Param: expr A pointer to a string that contains all or part of the expected expression for the breakpoint.
// Param: source A pointer to a string that contains all or part of the expected source filename for the breakpoint. NULL means use expression only. (Default value is NULL.)
// END_HELP_COMMENT
BOOL UIBreakpoints::BreakpointIsEnabled(LPCSTR expr, LPCSTR source /* NULL */)
{
	if(!(IsActive() && MST.WListEnabled(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
	int count = GetCount();
	CString cstr;

	CListBox clist;
	if( !clist.Attach( MST.WGetFocus() ) ) return FALSE;

	DWORD checkbox; 
	for (int i = 0; i < count - 1; i++)
 	{
		cstr = GetBreakpointStr(i);
		checkbox = clist.GetItemData(i);
   		if( (cstr.Find(expr) != -1) && ((!source) || (cstr.Find(source) != -1)) && (checkbox != 0) )
			break;
	}

	if( !clist.Detach() ) return FALSE;
	return (i < count - 1);
}


// BEGIN_HELP_COMMENT
// Function: int UIBreakpoints::GetCount(void)
// Description: Get the number of breakpoints set in the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: An integer that specifies the number of breakpoints set.
// END_HELP_COMMENT
int UIBreakpoints::GetCount(void)
	{
	if(!(IsActive() && MST.WListExists(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
	return MST.WListCount(GetLabel(UIBP_BREAKPOINTS_LIST));
	}


// BEGIN_HELP_COMMENT
// Function: CString UIBreakpoints::GetBreakpointStr(int bp /* 0 */)
// Description: Get a string that describes the specified breakpoint from the Breakpoints dialog. The Breakpoints dialog must be active when this function is called.
// Return: A CString that contains a string describing the specified breakpoint.
// Param: bp An integer that specifies the 0-based index into the list of breakpoints. (Default value is 0.)
// END_HELP_COMMENT
CString UIBreakpoints::GetBreakpointStr(int bp /* 0 */)
	
	{
//	if(!(IsActive() && MST.WListEnabled(GetLabel(UIBP_BREAKPOINTS_LIST)))) return FALSE;
///	//if(!(IsActive() && MST.WListExists(GetLabel(VCPP32_ID_SETBREAK_BREAKPOINT)))) return FALSE;
///					//REVIEW: why does above line cause CString::CString ambigious overloaded function call?
	CString cstr;

	MST.WListItemText(GetLabel(UIBP_BREAKPOINTS_LIST), bp+1, cstr);

	return cstr;
	}

// BEGIN_HELP_COMMENT
// Function: HWND UIBreakpoints::Close(void)
// Description: Close the Breakpoints dialog.
// Return: NULL if successful; the HWND of any remaining dialog or message box otherwise.
// END_HELP_COMMENT
HWND UIBreakpoints::Close(void)
{
//	ExpectValid();
 	
	if( GetLabel(UIBP_OK_BTN,NULL,TRUE) )    // Suppress warning if no button
		MST.WButtonClick(GetLabel(UIBP_OK_BTN));		// use the close button if there is one
	else
	{
	    LOG->RecordInfo( "Did not find Breakpoints Close button" );
		PostMessage(HWnd(), WM_CLOSE, 0, 0);	// otherwise, close it however we must
	}
	if( WaitUntilGone(3000) )		// REVIEW(davidga) 3000 is too arbitrary
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}


//      Implementation of the UIBreakpointCondition class
//

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpointCondition::Activate(void)
// Description: Open the Breakpoint Condition dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBreakpointCondition::Activate(void)
{
	MST.WButtonClick(GetLabel(25452)); // Condition button.
	Attach(MST.WFndWndWait(GetLocString(IDSS_BP_CONDITION_TITLE), FW_ALL|FW_FOCUS|FW_NOCASE|FW_FULL, 10));   // give it 10 seconds to come up.
	return (this != NULL);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpointCondition::SetExpression(LPCSTR expression, int expr_type)
// Description: Set the expression in the Breakpoint Condition dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: expr_type A value that indicates the type of expression. This parameter is NYI.
// END_HELP_COMMENT
BOOL UIBreakpointCondition::SetExpression(LPCSTR expression, int expr_type)
{
	if( !(IsActive() && MST.WEditEnabled(GetLabel(UIBP_LOCATION_EXPRESSION))) ) return FALSE;
	MST.WEditSetText("@1", expression);
	MST.WButtonClick(GetLabel(UIBP_OK_BTN));
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpointCondition::SetPassCount(LPCSTR passcount)
// Description: Set the pass count in the Breakpoint Condition dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: passcount A pointer to a string that contains the pass count.
// END_HELP_COMMENT
BOOL UIBreakpointCondition::SetPassCount(LPCSTR passcount)
{
	if( !(IsActive() && MST.WEditEnabled(GetLabel(UIBP_SKIP_COUNT))) ) return FALSE;
	MST.WEditSetText("@3", passcount);
//	if( !SetWindowText(GetDlgItem(UIBP_SKIP_COUNT), passcount) ) return FALSE;
	MST.WButtonClick(GetLabel(UIBP_OK_BTN));
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpointCondition::SetLength(LPCSTR passcount)
// Description: Set the Number of elements in the Breakpoint Condition dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: length - A pointer to a string that contains the length.
// END_HELP_COMMENT
BOOL UIBreakpointCondition::SetLength(LPCSTR length)
{
	if( !(IsActive() && MST.WEditEnabled(GetLabel(UIBP_LOCATION_EXPRESSION))) ) return FALSE;
//	if( !(IsActive() && MST.WEditEnabled(GetLabel(UIBP_NUMBER_OF_ELEMENTS))) ) return FALSE;
	MST.WEditSetText("@2", length);
	MST.WButtonClick(GetLabel(UIBP_OK_BTN));
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBreakpoints::EditCode(void)
// Description: Selects the Edit Code button in the breakpoints dlg.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBreakpoints::EditCode(void)

{
	// make sure the bp dlg is active.
	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in UIBreakpoints::EditCode() - bp dlg not active");
		return FALSE;
	}
	
	// make sure the Edit Code button is enabled.
	if(!MST.WButtonEnabled(GetLocString(IDSS_BP_EDIT_CODE_BUTTON)))
	{
		LOG->RecordInfo("ERROR in UIBreakpoints::EditCode() - \"%s\" button not enabled", 
						GetLocString(IDSS_BP_EDIT_CODE_BUTTON));
		return FALSE;
	}
	
	// hit the Edit Code button.
	MST.WButtonClick(GetLocString(IDSS_BP_EDIT_CODE_BUTTON));
	return TRUE;
}

