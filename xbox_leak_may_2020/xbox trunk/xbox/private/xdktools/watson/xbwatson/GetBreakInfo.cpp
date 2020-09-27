// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     GetBreakInfo.cpp
// Contents: Contains code to request information about the connected Xbox.
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
// Function:  GetStackBaseAndSize
// Purpose:   Obtain the stack base and stack size of the specified thread.
// Arguments: pdwStackBase      -- Pointer to buffer to contain the base of the stack
//            pdwStackSize      -- Pointer to buffer to contain the size of the stack
//            dwThreadId        -- Id of the thread on which we'll get information.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool GetStackBaseAndSize(DWORD *pdwStackBase, DWORD *pdwStackSize, DWORD dwThreadId)
{
    //  The Xbox stack looks (roughly) something like this:
    //      _________
    //      |       |   <-- NPX save (kernel stack information - we don't care about it).
    //      +-------+   <-- Stack Base
    //      |       |
    //      |       |   <-- Thread Local Storage (TLS)
    //      |       |
    //      +-------+   <-- TlsBase
    //      |       |
    //      |       |   <-- Stack
    //      |       |   
    //      +-------+   <-- ESP
    //      |///////|
    //      |///////|   <-- Not yet used memory
    //      |       |
    //
    //  We can obtain the 'TlsBase' value from the Xbox via DmGetThreadInfo.
    //  On the Xbox, we actually don't need the TLS information, so we can essentially collapse
    //  the stack so that StackBase == TlsBase (so we have our stackbase; 'TlsBase').
    //  From there, calculating the size of the stack we care about is simply TlsBase - ESP.

    // dmti             -- Will contain information about the specified thread.
    DM_THREADINFO dmti;

    // cr               -- Will contain the context information for the specified thread.
    CONTEXT cr;

    // Request the thread information from the Xbox
    if (DmGetThreadInfo(dwThreadId, &dmti) != XBDM_NOERR)
    {
        // Failed to communicate with the Xbox. Jump ship.
        return false;
    }

    // NOTE: It's possible (though very unlikely) that we'll get a TLSData value of '0'.  In that
    //       case it's okay to completely ignore the thread.
    if (dmti.TlsBase == 0)
        return false;
    
    // Get the current stack pointer (ESP).  UNDONE-WARN: Don't need all of these flags (elsewhere too)
    cr.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL | CONTEXT_FULL | CONTEXT_FLOATING_POINT |
                      CONTEXT_EXTENDED_REGISTERS;
    if (DmGetThreadContext(dwThreadId, &cr) != XBDM_NOERR)
    {
        // Failed to get Esp - can't determine stack size.
        return false;
    }

    // Set the stack base and stack size values as described above.
    //*pdwStackBase = (DWORD)dmti.TlsBase;
    //*pdwStackSize = *pdwStackBase - cr.Esp;

    // Note: to ease lookup, we point the base of the stack at esp rather than tlsbase; this allows
    //       easier address translation on the flip side.
    *pdwStackBase = cr.Esp;
    *pdwStackSize = (DWORD)dmti.TlsBase - *pdwStackBase;

    // Return that we successfully obtained the stack base and stack size.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetPerThreadBreakInfo
// Purpose:   Obtains thread-specific information (callstacks, context, etc) using the specified
//            thread id.
// Arguments: dwThreadId        -- Id of the thread on which we'll get information.
//            pthreadinfo       -- ThreadInfo structure that we'll fill in here.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool GetPerThreadBreakInfo(DWORD dwThreadId, sThreadInfo *pthreadinfo)
{
    // cBytesRead       -- Number of bytes read in the DmGetMemory call
    DWORD cBytesRead;

    // cr               -- Will contain the context information for the specified thread.
    CONTEXT cr;


    // Get the current stack pointer
    cr.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL | CONTEXT_FULL | CONTEXT_FLOATING_POINT |
                      CONTEXT_EXTENDED_REGISTERS;
    if (DmGetThreadContext(dwThreadId, &cr) != XBDM_NOERR)
    {
        // Failed to get Esp - can't read per-thread info
        pthreadinfo->fValid = false;
        return false;
    }

    // Store the entire context of the thread in the threadinfo structure
    pthreadinfo->cr = cr;
    pthreadinfo->dwThreadId = dwThreadId;

    // Determine the base and the size of the stack for the specified thread.
    GetStackBaseAndSize(&pthreadinfo->dwStackBase, &pthreadinfo->dwStackSize, dwThreadId);

    // Allocate space for the stack
    pthreadinfo->rgbyStack = new BYTE[pthreadinfo->dwStackSize];
    if (!pthreadinfo->rgbyStack)
    {
        // Out of memory.  Jump Ship.
        return false;
    }

    // Get the entire stack from the Xbox
    if (DmGetMemory((BYTE*)cr.Esp, pthreadinfo->dwStackSize, pthreadinfo->rgbyStack, &cBytesRead) != XBDM_NOERR)
    {
        // Failed to read for some reason.  Jump ship.
        return false;
    }

    // Mark that we successfully got the thread information
    pthreadinfo->fValid = true;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetBreakInfo
// Purpose:   Requests a slew of Access Violation-related information from the Xbox that reported
//            an error.
// Arguments: dwThreadId        -- Id of the thread on which we'll get information.
//            dwEventType       -- The event that occurred immediately before the dump (RIP, etc).
//            pbreakinfo        -- The breakinfo structure we'll fill in here.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetBreakInfo(DWORD dwThreadId, DWORD dwEventType, sBreakInfo *pbreakinfo)
{
    // rgdwThreads      -- Contains the list of threads currently running on the Xbox.
    DWORD rgdwThreads[MAX_XBOX_THREADS];

    PDM_WALK_MODULES pdmwm;
    DMN_MODLOAD      dmnml;

    // Note: Even though the connected Xbox has crashed, we're able to talk to it because of the
    //       'kd disable' trick we pulled earlier - it's sitting in a loop waiting for someone to
    //       talk to it.  We use this to query for the necessary exception-related information.
    
    // Store information about the type of break which occurred
    pbreakinfo->dwEventType      = dwEventType;
    pbreakinfo->dwBrokenThreadId = dwThreadId;
    if (dwEventType == IDD_RIP)
    {
        // The event was a RIP.  Store the string that the Xbox reported
        strcpy(pbreakinfo->szRIP, g_pszRIP);
    }
    else
    {
        // The event was an exception.  Store information about the exception.
        pbreakinfo->fWriteException = g_fWriteException;
        pbreakinfo->dwAVAddress     = (DWORD)g_pvAVAddress;
        pbreakinfo->dwEventCode     = g_dwExceptionCode;
    }
    
    // Get information about the application that AV'ed.
    DM_XBE dmxbe;
    if (DmGetXbeInfo(NULL, &dmxbe) != XBDM_NOERR)
    {
        // Failed that too.  Give up
        return false;
    }
    strcpy(pbreakinfo->szAppName, dmxbe.LaunchPath);

    // Get the time of the break (roughly).
    DmGetSystemTime(&pbreakinfo->systime);

    // Get the name of the Xbox.
    char szXboxName[1024];
    DWORD dw = 1024;
    DmGetXboxName(szXboxName, &dw);
    strcpy(pbreakinfo->szXboxName, szXboxName);

    // Get the list of modules on the Xbox.  Go through it once to determine the number of
    // modules; then allocate enough space to hold them and go through again.
    pbreakinfo->cModules = 0;
    pdmwm = NULL;
    while (DmWalkLoadedModules(&pdmwm, &dmnml) == XBDM_NOERR)
        pbreakinfo->cModules++;

    // Allocate space for the modules
    pbreakinfo->prgdmnml = new DMN_MODLOAD[pbreakinfo->cModules];
    
    // Do the module walk again now that we've allocated sufficient space for it
    pdmwm = NULL;
    for (DWORD i = 0; i < pbreakinfo->cModules; i++)
    {
        DmWalkLoadedModules(&pdmwm, &pbreakinfo->prgdmnml[i]);

        // If the loaded module == the user's application, then get the BaseAddress of the first
        // section.  In the .log parsing code, we will compare this section offset with the
        // unrelocated section offset in order to obtain the correct base address for the entire
        // module...

        // Note: Because the XBE is stored as 'path\app.xbe' and the module name is stored
        //       as 'app.exe', a bit of code is necessary here to obtain the undecorated module
        //       and file name for correct comparison.
        char szAppName[MAX_PATH];
        char szModuleName[MAX_PATH];
        strcpy(szAppName, strrchr(pbreakinfo->szAppName, '\\') + 1);
        *(strrchr(szAppName, '.')) = '\0';
        strcpy(szModuleName, pbreakinfo->prgdmnml[i].Name);
        *(strrchr(szModuleName, '.')) = '\0';
        if (!lstrcmpi(szAppName, szModuleName))
        {
            // This is the user's application module
            DMN_SECTIONLOAD dmnsl;
            PDM_WALK_MODSECT pdmwms = NULL;

            // We only need the first section, so call WalkModuleSections only once.
            DmWalkModuleSections(&pdmwms, pbreakinfo->prgdmnml[i].Name, &dmnsl);

            // Store the first section's base address in the breakinfo structure so that we can
            // write it out later.
            pbreakinfo->dwFirstSectionBase = (DWORD)dmnsl.BaseAddress;
        }
    }

    // State dump for each thread.

    // DmGetThreadList needs the upper bound the array it's passed, so specify it here.  After the
    // call to DmGetThreadList returns, 'pbreakinfo->cThreads' will contain the *actual* number of elements.
    pbreakinfo->cThreads = MAX_XBOX_THREADS;

    // Get the list of threads from the Xbox.
    if (DmGetThreadList(rgdwThreads, &pbreakinfo->cThreads) == XBDM_BUFFER_TOO_SMALL)
        OutputMsg("Warning: More than 100 threads running on the Xbox -- only the first 100 will be stored.\r\n");

    // Allocate space in the breakinfo structure to hold the per-thread information
    pbreakinfo->prgthreadinfo = new sThreadInfo[pbreakinfo->cThreads];

    // Output information on each thread currently running.
    for (i = 0; i < pbreakinfo->cThreads; i++)
        if (!GetPerThreadBreakInfo(rgdwThreads[i], &pbreakinfo->prgthreadinfo[i]))
        {
            // Failed to get the per-thread info.  Warn the user but continue on with the next thread.
            OutputMsg("Warning: Failed to get stack for thread '%d'.  Thread skipped.\r\n", dwThreadId);
        }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FreeBreakInfo
// Purpose:   Frees space allocated for the specified BreakInfo's internals
// Arguments: pbreakinfo        -- The breakinfo structure we'll free up here.
// Returns:   None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FreeBreakInfo(sBreakInfo *pbreakinfo)
{
    // Free up space allocated for the modules
    delete pbreakinfo->prgdmnml;

    // Free up the per-thread space
    for (DWORD i = 0; i < pbreakinfo->cThreads; i++)
        delete pbreakinfo->prgthreadinfo->rgbyStack;

    // Free up the list of threads
    delete pbreakinfo->prgthreadinfo;
}
