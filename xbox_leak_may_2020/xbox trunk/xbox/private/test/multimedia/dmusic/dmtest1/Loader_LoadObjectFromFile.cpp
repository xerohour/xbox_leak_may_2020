#include "globals.h"
#include "cicmusicx.h"


HRESULT Loader_LoadObjectFromFile_Valid_RepeatLoadScript(CtIDirectMusicPerformance *ptPerf8);

/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_BVT (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling BVT test function Loader_LoadObjectFromFile())");
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Valid (CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling valid test function Loader_LoadObjectFromFile())");
    DMTEST_EXECUTE(Loader_LoadObjectFromFile_Valid_RepeatLoadScript(ptPerf8));
    return hr;
};


/********************************************************************************
Valid test function.
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Valid_AbsolutePath(CtIDirectMusicPerformance8* ptPerf8, DWORD dwFilename, DWORD dwUnused2)
{
    HRESULT hr = S_OK;
    CHAR *szServerFilename = (CHAR *)dwFilename;
    CHAR szFilename[MAX_PATH] = {0};
    CHAR szPath[MAX_PATH] = {0};
    CtIDirectMusicLoader *ptLoader = NULL;
    IUnknown *pUnk = NULL;
    GUID *pCLSID = NULL;
    GUID *pIID = NULL;

    
    CHECKRUN(MediaCopyFile(szServerFilename));
    strcpy(szFilename, MediaServerToLocal(szServerFilename));
    CHECKRUN(ChopPath(szFilename, szPath, NULL));
    CHECKRUN(GUIDsFromFileName(szFilename, &pCLSID, &pIID));
    CHECKRUN(dmthCreateLoader(CTIID_IDirectMusicLoader, &ptLoader));

    CHECKRUN(ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szPath, TRUE));
    CHECKRUN(ptLoader->LoadObjectFromFile(*pCLSID, *pIID, szFilename, (void **)&pUnk));
    SAFE_RELEASE(pUnk);
    SAFE_RELEASE(ptLoader);
    return hr;
};




/********************************************************************************
BVT test function.
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Perf (CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT hr = S_OK;
    Log(FYILOGLEVEL, "Calling Perf test function Loader_LoadObjectFromFile_Perf())");
    return hr;
};



/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Valid_RepeatLoadScript(CtIDirectMusicPerformance *ptPerf8)
{
    HRESULT hr = S_OK;  
    CtIDirectMusicScript* pScript = NULL;
    CtIDirectMusicLoader8* pLoader = NULL;
    LPSTR szFilename = "DMusic/DMTest1/script/variables.spt";
    CHAR szDirectory[MAX_PATH] = {0};
    int i=0; 


    CHECKRUN(MediaCopyFile(szFilename));
    for (i=0; i<1000 && SUCCEEDED(hr); i++)
    {
        CHECKRUN( dmthCreateLoader( IID_IDirectMusicLoader8, &pLoader ) );
        CHECKRUN( ChopPath(MediaServerToLocal(szFilename), szDirectory, NULL));
        CHECKRUN( pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szDirectory, TRUE));
        CHECKALLOC( pLoader );
        CHECKRUN( dmthCreateScript( MediaServerToLocal(szFilename), pLoader, &pScript ) );
        CHECKALLOC( pScript );
        SAFE_RELEASE( pScript );
        SAFE_RELEASE( pLoader );
        if (FAILED(hr))
        {
            Log(ABORTLOGLEVEL, "Loading the script failed on iteration %d", i);
            break;
        }
    }

    return hr;

}

/********************************************************************************
********************************************************************************/
HRESULT Loader8_LoadObjectTwice(CtIDirectMusicPerformance8* ptPerf8)
{
    HRESULT                     hr              = 0;
    HRESULT                     hrCall          = S_OK;
    CtIDirectMusicSegment8      *ptSegment      = NULL;

    MEDIAFILEDESC MediaFileDesc[] = {
                                    {"SGT/test.sgt",      "T:\\DMTest1",    COPY_IF_NEWER},
                                    {"DLS/Main1.DLS",     "T:\\DMTest1",    COPY_IF_NEWER},
                                    {TERMINATE}
                                    };

    CHAR                        *szSegmentFile  = "T:\\DMTest1\\test.sgt";
    CHAR                        *szSegmentPath  = "T:\\DMTest1";
    CtIDirectMusicLoader8       *ptLoader8      = NULL;
    DWORD i;

    CHECKRUN(LoadMedia(MediaFileDesc));

    // **********************************************************************
    // 1) Create CtIDirectMusicLoader object (wrapped IDirectMusicLoader object)
    // **********************************************************************
    hr = dmthCreateLoader(IID_IDirectMusicLoader8, &ptLoader8);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: CoCreate Loader object ")
                TEXT("failed (%s == %08Xh)"),
                tdmXlatHRESULT(hr), hr);
        goto TEST_END;
    }

    hr = ptLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes, szSegmentPath, FALSE);
    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, TEXT("**** ABORT: Loader->SetSeachDirectory(media directory) ")
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

//    strcpy(desc.wszFileName, szSegmentFile);
    for (i=0; i<2; i++)
    {
        hr = ptLoader8->GetObject(&desc, CTIID_IDirectMusicSegment, (void **)&ptSegment);
        Log(ABORTLOGLEVEL, "dmthLoadSegment(%s) returned %s (%08Xh)", szSegmentFile, tdmXlatHRESULT(hr), hr);
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


enum LOADTYPE {BYFILENAME, BYNAME, BYGUID};

HRESULT Loader_LoadObjectFromFile_Container(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwName, LOADTYPE eLoadType, DWORD dwUseScript);



/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByFileName(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwFileName)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwFileName, BYFILENAME, FALSE);
};

/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByName(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwName)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwName, BYNAME, FALSE);
};

/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByGUID(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwGUID)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwGUID, BYGUID, FALSE);
};

/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByFileName_Script(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwFileName)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwFileName, BYFILENAME, TRUE);
};

/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByName_Script(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwName)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwName, BYNAME, TRUE);
};

/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Container_ByGUID_Script(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwGUID)
{
    return Loader_LoadObjectFromFile_Container(ptPerf8, bWave, dwGUID, BYGUID, TRUE);
};



/********************************************************************************
Note: Search Path is hard-coded.
********************************************************************************/
#define COUNT 1
#define TESTS 8

//#define SCRIPT
HRESULT Loader_LoadObjectFromFile_Container(CtIDirectMusicPerformance8* ptPerf8, DWORD bWave, DWORD dwName, LOADTYPE eLoadType, DWORD dwUseScript)
{
CtIDirectMusicSegment* ptSegment = NULL;
CtIDirectMusicLoader * ptLoader = NULL;
CtIUnknown *ptUnk               = NULL;
DWORD dwStartTime = 0, dwTotalTime = 0;
HRESULT hr = S_OK;
DWORD dwIndex = 0;
BOOL bLoadFromContainer = 0xFFFFFFFF;
LPSTR szName  = (LPSTR)dwName;
DWORD dwTimes[TESTS] = {0};
GUID guidObject = {NULL};
LPSTR szContLocationServer = "DMusic/DMTest1/Loader/Cont01/Cont/Cont01.con";
LPSTR szScrLocationServer  = "DMusic/DMTest1/Loader/Cont01/Cont/Cont01.spt";
LPSTR szFilesPathServer    = "DMusic/DMTest1/Loader/Cont01/Files/";
CHAR szContLocation[MAX_PATH] = {0};
CHAR szScrLocation [MAX_PATH] = {0};
CHAR szFilesPath   [MAX_PATH] = {0};
LPSTR szContainer = NULL;
BOOL bFailed = FALSE;

//Copy the directory containing these files.
CHECKRUN(MediaCopyDirectory(szContLocationServer));
CHECKRUN(MediaCopyDirectory(szScrLocationServer));
CHECKRUN(MediaCopyDirectory(szFilesPathServer));
strcpy(szContLocation,  MediaServerToLocal(szContLocationServer));
strcpy(szScrLocation,   MediaServerToLocal(szScrLocationServer));
strcpy(szFilesPath,     MediaServerToLocal(szFilesPathServer));

//Define the location of the container based on whether we're using the script or the container.
if (dwUseScript)
    szContainer = szScrLocation;
else
    szContainer = szContLocation;

for (dwIndex = 0; dwIndex < TESTS && SUCCEEDED(hr); dwIndex++)
{

    //Create a loader.
    CHECKRUN(dmthCreateLoader(IID_IDirectMusicLoader, &ptLoader));

    //bLoadFromContainer = !(dwIndex % 2);
    bLoadFromContainer = (dwIndex % 2);
    
    //If we're not loading by filename, we can't load directly from the disk.
    if ((BYGUID == eLoadType || BYNAME == eLoadType) && !bLoadFromContainer)
    {
        RELEASE(ptLoader);
        continue;
    }

    //Cache the files in the container.
    if (bLoadFromContainer)
    {
        if (dwUseScript)
        {
            CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicScript, CTIID_IUnknown, szContainer, (void **)&ptUnk));
        }
        else
        {
            CHECKRUN(ptLoader->LoadObjectFromFile(CLSID_DirectMusicContainer, CTIID_IUnknown, szContainer, (void **)&ptUnk));
        }
    }
    //Load and free the object MAX times.
    if (SUCCEEDED(hr))
    {
        DMUS_OBJECTDESC dmod = {0};
        dmod.dwSize = sizeof(dmod);
        dmod.dwValidData = DMUS_OBJ_CLASS;
        
        if (0)
            memcpy((void *)&dmod.guidClass, &CLSID_DirectSoundWave, sizeof(GUID));
        else
            memcpy((void *)&dmod.guidClass, &CLSID_DirectMusicSegment, sizeof(GUID));

        if (BYFILENAME == eLoadType)
        {
            mbstowcs(dmod.wszFileName, szName, MAX_PATH);
            dmod.dwValidData |= DMUS_OBJ_FILENAME;
        }
        else if (BYNAME == eLoadType)
        {
            mbstowcs(dmod.wszName, szName, MAX_PATH);
            dmod.dwValidData |= DMUS_OBJ_NAME;
        }
        else if (BYGUID == eLoadType)
        {                
            CHECKRUN(StringToGuid(szName, &dmod.guidObject));
            dmod.dwValidData |= DMUS_OBJ_OBJECT;
        }
        else
        {
            Log(ABORTLOGLEVEL, "Test App Error!!!  eLoadType = %d", eLoadType);
        }
        //If we haven't loaded from a container, then point ourselves to the directory.
        if (!bLoadFromContainer)
        {
            hr = ptLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szFilesPath, TRUE);
        }
        dwStartTime = timeGetTime();
        for (DWORD i=0; i<COUNT && SUCCEEDED(hr); i++)
        {
            if (SUCCEEDED(hr))
            {

//                if (bWave)
//                {
//                    //1) Call GetObject() with IID_IDirectMusicObject, then call GetDescriptor.  If this descriptor has a valid pStream pointer,
//                    //   then try calling SetObject with the same descriptor with the guidClass set to CLSID_DirectMusicSegment.
//                    Log(FYILOGLEVEL, "Since this is a WAVE object, we're gonna try James's trick of calling SetObject with CLSID_DirectMusicSegment");
//                    CtIDirectMusicObject *ptObject = NULL;
//                    DMUS_OBJECTDESC dmodObject = {0};
//                    
//                    //Copy the original objectdesc
//                    dmodObject = dmod;
//                    
//                    CHECKRUN(ptLoader->GetObject(&dmodObject, CTIID_IDirectMusicObject, (void **)&ptObject));
//                    if (FAILED(hr))
//                    {
//                        Log(ABORTLOGLEVEL, "GetObject on the wave for an IID_IDirectMusicObject interface failed with a %s (%08X)", tdmXlatHRESULT(hr), hr);
//                    }
//                    CHECKRUN(ptObject->GetDescriptor(&dmodObject));
//                    if (dmodObject.pStream)
//                    {
//                        memcpy((void *)&dmodObject.guidClass, &CLSID_DirectMusicSegment, sizeof(GUID));
//                        CHECKRUN(ptLoader->SetObject(&dmodObject));
//                    }
//                    else
//                    {
//                        Log(ABORTLOGLEVEL, "The Object Descriptor obtained from the loader had a NULL pStream");
//                        hr = E_FAIL;
//                    }
//                }
                CHECKRUN(ptLoader->GetObject(&dmod, CTIID_IDirectMusicSegment, (void **)&ptSegment));
                if (FAILED(hr))
                {
                    Log(ABORTLOGLEVEL, "GetObject from %s for object %s returned %s (%08X)", bLoadFromContainer ? "container" : "disk", szName, tdmXlatHRESULT(hr), hr);                    
                }
            }            
            RELEASE(ptSegment);
        }
        dwTotalTime = timeGetTime() - dwStartTime;
    }
    
    //store and print the timing info.
    if (SUCCEEDED(hr))
    {
        dwTimes[dwIndex] = dwTotalTime;
        Log(FYILOGLEVEL, "Took %u ms to load %s %u times from %s", dwTimes[dwIndex], szName, COUNT, bLoadFromContainer ? "container" : "disk");
    }
    else
    {
        bFailed = TRUE;
        hr = S_OK;
    }

    //If we loaded the container, release it.
    if (bLoadFromContainer)
    {
        CHECKRUN(ptLoader->ReleaseObjectByUnknown(ptUnk));
        RELEASE(ptUnk);
    }

    //Always clear the cache before we try the next method of loading.
//    CHECKRUN(ptLoader->ClearCache(GUID_DirectMusicAllTypes));
    RELEASE(ptLoader);

}

RELEASE(ptUnk);     //release container
RELEASE(ptSegment); //release segment
RELEASE(ptLoader);  //release loader

if (SUCCEEDED(hr))
    if (bFailed)
        hr = E_FAIL;

return hr;
};
#undef COUNT


/********************************************************************************
********************************************************************************/
HRESULT Loader_LoadObjectFromFile_Valid_Bug7707(CtIDirectMusicPerformance8* ptPerf8, DWORD dwUnused1, DWORD dwUnused2)
{   
    HRESULT hr = S_OK;
    CHECKRUN(MediaCopyDirectory("DMusic/DMTest1/Loader/Bug7707/"));
    CHECKRUN(Loader_LoadObjectFromFile_Valid_AbsolutePath(ptPerf8, (DWORD)"DMusic/DMTest1/Loader/Bug7707/Segment1.sgt", 0));
    return hr;
};
