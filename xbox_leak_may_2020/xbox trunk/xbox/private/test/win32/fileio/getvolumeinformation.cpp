/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetVolumeInformation.cpp

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

    Tests the GetVolumeInformation API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetVolumeInformation_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL GetVolumeInformation(
      LPCTSTR lpRootPathName,           // [in]  root directory
      LPTSTR lpVolumeNameBuffer,        // [out] volume name buffer
      DWORD nVolumeNameSize,            // [in]  length of name buffer
      LPDWORD lpVolumeSerialNumber,     // [out] volume serial number
      LPDWORD lpMaximumComponentLength, // [out] maximum file name length
      LPDWORD lpFileSystemFlags,        // [out] file system options
      LPTSTR lpFileSystemNameBuffer,    // [out] file system name buffer
      DWORD nFileSystemNameSize         // [in]  length of file system name buffer
    );
    */
    xSetFunctionName(hLog, "GetVolumeInformation");

    DWORD serialNum;
    DWORD maxFilename;
    DWORD flags;


    DWORD vnameSize = 128;
    DWORD fsnameSize = 128;
    char vnameBuffer[128];
    char fsnameBuffer[128];
    char lpFile1[80];


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NULL")
        {/*
        if(GetVolumeInformation(lpNull, NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());*/
        } ENDTESTCASE;
    TESTCASE("Empty Root, NULL")
        {
        if(GetVolumeInformation(lpEmpty, NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Valid Root Drive, NUL")
        {
        if(GetVolumeInformation(BASE_DRIVE ":\\", NULL, 0, NULL, NULL, NULL, NULL, 0) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Valid Root Drive")
        {
        if(GetVolumeInformation(BASE_DRIVE ":\\", vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Invalid Root Drive")
        {
        if(GetVolumeInformation("Fuzzy", NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \" \"")
        {
        if(GetVolumeInformation(" ", NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \":\\\"")
        {
        if(GetVolumeInformation(":\\", NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \"C\\\"")
        {
        if(GetVolumeInformation("C\\", NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \"CC:\\\"")
        {
        if(GetVolumeInformation("CC:\\", NULL, 0, NULL, NULL, NULL, NULL, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Valid Root Drive but small buffers")
        {
        sprintf(lpFile1, "%s:\\", BASE_DRIVE);
        if(GetVolumeInformation(lpFile1, vnameBuffer, 1, &serialNum, &maxFilename, &flags, fsnameBuffer, 1) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////


    // go through each drive and verify output based on actual xbox hardware
    /*
    char drives[] = "A:\\\0B:\\\0C:\\\0D:\\\0E:\\\0F:\\\0G:\\\0H:\\\0I:\\\0J:\\\0K:\\\0L:\\\0M:\\\0"
                     "N:\\\0O:\\\0P:\\\0Q:\\\0R:\\\0S:\\\0T:\\\0U:\\\0V:\\\0W:\\\0X:\\\0Y:\\\0Z:\\";
                     */
    char drives[] = "T:\\\0U:\\\0Z:\\";

    for(unsigned i=0; i<ARRAYSIZE(drives)/4; i++)
        {
        char testcase[64];
        sprintf(testcase, "Verify: %s", &drives[i*4]);
        TESTCASE(testcase)
            {
            if(GetVolumeInformation(&drives[i*4], vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize) == TRUE)
                {
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
                // TODO verify everything in here
                }
            else
                {
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
                }
            } ENDTESTCASE;
        }


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }

