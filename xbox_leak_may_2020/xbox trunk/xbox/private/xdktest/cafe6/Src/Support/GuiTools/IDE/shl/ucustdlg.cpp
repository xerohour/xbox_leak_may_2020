///////////////////////////////////////////////////////////////////////////////
//  UCUSTDLG.CPP
//
//  Created by :            Date :
//      EnriqueP              2/7/94
//                                
//  Description :
//      Implementation of the UICustomizeTabDlg class    
//

#include "stdafx.h"
#include "w32repl.h"
#include "ucustdlg.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "guiv1.h"
#include "guitarg.h"
#include "uwbframe.h"



#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: HWND UICustomizeTabDlg::Display() 
// Description: Bring up the Customize tabbed dialog.
// Return: The HWND of the Customize dialog.
// END_HELP_COMMENT
HWND UICustomizeTabDlg::Display() 
{       
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
		UIWB.DoCommand(IDM_CUSTOMIZE, DC_MNEMONIC);
		AttachActive();
	}
	return WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: HWND UICustomizeTabDlg::Display() 
// Description: Bring up the Customize tabbed dialog.
// Return: The HWND of the Customize dialog.
// END_HELP_COMMENT
HWND UICustomizeTabDlg::DisplayBars() 
{       
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
		UIWB.DoCommand(IDM_CUSTOMIZE, DC_MNEMONIC);
		AttachActive();
	}
    ShowPage(TAB_TOOLBARS,5);
	return WGetActWnd(0); 
}


// BEGIN_HELP_COMMENT
// Function: int UICustomizeTabDlg::AddTBButton(int nButtonType, HWND hwndToolbar /* = NULL */)
// Description: Add a toolbar button (nButtonType) to the toolbar indicated by hwndToolbar.
// Return: TRUE if successful; ERROR_ERROR otherwise.
// Param: nButtonType The button to add to the toolbar (this is NYI).
// Param: hwndToolbar The HWND of the toolbar to add the button to. NULL means create a new toolbar. (Default value is NULL.)
// END_HELP_COMMENT
int UICustomizeTabDlg::AddTBButton(int nButtonType, HWND hwndToolbar /* = NULL */)
{
    Display();          //REVIEW: Verify Toolbars tab is set as well
    ShowPage(TAB_COMMANDS,5);
	POINT ptButtonPos;
//      ptButtonPos = SelectButtonCategory(nButtonType);        Not Yet Implemented
	ptButtonPos.x = 10;
	ptButtonPos.y = 10;
	if ( hwndToolbar != NULL)                               // Put button in a toolbar else create new toolbar
    {
	if(!IsWindowVisible(hwndToolbar) )
		{
			LOG->RecordFailure("The toolbar was not visible so no button was added");
			return ERROR_ERROR;
		}
	DragMouse(VK_LBUTTON, GetCurrentMiniPage(), ptButtonPos.x, ptButtonPos.y, hwndToolbar, 2, 2);     //REVIEW: Button position is hardcoded
    }
    else
    {
	DragMouse(VK_LBUTTON, GetCurrentMiniPage(), ptButtonPos.x, ptButtonPos.y, HWnd(), -50, 100);
    }

    return TRUE;
}

HWND UICustomizeTabDlg::GetCurrentMiniPage()
{
	HWND hwnd = GetCurrentPage();   
	hwnd =  GetWindow(hwnd, GW_CHILD);        // Get First Page child
	char acClass[32];
	while (hwnd)                                                           // Search among siblings to find dialog page
	{
		::GetClassName(hwnd, acClass, 31);
		if( strncmp(acClass, "Afx:", 4) == 0 )
		{
			if ( ::GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE)
				return hwnd;                                                    // Return the page that is visible
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);            
	}
								
	return NULL;                                                            // No Visible page was found  (who knows why)
}

HWND UICustomizeTabDlg::BrowseForMacroFiles()
{
	MST.WButtonClick(GetLabel(AUTO1_IDC_BROWSE)) ;
	WaitForInputIdle(g_hTargetProc, 6000);
	return MST.WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: HWND UICustomizeTabDlg::DisplayKeyboardTab() 
// Description: Bring up the Customize Keyboard tabbed dialog.
// Return: The HWND of the Customize dialog.
// END_HELP_COMMENT
HWND UICustomizeTabDlg::DisplayKeyboardTab() 
{       
	AttachActive();           // Check if is already up
	if (!IsValid())
	{
		UIWB.DoCommand(IDM_CUSTOMIZE, DC_MNEMONIC);
		AttachActive();
	}
    ShowPage(TAB_KEYBOARD,4);
	return WGetActWnd(0); 
}

// BEGIN_HELP_COMMENT
// Function: BOOL UICustomizeTabDlg::SelectKeyAssignment(CString strEditor,CString strCategory,CString strCommand) 
// Description: Assigns the shortcut ctrl+shift+1 to a command.  Assumes keyboard tab is activated.
// This function resets all assignments and is limited to making only one keyboard assignment.
// PARAM:strEditor-String indicating Editor combobox choice-Main, Dialog,Text, Image
// PARAM:strCategory-String indicating Category listbox choice-File, Edit, View, etc.
// PARAM:strCommand-String indicating Command listbox choice-FileClose, FileCloseWorkspace, etc.
// Return: TRUE if successfully assigned, otherwise FALSE.
// END_HELP_COMMENT
BOOL UICustomizeTabDlg::SelectKeyAssignment(CString strEditor,CString strCategory,CString strCommand) 
{  
	UITabbedDialog::SetOption(TAB_CURRENT,VSHELL_IDC_EDITOR_LIST, strEditor);
	LPCSTR pOption = UITabbedDialog::GetOptionStr(TAB_CURRENT,VSHELL_IDC_EDITOR_LIST);
	if(pOption==NULL || pOption!= strEditor)
	{
		LOG->RecordFailure("Editor called '%s' does not exist",strEditor);
		return FALSE;
	}
	else  //editor set correctly, so set catergory
	{
		//reset key assignments for editor
		MST.WButtonClick(GetLabel(VSHELL_IDC_RESET_ALL));
		MST.DoKeys("{ENTER}");

		UITabbedDialog::SetOption(TAB_CURRENT,VSHELL_IDC_CAT_LIST, strCategory);
		if (UITabbedDialog::GetOptionStr(TAB_CURRENT,VSHELL_IDC_CAT_LIST) != strCategory)
		{
			LOG->RecordFailure("Category called '%s' does not exist",strCategory);
			return FALSE;
		}
		else  //catergory set correctly, so set command
		{
			UITabbedDialog::SetOption(TAB_CURRENT,VSHELL_IDC_CMD_LIST, strCommand);
			if (UITabbedDialog::GetOptionStr(TAB_CURRENT,VSHELL_IDC_CMD_LIST) != strCommand)
			{
				LOG->RecordFailure("Command called '%s' does not exist",strCommand);
				return FALSE;
			}
			else
			{
				//this is the only way to get to edit box
				MST.DoKeys("{TAB 3}"); //need to change this if tab order changes
				//press control+shift+1
				CString Keys = "^(+(1))";
				MST.DoKeys(Keys);
				if (!(MST.WButtonEnabled(GetLabel(VSHELL_IDC_ASSIGN))))
				{
					LOG->RecordFailure("Assign button isn't enabled");
					return FALSE;
				}
				else
				{
					MST.WButtonClick(GetLabel(VSHELL_IDC_ASSIGN));
					//button should now be disabled
					if (MST.WButtonEnabled(GetLabel(VSHELL_IDC_ASSIGN)))
					{
						LOG->RecordFailure("didn't assign shortcut properly-assign button still enabled");
						return FALSE;
					}
					else
					{
						MST.WButtonClick(GetLocString(IDSS_CLOSE));
						if (MST.WFndWnd(GetLocString(IDSS_CUSTOMIZE_TITLE),FW_ACTIVE) != 0)
							LOG->RecordFailure("Customize dialog didn't close properly.");
					}
				}  //end else for assign button
			}  //end else for command
		}  //end else for category
	}  //end else for editor

	return TRUE;
}

 