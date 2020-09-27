#include "globals.h"
#include "cicmusicx.h"
#include "Help_Stream.h"

using namespace LocalFileStream;

HRESULT VerifyRefCount(LPSTR szName, IUnknown *pUnk, DWORD dwExpected)
{
HRESULT hr = S_OK;
DWORD dwRefCount = 0;

    if (!pUnk)
        return E_FAIL;

    pUnk->AddRef();
    dwRefCount = pUnk->Release();

    //Verify that the refcount is 1.
    if (dwRefCount != dwExpected)
    {   
        Log(ABORTLOGLEVEL, "Error: refcount of %s is %u instead of %u", szName ? szName : "object", dwRefCount, dwExpected);
        hr = E_FAIL;
    }
    else
    {
        hr = S_OK;
    }

    return hr;
}



/********************************************************************************
********************************************************************************/
HRESULT Loader_GetObject_IStream_Generic(CtIDirectMusicPerformance8* ptPerf8, LPCSTR szFileName)
{
    HRESULT hr = S_OK;
    IStream *pStream = NULL;
    IStream *pClone = NULL;
    CtIDirectMusicLoader *ptLoader = NULL;
    IDirectMusicSegment *pSegment = NULL;
    CtIDirectMusicSegment *ptSegment = NULL;
    CtIDirectMusicAudioPath *ptPath = NULL;

    DMUS_OBJECTDESC dmod = {0};
    CHAR szPath[MAX_PATH];
    CHAR szFile[MAX_PATH];


    //Create the loader.
    CHECKRUN(dmthCreateLoader(CTIID_IDirectMusicLoader, &ptLoader));
    CHECKRUN(ChopPath(szFileName, szPath, szFile));
    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes , szPath, FALSE));

    
    //Create a stream from this file.
    AllocStreamFromFile(&pStream, szFileName, 0);
    if (NULL == pStream)
    {
        Log(ABORTLOGLEVEL, "AllocStreamFromFile(%s) returned %08X (%s)", szFileName, hr, tdmXlatHRESULT(hr));
        hr = E_FAIL;
    }

    //Refcount should be one.
//    VerifyRefCount("stream before loaded or playing", pStream, 1);
    
    //Set up my DMOD
    dmod.dwSize = sizeof(dmod);
    dmod.dwValidData = DMUS_OBJ_STREAM | DMUS_OBJ_CLASS;
    dmod.pStream = pStream;
    memcpy(&dmod.guidClass, (GUID *)&CLSID_DirectMusicSegment, sizeof(GUID));

    //Get the segment from the stream.
    CHECKRUN(ptLoader->GetObject(&dmod, IID_IDirectMusicSegment, (void **)&pSegment));
    CHECKRUN(dmthCreateTestWrappedObject(pSegment, &ptSegment));

    //Get the clone of this stream.
    //CHECKRUN(((CFileStream *)(pStream))->GetMyClone(&pClone));

    //TODO: replace pStream w/pClone
    //CHECKRUN(VerifyRefCount("clone stream while loaded but before or playing", pClone, 1));

    //Play the segment on an audiopath.
    CHECKRUN(ptPerf8->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREO, 128, TRUE, &ptPath));
    CHECKRUN(ptPerf8->PlaySegmentEx(ptSegment, NULL, NULL, 0, 0, NULL, NULL, ptPath));

    //Refcount should be 2 while the clone stream is playing.
    //CHECKRUN(VerifyRefCount("clone stream while playing", pClone, 1));

    //Stop the segment
    CHECKRUN(ptPerf8->StopEx(ptSegment, 0, 0));
    CHECKRUN(ExpectSegmentStop(ptPerf8, ptSegment, NULL, 5000, NULL, FALSE));
    
    //
    //CHECKRUN(VerifyRefCount("clone stream after playing but still loaded", pClone, 1));

    //Free the loader: Refcount should be one.
    RELEASE(ptLoader);
    //CHECKRUN(VerifyRefCount("clone stream after playing and unloaded", pClone, 1));

//    CHECKRUN(VerifyRefCount("stream after playing and unloaded", pStream, 1));

    RELEASE(ptPath);
    RELEASE(ptSegment);
    RELEASE(pSegment);
    RELEASE(pStream);

    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Loader_GetObject_IStream_Test(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
MEDIAFILEDESC MediaFileDesc[] = {
                                {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                {TERMINATE}
                                };
LPCSTR szFileName = "T:\\DMTest1\\test.sgt";

CHECKRUN(LoadMedia(MediaFileDesc));
CHECKRUN(Loader_GetObject_IStream_Generic(ptPerf8, szFileName));
return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Loader_GetObject_IStream_Streaming(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
HRESULT hr = S_OK;
LPCSTR szFilename = "WAV/Streaming10s.wav";
CHECKRUN(MediaCopyFile(szFilename));
CHECKRUN(Loader_GetObject_IStream_Generic(ptPerf8, MediaServerToLocal(szFilename)));
return hr;
};


