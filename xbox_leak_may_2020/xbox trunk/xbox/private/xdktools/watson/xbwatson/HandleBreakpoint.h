// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     HandleBreakpoint.h
// Contents: Contains declarations of functions and variables defined in HandleBreakpoint.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleBreakpoint
// Purpose:   Called when we've been notified of an Xbox Breakpoint and we need to handle it.  Pops
//            up the Breakpoint dialog box and lets it take control.
// Arguments: dwThreadId        -- Id of the thread in which the Breakpoint occurred.
//            pvAddress         -- Address at which the exception occurred.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleBreakpoint(DWORD dwThreadId, void *pvAddress);
