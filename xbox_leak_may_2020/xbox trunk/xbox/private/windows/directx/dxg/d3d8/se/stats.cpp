/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       stats.cpp
 *  Content:    implementation for debug-only statistics gathering
 *
 ***************************************************************************/

#include "precomp.hpp"
#include "dm.h"
#include <xbdm.h>

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if PROFILE

D3DPERF g_PerfCounters =
{
    // m_TSCLastResetVal
    0,

    // m_TSCLastPresentVal
    0,

    // m_FrameTimeMin
    { 0, (ULONGLONG)-1 },

    // m_FrameTimeMax
    { 0, 0 },

    // m_BreakCount
    0,

    // m_Breakperfctr
    PERF_OBJECTLOCK_WAITS,

    // m_pPutLocationAtReset
    NULL,

    // m_PushBufferEndDwordsSkipped
    0,

    // m_FrameRateIntervalTime
    0,

    // m_dwDumpFPSInfoMask
    D3DPERF_DUMP_FPS_INTINFO |
        D3DPERF_DUMP_FPS_MEMORY |
        D3DPERF_DUMP_FPS_PUSHBUFFER |
        D3DPERF_DUMP_FPS_WAITINFO |
        D3DPERF_DUMP_FPS_VERTEXINFO,

    // m_pfnDumpFrameRateInfoHandler
    D3DPERF_DumpFrameRateInfo,

    // m_DumpWaitCycleTimesThreshold
    (DWORD)-1,

    // m_pfnCycleThresholdHandler
    D3DPERF_DumpCounterCycleInfo,
};

KPRCB g_rcbAtReset;

DWORD g_CpuCyclesPerMsec = 1;  // init non-zero to avoid divide by zero, in case we can't get CPU speed

// Perf routine gpu last busy counts
DWORD g_PerfLastProfileSamples;
DWORD g_PerfLastGpuBusyCounts[D3DPERF_PROF_MAX];

// Boolean to make sure we only stop profile runs we started
static BOOL g_PerProfileStarted = FALSE;

// When true this will toggle the current vertex file/cache
// renderstate at the next D3DPERF_Reset() call. This allows
// someone to set those guys via the debugger.
extern "C" BOOL g_ToggleVTXStatesOnReset = FALSE;

// Profiler interrupt handler typedef.
typedef void (WINAPI * PROFINTHANDLER)(PKTRAP_FRAME ptf);

// Profile interrupt data.
struct D3DPERF_PROFDATA
{
    BOOL Sampling;          // currently sampling?
} g_PerfProfData;

//  Helper class for decimals since DbgPrint doesn't support %f.
class PERFFIXEDPOINT
{
public:
    PERFFIXEDPOINT(ULONGLONG Number, DWORD Base)
    {
        IntPart = (DWORD)(Number / Base);
        DecPart = (DWORD)(Number - (IntPart * Base));
    }

    DWORD IntPart;
    DWORD DecPart;
};

//------------------------------------------------------------------------------
// D3DPERF_GetPushBufferBytesWritten
//
// Calculates number of bytes written to the pushbuffer since last reset.

extern "C"
ULONGLONG WINAPI D3DPERF_GetPushBufferBytesWritten()
{
    DWORD NumJumps = g_PerfCounters.m_PerformanceCounters[PERF_PUSHBUFFER_JUMPTOBEGINNING].Count;

    ASSERT(!(g_pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER));

    if(NumJumps)
    {
        return ((BYTE *)g_pPushBuffer->m_pPut - (BYTE *)g_pDevice->m_pPushBase) +
            ((BYTE *)g_pDevice->m_pPushLimit - (BYTE *)g_PerfCounters.m_pPutLocationAtReset) +
            (ULONGLONG)(g_pDevice->m_PushBufferSize * (NumJumps - 1)) -
            g_PerfCounters.m_PushBufferEndDwordsSkipped * sizeof(DWORD);
    }
    else
    {
        ASSERT(!g_PerfCounters.m_PushBufferEndDwordsSkipped);

        return (BYTE *)g_pPushBuffer->m_pPut - (BYTE *)g_PerfCounters.m_pPutLocationAtReset;
    }
}

//------------------------------------------------------------------------------
// D3DPERF_GetPushBufferInfo
//
// Returns push buffer information.

extern "C"
void WINAPI D3DPERF_GetPushBufferInfo(
    D3DPUSHBUFFERINFO *pPushBufferInfo)
{
    pPushBufferInfo->PushBufferSize = g_pDevice->m_PushBufferSize;
    pPushBufferInfo->PushSegmentSize = g_pDevice->m_PushSegmentSize;
    pPushBufferInfo->PushSegmentCount = g_pDevice->m_PusherSegmentMask + 1;

    pPushBufferInfo->pPushBase = (DWORD*) g_pDevice->m_pPushBase;
    pPushBufferInfo->pPushLimit = (DWORD*) g_pDevice->m_pPushLimit;

    pPushBufferInfo->PushBufferBytesWritten = D3DPERF_GetPushBufferBytesWritten();
}

//------------------------------------------------------------------------------
// D3DPERF_GetStatistics
//
// Returns the performance structure which is statically allocated and should
// never be freed.

extern "C"
D3DPERF * WINAPI D3DPERF_GetStatistics()
{
    return &g_PerfCounters;
}

//------------------------------------------------------------------------------
// D3DPERF_GetNames
//
// Returns an array of names which is statically allocated.

extern "C"
D3DPERFNAMES * WINAPI D3DPERF_GetNames()
{
    return (D3DPERFNAMES *)&g_PerfNames;
}

//------------------------------------------------------------------------------
// D3DPERF_Reset
//
// Reset all the counters

extern "C"
void WINAPI D3DPERF_Reset()
{
    ASSERT(!(g_pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER));

    memset(&g_PerfCounters.m_PerformanceCounters, 0, sizeof(g_PerfCounters.m_PerformanceCounters));
    memset(&g_PerfCounters.m_APICounters, 0, sizeof(g_PerfCounters.m_APICounters));
    memset(&g_PerfCounters.m_RenderStateCounters, 0, sizeof(g_PerfCounters.m_RenderStateCounters));
    memset(&g_PerfCounters.m_TextureStateCounters, 0, sizeof(g_PerfCounters.m_TextureStateCounters));

    g_PerfCounters.m_pPutLocationAtReset = (DWORD*) g_pPushBuffer->m_pPut;
    g_PerfCounters.m_PushBufferEndDwordsSkipped = 0;

    g_rcbAtReset = *KeGetCurrentPrcb();

    g_PerfCounters.m_FrameTimeMin.Count = 0;
    g_PerfCounters.m_FrameTimeMin.Cycles = (ULONGLONG)-1;

    g_PerfCounters.m_FrameTimeMax.Count = 0;
    g_PerfCounters.m_FrameTimeMax.Cycles = 0;

    g_PerfCounters.m_TSCLastPresentVal = 0;

    // Prime any of the renderstate overrides if told to.
    if(g_PerfCounters.m_SetRenderStateOverridesOnReset)
    {
        for(int State = 0; State < D3DRS_MAX; State++)
        {
            if(g_PerfCounters.m_RenderStateOverrides[State].Override)
            {
                D3DDevice_SetRenderState((D3DRENDERSTATETYPE)State,
                    g_PerfCounters.m_RenderStateOverrides[State].Value);
            }
        }

        g_PerfCounters.m_SetRenderStateOverridesOnReset = FALSE;
    }

    // If the toggle vtx states bool is set then flip the vtx cache states.
    if(g_ToggleVTXStatesOnReset)
    {
        D3DPERF_SetState(D3DPERFSTATE_VTX_ALL, 0x80000000);

        g_ToggleVTXStatesOnReset = FALSE;
    }

#ifndef STARTUPANIMATION
    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_PERFPROFILE)
    {
        // make sure we're profiling
        D3DPERF_StartPerfProfile();

        g_PerProfileStarted = TRUE;
    }
    else if(g_PerProfileStarted)
    {
        // turn off perf profiling
        D3DPERF_StopPerfProfile();

        g_PerProfileStarted = FALSE;
    }
#endif // STARTUPANIMATION

    if(g_PerfCounters.pD3DPerfEvents)
    {
        // Disable interrupts so VBlanks don't jump in here and mess us up.
        _asm pushfd
        _asm cli

        memset(g_PerfCounters.pD3DPerfEvents, 0,
            sizeof(D3DPERFEvent) * g_PerfCounters.CountD3DPerfEvents);
        g_PerfCounters.CurrentD3DPerfEvent = 0;

        _asm popfd
    }

    // record the last reset time
    g_PerfCounters.m_TSCLastResetVal = ___D3DCYCLETIMER_GetTime();
}

//------------------------------------------------------------------------------
// D3DPERF_DumpCounterCycleInfo
//
// Default handler for WaitCycleTimeThreshold spew

extern "C"
void WINAPI D3DPERF_DumpCounterCycleInfo(
    D3DPERF_PerformanceCounters perfctr,
    ULONGLONG Cycles)
{
    if(perfctr == PERF_OBJECTLOCK_WAITS)
    {
        DbgPrint("#%Lu) %s %I64u cycles (%Lums)\n",
             g_PerfCounters.m_PerformanceCounters[perfctr].Count,
             g_PerfNames.m_PerformanceCounterNames[perfctr],
             Cycles,
             MilliSecsFromCycles(Cycles));
    }
}

//------------------------------------------------------------------------------
// D3DPERF_DumpFrameRateInfo
//
// Dumps current frame rate information.

extern "C"
void WINAPI D3DPERF_DumpFrameRateInfo()
{
    DWORD i;

    // Pause the perf profile interrupt handler so if you break into the debugger
    // after this point it doesn't keep going and tromp all over the captured
    // m_ProfileData.
    DmTell(DMTELL_GPUPERF, NULL);

    BOOL RecordD3DPerfEventsBak = g_PerfCounters.RecordD3DPerfEvents;
    g_PerfCounters.RecordD3DPerfEvents = FALSE;

    // Block here until idle as the high prio DbgPrints mess up the
    // gpu timing callbacks from here on out.
    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_PERFEVENTS)
        D3DDevice_BlockUntilIdle();

    ULONGLONG CurTime = ___D3DCYCLETIMER_GetTime();
    DWORD TotalTime = MilliSecsFromCycles(CurTime - g_PerfCounters.m_TSCLastResetVal);

    // Spit out frames per second info.

    DWORD frames = g_PerfCounters.m_APICounters[API_D3DDEVICE_PRESENT];
    PERFFIXEDPOINT fps(frames * (LONGLONG)1000000 / TotalTime, 1000);
    PERFFIXEDPOINT msecPerFrame(TotalTime * (LONGLONG)1000 / frames, 1000);

    DbgPrint("fps: %Lu.%Lu (%Lu.%Lu ms/frame)  frames: %Lu  time: %Lums\n",
        fps.IntPart, fps.DecPart,
        msecPerFrame.IntPart, msecPerFrame.DecPart,
        frames,
        TotalTime);

    DbgPrint("Min/max frame times:  frame #%Lu=%Lums  frame #%Lu=%Lums\n",
        g_PerfCounters.m_FrameTimeMin.Count,
        MilliSecsFromCycles(g_PerfCounters.m_FrameTimeMin.Cycles),
        g_PerfCounters.m_FrameTimeMax.Count,
        MilliSecsFromCycles(g_PerfCounters.m_FrameTimeMax.Cycles));

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_WAITINFO)
    {
        // Spit out WAIT info.
        static const D3DPERF_PerformanceCounters EventList[] =
        {
            PERF_PUSHBUFFER_WAITS,
            PERF_OBJECTLOCK_WAITS,
            PERF_D3DDEVICE_BLOCKUNTILIDLE_WAITS,
            PERF_D3DDEVICE_BLOCKONFENCE_WAITS,
            PERF_D3DDEVICE_BLOCKUNTILVERTICALBLANK_WAITS,
            PERF_PRESENT_WAITS,
            PERF_CPUSPINDURINGWAIT_TIME,
        };

        for(i = 0; i < sizeof(EventList) / sizeof(EventList[0]); i++)
        {
            // Check if we wasted any cycles on this counter
            DWORD id = EventList[i];

            if (g_PerfCounters.m_PerformanceCounters[id].Count)
            {
                ULONGLONG CyclesX10 =
                    MilliSecsFromCycles(g_PerfCounters.m_PerformanceCounters[id].Cycles * 10);
                PERFFIXEDPOINT WaitTime(CyclesX10, 10);
                PERFFIXEDPOINT WaitTimePerWait(
                    CyclesX10 / g_PerfCounters.m_PerformanceCounters[id].Count, 10);
                PERFFIXEDPOINT WaitTimePerFrame(CyclesX10 / frames, 10);

                DbgPrint("%s (%Lu): %Lu.%Lums (%Lu.%Lums/wait %Lu.%Lums/frame)\n",
                         g_PerfNames.m_PerformanceCounterNames[id],
                         g_PerfCounters.m_PerformanceCounters[id].Count,
                         WaitTime.IntPart, WaitTime.DecPart,
                         WaitTimePerWait.IntPart, WaitTimePerWait.DecPart,
                         WaitTimePerFrame.IntPart, WaitTimePerFrame.DecPart
                        );
            }
        }
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_VERTEXINFO)
    {
        // Spit out vertex info.
        DWORD  Vertices = g_PerfCounters.m_PerformanceCounters[PERF_VERTICES].Count;
        DWORD  VerticesPerFrame = Vertices / frames;
        DbgPrint("Vertices processed: %Lu verts (%Lu verts/frame)\n", Vertices, VerticesPerFrame);
        if( 0 != g_PerfCounters.m_PerformanceCounters[PERF_RUNPUSHBUFFER_BYTES].Count )
        {
            DbgPrint(" --> Doesn't include vertices processed via RunPushBuffer!\n");
        }
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_PUSHBUFFER)
    {
        // Spit out pushbuffer info.
        ULONGLONG PBBytes = D3DPERF_GetPushBufferBytesWritten();
        ULONGLONG PBBytesPerFrame = PBBytes / frames;
        DbgPrint("PushBuffer writes: %I64u bytes (%I64u bytes/frame)\n",
                 PBBytes,
                 PBBytesPerFrame
                );
        DWORD RunPushBufferBytes = g_PerfCounters.m_PerformanceCounters[PERF_RUNPUSHBUFFER_BYTES].Count;
        if( RunPushBufferBytes != 0 )
        {
            DWORD RunPushBufferBytesPerFrame = RunPushBufferBytes / frames;
            DbgPrint("RunPushBuffer bytes traversed: %Lu bytes (%Lu bytes/frame)\n",
                     RunPushBufferBytes,
                     RunPushBufferBytesPerFrame
                    );
        }
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_MEMORY)
    {
        // Print out the total number of available pages

        MM_STATISTICS mmStatistics = { sizeof(MM_STATISTICS) };

        MmQueryStatistics(&mmStatistics);

        DbgPrint("Available RAM: %Lu kbytes\n", mmStatistics.AvailablePages * 4);
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_INTINFO)
    {
        // Calculate number of interrupts and context switches

        PKPRCB Prcb = KeGetCurrentPrcb();

        DbgPrint(
            "Interrupts: %Lu\n"
            "Context Switches: %Lu\n",
            Prcb->InterruptCount - g_rcbAtReset.InterruptCount,
            Prcb->KeContextSwitches - g_rcbAtReset.KeContextSwitches);
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_PERFPROFILE)
    {
        // dump perf profile counts
        D3DPERF_DumpPerfProfCounts();
    }

    if(g_PerfCounters.m_dwDumpFPSInfoMask & D3DPERF_DUMP_FPS_PERFEVENTS)
    {
        // If a user set D3DPERF_DUMP_FPS_PERFEVENTS but didn't initialize
        // the storage structure then alloc it for them.
        if(!g_PerfCounters.pD3DPerfEvents)
        {
#ifndef STARTUPANIMATION
            D3DPERF_StartCountingPerfEvent(5000);

            if(!RecordD3DPerfEventsBak)
                RecordD3DPerfEventsBak = g_PerfCounters.RecordD3DPerfEvents;
#endif
        }
        else
        {
            D3DPERF_DumpPerfEvents();
        }
    }

    DbgPrint("\n");

    g_PerfCounters.RecordD3DPerfEvents = RecordD3DPerfEventsBak;

    DmTell(DMTELL_GPUPERF, &g_PerfCounters);
}

//------------------------------------------------------------------------------
// PerfEventCallbackStart
//
// Perf Event callback to record gpu start time.

void __cdecl PerfEventCallbackStart(
    DWORD Context)
{
    D3DPERFEvent *pD3DPerfEvent = (D3DPERFEvent *)Context;
    ULONGLONG Time = ___D3DCYCLETIMER_GetTime() - pD3DPerfEvent->CycleCpuStart;

    // Store the delta from when the cpu launched this guy.
    pD3DPerfEvent->CycleGpuStartDelta = (DWORD)Time;
}

//------------------------------------------------------------------------------
// PerfEventCallbackTime
//
// Perf Event callback to record gpu time.

void __cdecl PerfEventCallbackTime(
    DWORD Context)
{
    D3DPERFEvent *pD3DPerfEvent = (D3DPERFEvent *)Context;
    ULONGLONG Time = ___D3DCYCLETIMER_GetTime() - pD3DPerfEvent->CycleCpuStart;

    // Store the actual time the cpu thinks this thing took from
    // PerfEventCallbackStart to PerfEventCallbackTime.
    pD3DPerfEvent->CycleGpuLength = (DWORD)(Time - pD3DPerfEvent->CycleGpuStartDelta);
}

//------------------------------------------------------------------------------
// D3DPERF_PerfEventEnd
//
// End a performance event and write record info.

extern "C"
void WINAPI D3DPERF_PerfEventEnd(
    D3DPERFEvent *pD3DPerfEvent,
    BOOL RecordGpuTime)
{
    if(!pD3DPerfEvent)
        return;

    // Store the cpu length of this event.
    pD3DPerfEvent->CycleCpuLength =
        (DWORD)(___D3DCYCLETIMER_GetTime() - pD3DPerfEvent->CycleCpuStart);

    if(RecordGpuTime)
    {
        // Record end of gpu event.

        D3DDevice_InsertCallback(D3DCALLBACK_WRITE,
            PerfEventCallbackTime,
            (DWORD)pD3DPerfEvent);
    }

    // If data field hasn't been recorded, store the pushbuffer size used
    if(!pD3DPerfEvent->Data)
    {
        CDevice* pDevice = g_pDevice;

        // Get the current Cpu Put Ptr
        DWORD CpuPutPtr = (DWORD)(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER ?
            pDevice->m_pPushBufferRecordSavedPut : pDevice->m_Pusher.m_pPut);

        // Handle wrappage
        if(CpuPutPtr < pD3DPerfEvent->CpuPutPtr)
            CpuPutPtr += pDevice->m_PushBufferSize;

        pD3DPerfEvent->Data = CpuPutPtr - pD3DPerfEvent->CpuPutPtr;
    }
}

//------------------------------------------------------------------------------
// D3DPERF_PerfEventStart
//
// Start a performance event and return the record.

extern "C"
D3DPERFEvent * WINAPI D3DPERF_PerfEventStart(
    WORD Type,
    BOOL RecordGpuTime)
{
    if(!g_PerfCounters.CountD3DPerfEvents)
        return NULL;

    if((1 << Type) &&
        !(g_PerfCounters.RecordD3DPerfEvents & (1 << Type)))
    {
        return NULL;
    }

    // Grab an event. This also gets called from Dpc so be careful.
    DWORD CurrentEvent =
        (InterlockedIncrement((PLONG)&g_PerfCounters.CurrentD3DPerfEvent) - 1) %
        g_PerfCounters.CountD3DPerfEvents;

    D3DPERFEvent *pD3DPerfEvent = &g_PerfCounters.pD3DPerfEvents[CurrentEvent];

    // Calculate gpu busy percentage since last call.

    DWORD NumSamples = g_PerfCounters.m_ProfileSamples - g_PerfLastProfileSamples;
    if(NumSamples > 15)
    {
        // If the number of samples since last time is large enough, calc the %.

        DWORD GpuPct[D3DPERF_PROF_MAX];

        for(int iProf = D3DPERF_PROF_FE; iProf <= D3DPERF_PROF_BE; iProf++)
        {
            GpuPct[iProf] = (g_PerfCounters.m_ProfileBusyCounts[iProf] -
                g_PerfLastGpuBusyCounts[iProf]) * 99 / NumSamples;

            g_PerfLastGpuBusyCounts[iProf] = g_PerfCounters.m_ProfileBusyCounts[iProf];
        }

        g_PerfLastProfileSamples = g_PerfCounters.m_ProfileSamples;

        pD3DPerfEvent->GpuPct = (WORD)(GpuPct[D3DPERF_PROF_FE] | (GpuPct[D3DPERF_PROF_BE] << 8));
    }
    else
    {
        // Don't record anything for pct as we don't have enough samples.

        pD3DPerfEvent->GpuPct = 0xffff;
    }

    if(RecordGpuTime)
    {
        // Insert callback if we're timing how long the gpu is taking.

        D3DDevice_InsertCallback(D3DCALLBACK_WRITE,
            PerfEventCallbackStart,
            (DWORD)pD3DPerfEvent);
    }

    CDevice* pDevice = g_pDevice;

    pD3DPerfEvent->CpuPutPtr = (DWORD)(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER ?
        pDevice->m_pPushBufferRecordSavedPut : pDevice->m_Pusher.m_pPut);

    pD3DPerfEvent->Type = Type;
    pD3DPerfEvent->Data = 0;

    pD3DPerfEvent->CycleGpuStartDelta = 0;
    pD3DPerfEvent->CycleGpuLength = 0;

    pD3DPerfEvent->CycleCpuLength = 0;
    pD3DPerfEvent->CycleCpuStart = ___D3DCYCLETIMER_GetTime();

    return pD3DPerfEvent;
}

//------------------------------------------------------------------------------
// SpewPerfEvent
//
// Print out one pD3DPerfEvent.

void SpewPerfEvent(
    D3DPERFEvent *pD3DPerfEvent,
    ULONGLONG *pFirstCpuStart,
    ULONGLONG *pLastCpuTime)
{
    static const char *EventsNames[] =
    {
        "Header",               // 0
        "DrawVerticesUP",       // 1
        "DrawIndexedVertsUP",   // 2
        "DrawVertices",         // 3
        "DrawIndexedVerts",     // 4
        "BeginEnd",             // 5
        "RunPushBuffer",        // 6
        "VBlank",               // 7
        "Kickoff",              // 8
        "Present",              // 9
        "BlockUntilIdle",       // 10
        "BlockOnFence",         // 11
        "PushBufferWait",       // 12
        "ObjectLockWait",       // 13
        "event14",              // 14
        "event15",              // 15
        "event16",              // 16
        "event17",              // 17
        "event18",              // 18
        "event19",              // 19
        "event20",              // 20
        "User0",                // 21
        "User1",                // 22
        "User2",                // 23
        "User3",                // 24
        "User4",                // 25
        "User5",                // 26
        "User6",                // 27
        "User7",                // 28
        "User8",                // 29
        "User9",                // 30
        "User10",               // 31
    };

    if(pD3DPerfEvent->Type == D3DPERFEvent_Header)
    {
        DbgPrint("    *** %s ***\n", pD3DPerfEvent->HeaderText);
    }
    else if(pD3DPerfEvent->Type == D3DPERFEvent_Present ||
        /* pD3DPerfEvent->GpuPct != 0xffff || */
        pD3DPerfEvent->CycleCpuLength >= g_PerfCounters.m_DumpPerfCycleTimesThresholdCpu ||
        pD3DPerfEvent->CycleGpuLength >= g_PerfCounters.m_DumpPerfCycleTimesThresholdGpu)
    {
        char szBuf[11];
        const char *szEvent;
        ULONGLONG GpuStart = 0;
        DWORD Type = pD3DPerfEvent->Type;
        static const char HexDigits[] = "0123456789abcdef";

        if(pD3DPerfEvent->Type < ARRAYSIZE(EventsNames))
        {
            szEvent = EventsNames[pD3DPerfEvent->Type];
        }
        else
        {
            // For events larger than our array stick in the number.

            szBuf[0] = '0';
            szBuf[1] = 'x';

            for(int i = 2; i < 6; i++, Type <<= 4)
            {
                szBuf[i] = HexDigits[(Type >> 12) & 0xf];
            }

            szBuf[i] = 0;
            szEvent = szBuf;
        }

        // If this is our first record grab the CpuStart time
        if(!*pFirstCpuStart)
            *pFirstCpuStart = pD3DPerfEvent->CycleCpuStart;

        // If we've got a Gpu time, figure out when it started
        if(pD3DPerfEvent->CycleGpuStartDelta)
        {
            GpuStart = pD3DPerfEvent->CycleCpuStart +
                pD3DPerfEvent->CycleGpuStartDelta - *pFirstCpuStart;
        }

        // If we've got an event which happened within some other events cpu
        // time then bump it over by one char

        char Spacer[2];
        ULONGLONG CpuStart = pD3DPerfEvent->CycleCpuStart - *pFirstCpuStart;

        Spacer[0] = (CpuStart < *pLastCpuTime) ? '+' : 0;
        Spacer[1] = 0;

        char szPct[6];

        if(pD3DPerfEvent->GpuPct != 0xffff)
        {
            DWORD GpuFe = pD3DPerfEvent->GpuPct & 0xff;
            DWORD GpuBe = pD3DPerfEvent->GpuPct >> 8;

            // Convert Gpu Pct to a printable string

            szPct[0] = (BYTE)(GpuFe / 10) + '0';
            szPct[1] = (BYTE)(GpuFe % 10) + '0';
            szPct[2] = '/';
            szPct[3] = (BYTE)(GpuBe / 10) + '0';
            szPct[4] = (BYTE)(GpuBe % 10) + '0';
            szPct[5] = 0;
        }
        else
        {
            szPct[0] = 0;
        }

        // Print all this nonsense

        DbgPrint("%s%-*s %9I64u %9lu %9I64u %8lu %7lu %6lu %s\n",
            Spacer,
            19 - !!Spacer[0],
            szEvent,
            CpuStart,
            pD3DPerfEvent->CycleCpuLength,
            GpuStart,
            pD3DPerfEvent->CycleGpuLength,
            pD3DPerfEvent->CpuPutPtr - (DWORD)g_pDevice->m_pPushBase,
            pD3DPerfEvent->Data,
            szPct);

        // Record the largest cpu ending time we've gotten to date for indentation.

        *pLastCpuTime = max(*pLastCpuTime, CpuStart + pD3DPerfEvent->CycleCpuLength);
    }
}

//------------------------------------------------------------------------------
// D3DPERF_DumpPerfEvents
//
// Display all the performance events we've racked up

extern "C"
void WINAPI D3DPERF_DumpPerfEvents()
{
    if(!g_PerfCounters.CurrentD3DPerfEvent)
        return;

    // Turn off event recording so those VBlanks don't kill us whilst printing. I know this
    // was done in D3DPERF_DumpFrameRateInfo but D3DPERF_DumpPerfEvents() is user callable.
    BOOL RecordD3DPerfEventsBak = g_PerfCounters.RecordD3DPerfEvents;
    g_PerfCounters.RecordD3DPerfEvents = FALSE;

    // Block until we're idle to flush all the gpu events and make sure they've
    // had a chance to get written.
    D3DDevice_BlockUntilIdle();

    // Give em a header.
    DbgPrint("\nEvent                "
        "CpuStart   CpuTime  GpuStart  GpuTime  CpuPut  Data  Fe/Be\n");

    DWORD PresentNum = 0;
    ULONGLONG FirstCpuStart = 0;
    ULONGLONG LastCpuTime = 0;

    DWORD EndEvent =
        g_PerfCounters.CurrentD3DPerfEvent % g_PerfCounters.CountD3DPerfEvents;
    DWORD CurrentEvent =
        g_PerfCounters.CurrentD3DPerfEvent > g_PerfCounters.CountD3DPerfEvents ?
            EndEvent : 0;
    DWORD SpewFrameCount = g_PerfCounters.D3DPerfEventSpewFrameCount;
    DWORD SpewFrameStart =
        SpewFrameCount ? g_PerfCounters.D3DPerfEventSpewFrameStart : 0;

    do
    {
        D3DPERFEvent *pD3DPerfEvent = &g_PerfCounters.pD3DPerfEvents[CurrentEvent];
        BOOL IsPresentFrame = pD3DPerfEvent->Type == D3DPERFEvent_Present;

        PresentNum += IsPresentFrame;
        if(PresentNum >= SpewFrameStart)
        {
            SpewPerfEvent(pD3DPerfEvent, &FirstCpuStart, &LastCpuTime);
        }

        if(SpewFrameCount &&
            IsPresentFrame &&
            (PresentNum >= SpewFrameStart + SpewFrameCount))
        {
            // Run through the rest so we get an accurate Present count but
            // set FrameStart to -1 so we'll never spew anything else.
            SpewFrameStart = (DWORD)-1;
            SpewFrameCount = 0;
        }

        CurrentEvent = (CurrentEvent + 1) % g_PerfCounters.CountD3DPerfEvents;

    } while (CurrentEvent != EndEvent);

    DbgPrint("Total Perf Events: %lu  Presents Captured: %lu\n",
        g_PerfCounters.CurrentD3DPerfEvent, PresentNum);

    // Set everything back to normal and slide out of the room quietly.
    g_PerfCounters.RecordD3DPerfEvents = RecordD3DPerfEventsBak;
}

//------------------------------------------------------------------------------
// D3DPERF_Dump
//
// Display all the statistics we've accumulated

extern "C"
void WINAPI D3DPERF_Dump()
{
    DWORD id, i;

    ULONGLONG CurTime = ___D3DCYCLETIMER_GetTime();

    DbgPrint("Performance Counters:\n\n");

    for (id = 0; id < D3DPERF_MAX; id++)
    {
        if(g_PerfCounters.m_PerformanceCounters[id].Cycles)
        {
            DbgPrint("%10Lu - %s - %I64u\n",
                     g_PerfCounters.m_PerformanceCounters[id].Count,
                     g_PerfNames.m_PerformanceCounterNames[id],
                     g_PerfCounters.m_PerformanceCounters[id].Cycles);
        }
        else if (g_PerfCounters.m_PerformanceCounters[id].Count)
        {
            DbgPrint("%10Lu - %s\n",
                     g_PerfCounters.m_PerformanceCounters[id].Count,
                     g_PerfNames.m_PerformanceCounterNames[id]);
        }
    }

    DbgPrint("\nAPI Counters:\n\n");

    for (id = 0; id < D3DAPI_MAX; id++)
    {
        if (g_PerfCounters.m_APICounters[id])
        {
            DbgPrint("%10Lu - %s\n",
                     g_PerfCounters.m_APICounters[id],
                     g_PerfNames.m_APICounterNames[id]);
        }
    }

    DbgPrint("\nRenderState counters:\n\n");

    for (id = 0; id < D3DRS_MAX; id++)
    {
        if (g_PerfCounters.m_RenderStateCounters[id])
        {
            char *szName = "missing description";

            for (i = 0;
                 i < sizeof(g_PerfNames.m_RenderStateNames) / sizeof(D3DSTATENAME);
                 i++)
            {
                if (g_PerfNames.m_RenderStateNames[i].m_State == id)
                {
                    szName = g_PerfNames.m_RenderStateNames[i].m_Name;
                    break;
                }
            }

            DbgPrint("%10Lu - %s\n",
                     g_PerfCounters.m_RenderStateCounters[id],
                     szName);
        }
    }

    DbgPrint("\nTextureState counters:\n\n");

    for (id = 0; id < D3DTSS_MAX; id++)
    {
        if (g_PerfCounters.m_TextureStateCounters[id])
        {
            char *szName = "missing description";

            for (i = 0;
                 i < sizeof(g_PerfNames.m_TextureStateNames) / sizeof(D3DSTATENAME);
                 i++)
            {
                if (g_PerfNames.m_TextureStateNames[i].m_State == id)
                {
                    szName = g_PerfNames.m_TextureStateNames[i].m_Name;
                    break;
                }
            }

            DbgPrint("%10Lu - %s\n",
                     g_PerfCounters.m_TextureStateCounters[id],
                     szName);
        }
    }

    DbgPrint("\nTotal Cycles: %I64u\n",
        CurTime - g_PerfCounters.m_TSCLastResetVal);
}

//------------------------------------------------------------------------------
// D3DPERF_SetState
//
// Set perf state.

extern "C" HRESULT WINAPI
D3DPERF_SetState(
    D3DPERFSTATETYPE State,
    DWORD Value)
{
    static const DWORD rgDebug1[] =
    {
//$ BUGBUG: Not sure what the page table entry vertex cache dude does.
//  Commented out until we know and find out whether it is relevant to the
//  NV2A and/or useful.
//        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_PTE, _ENABLED),
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_CACHE, _ENABLED),
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_FILE, _ENABLED),
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_CACHE, _ENABLED) |
            DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_FILE, _ENABLED),
    };

    DWORD misc;
    CMiniport *pMiniPort = &g_pDevice->m_Miniport;
    BYTE *RegBase = (BYTE*)(pMiniPort->m_RegisterBase);

    switch(State)
    {
//$ case D3DPERFSTATE_VTX_PTE:
    case D3DPERFSTATE_VTX_CACHE:
    case D3DPERFSTATE_VTX_FILE:
    case D3DPERFSTATE_VTX_ALL:
        D3DDevice_BlockUntilIdle();

        misc = REG_RD32(RegBase, NV_PGRAPH_DEBUG_1);

        if(!Value)
            misc &= ~rgDebug1[State];
        else if(Value == 0x80000000)
            misc ^= rgDebug1[State];
        else
            misc |= rgDebug1[State];

        REG_WR32(RegBase, NV_PGRAPH_DEBUG_1, misc);
        return S_OK;
    }

    return E_FAIL;
}

/*
 * GPU Perf Profile stuff
 */

// Struct which hold card busy/idle bits and names.
//
static const struct
{
    DWORD State;
    const char *Name;
} g_GpuStates[D3DPERF_PROF_MAX] =
{
    { DRF_SHIFT(NV_PGRAPH_STATUS_STATE),    "gpu"       },
    { DRF_SHIFT(NV_PGRAPH_STATUS_FE_3D),    "frontend", },
    { DRF_SHIFT(NV_PGRAPH_STATUS_COMBINER), "backend",  },
};

//------------------------------------------------------------------------------
// D3DPERF_DumpPerfProfCounts
//

extern "C" void WINAPI
D3DPERF_DumpPerfProfCounts()
{
    if(g_PerfCounters.m_ProfileSamples)
    {
        for(int i = 0; i < D3DPERF_PROF_MAX; i++)
        {
            DbgPrint("%20s: %Lu (%Lu%%)\n",
                g_GpuStates[i].Name, g_PerfCounters.m_ProfileBusyCounts[i],
                g_PerfCounters.m_ProfileBusyCounts[i] * 100 / g_PerfCounters.m_ProfileSamples);
        }
    }

    DbgPrint("%20s: %Lu\n", "total samples", g_PerfCounters.m_ProfileSamples);
}

#ifndef STARTUPANIMATION

//------------------------------------------------------------------------------
// D3DPERF_StartPerfProfile
//
// Start GPU profiler.

extern "C"
BOOL WINAPI D3DPERF_StartPerfProfile()
{
    if(DmTell(DMTELL_GPUPERF, &g_PerfCounters))
        g_PerfProfData.Sampling = TRUE;

    g_PerfLastProfileSamples = 0;
    memset(&g_PerfLastGpuBusyCounts, 0, sizeof(g_PerfLastGpuBusyCounts));

    // clear profile information
    memset(g_PerfCounters.m_ProfileBusyCounts, 0, sizeof(g_PerfCounters.m_ProfileBusyCounts));
    memset(g_PerfCounters.m_ProfileData, 0xff, sizeof(g_PerfCounters.m_ProfileData));
    g_PerfCounters.m_ProfileSamples = 0;

    // record interrupt profile start time
    g_PerfCounters.m_TSCProfileStartTime = ___D3DCYCLETIMER_GetTime();

    return g_PerfProfData.Sampling;
}

//------------------------------------------------------------------------------
// D3DPERF_StopPerfProfile
//
// Stop GPU profiler.

extern "C"
void WINAPI D3DPERF_StopPerfProfile()
{
    if(g_PerfProfData.Sampling)
    {
        DmTell(DMTELL_GPUPERF, NULL);
        g_PerfProfData.Sampling = FALSE;
    }
}

#endif // STARTUPANIMATION

//------------------------------------------------------------------------------
// D3DPERF_HandlePresent
//
// Called once perf frame after Present call does its work.

extern "C"
void WINAPI D3DPERF_HandlePresent()
{
    // If the framerateinterval check to see if we've been doing stuff
    // for longer than the user specified

    if(g_PerfCounters.m_FrameRateIntervalTime)
    {
        ULONGLONG CurTime = ___D3DCYCLETIMER_GetTime();

        if(g_PerfCounters.m_TSCLastPresentVal)
        {
            ULONGLONG diff = CurTime - g_PerfCounters.m_TSCLastPresentVal;

            if(diff < g_PerfCounters.m_FrameTimeMin.Cycles)
            {
                g_PerfCounters.m_FrameTimeMin.Cycles = diff;
                g_PerfCounters.m_FrameTimeMin.Count =
                    g_PerfCounters.m_APICounters[API_D3DDEVICE_PRESENT];
            }

            if(diff > g_PerfCounters.m_FrameTimeMax.Cycles)
            {
                g_PerfCounters.m_FrameTimeMax.Cycles = diff;
                g_PerfCounters.m_FrameTimeMax.Count =
                    g_PerfCounters.m_APICounters[API_D3DDEVICE_PRESENT];
            }
        }

        g_PerfCounters.m_TSCLastPresentVal = CurTime;

        DWORD Time = MilliSecsFromCycles(CurTime - g_PerfCounters.m_TSCLastResetVal);

        if(Time > g_PerfCounters.m_FrameRateIntervalTime)
        {
            // Dump the current statistics and reset our counters

            if(g_PerfCounters.m_pfnDumpFrameRateInfoHandler)
                g_PerfCounters.m_pfnDumpFrameRateInfoHandler();

            D3DPERF_Reset();
        }
    }
}

//------------------------------------------------------------------------------
// PerfGetPushBufferDistance
//
// Performance Counter callback to track Gpu Get / Cpu put distance.

STDAPI PerfGetPushBufferDistance(
    PLARGE_INTEGER pliData,
    PLARGE_INTEGER pliUnused)
{
    CDevice* pDevice = g_pDevice;

    // Compute the push-buffer difference, accounting for wrapping of
    // the push-buffer:

    DWORD get = (DWORD) pDevice->GpuGet();
    if ((DWORD*) get <= pDevice->m_Pusher.m_pPut)
        get += pDevice->m_PusherLastSize;

    DWORD fence = (DWORD) pDevice->m_Pusher.m_pPut +
        pDevice->m_PusherLastSize;

    // The distance can be less than zero if the back-end write-back is
    // late because of slow rendering and we already gave away the pusher 
    // space in MakeSpace to newer stuff:

    INT distance = max(0, fence - get);

    pliData->QuadPart = (distance * 100) / g_pDevice->m_PushBufferSize;

    return XBDM_NOERR;
}

#endif  // PROFILE

//------------------------------------------------------------------------------
// D3DPERF_GetRegisterBase
//
// Return gpu register base.

extern "C"
BYTE * WINAPI D3DPERF_GetRegisterBase()
{
    CMiniport *pMiniPort = &g_pDevice->m_Miniport;
    BYTE *RegBase = (BYTE*)(pMiniPort->m_RegisterBase);

    return RegBase;
}

} // end of namespace

