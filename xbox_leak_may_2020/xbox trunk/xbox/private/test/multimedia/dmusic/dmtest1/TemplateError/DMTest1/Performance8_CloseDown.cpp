/********************************************************************************
FILE:
    Performance8_CloseDown.cpp

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_CloseDown(TESTPARAMS);

/********************************************************************************
Main test function.
********************************************************************************/
/*
HRESULT Performance8_CloseDown (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_CloseDown())");

    CALLDMTEST(Performance8_CloseDown(ptPerf8, 0, 0, 0, 0));

    return hr;
};
*/
/********************************************************************************
********************************************************************************/
/*
DWORD Performance8_CloseDown(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;

    //Attempt to closedown the global performance.
    hr = ptPerf8->CloseDown();
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "**** ABORT: Closedown returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    //Start it up again.  This is critical!!
    hr = ptPerf8->InitAudioX(NULL, NULL);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "**** ABORT: InitAudio returned (%s == %08Xh)",tdmXlatHRESULT(hr), hr);
        Log(ABORTLOGLEVEL, "**** ABORT: Testing will NOT be able to continue!!!!");
        goto TEST_END;
    }


    dwRes = FNS_PASS;

TEST_END:
    return dwRes;
};
*/