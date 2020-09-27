//  DIRS.CPP
//
//  Created 24-Mar-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------

//  CDirectories::Init
//      Allocate enough entries for all the directories we'll see

BOOL
CDirectories::Init(
    DWORD dwc
    )
{
    if (m_lpdirs)
        free(m_lpdirs);
    m_lpdirs = (LPDIRINFO)malloc(dwc * sizeof (DIRINFO));

    m_dwcDirsMax = dwc;
    m_dwcDirsNow = 0;

    return (m_lpdirs != NULL);
}


//  CDirectories::Add
//      Adds a new directory to the list

BOOL
CDirectories::Add(
    LPSTR lpID
    )
{
    LPDIRINFO lpdir;
    LPSTR lp;

    // Since we don't want to have duplicate keynames in the INI
    // we allow a single digit after the dirname. Strip it off here.
    lp = lpID + strlen(lpID) - 1;
    if (*lp >= '0' && *lp <= '9')
        *lp = 0;

    // If we already have too many or if this ID is already in there, bail
    if (m_dwcDirsNow == m_dwcDirsMax || Find(lpID))
        return FALSE;
    lpdir = &m_lpdirs[m_dwcDirsNow];
    lstrcpyn(lpdir->szID, lpID, MAX_ID);
    lpdir->dwSize = 0;

    ++m_dwcDirsNow;

    return TRUE;
}

//  CDirectories::AddSize (string overload)
//      Computes the worst-case, uncompressed size for a file
//      and accumulates it into the proper directory

BOOL
CDirectories::AddSize(
    LPCSTR lpID,
    LPCSTR lpFile
    )
{
    LPDIRINFO lp;
    HANDLE hfile;
    DWORD dwc;

    if (!(lp = Find(lpID)))
    {
        SetLastError(0);
        return FALSE;
    }

    // Get the file size
    hfile = CreateFile(lpFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
        return FALSE;
    dwc = GetFileSize(hfile, NULL);
    CloseHandle(hfile);

    // Round the size up to a 32K (worst case) cluster size
    dwc |= 32768;
    dwc &= ~32767;

    lp->dwSize += dwc;
    return TRUE;
}


//  CDirectories::AddSize (DWORD overload)
//      Accumulates the size of a file into the directory in
//      cases where we don't want to check the size here

BOOL
CDirectories::AddSize(
    LPCSTR lpID,
    DWORD dwc
    )
{
    LPDIRINFO lp;

    if (!(lp = Find(lpID)))
    {
        SetLastError(0);
        return FALSE;
    }

    // Round the size up to a 32K (worst case) cluster size
    dwc |= 32768;
    dwc &= ~32767;

    lp->dwSize += dwc;
    return TRUE;
}


//  CDirectories::Find
//      Locates a directory entry previously added

LPDIRINFO
CDirectories::Find(
    LPCSTR lpID
    )
{
    LPDIRINFO lp;
    DWORD i;

    for (lp = m_lpdirs, i = 0 ; i < m_dwcDirsNow ; ++i, ++lp)
    {
        if (_stricmp(lp->szID, lpID) == 0)
            break;
    }

    if (i == m_dwcDirsNow)
        return NULL;
    else
        return lp;
}

