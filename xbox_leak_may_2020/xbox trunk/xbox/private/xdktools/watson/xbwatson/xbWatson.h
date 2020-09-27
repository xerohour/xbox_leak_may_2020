// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbWatson.h
// Contents: Contains declarations of functions and variables defined in xbWatson.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_hwndEdit       -- Edit control that contains the current log dump
extern HWND g_hwndEdit;

// g_hwnd           -- The app's main window
extern HWND g_hwnd;

// g_hInstance      -- Handle to this instance of xbWatson.
extern HINSTANCE g_hInstance;

// g_szTargetXbox   -- Name of the Xbox that we're watching.
extern char g_szTargetXbox[MAX_PATH];
