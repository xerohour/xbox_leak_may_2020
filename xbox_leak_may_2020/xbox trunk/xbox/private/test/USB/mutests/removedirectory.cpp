/*

Copyright (c) 2000 Microsoft Corporation

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
#include "mutests.h"

/*

Routine Description:

    Tests the RemoveDirectory API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    
*/
DWORD MUTests::RemoveDirectory_Test(HANDLE hLog, DWORD ThreadID)
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
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Removing \\RemoveDirectory")
        {
        sprintf(lpFile1, "%ws:\\FRD01_%d", BASE_DRIVE, ThreadID);
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
        sprintf(lpFile1, "%ws:\\FRD02_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }

        sprintf(lpFile1, "%ws:\\FRD02_%d\\FRD03_%d", BASE_DRIVE, ThreadID, ThreadID);
        CreateDirectory(lpFile1, NULL);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial directory");
            }

        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        sprintf(lpFile1, "%ws:\\FRD02_%d", BASE_DRIVE, ThreadID);
        if(RemoveDirectory(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    /*
    TESTCASE("Removing \\ RemoveDirectory2")
        {
        sprintf(lpFile1, "%ws:\\ RemoveDirectory2_%d", BASE_DRIVE, ThreadID);
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
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }
