// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     SaveLogFile.h
// Contents: Contains declarations of functions and variables defined in SaveLogFile.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SaveLogFile
// Purpose:   Saves the contents of the specified edit control to a file.  Opens the common file-
//            save dialog to query the user for the desired file name\path.
// Arguments: hwndWindow        -- Handle to the app's window.  Necessary for the common dialog.
//            hwndEditControl   -- Handle of the edit control whose text we're going to write out.
// Return:    'true' if successfully written; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SaveLogFile(HWND hwndWindow, HWND hwndEditControl);

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
bool DumpLog(HWND hwndWindow, DWORD dwEventType, DWORD dwThreadId);
