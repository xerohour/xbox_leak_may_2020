/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashSettings.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHSETTINGS_H_
#define _XDASHSETTINGS_H_

#define COMPONENT_NAME_W    L"XDashSettings"
#define COMPONENT_NAME_A    "XDashSettings"
#define DLL_NAME_W          L"XDashSettings"
#define DLL_NAME_A          "XDashSettings"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHSETTINGS_H_