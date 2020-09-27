///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.H
//
//  Created by :            Date :
//      EnriqueP              2/7/94
//
//  Description :
//      Declaration of the UITabbedDialog class
//

#ifndef __UITABDLG_H__
#define __UITABDLG_H__

#include "..\..\udialog.h"

#include "shlxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

#define ERROR_ERROR -1
#define TAB_CURRENT  		-1     // Used by ShowPage to leave the current tab visible

///////////////////////////////////////////////////////////////////////////////
//  UITabbedDialog class

// BEGIN_CLASS_HELP
// ClassName: UITabbedDialog
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UITabbedDialog : public UIDialog
{
    UIWND_COPY_CTOR (UITabbedDialog, UIDialog) ; 

// Overrided Utilities
public:
	virtual BOOL VerifyClass(void) const;
	virtual LPCSTR GetLabel(UINT nCtrlId) const;
	virtual HWND GetDlgItem(UINT id) const;

// Utilities
// The utilites in the class assume that the tabbed dialog is already displayed and has focus.
public:
	HWND hActive ;

//		virtual BOOL IsValid(void) const;
	virtual HWND Display(void) {return NULL; }; // Should be implemented by all deriving classes	
	virtual int SetOption(INT iTab, UINT iCtrlId, UINT iNewValue) ;
	virtual int SetOption(INT iTab, UINT iCtrlId, LPCSTR szNewValue) ;
	virtual int GetOption(INT iTab, UINT iCtrlId);
	virtual LPCSTR GetOptionStr(INT iTab, UINT iCtrlId);
	virtual HWND ShowPage(int iCtrlID, int iTabCount) ;	// Used if Tab caption can't be read
	virtual HWND ShowPage(LPCSTR szTabCaption) ;   // Used fot Tabs that have a window caption.
	virtual HWND GetCurrentPage(void) const;
	virtual HWND GetCurrentMiniPage(void) const;
	virtual BOOL IsControlInCurrentPage(UINT iCtrlId) const;
	virtual HWND ShowNextPage(int iCtrlID);
		
//	protected:
	virtual HWND ActivateNextPage(BOOL bRight = TRUE) ;
   } ;

#endif //__UITABDLG_H__
