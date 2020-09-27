#include "globals.h"


/********************************************************************************
Verify that calling SetClockTimeLoopPoints sets segment to clocktime mode
(we can verify this with GetClockTimeLoopPoint's 2nd param)
/********************************************************************************/
/*
HRESULT Segment8_SetClockTimeLoopPoints_SetsClockTimeMode(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
REFERENCE_TIME rtDummy = 0;
BOOL bClockTime = 0;
CtIDirectMusicSegment *ptSegment = NULL;

CHECKRUN(dmthLoadSegment("T:\\DMTest1\\Segment8\\GetSetClockTimeLoopPoints\\SegMusicTime2Bars.sgt", &ptSegment));

//Verify the segment is in music time.
CHECKRUN(ptSegment->GetClockTimeLength(&rtDummy, &bClockTime));
if (SUCCEEDED(hr))
{
    if (bClockTime)
    {
        Log(ABORTLOGLEVEL, "Error: Music time segment initially had CLOCK TIME mode set instead of music time");
        hr = E_FAIL;
    }
}

//Set some random loop points.
CHECKRUN(ptSegment->SetClockTimeLoopPoints(10000000, 20000000));

//Verify we transitioned to Clock Time.
CHECKRUN(ptSegment->GetClockTimeLength(&rtDummy, &bClockTime));

if (SUCCEEDED(hr))
{
    if (!bClockTime)
    {
        Log(ABORTLOGLEVEL, "Error: Music time segment didn't have CLOCK TIME mode set after SetClockTimeLoopPoints was called.");
        hr = E_FAIL;
    }
}

//Set music-time loop points.
CHECKRUN(ptSegment->SetClockTimeLoopPoints(768, 0));

//Verify we transitioned to Music Time.
CHECKRUN(ptSegment->GetClockTimeLength(&rtDummy, &bClockTime));
if (SUCCEEDED(hr))
{
    if (bClockTime)
    {
        Log(ABORTLOGLEVEL, "Error: Music time segment didn't return to MUSIC_TIME mode after SetClockTimeLoopPoints was called with rtEnd= 0");
        hr = E_FAIL;
    }
}



SAFE_RELEASE(ptSegment);
return hr;
};

*/



/********************************************************************************
Verify that calling SetClockTimeLoopPoints doesn't change clocktime length
/********************************************************************************/
#define DEFAULT_CLOCKTIME_LENGTH 200090000
HRESULT Segment8_SetClockTimeLoopPoints_KeepsLength(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
REFERENCE_TIME rtBefore = 0;
REFERENCE_TIME rtAfter  = 0;
BOOL bClockTime = 0;
CtIDirectMusicSegment *ptSegment = NULL;
LPCSTR szSegmentServer= "T:\\DMTest1\\Segment8\\GetSetClockTimeLoopPoints\\SegGetClockTime200090000.sgt";


CHECKRUN(MediaCopyFile(szSegmentServer));
CHECKRUN(dmthLoadSegment(MediaServerToLocal(szSegmentServer), &ptSegment));

//Verify the segment's initial clock time is correct.
CHECKRUN(ptSegment->GetClockTimeLength(&rtBefore, &bClockTime));
if (SUCCEEDED(hr))
{
    if (!bClockTime)
    {
        Log(ABORTLOGLEVEL, "Error: clocktime segment didn't initially have CLOCK TIME mode set.");
        hr = E_FAIL;
    }

    if (rtBefore != DEFAULT_CLOCKTIME_LENGTH)
    {
        Log(ABORTLOGLEVEL, "Error: initial length wasn't %d.", DEFAULT_CLOCKTIME_LENGTH);
        hr = E_FAIL;
    }

}

//Set some random loop points.
CHECKRUN(ptSegment->SetClockTimeLoopPoints(10000000, 20000000));
if (SUCCEEDED(hr))
{
    //Verify we transitioned to Clock Time.
    CHECKRUN(ptSegment->GetClockTimeLength(&rtAfter, &bClockTime));
    if (!bClockTime)
    {
        Log(ABORTLOGLEVEL, "Error: clocktime segment didn't have CLOCK TIME mode set after SetClockTimeLoopPoints was called.");
        hr = E_FAIL;
    }

    if (rtAfter != DEFAULT_CLOCKTIME_LENGTH)
    {
        Log(ABORTLOGLEVEL, "Error: clocktime length after calling SetClockTimeLoopPoints was %d instead of %d", rtAfter, DEFAULT_CLOCKTIME_LENGTH);
        hr = E_FAIL;        
    }
}


SAFE_RELEASE(ptSegment);
return hr;
};

#undef DEFAULT_CLOCKTIME_LENGTH




//Prototyping is cool.
HRESULT Segment8_SetClockTimeLoopPoints_Generic(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, DWORD dwLoopPoint1, DWORD dwLoopPoint2);

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetClockTimeLoopPoints_DLS(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    HRESULT hr = S_OK;
    LPSTR szServerFile = "DMusic/DMTest1/Segment8/GetSetClockTimeLoopPoints/SegCountDLS.sgt";
    CHECKRUN(MediaCopyDirectory(szServerFile));    
    CHECKRUN(Segment8_SetClockTimeLoopPoints_Generic(ptPerf8, MediaServerToLocal(szServerFile), dwLoopPoint1, dwLoopPoint2));
    return hr;
};

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetClockTimeLoopPoints_WaveSeg(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    HRESULT hr = S_OK;
    LPSTR szServerFile = "DMusic/DMTest1/Segment8/GetSetClockTimeLoopPoints/SegCountWaveTrack.sgt";
    CHECKRUN(MediaCopyDirectory(szServerFile));    
    CHECKRUN(Segment8_SetClockTimeLoopPoints_Generic(ptPerf8, MediaServerToLocal(szServerFile), dwLoopPoint1, dwLoopPoint2));
    return hr;
};

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_SetClockTimeLoopPoints_Wave(CtIDirectMusicPerformance8* ptPerf8, DWORD dwLoopPoint1, DWORD dwLoopPoint2)
{
    HRESULT hr = S_OK;
    LPSTR szServerFile = "DMusic/DMTest1/Segment8/GetSetClockTimeLoopPoints/CountTo6.wav";
    CHECKRUN(MediaCopyDirectory(szServerFile));    
    CHECKRUN(Segment8_SetClockTimeLoopPoints_Generic(ptPerf8, MediaServerToLocal(szServerFile), dwLoopPoint1, dwLoopPoint2));
    return hr;
};


