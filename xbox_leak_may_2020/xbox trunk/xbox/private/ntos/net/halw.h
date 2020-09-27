// ---------------------------------------------------------------------------------------
// halw.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef _HALW_H_
#define _HALW_H_

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

typedef void (*PKDEFERRED_ROUTINE) (struct _KDPC *Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

typedef struct _KDPC {
    LIST_ENTRY              DpcListEntry;
    PKDEFERRED_ROUTINE      DeferredRoutine;
    PVOID                   DeferredContext;
    PVOID                   SystemArgument1;
    PVOID                   SystemArgument2;
} KDPC, *PKDPC, *PRKDPC;

typedef struct _KTIMER {
    LIST_ENTRY              TimerListEntry;
    KDPC *                  Dpc;
    DWORD                   DueTime;
    LONG                    Period;
} KTIMER, *PKTIMER, *PRKTIMER;

// ---------------------------------------------------------------------------------------
// CXnHal
// ---------------------------------------------------------------------------------------

void *  HalAlloc(size_t cb, ULONG tag);
void *  HalAllocZ(size_t cb, ULONG tag);
void    HalFree(void * pv);

class CXnHal : public CXnVoid
{
    // Definitions -----------------------------------------------------------------------

    #define KEVENT                      HANDLE
    #define PRKEVENT                    HANDLE
    #define EvtInit(prke)               prke = CreateEvent(NULL, FALSE, FALSE, NULL)
    #define EvtTerm(prke)               CloseHandle(prke)
    #define EvtSet(prke, pri)           SetEvent(prke)
    #define EvtClear(prke)              ResetEvent(prke)
    #define EvtRef(ke)                  ke
    #define EvtFromHandle(hEvent)       hEvent
    #define EvtDereference(prke)
    #define KeQuerySystemTime(x)        GetSystemTimeAsFileTime((LPFILETIME)(x))
    #define KeQueryTickCount()          GetTickCount()

    #define PASSIVE_LEVEL               0
    #define DISPATCH_LEVEL              1
    #define USER                        0x0001
    #define UDPC                        0x0002
    #define SDPC                        0x0004

    #define XC_ONLINE_IP_ADDRESS                0
    #define XC_ONLINE_SUBNET_ADDRESS            1
    #define XC_ONLINE_DEFAULT_GATEWAY_ADDRESS   2

#ifdef XNET_FEATURE_ASSERT
    #define TCHECK_(pXnBase, lev)       AssertSz((pXnBase)->HalThreadCheck(lev), "Not allowed to execute this code at the current level")
#else
    #define TCHECK_(pXnBase, lev)
#endif
    #define TCHECK(lev)                 TCHECK_(this, lev)

#ifdef XNET_FEATURE_ASSERT
    #define ICHECK_(pXnBase, lay, lev)  AssertSz((pXnBase)->TestInitFlag(INITF_##lay), "This layer is not initialized"); \
                                        AssertSz(!(pXnBase)->TestInitFlag(INITF_##lay##_TERM), "This layer has been terminated."); \
                                        TCHECK_(pXnBase, lev)
#else
    #define ICHECK_(pXnBase, lay, lev)
#endif
    #define ICHECK(lay, lev)            ICHECK_(this, lay, lev)

    #define HAL_DECLARE_NEW_DELETE(class) \
       INLINE void * operator new(size_t cb) { return(HalAllocZ(cb, PTAG_##class)); } \
       INLINE void operator delete(void * pv) { HalFree(pv); } \
       INLINE class() {} \
       INLINE ~class() {}

    // External --------------------------------------------------------------------------

public:

    HAL_DECLARE_NEW_DELETE(CXnHal)

    KIRQL               KeGetCurrentIrql();
    KIRQL               KeRaiseIrqlToDpcLevel();
    VOID                KeLowerIrql(KIRQL kirql);
    VOID                KeInitializeDpc(PRKDPC Dpc, PKDEFERRED_ROUTINE DeferredRoutine, PVOID DeferredContext);
    BOOLEAN             KeInsertQueueDpc(PRKDPC Dpc, PVOID SystemArgument1, PVOID SystemArgument2);
    BOOLEAN             KeRemoveQueueDpc(PRKDPC Dpc);
    VOID                KeInitializeTimer(PKTIMER Timer);
    BOOLEAN             KeSetTimerEx(PKTIMER Timer, LARGE_INTEGER DueTime, LONG Period, PKDPC Dpc);
    BOOLEAN             KeCancelTimer(PKTIMER Timer);

    BOOL                HalThreadCheck(UINT uiCheck);
    void                HalEnterDpc();
    BOOL                HalTryEnterDpc();
    void                HalLeaveDpc();

    UINT                HalRandGather(BYTE * pb, UINT cb);    

    void                SetInitFlag(DWORD dwFlag);
    INLINE BOOL         TestInitFlag(DWORD dwFlag)      { return(!!(_dwInitFlags & dwFlag)); }
    INLINE BOOL         EvtWait(PRKEVENT prke, UINT uiTimeout);

    #undef  HALAPI
    #define HALAPI(ret, fname, arglist, paramlist)  INLINE ret fname arglist { return(Hal##fname paramlist); }
    HALAPILIST()

protected:

    NTSTATUS            HalInit(XNetInitParams * pxnip);
    INLINE void         HalStart()  {};
    INLINE void         HalStop()   {};
    void                HalTerm();

    // Data ------------------------------------------------------------------------------

private:

    DWORD               _dwInitFlags;           // Initialization flags (INITF_*)

protected:

    char                _achXbox[64];           // Name of this xbox

};

INLINE BOOL CXnHal::EvtWait(PRKEVENT prke, UINT uiTimeout)
{
    BOOL fResult = (WaitForSingleObject(prke, uiTimeout ? uiTimeout : INFINITE) != WAIT_TIMEOUT);

    // We need to wait for the DPC thread to finish because it most likely just signalled
    // this event, and on the Xbox the user thread would not get a chance to run until
    // the dispatch routine returned.  On windows we don't simulate dispatch
    // exclusivity, so we just raise/lower to DPC here to make sure the dispatch call
    // that signalled the event is done.

    HalEnterDpc();
    HalLeaveDpc();

    return(fResult);
}

class CRaiseToDpc
{
public:
    CRaiseToDpc(CXnHal * pXnHal);
    ~CRaiseToDpc();
private:
    CRaiseToDpc();
    CXnHal *    _pXnHal;
    UINT        _irql;
};

#define RaiseToDpc()  CRaiseToDpc __RaiseToDpc__(this)

#define HalQueryTsc(pli)    QueryPerformanceCounter(pli)

#endif
