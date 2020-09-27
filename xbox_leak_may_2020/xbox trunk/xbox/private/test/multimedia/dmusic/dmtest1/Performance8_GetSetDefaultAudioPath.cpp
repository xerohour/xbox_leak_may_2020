#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_GetDefaultAudioPath_BVT_Same (CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetSetDefaultAudioPath_Valid_Independent(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_GetSetDefaultAudioPath_Valid_Dependent(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
BVT test function.  Set and get all the different types of paths.
********************************************************************************/
HRESULT Performance8_GetSetDefaultAudioPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_GetSetDefaultAudioPath())");
    DMTEST_EXECUTE(Performance8_GetDefaultAudioPath_BVT_Same (ptPerf8));
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_GetSetDefaultAudioPath_Valid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Performance8_GetSetDefaultAudioPath())");

    DMTEST_EXECUTE(Performance8_GetSetDefaultAudioPath_Valid_Independent(ptPerf8));
    DMTEST_EXECUTE(Performance8_GetSetDefaultAudioPath_Valid_Dependent(ptPerf8));  

    return hr;

};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_GetDefaultAudioPath_BVT_Same (CtIDirectMusicPerformance8* ptPerf8)
{

CtIDirectMusicAudioPath *ptAudioPath[2] = {NULL};
IDirectMusicAudioPath   *pAudioPath[2]  = {NULL};

DWORD dwPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_APATH_DYNAMIC_3D, DMUS_APATH_DYNAMIC_MONO, DMUS_APATH_SHARED_STEREO};
DWORD i,j = 0;
HRESULT hr = S_OK;

    //Set and get every type of default audiopath.
    for (i=0; i<AMOUNT(dwPaths) && SUCCEEDED(hr); i++)
    {
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPaths[i], 8, TRUE, &ptAudioPath[0]);
        CHECKRUN(ptPerf8->SetDefaultAudioPath(ptAudioPath[0]));
        CHECKRUN(ptPerf8->GetDefaultAudioPath(&ptAudioPath[1]));
    
        //Get the real object pointers out and compare them.
        for (j=0; j<2; j++)
        {
            CHECKRUN(ptAudioPath[j]->GetRealObjPtr(&pAudioPath[j])));
        }

        //Compare them.
        if (pAudioPath[0] != pAudioPath[1])
        {
            hr = E_FAIL;
            Log(ABORTLOGLEVEL, "FAIL: Pointers are %08X and %08X, but should be equal!", pAudioPath[0], pAudioPath[1]);
            goto END;
        }

        //Release both real and wrapped audiopaths.
        for (j=0; j<2; j++)
        {
            SAFE_RELEASE(ptAudioPath[j]);
            SAFE_RELEASE(pAudioPath[j]);
        }
    }

    //Destroy reference to the default audiopath.
    ptPerf8->SetDefaultAudioPath(NULL);

END:
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptAudioPath[i]);
        SAFE_RELEASE(pAudioPath[i]);
    }


return S_OK;

};





/********************************************************************************
1) Set the default audiopath when music is playing.on another audiopath.
********************************************************************************/
HRESULT Performance8_GetSetDefaultAudioPath_Valid_Independent(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptAudioPath = NULL;
    CICMusic Music;

    DWORD dwPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_APATH_DYNAMIC_3D, DMUS_APATH_DYNAMIC_MONO, DMUS_APATH_SHARED_STEREO};
    DWORD dwPath = 0;
    DWORD i = 0;

    //Start the music playing.
    dwPath = DMUS_APATH_SHARED_STEREOPLUSREVERB;
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwPath));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Log(FYILOGLEVEL, "Playing %s on a separate %s for 2 seconds", g_TestParams.szDefaultMedia, dmthXlatDMUS_APATH(dwPath)));
    CHECKRUN(Wait(2000));

    //Go through all the paths and back.
    for (i=0; i<AMOUNT(dwPaths)+1 && SUCCEEDED(hr); i++)
    {
        dwPath = dwPaths[i % AMOUNT(dwPaths)];
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPath, 8, TRUE, &ptAudioPath));
        CHECKRUN(ptPerf8->SetDefaultAudioPath(ptAudioPath));
        CHECKRUN(Log(FYILOGLEVEL, "Switching default audiopath to %s for 1 second, should hear no sound change.", dmthXlatDMUS_APATH(dwPath)));
        CHECKRUN(Wait(1000));
        SAFE_RELEASE(ptAudioPath);
    }

    //Destroy reference to the default audiopath.
    CHECKRUN(ptPerf8->SetDefaultAudioPath(NULL));

    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptAudioPath);
    return hr;
};




/********************************************************************************
1) Set the default audiopath when music is playing.on the default audiopath.
********************************************************************************/
HRESULT Performance8_GetSetDefaultAudioPath_Valid_Dependent(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptAudioPath = NULL;
    CICMusic Music;

    DWORD dwPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_APATH_DYNAMIC_3D, DMUS_APATH_DYNAMIC_MONO, DMUS_APATH_SHARED_STEREO};
    DWORD dwPath = 0;
    DWORD i = 0;

    //Start the music playing.
    dwPath = DMUS_APATH_SHARED_STEREOPLUSREVERB;
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, dwPath));
    CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
    CHECKRUN(Music.GetInterface(&ptSegment));
    
    //Set this one as the default one.
    CHECKRUN(Music.GetInterface(&ptAudioPath));
    CHECKRUN(ptPerf8->SetDefaultAudioPath(ptAudioPath));
    RELEASE(ptAudioPath);

    dwPath = DMUS_APATH_SHARED_STEREOPLUSREVERB;
	CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,
                                0,
                                0,
                                0,
                                __int64(0),
                                0,
                                0,
                                NULL));


    CHECKRUN(Log(FYILOGLEVEL, "Playing %s on a default %s for 2 seconds", g_TestParams.szDefaultMedia, dmthXlatDMUS_APATH(dwPath)));
    CHECKRUN(Wait(2000));

    //Go through all the paths and back.
    for (i=0; i<AMOUNT(dwPaths)+1 && SUCCEEDED(hr); i++)
    {
        dwPath = dwPaths[i % AMOUNT(dwPaths)];
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPath, 8, TRUE, &ptAudioPath));
        CHECKRUN(ptPerf8->SetDefaultAudioPath(ptAudioPath));
        CHECKRUN(Log(FYILOGLEVEL, "Switching default audiopath to %s for 2 seconds, should hear no sound change.", dmthXlatDMUS_APATH(dwPath)));
        CHECKRUN(Wait(2000));
        SAFE_RELEASE(ptAudioPath);
    }

    //Destroy reference to the default audiopath.
    ptPerf8->SetDefaultAudioPath(NULL);

    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptAudioPath);
    return hr;
};



/********************************************************************************

********************************************************************************/
HRESULT Performance8_GetSetDefaultAudioPath_Valid_UserCreated(CtIDirectMusicPerformance8* ptPerf8)
{
//TODO: When user-created paths become available, 

return S_OK;
};
