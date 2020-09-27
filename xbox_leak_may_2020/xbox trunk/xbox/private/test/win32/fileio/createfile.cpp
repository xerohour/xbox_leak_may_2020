/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    CreateFile.cpp

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

namespace FILEIO 
    {
    bool WriteBigFile(char *filename, DWORD size, unsigned char fill);
    }

/*

Routine Description:

    Tests the CreateFile API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

Notes:

    Cases for the below flags are handled in the ReadFile/WriteFile tests
        FILE_FLAG_RANDOM_ACCESS
        FILE_FLAG_SEQUENTIAL_SCAN
        FILE_FLAG_WRITE_THROUGH
    
*/
DWORD FILEIO::CreateFile_Test(HANDLE hLog, DWORD ThreadID)
    {
    extern DWORD stressMode;
    /*
    HANDLE CreateFile(
      LPCTSTR lpFileName,                         // file name
      DWORD dwDesiredAccess,                      // access mode
      DWORD dwShareMode,                          // share mode
      LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
      DWORD dwCreationDisposition,                // how to create
      DWORD dwFlagsAndAttributes,                 // file attributes
      HANDLE hTemplateFile                        // handle to template file
    );
    */
    xSetFunctionName(hLog, "CreateFile");

    char lpFile1[80]; sprintf(lpFile1, "%s:\\FCF01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%s:\\FCF02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%s:\\FCF03_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile4[80]; sprintf(lpFile4, "%s:\\FCF04_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile5[80]; sprintf(lpFile5, "%s:\\FCF05_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%s:\\FCF06_%d.txt", BASE_DRIVE, ThreadID);
    char lpTemp[80];
    char lpTemp2[80];

    HANDLE hFile = NULL;
    HANDLE hFile1 = NULL;
    HANDLE hFile2 = NULL;
    unsigned i;

    // initialize system state
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);
    DeleteFile(lpFile4);
    DeleteFile(lpFile5);
    DeleteFile(lpFakeFile);

    DWORD time = GetTickCount();

    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("All NUL")
        {
        hFile = CreateFile(lpNull, 0, 0, NULL, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("All NULL + INVALID_HANDLE_VALUE")
        {
        hFile = CreateFile(lpNull, 0, 0, NULL, 0, 0, INVALID_HANDLE_VALUE);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("NULL and Empty Name/SA")
        {
        hFile = CreateFile(lpEmpty, 0, 0, &saEmpty, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("NULL and Empty Name/SA (SA with size)")
        {
        hFile = CreateFile(lpEmpty, 0, 0, &saEmptyWithSize, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("NULL and Empty Name/SA (SA with size and empty SD)")
        {
        hFile = CreateFile(lpEmpty, 0, 0, &saWithEmptySD, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("NULL (SA with size and empty SD)")
        {
        hFile = CreateFile(lpNull, 0, 0, &saWithEmptySD, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("New file NULL (SA with size and empty SD)")
        {
        hFile = CreateFile(lpFile1, 0, 0, &saWithEmptySD, 0, 0, hNull);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        DeleteFile(lpFile1);
        } ENDTESTCASE;
*/

    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Create new file write")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpFile1))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("Create new file write/read")
        {
        hFile = CreateFile(lpFile2, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpFile2))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("Create new file read")
        {
        hFile = CreateFile(lpFile3, GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpFile3))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE;
    TESTCASE("Open existing file write")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        DeleteFile(lpFile1);
        } ENDTESTCASE;
    TESTCASE("Open existing file write/read")
        {
        hFile = CreateFile(lpFile2, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile2);});
    TESTCASE("Open existing file read")
        {
        hFile = CreateFile(lpFile3, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile3);});
    TESTCASE("ReOpen an opened file with read access")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile1 = CreateFile(lpFile1, STANDARD_RIGHTS_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile1 != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); CloseHandle(hFile1); DeleteFile(lpFile1);});
    TESTCASE("ReOpen an opened file with write access")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile1 = CreateFile(lpFile1, STANDARD_RIGHTS_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile1 != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); CloseHandle(hFile1); DeleteFile(lpFile1);});
    TESTCASE("ReOpen an opened file with read write access")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile1 = CreateFile(lpFile1, STANDARD_RIGHTS_WRITE | STANDARD_RIGHTS_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile1 != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); CloseHandle(hFile1); DeleteFile(lpFile1);});
    TESTCASE("TRUNCATE_EXISTING 16k")
        {
        DWORD filesize;
        sprintf(lpTemp, "%s:\\FCF08_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 16*KILOBYTE+7); // 7 is just randomness

        hFile = CreateFile(lpTemp, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS | TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        if((filesize=GetFileSize(hFile, NULL)) == 0)
            TESTPASS(hLog, "GetFileSize[%lu] (ec: %ld)", filesize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize[%lu] (ec: %ld)", filesize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("TRUNCATE_EXISTING 16meg")
        {
        DWORD filesize;
        sprintf(lpTemp, "%s:\\FCF08_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 16*MEGABYTE+7); // 7 is just randomness

        hFile = CreateFile(lpTemp, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS | TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        if((filesize=GetFileSize(hFile, NULL)) == 0)
            TESTPASS(hLog, "GetFileSize[%lu] (ec: %ld)", filesize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize[%lu] (ec: %ld)", filesize, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("DELETE_ON_CLOSE existing file")
        {
        sprintf(lpTemp, "%s:\\FCF09_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 16*MEGABYTE+7); // 7 is just randomness

        hFile = CreateFile(lpTemp, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        CloseHandle(hFile);

        if(! FileExists(lpTemp))
            TESTPASS(hLog, "FileExists (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FileExists (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpTemp);});
    TESTCASE("DELETE_ON_CLOSE new file")
        {
        sprintf(lpTemp, "%s:\\FCF10_%d.txt", BASE_DRIVE, ThreadID);

        hFile = CreateFile(lpTemp, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        FillFile(hFile, 16*MEGABYTE+7); // 7 is just randomness

        CloseHandle(hFile);

        if(! FileExists(lpTemp))
            TESTPASS(hLog, "FileExists (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FileExists (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpTemp);});
    /*
    TESTCASE("\\Device\\Harddisk0")
        {
        hFile = CreateFile("\\Device\\Harddisk0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); });
    TESTCASE("\\Device\\HardDisk0\\Paritition1")
        {
        hFile = CreateFile("\\Device\\HardDisk0\\Paritition1", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); });
    TESTCASE("\\" BASE_DRIVE ":")
        {
        hFile = CreateFile("\\" BASE_DRIVE ":", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); });
    */
    TESTCASE("FILE_FLAG_BACKUP_SEMANTICS")
        {
        CreateFilledFile(lpFile3, 16*KILOBYTE+7); // 7 is just randomness
        hFile = CreateFile(lpFile3, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile3);});
    TESTCASE("Multiple dots")
        {
        sprintf(lpTemp, "%s:\\a.b.c.d.e.f.g.h", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Leading dot")
        {
        sprintf(lpTemp, "%s:\\.dot", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Trailing dot")
        {
        sprintf(lpTemp, "%s:\\dot.", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Spaces")
        {
        sprintf(lpTemp, "%s:\\a b c d e f g h", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Leading space")
        {
        sprintf(lpTemp, "%s:\\ space", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Trailing space")
        {
        sprintf(lpTemp, "%s:\\space ", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("3.8")
        {
        sprintf(lpTemp, "%s:\\abc.zxcvbnmq", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("13 chars")
        {
        sprintf(lpTemp, "%s:\\0123456789012", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("20 chars")
        {
        sprintf(lpTemp, "%s:\\01234567890123456789", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("41 chars")
        {
        sprintf(lpTemp, "%s:\\01234567890123456789012345678901234567890", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("42 chars")
        {
        sprintf(lpTemp, "%s:\\012345678901234567890123456789012345678901", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    TESTCASE("Write Open to a file with RO attrib")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE_CLEAN({ SetFileAttributes(lpFile1, FILE_ATTRIBUTE_NORMAL); DeleteFile(lpFile1); });
    TESTCASE("ReadWrite Open to a file with RO attrib")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        } ENDTESTCASE_CLEAN({ SetFileAttributes(lpFile1, FILE_ATTRIBUTE_NORMAL); DeleteFile(lpFile1); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Create new \\\\?")
        {
        sprintf(lpTemp, "\\\\?\\%s:\\FCF07_%d.txt", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            DeleteFile(lpTemp);
            }
        } ENDTESTCASE;
    TESTCASE("43 chars")
        {
        sprintf(lpTemp, "%s:\\0123456789012345678901234567890123456789012", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            DeleteFile(lpTemp);
            }
        } ENDTESTCASE;
    TESTCASE("Open nonexisting file write")
        {
        hFile = CreateFile(lpFakeFile, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("Open new file relative")
        {
        hFile = CreateFile("FCF11.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            DeleteFile("CreateFileRel.txt");
            }
        } ENDTESTCASE;
    TESTCASE("Open nonexisting file write/read")
        {
        hFile = CreateFile(lpFakeFile, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("Open nonexisting file read")
        {
        hFile = CreateFile(lpFakeFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("New only on existing file")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        CloseHandle(hFile);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });
    TESTCASE("New only on existing write/read")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        CloseHandle(hFile);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });
    TESTCASE("New only on existing file read")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        CloseHandle(hFile);
        if(!FileExists(lpFile1))
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile = CreateFile(lpFile1, GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });
    TESTCASE("Open a directory read only")
        {
        sprintf(lpTemp, "%s:\\FCF12_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpTemp, NULL);
        if(!FileExists(lpTemp))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        hFile = CreateFile(lpTemp, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpTemp); });
    TESTCASE("Open a directory write")
        {
        sprintf(lpTemp, "%s:\\FCF13_%d", BASE_DRIVE, ThreadID);
        CreateDirectory(lpTemp, NULL);
        if(!FileExists(lpTemp))
            {
            BLOCKED(0, "Unable to create initial directory");
            }
        hFile = CreateFile(lpTemp, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE_CLEAN({ RemoveDirectory(lpTemp); });
    TESTCASE("ReOpen an opened file with exclusive access")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            BLOCKED(0, "Unable to create initial file");
            }
        hFile1 = CreateFile(lpFile1, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile1 == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
            CloseHandle(hFile1);
            }
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpFile1); });
    TESTCASE("TRUNCATE_EXISTING no Rights")
        {
        sprintf(lpTemp, "%s:\\FCF14_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 16*KILOBYTE+7); // 7 is just randomness

        hFile = CreateFile(lpTemp, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS | TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    /*
    TESTCASE("Template File")
        {
        sprintf(lpTemp, "%s:\\FCF15_%d.txt", BASE_DRIVE, ThreadID);
        sprintf(lpTemp2, "%s:\\FCF16_%d.txt", BASE_DRIVE, ThreadID);

        hFile = CreateFile(lpTemp, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        hFile1 = CreateFile(lpTemp2, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, hFile);
        if(hFile1 == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        CloseHandle(hFile);
        CloseHandle(hFile1);
        } ENDTESTCASE_CLEAN({ DeleteFile(lpTemp); DeleteFile(lpTemp2); });
        */
    TESTCASE("\\\\.\\C:")
        {
        hFile = CreateFile("\\\\.\\C:", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("\\\\?\\Volume{GUID0}\\")
        {
        hFile = CreateFile("\\\\?\\Volume{00000000-0000-0000-0000-000000000000}\\", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("\\\\?\\Volume{GUID1}\\")
        {
        hFile = CreateFile("\\\\?\\Volume{00000000-0000-0000-0000-000000000001}\\", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;
    TESTCASE("\\\\?\\Volume{GUIDF}\\")
        {
        hFile = CreateFile("\\\\?\\Volume{FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF}\\", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            {
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
            CloseHandle(hFile);
            }
        } ENDTESTCASE;



    //////////////////////////////////////////////////////////////////////////
    // Invalid characters
    //////////////////////////////////////////////////////////////////////////
    for(i=0; i<ARRAYSIZE(invalidFileNames); i++)
        {
        TESTCASE(invalidFileNames[i])
            {
            hFile = CreateFile(invalidFileNames[i], GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if(hFile == INVALID_HANDLE_VALUE)
                TESTPASS(hLog, "(ec: %ld)", GetLastError());
            else
                {
                TESTFAIL(hLog, "(ec: %ld)", GetLastError());
                CloseHandle(hFile);
                }
            } ENDTESTCASE;
        }


    // TODO
    // write access to cd/dvd file


    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Close the handle twice")
        {
        hFile = CreateFile(lpFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(CloseHandle(hFile) == TRUE)
            TESTPASS(hLog, "1st CloseHandle (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "1st CloseHandle (ec: %ld)", GetLastError());

        if(CloseHandle(hFile) == FALSE)
            TESTPASS(hLog, "2nd CloseHandle (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "2nd CloseHandle (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); });

    TESTCASE("Move Cache Entries") // for bug 1229
        {
        char *buffer = new char[1024];
        if(!buffer) break;
        DWORD size = sprintf(buffer, "Read and write to different files to screw with the cache\r\n");
        DWORD size2;
        hFile = CreateFile(lpFile1, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        hFile1 = CreateFile(lpFile2, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        // expand the files while reading and writting from them
        for(unsigned i=1024; i<15*MEGABYTE; i+=(PAGESIZE+123))
            {
            SetFilePointer(hFile, i, NULL, FILE_BEGIN);
            SetFilePointer(hFile1, i, NULL, FILE_BEGIN);
            SetEndOfFile(hFile);
            SetEndOfFile(hFile1);
            for(unsigned j=0; j<4; j++)
                {
                SetFilePointer(hFile, rand()*i/RAND_MAX, NULL, FILE_BEGIN);
                WriteFile(hFile, buffer, size, &size2, NULL);
                SetFilePointer(hFile, rand()*i/RAND_MAX, NULL, FILE_BEGIN);
                ReadFile(hFile, buffer+512, 128, &size2, NULL);

                SetFilePointer(hFile1, rand()*i/RAND_MAX, NULL, FILE_BEGIN);
                WriteFile(hFile1, buffer, size, &size2, NULL);
                SetFilePointer(hFile1, rand()*i/RAND_MAX, NULL, FILE_BEGIN);
                ReadFile(hFile1, buffer+512, 128, &size2, NULL);
                }
            }

        if(CloseHandle(hFile) == TRUE)
            TESTPASS(hLog, "CloseHandle(file1) (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CloseHandle(file1) (ec: %ld)", GetLastError());
        if(CloseHandle(hFile1) == TRUE)
            TESTPASS(hLog, "CloseHandle(file2) (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CloseHandle(file2) (ec: %ld)", GetLastError());

        delete[] buffer;
        } ENDTESTCASE_CLEAN({ DeleteFile(lpFile1); DeleteFile(lpFile2); });

    TESTCASE("Fragmented Cache Entries") // for bug 1229 + 4804
        {
        char *rbuffer = new char[9*MEGABYTE];
        if(!rbuffer) break;
        char *wbuffer = new char[9*MEGABYTE];
        if(!wbuffer) { delete[] rbuffer; break;}

        DWORD size, wSize, rSize;
        const unsigned NUMFILES = 4097;

        // create the files
        for(unsigned i=0; i<NUMFILES; i++)
            {
            sprintf(lpTemp, "%s:\\FCF20_%04u_%d.txt", BASE_DRIVE, i, ThreadID);
            hFile = CreateFile(lpTemp, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if(i % 256 == 0)
                SetFilePointer(hFile, 8192 * 100, NULL, FILE_BEGIN);
            else
                SetFilePointer(hFile, 100, NULL, FILE_BEGIN);
            WriteFile(hFile, "FCF", 3, &wSize, NULL);
            CloseHandle(hFile);
            }
        // delete half of them
        for(unsigned i=0; i<NUMFILES; i+=2)
            {
            sprintf(lpTemp, "%s:\\FCF20_%04u_%d.txt", BASE_DRIVE, i, ThreadID);
            DeleteFile(lpTemp);
            }

        // create and write a big file
        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxChunks = 64;
        hFile = CreateFile(lpFile1, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            {
            xLog(hLog, XLL_FAIL, "Unable to open file (ec: %ld)", GetLastError());
            }
        else
            {
            for(size=0; size<MEGABYTE*8; size++)
                {
                wbuffer[size] = (char)~size;
                }
            for(unsigned int i=0; i<maxChunks; i++)
                {
                if(i%6==0) DebugPrint("   (%d) %02d%%\n", ThreadID, 50*i/maxChunks);
                if(! (WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size))
                    break;
                }

            if(i == maxChunks)
                TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", size*i, GetLastError());
            else
                TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", size*i, GetLastError());

            if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0)
                {
                xLog(hLog, XLL_FAIL, "Unable to reset file pointer (ec: %ld)", GetLastError());
                }

            for(unsigned j=0; j<i; j++)
                {
                if(j%6==0) DebugPrint("   (%d) %02d%%\n", ThreadID, 50*j/maxChunks + 50);
                if(! (ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size))
                    break;

                if(! (wSize == rSize && memcmp(rbuffer, wbuffer, size) == 0))
                    break;
                }

            if(j == i)
                {
                TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", size*j, GetLastError());
                TESTPASS(hLog, "Write Buffer == Read Buffer");
                }
            else
                {
                if(! (wSize == rSize && memcmp(rbuffer, wbuffer, size) == 0))
                    {
                    TESTFAIL(hLog, "Write Buffer != Read Buffer");
                    }
                else
                    {
                    TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", size*j, GetLastError());
                    }
                }
            DebugPrint("   (%d) 100%%\n", ThreadID);
            }

        // cleanup
        for(unsigned i=1; i<NUMFILES; i+=2)
            {
            sprintf(lpTemp, "%s:\\FCF20_%04u_%d.txt", BASE_DRIVE, i, ThreadID);
            DeleteFile(lpTemp);
            }
        delete[] wbuffer;
        delete[] rbuffer;
        } ENDTESTCASE_CLEAN({  CloseHandle(hFile); DeleteFile(lpFile1); });

    TESTCASE("Evil HD Bugs")
        {
        if(stressMode != 1) break; // only run this test in stress mode
        unsigned i;
        unsigned maxStress;

        // 1 meg files
        maxStress = 5000;
        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
            sprintf(lpTemp, "%s:\\FCF21_%04u_%d.txt", TITLE_DRIVE, i, ThreadID);
            WriteBigFile(lpTemp, MEGABYTE, (unsigned char)i);
            }
        for(i=0; i<maxStress; i++)
            {
            sprintf(lpTemp, "%s:\\FCF21_%04u_%d.txt", TITLE_DRIVE, i, ThreadID);
            DeleteFile(lpTemp);
            }
        DebugPrint("   (%d) 100%%\n", ThreadID);

        // 50 meg files
        maxStress = 10000;
        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
            sprintf(lpTemp, "%s:\\FCF22_%04u_%d.txt", TITLE_DRIVE, i, ThreadID);
            WriteBigFile(lpTemp, 50*MEGABYTE, (unsigned char)i);
            }
        for(i=0; i<maxStress; i++)
            {
            sprintf(lpTemp, "%s:\\FCF22_%04u_%d.txt", TITLE_DRIVE, i, ThreadID);
            DeleteFile(lpTemp);
            }
        DebugPrint("   (%d) 100%%\n", ThreadID);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    int stressCount=0;
    int maxStress;
    if(stressMode == 1) maxStress = 500; // small number of handles in stress mode
    else maxStress = 1*MEGABYTE;
    HANDLE *handList = new HANDLE[maxStress];
    TESTCASE("Mini Stress")
        {
        if(!handList)
            {
            BLOCKED(0, "Unable to allocate memory, test skipped");
            }

        DebugPrint("FILEIO(%d): This test takes a long time to run...\n", ThreadID);
        for(stressCount=0; stressCount<maxStress; stressCount++)
            {
            if(stressCount%(maxStress/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, stressCount*100/maxStress);
            handList[stressCount] = CreateFile(lpFile5, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);
            if(handList[stressCount] == INVALID_HANDLE_VALUE)
                {
                xLog(hLog, XLL_INFO, "Stopped at %d (ec: %ld)", stressCount, GetLastError());
                break;
                }
            }
        } ENDTESTCASE;
    for(--stressCount; stressCount>=0; stressCount--)
        {
        CloseHandle(handList[stressCount]);
        }
    DebugPrint("   (%d) 100%%\n", ThreadID);
    delete[] handList;
    DeleteFile(lpFile5);


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    // clean up
    DeleteFile(lpFile1);
    DeleteFile(lpFile2);
    DeleteFile(lpFile3);
    DeleteFile(lpFile4);
    DeleteFile(lpFile5);
    DeleteFile(lpFakeFile);

    return 0;
    }

bool FILEIO::WriteBigFile(char *filename, DWORD size, unsigned char fill)
    {
    unsigned chunkSize = PAGESIZE;
    char *buffer = new char[chunkSize];
    if(!buffer) return false;
    DWORD wSize;

    memset(buffer, fill, chunkSize);

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        {
        delete[] buffer;
        return false;
        }

    for(unsigned int i=0; i<size; i+=chunkSize)
        {
        if(i+chunkSize > size) wSize = size-i;
        else wSize = chunkSize;
        if(! (WriteFile(hFile, buffer, wSize, &wSize, NULL) == TRUE && wSize == size))
            {
            CloseHandle(hFile);
            delete[] buffer;
            return false;
            }
        }

    CloseHandle(hFile);
    delete[] buffer;
    return true;
    }
