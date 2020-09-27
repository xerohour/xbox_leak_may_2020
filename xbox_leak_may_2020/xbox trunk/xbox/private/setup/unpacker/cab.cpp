//  CAB.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "unpacker.h"

//---------------------------------------------------------------------
//  Prototypes for FDI callbacks

INT_PTR DIAMONDAPI openfunc(LPSTR pszFile, int oflag, int pmode);
UINT DIAMONDAPI readfunc(INT_PTR hf, LPVOID pv, UINT cb);
UINT DIAMONDAPI writefunc(INT_PTR hf, LPVOID pv, UINT cb);
int DIAMONDAPI closefunc(INT_PTR hf);
long DIAMONDAPI seekfunc(INT_PTR hf, long dist, int seektype);
LPVOID DIAMONDAPI allocfunc(ULONG cb);
void DIAMONDAPI freefunc(LPVOID pv);
INT_PTR DIAMONDAPI notifyfunc(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin);


//---------------------------------------------------------------------
// Global definitions for making a resource look like a file.
// Since we don't want to have to copy the resource out to a file, then
// just read it in again, we pretend the resource is a file.
// This makes us do evil hackery because FDI wants to treat these as files
// and doesn't provide any way for us to pass context to the callback functions
// openfunc, readfunc, etc.
// These ugly macros make a illegal file handles starting at -2 that we can
// pass like file handles.
// This evil hackery is necessary because FDI wants to 'open' the file multiple times.
// Of course, each open instance needs a unique file position. This hack gives us
// a way to cheaply map a file position for each open instance.
#define MAX_FILES_OPEN  5
#define IS_RESOURCE_HANDLE(x) ((int)(x) < 0)
#define H_TO_I(x) ((-(int)(x)) - 2)
#define I_TO_H(x) ((INT_PTR)(-(x) - 2))

// We can only have one CCab object at a time due to these unfortunate globals
// (we have to use thse because of communication with te C functions required by FDI)
    HANDLE g_fhTable[MAX_FILES_OPEN];
    DWORD g_posTable[MAX_FILES_OPEN];
    DWORD g_lenTable[MAX_FILES_OPEN];
    TCHAR g_szProcessPath[MAX_PATH];
    LPDWORD g_lpdwSeek;
    LPDWORD g_lpdwLen;

    BOOL CCab::m_fStopCopying = FALSE;
    CFileInfo CCab::m_fi;

//---------------------------------------------------------------------
//  CCab methods

//  CCab::Init
//      Prepares to read the cab files from the end of the executable

BOOL
CCab::Init()
{
    EOFSTRUCT es;
    DWORD dwc;
    int i;

    // Create the FDI context
    m_hfdi = FDICreate(allocfunc, freefunc, openfunc, readfunc, writefunc, closefunc, seekfunc,
        cpu80386, &m_erf);
    if (!m_hfdi)
        return FALSE;

    // Did we get passed in a cab source name as a command line switch?
    lstrcpyn(g_szProcessPath, GetUnpacker()->GetCabSource(), sizeof (g_szProcessPath));

    // If we didn't (the normal case), just use the current EXE to get the cabs out of
    if (g_szProcessPath[0] == 0)
        GetModuleFileName(NULL, g_szProcessPath, MAX_PATH);

    // Open the filename and seek to the end
    m_handle = CreateFile(g_szProcessPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (m_handle == INVALID_HANDLE_VALUE)
        return FALSE;

    // We put a pointer to the packed stuff in the old exe header
    // Use it to seek and find the structure identifying our packed in data
    IMAGE_DOS_HEADER dos;
    ReadFile(m_handle, &dos, sizeof (dos), &dwc, NULL);
    DWORD dwEndPosition = *(DWORD*)(&dos.e_res[0]);
    SetFilePointer(m_handle, dwEndPosition - sizeof (EOFSTRUCT), NULL, FILE_BEGIN);

    // Read the structure identifying the number of files appended
    if (!ReadFile(m_handle, &es, sizeof (EOFSTRUCT), &dwc, NULL) ||
        dwc != sizeof (EOFSTRUCT) ||
        es.dwSignature != EXE_SIGNATURE)
    {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
        return FALSE;
    }
    m_dwcFiles = es.dwcFiles;

    // Initialize the handle table
    for (i = 0 ; i < MAX_FILES_OPEN ; ++i)
        g_fhTable[i] = INVALID_HANDLE_VALUE;

    // Read the file lengths in
    DWORD dwcb = m_dwcFiles * sizeof (DWORD);
    g_lpdwSeek = (LPDWORD)malloc(dwcb);
    g_lpdwLen = (LPDWORD)malloc(dwcb);
    SetFilePointer(m_handle, dwEndPosition - sizeof (EOFSTRUCT) - dwcb, NULL, FILE_BEGIN);
    if (!ReadFile(m_handle, g_lpdwSeek, dwcb, &dwc, NULL) || dwc != dwcb)
    {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    // Walk through the file list in reverse, computing the seek positions.
    // The file contains the sizes of each sub file. We subtract to compute the seek position
    DWORD dwPosition = SetFilePointer(m_handle, dwEndPosition - sizeof (EOFSTRUCT) - dwcb, NULL, FILE_BEGIN);
    DWORD dwLastPosition = dwPosition;
    for (i = (int)m_dwcFiles - 1 ; i > -1 ; --i)
    {
        g_lpdwSeek[i] = dwPosition - g_lpdwSeek[i];
        dwPosition = g_lpdwSeek[i];
        g_lpdwLen[i] = dwLastPosition - dwPosition;
        dwLastPosition = dwPosition;
    }
    return TRUE;
}


//  CCab::Close
//      Cleans up after the object so we can reuse it

void
CCab::Close()
{
    if (m_hfdi)
        FDIDestroy(m_hfdi);
    m_hfdi = NULL;

    if (m_handle != INVALID_HANDLE_VALUE)
        CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
}


//  CCab::~CCab

CCab::~CCab()
{
    Close();
}


//  CCab::ProcessWithManifest
//      Drives the file copy process.
//      This routine returns when all files in the cab have been processed.
//      Uses the manifest to decide where the files should land.
//      The dwFile parameter is a zero-based index into the file list appended onto the EXE

BOOL
CCab::ProcessWithManifest(
    CManifest* pmanifest,
    DWORD dwFile
    )
{
    BOOL fRet;
    TCHAR szShortcut[MAX_PATH];
    LPSTR lpShortcutRoot;

    // Make a special filename by doing * and a file number. This is the sequence number
    // of the file appended to the executable.
    char lpName[3];
    lpName[0] = '*';
    lpName[1] = '0' + ((UCHAR)dwFile % 10);
    lpName[2] = 0;

    // Save the manifest instance so we can retrieve in the callback
    m_pmanifest = pmanifest;

    // Reset manifest EOF flag
    m_fEOF = FALSE;
    m_fStopCopying = FALSE;

    // Write out uninstall info so that uninstall will delete all our
    // start menu shortcuts
    SHGetFolderPath(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, szShortcut);
    ::AppendSlash(szShortcut);
    lpShortcutRoot = m_psettings->GetSetting(CSTR_STARTMENUFOLDER);
    strcat(szShortcut, lpShortcutRoot);
    free(lpShortcutRoot);
    m_puninstall->AddDir(szShortcut);


    // Reset the manifest. We assume it's already been initialized here.
    // If this returns TRUE, we're already at EOF and must have a corrupt
    // installer.
    if (pmanifest->ResetPointer())
    {
        GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1009);
        return FALSE;
    }

    // Use '*' as the name of the file. This tells us to use the file appended to the EXE
    // instead of a separate file. Saves copying the cabfile out to a temp file
    // Pass in a this pointer so we can call back to the object

    fRet = FDICopy(m_hfdi, lpName, "", 0, notifyfunc, NULL, (LPVOID)this);
    if( !fRet )
    {
        GetUnpacker()->PostCopyError(IDS_SETUP_NOT_COMPLETE, 1010);
        return FALSE;
    }

    // Make sure the progress bar has been updated
    GetUnpacker()->UpdateProgressBar(0, 0, NULL);

    // Handle remaining manifest items
    if (!m_fEOF)
    {
        LPSTR lpLine;
        LPSTR lpCommand;

        do
        {
            // Get the current manifest line
            lpLine = m_pmanifest->GetLine();

            // Get the Lang IDs for the current line
            CLangID langID;
            LPSTR lpLangIDs = lpLine;
            ::ZapComma(lpLine);
            langID.ParseLangIDs( lpLangIDs );

            // If the langID is not valid, skip this line
            if( !langID.IsLangIDValid( GetUserDefaultUILanguageWithDefault() ) )
                continue;

            // Get the command
            lpCommand = lpLine;
            ::ZapComma(lpLine);
    
            // If it's a copy command, handle it
            if (_stricmp(lpCommand, "file") == 0)
            {
                DebugPrint("Got file command after cab exhausted\n");
                GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1011);
                return FALSE;
            }
    
            // Process other commands. Bail out if something bad happens
            // Assume it's already set the error
            if (!ProcessManifestLine(lpCommand, lpLine))
                return FALSE;
        }
        while (!m_pmanifest->SkipToNextLine());
    }

    // Make sure the progress bar has been updated
    GetUnpacker()->UpdateProgressBar(0, 0, NULL);

    return fRet;
}


//  CCab::ProcessWithoutManifest
//      Drives file copying in the no manifest case.
//      Without a manifest, all files are extracted to relative paths below
//      the passed-in path.
//      The dwFile parameter is a zero-based index into the file list appended onto the EXE

CCab::ProcessNoManifest(
    LPCSTR lpRoot,
    DWORD dwFile
    )
{
    // Make a special filename by doing * and a file number. This is the sequence number
    // of the file appended to the executable.
    char lpName[3];
    lpName[0] = '*';
    lpName[1] = '0' + ((UCHAR)dwFile % 10);
    lpName[2] = 0;

    // Make sure there's no manifest marked
    m_pmanifest = NULL;
    m_fStopCopying = FALSE;

    // Save the root directory. Make sure there's a trailing '\'
    strcpy(m_szNoManifestRoot, lpRoot);
    ::AppendSlash(m_szNoManifestRoot);

    // Use '*' as the name of the file. This tells us to use the resource
    // instead of a file. Saves copying the cabfile out to a temp file
    // Pass in a this pointer so we can call back to the object
    return FDICopy(m_hfdi, lpName, "", 0, notifyfunc, NULL, (LPVOID)this);
}


INT_PTR
CCab::Callback(
    FDINOTIFICATIONTYPE fdint,
    PFDINOTIFICATION pfdin
    )
{
    switch (fdint)
    {
    case fdintCOPY_FILE:
        {
            if( NULL != m_pmanifest )
            {
                // Check to see if we really want to copy this file
                LPSTR lpLine = m_pmanifest->GetLine();

                // Get the Lang IDs for the current line
                CLangID langID;
                LPSTR lpLangIDs = lpLine;
                ::ZapComma(lpLine);
                langID.ParseLangIDs( lpLangIDs );

                if( !langID.IsLangIDValid( GetUserDefaultUILanguageWithDefault() ) )
                {
                    // Move our manifest to the next line
                    m_fEOF = m_pmanifest->SkipToNextLine();

                    // Return NULL to skip the current file in the CAB
                    return NULL;
                }
            }

            return FileCopyOpen(pfdin);
        }

    case fdintCLOSE_FILE_INFO:
        {
            return FileCopyClose(pfdin);
        }

    // We don't support multiple cabinet files. We should never get this.
    case fdintNEXT_CABINET:
        return -1;

    default:
        return 0;
    }
}


//  CCab::FileCopyOpen
//      Called just before each file is extracted from the cab.
//      This gives us the chance to find it in the manifest and
//      do any special processing.

INT_PTR
CCab::FileCopyOpen(
    PFDINOTIFICATION pfdin
    )
{
    LPSTR lpLine;
    LPSTR lpCommand;
    LPTARGETDIR lpTarget;
    PARSEFILE pf;
    int nRet;
    DWORD dwFlags;

    // Check to see if a stop was requested
    if (m_fStopCopying)
        return -1;

    // Use the manifest to determine where the file should be copied
    if (m_pmanifest)
    {
        // Handle as many lines as possible until we encounter a 'file' command.
        while (TRUE)
        {
            // Get the current manifest line
            lpLine = m_pmanifest->GetLine();

            // Get the Lang IDs for the current line
            CLangID langID;
            LPSTR lpLangIDs = lpLine;
            ::ZapComma(lpLine);
            langID.ParseLangIDs( lpLangIDs );

            // Get the command
            lpCommand = lpLine;
            ::ZapComma(lpLine);

            // If it's a copy command, handle it
            if (_stricmp(lpCommand, "file") == 0)
                break;

            // Only process the line if our LangID is correct
            if( langID.IsLangIDValid( GetUserDefaultUILanguageWithDefault() ) )
            {
                // Process other commands. Bail out if something bad happens
                // Assume it's already set the error
                if (!ProcessManifestLine(lpCommand, lpLine))
                    return -1;
            }

            // Skip to next line. Save EOF status away for later
            m_fEOF = m_pmanifest->SkipToNextLine();
        }

        // Parse the manifest line
        ::ParseLine(lpLine, (LPSTR*)&pf, PARSEFILE_DWORDS, NULL);

        // Make sure the file is the one we expected
        if (_stricmp(pf.lpDest, pfdin->psz1) != 0)
        {
            DebugPrint("File %s encountered, %s expected\n", pfdin->psz1, pf.lpDest);
            GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1012);
            return -1;
        }

        // Find the target root the file is relative to
        // This was validated during the disk space check
        lpTarget = m_pdirs->FindTarget(pf.lpRoot);

        // Strip off the extra target root we prepended in the packer
        LPSTR lpSkip = pf.lpDest;
        for (; *lpSkip != '\\' ; ++lpSkip)
        {
            if (*lpSkip == 0)
                break;
        }
        if (*lpSkip != 0)
            ++lpSkip;

        // Now we have the file entry that matches the file we're about to copy.
        // Prepend the correct path onto the file
        strcpy(m_szPath, lpTarget->szPath);
        ::AppendSlash(m_szPath);
        strcat(m_szPath, lpSkip);

        // Save away the fileinfo stuff, we'll need it later to set the accurate file time of the file
        m_fi.Load(pf.lpFileInfo);

        // Get flags
        dwFlags = GetUnpacker()->AccumulateFlags(pf.lpFlags);

        // Check and see if we want to OLE register this file
        if (dwFlags & FILEFLAGS_OLEREGISTER)
            m_fRegister = TRUE;
        else
            m_fRegister = FALSE;

        // Decide if we want to write uninstall info for this file
        dwFlags |= lpTarget->dwFlags;

        // Put the file in the uninstall info if the user didn't say not to
        if (!(dwFlags & FILEFLAGS_NOUNINSTALL))
            m_puninstall->AddFile(m_szPath, pf.lpFileInfo, m_fRegister);

        // Skip the manifest to the next entry, we're ready regardless
        m_fEOF = m_pmanifest->SkipToNextLine();

        // Handle file overwriting
        nRet = PrepareFileOverwrite(lpTarget, pf.lpRoot, m_szPath, pf.lpFlags, pf.lpFileInfo);

        if (nRet == 0 || nRet == -1)
            return nRet;
    }

    // Otherwise, with no manifest, just overwrite all files in the specified directory
    else
    {
        strcpy(m_szPath, m_szNoManifestRoot);
        strcat(m_szPath, pfdin->psz1);
    }

    // Finally call the wrapper that opens the file
    return openfunc(m_szPath, _O_BINARY | _O_TRUNC | _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
}


//  CCab::FileCopyClose
//      Called just after the file is copied. This allows us to mark the date
//      and attributes as stored in the cab file.

INT_PTR
CCab::FileCopyClose(
    PFDINOTIFICATION pfdin
    )
{
    if (!AdjustFileTime(pfdin->hf))
        return -1;

    closefunc(pfdin->hf);

    if (!SetFileAttributes(m_szPath, Attr32FromAttrFAT(pfdin->attribs)))
        return -1 ;

    // If this is a delay until reboot we need to setup everything
    // for the reboot case.
    if (m_fDelayUntilReboot)
    {   
        // 1. Call MoveFileEx
        MoveFileEx(m_szPath, m_szFinalPath, MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
        // 2. If the m_fRegister Flag is set, add an entry to run once
        //    to have Regsvr32 register the file on reboot.
        if(m_fRegister)
        {
            RegisterDLLOnReboot(m_szFinalPath);
            m_fRegister = FALSE;
        }
        // 3. Set the reboot required flag.
        GetUnpacker()->SetFlags(UNPACKER_FLAGS_REBOOT_REQUIRED);
        m_fDelayUntilReboot = FALSE;
    } else

    // OLE Register the file if necessary
    if (m_fRegister)
    {
        RegisterDLL(m_szPath, FALSE);
        m_fRegister = FALSE;
    }

    return TRUE;
}


//  CCab::ProcessManifestLine
//      Processes a single line in the manifest.
//      Returns TRUE of the line was handled.

BOOL
CCab::ProcessManifestLine(
    LPSTR lpCommand,
    LPSTR lpLine
    )
{
    // 'copy' command. Copies a file to elsewhere on the disk saving space in the cab
    if (_stricmp(lpCommand, "copy") == 0)
    {
        return ProcessManifestCopyCommand(lpLine);
    }
    else if (_stricmp(lpCommand, "remove") == 0 ||
             _stricmp(lpCommand, "removedir") == 0)
    {
        return ProcessManifestRemoveCommand(lpCommand, lpLine);
    }
    else if (_stricmp(lpCommand, "addreg") == 0 ||
             _stricmp(lpCommand, "delreg") == 0)
    {
        return ProcessManifestRegCommand(lpCommand, lpLine);
    }
    else if (_stricmp(lpCommand, "shortcut") == 0)
    {
        return ProcessManifestShortcut(lpLine);
    }

    // Any other command isn't handled here
    else
    {
        DebugPrint("Unhandled command in ProcessManifestLine (%s)\n", lpCommand);
        GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1013);
        return FALSE;
    }

    return TRUE;
}


//  CCab::ProcessManifestCopyCommand
//      Handles the 'copy' command from the manifest

BOOL CCab::ProcessManifestCopyCommand(
    LPSTR lpLine
    )
{

    PARSECOPY pc;
    LPTARGETDIR lpTarget;
    TCHAR szSource[MAX_PATH];
    TCHAR szDest[MAX_PATH];
    int nRet;
    DWORD dwFlags;

    // Parse the line
    ::ParseLine(lpLine, (LPSTR*)&pc, PARSECOPY_DWORDS, NULL);

    // Find the target root the file is relative to
    // This was validated during the disk space check
    lpTarget = m_pdirs->FindTarget(pc.lpRoot);

    // Strip off the extra target root we prepended
    LPSTR lpSkipDest = pc.lpDest;
    for (; *lpSkipDest != '\\' ; ++lpSkipDest)
    {
        if (*lpSkipDest == 0)
            break;
    }
    if (*lpSkipDest != 0)
        ++lpSkipDest;
    LPSTR lpSkipSrc = pc.lpSource;
    for (; *lpSkipSrc != '\\' ; ++lpSkipSrc)
    {
        if (*lpSkipSrc == 0)
            break;
    }
    if (*lpSkipSrc != 0)
        ++lpSkipSrc;

    // Now we have the file entry that matches the file we're about to copy.
    // Prepend the correct path onto source and dest
    strcpy(szSource, lpTarget->szPath);
    ::AppendSlash(szSource);
    strcat(szSource, lpSkipSrc);
    strcpy(szDest, lpTarget->szPath);
    ::AppendSlash(szDest);
    strcat(szDest, lpSkipDest);

    // Get flags so we can decide if we want to write uninstall info for this file
    dwFlags = GetUnpacker()->AccumulateFlags(pc.lpFlags) | lpTarget->dwFlags;

    // Put the file in the uninstall info if the user didn't say not to
    if (!(dwFlags & FILEFLAGS_NOUNINSTALL))
        m_puninstall->AddFile(szDest, pc.lpFileInfo, FALSE);

    // Make sure the file's ready to be overwritten
    nRet = PrepareFileOverwrite(lpTarget, pc.lpRoot, szDest, pc.lpFlags, pc.lpFileInfo);
    if (nRet == -1)
        return FALSE;
    if (nRet != 0)
    {
        MakeDirectory(szDest);
        if (!CopyFile(szSource, szDest, FALSE))
        {
            // BUGBUG: this should be an abort retry ignore
            DebugPrint("CopyFile(%s, %s) failed, error=%d\n", szSource, szDest, GetLastError());
            GetUnpacker()->PostCopyError(IDS_FILE_COPY_PROBLEM, 1014);
            return FALSE;
        }

        // Set the correct time for the file
        CFileInfo fi;
        fi.Load(pc.lpFileInfo);
        HANDLE fh = CreateFile(szDest, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        if (fh != INVALID_HANDLE_VALUE)
        {
            SetFileTime(fh, fi.GetFiletime(), fi.GetFiletime(), fi.GetFiletime());
            CloseHandle(fh);
        }

    }
    return TRUE;
}


//  CCab::ProcessManifestRemoveCommand
//      Handles the 'remove' command from the manifest
BOOL
CCab::ProcessManifestRemoveCommand(
    LPSTR lpCommand,
    LPSTR lpLine
    )
{
    PARSEDELETE pd;
    LPTARGETDIR lpTarget;
    TCHAR szDest[MAX_PATH];

    // Parse the line
    ::ParseLine(lpLine, (LPSTR*)&pd, PARSEDELETE_DWORDS, NULL);

    // Get the flags from this command
    DWORD dwFlags = GetUnpacker()->AccumulateFlags(pd.lpFlags);

    // Check to see if this command is a 'first-run-only' command.
    // If it is, and this isn't a first run of this setup engine, we skip the command
    if ((dwFlags & REMOVELINE_FLAGS_ONLYONFIRSTRUN) &&
        (GetUnpacker()->GetFlags() & UNPACKER_FLAGS_SETUPHASRUNBEFORE))
    {
        return TRUE;
    }

    // Find the target root the file is relative to
    // This was validated during the disk space check
    lpTarget = m_pdirs->FindTarget(pd.lpRoot);
    
    //If the target directory doesn't exist then we fail.  This is a problem with the CSV file,
    //it has a remove command, but the target path is not defined.  Ideally, the packer would check
    //for this and never build the setup, but it doesn't yet.
    if(!lpTarget)
    {
        GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 5000);
        return FALSE;
    }

    // Prepend the correct path.
    strcpy(szDest, lpTarget->szPath);
    ::AppendSlash(szDest);
    strcat(szDest, pd.lpDest);

    // Check for directory or file deletes
    if (_stricmp(lpCommand, "removedir") == 0)
    {
        // Delete the file. We don't check error because it's ok if the files are not there.
        NukeDirectory(szDest);
    }

    else // remove
    {
        // Check first to see if the file is readonly. If it is, set it to not be
        DWORD dwAttr = GetFileAttributes(szDest);
        if (dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_READONLY))
            SetFileAttributes(szDest, dwAttr & ~FILE_ATTRIBUTE_READONLY);

        // Delete the file. We don't check error because it's ok if the file's not there.
        DeleteFile(szDest);
    }

    return TRUE;
}


//  CCab::NukeDirectory
//      Does a tree delete of a directory and everything below it. Use carefully!

VOID
CCab::NukeDirectory(
    LPSTR lpdir
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;
    TCHAR szPath[MAX_PATH];

    // Create the find path with a \*.* for the search
    strcpy(szPath, lpdir);
    ::AppendSlash(szPath);
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
                ::AppendSlash(szPath);
                strcat(szPath, find.cFileName);
                NukeDirectory(szPath);
            }

            // Otherwise, delete the file
            else
            {
                strcpy(szPath, lpdir);
                ::AppendSlash(szPath);
                strcat(szPath, find.cFileName);

                // Check first to see if the file is readonly. If it is, set it to not be
                DWORD dwAttr = GetFileAttributes(szPath);
                if (dwAttr & FILE_ATTRIBUTE_READONLY)
                    SetFileAttributes(szPath, dwAttr & ~FILE_ATTRIBUTE_READONLY);

                DeleteFile(szPath);
            }
        }
        while (FindNextFile(hfind, &find));
        FindClose(hfind);
    }

    // Nuke the directory
    RemoveDirectory(lpdir);
}


//  CCab::ProcessManifestRegCommand
//      Handles registry commands from the manifest

BOOL
CCab::ProcessManifestRegCommand(
    LPSTR lpCommand,
    LPSTR lpLine
    )
{
    PARSEREG pr;
    HKEY hkeyRoot;
    HKEY hkey;
    DWORD dw;
    LONG lRet;

    // Update status
    GetUnpacker()->UpdateProgressBar(0, IDS_UPDATING_REGISTRY, "");

    // Parse the line
    ::ParseLine(lpLine, (LPSTR*)&pr, PARSEREG_DWORDS, NULL);

    // Translate the key values
    if (_stricmp(pr.lpRootKey, "HKLM") == 0)
        hkeyRoot = HKEY_LOCAL_MACHINE;
    else if (_stricmp(pr.lpRootKey, "HKCU") == 0)
        hkeyRoot = HKEY_CURRENT_USER;
    else
    {
        DebugPrint("Reg root key value wasn't HKLM or HKCU\n");
        GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1015);
        return FALSE;
    }

    // Remove the key or value if it's a delreg
    if (*lpCommand == 'd')
    {
        if (*pr.lpValueName == 0)
            RegDeleteKey(hkeyRoot, pr.lpSubKey);
        else
        {
            if (RegOpenKeyEx(hkeyRoot, pr.lpSubKey, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
            {
                RegDeleteValue(hkey, pr.lpValueName);
                RegCloseKey(hkey);
            }
        }
    }

    // Otherwise add the key
    else
    {
        // Read the key
        if (RegCreateKeyEx(hkeyRoot, pr.lpSubKey, 0, NULL, 0, KEY_ALL_ACCESS,
                           NULL, &hkey, &dw) != ERROR_SUCCESS)
        {
            DebugPrint("Couldn't create reg key\n");
            GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1016);
            return FALSE;
        }

        // Add the uninstall information for the key
        m_puninstall->AddReg(pr.lpRootKey, pr.lpSubKey, pr.lpValueName);

        // Get the flags from this manifest item
        DWORD dwFlags = GetUnpacker()->AccumulateFlags(pr.lpFlags);

        // Is it a DWORD regkey?
        if (dwFlags & REGLINE_FLAGS_DWORD)
        {
            DWORD dw = atoi(pr.lpValue);
            lRet = RegSetValueEx(hkey, pr.lpValueName, 0, REG_DWORD, (BYTE*)&dw, sizeof (DWORD));
        }

        // default is REG_SZ
        else
        {
            //Substitute target paths in string values
            char szValue[4096];
            SubstitutePathsInString(pr.lpValue, szValue);
            // Set the value
            lRet = RegSetValueEx(hkey, pr.lpValueName, 0, REG_SZ, (BYTE *)szValue,
                                 strlen(szValue) + 1);
        }

        RegCloseKey(hkey);
        if (lRet != ERROR_SUCCESS)
        {
            DebugPrint("Couldn't create reg value\n");
            GetUnpacker()->PostCopyError(IDS_CORRUPT_INSTALLER, 1017);
            return FALSE;
        }
    }
    return TRUE;
}


//  CCab::PrepareFileOverwrite
//      Prepares a file for overwriting. Depending on flags this may mean
//      prompting or just blasting the file. This routine is used for
//      both 'copy' and 'file' commands.
//      Returns: 
//               1 to continue decompressing the current file
//               0 to skip this file
//              -1 to bail out of the copy on error (PostCopyError has been called)
//      Comment: this function is pretty expensive: an open and a findfirst for
//      each file located. It might be possible to optimize this.

int
CCab::PrepareFileOverwrite(
    LPTARGETDIR lpTarget,
    LPSTR lpRoot,
    LPSTR lpDest,
    LPSTR lpFlags,
    LPSTR lpFileInfo
    )
{
    HANDLE hfile;
    DWORD dwErrorOpen;
    CFileInfo fi;

    // Update the progress indicator
    fi.Load(lpFileInfo);
    GetUnpacker()->UpdateProgressBar(fi.GetSize(), IDS_COPYING_FILE, lpDest);

tryagain:
    dwErrorOpen = ERROR_SUCCESS;

    // Try to open the file
    hfile = CreateFile(lpDest, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, 0, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
        dwErrorOpen = GetLastError();
    else
        CloseHandle(hfile);

    // If the file or path wasn't found, this is good, return OK
    if (dwErrorOpen == ERROR_FILE_NOT_FOUND || dwErrorOpen == ERROR_PATH_NOT_FOUND)
        return 1;

    //
    //  Most likely, the flags are needed to resolve this.
    //

    // Flags from the user are always highest priority
    DWORD dwFlags = m_dwOverwriteFlags;

    // Manifest flags always supercede directory flags if they exist
    if (dwFlags == 0)
    dwFlags = GetUnpacker()->AccumulateFlags(lpFlags);

    // Finally, if no other flags, get from the targetdir
    if (dwFlags == 0)
            dwFlags = lpTarget->dwFlags;


    // If the file was found and opened successfully, the file exists. See if
    // it's the same as the file we're trying to copy down. If so, we can skip the
    // file since there's no need to copy it.
    if (dwErrorOpen == ERROR_SUCCESS)
    {
        // Dates may or may not be valid. Depending on how flags are set in the installer,
        // possibly ignore dates altogether
        if (!(GetUnpacker()->GetFlags() & UNPACKER_FLAGS_SETUPHASRUNBEFORE))
            return 1;

        int nRet;
        nRet = fi.Compare(lpDest);

        // If files match, we can skip the file
        if (nRet == 0)
            return 0;

        // If the file is older, we allow the copy
        if (nRet == 1)
            return 1;

        // Default is to prompt
        if (dwFlags & FILEFLAGS_CLOBBER)
            return 1;
        else if (dwFlags & FILEFLAGS_NEVERCLOBBER)
            return 0;

        // Returns -1 on cancel, 0 on skip file, 1 on clobber file
        strcpy(m_szPath, lpDest);
        return DialogBoxParam(::GetInstanceHandle(), MAKEINTRESOURCE(IDD_OVERWRITE),
            GetUnpacker()->GetHWNDDlg(), (DLGPROC)OverwriteDlgProc, (LPARAM)this);
    }

    // If file is busy or other error situation, give the user a chance to resolve
    else
    {
        // Check first to see if the file is readonly. If it is, make it not then try again
        DWORD dwAttr = GetFileAttributes(lpDest);
        if (dwAttr & FILE_ATTRIBUTE_READONLY)
        {
            SetFileAttributes(lpDest, dwAttr & ~FILE_ATTRIBUTE_READONLY);
            goto tryagain;
        }

        // This file may require a reboot to replace.  This is only supported
        // by a file command (not by a copy command).  HACK ALERT: we distinguish
        // the caller based on whether m_szPath is passed as lpDest.
        if ((dwFlags & FILEFLAGS_SYSTEMREBOOT)&&(m_szPath == lpDest))
        {
            // Copy the m_szPath path to the m_szFinalPath
            strcpy(m_szFinalPath, m_szPath);
            
            // Change m_szPath to be a temporary path
            LPSTR lpParse = strrchr(m_szFinalPath, '\\');   // Borrow the path in m_szFinalPath,
            *lpParse='\0';                                  //  by temporarily terminating before the filename.
            BOOL fResult = GetTempFileName(m_szFinalPath, "unp", 0, m_szPath);
            *lpParse='\\';                                  // Done borrow m_szFinalPath, restore it.

            // If for some reason we couldn't create the temporary file,
            // return failure.
            if(!fResult)                    
            {
                GetUnpacker()->PostCopyError(IDS_FILE_COPY_PROBLEM, 6000);
                strcpy(m_szPath, m_szFinalPath);  //Copy the path back in case GetTempFile clobbered it.
                return -1; //
            }
            
            // Set the delay until reboot flag, this will be processed after 
            // the copy to the temporary file.
            m_fDelayUntilReboot = TRUE;

            return 1; //proceed with copy
        }

        // Lots of stack depth, yuck.
        TCHAR sz[512];
        TCHAR szOutput[512];
        int nRet;

        LoadString(::GetInstanceHandle(), IDS_MB_COPYPROBLEM, sz, 512);
        sprintf(szOutput, sz, lpDest);
        nRet = MessageBox(GetUnpacker()->GetHWNDDlg(), szOutput, CUnpacker::GetProductName(), MB_YESNOCANCEL);

        // If they told us to retry, do so
        if (nRet == IDYES)
            goto tryagain;

        // If no retry, skip the file
        if (nRet == IDNO)
            return 0;

        // If cancel, tell the caller to bail
        if (nRet == IDCANCEL)
            return -1;
    }
    return 1;
}


//  CCab::ProcessManifestShortcut
//      Creates a windows shell program group icon

BOOL
CCab::ProcessManifestShortcut(
    LPSTR lpLine
    )
{
    PARSESHORTCUT ps;
    LPTARGETDIR lpTarget;
    TCHAR szDest[MAX_PATH];
    TCHAR szShortcut[MAX_PATH];
    LPSTR lpShortcutRoot;

    // Parse the line
    ::ParseLine(lpLine, (LPSTR*)&ps, PARSESHORTCUT_DWORDS, NULL);

    // Find the target root the file is relative to
    // This was validated during the disk space check
    lpTarget = m_pdirs->FindTarget(ps.lpRoot);

    // Prepend the correct path.
    strcpy(szDest, lpTarget->szPath);
    ::AppendSlash(szDest);
    strcat(szDest, ps.lpDest);

    // Get the path into the program group folder
    SHGetFolderPath(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, szShortcut);
    ::AppendSlash(szShortcut);
    lpShortcutRoot = m_psettings->GetSetting(CSTR_STARTMENUFOLDER);
    strcat(szShortcut, lpShortcutRoot);
    free(lpShortcutRoot);
    ::AppendSlash(szShortcut);
    strcat(szShortcut, ps.lpShortcutPath);

    // Create the shortcut. Note that if it fails, we don't fail setup. What
    // good would this do?
    MakeDirectory(szShortcut);
    if (FAILED(CreateLink(szDest, szShortcut, ps.lpDescription)))
    {
        DebugPrint("Failed creating shortcut to %s (%s), continuing setup\n",
                   szDest, szShortcut);
    }

    return TRUE;
}


//  CCab::CreateLink
//      Uses the Shell's IShellLink and IPersistFile interfaces 
//      to create and store a shortcut to the specified object. 
//      Returns the result of calling the member functions of the interfaces. 
//      lpszPathObj - address of a buffer containing the path of the object. 
//      lpszPathLink - address of a buffer containing the path where the 
//          Shell link is to be stored. 
//      lpszDesc - address of a buffer containing the description of the 
//          Shell link. 
 
HRESULT
CCab::CreateLink(
    LPCSTR lpszPathObj, 
    LPCSTR lpszPathLink,
    LPCSTR lpszDesc
    ) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, 
        CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl); 
    if (SUCCEEDED(hres))
    { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the 
        // description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
 
       // Query IShellLink for the IPersistFile interface for saving the 
       // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres))
        { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
    return hres; 
}


//---------------------------------------------------------------------
//  FDI Callback functions

//  openfunc
//      FDI callback function
//      Maps _open semantics to CreateFile

INT_PTR DIAMONDAPI
openfunc(
    LPSTR pszFile,
    int oflag,
    int pmode
    )
{
    HANDLE  FileHandle;
    BOOL    fExists     = FALSE;
    DWORD   fAccess;
    DWORD   fCreate;
    DWORD   dwShareMode = 0;

    // The special filename '*' means that we are using a resource as a file.
    // Allocate a special illegal handle to identify it. We have to be able to
    // keep track of multiple simultaneous opens because FDI will call open
    // multiple times and expect the seek pointer to be instanced.
    if (pszFile[0] == '*')
    {
        int i;

        // Find an unused slot
        for (i = 0 ; i < MAX_FILES_OPEN ; ++i)
        {
            if (g_fhTable[i] == INVALID_HANDLE_VALUE)
                break;
        }

        // No slots left, bail out
        if (i == MAX_FILES_OPEN)
            return -1;

        // Create a duplicate handle to the file
        g_fhTable[i] = CreateFile(g_szProcessPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        g_posTable[i] = g_lpdwSeek[pszFile[1] - '0'];
        g_lenTable[i] = g_lpdwLen[pszFile[1] - '0'];
        SetFilePointer(g_fhTable[i], g_posTable[i], NULL, FILE_BEGIN);
        return I_TO_H(i);
    }

    // BUGBUG: No Append Mode Support
    if (oflag & _O_APPEND)
        return -1;

    // Set Read-Write Access
    if ((oflag & _O_RDWR) || (oflag & _O_WRONLY))
        fAccess = GENERIC_WRITE;
    else
        fAccess = GENERIC_READ;

     // Set Create Flags
    if (oflag & _O_CREAT)
    {
        if (oflag & _O_EXCL)
            fCreate = CREATE_NEW;
        else if (oflag & _O_TRUNC)
            fCreate = CREATE_ALWAYS;
        else
            fCreate = OPEN_ALWAYS;
    }
    else
    {
        if (oflag & _O_TRUNC)
            fCreate = TRUNCATE_EXISTING;
        else
            fCreate = OPEN_EXISTING;
    }

    // Set sharing flags
    if (pmode & _S_IREAD)
        dwShareMode |= FILE_SHARE_READ;
    if (pmode & _S_IWRITE)
        dwShareMode |= FILE_SHARE_WRITE;

    FileHandle = CreateFile(pszFile, fAccess, dwShareMode, NULL, fCreate,
                            FILE_ATTRIBUTE_NORMAL, NULL);

    if ((FileHandle == INVALID_HANDLE_VALUE) && (fCreate != OPEN_EXISTING))
    {
        CCab::MakeDirectory(pszFile);
        FileHandle = CreateFile(pszFile, fAccess, 0, NULL, fCreate,
                                FILE_ATTRIBUTE_NORMAL, NULL);
    }
    return (INT_PTR)FileHandle;
}

UINT DIAMONDAPI
readfunc(
    INT_PTR hf,
    LPVOID pv,
    UINT cb
    )
{
    // Special read from EXE: get the EXE file handle and do the read from
    // the current position
    if (IS_RESOURCE_HANDLE(hf))
        hf = (INT_PTR)g_fhTable[H_TO_I(hf)];

    // Do the read
    if (!ReadFile((HANDLE)hf, pv, cb, (DWORD *)&cb, NULL))
        return -1;
    else
        return cb;
}

UINT DIAMONDAPI
writefunc(
    INT_PTR hf,
    LPVOID pv,
    UINT cb
    )
{
    // Check for resource read
    if (IS_RESOURCE_HANDLE(hf))
    {
        DebugPrint("Error: trying to write to resource!!\n");
        return -1;
    }

    // Normal write
    if (!WriteFile((HANDLE)hf, pv, cb, (DWORD *) &cb, NULL))
        return -1;
    else
        return cb;
}

int DIAMONDAPI
closefunc(
    INT_PTR hf
    )
{
    if (IS_RESOURCE_HANDLE(hf))
    {
        CloseHandle(g_fhTable[H_TO_I(hf)]);
        g_fhTable[H_TO_I(hf)] = INVALID_HANDLE_VALUE;
    }
    else
    {
        CloseHandle((HANDLE)hf);
    }

    return 0;
}

long DIAMONDAPI
seekfunc(
    INT_PTR hf,
    long dist,
    int seektype
    )
{
    DWORD W32seektype;

    // Check for resource seek
    if (IS_RESOURCE_HANDLE(hf))
    {
        DWORD i = H_TO_I(hf);
        HANDLE fh = g_fhTable[i];
        DWORD dwPosition = g_posTable[i];

        switch (seektype)
        {
        case SEEK_SET:
            W32seektype = FILE_BEGIN;
            dist += dwPosition;
            break;
        case SEEK_CUR:
            W32seektype = FILE_CURRENT;
            break;
        case SEEK_END:
            W32seektype = FILE_BEGIN;
            dist += dwPosition + g_lenTable[i];
            break;
        }

        return SetFilePointer(fh, dist, NULL, W32seektype) - dwPosition;
    }

    // Normal seek
    else
    {
        int W32seektype;

        switch (seektype)
        {
        case SEEK_SET:
            W32seektype = FILE_BEGIN;
            break;
        case SEEK_CUR:
            W32seektype = FILE_CURRENT;
            break;
        case SEEK_END:
            W32seektype = FILE_END;
            break;
        }
        return SetFilePointer((HANDLE)hf, dist, NULL, W32seektype);
    }
}

LPVOID DIAMONDAPI
allocfunc(
    ULONG cb
    )
{
    return (LPVOID)GlobalAlloc(GMEM_FIXED, cb);
}


void DIAMONDAPI
freefunc(
    LPVOID pv
    )
{
    GlobalFree(pv);
}

//  notifyfunc
//      This function is a bit different from the others in that it
//      is the only way we get control of the file copying process.
//      It's much more convenient to get this inside a member function
//      so this routine just turns around and calls the CCab object from
//      instance data passed through FDICopy.

INT_PTR DIAMONDAPI
notifyfunc(
    FDINOTIFICATIONTYPE fdint,
    PFDINOTIFICATION pfdin
    )
{
    // Call the member function
    return ((CCab*)pfdin->pv)->Callback(fdint, pfdin);
}


//---------------------------------------------------------------------
//  Static helper functions
//  (called from _cdecl callback functions)

//  MakeDirectory
//      Creates all directories along a path

VOID
CCab::MakeDirectory(
    LPCTSTR pszPath
    )
{
    LPTSTR pchChopper;
    int cExempt;

    if (pszPath[0] != '\0')
    {
        cExempt = 0;

        if ((pszPath[1] == ':') && (pszPath[2] == '\\'))
            pchChopper = (LPTSTR) (pszPath + 3);   // skip past "C:\"
        else if ((pszPath[0] == '\\') && (pszPath[1] == '\\'))
        {
            pchChopper = (LPTSTR) (pszPath + 2);   // skip past "\\"
            cExempt = 2;                // machine & share names exempt
        }
        else
            pchChopper = (LPTSTR) (pszPath + 1);   // skip past potential "\"

        while (*pchChopper != '\0')
        {
            if ((*pchChopper == '\\') && (*(pchChopper - 1) != ':'))
            {
                if (cExempt != 0)
                    cExempt--;
                else
                {
                    *pchChopper = '\0';
                    CreateDirectory(pszPath,NULL);
                    *pchChopper = '\\';
                }
            }
            pchChopper = CharNext(pchChopper);
        }
    }
}

//  CCab::AdjustFileTime
//      Make the file time match the time saved in the cab.
//      Note that the cab saves the date and time in DOS format (two WORDS)

BOOL
CCab::AdjustFileTime(
    INT_PTR hf
    )
{
    SetFileTime((HANDLE)hf, m_fi.GetFiletime(), m_fi.GetFiletime(), m_fi.GetFiletime());

    return TRUE;
}


//  CCab::Attr32FromAttrFAT
//      Converts from FAT attributes (cabfile) to Win32

DWORD
CCab::Attr32FromAttrFAT(
    WORD attrMSDOS
    )
{
#if 0   // 4/4/2001 [JonT] unifdef this if you want to propagate attributes.
        // Here, we always want files set as normal: not hidden, readonly, or system.
    if (attrMSDOS == _A_NORMAL)
        return FILE_ATTRIBUTE_NORMAL;

    // Mask off read-only, hidden, system, and archive bits
    // NOTE: These bits are in the same places in MS-DOS and Win32!
    return attrMSDOS & ( _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);
#else
    return FILE_ATTRIBUTE_NORMAL;
#endif
}


//  OverwriteDlgProc
//      Handle the yes, yes to all, no, no to all file overwrite dialog
//      Returns -1 on cancel, 0 on skip file, 1 on clobber file.

BOOL CALLBACK
OverwriteDlgProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    static CCab* s_pcab;    // Only one dialog up at once...

    // Check for init message where we get the 'this' pointer
    if (uMsg == WM_INITDIALOG)
        s_pcab = (CCab*)lParam;

    // Call the member function where the real processing happens
    return s_pcab->OnOverwriteMessage(hwnd, uMsg, wParam, lParam);
}

BOOL
CCab::OnOverwriteMessage(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetWindowText(GetDlgItem(hwnd, IDC_TEXT_FILENAME), m_szPath);
        FormatWindowText(GetDlgItem(hwnd, IDC_OVERWRITE_TEXT), CUnpacker::GetProductName());
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_YESTOALL:
            m_dwOverwriteFlags |= FILEFLAGS_CLOBBER;
            // fall through
        case IDYES:
            EndDialog(hwnd, 1);
            return TRUE;

        case IDC_NOTOALL:
            m_dwOverwriteFlags |= FILEFLAGS_NEVERCLOBBER;
            // fall through
        case IDNO:
            EndDialog(hwnd, 0);
            return TRUE;

        case IDCANCEL:
            EndDialog(hwnd, -1);
            return TRUE;
        }
    }

    return FALSE;
}

//  SubstituePathsInString
//      Walks a string with %foo% escape sequences.  It
//      looks for a directory name foo and if found replaces
//      %foo% with that directory name.  Otherwise, it leaves it
//      leaves %foo% alone.
void CCab::SubstitutePathsInString(LPSTR lpszInput, LPSTR lpszOutput)
{
    LPSTR  pszInputPos = lpszInput;
    LPSTR  pszOutputPos = lpszOutput;

    // Walk until the end of the input string.
    while(*pszInputPos)
    {
        // Check for % escape sequence
        if (*pszInputPos == '%')
        {   
            LPSTR pszPathEnd = ++pszInputPos;
            LPTARGETDIR pszTargetDir = NULL;

            // Find the end of the escape sequence
            while(*pszPathEnd && ('%'!=*pszPathEnd)) pszPathEnd++;

            // If we found the end, try to substitute
            if('%'==*pszPathEnd)
            {
                //%% =? %
                if(pszInputPos != pszPathEnd)
                {
                  //Look up the escape sequence.
                  *pszPathEnd = '\0';
                  pszTargetDir = m_pdirs->FindTarget(pszInputPos);
                  *pszPathEnd = '%';
                  //If we found a path, substitute it.
                  if(pszTargetDir)
                  {
                    pszInputPos = pszTargetDir->szPath;
                    while(*pszInputPos) *lpszOutput++ = *pszInputPos++;
                    pszInputPos = ++pszPathEnd;
                    continue;
                  } else
                  //Otherwise, just copy it
                  {
                    *lpszOutput++ = '%';
                    while(*pszInputPos) *lpszOutput++ = *pszInputPos++;
                  }
                }
            } else
            {
                //If we didn't find the end, then just leave the %.
                //It would be preferable to use %% to specify %, but if there
                //is a single unmatched %, we will allow it.
                pszInputPos--;
            }
        }
        // Copy the character and continue
        *lpszOutput++ = *pszInputPos++;
    }
    //Null terminate the output.
    *lpszOutput = '\0';
}


//  GetUserDefaultUILanguageWithDefault()
//      Will check the current machine language against the list of
//      all supported languages.  If it doesn't match, it will return
//      the default language that is also specified in the INI file.
LANGID
CCab::GetUserDefaultUILanguageWithDefault()
{
    BOOL bLanguageSupported = FALSE;
    m_punpacker->m_SupportedLangs.MoveFirstLangID();
    do
    {
        if( m_punpacker->m_SupportedLangs.GetCurrentLangID() == GetUserDefaultUILanguage() )
        {
            bLanguageSupported = TRUE;
            break;
        }
    } while( m_punpacker->m_SupportedLangs.MoveNextLangID() );

    if( bLanguageSupported )
        return GetUserDefaultUILanguage();
    else
        return (LANGID)m_punpacker->m_dwDefaultLang;
}