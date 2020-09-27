//      Copyright (c) 1996-2001 Microsoft Corporation
// UMSynth.h : Declaration of CUserModeSynth

#ifndef __UMSYNTH_H_
#define __UMSYNTH_H_

#include <ks.h>
#include <ksproxy.h>
#include <mmsystem.h>
#include "dsound.h"   
#include "dmusicc.h"
#include "dmusics.h"
#include "CSynth.h"
#include "synth.h"
#include "dsoundp.h"    // For IDirectSoundSource
#include "PLClock.h"

class CUserModeSynth;
typedef struct _DM_CONNECTION *PDM_CONNECTION;

// {AEE78E4D-8818-4020-AA5D-997756AF53FB}
DEFINE_GUID(CLSID_XboxSynth, 
0xaee78e4d, 0x8818, 0x4020, 0xaa, 0x5d, 0x99, 0x77, 0x56, 0xaf, 0x53, 0xfb);

// {E51F2AA9-F7B6-4397-9F70-783FC3642EF8}
DEFINE_GUID(GUID_Xbox_PROP_PCSynth, 
0xe51f2aa9, 0xf7b6, 0x4397, 0x9f, 0x70, 0x78, 0x3f, 0xc3, 0x64, 0x2e, 0xf8);

// {207348EF-09E7-425c-BBBA-94C92FEF09B7}
DEFINE_GUID(GUID_Xbox_PROP_XboxSynth, 
0x207348ef, 0x9e7, 0x425c, 0xbb, 0xba, 0x94, 0xc9, 0x2f, 0xef, 0x9, 0xb7);

DEFINE_GUID(GUID_Xbox_PROP_XboxAudioPath, 
0xa70f376a, 0x49f1, 0x4339, 0xbb, 0x93, 0xb2, 0x9b, 0x67, 0xee, 0xf5, 0x88);


typedef HRESULT (CUserModeSynth::*GENPROPHANDLER)(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG cbBuffer);

#define GENPROP_F_STATIC                0x00000000
#define GENPROP_F_FNHANDLER             0x00000001


#include <pshpack4.h>
struct GENERICPROPERTY
{
    const GUID *pguidPropertySet;       // What property set?
    ULONG       ulId;                   // What item?

    ULONG       ulSupported;            // Get/Set flags for QuerySupported

    ULONG       ulFlags;                // GENPROP_F_xxx

    LPVOID      pPropertyData;          // Data to be returned
    ULONG       cbPropertyData;         // and its size    

    GENPROPHANDLER pfnHandler;          // Handler fn iff GENPROP_F_FNHANDLER
};
#include <poppack.h>

class CDownloadItem : public CListItem
{
public:
	HANDLE	m_hItem;
	DWORD	m_dwID;
    CDownloadItem *GetNext() const {return (CDownloadItem *)CListItem::GetNext();};
};

/////////////////////////////////////////////////////////////////////////////
// CXboxLatencyClock

class CXboxLatencyClock: public IReferenceClock
{
public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

    // IReferenceClock methods
    STDMETHODIMP GetTime( REFERENCE_TIME *pTime);
    STDMETHODIMP AdviseTime( REFERENCE_TIME baseTime, REFERENCE_TIME streamTime, HANDLE hEvent, DWORD *pdwAdviseCookie);
    STDMETHODIMP AdvisePeriodic( REFERENCE_TIME startTime, REFERENCE_TIME periodTime, HANDLE hSemaphore, DWORD *pdwAdviseCookie);
    STDMETHODIMP Unadvise( DWORD dwAdviseCookie);

public:
	CXboxLatencyClock();
	~CXboxLatencyClock();
	void SetXboxLatency( REFERENCE_TIME rtLatency );
	void SetSinkLatencyClock( IReferenceClock *pIReferenceClock );
	void SetMasterClock( IReferenceClock *pIReferenceClock );
	REFERENCE_TIME GetXboxLatency( void ) { return m_rtXboxLatency; }

private:
	long			m_cRef;
	IReferenceClock	*m_pSinkLatencyClock;
	IReferenceClock	*m_pMasterClock;
	REFERENCE_TIME	m_rtXboxLatency;
};

#define XBOX_APATH_SHARED_STEREOPLUSREVERB   0xFFFF0002

/////////////////////////////////////////////////////////////////////////////
// CUserModeSynth

class CUserModeSynth : 
#ifdef IMPLEMENT_SYNTH8
public IDirectMusicSynth8,
#else
public IDirectMusicSynth,
#endif
public IDirectSoundSource, public IKsControl
{

public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

// IDirectMusicSynth
public:
	virtual STDMETHODIMP Open(LPDMUS_PORTPARAMS pPortParams);
	virtual STDMETHODIMP Close(); 
	virtual STDMETHODIMP SetNumChannelGroups(DWORD dwGroups);
	virtual STDMETHODIMP Download(LPHANDLE phDownload, LPVOID pvData, LPBOOL pbFree);
	virtual STDMETHODIMP Unload(HANDLE hDownload,
								HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE),
								HANDLE hUserData);
	virtual STDMETHODIMP PlayBuffer(REFERENCE_TIME rt, LPBYTE pbBuffer, DWORD cbBuffer);
	virtual STDMETHODIMP GetRunningStats(LPDMUS_SYNTHSTATS pStats);
	virtual STDMETHODIMP GetPortCaps(LPDMUS_PORTCAPS pCaps);
	virtual STDMETHODIMP SetMasterClock(IReferenceClock *pClock);
	virtual STDMETHODIMP GetLatencyClock(IReferenceClock **ppClock);
	virtual STDMETHODIMP Activate(BOOL fEnable);
	virtual STDMETHODIMP SetSynthSink(IDirectMusicSynthSink *pSynthSink);
	virtual STDMETHODIMP Render(short *pBuffer, DWORD dwLength, LONGLONG llPosition);
    virtual STDMETHODIMP SetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, DWORD dwPriority);
    virtual STDMETHODIMP GetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, LPDWORD pdwPriority);
    virtual STDMETHODIMP GetFormat(LPWAVEFORMATEX pWaveFormatEx, LPDWORD pdwWaveFormatExSize);
    virtual STDMETHODIMP GetAppend(DWORD* pdwAppend);

#ifdef IMPLEMENT_SYNTH8
// IDirectMusicSynth8
public:
	virtual STDMETHODIMP PlayVoice(REFERENCE_TIME rt, DWORD dwVoiceId, DWORD dwChannelGroup, DWORD dwChannel, DWORD dwDLId, PREL prPitch, VREL vrVolume, SAMPLE_TIME stVoiceStart, SAMPLE_TIME stLoopStart, SAMPLE_TIME stLoopEnd );
    virtual STDMETHODIMP StopVoice(REFERENCE_TIME rt, DWORD dwVoiceId );
    virtual STDMETHODIMP GetVoiceState(DWORD dwVoice[], DWORD cbVoice, DMUS_VOICE_STATE VoiceState[] );
    virtual STDMETHODIMP Refresh(DWORD dwDownloadID, DWORD dwFlags );
    virtual STDMETHODIMP AssignChannelToBuses(DWORD dwChannelGroup, DWORD dwChannel, LPDWORD pdwBuses, DWORD cBuses );
#endif

// IDirectSoundSource
public:
    virtual STDMETHODIMP GetFormat(LPWAVEFORMATEX pWaveFormatEx, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
    virtual STDMETHODIMP SetSink(IDirectSoundConnect* pSinkConnect);
    virtual STDMETHODIMP Seek(ULONGLONG sp);
    virtual STDMETHODIMP Read(LPVOID *ppvBuffer, LPDWORD pdwIDs, LPDWORD pdwFuncIDs, LPLONG plPitchBends, DWORD cpvBuffer, PULONGLONG pcb);
	virtual STDMETHODIMP GetSize(PULONGLONG pcb);

// IDirectSoundSource : members 
private:
	ULONGLONG m_ullPosition;

// IKsControl
public:
 	virtual STDMETHODIMP KsProperty(
		PKSPROPERTY pPropertyIn, 
		ULONG ulPropertyLength,
		LPVOID pvPropertyData, 
		ULONG ulDataLength,
		PULONG pulBytesReturned);

    virtual STDMETHODIMP KsMethod(
        IN PKSMETHOD Method,
        IN ULONG MethodLength,
        IN OUT LPVOID MethodData,
        IN ULONG DataLength,
        OUT PULONG BytesReturned
    );

    virtual STDMETHODIMP KsEvent(
        IN PKSEVENT Event,
        IN ULONG EventLength,
        IN OUT LPVOID EventData,
        IN ULONG DataLength,
        OUT PULONG BytesReturned
    );
    
	CUserModeSynth();
	~CUserModeSynth();
	HRESULT		Init();

private:
	long		m_cRef;
	IDirectMusicSynthSink *m_pSynthSink;
	IDirectSoundSynthSink *m_pSynthSink8;
	CSynth *	m_pSynth;		// Just one synth engine.
	BOOL		m_fActive;		// Should we be active?
	BOOL		m_fPCActive;	// PC Synth currently active.
	BOOL		m_fXboxActive;	// Xbox Synth currently active.
	BOOL		m_fPCSynth;		// Enable the PC Synth
	BOOL		m_fXboxSynth;	// Enable the Xbox Synth
	DWORD		m_dwSampleRate;	// Synths sample rate 
	DWORD		m_dwChannels;	// number of ouput channels 
    DWORD       m_dwXboxAudioPath; // Default audiopath on xbox
	DWORD 		m_dwBufferFlags;// flags the type of output buffer dssink or interleaved or plain mono
    LONG        m_lVolume;      // in 1/100 dB
    LONG        m_lBoost;       // in 1/100 dB
    LONG        m_lGainAdjust;  // in 1/100 dB
    CRITICAL_SECTION m_CriticalSection; // Critical section to manage access.
    CRITICAL_SECTION m_XboxCriticalSection; // Critical section to manage access to the Xbox
    BOOL        m_fCSInitialized;
	PDM_CONNECTION m_pdmConnection;
	IReferenceClock *m_pMasterClock;
	CPhaseLockClock m_PLCXbox;
	CXboxLatencyClock m_XboxLatencyClock;

    HRESULT HandleSetVolume(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    HRESULT HandleSetBoost(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
	HRESULT HandleReverb(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
	HRESULT HandleEffects(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    HRESULT HandleGetSampleRate(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    HRESULT HandlePCSynth(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    HRESULT HandleXboxSynth(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    HRESULT HandleXboxPath(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
	HRESULT HandleLatency(ULONG ulId, BOOL fSet, LPVOID pbBuffer, PULONG pcbBuffer);
    static GENERICPROPERTY m_aProperty[];
    static const int m_nProperty;
    //static GENERICPROPERTY *FindPropertyItem(REFGUID rguid, ULONG ulId);
    GENERICPROPERTY *FindPropertyItem(REFGUID rguid, ULONG ulId);

	HRESULT ActivateXboxSynth(BOOL fEnable);
	HRESULT ActivatePCSynth(BOOL fEnable);
	CList	m_lstDownloadItems;
};

// Class factory
//
// Common to emulation/WDM.
// 
class CDirectMusicSynthFactory : public IClassFactory
{
public:
	// IUnknown
    //
	virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// Interface IClassFactory
    //
	virtual STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv);
	virtual STDMETHODIMP LockServer(BOOL bLock); 

	// Constructor
    //
	CDirectMusicSynthFactory();

	// Destructor
	~CDirectMusicSynthFactory();

private:
	long m_cRef;
};



#endif //__UMSYNTH_H_
