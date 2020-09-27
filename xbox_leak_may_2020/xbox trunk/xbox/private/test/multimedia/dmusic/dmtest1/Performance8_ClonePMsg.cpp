#include "globals.h"
#include "cicmusicx.h"
#include "help_comcounter.h"


HRESULT Performance8_ClonePMsg_BVT1(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_ClonePMsg_AddRefpUnk(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_ClonePMsg_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_ClonePMsg())");             //PASS!!
    DMTEST_EXECUTE(Performance8_ClonePMsg_BVT1(ptPerf8));
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_ClonePMsg_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Performance8_ClonePMsg())");           //PASS
    DMTEST_EXECUTE(Performance8_ClonePMsg_AddRefpUnk(ptPerf8));
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_ClonePMsg_BVT1(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo  = NULL;
DMUS_TEMPO_PMSG* pCloned = NULL;

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
    }

    //Clone it.
    CHECKRUN(ptPerf8->ClonePMsg((DMUS_PMSG*)pTempo, (DMUS_PMSG**)&pCloned));
    CHECKALLOC(pCloned);
    if (FAILED(hr))
        goto TEST_END;

    //Make sure it's exactly the same.
    if (memcmp(pTempo, pCloned, sizeof(DMUS_TEMPO_PMSG)) != 0)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "Cloned PMsg is different than original");
        goto TEST_END;
    }

    //Send the PMsg.
    CHECKRUN(ptPerf8->SendPMsg( (DMUS_PMSG*)pCloned));

    Log(FYILOGLEVEL, "Just sent a tempo=500 pMsg, should notic a tempo change.  Playing at tempo=500 for 5s.");
    Wait(5000);

TEST_END:
    //Free the unsent one yerself.
    if (pTempo)
        ptPerf8->FreePMsg((DMUS_PMSG*)pTempo);

    return hr;
};





/********************************************************************************
Verify that ClonePMsg addrefs the punkUser.
********************************************************************************/
HRESULT Performance8_ClonePMsg_AddRefpUnk(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo = NULL;
DMUS_TEMPO_PMSG* pCloned = NULL;
CComCounter *pUnkComCounter = NULL;

    //Create our virtual COM object.
    CHECKRUN(CreateComCounter(&pUnkComCounter));

    //Play some music whose tempo we're gonna change.
    CHECKRUN(Music.Init(ptPerf8, g_TestParams.szDefaultMedia, DMUS_APATH_SHARED_STEREOPLUSREVERB));

    Log(FYILOGLEVEL, "Playing at default tempo for 2s.");
    Wait(2000);

    //Create your PMsg.  I copied this from the sample
    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_TEMPO_PMSG),(DMUS_PMSG**)&pTempo ));
    if (pTempo)
    {
        //Set up some typical data.
        pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
        pTempo->dblTempo = 500;
        pTempo->rtTime = 0;
        pTempo->dwFlags = DMUS_PMSGF_REFTIME;
        pTempo->dwType = DMUS_PMSGT_TEMPO;
        pTempo->punkUser = (IUnknown *)pUnkComCounter;
    }

    //Clone it.
    CHECKRUN(ptPerf8->ClonePMsg((DMUS_PMSG*)pTempo, (DMUS_PMSG**)&pCloned));


    //Make sure our COM object was addref'd.
    if (pUnkComCounter->m_dwCalledRelease != 0)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "COM object's Release was called %d instead %d times", pUnkComCounter->m_dwCalledRelease, 1);
        goto TEST_END;
    }

    if (pUnkComCounter->m_dwCalledAddRef != 1)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "COM object's AddRef was called %d instead %d times", pUnkComCounter->m_dwCalledAddRef, 1);
        goto TEST_END;
    }

    if (pUnkComCounter->m_dwCalledQueryInterface != 0)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "COM object's QueryInterface was called %d instead %d times", pUnkComCounter->m_dwCalledQueryInterface, 1);
        goto TEST_END;
    }

//    if (
    CHECKRUN(ptPerf8->FreePMsg((DMUS_PMSG *)pTempo));
    CHECKRUN(ptPerf8->FreePMsg((DMUS_PMSG *)pCloned));

TEST_END:
    pUnkComCounter->ActualRelease();
    return hr;
};

