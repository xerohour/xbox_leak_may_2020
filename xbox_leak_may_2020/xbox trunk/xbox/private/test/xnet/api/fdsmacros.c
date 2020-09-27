/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  fdsmacros.c

Abstract:

  This modules tests the fds macros

Author:

  Steven Kehrli (steveke) 12-Mar-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef XNETAPI_CLIENT

using namespace XNetAPINamespace;

namespace XNetAPINamespace {



VOID
fdsmacrosTest(
    IN HANDLE  hLog,
    IN HANDLE  hMemObject,
    IN BOOL    bRIPs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests the fds macros

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

    // sSocket is the socket descriptor
    SOCKET  sSocket;

    // fds is the set
    fd_set  fds;



    // Set the function name
    xSetFunctionName(hLog, "fds macros");

    // Initialize the net subsystem
    XNetAddRef();

    // Get the test cases
    lpszCaseTest = GetIniSection(hMemObject, "xnetapi_fdsmacros+");
    lpszCaseSkip = GetIniSection(hMemObject, "xnetapi_fdsmacros-");

    sSocket = INVALID_SOCKET;

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.1 Initialize Set"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.1 Initialize Set"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.1 Initialize Set");

        // Zero the set
        FD_ZERO(&fds);

        // Check if socket is in the set
        if (0 != FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is not set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.2 Set sSocket"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.2 Set sSocket"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.2 Set sSocket");

        // Zero the set
        FD_ZERO(&fds);

        // Set sSocket
        FD_SET(sSocket, &fds);

        // Check if socket is in the set
        if (0 == FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is not set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.3 Set Clear sSocket"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.3 Set Clear sSocket"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.3 Set Clear sSocket");

        // Zero the set
        FD_ZERO(&fds);

        // Set sSocket
        FD_SET(sSocket, &fds);

        // Check if socket is in the set
        if (0 == FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is not set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is set");
        }

        // Clear sSocket
        FD_CLR(sSocket, &fds);

        // Check if socket is in the set
        if (0 != FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is not set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.4 Set Zero sSocket"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.4 Set Zero sSocket"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.4 Set Zero sSocket");

        // Zero the set
        FD_ZERO(&fds);

        // Set sSocket
        FD_SET(sSocket, &fds);

        // Check if socket is in the set
        if (0 == FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is not set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is set");
        }

        // Zero sSocket
        FD_ZERO(&fds);

        // Check if socket is in the set
        if (0 != FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is not set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.5 Set Set sSocket"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.5 Set Set sSocket"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.5 Set Set sSocket");

        // Zero the set
        FD_ZERO(&fds);

        // Set sSocket
        FD_SET(sSocket, &fds);

        // Check if socket is in the set
        if (0 == FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is not set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is set");
        }

        // Set sSocket
        FD_SET(sSocket, &fds);

        // Check if socket is in the set
        if (0 == FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is not set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    if (((NULL == lpszCaseSkip) || (FALSE == ParseAndFindString(lpszCaseSkip, "27.6 Clear sSocket"))) && ((NULL == lpszCaseTest) || (TRUE == ParseAndFindString(lpszCaseTest, "27.6 Clear sSocket"))) && (FALSE == bRIPs)) {
        // Start the variation
        xStartVariation(hLog, "27.6 Clear sSocket");

        // Zero the set
        FD_ZERO(&fds);

        // Clear sSocket
        FD_CLR(sSocket, &fds);

        // Check if socket is in the set
        if (0 != FD_ISSET(sSocket, &fds)) {
            xLog(hLog, XLL_FAIL, "sSocket is set");
        }
        else {
            xLog(hLog, XLL_PASS, "sSocket is not set");
        }

        // End the variation
        xEndVariation(hLog);
    }

    // Terminate net subsystem if necessary
    XNetRelease();

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
