/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    SetEndOfFile.cpp

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

    Tests the SetEndOfFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::SetEndOfFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL SetEndOfFile(
      HANDLE hFile   // handle to file
    );
    */
    xSetFunctionName(hLog, "SetEndOfFile");

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FEO01_%d.txt", BASE_DRIVE, ThreadID);
    HANDLE hFile;
    DWORD size;
    DWORD highSize;

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NUL")
        {
        if(SetEndOfFile(hNull) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("INVALID_HANDLE_VALUE")
        {
        if(SetEndOfFile(INVALID_HANDLE_VALUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Empty File")
        {
        size = 0;
        CreateFilledFile(lpFile1, 0);
        hFile = FileOpenRW(lpFile1);

        if(SetEndOfFile(hFile) == TRUE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        if((highSize=GetFileSize(hFile, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("filesize 1, End 1")
        {
        size = 1;
        CreateFilledFile(lpFile1, 1);
        hFile = FileOpenRW(lpFile1);

        if((highSize=SetFilePointer(hFile, size, NULL, FILE_BEGIN)) == size)
            TESTPASS(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());

        if(SetEndOfFile(hFile) == TRUE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        if((highSize=GetFileSize(hFile, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("filesize 1, End 0")
        {
        size = 0;
        CreateFilledFile(lpFile1, 1);
        hFile = FileOpenRW(lpFile1);

        if((highSize=SetFilePointer(hFile, size, NULL, FILE_BEGIN)) == size)
            TESTPASS(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());

        if(SetEndOfFile(hFile) == TRUE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        if((highSize=GetFileSize(hFile, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("filesize 13meg, End 3meg")
        {
        size = MEGABYTE*3;
        CreateFilledFile(lpFile1, MEGABYTE*15);
        hFile = FileOpenRW(lpFile1);

        if((highSize=SetFilePointer(hFile, size, NULL, FILE_BEGIN)) == size)
            TESTPASS(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());

        if(SetEndOfFile(hFile) == TRUE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        if((highSize=GetFileSize(hFile, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Read Only file")
        {
        size = 512;
        CreateFilledFile(lpFile1, KILOBYTE);
        hFile = FileOpen(lpFile1);

        if((highSize=SetFilePointer(hFile, size, NULL, FILE_BEGIN)) == size)
            TESTPASS(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());

        if(SetEndOfFile(hFile) == FALSE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        if((highSize=GetFileSize(hFile, &highSize)) != size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("Closed Handle")
        {
        size = 512;
        CreateFilledFile(lpFile1, KILOBYTE);
        hFile = FileOpenRW(lpFile1);

        if((highSize=SetFilePointer(hFile, size, NULL, FILE_BEGIN)) == size)
            TESTPASS(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "SetFilePointer=%ld (ec: %ld)", highSize, GetLastError());

        CloseHandle(hFile);

        if(SetEndOfFile(hFile) == FALSE)
            TESTPASS(hLog, "SetEndOfFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "SetEndOfFile (ec: %ld)", GetLastError());

        hFile = FileOpenRW(lpFile1);

        if((highSize=GetFileSize(hFile, &highSize)) != size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        //DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 20; // this must be bigger than 8
        unsigned failures = 0;

        CreateFilledFile(lpFile1, KILOBYTE);
        hFile = FileOpenRW(lpFile1);

        for(unsigned i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                //DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }
            SetFilePointer(hFile, (i%8)*1024, NULL, FILE_BEGIN);
            SetEndOfFile(hFile);
            }
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    return 0;
    }
