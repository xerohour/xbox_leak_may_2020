///////////////////////////////////////////////////////////////////////////////
//	UFINDREP.CPP
//
//	Created by :			Date :
//		DavidGa					4/13/94
//
//	Description :
//		Implementation of the UIFindDlg and UIReplaceDlg classes
//

#include "stdafx.h"
#include "ufindrep.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vcpp32.h"
#include "..\sym\vshell.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//	UIFindDlg class

// base class overrides
BOOL UIFindDlg::VerifyUnique(void) const
{
	return VerifyDlgItemClass(VSHELL_IDC_FIND_WHAT, GL_COMBOBOX);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::Activate(void)
// Description: Open the Find dialog.
// Return: A Boolean that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFindDlg::Activate(void)
{
	UIWB.DoCommand(ID_EDIT_FIND_DLG, DC_MNEMONIC);
	if (WaitAttachActive(5000)) {
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIFindDlg::Close(void)
// Description: Close the Find dialog.
// Return: NULL, if successful; the HWND of the Find dialog, if it failed to close.
// END_HELP_COMMENT
HWND UIFindDlg::Close(void)
{
	return UIDialog::Cancel();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetFindString(LPCSTR szFind)
// Description: Set the text given in szFind in the Find What edit box in the Find dialog.
// Return: A Boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFind A pointer to a string containing the text to enter in the Find What edit box in the Find dialog.
// END_HELP_COMMENT
BOOL UIFindDlg::SetFindString(LPCSTR szFind)
{
	MST.WComboSetText(GetLabel(VSHELL_IDC_FIND_WHAT), szFind);
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetMatchWord(BOOL bMatchWord)
// Description: Check or uncheck the Match Whole Word Only checkbox based on the value of bMatchWord.
// Return: A Boolean that indicates success. TRUE if successful; FALSE otherwise.
// Param: bMatchWord A Boolean value that, if TRUE means check the Match Whole Word Only checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIFindDlg::SetMatchWord(BOOL bMatchWord /*= FALSE*/)
{
	if (bMatchWord) {
		MST.WCheckCheck(GetLabel(VSHELL_IDC_FIND_WHOLEWORD));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VSHELL_IDC_FIND_WHOLEWORD));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetMatchCase(BOOL bMatchCase)
// Description: Check or uncheck the Match Case checkbox based on the value of bMatchCase.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bMatchCase A Boolean value that, if TRUE means check the Match Case checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIFindDlg::SetMatchCase(BOOL bMatchCase /*= FALSE*/)
{
	if (bMatchCase) {
		MST.WCheckCheck(GetLabel(VSHELL_IDC_FIND_MATCHUPLO));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VSHELL_IDC_FIND_MATCHUPLO));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetRegularExpression(BOOL bRegExpr)
// Description: Check or uncheck the Regular Expression checkbox based on the value of bRegExpr.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bRegExpr A Boolean value that, if TRUE means check the Regular Expression checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIFindDlg::SetRegularExpression(BOOL bRegExpr)
{
	if (bRegExpr) {
		MST.WCheckCheck(GetLabel(VSHELL_IDC_FIND_REGEXP));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VSHELL_IDC_FIND_REGEXP));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetDirection(UIFindDlg::FindDirection fd)
// Description: Set the find direction in the Find dialog box.
// Return: A Boolean value that indicates succees. TRUE if successful; FALSE otherwise.
// Param: fd A value that indicates the find direction. FD_UP to set the find direction to up; FD_DOWN to set the find direction to down. (Default value is FD_DOWN.)
// END_HELP_COMMENT
BOOL UIFindDlg::SetDirection(UIFindDlg::FindDirection fd /*= FD_DOWN*/)
{
	if (fd == FD_UP) {
		MST.WOptionClick(GetLabel(VSHELL_IDC_FIND_UP));
	}
	else {
		if (fd == FD_DOWN) {
			MST.WOptionClick(GetLabel(VSHELL_IDC_FIND_DOWN));
		}
		else {
			ASSERT(0);
			// REVIEW(briancr): we need to set an the error type and string here
			return FALSE;
		}
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::FindNext(void)
// Description: Perform the find operation by clicking the Find Next button in the Find dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFindDlg::FindNext(void)
{
	MST.WButtonClick(GetLabel(VSHELL_IDC_FIND_NEXT));
	// REVIEW(briancr): we need to set an error type and string here if the dialog doesn't go away
	return WaitUntilGone(1000);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFindDlg::SetBookmarks(void)
// Description: Perform the set bookmarks operation by clicking on the Set Bookmarks on All button in the Find dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFindDlg::SetBookmarks(void)
{
	MST.WButtonClick(GetLabel(VSHELL_IDC_FIND_MARK_ALL));
	// REVIEW(briancr): we need to set an error type and string here if the dialog doesn't go away
	return WaitUntilGone(1000);
}

///////////////////////////////////////////////////////////////////////////////
//	UIReplaceDlg class

// base class overrides
BOOL UIReplaceDlg::VerifyUnique(void) const
{
	return VerifyDlgItemClass(VCPP32_ID_REPLACE_REPLACEWITH, GL_COMBOBOX);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::Activate(void)
// Description: Open the Replace dialog.
// Return: NULL, if successful; the HWND of the Replace dialog, if it failed to close.
// END_HELP_COMMENT
BOOL UIReplaceDlg::Activate(void)
{
	UIWB.DoCommand(ID_EDIT_REPLACE, DC_MNEMONIC);
	if (WaitAttachActive(5000)) {
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIReplaceDlg::Close(void)
// Description: Close the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
HWND UIReplaceDlg::Close(void)
{
	return UIDialog::Cancel();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetFindString(LPCSTR szFind)
// Description: Enter the text given in szFind in the Find What edit box in the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFind A pointer to a string that contains the text to enter in the Find What edit box in the Replace dialog.
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetFindString(LPCSTR szFind)
{
	MST.WComboSetText(GetLabel(VCPP32_ID_REPLACE_WHAT), szFind);
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetReplaceString(LPCSTR szReplace)
// Description: Enter the text given in szReplace in the Replace With edit box in the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szReplace A pointer to a string that contains the text to enter in the Replace With edit box in the Replace dialog.
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetReplaceString(LPCSTR szReplace)
{
	MST.WComboSetText(GetLabel(VCPP32_ID_REPLACE_REPLACEWITH), szReplace);
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetMatchWord(BOOL bMatchWord)
// Description: Check or uncheck the Match Whole Word Only checkbox based on the value of bMatchWord.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bMatchWord A Boolean value that, if TRUE means check the Match Whole Word Only checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetMatchWord(BOOL bMatchWord /*= FALSE*/)
{
	if (bMatchWord) {
		MST.WCheckCheck(GetLabel(VCPP32_ID_REPLACE_WHOLEWORD));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VCPP32_ID_REPLACE_WHOLEWORD));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetMatchCase(BOOL bMatchCase)
// Description: Check or uncheck the Match Case checkbox based on the value of bMatchCase.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bMatchCase A Boolean value that, if TRUE means check the Match Case checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetMatchCase(BOOL bMatchCase /*= FALSE*/)
{
	if (bMatchCase) {
		MST.WCheckCheck(GetLabel(VCPP32_ID_REPLACE_MATCHUPLO));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VCPP32_ID_REPLACE_MATCHUPLO));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetRegularExpression(BOOL bRegExpr)
// Description: Check or uncheck the Regular Expression checkbox based on the value of bRegExpr.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bRegExpr A Boolean value that, if TRUE means check the Regular Expression checkbox, and if FALSE means uncheck it. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetRegularExpression(BOOL bRegExpr)
{
	if (bRegExpr) {
		MST.WCheckCheck(GetLabel(VCPP32_ID_REPLACE_REGEXP));
	}
	else {
		MST.WCheckUnCheck(GetLabel(VCPP32_ID_REPLACE_REGEXP));
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::SetReplaceScope(UIReplaceDlg::ReplaceScope rs)
// Description: Set the replace scope in the Replace dialog box.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: fd A value that indicates the scope of the find and replace operation: RS_WHOLEFILE to set the replace scope to the whole file; RS_SELECTION to set the replace scope to the selection. (Default value is RS_WHOLEFILE.)
// END_HELP_COMMENT
BOOL UIReplaceDlg::SetReplaceScope(UIReplaceDlg::ReplaceScope rs)
{
	if (rs == UIReplaceDlg::RS_WHOLEFILE) {
		MST.WOptionClick(GetLabel(VCPP32_IDC_REPLACEMODEWHOLEFILE));
	}
	else {
		if (rs == UIReplaceDlg::RS_SELECTION) {
			MST.WOptionClick(GetLabel(VCPP32_IDC_REPLACEMODESELECTION));
		}
		else {
			ASSERT(0);
			// REVIEW(briancr): we need to set the error type and string here
			return FALSE;
		}
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::FindNext(void)
// Description: Perform the find operation by clicking the Find Next button in the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIReplaceDlg::FindNext(void)
{
	if (!MST.WButtonEnabled(GetLabel(VCPP32_ID_REPLACE_FINDNEXT))) {
		// REVIEW(briancr): we need to set the error type and string here
		return FALSE;
	}
	MST.WButtonClick(GetLabel(VCPP32_ID_REPLACE_FINDNEXT));
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::Replace(void)
// Description: Perform the replace operation by clicking the Replace button in the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIReplaceDlg::Replace(void)
{
	if (!MST.WButtonEnabled(GetLabel(VCPP32_ID_REPLACE_REPLACE))) {
		// REVIEW(briancr): we need to set the error type and string here
		return FALSE;
	}
	MST.WButtonClick(GetLabel(VCPP32_ID_REPLACE_REPLACE));
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIReplaceDlg::ReplaceAll(void)
// Description: Perform the replace all operation by clicking the Replace All button in the Replace dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
int UIReplaceDlg::ReplaceAll(void)
{
	MST.WButtonClick(GetLabel(VCPP32_ID_REPLACE_REPLACEALL));
	CString str = UIWB.GetStatusText();		// X occurance(s) have been replaced
	int n;
	sscanf(str, "%d", &n);
	return n;	// return how many were replaced
}
