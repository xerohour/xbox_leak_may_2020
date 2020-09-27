/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       engine.cpp
 *  Content:    XACT runtime Engine implementation
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/22/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"


#undef DPF_FNAME
#define DPF_FNAME "XACTEngineCreateI"


EXTERN_C    XACT::CEngine* XACT::g_pEngine = NULL;

VOID
XACTEngineDoWork()
{
    using namespace XACT;

    ENTER_EXTERNAL_FUNCTION();
    CEngine*  pEngine = g_pEngine;

    if(!pEngine)
        return;

    DirectSoundDoWork();
    pEngine->DoWork();

    return;
}



HRESULT
XACTEngineCreate
(
    PXACTENGINE *ppEngine,PXACT_RUNTIME_PARAMETERS pParams
)
{
    using namespace XACT;
    CEngine*  pEngine;
    HRESULT   hr = S_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifdef VALIDATE_PARAMETERS

    if(!ppEngine)
    {
        DPF_ERROR("Failed to supply an PXACTENGINE *");
    }

    if(!pParams)
    {
        DPF_ERROR("Failed to supply PXACT_RUNTIME_PARAMETERS");
    }

    if (!pParams->dwMax2DHwVoices)
    {

        DPF_ERROR("dwMax2dVoices must be at least max(1,number of wavebanks registered at any time)");

    }

#endif // VALIDATE_PARAMETERS
    
    DPF_ENTER();

    //
    // Check to see if the engine object exists
    //

    if(g_pEngine)
    {
        *ppEngine = g_pEngine;
        g_pEngine->AddRef();
    }
    else
    {
        hr = HRFROMP(pEngine = NEW(CEngine));

        if(SUCCEEDED(hr))
        {
            hr = pEngine->Initialize(pParams);
        }

        if(SUCCEEDED(hr))
        {
            *ppEngine = pEngine;
        }
        else
        {
            pEngine->Release();
        }
    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}


using namespace XACT;


#undef DPF_FNAME
#define DPF_FNAME "CEngine::CEngine"


CEngine::CEngine
(
    void
)
{
    DPF_ENTER();

    m_dwRefCount = 1;

    //
    // Set global engine object pointer
    //

    g_pEngine = this;
    InitializeListHead(&m_lstAvailable2DBuffers);
    InitializeListHead(&m_lstAvailableStreams);
    InitializeListHead(&m_lstAvailable3DBuffers);
    InitializeListHead(&m_lstWaveBanks);
    InitializeListHead(&m_lstSoundBanks);
    InitializeListHead(&m_lstPendingNotifications);

    //
    // sequencer variables
    //

    InitializeListHead(&m_lstActiveCues);
    KeInitializeTimer(&m_TimerObject);
    KeInitializeDpc(&m_DpcObject, DPCTimerCallBack, this);

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::~CEngine"

CEngine::~CEngine
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    PLIST_ENTRY pEntry;
    CSoundSource *pSoundSource;

    //
    // sequencer de-init
    //

    {
        AutoIrql();
        m_bAllowQueueing = FALSE;
    }

    if(m_bTimerSet){
        KeCancelTimer(&m_TimerObject);
    }

    ASSERT(m_pQueue);
    FreeAllEvents(); // From queue

    ASSERT(IsListEmpty(&m_lstActiveCues));

    DELETE(m_pQueue);

    //
    // free 2d buffers
    //

    pEntry = m_lstAvailable2DBuffers.Flink;
    while (pEntry != &m_lstAvailable2DBuffers) {

        pEntry = RemoveHeadList(&m_lstAvailable2DBuffers);
        pSoundSource = CONTAINING_RECORD(pEntry, CSoundSource, m_ListEntry);

        pSoundSource->Release();

        pEntry = m_lstAvailable2DBuffers.Flink;

    }

    //
    // free 3d buffers
    //

    pEntry = m_lstAvailable3DBuffers.Flink;
    while (pEntry != &m_lstAvailable3DBuffers) {

        pEntry = RemoveHeadList(&m_lstAvailable3DBuffers);
        pSoundSource = CONTAINING_RECORD(pEntry, CSoundSource, m_ListEntry);

        pSoundSource->Release();

        pEntry = m_lstAvailable3DBuffers.Flink;

    }

    //
    // free streams
    //

    pEntry = m_lstAvailableStreams.Flink;
    while (pEntry != &m_lstAvailableStreams) {

        pEntry = RemoveHeadList(&m_lstAvailableStreams);
        pSoundSource = CONTAINING_RECORD(pEntry, CSoundSource, m_ListEntry);

        pSoundSource->Release();

        pEntry = m_lstAvailableStreams.Flink;

    }

    if (m_pDirectSound) {
        m_pDirectSound->Release();
    }

    g_pEngine = NULL;
    DPF_INFO("XACT Engine shutdown completely");

    DPF_LEAVE_VOID();
}

__inline ULONG CEngine::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CEngine::AddRef");
    return ++m_dwRefCount;
}

ULONG CEngine::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CEngine::Release");
    
    ASSERT(m_dwRefCount);
    m_dwRefCount--;

    if (m_dwRefCount == m_dwTotalVoiceCount){

        //
        // if the refcount equals the number of pre-allocated voices
        // it means its time to delete the engine object
        //

        delete this;
    }

    return m_dwRefCount;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::Initialize"

HRESULT CEngine::Initialize(PXACT_RUNTIME_PARAMETERS pParams)
{
    HRESULT hr = S_OK;
    DWORD i = 0;
    CSoundSource *pSoundSource;
    ENTER_EXTERNAL_METHOD();

    DSBUFFERDESC dsbd;
    DSSTREAMDESC dssd;
    WAVEFORMATEX wfx;

    DPF_ENTER();

    if (SUCCEEDED(hr)) {
        CopyMemory(&m_RuntimeParams, pParams, sizeof(XACT_RUNTIME_PARAMETERS));
        ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
        ZeroMemory( &dssd, sizeof( DSSTREAMDESC ) );
        
        hr = DirectSoundCreate(NULL,&m_pDirectSound, NULL);
    }

    if (SUCCEEDED(hr)) {
        hr = InitializeSequencer(XACT_ENGINE_MAX_CONCURRENT_EVENTS);
    }

    //
    // based on the caller supplied parameters, pre-allocate all dsound buffers and streams
    // we are about to use
    //

    if (SUCCEEDED(hr)) {

        //
        // allocate 2d buffers first
        //
        
        dsbd.dwSize = sizeof( DSBUFFERDESC );
        XAudioCreatePcmFormat(1, 8000, 8, &wfx);
        dsbd.lpwfxFormat = &wfx;
        dssd.lpwfxFormat = &wfx;
        dssd.dwMaxAttachedPackets = XACT_ENGINE_PACKETS_PER_STREAM;

        for (i=0;i<pParams->dwMax2DHwVoices-pParams->dwMaxConcurrentStreams;i++) {
    
            //
            // create the context used to track DS buffers/streams
            //
    
            hr = AllocateSoundSource(&pSoundSource);
    
            if(SUCCEEDED(hr)) {

                m_dwTotalVoiceCount++;

                //
                // add voice to linked list
                //
    
                InsertTailList(&m_lstAvailable2DBuffers,&pSoundSource->m_ListEntry);
    
                //
                // create buffer
                //
        
                hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &pSoundSource->m_HwVoice.pBuffer, NULL );
    
                //
                // tell the voice the dsound flags used
                //
    
                pSoundSource->SetHwVoiceType(0);
    
            } else {

                break;

            }
    
        }

    }

    //
    // create all 3d submix voices
    //

    if (SUCCEEDED(hr)) {

        for (i=0;i<pParams->dwMax3DHwVoices;i++) {
    
            //
            // create the context used to track DS buffers/streams
            //
    
            hr = AllocateSoundSource(&pSoundSource);
    
            if(SUCCEEDED(hr)) {
    
                m_dwTotalVoiceCount++;

                //
                // add voice to linked list
                //
    
                InsertTailList(&m_lstAvailable3DBuffers,&pSoundSource->m_ListEntry);
    
                //
                // create buffer
                //
    
                dsbd.lpwfxFormat = NULL;
                dsbd.dwFlags = DSBCAPS_MIXIN | DSBCAPS_CTRL3D;
        
                hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &pSoundSource->m_HwVoice.pBuffer, NULL );
    
                //
                // tell the voice what hw voice its associated with
                //
    
                pSoundSource->SetHwVoiceType(dsbd.dwFlags);        
    
            } else {
    
                break;
    
            }

        }

    }

    //
    // allocate streams
    //

    if (SUCCEEDED(hr)) {

        for (i=0;i<pParams->dwMaxConcurrentStreams;i++) {
    
            //
            // create the context used to track DS buffers/streams
            //
    
            hr = AllocateSoundSource(&pSoundSource);
    
            if(SUCCEEDED(hr)) {

                m_dwTotalVoiceCount++;

                //
                // add voice to linked list
                //
    
                InsertTailList(&m_lstAvailableStreams,&pSoundSource->m_ListEntry);
    
                //
                // create dsound stream
                //
        
                hr = m_pDirectSound->CreateSoundStream( &dssd, &pSoundSource->m_HwVoice.pStream, NULL);
    
                //
                // tell the voice what hw voice its associated with
                //
    
                pSoundSource->SetHwVoiceType(dssd.dwFlags);        
    
            } else {
    
                break;
    
            }

        }

    }    

    if (SUCCEEDED(hr)) {

        ASSERT(m_dwRefCount == 
            (pParams->dwMax3DHwVoices +\
             pParams->dwMax2DHwVoices)+1);

    }

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::AllocateSoundSource"

HRESULT CEngine::AllocateSoundSource(CSoundSource **ppSoundSource)
{
    CSoundSource *pSoundSource;
    HRESULT hr = HRFROMP(pSoundSource = NEW(CSoundSource));

    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();
    
    ASSERT(ppSoundSource);

    if(SUCCEEDED(hr)) {

        hr = pSoundSource->Initialize();
        *ppSoundSource = pSoundSource;

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::FreeSoundSource"

void CEngine::FreeSoundSource(CSoundSource *pSoundSource)
{

    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

    ASSERT(pSoundSource);

    //
    // add the voice in the proper available list.
    //

    ASSERT(pSoundSource->m_dwRefCount == 1);

    if (pSoundSource->IsPositional()) {
        InsertTailList(&m_lstAvailable3DBuffers, &pSoundSource->m_ListEntry);
    } else {

        if (pSoundSource->m_HwVoice.pBuffer) {
            InsertTailList(&m_lstAvailable2DBuffers, &pSoundSource->m_ListEntry);
        }
        
        if (pSoundSource->m_HwVoice.pStream) {
            InsertTailList(&m_lstAvailableStreams, &pSoundSource->m_ListEntry);
        }

    }

}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::DoWork"

VOID CEngine::DoWork()
{
    ENTER_EXTERNAL_METHOD();
    PLIST_ENTRY pEntry;
    CSoundCue *pCue;

    //
    // re-sync offset between cpu clock with apu sample clock
    //
    
    SetTimeOffset();
    
    //
    // tell all active cues to get busy
    //
    
    pEntry = m_lstActiveCues.Flink;
    while (pEntry != &m_lstActiveCues)
    {
        pCue = CONTAINING_RECORD(pEntry, CSoundCue, m_SeqListEntry);
        pEntry = pEntry->Flink;
        
        pCue->DoWork();        
    }
    


}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::GetWaveBank"

HRESULT CEngine::GetWaveBank(LPCSTR lpFriendlyName, CWaveBank **ppWaveBank)
{
    PLIST_ENTRY pEntry;
    CWaveBank *pWaveBank;
    HRESULT hr = E_FAIL;

    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

    *ppWaveBank = NULL;

    //
    // search wavebank list using wavebank friendly name
    //

    ASSERT(lpFriendlyName);

    if (IsListEmpty(&m_lstWaveBanks)) {
        DPF_WARNING("No wavebanks registered");
        return E_FAIL;
    }

    pEntry = m_lstWaveBanks.Flink;
    while (pEntry != &m_lstWaveBanks) {

        pWaveBank = CONTAINING_RECORD(pEntry, CWaveBank, m_ListEntry);

        if (!strncmp(pWaveBank->m_WaveBankData.pHeader->szBankName,
            lpFriendlyName,
            XACT_SOUNDBANK_WAVEBANK_FRIENDLYNAME_LENGTH)) {

            //
            // found the correct wavebank
            //

            *ppWaveBank = pWaveBank;
            hr = S_OK;

        }

        pEntry = pEntry->Flink;

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;

}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::AddNotificationToPendingList"

VOID CEngine::AddNotificationToPendingList(NOTIFICATION_CONTEXT *pContext)
{

    if (pContext->bRegistered) {
        
        {
            AutoIrql();
            
            InsertTailList(&m_lstPendingNotifications,
                &pContext->ListEntry);
        }
                
        //
        // updated timestamp
        //
        
        KeQuerySystemTime((PLARGE_INTEGER)&pContext->PendingNotification.rtTimeStamp);
                               
        //
        // signal the event if present
        //
        
        if (pContext->PendingNotification.Header.hEvent) {
            SetEvent(pContext->PendingNotification.Header.hEvent);
        }
        
    }            

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::HandleNotificationRegistration"

VOID CEngine::HandleNotificationRegistration(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, BOOL bRegister)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    PNOTIFICATION_CONTEXT pContext = NULL;

#ifdef VALIDATE_PARAMETERS

    if(!pNotificationDesc)
    {
        DPF_ERROR("No pNotificationDesc supplied");
    }

    if (pNotificationDesc->pSoundBank && pNotificationDesc->pSoundCue) {

        DPF_ERROR("You cant supply pSoundBank AND pSoundCue");
    }

    if (!pNotificationDesc->pSoundBank && !pNotificationDesc->pSoundCue) {

        DPF_ERROR("You must supply pSoundBank OR pSoundCue");
    }

    if ((pNotificationDesc->dwSoundCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) &&
        (!pNotificationDesc->pSoundBank)) { 

        DPF_WARNING("YOu must supply pSoundBank if dwSoundCueIndex is specified");

    }

    //
    // validate notification type
    //

    if ((pNotificationDesc->dwType & XACT_MASK_NOTIFICATION_TYPE) >= eXACTNotification_Max) {
        DPF_ERROR("Invalid notification type");
    }

#endif // VALIDATE_PARAMETERS

    DWORD dwType = pNotificationDesc->dwType & XACT_MASK_NOTIFICATION_TYPE;

    //
    // first retrieve the correct notification context from a soundbank or a cue
    //

    if (pNotificationDesc->pSoundBank) {

        pContext = ((CSoundBank *)pNotificationDesc->pSoundBank)->GetNotificationContext(dwType);

    } else if (pNotificationDesc->pSoundCue) {

        //
        // tell the cue to handle the registration
        //

        pContext = ((CSoundCue *)pNotificationDesc->pSoundCue)->GetNotificationContext(dwType);

    }

    ASSERT(pContext);

    //
    // check if a notification of the same type is already registered
    //
    
#if DBG
    if (bRegister && pContext->bRegistered) {
        
        DPF_WARNING("Notification type %d already registered",
            dwType);
        
    }
    
    if (!bRegister && !pContext->bRegistered) {
        
        DPF_WARNING("Notification type %d never registered",
            dwType);
        
    }
#endif
    
    if (bRegister && !pContext->bRegistered) {
        
        //
        // this could be the first registration for this event/object combo
        //
        
        InitializeListHead(&pContext->ListEntry);
        InitializeListHead(&pContext->lstRegisteredCues);
        
    }
    
    //
    // save the notification description
    //
    
    memcpy(&pContext->PendingNotification.Header,
        pNotificationDesc,
        sizeof(XACT_NOTIFICATION_DESCRIPTION));
    
    
    pContext->bRegistered = bRegister;
    
    if (!IsListEmpty(&pContext->ListEntry)) {
        RemoveEntryList(&pContext->ListEntry);
    }

    if (pNotificationDesc->dwSoundCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) {

        PCUE_INDEX_NOTIFICATION_CONTEXT pCueContext;

        //
        // create a link list of cue indices that are registred for this event type on
        // the soundbank. Then when play is called on the soundbank, using the same CueIndex,
        // turn around and register a notification with the particular cue instance
        //

        pCueContext = GetCueNotificationContext(pContext,pNotificationDesc->dwSoundCueIndex);

        if (bRegister) {                        
            
            if (!pCueContext) {
                
                pCueContext = NEW(CUE_INDEX_NOTIFICATION_CONTEXT);
                
                if (pCueContext) {
                    
                    pCueContext->dwSoundCueIndex = pNotificationDesc->dwSoundCueIndex;
                    InitializeListHead(&pCueContext->ListEntry);
                    InsertTailList(&pContext->lstRegisteredCues,
                        &pCueContext->ListEntry);

                    pCueContext->bPersist = pNotificationDesc->dwType & XACT_FLAG_NOTIFICATION_PERSIST;
                }

            } else {

                DPF_WARNING("SoundCue index %d already registered on soundbank 0x%x",
                    pNotificationDesc->dwSoundCueIndex,
                    pContext->PendingNotification.Header.pSoundBank);

            }

        } else {

            if(!pCueContext) {

                DPF_WARNING("SoundCue index %d never registered on soundbank 0x%x",
                    pNotificationDesc->dwSoundCueIndex,
                    pContext->PendingNotification.Header.pSoundBank);

            } else {

                RemoveEntryList(&pCueContext->ListEntry);
                DELETE(pCueContext);

            }

        }

    }

    return;

}

PCUE_INDEX_NOTIFICATION_CONTEXT
CEngine::GetCueNotificationContext(PNOTIFICATION_CONTEXT pContext,DWORD dwSoundCueIndex)
{

    PLIST_ENTRY pEntry;
    PCUE_INDEX_NOTIFICATION_CONTEXT pCueContext = NULL;

    pEntry = pContext->lstRegisteredCues.Flink;
    while (pEntry && pEntry != &pContext->lstRegisteredCues) {

        pCueContext = CONTAINING_RECORD(pEntry,
            CUE_INDEX_NOTIFICATION_CONTEXT,
            ListEntry);
        
        if (pCueContext->dwSoundCueIndex == dwSoundCueIndex) {
            break;
        } else {
            pCueContext = NULL;
        }

        pEntry = pEntry->Flink;

    }

    return pCueContext;
}


VOID CEngine::IsDuplicateWaveBank(CWaveBank *pWaveBank)
{

#if DBG
    //
    // check if this wavebank has been registered before
    //
    
    CWaveBank *pExistingWaveBank;
    PLIST_ENTRY pEntry = m_lstWaveBanks.Flink;
    while (pEntry != &m_lstWaveBanks) {
        
        pExistingWaveBank = CONTAINING_RECORD(pEntry,CWaveBank,m_ListEntry);
        if (!strncmp(pExistingWaveBank->m_WaveBankData.pHeader->szBankName,
            pWaveBank->m_WaveBankData.pHeader->szBankName,
            WAVEBANKHEADER_BANKNAME_LENGTH)) {
            
            DPF_ERROR("Same wavebank (%s) has already been registered",
                pWaveBank->m_WaveBankData.pHeader->szBankName);
            
            break;
            
        }

        pEntry = pEntry->Flink;
        
    }
#endif
    
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// external methods
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#undef DPF_FNAME
#define DPF_FNAME "CEngine::LoadDspImage"

HRESULT CEngine::LoadDspImage(PVOID pvBuffer, DWORD dwSize, LPCDSEFFECTIMAGELOC pEffectLoc)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pvBuffer)
    {
        DPF_ERROR("No DSP image buffer supplied");
    }

    if (dwSize == 0)
    {
        DPF_ERROR("Invalid DSP image size");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Download the image, save the description
    //
    
    hr = m_pDirectSound->DownloadEffectsImage(pvBuffer, dwSize, pEffectLoc, &m_pDspImageDesc);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::CreateSoundSource"

HRESULT CEngine::CreateSoundSource(DWORD dwFlags,PXACTSOUNDSOURCE *ppSoundSource)
{
    HRESULT hr = S_OK;
    CSoundSource *pSoundSource;
    PLIST_ENTRY pEntry;

    ASSERT_IN_PASSIVE;
    DPF_ENTER();

    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!ppSoundSource)
    {
        DPF_ERROR("No ppSoundSource supplied");
    }

    if (!IsValidSoundSourceFlags(dwFlags)) {
        DPF_ERROR("Invalid sound source flags specified");
    }

#endif // VALIDATE_PARAMETERS

    //
    // give the caller one of the pre-allocated voices based
    // on the type they are requesting
    //

    if (dwFlags & XACT_FLAG_SOUNDSOURCE_3D) {
        ASSERT(!IsListEmpty(&m_lstAvailable3DBuffers));
        pEntry = RemoveHeadList(&m_lstAvailable3DBuffers);
    } else if (dwFlags & XACT_FLAG_SOUNDSOURCE_2D) {
        ASSERT(!IsListEmpty(&m_lstAvailable2DBuffers));
        pEntry = RemoveHeadList(&m_lstAvailable2DBuffers);
    }
    
    pSoundSource = CONTAINING_RECORD(pEntry, CSoundSource, m_ListEntry);
    *ppSoundSource = pSoundSource;

    pSoundSource->AddRef();

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::CreateSoundSourceInternal"

HRESULT CEngine::CreateSoundSourceInternal(DWORD dwFlags,CWaveBank *pWaveBank, CSoundSource **ppSoundSource)
{
    HRESULT hr = S_OK;
    CSoundSource *pSoundSource = NULL;
    PXACTSOUNDSOURCE pXactSoundSource;
    CWaveBank *pWaveBank2 = NULL;

    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

    DPF_ENTER();
    ASSERT(ppSoundSource);

    if (pWaveBank == NULL) {

        hr = CreateSoundSource(dwFlags,&pXactSoundSource);
        pSoundSource = (CSoundSource *) pXactSoundSource;

    } else {

        //
        // try to get the voice from the appropriate wavebank
        // the wavebank will try to grab one from the engine internal lists
        // so dont look there if this fails
        //

        hr = pWaveBank->AllocateSoundSource(&pSoundSource);

        if(FAILED(hr)) {

            //
            // ok we are out of voices in the engine as well.
            // try ANY wavebank for an available voice
            //

            PLIST_ENTRY pEntry = m_lstWaveBanks.Flink;
            while (pEntry != &m_lstWaveBanks)
            {

                pWaveBank2 = CONTAINING_RECORD(pEntry, CWaveBank, m_ListEntry);
                hr = pWaveBank2->AllocateSoundSource(&pSoundSource);

                pEntry = pEntry->Flink;

                if (SUCCEEDED(hr)) {

                    //
                    // set the owner to be the wavebank they requested originally since
                    // we want the sound source to be freed to that wavebank, not where
                    // it was allocated from
                    //
                    
                    pSoundSource->SetWaveBankOwner(pWaveBank);

                    break;
                }

            }
            
        }

    }
    
    if (FAILED(hr)) {
        
        //
        // TODO: use priority to steal voices from another sound
        //

    }
    
    *ppSoundSource = pSoundSource;

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::CreateSoundBank"

HRESULT CEngine::CreateSoundBank(PVOID pvBuffer, DWORD dwSize, PXACTSOUNDBANK *ppSoundBank)
{
    HRESULT hr = S_OK;
    CSoundBank* pSoundBank;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pvBuffer)
    {
        DPF_ERROR("No pvBuffer supplied");
    }

    if (dwSize == 0)
    {
        DPF_ERROR("Invalid buffer size");
    }

    if(!ppSoundBank)
    {
        DPF_ERROR("No ppSoundBank supplied");
    }

#endif // VALIDATE_PARAMETERS


    //
    // create the sound bank object
    //

    hr = HRFROMP(pSoundBank = NEW(CSoundBank));
    
    if(SUCCEEDED(hr))
    {
        hr = pSoundBank->Initialize(pvBuffer,dwSize);
    }
    
    if(SUCCEEDED(hr))
    {
        *ppSoundBank = pSoundBank;
    }
    else
    {
        pSoundBank->Release();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::RegisterWaveBank"

HRESULT CEngine::RegisterWaveBank(PVOID pvData, DWORD dwSize, PXACTWAVEBANK *ppWaveBank)
{
    HRESULT hr = S_OK;
    CWaveBank *pWaveBank;    

    DPF_ENTER();

    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pvData)
    {
        DPF_ERROR("No pvData supplied");
    }

    if (dwSize == 0)
    {
        DPF_ERROR("Invalid buffer size");
    }

    if(!ppWaveBank)
    {
        DPF_ERROR("No ppWaveBank supplied");
    }

#endif // VALIDATE_PARAMETERS

    *ppWaveBank = NULL;

    //
    // create a wrapper object to track this wavebank
    //
    
    hr = HRFROMP(pWaveBank = NEW(CWaveBank));

    if(SUCCEEDED(hr))
    {

        hr = pWaveBank->Initialize(pvData,dwSize);

    }

    if(SUCCEEDED(hr)) {

        //
        // add wave wank to our linked list of banks
        //

        IsDuplicateWaveBank(pWaveBank);

        InsertTailList(&m_lstWaveBanks,&pWaveBank->m_ListEntry);
        *ppWaveBank = (PXACTWAVEBANK) pWaveBank;

    } else {

        pWaveBank->Release();

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::RegisterStreamedWaveBank"

HRESULT CEngine::RegisterStreamedWaveBank(PVOID pvStreamingBuffer, DWORD dwSize, HANDLE hFileHandle, DWORD dwOffset, PXACTWAVEBANK *ppWaveBank)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pvStreamingBuffer)
    {
        DPF_ERROR("No pvStreamingBuffer supplied");
    }

    if (dwSize == 0)
    {
        DPF_ERROR("Invalid buffer size");
    }

    if(!hFileHandle)
    {
        DPF_ERROR("Invalid hFileHandle");
    }

    if(!ppWaveBank)
    {
        DPF_ERROR("No ppWaveBank supplied");
    }

#endif // VALIDATE_PARAMETERS

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::UnRegisterWaveBank"

HRESULT CEngine::UnRegisterWaveBank(PXACTWAVEBANK pWaveBankInstance)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ASSERT_IN_PASSIVE;
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS
    if(!pWaveBankInstance)
    {
        DPF_ERROR("No pWaveBankInstance supplied");
    }    
#endif // VALIDATE_PARAMETERS

    CWaveBank *pWaveBank = (CWaveBank *) pWaveBankInstance;

    //
    // remove wave bank from our registered list
    //

    RemoveEntryList(&pWaveBank->m_ListEntry);

    pWaveBank->StopAllCues();

    ASSERT(pWaveBank->m_dwRefCount == 1);

    //
    // release object
    //

    pWaveBank->Release();

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::SetMasterVolume"

HRESULT CEngine::SetMasterVolume(LONG lVolume)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS
    if((lVolume < DSBVOLUME_MIN) || (lVolume > DSBVOLUME_MAX))
    {
        DPF_ERROR("Invalid lVolume (has to be within dsound specifed volume range");
    }
#endif // VALIDATE_PARAMETERS

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::SetListenerParameters"

HRESULT CEngine::SetListenerParameters(LPCDS3DLISTENER pcDs3dListener, LPCDSI3DL2LISTENER pcds3dl, DWORD dwApply)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS
    if ((pcDs3dListener == NULL) && (pcds3dl == NULL)){
        DPF_ERROR("You must supply at least one set of listener parameters");
    }
#endif

    if (pcDs3dListener) {
        hr = m_pDirectSound->SetAllParameters(pcDs3dListener,dwApply);
    }

    if (pcds3dl && SUCCEEDED(hr)) {

        hr = m_pDirectSound->SetI3DL2Listener(pcds3dl,dwApply);

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::GlobalPause"

HRESULT CEngine::GlobalPause(BOOL bPause)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    DPF_LEAVE_HRESULT(hr);

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::RegisterNotification"

HRESULT CEngine::RegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{
    HandleNotificationRegistration(pNotificationDesc, TRUE);
    return S_OK;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::UnRegisterNotification"

HRESULT CEngine::UnRegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{    
    HandleNotificationRegistration(pNotificationDesc, FALSE);
    return S_OK;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::GetNotification"

HRESULT CEngine::GetNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc,PXACT_NOTIFICATION pNotification)
{
    HRESULT hr = S_OK;
    PNOTIFICATION_CONTEXT pContext = NULL;    

    DPF_ENTER();
    AutoIrql();
    
#ifdef VALIDATE_PARAMETERS

    if(!pNotificationDesc)
    {
        DPF_ERROR("No pNotificationDesc supplied");
    }

    ASSERT(!(pNotificationDesc->dwType & XACT_MASK_NOTIFICATION_FLAGS));

    if(!pNotification)
    {
        DPF_ERROR("No pNotification supplied");
    }

    if (pNotificationDesc->pSoundBank && pNotificationDesc->pSoundCue) {

        DPF_ERROR("You cant specify a notification desc that has both pSoundBank and pSoundCue");

    }

    if (pNotificationDesc->dwSoundCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) {

        DPF_WARNING("dwSoundCueIndex is ignored when calling this API. Set to -1");

    }
        
    if ((pNotificationDesc->dwType != XACT_NOTIFICATION_TYPE_UNUSED) && 
        (pNotificationDesc->dwType >= eXACTNotification_Max)) {

        DPF_ERROR("Invalid notification type specified (%d)",
            pNotificationDesc->dwType);

    }

    if ((pNotificationDesc->dwType == XACT_NOTIFICATION_TYPE_UNUSED) && 
        (pNotificationDesc->pSoundBank || pNotificationDesc->pSoundCue)) {

        DPF_ERROR("dwType must be valid if pSoundBank or pSoundCue is supplied");

    }

#endif // VALIDATE_PARAMETERS

    //
    // get a notification from our linked list or soundbank,soundcue
    // based on the criteria specified
    //

    if (pNotificationDesc->dwType == XACT_NOTIFICATION_TYPE_UNUSED) {

        PLIST_ENTRY pEntry;

        //
        // retrieve the next notification regadless of type
        //

        pEntry = m_lstPendingNotifications.Flink;
        while (pEntry != &m_lstPendingNotifications) {

            pContext = CONTAINING_RECORD(pEntry,NOTIFICATION_CONTEXT,ListEntry);
            break;

        }

    } else if (pNotificationDesc->pSoundBank) {
        CSoundBank *pSoundBank = (CSoundBank *) pNotificationDesc->pSoundBank;
        pContext = pSoundBank->GetNotificationContext(pNotificationDesc->dwType);        
    } else {
        CSoundCue *pSoundCue = (CSoundCue *) pNotificationDesc->pSoundCue;
        pContext = pSoundCue->GetNotificationContext(pNotificationDesc->dwType);
    }

    if (IsListEmpty(&pContext->ListEntry)) {

        //
        // this context does not contain a signalled event.
        // only contexts that belong to the global notification list have 
        // pending notifications
        //

        pContext = NULL;
    }

#if DBG
    if (pContext && !pContext->bRegistered) {
        DPF_WARNING("Attempting to retrieve notification type that was never registers");
    }           
#endif

    if (pContext && pContext->bRegistered) {

        RemoveEntryList(&pContext->ListEntry);
        if (!(pContext->PendingNotification.Header.dwType & XACT_FLAG_NOTIFICATION_PERSIST)){
            
            //
            // auto-unregister notification
            //
            
            pContext->bRegistered = FALSE;
            
        }

        //
        // copy pending notification to user-supplied buffer
        //

        memcpy(pNotification,
            &pContext->PendingNotification,
            sizeof(XACT_NOTIFICATION));
                
        
    }
   
    if (!pContext)
        hr = E_FAIL;

    DPF_LEAVE_HRESULT(hr);
    return hr;
}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::FlushNotification"

HRESULT CEngine::FlushNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{
    HRESULT hr = S_OK;
    PLIST_ENTRY pEntry;
    PNOTIFICATION_CONTEXT pContext = NULL;
    BOOL bFlush = FALSE;
    
    DPF_ENTER();
    
    AutoIrql();

#ifdef VALIDATE_PARAMETERS

    if(!pNotificationDesc)
    {
        DPF_ERROR("No pNotificationDesc supplied");
    }

    ASSERT(pNotificationDesc->dwType & XACT_MASK_NOTIFICATION_FLAGS);

    if ((pNotificationDesc->dwType != XACT_NOTIFICATION_TYPE_UNUSED) && 
        (pNotificationDesc->dwType >= eXACTNotification_Max)) {

        DPF_ERROR("Invalid notification type specified (%d)",
            pNotificationDesc->dwType);

    }

    if (pNotificationDesc->pSoundBank && pNotificationDesc->pSoundCue) {

        DPF_ERROR("You cant specify a notification desc that has both pSoundBank and pSoundCue");

    }

    if ((pNotificationDesc->dwSoundCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) &&
        !pNotificationDesc->pSoundBank){

        DPF_WARNING("You must supply pSoundBank if dwSoundCueIndex is valid");

    }

#endif // VALIDATE_PARAMETERS

    if (pNotificationDesc->pSoundBank &&
        (pNotificationDesc->dwSoundCueIndex != XACT_SOUNDCUE_INDEX_UNUSED)) {
        
        PCUE_INDEX_NOTIFICATION_CONTEXT pCueContext;
        CSoundBank *pSoundBank = (CSoundBank *) pNotificationDesc->pSoundBank;

        //
        // soundCueIndex was specified which means we need to remove it from the soundbanks
        // list of registered cue indices
        //

        pContext = pSoundBank->GetNotificationContext(pNotificationDesc->dwType);        
        pCueContext = GetCueNotificationContext(pContext,pNotificationDesc->dwSoundCueIndex);
        if (pCueContext && !(pCueContext->bPersist)) {

            RemoveEntryList(&pCueContext->ListEntry);
            DELETE(pCueContext);

        }
        
    }

    //
    // flush the appropriate pending notifications
    //

    pEntry = m_lstPendingNotifications.Flink;
    while (pEntry != &m_lstPendingNotifications) {
        
        pContext = CONTAINING_RECORD(pEntry,NOTIFICATION_CONTEXT,ListEntry);
        if (pNotificationDesc->dwType == XACT_NOTIFICATION_TYPE_UNUSED)
        {
            bFlush = TRUE;

        } else if (pNotificationDesc->dwType == 
            (pContext->PendingNotification.Header.dwType & XACT_MASK_NOTIFICATION_TYPE)){

            bFlush = TRUE;
        }

        //
        // flush any notification regadless of type. If pSoundBank is supplied
        // flush all notification associated with that soundbank. Same with pSoundCue
        //
        
        if (pContext->PendingNotification.Header.pSoundBank &&
            (pContext->PendingNotification.Header.pSoundBank != pNotificationDesc->pSoundBank)) {
            
            bFlush = FALSE;
            
        }

        if (pContext->PendingNotification.Header.pSoundCue && 
            (pContext->PendingNotification.Header.pSoundCue != pNotificationDesc->pSoundCue)) {
            
            bFlush = FALSE;
            
        }

        if (bFlush) {

            RemoveEntryList(&pContext->ListEntry);
            if (!(pContext->PendingNotification.Header.dwType & XACT_FLAG_NOTIFICATION_PERSIST)){

                //
                // unregister notification
                //

                pContext->bRegistered = FALSE;

            }

        }
        
    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::CommitDefferedSettings"

HRESULT CEngine::CommitDeferredSettings()
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    hr = m_pDirectSound->CommitDeferredSettings();

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::ScheduleEvent"

HRESULT CEngine::ScheduleEvent(XACT_TRACK_EVENT *pEventDesc, PXACTSOUNDCUE pSoundCueObject, DWORD dwTrackIndex)
{
    HRESULT hr = S_OK;
    PTRACK_EVENT_CONTEXT pEventContext;
    PXACT_TRACK_EVENT pEvent;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS    
    if(!pEventDesc)
    {
        DPF_ERROR("No pEvent supplied");
    }

    if (pEventDesc->Header.wType >= eXACTEvent_Max)
    {
        DPF_ERROR("Invalid Event type");
    }

    if (pSoundCueObject == NULL) {

        if (pEventDesc->Header.wType != eXACTEvent_SetEffectData) {
            DPF_ERROR("pSoundCue == NULL and eventType == eXACTEvent_SetEffectData is not a valid global event");
        }

        if (dwTrackIndex != XACT_TRACK_INDEX_UNUSED) {

            DPF_ERROR("pSoundCue must be != NULL if a valid dwTrackIndex is supplied");

        }
    }

    if (pSoundCueObject != NULL) {

        //
        // check if the event they are submitting is valid for runtime submission
        //

        if (pEventDesc->Header.wType == eXACTEvent_Play) {
            DPF_ERROR("Play is not a valid event type when submitting events through API");
        }
    }
    
#endif // VALIDATE_PARAMETERS
           
    if (SUCCEEDED(hr)) {

        //
        // pSoundCue is optional since this event can be a global event
        // such as SetEffectData that is not associated with a specific soundsource
        // If pSoundCue is supplied, based on the type of event, we modify source or target voice
        // since the user cant specify a track, only single track sounds are acceptable...
        //
        
        if (pSoundCueObject) {
            
            CSoundCue *pSoundCue = (CSoundCue *) pSoundCueObject;            
            hr = pSoundCue->ScheduleRuntimeEvent(pEventDesc,dwTrackIndex);
            
        } else {

            //
            // create a dummy track context only so we can use the 
            // CreateEventTimestamp function
            //

            TRACK_CONTEXT track;

            memset(&track,0,sizeof(track));            

            //
            // create an event context for this event
            //
            
            hr = HRFROMP(pEventContext = NEW(TRACK_EVENT_CONTEXT));
            
            if (SUCCEEDED(hr)) {
                
                hr = HRFROMP(pEvent = NEW(XACT_TRACK_EVENT));
                
            }
            
            if (SUCCEEDED(hr)) {
                
                //
                // copy user event desc
                //
                
                memcpy(pEvent,pEventDesc,sizeof(XACT_TRACK_EVENT));
                
                //
                // setup the event context
                //
                
                pEventContext->m_pEventHeader = &pEvent->Header;
                
            }
            
            track.wSamplesPerSec = 48000;
            KeQuerySystemTime((PLARGE_INTEGER)&track.rtStartTime);
            InitializeListHead(&track.lstEvents);

            CreateEventTimeStamp(&track,pEventContext);

            //
            // enqueue this event
            //

            hr = Enqueue(pEventContext);

        }
        
    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}






