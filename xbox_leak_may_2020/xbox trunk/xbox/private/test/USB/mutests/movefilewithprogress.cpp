/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    MoveFileWithProgress.cpp

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

static DWORD CALLBACK MoveFileProgressRoutine(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData);


/*

Routine Description:

    Tests the MoveFileWithProgress API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::MoveFileWithProgress_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL MoveFileWithProgress(
      LPCTSTR lpExistingFileName,            // file name
      LPCTSTR lpNewFileName,                 // new file name
      LPPROGRESS_ROUTINE lpProgressRoutine,  // callback function
      LPVOID lpData,                         // parameter for callback
      DWORD dwFlags                          // move options
    );
    */
    xSetFunctionName(hLog, "MoveFileWithProgress");

    char lpSmallFile[80]; sprintf(lpSmallFile, "%ws:\\FMP01_%d.txt", BASE_DRIVE, ThreadID);
    char lpBigFile[80]; sprintf(lpBigFile, "%ws:\\FMP02_%d.txt", BASE_DRIVE, ThreadID);
    char lpExtraBigFile[80]; sprintf(lpExtraBigFile, "%ws:\\FMP03_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%ws:\\FMP04_%d.bla", BASE_DRIVE, ThreadID);
    char lpFakeFile2[80]; sprintf(lpFakeFile2, "%ws:\\FMP05_%d.bla", BASE_DRIVE, ThreadID);
    char lpDevice[80]; sprintf(lpDevice, "%ws:\\FMP06_%d.txt", BASE_DRIVE, ThreadID);
    char lpDir1[80]; sprintf(lpDir1, "%ws:\\FMP07_%d", BASE_DRIVE, ThreadID);
    char lpSubFile[80]; sprintf(lpSubFile, "%ws:\\FMP07_%d\\FMP08_%d.txt", BASE_DRIVE, ThreadID, ThreadID);
    char lpFile1[80];
    char lpFile2[80];
    unsigned i;

    // initialize system state
    CreateDirectory(lpDir1, NULL);
    CreateFilledFile(lpSmallFile, KILOBYTE*2);
    CreateFilledFile(lpBigFile, MEGABYTE*2);
    CreateFilledFile(lpExtraBigFile, MEGABYTE*3);
    CreateFilledFile(lpSubFile, KILOBYTE*16);


    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Small file replace")
        {
        sprintf(lpFile1, "%ws:\\FMP09_%d.txt", BASE_DRIVE, ThreadID);

        if(MoveFileWithProgress(lpSmallFile, lpFile1, MoveFileProgressRoutine, hLog, MOVEFILE_REPLACE_EXISTING) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpFile1) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
        // the next case relies on this one
        } ENDTESTCASE;
    TESTCASE("Small file")
        {
        // this case relies on the previous test
        if(MoveFileWithProgress(lpFile1, lpSmallFile, MoveFileProgressRoutine, hLog, 0) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpSmallFile) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Big file replace")
        {
        sprintf(lpFile1, "%ws:\\FMP09_%d.txt", BASE_DRIVE, ThreadID);

        if(MoveFileWithProgress(lpBigFile, lpFile1, MoveFileProgressRoutine, hLog, MOVEFILE_REPLACE_EXISTING) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpFile1) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
        // the next case relies on this one
        } ENDTESTCASE;
    TESTCASE("Big file")
        {
        // this case relies on the previous test
        if(MoveFileWithProgress(lpFile1, lpBigFile, MoveFileProgressRoutine, hLog, 0) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpBigFile) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Extra Big file replace")
        {
        sprintf(lpFile1, "%ws:\\FMP09_%d.txt", BASE_DRIVE, ThreadID);

        if(MoveFileWithProgress(lpExtraBigFile, lpFile1, MoveFileProgressRoutine, hLog, MOVEFILE_REPLACE_EXISTING) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpFile1) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
        // the next case relies on this one
        } ENDTESTCASE;
    TESTCASE("Extra Big file w/write through")
        {
        // this case relies on the previous test
        if(MoveFileWithProgress(lpFile1, lpExtraBigFile, MoveFileProgressRoutine, hLog, MOVEFILE_WRITE_THROUGH) == TRUE)
            TESTPASS(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "MoveFileWithProgress (ec: %ld)", GetLastError());

        if(FileExists(lpExtraBigFile) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "File doesn't exist (ec: %ld)", GetLastError());
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
    DeleteFile(lpExtraBigFile);
    DeleteFile(lpSmallFile);
    DeleteFile(lpDevice);

    return 0;
    }

/*

Routine Description:

    Callback function used with the CopyFileEx and MoveFileWithProgress functions
    See "CopyProgressRoutine" in MSDN

Arguments:

    LARGE_INTEGER TotalFileSize          - file size
    LARGE_INTEGER TotalBytesTransferred  - bytes transferred
    LARGE_INTEGER StreamSize             - bytes in stream
    LARGE_INTEGER StreamBytesTransferred - bytes transferred for stream
    DWORD dwStreamNumber                 - current stream
    DWORD dwCallbackReason               - callback reason
    HANDLE hSourceFile                   - handle to source file
    HANDLE hDestinationFile              - handle to destination file
    LPVOID lpData                        - user data, HANDLE to XLOG object

Return Value:

    PROGRESS_CONTINUE, PROGRESS_STOP, or PROGRESS_CANCEL

*/
static DWORD CALLBACK MoveFileProgressRoutine(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred, LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred, DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData)
    {
    HANDLE hLog = (HANDLE)lpData;

    xLog(hLog, XLL_INFO, "%I64d of %I64d", TotalBytesTransferred.QuadPart, TotalFileSize.QuadPart);

    if(TotalBytesTransferred.QuadPart > TotalFileSize.QuadPart)
        {
        xLog(hLog, XLL_FAIL, "Transfered more than filesize: %I64d of %I64d", TotalBytesTransferred.QuadPart, TotalFileSize.QuadPart);
        }

    if(TotalFileSize.QuadPart == TotalBytesTransferred.QuadPart)
        {
        }

    return PROGRESS_CONTINUE;
    }
