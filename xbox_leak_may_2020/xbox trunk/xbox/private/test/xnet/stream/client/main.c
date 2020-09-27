/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.c

Abstract:

  This module streams data between client and server - executable

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XNetStreamNamespace;

void __cdecl main()
{
    // hLog is the handle to the xLog object
    HANDLE  hLog = INVALID_HANDLE_VALUE;



    // Create the xLog object
    hLog = xCreateLog(L"T:\\xnetstream.log", NULL, INVALID_SOCKET, XLL_LOGDEFAULT, XLO_DEBUG | XLO_CONFIG | XLO_STATE | XLO_REFRESH);

    // Run the test
    StartTest(hLog);
    EndTest();

    // Close the xLog object
    xCloseLog(hLog);

    // Launch the dashboard
    XLaunchNewImage(NULL, NULL);

    __asm int 3;
}
