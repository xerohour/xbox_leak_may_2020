/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xsndtrk.c

Abstract:

    XTL Soundtrack Enumeration APIs

Notes:

    Provides support for enumerating the soundtracks a user has ripped through
    the dash and enumerating the songs w/in those soundtracks.

--*/

#include "basedll.h"
#include <xboxp.h>

const OBJECT_STRING XapiStDbFile = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\TDATA\\FFFE0000\\MUSIC\\ST.DB"));
#define STDB_MUSICDIR "\\Device\\Harddisk0\\partition1\\TDATA\\FFFE0000\\MUSIC\\"


//
// Lookaside variables for fast access to data when querying for songs.
//
STDB_LIST XapiListLookaside = {0,INVALID_STID};
STDB_STDESC XapiStLookaside = {0,INVALID_STID};

BOOL
XapiReadFromStDb (
    HANDLE Handle,
    PBYTE Buffer,
    DWORD Signature
    )

/*++

Routine Description:

    Simple helper routing for reading data from the database.

Arguments:

    Handle - Contains a valid file handle of the open soundtrack database.
    Buffer - Used as a buffer for the read.
    Signature - If non-zero, assumes that the first DWORD in the buffer is a signature
                and checks it against this value.

Return Value:

    TRUE if the data was successfully read and the (optional) signature matches. FALSE otherwise.

Notes:



--*/

{

    DWORD byteCount;
    return ReadFile (Handle, Buffer, STDB_PAGE_SIZE, &byteCount, NULL)
        && byteCount == STDB_PAGE_SIZE
        && (!Signature || (*(PDWORD) Buffer) == Signature);
}


NTSTATUS
XapiOpenStDbAndReadHeader (
    HANDLE * Handle,
    PBYTE Buffer
    )

/*++

Routine Description:

    Opens the soundtrack database and validates the header.

Arguments:

    Handle - Receives a valid file handle on success.
    Buffer - Used as a buffer for reading. Receives the header structure of
             the soundtrack database on success.

Return Value:

    STATUS_SUCCESS or error code.

Notes:



--*/

{

    NTSTATUS status;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;



    InitializeObjectAttributes (&obja, (POBJECT_STRING) &XapiStDbFile, OBJ_CASE_INSENSITIVE, NULL, NULL);

    //
    // Attempt to open the soundtrack database.
    //

    status = NtCreateFile (
                Handle,
                SYNCHRONIZE | GENERIC_READ,
                &obja,
                &iosb,
                NULL,
                FILE_ATTRIBUTE_SYSTEM | FILE_NO_INTERMEDIATE_BUFFERING,
                FILE_SHARE_READ,
                FILE_OPEN,
                FILE_SYNCHRONOUS_IO_NONALERT
                );

    if (NT_SUCCESS(status) && !XapiReadFromStDb (*Handle, Buffer, 0)) {

            status = STATUS_UNSUCCESSFUL;
            NtClose (*Handle);
    }

    return status;

}



BOOL
XapiGetNextSoundtrack (
    PBYTE Buffer,
    PSNDTRK_ENUMSTATE State,
    PXSOUNDTRACK_DATA Data
    )

/*++

Routine Description:

    Worker function for retrieving the next soundtrack from the database.

Arguments:

    Buffer - Buffer for reading from the soundtrack database.
    State - State variable used for internal tracking of enumeration data.
    Data - receives soundtrack data for the next soundtrack.

Return Value:

    TRUE if there are additional soundtracks to enumerate, FALSE otherwise.

Notes:



--*/


{
    BOOL ok = FALSE;

    //
    // Note that Buffer is passed down to XapiGetNextSoundtrack for use as a temporary buffer.
    // XFindFirstSoundtrack and XFindNextSoundtrack must not assume anything about the contents
    // of this buffer following the call. Therefore, if they use the buffer (like XFindFirstSoundtrack
    // does) they are responsible for ensuring that the data in that buffer is not needed after the
    // XapiGetNextSoundtrack call. This was done to reduce the size of the stack allocations when using
    // the enum APIs.
    //


    if (State->Index < State->MaxIndex) {

        SetFilePointer (State->DbHandle, STDB_PAGE_SIZE * (State->Index + 1), NULL, FILE_BEGIN);
        State->Index++;

        ok = XapiReadFromStDb (State->DbHandle, Buffer, STDB_STSIG);
    }

    if (ok) {

        memcpy (&XapiStLookaside, Buffer, sizeof(STDB_STDESC));
        wcsncpy (Data->szName, XapiStLookaside.Name, MAX_SOUNDTRACK_NAME);
        Data->uSoundtrackId = XapiStLookaside.Id;
        Data->uSongCount = XapiStLookaside.SongCount;
        Data->uSoundtrackLength = XapiStLookaside.SoundtrackLength;

    }

    return ok;
}

XBOXAPI
BOOL
WINAPI
XFindNextSoundtrack (
    IN HANDLE FindHandle,
    OUT PXSOUNDTRACK_DATA SoundtrackData
    )

/*++

Routine Description:

    Continues an enumeration of soundtracks begun with XFindFirstSoundtrack.

Arguments:

    Handle - Contains a valid file handle returned from XFindFirstSoundtrack.
    SoundtrackData - Valid pointer to a soundtrack data structure. On
                     success, receives the data of the next soundtrack in the list.

Return Value:

    TRUE if there are additional soundtracks to enumerate, FALSE otherwise.

Notes:



--*/

{
    BYTE buf[STDB_PAGE_SIZE];

    RIP_ON_NOT_TRUE("XFindNextSoundtrack()", (NULL != FindHandle));
    RIP_ON_NOT_TRUE("XFindNextSoundtrack()", (INVALID_HANDLE_VALUE != FindHandle));
    RIP_ON_NOT_TRUE("XFindNextSoundtrack()", (NULL != SoundtrackData));

#if DBG
    if (FH_SIG_SOUNDTRACK != ((PSNDTRK_ENUMSTATE) FindHandle)->Signature)
    {
        RIP("XFindNextSoundtrack() - invalid parameter (FindHandle)");
    }
#endif // DBG

    return XapiGetNextSoundtrack (buf, (PSNDTRK_ENUMSTATE) FindHandle, SoundtrackData);

}

XBOXAPI
HANDLE
WINAPI
XFindFirstSoundtrack (
    OUT PXSOUNDTRACK_DATA SoundtrackData
    )

/*++

Routine Description:

    Begins an enumeration of the soundtracks on the media and returns the
    first soundtrack.

Arguments:

    SoundtrackData - Valid pointer to a soundtrack data structure. On
                     success, receives the data of the first soundtrack.

Return Value:

    valid HANDLE which may be used in future calls to XFindNextSoundtrack on success,
    otherwise, INVALID_HANDLE_VALUE.

Notes:



--*/

{


    HANDLE h;
    PSNDTRK_ENUMSTATE state = NULL;
    NTSTATUS status;
    BYTE buf[STDB_PAGE_SIZE];
    PSTDB_HDR hdr;



    RIP_ON_NOT_TRUE("XFindFirstSoundtrack()", (NULL != SoundtrackData));


    status = XapiOpenStDbAndReadHeader (&h, buf);

    //
    // Build the enum state data that will be passed back to the caller.
    //
    if (NT_SUCCESS (status)) {

        hdr = (PSTDB_HDR) buf;
        state = LocalAlloc (LMEM_FIXED, sizeof (SNDTRK_ENUMSTATE));

        if (!state) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            SetLastError (RtlNtStatusToDosError (status));
            NtClose (h);
        }
        else {

            state->Signature = FH_SIG_SOUNDTRACK;
            state->MaxIndex = hdr->StCount;
            state->Index = 0;
            state->DbHandle = h;
        }
    }
    else {

        SetLastError (RtlNtStatusToDosError (status));

    }

    //
    // Now, attempt to retrieve the first soundtrack from the database.
    //
    if (NT_SUCCESS (status)) {

            if (!XapiGetNextSoundtrack (buf, state, SoundtrackData)) {

                NtClose (h);
                LocalFree (state);
                state = NULL;

            }
    }


    return state ? (HANDLE) state : INVALID_HANDLE_VALUE;
}

XBOXAPI
BOOL
WINAPI
XGetSoundtrackSongInfo (
    IN DWORD SoundtrackId,
    IN UINT Index,
    OUT PDWORD SongId,
    OUT PDWORD SongLength,
    OUT OPTIONAL PWSTR NameBuffer,
    IN UINT BufferSize
    )

/*++

Routine Description:

    Retrieves information about a particular song index in a soundtrack.

Arguments:

    SoundtrackId - Contains the soundtrack identifier of interest.
    Index - Contains the index of the song within the soundtrack of interest.
    SongId - Receives the unique identifier of the song at the given index within the soundtrack.
    NameBuffer - Optionally receives the UNICODE name of the song.
    BufferSize - If NameBuffer is non null, specifies the size of NameBuffer in UNICODE characters.

Return Value:

    TRUE if the data was successfully returned, FALSE otherwise.

Notes:

    This API allows a title to randomly access songs from within a given soundtrack (for doing things
    such as shuffle, random play, etc.) However, linearly incrementing index is the most efficient
    way to enumerate songs because individual songs are batched in groups of 7 within the database. As long
    as the index is within the same group of seven as the last call to this function, no additional read
    is necessary.

--*/

{
    BOOL ok = TRUE;
    HANDLE h = INVALID_HANDLE_VALUE;
    PSTDB_HDR hdr;
    BYTE buf[STDB_PAGE_SIZE];
    UINT i;
    UINT val;
    NTSTATUS status;

    RIP_ON_NOT_TRUE("XGetSongInfo()", (INVALID_STID != SoundtrackId));
    RIP_ON_NOT_TRUE("XGetSongInfo()", (NULL != SongId));

#if DBG

    if (NameBuffer && BufferSize < MAX_SONG_NAME) {

        RIP ("XGetSongInfo() - 'NameBuffer' Buffer too small.");
    }

#endif

    //
    // Make sure the correct soundtrack is in the lookaside list.
    //
    if (XapiStLookaside.Id != SoundtrackId) {

        status = XapiOpenStDbAndReadHeader (&h, buf);
        ok = NT_SUCCESS(status);
        if (ok) {

            //
            // The header contains an array of all the possible soundtrack blocks
            // and the soundtrack ids in each of those blocks. Read through
            // the array to find the block to seek to.
            //

            hdr = (PSTDB_HDR) buf;
            val = min (hdr->StCount, MAX_SOUNDTRACKS);


            for (i = 0; i < val; i++) {
                if (hdr->StBlocks[i] == SoundtrackId) {
                    break;
                }
            }

            if (i == val)  {
                ok = FALSE;
                SetLastError (ERROR_INVALID_PARAMETER);
            }
        }
        else {

            SetLastError (RtlNtStatusToDosError (status));
        }


        if (ok) {
            ok = (SetFilePointer (h, i * STDB_PAGE_SIZE, NULL, FILE_CURRENT) != INVALID_SET_FILE_POINTER);
            if (!ok) {
                SetLastError (ERROR_INVALID_DATA);
            }

        }

        if (ok) {
            ok = XapiReadFromStDb (h, buf, STDB_STSIG);
        }

        if (ok) {
            memcpy (&XapiStLookaside, buf, sizeof (STDB_STDESC));
            ok = XapiStLookaside.Id == SoundtrackId && XapiStLookaside.SongCount > Index;
            if (!ok) {
                SetLastError (ERROR_INVALID_PARAMETER);
            }
        }
    }


    //
    // Make sure the correct list segment is in the lookaside list.
    //
    if (ok) {

        if (XapiListLookaside.StId != SoundtrackId ||
            Index < (XapiListLookaside.ListIndex * WMADESC_LIST_SIZE) ||
            Index >= (XapiListLookaside.ListIndex + 1) * WMADESC_LIST_SIZE
            ) {


            if (h == INVALID_HANDLE_VALUE) {
                status = XapiOpenStDbAndReadHeader (&h, buf);
                ok = NT_SUCCESS(status);
                if (!ok) {
                    SetLastError (RtlNtStatusToDosError (status));
                }
            }

            if (ok) {
                val = (XapiStLookaside.ListBlocks[Index / WMADESC_LIST_SIZE] + 1 + MAX_SOUNDTRACKS) * STDB_PAGE_SIZE;
                ok = (SetFilePointer (h, val, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER);
                if (!ok) {
                    SetLastError (ERROR_INVALID_DATA);
                }
            }


            if (ok) {
                ok = XapiReadFromStDb (h, buf, STDB_LISTSIG);
            }

            if (ok) {
                memcpy (&XapiListLookaside, buf, sizeof (STDB_LIST));
                ok = XapiListLookaside.StId == SoundtrackId;
                if (!ok) {
                    SetLastError (ERROR_INVALID_DATA);
                }
            }
        }
    }

    //
    // Get the song info for the selected song.
    //
    if (Index > XapiStLookaside.SongCount) {

        ok = FALSE;
        SetLastError (ERROR_INVALID_PARAMETER);
    }

    if (ok) {



        *SongId = XapiListLookaside.SongIds[Index % WMADESC_LIST_SIZE];
        *SongLength = XapiListLookaside.SongLengths[Index % WMADESC_LIST_SIZE];
        if (NameBuffer) {
            wcsncpy (NameBuffer, XapiListLookaside.SongNames[Index % WMADESC_LIST_SIZE], MAX_SONG_NAME);
        }

    }

    if (h != INVALID_HANDLE_VALUE) {
        CloseHandle (h);
    }

    return ok;
}
/*++

Copyright (c) Microsoft Corporation

Module Name:

    xsndtrk.c

Abstract:

    This module contains the Xbox Soundtrack APIs

Author:

    Marc R. Whitten (marcw)  13-Feb-2001

Revision History:

--*/


XBOXAPI
HANDLE
WINAPI
XOpenSoundtrackSong (
    IN DWORD SongId,
    IN BOOL Asynchronous
    )

/*++

Routine Description:

    Opens the WMA song matching the song ID in the Dash Music directory.

Arguments:

    SongId - Contains the songid to open.
    Asynchronous - TRUE if the flag should be opened for asynchronous reads, FALSE if
                   it should be opened for synchronous reading.

Return Value:

    A valid file handle to the WMA song represented by the songid if successful, or
    INVALID_HANDLE_VALUE otherwise.

Notes:



--*/
{
    CHAR path[MAX_PATH];
    HANDLE h;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING oPath;



    sprintf (path, "%s%04x\\%08x.WMA", STDB_MUSICDIR, HIWORD(SongId), SongId);


    RtlInitObjectString(&oPath, path);
    InitializeObjectAttributes (&obja, &oPath, OBJ_CASE_INSENSITIVE, NULL, NULL);


    //
    // Attempt to open the soundtrack.
    //
    status = NtCreateFile (
            &h,
            SYNCHRONIZE | GENERIC_READ | FILE_READ_ATTRIBUTES,
            &obja,
            &iosb,
            NULL,
            0,
            FILE_SHARE_READ,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE | (Asynchronous ? FILE_NO_INTERMEDIATE_BUFFERING : FILE_SYNCHRONOUS_IO_NONALERT)
            );



    if (!NT_SUCCESS(status)) {
        XapiSetLastNTError (status);
    }

    return NT_SUCCESS (status) ? h : INVALID_HANDLE_VALUE;

}



