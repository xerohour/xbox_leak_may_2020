/*
 *
 * dmcount.c
 *
 * performance counters
 *
 */

#include "dmp.h"
#include "d3d8.h"
#include "d3d8perf.h"

OBJECT_TYPE obtCounter = { DmAllocatePoolWithTag, DmFreePool, NULL, NULL, NULL, &kevtNull, 'cpmd' };
HANDLE hCounterDir;
KTIMER tmrRefreshCounters;
KDPC dpcRefreshCounters;
HANDLE hpobGPU;

LIST_ENTRY rgleUpdateQueues[10];
int ileCur;
enum { iulCpuUse, iulCpuDpc, iulCpuIsr, iulCpuMax };
ULONG rgulSystemTimes[iulCpuMax * 2];

enum { iulMemFree, iulMemCache, iulMemImage, iulMemPool, iulMemStack,
    iulMemVM, iulMemMax };
ULONG rgulPages[iulMemMax];
const char *rgszMemCounters[] = {
    "Free Pages",
    "Cache Pages",
    "Image Pages",
    "Pool Pages",
    "Stack Pages",
    "VM Pages"
};

struct {
    ULONG cbitShift;
    LPCSTR szName;
    ULONG ulData[2];
} rggcGPU[] = {
    { 0, "% GPU" },
    { 3, "% GPU Frontend" },
    { 27, "% GPU Backend" },
};
#define cgcGPU (sizeof rggcGPU / sizeof rggcGPU[0])

const OCHAR oszCounterDir[] = OTEXT("\\DmCounters");

static HRESULT HrGetFrameCount(PLARGE_INTEGER pli, PLARGE_INTEGER unused)
{
    if(!g_dmgd.FrameCounter)
        return XBDM_COUNTUNAVAILABLE;
    pli->HighPart = 0;
    pli->LowPart = *g_dmgd.FrameCounter;
    return XBDM_NOERR;
}

BOOL FGetCounterValue(PFC *ppfc, PLARGE_INTEGER pliCount, PLARGE_INTEGER pliRate)
{
    _asm {
        pushfd
        cli
    }
    switch(ppfc->dwType >> 16) {
    case DMCOUNT_SYNC>>16:
        if(SUCCEEDED(ppfc->pfnCount(pliCount, pliRate)))
            break;
        // fall through
    case 0:
        _asm popfd
        return FALSE;
    case DMCOUNT_ASYNC32>>16:
        pliCount->HighPart = 0;
        pliCount->LowPart = *ppfc->pul;
        if(pliRate)
            pliRate->QuadPart = ppfc->pul[1];
        break;
    case DMCOUNT_ASYNC64>>16:
        *pliCount = *ppfc->pli;
        if(pliRate)
            *pliRate = ppfc->pli[1];
        break;
    }
    _asm popfd
    return TRUE;
}

void RefreshCounter(PFC *ppfc)
{
    BOOL f;
    LARGE_INTEGER liValue;
    ULONG ulTickCur;
    ULONG ulFrameCur;
    int culFill;

    /* Get the current value of this counter */
    _asm {
        pushfd
        cli
    }
    if((ppfc->dwType & DMCOUNT_COUNTTYPE) == DMCOUNT_PRATIO) {
        LARGE_INTEGER li;
        f = FGetCounterValue(ppfc, &liValue, &li);
        if(f) {
            if(li.HighPart)
                f = FALSE;
            else
                ulTickCur = li.LowPart;
        }
    } else {
        f = FGetCounterValue(ppfc, &liValue, NULL);
        ulTickCur = KeQueryTickCount();
    }
    ulFrameCur = g_dmgd.FrameCounter ? *g_dmgd.FrameCounter : 0;
    _asm popfd
    if(!f) {
        /* The value is unavailable, so we reset the counter */
        ppfc->iulCur = -1;
        return;
    }

    /* Record the data */
    if(ppfc->iulCur < 0) {
        /* This is the first time we've gotten this value, so we need to
         * populate it into all of the sample slots */
        culFill = 11;
        ppfc->iulCur = 0;
    } else
        culFill = ppfc->dwType & DMCOUNT_FREQUENCY;

    /* Record the samples into the counter, into the current slot and into the
     * number of slots that will pass by prior to the next sample */
    while(culFill--) {
        if(++ppfc->iulCur == 11)
            ppfc->iulCur = 0;
        ppfc->rgliValues[ppfc->iulCur] = liValue;
        ppfc->rgulTicks[ppfc->iulCur] = ulTickCur;
        ppfc->rgulFrames[ppfc->iulCur] = ulFrameCur;
    }
}

void CounterDpc(PKDPC pdpc, PVOID pv1, PVOID pv2, PVOID pv3)
{
    PFC *ppfc;
    PFC *ppfcNext;
    int ile;
    PKPRCB pprcb = KeGetCurrentPrcb();
    struct {
        ULONG tickIdle;
        ULONG tickDpc;
        ULONG tickIsr;
        ULONG tickTotal;
    } tmsSystem;
    MM_STATISTICS mmst;

    /* First, update the system times */
    _asm {
        pushfd
        cli
    }
    tmsSystem.tickIdle = pprcb->IdleThread->KernelTime;
    //tmsSystem.tickIsr = pprcb->InterruptTime;
    tmsSystem.tickDpc = pprcb->DpcTime;
    tmsSystem.tickTotal = KeQueryTickCount();
    _asm popfd
    rgulSystemTimes[iulCpuUse*2] = tmsSystem.tickTotal - tmsSystem.tickIdle;
    rgulSystemTimes[iulCpuUse*2+1] = tmsSystem.tickTotal;
    rgulSystemTimes[iulCpuDpc*2] = tmsSystem.tickDpc;
    rgulSystemTimes[iulCpuDpc*2+1] = tmsSystem.tickTotal;
    rgulSystemTimes[iulCpuIsr*2] = tmsSystem.tickIsr;
    rgulSystemTimes[iulCpuIsr*2+1] = tmsSystem.tickTotal;

    /* Update memory usage */
    // TODO -- point counters directly at kernel's statistics
    mmst.Length = sizeof mmst;
    MmQueryStatistics(&mmst);
    rgulPages[iulMemFree] = mmst.AvailablePages;
    rgulPages[iulMemCache] = mmst.CachePagesCommitted;
    rgulPages[iulMemImage] = mmst.ImagePagesCommitted;
    rgulPages[iulMemPool] = mmst.PoolPagesCommitted;
    rgulPages[iulMemStack] = mmst.StackPagesCommitted;
    rgulPages[iulMemVM] = mmst.VirtualMemoryBytesCommitted >> PAGE_SHIFT;

    /* Walk the list of counters to be refreshed during this cycle and update
     * them, requeueing as appropriate */
    ppfc = (PFC *)rgleUpdateQueues[ileCur].Flink;
    ASSERT((PVOID)ppfc == (PVOID)&ppfc->leUpdate);
    if(ppfc == (PFC *)&rgleUpdateQueues[ileCur])
        ppfc = NULL;
    else {
        rgleUpdateQueues[ileCur].Blink->Flink = NULL;
        InitializeListHead(&rgleUpdateQueues[ileCur]);
    }

    while(ppfc) {
        ppfcNext = (PFC *)ppfc->leUpdate.Flink;
        if(ppfc->dwType & DMCOUNT_SYNCCD)
            ppfc->pfnCountData(0, ppfc, NULL);
        else
            RefreshCounter(ppfc);
        ile = ileCur + (ppfc->dwType & DMCOUNT_FREQUENCY);
        if(ile >= 10)
            ile -= 10;
        InsertTailList(&rgleUpdateQueues[ile], &ppfc->leUpdate);
        ppfc = ppfcNext;
    }

    if(++ileCur == 10)
        ileCur = 0;
}

void InitCounters(void)
{
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING ost;
    NTSTATUS st;
    LARGE_INTEGER li;

    /* Initialize the refresh queues */
    for(ileCur = 0; ileCur < 10; ++ileCur)
        InitializeListHead(&rgleUpdateQueues[ileCur]);
    ileCur = 0;

    /* Initialize the refresh timer */
    KeInitializeDpc(&dpcRefreshCounters, CounterDpc, NULL);
    KeInitializeTimer(&tmrRefreshCounters);

    /* Start it ticking right away */
    li.HighPart = li.LowPart = -1;
    KeSetTimerEx(&tmrRefreshCounters, li, 100, &dpcRefreshCounters);

    /* Create the performance counter object directory */
    RtlInitObjectString(&ost, oszCounterDir);
    InitializeObjectAttributes(&oa, &ost, OBJ_PERMANENT, NULL, NULL);
    st = NtCreateDirectoryObject(&hCounterDir, &oa);
    if(!NT_SUCCESS(st)) {
        DbgPrint("Failed to create counter object directory!\n");
        hCounterDir = NULL;
    } else {
        int i;

        /* Register the frame counter */
        DmRegisterPerformanceCounter("Frames", DMCOUNT_FREQ100MS |
            DMCOUNT_EVENT | DMCOUNT_SYNC, HrGetFrameCount);

        /* Register the CPU counters */
        DmRegisterPerformanceCounter("% CPU:total", DMCOUNT_FREQ100MS |
            DMCOUNT_PRATIO | DMCOUNT_ASYNC32, &rgulSystemTimes[iulCpuUse*2]);
        DmRegisterPerformanceCounter("% CPU:DPC", DMCOUNT_FREQ100MS |
            DMCOUNT_PRATIO | DMCOUNT_ASYNC32, &rgulSystemTimes[iulCpuDpc*2]);
        DmRegisterPerformanceCounter("% CPU:Interrupts", DMCOUNT_FREQ100MS |
            DMCOUNT_PRATIO | DMCOUNT_ASYNC32, &rgulSystemTimes[iulCpuIsr*2]);

        /* Register the memory counters */
        for(i = 0; i < iulMemMax; ++i)
            DmRegisterPerformanceCounter(rgszMemCounters[i],
                DMCOUNT_FREQ100MS | DMCOUNT_VALUE | DMCOUNT_ASYNC32,
                &rgulPages[i]);

        /* Register the GPU counters */
        for(i = 0; i < cgcGPU; ++i)
            DmRegisterPerformanceCounter(rggcGPU[i].szName,
                DMCOUNT_FREQ100MS | DMCOUNT_PRATIO | DMCOUNT_ASYNC32,
                rggcGPU[i].ulData);
    }
}

HRESULT DmRegisterPerformanceCounter(LPCSTR szName, DWORD dwType, PVOID pv)
{
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING ost;
    NTSTATUS st;
    PFC *ppfc;
    HANDLE h;
    int ile;
    DWORD grbit;

    if(!hCounterDir)
        return E_OUTOFMEMORY;

    if (!szName)
        return E_INVALIDARG;

    ile = dwType & DMCOUNT_FREQUENCY;
    /* Do some sanity checking on the counter type */
    if(ile > DMCOUNT_FREQ1SEC)
        return XBDM_BADCOUNTTYPE;
    
    RtlInitObjectString(&ost, szName);
    InitializeObjectAttributes(&oa, &ost, OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
        hCounterDir, NULL);

    st = ObCreateObject(&obtCounter, &oa, sizeof(PFC), (PVOID *)&ppfc);
    if(NT_SUCCESS(st)) {
        RtlZeroMemory(ppfc, sizeof(PFC));
        st = ObInsertObject(ppfc, &oa, 0, &h);
    }
    if(NT_SUCCESS(st)) {
        ppfc->dwType = dwType;
        ppfc->pv = pv;
        ppfc->iulCur = -1;
        /* If this counter has a periodic update, we need to queue it */
        if(ile) {
            _asm {
                pushfd
                cli
            }
            ile += ileCur;
            if(ile >= 10)
                ile -= 10;
            InsertTailList(&rgleUpdateQueues[ile], &ppfc->leUpdate);
            _asm popfd
        }
        NtClose(h);
    }
    
    return NT_SUCCESS(st) ? XBDM_NOERR : HrFromStatus(st, E_FAIL);
}

HRESULT DmUnregisterPerformanceCounter(LPCSTR szName){
    HRESULT hr;
    HANDLE h;

    hr = DmOpenPerformanceCounter(szName, &h);
    if(SUCCEEDED(hr)){
        PFC *ppfc;
        NTSTATUS st = ObReferenceObjectByHandle(h, NULL, &ppfc);
        hr = NT_SUCCESS(st) ? XBDM_NOERR : HrFromStatus(st, E_FAIL);

        if(SUCCEEDED(hr)){
            int ile = ppfc->dwType & DMCOUNT_FREQUENCY;
            /* If this counter has a periodic update, we need to dequeue it */
            if(ile) {
                _asm {
                    pushfd
                    cli
                }
                RemoveEntryList(&ppfc->leUpdate);
                _asm popfd
            }
            //Mark as temporary
            ObMakeTemporaryObject(ppfc);
            // Trash type, so nobody tries to use it
            _asm {
                pushfd
                cli
            }
            ppfc->dwType = 0;
            _asm popfd
            ObDereferenceObject(ppfc); // Release reference acquired in ObReferenceObjectByHandle
        }
        NtClose(h);
    }
    return hr;
}

HRESULT DmOpenPerformanceCounter(LPCSTR szName, PHANDLE ph)
{
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING ost;
    NTSTATUS st;

    if(!hCounterDir || !szName)
        return XBDM_NOSUCHFILE;

    RtlInitObjectString(&ost, szName);
    InitializeObjectAttributes(&oa, &ost, OBJ_CASE_INSENSITIVE, hCounterDir,
        NULL);
    st = ObOpenObjectByName(&oa, &obtCounter, NULL, ph);
    return NT_SUCCESS(st) ? XBDM_NOERR : XBDM_CANNOTACCESS;
}


HRESULT DmQueryPerformanceCounterHandle(HANDLE h, DWORD dwType, PDM_COUNTDATA pdmcd)
{
    NTSTATUS st;
    PFC *ppfc;
    HRESULT hr;
    int cul = dwType & DMCOUNT_FREQUENCY;
    int grbitType;

    if (!pdmcd)
        return E_INVALIDARG;

    st = ObReferenceObjectByHandle(h, &obtCounter, (PVOID *)&ppfc);
    if(NT_SUCCESS(st)) {
        grbitType = ppfc->dwType & DMCOUNT_COUNTTYPE;
        if(ppfc->dwType & DMCOUNT_ASYNC) {
            /* Disable interrupts so nobody updates this counter while
             * we're copying it */
            _asm {
                pushfd
                cli
            }
            memcpy(pdmcd, ppfc->pdmcd, sizeof *pdmcd);
            _asm popfd
            hr = XBDM_NOERR;
        } else if(ppfc->dwType & DMCOUNT_SYNCCD) {
            /* Call this counter's data function to fill in the actual
             * data */
            hr = ppfc->pfnCountData(dwType, ppfc, pdmcd);
        } else if(((dwType ^ ppfc->dwType) & DMCOUNT_COUNTTYPE) != 0) {
            /* Types must match */
            hr = XBDM_BADCOUNTTYPE;
        } else if((dwType & DMCOUNT_COUNTSUBTYPE) == 0 && grbitType !=
            DMCOUNT_PRATIO)
        {
            /* We want this counter's raw data, so we get it */
            if(FGetCounterValue(ppfc, &pdmcd->CountValue, grbitType ==
                DMCOUNT_PRATIO ? &pdmcd->RateValue : NULL))
            {
                hr = XBDM_NOERR;
                pdmcd->CountType = grbitType;
#if 0
                /* If the counter is a percent counter, then we need to scale
                 * it */
                if((pdmcd->CountType & (DMCOUNT_EVENT | DMCOUNT_PERCENT)) ==
                    DMCOUNT_PERCENT)
                {
                    pdmcd->RateValue.LowPart = -1;
                    pdmcd->RateValue.HighPart = 0;
                }
#endif
            } else
                hr = XBDM_COUNTUNAVAILABLE;
        } else if(grbitType != DMCOUNT_VALUE) {
            int iulBase;

            /* This is an event rate counter or a progressive ratio counter.
             * We need to calculate the event count delta and the time base
             * (time or frame) delta */
            if(cul == 0)
                cul = 10;
            _asm {
                pushfd
                cli
            }
            if(ppfc->iulCur < 0)
                hr = XBDM_COUNTUNAVAILABLE;
            else {
                iulBase = ppfc->iulCur - cul;
                if(iulBase < 0)
                    iulBase += 11;
                pdmcd->CountValue.QuadPart =
                    ppfc->rgliValues[ppfc->iulCur].QuadPart -
                    ppfc->rgliValues[iulBase].QuadPart;
                if((dwType & (DMCOUNT_PERFRAME | DMCOUNT_COUNTTYPE)) ==
                    (DMCOUNT_PERFRAME | DMCOUNT_EVENT))
                {
                    pdmcd->RateValue.QuadPart =
                        ppfc->rgulFrames[ppfc->iulCur] - 
                        ppfc->rgulFrames[iulBase];
                    pdmcd->CountType = DMCOUNT_PERFRAME | DMCOUNT_EVENT;
                } else {
                    pdmcd->RateValue.QuadPart =
                        ppfc->rgulTicks[ppfc->iulCur] - 
                        ppfc->rgulTicks[iulBase];
                    if(grbitType == DMCOUNT_EVENT && !(dwType &
                        DMCOUNT_PERTICK))
                    {
                        /* The value we've fetched isn't really the number of
                         * frames per the given number of milliseconds; it's
                         * the number of frames per the given number of ticks.
                         * We need to do the conversion.  Because of all the
                         * multiplication we have to do, we try to optimize
                         * for the case where the rate is a handy multiple
                         * of milliseconds so we can avoid letting the numbers
                         * get too big */
                        ULONG ulN, ulD;
                        ulN = 10000;
                        ulD = *KeTimeIncrement;
                        if(ulD == ulN)
                            ulN = ulD = 1;
                        if(dwType & DMCOUNT_PERSEC) {
                            if(ulN != 1 && pdmcd->RateValue.LowPart % 1000 == 0)
                                pdmcd->RateValue.LowPart /= 1000;
                            else if(ulN != 1 && pdmcd->RateValue.LowPart % 100 == 0) {
                                pdmcd->RateValue.LowPart /= 100;
                                ulN *= 10;
                            } else
                                ulN *= 1000;
                            pdmcd->CountType = grbitType | DMCOUNT_PERSEC;
                        } else
                            pdmcd->CountType = grbitType | DMCOUNT_PERMSEC;
                        pdmcd->CountValue.QuadPart *= ulN;
                        pdmcd->RateValue.QuadPart *= ulD;
                    } else if(grbitType == DMCOUNT_PRATIO)
                        pdmcd->CountType = grbitType;
                    else
                        pdmcd->CountType = DMCOUNT_EVENT | DMCOUNT_PERTICK;
                }
                pdmcd->CountType |= cul;
                hr = pdmcd->RateValue.QuadPart ? XBDM_NOERR :
                    XBDM_COUNTUNAVAILABLE;
            }
            _asm popfd
        } else {
            int iul;

            /* This is a value counter moving average */
            if(cul == 0)
                /* We need a value which will always correspond to the natural
                 * refresh frequency of this counter -- we know 1 will always
                 * work */
                cul = 1;
            pdmcd->CountValue.QuadPart = 0;
            pdmcd->RateValue.QuadPart = cul;
            _asm {
                pushfd
                cli
            }
            if(ppfc->iulCur < 0)
                hr = XBDM_COUNTUNAVAILABLE;
            else {
                for(iul = ppfc->iulCur; cul--; --iul) {
                    if(iul < 0)
                        iul += 11;
                    pdmcd->CountValue.QuadPart +=
                        ppfc->rgliValues[iul].QuadPart;
                }
                pdmcd->CountType = ppfc->dwType & 0xffff;
            }
            _asm popfd
        }
        ObDereferenceObject(ppfc);
    } else
        hr = HrFromStatus(st, E_FAIL);

    return hr;
}

HRESULT DmClosePerformanceCounter(HANDLE h)
{
    NTSTATUS st = NtClose(h);
    return NT_SUCCESS(st) ? XBDM_NOERR : HrFromStatus(st, E_FAIL);
}

HRESULT DmWalkPerformanceCounters(PDM_WALK_COUNTERS *ppdmwc, PDM_COUNTINFO pdmci)
{
    NTSTATUS st;
    DWORD cch;
    struct {
        OBJECT_DIRECTORY_INFORMATION odi;
        OCHAR sz[256];
    } odi;
    HRESULT hr;
    HANDLE h;

    if(!pdmci || !ppdmwc)
        return E_INVALIDARG;

    if(!hCounterDir)
        return XBDM_ENDOFLIST;

    st = NtQueryDirectoryObject(hCounterDir, &odi, sizeof odi, *ppdmwc == 0,
        (PULONG)ppdmwc, NULL);
    switch(st) {
    case STATUS_NO_MORE_ENTRIES:
        hr = XBDM_ENDOFLIST;
        break;
    case STATUS_BUFFER_TOO_SMALL:
        hr = XBDM_BUFFER_TOO_SMALL;
        break;
    default:
        if(NT_SUCCESS(st)) {
            memcpy(pdmci->Name, odi.odi.Name.Buffer, odi.odi.Name.Length);
            pdmci->Name[odi.odi.Name.Length] = 0;
            hr = DmOpenPerformanceCounter(pdmci->Name, &h);
            if(SUCCEEDED(hr)) {
                PFC *ppfc;

                st = ObReferenceObjectByHandle(h, &obtCounter, (PVOID *)&ppfc);
                if(NT_SUCCESS(st)) {
                    /* Only the low 16 bits are returned as supported type 
                     * info */
                    pdmci->Type = ppfc->dwType & 0xFFFF;
                    NtClose(h);
                    hr = XBDM_NOERR;
                } else
                    hr = E_UNEXPECTED;
            } else
                hr = E_UNEXPECTED;
        } else
            hr = HrFromStatus(st, E_FAIL);
        break;
    }

    return hr;
}

HRESULT DmCloseCounters(PDM_WALK_COUNTERS pdmwc)
{
    return XBDM_NOERR;
}

void GPUPerf(PDMN_PROFINT pdmpi)
{
    volatile DWORD *pdw;
    DWORD misc;
    int igc;
    D3DPERF *PerfCounters = g_dmgd.PerfCounters;
    DWORD ProfileDataIndex;
    DWORD busyflags;

    pdw = (PVOID)((ULONG_PTR)*g_dmgd.RegisterBase + 0x00400700);
    misc = *pdw;

    // The NV_PGRAPH_STATUS register tells whether part of the gpu is busy/idle.
    // From initial testing on various apps, the three parts which seem to have
    // the most relevance are STATE (entire GPU), FE_3D, and COMBINER. Hold off
    // on reporting the other data until we find out what it means and whether
    // it is useful.

    if(PerfCounters)
        ProfileDataIndex = PerfCounters->m_ProfileSamples %
            (sizeof(PerfCounters->m_ProfileData) / sizeof(PerfCounters->m_ProfileData[0]));
    busyflags = 0;

    for(igc = 0; igc < cgcGPU; ++igc)
    {
        if(misc & (1 << rggcGPU[igc].cbitShift)) {
            ++rggcGPU[igc].ulData[0];
            busyflags |= 1 << rggcGPU[igc].cbitShift;
            if(PerfCounters)
                ++PerfCounters->m_ProfileBusyCounts[igc];
        }
        ++rggcGPU[igc].ulData[1];
    }

    if(PerfCounters) {
        PerfCounters->m_ProfileData[ProfileDataIndex] = (WORD)busyflags;

        // Bump up total sample count.
        PerfCounters->m_ProfileSamples++;
    }
}

HRESULT DmEnableGPUCounter(BOOL f)
{
    KIRQL irqlSav = KeRaiseIrqlToDpcLevel();
    HRESULT hr;

    if(f && !hpobGPU) {
        hr = DmStartProfile(&hpobGPU, 1, GPUPerf);
        if(FAILED(hr))
            hpobGPU = NULL;
    } else if(!f && hpobGPU) {
        hr = DmStopProfile(hpobGPU);
        if(SUCCEEDED(hr))
            hpobGPU = NULL;
    } else
        hr = XBDM_NOERR;
    KeLowerIrql(irqlSav);
    return hr;
}
