///////////////////////////////////////////////////////////////////////////////
//	UCOMMDLG.CPP
//
//	Created by :			Date :
//		DavidGa					10/21/93
//
//	Description :
//		Implementation of the Common Dialog Utility classes
//

#include "stdafx.h"
#include "ucommdlg.h"
#include "testxcpt.h"
#include "..\..\testutil.h"
#include "mstwrap.h"
#include "uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//	UIFileDlg class

// BEGIN_HELP_COMMENT
// Function: void UIFileDlg::SetName(LPCSTR sz)
// Description: Set the filename field in a file common dialog box.
// Return: none
// Param: sz A pointer to a string containing the filename to set.
// END_HELP_COMMENT
void UIFileDlg::SetName(LPCSTR sz)
{
	EXPECT( IsValid() );
	MST.WEditSetText(GetLabel(IDC_FILE_FILENAME), sz);
}

// BEGIN_HELP_COMMENT
// Function: CString UIFileDlg::GetName()
// Return: The filename specified in the filename field in a file common dialog box.
// END_HELP_COMMENT
CString UIFileDlg::GetName()
{
	CString strName;

	EXPECT( IsValid() );
	MST.WEditText(GetLabel(IDC_FILE_FILENAME), strName);

	return strName;
}

// BEGIN_HELP_COMMENT
// Function: void UIFileDlg::SetPath(LPCSTR sz)
// Description: Set the path in a file common dialog box.
// Return: none
// Param: sz A pointer to a string containing the path to set.
// END_HELP_COMMENT
void UIFileDlg::SetPath(LPCSTR sz)
{
	EXPECT( IsValid() );
	MST.WEditSetText(GetLabel(IDC_FILE_FILENAME), sz);
	MST.WButtonClick(GetLabel(IDOK));	// REVIEW: OK() will handle possible messages
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIFileDlg::OK(BOOL bOverwrite)
// Description: Click the OK button a file common dialog box.
// Return: TRUE if the button is clicked and the dialog is successfully dismissed; FALSE otherwise.
// Param: bOverwrite TRUE to overwrite the file if it already exists; FALSE to not overwrite the file.
// END_HELP_COMMENT
BOOL UIFileDlg::OK(BOOL bOverwrite)
{
	// TODO(davidga): handle possible error messages
	return UIDialog::OK() == NULL;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIFileDlg::GetDlgItem(UINT id) const
// Description: Get the HWND of a particular item (referred to by ID) in the dialog.
// Return: The HWND of the dialog item specified by the id parameter.
// Param: id The id of the item in the dialog.
// END_HELP_COMMENT
HWND UIFileDlg::GetDlgItem(UINT id) const
// REVIEW(briancr): this function should move to the base class after all the common
// dialogs have been converted to use the new Win95 extension to common dialogs.
{
	HWND hItem;
	HWND hDlgExtension;
	// look for the dialog item as a child first
	hItem = ::GetDlgItem(HWnd(), id);

	// if the item's not a direct child, it may be part of the common dialog extension
	// extensions to common dialogs are implemented as child dialogs of the common dialog
	if (!hItem) {
		hDlgExtension = GetExtensionDialog();
		hItem = ::GetDlgItem(hDlgExtension, id);
	}

	return hItem;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIOpenDlg::GetLabel(UINT id) const
// Description: Get the label associated with a control on the dialog (specified by id).
// Return: A pointer to the string of the control specified by id. The string returned is the string immediately preceeding the control in the dialog.
// Param: id The id of the item in the dialog.
// END_HELP_COMMENT
LPCSTR UIFileDlg::GetLabel(UINT id) const
{
// REVIEW(briancr): this function should move to the base class after all the common
// dialogs have been converted to use the new Win95 extension to common dialogs.
	// is the control an immediate child of this dialog?
	if (::GetDlgItem(HWnd(), id)) {
		return ::GetLabel(id, HWnd());
	}
	else {
		// the control must be on the extension dialog
		return ::GetLabel(id, GetExtensionDialog());
	}
}

// BEGIN_HELP_COMMENT
// Function: HWND UIFileDlg::GetExtensionDialog(void)
// Description: This function gets the HWND of the extension dialog associated with this common dialog. Extensions to common dialogs are implemented as child dialogs of the main dialog.
// Return: HWND of the extension dialog. NULL if it doesn't exist.
// END_HELP_COMMENT
HWND UIFileDlg::GetExtensionDialog(void) const
{
	HWND hControl;
	char acClass[32];

	// get the HWND of the first control in this dialog
	hControl = GetWindow(HWnd(), GW_CHILD);

	// interate through the controls in this dialog
	// until the extension dialog is found
	while (hControl) {
		GetClassName(hControl, acClass, 31);
		// if the control's class is the dialog class, it's the extension dialog
		// note that this assumes there's only one extension dialog...
		if (strncmp(acClass, "#32770", strlen("#32770")) == 0) {
			return hControl;
		}
		// get the next control
		hControl = GetNextWindow(hControl, GW_HWNDNEXT);
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//	UIFileSaveAsDlg class

// BEGIN_HELP_COMMENT
// Function: BOOL UIFileSaveAsDlg::VerifyUnique(void) const
// Description: Verify the currently active dialog is the Save As dialog.
// Return: TRUE if the currently active dialog is the Save As dialog; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFileSaveAsDlg::VerifyUnique(void) const
{
	return	VerifyDlgItemClass(IDC_FILE_FILENAME, GL_EDIT);// &&
//			VerifyDlgItemClass(IDC_FILE_FILENAMELIST, GL_LISTBOX) &&
//			!IsWindowVisible(GetDlgItem(IDC_FILE_OPENAS));
}


///////////////////////////////////////////////////////////////////////////////
//	UIFileOpenDlg class

// BEGIN_HELP_COMMENT
// Function: HWND UIFileOpenDlg::Display(void)
// Description: Bring up the File Open dialog.
// Return: The HWND of the File Open dialog.
// END_HELP_COMMENT
HWND  UIFileOpenDlg::Display(void)
{
	AttachActive();		  // Check if is already up
	if (!IsValid())
	{
	 UIWB.DoCommand(ID_FILE_OPEN, DC_MNEMONIC);
	 WaitAttachActive(5000);
	}
	return WGetActWnd(0); 
}	

// BEGIN_HELP_COMMENT
// Function: BOOL UIFileOpenDlg::VerifyUnique(void) const
// Description: Verify the currently active dialog is the File Open dialog.
// Return: TRUE if the currently active dialog is the File Open dialog; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFileOpenDlg::VerifyUnique(void) const
{
	return	VerifyDlgItemClass(IDC_FILE_FILENAME, GL_EDIT) &&
//			VerifyDlgItemClass(IDC_FILE_FILENAMELIST, GL_LISTBOX) &&
			VerifyDlgItemClass(IDC_FILE_OPENAS, GL_COMBOBOX);
}


// BEGIN_HELP_COMMENT
// Function: void UIFileOpenDlg::SetEditor(EOpenAs oa)
// Description: Specify the type of editor that should be used in the File Open dialog.
// Return: none
// Param: oa The type of editor to use; specified in the File Open dialog: OA_AUTO, OA_TEXT, OA_BINARY, OA_MAKEFILE.
// END_HELP_COMMENT
void UIFileOpenDlg::SetEditor(EOpenAs oa)
{
	ExpectValid();
	MST.WComboItemClk(GetLabel(IDC_FILE_OPENAS), oa+1);	// oa indexes from 0, MSTest indexes from 1
}

// BEGIN_HELP_COMMENT
// Function: void UIFileOpenDlg::SetReadOnly(BOOL b)
// Description: Check or uncheck the Open File as Read Only check box in the File Open dialog.
// Return: none
// Param: b TRUE to check the Open File as Read Only check box; FALSE to uncheck it.
// END_HELP_COMMENT
void UIFileOpenDlg::SetReadOnly(BOOL b)
{
	ExpectValid();
	if( b )
		MST.WCheckCheck(GetLabel(IDC_FILE_READONLY));
	else
		MST.WCheckUnCheck(GetLabel(IDC_FILE_READONLY));
}

///////////////////////////////////////////////////////////////////////////////
//	UIFontDlg class


// BEGIN_HELP_COMMENT
// Function: BOOL UIFontDlg::VerifyUnique(void) const
// Description: Determine whether the currently active dialog is the font dialog by verifying it contains a font name control, font style control, and font size control. This function is valid for the Font dialog used from the text tool in the image editors.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIFontDlg::VerifyUnique(void) const
{
	return	VerifyDlgItemClass(IDC_FONT_NAME, GL_COMBOBOX) &&
			VerifyDlgItemClass(IDC_FONT_STYLE, GL_COMBOBOX) &&
			VerifyDlgItemClass(IDC_FONT_SIZE, GL_COMBOBOX);
}

// BEGIN_HELP_COMMENT
// Function: void UIFontDlg::SetName(LPCSTR szName)
// Description: Set the font name in the Font dialog. This function is valid for the Font dialog used from the text tool in the image editors.
// Return: none
// Param: szName A pointer to a string containing the font name.
// END_HELP_COMMENT
void UIFontDlg::SetName(LPCSTR szName)
{
	MST.WComboItemClk(GetLabel(IDC_FONT_NAME), szName);
}

// BEGIN_HELP_COMMENT
// Function: void UIFontDlg::SetSize(int nSize)
// Description: Set the font size in the Font dialog. This function is valid for the Font dialog used from the text tool in the image editors.
// Return: none
// Param: nSize An integer containing the point size.
// END_HELP_COMMENT
void UIFontDlg::SetSize(int nSize)
{
	CString strSize;
	strSize.Format("%d", nSize);	// must pass string, or it thinks it's an index
	MST.WComboItemClk(GetLabel(IDC_FONT_SIZE), strSize);
}

// BEGIN_HELP_COMMENT
// Function: void UIFontDlg::SetStyle(int nStyle)
// Description: Set the font style in the Font dialog. This function is valid for the Font dialog used from the text tool in the image editors.
// Return: none
// Param: nStyle An integer containing a 1-based index into the list of styles for the selected font. This value is usually: 1 for regular style; 2 for italic style; 3 for bold style; and 4 for bold italic style.
// END_HELP_COMMENT
void UIFontDlg::SetStyle(int nStyle)
{
	MST.WComboItemClk(GetLabel(IDC_FONT_STYLE), nStyle);
}
