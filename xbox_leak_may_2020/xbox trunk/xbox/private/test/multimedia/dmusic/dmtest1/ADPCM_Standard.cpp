/********************************************************************************
	FILE:
		ADPCM_Standard.cpp

	PURPOSE:
		Test Playback of ADPCM content.

	BY:
		DANHAFF
********************************************************************************/
#include "globals.h"


/********************************************************************************
********************************************************************************/
    //{"ADPCM_OneShot_11_1",  "Valid", ADPCM_TestWaveSegment, (DWORD)"OneShot", (DWORD)"OS_11_1"},

HRESULT ADPCM_TestWaveSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused, DWORD dwBaseName)
{
    HRESULT hr = S_OK;
    LPSTR szBaseName = (LPSTR)dwBaseName;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CtIDirectMusicSegment   *ptSegment = NULL;
    CICMusic *pMusic;
    CHAR szFrom[MAX_PATH]   = {0};
    CHAR szTo[MAX_PATH]     = {0};
    CHAR szLoad[MAX_PATH]     = {0};
    MEDIAFILEDESC MediaFileDescs[] = {szFrom, szTo, COPY_IF_NEWER, TERMINATE};
    
    for (int i=0; i<2 && SUCCEEDED(hr); i++)
    {
        sprintf(szFrom, "WAV/%s_%s.wav", i ? "ADPCM" : "PCM", dwBaseName);
        sprintf(szTo,   "T:\\WAV\\");
        sprintf(szLoad, "T:\\WAV\\%s_%s.wav", i ? "ADPCM" : "PCM", dwBaseName);
        CHECKRUN(LoadMedia(&MediaFileDescs[0]));
        ALLOCATEANDCHECK(pMusic, CICMusic);
        CHECKRUN(pMusic->Init(ptPerf8, szLoad, DMUS_APATH_SHARED_STEREOPLUSREVERB));
        CHECKRUN(pMusic->GetInterface(&ptSegment));
        CHECKRUN(pMusic->GetInterface(&ptPath));
        ptPerf8->StopEx(0, 0, 0);
        CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 5000, szTo));
        CHECKRUN(ptSegment->SetRepeats(0));
        CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,0,0,ptPath));
        CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, szTo));
        CHECKRUN(WaitForSegmentStop(ptPerf8, ptSegment, NULL, 11000, TRUE));
        ptPerf8->StopEx(0, 0, 0);
        SAFE_RELEASE(ptPath);
        SAFE_RELEASE(ptSegment);    
        delete pMusic;
        pMusic = NULL;
    }

    return hr;
};




/********************************************************************************
********************************************************************************/
HRESULT ADPCM_TestMusicSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwSegmentName)
{
    HRESULT hr = S_OK;
    LPSTR szSegmentName= (LPSTR)dwSegmentName;
    CtIDirectMusicAudioPath *ptPath = NULL;
    CtIDirectMusicSegment   *ptSegment = NULL;
    CICMusic *pMusic = NULL;
    CHAR szTo[MAX_PATH]     = {0};
    CHAR szLoad[MAX_PATH]     = {0};
    
    sprintf(szTo,   "T:\\DMTest1\\Perf8\\PlaySeg\\ADPCM\\");
    sprintf(szLoad,   "T:\\DMTest1\\Perf8\\PlaySeg\\ADPCM\\%s",szSegmentName);
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Perf8/PlaySeg/ADPCM/", szTo));    
    ALLOCATEANDCHECK(pMusic, CICMusic);
    CHECKRUN(pMusic->Init(ptPerf8, szLoad, DMUS_APATH_SHARED_STEREOPLUSREVERB));
    CHECKRUN(pMusic->GetInterface(&ptSegment));
    CHECKRUN(pMusic->GetInterface(&ptPath));
    CHECKRUN(InsertToolTrace(ptPath));
    CHECKRUN(ptSegment->SetRepeats(0));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment,0,0,0,0,0,0,ptPath));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 5000, szTo));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 21000, szTo));
    if (ptPath)
        RemoveToolTrace(ptPath);
    SAFE_RELEASE(ptPath);
    SAFE_RELEASE(ptSegment);    
    delete pMusic;
    pMusic = NULL;

    return hr;
};

