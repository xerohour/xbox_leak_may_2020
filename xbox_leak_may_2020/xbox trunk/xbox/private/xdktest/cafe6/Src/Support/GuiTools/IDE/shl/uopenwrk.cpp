//////////////////////////////
//  UOPENWRK.CPP
//
//  Created by :            Date :
//      BrianCr				07/26/95
//
//  Description :
//      Implementation of the UIOpenWorkspaceDlg class
//

#include "stdafx.h"
#include "uopenwrk.h"
#include "..\sym\cmdids.h"
#include "mstwrap.h"
#include "uwbframe.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIOpenWorkspaceDlg::Display(void)
// Description: Bring up the Open Workspace dialog.
// Return: TRUE if the Open Workspace dialog is displayed; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIOpenWorkspaceDlg::Display(void)
{
	const char* const THIS_FUNCTION = "UIOpenWorkspaceDlg::Display";
	
	// is the dialog already displayed?
	AttachActive();
	if (!IsValid()) {
		UIWB.DoCommand(IDM_OPEN_WORKSPACE, DC_MNEMONIC);
		WaitAttachActive(2000);
	}
	BOOL bDisplayed = IsValid();
	if (!bDisplayed) {
		LOG->RecordInfo("%s: Open Workspace dialog was not displayed.", THIS_FUNCTION);
	}
	return bDisplayed;
}	
