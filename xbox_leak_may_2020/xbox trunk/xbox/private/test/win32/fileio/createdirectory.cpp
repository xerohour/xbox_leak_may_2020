/*

Copyright (C) Microsoft Corporation.  All rights reserved.

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
#include "fileio_win32API.h"

/*

Routine Description:

    Tests the CreateDirectory API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    
*/
DWORD FILEIO::CreateDirectory_Test(HANDLE hLog, DWORD ThreadID)
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


    char lpFile1[80]; sprintf(lpFile1, BASE_DRIVE ":\\FCD01_%d", ThreadID);
    char lpFile1Sub1[80]; sprintf(lpFile1Sub1, BASE_DRIVE ":\\FCD01_%d\\FCD02_%d", ThreadID, ThreadID);
    char lpFile1Sub2[80]; sprintf(lpFile1Sub2, BASE_DRIVE ":\\FCD01_%d\\FCD03_%d", ThreadID, ThreadID);
    //char lpFile2[80]; sprintf(lpFile2, BASE_DRIVE ":\\FCD0_%d",  ThreadID);
    char lpFile3[80]; sprintf(lpFile3, BASE_DRIVE ":\\FCD04_%d", ThreadID);
    char lpTemp[80];

    // initialize system state
    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1Sub1);
    RemoveDirectory(lpFile1);
    //RemoveDirectory(lpFile2);
    RemoveDirectory(lpFile3);


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("lpPathName == NU")
        {
        if(CreateDirectory(lpNull, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("lpPathName == \"\"")
        {
        if(CreateDirectory(lpEmpty, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


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
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Creating \" \"")
        {
        if(CreateDirectory(" ", NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(" ");
        } ENDTESTCASE;
    TESTCASE("Recreating CreateSubDirectory")
        {
        // Note this test depends on a directory created above
        if(CreateDirectory(lpFile1Sub1, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(lpFile1Sub1);
        } ENDTESTCASE;
    TESTCASE("Recreating \\CreateDirectory")
        {
        // Note this test depends on a directory created above
        if(CreateDirectory(lpFile1, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(lpFile1);
        } ENDTESTCASE;
    /*
    TESTCASE("Recreating \"\\ CreateDirectory2\"")
        {
        // Note this test depends on a directory created above
        if(CreateDirectory(lpFile2, NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(lpFile2);
        } ENDTESTCASE;
        */
    TESTCASE("Creating " BASE_DRIVE ":\\")
        {
        if(CreateDirectory(BASE_DRIVE ":\\", NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        RemoveDirectory(BASE_DRIVE ":\\");
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Invalid characters
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(invalidFileNames); i++)
        {
        TESTCASE(invalidFileNames[i])
            {
            if(CreateDirectory(invalidFileNames[i], NULL) == FALSE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            } ENDTESTCASE;
        }


    //////////////////////////////////////////////////////////////////////////
    // MAXPATH limit
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Creating directories up to maxpath")
        {
        sprintf(buffer, "%s:", BASE_DRIVE);
        sprintf(lpTemp, "\\dir%06d", ThreadID);
        for(i=0; i<25; i++)
            {
            strcat(buffer + i*10, lpTemp);
            if(CreateDirectory(buffer, NULL) != TRUE)
                {
                if(i != 24)
                    {
                    TESTFAIL(hLog, "dir #%d (ec: %ld)", i, GetLastError());
                    break;
                    }
                }
            }
        if(i == 25) TESTPASS(hLog, "(ec: %ld)", GetLastError());
        for(; i>0; i--)
            {
            buffer[i*10+2] = 0;
            RemoveDirectory(buffer);
            }
        } ENDTESTCASE;


    TESTCASE("Creating 512 directories")
        {
        DWORD dirnum;
        DWORD err;

        CreateDirectory(BASE_DRIVE ":\\many", NULL);
        const unsigned int maxStress = 777;

        DebugPrint("'CreateDirectory'ing...\n");
        for(unsigned i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
            sprintf(buffer, "%s:\\many\\dir%lu", BASE_DRIVE, i);
            if(CreateDirectory(buffer, NULL) == FALSE)
                {
                err = GetLastError();
                break;
                }
            err = 0;
            }

        DebugPrint("'RemoveDirectory'ing...\n");
        do  {
            --i;
            if(i%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
            sprintf(buffer, "%s:\\many\\dir%lu", BASE_DRIVE, i);
            if(RemoveDirectory(buffer) == FALSE)
                {
                err = GetLastError();
                break;
                }
            err = 0;
            } while(i != 0);

        RemoveDirectory(BASE_DRIVE ":\\many");
        } ENDTESTCASE;

    //////////////////////////////////////////////////////////////////////////
    // 32000 limit
    //////////////////////////////////////////////////////////////////////////
    // this test takes a very very long time


    //////////////////////////////////////////////////////////////////////////
    // multiple directories
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("Creating as many directories as possible")
        {
        // this test takes a very very long time and should not be 
        // apart of the bvt

        DWORD dirnum;
        DWORD err;

        CreateDirectory(BASE_DRIVE ":\\many", NULL);

        DebugPrint("'CreateDirectory'ing...\n");
        for(dirnum=0; dirnum<(((DWORD)~0)-1); dirnum++)
            {
            if(dirnum%1000==0) DebugPrint("   (%d) %lu\n", ThreadID, dirnum);
            sprintf(buffer, "%s:\\many\\dir%lu", BASE_DRIVE, dirnum);
            if(CreateDirectory(buffer, NULL) == FALSE)
                {
                err = GetLastError();
                break;
                }
            err = 0;
            }

        DebugPrint("   (%d) Got to %lu with error: %lu\n", ThreadID, dirnum, err);

        DebugPrint("'RemoveDirectory'ing...\n");
        do  {
            --dirnum;
            if(dirnum%1000==0) DebugPrint("   (%d) %lu\n", ThreadID, dirnum);
            sprintf(buffer, "%s:\\many\\dir%lu", BASE_DRIVE, dirnum);
            if(RemoveDirectory(buffer) == FALSE)
                {
                err = GetLastError();
                break;
                }
            err = 0;
            } while(dirnum != 0);

        DebugPrint("   (%d) Got to %lu with error: %lu\n", ThreadID, dirnum, err);
        RemoveDirectory(BASE_DRIVE ":\\many");

        } ENDTESTCASE;
        */

    //////////////////////////////////////////////////////////////////////////
    // LPSECURITY_ATTRIBUTES
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("SA = {0,0,0}")
        {
        if(CreateDirectory(lpFile1, &saEmpty) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
    TESTCASE("SA = { 0xF000BAAA, (PISECURITY_DESCRIPTOR)0xF000BAAA, 0xF000BAAA};")
        {
        if(CreateDirectory(lpFile1, &saBogus) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
    TESTCASE("SA = { size, 0, 0 }")
        {
        if(CreateDirectory(lpFile1, &saEmptyWithSize) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
    TESTCASE("SA = { size, &sdEmpty, 0 }")
        {
        if(CreateDirectory(lpFile1, &saWithEmptySD) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
    TESTCASE("SA = { size, &sdBogus, 0 }")
        {
        if(CreateDirectory(lpFile1, &saWithBogusSD) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpFile1); });
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
