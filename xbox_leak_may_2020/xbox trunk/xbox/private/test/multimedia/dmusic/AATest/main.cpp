/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

Environment:

    Xbox

Revision History:
    
    Person      Date        Change      
    ------      ----        ------
    DANHAFF     11/29/01    Created

--*/

#include "..\globals.h"
#include "..\helpers.h"

//This is currently determined by the content in FILELIST.CPP, but hopefully we
//  can soon get it from the app's name.
extern char *g_szAppName;

LPSTR g_szContentDir                = "T:\\AudAnalyze\\Content\\";
CHAR  g_szContentList[MAX_PATH]     = {0};
LPSTR g_szFileListDir               = "DMusic/AudAnalyze/FileLists/";
CHAR  g_szFileList[MAX_PATH]        = {0};

IDirectMusicLoader      *g_pLoader  = NULL;
IDirectSound            *g_pDSound  = NULL;
IDirectMusicAudioPath   *g_pPath    = NULL;
IDirectMusicPerformance *g_pPerf    = NULL;

#include "obj\i386\createinstance.cpp"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT InitializeDSound(void)
{
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc = {0};
    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;
    CHECKRUN(DirectSoundCreate( NULL, &g_pDSound, NULL ));
    CHECKRUN(XAudioDownloadEffectsImage("dsstdfx", &EffectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc));
    CHECK(DirectSoundUseLightHRTF());
    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT InitializeDMusic(void)
{
    HRESULT hr = S_OK;
    
    //Create objects.
    //CHECKRUN(DirectMusicInitializeFixedSizeHeaps(30 * 1024 * 1024, 2 * 1024 * 1024, MyFactory));
    IDirectMusicHeap *pNorm = NULL;
    IDirectMusicHeap *pPhys = NULL;

    CHECKRUN(DirectMusicCreateDefaultHeap(&pNorm));
    CHECKRUN(DirectMusicCreateDefaultPhysicalHeap(&pPhys));
    CHECKRUN(DirectMusicInitializeEx(pNorm, pPhys, MyFactory));
    CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&g_pLoader));       
    CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance, (void **)&g_pPerf));       

    //Initialize objects.
    CHECKRUN(g_pPerf->InitAudioX(0, 128, 128, 0));
    CHECKRUN(g_pPerf->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, 128, &g_pPath));
    CHECKRUN(g_pPerf->SetDefaultAudioPath(g_pPath));
    return hr;

};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT InitializeOther(void)
{
    HRESULT hr = S_OK;

    return hr;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT GetFileList(MEDIAFILEDESC **ppMediaFileDesc)
{
    HRESULT hr = S_OK;
    MEDIAFILEDESC *pMediaFileDesc = NULL;

        //First get the file list.  We know where it is.        
        {
            MEDIAFILEDESC MediaFileDesc[2] = {0};
            sprintf(g_szFileList,   "%s%s.txt", g_szFileListDir, APPNAME);
            sprintf(g_szContentList,"%s%s.txt", g_szContentDir, APPNAME);
            MediaFileDesc[0].lpszFromPath       = g_szFileList;
            MediaFileDesc[0].lpszToPath         = g_szContentDir;
            MediaFileDesc[0].dwFlags            = COPY_IF_NEWER;
        
            //Copy the file down.
            CHECKRUN(LoadMedia(MediaFileDesc))
        }
        
        //Parse the file.
        if (SUCCEEDED(hr))
        {
            CHAR szLineOText [MAX_PATH] = {0};
            CHAR szSrcFile   [MAX_PATH] = {0};
            CHAR szDstSubPath[MAX_PATH] = {0};
            CHAR szDstPath   [MAX_PATH] = {0};
            BOOL bJustCounting;
            DWORD dwLines = 0;
            DWORD dwCurrent = 0;


            for (LONG bJustCounting=1; bJustCounting>=0; bJustCounting--)
            {
                FILE *fyle = fopen(g_szContentList, "rt");
                CHECKALLOC(fyle);                
                
                if (!bJustCounting)
                {
                    pMediaFileDesc = new MEDIAFILEDESC[dwLines + 1];
                    CHECKALLOC(pMediaFileDesc);
                    CHECK     (ZeroMemory(pMediaFileDesc, sizeof(MEDIAFILEDESC) * (dwLines + 1)));
                }

                while (fgets(szLineOText, MAX_PATH - 1, fyle) && SUCCEEDED(hr))
                {
                    //Bail if it's a comment.
                    if (szLineOText[0] == ';')
                        continue;

                    //Bail if we didn't get two items.
                    DWORD dwScanned = sscanf(szLineOText, "%[^,], %[^\n]",szSrcFile,szDstSubPath);
                    if (2 != dwScanned)
                        continue;

                    //Either count the lines or fill in the respective Media
                    if (bJustCounting)
                    {
                        dwLines++;
                    }
                    else
                    {
                        sprintf(szDstPath, "T:\\%s", szDstSubPath);


                        //Copy this into the destination path and replace '\' with '/'
                        pMediaFileDesc[dwCurrent].lpszFromPath   = new CHAR[strlen(szSrcFile)+1];
                        strcpy(pMediaFileDesc[dwCurrent].lpszFromPath,szSrcFile);                        
                        for (LPSTR szTemp = pMediaFileDesc[dwCurrent].lpszFromPath; *szTemp; szTemp++)
                        {
                            if (*szTemp == '\\')
                                *szTemp = '/';
                                
                        }


                        pMediaFileDesc[dwCurrent].lpszToPath     = new CHAR[strlen(szDstPath)+1];
                        strcpy(pMediaFileDesc[dwCurrent].lpszToPath,szDstPath);
                        pMediaFileDesc[dwCurrent].dwFlags        = COPY_IF_NEWER;
                        dwCurrent++;

                    }


                }

                //Close the file (reset it).
                if (fyle)
                {
                    fclose(fyle);
                    fyle = NULL;
                }
            }
        }

    *ppMediaFileDesc = pMediaFileDesc;
    return hr;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT LoadAndPlaySegments(MEDIAFILEDESC *pMediaFileDesc, DWORD dwTimeLimit)
{
//Run through the list, get the local segment names, and play them.
HRESULT hr = S_OK;
DWORD i = 0;
IDirectMusicSegment *pSegment   = NULL;
IDirectMusicScript  *pScript    = NULL;


for (i=0; pMediaFileDesc[i].lpszFromPath && pMediaFileDesc[i].lpszToPath; i++)
{
    CHAR szFilePath     [MAX_PATH] = {0};
    CHAR szFileName     [MAX_PATH] = {0};

    
    strcpy(szFileName, PathFindFileName(pMediaFileDesc[i].lpszFromPath));
    sprintf(szFilePath, "%s%s", pMediaFileDesc[i].lpszToPath, szFileName);
    _strlwr(szFilePath);

    if (strstr(szFileName, ".sgt") || strstr(szFileName, ".wav"))
    {
    
        CHECKRUN(g_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, pMediaFileDesc[i].lpszToPath, FALSE));
        CHECKRUN(g_pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFileName, (void **)&pSegment));
        CHECKRUN(g_pPerf->PlaySegmentEx(pSegment, 0, 0, 0, 0, 0, 0, g_pPath));
        CHECK   (Sleep(dwTimeLimit));
        CHECKRUN(g_pPerf->StopEx(0, 0, 0));
        RELEASE(pSegment);
    }
    else if (strstr(szFileName, ".spt"))
    {
        CHECKRUN(g_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, pMediaFileDesc[i].lpszToPath, FALSE));
        CHECKRUN(g_pLoader->LoadObjectFromFile(CLSID_DirectMusicScript, IID_IDirectMusicScript, szFileName, (void **)&pScript));
        CHECKRUN(pScript->Init(g_pPerf, NULL));

        //BUGBUG: Take this out.

        CHECKRUN(pScript->CallRoutine("Routine511c0", NULL));
        Sleep(5000);
        CHECKRUN(pScript->CallRoutine("Routine511c1", NULL));
        Sleep(5000);
        CHECKRUN(pScript->CallRoutine("Routine511c2", NULL));
        RELEASE(pScript);

    }

    if (FAILED(hr))
    {
        DbgPrint("Error on %s", szFileName);
        hr = S_OK;
    }

}
return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT FreeFileList(MEDIAFILEDESC **ppMediaFileDesc)
{
    delete [](*ppMediaFileDesc);
    *ppMediaFileDesc = NULL;
    return S_OK;
}






//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int __cdecl main()
{
    HRESULT hr = S_OK;
    MEDIAFILEDESC *pMediaFileDesc = NULL;

    CHECKRUN(InitializeDSound());
    CHECKRUN(InitializeDMusic());
    CHECKRUN(InitializeOther());
    CHECKRUN(GetFileList(&pMediaFileDesc));
    CHECKRUN(LoadMedia(pMediaFileDesc));    
    CHECKRUN(LoadAndPlaySegments(pMediaFileDesc, 15000));
    CHECKRUN(FreeFileList(&pMediaFileDesc));
    DbgPrint("App has finished");
    while (1);
    return 0;
}
