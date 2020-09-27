#include "globals.h"
#include "Help_StatsMemoryUse.h"


/********************************************************************************
Play a wave segment and then send WAVEF_OFF!
/********************************************************************************/
/*
HRESULT Segment8_SetWavePlaybackParams_WAVEF_OFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegment, DWORD dwUnused)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
LPSTR szSegment = (LPSTR)dwSegment;


CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000));
CHECKRUN(Log(FYILOGLEVEL, "Segment %s will play for .5 seconds.", szSegment));
CHECKRUN(Wait(500));
CHECKRUN(ptSegment->SetWavePlaybackParams(DMUS_WAVEF_OFF, DMUS_READAHEAD_MIN));
CHECKRUN(Log(FYILOGLEVEL, "Segment %s should stop immediately", szSegment));
CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 2000, szSegment));
CHECKRUN(Wait(2000));

//Stop everything anyway.
ptPerf8->StopEx(0, 0, 0);

RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};
*/




struct SETWAVEPLAYBACKPARAMS_TESTCASE
{
    LPSTR szSegment;
    BOOL  bStreaming;
    DWORD dwReadAhead;
    DWORD dwExpectedMemDifference;
};


HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(CtIDirectMusicPerformance8* ptPerf8, SETWAVEPLAYBACKPARAMS_TESTCASE &TestCase);


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingOFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference)
{
    SETWAVEPLAYBACKPARAMS_TESTCASE TestCase = {0};
    TestCase.szSegment = "T:\\DMTest1\\Wav\\Streaming10s.wav";
    TestCase.bStreaming = FALSE;
    TestCase.dwReadAhead = dwReadAhead;
    TestCase.dwExpectedMemDifference = dwExpectedMemDifference;
    return Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(ptPerf8, TestCase);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Streaming10s_StreamingON(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference)
{
    SETWAVEPLAYBACKPARAMS_TESTCASE TestCase = {0};
    TestCase.szSegment = "T:\\DMTest1\\Wav\\Streaming10s.wav";
    TestCase.bStreaming = TRUE;
    TestCase.dwReadAhead = dwReadAhead;
    TestCase.dwExpectedMemDifference = dwExpectedMemDifference;
    return Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(ptPerf8, TestCase);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingOFF(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference)
{
    SETWAVEPLAYBACKPARAMS_TESTCASE TestCase = {0};
    TestCase.szSegment = "WAV/CountTo3.wav";
    TestCase.bStreaming = FALSE;
    TestCase.dwReadAhead = dwReadAhead;
    TestCase.dwExpectedMemDifference = dwExpectedMemDifference;
    return Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(ptPerf8, TestCase);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_CountTo3_StreamingON(CtIDirectMusicPerformance8* ptPerf8, DWORD dwReadAhead, DWORD dwExpectedMemDifference)
{
    SETWAVEPLAYBACKPARAMS_TESTCASE TestCase = {0};
    TestCase.szSegment = "WAV/CountTo3.wav";
    TestCase.bStreaming = TRUE;
    TestCase.dwReadAhead = dwReadAhead;
    TestCase.dwExpectedMemDifference = dwExpectedMemDifference;
    return Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(ptPerf8, TestCase);
}



/********************************************************************************
How do we verify whether a segment is streaming or not?
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_STREAMING_Generic(CtIDirectMusicPerformance8* ptPerf8, SETWAVEPLAYBACKPARAMS_TESTCASE &TestCase)
{
HRESULT hr = S_OK;
DWORD dwMemDifference = 0;
FLOAT fMaxErrorPercentage = 2.f;
DWORD i = 0;
CtIDirectMusicAudioPath *ptPath = NULL;
CtIDirectMusicSegment *ptSegment = NULL;
BOOL bStabilized = FALSE;
DWORD dwMemBefore = 0;
DWORD dwMemAfter = 0;
CHAR szSegment[MAX_PATH] = {0};
    
CHECKRUN(MediaCopyFile(TestCase.szSegment));
CHECK(strcpy(szSegment, MediaServerToLocal(TestCase.szSegment)));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
CHECKRUN(ptSegment->SetWavePlaybackParams(TestCase.bStreaming ? DMUS_WAVEF_STREAMING : 0, TestCase.dwReadAhead));

//Make sure memory use stays within 1000 bytes for 1 second.  Timeout after 5 seconds.
CHECKRUN(ExpectMemoryToStabilize(MEM_NORMAL, 1000, 1000, 5000, &bStabilized, &dwMemBefore));
CHECKRUN(ExpectMemoryToStabilize(MEM_NORMAL, 1000, 1000, 5000, &bStabilized, &dwMemAfter));
CHECK   (dwMemDifference = dwMemAfter - dwMemBefore);
RELEASE(ptSegment);
RELEASE(ptPath);


if (SUCCEEDED(hr))
{
    double fErrorFactor = CalculateErrorPercentage((double)TestCase.dwExpectedMemDifference, (double)dwMemDifference);        
    Log(FYILOGLEVEL, "Downloading %s wave %s (dwReadAhead = %u) increased memory by %u bytes",
                     TestCase.bStreaming ? "streaming" : "non-streaming",
                     szSegment,
                     TestCase.dwReadAhead,
                     dwMemDifference);
    Log(FYILOGLEVEL, "Expected memory increase was %u bytes.", TestCase.dwExpectedMemDifference);
    Log(FYILOGLEVEL, "Error factor = %g; Max allowed = %g", fErrorFactor, fMaxErrorPercentage);
    if (fErrorFactor > fMaxErrorPercentage)
    {
        Log(ABORTLOGLEVEL, "Error factor = %g exceeded the maximum allowed of %g", fErrorFactor, fMaxErrorPercentage);
        hr = E_FAIL;
    }
}

//Stop everything no mattah what.
ptPerf8->StopEx(0, 0, 0);
return hr;
};


/*
STDMETHOD(SetWavePlaybackParams)    (THIS_ DWORD dwFlags, DWORD dwReadAhead) PURE;

#define DMUS_WAVEF_OFF              0x01       // If wave is playing and this is the off message. 
#define DMUS_WAVEF_STREAMING        0x02       // If wave is streaming. 
#define DMUS_WAVEF_NOINVALIDATE     0x04       // Don't invalidate this wave. 
#define DMUS_WAVEF_NOPREROLL        0x08       // Don't preroll any wave data.   
#define DMUS_WAVEF_IGNORELOOPS      0x20       // Ignore segment looping. 
*/


/********************************************************************************
Play a wave segment and then verify that calling Performance8::Invalidate doesn't turn it off.
/********************************************************************************/
HRESULT Segment8_SetWavePlaybackParams_WAVEF_NOINVALIDATE(CtIDirectMusicPerformance8* ptPerf8, DWORD bNoInvalidate, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
LPSTR szSegment = "T:\\WAV\\Streaming10s.wav";
CHECKRUN(MediaCopyFile("WAV/Streaming10s.wav"));
CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000, szSegment));
CHECKRUN(ptSegment->SetWavePlaybackParams(bNoInvalidate ? DMUS_WAVEF_NOINVALIDATE : 0, DMUS_READAHEAD_MIN));


if (bNoInvalidate)
{
    CHECKRUN(Log(FYILOGLEVEL, "Segment %s will play for 8 seconds", szSegment));
}
else 
{
    CHECKRUN(Log(FYILOGLEVEL, "Segment %s will play for 4 seconds, then be Invalidated", szSegment));
}

CHECKRUN(Wait(4000));
CHECKRUN(ptPerf8->Invalidate(0, 0));
CHECKRUN(Log(FYILOGLEVEL, "Invalidate() has been called %s", bNoInvalidate ? "But wave should continue playing" : "and wave should have stopped"));
CHECKRUN(Wait(4000));

/* THIS PART IS NOT A VALID TEST, BECAUSE IF EVEN IF THE WAVE WERE INVALIDATED, THE SEGMENT WOULD STILL RETURN ISPLAYING.
if (SUCCEEDED(hr))
{
    HRESULT hrPlaying = E_FAIL;
    Wait(4000);
    hrPlaying = ptPerf8->IsPlaying(ptSegment, NULL);

    //Verify we're still playing.
    if (bNoInvalidate)
    {
        if (hrPlaying != S_OK)
        {
            Log(ABORTLOGLEVEL, "ERROR: IsPlaying(%s) returned %s but should have returned S_OK.", szSegment, tdmXlatHRESULT(hrPlaying));
            hr = E_FAIL;        
        }       
    }
    //Verify we've stopped.
    else
    {
        if (hrPlaying != S_FALSE)
        {
            Log(ABORTLOGLEVEL, "ERROR: IsPlaying(%s) returned %s but should have returned S_FALSE.", szSegment, tdmXlatHRESULT(hrPlaying));
            hr = E_FAIL;        
        }       
    }
}
*/

//Stop everything anyway.
ptPerf8->StopEx(0, 0, 0);

RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};




/********************************************************************************
/********************************************************************************/
#define CUTOFF 0
#define NOCUT 1
#define NOCHANGE 2
HRESULT Segment8_SetWavePlaybackParams_WAVEF_IGNORELOOPS(CtIDirectMusicPerformance8* ptPerf8, DWORD dwBehavior, DWORD dwChange)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
LPSTR szSegment[2] = {"T:\\DMTest1\\Segment8\\SetWavePlaybackParams\\LpWavInLpSegCUTOFF.sgt", "T:\\DMTest1\\Segment8\\SetWavePlaybackParams\\LpWavInLpSegCUTOFF.sgt"};
DWORD dwWaitSeconds = 10;
DWORD dwExpectedBehavior = 0xFFFFFFFF;
MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Segment8/SetWavePlaybackParams/LpWavInLpSeg/LpWavInLpSegCUTOFF.sgt",  "T:\\DMTest1\\Segment8\\SetWavePlaybackParams\\", COPY_IF_NEWER},
                                {"DMusic/DMTest1/Segment8/SetWavePlaybackParams/LpWavInLpSeg/LpWavInLpSegNOCUT.sgt",  "T:\\DMTest1\\Segment8\\SetWavePlaybackParams\\", COPY_IF_NEWER},
                                {TERMINATE}
                                };

if (NOCHANGE == dwChange)
    dwExpectedBehavior = dwBehavior;
else
    dwExpectedBehavior = dwChange;

CHECKRUN(LoadMedia(MediaFileDesc));
CHECKRUN(dmthLoadSegment(szSegment[dwBehavior], &ptSegment));
if (NOCHANGE != dwChange)
{
    CHECKRUN(ptSegment->SetWavePlaybackParams( (NOCUT == dwChange ) ? DMUS_WAVEF_IGNORELOOPS: 0, DMUS_READAHEAD_MIN));
}
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000, NULL));
CHECKRUN(Log(FYILOGLEVEL, "Wave file should %sbe cut off when the segment loops.", (CUTOFF == dwExpectedBehavior) ? "" : "NOT "));
CHECKRUN(Log(FYILOGLEVEL, "Waiting %d seconds", dwWaitSeconds));
CHECKRUN(Wait(dwWaitSeconds * 1000));
ptPerf8->StopEx(0, 0, 0);
RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};

#undef NOCHANGE
#undef CUTOFF
#undef NOCUT
