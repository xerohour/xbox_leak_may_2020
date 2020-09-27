/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dashst.c

Abstract:

    Dash soundtrack library

Notes:

    Provides support for creating and managing soundtracks.

--*/

#include "dshstpch.h"
#include "stdio.h"

#define STDB_MUSICDIR "T:\\MUSIC\\"
#define STDB_FILE "T:\\MUSIC\\ST.DB"

#define SEEKTOSOUNDTRACK(block) (SetFilePointer (g_DashStGlobals.DbHandle, ((block) + 1) * STDB_PAGE_SIZE, 0, FILE_BEGIN))
#define SEEKTOLISTSEGMENT(block) (SetFilePointer (g_DashStGlobals.DbHandle, (MAX_SOUNDTRACKS + 1 + (block)) * STDB_PAGE_SIZE, 0, FILE_BEGIN))
#define SEEKBYPAGE(page) (SetFilePointer (g_DashStGlobals.DbHandle, (page) * STDB_PAGE_SIZE, 0, FILE_CURRENT))


//
// These are cache variables used by the enumeration/info routines in XTL. Need to clear them on some operations.
//
extern STDB_LIST XapiListLookaside;
extern STDB_STDESC XapiStLookaside;


struct {

    HANDLE DbHandle;
    STDB_HDR Header;
    BYTE Buffer[STDB_PAGE_SIZE];

} g_DashStGlobals;

VOID
DashClearStCaches (
    VOID
    )
{

    XapiListLookaside.StId = INVALID_STID;
    XapiStLookaside.Id = INVALID_STID;
}


BOOL
DashReadFromStDb (
    DWORD Signature
    )
{

    DWORD byteCount;
    return ReadFile (g_DashStGlobals.DbHandle, g_DashStGlobals.Buffer, STDB_PAGE_SIZE, &byteCount, NULL)
        && byteCount == STDB_PAGE_SIZE
        && (!Signature || (*(PDWORD) g_DashStGlobals.Buffer) == Signature);
}


BOOL
DashBeginUsingSoundtracks (
    VOID
    )
{
    PBYTE buf;
    DWORD byteCount;

    memset (&g_DashStGlobals, 0, sizeof (g_DashStGlobals));

    //
    // Create the Soundtrack Music Directory if it does not exist
    //
    CreateDirectory( STDB_MUSICDIR, NULL );

    //
    // Open the soundtrack database and read in header.
    //
    g_DashStGlobals.DbHandle = CreateFile (
                                    STDB_FILE,
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL
                                    );

    if (g_DashStGlobals.DbHandle != INVALID_HANDLE_VALUE) {
        if (!DashReadFromStDb (0)) {

            CloseHandle (g_DashStGlobals.DbHandle);
            g_DashStGlobals.DbHandle = INVALID_HANDLE_VALUE;
        }
        else {
            memcpy (&g_DashStGlobals.Header, g_DashStGlobals.Buffer, sizeof (STDB_HDR));
        }
    }





    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        if (GetLastError () == ERROR_FILE_NOT_FOUND) {
            //
            // Need to create the database file.
            //
            g_DashStGlobals.DbHandle = CreateFile (
                                    STDB_FILE,
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL
                                    );

            if (g_DashStGlobals.DbHandle != INVALID_HANDLE_VALUE) {

                buf = (PBYTE) LocalAlloc (LPTR, STDB_PAGE_SIZE * (MAX_SOUNDTRACKS + 1) * sizeof (BYTE));


                ((PSTDB_HDR) buf)->Version = STDB_VERSION;
                if (WriteFile (g_DashStGlobals.DbHandle, buf, STDB_PAGE_SIZE * (MAX_SOUNDTRACKS + 1), &byteCount, NULL) &&
                    byteCount == STDB_PAGE_SIZE * (MAX_SOUNDTRACKS + 1)) {

                    CloseHandle (g_DashStGlobals.DbHandle);
                    LocalFree (buf);
                    return DashBeginUsingSoundtracks ();
                }
                else {
                    CloseHandle (g_DashStGlobals.DbHandle);
                    LocalFree (buf);
                    g_DashStGlobals.DbHandle = INVALID_HANDLE_VALUE;
                }
            }
        }
    }

    return g_DashStGlobals.DbHandle != INVALID_HANDLE_VALUE;
}

VOID
DashEndUsingSoundtracks (
    VOID
    )
{
    CloseHandle (g_DashStGlobals.DbHandle);
}

BOOL
DashWriteToStDb (
    VOID
    )

{
    DWORD byteCount;
    return WriteFile (g_DashStGlobals.DbHandle, g_DashStGlobals.Buffer, STDB_PAGE_SIZE, &byteCount, NULL) && byteCount == STDB_PAGE_SIZE;
}

BOOL
DashUpdateHeaderInStDb (
    VOID
    )
{
    memcpy (g_DashStGlobals.Buffer, &g_DashStGlobals.Header, sizeof (STDB_HDR));
    SetFilePointer (g_DashStGlobals.DbHandle, 0, 0, FILE_BEGIN);
    return DashWriteToStDb ();
}


BOOL
DashAddSoundtrack (
    PCWSTR SoundtrackName,
    PDWORD Stid
    )
{
    PSTDB_STDESC pst = (PSTDB_STDESC) g_DashStGlobals.Buffer;
    UINT block;
    BOOL b;
    CHAR newDir[MAX_PATH];


    //
    // Ensure that the global data is up to date.
    //
    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (g_DashStGlobals.Header.StCount == MAX_SOUNDTRACKS) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB full. Delete soundtracks first.");
        SetLastError (ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }

    //
    // Create the new soundtrack data for the database.
    //
    memset (pst, 0, sizeof (STDB_STDESC));
    pst->Id = g_DashStGlobals.Header.NextStId++;
    *Stid = pst->Id;
    pst->Signature = STDB_STSIG;

    lstrcpyn (pst->Name, SoundtrackName, MAX_SOUNDTRACK_NAME);

    block = g_DashStGlobals.Header.StCount++;


    g_DashStGlobals.Header.StBlocks[block] = pst->Id;
    SEEKTOSOUNDTRACK(block);

    //
    // Create the soundtrack directory. This is where the songs themselves will eventually live.
    //
    sprintf (newDir, "%s%04x", STDB_MUSICDIR, *Stid);
    b = CreateDirectory (newDir, NULL);

    //
    // If the directory already exists, we will trust that the dash knows what it is doing.
    //
    if (!b && GetLastError () == ERROR_ALREADY_EXISTS) {
        b = TRUE;
    }


    //
    // Update information in soundtrack database.
    //
    if (b) {
        b = DashWriteToStDb () && DashUpdateHeaderInStDb ();
    }

    return b;
}

DWORD
DashFindStBlock (
    DWORD Stid
    )
{
    int i;
    for (i = 0; i < MAX_SOUNDTRACKS; i++) {
        if (g_DashStGlobals.Header.StBlocks[i] == Stid) {
            break;
        }
    }

    return i;
}

BOOL
DashDeleteSoundtrack (
    DWORD Stid
    )
{

    UINT i;
    BOOL b = TRUE;
    UINT block;
    STDB_STDESC st;
    CHAR buf[MAX_PATH];
    HANDLE h;
    WIN32_FIND_DATA fd;


    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }


    block = DashFindStBlock (Stid);
    if (block == MAX_SOUNDTRACKS) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        SetLastError (ERROR_INVALID_INDEX);
        return FALSE;
    }




    SEEKTOSOUNDTRACK(block);
    b = DashReadFromStDb (STDB_STSIG);

    //
    // Mark all list segments of this soundtrack as !InUse.
    //
    if (b) {

        memcpy (&st, g_DashStGlobals.Buffer, sizeof (STDB_STDESC));
        for (i = 0; i * WMADESC_LIST_SIZE < st.SongCount; i++) {
            SEEKTOLISTSEGMENT (st.ListBlocks[i]);
            b = DashReadFromStDb (STDB_LISTSIG);
            if (b) {

                ((PSTDB_LIST) g_DashStGlobals.Buffer)->InUse = FALSE;
                SEEKBYPAGE(-1);
                b = DashWriteToStDb ();
            }
        }
    }

    //
    // Update position of all following soundtrakcs and remove this
    // soundtrack from the list.
    //
    if (b) {
        for (i = block + 1; b && i < g_DashStGlobals.Header.StCount; i++) {

            g_DashStGlobals.Header.StBlocks[i-1] = g_DashStGlobals.Header.StBlocks[i];

            SEEKTOSOUNDTRACK(i);
            b = DashReadFromStDb (STDB_STSIG);
            if (b) {
                SEEKBYPAGE(-2);
                b = DashWriteToStDb ();
            }
        }
    }


    if (b) {

        g_DashStGlobals.Header.StCount--;
        DashUpdateHeaderInStDb ();
    }

    if (b) {

        //
        // Delete the directory and all contents.
        // BUGBUG -- currently assumes there are no subdirectories. function needs to
        // cnange if this assumption is wrong.
        //
        sprintf (buf, "%s%04x\\*.*", STDB_MUSICDIR, Stid);
        h = FindFirstFile (buf, &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {

                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    sprintf (buf, "%s%04x\\%s", STDB_MUSICDIR, Stid, fd.cFileName);
                    if( !DeleteFile (buf) )
                    {
                        if( !SetFileAttributes( buf, FILE_ATTRIBUTE_NORMAL ) )
                        {
                            XDBGWRN ("Dash Soundtracks", "Failed to set file attributes!! - '%hs', Error - '0x%0.8X (%d)'", buf, GetLastError(), GetLastError() );
                        }

                        if( !( b = DeleteFile (buf) ) )
                        {
                            XDBGWRN ("Dash Soundtracks", "Failed to delete a second time!! - '%hs', Error - '0x%0.8X (%d)'", buf, GetLastError(), GetLastError() );
                        }
                    }
                }

            } while (FindNextFile (h, &fd));

            FindClose( h );
        }

        if( b )
        {
            sprintf (buf, "%s%04x", STDB_MUSICDIR, Stid);
            if( !( b = RemoveDirectory (buf) ) )
            {
                XDBGWRN ("Dash Soundtracks", "Failed to remove the directory!! - '%hs', Error - '0x%0.8X (%d)'", buf, GetLastError(), GetLastError() );
            }
        }
    }


    return b;

}

UINT
DashFindNewListBlock (
    VOID
    )
{
    UINT block = 0;
    BOOL found = FALSE;


    SEEKTOLISTSEGMENT(0);

    while (DashReadFromStDb (STDB_LISTSIG)) {
        if (!((PSTDB_LIST) g_DashStGlobals.Buffer)->InUse) {
            found = TRUE;
            break;
        }

        block++;
    }

    if (!found) {
        //
        // No empty list block found. Fix that problem.
        //
        SetFilePointer (g_DashStGlobals.DbHandle, 0, 0, FILE_END);
        ((PSTDB_LIST) g_DashStGlobals.Buffer)->InUse = FALSE;
        ((PSTDB_LIST) g_DashStGlobals.Buffer)->Signature = STDB_LISTSIG;
        DashWriteToStDb ();
        return DashFindNewListBlock ();
    }

    return block;

}

BOOL
DashDeleteSongFromSoundtrack (
    IN DWORD Stid,
    IN UINT Index
    )
{
    STDB_STDESC st;
    UINT stBlock;
    BOOL b = TRUE;
    UINT listIndex;
    UINT aryIndex;
    STDB_LIST curList;
    STDB_LIST nextList;
    UINT songId;
    CHAR path[MAX_PATH];

    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }

    //
    // Find the appropriate soundtrack.
    //
    stBlock = DashFindStBlock (Stid);
    if (stBlock == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock);
    if (!DashReadFromStDb (STDB_STSIG)) {
        SetLastError (ERROR_INVALID_PARAMETER);
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid);
        return FALSE;
    }

    memcpy (&st, g_DashStGlobals.Buffer, sizeof (STDB_STDESC));

    if (Index >= st.SongCount) {
        XDBGWRN ("Dash Soundtracks", "Song index %u outside range in soundtrack %u.", Index, Stid);
        SetLastError (ERROR_INVALID_INDEX);
        return FALSE;
    }




    listIndex = Index / WMADESC_LIST_SIZE;
    aryIndex = Index % WMADESC_LIST_SIZE;
    SEEKTOLISTSEGMENT (st.ListBlocks[listIndex]);
    b = DashReadFromStDb (STDB_LISTSIG);

    if (b) {





        memcpy (&curList, g_DashStGlobals.Buffer, sizeof (STDB_LIST));
        st.SoundtrackLength -= curList.SongLengths[aryIndex];
        aryIndex = Index % WMADESC_LIST_SIZE;
        songId = curList.SongIds[aryIndex];

        //
        // Shift first list segment data.
        //
        memcpy (curList.SongLengths + aryIndex, curList.SongLengths + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (DWORD));
        memcpy (curList.SongIds + aryIndex, curList.SongIds + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (DWORD));
        memcpy (curList.SongNames + aryIndex, curList.SongNames + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (WCHAR) * MAX_SONG_NAME);

        Index += WMADESC_LIST_SIZE - aryIndex;
        while (st.SongCount > Index) {
            SEEKTOLISTSEGMENT (st.ListBlocks[listIndex + 1]);
            b = DashReadFromStDb (STDB_LISTSIG);
            memcpy (&nextList, g_DashStGlobals.Buffer, sizeof (STDB_LIST));
            if (b) {


                curList.SongIds[WMADESC_LIST_SIZE - 1] = nextList.SongIds[0];
                lstrcpyn (curList.SongNames[WMADESC_LIST_SIZE - 1], nextList.SongNames[0], MAX_SONG_NAME);
                curList.SongLengths[WMADESC_LIST_SIZE - 1] = nextList.SongLengths[0];

                memcpy (g_DashStGlobals.Buffer, &curList, sizeof (STDB_LIST));
                SEEKTOLISTSEGMENT (st.ListBlocks[listIndex]);
                b = DashWriteToStDb ();

                memcpy (&curList, &nextList, sizeof (STDB_LIST));
                listIndex++;
                if (st.SongCount == Index + 1) {
                    curList.InUse = FALSE;
                }

                aryIndex = 0;
                memcpy (curList.SongLengths + aryIndex, curList.SongLengths + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (DWORD));
                memcpy (curList.SongIds + aryIndex, curList.SongIds + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (DWORD));
                memcpy (curList.SongNames + aryIndex, curList.SongNames + aryIndex + 1, (WMADESC_LIST_SIZE - aryIndex - 1) * sizeof (WCHAR) * MAX_SONG_NAME);

                Index += WMADESC_LIST_SIZE - aryIndex;
            }
        }

        memcpy (g_DashStGlobals.Buffer, &curList, sizeof (STDB_LIST));
        SEEKTOLISTSEGMENT (st.ListBlocks[listIndex]);
        b = DashWriteToStDb ();


    }


    if (b) {


        //
        // Update soundtrack description information.
        //
        st.SongCount--;
        memcpy (g_DashStGlobals.Buffer, &st, sizeof (STDB_STDESC));
        SEEKTOSOUNDTRACK (stBlock);
        if (!DashWriteToStDb ()) {
            XDBGWRN ("Dash Soundtracks", "Unable to update soundtrack.");
            return FALSE;
        }
    }

    if (b) {

        sprintf (path, "T:\\MUSIC\\%04x\\%08x.WMA", HIWORD(songId), songId);
        if (!DeleteFile(path)) {
            if( !SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL) ) {
                XDBGWRN ("Dash Soundtracks", "Failed to set file attributes!! - '%hs', Error - '0x%0.8X (%d)'", path, GetLastError(), GetLastError() );
            }

            if( !(b = DeleteFile(path)) )
            {
                XDBGWRN ("Dash Soundtracks", "Failed to delete a second time!! - '%hs', Error - '0x%0.8X (%d)'", path, GetLastError(), GetLastError() );
            }
        }
    }


    DashClearStCaches ();


    return b;
}


BOOL
DashAddSongToSoundtrack (
    IN DWORD Stid,
    IN DWORD Songid,
    IN DWORD Length,
    IN PCWSTR Name
    )
{

    UINT stBlock;
    UINT listBlock;
    STDB_STDESC st;
    UINT listIndex;
    UINT index;
    PSTDB_LIST plist;
    BOOL b = TRUE;



    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }


    //
    // Find the appropriate soundtrack.
    //
    stBlock = DashFindStBlock (Stid);
    if (stBlock == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid);
        return FALSE;
    }

    memcpy (&st, g_DashStGlobals.Buffer, sizeof (STDB_STDESC));

    //
    // Ensure that greater than max_songs is not created.
    //
    if (st.SongCount >= MAX_SONGS) {
        SetLastError (ERROR_NOT_ENOUGH_MEMORY);
        XDBGWRN ("Dash Soundtracks", "Too many songs in soundtrack %u.", Stid);
        return FALSE;
    }

    st.SongCount++;
    st.SoundtrackLength += Length;
    index = ((st.SongCount - 1) % WMADESC_LIST_SIZE);
    listIndex = (st.SongCount - 1) / WMADESC_LIST_SIZE;


    //
    // Find list segment that will hold this new song.
    //
    if (index == 0) {
        //
        // Need a new list segment.
        //
        listBlock = DashFindNewListBlock ();
        st.ListBlocks[listIndex] = listBlock;
    }
    else {

        listBlock = st.ListBlocks[listIndex];
    }

    //
    // Update the soundtrack data.
    //
    SEEKTOSOUNDTRACK(stBlock);
    memcpy (g_DashStGlobals.Buffer, &st, sizeof (STDB_STDESC));
    b = DashWriteToStDb ();

    //
    // Update the list segment data.
    //

    if (b) {

        SEEKTOLISTSEGMENT(listBlock);
        b = DashReadFromStDb (STDB_LISTSIG);
        plist = (PSTDB_LIST) g_DashStGlobals.Buffer;
    }

    if (b) {
        if (index == 0) {

            memset (plist, 0, sizeof (STDB_LIST));
            plist->InUse = TRUE;
            plist->Signature = STDB_LISTSIG;
            plist->StId = Stid;
            plist->ListIndex = listIndex;
        }

        plist->SongIds[index] = Songid;
        plist->SongLengths[index] = Length;
        lstrcpyn (plist->SongNames[index], Name, MAX_SONG_NAME);

        SEEKBYPAGE(-1);
        DashWriteToStDb ();
    }

    DashClearStCaches ();

    return b;
}

BOOL
DashSwapSoundtracks (
    UINT Stid1,
    UINT Stid2
    )
{

    UINT stBlock1, stBlock2;
    STDB_STDESC stTemp;



    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }


    //
    // Find the appropriate soundtracks.
    //
    stBlock1 = DashFindStBlock (Stid1);
    if (stBlock1 == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid1);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock1);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid1);
        return FALSE;
    }

    memcpy (&stTemp, g_DashStGlobals.Buffer, sizeof (STDB_STDESC));

    stBlock2 = DashFindStBlock (Stid2);
    if (stBlock2 == MAX_SOUNDTRACKS) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid2);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock2);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid2);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock1);
    if (!DashWriteToStDb ()) {
        XDBGWRN ("Dash Soundtracks", "Unable to write soundtrack %u back to db.", Stid2);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock2);
    memcpy (g_DashStGlobals.Buffer, &stTemp, sizeof (STDB_STDESC));
    if (!DashWriteToStDb ()) {
        XDBGWRN ("Dash Soundtracks", "Unable to write soundtrack %u back to db.", Stid1);
        return FALSE;
    }

    g_DashStGlobals.Header.StBlocks[stBlock1] = g_DashStGlobals.Header.StBlocks[stBlock2];
    g_DashStGlobals.Header.StBlocks[stBlock2] = stTemp.Id;
    DashUpdateHeaderInStDb ();

    return TRUE;

}


BOOL
DashSwapSongs (
    DWORD Stid,
    DWORD Index1,
    DWORD Index2
    )
{

    UINT stBlock;
    PSTDB_STDESC pst;
    PSTDB_LIST plist;
    STDB_LIST tempList;
    UINT songBlock1;
    UINT songBlock2;
    UINT tempId;
    UINT tempLength;
    WCHAR tempName[MAX_SONG_NAME + 1];
    BOOL b = TRUE;

    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }


    //
    // Find the appropriate soundtracks.
    //
    stBlock = DashFindStBlock (Stid);
    if (stBlock == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid);
        return FALSE;
    }

    pst = (PSTDB_STDESC) g_DashStGlobals.Buffer;

    if (Index1 >= pst->SongCount || Index2 >= pst->SongCount) {
        XDBGWRN ("Dash Soundtracks", "Indexes out of range.");
        return FALSE;
    }

    songBlock1 = pst->ListBlocks[Index1 / WMADESC_LIST_SIZE];
    songBlock2 = pst->ListBlocks[Index2 / WMADESC_LIST_SIZE];

    Index1 %= WMADESC_LIST_SIZE;
    Index2 %= WMADESC_LIST_SIZE;

    SEEKTOLISTSEGMENT (songBlock1);
    if (!DashReadFromStDb (STDB_LISTSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read list segment.");
        return FALSE;
    }



    if (songBlock1 == songBlock2) {
        //
        // Update within same block.
        //
        plist = (PSTDB_LIST) g_DashStGlobals.Buffer;

        tempId = plist->SongIds[Index1];
        tempLength = plist->SongLengths[Index1];
        lstrcpyn (tempName, plist->SongNames[Index1], MAX_SONG_NAME);

        plist->SongIds[Index1] = plist->SongIds[Index2];
        plist->SongLengths[Index1] = plist->SongLengths[Index2];
        lstrcpyn (plist->SongNames[Index1], plist->SongNames[Index2], MAX_SONG_NAME);

        plist->SongIds[Index2] = tempId;
        plist->SongLengths[Index2] = tempLength;
        lstrcpyn (plist->SongNames[Index2], tempName, MAX_SONG_NAME);

        SEEKBYPAGE(-1);
        b = DashWriteToStDb ();
    }
    else {

        memcpy (&tempList, g_DashStGlobals.Buffer, sizeof (STDB_LIST));
        SEEKTOLISTSEGMENT (songBlock2);
        if (!DashReadFromStDb (STDB_LISTSIG)) {
            XDBGWRN ("Dash Soundtracks", "Unable read list segment.");
            return FALSE;
        }

        plist = (PSTDB_LIST) g_DashStGlobals.Buffer;

        tempId = tempList.SongIds[Index1];
        tempLength = tempList.SongLengths[Index1];
        lstrcpyn (tempName, tempList.SongNames[Index1], MAX_SONG_NAME);

        tempList.SongIds[Index1] = plist->SongIds[Index2];
        lstrcpyn (tempList.SongNames[Index1], plist->SongNames[Index2], MAX_SONG_NAME);
        tempList.SongLengths[Index1] = plist->SongLengths[Index2];

        plist->SongIds[Index2] = tempId;
        plist->SongLengths[Index2] = tempLength;
        lstrcpyn (plist->SongNames[Index2], tempName, MAX_SONG_NAME);

        SEEKBYPAGE(-1);
        b = DashWriteToStDb ();

        if (b) {

            SEEKTOLISTSEGMENT (songBlock1);
            memcpy (g_DashStGlobals.Buffer, &tempList, sizeof (STDB_LIST));
            b = DashWriteToStDb ();
        }
    }

    DashClearStCaches ();

    return b;
}

DashEditSoundtrackName (
    UINT Stid,
    PCWSTR NewName
    )
{
    UINT stBlock;
    PSTDB_STDESC pst;

    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return FALSE;
    }


    //
    // Find the appropriate soundtrack.
    //
    stBlock = DashFindStBlock (Stid);
    if (stBlock == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid);
        return FALSE;
    }

    pst = (PSTDB_STDESC) g_DashStGlobals.Buffer;
    lstrcpyn (pst->Name, NewName, MAX_SOUNDTRACK_NAME);
    SEEKBYPAGE(-1);

    DashClearStCaches ();

    return DashWriteToStDb ();

}

DashEditSongName (
    UINT Stid,
    UINT Index,
    PCWSTR NewName
    )
{
    UINT stBlock;
    PSTDB_STDESC pst;
    UINT songBlock;

    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        SetLastError (ERROR_INVALID_HANDLE);
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        return FALSE;
    }


    //
    // Find the appropriate soundtrack.
    //
    stBlock = DashFindStBlock (Stid);
    if (stBlock == MAX_SOUNDTRACKS) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Soundtrack %u not found in database.", Stid);
        return FALSE;
    }

    SEEKTOSOUNDTRACK(stBlock);
    if (!DashReadFromStDb (STDB_STSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in soundtrack %u to be modified.", Stid);
        return FALSE;
    }

    pst = (PSTDB_STDESC) g_DashStGlobals.Buffer;

    if (Index >= pst->SongCount) {
        SetLastError (ERROR_INVALID_INDEX);
        XDBGWRN ("Dash Soundtracks", "Index out of range for soundtrack %u.", Stid);
        return FALSE;
    }

    SEEKTOLISTSEGMENT (pst->ListBlocks[Index / WMADESC_LIST_SIZE]);
    if (!DashReadFromStDb (STDB_LISTSIG)) {
        XDBGWRN ("Dash Soundtracks", "Unable to read in list segment.");
        return FALSE;
    }

    lstrcpyn (((PSTDB_LIST) g_DashStGlobals.Buffer)->SongNames[Index % WMADESC_LIST_SIZE], NewName, MAX_SONG_NAME);
    SEEKBYPAGE(-1);

    DashClearStCaches ();
    return DashWriteToStDb ();


}

DWORD
DashGetSoundtrackCount (
    VOID
    )
{
    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return 0;
    }

    return g_DashStGlobals.Header.StCount;
}




DWORD
DashGetNewSongId (
    DWORD Stid
    )
{
    DWORD rid;


    if (g_DashStGlobals.DbHandle == INVALID_HANDLE_VALUE) {
        XDBGWRN ("Dash Soundtracks", "Soundtrack DB not open. Use DashBeginUsingSoundtracks ()");
        SetLastError (ERROR_INVALID_HANDLE);
        return 0xFFFFFFFF;
    }

    //
    // HIWORD(songId) == STID. LOWORD(songId) == songid. Song should be saved using the following template:
    // sprintf (path, "T:\\MUSIC\\%04x\\%08x.WMA", HIWORD(songId), SongId);
    //
    rid = (Stid << 16) | (g_DashStGlobals.Header.NextSongId & 0xFFFF);
    g_DashStGlobals.Header.NextSongId++;

    DashUpdateHeaderInStDb ();

    return rid;
}







