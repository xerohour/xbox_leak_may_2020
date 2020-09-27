///////////////////////////////////////////////////////////////////////////////
//  UINSCTRL.H
//
//  Created by :            Date :
//      MichMa					5/28/97
//
//  Description :
//  	Declaration of the UIInsertCtrlDlg class
//

#ifndef __UINSCTRL_H__
#define __UINSCTRL_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "export.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIInsertCtrlDlg class

// BEGIN_CLASS_HELP
// ClassName: UIInsertCtrlDlg
// BaseClass: UIDialog
// Category: Editor
// END_CLASS_HELP
class SRC_CLASS UIInsertCtrlDlg : public UIDialog

	{
// base class overrides
public:
	UIWND_COPY_CTOR(UIInsertCtrlDlg, UIDialog);
	CString ExpectedTitle(void) const {return GetLocString(IDSS_INSERT_CTRL_DLG_TITLE);}
	virtual BOOL Activate(void);

	// Utilities
	public:
		BOOL SetControl(LPCSTR szCtrl);
	};

#endif //__UINSCTRL_H__
