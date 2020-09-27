/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    launcher.cpp

Abstract:

    The DLL exports two of the mandatory APIs: StartTest and EndTest.
    TESTINI.INI must be modified correctly so that the
    harness will load and call StartTest/EndTest.

    Please see "x-box test harness.doc" for more information.

Author:

    jpoley

Environment:

    XBox

Revision History:

*/


#include "stdafx.h"
#include <xtestlib.h>
//#include <xbox.h>
//#include <xlog.h>

#include "CSockServer.h"
#include "launcherx.h"


//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

ULONG LauncherThreadID = 0;


//
// Heap handle from HeapCreate
//
HANDLE LauncherHeapHandle;

BOOL WINAPI LauncherDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        LauncherHeapHandle = HeapCreate(0, 0, 0);

        if(!LauncherHeapHandle)
            {
            OutputDebugString(L"LAUNCHER: Unable to create heap\n");
            }
        else
            {
            DebugPrint("LAUNCHER: DLL_PROCESS_ATTACH %X\n", LauncherHeapHandle);
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(LauncherHeapHandle)
            {
            HeapDestroy(LauncherHeapHandle);
            }

        penterDumpStats();
        }

    return TRUE;
    }



/*

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI LauncherStartTest(HANDLE hLog)
    {
    ULONG ID;

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample]. Everytime
    // the harness calls StartTest, LauncherThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    //

    ID = InterlockedIncrement((LPLONG) &LauncherThreadID);

    // ini file settings
    //const int buffLen = 1024;
    unsigned short port;
    unsigned timeout;
    //WCHAR testname[buffLen];


    // Pull out internal INI file settings
    //GetProfileString(L"launcher", L"testname", L"(all)", testname, buffLen);
    port = (unsigned short)GetProfileInt(L"launcher", L"port", 99);
    timeout = GetProfileInt(L"launcher", L"timeout", 600);

    DebugPrint("LAUNCHER(%d): **************************************************************\n", ID);
    DebugPrint("LAUNCHER(%d): Internal INI Settings:\n", ID);
    DebugPrint("  port=%u\n", port);
    DebugPrint("  timeout=%u\n", timeout);
    //DebugPrint("  testname=%ws\n", testname);

    Launcher::CSockServer *server = new Launcher::CSockServer;

    if(!server)
        {
        DebugPrint("LAUNCHER: Out of memory!\n");
        return;
        }

    server->SetPort(port);
    server->SetTimeout(timeout);
    server->m_socktype = SOCK_STREAM;
    server->OpenServerSocket(server->m_socktype);
    server->BindSocket();
    server->ThreadFunct();

    delete server;

    DebugPrint("LAUNCHER(%d): **************************************************************\n", ID);
    }


/*

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI LauncherEndTest(void)
    {
    LauncherThreadID = 0;
    }


/*

Routine Description:

    Formatted print function that sends the output through a connected SOCKET.

Arguments:

    SOCKET sock - connected socket
    char* format - printf style format specifier
    ... - arguments specified by the format specifier

Return Value:

    int - number of bytes sent out

Notes:

*/
int SockPrint(SOCKET sock, char* format, ...)
    {
    if(sock == INVALID_SOCKET) return 0;

    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    int len = vsprintf(szBuffer, format, args);
    send(sock, szBuffer, len, 0);

    va_end(args);

    return len;
    }


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( launcher )
#pragma data_seg()

BEGIN_EXPORT_TABLE( launcher )
    EXPORT_TABLE_ENTRY( "StartTest", LauncherStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", LauncherEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", LauncherDllMain )
END_EXPORT_TABLE( launcher )
