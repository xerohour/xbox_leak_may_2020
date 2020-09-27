//      Copyright (c) 1996-1999 Microsoft Corporation
//	    audiosink.h

#ifndef __AUDIO_SINK__
#define __AUDIO_SINK__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <math.h>
#include <mmsystem.h>
#include "dmusicc.h"
#include "dmusics.h"
#include "cclock.h"
#include "PLClock.h"
#include "dslink.h"
#include "dsoundp.h"
#include "dmksctrl.h"

#define DEFAULT_PHONEY_DS_LATENCY 20

#ifdef _DEBUG
#define IS_VALID_READ_PTR(a, b)         !IsBadReadPtr(a, b)
#define IS_VALID_WRITE_PTR(a, b)        !IsBadWritePtr(a, b)
#else // _DEBUG
#define IS_VALID_READ_PTR(a, b)         ((a) || (!(b)))
#define IS_VALID_WRITE_PTR(a, b)        ((a) || (!(b)))
#endif // _DEBUG

DEFINE_GUID(IID_CBuffer, 0xf1e13d57, 0x7c29, 0x4ae6, 0x8d, 0x77, 0x94, 0x32, 0xb6, 0x51, 0xf3, 0xa6);

class CAudioSink;
class CEffectChain;

#define AUDIOSINK_MAX_CHANNELS  2           // Max number of buses connected to buffer. We're limiting to 2 for now.
#define AUDIOSINK_NULL_BUS_ID   0xFFFFFFFF  // NULL bus ID.
#define AUDIOSINK_BUS_SIZE      30          // Bus size, in milliseconds

class CBuffer : public IDirectSoundBuffer8, public AListItem
{
friend class CAudioSink;
friend class CSendEffect;
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectSoundBuffer methods
    STDMETHODIMP GetCaps(LPDSBCAPS pBufferCaps) ;
    STDMETHODIMP GetCurrentPosition(LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) ;
    STDMETHODIMP GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) ;
    STDMETHODIMP GetVolume(LPLONG plVolume) ;
    STDMETHODIMP GetPan(LPLONG plPan) ;
    STDMETHODIMP GetFrequency(LPDWORD pdwFrequency) ;
    STDMETHODIMP GetStatus(LPDWORD pdwStatus) ;
    STDMETHODIMP Initialize(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcBufferDesc) ;
    STDMETHODIMP Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
                                           LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) ;
    STDMETHODIMP Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) ;
    STDMETHODIMP SetCurrentPosition(DWORD dwNewPosition) ;
    STDMETHODIMP SetFormat(LPCWAVEFORMATEX pcfxFormat) ;
    STDMETHODIMP SetVolume(LONG lVolume) ;
    STDMETHODIMP SetPan(LONG lPan) ;
    STDMETHODIMP SetFrequency(DWORD dwFrequency) ;
    STDMETHODIMP Stop() ;
    STDMETHODIMP Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) ;
    STDMETHODIMP Restore() ;

    // IDirectSoundBuffer8 methods
    STDMETHODIMP SetFX(DWORD dwEffectsCount, LPDSEFFECTDESC pDSFXDesc, LPDWORD pdwResultCodes) ;
    STDMETHODIMP AcquireResources(DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes) ;
    STDMETHODIMP GetObjectInPath(REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject) ;

    CBuffer(CAudioSink *pParent,LPCDSBUFFERDESC pcBufferDesc,LPDWORD pdwFuncID, DWORD dwBusIDCount,REFGUID guidBufferID);
    CBuffer(CAudioSink *pParent,IUnknown *punkBufferConfig);
    ~CBuffer();
    CBuffer* GetNext() { return (CBuffer*)AListItem::GetNext();}
	WAVEFORMATEX* Format( void );
	DWORD GetBufferType( void ) { return m_dwType; }
	bool IsPlaying( void ) { return true; }
    HRESULT FindSendLoop(CBuffer*);
    CAudioSink *        m_pParentSink;                  // Pointer to parent sink that manages the buffers.
private:
    DWORD               DBToMultiplier(long lGain);
    void                CalcVolume();
    long                m_cRef;
	DWORD				m_dwType;						// Buffer flags DSBCAPS_*
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
	short *				m_pExtraBuffer;					// Buffer for doing effects or mixin
    static DWORD        m_sdwNextBusID;                 // Global bus id generator.
	WAVEFORMATEX		m_wfBufferFormat;				// Format for audio.
	CEffectChain *		m_fxChain;
    CRITICAL_SECTION	m_CriticalSection;				// Critical section to manage access.

	void Render( long *plBuffer, DWORD dwLength, REFERENCE_TIME rtTime ); // Render the buffer into a temporary 32-bit buffer
	void PreRender( DWORD dwLength ); // Do any pre-render setup
	void RenderNoFX( long *plBuffer, DWORD dwLength ); // Render the buffer into a temporary 32-bit buffer
	void RenderFX( long *plBuffer, DWORD dwLength, REFERENCE_TIME rtTime ); // Render the buffer into a temporary 32-bit buffer
	void RenderFXFinalMix( long *plBuffer, DWORD dwLength ); // Render the buffer into a temporary 32-bit buffer
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

class CAudioSink :  public IDirectSoundSynthSink, public IDirectSoundConnect, public IReferenceClock, public IKsControl
{
friend class CBuffer;
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

    CAudioSink(WAVEFORMATEX *pwfxFormat);
    ~CAudioSink();
    void                    Render(short *pnBuffer,DWORD dwLength,LONGLONG llWritePosition, LONGLONG llPlayPosition);
    void                    RemoveBuffer(CBuffer *pBuffer);
	void					AddBuffer(CBuffer *pBuffer);
	CBuffer *				FindBufferFromGUID( REFGUID rguidBuffer );
    CDSLink                 m_DSLink;
private:
    void                    ResizeBusArrays();
    void                    FillBusArrays();
    void                    Mix(short *pnBuffer,DWORD dwLength, REFERENCE_TIME rtTime);

    CSampleClock            m_SampleClock;
    long                    m_cRef;
	WAVEFORMATEX		    m_wfSinkFormat;     // Format for audio.
    IReferenceClock *       m_pMasterClock;
    IDirectSoundSinkSync *  m_pMasterClockSync; // Control back over the master clock timing.
    BOOL                    m_fActive;
    CBufferList             m_BufferList;
    DWORD *                 m_pBusIDs;          // Array of bus ids.
    DWORD *                 m_pFunctionIDs;     // Array of function ids.
    long *                  m_pPBends;          // Array of pitch bends.
    short **                m_ppBusBuffers;     // Array of bus buffers to write into.
	long *					m_plTmpMixBuffer;	// 32-bit stereo array to mix into
    DWORD                   m_dwBusCount;       // Number of buses currently in use.
    IDirectSoundSource *    m_pSynthSource;     // Pointer to synth. Ultimately, we can host a list of these, but this will do for now.
    DWORD                   m_dwBusBufferSize;  // Size of the bus buffers, used to read from the synth.
	bool					m_fStartSampleClock;// Flag to start the sample clock
    CRITICAL_SECTION		m_CriticalSection;	// Critical section to manage access.
    CRITICAL_SECTION		m_csClock;			// Critical section to manage access to clock data
};


#endif // __AUDIO_SINK__