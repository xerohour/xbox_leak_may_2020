#include "globals.h"
#include "cicmusicx.h"

/********************************************************************************
FILE:
    AudioPath_SetPitch.cpp

PURPOSE:
    Contains AudioPath test functions.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "AudioPath_SetPitch.hpp"


#define INTERFACE_NAME CtIDirectMusicAudioPath

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT AudioPath_SetPitch_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function AudioPath_SetPitch()");
    DMTEST_EXECUTE(tmplSetPitch_BVT_Direct<INTERFACE_NAME>(ptPerf8, 0, 0));
    return hr;
};



/********************************************************************************
Valid test function.
*******************************************************************************/
HRESULT AudioPath_SetPitch_Valid_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwRange)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function AudioPath_SetPitch())");
    DMTEST_EXECUTE(tmplSetPitch_Ramp<INTERFACE_NAME>(ptPerf8, dwTime, dwRange));
    return hr;
};


/********************************************************************************
*******************************************************************************/
HRESULT AudioPath_SetPitch_Valid_Immediate(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
//CHAR *szSegmentName = "T:\\DMTest1\\wav\\GuitarStream1.wav";
CHAR *szSegmentName = "T:\\WAV\\Streaming10s.wav";
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;

MEDIAFILEDESC MediaFileDesc[] = {
                                {"WAV/Streaming10s.wav", "T:\\WAV",    COPY_IF_NEWER},
                                {TERMINATE}
                                };


    CHECKRUN(LoadMedia(MediaFileDesc));

    //Load
    CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment));
//    CHECKRUN(ptSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_MONO, 16, TRUE, &ptPath));
    
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,NULL,0,ptPath));    
    Log(FYILOGLEVEL, "Playing %s for 1 second", PathFindFileName(szSegmentName));
    Wait(1000);
    Log(FYILOGLEVEL, "Calling SetPitch(-4800, 0), then playing for 2 more seconds");
    CHECKRUN(ptPath->SetPitch(-4800, 0));
    Wait(2000);
    Log(FYILOGLEVEL, "Calling SetPitch(0, 0), then playing for 15 more seconds.  You should hear me count to 10, then stop.");
    CHECKRUN(ptPath->SetPitch(0, 0));
    Wait(15000);

    //Stop the segment.
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    
    //Release everything.
    SAFE_RELEASE(ptSegment);        
    SAFE_RELEASE(ptPath);


return hr;
};





