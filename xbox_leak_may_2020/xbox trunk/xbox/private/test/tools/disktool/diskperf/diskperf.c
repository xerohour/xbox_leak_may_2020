/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    diskperf.c

Abstract:

    disk perf for xbox

written by:

    John Daly (johndaly) 13-july-2000

Environment:

    XBox

Revision History:


Notes:

    yes, I know I could make this state or table driven, but it
    will be easier to maintain like this and easier to tweak / special case / debug as well

    using FscInvalidateIdleBlocks() to have the memory manager discard
    clean system cache pages.  Test wants this to get the cache back to a clean slate.  
    It's the responsibility of the caller to flush any volumes in order to maximum the 
    clean page count.
    
    some thing may be hard to port to NT, such as limiting / blowing away cache


--*/

#include "diskperf.h"

VOID
WINAPI
DiskPerfStartTest(
    HANDLE xLogHandle
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
    LogHandle = xLogHandle;

    xSetComponent( LogHandle, "kernel", "diskperf" );
    xSetFunctionName(LogHandle, "StartTest" );

    //
    // set up the timer
    //

    if (FALSE == QueryPerformanceFrequency(&Frequency)) {
        ;// log and exit
    }

    //
    // run the performance variations
    //



    //
    // test the timer
    // it seems to be pretty close, we'll use it for now
    //
    //for (x=0;x<1000;x++) {
    //    // start timer
    //    StartTimer();
    //    Sleep(x*2);
    //    Time = EndTimer();
    //    // log time
    //    xLog(LogHandle, 
    //    XLL_PASS, 
    //    "Time: %10.10u ms",
    //    Time);
    //}


    run_var1(); // ok
    run_var2(); // ok
    run_var3(); // ok


    // run_var4();  stupid, do not code

    run_var5(); // ok
    run_var6(); // ok
    run_var7(); // ok
//    run_var8(); // just use 7 to cover all media
    run_var9(); // ok
    run_var10(); // ok
    run_var11(); //ok
    run_var12();
    //run_var13(); // ignore
    //run_var14(); // ignore
    //run_var15(); // ignore
    //run_var16(); // ignore
    //run_var17(); // ignore

    return;
}

DWORD
WINAPI
SizeCache(
    DWORD Size
    )
/*++

Routine Description:

    Allocate enough memory to prevent the disk cache from growing 
    above the specified size
    
Arguments:

    Size - size in bytes you want the cache limited to

Return Value:

    Size of cache
    
Notes:

    without an elegant way to do this, eat memory
    
    other memory allocations can affect this
    
    free the buffer like this when finished with it
    VirtualFree(<whatever this function returned>, 0, MEM_RELEASE); 
    
    the size you get will be within ~200k, don't try passing a size less than 1 mb

--*/ 
{
    //
    // the old way before we had cache size control
    //
    //
    //MM_STATISTICS mmStats;
    //LPVOID VMem;
    //DWORD SizeInPages = Size / PAGE_SIZE;
    //
    //ZeroMemory(&mmStats, sizeof(mmStats));
    //mmStats.Length = sizeof(mmStats);
    //
    //MmQueryStatistics(&mmStats);
    //
    //VMem = VirtualAlloc(NULL, 
    //                    (((mmStats.AvailablePages * PAGE_SIZE) + 
    //                      (mmStats.CachePagesCommitted * PAGE_SIZE)) - 
    //                     Size),
    //                    MEM_COMMIT,
    //                    PAGE_READWRITE);
    //
    //ZeroMemory(&mmStats, sizeof(mmStats));
    //mmStats.Length = sizeof(mmStats);
    //MmQueryStatistics(&mmStats);
    //

    //
    // Note : cache is currently limited to 8 mb, so after that point we can't grow the cache
    // 
    //

    XSetFileCacheSize( Size );
    return(XGetFileCacheSize());
}

VOID
WINAPI
DiskPerfEndTest(
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
    OutputDebugString( TEXT("diskperf: EndTest is called\n") );
}

VOID
WINAPI
run_var1(
        VOID
        )
/*++

Routine Description:

    all writes (all sizes and flags)

Arguments:

    None

Return Value:

    None
    
Notes:

    the overlapped variation is pretty conservative, it waits for 
    each write, it could be rewritten to queue more writes and then wait
    or to use WriteFileEx and have a completion routine, and when the
    total bytes written is achieved then we know we are done

--*/ 
{
#define TestBufferSize 0x10000
    DWORD SizeArray[] = {128, 256, 512, 0x1000, 0x10000, 0x100000, 0x1000000, 0x10000000,  0x2E000000/*0xFFFF0000*/};
    DWORD FlagSets[] = {FILE_ATTRIBUTE_NORMAL,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_WRITE_THROUGH,
        FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_SEQUENTIAL_SCAN};
    DWORD x, y, z;
    HANDLE FileHandle;
    DWORD Time;
    PVOID TestData;
    DWORD BytesWritten;
    DWORD BytesWrittenX;
    OVERLAPPED OverLapped;
    OVERLAPPED *pOverlapped;
    BOOL bResult;
    DWORD Error;

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    CleanCaches();

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize);
    if (NULL == TestData) {
        Error = GetLastError();
        DebugBreak();
    }

    xStartVariation( LogHandle, "variation1" );
    xLog(LogHandle, XLL_INFO, "all writes (all sizes and flags)");

    DeleteFile(OTEXT("Z:\\TestFile.prf"));

    //
    // for each flag set....
    //

    for (y = 0; y < ARRAYSIZE(FlagSets); y++) {

        //
        // and each test file size...
        //

        for (x = 0; x < ARRAYSIZE(SizeArray); x++) {

            //
            // clean out caches, reset BytesWrittenX count
            //

            CleanCaches();

            BytesWrittenX = 0;

            //
            // we have to block certain writes, since they don't conform to 
            // limitations imposed by the OS
            //

            if ((FlagSets[y] & FILE_FLAG_NO_BUFFERING) && 
                (SizeArray[x] % 512)) {
                continue;
            }

            //
            // create file
            //

            FileHandle = CreateFile(OTEXT("Z:\\TestFile.prf"),
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FlagSets[y],
                                    NULL);

            if (INVALID_HANDLE_VALUE == FileHandle) {
                xLog(LogHandle, XLL_BLOCK, "CreateFile Failed! (LastError = %d)", GetLastError());
                continue;
            }

            //
            // we have to point to an OVERLAPPED struct when appropriate
            // associate Event with struct (this is optional I believe)
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped = &OverLapped;
                ZeroMemory(pOverlapped, sizeof(*pOverlapped));
                if (NULL == (OverLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                    Error = GetLastError();
                    _asm int 3
                }

            } else {
                pOverlapped = NULL;
            }

            //
            // start timer
            //

            StartTimer();

            //
            // write to it
            //

            if (SizeArray[x] > TestBufferSize) {

                for (z = SizeArray[x] / TestBufferSize; z > 0; z--) {

                    bResult = WriteFile(FileHandle, 
                                        TestData,
                                        TestBufferSize,
                                        &BytesWritten,
                                        pOverlapped);

                    //
                    // if we need to wait on the event during this multi-pass write, do it here
                    //

                    if (FALSE == bResult) {

                        if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                            (ERROR_IO_PENDING == GetLastError())) {

                            bResult = GetOverlappedResult(FileHandle,
                                                          pOverlapped,
                                                          &BytesWritten,
                                                          TRUE);
                            if (FALSE == bResult) {
                                Error = GetLastError();
                                DebugBreak();
                            }

                        } else {

                            //
                            // some other error, like disk full..
                            //

                            Error = GetLastError();
                            DebugBreak();
                        }
                    }

                    //
                    // advance pointer for next write
                    //

                    if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                        pOverlapped->Offset += BytesWritten;
                    }

                    BytesWrittenX += BytesWritten;
                }

            } else {

                bResult = WriteFile(FileHandle, 
                                    TestData,
                                    SizeArray[x],
                                    &BytesWrittenX,
                                    pOverlapped);

                //
                // if we need to wait on the event during this write, do it here
                //

                //
                // if we need to wait on the event during this multi-pass write, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesWritten,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }
            }

            //
            // stop timer
            //

            Time = EndTimer();

            //
            // cleanup
            //

            CloseHandle(FileHandle);

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                CloseHandle(OverLapped.hEvent);
            }

            //
            // Clean up the file
            //

            bResult = DeleteFile(OTEXT("Z:\\TestFile.prf"));

            //
            // log time
            //

            xLog(LogHandle, 
                 XLL_PASS, 
                 "Flags: 0x%8.8X Bytes Written: %10.10u Time: %10.10u ms",
                 FlagSets[y], BytesWrittenX, Time);
        }
    }

    //
    // free test data buffer
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var2(
    VOID
    )
/*++

Routine Description:

    all reads  (all sizes and flags)

Arguments:

    None

Return Value:

    None
    
Notes:

    it may seem redundant to create files using the exact same technique used 
    in the preceding test, but it makes writing all this code go faster

--*/ 
{
#define TestBufferSize 0x10000
    DWORD SizeArray[] = {128, 256, 512, 0x1000, 0x10000, 0x100000, 0x1000000, 0x10000000, 0x2E000000/*0xFFFF0000*/};
    DWORD FlagSets[] = {FILE_ATTRIBUTE_NORMAL,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_WRITE_THROUGH,
        FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_SEQUENTIAL_SCAN};
    DWORD x, y, z;
    HANDLE FileHandle;
    DWORD Time;
    PVOID TestData;
    PVOID TestDataIn;
    DWORD BytesWritten;
    DWORD BytesWrittenX;
    DWORD BytesRead;
    DWORD BytesReadX;
    OVERLAPPED OverLapped;
    OVERLAPPED *pOverlapped;
    BOOL bResult;
    DWORD Error;

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize);
    TestDataIn = VirtualAlloc(NULL, TestBufferSize, MEM_COMMIT, PAGE_READWRITE);
    if ((NULL == TestDataIn) || (NULL == TestData)) {
        Error = GetLastError();
        DebugBreak();
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    CleanCaches();
    
    xStartVariation( LogHandle, "variation2" );
    xLog(LogHandle, XLL_INFO, "all reads  (all sizes and flags)");

    DeleteFile(OTEXT("Z:\\TestFile.prf"));

    //
    // for each flag set....
    //

    for (y = 0; y < ARRAYSIZE(FlagSets); y++) {

        //
        // and each test file size...
        //

        for (x = 0; x < ARRAYSIZE(SizeArray); x++) {

            BytesWrittenX = 0;

            //
            // create file and file data prior to read
            // also, be sure to clear all caches before the reading begins
            //

            //
            // we have to block certain writes, since they don't conform to 
            // limitations imposed by the OS
            //

            if ((FlagSets[y] & FILE_FLAG_NO_BUFFERING) && 
                (SizeArray[x] % 512)) {
                continue;
            }

            FileHandle = CreateFile(OTEXT("Z:\\TestFile.prf"),
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FlagSets[y],
                                    NULL);

            if (INVALID_HANDLE_VALUE == FileHandle) {
                xLog(LogHandle, XLL_BLOCK, "CreateFile Failed! (LastError = %d)", GetLastError());
                continue;
            }

            //
            // we have to point to an OVERLAPPED struct when appropriate
            // associate Event with struct (this is optional I believe)
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped = &OverLapped;
                ZeroMemory(pOverlapped, sizeof(*pOverlapped));
                if (NULL == (OverLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                    Error = GetLastError();
                    _asm int 3
                }
            } else {
                pOverlapped = NULL;
            }

            //
            // create the file data to be read
            // Fill it in
            //

            if (SizeArray[x] > TestBufferSize) {

                for (z = SizeArray[x] / TestBufferSize; z > 0; z--) {

                    bResult = WriteFile(FileHandle, 
                                        TestData,
                                        TestBufferSize,
                                        &BytesWritten,
                                        pOverlapped);

                    //
                    // if we need to wait on the event during this multi-pass write, do it here
                    //

                    if (FALSE == bResult) {

                        if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                            (ERROR_IO_PENDING == GetLastError())) {

                            bResult = GetOverlappedResult(FileHandle,
                                                          pOverlapped,
                                                          &BytesWritten,
                                                          TRUE);
                            if (FALSE == bResult) {
                                Error = GetLastError();
                                DebugBreak();
                            }

                        } else {

                            //
                            // some other error, like disk full..
                            //

                            Error = GetLastError();
                            DebugBreak();
                        }
                    }

                    //
                    // advance pointer for next write
                    //

                    if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                        pOverlapped->Offset += BytesWritten;
                    }

                    BytesWrittenX += BytesWritten;
                }
            } else {

                bResult = WriteFile(FileHandle, 
                                    TestData,
                                    SizeArray[x],
                                    &BytesWrittenX,
                                    pOverlapped);

                //
                // if we need to wait on the event during this write, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesWritten,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }
            }

            //
            // reset overlapped IO stuff
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                CloseHandle(OverLapped.hEvent);
                pOverlapped = &OverLapped;
                ZeroMemory(pOverlapped, sizeof(*pOverlapped));
                if (NULL == (OverLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                    Error = GetLastError();
                    _asm int 3
                }
            } else {
                pOverlapped = NULL;
            }

            //
            // Set the file pointer back to the beginning
            //

            SetFilePointer(FileHandle, 0L, NULL, FILE_BEGIN);

            //
            // clean out caches, reset BytesReadX counter
            //

            CleanCaches();
            BytesReadX = 0;

            //
            // start timer
            //

            StartTimer();

            //
            // read file data
            //

            if (SizeArray[x] > TestBufferSize) {

                for (z = SizeArray[x] / TestBufferSize; z > 0; z--) {

                    bResult = ReadFile(FileHandle, 
                                       TestDataIn,
                                       TestBufferSize,
                                       &BytesRead,
                                       pOverlapped);

                    //
                    // if we need to wait on the event during this multi-pass read, do it here
                    //

                    if (FALSE == bResult) {

                        if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                            (ERROR_IO_PENDING == GetLastError())) {

                            bResult = GetOverlappedResult(FileHandle,
                                                          pOverlapped,
                                                          &BytesRead,
                                                          TRUE);
                            if (FALSE == bResult) {
                                Error = GetLastError();
                                DebugBreak();
                            }

                        } else {

                            //
                            // some other error, like disk full..
                            //

                            Error = GetLastError();
                            DebugBreak();
                        }
                    }

                    //
                    // advance pointer for next read
                    //

                    if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                        pOverlapped->Offset += BytesRead;
                    }

                    BytesReadX += BytesRead;
                }
            } else {

                bResult = ReadFile(FileHandle, 
                                   TestDataIn,
                                   SizeArray[x],
                                   &BytesReadX,
                                   pOverlapped);

                //
                // if we need to wait on the event during this multi-pass read, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesReadX,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }
            }

            //
            // stop timer
            //

            Time = EndTimer();

            //
            // cleanup
            //

            CloseHandle(FileHandle);

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                CloseHandle(OverLapped.hEvent);
            }

            //
            // clean up the file
            //

            bResult = DeleteFile(OTEXT("Z:\\TestFile.prf"));

            //
            // log time
            //

            xLog(LogHandle, 
                 XLL_PASS, 
                 "Flags: 0x%8.8X Bytes Read: %10.10u Time: %10.10u ms",
                 FlagSets[y], BytesReadX, Time);
        }
    }

    //
    // free test data buffer
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 
    VirtualFree(TestDataIn, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var3(
    VOID
    )
/*++

Routine Description:

    writes then reads (all sizes and flags)

Arguments:

    None

Return Value:

    None
    
Notes: 
    
    this is the same as run_var2, but we don't flush the cache
    think of it as write-and-read-back
    re-use the overlapped stuff, file handle, etc.


--*/ 
{
#define TestBufferSize 0x10000
    DWORD SizeArray[] = {128, 256, 512, 0x1000, 0x10000, 0x100000, 0x1000000, 0x10000000,  0x2E000000/*0xFFFF0000*/};
    DWORD FlagSets[] = {FILE_ATTRIBUTE_NORMAL,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_WRITE_THROUGH,
        FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_SEQUENTIAL_SCAN};
    DWORD x, y, z;
    HANDLE FileHandle;
    DWORD Time;
    PVOID TestData;
    PVOID TestDataIn;
    DWORD BytesWritten;
    DWORD BytesWrittenX;
    DWORD BytesRead;
    DWORD BytesReadX;
    OVERLAPPED OverLapped;
    OVERLAPPED *pOverlapped;
    BOOL bResult;
    DWORD Error;

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize);
    TestDataIn = VirtualAlloc(NULL, TestBufferSize, MEM_COMMIT, PAGE_READWRITE);
    if ((NULL == TestDataIn) || (NULL == TestData)) {
        Error = GetLastError();
        DebugBreak();
    }

    xStartVariation( LogHandle, "variation3" );
    xLog(LogHandle, XLL_INFO, "writes then reads (all sizes and flags)");

    DeleteFile(OTEXT("Z:\\TestFile.prf"));

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    
    //
    // for each flag set....
    //

    for (y = 0; y < ARRAYSIZE(FlagSets); y++) {

        //
        // and each test file size...
        //

        for (x = 0; x < ARRAYSIZE(SizeArray); x++) {

            BytesWrittenX = 0;

            //
            // create file and file data prior to read
            // also, be sure to clear all caches before the reading begins
            //

            CleanCaches();

            //
            // start timer
            //

            StartTimer();

            //
            // we have to block certain writes, since they don't conform to 
            // limitations imposed by the OS
            //

            if ((FlagSets[y] & FILE_FLAG_NO_BUFFERING) && 
                (SizeArray[x] % 512)) {
                continue;
            }

            FileHandle = CreateFile(OTEXT("Z:\\TestFile.prf"),
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FlagSets[y],
                                    NULL);

            if (INVALID_HANDLE_VALUE == FileHandle) {
                xLog(LogHandle, XLL_BLOCK, "CreateFile Failed! (LastError = %d)", GetLastError());
                continue;
            }

            //
            // we have to point to an OVERLAPPED struct when appropriate
            // associate Event with struct (this is optional I believe)
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped = &OverLapped;
                ZeroMemory(pOverlapped, sizeof(*pOverlapped));
                if (NULL == (OverLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                    Error = GetLastError();
                    _asm int 3
                }
            } else {
                pOverlapped = NULL;
            }

            //
            // create the file data to be read
            // Fill it in
            //

            if (SizeArray[x] > TestBufferSize) {

                for (z = SizeArray[x] / TestBufferSize; z > 0; z--) {

                    bResult = WriteFile(FileHandle, 
                                        TestData,
                                        TestBufferSize,
                                        &BytesWritten,
                                        pOverlapped);

                    //
                    // if we need to wait on the event during this multi-pass write, do it here
                    //

                    if (FALSE == bResult) {

                        if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                            (ERROR_IO_PENDING == GetLastError())) {

                            bResult = GetOverlappedResult(FileHandle,
                                                          pOverlapped,
                                                          &BytesWritten,
                                                          TRUE);
                            if (FALSE == bResult) {
                                Error = GetLastError();
                                DebugBreak();
                            }

                        } else {

                            //
                            // some other error, like disk full..
                            //

                            Error = GetLastError();
                            DebugBreak();
                        }
                    }

                    //
                    // advance pointer for next write
                    //

                    if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                        pOverlapped->Offset += BytesWritten;
                    }

                    BytesWrittenX += BytesWritten;
                }
            } else {

                bResult = WriteFile(FileHandle, 
                                    TestData,
                                    SizeArray[x],
                                    &BytesWrittenX,
                                    pOverlapped);

                //
                // if we need to wait on the event during this write, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesWritten,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }
            }

            //
            // Set the file pointer back to the beginning
            // 'reset' overlapped
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped->Internal = 0;
                pOverlapped->InternalHigh = 0;
                pOverlapped->Offset = 0;
                pOverlapped->OffsetHigh = 0;
            } else {
                SetFilePointer(FileHandle, 0L, NULL, FILE_BEGIN);
            }

            //
            // reset BytesReadX counter
            //

            BytesReadX = 0;

            //
            // read file data
            //

            if (SizeArray[x] > TestBufferSize) {

                for (z = SizeArray[x] / TestBufferSize; z > 0; z--) {

                    bResult = ReadFile(FileHandle, 
                                       TestDataIn,
                                       TestBufferSize,
                                       &BytesRead,
                                       pOverlapped);

                    //
                    // if we need to wait on the event during this multi-pass read, do it here
                    //

                    if (FALSE == bResult) {

                        if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                            (ERROR_IO_PENDING == GetLastError())) {

                            bResult = GetOverlappedResult(FileHandle,
                                                          pOverlapped,
                                                          &BytesRead,
                                                          TRUE);
                            if (FALSE == bResult) {
                                Error = GetLastError();
                                DebugBreak();
                            }

                        } else {

                            //
                            // some other error, like disk full..
                            //

                            Error = GetLastError();
                            DebugBreak();
                        }
                    }

                    //
                    // advance pointer for next read
                    //

                    if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                        pOverlapped->Offset += BytesRead;
                    }

                    BytesReadX += BytesRead;
                }
            } else {

                bResult = ReadFile(FileHandle, 
                                   TestDataIn,
                                   SizeArray[x],
                                   &BytesReadX,
                                   pOverlapped);

                //
                // if we need to wait on the event during this multi-pass read, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesReadX,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }
            }

            //
            // stop timer
            //

            Time = EndTimer();

            //
            // cleanup
            //

            CloseHandle(FileHandle);

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                CloseHandle(OverLapped.hEvent);
            }

            //
            // clean up the file
            //

            bResult = DeleteFile(OTEXT("Z:\\TestFile.prf"));

            //
            // log time
            //

            xLog(LogHandle, 
                 XLL_PASS, 
                 "Flags: 0x%8.8X Bytes Written: %10.10u Bytes Read: %10.10u Time: %10.10u ms",
                 FlagSets[y], BytesWrittenX, BytesReadX, Time);
        }
    }

    //
    // free test data buffer
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 
    VirtualFree(TestDataIn, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var4(
    VOID
    )
/*++

Routine Description:

    writes, then flush, then reads (all sizes and flags)

Arguments:

    None

Return Value:

    None
    
Note: 
    
    this is the same as run_var2 so we'll skip it
    just add the times for run_var1 and run_var2

--*/ 
{

}

VOID
WINAPI
run_var5(
    VOID
    )
/*++

Routine Description:

    writes then reads, using a growing file (1-750mb, 10mb steps)

Arguments:

    None

Return Value:

    None
    
Notes:

    Similar to run_var2, but the file is handled differently. 
    behaves somewhat like a page file
    extend / read

--*/ 
{
#define TestBufferSize5 (0x10000 * 20)
    DWORD FlagSets[] = {FILE_ATTRIBUTE_NORMAL,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_WRITE_THROUGH,
        FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_SEQUENTIAL_SCAN};
    DWORD x, y, z;
    HANDLE FileHandle;
    DWORD Time;
    PVOID TestData;
    PVOID TestDataIn;
    DWORD BytesWrittenX;
    DWORD BytesRead;
    DWORD BytesReadX = 0;
    OVERLAPPED OverLapped;
    OVERLAPPED *pOverlapped;
    BOOL bResult;
    DWORD Error;

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize5);
    TestDataIn = VirtualAlloc(NULL, TestBufferSize5, MEM_COMMIT, PAGE_READWRITE);
    if ((NULL == TestDataIn) || (NULL == TestData)) {
        Error = GetLastError();
        DebugBreak();
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    
    xStartVariation( LogHandle, "variation5" );
    xLog(LogHandle, XLL_INFO, "writes then reads, using a growing file (1-1000mb, 1310720b steps)");

    //
    // for each flag set....
    //

    for (y = 0; y < ARRAYSIZE(FlagSets); y++) {

        CleanCaches();

        //
        // create file with specified flags
        //

        FileHandle = CreateFile(OTEXT("Z:\\TestFile.prf"),
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                CREATE_ALWAYS,
                                FlagSets[y],
                                NULL);

        if (INVALID_HANDLE_VALUE == FileHandle) {
            Error = GetLastError();
            _asm int 3
        }

        //
        // we have to point to an OVERLAPPED struct when appropriate
        // associate Event with struct (this is optional I believe)
        //

        if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
            pOverlapped = &OverLapped;
            ZeroMemory(pOverlapped, sizeof(*pOverlapped));
            if (NULL == (OverLapped.hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                Error = GetLastError();
                _asm int 3
            }
        } else {
            pOverlapped = NULL;
        }

        //
        // for each 10mb extension
        //

        for (x = 0; x < 75; x++) {


            //
            // start timer
            //

            StartTimer();

            //
            // append to file
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped->Offset = BytesReadX;
            } else {
                SetFilePointer(FileHandle, BytesReadX, NULL, FILE_BEGIN);
            }

            bResult = WriteFile(FileHandle, 
                                TestData,
                                TestBufferSize5,
                                &BytesWrittenX,
                                pOverlapped);

            //
            // if we need to wait on the event during this write, do it here
            //

            if (FALSE == bResult) {

                if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                    (ERROR_IO_PENDING == GetLastError())) {

                    bResult = GetOverlappedResult(FileHandle,
                                                  pOverlapped,
                                                  &BytesWrittenX,
                                                  TRUE);
                    if (FALSE == bResult) {
                        Error = GetLastError();
                        DebugBreak();
                    }

                } else {

                    //
                    // some other error, like disk full..
                    //

                    Error = GetLastError();
                    DebugBreak();
                }
            }

            //
            // read it all sequentially
            // Set the file pointer back to the beginning
            // 'reset' overlapped
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped->Offset = 0;
            } else {
                SetFilePointer(FileHandle, 0L, NULL, FILE_BEGIN);
            }

            //
            // reset BytesReadX counter
            //

            BytesReadX = 0;

            //
            // read file data
            // file pointer should be at end when done so append works
            //

            for (z = 0; z <= x; z++) {

                bResult = ReadFile(FileHandle, 
                                   TestDataIn,
                                   TestBufferSize5,
                                   &BytesRead,
                                   pOverlapped);

                //
                // if we need to wait on the event during this multi-pass read, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle,
                                                      pOverlapped,
                                                      &BytesRead,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }

                //
                // advance pointer for next read
                //

                if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                    pOverlapped->Offset += BytesRead;
                }

                BytesReadX += BytesRead;
            }

            //
            // stop timer
            //

            Time = EndTimer();

            //
            // log time
            //

            xLog(LogHandle, 
                 XLL_PASS, 
                 "Flags: 0x%8.8X Bytes Written: %10.10u Bytes Read: %10.10u Time: %10.10u ms",
                 FlagSets[y], BytesWrittenX, BytesReadX, Time);
        }

        //
        // cleanup
        //

        CloseHandle(FileHandle);

        if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
            CloseHandle(OverLapped.hEvent);
        }

        //
        // clean up the file
        //

        bResult = DeleteFile(OTEXT("Z:\\TestFile.prf"));
    }

    //
    // free test data buffers
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 
    VirtualFree(TestDataIn, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var6(
    VOID
    )
/*++

Routine Description:

    writes then reads, using a growing queue of files to thrash out the cache 
    (all sizes and flags)

Arguments:

    None

Return Value:

    None

Notes:

    using 10mb files, write one, then read all of them, 1 - TestFiles
    set the cache size to 512k

--*/ 
{
#define TestBufferSize6 (0x100000 * 10)
#define TestFiles 50
    DWORD FlagSets[] = {FILE_ATTRIBUTE_NORMAL,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_WRITE_THROUGH,
        FILE_FLAG_NO_BUFFERING,
        FILE_FLAG_SEQUENTIAL_SCAN};
    DWORD x, y, z;
    HANDLE FileHandle[TestFiles] = {0};
    OVERLAPPED OverLapped[TestFiles] = {0};
    OCHAR FileNameString[MAX_PATH];
    DWORD Time;
    PVOID TestData;
    PVOID TestDataIn;
    DWORD BytesWritten;
    DWORD BytesWrittenX;
    DWORD BytesRead;
    ULONGLONG BytesReadX;
    OVERLAPPED *pOverlapped;
    BOOL bResult;
    DWORD Error;

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize6);
    TestDataIn = VirtualAlloc(NULL, TestBufferSize6, MEM_COMMIT, PAGE_READWRITE);
    if ((NULL == TestDataIn) || (NULL == TestData)) {
        Error = GetLastError();
        DebugBreak();
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);

    xStartVariation( LogHandle, "variation6" );
    xLog(LogHandle, XLL_INFO, "writes then reads, using a growing queue of files to thrash out the cache (all sizes and flags)");

    //
    // for each flag set....
    //

    for (y = 0; y < ARRAYSIZE(FlagSets); y++) {

        CleanCaches();

        //
        // reset BytesReadX, BytesWrittenX counter
        //

        BytesReadX = 0;
        BytesWrittenX = 0;

        //
        // for each 10mb file...
        //

        //
        // start timer
        //

        StartTimer();

        for (x = 0; x < TestFiles; x++) {

            //
            // we have to point to an OVERLAPPED struct when appropriate
            // associate Event with struct (this is optional I believe)
            //

            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                pOverlapped = &OverLapped[x];
                ZeroMemory(pOverlapped, sizeof(*pOverlapped));
                if (NULL == (OverLapped->hEvent = CreateEvent(NULL, TRUE, FALSE, OTEXT("DiskPerfEvent")))) {
                    Error = GetLastError();
                    _asm int 3
                }
            } else {
                pOverlapped = NULL;
            }

            //
            // create file with specified flags
            //

            soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
            FileHandle[x] = CreateFile(FileNameString,
                                       GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FlagSets[y],
                                       NULL);
            if (INVALID_HANDLE_VALUE == FileHandle[x]) {
                Error = GetLastError();
                _asm int 3
            }

            //
            // write the file (this will be the only write to this file)
            //

            bResult = WriteFile(FileHandle[x], 
                                TestData,
                                TestBufferSize6,
                                &BytesWritten,
                                pOverlapped);

            //
            // if we need to wait on the event during this write, do it here
            //

            if (FALSE == bResult) {

                if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                    (ERROR_IO_PENDING == GetLastError())) {

                    bResult = GetOverlappedResult(FileHandle[x],
                                                  pOverlapped,
                                                  &BytesWritten,
                                                  TRUE);
                    if (FALSE == bResult) {
                        Error = GetLastError();
                        DebugBreak();
                    }

                } else {

                    //
                    // some other error, like disk full..
                    //

                    Error = GetLastError();
                    DebugBreak();
                }
            }

            BytesWrittenX += BytesWritten;

            //
            // set file pointer to beginning and read file data
            //

            for (z = 0; z <= x; z++) {

                if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                    pOverlapped->Offset = 0;
                    pOverlapped->OffsetHigh = 0;
                } else {
                    SetFilePointer(FileHandle[z], 0L, NULL, FILE_BEGIN);
                }

                bResult = ReadFile(FileHandle[z], 
                                   TestDataIn,
                                   TestBufferSize6,
                                   &BytesRead,
                                   pOverlapped);

                //
                // if we need to wait on the event during this multi-pass read, do it here
                //

                if (FALSE == bResult) {

                    if ((FlagSets[y] & FILE_FLAG_OVERLAPPED) && 
                        (ERROR_IO_PENDING == GetLastError())) {

                        bResult = GetOverlappedResult(FileHandle[z],
                                                      pOverlapped,
                                                      &BytesRead,
                                                      TRUE);
                        if (FALSE == bResult) {
                            Error = GetLastError();
                            DebugBreak();
                        }

                    } else {

                        //
                        // some other error, like disk full..
                        //

                        Error = GetLastError();
                        DebugBreak();
                    }
                }

                BytesReadX += BytesRead;
            }
        }

        //
        // stop timer
        //

        Time = EndTimer();

        //
        // log time
        //

        xLog(LogHandle, 
             XLL_PASS, 
             "Flags: 0x%8.8X Bytes Written: %10.10u Bytes Read: %12.12I64u Time: %10.10u ms",
             FlagSets[y], BytesWrittenX, BytesReadX, Time);

        //
        // cleanup
        //

        for (x = 0; x < TestFiles; x++) {
            CloseHandle(FileHandle[x]);
            soprintf(FileNameString, OTEXT("Z:\\TestFile%d.prf"), x);
            DeleteFile(FileNameString);
            if (FlagSets[y] & FILE_FLAG_OVERLAPPED) {
                CloseHandle(OverLapped[x].hEvent);
            }
        }
    }

    //
    // free test data buffer
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 
    VirtualFree(TestDataIn, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );

}

VOID
WINAPI
run_var7(
    VOID
    )
/*++

Routine Description:

    CD-ROM read rate

Arguments:

    None

Return Value:

    None
    
Notes:

    basically read everything off a dvd/CD and to the hard drive, and see how 
    long it takes
    
    Crude but all we can do without custom CDs

--*/ 
{
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    DWORD Time;
    ULONGLONG TotalBytesCopied = 0;
    DWORD TotalFilesCopied = 0;

    //
    // figure out the media type, and if is a CD-ROM continue
    //
    //
    if (FALSE == GetDiskFreeSpaceEx(OTEXT("a:\\"), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes)) {
        return; // no disk in drive or no drive
    }

    //if (TotalNumberOfBytes.LowPart > 1000000000) {
    //    return; // too big, must be a DVD
    //}

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);

    xStartVariation( LogHandle, "variation7" );
    xLog(LogHandle, XLL_INFO, "CD-ROM read rate - copied contents of CD to disk");

    //
    // start timer
    //

    StartTimer();

    //
    // do all the copies
    //

    RecursivelyDiskCopy(OTEXT("a:\\"), OTEXT("z:\\"), &TotalBytesCopied, &TotalFilesCopied, CREATE_FLAG);

    //
    // stop timer
    //

    Time = EndTimer();

    //
    // log it and calculate some simple stats
    //

    xLog(LogHandle, 
         XLL_PASS, 
         "Bytes copied: %12.12I64u  Files Copied: %10.10u Time: %10.10u ms",
         TotalBytesCopied, TotalFilesCopied, Time);

    //
    // clean up the disk
    //

    RecursivelyDiskCopy(OTEXT("a:\\"), OTEXT("z:\\"), &TotalBytesCopied, &TotalFilesCopied, DELETE_FLAG);

    xEndVariation( LogHandle );
}

typedef struct _RecursiveDiskCopyData {
    WIN32_FIND_DATA NodalFindFileData;
    OCHAR ExistingFileName[MAX_PATH];
    OCHAR NewFileName[MAX_PATH];
    OCHAR NewDirectoryName[MAX_PATH];
    OCHAR CurrentSourcePath[MAX_PATH];
    OCHAR CurrentSearchSourcePath[MAX_PATH];
    OCHAR CurrentDestPath[MAX_PATH];
} RecursiveDiskCopyData, *pRecursiveDiskCopyData;

VOID
WINAPI
RecursivelyDiskCopy(
    POCHAR source, 
    POCHAR target,
    PULONGLONG pTotalBytesCopied,
    PDWORD pTotalFilesCopied,
    DWORD CreateDeleteFlag
    )
/*++

Routine Description:

    recursivly read the data from directory-based device to another

Arguments:

    PWCHAR source - root of source device
    PWCHAR target - root of destination device
    PULONGLONG pTotalBytesCopied - pointer to variable that keeps the total number of bytes copied
    PDWORD pTotalFilesCopied - pointer to variable that keeps the total number of files copied

Return Value:

    None
    
Notes:

    need to modify to handle case when target is full
    probably just return -1 and percolate that back through to abort the traversal 
    the tricky part will be making sure we do the same thing in the copy / delete cases    

--*/ 
{
    HANDLE hNodalFile;
    BOOL bResult;
    DWORD Error;
    pRecursiveDiskCopyData pdata;
    WCHAR LogString[200 + (MAX_PATH * 2)] = {0};

    //
    // sources needs to come in with a  trailing backslash
    //

    pdata = calloc(1, sizeof(RecursiveDiskCopyData));

    if (NULL == pdata) {
        return;
    }

    ocscpy( pdata->CurrentSourcePath, source );
    ocscpy( pdata->CurrentDestPath, target );
    ocscpy( pdata->CurrentSearchSourcePath, pdata->CurrentSourcePath);
    strcat( pdata->CurrentSearchSourcePath, OTEXT("*.*"));

    hNodalFile = FindFirstFile(pdata->CurrentSearchSourcePath, &pdata->NodalFindFileData);

    if (INVALID_HANDLE_VALUE == hNodalFile) {
        Error = GetLastError();
        OutputDebugStringA( pdata->CurrentSearchSourcePath );
        OutputDebugString( L"\n" );
        OutputDebugString( TEXT("FindFirstFile() returned false - exiting node\n") );
        free(pdata);
        return;
    }

    do {

        //
        // if this is a file, copy or delete it
        //

        if (!(pdata->NodalFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

            ocscpy( pdata->NewFileName, pdata->CurrentDestPath );
            strcat( pdata->NewFileName, pdata->NodalFindFileData.cFileName );
            ocscpy( pdata->ExistingFileName, pdata->CurrentSourcePath );
            strcat( pdata->ExistingFileName, pdata->NodalFindFileData.cFileName );

            if (CREATE_FLAG == CreateDeleteFlag) {

                bResult = CopyFile(pdata->ExistingFileName, pdata->NewFileName, FALSE);

                if (FALSE == bResult) {
                    Error = GetLastError();
                    swscanf(LogString, 
                            TEXT("CopyFile(%s to %s) returned false. Error :0x%8.8X - exiting node!\n"), 
                            pdata->ExistingFileName, 
                            pdata->NewFileName, 
                            Error);
                    OutputDebugString( LogString );
                    continue;
                }

                //
                // increment the bytes copied counter
                //

                *pTotalBytesCopied += (ULONGLONG)(pdata->NodalFindFileData.nFileSizeLow);
                *pTotalBytesCopied += (ULONGLONG)(pdata->NodalFindFileData.nFileSizeHigh << 32);
                ++*pTotalFilesCopied;

            } else if (DELETE_FLAG == CreateDeleteFlag) {
                bResult = SetFileAttributes(pdata->NewFileName, FILE_ATTRIBUTE_NORMAL);
                bResult = DeleteFile(pdata->NewFileName);
                if (FALSE == bResult) {
                    Error = GetLastError();
                    swscanf(LogString, 
                            TEXT("DeleteFile(%s) returned false. Error :0x%8.8X\n"), 
                            pdata->NewFileName, 
                            Error);
                    OutputDebugString( LogString );
                }
            }
        }

        //
        // if this is a directory, go in and recurse
        //

        if (pdata->NodalFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

            ocscpy( pdata->NewFileName, pdata->CurrentSourcePath );
            strcat( pdata->NewFileName, pdata->NodalFindFileData.cFileName );

            //
            // skip default dirs
            //

            if (pdata->NodalFindFileData.cFileName[0] == L'.') {
                continue;
            }

            //
            // create/delete matching target directory
            //

            ocscpy( pdata->NewDirectoryName, pdata->CurrentDestPath );
            strcat( pdata->NewDirectoryName, pdata->NodalFindFileData.cFileName );
            strcat( pdata->NewDirectoryName, OTEXT("\\" ));

            if (CREATE_FLAG == CreateDeleteFlag) {
                if (FALSE == CreateDirectory( pdata->NewDirectoryName, NULL )) {
                    continue;
                }
            }

            strcat( pdata->NewFileName, OTEXT("\\") );

            RecursivelyDiskCopy(pdata->NewFileName, 
                                pdata->NewDirectoryName, 
                                pTotalBytesCopied, 
                                pTotalFilesCopied, 
                                CreateDeleteFlag);

            if (DELETE_FLAG == CreateDeleteFlag) {
                if (FALSE == RemoveDirectory( pdata->NewDirectoryName)) {
                    Error = GetLastError();
                    swscanf(LogString, 
                            TEXT("RemoveDirectory(%s) returned false. Error :0x%8.8X\n"), 
                            pdata->NewDirectoryName, 
                            Error);
                    OutputDebugString( LogString );
                    continue;
                }
            }
            continue;
        }
    } while (FindNextFile(hNodalFile, &pdata->NodalFindFileData));

    //
    // if there are no more files or directories, return
    //

    free(pdata);
    FindClose(hNodalFile);
    return;
}

VOID
WINAPI
run_var8(
    VOID
    )
/*++

Routine Description:

    DVD read rate  (copied contents of DVD to disk)

Arguments:

    None

Return Value:

    None

--*/ 
{
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    DWORD Time;
    ULONGLONG TotalBytesCopied = 0;
    DWORD TotalFilesCopied = 0;

    //
    // figure out the media type, and if is a CD-ROM continue
    //

    if (FALSE == GetDiskFreeSpaceEx(OTEXT("a:\\"), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes)) {
        return; // no disk in drive or no drive
    }

    if (TotalNumberOfBytes.LowPart < 1000000000) {
        return; // too small, must be a CD-ROM
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    
    xStartVariation( LogHandle, "variation8" );
    xLog(LogHandle, XLL_INFO, "DVD read rate - copied contents of DVD to disk");

    //
    // start timer
    //

    StartTimer();

    //
    // do all the copies
    //

    RecursivelyDiskCopy(OTEXT("a:\\"), OTEXT("z:\\"), &TotalBytesCopied, &TotalFilesCopied, CREATE_FLAG);

    //
    // stop timer
    //

    Time = EndTimer();

    //
    // log it and calculate some simple stats
    //

    xLog(LogHandle, 
         XLL_PASS, 
         "Bytes copied: %12.12I64u  Files Copied: %10.10u Time: %10.10u ms",
         TotalBytesCopied, TotalFilesCopied, Time);

    //
    // clean up the disk
    //

    RecursivelyDiskCopy(OTEXT("a:\\"), OTEXT("z:\\"), &TotalBytesCopied, &TotalFilesCopied, DELETE_FLAG);

    xEndVariation( LogHandle );

}

VOID
WINAPI
run_var9(
    VOID
    )
/*++

Routine Description:

    Delete speed (especially on a full disk to simulate cache clear on startup)

Arguments:

    None

Return Value:

    None

--*/ 
{
#define TestBufferSize9 (0x1000 * 10)
#define TestFiles9 1000
    DWORD x;
    OCHAR FileNameString[MAX_PATH];
    DWORD Time;
    PVOID TestData;
    BOOL bResult;
    DWORD BytesWritten;
    DWORD Error;
    HANDLE FileHandle;

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    CleanCaches();

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize9);
    if (NULL == TestData) {
        Error = GetLastError();
        DebugBreak();
    }

    
    xStartVariation( LogHandle, "variation9" );
    xLog(LogHandle, XLL_INFO, "Delete speed (especially on a full disk to simulate cache clear on startup)");

    //
    // clean up any leftovers from the last test
    //

    for (x = 0; x < TestFiles9; x++) {
        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        DeleteFile(FileNameString);
    }

    for (x = 0; x < TestFiles9; x++) {

        //
        // create file with specified flags
        //

        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        FileHandle = CreateFile(FileNameString,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

        if (INVALID_HANDLE_VALUE == FileHandle) {
            Error = GetLastError();
            break;
        }

        //
        // write the file (this will be the only write to this file)
        //

        bResult = WriteFile(FileHandle, 
                            TestData,
                            TestBufferSize9,
                            &BytesWritten,
                            NULL);

        if (FALSE == bResult) {
            Error = GetLastError();
            CloseHandle(FileHandle);
            break;
        }

        CloseHandle(FileHandle);
    }

    //
    // start timer
    //

    StartTimer();

    for (x = 0; x < TestFiles9; x++) {

        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        DeleteFile(FileNameString);

        //
        // query timer, log time every few files
        //

        if ((x % 20) == 0) {
            Time = EndTimer();
            xLog(LogHandle, XLL_PASS, "files: %d Time: %10.10u ms", x, Time);
        }
    }

    //
    // free test data buffer
    //

    VirtualFree(TestData, 0, MEM_RELEASE); 

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var10(
    VOID
    )
/*++

Routine Description:

    file create slowdown as number of files in a directory increases

Arguments:

    None

Return Value:

    None

--*/ 
{
#define TestFiles10 15000
    DWORD x;
    OCHAR FileNameString[MAX_PATH];
    DWORD Time;
    DWORD Error;
    HANDLE FileHandle;

    xStartVariation( LogHandle, "variation10" );
    xLog(LogHandle, XLL_INFO, "file create slowdown as number of files in a directory increases");

    //
    // pre-clean
    //

    for (x = 0; x < TestFiles10; x++) {
        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        DeleteFile(FileNameString);
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    
    CleanCaches();

    //
    // Create files, we'll use empty files
    //

    for (x = 0; x < TestFiles10; x++) {

        if ((x % 20) == 0) {

            //
            // start timer to measure this group of 20 files
            //

            StartTimer();
        }

        //
        // create file with specified flags
        //

        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        FileHandle = CreateFile(FileNameString,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

        if (INVALID_HANDLE_VALUE == FileHandle) {
            Error = GetLastError();
            break;
        }

        CloseHandle(FileHandle);

        //
        // query timer, log time every few files
        //

        if ((x % 20) == 0) {
            Time = EndTimer();
            xLog(LogHandle, XLL_PASS, "files: %d Time: %10.10u ms", x, Time);
        }
    }

    //
    // cleanup
    //

    for (x = 0; x < TestFiles10; x++) {
        soprintf(FileNameString, OTEXT("Z:\\Test%d.prf"), x);
        DeleteFile(FileNameString);
    }
    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var11(
    VOID
    )
/*++

Routine Description:

    directory create slowdown as number of directory entries increases

Arguments:

    None

Return Value:

    None

--*/ 
{
#define TestDirectories11 15000
    DWORD x;
    OCHAR DirectoryNameString[MAX_PATH];
    DWORD Time;
    DWORD Error;
    BOOL bResult;

    xStartVariation( LogHandle, "variation11" );
    xLog(LogHandle, XLL_INFO, "directory create slowdown as number of directories in a directory increases");

    //
    // pre-clean
    //

    for (x = 0; x < TestDirectories11; x++) {
        soprintf(DirectoryNameString, OTEXT("Z:\\Test%d.prf"), x);
        RemoveDirectory(DirectoryNameString);
    }

    //
    // set the cache size to something reasonable (128k)
    //

    SizeCache(128 * 1024);
    
    CleanCaches();

    //
    // Create directories, we'll use empty directories
    //

    for (x = 0; x < TestDirectories11; x++) {

        if ((x % 20) == 0) {

            //
            // start timer to measure this group of 20 directories
            //

            StartTimer();
        }

        //
        // create file with specified flags
        //

        soprintf(DirectoryNameString, OTEXT("Z:\\Test%d.prf"), x);
        bResult = CreateDirectory(DirectoryNameString, NULL);

        if (FALSE == bResult) {
            Error = GetLastError();
            break;
        }

        //
        // query timer, log time every few directories
        //

        if ((x % 20) == 0) {
            Time = EndTimer();
            xLog(LogHandle, XLL_PASS, "directories: %d Time: %10.10u ms", x, Time);
        }
    }

    //
    // cleanup
    //

    for (x = 0; x < TestDirectories11; x++) {
        soprintf(DirectoryNameString, OTEXT("Z:\\Test%d.prf"), x);
        RemoveDirectory(DirectoryNameString);
    }

    xEndVariation( LogHandle );
}

VOID
WINAPI
run_var12(
    VOID
    )
/*++

Routine Description:

    random access within a file while varying cache size

Arguments:

    None

Return Value:

    None
    
Notes:

    repeatable random access will be done by seeding with a constant
    random numbers range from 0-32767
        
    make code stop at cache sizes larger than available memory... 
    since my current test box is not like the 'real' hardware...
    

--*/ 
{
#define TestBufferSize12 0x1000000
#define FileFactorSize 20
    DWORD CacheSizes[] = {
        5000000, 
        10000000, 
        15000000, 
        20000000, 
        25000000, 
        30000000, 
        35000000, 
        40000000, 
        45000000, 
        50000000, 
        55000000
    };
    HANDLE FileHandle;
    BOOL bResult;
    PVOID TestData;
    PVOID TestDataIn;
    DWORD BytesWritten;
    DWORD CacheSize;
    DWORD BytesRead;
    DWORD TestInBufferSize12;
    DWORD FileChunks = (FileFactorSize * TestBufferSize12) / RAND_MAX;
    int x, y;
    DWORD Error;
    DWORD Time;

    //
    // read buffer size rounded to page
    //

    TestInBufferSize12 = FileChunks - (FileChunks % 0x1000);

    xStartVariation( LogHandle, "variation12" );
    xLog(LogHandle, XLL_INFO, "random access within a file while varying cache size");

    //
    // allocate and initialize test buffer
    //

    TestData = CreateTestDataBuffer(TestBufferSize12);
    TestDataIn = VirtualAlloc(NULL, TestInBufferSize12, MEM_COMMIT, PAGE_READWRITE);

    if ((NULL == TestDataIn) || (NULL == TestData)) {
        Error = GetLastError();
        DebugBreak();
    }

    //
    // create and fill file
    //

    FileHandle = CreateFile(OTEXT("Z:\\TestFile.prf"),
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,  // may want to modify this later
                            NULL);

    if (INVALID_HANDLE_VALUE == FileHandle) {
        Error = GetLastError();
        DebugBreak();
    }

    for (x = 0; x < FileFactorSize; x++) {
        bResult = WriteFile(FileHandle, 
                            TestData,
                            TestBufferSize12,
                            &BytesWritten,
                            NULL);

        if (FALSE == bResult) {
            Error = GetLastError();
            DebugBreak();
        }
    }

    VirtualFree(TestData, 0, MEM_RELEASE); 

    for (x = 0; x < ARRAYSIZE(CacheSizes); x++) {

        //
        // initialize randomizer so that each variation has the same access pattern
        //

        srand(5);

        //
        // clear cache
        //

        CleanCaches();

        //
        // limit cache size
        // the functions that do this round up to page
        //

        CacheSize = SizeCache(CacheSizes[x]);

        //
        // start timer
        //

        StartTimer();

        //
        // do searches / reads
        //

        for (y = 0; y < 10000; y++) {

            SetFilePointer(FileHandle, 
                           FileChunks * rand(), 
                           NULL, 
                           FILE_BEGIN);

            bResult = ReadFile(FileHandle, 
                               TestDataIn,
                               TestInBufferSize12,
                               &BytesRead,
                               NULL);

            if (FALSE == bResult) {
                Error = GetLastError();
                DebugBreak();
            }
        }

        //
        // stop timer
        //

        Time = EndTimer();

        //
        // log time
        //

        xLog(LogHandle, 
             XLL_PASS, 
             "cachesize: %10.10u Time: %10.10u ms",
             CacheSizes[x], Time);

        //xLogResourceStatus(LogHandle,NULL);

    }

    //
    // delete file
    //

    CloseHandle(FileHandle);
    DeleteFile(OTEXT("Z:\\TestFile.prf"));
    VirtualFree(TestDataIn, 0, MEM_RELEASE); 
    xEndVariation( LogHandle );
    return;
}

VOID
WINAPI
StartTimer(
    VOID
    )
/*++

Routine Description:

    Start the test timer and returns 

Arguments:

    None

Return Value:

    None

Notes:

    this uses a global variable so the calling function does not have to 
    maintain state. this is to make this as convienient and easy as possible 
    to use and un-clutter the calling function

--*/ 
{
    QueryPerformanceCounter(&PerformanceCount);
}

DWORD
WINAPI
EndTimer(
    VOID
    )
/*++

Routine Description:

    Ends the test timer and returns the elapsed time in Milliseconds

Arguments:

    None

Return Value:

    elapsed time in Milliseconds

--*/ 
{
    LARGE_INTEGER PerformanceCountFinish;

    QueryPerformanceCounter(&PerformanceCountFinish);
    return(DWORD)((PerformanceCountFinish.QuadPart - PerformanceCount.QuadPart) / (Frequency.QuadPart / 1000));
}

PVOID
WINAPI
CreateTestDataBuffer(
    DWORD BufferSize
    )
/*++

Routine Description:

    create our test data buffer

Arguments:

    None

Return Value:

    pointer to test buffer

Notes:

    free the buffer like this when finished with it
    VirtualFree(<whatever this function returned>, 0, MEM_RELEASE);


--*/ 
{
    PVOID FileDataBuffer;
    char *tmpPtr;
    DWORD x;

    FileDataBuffer = VirtualAlloc(NULL, BufferSize, MEM_COMMIT, PAGE_READWRITE);

    if (NULL == FileDataBuffer) {
        OutputDebugString(L"Out of memory!");
        return(NULL);
    }

    tmpPtr = FileDataBuffer;
    for (x = 1; x < BufferSize; x++) {
        *tmpPtr = (char)(x % 16);
        ++tmpPtr;
    }

    return(FileDataBuffer);
}

VOID
WINAPI
CleanCaches(
    VOID
    )
/*++

Routine Description:

    clear out processor and all disk caches

Arguments:

    None

Return Value:

    pointer to test buffer

Notes:

    relies on non-documented functions

--*/ 
{

    FlushDiskCache();
    FscInvalidateIdleBlocks();

    //
    // the following command invalidates the L1 and L2 processor caches
    //

    _asm {
        wbinvd
    }

}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( diskperf )
#pragma data_seg()

BEGIN_EXPORT_TABLE( diskperf )
EXPORT_TABLE_ENTRY( "StartTest", DiskPerfStartTest )
EXPORT_TABLE_ENTRY( "EndTest", DiskPerfEndTest )
END_EXPORT_TABLE( diskperf )
