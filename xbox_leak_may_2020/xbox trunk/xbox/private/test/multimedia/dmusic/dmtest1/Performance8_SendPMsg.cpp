#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_PMsg_Generic(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_SendPMsg_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_SendPMsg_BVT())");
    DMTEST_EXECUTE(Performance8_PMsg_Generic(ptPerf8));   //FAIL: 2968
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_PMsg_Generic(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo = NULL;

    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    Log(FYILOGLEVEL, "Playing at default tempo for 2s.");
    Wait(2000);

    //Create your PMsg.  I copied this from the sample
    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_TEMPO_PMSG),(DMUS_PMSG**)&pTempo ));
    CHECKALLOC(pTempo);

    if (SUCCEEDED(hr))
    {
        pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
        pTempo->dblTempo = 500;
        pTempo->rtTime = 0;
        pTempo->dwFlags = DMUS_PMSGF_REFTIME;
        pTempo->dwType = DMUS_PMSGT_TEMPO;
        pTempo->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
        hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTempo );
        if (FAILED(hr))
        {
            ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo );
            pTempo = NULL;
            goto TEST_END;
        }
    }


    CHECKRUN(Log(FYILOGLEVEL, "Just sent a tempo=500 pMsg, should notice a tempo change.  Playing at tempo=500 for 5s."));
    CHECKRUN(Wait(5000));

TEST_END:
    //No need to free a sent pMsg.
    return hr;
};


 

/********************************************************************************
********************************************************************************/
HRESULT Performance8_PMsg_Valid_Tempos(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo = NULL;
double dblTempos[] = {0, 1, 120, 1000, 2000};
DWORD i = 0;

    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    Log(FYILOGLEVEL, "Playing at default tempo for 2s.");
    Wait(2000);


    for (i=0; i<AMOUNT(dblTempos); i++)
    {
        //Create your PMsg.  I copied this from the sample
        CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_TEMPO_PMSG),(DMUS_PMSG**)&pTempo ));
        CHECKALLOC(pTempo);

        if (SUCCEEDED(hr))
        {
            pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
            pTempo->dblTempo = dblTempos[i];
            pTempo->rtTime = 0;
            pTempo->dwFlags = DMUS_PMSGF_REFTIME;
            pTempo->dwType = DMUS_PMSGT_TEMPO;
            pTempo->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
            hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTempo );
            if (FAILED(hr))
            {
                ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo );
                pTempo = NULL;
                goto TEST_END;
            }
        }

        CHECKRUN(Log(FYILOGLEVEL, "Just sent a tempo=%g pMsg, should notice a tempo change.  Playing for 4s.", dblTempos[i]));
        CHECKRUN(Wait(4000));

    }



TEST_END:
    //No need to free a sent pMsg.
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_PMsg_Valid_Transpose(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TRANSPOSE_PMSG* pTranspose = NULL;
LPSTR szSegmentName = "T:\\DMTest1\\Perf8\\SendPMsg\\OnePitch.sgt";
int nTranspose = 0;
BOOL bDone = FALSE;

    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DMusic/DMTest1/Perf8/SendPMsg/OnePitch.sgt",  "T:\\DMTest1\\Perf8\\SendPMsg\\", COPY_IF_NEWER},
    {"DLS/Main1.DLS",               "T:\\DMTest1\\Perf8\\SendPMsg\\", COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, szSegmentName, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    Log(FYILOGLEVEL, "Playing C-5 note at default key for 2s.");
    Wait(2000);


    Log(FYILOGLEVEL, "Transposing playback from -128 to 127...");
    for (nTranspose=-128; nTranspose<=127 ; nTranspose++)
    {
        
        //Create your PMsg.  I copied this from the sample
        CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG),(DMUS_PMSG**)&pTranspose ));
        CHECKALLOC(pTranspose)

        if (SUCCEEDED(hr))
        {
            pTranspose->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
            pTranspose->rtTime = 0;
            pTranspose->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_DX8;
            pTranspose->dwType = DMUS_PMSGT_TRANSPOSE;
            pTranspose->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
            pTranspose->nTranspose = (short)nTranspose;
            pTranspose->wMergeIndex = 1;
            hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTranspose );
            if (FAILED(hr))
            {
                ptPerf8->FreePMsg( (DMUS_PMSG*)pTranspose );
                pTranspose = NULL;
                goto TEST_END;
            }


            if (nTranspose % 32 == 0)
                Log(FYILOGLEVEL, "Tranpose = %d", nTranspose);
            Wait(80);
        }
    }



TEST_END:

        
        //Set the transpose back to zero.
        ptPerf8->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG),(DMUS_PMSG**)&pTranspose );
        if (pTranspose)
        {
            pTranspose->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
            pTranspose->rtTime = 0;
            pTranspose->dwFlags = DMUS_PMSGF_REFTIME;
            pTranspose->dwType = DMUS_PMSGT_TRANSPOSE;
            pTranspose->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
            pTranspose->nTranspose = 0;
            hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTranspose );
        }



    //No need to free a sent pMsg.
    return hr;
};

/********************************************************************************
********************************************************************************/
HRESULT Performance8_PMsg_Valid_Transpose_ChangeSong(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TRANSPOSE_PMSG* pTranspose  = NULL;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
LPSTR szSegmentName = "T:\\DMTest1\\Perf8\\SendPMsg\\OnePitch.sgt";
int nTranspose = 0;
BOOL bDone = FALSE;

    //Load media.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DMusic/DMTest1/Perf8/SendPMsg/OnePitch.sgt",  "T:\\DMTest1\\Perf8\\SendPMsg\\", COPY_IF_NEWER},
    {"DLS/Main1.DLS",               "T:\\DMTest1\\Perf8\\SendPMsg\\", COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));


    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, szSegmentName, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Music.GetInterface(&ptPath));
    Log(FYILOGLEVEL, "Playing C-5 note at default key for 2s.");
    Wait(2000);

    nTranspose = 24;
        
    //Create your PMsg.  I copied this from the sample
    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG),(DMUS_PMSG**)&pTranspose ));
    CHECKALLOC(pTranspose)

    if (SUCCEEDED(hr))
    {
        pTranspose->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
        pTranspose->rtTime = 0;
        pTranspose->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_DX8;
        pTranspose->dwType = DMUS_PMSGT_TRANSPOSE;
        pTranspose->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
        pTranspose->nTranspose = (short)nTranspose;
        pTranspose->wMergeIndex = 1;
        hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTranspose );
        if (FAILED(hr))
        {
            ptPerf8->FreePMsg( (DMUS_PMSG*)pTranspose );
            pTranspose = NULL;
            goto TEST_END;
        }
    }

    Log(FYILOGLEVEL, "Transposing playback up two octaves for 4 seconds.");
    CHECKRUN(Wait(4000));


    CHECKRUN(Log(FYILOGLEVEL, "Restarting the song; Playing 4 seconds; Transpose level should remain at 2 octaves"));
    CHECKRUN(ptPerf8->StopEx(ptSegment, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, NULL, 0, NULL, NULL, ptPath));
    CHECKRUN(Wait(4000));

TEST_END:
        
        //Set the transpose back to zero.
        ptPerf8->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG),(DMUS_PMSG**)&pTranspose );
        if (pTranspose)
        {
            HRESULT hrResetTranspose;
            pTranspose->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
            pTranspose->rtTime = 0;
            pTranspose->dwFlags = DMUS_PMSGF_REFTIME;
            pTranspose->dwType = DMUS_PMSGT_TRANSPOSE;
            pTranspose->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
            pTranspose->nTranspose = 0;
            hrResetTranspose = ptPerf8->SendPMsg( (DMUS_PMSG*)pTranspose );
            if (FAILED(hrResetTranspose))
            {
                Log(ABORTLOGLEVEL, "ERROR: Resetting the transpose level failed because SendPMsg returned %s (%08X)", tdmXlatHRESULT(hr), hr);
                if (SUCCEEDED(hr))
                    hr = hrResetTranspose;
            }
        }



    //No need to free a sent pMsg.
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptPath);
    return hr;
};










/********************************************************************************
********************************************************************************/
HRESULT Performance8_PMsg_Valid_PitchBend(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_CURVE_PMSG* pCurve= NULL;
DWORD dwEndValue[] = {4000};
DWORD i = 0;

    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    Log(FYILOGLEVEL, "Playing at default for one measure.");


    for (i=0; i<AMOUNT(dwEndValue); i++)
    {
        //Create your PMsg.  I copied this from the sample
        CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG**)&pCurve ));
        CHECKALLOC(pCurve);

        if (SUCCEEDED(hr))
        {
            pCurve->dwSize = sizeof(DMUS_CURVE_PMSG);


        pCurve->mtDuration = DMUS_PPQ * 4;      /* how long this curve lasts */
        pCurve->mtOriginalStart = 0; /* must be set to either zero when this PMSG is created or to the original mtTime of the curve */
        pCurve->mtResetDuration = 0; /* how long after the curve is finished to allow a flush or
                                                        invalidation to reset to the reset value, nResetValue */
        pCurve->nStartValue = 0;      /* curve's start value */
        pCurve->nEndValue = (short)dwEndValue[i];    /* curve's end value */
        pCurve->nResetValue = 0;     /* curve's reset value, set when a flush or invalidation
                                                    occurs within mtDuration + mtResetDuration */
        pCurve->wMeasure = 1;        /* Measure in which this curve occurs */
        pCurve->nOffset = 0;         /* Offset from grid at which this curve occurs */
        pCurve->bBeat = 0;           /* Beat (in measure) at which this curve occurs */
        pCurve->bGrid = 0;           /* Grid offset from beat at which this curve occurs */
        pCurve->bType = DMUS_CURVET_PBCURVE ;           /* type of curve */
        pCurve->bCurveShape = DMUS_CURVES_LINEAR ;     /* shape of curve */
        pCurve->bCCData = 0;         /* CC# if this is a control change type */
        pCurve->bFlags = DMUS_CURVE_RESET ;          /* Curve reset and start from current value flags. */
        pCurve->wParamType = 0;      /* RPN or NRPN parameter number. */
        pCurve->wMergeIndex = 0;     /* Allows multiple parameters to be merged (pitchbend, volume, and expression.)*/



        hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pCurve );
        if (FAILED(hr))
        {
            ptPerf8->FreePMsg( (DMUS_PMSG*)pCurve );
            pCurve = NULL;
            goto TEST_END;
        }
    }

    CHECKRUN(Log(FYILOGLEVEL, "Just sent a %s curve.", "PitchBend"));
    CHECKRUN(Wait(8000));

    }



TEST_END:
    //No need to free a sent pMsg.
    return hr;
};



/**********************************************************************
Crossfade smoothly from one segment to another (they can be DLS or Wave
segments)
**********************************************************************/
HRESULT Performance8_PMsg_Crossfade(CtIDirectMusicPerformance *ptPerf8, DWORD dwSegmentServer1, DWORD dwSegmentServer2)
{
HRESULT hr = S_OK;
DWORD i = 0;
CHAR  szSegment[2][MAX_PATH]    = {0};
LPSTR szSegmentServer[2]        = {NULL};
CtIDirectMusicAudioPath *ptPath[2] = {NULL};
CtIDirectMusicSegment *ptSegment[2] = {NULL};
IDirectMusicGraph *pGraph[2] = {NULL};
CtIDirectMusicSegmentState *ptSegmentState[2] = {NULL};
szSegmentServer[0] = (LPSTR)dwSegmentServer1;
szSegmentServer[1] = (LPSTR)dwSegmentServer2;
DMUS_CURVE_PMSG *pCurve[2] = {NULL};

//Copy the media, and everything else in its directory.
for (i=0; i<2; i++)
{
    CHAR szDirectory[MAX_PATH] = {0};
    strcpy(szSegment[i], MediaServerToLocal(szSegmentServer[i]));
    CHECKRUN(MediaCopyDirectory(szSegmentServer[i]));

    //Get the directory of that file.
    CHECKRUN(ChopPath(szSegment[i], szDirectory, NULL));
    //Put MAIN1.DLS in that directory too, just in case.
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS", szDirectory,    COPY_IF_NEWER},
    {TERMINATE}
    };
    
    CHECKRUN(LoadMedia(MediaFileDesc));

}




//How long to play the first segment before starting the fade.
const DWORD dwWaitDuration              = 2000;  

//How long the fade should last.
const DWORD dwFadeoutDuration           = 2000;  

//How long the fade should last in reftime.
const LONGLONG llWaitDuration           = LONGLONG(dwWaitDuration) * LONGLONG(10000); 

//How soon to start the curve before the actual segment starts, so it doesn't start
//  playing at full volume slightly before it starts it's fade-in curve from 0-127.
//  200ms.
const LONGLONG llPreRollDuration       = LONGLONG(10000 * 200);   
REFERENCE_TIME rt                       = 0;

//Create segments on their respective audiopaths.
//Segments must be on separate audiopaths or crossfades will interfere with each other.
for (i=0; i<2; i++)
{
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREO, 128, TRUE, &ptPath[i]));
    CHECKRUN(dmthLoadSegment(szSegment[i], &ptSegment[i]));
}

//Play segment 1 immediately.  We play it as secondary because if it's primary we hit problems
//  when we play the next segment (this one pauses for some reason).
//  We must set use this segment's tempo though, so we specify DMUS_SEGF_CONTROL too.
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[0], NULL, NULL, DMUS_SEGF_REFTIME | DMUS_SEGF_SECONDARY | DMUS_SEGF_CONTROL,0,&ptSegmentState[0], NULL, ptPath[0]));

//After the specified wait time, queue the next segment slightly in the future, and start the crossfade curves immediately.
//  If we don't queue the next segment in the future, it will play at full volume for a short time before the fade-in curve
//  turns it down.  Being a secondary segment, it will pick up the previous segment's tempo which is fine.
Wait(dwWaitDuration);
CHECKRUN(ptPerf8->GetTime(&rt, NULL));
CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment[1], NULL, NULL, DMUS_SEGF_REFTIME | DMUS_SEGF_SECONDARY, rt + llPreRollDuration,&ptSegmentState[1], NULL, ptPath[1]));

//Create volume curve pMsgs for both segments.  They are exactly the same except
//the first segment fades out slowly and the second segment fades in quickly.
for (i=0; i<2; i++)
{
    //Obtain the tool graph for our segment state.
    CHECKRUN(ptSegmentState[i]->QueryInterface(IID_IDirectMusicGraph, (LPVOID *)&pGraph[i]));

    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG**)&pCurve[i] ));
    CHECKALLOC(pCurve[i])
    if (SUCCEEDED(hr))
    {
        pCurve[i]->dwSize = sizeof(DMUS_CURVE_PMSG);
        
        //Send this message immediately.
        pCurve[i]->rtTime = 0;  

        //Unused since we specified DMUS_PMSGF_REFTIME
        pCurve[i]->mtTime = 0;
        
        
        //Use the rtTime member instead of mtTime.
        pCurve[i]->dwFlags |= DMUS_PMSGF_REFTIME;

        //mtDuration is in milliseconds, not MUSIC_TIME.
        pCurve[i]->dwFlags |= DMUS_PMSGF_LOCKTOREFTIME;

        //"Message should be processed immediately, regardless of its time stamp."
        //This is redundant with specifying 0 for the time.
        pCurve[i]->dwFlags |= DMUS_PMSGF_TOOL_IMMEDIATE;

        //If this flag is not specified, wMergeIndex field will be ignored.
        pCurve[i]->dwFlags |= DMUS_PMSGF_DX8;               

        //Send to all pChannels on this segment.
        pCurve[i]->dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
        pCurve[i]->dwVirtualTrackID = 0;
        pCurve[i]->pTool= NULL;
        pCurve[i]->pGraph = NULL;
        pCurve[i]->dwType = DMUS_PMSGT_CURVE;
        pCurve[i]->dwVoiceID = 0;
        pCurve[i]->dwGroupID = 0xFFFFFFF;
        pCurve[i]->punkUser = NULL;

        //How long this curve lasts in milliseconds (since we specified DMUS_PMSGF_LOCKTOREFTIME)
        pCurve[i]->mtDuration = dwFadeoutDuration;

        //Must be set to either zero when this PMSG is created or to the original mtTime of the curve 
        pCurve[i]->mtOriginalStart = 0; 

        //How long after the curve is finished to allow a flush or invalidation to reset to the reset value, nResetValue 
        pCurve[i]->mtResetDuration = 0; 
                                                        
        //Fade high to low if, or low to high if fading in.
        pCurve[i]->nStartValue = (i==0) ? 127 :    0;
        pCurve[i]->nEndValue   = (i==0) ? 0   :  127;

        //Curve's reset value, set when a flush or invalidation
        pCurve[i]->nResetValue = 0;     

        //These 4 fields don't have any effect if we've specified DMUS_PMSGF_LOCKTOREFTIME
        pCurve[i]->wMeasure = 0;        // Measure in which this curve occurs 
        pCurve[i]->nOffset = 0;         // Offset from grid at which this curve occurs 
        pCurve[i]->bBeat = 0;           // Beat (in measure) at which this curve occurs 
        pCurve[i]->bGrid = 0;           // Grid offset from beat at which this curve occurs 


        //Volume curves are CC curves of type 7.
        pCurve[i]->bType = DMUS_CURVET_CCCURVE ;                              
        pCurve[i]->bCCData = 7;                                              

        //Fade out slowly and fade in quickly to preserve overall volume.
        pCurve[i]->bCurveShape = (i==0) ? DMUS_CURVES_EXP: DMUS_CURVES_LOG;   // shape of curve 

        //Blow away any current curves.
        pCurve[i]->bFlags = DMUS_CURVE_RESET;

        //RPN or NRPN parameter number (irrelevant here)
        pCurve[i]->wParamType = 0;      
        
        //An arbitrary value.  Choose one other than zero, and other than those used by curves in your content, if any.
        pCurve[i]->wMergeIndex = 55;
        
    }

    //Tell this pMsg to go to the correct segment's toolgraph.
    CHECKRUN(pGraph[i]->StampPMsg((DMUS_PMSG *)pCurve[i]));
    CHECKRUN(ptPerf8->SendPMsg( (DMUS_PMSG*)pCurve[i] ));
    if (FAILED(hr))
    {
        if (ptPerf8 && pCurve[i])
            ptPerf8->FreePMsg( (DMUS_PMSG*)pCurve[i]);
        pCurve[i] = NULL;
    }
}

//Wait the appripriate time for the fadeout to start, finish, plus 5 seconds.  Then stop everything.
CHECKRUN(Wait(dwFadeoutDuration + 5000));
CHECKRUN(ptPerf8->StopEx(0, 0, 0));

//Release the good stuff.
for (i=0; i<2; i++)
{    
    RELEASE(ptPath[i]);
    RELEASE(ptSegment[i]);
    RELEASE(pGraph[i]);
    RELEASE(ptSegmentState[i]);
}

return hr;
}
