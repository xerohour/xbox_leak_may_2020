///////////////////////////////////////////////////////////////////////////////
//  UIQW.H
//
//  Created by :            Date :
//      MichMa              1/14/94
//
//  Description :
//  	Declaration of the UIQuickWatch class
//

#ifndef __UIQW_H__
#define __UIQW_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "dbgxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIQuickWatch class

// BEGIN_CLASS_HELP
// ClassName: UIQuickWatch
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIQuickWatch : public UIDialog

	{
// base class overrides
public:
	UIWND_COPY_CTOR(UIQuickWatch, UIDialog);
	CString ExpectedTitle(void) const {return GetLocString(IDSS_QW_TITLE);}
	virtual BOOL Activate(void);

	// Utilities
	public:
		
		CString GetExpression(void);
		CString GetNewValue(void);
		CString GetCurrentValue(void);

		BOOL SetExpression(LPCSTR expression);
		BOOL SetNewValue(LPCSTR value); 
		
		BOOL Recalc(void);
		BOOL AddWatch(void);

	// Standard buttons
    	virtual HWND Close(void);
	};

#endif //__UIQW_H__
