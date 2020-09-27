#include "globals.h"
#include "CICMusicX.h"

DWORD Segment8_SetRepeats_BVT(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Segment8_SetRepeats (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Segment8_SetRepeats())");

    CALLDMTEST(Segment8_SetRepeats_BVT    (ptPerf8, 0, 0, 0, 0));

    return hr;
};


/********************************************************************************
********************************************************************************/
DWORD Segment8_SetRepeats_BVT(TESTPARAMS)
{
    DWORD dwRes = FNS_FAIL;
    HRESULT hr = S_OK;
    CICMusic Music;
    CtIDirectMusicSegment8 *ptSegment8 = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    DWORD dwRepeatValues[] = {0, 1, 2, 5, DMUS_SEG_REPEAT_INFINITE };
    DWORD i=0;


    //Load up and play our segment.  It's really short so we can count the number of times it plays.
    hr = Music.Init(ptPerf8, "T:\\Media\\Segment8\\SetRep\\ShortSeg.sgt", DMUS_APATH_SHARED_STEREOPLUSREVERB);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Music.Init returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    hr = Music.GetInterface(&ptSegment8);
    hr = Music.GetInterface(&ptPath);

    //Try all these repeat values.
    for (i=0; i<AMOUNT(dwRepeatValues); i++)
    {

        //Stop everything and wait a sec.
        hr = ptPerf8->StopEx(0, 0, 0);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: StopEx returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }
        Sleep(1000);


        //Set Repeats
        fnsLog(FYILOGLEVEL, "Calling SetRepeats(%u)",dwRepeatValues[i]);
        hr = ptSegment8->SetRepeats(dwRepeatValues[i]);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: SetRepeats(%u) returned (%s == %08Xh)",dwRepeatValues[i], tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }

         //Play
         hr = ptPerf8->PlaySegmentEx( 
		            ptSegment8, 
		            0,
		            NULL,
                    0, 
		            0,
		            NULL,
		            NULL,
		            ptPath);
	    if(FAILED(hr))
	    {
            fnsLog(ABORTLOGLEVEL, "**** ABORT: PlaySegmentEx() returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
		    goto TEST_END;
	    }

        //Wait for segment to start.
        while (ptPerf8->IsPlaying(ptSegment8, NULL) == S_FALSE);


        //Wait until done
        if (dwRepeatValues[i] != DMUS_SEG_REPEAT_INFINITE)
        {
            fnsLog(FYILOGLEVEL, "Waiting until segment stops");
            while (ptPerf8->IsPlaying(ptSegment8, NULL) == S_OK);
        }
        else
        {
            fnsLog(FYILOGLEVEL, "Waiting 5 seconds.");
            Sleep(5000);
        }
    }

    dwRes = FNS_PASS;

TEST_END:

    SAFE_RELEASE(ptSegment8);
    return dwRes;

}