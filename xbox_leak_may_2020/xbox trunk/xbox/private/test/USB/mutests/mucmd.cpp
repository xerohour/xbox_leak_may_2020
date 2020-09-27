/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ReadWriteFile.cpp

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

/*

Routine Description:

    Tests the ReadFile / WriteFile APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::MUCommand_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL ReadFile(
      HANDLE hFile,                // handle to file
      LPVOID lpBuffer,             // data buffer
      DWORD nNumberOfBytesToRead,  // number of bytes to read
      LPDWORD lpNumberOfBytesRead, // number of bytes read
      LPOVERLAPPED lpOverlapped    // overlapped buffer
    );

    BOOL WriteFile(
      HANDLE hFile,                    // handle to file
      LPCVOID lpBuffer,                // data buffer
      DWORD nNumberOfBytesToWrite,     // number of bytes to write
      LPDWORD lpNumberOfBytesWritten,  // number of bytes written
      LPOVERLAPPED lpOverlapped        // overlapped buffer
    );
    */
    xSetFunctionName(hLog, "ReadFile / WriteFile");

    char *wbuffer = new char[MEGABYTE*1];
    if(!wbuffer)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }
    char *rbuffer = new char[MEGABYTE*1];
    if(!rbuffer)
        {
        delete[] wbuffer;
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\MUCMDFile1_%d.txt", BASE_DRIVE, ThreadID);

    HANDLE hFile = NULL;

    DWORD wSize, rSize, size;

    // initialize state
    DeleteFile(lpFile1);

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("MU Command 1")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        char data[] = "\x55\x53\x42\x43\x88\x1A\x18\xD1\x08\x00\x00\x00\x80\x00\x0A\x25\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
        size = sizeof(data)-1;
        memcpy(wbuffer, data, sizeof(data));
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });

    TESTCASE("MU Command 2")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        char data[] = "\x55\x53\x42\x43\x48\xCF\x19\xD1\x00\x10\x00\x00\x80\x00\x0A\x28\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00";
        size = sizeof(data)-1;
        memcpy(wbuffer, data, sizeof(data));
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });

    TESTCASE("MU Command 3")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        char data[] = "\x55\x53\x42\x53\x88\x1A\x18\xD1\x00\x00\x00\x00\x00";
        size = sizeof(data)-1;
        memcpy(wbuffer, data, sizeof(data));
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });

    TESTCASE("MU Command 4")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        char data[] = "\x00\x00\x3E\xFF\x00\x00\x02\x00";
        size = sizeof(data)-1;
        memcpy(wbuffer, data, sizeof(data));
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    delete[] wbuffer;
    delete[] rbuffer;

    return 0;
    }
