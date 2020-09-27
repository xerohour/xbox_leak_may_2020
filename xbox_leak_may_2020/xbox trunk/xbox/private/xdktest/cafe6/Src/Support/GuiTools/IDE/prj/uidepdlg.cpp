//////////////////////////////
//  UIDEPDLG.CPP
//
//  Created by :            Date :
//      BrianCr				08/04/95
//
//  Description :
//      Implementation of the UIUpdateDepDialog class
//

#include "stdafx.h"
#include "uidepdlg.h"
#include "..\sym\cmdids.h"
#include "mstwrap.h"
#include "..\shl\uwbframe.h"
#include "guiv1.h"

#define new DEBUG_NEW

// BEGIN_HELP_COMMENT
// Function: BOOL UIUpdateDepDialog::Display(void)
// Description: Bring up the Update Dependencies dialog.
// Return: TRUE if the Update Dependencies dialog is displayed; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIUpdateDepDialog::Display(void)
{
	const char* const THIS_FUNCTION = "UIUpdateDepDialog::Display";
	
	// is the dialog already displayed?
	AttachActive();
	if (!IsValid()) {
		UIWB.DoCommand(IDM_PROJECT_SCAN_ALL, DC_MNEMONIC);
		WaitAttachActive(2000);
	}
	BOOL bDisplayed = IsValid();
	if (!bDisplayed) {
		LOG->RecordInfo("%s: Update Dependencies dialog was not displayed.", THIS_FUNCTION);
	}
	return bDisplayed;
}


