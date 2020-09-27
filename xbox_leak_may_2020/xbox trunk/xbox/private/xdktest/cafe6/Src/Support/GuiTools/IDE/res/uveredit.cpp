///////////////////////////////////////////////////////////////////////////////
//	UVEREDIT.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIVerEdit class
//

#include "stdafx.h"
#include "uveredit.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIVerEdit::IsValid(void) const
// Description: Determine whether the version editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the title is correct.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIVerEdit::IsValid(void) const
{
	return UIResEditor::IsValid() && IsValidType(m_pszTypeText[ED_VERSION]);
}

// Add Utilities here
