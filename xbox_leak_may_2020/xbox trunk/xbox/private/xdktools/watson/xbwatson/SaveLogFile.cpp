// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     SaveLogFile.cpp
// Contents: Contains code to write out the current log file to a file.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DumpLog
// Purpose:   Saves the crash information to a user-specified file.  Opens the common file-save
//            dialog to query the user for the desired file name\path.
//            NOTE: This saves crash info, NOT the log window contents.
// Arguments: hwndWindow        -- Handle to the app's window.  Necessary for the common dialog.
//            dwEventType       -- The event that occurred immediately before the dump (RIP, etc).
//            dwThreadId        -- Thread that the crash occurred in.
// Return:    'true' if the log was successfully saved; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool DumpLog(HWND hwndWindow, DWORD dwEventType, DWORD dwThreadId)
{
    sBreakInfo   breakinfo;
    OPENFILENAME ofn;
    static TCHAR szTitleName[MAX_PATH], szFileName[MAX_PATH];
    static TCHAR szFilter[] = TEXT("Log Files (*.LOG)\0*.log\0") \
                              TEXT("All Files (*.*)\0*.*\0\0");
    
    // Grab a slew of exception-related information
    if (!GetBreakInfo(dwThreadId, dwEventType, &breakinfo))
    {
        MessageBox(NULL, "Failed to obtain crash information from application.  Log will not be saved",
                   "Unable to get crash information", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Initialize the common save-file dialog box structure
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize    = sizeof(OPENFILENAME);
    ofn.lpstrFilter    = szFilter;
    ofn.nMaxFile       = MAX_PATH;
    ofn.nMaxFileTitle  = MAX_PATH;
    ofn.lpstrDefExt    = TEXT("log");
    ofn.hwndOwner      = hwndWindow;
    ofn.lpstrFile      = szFileName;
    ofn.lpstrFileTitle = szTitleName;
    ofn.Flags          = OFN_OVERWRITEPROMPT;

    // Pop up the common save-file dialog box.
    // UNDONE-FEATURE: Might be nice to track and reuse last-known-good filename for ease of saving...
    if (!GetSaveFileName(&ofn))
        return false;

    HANDLE hfile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        // Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_ALREADY_EXISTS:
            // Already prompted (automatically) for this in the GetSaveFileName dialog.  Fall
            // out of error handling
            break;

        case ERROR_DISK_FULL:
            MessageBox(NULL, "The specified drive is full.  Please free some space on it or select another drive, and try again.",
                       "Insufficient disk space", MB_ICONWARNING | MB_OK);
            return false;

        case ERROR_ACCESS_DENIED:
            MessageBox(NULL, "The specified file cannot be overwritten.  Please ensure the file isn't in use and try again.",
                       "Cannot overwrite file", MB_ICONWARNING | MB_OK);
            return false;

        default:
            MessageBox(NULL, "xbWatson cannot save the log to the specified location.  Please check the device and try again.",
                       "Cannot save file", MB_ICONWARNING | MB_OK);
        }
    }

    // Write the break information to the file.
    WriteBreakInfo(hfile, &breakinfo);

    // File successfully written
    CloseHandle(hfile);

    // Free up memory allocated for the BreakInfo structure
    FreeBreakInfo(&breakinfo);

    // return that the file was successfully saved.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SaveLogFile
// Purpose:   Saves the contents of the specified edit control to a file.  Opens the common file-
//            save dialog to query the user for the desired file name\path.
//            NOTE: This saves the log window contents, NOT crash info.
// Arguments: hwndWindow        -- Handle to the app's window.  Necessary for the common dialog.
//            hwndEditControl   -- Handle of the edit control whose text we're going to write out.
// Return:    'true' if successfully written; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SaveLogFile(HWND hwndWindow, HWND hwndEditControl)
{
    OPENFILENAME ofn;
    DWORD dwWritten;
    static TCHAR szTitleName[MAX_PATH], szFileName[MAX_PATH];
    static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0") \
                              TEXT("All Files (*.*)\0*.*\0\0");

    char szBuffer[65536];

    // Initialize the common save-file dialog box structure
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize    = sizeof(OPENFILENAME);
    ofn.lpstrFilter    = szFilter;
    ofn.nMaxFile       = MAX_PATH;
    ofn.nMaxFileTitle  = MAX_PATH;
    ofn.lpstrDefExt    = TEXT("txt");
    ofn.hwndOwner      = hwndWindow;
    ofn.lpstrFile      = szFileName;
    ofn.lpstrFileTitle = szTitleName;
    ofn.Flags          = OFN_OVERWRITEPROMPT;

    // Pop up the common save-file dialog box.
    // UNDONE-FEATURE: Might be nice to track and re-use last-known-good filename for ease of saving...
    if (!GetSaveFileName(&ofn))
        return false;

    HANDLE hfile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        // Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_ALREADY_EXISTS:
            // Already prompted (automatically) for this in the GetSaveFileName dialog.  Fall
            // out of error handling
            break;

        case ERROR_DISK_FULL:
            MessageBox(NULL, "The specified drive is full.  Please free some space on it or select another drive, and try again.",
                       "Insufficient disk space", MB_ICONWARNING | MB_OK);
            return false;

        case ERROR_ACCESS_DENIED:
            MessageBox(NULL, "The specified file cannot be overwritten.  Please ensure the file isn't in use and try again.",
                       "Cannot overwrite file", MB_ICONWARNING | MB_OK);
            return false;

        default:
            MessageBox(NULL, "xbWatson cannot save the log to the specified location.  Please check the device and try again.",
                       "Cannot save file", MB_ICONWARNING | MB_OK);
            return false;
        }
    }

    // Get the log file from the edit control
    SendMessage(hwndEditControl, WM_GETTEXT, (WPARAM)65535, (LPARAM)szBuffer);

    // Write the log file to the newly opened file
    if (WriteFile(hfile, szBuffer, strlen(szBuffer), &dwWritten, NULL) == 0)
    {
        // An error occurred.  Figure out why we died
        CloseHandle(hfile);
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_DISK_FULL:
            MessageBox(NULL, "The specified drive is full.  Please free some space on it or select another drive, and try again.", "Insufficient disk space", MB_ICONWARNING | MB_OK);
            return false;

        default:
            MessageBox(NULL, "xbWatson cannot save the log to the specified location.  Please check the device and try again.", "Cannot save file", MB_ICONWARNING | MB_OK);
            return false;
        }
    }

    // File successfully written
    CloseHandle(hfile);

    // Return that we successfully wrote the file
    return true;
}
