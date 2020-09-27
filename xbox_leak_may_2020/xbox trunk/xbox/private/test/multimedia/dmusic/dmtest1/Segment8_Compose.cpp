#include "globals.h"

#define ORIGINAL_SEGMENT 0
#define NEW_SEGMENT 1


HRESULT Segment8_Compose_Transition(CtIDirectMusicPerformance8* ptPerf8, MUSIC_TIME mt, LPSTR szFrom, LPSTR szTo, BOOL bNewSegment);
HRESULT Segment8_Compose_Yourself(CtIDirectMusicPerformance8* ptPerf8, LPSTR szFileName, BOOL bClone, LPSTR szInfo);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Segment8_Compose_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    return Segment8_Compose_Yourself(ptPerf8, g_TestParams.szDefaultMedia, FALSE, "Should hear normal boring test segment");
};

/********************************************************************************
Valid test function.
********************************************************************************/
/*
HRESULT Segment8_Compose_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{   
    //Load media.
    HRESULT hr = S_OK;
    MEDIAFILEDESC MediaFileDesc[]=
    {
    {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
    {"SGT/test2.sgt",     "T:\\DMTest1",    COPY_IF_NEWER},
    {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
    {TERMINATE}
    };
    CHECKRUN(LoadMedia(MediaFileDesc));
    CHECKRUN(Segment8_Compose_Transition(ptPerf8, 0, "T:\\DMTest1\\test.sgt", "T:\\DMTest1\\test2.sgt", NEW_SEGMENT));
    return hr;
};  
*/

/********************************************************************************
BVT test function.  Makes a segment compose itself.
********************************************************************************/
HRESULT Segment8_Compose_Yourself(CtIDirectMusicPerformance8* ptPerf8, LPSTR szFileName, BOOL bClone, LPSTR szInfo)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = NULL;
CtIDirectMusicSegment8              *ptSegment       = NULL;
CtIDirectMusicSegment8              *ptClone         = NULL;

CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,64,TRUE,&ptPath));
CHECKRUN(dmthLoadSegment(szFileName, &ptSegment));
CHECKRUN(ptSegment->Compose(0, NULL, NULL, bClone ? &ptClone : NULL));
CHECKRUN(ptPerf8->PlaySegmentEx(bClone ? ptClone : ptSegment,0,0,0,__int64(0),0,0,ptPath));
CHECKRUN(Log(FYILOGLEVEL, "Playing composed, %s segment %s in its entirety.", bClone ? "cloned" : "non-cloned", szFileName));
if (szInfo)
    CHECKRUN(Log(FYILOGLEVEL, szInfo));
CHECKRUN(ExpectSegmentStart(ptPerf8, bClone ? ptClone : ptSegment, NULL, 2000,  "To"));
CHECKRUN(ExpectSegmentStop (ptPerf8, bClone ? ptClone : ptSegment, NULL, 20000, "To"));

SAFE_RELEASE(ptSegment);
SAFE_RELEASE(ptClone);
SAFE_RELEASE(ptPath);
return hr;

};


/********************************************************************************
Valid test function.
********************************************************************************/
/*
HRESULT Segment8_Compose_Transition(CtIDirectMusicPerformance8* ptPerf8, MUSIC_TIME mt, LPSTR szFrom, LPSTR szTo, BOOL bNewSegment)
{
HRESULT hr                                              = S_OK;
CtIDirectMusicAudioPath             *ptPath          = NULL;
CtIDirectMusicSegment8              *ptSegment8[2]   = {NULL};
CtIDirectMusicSegment8              *ptOriginalEmpty = NULL;
CtIDirectMusicSegment8              *ptComposed        = NULL;

CHAR *szFileName[2]                 = {NULL};
DWORD i = 0;

    szFileName[0] = szFrom;
    szFileName[1] = szTo;
    
    //Create the audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                          64,
                                          TRUE,
                                          &ptPath));

    //Load up the filez.
    for (i=0; i<2; i++)
    {
        if (szFileName[i])
        {
            CHECKRUN(dmthLoadSegment(szFileName[i], &ptSegment8[i]));
        }
    }
    //Create an empty segment and compose something.
    CHECKRUN(dmthCreateEmptySegment(&ptOriginalEmpty));
    CHECKRUN(ptOriginalEmpty->Compose(mt, ptSegment8[0], ptSegment8[1], bNewSegment ? &ptComposed : NULL));    //Play
	CHECKRUN(ptPerf8->PlaySegmentEx(bNewSegment ? ptComposed : ptOriginalEmpty,0,0,0,__int64(0),0,0,ptPath));

    
    CHECKRUN(Log(FYILOGLEVEL, "Playing 'from' segment (%s)", szFileName[0]));
    CHECKRUN(ExpectSegmentStart(ptPerf8, bNewSegment ? ptComposed : ptOriginalEmpty, NULL, 2000,  "From"));
    CHECKRUN(ExpectSegmentStop (ptPerf8, bNewSegment ? ptComposed : ptOriginalEmpty, NULL, 20000, "From"));

    SAFE_RELEASE(ptOriginalEmpty);
    SAFE_RELEASE(ptComposed);
    SAFE_RELEASE(ptPath);

    return hr;

}



*/