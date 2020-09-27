/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    DVDRead.cpp

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

    Tests the Read data (low level) from the DVD

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD FILEIO::DVDRead_Test(HANDLE hLog, DWORD ThreadID)
    {
    extern DWORD stressMode;
    /*
    */
    const unsigned blockSize = 2048;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK statusBlock;
    HANDLE hVolume;
    LARGE_INTEGER offset;
    OBJECT_STRING deviceName;
    if(stressMode != 1) return 1; // only run this test in stress mode

    char *buffer = new char[blockSize+128];
    if(!buffer)
        {
        DebugPrint("FILEIO(%u): DVDRead_Test - Could not allocate enough memory\n", ThreadID);
        return 1;
        }


    RtlInitObjectString(&deviceName, "\\Device\\CdRom0");
    InitializeObjectAttributes(&obja, (POBJECT_STRING)&deviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_READ,
                        &obja,
                        &statusBlock,
                        FILE_SHARE_READ,
                        FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);
    if(NT_ERROR(status))
        {
        return 1;
        }

    unsigned readErrors = 0;

    offset.QuadPart = blockSize;
    for(unsigned i=0; i<50000; i++)
        {
        status = NtReadFile(hVolume,
                             0,
                             NULL,
                             NULL,
                             &statusBlock,
                             buffer,
                             blockSize,
                             &offset
                             );
        if(NT_ERROR(status))
            {
            ++readErrors;
            }
        offset.QuadPart += blockSize;
        }

    DebugPrint("FILEIO(%u): DVD Read Error Count %u\n", ThreadID, readErrors);

    delete[] buffer;
    NtClose(hVolume);

    return 0;
    }
