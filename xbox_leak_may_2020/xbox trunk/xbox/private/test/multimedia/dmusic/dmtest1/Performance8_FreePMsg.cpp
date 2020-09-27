#include "globals.h"
#include "cicmusicx.h"
#include "help_comcounter.h"

HRESULT Performance8_FreePMsg_BVT1(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_FreePMsg_ReleasepUnk(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_FreePMsg_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_FreePMsg())");
    DMTEST_EXECUTE(Performance8_FreePMsg_BVT1(ptPerf8));
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_FreePMsg_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Valid test function Performance8_FreePMsg())");
    DMTEST_EXECUTE(Performance8_FreePMsg_ReleasepUnk(ptPerf8));
    return hr;
};





/********************************************************************************
Allocate and free a PMsg.
********************************************************************************/
HRESULT Performance8_FreePMsg_BVT1(CtIDirectMusicPerformance8* ptPerf8)
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
    if (pTempo)
    {
        //Set up some typical data.
        pTempo->dwSize = sizeof(DMUS_TEMPO_PMSG);
        pTempo->dblTempo = 500;
        pTempo->rtTime = 0;
        pTempo->dwFlags = DMUS_PMSGF_REFTIME;
        pTempo->dwType = DMUS_PMSGT_TEMPO;
        CHECKRUN(ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo ));
    }

    return hr;
};



/********************************************************************************
Make sure that the pUnknown associated with a pMsg is freed also.
********************************************************************************/
HRESULT Performance8_FreePMsg_ReleasepUnk(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CICMusic Music;
DMUS_TEMPO_PMSG* pTempo = NULL;
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
        CHECKRUN(ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo ));
    }

    //Make sure our COM object wasn't abused.
    if (pUnkComCounter->m_dwCalledRelease != 1)
    {
        hr = E_FAIL;
        Log(ABORTLOGLEVEL, "COM object's Release was called %d instead %d times", pUnkComCounter->m_dwCalledRelease, 1);
        goto TEST_END;
    }

    if (pUnkComCounter->m_dwCalledAddRef != 0)
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


TEST_END:
    pUnkComCounter->ActualRelease();
    return hr;
};





