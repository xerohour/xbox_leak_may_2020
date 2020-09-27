#include "globals.h"
#include "cicmusicx.h"

DWORD Performance8_PMsg_Generic(TESTPARAMS);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_SendPMsg (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function Performance8_SendPMsg())");

    CALLDMTEST(Performance8_PMsg_Generic(ptPerf8, 0, 0, 0, 0));

    return hr;
};




/********************************************************************************
********************************************************************************/
DWORD Performance8_PMsg_Generic(TESTPARAMS)
{
DWORD dwRes = FNS_FAIL;
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo = NULL;

    //Play some music whose tempo we're gonna change.
    hr = Music.Init(ptPerf8, g_szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB);
    if (S_OK != hr)
    {
        fnsLog(ABORTLOGLEVEL, "Music.Init %s failed with %s (%08Xh)", g_szDefaultMedia, tdmXlatHRESULT(hr), hr);
	    goto TEST_END;
    }

    fnsLog(FYILOGLEVEL, "Playing at default tempo for 2s.");
    Sleep(2000);
    //Create your PMsg.  I copied this from the sample
    if( SUCCEEDED( ptPerf8->AllocPMsg(
            sizeof(DMUS_TEMPO_PMSG),
            (DMUS_PMSG**)&pTempo )))
    {
        pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
        pTempo->dblTempo = 500;
        pTempo->rtTime = 0;
        pTempo->dwFlags = DMUS_PMSGF_REFTIME;
        pTempo->dwType = DMUS_PMSGT_TEMPO;
        if (FAILED(ptPerf8->SendPMsg( (DMUS_PMSG*)pTempo )))
        {
            ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo );
        }
    }


    fnsLog(FYILOGLEVEL, "Just sent a temp=500 pMsg, should notic a tempo change.  Playing at tempo=500 for 5s.");
    Sleep(5000);

    dwRes = FNS_PASS;

TEST_END:
    return dwRes;
};


 