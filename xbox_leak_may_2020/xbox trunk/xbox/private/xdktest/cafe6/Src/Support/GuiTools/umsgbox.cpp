///////////////////////////////////////////////////////////////////////////////
//  UMSGBOX.H
//
//  Created by :            Date :
//      DavidGa                 1/12/94
//
//  Description :
//      Implementation of the UIMessageBox class
//

#include "stdafx.h"

#include "umsgbox.h"
#include "mstwrap.h"
#include "testutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

///////////////////////////////////////////////////////////////////////////////
//  UIMessageBox class

int UIMessageBox::m_idDefaultButton = 0;

int UIMessageBox::m_idOKButton = IDOK + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 1 : 2);
int UIMessageBox::m_idCancelButton = IDCANCEL + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 1 : 2);
int UIMessageBox::m_idAbortButton  = IDABORT + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 1 : 2);
int UIMessageBox::m_idRetryButton = IDRETRY + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 1 : 2);
int UIMessageBox::m_idIgnoreButton = IDIGNORE + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 1 : 2);
int UIMessageBox::m_idYesButton = IDYES + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 0 : 2);
int UIMessageBox::m_idNoButton = IDNO + 
	(((GetSystem() & SYSTEM_WIN) || (GetSystem() & SYSTEM_NT)) ? 0 : 2);

// BEGIN_HELP_COMMENT
// Function: BOOL UIMessageBox::IsValid(void) const
// Description: Determine if the message box is valid by checking that the window handle is valid and that the window class is #32770.
// Return: A Boolean value that indicates whether the message box is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIMessageBox::IsValid(void) const
{
    if( !UIWindow::IsValid() )
        return FALSE;
    
    char acClass[10];
    GetClassName(HWnd(), acClass, 9);
    return strcmp(acClass, "#32770") == 0;
}

// BEGIN_HELP_COMMENT
// Function: CString UIMessageBox::GetMessageText(void)
// Description: Get the message text from the message box.
// Return: A CString that contains the text of the message in the message box.
// END_HELP_COMMENT
CString UIMessageBox::GetMessageText(void)
{
    HWND hwndChild = GetWindow(HWnd(), GW_CHILD);
    HWND hwndLast = GetWindow(hwndChild, GW_HWNDLAST);
    return ::GetText(hwndLast);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMessageBox::ButtonClick(int btn /*=MSG_DEFAULT*/)
// Description: Click the specified button in the message box.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: btn An integer that specifies the ID of the button to click: MSG_DEFAULT, MSG_OK, MSG_CANCEL, MSG_ABORT, MSG_RETRY, MSG_IGNORE, MSG_YES, MSG_NO. (Default value is MSG_DEFAULT.)
// END_HELP_COMMENT
BOOL UIMessageBox::ButtonClick(int btn /*=MSG_DEFAULT*/)
{
    if( btn == MSG_DEFAULT )
        MST.WButtonClick(NULL);
    else if( !ButtonExists(btn) )
        return FALSE;
    else
        MST.WButtonClick(GetLabel(btn));
    return WaitUntilGone(1000);     // every button should make a message box go away
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIMessageBox::ButtonExists(int btn)
// Description: Determine whether the specified button exists on the message box.
// Return: A Boolean value that indicates whether the button exists (TRUE) or not.
// Param: btn An integer that contains the ID of the button.
// END_HELP_COMMENT
BOOL UIMessageBox::ButtonExists(int btn)
{
    return GetDlgItem(HWnd(), btn) != NULL;
}

// BEGIN_HELP_COMMENT
// Function: int UIMessageBox::GetActiveButton(void)
// Description: Get the ID of the active button on the message box.
// Return: An integer that specifies the ID of the active button.
// END_HELP_COMMENT
int UIMessageBox::GetActiveButton(void)
{
    for( int btn = MSG_OK; btn <= MSG_NO; btn++ )
        if( ButtonExists(btn) && MST.WButtonFocus(GetLabel(btn)) )
            return btn;
    return 0;   // huh?
}

