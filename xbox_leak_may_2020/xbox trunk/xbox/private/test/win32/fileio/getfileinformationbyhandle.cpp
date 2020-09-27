/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetFileInformationByHandle.cpp

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

    Tests the GetFileInformationByHandle API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetFileInformationByHandle_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL GetFileInformationByHandle(
      HANDLE hFile,                                  // handle to file 
      LPBY_HANDLE_FILE_INFORMATION lpFileInformation // buffer
    );
    */
    xSetFunctionName(hLog, "GetFileInformationByHandle");

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FGI01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%s:\\FGI02_%d.txt", BASE_DRIVE, ThreadID);
    unsigned i;

    CreateFilledFile(lpFile1, KILOBYTE);        // below tests rely on the exact file size
    CreateFilledFile(lpFile2, KILOBYTE*124);    // below tests rely on the exact file size

    HANDLE hFile = NULL;
    BY_HANDLE_FILE_INFORMATION fileInfo;

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("(NULL, NULL)")
        {
        if(GetFileInformationByHandle(hNull, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(INVALID_HANDLE_VALUE, NULL)")
        {
        if(GetFileInformationByHandle(INVALID_HANDLE_VALUE, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(NULL, valid)")
        {
        if(GetFileInformationByHandle(hNull, &fileInfo) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(valid, NULL)")
        {
        hFile = FileOpen(lpFile1);
        BUGGEDPASS(136, "ACCESS VIOLATION \"By Design\"");
        if(GetFileInformationByHandle(hFile, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN( { CloseHandle(hFile); } );
*/

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("valid file")
        {
        hFile = FileOpen(lpFile1);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to open file");
            }
        if(GetFileInformationByHandle(hFile, &fileInfo) == TRUE && fileInfo.nFileSizeLow == KILOBYTE)
            TESTPASS(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        else
            TESTFAIL(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        } ENDTESTCASE_CLEAN( { CloseHandle(hFile); } );
    TESTCASE("valid file 2")
        {
        hFile = FileOpen(lpFile2);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to open file");
            }
        if(GetFileInformationByHandle(hFile, &fileInfo) == TRUE && fileInfo.nFileSizeLow == KILOBYTE*124)
            TESTPASS(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        else
            TESTFAIL(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        } ENDTESTCASE_CLEAN( { CloseHandle(hFile); } );


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Closed Handle")
        {
        hFile = FileOpen(lpFile1);
        CloseHandle(hFile);
        if(GetFileInformationByHandle(hFile, &fileInfo) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    // handle tests
        // NULL
        // bogus address
        // valid file where nothing has been written
        // ro file (off cd/dvd)
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

    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    return 0;
    }
