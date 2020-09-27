#include "globals.h"
#include "cicmusicx.h"

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_ReleaseObjectByUnknown_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Loader_ReleaseObjectByUnknown())");
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Loader_ReleaseObjectByUnknown_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Loader_ReleaseObjectByUnknown())");
    return hr;
};

