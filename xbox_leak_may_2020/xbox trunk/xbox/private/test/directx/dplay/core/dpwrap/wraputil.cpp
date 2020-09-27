//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>

#undef DEBUG_SECTION
#define DEBUG_SECTION	"DpWrapDisplayException()"
//==================================================================================
// DpWrapDisplayException
//----------------------------------------------------------------------------------
//
// DpWrapDisplayException: Displays a message indicating type of exception that occurred
//
// Arguments:
//	DWORD	dwExceptionType		Type of exception that occurred
//
// Returns: Value indicating whether to handle the exception or not
//==================================================================================
DWORD DpWrapDisplayException(DWORD dwExceptionType)
{
	DWORD dwExceptAction = EXCEPTION_EXECUTE_HANDLER;
	LPSTR szExceptName = NULL;

	switch(dwExceptionType)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		szExceptName = "EXCEPTION_ACCESS_VIOLATION";
		break;
	case EXCEPTION_BREAKPOINT:
		szExceptName = "EXCEPTION_BREAKPOINT";
		break;
	case EXCEPTION_SINGLE_STEP:
		szExceptName = "EXCEPTION_SINGLE_STEP";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		szExceptName = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		szExceptName = "EXCEPTION_FLT_DENORMAL_OPERAND";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		szExceptName = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		szExceptName = "EXCEPTION_FLT_INEXACT_RESULT";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		szExceptName = "EXCEPTION_FLT_INVALID_OPERATION";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		szExceptName = "EXCEPTION_FLT_OVERFLOW";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		szExceptName = "EXCEPTION_FLT_STACK_CHECK";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		szExceptName = "EXCEPTION_FLT_UNDERFLOW";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		szExceptName = "EXCEPTION_INT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_INT_OVERFLOW:
		szExceptName = "EXCEPTION_INT_OVERFLOW";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		szExceptName = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		break;
	default:
		szExceptName = "UNKNOWN";
		break;
	}

//	DbgPrint("Exception occured in DirectPlay: %s\n", szExceptName);

	return dwExceptAction;
}
#undef DEBUG_SECTION
