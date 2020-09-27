/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbcovcmdp.h

Abstract:

    This is a pre-compiled header module for xbcovcmd

--*/

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "coverage.h"

#if DBG
#include <assert.h>
#define ASSERT(exp) assert(exp)
#else
#define ASSERT(exp)
#endif

DWORD
CoverageSendCommand(
    IN  ULONG  Command,
    OUT PVOID  Buffer OPTIONAL,
    IN OUT SIZE_T *BufferSize OPTIONAL,
    IN PCSTR Format OPTIONAL,
    ...
    );

