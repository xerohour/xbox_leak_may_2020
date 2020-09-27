//
// dmusiccp.h
//
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//
// Private interfaces

#ifndef _DMUSICCP_DOT_H_
#define _DMUSICCP_DOT_H_

#include <dsound.h>  // For IDirectSoundWave
#ifdef XBOX
#include "xsoundp.h"
#endif // XBOX

// Interfaces/methods removed from Direct Music Core layer:

// IDirectMusicVoiceP
interface IDirectMusicVoiceP : IUnknown
{
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR *)=0; 
	virtual ULONG STDMETHODCALLTYPE AddRef()=0; 
	virtual ULONG STDMETHODCALLTYPE Release()=0; 

	// IDirectMusicVoiceP
	virtual HRESULT STDMETHODCALLTYPE Play(
         REFERENCE_TIME rtStart,                // Time to play
         LONG prPitch,                          // Initial pitch
         LONG vrVolume                          // Initial volume
        )=0;
    
	virtual HRESULT STDMETHODCALLTYPE Stop(
          REFERENCE_TIME rtStop                 // When to stop
        )=0;
};


// IDirectSoundDownloadedWaveP
interface IDirectSoundDownloadedWaveP : IUnknown
{
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR *)=0; 
	virtual ULONG STDMETHODCALLTYPE AddRef()=0; 
	virtual ULONG STDMETHODCALLTYPE Release()=0; 

	// IDirectSoundDownloadedWaveP
};

// IDirectMusicPortP
interface IDirectMusicPortP : IUnknown
{
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID FAR *)=0; 
	virtual ULONG STDMETHODCALLTYPE AddRef()=0; 
	virtual ULONG STDMETHODCALLTYPE Release()=0; 

	// IDirectMusicPortP
	virtual HRESULT STDMETHODCALLTYPE DownloadWave(
		IDirectSoundWave *pWave,                // Wave object
        IDirectSoundDownloadedWaveP **ppWave,   // Returned downloaded wave
        REFERENCE_TIME rtStartHint = 0          // Where we're likely to start
        )=0;
        
	virtual HRESULT STDMETHODCALLTYPE UnloadWave(
		IDirectSoundDownloadedWaveP *pWave      // Wave object
        )=0;
            
	virtual HRESULT STDMETHODCALLTYPE AllocVoice(
         IDirectSoundDownloadedWaveP *pWave,    // Wave to play on this voice
         DWORD dwChannel,                       // Channel and channel group
         DWORD dwChannelGroup,                  //  this voice will play on
         REFERENCE_TIME rtStart,                // Start position (stream only)
         SAMPLE_TIME stLoopStart,               // Loop start (one-shot only)
         SAMPLE_TIME stLoopEnd,                 // Loop end (one-shot only)
         IDirectMusicVoiceP **ppVoice           // Returned voice
        )=0;
        
#ifndef XBOX
	virtual HRESULT STDMETHODCALLTYPE AssignChannelToBuses(
		DWORD dwChannelGroup,                   // Channel group and
		DWORD dwChannel,                        // channel to assign
		LPDWORD pdwBuses,                       // Array of bus id's to assign
		DWORD cBusCount                         // Count of bus id's           
        )=0;
#else
	virtual HRESULT STDMETHODCALLTYPE AssignChannelToOutput(
		DWORD dwChannelGroup,                   // Channel group and
		DWORD dwChannel,                        // channel to assign
        IDirectSoundBuffer *pBuffer,            // Buffer, if assigned to buffer.
        DWORD dwMixBins,                        // Or, mixbins, if assigned to them.
        BYTE *pbControllers)=0;

#endif
        
	virtual HRESULT STDMETHODCALLTYPE SetSink(
		IDirectSoundConnect *pSinkConnect       // From IDirectSoundPrivate::AllocSink
        )=0;
        
 	virtual HRESULT STDMETHODCALLTYPE GetSink(
		IDirectSoundConnect **ppSinkConnect     // The sink in use 
        )=0;
};

interface IDirectMusicSynthX;
interface IDirectMusicSynthSink;

#undef  INTERFACE
#define INTERFACE  IDirectMusicSynthX
DECLARE_INTERFACE_(IDirectMusicSynthX, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDirectMusicSynthX */
    STDMETHOD(Init)                 (THIS_ DWORD dwVoicePoolSize) PURE;
//    STDMETHOD(Close)                (THIS) PURE;
    STDMETHOD(SetNumChannelGroups)  (THIS_ DWORD dwGroups) PURE;
	STDMETHOD(SetMasterVolume)      (THIS_ LONG lVolume)PURE;
    STDMETHOD(Download)             (THIS_ LPHANDLE phDownload, 
                                           LPVOID pvData, 
                                           LPBOOL pbFree ) PURE;
    STDMETHOD(Unload)               (THIS_ HANDLE hDownload, 
                                           HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE), 
                                           HANDLE hUserData ) PURE; 
//    STDMETHOD(PlayBuffer)           (THIS_ REFERENCE_TIME rt, 
//                                           LPBYTE pbBuffer, 
//                                           DWORD cbBuffer) PURE;
//    STDMETHOD(GetRunningStats)      (THIS_ LPDMUS_SYNTHSTATS pStats) PURE;
//    STDMETHOD(GetPortCaps)          (THIS_ LPDMUS_PORTCAPS pCaps) PURE;
    STDMETHOD(SendShortMsg)         (THIS_ REFERENCE_TIME rt, DWORD dwGroup, DWORD dwMsg)PURE;
    STDMETHOD(SendLongMsg)          (THIS_ REFERENCE_TIME rt, DWORD dwGroup, BYTE *pbMsg, DWORD dwLength)PURE;
    STDMETHOD(GetMasterClock)       (THIS_ IReferenceClock **ppClock) PURE;
//    STDMETHOD(GetLatencyClock)      (THIS_ IReferenceClock **ppClock) PURE;
//    STDMETHOD(Activate)             (THIS_ BOOL fEnable) PURE;
//    STDMETHOD(SetSynthSink)         (THIS_ IDirectMusicSynthSink *pSynthSink) PURE;
//    STDMETHOD(Render)               (THIS_ short *pBuffer, 
//                                           DWORD dwLength, 
//                                           LONGLONG llPosition) PURE;
    STDMETHOD(SetChannelPriority)   (THIS_ DWORD dwChannelGroup,
                                           DWORD dwChannel,
                                           DWORD dwPriority) PURE;
    STDMETHOD(GetChannelPriority)   (THIS_ DWORD dwChannelGroup,
                                           DWORD dwChannel,
                                           LPDWORD pdwPriority) PURE;
//    STDMETHOD(GetFormat)            (THIS_ LPWAVEFORMATEX pWaveFormatEx,
//                                           LPDWORD pdwWaveFormatExSize) PURE;
//    STDMETHOD(GetAppend)            (THIS_ DWORD* pdwAppend) PURE;
    STDMETHOD(PlayWave)            (THIS_ REFERENCE_TIME rt,
										   DWORD dwChannelGroup, 
										   DWORD dwChannel,
										   long	 prPitch,			/* PREL not defined here */
										   long  vrVolume,          /* VREL not defined here */
                                           SAMPLE_TIME stVoiceStart,
                                           SAMPLE_TIME stLoopStart,
                                           SAMPLE_TIME stLoopEnd,
                                           IDirectSoundWave *pIWave,
                                           DWORD *pdwVoiceID) PURE;

    STDMETHOD(StopWave)            (THIS_ REFERENCE_TIME rt, 
										   DWORD dwVoiceId ) PURE;

//    STDMETHOD(GetVoiceState)        (THIS_ DWORD dwVoice[], 
//										   DWORD cbVoice,
//										   DMUS_VOICE_STATE dwVoiceState[] ) PURE;
//    STDMETHOD(Refresh)              (THIS_ DWORD dwDownloadID,
//                                           DWORD dwFlags) PURE;
//    STDMETHOD(IsReadyForData)       (THIS_ DWORD dwDownloadID) PURE;
    STDMETHOD(AssignChannelToOutput) (THIS_ DWORD dwChannelGroup,                   // Channel group and
		                                    DWORD dwChannel,                        // channel to assign
                                            IDirectSoundBuffer *pBuffer,            // Buffer, if assigned to buffer.
                                            DWORD dwMixBins,
                                            BYTE *pbMixBins)PURE;
};


// GUIDs for new core layer private interfaces
DEFINE_GUID(IID_IDirectMusicSynthX, 0x827ae931, 0xe44, 0x420d, 0x95, 0x24, 0x56, 0xf4, 0x93, 0x57, 0x8, 0xa6);
DEFINE_GUID(IID_IDirectMusicVoiceP, 0x827ae928, 0xe44, 0x420d, 0x95, 0x24, 0x56, 0xf4, 0x93, 0x57, 0x8, 0xa6);
DEFINE_GUID(IID_IDirectSoundDownloadedWaveP, 0x3b527b6e, 0x5577, 0x4060, 0xb9, 0x6, 0xcd, 0x34, 0xa, 0x46, 0x71, 0x27);
DEFINE_GUID(IID_IDirectMusicPortP, 0x7048bcd8, 0x43fd, 0x4ca5, 0x93, 0x11, 0xf3, 0x24, 0x8f, 0xa, 0x25, 0x22);

// Class ID for synth sink. We pulled this from public headers since apps should never cocreate this.
DEFINE_GUID(CLSID_DirectMusicSynthSink,0xaec17ce3, 0xa514, 0x11d1, 0xaf, 0xa6, 0x0, 0xaa, 0x0, 0x24, 0xd8, 0xb6);


#endif          // _DMUSICCP_DOT_H_
