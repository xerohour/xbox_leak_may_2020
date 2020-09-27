//      Copyright (c) 1996-1999 Microsoft Corporation
//
//      CSynth.h
//

#ifndef __CSYNTH_H__
#define __CSYNTH_H__

#include "synth.h"
#include "..\shared\dowork.h"
#include "..\shared\dmusiccp.h" // For class ids.
#include "..\shared\debug.h" // For class ids.

extern "C" ULONG _cdecl DbgPrint(PCH Format, ...);

#define MAX_CHANNEL_GROUPS	1000
#define MAX_VOICES			1000

class CClock : public IReferenceClock
{
friend class CSynth;
public:

    CClock();

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    /* IReferenceClock methods */
    HRESULT STDMETHODCALLTYPE GetTime( 
        /* [out] */ REFERENCE_TIME __RPC_FAR *pTime);
    
    HRESULT STDMETHODCALLTYPE AdviseTime( 
        /* [in] */ REFERENCE_TIME baseTime,
        /* [in] */ REFERENCE_TIME streamTime,
        /* [in] */ HANDLE hEvent,
        /* [out] */ DWORD __RPC_FAR *pdwAdviseCookie);
    
    HRESULT STDMETHODCALLTYPE AdvisePeriodic( 
        /* [in] */ REFERENCE_TIME startTime,
        /* [in] */ REFERENCE_TIME periodTime,
        /* [in] */ HANDLE hSemaphore,
        /* [out] */ DWORD __RPC_FAR *pdwAdviseCookie);
    
    HRESULT STDMETHODCALLTYPE Unadvise( 
        /* [in] */ DWORD dwAdviseCookie);
private:
    CSynth *    	m_pParent;      // Pointer to parent structure.
    DWORD           m_dwLastPosition;
    LONGLONG        m_llSampleTime;
};



class CSynth : public CMemTrack, public IDirectMusicSynthX
{
friend class CClock;
friend class CControlLogic;
friend class CPart;
public:	
public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

// IDirectMusicSynthX
public:
	virtual STDMETHODIMP Init(DWORD dwVoicePoolSize);
	virtual STDMETHODIMP SetNumChannelGroups(DWORD dwGroups);
    virtual STDMETHODIMP SetMasterVolume(LONG lVolume);
    virtual STDMETHODIMP SendShortMsg(REFERENCE_TIME rt, DWORD dwGroup, DWORD dwMsg);
    virtual STDMETHODIMP SendLongMsg(REFERENCE_TIME rt, DWORD dwGroup, BYTE *pbMsg, DWORD dwLength);
	virtual STDMETHODIMP GetMasterClock(IReferenceClock **ppClock);
    virtual STDMETHODIMP SetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, DWORD dwPriority);
    virtual STDMETHODIMP GetChannelPriority(DWORD dwChannelGroup, DWORD dwChannel, LPDWORD pdwPriority);
	virtual STDMETHODIMP PlayWave(REFERENCE_TIME rt, DWORD dwChannelGroup, DWORD dwChannel, PREL prPitch, VREL vrVolume, SAMPLE_TIME stVoiceStart, SAMPLE_TIME stLoopStart, SAMPLE_TIME stLoopEnd, IDirectSoundWave *pIWave, DWORD *pdwVoiceID );
    virtual STDMETHODIMP StopWave(REFERENCE_TIME rt, DWORD dwVoiceId );
    virtual STDMETHODIMP AssignChannelToOutput(DWORD dwChannelGroup, DWORD dwChannel, IDirectSoundBuffer *pBuffer,DWORD dwMixBins, BYTE *pbControllers );
    virtual STDMETHODIMP Download(LPHANDLE phDownload, void *pdwData, LPBOOL bpFree);
    virtual STDMETHODIMP Unload(HANDLE hDownload,HRESULT ( CALLBACK *lpFreeMemory)(HANDLE,HANDLE),HANDLE hUserData);

    CSynth();
					~CSynth();
	HRESULT			Initialize();
	HRESULT			Activate();
	HRESULT			Deactivate();
    void            SetGainAdjust(VREL vrGainAdjust);
	HRESULT			Close();
    HRESULT			AllNotesOff();
    HRESULT			SetMaxVoices(short nMaxVoices,short nTempVoices);
public:
    void            DoWork(REFERENCE_TIME *prtWakeUp);
private:
    void			StealNotes(REFERENCE_TIME rtTime);
	short			ChangeVoiceCount(CVoiceList *pList,short nOld,short nCount);

private:
    CWorker *       m_pSynthWorker;
    CVoice *        OldestVoice();
    void            QueueVoice(CVoice *pVoice);
    CVoice *        StealVoice(DWORD dwPriority);
    STIME           m_stLastTime;       // Sample time of last mix.
    REFERENCE_TIME  m_rtLastTime;       // Reference time of last DoWork().
    CVoiceList      m_VoicesFree;       // List of available voices.
    CVoiceList      m_VoicesExtra;      // Extra voices for temporary overload.
    CVoiceList      m_VoicesInUse;      // List of voices currently in use.
    short           m_nMaxVoices;       // Number of allowed voices.
    short           m_nExtraVoices;      // Number of voices over the limit that can be used in a pinch.

public:
  
    LPDIRECTSOUNDSEQUENCER GetSequencer() { return m_dssequencer; } // For CVoice
private:
    LPDIRECTSOUNDSEQUENCER m_dssequencer;  // The direct sound sequencer

public:	
    VREL            m_vrGainAdjust;     // Final output gain adjust
	DWORD           m_dwSampleRate;		// Sample rate 
    CInstManager    m_Instruments;      // Instrument manager.
	CControlLogic **m_ppControl;		// Array of open ControlLogics.
	DWORD			m_dwControlCount;	// # of open CLs.
    
    CRITICAL_SECTION m_CriticalSection; // Critical section to manage access.
    BOOL             m_fCSInitialized;

    // Stuff brought over from UMSynth:
    CClock          m_MasterClock;  // Master clock for timing, derived from DSound.
	long		    m_cRef;
    LONG            m_lVolume;      // in 1/100 dB
    LONG            m_lBoost;       // in 1/100 dB
    LONG            m_lGainAdjust;  // in 1/100 dB

    // For measuring performance
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DWORD           m_dwDLSVoices;
    DWORD           m_dwWaveVoices;
    DWORD           m_dwStreamVoices;
    DWORD           m_dwFreeVoices;
#endif
};

#endif// __CSYNTH_H__

