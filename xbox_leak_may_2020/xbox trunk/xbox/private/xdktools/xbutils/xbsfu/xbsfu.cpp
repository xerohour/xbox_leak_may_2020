/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    xbsfu.cpp

Abstract:

    This module implements routines to update system files on XDK.

--*/

#include "stdafx.h"

HRESULT
WINAPI
SfuUpdateSystemFile(
    IN CONST CHAR* Source,
    IN CONST CHAR* Destination,
    IN PDM_CONNECTION Connection
    )
{
    HRESULT hr = E_FAIL;
    HANDLE FileHandle = INVALID_HANDLE_VALUE;
    HANDLE MapHandle = NULL;
    PVOID MapView = NULL;
    LARGE_INTEGER FileSize;
    DWORD CRC;
    CHAR CommandBuffer[512];
    CHAR Response[128];
    DWORD ResponseSize = sizeof(Response);

    __try {

        FileHandle = CreateFile(Source, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, 0);

        if (FileHandle == INVALID_HANDLE_VALUE) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            __leave;
        }

        if (!GetFileSizeEx(FileHandle, &FileSize)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            __leave;
        }

        if (FileSize.HighPart) {
            hr = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
            __leave;
        }

        MapHandle = CreateFileMapping(FileHandle, 0, PAGE_READONLY, 0, 0, 0);

        if (!MapHandle) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            __leave;
        }

        MapView = MapViewOfFile(MapHandle, FILE_MAP_READ, 0, 0, 0);

        if (!MapView) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            __leave;
        }

        CRC = CRC32(~0UL, MapView, FileSize.LowPart);

        if (_snprintf(CommandBuffer, sizeof(CommandBuffer) - 1,
                "sysfileupd name=%s crc=0x%x size=0x%x", Destination, CRC,
                FileSize.LowPart) < 0) {
            hr = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
            __leave;
        }

        hr = DmSendCommand(Connection, CommandBuffer, Response, &ResponseSize);

        if (hr != XBDM_READYFORBIN) {
            __leave;
        }

        hr = DmSendBinary(Connection,  MapView, FileSize.LowPart);

    } __except(EXCEPTION_EXECUTE_HANDLER) {
        hr = HRESULT_FROM_NT(GetExceptionCode());
    }

    if (FileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(FileHandle);
    }

    if (MapHandle) {
        CloseHandle(MapHandle);
    }

    if (MapView) {
        UnmapViewOfFile(MapView);
    }

    return hr;
}

void usage()
{
    fprintf(stderr,
        "Xbox System File Update Version " VER_PRODUCTVERSION_STR "\n"
        "Copyright (c) Microsoft Corporation.  All rights reserved.\n\n"
        "Usage:\t  xbsfu source destination\n\n"
        "Example:  The following command will upload xbdm.dll from current\n"
        "\t  directory to first partition on Xbox hard drive\n\n"
        "\t  xbsfu xbdm.dll \\device\\harddisk0\\partition1\\xbdm.dll\n");
    exit(1);
}

int __cdecl main(int argc, char* argv[])
{
    PDM_CONNECTION pdmc = NULL;
    HRESULT hr;

    if (argc != 3) {
        usage();
    }

    hr = DmOpenConnection(&pdmc);

    if (SUCCEEDED(hr)) {
        hr = SfuUpdateSystemFile(argv[1], argv[2], pdmc);
    }

    if (FAILED(hr)) {
        char buffer[128];
        DmTranslateError(hr, buffer, sizeof(buffer));
        fprintf(stderr, "%s\n", buffer);
    } else {
        fprintf(stderr, "The operation completed successfully\n");
    }

    if (pdmc) {
        DmCloseConnection(pdmc);
    }

    return FAILED(hr);
}