/*
	Copyright 1996, Microsoft Corp.

  autodual.cpp

  This file contains dual interface definitions, which simply delegate
	to implementations in autodbg.cpp, autobp.cpp, & autobps.cpp.
*/

#include "stdafx.h"
#include <utilauto.h> //Shell Automation Utilities.
#include <objmodel\dbgdefs.h>
#include "Autodbg.h"
#include "autobp.h"
#include "autobps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/* IDebugger */

// properties

DELEGATE_GET(CAutoDebugger,Debugger,Application,LPDISPATCH)
DELEGATE_GET(CAutoDebugger,Debugger,Parent,LPDISPATCH)
DELEGATE_GET(CAutoDebugger,Debugger,Breakpoints,LPDISPATCH)
DELEGATE_GET(CAutoDebugger,Debugger,DefaultRadix,long)
DELEGATE_PUT(CAutoDebugger,Debugger,DefaultRadix,long)
DELEGATE_GET(CAutoDebugger,Debugger,State,DsExecutionState)
DELEGATE_BOOL_GET(CAutoDebugger,Debugger,JustInTimeDebugging)
DELEGATE_BOOL_PUT(CAutoDebugger,Debugger,JustInTimeDebugging)
DELEGATE_BOOL_GET(CAutoDebugger,Debugger,RemoteProcedureCallDebugging)
DELEGATE_BOOL_PUT(CAutoDebugger,Debugger,RemoteProcedureCallDebugging)

// methods

DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,Go)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,StepInto)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,StepOver)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,StepOut)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,Restart)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,Stop)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,Break)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,ShowNextStatement)
DELEGATE_NO_PARAMS(CAutoDebugger,Debugger,RunToCursor)
DELEGATE_ONE_PARAM(CAutoDebugger,Debugger,SetNextStatement,VARIANT)

STDMETHODIMP CAutoDebugger::XDebugger::Evaluate(THIS_ BSTR bstrText, BSTR* pRet)
{
	METHOD_PROLOGUE(CAutoDebugger, Dispatch)

	TRY_DUAL(IID_IDebugger)
	{
		CString str(bstrText); // translate to LPCTSTR
		*pRet = pThis->Evaluate(str);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


/* IBreakpoint */

// properties

DELEGATE_GET(CAutoBreakpoint,Breakpoint,Application,LPDISPATCH)
DELEGATE_GET(CAutoBreakpoint,Breakpoint,Parent,LPDISPATCH)
DELEGATE_GET(CAutoBreakpoint,Breakpoint,Elements,long)
DELEGATE_GET(CAutoBreakpoint,Breakpoint,PassCount,long)
DELEGATE_GET(CAutoBreakpoint,Breakpoint,Message,long)
DELEGATE_GET(CAutoBreakpoint,Breakpoint,Type,long)

DELEGATE_BOOL_GET(CAutoBreakpoint,Breakpoint,Enabled)
DELEGATE_BOOL_PUT(CAutoBreakpoint,Breakpoint,Enabled)

DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,WindowProcedure,WindowProcedure)
DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,Location,Location)
DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,File,File)
DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,Function,Function)
DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,Executable,Executable)
DELEGATE_BSTR_GET(CAutoBreakpoint,Breakpoint,Condition,Condition)
DELEGATE_BSTR_PUT(CAutoBreakpoint,Breakpoint,Condition,Condition)

DELEGATE_NO_PARAMS(CAutoBreakpoint,Breakpoint,Remove)

/* IBreakpoints */

// properties

DELEGATE_GET(CAutoBreakpoints,Breakpoints,Application,LPDISPATCH)
DELEGATE_GET(CAutoBreakpoints,Breakpoints,Parent,LPDISPATCH)
DELEGATE_GET(CAutoBreakpoints,Breakpoints,Count,long)
DELEGATE_GET(CAutoBreakpoints,Breakpoints,_NewEnum,LPUNKNOWN)

// methods

DELEGATE_NO_PARAMS(CAutoBreakpoints,Breakpoints,RemoveAllBreakpoints)
DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoBreakpoints,Breakpoints,Item,VARIANT, LPDISPATCH)
DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoBreakpoints,Breakpoints,AddBreakpointAtLine,VARIANT, LPDISPATCH)

DELEGATE_ONE_PARAM_WITH_BOOL_RETVAL(CAutoBreakpoints,Breakpoints,RemoveBreakpointAtLine,VARIANT)


