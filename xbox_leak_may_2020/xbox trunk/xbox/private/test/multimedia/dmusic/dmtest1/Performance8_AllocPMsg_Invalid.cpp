#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_AllocPMsg_Invalid_SizeZero(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg_Invalid_SizeSmall(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg_Invalid_SizeBig(CtIDirectMusicPerformance8* ptPerf8);
HRESULT Performance8_AllocPMsg_Invalid_NULLppMsg(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_AllocPMsg_Invalid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_AllocPMsg_Invalid())");
    
    Performance8_AllocPMsg_Invalid_SizeZero(ptPerf8);  
    Performance8_AllocPMsg_Invalid_SizeSmall(ptPerf8); 
    Performance8_AllocPMsg_Invalid_SizeBig(ptPerf8);   
    Performance8_AllocPMsg_Invalid_NULLppMsg(ptPerf8); 


    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_AllocPMsg_Invalid_SizeZero(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DMUS_PMSG *pMsg = NULL;
    CHECKRUN(ptPerf8->AllocPMsg(0, &pMsg));
    return hr;

};


/********************************************************************************
********************************************************************************/
HRESULT Performance8_AllocPMsg_Invalid_SizeSmall(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DMUS_PMSG *pMsg = NULL;
    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_PMSG) - 1, &pMsg));
    return hr;

};




/********************************************************************************
********************************************************************************/
HRESULT Performance8_AllocPMsg_Invalid_SizeBig(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DMUS_PMSG *pMsg = NULL;
    CHECKRUN(ptPerf8->AllocPMsg(0xFFFFFFFF, &pMsg));
    return hr;

};



/********************************************************************************
********************************************************************************/
HRESULT Performance8_AllocPMsg_Invalid_NULLppMsg(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    CHECKRUN(ptPerf8->AllocPMsg(sizeof(DMUS_PMSG), NULL));
    Log(FYILOGLEVEL, "Function correctly asserted!!");
    return hr;

};

