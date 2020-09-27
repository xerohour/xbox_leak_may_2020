///////////////////////////////////////////////////////////////////////////////
//  UWBFRAME.H
//
//  Created by :            Date :
//      DavidGa                 9/23/93
//
//  Description :
//      Declaration of the UIWBFrame class
//

#ifndef __UIWBFRAME_H__
#define __UIWBFRAME_H__

#include "..\..\uwindow.h"
#include "umainfrm.h"
#include "target.h" //TODO it shoud be moved to some more basic class (like UIWindow)
#include "shlxprt.h"

#ifndef __UIWINDOW_H__
    #error include 'uwindow.h' before including this file
#endif

#define MAKEDOCID(pkgId,wdId) {MAKELONG(pkgId,wdId)}

// forward declaration
class CEventHandler;
enum EGECP { GECP_LINE = 0, GECP_COLUMN = 1 };


// Debug Wait States
enum {  NOWAIT,  
		WAIT_FOR_RUN,
        WAIT_FOR_BREAK,
        WAIT_FOR_TERMINATION,
        ASSUME_NORMAL_TERMINATION,
        WAIT_FOR_EXCEPTION,
		WAIT_FOR_CODED_BREAK};

// memory format options

typedef enum {	MEM_FORMAT_ASCII = 0,
				MEM_FORMAT_WCHAR,
				MEM_FORMAT_BYTE,
				MEM_FORMAT_SHORT,
				MEM_FORMAT_SHORT_HEX,
				MEM_FORMAT_SHORT_UNSIGNED,
				MEM_FORMAT_LONG,
				MEM_FORMAT_LONG_HEX,
				MEM_FORMAT_LONG_UNSIGNED,
				MEM_FORMAT_REAL,
				MEM_FORMAT_REAL_LONG,
				MEM_FORMAT_INT64,
				MEM_FORMAT_INT64_HEX,
				MEM_FORMAT_INT64_UNSIGNED
			} MEM_FORMAT;
			
			
///////////////////////////////////////////////////////////////////////////////
//  UIWBFrame class

// BEGIN_CLASS_HELP
// ClassName: UIWBFrame
// BaseClass: UIMainFrm
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIWBFrame : public UIMainFrm
{
    UIWND_COPY_CTOR(UIWBFrame, UIMainFrm);
	operator BOOL ( ) {return (BOOL) HWnd();} ;
	operator HWND ( ) {return HWnd();} ;

    virtual void OnUpdate(void);
	virtual BOOL Find(void);

// Attributes
public:
    virtual BOOL IsValid(void) const;
    inline HWND GetClient(void) const
        {   return m_hwndClient; }

// Data
private:
    HWND m_hwndClient;
    static CMD_STRUCT m_aCmdTable[];    // filled out in CMDARRAY.CPP
//    static CMD_STRUCT m_aCmdTableJ[];    // filled out in CMDARRYJ.CPP
    static UINT m_msgGetProp;
    static UINT m_msgSetProp;
    static UINT m_msgGetItemProp;
    static UINT m_msgSetItemProp;
	static UINT m_msgGetProjPath;
    static UINT m_msgGetToolbar;
    static UINT m_msgEditorCurPos;
	
	// Build state
	static UINT m_msgCanWeBuild;
	static UINT m_msgGetErrorCount;
	static UINT m_msgGetWarningCount;

	// stores format currently used by Memory window. 
	// used by UIMemory and COMemory utility classes.
	// use UIWBFrame::SetMemoryFormat() to set it, and
	// UIWBFrame::GetMemoryFormat() to get it.

	MEM_FORMAT m_MemFormat;

	// width of memory window in format units
	int m_MemWidth;

// Generic Utilities
public:
    BOOL IsDebug(void);
	BOOL IsActive(void);
    CString GetVersion(void);
    BOOL IsAlert(void);
	UINT GetIDEState(void);
	BOOL WaitOnWndWithCtrl(INT ID, DWORD dwMillSec ) ;

// File Utilities
public:
    HWND CreateNewFile(LPCSTR szFileType);
    HWND OpenFile(LPCSTR szFileName, LPCSTR szChDir = NULL);
    BOOL SaveFileAs(LPCSTR szFileName, BOOL bOverwrite = TRUE);
// REVIEW(briancr): not shell general
    void CloseAllWindows(void);
	BOOL SaveAll(int intWaitInMilliSecs = 0);

// Project Utilities
public:

	BOOL BuildProject(void);
	BOOL CloseProject(void);
	BOOL DeleteProject(LPCSTR szProjName, LPCSTR szDir = NULL);
	BOOL GetPcodeSwitch(void);

// MDI Utilities
public:
    HWND GetActiveEditor(void);
    BOOL SetActiveEditor(HWND hwndEditor);
    HWND GetToolbar(int nTbId,int pkgId);
    HWND GetStatusBar(void);
    CString GetStatusText(void);
    BOOL WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact = TRUE);
	HWND ShowDockWindow(UINT id, BOOL bShow = TRUE);
	BOOL IsWindowDocked(HWND hWnd, int nLocation = 0x000F);	
	int GetEditorCurPos( EGECP x, HWND hwndEditor = NULL );

	BOOL VerifyFile(LPCSTR szfile);
	BOOL VerifyCaretPosition(int nRow, int nCol, LPCSTR szErrMsg );
	BOOL FVerifyClipboardText( CString stMatch );
	BOOL FVerifySelection( CString stMatch );
	BOOL FVerifyLine( CString stMatch );
	BOOL VerifySubstringAtLine( LPCSTR stSubstring, BOOL nCase = FALSE);
		

// Resource Utilities
public:
    void ExportResource(LPCSTR szFileName, BOOL bOverwrite = FALSE);
    HWND ShowPropPage(BOOL bShow);    //REVIEW: add param to push pin if this feature stays
    CString GetProperty(int nProp);
	int SetProperty(int nProp,LPCSTR propValue);

	// Project Window utilities
	CString GetProjPath(void ) ;
	CString GetItemProperty(int nProp);
	void SelectProjItem(int item, LPCSTR szName);

   // Build state
   BOOL CanWeBuild(void );
   int GetErrorCount(void);
   int GetWarningCount(void);
// Dialogs
public:
    HWND ClassWizard(void);
    HWND ProjectProperties(void);
	HWND BreakpointsDlg(void);
	HWND Options(void);
	HWND FileOpenDlg(void);
	HWND FileSaveAsDlg(void);

// UI Utility Fuctions
public:
    BOOL AddTBButton(int nButtonType, HWND hwndToolbar = NULL);
    BOOL SetToolbarCombo(LPCSTR str = NULL);

// Debugger Utilities
public:
	BOOL Go(int WAIT);
	BOOL Restart(int WAIT=0);
	BOOL StopDebugging(int WAIT=0);
	BOOL Break(int WAIT= WAIT_FOR_BREAK);
	BOOL StepInto(int WAIT);
	BOOL StepOver(int WAIT);
	BOOL StepOut(int  WAIT);
	BOOL StepToCursor(int WAIT);
	BOOL ToggleBreakpoint(void);
	void HandleFindLocalModuleDlgs(void);
	BOOL Wait(int WAIT,BOOL bCheckDebuggeeWindow = TRUE, BOOL bLeaveFindSource = FALSE);
	BOOL WaitForBreak(BOOL bLeaveFindSource = FALSE);
	BOOL WaitForTermination(void);
	BOOL WaitForRun(void);
	BOOL WaitForException(void);
	BOOL WaitForCodedBreak(void);
	BOOL VerifyProjectRan(LPCSTR szProject);
	BOOL SetNextStatement(void);
	BOOL SetMemoryWidth(int width);
	BOOL SetMemoryFormat(MEM_FORMAT format);
	MEM_FORMAT GetMemoryFormat(void);
	int GetMemoryWidth(void);

	CEventHandler	*m_pEventHandler; //implements the connection point to IApplicationEvent
	CTarget			*m_pTarget; //implements pointer to IDebugger interface (to fire DebugState polling)
};

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// REVIEW(chriskoz): global pointer to the UIWBFrame is a little better (you can substitute it)
// REVIEW(chriskoz): there is no way to insert it to CTarget class (where it logically belongs)
#define UIWB (*g_pUIWB)
extern SHL_DATA UIWBFrame *g_pUIWB;

///////////////////////////////////////////////////////////////////////////////
// UIWBFrame #defines

//#define WORKBENCH_CLASSNAME "Afx:"      // Afx generated name starts like this
//#define WORKBENCH_TITLEBAR  "Microsoft Visual C++"


#define KEY_TOGGLE_BREAKPOINT "{F9}"

#define KEY_Insert		"{insert}"
#define KEY_Ctrl		"{ctrl}"
#define KEY_Home		"{home}"
#define KEY_Shift		"{shift}"
#define KEY_End			"{end}"
#define KEY_Delete		"{delete}"
#define KEY_Enter		"{enter}"
#define KEY_Tab			"{tab}"

// File adding options for CreateNewProject()

#define CNP_ADD_ALL_SOURCE		0x00000001
#define CNP_ADD_ALL_HEADERS		0x00000002
#define CNP_ADD_ALL_RES_SCRIPTS	0x00000004
#define CNP_ADD_ALL_DEF_FILES	0x00000008
#define CNP_ADD_ALL_ODL_FILES	0x00000010
#define CNP_ADD_ALL_LIBS		0x00000020
#define CNP_ADD_ALL_OBJ_FILES	0x00000040
#define CNP_ADD_ALL_FILES		0x00000080
#define CNP_ADD_FILE			0x00000100
#define CNP_ADD_FILES_MYSELF    0x00000200

//Mnemonics for Set Includes dialog


//  Resource type keys for New Resource listbox and Editor Title bar strings


// state values for GetIDEState
#define ST_EXISTS       0x0001
#define ST_ACTIVE       0x0002
#define ST_MIN          0x0004
#define ST_MAX          0x0008
#define ST_RES          0x0010
#define ST_NODEBUG      0x0020
#define ST_DEBUGBREAK	0x0040
#define ST_DEBUGRUN		0x0080
#define ST_BUILDING		0x0100

// Position values used for IsWindowDocked() and DockWindowAt()
#define DW_TOP					0x0001
#define DW_BOTTOM				0x0002
#define DW_LEFT					0x0004
#define DW_RIGHT				0x0008
#define DW_DONT_CARE		   	0x000F				

// Popup menu IDs
#define MENU_FILE		1	// REVIEW: These should be safe menu IDs
#define MENU_EDIT		2
#define MENU_SEARCH		3
#define MENU_PROJECT	4
#define MENU_RESOURCE	5
#define MENU_DEBUG		6
#define MENU_TOOLS		7
#define MENU_WINDOW		8
#define MENU_HELP		9
#define MENU_IMAGE		10
#define MENU_LAYOUT		11
#define	MENU_INSERT		12
#define MENU_BUILD		13
#define MENU_VIEW		14

#define IDM_BUILD_DEBUG_GO			0xfffe
#define IDM_BUILD_DEBUG_TRACEINTO	0xfffd
#define IDM_BUILD_DEBUG_TOCURSOR	0xfffc
#define IDM_BUILD_DEBUG_ATTACH		0xfffb


#define IDM_VIEW_WATCH	    0xfffa
#define IDM_VIEW_VARS	    0xfff9
#define IDM_VIEW_REGS	    0xfff8
#define IDM_VIEW_MEM	    0xfff7
#define IDM_VIEW_STACK	    0xfff6
#define IDM_VIEW_ASM	    0xfff5

#define IDM_CONNECTION	    0xfff4
#define IDM_VIEW_WORKSPACE  0xfff3
#define IDM_VIEW_GLOBALWORKSPACE  0xfff2

#define ID_STATUS_BAR		0xe801

#endif //__UIWBFRAME_H__
