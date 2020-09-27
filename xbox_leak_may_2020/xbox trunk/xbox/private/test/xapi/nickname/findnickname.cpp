/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    FindNickname.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

    #define NUM_NICK_ENTRIES 50
    #define MAX_NICKNAME 32
*/

#include "stdafx.h"
#include "commontest.h"
#include "Nickname.h"

bool InCollection(WCHAR *collection[], unsigned collectionSize, WCHAR *name)
    {
    for(unsigned i=0; i<collectionSize; i++)
        {
        if(_wcsicmp(collection[i], name) == 0) return true;
        }

    return false;
    }

unsigned NumItemsOfSize(WCHAR *collection[], unsigned collectionSize, unsigned size)
    {
    unsigned count = 0;

    for(unsigned i=0; i<collectionSize; i++)
        {
        if(wcslen(collection[i]) <= size) ++count;
        }

    return count;
    }

/*

Routine Description:

    Tests the XFindFirstNickname / XFindNextNickname / XFindClose APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

*/
DWORD Nickname::XFindNickname_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HANDLE XFindFirstNicknameW(
        IN BOOL fThisTitleOnly,
        OUT LPWSTR lpNickname,
        IN UINT uSize
        );

    BOOL XFindNextNicknameW(
        IN HANDLE hFindNickname,
        OUT LPWSTR lpNickname,
        IN UINT uSize);

    BOOL XFindClose(
        IN HANDLE hFind);
    */
    xSetFunctionName(hLog, "XFindNickname");

    WCHAR nickname[MAX_NICKNAME+1];
    HANDLE hFind;
    unsigned i;
    unsigned foundItems;

    WCHAR* names[] =
        {
        L"JP1",
        L"jpoley",
        L"Shadow",
        L"QuestGod",
        L"MAX_IDENTITYNAME78901234567890C",
        L"Snoopy",
        L"Dapple",
        L"JP2",
        L" ",
        L"MAX_IDENTITYNAME78901234567890D",
        L"Sam",
        L"/.",
        L"XBOX",
        L"1",
        L"Chow Yun-Fat",
        L"EastsideKILLA",
        L"Neo",
        L"atg-scorto",
        L"MAX_IDENTITYNAME78901234567890A",
        L"MAX_IDENTITYNAME78901234567890B",
        L"MAX_IDENTITYNAME78901234567890E",
        L"MAX_IDENTITYNAME78901234567890F",
        L"1jp",
        L"a"
        };

    // clear & then populate the cache with our test data
    TESTCASE("Populating Nickname file")
        {
        if(RemoveNicknameFile() == false)
            TESTFAIL(hLog, "DeleteFile(NICKNAME.XBN) failed (ec: %lu)", GetLastError());

        for(i=0; i<ARRAYSIZE(names); i++)
            {
            if(XSetNickname(names[i], FALSE) == FALSE)
                TESTFAIL(hLog, "XSetNickname (ec: %ld)", GetLastError());
            }
        } ENDTESTCASE;

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("FindNickname length of MAX_NICKNAME")
        {
        unsigned length = MAX_NICKNAME;
        unsigned collectionSize = NumItemsOfSize(names, ARRAYSIZE(names), length);
        hFind = XFindFirstNickname(FALSE, nickname, length+1);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
            break;
            }

        foundItems = 0;
        DebugPrint("  ---------------\r\n", nickname);
        do
            {
            ++foundItems;
            DebugPrint("  '%ws'\r\n", nickname);
            if(!InCollection(names, ARRAYSIZE(names), nickname))
                TESTFAIL(hLog, "Found name is not in our collection (%ws)", nickname);
            } while(XFindNextNickname(hFind, nickname, length+1) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());

        if(foundItems == collectionSize)
            TESTPASS(hLog, "FoundCount[%u] == collectionSize[%u]", foundItems, collectionSize);
        else
            TESTFAIL(hLog, "FoundCount[%u] != collectionSize[%u]", foundItems, collectionSize);
        } ENDTESTCASE;

    TESTCASE("FindNickname length of 1")
        {
        unsigned length = 1;
        unsigned collectionSize = NumItemsOfSize(names, ARRAYSIZE(names), length);
        hFind = XFindFirstNickname(FALSE, nickname, length+1);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
            break;
            }

        foundItems = 0;
        DebugPrint("  ---------------\r\n", nickname);
        do
            {
            ++foundItems;
            DebugPrint("  '%ws'\r\n", nickname);
            if(!InCollection(names, ARRAYSIZE(names), nickname))
                TESTFAIL(hLog, "Found name is not in our collection (%ws)", nickname);
            } while(XFindNextNickname(hFind, nickname, length+1) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());

        if(foundItems == collectionSize)
            TESTPASS(hLog, "FoundCount[%u] == collectionSize[%u]", foundItems, collectionSize);
        else
            TESTFAIL(hLog, "FoundCount[%u] != collectionSize[%u]", foundItems, collectionSize);
        } ENDTESTCASE;

    TESTCASE("FindNickname length of 2")
        {
        unsigned length = 2;
        unsigned collectionSize = NumItemsOfSize(names, ARRAYSIZE(names), length);
        hFind = XFindFirstNickname(FALSE, nickname, length+1);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
            break;
            }

        foundItems = 0;
        DebugPrint("  ---------------\r\n", nickname);
        do
            {
            ++foundItems;
            DebugPrint("  '%ws'\r\n", nickname);
            if(!InCollection(names, ARRAYSIZE(names), nickname))
                TESTFAIL(hLog, "Found name is not in our collection (%ws)", nickname);
            } while(XFindNextNickname(hFind, nickname, length+1) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());

        if(foundItems == collectionSize)
            TESTPASS(hLog, "FoundCount[%u] == collectionSize[%u]", foundItems, collectionSize);
        else
            TESTFAIL(hLog, "FoundCount[%u] != collectionSize[%u]", foundItems, collectionSize);
        } ENDTESTCASE;

    TESTCASE("FindNickname length of 3")
        {
        unsigned length = 3;
        unsigned collectionSize = NumItemsOfSize(names, ARRAYSIZE(names), length);
        hFind = XFindFirstNickname(FALSE, nickname, length+1);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "FindFirstNickname (ec: %ld)", GetLastError());
            break;
            }

        foundItems = 0;
        DebugPrint("  ---------------\r\n", nickname);
        do
            {
            ++foundItems;
            DebugPrint("  '%ws'\r\n", nickname);
            if(!InCollection(names, ARRAYSIZE(names), nickname))
                TESTFAIL(hLog, "Found name is not in our collection (%ws)", nickname);
            } while(XFindNextNickname(hFind, nickname, length+1) == TRUE);
        
        if(XFindClose(hFind) != FALSE)
            TESTPASS(hLog, "XFindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "XFindClose (ec: %ld)", GetLastError());

        if(foundItems == collectionSize)
            TESTPASS(hLog, "FoundCount[%u] == collectionSize[%u]", foundItems, collectionSize);
        else
            TESTFAIL(hLog, "FoundCount[%u] != collectionSize[%u]", foundItems, collectionSize);
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
