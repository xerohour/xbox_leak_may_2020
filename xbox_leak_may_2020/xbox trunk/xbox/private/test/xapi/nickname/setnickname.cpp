/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    SetNickname.cpp

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
#include "Nickname.h"


/*

Routine Description:

    Tests the XSetNickname API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD Nickname::XSetNickname_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    BOOL
    XSetNicknameW(
        IN LPCWSTR lpNickname,
        IN BOOL fPreserveCase)
    */
    xSetFunctionName(hLog, "XSetNickname");

    WCHAR *buffer = new WCHAR[1024];
    unsigned i;

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("NULL")
        {
        if(XSetNickname(NULL, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
        */
    TESTCASE("Empty")
        {
        WCHAR *nick = L"";
        if(XSetNickname(nick, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Space")
        {
        WCHAR *nick = L" ";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("1 char")
        {
        WCHAR *nick = L"x";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("2 chars")
        {
        WCHAR *nick = L"ab";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("3 chars")
        {
        WCHAR *nick = L"JGP";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("characters")
        {
        WCHAR *nick = L"abcdefghijklmnopqrstuvwxyz";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("numbers")
        {
        WCHAR *nick = L"1234567890";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("jpoley")
        {
        WCHAR *nick = L"jpoley";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("§HådØW")
        {
        WCHAR *nick = L"§HådØW";
        if(XSetNickname(nick, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(nick)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("max-2 chars")
        {
        swprintf(buffer, L"%*ws", MAX_NICKNAME-2, L"");
        if(XSetNickname(buffer, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(buffer)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("max-1 chars")
        {
        swprintf(buffer, L"%*ws", MAX_NICKNAME-1, L"");
        if(XSetNickname(buffer, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(!CompareFirstNickname(buffer)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("max chars")
        {
        swprintf(buffer, L"%*ws", MAX_NICKNAME, L"");
        if(XSetNickname(buffer, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(CompareFirstNickname(buffer)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("max+1 chars")
        {
        swprintf(buffer, L"%*ws", MAX_NICKNAME+1, L"");
        if(XSetNickname(buffer, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(CompareFirstNickname(buffer)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    TESTCASE("max+2 chars")
        {
        swprintf(buffer, L"%*ws", MAX_NICKNAME+2, L"");
        if(XSetNickname(buffer, TRUE) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        if(CompareFirstNickname(buffer)) TESTFAIL(hLog, "Compare failed", GetLastError());
        } ENDTESTCASE;
    */

    //////////////////////////////////////////////////////////////////////////
    // Case sensitivity tests
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // Crazy Names
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(crazyNames); i++)
        {
        TESTCASE(crazyNames[i])
            {
            if(XSetNickname(crazyNames[i], TRUE) == TRUE)
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
        DebugPrint("NICKNAME(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 10000;
        unsigned failures = 0;
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }

            swprintf(buffer, L"nic%u", i);
            if(XSetNickname(buffer, TRUE) != TRUE)
                {
                ++failures;
                }
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


    //////////////////////////////////////////////////////////////////////////
    // Cleanup
    //////////////////////////////////////////////////////////////////////////


    delete[] buffer;

    return 0;
    }

