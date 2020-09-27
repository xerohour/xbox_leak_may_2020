// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     InitDM.cpp
// Contents: Xbox Debug Monitor initialization and uninitialization code.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_hthreadConnect -- Handle to the thread that takes care of connecting to the Xbox
HANDLE gs_hthreadConnect;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_pdmsession     -- Current Debug Monitor Session.
static PDMN_SESSION gs_pdmsession = NULL;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// UNDONE-FEATURE: Should user be able to specify which Xbox to care about?

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InitDM
// Purpose:   Function that initializes the various Xbox DebugMonitor (DM) connections and callbacks
// Arguments: None
// Returns:   true on success, false on failure
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT HrInitDM(void)
{
    HRESULT hr;
    bool fConnected = false;
    int idm;
    static int rgdm[] = { DM_DEBUGSTR, DM_RIP, DM_ASSERT, DM_EXEC, DM_BREAK,
        DM_EXCEPTION, DM_DATABREAK, DM_NONE };

    // Determine to which Xbox we're talking.
    DWORD dw = MAX_PATH;
    DmGetXboxName(g_szTargetXbox, &dw);

    // Inform XBDM that we need to be notified of events.  Ask that the
    // notification be set up asynchronously, so that we don't block while
    // connection is attempted.
    DmUseSharedConnection(TRUE);
    hr = DmOpenNotificationSession(DM_DEBUGSESSION | DM_ASYNCSESSION,
        &gs_pdmsession);
    if(FAILED(hr))
        gs_pdmsession = NULL;

    // Setup the notification callbacks.  'NotifyProc' will automatically be called whenever the
    // specified events are signaled.
    for(idm = 0; rgdm[idm] != DM_NONE; ++idm) {
        if(SUCCEEDED(hr))
            hr = DmNotify(gs_pdmsession, rgdm[idm], NotifyProc);
    }

    if(FAILED(hr)) {
        // Something went wrong - clean up after ourselves and return 'failure'.

        // Inform XBDM that we're done being notified
        if (gs_pdmsession) {
            DmCloseNotificationSession(gs_pdmsession);
            gs_pdmsession = NULL;
        }

        // We're done with the shared connection
        DmUseSharedConnection(FALSE);
    }

    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UninitDM
// Purpose:   Called to clean up after ourselves.  Shuts down the XBox Debug Monitor (DM) connection
//            and registered notification callbacks.
// Arguments: None
// Returns:   None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UninitDM()
{
    // Inform XBDM that we're done being notified
    if (gs_pdmsession) {
        // inform XBDM that we're done being notified
        DmCloseNotificationSession(gs_pdmsession);
        gs_pdmsession = NULL;
    }

    // We're done with the shared connection
    DmUseSharedConnection(FALSE);
}
