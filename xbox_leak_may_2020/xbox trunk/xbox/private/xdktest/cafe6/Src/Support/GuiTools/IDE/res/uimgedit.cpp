///////////////////////////////////////////////////////////////////////////////
//	UIMGEDIT.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the UIImgEdit class
//

#include "stdafx.h"
#include "uimgedit.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\dockids.h"
#include "..\sym\vres.h"
#include "..\sym\props.h"
#include "guiv1.h"
#include "..\shl\ucommdlg.h"
#include "..\shl\uwbframe.h"
#include "..\sym\reg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIImgEdit::IsValid(void) const
// Description: Determine whether the image editor is valid by determining if the window is valid, visible, and the Image menu is enabled.
// Return: A Boolean value that indicates whether the window is valid: TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIImgEdit::IsValid(void) const
{
	return UIEditor::IsValid();
}

/*  ClickInGraphTB
'*      Graphics selection utility called by higher level selection
'*      functions.  Not intended to  be called directly by tests.
*/
void UIImgEdit::ClickInGraphTB ( int nButton, int cX, int cY )
{
    HWND hwndGraphTB = UIWB.GetToolbar( IDDW_GRAPHICS,PACKAGE_VRES );
    EXPECT(hwndGraphTB != 0);

	::ClickMouse(nButton, hwndGraphTB, cX, cY);
}


/*  XClickImgTool
'*      Selects a graphics palette tool.  See
'*      the top of this file for tool index constants. */

// BEGIN_HELP_COMMENT
// Function: void UIImgEdit::ClickImgTool ( int nImgTool )
// Description: Select an image editing tool from the Graphics palette.
// Return: none
// Param: nImgTool A value that indicates the tool to select: GT_SELECT, GT_LASSO, GT_PICKER, GT_ERASER, GT_FILL, GT_ZOOM, GT_PENCIL, GT_BRUSH, GT_SPRAY, GT_LINE, GT_CURVE, GT_TEXT, GT_RECT, GT_COMBORECT, GT_FILLRECT, GT_ROUND, GT_COMBOROUND, GT_FILLROUND, GT_ELLIPSE, GT_COMBOELLIPSE, GT_FILLELLIPSE, GT_POLY, GT_COMBOPOLY, GT_FILLPOLY.
// END_HELP_COMMENT
void UIImgEdit::ClickImgTool ( int nImgTool )
{
	int nX = 14 + ( ( nImgTool % 3 ) * 24 );
    int nY = 24 + ( ( nImgTool / 3 ) * 24 );
    ClickInGraphTB(VK_LBUTTON, nX, nY);
}


/*  ClickColorMode    Clicks on Screen, Reverse or Black
 *      Color selection utility called by higher level selection
 *      functions.  Not intended to  be called directly by tests.
 *		REVIEW(Enrique): This function is dependent on the size of the toolbar and
 *						 will not work as expected on certain cases.
 */

void UIImgEdit::ClickColorMode( int nColor, int  nButton)
{

	// get the handle of the color toolbar (or we timeout)
	HWND hwndColorTB = NULL;
	for (int time = 0; time < 40 && hwndColorTB == NULL; time++) {
		Sleep(250);
		hwndColorTB = UIWB.GetToolbar(IDDW_COLORS,PACKAGE_VRES);
	}

	if (hwndColorTB == NULL) {
		LOG->RecordInfo("UIImgEdit::ClickColorMode: unable to get the handle of the color toolbar.");
		return;
	}

	CString cstrImgType = UIWB.GetProperty(P_ExternFile);	 // REVIEW(Enriquep) Implement GetImageType()

	cstrImgType.MakeUpper();
	
	int cX, cY;			   // Used to store the point where mouse will click 

	if ( (cstrImgType.Find(".BMP") != -1))   //  Is this a bitmap?
	{
		cX = 40;	  			// Click on first solid color position (black)
		cY = 17;
	}
	else
	{										 // Is an Icon or a Cursor
	    if (nColor == -1)
		{
	    	cX = 40;   			// nColor == -1  (Inverse)
	        cY = 34;
		}
		else
		{
			if (nColor == 0)  	// (Screen)
			{
		        cX = 40;
		        cY = 17;
			}
		    else  				// Reset to Black  if nColor > 0      
			{
			    cX = 67;
			    cY = 17;
			}
		}
	}
		  									
	::ClickMouse(nButton, hwndColorTB, cX, cY);
}

/* SetColor: Sets the foreground or background color to the specified color number. This function 
 *	uses the keyboard to navigate to the desired color. This function automatically adjusts color
 *  positions according to the type of palette that is currently active.
 *  nColor == -1   Inverse					 Cursors or Icons only
 *  nColor == 0    Screen or transparent		 Cursors or Icons only
 *  nColor >= 1    Position of a solid color starting with Black as 1.
 */

// BEGIN_HELP_COMMENT
// Function: void UIImgEdit::SetColor(int nColor, int nButton /* VK_LBUTTON */)
// Description: Set the foreground (left mouse button) or background (right mouse button) color in the image editor.
// Return: none
// Param: nColor An integer that represents the position of the color on the color palette. This value is 1-based value, starting with black as 1. For cursors and icons: -1 means use the inverse color; 0 means use the screen or transparent color.
// Param: nButton The mouse button to assign the color to: VK_LBUTTON, VK_RBUTTON. (Default value is VK_LBUTTON.)
// END_HELP_COMMENT
void  UIImgEdit::SetColor(int nColor, int nButton /* VK_LBUTTON */)   //REVIEW: Use CM_FOREGROUND or CM_BACKGROUND instead of button constants.
{
	ASSERT( (nButton == VK_LBUTTON) || (nButton == VK_RBUTTON) );
	ASSERT( nColor > -2 );	 // -1 = inverse, 0 = screen, 1 to n = solid colors

    ClickColorMode(nColor, nButton);
	CString strBKMode(ACC_NEXT_FG_COLOR);			// Default is foreground mode
    if (nColor > 0)   					// If color != inverse or screen
	{
        if (nButton == VK_RBUTTON)
            strBKMode = ACC_NEXT_BK_COLOR;   		// Set background color

        for (int i = 1; i <= nColor - 1; i++)		// Navigates thru colors
        	MST.DoKeys(strBKMode);
    }
}

// BEGIN_HELP_COMMENT
// Function: HWND UIImgEdit::GetPane(int nPane)
// Description: Get the HWND of one of the panes in the image editor.
// Return: The HWND of the requested pane. NULL if the requested pane does not exist.
// Param: nPane A value that indicates the pane to get the HWND for: PANE_LEFT, PANE_RIGHT.
// END_HELP_COMMENT
HWND UIImgEdit::GetPane(int nPane)
{
	HWND hLeft = HWnd();
	HWND hRight = NULL;

	hLeft = GetWindow(HWnd(), GW_CHILD);	// view	or toolbar
	hLeft = GetWindow(hLeft, GW_HWNDLAST);	// view
	hLeft = GetWindow(hLeft, GW_CHILD);		// first interesting window
	while( hLeft != NULL )
	{
		char acClass[32];
		GetClassName(hLeft, acClass, 31);
		if( strnicmp(acClass, "Afx:", 4 ) == 0 )
		{
			if( hRight == NULL )	// get the two windows of class "Afx:...."
				hRight = hLeft;
			else
				break;
		}
		hLeft = GetWindow(hLeft, GW_HWNDNEXT);
	}

	CRect rcLeft, rcRight;
	GetWindowRect(hLeft, &rcLeft);
	GetWindowRect(hRight, &rcRight);
	if( rcLeft.left > rcRight.left )
	{
		ASSERT( rcLeft.left > rcRight.right );	// just to make sure
		HWND hTemp = hLeft;		// if I have them backwards, swap them
		hLeft = hRight;
		hRight = hTemp;
	}

	switch( nPane )
	{
		case PANE_LEFT:
			return hLeft;
		case PANE_RIGHT:
			return hRight;
		default:
			ASSERT(FALSE);		// no such pane
	}
	return NULL;
}

// BEGIN_HELP_COMMENT
// Function: void UIImgEdit::ClickMouse(int nButton, int nPane, int cX, int cY, int nMag /*=1*/)
// Description: Click the mouse at the given coordinates in the given pane in the image editor. The coordinates are relative to the upper left corner of the pane.
// Return: none
// Param: nButton A value that indicates which button to click in the image editor: VK_LBUTTON, VK_MBUTTON, VK_RBUTTON.
// Param: nPane A value that indicates which pane of the image editor to click in: PANE_LEFT, PANE_RIGHT.
// Param: cX An integer that contains the X (horizontal) position to click. This position is relative to the upper left corner of the pane.
// Param: cY An integer that contains the Y (vertical) position to click. This position is relative to the upper left corner of the pane.
// Param: nMag An integer that contains the magnification in the image editor. This value is used to click a particular "bit" in the image editor by multiplying the X and Y coordinates by nMag. (Default value is 1.)
// END_HELP_COMMENT
void UIImgEdit::ClickMouse(int nButton, int nPane, int cX, int cY, int nMag /*=1*/)
{
	HWND hPane = GetPane(nPane);
	cX = 6 + cX * nMag;
	cY = 6 + cY * nMag;

	::ClickMouse(nButton, hPane, cX, cY);
}

// BEGIN_HELP_COMMENT
// Function: void UIImgEdit::DragMouse(int nButton, int nPane, int cX1, int cY1, int cX2, int cY2, int nMag /*=1*/)
// Description: Drag the mouse from the beginning coordinates to the ending coordinates in the given pane in the image editor. The coordinates are relative to the upper left corner of the pane.
// Return: none
// Param: nButton A value that indicates which button to click and hold while dragging in the image editor: VK_LBUTTON, VK_MBUTTON, VK_RBUTTON.
// Param: nPane A value that indicates which pane of the image editor to drag in: PANE_LEFT, PANE_RIGHT.
// Param: cX1 An integer that contains the X (horizontal) position to start the drag. This position is relative to the upper left corner of the pane.
// Param: cY1 An integer that contains the Y (vertical) position to start the drag. This position is relative to the upper left corner of the pane.
// Param: cX1 An integer that contains the X (horizontal) position to end the drag. This position is relative to the upper left corner of the pane.
// Param: cY1 An integer that contains the Y (vertical) position to end the drag. This position is relative to the upper left corner of the pane.
// Param: nMag An integer that contains the magnification in the image editor. This value is used to click and drag across particular "bits" in the image editor by multiplying the X and Y coordinates by nMag. (Default value is 1.)
// END_HELP_COMMENT
void UIImgEdit::DragMouse(int nButton, int nPane, int cX1, int cY1, int cX2, int cY2, int nMag /*=1*/)
{
	HWND hPane = GetPane(nPane);
	cX1 = 6 + cX1 * nMag;
	cY1 = 6 + cY1 * nMag;
	cX2 = 6 + cX2 * nMag;
	cY2 = 6 + cY2 * nMag;

	::DragMouse(nButton, hPane, cX1, cY1, hPane, cX2, cY2);
}

HWND UIImgEdit::FontDialog(void)
{
	HWND hwnd = UIWB.GetToolbar(IDDW_TEXTTOOL,PACKAGE_VRES);	// text tool hwnd
	if( !::IsWindowVisible(hwnd) )
	{
		ClickImgTool(GT_TEXT);	// only click if necessary, as this erases previous text
		EXPECT( ::IsWindowVisible(hwnd) );
	}
	hwnd = GetWindow(hwnd, GW_CHILD);			// child, which is parent of button and edit
	MST.WButtonClick(GetLabel(VRES_IDC_SETFONT, hwnd));

	UIFontDlg ufd;
	if( !ufd.WaitAttachActive(5000) )
		LOG->RecordInfo("Could not open Font dialog for text tool; '%s' is active", (LPCSTR)ufd.GetText());
	ufd.IsValid();
	return ufd;
}

// BEGIN_HELP_COMMENT
// Function: void UIImgEdit::SetFont(LPCSTR szFontName /*=NULL*/, int nSize /*=-1*/, int nStyle /*=-1*/)
// Description: Set the font to be used by the text tool in the image editor.
// Return: none
// Param: szFontName A pointer to a string that contains the name of the font. NULL means use the current font. (Default value is NULL.)
// Param: nSize An integer that contains the point size of the font. -1 means use the current point size. (Default value is -1.)
// Param: nStyle An index into the list of font styles. -1 means use the current style. This value is usually: 1 for regular style; 2 for italic style; 3 for bold style; and 4 for bold italic style.
// END_HELP_COMMENT
void UIImgEdit::SetFont(LPCSTR szFontName /*=NULL*/, int nSize /*=-1*/, int nStyle /*=-1*/)
{
	UIFontDlg ufd = FontDialog();
	
	if( szFontName != NULL )	// NULL means leave it as is
		ufd.SetName(szFontName);
	if( nSize >= 0 )				// <0 means leave it as is
		ufd.SetSize(nSize);
	if( nStyle >= 0 )			// <0 means leave it as is
		ufd.SetStyle(nStyle);	// style is just a base-1 index - you must know the font

	ufd.OK();
	ufd.WaitUntilGone(1000);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIImgEdit::IsStandalone(void)
// Description: Determine if the image editor is stand alone or part of a resource script. (If a resource script window is open, this function assumes this image editor belongs to that script.)
// Return: A Boolean value that indicates whether the image editor is stand alone. TRUE if it's stand alone; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIImgEdit::IsStandalone(void)
{
	return IsValid() && !UIResEditor::IsValid();	// it's an image editor, but not a resource
}

// BEGIN_HELP_COMMENT
// Function: int UIImgEdit::GetEditorType(void)
// Description: Get the specific type of image editor. This function is NYI.
// Return: A value indicating the type of image editor: ED_CURSOR, ED_ICON, ED_BITMAP.
// END_HELP_COMMENT
int UIImgEdit::GetEditorType(void)
{
	if( !IsValid() )
		return -2;		// ED_UNKNOWN

// REVIEW(briancr): this function would not work the way it's implemented, even on Dolphin
// controls with the given IDs do not exist
	ASSERT(0);
	return -2;

#if 0

	HWND hwndSplitterView = ::GetWindow(HWnd(), GW_CHILD);	// the view is the only child of the frame
	HWND hwnd = GetDlgItem(hwndSplitterView, VRES_IDC_HOTSPOT);
	if( hwnd && (GetDlgControlClass(hwnd) == GL_STATIC) )	// does it have a HotSpot indicator?
		return ED_CURSOR;
	hwnd = GetDlgItem(hwndSplitterView, VRES_IDC_IMAGELIST);
	if( hwnd && (GetDlgControlClass(hwnd) == GL_STATIC) )	// does it have a device indicator?
		return ED_ICON;
	return ED_BITMAP;	// what else could it be?
#endif
}

