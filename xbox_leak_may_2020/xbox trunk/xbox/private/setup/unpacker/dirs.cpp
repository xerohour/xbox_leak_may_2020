//  DIRS.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "unpacker.h"

//---------------------------------------------------------------------

//  CDirectories::Init
//      Called once we know how many target directories are supported.
//      This allows a buffer for the directories to be allocated.

BOOL
CDirectories::Init(
    DWORD dwcTargets
    )
{
    // Allocate a buffer sufficient for all target directories
    m_Targets = (LPTARGETDIR)malloc(dwcTargets * sizeof (TARGETDIR));
    if (!m_Targets)
        return FALSE;
    ZeroMemory(m_Targets, dwcTargets * sizeof (TARGETDIR));

    m_dwcTargets = 0;
    return TRUE;
}


//  CDirectories::AllTargets
//      Returns a pointer to all the targets.
//      This allows the caller to enumerate all targets.
//      Returns the count of valid targets in *pdwc

LPTARGETDIR
CDirectories::AllTargets(
    LPDWORD pdwc
    )
{
    if (pdwc)
        *pdwc = m_dwcTargets;

    return m_Targets;
}


//  CDirectories::ParseTargetDirs
//      Walk through all the target directories and prepare them
//      to copy files to.

VOID
CDirectories::ParseTargetDirs()
{
    LPSECTIONENUM lpSection;
    DWORD i;
    LPKEYINFO lpkey;

    // Read the section in the settings file
    lpSection = m_psettings->EnumerateSection(CSTR_TARGETDIRECTORIES);
    if (lpSection == NULL)
        return; // the errors will be caught later

    // Allocate our directory buffer with the total number of directories
    if (!Init(lpSection->dwcKeys))
        return; // errors will be caught later

    // Loop through and add each directory as a target directory.
    // Note that this call might do UI or otherwise not return for
    // a long time!
    // Note that we ignore the return value. It is legal to have a
    // target fail. For example, a REGKEY might not be found.
    for (i = 0, lpkey = lpSection->keys ; i < lpSection->dwcKeys ; ++i, ++lpkey)
    {
        AddTarget(lpkey->lpKey, lpkey->lpValue);
    }

    // Free up the memory allocated by the enumeration
    m_psettings->EnumerateFree(lpSection);
}


//  CDirectories::AddTarget
//      Adds a new directory target. This might mean enabling UI to
//      prompt for a target directory or a registry search.
//      Note that it is NOT an error to have a failure here.
//      The order of targets is important: you might have a regkey and
//      if the regkey isn't found, a prompt target later.
//      Once there is a successful add, we won't add any further targets
//      with that ID.

BOOL
CDirectories::AddTarget(
    LPSTR lpID,
    LPSTR lpCommand
    )
{
    LPSTR lpData;
    LPSTR lp;
    LPSTR lpFlags;
    LPTARGETDIR lpTarget;

    // Zap any trailing digits. We do this so that we don't have duplicate INI keys.
    // We just handle one digit.
    lp = lpID + strlen(lpID) - 1;
    if (*lp >= '0' && *lp <= '9')
        *lp = 0;

    // Check and see if we already have a target with that name
    // If we do and it already has a registered path, then we don't
    // need to add the new target. Otherwise add a new target
    lpTarget = FindTarget(lpID);
    if (lpTarget == NULL)
    {
        // New target
        lpTarget = &m_Targets[m_dwcTargets];
        m_dwcTargets++;
        lpTarget->lpID = _strdup(lpID);
        lpTarget->dwDiskSpaceUsed = 0;
        lpTarget->dwDiskSpaceNeeded = 0;
        lpTarget->szPath[0] = 0;
        lpFlags = m_psettings->GetString(CSTR_DIRFLAGS, lpID);
        lpTarget->dwFlags = GetUnpacker()->AccumulateFlags(lpFlags);
        free(lpFlags);
    }
    else
    {
        // Existing target. Path already filled in successfully?
        if (lpTarget->szPath[0] != 0)
            return FALSE;
    }

    // Parse the command
    // All commands are in the form verb:data
    for (lpData = lpCommand ; *lpData != ':' ; lpData++)
    {
        // No data if there's no :
        if (*lpData == 0)
            break;
    }
    if (*lpData != 0)
    {
        // Zap the ':' so we have a split between command and data and save the pointer
        *lpData++ = 0;
    }

    // Get strings that are ok to keep around
    lpTarget->lpCommand = _strdup(lpCommand);
    lpTarget->lpData = _strdup(lpData);

    // Process the commands
    return ProcessCommands(lpTarget);
}


//  CDirectories::FindTarget
//      Returns a pointer to info about the target or NULL

LPTARGETDIR
CDirectories::FindTarget(
    LPCSTR lpID
    )
{
    DWORD i;

    for (i = 0 ; i < m_dwcTargets ; i++)
    {
        if (_stricmp(lpID, m_Targets[i].lpID) == 0)
            return &m_Targets[i];
    }

    return NULL;
}


//  CDirectories::ProcessCommands
//      Knows how to handle the various commands passed in with each
//      directory target.

BOOL
CDirectories::ProcessCommands(
    LPTARGETDIR lpTarget
    )
{
    // Target directory found from looking up a regkey
    if (_stricmp(lpTarget->lpCommand, "REGKEY") == 0)
    {
        return FindRegKey(lpTarget);
    }

    // This points the directory to the default path entered by the user.
    else if (_stricmp(lpTarget->lpCommand, "DEFAULT") == 0)
    {
        return TRUE;
    }

    // These commands point to predefined system directories
    else if (_stricmp(lpTarget->lpCommand, "SYSTEM") == 0)
    {
        GetSystemDirectory(lpTarget->szPath, MAX_PATH);
        return TRUE;
    }
    else if (_stricmp(lpTarget->lpCommand, "WINDOWS") == 0)
    {
        GetWindowsDirectory(lpTarget->szPath, MAX_PATH);
        return TRUE;
    }

    // This command just means that we need to enumerate later and
    // set it in code as opposed to finding it some other templatized way
    else if (_stricmp(lpTarget->lpCommand, "SET_BY_UNPACKER") == 0)
    {
        return TRUE;
    }

    // Not a known command
    else
    {
        DebugPrint("Failed (unimplemented): ID: %s Command: PROMPT\n", lpTarget->lpID);
        return FALSE;
    }

    // Add future commands here
}


//  CDirectories::FindRegKey
//      Processes the "REGKEY" command by looking up a regkey and filling
//      the directory from a specific value in the regkey.
//      Returns FALSE if the regkey doesn't exist. Doesn't create the path.

#define CMD_REG_ROOTKEY   0
#define CMD_REG_SUBKEY    1
#define CMD_REG_VALUE     2
#define CMD_REG_SUBDIR    3
#define MAX_REG_CMD 4

BOOL
CDirectories::FindRegKey(
    LPTARGETDIR lpTarget
    )
{
    HKEY hkey;
    HKEY hkeyRoot;
    LONG lRet;
    DWORD dwSize = MAX_PATH;

    // Parse the rest of the line.
    LPSTR lp[MAX_REG_CMD];
    DWORD dwc;
    ::ParseLine(lpTarget->lpData, lp, MAX_REG_CMD, &dwc);

    // Translate the key values
    if (_stricmp(lp[CMD_REG_ROOTKEY], "HKLM") == 0)
        hkeyRoot = HKEY_LOCAL_MACHINE;
    else if (_stricmp(lp[CMD_REG_ROOTKEY], "HKCU") == 0)
        hkeyRoot = HKEY_CURRENT_USER;
    else
        return FALSE;

    // Read the key
    if (RegOpenKeyEx(hkeyRoot, lp[CMD_REG_SUBKEY], 0, KEY_READ, &hkey) != ERROR_SUCCESS)
        return FALSE;
    lRet = RegQueryValueEx(hkey, lp[CMD_REG_VALUE], NULL, NULL, (LPBYTE)lpTarget->szPath, &dwSize);
    RegCloseKey(hkey);

    if (lRet == ERROR_SUCCESS)
    {
        // If there's an additional dir to add, do it
        if (lp[CMD_REG_SUBDIR] != NULL)
        {
            ::AppendSlash(lpTarget->szPath);
            strcat(lpTarget->szPath, lp[CMD_REG_SUBDIR]);
        }
    }

    return (lRet == ERROR_SUCCESS);
}
