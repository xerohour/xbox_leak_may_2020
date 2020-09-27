/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  inet_addr.c

Abstract:

  This modules tests inet_addr

Author:

  Steven Kehrli (steveke) 11-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetAPINamespace;

#ifdef XNETAPI_CLIENT

namespace XNetAPINamespace {

typedef struct _INET_ADDR_TABLE {
    CHAR    szVariationName[VARIATION_NAME_LENGTH];  // szVariationName is the variation name
    char    *cp;                                     // cp is the IPv4 dotted address string
    u_long  addr;                                    // addr is the u_long representation of the address given
    BOOL    bRIP;                                    // Specifies a RIP test case
} INET_ADDR_TABLE, *PINET_ADDR_TABLE;

static INET_ADDR_TABLE inet_addrTable[] =
{
    { "5.1 \"0.0.0.0\"",              "0.0.0.0",             0x00000000,  FALSE },
    { "5.2 \"4.3.2.16\"",             "4.3.2.16",            0x10020304,  FALSE },
    { "5.3 \"255.255.255.255\"",      "255.255.255.255",     0xFFFFFFFF,  FALSE },
    { "5.4 \"4.3.2.256\"",            "4.3.2.256",           0x00020304,  FALSE },
    { "5.5 \"4.3.256.16\"",           "4.3.256.16",          0x10000304,  FALSE },
    { "5.6 \"4.256.2.16\"",           "4.256.2.16",          0x10020004,  FALSE },
    { "5.7 \"256.3.2.16\"",           "256.3.2.16",          0x10020300,  FALSE },
    { "5.8 \"000.000.000.000\"",      "000.000.000.000",     0x00000000,  FALSE },
    { "5.9 \"004.003.002.020\"",      "004.003.002.020",     0x10020304,  FALSE },
    { "5.10 \"0377.0377.0377.0377\"", "0377.0377.0377.0377", 0xFFFFFFFF,  FALSE },
    { "5.11 \"004.003.002.0400\"",    "004.003.002.0400",    0x00020304,  FALSE },
    { "5.12 \"004.003.0400.020\"",    "004.003.0400.020",    0x10000304,  FALSE },
    { "5.13 \"004.0400.002.020\"",    "004.0400.002.020",    0x10020004,  FALSE },
    { "5.14 \"0400.003.002.020\"",    "0400.003.002.020",    0x10020300,  FALSE },
    { "5.15 \"0x0.0x0.0x0.0x0\"",     "0x0.0x0.0x0.0x0",     0x00000000,  FALSE },
    { "5.16 \"0x4.0x3.0x2.0x10\"",    "0x4.0x3.0x2.0x10",    0x10020304,  FALSE },
    { "5.17 \"0xFF.0xFF.0xFF.0xFF\"", "0xFF.0xFF.0xFF.0xFF", 0xFFFFFFFF,  FALSE },
    { "5.18 \"0x4.0x3.0x2.0x100\"",   "0x4.0x3.0x2.0x100",   0x00020304,  FALSE },
    { "5.19 \"0x4.0x3.0x100.0x10\"",  "0x4.0x3.0x100.0x10",  0x10000304,  FALSE },
    { "5.20 \"0x4.0x100.0x2.0x10\"",  "0x4.0x100.0x2.0x10",  0x10020004,  FALSE },
    { "5.21 \"0x100.0x3.0x2.0x10\"",  "0x100.0x3.0x2.0x10",  0x10020300,  FALSE },
    { "5.22 \"0.000.0.0x0\"",         "0.000.0.0x0",         0x00000000,  FALSE },
    { "5.23 \"000.0.0x0.0\"",         "000.0.0x0.0",         0x00000000,  FALSE },
    { "5.24 \"0.0x0.0.000\"",         "0.0x0.0.000",         0x00000000,  FALSE },
    { "5.25 \"0x0.0.000.0\"",         "0x0.0.000.0",         0x00000000,  FALSE },
    { "5.26 \"4.003.2.0x10\"",        "4.003.2.0x10",        0x10020304,  FALSE },
    { "5.27 \"004.3.0x2.16\"",        "004.3.0x2.16",        0x10020304,  FALSE },
    { "5.28 \"4.0x3.2.020\"",         "4.0x3.2.020",         0x10020304,  FALSE },
    { "5.29 \"0x4.3.002.16\"",        "0x4.3.002.16",        0x10020304,  FALSE },
    { "5.30 \"255.0377.255.0xFF\"",   "255.0377.255.0xFF",   0xFFFFFFFF,  FALSE },
    { "5.31 \"0377.255.0xFF.255\"",   "0377.255.0xFF.255",   0xFFFFFFFF,  FALSE },
    { "5.32 \"255.0xFF.255.0377\"",   "255.0xFF.255.0377",   0xFFFFFFFF,  FALSE },
    { "5.33 \"0xFF.255.0377.255\"",   "0xFF.255.0377.255",   0xFFFFFFFF,  FALSE },
    { "5.34 \"4.0400.2.0x10\"",       "4.0400.2.0x10",       0x10020004,  FALSE },
    { "5.35 \"004.3.0x100.16\"",      "004.3.0x100.16",      0x10000304,  FALSE },
    { "5.36 \"256.0x3.2.020\"",       "256.0x3.2.020",       0x10020300,  FALSE },
    { "5.37 \"0x4.3.002.256\"",       "0x4.3.002.256",       0x00020304,  FALSE },
    { "5.38 \"0.0.0\"",               "0.0.0",               0x00000000,  FALSE },
    { "5.39 \"4.3.2\"",               "4.3.2",               0x02000304,  FALSE },
    { "5.40 \"255.255.255\"",         "255.255.255",         0xFF00FFFF,  FALSE },
    { "5.41 \"4.3.256\"",             "4.3.256",             0x00010304,  FALSE },
    { "5.42 \"4.3.65536\"",           "4.3.65536",           0x00000304,  FALSE },
    { "5.43 \"0.0\"",                 "0.0",                 0x00000000,  FALSE },
    { "5.44 \"4.3\"",                 "4.3",                 0x03000004,  FALSE },
    { "5.45 \"255.255\"",             "255.255",             0xFF0000FF,  FALSE },
    { "5.46 \"4.256\"",               "4.256",               0x00010004,  FALSE },
    { "5.47 \"4.16777216\"",          "4.16777216",          0x00000004,  FALSE },
    { "5.48 \"0\"",                   "0",                   0x00000000,  FALSE },
    { "5.49 \"4\"",                   "4",                   0x04000000,  FALSE },
    { "5.50 \"255\"",                 "255",                 0xFF000000,  FALSE },
    { "5.51 \"4294967296\"",          "4294967296",          0x00000000,  FALSE },
    { "5.52 \"4.3.2.16abcdef\"",      "4.3.2.16abcdef",      INADDR_NONE, FALSE },
    { "5.53 \"4.3.2abcdef.16\"",      "4.3.2abcdef.16",      INADDR_NONE, FALSE },
    { "5.54 \"4.3abcdef.2.16\"",      "4.3abcdef.2.16",      INADDR_NONE, FALSE },
    { "5.55 \"4abcdef.3.2.16\"",      "4abcdef.3.2.16",      INADDR_NONE, FALSE },
    { "5.56 \"4.3.2.abcdef16\"",      "4.3.2.abcdef16",      INADDR_NONE, FALSE },
    { "5.57 \"4.3.abcdef2.16\"",      "4.3.abcdef2.16",      INADDR_NONE, FALSE },
    { "5.58 \"4.abcdef3.2.16\"",      "4.abcdef3.2.16",      INADDR_NONE, FALSE },
    { "5.59 \"abcdef4.3.2.16\"",      "abcdef4.3.2.16",      INADDR_NONE, FALSE },
    { "5.60 \"4.3.2.16 \"",           "4.3.2.16 ",           0x10020304,  FALSE },
    { "5.61 \"4.3.2 .16\"",           "4.3.2 .16",           0x10020304,  FALSE },
    { "5.62 \"4.3 .2.16\"",           "4.3 .2.16",           0x10020304,  FALSE },
    { "5.63 \"4 .3.2.16\"",           "4 .3.2.16",           0x10020304,  FALSE },
    { "5.64 \"4.3.2. 16\"",           "4.3.2. 16",           0x10020304,  FALSE },
    { "5.65 \"4.3. 2.16\"",           "4.3. 2.16",           0x10020304,  FALSE },
    { "5.66 \"4. 3.2.16\"",           "4. 3.2.16",           0x10020304,  FALSE },
    { "5.67 \" 4.3.2.16\"",           " 4.3.2.16",           0x10020304,  FALSE },
    { "5.68 \"\"",                    "",                    0x00000000,  FALSE },
    { "5.69 \" \"",                   " ",                   0x00000000,  FALSE },
    { "5.70 NULL",                    NULL,                  WSAEFAULT,   TRUE  }
};

#define inet_addrTableCount (sizeof(inet_addrTable) / sizeof(INET_ADDR_TABLE))



VOID
inet_addrTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests inet_addr

Arguments:

  hLog - handle to the xLog log object
  hMemObject - handle to the memory object
  bRIPs - specifies RIP testing

------------------------------------------------------------------------------*/
{
    // lpszCaseTest is a pointer to the list of cases to test
    LPSTR   lpszCaseTest = NULL;
    // lpszCaseSkip is a pointer to the list of cases to skip
    LPSTR   lpszCaseSkip = NULL;
    // dwTableIndex is a counter to enumerate each entry in a test table
    DWORD   dwTableIndex;

    // addr is the returned u_long value
    u_long  addr;

    // bException indicates if an exception occurred
    BOOL    bException;



    // Set the function name
    xSetFunctionName(hLog, "inet_addr");

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_inet_addr+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_inet_addr-");

    for (dwTableIndex = 0; dwTableIndex < inet_addrTableCount; dwTableIndex++) {
        if ((NULL != lpszCaseSkip) && (TRUE == ParseAndFindString(lpszCaseSkip, inet_addrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if ((NULL != lpszCaseTest) && (FALSE == ParseAndFindString(lpszCaseTest, inet_addrTable[dwTableIndex].szVariationName))) {
            continue;
        }

        if (bRIPs != inet_addrTable[dwTableIndex].bRIP) {
            continue;
        }

        // Start the variation
        xStartVariation(hLog, inet_addrTable[dwTableIndex].szVariationName);

        bException = FALSE;

        __try {
            // Call inet_addr
            addr = inet_addr(inet_addrTable[dwTableIndex].cp);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (TRUE == inet_addrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_PASS, "inet_addr RIP'ed");
            }
            else {
                xLog(hLog, XLL_EXCEPTION, "inet_addr caused an exception - ec = 0x%08x", GetExceptionCode());
            }
            bException = TRUE;
        }

        if (FALSE == bException) {
            if (TRUE == inet_addrTable[dwTableIndex].bRIP) {
                xLog(hLog, XLL_FAIL, "inet_addr did not RIP");
            }

            if (addr != inet_addrTable[dwTableIndex].addr) {
                xLog(hLog, XLL_FAIL, "EXPECTED: 0x%08x; RECEIVED: 0x%08x", inet_addrTable[dwTableIndex].addr, addr);
            }
            else {
                xLog(hLog, XLL_PASS, "OUT: 0x%08x", addr);
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
