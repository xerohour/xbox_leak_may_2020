// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     GetBreakInfo.h
// Contents: Contains declarations of functions and variables defined in GetBreakInfo.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_XBOX_THREADS -- Maximum number of threads expected on the Xbox.
#define MAX_XBOX_THREADS 100


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sThreadInfo      -- Contains information about a particular thread that was running at time of
//                     crash.
typedef struct
{
    bool  fValid;                           // 'true' if the thread's stack was successfully read.
    DWORD dwThreadId;                       // Id of the thread.
    DWORD dwStackBase;                      // Loaded address of the first element in the stack.
    DWORD dwStackSize;                      // Size of the stack we're keeping around.
    BYTE  *rgbyStack;                       // Pointer to the stack data.
    CONTEXT cr;                             // Complete context at time of crash.
} sThreadInfo;

// sBreakInfo       -- Encapsulates all information tracked at time of crash.
typedef struct
{
    DWORD           dwEventType;            // The type of break event that occurred.
    DWORD           dwBrokenThreadId;       // ID of the thread in which the break occurred.
    DWORD           dwEventCode;            // Exception code of the break event if not a RIP.
    bool            fWriteException;        // 'true' = write AV.  'False' = read AV.
    DWORD           dwAVAddress;            // address of access violation (if that's what it was).
    char            szRIP[1024];            // RIP string.  Empty if the event wasn't a RIP.
    char            szXboxName[256];        // Name of the Xbox that the break occurred on.
    SYSTEMTIME      systime;                // Time the break occurred.
    char            szAppName[MAX_PATH];    // Name of the user's module.
    DWORD           cModules;               // Number of modules loaded.
    DMN_MODLOAD     *prgdmnml;              // Module information.
    DWORD           cThreads;               // Number of threads active in this break event.
    sThreadInfo     *prgthreadinfo;         // Per-thread break information.
    DWORD           dwFirstSectionBase;     // The LOADED base address of the first section.  Used
                                            // to determine offset from unrelocated address.
} sBreakInfo;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetBreakInfo
// Purpose:   Requests a slew of Access Violation-related information from the Xbox that reported
//            an error.
// Arguments: dwThreadId        -- Id of the thread on which we'll get information.
//            dwEventType       -- The event that occurred immediately before the dump (RIP, etc).
//            pbreakinfo        -- The breakinfo structure we'll fill in here.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetBreakInfo(DWORD dwThreadId, DWORD dwEventType, sBreakInfo *pbreakinfo);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FreeBreakInfo
// Purpose:   Frees space allocated for the specified BreakInfo's internals
// Arguments: pbreakinfo        -- The breakinfo structure we'll free up here.
// Returns:   None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FreeBreakInfo(sBreakInfo *pbreakinfo);
