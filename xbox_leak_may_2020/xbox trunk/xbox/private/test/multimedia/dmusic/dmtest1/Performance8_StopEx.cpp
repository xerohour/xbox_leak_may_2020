#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_StopEx_1Segment(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwUnused2);


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_StopEx_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_StopEx_BVT())");
    DMTEST_EXECUTE(Performance8_StopEx_1Segment(ptPerf8, 0, 0));                    
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_StopEx_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_StopEx_Valid())");


    return hr;
};



/********************************************************************************
Call StopEx when nothing is playing.
********************************************************************************/
HRESULT Performance8_StopEx_NoSegments(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    CHECKRUN(ptPerf8->StopEx(NULL, __int64(0), NULL));
    return hr;

};





/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_1Segment(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwUnused2)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = {NULL};
CtIDirectMusicSegmentState         *ptSegmentState = {NULL};
CtIDirectMusicSegment8              *ptSegment8      = {NULL};
CHAR *szFileName                    = NULL;
DWORD dwScenario                    = 0;

//Load media.
MEDIAFILEDESC MediaFileDesc[]=
{
{"SGT/test.sgt",              "T:\\DMTest1",          COPY_IF_NEWER},
{"DLS/Main1.DLS",             "T:\\DMTest1",          COPY_IF_NEWER},
{"WAV/Streaming10s.wav",      "T:\\WAV",    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));



if (!bWave)
{
    szFileName = "T:\\DMTest1\\test.sgt";
}
else
{
    szFileName = "T:\\WAV\\Streaming10s.wav";
}



    //Create the audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath));

    //Load up the media file.
    CHECKRUN(dmthLoadSegment(szFileName, &ptSegment8));

    for (dwScenario = 0; dwScenario<4; dwScenario++)
    {    
        //Play our segments.
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8,0,0,0,__int64(0),&ptSegmentState,0,ptPath));

        //Now both segments are playing.
       Log(FYILOGLEVEL, " Playing %s as segment.", szFileName);
       Log(FYILOGLEVEL, "Should hear music playing for 4 seconds.");
       Wait(4000);

        switch (dwScenario)
        {
            //StopEx(global)
            case 0:             
                CHECKRUN(ptPerf8->StopEx(NULL, __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(segment)
            case 1:             
                CHECKRUN(ptPerf8->StopEx(ptSegment8, __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptSegment8, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(SegmentState)
            case 2:             
                CHECKRUN(ptPerf8->StopEx(ptSegmentState, __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptSegmentState, NULL, NULL), all sound should stop for 2 seconds");
                break;

            //StopEx(AudioPath)
            case 3:             
                CHECKRUN(ptPerf8->StopEx(ptPath, __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptPath, NULL, NULL), all sound should stop for 2 seconds");
                break;
            
            default:
                Log(ABORTLOGLEVEL, "**** ABORT: TEST ERROR!!!!!!!");
                hr = E_FAIL;
                break;                
        };

        //Moment of silence.
        Wait(2000);

        //Stop everything anyway.
        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        Log(FYILOGLEVEL, "Called StopEx(NULL, NULL, NULL), all sound should stop");
        Wait(1000);

        //Release the segstates we aquired.
        SAFE_RELEASE(ptSegmentState);

        if (FAILED(hr))
            break;

    }


    //Release all items that might have been allocated.
    SAFE_RELEASE(ptSegment8);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegmentState);
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_1Segment_DLS(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_StopEx_1Segment(ptPerf8, FALSE, 0);
}

/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_1Segment_Wave(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_StopEx_1Segment(ptPerf8, TRUE, 0);
}




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
HRESULT Performance8_StopEx_2Segments(CtIDirectMusicPerformance8* ptPerf8, DWORD bSinglePath, DWORD bSingleSegment, BOOL bWave)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath[2]          = {NULL};
CtIDirectMusicSegmentState          *ptSegmentState[2]  = {NULL};
CtIDirectMusicSegment8              *ptSegment8[2]      = {NULL};
CHAR *szFileName[2]               = {0};
DWORD dwSegment = 0;
DWORD dwPath = 0;
DWORD i = 0;
DWORD dwScenario = 0; 

//Load media.
MEDIAFILEDESC MediaFileDesc[]=
{
{"SGT/test.sgt",              "T:\\DMTest1",          COPY_IF_NEWER},
{"SGT/test2.sgt",             "T:\\DMTest1",          COPY_IF_NEWER},
{"DLS/Main1.DLS",             "T:\\DMTest1",          COPY_IF_NEWER},
{"WAV/Streaming10s.wav",      "T:\\WAV",    COPY_IF_NEWER},
{"WAV/GuitarStream1.wav",     "T:\\WAV",    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));




if (!bWave)
{
    szFileName[0] = "T:\\DMTest1\\Test.sgt";
    szFileName[1] = "T:\\DMTest1\\Test2.sgt";
}
else
{
    szFileName[0] = "T:\\WAV\\Streaming10s.wav";
    szFileName[1] = "T:\\WAV\\GuitarStream1.wav";
}


    Log(FYILOGLEVEL, "");
    Log(FYILOGLEVEL, "================= STOPEX TEST: %d AudioPath(s), %d Segment(s)", bSinglePath ? 1 : 2, bSingleSegment ? 1 : 2);
    Log(FYILOGLEVEL, "");

    //Create the audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath[0]));

    //Create another different audiopath if required.
    if (!bSinglePath)
    {
        CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                              32,
                                              TRUE,
                                              &ptPath[1]));

    }

    //Load up 1 or 2 segments.
    for (i=0; i<2 && SUCCEEDED(hr); i++)
    {
        if (bSingleSegment && i==1)
            break;

        //Load up media file.
        CHECKRUN(dmthLoadSegment(szFileName[i], &ptSegment8[i]));
    }


    //Download our stuff to audiopaths or single audiopath (depending on test parameters)
    for (i=0; i<2 && SUCCEEDED(hr); i++)
    {
        //Bail if we don't have 2 segments.
        if (bSingleSegment && i==1)
            break;

        //Download both segments to path one if there's only one path, otherwise download each seg to respective path.
        dwPath = i;
        if (bSinglePath)
            dwPath = 0;

    }

    for (dwScenario = 0; dwScenario<4 && SUCCEEDED(hr); dwScenario++)
    {    
        //Play our segments.
        Log(FYILOGLEVEL, "Playing Segments:");
        for (i=0; i<2 && SUCCEEDED(hr); i++)
        {

            dwPath = i;
            if (bSinglePath)
                dwPath = 0;

            dwSegment = i;
            if (bSingleSegment)
                dwSegment = 0;

            CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment8[dwSegment],
                                        0,
                                        0,
                                        i ? DMUS_SEGF_SECONDARY : 0,
                                        __int64(0),
                                        &ptSegmentState[i],
                                        0,
                                        ptPath[dwPath]));

            Log(FYILOGLEVEL, " Playing %s as %s segment.", szFileName[dwSegment], i ? "secondary " : "primary ");
        }

        //Now both segments are playing.
        Log(FYILOGLEVEL, "Should hear music playing for 5 seconds.");
        Wait(5000);


        switch (dwScenario)
        {
            //StopEx(global)
            case 0:             
                CHECKRUN(ptPerf8->StopEx(NULL, __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(NULL, NULL, NULL), all sound should stop for 4 seconds");
                break;

            //StopEx(segment)
            case 1:             
                CHECKRUN(ptPerf8->StopEx(ptSegment8[0], __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptSegment8[0], NULL, NULL), %sSegment should Stop for 4 seconds.", bSingleSegment ? "" : "first ");
                break;

            //StopEx(SegmentState)
            case 2:             
                CHECKRUN(ptPerf8->StopEx(ptSegmentState[0], __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptSegmentState[0], NULL, NULL), SegmentState[0] should Stop for 4 seconds.");
                break;

            //StopEx(AudioPath)
            case 3:             
                CHECKRUN(ptPerf8->StopEx(ptPath[0], __int64(0), 0));
                Log(FYILOGLEVEL, "Calling StopEx(ptPath[0], NULL, NULL),%s should Stop for 4 seconds.", bSinglePath ? "All Music" : "Segment on Path 1");
                break;
            
            default:
                Log(ABORTLOGLEVEL, "**** ABORT: TEST ERROR!!!!!!!");
                hr = E_FAIL;
                break;                
        };

        //Moment of silence.
        Wait(4000);

        //Stop everything anyway.
        CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
        Log(FYILOGLEVEL, "Called StopEx(NULL, NULL, NULL), all sound should stop");
        Wait(1000);

        //Release the segstates we aquired.
        for (i=0; i<2; i++)
        {
            SAFE_RELEASE(ptSegmentState[i]);
        }

    }

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
********************************************************************************/
HRESULT Performance8_StopEx_2Segments_DLS(CtIDirectMusicPerformance8* ptPerf8, DWORD bSinglePath, DWORD bSingleSegment)
{
    return Performance8_StopEx_2Segments(ptPerf8, bSinglePath, bSingleSegment, FALSE);
}


/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_2Segments_Wave(CtIDirectMusicPerformance8* ptPerf8, DWORD bSinglePath, DWORD bSingleSegment)
{
    return Performance8_StopEx_2Segments(ptPerf8, bSinglePath, bSingleSegment, TRUE);
}




/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_TimeFuture(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    CICMusic    Music;
    CHAR *szSegment = "T:\\DMTest1\\Perf8\\StopEx\\16Notes.sgt";
    REFERENCE_TIME rt = 0;
    MUSIC_TIME mt = 0;

    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"SGT/16Notes.sgt",           "T:\\DMTest1\\Perf8\\StopEx\\",          COPY_IF_NEWER},
    {"DLS/Main1.DLS",             "T:\\DMTest1\\Perf8\\StopEx\\",          COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //Load up and play the media.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    //Get the time.
    CHECKRUN(ptPerf8->GetTime(&rt, &mt));

    Log(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
    mt += DMUS_PPQ * 8;
    hr = ptPerf8->StopEx(0,__int64(mt),0);
    Log(FYILOGLEVEL, "Stopping in 8 quarter-notes, or at mt = %08X.", mt);
    Log(FYILOGLEVEL, "You should hear music stop in 8 quarter notes.");

    //Check the result of StopEx.
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL, "StopEx failed");
        goto TEST_END;
    }

    //Wait 6 seconds.
    Wait(6000);

    Log(FYILOGLEVEL, "Music should be stopped now.  Waiting 5 more second.");
    Wait(5000);


    CHECKRUN(Music.Shutdown());


TEST_END:
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_StopEx_TimePast(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr                                              = S_OK;
    CICMusic Music;
    CHAR *szSegment = "T:\\DMTest1\\Perf8\\StopEx\\16Notes.sgt";
    REFERENCE_TIME rt = __int64(0);
    MUSIC_TIME mt = 0;

    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"SGT/16Notes.sgt",           "T:\\DMTest1\\Perf8\\StopEx\\",          COPY_IF_NEWER},
    {"DLS/Main1.DLS",             "T:\\DMTest1\\Perf8\\StopEx\\",          COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));

    //Load up and play the media.
    CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    Log(FYILOGLEVEL, "Playing for 2 seconds.");
    Wait(2000);

    //Get the time.
    CHECKRUN(ptPerf8->GetTime(&rt, &mt));
    Log(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
    mt -= DMUS_PPQ * 8;
    hr = ptPerf8->StopEx(0, __int64(mt), 0);
    Log(FYILOGLEVEL, "Stopping 8 quarter-notes ago, or at mt = %08X.", mt);
    Log(FYILOGLEVEL, "You should hear music stop immediately");

    //Check the results of StopEx.
    if (S_OK != hr)
    {
        Log(ABORTLOGLEVEL, "StopEx failed");
        goto TEST_END;
    }

    Wait(4000);
    CHECKRUN(Music.Shutdown());


TEST_END:
    return  hr;
};


/********************************************************************************
Call this test while DoWork ISN'T being called.
********************************************************************************/
HRESULT Performance8_StopEx_NoDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    CICMusic    Music;
    CHAR *szSegmentServer = "WAV/GuitarLoop.wav";
    DWORD i;
    DWORD dwStartTime = 0;
    CtIDirectMusicSegment *ptSegment = NULL;


    //Load teh media.
    CHECKRUN(MediaCopyFile(szSegmentServer));

    //Call DoWork
    DirectMusicDoWork(100);
    
    //Load up and play the media.
    CHECKRUN(Music.Init(ptPerf8, MediaServerToLocal(szSegmentServer), DMUS_APATH_SHARED_STEREOPLUSREVERB));

    //Wait a second.
    Log(FYILOGLEVEL, "Will stop sound in 1 second...");
    dwStartTime= timeGetTime();
    do
    {
        DirectMusicDoWork(50);            
        Sleep(50);
    }
    while (timeGetTime() - dwStartTime < 1000);

    //Call StopEx, but don't call DoWork.
    Log(FYILOGLEVEL, "Sound should stop NOW!!!");
    CHECK(ptPerf8->StopEx(ptSegment, 0, 0));
    Wait(2000);

    //After 2 seconds, call DoWork()
    Log(FYILOGLEVEL, "Calling DirectMusicDoWork()");
    CHECK(DirectMusicDoWork(2000));

    //Wait 2 seconds.
    Wait(2000);

    CHECKRUN(Music.Shutdown());

    RELEASE(ptSegment);
    return hr;
};




//TODO:!!!!
/********************************************************************************
********************************************************************************/
/* 
HRESULT Performance8_StopEx_BoundaryFlags(TESTPARAMS)
{
    HRESULT hr                                              = S_OK;
    CICMusic    Music;
    CHAR *szSegment = "T:\\DMTest1\\Perf8\\StopEx\\Boundary.sgt';
    REFERENCE_TIME rt;
    MUSIC_TIME mt;


    for (i=0; i<2 && SUCCEEDED(hr); i++)
    {

        //Load up and play the media.
        CHECKRUN(Music.Init(ptPerf8, szSegment, DMUS_APATH_SHARED_STEREOPLUSREVERB));
        Log(FYILOGLEVEL, "Playing for a second.");
        Wait(1000);

        //Get the time.
        CHECKRUN(ptPerf8->GetTime(&rt, &mt));
        if (i==0)
        {
            Log(FYILOGLEVEL, "Current REFERENCE_TIME is %08X%08X", rt >> 32, rt & 0xFFFFFFFF);
            rt -= 50000000;
            Log(FYILOGLEVEL, "Stopping 5 seconds ago, or at %08X%08X.", rt >> 32, rt & 0xFFFFFFFF);
            Log(FYILOGLEVEL, "You should hear music stop immediately.");
        }
        else
        {
            Log(FYILOGLEVEL, "Current MUSIC_TIME is %08X", mt);
            mt -= DMUS_PPQ * 8;
            Log(FYILOGLEVEL, "Stopping 8 quarter-notes ago, or at mt = %08X.", mt);
            Log(FYILOGLEVEL, "You should hear music stop immediately");


        }

        //Run our test.
        CHECKRUN(ptPerf8->StopEx(0,
                             i==0 ? rt : mt,
                             i==0 ? DMUS_SEGF_REFTIME : 0))
        Log(FYILOGLEVEL, "Music should be stopped.  Waiting 1 second.");
        Wait(1000);


        CHECKRUN(Music.Shutdown());
    }


TEST_END:
    return  hr
};
*/