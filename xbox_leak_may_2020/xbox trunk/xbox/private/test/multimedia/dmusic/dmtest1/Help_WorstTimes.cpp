#include "globals.h"

struct TIMINGMETRICS
{
CHAR szTestName[MAX_PATH];
DWORD dwTime;
};
#define MAXTIMINGMETRICS 10

TIMINGMETRICS *g_pTimingMetrics = NULL;
BOOL           g_bInitialized = FALSE;

/********************************************************************************
********************************************************************************/
HRESULT InitializeWorstTimes(void)
{
    HRESULT hr = S_OK;
    ASSERT(!g_bInitialized);
    //Allocate our set of timing metrics
    g_pTimingMetrics = new TIMINGMETRICS[MAXTIMINGMETRICS];
    CHECKALLOC(g_pTimingMetrics);
    if (SUCCEEDED(hr))
    {
        ZeroMemory(g_pTimingMetrics, sizeof(TIMINGMETRICS) * MAXTIMINGMETRICS);
        g_bInitialized = TRUE;
    }

    return hr;
}

/********************************************************************************
********************************************************************************/
HRESULT UnInitializeWorstTimes(void)
{
    HRESULT hr = S_OK;
    ASSERT(g_bInitialized);
    delete []g_pTimingMetrics;
    g_pTimingMetrics= NULL;
    g_bInitialized = FALSE;
    return hr;
}


/********************************************************************************
********************************************************************************/
//Do the "high score" thing.
//Element x is the lowest time (of the highs), Element 0 is the highest.
HRESULT ProcessTime(DWORD dwTime, LPCSTR szTestName)
{
    DWORD dwReplacingIndex = 0;
    DWORD dwScootingIndex = 0;
    BOOL bFound = FALSE;
    HRESULT hr = S_OK;

    ASSERT(g_bInitialized);

    //Start with the highest
    for (dwReplacingIndex =0; dwReplacingIndex < MAXTIMINGMETRICS; dwReplacingIndex++)
    {
        if (dwTime > g_pTimingMetrics[dwReplacingIndex].dwTime)
        {
            bFound = TRUE;
            break;
        }
    }

    //if we found one, replace it.
    if (bFound)
    {
        for (dwScootingIndex=MAXTIMINGMETRICS-1;  dwScootingIndex>dwReplacingIndex; dwScootingIndex--)
        {
            g_pTimingMetrics[dwScootingIndex] = g_pTimingMetrics[dwScootingIndex - 1];
        }

        g_pTimingMetrics[dwReplacingIndex].dwTime = dwTime;
        strncpy(g_pTimingMetrics[dwReplacingIndex].szTestName, szTestName, MAX_PATH);
    }
    return hr;
};
    


/********************************************************************************
********************************************************************************/
HRESULT PrintTimes(void)
{
    HRESULT hr = S_OK;
    ASSERT(g_bInitialized);

    Log(1, "Longest tests to run");
    Log(1, "--------------------");

    for (DWORD i=0; i<MAXTIMINGMETRICS; i++)
    {
        if (g_pTimingMetrics[i].dwTime)
        {
            Log(1, "%-60s:  %10ums", g_pTimingMetrics[i].szTestName, g_pTimingMetrics[i].dwTime); 
        }
    }

    return hr;
}
