/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

   mediadet.h

Abstract:

   support ReadFileTest.c

Author:

   John Daly

Notes:


--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <ntos.h>
#include <winbase.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <smcdef.h>
#include "dxconio.h"

#define BUTTON_PRESS_VALUE      100

extern
NTSTATUS
WINAPI
XWriteTitleInfoNoReboot(
    PCOSTR pszLaunchPath,
    PCOSTR pszDDrivePath,
    DWORD dwLaunchDataType,
    DWORD dwTitleId,
    PLAUNCH_DATA pLaunchData
    );

//
// global data
//

#define SOFT_REBOOT 1
#define HARD_REBOOT 2
#define SMC_RESET   3

#define DATASLOTS 10

DWORD RebootType = HARD_REBOOT; // default to this
char buffer[500] = {0};

struct _fbuffer {
    DWORD iteration;
    DWORD SizesDetected;
    DWORD RebootType;
    struct _DetectData {
        DWORD SizeDetectedCount;
        ULARGE_INTEGER SizeDetected;
    } DetectData[DATASLOTS];
} fbuffer = {0};

//
// function declarations
//

void 
__cdecl 
main(
    void
    );

DWORD 
__cdecl 
RebootMenu(
    void
    );

void 
__cdecl 
DumpStats(
    void
    );

void
__cdecl 
InputDukeInsertions(
    DWORD add, 
    DWORD remove
    );

BOOL
__cdecl 
InputCheckButton(
    int button
    );
