///////////////////////////////////////////////////////////////////////////////
//  UIWATCH.CPP
//
//  Created by :            Date :
//      MichMa              	1/17/94
//
//  Description :
//      Implementation of the UIWatch class
//

#include "stdafx.h"
#include "uiwatch.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIWatch::Activate(void)
// Description: Bring up the watch window.
// Return: A Boolean value that indicates whether the watch window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIWatch::Activate(void)

	{
	UIWB.DoCommand(IDM_VIEW_WATCH, DC_ACCEL);
	HWND hwnd;

    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_WATCH_WND_TITLE), FW_FULL, 2))
		return Attach(hwnd);
	else
		return Attach(FindFirstChild(UIWB.HWnd(), IDW_WATCH_WIN));
	}


// BEGIN_HELP_COMMENT
// Function: int UIWatch::SetName(LPCSTR name, int row /* ROW_CURRENT */)
// Description: Set the name of the expression at the specified row in the watch window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: name A pointer to a string that specifies the name to set in the watch window.
// Param: row An integer that specifies the 1-based row of the watch window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIWatch::SetName(LPCSTR name, int row /* ROW_CURRENT */)
	{
	//REVIEW (michma) this function could have timing problems.
	// needs to have precautions like UIVariables::SetValue.
	SelectRows(row);
	MST.DoKeys("{TAB}");
	MST.WEditSetText("", name);
	MST.DoKeys("{ENTER}");
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int UIWatch::SetValue(LPCSTR value, int row /* ROW_CURRENT */,
// Description: Set the value of the expression at the specified row in the watch window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: value A pointer to a string that contains the value.
// Param: row An integer that specifies the 1-based row of the watch window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// END_HELP_COMMENT
int UIWatch::SetValue(LPCSTR value, int row /* ROW_CURRENT */,
					  int name_editability /* NAME_IS_EDITABLE */)
	
	{
	//REVIEW (michma) this function could have timing problems.
	// needs to have precautions like UIVariables::SetValue.

	SelectRows(row);
	MST.DoKeys("{TAB}");
	
	if(name_editability == NAME_IS_EDITABLE)
		MST.DoKeys("{TAB}");

	MST.WEditSetText("", value);
	MST.DoKeys("{ENTER}");
	return ERROR_SUCCESS;
	}

	
// BEGIN_HELP_COMMENT
// Function: int UIWatch::Delete(int start_row /* ROW_CURRENT */, int total_rows /* 1 */)
// Description: Delete an expression from the watch window.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: start_row An integer that specifies the 1-based row of the watch window. Use ROW_CURRENT to specify the current row; ROW_BOTTOM to specify the last row; and ROW_ALL to specify all rows. (Default value is ROW_CURRENT.)
// Param: total_rows An integer that specifies the number of rows to delete. (Default value is 1.)
// END_HELP_COMMENT
int UIWatch::Delete(int start_row /* ROW_CURRENT */, int total_rows /* 1 */)
	{
	SelectRows(start_row, total_rows);
	MST.DoKeys("{DELETE}");
	return ERROR_SUCCESS;
	}							


int UIWatch::SetPane(int pane)
	{
	return ActivatePane(GetLocString(pane)) ? ERROR_SUCCESS : ERROR_ERROR;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWatch::SelectWatch(LPCSTR szWatch)
// Description: Selects the row in the watch window containing the specified watch.
// Return: An boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: szLocal A pointer to a string that contains the name of the watch.
// END_HELP_COMMENT
BOOL UIWatch::SelectWatch(LPCSTR szWatch)

{
	// start looking from the first row in the watch window.
	SelectRows(1);
	// tracks the last watch selected.
	CString strLastWatch;
	// tracks the currently selected watch.
	CString strCurrentWatch = GetName(ROW_CURRENT, METHOD_PROP_PAGE);

	// loop until we find a name field that matches the watch.
	while(strCurrentWatch != szWatch)

	{
		strLastWatch = strCurrentWatch;
		// select the next row in the watch window.
		SelectRows(ROW_NEXT);
		// get the watch name from the row.
		strCurrentWatch = GetName(ROW_CURRENT, METHOD_PROP_PAGE);
		
		// if it matches the last watch, we've reached the end of the list (or found a sequential duplicate).
		if(strCurrentWatch == strLastWatch)
		{
			LOG->RecordInfo("ERROR in UIWatch::SelectWatch(): "
							"can't find watch \"%s\". reached end of the list.", szWatch);
			return FALSE;
		}
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: int UIWatch::GetWatchCount(void)
// Description: Returns the number of locals in the Locals pane of the variables window.
// Return: An integer specifying the number of locals.
// END_HELP_COMMENT
int UIWatch::GetWatchCount(void)

{
	char szThisFunc[] = "UIWatch::GetWatchCount()";
	
	// start with the first row in the watch window.
	SelectRows(1);
	// tracks the last watch selected.
	CString strLastWatch;
	// tracks the currently selected watch.
	CString strCurrentWatch = GetName(ROW_CURRENT, METHOD_PROP_PAGE);
	// tracks the watch count.
	int intWatchCount = 0;

	// loop until we get to the end of the watch list (when the current watch matches the last watch).
	while(strCurrentWatch != strLastWatch)
	{
		// increment the watch count.
		intWatchCount++;
		// the current watch now becomes the last watch.
		strLastWatch = strCurrentWatch;
		// select the next row in the watch window.
		SelectRows(ROW_NEXT);
		// get the watch name from the current row.
		strCurrentWatch = GetName(ROW_CURRENT, METHOD_PROP_PAGE);
	}

	// we expect the last watch item to be empty.
	if(strCurrentWatch != "")
	{
		LOG->RecordInfo("ERROR in UIWatch::GetWatchCount(): "
						"last watch item was \"%s\". expected empty item.", strCurrentWatch);
		return intWatchCount;
	}
	else
	{
		// the last watch item row counted was empty, so we subtract it here.
		if(intWatchCount > 0)
			return intWatchCount - 1;
		// however if we didn't find any non-empty items, then we didn't count the last item in the first place.
		else
			return 0;
	}
}