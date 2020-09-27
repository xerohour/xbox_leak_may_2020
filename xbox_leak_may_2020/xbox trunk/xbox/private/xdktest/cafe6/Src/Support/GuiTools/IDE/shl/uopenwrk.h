//////////////////////////////
//  UOPENWRK.H
//
//  Created by :            Date :
//      BrianCr				07/12/95
//
//  Description :
//      Declaration of the UIOpenWorkspaceDlg class
//

#ifndef __UOPENWRK_H__
#define __UOPENWRK_H__

#include "ucommdlg.h"
#include "wbutil.h"

#include "shlxprt.h"

#ifndef __UICOMMDLG_H__
   #error include 'ucommdlg.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
// UIOpenWorkspaceDlg class declaration

// BEGIN_CLASS_HELP
// ClassName: UIOpenWorkspaceDlg
// BaseClass: UIFileDlg
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIOpenWorkspaceDlg : public UIFileDlg
{
   	UIWND_COPY_CTOR(UIOpenWorkspaceDlg, UIFileDlg) ;

	virtual CString ExpectedTitle (void) const
		{	return GetLocString(IDSS_OPENWRKDLG_TITLE); }

// Utilities
public:
	BOOL Display(void);
};			

#endif //__UOPENWRK_H__
