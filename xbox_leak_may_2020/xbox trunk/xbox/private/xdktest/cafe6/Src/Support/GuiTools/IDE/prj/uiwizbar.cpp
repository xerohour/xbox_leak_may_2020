///////////////////////////////////////////////////////////////////////////////
//  UIWIZBAR.CPP
//
//  Created by :            Date :
//      Anita George       9/4/96	copied and modified from UCWZDLG.CPP
//                                
//  Description :
//      Implementation of the UIWizardBar class
//

#include "stdafx.h"
#include "odbcdlgs.h"
#include "uiwizbar.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "guitarg.h"
#include "..\shl\ucustdlg.h"
#include "..\wrk\uiwrkspc.h"
#include "..\..\umsgbox.h"
//#include "..\sym\vshell.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

BOOL UIWizardBar::InitializeWizBar()  
{
	UICustomizeTabDlg uiTBD;
	uiTBD.DisplayBars();
	MST.DoKeys("{END}"); //selects wizard bar toolbar option at bottom of list
	MST.WButtonClick(GetLabel(VSHELL_IDC_RESET));
	uiTBD.DisplayKeyboardTab();
	//LOG->RecordInfo("MyHnd is %d",MyHnd);
	if (uiTBD.SelectKeyAssignment(GetLocString(IDSS_CUSTKB_MAIN),GetLocString(IDSS_CUSTKB_VIEW),COMMAND_STR)) //MST.WComboExists("#35074"))
	{
		LOG->RecordInfo("in if");
		Sleep(5000);				// TODO: Fix timing problem properly.
		MST.DoKeys("^(+(1))");
		return TRUE;
	}
	else
	{
		LOG->RecordFailure("Did not assign keyboard command to Wizard Bar correctly.");
		return FALSE;
	}
	
}

BOOL UIWizardBar::AddNewClass()  //implementation not complete
//new class functionality not yet in IDE
{
	MST.DoKeys("^(+(1))");  //make sure focus is in class combo of wizbar
	MST.DoKeys("+({F10})");		// Bring up the popup context menu.

	CString strGotoMenuItem = GetLocString(IDSS_NEW_CLASS);

	if (!MST.WMenuExists(strGotoMenuItem))
	{
		LOG->RecordFailure("Menu item %s doesn't exist",strGotoMenuItem);
		return FALSE;
	}
	else
	{
		MST.WMenu(strGotoMenuItem);
		return TRUE;
	}

}

void UIWizardBar::SelectClass(CString strClassName)  
{
	MST.DoKeys("^(+(1))");  //make sure focus is in class combo of wizbar
	MST.DoKeys(strClassName);		// Bring up the popup context menu.
	MST.DoKeys("{ENTER}");
	Sleep(5000);
	//will detect if correct class selected at end of UIWizardBar::GotoDefinitionFromHere
	MST.DoKeys("^(+(1))");  //put focus back in class combo of wizbar
}
// BEGIN_HELP_COMMENT
// Function: int UIWizardBar::GotoClassDefinition(LPCSTR pszClassName, COSource* pSrcFile = NULL)
// Description: Navigates to definition of the given class in the workspace window.  It assumes that the ClassView is currently the active view.  It currently does not support nested classes.
// Parameter: pszClassName Name of class to find in the workspace window.
// Parameter: pSrcFile - Input: Ptr to an allocated COSource object.  Output: Object is attached to source file that was activated by the Goto.  If NULL the parameter is ignored.
// Parameter: pszClassDef Line of code that should be selected when jumping to class definition.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWizardBar::GotoClassDefinition(LPCSTR pszClassName, COSource* pSrcFile/* = NULL*/, LPCSTR pszClassDef)
{
	SelectClass(pszClassName);

	Sleep(5000);

	if (GotoDefinitionFromHere(pszClassName, pszClassDef))
	{
		if (pSrcFile != NULL)
			pSrcFile->AttachActiveEditor();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
// BEGIN_HELP_COMMENT
// Function: int UIWizardbar::GotoDefinitionFromHere(LPCSTR pszName)
// Description: Navigates to the definition of the currently selected item in the workspace window.  It assumes that the ClassView is currently the active view.  This function is useful for calling right after successfully calling one of the SelectXXX functions.
// Parameter: pszName Name that should appear in wizardbar class combo.
// Parameter: pszClassDef Line of code that should be selected when jumping to class definition.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWizardBar::GotoDefinitionFromHere(LPCSTR pszName,LPCSTR pszClassDef)
{
	MST.DoKeys("+({F10})");		// Bring up the popup context menu.

	Sleep(10000);

	CString strGotoMenuItem = GetLocString(IDSS_WIZBAR_CLASSDEF);

	if (!MST.WMenuExists(strGotoMenuItem))
	{
		LOG->RecordFailure("Menu item %s doesn't exist",strGotoMenuItem);
		return FALSE;
	}
	else
	{

		MST.WMenu(strGotoMenuItem);

		WaitForInputIdle(g_hTargetProc, 10000);	// Wait for processing to end.
		Sleep(10000);							// Wait for message box to arrive.

		UIMessageBox wndMsgBox;
		if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
		{
			return FALSE;
			LOG->RecordFailure("error selecting menu item");
			wndMsgBox.ButtonClick();
		}
		else									// A source window must've become active.
		{
			COSource SrcFile;
		
			SrcFile.AttachActiveEditor();
	
			CString strFound = SrcFile.GetSelectedText();
			CString strBase = pszClassDef;

			// Remove spaces from both the source and target strings for comparison.
			UIWorkspaceWindow WS;
			WS.RemoveCharFromString(&strBase, ' ');
			WS.RemoveCharFromString(&strFound, ' ');

			if (strFound.Find(strBase) == -1)
			{
				LOG->RecordFailure("UIWizardBar::GotoDefinitionFromHere() Found: %s\n", (LPCSTR)strFound);
				LOG->RecordInfo("UIWizardbar::GotoDefinitionFromHere() Looking for: %s\n", (LPCSTR)strBase);
				return FALSE;
			}
			else
			{
				LOG->RecordInfo("Went to class definition correctly.");
				return TRUE;
			}
		}  //end else for no msgbox when selecting menu item
	}  //end else for menu item
}
