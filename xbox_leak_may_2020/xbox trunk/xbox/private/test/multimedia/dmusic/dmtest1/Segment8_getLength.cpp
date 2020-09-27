#include "globals.h"

HRESULT Segment8_GetLength_VerifySegmentLength(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegmentName, MUSIC_TIME dwExpectedLength);
HRESULT Segment8_GetLength_SetAndGetSegmentLength(CtIDirectMusicPerformance8* ptPerf8);

struct SEGLENGTH
{
LPSTR szSegmentName;
MUSIC_TIME mtLength;
};


/********************************************************************************
1) Verify length is 1 for wave files.
2) Verify length is 0 for 0-length segments.
3) Verify length is correct for other segments.
********************************************************************************/


/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Segment8_GetLength_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    DWORD i = 0;

    //Get the needed media for this test.
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Segment8/GetLength/"));
    LPSTR szDirectory = "T:\\DMTest1\\Segment8\\GetLength\\";
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"DLS/Main1.DLS",      szDirectory,    COPY_IF_NEWER},
    {"SGT/1Bar.sgt",       szDirectory,    COPY_IF_NEWER},
    {"SGT/2Bars.sgt",      szDirectory,    COPY_IF_NEWER},
    {"SGT/999Bars.sgt",    szDirectory,    COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));




    SEGLENGTH SegLength[] = {
                             {"T:\\DMTest1\\Segment8\\GetLength\\1Bar.sgt",                       DMUS_PPQ * 4 * 1},
                             {"T:\\DMTest1\\Segment8\\GetLength\\1BarExtended.sgt",               DMUS_PPQ * 4 * 1},
                             {"T:\\DMTest1\\Segment8\\GetLength\\2Bars.sgt",                      DMUS_PPQ * 4 * 2},
                             {"T:\\DMTest1\\Segment8\\GetLength\\2BarsLoopingInfinite.sgt",       DMUS_PPQ * 4 * 2},
                             {"T:\\DMTest1\\Segment8\\GetLength\\2BarsLoopingOnce.sgt",           DMUS_PPQ * 4 * 2},
                             {"T:\\DMTest1\\Segment8\\GetLength\\999Bars.sgt",                    DMUS_PPQ * 4 * 999}, 
                             {"T:\\DMTest1\\Segment8\\GetLength\\WaveSegment.wav",                1}          
                            };

    Log(FYILOGLEVEL, "Calling BVT test function Segment8_GetLength_BVT())");
    for (i=0; i<AMOUNT(SegLength); i++)
    {
        DMTEST_EXECUTE(Segment8_GetLength_VerifySegmentLength(ptPerf8, SegLength[i].szSegmentName, SegLength[i].mtLength));
    }
    
    return hr;

};

/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Segment8_GetLength_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Segment8_GetLength_Valid())");

    DMTEST_EXECUTE(Segment8_GetLength_SetAndGetSegmentLength(ptPerf8));

    return hr;

};





/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetLength_VerifySegmentLength(CtIDirectMusicPerformance8* ptPerf8, LPSTR szSegmentName, MUSIC_TIME mtExpectedLength)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
MUSIC_TIME mtLength = 0;

//Note: No need to load the media here, as it is already loaded by the calling function.
CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment8));
CHECKRUN(ptSegment8->GetLength(&mtLength));
if (mtLength != mtExpectedLength)
{
    Log(ABORTLOGLEVEL, "%s: Length was %d instead of %d.", szSegmentName, mtLength, mtExpectedLength);
    hr = E_FAIL;
}


SAFE_RELEASE(ptSegment8);
return hr;
};


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetLength_SetAndGetSegmentLength(CtIDirectMusicPerformance8* ptPerf8)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
LPSTR szSegmentNameServer = "DMusic/DMTest1/Segment8/GetLength/1Bar.sgt";
MUSIC_TIME mtLength = 17;
MUSIC_TIME mtLength2 = 0;

LPSTR szDirectory = "T:\\DMTest1\\Segment8\\GetLength\\";
MEDIAFILEDESC MediaFileDesc[]=
{
{"DLS/Main1.DLS",      szDirectory,    COPY_IF_NEWER},
{"SGT/1Bar.sgt",       szDirectory,    COPY_IF_NEWER},
{TERMINATE}
};

CHECKRUN(LoadMedia(MediaFileDesc));
CHECKRUN(dmthLoadSegment(MediaServerToLocal(szSegmentNameServer), &ptSegment8));
CHECKRUN(ptSegment8->SetLength(mtLength));
CHECKRUN(ptSegment8->GetLength(&mtLength2));
if (mtLength != mtLength2)
{
    Log(ABORTLOGLEVEL, "%s: Set length to %d but returned length was %d.", mtLength, mtLength2);
    hr = E_FAIL;
}

SAFE_RELEASE(ptSegment8);
return hr;
};