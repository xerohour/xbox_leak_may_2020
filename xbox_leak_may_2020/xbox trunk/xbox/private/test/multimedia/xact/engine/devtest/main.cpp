#include <xtl.h>
#include <xdbg.h>
#include <xactp.h>
#include "devtest.h"
#include <dsstdfx.h>

#define BREAK_ON_START 1

#define NUM_SOUNDBANKS  1
#define NUM_WAVEBANKS 2

#define NUM_CUES 1
#define NUM_VOICES 2

char * g_aszSoundBankFileNames[] = 
{
    "D:\\Media\\soundbank1.bin"
};

// List of wavebanks
char * g_aszWaveBankFileNames[] = 
{
    "D:\\Media\\wavebank.xwb",
    "D:\\Media\\wavebank1.xwb"
};


// List of cues
char * g_aszCueNames[] = 
{
    "CUE0",
    "CUE1"
};

CXactTest::CXactTest()
{
    
    m_pEngine = NULL;
    memset(m_aSoundBanks,0,sizeof(m_aSoundBanks));
    memset(m_aWaveBanks,0,sizeof(m_aWaveBanks));
    memset(m_paSoundSources,0,sizeof(m_paSoundSources));
    
    m_dwNumSoundSources = 0;
    m_dwNumSoundBanks = 0;
    m_dwNumWaveBanks = 0;
    
    m_pvDspImage = NULL;
    m_hThread = NULL;
}

CXactTest::~CXactTest()
{
    
    DWORD i = 0, j = 0;
    
    for (i=0;i<m_dwNumSoundSources;i++)
    {
        PXACTSOUNDSOURCE pSoundSource = m_paSoundSources[i];
        m_paSoundSources[i]->Release();
    }
    
    for (i=0;i<m_dwNumSoundBanks;i++)
    {
        
        PXACTSOUNDBANK pSoundBank = m_aSoundBanks[i].pSoundBank;

        //
        // stop all cues
        //

        pSoundBank->Stop(XACT_SOUNDCUE_INDEX_UNUSED,0,NULL);        
        pSoundBank->Release();
        delete [] m_aSoundBanks[i].pvSoundBankData;
    }
    
    for (i=0;i<m_dwNumWaveBanks;i++)
    {
        if (m_aWaveBanks[i].pWaveBank)
            m_pEngine->UnRegisterWaveBank(m_aWaveBanks[i].pWaveBank);

        delete [] m_aWaveBanks[i].pvWaveBankData;
        
    }

    //
    // tell the thread to exit
    //

    if (m_hThread != INVALID_HANDLE_VALUE) {
        PXACTENGINE pEngine = m_pEngine;
        m_pEngine = NULL;
        
        WaitForSingleObject(m_hThread,INFINITE);    
        pEngine->Release();
    }
    
    delete [] m_pvDspImage;
}


HRESULT CXactTest::Initialize(DWORD dwNumSoundBanks,
                   DWORD dwNumWaveBanks,
                   DWORD dwNumVoices
                   )
{
    HRESULT hr = S_OK;

    DWORD i = 0;
    PVOID pvData;
    DWORD dwSize;

    DirectSoundUseLightHRTF();

    XACT_RUNTIME_PARAMETERS params;

    memset(&params,0,sizeof(params));
    params.dwMax2DHwVoices = 150;
    params.dwMax3DHwVoices = 64;
    params.dwMaxConcurrentStreams = 10;

    //
    // create the engine
    //

    hr = XACTEngineCreate(&m_pEngine,&params);

    //
    // load a DSP Image
    //

    if (SUCCEEDED(hr)) {

        hr = LoadBinaryData("d:\\media\\dsstdfx.bin",&m_pvDspImage,&dwSize);

    }

    if (SUCCEEDED(hr)) {

        DSEFFECTIMAGELOC effectLoc;
        memset(&effectLoc,0,sizeof(effectLoc));

        effectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
        effectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

        hr = m_pEngine->LoadDspImage(m_pvDspImage,dwSize,&effectLoc);
    }

    if (SUCCEEDED(hr)) {
        
        ASSERT(m_dwNumSoundBanks <= MAX_SOUNDBANKS);
        ASSERT(m_dwNumWaveBanks <= MAX_WAVEBANKS);
        ASSERT(m_dwNumSoundSources <= MAX_SOUNDSOURCES);
        
        m_dwNumSoundBanks = dwNumSoundBanks;
        m_dwNumWaveBanks = dwNumWaveBanks;
        m_dwNumSoundSources = dwNumVoices;
        
    }

    if (SUCCEEDED(hr)) {

        //
        // load soundbank files
        //
        
        for (i=0;i<m_dwNumSoundBanks;i++)
        {
            if (SUCCEEDED(hr)) {
                
                hr = LoadBinaryData(g_aszSoundBankFileNames[i],&pvData,&dwSize);
                
            } else {
                break;
            }
            
            m_aSoundBanks[i].pvSoundBankData = pvData;

            //
            // create a soundbank object
            //
            
            if (SUCCEEDED(hr)) {
                
                hr = m_pEngine->CreateSoundBank(pvData,dwSize,&m_aSoundBanks[i].pSoundBank);
                
            } else {
                break;
            }
            
        }
        
    }

    if (SUCCEEDED(hr)) {
        
        //
        // load wavebank files
        //
        
        for (i=0;i<m_dwNumWaveBanks;i++)
        {
            if (SUCCEEDED(hr)) {
                
                hr = LoadBinaryData(g_aszWaveBankFileNames[i],&pvData,&dwSize);
                
            } else {
                break;
            }
            
            m_aWaveBanks[i].pvWaveBankData = pvData;
            m_aWaveBanks[i].dwSize = dwSize;
            
            //
            // register wavebank
            //
            
            if (SUCCEEDED(hr)) {
                
                hr = m_pEngine->RegisterWaveBank(m_aWaveBanks[i].pvWaveBankData,
                    m_aWaveBanks[i].dwSize,
                    &m_aWaveBanks[i].pWaveBank);
                
            } else {
                break;
            }
            
        }
        
    }

    if (SUCCEEDED(hr)) {

        //
        // create voice objects 
        //

        for (i=0;i<m_dwNumSoundSources;i++)
        {
                                   
            hr = m_pEngine->CreateSoundSource(XACT_FLAG_SOUNDSOURCE_3D,&m_paSoundSources[i]);
            
        }

    }

    if (SUCCEEDED(hr)) {
        //
        // create a thread for calling DoWork
        //
        
        m_hThread = CreateThread(NULL,
            4096,
            XactTestThreadProc,
            this,
            0,
            &m_dwThreadId);
        
        if (m_hThread == INVALID_HANDLE_VALUE) 
        {

            hr = E_FAIL;


        }
            
    }

    return hr;

}

DWORD WINAPI XactTestThreadProc(
    LPVOID pParameter
    )
{
    CXactTest *pThis = (CXactTest *) pParameter;
    pThis->ThreadProc();

    return 0;
}

VOID 
CXactTest::ThreadProc()
{

    while(m_pEngine) {

        //
        // simulate DoWork being called at 60FPS
        //

        Sleep(16);
        XACTEngineDoWork();

    }

}
    
#define NUM_EVENTS  7

HRESULT
CXactTest::RuntimeEvents()
{
    HRESULT hr = S_OK;
    DWORD dwSoundCueIndex = 0;
    DWORD i = 0;
    
    PXACTSOUNDBANK pSoundBank = m_aSoundBanks[0].pSoundBank;
    XACT_TRACK_EVENT aEventDesc[NUM_EVENTS];

    DWORD   aEventTable[NUM_EVENTS] = {        
        eXACTEvent_SetFrequency,
        eXACTEvent_PlayWithPitchAndVolumeVariation,
        eXACTEvent_SetVolume,
        eXACTEvent_SetHeadroom,
        eXACTEvent_SetLFO,
        eXACTEvent_SetEG,
        eXACTEvent_SetFilter
    };


    //
    // play a cue in manual release mode
    //
    
    if (SUCCEEDED(hr)) {
        hr = pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[0],&dwSoundCueIndex);
    }
    
    //
    // play the cue in manual release mode
    //
    
    if (SUCCEEDED(hr)) {
        hr = pSoundBank->Play(dwSoundCueIndex,
            m_paSoundSources[0],
            0,
            &m_aSoundBanks[0].paSoundCues[0]);
    }
    
    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].dwNumSoundCues ++;
    }
    
    //
    // submit random events for N seconds
    //
    
    i = 0;
    DWORD dwIndex = 0;

    do {
        
        memset(&aEventDesc[dwIndex],0,sizeof(XACT_TRACK_EVENT));
        aEventDesc[dwIndex].Header.wType = (WORD)aEventTable[dwIndex];
        
        
        CreateRandomEvent(&aEventDesc[dwIndex]);

        hr = m_pEngine->ScheduleEvent(&aEventDesc[dwIndex],
            m_aSoundBanks[0].paSoundCues[0],
            -1);

        i++;
        dwIndex ++;

        if (dwIndex >= NUM_EVENTS) {
            dwIndex = 0;
        }

        Sleep(500);
        
    } while( (i < 20) && SUCCEEDED(hr));
    
        
    //
    // force stop on all cues with the same index
    //
    
    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->Stop(dwSoundCueIndex,
            XACT_FLAG_SOUNDCUE_SYNCHRONOUS,
            NULL);
        
    }
    
        
    //
    // cue should be freed, NULL context
    //
    
    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].paSoundCues[i] = NULL;
        m_aSoundBanks[0].dwNumSoundCues--;
        
    }

    return hr;
    
}

HRESULT
CXactTest::BasicPlayStop()
{

    HRESULT hr = S_OK;
    DWORD dwSoundCueIndex = 0;
    DWORD i = 0;
    
    //
    // play the cue in manual release mode
    //

    for (i=0;i<NUM_CUES;i++)
    {
        
        if (SUCCEEDED(hr)) {
            hr = m_aSoundBanks[0].pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[i],&dwSoundCueIndex);
        }
        
        //
        // play the cue in manual release mode
        //

        if (SUCCEEDED(hr)) {
            hr = m_aSoundBanks[0].pSoundBank->Play(dwSoundCueIndex,
                m_paSoundSources[i],
                0,
                &m_aSoundBanks[0].paSoundCues[i]);
        }
        
        if (SUCCEEDED(hr)) {
            
            m_aSoundBanks[0].dwNumSoundCues ++;
        }

        //
        // release the wavebanks used by the cue
        //

        if (m_bTestPrematureWavebankUnregister) {

            for (i=0;i<m_dwNumWaveBanks;i++) {

                 if (SUCCEEDED(hr)) 
                     hr = m_pEngine->UnRegisterWaveBank(m_aWaveBanks[i].pWaveBank);

                 m_aWaveBanks[i].pWaveBank = NULL;

            }

        }

        Sleep(16);

        //
        // force stop on all cues with the same index
        //
        
        if (SUCCEEDED(hr)) {
            hr = m_aSoundBanks[0].pSoundBank->Stop(dwSoundCueIndex,
                0,
                NULL);
            
        }

        //
        // cue will be freed, NULL context
        //
        
        if (SUCCEEDED(hr)) {
            
            m_aSoundBanks[0].paSoundCues[i] = NULL;
            m_aSoundBanks[0].dwNumSoundCues--;

        }
        
    }

    if (m_bTestPrematureWavebankUnregister) {

        //
        // re-register the wavebanks
        //

        for (i=0;i<m_dwNumWaveBanks;i++) {
            if (SUCCEEDED(hr)) {
                
                hr = m_pEngine->RegisterWaveBank(m_aWaveBanks[i].pvWaveBankData,
                    m_aWaveBanks[i].dwSize,
                    &m_aWaveBanks[i].pWaveBank);
            }
            
        }

    }

    //
    // play cue in auto-release mode
    //

    if (SUCCEEDED(hr)) {
        
        for (i=0;i<NUM_CUES;i++)
        {
            
            if (SUCCEEDED(hr)) {
                hr = m_aSoundBanks[0].pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[i],&dwSoundCueIndex);
            }
            
            if (SUCCEEDED(hr)) {
                hr = m_aSoundBanks[0].pSoundBank->Play(dwSoundCueIndex,
                    m_paSoundSources[i],
                    XACT_FLAG_SOUNDCUE_AUTORELEASE,
                    NULL);
            }
            
            
        }
        
        //
        // wait for cues to finish on their own
        //

        Sleep(20*1000);
        
    }

    //
    // play cue with no voice specified and AUTORELEASE mode
    //

    if (SUCCEEDED(hr)) {
        
        for (i=0;i<NUM_CUES;i++)
        {
            
            if (SUCCEEDED(hr)) {
                hr = m_aSoundBanks[0].pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[i],&dwSoundCueIndex);
            }
            
            if (SUCCEEDED(hr)) {
                hr = m_aSoundBanks[0].pSoundBank->Play(dwSoundCueIndex,
                    NULL,
                    XACT_FLAG_SOUNDCUE_AUTORELEASE,
                    NULL);
            }
            
            
        }

        Sleep(100);

        //
        // release the wavebanks used by the cue
        //

        if (m_bTestPrematureWavebankUnregister) {

            for (i=0;i<m_dwNumWaveBanks;i++) {

                 if (SUCCEEDED(hr)) 
                     hr = m_pEngine->UnRegisterWaveBank(m_aWaveBanks[i].pWaveBank);

                 m_aWaveBanks[i].pWaveBank = NULL;

            }

        }

        //
        // wait for cues to finish on their own
        //

        Sleep(20000);
        
    }

    return hr;

}

HRESULT
CXactTest::Notifications()
{

    HRESULT hr = S_OK;
    XACT_NOTIFICATION_DESCRIPTION Desc;
    XACT_NOTIFICATION Notification;
    HANDLE hEvent;
    DWORD dwSoundCueIndex;
    DWORD i = 0;

    memset(&Desc,0,sizeof(Desc));

    hEvent = CreateEvent(NULL,
        FALSE,
        FALSE,
        NULL);

    if (hEvent == NULL) {

        hr = E_FAIL;
    }

    //
    // first test sounbank global notifications
    //

    if (SUCCEEDED(hr)) {

        //
        // register for a stop and a start
        Desc.pSoundBank = m_aSoundBanks[0].pSoundBank;
        Desc.dwType = eXACTNotification_Start; 
        Desc.hEvent = hEvent;
        Desc.dwSoundCueIndex = XACT_SOUNDCUE_INDEX_UNUSED;

        hr = m_pEngine->RegisterNotification(&Desc);

        Desc.pSoundBank = m_aSoundBanks[0].pSoundBank;
        Desc.dwType = eXACTNotification_Stop; 
        Desc.hEvent = NULL;
        Desc.dwSoundCueIndex = XACT_SOUNDCUE_INDEX_UNUSED;

        hr = m_pEngine->RegisterNotification(&Desc);

        
    }

    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[0],&dwSoundCueIndex);
    }

    //
    // play the cue in manual release mode
    //
    
    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->Play(dwSoundCueIndex,
            m_paSoundSources[i],
            0,
            &m_aSoundBanks[0].paSoundCues[0]);
    }
    
    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].dwNumSoundCues ++;
    }
    
    if (SUCCEEDED(hr)) {
        Desc.dwType = eXACTNotification_Start; 
        WaitForSingleObject(hEvent,INFINITE);
        hr = m_pEngine->GetNotification(&Desc,&Notification);
    }

    if (SUCCEEDED(hr)) {

        ASSERT(Notification.Header.dwType == Desc.dwType);
    }

    //
    // force stop on all cues with the same index
    //
    
    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->Stop(dwSoundCueIndex,
            0,
            NULL);
        
    }

    //
    // poll until stop notification is present
    //

    if (SUCCEEDED(hr)) {

        Desc.dwType = eXACTNotification_Stop;     
        do {
            hr = m_pEngine->GetNotification(&Desc,&Notification);
        } while (FAILED(hr));

    }

    //
    // cue will be freed, NULL context
    //
    
    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].paSoundCues[i] = NULL;
        m_aSoundBanks[0].dwNumSoundCues--;
        
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    //
    // now test cue specific notifications
    //
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->GetSoundCueIndexFromFriendlyName(g_aszCueNames[0],&dwSoundCueIndex);
    }

    if (SUCCEEDED(hr)) {

        memset(&Desc,0,sizeof(Desc));

        //
        // register for a stop and a start on the cue
        //

        Desc.dwSoundCueIndex = dwSoundCueIndex;
        Desc.pSoundBank = m_aSoundBanks[0].pSoundBank;
        Desc.dwType = eXACTNotification_Start; 
        Desc.hEvent = hEvent;
        Desc.dwSoundCueIndex = dwSoundCueIndex;

        hr = m_pEngine->RegisterNotification(&Desc);

        Desc.dwType = eXACTNotification_Stop; 
        Desc.dwSoundCueIndex = dwSoundCueIndex;
        Desc.pSoundBank = m_aSoundBanks[0].pSoundBank;
        Desc.hEvent = NULL;

        hr = m_pEngine->RegisterNotification(&Desc);
        
    }


    //
    // play the cue in manual release mode
    //
    
    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->Play(dwSoundCueIndex,
            m_paSoundSources[0],
            0,
            &m_aSoundBanks[0].paSoundCues[0]);
    }
    
    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].dwNumSoundCues ++;
    }
    
    if (SUCCEEDED(hr)) {

        memset(&Desc,0,sizeof(Desc));
        Desc.dwType = eXACTNotification_Start; 
        Desc.pSoundCue = m_aSoundBanks[0].paSoundCues[0];
        Desc.dwSoundCueIndex = XACT_SOUNDCUE_INDEX_UNUSED;

        WaitForSingleObject(hEvent,INFINITE);
        hr = m_pEngine->GetNotification(&Desc,&Notification);
    }

    if (SUCCEEDED(hr)) {

        ASSERT(Notification.Header.dwType == Desc.dwType);
    }

    //
    // force stop on all cues with the same index
    //
    
    if (SUCCEEDED(hr)) {
        hr = m_aSoundBanks[0].pSoundBank->Stop(dwSoundCueIndex,
            0,
            NULL);
        
    }

    //
    // poll until stop notification is present
    //

    if (SUCCEEDED(hr)) {

        Desc.dwType = eXACTNotification_Stop;     
        Desc.pSoundCue = m_aSoundBanks[0].paSoundCues[0];
        Desc.dwSoundCueIndex = XACT_SOUNDCUE_INDEX_UNUSED;

        do {
            hr = m_pEngine->GetNotification(&Desc,&Notification);
        } while (FAILED(hr));

    }

    if (SUCCEEDED(hr)) {
        
        m_aSoundBanks[0].paSoundCues[i] = NULL;
        m_aSoundBanks[0].dwNumSoundCues--;
        
    }

    return hr;
}


VOID __cdecl main()
{
    HRESULT hr = S_OK;
    CXactTest XactTest;

    XactTest.m_bTestNotifications = 0;
    XactTest.m_bTestBasicPlayStop = 1;
    XactTest.m_bTestRuntimeEvents = 0;
    XactTest.m_bTestPrematureWavebankUnregister = 0;
    
#ifdef BREAK_ON_START
    _asm int 3;
#endif

    hr = XactTest.Initialize(NUM_SOUNDBANKS,
        NUM_WAVEBANKS,
        NUM_VOICES);

    if (SUCCEEDED(hr) && XactTest.m_bTestRuntimeEvents){
        hr = XactTest.RuntimeEvents();
    }

    if (SUCCEEDED(hr) && XactTest.m_bTestBasicPlayStop ){
        hr = XactTest.BasicPlayStop();
    }

    if (SUCCEEDED(hr) && XactTest.m_bTestNotifications){
        hr = XactTest.Notifications();
    }

    if (SUCCEEDED(hr)) {

        DbgPrint("Test succeeded");

    }

    XactTest.LinkAll();
}
