/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XDashMemory.h

Abstract:



Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-17-2000  Created

*/
#ifndef _XDASHMEMORY_H_
#define _XDASHMEMORY_H_

#define COMPONENT_NAME_W    L"XDashMemory"
#define COMPONENT_NAME_A    "XDashMemory"
#define DLL_NAME_W          L"XDashMemory"
#define DLL_NAME_A          "XDashMemory"

typedef DWORD ( *APITEST_FUNC )( HANDLE hLog, CStepsLog* pStepsLog );

#endif // _XDASHMEMORY_H_