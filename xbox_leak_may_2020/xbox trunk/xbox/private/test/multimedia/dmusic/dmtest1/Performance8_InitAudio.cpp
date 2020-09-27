#include "globals.h"
#include "cicmusicx.h"



HRESULT Performance8_InitAudio_Params(CtIDirectMusicPerformance8* ptPerf8, DWORD dwDefaultPath, DWORD dwPChannels, LPSTR szSegment);


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_InitAudio_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Performance8_InitAudio())");

    //Not going to mess with a global performance; it's too risky.  If it's not global; we'll shut this
    //  one down and then re-open it.  If that doesn't work we'll create another one.
    if (InitPerformanceOnce())
        return S_OK;

    //TODO: Add some more tests in here!!!!  Wasn't test

    return hr;
};

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_InitAudio_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_InitAudio())");

    //Not going to mess with a global performance; it's too risky.  If it's not global; we'll shut this
    //  one down and then re-open it.  If that doesn't work we'll create another one.
    if (InitPerformanceOnce())
        return S_OK;


//    DMTEST_EXECUTE(Performance8_InitAudio_Params       (ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, g_TestParams.szDefaultMedia));

    return hr;
};




/********************************************************************************
********************************************************************************/
/*
HRESULT Performance8_InitAudio_Params(CtIDirectMusicPerformance8* ptPerf8, DWORD dwDefaultPath, DWORD dwPChannels, LPSTR szSegment)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8  *ptSegment8 = NULL;
   
    //Start up the performance.
    CHECKRUN(ptPerf8->InitAudioX(NULL, NULL));//pAudioParams));

    //Play your content!
	CHECKRUN(dmthPlayMidiFile(ptPerf8,CTIID_IDirectMusicSegment,&ptSegment8, NULL, szSegment));
    Log(FYILOGLEVEL, "Called InitAudio(dwDefaultPath = %s, dwPChannels = %d", szSegment);
    Log(FYILOGLEVEL, "Should hear %s playing for 3 seconds.", szSegment);
    Wait(3000);


    SAFE_RELEASE(ptSegment8);
    return hr;
};
*/
