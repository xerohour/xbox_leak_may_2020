/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashBoot.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-18-2000  Created

*/
#ifndef _XDASHBOOT_H_
#define _XDASHBOOT_H_

#define COMPONENT_NAME_W    L"XDashBoot"
#define COMPONENT_NAME_A    "XDashBoot"
#define DLL_NAME_W          L"XDashBoot"
#define DLL_NAME_A          "XDashBoot"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHBOOT_H_
