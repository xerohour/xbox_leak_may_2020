///////////////////////////////////////////////////////////////////////////////
//  UPRJFILE.CPP
//
//  Created by :            Date :
//      MichMa              12/01/93
//
//  Description :
//      Implementation of the UIProjectFiles class
//

#include "stdafx.h"
#include "uprjfile.h"
#include "mstwrap.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: BOOL UIProjectFiles::AddFile(LPCSTR szFileName)
// Description: Add a file to the current project via the Add Files dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFileName A pointer to a string that contains the name of the file to add.
// END_HELP_COMMENT
BOOL UIProjectFiles::AddFile(LPCSTR szFileName)
	{
	MST.WEditSetText(UIPF_EB_FILENAME, szFileName); 
	MST.WButtonClick(UIPF_BTN_ADD);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIProjectFiles::AddAllFiles(LPCSTR szType /* UIPF_FOT_SOURCE */, LPCSTR szProjFilesPath /* NULL */)   
// Description: Add all files (filtered by the given filter) in the directory specified to the current project via the Add Files dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szType A pointer to a string that contains the types of files to filter. This string is one of the strings found in the Files of Type combo box in the Add Files dialog.
// Param: szProjFilesPath A pointer to a string that contains the path to the directory that contains the files to be added.
// END_HELP_COMMENT
BOOL UIProjectFiles::AddAllFiles(LPCSTR szType /* UIPF_FOT_SOURCE */, LPCSTR szProjFilesPath /* NULL */)   
	
	{
	char szCurDir[256];
	
	if(szProjFilesPath)
		{
		GetCurrentDirectory(256, szCurDir);
		MST.WEditSetText(UIPF_EB_FILENAME, szProjFilesPath);
		MST.WEditSetFocus(UIPF_EB_FILENAME);
		MST.DoKeys("{ENTER}");
		}
	
	MST.WComboItemClk(UIPF_CB_FILES_OF_TYPE, szType);
	MST.WButtonClick(UIPF_BTN_ADD_ALL);

 	if(szProjFilesPath)
		{
		MST.WEditSetText(UIPF_EB_FILENAME, szCurDir);
		MST.WEditSetFocus(UIPF_EB_FILENAME);
		MST.DoKeys("{ENTER}");
		}

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: HWND UIProjectFiles::Close(void)
// Description: Close the Add Files dialog.
// Return: NULL if successful; the HWND of the Add Files dialog otherwise.
// END_HELP_COMMENT
HWND UIProjectFiles::Close(void)
{
	HWND hwnd = UIDialog::Close();
	MST.WFndWndWait("Scan Dependencies", FW_DEFAULT, 2);
	while (MST.WFndWnd("Scan Dependencies", FW_DEFAULT));  //TODO: get rid of literals.
	return hwnd;
}
