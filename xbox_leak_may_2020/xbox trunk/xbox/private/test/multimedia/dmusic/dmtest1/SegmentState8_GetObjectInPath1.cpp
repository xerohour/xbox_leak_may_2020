#include "globals.h"
#include "cicmusicx.h"
#include "AudioPath_GetObjectInPath1.hpp"




HRESULT SegmentState_GetObjectInPath_BVT1(CtIDirectMusicPerformance8 *ptPerf8,
                                      DWORD dwPathType,
                                      DWORD dwStage,
                                      GUID *pguidObject,
                                      GUID *pguidInterface);



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT SegmentState_GetObjectInPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling main test function SegmentState8_GetObjectInPath())");
    DMTEST_EXECUTE(SegmentState_GetObjectInPath_BVT1(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_PATH_SEGMENT,     (GUID *)&GUID_All_Objects, (GUID *)&IID_IDirectMusicSegment8));
    DMTEST_EXECUTE(SegmentState_GetObjectInPath_BVT1(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_PATH_PERFORMANCE, (GUID *)&CLSID_DirectMusicPerformance, (GUID *)&IID_IDirectMusicPerformance8));                                             
    DMTEST_EXECUTE(SegmentState_GetObjectInPath_BVT1(ptPerf8, DMUS_APATH_DYNAMIC_3D,              DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    DMTEST_EXECUTE(SegmentState_GetObjectInPath_BVT1(ptPerf8, DMUS_APATH_DYNAMIC_3D,              DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    DMTEST_EXECUTE(SegmentState_GetObjectInPath_BVT1(ptPerf8, DMUS_APATH_DYNAMIC_MONO,            DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    return hr;
};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT SegmentState_GetObjectInPath_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Valid test function Segment8_GetObjectInPath_Valid())");
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
HRESULT SegmentState_GetObjectInPath_BVT1(CtIDirectMusicPerformance8 *ptPerf8,
                                      DWORD dwPathType,
                                      DWORD dwStage,
                                      GUID *pguidObject,
                                      GUID *pguidInterface)
{
    return tmplGetObjectInPath<CtIDirectMusicSegmentState8>(ptPerf8, dwPathType, dwStage, pguidObject, pguidInterface);
};



