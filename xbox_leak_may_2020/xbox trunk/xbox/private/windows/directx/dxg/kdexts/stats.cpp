#include "precomp.hpp"

using namespace D3D;

#if DBG

VOID DumpPerf(VOID)
{
    DWORD id;
    DWORD i;
    CHAR* string;
    ULONG addr;
    DWORD Count[D3DPERF_MAX];
    
    addr = GetExpression("D3D__g_PerfCounters");
    if (addr == 0)
    {
        dprintf("Error: can't get g_PerfCounters address\n");
        return;
    }

    if (ReadMemory(addr, (PVOID)Count, sizeof(Count), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_PerfCounters\n");
        return;
    }

    dprintf("\nPerf counters:\n");

    for (id = 0; id < D3DPERF_MAX; id++)
    {
        if (Count[id])
        {
            dprintf("%10li  %s\n", Count[id], g_PerfNames.m_PerformanceCounterNames[id]);
        }
    }
}

VOID DumpApiCount(VOID)
{
    DWORD id;
    DWORD i;
    CHAR* string;
    ULONG addr;
    DWORD Count[D3DAPI_MAX];

    addr = GetExpression("D3D__g_PerfCounters");
    if (addr == 0)
    {
        dprintf("Error: can't get g_PerfCounters address\n");
        return;
    }

    addr += (ULONG)&((D3DPERF *)0)->m_APICounters;

    if (ReadMemory(addr, (PVOID)Count, sizeof(Count), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_PerfCounters\n");
        return;
    }

    dprintf("API counters:\n");

    for (id = 0; id < D3DAPI_MAX; id++)
    {
        if (Count[id])
        {
            dprintf("%10li  %s\n", Count[id], g_PerfNames.m_APICounterNames[id]);
        }
    }
}

VOID DumpTextureStageState(VOID)
{
    DWORD id;
    DWORD i;
    CHAR* string;
    ULONG addr;
    DWORD Count[D3DTSS_MAX];

    addr = GetExpression("D3D__g_PerfCounters");
    if (addr == 0)
    {
        dprintf("Error: can't get g_PerfCounters address\n");
        return;
    }

    addr += (ULONG)&((D3DPERF *)0)->m_TextureStateCounters;

    if (ReadMemory(addr, (PVOID)Count, sizeof(Count), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_PerfCounters\n");
        return;
    }

    dprintf("\nTextureStageState counters:\n");

    for (id = 0; id < D3DTSS_MAX; id++)
    {
        if (Count[id])
        {
            string = "Missing Description";

            for (i = 0; 
                 i < sizeof(g_PerfNames.m_TextureStateNames) / sizeof(g_PerfNames.m_TextureStateNames[0]);
                 i++)
            {
                if (g_PerfNames.m_TextureStateNames[i].m_State == id)
                {
                    string = g_PerfNames.m_TextureStateNames[i].m_Name;
                    break;
                }
            }

            dprintf("%10li  %s\n", Count[id], string);
        }
    }
}

VOID DumpRenderState(VOID)
{
    DWORD id;
    DWORD i;
    CHAR* string;
    ULONG addr;
    DWORD Count[D3DRS_MAX];

    addr = GetExpression("D3D__g_PerfCounters");
    if (addr == 0)
    {
        dprintf("Error: can't get g_PerfCounters address\n");
        return;
    }

    addr += (ULONG)&((D3DPERF *)0)->m_RenderStateCounters;

    if (ReadMemory(addr, (PVOID)Count, sizeof(Count), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_PerfCounters\n");
        return;
    }

    dprintf("\nRenderState counters:\n");

    for (id = 0; id < D3DRS_MAX; id++)
    {
        if (Count[id])
        {
            string = "Missing Description";

            for (i = 0; 
                 i < sizeof(g_PerfNames.m_RenderStateNames) / sizeof(g_PerfNames.m_RenderStateNames[0]);
                 i++)
            {
                if (g_PerfNames.m_RenderStateNames[i].m_State == id)
                {
                    string = g_PerfNames.m_RenderStateNames[i].m_Name;
                    break;
                }
            }

            dprintf("%10li  %s\n", Count[id], string);
        }
    }
}

DECLARE_API(stats)
{
    const char *s;

    if (args[0] == '\0')
    {
        s = "-artp";
    }
    else
    {
        s = args;
    }

    if (IsOption(s, '?') || IsOption(s, 'h'))
    {
        HelpStats();
    }

    if (IsOption(s, 'a'))
    {
        DumpApiCount();
    }

    if (IsOption(s, 'r'))
    {
        DumpRenderState();
    }

    if (IsOption(s, 't'))
    {
        DumpTextureStageState();
    }

    if (IsOption(s, 'p'))
    {
        DumpPerf();
    }
}

#else // DBG

DECLARE_API(stats)
{
    dprintf("Stats extention works for checked builds only\n");
}

#endif // DBG

