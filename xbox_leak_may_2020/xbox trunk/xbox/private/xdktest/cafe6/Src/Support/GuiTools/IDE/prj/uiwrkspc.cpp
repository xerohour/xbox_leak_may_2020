///////////////////////////////////////////////////////////////////////////////
//  UIWRKSPC.CPP
//
//  Created by :            Date :
//      JimGries            4/25/95         
//
//  Description :
//      Implementation for the UIWorkspaceWindow
//

#include "stdafx.h"
#include "uiwrkspc.h"
#include "..\..\testutil.h"
#include "..\shl\uwbframe.h"
#include "..\shl\upropwnd.h"
#include "..\shl\wbutil.h"
#include "..\sym\cmdids.h"
#include "guiv1.h"
#include "guitarg.h"
#include "mstwrap.h"
#include "..\..\umsgbox.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: UIWorkspaceWindow::UIWorkspaceWindow(void)
// Description: Constructor for Workspace window object.
// END_HELP_COMMENT
UIWorkspaceWindow::UIWorkspaceWindow()
{
    Activate();
    Attach(FindFirstChild(UIWB.HWnd(), IDDW_PROJECT));
}


// BEGIN_HELP_COMMENT
// Function: UIWorkspaceWindow::GetID(void) const
// Return: The ID of the UIWorkspaceWindow.
// END_HELP_COMMENT
UINT UIWorkspaceWindow::GetID(void) const
{
	return IDDW_PROJECT;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::Activate(void)
// Description: Activates the Workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::Activate(void)
{
    return UIWB.DoCommand(IDM_VIEW_WORKSPACE, DC_MNEMONIC);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::ActivateBuildPane()
// Description: Activates the Build pane in the workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::ActivateBuildPane()
{
    return ActivatePane(GetLocString(IDSS_WRKSPC_BUILD_PANE));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::ActivateDataPane()
// Description: Activates the Data pane in the workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::ActivateDataPane()
{
    return ActivatePane(GetLocString(IDSS_WRKSPC_DATA_PANE));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::ActivateHelpPane()
// Description: Activates the Help pane in the workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::ActivateHelpPane()
{
    return ActivatePane(GetLocString(IDSS_WRKSPC_HELP_PANE));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::ActivateClassPane()
// Description: Activates the Class browser pane in the workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::ActivateClassPane()
{
    return ActivatePane(GetLocString(IDSS_WRKSPC_CLASS_PANE));
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWorkspaceWindow::ActivateResourcePane()
// Description: Activates the Resource pane in the workspace window.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::ActivateResourcePane()
{
    return ActivatePane(GetLocString(IDSS_WRKSPC_RESOURCE_PANE));
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectProject(LPCSTR pszProjectName)
// Description: Selects the given project in the workspace window.  It assumes that the ClassView or FileView is currently the active view.
// Parameter: pszProjectName Name of project to select.
// Return: SUCCESS if successful, or ERROR_ERROR if some other problem occurred.
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectProject(LPCSTR pszProjectName)
{
	ASSERT(pszProjectName);

	// Contract all children.
	MST.DoKeys("{Home}{Left}");

	// Type the name of the item to find.
	MST.DoKeys(pszProjectName, TRUE);

	// Popup the property page for the currently active item.
	MST.DoKeys("%({Enter})");

	int ret = SUCCESS;

	CStatic wndStatic;
	if (wndStatic.Attach(FindFirstChild(MST.WGetActWnd(0), IDC_TARGNAME)) == 0)
		ret = ERROR_NO_PROJECT;
	else
	{
		CString strFoundName;
		wndStatic.GetWindowText(strFoundName);

		if (strnicmp(strFoundName, pszProjectName, strlen(pszProjectName)))
			 ret = ERROR_NO_PROJECT; 

		wndStatic.Detach();
	}

	// Dismiss the properties dialog.	
	MST.DoKeys("{Escape}");

	return ret;
}
// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectClass(LPCSTR pszClassName, LPCSTR pszProjectName = NULL)
// Description: Selects the given class in the workspace window.  It assumes that the ClassView is currently the active view.  It currently does not support nested classes.  This function leaves the class view in a state suitable for use by SelectClassMember.
// Parameter: pszClassName Name of class to find in the workspace window.
// Parameter: pszProjectName Name of project in which the class resides.  If NULL it assumes the current project.
// Return: SUCCESS if successful; ERROR_NO_CLASS if class wasn't found, or ERROR_ERROR if some other problem occurred.
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectClass(LPCSTR pszClassName, LPCSTR pszProjectName /* = NULL */)
{
	ASSERT(pszClassName);

	int ret = SUCCESS;

	if (pszProjectName)
		ret = SelectProject(pszProjectName);

	if (ret == SUCCESS)
	{
		// We have to wait here until the ClassView is able to display the class
		// information.  WaitForInputIdle() does not work here.  This loop assumes
		// that there will be some selectable node underneath the project node at
		// all times!
		BOOL bReady = FALSE;
		int nSec = 30;
		while (!bReady && ret == SUCCESS)
		{
			// Attempt to open the property page for the first class or the Globals node.
			MST.DoKeys("{Left}{Left}{Right}{Right}");
			
			MST.DoKeys("%({Enter})");

			CString str;
			MST.WGetText(NULL, str);
			MST.DoKeys("{Escape}");

			if (str.Find(GetLocString(IDSS_WRKSPC_CLASS_PROP_PAGE)) != -1 || 
				str.Find(GetLocString(IDSS_WRKSPC_GLOBALS_PROP_PAGE)) != -1 ||
				str.Find(GetLocString(IDSS_WRKSPC_INTERFACE_PROP_PAGE)) != -1)
			{
				bReady = TRUE;
				MST.DoKeys("{Left}");	// Go back to project node.
			}
			else
			{
				if (--nSec != 0)	// If we still have some waiting time...
				{
					LOG->RecordInfo("Waiting up to %d more second(s) for project node to display classes...", nSec);
					Sleep(1000);
				}
				else				// We've timed out.
				{
					LOG->RecordFailure("Timeout waiting for project node to open.");
					ret = ERROR_ERROR;
				}
			}
		}

		if (ret == SUCCESS)	// If classes are now expanded in the ClassView...
		{
			// Expand the immediate children of this project, wait up to 30 seconds for response.
			MST.DoKeys("{Left}{Right}", FALSE, 30000);

			// Type the name of the item to find.
			MST.DoKeys(pszClassName, TRUE, 10000);

			// Popup the property page for the currently active item.
			MST.DoKeys("%({Enter})");

			CStatic wndStatic;
			if (wndStatic.Attach(FindFirstChild(MST.WGetActWnd(0), IDC_CLASSNAME)) == 0)
				ret = ERROR_NO_CLASS;
			else
			{
				CString strFoundName;
				wndStatic.GetWindowText(strFoundName);

				if (strncmp(strFoundName, pszClassName, strlen(pszClassName)))
					 ret = ERROR_NO_CLASS; 

				wndStatic.Detach();
			}

			// Dismiss the properties dialog.	
			MST.DoKeys("{Escape}");
		}
	}

	WaitForInputIdle(g_hTargetProc, 10000);

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectGlobal(LPCSTR pszGlobalName, LPCSTR pszProjectName = NULL)
// Description: Selects the given global symbol in the workspace window.  It assumes that the ClassView is currently the active view.  
// Parameter: pszGlobalName Name of global symbol to find in the workspace window.
// Parameter: type Is this a function or data global?
// Parameter: pszProjectName Name of project in which the global resides.  If NULL it assumes the current project.
// Return: SUCCESS if successful; ERROR_NO_GLOBAL if class wasn't found, or ERROR_ERROR if some other problem occurred.
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectGlobal(LPCSTR pszGlobalName, GLOBAL_TYPE type, LPCSTR pszProjectName /* = NULL */)
{
	ASSERT(pszGlobalName);
	CString strGlobalName(pszGlobalName);

	int ret = SUCCESS;

	if (pszProjectName)
		ret = SelectProject(pszProjectName);

	if (ret == SUCCESS)
	{
		// Expand the globals node of this project.  We may have to search for it since, a project could define a "Globals" class.
		MST.DoKeys("{Left}{Right}", FALSE, 30000);

		int nCountDown = 3;
		BOOL bFound = FALSE;
		while(bFound == FALSE && ret == SUCCESS)
		{
			MST.DoKeys(GetLocString(IDSS_WRKSPC_GLOBALS_ITEM_NAME));

			// Popup the property page for this globals node.
			MST.DoKeys("%({Enter})");

			if (MST.WStaticExists(GetLocString(IDSS_WRKSPC_NO_PROPERTIES)))
				bFound = TRUE;
			else
			{
				if (--nCountDown != 0)
				{
					Sleep(1000);	// Give time for property dialog to go away, if not found yet.
					LOG->RecordInfo("Trying up to %d more times to find the Globals node", nCountDown);
				}
				else
				{
					LOG->RecordFailure("Timeout trying to find globals node.");
					ret = ERROR_ERROR;
				}
			}

			// Dismiss the properties dialog.	
			MST.DoKeys("{Escape}");
		}
	}

	if (ret == SUCCESS)
	{
		// We must've found it by now, so expand it.
		MST.DoKeys("{Right}");

		// Type the name of the item to find.
		MST.DoKeys(pszGlobalName, TRUE);

		// Popup the property page for the currently active item.
		MST.DoKeys("%({Enter})");
		
		CStatic wndStatic;
		for (int i = 0; i < 5; i++)
		{
			if  (wndStatic.Attach(FindFirstChild(MST.WGetActWnd(0), type == Function? IDC_FUNCTNAME : IDC_DATANAME)))
				break;
			Sleep(200);
		}

		if (wndStatic.m_hWnd == NULL)
			ret = ERROR_ERROR;
		else
		{			
			CString strFoundName;
			wndStatic.GetWindowText(strFoundName);

			RemoveCharFromString(&strFoundName, ' ');
			RemoveCharFromString(&strGlobalName, ' ');

			if (strGlobalName.Find(strFoundName) == -1)
				 ret = ERROR_NO_GLOBAL; 

			wndStatic.Detach();
		}

		// Dismiss the properties dialog.	
		MST.DoKeys("{Escape}");
	}

	WaitForInputIdle(g_hTargetProc, 10000);

	return ret;
}
// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectClassMember(LPCSTR pszMemberName, LPCSTR pszClassName = NULL, LPCSTR pszProjectName = NULL)
// Description: Selects the given item in the workspace window.  It assumes that the ClassView is currently the active view and that the class whose member your looking for is currently selected and not expanded.  If another class is currently expanded you could get an erroneous success if the member your trying to select is in the other class as well.
// Parameter: pszMemberName Name of class member to find in the workspace window.
// Parameter: type Is this a function or data member?
// Parameter: pszClassName Name of class in which to find this member.  If NULL, the function assumes the desired class is currently selected and unexpanded.
// Parameter: pszProjectName Name of project in which the class resides.  This parameter must be supplied if pszClassName is not NULL.
// Return: SUCCESS if successful.  ERROR_NO_MEMBER if member wasn't found. Or the return code from SelectClass() or SelectProject().
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectClassMember(LPCSTR pszMemberName, MEMBER_TYPE type, LPCSTR pszClassName /*= NULL */, LPCSTR pszProjectName /* = NULL */)
{
	ASSERT(pszMemberName);
	CString strMemberName(pszMemberName);

	// Make sure that either both the classname and project name is specified or neither is.
	ASSERT(!((!!pszClassName) ^ (!!pszProjectName)));

	int ret = SUCCESS;

	if (pszClassName != NULL)
		ret = SelectClass(pszClassName, pszProjectName);

	if (ret == SUCCESS)
	{
		// Expand the children of the class.
		MST.DoKeys("{Right}");

		// Type the name of the item to find.
		MST.DoKeys(pszMemberName, TRUE);

		// Popup the property page for the currently active item.
		MST.DoKeys("%({Enter})");

		CStatic wndStatic;
		for (int i = 0; i < 5; i++)
		{
			if  (wndStatic.Attach(FindFirstChild(MST.WGetActWnd(0), type == Function? IDC_FUNCTNAME : IDC_DATANAME)))
				break;
			Sleep(200);
		}

		if (wndStatic.m_hWnd == NULL)
			ret = ERROR_ERROR;
		else
		{
			CString strFoundName;
			wndStatic.GetWindowText(strFoundName);

			RemoveCharFromString(&strFoundName, ' ');
			RemoveCharFromString(&strMemberName, ' ');

			if (strMemberName.Find(strFoundName) == -1)
				 ret = ERROR_NO_MEMBER; 

			wndStatic.Detach();
		}

		// Dismiss the properties dialog.	
		MST.DoKeys("{Escape}");
	}

	WaitForInputIdle(g_hTargetProc, 10000);

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectResource(LPCSTR pszResourceType, LPCSTR pszResourceName)
// Description: Selects the given item in the workspace window.  It sets ResourceView as currently the active view.
// Parameter: pszResourceType Type of resource.  Dialog, Accelerator, etc.  This parameter is a string in order to support user defined resources.
// Parameter: pszResourceName Name of class member to find in the workspace window.
// Parameter: pszResourceFile Name of resource file to search.
// Return: SUCCESS if successful.  ERROR_NO_RESFILE, ERROR_NO_RESOURCE.
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectResource(LPCSTR pszResourceType, LPCSTR pszResourceName, LPCSTR pszResourceFile /* = NULL */)
{
	ASSERT(pszResourceType && pszResourceName);
	
	if(ActivateResourcePane()==FALSE)
		return ERROR_NO_RESOURCE;

	int ret = SUCCESS;
	Sleep(1000); //XTRA SAFETY
	// Close all of the resources of the topmost folder.
	MST.DoKeys("{Home}{Left}{Right}", FALSE);
	Sleep(2000); //XTRA SAFETY
	WaitForInputIdle(g_hTargetProc, 10000); //wait for idle if resources are expanding

	MST.DoKeys(pszResourceType, TRUE);	// Type the name of the resource type to search.
	Sleep(2000); //XTRA SAFETY
	MST.DoKeys("{NUMPAD+}");			// Expand the resources for the given type.
	Sleep(2000); //XTRA SAFETY
	MST.DoKeys(pszResourceName, TRUE);	// Type the name of the resource.
	Sleep(2000); //extra safety
	// Popup the property page for the currently active item.
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	BOOL bvalid=prop.IsValid();

	if(prop.GetID()!=pszResourceName)
		ret = ERROR_NO_RESOURCE;	

	// Dismiss the properties dialog.	
	MST.DoKeys("{Escape}");

	return ret;
}


// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectResource(LPCSTR pszResourceType, LPCSTR pszResourceName)
// Description: Selects the given item in the workspace window.  It sets ResourceView as currently the active view.
// Parameter: pszResourceType Type of resource.  Dialog, Accelerator, etc.  This parameter is a string in order to support user defined resources.
// Parameter: pszResourceName Name of resource to find in the workspace window.
// Parameter: pszResourceFile Name of resource file to search.
// Return: SUCCESS if successful.  ERROR_NO_RESFILE, ERROR_NO_RESOURCE, ERROR_ERROR if it fails to open the property page.
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectResource2(LPCSTR pszResourceType, LPCSTR pszResourceName, LPCSTR pszResourceFile /* = NULL */)
{
	ASSERT(pszResourceType && pszResourceName);
	
	if(ActivateResourcePane()==FALSE)
		return ERROR_NO_RESOURCE;

	int ret = SUCCESS;

	// Loop through all resources of given type until it finds or fails

	CString szType = (CString)pszResourceType;
	UIControlProp prop;
	BOOL bReady = FALSE;
	int nodeNumber = 0;

	while( !bReady )
	{
		// Close all of the resources of the topmost folder.
		MST.DoKeys("{Home}{Left}{Right}", FALSE, 10000); //wait for idle if resources are expanding

		MST.DoKeys(pszResourceType, TRUE);	// Type the name of the resource type to search.
		MST.DoKeys("{Right}");			// Expand the resources for the given type.
		nodeNumber++;
		for( int j = 0; j < nodeNumber; j++ )
		{
			MST.DoKeys("{Down}", FALSE, 500);
		}

		// Popup the property page for the currently active item.
		prop = UIWB.ShowPropPage(TRUE);
		if(!prop.IsValid())
		{
			LOG->RecordInfo("The Property Page is not valid");
		}
		MST.WGetText(NULL, szType);
		if (szType.Find(pszResourceType) != -1)
		{
		 	if(prop.GetID() == pszResourceName)
				bReady = TRUE;
		}
		else
		{
			ret = ERROR_NO_RESOURCE;
			bReady = TRUE;
		}
		// Dismiss the properties dialog.	
		MST.DoKeys("{Escape}");
	}	

	return ret;
}


// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SelectFile(LPCSTR pszFilename, LPCSTR pszProjectName = NULL)
// Description: Selects the given file workspace window's FileView.  It assumes that the FileView is currently the active view.
// Parameter: pszFilename Name of file to select in the workspace window.
// Parameter: pszProjectName Name of project in which the class resides.  If NULL it assumes the current project.
// Return: SUCCESS if successful, ERROR_NO_FILE if file wasn't found, or the return code from SelectProject().
// END_HELP_COMMENT
int UIWorkspaceWindow::SelectFile(LPCSTR pszFilename, LPCSTR pszProjectName /* = NULL */)
{
	ASSERT(FALSE);	// NOT FULLY IMPLEMENTED YET!
	ASSERT(pszFilename);

	int ret = SUCCESS;

	if (pszProjectName)
		ret = SelectProject(pszProjectName);
		
	if (ret == SUCCESS)
	{
		// Fully expand the children of the selected project folder.
		MST.DoKeys("*", TRUE);

		// Type the name of the item to find.
		MST.DoKeys(pszFilename, TRUE);

		// Popup the property page for the currently active item.
		MST.DoKeys("%({Enter})");
		
		// Assume property dialog is up.  Check for any static control on that window that
		// contains the validation string.  

		ret = ERROR_NO_FILE;// control ID of static control containing filename is VPROJ_IDC_FILE_NAME	

		// Dismiss the properties dialog.	
		MST.DoKeys("{Escape}");
	}
	return ret;
}

// BEGIN_HELP_COMMENT
// Function: void UIWorkspaceWindow::RemoveCharFromString(CString *pStr, char c)
// Description: Removes all instances of the char specified from the string given
// Parameter: pStr - The string to remove the character from.
// Parameter: c - The character to remove
// END_HELP_COMMENT
void UIWorkspaceWindow::RemoveCharFromString(CString *pStr, char c)
{
	char* pBuf = new char[pStr->GetLength()+1];

	int i, j;
	for (i = j = 0; i < pStr->GetLength(); i++)
		if (pStr->GetAt(i) != c)
			pBuf[j++] = pStr->GetAt(i);

	pBuf[j] = '\0';

	*pStr = pBuf;

	delete [] pBuf;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoClassDefinition(LPCSTR pszClassName, COSource* pSrcFile = NULL, LPCSTR pszProjectName = NULL)
// Description: Navigates to definition of the given class in the workspace window.  It assumes that the ClassView is currently the active view.  It currently does not support nested classes.
// Parameter: pszClassName Name of class to find in the workspace window.
// Parameter: pSrcFile - Input: Ptr to an allocated COSource object.  Output: Object is attached to source file that was activated by the Goto.  If NULL the parameter is ignored.
// Parameter: pszProjectName Name of project in which the class resides.  If NULL it assumes the current project.
// Return: SUCCESS if successful;  ERROR_NO_DEFINITION if declaration couldn't be found, or whatever GotoDefinitionFromHere(), or SelectClass() returns.
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoClassDefinition(LPCSTR pszClassName, COSource* pSrcFile/* = NULL*/, LPCSTR pszProjectName /*= NULL*/)
{
	int ret;

	ret = SelectClass(pszClassName, pszProjectName);

	if (ret == SUCCESS)
		ret = GotoDefinitionFromHere(pszClassName);

	if (ret == SUCCESS)
	{
		if (pSrcFile != NULL)
			pSrcFile->AttachActiveEditor();
	}

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoGlobalDefinition(LPCSTR pszGlobalName, COSource* pSrcFile = NULL, LPCSTR pszProjectName = NULL)
// Description: Navigates to the definition of the given global symbol in the workspace window.  It assumes that the ClassView is currently the active view.  
// Parameter: pszGlobalName Name of global symbol to find in the workspace window.
// Parameter: type Is this a function or data global?
// Parameter: pSrcFile - Input: Ptr to an allocated COSource object.  Output: Object is attached to source file that was activated by the Goto.  If NULL, the parameter is ignored.
// Parameter: pszProjectName Name of project in which the global resides.  If NULL it assumes the current project.
// Return: SUCCESS if successful; ERROR_NO_GLOBAL if class wasn't found; Whatever SelectGlobal() or GotoDefinitionFromHere() return.
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoGlobalDefinition(LPCSTR pszGlobalName, GLOBAL_TYPE type, COSource* pSrcFile/* = NULL*/, LPCSTR pszProjectName /* = NULL */)
{
	int ret;

	ret = SelectGlobal(pszGlobalName, type, pszProjectName);

	if (ret == SUCCESS)
		ret = GotoDefinitionFromHere(pszGlobalName);

	if (ret == SUCCESS)
	{
		if (pSrcFile != NULL)
			pSrcFile->AttachActiveEditor();
	}

	return ret;

}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoClassMemberDefinition(LPCSTR pszMemberName, MEMBER_TYPE type, COSource* pSrcFile = NULL, LPCSTR pszClassName = NULL, LPCSTR pszProjectName = NULL)
// Description: Navigates to the definition of the given class member.  It assumes that the ClassView is currently the active view.  
// Parameter: pszMemberName Name of class member to find in the workspace window.
// Parameter: type Is this a function or data member?
// Parameter: pSrcFile - Input: Ptr to an allocated COSource object.  Output: Object is attached to source file that was activated by the Goto.  If NULL, the parameter is ignored.
// Parameter: pszClassName Name of class in which to find this member.  If NULL, the function assumes the desired class is currently selected and unexpanded.
// Parameter: pszProjectName Name of project in which the class resides.  This parameter must be supplied if pszClassName is not NULL.
// Return: SUCCESS if successful; ERROR_NO_MEMBER if member wasn't found. Or the return code from SelectClassMember() or GotoDefinitionFromHere().
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoClassMemberDefinition(LPCSTR pszMemberName, MEMBER_TYPE type, COSource* pSrcFile, LPCSTR pszClassName /*= NULL */, LPCSTR pszProjectName /* = NULL */)
{
	int ret;

	ret = SelectClassMember(pszMemberName, type, pszClassName, pszProjectName);

	if (ret == SUCCESS)
		ret = GotoDefinitionFromHere(pszMemberName);

	if (ret == SUCCESS)
	{
		if (pSrcFile != NULL)
			pSrcFile->AttachActiveEditor();
	}

	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoClassMemberDeclaration(LPCSTR pszMemberName, MEMBER_TYPE type, COSource* pSrcFile = NULL, LPCSTR pszClassName = NULL, LPCSTR pszProjectName = NULL)
// Description: Navigates to the declaration of the given class member.  It assumes that the ClassView is currently the active view.  
// Parameter: pszMemberName Name of class member to find in the workspace window.
// Parameter: type Is this a function or data member?
// Parameter: pSrcFile - Input: Ptr to an allocated COSource object.  Output: Object is attached to source file that was activated by the Goto.  If NULL, the parameter is ignored.
// Parameter: pszClassName Name of class in which to find this member.  If NULL, the function assumes the desired class is currently selected and unexpanded.
// Parameter: pszProjectName Name of project in which the class resides.  This parameter must be supplied if pszClassName is not NULL.
// Return: SUCCESS if successful. ERROR_NO_MEMBER if member wasn't found. Or the return code from GotoDeclarationFromHere() or SelectClassMember().
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoClassMemberDeclaration(LPCSTR pszMemberName, MEMBER_TYPE type, COSource* pSrcFile, LPCSTR pszClassName /*= NULL */, LPCSTR pszProjectName /* = NULL */)
{
	int ret;

	ret = SelectClass(pszClassName, pszProjectName);
	if (ret == SUCCESS)
		ret = GotoDeclarationFromHere(pszMemberName);

	if (ret == SUCCESS)
	{
		if (pSrcFile != NULL)
			pSrcFile->AttachActiveEditor();
	}
		
	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoDefinitionFromHere(LPCSTR pszName)
// Description: Navigates to the definition of the currently selected item in the workspace window.  It assumes that the ClassView is currently the active view.  This function is useful for calling right after successfully calling one of the SelectXXX functions.
// Parameter: pszName Name that should appear in source line navigated to.
// Return: SUCCESS if successful; ERROR_GOTO if definition wasn't gone to; ERROR_NO_DEFINITION if definition couldn't be found.
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoDefinitionFromHere(LPCSTR pszName)
{
	static BOOL bFirst = 1;	// Need to maximize a MDI Child window the first time
	if(bFirst)
	{
		COSource JunkFile;	// Open text file that will be maximized and then closed.
		JunkFile.Create();
		UIWB.DoCommand(MENU_FILE, DC_MENU);
		MST.DoKeys("{esc}");
		MST.DoKeys("{left}{down 5}{enter}");
		JunkFile.Close();
		UIWB.DoCommand(IDM_WINDOW_ERRORS, DC_MNEMONIC);		// Open Output window
		UIWB.DoCommand(IDM_VIEW_WORKSPACE, DC_MNEMONIC);	// Set focus back to workspace window
		bFirst = 0;
	}
	
	CString szCaption, szTestCaption ;
	MST.WGetText(NULL, szCaption) ;			// Get current MDI Main Frame caption for compare
	int ret = SUCCESS;
	MST.DoKeys("+({F10})");		// Bring up the popup context menu.

	CString strGotoMenuItem = GetLocString(IDSS_WRKSPC_GOTO_DEFINITION);

	if (!MST.WMenuExists(strGotoMenuItem))
		ret = ERROR_NO_DEFINITION;

	MST.WMenu(strGotoMenuItem);
		
	WaitForInputIdle(g_hTargetProc, 5000);	// Wait for processing to end.
	// Sleep(5000);				// Wait for message box to arrive.

	int nCount = 0;
	MST.WGetText(NULL, szTestCaption);		// Test MDI Main Frame caption until it changes
	while (szCaption == szTestCaption && nCount < 7)
	{
		LOG->RecordInfo("Trying up to %d more times to find the Editor", nCount);
		LOG->RecordInfo("Current caption is %s and original caption is %s ", szTestCaption.GetBuffer(szTestCaption.GetLength()), szCaption.GetBuffer(szCaption.GetLength()));
		Sleep(1000);
		nCount += 1;
		MST.WGetText(NULL, szTestCaption);
	}

	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())			// If we can attach, then an error must've occurred.
	{
		ret = ERROR_NO_DEFINITION;
		wndMsgBox.ButtonClick();
	}
	else									// A source window must've become active.
	{
		COSource SrcFile;
		
		SrcFile.AttachActiveEditor();

		// Select to end of line.
		MST.DoKeys("+({End})");

		CString strFound = SrcFile.GetSelectedText();
		CString strBase = pszName;

		// Remove spaces from both the source and target strings for comparison.
		RemoveCharFromString(&strBase, ' ');
		RemoveCharFromString(&strFound, ' ');

		if (strFound.Find(strBase) == -1)
		{
			TRACE("UIWorkspaceWindow::GotoDefinitionFromHere() Found: %s\n", (LPCSTR)strFound);
			TRACE("UIWorkspaceWindow::GotoDefinitionFromHere() Looking for: %s\n", (LPCSTR)strBase);
			ret = ERROR_GOTO;
		}
	}
	
	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::GotoDeclarationFromHere(LPCSTR pszName)
// Description: Navigates to the declaration of the currently selected item in the workspace window.  It assumes that the ClassView is currently the active view.  This function is useful for calling right after successfully calling one of the SelectXXX functions.
// Parameter: pszName Name that should appear in source line navigated to.
// Return: SUCCESS if successful;  ERROR_GOTO if definition wasn't gone to; ERROR_NO_DECLARATION if declaration couldn't be found.
// END_HELP_COMMENT
int UIWorkspaceWindow::GotoDeclarationFromHere(LPCSTR pszName)
{
	int ret = SUCCESS;
	MST.DoKeys("+({F10})");		// Bring up the popup context menu.

	CString strGotoMenuItem = GetLocString(IDSS_WRKSPC_GOTO_DECLARATION);

	/* GAC looking for non-existant menu breaks javasys sniff
	if (!MST.WMenuExists(strGotoMenuItem))
		ret = ERROR_NO_DECLARATION;
	*/

	MST.WMenu(strGotoMenuItem);

	WaitForInputIdle(g_hTargetProc, 5000);
	Sleep(1000) ;
	UIMessageBox wndMsgBox;
	if (wndMsgBox.AttachActive())
	{
		ret = ERROR_NO_DECLARATION;
		wndMsgBox.ButtonClick();
	}
	else
	{
		COSource SrcFile;
		
		SrcFile.AttachActiveEditor();

		// Select to end of line.
		MST.DoKeys("+({End})");

		CString strFound = SrcFile.GetSelectedText();
		CString strBase = pszName;

		// Remove spaces from both the source and target strings for comparison.
		RemoveCharFromString(&strBase, ' ');
		RemoveCharFromString(&strFound, ' ');

		if (strFound.Find(strBase) == -1)
		{
			TRACE("UIWorkspaceWindow::GotoDeclarationFromHere() Found: %s\n", (LPCSTR)strFound);
			TRACE("UIWorkspaceWindow::GotoDeclarationFromHere() Looking for: %s\n", (LPCSTR)strBase);
			ret = ERROR_GOTO;
		}
	}
	
	return ret;
}

// BEGIN_HELP_COMMENT
// Function: int UIWorkspaceWindow::SetActiveProject(LPCSTR projName)
// Description: Sets the active project inside the workspace 
// Return: none
// END_HELP_COMMENT
BOOL UIWorkspaceWindow::SetActiveProject(LPCSTR projName)
{
	ASSERT(projName);
	if(ActivateBuildPane()==FALSE)
		return FALSE;

	int ret = TRUE;
	// Close all tree and expands projects only
	MST.DoKeys("{Home}{Left}{Right}", FALSE, 10000); //wait for idle
	MST.DoKeys(projName, TRUE);	// Type the name of the project to search.
	Sleep(500); //xtra safety
	MST.DoKeys("+{F10}");
	Sleep(1000); //xtra safety
	MST.DoKeys("v", FALSE, 1000);
	return ret;
} // SetActiveProject
