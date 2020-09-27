/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    DeleteFile.cpp

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

    Tests the DeleteFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::DeleteFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL DeleteFile(
      LPCTSTR lpFileName   // file name
    );
    */
    xSetFunctionName(hLog, "DeleteFile");

    char lpFile1[80];
    unsigned i;

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Existing file \\DeleteFile.txt")
        {
        sprintf(lpFile1, "%ws:\\FDF01_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpFile1);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        if(DeleteFile(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Existing large file \\DeleteFile_bigfile.txt")
        {
        sprintf(lpFile1, "%ws:\\FDF02_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpFile1, MEGABYTE*5);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        if(DeleteFile(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Existing file \\a")
        {
        sprintf(lpFile1, "%ws:\\%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpFile1, KILOBYTE);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        if(DeleteFile(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Existing file 8.3")
        {
        sprintf(lpFile1, "%ws:\\aaaaaa_%d.zzz", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpFile1, 1);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        if(DeleteFile(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }
