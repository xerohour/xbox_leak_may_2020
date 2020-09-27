/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashXZone.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHXZONE_H_
#define _XDASHXZONE_H_

#define COMPONENT_NAME_W    L"XDashXZone"
#define COMPONENT_NAME_A    "XDashXZone"
#define DLL_NAME_W          L"XDashXZone"
#define DLL_NAME_A          "XDashXZone"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHXZONE_H_