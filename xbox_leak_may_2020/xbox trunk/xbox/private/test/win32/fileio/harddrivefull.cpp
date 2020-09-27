/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    HardDriveFull.cpp

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

    Tests a bunch of APIs while the Hard Drive is full

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    

*/
DWORD FILEIO::HardDriveFull_Test(HANDLE hLog, DWORD ThreadID)
    {
    char drive[5]; sprintf(drive, "%s:\\", BASE_DRIVE);
    char lpBaseDirectory[80]; sprintf(lpBaseDirectory, "%sFHD_%d", drive, ThreadID);
    char lpFile1[80]; sprintf(lpFile1, "%s\\FHD01_%d.txt", lpBaseDirectory, ThreadID);
    char lpFile2[80]; sprintf(lpFile2, "%s\\FHD02_%d.txt", lpBaseDirectory, ThreadID);
    char lpFile3[80]; sprintf(lpFile3, "%s\\FHD03_%d.txt", lpBaseDirectory, ThreadID);
    char lpTempFile1[80]; sprintf(lpTempFile1, "%s\\FHD04_%d.txt", lpBaseDirectory, ThreadID);
    char lpTempFile2[80]; sprintf(lpTempFile2, "%s\\FHD05_%d.txt", lpBaseDirectory, ThreadID);
    char lpTempFile3[80]; sprintf(lpTempFile3, "%s\\FHD06_%d.txt", lpBaseDirectory, ThreadID);
    char lpFillHDFile[80];
    char lpTemp[80];

    HANDLE hFile;
    char *wbuffer=NULL;
    char *rbuffer=NULL;

    // initialize system state
    CreateDirectory(lpBaseDirectory, NULL);
    CreateFilledFile(lpFile1, KILOBYTE*16);       // GetFileSize test below relies on this file being 16k
    CreateFilledFile(lpFile2, MEGABYTE*32);
    CreateFilledFile(lpFile3, MEGABYTE*64);

    // Fill up the HD
    FillHDPartition(drive, lpFillHDFile);

    DWORD time = GetTickCount();

    xSetFunctionName(hLog, "GetDiskFreeSpaceEx");
    TESTCASE("Full HardDrive")
        {
        ULARGE_INTEGER available;
        ULARGE_INTEGER total;
        ULARGE_INTEGER free;
        available = total = free = ulBogus;

        if(GetDiskFreeSpaceEx(BASE_DRIVE ":\\", &available, &total, &free) == TRUE && available.QuadPart < total.QuadPart && available.QuadPart != ulBogus.QuadPart)
            TESTPASS(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);
        else
            TESTFAIL(hLog, "(ec: %ld) avail:%I64x total:%I64x", GetLastError(), available.QuadPart, total.QuadPart);

        if(available.QuadPart != 0)
            xLog(hLog, XLL_WARN, "Hard Drive is not full! The next set of tests will FAIL.");
        } ENDTESTCASE;


    xSetFunctionName(hLog, "CopyFile");
    TESTCASE("Full HardDrive")
        {
        if(CopyFile(lpFile1, lpTempFile1, TRUE) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        if(FileExists(lpFile1) && ! FileExists(lpTempFile1))
            TESTPASS(hLog, "Checking file existance (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Checking file existance (ec: %ld)", GetLastError());
        } ENDTESTCASE;


    xSetFunctionName(hLog, "MoveFile");
    TESTCASE("Full HardDrive")
        {
        if(MoveFile(lpFile1, lpTempFile1) == TRUE)
            TESTPASS(hLog, "Move 1 (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Move 1 (ec: %ld)", GetLastError());

        if(!FileExists(lpFile1) && FileExists(lpTempFile1))
            TESTPASS(hLog, "Checking file existance (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Checking file existance (ec: %ld)", GetLastError());

        if(MoveFile(lpTempFile1, lpFile1) == TRUE)
            TESTPASS(hLog, "Move 2 (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Move 2 (ec: %ld)", GetLastError());
        } ENDTESTCASE;


    xSetFunctionName(hLog, "CreateDirectory");
    TESTCASE("Full HardDrive")
        {
        if(CreateDirectory(BASE_DRIVE ":\\FHD07", NULL) == FALSE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());

        } ENDTESTCASE_CLEAN({ RemoveDirectory(BASE_DRIVE ":\\CreateDirectory"); });


    // create a new file, and try to write to it
    xSetFunctionName(hLog, "CreateFile");
    TESTCASE("Full HardDrive")
        {
        hFile = CreateFile(lpTempFile1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "Open new file (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Open new file (ec: %ld)", GetLastError());

        DWORD size = 15*KILOBYTE;
        xSetFunctionName(hLog, "SetFilePointer");
        if(SetFilePointer(hFile, size, NULL, FILE_BEGIN) == size)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", size, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", size, GetLastError());

        char w = 'X';
        DWORD wsize = sizeof(w);
        xSetFunctionName(hLog, "WriteFile");
        if(WriteFile(hFile, &w, wsize, &wsize, NULL) != TRUE)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wsize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wsize, GetLastError());

        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); DeleteFile(lpTempFile1); });


    // read and write from inside an existing file
    xSetFunctionName(hLog, "CreateFile");
    TESTCASE("Full HardDrive")
        {
        DWORD wSize, rSize, size;
        DWORD seek;

        wbuffer = new char[MEGABYTE*1];
        if(!wbuffer)
            {
            BLOCKED(0, "Unable to allocate memory");
            }
        rbuffer = new char[MEGABYTE*24];
        if(!rbuffer)
            {
            delete[] wbuffer;
            wbuffer = NULL;
            BLOCKED(0, "Unable to allocate memory");
            }

        hFile = CreateFile(lpFile3, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "Open existing file (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "Open existing file (ec: %ld)", GetLastError());

        seek = 11*KILOBYTE;
        xSetFunctionName(hLog, "SetFilePointer");
        if((seek=SetFilePointer(hFile, seek, NULL, FILE_BEGIN)) == 11*KILOBYTE)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        xSetFunctionName(hLog, "WriteFile");
        size = sprintf(wbuffer, "Hello World.\r\n\r\nThis is a ReadFile() / WriteFile() API Test!\r\n");
        if(WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size)
            TESTPASS(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());
        else
            TESTFAIL(hLog, "WriteFile[%d] (ec: %ld)", wSize, GetLastError());

        seek = 11*KILOBYTE;
        xSetFunctionName(hLog, "SetFilePointer");
        if((seek=SetFilePointer(hFile, seek, NULL, FILE_BEGIN)) == 11*KILOBYTE)
            TESTPASS(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());
        else
            TESTFAIL(hLog, "Seek[%d] (ec: %ld)", seek, GetLastError());

        xSetFunctionName(hLog, "ReadFile");
        if(ReadFile(hFile, rbuffer, size, &rSize, NULL) == TRUE && rSize == size)
            TESTPASS(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());
        else
            TESTFAIL(hLog, "ReadFile[%d] (ec: %ld)", rSize, GetLastError());

        if(wSize == rSize && wSize == size && memcmp(rbuffer, wbuffer, size) == 0)
            TESTPASS(hLog, "Write Buffer[%d] == Read Buffer[%d]", wSize, rSize);
        else
            TESTFAIL(hLog, "Write Buffer[%d] != Read Buffer[%d]", wSize, rSize);

        } ENDTESTCASE_CLEAN({ if(wbuffer) delete[] wbuffer; if(rbuffer) delete[] rbuffer; CloseHandle(hFile); });


    xSetFunctionName(hLog, "GetVolumeInformation");
    TESTCASE("Full HardDrive")
        {
        DWORD vnameSize = 128;
        DWORD fsnameSize = 128;
        char vnameBuffer[128];
        char fsnameBuffer[128];
        DWORD serialNum;
        DWORD maxFilename;
        DWORD flags;
        if(GetVolumeInformation(BASE_DRIVE ":\\", vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize) == TRUE)
            TESTPASS(hLog, "(ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "(ec: %ld)", GetLastError());
        } ENDTESTCASE;


    xSetFunctionName(hLog, "SetFileAttributes / GetFileAttributes");
    TESTCASE("Full HardDrive")
        {
        DWORD sattrib = 0;
        DWORD gattrib = 0;
        sattrib = FILE_ATTRIBUTE_READONLY;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());

        sattrib = GetFileAttributes(lpFile1) & ~FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NORMAL;
        if(SetFileAttributes(lpFile1, sattrib) == TRUE && (gattrib=GetFileAttributes(lpFile1)) == sattrib)
            TESTPASS(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        else
            TESTFAIL(hLog, "set(0x%X) get(0x%X) (ec: %ld)", sattrib, gattrib, GetLastError());
        } ENDTESTCASE;


    xSetFunctionName(hLog, "GetFileSize");
    TESTCASE("Full HardDrive")
        {
        DWORD size = KILOBYTE*16;
        DWORD highSize = 0;
        LARGE_INTEGER bigSize;
        bigSize.QuadPart = 0;
        hFile = FileOpen(lpFile1);
        if((highSize=GetFileSize(hFile, &highSize)) == size)
            TESTPASS(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSize=%ld (ec: %ld)", highSize, GetLastError());

        if(GetFileSizeEx(hFile, &bigSize) == TRUE && bigSize.QuadPart == size)
            TESTPASS(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        else
            TESTFAIL(hLog, "GetFileSizeEx=%I64d (ec: %ld)", bigSize.QuadPart, GetLastError());
        } ENDTESTCASE_CLEAN({ CloseHandle(hFile); });


    xSetFunctionName(hLog, "GetFileInformationByHandle");
    TESTCASE("Full HardDrive")
        {
        BY_HANDLE_FILE_INFORMATION fileInfo;
        hFile = FileOpen(lpFile1);
        if(GetFileInformationByHandle(hFile, &fileInfo) == TRUE && fileInfo.nFileSizeLow == KILOBYTE*16)
            TESTPASS(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        else
            TESTFAIL(hLog, "(ec: %ld) filesize: %ld", GetLastError(), fileInfo.nFileSizeLow);
        } ENDTESTCASE_CLEAN( { CloseHandle(hFile); } );


    xSetFunctionName(hLog, "FindFile");
    TESTCASE("Full HardDrive")
        {
        WIN32_FIND_DATA findData;
        HANDLE hFind = FindFirstFile(BASE_DRIVE ":\\*", &findData);
        if(hFind != INVALID_HANDLE_VALUE)
            TESTPASS(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindFirstFile (ec: %ld)", GetLastError());
        do
            {
            // TODO verify
            } while(FindNextFile(hFind, &findData) == TRUE);
        if(FindClose(hFind) != FALSE)
            TESTPASS(hLog, "FindClose (ec: %ld)", GetLastError());
        else
            TESTFAIL(hLog, "FindClose (ec: %ld)", GetLastError());
        } ENDTESTCASE;


    // TODO
        // Get/SetFileTime

    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    xSetFunctionName(hLog, "(hd full tests)");
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    // unfill the HD
    DeleteFile(lpFillHDFile);

    // clean up
    DeleteFile(lpFile1);
    DeleteFile(lpFile3);
    DeleteFile(lpFile2);
    DeleteFile(lpTempFile1);
    DeleteFile(lpTempFile3);
    DeleteFile(lpTempFile2);
    RemoveDirectory(lpBaseDirectory);

    return 0;
    }
