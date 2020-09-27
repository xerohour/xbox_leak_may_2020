/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    CreateDirectory.cpp

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

    Tests the CreateDirectory API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    
*/
DWORD MUTests::CreateDirectory_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL CreateDirectory(
      LPCTSTR lpPathName,                         // directory name
      LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
    );
    */
    xSetFunctionName(hLog, "CreateDirectory");

    unsigned i=0;
    char *buffer = new char[65536];
    if(!buffer)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }


    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FCD01_%d", BASE_DRIVE, ThreadID);
    char lpFile1Sub1[80]; sprintf(lpFile1Sub1, "%ws:\\FCD01_%d\\FCD03_%d", BASE_DRIVE, ThreadID, ThreadID);
    char lpFile1Sub2[80]; sprintf(lpFile1Sub2, "%ws:\\FCD01_%d\\FCD04_%d", BASE_DRIVE, ThreadID, ThreadID);
    //char lpFile2[80]; sprintf(lpFile2, "%ws:\\ CreateDirectory2_%d",  BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%ws:\\md_%d", BASE_DRIVE, ThreadID);
    char lpTemp[80];

    // initialize system state
    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1);
    //RemoveDirectory(lpFile2);
    RemoveDirectory(lpFile3);


    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Creating CreateDirectory")
        {
        if(CreateDirectory(lpFile1, NULL) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        // this directory is used below
        } ENDTESTCASE;
    TESTCASE("Creating CreateSubDirectory")
        {
        if(CreateDirectory(lpFile1Sub1, NULL) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        // this directory is used below
        } ENDTESTCASE;
    /*
    TESTCASE("Creating \"\\ CreateDirectory2\"")
        {
        if(CreateDirectory(lpFile2, NULL) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        // this directory is used below
        } ENDTESTCASE;
    TESTCASE("Creating \\CreateDirectory\\.\\..\\.\\CreateDirectory4")
        {
        // Note this test depends on a directory created above
        if(CreateDirectory(lpFile1Sub2, NULL) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(lpFile1Sub2);
        } ENDTESTCASE;
*/


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    delete[] buffer;

    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1);
    //RemoveDirectory(lpFile2);
    RemoveDirectory(lpFile3);

    return 0;
    }
