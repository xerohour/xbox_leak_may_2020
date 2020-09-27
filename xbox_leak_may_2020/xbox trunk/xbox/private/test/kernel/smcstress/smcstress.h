/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    smcstress.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace SMCStress
    {
    extern DWORD stressMode;

    //
    //  SMC Tests
    //
    extern DWORD HalReadSMBusValue_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD HalWriteSMBusValue_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD Message_Test(HANDLE hLog, DWORD ThreadID);
    } // namespace SMCStress