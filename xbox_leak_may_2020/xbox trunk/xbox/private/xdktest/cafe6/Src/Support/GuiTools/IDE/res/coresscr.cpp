///////////////////////////////////////////////////////////////////////////////
//	CORESSCR.CPP
//
//	Created by :			Date :
//		EnriqueP					1/24/94
//
//	Description :
//		Implementation of the COResScript component object class
//

#include "stdafx.h"
#include "coresscr.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vres.h"
#include "guiv1.h"
#include "guitarg.h"
#include "..\shl\uwbframe.h"
#include "Strings.h"
#include "..\shl\wbutil.h"
#include "..\shl\upropwnd.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

COResScript::COResScript()
{
 
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::Create(LPCSTR szSaveAs /*=NULL*/)
// Description: Create a new resource script in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szSaveAs A pointer to a string that contains the name to save the new file with. NULL indicates don't save (no name specified). (The default value is NULL.)
// END_HELP_COMMENT
int COResScript::Create(LPCSTR szSaveAs /*=NULL*/)
{
	return COFile::Create(IDSS_NEW_RC, szSaveAs);		
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/)
// Description: Open a resource script file and its editor in the IDE.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szFileName A pointer to a string that contains the name of the file to open.
// Param: szChDir A pointer to a string that contains the path to the file. NULL if szFileName includes a path or szFileName is in the current directory). (The default value is NULL.)
// END_HELP_COMMENT
int COResScript::Open(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/)
{
	return COFile::Open(szFileName, szChDir, OA_AUTO);
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::CreateResource(UINT idResType)
// Description: Add a resource to this resource script.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: idResType An value that contains the type of resource to add: IDSS_RT_ACCELERATOR, IDSS_RT_BINARY, IDSS_RT_BITMAP, IDSS_RT_CURSOR, IDSS_RT_DIALOG, IDSS_RT_ICON, IDSS_RT_MENU, IDSS_RT_STRING_TABLE, IDSS_RT_VERSION.
// END_HELP_COMMENT
int COResScript::CreateResource( UINT idResType )	   // This one uses the string table
{
	const char* const THIS_FUNCTION = "COResScript::CreateResource";

	CString strResType = GetLocString(idResType);

	UIWB.DoCommand(IDM_RESOURCE_NEW, DC_MNEMONIC);
	Sleep(1000) ; // Wait a little for the dialog to be displayed.
	MST.WButtonClick(GetLabel(VRES_IDC_CUSTOMTYPE));
	
	UIDialog nrd(VRES_IDC_TYPE_NAME, GL_EDIT);
	if( !nrd.WaitAttachActive(2000) )
	{
		LOG->RecordInfo("%s: could not open Resource/New dialog", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	MST.WEditSetText(GetLabel(VRES_IDC_TYPE_NAME), strResType);

	if (!MST.WButtonEnabled(GetLabel(IDOK)))  // Timeout supposedly defaults to 5 sec.
	{
		LOG->RecordFailure("%s: timeout waiting for OK button on New Resource Dialog to become enabled.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	nrd.OK();
	nrd.WaitUntilGone(2000);

	WaitForInputIdle(g_hTargetProc, 10000);

	// verify the resource editor window came up
	BOOL bFound = FALSE;
	strResType.MakeUpper();	// We need to compare using matching case.
	for (int i = 0; i < 5; i++) {
		if (MST.WFndWndWait(strResType, FW_PART, 2) != 0){
		bFound = TRUE;
		break;
		}

	/*	HWND hwndEd = UIWB.GetActiveEditor();
    	CString title = ::GetText(hwndEd);
 		title.MakeUpper();
		if (title.Find(strResType) >= 0) {
			bFound = TRUE;} */
			
		
		Sleep(500);
	}

	if (!bFound) {
		LOG->RecordInfo("%s: did not find resource editor window '%s'. Could not create resource editor.", THIS_FUNCTION, (LPCSTR)strResType);
		return ERROR_ERROR;
	}

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::CreateResource( LPCSTR szResName )
// Description: Add a resource to this resource script. This function, since it takes the name of the type of resource as a pointer to a string, is generally used for custom resource types.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szResName A pointer to a string that contains the name of the resource to add.
// END_HELP_COMMENT
int COResScript::CreateResource( LPCSTR szResName )		// This one is generally used for custom resource types
{
	UIWB.DoCommand(IDM_RESOURCE_NEW, DC_MNEMONIC);
	if( MST.WFndWndWait(GetLocString(IDSS_NEW_RES_TITLE), FW_PART, 2) == 0 )	// make sure the dialog comes up
		return ERROR_ERROR;		
	MST.WButtonClick(GetLabel(VRES_IDC_CUSTOMTYPE));
        Sleep(2000);
	MST.DoKeys(szResName);				// Type Name of resource .... 

	if (!MST.WButtonEnabled(GetLabel(IDOK)))  // Timeout supposedly defaults to 5 sec.
	{
		LOG->RecordFailure("Timeout waiting for OK button on New Resource Dialog to become enabled.");
		return ERROR_ERROR;
	}

	MST.WButtonClick(GetLabel(IDOK));

	if( MST.WFndWndWait(szResName, FW_PART, 2)  == 0 ) // Wait for the editor to come up and verify the Caption
		return ERROR_ERROR;

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::CreateResource(UINT idResType, UIResEditor& resEditor)
// Description: Add a resource to this resource script.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: idResType An value that contains the type of resource to add: IDSS_RT_ACCELERATOR, IDSS_RT_BINARY, IDSS_RT_BITMAP, IDSS_RT_CURSOR, IDSS_RT_DIALOG, IDSS_RT_ICON, IDSS_RT_MENU, IDSS_RT_STRING_TABLE, IDSS_RT_VERSION.
// Param: resEditor A reference to a UIResEditor object that will contain the resource editor created.
// END_HELP_COMMENT
int COResScript::CreateResource(UINT idResType, UIResEditor& resEditor)
{
	const char* const THIS_FUNCTION = "COResScript::CreateResource";

	// create the resource
	if (CreateResource(idResType) == ERROR_SUCCESS) {
		resEditor = UIWB.GetActiveEditor();
		if (resEditor.IsValid()) {
			return ERROR_SUCCESS;
		}
		else {
			LOG->RecordInfo("%s: resource editor '%s' is not valid (UIResEditor::IsValid failed).", THIS_FUNCTION, resEditor.GetTitle());
		}
	}
	else {
		LOG->RecordInfo("%s: cannot create resource editor %d (COResScript::CreateResource failed).", THIS_FUNCTION, idResType);
	}

	return ERROR_ERROR;
}


// BEGIN_HELP_COMMENT
// Function: int COResScript::OpenResource(UINT idResType, LPCSTR szResName, UIResEditor& resEditor)
// Description: Open a resource.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: idResType An value that contains the type of resource: IDSS_RT_ACCELERATOR, IDSS_RT_BINARY, IDSS_RT_BITMAP, IDSS_RT_CURSOR, IDSS_RT_DIALOG, IDSS_RT_ICON, IDSS_RT_MENU, IDSS_RT_STRING_TABLE, IDSS_RT_VERSION.
// Param: szResName A pointer to a string that contains the name of the resource.
// Param: resEditor A reference to a UIResEditor object that will contain the resource editor created.
// END_HELP_COMMENT
int COResScript::OpenResource(UINT idResType, LPCSTR szResName, UIResEditor& resEditor)
{
	const char* const THIS_FUNCTION = "COResScript::OpenResource";

	CString strResType = GetLocString(idResType);
	UIWorkspaceWindow uiwksp;

	if( !uiwksp.ActivateResourcePane() )
	{
		LOG->RecordInfo("%s: cannot activate resource pane (UIWorkspaceWindow::ActivateResourcePane failed).", THIS_FUNCTION);
	}

	// create the resource
	if (uiwksp.SelectResource(strResType, szResName) == SUCCESS)
	{
		MST.DoKeys("{Enter}");
		resEditor = UIWB.GetActiveEditor();
		if (resEditor.IsValid()) 
		{
			return ERROR_SUCCESS;
		}
		else
		{
			LOG->RecordInfo("%s: resource editor '%s' is not valid (UIResEditor::IsValid failed).", THIS_FUNCTION, resEditor.GetTitle());
		}
	}
	else
	{
		LOG->RecordInfo("%s: cannot select resource %d (UIWorkspaceWindow::SelectResource failed).", THIS_FUNCTION, idResType);
	}

	return ERROR_ERROR;
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::AddMenu(LPCSTR szMenuName, LPCSTR szMenuItem, UIResEditor& resEditor)
// Description: Add menu to the end of menu bar of opened Menu resource and also add the first menu item to this menu.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szMenuName A pointer to a string that contains the name of the new menu.
// Param: szMenuItem A pointer to a string that contains the name of the new first menu item.
// Param: resEditor A reference to a UIResEditor object that will contain the resource editor created.
// END_HELP_COMMENT
int COResScript::AddMenu(LPCSTR szMenuName, LPCSTR szMenuItem, UIResEditor& resEditor)
{
	const char* const THIS_FUNCTION = "COResScript::AddMenu";

	resEditor = UIWB.GetActiveEditor();
	if (!resEditor.IsValid()) 
	{
		LOG->RecordInfo("%s: resource editor '%s' is not valid (UIResEditor::IsValid failed).", THIS_FUNCTION, resEditor.GetTitle());
		return ERROR_ERROR;
	}

	MST.DoKeys("+{Tab}");	 // Get to the rigt-most menu placeholder
	MST.DoKeys(szMenuName);	 // Type in the Menu name
	MST.DoKeys("{Enter}");	 
	MST.DoKeys(szMenuItem);	 // Type in the Menu item name
	MST.DoKeys("{Enter}");	 
	MST.DoKeys("{Up}");	 

	UIControlProp prop;

	prop = UIWB.ShowPropPage(TRUE);
	if( !prop.IsValid() )
	{
		LOG->RecordInfo("%s: cannot validate property page (UIControlProp::IsValid failed).", THIS_FUNCTION);
	}
	if( !prop.SetID((CString)"ID_" + szMenuName + "_" + szMenuItem) )
	{
		LOG->RecordInfo("%s: could not set the menu item ID.", THIS_FUNCTION);
		return ERROR_ERROR;
	}

	// Dismiss the properties dialog.	
	MST.DoKeys("{Escape}");

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::ChangeDialogCaption( LPCSTR szDialogCaption, UIResEditor& resEditor)
// Description: Change the Dialog Caption; assume that the dialog resource is opened at the time
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szDialogCaption A pointer to a string that contains the dialog caption.
// Param: resEditor A reference to a UIResEditor object that will contain the resource editor created.
// END_HELP_COMMENT
int COResScript::ChangeDialogCaption( LPCSTR szDialogCaption, UIResEditor& resEditor)
{
	const char* const THIS_FUNCTION = "COResScript::ChangeDialogCaption";

	resEditor = UIWB.GetActiveEditor();
	if (!resEditor.IsValid()) 
	{
		LOG->RecordInfo("%s: resource editor '%s' is not valid (UIResEditor::IsValid failed).", THIS_FUNCTION, resEditor.GetTitle());
		return ERROR_ERROR;
	}

	MST.DoKeys(szDialogCaption);	 // Type in the Dialog caption
	MST.DoKeys("{Enter}");	 

	return ERROR_SUCCESS;
}

// BEGIN_HELP_COMMENT
// Function: int COResScript::ChangeDialogID( LPCSTR szDialogID, LPCSTR szOldDialogID)
// Description: Change the Dialog ID
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szNewDialogID A pointer to a string that contains the new dialog ID.
// Param: szOldDialogID A pointer to a string that contains the old dialog ID.
// END_HELP_COMMENT
int COResScript::ChangeDialogID( LPCSTR szNewDialogID, LPCSTR szOldDialogID)
{

	const char* const THIS_FUNCTION = "COResScript::ChangeDialogID";

//	CString strResType = GetLocString(IDSS_RT_DIALOG);
	UIWorkspaceWindow uiwksp;
	UIControlProp prop;

	if( !uiwksp.ActivateResourcePane() )
	{
		LOG->RecordInfo("%s: cannot activate resource pane (UIWorkspaceWindow::ActivateResourcePane failed).", THIS_FUNCTION);
	}

	if (uiwksp.SelectResource(GetLocString(IDSS_RT_DIALOG), szOldDialogID) == SUCCESS)
	{
		prop = UIWB.ShowPropPage(TRUE);
		if( !prop.IsValid() )
		{
			LOG->RecordInfo("%s: cannot validate property page (UIControlProp::IsValid failed).", THIS_FUNCTION);
		}
		if( !prop.SetID(szNewDialogID) )
		{
			LOG->RecordInfo("%s: could not set the new Dialog ID.", THIS_FUNCTION);
			return ERROR_ERROR;

		}

		// Dismiss the properties dialog.	
		MST.DoKeys("{Escape}");

	}
	else
	{
		LOG->RecordInfo("%s: cannot select resource %s (UIWorkspaceWindow::SelectResource failed).", THIS_FUNCTION, szOldDialogID);
	}

	return ERROR_SUCCESS;
}
