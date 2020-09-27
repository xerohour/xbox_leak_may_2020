///////////////////////////////////////////////////////////////////////////////
//  UIVAR.H
//
//  Created by :            Date :
//      MichMa              	10/20/94
//
//  Description :
//		Declaration of the UIVariables class
//

#ifndef __UIVAR_H__
#define __UIVAR_H__

#include "uieewnd.h"

#ifndef __UIEEWND_H__
	#error include 'uieewnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIVariables class


#define PANE_THIS	IDSS_VAR_PANE_THIS
#define PANE_AUTO	IDSS_VAR_PANE_AUTO
#define PANE_LOCALS	IDSS_VAR_PANE_LOCALS

#define TOTAL_VAR_PANES 3

// BEGIN_CLASS_HELP
// ClassName: UIVariables
// BaseClass: UIEEWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIVariables : public UIEEWindow
	
	{
	UIWND_COPY_CTOR(UIVariables, UIEEWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const {return IDW_LOCALS_WIN;}
	//todo: needs its own id

	// Public utilities
	public:
		int SetValue(LPCSTR value, int row = ROW_CURRENT);
		int SetPane(int pane);
		BOOL SelectLocal(LPCSTR szLocal);
		int GetLocalsCount(void);

		BOOL SelectRow(LPCSTR szName);
		int GetRowCount(void);

		BOOL SelectAuto(LPCSTR szName);
		int GetAutoCount(void);

		// The following functions are intended to completely mimic that of the same functions in UIStack.  
		CString GetFunction(int level = 0);
		CString GetAllFunctions(void);
		BOOL NavigateStack(int level = 0);
	};

#endif //__UIVAR_H__
