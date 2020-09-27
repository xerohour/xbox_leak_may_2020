///////////////////////////////////////////////////////////////////////////////
//	URESEDIT.CPP
//
//	Created by :			Date :
//		DavidGa					9/25/93
//
//	Description :
//		Implementation of the UIResEditor class
//

#include "stdafx.h"
#include "uresedit.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIResEditor::IsValid(void) const
// Description: Determine whether the resource editor is valid by determining if the window is valid, visible, and the Resource Symbols menu item is enabled.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIResEditor::IsValid(void) const
{
	if( !UIEditor::IsValid() )
		return FALSE;

	return UIWB.IsCommandEnabled(IDM_EDIT_SYMBOLS);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIResEditor::IsValidType(LPCSTR szType) const
// Description: Determine whether the resource editor is the type specified.
// Return: A Boolean value that indicates whether the resource editor is the type specified. TRUE if it is; FALSE otherwise.
// Param: szType A pointer to a string that contains the type of resource editor expected.
// END_HELP_COMMENT
BOOL UIResEditor::IsValidType(LPCSTR szType) const
{
	CString strTitle = GetText();
	if( szType != NULL )
		return strTitle.Find(szType) >= 0;
	else
	{
		for( int n = 0; m_pszTypeText[n] != NULL; n++ )
		{
			if( strTitle.Find(m_pszTypeText[n]) >= 0 )
				return TRUE;
		}
		return FALSE;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIResEditor::ExportResource(LPCSTR szFileName)
// Description: Export a resource.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFileName A pointer to a string that contains the filename to export the resource to. This parameter usually contains a full path to the file.
// END_HELP_COMMENT
BOOL UIResEditor::ExportResource(LPCSTR szFileName)
{
	// TODO: Make sure this editor has focus FIRST
	UIWB.ExportResource(szFileName);
	return TRUE;
}

LPCSTR aszTypes[] =
{
	"(Dialog)",		// must be in same order as above enum in header file!
	"(Menu)",
	"(Cursor)",
	"(Icon)",
	"(Bitmap)",
	"(String Table)",
	"(Accelerator)",
	"(Version)",
	"(\"",			// binary files are anything with a quoted name in parens
	NULL	// must be last item in array
};

LPCSTR* UIResEditor::m_pszTypeText = aszTypes;
