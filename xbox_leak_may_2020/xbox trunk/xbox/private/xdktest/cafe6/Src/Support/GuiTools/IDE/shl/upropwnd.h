///////////////////////////////////////////////////////////////////////////////
//  UPROPWND.H
//
//  Created by :            Date :
//      MichMa              12/01/93
//
//  Description :
//      Declaration of the UIProperties class
//

#ifndef __UIPROPWND_H__
#define __UIPROPWND_H__


#include "shlxprt.h"
#include "uitabdlg.h"


///////////////////////////////////////////////////////////////////////////////
//  UIProperties class


#define METHOD_EDIT 0
#define METHOD_LIST 1

// BEGIN_CLASS_HELP
// ClassName: UIControlProp
// BaseClass: UITabbedDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIControlProp : public UITabbedDialog
{
	UIWND_COPY_CTOR(UIControlProp, UITabbedDialog);

// Utilities
public:
	BOOL IsValid(void) const;

	CString GetID();
	BOOL SetID(CString newID);
	BOOL SetAllPage(void);
	CString getProperty(LPCSTR PropName);
	int putProperty(LPCSTR PropName,LPCSTR szValue,BOOL method=METHOD_EDIT);
    HWND Close(void); //udialog.h version does not work here

};



#endif //__UIPROPWND_H__
