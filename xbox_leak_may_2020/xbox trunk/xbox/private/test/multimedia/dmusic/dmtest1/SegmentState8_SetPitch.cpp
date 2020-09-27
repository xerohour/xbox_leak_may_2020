#include "globals.h"
#include "cicmusicx.h"

/********************************************************************************
FILE:
    SegmentState_SetPitch.cpp

PURPOSE:
    Contains SegmentState test functions.

BY:
    DANHAFF
********************************************************************************/
#include "globals.h"
#include "AudioPath_SetPitch.hpp"


#define INTERFACE_NAME CtIDirectMusicSegmentState

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT SegmentState_SetPitch_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function SegmentState_SetPitch()");
    DMTEST_EXECUTE(tmplSetPitch_BVT_Direct<INTERFACE_NAME>(ptPerf8, 0, 0));
    return hr;
};



/********************************************************************************
Valid test function.
*******************************************************************************/
HRESULT SegmentState_SetPitch_Valid_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwRange)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function SegmentState_SetPitch())");
    DMTEST_EXECUTE(tmplSetPitch_Ramp<INTERFACE_NAME>(ptPerf8, dwTime, dwRange));
    return hr;
};

