/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       stats.hpp
 *  Content:    header file for debug-only statistics gathering
 *
 ***************************************************************************/

#ifndef PROFILE

#if DBG
#define PROFILE     1
#endif

#endif

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if !PROFILE

    #define CLEAR_STATISTICS()

    #define COUNT_PERF(x)
    #define COUNT_ADD_PERF(x, amount)
    #define COUNT_API(x)
    #define COUNT_RENDERSTATE(x)
    #define COUNT_TEXTURESTATE(x)
    #define COUNT_PERFCYCLES(x)
    #define COUNT_PERFEVENT(x, y)

#else

    #include "d3d8perf.h"

    // function prototypes
    extern "C" void WINAPI D3DPERF_HandlePresent();

    STDAPI PerfGetPushBufferDistance(
        PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);

    // Stats prototypes:

    extern D3DPERF g_PerfCounters;
    extern DWORD   g_CpuCyclesPerMsec;

    // Reset all of our counters:

    #define CLEAR_STATISTICS() D3DPERF_Reset()

    // 'function' : no return value

    #pragma warning(disable:4035)

    // Read Time Stamp Counter helper

    __forceinline ULONGLONG ___D3DCYCLETIMER_GetTime()
    {
        _asm rdtsc
    }

    // Helper routine to calc milliseconds.

    __forceinline DWORD MilliSecsFromCycles(ULONGLONG Cycles)
    {
        return (DWORD)(Cycles / g_CpuCyclesPerMsec);
    }

    // Cycle Timer class

    struct ___D3DCYCLETIMER
    {
        ULONGLONG m_CycleStart;
        D3DPERF_PerformanceCounters m_perfctr;

        __forceinline ___D3DCYCLETIMER(D3DPERF_PerformanceCounters perfctr)
        {
            m_perfctr = perfctr;
            g_PerfCounters.m_PerformanceCounters[perfctr].Count++;
            if(g_PerfCounters.m_Breakperfctr == perfctr &&
                g_PerfCounters.m_PerformanceCounters[perfctr].Count == g_PerfCounters.m_BreakCount)
            {
                _asm int 3;
            }

            m_CycleStart = ___D3DCYCLETIMER_GetTime();
        }

        __forceinline ~___D3DCYCLETIMER()
        {
            _asm cpuid;
            ULONGLONG Cycles = ___D3DCYCLETIMER_GetTime() - m_CycleStart;

            g_PerfCounters.m_PerformanceCounters[m_perfctr].Cycles += Cycles;

            if(g_PerfCounters.m_pfnCycleThresholdHandler &&
                (g_PerfCounters.m_DumpWaitCycleTimesThreshold < (DWORD)Cycles))
            {
                g_PerfCounters.m_pfnCycleThresholdHandler(m_perfctr, Cycles);
            }
        }
    };

    // Use the following routines to increment counters for the various types
    // of calls.  On non-profile-enabled builds, these expand to nothing.

    __forceinline void COUNT_PERF(D3DPERF_PerformanceCounters perfctr)
    {
        g_PerfCounters.m_PerformanceCounters[perfctr].Count++;
        if(g_PerfCounters.m_Breakperfctr == perfctr &&
            g_PerfCounters.m_PerformanceCounters[perfctr].Count == g_PerfCounters.m_BreakCount)
        {
            _asm int 3;
        }
    }

    __forceinline void COUNT_ADD_PERF(D3DPERF_PerformanceCounters perfctr, DWORD amount)
    {
        DWORD countbak = g_PerfCounters.m_PerformanceCounters[perfctr].Count;

        g_PerfCounters.m_PerformanceCounters[perfctr].Count += amount;
        if(g_PerfCounters.m_Breakperfctr == perfctr &&
            countbak < g_PerfCounters.m_BreakCount &&
            g_PerfCounters.m_PerformanceCounters[perfctr].Count >= g_PerfCounters.m_BreakCount)
        {
            _asm int 3;
        }
    }

    // Perf Event class

    struct ___D3DPERFEVENT
    {
        BOOL m_RecordGpuTime;
        D3DPERFEvent *m_pD3DPerfEvent;

        __forceinline ___D3DPERFEVENT(WORD Type, BOOL RecordGpuTime)
        {
            m_RecordGpuTime = RecordGpuTime;
            m_pD3DPerfEvent = D3DPERF_PerfEventStart(Type, RecordGpuTime);
        }

        __forceinline ~___D3DPERFEVENT()
        {
            D3DPERF_PerfEventEnd(m_pD3DPerfEvent, m_RecordGpuTime);
        }
    };

    #define COUNT_API(x)                    g_PerfCounters.m_APICounters[x]++
    #define COUNT_RENDERSTATE(x)            g_PerfCounters.m_RenderStateCounters[x]++
    #define COUNT_TEXTURESTATE(x)           g_PerfCounters.m_TextureStateCounters[x]++
    #define COUNT_PERFCYCLES(x)             ___D3DCYCLETIMER _ct##x(x)
    #define COUNT_PERFEVENT(x, y)           ___D3DPERFEVENT _PerfEvent(x, y)

    const D3DPERFNAMES g_PerfNames =
    {
        {
            #define DEFINE_PERF(x) #x,

            #include "d3d8perfp.h"

            #undef DEFINE_PERF
        },
        {
            #define DEFINE_API(x) #x,

            #include "d3d8perfp.h"

            #undef DEFINE_API
        },
        {
            #define RS(x) { D3DRS_##x, #x },

            RS(ZENABLE)
            RS(FILLMODE)
            RS(SHADEMODE)
            RS(ZWRITEENABLE)
            RS(ALPHATESTENABLE)
            RS(SRCBLEND)
            RS(DESTBLEND)
            RS(CULLMODE)
            RS(ZFUNC)
            RS(ALPHAREF)
            RS(ALPHAFUNC)
            RS(DITHERENABLE)
            RS(ALPHABLENDENABLE)
            RS(FOGENABLE)
            RS(SPECULARENABLE)
            RS(FOGCOLOR)
            RS(FOGTABLEMODE)
            RS(FOGSTART)
            RS(FOGEND)
            RS(FOGDENSITY)
            RS(EDGEANTIALIAS)
            RS(ZBIAS)
            RS(RANGEFOGENABLE)
            RS(STENCILENABLE)
            RS(STENCILFAIL)
            RS(STENCILZFAIL)
            RS(STENCILPASS)
            RS(STENCILFUNC)
            RS(STENCILREF)
            RS(STENCILMASK)
            RS(STENCILWRITEMASK)
            RS(TEXTUREFACTOR)
            RS(WRAP0)
            RS(WRAP1)
            RS(WRAP2)
            RS(WRAP3)
            RS(LIGHTING)
            RS(AMBIENT)
            RS(COLORVERTEX)
            RS(LOCALVIEWER)
            RS(NORMALIZENORMALS)
            RS(DIFFUSEMATERIALSOURCE)
            RS(SPECULARMATERIALSOURCE)
            RS(AMBIENTMATERIALSOURCE)
            RS(EMISSIVEMATERIALSOURCE)
            RS(VERTEXBLEND)
            RS(POINTSIZE)
            RS(POINTSIZE_MIN)
            RS(POINTSPRITEENABLE)
            RS(POINTSCALEENABLE)
            RS(POINTSCALE_A)
            RS(POINTSCALE_B)
            RS(POINTSCALE_C)
            RS(MULTISAMPLEANTIALIAS)
            RS(MULTISAMPLEMASK)
            RS(PATCHEDGESTYLE)
            RS(PATCHSEGMENTS)
            RS(POINTSIZE_MAX)
            RS(COLORWRITEENABLE)
            RS(BLENDOP)

            #undef RS
        },
        {
            #define TS(x) { D3DTSS_##x, #x},

            TS(COLOROP)
            TS(COLORARG1)
            TS(COLORARG2)
            TS(ALPHAOP)
            TS(ALPHAARG1)
            TS(ALPHAARG2)
            TS(BUMPENVMAT00)
            TS(BUMPENVMAT01)
            TS(BUMPENVMAT10)
            TS(BUMPENVMAT11)
            TS(TEXCOORDINDEX)
            TS(ADDRESSU)
            TS(ADDRESSV)
            TS(BORDERCOLOR)
            TS(MAGFILTER)
            TS(MINFILTER)
            TS(MIPFILTER)
            TS(MIPMAPLODBIAS)
            TS(MAXMIPLEVEL)
            TS(MAXANISOTROPY)
            TS(BUMPENVLSCALE)
            TS(BUMPENVLOFFSET)
            TS(TEXTURETRANSFORMFLAGS)
            TS(ADDRESSW)
            TS(COLORARG0)
            TS(ALPHAARG0)
            TS(RESULTARG)

            #undef TS
        }
    };

#endif // PROFILE

} // end namespace
