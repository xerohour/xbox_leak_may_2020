/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    sections.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace XESECTIONS
    {
    //
    //  Loader Tests
    //
    extern DWORD XLoadSection_Test(HANDLE hLog, DWORD ThreadID);
    } // namespace XESECTIONS