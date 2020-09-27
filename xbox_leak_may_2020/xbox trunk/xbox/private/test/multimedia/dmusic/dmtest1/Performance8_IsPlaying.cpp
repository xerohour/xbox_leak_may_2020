/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"


HRESULT Performance8_IsPlaying_Same(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_IsPlaying_Transition(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_IsPlaying_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_IsPlaying_BVT())");
    DMTEST_EXECUTE(Performance8_IsPlaying_Same(ptPerf8));                               
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_IsPlaying_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_IsPlaying_Valid())");
    DMTEST_EXECUTE(Performance8_IsPlaying_Transition(ptPerf8));

    return hr;
};




/********************************************************************************
********************************************************************************/
HRESULT Performance8_IsPlaying_Same(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegmentState8 *ptSegmentState8 = NULL;
CtIDirectMusicSegment8 *ptSegment8 = NULL;


    //Load up and play our segment.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    //Get our segment and segment state.
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptSegmentState8));
    CHECKRUN(ptPerf8->IsPlaying(ptSegment8, NULL));
    CHECKRUN(ptPerf8->IsPlaying(NULL, ptSegmentState8));
    CHECKRUN(ptPerf8->IsPlaying(ptSegment8, ptSegmentState8));
    Log(ABORTLOGLEVEL, "Waiting 4 seconds, you should hear music playing");
    Wait(4000);

    //Stop immediately.
    CHECKRUN(ptPerf8->StopEx(ptSegment8, __int64(0), 0));
    CHECKRUN(SUCCEED_ON_FAIL(ptPerf8->IsPlaying(ptSegment8, NULL), S_FALSE));
    CHECKRUN(SUCCEED_ON_FAIL(ptPerf8->IsPlaying(NULL, ptSegmentState8), S_FALSE));
    CHECKRUN(SUCCEED_ON_FAIL(ptPerf8->IsPlaying(ptSegment8, ptSegmentState8), S_FALSE));
    Log(ABORTLOGLEVEL, "Waiting 4 seconds, you should hear no music because we just called StopEx(ptSegment8)");
    Wait(4000);

    SAFE_RELEASE(ptSegmentState8);
    SAFE_RELEASE(ptSegment8);

    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_IsPlaying_Transition(CtIDirectMusicPerformance8* ptPerf8)
{

HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegmentState8 *ptSegmentState[2] = {NULL};
CtIDirectMusicSegment8 *ptSegment          [2] = {NULL};
CtIDirectMusicAudioPath *ptAudioPath           = NULL;

MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //Load up and play our segment.
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\test.sgt", DMUS_APATH_SHARED_STEREOPLUSREVERB));

    //Get our segment and segment state.
    CHECKRUN(Music.GetInterface(&ptSegment[0]));
    CHECKRUN(Music.GetInterface(&ptSegmentState[0]));
    CHECKRUN(Music.GetInterface(&ptAudioPath));

    //Load up a different segment.
    CHECKRUN(dmthLoadSegment("T:\\DMTest1\\test2.sgt", &ptSegment[1]));
    
    //Do a transition to the next segment on the next measure.
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[1],
                            0,
                            0,
                            DMUS_SEGF_MEASURE,
                            __int64(0),
                            &ptSegmentState[1],
                            0,
                            ptAudioPath));

    //Since it hasn't transitioned yet, the first segment should still be playing.
    CHECKRUN(ptPerf8->IsPlaying(ptSegment[0], NULL));    
    CHECKRUN(ptPerf8->IsPlaying(NULL, ptSegmentState[0]));
    CHECKRUN(ptPerf8->IsPlaying(ptSegment[0], ptSegmentState[0]));
    if (hr != S_OK)
    {
        Log(ABORTLOGLEVEL, "Segment[0]->IsPlaying returned %s even though it hasn't transitioned yet", dmthXlatHRESULT(hr));
        hr = E_FAIL;
        goto END;
    }

    //Wait for this segment to stop playin'.  This means the other one has to be playin'.
    CHECKRUN(WaitForSegmentStop(ptPerf8, ptSegment[0], NULL, 20000, FALSE));

    //Since the "From" segment has stopped, the "To" segment had better be playing.
    CHECKRUN(ptPerf8->IsPlaying(ptSegment[1], NULL));    
    CHECKRUN(ptPerf8->IsPlaying(NULL, ptSegmentState[1]));
    CHECKRUN(ptPerf8->IsPlaying(ptSegment[1], ptSegmentState[1]));
    if (hr != S_OK)
    {
        Log(ABORTLOGLEVEL, "Segment[1]->IsPlaying returned %s even though its precursor has stopped.", dmthXlatHRESULT(hr));
        hr = E_FAIL;
        goto END;
    }
    
END:
    Log(FYILOGLEVEL, "Playing for 2 seconds");
    Wait(2000);

    //Release what we've acquired.
    for (int i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
        SAFE_RELEASE(ptSegmentState[i]);
    }

    SAFE_RELEASE(ptAudioPath);
    return hr;
};
