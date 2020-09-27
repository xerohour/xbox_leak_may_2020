#pragma once
 
#include "globals.h"
#include "Help_Definitions.h"

/********************************************************************************
IDirectMusicAudioPath::SetVolume()

HISTORY:
    Updated     04/05/00      danhaff - created.
********************************************************************************/
template <class T>
HRESULT tmplSetVolume_Direct(CtIDirectMusicPerformance8* ptPerf8, T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    T     *ptInterface     = NULL;
    LONG                        lValues[] = {DSBVOLUME_MIN, DSBVOLUME_MAX, -1, -1000, -5000, -9999};
    DWORD                       i = 0;

    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_DYNAMIC_MONO));

    //Get the IDirectMusicAudioPath interface.
    CHECKRUN(Music.GetInterface(&ptInterface));

    //MIN
    for (i=0; i<AMOUNT(lValues) && hr==S_OK; i++)
    {
        Log(FYILOGLEVEL, "Calling SetVolume(%d), playing for 2 seconds", lValues[i]);
        CHECKRUN(ptInterface->SetVolume(lValues[i], 0));
	    Wait (2000);
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
struct VOLUMERAMP
{
LONG lVolFrom;
LONG lVolTo;
DWORD dwTime;
};

template <class T>
HRESULT tmplSetVolume_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwAudioPath, T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				= S_OK;
    CICMusic                    Music;
    T     *ptInterface          = NULL;
    DWORD                       i = 0, j=0;


    LONG lValue[2]    = {DSBVOLUME_MIN, DSBVOLUME_MAX};

    //Start the music playing.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwAudioPath));

    //Get the IDirectMusicAudioPath interface.
    CHECKRUN(Music.GetInterface(&ptInterface));

    
    for (i=0; i<2 && SUCCEEDED(hr); i++)
        for (j=0; j<2 && SUCCEEDED(hr); j++)
        {
            Log(FYILOGLEVEL, "Test %d/4", i * 2 + j);
            Log(FYILOGLEVEL, "Playing at %d for 2 seconds, ramping from %d -> %d over %ums for 5 seconds, playing at %d for 5 seconds.",
                              lValue[i],
                              lValue[i],
                              lValue[j],
                              dwTime,
                              lValue[j]
                              );



            //Setting initial volume.
            CHECKRUN(ptInterface->SetVolume(lValue[i], 0));
            Log(FYILOGLEVEL, "SetVolume(%d, IMMEDIATE), waiting 2s", lValue[i]);
	        Wait (2000);

            //Setting volume curve.
            CHECKRUN(ptInterface->SetVolume(lValue[j], dwTime));
            Log(FYILOGLEVEL, "SetVolume(%d, %u), waiting 5s during ramp", lValue[j], dwTime);
	        Wait (5000);

            //Setting final volume.
            CHECKRUN(ptInterface->SetVolume(lValue[j], 0));
            Log(FYILOGLEVEL, "SetVolume(%d, IMMEDIATE), waiting 5s", lValue[j]);
	        Wait (5000);
        }

    SAFE_RELEASE(ptInterface);
    return hr;
    
};





