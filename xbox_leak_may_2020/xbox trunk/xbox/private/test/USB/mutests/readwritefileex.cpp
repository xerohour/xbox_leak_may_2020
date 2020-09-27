/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ReadWriteFileEx.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "mutests.h"


struct CompletionState
    {
    long state;         // current state 0=write 1=read 0xff=timeout
    DWORD errorCode;    // expected error code
    DWORD size;         // size of the data to read/write
    char *variation;    // name of this test variation
    HANDLE hFile;       // handle to the file
    char* filename;     // filename
    char *rbuffer;      // read buffer
    char *wbuffer;      // write buffer
    HANDLE hLog;        // handle to the logging object
    };

static void CALLBACK CompletionFunct(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
static void SerializeTest(HANDLE hLog, char *variation, char* filename, DWORD size, char *rbuffer, char *wbuffer);

/*

Routine Description:

    Tests the ReadFileEx / WriteFileEx APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::ReadWriteFileEx_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL ReadFileEx(
      HANDLE hFile,                                       // handle to file
      LPVOID lpBuffer,                                    // data buffer
      DWORD nNumberOfBytesToRead,                         // number of bytes to read
      LPOVERLAPPED lpOverlapped,                          // offset
      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
    );

    BOOL WriteFileEx(
      HANDLE hFile,                                       // handle to output file
      LPCVOID lpBuffer,                                   // data buffer
      DWORD nNumberOfBytesToWrite,                        // number of bytes to write
      LPOVERLAPPED lpOverlapped,                          // overlapped buffer
      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
    );
    */
    xSetFunctionName(hLog, "ReadFileEx / WriteFileEx");

    char *wbuffer = new char[MEGABYTE*16];
    if(!wbuffer)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }
    char *rbuffer = new char[MEGABYTE*16];
    if(!rbuffer)
        {
        delete[] wbuffer;
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FRWX01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%ws:\\FRWX02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%ws:\\FRWX03_%d.txt", BASE_DRIVE, ThreadID);

    HANDLE hFile = NULL;
    HANDLE hFile1 = NULL;
    HANDLE hFile2 = NULL;

    DWORD size;

    // initialize state
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);


    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    size = sprintf(wbuffer, "");
    SerializeTest(hLog, "Read/Write 0 bytes", lpFile1, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, "1");
    SerializeTest(hLog, "Read/Write 1 byte", lpFile2, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, " ");
    SerializeTest(hLog, "Read/Write 1 space", lpFile3, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, "69");
    SerializeTest(hLog, "Read/Write 2 bytes", lpFile1, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, "xboxboxboxboxbox");
    SerializeTest(hLog, "Read/Write 16 bytes", lpFile2, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFileEx() / WriteFileEx() API Test!\r\n");
    SerializeTest(hLog, "Read/Write short text", lpFile3, size, rbuffer, wbuffer);

    size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFileEx() / WriteFileEx() API Test!\r\n\r\n"
            "The quick brown fox jumps over the lazy dog\r\n"
            "1234567890\t-=`~!@#$%^&*()_+[]}{\\|';:\"/.,<>?\r\n\r\n"
            "This is a story about Ranet Jeno, the wicked witch from a swamp called DC.\r\n\r\n"
            "(All persons, places, and events are fictitious and bear no resemblance to any living or deceased (past, present, or future) persons, places or events.)\r\n\r\n");
    SerializeTest(hLog, "Read/Write long text", lpFile1, size, rbuffer, wbuffer);

    for(int i=0; i<8192; i++)
        {
        size += sprintf(wbuffer+size, "Hello World.\r\n\r\nThis is a ReadFileEx() / WriteFileEx() API Test!\r\n\r\n");
        }
    SerializeTest(hLog, "Read/Write really long text (0.5meg)", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<256; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write short binary (256b)", lpFile3, size, rbuffer, wbuffer);

    for(size=0; size<512; size++)
        {
        wbuffer[size] = (char)(~size);
        }
    SerializeTest(hLog, "Read/Write short binary (512b)", lpFile1, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*3; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 3k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*4; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 4k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*5; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 5k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*7; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 7k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*8; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 8k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<KILOBYTE*9; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 9k", lpFile2, size, rbuffer, wbuffer);

    for(size=0; size<MEGABYTE*6; size++)
        {
        wbuffer[size] = (char)size;
        }
    SerializeTest(hLog, "Read/Write 6 meg", lpFile2, size, rbuffer, wbuffer);


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    delete[] wbuffer;
    delete[] rbuffer;

    // cleanup
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);

    return 0;
    }



static VOID CALLBACK CompletionFunct(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
    // Bad pointer
    if(!lpOverlapped)
        {
        DebugPrint("OVERLAPPED struct is null (ec: %ld)", dwErrorCode);
        return;
        }

    CompletionState *testinfo = (CompletionState*)lpOverlapped->hEvent;

    // Bad pointer
    if(!testinfo)
        {
        memset(lpOverlapped, 0, sizeof(OVERLAPPED));
        delete lpOverlapped;
        DebugPrint("OVERLAPPED struct contained invalid pointer to state information (ec: %ld)", dwErrorCode);
        return;
        }

    long stateVal = InterlockedIncrement(&testinfo->state) - 1;

    // Write Callback
    if(stateVal == 0)
        {
        if(dwNumberOfBytesTransfered == testinfo->size && dwErrorCode == testinfo->errorCode)
            TESTPASS(testinfo->hLog, "WriteFileEx[%ld] (ec: %ld)", dwNumberOfBytesTransfered, dwErrorCode);
        else
            TESTFAIL(testinfo->hLog, "WriteFileEx[%ld] (ec: %ld)", dwNumberOfBytesTransfered, dwErrorCode);


        if(ReadFileEx(testinfo->hFile, testinfo->rbuffer, testinfo->size, lpOverlapped, CompletionFunct) == TRUE)
            TESTPASS(testinfo->hLog, "ReadFileEx (ec: %ld)", GetLastError());
        else
            TESTFAIL(testinfo->hLog, "ReadFileEx (ec: %ld)", GetLastError());
        }

    // Read Callback
    else if(stateVal == 1)
        {
        if(dwNumberOfBytesTransfered == testinfo->size && dwErrorCode == testinfo->errorCode)
            TESTPASS(testinfo->hLog, "ReadFileEx[%ld] (ec: %ld)", dwNumberOfBytesTransfered, dwErrorCode);
        else
            TESTFAIL(testinfo->hLog, "ReadFileEx[%ld] (ec: %ld)", dwNumberOfBytesTransfered, dwErrorCode);

        if(memcmp(testinfo->rbuffer, testinfo->wbuffer, testinfo->size) == 0)
            TESTPASS(testinfo->hLog, "Write Buffer[%d] == Read Buffer[%d]", testinfo->size, dwNumberOfBytesTransfered);
        else
            TESTFAIL(testinfo->hLog, "Write Buffer[%d] != Read Buffer[%d]", testinfo->size, dwNumberOfBytesTransfered);

        CloseHandle(testinfo->hFile);
        DeleteFile(testinfo->filename);
        memset(testinfo, 0, sizeof(CompletionState));
        memset(lpOverlapped, 0, sizeof(OVERLAPPED));
        delete testinfo;
        delete lpOverlapped;
        }

    // We hit timeout before the callback - things are messed, so cleanup and bail
    else if(stateVal == 0xFF)
        {
        CloseHandle(testinfo->hFile);
        DeleteFile(testinfo->filename);
        memset(testinfo, 0, sizeof(CompletionState));
        memset(lpOverlapped, 0, sizeof(OVERLAPPED));
        delete testinfo;
        delete lpOverlapped;
        }

    // unknown state
    else
        {
        memset(lpOverlapped, 0, sizeof(OVERLAPPED));
        delete lpOverlapped;
        TESTFAIL(testinfo->hLog, "State information contained invalid state (ec: %ld)", dwErrorCode);
        return;
        }
    }



static void SerializeTest(HANDLE hLog, char *variation, char* filename, DWORD size, char *rbuffer, char *wbuffer)
    {
    CompletionState *state = new CompletionState; // gets deleted in CompletionFunct
    if(!state) return;

    OVERLAPPED *overlap = new OVERLAPPED; // gets deleted in CompletionFunct
    if(!overlap)
        {
        delete state;
        return;
        }

    // hEvent is ignored by ReadFileEx & WriteFileEx and so they allow us to pass information via this pointer
    overlap->hEvent = (HANDLE)state;
    overlap->Internal = 0;
    overlap->InternalHigh = 0;
    overlap->Offset = 0;
    overlap->OffsetHigh = 0;

    state->rbuffer = rbuffer;
    state->wbuffer = wbuffer;
    state->errorCode = 0;
    state->filename = filename;
    state->size = size;
    state->variation = variation;
    state->state = 0;
    state->hLog = hLog;

    long stateVal=0;

    TESTCASE(variation)
        {
        state->hFile = CreateFile(state->filename, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
        if(state->hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        DWORD dwBytesWritten;

        if(WriteFile(state->hFile, state->wbuffer, state->size, &dwBytesWritten, overlap) == TRUE)
            TESTPASS(hLog, "WriteFileEx (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "WriteFileEx (ec: %ld)", GetLastError());

        // wait for the IO Callbacks
        do
            {
            if(SleepEx(15000, true) != WAIT_IO_COMPLETION)
                {
                stateVal = InterlockedExchange(&state->state, 0xFF);
                if(stateVal < 2)
                    {
                    TESTFAIL(hLog, "Hit timeout before IO Completed (memory may leak as a result) state=%d", stateVal);
                    SleepEx(15000, true); // wait for a bit longer just to be sure
                    }
                break;
                }

            // get the current value 
            stateVal = InterlockedCompareExchange(&state->state, 0xFF, 0xFF);
            } while(stateVal < 2);
        } ENDTESTCASE;
    
    }
