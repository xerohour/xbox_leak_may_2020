#include "globals.h"
#include "helpers.h"
#include "cicmusicx.h"
/*
#define BEFORESTOPPING 1
#define BEFOREUNLOADING 2
#define BEFORERELEASINGPATH 3
#define AFTEREVERYTHINGELSE 4
#define TWICE 5
*/



/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Performance8_CloseDown_WhilePlaying(CtIDirectMusicPerformance8* ptPerf8_x, DWORD dwScenario, DWORD dwUnused2)
{
    HRESULT hr = S_OK;


    CtIDirectMusicPerformance8* ptPerf8 = NULL;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;
    LPSTR szSegmentName = (LPSTR)g_TestParams.szDefaultMedia;

    
    if (!InitPerformanceNever())
        return S_OK;

    ASSERT(!ptPerf8_x);

    CHECKRUN(dmthCreatePerformance(IID_IDirectMusicPerformance, &ptPerf8))
    CHECKRUN(ptPerf8->InitAudioX(NULL, NULL, g_TestParams.dwDMVoiceCount, g_TestParams.bDoWorkLocalThread ? DMUS_INITAUDIO_NOTHREADS : 0));
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 16, TRUE, &ptPath));
    CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,NULL,0,ptPath));
    CHECKRUN(Log(FYILOGLEVEL, "Playing %s times; waiting until done", szSegmentName));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE));
    CHECKRUN(Wait(2000));
    
    if (ptPerf8)
    {
        if (CLOSEDOWN_BEFORESTOPPING == dwScenario)
            ptPerf8->CloseDown();

        CHECKRUN(Wait(1000));    

        if (CLOSEDOWN_BEFOREUNLOADING == dwScenario)
            ptPerf8->CloseDown();
    }


    SAFE_RELEASE(ptSegment);

    if (ptPerf8)
    {
        if (CLOSEDOWN_BEFORERELEASINGPATH == dwScenario)
            ptPerf8->CloseDown();
    }

    SAFE_RELEASE(ptPath);    

    if (ptPerf8)
    {
        if (CLOSEDOWN_AFTEREVERYTHINGELSE== dwScenario)
            ptPerf8->CloseDown();
    }

    SAFE_RELEASE(ptPerf8);


    if (ptPerf8)
    {
        if (CLOSEDOWN_TWICE == dwScenario)
        {
            ptPerf8->CloseDown();
            ptPerf8->CloseDown();
        }
    }



    return hr;
};
