/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.c

Abstract:

  This modules tests SG Data Validation - executable

Author:

  Steven Kehrli (steveke) 5-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace SGDataNamespace;

void __cdecl main()
{
    // hLog is the handle to the xLog object
    HANDLE  hLog = INVALID_HANDLE_VALUE;



    // Initialize the devices
    XInitDevices(0, NULL);

    // Create the xLog object
    hLog = xCreateLog(L"T:\\sgdata.log", NULL, INVALID_SOCKET, XLL_LOGDEFAULT, XLO_DEBUG | XLO_CONFIG | XLO_STATE | XLO_REFRESH);

    // Run the test
    StartTest(hLog);
    EndTest();

    // Close the xLog object
    xCloseLog(hLog);

    // Launch the dashboard
    XLaunchNewImage(NULL, NULL);

    __asm int 3;
}
