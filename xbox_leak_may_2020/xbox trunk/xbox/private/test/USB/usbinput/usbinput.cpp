/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    usbInput.cpp

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
#include <xbox.h>
#include <xtestlib.h>
#include <xlog.h>
#include "commontest.h"

#include "usbInput.h"

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

ULONG UsbInputThreadID = 0;


//
// Heap handle from HeapCreate
//
HANDLE UsbInputHeapHandle;


BOOL WINAPI UsbInputDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        UsbInputHeapHandle = HeapCreate(0, 0, 0);

        if(!UsbInputHeapHandle)
            {
            OutputDebugString(L"USBTEST: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(UsbInputHeapHandle)
            {
            HeapDestroy(UsbInputHeapHandle);
            }
        }

    return TRUE;
    }

static bool wcistrstr(WCHAR *string, WCHAR *substr, size_t len=0)
    {
    if(!string || ! substr) return false;
    if(!*string) return false;
    if(!len) len = wcslen(substr);
    if(_wcsnicmp(string, substr, len) == 0) return true;

    return wcistrstr(string+1, substr, len);
    }

/*

Routine Description:


Arguments:

    HANDLE hLog - to the logging object
    DWORD threadNumber - sequential number of the thread
    DWORD loops - # of times to run test
    WCHAR* testname - specific name of a test to run

Return Value:

    DWORD

*/
namespace USBInput {
static DWORD RunAll(HANDLE hLog, DWORD threadNumber, DWORD loops, WCHAR *testname)
    {
    Sleep(threadNumber*200); // stagger the thread's execution a bit

    struct APIFUNCT
        {
        APITEST_FUNC f;
        WCHAR *name;
        };
    #define BUILTAPISTRUCT(n) { n , L#n }

    // Define all the API tests
    APIFUNCT testlist[] =
        {
        /*
        */
        BUILTAPISTRUCT(USBInput::XInputOpenClose_Test),
        BUILTAPISTRUCT(USBInput::XInputGetState_Test),
        BUILTAPISTRUCT(USBInput::XInputGetCapabilities_Test),
        BUILTAPISTRUCT(USBInput::XInputSetState_Test),
        };

    bool runAll = _wcsicmp(testname, L"(all)") == 0 ? true : false;

    // Run the suite of tests
    for(DWORD i=0; i<loops; i++)
        {
        // run each test
        for(unsigned j=0; j<ARRAYSIZE(testlist); j++)
            {
            if(runAll || wcistrstr(testlist[j].name, testname))
                {
                xLog(hLog, XLL_INFO, "%ws() ******************************************", testlist[j].name);
                LogResourceStatus(hLog, false);
                DebugPrint("USBINPUT(%d): Entering %ws()\n", threadNumber, testlist[j].name);
                testlist[j].f(hLog, threadNumber);
                }
            }
        }
    LogResourceStatus(hLog, true);

    return 0;
    }
} // namespace USBInput



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
VOID WINAPI UsbInputStartTest(HANDLE hLog)
    {
    ULONG ID;

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample]. Everytime
    // the harness calls StartTest, UsbInputThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    //

    ID = InterlockedIncrement((LPLONG) &UsbInputThreadID);

    // test settings
    const int buffLen = 1024;
    WCHAR *testname = new WCHAR[buffLen];
    DWORD loops;


    // Pull out internal INI file settings
    GetProfileString(L"usbInput", L"testname", L"(all)", testname, buffLen);
    loops = GetProfileInt(L"usbInput", L"loops", 1);


    xSetOwnerAlias(hLog, "jpoley");
    xSetComponent(hLog, "USB Driver", "XInput");
    DebugPrint("USBINPUT(%d): **************************************************************\n", ID);
    DebugPrint("USBINPUT(%d): Entering StartTest()\n", ID);
    DebugPrint("USBINPUT(%d): Internal INI Settings:\n", ID);
    DebugPrint("  loops=%lu\n", loops);
    DebugPrint("  testname=%ws\n", testname);
    DebugPrint("\nUSBINPUT(%d): !!!      A USB Controller must be plugged into port 0      !!!\n", UsbInputThreadID);
    DebugPrint("             !!!               for these tests to pass                  !!!\n\n");

    switch(ID)
        {
        case 0:
        case 1:
        default:
            USBInput::RunAll(hLog, ID, loops, testname);
            break;
        }


    DebugPrint("USBINPUT(%d): Leaving StartTest()\n", ID);
    DebugPrint("USBINPUT(%d): **************************************************************\n", ID);
    USBInput::LogResourceStatus(NULL, true);
    xSetOwnerAlias(hLog, NULL);

    delete[] testname;
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
VOID WINAPI UsbInputEndTest(void)
    {
    UsbInputThreadID = 0;
    }


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( usbinput )
#pragma data_seg()

BEGIN_EXPORT_TABLE( usbinput )
    EXPORT_TABLE_ENTRY( "StartTest", UsbInputStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", UsbInputEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", UsbInputDllMain )
END_EXPORT_TABLE( usbinput )
