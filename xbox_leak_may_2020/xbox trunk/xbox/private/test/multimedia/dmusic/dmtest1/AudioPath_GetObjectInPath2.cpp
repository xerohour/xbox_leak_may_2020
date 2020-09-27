#include "globals.h"
#include "cicmusicx.h"
#include "AudioPath_GetObjectInPath2.hpp"


HRESULT AudioPath_GetObjectInPath_TraverseStandard(CtIDirectMusicPerformance8 *ptPerf8, DWORD dwStandardPath, PATH_DEFINITION pDef);


/********************************************************************************
Main test function.
********************************************************************************/
HRESULT AudioPath_GetObjectInPath (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function AudioPath_GetObjectInPath())");
    DMTEST_EXECUTE(AudioPath_GetObjectInPath_TraverseStandard(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB,  g_DefaultPathStereoPlusReverb));
    DMTEST_EXECUTE(AudioPath_GetObjectInPath_TraverseStandard(ptPerf8, DMUS_APATH_SHARED_STEREO,            g_DefaultPathStereo));
    DMTEST_EXECUTE(AudioPath_GetObjectInPath_TraverseStandard(ptPerf8, DMUS_APATH_DYNAMIC_MONO,             g_DefaultPathMono));
    DMTEST_EXECUTE(AudioPath_GetObjectInPath_TraverseStandard(ptPerf8, DMUS_APATH_DYNAMIC_3D,               g_DefaultPath3D));
    return hr;
};





