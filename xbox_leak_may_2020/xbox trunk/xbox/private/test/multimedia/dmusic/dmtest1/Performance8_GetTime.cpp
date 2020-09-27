#include "globals.h"
#include "cicmusicx.h"

/********************************************************************************
We measure reference time in real time.
********************************************************************************/
HRESULT Performance8_GetTime_BVT_ReferenceTime(CtIDirectMusicPerformance8* ptPerf8, DWORD dwErrorMargin, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;

REFERENCE_TIME rtStart=0, rtEnd=0, rtDiff=0;
REFERENCE_TIME rtExpected = 50000000;    
FLOAT fError = 0.f;         //
FLOAT fErrorMargin = (FLOAT)dwErrorMargin;

    //Play some music just to get things going.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(ptPerf8->GetTime(&rtStart , NULL));
    Log(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", DWORD(rtStart >> 32), DWORD(rtStart & 0xFFFFFFFF));
    Log(FYILOGLEVEL, "Waiting 5s from call.");
    
    //Wait 5s.
    CHECKRUN(Wait(5000));
    CHECKRUN(ptPerf8->GetTime(&rtEnd, NULL));

    //Calculate the results and pass or fail the test.
    rtDiff = abs(int(rtStart - rtEnd));
    Log(FYILOGLEVEL, "%d reference units passed; %d were expected to pass.", (DWORD)rtDiff, (DWORD)rtExpected);    
    fError = (FLOAT)fabs((FLOAT(rtDiff) - FLOAT(rtExpected)) / FLOAT(rtExpected) * 100.f);
    Log(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fError, fErrorMargin);
    if (fError > fErrorMargin)
    {
        hr = E_FAIL;
        goto TEST_END;
    }
    


TEST_END:
    return hr;
};



/********************************************************************************
We measure music time in notes.  This segment has 16 quarter notes, so it should
last DMUS_PPQ * 16 units in music time.
********************************************************************************/
HRESULT Performance8_GetTime_BVT_MusicTime(CtIDirectMusicPerformance8* ptPerf8, DWORD dwErrorMargin, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegment8      *ptSegment8       = NULL;
CtIDirectMusicSegmentState  *ptSegmentState   = NULL;
CtIDirectMusicAudioPath     *ptPath           = NULL;

MUSIC_TIME mtStart=0, mtEnd=0, mtDiff=0;
MUSIC_TIME mtExpected = DMUS_PPQ * 16;    
FLOAT fError = 0.f;
FLOAT fErrorMargin = (FLOAT)dwErrorMargin;

MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/16Notes.sgt",          "T:\\DMTest1\\Perf8\\GetTime\\", COPY_IF_NEWER},
                                {"DLS/Main1.DLS",            "T:\\DMTest1\\Perf8\\GetTime\\", COPY_IF_NEWER},
                                {TERMINATE}
                                };

    CHECKRUN(LoadMedia(MediaFileDesc));


    //Play some music just to get things going.
    CHECKRUN(Music.Init(ptPerf8, "T:\\DMTest1\\Perf8\\GetTime\\16Notes.sgt", DMUS_APATH_SHARED_STEREOPLUSREVERB));

    //Get some interfaces.
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(Music.GetInterface(&ptPath));

    //Stop the music.  We'll use the length of a segment to measure time.
    CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));

    //Tell it not to repeat.
    CHECKRUN(ptSegment8->SetRepeats(0));

    //Play the segment
	CHECKRUN(ptPerf8->PlaySegmentEx( 
		ptSegment8, 
		0,
		NULL,
        0, 
		//DMUS_SEGF_QUEUE,
		__int64(0),
		&ptSegmentState,
		NULL,
		ptPath));

    CHECKRUN(ExpectSegmentStart(ptPerf8, NULL, ptSegmentState, 5000, "16Notes.sgt"));

    //Get the time.
    CHECKRUN(ptPerf8->GetTime(NULL, &mtStart));

    if (SUCCEEDED(hr))
    {
        Log(FYILOGLEVEL, "Pre-playback MUSIC_TIME is %08X", mtStart);
        Log(FYILOGLEVEL, "Waiting 5s from call.");
    }
    
    //Wait for the segment to finish.
    CHECKRUN(ExpectSegmentStop(ptPerf8, NULL, ptSegmentState, 20000, "16Notes.sgt"));
    CHECKRUN(ptPerf8->GetTime(NULL, &mtEnd));

    //Calculate the results and pass or fail the test.
    if (SUCCEEDED(hr))
    {
        Log(FYILOGLEVEL, "Post-playback MUSIC_TIME is %08X", mtEnd);
        mtDiff = abs(mtStart - mtEnd);
        Log(FYILOGLEVEL, "%d MUSIC_TIME units passed; %d were expected", (DWORD)mtDiff, (DWORD)mtExpected);    
        fError = (FLOAT)fabs(  (FLOAT(mtDiff) - FLOAT(mtExpected)) / FLOAT(mtExpected) * 100.f);
        Log(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fError, fErrorMargin);
        if (fError > fErrorMargin)
        {
            hr = E_FAIL;
            goto TEST_END;
        }
    }
    

TEST_END:
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptPath);
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_GetTime_Valid_InOrder(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
MUSIC_TIME mtLast = 0, mtThis = 0;
DWORD dwStart = 0;
HRESULT hr = S_OK;
HRESULT hrCompare = S_OK;
CICMusic Music;

    
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    
    //Get the time.
    dwStart = timeGetTime();
    CHECKRUN(ptPerf8->GetTime(NULL, &mtLast));
    do
    {

        CHECKRUN(ptPerf8->GetTime(NULL, &mtThis));
        CHECKRUN(Log(MAXLOGLEVEL, "Current MUSIC_TIME is %d", mtThis));

        if (SUCCEEDED(hr))
        {
            if (mtThis < mtLast)
            {
                Log(FYILOGLEVEL, "Error: MUSIC_TIME reversed by %d units", mtLast- mtThis);
                hrCompare = E_FAIL;
            }        
            mtLast = mtThis;
        }
    }
    while (timeGetTime() - dwStart < 5000 && SUCCEEDED(hr));

    if (FAILED(hrCompare))
        hr = E_FAIL;

    return hr;

}