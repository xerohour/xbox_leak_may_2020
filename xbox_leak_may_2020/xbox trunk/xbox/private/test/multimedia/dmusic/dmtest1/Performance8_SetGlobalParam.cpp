#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_SetGlobalParam_Groove_BVT(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_SetGlobalParam_Tempo1(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, FLOAT fNewTempo);
HRESULT Performance8_SetGlobalParam_Volume1(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, long lNewVolume);





/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_SetGlobalParam_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_SetGlobalParam_BVT())");

    DMTEST_EXECUTE(Performance8_SetGlobalParam_Groove_BVT(ptPerf8));
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 2.0f));  
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, -1000));
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_SetGlobalParam_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Valid test function Performance8_SetGlobalParam_Valid())");

    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 0.01f));  
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 0.10f));  
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 1.00f));  
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 10.f));  
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Tempo1 (ptPerf8, g_TestParams.szDefaultMedia, 100.f));  

    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, 0));
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, -100));
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, -1000));
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, -5000));
    DMTEST_EXECUTE(Performance8_SetGlobalParam_Volume1(ptPerf8, g_TestParams.szDefaultMedia, -10000));

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_SetGlobalParam_Groove_BVT(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    char bOldMasterGrooveLevel                               = 0;
    char dwNewMasterGrooveLevel                            = 0;
    CHAR *szSegmentServer = "DMusic/DMTest1/Perf8/SetGlob/Groove.sgt";
    char i = 0;

    CHECKRUN(MediaCopyDirectory(szSegmentServer));

    bOldMasterGrooveLevel = 0;

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, MediaServerToLocal(szSegmentServer), DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    Log(FYILOGLEVEL, "Playing %s normally for 2 seconds.", MediaServerToLocal(szSegmentServer));
    Wait(2000);


    //Go through 5 groove levels.
    for (i=0; i<=4; i++)
    {
        dwNewMasterGrooveLevel = i;
        CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&dwNewMasterGrooveLevel, sizeof(dwNewMasterGrooveLevel)));
        Log(FYILOGLEVEL, "Setting dwNewMasterGrooveLevel to %d.  Should switch to ""%d"" being spoken at next measure break.", i, i+1);
        Wait(5000);
    }

    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bOldMasterGrooveLevel, sizeof(bOldMasterGrooveLevel)));

    //CLEANUP
    SAFE_RELEASE(ptSegment8);
    return  hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_SetGlobalParam_Groove1(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    char bOldMasterGrooveLevel                               = 0;
    char bNewMasterGrooveLevel                            = 0;
    CHAR *szSegmentServer = "DMusic/DMTest1/Perf8/SetGlob/Groove.sgt";
    DWORD i = 0;
    char dwLevels[] = {0, 1, 10, 100};


    CHECKRUN(MediaCopyDirectory(szSegmentServer));

    bOldMasterGrooveLevel = 0;

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, MediaServerToLocal(szSegmentServer), DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    Log(FYILOGLEVEL, "Playing %s normally for 2 seconds.", MediaServerToLocal(szSegmentServer));
    Wait(2000);


    //Go through 100 groove levels.
    for (i=0; i<AMOUNT(dwLevels); i++)
    {
        bNewMasterGrooveLevel = dwLevels[i];
        CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bNewMasterGrooveLevel, sizeof(bNewMasterGrooveLevel)));
        Log(FYILOGLEVEL, "Setting bNewMasterGrooveLevel to %d for 3 seconds.");
        Wait(3000);
    }

    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bOldMasterGrooveLevel, sizeof(bOldMasterGrooveLevel)));

    //CLEANUP
    SAFE_RELEASE(ptSegment8);
    return  hr;
};




/********************************************************************************
********************************************************************************/
HRESULT Performance8_SetGlobalParam_Tempo1(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, FLOAT fNewTempo)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    FLOAT fOldTempo                               = 0;
    fOldTempo = 1.f;

    ASSERT(strcmp(g_TestParams.szDefaultMedia, szSegment) == 0);

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    Log(FYILOGLEVEL, "Playing %s normally for 2 seconds.", szSegment);
    Wait(2000);

    //Set the tempo up way high.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempo, sizeof(fNewTempo)));
    Log(FYILOGLEVEL, "Setting fNewTempo to %g, will hear music at %g times normal for 5 seconds.", fNewTempo, fNewTempo);
    Wait(5000);

    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo)));
    Log(FYILOGLEVEL, "Setting tempo back to %g, will hear normal music for 3 seconds.", fOldTempo);
    Wait(3000);

    //CLEANUP
    SAFE_RELEASE(ptSegment8);
    return  hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_SetGlobalParam_Volume1(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, long lNewVolume)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    long  lOldVolume                               = 0;

    ASSERT(strcmp(g_TestParams.szDefaultMedia, szSegment) == 0);

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    Log(FYILOGLEVEL, "Playing %s normally for 2 seconds.", szSegment);
    Wait(2000);

    //Set the Volume down way low.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lNewVolume, sizeof(lNewVolume)));
    Log(FYILOGLEVEL, "Setting lNewVolume to %d, will hear quiet music for 5 seconds.", lNewVolume);
    Wait(5000);

    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume)));
    Log(FYILOGLEVEL, "Setting Volume back to %d, will hear normal-volume music for 3 seconds", lOldVolume);
    Wait(3000);

    SAFE_RELEASE(ptSegment8);
    return  hr;
};




