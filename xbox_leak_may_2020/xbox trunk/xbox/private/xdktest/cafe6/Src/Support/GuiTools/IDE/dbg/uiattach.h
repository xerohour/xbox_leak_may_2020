///////////////////////////////////////////////////////////////////////////////
//  UIATTACH.H
//
//  Created by:		MichMa		
//	Date:			2/18/98
//  Description:	Declaration of the UIAttachToProcessDlg class
//

#ifndef __UIATTACH_H__
#define __UIATTACH_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"
#include "dbgxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif


///////////////////////////////////////////////////////////////////////////////
//  UIAttachToProcessDlg class

// BEGIN_CLASS_HELP
// ClassName: UIAttachToProcessDlg
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP

class DBG_CLASS UIAttachToProcessDlg : public UIDialog

{
// base class overrides
public:	
	UIWND_COPY_CTOR(UIAttachToProcessDlg, UIDialog);
	virtual CString ExpectedTitle (void) const{return GetLocString(IDSS_ATTACH_TO_PROCESS_DLG_TITLE);}
	virtual BOOL Activate(void);
	virtual HWND OK(void);

// Utilities
public:
	BOOL ShowSystemProcesses(BOOL bShowSystemProcesses);
	BOOL SelectProcess(LPCSTR szProcessName);
};

#endif // __UIATTACH_H__
