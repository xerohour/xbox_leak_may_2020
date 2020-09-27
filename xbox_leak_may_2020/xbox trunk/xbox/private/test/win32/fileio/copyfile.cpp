/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    CopyFile.cpp

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

    Tests the CopyFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD FILEIO::CopyFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL CopyFile(
      LPCTSTR lpExistingFileName, // name of an existing file
      LPCTSTR lpNewFileName,      // name of new file
      BOOL bFailIfExists          // operation if file exists
    );
    */
    xSetFunctionName(hLog, "CopyFile");

    char lpSmallFile[80]; sprintf(lpSmallFile, BASE_DRIVE ":\\FCP01_%d.txt", ThreadID);
    char lpBigFile[80]; sprintf(lpBigFile, BASE_DRIVE ":\\FCP02_%d.txt", ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, BASE_DRIVE ":\\FCP03_%d.bla", ThreadID);
    char lpFile1[80]; sprintf(lpFile1, BASE_DRIVE ":\\FCP04_%d.txt", ThreadID);
    char lpFile2[80]; sprintf(lpFile2, BASE_DRIVE ":\\FCP05_%d.txt", ThreadID);
    char lpFile3[80]; sprintf(lpFile3, BASE_DRIVE ":\\FCP06_%d.txt", ThreadID);
    char lpFileTemp1[80];
    unsigned i;

    // initialize system state
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);
    DeleteFile(lpFakeFile);
    DeleteFile(lpBigFile);
    DeleteFile(lpSmallFile);
    CreateFilledFile(lpSmallFile, KILOBYTE*16);
    CreateFilledFile(lpBigFile, MEGABYTE*75);

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(NULL, exists, true)")
        {
        if(CopyFile(lpNull, lpSmallFile, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, NULL, true)")
        {
        BUGGEDPASS(56, "ACCESS VIOLATION \"By Design\"");
        if(CopyFile(lpSmallFile, lpNull, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(NULL, exists, false)")
        {
        if(CopyFile(lpNull, lpSmallFile, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, NULL, false)")
        {
        BUGGEDPASS(56, "ACCESS VIOLATION \"By Design\"");
        if(CopyFile(lpSmallFile, lpNull, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(nonexists, NULL, false)")
        {
        if(CopyFile(lpFakeFile, lpNull, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(NULL, nonexists, false)")
        {
        if(CopyFile(lpNull, lpFakeFile, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(\"\", exists, true)")
        {
        if(CopyFile(lpEmpty, lpSmallFile, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, \"\", true)")
        {
        if(CopyFile(lpSmallFile, lpEmpty, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(\"\", exists, false)")
        {
        if(CopyFile(lpEmpty, lpSmallFile, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, \"\", false)")
        {
        if(CopyFile(lpSmallFile, lpEmpty, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(nonexists, \"\", false)")
        {
        if(CopyFile(lpFakeFile, lpEmpty, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(\"\", nonexists, false)")
        {
        if(CopyFile(lpEmpty, lpFakeFile, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(exists, nonexists, true)")
        {
        if(CopyFile(lpSmallFile, lpFile1, TRUE) == TRUE && FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, nonexists, false)")
        {
        if(CopyFile(lpSmallFile, lpFile2, FALSE) == TRUE && FileExists(lpFile2))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, exists, false)")
        {
        if(CopyFile(lpSmallFile, lpFile1, FALSE) == TRUE && FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(exists, exists, true)")
        {
        if(CopyFile(lpSmallFile, lpFile1, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(nonexists, nonexists, false)")
        {
        if(CopyFile(lpFakeFile, lpFile3, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(exists, nonexist\\nonexists, false)")
        {
        sprintf(lpFileTemp1, "%s:\\FCP\\FCP07_%d.txt", BASE_DRIVE, ThreadID);
        if(CopyFile(lpFakeFile, lpFileTemp1, FALSE) == FALSE)
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
            if(CopyFile(lpSmallFile, invalidFileNames[i], FALSE) == FALSE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }


    //////////////////////////////////////////////////////////////////////////
    // copy abc to abc
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(file1, file1, true)")
        {
        if(CopyFile(lpBigFile, lpBigFile, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(file1, file1, false)")
        {
        if(CopyFile(lpBigFile, lpBigFile, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(i=0; i<10; i++)
            {
            DebugPrint("   (%d) %02d%%\n", ThreadID, i*10);
            CopyFile(lpSmallFile, lpFile1, FALSE);
            CopyFile(lpBigFile, lpFile2, FALSE);
            }
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Cleanup
    //////////////////////////////////////////////////////////////////////////
    xSetFunctionName(hLog, "DeleteFile");
    TESTCASE("Removing CopyFile_file1.txt")
        {
        if(DeleteFile(lpFile1) == TRUE && !FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing CopyFile_file2.txt")
        {
        if(DeleteFile(lpFile2) == TRUE && !FileExists(lpFile2))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing CopyFile_file3.txt")
        {
        if(DeleteFile(lpFile3) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing non existant file")
        {
        if(DeleteFile(lpFakeFile) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing large file")
        {
        if(DeleteFile(lpBigFile) != FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Removing small file")
        {
        if(DeleteFile(lpSmallFile) != FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;



    return 0;
    }
