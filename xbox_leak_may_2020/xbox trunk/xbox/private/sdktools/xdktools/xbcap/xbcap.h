/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbcap.h

Abstract:

    Profile control utility - declarations

--*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//
// Error functions
//
VOID errmsg(CHAR* format, ...);
VOID checkerr(BOOL err, CHAR* format, ...);

extern BOOL verbose;
extern BOOL dumpdpc, rawdump, noidle;
extern CHAR* symbolpath;

//
// Dump a binary profile data file to human-readable text format
//
VOID capdump(CHAR* cap_filename, CHAR* dump_filename);

