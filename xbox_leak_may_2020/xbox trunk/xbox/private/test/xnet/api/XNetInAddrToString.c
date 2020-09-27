/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetInAddrToString.c

Abstract:

  This modules tests XNetInAddrToString

Author:

  Steven Kehrli (steveke) 21-Jun-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETINADDRTOSTRING_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    IN_ADDR  in;                                      // in is the IPv4 network address
    BOOL     bBuffer;                                 // bBuffer indicates if the string buffer is valid
    int      nBufferSize;                             // nBufferSize indicates the size of the string buffer
    char     *cp;                                     // cp is the IPv4 dotted address string
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETINADDRTOSTRING_TABLE, *PXNETINADDRTOSTRING_TABLE;

static XNETINADDRTOSTRING_TABLE XNetInAddrToStringTable[] =
{
    { "32.1 0x10020304",      {0x04, 0x03, 0x02, 0x10}, TRUE,  16, "4.3.2.16",        0, FALSE },
    { "32.2 0x00000000",      {0x00, 0x00, 0x00, 0x00}, TRUE,  16, "0.0.0.0",         0, FALSE },
    { "32.3 0xFF020304",      {0x04, 0x03, 0x02, 0xFF}, TRUE,  16, "4.3.2.255",       0, FALSE },
    { "32.4 0x10FF0304",      {0x04, 0x03, 0xFF, 0x10}, TRUE,  16, "4.3.255.16",      0, FALSE },
    { "32.5 0x1002FF04",      {0x04, 0xFF, 0x02, 0x10}, TRUE,  16, "4.255.2.16",      0, FALSE },
    { "32.6 0x100203FF",      {0xFF, 0x03, 0x02, 0x10}, TRUE,  16, "255.3.2.16",      0, FALSE },
    { "32.7 0xFFFFFFFF",      {0xFF, 0xFF, 0xFF, 0xFF}, TRUE,  16, "255.255.255.255", 0, FALSE },
    { "32.8 0x9FBFDFFF",      {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  16, "159.191.223.255", 0, FALSE },
    { "32.9 NULL Buffer",     {0x9F, 0xBF, 0xDF, 0xFF}, FALSE, 16, "159.191.223.255", 0, TRUE  },
    { "32.10 Large Buffer",   {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  17, "159.191.223.255", 0, FALSE },
    { "32.11 Exact Buffer",   {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  16, "159.191.223.255", 0, FALSE },
    { "32.12 Small Buffer",   {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  15, "159.191.223.25",  0, FALSE },
    { "32.13 Zero Buffer",    {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  0,  "159.191.223.255", 0, TRUE  },
    { "32.14 Neg Buffer",     {0x9F, 0xBF, 0xDF, 0xFF}, TRUE,  -1, "159.191.223.255", 0, TRUE  },
};

#define XNetInAddrToStringTableCount (sizeof(XNetInAddrToStringTable) / sizeof(XNETINADDRTOSTRING_TABLE))



VOID
XNetInAddrToStringTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetInAddrToString

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR    lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR    lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD    dwTableIndex;

    // cpBuffer is the string buffer
    char     cpBuffer[16];

    // bException indicates if an exception occurred
    BOOL     bException;
    // dwReturnCode is the return code of the operation
    int      dwReturnCode;

    // szFunctionName is the function name
    CHAR     szFunctionName[FUNCTION_NAME_LENGTH];



    // Set the function name
    xSetFunctionName(hLog, "XNetInAddrToString");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetInAddrToString+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetInAddrToString-");

    for (dwTableIndex = 0; dwTableIndex < XNetInAddrToStringTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetInAddrToStringTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetInAddrToStringTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetInAddrToStringTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetInAddrToStringTable[dwTableIndex].szVariationName);

        // Clear the buffer
        ZeroMemory(cpBuffer, sizeof(cpBuffer));

        bException = FALSE;

        __try {
            // Call XNetInAddrToString
            dwReturnCode = XNetInAddrToString(XNetInAddrToStringTable[dwTableIndex].in, (TRUE == XNetInAddrToStringTable[dwTableIndex].bBuffer) ? cpBuffer : NULL, XNetInAddrToStringTable[dwTableIndex].nBufferSize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetInAddrToStringTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetInAddrToString RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetInAddrToString caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetInAddrToStringTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetInAddrToString did not RIP");
            }

            if (dwReturnCode != XNetInAddrToStringTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetInAddrToStringTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                if (0 != strcmp(cpBuffer, XNetInAddrToStringTable[dwTableIndex].cp)) {
                    xLog(hLog, XLL_FAIL, "EXPECTED: %s; RECEIVED: %s", XNetInAddrToStringTable[dwTableIndex].cp, cpBuffer);
                }
                else {
                    xLog(hLog, XLL_PASS, "OUT: %s", cpBuffer);
                }
            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }

        }

        // End the variation
        xEndVariation(hLog);
    }

    // Free the list of test cases
    if (NULL != lpszCaseSkip) {
        xMemFree(hMemObject, lpszCaseSkip);
    }

    if (NULL != lpszCaseTest) {
        xMemFree(hMemObject, lpszCaseTest);
    }
}

} // namespace XNetAPINamespace

#endif
