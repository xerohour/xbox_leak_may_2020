/*

Copyright (c) 2000 Microsoft Corporation

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
#include "mutests.h"

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
DWORD MUTests::CopyFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL CopyFile(
      LPCTSTR lpExistingFileName, // name of an existing file
      LPCTSTR lpNewFileName,      // name of new file
      BOOL bFailIfExists          // operation if file exists
    );
    */
    xSetFunctionName(hLog, "CopyFile");

    char lpSmallFile[80]; sprintf(lpSmallFile, "%ws:\\FCP01_%d.txt", BASE_DRIVE, ThreadID);
    char lpBigFile[80]; sprintf(lpBigFile, "%ws:\\FCP02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%ws:\\FCP03_%d.bla", BASE_DRIVE, ThreadID);
    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FCP04_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%ws:\\FCP05_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%ws:\\FCP06_%d.txt", BASE_DRIVE, ThreadID);
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
    CreateFilledFile(lpBigFile, MEGABYTE*3);

    DWORD time = GetTickCount();




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
        sprintf(lpFileTemp1, "%ws:\\CopyFile_nonexistdir\\CopyFile_file4_%d.txt", BASE_DRIVE, ThreadID);
        if(CopyFile(lpFakeFile, lpFileTemp1, FALSE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


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
        DebugPrint("MUTESTS(%d): This test takes a long time to run...\n", ThreadID);
        for(i=0; i<10; i++)
            {
            DebugPrint("   (%d) %02d%%\n", ThreadID, i*10);
            CopyFile(lpSmallFile, lpFile1, FALSE);
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
