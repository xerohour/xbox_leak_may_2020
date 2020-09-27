#include "globals.h"
#include "help_FileIOStress.h"

DWORD FileIOStressThreadProc(void* pvhWndMain);
void WaitForFileIOStressThreadEnd(HANDLE *phThread);
HANDLE g_hFileIOStressThread= NULL;
HANDLE g_hFileIOStressQuitEvent = NULL;

/********************************************************************************
********************************************************************************/
HRESULT StartFileIOStressThreadProc(void)
{
    DWORD dwTemp = 0;
    
    if (0 == g_TestParams.dwStressFileIODuration)
        return S_OK;

    g_hFileIOStressQuitEvent = CreateEvent(NULL, TRUE, 0, 0);
    if (!g_hFileIOStressQuitEvent)
    {
        Log(ABORTLOGLEVEL, "StartFileIOStressThreadProc: CreateEvent failed!!");
        return E_FAIL;        
    }
    
    g_hFileIOStressThread =    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)FileIOStressThreadProc,(void*)dwTemp,0,NULL);
    if (!g_hFileIOStressQuitEvent)
    {
        Log(ABORTLOGLEVEL, "StartFileIOStressThreadProc: CreateThread(FileIOStressThreadProc) failed!!");
        CloseHandle(g_hFileIOStressQuitEvent);
        return E_FAIL;        
    }

    if (FAILED(ThreadPriFromString(g_TestParams.szStressFileIOThreadPri, &dwTemp)))
    {
        Log(ABORTLOGLEVEL, "StartFileIOStressThreadProc: didn't recognize thread pri %s so using THREAD_PRIORITY_NORMAL", g_TestParams.szStressFileIOThreadPri);
        dwTemp = THREAD_PRIORITY_NORMAL;
    }

    if (0 == SetThreadPriority(g_hFileIOStressThread, dwTemp))
    {
        Log(ABORTLOGLEVEL, "SetThreadPriority(%u) failed with error code %u", dwTemp, GetLastError());
    }

    return S_OK;
};


/********************************************************************************
********************************************************************************/
HRESULT StopFileIOStressThreadProc(void)
{
HRESULT hr = S_OK;

    if (0 == g_TestParams.dwStressFileIODuration)
        return S_OK;


    if (!SetEvent(g_hFileIOStressQuitEvent))
    {
        Log(ABORTLOGLEVEL, "StopFileIOStressThreadProc: SetEvent failed!!  App may hang!");
        hr = E_FAIL;
    }
    
    WaitForFileIOStressThreadEnd(&g_hFileIOStressThread);
    
    if (!CloseHandle(g_hFileIOStressQuitEvent))
    {
        Log(ABORTLOGLEVEL, "StopFileIOStressThreadProc: CloseHandle failed!!  App may leak!");
        hr = E_FAIL;
    }

return hr;
};

/********************************************************************************
********************************************************************************/
#define LENGTH 10
DWORD FileIOStressThreadProc(void* pvhWndMain)
{
    DWORD dwStartTime = 0;
    CHAR szFileName[LENGTH] = {0};
    CHAR szFullPath[MAX_PATH] = {0};
    CHAR DummyData[512];
    DWORD i = 0;
    FILE *fyle = NULL;
    DWORD dwDeleteTime = 0;
    
    //Initialization.
    if (g_TestParams.dwStressFileIODuration)
    {
        Log(FYILOGLEVEL, "FileIOStressThreadProc: Doing File I/O for %dms at a time.", g_TestParams.dwStressFileIODuration);
    }
    else
        Log(FYILOGLEVEL, "FileIOStressThreadProc: not performaing FileIOStress.");


    while (WAIT_OBJECT_0 != WaitForSingleObject(g_hFileIOStressQuitEvent, 0))
    {   
        if (g_TestParams.dwStressFileIODuration)
        {
            dwStartTime = timeGetTime();
            do
            {
                for (i=0; i<LENGTH - 1; i++)
                {
                    szFileName[i] = 'a'+ rand() % 25;
                }
                szFileName[LENGTH-1] = 0;
                sprintf(szFullPath, "T:\\DMTest1\\%s", szFileName);
                fyle = NULL;
                fyle = fopen(szFullPath, "wb");


                if (fyle)
                {
                    for (i=0; i<512; i++)
                        DummyData[i] = rand() % 256;

                    fwrite((void *)&DummyData, 1, 512, fyle);
                    fclose(fyle);
                    dwDeleteTime = timeGetTime();
                    do
                    {
                        if (0 == DeleteFile(szFullPath))
                        {                            
                            DWORD dwLastError = GetLastError();
                            Log(ABORTLOGLEVEL, "DeleteFile(%s) failed with error code %u", szFileName, dwLastError);
                        }
                        else
                        {
                            break;
                        }
                    }
                    while (timeGetTime() - dwDeleteTime < 1000);
                }
            }
            while (timeGetTime() - dwStartTime < g_TestParams.dwStressFileIODuration);

            Sleep(500);
        }
    }

    Log(FYILOGLEVEL, "g_hFileIOStressQuitEvent has been set - bailing out of FileIOStressThreadProc");
    return 0;
} 



/********************************************************************************
********************************************************************************/
void WaitForFileIOStressThreadEnd(HANDLE *phThread)
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

