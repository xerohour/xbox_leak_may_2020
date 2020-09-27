/********************************************************************************
FILE:

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"


DWORD tdmperfIsPlayingSame(TESTPARAMS);

/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_IsPlaying (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_IsPlaying())");

    CALLDMTEST(tdmperfIsPlayingSame(ptPerf8, 0, 0, 0, 0));                               

    return hr;
};



/********************************************************************************
********************************************************************************/
DWORD tdmperfIsPlayingSame(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CICMusic Music;
CtIDirectMusicSegmentState8 *ptSegmentState8 = NULL;
CtIDirectMusicSegment8 *ptSegment8 = NULL;


    //Load up and play our segment.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: Music.Init returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    //Get our segment and segment state.
    hr = Music.GetInterface(&ptSegment8);
    hr = Music.GetInterface(&ptSegmentState8);

    hr = ptPerf8->IsPlaying(ptSegment8, NULL);
    if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(ptSegment8, NULL) returned returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->IsPlaying(NULL, ptSegmentState8);
    if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(NULL, ptSegmentState8) returned returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->IsPlaying(ptSegment8, ptSegmentState8);
    if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(ptSegment8, ptSegmentState8) returned returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    fnsLog(ABORTLOGLEVEL, "Waiting 4 seconds, you should hear music playing");
    Sleep(4000);

    //Stop immediately.
    hr = ptPerf8->StopEx(ptSegment8, 0, 0);
    if (S_OK != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: returned returned %s (%08Xh) instead of S_OK",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->IsPlaying(ptSegment8, NULL);
    if (S_FALSE != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(ptSegment8, NULL) returned returned %s (%08Xh) instead of S_FALSE",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->IsPlaying(NULL, ptSegmentState8);
    if (S_FALSE != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(NULL, ptSegmentState8) returned returned %s (%08Xh) instead of S_FALSE",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    hr = ptPerf8->IsPlaying(ptSegment8, ptSegmentState8);
    if (S_FALSE != hr)
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: IsPlaying(ptSegment8, ptSegmentState8) returned returned %s (%08Xh) instead of S_FALSE",tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}

    Log(ABORTLOGLEVEL, "Waiting 4 seconds, you should hear no music because we just called StopEx(ptSegment8)");
    Sleep(4000);

    dwRes = FNS_PASS;


TEST_END:
    SAFE_RELEASE(ptSegmentState8);
    SAFE_RELEASE(ptSegment8);

    return dwRes;
};