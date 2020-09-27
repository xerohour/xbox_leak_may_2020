//////////////////////////////
//  UITIPDLG.H
//
//  Created by :            Date :
//      BrianCr				07/12/95
//
//  Description :
//      Declaration of the UITipDialog class
//

#ifndef __UITIPDLG_H__
#define __UITIPDLG_H__

#include "..\..\udialog.h"
#include "wbutil.h"

#include "shlxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UITipDialog class declaration

// BEGIN_CLASS_HELP
// ClassName: UITipDialog
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UITipDialog : public UIDialog
{
   	UIWND_COPY_CTOR (UITipDialog, UIDialog) ;

	virtual CString ExpectedTitle (void) const
		{	return GetLocString(IDSS_TIP_TITLE); }

// Utilities
public:
	BOOL Display(void);
		
};			

#endif //__UITIPDLG_H__
