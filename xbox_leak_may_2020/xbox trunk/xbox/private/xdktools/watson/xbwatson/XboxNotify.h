// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     XboxNotify.h
// Contents: Contains declarations of functions and variables defined in XboxNotify.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_fWriteException    -- If an access violation occurred, 'true' here means it was a write
//                         violation; 'false' means a read violation occurred.
extern bool g_fWriteException;

// g_pvAVAddress        -- If an access violation occurred, this contains the address that was
//                         incorrectly read or written.
extern void *g_pvAVAddress;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  NotifyProc
// Purpose:   This is a callback routine registered by DmNotify.  It is called by XboxDbg for
//            various types of notification event
// Arguments: Notification      -- Type of notification, DM_DEBUGSTR for example
//            Parameter         -- Optional parameter based on notification type
// Returns:   Always zero
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI NotifyProc(DWORD dwNotification, DWORD dwParameter);
