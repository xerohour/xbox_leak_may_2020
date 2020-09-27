//
// Overload xLog() to also call xDebugString
//
#include <xtl.h>
#include <xlog.h>
#include <dxconio.h>

// Global variable to turn on/off logging to console
BOOL gbConsoleOut = FALSE;

BOOL
WINAPI
xLog(
	IN	BOOL	bConsoleOut,
    IN  HANDLE  hLog,
    IN  DWORD   dwLogLevel,
    IN  LPSTR   lpszFormatString_A,
    IN  ...
)
{
	va_list argptr;

	// xLog() stuff
	va_start(argptr, lpszFormatString_A);
	xLog_va(hLog, dwLogLevel, lpszFormatString_A, argptr);

	if(bConsoleOut) {
		// xDebugString stuff
		va_start(argptr, lpszFormatString_A);
		xvprintf(lpszFormatString_A, argptr);
		xDebugStringA("\n");
	}
	return TRUE;
}

//
// Overload xStarVariation() to also call xDebugString
//
BOOL
WINAPI
xStartVariation(
	IN	BOOL	bConsoleOut,
    IN  HANDLE  hLog,
    IN  LPSTR   lpszVariationName_A
)
{
	xStartVariation(hLog, lpszVariationName_A);

	if(bConsoleOut) {
		xDebugStringA("%s: Start\n", lpszVariationName_A);	
	}
	return TRUE;
}
