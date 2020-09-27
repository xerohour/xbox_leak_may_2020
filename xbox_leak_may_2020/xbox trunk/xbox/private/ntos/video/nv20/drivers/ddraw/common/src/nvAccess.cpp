//======================================================================
// Module:		nvAccess.cpp
// Description:	Generic kernel access routines that are neutral to the calling OS
// 
//  Copyright (C) 2000 NVidia Corporation.  All Rights Reserved.
//======================================================================

#include "nvprecomp.h"

#ifdef NV_USE_ZW_PFUNCS

NV_ZW_FILE_IO_FUNCS NvZwFileIoFuncs = { NULL, NULL, NULL, NULL };

void NvInitFileIoFuncs( PDEV *ppdev )
{
    DWORD dwReturnStatus, dwReturnSize;

    dwReturnStatus = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_ZW_GET_FILE_IO_FUNCS,
                                         NULL, 0ul,
                                         &NvZwFileIoFuncs, sizeof(NvZwFileIoFuncs),
                                         &dwReturnSize );
}

#endif

void NVAnsiToUnicode(WCHAR *destString, TCHAR *srcString)
{
    int sLen;

    sLen = nvStrLen(srcString) + 1;

    while (sLen)
    {
        *destString = (USHORT)(*(UCHAR *)srcString);
        destString++;
        srcString++;
        sLen--;
    }
}

void NVUnicodeToAnsi(TCHAR *destString, WCHAR *srcString) 
{

    DWORD dwNullCnt = 0;
    DWORD dwPos = 0;

    while (srcString[dwPos] != 0) {
        destString[dwPos] = (UCHAR)srcString[dwPos];
        dwPos++;
    }
    destString[dwPos] = '\0';
}

HANDLE NvCreateFile( LPCTSTR lpFileName,                         // file name
                   DWORD dwDesiredAccess,                      // access mode
                   DWORD dwSharedMode,                          // share mode
                   LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
                   DWORD dwCreationDisposition,                // how to create
                   DWORD dwFlagsAndAttributes,                 // file attributes
                   HANDLE hTemplateFile                        // handle to template file
                   ) {

    HANDLE hFile = INVALID_HANDLE_VALUE;

#ifndef WINNT
    // Standard Win32 file creation
    hFile = CreateFile(lpFileName, dwDesiredAccess, dwSharedMode, lpSecurityAttributes, dwCreationDisposition,
                dwFlagsAndAttributes, hTemplateFile);
#else

    NV_ZW_CREATE_FILE_IN CreateFileIn;
    TCHAR device[64];
    DWORD dwReturnStatus, dwReturnSize = 0ul;

    nvAssert( pDriverData != NULL );
    if( pDriverData == NULL ) return INVALID_HANDLE_VALUE;

    if( ppdev == NULL ) return INVALID_HANDLE_VALUE;

    // Mass parameter conversion

    if (dwDesiredAccess & GENERIC_READ) {
        CreateFileIn.ulDesiredAccess = CreateFileIn.ulDesiredAccess ^ GENERIC_READ;
        CreateFileIn.ulDesiredAccess |= FILE_GENERIC_READ;
    }
    if (dwDesiredAccess & GENERIC_WRITE) {
        CreateFileIn.ulDesiredAccess = CreateFileIn.ulDesiredAccess ^ GENERIC_WRITE;
        CreateFileIn.ulDesiredAccess |= FILE_GENERIC_WRITE;
    }
    if (dwDesiredAccess & GENERIC_EXECUTE) {
        CreateFileIn.ulDesiredAccess = CreateFileIn.ulDesiredAccess ^ GENERIC_EXECUTE;
        CreateFileIn.ulDesiredAccess |= FILE_GENERIC_EXECUTE;
    }

    // get string name and make it Unicode, with the device path (\??\c:\booyah.txt, etc...)
    nvSprintf( device, TEXT("\\??\\%s"), lpFileName);

    NVAnsiToUnicode(CreateFileIn.pwcObjectBuffer, device);
    CreateFileIn.uiObjectName.Length = (USHORT)nvStrLen(device)*sizeof(WCHAR);
    CreateFileIn.uiObjectName.MaximumLength = 256*sizeof(WCHAR);
    CreateFileIn.uiObjectName.Buffer = CreateFileIn.pwcObjectBuffer;
    InitializeObjectAttributes(&CreateFileIn.ObjectAttributes, &CreateFileIn.uiObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    CreateFileIn.liAllocationSize.QuadPart = 0;

    CreateFileIn.ulShareAccess = dwSharedMode;

    //CreationDisposition
    if (dwCreationDisposition == CREATE_NEW)
        CreateFileIn.ulCreateDisposition = FILE_CREATE;
    else if (dwCreationDisposition == CREATE_ALWAYS)
        CreateFileIn.ulCreateDisposition = FILE_SUPERSEDE;
    else if (dwCreationDisposition == OPEN_EXISTING)
        CreateFileIn.ulCreateDisposition = FILE_OPEN;
    else if (dwCreationDisposition == OPEN_ALWAYS)
        CreateFileIn.ulCreateDisposition = FILE_OPEN_IF;
    else if (dwCreationDisposition == TRUNCATE_EXISTING)
        CreateFileIn.ulCreateDisposition = FILE_OVERWRITE;

    CreateFileIn.ulDesiredAccess = dwDesiredAccess;

    CreateFileIn.ulCreateOptions = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT;

    CreateFileIn.pvEaBuffer = NULL;
    CreateFileIn.ulEaLength = 0ul;

#ifdef NV_USE_ZW_PFUNCS
    if( NvZwFileIoFuncs.pvZwCreateFile == NULL ) {
        // Init the function pointers
        NvInitFileIoFuncs( ppdev );
        if( NvZwFileIoFuncs.pvZwCreateFile == NULL )
            return NULL;
    }

    {
        IO_STATUS_BLOCK IoBlock;
        dwReturnStatus = NvZwFileIoFuncs.pvZwCreateFile( &hFile,
                                       CreateFileIn.ulDesiredAccess,
                                       &CreateFileIn.ObjectAttributes,
                                       &IoBlock,
                                       &CreateFileIn.liAllocationSize,
                                       CreateFileIn.ulFileAttributes,
                                       CreateFileIn.ulShareAccess,
                                       CreateFileIn.ulCreateDisposition,
                                       CreateFileIn.ulCreateOptions,
                                       CreateFileIn.pvEaBuffer,
                                       CreateFileIn.ulEaLength );
    }
#else
    dwReturnStatus = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_ZW_CREATE_FILE,
                                         &CreateFileIn, sizeof(CreateFileIn),
                                         &hFile, sizeof(hFile),
                                         &dwReturnSize );
#endif


    if( dwReturnStatus )
        hFile = INVALID_HANDLE_VALUE;
#endif

    return hFile;

} // NvCreateFile


BOOL NvCloseHandle(HANDLE hFile) {

    BOOL bRes = FALSE;
#ifndef WINNT
    bRes = CloseHandle(hFile);
#else
    DWORD dwReturnStatus, dwReturnSize = 0ul;

    nvAssert( pDriverData != NULL );
    if( pDriverData == NULL ) return FALSE;

    if( ppdev == NULL ) return FALSE;


#ifdef NV_USE_ZW_PFUNCS
    if( NvZwFileIoFuncs.pvZwClose == NULL ) {
        // Init the function pointers
        NvInitFileIoFuncs( ppdev );
        if( NvZwFileIoFuncs.pvZwClose == NULL )
            return NULL;
    }

    dwReturnStatus = NvZwFileIoFuncs.pvZwClose( hFile );
#else
    dwReturnStatus = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_ZW_CLOSE_HANDLE,
                                         &hFile, sizeof(hFile),
                                         NULL, 0ul,
                                         &dwReturnSize );
#endif

    if( dwReturnStatus )
        bRes = FALSE;
    else
        bRes = TRUE;
#endif

    return bRes;
} // NvCloseHandle


BOOL NvWriteFile( HANDLE hFile,                    // handle to file
                LPCVOID lpBuffer,                // data buffer
                DWORD nNumberOfBytesToWrite,     // number of bytes to write
                LPDWORD lpNumberOfBytesWritten,  // number of bytes written
                LPOVERLAPPED lpOverlapped        // overlapped buffer
                ) 
{
    BOOL bRes;


#ifndef WINNT
    bRes = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
#else
    NV_ZW_RW_FILE_IN WriteFileIn;
    DWORD dwReturnStatus, dwReturnSize = 0ul;

    nvAssert( pDriverData != NULL );
    if( pDriverData == NULL ) return FALSE;

    if( ppdev == NULL ) return TRUE;

    WriteFileIn.pvFileHandle = hFile;
    WriteFileIn.pvEvent = NULL;
    WriteFileIn.pvApcRoutine = NULL;
    WriteFileIn.pvApcContext = NULL;
    WriteFileIn.pvBuffer = (LPVOID)lpBuffer;
    WriteFileIn.ulLength = nNumberOfBytesToWrite;

    if (lpOverlapped != NULL) {
        WriteFileIn.liByteOffset.QuadPart = lpOverlapped->Offset;
    }
    else {
        WriteFileIn.liByteOffset.QuadPart = 0ul;
    }

    WriteFileIn.pvKey = NULL;

#ifdef NV_USE_ZW_PFUNCS
    if( NvZwFileIoFuncs.pvZwWriteFile == NULL ) {
        // Init the function pointers
        NvInitFileIoFuncs( ppdev );
        if( NvZwFileIoFuncs.pvZwWriteFile == NULL )
            return FALSE;
    }

    {
        IO_STATUS_BLOCK IoBlock;
        dwReturnStatus = NvZwFileIoFuncs.pvZwWriteFile( WriteFileIn.pvFileHandle,
                                                        WriteFileIn.pvEvent,
                                                        WriteFileIn.pvApcRoutine,
                                                        WriteFileIn.pvApcContext,
                                                        &IoBlock,
                                                        WriteFileIn.pvBuffer,
                                                        WriteFileIn.ulLength,
                                                        &WriteFileIn.liByteOffset,
                                                        WriteFileIn.pvKey );
    }
#else
    dwReturnStatus = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_ZW_WRITE_FILE,
                                        &WriteFileIn, sizeof(WriteFileIn),
                                        NULL, 0,
                                        &dwReturnSize );
#endif

    if (dwReturnStatus) {
        bRes = FALSE;
        *lpNumberOfBytesWritten = 0;
    }
    else {
        bRes = TRUE;
        *lpNumberOfBytesWritten = nNumberOfBytesToWrite;
    }
#endif
    
    return bRes;
} // NvWriteFile

BOOL NvReadFile(HANDLE hFile,                // handle to file
                LPVOID lpBuffer,             // data buffer
                DWORD nNumberOfBytesToRead,  // number of bytes to read
                LPDWORD lpNumberOfBytesRead, // number of bytes read
                LPOVERLAPPED lpOverlapped    // overlapped buffer
                )  
{
    BOOL bRes;

#ifndef WINNT
    bRes = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
#else
    NV_ZW_RW_FILE_IN ReadFileIn;
    DWORD dwReturnStatus, dwReturnSize = 0ul;

    nvAssert( pDriverData != NULL );
    if( pDriverData == NULL ) return FALSE;

    if( ppdev == NULL ) return FALSE;

    // Convert To NT style params.
    if (lpOverlapped != NULL) {
        ReadFileIn.liByteOffset.QuadPart = lpOverlapped->Offset;
    }

    ReadFileIn.pvFileHandle = hFile;
    ReadFileIn.pvApcContext = NULL;
    ReadFileIn.pvApcRoutine = NULL;
    ReadFileIn.pvBuffer = lpBuffer;
    ReadFileIn.pvEvent = NULL;
    ReadFileIn.pvKey = NULL;
    ReadFileIn.ulLength = nNumberOfBytesToRead;

#ifdef NV_USE_ZW_PFUNCS
    if( NvZwFileIoFuncs.pvZwReadFile == NULL ) {
        // Init the function pointers
        NvInitFileIoFuncs( ppdev );
        if( NvZwFileIoFuncs.pvZwReadFile == NULL )
            return FALSE;
    }

    {
        IO_STATUS_BLOCK IoBlock;
        dwReturnStatus = NvZwFileIoFuncs.pvZwReadFile( ReadFileIn.pvFileHandle,
                                                       ReadFileIn.pvEvent,
                                                       ReadFileIn.pvApcRoutine,
                                                       ReadFileIn.pvApcContext,
                                                       &IoBlock,
                                                       ReadFileIn.pvBuffer,
                                                       ReadFileIn.ulLength,
                                                       &WriteFileIn.liByteOffset,
                                                       ReadFileIn.pvKey );
    }
#else
    dwReturnStatus = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_ZW_READ_FILE,
                                        &ReadFileIn, sizeof(ReadFileIn),
                                        NULL, 0,
                                        &dwReturnSize );
#endif

    if (dwReturnStatus) {
        bRes = FALSE;
        *lpNumberOfBytesRead = 0;
    }
    else {
        bRes = TRUE;
        *lpNumberOfBytesRead = nNumberOfBytesToRead;
    }

#endif

    return bRes;
}



BOOL NvDeviceIoControl(
  HANDLE hDevice,              // handle to device
  DWORD dwIoControlCode,       // operation control code
  LPVOID lpInBuffer,           // input data buffer
  DWORD nInBufferSize,         // size of input data buffer
  LPVOID lpOutBuffer,          // output data buffer
  DWORD nOutBufferSize,        // size of output data buffer
  LPDWORD lpBytesReturned,     // byte count
  LPOVERLAPPED lpOverlapped    // overlapped information
)
{
    BOOL bRes;
#ifndef WINNT
    bRes =  DeviceIoControl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned,
                            lpOverlapped);
#else
    NTSTATUS status;
    NV_ZW_DEVICE_IO_CONTROL_FILE ZwDeviceIoControl;

    nvAssert( pDriverData != NULL );
    if( pDriverData == NULL ) return false;

    if( ppdev == NULL ) return false;

    ZwDeviceIoControl.hDevice = hDevice;
    ZwDeviceIoControl.ulIoControlCode = dwIoControlCode;
    ZwDeviceIoControl.pvInputBuffer = lpInBuffer;
    ZwDeviceIoControl.ulInputBufferSize = nInBufferSize;

#ifdef NV_USE_ZW_PFUNCS
    if( NvZwFileIoFuncs.pvZwDeviceIoControlFile == NULL ) {
        // Init the function pointers
        NvInitFileIoFuncs( ppdev );
        if( NvZwFileIoFuncs.pvZwDeviceIoControlFile == NULL )
            return false;
    }

    {
        IO_STATUS_BLOCK IoBlock;
        dwReturnStatus = NvZwFileIoFuncs.pvZwDeviceIoControlFile( hDevice,
                                                NULL,
                                                NULL,
                                                NULL,
                                                &IoBlock,
                                                dwIoControlCode,
                                                lpInBuffer,
                                                nInBufferSize,
                                                lpOutBuffer,
                                                nOutBufferSize );
    }
#else
    status = EngDeviceIoControl( ppdev->hDriver, IOCTL_VIDEO_CONTROL_DEVICE,
                                 &ZwDeviceIoControl, sizeof(ZwDeviceIoControl),
                                 lpOutBuffer, nOutBufferSize,
                                 lpBytesReturned );
#endif


    if (status == STATUS_SUCCESS) {
        bRes = true;
    }
    else {
        *lpBytesReturned = 0ul;
        bRes = false;
    }

#endif

    return bRes;
}

#pragma warning(disable: 4035)

DWORD NvGetTickCount()
{
#ifndef WINNT
    return GetTickCount();
#else
    __asm {
            mov     edx, 07ffe0000h
            mov     eax, [edx]
            mul     dword ptr [edx+4]
            shrd    eax,edx,18h
    };
#endif
}