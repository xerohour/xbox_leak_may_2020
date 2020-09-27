//  UNINSTALL.CPP
//
//  Created 2-Apr-2001 [JonT]

#include "uninstall.h"

//---------------------------------------------------------------------

    HINSTANCE g_hinst;
    HINSTANCE g_hinstCustom = NULL;
    LPSTR g_lpUninstallDir = NULL;
    LPSTR g_lpFile;
    DWORD g_fStopUninstall = FALSE;
    LPSTR g_lpProductName = NULL;
    BOOL ParseCommandLine(LPSTR lpCmdLine);
    BOOL ReadUninstallFile();
    BOOL UninstallPrecheck();
    DWORD WINAPI UninstallThread(HWND hwnd);
    VOID UninstallFile(LPSTR& lp, HWND hwndText, BOOL fUnregister);
    VOID UninstallReg(LPSTR& lp, HWND hwndText);
    VOID UninstallDir(LPSTR& lp, HWND hwnd);
    BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    VOID DeleteDirectory(LPCTSTR lpDir);
    VOID DeleteEmptyDirectory(LPCTSTR lpDir);
    int  rsprintf(LPSTR pszDest, UINT uFormatResource, ...);

#define WMP_FINISHED (WM_USER + 100)

//---------------------------------------------------------------------

int WINAPI
WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nCmdShow
    )
{
    TCHAR szText[MAX_PATH];
    TCHAR szTitle[MAX_PATH];

    g_hinst = hInst;
    
    //Parse the command line.
    if(!ParseCommandLine(lpCmdLine))
    {    
        // Get the generic title, since we don't know the product name
        LoadString(g_hinst, IDS_GENERIC_TITLE, szTitle, MAX_PATH);
        LoadString(g_hinst, IDS_MB_RUNFROMCONTROLPANEL, szText, sizeof (szText));
        MessageBox(NULL, szText, szTitle, MB_OK);
        return -1;
    }

    // Load and format our fancy title that has the product name in it.
    rsprintf(g_hinst, szTitle, IDS_TITLE, g_lpProductName);

    static HANDLE s_hevNamespace = CreateEvent(NULL, TRUE, TRUE, TEXT("Local\\X_SETUP_ENGINE"));
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        LoadString(g_hinst, IDS_MB_SINGLEINSTANCEONLY, szText, sizeof (szText));
        MessageBox(NULL, szText, szTitle, MB_OK);
        return -1;
    }

    // Make sure the user is an administrator. If not, no dice
    if (!IsAdministrator())
    {
        rsprintf(g_hinst, szText, IDS_MB_NOTADMIN, g_lpProductName);
        MessageBox(NULL, szText, szTitle, MB_OK);
        return -1;
    }

    // Bogus call to make sure comctl32 is linked in. This call is never made.
    if (hInst == NULL)
        PropertySheet(NULL);

    // Read the entire uninstall file
    if (!ReadUninstallFile())
    {
        LoadString(g_hinst, IDS_CANTFINDFILE, szText, sizeof (szText));
        MessageBox(NULL, szText, szTitle, MB_OK);
        return -1;
    }

    // Load the custom dll, if present
    strcpy(szText, g_lpUninstallDir);
    ::AppendSlash(szText);
    strcat(szText, SCAB_UNINSTALLDIR);
    ::AppendSlash(szText);
    strcat(szText, SCAB_CUSTOMDLL);
    g_hinstCustom = LoadLibrary(szText);

    // Make sure that they can do this.
    if(!UninstallPrecheck())
    {
        if(g_hinstCustom) FreeLibrary(g_hinstCustom);
        return -1;
    }
    
    // Make sure they really want to do this
    rsprintf(g_hinst, szText, IDS_MB_CONFIRM, g_lpProductName);
    if (MessageBox(NULL, szText, szTitle, MB_YESNO) != IDYES)
    {
        if(g_hinstCustom) FreeLibrary(g_hinstCustom);
        return -1;
    }
    
    // Create the UI and drive the rest from there
    int nRet = DialogBox(g_hinst, MAKEINTRESOURCE(IDD_PROGRESS), NULL, DlgProc);

    // If we were successful our done message is already in szText
    // otherwise load the "not successful message."
    if (nRet == 0)
    {
        rsprintf(g_hinst, szText, IDS_MB_FINISHED, g_lpProductName);
    }
    else
        LoadString(g_hinst, IDS_MB_NOTSUCCESSFUL, szText, sizeof (szText));
    MessageBox(NULL, szText, szTitle, MB_OK);

    CloseHandle(s_hevNamespace);
    return 0;
}


//  DlgProc
//      Dialog proc for uninstall progess UI

BOOL CALLBACK
DlgProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    DWORD dw;
    HANDLE hthread;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UninstallThread, hwnd, 0, &dw);
        CloseHandle(hthread);
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            g_fStopUninstall = TRUE;
            return TRUE;
        }
        return FALSE;

    case WMP_FINISHED:
        EndDialog(hwnd, 0);
        return TRUE;
    }

    return FALSE;
}



//  ParseCommandLine
//      Parses command line to get uninstall path, and the product name

BOOL
ParseCommandLine(
    LPSTR lpCmdLine
    )
{
    LPSTR lpRet;

    // Scan for first "
    for (; *lpCmdLine != '\"' ; ++lpCmdLine)
    {
        if (*lpCmdLine == 0)
            return FALSE;
    }

    g_lpUninstallDir = ++lpCmdLine;

    // Scan for second "
    for (; *lpCmdLine != '\"' ; ++lpCmdLine)
    {
        if (*lpCmdLine == 0)
            return FALSE;
    }
    *lpCmdLine++ = 0;

    // Scan for the next "
    for (; *lpCmdLine != '\"' ; ++lpCmdLine)
    {
        if (*lpCmdLine == 0)
            return FALSE;
    }

    g_lpProductName = ++lpCmdLine;

    // Scan for the f "
    for (; *lpCmdLine != '\"' ; ++lpCmdLine)
    {
        if (*lpCmdLine == 0)
            return FALSE;
    }

    *lpCmdLine = 0;
    
    return TRUE;
}


//  ReadUninstallFile
//      Reads the uninstall file into memory

BOOL
ReadUninstallFile()
{
    TCHAR szPath[MAX_PATH];
    HANDLE hfile;
    DWORD dwLen;
    DWORD dwActuallyRead;

    // Get the uninstall filename
    strcpy(szPath, g_lpUninstallDir);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_UNINSTALLDIR);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_UNINSTALLFILE);

    // Try to open the file
    hfile = CreateFile(szPath, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
        return FALSE;

    // Read the entire file
    dwLen = GetFileSize(hfile, NULL);
    if (dwLen == 0xffffffff)
        return FALSE;

    // Allocate a buffer big enough for the whole file
    g_lpFile = (LPSTR)malloc(dwLen + 1);
    if (!g_lpFile)
        return FALSE;

    // Read it completely and zero-terminate
    if (!ReadFile(hfile, g_lpFile, dwLen, &dwActuallyRead, NULL) ||
        dwActuallyRead != dwLen)
        return FALSE;
    *(g_lpFile + dwLen) = 0;

    // Close file, we're done with it
    CloseHandle(hfile);

    return TRUE;
}

// Uninstall pre-check
//  Allows the custom.dll if present to do an uninstall precheck.
BOOL
UninstallPrecheck()
{
    BOOL fRetValue = TRUE;
    // If the custom.dll exports a PREUNINSTALLPROC, call it now
    if(g_hinstCustom)
    {
        PFNPREUNINSTALL pfnPreUninstall;
        pfnPreUninstall = (PFNPREUNINSTALL) GetProcAddress(g_hinstCustom, PREUNINSTALLPROC);
        if(pfnPreUninstall)
        {
            fRetValue = pfnPreUninstall(g_lpUninstallDir);
        }
    }
    return fRetValue;
}


//  UninstallThread
//      A thread to run the uninstall process on so that UI can be displayed smoothly

DWORD WINAPI
UninstallThread(
    HWND hwnd
    )
{
    LPSTR lpCommand;
    LPSTR lp;
    TCHAR szPath[MAX_PATH];
    HWND hwndProgress = GetDlgItem(hwnd, IDC_PROGRESS);
    HWND hwndText = GetDlgItem(hwnd, IDC_STATUS);
    PFNPOSTUNINSTALL pfnPostUninstall;

    // Set up the progress bar by walking the file to find how many lines there are
    lp = g_lpFile;
    DWORD dwcLines = 0;
    do
    {
        ++dwcLines;
    }
    while (!::SkipRestOfLine(lp));
    SendMessage(hwndProgress, PBM_SETRANGE32, 0, dwcLines);
    SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
    SendMessage(hwndProgress, PBM_SETPOS, 0, 0);

    // Loop through all the lines in the uninstall file processing them
    lp = g_lpFile;
    do
    {
        // If told to cancel, do so
        if (g_fStopUninstall)
            return 0;

        // Update progress bar
        SendMessage(hwndProgress, PBM_STEPIT, 0, 0);

        // Parse the file and handle
        lpCommand = lp;
        ::ZapComma(lp);
        if (*lpCommand == 'f')
            UninstallFile(lp, hwndText, FALSE);
        else if (*lpCommand == 'o')
            UninstallFile(lp, hwndText, TRUE);
        else if (*lpCommand == 'r')
            UninstallReg(lp, hwndText);
        else if (*lpCommand == 'd')
            UninstallDir(lp, hwndText);
    }
    while (!::SkipRestOfLine(lp));

    
    // If the custom.dll exports a POSTUNINSTALLPROC, call it now
    if(g_hinstCustom)
    {
        pfnPostUninstall = (PFNPOSTUNINSTALL) GetProcAddress(g_hinstCustom, POSTUNINSTALLPROC);
        if(pfnPostUninstall)
            pfnPostUninstall(g_lpUninstallDir);
    
    
        // We are done with the custom dll, so free it.
        FreeLibrary(g_hinstCustom);
        g_hinstCustom = NULL;
    }
    
    // Cleanup directories and uninstall stuff files
    LoadString(g_hinst, IDS_DELETINGDIRECTORIES, szPath, MAX_PATH);
    SetWindowText(hwnd, szPath);
    // Nuke the uninstall subdirectory
    strcpy(szPath, g_lpUninstallDir);
    ::AppendSlash(szPath);
    strcat(szPath, SCAB_UNINSTALLDIR);
    DeleteDirectory(szPath);
    // Nuke any empty directories.
    strcpy(szPath, g_lpUninstallDir);
    DeleteEmptyDirectory(szPath);

    // Tell UI we're done
    PostMessage(hwnd, WMP_FINISHED, 0, 0);

    return 0;
}

typedef struct _UPARSEFILE
{
    LPSTR lpPath;
    LPSTR lpFileInfo;
} UPARSEFILE;

//  UninstallFile
//      Handles the uninstalling of a file

VOID
UninstallFile(
    LPSTR& lp,
    HWND hwnd,
    BOOL fUnregister
    )
{
    UPARSEFILE pf;
    CFileInfo fi;

    // Parse the information
    ::ParseLine(lp, (LPSTR*)&pf, sizeof (UPARSEFILE) / sizeof (LPSTR), NULL);
    fi.Load(pf.lpFileInfo);

    // Display the info
    // Display status
    TCHAR sz[MAX_PATH];
    LoadString(g_hinst, IDS_DELETEFILEENTRY, sz, MAX_PATH);
    TCHAR szStatus[MAX_PATH];
    sprintf(szStatus, sz, pf.lpPath);
    SetWindowText(hwnd, szStatus);

    // See if the file matches the one we want (or is older)
    if (fi.Compare(pf.lpPath) != -1)
    {
        // OLE Unregister (regsvr32 /u) the file?
        if (fUnregister)
            RegisterDLL(pf.lpPath, TRUE);

        // Delete it
        if(!DeleteFile(pf.lpPath))
        {
            // It should not even be necessary to tell the user to reboot.
            MoveFileEx(pf.lpPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
        }
    }
}

typedef struct _UPARSEREG
{
    LPSTR lpRootKey;
    LPSTR lpSubKey;
    LPSTR lpValueName;
} UPARSEREG;

//  UninstallReg
//      Handles the uninstalling of a regkey

VOID
UninstallReg(
    LPSTR& lp,
    HWND hwnd
    )
{
    UPARSEREG pr;
    HKEY hkeyRoot;
    HKEY hkey;

    // Parse the line
    ::ParseLine(lp, (LPSTR*)&pr, sizeof (UPARSEREG) / sizeof (DWORD), NULL);

    // Display status
    TCHAR sz[MAX_PATH];
    LoadString(g_hinst, IDS_DELETEREGENTRY, sz, MAX_PATH);
    TCHAR szStatus[MAX_PATH];
    sprintf(szStatus, sz, pr.lpRootKey, pr.lpSubKey, pr.lpValueName);
    SetWindowText(hwnd, szStatus);

    // Delete the key if it's there
    // Translate the key values
    if (_stricmp(pr.lpRootKey, "HKLM") == 0)
        hkeyRoot = HKEY_LOCAL_MACHINE;
    else if (_stricmp(pr.lpRootKey, "HKCU") == 0)
        hkeyRoot = HKEY_CURRENT_USER;
    else
        return;

    // Remove the key or value if it's a delreg
    if (*pr.lpValueName == 0)
        SHDeleteKey(hkeyRoot, pr.lpSubKey);
    else
    {
        if (RegOpenKeyEx(hkeyRoot, pr.lpSubKey, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
        {
            RegDeleteValue(hkey, pr.lpValueName);

            // Check and see if there are any values left in this key
            DWORD dwSubkeys = 1;
            DWORD dwValues = 1;
            if (RegQueryInfoKey(hkey, NULL, NULL, NULL, &dwSubkeys, NULL, NULL,
                &dwValues, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            {
                // Make sure we don't delete key if the query fails
                dwSubkeys = 1;
            }
            RegCloseKey(hkey);

            // If there were no more keys or values under the key, nuke it
            if (dwSubkeys == 0 && dwValues == 0)
                SHDeleteKey(hkeyRoot, pr.lpSubKey);
        }
    }
}


typedef struct _UPARSEDIR
{
    LPSTR lpPath;
} UPARSEDIR;


//  UninstallDir
//      Deletes an entire directory.

VOID
UninstallDir(
    LPSTR& lp,
    HWND hwnd
    )
{
    UPARSEDIR pd;

    // Parse the information
    ::ParseLine(lp, (LPSTR*)&pd, sizeof (UPARSEDIR) / sizeof (LPSTR), NULL);

    // Display the info
    // Display status
    TCHAR sz[MAX_PATH];
    LoadString(g_hinst, IDS_DELETEFILEENTRY, sz, MAX_PATH);
    TCHAR szStatus[MAX_PATH];
    sprintf(szStatus, sz, pd.lpPath);
    SetWindowText(hwnd, szStatus);

    // See if the file matches the one we want (or is older)
    DeleteDirectory(pd.lpPath);
}


//  DeleteEmptyDirectory
//      Recursively deletes subdirectories. Never deletes files, only
//      empty subdirectories

VOID
DeleteEmptyDirectory(
    LPCTSTR lpDir
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;
    TCHAR szPath[MAX_PATH];

    // Create the find path with a \*.* for the search
    strcpy(szPath, lpDir);
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
                strcpy(szPath, lpDir);
                ::AppendSlash(szPath);
                strcat(szPath, find.cFileName);
                DeleteEmptyDirectory(szPath);
            }
        }
        while (FindNextFile(hfind, &find));
        FindClose(hfind);
    }

    // Nuke the directory
    RemoveDirectory(lpDir);
}


//  DeleteDirectory
//      Deletes a single directory and all files in it.

VOID
DeleteDirectory(
    LPCTSTR lpDir
    )
{
    HANDLE hfind;
    WIN32_FIND_DATA find;
    TCHAR szPath[MAX_PATH];

    // Create the find path with a \*.* for the search
    strcpy(szPath, lpDir);
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

            // Skip any directories we might find
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }

            strcpy(szPath, lpDir);
            ::AppendSlash(szPath);
            strcat(szPath, find.cFileName);
            DeleteFile(szPath);
        }
        while (FindNextFile(hfind, &find));
        FindClose(hfind);
    }

    // Nuke the directory
    RemoveDirectory(lpDir);
}