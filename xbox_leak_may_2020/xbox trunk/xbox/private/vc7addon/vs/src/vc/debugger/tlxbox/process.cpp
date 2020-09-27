/*++

Copyright (c) Microsoft Corporation

Module Name:

    process.cpp

Abstract:

    Implementation of debugger routines related to launching and destroying processes on Xbox.
    
Author:

    mitchd

--*/
#include <precomp.h>

//-------------------------------------------------------------------------------------------------------------
// Default Exception Table:  I am not 100% sure where to put it.  It is need by CXboxProcess, but really
//                           feels like it should be separate.  We will put here has a static global, for now.
//-------------------------------------------------------------------------------------------------------------
// BUGBUG - The names of the default exceptions should arguably be localizable and stored in a resource.
//          They are hard-coded for now.  We could in the future restructure the table store to store resource
//          name and look them when needed.
#define efdDefault efdNotify
static EXCEPTION_DESCRIPTION g_DefaultExceptionTable[] = 
{
    // DBG_CONTROL_C and DBG_CONTROL_BREAK are *only*
    // raised if the app is being debugged.  The system
    // remotely creates a thread in the debuggee and then
    // raises one of these exceptions; the debugger must
    // respond to the first-chance exception if it wants
    // to trap it at all, because it will never see a
    // last-chance notification.
    {(DWORD)DBG_CONTROL_C,                    efdStop,    "Control-C"},
    {(DWORD)DBG_CONTROL_BREAK,                efdStop,    "Control-Break"},
    {(DWORD)EXCEPTION_DATATYPE_MISALIGNMENT,  efdDefault, "Datatype Misalignment"},
    {(DWORD)EXCEPTION_ACCESS_VIOLATION,       efdDefault, "Access Violation"},
    {(DWORD)EXCEPTION_IN_PAGE_ERROR,          efdDefault, "In Page Error"},
    {(DWORD)STATUS_ILLEGAL_INSTRUCTION,       efdDefault, "Illegal Instruction"},
    {(DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED,  efdDefault, "Array Bounds Exceeded"},
    // Floating point exceptions will only be raised if
    // the user calls _controlfp() to turn them on.
    {(DWORD)EXCEPTION_FLT_DENORMAL_OPERAND,   efdDefault, "Float Denormal Operand"},
    {(DWORD)EXCEPTION_FLT_DIVIDE_BY_ZERO,     efdDefault, "Float Divide by Zero"},
    {(DWORD)EXCEPTION_FLT_INEXACT_RESULT,     efdDefault, "Float Inexact Result"},
    {(DWORD)EXCEPTION_FLT_INVALID_OPERATION,  efdDefault, "Float Invalid Operation"},
    {(DWORD)EXCEPTION_FLT_OVERFLOW,           efdDefault, "Float Overflow"},
    {(DWORD)EXCEPTION_FLT_STACK_CHECK,        efdDefault, "Float Stack Check"},
    {(DWORD)EXCEPTION_FLT_UNDERFLOW,          efdDefault, "Float Underflow"},
    // STATUS_NO_MEMORY can be raised by HeapAlloc and
    // HeapRealloc.
    {(DWORD)STATUS_NO_MEMORY,                 efdDefault, "No Memory"},
    // STATUS_NONCONTINUABLE_EXCEPTION is raised if a
    // noncontinuable exception happens and an exception
    // filter return -1, meaning to resume execution.
    {(DWORD)STATUS_NONCONTINUABLE_EXCEPTION,  efdDefault, "Noncontinuable Exception"},
    // STATUS_INVALID_DISPOSITION means an NT exception
    // filter (which is slightly different from an MS C
    // exception filter) returned some value other than
    // 0 or 1 to the system.
    {(DWORD)STATUS_INVALID_DISPOSITION,       efdDefault, "Invalid Disposition"},
    {(DWORD)EXCEPTION_INT_DIVIDE_BY_ZERO,     efdDefault, "Integer Divide by Zero"},
    {(DWORD)EXCEPTION_INT_OVERFLOW,           efdDefault, "Integer Overflow"},
    {(DWORD)EXCEPTION_PRIV_INSTRUCTION,       efdDefault, "Privileged Instruction"},
    {(DWORD)STATUS_STACK_OVERFLOW,            efdDefault, "Stack Overflow"},
    {(DWORD)STATUS_DLL_NOT_FOUND,             efdDefault, "DLL Not Found"},
    {(DWORD)STATUS_DLL_INIT_FAILED,           efdDefault, "DLL Initialization Failed"},
    {(DWORD)VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND),
                                              efdDefault, "Module Not Found"},
    {(DWORD)VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND),
                                              efdDefault, "Procedure Not Found"},
    {(DWORD)STATUS_INVALID_HANDLE,            efdStop,    "Invalid Handle"},
    {(DWORD)(0xE0000000 | 'msc'),             efdNotify,  "Microsoft C++ Exception"},
};
// This NULL exception is reference in a few places as a coding convenience.
static EXCEPTION_DESCRIPTION g_NullExceptionDescription = {0};

//-------------------------------------------------------------------------------------------------------------
// Declare\Initialize CXboxProcess's static members
//-------------------------------------------------------------------------------------------------------------
CXboxProcess     *CXboxProcess::sm_pProcessList = NULL;
CRITICAL_SECTION  CXboxProcess::sm_CriticalSection;

//-------------------------------------------------------------------------------------------------------------
//  Public CXboxProcess Methods
//     Implemeneted in the same in logical order as in the class declaration.
//-------------------------------------------------------------------------------------------------------------

XOSD CXboxProcess::InitializeCXboxProcessClass()
/*++
  Routine Description:
    CXboxProcess has several static members.  These are initialized here.  Called from DMInit.
  Return Value:
    Always xosdNone.
--*/
{
    _ASSERTE(NULL == sm_pProcessList);
    InitializeCriticalSection(&sm_CriticalSection);
    return xosdNone;
}

XOSD CXboxProcess::Create(HPID hPid)
/*++
  Routine Description:
    Creates a new process and inserts into the process list.
  Arguments:
    hPid - handle to process ID assigned by OSD.  If hPid is NULL, then it is not going to find a process.
  Return Value:
    error code.
--*/
{
    XOSD xosd;
    CXboxProcess *pNewProcess = new CXboxProcess(hPid);
    if(pNewProcess)
    {
        xosd = pNewProcess->InitializeAndInsertIntoProcessList();
    } else
    {
        xosd = xosdOutOfMemory;
    }
    return xosd; 
}

XOSD CXboxProcess::Destroy()
/*++
  Routine Description:
    Removes an existing process from the process list, and deletes it.
  Return Value:
    Reports xosdUnknown if the process was not in the list.
--*/
{
    XOSD xosd = xosdUnknown;
    //Find this process in the list and remove it.  The block is for the critical section
    {
        CHoldCriticalSection  manipulatingProcessList(&sm_CriticalSection);
        CXboxProcess *pPreviousProcess = NULL;
        CXboxProcess *pProcess = sm_pProcessList; 
        while(pProcess && (pProcess != this))
        {
            pPreviousProcess = pProcess;
            pProcess = pProcess->m_pNextProcess;
        }
        
        // short circuit here and return an error
        if(pProcess)
        {
            // We found the process so assume success;
            xosd = xosdNone;
            if(pPreviousProcess)
            {
                pPreviousProcess->m_pNextProcess = m_pNextProcess;
            } else
            {
                sm_pProcessList = m_pNextProcess;
            }
            m_pNextProcess = NULL;
        } else
        {
            _ASSERTE(FALSE && "Process being destroyed was not in list.");
        }
    }
    
    // whether in the list or not, delete ourselves.
    delete this;
    return xosd;
}

CXboxProcess *CXboxProcess::GetProcess(HPID hPid)
/*++
  Routine Description:
    Get a pointer to an existing CXboxProcess given an hPid.
  Arguments:
    hPid - handle to process ID assigned by OSD.  If hPid is NULL, then it is not going to find a process.
  Return Value:
    Pointer to corresponding process if found, otherwise NULL.
--*/
{
    CXboxProcess *pDesiredProcess = NULL;
    if(hPid) 
    {
        CHoldCriticalSection  searchingProcessList(&sm_CriticalSection);
        pDesiredProcess = sm_pProcessList;
        while(sm_pProcessList)
        {
            if(pDesiredProcess->m_hPid == hPid)
                break;
            pDesiredProcess = pDesiredProcess->m_pNextProcess;
        }
    }
    return pDesiredProcess;
}


XOSD CXboxProcess::ProgramLoad(LPPROGLOAD pProgamLoadInfo)
/*++
  Routine Description:
    Called in response to dmfProgLoad.  Loads or attaches to the specified process.
    This is largely responsible for the process of beginning to debug an Xbox process.
  Arguments:
    pProgramLoadInfo - information about the process to debug.
--*/
{
    HRESULT hr = S_OK;
    XOSD    xosd;
    
    // We need an initial start breakpoint event
    //
    if(!m_hStartBreakPointEvent)
    {
        m_hStartBreakPointEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if(!m_hStartBreakPointEvent)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    
    //BUGBUG:  For now always reboot the Xbox, we will need to
    //         get more subtle soon.
    if(SUCCEEDED(hr))
    {
        hr = EnsurePendingExecState();
    }

    // Set the launch title
    //
    if(SUCCEEDED(hr))
    {
        hr = SetTitle(pProgamLoadInfo);
    }

    // Set an initial break point
    //
    if(SUCCEEDED(hr))
    {
        hr = DmSetInitialBreakpoint();
    }

    if(SUCCEEDED(hr))
    {
        hr = DmStopOn(DMSTOP_CREATETHREAD, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        ResetEvent(m_hStartBreakPointEvent);
        
        // Not our normal Notification Handle, but a special one, while
        // we are launching a process.
        //
        DmNotify(m_pDmNotificationSession, DM_CREATETHREAD, CXboxProcess::LaunchNotifications);
        DmNotify(m_pDmNotificationSession, DM_BREAK, CXboxProcess::LaunchNotifications);
        DmNotify(m_pDmNotificationSession, DM_EXCEPTION, CXboxProcess::LaunchNotifications);
        
        // We have not launched the title yet.
        _ASSERTE(0==m_tidMainThread);
        DmGo();

        // Wait for initial breakpoint, but not more than WAIT_FOR_LAUNCH_TIMEOUT
        WaitForSingleObject(m_hStartBreakPointEvent, WAIT_FOR_LAUNCH_TIMEOUT);

        if(m_tidMainThread && SUCCEEDED(DmConnectDebugger(TRUE)))
        {
            //xosd = XboxConnect(CONNECT_LAUNCH);
        } else
        {
            // We will do more elegant error handling
            /*
            char szError[256];
            xosd = xosdGeneral;
            if(LoadString(hInstance, IDS_ERR_NOSTART, szError, sizeof szError))
            {
                SendDBCError(hprc, xosd, szError);
            }
            */
        }
    } else
    {
        DPRINT(5, (_T("Can't start\n")));
        /* If we can't get our startup breakpoint, we'll start the
        * machine but fail to launch */
        DmGo();
        xosd = xosdGeneral;
        // Report the error please.
    }

    // Set the box running
    //
    return xosdNone;
}


XOSD CXboxProcess::GetExceptionInfo(LPEXCMD pExceptionCommand, OUT EXCEPTION_DESCRIPTION *pExceptionDescriptionRet)
/*++
  Routine Description:
    Called to get information about an exception.  Can be called with exfFirst and then
    exfNext to enumerate all the exception and their settings, or can be called with exfSpecified
    to get information about a particular exception.
  Arguments:
    pExceptionCommand - the data sent from the EM describing which exception it wants information about.
    pExceptionDescriptionRet- pointer to buffer to return the exepction description in.
  Return Value:
    Error Status
 
--*/
{
    XOSD                           xosd = xosdEndOfStack;
    PEXCEPTION_ITEM                pExceptionItem;
    
    CHoldCriticalSection  synchronizeAccessToProcess(&sm_CriticalSection);

    //Setup the first exception item
    pExceptionItem = m_pExceptionList;

    // If the caller does not want the first exception item,
    // then find the one that the caller wants/
    if(exfFirst != pExceptionCommand->exc)
    {
        while(pExceptionItem)
        {
            // Check for a match against the exception code that the caller passed in.
            if(pExceptionItem->Exception.dwExceptionCode == pExceptionCommand->exd.dwExceptionCode)
            {
                //  If the caller said exfNext, than the caller wants the one after the
                //  one it passed in.
                if(exfNext == pExceptionCommand->exc)
                {
                    pExceptionItem = pExceptionItem->NextExceptionItem;
                } else if(exfSpecified != pExceptionCommand->exc)
                //  exfFirst, exfNext, and exfSpecified are the only support
                //  commands, make sure the caller didn't send something else.
                {
                    pExceptionItem = NULL;
                    xosd = xosdUnknown;
                }
                break;
            }
            pExceptionItem = pExceptionItem->NextExceptionItem;
        }
    }

    //  We should now have an exception item, unless some
    //  error occured (such as reaching the end of the list).
    if(pExceptionItem)
    {
        xosd = xosdNone;
        *pExceptionDescriptionRet = pExceptionItem->Exception;
    }
    
    return xosd;
}


XOSD CXboxProcess::GetDefaultExceptionInfo(LPEXCMD pExceptionCommand, OUT EXCEPTION_DESCRIPTION *pExceptionDescriptionRet)
/*++
  Routine Description:
    See GetExceptionInfo, the only difference is that it queries the global defaults rather
    than the possibly modified list associated with a particular process.
--*/
{
    int index;
    XOSD                           xosd = xosdEndOfStack;

    // Find the correct index
    index=0;
    if(exfFirst != pExceptionCommand->exc)
    {
        for(; index < ARRAYSIZE(g_DefaultExceptionTable); index++)
        {
            // Check for a match against the exception code that the caller passed in.
            if(g_DefaultExceptionTable[index].dwExceptionCode == pExceptionCommand->exd.dwExceptionCode)
            {
                //  If the caller said exfNext, than the caller wants the one after the
                //  one it passed in.
                if(exfNext == pExceptionCommand->exc)
                {
                    index++;
                } else if( exfSpecified != pExceptionCommand->exc )
                {
                    index = ARRAYSIZE(g_DefaultExceptionTable);
                    xosd = xosdUnknown;
                }
                break;
            }
        }
    }

    //  If index is less than ARRAYSIZE(DefaultExceptionTable), then we
    //  found a matching index.
    if(ARRAYSIZE(g_DefaultExceptionTable) > index)
    {
        xosd = xosdNone;
        *pExceptionDescriptionRet = g_DefaultExceptionTable[index];
    }
    
    return xosd;
}


//-------------------------------------------------------------------------------------------------------------
//  Internal CXboxProcess Methods
//     Implemeneted in the same in logical order as in the class declaration.
//-------------------------------------------------------------------------------------------------------------

// CXboxProcess::CXboxProcess() implementation is in header file
CXboxProcess::~CXboxProcess()
/*++
  Routine Description:
    d'tor for a process.  Needs no synchronization, as the process has been removed
    from the process list, and noone else should have a reference to it.
--*/
{
    //Destroy our exception list,
    while(m_pExceptionList)
    {
        EXCEPTION_ITEM *pDoomedItem = m_pExceptionList;
        m_pExceptionList = pDoomedItem->NextExceptionItem;
        delete pDoomedItem;
    }
}

XOSD CXboxProcess::InitializeAndInsertIntoProcessList()
/*++
  Routine Description:
    Initialize a CXboxProcess and put it into the process list.
  Return Value:
    Error Status.
  
--*/
{
    int index;
    XOSD xosd = xosdNone;

    //  Initialize our exception list from the DefaultExceptionTable
    m_pExceptionList = NULL;
    for(index=0; index < ARRAYSIZE(g_DefaultExceptionTable); index++)
    {
        EXCEPTION_ITEM *pNewItem = new (EXCEPTION_ITEM);
        if(pNewItem)
        {
            pNewItem->Exception = g_DefaultExceptionTable[index];
            pNewItem->NextExceptionItem = m_pExceptionList;
            m_pExceptionList = pNewItem;
        } else
        {
            xosd = xosdOutOfMemory;
            break;
        }
    }
    
    
    //*** Do this only on success, and put all other initialization
    //*** first!!!!!!!
    if(xosdNone == xosd)
    {
        //Insert process into process list
        CHoldCriticalSection  manipulatingProcessList(&sm_CriticalSection);

        // If we are not multi-Xbox Debugging, then there better be only one process at a time.
        #ifndef MULTI_XBOX_DEBUGGING
        _ASSERTE(NULL == sm_pProcessList);
        #endif

        m_pNextProcess = sm_pProcessList;
        sm_pProcessList = this;
    }
    
    return  xosd;
}


HRESULT CXboxProcess::EnsurePendingExecState()
{
    HRESULT hr = S_OK;
    HANDLE hRebootEvent;
    if(DMN_EXEC_PENDING != m_dwExecState)
    {
        hr = EnsureOpenNotificationSession();
        if(SUCCEEDED(hr))
        {
            // There shouldn't be a reboot event yet.
            _ASSERTE(!m_hRebootEvent);
            
            // Create a reboot event (our standard handler will set this when reboot completes)
            hRebootEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            if(!hRebootEvent)
            {
            hr = HRESULT_FROM_WIN32(GetLastError());
            }
            else
            {
                m_hRebootEvent = hRebootEvent;

                // Make sure EXEC state changes are being notified.
                hr = DmNotify(m_pDmNotificationSession, DM_EXEC, CXboxProcess::LaunchNotifications);
  
                if(SUCCEEDED(hr)) {
                    
                    // Tell the box to reboot and wait for the debugger to specify a title.
                    DmReboot(DMBOOT_WAIT | DMBOOT_WARM);
                    
                    // Wait For the Reboot (but not forever)
                    if(WaitForSingleObject(m_hRebootEvent, WAIT_FOR_REBOOT_TIMEOUT) == WAIT_TIMEOUT)
                    {
                        hr = XBDM_CONNECTIONLOST;
                    }
                    else
                    {
                        _ASSERTE(m_dwExecState == DMN_EXEC_PENDING);
                    }
                }
                m_hRebootEvent = NULL;
                // Done with the reboot event
                HANDLE hRebootEvent = m_hRebootEvent;
                m_hRebootEvent = NULL;
                CloseHandle(hRebootEvent);
            }
        }
    }
    return hr;
}

HRESULT CXboxProcess::SetTitle(LPPROGLOAD pProgramLoadInfo)
/*++
  Routine Description:
    Reads and parse the arguments from LPPROGLOAD and calls DmSetTitle
--*/
{
    LPSTR  pszDir;
    LPSTR  pszTitle;
    LPSTR  pszCommandLine;
    
    _ASSERTE(pProgramLoadInfo->szDir);
    _ASSERTE(pProgramLoadInfo->szExe);
    
    // The Dir is usually the working directory.  Xbox doesn't have such a concept.  However, we will use this
    // as the directory on Xbox.
    pszDir = pProgramLoadInfo->szDir;
    
    //Strip a leading 'x' off the directory.
    if( ('x' == pszDir[0] || 'X' == pszDir[0]) && pszDir[1] && pszDir[2] == ':')
    {
        pszDir++;
    }
    
    // Command line has full path to the xbe, but it is the local path on the development system.
    // We clip the title name off the szCmdLine and tack it on the working directory to get the
    // path on the Xbox.
    pszTitle = strrchr(pProgramLoadInfo->szExe,'\\');
    if(pszTitle)
    {
        pszTitle++;
    }
    else
    {
        pszTitle = pProgramLoadInfo->szCmdLine;
    }

    //BUGBUG - As a temporary measure (to help out with symbols), we copy the title
    //         to a buffer, chop off the .exe and replace with .xbe.  In the future,
    //         we actually force the user to put in .xbe, but only after we modify the
    //         EE to suck symbols information from an XBE.
    char szHackedTitle[MAX_PATH];
    strcpy(szHackedTitle, pszTitle);
    pszTitle = strrchr(szHackedTitle, '.');
    if( pszTitle && ('e' == pszTitle[1]) && ('x' == pszTitle[2]) && ('e' == pszTitle[3]))
    {
        pszTitle[1] = 'x';
        pszTitle[2] = 'b';
    }
    
    // Command line args to pass to the caller
    pszCommandLine = pProgramLoadInfo->szCmdLine;
    if(pszCommandLine)
    {
        while( ' ' == *pszCommandLine|| '\t' == *pszCommandLine)
        {
            pszCommandLine++;
        }
    }
    
    return DmSetTitle(pszDir, szHackedTitle, *pszCommandLine ? pszCommandLine : NULL);
}



XOSD CXboxProcess::SynchronizeConnection(int dwReason)
/*++
  Routine Description:
   Does all the things to synchronize the debugger with process being debugged.

  Arguments:   
    dwReason
        SYNC_LAUNCH  - synchronize the debugger upon launching a process.
        SYNC_ATTACH  - synchronize the debugger upon attaching to a process.
    
        BUGBUG - for now, we only handle SYNC_LAUNCH, this needs to be fixed.
  
  Return Value:
    xosdNone - on success.

  Abstract:
    In the Windows world, once you attach to the debugger you start gettting debug
    events for things like process creation, module load, thread attach, etc.   Even
    if the process is already loaded (which it always is actually), you get all of
    these things to catch you up to the current state.  xbdm.dll (on Xbox) doesn't
    work this way, and XboxDbg.dll doesn't fake it for us.  So we have to.  In the VC6
    plug-in, we spun up a thread just to do that.  Here we are going to try a more
    elegant solution.  We will post events for each of these things in our CDebugEventPump
    (a.k.a. prime the pump), before we start our polling thread that start pumping them
    out to the EM.   This way we don't need to spin up a separate thread just to get
    things synchronized.

    Here is an outline of what we do:

    0) The process should be stopped before entering this routine.

    1) Disable KD

    2) Walk the modules, and find the XBE.  There may not be one, in which case, we refuse to attach.

    3) Build and post a CREATE_PROCESS_DEBUG_EVENT.

    4) Build and post a CREATE_THREAD_DEBUG_EVENT for each thread found excepting the main thread.  If
       we are attaching to a previously started process, we won't know which is the main thread, we
       just choose one.

    5) Build and post a LOAD_DLL_DEBUG_EVENT for every module, but the XBE's module.

    6) If we are attaching to a stopped process, walk the threads, find out which ones are stopped, building
       and posting an appropriate debug event for each one.

    At this point, are queue is primed with all the events to synchronize the EM with the current state.  Now
    just finish things up to keep it that way.
       
    6) Start the CDebugEventPump, so XboxDbg.DLL will start sending it nofications.

    7) Tell the Xbox to "go", except if we are attaching to a stopped process that is not stopped for an ASSERT
       (if it is stopped for an assert, we still tell it to go, don't worry it will hit the assert again
        immediately, this way we can get the assert string).

    8) Spin up the polling thread, which will convey all of this work up to the EM.

    We should now be running!!!  At least until we hit the initial break point, but that will just be
    handled through "normal" channels now.

--*/
{

#if 0 
//Under Construction
//Won't compile


    HRESULT            hr = S_OK;
    DEBUG_EVENT        debugEvent;

    //
    //  We don't want to compete with kd.
    //
    DmSendCommand(NULL, "kd disable", NULL, 0);


    //  Find the XBE, while we are at it get all the
    //  module information.
    const DMN_MODLOAD& LoadModuleInformation(DMN_MODULE_DEQUE& dmnModuleDeque

    BOOL               fFoundXBE = FALSE;
    BOOL               fFoundTLS = FALSE;
    PDM_WALK_MODULES   pDmWalkModules = NULL;
    DMN_MODULE_DEQUE   moduleDeque;
    DMN_MODLOAD&       dmLastModuleWithTls;
    DMN_MODLOAD        dmModule;
    while(SUCCEEDED(DmWalkLoadedModules(&pDmWalkModules, &dmModule))
    {
        // Insert the module
        modules.insert(modules.begin(), dmModule);
        if(dmModule.Flags & DMN_MODFLAG_XBE)
        {
            dmModule.BaseAddress 
            fFoundXBE = TRUE;
            break;
        }
    }

    //
    //  Generate a CREATE_PROCESS_DEBUG_EVENT and
    //  stuff it in the queue.
    ZeroStructure(debugEvent);


typedef struct _CREATE_PROCESS_DEBUG_INFO {
  HANDLE hFile; 
  HANDLE hProcess; 
  HANDLE hThread; 
  LPVOID lpBaseOfImage; 
  DWORD dwDebugInfoFileOffset; 
  DWORD nDebugInfoSize; 
  LPVOID lpThreadLocalBase; 
  LPTHREAD_START_ROUTINE lpStartAddress; 
  LPVOID lpImageName; 
  WORD fUnicode; 
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO; 

    debugEvent
/* Send the create process message.  If we're launching, we have the ID
     * of the first thread, and if we're connecting, we'll just pick the first
     * one in the thread list */
    ctid = sizeof(rgtid) / sizeof(DWORD);
    if(FAILED(DmGetThreadList(rgtid, &ctid)))
        ctid = rgtid[0] = 0;
    de.dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT;
    de.dwProcessId = 1;
    if(mdConnect == CONNECT_LAUNCH) {
        de.dwThreadId = g_tidBreak;
        de.u.CreateProcessInfo.lpStartAddress = g_pvThreadStart;
    } else 
        de.dwThreadId = rgtid[0];
    de.u.CreateProcessInfo.hFile = &conn.dmmlXbe;
    de.u.CreateProcessInfo.lpImageName = (PVOID)0x00010000;
    de.u.CreateProcessInfo.lpBaseOfImage = conn.dmmlXbe.BaseAddress;

    if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
        de.u.CreateProcessInfo.lpThreadLocalBase = dmti.TlsBase;
    else
        de.u.CreateProcessInfo.lpThreadLocalBase = NULL;
    ResetEvent(hEventContinue);
    ProcessDebugEvent(&de);
    WaitForSingleObject(hEventCreateProcess, INFINITE);
    WaitForSingleObject(hEventContinue, INFINITE);
    hprc = HPRCFromPID(1);
    _ASSERTE(hprc);
    _ASSERTE(hprc->pstate & ps_connect);


    
    
    



    // Start the polling thread, now we will get debug notifications, and start handling them.
    if(!StartPollingThread())
    {
        return xosdOutOfMemory;
    }
    
    /* We need to make sure we can find the XBE module.  We may be running on
     * an old build that doesn't report the XBE, so we'll key off of the last
     * module in the list that has TLS if that's the case */
    pdmwm = NULL;
    fSawXBE = FALSE;
    dmmlTls.Flags = 0;
    while(SUCCEEDED(DmWalkLoadedModules(&pdmwm, &conn.dmmlXbe))) {
        if(conn.dmmlXbe.Flags & DMN_MODFLAG_XBE) {
            fSawXBE = TRUE;
            break;
        } else if(conn.dmmlXbe.Flags & DMN_MODFLAG_TLS)
            memcpy(&dmmlTls, &conn.dmmlXbe, sizeof dmmlTls);
    }
    DmCloseLoadedModules(pdmwm);
    if(!fSawXBE) {
        if(dmmlTls.Flags)
            /* Didn't see the XBE, but we did find a module with TLS.  Pretend
             * it's the XBE */
            memcpy(&conn.dmmlXbe, &dmmlTls, sizeof dmmlTls);
        else
            return xosdBadProcess;
    }

    /* Now that our state is set up, we spin off a thread to send all of the
     * notifications through and we'll continue with startup */
    conn.hevtLoaded = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(!conn.hevtLoaded)
        return xosdOutOfMemory;
    conn.mdConnect = mdConnect;
    hthr = CreateThread(NULL, 0, ConnectThread, &conn, 0, NULL);
    if(!hthr) {
        CloseHandle(conn.hevtLoaded);
        return xosdOutOfMemory;
    }
    CloseHandle(hthr);

    /* We're going to connect to a running process.  We do so by synthesizing
     * all of the events the debugger would normally expect to see during
     * the course of process launch */
    int mdConnect;
    DWORD rgtid[128];
    DWORD ctid;
    DWORD itid;
    DWORD dwNotify;
    DEBUG_EVENT de;
    DMN_MODLOAD dmml;
    DMN_BREAK dmbr;
    DM_THREADSTOP dmts;
    DM_THREADINFO dmti;
    CONTEXT cr;
    HPRCX hprc;
    PDM_WALK_MODULES pdmwm;
    CONN conn;

    /* Grab our parameters */
    memcpy(&conn, pvParam, sizeof conn);
    SetEvent(conn.hevtLoaded);
    mdConnect = conn.mdConnect;

    /* Set up our notifications */
    DmStopOn(DMSTOP_CREATETHREAD | DMSTOP_FCE, TRUE);
    for(dwNotify = 1; dwNotify <= DM_NOTIFYMAX; ++dwNotify)
        DmNotify(psessNotify, dwNotify, XboxNotification);

    memset(&de, 0, sizeof de);
    /* Send the create process message.  If we're launching, we have the ID
     * of the first thread, and if we're connecting, we'll just pick the first
     * one in the thread list */
    ctid = sizeof(rgtid) / sizeof(DWORD);
    if(FAILED(DmGetThreadList(rgtid, &ctid)))
        ctid = rgtid[0] = 0;
    de.dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT;
    de.dwProcessId = 1;
    if(mdConnect == CONNECT_LAUNCH) {
        de.dwThreadId = g_tidBreak;
        de.u.CreateProcessInfo.lpStartAddress = g_pvThreadStart;
    } else 
        de.dwThreadId = rgtid[0];
    de.u.CreateProcessInfo.hFile = &conn.dmmlXbe;
    de.u.CreateProcessInfo.lpImageName = (PVOID)0x00010000;
    de.u.CreateProcessInfo.lpBaseOfImage = conn.dmmlXbe.BaseAddress;

    if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
        de.u.CreateProcessInfo.lpThreadLocalBase = dmti.TlsBase;
    else
        de.u.CreateProcessInfo.lpThreadLocalBase = NULL;
    ResetEvent(hEventContinue);
    ProcessDebugEvent(&de);
    WaitForSingleObject(hEventCreateProcess, INFINITE);
    WaitForSingleObject(hEventContinue, INFINITE);
    hprc = HPRCFromPID(1);
    _ASSERTE(hprc);
    _ASSERTE(hprc->pstate & ps_connect);

    /* Notify of the created threads */
    memset(&de.u, 0, sizeof de.u);
    de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
    /* Loop over all the threads to notify of their creation.  If we're
     * launching, we hit all of them and skip the one we already sent -- which
     * hopefully should total zero notifications (why would any other threads
     * exist if we've only created the first one?).  If we're not launching,
     * we skip the first thread since we already sent that one with the
     * process */
    for(itid = mdConnect != CONNECT_LAUNCH; itid < ctid; ++itid)
        {
        if(mdConnect != CONNECT_LAUNCH || rgtid[itid] != g_tidBreak) {
            /* For each thread, we need to wait for the DM to acknowledge
             * creation and continuation of the thread, and only then can we
             * mark the thread as running */
            ResetEvent(hprc->hEventCreateThread);
            ResetEvent(hEventContinue);
            de.dwThreadId = rgtid[itid];
            if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
                de.u.CreateThread.lpThreadLocalBase = dmti.TlsBase;
            else
                de.u.CreateThread.lpThreadLocalBase = NULL;
            ProcessDebugEvent(&de);
            WaitForSingleObject(hprc->hEventCreateThread, INFINITE);
            WaitForSingleObject(hEventContinue, INFINITE);
            SetDebugEventThreadState(hprc, ts_running);
        }
    }

    /* Enumerate the loaded DLLs and advise */
    pdmwm = NULL;
    while(SUCCEEDED(DmWalkLoadedModules(&pdmwm, &dmml))) {
        if(dmml.BaseAddress != conn.dmmlXbe.BaseAddress) {
            GetModLoadDebugEvent(&dmml, &de);
            ProcessDebugEvent(&de);
        }
    }
    DmCloseLoadedModules(pdmwm);

    /* At this point, we shouldn't have any deq's pending or in the works,
     * though we might have some continue events needing to be flushed */
    _ASSERTE(!dwTidStopped);
    _ASSERTE(!fDeqStopped);
    _ASSERTE(!pdeqFirst);
    DequeueAllEvents(TRUE, TRUE);

    switch(mdConnect) {
    case CONNECT_LAUNCH:
        /* We've sent all appropriate messages, and we're just waiting for
         * that breakpoint we're about to hit, so we'll continue the
         * initial thread and let the fireworks fly */
        hprc->pstate &= ~ps_connect;
        DmContinueThread(g_tidBreak, FALSE);
        DmGo();
        break;
    case CONNECT_ATTACH:
        /* this should have been converted to something else, but we'll just
         * treat it like connect to running */
        _ASSERTE(FALSE);
        // fall through
    case CONNECT_ATTACH_RUNNING:
        /* We forced this stop, which means we're connecting to a running
         * process.  We'll repost the breakpoint event and let it trigger
         * everything else */
        hprc->pstate &= ~ps_connect;
        cr.ContextFlags = CONTEXT_FULL;
        if(FAILED(DmGetThreadContext(g_tidBreak, (PCONTEXT)&cr)) ||
                !(cr.ContextFlags & CONTEXT_INTEGER))
            return xosdLineNotConnected;
        dmbr.ThreadId = g_tidBreak;
        dmbr.Address = (PVOID)cr.Eip;
        XboxNotification(DM_BREAK | DM_STOPTHREAD, (DWORD)&dmbr);
        break;
    case CONNECT_ATTACH_STOPPED:
        /* We didn't force this stop, so we'll say we've finished the load,
         * and then we'll go through every stopped thread and post a reason
         * for its stopped state */
        ConsumeAllProcessEvents (hprc, FALSE);
        ResetEvent(hEventContinue);
        de.dwDebugEventCode = LOAD_COMPLETE_DEBUG_EVENT;
        NotifyEM( &de, HTHDXFromPIDTID(1, rgtid[0]), 0, 0L);
        WaitForSingleObject( hEventContinue, INFINITE );

        /* We're connected now, so post those debug events */
        hprc->pstate &= ~(ps_connect | ps_preStart);
        mdConnect = CONNECT_ATTACH_RUNNING;
        for(itid = 0; itid < ctid; ++itid) {
            if(SUCCEEDED(DmIsThreadStopped(rgtid[itid], &dmts))) {
                if(dmts.NotifiedReason == DM_ASSERT) {
                    CONTEXT dmcr;
                    /* We missed the assert text, so we'll continue past the
                     * assert with an invalid continue and force the assert
                     * to repeat */
                    dmcr.ContextFlags = CONTEXT_INTEGER;
                    DmGetThreadContext(rgtid[itid], &dmcr);
                    dmcr.Eax = 0;
                    DmSetThreadContext(rgtid[itid], &dmcr);
                    DmContinueThread(rgtid[itid], FALSE);
                    if(mdConnect == CONNECT_ATTACH_RUNNING)
                        mdConnect = CONNECT_ATTACH_ASSERT;
                } else if(dmts.NotifiedReason != DM_NONE) {
                    mdConnect = CONNECT_ATTACH_STOPPED;
                    XboxNotification(dmts.NotifiedReason | DM_STOPTHREAD,
                        (DWORD)&dmts.u);
                    break;
                } else {
                    /* We should have continued these threads */
                    _ASSERTE(FALSE);
                }
            }
        }
        _ASSERTE(mdConnect != CONNECT_ATTACH_RUNNING);
        /* If our only stops are on asserts, we need to continue to force the
         * assert to reappear */
        if(mdConnect == CONNECT_ATTACH_ASSERT)
            DmGo();
        break;
    }
    #endif
    return xosdNone;
}

BOOL CXboxProcess::StartPollingThread()
/*++
  Routine Description:
    Kicks off a thread to begin polling for debug events.
--*/
{
    // Start only if it is not already running.
    if (!m_hPollThread)
    {
        m_hPollThread = (HANDLE)(LONG_PTR) _beginthreadex(
                                                0, 0, CXboxProcess::PollingLoopStub,
                                                reinterpret_cast<void *>(this), 0,
                                                reinterpret_cast<unsigned *>(&m_dwPollThreadId)
                                                );
    }
    _ASSERTE(m_hPollThread);
    return m_hPollThread ? TRUE : FALSE;
}

void CXboxProcess::StopPollingThread()
/*++
  Routine Description:
    Stops the polling thread and waits for it to stop.
--*/
{
    _ASSERTE(m_hPollThread);

    //  Killing the polling thread.
    //  
    //  1) Stopping notifications will tell XboxDbg.dll to stop sending notifications
    //     to m_DebugEventPump.
    //  2) It also causes m_DebugEventPump.GetDebugEvent to return FALSE immediately,
    //     when the debug event queue empties.
    //  3) The polling thread exists on its own when m_DebugEventPump.GetDebugEvent
    //     returns FALSE.
    m_DebugEventPump.StopNotifications();

    // Wait for the polling thread to exit.
    WaitForSingleObject(m_hPollThread, INFINITE);

    // The polling thread has exited, destroy all
    // memory of it.
    CloseHandle(m_hPollThread);
    m_hPollThread = NULL;
    m_dwPollThreadId = 0;
}

DWORD CXboxProcess::PollingLoop()
/*++
  Routine Description:
    The polling loop starts up a CDebugEventPump and then pumps on it
    dispatching debug events as necessary.

    The loop is terminated when another thread does SetEvent(m_hTerminatePollingEvent).
--*/
{
    DEBUG_EVENT debugEvent;
    while(m_DebugEventPump.GetDebugEvent(debugEvent));
    {
        // Write Code here to dispatch the debug Event. 

    }
    return 0;
}


//
// Notifications related methods.
//
// BUGBUG - this is one of the places that debugging multiple Xboxes simultaneously breaks down.  The notification
//          mechanism does not provide for a user context or a box ID.


HRESULT CXboxProcess::EnsureOpenNotificationSession()
/*++
  Routine Description:
    Makes sure that the process has an open notification session.
--*/
{
   HRESULT hr = XBDM_NOERR;
    if(!m_pDmNotificationSession)
    {
        hr = DmOpenNotificationSession(DM_PERSISTENT, &m_pDmNotificationSession);
    }
    return hr;
}

HRESULT CXboxProcess::CloseNotificationSession()
/*++
  Routine Description:
    Closes an open notification session
--*/
{
    HRESULT hr = XBDM_NOERR;
    if(m_pDmNotificationSession)
    {
        PDMN_SESSION pNotifySession = m_pDmNotificationSession;
        m_pDmNotificationSession = NULL;
        hr = DmCloseNotificationSession(pNotifySession);
    }
    return hr;
}
    
DWORD __stdcall CXboxProcess::LaunchNotifications(DWORD dwNotification, DWORD dwParam)
/*++
  Routine Description:
    Special notification handler during a process launch.
--*/
{
    DWORD dwTemp = 0;

    //BUGBUG - this really should be called in the context of a process.  It would
    //         be nice if the XboxDbg.dll would track our context and pass it in
    //         as dwContext.  The we could  do something like:
    // pThis = reinterpret_cast<CXboxProcess *>(dwContext);
    //         instead we are stuck with:
    CXboxProcess *pProcess = sm_pProcessList;
    _ASSERTE(pProcess);
    //         which of course this fails if there is more than one process in the list!
    
    switch(dwNotification & DM_NOTIFICATIONMASK)
    {
        case DM_EXEC:
           _ASSERTE(pProcess->m_hRebootEvent);
           dwTemp = pProcess->m_dwExecState;
           pProcess->m_dwExecState = dwParam;
           if((dwTemp == DMN_EXEC_REBOOT) && (pProcess->m_dwExecState != DMN_EXEC_REBOOT))
           {
                SetEvent(pProcess->m_hRebootEvent);
           }
           break;
        case DM_CREATETHREAD:
            pProcess->m_tidMainThread = reinterpret_cast<PDMN_CREATETHREAD>(dwParam)->ThreadId;
            pProcess->m_pvMainThreadStart = reinterpret_cast<PDMN_CREATETHREAD>(dwParam)->StartAddress;
            SetEvent(pProcess->m_hStartBreakPointEvent);
            break;

        case DM_EXCEPTION:
            // This was in the VC6 code.  Presumably, there might be some handled exception while
            // starting up that we just don't care about.  We are still XSS code when this happens,
            // presumably.
            if(
                (reinterpret_cast<PDMN_EXCEPTION>(dwParam)->Flags & DM_EXCEPT_FIRSTCHANCE) &&
                !(reinterpret_cast<PDMN_EXCEPTION>(dwParam)->Flags & DM_EXCEPT_NONCONTINUABLE)
                )
            {
                DmContinueThread(reinterpret_cast<PDMN_EXCEPTION>(dwParam)->ThreadId, TRUE);
                DmGo();
                //No event to set.
                return 0;
            }
            // Forward the error back on to kd
            _ASSERTE(!pProcess->m_tidMainThread);
            DmSendCommand(NULL, "kd exceptif", NULL, 0);
            break;
            
        case DM_BREAK:
            /* Uh oh, we failed to launch for some reason.  We need to inform the
            * shell that there is no process, which we will do by leaving
            * the start thread ID at zero */
            _ASSERTE(!pProcess->m_tidMainThread);
            DmReboot(DMBOOT_WAIT | DMBOOT_WARM);
            break;

        default:
            return 0;
    }
    
    SetEvent(pProcess->m_hStartBreakPointEvent);
    return 0;
}


/****
*****  Chopping Block
****/
#if 0

void ProcessSpawnOprhanCmd(LPDBB lpdbb)
/*++
  Routine Descrption:
    This routine used to be called to start a process without debugging it.  We have to figure out
    how to get called for this purpose again.
--*/
{
    XOSD        xosd;
    HRESULT     hr;
    DWORD       dwCreationFlags = 0;
    
    // Make sure the box is pending execution (waiting to launch a title) - reboot as necessary.
    hr = EnsurePendingExecState();
    if(SUCCEEDED(hr))
    {
        hr = ProcessSetLaunchTitle(reinterpret_cast<LPPROGLOAD>(lpdbb->rgbVar));
    }
    StopNotifications();    

    // If we managed to reboot and set a title, then
    // start running the title and report back
    if(SUCCEEDED(hr))
    {
        DmGo();
    }
    
    // Convert error code and reply
    if(hr == XBDM_NOSUCHFILE)
    {
        xosd = xosdFileNotFound;
    } else
    {
        xosd = xosdUnknown;
    }
}

#endif
