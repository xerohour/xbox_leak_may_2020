/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       voice.cpp
 *  Content:    XACT runtime voice object implementation
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
#define DPF_FNAME "CSoundCue::CSoundCue"


CSoundCue::CSoundCue
(
    void
)
{
    DPF_ENTER();

    m_dwCueIndex = -1;
    InitializeListHead(&m_SeqListEntry);
    InitializeListHead(&m_ListEntry);

    m_dwState = CUE_STATE_CREATED;

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::~CSoundCue"

CSoundCue::~CSoundCue
(
    void
)
{
    DPF_ENTER();
    PLIST_ENTRY pEntry;
    CSoundCue *pCue;

    ENTER_EXTERNAL_METHOD();    

    //
    // remove from sequencer list
    //
    
    if (!IsListEmpty(&m_SeqListEntry)){
        g_pEngine->RemoveCueFromSequencerList(this);
    }

    //
    // remove us from the soundbank linked list
    //
    
    m_pSoundBank->RemoveFromList(this);
   
    //
    // release any wavebank references
    //

    PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY pWaveBankEntry = GetWaveBankTable();

    if (m_paWaveBankEntries) {

        for (DWORD i=0;i<m_pSoundEntry->wWaveBankCount;i++) {
            
            CWaveBank *pWaveBank = (CWaveBank *)pWaveBankEntry[i].dwDataOffset;
            pWaveBank->RemoveCueFromList(&m_paWaveBankEntries[i]);
            pWaveBank->Release();
            
        }

        DELETE_A(m_paWaveBankEntries);

    }

    if (m_paTracks) {
        
        //
        // go thorugh each track and free the voices we had allocated
        //
        
        for (DWORD i=0;i<m_pSoundEntry->wTrackCount;i++) {
            
            g_pEngine->FreeEventsAtOrAfter(&m_paTracks[i],0);
            
            if (m_paTracks[i].pSoundSource) {
                m_paTracks[i].pSoundSource->Release();        
            }
                        
        }
        
        DELETE(m_paTracks);
        
    }

    ASSERT(m_pSoundBank);
    
    //
    // free destination voice
    //

    if (m_pControlSoundSource) {
        m_pControlSoundSource->Release();
    }

    DPF_LEAVE_VOID();
}

__inline ULONG CSoundCue::AddRef(void)
{
    ENTER_EXTERNAL_METHOD("CCueInstance::AddRef");
    return CRefCount::AddRef();
}


ULONG CSoundCue::Release(void)
{
    ENTER_EXTERNAL_METHOD("CCueInstance::Release");

    DWORD dwCount = CRefCount::Release();
    if ((dwCount == 1) && (m_dwFlags & XACT_FLAG_SOUNDCUE_AUTORELEASE)){

        //
        // if auto release is set and refcount is down to one
        // it means that we are no longer in the sequencer list
        // and we can free ourselves
        //

        dwCount = Release();

    }

    return dwCount;
}



#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::Initialize"

HRESULT CSoundCue::Initialize(CSoundBank *pSoundBank, DWORD dwCueIndex, CSoundSource *pSoundSource)
{
    HRESULT hr = S_OK;
    CWaveBank *pWaveBank;
    ENTER_EXTERNAL_METHOD();
    DPF_ENTER();

    ASSERT(pSoundBank);

    //
    // we dont dont ADDREF he soundbank since its valid for it to dissappear while Cues are active
    // it will delete all cues in thread safe manner when the soundbank is deleted
    //

    m_pSoundBank = pSoundBank;

    m_dwCueIndex = dwCueIndex;

    PXACT_SOUNDBANK_CUE_ENTRY pCueTable = pSoundBank->GetCueTable();
    PXACT_SOUNDBANK_SOUND_ENTRY pSoundTable = pSoundBank->GetSoundTable();

    m_pSoundEntry = &pSoundTable[pCueTable[dwCueIndex].dwSoundIndex];

    //
    // allocate a context entry for each wavebank we belong to
    // this entry alows each wavebank to track us independently of other wavebanks
    //

    hr = HRFROMP( m_paWaveBankEntries = NEW_A(WAVEBANK_CUE_CONTEXT,m_pSoundEntry->wWaveBankCount));

    if (SUCCEEDED(hr)) {

        //
        // we need to find the wavebank object for each wavebank referenced by this sound
        // this will allow track events to quickly get the wavebank 
        // they require
        //
        
        PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY pWaveBankEntry = GetWaveBankTable();
        for (DWORD i=0;i<m_pSoundEntry->wWaveBankCount;i++) {
            
            if (!pWaveBankEntry[i].dwDataOffset) {
                
                hr = g_pEngine->GetWaveBank(pWaveBankEntry[i].szFriendlyName,&pWaveBank);
                
                if (SUCCEEDED(hr)) {
                    
                    pWaveBankEntry[i].dwDataOffset = (DWORD) pWaveBank;
                    
                    //
                    // addref the wavebank so it does not magically go away from our
                    // under our nose
                    //
                    
                } else {
                    
                    DPF_ERROR("Wavebank (%s) not registered yet. Cue can not play",
                        pWaveBankEntry[i].szFriendlyName);
                    
                }
                
            }
            
            if (SUCCEEDED(hr)) {

                CWaveBank *pWaveBank = (CWaveBank *) pWaveBankEntry[i].dwDataOffset;
                pWaveBank->AddRef();

                m_paWaveBankEntries[i].pSoundCue = this;
                InitializeListHead(&m_paWaveBankEntries[i].ListEntry);

                pWaveBank->AddCueToList(&m_paWaveBankEntries[i]);

            }
            
        }
        
    }

    if (SUCCEEDED(hr)) {

        if (pSoundSource) {
            
            m_pControlSoundSource = pSoundSource->IsPositional() ? pSoundSource : NULL;            
            
            //
            // addref SoundSource. This should bring the current refcount to at least 3
            // one ref on creation from the engine, one when calling CreateSoundSource or 
            // CreateSoundSourceInternal and one from associating it with a Cue
            //
            
            pSoundSource->AddRef();            
            
        }
        
    }
        
    if (SUCCEEDED(hr)) {

        //
        // create an array of track contexts
        //
        
        hr = HRFROMP(m_paTracks = NEW_A(TRACK_CONTEXT,m_pSoundEntry->wTrackCount));
        
    }

    if (SUCCEEDED(hr)) {

        for (DWORD i=0;i<m_pSoundEntry->wTrackCount;i++)
        {
            InitializeListHead(&m_paTracks[i].lstEvents);
            m_paTracks[i].pSoundCue = this;
            m_paTracks[i].wSamplesPerSec = 48000;
            m_paTracks[i].pContentEntry = (PXACT_SOUNDBANK_TRACK_ENTRY)
                ((PUCHAR)m_pSoundBank->GetBaseDataOffset() + m_pSoundEntry->dwTrackTableOffset +
                sizeof(XACT_SOUNDBANK_TRACK_ENTRY)*i);

            if (i == 0 && pSoundSource && !pSoundSource->IsPositional()){

                //
                // a non 3d sound source is just used as track source, not submix
                // destination
                //

                m_paTracks[i].pSoundSource = pSoundSource;
            }

        }

    }

    if (SUCCEEDED(hr)) {

        m_dwState = CUE_STATE_INITIALIZED;

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::Play"

HRESULT CSoundCue::Play(DWORD dwFlags)
{
    HRESULT hr = S_OK;
    ENTER_EXTERNAL_METHOD();
    DWORD dwOldState;

    DPF_ENTER();

    CheckStateTransition(CUE_STATE_PLAYING,&dwOldState);    
    ASSERT(dwOldState != CUE_STATE_PLAYING);
    
    //
    // if we are autorelease add flags to state
    //

    m_dwFlags = dwFlags;

    //
    // First time play
    // add ourselves to the core sequencer so our track events can start
    // getting processed
    //
    
    hr = g_pEngine->AddCueToSequencerList(this);

    //
    // start scheduling track events right away
    //

    if (SUCCEEDED(hr)) {

        hr = ScheduleTrackEvents(XACT_ENGINE_SCHEDULE_QUANTUM);

    }

    DPF_LEAVE_HRESULT(hr);
    return hr;

}


#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::Stop"

HRESULT CSoundCue::Stop(DWORD dwFlags)
{
    HRESULT hr = S_OK;
    ENTER_EXTERNAL_METHOD();
    PLIST_ENTRY pEntry;
    CSoundCue *pCue;

    DPF_ENTER();

    CheckStateTransition(CUE_STATE_STOPPING,NULL);    

    //
    // tell the hw voices associated with this cue to stop now
    // this way by the time dowork is called more time has elapsed
    //

    for (DWORD i=0;i<m_pSoundEntry->wTrackCount;i++) {
                        
        if (m_paTracks[i].pSoundSource) {            
            m_paTracks[i].pSoundSource->Stop();                        
        }
        
    }

    if (dwFlags & XACT_FLAG_SOUNDCUE_AUTORELEASE) {

        //
        // the caller indicated that it wants stop to release the cue when its done...
        //

        m_dwFlags |= XACT_FLAG_SOUNDCUE_AUTORELEASE;

    }

    if (dwFlags & XACT_FLAG_SOUNDCUE_SYNCHRONOUS) {

        //
        // do work will stop and release this cue
        //

        DoWork();
    }

    DPF_LEAVE_HRESULT(hr);
    return hr;

}

//
// private methods
//


//
// utility functions
//
#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::GetWaveBank()"

//
// this routine is called by the sequencer so track events
// can get scheduled for execution
//

VOID CSoundCue::GetWaveBank(DWORD dwWaveBankIndex, CWaveBank **ppWaveBank)
{
    DPF_ENTER();

    PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY pWaveBankEntry = GetWaveBankTable();
    pWaveBankEntry += dwWaveBankIndex;
    
    CWaveBank *pWaveBank = (CWaveBank*) pWaveBankEntry->dwDataOffset;
    *ppWaveBank = pWaveBank;
    
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::ScheduleEventsFromContent"

//
// this routine is called by the sequencer so track events from the content
// can get scheduled for execution
//

HRESULT CSoundCue::ScheduleTrackEvents(DWORD dwQuantum)
{
    HRESULT hr = S_OK;
    PTRACK_CONTEXT pTrack;
    PTRACK_EVENT_CONTEXT pEventContext;
    PXACT_TRACK_EVENT pEvent;
    
    REFERENCE_TIME rtTimeThreshold;
    
    ENTER_EXTERNAL_METHOD();
    
    DPF_ENTER();
    
    //
    // run through content for all tracks and submit events up to the time
    // of the current time + dwQuantum
    //
    
    for (DWORD i=0;i<m_pSoundEntry->wTrackCount;i++) {
        
        pTrack = &m_paTracks[i];
        ASSERT(pTrack->pContentEntry->wEventEntryCount >= pTrack->wNextEventIndex);
        
        KeQuerySystemTime((PLARGE_INTEGER)&rtTimeThreshold);
        
        rtTimeThreshold += dwQuantum;            
        
        //
        // this track might have already reached the end of its event table
        //
        
        if (pTrack->wNextEventIndex >= pTrack->pContentEntry->wEventEntryCount)
        {
            continue;
        }
        
        do {
            //
            // retrieve the next event we need to queue up
            // and create an event context for it
            //
            
            hr = HRFROMP(pEventContext = NEW(TRACK_EVENT_CONTEXT));
            
            if (SUCCEEDED(hr)) {
                
                InitializeListHead(&pEventContext->m_ListEntry);
                pEventContext->m_pTrack = pTrack;

                //
                // fine the event data in the content. We create the absolute data offset into the
                // soundbank by 
                // 1) getting the base soundbank data pointer +
                // 2) base data pointer for all events of this tracks
                // 3) data offset of this event form the start of the event data for the track
                //

                pEventContext->m_pEventHeader = (PXACT_TRACK_EVENT_HEADER) 
                    (m_pSoundBank->GetBaseDataOffset() + pTrack->pContentEntry->dwEventDataOffset +
                     pTrack->wNextEventDataOffset);
                
                pEvent = (PXACT_TRACK_EVENT) pEventContext->m_pEventHeader;
                
            }

            //
            // increment event pointers into the content
            //
            
            if (SUCCEEDED(hr)) {
                
                pTrack->wNextEventIndex++;
                pTrack->wNextEventDataOffset += sizeof(XACT_TRACK_EVENT_HEADER) + pEvent->Header.wSize;
                
            }

            //
            // process and submit event to sequencer
            //

            if (SUCCEEDED(hr)) {
                hr = SubmitEvent(pEventContext);
            }

            if (pTrack->wNextEventIndex >= pTrack->pContentEntry->wEventEntryCount)
            {
                break;
            }

            
        } while ((pEventContext->m_rtTimeStamp < rtTimeThreshold) && SUCCEEDED(hr));
        
    }
    
    DPF_LEAVE_HRESULT(hr);
    return hr;
    
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::ScheduleRuntimeEvent"

HRESULT CSoundCue::ScheduleRuntimeEvent(PXACT_TRACK_EVENT pEventDesc, DWORD dwTrackIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStart, dwEnd;
    PTRACK_EVENT_CONTEXT pEventContext = NULL;
    PXACT_TRACK_EVENT pEvent = NULL;

    ENTER_EXTERNAL_METHOD();
    ASSERT_IN_PASSIVE;

    if (dwTrackIndex == XACT_TRACK_INDEX_UNUSED) {

        //
        // this events needs to be scheduled for all tracks
        //

        dwStart = 0; dwEnd = m_pSoundEntry->wTrackCount;

    } else {

        ASSERT(dwTrackIndex < m_pSoundEntry->wTrackCount);
        dwStart = dwTrackIndex;
        dwEnd = dwTrackIndex+1;

    }
    

    for (DWORD i=dwStart;i<dwEnd;i++) {
        
        //
        // create an event context for this event
        //
        
        hr = HRFROMP(pEventContext = NEW(TRACK_EVENT_CONTEXT));
        
        if (SUCCEEDED(hr)) {            
            hr = HRFROMP(pEvent = NEW(XACT_TRACK_EVENT));            
        } else {
            pEvent = NULL;
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
            pEventContext->m_pTrack = &m_paTracks[i];
            InitializeListHead(&pEventContext->m_ListEntry);

            //
            // the sample time has to be adjusted based on the real playback
            // frequency of the track. When the event was submitted 48k was assumed
            //

            FLOAT fTemp = (FLOAT) pEventContext->m_pEventHeader->lSampleTime;
            fTemp = fTemp * (48000/m_paTracks[i].wSamplesPerSec);
            pEventContext->m_pEventHeader->lSampleTime = (LONG) fTemp;

            //
            // flag this event as being external
            //

            pEvent->Header.dwFlags |= XACT_FLAG_EVENT_RUNTIME;

            //
            // submit event to sequencer
            //

            hr = SubmitEvent(pEventContext);
            
        } else {            

            if (pEvent)
                DELETE(pEvent);
            
            if (pEventContext)
                DELETE(pEventContext);

            break;
        }

    }

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::SubmitEvent"

HRESULT CSoundCue::SubmitEvent(PTRACK_EVENT_CONTEXT pEventContext)
{

    HRESULT hr = S_OK;
    PXACT_TRACK_EVENT pEvent = (PXACT_TRACK_EVENT) pEventContext->m_pEventHeader;
    PTRACK_CONTEXT pTrack = pEventContext->m_pTrack;

    LPDIRECTSOUNDBUFFER pDSBuffer;
    LPDIRECTSOUNDSTREAM pDSStream;
    BOOL bUpdateTrackTime = FALSE;

    ASSERT(pEvent);
    
    //
    // some events need special processing before they are added to the schedule
    //    

    switch (pEventContext->m_pEventHeader->wType) {
    case eXACTEvent_PlayWithPitchAndVolumeVariation:
    case eXACTEvent_Play:
        
        if (pEvent->Header.dwFlags & XACT_FLAG_EVENT_USES_FXIN) {                    
            //
            // TODO set the buffer to read from an FXIN
            //                                                            
        } else {
            
            WAVEBANKUNIWAVEFORMAT waveFormat;
            CWaveBank *pWaveBank;
            LPWAVEBANKENTRY pWaveBankEntry;
            
            GetWaveBank(pEvent->EventData.Play.PlayDesc.WaveSource.wBankIndex,&pWaveBank);
            
            //
            // get the actual wavebank entry describing the wave we want to play
            //
            
            pWaveBankEntry = pWaveBank->GetWaveBankEntry(                
                pEvent->EventData.Play.PlayDesc.WaveSource.wWaveIndex);
            
            //
            // if this track has no source voice allocated it for it, get one now
            //
            
            if (pTrack->pSoundSource == NULL) {
                
                hr = g_pEngine->CreateSoundSourceInternal(
                    XACT_FLAG_SOUNDSOURCE_2D,
                    pWaveBank,
                    &pTrack->pSoundSource);
                
                if (FAILED(hr)) {
                    
                    DPF_ERROR("Could not allocate voice for Play event");
                    return hr;
                }
                
            }
            
            pDSBuffer = pTrack->pSoundSource->GetDSoundBuffer();
            pDSStream = pTrack->pSoundSource->GetDSoundStream();
            
            //
            // set the proper format for the hw buffer/stream
            //
            
            if (pWaveBankEntry->Format.wFormatTag == WAVEBANKMINIFORMAT_TAG_PCM) {
                
                XAudioCreatePcmFormat(pWaveBankEntry->Format.nChannels,
                    pWaveBankEntry->Format.nSamplesPerSec,
                    (pWaveBankEntry->Format.wBitsPerSample == WAVEBANKMINIFORMAT_BITDEPTH_8 ) ? 8 : 16, 
                    &waveFormat.WaveFormatEx);
                
            } else {
                
                XAudioCreateAdpcmFormat(pWaveBankEntry->Format.nChannels,
                    pWaveBankEntry->Format.nSamplesPerSec,
                    &waveFormat.AdpcmWaveFormat);
                
            }
            
            //
            // keep track of what source freq the wave is so the 
            // sample time stamps in each event can be properly converted
            //
            
            pTrack->wSamplesPerSec = (WORD)waveFormat.WaveFormatEx.nSamplesPerSec;
            
            //
            // BUGBUG at this point we should morph a buffer into a stream, 
            // if the wavebank is streamed
            //
            
            if (pDSBuffer) {                    
                
                hr = pDSBuffer->SetFormat(&waveFormat.WaveFormatEx);
                
                //
                // the buffer size to span entire wavebank. a voice has already been associated with this wavebank
                // making this call very fast
                //
                
                if (SUCCEEDED(hr)){
                    
                    hr = pWaveBank->SetBufferData(pDSBuffer);
                    
                }
                
                if (SUCCEEDED(hr)){
                    
                    //
                    // set the correct play region for this wave and wavebank
                    //
                    
                    hr = pDSBuffer->SetPlayRegion(
                        pWaveBankEntry->PlayRegion.dwStart,
                        pWaveBankEntry->PlayRegion.dwLength);
                    
                }
                
                if (SUCCEEDED(hr)) {
                    
                    //
                    // set loop region
                    //
                    
                    hr = pDSBuffer->SetLoopRegion(
                        pWaveBankEntry->LoopRegion.dwStart,
                        pWaveBankEntry->LoopRegion.dwLength);
                    
                }
                
                if (SUCCEEDED(hr)) {
                    
                    //
                    // associate this source voice with a destination
                    //
                    
                    if (IsPositional() && m_pControlSoundSource){
                        hr = pDSBuffer->SetOutputBuffer(m_pControlSoundSource->GetDSoundBuffer());
                    }
                    
                }
                
            } else { // if DSBUffer
                
                //
                // Stream
                // Get some packets with disk data and submit them to the sound source
                // for streaming
                //
                
            }
            
        } // if FXSOURCE
         
        if (pTrack->wNextEventIndex == 0) {        

            //
            // this is the firt play event for this track.
            // record the start time
            //            

            bUpdateTrackTime = TRUE;
        }
        
        break;
         
    case eXACTEvent_Stop:
        
        //
        // stop does not need any additional processing
        //
        
        break;
      
    case eXACTEvent_LoopStart:
        
        //
        // loop is starting set the loop count,remember index to jump to
        // on loop end. Loops work like this:
        // When the loopStart event is encountered we setup the number of loop
        // we need to execute and the event index into the context we need to loop
        // BACK after the loop ends. that event is the LoopStart
        // event. When we encounter a loopEnd event, we jump back to the cached index.
        // and we actually skip the LoopStart event on any loop after the first one
        //
        
        ASSERT(pTrack->wLoopCount == 0);
        pTrack->wLoopCount = pEvent->EventData.LoopStart.wLoopCount;
        pTrack->wLoopStartEventIndex = pTrack->wNextEventIndex;
        pTrack->wLoopStartEventDataOffset = pTrack->wNextEventDataOffset;
            
        break;
        
    case eXACTEvent_LoopEnd:

        //
        // this event will never make it to the sequencer during looping
        // since it resets the current event index into the content back to the 
        // loop start ...
        //

        if (pTrack->wLoopCount) {
            
            //
            // make the track go back to the loop start index
            //
            
            pTrack->wNextEventIndex = pTrack->wLoopStartEventIndex;
            pTrack->wNextEventDataOffset = pTrack->wLoopStartEventDataOffset;
            
            //
            // decrement loop count
            //
            
            pTrack->wLoopCount--;

            //
            // each time we loop, we update the start time of our track
            // so the events are scheduled relative to the start of the loop
            //

            bUpdateTrackTime = TRUE;
            
        }

        break;

    default:
        
        break;
        
        
    } //switch

    if (bUpdateTrackTime) {

        g_pEngine->GetTime(&pTrack->rtStartTime);                    

    }

    //
    // create an absolute system reference time from event time stamp(in samples)
    //

    if (SUCCEEDED(hr)) {

        g_pEngine->CreateEventTimeStamp(pTrack,
            pEventContext);
        
        g_pEngine->PrintTimeStamps(DPF_FNAME,
            pEventContext,
            pEventContext->m_rtTimeStamp,
            0);
        
        //
        // add to list of events for this track
        //
        
        {
            AutoIrql();
            InsertTailList(&pTrack->lstEvents,&pEventContext->m_ListEntry);
        }
        
        //
        // enqueue this event
        //
        
        hr = g_pEngine->Enqueue(pEventContext);
        
    }

    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::IsPositional"

__inline BOOL CSoundCue::IsPositional()
{
    DPF_ENTER();
    return (m_pSoundEntry->dwFlags & XACT_FLAG_SOUND_3D);
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::IsPlaying"

VOID CSoundCue::DoWork()
{
    DPF_ENTER();

    ENTER_EXTERNAL_METHOD();

    PTRACK_CONTEXT pTrack;
    DWORD dwStoppedTracks = 0;
    
    if ((m_dwState == CUE_STATE_STOPPED) || (m_dwState == CUE_STATE_INITIALIZED)) {
        return;
    }

    //
    // figure out if we are still playing
    //

    for (DWORD i=0;i<m_pSoundEntry->wTrackCount;i++) {
        
        pTrack = &m_paTracks[i];
        
        if (m_dwState == CUE_STATE_STOPPING) {
            
            //
            // stopped was called on the api
            // remove any events still scheduled
            // this essentialy forces a stop no matter what the tracks are doing
            //
                        
            if (pTrack->wNextEventIndex) {
                g_pEngine->FreeEventsAtOrAfter(pTrack,0);
            }
            
            //
            // set index to zero to signal the track is back to initial state
            //
            
            pTrack->wNextEventIndex = 0;
            
            dwStoppedTracks++;
            continue;
            
        }

        //
        // A track is considered playing if any of the following is true:
        // 1) it has a voice associted with it and the voice is playing
        // 2) its current event index < then the eventCount for the track
        //
                
        {
            AutoIrql();
            if (!IsListEmpty(&pTrack->lstEvents)) {
                
                //
                // still playing, there are queued events waiting to be dispatched, 
                //
                
                continue;
            }
        }
        
        if (pTrack->wNextEventIndex && 
            (pTrack->wNextEventIndex < pTrack->pContentEntry->wEventEntryCount)) {
            
            //
            // still playing, track was started and are events left to be scheduled
            //
            
            continue;
        }
        
        if (pTrack->pSoundSource) {
            
            if (pTrack->pSoundSource->IsPlaying()) {
                continue;
            }
        }
        
        //
        // if we made it here it means the track is stopped
        //
        
        dwStoppedTracks++;
        
    }

    if (dwStoppedTracks == m_pSoundEntry->wTrackCount) {

        XACT_TRACK_EVENT Event;

        //
        // all tracks are stopped, mark this voice as stopped
        //

        CheckStateTransition(CUE_STATE_STOPPED,NULL);
        
        memset(&Event,0,sizeof(Event));
        Event.Header.wType = eXACTEvent_Stop;
        
        //
        // call the cue in case we need to generate a notification
        // we pass in a stop event we just created here to signal the entire
        // cue has stopped playing
        //
        
        ProcessRuntimeEvent(&Event);

        //
        // cue is stopped, remove from active linked list
        //
        
        g_pEngine->RemoveCueFromSequencerList(this);
        
    } else {

        //
        // we are still playing, schedule some more events
        //

        ScheduleTrackEvents(XACT_ENGINE_SCHEDULE_QUANTUM);

    }

    return ;
}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::CheckStateTransition"

VOID CSoundCue::CheckStateTransition(DWORD dwNewState, PDWORD pdwOldState)
{
    AutoIrql();
    HRESULT hr = S_OK;
    DPF_ENTER();

    //
    // validate state transition depending on new state
    //

    switch (dwNewState) {
    case CUE_STATE_PLAYING:
        
        switch (m_dwState) {
        case CUE_STATE_STOPPED:
        case CUE_STATE_INITIALIZED:
            
            //
            // valid transition
            //
            
            DPF_INFO("Starting to play cue 0x%x",this);
            break;
        case CUE_STATE_CREATED:
            
            DPF_ERROR("Cue 0x%x not initialized yet!");
            break;
            
        case CUE_STATE_PLAYING:
                       
            DPF_INFO("Cue 0x%x Already Playing",this);           
            break;
            
        }
        
        break;
        
    case CUE_STATE_STOPPING:
            
        switch (m_dwState) {
        case CUE_STATE_PLAYING:
            
            //
            // valid transition
            //
            DPF_INFO("Stopping cue 0x%x",this);
            break;
            
        case CUE_STATE_INITIALIZED:
            
            DPF_WARNING("Cue 0x%x not playing yet",this);
            break;
            
        case CUE_STATE_CREATED:
            
            DPF_ERROR("Cue 0x%x not initialized yet!");
            break;
            
        case CUE_STATE_STOPPED:
            
            DPF_WARNING("Cue 0x%x Already stopped",this);
            break;
            
        }
        
        break;
        
    case CUE_STATE_STOPPED:
        
        switch (m_dwState) {
        case CUE_STATE_STOPPING:
            
            //
            // valid transition
            //
            
            DPF_INFO("Cue 0x%x is now stopped (was in Stopping state)",this);
            break;
            
        case CUE_STATE_INITIALIZED:
            
            DPF_WARNING("Cue 0x%x not playing yet",this);
            break;
            
        case CUE_STATE_CREATED:
            
            DPF_ERROR("Cue 0x%x not initialized yet!");
            break;
            
        case CUE_STATE_PLAYING:
            
            DPF_INFO("Cue 0x%x is now stopped (was in Playing state)",this);
            break;
            
        }
        
        break;        
        
    }

    if (pdwOldState) {
        *pdwOldState = m_dwState;
    }

    m_dwState = dwNewState;

}

#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::ProcessRuntimeEvent"

VOID CSoundCue::ProcessRuntimeEvent(XACT_TRACK_EVENT *pEventDesc)
{
    AutoIrql();

    BOOL bHandledLocally = TRUE;
    
    PNOTIFICATION_CONTEXT pContext;
    DWORD dwNotificationEvent = -1;

    //
    // generate notifications based on runtime events, if the user
    // has registered appropriately
    //

    switch (pEventDesc->Header.wType) {

    case eXACTEvent_PlayWithPitchAndVolumeVariation:
    case eXACTEvent_Play:
        
        dwNotificationEvent = eXACTNotification_Start;
        pContext = &m_aNotificationContexts[dwNotificationEvent];

        //
        // only the first play event of ANY track matters...
        // dont handle the notification for any subsequent plays
        //

        if (pContext->bRegistered && (pContext->PendingNotification.rtTimeStamp != 0))
        {

            bHandledLocally = FALSE;

        } else {

            //
            // call the soundbank in case it has some notifications registered for this
            //
            
            m_pSoundBank->ProcessRuntimeEvent(pEventDesc);
        }

        break;

    case eXACTEvent_Stop:

        //
        // Stop is a special case. This is not a stop that got dispatched from the
        // sequencer. Its instead a made-up stop called from when the Cue realizes all its
        // tracks have stopped playing
        //

        dwNotificationEvent = eXACTNotification_Stop;
        pContext = &m_aNotificationContexts[dwNotificationEvent];

        //
        // call the soundbank in case it has some notifications registered for this
        //

        m_pSoundBank->ProcessRuntimeEvent(pEventDesc);
        break;

    case eXACTEvent_Marker:
        dwNotificationEvent = eXACTNotification_Marker;
        pContext = &m_aNotificationContexts[dwNotificationEvent];

        //
        // copy data to cached context
        //

        memcpy(pContext->PendingNotification.Data.Marker.bData,
            pEventDesc->EventData.Marker.bData, XACT_SIZEOF_MARKER_DATA);

        break;
    }

    if (bHandledLocally) {

        g_pEngine->AddNotificationToPendingList(pContext);

    }            

}


#undef DPF_FNAME
#define DPF_FNAME "CSoundCue::GetNotificationContext"

PNOTIFICATION_CONTEXT CSoundCue::GetNotificationContext(DWORD dwType)
{

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();    
    return &m_aNotificationContexts[dwType];

}
