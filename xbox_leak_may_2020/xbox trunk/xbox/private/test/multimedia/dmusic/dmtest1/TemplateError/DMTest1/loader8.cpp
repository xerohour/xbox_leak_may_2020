/********************************************************************************
FILE:
    LOADER8.cpp

PURPOSE:
    Contains loader test functions.

BY:
    DANROSE
********************************************************************************/
#include "globals.h"



/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectTwice(CtIDirectMusicPerformance8* ptPerf8)
{
	HRESULT						hr				= 0;
	HRESULT						hrCall          = S_OK;
	CtIDirectMusicSegment8		*ptSegment		= NULL;
	CHAR						*szSegmentFile	= "T:\\MEDIA\\Perf8\\GetSegSt\\DSegTst3.sgt";
	CHAR						*szSegmentPath	= "T:\\MEDIA\\Perf8\\GetSegSt";
    CtIDirectMusicLoader8       *ptLoader8      = NULL;
    BOOL bFailed = FALSE;
    DWORD i;

    // **********************************************************************
    // 1) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)
    // **********************************************************************
    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &ptLoader8);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE);
    if (FAILED(hr))
    {
        fnsLog(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
                TEXT("failed (%s === %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    // **********************************************************************
    // 4) Load the Segment Object
    // **********************************************************************
    DMUS_OBJECTDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize         = sizeof(DMUS_OBJECTDESC);
    desc.guidClass      = CLSID_DirectMusicSegment;
    desc.dwValidData    |= (DMUS_OBJ_CLASS | DMUS_OBJ_FULLPATH);


    // Get the Segment object
    mbstowcs(desc.wszFileName, szSegmentFile, strlen(szSegmentFile) + 1);

    for (i=0; i<2; i++)
    {
        hr = ptLoader8->GetObject(&desc, CTIID_IDirectMusicSegment, (void **)&ptSegment);
        fnsLog(ABORTLOGLEVEL, "dmthLoadSegment(%s) returned %s (%08Xh)", szSegmentFile, tdmXlatHRESULT(hr), hr);
        if(FAILED(hr))
        {
            hrCall = hr;
        }

        SAFE_RELEASE(ptSegment);
    }

    TEST_END:


    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptLoader8);
    return hrCall;

}
