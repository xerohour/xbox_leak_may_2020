//  FILEINFO.CPP
//
//  Created 30-Mar-2001 [JonT]

#include "common.h"
#include "fileinfo.h"


//  CFileInfo::Get
//      Reads in the important information about a file so it
//      can be saved and reloaded.

BOOL
CFileInfo::Get(
    LPCSTR lpFile
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;

    // Find the file
    if ((hfind = FindFirstFile(lpFile, &find)) != INVALID_HANDLE_VALUE)
    {
        m_dwSize = find.nFileSizeLow;
        m_ft = find.ftLastWriteTime;
        FindClose(hfind);
        return TRUE;
    }
    else
        return FALSE;
}


//  CFileInfo::Compare
//      Compares a file in the filesystem to the info currently stored.
//      If the file doesn't exist, returns 1 (same as existing file is older)
//      If the file does exist and matches, returns 0.
//      If the file exists and is older, returns 1.
//      If the file exists and is newer, returns -1

int
CFileInfo::Compare(
    LPCSTR lpFile
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;

    // Find the file
    if ((hfind = FindFirstFile(lpFile, &find)) != INVALID_HANDLE_VALUE)
    {
        FindClose(hfind);

        // Strip off the low DWORD and the low bit of the high DWORD.
        // This gives us accuracy that roughly matches the accuracy of FAT
        __int64 i64New = ((LARGE_INTEGER*)&m_ft)->QuadPart;
        __int64 i64Existing = ((LARGE_INTEGER*)&find.ftLastWriteTime)->QuadPart;
        __int64 i64Delta = i64New - i64Existing;
        if (i64Delta < 0)
            i64Delta = -i64Delta;

        // FILETIMEs match (within the resolution of FAT: 2 seconds,
        // size matches, return file is the same.
        // If size doesn't match but time does, assume corruption and force copy.
        if (i64Delta <= 20000001)
            return 0;

        // If existing file is older, return 1
        if (i64Existing < i64New)
            return 1;

        // If existing file is newer, return -1
        return -1;
    }
    return 1;
}


//  CFileInfo::NoLessThan
//      Makes sure a date for the file is no less than a given date.

void
CFileInfo::NoLessThan(
    FILETIME ft
    )
{
    __int64 i64MinTime = ((LARGE_INTEGER*)&ft)->QuadPart;

    // If the date we have is less than the date passed in, set the date
    // to the one passed in
    __int64* pi64Current = &((LARGE_INTEGER*)&m_ft)->QuadPart;
    if (*pi64Current < i64MinTime)
        *pi64Current = i64MinTime;
}


//  CFileInfo::Load
//      Retrieves the text version of the file info and reloads
//      the values into the class.

BOOL
CFileInfo::Load(
    LPSTR lpIn
    )
{
    // Start address
    LPBYTE lp = (LPBYTE)(&m_dwSize);

    // Get address of the next byte after the FILETIME structure
    LPBYTE lpEnd = (LPBYTE)((&m_ft) + 1);

    for (; *lpIn != 0 && lp < lpEnd ; ++lp)
    {
        *lp = (*lpIn++ - 'A') << 4;
        *lp |= (*lpIn++ - 'A');
    }

    if (lp == lpEnd)
        return TRUE;
    else
        return FALSE;
}


//  CFileInfo::Save
//      Saves the info about a file into text format so
//      it can be reloaded later.
//      Assumes the buffer is at least FILEINFO_SAVE_SIZE large.

BOOL
CFileInfo::Save(
    LPSTR lpOut
    )
{
    // Start address
    LPBYTE lp = (LPBYTE)(&m_dwSize);

    // Get address of the next byte after the FILETIME structure
    LPBYTE lpEnd = (LPBYTE)((&m_ft) + 1);

    // Write out one character per nibble
    for (; lp < lpEnd ; ++lp)
    {
        *lpOut++ = (*lp >> 4) + 'A';
        *lpOut++ = (*lp & 0x0f) + 'A';
    }
    *lpOut = 0;

    return TRUE;
}

