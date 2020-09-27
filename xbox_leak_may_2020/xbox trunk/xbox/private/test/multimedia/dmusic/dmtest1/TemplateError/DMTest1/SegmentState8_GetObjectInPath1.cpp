#include "globals.h"
#include "cicmusicx.h"
#include "AudioPath_GetObjectInPath1.hpp"




HRESULT SegmentState8_GetObjectInPath_BVT(CtIDirectMusicPerformance8 *ptPerf8,
                                      DWORD dwPathType,
                                      DWORD dwStage,
                                      GUID *pguidObject,
                                      GUID *pguidInterface);



/********************************************************************************
Main test function.
********************************************************************************/
HRESULT SegmentState8_GetObjectInPath (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    DWORD dwRes = FNS_FAIL;
    fnsLog(FYILOGLEVEL, "Calling main test function AudioPath_GetObjectInPath())");
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_PATH_SEGMENT,     (GUID *)&GUID_All_Objects, (GUID *)&IID_IDirectMusicSegment8));
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_PATH_PERFORMANCE, (GUID *)&CLSID_DirectMusicPerformance, (GUID *)&IID_IDirectMusicPerformance8));                                             
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_DYNAMIC_3D,              DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_DYNAMIC_3D,              DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_DYNAMIC_MONO,            DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    CHECKEXECUTE(SegmentState8_GetObjectInPath_BVT(ptPerf8, DMUS_APATH_DYNAMIC_STEREO,          DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    return hr;
};



/********************************************************************************
UNIT TEST
IDirectMusicAudioPath::GetObjectInPath()

HISTORY:

PARAMS:
    dwParam3:
        Which test scenario will be run.  Currently there are i scenarios, each
        with different standard paths, objects to retrieve, etc...
    dwParam4         Not used
********************************************************************************/
HRESULT SegmentState8_GetObjectInPath_BVT(CtIDirectMusicPerformance8 *ptPerf8,
                                      DWORD dwPathType,
                                      DWORD dwStage,
                                      GUID *pguidObject,
                                      GUID *pguidInterface)
{
    return tmplGetObjectInPath<CtIDirectMusicSegmentState8>(ptPerf8, dwPathType, dwStage, pguidObject, pguidInterface);
};



