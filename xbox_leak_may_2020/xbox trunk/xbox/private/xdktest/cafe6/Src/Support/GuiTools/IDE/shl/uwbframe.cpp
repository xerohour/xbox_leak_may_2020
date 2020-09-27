/////////////////////////////////////////////////////////////////////////////////
// UWBFRAME.CPP
//
//  Created by :            Date :
//      DavidGa                 9/23/93
//
//  Description :
//      Implementation of the UIWBFrame class
//
#include "stdafx.h"

#include "uwbframe.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\qcqp.h"
#include "..\sym\vcpp32.h"
#include "guiv1.h"
#include "..\..\udialog.h"
#include "ucommdlg.h"
#include "uiwbmsg.h"
#include "udockwnd.h"
#include "upropwnd.h"
#include "uioptdlg.h"
#include "uioutput.h"
#include "wbutil.h"
#include "guitarg.h"
#include "newfldlg.h"
#include "evnthdlr.h"

#pragma comment(lib, "version.lib")

// Obviously this could easily change.  Hopefully, it will last for a while.
#define STATUSBAR_CLASSNAME "Afx:400000:0:0:10:0"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// instantiate a global pointer to UIWBFrame
// this pointer can be substituted if another instance of IDE must be driven
static UIWBFrame theUIWB;
SHL_DATA UIWBFrame *g_pUIWB=&theUIWB;

// I use this all the time when waiting using WFndWndWait to wait for a dialog
#define FW_MODAL_DIALOG     FW_DIALOG | FW_ACTIVE | FW_NOCASE | FW_NOFOCUS | FW_CHILDNOTOK | FW_HIDDENNOTOK

UINT UIWBFrame::m_msgGetProp = RegisterWindowMessage( "GetProp" );
UINT UIWBFrame::m_msgSetProp = RegisterWindowMessage( "SetProp" );
UINT UIWBFrame::m_msgGetToolbar = RegisterWindowMessage( "TestDockable" );
UINT UIWBFrame::m_msgGetItemProp = RegisterWindowMessage( "GetItemProp" );
UINT UIWBFrame::m_msgGetProjPath = RegisterWindowMessage( "GetProjPath" );
UINT UIWBFrame::m_msgSetItemProp = RegisterWindowMessage( "SetItemProp" );
UINT UIWBFrame::m_msgEditorCurPos = RegisterWindowMessage( "MSVCEditorCurPos" );

// Build states
UINT UIWBFrame::m_msgCanWeBuild = RegisterWindowMessage( "CanWeBuild" );
UINT UIWBFrame::m_msgGetErrorCount = RegisterWindowMessage( "GetErrCount" );
UINT UIWBFrame::m_msgGetWarningCount = RegisterWindowMessage( "GetWarnCount" );


void UIWBFrame::OnUpdate(void)
{
	const char* const THIS_FUNCTION = "UIWBFrame::OnUpdate";

	UIMainFrm::OnUpdate();

	// set up the command table
	m_pCmdTable = m_aCmdTable;

	if( !IsValid() )
		m_hwndClient = NULL;
	else
	{
		MST.WSetActWnd(HWnd());
		// wait for up to 2 seconds for the app to be active
		for (int nWait = 0; nWait < 20; nWait++) {
			if (IsActive()) {
				break;
			}
			Sleep(100);
		}
		// check that the app is active
		if (IsActive()) {
			m_hwndClient = MST.WFndWndC( "", "MDIClient", FW_ACTIVE | FW_FULL | FW_NOFOCUS );
		}
		else {
			char aBuf[1024];
			sprintf(aBuf, "%s: Target application (HWND = %lx) is not active. Unable to continue.", THIS_FUNCTION, HWnd());
			throw CTestException(aBuf, CTestException::causeOperationFail);
		}
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::Find(void)
// Description: Find the IDE's frame window and attach to it, if found.
// Return: TRUE if the IDE's frame window is successfully found; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::Find(void)
{
	// search top level windows for hwnd that passes Attach()'s IsValid()
	if( Attach(::GetWindow(MST.WGetActWnd(0), GW_HWNDFIRST)) )
		return TRUE;
	
	while( !Attach(::GetWindow(HWnd(), GW_HWNDNEXT)) )
	{
		CString str = GetText();
		if(HWnd() == NULL)		// if we go past the end of the list
			return FALSE;		// then it must not exist
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::IsValid(void) const
// Description: Determine if the IDE's frame window is valid by checking the window handle, the title of the window, and the window class.
// Return: TRUE if the frame window is valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::IsValid(void) const
{
	if( !UIMainFrm::IsValid() )
		return FALSE;

    char acBuf[256];
    if(strstr(GetText(acBuf, 255), GetLocString(IDSS_WORKBENCH_TITLEBAR)) != acBuf)
        return FALSE;       // title is not correct

    VERIFY(::GetClassName(HWnd(), acBuf, 255));
    if(strstr(acBuf, GetLocString(IDSS_WORKBENCH_CLASSNAME)) != acBuf)
        return FALSE;       // then compare the classname, it might be Cuda

    return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::IsDebug(void)
// Description: Determine if the IDE is a debug build or retail build. (This function is NYI.)
// Return: TRUE if the IDE is a debug build; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::IsDebug(void)
{
    return TRUE;        // STUB: Sushi is always built DEBUG these days
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::IsActive(void)
// Description: Determine whether the IDE is active.
// Return: TRUE if the IDE is active; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::IsActive(void)
{
	for ( HWND hwndActive = MST.WGetActWnd(0); hwndActive; hwndActive = GetParent( hwndActive ) )
		if ( hwndActive == HWnd() )
			return TRUE;
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::IsAlert(void)
// Description: Determine if an alert box (message or dialog) is being displayed by the IDE. (This function is NYI.)
// Return: TRUE if an alert is displayed; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::IsAlert(void)
{
    return FALSE;   // STUB
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::CreateNewFile(LPCSTR szFileType)
// Description: Create a new file in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the window that contains the new file.
// Param: szFileType A pointer to a string containing the type of the new file to create. Valid file types are listed in the File New dialog in the IDE.
// END_HELP_COMMENT
HWND UIWBFrame::CreateNewFile(LPCSTR szFileType)
{
	ASSERT( (szFileType != NULL) && (*szFileType != '\0') );

	UINewFileDlg nwFlDlg ;
	
	nwFlDlg.Display() ;
	nwFlDlg.NewFileTab() ;
	nwFlDlg.SelectFromList(szFileType);
	nwFlDlg.AddToProject(FALSE);
	MST.WButtonClick(GetLabel(IDOK));
	nwFlDlg.WaitUntilGone(2000) ;
  	WaitForInputIdle(g_hTargetProc, 10000);
	CString Text = GetLocString(IDSS_NEW_SOURCE) ;
	if(Text == szFileType)
	{// change the file type to C/C++ (just a hack for now)

		MST.DoKeys("%{ENTER}") ;
		MST.DoKeys("C") ;
		MST.DoKeys("{ENTER}");
	}
	/*DoCommand(ID_FILE_NEW, DC_ACCEL);
	UIDialog nd(0x64,GL_LISTBOX);
	if( !nd.WaitAttachActive(1000) )
	{
		LOG->RecordInfo("Could not open File/New dialog");
		return NULL;
	}

	MST.WListItemClk("", szFileType);
	nd.OK();
	nd.WaitUntilGone(1000);
*/
	return GetActiveEditor();

/*	if (!WaitOnWndWithCtrl(AFX_IDC_LISTBOX,2000))	 // REVIEW(Ivanl) fix this.
	{
		 // Cancel out of the dialog and exit.
		 LOG->RecordInfo("Could not open the File/New dialog");
		 MST.DoKeys("{ESC 5}") ;
		 return NULL ;
	}
	
    MST.WListItemClk("", szFileType);
	MST.WButtonClick(GetLabel(IDOK)) ;
   	WaitOnWndWithCtrl(APPWZ_IDC_PROJ_DIR,2000) ;
    return GetActiveEditor() ;
*/
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::OpenFile(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/)
// Description: Open a file in the IDE.  NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the window that contains the opened file.
// Param: szFileName A pointer to a string that contains the filename of the file to open.
// Param: szChDir A pointer to a string that contains the path the file. NULL means the path is given with the filename or use the current working directory. (Default is NULL.)
// END_HELP_COMMENT
HWND UIWBFrame::OpenFile(LPCSTR szFileName, LPCSTR szChDir /*=NULL*/)
{
//    UIEditor uedOld = GetActiveEditor();
	HWND uedOld = GetActiveEditor();

    UIFileOpenDlg ufod = FileOpenDlg();
	if( !ufod.IsValid() )
	{
		LOG->RecordInfo("Could not open File/Open dialog");
		return NULL;
	}

    if( szChDir != NULL )
        ufod.SetPath(szChDir);
    ufod.SetName(szFileName);
    EXPECT( ufod.OK(FALSE) );

    HWND hwndActive = MST.WGetActWnd(0);
    // (briancr) uedNew == uedOld is not always a valid check that file open
    // succeeded; the file may already be opened and be the top level MDI window
    // possibly a better check would be to check the child window's title for
    // the file name that's passed in...
//  while( (hwndActive == ufod) || ((hwndActive == HWnd()) && (uedNew == uedOld)) )
	ufod.WaitUntilGone(1500);	// the only files that take a while are RC, which causes the whole system to wait while it compiles

    if( !IsActive() )
        return hwndActive;  // return handle of any messagebox that comes up
    else
        return GetActiveEditor();  // or else return handle of new editor window
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SaveFileAs(LPCSTR szFileName, BOOL bOverwrite /*=TRUE*/)
// Description: Save a file in the IDE with a new name.  NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if successful; FALSE otherwise.
// Param: szFileName A pointer to a string that contains the filename.
// Param: bOverwrite TRUE to overwrite an existing file (if one exists); FALSE to not overwrite. (Default is TRUE.)
// END_HELP_COMMENT
BOOL UIWBFrame::SaveFileAs(LPCSTR szFileName, BOOL bOverwrite /*=TRUE*/)
{                                                 //REVIEW params and #define BNT_YES & BTN_NO
    UIFileSaveAsDlg ufsad = FileSaveAsDlg();
	if( !ufsad.IsValid() )
	{
		LOG->RecordInfo("Could not open File/Save As dialog");
		return FALSE;
	}

    ufsad.SetName(szFileName);
    ufsad.OK(bOverwrite);

	UIWBMessageBox mb;
	while( mb.WaitAttachActive(2000) || !ufsad.WaitUntilGone(0) )
    {
		if( !mb.IsValid() )
			continue;

        if( mb.ButtonExists(MSG_YES) )
        {
            if( !bOverwrite )
            {
                mb.ButtonClick(MSG_NO);
                return FALSE;           // File already exists and don't overwrite
            }
            mb.ButtonClick(MSG_YES);	//Overwrite file
        }
		else
			MST.DoKeys("~");	// on any other dialogs, take the default
    }

    return TRUE;                        //File was saved
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SaveAll(int intWaitInMilliSecs /* 0 */)
// Description: selects the File.SaveAll menu item and waits for a specified number of milliseconds.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
// REVIEW (michma): how can we know when Save All is finished?
BOOL UIWBFrame::SaveAll(int intWaitInMilliSecs /* 0 */)
	{
	this->DoCommand(IDM_FILE_SAVE_ALL, DC_MNEMONIC);
	Sleep(intWaitInMilliSecs);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::CloseProject(void)
// Description: Close a project window in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::CloseProject(void)
{
	this->DoCommand(IDM_CLOSE_WORKSPACE, DC_MNEMONIC);

	UIWBMessageBox mb;
	while( mb.WaitAttachActive(3000) != NULL )
	{
		if( mb.ButtonExists(MSG_NO) )
			// this will handle "do you want to save" msg box
			mb.ButtonClick(MSG_NO);
		else
			// this will handle "access denied" msg box for read-only projects
			MST.DoKeys("~");
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: void UIWBFrame::CloseAllWindows(void)
// Description: Close all open editor windows in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: none
// END_HELP_COMMENT
void UIWBFrame::CloseAllWindows(void)   // REVIEW (EnriqueP): Move to CWorkSpace?
{
	// REVIEW: Shouldn't this be IDM instead of ID?
    DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);	

	// Click away any message boxes that come up on closing the windows.
	// Likely to be "File hasn't been saved..." type messages.
	UIWBMessageBox mb;
	while( mb.WaitAttachActive(2000) != NULL )
	{
		if( mb.ButtonExists(MSG_NO) )
			mb.ButtonClick(MSG_NO);
		else
			MST.DoKeys("~");
	}

	CloseProject();
}


// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::GetActiveEditor(void)
// Description: Get the HWND of the active editor in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the active editor, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIWBFrame::GetActiveEditor(void)
{
  	WaitForInputIdle(g_hTargetProc, 10000);
	HWND hwnd = (HWND)SendMessage(GetClient(), WM_MDIGETACTIVE, 0, 0 );
	if (!hwnd)
		hwnd =	MST.WGetActWnd(0) ;

    if (!hwnd || IsIconic(hwnd))
        return 0L;

    return hwnd;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SetActiveEditor(HWND hwndEditor)
// Description: Set the active editor in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if successful; FALSE otherwise.
// Param: hwndEditor The HWND of the window to make the active editor.
// END_HELP_COMMENT
BOOL UIWBFrame::SetActiveEditor(HWND hwndEditor)
{
	SendMessage( GetClient(), WM_MDIACTIVATE, (WPARAM)hwndEditor, 0 );	 //REVIEW: is this the correct UI method?
	return GetActiveEditor() == hwndEditor;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::GetToolbar(int nTbId)
// Description: Get the HWND of the specified toolbar. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the toolbar, if successful; NULL otherwise.
// Param: nTbId The id of the toolbar: IDTB_SUSHI_STANDARD, IDTB_SUSHI_EDIT, IDTB_VRES_RESOURCE, IDTB_VCPP_DEBUG, IDTB_VCPP_BROWSE, IDTB_VRES_DIALOG, IDDW_CONTROLS, IDDW_GRAPHICS, IDDW_COLORS, IDDW_TEXTTOOL, IDTB_VPROJ_BUILD.
// END_HELP_COMMENT
HWND UIWBFrame::GetToolbar(int nTbId,int pkgId)
{
	long docId = MAKELONG(nTbId,pkgId) ;
	return (HWND)PWnd()->SendMessage(m_msgGetToolbar, docId);
}				

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::ShowDockWindow(UINT id, BOOL bShow /*=TRUE*/) // REVIEW (EnriqueP): Move to CWorkSpace
// Description: Display or hide the specified dockable window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the dockable window, if successful; NULL otherwise.
// Param: id The id of the dockable window: IDW_OUTPUT_WIN
// Param: bShow TRUE to show the window; FALSE to hide it. (Default is TRUE.)
// END_HELP_COMMENT
HWND UIWBFrame::ShowDockWindow(UINT id, BOOL bShow /*=TRUE*/) // REVIEW (EnriqueP): Move to CWorkSpace
{
	UIDockWindow* pudw;
	switch( id )
	{
		case IDW_OUTPUT_WIN:
			pudw = new UIOutput;
			break;
		case IDW_WATCH_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the watch window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UIWatch;
			break;
		case IDW_LOCALS_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the locals window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UILocals;
			break;
		case IDW_CPU_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the registers window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UIRegisters;
			break;
		case IDW_MEMORY_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the memory window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UIMemory;
			break;
		case IDW_CALLS_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the call stack window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UIStack;
			break;
		case IDW_DISASSY_WIN:
			LOG->RecordInfo("UIWBFrame::ShowDockWindow does not open the DAM window. Use UIDebug::ShowDockWindow.");
			return NULL;
//			pudw = new UIDAM;
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
	MST.DoKeys("+{escape}");		// shift escape hides the active docking-window/toolbar
	return NULL;				// should we return newly active window?
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::IsWindowDocked(HWND hWnd, int nLocation)
// Description: Determine if a dockable window is docked. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if the window is docked; FALSE otherwise.
// Param: hWnd The HWND of the dockable window.
// Param: nLocation The location of the dockable window: DW_TOP, DW_BOTTOM, DW_LEFT, DW_RIGHT, DW_DONT_CARE.
// END_HELP_COMMENT
BOOL UIWBFrame::IsWindowDocked(HWND hWnd, int nLocation)   // (enriquep)
{
	EXPECT(IsWindowVisible(hWnd));
	//REVIEW (enriquep): Do we need to verify this is a dockable window?
	
	HWND  hParent = GetParent(hWnd);
	EXPECT(hParent != NULL);
	if ( ::GetWindowLong(hParent, GWL_STYLE) & WS_POPUP)
		return FALSE; 							// Window is floating

	// We know is docked , now check position.
	RECT rcWBClient;

	::GetClientRect( HWnd(), &rcWBClient);
	::MapWindowPoints( HWnd(), HWND_DESKTOP, (POINT*)&rcWBClient, 2);		  	// Client area RECT to screen coord.
	
	RECT rcWBMDIClient;
	HWND hMDIClient = GetClient();					// Returns MDI Client
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


// BEGIN_HELP_COMMENT
// Function: int UIWBFrame::GetEditorCurPos( EGECP x, HWND hwndEditor /*=NULL*/ )
// Description: Get the line or column position of the caret in the editor (depending on x). NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The line or column position of the caret in the window.
// Param: x Value indicating whether to get the line position or column position: GECP_LINE, GECP_COLUMN.
// Param: hwndEditor The HWND of the window to get line or column position for. NULL specifies the active editor window. (Default is NULL.)
// END_HELP_COMMENT
int UIWBFrame::GetEditorCurPos( EGECP x, HWND hwndEditor /*=NULL*/ )
{
//	EXPECT( (x == GECP_LINE) || (x == GECP_COLUMN) );
	HWND hwnd = hwndEditor == NULL ? GetActiveEditor() : hwndEditor;
	hwnd = GetWindow(hwnd, GW_CHILD);		// MDIFrameWnd
	hwnd = GetWindow(hwnd, GW_CHILD);		// view or a scrollbar
	hwnd = GetWindow(hwnd, GW_HWNDLAST);	// view for sure
	return (int)SendMessage(hwnd, m_msgEditorCurPos, x, 0);
}

// BEGIN_HELP_COMMENT
// Function: void UIWBFrame::ExportResource(LPCSTR szFileName, BOOL bOverwrite /*=FALSE*/)
// Description: Export a resource from the IDE> NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: none
// Param: szFileName A pointer to a string that contains the filename to save the exported resource under.
// Param: bOverwrite A value that indicates whether to overwrite an existing file or not. TRUE to overwrite; FALSE to not. (Default is FALSE.)
// END_HELP_COMMENT
void UIWBFrame::ExportResource(LPCSTR szFileName, BOOL bOverwrite /*=FALSE*/)
{
    DoCommand(IDM_RESOURCE_EXPORT, DC_MNEMONIC);

	UIDialog erd(GetLocString(IDSS_ER_TITLE));		// "Export Resource"
	if( !erd.WaitAttachActive(1000) )
	{
		LOG->RecordInfo("Could not open Resource/Export dialog");
		return;
	}
	
    MST.DoKeys(szFileName);
	erd.OK();
	
	UIWBMessageBox mb;
	while( mb.WaitAttachActive(1000) )
	{
		if( mb.ButtonExists(MSG_YES) && bOverwrite )
			mb.ButtonClick(MSG_YES);
		else
			mb.ButtonClick();			
	}
	erd.WaitUntilGone(1000);
	WaitForStatusText(GetLocString(IDSS_READY), 1000);

//  MST.WMenuEnd();     // BUG?: status bar text doesn't revert to 'Ready'
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::ShowPropPage(BOOL bShow)
// Description: Display or hide the property page for the active window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the property page for the active window.
// Param: bShow A Boolean value that indicates whether to show or hide the property page. TRUE to show it; FALSE to hide it.
// END_HELP_COMMENT
HWND UIWBFrame::ShowPropPage(BOOL bShow)
{
	UIControlProp upp;
	if( !upp.AttachActive() )
	{
		DoCommand(IDM_WINDOW_SHOWPROPERTIES, DC_ACCEL);
//		MST.DoKeys("%{ENTER}");		//Give Focus to PropPage
		upp.WaitAttachActive(1000);
	}
	if( bShow == FALSE )
	{
		upp.Close();
//		MST.DoKeys("+{ESC}");		// Hide it 	
		return NULL;			// no HWND of prop page
	}
	else
		return upp;		// HWND of prop page
}

// BEGIN_HELP_COMMENT
// Function: CString UIWBFrame::GetProperty(int nProp)
// Description: Get the property of the item specified by nProp. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A CString containing the property of the item.
// Param: nProp The id of the item.
// END_HELP_COMMENT
CString UIWBFrame::GetProperty(int nProp)   // REVIEW (EnriqueP): Move to CWorkSpace?
{
	CString str = "Empty Property";
	ATOM atom = (ATOM)PWnd()->SendMessage( m_msgGetProp, nProp);
	if (atom != 0)
	{
		UINT ret = GlobalGetAtomName( atom, str.GetBufferSetLength(MAX_ATOM_LENGTH), MAX_ATOM_LENGTH );
		ret = GlobalDeleteAtom(atom);	// this atom created by the Workbench
		str.ReleaseBuffer(-1);
	}
 	return str;
}	

int UIWBFrame::SetProperty(int nProp,LPCSTR propValue)
{
	int ret;
	ATOM atom = GlobalAddAtom(propValue);
	TRACE1("Added Global Atom: %u\n", atom);
	ret=PWnd()->SendMessage( m_msgSetProp, nProp, (LPARAM)atom);
	GlobalDeleteAtom(atom);	// this atom created by the Workbench
	return ret;
}						

// BEGIN_HELP_COMMENT
// Function: CString UIWBFrame::GetProjPath(void)
// Description: Get the current project's path. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A CString containing the path the current project.
// END_HELP_COMMENT
CString UIWBFrame::GetProjPath(void)   // REVIEW (EnriqueP): Move to CWorkSpace?
{	int nProp = 0 ;
	CString str = "Empty Property";
	ATOM atom = (ATOM)PWnd()->SendMessage( m_msgGetProjPath, nProp);
	if (atom != 0)
	{
		UINT ret = GlobalGetAtomName( atom, str.GetBufferSetLength(MAX_ATOM_LENGTH), MAX_ATOM_LENGTH );
		ret = GlobalDeleteAtom(atom);	// this atom created by the Workbench
		str.ReleaseBuffer(-1);
	}
 	return str;
}							

// BEGIN_HELP_COMMENT
// Function: void UIWBFrame::SelectProjItem(int item, LPCSTR szName)
// Description: Select the item specified. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: none
// Param: item The item to select.
// Param: szName The name of the item to select.
// END_HELP_COMMENT
void UIWBFrame::SelectProjItem(int item, LPCSTR szName)
{	
	HWND hProjOptDlg = MST.WGetActWnd(0) ;
	SendMessage(hProjOptDlg,WM_USER +0, (WPARAM) item, (LPARAM)GlobalAddAtom(szName));
}
				
// BEGIN_HELP_COMMENT
// Function: CString UIWBFrame::GetItemProperty(int nProp)
// Description: Get the property of any item. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A CString containing the property.
// Param: nProp The id of the property.
// END_HELP_COMMENT
CString UIWBFrame::GetItemProperty(int nProp)
{
	CString str = "Empty Property";
	ATOM atom = (ATOM)PWnd()->SendMessage( m_msgGetItemProp, nProp);
	if (atom != 0)
	{
		UINT ret = GlobalGetAtomName( atom, str.GetBufferSetLength(512), 512 );
		ret = GlobalDeleteAtom(atom);	// this atom created by the Workbench
		str.ReleaseBuffer(-1);
	}
 	return str;
}

   // Build state
// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::CanWeBuild(void )
// Description: Determine whether the IDE is able to build a project or not. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates whether the IDE is able to build or not. TRUE if the IDE can build; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::CanWeBuild(void )
{
  	int nProp = 0, iCan  ;
	iCan  = PWnd()->SendMessage( m_msgCanWeBuild, nProp);
	return iCan ;
}

// BEGIN_HELP_COMMENT
// Function: int UIWBFrame::GetErrorCount(void)
// Description: Get the number of errors from the last build. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A count of the number of errors from the last build.
// END_HELP_COMMENT
int UIWBFrame::GetErrorCount(void)
{
  	int nProp = 0, iCnt = -1 ;
	iCnt = PWnd()->SendMessage(m_msgGetErrorCount, nProp);
	return iCnt ;
}

// BEGIN_HELP_COMMENT
// Function: int UIWBFrame::GetWarningCount(void)
// Description: Get the number of warnings from the last build. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A count of the number of warnings from the last build.
// END_HELP_COMMENT
int UIWBFrame::GetWarningCount(void)
{
   	int nProp = 0, iCnt = -1 ;	
	iCnt = PWnd()->SendMessage(m_msgGetWarningCount, nProp);
	return iCnt ;
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::ClassWizard(void)
// Description: Display the ClassWizard dialog. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the ClassWizard dialog.
// END_HELP_COMMENT
HWND UIWBFrame::ClassWizard(void)
{
	WaitForInputIdle(g_hTargetProc, 6000);		// In case IDE is busy
    if( !DoCommand(IDMY_CLASSWIZARD, DC_MENU) )
        return NULL;    // Class Wizard wasn't on menu - must not be available
	Sleep(1000);
  	return MST.WGetActWnd(0);	//Attach to the ClassWizard dialog.
}                                                                                          ;

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::ProjectProperties(void)
// Description: Display the project properties. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: NULL
// END_HELP_COMMENT
HWND UIWBFrame::ProjectProperties(void)
{
// REVIEW(Ivan) This has been changed, we nolonger have the Settings menuitem on project.
//	DoCommand (IDM_PROJECT_CONFIGURE, DC_MNEMONIC);
//	WFndWndWaitC("Project Properties", "Static", FW_PART, 10);	// LOCALIZE
	return NULL ; // MST.WGetActWnd(0);
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::Options(void)
// Description: Display the Options dialog in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the Options dialog.
// END_HELP_COMMENT
HWND UIWBFrame::Options(void)
{
	DoCommand(IDM_OPTIONS, DC_MNEMONIC);
	UIOptionsTabDlg uiopt;		
	return uiopt.WaitAttachActive(10000);
}


// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::GetStatusBar(void)
// Description: Get the HWND of the IDE's status bar. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the status bar, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIWBFrame::GetStatusBar(void)
{
	HWND hwndStatusBar = FindFirstChild(HWnd(), ID_STATUS_BAR);
	return hwndStatusBar;
}

// BEGIN_HELP_COMMENT
// Function: CString UIWBFrame::GetStatusText(void)
// Description: Get the text from the IDE's status bar. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A CString containing the text in the status bar.
// END_HELP_COMMENT
CString UIWBFrame::GetStatusText(void)		  // REVIEW (EnriqueP): Move to CWorkSpace
{
    HWND hwnd = GetStatusBar();
    return ::GetText(hwnd);
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact /*=TRUE*/)
// Description: Wait for the given text (sz) to appear in the status bar. This function will wait until the text is displayed in the status bar or the number of milliseconds given in dwMilliSeconds have elapsed. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if the text is found in the status bar before dwMilliSeconds elapses; FALSE otherwise.
// Param: sz A pointer to a string containing the text to wait for in the status bar.
// Param: dwMilliSeconds The number of milliseconds to wait for the text to appear.
// Param: bExact TRUE if the text in status bar must match the text in sz exactly; FALSE for a partial patch.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact /*=TRUE*/)
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
// REVIEW(davidga): this will never work.  Use GetEditorCurPos or COSource::GetCurrentLine/Column


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::VerifyFile(LPCSTR szfile)
// Description: Determine whether the active editor has the given filename in its title. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates whether the active editor has the given filename. TRUE if it does; FALSE otherwise.
// Param: szfile A pointer to a string containing the filename expected in the active editor.
// END_HELP_COMMENT
BOOL UIWBFrame::VerifyFile(LPCSTR szfile)
{
   CString caption;
   HWND hwnd = GetActiveEditor();
   MST.WGetText(hwnd, caption);
   return (((CString) caption).Find((CString)szfile) != (-1));
};


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::VerifyCaretPosition( int nRow, int nCol, LPCSTR szErrTxt )
// Description: Verify the caret position in the active window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if the caret position matches the given row and column; FALSE otherwise.
// Param: nRow The expected row for the caret.
// Param: nCol The expected column for the caret.
// Param: szErrTxt A pointer to a string containing the text to emit to log if the caret position doesn't match the given row and column.
// END_HELP_COMMENT
BOOL UIWBFrame::VerifyCaretPosition( int nRow, int nCol, LPCSTR szErrTxt )
{
    char szBuffer[80];

	EXPECT( nRow >= 0 && nCol >= 0 );
	wsprintf( szBuffer, "Ln %d,Col %d", nRow, nCol );
		
	if (WaitForStatusText( szBuffer, (DWORD) 5000 /* 5 seconds */, TRUE ) )
		return TRUE;
	LOG->Comment( "VerifyCaretPosition: ",szErrTxt,": expected ",szBuffer," <> status bar column ",GetStatusText() );
	return FALSE;
}			

/*************************
**	FVerifyClipboardText
**		-larryh
**	just check the clipboard for the match string.
**
**	Clipboard is unaffected.
**/
// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::FVerifyClipboardText( CString stMatch )
// Description: Determine whether the text in the clipboard matches the given string. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates whether the text in the clipboard matches the given string. TRUE if it does; FALSE otherwise.
// Param: stMatch A CString that contains the text expected in the clipboard.
// END_HELP_COMMENT
BOOL UIWBFrame::FVerifyClipboardText( CString stMatch )
{
	CString	stClip;

	GetClipText( stClip );
	return (stClip == stMatch);
}
/*************************
**	FVerifySelection
**		-larryh
**	after a copy, stTest is compared to the clipboard.
**
**	Clipboard is toasted.
**/
// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::FVerifySelection( CString stTest )
// Description: Determine whether the selection in the active editor matches the given string. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates whether the selection in the active editor matches the given string. TRUE if it does; FALSE otherwise.
// Param: stTest A CString that contains the text expected in the selection.
// END_HELP_COMMENT
BOOL UIWBFrame::FVerifySelection( CString stTest )
{
	CString	stClip;

	// REVIEW larryh: How do we bail if no selection? Should we?
	// REVIEW larryh: allow different windows, defaulting to current
	DoCommand( ID_EDIT_COPY, DC_MNEMONIC );

	GetClipText( stClip );
	return (stClip == stTest);
}

/*************************
**	FVerifyLine
**		-larryh
**	the line at the cursor is selected, and verified
**
**	Clipboard is toasted.
**/

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::FVerifyLine( CString stTest )
// Description: Determine whether the current line in the active editor matches the given string. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates whether the current line in the active editor matches the given string. TRUE if it does; FALSE otherwise.
// Param: stTest A CString that contains the text expected in the current line.
// END_HELP_COMMENT
BOOL UIWBFrame::FVerifyLine( CString stTest )
{
	MST.DoKeyshWnd(HWnd(), KEY_Home  );
	MST.DoKeyshWnd(HWnd(), "+{end}" );
	return FVerifySelection( stTest );
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::AddTBButton(int nButtonType, HWND hwndToolbar /* = NULL */)
// Description: Add a toolbar button (nButtonType) to the toolbar indicated by hwndToolbar. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: TRUE if successful; ERROR_ERROR otherwise.
// Param: nButtonType The button to add to the toolbar (this is NYI).
// Param: hwndToolbar The HWND of the toolbar to add the button to. NULL means create a new toolbar. (Default value is NULL.)
// END_HELP_COMMENT
BOOL UIWBFrame::AddTBButton(int nButtonType, HWND hwndToolbar /* = NULL */)
{
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
        DragMouse(VK_LBUTTON, uDlg, 155, 70, HWnd(), 2, 2);
    }

    return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SetToolbarCombo(LPCSTR str /*NULL*/)
// Description: Set the text in the standard toolbar's combo box. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: str A pointer to a string that contains the text to put in the toolbar's combo box.
// END_HELP_COMMENT
BOOL UIWBFrame::SetToolbarCombo(LPCSTR str /*NULL*/)
{
    MST.DoKeyshWnd(HWnd(),"%(a)");       // review WAYNEBR internationalize this line
    
	int iSec = 0;
	
	while(!MST.WComboExists(""))
	
	{
		if(iSec == 3)
		{
			LOG->RecordInfo("ERROR in UIWBFrame::SetToolbarCombo - toolbar combo didn't get focus within 3 seconds."); 
			return FALSE;
		}

		Sleep(1000);
		iSec++;
	}

	::WaitForInputIdle(g_hTargetProc,2000);

	if (str!=NULL)
        MST.DoKeyshWnd(HWnd(),str,TRUE);
    
	return TRUE;        // then user can do what he/she wishes
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::Break(int WAIT)
// Description: Select Break mode while debugging in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::Break(int WAIT)

{
	// in case the debuggee is a GUI app, activate the ide and wait up to 10 seconds for it to come to the foreground.
	// TODO(michma - 2/25/98): why doesn't the code below work? Activate is failing for some reason.
	/*
	int iSec;

	if(!IsActive())
	
	{
		Activate();

		for(iSec = 0; iSec < 10; iSec++)
		
		{
			Sleep(1000);
		
			if(IsActive())
				break;
		}
	}

	if(iSec == 10)
	{
		LOG->RecordInfo("ERROR in UIWBFrame::Break() - could not activate IDE to issue command.");
		return FALSE;
	}
	*/

	if(!MST.WFndWndWait(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_PART | FW_FOCUS, 10))
	{
		LOG->RecordInfo("ERROR in UIWBFrame::Break() - could not activate IDE to issue command.");
		return FALSE;
	}

	// TODO(michma): for some reason the debugger gets hung when breaking into a GUI app if we choose the menu command 
	// via ms-test's functions like DoKeys (which DoCommand does). so we are working around this by sending the keys
	// more directly.
	// DoCommand(IDM_RUN_BREAK, DC_MNEMONIC);
	CString strDebugBreakHotKeys = ExtractHotKey(GetLocString(IDS_DEBUGMENU) + GetLocString(IDS_BREAK));
	WaitStepInstructions("Sending keys '%%%c%c'", strDebugBreakHotKeys[0], strDebugBreakHotKeys[1]);
	keybd_event(VK_MENU, 0,0,0);
	keybd_event(LOBYTE(VkKeyScan(strDebugBreakHotKeys[0])), 0,0,0);
	keybd_event(LOBYTE(VkKeyScan(strDebugBreakHotKeys[0])), 0,KEYEVENTF_KEYUP,0);
	keybd_event(VK_MENU, 0,KEYEVENTF_KEYUP,0);
	keybd_event(LOBYTE(VkKeyScan(strDebugBreakHotKeys[1])), 0,0,0);
	keybd_event(LOBYTE(VkKeyScan(strDebugBreakHotKeys[1])), 0,KEYEVENTF_KEYUP,0);
	
	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::Go(int WAIT)
// Description: Start debugging the current project or run from a breakpoint. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Run state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::Go(int WAIT)
{
	BOOL nodebugee;
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState currentState;
		m_pEventHandler->m_pDbg->get_State(&currentState);
		nodebugee=(currentState==dsNoDebugee);
	}
	else
	{ //this is when Dbg object model does not work in which case we assign IDebugger* to NULL & revert to polling the title
		nodebugee=(GetIDEState() & ST_NODEBUG);
	}
	if(nodebugee)
	{
		DoCommand(IDM_RUN_GO, DC_ACCEL);
		::WaitForInputIdle(g_hTargetProc,2000);
		// this is to handle possible "does not contain debug info" dlg when starting debugging
		// of a retail exe in order to hit breakpoints in a dll (like controls in internet explorer).
		HWND hmsg;
		if(hmsg=MST.WFndWndWaitC(GetLocString(IDSS_DOES_NOT_CONTAIN_DEBUG_INFO), "Static", FW_PART, 2))
			MST.DoKeyshWnd(hmsg,"{ENTER}");
	}
	else
		DoCommand(IDM_RUN_GO, DC_ACCEL);

	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
		WaitForRun();

	return Wait(WAIT,FALSE);    // Focus bug: do not check the debuggee's window
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::Restart(int WAIT)
// Description: Restart the current debugging session in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state after restarting.
// END_HELP_COMMENT
BOOL UIWBFrame::Restart(int WAIT)

{
	LOG->RecordInfo("starting debugging, checking for find symbols dlg");
	// REVIEW(michma - 3/5/99): DoCommand uses DoKeys and that has severe problems on nt4 sp5. sometimes not all the
	// keystrokes get sent due to apparent journaling hook imcompatibilites with ms-test on that platform.
	//DoCommand(IDM_RUN_RESTART, DC_ACCEL);
	WaitStepInstructions("Sending keys '+({F5})'");
	keybd_event(VK_SHIFT, 0,0,0);
	keybd_event(VK_F5, 0,0,0);
	keybd_event(VK_F5, 0,KEYEVENTF_KEYUP,0);
	keybd_event(VK_SHIFT, 0,KEYEVENTF_KEYUP,0);

	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
		WaitForRun();

	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::StopDebugging(int WAIT)
// Description: Stop the current debugging session in the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for the debugging session to end.
// END_HELP_COMMENT
BOOL UIWBFrame::StopDebugging(int WAIT)
{
	DoCommand(IDM_RUN_STOPDEBUGGING, DC_MNEMONIC);
	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::StepInto(int WAIT)
// Description: Perform a step into in the current debugging session (or start debugging the current project, if not already). NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::StepInto(int WAIT)
{
	BOOL nodebugee;
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState currentState;
		m_pEventHandler->m_pDbg->get_State(&currentState);
		nodebugee=(currentState==dsNoDebugee);
	}
	else
	{	//this is when Dbg object model does not work in which case we assign IDebugger* to NULL & revert to polling the title
		nodebugee=(GetIDEState() & ST_NODEBUG);
	}
	DoCommand(IDM_RUN_TRACEINTO, DC_ACCEL);
	
	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
	{	
		LOG->Comment("UIWBFrame::StepInto - about to call WaitForRun");
		WaitForRun();
	}

	LOG->Comment("UIWBFrame::StepInto - about to call Wait(%d)", WAIT);
	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::StepOver(int WAIT)
// Description: Perform a step over in the current debugging session (or start debugging the current project, if not already). NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::StepOver(int WAIT)
{
	BOOL nodebugee;
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState currentState;
		m_pEventHandler->m_pDbg->get_State(&currentState);
		nodebugee=(currentState==dsNoDebugee);
	}
	else
	{	//this is when Dbg object model does not work in which case we assign IDebugger* to NULL & revert to polling the title
		nodebugee=(GetIDEState() & ST_NODEBUG);
	}
	DoCommand(IDM_RUN_STEPOVER, DC_ACCEL);

	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
		WaitForRun();

	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::StepOut(int WAIT)
// Description: Perform a step out in the current debugging session. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::StepOut(int WAIT)
{
	DoCommand(IDM_RUN_STOPAFTERRETURN, DC_MNEMONIC);

	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
		WaitForRun();

	return Wait(WAIT,FALSE);    // Focus bug: do not check the debuggee's window
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::StepToCursor(int WAIT)
// Description: Perform a step to cursor in the current debugging session (or start debugging the current project, if not already). NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT An integer indicating the amount of time to wait for Break state in the IDE.
// END_HELP_COMMENT
BOOL UIWBFrame::StepToCursor(int WAIT)
{
	// REVIEW (michma): timing problem in ide (not cafe), won't fix.
	Sleep(1000);
	BOOL nodebugee;
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState currentState;
		m_pEventHandler->m_pDbg->get_State(&currentState);
		nodebugee=(currentState==dsNoDebugee);
	}
	else
	{	//this is when Dbg object model does not work in which case we assign IDebugger* to NULL & revert to polling the title
		nodebugee=(GetIDEState() & ST_NODEBUG);
	}
	DoCommand(IDM_RUN_TOCURSOR, DC_ACCEL);

	WaitForInputIdle(g_hTargetProc, 10000);

	// The following is a workaround to bypass a strange occurrence that sometimes happens.  Basically
	// every once in a while when the a breakpoint is hit (doesn't matter if it's a RunToCursor, or
	// a SetBreakpoint/Go/SetBreakpoint combination, the debuggee will break, but not become cleared
	// from the display, this seems to cause problems for the sniffs that use this function.
	MST.DoKeys("{Right}{Left}");

	// shouldn't wait for break or termination until we know that [run]
	// has been displayed. 
	if((WAIT != NOWAIT) && (WAIT != WAIT_FOR_RUN))
		WaitForRun();

	BOOL bSuccess = Wait(WAIT, FALSE);
	return bSuccess;    // Focus bug: do not check the debuggee's window
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::ToggleBreakpoint(void)
// Description: Toggle a breakpoint at the current line in the active editor. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::ToggleBreakpoint(void)             //REVIEW: Add extra functionality to make worthwhile.
{
	MST.DoKeys(KEY_TOGGLE_BREAKPOINT);		   //REVIEW: changed to DoCommand when available.
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SetNextStatement(void)
// Description: Set the next statement to be executed in the debugger to the current line in the current editor. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::SetNextStatement(void)
	{
	EXPECT(IsActive());
	//REVIEW: this command has no menu/accel access, only hotkey. michma
	//DoCommand(IDM_RUN_SETNEXTSTMT, DC_ACCEL);
	MST.DoKeys("^(+({f7}))");
	return TRUE;
	}


// REVIEW(briancr): this needs to be code reviewed
// return the current state of the IDE
// this includes: exists, active, minimized, maximized, restored,
// not debugging, debugging at a breakpoint, debugging running debuggee
// TODO: add support for other state indicators
// (davidga): we need to review the list of useful states
// BEGIN_HELP_COMMENT
// Function: UINT UIWBFrame::GetIDEState(void)
// Description: Get the state of the IDE. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: An bitfield representing the state of the IDE: ST_EXISTS | ST_ACTIVE | ST_MIN | ST_MAX | ST_RES | ST_NODEBUG | ST_DEBUGBREAK | ST_DEBUGRUN | ST_BUILDING.
// END_HELP_COMMENT
UINT UIWBFrame::GetIDEState(void)
{
//	REVIEW(michma - 10/22/97): why are we doing this? it seems to screw up the focus of the
//	ide, and it wasn't like this too long ago. we need to be able to check the ide's
//	title bar without messing with the focus.
//	MST.WSetActWnd(HWnd());
//	XSAFETY;

	UINT state = 0;

#if 1
	//
	//	Check title string to determine debug state.  IsCommandEnabled() has proved to
	//	be unreliable on fast build machines.
	//
	CString szTitleBreak;
	CString szTitleRun;
	szTitleBreak  = "[";
	szTitleBreak += GetLocString(IDSS_DBG_BREAK);
	szTitleBreak += "]";
	szTitleRun  = "[";
	szTitleRun += GetLocString(IDSS_DBG_RUN);
	szTitleRun += "]";

	CString szTitle;
	MST.WGetText(HWnd(), szTitle);

	if( szTitle.Find(szTitleBreak) >= 0 ) {
		state |= ST_DEBUGBREAK;
	}
	else if( szTitle.Find(szTitleRun) >= 0 ) {
		state |= ST_DEBUGRUN;
	}
	else {
		state |= ST_NODEBUG;
	}

#else
	BOOL bStop = IsCommandEnabled(IDM_RUN_STOPDEBUGGING);
	BOOL bBreak = IsCommandEnabled(IDM_RUN_BREAK);
	if( !bStop )
		state |= ST_NODEBUG;
	if( bStop && !bBreak )
		state |= ST_DEBUGBREAK;
	if( bStop && bBreak )
		state |= ST_DEBUGRUN;
#endif

	if( !IsCommandEnabled(IDM_PROJECT_STOP_BUILD) )
		state |= ST_BUILDING;

	return state;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::Wait(int option, BOOL bCheckDebuggeeWindow /*TRUE*/)
// Description: Wait for the IDE debugger to be in a known state. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: option An integer that indicates the type of wait: 0 (no wait), WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION.
// Param: BOOL bLeaveFindSource A Boolean value indicated whether or not to leave the Find Source dlg up at the break.
// END_HELP_COMMENT
BOOL UIWBFrame::Wait(int option, BOOL bCheckDebuggeeWindow /*TRUE*/, BOOL bLeaveFindSource /* FALSE */)  		//TODO: needs to be more robust (time-limits, FALSE returns, etc.)
{
	BOOL Success=FALSE;
	
	switch(option)
	{
		case 0:      /* NOWAIT */
			{
			Success = TRUE;
			break;			
			}
		case WAIT_FOR_RUN:
			{
			Success = WaitForRun();
			break;			
			}
		case WAIT_FOR_BREAK:
			{
			Success = WaitForBreak(bLeaveFindSource);
			break;			
			}
		case WAIT_FOR_TERMINATION:
		case ASSUME_NORMAL_TERMINATION:
			{
			Success = WaitForTermination(); // msgbox removed so needn't dismiss msgbox anymore
			break;
			}
		case WAIT_FOR_EXCEPTION:
			{
			Success = WaitForException();
			break;
			}
		case WAIT_FOR_CODED_BREAK:
			{
			Success = WaitForCodedBreak();
			break;
			}
	}
	
	return Success;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForBreak(BOOL bLeaveFindSource /* FALSE */)
// Description: Wait for the IDE to be in Break state. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: BOOL bLeaveFindSource A Boolean value indicated whether or not to leave the Find Source dlg up at the break.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForBreak(BOOL bLeaveFindSource /* FALSE */)
{
	UIWBMessageBox mb;
	UIDialog db(GetLocString(IDSS_FS_TITLE));		// ExpectedTitle
	CString strBreak	= GetLocString(IDSS_DBG_BREAK);
	CString strBreakAt	= GetLocString(IDSS_DBG_BREAK_AT);
	CString strTitleBreak = "[";
	strTitleBreak += strBreak + "]";

	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		CString strTitleEnc ="[" + GetLocString(IDSS_EC_TITLE) + "]";
		DsExecutionState currentState = dsRunning; //assume it's called when debugee is running
		for (int i=0; i<2*5*60; i++)
		{
			m_pEventHandler->m_pDbg->get_State(&currentState);
			if(currentState==dsBreak)
			{
				if(GetText().Find(strTitleEnc)>0)  
				{	//REVIEW(chriskoz): dbg reports state==dsBreak during EnC build, which is bogus.
					//But it won't change soon; we need to workaround by polling the UIWB window title against [Edit and Continue]
					if(i % 10 ==0)
						LOG->RecordInfo("WaitForBreak: Reached break but Edit & continue state displayed. Waiting for Enc to complete...");
				}
				else
				{
					LOG->RecordInfo("WaitForBreak: Reached break state!");
					break;
				}
			}
			if(i % 10 ==0)
				LOG->Comment("WaitForBreak: didn't reach break state: Waiting additional 5 sec...");
			Sleep(500);
		}
		if(currentState!=dsBreak)
		{
			LOG->RecordFailure("Break state not reached within 5 minutes...");
			return FALSE;
		}
//REVIEW(chriskoz) these are the message boxes I don't want to handle asyncronously in CTarget class
//CTarget handles FindSymbols & FindSource dialogs

		// Handle the case of a dialog (data breakpoint)
		// If "Break at" is in the dialog then it's an expected dialog, otherwise
		// something unexpected has happened, return FALSE.
		// REVIEW(BruceKu): Should we handle the FALSE case better with an EXPECT(FALSE)?
		if( mb.AttachActive() )
		{
			BOOL b = mb.GetMessageText().Find(strBreakAt) >= 0;
			mb.ButtonClick();
			return b;
		}

		//
		//	Handle case where [break] is in title and Edit and Continue dialog is displayed
		//
		UIDialog dbEC(GetLocString(IDSS_EC_TITLE));		// ExpectedTitle
		if(dbEC.AttachActive())
		{
			// Check for Edit and Continue dialog, dismiss it.
			if(dbEC.VerifyTitle())
				return TRUE;
		}

		if( !HasFocus() )
		{  //TODO: unfortunately, we return from break when Find Source or Find Symbol Dialog is still there. Try to get rid of any dialog
			UIDialog db;
			db.AttachActive();
			if(bLeaveFindSource && db.GetText()==GetLocString(IDSS_FS_TITLE))
				return TRUE; //special case for Find Source dialog, which is expected here
			LOG->RecordInfo( "Debugger in break state. But unexpected dialog '%s' has stolen the focus. Hitting ESC & forcing focus to IDE", db.GetText());
			MST.DoKeys("{ESC}");
			this->Activate();
		}
		return TRUE;
	}
	//	While [break] does not appear in the caption 3 times then loop
	//  60 sec     maxTimeOut value so we do not starve.
	int ct = 0;
	int nTSec = 0;
	int nSecMaxWaitForBreak = 60 * 5;	// Wait max of 5 minutes
	while(ct <= 3)
	{
		if( GetText().Find(strTitleBreak) < 0 )
		{
			//
			//	Check dialog( for compatibility of break at dialog up before
			//	break is signalled in title.  Can remove this code when IDE
			//	change is fully implemented across platforms.
			//
			if( db.AttachActive() )
			{
				// Check for FindSource dialog, dismiss it.
				if( db.VerifyTitle() )
					db.Cancel();
				else
				{
					CString msg = "Unexpected dialog box:  ";
					msg += db.GetText();
					msg += " Dismissing.";

					LOG->RecordInfo( msg.GetBuffer(256) );
					MST.DoKeys("{enter}");					// Dismiss Unexpected msgbox
					return FALSE; 							// FAIL due to Unexpected msgbox
				}
			}
			else if( mb.AttachActive() )		// No need to wait since db waited 1 sec.
			{
				BOOL b = mb.GetMessageText().Find(strBreakAt) >= 0;
				mb.ButtonClick();
				if( !b ) {
					return b;
				}
				//
				//	Once dialog is handled, [break] should appear in title and be
				//	handled properly by 'while(ct <= 3)' loop.
				//
			}
	

			// No break yet, in [run] state
			if( nTSec >= nSecMaxWaitForBreak )
			{
				LOG->RecordFailure( "Break state not reached within %d minute(s) %d seconds.  Exitting test..", nSecMaxWaitForBreak/60, nSecMaxWaitForBreak%60);
				EXPECT( FALSE );
			}
			Sleep(1000);		// increment a second - Let target run...
			nTSec++;
		}
		else // Break state
		{	//REVIEW (chriskoz) I have no clue what this idiot "ct" variable is doing here
			//but it shld log "success" message only once
			if(++ct>3)
			LOG->RecordInfo("Found %s in IDE title, don't wait anymore.", strTitleBreak);
		}
	}
	::WaitForInputIdle(g_hTargetProc,3000);
	Sleep(2000);	// Give time for full IDE reactivation.

	//
	//	Handle case where [break] is in title and Find Source dialog is displayed
	//
	if( db.AttachActive() )
	{
		// Check for FindSource dialog, dismiss it.
		if( db.VerifyTitle() )
		{
			if(bLeaveFindSource)
				return TRUE;
			else
				db.Cancel();
		}
		else
		{
			CString msg = "Unexpected dialog box:  ";
			msg += db.GetText();
			msg += " Dismissing.";

			LOG->RecordInfo( msg.GetBuffer(256) );
			MST.DoKeys("{enter}");					// Dismiss Unexpected msgbox
			return FALSE; 							// FAIL due to Unexpected msgbox
		}
	}


	//
	//	Handle case where [break] is in title and Edit and Continue dialog is displayed
	//
	UIDialog dbEC(GetLocString(IDSS_EC_TITLE));		// ExpectedTitle
	if(dbEC.AttachActive())
	{
		// Check for Edit and Continue dialog, dismiss it.
		if(dbEC.VerifyTitle())
			return TRUE;
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForTermination(void)
// Description: Wait for the IDE to terminate the current debugger session. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForTermination(void)
{
	CString szTitleBreak, szTitleRun, text;
	szTitleBreak  = "[";
	szTitleBreak += GetLocString(IDSS_DBG_BREAK);
	szTitleBreak += "]";
	szTitleRun  = "[";
	szTitleRun += GetLocString(IDSS_DBG_RUN);
	szTitleRun += "]";

	MST.WGetText(HWnd(), text);

	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState currentState = dsRunning; //assume it's called when debugee is running
		for (int i=0; i<120; i++)
		{
			m_pEventHandler->m_pDbg->get_State(&currentState);
			if(currentState==dsNoDebugee)
			{
				LOG->RecordInfo("Reached break state: don't wait anymore.");
				break;
			}
			Sleep(500);
		}
		if(currentState!=dsNoDebugee)
		{
			LOG->RecordFailure("Timed out waiting for termination to occur.");
			return FALSE;
		}
		return TRUE;
	}

	int i;
	for(i = 0; i < 10; i++)
	{
		if(text.Find(szTitleRun) == -1 && text.Find(szTitleBreak) == -1)
			break; //neither "break" nor "run"
		LOG->RecordInfo("UIWBFrame::WaitForTermination(): ide title is %s", text);
		MST.WGetText(HWnd(), text);			

		// REVIEW: Waynebr  this should handle hitting an unexpected msgbox
		if (MST.WFndWndWaitC(GetLocString(IDSS_WORKBENCH_TITLEBAR), "#32770", FW_NOCASE | FW_PART,1))
		{
			MST.DoKeys("{enter}");						// Dismiss Unexpected msgbox
			return FALSE; 								// FAIL due to Unexpected msgbox
		}
	}

	if (i == 10)
	{
		LOG->RecordFailure("UIWBFrame::WaitForTermination(): Timed out waiting for termination to occur.");
		return FALSE;				// Timed out waiting for IDE to comeback.
	}

	WaitForInputIdle(g_hTargetProc, 60000);		// WinslowF - Wait for deebugee to close. UI update delays.
	this->Activate();	// WinslowF - If focus problems make sure the DE has the focus.

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForRun(void)
// Description: Wait for the IDE to be in Run state. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForRun(void)
{
	CString szTitleRun, text;
	szTitleRun  = "[";
	szTitleRun += GetLocString(IDSS_DBG_RUN);
	szTitleRun +="]";

#if 0 /* pre-xbox code */
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		DsExecutionState state;
		Sleep(200); //TODO adjust the sleep here

		m_pEventHandler->m_pDbg->get_State(&state);
		if(state==dsRunning)
			return TRUE;
		LOG->RecordInfo("WARNING: WaitForRun(): didn't reach run state within 0.2 sec");
		return FALSE;
	}
#else
	LOG->RecordInfo("WaitForRun(): waiting for dsState to become dsRunning");
	if(m_pEventHandler!=NULL && m_pEventHandler->m_pDbg!=NULL)
	{
		int sleepTotal = 0;
		DsExecutionState state;

		while (sleepTotal < 60000)
		{
			Sleep(200); //TODO adjust the sleep here
			sleepTotal += 200;

			m_pEventHandler->m_pDbg->get_State(&state);
			if(state==dsRunning)
			{
				LOG->RecordInfo("WaitForRun(): dsExecutionState toggled to dsRunning at %d ms", sleepTotal);
				return TRUE;
			}
		}
		LOG->RecordInfo("WARNING: WaitForRun(): didn't reach run state within 6000 ms");
		return FALSE;
	}
#endif

	int i = 0;

	// only wait 1 seconds for debugee to start running
	while(i < 2)
		{
		MST.WGetText(HWnd(), text);
		
		if(text.Find(szTitleRun) != -1)
			break;
				
		Sleep(500);
		if(++i==2)
			LOG->RecordInfo("UIWBFrame::WaitForRun(): ide title is %s", text);
		}

	if(i == 2)
	{
		LOG->RecordInfo("WARNING: UIWBFrame::WaitForRun(): could not find %s "
						"in ide title bar within 1 seconds", szTitleRun);
		return FALSE;
	}
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForException(void)
// Description: Wait for the IDE to catch an exception while debugging. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForException(void)		// exception must be hit within five minutes
																		
	{																	
	if(MST.WFndWndWaitC("Exception", "Static", FW_DEFAULT | FW_PART, 300))
		{
		MST.WButtonClick(GetLabel(MSG_OK));				// Dismiss Exception hit msgbox
		return TRUE;
		}
	
	return FALSE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitForCodedBreak(void)
// Description: This function waits for (up to 1 minute) and ok's a coded break notification msg box (ie. _int 3 for x86, Debugger() for mac)
// Return: returns TRUE if the msg box comes up within 1 minute, FALSE if not.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitForCodedBreak(void)		
																		
	{																	
	if(MST.WFndWndWaitC("User breakpoint called from code at 0x", "Static", FW_PART, 60))
		{
		MST.WButtonClick(GetLabel(MSG_OK));				// Dismiss Exception hit msgbox
		return TRUE;
		}
	
	LOG->RecordInfo("WARNING in UIWBFrame::WaitForCodedBreak(): coded break not detected within 1 minute.");
	return FALSE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::BuildProject(void)
// Description: Build the current project. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::BuildProject(void)

{
	EXPECT(IsValid() & IsActive());
	DoCommand(IDM_PROJITEM_BUILD, DC_MNEMONIC);
	while(GetIDEState() & ST_BUILDING);
	return TRUE;
}

// TODO:this function is out of date.  needs to handle platform-specific project directories
// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::DeleteProject(LPCSTR szProjName, LPCSTR szDir)
// Description: Delete the project with the name szProjName. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szProjName A pointer to a string containing the name of the project to delete.
// Param: szDir A pointer to a string containing the project's directory. NULL means use the current directory.
// END_HELP_COMMENT
BOOL UIWBFrame::DeleteProject(LPCSTR szProjName, LPCSTR szDir)

	{
	WIN32_FIND_DATA wfd;							//TODO: move this to DeleteProject function in wbutil
	HANDLE h;										//		class when its created.
	CString cstrDir = (szDir != NULL) ? szDir : ".";

	if((h = FindFirstFile(cstrDir + "\\" + szProjName + ".*", &wfd)) != INVALID_HANDLE_VALUE)
	
		{
		DeleteFile(cstrDir + "\\" + wfd.cFileName);

		while(FindNextFile(h, &wfd))
			DeleteFile(cstrDir + "\\" + wfd.cFileName);

		FindClose(h);
		}

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::VerifyProjectRan(LPCSTR szProject)
// Description: Determine if the current project ran by checking the active message box for the string "exit code 0." NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szProject A pointer to a string containing the name of the project.
// END_HELP_COMMENT
BOOL UIWBFrame::VerifyProjectRan(LPCSTR szProject)
{
	UIWBMessageBox uiwbmb;
	uiwbmb.WaitAttachActive(1000);
	CString str = uiwbmb.GetMessageText();
	if( str.Find("exit code 0") < 0 )
		return FALSE;

	uiwbmb.ButtonClick(MSG_OK);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::VerifySubstringAtLine( LPCSTR stSubstring, BOOL nCase /* FALSE */)
// Description: Determine if the given string (stSubString) is contained within the current line in the current editor. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: stSubString A pointer to a string that contains the substring to search for.
// END_HELP_COMMENT
BOOL UIWBFrame::VerifySubstringAtLine(LPCSTR stSubstring, BOOL nCase /* FALSE */)

	{
	// Select the current line
	MST.DoKeys("{home}");
	MST.DoKeys("+{end}");

	// if source line in question displays same string
	// over 500 ms, we'll assume it is done painting.
	// we'll only wait 5 seconds though.
	CString last;
	CString current;
	DoCommand(ID_EDIT_COPY, DC_ACCEL);
	GetClipText(last);					// WinslowF remove EXPECT( )

	for(int i = 0; i < 10; i++)
		
		{
		Sleep(500);
		DoCommand(ID_EDIT_COPY, DC_ACCEL);
		GetClipText(current);					// WinslowF remove EXPECT( )

		if((current == last) && (current != ""))
			break;
		
		last = current;
		}


	LOG->RecordInfo("UIWBFrame::VerifySubstringAtLine(): current line = \"%s\".", current);

	if (nCase)							// WinslowF - added for case insensitive surch.
		{
		current.MakeLower();
		CString szTemp = stSubstring;
		szTemp.MakeLower();
		if(current.Find(szTemp) < 0)
			return FALSE;
		else
			return TRUE;
		}
	else
		{
		if(current.Find(stSubstring) < 0)
			return FALSE;
		else
			return TRUE;
		}
	}


// BEGIN_HELP_COMMENT
// Function: int UIWBFrame::GetMemoryWidth(void)
// Description: Get the width of the memory window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: An integer containing the width of the memory window.
// END_HELP_COMMENT
int UIWBFrame::GetMemoryWidth(void)			//must only set width with SetMemoryWidth!			
{
	return m_MemWidth;
}


// BEGIN_HELP_COMMENT
// Function: MEM_FORMAT UIWBFrame::GetMemoryFormat(void)
// Description: Get the format of the memory window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A MEM_FORMAT value containing the format of the memory window: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG.
// Param: Param The parameter description(s) for this function are not available.
// END_HELP_COMMENT
MEM_FORMAT UIWBFrame::GetMemoryFormat(void)		//must only set format with SetMemoryFormat!
	{
	return m_MemFormat;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SetMemoryWidth(int width)
// Description: Set the width of the memory window. This function is NYI. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: width An integer containing the width to set the memory window to.
// END_HELP_COMMENT
BOOL UIWBFrame::SetMemoryWidth(int width)	// sets fixed width
	{
	//TODO
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::SetMemoryFormat(MEM_FORMAT format)	
// Description: Set the format of the memory window. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: format A MEM_FORMAT value specifying the format of the memory window: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG.
// END_HELP_COMMENT
BOOL UIWBFrame::SetMemoryFormat(MEM_FORMAT format)	
	{
	UIOptionsTabDlg uiopt;
	uiopt.Display();
	uiopt.ShowPage(TAB_DEBUG, 6);
	MST.WComboItemClk(uiopt.GetLabel(VCPP32_IDC_DEBUG_FORMAT), format + 1);
	uiopt.OK();
	m_MemFormat = format;
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::WaitOnWndWithCtrl(INT ID, DWORD dwMillSec )
// Description: Wait for a window that has a control with the given ID on it. This function will wait up to dwMillSec milliseconds. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value that indicates success. TRUE if the window was found within the specified time; FALSE otherwise.
// Param: ID The id of the control to wait for.
// Param: dwMillSec The number of milliseconds to wait for the window with the control.
// END_HELP_COMMENT
BOOL UIWBFrame::WaitOnWndWithCtrl(INT ID, DWORD dwMillSec )
{
	//REVIEW(chriskoz) obsolete & stupid wrapper but let's keep it for now (it's called throughout support)
	//would be useful if it returned HWND & restricted the search to the windows owned by the target process
	return (WaitForWndWithCtrl(ID, dwMillSec)!=NULL); // Defined in ..\cafe\src\testutil.cpp
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::FileOpenDlg(void)
// Description: Open the File Open dialog. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the File Open dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIWBFrame::FileOpenDlg(void)
{
	DoCommand(ID_FILE_OPEN, DC_MNEMONIC);
	UIFileOpenDlg ufod;
	ufod.WaitAttachActive(15000);
	return ufod.HWnd();
}

// BEGIN_HELP_COMMENT
// Function: HWND UIWBFrame::FileSaveAsDlg(void)
// Description: Open the File Save As dialog. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: The HWND of the File Save As dialog, if successful; NULL otherwise.
// END_HELP_COMMENT
HWND UIWBFrame::FileSaveAsDlg(void)
{
	DoCommand(ID_FILE_SAVE_AS, DC_MNEMONIC);
	UIFileSaveAsDlg ufsad;
	ufsad.WaitAttachActive(5000);
	return ufsad.HWnd();
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::GetPcodeSwitch(void)
// Description: Get the value of the CAFE PCode switch. NOTE: This function is obsolete; use an applicable function from the CO class that represents the editor you wish to use.
// Return: A Boolean value: TRUE if the switch is set; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIWBFrame::GetPcodeSwitch(void)
{
	CString	cstrSwitch;

//	cstrSwitch = CMDLINE->GetTextValue("pcode");
	cstrSwitch = CMDLINE->GetTextValue("pcode");

	if(cstrSwitch == "yes") 
		return TRUE;
	else
		return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIWBFrame::CheckForFindLocalModuleDlgs(void)
// Description: this function cancels any Find Local Module dlgs that come up after starting debugging (used only for win32s).
// Return: void
// END_HELP_COMMENT
void UIWBFrame::HandleFindLocalModuleDlgs(void)
	{
	// if we don't see a dlg within 5 seconds, we're likely not gonna see any.
	//REVIEW(chriskoz) obsolete - MSTEST4.0 handles the timeout
	//for(int i = 0; i < 5; i++)
	//	{
	//	Sleep(1000);
		if(MST.WFndWnd(GetLocString(IDSS_FIND_LOCAL_MODULE_DLG_TITLE), FW_DEFAULT))
			
			{
			// we saw a dlg withtin 5 seconds, so we're likely to get more over the next 20 seconds.
			for(int j = 0; j < 20; j++)
				{
				if(MST.WFndWndWait(GetLocString(IDSS_FIND_LOCAL_MODULE_DLG_TITLE), FW_DEFAULT,1))
					MST.WButtonClick(GetLocString(IDSS_CANCEL));
				}

//			break;
			}
//		}
	}
