/********************************************************************************
FILE:
    Performance8_Release.cpp

PURPOSE:

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "cicmusicx.h"

HRESULT Performance8_Release_Feb(CtIDirectMusicPerformance8* ptPerf8);

/********************************************************************************
Main test function.
********************************************************************************/
HRESULT Performance8_Release (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function Performance8_Release())");

    hr = Performance8_Release_Feb(ptPerf8);

    return hr;
};


/********************************************************************************
THIS SHOUDLD ASSERT!!!!
********************************************************************************/
HRESULT Performance8_Release_Feb(CtIDirectMusicPerformance8* ptPerf8)
{
    //Attempt to Release the global performance.  This should assert for February!!
    ptPerf8->Release();
    return S_OK;
};
