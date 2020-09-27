#include "globals.h"
#include "Help_MusicHeap.h"


static IMusicHeap *g_pNormal   = NULL;
static IMusicHeap *g_pPhysical = NULL;

/********************************************************************************
********************************************************************************/
//HRESULT CreateHeaps(IMusicHeap **ppNormal, IMusicHeap **ppPhysical)
HRESULT CreateHeaps(LPCSTR szHeap, DWORD dwLimitNormal, DWORD dwLimitPhysical)
{
HRESULT hr = S_OK;

ASSERT(&g_pNormal);
ASSERT(&g_pPhysical);

CHECKRUN(CreateMusicHeapTrack(&g_pNormal, FALSE));
CHECKRUN(CreateMusicHeapTrack(&g_pPhysical, TRUE));
if (_strcmpi(szHeap, "tracking") == 0)
{
}
else if (_strcmpi(szHeap, "fixeduser") == 0)
{
    CHECKRUN(g_pNormal->SetSize  (dwLimitNormal));
    CHECKRUN(g_pPhysical->SetSize(dwLimitPhysical));
}
else
{
    ::Log(ABORTLOGLEVEL, "Error: specified unknown heap type %s", szHeap);
    hr = E_FAIL;
}

if (FAILED(hr))
{
    RELEASE(g_pNormal);
    RELEASE(g_pPhysical);
}

return hr;
};

/********************************************************************************
********************************************************************************/
HRESULT InitializeDMusicWithCreatedHeaps(void)
{
    ASSERT(g_pNormal);
    ASSERT(g_pPhysical);
    return DirectMusicInitializeEx(g_pNormal, g_pPhysical, &DirectMusicDefaultFactory);
}


/********************************************************************************
********************************************************************************/
//HRESULT DestroyHeaps(IMusicHeap *pNormal, IMusicHeap *pPhysical)
HRESULT DestroyHeaps(void)
{
HRESULT hr = S_OK;

//No reason to call this if they're NULL anyway.
ASSERT(g_pNormal);
ASSERT(g_pPhysical);

RELEASE(g_pNormal);
RELEASE(g_pPhysical);

return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT GetMemoryUsage(BOOL bPhysical, DWORD *pdwMemUsage)
{
    IMusicHeap *pHeap = bPhysical ? g_pPhysical : g_pNormal;
    ASSERT(pHeap);
    ASSERT(pdwMemUsage);
    return pHeap->GetHeapAllocation(pdwMemUsage);
}



/********************************************************************************
INPARAMS:
    bPhysical           = TRUE if physical memory, FALSE if normal memory.
    dwTargetRange       = The maximum number of bytes the memory may vary over the target duration.
    dwTargetDuration    = how long the memory must stay within the target range.
    dwTimeOut           = how long we'll wait before timing out.

OUTPARAMS:
    pbStabilized        = Did the memory stabilize within the given parameters before the timeout period elapsed?
    pdwMemory           = The final memory reading, after stabilization has been reached.

NOTES:
    If memory doesn't stabilize, the function will still return
        S_OK
        *pbStabilized = FALSE
        *pdwMemory    = 0
********************************************************************************/
//How many milliseconds to wait between memory samples.  Currently a target duration of 1-second will require a 50-element array.
#define SAMPLE_FREQUENCY 20
HRESULT WaitForMemoryToStabilize(BOOL bPhysical, DWORD dwTargetRange, DWORD dwTargetDuration, DWORD dwTimeOut, BOOL *pbStabilized, DWORD *pdwMemory)
{
HRESULT hr = S_OK;
DWORD dwStartTime = 0;
DWORD  dwTotalMemorySamples = 0;
DWORD  dwNextSample = 0;
BOOL   bFilledArray = FALSE;
DWORD *pdwMemorySamples = NULL;
BOOL   bStabilized = FALSE;
DWORD  dwMemory = 0;


//We allocate a circular array to hold the samples.
dwTotalMemorySamples = dwTargetDuration / SAMPLE_FREQUENCY;
pdwMemorySamples = new DWORD[dwTotalMemorySamples];

ASSERT(dwTotalMemorySamples > 20);

//Repeatedly take samples.
dwStartTime = timeGetTime();
while (timeGetTime() - dwStartTime < dwTimeOut)
{
    //Take a sample.
    CHECKRUN(GetMemoryUsage(bPhysical, &pdwMemorySamples[dwNextSample]));

    //If we've hit the end of the array, wrap around.  Also set the "filled" flag
    //  if its our first time, signifying we've collected enough data to analyze.
    if (SUCCEEDED(hr))
    {
        dwNextSample++;
        if (dwNextSample == dwTotalMemorySamples)
        {
            if (!bFilledArray)
                bFilledArray = TRUE;

            dwNextSample = 0;
        }

        //If we've collection enough data to analyze...
        if (bFilledArray)
        {
            
            //Then calculate the range of the samples.  Not the most efficient code, but the most readable.
            DWORD dwLowest = 0xFFFFFFFF;
            DWORD dwHighest = 0;
            DWORD dwRange = 0;
            DWORD i = 0;

            for (i=0; i<dwTotalMemorySamples; i++)
            {
                if (pdwMemorySamples[i] < dwLowest)
                    dwLowest = pdwMemorySamples[i];
                if (pdwMemorySamples[i] > dwHighest)
                    dwHighest = pdwMemorySamples[i];                
            }

            //If we meet our criteria, then memory has stabilized and we're outta here!!
            dwRange = (dwHighest - dwLowest);
            if (dwRange <= dwTargetRange)
            {
                bStabilized = TRUE;
                break;
            }
        }
    }

    Sleep(SAMPLE_FREQUENCY);
}

//If we stabilize then set our memory to the last sample taken.
if (bStabilized)
{
    DWORD dwMostRecentSample;

    if (0 == dwNextSample)
        dwMostRecentSample = dwTotalMemorySamples - 1;
    else
        dwMostRecentSample = dwNextSample - 1;

    dwMemory = pdwMemorySamples[dwMostRecentSample];
}
else
{
    dwMemory = 0;
}

//At the highest log level, print out all the values obtained.
if (SUCCEEDED(hr))
{
    Log(MAXLOGLEVEL, "MEMORY STABLIZED: %s", bStabilized ? "YES" : "NO");
    Log(MAXLOGLEVEL, "Memory values");
    Log(MAXLOGLEVEL, "-------------");

    for (DWORD i=0; i<dwTotalMemorySamples; i++)
    {
        Log(MAXLOGLEVEL, "%8u", pdwMemorySamples[ (dwNextSample + i) % dwTotalMemorySamples]);
    }
}

//Clean up.
delete []pdwMemorySamples;
pdwMemorySamples = NULL;

//Set the outparams.
if (pbStabilized)
    *pbStabilized = bStabilized;
if (pdwMemory)
    *pdwMemory = dwMemory;

return hr;


}
#undef SAMPLE_FREQUENCY


/********************************************************************************
********************************************************************************/
HRESULT ExpectMemoryToStabilize(BOOL bPhysical, DWORD dwTargetRange, DWORD dwTargetDuration, DWORD dwTimeOut, BOOL *pbStabilized, DWORD *pdwMemory)
{
HRESULT hr = WaitForMemoryToStabilize(bPhysical, dwTargetRange, dwTargetDuration, dwTimeOut, pbStabilized, pdwMemory);
if (SUCCEEDED(hr))
{
    if (!(*pbStabilized))
    {
        hr = E_FAIL;
    }
}

return hr;
}
