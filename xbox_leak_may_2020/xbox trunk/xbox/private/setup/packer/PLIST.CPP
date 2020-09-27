//  PLIST.CPP
//
//  Created 12-Mar-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------
//  CPackingList class


//  CPackingList::SetFile
//      Opens the packing list file, returns error if it cannot be located

BOOL
CPackingList::SetFile(
    CSettingsFile* psettings,
    LPCTSTR lpPackingList,
    BOOL fTestOnly
    )
{
    DWORD dwLen;
    DWORD dwActuallyRead;
    HANDLE hfile;

    // Save the test-only flag
    m_fTestOnly = fTestOnly;

    // Save the settings object
    m_psettings = psettings;

    // Keep the name of the packing list around
    lstrcpyn(m_szPackingList, lpPackingList, MAX_PATH);

    // Free any memory before allocating a new buffer
    if (m_lpEntireFile)
        free(m_lpEntireFile);

    // Try to open the file
    hfile = CreateFile(lpPackingList, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        m_error = GetLastError();
        return FALSE;
    }

    // Read the entire file
    dwLen = GetFileSize(hfile, NULL);
    if (dwLen == 0xffffffff)
    {
        m_error = GetLastError();
        return FALSE;
    }

    // Allocate a buffer big enough for the whole file
    m_lpEntireFile = (LPSTR)malloc(dwLen + 1);
    if (!m_lpEntireFile)
    {
        m_error = ERROR_OUTOFMEMORY;
        return FALSE;
    }

    // Read it completely and zero-terminate
    if (!ReadFile(hfile, m_lpEntireFile, dwLen, &dwActuallyRead, NULL) ||
        dwActuallyRead != dwLen)
    {
        m_error = GetLastError();
        return FALSE;
    }
    *(m_lpEntireFile + dwLen) = 0;

    // Close file, we're done with it
    CloseHandle(hfile);

    // Point to the first content
    m_lpLine = m_lpEntireFile;

    // Get the time to update all the files to
    LPSTR lpDateUpdate = m_psettings->GetSetting(CSTR_UPDATEDATESTO);
    if (lpDateUpdate && *lpDateUpdate)
    {
        // Parse the month, day, year from the settings string
        LPSTR lpMonth = lpDateUpdate;
        LPSTR lpDay;
        LPSTR lpYear;
        for (lpDay = lpMonth ; *lpDay ; ++lpDay)
        {
            if (*lpDay == '/' || *lpDay == '-')
            {
                *lpDay++ = 0;
                break;
            }
        }
        for (lpYear = lpDay ; *lpYear ; ++lpYear)
        {
            if (*lpYear == '/' || *lpYear == '-')
            {
                *lpYear++ = 0;
                break;
            }
        }

        // Get the parsed string date components into a SYSTEMTIME
        SYSTEMTIME st = { 0 };
        st.wYear = (WORD)atoi(lpYear);
        if (st.wYear < 80)
            st.wYear += 2000;
        else if (st.wYear < 100)
            st.wYear += 1900;
        st.wMonth = (WORD)atoi(lpMonth);
        st.wDay = (WORD)atoi(lpDay);

        // Save the time as a FILETIME for later use, plus a flag if we have a valid date
        FILETIME ftLocal;
        m_fDateUpdate = SystemTimeToFileTime(&st, &ftLocal);
        LocalFileTimeToFileTime(&ftLocal, &m_ftUpdateTo);
    }

    return TRUE;
}


//  CPackingList::SetBuildFlavor
//      Sets the 'build flavor.' This allows a single packing list
//      to serve for multiple installs. Only files matching one or
//      more of the build flavor flags will be emitted into the manifest
//      and cabfile.

void
CPackingList::SetBuildFlavor(
    LPCSTR lp
    )
{
    m_dwBuildFlavor = AccumulateFlavor(lp);
}


//  CPackingList::ProcessLines
//      Drives the loop through all lines in the manifest

BOOL
CPackingList::ProcessLines(
    CManifest* pmanifest,
    CCabFile* pcabfile,
    CDirectories* pdirs
    )
{
    DWORD dwcLines = 0;
    DWORD dwcCurrent = 0;

    // Keep these objects around so the command handlers can see them
    m_pmanifest = pmanifest;
    m_pcabfile = pcabfile;
    m_pdirs = pdirs;

    // Count lines for progress indicator
    m_lpLine = m_lpEntireFile;
    do
    {
        ++dwcLines;
    }
    while (!::SkipRestOfLine(m_lpLine));

    // Loop through all the files in our packing list
    m_lpLine = m_lpEntireFile;
    do
    {
        // Skip over any leading whitespace
        ::SkipWhitespace(m_lpLine);

        // See if we need to process this line. If the build flavor
        // flags don't match, we skip...
        if (!(GetBuildFlavor() & m_dwBuildFlavor))
            continue;

        // Get the Lang IDs for the current line
        LPSTR lpLangIDs = m_lpLine;
        ::ZapComma(m_lpLine);
        m_LangIDs.ParseLangIDs( lpLangIDs );

        // Get the command type for this line
        m_lpCommand = GetCommand();

        // Dispatch the various types of commands
        if (!DispatchCommand())
        {
            ReportError("packing list command '%s' not supported", m_lpCommand);
        }

        // Display progress
        fprintf(stderr, "progress: %d of %d lines     \r", ++dwcCurrent, dwcLines);
    }
    while (!::SkipRestOfLine(m_lpLine));

    // If there are errors, bail
    if (m_dwcErrors > 0)
    {
        printf("%d error%s, stopping.                                              \n",
            m_dwcErrors, m_dwcErrors > 1 ? "s" : "");
        return FALSE;
    }

    fprintf(stderr, "Complete: %d of %d lines             \n", ++dwcCurrent, dwcLines);

    // Write out the manifest and the cab
    pmanifest->Close();

    fprintf(stderr, "Writing out cab file...\n");

    pcabfile->Flush();
    return TRUE;
}


//  CPackingList::~CPackingList

CPackingList::~CPackingList()
{
    // Free the buffer
    if (m_lpEntireFile)
    {
        free(m_lpEntireFile);
        m_lpEntireFile = NULL;
        m_lpLine = NULL;
    }
}

//---------------------------------------------------------------------
//  Private methods

//  _ReportError
//      Since processing a packing list is akin to compiling a file,
//      make a compiler-style error message when we find an error

void
CPackingList::_ReportError(
    LPCTSTR lpszFormat,
    va_list vararg
    )
{
    DWORD dwLine = 1;
    LPCSTR lp;

    // Increment the number of errors
    ++m_dwcErrors;

    // Count the number of LF characters up to the error line
    for (lp = m_lpEntireFile ; lp != m_lpCommand ; ++lp)
    {
        if (*lp == '\n')
            ++dwLine;
    }

    // Display the message
    ::_ReportError(m_szPackingList, dwLine, lpszFormat, vararg);
}

//  GetCommand
//      Returns a pointer to the command portion of the packing list line
//      and moves the pointer past it

LPCSTR
CPackingList::GetCommand()
{
    LPSTR lpCommand = m_lpLine;

    // Aap the comma and move the pointer past it
    ::ZapComma(m_lpLine);

    return lpCommand;
}


//  DispatchCommand
//      Calls the appropriate command handler for the given command

BOOL
CPackingList::DispatchCommand()
{
    int i;

    // Loop through all commands until we find a match
    for (i = 0 ; m_CommandList[i].lpText != NULL ; i++)
    {
        if (_stricmp(m_lpCommand, m_CommandList[i].lpText) == 0)
        {
            (this->*m_CommandList[i].lpHandler)();
            return TRUE;
        }
    }

    return FALSE;
}


//  GetBuildFlavor
//      Returns the DWORD build flavor flags retrieved on the packing list line
//      and moves the pointer past it

DWORD
CPackingList::GetBuildFlavor()
{
    LPSTR lpFlavor = m_lpLine;

    // Aap the comma and move the pointer past it
    ::ZapComma(m_lpLine);

    return AccumulateFlavor(lpFlavor);
}


//  CPackingList::AccumulateFlavor
//      ORs bits into the build flavor. Note that each letter has a unique
//      value by doing 1 << (letter value 1-26). This way, we can have a single
//      packing list line be marked for one or more build flavors.

DWORD
CPackingList::AccumulateFlavor(
    LPCSTR lp
    )
{
    UCHAR ch;
    DWORD dwAccum;

    // Add flag values for each letter in the list
    for (dwAccum = 0 ; *lp != 0 ; ++lp)
    {
        ch = (*lp | 32) - 'a';
        if (ch < 26)
            dwAccum |= 1 << ch;
    }

    return dwAccum;
}

//---------------------------------------------------------------------
//  Packing List commands
//      To add a new packing list command, you must add it to this table, add a new
//      function here with the correct signature, and add this member function
//      to CPackingList in packer.h.

    const COMMAND CPackingList::m_CommandList[] =
    {
        { "file", ProcessFile },
        { "source", ProcessCopySource },
        { "remove", ProcessRemove },
        { "removedir", ProcessRemove },
        { "addreg", ProcessRegCommands },
        { "delreg", ProcessRegCommands },
        { "copy", ProcessCopy },
        { "shortcut", ProcessShortcut },
        { NULL, NULL }
    };


//  ProcessFile
//      Handles a file entry from the packing list
//      The format of a file line is:
//      flavor, command, source root, source path, source file, target root, target path,
//          target file, flags

#define LINE_FILE_SRCROOT 0
#define LINE_FILE_SRCPATH 1
#define LINE_FILE_SRCFILE 2
#define LINE_FILE_TRGROOT 3
#define LINE_FILE_TRGPATH 4
#define LINE_FILE_TRGFILE 5
#define LINE_FILE_FLAGS   6
#define MAX_FILE_LINE 7
#define MIN_FILE_LINE 4

BOOL
CPackingList::ProcessFile()
{
    TCHAR szSource[MAX_PATH];
    TCHAR szDest[MAX_PATH];
    DWORD dwc;
    LPCSTR lpRoot;
    char bFileInfo[FILEINFO_SAVE_SIZE];

    // Parse the rest of the line.
    LPSTR lp[MAX_FILE_LINE];
    ::ParseLine(m_lpLine, lp, MAX_FILE_LINE, &dwc);
    if (dwc < MIN_FILE_LINE || dwc > MAX_FILE_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_FILE_LINE + 1, dwc + 1);
        return FALSE;
    }

    // Source name
    lpRoot = m_psettings->GetString(CSTR_PACKERPATHS, lp[LINE_FILE_SRCROOT]);
    szSource[0] = 0;
    if (*lpRoot != 0)
    {
        strcpy(szSource, lpRoot);
        ::AppendSlash(szSource);
    }
    strcat(szSource, lp[LINE_FILE_SRCPATH]);
    ::AppendSlash(szSource);
    strcat(szSource, lp[LINE_FILE_SRCFILE]);

    // Target name. Prepend the root name to make sure it's unique
    // If the target name is blank, use the source name
    strcpy(szDest, lp[LINE_FILE_TRGROOT]);
    ::AppendSlash(szDest);
    strcat(szDest, lp[LINE_FILE_TRGPATH]);
    ::AppendSlash(szDest);
    if (*lp[LINE_FILE_TRGFILE] != 0)
        strcat(szDest, lp[LINE_FILE_TRGFILE]);
    else
        strcat(szDest, lp[LINE_FILE_SRCFILE]);

    // Get the information about the source file. Note that since this is the
    // first time we have touched the file, we report the errors when we can't find
    // the file here.
    CFileInfo fi;
    if (!fi.Get(szSource))
    {
        DWORD dw = GetLastError();
        if (dw == ERROR_FILE_NOT_FOUND || dw == ERROR_PATH_NOT_FOUND)
            ReportError("file '%s' not found", szSource);
        else
            ReportError("couldn't open %s (error is %08x)", szSource, GetLastError());
        return FALSE;
    }

    // If we need to update the date, do so
    if (m_fDateUpdate)
        fi.NoLessThan(m_ftUpdateTo);

    // Account for this file's size in the directory size
    fi.Save(bFileInfo);
    m_pdirs->AddSize(lp[LINE_FILE_TRGROOT], fi.GetSize());

    // See if we've detected a duplicate file
    LPSTR lpCopyTarget = NULL;
    lpCopyTarget = m_filehash.FindDuplicate(szSource, lp[LINE_FILE_TRGROOT], szDest);

    // Did we find a duplicate?
    if (lpCopyTarget != NULL)
    {
        // See if the file is COMPLETELY duplicate: same source, dest name
        if (_stricmp(lpCopyTarget, szDest) == 0)
        {
            ReportError("Duplicate file (%s) encountered!", szDest);
            return FALSE;
        }

        // This file is a copy so just write a copy line to the manifest
        m_pmanifest->WriteCommand(&m_LangIDs, "copy", lp[LINE_FILE_TRGROOT], lpCopyTarget,
                             szDest, lp[LINE_FILE_FLAGS], bFileInfo, NULL);
    }

    // Otherwise, process the normal file command
    else
    {
        // Write a line to the manifest
        m_pmanifest->WriteCommand(&m_LangIDs, m_lpCommand, lp[LINE_FILE_TRGROOT], szDest, lp[LINE_FILE_FLAGS], bFileInfo, NULL);
    
        // Add to cab
        if (!m_pcabfile->AddFile(szSource, szDest))
        {
            ReportError("couldn't add %s to cab (error is %08x)", szSource, m_pcabfile->GetError()->erfOper);
            return FALSE;
        }
    }

    return TRUE;
}

//  ProcesssCopySource
//      Handles a 'source' entry from the packing list.
//      A source entry is the source of a copy command. This line doesn't
//      emit anything to the manifest but allows future copy entries to
//      reference this one.
//      The format of a file line is:
//      flavor, command, source id, source root, source path, source file, target root

#define LINE_SOURCE_SRCID   0
#define LINE_SOURCE_SRCROOT 1
#define LINE_SOURCE_SRCPATH 2
#define LINE_SOURCE_SRCFILE 3
#define LINE_SOURCE_TRGROOT 4
#define MAX_SOURCE_LINE 5
#define MIN_SOURCE_LINE 5

BOOL
CPackingList::ProcessCopySource()
{
    TCHAR szSource[MAX_PATH];
    DWORD dwc;
    LPCSTR lpRoot;
    LPSOURCENODE pnode;

    // Parse the rest of the line.
    LPSTR lp[MAX_SOURCE_LINE];
    ::ParseLine(m_lpLine, lp, MAX_SOURCE_LINE, &dwc);
    if (dwc < MIN_SOURCE_LINE || dwc > MAX_SOURCE_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_SOURCE_LINE + 1, dwc + 1);
        return FALSE;
    }

    // Source name
    lpRoot = m_psettings->GetString(CSTR_PACKERPATHS, lp[LINE_SOURCE_SRCROOT]);
    szSource[0] = 0;
    if (*lpRoot != 0)
    {
        strcpy(szSource, lpRoot);
        ::AppendSlash(szSource);
    }
    strcat(szSource, lp[LINE_SOURCE_SRCPATH]);
    ::AppendSlash(szSource);
    strcat(szSource, lp[LINE_SOURCE_SRCFILE]);

    // Create a new source node entry
    pnode = (LPSOURCENODE)malloc(sizeof (SOURCENODE));
    if (pnode == NULL)
    {
        ReportError("out of memory");
        return FALSE;
    }
    pnode->pNext = m_psources;
    m_psources = pnode;
    pnode->lpID = _strdup(lp[LINE_SOURCE_SRCID]);
    pnode->lpRoot = _strdup(lp[LINE_SOURCE_TRGROOT]);
    pnode->lpSource = _strdup(szSource);
    pnode->lpName = _strdup(lp[LINE_SOURCE_SRCFILE]);

    // Find the file and get its time/date and size. Note that this is the first time
    // we have tried to locate the file, so report error here.
    CFileInfo fi;
    if (!fi.Get(szSource))
    {
        DWORD dw = GetLastError();
        if (dw == ERROR_FILE_NOT_FOUND || dw == ERROR_PATH_NOT_FOUND)
            ReportError("file '%s' not found", szSource);
        else
            ReportError("couldn't open %s (error is %08x)", szSource, GetLastError());
        return FALSE;
    }

    // If we need to update the date, do so
    if (m_fDateUpdate)
        fi.NoLessThan(m_ftUpdateTo);

    // Save the information in the structure to pull out later...
    fi.Save(pnode->bFileInfo);
    pnode->nSize = -(int)fi.GetSize();

    return TRUE;
}



//  ProcessCopy
//      Handles a file entry from the packing list
//      The format of a file line is:
//      flavor, command, id, target path, target file, flags

#define LINE_COPY_ID      0
#define LINE_COPY_TRGPATH 1
#define LINE_COPY_TRGFILE 2
#define LINE_COPY_FLAGS   3
#define MAX_COPY_LINE 4
#define MIN_COPY_LINE 2

BOOL
CPackingList::ProcessCopy()
{
    TCHAR szDest[MAX_PATH];
    DWORD dwc;
    LPSOURCENODE pnode;

    // Parse the rest of the line.
    LPSTR lp[MAX_COPY_LINE];
    ::ParseLine(m_lpLine, lp, MAX_COPY_LINE, &dwc);
    if (dwc < MIN_COPY_LINE || dwc > MAX_COPY_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_COPY_LINE + 1, dwc + 1);
        return FALSE;
    }

    // Find the SOURCENODE entry
    for (pnode = m_psources ; pnode != NULL ; pnode = pnode->pNext)
    {
        if (_stricmp(pnode->lpID, lp[LINE_COPY_ID]) == 0)
            break;
    }
    if (pnode == NULL)
    {
        // Report number of entries including command
        ReportError("source ID '%s' not found", lp[LINE_COPY_ID]);
        return FALSE;
    }

    // Dest name
    strcpy(szDest, pnode->lpRoot);
    ::AppendSlash(szDest);
    strcat(szDest, lp[LINE_COPY_TRGPATH]);
    ::AppendSlash(szDest);
    if (*lp[LINE_COPY_TRGFILE] == 0)
        strcat(szDest, pnode->lpName);
    else
        strcat(szDest, lp[LINE_COPY_TRGFILE]);

    // The first time we encounter a copy entry, we need to actually
    // create the file on the target, so we emit a file to the manifest.
    // Every time we encounter it after this, we emit a copy to copy that file
    // to the target location. Saves a lot of space in the cabfile...
    if (pnode->nSize < 0)
    {
        // Write the manifest line
        m_pmanifest->WriteCommand(&m_LangIDs, "file", pnode->lpRoot, szDest, lp[LINE_COPY_FLAGS], pnode->bFileInfo, NULL);

        // Add to cab
        if (!m_pcabfile->AddFile(pnode->lpSource, szDest))
        {
            ReportError("couldn't add %s to cab (error is %08x)",
                        pnode->lpSource, m_pcabfile->GetError()->erfOper);
            return FALSE;
        }

        // We're done with the source location now, so save the dest location.
        // This dest location next time will be the source of the copy
        free(pnode->lpSource);
        pnode->lpSource = _strdup(szDest);
        pnode->nSize = -pnode->nSize;
    }
    else
    {
        // Write the copy line to the manifest
        m_pmanifest->WriteCommand(&m_LangIDs, m_lpCommand, pnode->lpRoot, pnode->lpSource,
                             szDest, lp[LINE_COPY_FLAGS], pnode->bFileInfo, NULL);
    }

    // Add the size for this entry
    m_pdirs->AddSize(pnode->lpRoot, pnode->nSize);

    return TRUE;
}


//  ProcessRemove
//      Handles a 'remove' entry from the packing list
//      The format of a file line is:
//      flavor, command, target root, target path, target file, flags

#define LINE_REMOVE_TRGROOT 0
#define LINE_REMOVE_TRGPATH 1
#define LINE_REMOVE_TRGFILE 2
#define LINE_REMOVE_FLAGS   3
#define MAX_REMOVE_LINE 4
#define MIN_REMOVE_LINE 3

BOOL
CPackingList::ProcessRemove()
{
    TCHAR szFile[MAX_PATH];
    DWORD dwc;

    // Parse the rest of the line.
    LPSTR lp[MAX_REMOVE_LINE];
    ::ParseLine(m_lpLine, lp, MAX_REMOVE_LINE, &dwc);
    if (dwc < MIN_REMOVE_LINE || dwc > MAX_REMOVE_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_REMOVE_LINE + 1, dwc + 1);
        return FALSE;
    }

    strcpy(szFile, lp[LINE_REMOVE_TRGPATH]);
    if (szFile[0] != 0 && lp[LINE_REMOVE_TRGFILE][0] != 0)
        ::AppendSlash(szFile);
    strcat(szFile, lp[LINE_REMOVE_TRGFILE]);

    // Write a line to the manifest
    m_pmanifest->WriteCommand(&m_LangIDs, m_lpCommand, lp[LINE_REMOVE_TRGROOT], szFile, lp[LINE_REMOVE_FLAGS], NULL);

    return TRUE;
}


//  ProcessRegCommands
//      Handles 'addreg' 'delreg' entries from the packing list
//      The format of a file line is:
//      flavor, command, root key, subkey, value name, value, flags

#define LINE_REG_ROOTKEY   0
#define LINE_REG_SUBKEY    1
#define LINE_REG_VALUENAME 2
#define LINE_REG_VALUE     3
#define LINE_REG_FLAGS     4
#define MAX_REG_LINE 5
#define MIN_REG_LINE 3

BOOL
CPackingList::ProcessRegCommands()
{
    DWORD dwc;

    // Parse the rest of the line.
    LPSTR lp[MAX_REG_LINE];
    ::ParseLine(m_lpLine, lp, MAX_REG_LINE, &dwc);
    if (dwc < MIN_REG_LINE || dwc > MAX_REG_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_REG_LINE + 1, dwc + 1);
        return FALSE;
    }

    // Write a line to the manifest
    m_pmanifest->WriteCommand(
        &m_LangIDs,
        m_lpCommand,
        lp[LINE_REG_ROOTKEY],
        lp[LINE_REG_SUBKEY],
        lp[LINE_REG_VALUENAME],
        lp[LINE_REG_VALUE],
        lp[LINE_REG_FLAGS],
        NULL);

    return TRUE;
}


//  CPackingList::ProcessShortcut
//      Processes start menu shortcuts.
//      Format of packing list file line is:
//      flavor, command, target root, target path, target file, shortcut path, shortcut file

#define LINE_SHORTCUT_TRGROOT   0
#define LINE_SHORTCUT_TRGPATH   1
#define LINE_SHORTCUT_TRGFILE   2
#define LINE_SHORTCUT_SHPATH    3
#define LINE_SHORTCUT_SHFILE    4
#define LINE_SHORTCUT_DESCRIPTION 5
#define MAX_SHORTCUT_LINE 6
#define MIN_SHORTCUT_LINE 6

BOOL
CPackingList::ProcessShortcut()
{
    DWORD dwc;
    TCHAR szFile[MAX_PATH];
    TCHAR szShortcut[MAX_PATH];

    // Parse the rest of the line.
    LPSTR lp[MAX_SHORTCUT_LINE];
    ::ParseLine(m_lpLine, lp, MAX_SHORTCUT_LINE, &dwc);
    if (dwc < MIN_SHORTCUT_LINE || dwc > MAX_SHORTCUT_LINE)
    {
        // Report number of entries including command
        ReportError("expected %d entries, received %d", MAX_SHORTCUT_LINE + 1, dwc + 1);
        return FALSE;
    }

    // Build the target path and shortcut path
    strcpy(szFile, lp[LINE_SHORTCUT_TRGPATH]);
    if (szFile[0] != 0)
        ::AppendSlash(szFile);
    strcat(szFile, lp[LINE_SHORTCUT_TRGFILE]);
    strcpy(szShortcut, lp[LINE_SHORTCUT_SHPATH]);
    if (szShortcut[0] != 0)
        ::AppendSlash(szShortcut);
    strcat(szShortcut, lp[LINE_SHORTCUT_SHFILE]);

    // Write out the manifest line
    m_pmanifest->WriteCommand(&m_LangIDs, m_lpCommand, lp[LINE_SHORTCUT_TRGROOT], szFile, szShortcut,
        lp[LINE_SHORTCUT_DESCRIPTION], NULL);

    return TRUE;
}

