/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    stringrc.cpp

Abstract:

    This module implements miscellaneous utility routines for the utility
    program to build an Xbox executable image.

--*/

#include "imgbldp.h"

VOID
ImgbResourcePrintfV(
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
ImgbResourcePrintf(
    FILE *file,
    UINT uStringID,
    ...
    )
{
    va_list args;

    va_start(args, uStringID);

    ImgbResourcePrintfV(file, uStringID, args);

    va_end(args);
}

VOID
ImgbResourcePrintLogoBanner(
    VOID
    )
{
    ImgbResourcePrintf(stderr, IDS_LOGO_NAME_AND_VERSION, VER_PRODUCTVERSION_STR);
    fputs("\n", stderr);
    ImgbResourcePrintf(stderr, IDS_LOGO_COPYRIGHT);
    fputs("\n\n", stderr);
}

VOID
ImgbResourcePrintErrorAndExit(
    UINT uStringID,
    ...
    )
{
    va_list args;

    fprintf(stderr, "IMAGEBLD : error IM%04d: ", uStringID);

    va_start(args, uStringID);

    ImgbResourcePrintfV(stderr, uStringID, args);
    fputs("\n", stderr);

    va_end(args);

    ImgbExitProcess(1);
}

VOID
ImgbResourcePrintWarning(
    UINT uStringID,
    ...
    )
{
    va_list args;

    fprintf(stderr, "IMAGEBLD : warning IM%04d: ", uStringID);

    va_start(args, uStringID);

    ImgbResourcePrintfV(stderr, uStringID, args);
    fputs("\n", stderr);

    va_end(args);
}

VOID
ImgbResourcePrintRange(
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
