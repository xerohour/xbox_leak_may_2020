/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    libverp.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the program to dump the version of an Xbox library.

--*/

#ifndef _LIBVERP_
#define _LIBVERP_

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>
#include <xbeimage.h>
#include <cklibver.h>
#include "libverrc.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

//
// General support functions.
//

DECLSPEC_NORETURN
VOID
LibvExitProcess(
    int ExitCode
    );

//
// String resource support functions.
//

VOID
LibvResourcePrintfV(
    FILE *file,
    UINT uStringID,
    va_list args
    );

VOID
LibvResourcePrintf(
    FILE *file,
    UINT uStringID,
    ...
    );

VOID
LibvResourcePrintErrorAndExit(
    UINT uStringID,
    ...
    );

VOID
LibvResourcePrintWarning(
    UINT uStringID,
    ...
    );

VOID
LibvResourcePrintRange(
    FILE *file,
    UINT uStartingStringID
    );

VOID
LibvResourcePrintLogoBanner(
    VOID
    );

#endif  // _LIBVERP_
