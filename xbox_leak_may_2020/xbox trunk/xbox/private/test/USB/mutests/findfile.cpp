/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    FindFile.cpp

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

    Tests the FindFirstFile / FindNextFile / FindClose APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::FindFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL FindClose(
      HANDLE hFindFile   // file search handle
    );

    HANDLE FindFirstFile(
      LPCTSTR lpFileName,               // file name
      LPWIN32_FIND_DATA lpFindFileData  // data buffer
    );

    BOOL FindNextFile(
      HANDLE hFindFile,                // search handle 
      LPWIN32_FIND_DATA lpFindFileData // data buffer
    );
    */
    xSetFunctionName(hLog, "FindFile");

    WIN32_FIND_DATA findData;
    HANDLE hFind;
    unsigned i;

    char lpFile1[80];
    char lpFile2[80];


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("default:\\*")
        {
        sprintf(lpFile1, "%ws:\\*", BASE_DRIVE);
        hFind = FindFirstFile(lpFile1, &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        do
            {
            // TODO verify
            } while(FindNextFile(hFind, &findData) == TRUE);
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("\\x*")
        {
        sprintf(lpFile1, "%ws:\\xFFF_%d.txt", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, FALSE);
        sprintf(lpFile2, "%ws:\\x*", BASE_DRIVE);
        hFind = FindFirstFile(lpFile2, &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        do
            {
            // TODO verify
            } while(FindNextFile(hFind, &findData) == TRUE);
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
    TESTCASE("\\?*")
        {
        sprintf(lpFile2, "%ws:\\?*", BASE_DRIVE);
        hFind = FindFirstFile(lpFile2, &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        do
            {
            // TODO verify
            } while(FindNextFile(hFind, &findData) == TRUE);
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("one match \\FFF*")
        {
        sprintf(lpFile1, "%ws:\\FFF_%d", BASE_DRIVE, ThreadID);
        sprintf(lpFile2, "%ws:\\FFF_%d*", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, FALSE);
        hFind = FindFirstFile(lpFile2, &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });




    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    return 0;
    }