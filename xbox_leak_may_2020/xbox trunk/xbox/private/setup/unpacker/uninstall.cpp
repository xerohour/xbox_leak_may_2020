//  UNINSTALL.CPP
//
//  Created 1-Apr-2001 [JonT]

#include "unpacker.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <search.h>

// Prototype for compare function used in qsort and bsearch
    int __cdecl CompareFunc(const void* e1, const void* e2);

//---------------------------------------------------------------------

BOOL
CUninstall::Init(
    DWORD dwcManifest,
    LPSTR lpUninstallDir
    )
{
    // One shot object: we can create just one uninstall object
    if (m_lpFile)
        return FALSE;

    m_dwcOldLines = ReadOldUninstallFile(lpUninstallDir);
    m_dwcMaxLines = dwcManifest + m_dwcOldLines + 10; // Leave room for some extra commands
    m_dwcTotalLines = 0;

    // Allocate the line table
    m_ptrtable = (LPSTR*)malloc(m_dwcMaxLines * sizeof (LPSTR*));
    ZeroMemory(m_ptrtable, m_dwcMaxLines * sizeof (LPSTR*));

    // If there were no old lines, we're done
    if (m_dwcOldLines == 0)
        return TRUE;

    // Walk through all the old lines pointing the line table at them
    // File line format is:
    // f,fully-qualified filename,ascii-encoded CFileInfo
    // Registry line format is:
    // r,root key,subkey,valuename
    // The part we search on is the filename for files and the root,subkey,vname for regkeys
    LPSTR lp = m_lpFile;
    LPSTR lpValues[3];
    do
    {
        // File lines
        if (*lp == 'f' || *lp == 'o')
        {
            ::ParseLine(lp, lpValues, 3, NULL);
            m_ptrtable[m_dwcTotalLines++] = lpValues[1];
        }
        
        // Registry lines or directory lines
        else if (*lp == 'r' || *lp == 'd')
        {
            // Zap the first comma and the end of the line
            ::ParseLine(lp, lpValues, 1, NULL);

            // Depend on the fact that the command is just one char long
            m_ptrtable[m_dwcTotalLines++] = lpValues[0] + 2;
        }
    }
    while (!::SkipRestOfLine(lp));

    // Sort the file
    m_dwcOldLines = m_dwcTotalLines;
    qsort(m_ptrtable, m_dwcOldLines, sizeof (LPSTR*), CompareFunc);

    return TRUE;
}


//  CUninstall::AddFile
//      Adds uninstall information for a file

VOID
CUninstall::AddFile(
    LPCSTR lpPath,
    LPCSTR lpFileInfo,
    BOOL fRegister
    )
{
    DWORD dwcLen;
    DWORD dwcLenPath = strlen(lpPath);
    LPSTR lp;
    LPSTR* lpTable;

    // Search for the string in the old lines first
    lpTable = (LPSTR*)bsearch((void*)&lpPath, m_ptrtable, m_dwcOldLines, sizeof  (m_ptrtable),
          (int (__cdecl*)(const void*, const void*))CompareFunc);

    // Get the length of the new entry:
    // one byte for command, zero, length of path, zero, length of fileinfo, zero
    dwcLen = strlen(lpFileInfo) + dwcLenPath + 4;

    // Allocate the string
    lp = (LPSTR)malloc(dwcLen);

    // Copy the information in
    if (fRegister)
        lp[0] = 'o';
    else
        lp[0] = 'f';
    lp[1] = 0;
    strcpy(&lp[2], lpPath);
    strcpy(&lp[2 + dwcLenPath + 1], lpFileInfo);

    // If we found an entry, use its place in the table rather than
    // creating a new one
    if (lpTable)
        *lpTable = &lp[2];
    else
        m_ptrtable[m_dwcTotalLines++] = &lp[2];
}


//  CUninstall::AddReg
//      Adds uninstall information for a regkey

VOID
CUninstall::AddReg(
    LPCSTR lpRoot,
    LPCSTR lpSubkey,
    LPCSTR lpValueName
    )
{
    DWORD dwcLenPath;
    LPSTR lp;
    LPSTR* lpTable;

    // Put the reg elements into a string so we can search and save them
    char sz[MAX_PATH];
    dwcLenPath = sprintf(sz, "%s,%s,%s", lpRoot, lpSubkey, lpValueName);
    lp = sz;

    // Search for the string in the old lines first
    lpTable = (LPSTR*)bsearch((void*)&lp, m_ptrtable, m_dwcOldLines, sizeof  (m_ptrtable),
          (int (__cdecl*)(const void*, const void*))CompareFunc);

    // Allocate the new string
    // one byte for command, zero, length of path, zero
    lp = (LPSTR)malloc(dwcLenPath + 3);

    // Copy the information in
    lp[0] = 'r';
    lp[1] = 0;
    strcpy(&lp[2], sz);

    // If we found an entry, use its place in the table rather than
    // creating a new one
    if (lpTable)
        *lpTable = &lp[2];
    else
        m_ptrtable[m_dwcTotalLines++] = &lp[2];
}


//  CUninstall::AddDir
//      Adds uninstall information for an entire directory

VOID
CUninstall::AddDir(
    LPCSTR lpPath
    )
{
    DWORD dwcLenPath = strlen(lpPath);
    LPSTR lp;
    LPSTR* lpTable;

    // Search for the string in the old lines first
    lpTable = (LPSTR*)bsearch((void*)&lpPath, m_ptrtable, m_dwcOldLines, sizeof  (m_ptrtable), CompareFunc);

    // Allocate the string, save room for the command
    lp = (LPSTR)malloc(dwcLenPath + 3);

    // Copy the information in
    lp[0] = 'd';
    lp[1] = 0;
    strcpy(&lp[2], lpPath);

    // If we found an entry, use its place in the table rather than
    // creating a new one
    if (lpTable)
        *lpTable = &lp[2];
    else
        m_ptrtable[m_dwcTotalLines++] = &lp[2];
}


//  CUninstall::Flush
//      Writes out all the uninstall information

VOID
CUninstall::Flush()
{
    FILE* handle;
    DWORD i;
    LPSTR lpCommand;
    LPSTR lpName;
    LPSTR lpFileInfo;
    HKEY hkey;
    TCHAR szName[MAX_PATH];
    DWORD dw;
    TCHAR s_szUninstallKey[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";

    // Write the regkey so the uninstaller gets launched
    // Create the key
    lpName = GetSettings()->GetSetting(CSTR_DEFAULTDIRTEXT);
    strcpy(szName, s_szUninstallKey);
    strcat(szName, lpName);
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szName, 0, NULL, 0, KEY_ALL_ACCESS,
                       NULL, &hkey, &dw) != ERROR_SUCCESS)
    {
        DebugPrint("Couldn't create reg key\n");
        return;
    }

    // Write an uninstall entry so it goes away when we're done
    AddReg("HKLM", szName, "");

    // Write the display name
    RegSetValueEx(hkey, "DisplayName", 0, REG_SZ, (BYTE*)lpName, strlen(lpName) + 1);
    free(lpName);

    // Move the uninstaller EXE to somewhere it can run while we can still
    // delete everything. Use the Windows directory (everyone else does...)
    TCHAR szDest[MAX_PATH];
    GetWindowsDirectory(szDest, MAX_PATH);
    ::AppendSlash(szDest);
    strcat(szDest, SCAB_UNINSTALLER);
    strcpy(szName, GetUnpacker()->GetUninstallDir());
    ::AppendSlash(szName);
    strcat(szName, SCAB_UNINSTALLER);
    if (!MoveFileEx(szName, szDest, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
    {
        DebugPrint("uninstaller move failing. Error=%d\n", GetLastError());
    }

    // Write the regkey pointing to the uninstaller and icon
    RegSetValueEx(hkey, "DisplayIcon", 0, REG_SZ, (BYTE*)szDest, strlen(szDest) + 1);
    strcat(szDest, " \"");
    strcat(szDest, GetUnpacker()->GetDefaultDir());
    strcat(szDest, "\" \"");
    strcat(szDest, CUnpacker::GetProductName());
    strcat(szDest, "\"");
    RegSetValueEx(hkey, "UninstallString", 0, REG_SZ, (BYTE*)szDest, strlen(szDest) + 1);

    // Create the new uninstall file
    handle = fopen(m_szUninstallFile, "w");
    if (handle == NULL)
    {
        DebugPrint("Couldn't create uninstall file %s\n", m_szUninstallFile);
        return;
    }

    // Loop through all items in the table
    for (i = 0 ; i < m_dwcTotalLines ; ++i)
    {
        lpName = m_ptrtable[i];
        lpCommand = lpName - 2;
        if (*lpCommand == 'f' || *lpCommand == 'o')
        {
            lpFileInfo = lpName + strlen(lpName) + 1;
            fprintf(handle, "%s,%s,%s\n", lpCommand, lpName, lpFileInfo);
        }
        else if (*lpCommand == 'r' || *lpCommand == 'd')
        {
            fprintf(handle, "%s,%s\n", lpCommand, lpName);
        }
    }

    // We're done with the file
    fclose(handle);
}


//---------------------------------------------------------------------
// Private routines

//  CUninstall::ReadOldUninstallFile
//      Reads the uninstall file left by the previous install if any.
//      Returns the number of lines in the uninstall file read.

DWORD
CUninstall::ReadOldUninstallFile(
    LPSTR lpUninstallDir
    )
{
    HANDLE hfile;
    DWORD dwLen;
    DWORD dwActuallyRead;

    // Get the uninstall filename
    strcpy(m_szUninstallFile, lpUninstallDir);
    ::AppendSlash(m_szUninstallFile);
    strcat(m_szUninstallFile, SCAB_UNINSTALLFILE);

    // Try to open the file
    hfile = CreateFile(m_szUninstallFile, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
        return 0;

    // Read the entire file
    dwLen = GetFileSize(hfile, NULL);
    if (dwLen == 0xffffffff)
        return 0;

    // Allocate a buffer big enough for the whole file
    m_lpFile = (LPSTR)malloc(dwLen + 1);
    if (!m_lpFile)
        return 0;

    // Read it completely and zero-terminate
    if (!ReadFile(hfile, m_lpFile, dwLen, &dwActuallyRead, NULL) ||
        dwActuallyRead != dwLen)
        return FALSE;
    *(m_lpFile + dwLen) = 0;

    // Close file, we're done with it
    CloseHandle(hfile);

    // Count the number of lines in it
    LPSTR lp = m_lpFile;
    DWORD dwcLines = 0;
    
    // Loop through all the lines in the file couting the lines
    do
    {
        ++dwcLines;
    }
    while (!::SkipRestOfLine(lp));

    return dwcLines;
}


//  CompareFunc
//      Compare function for qsort and bsearch.
//      Looks up the compare string given the string table pointer

int __cdecl
CompareFunc(
    const void* e1,
    const void* e2
    )
{
    return _stricmp(*(LPSTR*)e1, *(LPSTR*)e2);
}

