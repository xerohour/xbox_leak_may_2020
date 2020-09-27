#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_StopEx_NoSegments(TESTPARAMS);
DWORD Performance8_StopEx_1Segment(TESTPARAMS);
DWORD Performance8_StopEx_2Segments(TESTPARAMS);
DWORD Performance8_StopEx_TimePast(TESTPARAMS);
DWORD Performance8_StopEx_TimeFuture(TESTPARAMS);

/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_StopEx (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_StopEx())");

    CALLDMTEST(Performance8_StopEx_NoSegments (ptPerf8, TRUE, TRUE, 0, 0)); // PASSES!
    CALLDMTEST(Performance8_StopEx_1Segment   (ptPerf8, TRUE, TRUE, 0, 0)); // PASSES!

    CALLDMTEST(Performance8_StopEx_2Segments(ptPerf8, TRUE,  TRUE, 0, 0));  //1 Path, 1 Segment  PASSES!
    CALLDMTEST(Performance8_StopEx_2Segments(ptPerf8, TRUE,  FALSE,0, 0));  //1 Path, 2 Segments FAILS due to 2987
    CALLDMTEST(Performance8_StopEx_2Segments(ptPerf8, FALSE, TRUE, 0, 0));  //2 Paths,1 Segment  FAILS due to 2987
    CALLDMTEST(Performance8_StopEx_2Segments(ptPerf8, FALSE, FALSE, 0, 0)); //2 Paths,2 Segments FAILS due to 2987
    
    CALLDMTEST(Performance8_StopEx_TimePast(ptPerf8,   0, 0, 0, 0));        //Passes
    CALLDMTEST(Performance8_StopEx_TimeFuture(ptPerf8, 0, 0, 0, 0));        //Fails due to 2992

    return hr;
};



/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_StopEx_NoSegments(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;

    //Nothing's playing - This call should be fine.
    hr = ptPerf8->StopEx(NULL, NULL, NULL);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx() returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr),hr);
	    goto TEST_END;
    }

    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};




/********************************************************************************
A simpler test for a simpler time.  Like Feb release.  This is all that needs
to work for that.
********************************************************************************/
DWORD Performance8_StopEx_1Segment(TESTPARAMS)
{
DWORD dwRes                                             = FNS_FAIL;
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegmentState         *ptSegmentState = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName                    = g_szDefaultMedia;
BOOL bDownloaded                    = FALSE;
DWORD dwScenario                    = 0;

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


    //Load up the media file.
    dmthLoadSegment(szFileName, &ptSegment8);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    //Download everything.
    hr = ptSegment8->Download(ptPath); 
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    bDownloaded = TRUE;

    for (dwScenario = 0; dwScenario<4; dwScenario++)
    {    
    

        //Play our segments.
	    hr = ptPerf8->PlaySegmentEx(ptSegment8,0,0,0,0,&ptSegmentState,0,ptPath);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx(%s) function returned %s (%08Xh)",szFileName, tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }


        //Now both segments are playing.
       fnsLog(FYILOGLEVEL, " Playing %s as segment.", szFileName);
       fnsLog(FYILOGLEVEL, "Should hear music playing for 4 seconds.");
       Sleep(4000);



        switch (dwScenario)
        {
            //StopEx(global)
            case 0:	            
                hr = ptPerf8->StopEx(NULL, 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(segment)
            case 1:	            
                hr = ptPerf8->StopEx(ptSegment8, 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptSegment8, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptSegment8, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(SegmentState)
            case 2:	            
                hr = ptPerf8->StopEx(ptSegmentState, 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptSegmentState, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptSegmentState, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(AudioPath)
            case 3:	            
                hr = ptPerf8->StopEx(ptPath, 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptPath, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptPath, NULL, NULL), all sound should stop for 2 seconds");
                break;
            
            default:
                fnsLog(ABORTLOGLEVEL, "**** ABORT: TEST ERROR!!!!!!!");
		        goto TEST_END;
                break;                
        };

        //Moment of silence.
        Sleep(2000);

        //Stop everything anyway.
        hr = ptPerf8->StopEx(0, 0, 0);
        if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }

        fnsLog(FYILOGLEVEL, "Called StopEx(NULL, NULL, NULL), all sound should stop");
        Sleep(1000);


        //Release the segstates we aquired.
        SAFE_RELEASE(ptSegmentState);

    }

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
    SAFE_RELEASE(ptSegmentState);

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
DWORD Performance8_StopEx_2Segments(TESTPARAMS)
{
DWORD dwRes                                             = FNS_FAIL;
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath[2]          = {NULL};
CtIDirectMusicSegmentState         *ptSegmentState[2] = {NULL};
CtIDirectMusicSegment8              *ptSegment8[2]      = {NULL};
CHAR *szFileName[2]               = {g_szDefaultMedia, "T:\\Media\\Sample\\lfive.sgt"};
BOOL bDownloaded[2] = {0};
BOOL bSinglePath    = dwParam1;
BOOL bSingleSegment = dwParam2;
DWORD dwSegment = 0;
DWORD dwPath = 0;
DWORD i = 0;
DWORD dwScenario = 0; 


    fnsLog(FYILOGLEVEL, "");
    fnsLog(FYILOGLEVEL, "================= STOPEX TEST: %d AudioPath(s), %d Segment(s)", bSinglePath ? 1 : 2, bSingleSegment ? 1 : 2);
    fnsLog(FYILOGLEVEL, "");

    //Create the audiopath.
    hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath[0]);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr),hr);
	    goto TEST_END;
    }

    //Create another different audiopath if required.
    if (!bSinglePath)
    {
        hr = ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_MONO,
                                              32,
                                              TRUE,
                                              &ptPath[1]);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: CreateStandardAudioPath returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr),hr);
	        goto TEST_END;
        }
    }

    //Load up 1 or 2 segments.
    for (i=0; i<2; i++)
    {
        if (bSingleSegment && i==1)
            break;

        //Load up media file.
        dmthLoadSegment(szFileName[i], &ptSegment8[i]);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: dmthLoadSegment(%s) returned %s (%08Xh)",szFileName[i], tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
    }


    //Download our stuff to audiopaths or single audiopath (depending on test parameters)
    for (i=0; i<2; i++)
    {
        //Bail if we don't have 2 segments.
        if (bSingleSegment && i==1)
            break;

        //Download both segments to path one if there's only one path, otherwise download each seg to respective path.
        dwPath = i;
        if (bSinglePath)
            dwPath = 0;


        hr = ptSegment8[i]->Download(ptPath[dwPath]); 
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: Download() returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
        bDownloaded[i] = TRUE;
    }

    for (dwScenario = 0; dwScenario<4; dwScenario++)
    {    
    

        //Play our segments.
        fnsLog(FYILOGLEVEL, "Playing Segments:");
        for (i=0; i<2; i++)
        {

            dwPath = i;
            if (bSinglePath)
                dwPath = 0;

            dwSegment = i;
            if (bSingleSegment)
                dwSegment = 0;


	        hr = ptPerf8->PlaySegmentEx(ptSegment8[dwSegment],
                                        0,
                                        0,
                                        i ? DMUS_SEGF_SECONDARY : 0,
                                        0,
                                        &ptSegmentState[i],
                                        0,
                                        ptPath[dwPath]);

	        if(FAILED(hr))
	        {
                fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx(%s) function returned %s (%08Xh)",szFileName[i], tdmXlatHRESULT(hr), hr);
		        goto TEST_END;
	        }
            fnsLog(FYILOGLEVEL, " Playing %s as %s segment.", szFileName[i], i ? "secondary " : "primary ");
        }


        //Now both segments are playing.
        fnsLog(FYILOGLEVEL, "Should hear music playing for 5 seconds.");
        Sleep(5000);


        switch (dwScenario)
        {
            //StopEx(global)
            case 0:	            
                hr = ptPerf8->StopEx(NULL, 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(NULL, NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 4 seconds");
                break;

            //StopEx(segment)
            case 1:	            
                hr = ptPerf8->StopEx(ptSegment8[0], 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptSegment8[0], NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptSegment8[0], NULL, NULL), %sSegment should Stop for 4 seconds.", bSingleSegment ? "" : "first ");
                break;

            //StopEx(SegmentState)
            case 2:	            
                hr = ptPerf8->StopEx(ptSegmentState[0], 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptSegmentState[0], NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptSegmentState[0], NULL, NULL), SegmentState[0] should Stop for 4 seconds.");
                break;

            //StopEx(AudioPath)
            case 3:	            
                hr = ptPerf8->StopEx(ptPath[0], 0, 0);
                if(FAILED(hr))
	            {
                    fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx(ptPath[0], NULL, NULL) returned %s (%08Xh)",tdmXlatHRESULT(hr), hr);
		            goto TEST_END;
	            }
                fnsLog(FYILOGLEVEL, "Calling StopEx(ptPath[0], NULL, NULL),%s should Stop for 4 seconds.", bSinglePath ? "All Music" : "Segment on Path 1");
                break;
            
            default:
                fnsLog(ABORTLOGLEVEL, "**** ABORT: TEST ERROR!!!!!!!");
		        goto TEST_END;
                break;                
        };

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

        //Release the segstates we aquired.
        for (i=0; i<2; i++)
        {
            SAFE_RELEASE(ptSegmentState[i]);
        }

    }

    dwRes = FNS_PASS;

TEST_END:

    //Unload - necessary to counteract all downloads!
    for (i=0; i<2; i++)
    {

        dwPath = i;
        if (bSinglePath)
            dwPath = 0;

        if (ptSegment8[i] && ptPath[dwPath] && bDownloaded[i])
        {
            hr = ptSegment8[i]->Unload(ptPath[dwPath]);
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
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_StopEx_TimeFuture(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic    Music;
    CHAR *szSegment = "T:\\Media\\Perf8\\StopEx\\16Notes.sgt";
    REFERENCE_TIME rt;
    MUSIC_TIME mt;
    DWORD i;


    for (i=0; i<2; i++)
    {

        //Load up and play the media.
        hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.Init %s failed", szSegment);
	        goto TEST_END;
        }

        //Get the time.
        hr = ptPerf8->GetTime(&rt, &mt);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "GetTime failed");
	        goto TEST_END;
        }

        if (i==0)
        {
            fnsLog(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", DWORD(rt >> 32), DWORD(rt & 0xFFFFFFFF));
            rt += 50000000;
            //Run our test.
            hr = ptPerf8->StopEx(0,rt,DMUS_SEGF_REFTIME);
            fnsLog(FYILOGLEVEL, "Stopping in 5 seconds, or at %08X%08X.", DWORD(rt >> 32), DWORD(rt & 0xFFFFFFFF));
            fnsLog(FYILOGLEVEL, "You should hear music stop in 5 seconds.");
        }
        else
        {
            fnsLog(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
            mt += DMUS_PPQ * 8;
            hr = ptPerf8->StopEx(0,mt,0);
            fnsLog(FYILOGLEVEL, "Stopping in 8 quarter-notes, or at mt = %08X.", mt);
            fnsLog(FYILOGLEVEL, "You should hear music stop in 8 quarter notes.");


        }
        //Check the result of StopEx.
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "StopEx failed");
	        goto TEST_END;
        }


        //Wait 6 seconds.
        Sleep(6000);

        fnsLog(FYILOGLEVEL, "Music should be stopped now.  Waiting 5 more second.");
        Sleep(5000);


        hr = Music.Shutdown();
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.ShutDown failed");
	        goto TEST_END;
        }

    }

    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};



/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
DWORD Performance8_StopEx_TimePast(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic    Music;
    CHAR *szSegment = "T:\\Media\\Perf8\\StopEx\\16Notes.sgt";
    REFERENCE_TIME rt;
    MUSIC_TIME mt;
    DWORD i;


    for (i=0; i<2; i++)
    {

        //Load up and play the media.
        hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.Init %s failed", szSegment);
	        goto TEST_END;
        }

        fnsLog(FYILOGLEVEL, "Playing for 2 seconds.");
        Sleep(2000);

        //Get the time.
        hr = ptPerf8->GetTime(&rt, &mt);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "GetTime failed");
	        goto TEST_END;
        }

        if (i==0)
        {
            fnsLog(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", DWORD(rt >> 32), DWORD(rt & 0xFFFFFFFF));
            rt -= 50000000;
            hr = ptPerf8->StopEx(0,rt, DMUS_SEGF_REFTIME);
            fnsLog(FYILOGLEVEL, "Stopping 5 seconds ago, or at %08X%08X.", DWORD(rt >> 32), DWORD(rt & 0xFFFFFFFF));
            fnsLog(FYILOGLEVEL, "You should hear music stop immediately.");
        }
        else
        {
            fnsLog(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
            mt -= DMUS_PPQ * 8;
            hr = ptPerf8->StopEx(0,mt, 0);
            fnsLog(FYILOGLEVEL, "Stopping 8 quarter-notes ago, or at mt = %08X.", mt);
            fnsLog(FYILOGLEVEL, "You should hear music stop immediately");


        }

        //Check the results of StopEx.
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "StopEx failed");
	        goto TEST_END;
        }

        Sleep(4000);


        hr = Music.Shutdown();
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.ShutDown failed");
	        goto TEST_END;
        }

    }

    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};



//TODO:!!!!  Post-feb release.
/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
/* 
DWORD Performance8_StopEx_BoundaryFlags(TESTPARAMS)
{
    DWORD dwRes                                             = FNS_FAIL;
    HRESULT hr                                              = S_OK;
    CICMusic    Music;
    CHAR *szSegment = "T:\\Media\\Perf8\\StopEx\\Boundary.sgt';
    REFERENCE_TIME rt;
    MUSIC_TIME mt;


    for (i=0; i<2; i++)
    {

        //Load up and play the media.
        hr = Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.Init %s failed", g_wszSegment);
	        goto TEST_END;
        }

        fnsLog(FYILOGLEVEL, "Playing for a second.");
        Sleep(1000);

        //Get the time.
        hr = ptPerf8->GetTime(&rt, &mt);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "GetTime failed");
	        goto TEST_END;
        }

        if (i==0)
        {
            fnsLog(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", rt >> 32, rt & 0xFFFFFFFF);
            rt -= 50000000;
            fnsLog(FYILOGLEVEL, "Stopping 5 seconds ago, or at %08X%08X.", rt >> 32, rt & 0xFFFFFFFF);
            fnsLog(FYILOGLEVEL, "You should hear music stop immediately.");
        }
        else
        {
            fnsLog(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
            mt -= DMUS_PPQ * 8;
            fnsLog(FYILOGLEVEL, "Stopping 8 quarter-notes ago, or at mt = %08X.", mt);
            fnsLog(FYILOGLEVEL, "You should hear music stop immediately");


        }

        //Run our test.
        hr = ptPerf8->StopEx(0,
                             i==0 ? rt : mt,
                             i==0 ? DMUS_SEGF_REFTIME : 0);
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "StopEx failed");
	        goto TEST_END;
        }

        fnsLog(FYILOGLEVEL, "Music should be stopped.  Waiting 1 second.");
        Sleep(1000);


        hr = Music.Shutdown();
        if (S_OK != hr)
        {
            fnsLog(ABORTLOGLEVEL, "Music.ShutDown failed");
	        goto TEST_END;
        }

    }

    dwRes = FNS_PASS;

TEST_END:
    return  dwRes;
};
*/