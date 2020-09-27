/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    FindSoundtrack.cpp

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

    Tests the XFindFirstSoundtrack / XFindNextSoundtrack / XFindClose APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

Notes:
    typedef struct {
        UINT    uSoundtrackId;
        UINT    uSongCount;
        WCHAR   szName[MAX_SOUNDTRACK_NAME];
        } XSOUNDTRACK_DATA, *PSOUNDTRACK_DATA;

*/
DWORD Soundtrack::XFindSoundtrack_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HANDLE
    XFindFirstSoundtrack(
        OUT PSOUNDTRACK_DATA Data
        );

    BOOL
    XFindNextSoundtrack(
        IN HANDLE FindHandle
        OUT PSOUNDTRACK_DATA Data
        );

    BOOL XFindClose(
        IN HANDLE hFind);
    */
    xSetFunctionName(hLog, "XFindSoundtrack");

    HANDLE hFind;
    XSOUNDTRACK_DATA data;
    unsigned i;
    unsigned foundItems;


    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("FindSoundtrack")
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
            } while(XFindNextSoundtrack(hFind, &data) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;




    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("XFindClose(NULL)")
        {
        if(XFindClose(hNull) == FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("XFindClose(INVALID_HANDLE_VALUE)")
        {
        if(XFindClose(INVALID_HANDLE_VALUE) == FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("XFindClose(bogus)")
        {
        if(XFindClose(hBogus) == FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("XFindClose the handle twice")
        {
        hFind = FindFirstNickname(FALSE, nickname, MAX_NICKNAME);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
        if(XFindClose(hFind) == FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());
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
