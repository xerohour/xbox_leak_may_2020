/*

Copyright (c) 2000 Microsoft Corporation

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


#include <xlog.h>
#include "utils.h"

#define CACHE_DRIVE         L"Z"
#define TITLE_DRIVE         L"T"
#define USER_DRIVE          L"U"
#define BASE_DRIVE          CACHE_DRIVE

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

static void TESTPASS(HANDLE hLog, char *fmt, ...)
    {
    va_list args;
    va_start(args, fmt);
    xLog_va(hLog, XLL_PASS, fmt, args);
    }
static void TESTFAIL(HANDLE hLog, char *fmt, ...)
    {
    va_list args;
    va_start(args, fmt);
    xLog_va(hLog, XLL_FAIL, fmt, args);
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
    xStartVariation(hLog, variant);
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

    xStartVariation(hLog, aVar);

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

#define ALLCHARS L"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f" \
    L"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff\x00"

// sizes
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)
#define PAGESIZE (4*KILOBYTE)

// common numbers
static DWORD dwBogus = 0xF000BAAA;
static ULARGE_INTEGER ulBogus = { 0xF000BAAA, 0xF000BAAA };

// common HANDLE parameters
static HANDLE hNull = NULL;
static HANDLE hBogus = (void*)0xF000BAAA;

// common string parameters
static LPCTSTR lpNull = NULL;
static LPCTSTR lpEmpty = L"";
static LPCTSTR lpSpace = L" ";
static LPCTSTR lpBogus = (const unsigned short*)0xF000BAAA;
static LPCTSTR lpCurrentDir = L".";
static LPCTSTR lpParentDir = L"..";
static LPCTSTR lpUnderMaxPathDir = BASE_DRIVE L":\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory";
static LPCTSTR lpOverMaxPathDir = BASE_DRIVE L":\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory\\directory";

// security attributes
static SECURITY_DESCRIPTOR sdEmpty = { 0, 0, 0, 0, 0, 0, 0 };
static SECURITY_DESCRIPTOR sdBogus = { 0x69, 0x69, 0xF000, (void*)0xF000BAAA, (void*)0xF000BAAA, (PACL)0xF000BAAA, (PACL)0xF000BAAA};
static SECURITY_ATTRIBUTES saEmpty = { 0, 0, 0 };
static SECURITY_ATTRIBUTES saBogus = { 0xF000BAAA, (PISECURITY_DESCRIPTOR)0xF000BAAA, 0xF000BAAA};
static SECURITY_ATTRIBUTES saEmptyWithSize = { sizeof(SECURITY_ATTRIBUTES), 0, 0 };
static SECURITY_ATTRIBUTES saWithEmptySD = { sizeof(SECURITY_ATTRIBUTES), &sdEmpty, 0 };
static SECURITY_ATTRIBUTES saWithBogusSD = { sizeof(SECURITY_ATTRIBUTES), &sdBogus, 0 };

// device names
static WCHAR *deviceNames[] = {
    BASE_DRIVE L":\\prn",
    BASE_DRIVE L":\\aux",
    BASE_DRIVE L":\\con",
    BASE_DRIVE L":\\lpt1",
    L"\\\\?\\prn",
    L"\\\\?\\aux",
    L"\\\\?\\con",
    L"\\\\?\\lpt1",
    L"\\\\lpt1",
    L"CONOUT$",
    L"CONIN$",
    };

// invalid file names
static WCHAR *invalidFileNames[] = {
    BASE_DRIVE L":\\abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcd",
    BASE_DRIVE L":\\)#+äßl+«RÉI±±+Td¦ª¦<ç\x12>Ö",
    BASE_DRIVE L":\\~`!@#$%^&*()-=_+[]{}\\|';:\",./?><",
    BASE_DRIVE L":\\*",
    BASE_DRIVE L":\\?",
    BASE_DRIVE L":\\" ALLCHARS,
    BASE_DRIVE L":\\abc?",
    BASE_DRIVE L":\\abc>def",
    BASE_DRIVE L":\\abc<def",
    BASE_DRIVE L":\\abc|def",
    BASE_DRIVE L":\\abc:def",
    BASE_DRIVE L":\\abc\"def",
    BASE_DRIVE L":\\>",
    BASE_DRIVE L":\\<",
    BASE_DRIVE L":\\:",
    BASE_DRIVE L":\\|",
    BASE_DRIVE L":\\\"",
    BASE_DRIVE L":\\..\\abcdef",
    BASE_DRIVE L":\\.\\..\\abcdef",
    BASE_DRIVE L":\\..\\..\\..\\..\\..\\..\\..\\..\\..\\abcdef",
    BASE_DRIVE L":\\abc" BASE_DRIVE L":\\def",
    BASE_DRIVE L":\\new\nline",
    BASE_DRIVE L":\\newline\n",
    BASE_DRIVE L":\\Carriage\rreturn",
    BASE_DRIVE L":\\Carriagereturn\r",
    BASE_DRIVE L":\\\ttab",
    BASE_DRIVE L":\\tab\t",
    BASE_DRIVE L":\\\abell",
    BASE_DRIVE L":\\bell\a",
    BASE_DRIVE BASE_DRIVE L":\\abc",
    BASE_DRIVE L"::\\abc",
    BASE_DRIVE L"abc",
    BASE_DRIVE L".",
    BASE_DRIVE L"..",
    BASE_DRIVE L" ",
    L"\\\\?\\" BASE_DRIVE L":\\)#+äßl+«RÉI±±+Td¦ª¦<ç\x12>Ö",
    L"\\\\?\\" BASE_DRIVE L":\\~`!@#$%^&*()-=_+[]{}\\|';:\",./?><",
    L"\\\\?\\" BASE_DRIVE L":\\*",
    L"\\\\?\\" BASE_DRIVE L":\\?",
    L"\\\\?\\" BASE_DRIVE L":\\" ALLCHARS,
    L"\\\\?\\" BASE_DRIVE L":\\..\\abcdef",
    L"\\\\?\\" BASE_DRIVE L":\\.\\..\\abcdef",
    L"\\\\?\\" BASE_DRIVE L":\\new\nline",
    L"\\\\?\\" BASE_DRIVE L":\\Carriage\rreturn",
    L"\\\\?\\" BASE_DRIVE L":\\\ttab",
    L"\\\\?\\" BASE_DRIVE BASE_DRIVE L":\\abc",
    L"\\\\?\\" BASE_DRIVE L"::\\abc",
    L"\\\\?\\" BASE_DRIVE L":\\\\abc",
    L"\\\\?\\" BASE_DRIVE L"abc",
    L"\\\\?\\abc",
    L"\\\\?\\ ",
    L"\\\\?\\/",
    L"\\\\?\\\\",
    L"\\\\?\\.",
    L"\\\\?\\..",
    L" ",
    L"/",
    L"\\",
    L".",
    L".."
    };


