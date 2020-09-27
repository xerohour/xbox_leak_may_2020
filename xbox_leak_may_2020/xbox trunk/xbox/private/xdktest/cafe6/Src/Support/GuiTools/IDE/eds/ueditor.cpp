///////////////////////////////////////////////////////////////////////////////
//	UEDITOR.CPP
//
//	Created by :			Date :
//		DavidGa					9/25/93
//
//	Description :
//		Implementation of the UIEditor class
//

#include "stdafx.h"
#include "ueditor.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\shl\uiwbmsg.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIEditor::AttachActive(void)
// Description: Initialize this object by attaching to the active editor window.
// Return: A Boolean value that indicates success. TRUE if successfully attached to the active editor window; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIEditor::AttachActive(void)
{
	return Attach(UIWB.GetActiveEditor());
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIEditor::IsActive(void)
// Description: Determine if this editor window is active.
// Return: A Boolean value that indicates success. TRUE if the editor window is active; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIEditor::IsActive(void)
{
	return UIWB.IsActive() && (UIWB.GetActiveEditor() == HWnd());
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIEditor::Activate(void)
// Description: Activate this editor window by giving the window the focus.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIEditor::Activate(void)
{
	return UIWB.SetActiveEditor(HWnd());
}

// BEGIN_HELP_COMMENT
// Function: CString UIEditor::GetTitle(void) const
// Description: Get the title of this editor window.
// Return: A CString containing the title of this window.
// END_HELP_COMMENT
CString UIEditor::GetTitle(void) const
{
	if( IsZoomed(HWnd()) )		// return portion of main caption
	{							// that would have been the editor's caption
		CString str = UIWB.GetText();
		int nLeftBracket = str.ReverseFind( '[' );
		EXPECT( -1 != nLeftBracket );
		EXPECT( str[str.GetLength()-1] == ']' );
		str = str.Mid( nLeftBracket+1 );
		return str.Left( str.GetLength()-1 );
	}
	else
	{
		return GetText();
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIEditor::Close(BOOL bLoseChanges /*=TRUE*/)
// Description: Close this editor window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: bLoseChanges A Boolean value that indicates whether to lose changes (TRUE) or save them (FALSE). (The default value is TRUE.)
// END_HELP_COMMENT
BOOL UIEditor::Close(BOOL bLoseChanges /*=TRUE*/)
{
//	if( !IsActive() && UIWB.SetActiveEditor(HWnd()) )
//		return FALSE;

	MST.DoKeys("^{F4}");	// File/Close can close too much in RC files

	UIWBMessageBox mb;
	while( mb.WaitAttachActive(500) )
	{
		if( mb.ButtonExists(MSG_NO) )
			mb.ButtonClick(MSG_NO);
		else
			EXPECT( FALSE );	// this shouldn't happen
	}

	return TRUE ; // WaitUntilGone(4000);		// give it 2 seconds to disappear
}

