/********************************************************************************

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"



/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_IsPlaying_Invalid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    CICMusic Music;
    HRESULT hr = S_OK;

    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, NULL);
    Log(FYILOGLEVEL, "INT3 INT3 INT3 INT3 INT3 ON DEBUG!!!!");

    hr = ptPerf8->IsPlaying(NULL, NULL);
    Log(FYILOGLEVEL, "INT3 INT3 INT3 INT3 INT3 ON DEBUG!!!!");


    return S_OK;
};

