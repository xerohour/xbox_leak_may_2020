//////////////////////////// ///////////////////////////////////////////////////
//  UIDAM.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the UIDAM class
//

#include "stdafx.h"
#include "uidam.h"

#include "..\SYM\cmdids.h"
#include "..\SYM\reg.h"
#include "..\..\testutil.h"
#include "..\SHL\uwbframe.h"
#include "..\SRC\cosource.h"

#include "guitarg.h"

#include <mstwrap.h>

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::GotoSymbol(LPCSTR symbol)
// Description: Go to the specified symbol in the disassembly window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the name of the symbol to go to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL UIDAM::GotoSymbol(LPCSTR symbol, BOOL bCloseAfter /* TRUE */)
	{
		COSource src;

// REVIEW(briancr): this is obsolete code	
//	return UIWB.GoToLine(symbol);
		return src.GoToLine(symbol, bCloseAfter);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::GotoAddress(int address)
// Description: Go to the specified address in the disassembly window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: address An integer that contains the address to go to.
// Param: bCloseAfter A Boolean value that indicates if dialog should be closed after the command was implemented (TRUE).
// END_HELP_COMMENT
BOOL UIDAM::GotoAddress(int address, BOOL bCloseAfter /* TRUE */)
	{
		COSource src;

// REVIEW(briancr): this is obsolete code
//	return UIWB.GoToLine(address);
		return src.GoToLine(address, bCloseAfter);
	}

// BEGIN_HELP_COMMENT
// Function: CString UIDAM::GetInstruction(int offset /* 0 */, int range /* 1 */)
// Description: Get the instruction(s) at the current location in the diassembly window.
// Return: A CString that contains the instructions in the range specified.
// Param: offset An integer that contains the offset (in lines) from the current cursor position to start retrieving instructions from. This value is negative to move up in the window (down in memory). (Default value is 0.)
// Param: range An integer that contains the number of instructions to retrieve (also in lines). (Default value is 1.)
// Param: bLocateCaretAtLeftTop A Boolean value that indicates if the caret should be moved to the left top corner of the DAM window before offset
// END_HELP_COMMENT
CString UIDAM::GetInstruction(int offset /* 0 */, int range /* 1 */, BOOL bLocateCaretAtLeftTop)
	{
	int i;

	::WaitForInputIdle(g_hTargetProc,5000);

	if(bLocateCaretAtLeftTop)
		MST.DoKeys("^{HOME}");

	if( offset < 0 )
		for( i = offset; i < 0; i++ )
			MST.DoKeys( "{UP}" );
	else
		for( i = 0; i < offset; i++ )
			MST.DoKeys( "{DOWN}" );

	MST.DoKeys("{HOME}+({END})");

	ASSERT( range>0 );
	for( i=0; i < range-1; i++ )
		MST.DoKeys("+{DOWN}" );
	MST.DoKeys("^(c)");

	CString cstr;
	GetClipText(cstr);
	
	// We should return to the original position, otherwise next call of this function will give incorrect result
	for( i=0; i < range-1; i++ )
		MST.DoKeys("{UP}" );

	if( offset < 0 )
		for( i = offset; i < 0; i++ )
			MST.DoKeys( "{DOWN}" );
	else
		for( i = 0; i < offset; i++ )
			MST.DoKeys( "{UP}" );


	return cstr;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::IsValid(void) const
// Description: Determine if the disassembly window is a valid window.
// Return: A Boolean that indicates whether the diassembly window is valid (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDAM::IsValid(void) const
	{
	return UIWindow::IsValid();
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::AttachActive(void)
// Description: Attach to the disassembly window if it's the active window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDAM::AttachActive(void)
	{
	HWND hwnd;

    if(hwnd = MST.WFndWndWait(GetLocString(IDSS_ASM_WND_TITLE), FW_FULL, 2))
		return Attach(hwnd);
	else
		return Attach(FindFirstChild(UIWB.HWnd(), IDW_DISASSY_WIN));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::Find(void)
// Description: Find the disassembly window and attach to it
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIDAM::Find(void)
	{
	int pkgId ;
	switch(GetID())
	{
	case 0:
		pkgId = 0 ;
		break ;
	case IDDW_PROJECT:
		pkgId = PACKAGE_VPROJ ;
		break;
	case IDW_LOCALS_WIN: 
		pkgId = PACKAGE_VCPP ;
		break ;
	default:
		break;
	}
	return Attach(UIWB.GetToolbar(GetID(),pkgId));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::IsActive(void)
// Description: Determine if the disassembly window is active.
// Return: A Boolean value that indicates whether the disassembly window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDAM::IsActive(void)
	{
	HWND hwnd = MST.WGetFocus();
	return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::Activate(void)
// Description: Bring up the disassembly window.
// Return: A Boolean value that indicates whether the disassembly window is active (TRUE) or not.
// END_HELP_COMMENT
BOOL UIDAM::Activate(void)
{
	UIWB.DoCommand(IDM_VIEW_ASM, DC_MNEMONIC);

//REVIEW(chriskoz) Find symbols dialog handling moved to CTarget
	::Sleep(1000); //we must wait at least 1 second because IDE does idle before openning the dialog
	::WaitForInputIdle(g_hTargetProc,5000);	//works if openning the Find Symbol dlg last a little bit

	return AttachActive();
}


//////////////////////////////////////////////////////////////////////////////////////////

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::SwitchBetweenASMAndSRC(BOOL bUsingContextMenu)
// Description: Switches from Source window to DAM and back
// Return: A Boolean value which is currenly always TRUE
// Param: bUsingContextMenu A Boolean value that indicates if the context menu should be used to switch.  If FALSE, hot key is used.
// END_HELP_COMMENT
BOOL UIDAM::SwitchBetweenASMAndSRC(BOOL bUsingContextMenu)
{

	if(!bUsingContextMenu)
	{
		MST.DoKeys("^({F7})");
		return TRUE;
	}
	else
	{
	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the menu item
	if(IsActive())
		MST.DoKeys("{DOWN 1}");
	else
		MST.DoKeys("{DOWN 11}");

	// Toggle it
	MST.DoKeys("{ENTER}");

		return TRUE;
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::EnableDockingView(BOOL bEnable)
// Description: Enables or disables the Docking View for DAM window
// Param: bEnable If the parameter is TRUE, Docking View will be enabled. If FALSE - disabled.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIDAM::EnableDockingView(BOOL bEnable)
{
	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIDAM::EnableDockingView(): DAM window not active");
		return FALSE;
		}

	if((IsDockingViewEnabled() && bEnable) || (!IsDockingViewEnabled() && !bEnable))
		return TRUE;

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");

	//Go to the "Docking view" menu item
	MST.DoKeys("{DOWN 9}");

	// Toggle it
	MST.DoKeys("{ENTER}");

	// Check that the edit control exists
	if((IsDockingViewEnabled() && bEnable) || (!IsDockingViewEnabled() && !bEnable))
		return TRUE;
	else 
		return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::IsDockingViewEnabled(BOOL bEnable)
// Description: Check if Docking View is enabled for DAM window
// Param: No Params
// Return: A Boolean value that indicates whether Docking View is enabled (TRUE) or not.
// END_HELP_COMMENT

BOOL UIDAM::IsDockingViewEnabled(void)
{
	UIWBFrame uiwbframe;
	BOOL bResult, bInitiallyMaximized;
	CString csTitle;

	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("ERROR in UIDAM::IsDockingViewEnabled(): Disassembly window not active");
		return FALSE;
		}

	HWND hwnd = HWnd();

	// Compiler gives the error that WIsMaximized is not a member of MSTest, so we use standard Windows function
	//if(MST.WIsMaximized(hwnd))
	if(IsZoomed(hwnd))
		bInitiallyMaximized = TRUE;
	else 
	{
		bInitiallyMaximized = FALSE;
		// MSTest.WMaxWnd doesn't work here for some reason, so we use standard Windows function
		// MST.WMaxWnd(hwnd);
		ShowWindow(hwnd, SW_MAXIMIZE);
	}

	MST.WGetText(uiwbframe.HWnd(), csTitle);
	if(-1 == csTitle.Find("["+ GetLocString(IDSS_ASM_WND_TITLE)+"]"))
		bResult = TRUE;
	else
		bResult = FALSE;


	if(!bInitiallyMaximized)
		// Compiler gives the error that WResWnd is not a member of MSTest, so we use standard Windows function
		//MST.WResWnd(hwnd);
		ShowWindow(hwnd, SW_RESTORE);

	return bResult;

}

// BEGIN_HELP_COMMENT
// Function: BOOL UIDAM::ChooseContextMenuItem(DAM_MENU_ITEM DamMenuItem)
// Description: Choses (toggles) one of the following DAM context menu items
// Param: DamMenuItem One of the following menu items to be chosen (toggled)
// GO_TO_SOURCE, SHOW_NEX_STATEMENT, INSERT_REMOVE_BREAK_POINT, TOGGLE_BREAKPOINT, RUN_TO_CURSOR,
// SET_NEXT_STATEMENT, SOURCE_ANNOTATION, CODE_BYTES, DOCKING_VIEW, CLOSE_HIDE
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT

BOOL UIDAM::ChooseContextMenuItem(DAM_MENU_ITEM DamMenuItem)
{
	Activate();

	if(!IsActive())
		{
		LOG->RecordInfo("UIDAM::ChooseContextMenuItem(): Disassembly window not active");
		return FALSE;
		}

	// Bring up the popup context menu
	MST.DoKeys("+({F10})");
	//Go to the desired menu item

	switch(DamMenuItem) 
	{
		case GO_TO_SOURCE:
		MST.DoKeys("{DOWN 1}");
		break;

		case SHOW_NEX_STATEMENT:
		MST.DoKeys("{DOWN 2}");
		break;

		case INSERT_REMOVE_BREAK_POINT:
		MST.DoKeys("{DOWN 3}");
		break;

		case TOGGLE_BREAKPOINT:
		MST.DoKeys("{DOWN 4}");
		break;

		case RUN_TO_CURSOR:
		MST.DoKeys("{DOWN 5}");
		break;

		case SET_NEXT_STATEMENT:
		MST.DoKeys("{DOWN 6}");
		break;

		case SOURCE_ANNOTATION:
		MST.DoKeys("{DOWN 7}");
		break;

		case CODE_BYTES:
		MST.DoKeys("{DOWN 8}");
		break;

		case DOCKING_VIEW:
		MST.DoKeys("{DOWN 9}");
		break;

		case CLOSE_HIDE:
		MST.DoKeys("{DOWN 10}");
		break;

	}

	// Toggle it
	MST.DoKeys("{ENTER}");

	return TRUE;
}

