///////////////////////////////////////////////////////////////////////////////
//	UFNDINF.CPP
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Implementation of the UIFindInFilesDlg class
//

#include "stdafx.h"
#include "ufindinf.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\vcpp32.h"
#include "guiv1.h"
#include "..\shl\uioutput.h"
#include "..\shl\uwbframe.h"

// this dialog is in sushi\vcpp32's RC file (MSVC.RC)

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: void UIFindInFilesDlg::AddFile(LPCSTR szFile)
// Description: Add a file to the list of selected files to search in the Selected Files list box.
// Return: none
// Param: szFile A pointer to a string that contains the filename of the file to add to the list of files to search. This is usually a full path to the file.
// END_HELP_COMMENT
void UIFindInFilesDlg::AddFile(LPCSTR szFile)
{
	ExpectValid();
	MST.WEditSetText(GetLabel(1152), szFile);	// REVIEW: what if szFile contains '*' or '?'
	MST.WButtonClick(GetLabel(VCPP32_DLG_GREP_ADD));		// we could Add All in that case
}

// BEGIN_HELP_COMMENT
// Function: void UIFindInFilesDlg::FindWhat(LPCSTR szFind)
// Description: Set the text to search for in the list of files in the Find What edit box.
// Return: none
// Param: szFind A pointer to a string containing the text to search for in the list of files.
// END_HELP_COMMENT
void UIFindInFilesDlg::FindWhat(LPCSTR szFind)
{
	ExpectValid();
	MST.WComboSetText(GetLabel(VCPP32_DLG_GREP_FINDWHAT), szFind);
}

// BEGIN_HELP_COMMENT
// Function: void UIFindInFilesDlg::DoFind(void)
// Description: Perform the find in files. Call this function after setting the list of files with UIFindInFilesDlg::AddFile and the text to search for with UIFindInFilesDlg::FindWhat.
// Return: none
// END_HELP_COMMENT
void UIFindInFilesDlg::DoFind(void)
{
	ExpectValid();
	// do the find
	MST.WButtonClick(GetLabel(IDOK));

	WaitUntilGone(10000);
	EXPECT( UIWB.IsActive() );

	UIOutput uo = UIWB.ShowDockWindow(IDW_OUTPUT_WIN);
	int nSeconds = 0;
	while(nSeconds++ < 30)
	{
		DoKeys("^{end}+{up}^c");	// select last line of file, and copy it
		CString str;
		GetClipText(str);
		if( (str.Find(GetLocString(IDSS_FIF_OCCURANCES)) >= 0) ||		// "occurrence(s) have been found."
			(str.Find(GetLocString(IDSS_FIF_NOTFOUND)) >= 0) )			// "' was not found."
			break;		// the search is done when one of these lines appears at the bottom
		LOG->Comment((LPCSTR)str);
		Sleep(1000);
	}
}

