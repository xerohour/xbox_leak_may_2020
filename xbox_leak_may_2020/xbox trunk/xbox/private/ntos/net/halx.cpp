// ---------------------------------------------------------------------------------------
// halx.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#ifdef XNET_FEATURE_XBOX

//
// Linker directive to merge XNET_RD section into XNET section
//
#pragma comment(linker, "/merge:XNET_RD=XNET")

// ---------------------------------------------------------------------------------------
// TCHECK
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ASSERT

KIRQL CXnHal::KeRaiseIrqlToDpcLevel()
{
    KIRQL kirql = ::KeRaiseIrqlToDpcLevel();

    if (kirql == PASSIVE_LEVEL)
    {
        _fUserDpc = TRUE;
    }

    return(kirql);
}

void CXnHal::KeLowerIrql(KIRQL kirql)
{
    if (kirql == PASSIVE_LEVEL)
    {
        _fUserDpc = FALSE;
    }

    ::KeLowerIrql(kirql);
}

BOOL CXnHal::HalThreadCheck(UINT uiCheck)
{
    KIRQL kirql = KeGetCurrentIrql();

    if (kirql != DISPATCH_LEVEL)
        return(uiCheck & USER);
    else if (_fUserDpc)
        return(uiCheck & UDPC);
    else
        return(uiCheck & SDPC);
}

#endif

// ---------------------------------------------------------------------------------------
// SetInitFlag
// ---------------------------------------------------------------------------------------

void CXnHal::SetInitFlag(DWORD dwFlag)
{
    DWORD dwFlagsOld, dwFlagsNew;

    while (1)
    {
        dwFlagsOld = _dwInitFlags;

        Assert((dwFlagsOld & dwFlag) == 0);

        dwFlagsNew = dwFlagsOld | dwFlag;

        if (InterlockedCompareExchange((LONG *)&_dwInitFlags, (LONG)dwFlagsNew, (LONG)dwFlagsOld) == (LONG)dwFlagsOld)
            break;

        TraceSz(Warning, "CXnHal::SetInitFlag: Thread/DPC contention.  Retrying.");
    }
}

// ---------------------------------------------------------------------------------------
// QueryTsc, HalQueryTsc
// ---------------------------------------------------------------------------------------

__declspec(naked) static void QueryTsc(LARGE_INTEGER * pli)
{
    _asm {
        mov ecx, [esp+4]
        rdtsc
        mov [ecx], eax
        mov [ecx+4], edx
        ret 4
    }
}

void HalQueryTsc(LARGE_INTEGER * pli)
{
    QueryTsc(pli);
}

// ---------------------------------------------------------------------------------------
// HalRandGather
// ---------------------------------------------------------------------------------------

struct CRandBytes
{
    BYTE *  _pb;
    UINT    _cb;

    INLINE void Init(BYTE * pb, UINT cb) { _pb = pb; _cb = cb; }
    INLINE void AddDword(DWORD dw) { Add(&dw, sizeof(DWORD)); }
    INLINE void AddWord(WORD w) { Add(&w, sizeof(WORD)); }
    INLINE void AddByte(BYTE b) { Add(&b, sizeof(BYTE)); }
    void Add(void * pv, UINT cb);
};

void CRandBytes::Add(void * pv, UINT cb)
{
    AssertSz(_cb >= cb, "HalRandGather has more randomness than the buffer has room");

    if (_cb >= cb)
    {
        memcpy(_pb, pv, cb);
        _pb += cb;
        _cb -= cb;
    }
}

UINT CXnHal::HalRandGather(BYTE * pb, UINT cb)
{
    LARGE_INTEGER liPerfCount;
    CRandBytes  randbytes;

    // We shouldn't need to initialize liPerfCount, but the compiler is stupid,
    // and if we don't initialize it, the compiler will fail to computs its
    // live range and will put it in the same place on the stack as randbytes.
    liPerfCount.QuadPart = 0;
    randbytes.Init(pb, cb);

    // ** indicates US DoD's specific recommendations for password generation

    // ** ticks since boot (system clock)
    DWORD dwTickBegin = NtGetTickCount(); 
    randbytes.AddDword(dwTickBegin);
    
    // ** system time
    LARGE_INTEGER liTime;
    KeQuerySystemTime(&liTime);
    randbytes.Add(&liTime, sizeof(liTime));

    // ** hi-res performance counter (system counters)
    QueryTsc(&liPerfCount);
    randbytes.AddDword(liPerfCount.LowPart ^ liPerfCount.HighPart);

    // memory status
    MM_STATISTICS mstMemStat;
    MmQueryStatistics(&mstMemStat);
    randbytes.AddWord(LOWORD(mstMemStat.AvailablePages));
    randbytes.AddWord(LOWORD(mstMemStat.PoolPagesCommitted));
    randbytes.AddByte(HIBYTE(LOWORD(mstMemStat.VirtualMemoryBytesReserved)));

#ifndef XNET_FEATURE_XBDM_SERVER

    // Free disk space
    ULARGE_INTEGER  ulintDriveFreeSpace;
    ULARGE_INTEGER  ulintTemp;
    ULARGE_INTEGER  ulintDiskFreeSpace;

    if (GetDiskFreeSpaceEx("T:", &ulintDriveFreeSpace, &ulintTemp, &ulintDiskFreeSpace))
    {
        randbytes.Add(&ulintDriveFreeSpace, sizeof(ulintDriveFreeSpace));
        randbytes.Add(&ulintDiskFreeSpace, sizeof(ulintDiskFreeSpace));
    }

#endif // XBDM

    // EEPROM values
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    NTSTATUS status;
    DWORD type, size;

    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (NT_SUCCESS(status))
    {
        Assert(type == REG_BINARY);
        Assert(size == EEPROM_TOTAL_MEMORY_SIZE);
        randbytes.Add(buf, size);
    }

    // Use hard drive model and serial number
    randbytes.Add(HalDiskModelNumber->Buffer, HalDiskModelNumber->Length);
    randbytes.Add(HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length);

    return(cb - randbytes._cb);
}

// ---------------------------------------------------------------------------------------
// DbgPrintToKd
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBDM_SERVER

void DbgPrintToKd(const char * psz)
{
	STRING st, *pst;
	st.Buffer = (PCHAR)psz;
	st.Length = (USHORT)strlen(psz);
	st.MaximumLength = st.Length + 1;
	pst = &st;
	_asm {
		mov eax, BREAKPOINT_KDPRINT
		mov ecx, pst
		int 2dh
		int 3
	}
}

#endif

// ---------------------------------------------------------------------------------------

#endif
