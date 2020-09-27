//======================================================================
// Module:		nvFile.cpp
// Description:	Generic file routines that are neutral to the calling OS
// 
//  Copyright (C) 2000 NVidia Corporation.  All Rights Reserved.
//======================================================================

#ifndef NVFILE_H
#define NVFILE_H

#include <nvprecomp.h>

bool NvReadFile(HANDLE hFile,                // handle to file
                LPVOID lpBuffer,             // data buffer
                DWORD nNumberOfBytesToRead,  // number of bytes to read
                LPDWORD lpNumberOfBytesRead, // number of bytes read
                LPOVERLAPPED lpOverlapped    // overlapped buffer
                );

bool NvWriteFile( HANDLE hFile,                    // handle to file
                LPCVOID lpBuffer,                // data buffer
                DWORD nNumberOfBytesToWrite,     // number of bytes to write
                LPDWORD lpNumberOfBytesWritten,  // number of bytes written
                LPOVERLAPPED lpOverlapped        // overlapped buffer
                );

bool NvCloseHandle(HANDLE h);

HANDLE NvCreateFile(LPCTSTR lpFileName,                         // file name
                   DWORD dwDesiredAccess,                      // access mode
                   DWORD dwShareMode,                          // share mode
                   LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
                   DWORD dwCreationDisposition,                // how to create
                   DWORD dwFlagsAndAttributes,                 // file attributes
                   HANDLE hTemplateFile                        // handle to template file
                   );
#endif
