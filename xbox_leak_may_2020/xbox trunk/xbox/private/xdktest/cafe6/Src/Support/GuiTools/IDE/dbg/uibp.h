///////////////////////////////////////////////////////////////////////////////
//  UIBP.H
//
//  Created by :            Date :
//      MichMa              1/14/94
//
//  Description :
//      Declaration of the UIBreakpoints class
//

#ifndef __UIBP_H__
#define __UIBP_H__

#include "..\..\udialog.h"
#include "..\shl\wbutil.h"

#include "dbgxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIBreakpoints symbols

enum {  UIBP_TYPE_LOC=1,
		UIBP_TYPE_LOC_IF_EXP_TRUE,
		UIBP_TYPE_LOC_IF_EXP_CHANGED,
		UIBP_TYPE_IF_EXP_TRUE,
		UIBP_TYPE_IF_EXP_CHANGED,
		UIBP_TYPE_WNDPROC_IF_MSG_RECVD };

enum {  BK_MESSAGE=1, BK_CLASS };

/// Control IDs for the new Breakpoints dialog
enum {                                  // From \olympus\ide\src\rsc\resource.h
		UIBP_LOCATION_EDIT = 24134,     // IDC_LOCATION
//		UIBP_LOCATION_TEXT = 24143,     // IDC_BPTAB_GEN1
//		UIBP_EXPRESSION_EDIT = 24135,   // IDC_EXPRESSION
		UIBP_LOCATION_EXPRESSION = 25449,   // Not in rsc\resource.h
		UIBP_DATA_EXPRESSION = 25453,   // Not in rsc\resource.h
//		UIBP_IS_TRUE_BUTTON = 24141,    // IDC_TRUE
//		UIBP_CHANGES_BUTTON = 24142,    // IDC_CHANGES
		UIBP_WINDPROC_COMBO = 24138,    // IDC_WNDPROC
		UIBP_WINDPROC_EDIT = 1001,      // Not in rsc\resource.h
		UIBP_MESSAGE_COMBO = 24137,     // IDC_MESSAGE
		UIBP_MESSAGE_EDIT = 1001,       // Not in rsc\resource.h
//		UIBP_PASSCOUNT_EDIT = 24139,    // IDC_SKIP
//		UIBP_LENGTH_EDIT = 24136,       // IDC_LENGTH
		UIBP_SKIP_COUNT = 25450,		// Not in rsc\resource.h
		UIBP_NUMBER_OF_ELEMENTS = 25460,    // Not in rsc\resource.h
		UIBP_BREAKPOINTS_LIST = 2000,   // Not in rsc\resource.h
		UIBP_OK_BTN = 1,                // Standard ctrl-ID
		UIBP_CANCEL_BTN = 2,            // Standard ctrl-ID
		UIBP_HELP_BTN = 57669,          // Standard ctrl-ID
		UIBP_EDITCODE_BTN = 2001,       // Not in rsc\resource.h
		UIBP_REMOVE_BTN = 2002,         // Not in rsc\resource.h
		UIBP_CONDITION_BTN = 25452,     // Not in rsc\resource.h
	  };


///////////////////////////////////////////////////////////////////////////////
//  UIBreakpointCondition class

// BEGIN_CLASS_HELP
// ClassName: UIBreakpointCondition
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIBreakpointCondition : public UIDialog

	{
// base class overrides
public:
	UIWND_COPY_CTOR (UIBreakpointCondition, UIDialog) ;
	virtual CString ExpectedTitle (void) const{return GetLocString(IDSS_BP_CONDITION_TITLE);}
	virtual BOOL Activate(void);

	// Utilities
	public:

		BOOL SetExpression(LPCSTR expression, int expr_type);
		BOOL SetLength(LPCSTR length);
		BOOL SetPassCount(LPCSTR passcount);
//		BOOL ExpressionTypeIs(int expr_type);

	// Standard buttons
//		virtual HWND Close(void);
	};

///////////////////////////////////////////////////////////////////////////////
//  UIBreakpoints class

// BEGIN_CLASS_HELP
// ClassName: UIBreakpoints
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIBreakpoints : public UIDialog

	{
// base class overrides
public:
	UIWND_COPY_CTOR (UIBreakpoints, UIDialog) ;
	virtual CString ExpectedTitle (void) const{return GetLocString(IDSS_BP_TITLE);}
	virtual BOOL Activate(void);

	// Utilities
	public:

//		BOOL SetType(int type);
		BOOL SetLocation(LPCSTR location);
//		BOOL SetExpression(LPCSTR expression, int expr_type, BOOL fForceType = TRUE);
		BOOL SetLocationExpression(LPCSTR expression, int expr_type);
		BOOL SetDataExpression(LPCSTR expression, int expr_type);
		BOOL SetExprLength(LPCSTR length);
		BOOL SetLocationExprLength(LPCSTR length);
		BOOL SetPassCount(LPCSTR passcount);
		BOOL SetCurrentBreakpoint(int bp = 0);
		BOOL SetWndProc(LPCSTR location);
		BOOL SetMessage(LPCSTR message, int type);
		
//		BOOL Add();
		BOOL Remove(int bp = 0);
		BOOL Enable(int bp = 0);
		BOOL Disable(int bp = 0);
		BOOL ClearAll();

		int GetCount(void);
		CString GetBreakpointStr(int bp = 0);

		BOOL BreakpointIsSet(LPCSTR expr, LPCSTR source = NULL);
		BOOL BreakpointIsEnabled(LPCSTR expr, LPCSTR source = NULL);

		BOOL EditCode(void);

	// Standard buttons
		virtual HWND Close(void);

	private:
		UIBreakpointCondition uibpcond;
	};


#endif //__UIBP_H__
