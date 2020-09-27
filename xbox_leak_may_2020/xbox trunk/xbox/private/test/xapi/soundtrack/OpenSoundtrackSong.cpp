/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XOpenSoundtrackSong.cpp

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

    Tests the XOpenSoundtrackSong API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

*/
DWORD Soundtrack::XOpenSoundtrackSong_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HANDLE
    XOpenSoundtrackSong(
        IN DWORD dwSongId,
        IN BOOL fAsyncMode
        );
    */
    xSetFunctionName(hLog, "XOpenSoundtrackSong");

    HANDLE hFile;
    HANDLE hFind;
    XSOUNDTRACK_DATA data;
    DWORD songID;
    DWORD songLength;
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
                XGetSoundtrackSongInfo(data.uSoundtrackId, i, &songID, &songLength, NULL, 0);
                hFile = XOpenSoundtrackSong(songID, FALSE);
                if(hFile != INVALID_HANDLE_VALUE)
                    {
                    TESTPASS(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());
                    CloseHandle(hFile);
                    }
                else
                    TESTFAIL(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());

                hFile = XOpenSoundtrackSong(songID, TRUE);
                if(hFile != INVALID_HANDLE_VALUE)
                    {
                    TESTPASS(hLog, "XOpenSoundtrackSong Async (ec: %ld)", GetLastError());
                    CloseHandle(hFile);
                    }
                else
                    TESTFAIL(hLog, "XOpenSoundtrackSong Async (ec: %ld)", GetLastError());
                }
            } while(XFindNextSoundtrack(hFind, &data) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Invalid dwSongId ~0")
        {
        hFile = XOpenSoundtrackSong(~0, FALSE);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("Invalid dwSongId 0xEFFF")
        {
        hFile = XOpenSoundtrackSong(0xEFFF, FALSE);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "XOpenSoundtrackSong Non-Async (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;

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

            hFile = XOpenSoundtrackSong(songID, FALSE);
            if(hFile == INVALID_HANDLE_VALUE)
                ++failures;
            else
                CloseHandle(hFile);
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

    return 0;
    }
