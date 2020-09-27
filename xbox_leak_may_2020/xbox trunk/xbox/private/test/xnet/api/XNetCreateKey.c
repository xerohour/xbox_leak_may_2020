/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  XNetCreateKey.c

Abstract:

  This modules tests XNetCreateKey

Author:

  Steven Kehrli (steveke) 5-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _XNETCREATEKEY_TABLE {
    CHAR     szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    BOOL     bXnetInitialized;                        // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bXnKid;                                  // bXnKid indicates if the xnkid is valid
    BOOL     bXnKey;                                  // bXnKey indicates if the xnkey is valid
    DWORD    dwReturnCode;                            // dwReturnCode is the return code of the operation
    BOOL     bRIP;                                    // Specifies a RIP test case
} XNETCREATEKEY_TABLE, *PXNETCREATEKEY_TABLE;

static XNETCREATEKEY_TABLE XNetCreateKeyTable[] =
{
    { "34.1 Not Initialized",      FALSE, TRUE,  TRUE,  WSANOTINITIALISED, FALSE },
    { "34.2 Valid",                TRUE,  TRUE,  TRUE,  0,                 FALSE },
    { "34.3 NULL xnkid",           TRUE,  FALSE, TRUE,  0,                 TRUE  },
    { "34.4 NULL xnkey",           TRUE,  TRUE,  FALSE, 0,                 TRUE  },
    { "34.5 NULL xnkid and xnkey", TRUE,  TRUE,  FALSE, 0,                 TRUE  },
    { "34.6 Not Initialized",      FALSE, TRUE,  TRUE,  WSANOTINITIALISED, FALSE }
};

#define XNetCreateKeyTableCount (sizeof(XNetCreateKeyTable) / sizeof(XNETCREATEKEY_TABLE))



VOID
XNetCreateKeyTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests XNetCreateKey

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

    // bXnetInitialized indicates if the net subsystem is initialized
    BOOL     bXnetInitialized = FALSE;

    // XnKid is the xnkid
    XNKID    XnKid;
    XNKID    XnKid0;
    // XnKey is the xnkey
    XNKEY    XnKey;
    XNKEY    XnKey0;

    // bException indicates if an exception occurred
    BOOL     bException;
    // dwReturnCode is the return code of the operation
    int      dwReturnCode;
    // bTestPassed indicates if the test passed
    BOOL     bTestPassed;



    // Set the function name
    xSetFunctionName(hLog, "XNetCreateKey");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_XNetCreateKey+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_XNetCreateKey-");

    for (dwTableIndex = 0; dwTableIndex < XNetCreateKeyTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, XNetCreateKeyTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, XNetCreateKeyTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != XNetCreateKeyTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, XNetCreateKeyTable[dwTableIndex].szVariationName);

        // Check the state of the net subsystem
        if (bXnetInitialized != XNetCreateKeyTable[dwTableIndex].bXnetInitialized) {
            // Initialize or terminate net subsystem as necessary
            if (TRUE == XNetCreateKeyTable[dwTableIndex].bXnetInitialized) {
                XNetAddRef();
            }
            else {
                XNetRelease();
            }

            // Update the state of net subsystem
            bXnetInitialized = XNetCreateKeyTable[dwTableIndex].bXnetInitialized;
        }

        // Clear the xnkid and xnkey
        ZeroMemory(&XnKid, sizeof(XnKid));
        ZeroMemory(&XnKid0, sizeof(XnKid0));
        ZeroMemory(&XnKey, sizeof(XnKey));
        ZeroMemory(&XnKey0, sizeof(XnKey0));

        bTestPassed = TRUE;
        bException = FALSE;

        __try {
            // Call XNetCreateKey
            dwReturnCode = XNetCreateKey((TRUE == XNetCreateKeyTable[dwTableIndex].bXnKid) ? &XnKid : NULL, (TRUE == XNetCreateKeyTable[dwTableIndex].bXnKey) ? &XnKey : NULL);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == XNetCreateKeyTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "XNetCreateKey RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "XNetCreateKey caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == XNetCreateKeyTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "XNetCreateKey did not RIP");
            }

            if (dwReturnCode != XNetCreateKeyTable[dwTableIndex].dwReturnCode) {
                xLog(hLog, XLL_FAIL, "dwReturnCode - EXPECTED: %u; RECEIVED: %u", XNetCreateKeyTable[dwTableIndex].dwReturnCode, dwReturnCode);
            }
            else if (0 == dwReturnCode) {
                // Check the xnkid
                if (0 == memcmp(&XnKid0, &XnKid, sizeof(XnKid))) {
                    xLog(hLog, XLL_FAIL, "XnKid zero buffer");
                    bTestPassed = FALSE;
                }

                // Check the xnkey
                if (0 == memcmp(&XnKey0, &XnKey, sizeof(XnKey))) {
                    xLog(hLog, XLL_FAIL, "XnKey zero buffer");
                    bTestPassed = FALSE;
                }

                // Copy the xnkid and xnkey
                CopyMemory(&XnKid0, &XnKid, sizeof(XnKid));
                CopyMemory(&XnKey0, &XnKey, sizeof(XnKey));

                // Call XNetCreateKey again
                dwReturnCode = XNetCreateKey(&XnKid, &XnKey);

                // Check the xnkid
                if (0 == memcmp(&XnKid0, &XnKid, sizeof(XnKid))) {
                    xLog(hLog, XLL_FAIL, "XnKid zero buffer");
                    bTestPassed = FALSE;
                }

                // Check the xnkey
                if (0 == memcmp(&XnKey0, &XnKey, sizeof(XnKey))) {
                    xLog(hLog, XLL_FAIL, "XnKey zero buffer");
                    bTestPassed = FALSE;
                }

                if (TRUE == bTestPassed) {
                    xLog(hLog, XLL_PASS, "XNetCreateKey succeeded");
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
