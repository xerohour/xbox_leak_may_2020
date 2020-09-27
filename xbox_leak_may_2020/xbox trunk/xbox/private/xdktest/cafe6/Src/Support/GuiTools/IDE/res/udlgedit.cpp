///////////////////////////////////////////////////////////////////////////////
//	UDLGEDIT.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIDlgEdit class
//

#include "stdafx.h"
#include "mstwrap.h"
#include "..\shl\wbutil.h"
#include "udlgedit.h"
#include "..\..\testutil.h"
#include "..\sym\dockids.h"
#include "..\shl\uwbframe.h"
#include "..\sym\reg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIDlgEdit::IsValid(void) const
// Description: Determine whether the dialog editor is valid by determining if the window is valid, visible, the Resource Symbols menu item is enabled, and the title is correct.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDlgEdit::IsValid(void) const
{
	return UIResEditor::IsValid() && IsValidType(m_pszTypeText[ED_DIALOG]);
}

void UIDlgEdit::OnUpdate(void)
{
	UIResEditor::OnUpdate();

	if( !IsValid() )
		m_hwndDialog = NULL;
	else
	{
	    HWND hwndTop = GetDlgItem(HWnd(), 59648);    	// Get the Top Window
	    HWND hwndScroll1 = GetWindow(hwndTop, GW_CHILD ); // Get the 1st Scrollbar
	    HWND hwndCanvas =  GetWindow(hwndScroll1, GW_HWNDLAST );  // Get the Canvas (last in Zorder)
	    m_hwndDialog = GetWindow(hwndCanvas, GW_CHILD);  		// The Dialog frame is the child
	}
}

/*  DropControl
'*      Drops a specified control relative to the client are of the dialog
'*      for give editor window.		  */

// BEGIN_HELP_COMMENT
// Function: BOOL UIDlgEdit::DropControl(int nControl, int cX, int cY )
// Description: Drop the specified control in the dialog editor.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: nControl An integer containing the number of the control, which is a zero-based index into the control palette.
// Param: cX An integer containing the X (horizontal) position where the control should be dropped relative to the upper left corner of the dialog editor.
// Param: cY An integer containing the Y (vertical) position where the control should be dropped relative to the upper left corner of the dialog editor.
// END_HELP_COMMENT
BOOL UIDlgEdit::DropControl(int nControl, int cX, int cY )
{
	HWND hwndCtrl = UIWB.GetToolbar( IDDW_CONTROLS,PACKAGE_VRES );
    	
	OnUpdate();
    // Drop the control relative to the dialog client area.

    // Try finding the control pallet by using the caption.	
	if (hwndCtrl == NULL)
	{
		hwndCtrl = MST.WFndWnd(GetLocString(10013),FW_DEFAULT) ;
	}
	if (hwndCtrl == NULL)
	{
	  		return FALSE;
	}
	else				 //REVIEW: What if toolbar is hidden
    {   
		CRect	rcPalette;
    
    	GetClientRect(hwndCtrl, &rcPalette);
		
        //Size of buttons in toolbar = 24x22
		int nCols = ( rcPalette.right + 1 ) / 24;
    	DragMouse(VK_LBUTTON, 
    		hwndCtrl, 13+((nControl % nCols)*24), 13+((nControl/nCols)*21), 
    		m_hwndDialog, cX, cY);
    }
  	return TRUE;
}
