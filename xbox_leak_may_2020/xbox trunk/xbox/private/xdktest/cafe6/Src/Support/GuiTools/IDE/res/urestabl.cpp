///////////////////////////////////////////////////////////////////////////////
//	URESTABL.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIResTableEditor class
//

#include "stdafx.h"
#include "urestabl.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\props.h"
#define __UNDO_H__			// Prevents #inclusion of some SUSHI headers
#define __SLOBWND_H__
#include "..\sym\slob.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIResTableEditor::IsValid(void) const
// Description: Determine whether the resource table editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the only child window is a list box.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIResTableEditor::IsValid(void) const
{
	if( !UIResEditor::IsValid() )
		return FALSE;

    HWND hwndView = GetWindow(HWnd(), GW_CHILD);   	// view, with listbox and column captions
	if( hwndView == NULL )
		return FALSE;
    HWND hwndList = GetWindow(hwndView, GW_CHILD);	// the listbox is the only child of the view
	if( hwndList == NULL )
			return FALSE;
	return GetDlgControlClass(hwndList) == GL_LISTBOX;
}

void UIResTableEditor::OnUpdate(void)
{
	UIResEditor::OnUpdate();

	if( !IsValid() )
		m_hwndListbox = NULL;
	else
	{
	    HWND hwndView = GetWindow(HWnd(), GW_CHILD);   	// view, with listbox and column captions
	    m_hwndListbox = GetWindow(hwndView, GW_CHILD);	// the listbox is the only child of the view
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIResTableEditor::SelectLine(int line, BOOL bMulti /*=FALSE*/)
// Description: Select a line in the resource table editor.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line An integer that contains the 1-based line to select.
// Param: bMulti A Boolean value that indicates whether to select muliple lines (hold down the control key while selecting the line) (TRUE) or not. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIResTableEditor::SelectLine(int line, BOOL bMulti /*=FALSE*/)
{
	if( !IsActive() && !Activate() )
	{
		LOG->RecordInfo("Could not activate string table");
		return FALSE;
	}
	else
	{
		if( !bMulti )
			MST.WListItemClk(NULL, line+1);	// no caption - only one listbox, and it has the focus
		else
			MST.WListItemCtrlClk(NULL, line+1);	// ctrl-click to mutli-select
		return GetCurLine() == line;
	}
}

// BEGIN_HELP_COMMENT
// Function: CString UIResTableEditor::GetCaption(int line /*=-1*/)
// Description: Get the caption of the resource table editor.
// Return: A CString that contains the caption of the resource table editor.
// Param: line An integer that contains the line to select in the resource editor. Why is this parameter here? -1 means don't select any line. (Default value is -1.)
// END_HELP_COMMENT
CString UIResTableEditor::GetCaption(int line /*=-1*/)
{
	if( line >= 0 )
		SelectLine(line);	// REVIEW: reset to previous selection?
	return UIWB.GetProperty(P_Caption);
}

// BEGIN_HELP_COMMENT
// Function: CString UIResTableEditor::GetId(int line /*=-1*/)
// Description: Get the ID of the resource table editor.
// Return: A CString containing the ID of the resource table editor.
// Param: line An integer that contains the line to select in the resource editor. Why is this parameter here? -1 means don't select any line. (Default value is -1.)
// END_HELP_COMMENT
CString UIResTableEditor::GetId(int line /*=-1*/)
{
	if( line >= 0 )
		SelectLine(line);
	return UIWB.GetProperty(P_ID);
}

// BEGIN_HELP_COMMENT
// Function: int UIResTableEditor::GetCurLine(void)
// Description: Get the current line in the resource table editor.
// Return: An integer that contains the line currently selected in the table editor.
// END_HELP_COMMENT
int UIResTableEditor::GetCurLine(void)
{
//	return MST.WListIndex(NULL);
	return ::SendMessage(m_hwndListbox, LB_GETCURSEL, 0, 0);
}

// BEGIN_HELP_COMMENT
// Function: int UIResTableEditor::GetLineCount(void)
// Description: Get the number of lines in the resource table editor.
// Return: An integer containing the number of lines in the table editor.
// END_HELP_COMMENT
int UIResTableEditor::GetLineCount(void)
{
//	return MST.WListCount(NULL);
	return ::SendMessage(m_hwndListbox, LB_GETCOUNT, 0, 0);
}
