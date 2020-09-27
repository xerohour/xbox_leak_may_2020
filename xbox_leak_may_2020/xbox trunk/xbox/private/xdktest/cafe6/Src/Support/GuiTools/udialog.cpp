///////////////////////////////////////////////////////////////////////////////
//  UDIALOG.H
//
//  Created by :            Date :
//      DavidGa                 9/20/93
//
//  Description :
//      Declaration of the UIDialog class
//

#include "stdafx.h"
#include "testxcpt.h"
#include "udialog.h"
#include "testutil.h"
#include "mstwrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

UIDialog::UIDialog( LPCSTR szTitle )
: UIWindow()
{
	SetExpectedTitle(szTitle);
}

UIDialog::UIDialog( UINT idCtrl, int nType )
: UIWindow()
{
	m_UniqueCtrl.m_id = idCtrl;
	m_UniqueCtrl.m_type = nType;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::Open(void)
// Description: Open the dialog. This member function must be overridden in the derived class.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDialog::Open(void) const
{
	// override this in the derived class
	ASSERT(0);
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::IsValid(void) const
// Description: Determine if the dialog is valid by verifying that the dialog is a valid window and by calling VerifyClass and VerifyUnique.
// Return: A Boolean value that indicates whether the dialog is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDialog::IsValid(void) const
{
	if( !UIWindow::IsValid() )
		return FALSE;

	return VerifyClass() && VerifyUnique();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::VerifyClass(void) const
// Description: Determine if the window class of the dialog is #32770.
// Return: A Boolean value that indicates whether the window class is #32770 (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDialog::VerifyClass(void) const
{
	char acClass[32];
	GetClassName(HWnd(), acClass, 31);
	return ( lstrcmpi(acClass, "#32770") == 0 );
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::VerifyUnique(void) const
// Description: Verify that this dialog is unique.
// Return: A Boolean value that indicates whether the dialog is unique (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDialog::VerifyUnique(void) const
{				// if no derived class has overridden this function,
	if( m_UniqueCtrl.IsValid() )	// then either verify unique dialog control
		return VerifyDlgItemClass(m_UniqueCtrl.m_id, m_UniqueCtrl.m_type);
	else
		return VerifyTitle();	// or verify dialog caption
}

// BEGIN_HELP_COMMENT
// Function: LPCSTR UIDialog::ExpectedTitle(void) const
// Description: Get the title of the dialog (as expected by this object--not from the actual dialog).
// Return: A pointer to a string that contains the title of the dialog.
// END_HELP_COMMENT
CString UIDialog::ExpectedTitle(void) const
{
	return m_strTitle.IsEmpty()? "" : m_strTitle;
}

// BEGIN_HELP_COMMENT
// Function: void UIDialog::SetExpectedTitle(LPCSTR szTitle)
// Description: Set the expected title for this dialog.
// Return: <none>
// Param: szTitle A pointer to a string that contains the expected title of this dialog.
// END_HELP_COMMENT
void UIDialog::SetExpectedTitle(LPCSTR szTitle)
{
	if( szTitle == NULL )
		m_strTitle.Empty();
	else
		m_strTitle = szTitle;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::VerifyTitle(void) const
// Description: Determine if the title of the dialog matches the expected title of the dialog.
// Return: A Boolean value that indicates whether the title of the dialog matches the expected title (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDialog::VerifyTitle(void) const
{
	CString strTitle = ExpectedTitle();   // or against the expected titlebar text
	if (strTitle.IsEmpty())        // if that is also NULL
		return TRUE;        // then no particular title is expected.  REVIEW: does this make sense?

	char acBuf[256];
	GetText(acBuf, 255);
	return lstrcmpi(acBuf, strTitle) == 0;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIDialog::GetDlgItem(UINT id) const
// Description: Get the HWND of an item in the dialog.
// Return: The HWND of the specified item in the dialog.
// Param: id An integer that specifies the ID of the item in the dialog.
// END_HELP_COMMENT
HWND UIDialog::GetDlgItem(UINT id) const
{
	return ::GetDlgItem(HWnd(), id);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDialog::VerifyDlgItemClass(UINT id, int nClass) const
// Description: Verify that the class of the specified dialog item matches the given class.
// Return: A Boolean value that indicates whether the class of the specified dialog item matches the given class (TRUE) or not.
// Param: id An integer that specifies the ID of the item in the dialog.
// Param: nClass An integer that specifies the class of the dialog item.
// END_HELP_COMMENT
BOOL UIDialog::VerifyDlgItemClass(UINT id, int nClass) const
{
	return  (GetDlgItem(id) != NULL) &&
			(GetDlgControlClass(GetDlgItem(id)) == nClass);
}

// BEGIN_HELP_COMMENT
// Function: HWND UIDialog::OK(void)
// Description: Click the OK button in the dialog.
// Return: NULL if successful; the HWND of a message or dialog box that has focus otherwise.
// END_HELP_COMMENT
HWND UIDialog::OK(void)
{
	ExpectValid();

//	if( !MST.WButtonExists(UD_BTN_OK) )
//		return FALSE;

	MST.WButtonClick(GetLabel(IDOK));
	if( WaitUntilGone(1000) )
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}

// BEGIN_HELP_COMMENT
// Function: HWND UIDialog::Cancel(void)
// Description: Click the Cancel button in the dialog.
// Return: NULL if successful; the HWND of a message or dialog box that has focus otherwise.
// END_HELP_COMMENT
HWND UIDialog::Cancel(void)
{
	ExpectValid();

//	if( !MST.WButtonExists(UD_BTN_CANCEL) )
//		return FALSE;

	MST.WButtonClick(GetLabel(IDCANCEL));
	if( WaitUntilGone(1000) )
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}

// BEGIN_HELP_COMMENT
// Function: HWND UIDialog::Close(void)
// Description: Click the Close button in the dialog.
// Return: NULL if successful; the HWND of a message or dialog box that has focus otherwise.
// END_HELP_COMMENT
HWND UIDialog::Close(void)
{
	ExpectValid();

	if( MST.WButtonExists("Close") )		// LOCALIZE
		MST.WButtonClick("Close");		// use the close button if there is one
	else if( MST.WButtonExists(GetLabel(IDCANCEL) ))	// use the Cancel button if not 
		MST.WButtonClick(GetLabel(IDCANCEL));
		else
			PostMessage(HWnd(), WM_CLOSE, 0, 0);	// otherwise, close it however we must

	if( WaitUntilGone(3000) )		// REVIEW(davidga) 3000 is too arbitrary
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}

// BEGIN_HELP_COMMENT
// Function: HWND UIDialog::Help(void)
// Description: Click the Help button in the dialog.
// Return: NULL if successful; the HWND of a message or dialog box that has focus otherwise.
// END_HELP_COMMENT
HWND UIDialog::Help(void)
{
	ExpectValid();

	if( !MST.WButtonExists("Help") )
		return NULL;
	MST.WButtonClick ("Help");

	int n = 0;
	while( MST.WGetActWnd(0) == HWnd() )
	{
		if( n++ > 10 )		
			return NULL;	// no new window popped up within 10 seconds
		Sleep(1000);
	}
	return MST.WGetActWnd(0);	// return HWND of Help window
}
