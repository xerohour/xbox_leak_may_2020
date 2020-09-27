#include "globals.h"
#include "cicmusicx.h"


#include "globals.h"
#include "cicmusicx.h"


HRESULT Performance8_GetGlobalParam_Groove_Range(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetGlobalParam_Tempo_Range(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetGlobalParam_Volume_Range(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_GetGlobalParam_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_GetGlobalParam_BVT()");  //THESE ALL PASS!!!
    DMTEST_EXECUTE(Performance8_GetGlobalParam_Groove_Range(ptPerf8));
    DMTEST_EXECUTE(Performance8_GetGlobalParam_Tempo_Range (ptPerf8));
    DMTEST_EXECUTE(Performance8_GetGlobalParam_Volume_Range(ptPerf8));
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_GetGlobalParam_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Performance8_GetGlobalParam()");
    return hr;
};

/********************************************************************************
********************************************************************************/
HRESULT Performance8_GetGlobalParam_Groove_Range(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    char bOldMasterGrooveLevel                               = 0;
    char bNewMasterGrooveLevel                            = 0;
    LPCSTR szSegmentServer = "DMusic/DMTest1/Perf8/SetGlob/Groove.sgt";
    CHAR szSegment[MAX_PATH] = {0};
    char i = 0;


    CHECKRUN(MediaCopyDirectory(szSegmentServer));
    strcpy(szSegment, MediaServerToLocal(szSegmentServer));

    //Set the groove level to 1.
    bOldMasterGrooveLevel = 1;

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));


    //Go through all the groove levels.
    for (i=1; i<=100; i++)
    {    
        bNewMasterGrooveLevel = i;
        if (i%20 == 0)
            Log(FYILOGLEVEL, "Setting bNewMasterGrooveLevel to %d", i);
        CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&i, sizeof(i)));
        CHECKRUN(ptPerf8->GetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bNewMasterGrooveLevel, sizeof(bNewMasterGrooveLevel)));
        if (bNewMasterGrooveLevel != i)
        {
            Log(ABORTLOGLEVEL, "Error: We set a groove level of %d and retrieved %d", i, bNewMasterGrooveLevel);
	        goto TEST_END;
        }
    }

    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bOldMasterGrooveLevel, sizeof(bOldMasterGrooveLevel)));
    Log(ABORTLOGLEVEL, "Setting groove level back to %d", bOldMasterGrooveLevel);
    Wait(1000);

TEST_END:
    SAFE_RELEASE(ptSegment8);
    return  hr;
};





/********************************************************************************
********************************************************************************/
HRESULT Performance8_GetGlobalParam_Tempo_Range(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    FLOAT fOldTempo                               = 0;
    FLOAT fNewTempo                            = 0;
    CHAR *szSegment = g_TestParams.szDefaultMedia;
    FLOAT f = 0;
    DWORD dwCounter = 0;

    //Set the tempo level to 1.
    fOldTempo = 1.f;

    //Start music playing.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    Wait(2000);


    //Set the tempo up way high.
    for (f = DMUS_MASTERTEMPO_MIN; f < DMUS_MASTERTEMPO_MAX && SUCCEEDED(hr); f+= (DMUS_MASTERTEMPO_MAX - DMUS_MASTERTEMPO_MIN) / 100)
    {

        //Logging
        if (dwCounter++ % 20 == 0)
        {
            Log(ABORTLOGLEVEL, "fNewTempo = %g.", f);
        }

        CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&f, sizeof(f)));
        CHECKRUN(ptPerf8->GetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempo, sizeof(fNewTempo)));
        if (!FloatsAreEqual(fNewTempo,f))
        {
            Log(ABORTLOGLEVEL, "Error: We set a tempo of %g and retrieved %g", f, fNewTempo);
            hr = E_FAIL;
        }
    }

    //Set the old one again, even if our test failed.    
    ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo));
    Log(ABORTLOGLEVEL, "Setting tempo back to %d", fOldTempo);
    Wait(1000);
    SAFE_RELEASE(ptSegment8);
    return  hr;
};



/********************************************************************************
d********************************************************************************/
HRESULT Performance8_GetGlobalParam_Volume_Range(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    long  lOldVolume                               = 0;
    long lNewVolume                            = 0;
    CHAR *szSegment = g_TestParams.szDefaultMedia;
    long i = 0;

    //Set the volume level to 0
    lOldVolume = 0;

    //Start music playing.
    Log(FYILOGLEVEL, "Playing %s normally.", szSegment);
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment8));
    CHECKRUN(ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE));

    //Set the Volume up way high.
    for (i = -200; i<=20; i++)
    {
        if (i%20==0)
        {
            Log(ABORTLOGLEVEL, "Setting Volume to %d", i);
        }

        CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&i, sizeof(i)));
        CHECKRUN(ptPerf8->GetGlobalParam(GUID_PerfMasterVolume, (void *)&lNewVolume, sizeof(lNewVolume)));
        if (i != lNewVolume)
        {
            Log(ABORTLOGLEVEL, "Error: We set a volume level of %d and retrieved %d", i, lNewVolume);
	        goto TEST_END;
        }
    }


    //Set the old one again.
    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume)));
    Log(ABORTLOGLEVEL, "Setting Volume back to %d", lOldVolume);
    Wait(1000);

TEST_END:
    SAFE_RELEASE(ptSegment8);
    return  hr;
};




