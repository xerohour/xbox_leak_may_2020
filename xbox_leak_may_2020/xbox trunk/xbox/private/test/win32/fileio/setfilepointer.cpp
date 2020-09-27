/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    SetFilePointer.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

    If you make changes to the test cases, be sure to make similiar changes
    to the cases in SetFilePointerEx.cpp

*/

#include "stdafx.h"
#include "commontest.h"
#include "fileio_win32API.h"

/*

Routine Description:

    Tests the SetFilePointer API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::SetFilePointer_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD SetFilePointer(
      HANDLE hFile,                // handle to file
      LONG lDistanceToMove,        // bytes to move pointer
      PLONG lpDistanceToMoveHigh,  // bytes to move pointer
      DWORD dwMoveMethod           // starting point
    );
    */
    xSetFunctionName(hLog, "SetFilePointer");

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FFP01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%s:\\FFP02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%s:\\FFP03_%d.txt", BASE_DRIVE, ThreadID);
    char lpFileTemp[80];

    char *stdStr = "DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);";
    DWORD size;
    DWORD wSize, rSize;

    HANDLE hFile = NULL;
    HANDLE hFileTemp;
    DWORD seek = 0;

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

    hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NUL")
        {
        if(SetFilePointer(hNull, 0, NULL, 0) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("INVALID_HANDLE_VALUE")
        {
        if(SetFilePointer(INVALID_HANDLE_VALUE, 0, NULL, 0) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Seek(0 FILE_BEGIN) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) == 0)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if((seek=SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) == 0)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;
    TESTCASE("Seek(1 FILE_BEGIN) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, 1, NULL, FILE_BEGIN)) == 1)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if((seek=SetFilePointer(hFile, 1, NULL, FILE_BEGIN)) == 1)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;
    TESTCASE("Seek(10 FILE_BEGIN) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, 10, NULL, FILE_BEGIN)) == 10)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if((seek=SetFilePointer(hFile, 10, NULL, FILE_BEGIN)) == 10)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;
    TESTCASE("Seek(300 FILE_BEGIN) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, 300, NULL, FILE_BEGIN)) == 300)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if((seek=SetFilePointer(hFile, 300, NULL, FILE_BEGIN)) == 300)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;
    TESTCASE("Seek(-200 FILE_END) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -200, NULL, FILE_END)) == 100+strlen(stdStr))
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if((seek=SetFilePointer(hFile, -200, NULL, FILE_END)) == 100+strlen(stdStr))
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;
    TESTCASE("FatxFreeClusters Stress Test")
        {
        int stressCount;
        const int maxStress = 1000;
        // Testcase for bug #2095: FatxFreeClusters overcounts volume free cluster count when truncating a file allocation
        sprintf(lpFileTemp, "%s:\\FFP04_%d.txt", BASE_DRIVE, ThreadID);
        hFileTemp = CreateFile(lpFileTemp, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(stressCount=0; stressCount<maxStress; stressCount++)
            {
            if(stressCount%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, stressCount*100/maxStress);

            SetFilePointer(hFileTemp, 1000000, NULL, FILE_BEGIN);
            SetEndOfFile(hFileTemp);
            SetFilePointer(hFileTemp, 1000, NULL, FILE_BEGIN);
            SetEndOfFile(hFileTemp);
            }

        CloseHandle(hFileTemp);
        DeleteFile(lpFileTemp);
        } ENDTESTCASE_CLEAN( { DebugPrint("   (%d) 100%%\n", ThreadID); } );


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Seek(-1 FILE_BEGIN)")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -1, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Seek(-2 FILE_BEGIN)")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -2, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Seek(-200 FILE_BEGIN)")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -200, NULL, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Seek(-1000 FILE_END)")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -1000, NULL, FILE_END)) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Seek(-1000 FILE_CURRENT)")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if((seek=SetFilePointer(hFile, -1000, NULL, FILE_CURRENT)) == INVALID_SET_FILE_POINTER)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    CloseHandle(hFile);
    DeleteFile(lpFile1);
    delete[] wbuffer;
    delete[] rbuffer;

    return 0;
    }

