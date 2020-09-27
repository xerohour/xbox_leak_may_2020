///////////////////////////////////////////////////////////////////////////////
//  UIWBDLG.CPP
//
//  Created by :            Date :
//      DavidGa                 9/20/93
//
//  Description :
//      Declaration of the UIWBDialog class
//

#include "stdafx.h"
#include "uiwbdlg.h"
#include "testxcpt.h"
#include "..\..\testutil.h"
#include "mstwrap.h"
#include "wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

UIWBDialog::UIWBDialog(LPCSTR szTitle)
: UIDialog(szTitle)
{
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBDialog::Close(void)
// Description: Close an IDE dialog.
// Return: NULL if the dialog closed successfully; the HWND of a message box otherwise.
// END_HELP_COMMENT
HWND UIWBDialog::Close(void)
{
	ExpectValid();

	if( MST.WButtonExists(GetLocString(IDSS_CLOSE)) )	
		MST.WButtonClick(GetLocString(IDSS_CLOSE));		// use the close button if there is one
	else if( MST.WButtonExists(GetLabel(IDOK) ))		// use the OK button if not 
		MST.WButtonClick(GetLabel(IDOK));				// makes sure changes are commited
		else
			PostMessage(HWnd(), WM_CLOSE, 0, 0);		// otherwise, close it however we must

	if( WaitUntilGone(3000) )		// REVIEW(davidga) 3000 is too arbitrary
		return NULL;				// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBDialog::Help(void)
// Description: Bring up help from an IDE dialog. This function clicks the Help button on the dialog.
// Return: The HWND of the help window, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIWBDialog::Help(void)
{
	ExpectValid();

	if( !MST.WButtonExists(GetLocString(IDSS_HELP)) )
		return NULL;
	MST.WButtonClick (GetLocString(IDSS_HELP));

	int n = 0;
	while( MST.WGetActWnd(0) == HWnd() )
	{
		if( n++ > 10 )		
			return NULL;	// no new window popped up within 10 seconds
		Sleep(1000);
	}
	return MST.WGetActWnd(0);	// return HWND of Help window
}
