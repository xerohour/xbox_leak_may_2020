/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    usbInput.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-20-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace USBInput
    {
    //
    //  USB XInput Tests
    //
    extern DWORD DumpDevice_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XInputOpenClose_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XInputGetState_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XInputGetCapabilities_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD XInputSetState_Test(HANDLE hLog, DWORD ThreadID);
    } // namespace USBInput