///////////////////////////////////////////////////////////////////////////////
//  UIVAR.CPP
//
//  Created by :            Date :
//      MichMa              	10/20/94
//
//  Description :
//      Implementation of the UIVariables class
//

#include "stdafx.h"
#include "uivar.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIVariables::Activate(void)
// Description: Bring up the variables window.
// Return: A Boolean value that indicates whether the variables window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIVariables::Activate(void)

	{
	UIWB.DoCommand(IDM_VIEW_VARS, DC_MNEMONIC);
//REVIEW (chriskoz) do not understand the purpose of this idiot sequence
//	commented out the offending code & replaced with AttachActive() which makes sense
//	HWND hwnd;
//    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_VAR_WND_TITLE), FW_FULL, 2))
//		return Attach(hwnd);
//	else
//		return Attach(FindFirstChild(UIWB.HWnd(), IDW_LOCALS_WIN));
	return AttachActive();
	}


// BEGIN_HELP_COMMENT
// Function: int UIVariables::SetValue(LPCSTR value, int row /* ROW_CURRENT */)
// Description: Set the value for the expression at the row specified in the variables window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: value A pointer to a string that contains the value.
// Param: row An integer that specifies the 1-based row of the variables window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIVariables::SetValue(LPCSTR value, int row /* ROW_CURRENT */)
	
	{
	SelectRows(row);
	MST.DoKeys("{TAB}");

	int i = 0;
 
	while(!MST.WEditExists("@1") && (++i < 10))
		Sleep(500);

	if(i == 10)

		{
		LOG->RecordInfo("ERROR in UIVariables::SetValue(): "
						"edit field did not appear within 5 seconds");

		return ERROR_ERROR;
		}

	MST.WEditSetText("@1", value);
	MST.DoKeys("{ENTER}");
	while(MST.WEditExists("@1"));
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIVariables::SetPane(int pane)
// Description: Set the current pane to the specified pane in the variables window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: pane A value that specifies the pane to activate: PANE_THIS, PANE_AUTO, PANE_LOCALS.
// END_HELP_COMMENT
int UIVariables::SetPane(int pane)

	{
	// REVIEW (michma): UIDockWindow::ActivatePane() is broken.
	// REVIEW (chriskoz) it's not broken but difficult to get(use ::WCustomExists())
	//	return ActivatePane(GetLocString(pane)) ? ERROR_SUCCESS : ERROR_ERROR;

	char pane_str[128];
	strncpy(pane_str, GetLocString(pane), 128);
	

	// cycle through panes until we find the pane
	for(int i = 0; i < TOTAL_VAR_PANES; i++)
		{
//		if(GetName(1) == this_str)
		Sleep(500);
//		::WCustomSetClass("Afx:400000:0"); //obsolete
//		if(::WCustomExists(pane_str,0)!=0) //obsolete
//		if(::WFndWndC(pane_str,"Afx:400000:0",FW_ACTIVE,0)!=NULL)
		if(::WFndWndC(pane_str,NULL,FW_ACTIVE,0)!=NULL) //we don't want to rely on th above class name
			break;
		MST.DoKeys("^({PGDN})");
		}
	
/*		if(MST.FindWndWait(pane_str,1)
			break;
		else
			MST.DoKeys("^({PGDN})");
*/ //there is no way to use FindWndWait
	if(i == TOTAL_VAR_PANES)
		{
		LOG->RecordInfo("ERROR in UIVariables::SetPane(): "
						"can't find %s pane", pane_str);

		return ERROR_ERROR;
		}	
	return ERROR_SUCCESS;
	}

// BEGIN_HELP_COMMENT
// Function: CString UIVariables::GetFunction(int level /* 0 */)
// Description: Get the call stack description string at the given call stack level (index).  It assumes that the Variables window already has focus.
// Return: A CString that contains the call stack description string from the given call stack level.
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// END_HELP_COMMENT
CString UIVariables::GetFunction(int level /* 0 */)
{
	// First we must give the combo box focus.  It may not be that important here, but for
	// certain situations it seems necessary.  See NavigateStack for more details.

//	MST.DoKeys("%+O{Escape}");
	MST.WComboSetFocus(GetLocString(IDSS_VAR_WND_CALLS));

	CString str;
	MST.WComboItemText(GetLocString(IDSS_VAR_WND_CALLS), level + 1, str);
	return str;
}

// BEGIN_HELP_COMMENT
// Function: CString UIVariables::GetAllFunctions(void)
// Description: Get all the call stack frame descriptions for all frames on the call stack.
// Return: A CString that contains a newline-delimited list of call stack frames.
// END_HELP_COMMENT
CString UIVariables::GetAllFunctions(void)
{
	// First we must give the combo box focus.  It may not be that important here, but for
	// certain situations it seems necessary.  See NavigateStack for more details.

//	MST.DoKeys("%+O{Escape}");
	MST.WComboSetFocus(GetLocString(IDSS_VAR_WND_CALLS));

	int nItems = MST.WComboCount(GetLocString(IDSS_VAR_WND_CALLS));

	CString str, temp;
	for (int i = 0; i < nItems; i++)
	{
		MST.WComboItemText(GetLocString(IDSS_VAR_WND_CALLS), i + 1, temp);
		str += temp + '\n';
	}
	return str;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIVariables::NavigateStack(int level /* 0 */)
// Description: Navigate to the specified call stack frame.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// END_HELP_COMMENT
BOOL UIVariables::NavigateStack(int level /* 0 */)
{
	// First we must give the combo box focus.  This seems to be necessary due to an MSTest bug.
	// The reason I think that is because, if the combo box is not given focus, after 
	// displaying the Variables window, then the WComboExists("C&ontext") call is successful,
	// while the WComboItemClk("C&ontext", n) call is not.  This really doesn't make any 
	// sense.  If the combo is given focus first then both calls are successful. (JPG).

//	MST.DoKeys("%+O{Escape}");
	MST.WComboSetFocus(GetLocString(IDSS_VAR_WND_CALLS));

	CString str = GetLocString(IDSS_VAR_WND_CALLS);
	if (MST.WComboExists(str))
		MST.WComboItemClk(str, level + 1);
	else
		return FALSE;
//REVIEW(chriskoz): FInd Source handled by CTarget
	// verification here
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIVariables::SelectLocal(LPCSTR szLocal)
// Description: Selects the row in the locals pane containing the specified local.
// Return: An boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: szLocal A pointer to a string that contains the name of the local.
// END_HELP_COMMENT
BOOL UIVariables::SelectLocal(LPCSTR szLocal)

{
	// select the locals pane in the variables window.
	if(SetPane(PANE_LOCALS) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in UIVariables::SelectLocal() - can't select locals pane.");
		return FALSE;
	}

	// start looking from the first row in the locals pane.
	SelectRows(1);
	// tracks the last local selected.
	CString strLastLocal;
	// tracks the currently selected locally.
	CString strCurrentLocal = GetName();

	// loop until we find a name field that matches the local.
	while(strCurrentLocal != szLocal)

	{
		strLastLocal = strCurrentLocal;
		// select the next row in the locals pane.
		SelectRows(ROW_NEXT);
		// get the local name from the row.
		strCurrentLocal = GetName();
		
		// if it matches the last local, we've reached the end of the list (or found a sequential duplicate).
		if(strCurrentLocal == strLastLocal)
		{
			LOG->RecordInfo("ERROR in UIVariables::SelectLocal(): "
							"can't find local \"%s\". reached end of the list.", szLocal);
			return FALSE;
		}
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: int UIVariables::GetLocalsCount(void)
// Description: Returns the number of locals in the Locals pane of the variables window.
// Return: An integer specifying the number of locals.
// END_HELP_COMMENT
int UIVariables::GetLocalsCount(void)

{
	char szThisFunc[] = "UIVariables::GetLocalsCount()";
	
	// select the locals pane in the variables window.
	if(SetPane(PANE_LOCALS) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s - can't select locals pane.", szThisFunc);
		return FALSE;
	}

	// start with the first row in the locals pane.
	SelectRows(1);
	// tracks the last local selected.
	CString strLastLocal;
	// tracks the currently selected locally.
	CString strCurrentLocal = GetName();
	// tracks the locals count.
	int intLocalsCount = 0;

	// loop until we get to the end of the locals list (when the current local matches the last local).
	while(strCurrentLocal != strLastLocal)
	{
		// increment the locals count.
		intLocalsCount++;
		// the current local now becomes the last local.
		strLastLocal = strCurrentLocal;
		// select the next row in the locals pane.
		SelectRows(ROW_NEXT);
		// get the local name from the current row.
		strCurrentLocal = GetName();
	}

	return intLocalsCount;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIVariables::SelectAuto(LPCSTR szName)
// Description: Selects the row in the auto pane containing the specified auto.
// Return: An boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: szName A pointer to a string that contains the name of the auto.
// END_HELP_COMMENT
BOOL UIVariables::SelectAuto(LPCSTR szName)

{
	// select the auto pane in the variables window.
	if(SetPane(PANE_AUTO) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in UIVariables::SelectAuto() - can't select auto pane.");
		return FALSE;
	}

	return SelectRow(szName);
}

// BEGIN_HELP_COMMENT
// Function: int UIVariables::GetAutoCount(void)
// Description: Returns the number of items in the Auto pane of the variables window.
// Return: An integer specifying the number of items in the Auto pane.
// END_HELP_COMMENT
int UIVariables::GetAutoCount(void)

{
	const char* const THIS_FUNCTION = "UIVariables::GetAutoCount";
	
	// select the locals pane in the variables window.
	if(SetPane(PANE_AUTO) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s - can't select locals pane.", THIS_FUNCTION);
		return FALSE;
	}

	return GetRowCount();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIVariables::SelectRow(LPCSTR szName)
// Description: Selects the row in the current pane containing the specified name field.
// Return: An boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: szName A pointer to a string that contains the name in the row to select.
// END_HELP_COMMENT
BOOL UIVariables::SelectRow(LPCSTR szName)

{
	const char* const THIS_FUNCTION = "UIVariables::SelectRow";

	// start looking from the first row in the pane.
	SelectRows(1);
	// tracks the name in the last row selected.
	CString strLastName;
	// tracks the currently selected name.
	CString strCurrentName = GetName();

	// loop until we find a name field that matches the one specified.
	while(strCurrentName != szName)

	{
		strLastName = strCurrentName;
		// select the next row in the pane.
		SelectRows(ROW_NEXT);
		// get the name from the row.
		strCurrentName = GetName();
		
		// if it matches the last name, we've reached the end of the list (or found a sequential duplicate).
		if(strCurrentName == strLastName)
		{
			LOG->RecordInfo("ERROR in %s: can't find name \"%s\". reached end of the list.", THIS_FUNCTION, szName);
			return FALSE;
		}
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: int UIVariables::GetRowCount(void)
// Description: Returns the number of rows in the current pane of the variables window.
// Return: An integer specifying the number of rows.
// END_HELP_COMMENT
int UIVariables::GetRowCount(void)

{
	// start with the first row in the pane.
	SelectRows(1);
	// tracks the last local selected.
	CString strLastName;
	// tracks the currently selected row.
	CString strCurrentName = GetName();
	// tracks the row count.
	int intRowCount = 0;

	// loop until we get to the end of the list (when the current name matches the last name).
	while(strCurrentName != strLastName)
	{
		// increment the row count.
		intRowCount++;
		// the current now becomes the last row.
		strLastName = strCurrentName;
		// select the next row in the pane.
		SelectRows(ROW_NEXT);
		// get the name from the current row.
		strCurrentName = GetName();
	}

	return intRowCount;
}
