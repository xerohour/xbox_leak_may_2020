//////////////////////////////
//  UITIPDLG.CPP
//
//  Created by :            Date :
//      BrianCr				07/11/95
//
//  Description :
//      Implementation of the UITipDialog class
//

#include "stdafx.h"
#include "uitipdlg.h"
#include "..\sym\cmdids.h"
#include "mstwrap.h"
#include "uwbframe.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UITipDialog::Display(void)
// Description: Bring up the Tip of the Day dialog.
// Return: TRUE if the Tip of the Day dialog is displayed; FALSE otherwise.
// END_HELP_COMMENT
BOOL UITipDialog::Display(void)
{
	const char* const THIS_FUNCTION = "UITipDialog::Display";
	
	if ( MST.WFndWndWait(GetLocString(IDSS_TIP_TITLE), FW_PART, 1) != 0 )
		// the dialog's already up
		return TRUE;

	UIWB.DoCommand(IDM_HELP_TIPOFTHEDAY, DC_MNEMONIC);	
	
	if ( MST.WFndWndWait(GetLocString(IDSS_TIP_TITLE), FW_PART, 1) == 0)
	{
		LOG->RecordInfo("%s: Tip of the Day dialog was not displayed.", THIS_FUNCTION);
		return FALSE;
	}

	return TRUE;
}


