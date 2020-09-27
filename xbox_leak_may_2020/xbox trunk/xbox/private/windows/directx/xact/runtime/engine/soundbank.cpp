/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       soundbank.cpp
 *  Content:    XACT runtime soundbank object implementation
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/22/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"

using namespace XACT;


#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::CSoundBank"


CSoundBank::CSoundBank
(
    void
)
{
    DPF_ENTER();

    InitializeListHead(&m_lstCues);
    InitializeListHead(&m_ListEntry);

    ASSERT(g_pEngine);
    g_pEngine->AddRef();

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::~CSoundBank"

CSoundBank::~CSoundBank
(
    void
)
{
    PLIST_ENTRY pEntry;
    CSoundCue *pCue;

    DPF_ENTER();

    ASSERT(g_pEngine);
    g_pEngine->Release();

    //
    // if we still have cues around, delete them here
    //

    pEntry = m_lstCues.Flink;
    while (pEntry != &m_lstCues) 
    {
        pCue = CONTAINING_RECORD(pEntry,CSoundCue,m_ListEntry);        
        pEntry = pEntry->Flink;        
        
        pCue->Stop(XACT_FLAG_SOUNDCUE_SYNCHRONOUS);
        
    }

    //
    // remove any registered cue notifications
    //

    PCUE_INDEX_NOTIFICATION_CONTEXT pCueContext;
    PNOTIFICATION_CONTEXT pContext;
    
    for (DWORD i=0;i<eXACTNotification_Max;i++) {
        
        pContext = GetNotificationContext(i);

        for (DWORD j = 0; j < m_pFileHeader->dwCueEntryCount; j++) {
            
            pCueContext = g_pEngine->GetCueNotificationContext(pContext,j);
            if (pCueContext) {
                DELETE(pCueContext);
            }

        }

    }

    DPF_LEAVE_VOID();
}

__inline ULONG CSoundBank::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CSoundBank::AddRef");    
    return CRefCount::AddRef();
}


__inline ULONG CSoundBank::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CSoundBank::Release");
    return CRefCount::Release();
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::Initialize"

HRESULT CSoundBank::Initialize(PVOID pvBuffer, DWORD dwSize)
{
    HRESULT hr = S_OK;

    DPF_ENTER();

    m_pDataBuffer = pvBuffer;
    m_dwDataSize = dwSize;

    m_pFileHeader = (PXACT_SOUNDBANK_FILE_HEADER)m_pDataBuffer;

#ifdef VALIDATE_PARAMETERS
    if (!IsValidHeader()) {
        DPF_ERROR("Invalid Soundbank header (0x%x)",pvBuffer);
    }
#endif

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::GetSoundCueIndexFromFriendlyName"

HRESULT CSoundBank::GetSoundCueIndexFromFriendlyName(LPCSTR lpFriendlyName, PDWORD pdwCueIndex)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!lpFriendlyName)
    {
        DPF_ERROR("No lpFriendlyName specified");
    }

    if(!pdwCueIndex)
    {
        DPF_ERROR("No pdwCueIndex supplied");
    }

#endif // VALIDATE_PARAMETERS

    //
    // search for matching friendly name among cues
    // this is not done super efficiently since we dont expect this function
    // to be used on retail builds
    // caller should use hardcoded CUE index 
    //

    *pdwCueIndex = -1;

    for (DWORD i= 0;i < m_pFileHeader->dwCueEntryCount; i++) 
    {

        if (!strncmp(lpFriendlyName,
            GetCueFriendlyName(i),
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH)) {

            *pdwCueIndex = i;
            break;
        }

    }

    if (*pdwCueIndex == -1) {
        DPF_WARNING("Cue friendly name %s not found in cue table for soundbank 0x%x",lpFriendlyName,this);
        hr = E_FAIL;
    }

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::Play"

HRESULT CSoundBank::Play( DWORD dwCueIndex, PXACTSOUNDSOURCE pSoundSourceObject,DWORD dwFlags, PXACTSOUNDCUE *ppCue)
{
    HRESULT hr = S_OK;
    CSoundSource *pSoundSource = (CSoundSource *) pSoundSourceObject;
    CSoundCue *pCue = NULL, *pNewCue = NULL;

    PXACT_SOUNDBANK_CUE_ENTRY pCueTable = GetCueTable();
    PXACT_SOUNDBANK_SOUND_ENTRY pSoundTable = GetSoundTable();


    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!IsValidCue(dwCueIndex))
    {
        DPF_ERROR("Cue index is not valid for this soundbank");
    }


    if(pSoundSource && 
        !IsValidSoundSourceForSound(pSoundSource->GetFlags(),
        pSoundTable[pCueTable[dwCueIndex].dwSoundIndex].dwFlags))
    {
        DPF_ERROR("SoundSource type (0x%x) does not match sound (0x%x) type 0x%x",
                  pSoundSource->GetFlags(),
                  pCueTable[dwCueIndex].dwSoundIndex,
                  pSoundTable[pCueTable[dwCueIndex].dwSoundIndex].dwFlags);

    }

    if (!ppCue && !(dwFlags & XACT_FLAG_SOUNDCUE_AUTORELEASE)) {

        DPF_ERROR("XACT_FLAG_SOUNDCUE_AUTORELEASE must be set if ppCue == NULL");

    }

    if (ppCue && (dwFlags & XACT_FLAG_SOUNDCUE_AUTORELEASE)) {

        DPF_ERROR("No need to supply ppCue if XACT_FLAG_SOUNDCUE_AUTORELEASE is set");

    }

#endif // VALIDATE_PARAMETERS

    //
    // search for any other cues associated with the supplied sound source
    //
    
    
    PLIST_ENTRY pEntry = m_lstCues.Flink;
    while (pEntry != &m_lstCues) 
    {
        pCue = CONTAINING_RECORD(pEntry,CSoundCue,m_ListEntry);
        pEntry = pEntry->Flink;        
        
        //
        // if another cue of the same layer is playing and a source was supplied
        // enforce the layering behavior
        // if now SoundSource was supplied but another cue of the same layer is playing
        
        if (pCue->m_pSoundEntry->wLayer == pSoundTable[pCueTable[dwCueIndex].dwSoundIndex].wLayer)
        {
      
            if (pSoundSource == pCue->m_pControlSoundSource) {
                
                //
                // the cues have the same layer and same voice which means the existing cue
                // must be stopped.
                //
                
                hr = pCue->Stop(XACT_FLAG_SOUNDCUE_SYNCHRONOUS);
                if (FAILED(hr)){
                    break;
                }
                
                //
                // after calling stop on a cue you CANT assume its still allocated
                //

            }                        
            
        }                
        
    }
    
    if (SUCCEEDED(hr) && !pNewCue)
    {
        //
        // create a Cue instance
        //
        
        hr = HRFROMP(pNewCue = NEW(CSoundCue));
        
        if(SUCCEEDED(hr))
        {
            hr = pNewCue->Initialize(this, dwCueIndex, pSoundSource);
        }
        
        if (FAILED(hr))
        {
            pNewCue->Release();
            pNewCue = NULL;
        }
        
        if(SUCCEEDED(hr))
        {        

            //
            // add cue to our list
            //
            
            InsertTailList(&m_lstCues,&pNewCue->m_ListEntry);
            
        }
        
        if (SUCCEEDED(hr) && pSoundSource && pSoundSource->IsPositional()) {
            
            //
            // set the 3d properties associated with the sound, on the destination voice
            //
            
            PXACT_SOUNDBANK_SOUND_ENTRY pSoundEntry = &pSoundTable[pCueTable[dwCueIndex].dwSoundIndex];            
            
            if (pSoundEntry->dw3DParametersOffset != -1) {
                
                PXACT_SOUNDBANK_SOUND_3D_PARAMETERS pParams = (PXACT_SOUNDBANK_SOUND_3D_PARAMETERS)
                    ((PUCHAR)m_pDataBuffer + pSoundEntry->dw3DParametersOffset);
                
                DS3DBUFFER ds3db;
                ZeroMemory(&ds3db,sizeof(DS3DBUFFER));
                
                ds3db.dwSize = sizeof(DS3DBUFFER);
                ds3db.dwInsideConeAngle = pParams->dwInsideConeAngle;
                ds3db.dwOutsideConeAngle = pParams->dwOutsideConeAngle;
                ds3db.flMaxDistance = pParams->flMaxDistance;
                ds3db.flMinDistance = pParams->flMinDistance;
                ds3db.dwMode = pParams->dwMode;
                ds3db.lConeOutsideVolume = pParams->lConeOutsideVolume;
                ds3db.flDistanceFactor = pParams->flDistanceFactor;
                ds3db.flDopplerFactor = pParams->flDopplerFactor;
                ds3db.flRolloffFactor = pParams->flRolloffFactor;
                ds3db.vConeOrientation.x = 1;
                
                hr = pSoundSource->m_HwVoice.pBuffer->SetAllParameters(&ds3db,DS3D_DEFERRED);
                
            }
            
        }

    } //if pNewCue

    if (SUCCEEDED(hr)) {

        if (ppCue) {

            *ppCue = (PXACTSOUNDCUE) pNewCue;

            //
            // handle registration for notifications associated with a CueIndex
            // Since the caller cant register a notification for a cue before it starts playing
            // it instead registers one on the soundbank, for a specific cue index.
            // then when the time comes to play the cue, we register the notification on the specific cue
            //

            PCUE_INDEX_NOTIFICATION_CONTEXT pCueContext;
            PNOTIFICATION_CONTEXT pContext;

            for (DWORD i=0;i<eXACTNotification_Max;i++) {

                pContext = GetNotificationContext(i);

                pCueContext = g_pEngine->GetCueNotificationContext(pContext,dwCueIndex);
                if (pContext->bRegistered && pCueContext) {

                    XACT_NOTIFICATION_DESCRIPTION Desc;

                    memcpy(&Desc,
                        &pContext->PendingNotification.Header,
                        sizeof(Desc));

                    Desc.pSoundCue = (PXACTSOUNDCUE) pNewCue;
                    Desc.pSoundBank = NULL;
                    Desc.dwSoundCueIndex = XACT_SOUNDCUE_INDEX_UNUSED;

                    //
                    // register for this type of notification, on the cue
                    //

                    g_pEngine->HandleNotificationRegistration(&Desc,TRUE);

                }

            }

        }

    }

    if (SUCCEEDED(hr)) {

        //
        // play the cue
        //
        
        hr = pNewCue->Play(dwFlags);
        
    }


    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::Play"

HRESULT CSoundBank::Stop( DWORD dwCueIndex, DWORD dwFlags, PXACTSOUNDCUE pCueObject)
{
    HRESULT hr = S_OK;
    CSoundCue *pCue = (CSoundCue *) pCueObject;
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((dwCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) && !IsValidCue(dwCueIndex))
    {
        DPF_ERROR("Cue index is not valid for this soundbank");
    }

    if((dwCueIndex == XACT_SOUNDCUE_INDEX_UNUSED) && (pCueObject != NULL))
    {
        DPF_ERROR("You cant supply a valid cue instance with an unspecified cue index");
    }

    if((dwCueIndex != XACT_SOUNDCUE_INDEX_UNUSED) && (pCueObject != NULL))
    {
        DPF_ERROR("You cant supply both a valid cue instance AND a valid cue index");
    }

    if (dwFlags & XACT_FLAG_SOUNDCUE_AUTORELEASE) {
        DPF_ERROR("XACT_FLAG_SOUNDCUE_AUTORELEASE is not valid for this API");
    }

#endif

    //
    // instead of calling release after calling stop, instruct the cue to free itself
    // this solves the issue of a cue already being autorelease, us calling stop, cue
    // freeing itself then us try to release again
    //

    dwFlags |= XACT_FLAG_SOUNDCUE_AUTORELEASE; 

    if (pCue == NULL) {

        PLIST_ENTRY pEntry = m_lstCues.Flink;
        while (pEntry != &m_lstCues) 
        {
            pCue = CONTAINING_RECORD(pEntry,CSoundCue,m_ListEntry);
            pEntry = pEntry->Flink;        

            //
            // if the voice has already been associated with another cue, check the layer
            // of the current cue with the layer of the already associated cue
            //
            
            if ((pCue->m_dwCueIndex == dwCueIndex) ||
                (dwCueIndex == XACT_SOUNDCUE_INDEX_UNUSED))
            {                
                hr = pCue->Stop(dwFlags);
            }                        
            
        }

    } else {

        hr = pCue->Stop(dwFlags);

    }
    
            
    DPF_LEAVE_HRESULT(hr);
    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::GetNotificationContext"

PNOTIFICATION_CONTEXT CSoundBank::GetNotificationContext(DWORD dwType)
{

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();    
    return &m_aNotificationContexts[dwType];

}
 

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::ProcessRuntimeEvent"

VOID CSoundBank::ProcessRuntimeEvent(XACT_TRACK_EVENT *pEventDesc)
{
    DWORD dwNotificationEvent;
    BOOL bHandledLocally = FALSE;
    PNOTIFICATION_CONTEXT pContext = NULL;

    AutoIrql();
    switch (pEventDesc->Header.wType) {

    case eXACTEvent_PlayWithPitchAndVolumeVariation:
    case eXACTEvent_Play:

        dwNotificationEvent = eXACTNotification_Start;
        pContext = &m_aNotificationContexts[dwNotificationEvent];

        //
        // increment playing count
        //

        m_dwPlayingCount++;

        if (m_dwPlayingCount == 1) {

            //
            // when the very first cue starts playing for the first time
            // signal a notification that the soundbank has started playing
            //

            bHandledLocally = TRUE;
        }    

        break;

    case eXACTEvent_Stop:

        dwNotificationEvent = eXACTNotification_Stop;
        pContext = &m_aNotificationContexts[dwNotificationEvent];

        if(m_dwPlayingCount) {
            m_dwPlayingCount--;
        } else {
            DPF_INFO("(0x%x)Got stop event without ever getting play event",this);
        }
       
        if (m_dwPlayingCount == 0) {

            //
            // when all cues have stopped playing
            // signal a notification
            //

            bHandledLocally = TRUE;
        }

        break;
    }

    ASSERT(pContext);

    if (bHandledLocally) {

        //
        // the following routine will only add a notification if it has been registered
        //

        g_pEngine->AddNotificationToPendingList(pContext);

    }            

}

#undef DPF_FNAME
#define DPF_FNAME "CSoundBank::RemoveFromList"

void CSoundBank::RemoveFromList(CSoundCue *pCue)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
    RemoveEntryList(&pCue->m_ListEntry);
}

