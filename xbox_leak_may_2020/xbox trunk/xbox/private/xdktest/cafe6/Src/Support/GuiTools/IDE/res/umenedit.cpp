///////////////////////////////////////////////////////////////////////////////
//	UMENEDIT.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIMenEdit class
//

#include "stdafx.h"
#include "umenedit.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIMenEdit::IsValid(void) const
// Description: Determine whether the menu editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the title is correct.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIMenEdit::IsValid(void) const
{
	return UIResEditor::IsValid() && IsValidType(m_pszTypeText[ED_MENU]);
}

// BEGIN_HELP_COMMENT
// Function: HWND UIMenEdit::GetMenuBar(void)
// Description: Get the HWND to the menu bar in the menu bar editor.
// Return: The HWND of the menu bar in the menu bar editor.
// END_HELP_COMMENT
HWND UIMenEdit::GetMenuBar(void)
{
	HWND hwndChild = GetWindow(HWnd(), GW_CHILD);
    HWND hwndCanvas = GetDlgItem(hwndChild, -1 );

	return GetDlgItem(hwndCanvas, 200 );
}
	  
