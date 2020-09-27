///////////////////////////////////////////////////////////////////////////////
//  UIATTACH.CPP
//
//  Created by:		MichMa		
//	Date:			2/18/98
//  Description:	Implementation of the UIAttachToProcessDlg class
//

#include "stdafx.h"
#include "uiattach.h"
#include "..\shl\uwbframe.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "guitarg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIAttachToProcessDlg::Activate(void)
// Description: Open the Attach To Process dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIAttachToProcessDlg::Activate(void)

{
	UIWB.DoCommand(IDM_BUILD_DEBUG_ATTACH, DC_MNEMONIC);

	if(WaitAttachActive(5000))
		return TRUE;
	
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAttachToProcessDlg::ShowSystemProcesses(BOOL bShowSystemProcesses)
// Description: Toggles display of system processes in the Attach To Process dlg.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: bShowSystemProcesses - A boolean value specifying whether or not to show system processes.
// END_HELP_COMMENT
BOOL UIAttachToProcessDlg::ShowSystemProcesses(BOOL bShowSystemProcesses)

{
	char szThisFunc[] = "UIAttachToProcessDlg::ShowSystemProcesses";

	// verify attach to process dlg is up.
	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in %s: 'Attach To Process' dlg isn't active.", szThisFunc);
		return FALSE;
	}

	// check if the Show System Processes checkbox is enabled.
	if(!MST.WCheckEnabled("@1"))
	{
		LOG->RecordInfo("ERROR in %s: 'Show System Processes' checkbox isn't enabled.", szThisFunc);
		return FALSE;
	}
	
	// toggle the Show System Processes checkbox appropriately.
	if(bShowSystemProcesses)
		MST.WCheckCheck("@1");
	else
		MST.WCheckUnCheck("@1");

	// wait for the dialog to update its list with the system processes.
	WaitForInputIdle(g_hTargetProc, 10000);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAttachToProcessDlg::SelectProcess(LPCSTR szProcessName)
// Description: Selects a process in the Attach To Process dlg.
// Return: A boolean value that indicates success or failure. TRUE if successful, FALSE if not.
// Param: szProcessName - A string specifying the name of the process to select.
// END_HELP_COMMENT
BOOL UIAttachToProcessDlg::SelectProcess(LPCSTR szProcessName)

{
	char szThisFunc[] = "UIAttachToProcessDlg::SelectProcess";

	// verify attach to process dlg is up.
	if(!IsActive())
	{
		LOG->RecordInfo("ERROR in %s: 'Attach To Process' dlg isn't active.", szThisFunc);
		return FALSE;
	}

	// check if the specified process exists in the process list.
	if(!MST.WViewItemExists("@1", szProcessName))
	{
		LOG->RecordInfo("ERROR in %s: couldn't find process '%s'.", szThisFunc, szProcessName);
		return FALSE;
	}

	MST.WViewItemClk("@1", szProcessName);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAttachToProcessDlg::OK(void)
// Description: OK's the Attach To Process dlg.
// Return: NULL if successful, otherwise the handle of the window that was active after the OK was attempted on the Attach To Process dlg.
// END_HELP_COMMENT
HWND UIAttachToProcessDlg::OK(void)

{
	// TODO(michma): for some reason the debugger gets hung when attaching if we close the dialog via ms-test's 
	// WButtonClick like UIDialog::OK() does. so we are working around this by sending the keys more directly.
	WaitStepInstructions("Sending RETURN key");
	keybd_event(VK_RETURN, 0,0,0);
	keybd_event(VK_RETURN, 0,KEYEVENTF_KEYUP,0);

	if(WaitUntilGone(1000))
		return NULL;		
	else
		return MST.WGetActWnd(0);
}
