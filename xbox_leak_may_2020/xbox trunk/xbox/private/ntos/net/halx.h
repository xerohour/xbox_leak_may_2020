// ---------------------------------------------------------------------------------------
// halx.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef _HALX_H_
#define _HALX_H_

#ifdef XNET_FEATURE_XBDM_SERVER
#include <dm.h>
#endif

// ---------------------------------------------------------------------------------------
// CXnHal
// ---------------------------------------------------------------------------------------

class CXnHal : public CXnVoid
{
    // Definitions -----------------------------------------------------------------------

    #define EvtInit(prke)               KeInitializeEvent(prke, NotificationEvent, FALSE)
    #define EvtTerm(prke)
    #define EvtSet(prke, pri)           KeSetEvent((prke), (pri), FALSE)
    #define EvtClear(prke)              KeClearEvent(prke)
    #define EvtRef(ke)                  &(ke)
    #define EvtDereference(prke)        ObDereferenceObject(prke)

#ifdef XNET_FEATURE_ASSERT
    #define USER                        0x0001
    #define UDPC                        0x0002
    #define SDPC                        0x0004
    #define TCHECK_(pXnBase, lev)       AssertSz((pXnBase)->HalThreadCheck(lev), "Not allowed to execute this code at the current level")
#else
    #define TCHECK_(pXnBase, lev)
#endif
    #define TCHECK(lev)                 TCHECK_(this, lev)

#ifdef XNET_FEATURE_ASSERT
    #define ICHECK_(pXnBase, lay, lev)  AssertSz((pXnBase)->TestInitFlag(INITF_##lay), "The " #lay " layer is not initialized"); \
                                        AssertSz(!(pXnBase)->TestInitFlag(INITF_##lay##_TERM), "The " #lay " layer has been terminated."); \
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

    INLINE PRKEVENT EvtFromHandle(HANDLE hEvent)
    {
        PRKEVENT kEvent;
        return(hEvent && ObReferenceObjectByHandle(hEvent, ExEventObjectType, (VOID**)&kEvent) == STATUS_SUCCESS ? kEvent : NULL);
    }

    INLINE BOOL EvtWait(PRKEVENT prke, UINT timeout)
    {
        LARGE_INTEGER waittime;
        waittime.QuadPart = Int32x32To64(timeout, -10000);
        return(KeWaitForSingleObject(prke, UserRequest, UserMode, FALSE, timeout ? &waittime : NULL) == STATUS_SUCCESS);
    }

#ifdef XNET_FEATURE_ASSERT
    BOOL                HalThreadCheck(UINT uiLevel);
    KIRQL               KeRaiseIrqlToDpcLevel();
    void                KeLowerIrql(KIRQL kirql);
#endif

    void                SetInitFlag(DWORD dwFlag);
    INLINE BOOL         TestInitFlag(DWORD dwFlag)      { return(!!(_dwInitFlags & dwFlag)); }

    UINT                HalRandGather(BYTE * pb, UINT cb);
    void                HalRandPersist(BYTE * pb, UINT cb);

protected:

    NTSTATUS            HalInit(XNetInitParams * pxnip) { return(NETERR_OK); }
    INLINE void         HalStart()  {};
    void                HalStop()   {};
    void                HalTerm()   {};

    // Data ------------------------------------------------------------------------------

    DWORD                       _dwInitFlags;

#ifdef XNET_FEATURE_ASSERT
    BOOL                        _fUserDpc;              // TRUE when user raised IRQL to DPC
#endif

};

// ---------------------------------------------------------------------------------------
// HAL System Memory
// ---------------------------------------------------------------------------------------

INLINE void * HalAlloc(size_t size, ULONG tag)
{
    return(ExAllocatePoolWithTag(size, tag));
}

INLINE void * HalAllocZ(size_t size, ULONG tag)
{
    void * pv = HalAlloc(size, tag);

    if (pv)
    {
        memset(pv, 0, size);
    }

    return(pv);
}

INLINE void HalFree(void * pv)
{
    if (pv)
    {
        ExFreePool(pv);
    }
}

// ---------------------------------------------------------------------------------------
// HAL Dma Memory
// ---------------------------------------------------------------------------------------

INLINE void * HalDmaAlloc(UINT cb, ULONG ulMax = MAXULONG_PTR)
{
    return(MmAllocateContiguousMemoryEx(cb, 0, ulMax, 0, PAGE_READWRITE));
}

INLINE void HalDmaFree(void * pv)
{
    MmFreeContiguousMemory(pv);
}

// ---------------------------------------------------------------------------------------
// CRaiseToDpc
// ---------------------------------------------------------------------------------------

class CRaiseToDpc
{

public:

    INLINE CRaiseToDpc(CXnHal * pXnHal)
    {
#ifdef XNET_FEATURE_ASSERT
        _pXnHal = pXnHal;
        _irql = pXnHal->KeRaiseIrqlToDpcLevel();
#else
        _irql = KeRaiseIrqlToDpcLevel();
#endif
    }

    INLINE ~CRaiseToDpc()
    {
#ifdef XNET_FEATURE_ASSERT
        _pXnHal->KeLowerIrql(_irql);
#else
        KeLowerIrql(_irql);
#endif
    }

private:

#ifdef XNET_FEATURE_ASSERT
    CXnHal * _pXnHal;
#endif
    KIRQL    _irql;
};

#define RaiseToDpc()  CRaiseToDpc __RaiseToDpc__(this)

// ---------------------------------------------------------------------------------------
// Miscellaneous
// ---------------------------------------------------------------------------------------

void DbgPrintToKd(const char * psz);
void HalQueryTsc(LARGE_INTEGER * pli);

// ---------------------------------------------------------------------------------------

#endif
