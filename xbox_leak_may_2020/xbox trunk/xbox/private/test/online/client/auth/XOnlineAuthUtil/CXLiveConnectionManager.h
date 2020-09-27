/***************************************************************************
 *
 *  Copyright (C) 2002 by Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       CXLiveConnectionManager.h
 *  Content:    Connection manager class declarations
 *  Owner:      davidhef
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xonlinep.h>
#include <debug.h>
#include <new.h>


namespace XLive
{

// Forward declare the finalizer.
void __cdecl DestroyCXLiveConnectionManager( void );



class CXLiveConnectionManager
{
    friend void __cdecl DestroyCXLiveConnectionManager( void );
    friend DWORD WINAPI LogonWorkerThreadFunction(void *pArgs);

    // This class is a singleton.
public:
    inline static CXLiveConnectionManager& Instance()
    {
        if (NULL == m_pInstance)
        {
            if (m_bDestroyed)
            {
                HandleDeadReference();
                m_bDestroyed = false;
            }
            else
            {
                // Create for the first time.
                CreateInstance();
            }
        }
        return *m_pInstance;
    }

    HRESULT Logon(XONLINE_USER *pUsers, DWORD dwTimeout = 5000, bool fWaitForConnection = false);
    HRESULT LogonAndWaitForConnection(XONLINE_USER *pUsers, DWORD dwTimeout = 5000);
    HRESULT WaitForConnection();
    HRESULT Logoff();

    HRESULT SelectServices(DWORD svc);
    HRESULT SelectServices(DWORD svc1, DWORD svc2);
    HRESULT SelectServices(DWORD svc1, DWORD svc2, DWORD svc3);
    HRESULT SelectServices(DWORD *pServices, DWORD dwServicesCount);
    inline HRESULT SelectNoServices( void ) { m_pdwServices = NULL; m_dwServicesCount = 0; return S_OK; }

    inline HRESULT GetLogonStatus() const
    {
        if (NULL != m_taskHandle && Ready != m_classState)
        {
            return XOnlineLogonTaskGetResults(m_taskHandle);
        }
        return XONLINE_E_USER_NOT_LOGGED_ON;
    }

private:
    explicit CXLiveConnectionManager()
        :   m_lClassLock(0), m_dwOwningThreadId(0),
            m_logonEventHandle(NULL), m_timeoutHandle(NULL),
            m_taskHandle(NULL),
            m_hWorkerThreadHandle(NULL), m_dwWorkerThreadId(0),
            m_lastHR(S_OK), m_bAbortThread(false),
            m_classState(Ready)
    {
        HRESULT hr = Init();
        AssertSz(S_OK == hr, "Unable to construct XLive::CXLiveConnectionManager!");
    }
    
    ~CXLiveConnectionManager()
    {
        // Kill the worker thread, if it's still alive.
        // However, it should already have been killed
        // by now.
        if (m_dwWorkerThreadId != 0 && m_logonEventHandle != NULL)
        {
            m_bAbortThread = true;
            SetEvent(m_logonEventHandle);
            Sleep(1000);
        }

        (void) CleanupResources();
        m_pInstance = NULL;
        m_bDestroyed = true;
    }

    inline static void CreateInstance()
    {
        static CXLiveConnectionManager connectionMgr;
        new (&connectionMgr) CXLiveConnectionManager;
        m_pInstance = &connectionMgr;
    }

    inline static void HandleDeadReference()
    {
        CreateInstance();
        atexit(DestroyCXLiveConnectionManager);
        m_bDestroyed = false;
    }

    inline CXLiveConnectionManager(CXLiveConnectionManager const &) {}
    inline CXLiveConnectionManager& operator=(CXLiveConnectionManager const &) {}

    HRESULT Init();
    HRESULT CleanupResources();

    inline HRESULT VerifyOwnership();
    HRESULT CreateLogonWorkerThread();
    HRESULT _WaitForConnection();

    static CXLiveConnectionManager  *m_pInstance;
    static bool                     m_bDestroyed;

    bool                        m_bInitialized;     // true, if Init() was successful.
    LONG                        m_lClassLock;       // Synchronizes use of the class.
    DWORD                       m_dwOwningThreadId; // This thread owns the class.

    XONLINE_USER                m_users[XONLINE_MAX_LOGON_USERS];   // Users to log on

    DWORD                       m_dwServices[XONLINE_MAX_NUMBER_SERVICE];   // Used by Logon
    DWORD                       m_dwServicesCount;                          // Used by Logon
    DWORD                       *m_pdwServices;     // Used by Logon

    volatile XONLINETASK_HANDLE m_taskHandle;       // Used by logon task pump

    volatile HANDLE             m_logonEventHandle; // Used by logon task pump.
    volatile HANDLE             m_timeoutHandle;    // Used to timeout logons.

    // The following is the handle to an event that is signalled by the
    // logon worker thread.  The main thread waits for this event when
    // waiting for the worker thread to complete the connection.
    volatile HANDLE             m_mainThreadTimeoutHandle;

    volatile enum{Ready, LoggedOn, Connected}       m_classState;

    // Thread/connection state.
    volatile DWORD              m_dwWorkerThreadId; // When 0, there is no worker thread.
    volatile HANDLE             m_hWorkerThreadHandle;  // Returned by CreateThread.
    volatile HRESULT            m_lastHR;           // The worker thread's hr from XOnlineTaskContinue

    // Worker thread control
    volatile bool               m_bAbortThread;     // Setting this flag terminates the worker thread.

};  // class CXLiveConnectionManager





} // namespace XLive





