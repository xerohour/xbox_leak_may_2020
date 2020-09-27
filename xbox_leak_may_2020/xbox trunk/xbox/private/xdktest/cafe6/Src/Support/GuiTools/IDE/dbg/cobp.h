///////////////////////////////////////////////////////////////////////////////
//  COBP.H
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Declaration of the COBreakpoints class
//
	 
#ifndef __COBP_H__
#define __COBP_H__

#include "uibp.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COBreakpoints symbols

enum {COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED};

///////////////////////////////////////////////////////////////////////////////
//  bp (supporting) struct

struct bp
	{
	CString name;
	};

///////////////////////////////////////////////////////////////////////////////
//  COBreakpoints class

// BEGIN_CLASS_HELP
// ClassName: COBreakpoints
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COBreakpoints

	{
	// Data
	private:
		CPtrList bp_list;
		UIBreakpoints uibp;

	// Utilities
	public:

		// obsolete api's kept for backward compatability
		bp *SetBreakpoint(int line, LPCSTR source=NULL, int passcount = 0);
		bp *SetBreakpoint(int line, LPCSTR expression, int option, int length,  int passcount = 0);
		bp *SetBreakpoint(int line, LPCSTR source, LPCSTR expression, int option, int length, int passcount = 0);

		bp *SetBreakpoint(LPCSTR function, LPCSTR source, int passcount = 0);
		bp *SetBreakpoint(LPCSTR function, LPCSTR expression, int option, int length, int passcount = 0);
		bp *SetBreakpoint(LPCSTR function, LPCSTR source, LPCSTR expression, int option, int length, int passcount = 0);

		bp *SetBreakpoint(LPCSTR expression, int option, int length, int passcount = 0);


		// suggested api's
		bp *SetBreakpoint(LPCSTR location = NULL, int passcount = 0);
		bp *SetAmbiguousBreakpoint(LPCSTR function, int ambiguity);
		bp *SetBreakOnMsg(LPCSTR location, LPCSTR message);
		bp *SetBreakOnExpr(LPCSTR location, LPCSTR expression, int expr_type, int length = 1, int passcount = 0);
		bp *SetBreakOnExpr(LPCSTR expression, int expr_type, int length = 1, int passcount = 0);

		// other api's
		BOOL EnableBreakpoint(bp *pbp);
		BOOL EnableAllBreakpoints(void);
		BOOL DisableBreakpoint(bp *pbp);
		BOOL DisableAllBreakpoints(void);
		BOOL RemoveBreakpoint(bp *pbp);
		BOOL ClearAllBreakpoints(void);

		BOOL VerifyBreakpointHit(void);
		BOOL EditCode(bp *pbp, LPCSTR szFileDir = NULL);
		BOOL RemainingPassCountIs(bp *pbp, int iPassCount);

	private:
		bp *AddBPandClose(void);
		bp *FailAndClose(void);
		int FindBPIndex(POSITION pos);
		bp *AddBPToList(void);
	};

#endif // __COBP_H__
