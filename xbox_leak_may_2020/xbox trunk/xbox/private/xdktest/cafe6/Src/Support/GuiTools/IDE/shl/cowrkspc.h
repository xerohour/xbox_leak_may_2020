///////////////////////////////////////////////////////////////////////////////
//  CWRKSPCE.H
//
//  Created by :            Date :
//      EnriqueP                 1/7/94
//
//  Description :
//      Declaration of the COWorkSpace class
//

#ifndef __CWRKSPCE_H__
#define __CWRKSPCE_H__

#include "..\..\uwindow.h"

#include "shlxprt.h"

#ifndef __UIWINDOW_H__
    #error include 'uwindow.h' before including this file
#endif


///////////////////////////////////////////////////////////////////////////////
//  COWorkSpace class

// BEGIN_CLASS_HELP
// ClassName: COWorkSpace
// BaseClass: none
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS COWorkSpace 
{
// Attributes

// Data
private:
    
// Generic Utilities
public:
    

//Utilities
public:
	void CloseAllWindows(void);
	BOOL IsToolbarVisible(UINT id);
	BOOL IsDockWindowVisible(UINT id);
//  int ShowDockWindow(UINT id, BOOL bShow = TRUE);
	int ShowToolbar(UINT id, BOOL bShow = TRUE);
	BOOL IsWindowDocked(UINT id, int nLocation = 0x000F /*DW_DONT_CARE*/); // REVIEW:(EnriqueP)Use enum value for ID
	BOOL IsToolbarDocked(UINT id, int nLocation = 0x000F /*DW_DONT_CARE*/);
	CString GetStatusText(void);
    BOOL WaitForStatusText(LPCSTR sz, DWORD dwMilliSeconds, BOOL bExact = TRUE);
	BOOL VerifyCaretPosition( int nRow, int nCol, LPCSTR szErrTxt );
	BOOL ShowPropPage(BOOL bShow);    //REVIEW: add param to push pin if this feature stays

// Added Feb 22,95 GeorgeCh
	BOOL AddTool(CString strMenuText,  
				 CString strToolPath, // full path for command name
				 CString strArguments,
				 CString strInitDir,
				 BOOL PromptArgument,
 				 BOOL RedirectOutput,
				 BOOL CloseOnExit);
	BOOL DeleteTools(int Number);
   
//  Not Yet Implemented
//	BOOL DockWindowAt(HWND hWnd, int nLocation = PREV, BOOL bDock = TRUE);
//  BOOL MoveWindow( HWND hWnd, POINT point);
//	BOOL MakeViewDockable(int nViewID, BOOL bDockable = TRUE);

//  CString GetProperty(int nProp);

};

// REVIEW(briancr): this object is declared as a global for backward compatibility
// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
extern SHL_DATA COWorkSpace WS;

///////////////////////////////////////////////////////////////////////////////
// COWorkSpace #defines
// Position values used for IsWindowDocked() and DockWindowAt()
/*#define DW_TOP					0x0001
#define DW_BOTTOM				0x0002
#define DW_LEFT					0x0004
#define DW_RIGHT				0x0008
#define DW_DONT_CARE		   	0x000F */



 
#endif //__CWRKSPCE_H__
