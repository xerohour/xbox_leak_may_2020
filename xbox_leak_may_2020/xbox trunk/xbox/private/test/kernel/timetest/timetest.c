/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    timetest.c

Abstract:

    test XBox time related APIs

Author:

    John Daly (johndaly) 10-May-2000

Environment:

    X-Box

Notes:

timetest - test these API's
        CompareFileTime
        FileTimeToLocalFileTime
        FileTimeToSystemTime
        GetLocalTime
        GetSystemTime
        GetSystemTimeAsFileTime
        GetTickCount
        GetTimeZoneInformation
        LocalFileTimeToFileTime
        Sleep
        SleepEx

Revision History:

    10-May-2000     johndaly
    created

Current Status:

    on hold until they sort out the timezone issue, the floor
    will fall out on me if they yank some of the OS stuff, 
    already they may pull timezone support, that will make re-writing
    this stuff a pain

--*/

//
// includes
//

#include "timetest.h"

VOID
WINAPI
test_FileTimeToLocalFileTime(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test the FileTimeToLocalFileTime() API
    test the LocalFileTimeToFileTime() API
    test the SystemTimeToFileTime() API
    test the FileTimeToSystemTime() API
    
Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

Notes:

    This is basically a self-referntial integrity test mixed 
    with an alternate calculation of the same value.  if 
    everything comes out ok, PASS
    PS this code sucks, but it sucked worse trying to debug it when I had macros doing all the work
    
--*/
{
    TIME_ZONE_INFORMATION SaveTimeZoneInfo = {0};   // current system timezone
    SYSTEMTIME SaveCSystemTime = {0};   // current system time
    
    TIME_ZONE_INFORMATION TimeZoneInfo = {0};
    SYSTEMTIME TestSystemTime[3] = {0};   // current system time
    
    SYSTEMTIME xTestSystemTime1 = {0};   // test system time
    SYSTEMTIME xTestSystemTime2 = {0};   // test system time
    FILETIME TestFileTime = {0};   // test file time
    FILETIME TestFileTime1 = {0};   // test file time

    ULARGE_INTEGER tmpULFileTime = {0};
    ULARGE_INTEGER ULFileTime = {0};
        
    FILETIME tmpFileTime = {0};
    FILETIME LocalFileTime = {0};

    int x = 0;
    int y = 0;

    //
    // positive test cases (probably overkill... the function is simple)
    // code review shows this function only returns TRUE, and does 
    // no parameter validation
    //

    xSetFunctionName( LogHandle, "FileTimeToLocalFileTime" );
    xStartVariation( LogHandle, "goodparam1" );
    
    //
    // a simple test (this code will not change much or at all from NT)
    // for last year, this year, 10 years from now
    // at UTC 00:00.0000000, 00:00.0000001, 12:00.0000000, 23:59.9999999
    // rotate through all the time zones, and see
    // if we got a result that makes sense
    //
    
    //
    // save
    //

    GetTimeZoneInformation(&SaveTimeZoneInfo);
    GetSystemTime(&SaveCSystemTime);
    
    //
    // I need to make this adjustment?
    //
    //
    //if (bUsingDST) {
    //    SaveCSystemTime.DaylightBias += 60;
    //}
    
    // current
    GetSystemTime(&TestSystemTime[0]);
    
    // 1 year ago
    GetSystemTime(&TestSystemTime[1]);
    SystemTimeToFileTime(&TestSystemTime[1], &tmpFileTime);
    ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
    ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
    ULFileTime.QuadPart -= YEAR_TIME;
    tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
    tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
    FileTimeToSystemTime(&tmpFileTime, &TestSystemTime[1]);

    // 10 years from now
    GetSystemTime(&TestSystemTime[2]);
    SystemTimeToFileTime(&TestSystemTime[2], &tmpFileTime);
    ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
    ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
    ULFileTime.QuadPart += 10*YEAR_TIME;
    tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
    tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
    FileTimeToSystemTime(&tmpFileTime, &TestSystemTime[2]);
    
    //
    // Current time, set the bias to 0, we play with the bias during the test loop
    //

    GetTimeZoneInformation(&TimeZoneInfo);
    TimeZoneInfo.Bias = 0;
    XapipSetTimeZoneInformation(&TimeZoneInfo);  // put us in UTC - bias is now gone
    
    //
    // make system times tstX (can be fake)
    //  - set UTC time
    //  - set time zone
    //  - set case - offsets from 00:00.0000000
    // 
    // SystemTimeToFileTime( IN xTestSystemTime1, OUT TestFileTime) // UTC
    // FileTimeToSystemTime(IN TestFileTime, OUT xTestSystemTime2 ) // UTC
    // assert (xTestSystemTime == xTestSystemTime2)
    //
    // SystemTimeToFileTime(&xTestSystemTime1, &TestFileTime); 
    // FileTimeToSystemTime(&TestFileTime, &xTestSystemTime2); 
    // assert (cmpLocalFileTime == LocalFileTime)
    //
    // y[0] = current, y[1] = 1 year ago, y[2] = 10 years from now
    //

    for (y = 0; y < 3; y++) {
        while(x < 24) {
    
            //
            // set system time / time zone
            //
    
            TimeZoneInfo.Bias += (x * 60);
            XapipSetTimeZoneInformation(&TimeZoneInfo);
    
            //
            // convert various times / check them
            //
    
            memcpy(&xTestSystemTime1, &TestSystemTime[y], sizeof(SYSTEMTIME));
            
            //
            // at UTC 00:00.0000000
            //
    
            SystemTimeToFileTime(&xTestSystemTime1, &TestFileTime);
            FileTimeToSystemTime(&TestFileTime, &xTestSystemTime2);
            xLog(LogHandle,
                 (0 == memcmp(&xTestSystemTime1, &xTestSystemTime2, sizeof(SYSTEMTIME))) ? 
                 XLL_PASS : XLL_FAIL, "sys - UTC 00:00.0000000");

            FileTimeToLocalFileTime(&TestFileTime, &LocalFileTime);
            LocalFileTimeToFileTime(&LocalFileTime, &TestFileTime1);
            xLog(LogHandle,
                 (0 == memcmp(&TestFileTime, &TestFileTime1, sizeof(FILETIME))) ? 
                 XLL_PASS : XLL_FAIL, "file - UTC 00:00.0000000");
            
            //
            // at UTC 00:00.0000001
            //

            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart += TIME_0001;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);
            
            SystemTimeToFileTime(&xTestSystemTime1, &TestFileTime);
            FileTimeToSystemTime(&TestFileTime, &xTestSystemTime2);
            xLog(LogHandle,
                 (0 == memcmp(&xTestSystemTime1, &xTestSystemTime2, sizeof(SYSTEMTIME))) ? 
                 XLL_PASS : XLL_FAIL, "sys - UTC 00:00.0000001");
            
            FileTimeToLocalFileTime(&TestFileTime, &LocalFileTime);
            LocalFileTimeToFileTime(&LocalFileTime, &TestFileTime1);
            xLog(LogHandle,
                 (0 == memcmp(&TestFileTime, &TestFileTime1, sizeof(FILETIME))) ? 
                 XLL_PASS : XLL_FAIL, "file - UTC 00:00.0000001");
            
            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart -= TIME_0001;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);
    
            //
            // at UTC 12:00.0000000
            //

            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart += TIME_1200;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);
            
            SystemTimeToFileTime(&xTestSystemTime1, &TestFileTime);
            FileTimeToSystemTime(&TestFileTime, &xTestSystemTime2);
            xLog(LogHandle,
                 (0 == memcmp(&xTestSystemTime1, &xTestSystemTime2, sizeof(SYSTEMTIME))) ? 
                 XLL_PASS : XLL_FAIL, "sys - UTC 12:00.0000000");
            
            FileTimeToLocalFileTime(&TestFileTime, &LocalFileTime);
            LocalFileTimeToFileTime(&LocalFileTime, &TestFileTime1);
            xLog(LogHandle,
                 (0 == memcmp(&TestFileTime, &TestFileTime1, sizeof(FILETIME))) ? 
                 XLL_PASS : XLL_FAIL, "file - UTC 12:00.0000000");
            
            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart -= TIME_1200;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);
    
            //
            // at UTC 23:59.9999999
            //
    
            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart += TIME_2359;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);
            
            SystemTimeToFileTime(&xTestSystemTime1, &TestFileTime);
            FileTimeToSystemTime(&TestFileTime, &xTestSystemTime2);
            xLog(LogHandle,
                 (0 == memcmp(&xTestSystemTime1, &xTestSystemTime2, sizeof(SYSTEMTIME))) ? 
                 XLL_PASS : XLL_FAIL, "sys - UTC 23:59.9999999");
            
            FileTimeToLocalFileTime(&TestFileTime, &LocalFileTime);
            LocalFileTimeToFileTime(&LocalFileTime, &TestFileTime1);
            xLog(LogHandle,
                 (0 == memcmp(&TestFileTime, &TestFileTime1, sizeof(FILETIME))) ? 
                 XLL_PASS : XLL_FAIL, "file - UTC 23:59.9999999");
            
            SystemTimeToFileTime(&xTestSystemTime1, &tmpFileTime);
            ULFileTime.LowPart = tmpFileTime.dwLowDateTime;
            ULFileTime.HighPart = tmpFileTime.dwHighDateTime;
            ULFileTime.QuadPart -= TIME_2359;
            tmpFileTime.dwLowDateTime = ULFileTime.LowPart;
            tmpFileTime.dwHighDateTime = ULFileTime.HighPart;
            FileTimeToSystemTime(&tmpFileTime, &xTestSystemTime1);

            x++;
        }

        //
        // restore
        //
    
        XapiSetLocalTime(&SaveCSystemTime);
        XapipSetTimeZoneInformation(&SaveTimeZoneInfo);
        x = 0;
    }
    
    XapipSetTimeZoneInformation(&SaveTimeZoneInfo);
    
    xEndVariation( LogHandle );

    return;
}

VOID
WINAPI
test_GetTickCount(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test the GetTickCount() API

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

--*/
{
    //
    // positive test cases
    // just use a very simple test
    //
    
    DWORD dwTickCount1;
    DWORD dwTickCount2;
    DWORD dwTickCount;

    xSetFunctionName( LogHandle, "GetTickCount" );
    xStartVariation( LogHandle, "goodparam1" );
    
    dwTickCount1 = GetTickCount();
    Sleep(1000);
    dwTickCount2 = GetTickCount();
    dwTickCount = dwTickCount2 - dwTickCount1;

    if ((dwTickCount < 1100) || (dwTickCount > 900)) {
        xLog(LogHandle, XLL_PASS, "GetTickCount within 10%% tolerance");
    } else {
        xLog(LogHandle,
             XLL_FAIL, 
             "GetTickCount outside 10% tolerance, expected 1000ms, got:%dms", 
             dwTickCount);
    }

    xEndVariation( LogHandle );

    return;
}

#define THREADNUM 50

struct RESULTSARRAY {
        SYSTEMTIME start;
        SYSTEMTIME finish;
}ResultsArray[THREADNUM] = {0};

VOID
WINAPI
test_Sleep(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test the Sleep() API

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

--*/
{
    DWORD ThreadID;
    FILETIME FileTimeStart;
    FILETIME FileTimeFinish;
    ULARGE_INTEGER ulFileTimeStart;
    ULARGE_INTEGER ulFileTimeFinish;
    ULONGLONG ulExpectedTime;
    ULONGLONG ulExpectedTimeStart;
    ULONGLONG ulExpectedTimeFinish;
    
    HANDLE threads[50] = {0};
    int x = 0;

    //
    // positive test cases
    //
    
    xSetFunctionName( LogHandle, "Sleep" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // create 50 threads, each sleeps a multiple of 2000 milliseconds, 0 - 100
    // waits for threads to all complete, then checks results
    //

    for ( x = 0; x < THREADNUM ; x++ ) {
        threads[x] = CreateThread(NULL,
                     4096,
                     (LPTHREAD_START_ROUTINE)SleepTestThread,
                     (LPVOID)(x),
                     0,
                     &ThreadID);
    }

    WaitForMultipleObjects(THREADNUM, 
                           &threads[0],
                           TRUE,
                           200 * 1000);

    for ( x = 0; x < THREADNUM ; x++ ) {
        CloseHandle(threads[x]);
    }

    //
    // check results
    // iterate through the times, make sure the waits were within 10% 
    // (is this an appropriate tolerance?) of the expected sleep time
    //

    for ( x = 0; x < THREADNUM ; x++ ) {
        
        SystemTimeToFileTime(&ResultsArray[x].start, &FileTimeStart);
        SystemTimeToFileTime(&ResultsArray[x].finish, &FileTimeFinish);
        ulFileTimeStart.LowPart = FileTimeStart.dwLowDateTime;  
        ulFileTimeStart.HighPart = FileTimeStart.dwHighDateTime;
        ulFileTimeFinish.LowPart = FileTimeFinish.dwLowDateTime;  
        ulFileTimeFinish.HighPart = FileTimeFinish.dwHighDateTime;
        ulFileTimeFinish.QuadPart -= ulFileTimeStart.QuadPart;

        if (0 == x) {
            
            //
            // ok, how long should sleeping 0 take...
            //

            xLog(LogHandle, XLL_PASS, "sleep(0) call took:%I64u ", ulFileTimeFinish.QuadPart);
            continue;

        } else {

            ulExpectedTime = (x * 2) * 10000000;
            ulExpectedTimeFinish = (ulExpectedTime + ulExpectedTime/10);
            ulExpectedTimeStart = (ulExpectedTime - ulExpectedTime/10);

        }

        xLog(LogHandle,
            ((ulFileTimeFinish.QuadPart < ulExpectedTimeFinish) && 
             (ulFileTimeFinish.QuadPart > ulExpectedTimeStart))
            ? XLL_PASS : XLL_FAIL, "thread sequence #:%d", x);

    }
    
    xEndVariation( LogHandle );

}

DWORD
WINAPI
SleepTestThread(
    LPVOID lpParameter
    )
/*++

Routine Description:

    test test thread for the SleepEx() API test

Arguments:

    lpParameter - thread sequence number to access the thread data RESULTSARRAY array

Return Value:

    None

--*/
{
    GetSystemTime(&ResultsArray[(int)lpParameter].start);
    Sleep((DWORD)lpParameter * 2000);
    GetSystemTime(&ResultsArray[(int)lpParameter].finish);
    return(0);
}

typedef struct _SLEEPEXTESTTHREADPARAMS {
    DWORD dwMilliseconds;  // time-out interval
    BOOL bAlertable;
    DWORD threadnum;
    HANDLE IoCompletionPortHandle;
    DWORD dwErrorCode;                // completion code
    DWORD dwNumberOfBytesTransfered;  // number of bytes transferred
    DWORD NumberOfBytes;     
    ULONG CompletionKey;  // file completion key
    BOOL CompletionStatusRetVal;
} SLEEPEXTESTTHREADPARAMS, *LPSLEEPEXTESTTHREADPARAMS;

SLEEPEXTESTTHREADPARAMS ThreadParams[THREADNUM] = {0};

VOID
WINAPI
test_SleepEx(
            HANDLE LogHandle
            )
/*++

Routine Description:

    test the SleepEx() API

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

--*/ 
{

    //
    // positive test cases
    //

    //
    // Scenario #1
    // plain-old sleeping...
    // we can re-write this to call the test_sleep function with changed pointers
    // to cut down on the code, maybe later
    //
    {
        DWORD ThreadID;
        FILETIME FileTimeStart;
        FILETIME FileTimeFinish;
        ULARGE_INTEGER ulFileTimeStart;
        ULARGE_INTEGER ulFileTimeFinish;
        ULONGLONG ulExpectedTime;
        ULONGLONG ulExpectedTimeStart;
        ULONGLONG ulExpectedTimeFinish;
        DWORD ExitCode;

        HANDLE threads[50] = {0};
        int x = 0;

        xSetFunctionName( LogHandle, "SleepEx" );
        xStartVariation( LogHandle, "Variation#1" );

        //
        // create 50 threads, each sleeps a multiple of 2000 milliseconds, 0 - 100
        // waits for threads to all complete, then checks results
        //

        ZeroMemory(&ResultsArray[0], THREADNUM * (sizeof (struct RESULTSARRAY)));

        for ( x = 0; x < THREADNUM ; x++ ) {
            ThreadParams[x].bAlertable = FALSE;
            ThreadParams[x].dwMilliseconds = 2000 * x;
            ThreadParams[x].threadnum = x;

            threads[x] = CreateThread(NULL,
                                      65536,
                                      (LPTHREAD_START_ROUTINE)SleepExTestThread,
                                      (LPVOID)&ThreadParams[x],
                                      0,
                                      &ThreadID);
        }

        WaitForMultipleObjects(THREADNUM, 
                               &threads[0],
                               TRUE,
                               200 * 1000);

        //
        // check exit codes, close handles
        //

        for ( x = 0; x < THREADNUM ; x++ ) {
            GetExitCodeThread(threads[x], &ExitCode);
            if (0 != ExitCode) {
                xLog(LogHandle, 
                     XLL_FAIL, 
                     "Non-0 exit code detected in SleepEx: code:%d Handle:%d", 
                     ExitCode, 
                     threads[x]);
            }
            CloseHandle(threads[x]);
        }

        //
        // check results
        // iterate through the times, make sure the waits were within 10% 
        // (is this an appropriate tolerance?) of the expected sleep time
        //

        for ( x = 0; x < THREADNUM ; x++ ) {

            SystemTimeToFileTime(&ResultsArray[x].start, &FileTimeStart);
            SystemTimeToFileTime(&ResultsArray[x].finish, &FileTimeFinish);
            ulFileTimeStart.LowPart = FileTimeStart.dwLowDateTime;  
            ulFileTimeStart.HighPart = FileTimeStart.dwHighDateTime;
            ulFileTimeFinish.LowPart = FileTimeFinish.dwLowDateTime;  
            ulFileTimeFinish.HighPart = FileTimeFinish.dwHighDateTime;
            ulFileTimeFinish.QuadPart -= ulFileTimeStart.QuadPart;

            if (0 == x) {

                //
                // ok, how long should sleeping 0 take...
                //

                xLog(LogHandle, XLL_PASS, "sleep(0) call took:%I64u ", ulFileTimeFinish.QuadPart);
                continue;

            } else {

                ulExpectedTime = (x * 2) * 10000000;
                ulExpectedTimeFinish = (ulExpectedTime + ulExpectedTime/10);
                ulExpectedTimeStart = (ulExpectedTime - ulExpectedTime/10);

            }

            xLog(LogHandle,
                 ((ulFileTimeFinish.QuadPart < ulExpectedTimeFinish) && 
                  (ulFileTimeFinish.QuadPart > ulExpectedTimeStart))
                 ? XLL_PASS : XLL_FAIL, "thread sequence #:%d", x);

        }

        xEndVariation( LogHandle );
    }
    //
    // Scenario #2
    // IO completion scenario
    // 
    // 1.) loop:
    // 2.) start a file operation that takes increasingly long
    // 3.) SleepEx it for 5 seconds
    // 4.) check to see that the sleep terminates when the IO completes, 
    //     and that the sleep finishes when the IO takes too long
    // 5.) check return value of sleepex
    // 6.) do it again
    //
    {
        HANDLE hTestFile = 0;
        LPVOID pBuffer = 0;
        OVERLAPPED ReadOverlapped = {0};
        OVERLAPPED WriteOverlapped = {0};
        int x, y;
        DWORD RetVal;
        DWORD BytesWritten;
        WCHAR buffer[200] = {0};

        xSetFunctionName( LogHandle, "SleepEx" );
        xStartVariation( LogHandle, "Variation#2" );

        //
        // Create the file we use for the test
        //

        hTestFile = CreateFile("z:\\testfile",
                               GENERIC_READ | GENERIC_WRITE | STANDARD_RIGHTS_ALL,
                               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_WRITE_THROUGH,
                               NULL);

        if (INVALID_HANDLE_VALUE == hTestFile) {
            xLog(LogHandle, XLL_BLOCK, "file Creation error #:%d", GetLastError());
            goto s2_end;
        }

        for (x = 1; x <= 50; x++) {

            DWORD dwBytesWritten;

            pBuffer = VirtualAlloc(NULL, x * 512 * 1024, MEM_COMMIT, PAGE_READWRITE);
            if (0 == pBuffer) {
                xLog(LogHandle, XLL_BLOCK, "memory allocation failure: %d", x * 512 * 1024);
                goto s2_end;
            }

            //
            // do a file operation
            //

            if (0 == WriteFile(hTestFile,
                               pBuffer,
                               x * 512 * 1024,
                               &dwBytesWritten,
                               &WriteOverlapped)) {
                xLog(LogHandle, XLL_FAIL, "file write error #:%d", GetLastError());
            }


#ifdef DBG
            swprintf(buffer, L"GetLastError = %u\n", GetLastError());
            OutputDebugString(buffer);
            if (GetOverlappedResult(hTestFile, &WriteOverlapped, &BytesWritten, TRUE))
                OutputDebugString(L"GetOverlappedResult(hTestFile, &WriteOverlapped, ...) = TRUE\n");
#endif

            if (0 == ReadFileEx(hTestFile,
                                pBuffer,
                                x * 512 * 1024,
                                &ReadOverlapped,
                                TestFileIOCompletionRoutine)) {
                xLog(LogHandle, XLL_FAIL, "file read error #:%d", GetLastError());
            }

#ifdef DBG
            swprintf(buffer, L"GetLastError = %u\n", GetLastError());
            OutputDebugString(buffer);
            if (GetOverlappedResult(hTestFile, &ReadOverlapped, &BytesWritten, TRUE))
                OutputDebugString(L"GetOverlappedResult(hTestFile, &ReadOverlapped, ...) = TRUE\n");
#endif

            //
            // sleep, get return value
            //

            //if (FALSE == HasOverlappedIoCompleted(&ReadOverlapped) || 
            //    FALSE == HasOverlappedIoCompleted(&WriteOverlapped)) {
            RetVal = SleepEx(10000, TRUE);
            if (WAIT_IO_COMPLETION == RetVal) {
                xLog(LogHandle, XLL_PASS, "SleepEx IO completion occured");
            } else if (0 == RetVal) {
                xLog(LogHandle, XLL_PASS, "SleepEx finished");
            } else {
                xLog(LogHandle, XLL_FAIL, "Unexpected SleepEx return value: %d", RetVal);
            }
            //} else {
            //    xLog(LogHandle, XLL_PASS, "Overlapped IO completed");
            //    }

            if (0 == VirtualFree(pBuffer, 0, MEM_RELEASE)) {
                xLog(LogHandle, XLL_BLOCK, "memory free failure: %d", x * 512 * 1024);
                goto s2_end;
            }
            pBuffer = 0;
        }
        s2_end:
        //
        // clean up
        //

#ifdef DBG
        if (GetOverlappedResult(hTestFile, &WriteOverlapped, &BytesWritten, TRUE))
            OutputDebugString(L"GetOverlappedResult(hTestFile, &WriteOverlapped, ...) = TRUE\n");
        if (GetOverlappedResult(hTestFile, &ReadOverlapped, &BytesWritten, TRUE))
            OutputDebugString(L"GetOverlappedResult(hTestFile, &ReadOverlapped, ...) = TRUE\n");
#endif

        //if (CancelIo(hTestFile))
        //    OutputDebugString(L"CancelIO = TRUE\n");

        if (hTestFile) {
            CloseHandle(hTestFile);
        }

        if (pBuffer) {
            VirtualFree(pBuffer, 0, MEM_RELEASE);
        }

        xEndVariation( LogHandle );
    }
    //
    // Scenario #3
    // APC queued to thread
    // make sure to do one with an INFINITE delay
    // 1.) create thread 
    // 2.) SleepEx it
    // 3.) throw APCs at it
    // 4.) make sure it wakes and processes
    // 5.) check return value of sleepex
    //
    //

    {
        HANDLE threads[THREADNUM] = {0};
        DWORD x;
        DWORD ThreadRetVal;
        DWORD LastError = 0;
        FILETIME FileTimeStart;
        FILETIME FileTimeFinish;
        ULARGE_INTEGER ulFileTimeStart;
        ULARGE_INTEGER ulFileTimeFinish;
        ULONGLONG ulExpectedTime;
        ULONGLONG ulExpectedTimeStart;
        ULONGLONG ulExpectedTimeFinish;
        DWORD RetVal;

        xSetFunctionName( LogHandle, "SleepEx" );
        xStartVariation( LogHandle, "Variation#3" );

        //
        // Create the threads
        //
        ZeroMemory(&ResultsArray[0], THREADNUM * (sizeof (struct RESULTSARRAY)));

        for (x = 0; x < THREADNUM; x++) {

            ThreadParams[x].bAlertable = TRUE;
            ThreadParams[x].dwMilliseconds = 60000;    // one minute
            ThreadParams[x].IoCompletionPortHandle = 0;
            ThreadParams[x].threadnum = x;
            ThreadParams[x].dwErrorCode = FALSE;
            ThreadParams[x].dwNumberOfBytesTransfered = 0;
            ThreadParams[x].NumberOfBytes = 0;
            ThreadParams[x].CompletionKey = 0;
            ThreadParams[x].CompletionStatusRetVal = 0;
            
            //
            // create the thread that will have the sleepex and io completion port
            // that will get touched by the posted completion status
            //

            threads[x] = CreateThread(NULL,
                                      65536,
                                      (LPTHREAD_START_ROUTINE)SleepExTestThread,
                                      (LPVOID)&ThreadParams[x],
                                      0,
                                      &ThreadID);
        }

        //
        // make sure they all started and have completion ports
        //

        for (x = 0; x < THREADNUM; x++) {
            if (0 == ResultsArray[x].start.wYear) {
                Sleep(0);   // yield a little
                x = 0;      // and start again
            }
        }

        //
        // post the statuses
        //

        for (x = 0; x < THREADNUM; x++) {

            //
            // wait a while, then post (we end up posting to a thread 1 time / second)
            //

            Sleep(1000);
            RetVal = QueueUserAPC((PAPCFUNC)TestAPCProc,
                                  threads[x],
                                  (ULONG_PTR)x);

            if (0 == RetVal) {
                LastError = GetLastError();
            }
        }

        //
        // check the results
        //

        WaitForMultipleObjects(THREADNUM, 
                               &threads[0],
                               TRUE,
                               200 * 1000);

        for (x = 0; x < THREADNUM; x++) {

            //
            // get time from the test thread
            //

            SystemTimeToFileTime(&ResultsArray[x].start, &FileTimeStart);
            SystemTimeToFileTime(&ResultsArray[x].finish, &FileTimeFinish);
            ulFileTimeStart.LowPart = FileTimeStart.dwLowDateTime;  
            ulFileTimeStart.HighPart = FileTimeStart.dwHighDateTime;
            ulFileTimeFinish.LowPart = FileTimeFinish.dwLowDateTime;  
            ulFileTimeFinish.HighPart = FileTimeFinish.dwHighDateTime;
            ulFileTimeFinish.QuadPart -= ulFileTimeStart.QuadPart;

            if (0 == x) {

                //
                // ok, how long should sleeping 0 take...
                //

                xLog(LogHandle, XLL_PASS, "sleep(0) call took:%I64u ", ulFileTimeFinish.QuadPart);
                continue;

            } else {

                ulExpectedTime = (x + 1) * 10000000;
                ulExpectedTimeFinish = (ulExpectedTime + ulExpectedTime/10);
                ulExpectedTimeStart = (ulExpectedTime - ulExpectedTime/10);
            }

            GetExitCodeThread(threads[x], &ThreadRetVal);
            CloseHandle(threads[x]);

            xLog(LogHandle,
                 ((ulFileTimeFinish.QuadPart < ulExpectedTimeFinish) && 
                  (ulFileTimeFinish.QuadPart > ulExpectedTimeStart) && 
                  WAIT_IO_COMPLETION == ThreadRetVal && 
                  ThreadParams[x].dwErrorCode)
                 ? XLL_PASS : XLL_FAIL, "thread sequence #:%d retval:%d", x, ThreadRetVal);
        }

        xEndVariation( LogHandle );
    }


    //
    // negative test cases
    //

//    xSetFunctionName( LogHandle, "SleepEx" );
//    xStartVariation( LogHandle, "badparam1" );
//
//    
//   
//    xLog(LogHandle,
//         FileHandle == INVALID_HANDLE_VALUE ? XLL_FAIL : XLL_PASS,
//         "... (LastError=%d)",
//         GetLastError()
//         );

    return;
}

VOID 
CALLBACK 
TestAPCProc(
  ULONG_PTR dwParam
)
/*++

Routine Description:

    test APCProc for the SleepEx() API test

Arguments:

    dwParam

Return Value:

    None

--*/
{
    
#ifdef DBG
    WCHAR dbgbuffer[100];
    swprintf(dbgbuffer, L"TestAPCProc hit: %u\n", dwParam);
    OutputDebugString(dbgbuffer);
#endif
    ThreadParams[dwParam].dwErrorCode = TRUE;
    return;
}

VOID
CALLBACK 
TestFileIOCompletionRoutine(
  DWORD dwErrorCode,
  DWORD dwNumberOfBytesTransfered,
  LPOVERLAPPED lpOverlapped
)
/*++

Routine Description:

    test TestFileIOCompletionRoutine for the SleepEx() API test

Arguments:

    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped

Return Value:

    None

--*/
{
#ifdef DBG
    WCHAR dbgbuffer[200];

    swprintf(dbgbuffer, 
             L"TestFileIOCompletionRoutine hit:\n\tErrorCode: %d\n\tdwNumberOfBytesTransfered %d\n", 
             dwErrorCode,
             dwNumberOfBytesTransfered);
    OutputDebugString(dbgbuffer);
#endif    
    return;
}

DWORD
WINAPI
SleepExTestThread(
    LPVOID lpParameter
    )
/*++

Routine Description:

    test thread for the SleepEx() API test

Arguments:

    lpParameter - pointer to struct SLEEPEXTESTTHREADPARAMS

Return Value:

    None

--*/
{
    LPSLEEPEXTESTTHREADPARAMS param;
    DWORD RetVal;
    
    param = (LPSLEEPEXTESTTHREADPARAMS)lpParameter;

    if(FALSE == param->bAlertable){

        GetSystemTime(&ResultsArray[param->threadnum].start);
        RetVal = SleepEx(param->dwMilliseconds, param->bAlertable);
        GetSystemTime(&ResultsArray[param->threadnum].finish);

    } else {
        
        //
        // APC case
        //
        
        GetSystemTime(&ResultsArray[param->threadnum].start);
        RetVal = SleepEx(param->dwMilliseconds, param->bAlertable);
        GetSystemTime(&ResultsArray[param->threadnum].finish);
        
    }
    ExitThread(RetVal);
    return(RetVal); // make compiler happy
}

VOID
WINAPI
TimeTestStartTest(
    HANDLE LogHandle
    )

/*++

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

--*/

{
    LONG ID;

    OutputDebugString( TEXT("timetest: StartTest is called\n") );

    //
    // One way to run multiple threads without creating threads is to
    // set NumberOfThreads=n under the section [sample.dll]. Everytime
    // the harness calls StartTest, ThreadID will get increment by one
    // after InterlockedIncrement call below. If no NumberOfThreads
    // specified, only RunTest1 will be run
    // 

    ID = InterlockedIncrement( &ThreadID );

    xSetComponent( LogHandle, "kernel", "timetest" );

    //
    // call the tests
    //

    // this exposes some timezone problems I need to look at later
    //    test_GetTimeZoneInformationSimpleCase(LogHandle);
    
    test_CompareFileTime(LogHandle);
    test_FileTimeToLocalFileTime(LogHandle);
    test_GetTickCount(LogHandle);
    test_Sleep(LogHandle);
    test_SleepEx(LogHandle);
    test_GetTimeZoneInformation(LogHandle);

    return;

}

VOID
WINAPI
TimeTestEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    ThreadID = 0;
    OutputDebugString( TEXT("timetest: EndTest is called\n") );
}

VOID
WINAPI
test_CompareFileTime(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test the CompareFileTime() API

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

--*/
{
    FILETIME Time1 = {0,0};
    FILETIME Time2 = {0,0};
    
    //
    // positive test cases
    //
    
    xSetFunctionName( LogHandle, "CompareFileTime" );
    xStartVariation( LogHandle, "goodparam1" );
    
    //
    // time1 > time2
    //

    Time1.dwLowDateTime  = 1;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 1;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = MAX_DWORD;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = MAX_DWORD - 1;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = MAX_DWORD;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = MAX_DWORD - 1;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = MAX_DWORD;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = MAX_DWORD - 1;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 1 ? XLL_PASS : XLL_FAIL,
         "time1 > time2");

    //
    // time1 == time2
    //

    Time1.dwLowDateTime  = 1;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 1;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 1;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 1;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = MAX_DWORD;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = MAX_DWORD;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");
    
    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = MAX_DWORD;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = MAX_DWORD;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == 0 ? XLL_PASS : XLL_FAIL,
         "time1 == time2");
    
    //
    // time1 < time2
    //

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 1;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = 1;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = MAX_DWORD;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = MAX_DWORD - 1;
    Time1.dwHighDateTime = 0;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = 0;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = 0;
    Time1.dwHighDateTime = MAX_DWORD - 1;
    Time2.dwLowDateTime  = 0;
    Time2.dwHighDateTime = MAX_DWORD;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");

    Time1.dwLowDateTime  = MAX_DWORD;
    Time1.dwHighDateTime = MAX_DWORD - 1;
    Time2.dwLowDateTime  = MAX_DWORD;
    Time2.dwHighDateTime = MAX_DWORD;
    xLog(LogHandle,
         CompareFileTime( &Time1, &Time2 ) == -1 ? XLL_PASS : XLL_FAIL,
         "time1 < time2");


    xEndVariation( LogHandle );

    return;
}


VOID
WINAPI
test_GetTimeZoneInformationSimpleCase(
    HANDLE LogHandle
    )
/*++

Routine Description:

    document some strange things I found while writting test_GetTimeZoneInformation()

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

Notes:
    


--*/

{
    #undef YEARS
    #define YEARS 1
    #define START_YEAR 2002

    // time that timzone switch happen - I only tested this value, no guarentees my 
    // code works with others, though it should
    #define ROLL_HOUR 12    

    SYSTEMTIME CompareSTDdate[YEARS] = {0};
    SYSTEMTIME CompareDSTdate[YEARS] = {0};

    SYSTEMTIME SaveLocalTime = {0}, SaveSystemTime = {0};
    SYSTEMTIME TestLocalTime = {0}, TestSystemTime = {0};
    TIME_ZONE_INFORMATION SaveTimeZoneInformation = {0};
    TIME_ZONE_INFORMATION SetTestTimeZoneInformation = {0}, GetTestTimeZoneInformation = {0};

    int DSTFlag = 0;
    int TimeZone = 0;
    int YearOffset = 0;
    BOOL bUsingDST = XapipUseDaylightSavingTime();
    ULONG type, size;
    DWORD flags;
    BOOL bRetVal;
    DWORD dwRetVal;
    DWORD dwLastError;
    WCHAR wbuffer[250] = {0};
    LONG TotalBias = 0;
    LONG DayAdjust = 0;
    LONG HourAdjust = 0;

    //
    // initialize non-variable test data
    //

    SetTestTimeZoneInformation.StandardDate.wYear         = 0; 
    SetTestTimeZoneInformation.StandardDate.wMonth        = 3; 
    SetTestTimeZoneInformation.StandardDate.wDayOfWeek    = 0;  // 0-6, 0 == Sunday 
    SetTestTimeZoneInformation.StandardDate.wDay          = 1;  // which instance of day in month, 1-5
    SetTestTimeZoneInformation.StandardDate.wHour         = ROLL_HOUR; 
    SetTestTimeZoneInformation.StandardDate.wMinute       = 0; 
    SetTestTimeZoneInformation.StandardDate.wSecond       = 0; 
    SetTestTimeZoneInformation.StandardDate.wMilliseconds = 0;

    SetTestTimeZoneInformation.DaylightDate.wYear         = 0;
    SetTestTimeZoneInformation.DaylightDate.wMonth        = 10; 
    SetTestTimeZoneInformation.DaylightDate.wDayOfWeek    = 3;  // 0-6, 0 == Sunday                    
    SetTestTimeZoneInformation.DaylightDate.wDay          = 4;  // which instance of day in month, 1-5 
    SetTestTimeZoneInformation.DaylightDate.wHour         = ROLL_HOUR; 
    SetTestTimeZoneInformation.DaylightDate.wMinute       = 0; 
    SetTestTimeZoneInformation.DaylightDate.wSecond       = 0; 
    SetTestTimeZoneInformation.DaylightDate.wMilliseconds = 0;

    //
    // initialize compare data
    //

    //CompareSTDdate[0];  // 2002
    CompareSTDdate[0].wYear         = START_YEAR + 0;
    CompareSTDdate[0].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[0].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[0].wDay          = 3;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[0].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[0].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[0].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[0].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[0];  // 2002
    CompareDSTdate[0].wYear         = START_YEAR + 0;
    CompareDSTdate[0].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[0].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[0].wDay          = 23;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[0].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[0].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[0].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[0].wMilliseconds = 0;    // ignore!


    xSetFunctionName( LogHandle, "GetTimeZoneInformation" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // save surrent timezone
    // save current time
    //

    if (0xffffffff == GetTimeZoneInformation( &SaveTimeZoneInformation )) {
        xLog(LogHandle,
             XLL_BLOCK,
             "GetTimeZoneInformation failed! aborting test");
        xEndVariation( LogHandle );
        return;
    }

    GetLocalTime( &SaveLocalTime );
    GetSystemTime( &SaveSystemTime );

    //
    // for (DST enabled / DST disabled)
    // DSTFlag == 0 = disabled
    // DSTFlag == 1 = enabled
    // 

    DSTFlag = 1;

    if (ERROR_SUCCESS == XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size)) {
        if (DSTFlag) {
            flags &= ~XC_MISC_FLAG_DONT_USE_DST;
        } else {
            flags |= XC_MISC_FLAG_DONT_USE_DST;
        }

        XSetValue(XC_MISC_FLAGS, REG_DWORD, &flags, sizeof(flags));

    } else {
        xLog(LogHandle,
             XLL_BLOCK,
             "XQueryValue failed! aborting test!");
        xEndVariation( LogHandle );
        return;
    }

    //
    // set and check timezone info
    //
    TimeZone = 8;

    SetTestTimeZoneInformation.Bias = 60 * TimeZone;   // added to UTC to get current time
    swprintf(SetTestTimeZoneInformation.StandardName, L"STDTestTZName%d", TimeZone);
    SetTestTimeZoneInformation.StandardBias = 0;        // base time, no bias in most cases
    swprintf(SetTestTimeZoneInformation.DaylightName, L"DSTTestTZName%d", TimeZone);
    SetTestTimeZoneInformation.DaylightBias = -60;      // subtract 60 minutes

    bRetVal = XapipSetTimeZoneInformation(&SetTestTimeZoneInformation);
    dwLastError = GetLastError();
    if (!NT_SUCCESS(bRetVal)) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapipSetTimeZoneInformation failed. GetLastError %lu",
             dwLastError);
    } else {
        xLog(LogHandle,
             XLL_PASS,
             "XapipSetTimeZoneInformation succeeded:StandardName=%S : DaylightName=%S", 
             SetTestTimeZoneInformation.StandardName, 
             SetTestTimeZoneInformation.DaylightName);
    }

    //
    // make sure GetTimeZoneInformation() works OK
    //

    dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
    dwLastError = GetLastError();
    if (TIME_ZONE_ID_INVALID == dwRetVal) {
        xLog(LogHandle,
             XLL_FAIL,
             "GetTimeZoneInformation returned TIME_ZONE_ID_UNKNOWN. GetLastError %lu",
             dwLastError);
    } else {
        xLog(LogHandle,
             XLL_PASS,
             "GetTimeZoneInformation succeeded");
    }


    //
    // set / get / check 
    // for (this year, next year ... in 5 years)
    // yes, this is overkill, but cycles are free
    // 

    YearOffset = 0;

    //
    // for (each side of DST, and across boundary)
    // DST has a start date and an end date (April 1, October 28)
    // in the US, it starts at 2AM, it starts at other times in other parts of the world
    // the cases we'll hit:
    //
    // set time 1 second before it begins - check time (this is also the pre-switch test)
    // check GetTimeZoneInformation()
    // sleep 2 seconds
    // check time, make sure it has moved forward - check time (this is also the post-switch test)
    //
    // set time 1 second before it ends - check time (this is also the pre-switch test)
    // check GetTimeZoneInformation()
    // sleep 2 seconds
    // check time, make sure it has moved back - check time (this is also the post-switch test)
    //
    // if it is not enabled (DSTFlag == 0), make sure it has not moved
    // if it is enabled (DSTFlag == 1), make sure it not moved
    //

    //
    // go from DST -> Standard
    // get us 1 second before Standard date starts
    //

    TestLocalTime.wYear         = CompareSTDdate[YearOffset].wYear;
    TestLocalTime.wMonth        = CompareSTDdate[YearOffset].wMonth;
    TestLocalTime.wDay          = CompareSTDdate[YearOffset].wDay;
    TestLocalTime.wHour         = SetTestTimeZoneInformation.StandardDate.wHour - 1;
    TestLocalTime.wMinute       = 59;
    TestLocalTime.wSecond       = 59;
    TestLocalTime.wMilliseconds = 0;

    //
    // set the local time, sleep 2 seconds
    // we are now in DST
    // we set local time 2 times since there is a known problem in XBox with 
    //

    if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapiSetLocalTime failed setting test time info (DST->Standard)!");
    } else {
        xLog(LogHandle,
             XLL_PASS,
             "XapiSetLocalTime set test time info (DST->Standard)");
    }

    //
    // check the timezone information
    //

    dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
    dwLastError = GetLastError();

    switch (dwRetVal) {
        case TIME_ZONE_ID_INVALID:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (DST->Standard). GetLastError %lu",
                 dwLastError);
            break;

            // what should happen if there is no DST enabled?
        case TIME_ZONE_ID_UNKNOWN:
            if (DSTFlag == 0) { // no DST
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==0)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (DST->Standard:DSTFlag==1)");
            }
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            if (DSTFlag == 1) { // DST enabled
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==1)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (DST->Standard:DSTFlag==0)");
            }
            break;

        default:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation - wrong result - default case hit (DST->Standard)! GetLastError %lu : dwRetVal = %lu", 
                 dwLastError, dwRetVal);
            break;
    }

    //
    // roll over
    //

    Sleep(2000);

    //
    // get the local time, we have crossed the line from DST to Standard
    // we are now in Standard
    //

    GetLocalTime(&TestLocalTime);
    GetSystemTime(&TestSystemTime);

    //
    // check the timezone information
    //

    dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
    dwLastError = GetLastError();
    switch (dwRetVal) {
        case TIME_ZONE_ID_INVALID:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (DST->Standard). GetLastError %lu",
                 dwLastError);
            break;

            // what should happen if there is no DST enabled?
        case TIME_ZONE_ID_UNKNOWN:
            if (DSTFlag == 0) { // no DST
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==0)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (DST->Standard:DSTFlag==1)");
            }
            break;

        case TIME_ZONE_ID_STANDARD:
            if (DSTFlag == 1) { // DST enabled
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==1)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (DST->Standard:DSTFlag==0)");
            }
            break;

        default:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation - wrong result - default case hit! (DST->Standard)  GetLastError %lu : dwRetVal = %lu", 
                 dwLastError, dwRetVal);
            break;
    }

    if (DSTFlag == 0) {

        //
        // should be 2 seconds forward, no bias
        //

        if ((TestLocalTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
            (TestLocalTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
            (TestLocalTime.wDay    == CompareSTDdate[YearOffset].wDay) &&
            (TestLocalTime.wHour   == CompareSTDdate[YearOffset].wHour) &&
            (TestLocalTime.wMinute == 0) &&
            (TestLocalTime.wSecond == 1)) {
            xLog(LogHandle,
                 XLL_PASS,
                 "TestLocalTime correct (DST->Standard:DSTFlag==0)");
        } else {
            xLog(LogHandle,
                 XLL_FAIL,
                 "TestLocalTime incorrect (DST->Standard:DSTFlag==0)");
            DumpSystemTime(TestLocalTime, L"TestLocalTime");
            DumpTimeZone(SetTestTimeZoneInformation);
        }

    } else {

        //
        // should be 2 seconds forward, DST bias applied (standard to DST - fall back!)
        //

        if ((TestLocalTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
            (TestLocalTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
            (TestLocalTime.wDay    == CompareSTDdate[YearOffset].wDay) &&
            ((TestLocalTime.wHour  == CompareSTDdate[YearOffset].wHour) + (SetTestTimeZoneInformation.DaylightBias / 60)) &&
            (TestLocalTime.wMinute == 0) &&
            (TestLocalTime.wSecond == 1)) {
            xLog(LogHandle,
                 XLL_PASS,
                 "TestLocalTime correct (DST->Standard:DSTFlag==1)");
        } else {
            xLog(LogHandle,
                 XLL_FAIL,
                 "TestLocalTime incorrect (DST->Standard:DSTFlag==1)");
            DumpSystemTime(TestLocalTime, L"TestLocalTime");
            DumpTimeZone(SetTestTimeZoneInformation);
        }
    }

    //
    // check the system time, we have crossed the line - system time should not change
    // rollover check - as we go to different timezones, the bias will roll us over
    // in this case, we apply the daylight bias
    //

    DayAdjust = 0;
    HourAdjust = TotalBias = (SetTestTimeZoneInformation.Bias + (DSTFlag ? (SetTestTimeZoneInformation.DaylightBias) : 0)) / 60;
    if (ROLL_HOUR + TotalBias > 23) {
        DayAdjust = 1;
        HourAdjust = TotalBias - 24;
    } else if (ROLL_HOUR + TotalBias < 0) {
        DayAdjust = -1;
        HourAdjust = 24 + TotalBias;
    }

    if ((TestSystemTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
        (TestSystemTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
        // adjust when bias rolls over
        (TestSystemTime.wDay    == CompareSTDdate[YearOffset].wDay + DayAdjust) &&
        // adjust when bias rolls over
        (TestSystemTime.wHour   == CompareSTDdate[YearOffset].wHour + HourAdjust) &&
        (TestSystemTime.wMinute == 0) &&
        (TestSystemTime.wSecond == 1)) {
        xLog(LogHandle,
             XLL_PASS,
             "TestSystemTime correct (DST->Standard)");
    } else {
        xLog(LogHandle,
             XLL_FAIL,
             "TestSystemTime incorrect (DST->Standard)");
        DumpSystemTime(TestSystemTime, L"TestSystemTime");
        DumpTimeZone(SetTestTimeZoneInformation);
    }

    //*************************************************************
    // now try Standard -> DST
    //*************************************************************

    //
    // get us 1 second before DST starts
    //

    TestLocalTime.wYear      = CompareDSTdate[YearOffset].wYear; 
    TestLocalTime.wMonth     = CompareDSTdate[YearOffset].wMonth;
    TestLocalTime.wDay       = CompareDSTdate[YearOffset].wDay;  
    TestLocalTime.wHour      = SetTestTimeZoneInformation.DaylightDate.wHour - 1;
    TestLocalTime.wMinute    = 59;
    TestLocalTime.wSecond    = 59;
    TestLocalTime.wMilliseconds = 0;

    //
    // set the local time, sleep 2 seconds
    // we are now in Standard time
    //

    if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapiSetLocalTime failed setting test time info (Standard->DST)!");
    } else {
        xLog(LogHandle,
             XLL_PASS,
             "XapiSetLocalTime set test time info (Standard->DST)");
    }

    //
    // check the timezone information
    //

    dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
    dwLastError = GetLastError();

    switch (dwRetVal) {
        case TIME_ZONE_ID_INVALID:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (Standard->DST). GetLastError %lu",
                 dwLastError);
            break;

            // what should happen if there is no DST enabled?
        case TIME_ZONE_ID_UNKNOWN:
            if (DSTFlag == 0) { // no DST
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==0)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (Standard->DST:DSTFlag==1)");
            }
            break;

        case TIME_ZONE_ID_STANDARD:
            if (DSTFlag == 1) { // DST enabled
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==1)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (Standard->DST:DSTFlag==0)");
            }
            break;

        default:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation - wrong result - default case hit (Standard->DST)!  GetLastError %lu : dwRetVal = %lu", 
                 dwLastError, dwRetVal);
            break;
    }

    //
    // roll over
    //

    Sleep(2000);

    //
    // get the local time, we have crossed the line from Standard to DST
    // we are now in DST
    //

    GetLocalTime(&TestLocalTime);
    GetSystemTime(&TestSystemTime);

    //
    // check the timezone information
    //

    dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
    dwLastError = GetLastError();
    switch (dwRetVal) {
        case TIME_ZONE_ID_INVALID:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (Standard->DST). GetLastError %lu",
                 dwLastError);
            break;

            // what should happen if there is no DST enabled?
        case TIME_ZONE_ID_UNKNOWN:
            if (DSTFlag == 0) { // no DST
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==0)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (Standard->DST:DSTFlag==1)");
            }
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            if (DSTFlag == 1) { // DST enabled
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==1)");
            } else {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (Standard->DST:DSTFlag==0)");
            }
            break;

        default:
            xLog(LogHandle,
                 XLL_FAIL,
                 "GetTimeZoneInformation - wrong result - default case hit! (Standard->DST).  GetLastError %lu : dwRetVal = %lu", 
                 dwLastError, dwRetVal);
            break;
    }

    if (DSTFlag == 0) {

        //
        // should be 2 seconds forward, no bias
        //

        if ((TestLocalTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
            (TestLocalTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
            (TestLocalTime.wDay    == CompareDSTdate[YearOffset].wDay) &&
            (TestLocalTime.wHour   == CompareDSTdate[YearOffset].wHour) &&
            (TestLocalTime.wMinute == 0) &&
            (TestLocalTime.wSecond == 1)) {
            xLog(LogHandle,
                 XLL_PASS,
                 "TestLocalTime correct (Standard->DST:DSTFlag==0)");
        } else {
            xLog(LogHandle,
                 XLL_FAIL,
                 "TestLocalTime incorrect (Standard->DST:DSTFlag==0)");
            DumpSystemTime(TestLocalTime, L"TestLocalTime");
            DumpTimeZone(SetTestTimeZoneInformation);
        }

    } else {

        //
        // should be 2 seconds forward, DST bias applied (standard to DST - spring forward!)
        //

        if ((TestLocalTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
            (TestLocalTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
            (TestLocalTime.wDay    == CompareDSTdate[YearOffset].wDay) &&
            ((TestLocalTime.wHour  == CompareDSTdate[YearOffset].wHour) + (SetTestTimeZoneInformation.DaylightBias / 60)) &&
            (TestLocalTime.wMinute == 0) &&
            (TestLocalTime.wSecond == 1)) {
            xLog(LogHandle,
                 XLL_PASS,
                 "TestLocalTime correct (Standard->DST:DSTFlag==1)");
        } else {
            xLog(LogHandle,
                 XLL_FAIL,
                 "TestLocalTime incorrect (Standard->DST:DSTFlag==1)");
            DumpSystemTime(TestLocalTime, L"TestLocalTime");
            DumpTimeZone(SetTestTimeZoneInformation);
        }
    }

    //
    // check the system time, we have crossed the line - system time should not change
    // rollover check - as we go to different timezones, the bias will roll us over
    //

    DayAdjust = 0;
    HourAdjust = TotalBias = SetTestTimeZoneInformation.Bias / 60;
    if (ROLL_HOUR + TotalBias > 23) {
        DayAdjust = 1;
        HourAdjust = TotalBias - 24;
    } else if (ROLL_HOUR + TotalBias < 0) {
        DayAdjust = -1;
        HourAdjust = 24 + TotalBias;
    }


    if ((TestSystemTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
        (TestSystemTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
        // adjust when bias rolls over
        (TestSystemTime.wDay    == CompareDSTdate[YearOffset].wDay + DayAdjust) &&
        // adjust when bias rolls over
        (TestSystemTime.wHour   == CompareDSTdate[YearOffset].wHour + HourAdjust) &&
        (TestSystemTime.wMinute == 0) &&
        (TestSystemTime.wSecond == 1)) {
        xLog(LogHandle,
             XLL_PASS,
             "TestSystemTime correct (Standard->DST)");
    } else {
        xLog(LogHandle,
             XLL_FAIL,
             "TestSystemTime incorrect (Standard->DST)");
        DumpSystemTime(TestSystemTime, L"TestSystemTime");
        DumpTimeZone(SetTestTimeZoneInformation);
    }

    //
    // restore surrent timezone
    // restore current time
    //

    if (ERROR_SUCCESS == XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size)) {
        if (bUsingDST) {
            flags &= ~XC_MISC_FLAG_DONT_USE_DST;
        } else {
            flags |= XC_MISC_FLAG_DONT_USE_DST;
        }

        XSetValue(XC_MISC_FLAGS, REG_DWORD, &flags, sizeof(flags));
    }

    if (!NT_SUCCESS(XapipSetTimeZoneInformation( &SaveTimeZoneInformation ))) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapipSetTimeZoneInformation failed restoring saved time zone info!");
        DumpSystemTime(SaveTimeZoneInformation.DaylightDate, L"SaveTimeZoneInformation.DaylightDate:" );
        DumpSystemTime(SaveTimeZoneInformation.StandardDate, L"SaveTimeZoneInformation.StandardDate:" );
    }

    if (FALSE == XapiSetLocalTime( &SaveLocalTime )) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapiSetLocalTime failed restoring saved time info!");
    }

    xEndVariation( LogHandle );
    return;
}

VOID
WINAPI
test_GetTimeZoneInformation(
    HANDLE LogHandle
    )
/*++

Routine Description:

    test the GetTimeZoneInformation() API

Arguments:

    LogHandle - Handle used to call logging APIs

Return Value:

    None

Notes:
    
    save surrent timezone
    save current time
    for (DST enabled / DST disabled)
        set / get /check 
            for (each time zone(0(GMT)-23))
                XapipSetTimeZoneInformation()
                GetTimeZoneInformation()
                    set / get / check 
                        for (each side of DST, and at boundary)
                            for (this year, next year, in 5 years)
                            XapiSetLocalTime()
                            GetLocalTime()
                            GetSystemTime()
    restore surrent timezone
    restore current time

    this needs to be ported to NT and compared

--*/ 
{
    #undef YEARS
    #define YEARS 5
    #define START_YEAR 2002

    // time that timzone switch happen - I only tested this value, no guarentees my 
    // code works with others, though it should
    #define ROLL_HOUR 12    

    SYSTEMTIME CompareSTDdate[YEARS] = {0};
    SYSTEMTIME CompareDSTdate[YEARS] = {0};

    SYSTEMTIME SaveLocalTime = {0}, SaveSystemTime = {0};
    SYSTEMTIME TestLocalTime = {0}, TestSystemTime = {0};
    TIME_ZONE_INFORMATION SaveTimeZoneInformation = {0};
    TIME_ZONE_INFORMATION SetTestTimeZoneInformation = {0}, GetTestTimeZoneInformation = {0};

    int DSTFlag = 0;
    int TimeZone = 0;
    int YearOffset = 0;
    BOOL bUsingDST = XapipUseDaylightSavingTime();
    ULONG type, size;
    DWORD flags;
    BOOL bRetVal;
    DWORD dwRetVal;
    DWORD dwLastError;
    WCHAR wbuffer[250] = {0};
    LONG TotalBias = 0;
    LONG DayAdjust = 0;
    LONG HourAdjust = 0;

    //
    // initialize non-variable test data
    //

    SetTestTimeZoneInformation.StandardDate.wYear         = 0; 
    SetTestTimeZoneInformation.StandardDate.wMonth        = 3; 
    SetTestTimeZoneInformation.StandardDate.wDayOfWeek    = 0;  // 0-6, 0 == Sunday 
    SetTestTimeZoneInformation.StandardDate.wDay          = 1;  // which instance of day in month, 1-5
    SetTestTimeZoneInformation.StandardDate.wHour         = ROLL_HOUR; 
    SetTestTimeZoneInformation.StandardDate.wMinute       = 0; 
    SetTestTimeZoneInformation.StandardDate.wSecond       = 0; 
    SetTestTimeZoneInformation.StandardDate.wMilliseconds = 0;

    SetTestTimeZoneInformation.DaylightDate.wYear         = 0;
    SetTestTimeZoneInformation.DaylightDate.wMonth        = 10; 
    SetTestTimeZoneInformation.DaylightDate.wDayOfWeek    = 3;  // 0-6, 0 == Sunday                    
    SetTestTimeZoneInformation.DaylightDate.wDay          = 4;  // which instance of day in month, 1-5 
    SetTestTimeZoneInformation.DaylightDate.wHour         = ROLL_HOUR; 
    SetTestTimeZoneInformation.DaylightDate.wMinute       = 0; 
    SetTestTimeZoneInformation.DaylightDate.wSecond       = 0; 
    SetTestTimeZoneInformation.DaylightDate.wMilliseconds = 0;

    //
    // initialize compare data
    //

    //CompareSTDdate[0];  // 2002
    CompareSTDdate[0].wYear         = START_YEAR + 0;
    CompareSTDdate[0].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[0].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[0].wDay          = 3;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[0].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[0].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[0].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[0].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[0];  // 2002
    CompareDSTdate[0].wYear         = START_YEAR + 0;
    CompareDSTdate[0].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[0].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[0].wDay          = 23;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[0].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[0].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[0].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[0].wMilliseconds = 0;    // ignore!

    //CompareSTDdate[1];  // 2003
    CompareSTDdate[1].wYear         = START_YEAR + 1;
    CompareSTDdate[1].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[1].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[1].wDay          = 2;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[1].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[1].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[1].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[1].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[1];  // 2003
    CompareDSTdate[1].wYear         = START_YEAR + 1;
    CompareDSTdate[1].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[1].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[1].wDay          = 22;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[1].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[1].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[1].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[1].wMilliseconds = 0;    // ignore!

    //CompareSTDdate[2];  // 2004
    CompareSTDdate[2].wYear         = START_YEAR + 2;
    CompareSTDdate[2].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[2].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[2].wDay          = 7;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[2].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[2].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[2].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[2].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[2];  // 2004
    CompareDSTdate[2].wYear         = START_YEAR + 2;
    CompareDSTdate[2].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[2].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[2].wDay          = 27;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[2].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[2].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[2].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[2].wMilliseconds = 0;    // ignore!

    //CompareSTDdate[3];  // 2005
    CompareSTDdate[3].wYear         = START_YEAR + 3;
    CompareSTDdate[3].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[3].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[3].wDay          = 6;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[3].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[3].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[3].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[3].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[3];  // 2005
    CompareDSTdate[3].wYear         = START_YEAR + 3;
    CompareDSTdate[3].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[3].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[3].wDay          = 26;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[3].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[3].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[3].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[3].wMilliseconds = 0;    // ignore!

    //CompareSTDdate[4];  // 2006
    CompareSTDdate[4].wYear         = START_YEAR + 4;
    CompareSTDdate[4].wMonth        = SetTestTimeZoneInformation.StandardDate.wMonth;
    CompareSTDdate[4].wDayOfWeek    = 0;    // we don't compare this
    CompareSTDdate[4].wDay          = 5;    // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareSTDdate[4].wHour         = SetTestTimeZoneInformation.StandardDate.wHour;    // this is adjusted in the test
    CompareSTDdate[4].wMinute       = 0;    // this is adjusted in the test
    CompareSTDdate[4].wSecond       = 0;    // this is adjusted in the test
    CompareSTDdate[4].wMilliseconds = 0;    // ignore!
    //CompareDSTdate[4];  // 2006
    CompareDSTdate[4].wYear         = START_YEAR + 4;
    CompareDSTdate[4].wMonth        = SetTestTimeZoneInformation.DaylightDate.wMonth;
    CompareDSTdate[4].wDayOfWeek    = 0;    // we don't compare this
    CompareDSTdate[4].wDay          = 25;   // real day of month, as opposed to the scheme used by Get/SetTimeZoneInformation
    CompareDSTdate[4].wHour         = SetTestTimeZoneInformation.DaylightDate.wHour;    // this is adjusted in the test
    CompareDSTdate[4].wMinute       = 0;    // this is adjusted in the test
    CompareDSTdate[4].wSecond       = 0;    // this is adjusted in the test
    CompareDSTdate[4].wMilliseconds = 0;    // ignore!


    xSetFunctionName( LogHandle, "GetTimeZoneInformation" );
    xStartVariation( LogHandle, "goodparam1" );

    //
    // save surrent timezone
    // save current time
    //

    if (0xffffffff == GetTimeZoneInformation( &SaveTimeZoneInformation )) {
        xLog(LogHandle,
             XLL_BLOCK,
             "GetTimeZoneInformation failed! aborting test");
        xEndVariation( LogHandle );
        return;
    }

    GetLocalTime( &SaveLocalTime );
    GetSystemTime( &SaveSystemTime );

    //
    // for (DST enabled / DST disabled)
    // DSTFlag == 0 = disabled
    // DSTFlag == 1 = enabled
    // 

    for (DSTFlag = 0; DSTFlag < 2; DSTFlag++) {

        if (ERROR_SUCCESS == XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size)) {
            if (DSTFlag) {
                flags &= ~XC_MISC_FLAG_DONT_USE_DST;
            } else {
                flags |= XC_MISC_FLAG_DONT_USE_DST;
            }

            XSetValue(XC_MISC_FLAGS, REG_DWORD, &flags, sizeof(flags));

        } else {
            xLog(LogHandle,
                 XLL_BLOCK,
                 "XQueryValue failed! aborting test!");
            xEndVariation( LogHandle );
            return;
        }

        //
        // for (each time zone(0(GMT)-23))
        // TimeZone = 0 = GMT/UTC
        // 0-23 gets you around the world
        //

        for (TimeZone = -13; TimeZone <= 13; TimeZone++) {

            //
            // hack part1 - there were some problems with going through the year loop
            // they seemed to have to do with getting the time info all synced up
            // if we do the folloing steps, it works
            // there is a similar hack in the dash, but their hack is nicer than mine
            //*****************************************************************
            // go from DST -> Standard
            // get us 3 hours before Standard date starts
            //

            TestLocalTime.wYear         = CompareSTDdate[0].wYear;
            TestLocalTime.wMonth        = CompareSTDdate[0].wMonth;
            TestLocalTime.wDay          = CompareSTDdate[0].wDay;
            TestLocalTime.wHour         = SetTestTimeZoneInformation.StandardDate.wHour - 3;
            TestLocalTime.wMinute       = 0;
            TestLocalTime.wSecond       = 0;
            TestLocalTime.wMilliseconds = 0;

            if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
                xLog(LogHandle,
                     XLL_FAIL,
                     "hack2:part1:XapiSetLocalTime failed setting test time info (DST->Standard)!");
            } else {
                xLog(LogHandle,
                     XLL_PASS,
                     "hack2:part1:XapiSetLocalTime set test time info (DST->Standard)");
            }

            //
            // end hack part 1
            //*****************************************************************
            //

            //
            // set and check timezone info
            //

            SetTestTimeZoneInformation.Bias = 60 * TimeZone;   // added to UTC to get current Local time
            swprintf(SetTestTimeZoneInformation.StandardName, L"STDTestTZName%d", TimeZone);
            SetTestTimeZoneInformation.StandardBias = 0;        // base time, no bias in most cases
            swprintf(SetTestTimeZoneInformation.DaylightName, L"DSTTestTZName%d", TimeZone);
            SetTestTimeZoneInformation.DaylightBias = -60;      // subtract 60 minutes for the typical DST case

            bRetVal = XapipSetTimeZoneInformation(&SetTestTimeZoneInformation);
            dwLastError = GetLastError();
            if (!NT_SUCCESS(bRetVal)) {
                xLog(LogHandle,
                     XLL_FAIL,
                     "XapipSetTimeZoneInformation failed. GetLastError %lu",
                     dwLastError);
            } else {
                xLog(LogHandle,
                     XLL_PASS,
                     "XapipSetTimeZoneInformation succeeded:StandardName=%S : DaylightName=%S", 
                     SetTestTimeZoneInformation.StandardName, 
                     SetTestTimeZoneInformation.DaylightName);
            }

            //
            // make sure GetTimeZoneInformation() works OK
            //

            dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
            dwLastError = GetLastError();
            if (TIME_ZONE_ID_INVALID == dwRetVal) {
                xLog(LogHandle,
                     XLL_FAIL,
                     "GetTimeZoneInformation returned TIME_ZONE_ID_UNKNOWN. GetLastError %lu",
                     dwLastError);
            } else {
                xLog(LogHandle,
                     XLL_PASS,
                     "GetTimeZoneInformation succeeded");
            }

            //
            // hack part 2
            // after setting the timezone, the folowing calls seem to normalize things
            //*****************************************************************
            // set time again
            // get us 1 second before Standard date starts
            //

            if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
                xLog(LogHandle,
                     XLL_FAIL,
                     "hack2:part2:XapiSetLocalTime failed setting test time info (DST->Standard)!");
            } else {
                xLog(LogHandle,
                     XLL_PASS,
                     "hack2:part2:XapiSetLocalTime set test time info (DST->Standard)");
            }
            dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
            GetLocalTime(&TestLocalTime);
            GetSystemTime(&TestSystemTime);

            //
            // end hack part 2
            //*****************************************************************
            //

            //
            // set / get / check 
            // for (this year, next year ... in 5 years)
            // yes, this is overkill, but cycles are free
            // 

            for (YearOffset = 0; YearOffset < 5; YearOffset++) {

                //
                // for (each side of DST, and across boundary)
                // DST has a start date and an end date (April 1, October 28)
                // in the US, it starts at 2AM, it starts at other times in other parts of the world
                // the cases we'll hit:
                //
                // set time 1 second before it begins - check time (this is also the pre-switch test)
                // check GetTimeZoneInformation()
                // sleep 2 seconds
                // check time, make sure it has moved forward - check time (this is also the post-switch test)
                //
                // set time 1 second before it ends - check time (this is also the pre-switch test)
                // check GetTimeZoneInformation()
                // sleep 2 seconds
                // check time, make sure it has moved back - check time (this is also the post-switch test)
                //
                // if it is not enabled (DSTFlag == 0), make sure it has not moved
                // if it is enabled (DSTFlag == 1), make sure it not moved
                //

                //
                // go from DST -> Standard
                // get us 1 second before Standard date starts
                //

                TestLocalTime.wYear         = CompareSTDdate[YearOffset].wYear;
                TestLocalTime.wMonth        = CompareSTDdate[YearOffset].wMonth;
                TestLocalTime.wDay          = CompareSTDdate[YearOffset].wDay;
                TestLocalTime.wHour         = SetTestTimeZoneInformation.StandardDate.wHour - 1;
                TestLocalTime.wMinute       = 59;
                TestLocalTime.wSecond       = 59;
                TestLocalTime.wMilliseconds = 0;

                //
                // set the local time, sleep 2 seconds
                // we are now in DST
                // we set local time 2 times since there is a known problem in XBox with 
                //

                if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
                    xLog(LogHandle,
                         XLL_FAIL,
                         "XapiSetLocalTime failed setting test time info (DST->Standard)!");
                } else {
                    xLog(LogHandle,
                         XLL_PASS,
                         "XapiSetLocalTime set test time info (DST->Standard)");
                }

                //
                // check the timezone information
                //

                dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
                dwLastError = GetLastError();

                switch (dwRetVal) {
                    case TIME_ZONE_ID_INVALID:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (DST->Standard). GetLastError %lu",
                             dwLastError);
                        break;

                        // what should happen if there is no DST enabled?
                    case TIME_ZONE_ID_UNKNOWN:
                        if (DSTFlag == 0) { // no DST
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==0)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (DST->Standard:DSTFlag==1)");
                        }
                        break;

                    case TIME_ZONE_ID_DAYLIGHT:
                        if (DSTFlag == 1) { // DST enabled
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==1)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (DST->Standard:DSTFlag==0)");
                        }
                        break;

                    default:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation - wrong result - default case hit (DST->Standard)! GetLastError %lu : dwRetVal = %lu", 
                             dwLastError, dwRetVal);
                        break;
                }

                //
                // roll over
                //

                Sleep(2000);

                //
                // get the local time, we have crossed the line from DST to Standard
                // we are now in Standard
                //

                GetLocalTime(&TestLocalTime);
                GetSystemTime(&TestSystemTime);

                //
                // check the timezone information
                //

                dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
                dwLastError = GetLastError();
                switch (dwRetVal) {
                    case TIME_ZONE_ID_INVALID:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (DST->Standard). GetLastError %lu",
                             dwLastError);
                        break;

                        // what should happen if there is no DST enabled?
                    case TIME_ZONE_ID_UNKNOWN:
                        if (DSTFlag == 0) { // no DST
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==0)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (DST->Standard:DSTFlag==1)");
                        }
                        break;

                    case TIME_ZONE_ID_STANDARD:
                        if (DSTFlag == 1) { // DST enabled
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (DST->Standard:DSTFlag==1)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (DST->Standard:DSTFlag==0)");
                        }
                        break;

                    default:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation - wrong result - default case hit! (DST->Standard)  GetLastError %lu : dwRetVal = %lu", 
                             dwLastError, dwRetVal);
                        break;
                }

                if (DSTFlag == 0) {

                    //
                    // should be 2 seconds forward, no bias
                    //

                    if ((TestLocalTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
                        (TestLocalTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
                        (TestLocalTime.wDay    == CompareSTDdate[YearOffset].wDay) &&
                        (TestLocalTime.wHour   == CompareSTDdate[YearOffset].wHour) &&
                        (TestLocalTime.wMinute == 0) &&
                        (TestLocalTime.wSecond == 1)) {
                        xLog(LogHandle,
                             XLL_PASS,
                             "TestLocalTime correct (DST->Standard:DSTFlag==0)");
                    } else {
                        xLog(LogHandle,
                             XLL_FAIL,
                             "TestLocalTime incorrect (DST->Standard:DSTFlag==0)");
                        DumpSystemTime(TestLocalTime, L"TestLocalTime");
                        DumpTimeZone(SetTestTimeZoneInformation);
                    }

                } else {

                    //
                    // should be 2 seconds forward, DST bias applied (standard to DST - fall back!)
                    //

                    if ((TestLocalTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
                        (TestLocalTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
                        (TestLocalTime.wDay    == CompareSTDdate[YearOffset].wDay) &&
                        ((TestLocalTime.wHour  == CompareSTDdate[YearOffset].wHour) + (SetTestTimeZoneInformation.DaylightBias / 60)) &&
                        (TestLocalTime.wMinute == 0) &&
                        (TestLocalTime.wSecond == 1)) {
                        xLog(LogHandle,
                             XLL_PASS,
                             "TestLocalTime correct (DST->Standard:DSTFlag==1)");
                    } else {
                        xLog(LogHandle,
                             XLL_FAIL,
                             "TestLocalTime incorrect (DST->Standard:DSTFlag==1)");
                        DumpSystemTime(TestLocalTime, L"TestLocalTime");
                        DumpTimeZone(SetTestTimeZoneInformation);
                    }
                }

                //
                // check the system time, we have crossed the line - system time should not change
                // rollover check - as we go to different timezones, the bias will roll us over
                // in this case, we apply the daylight bias
                //

                DayAdjust = 0;
                HourAdjust = TotalBias = (SetTestTimeZoneInformation.Bias + (DSTFlag ? (SetTestTimeZoneInformation.DaylightBias) : 0)) / 60;
                if (ROLL_HOUR + TotalBias > 23) {
                    DayAdjust = 1;
                    HourAdjust = TotalBias - 24;
                } else if (ROLL_HOUR + TotalBias < 0) {
                    DayAdjust = -1;
                    HourAdjust = 24 + TotalBias;
                }

                if ((TestSystemTime.wYear   == CompareSTDdate[YearOffset].wYear) &&
                    (TestSystemTime.wMonth  == CompareSTDdate[YearOffset].wMonth) &&
                    // adjust when bias rolls over
                    (TestSystemTime.wDay    == CompareSTDdate[YearOffset].wDay + DayAdjust) &&
                    // adjust when bias rolls over
                    (TestSystemTime.wHour   == CompareSTDdate[YearOffset].wHour + HourAdjust) &&
                    (TestSystemTime.wMinute == 0) &&
                    (TestSystemTime.wSecond == 1)) {
                    xLog(LogHandle,
                         XLL_PASS,
                         "TestSystemTime correct (DST->Standard)");
                } else {
                    xLog(LogHandle,
                         XLL_FAIL,
                         "TestSystemTime incorrect (DST->Standard)");
                    DumpSystemTime(TestSystemTime, L"TestSystemTime");
                    DumpTimeZone(SetTestTimeZoneInformation);
                }

                //*************************************************************
                // now try Standard -> DST
                //*************************************************************

                //
                // get us 1 second before DST starts
                //

                TestLocalTime.wYear      = CompareDSTdate[YearOffset].wYear; 
                TestLocalTime.wMonth     = CompareDSTdate[YearOffset].wMonth;
                TestLocalTime.wDay       = CompareDSTdate[YearOffset].wDay;  
                TestLocalTime.wHour      = SetTestTimeZoneInformation.DaylightDate.wHour - 1;
                TestLocalTime.wMinute    = 59;
                TestLocalTime.wSecond    = 59;
                TestLocalTime.wMilliseconds = 0;

                //
                // set the local time, sleep 2 seconds
                // we are now in Standard time
                //

                if (FALSE == XapiSetLocalTime(&TestLocalTime)) {
                    xLog(LogHandle,
                         XLL_FAIL,
                         "XapiSetLocalTime failed setting test time info (Standard->DST)!");
                } else {
                    xLog(LogHandle,
                         XLL_PASS,
                         "XapiSetLocalTime set test time info (Standard->DST)");
                }

                //
                // check the timezone information
                //

                dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
                dwLastError = GetLastError();

                switch (dwRetVal) {
                    case TIME_ZONE_ID_INVALID:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (Standard->DST). GetLastError %lu",
                             dwLastError);
                        break;

                        // what should happen if there is no DST enabled?
                    case TIME_ZONE_ID_UNKNOWN:
                        if (DSTFlag == 0) { // no DST
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==0)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (Standard->DST:DSTFlag==1)");
                        }
                        break;

                    case TIME_ZONE_ID_STANDARD:
                        if (DSTFlag == 1) { // DST enabled
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==1)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (Standard->DST:DSTFlag==0)");
                        }
                        break;

                    default:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation - wrong result - default case hit (Standard->DST)!  GetLastError %lu : dwRetVal = %lu", 
                             dwLastError, dwRetVal);
                        break;
                }

                //
                // roll over
                //

                Sleep(2000);

                //
                // get the local time, we have crossed the line from Standard to DST
                // we are now in DST
                //

                GetLocalTime(&TestLocalTime);
                GetSystemTime(&TestSystemTime);

                //
                // check the timezone information
                //

                dwRetVal = GetTimeZoneInformation(&GetTestTimeZoneInformation);
                dwLastError = GetLastError();
                switch (dwRetVal) {
                    case TIME_ZONE_ID_INVALID:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation returned TIME_ZONE_ID_INVALID (Standard->DST). GetLastError %lu",
                             dwLastError);
                        break;

                        // what should happen if there is no DST enabled?
                    case TIME_ZONE_ID_UNKNOWN:
                        if (DSTFlag == 0) { // no DST
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==0)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_STANDARD in this case (Standard->DST:DSTFlag==1)");
                        }
                        break;

                    case TIME_ZONE_ID_DAYLIGHT:
                        if (DSTFlag == 1) { // DST enabled
                            xLog(LogHandle,
                                 XLL_PASS,
                                 "GetTimeZoneInformation succeeded (Standard->DST:DSTFlag==1)");
                        } else {
                            xLog(LogHandle,
                                 XLL_FAIL,
                                 "GetTimeZoneInformation - wrong result - expected TIME_ZONE_ID_UNKNOWN in this case (Standard->DST:DSTFlag==0)");
                        }
                        break;

                    default:
                        xLog(LogHandle,
                             XLL_FAIL,
                             "GetTimeZoneInformation - wrong result - default case hit! (Standard->DST).  GetLastError %lu : dwRetVal = %lu", 
                             dwLastError, dwRetVal);
                        break;
                }

                if (DSTFlag == 0) {

                    //
                    // should be 2 seconds forward, no bias
                    //

                    if ((TestLocalTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
                        (TestLocalTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
                        (TestLocalTime.wDay    == CompareDSTdate[YearOffset].wDay) &&
                        (TestLocalTime.wHour   == CompareDSTdate[YearOffset].wHour) &&
                        (TestLocalTime.wMinute == 0) &&
                        (TestLocalTime.wSecond == 1)) {
                        xLog(LogHandle,
                             XLL_PASS,
                             "TestLocalTime correct (Standard->DST:DSTFlag==0)");
                    } else {
                        xLog(LogHandle,
                             XLL_FAIL,
                             "TestLocalTime incorrect (Standard->DST:DSTFlag==0)");
                        DumpSystemTime(TestLocalTime, L"TestLocalTime");
                        DumpTimeZone(SetTestTimeZoneInformation);
                    }

                } else {

                    //
                    // should be 2 seconds forward, DST bias applied (standard to DST - spring forward!)
                    //

                    if ((TestLocalTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
                        (TestLocalTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
                        (TestLocalTime.wDay    == CompareDSTdate[YearOffset].wDay) &&
                        ((TestLocalTime.wHour  == CompareDSTdate[YearOffset].wHour) + (SetTestTimeZoneInformation.DaylightBias / 60)) &&
                        (TestLocalTime.wMinute == 0) &&
                        (TestLocalTime.wSecond == 1)) {
                        xLog(LogHandle,
                             XLL_PASS,
                             "TestLocalTime correct (Standard->DST:DSTFlag==1)");
                    } else {
                        xLog(LogHandle,
                             XLL_FAIL,
                             "TestLocalTime incorrect (Standard->DST:DSTFlag==1)");
                        DumpSystemTime(TestLocalTime, L"TestLocalTime");
                        DumpTimeZone(SetTestTimeZoneInformation);
                    }
                }

                //
                // check the system time, we have crossed the line - system time should not change
                // rollover check - as we go to different timezones, the bias will roll us over
                //

                DayAdjust = 0;
                HourAdjust = TotalBias = SetTestTimeZoneInformation.Bias / 60;
                if (ROLL_HOUR + TotalBias > 23) {
                    DayAdjust = 1;
                    HourAdjust = TotalBias - 24;
                } else if (ROLL_HOUR + TotalBias < 0) {
                    DayAdjust = -1;
                    HourAdjust = 24 + TotalBias;
                }


                if ((TestSystemTime.wYear   == CompareDSTdate[YearOffset].wYear) &&
                    (TestSystemTime.wMonth  == CompareDSTdate[YearOffset].wMonth) &&
                    // adjust when bias rolls over
                    (TestSystemTime.wDay    == CompareDSTdate[YearOffset].wDay + DayAdjust) &&
                    // adjust when bias rolls over
                    (TestSystemTime.wHour   == CompareDSTdate[YearOffset].wHour + HourAdjust) &&
                    (TestSystemTime.wMinute == 0) &&
                    (TestSystemTime.wSecond == 1)) {
                    xLog(LogHandle,
                         XLL_PASS,
                         "TestSystemTime correct (Standard->DST)");
                } else {
                    xLog(LogHandle,
                         XLL_FAIL,
                         "TestSystemTime incorrect (Standard->DST)");
                    DumpSystemTime(TestSystemTime, L"TestSystemTime");
                    DumpTimeZone(SetTestTimeZoneInformation);
                }

            }
        }
    }

    //
    // other interesting cases
    // GetTimeZoneInformation()
    // returns TIME_ZONE_ID_UNKNOWN The system cannot determine the current time zone. 
    // This error is also returned if you call the SetTimeZoneInformation function and 
    // supply the bias values but no transition dates.  
    //
    // check to make sure correct day of week shows up in GetLocalTime() GetSystemTime()
    //

    //
    // restore surrent timezone
    // restore current time
    //

    if (ERROR_SUCCESS == XQueryValue(XC_MISC_FLAGS, &type, &flags, sizeof(flags), &size)) {
        if (bUsingDST) {
            flags &= ~XC_MISC_FLAG_DONT_USE_DST;
        } else {
            flags |= XC_MISC_FLAG_DONT_USE_DST;
        }

        XSetValue(XC_MISC_FLAGS, REG_DWORD, &flags, sizeof(flags));
    }

    if (!NT_SUCCESS(XapipSetTimeZoneInformation( &SaveTimeZoneInformation ))) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapipSetTimeZoneInformation failed restoring saved time zone info!");
        DumpSystemTime(SaveTimeZoneInformation.DaylightDate, L"SaveTimeZoneInformation.DaylightDate:" );
        DumpSystemTime(SaveTimeZoneInformation.StandardDate, L"SaveTimeZoneInformation.StandardDate:" );
    }

    if (FALSE == XapiSetLocalTime( &SaveLocalTime )) {
        xLog(LogHandle,
             XLL_FAIL,
             "XapiSetLocalTime failed restoring saved time info!");
    }

    xEndVariation( LogHandle );
    return;
}

BOOL
WINAPI
EqualTime(
    HANDLE LogHandle,
    LPSYSTEMTIME pst1, 
    LPSYSTEMTIME pst2
    )
/*++

Routine Description:

    This function receives addresses of two LPSYSTEMTIME structures.
    It returns TRUE if the times are equal else it returns FALSE.

Arguments:

    LogHandle - Handle used to call logging APIs
    LPSYSTEMTIME pst1
    LPSYSTEMTIME pst2
    
Return Value:

    None

Notes:
    
    Stolen mostly from RATS akrnutil.c

--*/ 
{
    if ( pst1->wYear == pst2->wYear     &&
         pst1->wMinute == pst2->wMinute &&
         pst1->wMonth == pst2->wMonth    &&
         pst1->wDay == pst2->wDay        &&
         pst1->wHour == pst2->wHour     ) {
        xLog(LogHandle,
             XLL_PASS,
             "EqualTime: The times are as expected.");
        return (TRUE);
    }

    xLog(LogHandle,
         XLL_FAIL,
         "EqualTime(pst1): wYear %u wMonth %u Hour %u Minute %u DayOfWeek %u wDay %u",
         pst1->wYear, pst1->wMonth,
         pst1->wHour, pst1->wMinute,
         pst1->wDayOfWeek, pst1->wDay);

    xLog(LogHandle,
         XLL_FAIL,
         "EqualTime(pst2): wYear %u wMonth %u Hour %u Minute %u DayOfWeek %u wDay %u",
         pst2->wYear, pst2->wMonth,
         pst2->wHour, pst2->wMinute,
         pst2->wDayOfWeek, pst2->wDay);

    return (FALSE);
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( timetest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( timetest )
    EXPORT_TABLE_ENTRY( "StartTest", TimeTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", TimeTestEndTest )
END_EXPORT_TABLE( timetest )
