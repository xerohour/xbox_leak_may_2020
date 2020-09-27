/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    fileio.cpp

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
#include <xlog.h>
#include "fileio_win32API.h"
#include "commontest.h"
#include "utils.h"
#include "xtestlib.h"

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

static ULONG ThreadID = 0;


//
// Heap handle from HeapCreate
//
HANDLE FileIOHeapHandle;


BOOL WINAPI FileIODllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        FileIOHeapHandle = HeapCreate(0, 0, 0);

        if(!FileIOHeapHandle)
            {
            OutputDebugString(L"FILEIO: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(FileIOHeapHandle)
            {
            HeapDestroy(FileIOHeapHandle);
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
namespace FILEIO {

DWORD stressMode = 0;


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
        BUILTAPISTRUCT(FILEIO::CopyFile_Test),
        BUILTAPISTRUCT(FILEIO::CopyFileEx_Test),
        BUILTAPISTRUCT(FILEIO::CreateDirectory_Test),
        BUILTAPISTRUCT(FILEIO::DeleteFile_Test),
        BUILTAPISTRUCT(FILEIO::FindFile_Test),
        BUILTAPISTRUCT(FILEIO::FlushFileBuffers_Test),
        BUILTAPISTRUCT(FILEIO::GetDiskFreeSpaceEx_Test),
        BUILTAPISTRUCT(FILEIO::GetFileInformationByHandle_Test),
        BUILTAPISTRUCT(FILEIO::GetFileSize_Test),
        BUILTAPISTRUCT(FILEIO::GetSetFileAttributes_Test),
        BUILTAPISTRUCT(FILEIO::GetSetFileTime_Test),
        BUILTAPISTRUCT(FILEIO::GetVolumeInformation_Test),
        BUILTAPISTRUCT(FILEIO::MoveFile_Test),
        BUILTAPISTRUCT(FILEIO::MoveFileWithProgress_Test),
        BUILTAPISTRUCT(FILEIO::ReadWriteFile_Test),
        BUILTAPISTRUCT(FILEIO::ReadWriteFileEx_Test),
        BUILTAPISTRUCT(FILEIO::ReadWriteFileGatherScatter_Test),
        BUILTAPISTRUCT(FILEIO::RemoveDirectory_Test),
        BUILTAPISTRUCT(FILEIO::SetEndOfFile_Test),
        BUILTAPISTRUCT(FILEIO::SetFilePointer_Test),
        BUILTAPISTRUCT(FILEIO::SetFilePointerEx_Test),
        BUILTAPISTRUCT(FILEIO::DVDRead_Test),
        BUILTAPISTRUCT(FILEIO::HardDriveFull_Test),
        BUILTAPISTRUCT(FILEIO::CreateFile_Test),        // CreateFile put last because we want the stress test run last
        /*
        */
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
                DebugPrint("FILEIO(%d): Entering %ws()\n", threadNumber, testlist[j].name);
                testlist[j].f(hLog, threadNumber);
                }
            }
        }

    return 0;
    }
    } // namespace FILEIO


/*
DWORD Test(void)
    {
    MEMORYSTATUS stats;
    memset(&stats, 0, sizeof(MEMORYSTATUS));
    stats.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&stats);

    return 0;
    }
*/
    /*
DWORD Test(void)
    {

    DebugPrint("LeakCheck: HandleCount=   %lu\n (ec: %lu)", handles, err);

    HRESULT hr = S_OK;
    DWORD status;
    char m_pvBuffer[2048];

    //while(hr == S_OK)
        //{
        if(m_pvBuffer)
            {
            status = NtQuerySystemInformation(SystemProcessInformation, m_pvBuffer, 2048, NULL);

            // If we succeeded, great, get outta here.  If not, any error other
            // than "buffer too small" is fatal, in which case we bail
            if(NT_SUCCESS(status))
                {
                SYSTEM_PROCESS_INFORMATION *stats = (SYSTEM_PROCESS_INFORMATION*)m_pvBuffer;
                DebugPrint("LeakCheck: Threads=       %lu\n", stats->NumberOfThreads);
                DebugPrint("LeakCheck: WorkingSetSize=%lu\n", stats->WorkingSetSize);
                //break;
                }
            if(status != STATUS_INFO_LENGTH_MISMATCH)
                {
                DebugPrint("LeakCheck: ERROR from NtQuerySystemInformation %lX\n", status);
                hr = E_FAIL;
                //break;
                }
            }
/
        // Buffer wasn't large enough to hold the process info table, so resize it
        // to be larger, then retry.
        if(m_pvBuffer)
            {
            VirtualFree(m_pvBuffer, 0, MEM_RELEASE);
            m_pvBuffer = NULL;
            }
        m_cbBuffer += PROCBUF_GROWSIZE;
        m_pvBuffer = VirtualAlloc(NULL, m_cbBuffer, MEM_COMMIT, PAGE_READWRITE);
        if(m_pvBuffer == NULL)
            {
            hr = E_OUTOFMEMORY;
            DebugPrint("LeakCheck: ERROR E_OUTOFMEMORY\n");
            break;
            }*
        //}

    return hr;
    return 0;
    }
*/
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
VOID WINAPI FileIOStartTest(HANDLE hLog)
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
    GetProfileString(L"fileio", L"testname", L"(all)", testname, buffLen);
    loops = GetProfileInt(L"fileio", L"loops", 1);
    FILEIO::stressMode = GetProfileInt(L"GeneralSettings", L"StressTestMode", 0);


    xSetOwnerAlias(hLog, "jpoley");
    xSetComponent(hLog, "Win32 API", "FILE IO");
    DebugPrint("FILEIO(%d): **************************************************************\n", ID);
    DebugPrint("FILEIO(%d): Entering StartTest()\n", ID);
    DebugPrint("FILEIO(%d): Internal INI Settings:\n", ID);
    DebugPrint("  loops=%lu\n", loops);
    DebugPrint("  testname=%ws\n", testname);


    switch(ID)
        {
        case 0:
        case 1:
        default:
            FILEIO::RunAll(hLog, ID, loops, testname);
            break;
        }


    DebugPrint("FILEIO(%d): Leaving StartTest()\n", ID);
    DebugPrint("FILEIO(%d): **************************************************************\n", ID);
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
VOID WINAPI FileIOEndTest(void)
    {
    ThreadID = 0;
    }


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( fileio )
#pragma data_seg()

BEGIN_EXPORT_TABLE( fileio )
    EXPORT_TABLE_ENTRY( "StartTest", FileIOStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", FileIOEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", FileIODllMain )
END_EXPORT_TABLE( fileio )
