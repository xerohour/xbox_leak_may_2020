/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    commontest.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/


#define CACHE_DRIVE         "Z"
#define TITLE_DRIVE         "T"
#define USER_DRIVE          "U"
#define BASE_DRIVE          CACHE_DRIVE

extern void DebugPrint(char* format, ...);

#pragma warning(push)
#pragma warning(disable : 4200) // disable warning on zero sized array
struct UsbInitInfo
    {
    unsigned variation;
    char data[0];
    };
#pragma warning(pop)


static char* GetExceptionString(DWORD dwCode);
static void TESTPASS(HANDLE hLog, char *fmt, ...);
static void TESTFAIL(HANDLE hLog, char *fmt, ...);
static DWORD ExceptionHandler(HANDLE hLog, LPEXCEPTION_POINTERS exceptionInfo);
static void StartTest(HANDLE hLog, WCHAR *variant);
static void StartTest(HANDLE hLog, char *variant);

#define TESTCASE(variant)   StartTest(hLog, variant); SetLastError(0); __try { do
#define ENDTESTCASE         while(0); } __except(ExceptionHandler(hLog, GetExceptionInformation())) {  } xEndVariation(hLog);
#define ENDTESTCASE_CLEAN(c)while(0); } __except(ExceptionHandler(hLog, GetExceptionInformation())) {  } c xEndVariation(hLog);
#define BLOCKED(bugid, msg) xLog(hLog, XLL_BLOCK, "BugID %d - %s", bugid, msg); break;
#define BUGGED(bugid, msg)  xLog(hLog, XLL_FAIL, "BugID %d - %s", bugid, msg); break;
#define BUGGEDPASS(bugid, msg)  xLog(hLog, XLL_PASS, "BugID %d - %s", bugid, msg); break;

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#define NEWSIZE(ptr)        ( ((unsigned int*)(ptr))[-4] )

extern char *logVariationStr;

#define XLL_EXCEPTION             0x00000001L  // Enables logging of exception events
#define XLL_BREAK                 0x00000002L  // Enables logging to break into the debugger
#define XLL_FAIL                  0x00000003L  // Enables logging of failure events
#define XLL_WARN                  0x00000004L  // Enables logging of warning events
#define XLL_BLOCK                 0x00000005L  // Enables logging of blocking events
#define XLL_PASS                  0x00000006L  // Enables logging of pass events
#define XLL_INFO                  0x00000007L  // Enables logging of informational events

#define XLO_DEFAULT               0x00000000L  // Opens log file in append mode
#define XLO_REFRESH               0x00000008L  // Creates a new log file.  If the log file already exists, overwrites the file and clears the existing attributes.

static void xEndVariation(HANDLE hLog)
    {
    }

static HANDLE xCreateLog(char *filename, DWORD createFlag)
    {
    if(createFlag == XLO_REFRESH)
        return fopen(filename, "w+");
    else
        return fopen(filename, "a+");
    }
static void xCloseLog(HANDLE hLog)
    {
    fclose((FILE*)hLog);
    }

static void xLog(HANDLE hLog, DWORD level, char *fmt, ...)
    {
    char *levels[] =
        {
        "UNKNOWN",
        "EXCEPTION",
        "BREAK",
        "FAIL",
        "WARN",
        "BLOCK",
        "PASS",
        "INFO"
        };

    va_list args;
    va_start(args, fmt);
    char szBuffer[1024];
    vsprintf(szBuffer, fmt, args);
    fprintf((FILE*)hLog, "0000,00:00:00,%s,USB,Init/Link Config,XInitDevices,%s,%s\n", levels[level], logVariationStr, szBuffer);
    va_end(args);
    }
static void xLogVA(HANDLE hLog, DWORD level, char *fmt, va_list args)
    {
    char *levels[] =
        {
        "UNKNOWN",
        "EXCEPTION",
        "BREAK",
        "FAIL",
        "WARN",
        "BLOCK",
        "PASS",
        "INFO"
        };

    char szBuffer[1024];
    vsprintf(szBuffer, fmt, args);
    fprintf((FILE*)hLog, "0000,00:00:00,%s,USB,Init/Link Config,XInitDevices,%s,%s\n", levels[level], logVariationStr, szBuffer);
    va_end(args);
    }

static void TESTPASS(HANDLE hLog, char *fmt, ...)
    {
    va_list args;
    va_start(args, fmt);
    xLogVA(hLog, XLL_PASS, fmt, args);
    }
static void TESTFAIL(HANDLE hLog, char *fmt, ...)
    {
    va_list args;
    va_start(args, fmt);
    xLogVA(hLog, XLL_FAIL, fmt, args);
    }
static DWORD ExceptionHandler(HANDLE hLog, LPEXCEPTION_POINTERS exceptionInfo)
    {
    if(exceptionInfo->ExceptionRecord->ExceptionCode == 0xCAFEBEEF) return EXCEPTION_EXECUTE_HANDLER;

    xLog(hLog, XLL_EXCEPTION, "Code: 0x%08X %s (Exception Pointer: 0x%08X)", exceptionInfo->ExceptionRecord->ExceptionCode, GetExceptionString(exceptionInfo->ExceptionRecord->ExceptionCode), exceptionInfo);
    DebugPrint("  Exception Caught - Code: 0x%08X %s (Exception Pointer: 0x%08X)\n", exceptionInfo->ExceptionRecord->ExceptionCode, GetExceptionString(exceptionInfo->ExceptionRecord->ExceptionCode), exceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
    }
static void StartTest(HANDLE hLog, char *variant)
    {
    logVariationStr = variant;
    }
static void StartTest(HANDLE hLog, WCHAR *variant)
    {
    // UNICODE to ANSI hack
    size_t len = wcslen(variant);
    char *aVar = new char[len+1];
    if(!aVar) return;
    for(size_t i=0; i<len+1; i++)
        {
        aVar[i] = (char)variant[i];
        }

    StartTest(hLog, (char*)aVar);

    delete[] aVar;
    }
static char* GetExceptionString(DWORD dwCode)
    {
    #define EXCEPTION( x ) case EXCEPTION_##x: return #x;

    static char szBuffer[512] = { 0 };

    switch(dwCode)
        {
        EXCEPTION( ACCESS_VIOLATION )
        EXCEPTION( DATATYPE_MISALIGNMENT )
        EXCEPTION( BREAKPOINT )
        EXCEPTION( SINGLE_STEP )
        EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
        EXCEPTION( FLT_DENORMAL_OPERAND )
        EXCEPTION( FLT_DIVIDE_BY_ZERO )
        EXCEPTION( FLT_INEXACT_RESULT )
        EXCEPTION( FLT_INVALID_OPERATION )
        EXCEPTION( FLT_OVERFLOW )
        EXCEPTION( FLT_STACK_CHECK )
        EXCEPTION( FLT_UNDERFLOW )
        EXCEPTION( INT_DIVIDE_BY_ZERO )
        EXCEPTION( INT_OVERFLOW )
        EXCEPTION( PRIV_INSTRUCTION )
        EXCEPTION( IN_PAGE_ERROR )
        EXCEPTION( ILLEGAL_INSTRUCTION )
        EXCEPTION( NONCONTINUABLE_EXCEPTION )
        EXCEPTION( STACK_OVERFLOW )
        EXCEPTION( INVALID_DISPOSITION )
        EXCEPTION( GUARD_PAGE )
        EXCEPTION( INVALID_HANDLE )
        default:
            break;
        }

    sprintf(szBuffer, "Unknown Error");
    return szBuffer;
    }

// sizes
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)
#define PAGESIZE (4*KILOBYTE)
