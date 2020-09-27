/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  syncsrv.c

Abstract:

  This module runs a Netsync server

Author:

  Steven Kehrli (steveke) 3-Oct-2000

------------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <netsync.h>



VOID
DisplayUsageInfo(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Displays the usage info in stdout

Return Value:

  None

------------------------------------------------------------------------------*/
{
    wprintf(L"Starts a Netsync server.\n");
    wprintf(L"\n");
    wprintf(L"SYNCSRV\n");
    wprintf(L"\n");
}



int _cdecl
wmain(
    INT    argc,
    WCHAR  *argvW[]
)
{
    // cInput is the character input from the console
    int    cInput;
    // dwLastError is the last error code
    DWORD  dwLastError = ERROR_SUCCESS;

    if (1 < argc) {
        DisplayUsageInfo();
        goto ExitMain;
    }

    wprintf(L"Starting the Netsync server...\n");

    if (FALSE == NetsyncCreateServer()) {
        dwLastError = GetLastError();
        wprintf(L"Start failed: 0x%08x\n", dwLastError);
        goto ExitMain;
    }

    wprintf(L"Started.\n\n");
    
    wprintf(L"Press 'X' to stop.\n\n");

    do {
        cInput = _getch();
    } while (('x' != cInput) && ('X' != cInput));

    wprintf(L"Stopping the Netsync server...\n");

    NetsyncCloseServer();

    wprintf(L"Stopped.\n");

ExitMain:
    return dwLastError;
}
