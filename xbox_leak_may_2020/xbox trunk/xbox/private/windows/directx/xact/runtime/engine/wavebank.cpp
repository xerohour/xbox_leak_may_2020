/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wavebank.cpp
 *  Content:    XACT runtime wavebank object implementation
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/27/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"
#include "wavbndlr.h"

using namespace XACT;

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::CWaveBank"


CWaveBank::CWaveBank
(
    void
)
{
    DPF_ENTER();
    InitializeListHead(&m_ListEntry);
    InitializeListHead(&m_lstCues);
    InitializeListHead(&m_lstAvailableSources);

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::~CWaveBank"

CWaveBank::~CWaveBank
(
    void
)
{
    ENTER_EXTERNAL_METHOD();
    CSoundSource *pSource;

    DPF_ENTER();
    PLIST_ENTRY pEntry;

    g_pEngine->Release();
 
    m_WaveBankData.pvData = NULL;
    m_WaveBankData.dwDataSize = 0;

    pEntry = m_lstAvailableSources.Flink;
    while (pEntry != &m_lstAvailableSources){

        pSource = CONTAINING_RECORD(pEntry,CSoundSource,m_ListEntry);
        pEntry = pEntry->Flink;

        pSource->SetWaveBankOwner(NULL);

        //
        // tell dsound to release the page SGEs
        //

        if (LPDIRECTSOUNDBUFFER pBuffer = pSource->GetDSoundBuffer()) {
            pBuffer->SetBufferData(0,0);
        }

        RemoveEntryList(&pSource->m_ListEntry);
        pSource->Release();

    }

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::Initialize"

HRESULT CWaveBank::Initialize(PVOID pvData, DWORD dwSize)
{
    HRESULT hr = S_OK;
    DWORD dwOffset = 0;
    CSoundSource *pSource;

    ENTER_EXTERNAL_METHOD();
    DPF_ENTER();

    ASSERT(g_pEngine);    
    ASSERT(pvData);
    ASSERT(dwSize);

    g_pEngine->AddRef();

    hr = AllocateSoundSource(&pSource);

    if (SUCCEEDED(hr)) {

        m_WaveBankData.pHeader = (LPWAVEBANKHEADER)((PUCHAR)pvData+dwOffset);
        
#ifdef VALIDATE_PARAMETERS
        //
        // Validate the header
        //
        
        if (m_WaveBankData.pHeader->dwSignature  != WAVEBANKHEADER_SIGNATURE ||
            m_WaveBankData.pHeader->dwVersion    != WAVEBANKHEADER_VERSION ||
            m_WaveBankData.pHeader->dwEntryCount == 0)
        {
            DPF_ERROR("Invalid wavebank header (0x%x)", pvData);
            hr = E_FAIL;
        }
#endif
        
    }

    if (SUCCEEDED(hr)) {
        
        dwOffset += sizeof(WAVEBANKHEADER);
        m_WaveBankData.paMetaData = (LPWAVEBANKENTRY) ((PUCHAR)pvData + dwOffset);
        
        dwOffset += m_WaveBankData.pHeader->dwEntryCount*sizeof(WAVEBANKENTRY);
        m_WaveBankData.pvData = (PVOID) ((PUCHAR)pvData+dwOffset);
        m_WaveBankData.dwDataSize = dwSize - dwOffset;
        
        //
        // map one 2d voice to span the entire data buffer.
        // this makes dsound pre-allocate the SGEs required by all the waves in the bank
        // and minimizes latency when playing voices pointing to this wavebank later
        //

        hr = SetBufferData(pSource->GetDSoundBuffer());

    }

    //
    // the first voice is available for use by a cue since we just used it to map wave data
    // AllocateSoundSource addrefs the source one extra time to bring the voice total to at least 3
    // this way when its released it looks like it was released from a cue and it gets
    // freed back to the wavebank freelist
    //    

    if (SUCCEEDED(hr)) {

        pSource->Release();

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::AllocateSoundSource"

HRESULT CWaveBank::AllocateSoundSource(CSoundSource **ppSource)
{

    HRESULT hr = S_OK;
    ENTER_EXTERNAL_METHOD();

    PLIST_ENTRY pEntry;
    CSoundSource *pSource;

    while (TRUE && SUCCEEDED(hr)) {

        if (!IsListEmpty(&m_lstAvailableSources)) {
            
            pEntry = RemoveHeadList(&m_lstAvailableSources);
            pSource = CONTAINING_RECORD(pEntry,CSoundSource,m_ListEntry);
            
        } else {
            
            //
            // get one from the engine
            //
            
            hr = g_pEngine->CreateSoundSourceInternal(XACT_FLAG_SOUNDSOURCE_2D,NULL,&pSource);        
            
        }
        
        if (SUCCEEDED(hr)){
            
            pSource->AddRef();
            pSource->SetWaveBankOwner(this);
            *ppSource = pSource;
            
        }
        
        if (SUCCEEDED(hr) && pSource->IsPlaying()) {

            DPF_WARNING("Voice form available list was still playing in hw, attempting re-alloc of new one");

            //
            // hmm the free voice we got is still playing...
            // we d rather allocate a new one and leave this one alone for now
            //
            
            pSource->Release();
            
        } else {
            break;
        }

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::FreeSoundSource"

VOID CWaveBank::FreeSoundSource(CSoundSource *pSource)
{

    ENTER_EXTERNAL_METHOD();

    ASSERT(pSource);
    ASSERT(pSource->m_pWaveBankOwner == this);

    InsertTailList(&m_lstAvailableSources,&pSource->m_ListEntry);

}



#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::AddCueToList"

VOID CWaveBank::AddCueToList(PWAVEBANK_CUE_CONTEXT pEntry)
{
    ENTER_EXTERNAL_METHOD();
    InsertTailList(&m_lstCues,
        &pEntry->ListEntry);

}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::RemoveCueFromList"

VOID CWaveBank::RemoveCueFromList(PWAVEBANK_CUE_CONTEXT pEntry)
{
    ENTER_EXTERNAL_METHOD();
    RemoveEntryList(&pEntry->ListEntry);
}

#undef DPF_FNAME
#define DPF_FNAME "CWaveBank::StopAllCues"

VOID CWaveBank::StopAllCues()
{
    ENTER_EXTERNAL_METHOD();
    CSoundCue *pCue = NULL;
    PWAVEBANK_CUE_CONTEXT pCueEntry;

    //
    // stop all cues associated with this wavebank
    //

    PLIST_ENTRY pEntry = m_lstCues.Flink;
    while (pEntry != &m_lstCues){

        pCueEntry = CONTAINING_RECORD(pEntry,WAVEBANK_CUE_CONTEXT,ListEntry);
        pCue = pCueEntry->pSoundCue;
        pEntry = pEntry->Flink;

        DPF_WARNING("You are un-registering wavebank 0x%x still referenced by cue %s.\n"\
            "        This can cause breakup and glitching. Cue %x is no longer valid",
            this,
            pCue->GetFriendlyName(),
            pCue);

        pCue->Stop(XACT_FLAG_SOUNDCUE_SYNCHRONOUS | XACT_FLAG_SOUNDCUE_AUTORELEASE);

    }

}