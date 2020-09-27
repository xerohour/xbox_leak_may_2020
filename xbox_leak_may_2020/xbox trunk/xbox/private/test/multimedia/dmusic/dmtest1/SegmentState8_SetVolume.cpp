#include "globals.h"
#include "AudioPath_SetVolume.hpp"

//lint -e767
#define INTERFACE_NAME CtIDirectMusicSegmentState

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT SegmentState_SetVolume_BVT(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return tmplSetVolume_Direct<INTERFACE_NAME>(ptPerf8);
};



/********************************************************************************
Valid test function.
*******************************************************************************/
HRESULT SegmentState_SetVolume_Valid_Ramp(CtIDirectMusicPerformance8* ptPerf8, DWORD dwTime, DWORD dwAudioPath)
{
    return tmplSetVolume_Ramp<INTERFACE_NAME>(ptPerf8, dwTime, dwAudioPath);
};

#undef INTERFACE_NAME