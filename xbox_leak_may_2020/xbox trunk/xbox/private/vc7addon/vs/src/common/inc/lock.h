////////////////////////////////////////////////////////////////////////////////
// LOCK.H
//
// Helper classes for adding thread synchronization to objects.  To make any
// object 'thread safe', make it derive from CThreadLockable, which contains the
// overhead of the lock and exposes two functions, ThreadLock and ThreadUnlock.
//
// For convenience, use a local CLock object to provide scope-locking on an
// object that is CThreadLockable, such as:
//
//      CLock       lock(&MyObject);
//
// This will lock MyObject (by calling ThreadLock()) on construction, and unlock
// on destruction.
//
// If your object has its own locking mechanism, you can use the template form
// of CLock (CObjLock<T>) which makes the same calls to ThreadLock/Unlock, but
// does not require the object to derive CThreadLockable.

#pragma once
#include "dbgout.h" // ASSERT
////////////////////////////////////////////////////////////////////////////////
// CSmallLock
//
// This object is a very small version of a critical section.  It grants 'access'
// to a single thread, using an efficient wait state when the lock is contended.
// This object is 4 bytes in size, as opposed to the 24 bytes required by a
// Win32 critical section.  However, you lose two important things by using
// CSmallLock instead of a critical section:
//
//  1) A lock count is NOT maintained, so the same thread cannot request access
//     to the same lock a second time (if it does it will hang forever).
//  2) Critical sections use a semaphore (when the lock is contended) so that
//     threads requesting access to a locked critical section are queued.  This
//     object does not do this, so access to a newly unlocked object is granted
//     to the next thread awaiting access that the scheduler happens to start.
//     Plus, waiting on a semaphore is a kernel wait, which is slightly more
//     efficient.
//
// CSmallLock enters an efficient spin loop when the lock is contended.  On a
// single processor machine, this loop simply gives up its time slice; on a
// multi-processor machine, the loop 'spins' re-checking the lock state for 'a
// while' before relinquishing the processor in hopes that the thread owning
// the lock will release it, thus avoiding the expensive context switch.

class CSmallLock
{
private:
    volatile    long        m_iLock;

    // UNDONE:  Implement spin count stuff...
    long        SpinStart () { return 0; }
    void        Spin (long *piSpinCount) { if ((*piSpinCount)-- < 0) { Sleep(1); *piSpinCount = SpinStart(); } }

public:
    CSmallLock() : m_iLock(0) {}

    void    Lock ()
    {
        long    iSpin = SpinStart ();
        while (InterlockedExchange ((long *)&m_iLock, 1) != 0)
            Spin (&iSpin);
    }

    void    Unlock ()
    {
        ASSERT (m_iLock == 1);
        m_iLock = 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
// CCountLock
//
// This is a lock-count-maintaining version of CSmallLock.  It allows a thread
// to nest calls to Lock/Unlock without deadlocking.  Takes 8 bytes instead of
// 4.

class CCountLock
{
private:
    volatile    DWORD       m_dwOwner;          // Owning thread (non-zero means locked)
                long        m_iLockCount;       // Lock count

    // UNDONE:  Implement spin count stuff...
    long        SpinStart () { return 0; }
    void        Spin (long *piSpinCount) { if ((*piSpinCount)-- < 0) { Sleep(1); *piSpinCount = SpinStart(); } }

public:
    CCountLock () : m_dwOwner(0), m_iLockCount(0) {}

    long    Lock ()
    {
        long    iSpin = SpinStart ();
        DWORD   dwOld, dwTid = GetCurrentThreadId();

        while (TRUE)
        {
/*
#if defined(_WIN64) || defined(_NEW_SDK)
            dwOld = (DWORD)InterlockedCompareExchange ((long *)&m_dwOwner, (long)dwTid, 0);
#else
            dwOld = (DWORD)InterlockedCompareExchange ((void **)&m_dwOwner, (void *)dwTid, 0);
#endif
*/

        //TIMSULL try this instead so we can run on Win95   V7-43140
        dwOld = (DWORD)_InterlockedCompareExchange ((long*)&m_dwOwner, (long)dwTid, 0);

        if (dwOld == 0 || dwOld == dwTid)
                break;
            Spin (&iSpin);
        }

        return ++m_iLockCount;
    }

    long    Unlock ()
    {
        ASSERT (m_dwOwner == GetCurrentThreadId());
        ASSERT (m_iLockCount > 0);
        if (--m_iLockCount == 0)
        {
            m_dwOwner = 0;
            return 0;
        }
        return m_iLockCount;
    }

    BOOL    LockedByMe () { return m_dwOwner == GetCurrentThreadId(); }


// Win64Fix (MikhailA)
#ifndef _WIN64
//TIMSULL source for InterlockedCompareExchange   V7-43140
__forceinline
LONG
__fastcall
_InterlockedCompareExchange(
  IN OUT PLONG Destination,
  IN LONG Exchange,
  IN LONG Comperand
  )
{
  LONG lRet;
  __asm {
    mov  eax, Comperand
    mov  ecx, Destination
    mov  edx, Exchange
    lock cmpxchg [ecx], edx
    mov  lRet, eax
  }
  return lRet;
};

#endif // !_WIN64

};

////////////////////////////////////////////////////////////////////////////////
// CReadWriteLock
//
// This is a multiple-reader, single-writer lock mechanism.  Up to 4 threads can
// simultaneously acquire read permission (when no writer exists), and a single
// thread can acquire write permission (when no other readers exist).  It's also
// permitted for the same thread to obtain a read lock after obtaining a write
// lock, and vice versa (provided no other readers exist).  All locks can be
// recursively acquired.
//
// Additionally, the writer thread can mark the lock as 'write-suspended', which
// allows other threads to acquire read locks but disallows other threads to
// acquire write locks.  This is handy for protecting an object that fires
// events when changes are made to it -- the writer thread will write lock, make
// its changes, write-suspend the lock, fire the events (opening other threads to
// read the object to examine changed data but disallowing further modifications)
// and finally unlocking when all events are fired.  Suspending writes allows
// reads regardless of the number of locks the writer thread has acquired, and
// the calling thread must still unlock once for each time it acquired write
// access.  Once suspended, the writer thread must 'resume' its write lock, which
// will block until any other reader threads have 'left the building'.

class CReadWriteLock
{
private:
    enum
    {
        LF_WRITEPENDING = 0x01,     // Thread is waiting for write access
        LF_WRITESUSPENDED = 0x02,   // Writer thread has allowed reads without revoking its write privilege

        READERS = 4,                // Number of readers (keep small)
    };

    CSmallLock      m_lock;                     // Lock for access to below data
    DWORD           m_dwWriter;                 // Writer thread ID
    DWORD           m_rgdwReaders[READERS];     // Reader thread IDs
    BYTE            m_rgiReadLocks[READERS];    // Reader lock counts
    BYTE            m_iWriteLocks;              // Writer lock count
    BYTE            m_wFlags;                   // Flags

public:
    CReadWriteLock () : m_dwWriter(0), m_iWriteLocks(0), m_wFlags(0)
    {
        ZeroMemory (m_rgdwReaders, sizeof (m_rgdwReaders));
        ZeroMemory (m_rgiReadLocks, sizeof (m_rgiReadLocks));
    }

#ifdef _DEBUG
    ~CReadWriteLock () { ASSERT (m_wFlags == 0); ASSERT (m_dwWriter == 0); for (long i=0; i<READERS; i++) ASSERT (m_rgdwReaders[i] == 0); }
#endif

    void        ReadLock ();
    void        ReadUnlock ();
    void        WriteLock ();
    void        WriteUnlock ();
    BOOL        WriteSuspend ();        // NOTE:  Returns FALSE if writes were already suspended -- these are NOT cumulative!
    void        WriteResume ();
};

////////////////////////////////////////////////////////////////////////////////
// CThreadLockable

class CThreadLockable
{
    CCountLock      m_lock;

public:
    void        ThreadLock () { m_lock.Lock (); }
    void        ThreadUnlock () { m_lock.Unlock (); }
};

////////////////////////////////////////////////////////////////////////////////
// CObjLock

template <class T>
class CObjLock
{
private:
    T   *m_pObject;

public:
    CObjLock (T *p) : m_pObject(p) { m_pObject->ThreadLock (); }
    ~CObjLock () { Unlock (); }
    void    Unlock () { if (m_pObject != NULL) { m_pObject->ThreadUnlock (); m_pObject = NULL; } }
    void    Lock (T *p) { ASSERT (m_pObject == NULL || p == m_pObject); m_pObject = p; p->ThreadLock (); }
};

////////////////////////////////////////////////////////////////////////////////
// CLock

typedef CObjLock<CThreadLockable>   CLock;

