#include "globals.h"

/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetRepeats_Content(CtIDirectMusicPerformance8* ptPerf8, DWORD dwRepeats, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment *ptSegment = NULL;
CtIDirectMusicAudioPath *ptPath = NULL;
CHAR szServer[MAX_PATH] = {0};
CHAR szLocal [MAX_PATH] = {0};
DWORD dwRepeatsActualValue = 0;
DWORD dwStartTime = 0;
DWORD dwExpectedTime = 0;
DWORD dwActualTime = 0;
double fErrorPercent= 0;
double fMaxErrorPercent= 0;

//Download the media.
if (dwRepeats != 0xFFFFFFFF)
    sprintf(szServer, "DMusic/DMTest1/Segment8/GetRepeats/Repeat_%d.sgt", dwRepeats);
else
    sprintf(szServer, "DMusic/DMTest1/Segment8/GetRepeats/Repeat_Inf.sgt");
CHECKRUN(MediaServerToLocal(szServer, szLocal))
CHECKRUN(MediaCopyFile(szServer));

//Verify we got the correct repeats programatically.
CHECKRUN(dmthLoadSegment(szLocal, &ptSegment));
CHECKRUN(ptSegment->GetRepeats(&dwRepeatsActualValue));
if (SUCCEEDED(hr))
{
    if (dwRepeatsActualValue != dwRepeats)
    {
        Log(ABORTLOGLEVEL, "Error: Expected %u repeats, got %u.", dwRepeats, dwRepeatsActualValue);
        hr = E_FAIL;
    }
}

//Verify the timing is correct.
if (dwRepeats != 0xFFFFFFFF && dwRepeats != 32767)
{
    dwExpectedTime = (dwRepeats + 1) * 2 * 1000;
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &ptPath));
    dwStartTime = timeGetTime();
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));
    CHECKRUN(ExpectSegmentStart(ptPerf8, ptSegment, NULL, 1000, NULL));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, dwExpectedTime * 2, szLocal));
    if (SUCCEEDED(hr))
    {   
        dwActualTime = timeGetTime() - dwStartTime;
        fErrorPercent = CalculateErrorPercentage((double)dwExpectedTime, (double)dwActualTime);
        Log(FYILOGLEVEL, "Error = %.2f%%, maximum allowed error is %.2f%%", fErrorPercent, fMaxErrorPercent);
        if (fErrorPercent> fMaxErrorPercent)
        {
                Log(ABORTLOGLEVEL, "FAILURE!!  %.2f%% exceeds maximum allowed error of %.2f%%!!!", fErrorPercent, fMaxErrorPercent);
            hr = E_FAIL;
        }
    }
}

//Print info regarding the tempo.
if (SUCCEEDED(hr))
{
    double fTempo = 0;
    if (SUCCEEDED(GetTempo(ptPerf8, &fTempo)))
    {
        if (fTempo != 120.f)
        {
            Log(ABORTLOGLEVEL, "Note: Tempo was %.2f% instead of 120", fTempo);
        }
    }
}

ptPerf8->StopEx(0, 0, 0);
RELEASE(ptSegment);
RELEASE(ptPath);
return hr;
};



/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetRepeats_Programmatic(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment *ptSegment = NULL;
LPSTR szServer = "DMusic/DMTest1/Segment8/GetRepeats/Repeat_1.sgt";
CHAR szLocal [MAX_PATH] = {0};

DWORD dwRepeats[] = {0, 1, 2, 32767, 0x0000FFFF, 0xFFFFFFFF};

CHECKRUN(MediaServerToLocal(szServer, szLocal))
CHECKRUN(MediaCopyFile(szServer));
CHECKRUN(dmthLoadSegment(szLocal, &ptSegment));

for (DWORD i=0; i<NUMELEMS(dwRepeats) && SUCCEEDED(hr); i++)
{
    DWORD dwTemp = 0;
    CHECKRUN(ptSegment->SetRepeats(dwRepeats[i]));
    CHECKRUN(ptSegment->GetRepeats(&dwTemp));
    if (dwTemp != dwRepeats[i])
    {
        Log(ABORTLOGLEVEL, "Error: SetRepeats %u, got back %u", dwRepeats[i], dwTemp);
        hr = E_FAIL;
    }
}

RELEASE(ptSegment);
return hr;
};

