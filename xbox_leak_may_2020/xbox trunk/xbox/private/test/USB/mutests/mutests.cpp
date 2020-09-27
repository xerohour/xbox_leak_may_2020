/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mutests.cpp

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
#include <xlog.h>
#include "mutests.h"
#include "commontest.h"
#include "utils.h"
#include "usbmanager.h"


//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

static ULONG ThreadID = 0;

WCHAR MUTests::BASE_DRIVE[5];

//
// Heap handle from HeapCreate
//
HANDLE MuTestHeapHandle;


BOOL WINAPI MuTestDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        MuTestHeapHandle = HeapCreate(0, 0, 0);

        if(!MuTestHeapHandle)
            {
            OutputDebugString(L"MUTESTS: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(MuTestHeapHandle)
            {
            HeapDestroy(MuTestHeapHandle);
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
namespace MUTests {
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
        BUILTAPISTRUCT(MUTests::CopyFile_Test),
        BUILTAPISTRUCT(MUTests::CopyFileEx_Test),
        BUILTAPISTRUCT(MUTests::CreateDirectory_Test),
        BUILTAPISTRUCT(MUTests::CreateFile_Test),
        BUILTAPISTRUCT(MUTests::DeleteFile_Test),
        BUILTAPISTRUCT(MUTests::FindFile_Test),
        BUILTAPISTRUCT(MUTests::FlushFileBuffers_Test),
        BUILTAPISTRUCT(MUTests::GetFileInformationByHandle_Test),
        BUILTAPISTRUCT(MUTests::GetFileSize_Test),
        BUILTAPISTRUCT(MUTests::GetSetFileAttributes_Test),
        BUILTAPISTRUCT(MUTests::GetSetFileTime_Test),
        BUILTAPISTRUCT(MUTests::GetVolumeInformation_Test),
        BUILTAPISTRUCT(MUTests::MoveFile_Test),
        BUILTAPISTRUCT(MUTests::MoveFileWithProgress_Test),
        BUILTAPISTRUCT(MUTests::MUCommand_Test),
        BUILTAPISTRUCT(MUTests::ReadWriteFile_Test),
        BUILTAPISTRUCT(MUTests::ReadWriteFileEx_Test),
        BUILTAPISTRUCT(MUTests::ReadWriteFileGatherScatter_Test),
        BUILTAPISTRUCT(MUTests::RemoveDirectory_Test),
        BUILTAPISTRUCT(MUTests::SetEndOfFile_Test),
        BUILTAPISTRUCT(MUTests::SetFilePointerEx_Test),
        BUILTAPISTRUCT(MUTests::HardDriveFull_Test),
        /*
        */
        };

    bool runAll = _wcsicmp(testname, L"(all)") == 0 ? true : false;
    USBManager *usbmanager = new USBManager;
    if(!usbmanager) return 0;


    // Run the suite of tests
    for(DWORD i=0; i<loops; i++)
        {
        // run each test
        for(unsigned j=0; j<ARRAYSIZE(testlist); j++)
            {
            if(runAll || wcistrstr(testlist[j].name, testname))
                {
                xLog(hLog, XLL_INFO, "%ws() ******************************************", testlist[j].name);
                usbmanager->CheckForHotplugs();
                LogResourceStatus(hLog, false);
                DebugPrint("MUTESTS(%d): Entering %ws()\n", threadNumber, testlist[j].name);
                testlist[j].f(hLog, threadNumber);
                }
            }
        }

    delete usbmanager;
    return 0;
    }
} // namespace MUTests



/*

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    hLog - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI MuTestStartTest(HANDLE hLog)
    {
    DWORD ID = 0;

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample]. Everytime
    // the harness calls StartTest, ThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    //

    ID = InterlockedIncrement((LPLONG) &ThreadID);


    // test settings
    const int buffLen = 1024;
    WCHAR testname[buffLen];
    DWORD loops;



    // Pull out internal INI file settings
    GetProfileString(L"mutests", L"testname", L"(all)", testname, buffLen);
    GetProfileString(L"mutests", L"drive", L"F", MUTests::BASE_DRIVE, 5);
    loops = GetProfileInt(L"mutests", L"loops", 1);

    DebugPrint("  drive=%s\n", MUTests::BASE_DRIVE);

    MUTests::BASE_DRIVE[0] = MUTests::BASE_DRIVE[0] + (unsigned short)ID - 1;
    MUTests::BASE_DRIVE[1] += L'\0';

    xSetOwnerAlias(hLog, "jpoley");
    xSetComponent(hLog, "Device Driver", "MU");
    DebugPrint("MUTESTS(%d): **************************************************************\n", ID);
    DebugPrint("MUTESTS(%d): Entering StartTest()\n", ID);
    DebugPrint("MUTESTS(%d): Internal INI Settings:\n", ID);
    DebugPrint("  loops=%lu\n", loops);
    DebugPrint("  testname=%ws\n", testname);
    DebugPrint("  drive=%ws\n", MUTests::BASE_DRIVE);
    DebugPrint("MUTESTS(%d): **************************************************************\n", ID);


    switch(ID)
        {
        case 0:
        case 1:
        default:
            MUTests::RunAll(hLog, ID, loops, testname);
            break;
        }

    DebugPrint("MUTESTS(%d): **************************************************************\n", ID);
    DebugPrint("MUTESTS(%d): Leaving StartTest()\n", ID);
    DebugPrint("MUTESTS(%d): **************************************************************\n", ID);
    xSetOwnerAlias(hLog, NULL);
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
VOID WINAPI MuTestEndTest(void)
    {
    ThreadID = 0;
    }


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mutests )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mutests )
    EXPORT_TABLE_ENTRY( "StartTest", MuTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MuTestEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", MuTestDllMain )
END_EXPORT_TABLE( mutests )
