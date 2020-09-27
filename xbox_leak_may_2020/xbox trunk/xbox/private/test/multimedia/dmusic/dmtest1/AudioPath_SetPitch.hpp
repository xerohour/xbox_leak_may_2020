#pragma once
 
#include "globals.h"


/********************************************************************************
IDirectMusicAudioPath::SetPitch()

HISTORY:
    Updated     04/05/00      danhaff - created.
********************************************************************************/
template <class T>
HRESULT tmplSetPitch_BVT_Direct(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2, T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    T   *ptInterface= NULL;
    long                        lValues[] = {-0x2000, -100, -1, 0, 1, 100, 0x1FFF};
    DWORD                       i = 0;


    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_DYNAMIC_MONO));

    //Get the IDirectMusicAudioPath interface.
    CHECKRUN(Music.GetInterface(&ptInterface));

    for (i=0; i<AMOUNT(lValues) && hr==S_OK; i++)
    {
        Log(FYILOGLEVEL, "Calling SetPitch(%d), playing for 2 seconds", lValues[i]);
        CHECKRUN(ptInterface->SetPitch(lValues[i], 0));
	    CHECKRUN(Wait(2000));
    }

   SAFE_RELEASE(ptInterface);
   return hr;
    
};




/********************************************************************************
IDirectMusicAudioPath::SetVolume()

HISTORY:
    Updated     04/05/00      danhaff - created.

//Ramp from 
********************************************************************************/
#define DSBPITCH_MIN -0x2000
#define DSBPITCH_MAX  0x1FFF


struct PITCHRAMP
{
LONG lFrom;
LONG lTo;
DWORD dwTime;
};


template <class T>
HRESULT tmplSetPitch_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwRange, T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    T     *ptInterface          = NULL;
    DWORD                       i = 0, j=0;
    LONG lRange = (LONG)dwRange;

    LONG lValue[3]    = {0};    
    lValue[0] = -lRange < DSBPITCH_MIN ? DSBPITCH_MIN : -lRange;
    lValue[2] =  lRange > DSBPITCH_MAX ? DSBPITCH_MAX : lRange;

    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_DYNAMIC_MONO));

    //Get the IDirectMusicAudioPath interface.
    CHECKRUN(Music.GetInterface(&ptInterface));

    
    for (i=0; i<AMOUNT(lValue) && SUCCEEDED(hr); i++)
        for (j=0; j<AMOUNT(lValue) && SUCCEEDED(hr); j++)
        {
            Log(FYILOGLEVEL, "Test %d/%d", i * AMOUNT(lValue) + j, AMOUNT(lValue) * AMOUNT(lValue));
            Log(FYILOGLEVEL, "Playing at lPitch=%d for 2 seconds, ramping from %d -> %d over %ums for 5 seconds, playing at %d for 5 seconds.",
                              lValue[i],
                              lValue[i],
                              lValue[j],
                              dwTime,
                              lValue[j]
                              );



            //Setting initial Pitch.
            CHECKRUN(ptInterface->SetPitch(lValue[i], 0));
            Log(FYILOGLEVEL, "SetPitch(%d, IMMEDIATE), waiting 2s", lValue[i]);
	        Wait (2000);

            //Setting Pitch curve.
            CHECKRUN(ptInterface->SetPitch(lValue[j], dwTime));
            Log(FYILOGLEVEL, "SetPitch(%d, %u), waiting 5s during ramp", lValue[j], dwTime);
	        Wait (5000);

            //Setting final Pitch.
            CHECKRUN(ptInterface->SetPitch(lValue[j], 0));
            Log(FYILOGLEVEL, "SetPitch(%d, IMMEDIATE), waiting 5s", lValue[j]);
	        Wait (5000);
        }

    SAFE_RELEASE(ptInterface);
    return hr;
    
};
