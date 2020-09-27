//////////////////////////////////////////////////////////////////////////////
//  CODEBUG.CPP
//
//  Created by :            Date :
//      MichMa                  1/14/93
//
//  Description :
//      Implementation of the CODebug class
//
		
#include "stdafx.h"
#include "codebug.h"
#include "mstwrap.h"
#include "..\..\udialog.h"
#include "..\sym\qcqp.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"
#include "..\src\cosource.h"
#include "coee.h"
#include "parse.h"
#include "uidebug.h"
#include "uiattach.h"
#include "uidam.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// stores all localizable string id's for msgs used by CancelMsg() and HandleMsg()
int msg_array[] = 
{
	IDSS_REMOTE_EXE_NAME_REQUIRED,
	IDSS_REMOTE_LOCAL_EXE_MISMATCH,
	IDSS_EC_DELAYED_DUE_TO_CTOR_DTOR,
	IDSS_EC_ONLY_ASM_AVAILABLE,
	IDSS_EXCEPTION_DIVIDE_BY_ZERO,
	IDSS_EC_POINT_OF_EXECUTION_MOVED,
	IDSS_EC_LOCAL_VAR_REMOVED
};


CODebug::CODebug() {
	mMode = SRC;       //default to source mode.
	m_pOwnedStack = NULL;
};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::Exec(BOOL (UIWBFrame::*stepfcn)(int), int count /*= 1*/, LPCSTR symbol /*= NULL*/, LPCSTR line /*= NULL*/, LPCSTR function /*= NULL*/, int WAIT /*= WAIT_FOR_BREAK*/, int ModeVerify /*= TRUE*/)
// Description: Execute a debugging operation and verify success.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: stepfcn A pointer to a function in UIWBFrame that takes an int. This parameter specifies the debugging operation to perform: 	Go, Restart, StopDebugging, Break, StepInto, StepOver, StepOut, StepToCursor, ToggleBreakpoint.
// Param: count An integer that contains the number of times to perform this debugging operation. (Default value is 1.)
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the debugging operation is complete. NULL means don't verify based on symbol name. (Default value is NULL.)
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the debugging operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the debugging operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// Param: ModeVerify A Boolean value that is unused. This parameter is NYI. (Default value is TRUE.)
// END_HELP_COMMENT
BOOL CODebug::Exec(BOOL (UIWBFrame::*stepfcn)(int), int count, LPCSTR symbol, LPCSTR line, LPCSTR function, int WAIT, int ModeVerify)
	{	
	BOOL success = TRUE;
	if (!MyIsActive()) return FALSE;

	DirtyStack();

	if((WAIT == NOWAIT) && (line || function || symbol || (count > 1)))
		{
		WAIT = WAIT_FOR_BREAK;
		LOG->RecordInfo("WARNING: exec with verification attempted without\n"
						"waiting for break. your wait param was overridden\n"
						"to prevent timing problems on fast machines.");
		}

	Sleep(200);

	for(int i = 0; i < count; i++)
		{
		if (! (UIWB.*stepfcn)(WAIT) ) return FALSE;
		};
	if ((line) && (!VerifyCurrentLine(line)))
		success = FALSE;
	
	if ((function) && (!cos.VerifyCurrentFunction(function)))
		success = FALSE;

	if ((symbol) && (!VerifyAtSymbol(symbol)))
		success = FALSE;

	return success;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ExecUntilLine(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR line, LPCSTR function /*= NULL*/, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Execute a debugging operation until the specified line is hit.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: stepfcn A pointer to a function in UIWBFrame that takes an int. This parameter specifies the debugging operation to perform: 	Go, Restart, StopDebugging, Break, StepInto, StepOver, StepOut, StepToCursor, ToggleBreakpoint.
// Param: line A pointer to a string that contains the text of the line to be found that will end the debugging operation.
// Param: function A pointer to a string that contains the function name that contains the line to wait for. This parameter augments line. NULL means only look for the given line. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::ExecUntilLine(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR line, LPCSTR function, int WAIT)	// REVIEW set limit?
	{									
	BOOL success = TRUE;
	if (!MyIsActive()) return FALSE;
	DirtyStack();
	if (!ValidateSteppingMode(GetSteppingMode())) return FALSE;

	if(WAIT == NOWAIT)
		{
		WAIT = WAIT_FOR_BREAK;
		LOG->RecordInfo("WARNING: exec until operation attempted without\n"
						"waiting for break. your wait param was overridden\n"
						"to prevent timing problems on fast machines.");
		}

	while(!(CurrentLineIs(line) && (cos.CurrentFunctionIs(function) || !function)))
		{
		if (! (UIWB.*stepfcn)(WAIT) ) return FALSE;
		}
	return success;
	}
			

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ExecUntilFunc(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR function, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Execute a debugging operation until the specified function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: stepfcn A pointer to a function in UIWBFrame that takes an int. This parameter specifies the debugging operation to perform: 	Go, Restart, StopDebugging, Break, StepInto, StepOver, StepOut, StepToCursor, ToggleBreakpoint.
// Param: function A pointer to a string that contains the function name to be found that will end the debugging operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::ExecUntilFunc(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR function, int WAIT)	//REVIEW: set limit?
	{									
	BOOL success = TRUE;
	if (!MyIsActive()) return FALSE;
	DirtyStack();
	if (!ValidateSteppingMode(GetSteppingMode())) return FALSE;
	CString curfunc = cos.GetFunction();

	// exec operations without waiting for break causes timing problems
	if(WAIT == NOWAIT)
		{
		WAIT = WAIT_FOR_BREAK;
		LOG->RecordInfo("WARNING: exec until operation attempted without\n"
						"waiting for break. your wait param was overridden\n"
						"to prevent timing problems on fast machines.");
		}

	while(!(cos.CurrentFunctionIs(function) || (!function && !cos.CurrentFunctionIs(curfunc))))
		{
		if (! (UIWB.*stepfcn)(WAIT) ) return FALSE;
		}
	
	return success;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ExecUntilSymbol(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR symbol, int WAIT)
// Description: Execute a debugging operation until the specified symbol is found at the current line.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: stepfcn A pointer to a function in UIWBFrame that takes an int. This parameter specifies the debugging operation to perform: 	Go, Restart, StopDebugging, Break, StepInto, StepOver, StepOut, StepToCursor, ToggleBreakpoint.
// Param: symbol A pointer to a string that contains the symbol name to be found that will end the debugging operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::ExecUntilSymbol(BOOL (UIWBFrame::*stepfcn)(int), LPCSTR symbol, int WAIT)
	{									
	BOOL success = TRUE;
	if (!MyIsActive()) return FALSE;
	DirtyStack();
	if (!ValidateSteppingMode(GetSteppingMode())) return FALSE;

	// multiple exec operations without waiting for break causes timing problems
	// exec operations without waiting for break causes timing problems
	if(WAIT == NOWAIT)
		{
		WAIT = WAIT_FOR_BREAK;
		LOG->RecordInfo("WARNING: exec until operation attempted without\n"
						"waiting for break. your wait param was overridden\n"
						"to prevent timing problems on fast machines.");
		}

	while(!(AtSymbol(symbol)))
		{
		if (! (UIWB.*stepfcn)(WAIT) ) return FALSE;
		}
	return success;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::Go(LPCSTR symbol, LPCSTR line /*= NULL*/, LPCSTR function /*= NULL*/, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Perform the Go debugging operation and verify the correct line/function is entered when the operation is complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the Go operation is complete.
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the Go operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the Go operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::Go(LPCSTR symbol, LPCSTR line /* 0 */, LPCSTR function /* NULL */, int WAIT)
	{
		return Exec(&UIWBFrame::Go, 1, symbol , line, function, WAIT, FALSE);
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::Restart(LPCSTR path /*= NULL*/)
// Description: Perform the Restart debugging operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: path A pointer to a string that contains the path to the source files. (Default value is NULL.)
// END_HELP_COMMENT
BOOL CODebug::Restart(LPCSTR path /* NULL */)
{
	DirtyStack();
	// if a file path was specified, we are expecting the find source dlg & want to handle it
	if(path)
		UIWB.m_pTarget->SetHandleMsgs(FALSE);

	if (!UIWB.Restart(WAIT_FOR_BREAK)) 
	{
		UIWB.m_pTarget->SetHandleMsgs(TRUE);
		return FALSE;
	}
	if(path)
	{
		UIDialog fs(GetLocString(IDSS_FS_TITLE));	// "Find Source" dialog
		if( fs.WaitAttachActive(5000) )			// REVIEW: is 5 seconds enough?
		{
			if (path)
			{
				MST.DoKeys(path,TRUE);
				fs.OK();
				fs.WaitUntilGone(1000);
			}
			UIWB.m_pTarget->SetHandleMsgs(TRUE);
			return TRUE;
		}
	}
	// no find source dialog  (but if we specified path, then FAIL)
	UIWB.m_pTarget->SetHandleMsgs(TRUE);
	return (path==NULL);
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StopDebugging(int WAIT /*= ASSUME_NORMAL_TERMINATION*/)
// Description: Perform the Stop Debugging debugging operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is ASSUME_NORMAL_TERMINATION.)
// END_HELP_COMMENT
BOOL CODebug::StopDebugging(int WAIT)		
{
	if (!MyIsActive()) return FALSE;
	Sleep(1000);
	UINT state = UIWB.GetIDEState();
	if((state & ST_DEBUGRUN) || (state & ST_DEBUGBREAK))
	{
		return (UIWB.StopDebugging(WAIT));
	}
	else
	{
		LOG->RecordInfo( (LPCSTR) "Called Stop debugging when not currently debugging");
		return (UIWB.StopDebugging(WAIT));
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::Break()
// Description: Perform the Break debugging operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CODebug::Break()
{
	return (UIWB.Break(WAIT_FOR_BREAK));
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepInto(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Into debugging operation and verify the correct line/function/symbol is entered when the operation is complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: count An integer that contains the number of times to perform this debugging operation. (Default value is 1.)
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the Step Into operation is complete. NULL means don't verify based on symbol name. (Default value is NULL.)
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the Step Into operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the Step Into operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepInto(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
	{
		return Exec(&UIWBFrame::StepInto, count, symbol , line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepIntoUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Perform the Step Into debugging operation until the specified line/function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line A pointer to a string that contains the text of the line to step into until reached.
// Param: function A pointer to a string that contains the function name that contains the line to wait for. This parameter augments line. NULL means only look for the given line. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepIntoUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT)
	{
		return ExecUntilLine(&UIWBFrame::StepInto, line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepIntoUntilFunc(LPCSTR function /* NULL */, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Perform the Step Into debugging operation until the specified function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: function A pointer to a string that contains the function name to be found that will end the step into operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepIntoUntilFunc(LPCSTR function /* NULL */, int WAIT)
	{
		return ExecUntilFunc(&UIWBFrame::StepInto, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepIntoUntilSymbol(LPCSTR symbol, int WAIT /*= WAIT_FOR_BREAK*/)
// Description: Perform the Step Into debugging operation until the specified symbol is reached.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the symbol name to be found that will end the step into operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepIntoUntilSymbol(LPCSTR symbol, int WAIT)
	{
		return ExecUntilSymbol(&UIWBFrame::StepInto, symbol, WAIT);
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOver(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Over debugging operation and verify the correct line/function/symbol is entered when the operation is complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: count An integer that contains the number of times to perform this debugging operation. (Default value is 1.)
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the Step Over operation is complete. NULL means don't verify based on symbol name. (Default value is NULL.)
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the Step Over operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the Step Over operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOver(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
	{
		return Exec(&UIWBFrame::StepOver, count, symbol , line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOverUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Over debugging operation until the specified line/function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line A pointer to a string that contains the text of the line to be found that will end the step over operation.
// Param: function A pointer to a string that contains the function name that contains the line to wait for. This parameter augments line. NULL means only look for the given line. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOverUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT)
	{
		return ExecUntilLine(&UIWBFrame::StepOver, line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOverUntilFunc(LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Over debugging operation until the specified function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: function A pointer to a string that contains the function name to be found that will end the step over operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOverUntilFunc(LPCSTR function /* NULL */, int WAIT)
	{
		return ExecUntilFunc(&UIWBFrame::StepOver, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOverUntilSymbol(LPCSTR symbol, int WAIT)
// Description: Perform the Step Over debugging operation until the specified symbol is reached.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the symbol name to be found that will end the step over operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOverUntilSymbol(LPCSTR symbol, int WAIT)
	{
		return ExecUntilSymbol(&UIWBFrame::StepOver, symbol, WAIT);
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOut(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Out debugging operation and verify the correct line/function/symbol is entered when the operation is complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: count An integer that contains the number of times to perform this debugging operation. (Default value is 1.)
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the Step Out operation is complete. NULL means don't verify based on symbol name. (Default value is NULL.)
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the Step Out operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the Step Out operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOut(int count /* 1 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT)
	{
		return Exec(&UIWBFrame::StepOut, count, symbol , line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOutUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Out debugging operation until the specified line/function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line A pointer to a string that contains the text of the line to be found that will end the step out operation.
// Param: function A pointer to a string that contains the function name that contains the line to wait for. This parameter augments line. NULL means only look for the given line. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOutUntilLine(LPCSTR line, LPCSTR function /* NULL */, int WAIT)	//REVIEW: set limit?
	{
		return ExecUntilLine(&UIWBFrame::StepOut, line, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOutUntilFunc(LPCSTR function /* NULL */, int WAIT)
// Description: Perform the Step Out debugging operation until the specified function is entered.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: function A pointer to a string that contains the function name to be found that will end the step out operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOutUntilFunc(LPCSTR function /* NULL */, int WAIT)
	{
		return ExecUntilFunc(&UIWBFrame::StepOut, function, WAIT);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepOutUntilSymbol(LPCSTR symbol, int WAIT)
// Description: Perform the Step Out debugging operation until the specified symbol is reached.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that contains the symbol name to be found that will end the step out operation.
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepOutUntilSymbol(LPCSTR symbol, int WAIT)
	{
		return ExecUntilSymbol(&UIWBFrame::StepOut, symbol, WAIT);
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::StepToCursor(int linepos /* 0 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT /*WAIT_FOR_BREAK*/)
// Description: Perform the Step To Cursor debugging operation and verify the correct line/function/symbol is entered when the operation is complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: linepos An integer that contains the line to position the cursor on before performing the Step To Cursor operation. 0 means use the current line. (Default is 0.)
// Param: symbol A pointer to a string that contains the symbol to expect at the current line when the Step To Cursor operation is complete. NULL means don't verify based on symbol name. (Default value is NULL.)
// Param: line A pointer to a string that contains the text of the line to expect at the current line when the Step To Cursor operation is complete. NULL means don't verify based on line text. (Default value is NULL.)
// Param: function A pointer to a string that contains the function name to expect when the Step To Cursor operation is complete. NULL means don't verify based on function name. (Default value is NULL.)
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// END_HELP_COMMENT
BOOL CODebug::StepToCursor(int linepos /* 0 */, LPCSTR symbol /* NULL */, LPCSTR line /* NULL */, LPCSTR function /* NULL */, int WAIT /*WAIT_FOR_BREAK*/)
	{
		COSource src;

	if(linepos)
	{
// REVIEW(briancr): this is obsolete code
//		if (!UIWB.GoToLine(linepos)) return FALSE;
		if (!src.GoToLine(linepos)) {
			return FALSE;
		}
	};
	return Exec(&UIWBFrame::StepToCursor, 1, symbol , line, function, WAIT, FALSE);
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::AttachToProcess(LPCSTR szProcessName, int WAIT /*WAIT_FOR_RUN*/, BOOL bSysProcess /*FALSE*/)
// Description: Attaches the debugger to a specified process via the Attach To Process dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szProcessName A pointer to a string specifying the name of the process to attach to (without the .EXE extension).
// Param: WAIT A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_RUN.)
// Param: bSysProcess A Boolean that indicates if szProcessName is system process. If true, Show System Processes checkbox is set in the Attach To Process dialog
// END_HELP_COMMENT

BOOL CODebug::AttachToProcess(LPCSTR szProcessName, int WAIT /*WAIT_FOR_RUN*/, BOOL bSysProcess /*FALSE*/)

{
	char szThisFunc[] = "CODebug::AttachToProcess";
	UIAttachToProcessDlg uiattach;

	// activate the Attach To Process dlg.
	if(!uiattach.Activate())
	{
		LOG->RecordInfo("ERROR in %s: couldn't activate Attach To Process dlg.", szThisFunc);
		return FALSE;
	}

	// toggle display of the system processes appropriately.
	if(!uiattach.ShowSystemProcesses(bSysProcess))
	{
		LOG->RecordInfo("ERROR in %s: couldn't toggle display of system processes.", szThisFunc);
		uiattach.Cancel();
		return FALSE;
	}
	
	// select the process.
	if(!uiattach.SelectProcess(szProcessName))
	{
		LOG->RecordInfo("ERROR in %s: couldn't select the '%s' process.", szThisFunc, szProcessName);
		uiattach.Cancel();
		return FALSE;
	}

	// ok the Attach To Process dlg.
	if(uiattach.OK() != NULL)
	{
		LOG->RecordInfo("ERROR in %s: couldn't OK Attach To Process dlg.", szThisFunc);
		return FALSE;
	}

	return Wait(WAIT);
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::SetNextStatement(int line /* 0 */)
// Description: Set the next statement (IP) to the given line.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line An integer that contains the line to set the next statement to. 0 means use the current line. (Default value is 0.)
// END_HELP_COMMENT
BOOL CODebug::SetNextStatement(int line /* 0 */)		// REVIEW: check for non-zero line number?
	{
		COSource src;

	if(line>0)
	{
// REVIEW(briancr): this is obsolete code
//		if (!UIWB.GoToLine(line)) return FALSE;
		if (!src.GoToLine(line)) {
			return FALSE;
		}
	};
	DirtyStack();
	return UIWB.SetNextStatement();
	}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::SetNextStatement(LPCSTR line)
// Description: Set the next statement (IP) to the given line.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: line A pointer to a string that contains the line to set the next statement to. NULL means use the current line. (Default value is NULL.)
// END_HELP_COMMENT
BOOL CODebug::SetNextStatement(LPCSTR line)		//TODO: use UIFind class
{
	COSource src;

	if(line) 	// if line is NULL  use current line
	{
		if (*line == '@')  // handle "@23" for line number 23
// REVIEW(briancr): this is obsolete code
//			if (!UIWB.GoToLine( atoi((line+1)) )) return FALSE;
			if (!src.GoToLine(atoi(line+1))) {
				return FALSE;
			}
		else
// REVIEW(briancr): this is obsolete code
//			if (!UIWB.GoToLine(line)) return FALSE;
			if (!src.GoToLine(line)) {
				return FALSE;
			}
	};
	DirtyStack();
	return (UIWB.SetNextStatement());
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ShowNextStatement(void)
// Description: Go to the next statement to be executed by the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CODebug::ShowNextStatement(void) 
{
	WaitStepInstructions("Sending keys '%({NUMPAD*})'");
	keybd_event(VK_MENU, 0,0,0);
	keybd_event(VK_MULTIPLY, 0,0,0);
	keybd_event(VK_MULTIPLY, 0,KEYEVENTF_KEYUP,0);
	keybd_event(VK_MENU, 0,KEYEVENTF_KEYUP,0);
	// give the ide a half-second to update the cursor. if we return too soon then menu items might be grayed when we expect them to be enabled.
	Sleep(500);
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: DebugState CODebug::GetDebugState(void)
// Description: Get the state of the debugger.
// Return: A DebugState value that indicates the state of the debugger: ST_NODEBUG, ST_DEBUGBREAK, ST_DEBUGRUN.
// END_HELP_COMMENT
DebugState CODebug::GetDebugState(void) {
    MyIsActive();
    UINT state = UIWB.GetIDEState();
    if (state & ST_NODEBUG)
        return NotDebugging;
    if (state & ST_DEBUGBREAK)
        return Debug_Break;
    if (state & ST_DEBUGRUN)
        return Debug_Run;
    // defualt
    return NotDebugging;
};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::Wait(int option, BOOL bLeaveFindSource /* FALSE */)
// Description: Wait for the current debugging operation to complete.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: option A value that specifies how to wait for the debugging operation to complete: NOWAIT, WAIT_FOR_RUN, WAIT_FOR_BREAK, WAIT_FOR_TERMINATION, ASSUME_NORMAL_TERMINATION, WAIT_FOR_EXCEPTION, WAIT_FOR_CODED_BREAK. (Default value is WAIT_FOR_BREAK.)
// Param: BOOL bLeaveFindSource A Boolean value indicated whether or not to leave the Find Source dlg up at the break.
// END_HELP_COMMENT
BOOL CODebug::Wait(int option, BOOL bLeaveFindSource /* FALSE */)
	{
	return UIWB.Wait(option, TRUE, bLeaveFindSource);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::CurrentLineIs(LPCSTR line)		
// Description: Determine whether the current line in the debugger matches the line given.
// Return: A Boolean value that indicates whether the current line in the debugger matches the given line (TRUE) or not.
// Param: line A pointer to a string that contains either part of the text of the line expected or the line number specified by @line.
// END_HELP_COMMENT
BOOL CODebug::CurrentLineIs(LPCSTR line)		
{
	if (*line == '@') // handle "@23" for line number 23
		return ( UIWB.GetEditorCurPos(GECP_LINE) == atoi((line+1)) );
	else
		return ( UIWB.VerifySubstringAtLine(line) );
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::VerifyCurrentLine(LPCSTR line)
// Description: Determine whether the current line in the debugger matches the line given.
// Return: A Boolean value that indicates whether the current line in the debugger matches the given line (TRUE) or not.
// Param: line A pointer to a string that contains either part of the text of the line expected or the line number specified by @line.
// END_HELP_COMMENT
BOOL CODebug::VerifyCurrentLine(LPCSTR line)
{
	if (*line == '@') // handle "@23" for line number 23
	{
		int actual_pos, expected_pos;
		actual_pos= UIWB.GetEditorCurPos(GECP_LINE);
		expected_pos= atoi((line+1));
		if (expected_pos==actual_pos)
			return TRUE;
		else
		{
			LOG->RecordInfo( (LPCSTR) "Current line is %d : expected %d", actual_pos, expected_pos);
			return FALSE;
		}
		
	}
	else
	{
		if ( UIWB.VerifySubstringAtLine(line) )
			return TRUE;
		else
		{
			LOG->RecordInfo( (LPCSTR) "Current line does not contain '%s'", line);
			return FALSE;
		}
	};
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::AtSymbol(LPCSTR symbol)
// Description: Determine whether the current debugging location (IP) is at the given symbol.
// Return: A Boolean value that indicates whether the current debugging location (IP) is at the given symbol (TRUE) or not.
// Param: symbol A pointer to a string that contains the symbol.
// END_HELP_COMMENT
BOOL CODebug::AtSymbol(LPCSTR symbol)
{
	COExpEval coee;
	CString InstructionPtr;

#if defined( _M_MRX000 )
	InstructionPtr = "FIR";    	// MIPS specific

	//
	//	Variance is allowed for matching breakpoint at function for MIPS
	//	since it must set the breakpoint after the prolog.  Note that it
	//	possible for the prolog to be much greater but hopefully unlikely.
	//
	const CString AllowedVariance( "40" );	//	Arbitrary value for variance offset (bytes)

	CString expr = InstructionPtr + " >= " + symbol + " && " +
		InstructionPtr  + " - " + AllowedVariance + " <= " + symbol;
#elif defined(_M_ALPHA)
	InstructionPtr = "FIR";    	// ALPHA specific

	//
	//	Variance is allowed for matching breakpoint at function for ALPHA
	//	since it must set the breakpoint after the prolog.  Note that it
	//	possible for the prolog to be much greater but hopefully unlikely.
	//
	const CString AllowedVariance( "80" );	//	Arbitrary value for variance offset (bytes)

	CString expr = InstructionPtr + " >= " + symbol + " && " +
		InstructionPtr  + " - " + AllowedVariance + " <= " + symbol;
#else
	
	if((GetUserTargetPlatforms() == PLATFORM_MAC_68K) ||
	   (GetUserTargetPlatforms() == PLATFORM_MAC_PPC) ||
	   (GetUserTargetPlatforms() == PLATFORM_WIN32_PPC))
		InstructionPtr = "PC";
	else if((GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA))
		InstructionPtr = "PC";    	
	else if((GetUserTargetPlatforms() == PLATFORM_WIN32_X86) ||
			(GetUserTargetPlatforms() == PLATFORM_WIN32S_X86))
		InstructionPtr = "EIP";
	else
		{
		ASSERT(0);
		return FALSE;
		}		    	

	CString expr = InstructionPtr + " == " + symbol;
#endif
	return (coee.ExpressionValueIs(expr,TRUE));
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::VerifyAtSymbol(LPCSTR symbol)
// Description: Determine whether the current debugging location (IP) is at the given symbol.
// Return: A Boolean value that indicates whether the current debugging location (IP) is at the given symbol (TRUE) or not.
// Param: symbol A pointer to a string that contains the symbol.
// END_HELP_COMMENT
BOOL CODebug::VerifyAtSymbol(LPCSTR symbol)
{
	if  (AtSymbol(symbol))
		return TRUE;
	else
	{
		LOG->RecordInfo( (LPCSTR) "Instruction Pointer not at Symbol '%s'", symbol);
		return FALSE;
	}
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::VerifyTermination(int exitcode)
// Description: Determine whether the debuggee terminated with the specified exit code. (See VerifyTermination(void) to verify termination regardless of exit code.) This function is NYI.
// Return: A Boolean value that indicates whether the debuggee terminated with the specified exit code (TRUE) or not.
// Param: exitcode An integer that specifies the expected exit code.
// END_HELP_COMMENT
BOOL CODebug::VerifyTermination(int exitcode)
{
    // todo  check for specific termination code
    //       log warning only if code doesn't match
    return TRUE;
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::VerifyTermination(void)
// Description: Determine whether the debuggee terminated. (See VerifyTermination(int) to verify termination with a specific exit code.) This function is NYI.
// Return: A Boolean value that indicates whether the debuggee terminated (TRUE) or not.
// END_HELP_COMMENT
BOOL CODebug::VerifyTermination(void)          //any exitcode
{
    // todo check for ANY termination
    //        don't log warning
    return TRUE;
};


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::SetSteppingMode(StepMode mode)
// Description: Set the stepping mode in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: mode A StepMode value that indicates the stepping mode: SRC, ASM.
// END_HELP_COMMENT
BOOL CODebug::SetSteppingMode(StepMode mode)
{
	BOOL success=TRUE;
	mMode = mode;

	if (GetDebugState() == NotDebugging)
		return success;  // bail-out but succeed if not debugging
	
	if (mode == SRC)	
	{
		UIDAM uidam;

		// find out what more we're in
		// emmang@xbox.com
		CString cstrCaption = "[";
		cstrCaption += GetLocString(IDSS_ASM_WND_TITLE) + "]";
		CString cstrFound;

		MST.WGetText(WGetActWnd(0), cstrFound);

		// if we're in asm mode, switch to source using hotkey. else just show the next statement in source mode.
		//if(uidam.AttachActive())
		if (cstrFound.Find(cstrCaption) != -1)
		{
			LOG->RecordInfo("SetSteppingMode(%d) : uidam.AttachActive() == TRUE, calling uidam.SwitchBetweenASMAndSrc()", mode);
			uidam.SwitchBetweenASMAndSRC(FALSE);
		}
		else
			ShowNextStatement();

		mMode = mode;
	}
	else if (mode == ASM)
	{
		UIDebug::ShowDockWindow(IDW_DISASSY_WIN); // focus to dam
		mMode = mode;
	}
	else
	{
		LOG->RecordInfo( (LPCSTR) "Stepping Mode, %d is not valid", mode);
		success=FALSE;
	};
	return success;
};

// BEGIN_HELP_COMMENT
// Function: StepMode CODebug::GetSteppingMode(void)
// Description: Get the current stepping mode in the debugger.
// Return: A StepMode value that indicates the current stepping mode: SRC, ASM.
// END_HELP_COMMENT
StepMode CODebug::GetSteppingMode(void)
{
	return mMode;
};

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ValidateSteppingMode(StepMode mode)
// Description: Determine whether the current stepping mode matches the given stepping mode.
// Return: A Boolean value that indicates whether the current stepping mode matches the given stepping mode (TRUE) or not.
// Param: mode A StepMode value that contains the expected stepping mode: SRC, ASM.
// END_HELP_COMMENT
BOOL CODebug::ValidateSteppingMode(StepMode mode)
{
	// todo be smarter here and check before re-setting each time
	return SetSteppingMode(mode);
};

// code reminant from old cafe sniff tests
// although not portable  may be very useful
// todo: review the implementation.

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::DoDebuggeeKeys(CString str)
// Description: Perform the given keystrokes in the debuggee. This code is not portable and should be used with extreme caution.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: str A CString that contains the title, class, and keystrokes to send to the debuggee. The form is: Title#Class#Keys.
// END_HELP_COMMENT
BOOL CODebug::DoDebuggeeKeys(CString str)
{
	// Title#Class#Keys

	CString strTitle, strClass, strKeys;
	CTokenList listToken;

	// parse the parameters
	Parse(str, "#", listToken);
	strTitle = listToken.GetTokenString(0);
	strClass = listToken.GetTokenString(1);
	strKeys = listToken.GetTokenString(2);

	// give the debuggee focus
	if (MST.WFndWndC(strTitle, strClass, FW_NOCASE | FW_PART | FW_FOCUS) != NULL) {
		MST.DoKeys(strKeys);
		LOG->RecordInfo("Keystrokes '"+str+"' sent to debuggee");
		return (TRUE);
	}
	else {
		LOG->RecordInfo("Keystrokes '"+str+"' not sent to debuggee");
		return (FALSE);
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::DoDebuggeeCmd(CString str)
// Description: Perform the given command in the debuggee. This code is not portable and should be used with extreme caution.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: str A CString that contains the title, class, and command to send to the debuggee. The form is: Title#Class#Command.
// END_HELP_COMMENT
BOOL CODebug::DoDebuggeeCmd(CString str)
{
	// Title#Class#Command

	CString strTitle, strClass, strCmd;
	CTokenList listToken;
	HWND hwnd;

	// parse out the parameters
	Parse(str, "#", listToken);
	strTitle = listToken.GetTokenString(0);
	strClass = listToken.GetTokenString(1);
	strCmd = listToken.GetTokenString(2);

	// give the debuggee focus
	if ((hwnd = MST.WFndWndC(strTitle, strClass, FW_NOCASE | FW_PART | FW_FOCUS)) != NULL) {
		PostMessage(hwnd, WM_COMMAND, (WPARAM)atoi(strCmd), 0L);

	// wait for the IDE to enter break mode or display a msg box
/*	for (BOOL bDone = FALSE; !bDone; ) {
		MST.WFndWnd(GetLocString(IDSS_WORKBENCH_TITLEBAR), FW_PART | FW_NOCASE | FW_FOCUS);
		if (MST.WFndWndC(GetLocString(IDSS_WORKBENCH_TITLEBAR), "#32770", FW_PART | FW_NOCASE) != NULL) {
			bDone = TRUE;
		}
		UIWB.Activate();
		if (UIWB.GetIDEState() & ST_DEBUGBREAK) {
			bDone = TRUE;
		}
	} */
		Sleep(20000);
		LOG->RecordInfo("Command "+str+" sent to debuggee");
		return (TRUE);
	}
	else {
		LOG->RecordInfo("Command "+str+" not sent to debuggee");
		return (FALSE);
	}
}


BOOL CODebug::MyIsActive()
{
	if (!UIWB.IsActive())
	{
		UIWB.BlockingActivate();
	};
	return TRUE;
}

BOOL CODebug::SetOwnedStack( COStack* pStack ) {

	// Unlink previous owned stack
	if( m_pOwnedStack != NULL ) {
		ASSERT( m_pOwnedStack->m_pOwner == this );
		m_pOwnedStack->m_pOwner = NULL;
	}

	// Link new stack
	if( pStack != NULL ) {
		ASSERT( pStack->m_pOwner == NULL );

		pStack->m_pOwner = this;
	}

	m_pOwnedStack = pStack;
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::VerifyNotification(LPCSTR notification, int startLine /* 1 */, int numberOfLines /* 1 */)
// Description: Verifies any debugger output (substring).
// Return: TRUE if the notification substring is found at line, FALSE - if not
// Param: notification A pointer to a string that contains the substring to look for in the debugger output.
// Param: nOccurence An integer that specifies the 1-based number of occurence of the Substring in the Output window. (Default value is 1.)
// Old: Param: startLine An integer that specifies the 1-based line number in the Output window to start looking for the specified substring. (Default value is 1.)
// Param: numberOfLines An integer that specifies the number of lines to search (from startLine) for the substring. (Default value is 1.)
// Param: nCase A boolean: TRUE specifies case insensitive search. (Default value is FALSE.)
// END_HELP_COMMENT
BOOL CODebug::VerifyNotification(LPCSTR notification, int nOccurence /* 1 */, int numberOfLines /* 1 */, BOOL nCase /* FALSE */)
{
	UIDebug::ShowDockWindow(IDW_OUTPUT_WIN); // focus to Output wnd
	// REVIEW(michma - 6/4/99):
	// this timing problem is showing up on w2k. apparently the ^home isn't making it to the output window
	// because it doesn't have focus yet.
	Sleep(500);
	MST.DoKeys("^{home}");
	int counter = 0;

	for( int i = 0; i < numberOfLines; i++)
	{
		if( UIWB.VerifySubstringAtLine(notification, nCase) )
		{
			if( ++counter == nOccurence )
				return TRUE;
		}
		MST.DoKeys("{down}");
	}
	
	if( counter )
	{
		LOG->RecordInfo( (LPCSTR) "Only %d notification occurences were found", counter);
	}
	else
	{
		LOG->RecordInfo( (LPCSTR) "Notification was not found");
	}

	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::HandleMsg(MSG_TYPE msg, LPCSTR pdata /* NULL */, int wait_time /* 60 */)
// Description: Waits for a specific msg box to be displayed, supplies the data requested, and continues
// Return: TRUE if the correct msg box comes up, the data is supplied, and the msg box is dismissed.  FALSE if any of those operations fail.
// Param: msg: a symbol representing the particular msg box to be handled.  current legal values are MSG_REMOTE_EXE_NAME_REQUIRED and MSG_REMOTE_LOCAL_EXE_MISMATCH
// Param: pdata: a pointer to the data that needs to be supplied to handle the message. the type of data depends on the msg being handled.  currently only MSG_REMOTE_EXE_NAME_REQUIRED uses data, and that is a string pointing to the remote path of the target.  the default data pointer is NULL.
// Param: wait_time: time in seconds to wait for msg box to be displayed. default is 60 seconds.
// END_HELP_COMMENT

BOOL CODebug::HandleMsg(MSG_TYPE msg, LPCSTR pdata /* NULL */, int wait_time /* 60 */)

{
	if(!WaitMsgDisplayed(msg, wait_time)) return FALSE;
	BOOL fDataSupplied = FALSE;
	CString str;

	switch(msg)
		
	{
		case MSG_REMOTE_EXE_NAME_REQUIRED:
		
		{
			MST.WEditSetText("@1", pdata);
			MST.WEditText("@1", str);
			
			if(!(str == CString(pdata)))
			{
				LOG->RecordInfo("ERROR in CODebug::HandleMsg(): "
								"data \"%s\" not supplied to msg box", (LPCSTR)pdata);
				return FALSE;
			}

			break;
		}

		default:
			break;
	}

	MST.DoKeys("{ENTER}");
	return WaitMsgDismissed(msg);
}
	

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::CancelMsg(MSG_TYPE msg, int wait_time /* 60 */)
// Description: Waits for a specific msg box to be displayed and cancels it
// Return: TRUE if the correct msg box comes up and is canceled.  FALSE if either of those operations fail.
// Param: msg: a symbol representing the particular msg box to be handled.  current legal values are MSG_REMOTE_EXE_NAME_REQUIRED and MSG_REMOTE_LOCAL_EXE_MISMATCH
// Param: wait_time: time in seconds to wait for msg box to be displayed. default is 60 seconds.
// END_HELP_COMMENT

BOOL CODebug::CancelMsg(MSG_TYPE msg, int wait_time /* 60 */)
	{
	if(!WaitMsgDisplayed(msg, wait_time)) return FALSE;
	MST.DoKeys("{ESC}");
	return WaitMsgDismissed(msg);
	}


BOOL CODebug::WaitMsgDisplayed(MSG_TYPE msg, int wait_time)
	
	{
	if(!MST.WFndWndWaitC(GetLocString(msg_array[msg]), "Static", FW_PART, wait_time))
		
		{
		LOG->RecordInfo("ERROR in CODebug::WaitMsgDisplayed(): msg box containing \"%s\" not "
						"displayed after %d seconds", (LPCSTR)GetLocString(msg_array[msg]), wait_time);

		return FALSE;
		}

	return TRUE;
	}


BOOL CODebug::WaitMsgDismissed(MSG_TYPE msg)

	{
	int i = 0;
	
	while(MST.WFndWndWaitC(GetLocString(msg_array[msg]), "Static", FW_PART,1))
		
		{

		if(++i > 5)
			{
			LOG->RecordInfo("ERROR in CODebug::WaitMsgDismissed(): msg box containing \"%s\""
							"could not be dismissed", (LPCSTR)GetLocString(msg_array[msg]));

			return FALSE;
			}
		}

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: int CODebug::ToggleReturnValue(TOGGLE_TYPE t)
// Description: Toggles the return values feature on and off.
// Return: Currently always returns TRUE.
// Param: t: a TOGGLE_TYPE value specifying to turn return values on (TOGGLE_ON) or off (TOGGLE_OFF).
// END_HELP_COMMENT

/* TODO (michma): add error checking. */
BOOL CODebug::ToggleReturnValue(TOGGLE_TYPE t)
	
	{
	UIOptionsTabDlg uiopt;
	uiopt.Display();
	uiopt.ShowPage(TAB_DEBUG, 6);
	
	if(t == TOGGLE_ON)
		MST.WCheckCheck(GetLocString(IDSS_OPT_DEBUG_RETURN_VALUE));
	else
 		MST.WCheckUnCheck(GetLocString(IDSS_OPT_DEBUG_RETURN_VALUE));

	uiopt.OK();
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ToggleEditAndContinue(TOGGLE_TYPE t)
// Description: Toggles EditAndContinue check box
// Return: 
// Param: t: Toggle type: TOGGLE_ON if box is to be checked; TOGGLE_OFF if box is to be unchecked  
// END_HELP_COMMENT

/* TODO (michma): add error checking. */
BOOL CODebug::ToggleEditAndContinue(TOGGLE_TYPE t)
	
	{
	UIOptionsTabDlg uiopt;
	uiopt.Display();
	uiopt.ShowPage(TAB_DEBUG, 6);
	
	if(t == TOGGLE_ON)
		MST.WCheckCheck(GetLocString(IDSS_OPT_DEBUG_EDIT_AND_CONTINUE));
	else
 		MST.WCheckUnCheck(GetLocString(IDSS_OPT_DEBUG_EDIT_AND_CONTINUE));

	uiopt.OK();
	return TRUE;
	}


// TODO(michma): RSLA dlg needs its own support layer class
// BEGIN_HELP_COMMENT
// Function: int CODebug::ResolveSourceLineAmbiguity(int iEntry)
// Description: waits for a "resolve source line ambiguity" message from the debugger, and resolves it.
// Return: BOOL returns TRUE if successful, FALSE if not.
// Param: iEntry: an int indicated which entry in the ambiguity list to select
// END_HELP_COMMENT
BOOL CODebug::ResolveSourceLineAmbiguity(int iEntry)
{
	
	// wait for the rsla dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RSLA_DLG_TITLE), FW_DEFAULT, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Source Line Ambiguity dlg not found");
		return FALSE;
	}

	// make sure the dlg has enough entries in it.
	if(MST.WListCount("") < iEntry)
	{
		LOG->RecordInfo("ERROR: Number of entries in RSLA dlg is less than %d", iEntry);
		return FALSE;
	}
	
	// choose the source line entry from the list box in the rsla dlg
	for(int i = 1; i < iEntry; i++)
		MST.DoKeys("{DOWN}");
	
	// ok the dlg
	MST.DoKeys("{ENTER}");

	// wait for the rsla dlg to be disappear
	if(!MST.WFndWndWait(GetLocString(IDSS_RSLA_DLG_TITLE), FW_DEFAULT | FW_NOEXIST, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Source Line Ambiguity dlg still active");
		return FALSE;
	}
	
	return TRUE;
}


// TODO(michma): RA dlg has a class of its own in uibrowse.*.
// BEGIN_HELP_COMMENT
// Function: int CODebug::ResolveAmbiguity(int iEntry)
// Description: waits for a "resolve ambiguity" message from the debugger, and resolves it.
// Return: BOOL returns TRUE if successful, FALSE if not.
// Param: iEntry: an int indicated which entry in the ambiguity list to select
// END_HELP_COMMENT
BOOL CODebug::ResolveAmbiguity(int iEntry)
{
	
	// wait for the ra dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg not found");
		return FALSE;
	}

	// make sure the dlg has enough entries in it.
	if(MST.WListCount("") < iEntry)
	{
		LOG->RecordInfo("ERROR: Number of entries in RA dlg is less than %d", iEntry);
		return FALSE;
	}
	
	// choose the source line entry from the list box in the rsla dlg
	for(int i = 1; i < iEntry; i++)
		MST.DoKeys("{DOWN}");
	
	// ok the dlg
	MST.DoKeys("{ENTER}");

	// wait for the ra dlg to be disappear
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT | FW_NOEXIST, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg still active");
		return FALSE;
	}
	
	return TRUE;
}

// TODO(michma): RA dlg has a class of its own in uibrowse.*.
// BEGIN_HELP_COMMENT
// Function: int CODebug::ResolveAmbiguity(LPCSTR szEntry)
// Description: waits for a "resolve ambiguity" message from the debugger, and resolves it.
// Return: BOOL returns TRUE if successful, FALSE if not.
// Param: szEntry: a string (partial allowed) indicating which entry in the ambiguity list to select.
// END_HELP_COMMENT
BOOL CODebug::ResolveAmbiguity(LPCSTR szEntry)
{
	
	// wait for the ra dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg not found");
		return FALSE;
	}

	CString strEntry;
	
	// iterate through each entry in the list box.
	for(int iEntry = 1; iEntry <= MST.WListCount(""); iEntry++)
	{
		// get the text for the entry.
		MST.WListItemText("", iEntry, strEntry);

		// see if it contains the specified text.
		if(strEntry.Find(szEntry) != -1)
			break;
	}

	// if we found a match, select it.
	if(iEntry <= MST.WListCount(""))
		MST.WListItemClk("", strEntry);
	
	// ok the dlg
	MST.DoKeys("{ENTER}");

	// wait for the ra dlg to be disappear
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT | FW_NOEXIST, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg still active");
		return FALSE;
	}
	
	return TRUE;
}

// TODO(michma): RA dlg has a class of its own in uibrowse.*.
// BEGIN_HELP_COMMENT
// Function: int CODebug::ResolveSourceAmbiguity(LPCSTR szEntry)
// Description: waits for a "resolve source ambiguity" message from the debugger, and resolves it.
// Return: BOOL returns TRUE if successful, FALSE if not.
// Param: szEntry: a string (partial allowed) indicating which entry in the ambiguity list to select.
// END_HELP_COMMENT
BOOL CODebug::ResolveSourceLineAmbiguity(LPCSTR szEntry)
{
	
	// wait for the ra dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_SOURCE_AMBIGUITY_TITLE), FW_DEFAULT, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg not found");
		return FALSE;
	}

	CString strEntry;
	
	// iterate through each entry in the list box.
	for(int iEntry = 1; iEntry <= MST.WListCount(""); iEntry++)
	{
		// get the text for the entry.
		MST.WListItemText("", iEntry, strEntry);

		// see if it contains the specified text.
		if(strEntry.Find(szEntry) != -1)
			break;
	}

	// if we found a match, select it.
	if(iEntry <= MST.WListCount(""))
		MST.WListItemClk("", strEntry);
	
	// ok the dlg
	MST.DoKeys("{ENTER}");

	// wait for the ra dlg to be disappear
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_SOURCE_AMBIGUITY_TITLE), FW_DEFAULT | FW_NOEXIST, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Source Ambiguity dlg still active");
		return FALSE;
	}
	
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::SupplySourceLocation(LPCSTR path)
// Description: Supplys a source location to the debugger (assuming it is asking for one).
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: path A pointer to a string that contains the path to the source file.
// END_HELP_COMMENT
BOOL CODebug::SupplySourceLocation(LPCSTR path)

{
	//REVIEW(chriskoz) is handled by default by CTarget, 
	//so you must call UIWB.m_pTarget->SetHandleMsgs(FALSE) to handle it manually in this function
	//then call UIWB.m_pTarget->SetHandleMsgs(TRUE) to restore the automatic handling
	// set up the object for the Find Source dlg.
	UIDialog dlgFindSource(GetLocString(IDSS_FS_TITLE));

	// wait up to 3 seconds for Find Source dlg to come up.
	if(dlgFindSource.WaitAttachActive(3000))
	{
		// verify that it is the Find Source dlg.
		if(dlgFindSource.VerifyTitle() )
		{
			// enter the specified directory and close the dlg.
			MST.DoKeys(path);
			dlgFindSource.OK();
		}
		else
		{
			LOG->RecordInfo("ERROR in CODebug::SupplySourceLocation() - dlg other than Find Source came up");
			return FALSE;
		}
	}
	else
	{
		LOG->RecordInfo("ERROR in CODebug::SupplySourceLocation() - no dlg came up (expected Find Source)");
		return FALSE;
	}

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: int CODebug::SetDebugOutputPane( int pane)
// Description: Set the current pane of the Output window to Debug.
// Return: An integer that indicates success. ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: pane A value that specifies the pane to activate: IDSS_PANE_DEBUG_OUTPUT.
// END_HELP_COMMENT
int CODebug::SetDebugOutputPane(int pane)
{
	UIDebug::ShowDockWindow(IDW_OUTPUT_WIN); // focus to Output wnd

	char pane_str[128];
	strncpy(pane_str, GetLocString(pane), 128);
	

	// cycle through panes until we find the pane
	for(int i = 0; i < TOTAL_OUTPUT_PANES; i++)
		{
//		if(GetName(1) == this_str)
		Sleep(500);
//		::WCustomSetClass("Afx:400000:0"); //obsolete
//		if(::WCustomExists(pane_str,0)!=0) //obsolete
//		if(::WFndWndC(pane_str,"Afx:400000:0",FW_ACTIVE,0)!=NULL)
		if(::WFndWndC(pane_str,NULL,FW_ACTIVE,0)!=NULL) //we don't want to rely on th above class name
			break;
		MST.DoKeys("^({PGDN})");
		}
	
/*		if(MST.FindWndWait(pane_str,1)
			break;
		else
			MST.DoKeys("^({PGDN})");
*/ //there is no way to use FindWndWait
	if(i == TOTAL_OUTPUT_PANES)
		{
		LOG->RecordInfo("ERROR in CODebug::SetDebugOutputPane(): "
						"can't find %s pane", pane_str);

		return ERROR_ERROR;
		}	
	return ERROR_SUCCESS;
}


// BEGIN_HELP_COMMENT
// Function: int CODebug::StepIntoSpecific(int iWait /* WAIT_FOR_BREAK */)
// Description: Executes a Step Into <func> operation. The cursor is expected to be on a function call.
// Return: A BOOL that indicates success. TRUE if successful; FALSE otherwise.
// Param: pane A value that specifies the pane to activate: IDSS_PANE_DEBUG_OUTPUT.
// END_HELP_COMMENT
BOOL CODebug::StepIntoSpecific(int iWait /* WAIT_FOR_BREAK */)
{
	MST.DoKeys(GetLocString(IDSS_STEP_INTO_SPECIFIC_ACCEL));
	return Wait(iWait);
}

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ToggleHexDisplay(BOOL bEnable)
// Description: Toggles Hexadecimal display mode
// Param: bEnable If the parameter is TRUE, the Hexadecimal display mode is on. If FALSE - off.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT
BOOL CODebug::ToggleHexDisplay(BOOL bEnable)
{

	UIOptionsTabDlg OptionsTabDlg;
	return OptionsTabDlg.ToggleHexDisplay(bEnable);
}

// BEGIN_HELP_COMMENT
// Function: BOOL CODebug::ToggleSourceAnnotation(BOOL bEnable)
// Description: Toggles Source Annotation
// Param: bEnable If the parameter is TRUE, the Hexadecimal display mode is on. If FALSE - off.
// Return: A Boolean value that indicates whether the toggle operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT
BOOL CODebug::ToggleSourceAnnotation(BOOL bEnable)
{

	UIOptionsTabDlg OptionsTabDlg;
	return OptionsTabDlg.ToggleSourceAnnotation(bEnable);
}
