// Copyright Microsoft, Inc. All Rights Reserved.

#ifdef XMIX
#ifndef DPF_LIBRARY
#error DPF_LIBRARY Undefined. Are you compiling with Visual Studio? See dmusic\sources.ini for the correct definition.
#endif
#ifndef POOL_TAG
#error POOL_TAG Undefined. Are you compiling with Visual Studio? See dmusic\sources.ini for the correct definition.
#endif

#if defined(DBG) || defined(PROFILE)
#define IMPLEMENT_PERFORMANCE_COUNTERS
#endif

#define PoolAlloc PoolAlloc_DMusic
#define DSOUND_NO_OVERRIDE_NEW_DELETE

#include "xalloc.h" // DMusic memory allocator

/*
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define NODSOUND
#include <xtl.h>
#include <dsoundp.h>
#include "..\..\dsound\dsound\debug.h"
*/

#include "dsoundi.h"
#include "..\dmime\cmixbins.h"

// DMusic assumes DBG is only defined if debugging is happening.
// But dsoundi.h includes ntos.h which includes ntosdef.h, which defines
// DBG to be 0 when there's no debugging.
// So if DBG is equal to zero, we undefine it for the rest of the file

#ifdef DBG
#if DBG == 0
#undef DBG
#endif
#endif

// Double-check to make sure DBG is undefined during release builds.

#if defined(DBG) && defined(NDEBUG)
#error DBG and NDEBUG symbols are both defined.
#endif

#include "dsoundsequencer.h"

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
#include "dm.h"
#include <xbdm.h>

extern "C" void DMusicRegisterPerformanceCounter(const char* szName, unsigned long dwType, void* pvArg){
    DmTell_RegisterPerformanceCounter(szName, dwType, pvArg);
}

extern "C" void DMusicUnregisterPerformanceCounter(const char* szName){
    DmTell_UnregisterPerformanceCounter(szName);
}

#endif

#ifdef DBG
extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);
#define DEBUGF(A) DbgPrint A
#else
#define DEBUGF(A) (void) 0
#endif

// Start of compile-time options

#ifdef DBG

#define DO_VERIFY_PRIORITYQUEUE
//#define DO_FULL_VERIFY_PRIORITYQUEUE // Warning - the verification takes too long when there are a lot of events released at once in a DPC
//#define PRINT_PRIORITYQUEUE
//#define PRINT_QUEUE_OPS
//#define PRINT_EACH_CALL
//#define TRACK_PLAYING
//#define PRINT_SQUELCHED_PLAYS
//#define PRINT_CALLS_AFTER_EVENT_NUMBER 0x8d50
//#define WARN_IF_IN_PAST
//#define ALWAYS_QUEUE
#define TRACK_BUFFER_STATE
//#define PRINT_EACH_ENQUEUE

#endif // DBG

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
#define GATHER_STATISTICS
#endif

// Options that can be enabled even when not debugging

// #define IGNORE_REDUNDANT_SETMIXBINS // Not needed any more - DSound takes care of this

// End of compile-time options

#define SAFE_RELEASE(p) do { if(p) { (p)->Release(); (p)=NULL; } } while(0)

#ifdef GATHER_STATISTICS
#define NOTES_PER_STAT 400
#endif

#ifdef PRINT_CALLS_AFTER_EVENT_NUMBER
#define EACH_CALL_DEBUGF(A) do { if( m_dwTotalEvents > PRINT_CALLS_AFTER_EVENT_NUMBER) { DEBUGF(A); } } while(0)
#elif defined(PRINT_EACH_CALL)
#define EACH_CALL_DEBUGF(A) DEBUGF(A)
#else
#define EACH_CALL_DEBUGF(A) (void) 0
#endif

#if defined(PRINT_EACH_ENQUEUE)
#define EACH_ENQUEUE_DEBUGF(A) DEBUGF(A)
#else
#define EACH_ENQUEUE_DEBUGF(A) (void) 0
#endif

#ifdef PRINT_QUEUE_OPS
#define DEBUGF_QUEUE_OPS(A) DEBUGF(A)
#else
#define DEBUGF_QUEUE_OPS(A) (void) 0
#endif

#ifdef DO_VERIFY_PRIORITYQUEUE
#define SAFE_VPQ(QUEUE) do { RAISE_DPC_IRQL; QUEUE.Verify(); } while(0)
#else
#define SAFE_VPQ(QUEUE) ((void) 0)
#endif

class CDirectSoundSequencer;
struct Event;
typedef Event* LPEVENT;

class EnterDPCIRQL {
public:
    EnterDPCIRQL(){
        m_oldIRQL = KeRaiseIrqlToDpcLevel();
    }
    ~EnterDPCIRQL(){
        KeLowerIrql(m_oldIRQL);
    }
private:
    KIRQL m_oldIRQL;
};


#define RAISE_DPC_IRQL EnterDPCIRQL __enterDPCIRQL

#ifdef DBG
#define ASSERT_IN_DPC ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL)
#define ASSERT_IN_PASSIVE ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL)
#else
#define ASSERT_IN_DPC ((void) 0)
#define ASSERT_IN_PASSIVE ((void) 0)
#endif

class PriorityQueue {

public:
    PriorityQueue();
    ~PriorityQueue();
    HRESULT Initialize(DWORD maxSize);
    inline bool Initialized() { return m_pEvents != 0; }
    Event* Pop();
    Event* PopIfLessThanOrEqual(const REFERENCE_TIME* pTime);
    bool GetNextEventTime(REFERENCE_TIME* pTime);
    void Remove(Event* pEvent);
    HRESULT Push(Event* pEvent);
    void AdjustEventTimes(REFERENCE_TIME delta);
#ifdef DBG
    void Verify();
    void Print();
    void Print2(bool bPrintEvents);
#endif
    inline DWORD Size() { return m_size; }
private:
    Event* Pop2();
    Event* At(DWORD index); // One based index into priority queue
    void AtPut(DWORD index, Event* event);
    void Swap(DWORD index1, DWORD index2);
    void Move(DWORD dest, DWORD source);
    Event** m_pEvents;
    DWORD m_capacity;
    DWORD m_size;
    __int64 m_pseudoTime;
};

typedef enum EventType {
    eBufferPlay,
    eBufferStop,
    eBufferStopEx,
    eBufferSetPlayRegion,
    eBufferSetLoopRegion,
    eBufferSetCurrentPosition,
    eBufferSetBufferData,
    eBufferSetFrequency,
    eBufferSetPitch,
    eBufferSetVolume,
    eBufferSetLFO,
    eBufferSetEG,
    eBufferSetFilter,
    eBufferSetOutputBuffer,
    eBufferSetMixBins,
    eBufferSetMixBinVolumes,
    eBufferSetAllParameters,
    eBufferSetConeAngles,
    eBufferSetConeOrientation,
    eBufferSetConeOutsideVolume,
    eBufferSetMaxDistance,
    eBufferSetMinDistance,
    eBufferSetMode,
    eBufferSetPosition,
    eBufferSetVelocity,
    eBufferSetNotificationPositions,

    eStreamProcess,
    eStreamDiscontinuity,
    eStreamFlush,
    eStreamSetFrequency,
    eStreamSetPitch,
    eStreamSetVolume,
    eStreamSetLFO,
    eStreamSetEG,
    eStreamSetFilter,
    eStreamSetOutputBuffer,
    eStreamSetMixBins,
    eStreamSetMixBinVolumes,
    eStreamPause,
    eStreamSetAllParameters,
    eStreamSetConeAngles,
    eStreamSetConeOrientation,
    eStreamSetConeOutsideVolume,
    eStreamSetMaxDistance,
    eStreamSetMinDistance,
    eStreamSetMode,
    eStreamSetPosition,
    eStreamSetVelocity,

};

struct Buffer_Play {
    DWORD dwFlags;
};

struct Buffer_Stop {
};

struct Buffer_StopEx {
    DWORD dwFlags;
};

struct Buffer_SetPlayRegion {
    DWORD dwPlayStart;
    DWORD dwPlayLength;
};

struct Buffer_SetLoopRegion {
    DWORD dwLoopStart;
    DWORD dwLoopLength;
};

struct Buffer_SetCurrentPosition {
    DWORD dwPlayCursor;
};

struct Buffer_SetBufferData {
    LPVOID pvBufferData;
    DWORD dwBufferBytes;
};

struct Buffer_SetFrequency {
    DWORD dwFrequency;
};

struct Buffer_SetPitch {
    LONG lPitch;
};

struct Buffer_SetVolume {
    LONG lVolume;
};

struct Buffer_SetLFO {
    DSLFODESC lFODesc;
};

struct Buffer_SetEG {
    DSENVELOPEDESC envelopeDesc;
};

struct Buffer_SetFilter {
    DSFILTERDESC filterDesc;
};

struct Buffer_SetOutputBuffer {
    LPDIRECTSOUNDBUFFER pOutputBuffer;
};

struct Buffer_SetMixBins {
    DSMIXBINS DSMixBins;
    DSMIXBINVOLUMEPAIR VolumePairs[DSMIXBIN_ASSIGNMENT_MAX];

};

struct Buffer_SetMixBinVolumes {
    DSMIXBINS DSMixBins;
    DSMIXBINVOLUMEPAIR VolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
};

struct Buffer_SetAllParameters {
    unsigned char ds3db[sizeof(DS3DBUFFER)]; // avoid constructor problems
    DWORD dwApply;
};

struct Buffer_SetConeAngles {
    DWORD dwInsideConeAngle;
    DWORD dwOutsideConeAngle;
    DWORD dwApply;
};

struct Buffer_SetConeOrientation {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    DWORD dwApply;
};

struct Buffer_SetConeOutsideVolume {
    LONG lConeOutsideVolume;
    DWORD dwApply;
};

struct Buffer_SetMaxDistance {
    FLOAT flMaxDistance;
    DWORD dwApply;
};

struct Buffer_SetMinDistance {
    FLOAT flMinDistance;
    DWORD dwApply;
};

struct Buffer_SetMode {
    DWORD dwMode;
    DWORD dwApply;
};

struct Buffer_SetPosition {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    DWORD dwApply;
};

struct Buffer_SetVelocity {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    DWORD dwApply;
};

struct Buffer_SetNotificationPositions {
    DWORD dwNotifyCount;
    DSBPOSITIONNOTIFY aNotifies[4];
};

struct Stream_Process {
    XMEDIAPACKET inputPacket;
};

struct Stream_Discontinuity {
};

struct Stream_Flush {
};

struct Stream_Pause {
    DWORD dwPause;
};

union EventUnion {
    Buffer_Play bufferPlay;
    Buffer_Stop bufferStop;
    Buffer_StopEx bufferStopEx;
    Buffer_SetPlayRegion bufferSetPlayRegion;
    Buffer_SetLoopRegion bufferSetLoopRegion;
    Buffer_SetCurrentPosition bufferSetCurrentPosition;
    Buffer_SetBufferData bufferSetBufferData;
    Buffer_SetFrequency bufferSetFrequency;
    Buffer_SetPitch bufferSetPitch;
    Buffer_SetVolume bufferSetVolume;
    Buffer_SetLFO bufferSetLFO;        
    Buffer_SetEG bufferSetEG;  
    Buffer_SetFilter bufferSetFilter;
    Buffer_SetOutputBuffer bufferSetOutputBuffer;
    Buffer_SetMixBins bufferSetMixBins;
    Buffer_SetMixBinVolumes bufferSetMixBinVolumes;
    Buffer_SetAllParameters bufferSetAllParameters;
    Buffer_SetConeAngles bufferSetConeAngles;
    Buffer_SetConeOrientation bufferSetConeOrientation;
    Buffer_SetConeOutsideVolume bufferSetConeOutsideVolume;
    Buffer_SetMaxDistance bufferSetMaxDistance;
    Buffer_SetMinDistance bufferSetMinDistance;
    Buffer_SetMode bufferSetMode;
    Buffer_SetPosition bufferSetPosition;
    Buffer_SetVelocity bufferSetVelocity;
    Buffer_SetNotificationPositions bufferSetNotificationPositions;

    Stream_Process streamProcess;
    Stream_Discontinuity streamDiscontinuity;
    Stream_Flush streamFlush;
    Stream_Pause streamPause;
};

struct EventArgs {
    REFERENCE_TIME timeStamp; // When this event should fire
    EventType eventType;
    EventUnion body;
};        

struct ErrorInfo {
    HRESULT error;
    SEQUENCE_NUMBER errorSequenceNumber;
    REFERENCE_TIME errorTime;
};

class Buffer;
class Stream;

union EventTarget {
    Buffer* m_pBuffer;
    Stream* m_pStream;
};

struct Event {
    Event* pPrevSameOwner;   // Previous event same owner
    Event* pNextSameOwner;   // Next event same owner
    DWORD m_priorityQueueIndex; // Makes PriorityQueue::Remove fast
    SEQUENCE_NUMBER sequenceNumber;
    __int64 m_pseudoTime;
    EventTarget m_target;
    EventArgs eventArgs;

    bool IsLessThanOrEqual(const REFERENCE_TIME* time) const;
    bool IsLessThan(const Event* other) const;
    bool IsGreaterThan(const Event* other) const;
    int Compare(const Event* other) const;

    bool IsBufferEvent() const;

    void ReleaseMembers();

};

class Buffer : public IDirectSoundSequencerBuffer {
public:
    Buffer();
    ~Buffer();

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    STDMETHOD(Initialize)(CDirectSoundSequencer* pSequencer, LPCDSBUFFERDESC pdsbd);
    STDMETHOD(Clear)();
    STDMETHOD(ClearAtOrAfter)(THIS_ REFERENCE_TIME timeStamp);
    STDMETHOD(GetSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber); // Associated with next event to be queued
    STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus);

    STDMETHOD(GetError)(THIS_ HRESULT* pError); // returns S_OK if no error is recorded
    STDMETHOD(GetErrorSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber);
    STDMETHOD(GetErrorTime)(LPREFERENCE_TIME prtErrorTime);
    STDMETHOD(ClearError)();

    // DirectSoundBuffer methods

    STDMETHOD(Play)                (THIS_ REFERENCE_TIME timeStamp, DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags);
    STDMETHOD(Stop)                (THIS_ REFERENCE_TIME timeStamp);
    STDMETHOD(StopEx)              (THIS_ REFERENCE_TIME timeStamp, DWORD dwFlags);
    STDMETHOD(SetPlayRegion)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwPlayStart, DWORD dwPlayLength);
    STDMETHOD(SetLoopRegion)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwLoopStart, DWORD dwLoopLength);
    STDMETHOD(SetCurrentPosition)  (THIS_ REFERENCE_TIME timeStamp, DWORD dwPlayCursor);
    STDMETHOD(SetBufferData)       (THIS_ REFERENCE_TIME timeStamp, LPVOID pvBufferData, DWORD dwBufferBytes);
    STDMETHOD(SetFrequency)        (THIS_ REFERENCE_TIME timeStamp, DWORD dwFrequency);
    STDMETHOD(SetPitch)            (THIS_ REFERENCE_TIME timeStamp, LONG dwPitch);
    STDMETHOD(SetVolume)           (THIS_ REFERENCE_TIME timeStamp, LONG lVolume);
    STDMETHOD(SetLFO)              (THIS_ REFERENCE_TIME timeStamp, LPCDSLFODESC pLFODesc);
    STDMETHOD(SetEG)               (THIS_ REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc);
    STDMETHOD(SetFilter)           (THIS_ REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc);
    STDMETHOD(SetOutputBuffer)     (THIS_ REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer);
    STDMETHOD(SetMixBins)          (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins);
    STDMETHOD(SetMixBinVolumes)    (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins);

    // IDirectSound3DBuffer methods
    STDMETHOD(SetAllParameters)    (THIS_ REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply);
    STDMETHOD(SetConeAngles)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply);
    STDMETHOD(SetConeOrientation)  (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    STDMETHOD(SetConeOutsideVolume)(THIS_ REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply);
    STDMETHOD(SetMaxDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply);
    STDMETHOD(SetMinDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply);
    STDMETHOD(SetMode)             (THIS_ REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply);
    STDMETHOD(SetPosition)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    STDMETHOD(SetVelocity)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);

    // IDirectSoundNotify methods
    STDMETHOD(SetNotificationPositions)(THIS_ REFERENCE_TIME sampleTime, DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies);

    // Dispatcher callback

    STDMETHOD(DispatchEvent)(EventArgs* pEventArgs);

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    static void RegisterPerformanceCounters();
    static void UnregisterPerformanceCounters();
#endif
public:
    // Public member variables -- no other member variables before this because memory layout has to match class Stream

    CDirectSoundSequencer* m_pSequencer;
    DWORD m_dwid;
    Event* m_pHead;
    ErrorInfo m_errorInfo;
    SEQUENCE_NUMBER m_sequenceNumber;

#ifdef TRACK_PLAYING
    bool m_bStopped;
    bool m_bPlaying;
#endif

    LPDIRECTSOUNDBUFFER m_pObject;

private:

    STDMETHOD(Enqueue)(EventArgs* pEventArgs);

    ULONG m_cRef;

#ifdef TRACK_BUFFER_STATE
public:
    void TrackBufferState(const EventArgs* pEventArgs);

private:
    bool m_tbs_bBufferDataValid;
#define NUM_RECORDED_EVENTS 5
    int m_nextEventToRecord;
    EventArgs m_recentEvents[NUM_RECORDED_EVENTS];
#endif

#ifdef IGNORE_REDUNDANT_SETMIXBINS
    CMIXBINS m_MixBins;
#endif

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    static DWORD m_gAllocatedCount;
#endif
};

#define MAX_QUEUED_PACKETS 2

class Stream : public IDirectSoundSequencerStream {
public:
    Stream();
    ~Stream();

    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
        long l = InterlockedDecrement((long*)&m_cRef);
        if (l == 0)
           delete this;
        return l;
    }

    STDMETHOD(Initialize)(CDirectSoundSequencer* pSequencer, LPCDSSTREAMDESC pdssd);
    STDMETHOD(Clear)();
    STDMETHOD(ClearAtOrAfter)(THIS_ REFERENCE_TIME timeStamp);
    STDMETHOD(GetSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber); // Associated with next event to be queued
    STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus);
    STDMETHOD(GetPacketStatus)(THIS_ DWORD dwPacketStatus, LPDWORD pdwStatus);

    STDMETHOD(GetError)(THIS_ HRESULT* pError); // returns S_OK if no error is recorded
    STDMETHOD(GetErrorSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber);
    STDMETHOD(GetErrorTime)(LPREFERENCE_TIME prtErrorTime);
    STDMETHOD(ClearError)();

    // XMediaObject methods
    STDMETHOD(Process)(THIS_ REFERENCE_TIME timeStamp, LPCXMEDIAPACKET pInputPacket, LPDWORD pdwPacketIndex);
    STDMETHOD(Discontinuity)(THIS_ REFERENCE_TIME timeStamp);
    STDMETHOD(Flush)(THIS_ REFERENCE_TIME timeStamp);

    // IDirectSoundStream methods

    STDMETHOD(SetFrequency)        (THIS_ REFERENCE_TIME timeStamp, DWORD dwFrequency);
    STDMETHOD(SetPitch)            (THIS_ REFERENCE_TIME timeStamp, LONG dwPitch);
    STDMETHOD(SetVolume)           (THIS_ REFERENCE_TIME timeStamp, LONG lVolume);
    STDMETHOD(SetLFO)              (THIS_ REFERENCE_TIME timeStamp, LPCDSLFODESC pLFODesc);
    STDMETHOD(SetEG)               (THIS_ REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc);
    STDMETHOD(SetFilter)           (THIS_ REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc);
    STDMETHOD(SetOutputBuffer)     (THIS_ REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer);
    STDMETHOD(SetMixBins)          (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins);
    STDMETHOD(SetMixBinVolumes)    (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins);    
    STDMETHOD(Pause)               (THIS_ REFERENCE_TIME timeStamp, DWORD dwPause);

    // IDirectSound3DBuffer methods
    STDMETHOD(SetAllParameters)    (THIS_ REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply);
    STDMETHOD(SetConeAngles)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply);
    STDMETHOD(SetConeOrientation)  (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    STDMETHOD(SetConeOutsideVolume)(THIS_ REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply);
    STDMETHOD(SetMaxDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply);
    STDMETHOD(SetMinDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply);
    STDMETHOD(SetMode)             (THIS_ REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply);
    STDMETHOD(SetPosition)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);
    STDMETHOD(SetVelocity)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply);

    // Dispatcher callback

    STDMETHOD(DispatchEvent)(EventArgs* pEventArgs);

    // Public methods

    CDirectSoundSequencer* m_pSequencer;
    DWORD m_dwid;
    Event* m_pHead;
    ErrorInfo m_errorInfo;
    SEQUENCE_NUMBER m_sequenceNumber;
#ifdef TRACK_PLAYING
    bool m_bStopped;
    bool m_bPlaying;
#endif

    LPDIRECTSOUNDSTREAM m_pObject;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    static void RegisterPerformanceCounters();
    static void UnregisterPerformanceCounters();
#endif
private:

    STDMETHOD(Enqueue)(EventArgs* pEventArgs);

    ULONG m_cRef;

    DWORD m_dwNextPacketIndex;
    DWORD m_dwPacketStatus[MAX_QUEUED_PACKETS];

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    static DWORD m_gAllocatedCount;
#endif
};

class EventPool {
public:
    EventPool();
    ~EventPool();
    HRESULT Initialize(DWORD numEvents);

    Event*  Allocate(); // Can be called at DPC level
    void    Free(Event* pEvent); // Can be called at DPC level
    void    RecycleEvents(); // Must be called at Passive level.
private:
    Event*  PopRelease();
    void    PushFree(Event*);

    Event*          m_pEvents;
    Event*          m_pFree;
    Event*          m_pToBeReleased; // Events which are done, but for which we haven't
                                     // called Release yet....
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DWORD   m_pFreeEvents;
    DWORD   m_pAllocatedEvents;
    DWORD   m_pReturnedEvents;
    DWORD   m_pMinFreeEvents;
    DWORD   m_pMaxAllocatedEvents;
    DWORD   m_pMaxReturnedEvents;
#endif
};

EventPool::EventPool(){
    m_pEvents = 0;
    m_pFree = 0;
    m_pToBeReleased = 0;
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    m_pFreeEvents = 0;
    m_pAllocatedEvents = 0;
    m_pReturnedEvents = 0;
    m_pMinFreeEvents = 0;
    m_pMaxAllocatedEvents = 0;
    m_pMaxReturnedEvents = 0;

    DMusicRegisterPerformanceCounter( "DMMS event pool allocated events current",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pAllocatedEvents );
    DMusicRegisterPerformanceCounter( "DMMS event pool allocated events max",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pMaxAllocatedEvents );
    DMusicRegisterPerformanceCounter( "DMMS event pool free events current",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pFreeEvents );
    DMusicRegisterPerformanceCounter( "DMMS event pool free events min",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pMinFreeEvents );
    DMusicRegisterPerformanceCounter( "DMMS event pool returned events current",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pReturnedEvents );
    DMusicRegisterPerformanceCounter( "DMMS event pool returned events max",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_pMaxReturnedEvents );

#endif
}

EventPool::~EventPool(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicUnregisterPerformanceCounter( "DMMS event pool allocated events current");
    DMusicUnregisterPerformanceCounter( "DMMS event pool allocated events max");
    DMusicUnregisterPerformanceCounter( "DMMS event pool free events current");
    DMusicUnregisterPerformanceCounter( "DMMS event pool free events min");
    DMusicUnregisterPerformanceCounter( "DMMS event pool returned events current");
    DMusicUnregisterPerformanceCounter( "DMMS event pool returned events max");
#endif

    RecycleEvents();
    delete[] m_pEvents;
}

HRESULT EventPool::Initialize(DWORD numEvents){
    HRESULT hr = S_OK;
    if(SUCCEEDED(hr))
    {
        m_pEvents = new Event[numEvents];
        if(!m_pEvents){
            hr = E_OUTOFMEMORY;
        }
    }
    if(SUCCEEDED(hr))
    {
        // Set up the free list
        m_pFree = m_pEvents;
        memset(m_pEvents, 0, sizeof(Event)*numEvents);
        for(DWORD i = 1; i < numEvents; i++){
           m_pEvents[i-1].pNextSameOwner = &m_pEvents[i];
        }
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
        m_pFreeEvents = numEvents;
        m_pMinFreeEvents = m_pFreeEvents;
#endif
    }
    return hr;
}

Event* EventPool::Allocate(){
    Event* pEvent = NULL;

    RAISE_DPC_IRQL;

    if(m_pFree){
        pEvent = m_pFree;
        m_pFree = pEvent->pNextSameOwner;
        pEvent->pNextSameOwner = 0;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS

        if(pEvent) {
            m_pFreeEvents--;
            if(m_pMinFreeEvents > m_pFreeEvents){
                m_pMinFreeEvents = m_pFreeEvents;
            }
            m_pAllocatedEvents++;
            if(m_pMaxAllocatedEvents < m_pAllocatedEvents){
                m_pMaxAllocatedEvents = m_pAllocatedEvents;
            }
        }

#endif

    }

    return pEvent;
}

void EventPool::Free(Event* pEvent){
    ASSERT(pEvent);
    ASSERT(pEvent->m_priorityQueueIndex == 0); // Not in the priority queue

    RAISE_DPC_IRQL;

    Event* pPrevSameOwner = pEvent->pPrevSameOwner;
    Event* pNextSameOwner = pEvent->pNextSameOwner;
    if(pPrevSameOwner){
        ASSERT(pPrevSameOwner->pNextSameOwner == pEvent);
        pPrevSameOwner->pNextSameOwner = pNextSameOwner;
    }
    else {
        // Removing first event owned by an owner, so update the head
        // Assume Buffer and Stream have the same member offsets. They really ought to inherit from
        // a common "Owner" base class
        Buffer* pOwner = pEvent->m_target.m_pBuffer;
        ASSERT(pOwner->m_pHead == pEvent);
        pOwner->m_pHead = pNextSameOwner;
    }
    if(pNextSameOwner){
        ASSERT(pNextSameOwner->pPrevSameOwner == pEvent);
        pNextSameOwner->pPrevSameOwner = pPrevSameOwner;
    }

    pEvent->pPrevSameOwner = 0;
    pEvent->pNextSameOwner = m_pToBeReleased;
    m_pToBeReleased = pEvent;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    if(pEvent) {
        m_pAllocatedEvents--;
        m_pReturnedEvents++;
        if(m_pMaxReturnedEvents < m_pReturnedEvents){
            m_pMaxReturnedEvents = m_pReturnedEvents;
        }
    }
#endif
}

void EventPool::RecycleEvents(){ // Must not be called at DPC level
    // This function can end up deleting objects, so it should
    // never be called at DPC time...

    ASSERT_IN_PASSIVE;

    Event* pEvent;
    while(pEvent = PopRelease()){

        ASSERT(pEvent->m_priorityQueueIndex == 0);

        pEvent->ReleaseMembers();
        PushFree(pEvent);
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
        m_pFreeEvents++;
        m_pReturnedEvents--;
#endif

    }
}

Event* EventPool::PopRelease(){
    RAISE_DPC_IRQL;

    Event* pEvent = m_pToBeReleased;
    if(pEvent){
        ASSERT(pEvent->m_priorityQueueIndex == 0);
        m_pToBeReleased = pEvent->pNextSameOwner;
    }

    return pEvent;
}

void EventPool::PushFree(Event* pEvent){
    ASSERT(pEvent);
    ASSERT(pEvent->m_priorityQueueIndex == 0);

    RAISE_DPC_IRQL;

    pEvent->pNextSameOwner = m_pFree;
    m_pFree = pEvent;
}

class CDirectSoundSequencer : public IDirectSoundSequencer
{
public:
    CDirectSoundSequencer();
    ~CDirectSoundSequencer();

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    STDMETHOD(Initialize)(LPGUID pguidDeviceId, DWORD maxEvents);
    STDMETHOD(GetTime)(LPREFERENCE_TIME prtCurrent);
    STDMETHOD(SetTime)(REFERENCE_TIME rtCurrent);
    STDMETHOD(SetAbsoluteTime)(REFERENCE_TIME rtSystemTime, REFERENCE_TIME rtCurrent);

    STDMETHOD(CreateBuffer)(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDSEQUENCERBUFFER *ppBuffer);
    STDMETHOD(CreateStream)(THIS_ LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSEQUENCERSTREAM* ppStream);

    STDMETHOD(Enqueue)(void* pOwner, const EventArgs* pEventArgs);

    LPDIRECTSOUND GetDirectSound() { return m_pDirectSound; }

    
private:
    STDMETHOD(SetTimeOffset)(REFERENCE_TIME rtNewOffset);
    // This method is called at DPC time to dispatch any currently waiting events

    STDMETHOD(Dispatch)();
    STDMETHOD(DispatchEventsUntil)(const REFERENCE_TIME* pTime, const REFERENCE_TIME* pNow);
    STDMETHOD(DispatchEvent)(Event* pEvent);

    STDMETHOD(SetTimer)(bool bDelayOneMS);

    static VOID DPCTimerCallBack(
        PKDPC Dpc,
        PVOID DeferredContext,
        PVOID SystemArgument1,
        PVOID SystemArgument2
        );

    ULONG m_cRef;
    bool            m_bAllowQueueing;
    KTIMER          m_timer;
    KDPC            m_dpc;

    bool            m_bTimerSet;
    REFERENCE_TIME  m_nextEventTime;
    REFERENCE_TIME  m_rtOffset;

    LPDIRECTSOUND   m_pDirectSound;

    PriorityQueue   m_queue;
    EventPool       m_eventPool;


#ifdef GATHER_STATISTICS
    __int64         m_minLatency;
    __int64         m_maxLatency;
    __int64         m_totalLatency;
    __int64         m_averageLatency;
    __int64         m_dispatchCount;
    DWORD           m_currentQueued;
    DWORD           m_maxQueued;
    DWORD           m_notesPlayed;
    __int64         m_bFutureInitialized;
    __int64         m_minFutureQueue;
    __int64         m_maxFutureQueue;
    __int64         m_dwImmediateEvents;
    __int64         m_dwTotalEvents;
    
#endif

    void FreeAllEvents();
    void FreeEvent(Event* pEvent);
    void ReleaseEvents();
    HRESULT GetEvent(Event** ppEvent);

public:
    void FreeEvents(Buffer* pBuffer);
    void FreeEventsAtOrAfter(Buffer* pBuffer, REFERENCE_TIME timeStamp);
};

CDirectSoundSequencer::CDirectSoundSequencer()
{
    m_cRef = 1;
    m_bTimerSet = false;
    KeInitializeTimer(&m_timer);
    KeInitializeDpc(&m_dpc, DPCTimerCallBack, this);
#ifdef GATHER_STATISTICS
    m_minLatency = 0;
    m_maxLatency = 0;
    m_totalLatency = 0;
    m_averageLatency = 0;
    m_dispatchCount = 0;
    m_currentQueued = 0;
    m_maxQueued = 0;
    m_notesPlayed = 0;
    m_bFutureInitialized = false;
    m_minFutureQueue = 0;
    m_maxFutureQueue = 0;
    m_dwImmediateEvents = 0;
    m_dwTotalEvents = 0;
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicRegisterPerformanceCounter( "DMMS events immediate",
                                  DMCOUNT_FREQ100MS | DMCOUNT_EVENT  | DMCOUNT_ASYNC64,
                                  &m_dwImmediateEvents );
    DMusicRegisterPerformanceCounter( "DMMS events queued",
                                  DMCOUNT_FREQ100MS | DMCOUNT_EVENT  | DMCOUNT_ASYNC64,
                                  &m_dispatchCount );
    DMusicRegisterPerformanceCounter( "DMMS events total",
                                  DMCOUNT_FREQ100MS | DMCOUNT_EVENT  | DMCOUNT_ASYNC64,
                                  &m_dwTotalEvents );
    DMusicRegisterPerformanceCounter( "DMMS notes played",
                                  DMCOUNT_FREQ100MS | DMCOUNT_EVENT  | DMCOUNT_ASYNC64,
                                  &m_notesPlayed );
    DMusicRegisterPerformanceCounter( "DMMS events queued currently",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_currentQueued );
    DMusicRegisterPerformanceCounter( "DMMS events queued max",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_maxQueued );
    DMusicRegisterPerformanceCounter( "DMMS relative enqueue time earliest (100ns)",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_minFutureQueue );
    DMusicRegisterPerformanceCounter( "DMMS relative enqueue time latest (100ns)",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC64,
                                  &m_maxFutureQueue );
    DMusicRegisterPerformanceCounter( "DMMS dispatch latency minimum (100ns)",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_minLatency );
    DMusicRegisterPerformanceCounter( "DMMS dispatch latency average (100ns)",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_averageLatency );
    DMusicRegisterPerformanceCounter( "DMMS dispatch latency maximum (100ns)",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_maxLatency );

    Buffer::RegisterPerformanceCounters();
    Stream::RegisterPerformanceCounters();

#endif

#endif
    m_rtOffset = 0;
    m_bAllowQueueing = false;
}

CDirectSoundSequencer::~CDirectSoundSequencer()
{
#ifdef GATHER_STATISTICS
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicUnregisterPerformanceCounter( "DMMS events immediate");
    DMusicUnregisterPerformanceCounter( "DMMS events queued");
    DMusicUnregisterPerformanceCounter( "DMMS events total");
    DMusicUnregisterPerformanceCounter( "DMMS notes played");
    DMusicUnregisterPerformanceCounter( "DMMS events queued currently");
    DMusicUnregisterPerformanceCounter( "DMMS events queued max");
    DMusicUnregisterPerformanceCounter( "DMMS relative enqueue time earliest (100ns)");
    DMusicUnregisterPerformanceCounter( "DMMS relative enqueue time latest (100ns)");
    DMusicUnregisterPerformanceCounter( "DMMS dispatch latency minimum (100ns)");
    DMusicUnregisterPerformanceCounter( "DMMS dispatch latency average (100ns)");
    DMusicUnregisterPerformanceCounter( "DMMS dispatch latency maximum (100ns)");

    Buffer::UnregisterPerformanceCounters();
    Stream::UnregisterPerformanceCounters();

#endif
#endif

    {
        RAISE_DPC_IRQL;
        m_bAllowQueueing = false;
    }

    if(m_bTimerSet){
        KeCancelTimer(&m_timer);
    }
    FreeAllEvents(); // From queue
    ReleaseEvents();
}

HRESULT CDirectSoundSequencer::Initialize(LPGUID pguidDeviceId, DWORD maxEvents){

    HRESULT hr = S_OK;

    {
        if(m_bAllowQueueing){
            // Already initialized
            return E_FAIL;
        }

        m_bAllowQueueing = true;

        if(SUCCEEDED(hr)){
            if(m_bTimerSet){
                KeCancelTimer(&m_timer);
                m_bTimerSet = false;
            }
        }

        if(SUCCEEDED(hr))
        {
            hr = m_eventPool.Initialize(maxEvents);
        }
        if(SUCCEEDED(hr))
        {
            hr = m_queue.Initialize(maxEvents);
        }
    }

    if(SUCCEEDED(hr)){
        hr = DirectSoundCreate(pguidDeviceId, &m_pDirectSound, 0);
    }
    return hr;
}

HRESULT CDirectSoundSequencer::GetTime(LPREFERENCE_TIME prtCurrent)
{
    HRESULT hr = S_OK;
    if(SUCCEEDED(hr))
    {
        REFERENCE_TIME rtSystem;
        KeQuerySystemTime((PLARGE_INTEGER) &rtSystem);
        *prtCurrent = rtSystem - m_rtOffset;
    }
    return hr;
}

HRESULT CDirectSoundSequencer::SetTime(REFERENCE_TIME rtCurrent)
{
    HRESULT hr = S_OK;
    if(SUCCEEDED(hr))
    {
        REFERENCE_TIME rtSystem;
        KeQuerySystemTime((PLARGE_INTEGER) &rtSystem);
        hr = SetTimeOffset(rtSystem - rtCurrent);
    }
    // EACH_ENQUEUE_DEBUGF(("SetAbsoluteTime m_rtOffset = %I64d\n", m_rtOffset));
    return hr;
}

HRESULT CDirectSoundSequencer::SetAbsoluteTime(REFERENCE_TIME rtSystemTime, REFERENCE_TIME rtCurrent)
{
    HRESULT hr = S_OK;
    if(SUCCEEDED(hr))
    {
        hr = SetTimeOffset(rtSystemTime - rtCurrent);
    }
    // DEBUGF(("SetAbsoluteTime m_rtOffset = %I64d\n", m_rtOffset));
    return hr;
}

HRESULT CDirectSoundSequencer::SetTimeOffset(REFERENCE_TIME rtNewOffset)
{
    HRESULT hr = S_OK;
    REFERENCE_TIME rtDelta = rtNewOffset - m_rtOffset;
    if(rtDelta){
        // Need to adjust the absolute time stamps on any currently queued events.
        {
            RAISE_DPC_IRQL;
            m_queue.AdjustEventTimes(rtDelta);
            hr = Dispatch(); // Because some events may now be due, and anyway, the timer needs to be reset
        }
        m_rtOffset = rtNewOffset;
    }
    return hr;
}

HRESULT CDirectSoundSequencer::CreateBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDSEQUENCERBUFFER *ppBuffer)
{
    HRESULT hr = S_OK;
    Buffer* pBuffer;
    if(SUCCEEDED(hr))
    {
        pBuffer = new Buffer();
        if(!pBuffer){
            hr = E_OUTOFMEMORY;
        }
    }
    if(SUCCEEDED(hr))
    {
        hr = pBuffer->Initialize(this, pdsbd);
        if(FAILED(hr)){
            delete pBuffer;
            pBuffer = 0;
        }
    }
    if(SUCCEEDED(hr)){
        *ppBuffer = pBuffer;
    }
    return hr;
}

HRESULT CDirectSoundSequencer::CreateStream(LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSEQUENCERSTREAM* ppStream)
{
    HRESULT hr = S_OK;
    Stream* pStream;
    if(SUCCEEDED(hr))
    {
        pStream = new Stream();
        if(!pStream){
            hr = E_OUTOFMEMORY;
        }
    }
    if(SUCCEEDED(hr))
    {
        hr = pStream->Initialize(this, pdssd);
        if(FAILED(hr)){
            delete pStream;
            pStream = 0;
        }
    }
    if(SUCCEEDED(hr)){
        *ppStream = pStream;
    }
    return hr;
}

HRESULT CDirectSoundSequencer::GetEvent(Event** ppEvent)
{
    ASSERT(ppEvent);
    *ppEvent = m_eventPool.Allocate();
    return (*ppEvent != NULL) ? S_OK : E_OUTOFMEMORY;
}

void CDirectSoundSequencer::FreeAllEvents()
{
    RAISE_DPC_IRQL;
    if(m_queue.Initialized()){
        Event* pEvent;
        while(pEvent = m_queue.Pop()){
            FreeEvent(pEvent);
        }
    }
}

void CDirectSoundSequencer::FreeEvent(Event* pEvent)
{
    ASSERT(pEvent);

    SAFE_VPQ(m_queue);

    m_eventPool.Free(pEvent);

    SAFE_VPQ(m_queue);
}

void CDirectSoundSequencer::ReleaseEvents() {
    ASSERT_IN_PASSIVE;

    SAFE_VPQ(m_queue);

    m_eventPool.RecycleEvents();

    SAFE_VPQ(m_queue);
}

void CDirectSoundSequencer::FreeEvents(Buffer* pBuffer)
{
    RAISE_DPC_IRQL;

    ASSERT(pBuffer);
    Event* pNext;
    for(Event* pEvent = pBuffer->m_pHead; pEvent; pEvent = pNext){
        pNext = pEvent->pNextSameOwner;
        m_queue.Remove(pEvent);
        FreeEvent(pEvent);
    }
    pBuffer->m_pHead = 0;
}

void CDirectSoundSequencer::FreeEventsAtOrAfter(Buffer* pBuffer, REFERENCE_TIME timeStamp)
{
    RAISE_DPC_IRQL;

    ASSERT(pBuffer);
    Event* pNext;
    // Add timing offset. 
    timeStamp += m_rtOffset;
    for(Event* pEvent = pBuffer->m_pHead; pEvent; pEvent = pNext){
        pNext = pEvent->pNextSameOwner;
        if(pEvent->eventArgs.timeStamp >= timeStamp){
            m_queue.Remove(pEvent);
            FreeEvent(pEvent);
        }
    }
}

HRESULT CDirectSoundSequencer::Enqueue(void* pvOwner, const EventArgs* pEventArgs)
{
    HRESULT hr = S_OK;

    ReleaseEvents();

    REFERENCE_TIME now;
    REFERENCE_TIME eventTime = pEventArgs->timeStamp;

    bool bExecuteNow = false;
    bool bProcessQueueUpToEvent = false;


#ifdef ALWAYS_QUEUE

    KeQuerySystemTime((PLARGE_INTEGER) &now);


    if(eventTime <= 0){
        // relative to "now"
        eventTime += now;
    }
    else {
        // convert user time to system time
        eventTime += m_rtOffset;
    }
#ifdef WARN_IF_IN_PAST
            __int64 inPast = now - eventTime;
            if (inPast > 100 * 10000){
                DEBUGF(("DirectSoundSequencer: Trying to queue an event %dms in the past.\n",
                    (long) inPast / 10000));
                _asm int 3;
            }
#endif

#else // ! ALWAYS_QUEUE

    if(eventTime == 0){
        bExecuteNow = true; // Avoid getting time if you want the call done "now".
    }
    else {

        KeQuerySystemTime((PLARGE_INTEGER) &now);

        if(eventTime <= 0){
            // relative to "now"
            eventTime += now;
        }
        else {
            // convert user time to system time
            eventTime += m_rtOffset;
        }

        if(now >= eventTime){
            bExecuteNow = true;
            bProcessQueueUpToEvent = true;
#ifdef WARN_IF_IN_PAST
            __int64 inPast = now - eventTime;
            if (inPast > 100 * 10000){
                DEBUGF(("DirectSoundSequencer: Trying to queue an event %dms in the past.\n",
                    (long) inPast / 10000));
                _asm int 3;
            }
#endif
        }
    }

#endif // ! ALWAYS_QUEUE

#ifdef GATHER_STATISTICS
    InterlockedIncrement((PLONG) &m_dwTotalEvents);
#endif

    // If the event is scheduled for now or earlier, execute it right away.
    if(bExecuteNow){
#ifdef GATHER_STATISTICS
        InterlockedIncrement((PLONG) &m_dwImmediateEvents);
#endif
        if(bProcessQueueUpToEvent){
            RAISE_DPC_IRQL;
            DispatchEventsUntil(&eventTime, &now);
        }

        Event dummy;
        Buffer* pOwner = (Buffer*) pvOwner;
        dummy.m_target.m_pBuffer = pOwner;
        dummy.eventArgs = *pEventArgs;
        dummy.sequenceNumber = InterlockedIncrement((PLONG) &pOwner->m_sequenceNumber); // Sequence number is recorded if there is an error. 

        hr = DispatchEvent(&dummy);

        dummy.m_target.m_pBuffer = 0;

        dummy.ReleaseMembers(); // We want to do this even if DispatchEvent failed.
    }
    else {
        RAISE_DPC_IRQL;

        if(!m_bAllowQueueing){
            return E_FAIL;
        }

        Event* pEvent = 0;
        if(SUCCEEDED(hr)){
            hr = GetEvent(&pEvent);
        }

        if(SUCCEEDED(hr)){
            // Assume Buffer and Stream have the same member offsets. They really ought to inherit from
            // a common "Owner" base class
            Buffer* pOwner = (Buffer*) pvOwner;
            pEvent->m_target.m_pBuffer = pOwner;
            pEvent->pPrevSameOwner = NULL;
            pEvent->pNextSameOwner = pOwner->m_pHead;
            if(pOwner->m_pHead){
                pOwner->m_pHead->pPrevSameOwner = pEvent;
            }
            pOwner->m_pHead = pEvent;
            pEvent->m_priorityQueueIndex = 0; // Filled in when inserted into queue
            pEvent->sequenceNumber = InterlockedIncrement((PLONG) &pOwner->m_sequenceNumber);
            pEvent->eventArgs = *pEventArgs;

            pEvent->eventArgs.timeStamp = eventTime;

            if(pEvent->IsBufferEvent()){
                pEvent->m_target.m_pBuffer->AddRef();
            }
            else {
                pEvent->m_target.m_pStream->AddRef();
            }

#ifdef GATHER_STATISTICS
            {
                __int64 future = pEvent->eventArgs.timeStamp - now;
                if(!m_bFutureInitialized){
                    m_bFutureInitialized = true;
                    m_minFutureQueue = future;
                    m_maxFutureQueue = future;
                }
                else {
                    if(future < m_minFutureQueue){ 
                        m_minFutureQueue = future;
                    }
                    if(future > m_maxFutureQueue){ 
                        m_maxFutureQueue = future;
                    }
                }
            }
#endif

            hr = m_queue.Push(pEvent);

#ifdef GATHER_STATISTICS
            m_currentQueued = m_queue.Size();
            if(m_queue.Size() > m_maxQueued){
                m_maxQueued = m_queue.Size();
            }
#endif

        }
        if(SUCCEEDED(hr)){
            hr = SetTimer(false);
        }
    }

    return hr;
}

HRESULT CDirectSoundSequencer::Dispatch(){
    // Execute everything that's less than or equal to now.
    HRESULT hr = S_OK;
    
    REFERENCE_TIME now;
    KeQuerySystemTime((PLARGE_INTEGER) &now);
    EACH_CALL_DEBUGF(("Dispatch %d\n", *(long*) &now));

    m_bTimerSet = false;

    if(SUCCEEDED(hr)){
        hr = DispatchEventsUntil(&now, &now);
    }
    return hr;
}

HRESULT CDirectSoundSequencer::DispatchEventsUntil(const REFERENCE_TIME* pTime, const REFERENCE_TIME* pNow){
    HRESULT hr = S_OK;

    bool bFirstEvent = true;
    if(SUCCEEDED(hr)){
        Event* pEvent;
        while(pEvent = m_queue.PopIfLessThanOrEqual(pNow)){
#ifdef GATHER_STATISTICS
            m_dispatchCount++;
            REFERENCE_TIME actualTime;
            if(bFirstEvent){
                bFirstEvent = false;
                actualTime = *pNow;
            }
            else {
                KeQuerySystemTime((PLARGE_INTEGER) &actualTime);
            }
            __int64 latency = *(__int64*) &actualTime - *(__int64*) & pEvent->eventArgs.timeStamp;
            if(latency > 0){
                m_totalLatency += latency;
                m_averageLatency = m_totalLatency / m_dispatchCount;
            }

            if(latency < m_minLatency || (m_minLatency == 0 && m_maxLatency == 0)){
                m_minLatency = latency;
            }
            if(latency > m_maxLatency){
                m_maxLatency = latency;
            }
#endif
            hr = DispatchEvent(pEvent);
            FreeEvent(pEvent);
        }
#ifdef GATHER_STATISTICS
        m_currentQueued = m_queue.Size();
#endif
    }
    if(SUCCEEDED(hr)){
        hr = SetTimer(false);
    }
    return hr;
}

#if 0

void CDirectSoundSequencer::PrintStats(){
    if(true){
        DWORD minLat = (DWORD) (m_minLatency / 10);
        DWORD maxLat = (DWORD) (m_maxLatency / 10);
        DWORD avgLat = 0;
        if(m_dispatchCount) {
            avgLat = (DWORD) ((m_totalLatency / 10) / m_dispatchCount);
        }
        DEBUGF(("\ndss: notes: %d events total: %d immediate: %d queued: %d\n",
            m_notesPlayed,
            m_dwTotalEvents,
            m_dwImmediateEvents, m_dwTotalEvents - m_dwImmediateEvents));
        DEBUGF(("     latency avg: %dus min: %dus max: %dus. queue size: current %d max %d queue time: min %dms max %dms\n",
            avgLat, minLat, maxLat,
            m_queue.Size(),
            m_maxQueued,
            (int) (m_minFutureQueue / 10000),
            (int) (m_maxFutureQueue / 10000)
            ));
        m_queue.Print2(false);
        m_bPrintStatsNow = false;
    }
}

#endif

#ifdef DBG
const char* gEventTypeToText[] = {
    "eBufferPlay",
    "eBufferStop",
    "eBufferStopEx",
    "eBufferSetPlayRegion",
    "eBufferSetLoopRegion",
    "eBufferSetCurrentPosition",
    "eBufferSetBufferData",
    "eBufferSetFrequency",
    "eBufferSetPitch",
    "eBufferSetVolume",
    "eBufferSetLFO",
    "eBufferSetEG",
    "eBufferSetFilter",
    "eBufferSetOutputBuffer",
    "eBufferSetMixBins",
    "eBufferSetMixBinVolumes",
    "eBufferSetAllParameters",
    "eBufferSetConeAngles",
    "eBufferSetConeOrientation",
    "eBufferSetConeOutsideVolume",
    "eBufferSetMaxDistance",
    "eBufferSetMinDistance",
    "eBufferSetMode",
    "eBufferSetPosition",
    "eBufferSetVelocity",
    "eBufferSetNotificationPositions",

    "eStreamProcess",
    "eStreamDiscontinuity",
    "eStreamFlush",
    "eStreamSetFrequency",
    "eStreamSetPitch",
    "eStreamSetVolume",
    "eStreamSetLFO",
    "eStreamSetEG",
    "eStreamSetFilter",
    "eStreamSetOutputBuffer",
    "eStreamSetMixBins",
    "eStreamSetMixBinVolumes",
    "eStreamPause",
    "eStreamSetAllParameters",
    "eStreamSetConeAngles",
    "eStreamSetConeOrientation",
    "eStreamSetConeOutsideVolume",
    "eStreamSetMaxDistance",
    "eStreamSetMinDistance",
    "eStreamSetMode",
    "eStreamSetPosition",
    "eStreamSetVelocity"
};
#endif

HRESULT CDirectSoundSequencer::DispatchEvent(Event* pEvent){
    HRESULT hr = S_OK;
    if(pEvent->IsBufferEvent()){
        hr = pEvent->m_target.m_pBuffer->DispatchEvent(&pEvent->eventArgs);
    }
    else {
        hr = pEvent->m_target.m_pStream->DispatchEvent(&pEvent->eventArgs);
    }
    if(FAILED(hr)){
#ifdef DBG
        EventType et = pEvent->eventArgs.eventType;
        const char* pEventType = "Unknown";
        if(et >= eBufferPlay && et <= eStreamSetVelocity){
            pEventType = gEventTypeToText[et];
        }
        DEBUGF(("CDirectSoundSequencer::DispatchEvent: error 0x%08x while executing %s target=0x%x, eventType=%s, sequenceNumber=%d\n",
            hr,
            pEvent->IsBufferEvent() ? "Buffer" : "Stream",
            pEvent->m_target.m_pBuffer,
            pEventType,
            pEvent->sequenceNumber));
#endif
        if(SUCCEEDED(pEvent->m_target.m_pBuffer->m_errorInfo.error)){
            // record error (offsets are the same for both buffers and streams)
            pEvent->m_target.m_pBuffer->m_errorInfo.error = hr;
            pEvent->m_target.m_pBuffer->m_errorInfo.errorSequenceNumber = pEvent->sequenceNumber;
            pEvent->m_target.m_pBuffer->m_errorInfo.errorTime = pEvent->eventArgs.timeStamp;
        }
    }
    return hr;
}

HRESULT CDirectSoundSequencer::SetTimer(bool bDelayOneMS){    
    if(bDelayOneMS){
        REFERENCE_TIME now;
        KeQuerySystemTime((PLARGE_INTEGER) &now);
        m_nextEventTime = now + 10000;
        m_bTimerSet = true;
        KeSetTimer(&m_timer, * (LARGE_INTEGER*) &m_nextEventTime, &m_dpc); 
    }
    else {
        REFERENCE_TIME nextEventTime;
        if(m_queue.GetNextEventTime(&nextEventTime)){
            if(!m_bTimerSet || nextEventTime < m_nextEventTime){
                m_nextEventTime = nextEventTime;
                m_bTimerSet = true;
                EACH_CALL_DEBUGF(("SetTimer(%d)\n", *(long*) & nextEventTime));
                KeSetTimer(&m_timer, * (LARGE_INTEGER*) &nextEventTime, &m_dpc); 
            }
        }
    }
    return S_OK;
}

VOID CDirectSoundSequencer::DPCTimerCallBack(
        PKDPC Dpc,
        PVOID DeferredContext,
        PVOID SystemArgument1,
        PVOID SystemArgument2
        )
{
    ((CDirectSoundSequencer*) DeferredContext)->Dispatch();
}

// ------------ Event methods ------------------

bool Event::IsLessThanOrEqual(const REFERENCE_TIME* time) const{
    return eventArgs.timeStamp <= *time;
}

bool Event::IsLessThan(const Event* other) const{
    return Compare(other) == -1;
}

bool Event::IsGreaterThan(const Event* other) const{
    return Compare(other) == 1;
}

int Event::Compare(const Event* other) const{
    // Check time
    if(this->eventArgs.timeStamp < other->eventArgs.timeStamp){
        return -1;
    }
    if(this->eventArgs.timeStamp > other->eventArgs.timeStamp){
        return 1;
    }
    // check owner m_dwid
    if(this->m_pseudoTime < other->m_pseudoTime){
        return -1;
    }
    if(this->m_pseudoTime > other->m_pseudoTime){
        return 1;
    }
    return 0;
}

bool Event::IsBufferEvent() const {
    return eventArgs.eventType < eStreamProcess;
}

void Event::ReleaseMembers()
{
    // This function can end up deleting objects, so it should
    // never be called at DPC time...

    ASSERT_IN_PASSIVE;

    if(IsBufferEvent()){
        SAFE_RELEASE(m_target.m_pBuffer);
    }
    else {
        SAFE_RELEASE(m_target.m_pStream);
    }

    // A few events have reference counted pointers. Need to dereference them here
    // if they're still referenced. (Can happen if Clear() is called with queued events.

    switch(eventArgs.eventType){
    case eBufferSetOutputBuffer:
        SAFE_RELEASE(eventArgs.body.bufferSetOutputBuffer.pOutputBuffer);
        break;
    case eStreamSetOutputBuffer:
        SAFE_RELEASE(eventArgs.body.bufferSetOutputBuffer.pOutputBuffer);
        break;
    default:
        break;
    }

    #ifdef DBG
        memset(this,0xef, sizeof(Event));
        m_priorityQueueIndex = 0;
    #endif
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

PriorityQueue::PriorityQueue(){
    m_pEvents = 0;
    m_capacity = 0;
    m_size = 0;
    m_pseudoTime = 0;
}

PriorityQueue::~PriorityQueue(){
    delete [] m_pEvents;
}

HRESULT PriorityQueue::Initialize(DWORD maxSize){
    delete [] m_pEvents;
    m_capacity = 0;
    m_size = 0;
    m_pEvents = new LPEVENT[maxSize];
    memset(m_pEvents, 0, sizeof(Event*)*maxSize);
    if(!m_pEvents){
        return E_OUTOFMEMORY;
    }
    m_capacity = maxSize;
    VERIFY_PRIORITYQUEUE;
    return S_OK;
}

void PriorityQueue::Remove(Event* pEvent){
    DEBUGF_QUEUE_OPS(("Remove{\n"));
    VERIFY_PRIORITYQUEUE;
    // Bubble the event to the top.
    ASSERT(pEvent->m_priorityQueueIndex);

    DWORD test_node = pEvent->m_priorityQueueIndex;
    while(test_node > 1){
        DWORD parent = test_node / 2;
        Swap(test_node, parent);
        test_node = parent;
    }

    // Remove from queue - this balances the queue again

    Pop2();

    VERIFY_PRIORITYQUEUE;
    DEBUGF_QUEUE_OPS(("} // Remove\n"));
}

bool PriorityQueue::GetNextEventTime(REFERENCE_TIME* pTime){
    VERIFY_PRIORITYQUEUE;
    if(!m_size){
        return false;
    }
    *pTime = At(1)->eventArgs.timeStamp;
    return true;
}

Event* PriorityQueue::PopIfLessThanOrEqual(const REFERENCE_TIME* time){
    DEBUGF_QUEUE_OPS(("PopIfLessThanOrEqual{\n"));
    VERIFY_PRIORITYQUEUE;
    Event* pResult = 0;
    if(m_size && At(1)->IsLessThanOrEqual(time)){
        pResult = Pop2();
    }
    DEBUGF_QUEUE_OPS(("} // PopIfLessThanOrEqual\n"));
    return pResult;
}

Event* PriorityQueue::Pop(){
    DEBUGF_QUEUE_OPS(("Pop{\n"));
    VERIFY_PRIORITYQUEUE;
    Event* pResult = Pop2();
    DEBUGF_QUEUE_OPS(("} // Pop\n"));
    return pResult;
}

Event* PriorityQueue::Pop2(){
    DEBUGF_QUEUE_OPS(("Pop2{\n"));
    // Don't verify the queue, because it won't be valid if we're called from Remove
    // (Because Remove has bubbled the item it wants to remove to the top of the heap.)

    if(m_size <= 0){
        return NULL;
    }

    // Return the top of the queue, and
    // adjust the heap 

    Event* pTop = At(1);
    Move(1, m_size); // last element can be moved because it has no children, doesn't leave hole.

    DWORD test_node = 1;
    for(;;){
        DWORD child;
        if ( ( test_node * 2 ) >= m_size )
            break;
        if ( ( test_node * 2 + 1) >= m_size )
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
    --m_size;
    pTop->m_priorityQueueIndex = 0; // no longer in queue

    VERIFY_PRIORITYQUEUE;
    DEBUGF_QUEUE_OPS(("} // Pop2 returns\n"));
    return pTop;
}

HRESULT PriorityQueue::Push(Event* pEvent){
    VERIFY_PRIORITYQUEUE;
    ASSERT(m_pEvents);
    ASSERT(m_capacity);
    ASSERT(m_capacity >= m_size);
    if(m_capacity <= m_size){
        return E_OUTOFMEMORY;
    }

    pEvent->m_pseudoTime = m_pseudoTime++;

    AtPut(++m_size, pEvent); // At and AtPut are one-based, so pre-increment

    // Bubble into proper position
    DWORD test_node = m_size;
    while ( test_node > 1 ) {
        DWORD parent_node = test_node/2;
        if ( At(parent_node)->IsGreaterThan(At(test_node)) ){
            Swap(parent_node, test_node);
            test_node = parent_node;
        } else
            break;
    }

    VERIFY_PRIORITYQUEUE;
    return S_OK;
}

void PriorityQueue::AdjustEventTimes(REFERENCE_TIME delta){
    for(DWORD i = 1; i <= m_size; i++){
        Event* pEvent = At(i);
        pEvent->eventArgs.timeStamp += delta;
    }
}

void PriorityQueue::AtPut(DWORD index, Event* pEvent){
    ASSERT(1 <= index);
    ASSERT(index <= m_size);
    m_pEvents[index-1] = pEvent;
    pEvent->m_priorityQueueIndex = index;
}

void PriorityQueue::Move(DWORD dest, DWORD source){
    ASSERT(1 <= dest);
    ASSERT(dest <= m_size);
    ASSERT(1 <= source);
    ASSERT(source <= m_size);
    if(source != dest){
        m_pEvents[dest-1] = m_pEvents[source-1];
        m_pEvents[dest-1]->m_priorityQueueIndex = dest;
        m_pEvents[source-1] = 0;
    }
}

Event* PriorityQueue::At(DWORD index){
    ASSERT(1 <= index);
    ASSERT(index <= m_size);
    Event* result = m_pEvents[index-1];
    ASSERT(result);
    return result;
}

void PriorityQueue::Swap(DWORD index1, DWORD index2){
    Event* temp = At(index1);
    AtPut(index1, At(index2));
    AtPut(index2, temp);
}

#ifdef DBG
void PriorityQueue::Print(){
    Print2(true);
}

void PriorityQueue::Print2(bool bPrintEvents){
    REFERENCE_TIME now;
    KeQuerySystemTime((PLARGE_INTEGER) &now);
    __int64 minDelta = 0;
    __int64 maxDelta = 0;
    for(DWORD i = 1; i <= m_size; i++){
        Event* pEvent = At(i);
        __int64 deltams = (pEvent->eventArgs.timeStamp - now) / 10000;
        if(bPrintEvents){
            DEBUGF(("%d: owner: %s[%003d] seq: %I64d eventType: %s time: %I64d (now + %I64d ms) ptime: %I64d\n",
                i,
                pEvent->IsBufferEvent() ? "Buffer" : "Stream",
                pEvent->m_target.m_pBuffer->m_dwid,
                pEvent->sequenceNumber,
                gEventTypeToText[pEvent->eventArgs.eventType],
                pEvent->eventArgs.timeStamp,
                deltams,
                pEvent->m_pseudoTime
                ));
        }
        if(minDelta > deltams || i == 1){
            minDelta = deltams;
        }
        if(maxDelta < deltams || i == 1){
            maxDelta = deltams;
        }
    }
    DEBUGF(("     queued event time range: %d..%d ms\n",
            * (long*) & minDelta,
            * (long*) & maxDelta
        ));
}

void PriorityQueue::Verify(){
#ifdef PRINT_PRIORITYQUEUE
    Print();
#endif
    if(Initialized()){
        ASSERT(m_pEvents);
        ASSERT(m_capacity);
        ASSERT(m_capacity >= m_size);
        DWORD dwMaxSize = m_size;
        if(dwMaxSize > m_capacity){
            dwMaxSize = m_capacity;
        }
#ifndef DO_FULL_VERIFY_PRIORITYQUEUE
        if(dwMaxSize > 20){
            dwMaxSize = 20;
        }
#endif
        for(DWORD i = 1; i <= dwMaxSize; i++){
            Event* pEvent = At(i);
            ASSERT(pEvent->m_priorityQueueIndex == i);
            int parent = i / 2;
            if(parent>0){
                if( !m_pEvents[parent-1]->IsLessThan(m_pEvents[i-1]) ){
                    DEBUGF(("Error: parent: %d is not less than child: %d\n", parent, i));
                    Print();
                    ASSERT(false);
                }
            }
        }
    }
}

#endif // DBG

// Buffer

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
DWORD Buffer::m_gAllocatedCount;

void Buffer::RegisterPerformanceCounters(){
    DMusicRegisterPerformanceCounter( "DMMS allocated Buffers",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_gAllocatedCount );
}

void Buffer::UnregisterPerformanceCounters(){
    DMusicUnregisterPerformanceCounter( "DMMS allocated Buffers");
}

#endif

Buffer::Buffer(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    InterlockedIncrement((long*)&m_gAllocatedCount);
#endif
    m_cRef = 1;
    m_pSequencer = NULL;
    m_pObject = NULL;

    static long gid;

    m_dwid = InterlockedIncrement(&gid)-1;
    m_pHead = NULL;
    memset(&m_errorInfo,0,sizeof(m_errorInfo));
    m_sequenceNumber = 0;
#ifdef TRACK_PLAYING
    m_bStopped = false;
    m_bPlaying = false;
#endif
#ifdef TRACK_BUFFER_STATE
    m_tbs_bBufferDataValid = false;
    m_nextEventToRecord = 0;
#endif
#ifdef IGNORE_REDUNDANT_SETMIXBINS
    CMIXBINS m_MixBins;
#endif

}

Buffer::~Buffer(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    InterlockedDecrement((long*)&m_gAllocatedCount);
#endif
    if(m_pObject){
        m_pObject->Release();
    }
#ifdef DBG
    m_pSequencer = 0;
    m_pObject = 0;
    m_pHead = 0;
    m_sequenceNumber = 0;
#endif

}

HRESULT
Buffer::Initialize(CDirectSoundSequencer* pSequencer, LPCDSBUFFERDESC pdsbd){
    HRESULT hr = S_OK;

    if(m_pObject){
        DEBUGF(("DirectSoundSequencer Buffer::Initialize called twice.\n"));
#ifdef DBG
        _asm int 3;
#endif
        return E_FAIL; // Can't call initialize twice
    }

    m_pSequencer = pSequencer;

    hr = m_pSequencer->GetDirectSound()->CreateSoundBuffer(pdsbd, (LPDIRECTSOUNDBUFFER*) & m_pObject, 0);

    return hr;
}

HRESULT
Buffer::Clear(){
    EACH_ENQUEUE_DEBUGF(("Clear Buffer[%03d]\n", m_dwid));
    m_pSequencer->FreeEvents(this);
    return S_OK;
}

HRESULT
Buffer::ClearAtOrAfter(REFERENCE_TIME timeStamp){
    EACH_ENQUEUE_DEBUGF(("ClearAtOrAfter Buffer[%03d] %I64d\n", m_dwid, timeStamp));
    m_pSequencer->FreeEventsAtOrAfter(this, timeStamp);
    return S_OK;
}

HRESULT
Buffer::GetSequenceNumber(LPSEQUENCE_NUMBER pSequenceNumber){
    RAISE_DPC_IRQL;
    *pSequenceNumber = m_sequenceNumber;
    return S_OK;
}

HRESULT
Buffer::GetStatus(THIS_ LPDWORD pdwStatus){
#ifndef ASSUME_VALID_PARAMETERS
    ASSERT(pdwStatus);
#endif
    DWORD dwStatus = 0;

    {
        RAISE_DPC_IRQL;

        if(m_pObject){
            m_pObject->GetStatus(&dwStatus);
            dwStatus &= (DSSBSTATUS_PLAYING | DSSBSTATUS_LOOPING);
        }
        Event* pEvent = m_pHead;
        while(pEvent){
            if(pEvent->eventArgs.eventType == eBufferPlay){
                dwStatus |= DSSBSTATUS_PENDINGPLAYEVENTS;
                break;
            }
            pEvent = pEvent->pNextSameOwner;
        }
    }

    *pdwStatus = dwStatus;

    return S_OK;
}

HRESULT
Buffer::GetError(HRESULT* pError){
    RAISE_DPC_IRQL;
    *pError = m_errorInfo.error;
    return S_OK;
}

HRESULT
Buffer::GetErrorSequenceNumber(LPSEQUENCE_NUMBER pSequenceNumber){
    RAISE_DPC_IRQL;
    *pSequenceNumber = m_errorInfo.errorSequenceNumber;
    return S_OK;
}

HRESULT
Buffer::GetErrorTime(LPREFERENCE_TIME prtErrorTime){
    RAISE_DPC_IRQL;
    *prtErrorTime = m_errorInfo.errorTime;
    return S_OK;
}

HRESULT
Buffer::ClearError(){
    RAISE_DPC_IRQL;
    memset(&m_errorInfo, 0, sizeof(m_errorInfo));
    return S_OK;
}

HRESULT
Buffer::Play(REFERENCE_TIME timeStamp, DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags){
    EACH_ENQUEUE_DEBUGF(("enqueue Buffer[%03d]->Play %I64d\n", m_dwid, timeStamp));
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferPlay;
    eventArgs.body.bufferPlay.dwFlags = dwFlags;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::Stop(REFERENCE_TIME timeStamp){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferStop;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::StopEx(REFERENCE_TIME timeStamp, DWORD dwFlags){
    EACH_ENQUEUE_DEBUGF(("enqueue Buffer[%03d]->StopEx %I64d\n", m_dwid, timeStamp));
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferStopEx;
    eventArgs.body.bufferStopEx.dwFlags = dwFlags;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetPlayRegion(REFERENCE_TIME timeStamp, DWORD dwPlayStart, DWORD dwPlayLength){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetPlayRegion;
    eventArgs.body.bufferSetPlayRegion.dwPlayStart = dwPlayStart;
    eventArgs.body.bufferSetPlayRegion.dwPlayLength = dwPlayLength;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetLoopRegion(REFERENCE_TIME timeStamp, DWORD dwLoopStart, DWORD dwLoopLength){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetLoopRegion;
    eventArgs.body.bufferSetLoopRegion.dwLoopStart = dwLoopStart;
    eventArgs.body.bufferSetLoopRegion.dwLoopLength = dwLoopLength;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetCurrentPosition(REFERENCE_TIME timeStamp, DWORD dwPlayCursor){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetCurrentPosition;
    eventArgs.body.bufferSetCurrentPosition.dwPlayCursor = dwPlayCursor;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetBufferData(REFERENCE_TIME timeStamp, LPVOID pvBufferData, DWORD dwBufferBytes){

#ifndef ASSUME_VALID_PARAMETERS
#ifdef DVTSNOOPBUG

    if(pvBufferData && dwBufferBytes) {
        if(!(MmQueryAddressProtect(pvBufferData) & PAGE_NOCACHE))
        {
            DEBUGF(("The buffer data must be allocated with PAGE_NOCACHE\n"));
            _asm int 3;
        }
    }

#endif // DVTSNOOPBUG
#endif // !ASSUME_VALID_PARAMETERS

#ifdef TRACK_BUFFER_STATE
    // Don't allow a SetBufferData to zero if we have a queued Play event
    if(! dwBufferBytes ){
        RAISE_DPC_IRQL;
        for(Event* pEvent = m_pHead; pEvent; pEvent = pEvent->pNextSameOwner){
            if (pEvent->eventArgs.eventType == eBufferPlay){
                if(timeStamp <= 0 || pEvent->eventArgs.timeStamp < timeStamp){
                    ASSERT(FALSE); // Clearing the buffer data while a play event is queued.
                }
            }
        }
    }
#endif

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetBufferData;
    eventArgs.body.bufferSetBufferData.pvBufferData = pvBufferData;
    eventArgs.body.bufferSetBufferData.dwBufferBytes = dwBufferBytes;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetFrequency(REFERENCE_TIME timeStamp, DWORD dwFrequency){
    // It turns out that you can't perform floating point operations at DPC time
    // so we can't set the Frequency at DPC time. We can, however, set
    // the pitch at DPC time. So convert this into a SetPitch call
#ifdef DPC_OK_TO_CALL_SETFREQUENCY
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetFrequency;
    eventArgs.body.bufferSetFrequency.dwFrequency = dwFrequency;
    return Enqueue(&eventArgs);
#else
    LONG lPitch = XAudioCalculatePitch(dwFrequency);
    return SetPitch(timeStamp, lPitch);
#endif
}

HRESULT
Buffer::SetPitch(REFERENCE_TIME timeStamp, LONG lPitch){
#ifndef ASSUME_VALID_PARAMETERS

    if(lPitch)
    {
        if((lPitch < DSBPITCH_MIN) || (lPitch > DSBPITCH_MAX))
        {
            DPF_ERROR("Invalid pitch value");
        }
    }

#endif // ASSUME_VALID_PARAMETERS

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetPitch;
    eventArgs.body.bufferSetPitch.lPitch = lPitch;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetVolume(REFERENCE_TIME timeStamp, LONG lVolume){

#ifndef ASSUME_VALID_PARAMETERS

    if((lVolume < DSBVOLUME_MIN) || (lVolume > DSBVOLUME_MAX))
    {
        DEBUGF(("Volume value out-of-bounds: %d\n", lVolume));
        _asm int 3;
    }

#endif // ASSUME_VALID_PARAMETERS

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetVolume;
    eventArgs.body.bufferSetVolume.lVolume = lVolume;
    return Enqueue(&eventArgs);
}

#undef DPF_FNAME
#define DPF_FNAME "Buffer::SetLFO"

HRESULT
Buffer::SetLFO(REFERENCE_TIME timeStamp, LPCDSLFODESC pLfo){
#ifndef ASSUME_VALID_PARAMETERS

    if(!pLfo)
    {
        DPF_ERROR("LFO description not provided");
    }

    if((pLfo->dwLFO < DSLFO_FIRST) && (pLfo->dwLFO > DSLFO_LAST))
    {
        DPF_ERROR("Invalid LFO identifier");
    }

    if(pLfo->dwDelay > 0x7FFF)
    {
        DPF_ERROR("Invalid LFO delay value");
    }

    if(pLfo->dwDelta > 0x3FF)
    {
        DPF_ERROR("Invalid LFO delta value");
    }

    if((pLfo->lPitchModulation < -128) || (pLfo->lPitchModulation > 127))
    {
        DPF_ERROR("Invalid LFO pitch modulation value");
    }

    if((pLfo->lFilterCutOffRange < -128) || (pLfo->lFilterCutOffRange > 127))
    {
        DPF_ERROR("Invalid LFO filter cutoff value");
    }

    if((pLfo->lAmplitudeModulation < -128) || (pLfo->lAmplitudeModulation > 127))
    {
        DPF_ERROR("Invalid LFO amplitude modulation value");
    }

    if(pLfo->dwLFO != DSLFO_MULTI)
    {
        if(pLfo->lFilterCutOffRange || pLfo->lAmplitudeModulation)
        {
            DPF_WARNING("The pitch LFO doesn not support filter cut-off range or amplitude modulation");
        }
    }

#endif // ASSUME_VALID_PARAMETERS

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetLFO;
    eventArgs.body.bufferSetLFO.lFODesc = *pLfo;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetEG(REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetEG;
    eventArgs.body.bufferSetEG.envelopeDesc = *pEnvelopeDesc;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetFilter(REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetFilter;
    eventArgs.body.bufferSetFilter.filterDesc = *pFilterDesc;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetOutputBuffer(REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetOutputBuffer;
    eventArgs.body.bufferSetOutputBuffer.pOutputBuffer = pOutputBuffer;
    if(pOutputBuffer){
        pOutputBuffer->AddRef();
    }
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetMixBins(REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetMixBins;
    ASSERT(pMixBins->dwMixBinCount <= DSMIXBIN_ASSIGNMENT_MAX);
    eventArgs.body.bufferSetMixBins.DSMixBins.dwMixBinCount         = pMixBins->dwMixBinCount;
    eventArgs.body.bufferSetMixBins.DSMixBins.lpMixBinVolumePairs   = NULL;
    memcpy((void *)eventArgs.body.bufferSetMixBins.VolumePairs, (void *)pMixBins->lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * pMixBins->dwMixBinCount);
    
    return Enqueue(&eventArgs);
}


//TODO: There's an assembly language instruction that does this.
//static DWORD CountSetBits(DWORD dwWord){
//    DWORD result = 0;
//    for(int i = 0; i < 32; i++){
//        if((1 << i) & dwWord){
//            result++;
//        }
//    }
//    return result;
//}

HRESULT
Buffer::SetMixBinVolumes(REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins){

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetMixBinVolumes;
    ASSERT(pMixBins->dwMixBinCount <= DSMIXBIN_ASSIGNMENT_MAX);
    eventArgs.body.bufferSetMixBinVolumes.DSMixBins.dwMixBinCount         = pMixBins->dwMixBinCount;
    eventArgs.body.bufferSetMixBinVolumes.DSMixBins.lpMixBinVolumePairs   = NULL;
    memcpy((void *)eventArgs.body.bufferSetMixBinVolumes.VolumePairs, (void *)pMixBins->lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * pMixBins->dwMixBinCount);
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetAllParameters(REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetAllParameters;
    memcpy(eventArgs.body.bufferSetAllParameters.ds3db, pds3db,
        sizeof(eventArgs.body.bufferSetAllParameters.ds3db));
    eventArgs.body.bufferSetAllParameters.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetConeAngles(REFERENCE_TIME timeStamp,
                      DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply)
{
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetConeAngles;
    eventArgs.body.bufferSetConeAngles.dwInsideConeAngle = dwInsideConeAngle;
    eventArgs.body.bufferSetConeAngles.dwOutsideConeAngle = dwOutsideConeAngle;
    eventArgs.body.bufferSetConeAngles.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetConeOrientation(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetConeOrientation;
    eventArgs.body.bufferSetConeOrientation.x = x;
    eventArgs.body.bufferSetConeOrientation.y = y;
    eventArgs.body.bufferSetConeOrientation.z = z;
    eventArgs.body.bufferSetConeOrientation.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetConeOutsideVolume(REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetConeOutsideVolume;
    eventArgs.body.bufferSetConeOutsideVolume.lConeOutsideVolume = lConeOutsideVolume;
    eventArgs.body.bufferSetConeOutsideVolume.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetMaxDistance(REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetMaxDistance;
    eventArgs.body.bufferSetMaxDistance.flMaxDistance = flMaxDistance;
    eventArgs.body.bufferSetMaxDistance.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetMinDistance(REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetMinDistance;
    eventArgs.body.bufferSetMinDistance.flMinDistance = flMinDistance;
    eventArgs.body.bufferSetMinDistance.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetMode(REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetMode;
    eventArgs.body.bufferSetMode.dwMode = dwMode;
    eventArgs.body.bufferSetMode.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetPosition(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetPosition;
    eventArgs.body.bufferSetPosition.x = x;
    eventArgs.body.bufferSetPosition.y = y;
    eventArgs.body.bufferSetPosition.z = z;
    eventArgs.body.bufferSetPosition.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetVelocity(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetVelocity;
    eventArgs.body.bufferSetVelocity.x = x;
    eventArgs.body.bufferSetVelocity.y = y;
    eventArgs.body.bufferSetVelocity.z = z;
    eventArgs.body.bufferSetVelocity.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Buffer::SetNotificationPositions(REFERENCE_TIME timeStamp, DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies){
    const int kMaxNotifyCount = 4;
    if(dwNotifyCount > kMaxNotifyCount){
        dwNotifyCount = kMaxNotifyCount;
    }
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eBufferSetNotificationPositions;
    eventArgs.body.bufferSetNotificationPositions.dwNotifyCount = dwNotifyCount;
    memcpy(eventArgs.body.bufferSetNotificationPositions.aNotifies, paNotifies, dwNotifyCount*sizeof(DSBPOSITIONNOTIFY));
    return Enqueue(&eventArgs);
}



HRESULT
Buffer::DispatchEvent(EventArgs* pEventArgs){
    HRESULT hr = S_OK;
#ifdef TRACK_BUFFER_STATE
    TrackBufferState(pEventArgs);
#endif // TRACK_BUFFER_STATE

    EACH_CALL_DEBUGF(("%d ", (DWORD) pEventArgs->timeStamp));

    switch(pEventArgs->eventType){
    case eBufferPlay:
        EACH_CALL_DEBUGF(("Buffer[%03d]->Play(0,0,0x%x)\n", m_dwid, pEventArgs->body.bufferPlay.dwFlags));
#ifdef TRACK_PLAYING
        if(!m_bStopped){
            m_bPlaying = true;
            hr = m_pObject->Play(
                0,
                0,
                pEventArgs->body.bufferPlay.dwFlags
                );
        }
        else {
#ifdef PRINT_SQUELCHED_PLAYS
            DEBUGF(("...ignoring Play because Buffer[%03d] has already been stopped.\n", m_dwid));
#endif
        }
#else // TRACK_PLAYING
        hr = m_pObject->Play(
            0,
            0,
            pEventArgs->body.bufferPlay.dwFlags
            );
#endif // TRACK_PLAYING

        break;

    case eBufferStop:
        EACH_CALL_DEBUGF(("Buffer[%03d]->Stop()\n", m_dwid));
        hr = m_pObject->Stop();
#ifdef TRACK_PLAYING
        m_bStopped = true;
#endif
        break;

    case eBufferStopEx:
        EACH_CALL_DEBUGF(("Buffer[%03d]->StopEx(%d)\n", m_dwid,
            pEventArgs->body.bufferStopEx.dwFlags
            ));
#ifdef TRACK_PLAYING
#ifdef PRINT_SQUELCHED_PLAYS
        if(!m_bPlaying) {
            DEBUGF(("Buffer[%03d]->StopEx at time %I64d while not playing...\n", m_dwid, pEventArgs->timeStamp));
        }
#endif
        m_bPlaying = false;
#endif
        hr = m_pObject->StopEx(
            0, // Do it now
            pEventArgs->body.bufferStopEx.dwFlags
            );
#ifdef TRACK_PLAYING
        m_bStopped = true;
#endif
        break;

    case eBufferSetPlayRegion:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetPlayRegion(%d, %d)\n", m_dwid,
            pEventArgs->body.bufferSetPlayRegion.dwPlayStart,
            pEventArgs->body.bufferSetPlayRegion.dwPlayLength));
        hr = m_pObject->SetPlayRegion(
            pEventArgs->body.bufferSetPlayRegion.dwPlayStart,
            pEventArgs->body.bufferSetPlayRegion.dwPlayLength
            );
        break;

    case eBufferSetLoopRegion:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetLoopRegion(%d, %d)\n", m_dwid,
            pEventArgs->body.bufferSetLoopRegion.dwLoopStart,
            pEventArgs->body.bufferSetLoopRegion.dwLoopLength));
        hr = m_pObject->SetLoopRegion(
            pEventArgs->body.bufferSetLoopRegion.dwLoopStart,
            pEventArgs->body.bufferSetLoopRegion.dwLoopLength
            );
        break;

    case eBufferSetBufferData:
        {
            EACH_CALL_DEBUGF(("Buffer[%03d]->SetBufferData(0x%x, 0x%x)\n", m_dwid,
                pEventArgs->body.bufferSetBufferData.pvBufferData,
                pEventArgs->body.bufferSetBufferData.dwBufferBytes));
            hr = m_pObject->SetBufferData(
                pEventArgs->body.bufferSetBufferData.pvBufferData,
                pEventArgs->body.bufferSetBufferData.dwBufferBytes
                );
        }
        break;

    case eBufferSetCurrentPosition:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetCurrentPosition(%d)\n", m_dwid,
            pEventArgs->body.bufferSetCurrentPosition.dwPlayCursor));
        hr = m_pObject->SetCurrentPosition(
            pEventArgs->body.bufferSetCurrentPosition.dwPlayCursor
            );
        break;

    case eBufferSetFrequency:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetFrequency(%d)\n", m_dwid,
            pEventArgs->body.bufferSetFrequency.dwFrequency));
        hr = m_pObject->SetFrequency(
            pEventArgs->body.bufferSetFrequency.dwFrequency
            );
        break;

    case eBufferSetPitch:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetPitch(%d)\n", m_dwid,
            pEventArgs->body.bufferSetPitch.lPitch));
        hr = m_pObject->SetPitch(
            pEventArgs->body.bufferSetPitch.lPitch
            );
        break;

    case eBufferSetVolume:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetVolume(%d)\n", m_dwid,
            pEventArgs->body.bufferSetVolume.lVolume));
        hr = m_pObject->SetVolume(
            pEventArgs->body.bufferSetVolume.lVolume
            );
        break;

    case eBufferSetLFO:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetLFO({lfo: %d, delay: %d, delta: %d, pitchMod: %d, filterCutoffRange: %d, ampMod: %d})\n", m_dwid,
            pEventArgs->body.bufferSetLFO.lFODesc.dwLFO,
            pEventArgs->body.bufferSetLFO.lFODesc.dwDelay,
            pEventArgs->body.bufferSetLFO.lFODesc.dwDelta,
            pEventArgs->body.bufferSetLFO.lFODesc.lPitchModulation,
            pEventArgs->body.bufferSetLFO.lFODesc.lFilterCutOffRange,
            pEventArgs->body.bufferSetLFO.lFODesc.lAmplitudeModulation
            ));
        hr = m_pObject->SetLFO(
            &pEventArgs->body.bufferSetLFO.lFODesc
            );
        break;

    case eBufferSetEG:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetEG({eg: %d, mode: %d, delay: %d, attack: %d, hold: %d, decay: %d, release: %d, sustain: %d, pitchScale: %d, filterCutoff: %d})\n", m_dwid,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwEG,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwMode,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwDelay,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwAttack,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwHold,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwDecay,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwRelease,
            pEventArgs->body.bufferSetEG.envelopeDesc.dwSustain,
            pEventArgs->body.bufferSetEG.envelopeDesc.lPitchScale,
            pEventArgs->body.bufferSetEG.envelopeDesc.lFilterCutOff
            ));
        hr = m_pObject->SetEG(
            &pEventArgs->body.bufferSetEG.envelopeDesc
            );
        break;

    case eBufferSetFilter:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetFilter({mode: %d, Q: %d, adw:{%d,%d,%d,%d}})\n", m_dwid,
            pEventArgs->body.bufferSetFilter.filterDesc.dwMode,
            pEventArgs->body.bufferSetFilter.filterDesc.dwQCoefficient,
            pEventArgs->body.bufferSetFilter.filterDesc.adwCoefficients[0],
            pEventArgs->body.bufferSetFilter.filterDesc.adwCoefficients[1],
            pEventArgs->body.bufferSetFilter.filterDesc.adwCoefficients[2],
            pEventArgs->body.bufferSetFilter.filterDesc.adwCoefficients[3]
            ));
        hr = m_pObject->SetFilter(
            &pEventArgs->body.bufferSetFilter.filterDesc
            );
        break;

    case eBufferSetOutputBuffer:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetOutputBuffer(0x%x)\n", m_dwid,
            pEventArgs->body.bufferSetOutputBuffer.pOutputBuffer
            ));
        hr = m_pObject->SetOutputBuffer(
            pEventArgs->body.bufferSetOutputBuffer.pOutputBuffer
            );
        break;

    case eBufferSetMixBins:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetMixBins(...)\n", m_dwid));

        //Must fix up the pointer.
        pEventArgs->body.bufferSetMixBins.DSMixBins.lpMixBinVolumePairs = pEventArgs->body.bufferSetMixBins.VolumePairs;

#ifdef IGNORE_REDUNDANT_SETMIXBINS
        if (m_MixBins != &pEventArgs->body.bufferSetMixBins.DSMixBins) {
#endif

        hr = m_pObject->SetMixBins(
            &pEventArgs->body.bufferSetMixBins.DSMixBins
            );
#ifdef IGNORE_REDUNDANT_SETMIXBINS
        }
#endif
        break;

    case eBufferSetMixBinVolumes:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetMixBinVolumes(...)\n", m_dwid));
        //Must fix up the pointer.
        pEventArgs->body.bufferSetMixBinVolumes.DSMixBins.lpMixBinVolumePairs = pEventArgs->body.bufferSetMixBinVolumes.VolumePairs;
        hr = m_pObject->SetMixBinVolumes(
        &pEventArgs->body.bufferSetMixBinVolumes.DSMixBins
        );
       
        break;

    case eBufferSetAllParameters:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetAllParameters(...)\n", m_dwid
            ));
        hr = m_pObject->SetAllParameters(
            (DS3DBUFFER*) pEventArgs->body.bufferSetAllParameters.ds3db,
            pEventArgs->body.bufferSetAllParameters.dwApply
            );
        break;

    case eBufferSetConeAngles:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetConeAngles(%d,%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeAngles.dwInsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwOutsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwApply
            ));
        hr = m_pObject->SetConeAngles(
            pEventArgs->body.bufferSetConeAngles.dwInsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwOutsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwApply
            );
        break;

    case eBufferSetConeOrientation:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetConeOrientation(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeOrientation.x,
            pEventArgs->body.bufferSetConeOrientation.y,
            pEventArgs->body.bufferSetConeOrientation.z,
            pEventArgs->body.bufferSetConeOrientation.dwApply
            ));
        hr = m_pObject->SetConeOrientation(
            pEventArgs->body.bufferSetConeOrientation.x,
            pEventArgs->body.bufferSetConeOrientation.y,
            pEventArgs->body.bufferSetConeOrientation.z,
            pEventArgs->body.bufferSetConeOrientation.dwApply
            );
        break;

    case eBufferSetConeOutsideVolume:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetConeOutsideVolume(%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeOutsideVolume.lConeOutsideVolume,
            pEventArgs->body.bufferSetConeOutsideVolume.dwApply
            ));
        hr = m_pObject->SetConeOutsideVolume(
            pEventArgs->body.bufferSetConeOutsideVolume.lConeOutsideVolume,
            pEventArgs->body.bufferSetConeOutsideVolume.dwApply
            );
        break;

    case eBufferSetMaxDistance:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetMaxDistance(%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMaxDistance.flMaxDistance,
            pEventArgs->body.bufferSetMaxDistance.dwApply
            ));
        hr = m_pObject->SetMaxDistance(
            pEventArgs->body.bufferSetMaxDistance.flMaxDistance,
            pEventArgs->body.bufferSetMaxDistance.dwApply
            );
        break;

    case eBufferSetMinDistance:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetMaxDistance(%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMinDistance.flMinDistance,
            pEventArgs->body.bufferSetMinDistance.dwApply
            ));
        hr = m_pObject->SetMinDistance(
            pEventArgs->body.bufferSetMinDistance.flMinDistance,
            pEventArgs->body.bufferSetMinDistance.dwApply
            );
        break;

    case eBufferSetMode:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetMode(%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMode.dwMode,
            pEventArgs->body.bufferSetMode.dwApply
            ));
        hr = m_pObject->SetMode(
            pEventArgs->body.bufferSetMode.dwMode,
            pEventArgs->body.bufferSetMode.dwApply
            );
        break;

    case eBufferSetPosition:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetPosition(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetPosition.x,
            pEventArgs->body.bufferSetPosition.y,
            pEventArgs->body.bufferSetPosition.z,
            pEventArgs->body.bufferSetPosition.dwApply
            ));
        hr = m_pObject->SetPosition(
            pEventArgs->body.bufferSetPosition.x,
            pEventArgs->body.bufferSetPosition.y,
            pEventArgs->body.bufferSetPosition.z,
            pEventArgs->body.bufferSetPosition.dwApply
            );
        break;

    case eBufferSetVelocity:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetVelocity(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetVelocity.x,
            pEventArgs->body.bufferSetVelocity.y,
            pEventArgs->body.bufferSetVelocity.z,
            pEventArgs->body.bufferSetVelocity.dwApply
            ));
        hr = m_pObject->SetVelocity(
            pEventArgs->body.bufferSetVelocity.x,
            pEventArgs->body.bufferSetVelocity.y,
            pEventArgs->body.bufferSetVelocity.z,
            pEventArgs->body.bufferSetVelocity.dwApply
            );
        break;

    case eBufferSetNotificationPositions:
        EACH_CALL_DEBUGF(("Buffer[%03d]->SetNotificationPositions(%d,0x%08x)\n", m_dwid,
            pEventArgs->body.bufferSetNotificationPositions.dwNotifyCount,
            &pEventArgs->body.bufferSetNotificationPositions.aNotifies[0]
            ));
        hr = m_pObject->SetNotificationPositions(
            pEventArgs->body.bufferSetNotificationPositions.dwNotifyCount,
            &pEventArgs->body.bufferSetNotificationPositions.aNotifies[0]
            );
        break;
    default:
        hr = E_FAIL;
        break;
    }
    return hr;
}


#ifdef TRACK_BUFFER_STATE

void
Buffer::TrackBufferState(const EventArgs* pEventArgs){
    RAISE_DPC_IRQL;
    m_recentEvents[m_nextEventToRecord] = *pEventArgs;
    m_nextEventToRecord++;
    if (m_nextEventToRecord >= NUM_RECORDED_EVENTS) {
        m_nextEventToRecord = 0;
    }
    switch(pEventArgs->eventType){
    case eBufferPlay:
        ASSERT(m_tbs_bBufferDataValid);
        break;
    case eBufferSetBufferData:
        m_tbs_bBufferDataValid = pEventArgs->body.bufferSetBufferData.dwBufferBytes != 0;
        break;
    default:
        break;
    }
}

#endif // TRACK_BUFFER_STATE

HRESULT
Buffer::Enqueue(EventArgs* pArgs)
{
    return m_pSequencer->Enqueue(this, pArgs);
}

// Stream


#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
DWORD Stream::m_gAllocatedCount;

void Stream::RegisterPerformanceCounters(){
    DMusicRegisterPerformanceCounter( "DMMS allocated Streams",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_gAllocatedCount );
}

void Stream::UnregisterPerformanceCounters(){
    DMusicUnregisterPerformanceCounter( "DMMS allocated Streams");
}

#endif

Stream::Stream(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    InterlockedIncrement((long*)&m_gAllocatedCount);
#endif
    m_cRef = 1;
    static DWORD gid;
    m_pSequencer = NULL;
    m_pObject = NULL;
    m_dwid = InterlockedIncrement((long*)&gid)-1;
    m_pHead = NULL;
    memset(&m_errorInfo,0,sizeof(m_errorInfo));
    m_sequenceNumber = 0;
    m_dwNextPacketIndex = 0;
#ifdef TRACK_PLAYING
    m_bStopped = false;
    m_bPlaying = false;
#endif
}

Stream::~Stream(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    InterlockedDecrement((long*)&m_gAllocatedCount);
#endif
    if(m_pObject){
        m_pObject->Release();
    }
#ifdef DBG
    m_pSequencer = 0;
    m_pObject = 0;
    m_pHead = 0;
    m_sequenceNumber = 0;
#endif

}

HRESULT
Stream::Initialize(CDirectSoundSequencer* pSequencer, LPCDSSTREAMDESC pdssd){
    HRESULT hr = S_OK;

    if(m_pObject){
        DEBUGF(("DirectSoundSequencer Stream::Initialize called twice.\n"));
#ifdef DBG
        _asm int 3;
#endif
        return E_FAIL; // Can't call initialize twice
    }

    m_pSequencer = pSequencer;

    hr = m_pSequencer->GetDirectSound()->CreateSoundStream(pdssd, (LPDIRECTSOUNDSTREAM*) & m_pObject, 0);

    return hr;
}

HRESULT
Stream::Clear(){
    m_pSequencer->FreeEvents((Buffer*) this);
    return S_OK;
}

HRESULT
Stream::ClearAtOrAfter(REFERENCE_TIME timeStamp){
    m_pSequencer->FreeEventsAtOrAfter((Buffer*) this, timeStamp);
    return S_OK;
}

HRESULT
Stream::GetSequenceNumber(LPSEQUENCE_NUMBER pSequenceNumber){
    RAISE_DPC_IRQL;
    *pSequenceNumber = m_sequenceNumber;
    return S_OK;
}


HRESULT
Stream::GetStatus(LPDWORD pdwStatus){
#ifndef ASSUME_VALID_PARAMETERS
    ASSERT(pdwStatus);
#endif
    DWORD dwStatus = 0;

    {
        RAISE_DPC_IRQL;

        if(m_pHead){
            dwStatus |= DSSSSTATUS_PENDINGEVENTS;
        }
    }

    *pdwStatus = dwStatus;

    return S_OK;
}

HRESULT
Stream::GetPacketStatus(DWORD dwPacketIndex, LPDWORD pdwStatus){
#ifndef ASSUME_VALID_PARAMETERS
    ASSERT(pdwStatus);
    ASSERT(dwPacketIndex < MAX_QUEUED_PACKETS);
#endif
    *pdwStatus = m_dwPacketStatus[dwPacketIndex];

    return S_OK;
}

HRESULT
Stream::GetError(HRESULT* pError){
    RAISE_DPC_IRQL;
    *pError = m_errorInfo.error;
    return S_OK;
}

HRESULT
Stream::GetErrorSequenceNumber(LPSEQUENCE_NUMBER pSequenceNumber){
    RAISE_DPC_IRQL;
    *pSequenceNumber = m_errorInfo.errorSequenceNumber;
    return S_OK;
}

HRESULT
Stream::GetErrorTime(LPREFERENCE_TIME prtErrorTime){
    RAISE_DPC_IRQL;
    *prtErrorTime = m_errorInfo.errorTime;
    return S_OK;
}

HRESULT
Stream::ClearError(){
    RAISE_DPC_IRQL;
    memset(&m_errorInfo, 0, sizeof(m_errorInfo));
    return S_OK;
}

HRESULT
Stream::Process(REFERENCE_TIME timeStamp, LPCXMEDIAPACKET pInputPacket, LPDWORD pdwPacketIndex){

#ifndef ASSUME_VALID_PARAMETERS
#ifdef DVTSNOOPBUG

        if(!(MmQueryAddressProtect(pInputPacket->pvBuffer) & PAGE_NOCACHE))
        {
            DEBUGF(("The stream buffer must be allocated with PAGE_NOCACHE\n"));
            _asm int 3;
        }

#endif // DVTSNOOPBUG
#endif // !ASSUME_VALID_PARAMETERS

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamProcess;
    eventArgs.body.streamProcess.inputPacket = *pInputPacket;

    m_dwPacketStatus[m_dwNextPacketIndex] = XMEDIAPACKET_STATUS_NOT_YET_SUBMITTED;
    eventArgs.body.streamProcess.inputPacket.pdwStatus = &m_dwPacketStatus[m_dwNextPacketIndex];

    *pdwPacketIndex = m_dwNextPacketIndex;

    if(++m_dwNextPacketIndex >= MAX_QUEUED_PACKETS){
        m_dwNextPacketIndex = 0;
    }

    return Enqueue(&eventArgs);
}

HRESULT
Stream::Discontinuity(REFERENCE_TIME timeStamp){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamDiscontinuity;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::Flush(REFERENCE_TIME timeStamp){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamFlush;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetFrequency(REFERENCE_TIME timeStamp, DWORD dwFrequency){
    // It turns out that you can't perform floating point operations at DPC time
    // so we can't set the Frequency at DPC time. We can, however, set
    // the pitch at DPC time. So convert this into a SetPitch call
#ifdef DPC_OK_TO_CALL_SETFREQUENCY
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetFrequency;
    eventArgs.body.bufferSetFrequency.dwFrequency = dwFrequency;
    return Enqueue(&eventArgs);
#else
    LONG lPitch = XAudioCalculatePitch(dwFrequency);
    return SetPitch(timeStamp, lPitch);
#endif
}

HRESULT
Stream::SetPitch(REFERENCE_TIME timeStamp, LONG lPitch){
#ifndef ASSUME_VALID_PARAMETERS

    if(lPitch)
    {
        if((lPitch < DSBPITCH_MIN) || (lPitch > DSBPITCH_MAX))
        {
            DPF_ERROR("Invalid pitch value");
        }
    }

#endif // ASSUME_VALID_PARAMETERS

    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetPitch;
    eventArgs.body.bufferSetPitch.lPitch = lPitch;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetVolume(REFERENCE_TIME timeStamp, LONG lVolume){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetVolume;
    eventArgs.body.bufferSetVolume.lVolume = lVolume;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetLFO(REFERENCE_TIME timeStamp, LPCDSLFODESC pLFODesc){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetLFO;
    eventArgs.body.bufferSetLFO.lFODesc = *pLFODesc;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetEG(REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetEG;
    eventArgs.body.bufferSetEG.envelopeDesc = *pEnvelopeDesc;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetFilter(REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetFilter;
    eventArgs.body.bufferSetFilter.filterDesc = *pFilterDesc;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetOutputBuffer(REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetOutputBuffer;
    eventArgs.body.bufferSetOutputBuffer.pOutputBuffer = pOutputBuffer;
    if(pOutputBuffer){
        pOutputBuffer->AddRef();
    }
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetMixBins(REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetMixBins;
    ASSERT(pMixBins->dwMixBinCount <= DSMIXBIN_ASSIGNMENT_MAX);
    eventArgs.body.bufferSetMixBins.DSMixBins.dwMixBinCount         = pMixBins->dwMixBinCount;
    eventArgs.body.bufferSetMixBins.DSMixBins.lpMixBinVolumePairs   = NULL;
    memcpy((void *)eventArgs.body.bufferSetMixBins.VolumePairs, (void *)pMixBins->lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * pMixBins->dwMixBinCount);
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetMixBinVolumes(REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetMixBinVolumes;
    ASSERT(pMixBins->dwMixBinCount <= DSMIXBIN_ASSIGNMENT_MAX);
    eventArgs.body.bufferSetMixBinVolumes.DSMixBins.dwMixBinCount         = pMixBins->dwMixBinCount;
    eventArgs.body.bufferSetMixBinVolumes.DSMixBins.lpMixBinVolumePairs   = NULL;
    memcpy((void *)eventArgs.body.bufferSetMixBinVolumes.VolumePairs, (void *)pMixBins->lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * pMixBins->dwMixBinCount);
    return Enqueue(&eventArgs);
}

HRESULT
Stream::Pause(REFERENCE_TIME timeStamp, DWORD dwPause){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamPause;
    eventArgs.body.streamPause.dwPause = dwPause;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetAllParameters(REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetAllParameters;
    memcpy(eventArgs.body.bufferSetAllParameters.ds3db, pds3db,
        sizeof(eventArgs.body.bufferSetAllParameters.ds3db));
    eventArgs.body.bufferSetAllParameters.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetConeAngles(REFERENCE_TIME timeStamp,
                      DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply)
{
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetConeAngles;
    eventArgs.body.bufferSetConeAngles.dwInsideConeAngle = dwInsideConeAngle;
    eventArgs.body.bufferSetConeAngles.dwOutsideConeAngle = dwOutsideConeAngle;
    eventArgs.body.bufferSetConeAngles.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetConeOrientation(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetConeOrientation;
    eventArgs.body.bufferSetConeOrientation.x = x;
    eventArgs.body.bufferSetConeOrientation.y = y;
    eventArgs.body.bufferSetConeOrientation.z = z;
    eventArgs.body.bufferSetConeOrientation.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetConeOutsideVolume(REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetConeOutsideVolume;
    eventArgs.body.bufferSetConeOutsideVolume.lConeOutsideVolume = lConeOutsideVolume;
    eventArgs.body.bufferSetConeOutsideVolume.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetMaxDistance(REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetMaxDistance;
    eventArgs.body.bufferSetMaxDistance.flMaxDistance = flMaxDistance;
    eventArgs.body.bufferSetMaxDistance.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetMinDistance(REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetMinDistance;
    eventArgs.body.bufferSetMinDistance.flMinDistance = flMinDistance;
    eventArgs.body.bufferSetMinDistance.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetMode(REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetMode;
    eventArgs.body.bufferSetMode.dwMode = dwMode;
    eventArgs.body.bufferSetMode.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetPosition(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetPosition;
    eventArgs.body.bufferSetPosition.x = x;
    eventArgs.body.bufferSetPosition.y = y;
    eventArgs.body.bufferSetPosition.z = z;
    eventArgs.body.bufferSetPosition.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::SetVelocity(REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply){
    EventArgs eventArgs;
    eventArgs.timeStamp = timeStamp;
    eventArgs.eventType = eStreamSetVelocity;
    eventArgs.body.bufferSetVelocity.x = x;
    eventArgs.body.bufferSetVelocity.y = y;
    eventArgs.body.bufferSetVelocity.z = z;
    eventArgs.body.bufferSetVelocity.dwApply = dwApply;
    return Enqueue(&eventArgs);
}

HRESULT
Stream::DispatchEvent(EventArgs* pEventArgs){
    HRESULT hr = S_OK;
    switch(pEventArgs->eventType){
    case eStreamProcess:
        EACH_CALL_DEBUGF(("Stream[%03d]->Process()\n", m_dwid
            ));
#ifdef TRACK_PLAYING
        if(!m_bStopped){
#endif // TRACK_PLAYING
            hr = m_pObject->Process(
                &pEventArgs->body.streamProcess.inputPacket,
                0
                );
#ifdef TRACK_PLAYING
        }
        else {
            EACH_CALL_DEBUGF(("... Process ignored because Stream[%03d] is stopped.\n", m_dwid));
        }
#endif // TRACK_PLAYING
        break;

    case eStreamDiscontinuity:
        EACH_CALL_DEBUGF(("Stream[%03d]->Discontinuity()\n", m_dwid));
        hr = m_pObject->Discontinuity();
        break;

    case eStreamFlush:
#ifdef TRACK_PLAYING
        m_bStopped = true;
#endif
        EACH_CALL_DEBUGF(("Stream[%03d]->Flush()\n", m_dwid
            ));
        hr = m_pObject->Flush(
            );
        break;

    case eStreamSetFrequency:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetFrequency(%d)\n", m_dwid,
            pEventArgs->body.bufferSetFrequency.dwFrequency));
        hr = m_pObject->SetFrequency(
            pEventArgs->body.bufferSetFrequency.dwFrequency
            );
        break;

    case eStreamSetPitch:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetPitch(%d)\n", m_dwid,
            pEventArgs->body.bufferSetPitch.lPitch));
        hr = m_pObject->SetPitch(
            pEventArgs->body.bufferSetPitch.lPitch
            );
        break;

    case eStreamSetVolume:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetVolume(%d)\n", m_dwid,
            pEventArgs->body.bufferSetVolume.lVolume));
        hr = m_pObject->SetVolume(
            pEventArgs->body.bufferSetVolume.lVolume
            );
        break;

    case eStreamSetLFO:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetLFO(...)\n", m_dwid
            ));
        hr = m_pObject->SetLFO(
            &pEventArgs->body.bufferSetLFO.lFODesc
            );
        break;

    case eStreamSetEG:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetEG(...)\n", m_dwid
            ));
        hr = m_pObject->SetEG(
            &pEventArgs->body.bufferSetEG.envelopeDesc
            );
        break;

    case eStreamSetFilter:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetFilter(...)\n", m_dwid
            ));
        hr = m_pObject->SetFilter(
            &pEventArgs->body.bufferSetFilter.filterDesc
            );
        break;

    case eStreamSetOutputBuffer:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetOutputBuffer(...)\n", m_dwid
            ));
        hr = m_pObject->SetOutputBuffer(
            pEventArgs->body.bufferSetOutputBuffer.pOutputBuffer
            );
        break;

    case eStreamSetMixBins:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetMixBins(...)\n", m_dwid));
        //Must fix up the pointer.
        pEventArgs->body.bufferSetMixBins.DSMixBins.lpMixBinVolumePairs = pEventArgs->body.bufferSetMixBins.VolumePairs;
        hr = m_pObject->SetMixBins(
            &pEventArgs->body.bufferSetMixBins.DSMixBins
            );
        break;

    case eStreamSetMixBinVolumes:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetMixBinVolumes(...)\n", m_dwid));
        //Must fix up the pointer.
        pEventArgs->body.bufferSetMixBinVolumes.DSMixBins.lpMixBinVolumePairs = pEventArgs->body.bufferSetMixBinVolumes.VolumePairs;
        hr = m_pObject->SetMixBinVolumes(
        &pEventArgs->body.bufferSetMixBinVolumes.DSMixBins
        );
        break;

    case eStreamPause:
        EACH_CALL_DEBUGF(("Stream[%03d]->Pause(%d)\n", m_dwid,
            pEventArgs->body.streamPause.dwPause));
        hr = m_pObject->Pause(
            pEventArgs->body.streamPause.dwPause
            );
        break;

    case eStreamSetAllParameters:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetAllParameters(...)\n", m_dwid
            ));
        hr = m_pObject->SetAllParameters(
            (DS3DBUFFER*) pEventArgs->body.bufferSetAllParameters.ds3db,
            pEventArgs->body.bufferSetAllParameters.dwApply
            );
        break;

    case eStreamSetConeAngles:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetConeAngles(%d,%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeAngles.dwInsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwOutsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwApply
            ));
        hr = m_pObject->SetConeAngles(
            pEventArgs->body.bufferSetConeAngles.dwInsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwOutsideConeAngle,
            pEventArgs->body.bufferSetConeAngles.dwApply
            );
        break;

    case eStreamSetConeOrientation:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetConeOrientation(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeOrientation.x,
            pEventArgs->body.bufferSetConeOrientation.y,
            pEventArgs->body.bufferSetConeOrientation.z,
            pEventArgs->body.bufferSetConeOrientation.dwApply
            ));
        hr = m_pObject->SetConeOrientation(
            pEventArgs->body.bufferSetConeOrientation.x,
            pEventArgs->body.bufferSetConeOrientation.y,
            pEventArgs->body.bufferSetConeOrientation.z,
            pEventArgs->body.bufferSetConeOrientation.dwApply
            );
        break;

    case eStreamSetConeOutsideVolume:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetConeOutsideVolume(%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetConeOutsideVolume.lConeOutsideVolume,
            pEventArgs->body.bufferSetConeOutsideVolume.dwApply
            ));
        hr = m_pObject->SetConeOutsideVolume(
            pEventArgs->body.bufferSetConeOutsideVolume.lConeOutsideVolume,
            pEventArgs->body.bufferSetConeOutsideVolume.dwApply
            );
        break;

    case eStreamSetMaxDistance:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetMaxDistance(%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMaxDistance.flMaxDistance,
            pEventArgs->body.bufferSetMaxDistance.dwApply
            ));
        hr = m_pObject->SetMaxDistance(
            pEventArgs->body.bufferSetMaxDistance.flMaxDistance,
            pEventArgs->body.bufferSetMaxDistance.dwApply
            );
        break;

    case eStreamSetMinDistance:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetMaxDistance(%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMinDistance.flMinDistance,
            pEventArgs->body.bufferSetMinDistance.dwApply
            ));
        hr = m_pObject->SetMinDistance(
            pEventArgs->body.bufferSetMinDistance.flMinDistance,
            pEventArgs->body.bufferSetMinDistance.dwApply
            );
        break;

    case eStreamSetMode:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetMode(%d,%d)\n", m_dwid,
            pEventArgs->body.bufferSetMode.dwMode,
            pEventArgs->body.bufferSetMode.dwApply
            ));
        hr = m_pObject->SetMode(
            pEventArgs->body.bufferSetMode.dwMode,
            pEventArgs->body.bufferSetMode.dwApply
            );
        break;

    case eStreamSetPosition:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetPosition(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetPosition.x,
            pEventArgs->body.bufferSetPosition.y,
            pEventArgs->body.bufferSetPosition.z,
            pEventArgs->body.bufferSetPosition.dwApply
            ));
        hr = m_pObject->SetPosition(
            pEventArgs->body.bufferSetPosition.x,
            pEventArgs->body.bufferSetPosition.y,
            pEventArgs->body.bufferSetPosition.z,
            pEventArgs->body.bufferSetPosition.dwApply
            );
        break;

    case eStreamSetVelocity:
        EACH_CALL_DEBUGF(("Stream[%03d]->SetVelocity(%g,%g,%g,%d)\n", m_dwid,
            pEventArgs->body.bufferSetVelocity.x,
            pEventArgs->body.bufferSetVelocity.y,
            pEventArgs->body.bufferSetVelocity.z,
            pEventArgs->body.bufferSetVelocity.dwApply
            ));
        hr = m_pObject->SetVelocity(
            pEventArgs->body.bufferSetVelocity.x,
            pEventArgs->body.bufferSetVelocity.y,
            pEventArgs->body.bufferSetVelocity.z,
            pEventArgs->body.bufferSetVelocity.dwApply
            );
        break;

    default:
        hr = E_FAIL;
        break;
    }
    return hr;
}

HRESULT
Stream::Enqueue(EventArgs* pArgs)
{
    return m_pSequencer->Enqueue(this, pArgs);
}

HRESULT
WINAPI DirectSoundCreateSequencer(LPGUID pguidDeviceId, DWORD dwMaxEvents,
                           LPDIRECTSOUNDSEQUENCER * ppDirectSoundSequencer)
{
    HRESULT hr = DS_OK;

    CDirectSoundSequencer* pDirectSoundSequencer = new CDirectSoundSequencer;
    if(!pDirectSoundSequencer){
        hr = E_OUTOFMEMORY;
    }

    if(SUCCEEDED(hr)){
        hr = pDirectSoundSequencer->Initialize(pguidDeviceId, dwMaxEvents);
    }
    if(SUCCEEDED(hr)){
        *ppDirectSoundSequencer = pDirectSoundSequencer;
    }
    else{
        delete pDirectSoundSequencer;
    }

    return hr;
}

#endif // XMIX