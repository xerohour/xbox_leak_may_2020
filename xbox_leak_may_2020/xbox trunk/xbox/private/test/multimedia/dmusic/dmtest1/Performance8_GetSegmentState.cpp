/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_GetSegmentState_BVT1(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetSegmentState_1(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_GetSegmentState_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_GetSegmentState_BVT())");

    DMTEST_EXECUTE(Performance8_GetSegmentState_BVT1(ptPerf8));                                            

    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_GetSegmentState_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_GetSegmentState_Valid())");

    DMTEST_EXECUTE(Performance8_GetSegmentState_1(ptPerf8));  //PASSES!                                           

    return hr;
};





/********************************************************************************
********************************************************************************/
HRESULT Performance8_GetSegmentState_BVT1(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT                     hr                  = S_OK;
    CtIDirectMusicSegment8      *ptSegment          = NULL;
    CtIDirectMusicSegmentState  *ptSegmentState     = NULL;
    MUSIC_TIME                  mt                  = 1024;
    CICMusic Music;

    // Call PlayMidiFile() convienence function
    //CHECKRUN(dmthPlayMidiFile(ptPerf8,CTIID_IDirectMusicSegment,&ptSegment, &ptSegmentState, NULL));

    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));

    //Wait til it starts playing.
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE));

    // Call GetTime() and check results
    CHECKRUN(ptPerf8->GetTime( NULL, &mt ));

    // Call GetSegmentState and check resutls
    CHECKRUN(ptPerf8->GetSegmentState( &ptSegmentState, mt));

    // Call Stop and check results
    CHECKRUN(Wait(3000));
    CHECKRUN(ptPerf8->StopEx(NULL, __int64(0), 0));


    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptSegment);

    return hr;

}




/******************************************************************************************
******************************************************************************************/
HRESULT Performance8_GetSegmentState_1(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT                     hr              = S_OK;
    CtIDirectMusicSegment8      *pwSegment      = NULL;
    CtIDirectMusicSegmentState  *pwSegmentState = NULL;
    CtIDirectMusicAudioPath     *pwAudioPath    = NULL;
    MUSIC_TIME                  mt              = 0;

    CHAR                        *pSegmentFile   = g_TestParams.szDefaultMedia;

    // Get the Segment object
    CHECKRUN(dmthLoadSegment(pSegmentFile, &pwSegment));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, TRUE, &pwAudioPath));

    // Call GetTime to get the current time and check results
    CHECKRUN(ptPerf8->GetTime(NULL, &mt));

    //Queue segment for a ways in advance.
    Log(FYILOGLEVEL, "MUSIC_TIME is         %d.", mt);
    mt = mt + 1000000;
    Log(FYILOGLEVEL, "Queuing segment for   %d.", mt);


    // Call PlaySegment way into the future and check results
    CHECKRUN(ptPerf8->PlaySegmentEx(pwSegment, NULL, NULL, DMUS_SEGF_BEAT, __int64(mt), NULL, NULL, pwAudioPath));

    // Call GetSegmentState way in the future
    CHECKRUN(ptPerf8->GetSegmentState(&pwSegmentState, mt));
    Log(FYILOGLEVEL, "Getting segment state at %d.", mt);
    Wait(5000);
        
    // CLEANUP
    SAFE_RELEASE(pwSegment);
    SAFE_RELEASE(pwSegmentState);
    SAFE_RELEASE(pwAudioPath);
    return hr; 
} 
