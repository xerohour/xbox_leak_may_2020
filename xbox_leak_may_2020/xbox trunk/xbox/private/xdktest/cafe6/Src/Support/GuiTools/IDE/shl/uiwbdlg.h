///////////////////////////////////////////////////////////////////////////////
//  UIWBDLG.H
//
//  Created by :            Date :
//      DavidGa                 9/20/93
//
//  Description :
//      Declaration of the UIWBDialog class
//

#ifndef __UIWBDLG_H__
#define __UIWBDLG_H__

#include "..\..\udialog.h"

#include "shlxprt.h"

#ifndef __UIDIALOG_H__
    #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UIWBDialog class

// BEGIN_CLASS_HELP
// ClassName: UIWBDialog
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIWBDialog : public UIDialog
{
	UIWBDialog(LPCSTR szTitle);
	UIWND_COPY_CTOR(UIWBDialog, UIDialog);

// Data
protected:

// Utilities
public:

// Standard buttons
    virtual HWND Close(void);
    virtual HWND Help(void);
};

#endif //__UIWBDLG_H__
