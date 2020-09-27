/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    fileio_win32API.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

*/

typedef DWORD (*APITEST_FUNC)(HANDLE hLog, DWORD ThreadID);

namespace FILEIO
    {
    //
    //  Win32 FILE IO Tests
    //
    extern DWORD CopyFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD CopyFileEx_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD CreateDirectory_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD CreateFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD DeleteFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD FindFile_Test(HANDLE hLog, DWORD ThreadID);
    //extern DWORD FindFirstFileEx_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD FlushFileBuffers_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetDiskFreeSpaceEx_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetDriveType_Test(HANDLE hLog, DWORD ThreadID);
    //extern DWORD GetFileAttributesEx_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetFileInformationByHandle_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetFileSize_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetLogicalDrives_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetSetFileAttributes_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetSetFileTime_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD GetVolumeInformation_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD MoveFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD MoveFileWithProgress_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD ReadWriteFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD ReadWriteFileEx_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD ReadWriteFileGatherScatter_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD RemoveDirectory_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD ReplaceFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD SetEndOfFile_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD SetFilePointer_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD SetFilePointerEx_Test(HANDLE hLog, DWORD ThreadID);

    extern DWORD DVDRead_Test(HANDLE hLog, DWORD ThreadID);
    extern DWORD HardDriveFull_Test(HANDLE hLog, DWORD ThreadID);

    } // namespace FILEIO