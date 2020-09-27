#include "globals.h"

HRESULT SegmentState_GetStartTime_CompareStartTimes(CtIDirectMusicPerformance8* ptPerf8, DWORD dwMaxError);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT SegmentState_GetStartTime_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function SegmentState_GetStartTime_BVT())");
    DMTEST_EXECUTE(SegmentState_GetStartTime_CompareStartTimes(ptPerf8, 1000));
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT SegmentState_GetStartTime_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function SegmentState_GetStartTime_Valid())");
    DMTEST_EXECUTE(SegmentState_GetStartTime_CompareStartTimes(ptPerf8, 10));
    return hr;
};



/********************************************************************************
Simply play a segment and make sure it started around the time you called play.
********************************************************************************/
HRESULT SegmentState_GetStartTime_CompareStartTimes(CtIDirectMusicPerformance8* ptPerf8, DWORD dwMaxError)
{
    HRESULT hr = S_OK;

    CtIDirectMusicAudioPath             *ptPath          = NULL;
    CtIDirectMusicSegment               *ptSegment       = {NULL};
    CtIDirectMusicSegmentState          *ptSegmentState   = NULL;
    MUSIC_TIME mtBase = 0;
    MUSIC_TIME mtStartTime = 0;
    MUSIC_TIME mtDiff = 0;
    DWORD dwMS = 0;
    double dblTempo = 0;

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&ptPath));
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment));
    CHECKRUN(ptPerf8->GetTime(NULL, &mtBase));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,__int64(0),&ptSegmentState,0,ptPath));
    Wait(500);
    CHECKRUN(ptSegmentState->GetStartTime(&mtStartTime));
    CHECKRUN(Log(FYILOGLEVEL, "Time obtained before PlaySegmentEx=%u; SegmentState::StartTime=%u", mtBase, mtStartTime));
    mtDiff = mtStartTime - mtBase;
    CHECKRUN(Log(FYILOGLEVEL, "Difference is %u MUSIC_TIME units", mtDiff));
    CHECKRUN(GetTempo(ptPerf8, &dblTempo));
    dwMS = DWORD(mtDiff * 60 * 1000 / dblTempo / DMUS_PPQ);
    CHECKRUN(Log(FYILOGLEVEL, "Difference is %d ms", dwMS));
    //fError = CalculateErrorPercentage(mtBase, mtStartTime);
    CHECKRUN(Log(FYILOGLEVEL, "Error is %d ms; Maximum allowed is %d ms", dwMS, dwMaxError));
    if (dwMS > dwMaxError)
    {
        Log(ABORTLOGLEVEL, "Exceeded maximum error factor!");
        hr = E_FAIL;
    }


    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptPath);
    return hr;
};
