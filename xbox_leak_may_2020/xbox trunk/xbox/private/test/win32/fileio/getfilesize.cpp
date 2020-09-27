/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetFileSize.cpp

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

    Tests the GetFileSize / GetFileSizeEx APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetFileSize_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD GetFileSize(
      HANDLE hFile,           // handle to file
      LPDWORD lpFileSizeHigh  // high-order word of file size
    );

    BOOL GetFileSizeEx(
      HANDLE hFile,              // handle to file
      PLARGE_INTEGER lpFileSize  // file size
    );
    */
    xSetFunctionName(hLog, "GetFileSize");

    HANDLE file = NULL;
    DWORD size;
    DWORD highSize;
    LARGE_INTEGER bigSize;
    bigSize.QuadPart = 0;

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FGS01_%d.txt", BASE_DRIVE, ThreadID);

    DeleteFile(lpFile1);


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("hFile == NUL")
        {
        highSize = dwBogus;
        if((highSize=GetFileSize(NULL, &highSize)) == -1)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(NULL, &bigSize) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE;
    TESTCASE("hFile == INVALID_HANDLE_VALUE")
        {
        highSize = dwBogus;
        if((highSize=GetFileSize(INVALID_HANDLE_VALUE, &highSize)) == -1)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(INVALID_HANDLE_VALUE, &bigSize) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE;
    TESTCASE("hFile == NULL && lpFileSizeHigh == NUL")
        {
        if((highSize=GetFileSize(NULL, NULL)) == -1)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(NULL, NULL) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("lpFileSizeHigh == NUL")
        {
        size = 5;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);

        BUGGEDPASS(137, "ACCESS VIOLATION \"By Design\"");

        if((highSize=GetFileSize(file, NULL)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, NULL) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("filesize = 0")
        {
        size = 0;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("filesize = 1")
        {
        size = 1;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("filesize = 133")
        {
        size = 133;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("filesize = 16384")
        {
        size = KILOBYTE*16;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("filesize = 33554432")
        {
        size = 33554432;
        highSize = dwBogus;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("filesize = 134217728")
        {
        size = 134217728;
        CreateFilledFile(lpFile1, size);
        file = FileOpen(lpFile1);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("hFile == bogus")
        {
        highSize = dwBogus;
        if((highSize=GetFileSize(hBogus, &highSize)) == -1)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(hBogus, &bigSize) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE;
    TESTCASE("closed handle")
        {
        highSize = dwBogus;
        CreateFilledFile(lpFile1, 133);
        file = FileOpen(lpFile1);
        CloseHandle(file);
        if((highSize=GetFileSize(file, &highSize)) == -1)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == FALSE)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // More successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("new file with nothing written")
        {
        size = 0;
        highSize = dwBogus;
        file = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("new file with some stuff written")
        {
        size = sizeof(DWORD);
        highSize = dwBogus;
        file = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        WriteFile(file, &highSize, size, &highSize, NULL);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });
    TESTCASE("new file with some stuff written and a flush")
        {
        size = sizeof(DWORD);
        highSize = dwBogus;
        file = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        WriteFile(file, &highSize, sizeof(DWORD), &highSize, NULL);
        FlushFileBuffers(file);
        if((highSize=GetFileSize(file, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(file, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(file); DeleteFile(lpFile1); });

    // TODO handle tests
        // ro file
        // device handle
        // find file handle
        // memory handle

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }
