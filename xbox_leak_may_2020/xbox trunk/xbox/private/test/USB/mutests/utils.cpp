/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    utils.cpp

Abstract:

    This module contains the helper functions used by the API tests

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/
#include "stdafx.h"
#include "utils.h"

#ifdef  __cplusplus
    extern "C" {
    #include "ntos.h"
    }
#else
    #include "ntos.h"
#endif

namespace MUTests {

/*****************************************************************************

Routine Description:

    Ansi2UnicodeHack

    In-place Pseudo Ansi to Unicode (char to wide char) conversion.

Arguments:

    IN char* str - char string to convert to wide char string

Return Value:

    char* - pointer to Unicode string

Note:

    Because a bunch of Unicode functions expect a char string to be on an
    even boundry, the returned string may be moved 1 character over.

*****************************************************************************/
char* Ansi2UnicodeHack(char *str)
    {
    if(!str) return NULL;

    int align = 0;
    int len = strlen(str)+1;

    // put string on an even boundry because some freak put a bunch of ASSERTs
    // that check for even boundries in Unicode functions like 
    // RtlEqualUnicodeString()
    if(((ULONG_PTR)str & 1) != 0)
        {
        align = 1;
        }

    for(; len>=0; len--)
        {
        str[len*2+align] = str[len];
        str[len*2+align+1] = '\0';
        }

    str += align;

    return (char*)str;
    }


/*****************************************************************************

Routine Description:

    Unicode2AnsiHack

    In-place Pseudo Unicode to Ansi (wide char to char) conversion.

Arguments:

    IN unsigned short* str - wide char string to convert to char string

Return Value:

    char* - pointer to ANSI string

*****************************************************************************/
char* Unicode2AnsiHack(unsigned short *str)
    {
    if(!str) return NULL;
    char *str2 = (char*)str;

    size_t len = wcslen(str)+1;
    for(size_t i=0; i<len; i++)
        {
        str2[i] = str2[i*2];
        }

    return (char*)str;
    }

/*

Routine Description:

    Checks to see if a file or directory exists

Arguments:

    char *filename - the name of the file or dir to search for

Return Value:

    true if it exists, false if it doesnt

*/
bool FileExists(const char *filename)
    {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool retval = false;

    hFind = FindFirstFile(filename, &FindFileData);

    if(hFind != INVALID_HANDLE_VALUE)
        {
        retval = true;
        }
    FindClose(hFind);

    return retval;
    }


/*

Routine Description:

    Creates a new file and fills it with x bytes of garbage

Arguments:

    char *filename - the name of the file to create
    DWORD size - how big the file should be

Return Value:

    DWORD actual size of file

*/
DWORD CreateFilledFile(const char *filename, DWORD size /*=16384*/)
    {
    HANDLE file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    size = FillFile(file, size);

    CloseHandle(file);

    return size;
    return size;
    }




/*

Routine Description:

    Fills an opened file with x bytes of garbage

Arguments:

    HANDLE hFile - handle to opened file
    DWORD size - how big the file should be

Return Value:

    DWORD actual size of file

*/
DWORD FillFile(HANDLE hFile, DWORD size)
    {
    char *error = NULL;
    char w = 'X';
    DWORD wsize = sizeof(w);
    DWORD errCode = 0;

    do
        {
        if(hFile == INVALID_HANDLE_VALUE)
            {
            error = "unable to create/open file";
            errCode = GetLastError();
            break;
            }

        if(size == 0) break;

        if(SetFilePointer(hFile, size-1, NULL, FILE_BEGIN) != size-1)
            {
            error = "unable to size file";
            errCode = GetLastError();
            break;
            }

        if(WriteFile(hFile, &w, wsize, &wsize, NULL) != TRUE)
            {
            error = "unable to write last byte";
            errCode = GetLastError();
            break;
            }
        } while(0);

    FlushFileBuffers(hFile);

    if(error)
        {
        DebugPrint("MUTESTS: FillFile(%ld) - %ws (ec: %d)\n", size, error, errCode);
        size = 0;
        }

    return size;
    }

/*

Routine Description:

    Opens a file with Read Only access

Arguments:

    char *filename - name of the file to open

Return Value:

    BOOL - returns the result from CreateFile

*/
HANDLE FileOpen(const char *filename)
    {
    return CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }




/*

Routine Description:

    Opens a file with Read Only access

Arguments:

    char *filename - name of the file to open

Return Value:

    BOOL - returns the result from CreateFile

*/
HANDLE FileOpenRW(const char *filename)
    {
    return CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }




/*

Routine Description:

    Generates a FILETIME structure from the date components by calling 
    SystemTimeToFileTime

Arguments:

    LPFILETIME fileTime - pointer to the FILETIME struct to populate with the date
    WORD year
    WORD month
    WORD day
    WORD hour
    WORD min
    WORD sec
    WORD msec

Return Value:

    BOOL - returns the result from SystemTimeToFileTime

*/
BOOL CreateFileTime(LPFILETIME fileTime, WORD year, WORD month, WORD day, WORD hour, WORD min, WORD sec, WORD msec)
    {
    SYSTEMTIME sysTime;

    sysTime.wYear = year;
    sysTime.wMonth = month;
    sysTime.wDay = day;
    sysTime.wHour = hour;
    sysTime.wMinute = min;
    sysTime.wSecond = sec;
    sysTime.wMilliseconds = msec;

    return SystemTimeToFileTime(&sysTime, fileTime);
    }

/*

Routine Description:


Arguments:

    drive - Drive to fill up, it should be in the form 
        C:\
        D:\
        etc.
    lpFile - This function will populate buffer with the name of the file created

Return Value:

    DWORD return code from GetLastError in the event of a problem

*/
DWORD FillHDPartition(const char *drive, char *lpFile)
    {
    if(!drive || !lpFile) return ERROR_INVALID_PARAMETER;

    DWORD error = 0;
    char w = 'X';
    DWORD wsize = sizeof(w);
    LARGE_INTEGER distance;
    LARGE_INTEGER size;
    ULARGE_INTEGER available;
    ULARGE_INTEGER total;
    ULARGE_INTEGER free;

    DebugPrint("MUTESTS: Filling HD Partition: %s...\n", drive);

    sprintf(lpFile, "%sFHD.dmp", drive, GetCurrentThreadId());

    HANDLE file = CreateFile(lpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if(file == INVALID_HANDLE_VALUE)
        {
        return GetLastError();
        }

    // Get the disk space free
    GetDiskFreeSpaceEx(drive, &available, &total, &free);
    distance.QuadPart = (__int64)available.QuadPart - 256;
    size.QuadPart = 0;

    // Do a test write
    if(WriteFile(file, &w, wsize, &wsize, NULL) != TRUE)
        {
        error = GetLastError();
        CloseHandle(file);
        return error;
        }

    // write as big as a file as possible
    do
        {
        distance.QuadPart -= 256;
        if(distance.QuadPart < 0) break;

        if(SetFilePointerEx(file, distance, &size, FILE_BEGIN) != TRUE)
            {
            error = GetLastError();
            //DebugPrint("SetFilePointerEx (ec: %lu)\n", error);
            }
        else if(size.QuadPart != distance.QuadPart)
            {
            error = 99;
            //DebugPrint("SetFilePointerEx size != dist\n");
            }
        else if(WriteFile(file, &w, wsize, &wsize, NULL) != TRUE)
            {
            error = GetLastError();
            //DebugPrint("WriteFile (ec: %lu)\n", error);
            }
        else
            {
            error = 0;
            }

        } while(error);

    // clean up
    if(FlushFileBuffers(file) != TRUE)
        {
        error = GetLastError();
        //DebugPrint("FlushFileBuffers (ec: %lu)\n", error);
        }
    if(CloseHandle(file) != TRUE)
        {
        error = GetLastError();
        //DebugPrint("CloseHandle (ec: %lu)\n", error);
        }

    DebugPrint("MUTESTS: Filled  HD Partition. %ws(%I64d)\n", lpFile, size.QuadPart);

    return error;
    }

/*

Routine Description:

    Writes system resource usage information to the specified log file 
    and/or to the debugger.

    Be sure to include extern "C" { "ntos.h" } to use MmQueryStatistics

    MM_STATISTICS:
        ULONG TotalPhysicalPages;
        ULONG AvailablePages;
        ULONG VirtualMemoryBytesCommitted;
        ULONG VirtualMemoryBytesReserved;
        ULONG CachePagesCommitted;
        ULONG PoolPagesCommitted;
        ULONG StackPagesCommitted;
        ULONG ImagePagesCommitted;

Arguments:

    HANDLE hLog - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs
    bool debugger - true sends memory data to the debugger

Return Value:

    None

*/
void FlushDiskCache(void);

void LogResourceStatus(HANDLE hLog, bool debugger)
    {
    const DWORD pageSize = 4096;
    MM_STATISTICS mmStats;
    PS_STATISTICS psStats;

    // initialize the stat info
    memset(&mmStats, 0, sizeof(MM_STATISTICS));
    mmStats.Length = sizeof(MM_STATISTICS);
    memset(&psStats, 0, sizeof(PS_STATISTICS));
    psStats.Length = sizeof(PS_STATISTICS);

    FlushDiskCache();

    // gather all the data
    MmQueryStatistics(&mmStats);
    PsQueryStatistics(&psStats);

    // display/write the data
    if(hLog)
        {
        xLog(hLog, XLL_INFO, "Memory Status------------------------------------------");
        xLog(hLog, XLL_INFO, "   Total:     %lu", mmStats.TotalPhysicalPages*pageSize);
        xLog(hLog, XLL_INFO, "   Available: %lu", mmStats.AvailablePages*pageSize);
        xLog(hLog, XLL_INFO, "   Cache:     %lu", mmStats.CachePagesCommitted*pageSize);
        xLog(hLog, XLL_INFO, "   Pool:      %lu", mmStats.PoolPagesCommitted*pageSize);
        xLog(hLog, XLL_INFO, "   Stack:     %lu", mmStats.StackPagesCommitted*pageSize);
        xLog(hLog, XLL_INFO, "   Image:     %lu", mmStats.ImagePagesCommitted*pageSize);
        xLog(hLog, XLL_INFO, "   Handles:   %lu", psStats.HandleCount);
        xLog(hLog, XLL_INFO, "Memory Status------------------------------------------");
        }
    if(debugger)
        {
        DebugPrint("MEM: Available=%lu, Cache=%lu, Pool=%lu, Stack=%lu, Image=%lu, Handles=%lu\n", mmStats.AvailablePages*pageSize, mmStats.CachePagesCommitted*pageSize, mmStats.PoolPagesCommitted*pageSize, mmStats.StackPagesCommitted*pageSize, mmStats.ImagePagesCommitted*pageSize, psStats.HandleCount);
        }
    }

void FlushVolume(char *volume)
    {
    HANDLE hFile = CreateFile(volume, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        DebugPrint("MEM: FlushError: %lu, 0x%x\n", GetLastError(), GetLastError());
    FlushFileBuffers(hFile);
    CloseHandle(hFile);
    }

/*

Routine Description:

    This routine will flush the disk cache

Arguments:

    None

Return Value:

    No return value

*/
void FlushDiskCache(void)
    {
    ULONG Volume, Disk;
    HANDLE Handle;
    NTSTATUS Status;
    char VolumeBuffer[MAX_PATH];
    OBJECT_STRING VolumeString;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;

    for(Disk=0; Disk<2; Disk++)
        {
        for(Volume=1; Volume<4; Volume++)
            {
            sprintf(VolumeBuffer, "\\Device\\Harddisk%d\\Partition%d", Disk, Volume);

            RtlInitObjectString(&VolumeString, VolumeBuffer);

            InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

            Status = NtCreateFile(&Handle, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

            if(!NT_SUCCESS(Status)) continue;

            Status = NtFlushBuffersFile(Handle, &IoStatusBlock);
            NtClose(Handle);
            }
        }
    }

} // namespace MUTests
