///////////////////////////////////////////////////////////////////////////////
//	USTREDIT.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIStrEdit class
//

#include "stdafx.h"
#include "ustredit.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\sym\cmdids.h"
#include "..\sym\shrdres.h"
#include "..\sym\props.h"
#include "..\shl\upropwnd.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIStrEdit::IsValid(void) const
// Description: Determine whether the string editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the title is correct.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIStrEdit::IsValid(void) const
{
	return UIResTableEditor::IsValid() && IsValidType(m_pszTypeText[ED_STRING]);
}

// BEGIN_HELP_COMMENT
// Function: int UIStrEdit::AddString(LPCSTR szCaption, LPCSTR szId /*=NULL*/, BOOL bIdFirst /*=FALSE*/)
// Description: Add a string to the string editor.
// Return: An integer that contains the 1-based line number where the string was added.
// Param: szCaption A pointer to a string that contains the text of the string to add.
// Param: szId A pointer to a string that contains the name of the ID to assign to this string. NULL means use the default ID name. (Default value is NULL.)
// Param: bIdFirst A Boolean value that indicates whether to set the ID first (TRUE) or the text of the string first (FALSE). (Default value is FALSE.)
// END_HELP_COMMENT
int UIStrEdit::AddString(LPCSTR szCaption, LPCSTR szId /*=NULL*/, BOOL bIdFirst /*=FALSE*/)
{
	UIWB.DoCommand(IDM_NEW_STRING, DC_ACCEL);
	UIControlProp upp;
	EXPECT( upp.WaitAttachActive(1000) );
	if( bIdFirst )
	{
		if( szId != NULL )
			MST.WComboSetText(upp.GetLabel(SHRDRES_IDC_ID), szId);	// ID first
		MST.WEditSetText(upp.GetLabel(SHRDRES_IDC_CAPTION), szCaption);
	}
	else
	{
		MST.WEditSetText(upp.GetLabel(SHRDRES_IDC_CAPTION), szCaption);	// caption first
		if( szId != NULL )
			MST.WComboSetText(upp.GetLabel(SHRDRES_IDC_ID), szId);
	}
	int n = GetCurLine();
	UIWB.ShowPropPage(FALSE);	// validate new string
	return n;
}

// BEGIN_HELP_COMMENT
// Function: void UIStrEdit::EditString(LPCSTR szCaption /*=NULL*/, LPCSTR szId /*=NULL*/)
// Description: Edit the selected string in the string editor.
// Return: none
// Param: szCaption A pointer to a string that contains the new text for the selected string. NULL means use the existing text. (Default value is NULL.)
// Param: szId A pointer to a string that contains the name of the new ID for the selected string. NULL means use the existing ID name. (Default value is NULL.)
// END_HELP_COMMENT
void UIStrEdit::EditString(LPCSTR szCaption /*=NULL*/, LPCSTR szId /*=NULL*/)
{
	UIControlProp upp = UIWB.ShowPropPage(TRUE);
	if( szCaption != NULL )
		MST.WEditSetText(upp.GetLabel(SHRDRES_IDC_CAPTION), szCaption);
	if( szId != NULL )
		MST.WComboSetText(upp.GetLabel(SHRDRES_IDC_ID), szId);
	UIWB.ShowPropPage(FALSE);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIStrEdit::FindString(LPCSTR szCaption /*=NULL*/, LPCSTR szId /*=0*/)
// Description: Select a string in the string editor. This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szCaption A pointer to a string that contains the text of the string to select.
// Param: szId A pointer to a string that contains the name of the ID of the string to select.
// END_HELP_COMMENT
BOOL UIStrEdit::FindString(LPCSTR szCaption /*=NULL*/, LPCSTR szId /*=0*/)
{
	ASSERT(FALSE);	// NYI
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIStrEdit::GetString(int line /*=-1*/)
// Description: Get the text of the string at the given line in the string editor.
// Return: A CString containing the text of the string.
// Param: line An integer that contains the 1-based line to retrieve the text for.
// END_HELP_COMMENT
CString UIStrEdit::GetString(int line /*=-1*/)
{
	if( line >= 0 )
		SelectLine(line);	// REVIEW: reset to previous selection?
	return UIWB.GetProperty(P_String);
}
