/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_CreateStandardAudioPath_AllCombos(CtIDirectMusicPerformance8* ptPerf8, DWORD bAllCombos, DWORD dwUnused2);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    CtIDirectMusicAudioPath *ptPath = NULL;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_CreateStandardAudioPath_BVT())");

    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          16,
                                          TRUE,
                                          &ptPath));

    RELEASE(ptPath);


    return hr;
};


/********************************************************************************
//Later we'll go through the entire audiopath and make sure we can get everything
//  out of it.  For now though we'll just play a bunch of stuff on them.

********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_AllCombos(CtIDirectMusicPerformance8* ptPerf8, DWORD bAllCombos, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath     = NULL;
DWORD dwStandardPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, 
                         DMUS_APATH_DYNAMIC_MONO,
                         DMUS_APATH_SHARED_STEREO,
                         DMUS_APATH_DYNAMIC_3D};
DWORD dwPChannelValues[] = {1, 16, 17, 999, 1000};
BOOL bActivateValues[] = {0, 99};
DWORD dwStandardPath = 0;
DWORD dwPChannelValue = 0;
DWORD dwActivateValue = 0;


    //Load Default Media.
    CHECKRUN(dmthLoadSegment(g_TestParams.szDefaultMedia, &ptSegment8));

    for (dwStandardPath = 0; dwStandardPath<AMOUNT(dwStandardPaths); dwStandardPath++)
    {
        for (dwPChannelValue=0; dwPChannelValue<AMOUNT(dwPChannelValues); dwPChannelValue++)
        {
            for (dwActivateValue=0; dwActivateValue<AMOUNT(bActivateValues); dwActivateValue++)
            {
                //Create the audiopath.
                Log(FYILOGLEVEL, "dwStandardPath = %s", dmthXlatDMUS_APATH(dwStandardPaths[dwStandardPath]));
                Log(FYILOGLEVEL, "dwPChannels    = %d", dwPChannelValues[dwPChannelValue]);
                Log(FYILOGLEVEL, "Activate       = %s", bActivateValues[dwActivateValue] ? "TRUE " : "FALSE");
                CHECKRUN(ptPerf8->CreateStandardAudioPath(dwStandardPaths[dwStandardPath],
                                                      dwPChannelValues[dwPChannelValue],
                                                      bActivateValues[dwActivateValue],
                                                      &ptPath));

                //If it's an active audiopath, download to and try to play the segment.
                if (bActivateValues[dwActivateValue])
                {

	                CHECKRUN(ptPerf8->PlaySegmentEx( 
		                ptSegment8, 
		                0,
		                NULL,
                        0, 
		                0,
		                NULL,
		                NULL,
		                ptPath));
                    Log(FYILOGLEVEL, "Playing segment on active audiopath for 3 seconds.");
                    Wait(3000);

                    //Stop the segment (is this necessary?)
	                CHECKRUN(ptPerf8->StopEx(ptSegment8, 0, 0));

                    if (FAILED(hr))
                        goto TEST_END;
                }
                
                //Release the audiopath.  The segment should stop playing.
                SAFE_RELEASE(ptPath);
            }

            //Break outta here if we're just doing one set.
            if (!bAllCombos)
                goto TEST_END;
        }
    }


TEST_END:
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return hr;
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
HRESULT Performance8_Play2AudioPaths(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath[2]          = {NULL};
CtIDirectMusicSegmentState         *ptSegmentState[2] = {NULL};
CtIDirectMusicSegment8              *ptSegment8[2]      = {NULL};
CHAR *szFileName[2]               = {"T:\\DMTest1\\Test.sgt", "T:\\DMTest1\\Test2.sgt"};
DWORD i, dwIteration;

MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };


    CHECKRUN(LoadMedia(MediaFileDesc));

    for (dwIteration = 0; dwIteration < 2  && SUCCEEDED(hr); dwIteration++)
    {

        //Create the audiopath.
        for (i=0; i<2 && SUCCEEDED(hr); i++)
        {
            CHECKRUN(ptPerf8->CreateStandardAudioPath(i ? DMUS_APATH_DYNAMIC_MONO : DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                                  i ? 32 :                      64,
                                                  TRUE,
                                                  &ptPath[i]));

            //Load a segment.
            CHECKRUN(dmthLoadSegment(szFileName[i], &ptSegment8[i]));

            //PlaySegment.
	        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8[i],
                                        0,
                                        0,
                                        i ? DMUS_SEGF_SECONDARY : 0,
                                        0,
                                        &ptSegmentState[i],
                                        0,
                                        ptPath[i]));
            CHECKRUN(Log(FYILOGLEVEL, " Playing %s as %s segment for 4 seconds.", szFileName[i], i ? "secondary " : "primary "));
        }

        //Moment of silence.
        CHECKRUN(Wait(4000));

        //Stop everything anyway.
        CHECKRUN(ptPerf8->StopEx(0, 0, 0));
        CHECKRUN(Log(FYILOGLEVEL, "Called StopEx(NULL, NULL, NULL), all sound should stop"));
        CHECKRUN(Wait(1000));

        //Release all items that might have been allocated.
        for (i=0; i<2; i++)
        {
            SAFE_RELEASE(ptSegment8[i]);
            SAFE_RELEASE(ptPath[i]);
            SAFE_RELEASE(ptSegmentState[i]);
        }

    }//end for dwIteration.

    //Release all items that might have been allocated.
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment8[i]);
        SAFE_RELEASE(ptPath[i]);
        SAFE_RELEASE(ptSegmentState[i]);
    }

    return hr;

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
HRESULT Performance8_CreateStandardAudioPath_Wait1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName               = g_TestParams.szDefaultMedia;
DWORD i = 0;


    //Create the audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath));

    //Load up media file.
    CHECKRUN(dmthLoadSegment(szFileName, &ptSegment8));

    //Play our segments.
    Log(FYILOGLEVEL, "Playing Segment:");
	CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                0,
                                ptPath));


    //Now both segments are playing.
    Log(FYILOGLEVEL, "Playing %s as %s segment.", szFileName, i ? "secondary " : "primary ");
    Log(FYILOGLEVEL, "Should hear music playing for 2 seconds.");
    Wait(2000);


    CHECKRUN(ptPerf8->StopEx(NULL, 0, 0));
    Log(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL)");


    //Moment of silence.
    Log(FYILOGLEVEL, "Waiting 4 seconds");
    Wait(4000);

    //Release all items that might have been allocated.
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return hr;

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
HRESULT Performance8_CreateStandardAudioPath_Wait2(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName               = g_TestParams.szDefaultMedia;
DWORD i = 0;


    //Create the audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath));

    //Load up media file.
    CHECKRUN(dmthLoadSegment(szFileName, &ptSegment8));

    //Play our segments.
    Log(FYILOGLEVEL, "Playing Segment:");
	CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                0,
                                ptPath));
    Log(FYILOGLEVEL, " Playing %s as %s segment.", szFileName, i ? "secondary " : "primary ");


    //Now both segments are playing.
    Log(FYILOGLEVEL, "Should hear music playing for 5 seconds.");
    Wait(5000);


    CHECKRUN(ptPerf8->StopEx(NULL, 0, 0));
    Log(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 4 seconds");

    //Release all items that might have been allocated.
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return hr;

};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_Wait(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_StopEx())");
    DMTEST_EXECUTE(Performance8_CreateStandardAudioPath_Wait1(ptPerf8, 0, 0));
    DMTEST_EXECUTE(Performance8_CreateStandardAudioPath_Wait2(ptPerf8, 0, 0));  
    return hr;
}




/********************************************************************************

********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_LimitPChannels(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
LPCSTR szFileNameServer = "DMusic/DMTest1/Perf8/CreateStandardAudioPath/PChannel_Limit/PChannel_Limit.sgt";
CHAR szFileName[MAX_PATH]= {0};
DWORD i = 0;
DWORD dwPChannelValues[] = {1, 2, 3, 4};


    CHECKRUN(MediaCopyDirectory(szFileNameServer));
    strcpy(szFileName, MediaServerToLocal(szFileNameServer));

    //Load up media file.
    CHECKRUN(dmthLoadSegment(szFileName, &ptSegment8));

    for (i=0; i<AMOUNT(dwPChannelValues); i++)
    {

        //Create the audiopath.
        CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                              dwPChannelValues[i],
                                              TRUE,
                                              &ptPath));

        //Play our segments.
        Log(FYILOGLEVEL, "Playing Segment:");
	    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8,
                                    0,
                                    0,
                                    0,
                                    0,
                                    NULL,
                                    0,
                                    ptPath));
        Log(FYILOGLEVEL, "Maximum PChannels on path is %d, should numbers up through %d spoken.", dwPChannelValues[i], dwPChannelValues[i]);
        Wait(2000);

        //Stop anything that's playin'.
        CHECKRUN(ptPerf8->StopEx(NULL, 0, 0));

        //Release all items that might have been allocated.
        SAFE_RELEASE(ptPath);

    }



    SAFE_RELEASE(ptSegment8);

    return hr;

};




/********************************************************************************

********************************************************************************/

HRESULT Performance8_CreateStandardAudioPath_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD bFull, DWORD dwAudioPath)
{
    HRESULT hr = S_OK;
    IDirectMusicPerformance *pPerf = NULL;
    IDirectMusicAudioPath *pPath = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;

    
    ASSERT(IsValidAudioPath(dwAudioPath));
    
    if (!bFull)
        CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));

    dwStartTime = timeGetTime();
    do
    {

        if (bFull)
            CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));

        CHECKRUN(pPerf->CreateStandardAudioPath(dwAudioPath,64,TRUE,&pPath));
        SAFE_RELEASE(pPath);
        
        
        if (bFull)
            SAFE_RELEASE(pPerf);

        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    SAFE_RELEASE(pPath);
    SAFE_RELEASE(pPerf);
    return S_OK;
};






/********************************************************************************

********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_PerfMem_Bug10385(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTempoFactor, DWORD dwAudioPath)
{
    HRESULT hr = S_OK;
    IDirectMusicPerformance *pPerf = NULL;
    IDirectMusicAudioPath *pPath = NULL;
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicSegment *pSegment = NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    float fTempoFactor = (float)dwTempoFactor;
    float fTempoFactorNormal = 1;
    LPCSTR szFile = g_TestParams.szDefaultMedia;
    CHAR szPath[MAX_PATH] = {0};

    ASSERT(IsValidAudioPath(dwAudioPath));    
    CHECKRUN(ChopPath(szFile, szPath, NULL));

    //Set the tempo factor.
    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));
    CHECKRUN(pPerf->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fTempoFactor, sizeof(fTempoFactor)));
    CHECKRUN(Log(FYILOGLEVEL, "Setting fNewTempoFactor to %g, will hear music at %g times normal tempo.", fTempoFactor, fTempoFactor));
    //CHECK(Wait(2000));

    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));
    CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
    CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));


    dwStartTime = timeGetTime();
    do
    {
        //Load a bunch of other stuff here too.        
        CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFile, (void **)&pSegment));
        CHECKRUN(pPerf->CreateStandardAudioPath(rand() % 2 ? DMUS_APATH_SHARED_STEREOPLUSREVERB : DMUS_APATH_DYNAMIC_MONO, 190, TRUE, &pPath));
        CHECKRUN(pPerf->PlaySegmentEx(pSegment, NULL, NULL, 0, 0, NULL, NULL, pPath));
        SAFE_RELEASE(pPath);
        SAFE_RELEASE(pSegment);
        CHECKRUN(pPerf->StopEx(0, 0, 0));
        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));

    SAFE_RELEASE(pPath);
    SAFE_RELEASE(pPerf);
    SAFE_RELEASE(pLoader);
    SAFE_RELEASE(pSegment);
    ptPerf8->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fTempoFactorNormal, sizeof(fTempoFactorNormal));
    //Sleep(2000);


    return S_OK;
};