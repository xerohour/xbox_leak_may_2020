///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.H
//
//  Created by :            Date :
//      EnriqueP              1/27/94
//
//  Description :
//      Declaration of the UIOptionsTabDlg class
//

#ifndef __UIOPTDLG_H__
#define __UIOPTDLG_H__

#include "uitabdlg.h"
#include "wbutil.h"

#include "shlxprt.h"
#include "..\sym\vcpp32.h"
#include "..\sym\vproj.h"

#ifndef __UITABDLG_H__
	#error include 'uitabdlg.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIOptionsTabDlg class

//  Tools Options tabs   (These are acually unique control id's found in each page) REVIEW: Use True tab id's
enum{   TAB_WORKSPACE =   20640,	// IDC_STATUSBAR
		TAB_DEBUG =       24100,    // IDC_DEBUG_CODEBYTES
		TAB_COLORS =      24035,    // ID_COLORS_SYNTAX
		TAB_FONTS =       24073,    // IDC_SHOWPROPORTIONAL
		TAB_DIRECTORIES = 16037,	 // IDC_DIRS_LIST
		TAB_COMPATIBILITY = VCPP32_IDC_EMULATION,  // Emulation 
		TABS_TAB = VCPP32_IDC_INDENT_CLOSEBRACE	// The Tabs tab
		} ;

typedef	enum { EMULATE_VC3, EMULATE_VC2, EMULATE_BRIEF, EMULATE_EPSILON, EMULATE_CUSTOM } EmulationType ;

typedef enum {NONE, DEFAULT, SMART, OPEN, CLOSING} Indentation ;
// Control ID's for Directories Page  ( original id's defined in vproj.h)
#define DIR_PLATFORM        VPROJ_IDC_DIRS_TOOLSETS
#define DIR_DIRTYPE         VPROJ_IDC_DIRS_OPTIONS
#define DIR_DIRS_LIST       VPROJ_IDC_DIRS_LIST
#define DIR_ADD             VPROJ_IDC_DIRS_ADD
#define DIR_REMOVE          VPROJ_IDC_DIRS_DELETE
#define DIR_MOVE_UP         VPROJ_IDC_DIRS_MOVEUP
#define DIR_MOVE_DOWN       VPROJ_IDC_DIRS_MOVEDOWN

#define ADD_DIR_NAME            VPROJ_IDC_DIRS_DIRNAME
#define ADD_DIR_BROWSE          16552 /*VPROJ_IDC_DIRS_BROWSE  */
#define ADD_DIR_BUTTON          IDOK

// Indexes for combo box options in Directories Page

#define DIR_PLATFORM_WIN32      1
#define DIR_PLATFORM_DUMMY      2
#define DIR_DIRTYPE_EXE         1
#define DIR_DIRTYPE_INC         2
#define DIR_DIRTYPE_LIB         3


///////////////////////////////////////////////////////////////////////////////
 
// BEGIN_CLASS_HELP
// ClassName: UIOptionsTabDlg
// BaseClass: UITabbedDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIOptionsTabDlg : public UITabbedDialog 
	{
	UIWND_COPY_CTOR (UIOptionsTabDlg, UITabbedDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return GetLocString(IDSS_OD_TITLE); }     // Options
			
			HWND Display(void) ;
			
			
// Directories 
	public:
			int AddDirectory( LPCSTR szNewDir, int iDirType = -1, int iPlatform = -1, int atTop = 0);
			int UIOptionsTabDlg::ShowDirectoryTab()	 ;

// Workspace
			int RestoreWnd( BOOL state = TRUE);
//
			int ShowTabsTab()
			{	Display() ;
				ShowPage(TABS_TAB,12) ;
				return TRUE ;
			}
// Editor
			int SetAutoIndentMode(int );
			int SetSmartIndentOption(int , int PrevLine = 0 );

// Compatibility
			int ChangeCompatility(int Scheme) ;
			int ActivateCompaTab() ;

// Debugger
			int ShowDebugTab()
			{	Display() ;
				ShowPage(0x5E2A,12) ;
				return TRUE ;
			}
 
// Colors

// Fonts
        int SetFont( int FontIndex, int Size = 1, CString Category = "") ;
		int SetFont(LPCSTR pszFontName = NULL, int iFontSize = 0, LPCSTR pszFontStyle = NULL, LPCSTR pszCategory = NULL);

		BOOL ToggleHexDisplay(BOOL bEnable);
		BOOL ToggleSourceAnnotation(BOOL bEnable);
   } ;



#endif //__UIOPTDLG_H__
