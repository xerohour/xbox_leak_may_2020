/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    testgame.h

Abstract:

    support for testgame.c
    
Author:

    John Daly (johndaly) 30-Oct-2000

Revision History:

--*/

//
// includes
//

#undef _UNICODE
#undef UNICODE

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <stdio.h>
#include <tchar.h>
#include "ntos.h"
#include <xbeimage.h>
#include <ldr.h>
#include <xcrypt.h>
#include <xtestlib.h>
#include <xboxp.h>
#include <xbox.h>
#include <xapip.h>
#include "xlaunch.h"

//
// global data
//

LARGE_INTEGER Frequency;
LARGE_INTEGER PerformanceCount;
PVOID FileDataBuffer;

//
// defines
//

#define FILE_DATA_SIZE 0x400000

//
// stolen from pathmisc.c
//

#define MU_FILENAME "muname.xbx"
#define MU_FILENAME_LENGTH (sizeof(MU_FILENAME) - 1)

typedef struct TEST_PARTITION_INFO {
    TCHAR DeviceName[5];
    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    DWORD TotalNumberOfFiles;
    UINT DriveType;
} TestPartitionInfo, *pTestPartitionInfo;

TestPartitionInfo TPO[26] = {0};

XDEVICE_PREALLOC_TYPE deviceTypes[] = 
{
    {XDEVICE_TYPE_GAMEPAD,4},
    {XDEVICE_TYPE_MEMORY_UNIT,8},
    {XDEVICE_TYPE_VOICE_MICROPHONE,4},
    {XDEVICE_TYPE_VOICE_HEADPHONE,4}
};
    
LAUNCH_DATA LaunchData = {0};

DWORD XIDMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };
DWORD XMUMasks[] = {
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_TOP_MASK,
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT3_BOTTOM_MASK
    };

//
// list of all found drives
//

TCHAR Drives[27] = {0};

//
// list of all UData drives / MUs (U + MUs)
//

TCHAR UDataDrives[27] = {0};

//
// allocate here instead of wasting stack space...
//

char FindPath[350] = {0};
WIN32_FIND_DATA FindData = {0};
XGAME_FIND_DATA FindGameData = {0};

//
// macro to return the number of elements in an array
//

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))

TCHAR DeviceLetters[] = {TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ")};
TCHAR MULetters[] = {TEXT("FGHIJKLM")};

//
// function declarations
//

void 
__cdecl 
main(
    void
    );

ULONG
GetTitleID(
    void
    );

void
CheckMachineStatePre(
    ULONG TestID,
    HANDLE hLogFile
    );

void
CheckMachineStatePost(
    ULONG TestID,
    HANDLE hLogFile
    );

void
SaveSomeGames(
    ULONG TestID,
    HANDLE hLogFile
    );

void
FillCachePartition(
    ULONG TestID,
    HANDLE hLogFile
    );

void
UseTitlePersistentData(
    ULONG TitleID,
    HANDLE hLogFile
    );

void
CheckConfigData(
    ULONG TitleID,
    HANDLE hLogFile
    );

void
SetupNextRun(
    ULONG TestID,
    HANDLE hLogFile
    );

void
LogText(
    ULONG TitleID,
    HANDLE hLogFile,
	TCHAR *format,
	...
    );

void 
LogDiskStuff(
    DWORD TitleID, 
    HANDLE hLogFile
    );

void 
AltTitleTest(
    DWORD TitleID, 
    HANDLE hLogFile
    );

void
MountMUs(
    DWORD TitleID, 
    HANDLE hLogFile
    );

void
NameMUs(
    DWORD TitleID, 
    HANDLE hLogFile
    );

void
UseNickNames(
    DWORD TitleID, 
    HANDLE hLogFile
    );

void
CheckTitleInfo(
    DWORD TitleID, 
    HANDLE hLogFile
    );

VOID
WINAPI
StartTimer(
    VOID
    );

DWORD
WINAPI
EndTimer(
    VOID
    );

