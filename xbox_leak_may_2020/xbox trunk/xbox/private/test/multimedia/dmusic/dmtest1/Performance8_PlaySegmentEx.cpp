#include "globals.h"
#include "cicmusicX.h"

HRESULT Performance8_PlaySegment_BVT1(CtIDirectMusicPerformance8 *ptPerf8);
HRESULT Performance8_PlaySegment_StartTime(CtIDirectMusicPerformance8 *ptPerf8, BOOL bTransition, MUSIC_TIME mtRel, REFERENCE_TIME rtRel);


LPSTR szWaveFiles[] = { 
	"WAV/48_16_2.wav",  // 48000, 16, 2												
	"WAV/48_16_1.wav",  // 48000, 16, 1												
	"WAV/48_08_2.wav",  // 48000,  8, 2
	"WAV/48_08_1.wav",  // 48000,  8, 1 
	"WAV/44_16_2.wav",  // 44100, 16, 2 
	"WAV/44_16_1.wav",  // 44100, 16, 1
	"WAV/44_08_2.wav",  // 44100,  8, 2
	"WAV/44_08_1.wav",  // 44100,  8, 1
	"WAV/32_16_2.wav",  // 32000, 16, 2
	"WAV/32_16_1.wav",  // 32000, 16, 1
	"WAV/32_08_2.wav",  // 32000,  8, 2
	"WAV/32_08_1.wav",  // 32000,  8, 1
	"WAV/22_16_2.wav",  // 22050, 16, 2
	"WAV/22_16_1.wav",  // 22050, 16, 1
	"WAV/22_08_2.wav",  // 22050,  8, 2
	"WAV/22_08_1.wav",  // 22050,  8, 1
	"WAV/16_16_2.wav",  // 16000, 16, 2
	"WAV/16_16_2.wav",  // 16000, 16, 1
	"WAV/16_08_2.wav",  // 16000,  8, 2
	"WAV/16_08_1.wav",  // 16000,  8, 1
	"WAV/11_16_2.wav",  // 11025, 16, 2
	"WAV/11_16_1.wav",  // 11025, 16, 1
	"WAV/11_08_2.wav",  // 11025,  8, 2
	"WAV/11_08_1.wav",  // 11025,  8, 1
	"WAV/08_16_2.wav",  //  8000, 16, 2
	"WAV/08_16_1.wav",  //  8000, 16, 1
	"WAV/08_08_2.wav",  //  8000,  8, 2
	"WAV/08_08_1.wav"   //  8000,  8, 1
};


LPSTR szADPCMWaves[] = 
{
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_LP_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_LP_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_LP_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_OS_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_OS_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_OS_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_ST_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_ST_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegDLS_ADPCM_ST_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_11_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_22_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_LP_44_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_11_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_22_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_OS_44_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_11_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_11_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_22_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_22_2.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_44_1.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/ADPCM/SegWAV_ADPCM_ST_44_2.sgt"
};


LPSTR szWaveSegments[] = 
{
"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegEmbedWave.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveLoopForever.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveShort.sgt",
"DMusic/DMTest1/Perf8/PlaySeg/PerfMem/SegWaveStreaming10s.sgt"
};


/*
LPSTR szWaves[] = 
{
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\HitTile.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\Rocket1.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\Rocket2.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\Torpedo.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\HitWater.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\HitWorld.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\Explosion1.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\Explosion2.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\MiniCannon.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\PassBy_Rocket.sgt",
"T:\\DMTest1\\Perf8\\PlaySeg\\Scott02\\PassBy_Torpedo.sgt"
};
*/






/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_PlaySegmentEx_BVT())");
    

    //Play MIDI segment.
    DMTEST_EXECUTE(Performance8_PlaySegment_BVT1(ptPerf8));

    return hr;
};

/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegment_BVT1(CtIDirectMusicPerformance8 *ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegment *ptSegment = NULL;

//Play some music whose tempo we're gonna change.
CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));
CHECKRUN(Music.GetInterface(&ptSegment));
CHECKRUN(ptSegment->SetRepeats(0));
CHECKRUN(WaitForSegmentStop(ptPerf8, ptSegment, NULL, 15000));
SAFE_RELEASE(ptSegment);
return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegment_StartTime_Start(CtIDirectMusicPerformance8 *ptPerf8, DWORD mtRel, DWORD rtRel)
{
    return Performance8_PlaySegment_StartTime(ptPerf8, FALSE, mtRel, rtRel);
}

/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegment_StartTime_Transition(CtIDirectMusicPerformance8 *ptPerf8, DWORD mtRel, DWORD rtRel)
{
    return Performance8_PlaySegment_StartTime(ptPerf8, TRUE, mtRel, rtRel);
}


/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegment_StartTime(CtIDirectMusicPerformance8 *ptPerf8, BOOL bTransition, MUSIC_TIME mtRel, REFERENCE_TIME rtRel)
{
HRESULT hr = S_OK;
CHAR *szSegmentName[2] = {"T:\\DMTest1\\test.sgt", "T:\\DMTest1\\test2.sgt"};
CtIDirectMusicSegment *ptSegment[2] = {NULL};
CtIDirectMusicAudioPath *ptPath = NULL;
MUSIC_TIME mt=0;
REFERENCE_TIME rt=__int64(0);
DWORD dwTempo = 100;
LONG lWait = 0;
int i = 0;

//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
CHECKRUN(LoadMedia(MediaFileDesc));


    //Load segs.
    for (i=0; i<2; i++)
    {
        CHECKRUN(dmthLoadSegment(szSegmentName[i], &ptSegment[i]));
    }
    
    //Create AP for them to play on.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 16, TRUE, &ptPath));

    //If we're gonna transition, then start the original segment playing.
    if (bTransition)
    {	    
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0],0,0,0,0,NULL,0,ptPath));
        CHECKRUN(Log(FYILOGLEVEL, "Playing %s, will transition in 4 seconds.", szSegmentName[0]));
        CHECKRUN(CountDown(4000, 200));
    }
    else
    {
        CHECKRUN(Log(FYILOGLEVEL, "No sound for 4 seconds..."));
        CHECKRUN(CountDown(4000, 500));
    }

    //Wait exactly that long.
    if (mtRel)
    {
        lWait = 1000 * mtRel *  60 / dwTempo  / 768;
    }
    else
    {
        lWait = LONG(rtRel / 10000);
    }

    if (lWait < 0)
        lWait = 0;


    //Queue segment to play relative.
    CHECKRUN(ptPerf8->GetTime(&rt, &mt));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[1], 0, 0, mtRel?0:DMUS_SEGF_REFTIME, mtRel?mtRel+mt:rtRel+rt, NULL,0,ptPath));
    CHECKRUN(Log(FYILOGLEVEL, "%s is queued to play with in %s = %I64d units (%d ms)", szSegmentName[1], mtRel ? "MUSIC_TIME" : "REFERENCE_TIME", REFERENCE_TIME(mtRel ? mtRel+mt : rtRel+rt), lWait));
    CHECKRUN(CountDown(lWait, lWait / 10)); 
    CHECKRUN(Log(FYILOGLEVEL, "%s should be playing now.  Waiting 4 more seconds.", szSegmentName[1]));
    CHECKRUN(CountDown(4000, 500));
    
    
    
    ptPerf8->StopEx(0, __int64(0), 0); 
    //Unload and free the segments.
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
    }
        
    SAFE_RELEASE(ptPath);


return hr;
};

 

/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegment_TransitionType(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwFlags, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CHAR *szSegmentName[2] = {"T:\\DMTest1\\test.sgt", "T:\\DMTest1\\test2.sgt"};
CtIDirectMusicSegment *ptSegment[2] = {NULL};
CtIDirectMusicAudioPath *ptPath = NULL;
LONG lWait = 0;
int i = 0;

//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
CHECKRUN(LoadMedia(MediaFileDesc));


    //Load segs.
    for (i=0; i<2; i++)
    {
        CHECKRUN(dmthLoadSegment(szSegmentName[i], &ptSegment[i]));
    }
    
    //Create AP for them to play on.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 16, TRUE, &ptPath));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0],0,0,0,0,NULL,0,ptPath));
    CHECKRUN(Log(FYILOGLEVEL, "Playing %s.", szSegmentName[0]));
    CHECKRUN(Log(FYILOGLEVEL, "In 4s, %s will be queued to play with dwFlags = %s.", szSegmentName[1], Log_DMUS_SEGF(dwFlags)));
    CHECKRUN(CountDown(4000, 1000));    
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[1],0,0,dwFlags,0,NULL,0,ptPath));
    
    //How long we wait depends on what we're doing.
    if (dwFlags & DMUS_SEGF_QUEUE)
        lWait = 10000;
    else
        lWait = 4000;

    CHECKRUN(Log(FYILOGLEVEL, "%s is queued to play with dwFlags = %s.  Waiting %ds for it", szSegmentName[1], Log_DMUS_SEGF(dwFlags)));
    CHECKRUN(CountDown(lWait, lWait / 5)); 
    
    
    ptPerf8->StopEx(0, 0, 0);
 
    //Unload and free the segments.
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
    }
        
    SAFE_RELEASE(ptPath);
return hr;
};

 

/********************************************************************************
Using the DMUS_SEGF_ALIGN flag in combo with other flags, switch between segments
on certain boundaries.
********************************************************************************/
HRESULT Performance8_PlaySegment_TransPreAlign(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwFlags, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CHAR *szSegmentName[2] = {"T:\\DMTest1\\Perf8\\PlaySeg\\TransPreAlign0.sgt", "T:\\DMTest1\\Perf8\\PlaySeg\\TransPreAlign1.sgt"};
CtIDirectMusicSegment *ptSegment[2] = {NULL};
CtIDirectMusicAudioPath *ptPath = NULL;
MUSIC_TIME mt=0;
REFERENCE_TIME rt=0;
int i = 0;
DWORD dwWaits[] = {500, 1000, 2000, 3000};
DWORD dwCurrentSegment = 0;

//Load the needed medja.
MEDIAFILEDESC MediaFileDesc[] = {
                                {"DMusic/DMTest1/Perf8/PlaySeg/TransPreAlign0.sgt", "T:\\DMTest1\\Perf8\\PlaySeg\\",    COPY_IF_NEWER},
                                {"DMusic/DMTest1/Perf8/PlaySeg/TransPreAlign1.sgt", "T:\\DMTest1\\Perf8\\PlaySeg\\",    COPY_IF_NEWER},
                                {"DMusic/DMTest1/Perf8/PlaySeg/WaveEmb.sgt",        "T:\\DMTest1\\Perf8\\PlaySeg\\",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",                   "T:\\DMTest1\\Perf8\\PlaySeg\\",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
CHECKRUN(LoadMedia(MediaFileDesc));


    //Load segs.
    for (i=0; i<2; i++)
    {
        CHECKRUN(dmthLoadSegment(szSegmentName[i], &ptSegment[i]));
        CHECKRUN(ptSegment[i]->SetRepeats(DMUS_SEG_REPEAT_INFINITE));
    }
    
    //Create AP for them to play on.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 16, TRUE, &ptPath));

    //Play the original segment.
    dwCurrentSegment = 0;
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[dwCurrentSegment],0,0,0,0,NULL,0,ptPath));
    CHECKRUN(ptPerf8->GetTime(&rt, &mt));


    for (i=0; i<AMOUNT(dwWaits); i++)
    {
        CHECKRUN(Log(FYILOGLEVEL, "Playing %s, will invoke transition in %d ms.", szSegmentName[dwCurrentSegment], dwWaits[i]));
        CHECKRUN(Wait(dwWaits[i]));   
        //Transition based on the original time.
        dwCurrentSegment = !dwCurrentSegment;
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[dwCurrentSegment],0,0,dwFlags,mt,NULL,0,ptPath));
        CHECKRUN(Log(FYILOGLEVEL, "%s has been queued with dwFlags = %s.  Listing for 4s.", szSegmentName[dwCurrentSegment], Log_DMUS_SEGF(dwFlags)));
        CHECKRUN(Wait(4000)); 
    }

    //Stop everything.
    ptPerf8->StopEx(0, 0, 0);
 
    //Unload and free the segments.
    for (i=0; i<2; i++)
    {
        SAFE_RELEASE(ptSegment[i]);
    }
        
    SAFE_RELEASE(ptPath);
return hr;
};

//Scenarios!

//Streaming wave BVT (done)
//One-shot wave BVT (done)
//MIDI BVT (done)


/**********************************************************************
Plays a segment all the way through once.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Segment_WaitForEnd(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwSegmentNameServer, DWORD dwRepeats)
{
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
HRESULT hr = S_OK;
LPSTR szSegmentNameServer = (LPSTR)dwSegmentNameServer;
CHAR szSegmentName[MAX_PATH] = {0};

//Since we're only copying the file, we're assuming no .SGT files are being used that reference other wave files.
ASSERT(strstr(szSegmentNameServer, ".wav"));
CHECKRUN(MediaCopyFile(szSegmentNameServer));
strcpy(szSegmentName, MediaServerToLocal(szSegmentNameServer));

CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment));
CHECKRUN(ptSegment->SetRepeats(dwRepeats));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,NULL,0,ptPath));
CHECKRUN(Log(FYILOGLEVEL, "Playing %s %d times; waiting until done", szSegmentName, dwRepeats));
if (SUCCEEDED(hr))
{
    ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE);
    ExpectSegmentStop (ptPerf8, ptSegment, NULL, 0xFFFFFFFF, NULL, TRUE);
}

ptPerf8->StopEx(NULL, NULL, NULL);

SAFE_RELEASE(ptSegment);
SAFE_RELEASE(ptPath);
return hr;
};



/**********************************************************************
Plays a segment for 10 seconds.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Segment_WaitFor10Seconds(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwSegmentNameServer, DWORD dwRepeats)
{
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
HRESULT hr = S_OK;
LPSTR szSegmentNameServer = (LPSTR)dwSegmentNameServer;
CHAR szSegmentName[MAX_PATH] = {0};


//Since we're only copying the file, we're assuming no .SGT files are being used that reference other files.
ASSERT(strstr(szSegmentNameServer, ".wav"));
CHECKRUN(MediaCopyFile(szSegmentNameServer));
strcpy(szSegmentName, MediaServerToLocal(szSegmentNameServer));

CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment));
CHECKRUN(ptSegment->SetRepeats(dwRepeats));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,NULL,0,ptPath));
CHECKRUN(Log(FYILOGLEVEL, "Playing %s %d times; waiting for 10 seconds", szSegmentName, dwRepeats));
if (SUCCEEDED(hr))
{
    CountDown(10000, 1000);
}

ptPerf8->StopEx(NULL, NULL, NULL);

SAFE_RELEASE(ptSegment);
SAFE_RELEASE(ptPath);
return hr;
};






/**********************************************************************
if (szSegmentName == NULL) do (dwCount) multiple segments.
else do (dwCount) same segment.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Multiple(CtIDirectMusicPerformance *ptPerf8, LPSTR szSegmentNameServer, DWORD dwCount, DWORD dwRepeats)
{
#define MAX 30
CtIDirectMusicSegment *ptSegment[MAX] = {0};
CtIDirectMusicSegmentState *ptSegmentState[MAX] = {0};
CtIDirectMusicAudioPath *ptPath = NULL;
HRESULT hr = S_OK;
DWORD i = 0;
CHAR szSegmentName[MAX_PATH] = {0};


ASSERT(dwCount <= MAX);

//Make sure we're using a .WAV file, so there are no dependencies for medialoader to deal with.
if (szSegmentNameServer)
{
    ASSERT(strstr(szSegmentNameServer, ".wav"));
}
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
if (szSegmentNameServer)
{
    CHECKRUN(MediaCopyFile(szSegmentNameServer));
    CHECK   (strcpy(szSegmentName, MediaServerToLocal(szSegmentNameServer)));
    CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment[0]));
    CHECKRUN(ptSegment[0]->SetRepeats(dwRepeats));
    for (i=1; i<dwCount && SUCCEEDED(hr); i++)
    {
        ptSegment[i] = ptSegment[0];
        ptSegment[i]->AddRef();
    }
}
else
{
    //Download the file they specified and get its location. 
    Log(FYILOGLEVEL, "Downloading wave files from server...");
    CHECKRUN(MediaCopyDirectory(szWaveFiles[0]));    
    ASSERT(FilesInSameDirectory(szWaveFiles, dwCount));

    for (i=0; i<dwCount && SUCCEEDED(hr); i++)
    {
        CHECKRUN(dmthLoadSegment(MediaServerToLocal(szWaveFiles[i]), &ptSegment[i]));
        CHECKRUN(ptSegment[i]->SetRepeats(dwRepeats));
    }
}

/*
for (i=0; i<dwCount && SUCCEEDED(hr); i++)
{
    CHECKRUN(Log(FYILOGLEVEL, "Playing segment %2d/%2d (%s)", i+1, dwCount, szSegmentName ? szSegmentName : MediaServerToLocal(szWaveFiles[i])));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[i],0,0,i ? DMUS_SEGF_SECONDARY : 0,0,&ptSegmentState[i],0,ptPath));
    Sleep(1000);
}
*/


CHECKRUN(Log(FYILOGLEVEL, "Waiting 10 seconds."));
CountDown(10000, 1000);

//Stop and release everything.
for (i=0; i<dwCount; i++)
{
    if (ptSegmentState[i])
        ptPerf8->StopEx(ptSegmentState[i], 0, 0);
    SAFE_RELEASE(ptSegmentState[i]);
    SAFE_RELEASE(ptSegment[i]);
}

SAFE_RELEASE(ptPath);
return hr;
}

/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Multiple_NoRepeats(CtIDirectMusicPerformance *ptPerf8, DWORD szSegmentName, DWORD dwCount)
{

    return Performance8_PlaySegmentEx_Multiple(ptPerf8, (LPSTR)szSegmentName, dwCount, 0);
}

/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Multiple_OneRepeat(CtIDirectMusicPerformance *ptPerf8, DWORD szSegmentName, DWORD dwCount)
{

    return Performance8_PlaySegmentEx_Multiple(ptPerf8, (LPSTR)szSegmentName, dwCount, 1);
}





/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegment_Latency(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;

    CICMusic Music;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicSegmentState *ptSegmentState = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CHAR *szSegmentNameServer[] = {"WAV/08_08_1.wav", "WAV/48_16_2.wav"};
    CHAR szSegmentName[2][MAX_PATH] = {0};
    DWORD i=0;
    //Copy down the files.
    for (i=0; i<AMOUNT(szSegmentName); i++)
    {
        CHECKRUN(MediaCopyFile(szSegmentNameServer[i]));
        strcpy(szSegmentName[i], MediaServerToLocal(szSegmentNameServer[i]));                
        ASSERT(i < 2);
    }

    for (i=0; i<AMOUNT(szSegmentName); i++)
    {

        CHECKRUN(dmthLoadSegment(szSegmentName[i], &ptSegment));
        CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));    
        Log(FYILOGLEVEL, "Will play %s in 1 second - listen for latency.", szSegmentName[i]);
        Wait(1000);

        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,&ptSegmentState,0,ptPath));
        Log(FYILOGLEVEL, "Playing");
        Log(FYILOGLEVEL, "0ms");
        Wait(200);
        Log(FYILOGLEVEL, "200ms");
        Wait(200);
        Log(FYILOGLEVEL, "400ms");
        Wait(200);
        Log(FYILOGLEVEL, "600ms");
        Wait(200);
        Log(FYILOGLEVEL, "800ms");
        Wait(200);
        Log(FYILOGLEVEL, "1000ms");
    
        CHECKRUN(ptPerf8->StopEx(ptSegmentState, 0, 0));    

        SAFE_RELEASE(ptSegmentState);
        SAFE_RELEASE(ptSegment);
        SAFE_RELEASE(ptPath);
    }

    return hr;
}


/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegment_Latency_Interactive(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;

    CICMusic Music;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicSegmentState *ptSegmentState = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CHAR *szSegmentNameServer = "WAV/08_08_1.wav";
    CHAR szSegmentName[MAX_PATH];

    DWORD i=0;
    BOOL bDone = FALSE;
    DWORD dwButtonMask = 0;


    if (g_TestParams.bSkipUserInput)
        return S_OK;

    CHECKRUN(MediaCopyFile(szSegmentNameServer));
    strcpy(szSegmentName, szSegmentNameServer);

    CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));    


    while (!bDone)
    {
        Log(ABORTLOGLEVEL, "Press button A to trigger the sound; B to quit.");
        dwButtonMask = dmthWaitForAnyButton(DMTH_JOYBUTTON_A | DMTH_JOYBUTTON_B);
        if (dwButtonMask & DMTH_JOYBUTTON_A)
        {            
            CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,&ptSegmentState,0,ptPath));
            dmthWaitForAllButtonsUp();
        }
        else
        {
            bDone = TRUE;
        }
    };


    CHECKRUN(ptPerf8->StopEx(ptSegmentState, 0, 0));    
    SAFE_RELEASE(ptSegmentState);
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptPath);

    return hr;
}


/********************************************************************************
dwTime = 0xFFFFFFFF = WAIT for segment to stop
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_Valid_Play(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime)
{
    HRESULT hr = S_OK;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CICMusic Music;
    BYTE bMasterGrooveLevel_100 = 100;
    BYTE bMasterGrooveLevel_0 = 0;  
    LPCSTR szFullPathServer = NULL;
    CHAR szFullPath[MAX_PATH];

    //Cast our parameter.
    szFullPathServer = LPSTR(dwFilename);
    
    if (_strcmpi(szFullPathServer, "DEFAULT") == 0)
    {
        strcpy(szFullPath,g_TestParams.szDefaultMedia);
    }
    else
    {
        //Copy all files locally, main1.dls, and get the name of your specific file.

        CHECKRUN(MediaCopyDirectory(szFullPathServer));
        strcpy(szFullPath, MediaServerToLocal(szFullPathServer));
        CHAR szDirectoryLocal[MAX_PATH] = {0};
        CHECKRUN(ChopPath(szFullPath, szDirectoryLocal, NULL));

        MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DLS/Main1.DLS", szDirectoryLocal, COPY_IF_NEWER},
                                    {TERMINATE}
                                    };

        CHECKRUN(LoadMedia(MediaFileDesc));
    }

    CHECKRUN(Music.Init(ptPerf8, szFullPath, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Music.GetInterface(&ptPath));
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE));
    CHECKRUN(ptSegment->SetRepeats(0));
    CHECKRUN(InsertToolTrace(ptPath));


    CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bMasterGrooveLevel_100, sizeof(bMasterGrooveLevel_100)));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,0,0,ptPath));

    if (dwTime == 0xFFFFFFFF)
    {
        CHECKRUN(Log(FYILOGLEVEL, "Playing %s all the way through.", szFullPath));
        CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, szFullPath));
        CHECKRUN(ExpectSegmentStop (ptPerf8, ptSegment, NULL, 20000, szFullPath)); //15 seconds max length of segment.
    }
    else
    {
        CHECKRUN(Log(FYILOGLEVEL, "Playing %s for %d seconds.", PathFindFileName(szFullPath), dwTime));
        CHECKRUN(CountDown(dwTime * 1000, 1000));
    }

    ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, (void *)&bMasterGrooveLevel_0, sizeof(bMasterGrooveLevel_0));
    if (ptPath)
        RemoveToolTrace(ptPath);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);

    dmthPromptUserTestResults(hr, EXPECT_YES, "Does %s playing correctly?", PathFindFileName(szFullPath));
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_PerfMem(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD bFull)
{
    HRESULT hr = S_OK;
    IDirectMusicLoader *pLoader = NULL;
    IDirectMusicPerformance *pPerf = NULL;
    IDirectMusicSegment *pSegment = NULL;
    IDirectMusicAudioPath *pPath= NULL;
    DWORD dwStartTime = 0;
    DWORD dwTimePassed = 0;
    DWORD dwCounter = 0;
    CHAR szPath[MAX_PATH];
    CHAR szFile[MAX_PATH];
    LPCSTR szFullPathServer = NULL;
    CHAR szFullPath[MAX_PATH] = {0};
    szFullPathServer = LPSTR(dwFilename);
    
    //Cast our parameter.
    szFullPathServer = LPSTR(dwFilename);

    //Copy all files locally and get the name of your specific file.
    CHECKRUN(MediaCopyDirectory(szFullPathServer));
    strcpy(szFullPath, MediaServerToLocal(szFullPathServer));


    CHECKRUN(ChopPath(szFullPath, szPath, szFile));
    CHECKRUN(ptPerf8->GetRealObjPtr(&pPerf));

    //If we're testing the full load/free scenario, then do this inside the loop.  Otherwise do it here.
    if (!bFull)
    {
        CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
        CHECKRUN(pPerf->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &pPath));
        CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
        CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFile, (void **)&pSegment));
    }



    dwStartTime = timeGetTime();
    do
    {

        //If we're testing the full load/free scenario, then do everything *inside* the loop.
        if (bFull)
        {
            CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&pLoader));
            CHECKRUN(pPerf->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &pPath));
            CHECKRUN(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
            CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFile, (void **)&pSegment));
        }

        //Here's the money maker.
       CHECKRUN(pPerf->PlaySegmentEx(pSegment,0,0,0,0,NULL,0,pPath));
       Wait(100);
        
        //If we're testing the full load/free scenario, then do everything *inside* the loop.
        if (bFull)
        {
            
            SAFE_RELEASE(pPath);
            SAFE_RELEASE(pSegment);
            SAFE_RELEASE(pLoader);

        }



        dwTimePassed = timeGetTime() - dwStartTime;
        if (dwTimePassed / 1000 > dwCounter)
        {
            Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
            dwCounter++;
        }
    }
    while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));



    //Make sure everything gets released.
    SAFE_RELEASE(pPath);
    SAFE_RELEASE(pSegment);
    SAFE_RELEASE(pLoader);
    SAFE_RELEASE(pPerf);

    if (FAILED(hr))
        Log(ABORTLOGLEVEL, "Loader_GetObject_PerfMem_Type_Full failed with %s (%08X)", tdmXlatHRESULT(hr), hr);
    return hr;
};



/********************************************************************************
dwTime = 0xFFFFFFFF = WAIT for segment to stop

NOTE: This is meant to be called when DoWork is NOT being called!!  See the first check.
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_Valid_PlayBeforeDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime)
{
    HRESULT hr = S_OK;
    CICMusic Music;

    //Copy the current content.
    LPCSTR szFullPathServer = LPSTR(dwFilename);
    CHAR szFullPath[MAX_PATH] = {0};
    strcpy(szFullPath, MediaServerToLocal(szFullPathServer));
    CHECKRUN(MediaCopyDirectory(szFullPathServer));
    //Also copy MAIN1.DLS, how annoying.
    CHAR szDirectory[MAX_PATH] = {0};
    CHECKRUN(ChopPath(szFullPath, szDirectory, NULL));
    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DLS/Main1.DLS", szDirectory, COPY_IF_NEWER},
                                    {TERMINATE}
                                    };


    if (0 != g_TestParams.dwDoWorkFrequency)
        Log(ABORTLOGLEVEL, "ERROR!!! Performance8_PlaySegmentEx_Valid_PlayBeforeDoWork() called when DoWork thread is active!!!");

    CHECKRUN(Music.Init(ptPerf8, szFullPath, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    Wait(2000);
    CHECK(DirectMusicDoWork(16));

    dmthPromptUserTestResults(hr, EXPECT_YES, "Does %s playing correctly?", PathFindFileName(szFullPath));
    return hr;
};


/********************************************************************************
dwTime = 0xFFFFFFFF = WAIT for segment to stop

NOTE: This is meant to be called when DoWork is NOT being called!!  See the first check.
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_Valid_DelayDoWork(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwDelay)
{
    HRESULT hr = S_OK;
    CICMusic Music;
    DWORD dwStartTime = 0;
    HRESULT hrIsPlaying = S_OK;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;

    //Copy the current content.
    LPCSTR szFullPathServer = LPSTR(dwFilename);
    CHAR szFullPath[MAX_PATH] = {0};
    strcpy(szFullPath, MediaServerToLocal(szFullPathServer));
    CHECKRUN(MediaCopyDirectory(szFullPathServer));
    //Also copy MAIN1.DLS, how annoying.
    CHAR szDirectory[MAX_PATH] = {0};
    CHECKRUN(ChopPath(szFullPath, szDirectory, NULL));
    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DLS/Main1.DLS", szDirectory, COPY_IF_NEWER},
                                    {TERMINATE}
                                    };


    CHECKRUN(LoadMedia(MediaFileDesc));
    if (0 != g_TestParams.dwDoWorkFrequency)
        Log(ABORTLOGLEVEL, "ERROR!!! Performance8_PlaySegmentEx_Valid_DelayDoWork() called when DoWork thread is active!!!");


    CHECKRUN(Music.Init(ptPerf8, szFullPath, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Music.GetInterface(&ptPath));
    CHECKRUN(ptPerf8->StopEx(0, __int64(0), 0));
    CHECKRUN(ptSegment->SetRepeats(0));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
    CHECKRUN(CountDown(dwDelay, dwDelay));

    dwStartTime = timeGetTime();
    do
    {            
        CHECK(DirectMusicDoWork(16));
        CHECK(Sleep(16));
        CHECK(hrIsPlaying = ptPerf8->IsPlaying(ptSegment, NULL));
    }
    while (S_OK == hrIsPlaying && SUCCEEDED(hr));


    dmthPromptUserTestResults(hr, EXPECT_YES, "Does %s playing correctly?", PathFindFileName(szFullPath));
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptPath);
    return hr;
};




/********************************************************************************
********************************************************************************/
HRESULT Performance8_PlaySegmentEx_Valid_SetFrequency(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime)
{
    HRESULT hr = S_OK;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CICMusic Music;

    //Copy the current content.
    LPCSTR szFullPathServer = LPSTR(dwFilename);
    CHAR szFullPath[MAX_PATH] = {0};
    strcpy(szFullPath,MediaServerToLocal(szFullPathServer));
    CHECKRUN(MediaCopyDirectory(szFullPathServer));

    //Also copy MAIN1.DLS, how annoying.
    CHAR szDirectory[MAX_PATH] = {0};
    CHECKRUN(ChopPath(szFullPath, szDirectory, NULL));
    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DLS/Main1.DLS", szDirectory, COPY_IF_NEWER},
                                    {TERMINATE}
                                    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    CHECKRUN(Music.Init(ptPerf8, szFullPath, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Music.GetInterface(&ptPath));
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    CHECKRUN(ptSegment->SetRepeats(0));
    //Set pitch to increase.
    CHECKRUN(ptPath->SetPitch(2000, 5000));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,0,0,ptPath));


    if (dwTime == 0xFFFFFFFF)
    {
        CHECKRUN(Log(FYILOGLEVEL, "%s should repeat normally.", szFullPath));
        CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, szFullPath));
        CHECKRUN(ExpectSegmentStop (ptPerf8, ptSegment, NULL, 20000, szFullPath)); //15 seconds max length of segment.
    }
    else
    {
        CHECKRUN(Log(FYILOGLEVEL, "Playing %s for %d seconds.", PathFindFileName(szFullPath), dwTime));
        CHECKRUN(CountDown(dwTime * 1000, 1000));
    }

    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);
    dmthPromptUserTestResults(hr, EXPECT_YES, "Does %s playing correctly?", PathFindFileName(szFullPath));
    return hr;
};


HRESULT Sleep_Zero(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwTime)
{
Sleep(0);
return S_OK;

}



//prototype
HRESULT Performance8_PlaySegmentEx_QuickStartStop(CtIDirectMusicPerformance *ptPerf8, LPSTR *szWaveFiles, DWORD dwNumFiles, DWORD dwNumSounds);


/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_QuickStartStop_ADPCM(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_PlaySegmentEx_QuickStartStop(ptPerf8, szADPCMWaves, NUMELEMS(szADPCMWaves), 10);
};

/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_QuickStartStop_WaveFiles(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_PlaySegmentEx_QuickStartStop(ptPerf8, szWaveFiles, NUMELEMS(szWaveFiles), 10);
};

/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_QuickStartStop_WaveSegments(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Performance8_PlaySegmentEx_QuickStartStop(ptPerf8, szWaveSegments, NUMELEMS(szWaveSegments), 1);
};



/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_QuickStartStop(CtIDirectMusicPerformance *ptPerf8, LPSTR *szWaveFiles, DWORD dwNumFiles, DWORD dwNumSounds)
{
#define TOTALTIME 2000  //Test for x milliseconds.
#define INTERVAL 0     //Check every x milliseconds
#define HITPERCENTAGE 100 //30% of the time, toggle sound on/off

//Seed with some random, but constant, value.
srand(101400);

CtIDirectMusicSegment *ptSegment            [100] = {0};
CtIDirectMusicSegmentState *ptSegmentState  [100] = {0};
CtIDirectMusicAudioPath *ptPath             [100] = {NULL};

HRESULT hr = S_OK;
DWORD i = 0;


//Assuming all the files are in the same directory.
CHECKRUN(MediaCopyDirectory(szWaveFiles[0]));

for (i=0; i<dwNumSounds; i++)
{
    //Create a 3D path.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath[i]));

    //Load a random file for the path.
//    CHECKRUN(dmthLoadSegment(szWaveFiles[rand() % dwNumFiles], &ptSegment[i]));

    CHECKRUN(dmthLoadSegment(MediaServerToLocal(szWaveFiles[i % dwNumFiles]), &ptSegment[i]));

    //Verify these files are in the perfmem directory.
    ASSERT(!strstr(szWaveFiles[i % dwNumFiles], "Perfmem"));
}

Log(FYILOGLEVEL, "\n\nPLAYING %d sounds FOR %u ms", dwNumSounds, TOTALTIME);

//For 5 seconds start and stop sounds.
DWORD dwStartTime = timeGetTime();
do
{
    for (i=0; i<dwNumSounds; i++)
    {
        if (rand() % 100 < HITPERCENTAGE)
        {
            if (!ptSegmentState[i])
            {
                CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[i], NULL, NULL, 0, 0, &ptSegmentState[i], NULL, ptPath[i]));
            }
            else
            {
                CHECKRUN(ptPerf8->StopEx(ptSegmentState[i], 0, 0));
                RELEASE(ptSegmentState[i]);
            }
        }

    }

    Sleep(INTERVAL);
}
while (timeGetTime() - dwStartTime < TOTALTIME);




//Stop all segment states.
Log(FYILOGLEVEL, "STOPPING ALL SEGMENT STATES...");
for (i=0; i<dwNumSounds; i++)
{
    if (ptSegmentState[i])
    {
        CHECKRUN(ptPerf8->StopEx(ptSegmentState[i], 0, 0));
        RELEASE(ptSegmentState[i]);
    }
}

Log(FYILOGLEVEL, "\n\nALL SOUNDS SHOULD BE TOTALLY STOPPED NOW.\n\n");
Log(FYILOGLEVEL, "Waiting 5 seconds so you can hear any rogue sounds.");
Log(FYILOGLEVEL, "File a bug if you hear any");
Sleep(5000);

//Release everything.
for (i=0; i<dwNumSounds; i++)
{
    RELEASE(ptSegmentState[i]);
    RELEASE(ptSegment[i]);
    RELEASE(ptPath[i]);
}


return hr;
}

#undef dwNumFiles 
#undef TOTALTIME
#undef INTERVAL 
#undef HITPERCENTAGE 




/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_QuickStartStopOneFile(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicSegmentState *ptSegmentState = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
HRESULT hr = S_OK;
DWORD i = 0;


LPCSTR szWaveServer = "WAV/08_08_2.wav";
LPCSTR szWave       = MediaServerToLocal(szWaveServer);

CHECKRUN(MediaCopyFile(szWaveServer));

//Create a 3D path.
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));

//Load a random file for the path.
CHECKRUN(dmthLoadSegment(szWave, &ptSegment));

//Quickly start and stop segment.
CHECKRUN(Log(FYILOGLEVEL, "Waiting 1 second for download to complete, then we'll play and stop the file..."));
CHECKRUN(Wait(1000));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, &ptSegmentState, NULL, ptPath));
CHECKRUN(ptPerf8->StopEx(ptSegmentState, 0, 0));

//Check to see if sound has stopped.
CHECKRUN(Log(FYILOGLEVEL, "Sound should be totally stopped now.  File bug if it ain't"));
CHECKRUN(Log(FYILOGLEVEL, "Waiting 2 seconds"));
CHECKRUN(Wait(2000));

//Stop playback.
if (ptSegmentState)
{
    CHECKRUN(ptPerf8->StopEx(ptSegmentState, 0, 0));
}

//Release
RELEASE(ptSegmentState);
RELEASE(ptSegment);
RELEASE(ptPath);

return hr;
}




/********************************************************************************
********************************************************************************/
HRESULT Script_CallRoutine_Valid_TransitionTempo(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT hr = S_OK;	
    CtIDirectMusicScript *ptScript          = NULL;
    CtIDirectMusicLoader *ptLoader          = NULL;
    CtIDirectMusicAudioPath *ptPath         = NULL;

    LPCSTR szScriptServer = "DMusic/DMTest1/Perf8/PlaySeg/TransTempo/TransTempo.spt";
    CHAR szScript[MAX_PATH] = {0};
    CHAR  szPath[MAX_PATH] = {0};
    CHAR  szFile[MAX_PATH] = {0};
    LPSTR szRoutine[2] = {"StartMusic", "Transition"};

    //Copy the current content.
    strcpy(szScript, MediaServerToLocal(szScriptServer));
    CHECKRUN(MediaCopyDirectory(szScriptServer));

    //Also copy MAIN1.DLS, how annoying.
    CHECKRUN(ChopPath(szScript, szPath, szFile));
    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"DLS/Main1.DLS", szPath, COPY_IF_NEWER},
                                    {TERMINATE}
                                    };
    CHECKRUN(LoadMedia(MediaFileDesc));

    //For processing
    LONG  lNextRoutineDelay = 0;
 
    //Init performance
    CHECKRUN( ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
    CHECKRUN( ptPerf8->SetDefaultAudioPath(ptPath));
    CHECKRUN( InsertToolTrace(ptPerf8));
    
    //Init loader
    CHECKRUN( dmthCreateLoader( CTIID_IDirectMusicLoader8, &ptLoader ) );
    CHECKRUN( ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));


    //Load/Init our script
    CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IDirectMusicScript, szFile, (void **)&ptScript));
    CHECKRUN(ptScript->Init( ptPerf8, NULL));

    //Call the routine we're supposed to.  If it doesn't exist, the script is messed up.
    CHECKRUN(ptScript->CallRoutine(szRoutine[0], NULL));
    Log(FYILOGLEVEL, "Calling %s, waiting 2 seconds before transition", szRoutine[0]);
    Wait(2000);
    CHECKRUN(ptScript->CallRoutine(szRoutine[1], NULL));
    Log(FYILOGLEVEL, "Calling %s, should hear transition segment playing at same tempo as previous segment.", szRoutine[1]);
    Log(FYILOGLEVEL, "Waiting 5 seconds");
    
    Wait(5000);
        
    //Stop everything.
    ptPerf8->StopEx(0, 0, 0);

    RemoveToolTrace(ptPerf8);
    ptPerf8->SetDefaultAudioPath(NULL);
    SAFE_RELEASE(ptScript);
    SAFE_RELEASE(ptLoader);
    SAFE_RELEASE(ptPath);
    return hr;
};




/**********************************************************************
if (szSegmentName == NULL) do (dwCount) multiple segments.
else do (dwCount) same segment.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_RelativeVolume(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
DWORD i = 0;
LPCSTR szWaveServer = "WAV/GuitarLoop.wav";
LPCSTR szWave = MediaServerToLocal(szWaveServer);
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
DWORD dwPath[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_APATH_SHARED_STEREO, DMUS_APATH_DYNAMIC_MONO, DMUS_APATH_DYNAMIC_3D};
IDirectSoundBuffer *pBuffer = NULL;
DWORD dwDataSize    = 0;
DSBUFFERDESC dsbd = {0};

CHECKRUN(MediaCopyFile(szWaveServer));

//These will be allocated and must be freed.
LPVOID pvData        = NULL;
LPCWAVEFORMATEX pWfx = NULL;

//Load the wave file into our soundbuffer
CHECKRUN(HelpLoadWaveFileData(szWave, &pWfx, &pvData, &dwDataSize));
dsbd.dwSize = sizeof(dsbd);
dsbd.dwFlags = 0;
dsbd.dwBufferBytes = dwDataSize;
dsbd.lpwfxFormat = (LPWAVEFORMATEX)pWfx;
//dsbd.dwMixBinMask = 0;      //Defaults, I guess.
dsbd.lpMixBins = NULL;      //Defaults, I guess.
//dsbd.dwMixBinMask = DSMIXBIN_FRONT_LEFT;

//dsbd.dwInputMixBinMask = 0;
dsbd.dwInputMixBin = 0;
CHECKRUN(DirectSoundCreateBuffer(&dsbd, &pBuffer));
CHECKRUN(pBuffer->SetBufferData(pvData, dwDataSize));

//Load the wave file into our segment.
CHECKRUN(dmthLoadSegment(szWave, &ptSegment));


for (i=0; i<NUMELEMS(dwPath) * 2 && SUCCEEDED(hr); i++)
{    
    //Play the DSound version
    if (i%2 == 0)
    {        
        CHECKRUN(pBuffer->SetCurrentPosition(0));
        CHECKRUN(Log(FYILOGLEVEL, "Playing via IDirectSoundBuffer"));
        CHECKRUN(pBuffer->Play(0, 0, 0));
        CHECKRUN(Wait(1500));
        CHECKRUN(pBuffer->Stop());

    }
    //Play the DMusic version on an audiopath
    else
    {    

        CHECKRUN(Log(FYILOGLEVEL, "Playing on %s", tdmXlatDMUS_APATH(dwPath[i/2])));

        //Create the one of our paths.
        CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPath[i / 2], 128, TRUE, &ptPath));

        //Play the file.
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));

        //Release the path (the playing segment will have a reference to it, making things okay)
        CHECKRUN(Wait(1500));
        CHECKRUN(ptPerf8->StopEx(ptSegment, NULL, NULL));
        RELEASE(ptPath);
    }
}

//Release DMusic stuff
RELEASE(ptSegment);
RELEASE(ptPath);

//Release DSound stuff
RELEASE(pBuffer);

//Release allocated memory.
delete [](BYTE *)pvData;
delete [](BYTE *)pWfx;

return hr;
}




/**********************************************************************
if (szSegmentName == NULL) do (dwCount) multiple segments.
else do (dwCount) same segment.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_CompareWithDSound(CtIDirectMusicPerformance *ptPerf8, DWORD dwPath, DWORD dwWave)
{
HRESULT hr = S_OK;
DWORD i = 0;
LPSTR szWaveServer = LPSTR(dwWave);
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
IDirectSoundBuffer *pBuffer = NULL;
DWORD dwDataSize    = 0;
DSBUFFERDESC dsbd = {0};
DWORD dwStatus = 0;
CHAR szWave[MAX_PATH] = {0};

ASSERT(szWaveServer);
ASSERT(strstr(szWaveServer, ".wav"));
strcpy(szWave, MediaServerToLocal(szWaveServer));
_strlwr(szWave);
CHECKRUN(MediaCopyFile(szWaveServer));

//These will be allocated and must be freed.
LPVOID pvData        = NULL;
LPCWAVEFORMATEX pWfx = NULL;

//Load the wave file into our soundbuffer
CHECKRUN(HelpLoadWaveFileData(szWave, &pWfx, &pvData, &dwDataSize));


if (SUCCEEDED(hr))
{
    DSMIXBINVOLUMEPAIR pair[2] = {DSMIXBIN_FRONT_LEFT, 0, DSMIXBIN_FRONT_RIGHT, 0};
    DSMIXBINS dsmixbins = {2, pair};
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = 0;
    dsbd.dwBufferBytes = dwDataSize;
    dsbd.lpwfxFormat = (LPWAVEFORMATEX)pWfx;
    dsbd.lpMixBins = &dsmixbins;
    dsbd.dwInputMixBin = 0;
    CHECKRUN(DirectSoundCreateBuffer(&dsbd, &pBuffer));
}

CHECKRUN(pBuffer->SetBufferData(pvData, dwDataSize));

//Load the wave file into our segment.
CHECKRUN(dmthLoadSegment(szWave, &ptSegment));

//DSound
CHECKRUN(pBuffer->SetCurrentPosition(0));
CHECKRUN(Log(FYILOGLEVEL, "Playing via IDirectSoundBuffer"));
CHECKRUN(pBuffer->Play(0, 0, 0));
if (SUCCEEDED(hr))
{
    do
    {
        CHECKRUN(pBuffer->GetStatus(&dwStatus));
    }
    while (DSBSTATUS_PLAYING & dwStatus);
}
CHECKRUN(pBuffer->Stop());
CHECKRUN(Wait(1000));

///DMusic
CHECKRUN(Log(FYILOGLEVEL, "Playing on %s", tdmXlatDMUS_APATH(dwPath)));
CHECKRUN(ptPerf8->CreateStandardAudioPath(dwPath, 128, TRUE, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000,  szWave, TRUE));

if (strstr(szWave, "_lp_"))
{
    CHECKRUN(WaitForSegmentStop(ptPerf8, ptSegment, NULL, 2000, TRUE));
}
else
{
    CHECKRUN(ExpectSegmentStop (ptPerf8, ptSegment, NULL, 20000, szWave, TRUE));
}

if (ptPerf8)
    ptPerf8->StopEx(0, 0, 0);


//Release DMusic stuff
RELEASE(ptSegment);
RELEASE(ptPath);

//Release DSound stuff
RELEASE(pBuffer);

//Release allocated memory.
delete [](BYTE *)pvData;
delete [](BYTE *)pWfx;

return hr;
}






/**********************************************************************
Play a segment and verify it takes the expected amount of time.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Timing(CtIDirectMusicPerformance *ptPerf8, DWORD dwSegment, DWORD dwExpectedTime)
{
HRESULT hr = S_OK;
DWORD i = 0;
LPCSTR szSegmentServer = LPSTR(dwSegment);
CHAR szSegment[MAX_PATH] = {0};
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
DWORD dwStartTime = 0;
DWORD dwDiffTime = 0;
double fErrorPercent = 0.f;
const double fMaxErrorPercent = 5.f;

//Copy everything in that directory.
//BUGBUG: We may need to copy something else as well?
CHECKRUN(MediaCopyDirectory(szSegmentServer));
strcpy(szSegment, MediaServerToLocal(szSegmentServer));

//Load the wave file into our segment.
CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));

///DMusic
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000, NULL, TRUE));
dwStartTime = timeGetTime();
CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, dwExpectedTime * 2, NULL, TRUE));
dwDiffTime = timeGetTime() - dwStartTime;

//Analyze the time spent playing the segment.
if (SUCCEEDED(hr))
{
    fErrorPercent = fabs(double(dwDiffTime) - double(dwExpectedTime)) * 100.f / double(dwExpectedTime);
    Log(FYILOGLEVEL, "%s took %ums to play.  Expected %ums", PathFindFileName(szSegment), dwDiffTime, dwExpectedTime);    
    Log(FYILOGLEVEL, "Error: %5.2f%%.  Maximum Allowed: %5.2f%%", fErrorPercent, fMaxErrorPercent);
    if (fErrorPercent > fMaxErrorPercent)
    {
        hr = E_FAIL;
    }
}

//Release DMusic stuff
//if (ptSegment && ptPath)
//{
//    ptSegment->Unload(ptPath);
//}
RELEASE(ptSegment);
RELEASE(ptPath);

return hr;
}


/**********************************************************************
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_Leak(CtIDirectMusicPerformance *ptPerf8, DWORD dwSegment1, DWORD dwSegment2)
{
HRESULT hr = S_OK;
DWORD i = 0;

CtIDirectMusicAudioPath *ptPath = NULL;
CtIDirectMusicSegment *ptSegment = NULL;
LPCSTR szSegmentServer = (LPSTR)dwSegment1;
LPCSTR szSegment = MediaServerToLocal(szSegmentServer);

ASSERT(strstr(szSegment, ".wav"));
CHECKRUN(MediaCopyFile(szSegmentServer));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREO, 128, TRUE, &ptPath));
CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0,0,NULL, NULL, ptPath));//  repro
CHECKRUN(ptPerf8->StopEx(0, 0, 0));

//Release the good stuff.
RELEASE(ptPath);
RELEASE(ptSegment);
return hr;
}





/**********************************************************************
if (szSegmentName == NULL) do (dwCount) multiple segments.
else do (dwCount) same segment.
**********************************************************************/
HRESULT Performance8_PlaySegmentEx_StartStopStreaming_PerfMem(CtIDirectMusicPerformance *ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
#define MAX 30
CtIDirectMusicSegment *ptSegment[MAX] = {0};
//CtIDirectMusicSegmentState *ptSegmentState[MAX] = {0};
CtIDirectMusicAudioPath *ptPath = NULL;
DWORD dwStartTime = 0;
DWORD dwTimePassed = 0;
DWORD dwCounter = 0;
DWORD dwCount = 0;
HRESULT hr = S_OK;
DWORD i = 0;


dwCount = AMOUNT(szWaveFiles);

Log(FYILOGLEVEL, "Downloading wave files from server...");
/*

for (i=0; i<dwCount && SUCCEEDED(hr); i++)
{
    CHECKRUN(MediaCopyFile(szWaveFiles[i]));
};
*/
//Since they're all in the same directory, do them all at once.
CHECKRUN(MediaCopyDirectory(szWaveFiles[0]));



CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
for (i=0; i<dwCount && SUCCEEDED(hr); i++)
{
    CHECKRUN(dmthLoadSegment(MediaServerToLocal(szWaveFiles[i]), &ptSegment[i]));
    CHECKRUN(ptSegment[i]->SetWavePlaybackParams(DMUS_WAVEF_STREAMING, DMUS_READAHEAD_MIN));
}


dwStartTime = timeGetTime();
do
{
    for (i=0; i<dwCount && SUCCEEDED(hr); i++)
    {
        //CHECKRUN(Log(FYILOGLEVEL, "Playing segment %2d/%2d (%s)", i+1, dwCount, MediaServerToLocal(szWaveFiles[i])));
        if (rand() % 2)
        {
            CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[i],0,0,DMUS_SEGF_SECONDARY,0,NULL,0,ptPath));
        }
        
        if (SUCCEEDED(hr))
        {
            HRESULT hrPlaying = ptPerf8->IsPlaying(ptSegment[i], NULL);
            if (S_OK == hrPlaying)
            {
                if (rand() % 2)
                   ptPerf8->StopEx(ptSegment[i],0,0);
            }

        }
    }

    Sleep(rand() % 32);

    dwTimePassed = timeGetTime() - dwStartTime;
    if (dwTimePassed / 1000 > dwCounter)
    {
        Log(FYILOGLEVEL, "%d/%d seconds...", dwCounter, g_TestParams.dwPerfWait);
        dwCounter++;
    }
}
while ((dwTimePassed < g_TestParams.dwPerfWait * 1000) && SUCCEEDED(hr));



//Stop and release everything.
ptPerf8->StopEx(0, 0, 0);
for (i=0; i<dwCount && SUCCEEDED(hr); i++)
{
//    ptPerf8->StopEx(ptSegmentState[i], 0, 0);
//    SAFE_RELEASE(ptSegmentState[i]);
    SAFE_RELEASE(ptSegment[i]);
}

SAFE_RELEASE(ptPath);
return hr;
}
#undef MAX



/********************************************************************************
/********************************************************************************/
HRESULT Performance8_PlaySegmentEx_ClockTime_Looping(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegment, DWORD dwLoopPoint1, DWORD dwLoopPoint2, DWORD dwRepeats)
{
    HRESULT hr = S_OK;
    HRESULT hrPlaying = S_OK;
    REFERENCE_TIME rtLoopPoint[2] = {dwLoopPoint1, dwLoopPoint2};
    REFERENCE_TIME rtExpectedPlayingTime = 0;
    DWORD dwExpectedPlayingTime = 0;
    DWORD dwStartTime = 0;
    DWORD dwStopTime  = 0;
    DWORD dwPlayingTime = 0;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    FLOAT fErrorPercent = 0.f;
    FLOAT fMaxErrorPercent = 2.f;
    BOOL bClockTimeSegment = FALSE;
    REFERENCE_TIME rtDummy = 0;

    //Assuming media has already been loaded from the calling function.    
    CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));    
    CHECK   (dwStartTime = timeGetTime());
    CHECK   (Log(FYILOGLEVEL, "Playing %s (contains loop points at %d and %d ticks, and repeats %d time(s)", PathFindFileName(szSegment), dwLoopPoint1, dwLoopPoint2, dwRepeats));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, 0, 0, 0, 0, 0, 0, ptPath));
    //CHECKRUN(Log(FYILOGLEVEL, "Expecting loop points to %I64u and %I64u", rtLoopPoint[0], rtLoopPoint[1]));

    //Calculate how long the file SHOULD play given these loop points.  6 + (loop2 - loop1) * repeats
    rtExpectedPlayingTime = 60000000 + (rtLoopPoint[1] - rtLoopPoint[0]) * dwRepeats;

    dwExpectedPlayingTime = DWORD(rtExpectedPlayingTime / 10000);
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, dwExpectedPlayingTime, NULL));
    CHECKRUN(WaitForSegmentStop (ptPerf8, ptSegment, NULL, dwExpectedPlayingTime * 4, NULL));
    CHECK   (dwStopTime = timeGetTime());
    if (SUCCEEDED(hr))
    {
        dwPlayingTime = dwStopTime - dwStartTime;
        fErrorPercent = 100.f * FLOAT(fabs(LONG(dwPlayingTime - dwExpectedPlayingTime) / FLOAT(dwExpectedPlayingTime)));
        Log(FYILOGLEVEL, "Segment stopped at %ums, expected to stop at %ums.", dwPlayingTime, dwExpectedPlayingTime);
        Log(FYILOGLEVEL, "Error = %.2f%%, maximum allowed error is %.2f%%", fErrorPercent, fMaxErrorPercent);
        if (fErrorPercent > fMaxErrorPercent)
        {
            Log(ABORTLOGLEVEL, "FAILURE!!  %.2f%% exceeds maximum allowed error of %.2f%%!!!", fErrorPercent, fMaxErrorPercent);
            hr = E_FAIL;
        }
    }

    ptPerf8->StopEx(0, 0, 0);    
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);

    return hr;

};



/********************************************************************************
/********************************************************************************/
/*
HRESULT Performance8_PlaySegmentEx_ClockTime_Looping_Wave(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    MEDIAFILEDESC MediaFileDesc[] = {
                                    //{"DMusic/DMTest1/Segment8/GetClockTimeLength/CountTo6.wav", "T:\\DMTest1\\Segment8\\GetClockTimeLength\\",    COPY_IF_NEWER},
                                    {"WAV/CountTo6Looped.wav", "T:\\WAV\\",    COPY_IF_NEWER},
                                    {TERMINATE}
                                    };

    CHECKRUN(LoadMedia(MediaFileDesc));
    CHECKRUN(Performance8_PlaySegmentEx_ClockTime_Looping(ptPerf8, "T:\\WAV\\CountTo6Looped.wav", 0, 60000000));
    return hr;
}

*/

/********************************************************************************
/********************************************************************************/
#define SEGMENT 0
#define WAVE 1

HRESULT Performance8_PlaySegmentEx_ClockTime_Looping(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSuffix, DWORD dwType)
{
    HRESULT hr = S_OK;
    LPCSTR szSuffix = (LPCSTR)dwSuffix;
    DWORD dwNumber[3] = {0};

    CHAR szSrcSeg[MAX_PATH] = {0};
    CHAR szDstSeg[MAX_PATH] = {0};
    CHAR szDstPath[MAX_PATH] = {0};

    //We're getting the start point, endpoint, and repeat count from the title of the wave file.  Totally hacky but works as
    //  long as we don't use some random wave file in here.
    {
        LPCSTR szCurrent = szSuffix;
        for (DWORD i = 0; i<3; i++)
        {
            ASSERT(*szCurrent);
            while (!isdigit(*szCurrent))
            {
                ASSERT(*szCurrent);
                szCurrent++;
            }
            dwNumber[i] = atoi(szCurrent++);
        }
    }


    sprintf(szSrcSeg, "DMusic/DMTest1/Perf8/PlaySeg/CTLooping/%sCTLooped_%s.%s", (dwType==SEGMENT) ? "Seg" : "Wav", szSuffix, (dwType==SEGMENT) ? "sgt" : "wav");
    sprintf(szDstSeg, MediaServerToLocal(szSrcSeg));
    CHECKRUN(ChopPath(szDstSeg, szDstPath, NULL));

    MEDIAFILEDESC MediaFileDesc[3] = {0};
    
    MediaFileDesc[0].lpszFromPath = szSrcSeg;
    MediaFileDesc[0].lpszToPath   = szDstPath;
    MediaFileDesc[0].dwFlags      = COPY_IF_NEWER;

    if (dwType == SEGMENT)
    {
        MediaFileDesc[1].lpszFromPath = "DMusic/DMTest1/Perf8/PlaySeg/CTLooping/CTLooping.dls";
        MediaFileDesc[1].lpszToPath   = "T:\\DMTest1\\Perf8\\PlaySeg\\CtLooping\\";
        MediaFileDesc[1].dwFlags      = COPY_IF_NEWER;
    }

    CHECKRUN(LoadMedia(MediaFileDesc));
    CHECKRUN(Performance8_PlaySegmentEx_ClockTime_Looping(ptPerf8, szDstSeg, dwNumber[0] * 10000000, dwNumber[1] * 10000000, dwNumber[2]));
    return hr;
}
#undef SEGMENT
#undef WAVE


/********************************************************************************
/********************************************************************************/
#define MAXMEDIA 10

#define PMAX 4
#define CMAX 7
#define OS 0
#define ST 1
#define MU 2
#define MP 3


HRESULT Performance8_PlaySegmentEx_AudioPath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback, DWORD dwContentChannels)
{
    HRESULT hr = S_OK;
    MEDIAFILEDESC MediaFileDesc[MAXMEDIA] = {0};
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CHAR szSegmentLocal[MAX_PATH] = {0};
    LPCSTR szSegmentName = NULL;


    LPCSTR szPlayBack[CMAX][PMAX] =  {
         {NULL, NULL, NULL},
         {"WAV/PCM_OS_11_1.wav",        "WAV/PCM_ST_11_1.wav",          g_TestParams.szDefaultMedia, "SGT/MultiPan.sgt"},        //1 Channel
         {"WAV/PCM_OS_11_2.wav",        "WAV/PCM_ST_11_2.wav",          g_TestParams.szDefaultMedia, "SGT/MultiPan.sgt"},        //2 Channel
         {NULL, NULL, NULL},
         {"WAV/4Chan16Short.wav",       "WAV/4Chan16Long.wav",          g_TestParams.szDefaultMedia, "SGT/MultiPan.sgt"},        //4 Channel
         {NULL, NULL, NULL},
         {"WAV/FivePointOne16.wav",     "WAV/FivePointOne16Long.wav",   g_TestParams.szDefaultMedia, "SGT/MultiPan.sgt"}         //6 Channel
         };

    ASSERT(IsValidAudioPath(dwType));
    ASSERT(dwContentPlayback <= PMAX);
    ASSERT(dwContentChannels <= CMAX);

    szSegmentName = szPlayBack[dwContentChannels][dwContentPlayback];
    if (szSegmentName == g_TestParams.szDefaultMedia)
    {
        strcpy(szSegmentLocal, szSegmentName);
    }
    else
    {
        CHECKRUN(MediaCopyFile(szSegmentName));
        strcpy(szSegmentLocal, MediaServerToLocal(szSegmentName));
    }

    
    ASSERT(szSegmentLocal[0]);
    CHECK(   Log(FYILOGLEVEL, "Playing %s on %s", szSegmentName, dmthXlatDMUS_APATH(dwType)));
    CHECKRUN(dmthLoadSegment(szSegmentLocal, &ptSegment));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(dwType, 16, TRUE, &ptPath));
    CHECKRUN(InsertToolTrace(ptPath));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
    CHECKRUN(WaitForSegmentStop(ptPerf8, ptSegment, NULL, 25000));
    CHECK   (RemoveToolTrace(ptPath));
    RELEASE(ptSegment);
    RELEASE(ptPath);
    
    //Unconditional wait; necessary to prevent future assert from reused audiopath.
    Sleep(1000);  
    return hr;
}



HRESULT Performance8_PlaySegmentEx_AudioPath_1(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback)
{
    return Performance8_PlaySegmentEx_AudioPath(ptPerf8, dwType, dwContentPlayback, 1);
}

HRESULT Performance8_PlaySegmentEx_AudioPath_2(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback)
{
    return Performance8_PlaySegmentEx_AudioPath(ptPerf8, dwType, dwContentPlayback, 2);
}

HRESULT Performance8_PlaySegmentEx_AudioPath_4(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback)
{
    return Performance8_PlaySegmentEx_AudioPath(ptPerf8, dwType, dwContentPlayback, 4);
}

HRESULT Performance8_PlaySegmentEx_AudioPath_6(CtIDirectMusicPerformance8* ptPerf8, DWORD dwType, DWORD dwContentPlayback)
{
    return Performance8_PlaySegmentEx_AudioPath(ptPerf8, dwType, dwContentPlayback, 6);
}

#undef PMAX 
#undef CMAX 
#undef OS 
#undef ST 
#undef MU
#undef MP


/********************************************************************************
/********************************************************************************/
HRESULT Performance8_PlaySegmentEx_MunchPerf(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
char chLowGrooveLevel  = -9;
char chHighGrooveLevel = 99;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
REFERENCE_TIME rtStart = 0;
DWORD dwSeconds = 15;

CHECKRUN(ptPerf8->SetGlobalParam(GUID_PerfMasterGrooveLevel, &chHighGrooveLevel, sizeof(chLowGrooveLevel)));
CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Perf8/PlaySeg/MunchPerf/"));
CHECKRUN(Log(FYILOGLEVEL, "Silence for %d seconds", dwSeconds));
CHECKRUN(CountDown(dwSeconds * 1000, 1000));

CHECKRUN(dmthLoadSegment("T:\\DMTest1\\Perf8\\PlaySeg\\MunchPerf\\Munch06.sgt", &ptSegment));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 64, &ptPath));
CHECKRUN(ptPerf8->GetTime(&rtStart, NULL));
rtStart -= dwSeconds * 10000000;

//Play segment 5 seconds ago.
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, 0, 0, DMUS_SEGF_REFTIME | DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_TICK, rtStart, 0, 0, ptPath));

CHECKRUN(Log(FYILOGLEVEL, "Playing for 4 seconds at high groove level"));
CHECKRUN(Wait(4000));
CHECKRUN(ptPerf8->StopEx(0, 0, 0));
CHECKRUN(Log(FYILOGLEVEL, "Silence for 1 second"));
CHECKRUN(Wait(1000));
RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};


/********************************************************************************
/********************************************************************************/
HRESULT Performance8_PlaySegmentEx_AntzLooping(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;

CHECKRUN(MediaCopyFile("SGT/Antz2_melody_strings_a.sgt"));
CHECKRUN(dmthLoadSegment("T:\\SGT\\Antz2_melody_strings_a.sgt", &ptSegment));
CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 64, 64, &ptPath));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, 0, 0, 0, 0, 0, 0, ptPath));
CHECKRUN(Log(FYILOGLEVEL, "Playing for 10 seconds, should loop smoothly."));
CHECKRUN(Wait(10000));
CHECKRUN(ptPerf8->StopEx(0, 0, 0));
CHECKRUN(Log(FYILOGLEVEL, "Silence for 1 second"));
CHECKRUN(Wait(1000));
RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};

