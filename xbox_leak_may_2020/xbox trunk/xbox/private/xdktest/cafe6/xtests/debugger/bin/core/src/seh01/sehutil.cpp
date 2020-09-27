//		Utility functions for SEH.

//#include "stdafx.h"
//not in xbox - #include <wtypes.h>
#include <excpt.h>
//#include <afxwin.h>
#include "sehutil.h"

//	Use counter for tracing execution/setting breakpoints

int counter = 0;


//	Functions for causing exceptions.

void RaiseNoException() {
	//
	//	Empty function call
	//
}

void RaiseIntDivideByZero() {
	RaiseException( EXCEPTION_INT_DIVIDE_BY_ZERO, 0, 0, NULL );
}

void RaiseControlC() {
	RaiseException( DBG_CONTROL_C, 0, 0, NULL );
	//	GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0)
}

void IntDivideByZero() {
	int i = 1;
	int j = 0;
	int k;
	k = i/j;
}

//
//	Functions used for filter-expressions.
//
LONG ContinueExecution() {
	++counter;
	return EXCEPTION_CONTINUE_EXECUTION;
}

LONG ExecuteHandler() {
	++counter;
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG ContinueSearch() {
	++counter;
	return EXCEPTION_CONTINUE_SEARCH;
}
