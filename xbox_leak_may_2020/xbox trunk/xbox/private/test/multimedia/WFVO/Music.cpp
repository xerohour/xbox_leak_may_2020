/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Music.cpp

Abstract:

	Music Playback engine for stress

Author:

	Dan Haffner(danhaff) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	29-Apr-2001 danhaff
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "media.h"
#include "macros.h"
#include "music.h"
#include "helpers.h"
#include "scene.h"
#include "MusicHeap.h"

using namespace WFVO;

namespace WFVO {

extern CMusic g_music;
extern CScene g_scene;

struct DATA
{
  LPSTR *ppszFiles;
  DWORD *pdwMaxFiles;
};



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT LoadFileCallBack(LPSTR szFullPath, LPVOID pVoid)
{
DATA *pData = (DATA *)pVoid;


    if (strstr(szFullPath, ".wav") || strstr(szFullPath, ".sgt"))
    {
        pData->ppszFiles[*(pData->pdwMaxFiles)] = new CHAR[strlen(szFullPath) + 1];
        strcpy(pData->ppszFiles[*(pData->pdwMaxFiles)], szFullPath);
        (*(pData->pdwMaxFiles))++;
        if (*(pData->pdwMaxFiles) == FILES)
        {
            Log("ERROR!!  With %s, we reached the limit of %d files!!\n", szFullPath, FILES);
            return FALSE;
        }
    }
    return S_OK;
}


#define FILES 5000
void RandomizeFiles(LPSTR *ppszFileNames, DWORD dwCount)
{
LPSTR szTemp = NULL;
DWORD i,j;

    //Go through and swap with a random slot.
    for (i=0; i<dwCount; i++)
    {
        j = rand() % dwCount;

        //swap
        szTemp = ppszFileNames[i];
        ppszFileNames[i] = ppszFileNames[j];
        ppszFileNames[j] = szTemp;
    }
};



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CMusic::CMusic(void)
{
m_pPerf = NULL;
m_pPath = NULL;
m_pSegmentCurrent = NULL;
m_pSegmentNext = NULL;
m_pHeapNormal = NULL;
m_pHeapPhysical = NULL;
m_pLoader = NULL;

m_dwMaxFiles= 0;
m_dwCurrentFile = 0;
m_eState = eWaiting;
m_hThreadLoad = NULL;
m_hThreadTransition = NULL;
m_hrThreadProcReturn = S_OK;
m_szSegmentTitleCurrent[0] = 0;
m_szSegmentTitleNext[0] = 0;
m_dwBeat = 0;
m_bNextSegment = FALSE;

m_dwReverb = 0xFFFFFFFF;
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CMusic::~CMusic(void)
{
DWORD i = 0;

    //Wait for threadproc to shut down if we're in one.
    if (eLoading == m_eState)
    {    
        Log("CMusic destruktor: waiting for threadproc -o- content loading to shut down\n");
        WaitForThreadEnd(&m_hThreadLoad);
    }

    //Wait for threadproc to shut down if we're in one.
    if (eTransitioning == m_eState)
    {    
        Log("CMusic destruktor: waiting for threadproc -o- transitioning to shut down\n");
        WaitForThreadEnd(&m_hThreadTransition);
    }



    RELEASE(m_pPath);
    RELEASE(m_pSegmentCurrent);
    RELEASE(m_pSegmentNext);
    RELEASE(m_pSegmentGMReset);
    RELEASE(m_pLoader);
    if (m_pPerf)
    {
        m_pPerf->AddNotificationType(GUID_NOTIFICATION_MEASUREANDBEAT);
        m_pPerf->CloseDown();
        RELEASE(m_pPerf);
    }

    RELEASE(m_pHeapNormal);
    RELEASE(m_pHeapPhysical);

    for (i=0; i<m_dwMaxFiles; i++)
    {
        if (m_ppszFiles[i])
        {
            delete []m_ppszFiles[i];
            m_ppszFiles[i] = NULL;
        }
        else
            Log("ERROR!!  Deleting string of m_ppszFiles[%d] but it was already NULL!\n", i);
    }

};





//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::Initialize(void)
{
HRESULT hr = S_OK;
DATA Data = {0};
DWORD i = 0;
IUnknown *pUnkConfig = NULL;

if (!g_TestParams.bDMusic)
    return S_OK;

//Create the heaps
if (_strcmpi(g_TestParams.szDMHeap, "Default") == 0)
{
    CHECKRUN(DirectMusicInitialize());
}
else if (_strcmpi(g_TestParams.szDMHeap, "Tracking") == 0)
{
    CHECKRUN(CreateMusicHeapTrack(&m_pHeapNormal,   FALSE));
    CHECKRUN(CreateMusicHeapTrack(&m_pHeapPhysical, TRUE));
    CHECKRUN(DirectMusicInitializeEx(m_pHeapNormal, m_pHeapPhysical, DirectMusicDefaultFactory));
}
else if (_strcmpi(g_TestParams.szDMHeap, "FixedUser") == 0)
{
    CHECKRUN(CreateMusicHeapFixed(&m_pHeapNormal,   FALSE, g_TestParams.dwDMFixedHeapSizeNorm));
    CHECKRUN(CreateMusicHeapFixed(&m_pHeapPhysical, TRUE,  g_TestParams.dwDMFixedHeapSizePhys));
    CHECKRUN(DirectMusicInitializeEx(m_pHeapNormal, m_pHeapPhysical, DirectMusicDefaultFactory));
}
else if (_strcmpi(g_TestParams.szDMHeap, "FixedDefault") == 0)
{
    IDirectMusicHeap *pDirectMusicHeapNormal   = NULL;
    IDirectMusicHeap *pDirectMusicHeapPhysical = NULL;
    
    CHECKRUN(DirectMusicCreateFixedSizeHeap        (g_TestParams.dwDMFixedHeapSizeNorm, &pDirectMusicHeapNormal));
    CHECKRUN(DirectMusicCreateFixedSizePhysicalHeap(g_TestParams.dwDMFixedHeapSizePhys, &pDirectMusicHeapPhysical));
    CHECKRUN(DirectMusicInitializeEx(pDirectMusicHeapNormal, pDirectMusicHeapPhysical, DirectMusicDefaultFactory));
    RELEASE(pDirectMusicHeapNormal);
    RELEASE(pDirectMusicHeapPhysical);
}
else if (_strcmpi(g_TestParams.szDMHeap, "FixedCache") == 0)
{
    CHECKRUN(CreateMusicHeapFixedCache(&m_pHeapNormal,   FALSE, g_TestParams.dwDMFixedHeapSizeNorm));
    CHECKRUN(CreateMusicHeapFixedCache(&m_pHeapPhysical, TRUE,  g_TestParams.dwDMFixedHeapSizePhys));
    CHECKRUN(DirectMusicInitializeEx(m_pHeapNormal, m_pHeapPhysical, DirectMusicDefaultFactory));
}

else
{
    Log("ERROR: Invalid value %s for g_TestParams.szDMHeap\n", g_TestParams.szDMHeap);
}


//Create the loader and set GM.DLS on it if requested by user.
CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicLoader, NULL, IID_IDirectMusicLoader, (void **)&m_pLoader));
if (g_TestParams.bDMUseGM)
{
	DMUS_OBJECTDESC DESC;					
    CHAR szTemp[MAX_PATH] = {0};

	memset( &DESC, 0, sizeof(DMUS_OBJECTDESC) );
	DESC.dwSize = sizeof (DMUS_OBJECTDESC);
	DESC.guidClass = CLSID_DirectMusicCollection;  
	        
    sprintf(szTemp, "%s\\gm.dls", g_TestParams.szDMSupportDir);
    mbstowcs(DESC.wszFileName, szTemp, strlen(szTemp));

	DESC.guidObject = GUID_DefaultGMCollection;
	DESC.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FULLPATH | DMUS_OBJ_OBJECT;
	hr = m_pLoader->SetObject(&DESC);
    if(FAILED(hr))
    {
        Log("SetObject on GM.DLS object returned %08Xh; continuing to load anyway\n",hr);
        hr = S_OK;
    }
}


CHECKRUN(DirectMusicCreateInstance(CLSID_DirectMusicPerformance, NULL, IID_IDirectMusicPerformance, (void **)&m_pPerf));
CHECK   (DirectMusicSetDebugLevel(g_TestParams.dwDMDebugLevel, g_TestParams.dwDMRIPLevel));
CHECKRUN(m_pPerf->InitAudioX(0, 0, g_TestParams.dwDMVoiceCount, g_TestParams.bDMDoWorkLocally ? DMUS_INITAUDIO_NOTHREADS : 0));


for (i=0; i<NUMELEMS(AudioPathPairs); i++)
{
    if (_strcmpi((const char *)AudioPathPairs[i].szString, g_TestParams.szDMAudioPath)==0)
    {
        CHECKRUN(m_pPerf->CreateStandardAudioPath(AudioPathPairs[i].dwValue, 128, TRUE, &m_pPath));
        break;
    }
}

//Load up the GMReset segment for future use.
CHAR szTemp[MAX_PATH] = {0};
CHECK(sprintf(szTemp, "%s\\GMReset.sgt", g_TestParams.szDMSupportDir));
CHECKRUN(m_pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szTemp, (void **)&m_pSegmentGMReset));

g_music.SetMasterVolume(g_TestParams.lDMMasterVolume);
g_music.SetMasterTempo(g_TestParams.fDMMasterTempo);
g_music.SetMasterTranspose(g_TestParams.lDMMasterTranspose);

/*
TODO: LOAD UP YOUR AUDIOPATH CONFIG FILE HERE!!
if (!m_pPath)
{
CHECKRUN(
CHECKRUN(m_pPerf->CreateAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 128, TRUE, &m_pPath));
*/


//After we're done, g_szFiles will contain a list of files, and g_dwFiles will contain the number.
m_dwMaxFiles = 0;
m_dwCurrentFile = 0;

//Use the segment list if provided, otherwise RecurseDirectory.
if (g_TestParams.dwDMSegments)
{
    for (DWORD i=0; i<g_TestParams.dwDMSegments; i++)
    {
        m_ppszFiles[i]= new CHAR[strlen(g_TestParams.ppDMSegments[i] + 1)];
        strcpy(m_ppszFiles[i], g_TestParams.ppDMSegments[i]);
    }
    m_dwMaxFiles = g_TestParams.dwDMSegments;
}
else
{
    Data.pdwMaxFiles = &m_dwMaxFiles;
    Data.ppszFiles = m_ppszFiles;
    CHECKRUN(RecurseDirectory(g_TestParams.szDMStressDir, LoadFileCallBack, (void *)&Data));
    CHECK   (RandomizeFiles(m_ppszFiles, m_dwMaxFiles));
}

LoadNextSegment();


//Make it notify us of the beats.
m_pPerf->AddNotificationType(GUID_NOTIFICATION_MEASUREANDBEAT);

return hr;
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
DWORD TransitionSegmentsThreadProc(void *pvMusic)
{
CMusic *pMusic = (CMusic *)pvMusic;
HRESULT hr = S_OK;
HRESULT hrTimeOut = S_OK;
FLOAT fGMResetLength;
DWORD dwTimeOuts = 0;
    
    //Stop the current segment (it may be stopped already but who cares).  Release it.
    if (pMusic->m_pSegmentCurrent)
    {
        CHECKRUN(pMusic->m_pPerf->StopEx(pMusic->m_pSegmentCurrent, 0, 0));
        if (FAILED(hr))
        {
            Log("StopEx() failed on %s; returned %08X", pMusic->m_pSegmentCurrent, hr);
        }
/*
        CHECKRUN(pMusic->m_pSegmentCurrent->Unload(pMusic->m_pPerf));
        if (FAILED(hr))
        {
            Log("Unload() failed on %s; returned %08X", pMusic->m_pSegmentCurrent, hr);
        }
*/
        CHECKRUN(pMusic->m_pLoader->ReleaseObjectByUnknown(pMusic->m_pSegmentCurrent));
        RELEASE(pMusic->m_pSegmentCurrent);
    }

    //Play the GM Reset segment as a secondary segment.  After that, set the transpose back to where it was.
    CHECKRUN(pMusic->m_pPerf->PlaySegmentEx(pMusic->m_pSegmentGMReset, NULL, NULL, 0, 0, NULL, NULL, pMusic->m_pPath));
    if (FAILED(hr))
    {
        Log("GMReset failed to start: PlaySegmentEx returned %08X", hr);
    }
    
    //Wait for it to start, or 1000ms, whichever comes first.
    hrTimeOut = WaitForSegmentStart(pMusic->m_pPerf, pMusic->m_pSegmentGMReset, NULL, 1000);
    if (FAILED(hrTimeOut))
        Log("Timed out waiting 1 second for GM Reset segment to start.\n");

    
    //This segment is 0.25 seconds long at tempo factor 1.  We should divide this number by the tempo factor, and add a fudge factor of (5x + 1s)
    fGMResetLength = (250.f  * 5.f + 1.f) / g_TestParams.fDMMasterTempo;
    
    hrTimeOut = WaitForSegmentStop(pMusic->m_pPerf, pMusic->m_pSegmentGMReset, NULL, DWORD(fGMResetLength));
    if (FAILED(hrTimeOut))
        Log("Timed out waiting %d ms for GM Reset segment to stop.\n", DWORD(fGMResetLength));

    //If the next segment exists, Make it the current one and play it.
    if (pMusic->m_pSegmentNext)
    {
        pMusic->m_pSegmentCurrent = pMusic->m_pSegmentNext;
        strcpy(pMusic->m_szSegmentTitleCurrent, pMusic->m_szSegmentTitleNext);
        CHECKRUN(pMusic->m_pPerf->PlaySegmentEx(pMusic->m_pSegmentCurrent, NULL, NULL, 0, 0, NULL, NULL, pMusic->m_pPath));
        if (SUCCEEDED(hr))
        {
        
            //Wait for this segment to start.
            dwTimeOuts = 0;
            do
            {
                hrTimeOut = WaitForSegmentStart(pMusic->m_pPerf, pMusic->m_pSegmentCurrent, NULL, 1000);
                if (FAILED(hrTimeOut))
                {
                    dwTimeOuts++;
                    Log("We've waited %u second(s) for %s to start\n", dwTimeOuts, pMusic->m_szSegmentTitleCurrent);
                }
            }
            while (FAILED(hrTimeOut) && dwTimeOuts < 10);

            if (FAILED(hrTimeOut))
            {
                Log("Giving up on %s; it failed to start after 10 seconds\n", pMusic->m_szSegmentTitleCurrent);
            }
        }
        else
        {
            Log("%s failed to start: PlaySegmentEx() returned %08X\n", pMusic->m_szSegmentTitleCurrent, hr);
        }
    }

//    //Set the transpose value, which gets reset whenever there's a GM Reset.
//    g_music.SetMasterTranspose(g_TestParams.lDMMasterTranspose);

    pMusic->m_hrThreadProcReturn = hr;
    return 0;
};



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
DWORD LoadNextSegmentThreadProc(void *pvMusic)
{
CMusic *pMusic = (CMusic *)pvMusic;

CHAR szFileName[MAX_PATH];
CHAR szFilePath[MAX_PATH];
LPSTR szFullPath = NULL;
HRESULT hr = S_OK;



    //Load up new segment; queue it, get its segstate, and save its name.  Now we have SegmentNext and SegStateNext.    
    do
    {
        hr = S_OK;
        szFullPath = pMusic->m_ppszFiles[pMusic->m_dwCurrentFile];
        ChopPath(szFullPath, szFilePath, szFileName);
        strcpy(pMusic->m_szSegmentTitleNext, szFileName);
        CHECKRUN(pMusic->m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, szFilePath, FALSE));
        CHECKRUN(pMusic->m_pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, szFileName, (void **)&pMusic->m_pSegmentNext));
        if (FAILED(hr))
        {
            Log("LoadObjectFromFile(%s) returned %08X\n", szFileName, hr);
        }
/*
        CHECKRUN(pMusic->m_pSegmentNext->Download(pMusic->m_pPerf));
        if (S_FALSE == hr)
        {
            //TODO: Put back in when 7038 is fixed!!!
            //Log("Downloading %s to the performance returned S_FALSE - why?\n", szFileName);
            hr = S_OK;
        }
*/

        pMusic->m_dwCurrentFile++;
        if (pMusic->m_dwCurrentFile == pMusic->m_dwMaxFiles)
            pMusic->m_dwCurrentFile = 0;
    }
    while (hr != S_OK);
    pMusic->m_hrThreadProcReturn = hr;
    return 0;
}



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::LoadNextSegment(void)
{
HRESULT hr = S_OK;

    if (g_TestParams.bDMLoaderThread)
    {
        m_hThreadLoad = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)LoadNextSegmentThreadProc,(void*)this, 0, NULL);
        if (!m_hThreadLoad)
        {
            Log("CMusic::LoadNextSegment: CreateThread(LoadNextSegmentThreadProc) failed!!\n");
            return E_FAIL;        
        }

    }

    //If we're not using threads, then call the threadproc directly.
    else
    {
        //This will set m_hrThreadProcReturn appropriately.
        LoadNextSegmentThreadProc(this);
        hr = m_hrThreadProcReturn;
    }

    return hr;
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::TransitionSegments(void)
{
HRESULT hr = S_OK;

    if (g_TestParams.bDMLoaderThread)
    {
        m_hThreadTransition = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TransitionSegmentsThreadProc,(void*)this, 0, NULL);
        if (!m_hThreadTransition)
        {
            Log("CMusic::TransitionSegments: CreateThread(TransitionSegmentsThreadProc) failed!!\n");
            return E_FAIL;        
        }

    }

    //If we're not using threads, then call the threadproc directly.
    else
    {
        //This will set m_hrThreadProcReturn appropriately.
        TransitionSegmentsThreadProc(this);
        hr = m_hrThreadProcReturn;
    }

    return hr;
};



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::Update(void)
{
    HRESULT hr = S_OK;
    
    CHECKRUN(UpdatePlaybackState());
    CHECKRUN(UpdateBeatState());

    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT CMusic::UpdatePlaybackState(void)
{
HRESULT hr = S_OK;
LONG lRange = 2000;
DWORD dwQuantum = 0;
static DWORD dwLastTime = 0;
DWORD dwThisTime = 0;
DWORD dwWaitResult = 0;
BOOL bMoveToNextState = FALSE;

    if (!g_TestParams.bDMusic)
        return S_OK;

    if (g_TestParams.bDMDoWorkLocally)
    {
        dwThisTime = timeGetTime();
        dwQuantum = dwThisTime - dwLastTime;
        if (dwQuantum > 1000)
            dwQuantum = 10000; 
        dwLastTime = dwThisTime;        
        DirectMusicDoWork(dwQuantum);
    }


    // ------------- Process state variables
    
    
    // ------------- Process the state (m_eState) --------------------------

    //If we're just waiting for the current segment to finish, check if it's finished.
    if (eWaiting == m_eState)
    {
        BOOL bTransition = FALSE;

        //If our current statement has stopped, is non-existent, or we've been instructed to, 
        //  start the new segment playing and start a thread to load up the next one.
        if (!m_pSegmentCurrent)
        {
            bTransition = TRUE;
        }
        else
        {
            if (S_FALSE == m_pPerf->IsPlaying(m_pSegmentCurrent, NULL))
            {
                bTransition = TRUE;
            }
        
        }
        
        //If the user has requested a segment update with the controller, do the same.
        if (m_bNextSegment)
        {
           bTransition = TRUE;
           m_bNextSegment = FALSE;        
        }        

        //If we've waited 5 minutes, then move along to the next segment.  This is so looping waves don't hang the system.
        if (timeGetTime() - m_dwStartTime > 300000)
        {
            bTransition = TRUE;
        }

        //If in one of the above blocks, we've decided to transition, make it so.
        if (bTransition == TRUE)
        {           
           m_eState = eTransitioning;
           TransitionSegments();
        }
       
    }

    //If we're transitioning, check to see if the transition thread has finished and destroy it if it has.
    //  If we are playing, then start the next segment loading.
    else if (eTransitioning == m_eState)
    {
        bMoveToNextState = FALSE;

        if (g_TestParams.bDMLoaderThread)
        {
            dwWaitResult = WaitForSingleObject(m_hThreadTransition, 0);
            if (dwWaitResult == WAIT_OBJECT_0)
            {
                CloseHandle(m_hThreadTransition);
                m_hThreadTransition = NULL;
                bMoveToNextState = TRUE;
            }
        }
        else
        {
            bMoveToNextState = TRUE;
        }


        if (bMoveToNextState)
        {
            m_eState = eLoading;
            LoadNextSegment();
        }

    }
    
    //If we're loading then check to see if the loading thread has finished and destroy it if it has.
    else if (eLoading == m_eState)
    {

        bMoveToNextState = FALSE;

        if (g_TestParams.bDMLoaderThread)
        {
            dwWaitResult = WaitForSingleObject(m_hThreadLoad, 0);
            if (dwWaitResult == WAIT_OBJECT_0)
            {
                CloseHandle(m_hThreadLoad);
                m_hThreadLoad = NULL;
                bMoveToNextState = TRUE;
            }
        }
        else
        {
            bMoveToNextState = TRUE;
        }

        //If we're done loading, move to the next state and note the countdown time.
        if (bMoveToNextState)
        {
            m_eState = eWaiting;
            m_dwStartTime = timeGetTime();
        }
    }

    // default case
    else 
    {    
        Log("Error: Invalid m_eState in %08X\n", this);
    }

    return hr;
}

/********************************************************************************
********************************************************************************/
HRESULT CMusic::UpdateBeatState(void)
{
DMUS_NOTIFICATION_PMSG *pMsg;
HRESULT hr = S_OK;

    if (!g_TestParams.bDMusic)
        return S_OK;

    do
    {
        pMsg = NULL;
        hr = m_pPerf->GetNotificationPMsg(&pMsg);
        if (S_OK == hr)
        {
            m_dwBeat = pMsg->dwField1;
            m_pPerf->FreePMsg((DMUS_PMSG*)pMsg);
        }
    }
    while (S_OK == hr);
    return S_OK;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::GetSegmentTitleCurrent(WCHAR *wszSongTitle)
{
    mbstowcs(wszSongTitle, m_szSegmentTitleCurrent, MAX_PATH);
    return S_OK;
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
DWORD CMusic::GetBeat(void)
{
    return m_dwBeat;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::NextSegment(void)
{
m_bNextSegment = TRUE;
return S_OK;

}



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::SetMasterVolume(LONG lMasterVolume)
{
HRESULT hr = S_OK;
CHECKRUN(m_pPerf->SetGlobalParam(GUID_PerfMasterVolume, (void *)&lMasterVolume, sizeof(lMasterVolume)));
return hr;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::SetMasterTempo(float fMasterTempo)
{
HRESULT hr = S_OK;
CHECKRUN(m_pPerf->SetGlobalParam(GUID_PerfMasterTempo, (void *)&fMasterTempo, sizeof(fMasterTempo)));
return hr;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::SetMasterTranspose(short lMasterTranspose)
{
HRESULT hr = S_OK;
DMUS_TRANSPOSE_PMSG *pTranspose = NULL;

CHECKRUN(m_pPerf->AllocPMsg(sizeof(DMUS_TRANSPOSE_PMSG),(DMUS_PMSG**)&pTranspose ));
if (!pTranspose)
{
    Log("TEST ERROR: Failed to AllocPMsg!!\n");
    hr = E_FAIL;
}

if (SUCCEEDED(hr))
{
    pTranspose->dwSize = sizeof(DMUS_TRANSPOSE_PMSG);
    pTranspose->rtTime = 0;
    pTranspose->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_DX8;
    pTranspose->dwType = DMUS_PMSGT_TRANSPOSE;
    pTranspose->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
    pTranspose->nTranspose = g_TestParams.lDMMasterTranspose;
    pTranspose->wMergeIndex = 1; //1 = add this value.
    hr = m_pPerf->SendPMsg( (DMUS_PMSG*)pTranspose );
    if (FAILED(hr))
    {
        m_pPerf->FreePMsg( (DMUS_PMSG*)pTranspose );
        pTranspose = NULL;
    }
}
return hr;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::SetReverb(unsigned short nReverb)
{
HRESULT hr = S_OK;
DMUS_CURVE_PMSG *pCurve= NULL;
m_dwReverb = nReverb;

CHECKRUN(m_pPerf->AllocPMsg(sizeof(DMUS_CURVE_PMSG),(DMUS_PMSG**)&pCurve ));
if (!pCurve)
{
    Log("TEST ERROR: Failed to AllocPMsg!!\n");
    hr = E_FAIL;
}

if (SUCCEEDED(hr))
{
    pCurve->dwSize = sizeof(DMUS_CURVE_PMSG);
    pCurve->rtTime = 0;
    pCurve->dwGroupID = 0xFFFFFFF;     //This was in the sample so I copied it.
    pCurve->dwFlags = DMUS_PMSGF_REFTIME;
    pCurve->dwType = DMUS_PMSGT_CURVE;    
    pCurve->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
    
    pCurve->mtDuration = 10;     //duration of the curve.
    pCurve->mtOriginalStart = 0; //right away.
    pCurve->mtResetDuration = 0; //Ignored if DMUS_CURVE_RESET isn't in flags.

    pCurve->nStartValue = (unsigned short)m_dwReverb;  //who cares, it's instant.
    pCurve->nEndValue   = (unsigned short)m_dwReverb;  //what we're setting.
    pCurve->nResetValue = (unsigned short)m_dwReverb;  //Reset to default when invalidated.  //BUGBUG: Right now we don't know the default so we'll use zero.

    pCurve->wMeasure = 0;           // Measure in which this curve occurs 
    pCurve->nOffset  = 0;           // Offset from grid at which this curve occurs 
    pCurve->bBeat    = 0;           // Beat (in measure) at which this curve occurs 
    pCurve->bGrid    = 0;           // Grid offset from beat at which this curve occurs 
    pCurve->bType       = DMUS_CURVET_CCCURVE; // type of curve: CC curve
    pCurve->bCurveShape = DMUS_CURVES_INSTANT; // shape of curve 
    pCurve->bCCData  = 91;          // CC# if this is a control change type 
    pCurve->bFlags   = 0;           // Don't reset the curve.
    pCurve->wParamType = 0;         // RPN or NRPN parameter number.  I think this is zero.
    pCurve->wMergeIndex = 0;        // Allows multiple parameters to be merged (pitchbend, volume, and expression.)

    hr = m_pPerf->SendPMsg( (DMUS_PMSG*)pCurve );
    if (FAILED(hr))
    {
        m_pPerf->FreePMsg( (DMUS_PMSG*)pCurve );
        pCurve = NULL;
    }
}
return hr;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT CMusic::GetReverb(DWORD *dwReverb)
{
*dwReverb = m_dwReverb;
return S_OK;
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMusic::GetHeapAllocation          (DWORD *pdwHeapAllocationNormal, DWORD *pdwHeapAllocationPhysical)
{
    DWORD dw = 0;
    if (!g_TestParams.bDMusic)
    {
        *pdwHeapAllocationNormal = 0;
        *pdwHeapAllocationPhysical = 0;
        return;
    }
    if (pdwHeapAllocationNormal)
    {
        if (m_pHeapNormal)
        {
            m_pHeapNormal->GetHeapAllocation(&dw);
            *pdwHeapAllocationNormal = dw;
        }
    }
    if (pdwHeapAllocationPhysical)
    {
        if (m_pHeapPhysical)
        {
            m_pHeapPhysical->GetHeapAllocation(&dw);
            *pdwHeapAllocationPhysical = dw;
        }
    }
};



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMusic::GetHeapAllocationPeak      (DWORD *pdwHeapAllocationPeakNormal, DWORD *pdwHeapAllocationPeakPhysical)
{
    DWORD dw = 0;
    if (!g_TestParams.bDMusic)
    {
        *pdwHeapAllocationPeakNormal = 0;
        *pdwHeapAllocationPeakPhysical = 0;
        return;
    }
    if (pdwHeapAllocationPeakNormal)
    {
        if (m_pHeapNormal)
        {
            m_pHeapNormal->GetHeapAllocationPeak(&dw);
            *pdwHeapAllocationPeakNormal = dw;
        }
    }
    if (pdwHeapAllocationPeakPhysical)
    {
        if (m_pHeapPhysical)
        {
            m_pHeapPhysical->GetHeapAllocationPeak(&dw);
            *pdwHeapAllocationPeakPhysical = dw;
        }
    }
};




//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMusic::GetHeapAllocationBlocks    (DWORD *pdwHeapAllocationBlocksNormal, DWORD *pdwHeapAllocationBlocksPhysical)
{
    DWORD dw = 0;
    if (!g_TestParams.bDMusic)
    {
        *pdwHeapAllocationBlocksNormal = 0;
        *pdwHeapAllocationBlocksPhysical = 0;
        return;
    }
    if (pdwHeapAllocationBlocksNormal)
    {
        if (m_pHeapNormal)
        {
            m_pHeapNormal->GetHeapAllocationBlocks(&dw);
            *pdwHeapAllocationBlocksNormal = dw;
        }
    }
    if (pdwHeapAllocationBlocksPhysical)
    {
        if (m_pHeapPhysical)
        {
            m_pHeapPhysical->GetHeapAllocationBlocks(&dw);
            *pdwHeapAllocationBlocksPhysical = dw;
        }
    }
};




//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMusic::GetHeapAllocationBlocksPeak(DWORD *pdwHeapAllocationBlocksPeakNormal, DWORD *pdwHeapAllocationBlocksPeakPhysical)
{
    DWORD dw = 0;
    if (!g_TestParams.bDMusic)
    {
        *pdwHeapAllocationBlocksPeakNormal = 0;
        *pdwHeapAllocationBlocksPeakPhysical = 0;
        return;
    }
    if (pdwHeapAllocationBlocksPeakNormal)
    {
        if (m_pHeapNormal)
        {
            m_pHeapNormal->GetHeapAllocationBlocksPeak(&dw);
            *pdwHeapAllocationBlocksPeakNormal = dw;
        }
    }
    if (pdwHeapAllocationBlocksPeakPhysical)
    {
        if (m_pHeapPhysical)
        {
            m_pHeapPhysical->GetHeapAllocationBlocksPeak(&dw);
            *pdwHeapAllocationBlocksPeakPhysical = dw;
        }
    }
};



}//end "namespace WFVO"
