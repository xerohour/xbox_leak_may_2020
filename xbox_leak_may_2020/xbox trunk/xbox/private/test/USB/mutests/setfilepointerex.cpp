/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    SetFilePointerEx.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

    If you make changes to the test cases, be sure to make similiar changes
    to the cases in SetFilePointer.cpp

*/

#include "stdafx.h"
#include "commontest.h"
#include "mutests.h"

/*

Routine Description:

    Tests the SetFilePointerEx API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::SetFilePointerEx_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL SetFilePointerEx(
      HANDLE hFile,                    // handle to file
      LARGE_INTEGER liDistanceToMove,  // bytes to move pointer
      PLARGE_INTEGER lpNewFilePointer, // new file pointer
      DWORD dwMoveMethod               // starting point
    );
    */
    xSetFunctionName(hLog, "SetFilePointerEx");

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FFPX01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%ws:\\FFPX02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%ws:\\FFPX03_%d.txt", BASE_DRIVE, ThreadID);

    char *stdStr = "DWORD SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);";
    DWORD size;
    DWORD wSize, rSize;
    LARGE_INTEGER i64;
    HANDLE hFile = NULL;
    DWORD seek = 0;

    char *wbuffer = new char[MEGABYTE*10];
    if(!wbuffer)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }
    char *rbuffer = new char[MEGABYTE*10];
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
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Seek(0 FILE_BEGIN) Read/Write")
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            sprintf(wbuffer, "Unable to open file (ec: %ld)", GetLastError());
            BLOCKED(0, wbuffer);
            }

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(0), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 0)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(0), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 0)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

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

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(1), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 1)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(1), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 1)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

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

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(10), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 10)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(10), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 10)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

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

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(300), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 300)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(300), &i64, FILE_BEGIN) == TRUE && i64.QuadPart == 300)
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

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

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(-200), &i64, FILE_END) == TRUE && i64.QuadPart == 100+strlen(stdStr))
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        size = sprintf(wbuffer, "%s", stdStr);
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        if(SetFilePointerEx(hFile, iLARGE_INTEGER(-200), &i64, FILE_END) == TRUE && i64.QuadPart == 100+strlen(stdStr))
            TESTPASS(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%I64d] (ec: %ld)", i64.QuadPart, GetLastError());

        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);
        } ENDTESTCASE;


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

