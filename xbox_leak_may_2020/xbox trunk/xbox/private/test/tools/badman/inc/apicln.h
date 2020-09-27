/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    apicln.c

Abstract:

    This module contains functions for cleaning up whatever resources were
    created by an API which was tested.

Author:

    John Miller (johnmil) 19-Mar-1992

Environment:

    XBox

Revision History:

    11-Apr-2000     schanbai

        Added more clean up routines

--*/

#include <windows.h>
#include <stdio.h>
#include <xlog.h>

VOID
ErrorPrint(
    HANDLE hLog,
    char *Module,
    int CaseNo,
    char *Message
    );

INT
APrintf(
    char *Format,
    ...
    );
