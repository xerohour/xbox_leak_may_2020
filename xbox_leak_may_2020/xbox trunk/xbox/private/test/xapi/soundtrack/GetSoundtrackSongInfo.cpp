/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    GetSoundtrackSongInfo.cpp

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
#include "Soundtrack.h"

/*

Routine Description:

    Tests the GetSoundtrackSongInfo API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

Notes:
    typedef struct _XSOUNDTRACK_DATA
        {
        UINT    uSoundtrackId;
        UINT    uSongCount;
        UINT    uSoundtrackLength;
        WCHAR   szName[MAX_SOUNDTRACK_NAME];
        } XSOUNDTRACK_DATA, *PXSOUNDTRACK_DATA;

*/
DWORD Soundtrack::XGetSoundtrackSongInfo_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL
    XGetSoundtrackSongInfo(
        IN DWORD dwSoundtrackId,
        IN UINT uIndex,
        OUT PDWORD pdwSongId,
        OUT PDWORD pdwSongLength,
        OUT OPTIONAL PWSTR szNameBuffer,
        IN UINT uBufferSize
        );
    */
    xSetFunctionName(hLog, "XGetSoundtrackSongInfo");

    HANDLE hFind;
    XSOUNDTRACK_DATA data;
    DWORD songID;
    DWORD songLength;
    WCHAR *songname = new WCHAR[1024];
    if(!songname)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to allocate memory, all tests skipped");
        xEndVariation(hLog);
        return -1;
        }
    unsigned i;
    unsigned foundItems;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("All Songs")
        {
        hFind = XFindFirstSoundtrack(&data);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "XFindFirstSoundtrack (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "XFindFirstSoundtrack (ec: %ld)", GetLastError());
            break;
            }

        foundItems = 0;
        DebugPrint("  ---------------\r\n");
        do
            {
            ++foundItems;
            DebugPrint("  %2u %2u '%ws'\r\n", data.uSoundtrackId, data.uSongCount, data.szName);
            for(i=0; i<data.uSongCount; i++)
                {
                if(XGetSoundtrackSongInfo(data.uSoundtrackId, i, &songID, &songLength, songname, 1024) == TRUE)
                    {
                    DebugPrint("  %ws\r\n", songname);
                    TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
                    }
                else
                    {
                    DebugPrint("  Fail\r\n");
                    TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
                    }
                }
            } while(XFindNextSoundtrack(hFind, &data) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("szNameBuffer==NULL (uBufferSize==1024)")
        {
        if(XGetSoundtrackSongInfo(1, 0, &songID, &songLength, NULL, 1024) == TRUE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("szNameBuffer==NULL (uBufferSize==0)")
        {
        if(XGetSoundtrackSongInfo(1, 0, &songID, &songLength, NULL, 0) == TRUE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("uBufferSize==~0")
        {
        if(XGetSoundtrackSongInfo(1, 0, &songID, &songLength, songname, ~0) == TRUE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;




    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Invalid entry 0xEFFF.0xEFFF")
        {
        if(XGetSoundtrackSongInfo(0xEFFF, 0xEFFF, &songID, &songLength, songname, 1024) == FALSE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Invalid entry 1.0xEFFF")
        {
        if(XGetSoundtrackSongInfo(1, 0xEFFF, &songID, &songLength, songname, 1024) == FALSE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("Invalid entry 0xEFFF.0")
        {
        if(XGetSoundtrackSongInfo(0xEFFF, 0, &songID, &songLength, songname, 1024) == FALSE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    /*
    TESTCASE("pdwSongId == NULL ")
        {
        if(XGetSoundtrackSongInfo(1, 0, NULL, &songLength, songname, 1024) == FALSE)
            TESTPASS(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XGetSoundtrackSongInfo (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    */

    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("SOUNDTRACK(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 100000;
        unsigned failures = 0;
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }

            if(XGetSoundtrackSongInfo(1, 0, &songID, &songLength, songname, 1024) != TRUE)
                ++failures;
            }
        if(failures)
            TESTFAIL(hLog, "Mini stress failures: %u", failures);
        else
            TESTPASS(hLog, "Mini stress failures: %u", failures);
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    delete[] songname;
    return 0;
    }
