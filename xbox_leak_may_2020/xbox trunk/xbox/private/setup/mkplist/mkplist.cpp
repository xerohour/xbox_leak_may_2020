//  MKPLIST.CPP
//
//      A test utility to make a packing list from a directory image.
//      To make this more useful, having an exclude list would be a nice feature.
//
//  Created 3-Apr-2001 [JonT]

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\common\global.h"
#include "..\common\settings.h"


    void PrintUsage(void);
    void WalkDirectory(LPSTR lpdir, LPSTR lpdirRelative);
    LPTSTR GetSetting(LPSTR lpSetting);
    BOOL ExcludeFilename(LPSTR lp);

    CSettingsFile g_settings;

    LPSTR g_lpFlavor;
    LPSTR g_lpSourceID;
    LPSTR g_lpDestID;
    LPSTR g_lpPath;
    LPSECTIONENUM g_lpExceptions;

int __cdecl
main(
    int argc,
    char** argv
    )
{
    TCHAR szPath[MAX_PATH];

    if (argc != 2)
    {
        PrintUsage();
        return -1;
    }

    // Point to our ini file. Fully qualify the path. the INI code does very
    // strange things with searches.
    _fullpath(szPath, argv[1], MAX_PATH);
    if (GetFileAttributes(szPath) == 0xffffffff ||
        !g_settings.SetFile(szPath))
    {
        printf("Couldn't find settings file\n");
        return -1;
    }

    g_lpFlavor = GetSetting("Flavor");
    g_lpSourceID = GetSetting("SourceRoot");
    g_lpDestID = GetSetting("DestinationRoot");
    g_lpPath = GetSetting("Path");

    // Get the exclude list
    g_lpExceptions = g_settings.EnumerateINISection("ExcludeList");

    // Walk through all the directories
    WalkDirectory(g_lpPath, "");

    return 0;
}


void
PrintUsage(void)
{
    fprintf(stderr,
        "mkplist: usage: mkplist [settings file]\n\n"
        "Builds a packing list for the setup packer\n\n"
        "Result is sent to stdout. This allows the results of several\n"
        "mkplist commands to be concatenated with >>");
}


void
WalkDirectory(
    LPSTR lpdir,
    LPSTR lpdirRelative
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;
    TCHAR szPath[MAX_PATH];     // This is going to chew up some serious stack space--it's recursive!
    TCHAR szPathRelative[MAX_PATH];

    // Create the find path with a \*.* for the search
    strcpy(szPath, lpdir);
    AppendSlash(szPath);
    strcat(szPath, "*.*");

    // Loop through all the files deleting them
    if ((hfind = FindFirstFile(szPath, &find)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            // Skip . and ..
            if (find.cFileName[0] == '.')
                continue;

            // We found a directory, descend into it
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                strcpy(szPath, lpdir);
                AppendSlash(szPath);
                strcat(szPath, find.cFileName);
                strcpy(szPathRelative, lpdirRelative);
                if (*lpdirRelative != 0)
                    AppendSlash(szPathRelative);
                strcat(szPathRelative, find.cFileName);
                if (!ExcludeFilename(find.cFileName))
                    WalkDirectory(szPath, szPathRelative);
            }

            // Otherwise, add the file to the list. We descend directories first
            else
            {
                if (!ExcludeFilename(find.cFileName))
                {
                    printf("%s,file,%s,%s,%s,%s,%s,,\n",
                           g_lpFlavor, g_lpSourceID, lpdirRelative, find.cFileName,
                           g_lpDestID, lpdirRelative);
                }
            }
        }
        while (FindNextFile(hfind, &find));
        FindClose(hfind);
    }
}


LPTSTR
GetSetting(
    LPSTR lpSetting
    )
{
    return g_settings.GetINIString("MakePackingList", lpSetting);
}



BOOL
ExcludeFilename(
    LPSTR lp
    )
{
    TCHAR szName[MAX_PATH];
    LPSTR lpT;
    LPSTR lpExt;
    DWORD i;

    // Make sure we have an exclude list
    if (g_lpExceptions == NULL)
        return FALSE;

    // Split filename into name and extension
    strcpy(szName, lp);
    for (lpT = szName ; *lpT != '.' ; ++lpT)
    {
        if (*lpT == 0)
            break;
    }
    if (*lpT != 0)
        *lpT++ = 0;
    lpExt = lpT;

    // Now that we have name and extension, walk through the exclude list
    // looking for matches
    for (i = 0 ; i < g_lpExceptions->dwcKeys ; ++i)
    {
        // Exclude by extension
        if (g_lpExceptions->keys[i].lpKey[0] == '.')
        {
            if (_stricmp(&g_lpExceptions->keys[i].lpKey[1], lpExt) == 0)
                break;
        }

        // Exclude by name (including extension if any)
        else
        {
            if (_stricmp(g_lpExceptions->keys[i].lpKey, lp) == 0)
                break;
        }
    }

    // Return TRUE only if we found the file
    return !(i == g_lpExceptions->dwcKeys);
}
