///////////////////////////////////////////////////////////////////////////////
//  UCUSTDLG.H
//
//  Created by :            Date :
//      EnriqueP              2/4/94
//
//  Description :
//      Declaration of the UICustomizeTabDlg class
//

#ifndef __UCUSTDLG_H__
#define __UCUSTDLG_H__

#include "uitabdlg.h"					
#include "wbutil.h"
#include "..\sym\vshell.h"
#include "Strings.h"
#include "..\sym\auto1.h"

#include "shlxprt.h"

#ifndef __UITABDLG_H__
	#error include 'uitabdlg.h' before including this file
#endif

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  UICustomizeTabDlg class

//	Tools Options tabs	 (These are acually unique control id's found in each page) REVIEW: Use True tab id's
typedef enum { TAB_COMMANDS = VSHELL_IDC_CATEGORIES , 
			   TAB_TOOLBARS = VSHELL_IDC_TOOLTIPS , 
			   TAB_TOOLS = VSHELL_ID_TOOLS_MENUCONTENTS , 
			   TAB_KEYBOARD = VSHELL_IDC_CAT_LIST,
			   TAB_MACRO_ADDIN = AUTO1_IDC_LIST } CUST_DLG_TAB;



// Control ID's for Toolbars Page  ( original id's defined in vshell.h)

// Control ID's for Tools Page  ( original id's defined in vshell.h)

// Control ID's for Keboard Page  ( original id's defined in vshell.h)


///////////////////////////////////////////////////////////////////////////////
 
// BEGIN_CLASS_HELP
// ClassName: UICustomizeTabDlg
// BaseClass: UITabbedDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UICustomizeTabDlg : public UITabbedDialog 
	{
    UIWND_COPY_CTOR (UICustomizeTabDlg, UITabbedDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return GetLocString(IDSS_CUSTOMIZE_TITLE); }		// Title: "Customize"

			HWND Display(void) ;
			
			
// Toolbars Tab
	public:
			int AddTBButton(int nButtonType, HWND hwndToolbar = NULL );
			HWND GetCurrentMiniPage(void); // REVIEW (EnriqueP) Is this a general utility for all tabs?
			HWND DisplayBars(void);
// Tools Tab

// Keyboard
			HWND DisplayKeyboardTab(void); 
			BOOL SelectKeyAssignment(CString strEditor,CString strCategory,CString strCommand);

// Macros and Add-Ins 
			int ShowAddIn_MacroPg() { ShowPage(TAB_MACRO_ADDIN,5) ; return ERROR_SUCCESS;} ;
			HWND BrowseForMacroFiles() ;


 
   } ;

#endif //__UCUSTDLG_H__
