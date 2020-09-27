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

namespace USBInput {


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
        DebugPrint("MEM: Available=%lu, Cache=%lu, Pool=%lu, Handles=%lu\n", mmStats.AvailablePages*pageSize, mmStats.CachePagesCommitted*pageSize, mmStats.PoolPagesCommitted*pageSize, psStats.HandleCount);
        }
    }

void FlushVolume(OCHAR *volume)
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
    OCHAR VolumeBuffer[MAX_PATH];
    OBJECT_STRING VolumeString;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;

    for(Disk=0; Disk<2; Disk++)
        {
        for(Volume=1; Volume<4; Volume++)
            {
            soprintf(VolumeBuffer, OTEXT("\\Device\\Harddisk%d\\Partition%d"), Disk, Volume);

            RtlInitObjectString(&VolumeString, VolumeBuffer);

            InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

            Status = NtCreateFile(&Handle, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

            if(!NT_SUCCESS(Status)) continue;

            Status = NtFlushBuffersFile(Handle, &IoStatusBlock);
            NtClose(Handle);
            }
        }
    }
} // namespace USBInput
