/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_CreateStandardAudioPath_BVT(TESTPARAMS);
DWORD Performance8_Play2AudioPaths(TESTPARAMS);
HRESULT Performance8_CreateStandardAudioPath_Sleep(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_CreateStandardAudioPath())");

    CALLDMTEST(Performance8_CreateStandardAudioPath_BVT(ptPerf8, 0, 0, 0, 0));  //Fails due to 2762
    CALLDMTEST(Performance8_Play2AudioPaths(ptPerf8, 0, 0, 0, 0));              //PASSES!
    CALLDMTEST(Performance8_CreateStandardAudioPath_Sleep(ptPerf8));         //Fails due to 2987

    return hr;
};



/********************************************************************************
//Later we'll go through the entire audiopath and make sure we can get everything
//  out of it.  For now though we'll just play a bunch of stuff on them.

********************************************************************************/
DWORD Performance8_CreateStandardAudioPath_BVT(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
CtIDirectMusicAudioPath *ptPath     = NULL;
DWORD dwStandardPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, 
                         DMUS_APATH_DYNAMIC_MONO,
                         DMUS_APATH_DYNAMIC_STEREO,
                         DMUS_APATH_DYNAMIC_3D};
DWORD dwPChannelValues[] = {1, 16, 17, 999, 1000};
BOOL bActivateValues[] = {0, 1, 2, 0xFFFFFFFF};
DWORD dwStandardPath = 0;
DWORD dwPChannelValue = 0;
DWORD dwActivateValue = 0;


    //Load Default Media.
    hr = dmthLoadSegment(g_szDefaultMedia, &ptSegment8);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment function returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    for (dwStandardPath = 0; dwStandardPath<AMOUNT(dwStandardPaths); dwStandardPath++)
    {
        for (dwPChannelValue=0; dwPChannelValue<AMOUNT(dwPChannelValues); dwPChannelValue++)
        {
            for (dwActivateValue=0; dwActivateValue<AMOUNT(bActivateValues); dwActivateValue++)
            {
                //Create the audiopath.
                fnsLog(FYILOGLEVEL, "dwStandardPath = %s", dmthXlatDMUS_APATH(dwStandardPaths[dwStandardPath]));
                fnsLog(FYILOGLEVEL, "dwPChannels    = %d", dwPChannelValues[dwPChannelValue]);
                fnsLog(FYILOGLEVEL, "Activate       = %s", bActivateValues[dwActivateValue] ? "TRUE " : "FALSE");
                hr = ptPerf8->CreateStandardAudioPath(dwStandardPaths[dwStandardPath],
                                                      dwPChannelValues[dwPChannelValue],
                                                      bActivateValues[dwActivateValue],
                                                      &ptPath);
                if (S_OK != hr)
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath(%s, %d, %d) returned %s (%08Xh) instead of S_OK",
                                          dmthXlatDMUS_APATH(dwStandardPaths[dwStandardPath]),
                                          dwPChannelValues[dwPChannelValue],
                                          bActivateValues[dwActivateValue],
                                          tdmXlatHRESULT(hr),
                                          hr);
		            goto TEST_END;
	            }
                
                //If it's an active audiopath, download to and try to play the segment.
                if (bActivateValues[dwActivateValue])
                {

                    hr = ptSegment8->Download(ptPath); 
                    //hr = ptSegment8->Download(ptPerf8);  //BUGBUG
	                if(FAILED(hr))
	                {
                        fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		                goto TEST_END;
	                }

	                hr = ptPerf8->PlaySegmentEx( 
		                ptSegment8, 
		                0,
		                NULL,
                        0, 
		                //DMUS_SEGF_QUEUE,
		                0,
		                NULL,
		                NULL,
		                ptPath);

	                if(FAILED(hr))
	                {
                        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx function returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		                goto TEST_END;
	                }

                    fnsLog(FYILOGLEVEL, "Playing segment on active audiopath for 3 seconds.");
                    Sleep(3000);

                    //Unload - necessary to counteract all downloads!
                    hr = ptSegment8->Unload(ptPath);
	                if(FAILED(hr))
	                {
                        fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		                goto TEST_END;
	                }

                    //Stop the segment (is this necessary?)
	                hr = ptPerf8->StopEx(ptSegment8, 0, 0);
                    if(FAILED(hr))
	                {
                        fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		                goto TEST_END;
	                }

                }
                
                //Release the audiopath.  The segment should stop playing.
                SAFE_RELEASE(ptPath);
            }
        }
    }


    dwRes = FNS_PASS;


TEST_END:
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return dwRes;
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
DWORD Performance8_Play2AudioPaths(TESTPARAMS)
{
DWORD dwRes                                             = FNS_FAIL;
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath[2]          = {NULL};
CtIDirectMusicSegmentState         *ptSegmentState[2] = {NULL};
CtIDirectMusicSegment8              *ptSegment8[2]      = {NULL};
//CHAR *szFileName[2]               = {g_szDefaultMedia, "T:\\Media\\Sample\\lfive.sgt"};
CHAR *szFileName[2]               = {g_szDefaultMedia, "T:\\Media\\Sample\\lfive.sgt"};
BOOL bDownloaded[2] = {0};
DWORD i, dwIteration;


    for (dwIteration = 0; dwIteration < 2; dwIteration++)
    {

        //Create the audiopath.
        for (i=0; i<2; i++)
        {
            hr = ptPerf8->CreateStandardAudioPath(i ? DMUS_APATH_DYNAMIC_MONO : DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                                  i ? 32 :                      64,
                                                  TRUE,
                                                  &ptPath[i]);
            if (S_OK != hr)
            {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath(%d) returned %s (%08Xh) instead of S_OK",i, tdmXlatHRESULT(hr),hr);
	            goto TEST_END;
            }

            //Load a segment.
            dmthLoadSegment(szFileName[i], &ptSegment8[i]);
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName[i], tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }



            //Download our segment to both audiopaths or single audiopath (depending on test parameters)
            hr = ptSegment8[i]->Download(ptPath[i]); 
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }
            bDownloaded[i] = TRUE;

            //PlaySegment.
	        hr = ptPerf8->PlaySegmentEx(ptSegment8[i],
                                        0,
                                        0,
                                        i ? DMUS_SEGF_SECONDARY : 0,
                                        0,
                                        &ptSegmentState[i],
                                        0,
                                        ptPath[i]);

	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx(%s) function returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }
            fnsLog(FYILOGLEVEL, " Playing %s as %s segment for 4 seconds.", szFileName, i ? "secondary " : "primary ");
        }

        //Moment of silence.
        Sleep(4000);

        //Stop everything anyway.
        hr = ptPerf8->StopEx(0, 0, 0);
        if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }

        fnsLog(FYILOGLEVEL, "Called StopEx(NULL, NULL, NULL), all sound should stop");
        Sleep(1000);

        //Cleanup
        for (i=0; i<2; i++)
        {

            if (ptSegment8[i] && ptPath[i] && bDownloaded[i])
            {
                hr = ptSegment8[i]->Unload(ptPath[i]);
	            if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                bDownloaded[i] = FALSE;
            }
        }

        //Release all items that might have been allocated.
        for (i=0; i<2; i++)
        {
            SAFE_RELEASE(ptSegment8[i]);
            SAFE_RELEASE(ptPath[i]);
            SAFE_RELEASE(ptSegmentState[i]);
        }

    }//end for dwIteration.


    dwRes = FNS_PASS;

TEST_END:

    //Unload - necessary to counteract all downloads!
    for (i=0; i<2; i++)
    {

        if (ptSegment8[i] && ptPath[i] && bDownloaded[i])
        {
            hr = ptSegment8[i]->Unload(ptPath[i]);
	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }
        }
    }

    //Release all items that might have been allocated.
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment8[i]);
        SAFE_RELEASE(ptPath[i]);
        SAFE_RELEASE(ptSegmentState[i]);
    }

    return dwRes;

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
DWORD Performance8_CreateStandardAudioPath_Sleep1(TESTPARAMS)
{
DWORD dwRes                                             = FNS_FAIL;
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName               = g_szDefaultMedia;
BOOL bDownloaded = {0};
DWORD i = 0;


    //Create the audiopath.
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr),hr);
	    goto TEST_END;
    }


    //Load up media file.
    dmthLoadSegment(szFileName, &ptSegment8);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}


    hr = ptSegment8->Download(ptPath); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    bDownloaded = TRUE;


    //Play our segments.
    fnsLog(FYILOGLEVEL, "Playing Segment:");
	hr = ptPerf8->PlaySegmentEx(ptSegment8,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                0,
                                ptPath);

	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx(%s) function returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, " Playing %s as %s segment.", szFileName, i ? "secondary " : "primary ");


    //Now both segments are playing.
    fnsLog(FYILOGLEVEL, "Should hear music playing for 2 seconds.");
    Sleep(2000);


    hr = ptPerf8->StopEx(NULL, 0, 0);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL)");


    //Moment of silence.
    fnsLog(FYILOGLEVEL, "Waiting 4 seconds");
    Sleep(4000);


    dwRes = FNS_PASS;

TEST_END:

    //Unload - necessary to counteract all downloads!
    if (ptSegment8 && ptPath && bDownloaded)
    {
        hr = ptSegment8->Unload(ptPath);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
    }

    //Release all items that might have been allocated.
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return dwRes;

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
DWORD Performance8_CreateStandardAudioPath_Sleep2(TESTPARAMS)
{
DWORD dwRes                                             = FNS_FAIL;
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName               = g_szDefaultMedia;
BOOL bDownloaded = {0};
DWORD i = 0;


    //Create the audiopath.
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr),hr);
	    goto TEST_END;
    }


    //Load up media file.
    dmthLoadSegment(szFileName, &ptSegment8);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}


    hr = ptSegment8->Download(ptPath); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    bDownloaded = TRUE;


    //Play our segments.
    fnsLog(FYILOGLEVEL, "Playing Segment:");
	hr = ptPerf8->PlaySegmentEx(ptSegment8,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                0,
                                ptPath);

	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx(%s) function returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, " Playing %s as %s segment.", szFileName, i ? "secondary " : "primary ");


    //Now both segments are playing.
    fnsLog(FYILOGLEVEL, "Should hear music playing for 5 seconds.");
    Sleep(5000);


    hr = ptPerf8->StopEx(NULL, 0, 0);
    if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    fnsLog(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 4 seconds");


    dwRes = FNS_PASS;

TEST_END:

    //Unload - necessary to counteract all downloads!

    if (ptSegment8 && ptPath && bDownloaded)
    {
        hr = ptSegment8->Unload(ptPath);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Unload() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
    }

    //Release all items that might have been allocated.
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);

    return dwRes;

};




/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_CreateStandardAudioPath_Sleep(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_StopEx())");
    CALLDMTEST(Performance8_CreateStandardAudioPath_Sleep1(ptPerf8, 0,  0,0, 0));  //SCREWS UP MUSIC FOR NEXT TESTS.
    CALLDMTEST(Performance8_CreateStandardAudioPath_Sleep2(ptPerf8, 0,  0,0, 0));  

    return hr;
}
