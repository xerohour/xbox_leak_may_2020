/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    MoveFile.cpp

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

    Tests the MoveFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::MoveFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL MoveFile(
      LPCTSTR lpExistingFileName, // file name
      LPCTSTR lpNewFileName       // new file name
    );
    */
    xSetFunctionName(hLog, "MoveFile");

    char lpSmallFile[80]; sprintf(lpSmallFile, "%ws:\\FMF01_%d.txt", BASE_DRIVE, ThreadID);
    char lpBigFile[80]; sprintf(lpBigFile, "%ws:\\FMF02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%ws:\\FMF03_%d.bla", BASE_DRIVE, ThreadID);
    char lpFakeFile2[80]; sprintf(lpFakeFile2, "%ws:\\FMF04_%d.bla", BASE_DRIVE, ThreadID);
    char lpDevice[80]; sprintf(lpDevice, "%ws:\\FMF05_%d.txt", BASE_DRIVE, ThreadID);
    char lpDir1[80]; sprintf(lpDir1, "%ws:\\FMF06_%d", BASE_DRIVE, ThreadID);
    char lpSubFile[80]; sprintf(lpSubFile, "%ws:\\FMF06_%d\\FMF07_%d.txt", BASE_DRIVE, ThreadID, ThreadID);
    char lpFile1[80];
    char lpFile2[80];
    unsigned i;

    // initialize system state
    CreateDirectory(lpDir1, NULL);
    CreateFilledFile(lpSmallFile, KILOBYTE*3);
    CreateFilledFile(lpBigFile, MEGABYTE*2);
    CreateFilledFile(lpSubFile, KILOBYTE*16);


    DWORD time = GetTickCount();



    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(valid file, nonexist)")
        {
        sprintf(lpFile1, "%ws:\\FMF08_%d.txt", BASE_DRIVE, ThreadID);
        if(MoveFile(lpSmallFile, lpFile1) == TRUE && FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        // the next case relies on this one
        } ENDTESTCASE;
    TESTCASE("(valid file, nonexist)")
        {
        // this case relies on the previous test
        if(MoveFile(lpFile1, lpSmallFile) == TRUE && FileExists(lpSmallFile))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(valid dir, nonexist)")
        {
        sprintf(lpFile1, "%ws:\\MoveFileDir2_%d.txt", BASE_DRIVE, ThreadID);
        sprintf(lpFile2, "%ws\\file_%d.txt", lpFile1, ThreadID);
        if(MoveFile(lpDir1, lpFile1) == TRUE && FileExists(lpFile1) && FileExists(lpFile2))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(valid dir, nonexist)")
        {
        if(MoveFile(lpFile1, lpDir1) == TRUE && FileExists(lpDir1) && FileExists(lpSubFile))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(file1, file1)")
        {
        if(MoveFile(lpSmallFile, lpSmallFile) == TRUE && FileExists(lpSmallFile) )
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("(valid file, existing)")
        {
        if(MoveFile(lpSmallFile, lpBigFile) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(valid dir, existing)")
        {
        if(MoveFile(lpDir1, lpBigFile) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(nonexist, nonexist)")
        {
        if(MoveFile(lpFakeFile, lpFakeFile2) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("(\\, nonexist)")
        {
        sprintf(lpFile1, "%ws:\\", BASE_DRIVE);
        sprintf(lpFile2, "%ws:\\backup", BASE_DRIVE);
        if(MoveFile(lpFile1, lpFile2) == FALSE)
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


    //////////////////////////////////////////////////////////////////////////
    // Cleanup
    //////////////////////////////////////////////////////////////////////////
    DeleteFile(lpSubFile);
    RemoveDirectory(lpDir1);
    DeleteFile(lpBigFile);
    DeleteFile(lpSmallFile);
    DeleteFile(lpDevice);

    return 0;
    }
