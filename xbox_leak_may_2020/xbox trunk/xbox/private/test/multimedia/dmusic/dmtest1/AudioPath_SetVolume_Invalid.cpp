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
HRESULT AudioPath_SetVolume_Invalid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{

    HRESULT hr = S_OK;
    CICMusic Music;
    CtIDirectMusicAudioPath *ptPath = NULL;

    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_DYNAMIC_MONO));

    CHECKRUN(Music.GetInterface(&ptPath));

    CHECKRUN(ptPath->SetVolume(-10001, 0));
    Log(FYILOGLEVEL, "Should have int3'd");

    CHECKRUN(ptPath->SetVolume(1, 0));
    Log(FYILOGLEVEL, "Should have int3'd");

    SAFE_RELEASE(ptPath);
    return hr;
};

