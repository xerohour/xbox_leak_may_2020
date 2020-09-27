/*++

Copyright (c) Microsoft Corporation

Module Name:

    process.h

Abstract:
    
    Declarations related to a process


Notes:
    
    Xboxes are single process, so the term process is synonymous with Xbox.  Here you will find code that almost
    encapsulates a process so that multiple processes (Xboxes) maybe debugged at once.  VC7 now supports this type
    of thing for the purpose of interprocess\machine debugging.  This could be especially powerful when debugging
    two Xboxes attached via a system link, or multiple Xboxes along with a backend server.

    Alas, it doesn't work.  Currently, XboxDbg.dll will only handle one box at a time from a single process.  Yes, there
    is the IXboxConnection thing, but that currently doesn't support notifications.  So we are out of luck for the time being.
    Maybe next year.

    So while this code comes close, there are some thing missing, and there are some places where we specifically prohibit it.
    The absence of something is hard to document (hint, we never tell XboxDbg which Xbox to debug, it is just always the default one).
    The extra stuff is enclosed as much as possible in #ifndef MULTI_XBOX_DEBUGGING.  Of course the whole notification mechanism doesn't
    support multiprocess debugging, so good luck there.
    
--*/

#ifndef  __XBOX_PROCESS__
#define  __XBOX_PROCESS__


// The exception list is tracked per process.  It contains a list of exceptions
// and how we respond to them.  When the process is initialized this list is populated
// from a static table.  After that,
typedef struct EXCEPTION_ITEM *PEXCEPTION_ITEM;
struct EXCEPTION_ITEM
{
    EXCEPTION_ITEM        *NextExceptionItem;
    EXCEPTION_DESCRIPTION Exception;
};

typedef std::deque<DMN_MODLOAD> DMN_MODULE_DEQUE;
typedef DMN_MODULE_DEQUE *PDMN_MODULE_DEQUE;

class CXboxProcess
{
  public:
    // Initialization and destructions
    static XOSD InitializeCXboxProcessClass();  //Global Initializer for 
    static XOSD Create(HPID hPid);
    XOSD Destroy();
    
    // Find a process from an HPID
    static CXboxProcess *GetProcess(HPID hPid);
    
    // Loading and Unloading Processes
    XOSD ProgramLoad(LPPROGLOAD pProgamLoadInfo);

    // Exceptions
    XOSD GetExceptionInfo(LPEXCMD pExceptionCommand, OUT EXCEPTION_DESCRIPTION *pExceptionDescriptionRet);
    static XOSD GetDefaultExceptionInfo(LPEXCMD pExceptionCommand, OUT EXCEPTION_DESCRIPTION *pExceptionDescriptionRet);
    
  private:
    
    // Private c'tor and d'tor force clients to use public create and destroy.
    CXboxProcess(HPID hPid) : m_hPid(hPid), m_pNextProcess(NULL), m_dwExecState(0), m_hPollThread(NULL),
                              m_dwPollThreadId(0), m_hTerminatePollingEvent(NULL),
                              m_pExceptionList(NULL), m_pDmNotificationSession(NULL), m_hRebootEvent(NULL),
                              m_hStartBreakPointEvent(NULL), m_tidMainThread(0), m_pvMainThreadStart(NULL){}
    ~CXboxProcess();
    XOSD InitializeAndInsertIntoProcessList();

    // Title\State Management
    HRESULT EnsurePendingExecState();
    HRESULT SetTitle(LPPROGLOAD pProgramLoadInfo);
    XOSD SynchronizeConnection(int dwReason);

    // Debug Events
    BOOL StartPollingThread();
    void StopPollingThread();
    DWORD PollingLoop();
    static unsigned __stdcall PollingLoopStub(void *pvProcess)
        {return reinterpret_cast<CXboxProcess *>(pvProcess)->PollingLoop();}

    // Notifications
    HRESULT EnsureOpenNotificationSession();
    HRESULT CloseNotificationSession();
    static DWORD __stdcall LaunchNotifications(ULONG dwNotification, DWORD dwParam);

    //
    //  Data members
    //
    HPID             m_hPid;
    CXboxProcess     *m_pNextProcess;
    DWORD            m_dwExecState;

    // Debug Events
    CDebugEventPump m_DebugEventPump;
    HANDLE          m_hPollThread;
    DWORD           m_dwPollThreadId;
    HANDLE          m_hTerminatePollingEvent;
    
    // Exception handling - including single step, and breakpoints
    PEXCEPTION_ITEM  m_pExceptionList;

    // Notifications
    PDMN_SESSION    m_pDmNotificationSession; // Notification Session Handle    
    HANDLE          m_hRebootEvent;           // Event to signal that an Xbox has rebooted (and is now in exec pending state).
    HANDLE          m_hStartBreakPointEvent;  // Event to signal that an Xbox has just stated a new process (after reboot).
    

    //Info About the Main Thread
    DWORD   m_tidMainThread;
    PVOID   m_pvMainThreadStart;

    //
    //  Static data members
    //
    static CRITICAL_SECTION  sm_CriticalSection; // One critical section for all processes.
    static CXboxProcess     *sm_pProcessList;    // List of currently known processes.
};




#endif  //__XBOX_PROCESS__
