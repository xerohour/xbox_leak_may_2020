/********************************************************************************
	FILE:
		DLS_Volume.cpp

	PURPOSE:
		Test Volume Articulation on DLS2 instruments

	BY:
		DANHAFF
********************************************************************************/
#include "globals.h"


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT DLS_TestSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentName, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    LPSTR szSegmentName = (LPSTR)dwSegmentName;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CtIDirectMusicSegment   *ptSegment = NULL;
    CICMusic Music;
    CHAR *szFileLocation[MAX_PATH] = {0};

    //copy everything in that directory
    CHECKRUN(MediaCopyDirectory(szSegmentName));
    CHECKRUN(Music.Init(ptPerf8, MediaServerToLocal(szSegmentName), DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(Music.GetInterface(&ptSegment));
    CHECKRUN(Music.GetInterface(&ptPath));
    CHECKRUN(ptPerf8->StopEx(0, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 50000, szSegmentName));
    CHECKRUN(ptSegment->SetRepeats(0));
    CHECKRUN(InsertToolTrace(ptPath));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,0,0,ptPath));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, szSegmentName));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 50000, szSegmentName));
    RemoveToolTrace(ptPath);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);    


    return hr;
};


