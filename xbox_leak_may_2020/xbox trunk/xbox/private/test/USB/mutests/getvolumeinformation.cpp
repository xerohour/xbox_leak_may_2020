/*

Copyright (c) 2000 Microsoft Corporation

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
#include "mutests.h"

/*

Routine Description:

    Tests the GetVolumeInformation API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::GetVolumeInformation_Test(HANDLE hLog, DWORD ThreadID)
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
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Valid Root Drive")
        {
        sprintf(lpFile1, "%ws:\\", BASE_DRIVE);
        if(GetVolumeInformation(lpFile1, vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    /*
    TESTCASE("Valid Root Drive (UNC style)")
        {
        sprintf(lpFile1, "\\\\?\\%ws:\\", BASE_DRIVE);
        if(GetVolumeInformation(lpFile1, vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize) == TRUE)
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

