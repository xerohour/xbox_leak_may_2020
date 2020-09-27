/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    stressmon.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

VOID WINAPI StressMONStartTest(HANDLE hLog);

namespace StressMON
    {
    extern DWORD stressMode;
    } // namespace StressMON