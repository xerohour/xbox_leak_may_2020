#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_AllocPMsg_Many(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg_Big(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg_Generic(CtIDirectMusicPerformance8* ptPerf8);



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_AllocPMsg_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Performance8_AllocPMsg())");
    DMTEST_EXECUTE(Performance8_AllocPMsg_Generic(ptPerf8));  //PASS
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Performance8_AllocPMsg_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Performance8_AllocPMsg())");

    DMTEST_EXECUTE(Performance8_AllocPMsg_Many(ptPerf8));  //PASS
    DMTEST_EXECUTE(Performance8_AllocPMsg_Big(ptPerf8));   //PASS

    return hr;
};


/********************************************************************************
Note: Tested by itself, we can allocate 976521 PMsgs.  Let's just do 900000 for now.
********************************************************************************/
typedef DMUS_PMSG* LPDMUS_PMSG;
HRESULT Performance8_AllocPMsg_Many(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwMax = 800000; //Allocate dwMax PMsgs
    DWORD i = 0;
    LPDMUS_PMSG *ppMsg = NULL;
    
    
    ppMsg = new LPDMUS_PMSG[dwMax];
    CHECKALLOC(ppMsg);

    //Allocate the PMsgs.
    for (i=0; i<dwMax && SUCCEEDED(hr); i++)
    {
        if (i%100000 == 0)
            Log(FYILOGLEVEL, "Allocated %d pMsgs...", i);
        CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_PMSG), &ppMsg[i]));
    }

    //Free these PMsgs.
    for (i=0; i<dwMax && SUCCEEDED(hr); i++)
    {
        if (ppMsg[i])
        {
            ptPerf8->FreePMsg(ppMsg[i]);
            ppMsg[i] = NULL;
            if (i%100000 == 0)
                Log(FYILOGLEVEL, "Freed %d pMsgs...", i);

        }
    }

    delete []ppMsg;
    //SAFEDELETE(ppMsg);
    return hr;

};



/********************************************************************************
To the closest million, I could allocate a message of size 13000000.  14000000 failed.
********************************************************************************/
HRESULT Performance8_AllocPMsg_Big(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT     hr  = S_OK;
    DMUS_PMSG * pMsg= NULL;
    DWORD dwSize = 1000;

    //Allocate the PMsg
    CHECKRUN(ptPerf8->AllocPMsg(dwSize,  &pMsg));  //Allocate an appx 13 MB PMsg

    //Free these PMsgs.
    if (pMsg)
    {
        ptPerf8->FreePMsg(pMsg);
        pMsg = NULL;
    }

    return hr;

};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_AllocPMsg_Generic(CtIDirectMusicPerformance8* ptPerf8)
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
        hr = ptPerf8->SendPMsg( (DMUS_PMSG*)pTempo );
        if (FAILED(hr))
        {
            ptPerf8->FreePMsg( (DMUS_PMSG*)pTempo );
            pTempo = NULL;
            goto TEST_END;
        }


        Log(FYILOGLEVEL, "Just sent a temp=500 pMsg, should notice a tempo change.  Playing at tempo=500 for 5s.");
        Wait(5000);
    }

TEST_END:
    //No need to free a sent pMsg.
    return hr;
};


//lint -e10