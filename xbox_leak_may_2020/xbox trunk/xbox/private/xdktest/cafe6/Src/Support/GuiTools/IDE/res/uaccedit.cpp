///////////////////////////////////////////////////////////////////////////////
//	UACCEDIT.CPP
//
//	Created by :			Date :
//		DavidGa					3/25/94
//
//	Description :
//		Implementation of the UIAccEdit class
//

#include "stdafx.h"
#include "uaccedit.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vres.h"
#include "..\sym\shrdres.h"
#include "..\sym\props.h"
#include "..\shl\upropwnd.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIAccEdit::IsValid(void) const
// Description: Determine whether the accelerator editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the title is correct.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIAccEdit::IsValid(void) const
{
	return UIResTableEditor::IsValid() && IsValidType(m_pszTypeText[ED_ACCEL]);
}

// BEGIN_HELP_COMMENT
// Function: int UIAccEdit::AddAccel(LPCSTR szChar, LPCSTR  szId /*=NULL*/, DWORD dwKeyMods /*=0x0*/, BOOL bNextKeyType /*=FALSE*/)
// Description: Add an accelerator to the accelerator editor.
// Return: An integer value that contains the line number in the editor of the new accelerator.
// Param: szChar A pointer to a string that contains the accelerator key to add.
// Param: szId A pointer to a string that contains the name of the ID for this accelerator. NULL indicates use the default name. (The default is NULL.)
// Param: dwKeyMods A bit field that indicates the modifier keys to apply to this accelerator: ACC_CONTROL, ACC_NO_CONTROL, ACC_ALT, ACC_NO_ALT, ACC_SHIFT, ACC_NO_SHIFT, ACC_ASCII, ACC_VIRTKEY. (The default is 0.)
// Param: bNextKey A Boolean value that indicates whether to use the next key typed (TRUE) or not. This parameter is NYI. (The default is FALSE.)
// END_HELP_COMMENT
int UIAccEdit::AddAccel(LPCSTR szChar, LPCSTR  szId /*=NULL*/, DWORD dwKeyMods /*=0x0*/, BOOL bNextKeyType /*=FALSE*/)
{
	UIWB.DoCommand(IDM_NEW_ACCELERATOR, DC_ACCEL);
	
	UIControlProp upp;
	EXPECT( upp.WaitAttachActive(1000) );
	MST.WComboSetText(upp.GetLabel(VRES_IDC_KEY), szChar);
	if( szId != NULL )
		MST.WComboSetText(GetLabel(SHRDRES_IDC_ID), szId);
	if( dwKeyMods & ACC_CONTROL )
		MST.WCheckCheck(GetLabel(VRES_IDC_CTRL));
	else if( dwKeyMods & ACC_NO_CONTROL )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_CTRL));
	if( dwKeyMods & ACC_ALT )
		MST.WCheckCheck(GetLabel(VRES_IDC_ALT));
	else if( dwKeyMods & ACC_NO_ALT )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_ALT));
	if( dwKeyMods & ACC_SHIFT )
		MST.WCheckCheck(GetLabel(VRES_IDC_SHIFT));
	else if( dwKeyMods & ACC_NO_SHIFT )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_SHIFT));
	if( dwKeyMods & ACC_ASCII )
		MST.WOptionClick(GetLabel(VRES_IDC_ASCII));
	else if( dwKeyMods & ACC_VIRTKEY )
		MST.WOptionClick(GetLabel(VRES_IDC_VIRTKEY));

	int n = GetCurLine();
	UIWB.ShowPropPage(FALSE);	// validate new string
	return n;
}

// BEGIN_HELP_COMMENT
// Function: void UIAccEdit::EditAccel(LPCSTR szChar /*=NULL*/, LPCSTR  szId /*=NULL*/, DWORD dwKeyMods /*=0x0*/)
// Description: Edit the currently selected accelerator in the accelerator editor.
// Return: none
// Param: szChar A pointer to a string that contains the new accelerator key for the selected accelerator.
// Param: szId A pointer to a string that contains the new name of the ID for this accelerator. NULL indicates use the current name. (The default is NULL.)
// Param: dwKeyMods A bit field that indicates the modifier keys to apply to this accelerator: ACC_CONTROL, ACC_NO_CONTROL, ACC_ALT, ACC_NO_ALT, ACC_SHIFT, ACC_NO_SHIFT, ACC_ASCII, ACC_VIRTKEY. (The default is 0.)
// END_HELP_COMMENT
void UIAccEdit::EditAccel(LPCSTR szChar /*=NULL*/, LPCSTR  szId /*=NULL*/, DWORD dwKeyMods /*=0x0*/)
{
	UIControlProp upp = UIWB.ShowPropPage(TRUE);
	if( szChar != NULL )
		MST.WComboSetText(upp.GetLabel(VRES_IDC_KEY), szChar);
	if( szId != NULL )
		MST.WComboSetText(GetLabel(SHRDRES_IDC_ID), szId);
	if( dwKeyMods & ACC_CONTROL )
		MST.WCheckCheck(GetLabel(VRES_IDC_CTRL));
	else if( dwKeyMods & ACC_NO_CONTROL )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_CTRL));
	if( dwKeyMods & ACC_ALT )
		MST.WCheckCheck(GetLabel(VRES_IDC_ALT));
	else if( dwKeyMods & ACC_NO_ALT )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_ALT));
	if( dwKeyMods & ACC_SHIFT )
		MST.WCheckCheck(GetLabel(VRES_IDC_SHIFT));
	else if( dwKeyMods & ACC_NO_SHIFT )
		MST.WCheckUnCheck(GetLabel(VRES_IDC_SHIFT));
	if( dwKeyMods & ACC_ASCII )
		MST.WOptionClick(GetLabel(VRES_IDC_ASCII));
	else if( dwKeyMods & ACC_VIRTKEY )
		MST.WOptionClick(GetLabel(VRES_IDC_VIRTKEY));
	UIWB.ShowPropPage(FALSE);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIAccEdit::FindAccel(LPCSTR sz /*=NULL*/, LPCSTR szId /*=0*/)
// Description: Determine if the given accelerator is defined. This function is NYI.
// Return: A Boolean value that indicates success; TRUE if successful; FALSE otherwise.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
BOOL UIAccEdit::FindAccel(LPCSTR sz /*=NULL*/, LPCSTR szId /*=0*/)
{
	ASSERT(FALSE);	// NYI
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIAccEdit::GetKey(int line /*=-1*/)
// Description: Get the accelerator key associated with the accelerator on the specified line (line).
// Return: A CString containing the key associated with the accelerator on the specified line.
// Param: line An integer containing the line to get the accelerator key for. -1 means use the current line. (The default is -1.)
// END_HELP_COMMENT
CString UIAccEdit::GetKey(int line /*=-1*/)
{
	if( line >= 0 )
		SelectLine(line);	// REVIEW: reset to previous selection?
	return UIWB.GetProperty(P_Key);
}

// BEGIN_HELP_COMMENT
// Function: DWORD UIAccEdit::GetKeyMods(int line /*=-1*/)
// Description: Get the accelerator key modifiers for the accelerator on the specified line (line).
// Return: A bit field that indicates the modifier keys to apply to this accelerator: ACC_CONTROL, ACC_NO_CONTROL, ACC_ALT, ACC_NO_ALT, ACC_SHIFT, ACC_NO_SHIFT, ACC_ASCII, ACC_VIRTKEY.
// Param: line An integer containing the line to get the accelerator modifier keys for. -1 means use the current line. (The default is -1.)
// END_HELP_COMMENT
DWORD UIAccEdit::GetKeyMods(int line /*=-1*/)
{
	if( line >= 0 )
		SelectLine(line);	// REVIEW: reset to previous selection?

	DWORD dw;
	CString str;
	str = UIWB.GetProperty(P_Ctrl);
	if( str == "1" )
		dw |= ACC_CONTROL;
	str = UIWB.GetProperty(P_Alt);
	if( str == "1" )
		dw |= ACC_ALT;
	str = UIWB.GetProperty(P_Shift);
	if( str == "1" )
		dw |= ACC_SHIFT;
	str = UIWB.GetProperty(P_Type);
	if( str == "1" )
		dw |= ACC_ASCII;

	return dw;
}
