/***************************************************************************
 *
 *  Copyright (C) 2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xacti.h
 *  Content:    XACT runtime main internal header file.
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  01/23/2002  georgioc  Created.
 *
 ***************************************************************************/

#ifndef __XACTI_H__
#define __XACTI_H__


#pragma code_seg("XACTENG")
#pragma data_seg("XACTENG_RW")
#pragma const_seg("XACTENG_RD")
#pragma bss_seg("XACTENG_URW")


#define NAMESPACE XACT

#include "common.h"

//
// use a bunch of dsound common utility functions for debug prints, memory alloc etc
//

#include <wavbndlr.h>

//
// API defines for XACT
//

#include "xactp.h"

//
// critical section
//

EXTERN_C CRITICAL_SECTION g_XACTCriticalSection;

__inline BOOL XACTEnterCriticalSection(void)
{
    if(PASSIVE_LEVEL != KeGetCurrentIrql())
    {
        return FALSE;
    }

    EnterCriticalSection(&g_XACTCriticalSection); 

    return TRUE;
}

__inline void XACTLeaveCriticalSection(void)
{
    LeaveCriticalSection(&g_XACTCriticalSection);
}


namespace XACT {

//
// Automatic (functon-scope) locking mechanism
//

class CAutoLock
{
private:
    BOOL                    m_fLocked;

public:
    CAutoLock(void);
    ~CAutoLock(void);
};

__inline CAutoLock::CAutoLock(void)
{
    m_fLocked = (BOOLEAN)XACTEnterCriticalSection();
}

__inline CAutoLock::~CAutoLock(void)
{
    if(m_fLocked)
    {
        XACTLeaveCriticalSection();
    }
}

#define AutoLock() \
    CAutoLock __AutoLock


#define ENTER_EXTERNAL_FUNCTION() \
    AutoLock()

#define ENTER_EXTERNAL_METHOD() \
    AutoLock(); \

#define _ENTER_EXTERNAL_METHOD(name) \
    AutoLock(); \


class CLocalAutoLock
{
private:
    CRITICAL_SECTION        *m_pCr;
    
public:
    CLocalAutoLock(CRITICAL_SECTION *pCr);
    ~CLocalAutoLock(void);
};

__inline CLocalAutoLock::CLocalAutoLock(CRITICAL_SECTION *pCr)
{
    if(PASSIVE_LEVEL == KeGetCurrentIrql())
    {
        EnterCriticalSection(pCr); 
        m_pCr = pCr;
    } else {
        m_pCr = NULL;
    }
    
}

__inline CLocalAutoLock::~CLocalAutoLock(void)
{
    if(m_pCr)
    {
        LeaveCriticalSection(m_pCr);
    }
}




#define ENTER_INTERNAL_METHOD() \
    CLocalAutoLock Lock(&m_cr); \


class CSoundBank;
class CSoundCue;
class CSoundSource;
class CWaveBank;
class CSequencer;
class CEngine;
class CPriorityQueue;

//
// forward declarations
//

typedef struct _TRACK_CONTEXT TRACK_CONTEXT;
typedef struct _TRACK_EVENT_CONTEXT TRACK_EVENT_CONTEXT;
typedef struct _NOTIFICATION_CONTEXT NOTIFICATION_CONTEXT;
typedef struct _CUE_INDEX_NOTIFICATION_CONTEXT CUE_INDEX_NOTIFICATION_CONTEXT;

//
// globals
//

EXTERN_C CEngine * g_pEngine;


//
// xact runtime engine implementation object
//

#define XACT_ENGINE_MAX_CONCURRENT_EVENTS   100
#define XACT_ENGINE_PACKETS_PER_STREAM 2
#define XACT_ENGINE_SCHEDULE_QUANTUM	(5*10000) //5ms in units of 100nsecs

class CEngine
    : public IXACTEngine
{
public:
    CEngine();
    ~CEngine();
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    HRESULT STDMETHODCALLTYPE LoadDspImage(PVOID pvBuffer, DWORD dwSize, LPCDSEFFECTIMAGELOC pEffectLoc);
    HRESULT STDMETHODCALLTYPE CreateSoundSource(DWORD dwFlags, PXACTSOUNDSOURCE *ppSoundSource);
    HRESULT STDMETHODCALLTYPE CreateSoundBank(PVOID pvBuffer, DWORD dwSize, PXACTSOUNDBANK *ppSoundBank);
    HRESULT STDMETHODCALLTYPE RegisterWaveBank(PVOID pvData, DWORD dwSize, PXACTWAVEBANK *ppWaveBank);
    HRESULT STDMETHODCALLTYPE RegisterStreamedWaveBank(PVOID pvStreamingBuffer, DWORD dwSize, HANDLE hFileHandle, DWORD dwOffset, PXACTWAVEBANK *ppWaveBank);
    HRESULT STDMETHODCALLTYPE UnRegisterWaveBank(PXACTWAVEBANK pWaveBank);
    HRESULT STDMETHODCALLTYPE SetMasterVolume(LONG lVolume);
    HRESULT STDMETHODCALLTYPE SetListenerParameters(LPCDS3DLISTENER pcDs3dListener, LPCDSI3DL2LISTENER pds3dl, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE GlobalPause(BOOL bPause); 
    HRESULT STDMETHODCALLTYPE RegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
    HRESULT STDMETHODCALLTYPE UnRegisterNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
    HRESULT STDMETHODCALLTYPE GetNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, PXACT_NOTIFICATION pNotificationEvent);
    HRESULT STDMETHODCALLTYPE FlushNotification(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc);
    HRESULT STDMETHODCALLTYPE CommitDeferredSettings();
    HRESULT STDMETHODCALLTYPE ScheduleEvent(XACT_TRACK_EVENT *pEventDesc, PXACTSOUNDCUE pSoundCue, DWORD dwTrackIndex);

    //
    // non exported methods
    //

    HRESULT Initialize(PXACT_RUNTIME_PARAMETERS pParams);
    HRESULT GetWaveBank(LPCSTR lpFriendlyName, CWaveBank **ppWaveBank);
    VOID    DoWork();
    HRESULT CreateSoundSourceInternal(DWORD dwFlags, CWaveBank *pWaveBank, CSoundSource **ppSoundSource);
    VOID IsDuplicateWaveBank(CWaveBank *pWaveBank);

    BOOL   IsValidSoundSourceFlags(DWORD dwFlags)
    {
        if (!(dwFlags & XACT_MASK_SOUNDSOURCE_FLAGS)) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    //
    // notifications
    //

    VOID HandleNotificationRegistration(PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, BOOL bRegister);
    VOID AddNotificationToPendingList(NOTIFICATION_CONTEXT *pNotification);
    CUE_INDEX_NOTIFICATION_CONTEXT *GetCueNotificationContext(NOTIFICATION_CONTEXT *pContext,DWORD dwSoundCueIndex);

    //
    // sequencer methods
    //

    STDMETHOD(InitializeSequencer)(DWORD dwMaxEvents);
	HRESULT AddCueToSequencerList(CSoundCue *pCue);
    HRESULT RemoveCueFromSequencerList(CSoundCue *pCue);

    VOID GetTime(LPREFERENCE_TIME prtCurrent);

    STDMETHOD(Enqueue)(TRACK_EVENT_CONTEXT* pEvent);
    VOID FreeAllEvents();
    HRESULT CreateEventVariation(TRACK_EVENT_CONTEXT *pEventContext);
    VOID FreeEventsAtOrAfter(TRACK_CONTEXT *pTrack, REFERENCE_TIME timeStamp);
    VOID CreateEventTimeStamp(TRACK_CONTEXT *pTrack, TRACK_EVENT_CONTEXT *pEventContext);

#if DBG
    VOID PrintTimeStamps(LPCSTR lpcszName, TRACK_EVENT_CONTEXT *pEvent, REFERENCE_TIME rt, REFERENCE_TIME rt1);
#else
    VOID PrintTimeStamps(LPCSTR lpcszName, TRACK_EVENT_CONTEXT *pEvent, REFERENCE_TIME rt, REFERENCE_TIME rt1)
    {}
#endif

    VOID SetTimeOffset();

private:

    //
    // This method is called at DPC time to dispatch any currently waiting events
    //

    STDMETHOD(Dispatch)();
    STDMETHOD(DispatchEventsUntil)(const REFERENCE_TIME* pTime, const REFERENCE_TIME* pNow);
    STDMETHOD(DispatchEvent)(TRACK_EVENT_CONTEXT* pEvent);

    VOID SetTimer();

    static VOID DPCTimerCallBack(
        PKDPC Dpc,
        PVOID DeferredContext,
        PVOID SystemArgument1,
        PVOID SystemArgument2
        );

    HRESULT GetEvent(TRACK_EVENT_CONTEXT** ppEvent);
    VOID FreeEvent(TRACK_EVENT_CONTEXT* pEvent);

protected:

    
    friend class CSoundSource;
    HRESULT AllocateSoundSource(CSoundSource **ppSoundSource);
    void FreeSoundSource(CSoundSource *pSoundSource);


    LPDSEFFECTIMAGEDESC     m_pDspImageDesc;
    LPDIRECTSOUND           m_pDirectSound;

    LIST_ENTRY  m_lstAvailable2DBuffers;
    LIST_ENTRY  m_lstAvailable3DBuffers;
    LIST_ENTRY  m_lstAvailableStreams;

    LIST_ENTRY  m_lstWaveBanks;
    LIST_ENTRY  m_lstSoundBanks;

    LIST_ENTRY  m_lstActiveCues;
    LIST_ENTRY  m_lstPendingNotifications;

    DWORD       m_dwTotalVoiceCount;
    DWORD       m_dwRefCount;

    XACT_RUNTIME_PARAMETERS m_RuntimeParams;

    //
    // sequencer variables
    //

    BOOL            m_bAllowQueueing;
    BOOL            m_bTimerSet;

    KTIMER          m_TimerObject;
    KDPC            m_DpcObject;
    KFLOATING_SAVE  m_fps;


    DWORD           m_dwLastPosition;
    ULONGLONG       m_llSampleTime;
    REFERENCE_TIME  m_rtNextEventTime;
    REFERENCE_TIME  m_rtTimeOffset;

    CPriorityQueue  *m_pQueue;

};

//
// context used for tracking the cues associated with a wavebank
//

typedef struct _WAVEBANK_CUE_CONTEXT {

    CSoundCue   *pSoundCue;
    LIST_ENTRY  ListEntry;

} WAVEBANK_CUE_CONTEXT, *PWAVEBANK_CUE_CONTEXT;

//
// notification structs
//

typedef struct _CUE_INDEX_NOTIFICATION_CONTEXT {

    LIST_ENTRY  ListEntry;
    DWORD       dwSoundCueIndex;
    BOOL        bPersist;

} CUE_INDEX_NOTIFICATION_CONTEXT, *PCUE_INDEX_NOTIFICATION_CONTEXT, *LPCUE_INDEX_NOTIFICATION_CONTEXT;

//
// notification context
//

typedef struct _NOTIFICATION_CONTEXT {

    BOOL                    bRegistered;
    XACT_NOTIFICATION       PendingNotification;
    LIST_ENTRY              lstRegisteredCues;
    LIST_ENTRY              ListEntry;

} NOTIFICATION_CONTEXT, *PNOTIFICATION_CONTEXT, *LPNOTIFICATION_CONTEXT;


//
// soundbank object
//

class CSoundBank
    : public IXACTSoundBank, public CRefCount
{
public:
    CSoundBank();
    ~CSoundBank();
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE GetSoundCueIndexFromFriendlyName(LPCSTR lpFriendlyName, PDWORD pdwCueIndex);
    HRESULT STDMETHODCALLTYPE Play( DWORD dwCueIndex, PXACTSOUNDSOURCE pSoundSource, DWORD dwFlags, PXACTSOUNDCUE *ppCue);
    HRESULT STDMETHODCALLTYPE Stop( DWORD dwCueIndex, DWORD dwFlags, PXACTSOUNDCUE pCue);

    //
    // non exported
    //

    NOTIFICATION_CONTEXT * GetNotificationContext(DWORD dwType);    
    VOID ProcessRuntimeEvent(XACT_TRACK_EVENT *pEventDesc);


private:

    friend class CEngine;
    friend class CSoundCue;

    HRESULT Initialize(PVOID pvBuffer, DWORD dwSize);
#if DBG
    BOOL    IsValidCue(DWORD dwCueIndex)
    {
        
        if (dwCueIndex > m_pFileHeader->dwCueEntryCount) {
            return FALSE;
        }

        return TRUE;
    }

    BOOL    IsValidHeader()
    {
        
        if (((DWORD)XACT_SOUNDBANK_HEADER_SIGNATURE) != m_pFileHeader->dwSignature) {
            DPF_ERROR("Invalid soundbank signature in header");
            return FALSE;
        }

        if (XACT_SOUNDBANK_HEADER_VERSION != m_pFileHeader->dwVersion) {

            DPF_ERROR("Invalid version (%d) in header. Current %d",m_pFileHeader->dwVersion, XACT_SOUNDBANK_HEADER_VERSION);
            return FALSE;

        }

        return TRUE;
    }

    BOOL    IsValidSoundSourceForSound(DWORD dwSoundSourceFlags, DWORD dwSoundFlags)
    {
        if ((dwSoundSourceFlags & XACT_FLAG_SOUNDSOURCE_3D) && (dwSoundFlags & XACT_FLAG_SOUNDSOURCE_3D)) {
            return TRUE;
        } else if ((dwSoundSourceFlags & XACT_FLAG_SOUNDSOURCE_3D) && (dwSoundFlags & XACT_FLAG_SOUNDSOURCE_2D)) {
            return TRUE;
        } else if ((dwSoundSourceFlags & XACT_FLAG_SOUNDSOURCE_2D) && (dwSoundFlags & XACT_FLAG_SOUNDSOURCE_2D)) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
#endif

    //
    // utility functions
    //

    PXACT_SOUNDBANK_CUE_ENTRY GetCueTable()
    {
        return (PXACT_SOUNDBANK_CUE_ENTRY) ((PUCHAR)m_pDataBuffer+sizeof(XACT_SOUNDBANK_FILE_HEADER));       
    }

    LPCSTR GetCueFriendlyName(DWORD dwCueIndex)
    {
        PXACT_SOUNDBANK_CUE_ENTRY pCueTable = GetCueTable();
        return pCueTable[dwCueIndex].szFriendlyName;
    }

    PXACT_SOUNDBANK_SOUND_ENTRY GetSoundTable()
    {
        return (PXACT_SOUNDBANK_SOUND_ENTRY) ((PUCHAR)m_pDataBuffer+sizeof(XACT_SOUNDBANK_FILE_HEADER)+
            sizeof(XACT_SOUNDBANK_CUE_ENTRY)*m_pFileHeader->dwCueEntryCount);     
    }

    PUCHAR GetBaseDataOffset()
    {
        return (PUCHAR)m_pDataBuffer;
    }

    void RemoveFromList(CSoundCue *pCue);    

protected:

    friend class CSoundCue;

    PXACT_SOUNDBANK_FILE_HEADER m_pFileHeader;

    PVOID   m_pDataBuffer;
    DWORD   m_dwDataSize;

    LIST_ENTRY  m_ListEntry;
    LIST_ENTRY  m_lstCues;

    DWORD       m_dwPlayingCount;

    NOTIFICATION_CONTEXT    m_aNotificationContexts[eXACTNotification_Max];

};

//
// Cue Object
//

class CSoundCue
    : public CRefCount
{
public:
    CSoundCue();
    ~CSoundCue();
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT Initialize(CSoundBank *pSoundBank, DWORD dwCueIndex, CSoundSource *pSoundSource);
    HRESULT STDMETHODCALLTYPE Play(DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE Stop(DWORD dwFlags);

    VOID DoWork();
    NOTIFICATION_CONTEXT * GetNotificationContext(DWORD dwType);    

    typedef enum _CUE_STATE{
        CUE_STATE_CREATED = 0,
        CUE_STATE_INITIALIZED,
        CUE_STATE_PLAYING,
        CUE_STATE_STOPPING,
        CUE_STATE_STOPPED
    } CUE_STATE;

    VOID GetWaveBank(DWORD dwWaveBankIndex, CWaveBank **ppWaveBank);
    LPCSTR GetFriendlyName()
    {
        return m_pSoundBank->GetCueFriendlyName(m_dwCueIndex);
    }

    PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY GetWaveBankTable()
    {
        return (PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY) ((PUCHAR)m_pSoundBank->GetBaseDataOffset() + m_pSoundEntry->dwWaveBankTableOffset);
    }

protected:

    friend class CSoundBank;
    friend class CWaveBank;
    friend class CEngine;

    CSoundBank                  *m_pSoundBank;

    PXACT_SOUNDBANK_SOUND_ENTRY m_pSoundEntry;

    LIST_ENTRY                  m_ListEntry;
    LIST_ENTRY                  m_SeqListEntry;
    PWAVEBANK_CUE_CONTEXT       m_paWaveBankEntries;
    DWORD                       m_dwCueIndex;


    HRESULT ScheduleTrackEvents(DWORD dwQuantum);
    HRESULT ScheduleRuntimeEvent(XACT_TRACK_EVENT *pEventDesc, DWORD dwTrackIndex);

    VOID ProcessRuntimeEvent(XACT_TRACK_EVENT *pEventDesc);
    
    BOOL IsPositional();

private:

    VOID CheckStateTransition(DWORD dwNewState, PDWORD pdwOldState);
    HRESULT SubmitEvent(TRACK_EVENT_CONTEXT *pEventContext);
    
    NOTIFICATION_CONTEXT        m_aNotificationContexts[eXACTNotification_Max];
    TRACK_CONTEXT               *m_paTracks;
    DWORD                       m_dwState;
    DWORD                       m_dwFlags;
    CSoundSource                *m_pControlSoundSource;

};

//
// soundsource object
//

class CSoundSource
    : public IXACTSoundSource, public CRefCount
{
public:
    CSoundSource();
    ~CSoundSource();
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DBUFFER pcDs3dBuffer, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    HRESULT STDMETHODCALLTYPE SetMixBins(LPCDSMIXBINS pMixBins);
    HRESULT STDMETHODCALLTYPE SetMixBinVolumes(LPCDSMIXBINS pMixBins);

    //
    // utility functions
    //

    DWORD GetFlags()
    {
        DWORD dwSoundSourceFlags = 0;
        if (m_HwVoice.dwFlags & DSBCAPS_CTRL3D) {
            dwSoundSourceFlags |= XACT_FLAG_SOUNDSOURCE_3D;
        }

        return dwSoundSourceFlags;
    }

    BOOL IsPositional()
    {        
        return (m_HwVoice.dwFlags & DSBCAPS_CTRL3D);
    }

    __inline LPDIRECTSOUNDBUFFER GetDSoundBuffer()
    {

        return m_HwVoice.pBuffer;

    }

    __inline LPDIRECTSOUNDSTREAM GetDSoundStream()
    {

        return m_HwVoice.pStream;

    }

    BOOL IsPlaying()
    {

        DWORD dwStatus;

        if (m_HwVoice.pBuffer) {
            
            m_HwVoice.pBuffer->GetStatus(&dwStatus);
            return (dwStatus & DSBSTATUS_PLAYING);

        } else {

            m_HwVoice.pStream->GetStatus(&dwStatus);
            return (dwStatus & (DSSTREAMSTATUS_PLAYING | DSSTREAMSTATUS_PAUSED ));
        }

    }

    VOID SetWaveBankOwner(CWaveBank *pWaveBank)
    {

        m_pWaveBankOwner = pWaveBank;

    }

private:

    friend class CEngine;
    friend class CSoundBank;
    friend class CSequencer;
    friend class CSoundCue;
    friend class CWaveBank;

    HRESULT Initialize();
    HRESULT Stop();
    HRESULT Play();

    LIST_ENTRY              m_ListEntry;
    CWaveBank               *m_pWaveBankOwner;

protected:
    friend class CEngine;

    __inline void SetHwVoiceType(DWORD dwDSoundFlags)
    {
        //
        // dsound flags for buffer/stream
        //

        m_HwVoice.dwFlags = dwDSoundFlags;
    }
    
    struct {
        
        LPDIRECTSOUNDBUFFER     pBuffer;
        LPDIRECTSOUNDSTREAM     pStream;
        DWORD   dwFlags;

    } m_HwVoice;

};



// ****************************************************************************
// private objects
// ****************************************************************************

//
// wavebank object
//

class CWaveBank : public CRefCount
{
public:
    CWaveBank();
    ~CWaveBank();
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT Initialize(PVOID pBuffer, DWORD dwSize);
    HRESULT AllocateSoundSource(CSoundSource **ppSoundSource);
    VOID FreeSoundSource(CSoundSource *pSoundSource);

    VOID RemoveCueFromList(WAVEBANK_CUE_CONTEXT *pEntry);
    VOID AddCueToList(WAVEBANK_CUE_CONTEXT *pEntry);
    VOID StopAllCues();

    __inline LPWAVEBANKENTRY GetWaveBankEntry(DWORD dwWaveIndex)
    {

        return &m_WaveBankData.paMetaData[dwWaveIndex];

    }

    __inline HRESULT SetBufferData(LPDIRECTSOUNDBUFFER pBuffer)
    {

        ASSERT(pBuffer);
        return pBuffer->SetBufferData(m_WaveBankData.pvData, m_WaveBankData.dwDataSize);

    }

protected:
    friend class CEngine;

    WAVEBANKSECTIONDATA m_WaveBankData;
    LIST_ENTRY          m_lstAvailableSources;
    LIST_ENTRY          m_lstCues;
    LIST_ENTRY          m_ListEntry;

};

__inline ULONG CWaveBank::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CWaveBank::AddRef");
    return CRefCount::AddRef();
}


__inline ULONG CWaveBank::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CWaveBank::Release");
    return CRefCount::Release();
}

//
// track context
//

typedef struct _TRACK_CONTEXT {

    //
    // source voice for this track
    //

    CSoundSource        *pSoundSource;

    //
    // parent cue back pointer
    //

    CSoundCue           *pSoundCue;

    //
    // list of currently queued events
    //

	LIST_ENTRY          lstEvents;

    //
    // index into content for next event to process
    //

    WORD                wNextEventDataOffset;
    WORD                wNextEventIndex;

    //
    // loop start indices
    //

    WORD                wLoopStartEventDataOffset;
    WORD                wLoopStartEventIndex;

    //
    // current source playback frequency
    //

    WORD                wSamplesPerSec;
    
    //
    // remaining loop count
    //

    WORD                wLoopCount;

    //
    // timestamp in absolute system time
    //

    REFERENCE_TIME  rtStartTime;
    
    //
    // pointer to the track entry in the soundbank data block
    //

    PXACT_SOUNDBANK_TRACK_ENTRY   pContentEntry;

} TRACK_CONTEXT, *PTRACK_CONTEXT;


//
// track event context
//

typedef struct _TRACK_EVENT_CONTEXT {
    LIST_ENTRY      m_ListEntry;
    DWORD           m_dwQueueIndex;    
    PTRACK_CONTEXT  m_pTrack;
    PXACT_TRACK_EVENT_HEADER    m_pEventHeader;

    REFERENCE_TIME  m_rtTimeStamp;

    BOOL IsLessThanOrEqual(const REFERENCE_TIME* time) const;
    BOOL IsLessThan(const TRACK_EVENT_CONTEXT* other) const;
    BOOL IsGreaterThan(const TRACK_EVENT_CONTEXT* other) const;
    LONG Compare(const TRACK_EVENT_CONTEXT* other) const;

} TRACK_EVENT_CONTEXT, *PTRACK_EVENT_CONTEXT;

//
// priority queue used by sequencer
//

class CPriorityQueue {

public:
    CPriorityQueue();
    ~CPriorityQueue();

    HRESULT Initialize(DWORD maxSize);
    __inline BOOL Initialized() { return m_paEvents != 0; }
    PTRACK_EVENT_CONTEXT Pop();
    PTRACK_EVENT_CONTEXT PopIfLessThanOrEqual(const REFERENCE_TIME* pTime);
    BOOL GetNextEventTime(REFERENCE_TIME* pTime);
    VOID Remove(PTRACK_EVENT_CONTEXT pEvent);
    HRESULT Push(PTRACK_EVENT_CONTEXT pEvent);
    VOID AdjustEventTimes(REFERENCE_TIME delta);

#ifdef DBG
    VOID Verify();
    VOID Print();
    VOID Print2(bool bPrintEvents);
#endif

    __inline DWORD Size() { return m_dwSize; }
private:

    PTRACK_EVENT_CONTEXT Pop2();
    PTRACK_EVENT_CONTEXT At(DWORD index); // One based index into priority queue
    VOID AtPut(DWORD index, PTRACK_EVENT_CONTEXT pEvent);
    VOID Swap(DWORD index1, DWORD index2);
    VOID Move(DWORD dest, DWORD source);
    PTRACK_EVENT_CONTEXT* m_paEvents;
    DWORD m_dwCapacity;
    DWORD m_dwSize;

};

} //namespace





#endif // __XACTI_H__
