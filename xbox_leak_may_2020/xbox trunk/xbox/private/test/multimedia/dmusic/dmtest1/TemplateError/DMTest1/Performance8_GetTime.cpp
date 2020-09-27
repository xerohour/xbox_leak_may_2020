#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_GetTime_ReferenceTime(TESTPARAMS);
DWORD Performance8_GetTime_MusicTime(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_GetTime (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_GetTime())");

    CALLDMTEST(Performance8_GetTime_ReferenceTime(ptPerf8, 0, 0, 0, 0));  //BUGBUG
    CALLDMTEST(Performance8_GetTime_MusicTime(ptPerf8, 0, 0, 0, 0));

    return hr;
};



/********************************************************************************
We measure reference time in real time.
********************************************************************************/
DWORD Performance8_GetTime_ReferenceTime(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CICMusic Music;

REFERENCE_TIME rtStart, rtEnd, rtDiff;
REFERENCE_TIME rtExpected = 50000000;    
FLOAT fErrorMargin = 2.f; //Allow 2% error.
FLOAT fError = 0.f;         //
DWORD dwStartTime = 0;
DWORD dwEndTime = 0;

    //Play some music just to get things going.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", g_szDefaultMedia, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }

    hr = ptPerf8->GetTime(&rtStart , NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetTime failed");
	    goto TEST_END;
    }

    dwStartTime = timeGetTime();
    fnsLog(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", rtStart >> 32, rtStart & 0xFFFFFFFF);
    fnsLog(FYILOGLEVEL, "Waiting 5s from call.");
    
    //Wait 5s.
    while (timeGetTime() - dwStartTime < 5000);

    hr = ptPerf8->GetTime(&rtEnd, NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetTime failed");
	    goto TEST_END;
    }


    //Calculate the results and pass or fail the test.
    rtDiff = abs(int(rtStart - rtEnd));
    //rtDiffAllowed = (REFERENCE_TIME)(FLOAT(rtExpected) * fErrorMargin);
    fnsLog(FYILOGLEVEL, "%d reference units passed; %d were expected to pass.", (DWORD)rtDiff, (DWORD)rtExpected);    
    fError = (FLOAT)fabs(FLOAT(rtDiff - rtExpected) / FLOAT(rtExpected) * 100.f);
    fnsLog(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fError, fErrorMargin);
    if (fError > fErrorMargin)
    {
        goto TEST_END;
    }
    

    dwRes = FNS_PASS;

TEST_END:
    return dwRes;
};



/********************************************************************************
We measure music time in notes.  This segment has 16 quarter notes, so it should
last DMUS_PPQ * 16 units in music time.
********************************************************************************/
DWORD Performance8_GetTime_MusicTime(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegment8      *ptSegment8       = NULL;
CtIDirectMusicSegmentState  *ptSegmentState   = NULL;
CtIDirectMusicAudioPath     *ptPath           = NULL;

MUSIC_TIME mtStart, mtEnd, mtDiff, mtError;
MUSIC_TIME mtExpected = DMUS_PPQ * 16;    
FLOAT fErrorMargin = 2.f; //Allow 2% error.
FLOAT fError = 0.f;
DWORD dwStartTime = 0;
DWORD dwEndTime   = 0;

    //Play some music just to get things going.
    hr = Music.Init(ptPerf8, "T:\\Media\\Perf8\\GetTime\\16Notes.sgt", DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", g_szDefaultMedia, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }

    //Get some interfaces.
    hr = Music.GetInterface(&ptSegment8);
    hr = Music.GetInterface(&ptSegmentState);
    hr = Music.GetInterface(&ptPath);

    //Stop the music.  We'll use the length of a segment to measure time.
    hr = ptPerf8->StopEx(0, 0, 0);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "StopEx failed with %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }

    //Tell it not to repeat.
    hr = ptSegment8->SetRepeats(0);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetRepeats failed with %s (%08Xh)", tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    //Play the segment
	hr = ptPerf8->PlaySegmentEx( 
		ptSegment8, 
		0,
		NULL,
        0, 
		//DMUS_SEGF_QUEUE,
		0,
		&ptSegmentState,
		NULL,
		ptPath);

	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx function returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    
    //Wait for the segment to start
    do
    {
        hr = ptPerf8->IsPlaying(NULL, ptSegmentState);
        if (FAILED(hr))
            goto TEST_END;
    }
    while (S_FALSE == hr);

    //Get the time.
    hr = ptPerf8->GetTime(NULL, &mtStart);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetTime failed");
	    goto TEST_END;
    }

    dwStartTime = timeGetTime();
    fnsLog(FYILOGLEVEL, "Pre-playback MUSIC_TIME is %08X", mtStart);
    fnsLog(FYILOGLEVEL, "Waiting 5s from call.");

    
    //Wait for the segment to finish.
    do
    {
        hr = ptPerf8->IsPlaying(NULL, ptSegmentState);
        if (FAILED(hr))
            goto TEST_END;
    }
    while (S_OK == hr);

    hr = ptPerf8->GetTime(NULL, &mtEnd);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetTime failed");
	    goto TEST_END;
    }


    //Calculate the results and pass or fail the test.
    fnsLog(FYILOGLEVEL, "Post-playback MUSIC_TIME is %08X", mtEnd);
    mtDiff = abs(mtStart - mtEnd);
//    mtError = (MUSIC_TIME)(FLOAT(mtExpected) * fErrorMargin);
    fnsLog(FYILOGLEVEL, "%d MUSIC_TIME units passed; %d were expected", (DWORD)mtDiff, (DWORD)mtExpected);    
    fError = (FLOAT)fabs(FLOAT(mtDiff - mtExpected) / FLOAT(mtExpected) * 100.f);
    fnsLog(FYILOGLEVEL, "Error is %6.2f percent, maximum allowed is %6.2f percent.", fError, fErrorMargin);
    if (fError > fErrorMargin)
    {
        goto TEST_END;
    }
    

    dwRes = FNS_PASS;

TEST_END:
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptPath);
    return dwRes;
};
