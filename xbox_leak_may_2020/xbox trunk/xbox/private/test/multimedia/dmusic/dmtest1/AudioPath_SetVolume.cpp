/********************************************************************************
FILE:
    AudioPath_SetVolume.cpp

PURPOSE:
    Contains AudioPath test functions.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "AudioPath_SetVolume.hpp"


#define INTERFACE_NAME CtIDirectMusicAudioPath

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT AudioPath_SetVolume_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return tmplSetVolume_Direct<INTERFACE_NAME>(ptPerf8);
};



/********************************************************************************
Valid test function.
*******************************************************************************/
HRESULT AudioPath_SetVolume_Valid_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwAudioPath)
{
    return tmplSetVolume_Ramp<INTERFACE_NAME>(ptPerf8, dwTime, dwAudioPath);
};



/********************************************************************************
By NULL, Segment

Nothing playing: Works with (NULL, Segment, SegmentState, AudioPath)
One Segment:     Stops same (NULL, Segment, SegmentState, AudioPath);


Two Segments:
variables:
    AudioPath: Single / Separate
    Segments:  Same / Separate
    SegmentStates: There will be 2 segmentstates anyway.

 //There are 4 scenarios:
    Stop(NULL): Everything stops.
    Stop(Segment);
    Stop(SegmentState);
    Stop(AudioPath);

********************************************************************************/
HRESULT AudioPath_SetVolume_Valid_Ind(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD bShared)
{

HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath[2]          = {NULL};
CtIDirectMusicSegment8              *ptSegment8[2]      = {NULL};
CHAR *szFileName[2]               = {0};
DWORD i = 0;
DWORD dwScenario = 0; 
DWORD dwAudioPath = 0;

if (bShared)
    dwAudioPath = DMUS_APATH_SHARED_STEREOPLUSREVERB;
else
    dwAudioPath = DMUS_APATH_DYNAMIC_MONO;
    

//Load media.
MEDIAFILEDESC MediaFileDesc[]=
{
{"SGT/test.sgt",              "T:\\DMTest1",          COPY_IF_NEWER},
{"SGT/test2.sgt",             "T:\\DMTest1",          COPY_IF_NEWER},
{"DLS/Main1.DLS",             "T:\\DMTest1",          COPY_IF_NEWER},
{"WAV/Streaming10s.wav",      "T:\\WAV",    COPY_IF_NEWER},
{"WAV/PCM_ST_11_1.wav",       "T:\\WAV",    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));


if (!bWave)
{
    szFileName[0] = "T:\\DMTest1\\Test.sgt";
    szFileName[1] = "T:\\DMTest1\\Test2.sgt";
}
else
{
    szFileName[0] = "T:\\WAV\\Streaming10s.wav";
    szFileName[1] = "T:\\WAV\\PCM_ST_11_1.wav";
}


//Load up 1 or 2 segments.
for (i=0; i<2 && SUCCEEDED(hr); i++)
{
    //Load up media file.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(dwAudioPath,64,TRUE,&ptPath[i]));
    CHECKRUN(dmthLoadSegment(szFileName[i], &ptSegment8[i]));
}


//Play our segments for 3 seconds.
CHECKRUN(Log(FYILOGLEVEL, "Playing both segments for 3 seconds."));
for (i=0; i<2 && SUCCEEDED(hr); i++)
{


    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8[i],
                                0,
                                0,
                                i ? DMUS_SEGF_SECONDARY : 0,
                                __int64(0),
                                NULL,
                                0,
                                ptPath[i]));

    CHECKRUN(Log(FYILOGLEVEL, " Playing %s as %s segment.", szFileName[i], i ? "secondary " : "primary "));
}

//Path1: Playing
//Path2: Playing
CHECKRUN(Log(FYILOGLEVEL, " " ));
CHECKRUN(Log(FYILOGLEVEL, " Both segments should be audible."));
CHECKRUN(Wait(3000));

//Path1: Silent
//Path2: Playing
CHECKRUN(Log(FYILOGLEVEL, " " ));
CHECKRUN(Log(FYILOGLEVEL, " Silencing volume on audiopath 1"));
CHECKRUN(Log(FYILOGLEVEL, " Expected: "));
CHECKRUN(Log(FYILOGLEVEL, "    Path1: Silent" ));
CHECKRUN(Log(FYILOGLEVEL, "    Path2: Audible" ));
CHECKRUN(ptPath[0]->SetVolume(-10000, 0));
CHECKRUN(Wait(3000));


ptPerf8->StopEx(0, 0, 0);
CHECKRUN(Log(FYILOGLEVEL, "  Music should stop." ));
CHECKRUN(Log(FYILOGLEVEL, " " ));
CHECKRUN(Wait(1000));
//Release all items that might have been allocated.
for (i=0; i<2; i++)
{
    SAFE_RELEASE(ptSegment8[i]);
    SAFE_RELEASE(ptPath[i]);
}

return hr;
};







