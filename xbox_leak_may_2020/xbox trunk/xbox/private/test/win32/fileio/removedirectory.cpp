/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    RemoveDirectory.cpp

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

    Tests the RemoveDirectory API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    
*/
DWORD FILEIO::RemoveDirectory_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL RemoveDirectory(
      LPCTSTR lpPathName   // directory name
    );
    */
    xSetFunctionName(hLog, "RemoveDirectory");

    char lpFile1[256];
    char lpFile2[256];
    unsigned i;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Removing NU")
        {
        if(RemoveDirectory(lpNull) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing \"\"")
        {
        if(RemoveDirectory(lpEmpty) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Removing \\RemoveDirectory")
        {
        sprintf(lpFile1, "%s:\\FRD01_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing SubRemoveDirectory")
        {
        sprintf(lpFile1, "%s:\\FRD02_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }

        sprintf(lpFile1, "%s:\\FRD02_%d\\FRD03_%d", BASE_DRIVE, ThreadID, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }

        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        sprintf(lpFile1, "%s:\\FRD02_%d", BASE_DRIVE, ThreadID);
        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    /*
    TESTCASE("Removing \\ RemoveDirectory2")
        {
        sprintf(lpFile1, "%s:\\ RemoveDirectory2_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
*/

    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Removing \\MissingRemoveDirectory")
        {
        sprintf(lpFile1, "%s:\\FRD04", BASE_DRIVE, ThreadID);
        if(RemoveDirectory(lpFile1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing lpSmallFile")
        {
        sprintf(lpFile1, "%s:\\FRD05_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpFile1, KILOBYTE*16);
        if(RemoveDirectory(lpFile1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });
    TESTCASE("Removing .")
        {
        if(RemoveDirectory(lpCurrentDir) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing ..")
        {
        if(RemoveDirectory(lpParentDir) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing " BASE_DRIVE ":\\")
        {
        sprintf(lpFile1, "%s:\\", BASE_DRIVE);
        if(RemoveDirectory(lpFile1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing Nonempty directory")
        {
        sprintf(lpFile1, "%s:\\FRD06_%d", BASE_DRIVE, ThreadID);
        sprintf(lpFile2, "%s:\\FRD06_%d\\FRD07_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        CreateDirectory(lpFile2, NULL);
        if(!FileExists(lpFile2))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        if(RemoveDirectory(lpFile1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        // clean up
        if(RemoveDirectory(lpFile2) == TRUE)
            TESTPASS(hLog, "Cleanup subdir (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Cleanup subdir (ec: %ld)", GetLastError());
        if(RemoveDirectory(lpFile1) == TRUE)
            TESTPASS(hLog, "Cleanup dir (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Cleanup dir (ec: %ld)", GetLastError());
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Invalid characters
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(invalidFileNames); i++)
        {
        TESTCASE(invalidFileNames[i])
            {
            if(RemoveDirectory(invalidFileNames[i]) == FALSE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }


    // over max path and 32000
    // messing with naming convention
        // use mixed / and \
        // spaces
        // .
        // ..
        // with < > : " / \ |
        // unc: \\?\UNC\poleyland1\public\tests
        // case insensitivity

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }
