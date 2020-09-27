/*++

Copyright (c) Microsoft Corporation

Module Name:

    events.h

Abstract:

    Declaration of CDebugEventPump.

    CDebugEventPump is a central receiving and translation facility for debug events.  This
    class is used by the processes polling thread.

    1) Set up notifications of debug events.
    2) Receives notifications from xboxdbg.dll.
    2) Translates notifications into debug events.
    3) Synchronizes access to debug events.
    
Author:
    mitchd

--*/

#ifndef __XBOX_EVENTS__
#define __XBOX_EVENTS__

typedef std::queue<DEBUG_EVENT> CDebugEventQueue;  //Yes, queue is from STL.
class CDebugEventPump
{
  public:
    CDebugEventPump() : m_pNotificationSession(NULL) {}
    ~CDebugEventPump();

    HRESULT Initialize(PDMN_SESSION pNotificationSession);
    HRESULT StartNotifications();
    inline HRESULT PauseNotifications() 
        {return DmNotify(m_pNotificationSession, DM_NONE, CDebugEventPump::EventNotifications);}
    HRESULT StopNotifications();
    
    BOOL GetDebugEvent(OUT DEBUG_EVENT& DebugEvent);
    void PostDebugEvent(const DEBUG_EVENT& DebugEvent);

private:
    
    static DWORD __stdcall EventNotifications(ULONG dwNotification, DWORD dwParam);
    static CDebugEventPump *sm_pTheOneAndOnly;

    CDebugEventQueue  m_EventQueue;
    PDMN_SESSION      m_pNotificationSession;
    CRITICAL_SECTION  m_CriticalSection;

    enum {QuitPump=0,DebugEventPosted=1};
    HANDLE  m_hSynchronizationEvents[2];

};

#endif //__XBOX_EVENTS__