#include "globals.h"



/********************************************************************************
/********************************************************************************/
HRESULT Segment8_GetStartPoint_Generic(CtIDirectMusicPerformance8* ptPerf8, DWORD dwSegmentServer, DWORD dwStartPointExpected)
{
HRESULT hr = S_OK;
LPSTR szSegmentServer = (LPSTR)dwSegmentServer;
MUSIC_TIME mtStartPointExpected = dwStartPointExpected;
MUSIC_TIME mtStart = 0;
CtIDirectMusicSegment *ptSegment = NULL;
CHAR szSegment[MAX_PATH] = {0};

ASSERT(mtStartPointExpected >=0);

//Copy down the media.
CHECKRUN(MediaCopyDirectory(szSegmentServer));
strcpy(szSegment, MediaServerToLocal(szSegmentServer));

CHECKRUN(dmthLoadSegment(szSegment, &ptSegment));
CHECKRUN(ptSegment->GetStartPoint(&mtStart));
if (mtStart != mtStartPointExpected)
{
    Log(ABORTLOGLEVEL, "Start point for %s was %d instead of %d", szSegment, mtStart, mtStartPointExpected);
    hr = E_FAIL;
}

RELEASE(ptSegment);
return hr;
};

