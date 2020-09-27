/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fileio.c
 *  Content:    Multi-streamed file read utility
 *
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include "fileio.h"
#include "xmvtool.h"

#define FILE_MAPPING_GRANULARITY    (64 * 1024)
#define FILE_WINDOW_SIZE            (2 * FILE_MAPPING_GRANULARITY)

/*
 * Opens the file, initializing the media file structure plus an initial 
 * stream.
 */

void OpenMediaFile
(
    MediaFile *pFile, 
    char *szFile
)
{
    // Make sure everything is initialized correctly.
    assert(!pFile->Initialized);

    pFile->hFile        = INVALID_HANDLE_VALUE;
    pFile->hFileMapping = NULL;
    pFile->pFileBase    = NULL;
    pFile->FileLength   = 0;
    pFile->RangeStart   = 0;
    pFile->RangeEnd     = 0;

    pFile->Initialized  = TRUE;
    
    // Open the file.
    pFile->hFile = CreateFileA(szFile, 
                               GENERIC_READ, 
                               FILE_SHARE_READ, 
                               NULL, 
                               OPEN_EXISTING, 
                               FILE_FLAG_NO_BUFFERING, 
                               NULL);

    if (pFile->hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "ERROR: Unable to open input file '%s'.\n", szFile);

        // Does not return.
        Done();
    }

    // Remember the file's size.
    if (!GetFileSizeEx(pFile->hFile, (LARGE_INTEGER *)&pFile->FileLength))
    {
        fprintf(stderr, "ERROR: Unable to read the input file's size.\n");

        // Does not return.
        Done();
    }

    // Create the mapping.
    pFile->hFileMapping = CreateFileMapping(pFile->hFile, 
                                            NULL,
                                            PAGE_READONLY,
                                            0,
                                            0,
                                            NULL);

    if (!pFile->hFileMapping)
    {
        fprintf(stderr, "ERROR: Unable to open input file '%s'.\n", szFile);

        // Does not return.
        Done();
    }
}

/*
 * Closes a file.  All streams are closed as well.
 */

void CloseMediaFile
(
    MediaFile *pFile
)
{
    if (pFile->Initialized)
    {
        if (pFile->pFileBase)
        {
            UnmapViewOfFile(pFile->pFileBase);
            pFile->pFileBase = NULL;
        }

        if (pFile->hFileMapping)
        {
            CloseHandle(pFile->hFileMapping);
            pFile->hFileMapping = NULL;
        }

        if (pFile->hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(pFile->hFile);
            pFile->hFile = INVALID_HANDLE_VALUE;
        }
    }
}

/*
 * Opens a stream in a file.
 */

void OpenMediaStream
(
    MediaFile *pFile, 
    MediaStream *pStream
)
{
    pStream->pFile  = pFile;
    pStream->Offset = 0;
}

/*
 * Returns the size of the file.
 */

LONGLONG GetMediaSize
(
    MediaStream *pStream
)
{
    return pStream->pFile->FileLength;
}

/*
 * Returns the stream's position in the file.
 */

LONGLONG GetStreamPosition
(
    MediaStream *pStream
)
{
    return pStream->Offset;
}


/*
 * Reset a stream back to the beginning of the file.
 */

void ResetMediaStream
(
    MediaStream *pStream
)
{
    pStream->Offset = 0;
}

/*
 * Clones the state of one stream into another.
 */

void CloneMediaStream
(
    MediaStream *pSource, 
    MediaStream *pDest
)
{
    memcpy(pDest, pSource, sizeof(MediaStream));
}

/*
 * Reads data out of a stream.  This pointer is only valid until the next
 * read.
 */

void *ReadStream
(
    MediaStream *pStream, 
    DWORD Size
)
{
    LONGLONG OffsetStart, OffsetEnd, NewStart;
    DWORD WindowSize;
    MediaFile *pFile;

    // We need to save 4K for alignment issues.
    assert(Size <= FILE_MAPPING_GRANULARITY);

    pFile = pStream->pFile;

    OffsetStart = pStream->Offset;
    OffsetEnd   = OffsetStart + Size;

    if (OffsetEnd > pFile->FileLength)
    {
        fprintf(stderr, "ERROR: Attempted to read beyond the end of the input file.\n");

        // Never returns.
        Done();
    }

    // If this part of the file is not mapped in, grab it.
    if (OffsetStart < pFile->RangeStart || OffsetEnd > pFile->RangeEnd)
    {
        // Destroy the current mapping.
        if (pFile->pFileBase)
        {
            UnmapViewOfFile(pFile->pFileBase);
            pFile->pFileBase = NULL;
        }

        // Create a new one on a 4k boundary in the file.
        NewStart = OffsetStart & ~(FILE_MAPPING_GRANULARITY - 1);

        if (NewStart + FILE_WINDOW_SIZE > pFile->FileLength)
        {
            WindowSize = (DWORD)(pFile->FileLength - NewStart);
        }
        else
        {
            WindowSize = FILE_WINDOW_SIZE;
        }

        pFile->pFileBase = (BYTE *)MapViewOfFile(pFile->hFileMapping,
                                                 FILE_MAP_READ,
                                                 (DWORD)(NewStart >> 32),
                                                 (DWORD)NewStart,
                                                 WindowSize);

        if (!pFile->pFileBase)
        {
            fprintf(stderr, "ERROR: Unable to read the input file (%d).\n", GetLastError());

            // Does not return.
            Done();
        }
        
        pFile->RangeStart = NewStart;
        pFile->RangeEnd   = NewStart + FILE_WINDOW_SIZE;
    }

    // Remember what we read.
    pStream->Offset = OffsetEnd;

    // Return the pointer.
    return pFile->pFileBase + (OffsetStart - pFile->RangeStart);
}

/*
 * Skips data in the stream.
 */

void SkipStreamData
(
    MediaStream *pStream, 
    LONGLONG Size
)
{
    pStream->Offset += Size;
}


