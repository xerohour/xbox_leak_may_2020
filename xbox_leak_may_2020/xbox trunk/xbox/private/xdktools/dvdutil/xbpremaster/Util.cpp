// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      util.cpp
// Contents:  Simple utilities for the xbPremaster application.
// Revisions: 31-May-2001: Created (jeffsim)
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
// Function:  ReportError
// Purpose:   Display appropriate error message to user
// Arguments: error         -- The error message to report
// Return:    None - the application will exit at the end of the function.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ReportError(eError error)
{
    switch(error)
    {
    case ERROR_INIT:
        MessageBox(NULL, "The application failed to properly initialize.  Please restart your" \
                         " computer and try again.", "Failed to initialize",
                         MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION);
        break;
        
    case ERROR_OUTOFMEM:
        MessageBox(NULL, "Not enough memory to continue the operation.  xbPremaster will now exit",
                   "Out of memory", MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION);
        break;
        
    default:
    case ERROR_UNEXPECTED:
        MessageBox(NULL, "An unexpected error has occurred in the application.  Please restart it" \
                         " and try again.", "Unexpected error occurred", MB_OK | MB_TASKMODAL |
                         MB_ICONEXCLAMATION);
        break;
    }    
    exit(-1);
}
