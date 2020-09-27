/********************************************************************************
	FILE:
		Loader_CollectGarbage.cpp

	PURPOSE:
		CollectGarbage Tests for Loader

	BY:
		DANROSE
********************************************************************************/
#include "globals.h"


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_ClearCache_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Loader8_ClearCache_BVT())");
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Loader_ClearCache_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Loader8_ClearCache())");
    return hr;
};
