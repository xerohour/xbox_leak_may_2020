/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ditestAPI.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    05-30-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

/*
    DirectInput API Tests
*/
extern DWORD Acquire_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD CreateDevice_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD DirectInputCreate_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD EnumDevices_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD GetCapabilities_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD GetDeviceStatus_Test(HANDLE hLog, DWORD ThreadID);
extern DWORD DeviceDump_Test(HANDLE hLog, DWORD ThreadID);
