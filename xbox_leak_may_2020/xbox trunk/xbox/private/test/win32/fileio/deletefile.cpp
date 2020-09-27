/*

Copyright (C) Microsoft Corporation.  All rights reserved.

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
#include "fileio_win32API.h"

/*

Routine Description:

    Tests the DeleteFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::DeleteFile_Test(HANDLE hLog, DWORD ThreadID)
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
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("lpPathName == NU")
        {
        if(DeleteFile(lpNull) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("lpPathName == \"\"")
        {
        if(DeleteFile(lpNull) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Existing file \\DeleteFile.txt")
        {
        sprintf(lpFile1, "%s:\\FDF01_%d.txt", BASE_DRIVE, ThreadID);
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
        sprintf(lpFile1, "%s:\\FDF02_%d.txt", BASE_DRIVE, ThreadID);
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
        sprintf(lpFile1, "%s:\\%d.txt", BASE_DRIVE, ThreadID);
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


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("lpPathName == \" \"")
        {
        if(DeleteFile(lpSpace) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Missing file \\MissingDeleteFile.txt")
        {
        if(DeleteFile("C:\\MissingDeleteFile.txt") == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE(BASE_DRIVE ":\\")
        {
        if(DeleteFile(BASE_DRIVE ":\\") == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory \\DeleteFileDir")
        {
        sprintf(lpFile1, "%s:\\FDF03_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        if(DeleteFile(lpFile1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(lpFile1);
        } ENDTESTCASE;
    TESTCASE("Directory .")
        {
        if(DeleteFile(lpCurrentDir) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory ..")
        {
        if(DeleteFile(lpParentDir) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Invalid characters
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(invalidFileNames); i++)
        {
        TESTCASE(invalidFileNames[i])
            {
            if(DeleteFile(invalidFileNames[i]) == FALSE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }


    // TODO over max path and 32000

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }
