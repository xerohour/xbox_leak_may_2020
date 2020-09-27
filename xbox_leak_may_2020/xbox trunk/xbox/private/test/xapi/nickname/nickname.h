/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    Nickname.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace Nickname
    {
    //
    //  XAPI Nickname Tests
    //
    extern DWORD XSetNickname_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XFindNickname_Test(HANDLE hLog, DWORD ThreadID);
    } // namespace Nickname