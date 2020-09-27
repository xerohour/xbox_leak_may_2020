/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  client.c

Abstract:

  Entry point for xboxkeys

Author:

  Steven Kehrli (steveke) 9-Jul-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace XboxKeysNamespace;

namespace XboxKeysNamespace {

VOID
XboxKeysMain()
{
    // LaunchData is the title launch data
    LAUNCH_DATA  LaunchData;
    // dwDataType is the type of title launch data
    DWORD        dwDataType;
    // hLog is a handle to the xLog log file
    HANDLE       hLog;



    // Get the launch info
    XGetLaunchInfo(&dwDataType, &LaunchData);

    // Create the xLog log file
    hLog = xCreateLog(L"T:\\xboxkeys.log", NULL, INVALID_SOCKET, XLL_LOGDEFAULT, XLO_DEBUG | XLO_CONFIG | XLO_STATE | (LDT_TITLE == dwDataType ? 0 : XLO_REFRESH));

    // Test keys
    XboxKeysTest(hLog, (LDT_TITLE == dwDataType), (PXBOXKEYS) &LaunchData);

    // Test keys over network
    XboxKeysNetworkTest(hLog, (LDT_TITLE == dwDataType));

    xCloseLog(hLog);

    if (LDT_TITLE == dwDataType) {
        XLaunchNewImage(NULL, NULL);
    }
    else {
        XLaunchNewImage("D:\\xboxkeys2.xbe", &LaunchData);
    }
}

} // namespace XboxKeysNamespace
