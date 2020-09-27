/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    sdstress.h

Abstract:

    This the MAIN SdStress - Console header file.

Author / ported by:

    John Daly (johndaly) porting date 4/29/2000

Revision History:

    initial port: johndaly 4/29/2000
        port from NT

--*/

#ifndef _SDSTRESS_
#define _SDSTRESS_


//
// include files
//

#include <io.h>

//
// Function prototypes
//

unsigned long 
__stdcall 
SdStressThread (
    LPVOID pThreadData
    );

long 
GetNumberOfFiles (
    char DriveLetter
    );

char 
GetRandomFile(
    char DriveLetter, 
    char *pszRndFile, 
    unsigned long *FileLength, 
    long NumberOfFiles
    );

unsigned char * 
AllocateMemory (
    DWORD Size
    );

BOOL 
DeAllocateMemory (
    DWORD Size, 
    LPVOID pszBuffer
    );

long 
ParseReturnValue (
    COPYTESTFILE_DATA *CopyTestFileData, 
    THREAD_DATA *ThreadData
    );

unsigned long 
__stdcall 
VerifyData (
    LPVOID CTFD
    );

BOOL 
Close_Handle (
    HANDLE *phHandle
    );

void 
InitConsole (
    void
    );

BOOL 
WriteTextConsole (
    char *pszText, 
    WORD Attribute, 
    DWORD LoggedLevel
    );

void 
ExitConsole (
    void
    );

BOOL 
PrintError (
    long ErrorCode
    );

BOOL 
PrintWCError (
    long ErrorCode
    );

void 
Debug_Print (
    char *Text
    );

unsigned long 
__stdcall 
CopyTestFile (
    LPVOID pCopyTestFileData
    );

unsigned long 
__stdcall 
CopyTestFileUsingIoC (
    LPVOID pCopyTestFileData
    );

unsigned long 
__stdcall 
CopyTestFileMultiple (
    LPVOID pCopyTestFileData
    );

unsigned long 
__stdcall 
CopyTestFileUsingWriteFileEx (
    LPVOID pCopyTestFileData
    );

VOID
CALLBACK 
WriteFileExCompletionRoutine(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    );

#endif // _SDSTRESS_


