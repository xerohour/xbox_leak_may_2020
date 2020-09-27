//////////////////////////////
//  UTOOLBAR.CPP
//
//  Created by :            Date :
//      EnriqueP              1/13/94
//
//  Description :
//      Implementation of the UIToolbar class
//

#include "stdafx.h"

#include "utoolbar.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\sym\dockids.h"
#include "ucustdlg.h"
#include "uwbframe.h"
#include "Strings.h"
#include "..\sym\reg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: int UIToolbar::DisplayTBDlg(void)
// Description: Bring up the Toolbars dialog.
// Return: TRUE if the Toolbars dialog is displayed; FALSE otherwise.
// END_HELP_COMMENT
int UIToolbar::DisplayTBDlg(void)
{
	UICustomizeTabDlg uiTBD;
	return uiTBD.DisplayBars()!=NULL;
}


// BEGIN_HELP_COMMENT
// Function: int UIToolbar::ShowTB(int nTbId, BOOL bShow)
// Description: Display or hide the toolbar specified by nTbId.
// Return: TRUE if successful; FALSE otherwise.
// Param: nTbId The id of the toolbar: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD.
// Param: bShow TRUE to show the toolbar; FALSE to hide it.
// END_HELP_COMMENT
int UIToolbar::ShowTB(int nTbId, BOOL bShow)
{
	long pkgId ;
	switch(nTbId)
	{
		case IDTB_SUSHI_STANDARD:
			pkgId = PACKAGE_SUSHI;
			break;
		case IDTB_VCPP_DEBUG:
			pkgId = PACKAGE_DEBUG;
			break ;
		case IDTB_VCPP_BROWSE:
			pkgId = PACKAGE_VCPP;
			break ;
		case IDTB_VPROJ_BUILD:
			pkgId = PACKAGE_VPROJ ;
			break ;
		case IDTB_CLASSVIEW_WIZBARU:
			pkgId = PACKAGE_CLASSVIEW;
			break ;
		case IDTB_VRES_RESOURCE:
		case IDTB_VRES_DIALOG: 
		case IDDW_CONTROLS:
		case IDDW_GRAPHICS:
		case IDDW_COLORS:
		case IDDW_TEXTTOOL:
			pkgId = PACKAGE_VRES ;
			break ;
		default: ;
	}
	HWND hTb = UIWB.GetToolbar(nTbId,pkgId);

	if ( bShow == IsWindowVisible(hTb) )		
		return TRUE; //UITB_NO_ERROR;
	if (!DisplayTBDlg())
		return FALSE;  //UITB_ERR_NOT_DISPLAYED;
	
	char  szTBName[21];
	
	if (IsDocked(hTb) ||  !IsWindowVisible(hTb) )
	{
		 strcpy(szTBName, GetTBText(nTbId) );
	}
	else
	{
		HWND  hParent = GetParent(hTb);	   // Get Title on the fly if is not docked
		EXPECT(hParent != NULL);
		SendMessage(hParent, WM_GETTEXT, (WPARAM)20, (LPARAM)szTBName );
	}

	CString id;
	id.Format("Toolbars:",VSHELL_IDC_TOOLBAR);
	MST.WListItemClk(NULL, szTBName, 5);	// Click on toolbar to show it
	hTb = UIWB.GetToolbar(nTbId,pkgId);
	
	if (bShow != IsWindowVisible(hTb) )
	{
		//LOG->Comment("WARNING: Toolbar: Show / Hide failed");
		return FALSE; //UITB_ERR_FAILED;
	}

	return TRUE; // UITB_NO_ERROR;		
}



// BEGIN_HELP_COMMENT
// Function: CString UIToolbar::GetTBText(int nTbId)
// Description: Get the title of the toolbar.
// Return: A CString that contains the title of the toolbar.
// Param: nTbId The id of the toolbar: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD.
// END_HELP_COMMENT
CString UIToolbar::GetTBText(int nTbId)
{
	switch (nTbId)
	{
		case  IDTB_SUSHI_STANDARD:  
				return GetLocString(IDSS_TB_STANDARD);
		case  IDTB_SUSHI_EDIT:  
				return GetLocString(IDSS_TB_EDIT);
		case  IDTB_VRES_RESOURCE:  
				return GetLocString(IDSS_TB_RESOURCE);
		case  IDTB_VCPP_DEBUG:  
				return GetLocString(IDSS_TB_DEBUG);
		case  IDTB_VCPP_BROWSE:  
				return GetLocString(IDSS_TB_BROWSE);
		case  IDTB_VPROJ_BUILD:  
				return GetLocString(IDSS_TB_BUILD);
		case  IDDW_CONTROLS:  
				return GetLocString(IDSS_TB_CONTROLS);
		case  IDTB_VRES_DIALOG:  
				return GetLocString(IDSS_TB_DIALOG);
		case  IDDW_COLORS:  
				return GetLocString(IDSS_TB_COLORS);
		case  IDDW_GRAPHICS:  
				return GetLocString(IDSS_TB_GRAPHICS);
		case  IDDW_TEXTTOOL:  
				return GetLocString(IDSS_TB_TEXT_TOOL);
		case  IDTB_CLASSVIEW_WIZBARU:  
				return GetLocString(IDSS_TB_WIZBAR);
		default:
				return GetLocString(IDSS_TB_BAD_PARAM); //"Bad Param was passed"
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIToolbar::IsDocked(HWND hWnd, int nLocation)
// Description: Determine if the toolbar specified by the HWND is docked in the position given by nLocation.
// Return: TRUE if the toolbar is docked; FALSE otherwise.
// Param: hWnd The handle of the toolbar.
// Param: nLocation The dock location of the toolbar: DW_TOP, DW_BOTTOM, DW_RIGHT, DW_LEFT.
// END_HELP_COMMENT
BOOL UIToolbar::IsDocked(HWND hWnd, int nLocation)   // (EnriqueP)
{
	if (!IsWindowVisible(hWnd))
		return FALSE;
	//REVIEW (enriquep): Do we need to verify this is a dockable window?
	
	HWND  hParent = GetParent(hWnd);
	EXPECT(hParent != NULL);
	if ( ::GetWindowLong(hParent, GWL_STYLE) & WS_POPUP)
		return FALSE; 							// Window is floating

	// We know is docked , now check position.
	RECT rcWBClient;

	::GetClientRect( UIWB.HWnd(), &rcWBClient);
	::MapWindowPoints( UIWB.HWnd(), HWND_DESKTOP, (POINT*)&rcWBClient, 2);		  	// Client area RECT to screen coord.
	
	RECT rcWBMDIClient;
	HWND hMDIClient = UIWB.GetClient();					// Returns MDI Client
	::GetWindowRect( hMDIClient, &rcWBMDIClient );
				
	
	RECT rcDockableWnd;

	::GetWindowRect(hWnd, &rcDockableWnd);			   // Get dockable window dimensions
	
	int nXBorder = ::GetSystemMetrics(SM_CXBORDER);	// Width of a border (usually 1 pix)
	int nYBorder = ::GetSystemMetrics(SM_CYBORDER);   	// Heigth of border (usually 1 pix)	
	
	if( (nLocation & DW_TOP) )
	{
		if( (rcWBClient.top  <= rcDockableWnd.top + nYBorder) && (rcWBMDIClient.top >= rcDockableWnd.bottom - nYBorder) )
			return TRUE;
	}

	if( (nLocation & DW_BOTTOM) )
	{
		if( (rcWBClient.bottom >= rcDockableWnd.bottom - nYBorder) && (rcWBMDIClient.bottom <= rcDockableWnd.top + nYBorder) )
			return TRUE;
	}

	if( (nLocation & DW_LEFT) )
	{
		if( (rcWBClient.left <= rcDockableWnd.left + nXBorder) && (rcWBMDIClient.left >= rcDockableWnd.right - nXBorder) )
			return TRUE;
	}

	if( (nLocation & DW_RIGHT) )
	{
		if( (rcWBClient.right >= rcDockableWnd.right - nXBorder) && (rcWBMDIClient.right <= rcDockableWnd.left + nXBorder ) )
			return TRUE;
	}

	return FALSE;
} 	   

						
