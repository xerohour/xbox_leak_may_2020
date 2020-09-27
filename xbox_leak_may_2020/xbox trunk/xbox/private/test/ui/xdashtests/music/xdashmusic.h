/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashMusic.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHMUSIC_H_
#define _XDASHMUSIC_H_

#define COMPONENT_NAME_W    L"XDashMusic"
#define COMPONENT_NAME_A    "XDashMusic"
#define DLL_NAME_W          L"XDashMusic"
#define DLL_NAME_A          "XDashMusic"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHMUSIC_H_