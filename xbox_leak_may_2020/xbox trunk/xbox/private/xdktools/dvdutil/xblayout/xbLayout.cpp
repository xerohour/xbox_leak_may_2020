// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      xbLayout.cpp
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WinMain
// Purpose:   This is the default entry point into a Windows application.  When
//            the application is started, Windows calls into this function -
//            when this function exits, the application ends.  This function
//            creates the Win32 class that represents this application, sets up
//            the Win32 Window in which the shell will be rendered, and then
//            goes into the Win32 message loop that drives the application.
// Arguments: hinst         -- The handle to this instance of the application
//            hinstPrev     -- Obsolete; throwback to Win16 -- ignore
//            szCmdLine     -- Contains the parameter list passed to this app
//            nCmdShow      -- Default visibility of the application
// Return:    Value app returns to the OS specifying result of execution.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR szCmdLine,
                     int nCmdShow)
{
    InitProfile();

    // Ensure that the common control DLL is loaded. 
    InitCommonControls(); 

    // Create our Window
    CWindow_Main *pwindow = new CWindow_Main(FLAG_WINDOW_POS_LASTKNOWN, hinst);
    if (!pwindow)
        FatalError(E_OUTOFMEMORY);
    if (!pwindow->Init())
    {
        // Failed to initialize the window for some reason.  The user has
        // already been informed of the problem, so just return the error code
        return -1;
    }
    
    // Run the window.  Control remains within this function until the user
    // exits the app
    int nExitVal = pwindow->Run();

    DumpProfile();
    
    // At this point, the app has finished shutting down.
    delete pwindow;

    return nExitVal;
}
