////////////////////////////////////////////////////////////////////////////////
// EVENT.H

#pragma once

////////////////////////////////////////////////////////////////////////////////
// CEvent -- wrapper for Win32 Event object

class CEvent
{
private:
    HANDLE  m_hEvent;               // Event handle

public:
    CEvent (BOOL fState = FALSE, BOOL fManual = FALSE,
            LPCTSTR pszName = NULL, SECURITY_ATTRIBUTES *psa = NULL)
    {
        m_hEvent = CreateEvent (psa, fManual, fState, pszName);
    }
    ~CEvent () { if (m_hEvent) CloseHandle (m_hEvent); }

    operator HANDLE() { return m_hEvent; }

    BOOL    Set () { return SetEvent (m_hEvent); }
    BOOL    Reset () { return ResetEvent (m_hEvent); }
    BOOL    Pulse () { return PulseEvent (m_hEvent); }
    DWORD   Wait (DWORD dw = INFINITE) { return WaitForSingleObject (m_hEvent, dw); }
};

////////////////////////////////////////////////////////////////////////////////
// CMutex -- wrapper around Win32 mutex

class CMutex
{
private:
    HANDLE  m_hMutex;

public:
    CMutex (BOOL fOwned = FALSE, PCSTR pszName = NULL, SECURITY_ATTRIBUTES *psz = NULL)
    {
        m_hMutex = CreateMutexA (psz, fOwned, pszName);
    }
    ~CMutex () { if (m_hMutex) CloseHandle (m_hMutex); }

    operator HANDLE() { return m_hMutex; }

    DWORD   Get (DWORD dwTimeout = INFINITE) { return WaitForSingleObject (m_hMutex, dwTimeout); }
    BOOL    Release () { return ReleaseMutex (m_hMutex); }
};

////////////////////////////////////////////////////////////////////////////////
// CThreadLock  -- wrapper around Win32 CRITICAL_SECTION

class CThreadLock
{
private:
    CRITICAL_SECTION    m_cs;

public:
    CThreadLock ()
#ifdef _DEBUG
    : m_iLocks(0), m_pszName("<unnamed>")
#endif
    { InitializeCriticalSection (&m_cs); }
    ~CThreadLock () { DeleteCriticalSection (&m_cs); }

#ifdef _DEBUG
    long    m_iLocks;
    PCSTR   m_pszName;
    VOID    Lock ()
    {
        //DBGOUT (("Waiting for lock %08X (%s)", this, m_pszName));
        EnterCriticalSection (&m_cs);
        //DBGOUT (("         Locked: %08X (%s) (%d locks now)", this, m_pszName, ++m_iLocks));
    }

    VOID    Unlock ()
    {
        //DBGOUT (("       Unlocked: %08X (%s) (%d locks now)", this, m_pszName, --m_iLocks));
        LeaveCriticalSection (&m_cs);
    }

#else
    VOID    Lock () { EnterCriticalSection (&m_cs); }
    VOID    Unlock () { LeaveCriticalSection (&m_cs); }
#endif
};

#ifdef _DEBUG
#define SET_THREADLOCK_NAME(lock,name) lock.m_pszName = name
#else
#define SET_THREADLOCK_NAME(lock,name) 0
#endif

////////////////////////////////////////////////////////////////////////////////
// CMsgThreadLock - same as ThreadLock but uses a mutex and uses MsgWaitFor...
// to allow message processing

class CMsgThreadLock
{
private:
    HANDLE      m_hMutex;

public:
    CMsgThreadLock () { m_hMutex = CreateMutex (NULL, FALSE, NULL); }
    ~CMsgThreadLock () { if (m_hMutex) CloseHandle (m_hMutex); }

    VOID    Lock ()
    {
        DWORD   dw;

        while (TRUE)
        {
            // REVIEW:  Only sent (not posted) messages will interrupt this...
            dw = MsgWaitForMultipleObjects (1, &m_hMutex, FALSE, 1000, QS_SENDMESSAGE);
            if (dw == WAIT_OBJECT_0 + 1 || dw == WAIT_TIMEOUT)
            {
                // A message may be in the queue.  PeekMessage will process
                // any messages SENT (not posted) without us needed to dispatch it.
                MSG msg;
                PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE);
            }
            else if (dw == WAIT_FAILED)
            {
                // Odd -- wait failure?  Now what?
                if (m_hMutex)
                    CloseHandle (m_hMutex);
                m_hMutex = CreateMutex (NULL, FALSE, NULL);
            }
            else
            {
                // Return value must have been WAIT_OBJECT_0 or WAIT_ABANDONED_0,
                // in which case we were given ownership of the mutex.
                break;
            }
        }
    }

    VOID    Unlock () { ReleaseMutex (m_hMutex); }
};


////////////////////////////////////////////////////////////////////////////////
// CEventHandler
//
// The base event handler class (abstract).  Create one of these to represent an
// event that you want the dispatcher to watch for, and place the handling code
// in HandleEvent().

class CEventHandler
{
public:
    CEventHandler () {}
    virtual ~CEventHandler () {}

    // The Win32 handle object that gets watched must be returned here.
    virtual HANDLE  GetHandle () = 0;

    // This function gets called when m_hHandle is signalled.  dwWaitRet is the
    // return value from WaitForMultipleObjects (minus the object index)
    virtual DWORD   HandleEvent (DWORD dwWaitRet) = 0;

    // This function is a "back-up" function.  If it gets called, it means that
    // the handle provided is no longer a valid waitable object handle.  If
    // TRUE is returned, GetHandle() will be called to retrieve the new handle.
    // Otherwise, this event handler will be removed from the dispatcher and
    // destroyed.
    virtual BOOL    ValidateHandle () { return FALSE; }
};

////////////////////////////////////////////////////////////////////////////////
// CEventDispatcher

class CEventDispatcher
{
private:
    HANDLE              m_hThread;          // Our thread handle
    DWORD               m_dwEvents;         // Number of events we're watching
    DWORD               m_dwAlloc;          // Amount of space allocated for events
    HANDLE              *m_rghEvents;       // The event handles
    CEventHandler       **m_rgpHandlers;    // The event handlers
    CThreadLock         m_Lock;             // The lock on the command queue
    BOOL                m_fTerminate;       // TRUE if we're supposed to die
    long                m_iSuspendCount;    // Non-zero means we're suspended

    typedef enum
    {
        DC_ADDEVENT,        // Command to add an event
        DC_REMOVEEVENT,     // Command to remove an event
        DC_SUSPEND,
        DC_RESUME,
        DC_TERMINATE        // Command to terminate event dispatcher
    } DISPCMD;

    ////////////////////////////////////////////////////////////////////////////
    // CCommand

    class CCommand
    {
    public:
        CCommand        *m_pNext;       // Next command in queue
        DISPCMD         m_cmd;          // Command
        CEventHandler   *m_pHandler;    // Event handler
        CCommand (DISPCMD cmd, CEventHandler *pHandler) : m_pNext(NULL), m_cmd(cmd), m_pHandler(pHandler) {}
    };

    ////////////////////////////////////////////////////////////////////////////
    // CCommandHandler -- event handler thunk for our own commands

    class CCommandHandler : public CEventHandler
    {
    private:
        CEventDispatcher    *m_pDispatch;
        CEvent              m_event;

    public:
        CCommandHandler (CEventDispatcher *pDispatch) : m_pDispatch(pDispatch), m_event(FALSE, TRUE) {}
        ~CCommandHandler() {}

        HANDLE      GetHandle () { return m_event; }
        DWORD       HandleEvent (DWORD dwWaitRet) { return m_pDispatch->HandleCommand (dwWaitRet); }
        void        Set () { m_event.Set(); }
        void        Reset () { m_event.Reset(); }
    };

    friend class CCommandHandler;
    CCommandHandler     *m_pCmdHandler;     // The command handler
    CCommand            *m_pCmdQueue;       // First command in queue
    CCommand            **m_ppNextCmd;      // End of queue (incoming side)

    // These are the thread entry point/management functions
    static  DWORD   ThreadEntry (CEventDispatcher *pThis) { CoInitialize(NULL); pThis->Dispatch(); CoUninitialize(); return 0; }
    void            Dispatch ();

    // Command queue handling
    HRESULT         EnqueueCommand (DISPCMD cmd, CEventHandler *pHandler);
    CCommand        *FetchCommand ();
    DWORD           HandleCommand (DWORD dwWaitRet);

    // Event list handling
    void            AddEventToList (CEventHandler *pHandler);
    void            RemoveEventFromList (CEventHandler *pHandler);
    void            ValidateEventHandles ();

public:
    CEventDispatcher();
    ~CEventDispatcher();

    HRESULT     Initialize ();

    // Add an event to the list of watched events.  NOTE:  Ownership of the event
    // is transferred to the dispatcher -- it will delete it on RemoveEvent or
    // Terminate().
    HRESULT     AddEvent (CEventHandler *pEvent);

    // Remove an event from the list of watched events.  The event will be
    // destroyed.
    HRESULT     RemoveEvent (CEventHandler *pEvent);

    // Suspend dispatching of events.  Calls must be 1-to-1 matched with calls
    // to Resume
    HRESULT     Suspend ();

    // Resume dispatching of events.  Returns S_FALSE if dispatching was not
    // suspended.
    HRESULT     Resume ();

    // Terminate the event dispatcher.  All events are destroyed, and the
    // thread is terminated.
    HRESULT     Terminate ();
};


////////////////////////////////////////////////////////////////////////////////
// CEventSource
//
// Helper class for maintaining an event source -- list of sinks, etc.  Also
// duplicates the sinks for thread-safe firing.
//
// REVIEW:  This is getting a bit big to be a template class -- look into creating
// a CEventSourceImpl class...

template <class T>
class CEventSource
{
private:
    enum { BLOCK_SIZE = 4 };

    CThreadLock     m_Lock;         // For serialization to the array.
    long            m_iSize;        // NOTE:  This is the size of the allocated array.  either 0 or > 1.
    union
    {
        T           *m_pSink;       // The sink, if we have only one
        T           **m_ppSinks;    // The array of sinks, if we have allocated more than 1
    };

protected:
    ////////////////////////////////////////////////////////////////////////////
    // CopySinks
    //
    // NOTE:  This returns a NULL-terminated array of T pointers, if there are
    // any sinks in the list; FAILS if 0 sinks, or out of memory.  Even though
    // it may fail in the 0-sink case, it may also return a zero length array (see below)

    HRESULT     CopySinks (T ***pppSinks)
    {
        HRESULT hr = S_OK;

        m_Lock.Lock();
        if (m_iSize > 0 || m_pSink != NULL)
        {
            long    iSize = (m_iSize == 0) ? 2 : m_iSize + 1;
            *pppSinks = (T **)malloc (iSize * sizeof (T *));
            if (*pppSinks == NULL)
            {
                hr = E_OUTOFMEMORY;
            }
            else
            {
                if (m_iSize == 0)
                {
                    // Just one sink
                    (*pppSinks)[0] = m_pSink;
                    m_pSink->AddRef();
                    (*pppSinks)[1] = NULL;
                }
                else
                {
                    T       **ppCopy = *pppSinks;

                    // NOTE:  This could create a zero-sized array, if we had >1 sinks
                    // at one time, but they've all been removed...
                    for (long i=0; i<m_iSize; i++)
                    {
                        if (m_ppSinks[i] != NULL)
                        {
                            *ppCopy++ = m_ppSinks[i];
                            m_ppSinks[i]->AddRef();
                        }
                    }
                    *ppCopy = NULL;
                }
            }
        }
        else
        {
            hr = E_FAIL;
        }
        m_Lock.Unlock();
        return hr;
    }

    // Use this to free the copy of sinks made by CopySinks
    static void    FreeSinkCopy (T **ppSinks)
    {
        for (long i=0; ppSinks[i] != NULL; i++)
            ppSinks[i]->Release();
        free (ppSinks);
    }

public:
    CEventSource () : m_iSize(0) { m_pSink = NULL; }
    ~CEventSource ()
    {
        m_Lock.Lock();
        if (m_iSize > 1)
        {
            for (long i=0; i<m_iSize; i++)
            {
                if (m_ppSinks[i] != NULL)
                    m_ppSinks[i]->Release();
            }

            free (m_ppSinks);
        }
        else
        {
            if (m_pSink != NULL)
                m_pSink->Release();
        }
        m_Lock.Unlock();
    }

    ////////////////////////////////////////////////////////////////////////////
    // HasSinks

    BOOL        HasSinks ()
    {
        BOOL    fRet = FALSE;

        m_Lock.Lock();

        if (m_iSize == 0)
        {
            fRet = (m_pSink != NULL);
        }
        else
        {
            for (long i=0; i<m_iSize; i++)
            {
                if (m_ppSinks[i] != NULL)
                {
                    fRet = TRUE;
                    break;
                }
            }
        }

        m_Lock.Unlock();
        return fRet;
    }

    ////////////////////////////////////////////////////////////////////////////
    // AddSink

    HRESULT     AddSink (T *pSink, DWORD_PTR *pdwCookie)
    {
        HRESULT     hr = S_OK;

        m_Lock.Lock();
        if (m_iSize == 0)
        {
            if (m_pSink != NULL)
            {
                T   *pHold = m_pSink;

                // First-time allocation of array
                m_ppSinks = (T **)malloc (BLOCK_SIZE * sizeof (T *));
                if (m_ppSinks == NULL)
                {
                    m_pSink = pHold;
                    hr = E_OUTOFMEMORY;
                }
                else
                {
                    ZeroMemory (m_ppSinks, BLOCK_SIZE * sizeof (T *));
                    m_ppSinks[0] = pHold;
                    m_ppSinks[1] = pSink;
                    pSink->AddRef();
                    m_iSize = BLOCK_SIZE;
                }
            }
            else
            {
                // First sink
                m_pSink = pSink;
                pSink->AddRef();
            }
        }
        else
        {
            ASSERT (m_iSize > 1);

            // Try to find an open space
            for (long i=0; i<m_iSize; i++)
            {
                if (m_ppSinks[i] == NULL)
                {
                    m_ppSinks[i] = pSink;
                    pSink->AddRef();
                    break;
                }
            }

            if (i == m_iSize)
            {
                // Nuts -- need to grow array
                T   **pTmp = (T**)realloc (m_ppSinks, (m_iSize + BLOCK_SIZE) * sizeof (T*));
                if (pTmp == NULL)
                {
                    hr = E_OUTOFMEMORY;
                }
                else
                {
                    ZeroMemory (pTmp + m_iSize, BLOCK_SIZE * sizeof (T*));
                    m_ppSinks = pTmp;
                    m_iSize += BLOCK_SIZE;
                    m_ppSinks[i] = pSink;
                    pSink->AddRef();
                }
            }
        }
        m_Lock.Unlock();

        if (SUCCEEDED (hr))
            *pdwCookie = (DWORD_PTR)pSink;

        return hr;
    }

    ////////////////////////////////////////////////////////////////////////////
    // RemoveSink

    HRESULT     RemoveSink (DWORD_PTR dwCookie)
    {
        HRESULT     hr = S_OK;
        T           *pSink = (T*)dwCookie;

        m_Lock.Lock();

        if (m_iSize == 0)
        {
            if (pSink == m_pSink)
            {
                m_pSink->Release();
                m_pSink = NULL;
            }
            else
                hr = E_FAIL;
        }
        else
        {
            ASSERT (m_ppSinks != NULL);
            for (long i=0; i<m_iSize; i++)
            {
                if (m_ppSinks[i] == pSink)
                {
                    m_ppSinks[i]->Release();
                    m_ppSinks[i] = NULL;
                    break;
                }
            }

            if (i == m_iSize)
                hr = E_FAIL;
        }
        m_Lock.Unlock();
        return hr;
    }

    /*
    ////////////////////////////////////////////////////////////////////////////
    // Here's some pseudocode for firing events from your subclass
    //

    void FireOnFoobar ()
    {
        T   **ppSinks;
        if (SUCCEEDED (CopySinks (&ppSinks)))
        {
            for (long i=0; ppSinks[i] != NULL; i++)
                ppSinks[i]->OnFoobar();
            FreeSinkCopy (ppSinks);
        }
    }
    */
};

/*
#define LOCKTHIS CLockMe _x(this)

////////////////////////////////////////////////////////////////////////////////
// CEventSource

template <class T>
class CEventSource
{
protected:
    ////////////////////////////////////////////////////////////////////////////////
    // CSinkNode

    class CSinkNode
    {
    public:
        CComPtr<T>      m_sp;           // Sink interface pointer
        CSinkNode       *m_pNext;       // Next sink in list

        CSinkNode(T *p, CSinkNode *pNext = NULL) : m_sp(p), m_pNext(pNext) {}
    };

    CSinkNode       *m_pSinks;          // First sink in list
    CThreadLock     m_Lock;             // Crit sec

    class CLockMe
    {
    private:
        CEventSource    *m_p;
    public:
        CLockMe (CEventSource *p) { m_p = p; p->Lock(); }
        ~CLockMe () { m_p->Unlock(); }
    };

public:
    CEventSource() : m_pSinks(NULL) {}
    ~CEventSource()
    {
        LOCKTHIS;

        while (m_pSinks != NULL)
        {
            CSinkNode   *pTmp = m_pSinks->m_pNext;
            delete m_pSinks;
            m_pSinks = pTmp;
        }
    }

    void        Lock () { m_Lock.Lock(); }
    void        Unlock () { m_Lock.Unlock(); }

    HRESULT     AddSink (T *pSink, DWORD_PTR *pdwCookie)
    {
        LOCKTHIS;

        CSinkNode   *pNew = new CSinkNode (pSink, m_pSinks);
        if (pNew == NULL)
            return E_OUTOFMEMORY;
        m_pSinks = pNew;
        *pdwCookie = (DWORD_PTR)m_pSinks;
        return S_OK;
    }

    HRESULT     RemoveSink (DWORD_PTR dwCookie)
    {
        LOCKTHIS;

        CSinkNode   **ppRun = &m_pSinks;

        while ((*ppRun != NULL) && (*ppRun != (CSinkNode *)dwCookie))
            ppRun = &(*ppRun)->m_pNext;

        if (*ppRun != NULL)
        {
            CSinkNode   *pTmp = (*ppRun)->m_pNext;
            delete *ppRun;
            *ppRun = pTmp;
            return S_OK;
        }

        return E_INVALIDARG;
    }
};
*/

////////////////////////////////////////////////////////////////////////////////
// CAccessLock
//
// This class provides multiple-reader-thread, single-writer-thread access to a
// resource.  The basic premise is to use this object in much the same way you
// would use a mutex -- wait for access, use the protected resource, and then
// release the access -- but with a 'reader count' and a 'write cookie'.
//
// To get read access to the resource, call BeginRead(); when finished, call
// EndRead(). This increments/decrements the reader count, after properly
// ensuring that the write cookie has not been granted (to another thread, not
// the caller).
//
// To get write access, call BeginWrite(), and when finished, call EndWrite().
// This grants the write cookie to the caller, after ensuring it has not been
// granted to another thread, and that no other readers exist (INCLUDING the
// calling thread).  This ALSO increments the reader count; essentially, having
// 'write' access means having 'read/write' access.
//
// If you want to release the write cookie without releasing the read access,
// call EndWrite with TRUE, indicating that the reader count should NOT be
// decremented.  Callers should then be aware that they must call EndRead() to
// fully release their access grants to the resource.
//
// Note that requests for multiple write locks, or requests for a write lock
// by a thread that already has a read lock, will IMMEDIATELY FAIL.

class CAccessLock
{
private:
    enum { TIMEOUT_VAL = 10000 };

    HANDLE          m_hNoReaders;       // 'zero-readers' event
    HANDLE          m_hWriteMutex;      // Write mutex
	HANDLE			m_hWriteSerializerMutex; // Ensures serialization of write requests
    long            m_iReaders;         // Number of readers
    unsigned        m_iWriters:31;      // Number of write grants (to the same thread, obviously)
//    unsigned        m_fWritesDenied:1;  // Set if writers exist but are relaxed

	unsigned		m_cSuspendDepth;			// # write denials

public:
    CAccessLock () : m_iReaders(0), m_iWriters(0), /*m_fWritesDenied(0),*/ m_cSuspendDepth(0)
    {
        m_hWriteMutex = CreateMutex (NULL, FALSE, NULL);
		m_hWriteSerializerMutex = CreateMutex (NULL, FALSE, NULL);
        m_hNoReaders = CreateEvent (NULL, TRUE, TRUE, NULL);
    }

    ~CAccessLock ()
    {
        if (m_hWriteMutex)
            CloseHandle (m_hWriteMutex);
        if (m_hWriteSerializerMutex)
            CloseHandle (m_hWriteSerializerMutex);
        if (m_hNoReaders)
            CloseHandle (m_hNoReaders);
    }

    ////////////////////////////////////////////////////////////////////////////
    // BeginRead

    BOOL    BeginRead ()
    {
        if (WaitForSingleObject (m_hWriteMutex, TIMEOUT_VAL) == WAIT_TIMEOUT)
        {
            ASSERT (FALSE);
            return FALSE;
        }
        if (InterlockedIncrement (&m_iReaders) == 1)
            ResetEvent (m_hNoReaders);
        ReleaseMutex (m_hWriteMutex);
        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // EndRead

    void    EndRead ()
    {
        if (InterlockedDecrement (&m_iReaders) == 0)
            SetEvent (m_hNoReaders);
    }

    ////////////////////////////////////////////////////////////////////////////
    // BeginWrite

    BOOL    BeginWrite ()
    {
        if (WaitForSingleObject (m_hWriteSerializerMutex, TIMEOUT_VAL) == WAIT_TIMEOUT)
        {
            ASSERT (FALSE);
            return FALSE;
        }

        if (WaitForSingleObject (m_hWriteMutex, TIMEOUT_VAL) == WAIT_TIMEOUT)
        {
            ASSERT (FALSE);
            return FALSE;
        }

/*        if (m_fWritesDenied)
        {
            // Sorry, charlie...
			ASSERT(FALSE); // this shouldn't ever happen!!!
            ReleaseMutex (m_hWriteMutex);
            return FALSE;
        }*/

        if (m_iWriters == 0)
        {
            // We don't already have write access, so we need to clear all the
            // readers out.
            if (WaitForSingleObject (m_hNoReaders, TIMEOUT_VAL) == WAIT_TIMEOUT)
            {
                ASSERT (FALSE);
                ReleaseMutex (m_hWriteMutex);
                return FALSE;
            }
        }

        if (InterlockedIncrement (&m_iReaders) == 1)
            ResetEvent (m_hNoReaders);
        m_iWriters++;
        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // SuspendWrite
    //
    // This function puts the lock in a state where no further write permissions
    // we be granted (even to the calling thread), but reads from ANY thread
    // will be.  Use ResumeWrite() to get back to original state.

    BOOL    SuspendWrite ()
    {
        if (WaitForSingleObject (m_hWriteMutex, TIMEOUT_VAL) == WAIT_TIMEOUT)
        {
            ASSERT (FALSE);
            return FALSE; // If we can't get it, we don't HAVE it!
        }

		if (m_cSuspendDepth++)
			goto l_done;

/*        ASSERT (!m_fWritesDenied);
        m_fWritesDenied = TRUE;*/

        // For each writer, release the write mutex
		unsigned i;
        for (i=0; i<m_iWriters; i++)
            ReleaseMutex (m_hWriteMutex);

l_done:
        // Release it one more time, which should render it signalled
        ReleaseMutex (m_hWriteMutex);
        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // ResumeWrite
    //
    // This function puts the lock back in the state at which SuspendWrite was
    // called by obtaining the write mutex for each writer.  This
    // allows threads to nest calls to BeginWrite, call SuspendWrite to allow
    // read access, and then ResumeWrite to get back to the nested state.

    BOOL    ResumeWrite ()
    {
        if (WaitForSingleObject (m_hWriteMutex, TIMEOUT_VAL) == WAIT_TIMEOUT)
        {
            ASSERT (FALSE);     // This is bad news...
            return FALSE;
        }

		ASSERT(m_cSuspendDepth > 0);
		if (--m_cSuspendDepth)
			goto l_done;

        // Make sure we're actually resuming a suspended write state
//        ASSERT (m_fWritesDenied);

        // For each writer we had previously, grab the write mutex
		unsigned i;
        for (i=0; i<m_iWriters; i++)
        {
            DWORD   dw = WaitForSingleObject (m_hWriteMutex, 0);
            ASSERT (dw == WAIT_OBJECT_0);       // We already own it!
        }

//        m_fWritesDenied = FALSE;

l_done:
        // Release the mutex that we grabbed for this function
        ReleaseMutex (m_hWriteMutex);
        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // EndWrite

    void    EndWrite ()
    {
		if (m_iWriters < 1)
		{
			ASSERT(FALSE); // mismatched BeginWrite/EndWrite calls!
			return;
		}

        m_iWriters--;
        ReleaseMutex (m_hWriteMutex);
		ReleaseMutex (m_hWriteSerializerMutex);
        EndRead ();
    }
};


struct POOLEDMUTEX
{
    HANDLE  hMutex;
    union
    {
        long    iLockCount;
        long    iNextFree;
    };
};

////////////////////////////////////////////////////////////////////////////////
// CPooledMutex
//
// A 'pooled mutex' is a locking mechanism that does not require the existence
// of an actual mutex until the object is actually locked.  Mutex objects are
// stored in a global pool, and dished out only to locked objects.  When the
// object is finally unlocked, the mutex object is returned to the pool.
//
// Benefits of Pooled Mutex over normal mutex or critical sections:
//  1) Size (this object's instance data is 2 bytes; critsec's are 24, mutex handles are 4)
//  2) Reduced system resource depletion (if numerous objects need their own
//     locks, but few are locked at once) -- comparison doesn't apply to critsec's
//
// Costs:
//  1) Speed (there's more grunt work to do, especially in the non-nested, single
//     use lock/unlock case, where we must get/release handles from/to the pool, etc.)

class CPooledMutex
{
    class CHouseKeeper
    {
    public:
        CHouseKeeper ();
        ~CHouseKeeper ();
    };
    friend class CHouseKeeper;

private:
    static  POOLEDMUTEX     *m_rghPool;         // The mutex handle pool
    static  long            m_iAllocSize;       // Allocated size of the pool
    static  long            m_iNextFree;        // Index of first available mutex
    static  CThreadLock     m_lock;             // Critsec for locking/unlocking
    static  CHouseKeeper    m_hk;               // Handles construction/destruction of pool

    static  void            Initialize ();
    static  void            Shutdown ();
    static  long            GetNextMutex ();
    static  void            RepoolMutex (long iIndex);
    static  void            Lock (short *piCookie);
    static  void            Unlock (short *piCookie);

    short                   m_iIndex;           // This instance's mutex index (-1 means not locked)

public:
    CPooledMutex () : m_iIndex(-1) {}
    ~CPooledMutex () { ASSERT (m_iIndex == -1);  } // If you hit this assert, you're destroying a locked mutex!

    void    Lock () { Lock (&m_iIndex); }
    void    Unlock () { Unlock (&m_iIndex); }
};
