#include "globals.h"

HRESULT Loader8_LoadObjectFromFile_Invalid_Ref(CtIDirectMusicPerformance8* ptPerf8);


/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectFromFile_Invalid(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    DMTEST_EXECUTE(Loader8_LoadObjectFromFile_Invalid_Ref(ptPerf8));
    return hr;
}



/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectFromFile_Invalid_Ref(CtIDirectMusicPerformance8* ptPerf8)
{
	HRESULT						hr				= 0;
	CtIDirectMusicSegment8		*ptSegment		= NULL;
	CHAR						*szSegmentFileServer = "DMusic/DMTest1/Loader/Inv_BrokenRef/Inv_BrokenRef.sgt";
	CHAR						szSegmentFile[MAX_PATH] = {0};
	CHAR						szSegmentPath[MAX_PATH] = {0};
    CtIDirectMusicLoader       *ptLoader      = NULL;

    CHECKRUN(MediaCopyFile(szSegmentFileServer));
    strcpy(szSegmentFile, MediaServerToLocal(szSegmentFileServer));
    CHECKRUN(ChopPath(szSegmentFile, szSegmentPath, NULL));

    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));
    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE));
    CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szSegmentFile, (void **)&ptSegment));
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptLoader);

    return hr;
}



/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectFromFile_Invalid_GMRef(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2)
{
	HRESULT						hr				        = 0;
	CtIDirectMusicSegment		*ptSegment		        = NULL;
    CtIDirectMusicLoader        *ptLoader               = NULL;
	CHAR						*szSegmentFileServer	= (CHAR *)dwFilename;
    CHAR                        szSegmentFile[MAX_PATH] = {0};
    CHAR                        szPath[MAX_PATH] = {0};
    CHAR                        szFile[MAX_PATH] = {0};

    
    CHECKRUN(MediaCopyFile(szSegmentFileServer));
    strcpy(szSegmentFile, MediaServerToLocal(szSegmentFileServer));
    CHECKRUN(ChopPath(szSegmentFile, szPath, szFile));
    
    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));
    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, FALSE));
    CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szFile, (void **)&ptSegment));
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptLoader);

    return hr;
}


/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectFromFile_TooLongPath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT						hr				= 0;
	CtIDirectMusicSegment8		*ptSegment		= NULL;
	CHAR						*szSegmentFileServer = "DMusic/DMTest1/Loader/Inv_TooLongPath/40CharacterName789_123456789_123456.wav";
	CHAR						szSegmentFile[MAX_PATH] = {0};
	CHAR						szSegmentPath[MAX_PATH] = {0};
    CtIDirectMusicLoader       *ptLoader      = NULL;
    HRESULT hrCall = S_OK;

    CHECKRUN(MediaCopyFile(szSegmentFileServer));
    strcpy(szSegmentFile, MediaServerToLocal(szSegmentFileServer));
    CHECKRUN(ChopPath(szSegmentFile, szSegmentPath, NULL));


    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));
    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE));
    if (SUCCEEDED(hr))
    {
        hrCall = ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szSegmentFile, (void **)&ptSegment);
    }
    if (hrCall != DMUS_E_LOADER_FAILEDCREATE)
    {
        Log(ABORTLOGLEVEL, "Error: LoadObjectFromFile returned %s instead of DMUS_E_LOADER_FAILEDCREATE", tdmXlatHRESULT(hr));
        if (SUCCEEDED(hr))
            hr = E_FAIL;
    }

    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptLoader);
    return hr;
}


/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectFromFile_Invalid_Content(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
	HRESULT						hr			   = 0;
	CtIDirectMusicSegment8		*ptSegment	   = NULL;
	CHAR						*szSegmentFileServer = "DMusic/DMTest1/Loader/Inv_AudioPath/Inv_AudioPath.sgt";	
	CHAR						szSegmentFile[MAX_PATH] = {0};
	CHAR						szSegmentPath[MAX_PATH] = {0};
    CtIDirectMusicLoader        *ptLoader      = NULL;

    CHECKRUN(MediaCopyFile(szSegmentFileServer));
    strcpy(szSegmentFile, MediaServerToLocal(szSegmentFileServer));
    CHECKRUN(ChopPath(szSegmentFile, szSegmentPath, NULL));

    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));
    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE));
    CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, CTIID_IDirectMusicSegment, szSegmentFile, (void **)&ptSegment));
    SAFE_RELEASE(ptSegment);
    SAFE_RELEASE(ptLoader);

    return hr;
}
