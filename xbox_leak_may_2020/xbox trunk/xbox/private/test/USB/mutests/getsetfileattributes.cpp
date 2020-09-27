/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    GetSetFileAttributes.cpp

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

    Tests the GetFileAttributes / SetFileAttributes APIs

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD MUTests::GetSetFileAttributes_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD GetFileAttributes(
      LPCTSTR lpFileName   // name of file or directory
    );

    BOOL SetFileAttributes(
      LPCTSTR lpFileName,      // file name
      DWORD dwFileAttributes   // attributes
    );

    #define FILE_ATTRIBUTE_READONLY             0x00000001
    #define FILE_ATTRIBUTE_HIDDEN               0x00000002
    #define FILE_ATTRIBUTE_SYSTEM               0x00000004
    #define FILE_ATTRIBUTE_DIRECTORY            0x00000010
    #define FILE_ATTRIBUTE_ARCHIVE              0x00000020
    #define FILE_ATTRIBUTE_DEVICE               0x00000040
    #define FILE_ATTRIBUTE_NORMAL               0x00000080
    #define FILE_ATTRIBUTE_TEMPORARY            0x00000100
    #define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
    #define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
    #define FILE_ATTRIBUTE_COMPRESSED           0x00000800
    #define FILE_ATTRIBUTE_OFFLINE              0x00001000
    #define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
    #define FILE_ATTRIBUTE_ENCRYPTED            0x00004000
    */
    xSetFunctionName(hLog, "SetFileAttributes / GetFileAttributes");

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FFA01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%ws:\\FFA02_%d.txt", BASE_DRIVE, ThreadID);
    char lpDir1[80]; sprintf(lpDir1, "%ws:\\FFA03_%d", BASE_DRIVE, ThreadID);

    CreateFilledFile(lpFile1, KILOBYTE*16);
    CreateDirectory(lpDir1, FALSE);
    SetFileAttributes(lpFile1, FILE_ATTRIBUTE_NORMAL);
    SetFileAttributes(lpDir1, FILE_ATTRIBUTE_NORMAL);

    DWORD sattrib = 0;
    DWORD gattrib = 0;
    unsigned i;


    DWORD time = GetTickCount();



    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("File Set read only")
        {
        sattrib = FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("File Clear read only")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("File Set archive")
        {
        sattrib = FILE_ATTRIBUTE_ARCHIVE;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("File Clear archive")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("File Set hidden")
        {
        sattrib = FILE_ATTRIBUTE_HIDDEN;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("File Clear hidden")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("File Set system")
        {
        sattrib = FILE_ATTRIBUTE_SYSTEM;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("File Clear system")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("File Set arhs")
        {
        sattrib = FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("File Clear arhs")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_SYSTEM & ~FILE_ATTRIBUTE_HIDDEN & ~FILE_ATTRIBUTE_ARCHIVE & ~FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;

    TESTCASE("Directory Set read only")
        {
        sattrib = GetFileAttributes(lpDir1) | FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("Directory Clear read only")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpDir1) & ~FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory Set archive")
        {
        sattrib = GetFileAttributes(lpDir1) | FILE_ATTRIBUTE_ARCHIVE;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("Directory Clear archive")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpDir1) & ~FILE_ATTRIBUTE_ARCHIVE;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory Set hidden")
        {
        sattrib = GetFileAttributes(lpDir1) | FILE_ATTRIBUTE_HIDDEN;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("Directory Clear hidden")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpDir1) & ~FILE_ATTRIBUTE_HIDDEN;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory Set system")
        {
        sattrib = GetFileAttributes(lpDir1) | FILE_ATTRIBUTE_SYSTEM;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("Directory Clear system")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpDir1) & ~FILE_ATTRIBUTE_SYSTEM;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;
    TESTCASE("Directory Set arhs")
        {
        sattrib = GetFileAttributes(lpDir1) | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        // The next test case depends on this test case
        } ENDTESTCASE;
    TESTCASE("Directory Clear arhs")
        {
        // The previous test case depends on this test case
        sattrib = GetFileAttributes(lpDir1) & ~FILE_ATTRIBUTE_SYSTEM & ~FILE_ATTRIBUTE_HIDDEN & ~FILE_ATTRIBUTE_ARCHIVE & ~FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpDir1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpDir1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("SetFileAttributes(missing file, 0)")
        {
        if(SetFileAttributes(lpFakeFile, 0) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GetFileAttributes(missing file)")
        {
        if(GetFileAttributes(lpFakeFile) == -1)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;




    xSetFunctionName(hLog, "SetFileAttributes / GetFileAttributes");
    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////


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
    DeleteFile(lpFile1);
    RemoveDirectory(lpDir1);

    return 0;
    }
