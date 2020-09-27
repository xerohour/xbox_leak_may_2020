#include "globals.h"
#include "cicmusicx.h"



DWORD Performance8_InitAudio_Params(TESTPARAMS);
DWORD Performance8_InitAudio_Invalid_Feb1(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_InitAudio (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_InitAudio())");

    CALLDMTEST(Performance8_InitAudio_Params       (ptPerf8, 0, 0, 0, 0));                               

    return hr;
};




/********************************************************************************
********************************************************************************/
DWORD Performance8_InitAudio_Params(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CtIDirectMusicSegment8  *ptSegment8 = NULL;

DWORD dwDefaultPath             = dwParam1;
DWORD dwPChannels               = dwParam2;
DWORD dwFlags                   = dwParam3;
//DMUS_AUDIOPARAMS *pAudioParams  = (DMUS_AUDIOPARAMS *)dwParam4;
CHAR *szSegment               = (CHAR *)dwParam4;

    
    if (dwFlags != 0)
    {
        Log(ABORTLOGLEVEL, "**** ABORT: TEST APP ERROR, SOMEBODY IS PASSING %d for dwFlags, it should be zero!!", dwFlags);
        goto TEST_END;
    }
    
    //Start up the performance.
    hr = ptPerf8->InitAudioX(NULL, NULL);//pAudioParams);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "**** ABORT: InitAudio(NULLs) returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    //Play your content!
	hr = dmthPlayMidiFile(ptPerf8,CTIID_IDirectMusicSegment,&ptSegment8, NULL, szSegment);
	if(FAILED(hr))
	{
        fnsLog(ABORTLOGLEVEL, "**** ABORT: PlayMidiFile convienence function failed (%s == %08Xh)", tdmXlatHRESULT(hr), hr);
		goto TEST_END;
	}
    
    fnsLog(FYILOGLEVEL, "Should hear %s playing for 3 seconds.", szSegment);
    Sleep(3000);


    dwRes = FNS_PASS;

TEST_END:
    if (ptSegment8)
    {
        ptSegment8->Unload(ptPerf8);
        SAFE_RELEASE(ptSegment8);
    }
    return dwRes;
};


