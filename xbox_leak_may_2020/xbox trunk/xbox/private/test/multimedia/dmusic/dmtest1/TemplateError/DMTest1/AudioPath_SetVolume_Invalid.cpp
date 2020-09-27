/********************************************************************************
FILE:
    AUDIOPATH8.cpp

PURPOSE:
    Contains AudioPath test functions.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"



/********************************************************************************
Main test function.
********************************************************************************/
HRESULT AudioPath_SetVolume_Invalid(CtIDirectMusicPerformance8* ptPerf8)
{

    HRESULT hr = S_OK;
    CICMusic Music;
    CtIDirectMusicAudioPath *ptPath = NULL;

    //Start the music playing.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_DYNAMIC_STEREO);
    if (S_OK != hr)
        goto TEST_END;

    hr = Music.GetInterface(&ptPath);

    hr = ptPath->SetVolume(-10001, 0);
    Log(FYILOGLEVEL, "ERROR!!!!!  Should have int3'd");


    hr = ptPath->SetVolume(1, 0);
    Log(FYILOGLEVEL, "ERROR!!!!!  Should have int3'd");


TEST_END:

    SAFE_RELEASE(ptPath);
    return hr;
};

