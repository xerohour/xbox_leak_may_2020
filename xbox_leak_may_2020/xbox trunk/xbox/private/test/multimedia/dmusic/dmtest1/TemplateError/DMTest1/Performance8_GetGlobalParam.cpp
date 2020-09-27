#include "globals.h"
#include "cicmusicx.h"


#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_GetGlobalParam_Groove_BVT(TESTPARAMS);
DWORD Performance8_GetGlobalParam_Tempo_BVT(TESTPARAMS);
DWORD Performance8_GetGlobalParam_Volume_BVT(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_GetGlobalParam (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_GetGlobalParam())");

    CALLDMTEST(Performance8_GetGlobalParam_Groove_BVT(ptPerf8, 0, 0, 0, 0));
    CALLDMTEST(Performance8_GetGlobalParam_Tempo_BVT (ptPerf8, 0, 0, 0, 0));
    CALLDMTEST(Performance8_GetGlobalParam_Volume_BVT(ptPerf8, 0, 0, 0, 0));

    return hr;
};


/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_GetGlobalParam_Groove_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    char bOldMasterGrooveLevel                               = 0;
    char bNewMasterGrooveLevel                            = 0;
    CHAR *szSegment = "T:\\Media\\Perf8\\SetGlob\\Groove.sgt";
    char i = 0;


    //Set the groove level to 1.
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bOldMasterGrooveLevel, sizeof(bOldMasterGrooveLevel));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    

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


    //Go through all the groove levels.
    for (i=0; i<=100; i++)
    {
    
        bNewMasterGrooveLevel = i;
        fnsLog(FYILOGLEVEL, "Setting bNewMasterGrooveLevel to %d", i);
        hr = ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&i, sizeof(i));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        hr = ptPerf8->GetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bNewMasterGrooveLevel, sizeof(bNewMasterGrooveLevel));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        if (bNewMasterGrooveLevel != i)
        {
            fnsLog(ABORTLOGLEVEL, "Error: We set a groove level of %d and retrieved %d", i, bNewMasterGrooveLevel);
	        goto TEST_END;
        }
    }

    //Set the old one again.
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bOldMasterGrooveLevel, sizeof(bOldMasterGrooveLevel));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    


    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};





/********************************************************************************
********************************************************************************/
DWORD Performance8_GetGlobalParam_Tempo_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    FLOAT fOldTempo                               = 0;
    FLOAT fNewTempo                            = 0;
    CHAR *szSegment = g_szDefaultMedia;
    FLOAT f = 0;

    //Set the groove level to 1.
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    

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
    Sleep(2000);


    //Set the tempo up way high.
    for (f = DMUS_MASTERTEMPO_MIN; f < DMUS_MASTERTEMPO_MAX; f+= (DMUS_MASTERTEMPO_MAX - DMUS_MASTERTEMPO_MIN) / 10)
    {
        fnsLog(ABORTLOGLEVEL, "Setting fNewTempo to %g.", f);
        hr = ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&f, sizeof(f));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        hr = ptPerf8->GetGlobalParam(GUID_PerfMasterTempo, (void *)&fNewTempo, sizeof(fNewTempo));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        if (fNewTempo != f)
        {
            fnsLog(ABORTLOGLEVEL, "Error: We set a tempo of %d and retrieved %d", f, fNewTempo);
	        goto TEST_END;
        }
    }

    //Set the old one again.
    fnsLog(ABORTLOGLEVEL, "Setting tempo back to %d", fOldTempo);
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fOldTempo, sizeof(fOldTempo));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    Sleep(1000);



    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};



/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_GetGlobalParam_Volume_BVT(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8  *ptSegment8                     = NULL;
    long  lOldVolume                               = 0;
    long lNewVolume                            = 0;
    CHAR *szSegment = g_szDefaultMedia;
    long i = 0;

    //Set the groove level to 1.
    hr = ptPerf8->GetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    

    //Start music playing.
    fnsLog(FYILOGLEVEL, "Playing %s normally.", szSegment);
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


    //Set the Volume up way high.
    for (i = -200; i<=20; i++)
    {
        fnsLog(ABORTLOGLEVEL, "Setting Volume to %d", i);
        hr = ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&i, sizeof(i));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        hr = ptPerf8->GetGlobalParam(GUID_PerfMasterVolume, (void *)&lNewVolume, sizeof(lNewVolume));
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "GetGlobalParam returned %s (%08Xh)", tdmXlatHRESULT(hr), hr);
	        goto TEST_END;
        }    

        if (i != lNewVolume)
        {
            fnsLog(ABORTLOGLEVEL, "Error: We set a volume level of %d and retrieved %d", i, lNewVolume);
	        goto TEST_END;
        }
    }


    //Set the old one again.
    fnsLog(ABORTLOGLEVEL, "Setting Volume back to %d", lOldVolume);
    hr = ptPerf8->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lOldVolume, sizeof(lOldVolume));
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "SetGlobalParam returned %s (%08Xh)", szSegment, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }    
    Sleep(1000);



    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};




