//      Copyright (c) 1996-1999 Microsoft Corporation
//	    audiosink.h

#ifndef __AUDIO_SINK__
#define __AUDIO_SINK__

#include <math.h>
#include <mmsystem.h>
#include "dmusicc.h"
#include "dmusics.h"
#include "cclock.h"
#include "PLClock.h"
//#include "dslink.h"
#include <dsound.h>
#include <dsoundp.h>
#include <dmksctrl.h>

DEFINE_GUID(IID_CBuffer, 0xf1e13d57, 0x7c29, 0x4ae6, 0x8d, 0x77, 0x94, 0x32, 0xb6, 0x51, 0xf3, 0xa6);

class CAudioSink;

#define AUDIOSINK_MAX_CHANNELS  2           // Max number of buses connected to buffer. We're limiting to 2 for now.
#define AUDIOSINK_NULL_BUS_ID   0xFFFFFFFF  // NULL bus ID.
#define AUDIOSINK_BUS_SIZE      100         // Bus size, in milliseconds

class CBuffer : 
#ifdef SILVER
public IDirectSoundBuffer,
#endif 
    public AListItem
{
friend class CAudioSink;
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectSoundBuffer methods
    STDMETHODIMP Play(DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags) ;
    STDMETHODIMP PlayEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags) ;
    STDMETHODIMP Stop() ;
    STDMETHODIMP StopEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags) ;
    STDMETHODIMP SetLoopRegion(DWORD dwLoopStart, DWORD dwLoopLength) ;
    STDMETHODIMP GetStatus(LPDWORD pdwStatus) ;
    STDMETHODIMP GetCurrentPosition(LPDWORD pdwPlayCursor, LPDWORD pdwWriteCursor) ;
    STDMETHODIMP SetCurrentPosition(DWORD dwPlayCursor) ;
    STDMETHODIMP SetBufferData(LPVOID pvBufferData, DWORD dwBufferBytes) ;
    STDMETHODIMP Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) ;
    STDMETHODIMP SetFrequency(DWORD dwFrequency) ;
    STDMETHODIMP SetVolume(LONG lVolume) ;

#ifdef SILVER

    STDMETHODIMP SetChannelVolume(LPCDSCHANNELVOLUME pVolume) ;

#else // SILVER

    STDMETHODIMP SetPitch(LONG lPitch);
    STDMETHODIMP SetLFO(LPCDSLFODESC pLFODesc);
    STDMETHODIMP SetEG(LPCDSENVELOPEDESC pEnvelopeDesc);    
    STDMETHODIMP SetFilter(LPCDSFILTERDESC pFilterDesc);
    STDMETHODIMP SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer);
    STDMETHODIMP SetMixBins(DWORD dwMixBins);
    STDMETHODIMP SetMixBinVolumes(DWORD dwMixBins, const LONG *alVolumes);
    STDMETHODIMP SetHeadroom(THIS_ DWORD dwHeadroom);

#endif // SILVER

    // IDirectSound3DBuffer methods
    STDMETHODIMP SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwApply) ;
    STDMETHODIMP SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply) ;
    STDMETHODIMP SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) ;
    STDMETHODIMP SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwApply) ;
    STDMETHODIMP SetMaxDistance(FLOAT flMaxDistance, DWORD dwApply) ;
    STDMETHODIMP SetMinDistance(FLOAT flMinDistance, DWORD dwApply) ;
    STDMETHODIMP SetMode(DWORD dwMode, DWORD dwApply) ;
    STDMETHODIMP SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) ;
    STDMETHODIMP SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) ;

    // IDirectSoundNotify methods
    STDMETHODIMP SetNotificationPositions(DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies) ;
    STDMETHODIMP GetObjectInPath(REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject) ;

    CBuffer(CAudioSink *pParent,LPCDSBUFFERDESC pcBufferDesc,LPDWORD pdwFuncID, DWORD dwBusIDCount,REFGUID guidBufferID);
    ~CBuffer();
    CBuffer* GetNext() { return (CBuffer*)AListItem::GetNext();}
private:
    DWORD               DBToMultiplier(long lGain);
    void                CalcVolume();
    long                m_cRef;
    CAudioSink *        m_pParentSink;                  // Pointer to parent sink that manages the buffers.
    DWORD               m_dwFrequency;                  // Buffer playback frequency.
    long                m_lPBend;                       // Equivalent pitch bend.
    long                m_lVolume;                      // Gain, in dB.
    long                m_lPan;                         // Pan.
    DWORD               m_dwLeftVolume;                 // Left volume linear multiplier.
    DWORD               m_dwRightVolume;                // Right volume linear multiplier.
    GUID                m_guidBufferID;                 // Each mixin buffer has a unique guid.
    DWORD               m_dwChannels;                   // Number of channels in buffer itself.
    DWORD               m_dwBusCount;                   // Number of buses connected to buffer. We're maxing at 2 for now.
    DWORD               m_dwFunctionIDs[AUDIOSINK_MAX_CHANNELS];  // Function ids for each bus that feeds this buffer.
    DWORD               m_dwBusIDs[AUDIOSINK_MAX_CHANNELS];       // Equivalent bus ids.
    short *             m_pBusBuffer[AUDIOSINK_MAX_CHANNELS]; // Bus buffers to feed to synth.
    static DWORD        m_sdwNextBusID;                 // Global bus id generator.
    bool                m_fActive;
    CMemTrack           m_MemTrack;             // For debug memory tracking.
};

class CBufferList : public AList
{
public:
    void Clear();
    void AddHead(CBuffer* pBuffer) { AList::AddHead((AListItem*)pBuffer);}
    CBuffer* GetHead(){return (CBuffer*)AList::GetHead();}
    CBuffer* RemoveHead() {return (CBuffer *) AList::RemoveHead();}
    void Remove(CBuffer* pBuffer){AList::Remove((AListItem*)pBuffer);}
    void AddTail(CBuffer* pBuffer){AList::AddTail((AListItem*)pBuffer);}
    BOOL IsMember(CBuffer* pBuffer){return AList::IsMember((AListItem*)pBuffer);}
};

#define PACKETDUR   37      // duration in ms for one packet
#define PACKETSIZE  (((SAMPLERATE * PACKETDUR) + 500) / 1000)  // Size in # of samples.

#ifdef XBOX
#define PACKETCOUNT 2 // Save memory, reduce latency, on Xbox
#else
#define PACKETCOUNT 3
#endif

#define PACKETLATENCY ((PACKETCOUNT - 1) * PACKETDUR)

class CAudioSink :  public IDirectSoundSynthSink, public IDirectSoundConnect, public IReferenceClock, public IKsControl
{
friend class CBuffer;
friend class CClock;
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectSoundSynthSink methods
    STDMETHODIMP GetLatencyClock(IReferenceClock **ppClock) ;
    STDMETHODIMP Activate(BOOL fEnable) ;
    STDMETHODIMP SampleToRefTime(LONGLONG llSampleTime, REFERENCE_TIME *prtTime) ;
    STDMETHODIMP RefToSampleTime(REFERENCE_TIME rtTime, LONGLONG *pllSampleTime) ;
    STDMETHODIMP GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) ;

    // IDirectSoundConnect methods
    STDMETHODIMP AddSource(LPDIRECTSOUNDSOURCE pDSSource) ;
    STDMETHODIMP RemoveSource(LPDIRECTSOUNDSOURCE pDSSource) ;
    STDMETHODIMP SetMasterClock(IReferenceClock *pClock) ;
    STDMETHODIMP CreateSoundBuffer(LPCDSBUFFERDESC pcBufferDesc, LPDWORD pdwFuncID, DWORD dwBusIDCount,
                                           REFGUID guidBufferID, LPDIRECTSOUNDBUFFER *ppBuffer) ;
    STDMETHODIMP CreateSoundBufferFromConfig(LPUNKNOWN pConfig, LPDIRECTSOUNDBUFFER *ppBuffer) ;
    STDMETHODIMP GetSoundBuffer(DWORD dwBusID, LPDIRECTSOUNDBUFFER *ppBuffer) ;
    STDMETHODIMP GetBusCount(LPDWORD pdwCount) ;
    STDMETHODIMP GetBusIDs(LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, DWORD dwBusCount) ;
    STDMETHODIMP GetFunctionalID(DWORD dwBusID, LPDWORD pdwFuncID) ;
    STDMETHODIMP GetSoundBufferBusIDs(LPDIRECTSOUNDBUFFER pBuffer, LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, LPDWORD pdwBusCount) ;

    /* IReferenceClock methods */
    STDMETHODIMP GetTime(REFERENCE_TIME *pTime);
    STDMETHODIMP AdviseTime(REFERENCE_TIME baseTime,REFERENCE_TIME streamTime,HANDLE hEvent,DWORD *pdwAdviseCookie);
    STDMETHODIMP AdvisePeriodic(REFERENCE_TIME startTime,REFERENCE_TIME periodTime,HANDLE hSemaphore,DWORD *pdwAdviseCookie);
    STDMETHODIMP STDMETHODCALLTYPE Unadvise(DWORD dwAdviseCookie);

    /* IKsControl methods */
    STDMETHODIMP KsProperty(PKSPROPERTY Property,ULONG PropertyLength,
        LPVOID PropertyData,ULONG DataLength,ULONG* BytesReturned);
    STDMETHODIMP KsMethod(PKSMETHOD Method,ULONG MethodLength,
        LPVOID MethodData,ULONG DataLength,ULONG* BytesReturned) ;
    STDMETHODIMP KsEvent(PKSEVENT Event OPTIONAL,ULONG EventLength,
        LPVOID EventData,ULONG DataLength,ULONG* BytesReturned);

    IReferenceClock *       GetMasterClock();
    CAudioSink(WAVEFORMATEX *pwfxFormat);
    ~CAudioSink();
    void                    Render(short *pnBuffer,DWORD dwLength,LONGLONG llWritePosition, LONGLONG llPlayPosition);
    void                    RemoveBuffer(CBuffer *pBuffer);
    HRESULT                 StreamThread();
private:
    void                    ResizeBusArrays();
    void                    FillBusArrays();
#ifndef XMIX
    void                    Mix(short *pnBuffer,DWORD dwLength);
#endif

    CClock                  m_MasterClock;      // Provides clock interface for rest of system.
//    CSampleClock            m_SampleClock;
    long                    m_cRef;
	WAVEFORMATEX		    m_wfSinkFormat;     // Format for audio.
//    IReferenceClock *       m_pMasterClock;
//    IDirectSoundSinkSync *  m_pMasterClockSync; // Control back over the master clock timing.
    BOOL                    m_fActive;
    CBufferList             m_BufferList;
    DWORD *                 m_pBusIDs;          // Array of bus ids.
    DWORD *                 m_pFunctionIDs;     // Array of function ids.
    long *                  m_pPBends;          // Array of pitch bends.
    short **                m_ppBusBuffers;     // Array of bus buffers to write into.
    DWORD                   m_dwBusCount;       // Number of buses currently in use.
    IDirectSoundSource *    m_pSynthSource;     // Pointer to synth. Ultimately, we can host a list of these, but this will do for now.
	LONGLONG			    m_llAbsPlay;		// Absolute point in sample time where play head is.
	LONGLONG			    m_llAbsWrite;	    // Absolute point in sample time we've written up to.
    DWORD                   m_dwBusBufferSize;  // Size of the bus buffers, used to read from the synth.
    CRITICAL_SECTION		m_CriticalSection;	// Critical section to manage access.

    HRESULT                 InitStream();       // Initializes the output buffer and starts it streaming.
public:
    void                    DoWork(REFERENCE_TIME *prtWakeUp);           // Processes whatever needs to be processed.
private:
    void                    KillStream();       // Kills the buffer and stops output.
#ifdef XMIX
    CBuffer *               m_pDummyBuffer;     // Just to keep things running for now...
    bool                    m_bStartTimeInitialized;
    REFERENCE_TIME          m_rtStartTime; // What the AC97 Clock was when we first looked at it
#else
    short *                 m_pnBuffers[PACKETCOUNT]; // Array of buffers.
    DWORD                   m_dwStatus[PACKETCOUNT];  // Status for each buffer.
    DWORD                   m_dwCurrentBuffer;  // Index into current buffer.
    LPDIRECTSOUNDSTREAM     m_pStream;          // Pointer to stream.
#endif
    LONGLONG                m_llSampleTime;     // Position in time.
    CWorker *               m_pSinkWorker;
    HANDLE                  m_hSinkThread; 
    DWORD                   m_dwSinkThreadID;
};


#endif // __AUDIO_SINK__