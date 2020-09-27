/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetDiskFreeSpaceEx.cpp

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

    Tests the GetDiskFreeSpaceEx API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetDiskFreeSpaceEx_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL GetDiskFreeSpaceEx(
      LPCTSTR lpDirectoryName,                 // directory name
      PULARGE_INTEGER lpFreeBytesAvailable,    // bytes available to caller
      PULARGE_INTEGER lpTotalNumberOfBytes,    // bytes on disk
      PULARGE_INTEGER lpTotalNumberOfFreeBytes // free bytes on disk
    );
    */
    xSetFunctionName(hLog, "GetDiskFreeSpaceEx");

    ULARGE_INTEGER available;
    ULARGE_INTEGER total;
    ULARGE_INTEGER free;
    unsigned i;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("GetDiskFreeSpaceEx(NULL, NULL, NULL, NULL)")
        {/*
        if(GetDiskFreeSpaceEx(lpNull, NULL, NULL, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());*/
        } ENDTESTCASE;
    TESTCASE("GetDiskFreeSpaceEx(\"\", NULL, NULL, NULL)")
        {
        if(GetDiskFreeSpaceEx(lpEmpty, NULL, NULL, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GetDiskFreeSpaceEx(\" \", NULL, NULL, NULL)")
        {
        if(GetDiskFreeSpaceEx(lpSpace, NULL, NULL, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("lpPathName == \"" CACHE_DRIVE ":\\\"")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx(CACHE_DRIVE ":\\", &available, &total, &free) == TRUE && available.QuadPart < total.QuadPart && available.QuadPart != ulBogus.QuadPart)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;
    TESTCASE("lpPathName == \"" TITLE_DRIVE ":\\\"")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx(TITLE_DRIVE ":\\", &available, &total, &free) == TRUE && available.QuadPart < total.QuadPart && available.QuadPart != ulBogus.QuadPart)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;
    TESTCASE("lpPathName == \"" USER_DRIVE ":\\\"")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx(USER_DRIVE ":\\", &available, &total, &free) == TRUE && available.QuadPart < total.QuadPart && available.QuadPart != ulBogus.QuadPart)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("lpPathName == \"c:\\\"")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx("c:\\", &available, &total, &free) == FALSE)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;
    TESTCASE("lpPathName == .")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx(".", &available, &total, &free) == FALSE)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;
    TESTCASE("lpPathName == \\")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx("\\", &available, &total, &free) == FALSE)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        } ENDTESTCASE;
    TESTCASE("(\"\\\", &a, &a, &a)")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx("\\", &available, &available, &available) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("lpPathName == \"y:\\\"")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx("y:\\", &available, &total, &free) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("lpPathName == GetDiskFreeSpaceEx")
        {
        available = total = free = ulBogus;
        if(GetDiskFreeSpaceEx("GetDiskFreeSpaceEx", &available, &total, &free) == FALSE)
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
            available = total = free = ulBogus;
            if(GetDiskFreeSpaceEx(invalidFileNames[i], &available, &total, &free) == FALSE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        //DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(i=0; i<100; i++)
            {
            //DebugPrint("   (%d) %02d%%\n", ThreadID, i*10);
            available = total = free = ulBogus;
            GetDiskFreeSpaceEx(BASE_DRIVE ":\\", &available, &total, &free);
            }
        } ENDTESTCASE_CLEAN({ /*DebugPrint("   (%d) 100%%\n", ThreadID);*/ });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    
    return 0;
    }
