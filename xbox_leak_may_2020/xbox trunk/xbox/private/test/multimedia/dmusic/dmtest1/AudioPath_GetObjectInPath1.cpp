#include "globals.h"
#include "AudioPath_GetObjectInPath1.hpp"

#define INTERFACE_NAME CtIDirectMusicAudioPath

HRESULT AudioPath_GetObjectInPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
HRESULT AudioPath_GetObjectInPath_Valid_GetAll(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
HRESULT AudioPath_GetObjectInPath_Valid_Traverse(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);
HRESULT AudioPath_GetObjectInPath_Valid_SimplyGetBuffer(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2);


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function AudioPath_GetObjectInPath())");
    DMTEST_EXECUTE(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_PATH_PERFORMANCE, (GUID *)&CLSID_DirectMusicPerformance, (GUID *)&IID_IDirectMusicPerformance8));                                             
    DMTEST_EXECUTE(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, DMUS_APATH_SHARED_STEREO,           DMUS_PATH_PERFORMANCE, (GUID *)&CLSID_DirectMusicPerformance, (GUID *)&IID_IDirectMusicPerformance8));                                             
    DMTEST_EXECUTE(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, DMUS_APATH_DYNAMIC_3D,              DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    DMTEST_EXECUTE(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, DMUS_APATH_DYNAMIC_MONO,            DMUS_PATH_BUFFER,      (GUID *)&GUID_All_Objects, (GUID *)&GUID_NULL));                                             
    return hr;
};




/********************************************************************************
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_Valid_Traverse(CtIDirectMusicPerformance8* ptPerf8, DWORD dwPathType, DWORD dwPathDefinition)
{
    HRESULT hr = S_OK;
    PATH_DEFINITION *pPathDefinition = (PATH_DEFINITION *)dwPathDefinition;
    DMTEST_EXECUTE(tmplTraverseStandard<INTERFACE_NAME>(ptPerf8, dwPathType,  pPathDefinition));
    return hr;
};





/********************************************************************************
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_Valid_GetAll(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    DWORD dwPerfPaths[] = {DMUS_APATH_SHARED_STEREOPLUSREVERB, DMUS_APATH_DYNAMIC_3D, DMUS_APATH_DYNAMIC_MONO, DMUS_APATH_SHARED_STEREO};
    GUID *Perf_CLSIDs[] = 
    {
        (GUID *)&CLSID_DirectMusicPerformance,
        (GUID *)&GUID_All_Objects,
        (GUID *)&GUID_NULL,
    };
    GUID *Perf_IIDs[] = 
    {
        (GUID *)&IID_IDirectMusicPerformance,
        (GUID *)&IID_IUnknown,
    };

    DWORD dwBufPaths[] = {DMUS_APATH_DYNAMIC_3D, DMUS_APATH_DYNAMIC_MONO};
    GUID *Buf_CLSIDs[] = 
    {
        (GUID *)&GUID_All_Objects,
        (GUID *)&GUID_NULL,
    };
    GUID *Buf_IIDs[] = 
    {
        (GUID *)&GUID_NULL,
    };

    DWORD i,j,k;
    //Make sure you can get a performance from all the paths with all GUIDs and all IIDs possible.
    for (i=0; i<AMOUNT(dwPerfPaths); i++)
    {
        for (j=0; j<AMOUNT(Perf_CLSIDs); j++)
        {
            for (k=0; k<AMOUNT(Perf_IIDs); k++)
            {
                CHECKRUN(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, dwPerfPaths[i], DMUS_PATH_PERFORMANCE, Perf_CLSIDs[j], Perf_IIDs[k]));                                             

                if (FAILED(hr))
                {
                    Log(ABORTLOGLEVEL, "Couldn't obtain %s, %s from DMUS_PATH_PERFORMANCE on %s", tdmXlatGUID(*Perf_CLSIDs[j]), tdmXlatGUID(*Perf_IIDs[k]), dmthXlatDMUS_APATH(dwPerfPaths[i]));
                    return hr;
                }
                else
                {
                    Log(FYILOGLEVEL, "Obtained %s, %s from DMUS_PATH_PERFORMANCE on %s", tdmXlatGUID(*Perf_CLSIDs[j]), tdmXlatGUID(*Perf_IIDs[k]), dmthXlatDMUS_APATH(dwPerfPaths[i]));
                }


            }
        }
    }


    //Make sure you can get a buffer from all the paths with all GUIDs and all IIDs possible.
    for (i=0; i<AMOUNT(dwBufPaths); i++)
    {
        for (j=0; j<AMOUNT(Buf_CLSIDs); j++)
        {
            for (k=0; k<AMOUNT(Buf_IIDs); k++)
            {
                CHECKRUN(tmplGetObjectInPath<INTERFACE_NAME>(ptPerf8, dwBufPaths[i], DMUS_PATH_BUFFER, Buf_CLSIDs[j], Buf_IIDs[k]));                                             
                if (FAILED(hr))
                {
                    Log(ABORTLOGLEVEL, "Couldn't obtain %s, %s from DMUS_PATH_BUFFER on %s", tdmXlatGUID(*Buf_CLSIDs[j]), tdmXlatGUID(*Buf_IIDs[k]), dmthXlatDMUS_APATH(dwBufPaths[i]));
                    return hr;
                }
                else
                {
                    Log(ABORTLOGLEVEL, "Obtained %s, %s from DMUS_PATH_BUFFER on %s", tdmXlatGUID(*Buf_CLSIDs[j]), tdmXlatGUID(*Buf_IIDs[k]), dmthXlatDMUS_APATH(dwBufPaths[i]));
                }
            }
        }
    }


    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT AudioPath_GetObjectInPath_Valid_SimplyGetBuffer(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;    
    DMTEST_EXECUTE(tmplGetBuffer<INTERFACE_NAME>(ptPerf8, DMUS_APATH_DYNAMIC_3D));
    return hr;
};