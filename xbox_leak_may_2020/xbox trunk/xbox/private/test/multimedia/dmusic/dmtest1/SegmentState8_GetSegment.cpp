#include "globals.h"

//BVT
HRESULT SegmentState_GetSegment_BVT1(CtIDirectMusicPerformance8* ptPerf8);

//Valid
HRESULT SegmentState_GetSegment_OldSegmentReturnsNotFound(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT SegmentState_GetSegment_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function SegmentState_GetSegment_BVT())");

    DMTEST_EXECUTE(SegmentState_GetSegment_BVT1(ptPerf8));
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT SegmentState_GetSegment_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Valid test function SegmentState_GetSegment_Valid())");

    DMTEST_EXECUTE(SegmentState_GetSegment_OldSegmentReturnsNotFound(ptPerf8));

    return hr;
};


/********************************************************************************
Simply make sure the segment pointer obtained from a segmentstate matches the original
segment.
********************************************************************************/
HRESULT SegmentState_GetSegment_BVT1(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;

    CtIDirectMusicAudioPath             *ptPath          = NULL;
    CtIDirectMusicSegment               *ptSegment[2]       = {NULL};
    IDirectMusicSegment                 *pSegment[2]       = {NULL};
    CtIDirectMusicSegmentState            *ptSegmentState   = NULL;
    DWORD i = 0;

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&ptPath));
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment[0]));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0],0,0,0,0,&ptSegmentState,0,ptPath));
    CHECKRUN(ptSegmentState->GetSegment(&ptSegment[1]));


    for (i=0; i<2; i++)
    {
        CHECKRUN(ptSegment[i]->GetRealObjPtr(&pSegment[i]));
    }

    if (pSegment[0] != pSegment[1])
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "SegmentState_GetSegment_BVT1 ERROR: ");
        Log(ABORTLOGLEVEL, "GetSegment returned %08X, original segment was %08X", pSegment[0], pSegment[1]);
    }

   
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
        SAFE_RELEASE(pSegment[i]);
    }

    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptPath);
    return hr;
};



/********************************************************************************
Wait until a segment is done, then get the segment state from it.  Make sure it 
matches the original segment pointer.
********************************************************************************/
HRESULT SegmentState_GetSegment_OldSegmentReturnsNotFound(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;

    CtIDirectMusicAudioPath             *ptPath          = NULL;
    CtIDirectMusicSegment               *ptSegment[2]       = {NULL};
    CtIDirectMusicSegmentState            *ptSegmentState   = NULL;
    DWORD i = 0;
    LPSTR szFilename = "T:\\DMTest1\\Perf8\\SendPMsg\\OnePitch.sgt";
    HRESULT hrCall = S_OK;

    CHECKRUN(MediaCopyFile("DMusic/DMTest1/Perf8/SendPMsg/OnePitch.sgt", "T:\\DMTest1\\Perf8\\SendPMsg\\"));
    CHECKRUN(MediaCopyFile("DLS/Main1.dls",              "T:\\DMTest1\\Perf8\\SendPMsg\\"));

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&ptPath));
    CHECKRUN(dmthLoadSegment(szFilename, &ptSegment[0]));
    CHECKRUN(ptSegment[0]->SetRepeats(0)); 
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0],0,0,0,0,&ptSegmentState,0,ptPath));
    
    //Wait til this stops.
    CHECKRUN(ExpectSegmentStart(ptPerf8, NULL, ptSegmentState, 2000, szFilename));
    CHECKRUN(ExpectSegmentStop (ptPerf8, NULL, ptSegmentState, 5000, szFilename));
    //Wait 2 more seconds.
    Log(FYILOGLEVEL, "The music should have stopped by now.");
    Log(FYILOGLEVEL, "Waiting 2 more seconds before getting its SegmentState");
    CountDown(2000, 1000);
       
    if (SUCCEEDED(hr))
    {
        hrCall = ptSegmentState->GetSegment(&ptSegment[1]);
        if (DMUS_E_NOT_FOUND != hrCall)
        {
            hr = E_FAIL;
            Log(ABORTLOGLEVEL, "SegmentState_GetSegment returned %s instead of DMUS_E_NOT_FOUND after segment finished.", tdmXlatHRESULT(hrCall));
        }
    }

    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
    }

    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptPath);
    return hr;
};







/********************************************************************************
Simply make sure the segment pointer obtained from a segmentstate matches the original
segment.
********************************************************************************/
HRESULT SegmentState_GetSegment_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD bFull, DWORD dwUnused2)
{
    HRESULT hr = S_OK;

    IDirectMusicPerformance           *pPerf8       = NULL;
    IDirectMusicAudioPath             *pPath          = NULL;
    IDirectMusicSegment               *pSegment[2]       = {NULL};
    IDirectMusicSegmentState            *pSegmentState   = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;

    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf8));
    
    if (!bFull)
    {
        CHECKRUN(pPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&pPath));
        CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &pSegment[0]));
        CHECKRUN(pSegment[0]->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
        CHECKRUN(pPerf8->PlaySegmentEx(pSegment[0],0,0,0,0,&pSegmentState,0,pPath));
    }
    
    dwStartTime = timeGetTime();
    do
    {
        //Allocate stuff inside loop.
        if (bFull)
        {
            CHECKRUN(pPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&pPath));
            CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &pSegment[0]));
            CHECKRUN(pSegment[0]->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
            CHECKRUN(pPerf8->PlaySegmentEx(pSegment[0],0,0,0,0,&pSegmentState,0,pPath));
        }

        CHECKRUN(pSegmentState->GetSegment(&pSegment[1]));
        SAFE_RELEASE(pSegment[1]);

        //Release all the stuff inside the loop.
        if (bFull)
        {
            SAFE_RELEASE(pSegment[0]);
            SAFE_RELEASE(pSegmentState);
            SAFE_RELEASE(pPath);
        }


        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));


    //Final release.
    SAFE_RELEASE(pSegment[0]);
    SAFE_RELEASE(pSegmentState);
    SAFE_RELEASE(pPath);
    SAFE_RELEASE(pPerf8);


    return hr;
};
