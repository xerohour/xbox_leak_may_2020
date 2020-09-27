//////////////////////////////
//  UTBARDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              1/13/94
//
//  Description :
//      Implementation of the UITBarDialog class
//

#include "stdafx.h"
#include "utbardlg.h"
#include "..\sym\cmdids.h"
#include "mstwrap.h"
#include "uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: int UITBarDialog::Display(void)
// Description: Bring up the Toolbars dialog.
// Return: TRUE if the Toolbars dialog is displayed; FALSE otherwise.
// END_HELP_COMMENT
int UITBarDialog::Display(void)
{
	if ( MST.WFndWndWait(GetLocString(IDSS_TB_TITLE), FW_PART, 1) != 0 )	 // Fix VerifyText function
		return TRUE; //UITB_NO_ERROR;						 // Dialog is already up

	UIWB.DoCommand(IDM_TOOLBAR_EDIT, DC_MNEMONIC);	
	
	if ( MST.WFndWndWait(GetLocString(IDSS_TB_TITLE), FW_PART, 1) == 0)		  // Fix VerifyText function
	{
		//LOG->Comment("WARNING: Toolbars Dialog was not displayed "); // Print isn't working
		return FALSE; //UITB_ERR_NOT_DISPLAYED;
	}

	return TRUE;   // UITB_NO_ERROR;
}


