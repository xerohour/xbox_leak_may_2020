/***************************************************************************
 *
 *  Copyright (C) 2002 by Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       CXLiveConnectionManager.cpp
 *  Content:    Connection manager implementation
 *  Owner:      davidhef
 *
 ****************************************************************************/
#include "CXLiveConnectionManager.h"
#include <new>
#include <debug.h>

namespace XLive
{

// Class static variables
CXLiveConnectionManager *CXLiveConnectionManager::m_pInstance = NULL;
bool CXLiveConnectionManager::m_bDestroyed = false;

    
HRESULT CXLiveConnectionManager::Init()
{
    HRESULT hr = S_OK;

    m_lClassLock = 0;
    m_dwOwningThreadId = 0;
    ZeroMemory(m_users, sizeof(m_users));
    ZeroMemory(m_dwServices, sizeof(m_dwServices));
    m_dwServicesCount = 0;
    m_pdwServices = NULL;
    m_taskHandle = 0;
    m_logonEventHandle = 0;
    m_timeoutHandle = NULL;
    m_mainThreadTimeoutHandle = NULL;
    m_classState = Ready;
    m_dwWorkerThreadId = 0;
    m_hWorkerThreadHandle = 0;
    m_lastHR = E_INVALIDARG;
    m_bAbortThread = false;

    //
    // Acquire needed dynamic resources.
    //
    if (SUCCEEDED(hr))
    {
        m_logonEventHandle = CreateEvent( NULL, FALSE, TRUE, "XOnlineLogonEvent" );
        hr = (NULL == m_logonEventHandle) ? E_FAIL : hr;
    }

    if (SUCCEEDED(hr))
    {
        m_timeoutHandle = CreateWaitableTimer(NULL, TRUE, "LogonTimeoutTimer");
        hr = (NULL == m_timeoutHandle) ? E_FAIL : hr;
    }

    if (SUCCEEDED(hr))
    {
        m_mainThreadTimeoutHandle = CreateEvent( NULL, FALSE, TRUE, NULL );
        hr = (NULL == m_mainThreadTimeoutHandle) ? E_FAIL : hr;
    }

    m_bInitialized = (SUCCEEDED(hr)) ? true : false;

    return hr;
}

HRESULT CXLiveConnectionManager::CleanupResources()
{
    //
    // This function should restore the instance
    // to the same state it was in after construction.
    //

    HRESULT hr = S_OK;

    // Close the events.
    if (NULL != m_logonEventHandle)
    {
        hr = (!CloseHandle(m_logonEventHandle)) ? E_FAIL : hr;
        m_logonEventHandle = (SUCCEEDED(hr)) ? NULL : m_logonEventHandle;
    }
    
    if (NULL != m_mainThreadTimeoutHandle)
    {
        hr = (!CloseHandle(m_mainThreadTimeoutHandle)) ? E_FAIL : hr;
        m_mainThreadTimeoutHandle = (SUCCEEDED(hr)) ? NULL : m_mainThreadTimeoutHandle;
    }

    // Close the Logon task.
    if (NULL != m_taskHandle)
    {
        hr = (SUCCEEDED(XOnlineTaskClose(m_taskHandle))) ? hr : E_FAIL;
        m_taskHandle = (SUCCEEDED(hr)) ? NULL : m_taskHandle;
    }

    // Close the worker thread handle.
    if (NULL != m_hWorkerThreadHandle)
    {
        hr = (!CloseHandle(m_hWorkerThreadHandle)) ? E_FAIL : hr;
        m_hWorkerThreadHandle = (SUCCEEDED(hr)) ? NULL : m_hWorkerThreadHandle;
    }

    // Close the timeout timer handle.
    if (NULL != m_timeoutHandle)
    {
        hr = (!CloseHandle(m_timeoutHandle)) ? E_FAIL : hr;
        m_timeoutHandle = (SUCCEEDED(hr)) ? NULL : m_timeoutHandle;
    }

    m_bInitialized = false;

    return hr;
}

inline HRESULT CXLiveConnectionManager::VerifyOwnership()
{
    // Assume that the class is already locked.
    AssertSz(m_dwOwningThreadId == GetCurrentThreadId(), "Another thread is using the connection manager!");
    if (m_dwOwningThreadId == 0)
    {
        // Take ownership.
        m_dwOwningThreadId = GetCurrentThreadId();
    }

    return (m_dwOwningThreadId == GetCurrentThreadId()) ? S_OK : E_FAIL;
}

HRESULT CXLiveConnectionManager::Logon(XONLINE_USER *pUsers, DWORD dwTimeout, bool fWaitForConnection)
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
    {
        // Only 1 thread is allowed to use this singleton.
        hr = VerifyOwnership();
    }

    hr = (m_classState == Ready) ? S_OK : E_FAIL;

    // Reset the event that we will wait for when we are called with fWaitForConnection == true.
    if (SUCCEEDED(hr))
    {
        hr = (TRUE == ResetEvent(m_mainThreadTimeoutHandle)) ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        hr = XOnlineLogon(pUsers, m_pdwServices, m_dwServicesCount, m_logonEventHandle, (PXONLINETASK_HANDLE) &m_taskHandle);
        m_lastHR = hr;
    }

    // Decode the return code.
    switch (hr)
    {
    case XONLINE_E_LOGON_SERVICE_UNAVAILABLE:
    case XONLINE_E_LOGON_CONNECTION_LOST:
    case XONLINE_E_LOGON_UPDATE_REQUIRED:
    case XONLINE_E_LOGON_XBOX_ACCOUNT_INVALID:
    case XONLINE_E_LOGON_USER_ACCOUNT_INVALID:
    case XONLINE_E_LOGON_NO_SUBSCRIPTION:
        {
            break;
        }
    case S_OK:
        {
            // This is what we expect to get back from XOnlineLogon().
            // The logon is incomplete, but, so far, successful.
            break;
        }
    default:
        {
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        // Create a worker thread to pump the task.
        hr = CreateLogonWorkerThread();
    }
    
    if (SUCCEEDED(hr))
    {
        // Convert the timeout into an absolute time.
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);
        ULARGE_INTEGER ulInt;
        Assert(sizeof(ulInt) == sizeof(fileTime));
        memcpy(&ulInt, &fileTime, sizeof(ulInt));
        __int64 qInt = ulInt.QuadPart;
        qInt += (__int64) (10000000 * dwTimeout);

        // Set the timeout timer.
        if (!SetWaitableTimer(  m_timeoutHandle,
                                (const LARGE_INTEGER *) &qInt,
                                0L,
                                NULL,
                                NULL,
                                FALSE))
        {
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = (ResumeThread(m_hWorkerThreadHandle) != -1) ? S_OK : E_FAIL;
    }

    if (FAILED(hr))
    {
        // Clean up resources and return the instance to pristine state, so
        // that it can be called again.
        (void) DestroyCXLiveConnectionManager();
    }
    else
    {
        // We succeeded.  The logon is now being processed by the
        // worker thread.
        m_classState = LoggedOn;
    }

    if (SUCCEEDED(hr) && fWaitForConnection)
    {
        hr = _WaitForConnection();
    }

    // Release the lock.
    m_lClassLock = 0;

    return hr;
}

HRESULT CXLiveConnectionManager::LogonAndWaitForConnection(XONLINE_USER *pUsers, DWORD dwTimeout)
{
    return Logon(pUsers, dwTimeout, true);
}
    
HRESULT CXLiveConnectionManager::_WaitForConnection()
{
    DWORD dwWait = WaitForSingleObject(m_mainThreadTimeoutHandle, INFINITE);
    if (dwWait != WAIT_OBJECT_0)
    {
        return E_FAIL;
    }
    return m_lastHR;
}

HRESULT CXLiveConnectionManager::WaitForConnection()
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = VerifyOwnership();

    if (SUCCEEDED(hr))
    {
        hr = _WaitForConnection();
    }
    
    // Release the lock.
    m_lClassLock = 0;

    return hr;
}

    
HRESULT CXLiveConnectionManager::Logoff()
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = VerifyOwnership();

    if (SUCCEEDED(hr))
    {
        switch (m_classState)
        {
        case Ready:
            {
                // No user is logged on.
                hr = XONLINE_E_USER_NOT_LOGGED_ON;
                break;
            }
        case LoggedOn:
            {
                // The logon is currently proceding, but has not yet produced
                // a connection, so we'll wait for a connection, before
                // allowing the logoff.
                hr = _WaitForConnection();
                hr = (XONLINE_S_LOGON_CONNECTION_ESTABLISHED == hr && Connected == m_classState) ? S_OK : E_FAIL;
                if (FAILED(hr))
                {
                    break;
                }
                // The state just changed from LoggedOn to Connected.
                // Fall through to handle the Connected state.
            }
        case Connected:
            {
                // This is the expected state.
                hr = S_OK;
                break;
            }
        default:
            {
                // Should never get here!
                __assume(0);
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        // Cause the worker thread to terminate cleanly.
        m_bAbortThread = true;
        if (NULL != m_logonEventHandle)
        {
            (void) SetEvent(m_logonEventHandle);
            Sleep(0);
        }
        
        // Reinitialize the instance.
        DestroyCXLiveConnectionManager();
    }
    
    // Release the lock.
    m_lClassLock = 0;
    
    return hr;
}
    
DWORD WINAPI LogonWorkerThreadFunction(void *pArgs)
{
    DWORD dwWait = 0;
    HRESULT hr = S_OK;

    // Return the threadId so that the other thread can tell that we are alive.
    CXLiveConnectionManager::Instance().m_dwWorkerThreadId = GetCurrentThreadId();

    // Set up the WaitForMultipleObjectsEx() call.
    HANDLE handles[2];
    handles[0] = CXLiveConnectionManager::Instance().m_timeoutHandle;
    handles[1] = CXLiveConnectionManager::Instance().m_logonEventHandle;
    
    do
    {
        if (CXLiveConnectionManager::Instance().m_bAbortThread)
        {
            // Terminate this worker thread.  We won't set hr to E_ABORT,
            // because we want to return only what XOnlineTaskContinue() returned.
            CXLiveConnectionManager::Instance().m_bAbortThread = false;
            break;
        }
        
        dwWait = WaitForMultipleObjectsEx(  (DWORD)2,
                                            &handles[0],
                                            FALSE,
                                            INFINITE,
                                            TRUE);  

        if (CXLiveConnectionManager::Instance().m_bAbortThread)
        {
            // Terminate this worker thread.  We won't set hr to E_ABORT,
            // because we want to return only what XOnlineTaskContinue() returned.
            CXLiveConnectionManager::Instance().m_bAbortThread = false;
            break;
        }
        
        if (dwWait == WAIT_OBJECT_0 + 0)
        {
            // The logon has timed out.
            hr = (TRUE == SetEvent(CXLiveConnectionManager::Instance().m_mainThreadTimeoutHandle)) ? XONLINE_E_LOGON_SERVICE_UNAVAILABLE : E_FAIL;

            // Return the hr, so that the title (other thread) can get it.
            CXLiveConnectionManager::Instance().m_lastHR = hr;
        }
        
        if (SUCCEEDED(hr))
        {
            // Pump the handle.
            hr = XOnlineTaskContinue( CXLiveConnectionManager::Instance().m_taskHandle );
            
            // Return the hr, so that the title (other thread) can get it.
            CXLiveConnectionManager::Instance().m_lastHR = hr;
            
            // Decode the HRESULT.
            switch (hr)
            {
            case XONLINE_E_LOGON_SERVICE_UNAVAILABLE:
            case XONLINE_E_LOGON_NO_DNS_SERVICE:
            case XONLINE_E_LOGON_NO_IP_ADDRESS:
                {
                    // Return hr and terminate the thread.
                    CXLiveConnectionManager::Instance().m_lastHR = hr;
                    SetEvent(CXLiveConnectionManager::Instance().m_mainThreadTimeoutHandle);
                    break;
                }
            case XONLINETASK_S_RUNNING:
                {
                    continue;
                }
            case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
                {
                    CXLiveConnectionManager::Instance().m_classState = CXLiveConnectionManager::Connected;
                    hr = (TRUE == SetEvent(CXLiveConnectionManager::Instance().m_mainThreadTimeoutHandle)) ? S_OK : E_FAIL;
                    continue;
                }
            default:
                {
                    break;
                }
            }
        }
    } while (SUCCEEDED(hr));
    
    // Prepare to terminate the thread.
    CXLiveConnectionManager::Instance().m_dwWorkerThreadId = 0;
    DWORD dwExitCode = (DWORD) hr;
    ExitThread(dwExitCode);
}

HRESULT CXLiveConnectionManager::CreateLogonWorkerThread()
{
    HRESULT hr = S_OK;
    DWORD dwThreadId = 0;

    if (SUCCEEDED(hr))
    {
        m_hWorkerThreadHandle = CreateThread(
                                                NULL,
                                                8192,
                                                (LPTHREAD_START_ROUTINE) LogonWorkerThreadFunction,
                                                NULL,
                                                CREATE_SUSPENDED,
                                                &dwThreadId);
        hr = (m_hWorkerThreadHandle == NULL) ? E_FAIL : S_OK;
    }

    if (SUCCEEDED(hr))
    {
        hr = (TRUE == SetThreadPriority(m_hWorkerThreadHandle, THREAD_PRIORITY_ABOVE_NORMAL)) ? S_OK : E_FAIL;
    }

    return hr;
}

HRESULT CXLiveConnectionManager::SelectServices(DWORD svc)
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = VerifyOwnership();
    
    hr = (Ready == m_classState) ? hr : E_FAIL;
    
    if (SUCCEEDED(hr))
    {
        m_dwServicesCount = 0;
        m_dwServices[m_dwServicesCount++] = svc;
        m_pdwServices = &m_dwServices[0];
    }
    
    // Release the lock.
    m_lClassLock = 0;

    return hr;
}

HRESULT CXLiveConnectionManager::SelectServices(DWORD svc1, DWORD svc2)
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = VerifyOwnership();
    
    hr = (Ready == m_classState) ? hr : E_FAIL;
    
    if (SUCCEEDED(hr))
    {
        m_dwServicesCount = 0;
        m_dwServices[m_dwServicesCount++] = svc1;
        m_dwServices[m_dwServicesCount++] = svc2;
        m_pdwServices = &m_dwServices[0];
    }
    
    // Release the lock.
    m_lClassLock = 0;

    return hr;
}

HRESULT CXLiveConnectionManager::SelectServices(DWORD svc1, DWORD svc2, DWORD svc3)
{
    while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
    {
        Sleep(0);
    }
    
    HRESULT hr = VerifyOwnership();

    hr = (Ready == m_classState) ? hr : E_FAIL;
    
    if (SUCCEEDED(hr))
    {
        m_dwServicesCount = 0;
        m_dwServices[m_dwServicesCount++] = svc1;
        m_dwServices[m_dwServicesCount++] = svc2;
        m_dwServices[m_dwServicesCount++] = svc3;
        m_pdwServices = &m_dwServices[0];
    }
    
    // Release the lock.
    m_lClassLock = 0;

    return hr;
}

HRESULT CXLiveConnectionManager::SelectServices(DWORD *pServices, DWORD dwServicesCount)
{
    HRESULT hr = S_OK;

    if (dwServicesCount < 0 || dwServicesCount >= XONLINE_MAX_NUMBER_SERVICE)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        while (InterlockedCompareExchange(&m_lClassLock, 1L, 0L))
        {
            Sleep(0);
        }
        
        hr = VerifyOwnership();
        
        if (SUCCEEDED(hr))
        {
            hr = (Ready == m_classState) ? hr : E_FAIL;
        }
        
        if (SUCCEEDED(hr))
        {
            if (dwServicesCount == 0)
            {
                m_pdwServices = NULL;
                m_dwServicesCount = 0;
            }
            else
            {
                m_dwServicesCount = dwServicesCount;
                m_pdwServices = &m_dwServices[0];
                memcpy(m_pdwServices, pServices, m_dwServicesCount * sizeof(DWORD));
            }
        }
        
        // Release the lock.
        m_lClassLock = 0;
    }
    
    return hr;
}


//
/////////////////////////////////////////////////////////
//

void __cdecl DestroyCXLiveConnectionManager( void )
{
    CXLiveConnectionManager::Instance().~CXLiveConnectionManager();
}


    
}   // namespace XLive

