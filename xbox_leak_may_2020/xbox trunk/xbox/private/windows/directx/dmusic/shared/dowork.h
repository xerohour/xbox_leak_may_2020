// Copyright (c) 1998-1999 Microsoft Corporation
// DoWork.h 

#include "Alist.h"
#include "..\shared\debug.h"

class CBossMan;

typedef void (WINAPI *PWORKER_ROUTINE)(void * pWorkerState, REFERENCE_TIME *prtWakeUp );

class CWorker : public AListItem
{
    friend class CBossMan;
public:     
    CWorker *   GetNext() { return (CWorker*)AListItem::GetNext();};
    void        WakeUp() { m_rtWakeUpTime = 0; };   // Tells the worker to wake up as soon as possible.
    void        PinkSlip();   // Gets rid of the worker.
private:
    bool        HasBeenToldToDie();
    void        DoPinkSlip();   // Does the actual work of getting rid of the worker.
    bool                    m_bToldToDie;
    PWORKER_ROUTINE         m_pWorkRoutine;     // Function to call.
    void *                  m_pContext;         // Pointer to object associated with function.
    REFERENCE_TIME          m_rtWakeUpTime;     // When to wake up next.
    DWORD                   m_dwOrder;          // Where in the chain of workers this should reside.
    CBossMan *              m_pBossMan;         // Parent.
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    REFERENCE_TIME          m_rtElapsedTime;    // For tracking time elapsed
    DWORD                   m_dwElapsedTime;    // Translated for performance counter. 
    char                    m_szName[100];      // Name for display.
#endif
};

class CBossMan : public AList
{
public:
                CBossMan() { m_pMasterClock = NULL; };
                ~CBossMan();

    CWorker*    GetHead(){return (CWorker*)AList::GetHead();};
    CWorker*    RemoveHead(){return (CWorker*)AList::RemoveHead();};
//    void        InsertBefore(CWorker *pItem,CWorker *pInsert){ AList::InsertBefore((AListItem *pItem,AListItem
    void        Remove(CWorker *pItem){AList::Remove((AListItem *)pItem);};
    void        SetMasterClock(IReferenceClock *pClock);
    CWorker *   InstallWorker(PWORKER_ROUTINE pRoutine,void * pContext,DWORD dwOrder,char *pszName);
    void        DoWork(DWORD dwQuantum);
    void        WakeUp(CWorker*);
    IReferenceClock * m_pMasterClock;
};


