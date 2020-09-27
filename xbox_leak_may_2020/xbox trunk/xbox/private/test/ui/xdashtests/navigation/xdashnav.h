/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashNav.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHNAV_H_
#define _XDASHNAV_H_

#define COMPONENT_NAME_W    L"XDashNav"
#define COMPONENT_NAME_A    "XDashNav"
#define DLL_NAME_W          L"XDashNav"
#define DLL_NAME_A          "XDashNav"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHNAV_H_