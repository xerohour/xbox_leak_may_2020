// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     
// Contents: 
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
extern bool g_fForceWaitCursor;
HANDLE g_hA, g_hB;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConnectThread
// Purpose:   Thread that handles connecting to the user's Xbox.
// Arguments: pvUnused          -- Ignore.
// Returns:   '0' if successfully connected; '-1' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI ConnectThread(LPVOID pvArg)
{
    while (true)
    {
        WaitForSingleObject(g_hA, INFINITE);
        if (WaitForSingleObject(g_hB, 2000) == WAIT_TIMEOUT)
            SetCursor(LoadCursor(NULL, IDC_WAIT));
    }

    return 0;
}

CXboxConnection::CXboxConnection()
{
    g_hA = CreateEvent(NULL, false, false, NULL);
    g_hB = CreateEvent(NULL, true, false, NULL);

    // Create the connection thread. 
    m_hthreadConnect = CreateThread(NULL, 0, ConnectThread, this, 0, NULL);
}

CXboxConnection::~CXboxConnection()
{
    CloseHandle(g_hA);
    CloseHandle(g_hB);
    
    // Close off our Connect thread
    CloseHandle(m_hthreadConnect);
}

void CXboxConnection::Before()
{
    DmUseSharedConnection(TRUE);
    ResetEvent(g_hB);
    SetEvent(g_hA);
}

void CXboxConnection::After()
{
    DmUseSharedConnection(FALSE);
    SetEvent(g_hB);
    if (!g_fForceWaitCursor)
        SetCursor(LoadCursor(NULL, IDC_ARROW));
}

