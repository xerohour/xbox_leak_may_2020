#include "globals.h"

#define SEGMENTLENGTH 6  //Both measures and seconds.
#define REPEATS 2


HRESULT Segment8_SetStartPoint_Generic(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, MUSIC_TIME mtStartPoint, MUSIC_TIME mtLoopPoint1, MUSIC_TIME mtLoopPoint2);

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_DLS_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtDLS.sgt", DMUS_PPQ*4, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_DLS_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtDLS.sgt", DMUS_PPQ*4 * 2, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_DLS_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtDLS.sgt", DMUS_PPQ*4 * 5, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_DLS_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtDLS.sgt", DMUS_PPQ*4*SEGMENTLENGTH-1, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_DLS_End(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtDLS.sgt", DMUS_PPQ*4*SEGMENTLENGTH, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}



/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_WaveSeg_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtWaveTrack.sgt", DMUS_PPQ*4, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_WaveSeg_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtWaveTrack.sgt", DMUS_PPQ*4 * 2, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_WaveSeg_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtWaveTrack.sgt", DMUS_PPQ*4 * 5, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_WaveSeg_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtWaveTrack.sgt", DMUS_PPQ*4*SEGMENTLENGTH-1, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_WaveSeg_End(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/SegCountMtWaveTrack.sgt", DMUS_PPQ*4*SEGMENTLENGTH, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}



/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Wave_1Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav", DMUS_PPQ*4, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Wave_2Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav", DMUS_PPQ*4 * 2, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Wave_5Measure(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav", DMUS_PPQ*4 * 5, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Wave_EndMinus1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav", DMUS_PPQ*4*SEGMENTLENGTH-1, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Wave_End(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    return Segment8_SetStartPoint_Generic(ptPerf8, "DMusic/DMTest1/Segment8/GetSetStartPoint/CountTo6.wav", DMUS_PPQ*4*SEGMENTLENGTH, (MUSIC_TIME)dwLoopPoint1, (MUSIC_TIME)dwLoopPoint2);
}



/********************************************************************************
A variation of the SetClockTimeLoopPoints test.  Slightly more complex, however.
/********************************************************************************/
HRESULT Segment8_SetStartPoint_Generic(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegmentServer, MUSIC_TIME mtStartPoint, MUSIC_TIME mtLoopPoint1, MUSIC_TIME mtLoopPoint2)
{
    HRESULT         hr = S_OK;
    DWORD           dwExpectedPlayingTime = 0;
    REFERENCE_TIME  rtExpectedPlayingTime = 0;
    MUSIC_TIME      mtExpectedPlayingTime = 0;
    MUSIC_TIME      mtLoopPoint[2] = {mtLoopPoint1, mtLoopPoint2};
    BOOL            bUsingLoopPoints = (!(-1 == mtLoopPoint[0] && -1 == mtLoopPoint[1]));
    DWORD dwStartTime = 0;
    DWORD dwStopTime  = 0;
    DWORD dwPlayingTime = 0;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    FLOAT fErrorPercent = 0.f;
    FLOAT fMaxErrorPercent = 5.f;
    BOOL bClockTimeSegment = FALSE;
    REFERENCE_TIME rtDummy = 0;
    CHAR szSegment[MAX_PATH] = {0};

    ASSERT(szSegmentServer);
    strcpy(szSegment, MediaServerToLocal(szSegmentServer));    
    CHECKRUN(MediaCopyDirectory(szSegmentServer));

    CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
    CHECKRUN(ptSegment->GetClockTimeLength(&rtDummy, &bClockTimeSegment));

    //Validate that whatever we loaded is 6 measures long (= 6 seconds).
    if (SUCCEEDED(hr))
    {
        if (bClockTimeSegment)
        {
            ASSERT(rtDummy == SEGMENTLENGTH * 10000000);
        }
        else
        {
            MUSIC_TIME mtLength = 0;
            CHECKRUN(ptSegment->GetLength(&mtLength));
            if (SUCCEEDED(hr))
            {
                ASSERT(mtLength == SEGMENTLENGTH * 4 * DMUS_PPQ);
            }
        }
    }

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));    
    CHECKRUN(ptSegment->SetRepeats(REPEATS));


    //If we're not using loop points, then they are at the beginning and end of the segment.  Set the data appropriately.
    if (!bUsingLoopPoints)
    {
        mtLoopPoint[0] = 0;
        mtLoopPoint[1] = SEGMENTLENGTH * 4 * DMUS_PPQ;
    }

    if (bUsingLoopPoints)
    {
        CHECKRUN(ptSegment->SetLoopPoints(mtLoopPoint[0], mtLoopPoint[1]));
    }
    CHECKRUN(ptSegment->SetStartPoint(mtStartPoint));
    CHECK   (dwStartTime = timeGetTime());
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, 0, 0, 0, 0, 0, 0, ptPath));
    CHECKRUN(Log(FYILOGLEVEL, "Set loop points to %u and %u", mtLoopPoint[0], mtLoopPoint[1]));
    
    if (SUCCEEDED(hr))
    {
        //If we're a clock-time segment, or we're starting ahead of our loop point, then the length is fixed at 6 seconds minus wherever we started.
        if (bClockTimeSegment || mtStartPoint >= mtLoopPoint[1])
        {
            mtExpectedPlayingTime = SEGMENTLENGTH * 4 * DMUS_PPQ - mtStartPoint;
            ASSERT(mtExpectedPlayingTime >= 0);
        }
        
        //If we're a music-time segment, then the expected playing time varies based on repeats and start point.
        else
        {
            {
                MUSIC_TIME mtLoopRegion = 0;

                if (bUsingLoopPoints)
                    mtLoopRegion = mtLoopPoint[1] - mtLoopPoint[0];
                else
                    mtLoopRegion = SEGMENTLENGTH * 4 * DMUS_PPQ ;

                //Calculate how long the file SHOULD play given these loop points.  6 + (loop2 - loop1) * repeats
                mtExpectedPlayingTime = SEGMENTLENGTH * 4 * DMUS_PPQ +                   //Total segment time plus
                                        mtLoopRegion * REPEATS -                        //Time of looping region minus
                                        mtStartPoint;                                    //Start time.
            }
        }
    }

    //Wait for segment to start, before we can calculate ms (needs tempo).
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000, szSegment));

    //Validate that the tempo is 240 if we're a music_time segment.
    if (SUCCEEDED(hr))
    {
        if (!bClockTimeSegment)
        {
            double pfTempo = 0;
            CHECKRUN(GetTempo(ptPerf8, &pfTempo));
            if (SUCCEEDED(hr))
            {
                if (pfTempo != 240)
                {
                    Log(ABORTLOGLEVEL, "Error: Tempo is %g instead of 240", pfTempo);
                    hr = E_FAIL;
                }
            }
            else
            {
                Log(ABORTLOGLEVEL, "Error: Couldn't get the tempo!!!");
            }
        }
    }

    //Calculate expected milliseconds from MUSIC_TIME.
//    CHECKRUN(ptPerf8->MusicToReferenceTime(mtExpectedPlayingTime, &rtExpectedPlayingTime));
    CHECKRUN(MusicToReferenceTime(ptPerf8, mtExpectedPlayingTime, &rtExpectedPlayingTime));
    if (SUCCEEDED(hr))
    {
        ASSERT(rtExpectedPlayingTime < 0xFFFFFFFF);
        dwExpectedPlayingTime = DWORD(rtExpectedPlayingTime / 10000);        
    }


    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, dwExpectedPlayingTime * 4, szSegment));
    CHECK   (dwStopTime = timeGetTime());
    
    if (SUCCEEDED(hr))
    {
        dwPlayingTime = dwStopTime - dwStartTime;
        fErrorPercent = 100.f * FLOAT(fabs(LONG(dwPlayingTime - dwExpectedPlayingTime) / FLOAT(dwExpectedPlayingTime)));
        Log(FYILOGLEVEL, "Segment stopped at %ums, expected to stop at %ums.", dwPlayingTime, dwExpectedPlayingTime);
        Log(FYILOGLEVEL, "Error = %.2f%%, maximum allowed error is %.2f%%", fErrorPercent, fMaxErrorPercent);
        if (fErrorPercent > fMaxErrorPercent)
        {
            Log(ABORTLOGLEVEL, "FAILURE!!  %.2f%% exceeds maximum allowed error of %.2f%%!!!", fErrorPercent, fMaxErrorPercent);
            hr = E_FAIL;
        }
    }

    ptPerf8->StopEx(0, 0, 0);    
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);

    return hr;

};
