// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbWatson.h
// Contents: Contains declarations of functions and variables defined in xbWatson.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ENUMERATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// eMode            -- Contains the various modes that this application can run in.
typedef enum eMode
{
    // MODE_LOGGING -- In this mode, xbWatson displays a log view window, allows the user to save the
    //                 log, and displays Xbox assert messages.
    MODE_LOGGING,

    // MODE_STACKDUMP -- In this mode, xbWatson parses a previously created log file and displays
    //                   the callstack (with function names where possible)
    MODE_STACKDUMP,
};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_hwndEdit       -- Edit control that contains the current log dump
extern HWND g_hwndEdit;

// g_hwnd           -- The app's main window
extern HWND g_hwnd;

// g_hInstance      -- Handle to this instance of xbWatson.
extern HINSTANCE g_hInstance;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  OutputMsg
// Purpose:   Utility function - dumps the specified message to the command prompt.  This will
//            only work if launched from a command prompt.
// Arguments: szMsg         -- The format of the message (just like 'printf').
//            ...           -- Arguments for the message.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void OutputMsg(char *szMsg, ...);
