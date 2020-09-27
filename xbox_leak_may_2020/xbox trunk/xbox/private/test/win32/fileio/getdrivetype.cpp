/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    GetDriveType.cpp

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

    Tests the GetDriveType API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::GetDriveType_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    UINT GetDriveType(
      LPCTSTR lpRootPathName   // root directory
    );
    */
    xSetFunctionName(hLog, "GetDriveType");

    UINT type;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NU")
        {
        type = GetDriveType(lpNull);
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Empty Root")
        {
        type = GetDriveType(lpEmpty);
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Valid Root Drive")
        {
        type = GetDriveType(BASE_DRIVE ":\\");
        if(type == DRIVE_FIXED)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Valid Root Drive (UNC style)")
        {
        type = GetDriveType("\\\\?\\" BASE_DRIVE ":\\");
        if(type == DRIVE_FIXED)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == " BASE_DRIVE ":")
        {
        type = GetDriveType(BASE_DRIVE ":");
        if(type == DRIVE_FIXED)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("C:\\")
        {
        type = GetDriveType("C:\\");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Invalid Root Drive")
        {
        type = GetDriveType("Fuzzy");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \" \"")
        {
        type = GetDriveType(" ");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \":\\\"")
        {
        type = GetDriveType(":\\");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \"C\\\"")
        {
        type = GetDriveType("C\\");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Root Drive == \"CC:\\\"")
        {
        type = GetDriveType("CC:\\");
        if(type == DRIVE_NO_ROOT_DIR)
            TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        else
            TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////


    // go through each drive and verify output based on actual xbox hardware
    Pchar drives = new char[64];
    unsigned size = GetLogicalDriveStrings(64, drives);

    for(unsigned i=0; i<size/4; i++)
        {
        char testcase[64];
        sprintf(testcase, "Verify: %s", &drives[i*4]);
        TESTCASE(testcase)
            {
            type = GetDriveType(&drives[i*4]);
            // TODO verify actual drive w/ expected
            if(type == DRIVE_REMOVABLE || type == DRIVE_FIXED || type == DRIVE_CDROM)
                {
                TESTPASS(hLog, "Type = %d (ec: %ld)", type, GetLastError());
                }
            else
                {
                TESTFAIL(hLog, "Type = %d (ec: %ld)", type, GetLastError());
                }
            } ENDTESTCASE;
        }

    delete[] drives;


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    return 0;
    }

