/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    savegame.h

Abstract:

    support for savegame.c
    
Author:

    John Daly (johndaly) 22-June-2000

Revision History:

--*/

#ifndef _SAVEGAME_
#define _SAVEGAME_

//
// includes
//

#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <xlog.h>
#include <xtestlib.h>

namespace SaveGameNameSpace {

//
// list of all UData drives / MUs (U + MUs)
//

char UDataDrives[27] = {0};

//
// list of all found drives
//

char Drives[27] = {0};

//
// global data
//

//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//

LONG SaveGameThreadID = 0;

//
// Heap handle from HeapCreate
//

HANDLE HeapHandle;

WCHAR wcDbgMsgBuffer[500] = {0};
PVOID FileDataBuffer;
#define FILE_DATA_SIZE 0x4000
long holdrand;
int MaxFiles;

char DeviceLetters[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
char MULetters[] = {"FGHIJKLM"};

//
// RIP stuff
//

BYTE Aspace[5] = {0};
BYTE Bspace[4] = {0};
BYTE *Address;
BYTE *jump_target = (BYTE *)Bspace;

DWORD RIPSaveRegsEBX;
DWORD RIPSaveRegsESI;
DWORD RIPSaveRegsEDI;

#if DBG_RIP
    #define PreRIPSaveRegs              \
        _asm mov RIPSaveRegsEBX, ebx    \
        _asm mov RIPSaveRegsESI, esi    \
        _asm mov RIPSaveRegsEDI, edi

    #define RIPRestoreSaveRegs             \
        _asm mov ebx, RIPSaveRegsEBX       \
        _asm mov esi, RIPSaveRegsESI       \
        _asm mov edi, RIPSaveRegsEDI
#else
    #define PreRIPSaveRegs
    #define RIPRestoreSaveRegs
#endif

//
// macros
//

//
// macro to return the number of elements in an array
//

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))

//
// defines
//

XDEVICE_PREALLOC_TYPE deviceTypes[] = 
{
    {XDEVICE_TYPE_GAMEPAD,4},
    {XDEVICE_TYPE_MEMORY_UNIT,8},
    {XDEVICE_TYPE_VOICE_MICROPHONE,4},
    {XDEVICE_TYPE_VOICE_HEADPHONE,4}
};
    
}   //SaveGameNameSpace

//
// function declarations
//

VOID
WINAPI
test_XSignature(
    HANDLE LogHandle
    );

VOID
WINAPI
test_XCreateSaveGame(
    HANDLE LogHandle,
    char uDrive
    );

VOID
WINAPI
test_XDeleteSaveGame(
    HANDLE LogHandle,
    char uDrive
    );

VOID
WINAPI
test_XFindFirstSaveGame(
    HANDLE LogHandle,
    char uDrive
    );

VOID
WINAPI
test_XFindNextSaveGame(
    HANDLE LogHandle,
    char uDrive
    );

VOID
WINAPI
test_XFindClose(
    HANDLE LogHandle,
    char uDrive
    );

VOID
WINAPI
StartTest(
    HANDLE LogHandle
    );

VOID
WINAPI
EndTest(
    VOID
    );

BOOL
WINAPI
DllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    );

BOOL
WINAPI
DeleteAllSavedGames(
    char uDrive
    );

BOOL
WINAPI
SaveAGame(
    PCHAR SavePath,
    PCHAR BaseFileName,
    PCHAR FileNameExt,
    DWORD FileSize,
    DWORD FileCount
    );

PWCHAR
WINAPI
AppendRandUnicodeChar(
    PWCHAR pwStr
    );

void 
__cdecl 
xtsrand(
    unsigned int seed
    );

int 
__cdecl 
xtrand (
    void
    );

int 
Eval_Exception ( 
    PEXCEPTION_POINTERS pExceptionPointers, 
    DWORD ArgBytes
    );

void
DummyCode(
    void
    );

void
MountMUs(
    void
    );


#endif // _SAVEGAME_


