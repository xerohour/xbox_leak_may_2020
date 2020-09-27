#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_SetGlobalParam_Groove_BVT(TESTPARAMS);
DWORD Performance8_SetGlobalParam_Tempo_BVT(TESTPARAMS);
DWORD Performance8_SetGlobalParam_Volume_BVT(TESTPARAMS);
/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_SetGlobalParam (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_SetGlobalParam())");

    //THESE ALL PASS!
    CALLDMTEST(Performance8_SetGlobalParam_Groove_BVT(ptPerf8, 0, 0, 0, 0));
    CALLDMTEST(Performance8_SetGlobalParam_Tempo_BVT (ptPerf8, 0, 0, 0, 0));  
    CALLDMTEST(Performance8_SetGlobalParam_Volume_BVT(ptPerf8, 0, 0, 0, 0));

    return hr;
};



/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_SetGlobalParam_Groove_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    char dwOldMasterGrooveLevel                               = 0;
    char dwNewMasterGrooveLevel                            = 0;
    CHAR *szSegment = "T:\\Media\\Perf8\\SetGlob\\Groove.sgt";
    char i = 0;


    //Set the groove level to 1.
/*
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&dwOldMasterGrooveLevel, sizeof(dwOldMasterGrooveLevel));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
*/

    //BUGBUG: Remove when 2921 is fixed.
    dwOldMasterGrooveLevel = 0;

    //Start music playing.
    hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }        
    Music.GetInterface(&ptSegment8);
    hr = ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetRepeats returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }
    fnsLog(FYILOGLEVEL, "Playing %s normally for 2 seconds.", szSegment);
    Sleep(2000);


    //Go through 5 groove levels.
    for (i=0; i<=4; i++)
    {
        dwNewMasterGrooveLevel = i;
        hr = ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&dwNewMasterGrooveLevel, sizeof(dwNewMasterGrooveLevel));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        fnsLog(FYILOGLEVEL, "Setting dwNewMasterGrooveLevel to %d.  Should switch to ""%d"" being spoken at next measure break.", i, i+1);
        Sleep(5000);
    }

    //Set the old one again.
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&dwOldMasterGrooveLevel, sizeof(dwOldMasterGrooveLevel));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    



    dwRes = FNS_PASS;

TEST_END:
    SAFE_RELEASE(ptSegment8);
    return  dwRes;
};





/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_SetGlobalParam_Tempo_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    FLOAT fOldTempo                               = 0;
    FLOAT fNewTempo                            = 0;
    CHAR *szSegment = g_szDefaultMedia;
    DWORD i = 0;

/*
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
*/

    //BUGBUG: Remove when 2921 is fixed.
    fOldTempo = 1.f;


    //Start music playing.
    hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }        
    Music.GetInterface(&ptSegment8);
    hr = ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetRepeats returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }
    fnsLog(FYILOGLEVEL, "Playing %s normally for 2 seconds.", szSegment);
    Sleep(2000);


    //Set the tempo up way high.
    fNewTempo = 2.0;
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempo, sizeof(fNewTempo));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    fnsLog(ABORTLOGLEVEL, "Setting fNewTempo to %g, will hear music at %g times normal for 5 seconds.", fNewTempo, fNewTempo);
    Sleep(5000);


    //Set the old one again.
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    fnsLog(ABORTLOGLEVEL, "Setting tempo back to %g, will hear normal music for 3 seconds.", fOldTempo);
    Sleep(3000);



    dwRes = FNS_PASS;

TEST_END:
    SAFE_RELEASE(ptSegment8);
    return  dwRes;
};



/********************************************************************************
********************************************************************************/
DWORD Performance8_SetGlobalParam_Volume_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    long  lOldVolume                               = 0;
    long lNewVolume                                = 0;
    CHAR *szSegment = g_szDefaultMedia;
    DWORD i = 0;

/*
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
*/

    //BUGBUG: Remove when 2921 is fixed.
    lOldVolume = 0;


    //Start music playing.
    hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }        
    Music.GetInterface(&ptSegment8);
    hr = ptSegment8->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetRepeats returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }
    fnsLog(FYILOGLEVEL, "Playing %s normally for 2 seconds.", szSegment);
    Sleep(2000);


    //Set the Volume down way low.
    lNewVolume = -1000;  //10 decibels.
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lNewVolume, sizeof(lNewVolume));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    fnsLog(ABORTLOGLEVEL, "Setting lNewVolume to %d, will hear quiet music for 5 seconds.", lNewVolume);
    Sleep(5000);


    //Set the old one again.
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    fnsLog(ABORTLOGLEVEL, "Setting Volume back to %d, will hear normal-volume music for 3 seconds", lOldVolume);
    Sleep(3000);

    dwRes = FNS_PASS;

TEST_END:
    SAFE_RELEASE(ptSegment8);
    return  dwRes;
};




