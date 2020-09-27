/********************************************************************************
FILE:
    AUDIOPATH8.cpp

PURPOSE:
    Contains AudioPath test functions.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

DWORD AudioPath_SetVolume_Direct(CtIDirectMusicPerformance8* ptPerf8);
DWORD AudioPath_SetVolume_Ramp  (CtIDirectMusicPerformance8* ptPerf8);





/********************************************************************************
Main test function.
********************************************************************************/
HRESULT AudioPath_SetVolume (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function AudioPath_SetVolume())");
    CALLDMTEST(AudioPath_SetVolume_Direct(ptPerf8));
    CALLDMTEST(AudioPath_SetVolume_Ramp(ptPerf8));
    return hr;
};


/********************************************************************************
IDirectMusicAudioPath::SetVolume()

HISTORY:
    Updated     04/05/00      danhaff - created.
********************************************************************************/
DWORD AudioPath_SetVolume_Direct(CtIDirectMusicPerformance8* ptPerf8)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    CtIDirectMusicAudioPath     *ptAudioPath     = NULL;
    LONG                        lValues[] = {DSBVOLUME_MIN, DSBVOLUME_MAX, -1, -1000, -5000, -9999};
    DWORD                       i = 0;


    //Start the music playing.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_DYNAMIC_STEREO);
    if (S_OK != hr)
        goto TEST_END;

    //Get the IDirectMusicAudioPath interface.
    hr = Music.GetInterface(&ptAudioPath);
    if (S_OK != hr)
        goto TEST_END;

    //MIN
    for (i=0; i<AMOUNT(lValues); i++)
    {
        Log(FYILOGLEVEL, "Calling SetVolume(%d), playing for 2 seconds", lValues[i]);
        hr = ptAudioPath->SetVolume(lValues[i], 0);
        if (S_OK != hr)
        {
            dwRes = FNS_FAIL;
            goto TEST_END;
        }
	    Sleep (2000);
    }

    dwRes = FNS_PASS;

TEST_END:
   SAFE_RELEASE(ptAudioPath);

    return dwRes;
    
};




/********************************************************************************
IDirectMusicAudioPath::SetVolume()

HISTORY:
    Updated     04/05/00      danhaff - created.

//Ramp from 
********************************************************************************/
struct VOLUMERAMP
{
LONG lVolFrom;
LONG lVolTo;
DWORD dwTime;
};

DWORD AudioPath_SetVolume_Ramp(CtIDirectMusicPerformance8* ptPerf8)
{
	DWORD						dwRes			= FNS_ABORTED;
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    CtIDirectMusicAudioPath     *ptAudioPath     = NULL;
    DWORD                       i = 0;

    VOLUMERAMP vRamp[] = {
                          
                          //Long range, no time.
                          {DSBVOLUME_MIN, DSBVOLUME_MAX, 0},  
                          {DSBVOLUME_MAX, DSBVOLUME_MIN, 0},  

                          //Long range, short time.
                          {DSBVOLUME_MIN, DSBVOLUME_MAX, 1},  
                          {DSBVOLUME_MAX, DSBVOLUME_MIN, 1},  

                          //Long range, avg time.
                          {DSBVOLUME_MIN, DSBVOLUME_MAX, 1000},  
                          {DSBVOLUME_MAX, DSBVOLUME_MIN, 1000},  

                          //Long range, long time.
                          {DSBVOLUME_MIN, DSBVOLUME_MAX, 10000},  //Use these 2 to repro 2904.
                          {DSBVOLUME_MAX, DSBVOLUME_MIN, 10000},  

                          //Long range, INFINITE time.
                          {DSBVOLUME_MIN, DSBVOLUME_MAX, ULONG_MAX},   //Use these 2 to repro 2905.
                          {DSBVOLUME_MAX, DSBVOLUME_MIN, ULONG_MAX},  

                          //Short range, no time
                          {DSBVOLUME_MIN, DSBVOLUME_MIN + 1, 0},  
                          {DSBVOLUME_MAX, DSBVOLUME_MAX - 1, 0},  

                          //Short range, ININITE time.
                          {DSBVOLUME_MIN, DSBVOLUME_MIN + 1, ULONG_MAX},  
                          {DSBVOLUME_MAX, DSBVOLUME_MAX - 1, ULONG_MAX},  


                          //no range, no time
                          {DSBVOLUME_MIN, DSBVOLUME_MIN, 0},  
                          {DSBVOLUME_MAX, DSBVOLUME_MAX, 0},  
                          {5000,          5000,          0},  

                          //no range, INFINITE time
                          {DSBVOLUME_MIN, DSBVOLUME_MIN, ULONG_MAX},  
                          {DSBVOLUME_MAX, DSBVOLUME_MAX, ULONG_MAX},  
                          {5000,          5000,          ULONG_MAX}
                          };


    //Start the music playing.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_DYNAMIC_STEREO);
    if (S_OK != hr)
        goto TEST_END;

    //Get the IDirectMusicAudioPath interface.
    hr = Music.GetInterface(&ptAudioPath);
    if (S_OK != hr)
        goto TEST_END;

    
    for (i=0; i<AMOUNT(vRamp); i++)
    {
        Log(FYILOGLEVEL, "Test %d/%d", i, AMOUNT(vRamp));
        Log(FYILOGLEVEL, "Playing at %d for 2 seconds, ramping from %d -> %d over %dms for 5 seconds, playing at %d for 5 seconds.",
                          vRamp[i].lVolFrom,
                          vRamp[i].lVolFrom,
                          vRamp[i].lVolTo,
                          vRamp[i].dwTime,
                          vRamp[i].lVolTo
                          );
        
        Log(FYILOGLEVEL, "SetVolume(%d, IMMEDIATE), waiting 2s", vRamp[i].lVolFrom);
        hr = ptAudioPath->SetVolume(vRamp[i].lVolFrom, 0);
        if (S_OK != hr)
        {
            dwRes = FNS_FAIL;
            goto TEST_END;
        }
	    Sleep (2000);


        Log(FYILOGLEVEL, "SetVolume(%d, %u), waiting 5s during ramp", vRamp[i].lVolTo, vRamp[i].dwTime);
        hr = ptAudioPath->SetVolume(vRamp[i].lVolTo, vRamp[i].dwTime);
        if (S_OK != hr)
        {
            dwRes = FNS_FAIL;
            goto TEST_END;
        }
	    Sleep (5000);

        Log(FYILOGLEVEL, "SetVolume(%d, IMMEDIATE), waiting 5s", vRamp[i].lVolTo);
        hr = ptAudioPath->SetVolume(vRamp[i].lVolTo, 0);
        if (S_OK != hr)
        {
            dwRes = FNS_FAIL;
            goto TEST_END;
        }
	    Sleep (5000);
    }

    dwRes = FNS_PASS;


TEST_END:
   SAFE_RELEASE(ptAudioPath);

    return dwRes;
    
};




