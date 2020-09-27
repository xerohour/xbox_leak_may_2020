#include "globals.h"



/********************************************************************************
Verify that you get what you set.
/********************************************************************************/
HRESULT Segment8_GetClockTimeLoopPoints_SetAndGet(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
REFERENCE_TIME rtLoopPoint[2] = {0};
const REFERENCE_TIME rtLoopPointExpected[][2] = {
                                                {0,       1},
                                                {0,       10000000},
                                                {0,       60000000},
                                                {5999999, 60000000}
                                                };
BOOL bClockTime = 0;
CtIDirectMusicSegment *ptSegment = NULL;
CHECKRUN(dmthLoadSegment("T:\\DMTest1\\Segment8\\GetSetClockTimeLoopPoints\\CountTo6.wav", &ptSegment));


//Set some random loop points.
if (SUCCEEDED(hr))
{
    for (int j=0; j<AMOUNT(rtLoopPointExpected);j++)
    {
        CHECKRUN(ptSegment->SetClockTimeLoopPoints(rtLoopPointExpected[j][0],rtLoopPointExpected[j][1]));
        CHECKRUN(ptSegment->GetClockTimeLoopPoints(&rtLoopPoint[0], &rtLoopPoint[1]));
        for (int i=0; i<2; i++)
        {
            if (rtLoopPointExpected[j][i] != rtLoopPoint[i])
            {
                Log(ABORTLOGLEVEL, "Error: Set %I64u for %s but got back %I64u", rtLoopPointExpected[i], (i==0 ? "rtStart" : "rtEnd"),  rtLoopPoint[i]);
                hr = E_FAIL;
                break;
            }
        }
    }
}


SAFE_RELEASE(ptSegment);
return hr;
};



/********************************************************************************
Verify that you get what you set.
/********************************************************************************/
/*
HRESULT Segment8_GetClockTimeLoopPoints_FromSegment(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegment, DWORD dwLoopPointSet)
{
HRESULT hr = S_OK;
REFERENCE_TIME rtLoopPoint[2] = {0};
REFERENCE_TIME rtLoopPointExpected[][2] =   {
                                            {0,       1},
                                            {0,       10000000},


{0};
LPSTR szSegment = (LPSTR)dwSegment;


BOOL bClockTime = 0;
CtIDirectMusicSegment *ptSegment = NULL;
CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));

//Set some random loop points.
CHECKRUN(ptSegment->GetClockTimeLoopPoints(&rtLoopPoint[0], &rtLoopPoint[1]));
        for (int i=0; i<2; i++)
        {
            if (rtLoopPointExpected[j][i] != rtLoopPoint[i])
            {
                Log(ABORTLOGLEVEL, "Error: Set %I64u for %s but got back %I64u", rtLoopPointExpected[i], (i==0 ? "rtStart" : "rtEnd"),  rtLoopPoint[i]);
                hr = E_FAIL;
                break;
            }
        }
    }
}


SAFE_RELEASE(ptSegment);
return hr;};


*/