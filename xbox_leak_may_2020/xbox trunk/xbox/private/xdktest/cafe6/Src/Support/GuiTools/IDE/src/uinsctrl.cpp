///////////////////////////////////////////////////////////////////////////////
//  UINSCTRL.CPP
//
//  Created by :            Date :
//      MichMa					5/28/97
//
//  Description :
//      Implementation of the UIInsertCtrlDlg class
//

#include "stdafx.h"
#include "uinsctrl.h"
#include "mstwrap.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIInsertCtrlDlg::Activate(void)
// Description: Open the "Insert ActiveX Control" dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIInsertCtrlDlg::Activate(void)
{

	// TODO(michma): must be localized.
	// how to add this command to cmdarray.cpp?
	MST.DoKeys("%ihx");

	// wait for "Insert ActiveX Control" dlg to show up (which could take
	// a while if there are lots of controls registered).
	if(WaitAttachActive(10000))
		return TRUE;

	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIInsertCtrlDlg::SetControl(LPCSTR szCtrl)
// Description: Sets the control in the "Insert ActiveX Control" dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: exp A pointer to a string that contains the control to set in the "Insert ActiveX Control" dialog.
// END_HELP_COMMENT
BOOL UIInsertCtrlDlg::SetControl(LPCSTR szCtrl)

{
	
	// make sure the dlg is active.
	if(!IsActive()) 
	{
			LOG->RecordInfo("ERROR! UIInsertCtrlDlg::SetControl - %s dlg is not active.", ExpectedTitle());
	 		return FALSE;
	}

	// make sure the list of controls is enabled.
	if(!MST.WListEnabled("@1")) 
	{
			LOG->RecordInfo("ERROR! UIInsertCtrlDlg::SetControl - control list is not enabled.");
	 		return FALSE;
	}

	// make sure the list of controls exists.
	if(!MST.WListExists("@1")) 
	{
			LOG->RecordInfo("ERROR! UIInsertCtrlDlg::SetControl - control list does not exist.");
	 		return FALSE;
	}

	// make sure the list of controls is enabled.
	if(!MST.WListEnabled("@1")) 
	{
			LOG->RecordInfo("ERROR! UIInsertCtrlDlg::SetControl - control list is not enabled.");
	 		return FALSE;
	}

	// select the control.
	// TODO(michma): because the ide stores the guid for the controls in the listbox,
	// even though they don't get displayed, ms-test cannot seem to verify they exist.
	// so we just type out the name which scrolls the listbox to the appropriate item.
	// we need some better verification for this though.
	MST.DoKeys(szCtrl);	

	return TRUE;
}

