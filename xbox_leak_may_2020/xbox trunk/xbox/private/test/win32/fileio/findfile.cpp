/*

Copyright (C) Microsoft Corporation.  All rights reserved.

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
#include "fileio_win32API.h"

/*

Routine Description:

    Tests the FindFirstFile / FindNextFile / FindClose APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::FindFile_Test(HANDLE hLog, DWORD ThreadID)
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

    // TODO verify data in fileData

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE(TITLE_DRIVE ":\\*.*")
        {
        sprintf(lpFile1, "%s:\\*.*", TITLE_DRIVE);
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
        sprintf(lpFile1, "%s:\\FFF01_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, FALSE);
        sprintf(lpFile2, "%s:\\F*", BASE_DRIVE);
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
    TESTCASE("one match \\FFF*.*")
        {
        sprintf(lpFile1, "%s:\\FFF02_%d", BASE_DRIVE, ThreadID);
        sprintf(lpFile2, "%s*.*", lpFile1);
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
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("C:\\*")
        {
        hFind = FindFirstFile("C:\\*", &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        do
            {
            // TODO verify
            } while(FindNextFile(hFind, &findData) == TRUE);
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("no match \\qrz")
        {
        sprintf(lpFile1, "%s:\\qrz", BASE_DRIVE);
        hFind = FindFirstFile(lpFile1, &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("\".\"")
        {
        hFind = FindFirstFile(".", &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("\"..\"")
        {
        hFind = FindFirstFile("..", &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("..\\*.*")
        {
        hFind = FindFirstFile("..\\*.*", &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE(".\\*.*")
        {
        hFind = FindFirstFile(".\\*.*", &findData);
        if(hFind == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("FindFirst, FindClose, FindNext")
        {
        BUGGEDPASS(61, "Crash \"By Design\"");

        hFind = FindFirstFile("Z:\\*.*", &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("FindFirst, CloseHandle, FindNext, FindClose")
        {
        BUGGEDPASS(62, "Crash \"By Design\"");

        hFind = FindFirstFile("Z:\\*.*", &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(CloseHandle(hFind) != FALSE)
            TESTPASS(hLog, "CloseHandle (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CloseHandle (ec: %ld)", GetLastError());
        if(FindNextFile(hFind, &findData) == FALSE)
            TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;

    // Find Close tests
    /*
    TESTCASE("FindClose(NULL)")
        {
        if(FindClose(hNull) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("FindClose(INVALID_HANDLE_VALUE)")
        {
        if(FindClose(INVALID_HANDLE_VALUE) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("FindClose(bogus)")
        {
        if(FindClose(hBogus) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
        */
    TESTCASE("FileClose the handle twice")
        {
        BUGGEDPASS(63, "Crash \"By Design\"");

        hFind = FindFirstFile(BASE_DRIVE ":\\*", &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        if(FindClose(hFind) == FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Invalid characters
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(invalidFileNames); i++)
        {
        if(strstr(invalidFileNames[i], BASE_DRIVE ":\\<") != 0)
            {
            // This test should pass because of undocumented "feature" where '<' == '*'
            continue;
            }
        else if(strstr(invalidFileNames[i], BASE_DRIVE ":\\>") != 0)
            {
            // This test should pass because of undocumented "feature" where '<' == '*'
            continue;
            }
        else if(strstr(invalidFileNames[i], BASE_DRIVE ":\\*") != 0)
            {
            continue;
            }
        else if(strstr(invalidFileNames[i], BASE_DRIVE ":\\?") != 0)
            {
            continue;
            }
        TESTCASE(invalidFileNames[i])
            {
            hFind = FindFirstFile(invalidFileNames[i], &findData);
            if(hFind == INVALID_HANDLE_VALUE)
                TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
            if(FindNextFile(hFind, &findData) == FALSE)
                TESTPASS(hLog, "FindNextFile (ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "FindNextFile (ec: %ld)", GetLastError());
            if(FindClose(hFind) == FALSE)
                TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }



    // FindNextFile
        // handle tests
            // NULL
            // bogus address
            // valid file handle
            // closed file handle
            // ro file
            // device handle
            // valid find file handle
            // closed find file handle (using FindClose)
            // closed file handle (using CloseHandle)
            // memory handle
        // LPWIN32_FIND_DATA
            // NULL
            // bogus address
        // find next when only 0 files match
        // find next when only 1 file matches
        // find next when only many files match
        // create a file that matches the criteria after i start searching
        // delete a file that matches the criteria after i start searching
        // verify LPWIN32_FIND_DATA is zeroed


    // FindFirstFile
        // filename tests
            // NULL
            // ""
            // filename/directories with spaces, etc.
            // * and ? wild cards
            // maxpath, 32000
            // use mixed / and \
            // spaces
            // .
            // ..
            // with < > : " / \ |
            // using device names: aux, con, lpt1, and prn
            // unc: \\?\UNC\poleyland1\public\tests
            // case insensitivity
        // LPWIN32_FIND_DATA
            // NULL
            // bogus address
        // find first when only 0 files match
        // find first when only 1 file matches
        // find first when only many files match


    // FindClose
        // handle tests
            // NULL
            // bogus address
            // valid file handle
            // closed file handle
            // ro file
            // device handle
            // valid find file handle
            // closed find file handle (using FindClose)
            // closed file handle (using CloseHandle)
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