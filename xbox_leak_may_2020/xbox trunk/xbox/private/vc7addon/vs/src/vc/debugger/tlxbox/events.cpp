/*++

Copyright (c) Microsoft Corporation

Module Name:

    events.cpp

Abstract:

    Implementation of CDebugEventPump.  

    CDebugEventPump is a central receiving and translation facility for debug events.
    1) Set up notifications of debug events.
    2) Translates notifications into their proper debug events.
    3) Synchronizes access to debug events.

    Does not provide the polling thread, nor does it actually handle the event
    
Author:
    mitchd

--*/
#include <precomp.h>
#include <queue>
using namespace std;

//-----------------------------------------------------------------------------------
//  Static Member Initialization
//-----------------------------------------------------------------------------------
// BUGBUG: xboxdbg.dll won't hold a context for us, so we can currently have only
//         one instance which we store here.
CDebugEventPump *CDebugEventPump::sm_pTheOneAndOnly = NULL;

//-----------------------------------------------------------------------------------
//  Implementation of public members of CDebugEventPump
//-----------------------------------------------------------------------------------
HRESULT CDebugEventPump::Initialize(PDMN_SESSION pNotificationSession)
/*++
  Routine Description:
    Initialization is delayed from the c'tor to here.  As is often the case in C++, it is more
    convenient:  you don't need to initialize in the member initialize list of class if you
    include it as a member, you don't get a warning if you access the this pointer, you can
    return a meaningful error code rather throwing an exception, etc.
  
  Arguments:
    pNotificationSession   - notification session to use for getting callbacks from XboxDbg.dll.  Since
                             each session creates a thread, this is just reasonable optimization.
  Return Value:
    S_OK on success.  Otherwise, a win32 error code translated into an HRESULT.
--*/
{
    // We need two events for synchronizing the pump.
    //  QuitPump - StopNotification signals it to force GetDebugEvent to return FALSE;
    //  DebugEventPosted - Signaled when a debug event is added to the queue.
    m_hSynchronizationEvents[QuitPump] = CreateEvent(NULL, FALSE, TRUE, NULL);
    if(m_hSynchronizationEvents[QuitPump])
    {
        m_hSynchronizationEvents[DebugEventPosted] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if(!m_hSynchronizationEvents[DebugEventPosted])
        {
            CloseHandle(m_hSynchronizationEvents[QuitPump]);
            m_hSynchronizationEvents[QuitPump] = NULL;
        }
    }
    // If the events were not created, return and error
    if(!m_hSynchronizationEvents[QuitPump])
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Make sure that we are initializing the one and only instance.
    _ASSERTE(!sm_pTheOneAndOnly);
    sm_pTheOneAndOnly = this;

    // We will need a critical section to protect our queue (based on STL),
    // which is not multithread safe.
    InitializeCriticalSection(&m_CriticalSection);  

    // Make a copyof the notification session passed in by the caller
    m_pNotificationSession = pNotificationSession;
    return S_OK;
}

CDebugEventPump::~CDebugEventPump()
/*++
    Routine Description:
      D'tor cleans up.
--*/
{
    // They should be stopped, but this cannot hurt.
    StopNotifications();

    //  We borrowed the notification session, it was
    //  around before us, and will live after us.
    m_pNotificationSession = NULL;  
    
    
    // Destroy the two synchronization events we created/
    CloseHandle(m_hSynchronizationEvents[QuitPump]);
    CloseHandle(m_hSynchronizationEvents[DebugEventPosted]);
    m_hSynchronizationEvents[QuitPump] = NULL;
    m_hSynchronizationEvents[DebugEventPosted] = NULL;

    // We should be just a memory
    sm_pTheOneAndOnly = NULL;
}

HRESULT CDebugEventPump::StartNotifications()
/*++
  Routine Description:
    Called to start up notifications.  This starts asking XboxDbg.dll
    for notifications.  It also makes sure that GetDebugEvent will
    block if the queue is empty.
--*/
{
    DWORD dwNotification;
    HRESULT hr = S_OK;
    
    // Make sure GetDebugEvent will block properly
    ResetEvent(m_hSynchronizationEvents[QuitPump]);
    
    // Setup all of our notifications.
    hr = DmStopOn(DMSTOP_CREATETHREAD | DMSTOP_FCE, TRUE);
    if(SUCCEEDED(hr))
    {
        for(dwNotification=DM_BREAK; dwNotification <= DM_NOTIFYMAX; dwNotification++)
        {
            hr = DmNotify(m_pNotificationSession, dwNotification, CDebugEventPump::EventNotifications);
            if(FAILED(hr)) break;
        }
    }
    return hr;
}

HRESULT CDebugEventPump::StopNotifications()
/*++
  Routine Description:
    Not only stops XboxDbg.dll from sending us notification to post,
    it also unblocks GetDebugEvent.  When the queue is empty it will return
    FALSE.
--*/
{
    HRESULT hr = PauseNotifications();
    SetEvent(m_hSynchronizationEvents[QuitPump]);
    return hr;
}

BOOL CDebugEventPump::GetDebugEvent(OUT DEBUG_EVENT& DebugEvent)
/*++
  Routine Description:
    The header of CDebugEventPump.  It pulls an event off the queue and
    returns it to the caller.  Blocks indefinately if there are no events.
  
  Arugments:
    DebugEvent - [OUT] event from queue, returned by reference.

  Return Value:
    TRUE  - if there is a new event.
    FALSE - if the queue is empty AND terminate polling has been signalled.
--*/
{
    do
    {
        //Try to get an event
        { CHoldCriticalSection protectEventQueue(&m_CriticalSection);
            if(!m_EventQueue.empty())
            {
                //Found one, return it.
                DebugEvent = m_EventQueue.front();
                m_EventQueue.pop();
                return TRUE;
            }
        }
        // Wait for QuitPump or DebugEventPosted
    } while( (QuitPump + WAIT_OBJECT_0) !=
              WaitForMultipleObjects(
                ARRAYSIZE(m_hSynchronizationEvents),
                m_hSynchronizationEvents,
                FALSE, INFINITE)
                );
    

    // If we fell out, the QuitPump event was signaled. It is possible, 
    // though fairly unlikely, that the queue is not empty.  An event could
    // have been posted just before StopNotifications was signaled.  If this
    // thread hadn't run in between, we would be stuck with a non-empty queue.
    // So ... check again.
    CHoldCriticalSection protectEventQueue(&m_CriticalSection);
    if(!m_EventQueue.empty())
    {
        //Found one, return it.
        DebugEvent = m_EventQueue.front();
        m_EventQueue.pop();
        return TRUE;
    }
    return FALSE;
}

void CDebugEventPump::PostDebugEvent(const DEBUG_EVENT& DebugEvent)
/*++
    Routine Description:
      Used to stuff debug events into the queue.  
    Arguments:
      Event to stuff in queue.
    Motivation:
      Primarly: When attaching to a process (either launching a new one or attaching
      to a running one), the debugger expects events for module loads, and thread
      creates that Xbox doesn't provide these ex-post-facto.  So the attachment code
      goes and enumerates the modules and threads and posts them as debug events.
      Walla, when we start the polling thread starts pumping, the first debug events
      are all these things that the debugger was expecting.
--*/
{
    _ASSERTE(m_pNotificationSession);
    CHoldCriticalSection protectEventQueue(&m_CriticalSection);
    m_EventQueue.push(DebugEvent);
}

DWORD __stdcall CDebugEventPump::EventNotifications(ULONG dwNotification, DWORD dwParam)
/*++
  Routine Description:
    The normal way we get debug events.  The notifications sent by XboxDbg.dll do not
    exactly correspond to the Windows DEBUG_EVENTs that the debugger is expecting.  So
    we do some translation, sometimes here, sometimg
--*/
{
    DEBUG_EVENT debugEvent;
    ZeroStructure(debugEvent);
    CDebugEventPump *pTheDebugEventPump = CDebugEventPump::sm_pTheOneAndOnly;
    BOOL fPostEvent = TRUE;
    

    //  NOTE: We abuse debugEvent.dwProcessId:
    //  1) We are pumping messages for one process, the reader
    //     pulling the messages knows what it told the debugger
    //     the dwProcessId was.  So the caller of GetDebugEvent
    //     fills it in.  If we ever move to multi-Xbox debugging,
    //     it will be cleaner this way.
    //  2) We have flags we can stick there. Like DM_STOPTHREAD
    debugEvent.dwProcessId = dwNotification&DM_STOPTHREAD;
    dwNotification &= DM_NOTIFICATIONMASK;
    
    switch(dwNotification)
    {
        // DM_BREAK, DM_SINGLESTEP, and DM_DATABREAK are all EXCEPTION_BREAKPOINT.
        // There are some slight differences in how you fill it out, but they are similar.
        case DM_BREAK:
            debugEvent.u.Exception.ExceptionRecord.ExceptionCode = EXCEPTION_BREAKPOINT;
            //fall through - sort of ...
            if(dwNotification != DM_BREAK)  //If the compiler is smart, this will just be a jmp.
            {
        case DM_DATABREAK:
                debugEvent.u.Exception.ExceptionRecord.NumberParameters = 2;
                debugEvent.u.Exception.ExceptionRecord.ExceptionInformation[0] =
                                         reinterpret_cast<ULONG_PTR>(reinterpret_cast<DMN_DATABREAK *>(dwParam)->DataAddress);
                debugEvent.u.Exception.ExceptionRecord.ExceptionInformation[1] = 
                                        reinterpret_cast<DMN_DATABREAK *>(dwParam)->BreakType;
                //fall through
        case DM_SINGLESTEP:
                debugEvent.u.Exception.ExceptionRecord.ExceptionCode = EXCEPTION_SINGLE_STEP;
            }
            debugEvent.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
            debugEvent.dwThreadId = reinterpret_cast<DMN_BREAK *>(dwParam)->ThreadId;
            debugEvent.u.Exception.ExceptionRecord.ExceptionAddress =
                                    reinterpret_cast<DMN_BREAK *>(dwParam)->Address;
            break;
    
        // The dwParam parameter contains a pointer to a DMN_DEBUGSTR structure. 
        case DM_DEBUGSTR:
            debugEvent.dwDebugEventCode = OUTPUT_DEBUG_STRING_EVENT;
            debugEvent.dwThreadId = reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->ThreadId;
            debugEvent.u.DebugString.nDebugStringLength = static_cast<WORD>(reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->Length);
            // Beware of leaks - we are copying the string.  Our queue is dumb, it will just
            // copy the pointer value.  That is actually good.  Debug events should all be pulled from
            // queue before the queue dies, and there is only one queue reader, so that reader owns
            // freeing the string.
            debugEvent.u.DebugString.lpDebugStringData = new char[debugEvent.u.DebugString.nDebugStringLength + 1];
            memcpy(
                debugEvent.u.DebugString.lpDebugStringData,
                reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->String, 
                debugEvent.u.DebugString.nDebugStringLength
                );
            break;

        // The dwParam parameter contains a DMN_EXEC constant.
        case DM_EXEC:
            // Hmm, helps keep track of exec state.  If it 
            // went into the reboot state, we should generate
            // an EXIT_PROCESS_DEBUG_EVENT.  Otherwise, we
            // are not tracking the exec state, for now.
            if(dwParam == DMN_EXEC_REBOOT)
            {
              debugEvent.dwDebugEventCode = EXIT_PROCESS_DEBUG_EVENT;
              //debugEvent.u.ExitProcess.dwExitCode = 0; - Already 0.
              pTheDebugEventPump->PauseNotifications();
            } else
            {
              fPostEvent = FALSE;
            }
            break;
        // The dwParam parameter contains a pointer to a DMN_MODLOAD structure. 
        case DM_MODLOAD:
           // We are ill prepared to handle this here.
           debugEvent.dwDebugEventCode = LOAD_DLL_DEBUG_EVENT;
           debugEvent.u.LoadDll.hFile = (HANDLE)dwParam;  //For internal use.
           debugEvent.u.LoadDll.lpBaseOfDll = reinterpret_cast<DMN_MODLOAD *>(dwParam)->BaseAddress;
           /*  These are
           debugEvent.u.LoadDll.dwDebugInfoFileOffset = 0;
           debugEvent.u.LoadDll.nDebugInfoSize = 0 ;
           debugEvent.u.LoadDll.lpImageName = NULL; //Don't give the image name for now.
           *
           reinterpret_cast<DMN_MODLOAD *>(wParam)->CheckSum 
           reinterpret_cast<DMN_MODLOAD *>(wParam)->Flags 
           reinterpret_cast<DMN_MODLOAD *>(wParam)->Name 
           reinterpret_cast<DMN_MODLOAD *>(wParam)->Size 
           reinterpret_cast<DMN_MODLOAD *>(wParam)->TimeStamp 
           */
            break;

        // The dwParam parameter contains a pointer to a DMN_MODLOAD structure. 
        case DM_MODUNLOAD:
           // We are ill prepared to handle this here.
           debugEvent.dwDebugEventCode = UNLOAD_DLL_DEBUG_EVENT;
           debugEvent.u.UnloadDll.lpBaseOfDll = reinterpret_cast<DMN_MODLOAD *>(dwParam)->BaseAddress;
            break;

        // The dwParam parameter contains a pointer to a DMN_CREATETHREAD structure. 
        case DM_CREATETHREAD:
            debugEvent.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
            debugEvent.dwThreadId = reinterpret_cast<DMN_CREATETHREAD *>(dwParam)->ThreadId;
            debugEvent.u.CreateThread.lpStartAddress = 
                reinterpret_cast<LPTHREAD_START_ROUTINE>(reinterpret_cast<DMN_CREATETHREAD *>(dwParam)->StartAddress);
            {
                DM_THREADINFO dmThreadInfo;
                // Check for local storage - if it fails the lpThreadLocalBase is already NULL
                if(SUCCEEDED(DmGetThreadInfo(debugEvent.dwThreadId, &dmThreadInfo)))
                {
                    debugEvent.u.CreateThread.lpThreadLocalBase = dmThreadInfo.TlsBase;
                }
            }
            break;

        // The dwParam parameter contains the ID of the thread that was destroyed. 
        case DM_DESTROYTHREAD:
            debugEvent.dwDebugEventCode = EXIT_THREAD_DEBUG_EVENT;
            debugEvent.dwThreadId = dwParam;
            debugEvent.u.ExitThread.dwExitCode = 0; //Who cares?  Why do threads have exit codes?
            break;

        // The dwParam parameter contains a pointer to a DMN_EXCEPTION structure. 
        case DM_EXCEPTION:
            debugEvent.dwDebugEventCode =EXCEPTION_DEBUG_EVENT;
            debugEvent.dwThreadId = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->ThreadId;
            debugEvent.u.Exception.dwFirstChance = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Flags & DM_EXCEPT_FIRSTCHANCE;
            debugEvent.u.Exception.ExceptionRecord.ExceptionCode = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Code;
            // Xbox doesn't tell us how many elements of the ExceptionInformation are in use, basically we
            // hard code off the ExceptionCode.  EXCEPTION_ACCESS_VIOLATION has two, nobody else has anything.
            // Seems like a short-coming, since I user defined exception might want to pass information.
            if(EXCEPTION_ACCESS_VIOLATION == debugEvent.u.Exception.ExceptionRecord.ExceptionCode)
            {
                debugEvent.u.Exception.ExceptionRecord.NumberParameters = 2;
                debugEvent.u.Exception.ExceptionRecord.ExceptionInformation[0] = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Information[0];
                debugEvent.u.Exception.ExceptionRecord.ExceptionInformation[1] = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Information[1];
            }
            debugEvent.u.Exception.ExceptionRecord.ExceptionAddress = reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Address;
            debugEvent.u.Exception.ExceptionRecord.ExceptionFlags =
                (reinterpret_cast<DMN_EXCEPTION *>(dwParam)->Flags & DM_EXCEPT_NONCONTINUABLE) ?  EXCEPTION_NONCONTINUABLE : 0;
            break;

        // The dwParam parameter contains a pointer to a DMN_DEBUGSTR structure. 
        case DM_ASSERT:
            fPostEvent = FALSE;
            //TODO: HandleAssert(reinterpret_cast<DMN_DEBUGSTR *>(dwParam));
            break;

        // The dwParam parameter contains a pointer to a RIP_INFO structure. 
        case DM_RIP:
            debugEvent.dwDebugEventCode = RIP_EVENT;
            debugEvent.dwThreadId = reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->ThreadId;
            // Beware of leaks - we are copying the string.  Our queue is dumb, it will just
            // copy the pointer value.  That is actually good.  Debug events should all be pulled from
            // queue before the queue dies, and there is only one queue reader, so that reader owns
            // freeing the string.
            if(reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->String)
            {
                debugEvent.u.RipInfo.dwError = reinterpret_cast<DWORD>
                    (new char[reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->Length + 1]);
                memcpy(
                    reinterpret_cast<PVOID>(debugEvent.u.DebugString.lpDebugStringData),
                    reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->String, 
                    reinterpret_cast<DMN_DEBUGSTR *>(dwParam)->Length
                    );
            }
            break;
    }
    if(fPostEvent)
    {
        pTheDebugEventPump->PostDebugEvent(debugEvent);
    }
    return 0;
}