/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fileio.h
 *  Content:    Multi-streamed file read utility
 *
 ****************************************************************************/

#define FILE_MAPPING_GRANULARITY    (64 * 1024)
#define FILE_WINDOW_SIZE            (2 * FILE_MAPPING_GRANULARITY)

/*
 * It's kinda weird that we need such a complicated file-io process for what
 * amounts to a silly little tool but we ultimately need to be able to read
 * multiple streams through a file or even the same stream multiple times.
 * We do all this complication here to avoid having to do any of this on the 
 * Xbox when all it wants to do is play a movie.
 *
 * This may not be the optimal way of doing this but should work for now.
 */

typedef struct _MediaFile MediaFile;

/*
 * A stream in the file.  Must be caller-allocated but does not need to be
 * zero-initialized.
 */

typedef struct _MediaStream
{
    // The containing file.
    MediaFile *pFile;

    // The current offset of this stream in the file, from the first byte of
    // the file.
    //
    LONGLONG Offset;
}
MediaStream;

/*
 * The main structure for a file.  This must be allocatedby the caller.  
 * Closing this file closes all streams.
 */ 

typedef struct _MediaFile
{
    // Only used so this structure can be zero-initialized.
    BOOL Initialized;

    // The file information.
    HANDLE hFile;
    HANDLE hFileMapping;
    BYTE  *pFileBase;

    LONGLONG FileLength;

    // The range of the file mapped in.
    LONGLONG RangeStart;
    LONGLONG RangeEnd;
}
MediaFile;

/*
 * APIs.  All of these methods call "Done()" if there is an error.
 */

// Opens the file, initializing the media file structure plus an initial 
// stream.
//
void OpenMediaFile(MediaFile *pFile, char *szFile);

// Closes a file.  All streams are closed as well.
void CloseMediaFile(MediaFile *pFile);

// Opens a stream in a file.
void OpenMediaStream(MediaFile *pFile, MediaStream *pStream);

// Returns the size of the file.
LONGLONG GetMediaSize(MediaStream *pStream);

// Returns the stream's position in the file.
LONGLONG GetStreamPosition(MediaStream *pStream);

// Reset a stream back to the beginning of the file.
void ResetMediaStream(MediaStream *pStream);

// Clones the state of one stream into another.
void CloneMediaStream(MediaStream *pSource, MediaStream *pDest);

// Reads data out of a stream.  This pointer is only valid until the next 
// read.
//
void *ReadStream(MediaStream *pStream, DWORD Size);

// Skips data in the stream.
void SkipStreamData(MediaStream *pStream, LONGLONG Size);
