/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashGames.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-16-2000  Created

*/
#ifndef _XDASHGAMES_H_
#define _XDASHGAMES_H_

#define COMPONENT_NAME_W    L"XDashGames"
#define COMPONENT_NAME_A    "XDashGames"
#define DLL_NAME_W          L"XDashGames"
#define DLL_NAME_A          "XDashGames"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHGAMES_H_