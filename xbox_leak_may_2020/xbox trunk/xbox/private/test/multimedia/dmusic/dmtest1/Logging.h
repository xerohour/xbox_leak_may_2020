#pragma once

#define LOG_DEBUG 1
#define LOG_FILE 2
#define LOG_ALL 3


#define ABORTLOGLEVEL 1
#define FYILOGLEVEL 2


HRESULT Log
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags,
    LPSTR                   szFormat,
    ...
);

//Logs everywhere.
HRESULT Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
);


/*
//So the test logging will still work.  Grrr.
HRESULT Log
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags, 
    LPSTR                   szFormat,
    ...
);

HRESULT Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
);

*/

void LogSetDbgLevel(DWORD dwLevel);
void LogIncrementIndent(void);
void LogDecrementIndent(void);
HRESULT LogInit(void);
HRESULT LogFree(void);


extern DWORD g_dwPassed;
extern DWORD g_dwFailed;



//This macro is for those main test functions which call a bunch of other test functions (subtests).  This helps log
//  the subtest that failed, although these subtest results are not logged in the main test results.

#define DMTEST_EXECUTE( func )                                      \
{                                                                   \
HRESULT hrXXXCall;                                                  \
    hrXXXCall = (func);                                               \
	if ( FAILED( hrXXXCall) )                                       \
	{                                                               \
        Log(FYILOGLEVEL, "FAILED: "#func ## );                      \
        Log(FYILOGLEVEL, "-------");    \
        hr = hrXXXCall;                                             \
	}                                                               \
	else                                                            \
	{                                                               \
        Log(FYILOGLEVEL, "PASSED: "#func ## );                      \
        Log(FYILOGLEVEL, "-------");    \
	}                                                               \
}                                                                    

