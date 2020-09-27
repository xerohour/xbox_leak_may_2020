///////////////////////////////////////////////////////////////////////////////
//	URESBRWS.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIResBrowser class
//

#include "stdafx.h"
#include "uresbrws.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIResBrowser::IsValid(void) const
// Description: Determine whether the resource browser is valid by determining if the window is valid, visible, the window contains a list box, and it's not a string table editor or accelerator editor.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIResBrowser::IsValid(void) const
{
	if( !UIEditor::IsValid() )	// bypass UIResEditor and UIResTable
		return FALSE;

	return (GetTitle().Find('(') < 0) &&	// has listbox, and isn't (Stringtable) or (Accelerator)
			(GetDlgControlClass(m_hwndListbox) == GL_LISTBOX);
}

// BEGIN_HELP_COMMENT
// Function: int UIResBrowser::SelectFolder(int nFolder)
// Description: Select a folder in the resource browser by folder type (see also SelectFolder(LPCSTR)).
// Return: An integer that contains the line of the selected folder in the resource browser.
// Param: nFolder A value that contains the folder to select in the resource browser: ED_DIALOG, ED_MENU, ED_CURSOR, ED_ICON, ED_BITMAP, ED_STRING, ED_ACCEL, ED_VERSION, ED_BINARY.
// END_HELP_COMMENT
int UIResBrowser::SelectFolder(int nFolder)
{
	ASSERT( (nFolder >= ED_DIALOG) && (nFolder <= ED_VERSION) );
	LPCSTR sz = m_pszTypeText[nFolder] + 1;
	CString str(sz, strlen(sz) - 1);	// string minus parens
	return SelectFolder(str);
}

// BEGIN_HELP_COMMENT
// Function: int UIResBrowser::SelectFolder(LPCSTR szFolder)
// Description: Select a folder in the resource browser by name (see also SelectFolder(int)).
// Return: An integer that contains the line of the selected folder in the resource browser.
// Param: szFolder A pointer to a string that contains the name of the folder to select.
// END_HELP_COMMENT
int UIResBrowser::SelectFolder(LPCSTR szFolder)
{
	if( !IsActive() && !Activate() )
	{
		LOG->RecordInfo("Could not activate resource browser");
		return -1;
	}

	// TODO:
	// shortcut: Close/Open folder at top level to list only sub-folders
	// Select each line in the list
	// If it is a folder (id == ?)
	// and it is the right folder (caption == ?)
	// then stop

	if( !MST.WListItemExists(NULL, szFolder) )	// list doesn't have strings
		return -1;
	MST.WListItemClk(NULL, szFolder);
	return GetCurLine();
}

// BEGIN_HELP_COMMENT
// Function: int UIResBrowser::OpenFolder(int nOpen)
// Description: Open or close the selected folder in the resource editor.
// Return: An integer containing the number of items in the folder. Positive if the folder is opened; negative if the folder is closed.
// Param: nOpen A value that indicates the action to perform on the folder: CLOSE_FOLDER, OPEN_FOLDER, OPEN_FOLDER_ALL.
// END_HELP_COMMENT
int UIResBrowser::OpenFolder(int nOpen)
{
	if( !IsActive() && !Activate() )
	{
		LOG->RecordInfo("Could not activate resource browser");
		return 0;
	}
	int n = GetLineCount();
	if( nOpen == CLOSE_FOLDER )	// or FALSE
		MST.DoKeys("-", TRUE);
	else if( nOpen == OPEN_FOLDER_ALL )
		MST.DoKeys("*", TRUE);
	else						// OPEN_FOLDER or TRUE
		MST.DoKeys("+", TRUE);
	return GetLineCount() - n;	// return number of items in folder
}

// BEGIN_HELP_COMMENT
// Function: int UIResBrowser::FindResource(LPCSTR szId, int nFolder)
// Description: Select an item within a folder (by folder type) in the resource browser (see also FindResource (LPCSTR, LPCSTR)).
// Return: An integer that contains the line of the selected item in the resource browser.
// Param: szId A pointer to a string that contains the name of the item to select within the given folder.
// Param: nFolder A value that contains the folder to select in the resource browser: ED_DIALOG, ED_MENU, ED_CURSOR, ED_ICON, ED_BITMAP, ED_STRING, ED_ACCEL, ED_VERSION, ED_BINARY.
// END_HELP_COMMENT
int UIResBrowser::FindResource(LPCSTR szId, int nFolder)
{
	ASSERT( (nFolder >= ED_DIALOG) && (nFolder <= ED_VERSION) );
	LPCSTR sz = m_pszTypeText[nFolder] + 1;
	CString str(sz, strlen(sz) - 1);	// string minus parens
	return FindResource(szId, str);
}

// BEGIN_HELP_COMMENT
// Function: int UIResBrowser::FindResource(LPCSTR szId, LPCSTR szFolder /*=NULL*/)
// Description: Select an item within a folder (by folder name) in the resource browser (see also FindResource (LPCSTR, int)).
// Return: An integer that contains the line of the selected item in the resource browser.
// Param: szId A pointer to a string that contains the name of the item to select within the given folder.
// Param: szFolder A pointer to a string that contains the name of the folder to select.
// END_HELP_COMMENT
int UIResBrowser::FindResource(LPCSTR szId, LPCSTR szFolder /*=NULL*/)
{
	if( !IsActive() && !Activate() )
	{
		LOG->RecordInfo("Could not activate resource browser");
		return -1;
	}

	if( szFolder != NULL )
	{
		SelectFolder(szFolder);
		OpenFolder(TRUE);
	}
	if( !MST.WListItemExists(NULL, szId) )
		return -1;
	MST.WListItemClk(NULL, szId);
	return GetCurLine();
}
