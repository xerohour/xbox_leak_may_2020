/*

Copyright (C) Microsoft Corporation.  All rights reserved.

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
#include "fileio_win32API.h"

/*

Routine Description:

    Tests the ReadFile / WriteFile APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::ReadWriteFile_Test(HANDLE hLog, DWORD ThreadID)
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

    char *wbuffer = new char[MEGABYTE*24];
    if(!wbuffer)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }
    char *rbuffer = new char[MEGABYTE*24];
    if(!rbuffer)
        {
        delete[] wbuffer;
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FRW01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%s:\\FRW02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%s:\\FRW03_%d.txt", BASE_DRIVE, ThreadID);

    HANDLE hFile = NULL;
    HANDLE hFile1 = NULL;
    HANDLE hFile2 = NULL;

    DWORD wSize, rSize, size;

    // initialize state
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("All NUL")
        {
        if(WriteFile(hNull, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "WriteFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "WriteFile (ec: %ld)", GetLastError());
        if(ReadFile(hNull, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "ReadFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "ReadFile (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("INVALID_HANDLE_VALUE NUL")
        {
        if(WriteFile(INVALID_HANDLE_VALUE, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "WriteFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "WriteFile (ec: %ld)", GetLastError());
        if(ReadFile(INVALID_HANDLE_VALUE, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "ReadFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "ReadFile (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Valid file handle, All NUL")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        BUGGEDPASS(196, "ACCESS VIOLATION \"By Design\"");
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        if(WriteFile(hFile, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "WriteFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "WriteFile (ec: %ld)", GetLastError());
        if(ReadFile(hFile, NULL, 0, NULL, NULL) == FALSE)
            TESTPASS(hLog, "ReadFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "ReadFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
*/

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Read/Write 0 bytes")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "");
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
    TESTCASE("Read/Write 1 byte")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "1");
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
    TESTCASE("Read/Write 1 space")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, " ");
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
    TESTCASE("Read/Write 2 bytes")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "69");
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
    TESTCASE("Read/Write short text")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n");
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
    TESTCASE("Read/Write long text")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n\r\n"
                "The quick brown fox jumps over the lazy dog\r\n"
                "1234567890\t-=`~!@#$%^&*()_+[]}{\\|';:\"/.,<>?\r\n\r\n"
                "This is a story about Ranet Jeno, the wicked witch from a swamp called DC.\r\n\r\n"
                "(All persons, places, and events are fictitious and bear no resemblance to any living or deceased (past, present, or future) persons, places or events.)\r\n\r\n");
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
    TESTCASE("Read/Write really long text (0.5meg)")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(int i=0; i<8192; i++)
            {
            size += sprintf(wbuffer+size, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n\r\n");
            }
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
    TESTCASE("Read/Write short binary (256b)")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<256; size++)
            {
            wbuffer[size] = (char)size;
            }
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
    TESTCASE("FILE_FLAG_SEQUENTIAL_SCAN")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<256; size++)
            {
            wbuffer[size] = (char)~size;
            }
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
    TESTCASE("FILE_FLAG_RANDOM_ACCESS")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<256; size++)
            {
            wbuffer[size] = (char)size;
            }
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
    TESTCASE("FILE_FLAG_WRITE_THROUGH")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<256; size++)
            {
            wbuffer[size] = (char)~size;
            }
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
    TESTCASE("Read/Write 15 meg")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<MEGABYTE*15; size++)
            {
            wbuffer[size] = (char)size;
            }
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
    TESTCASE("Read/Write 256 meg (8 meg chunks)")
        {
        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxChunks = 32;
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        for(size=0; size<MEGABYTE*8; size++)
            {
            wbuffer[size] = (char)~size;
            }
        for(unsigned int i=0; i<maxChunks; i++)
            {
            if(i%6==0) DebugPrint("   (%d) %02d%%\n", ThreadID, 50*i/maxChunks);
            if(! (WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size))
                break;
            }

        if(i == maxChunks)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", size*i, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", size*i, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        for(unsigned j=0; j<i; j++)
            {
            if(j%6==0) DebugPrint("   (%d) %02d%%\n", ThreadID, 50*j/maxChunks + 50);
            if(! (ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size))
                break;

            if(! (wSize == rSize && memcmp(rbuffer, wbuffer, size) == 0))
                break;
            }

        if(j == i)
            {
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", size*j, GetLastError());
            TESTPASS(hLog, "Write Buffer == Read Buffer");
            }
        else
            {
            if(! (wSize == rSize && memcmp(rbuffer, wbuffer, size) == 0))
                {
                TESTFAIL(hLog, "Write Buffer != Read Buffer");
                }
            else
                {
                TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", size*j, GetLastError());
                }
            }
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); CloseHandle(hFile); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Read from a write only file")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n");
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
            {
            sprintf(wbuffer, "Unable to reset file pointer (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == FALSE)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        // Note, the next test ("Write to a read only file") depends on this test's file.
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); });
    TESTCASE("Write to a read only file")
        {
        // This test depends on the prevous test's ("Read from a write only file") file.
        hFile = CreateFile(lpFile1, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == FALSE)
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
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("Read/Write to closed handle")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }
        size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n");
        CloseHandle(hFile);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == FALSE)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == FALSE)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });


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
