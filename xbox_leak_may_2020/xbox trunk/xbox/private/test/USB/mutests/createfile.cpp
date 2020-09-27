/*

Copyright (c) 2000 Microsoft Corporation

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
#include "mutests.h"

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
DWORD MUTests::CreateFile_Test(HANDLE hLog, DWORD ThreadID)
    {
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

    char lpFile1[80]; sprintf(lpFile1, "%ws:\\FCF01_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%ws:\\FCF02_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%ws:\\FCF03_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile4[80]; sprintf(lpFile4, "%ws:\\FCF04_%d.txt", BASE_DRIVE, ThreadID);
    char lpFile5[80]; sprintf(lpFile5, "%ws:\\FCF05_%d.txt", BASE_DRIVE, ThreadID);
    char lpFakeFile[80]; sprintf(lpFakeFile, "%ws:\\FCF06_%d.txt", BASE_DRIVE, ThreadID);
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
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Create new, 8.3")
        {
        sprintf(lpTemp, "%ws:\\%d.zzz", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        DeleteFile(lpTemp);
        } ENDTESTCASE;
    TESTCASE("Create new, 8.3")
        {
        sprintf(lpTemp, "%ws:\\aaaaaa_%d.000", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        DeleteFile(lpTemp);
        } ENDTESTCASE;
    /*
    TESTCASE("Create new \\\\?")
        {
        sprintf(lpTemp, "\\\\?\\%ws:\\CreateFileFullPath_%d.txt", BASE_DRIVE, ThreadID);
        hFile = CreateFile(lpTemp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE && FileExists(lpTemp))
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        CloseHandle(hFile);
        DeleteFile(lpTemp);
        } ENDTESTCASE;
        */
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
        sprintf(lpTemp, "%ws:\\FCF07_%d.txt", BASE_DRIVE, ThreadID);
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
    TESTCASE("TRUNCATE_EXISTING 6meg")
        {
        DWORD filesize;
        sprintf(lpTemp, "%ws:\\FCF08_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 6*MEGABYTE+7); // 7 is just randomness

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
        sprintf(lpTemp, "%ws:\\FCF09_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 3*MEGABYTE+7); // 7 is just randomness

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
        sprintf(lpTemp, "%ws:\\FCF10_%d.txt", BASE_DRIVE, ThreadID);

        hFile = CreateFile(lpTemp, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());

        FillFile(hFile, 5*MEGABYTE+7); // 7 is just randomness

        CloseHandle(hFile);

        if(! FileExists(lpTemp))
            TESTPASS(hLog, "FileExists (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FileExists (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ DeleteFile(lpTemp);});


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
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
        sprintf(lpTemp, "%ws:\\CreateFileDirR_%d", BASE_DRIVE, ThreadID);
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
        sprintf(lpTemp, "%ws:\\CreateFileDirW_%d", BASE_DRIVE, ThreadID);
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
        sprintf(lpTemp, "%ws:\\FCF11_%d.txt", BASE_DRIVE, ThreadID);
        CreateFilledFile(lpTemp, 3*KILOBYTE+7); // 7 is just randomness

        hFile = CreateFile(lpTemp, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS | TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "CreateFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "CreateFile (ec: %ld)", GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTemp);});
    /*
    TESTCASE("Template File")
        {
        sprintf(lpTemp, "%ws:\\FCF12_%d.txt", BASE_DRIVE, ThreadID);
        sprintf(lpTemp2, "%ws:\\FCF013_%d.txt", BASE_DRIVE, ThreadID);

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
