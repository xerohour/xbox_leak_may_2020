//////////////////////       CWRKSPCE.CPP
//
//  Created by :            Date :
//      EnriqueP                 1/10/94
//
//  Description :
//      Implementation of the COWorkSpace class
//
#include "stdafx.h"
#include "cowrkspc.h"
#include "..\sym\cmdids.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "uiwbmsg.h"
#include "utoolbar.h"
#include "guiv1.h"
#include "ucustdlg.h"
#include "uwbframe.h"
#include "..\sym\reg.h"
#include "..\sym\dockids.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// instantiate a global COWorkspace
SHL_DATA COWorkSpace WS;

// BEGIN_HELP_COMMENT
// Function: void COWorkSpace::CloseAllWindows(void)
// Description: Close all open windows in the IDE.
// Return: none
// END_HELP_COMMENT
void COWorkSpace::CloseAllWindows(void)
{
UIWB.CloseAllWindows();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::IsToolbarVisible(UINT id)
// Description: Determine if the toolbar indicated by id is visible. (Use IsDockWindowVisible for dockable windows.)
// Return: TRUE if the toolbar is visible; FALSE if not.
// Param: id The id of the toolbar: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD.
// END_HELP_COMMENT
BOOL COWorkSpace::IsToolbarVisible(UINT id)
{
	long pkgId ;
	switch(id)
	{
		case IDTB_SUSHI_STANDARD:
			pkgId = PACKAGE_SUSHI;
			break;
		case IDTB_VCPP_DEBUG:
		case IDTB_VCPP_BROWSE:
			pkgId = PACKAGE_VCPP;
			break ;
		case IDTB_VPROJ_BUILD:
			pkgId = PACKAGE_VPROJ ;
			break ;
		case IDTB_VRES_RESOURCE:
		case IDTB_VRES_DIALOG: 
		case IDDW_CONTROLS:
		case IDDW_GRAPHICS:
		case IDDW_COLORS:
		case IDDW_TEXTTOOL:
			pkgId = PACKAGE_VRES ;
			break ;
		default:
			;
	}
	HWND  hTb = UIWB.GetToolbar(id,pkgId);
	return IsWindowVisible(hTb);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::IsDockWindowVisible(UINT id)
// Description: Determine if the dockable window indicated by id is visible. (Use IsToolbarVisible for toolbars.)
// Return: TRUE if the dockable window is visible; FALSE if not.
// Param: id The id of the dockable window: IDDW_VCPP_ERROR_WIN, IDDW_VCPP_WATCH_WIN, IDDW_VCPP_LOCALS_WIN, IDDW_VCPP_CPU_WIN, IDDW_VCPP_MEMORY_WIN, IDDW_VCPP_CALLS_WIN, IDDW_VCPP_DISASSY_WIN.
// END_HELP_COMMENT
BOOL COWorkSpace::IsDockWindowVisible(UINT id)
{
	HWND  hDW = UIWB.GetToolbar(id,PACKAGE_VCPP);
	return IsWindowVisible(hDW);
}

// BEGIN_HELP_COMMENT
// Function: int COWorkSpace::ShowToolbar(UINT id, BOOL bShow /*=TRUE*/)
// Description: Show or hide the toolbar indicated by id. (Use this function to show or hide dockable windows, also.)
// Return: TRUE if the toolbar is successfully shown (or hidden); FALSE otherwise.
// Param: id The id of the toolbar or dockable window: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD, IDDW_VCPP_ERROR_WIN, IDDW_VCPP_WATCH_WIN, IDDW_VCPP_LOCALS_WIN, IDDW_VCPP_CPU_WIN, IDDW_VCPP_MEMORY_WIN, IDDW_VCPP_CALLS_WIN, IDDW_VCPP_DISASSY_WIN.
// Param: bShow TRUE to show the toolbar or window; FALSE to hide it. (Default is TRUE.)
// END_HELP_COMMENT
int COWorkSpace::ShowToolbar(UINT id, BOOL bShow /*=TRUE*/)	  //	REVIEW(enriquep): Use enum for id
{
	UIToolbar uiTB;
	return uiTB.ShowTB(id, bShow);
}		

//int COWorkSpace::ShowDockWindow(UINT id, BOOL bShow /*=TRUE*/)
/*{
	UIDockWindow* pudw;
	switch( id )
	{
		case IDW_OUTPUT_WIN:
			pudw = new UIOutput;
			break;
		case IDW_WATCH_WIN:
			pudw = new UIWatch;
			break;
		case IDW_LOCALS_WIN:
			pudw = new UILocals;
			break;
		case IDW_CPU_WIN:
			pudw = new UIRegisters;
			break;
		case IDW_MEMORY_WIN:
			pudw = new UIMemory;
			break;
		case IDW_CALLS_WIN:
			pudw = new UICallStack;
			break;
		case IDW_DISASSY_WIN:
			pudw = new UIDisassembly;
			break;
		default:
			ASSERT(FALSE);		// never heard of that ID
			return NULL;
	}
	BOOL b = pudw->Activate();
	delete pudw;
	if( !b )					// attempt to show failed - who knows why
		return NULL;
	if( bShow )					// window was activated and has the focus
		return MST.WGetFocus();
	DoKeys("+{escape}");		// shift escape hides the active docking-window/toolbar
	return NULL;				// should we return newly active window?
}	  
*/

// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::IsWindowDocked(UINT id, int nLocation)
// Description: Determine if the dockable window indicated by id is docked. (Use IsToolbarDocked for toolbars.)
// Return: TRUE if the dockable window is docked; FALSE otherwise.
// Param: id The id of the dockable window: IDDW_VCPP_ERROR_WIN, IDDW_VCPP_WATCH_WIN, IDDW_VCPP_LOCALS_WIN, IDDW_VCPP_CPU_WIN, IDDW_VCPP_MEMORY_WIN, IDDW_VCPP_CALLS_WIN, IDDW_VCPP_DISASSY_WIN.
// Param: nLocation The location of the dockable window: DW_TOP, DW_BOTTOM, DW_LEFT, DW_RIGHT.
// END_HELP_COMMENT
BOOL COWorkSpace::IsWindowDocked(UINT id, int nLocation)   // (enriquep)	 REVIEW: Use enum for id
{
	HWND hWnd = UIWB.GetToolbar(id,PACKAGE_VCPP);
	UIToolbar uiDW;
	return uiDW.IsDocked(hWnd, nLocation); 	//REVIEW: (enriquep) we use the UIToolbar version for now
} 	

// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::IsToolbarDocked(UINT id, int nLocation)
// Description: Determine if the toolbar indicated by id is docked. (Use IsWindowDocked for dockable windows.)
// Return: TRUE if the toolbar is docked; FALSE otherwise.
// Param: id The id of the toolbar: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD.
// Param: nLocation The location of the toolbar: DW_TOP, DW_BOTTOM, DW_LEFT, DW_RIGHT.
// END_HELP_COMMENT
BOOL COWorkSpace::IsToolbarDocked(UINT id, int nLocation)   // (enriquep)	 REVIEW: Use enum for id
{
	long pkgId ;
	switch(id)
	{
		case IDTB_SUSHI_STANDARD:
			pkgId = PACKAGE_SUSHI;
			break;
		case IDTB_VCPP_DEBUG:
		case IDTB_VCPP_BROWSE:
			pkgId = PACKAGE_VCPP;
			break ;
		case IDTB_VPROJ_BUILD:
			pkgId = PACKAGE_VPROJ ;
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

	HWND hWnd = UIWB.GetToolbar(id,pkgId);
	UIToolbar uiTB;
	return uiTB.IsDocked(hWnd, nLocation); 	
}    


// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::ShowPropPage(BOOL bShow)
// Description: Display or hide the property page for the currently active window.
// Return: TRUE if the property page is successfully shown (or hidden); FALSE otherwise.
// Param: bShow TRUE to show the property page; FALSE to hide it.
// END_HELP_COMMENT
BOOL COWorkSpace::ShowPropPage(BOOL bShow)
{
	if( !IsWindowActive("Properties") )
		DoKeys("%{ENTER}");		//Give Focus to PropPage REVIEW:Use SetFocus() when hook is available
	if( bShow == FALSE )
		DoKeys("+{ESC}");		// Hide it 	
	return TRUE;
}

/*CString COWorkSpace::GetProperty(int nProp)
{
	CString str = "Empty Property";
	ATOM atom = (ATOM)UIWB.PWnd()->SendMessage( m_msgGetProp, nProp);
	if (atom != 0)
	{
		UINT ret = GlobalGetAtomName( atom, str.GetBufferSetLength(MAX_ATOM_LENGTH), MAX_ATOM_LENGTH );
		ret = GlobalDeleteAtom(atom);	// this atom created by the Workbench
		str.ReleaseBuffer(-1);
	}
 	return str;
}	 */


/*HWND COWorkSpace::GetStatusBar(void)
{
    HWND hwnd = ::GetWindow(UIWB.HWnd(), GW_CHILD);
    char acClass[128];
    ::GetClassName(hwnd, acClass, 127);
    while( (hwnd != NULL) && strcmp(acClass, "AfxControlBar") ) // REVIEW(davidga): move to #define
    {
        hwnd = ::GetWindow(hwnd, GW_HWNDNEXT);
        ::GetClassName(hwnd, acClass, 127);
    }
    return hwnd;    // will be Statusbar or NULL
}							*/

// BEGIN_HELP_COMMENT
// Function: CString COWorkSpace::GetStatusText(void)
// Description: Get the text from the IDE's status bar.
// Return: A CString containing the text in the status bar.
// END_HELP_COMMENT
CString COWorkSpace::GetStatusText(void)
{
    HWND hwnd = UIWB.GetStatusBar();
    return ::GetText(hwnd);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact /*=TRUE*/)
// Description: Wait for the given text (sz) to appear in the status bar. This function will wait until the text is displayed in the status bar or the number of milliseconds given in dwMilliSeconds have elapsed.
// Return: TRUE if the text is found in the status bar before dwMilliSeconds elapses; FALSE otherwise.
// Param: sz A pointer to a string containing the text to wait for in the status bar.
// Param: dwMilliSeconds The number of milliseconds to wait for the text to appear.
// Param: bExact TRUE if the text in status bar must match the text in sz exactly; FALSE for a partial patch.
// END_HELP_COMMENT
BOOL COWorkSpace::WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact /*=TRUE*/)
{
    DWORD dwDone = 0;
    while( dwDone <= dwMilliSeconds )
    {
        CString strStatus = GetStatusText();
        if( bExact )
        {
            if( strStatus == sz )
                return TRUE;
        }
        else
        {
            if( strStatus.Find(sz) == 0 )   // check first part of string
                return TRUE;
        }
        
        if( dwDone >= dwMilliSeconds )  // no more waiting
            return FALSE;       // just say we didn't find it

        DWORD dwSleep = dwMilliSeconds - dwDone >= 1000 ? 1000 : dwMilliSeconds - dwDone;
        Sleep(dwSleep);     // wait another second (or whatever's left)
        dwDone += dwSleep;  // and try at least one more time
    }
    return FALSE;       // should never get hit
}

// REVIEW: larryh - When this CAN work, it needs to be fixed TO work.

// BEGIN_HELP_COMMENT
// Function: BOOL COWorkSpace::VerifyCaretPosition( int nRow, int nCol, LPCSTR szErrTxt )
// Description: Verify the caret position in the active window.
// Return: TRUE if the caret position matches the given row and column; FALSE otherwise.
// Param: nRow The expected row for the caret.
// Param: nCol The expected column for the caret.
// Param: szErrTxt A pointer to a string containing the text to emit to log if the caret position doesn't match the given row and column.
// END_HELP_COMMENT
BOOL COWorkSpace::VerifyCaretPosition( int nRow, int nCol, LPCSTR szErrTxt )
{
    char szBuffer[80];

	EXPECT( nRow >= 0 && nCol >= 0 );
	wsprintf( szBuffer, "Ln %d,Col %d", nRow, nCol );
		
	if (UIWB.WaitForStatusText( szBuffer, (DWORD) 5000 /* 5 seconds */, TRUE ) )
		return TRUE;
	LOG->Comment( "VerifyCaretPosition: ",szErrTxt,": expected ",szBuffer," <> status bar column ",UIWB.GetStatusText() );
	return FALSE;
}

//BOOL COWorkSpace::AddTBButton(int nButtonType, HWND hwndToolbar /* = NULL */)
/*{
    EXPECT( IsWindowActive("Customize") );		//REVIEW: Verify Toolbars tab is set as well
    UIDialog uDlg;   // REVIEW(davidga): when UICustDlg is implemented, you can use ExpectValid instead
    uDlg.AttachActive();
        
    if ( hwndToolbar != NULL)                               // Put button in a toolbar else create new toolbar
    {                               
        EXPECT( IsWindowVisible(hwndToolbar) );

        DragMouse(VK_LBUTTON, uDlg, 155, 70, hwndToolbar, 2, 2);     //REVIEW: Button position is hardcoded
    }
    else
    {
        DragMouse(VK_LBUTTON, uDlg, 155, 70, UIWB.HWnd(), 2, 2);
    }

    return TRUE;
}		  */

// BEGIN_HELP_COMMENT
// Function: COWorkSpace::AddTool(CString strMenuText, CString strToolPath, CString strArguments, CString strInitDir, BOOL PromptArguments, BOOL RedirectOutput, BOOL CloseOnExit);
// Description: Add a Tool to the Tools menu.
// Return: TRUE if successful; FALSE otherwise.
// Param: strToolPath The complete drive, path, and executable file name
// Param: strArguments Any arguments to be passed to the tool on startup
// Param: strInitDir Initial Directory
// Param: strMenuText Name and shortcut to appear on tools menu
// Param: PromptArgument Check Prompt for Arguments box
// Param: RedirectOutput Check Redirect to Output Window
// Param: CloseOnExit Check Close Window on Exit
// END_HELP_COMMENT
BOOL COWorkSpace::AddTool(CString strMenuText,  
						  CString strToolPath, // full path for command name
	 					  CString strArguments,
	 					  CString strInitDir,
	 					  BOOL PromptArgument,
 						  BOOL RedirectOutput,
						  BOOL CloseOnExit)
{
	// To use ShowPage
	UICustomizeTabDlg CustomizeTools;
// WARNING: Due to a bug in MS Test, shortcut keys added with this function that
// conflict with Tools.Customize.Tools shortcuts will be sidetracked by going to
// the shortcuts for the menu. DO NOT add &C A N M I W P E O C R U D H 
	CustomizeTools.ShowPage(TAB_TOOLS,3);	// Tools.Customize.Tools
	MST.WButtonClick(GetLabel(VSHELL_ID_TOOLS_ADD));						// select Add
	MST.DoKeys(strToolPath);
	MST.DoKeys("{ENTER}");					// OK for path
	MST.WEditSetText(GetLabel(VSHELL_ID_TOOLS_ARGUMENTS),strArguments);			// select arguments
	MST.WEditSetText(GetLabel(VSHELL_ID_TOOLS_MENUTEXT),strMenuText);
	MST.WEditSetText(GetLabel(VSHELL_ID_TOOLS_STAT_INITDIR),strInitDir);
	if (RedirectOutput)	MST.WCheckCheck(GetLabel(VSHELL_ID_TOOLS_REDIRECT));					
	if (PromptArgument) MST.WCheckCheck(GetLabel(VSHELL_ID_TOOLS_ASKARGUMENTS));		
	if (CloseOnExit)	MST.WCheckCheck(GetLabel(VSHELL_ID_TOOLS_CLOSE));			
	MST.DoKeys("{ENTER}");					// OK for whole dialog

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: COWorkSpace::DeleteTool(int Number)
// Description: Delete a number of Tools from the Tools menu
// Return: TRUE if successful; FALSE otherwise.
// Param: int Number Number of tools to delete
// END_HELP_COMMENT
BOOL COWorkSpace::DeleteTools(int Number)
{  // REVIEW GEORGECH this must be more robust - as when &Remove is not enabled
	int i;
	// To use ShowPage
	UICustomizeTabDlg CustomizeTools;

	CustomizeTools.ShowPage(TAB_TOOLS,3);		// Tools.Customize
	MST.WListItemClk(GetLabel(VSHELL_ID_TOOLS_MENUCONTENTS),1);				// Menu &Contents	
	for (i=0;i<Number;i++)
		MST.WButtonClick(GetLabel(VSHELL_ID_TOOLS_DELETE));			// &Remove	

	MST.DoKeys("{ENTER}");			// OK close menu	

	return (TRUE);
}



