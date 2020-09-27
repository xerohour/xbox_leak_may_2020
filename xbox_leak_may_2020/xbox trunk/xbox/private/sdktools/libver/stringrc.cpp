/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    stringrc.cpp

Abstract:

    This module implements miscellaneous utility routines for the 
    program to dump the version of an Xbox library.

--*/

#include "libverp.h"

VOID
LibvResourcePrintfV(
    FILE *file,
    UINT uStringID,
    va_list args
    )
{
    CHAR szResource[256];
    CHAR szMessage[1024];

    if (LoadString(NULL, uStringID, szResource, sizeof(szResource)) > 0) {

        FormatMessage(FORMAT_MESSAGE_FROM_STRING, szResource, 0, 0, szMessage,
            sizeof(szMessage), &args);

        fputs(szMessage, file);
    }
}

VOID
LibvResourcePrintf(
    FILE *file,
    UINT uStringID,
    ...
    )
{
    va_list args;

    va_start(args, uStringID);

    LibvResourcePrintfV(file, uStringID, args);

    va_end(args);
}

VOID
LibvResourcePrintLogoBanner(
    VOID
    )
{
    LibvResourcePrintf(stderr, IDS_LOGO_NAME_AND_VERSION, VER_PRODUCTVERSION_STR);
    fputs("\n", stderr);
    LibvResourcePrintf(stderr, IDS_LOGO_COPYRIGHT);
    fputs("\n\n", stderr);
}

VOID
LibvResourcePrintErrorAndExit(
    UINT uStringID,
    ...
    )
{
    va_list args;

    fprintf(stderr, "LIBVER : error IM%04d: ", uStringID);

    va_start(args, uStringID);

    LibvResourcePrintfV(stderr, uStringID, args);
    fputs("\n", stderr);

    va_end(args);

    LibvExitProcess(1);
}

VOID
LibvResourcePrintWarning(
    UINT uStringID,
    ...
    )
{
    va_list args;

    fprintf(stderr, "LIBVER : warning IM%04d: ", uStringID);

    va_start(args, uStringID);

    LibvResourcePrintfV(stderr, uStringID, args);
    fputs("\n", stderr);

    va_end(args);
}

VOID
LibvResourcePrintRange(
    FILE *file,
    UINT uStartingStringID
    )
{
    UINT uStringID;
    CHAR szResource[256];

    uStringID = uStartingStringID;

    for (;;) {

        if (LoadString(NULL, uStringID, szResource, sizeof(szResource)) == 0) {
            break;
        }

        if (szResource[0] == '~') {
            break;
        }

        fputs(szResource, file);
        fputs("\n", file);

        uStringID++;
    }
}
