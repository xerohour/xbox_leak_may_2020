#include "globals.h"
#include "help_DoWork.h"

DWORD DoDMWorkThreadProc(void* pvhWndMain);
void WaitForDoDMWorkThreadEnd(HANDLE *phThread);
HANDLE g_hDoDMWorkThread= NULL;
HANDLE g_hDoDMWorkQuitEvent = NULL;

/********************************************************************************
********************************************************************************/
HRESULT StartDoDMWorkThreadProc(void)
{
    DWORD dwTemp = 0;

    //If we're letting DMusic do our thread, then bail.
    if (!g_TestParams.bDoWorkLocalThread)
        return S_OK;

    g_hDoDMWorkQuitEvent = CreateEvent(NULL, TRUE, 0, 0);
    if (!g_hDoDMWorkQuitEvent)
    {
        Log(ABORTLOGLEVEL, "StartDoDMWorkThreadProc: CreateEvent failed!!");
        return E_FAIL;        
    }
    
    g_hDoDMWorkThread =    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoDMWorkThreadProc,(void*)dwTemp,0,NULL);
    if (!g_hDoDMWorkQuitEvent)
    {
        Log(ABORTLOGLEVEL, "StartDoDMWorkThreadProc: CreateThread(DoDMWorkThreadProc) failed!!");
        CloseHandle(g_hDoDMWorkQuitEvent);
        return E_FAIL;        
    }

    return S_OK;
};


/********************************************************************************
********************************************************************************/
HRESULT StopDoDMWorkThreadProc(void)
{
HRESULT hr = S_OK;

    //If we're letting DMusic do our thread, then bail.
    if (!g_TestParams.bDoWorkLocalThread)
        return S_OK;

    if (!SetEvent(g_hDoDMWorkQuitEvent))
    {
        Log(ABORTLOGLEVEL, "StopDoDMWorkThreadProc: SetEvent failed!!  App may hang!");
        hr = E_FAIL;
    }
    
    WaitForDoDMWorkThreadEnd(&g_hDoDMWorkThread);
    
    if (!CloseHandle(g_hDoDMWorkQuitEvent))
    {
        Log(ABORTLOGLEVEL, "StopDoDMWorkThreadProc: CloseHandle failed!!  App may leak!");
        hr = E_FAIL;
    }

return hr;
};

/********************************************************************************
********************************************************************************/
DWORD DoDMWorkThreadProc(void* pvhWndMain)
{
    
    DWORD dwQuantum;
    if (g_TestParams.dwDoWorkFrequency)
    {
        dwQuantum = 1000 / g_TestParams.dwDoWorkFrequency;
        Log(FYILOGLEVEL, "DoDMWorkThreadProc: Setting dwQuantum to %dms.", dwQuantum);
    }
    else
        Log(FYILOGLEVEL, "DoDMWorkThreadProc: not calling DirectMusicDoDMWork.");


    while (WAIT_OBJECT_0 != WaitForSingleObject(g_hDoDMWorkQuitEvent, 0))
    {   
        if (g_TestParams.dwDoWorkFrequency)
            DirectMusicDoWork(dwQuantum);
        Sleep(dwQuantum);
    }

    Log(FYILOGLEVEL, "g_hDoDMWorkQuitEvent has been set - bailing out of DoDMWorkThreadProc");
    return 0;
} 



/********************************************************************************
********************************************************************************/
void WaitForDoDMWorkThreadEnd(HANDLE *phThread)
{
    DWORD dwWaitResult = WAIT_TIMEOUT;

    if(*phThread)
    {
        dwWaitResult = WAIT_TIMEOUT;
        while(dwWaitResult != WAIT_OBJECT_0)
        {
            dwWaitResult = WaitForSingleObject(*phThread, 0);
            Sleep(100);
        }

        CloseHandle(*phThread);
        *phThread = NULL;
    }
}

