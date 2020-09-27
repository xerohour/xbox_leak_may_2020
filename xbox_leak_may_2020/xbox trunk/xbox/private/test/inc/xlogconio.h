//
// Overload xLog() to also call xDebugString
//
#include <dxconio.h>

extern gbConsoleOut;

BOOL
WINAPI
xLog(
	IN BOOL	bConsoleOut,
    IN HANDLE  hLog,
    IN DWORD   dwLogLevel,
    IN LPSTR   lpszFormatString_A,
    IN ...
);

//
// Overload xStarVariation() to also call xDebugString
//
BOOL
WINAPI
xStartVariation(
	IN BOOL	bConsoleOut,
    IN HANDLE  hLog,
    IN LPSTR   lpszVariationName_A
);
