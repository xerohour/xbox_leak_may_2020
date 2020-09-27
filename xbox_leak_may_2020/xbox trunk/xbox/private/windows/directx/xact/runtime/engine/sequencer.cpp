/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       sequencer.cpp
 *  Content:    XACT runtime Engine sequencer. 
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/22/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"

using namespace XACT;


//
// event names
//

#if DBG

//
// NOTE: the order of the names below MUST match the
// XACT_TRACK_EVENT_TYPES enum in xactp.h
//

EXTERN_C 
char *g_aszXACTEventNames[eXACTEvent_Max] = {

        "PLAY",
        "PLAYWITHPITCHANDAVOLUME",
        "STOP",
        "PITCHANDAVOLUMEVARIATION",
        "SETFREQUENCY",
        "SETVOLUME",
        "SETHEADROOM",
        "SETLFO",
        "SETEG",
        "SETFILTER",
        "MARKER",
        "LOOPSTART",
        "LOOPEND",
        "SETMIXBINVOLUMES",
        "SETEFFECTDATA"        

};

#endif //DBG



// ------------ Event methods ------------------

BOOL TRACK_EVENT_CONTEXT::IsLessThanOrEqual(const REFERENCE_TIME* time) const
{
    return m_rtTimeStamp <= *time;
}

__inline BOOL TRACK_EVENT_CONTEXT::IsLessThan(const TRACK_EVENT_CONTEXT* other) const
{
    return Compare(other) == -1;
}

__inline BOOL TRACK_EVENT_CONTEXT::IsGreaterThan(const TRACK_EVENT_CONTEXT* other) const
{
    return Compare(other) == 1;
}

LONG TRACK_EVENT_CONTEXT::Compare(const TRACK_EVENT_CONTEXT* other) const
{
    // Check time
    if(m_rtTimeStamp < other->m_rtTimeStamp){
        return -1;
    }
    if(m_rtTimeStamp > other->m_rtTimeStamp){
        return 1;
    }
    return 0;
}

/*
 * A priority queue implementation. This queue has the following
 * properties:
 *
 * Inserting is O(log(n))
 * Removing is O(log(n))
 * Poping is O(log(n))
 *
 * Overhead of 8 bytes per element. 4 for the Element pointer, 4 for
 * the queue index inside the element. The queue index is used to
 * enable Remove to operate in O(log(n)) instead of O(n).
 *
 */

#ifdef DO_VERIFY_PRIORITYQUEUE
#define VERIFY_PRIORITYQUEUE Verify()
#else
#define VERIFY_PRIORITYQUEUE (void) 0
#endif

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::CPriorityQueue"

CPriorityQueue::CPriorityQueue()
{
    DPF_ENTER();
    m_paEvents = NULL;
    m_dwCapacity = 0;
    m_dwSize = 0;

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::~CPriorityQueue"

CPriorityQueue::~CPriorityQueue()
{
    DPF_ENTER();
    if (m_paEvents) 
    {
        DELETE_A(m_paEvents);
    }

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Initialize"

HRESULT CPriorityQueue::Initialize(DWORD dwMaxSize)
{
    HRESULT hr = S_OK;
    DPF_ENTER();
    ASSERT(m_paEvents == NULL);

    hr = HRFROMP(m_paEvents = NEW_A(PTRACK_EVENT_CONTEXT,dwMaxSize));

    if (SUCCEEDED(hr)) {

        m_dwCapacity = dwMaxSize;
        VERIFY_PRIORITYQUEUE;

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Remove"

void CPriorityQueue::Remove(PTRACK_EVENT_CONTEXT pEvent)
{
    DPF_ENTER();
    VERIFY_PRIORITYQUEUE;

    //
    // Bubble the event to the top.
    //

    ASSERT(pEvent->m_dwQueueIndex);

    DWORD test_node = pEvent->m_dwQueueIndex;
    while(test_node > 1){
        DWORD parent = test_node / 2;
        Swap(test_node, parent);
        test_node = parent;
    }

    //
    // Remove from queue - this balances the queue again
    //

    Pop2();

    VERIFY_PRIORITYQUEUE;

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::GetNextEventTime"

BOOL CPriorityQueue::GetNextEventTime(REFERENCE_TIME* pTime)
{

    DPF_ENTER();
    VERIFY_PRIORITYQUEUE;
    if(!m_dwSize){
        return false;
    }

    *pTime = At(1)->m_rtTimeStamp;
    return true;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::PopIfLessThanOrEqual"

PTRACK_EVENT_CONTEXT CPriorityQueue::PopIfLessThanOrEqual(const REFERENCE_TIME* rtTime)
{

    DPF_ENTER();
    VERIFY_PRIORITYQUEUE;
    PTRACK_EVENT_CONTEXT pResult = 0;

    if(m_dwSize && At(1)->IsLessThanOrEqual(rtTime)){
        pResult = Pop2();
    }

    return pResult;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Pop"

PTRACK_EVENT_CONTEXT CPriorityQueue::Pop()
{

    DPF_ENTER();
    VERIFY_PRIORITYQUEUE;
    PTRACK_EVENT_CONTEXT pResult = Pop2();
    return pResult;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Pop2"

PTRACK_EVENT_CONTEXT CPriorityQueue::Pop2()
{

    DPF_ENTER();

    //
    // Don't verify the queue, because it won't be valid if we're called from Remove
    // (Because Remove has bubbled the item it wants to remove to the top of the heap.)

    if(m_dwSize <= 0){
        return NULL;
    }

    // Return the top of the queue, and
    // adjust the heap 

    PTRACK_EVENT_CONTEXT pTop = At(1);
    Move(1, m_dwSize); // last element can be moved because it has no children, doesn't leave hole.

    DWORD test_node = 1;
    for(;;){
        DWORD child;
        if ( ( test_node * 2 ) >= m_dwSize )
            break;
        if ( ( test_node * 2 + 1) >= m_dwSize )
            child = test_node * 2;
        else if ( At( test_node * 2)->IsLessThan(At(test_node * 2 + 1)) )
            child = test_node * 2;
        else
            child = test_node * 2 + 1;
        if ( At(test_node)->IsGreaterThan(At(child)) ) {
            Swap(test_node, child);
            test_node = child;
        } else
            break;
    }

    //
    // no longer in queue
    //

    --m_dwSize;
    pTop->m_dwQueueIndex = 0; 

    VERIFY_PRIORITYQUEUE;
    return pTop;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Push"

HRESULT CPriorityQueue::Push(PTRACK_EVENT_CONTEXT pEvent)
{
    HRESULT hr = S_OK;

    DPF_ENTER();
    VERIFY_PRIORITYQUEUE;
    ASSERT(m_paEvents);
    ASSERT(m_dwCapacity);
    ASSERT(m_dwCapacity >= m_dwSize);

    if (m_dwCapacity <= m_dwSize){
        return E_OUTOFMEMORY;
    }

    //
    // At and AtPut are one-based, so pre-increment
    //

    AtPut(++m_dwSize, pEvent); 

    // Bubble into proper position
    DWORD test_node = m_dwSize;
    while ( test_node > 1 ) {
        DWORD parent_node = test_node/2;
        if ( At(parent_node)->IsGreaterThan(At(test_node)) ){
            Swap(parent_node, test_node);
            test_node = parent_node;
        } else
            break;
    }

    VERIFY_PRIORITYQUEUE;
    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::AdjustEventTimes"

VOID CPriorityQueue::AdjustEventTimes(REFERENCE_TIME rtDelta)
{
    DPF_ENTER();
    for(DWORD i = 1; i <= m_dwSize; i++){
        PTRACK_EVENT_CONTEXT pEvent = At(i);
        pEvent->m_rtTimeStamp += rtDelta;
    }

    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::AtPut"

__inline VOID CPriorityQueue::AtPut(DWORD dwIndex, PTRACK_EVENT_CONTEXT pEvent)
{
    DPF_ENTER();
    ASSERT(1 <= dwIndex);
    ASSERT(dwIndex <= m_dwSize);
    m_paEvents[dwIndex-1] = pEvent;
    pEvent->m_dwQueueIndex = dwIndex;
    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Move"

VOID CPriorityQueue::Move(DWORD dwDest, DWORD dwSource)
{
    DPF_ENTER();
    ASSERT(1 <= dwDest);
    ASSERT(dwDest <= m_dwSize);
    ASSERT(1 <= dwSource);
    ASSERT(dwSource <= m_dwSize);
    if(dwSource != dwDest){
        m_paEvents[dwDest-1] = m_paEvents[dwSource-1];
        m_paEvents[dwDest-1]->m_dwQueueIndex = dwDest;
        m_paEvents[dwSource-1] = 0;
    }
    DPF_LEAVE_VOID();
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::At"

__inline PTRACK_EVENT_CONTEXT CPriorityQueue::At(DWORD dwIndex)
{
    DPF_ENTER();
    ASSERT(1 <= dwIndex);
    ASSERT(dwIndex <= m_dwSize);
    PTRACK_EVENT_CONTEXT pResult = m_paEvents[dwIndex-1];
    ASSERT(pResult);
    return pResult;
}

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Swap"

VOID CPriorityQueue::Swap(DWORD dwIndex1, DWORD dwIndex2)
{
    DPF_ENTER();

    PTRACK_EVENT_CONTEXT pTemp = At(dwIndex1);
    AtPut(dwIndex1, At(dwIndex2));
    AtPut(dwIndex2, pTemp);
}

#ifdef DBG

#undef DPF_FNAME
#define DPF_FNAME "CPriorityQueue::Verify"

VOID CPriorityQueue::Verify()
{

    if(Initialized()){
        ASSERT(m_paEvents);
        ASSERT(m_dwCapacity);
        ASSERT(m_dwCapacity >= m_dwSize);
        DWORD dwMaxSize = m_dwSize;
        if(dwMaxSize > m_dwCapacity){
            dwMaxSize = m_dwCapacity;
        }

#ifndef DO_FULL_VERIFY_PRIORITYQUEUE
        if(dwMaxSize > 20){
            dwMaxSize = 20;
        }
#endif
        for(DWORD i = 1; i <= dwMaxSize; i++){
            PTRACK_EVENT_CONTEXT pEvent = At(i);
            ASSERT(pEvent->m_dwQueueIndex == i);
            int parent = i / 2;
            if(parent>0){
                if( !m_paEvents[parent-1]->IsLessThan(m_paEvents[i-1]) ){
                    DPF_ERROR("Error: parent: %d is not less than child: %d\n", parent, i);
                }
            }
        }
    }
}

#endif // DBG

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// sequencer methods
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#undef DPF_FNAME
#define DPF_FNAME "CEngine::RemoveCueFromSequencerList"

HRESULT CEngine::RemoveCueFromSequencerList(CSoundCue *pCue)
{

    HRESULT hr = S_OK;
    DPF_ENTER();
    ASSERT(pCue);

    ENTER_EXTERNAL_METHOD();

    RemoveEntryList(&pCue->m_SeqListEntry);    
    pCue->Release();
    
    return hr;

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::AddCueToSequencerList"

HRESULT CEngine::AddCueToSequencerList(CSoundCue *pCue)
{
    HRESULT hr = S_OK;
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    InsertTailList(&m_lstActiveCues,&pCue->m_SeqListEntry);
    pCue->AddRef();


    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::InitializeSequencer"

HRESULT CEngine::InitializeSequencer(DWORD dwMaxConcurrentEvents)
{

    HRESULT hr = S_OK;
    AutoIrql();

    if(m_bAllowQueueing){

        //
        // Already initialized
        //

        return E_FAIL;
    }

    m_bAllowQueueing = TRUE;

    hr = HRFROMP(m_pQueue = NEW(CPriorityQueue));

    if(SUCCEEDED(hr))
    {
        hr = m_pQueue->Initialize(dwMaxConcurrentEvents);
    }

    SetTimeOffset();

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::GetTime"

VOID CEngine::GetTime(LPREFERENCE_TIME prtCurrent)
{
    DPF_ENTER();
    AutoIrql();

    DWORD dwPosition = *((PDWORD)0xfe80200c);

    //
    // Check if audio hw counter wrapped
    //

    LONGLONG delta;
    if(dwPosition < m_dwLastPosition){

        if(dwPosition < 100000 && m_dwLastPosition > (1 << 30) ){

            DPF_WARNING("Sample counter wrapped (as it should every 24 hours. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = ((((LONGLONG) 1) << 32) + dwPosition) - m_dwLastPosition;

        } else {

            DPF_WARNING("Sample counter decreased unexpectedly. Was: %u is: %u\n",
                m_dwLastPosition, dwPosition);
            delta = 0;
            m_dwLastPosition = dwPosition;

        }

    } else {

        delta = dwPosition - m_dwLastPosition;

    }

    m_dwLastPosition = dwPosition;
    m_llSampleTime += delta;

    LONGLONG llTemp = m_llSampleTime * 10000;
    llTemp /= 48000;
    llTemp *= 1000;
    *prtCurrent = llTemp;


    DPF_LEAVE_VOID();
    return;
}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::ConvertTimeValues"

VOID CEngine::CreateEventTimeStamp(PTRACK_CONTEXT pTrack, PTRACK_EVENT_CONTEXT pEventContext)
{

    ASSERT(pTrack->wSamplesPerSec);        

    //
    // the event time is calculated based on 
    // 1)the source playback frequency of the track
    // 2)the sample count offset into the track of the particular event
    // 3)the track starting time (when the first PlayEvent was processed)
    // 4) the offset between the audio hw clock and the system clock
    //

    
    LONGLONG llTemp = pEventContext->m_pEventHeader->lSampleTime * 10000;
    llTemp /= pTrack->wSamplesPerSec;
    llTemp *= 1000;
    pEventContext->m_rtTimeStamp = llTemp;

    //
    // add track starting time (this value came from the audio hw clock)
    //

    pEventContext->m_rtTimeStamp += pTrack->rtStartTime;

    //
    // add offset of audio clock from system clock
    //

    pEventContext->m_rtTimeStamp += m_rtTimeOffset;

}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::GetEvent"

HRESULT CEngine::GetEvent(TRACK_EVENT_CONTEXT** ppEvent)
{
    ASSERT(ppEvent);
    DPF_ENTER();

    HRESULT hr = HRFROMP(*ppEvent = NEW(TRACK_EVENT_CONTEXT));

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::FreeAllEvents"

VOID CEngine::FreeAllEvents()
{
    AutoIrql();
    DPF_ENTER();
    if(m_pQueue->Initialized()){
        PTRACK_EVENT_CONTEXT pEvent;
        while(pEvent = m_pQueue->Pop()){
            FreeEvent(pEvent);
        }
    }
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::FreeEvent"

VOID CEngine::FreeEvent(PTRACK_EVENT_CONTEXT pEvent)
{
    AutoIrql();
    ASSERT(pEvent);
    
    DPF_ENTER();

    if (pEvent->m_pTrack)
    {

        DPF_BLAB("Freeing Event (0x%x) type %d, track %x",
            pEvent,
            pEvent->m_pEventHeader->wType,
            pEvent->m_pTrack);
        RemoveEntryList(&pEvent->m_ListEntry);

    }

    if (pEvent->m_pEventHeader->dwFlags & XACT_FLAG_EVENT_RUNTIME){

        //
        // API supplied event points to allocated context
        //

        DELETE(pEvent->m_pEventHeader);
    }

    DELETE(pEvent);
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::FreeEventsAtOrAfter"

VOID CEngine::FreeEventsAtOrAfter(PTRACK_CONTEXT pTrack, REFERENCE_TIME timeStamp)
{
    AutoIrql();
    PTRACK_EVENT_CONTEXT pEvent;
    PLIST_ENTRY pEntry;
    REFERENCE_TIME rtCurrent;

    DPF_ENTER();

    //
    // since we are at raised irql we can traverse the event list associated with a voice
    // without asking the track to do it 
    //

    ASSERT(pTrack);

    pEntry = pTrack->lstEvents.Flink;

    GetTime(&rtCurrent);

    // Add timing offset. 
    timeStamp += rtCurrent;

    while (pEntry != &pTrack->lstEvents) {

        pEvent = CONTAINING_RECORD(pEntry, TRACK_EVENT_CONTEXT, m_ListEntry);

        //
        // grab the next pointer before removing this event from the list...
        //

        pEntry = pEntry->Flink;

        //
        // make sure the event was actually queued
        // it can exist in the track list without being in the priority queue
        // since this call could have pre-empted the enqueue
        //

        if((pEvent->m_rtTimeStamp >= timeStamp) && pEvent->m_dwQueueIndex){
            m_pQueue->Remove(pEvent);            
        }

        FreeEvent(pEvent);

    }

}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::SetTimeOffset"

VOID CEngine::SetTimeOffset()
{
    REFERENCE_TIME rtSampleTime,rtSystemTime;

    //
    // the audio hw clock and the system clock are two independent timers
    // both values returned below are in unsing of 100 nsecs.
    // Once in a while we adjust the offset between the clocks to create
    // a time offset used to convert event timestamps (that are in audio hw sampletime)
    // to absolute system times
    //

    AutoIrql();
    GetTime(&rtSampleTime);
    KeQuerySystemTime((PLARGE_INTEGER)&rtSystemTime);

    m_rtTimeOffset = rtSystemTime - rtSampleTime;

}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::Enqueue"

HRESULT CEngine::Enqueue(PTRACK_EVENT_CONTEXT pEvent)
{
    HRESULT hr = S_OK;
    DPF_ENTER();

    REFERENCE_TIME rtCurrentSystemTime;
    

    BOOL bExecuteNow = FALSE;
    BOOL bProcessQueueUpToEvent = FALSE;
        
    KeQuerySystemTime((PLARGE_INTEGER)&rtCurrentSystemTime);
    
    if(rtCurrentSystemTime >= pEvent->m_rtTimeStamp){
        
        bExecuteNow = TRUE;
        bProcessQueueUpToEvent = TRUE;
        
    }
    
    // If the event is scheduled for now or earlier, execute it right away.
    if (bExecuteNow){

        AutoIrql();

        if(bProcessQueueUpToEvent){
            DispatchEventsUntil(&pEvent->m_rtTimeStamp, &rtCurrentSystemTime);
        }

        hr = DispatchEvent(pEvent);

    } else {

        AutoIrql();

        if(!m_bAllowQueueing){
            return E_FAIL;
        }

        pEvent->m_dwQueueIndex = 0;         

        hr = m_pQueue->Push(pEvent);
                
        if(SUCCEEDED(hr)){
            SetTimer();
        }

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::Dispatch"

HRESULT CEngine::Dispatch()
{
    // Execute everything that's less than or equal to now.
    HRESULT hr = S_OK;
    DPF_ENTER();    

    REFERENCE_TIME now;
    KeQuerySystemTime((PLARGE_INTEGER) &now);

    m_bTimerSet = FALSE;

    if(SUCCEEDED(hr)){
        hr = DispatchEventsUntil(&now, &now);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::DispatchEventsUntil"

HRESULT CEngine::DispatchEventsUntil(const REFERENCE_TIME* pTime, const REFERENCE_TIME* pNow)
{
    HRESULT hr = S_OK;
    DPF_ENTER();

    BOOL bFirstEvent = TRUE;
    if(SUCCEEDED(hr)){

        PTRACK_EVENT_CONTEXT pEvent;

        while(pEvent = m_pQueue->PopIfLessThanOrEqual(pNow)){
            hr = DispatchEvent(pEvent);
        }

    }

    SetTimer();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


#undef DPF_FNAME
#define DPF_FNAME "CEngine::DispatchEvent"

HRESULT CEngine::DispatchEvent(PTRACK_EVENT_CONTEXT pEventContext)
{

    ASSERT_IN_DPC;
    HRESULT hr = S_OK;

    LPDIRECTSOUNDBUFFER pDSBuffer;
    LPDIRECTSOUNDSTREAM pDSStream;
    
    PXACT_TRACK_EVENT pEvent = (PXACT_TRACK_EVENT) pEventContext->m_pEventHeader;
    CSoundSource *pSoundSource = pEventContext->m_pTrack->pSoundSource;

    if (pSoundSource) {
        pDSBuffer = pSoundSource->GetDSoundBuffer();
        pDSStream = pSoundSource->GetDSoundStream();
    }

    DPF_ENTER();
    
    PrintTimeStamps("DispatchEvent",pEventContext, pEventContext->m_rtTimeStamp,0);

    switch (pEvent->Header.wType) {
    
    case eXACTEvent_PitchAndVolumeVariation:
        CreateEventVariation(pEventContext);
        break;

    case eXACTEvent_PlayWithPitchAndVolumeVariation:
        CreateEventVariation(pEventContext);

        //
        // fall in below so we can now play the variation
        //

    case eXACTEvent_Play:
        
        ASSERT(pSoundSource);
        hr = pSoundSource->Play();        
        pEventContext->m_pTrack->pSoundCue->ProcessRuntimeEvent(pEvent);
        break;

    case eXACTEvent_Stop:

        //
        // stop the voice playing the current wave/fxin
        //

        ASSERT(pSoundSource);
        hr = pSoundSource->Stop();
        break;

    case eXACTEvent_SetFrequency:

        if (pDSBuffer) {
            hr = pDSBuffer->SetFrequency((DWORD)pEvent->EventData.SetFrequency.wFrequency);
        } else {
            hr = pDSStream->SetFrequency((DWORD)pEvent->EventData.SetFrequency.wFrequency);
        }
        break;

    case eXACTEvent_SetVolume:

        if (pDSBuffer) {
            hr = pDSBuffer->SetVolume((LONG)pEvent->EventData.SetVolume.sVolume);
        } else {
            hr = pDSStream->SetVolume((LONG)pEvent->EventData.SetVolume.sVolume);
        }
        break;

    case eXACTEvent_SetHeadroom:

        if (pDSBuffer) {
            hr = pDSBuffer->SetHeadroom((DWORD)pEvent->EventData.SetHeadroom.wHeadroom);
        } else {
            hr = pDSStream->SetHeadroom((DWORD)pEvent->EventData.SetHeadroom.wHeadroom);
        }
        break;

    case eXACTEvent_SetLFO:

        if (pDSBuffer) {
            hr = pDSBuffer->SetLFO(&pEvent->EventData.SetLFO.Desc);
        } else {
            hr = pDSStream->SetLFO(&pEvent->EventData.SetLFO.Desc);
        }
        break;

    case eXACTEvent_SetEG:

        if (pDSBuffer) {
            hr = pDSBuffer->SetEG(&pEvent->EventData.SetEG.Desc);
        } else {
            hr = pDSStream->SetEG(&pEvent->EventData.SetEG.Desc);
        }
        break;

    case eXACTEvent_SetFilter:

        if (pDSBuffer) {
            hr = pDSBuffer->SetFilter(&pEvent->EventData.SetFilter.Desc);
        } else {
            hr = pDSStream->SetFilter(&pEvent->EventData.SetFilter.Desc);
        }
        break;

    case eXACTEvent_SetEffectData:

        hr = m_pDirectSound->SetEffectData(
            (DWORD)pEvent->EventData.SetEffectData.wEffectIndex,
            (DWORD)pEvent->EventData.SetEffectData.wOffset,
            &pEvent->EventData.SetEffectData.dwData,
            (DWORD)pEvent->EventData.SetEffectData.wDataSize,
            DSFX_IMMEDIATE
            );
        
        break;
    case eXACTEvent_Marker:

        //
        // a marker event is purely a notification generator
        //

        pEventContext->m_pTrack->pSoundCue->ProcessRuntimeEvent(pEvent);
        break;
        
    case eXACTEvent_SetMixBinVolumes:

        DSMIXBINS dsMixBins;
        dsMixBins.dwMixBinCount = pEvent->EventData.SetMixBinVolumes.dwCount;
        dsMixBins.lpMixBinVolumePairs = pEvent->EventData.SetMixBinVolumes.aVolumePairs;

        if (pDSBuffer) {
            hr = pDSBuffer->SetMixBinVolumes(&dsMixBins);
        } else {
            hr = pDSStream->SetMixBinVolumes(&dsMixBins);
        }

        break;

    case eXACTEvent_LoopStart:
        break;
        
    case eXACTEvent_LoopEnd:
        break;
                    
    default:
        DPF_WARNING("(0x%x) Event (%s) not recognised",
            this,
            pEvent,
            g_aszXACTEventNames[pEvent->Header.wType]);
        break;
    }

    //
    // the event has been dispatched. we can now remove it from the track event list
    // and release it
    //

    FreeEvent(pEventContext);

    DPF_LEAVE_HRESULT(hr);
    return hr;
}

HRESULT CEngine::CreateEventVariation(PTRACK_EVENT_CONTEXT pEventContext)
{

    
    HRESULT hr = S_OK;

    ASSERT_IN_DPC;
    KeSaveFloatingPointState(&m_fps);

    //
    // we will touch fp state, so save it here
    //


    FLOAT fTemp = 0.0f, fRange = 0.0f, fRandom = 1.0f;
    
    LPDIRECTSOUNDBUFFER pDSBuffer;
    LPDIRECTSOUNDSTREAM pDSStream;
    
    PXACT_TRACK_EVENT pEvent = (PXACT_TRACK_EVENT) pEventContext->m_pEventHeader;
    PXACT_EVENT_PITCH_VOLUME_VAR_DESC pVarDesc = NULL;

    ASSERT (pEventContext->m_pTrack->pSoundSource);
    pDSBuffer = pEventContext->m_pTrack->pSoundSource->GetDSoundBuffer();
    pDSStream = pEventContext->m_pTrack->pSoundSource->GetDSoundStream();
    
    switch (pEvent->Header.wType) {
    case eXACTEvent_PitchAndVolumeVariation:
        pVarDesc = &pEvent->EventData.PitchAndVolumeVariation.VarDesc;

    case eXACTEvent_PlayWithPitchAndVolumeVariation:

        if (pVarDesc == NULL)
            pVarDesc = &pEvent->EventData.PlayWithPitchAndVolumeVariation.VarDesc;

        ULONG seed = (ULONG)pEventContext->m_rtTimeStamp;
        fRandom = (FLOAT)(RtlRandom(&seed) & 0x7FFFFFFF);
        fRandom = fRandom/(FLOAT)0x7FFFFFFF;

        fRange = (FLOAT)(pVarDesc->Pitch.sPitchHi - 
            pVarDesc->Pitch.sPitchLo);
        
        //
        // create a random pitch value
        // 
        
        fTemp = fRandom*fRange + pVarDesc->Pitch.sPitchLo;
        if (pDSBuffer) {
            hr = pDSBuffer->SetPitch((LONG)fTemp);
        } else {
            hr = pDSStream->SetPitch((LONG)fTemp);
        }

        //
        // create a random volume value
        // 
        
        fRange = (FLOAT)(pVarDesc->Volume.sVolHi - 
            pVarDesc->Volume.sVolLo);
        

        fTemp = fRandom*fRange + pVarDesc->Volume.sVolLo;
        if (pDSBuffer) {
            hr = pDSBuffer->SetVolume((LONG)fTemp);
        } else {
            hr = pDSStream->SetVolume((LONG)fTemp);
        }

        break; 
        
    } // switch


    KeRestoreFloatingPointState(&m_fps);
    return hr;
   
}

#undef DPF_FNAME
#define DPF_FNAME "CEngine::SetTimer"

VOID CEngine::SetTimer()
{

    AutoIrql();

    REFERENCE_TIME rtNextEventTime;
    if(m_pQueue->GetNextEventTime(&rtNextEventTime)){
        
        if(!m_bTimerSet || (rtNextEventTime < m_rtNextEventTime)){
            
            m_rtNextEventTime = rtNextEventTime;
            m_bTimerSet = TRUE;
            
            KeSetTimer(&m_TimerObject, * (LARGE_INTEGER*) &rtNextEventTime, &m_DpcObject); 
            
        }
        
    }

    return;
}

VOID CEngine::DPCTimerCallBack(
        PKDPC Dpc,
        PVOID DeferredContext,
        PVOID SystemArgument1,
        PVOID SystemArgument2
        )
{
    ((CEngine*) DeferredContext)->Dispatch();
}


#if DBG

#undef DPF_FNAME
#define DPF_FNAME "CEngine::PrintTimeStampInfo"

VOID CEngine::PrintTimeStamps(LPCSTR lpcszName, TRACK_EVENT_CONTEXT *pEvent, REFERENCE_TIME rt, REFERENCE_TIME rt1)
{


    LARGE_INTEGER time,time1;
    time.QuadPart = rt;
    time1.QuadPart = rt1;

    if (rt1 == 0) {
        KeQuerySystemTime(&time1);
    }

    //
    // convert to milliseconds
    //

    time.QuadPart = time.QuadPart/10000;
    time1.QuadPart = time1.QuadPart/10000;
    
    if (pEvent) {
        
        DPF_BLAB("Function (%s), EventType (%s), EventTime (%u %u), currentTime (%u %u)\n",             
            lpcszName,
            g_aszXACTEventNames[pEvent->m_pEventHeader->wType],
            time.HighPart,
            time.LowPart,
            time1.HighPart,
            time1.LowPart);

    } else {
        
        DPF_BLAB("Function (%s), EventTime (%u %u), currentTime (%u %u)\n", 
            lpcszName,
            time.HighPart,
            time.LowPart,
            time1.HighPart,
            time1.LowPart);
    }
}

#endif

