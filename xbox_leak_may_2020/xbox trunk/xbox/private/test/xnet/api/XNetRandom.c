/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetRandom.c

Abstract:

  This modules tests XNetRandom

Author:

  Steven Kehrli (steveke) 5-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

#define BUFFER_PATTERN       "feFEdcDCbaBA"
#define BUFFER_PATTERN_SIZE  12

typedef struct _XNETRANDOM_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bBuffer;                                 // bBuffer indicates if the buffer is valid
    UINT     nBufferSize;                             // nBufferSize indicates the size of the buffer
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETRANDOM_TABLE, *PXNETRANDOM_TABLE;

static XNETRANDOM_TABLE XNetRandomTable[] =
{
    { "33.1 Not Initialized",      FALSE, TRUE,  12,   WSANOTINITIALISED, FALSE },
    { "33.2 0 bytes",              TRUE,  TRUE,  0,    0,                 FALSE },
    { "33.3 12 bytes",             TRUE,  TRUE,  12,   0,                 FALSE },
    { "33.4 24 bytes",             TRUE,  TRUE,  24,   0,                 FALSE },
    { "33.5 1200 bytes",           TRUE,  TRUE,  1200, 0,                 FALSE },
    { "33.6 4800 bytes",           TRUE,  TRUE,  4800, 0,                 FALSE },
    { "33.7 NULL Buffer 0 bytes",  TRUE,  FALSE, 0,    0,                 FALSE },
    { "33.8 NULL Buffer",          TRUE,  FALSE, 12,   0,                 TRUE  },
    { "33.9 Not Initialized",      FALSE, TRUE,  12,   WSANOTINITIALISED, FALSE }
};

#define XNetRandomTableCount (sizeof(XNetRandomTable) / sizeof(XNETRANDOM_TABLE))



VOID
XNetRandomTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetRandom

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR  lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR  lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD  dwTableIndex;

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL   bXnetInitialized = FALSE;

    // pBuffer1 is the buffer
    BYTE   pBuffer1[6000];
    // pBuffer2 is the buffer
    BYTE   pBuffer2[6000];
    // dwFillBuffer is a counter to fill the buffer
    DWORD  dwFillBuffer;

    // bException indicates if an exception occurred
    BOOL   bException;
    // dwReturnCode is the return code of the operation
    int    dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL   bTestPassed;



    // Set the function name
    xSetFunctionName(hLog, "XNetRandom");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetRandom+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetRandom-");

    for (dwTableIndex = 0; dwTableIndex < XNetRandomTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetRandomTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetRandomTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetRandomTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetRandomTable[dwTableIndex].szVariationName);

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetRandomTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetRandomTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetRandomTable[dwTableIndex].bXnetInitialized;
        }

        // Clear the buffer
        ZeroMemory(pBuffer1, sizeof(pBuffer1));
        ZeroMemory(pBuffer2, sizeof(pBuffer2));

        // Initialize the buffer
        for (dwFillBuffer = 0; dwFillBuffer < sizeof(pBuffer1); dwFillBuffer += BUFFER_PATTERN_SIZE) {
            CopyMemory(&pBuffer1[dwFillBuffer], BUFFER_PATTERN, sizeof(pBuffer1) - dwFillBuffer > BUFFER_PATTERN_SIZE ? BUFFER_PATTERN_SIZE : sizeof(pBuffer1) - dwFillBuffer);
            CopyMemory(&pBuffer2[dwFillBuffer], BUFFER_PATTERN, sizeof(pBuffer2) - dwFillBuffer > BUFFER_PATTERN_SIZE ? BUFFER_PATTERN_SIZE : sizeof(pBuffer2) - dwFillBuffer);
        }

        bTestPassed = TRUE;
        bException = FALSE;

        __try {
            // Call XNetRandom
            dwReturnCode = XNetRandom((TRUE == XNetRandomTable[dwTableIndex].bBuffer) ? pBuffer1 : NULL, XNetRandomTable[dwTableIndex].nBufferSize);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetRandomTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetRandom RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetRandom caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetRandomTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetRandom did not RIP");
            }

            if (dwReturnCode != XNetRandomTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetRandomTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                // Check the buffer
                for (dwFillBuffer = 0; dwFillBuffer < sizeof(pBuffer1); dwFillBuffer += BUFFER_PATTERN_SIZE) {
                    if (0 == memcmp(&pBuffer1[dwFillBuffer], BUFFER_PATTERN, sizeof(pBuffer1) - dwFillBuffer > BUFFER_PATTERN_SIZE ? BUFFER_PATTERN_SIZE : sizeof(pBuffer1) - dwFillBuffer)) {
                        break;
                    }
                }

                if (dwFillBuffer != XNetRandomTable[dwTableIndex].nBufferSize) {
                    xLog(hLog, XLL_FAIL, "Buffer pattern - EXPECTED: %u; FOUND: %u", XNetRandomTable[dwTableIndex].nBufferSize, dwFillBuffer);
                    bTestPassed = FALSE;
                }

                if (0 != XNetRandomTable[dwTableIndex].nBufferSize) {
                    // Call XNetRandom again
                    dwReturnCode = XNetRandom(pBuffer2, XNetRandomTable[dwTableIndex].nBufferSize);

                    if (0 == memcmp(pBuffer1, pBuffer2, XNetRandomTable[dwTableIndex].nBufferSize)) {
                        xLog(hLog, XLL_FAIL, "Same random bytes");
                        bTestPassed = FALSE;
                    }
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetRandom succeeded");
                }
            }
            else {
                xLog(hLog, XLL_PASS, "dwReturnCode - OUT: %u", dwReturnCode);
            }

        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate net subsystem if necessary
    if (TRUE == bXnetInitialized) {
        XNetRelease();
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
