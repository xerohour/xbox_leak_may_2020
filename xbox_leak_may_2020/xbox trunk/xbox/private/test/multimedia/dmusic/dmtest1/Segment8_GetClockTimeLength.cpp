#include "globals.h"


/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetClockTimeLength_VerifySegmentLength(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentNameServer, DWORD dwExpectedLength)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
LPSTR szSegmentNameServer = (LPSTR)dwSegmentNameServer;
CHAR szSegmentName[MAX_PATH] = {0};
REFERENCE_TIME rtExpectedLength = *(REFERENCE_TIME *)dwExpectedLength;
REFERENCE_TIME rtLength             = 0;
BOOL           bClockTime           = 0;
BOOL           bExpectedClockTime   = 0;



//Copy everything from this segment's directory to the corresponding local directory, and also copy main1.dls to that directory.
strcpy(szSegmentName, MediaServerToLocal(szSegmentNameServer));
CHECKRUN(MediaCopyDirectory(szSegmentNameServer));
CHAR szDirectory[MAX_PATH] = {0};
CHECKRUN(ChopPath(szSegmentName, szDirectory, NULL));
MEDIAFILEDESC MediaFileDesc[]=
{
{"DLS/Main1.DLS",  szDirectory,    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));


if (strstr(szSegmentName, "rt") || strstr(szSegmentName, ".wav"))
    bExpectedClockTime = TRUE;
else if (strstr(szSegmentName, "mt"))
    bExpectedClockTime = FALSE;
else
    {
        Log(ABORTLOGLEVEL, "Test App Error - can't figure out type of segment from name");
        hr = E_FAIL;
    }

CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment8));
CHECKRUN(ptSegment8->GetClockTimeLength(&rtLength, &bClockTime));
if (SUCCEEDED(hr))
{
    if (rtLength != rtExpectedLength)
    {
        Log(ABORTLOGLEVEL, "%s: rtLength was %I64u instead of %I64u.", szSegmentName, rtLength, rtExpectedLength);
        hr = E_FAIL;
    }

    if (bClockTime != bExpectedClockTime)
    {
        Log(ABORTLOGLEVEL, "%s: bClockTime was %s instead of %s.", szSegmentName, bClockTime ? "TRUE" : "FALSE", bExpectedClockTime ? "TRUE" : "FALSE");
        hr = E_FAIL;
    }
}


SAFE_RELEASE(ptSegment8);
return hr;
};





/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetClockTimeLength_GetSet(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentNameServer, DWORD dwExpectedLength)
{
HRESULT hr = S_OK;
CtIDirectMusicSegment8 *ptSegment8 = NULL;
LPSTR szSegmentNameServer = (LPSTR)dwSegmentNameServer;
CHAR szSegmentName[MAX_PATH] = {0};
REFERENCE_TIME rtExpectedLength = *(REFERENCE_TIME *)dwExpectedLength;
REFERENCE_TIME rtLength             = 0;
BOOL           bClockTime           = 0;
BOOL           bExpectedClockTime   = 0;

//Copy everything from this segment's directory to the corresponding local directory, and also copy main1.dls to that directory.
strcpy(szSegmentName, MediaServerToLocal(szSegmentNameServer));
CHECKRUN(MediaCopyDirectory(szSegmentNameServer));
CHAR szDirectory[MAX_PATH] = {0};
CHECKRUN(ChopPath(szSegmentName, szDirectory, NULL));
MEDIAFILEDESC MediaFileDesc[]=
{
{"DLS/Main1.DLS",  szDirectory,    COPY_IF_NEWER},
{TERMINATE}
};
CHECKRUN(LoadMedia(MediaFileDesc));

    for (bExpectedClockTime = 0; bExpectedClockTime < 2; bExpectedClockTime++)
    {
        CHECKRUN(dmthLoadSegment(szSegmentName, &ptSegment8));
        CHECKRUN(ptSegment8->SetClockTimeLength(rtLength, bExpectedClockTime));
        CHECKRUN(ptSegment8->GetClockTimeLength(&rtExpectedLength, &bClockTime));
        if (SUCCEEDED(hr))
        {
            if (rtLength != rtExpectedLength)
            {
                Log(ABORTLOGLEVEL, "%s: rtLength was %I64u instead of %I64u.", szSegmentName, rtLength, rtExpectedLength);
                hr = E_FAIL;
            }

            if (bClockTime != bExpectedClockTime)
            {
                Log(ABORTLOGLEVEL, "%s: bClockTime was %s instead of %s.", szSegmentName, bClockTime ? "TRUE" : "FALSE", bExpectedClockTime ? "TRUE" : "FALSE");
                hr = E_FAIL;
            }
        }
        SAFE_RELEASE(ptSegment8);
    }

    return hr;
};

