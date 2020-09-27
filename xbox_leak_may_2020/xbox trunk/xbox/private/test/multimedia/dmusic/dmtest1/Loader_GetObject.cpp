#include "globals.h"
#include "cicmusicx.h"


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_GetObject_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Loader_GetObject())");
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Loader_GetObject_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Loader_GetObject())");
    return hr;
};



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_GetObject_PerfMem_DownloadWaveLeak(CtIDirectMusicPerformance8* ptPerf8, DWORD bStreaming, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicPerformance *pPerf = NULL;
    IDirectMusicSegment *pSegment = NULL;
    IDirectMusicSegmentState *pSegmentState = NULL;
    IDirectMusicAudioPath *pPath = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    CHAR szPath[MAX_PATH];
    CHAR szFile[MAX_PATH];
    LPSTR szServerPathNames[] = {"WAV/08_08_2.wav", "DMusic/DMTest1/_wav/Streaming10s.wav"};
    CHAR szPathNames[2][MAX_PATH] = {0};
    LPSTR szFullPath = NULL;


    //Copy the files locally.
    for (DWORD i=0; i<NUMELEMS(szServerPathNames); i++)
    {
        CHECKRUN(MediaCopyFile(szServerPathNames[i]));
        strcpy(szPathNames[i], MediaServerToLocal(szServerPathNames[i]));
    }


    if (bStreaming)
        szFullPath = szPathNames[1];
    else
        szFullPath = szPathNames[0];
    
    CHECKRUN(ChopPath(szFullPath, szPath, szFile));
    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));
    dwStartTime = timeGetTime();
    do
    {
        CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
        CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
        CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFile, (void **)&pSegment));
        if (FAILED(hr))
            Log(FYILOGLEVEL, "FAIL!!!!");

        CHECKRUN(pPerf->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &pPath));    

//        CHECKRUN(pPerf->PlaySegmentEx(pSegment,0,0,0,0,&pSegmentState,0,pPath));    
//        CHECKRUN(pPerf->StopEx(pSegmentState, 0, 0));    

        Wait(100);

        SAFE_RELEASE(pSegmentState);
        SAFE_RELEASE(pSegment);
        SAFE_RELEASE(pPath);
        SAFE_RELEASE(pLoader);
        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    SAFE_RELEASE(pPerf);

    return hr;
};




/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_GetObject_PerfMem_Type_Full(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicPerformance *pPerf = NULL;
    IUnknown *pUnknown = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    DWORD dwLoads = 0;
    CHAR szPath[MAX_PATH];
    CHAR szFile[MAX_PATH];
    CHAR szFullPath[MAX_PATH] = {0};
    GUID *pCLSID= NULL;
    GUID *pIID= NULL;
    
    CHECKRUN(MediaCopyDirectory((LPSTR)dwFilename));
    strcpy(szFullPath, MediaServerToLocal((LPSTR)dwFilename));
    CHECKRUN(GUIDsFromFileName(szFullPath, &pCLSID, &pIID));
    CHECKRUN(ChopPath(szFullPath, szPath, szFile));
    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));
    dwStartTime = timeGetTime();
    do
    {
        dwLoads++;
        CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
        CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
        CHECKRUN(pLoader->LoadObjectFromFile(*pCLSID, *pIID, szFile, (void **)&pUnknown));
        if (FAILED(hr))
        {
            Log(ABORTLOGLEVEL, "Attempt %u to load %s FAILED!!!!", dwLoads, szFile);
        }

        SAFE_RELEASE(pUnknown);
        SAFE_RELEASE(pLoader);
        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    //Make sure everything gets released.
    SAFE_RELEASE(pUnknown);
    SAFE_RELEASE(pLoader);
    SAFE_RELEASE(pPerf);

    if (FAILED(hr))
        Log(ABORTLOGLEVEL, "Loader_GetObject_PerfMem_Type_Full failed with %s (%08X)", tdmXlatHRESULT(hr), hr);
    return hr;
};



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_GetObject_PerfMem_Type_Partial(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicPerformance *pPerf = NULL;
    IUnknown *pUnknown = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    CHAR szPath[MAX_PATH];
    CHAR szFile[MAX_PATH];
    CHAR szFullPath[MAX_PATH] = {0};
    GUID *pCLSID= NULL;
    GUID *pIID= NULL;

    CHECKRUN(MediaCopyDirectory((LPSTR)dwFilename));
    strcpy(szFullPath, MediaServerToLocal((LPSTR)dwFilename));
    CHECKRUN(GUIDsFromFileName(szFullPath, &pCLSID, &pIID));
    CHECKRUN(ChopPath(szFullPath, szPath, szFile));
    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));
    CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
    CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
    dwStartTime = timeGetTime();
    do
    {
        CHECKRUN(pLoader->LoadObjectFromFile(*pCLSID, *pIID, szFile, (void **)&pUnknown));
        if (FAILED(hr))
        {
            Log(ABORTLOGLEVEL, "Load %s FAILED!!!!", szFile);
        }

        SAFE_RELEASE(pUnknown);
        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    //Make sure everything gets released.
    SAFE_RELEASE(pUnknown);
    SAFE_RELEASE(pLoader);
    SAFE_RELEASE(pPerf);

    if (FAILED(hr))
        Log(ABORTLOGLEVEL, "Loader_GetObject_PerfMem_Type_Full failed with %s (%08X)", tdmXlatHRESULT(hr), hr);
    return hr;
};



