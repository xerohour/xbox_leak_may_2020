//      Copyright (c) 1996-1999 Microsoft Corporation
//	DSLink.h

#ifndef __DS_LINK__
#define __DS_LINK__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "directks.h"
#include <math.h>
#include <mmsystem.h>
#include <dsoundp.h>
#include "dmusicc.h"
#include "dmusics.h"
#include "cclock.h"
#include "PLClock.h"
#include "alist.h"

//#define USE_STREAMING_BUFFERS 1
//#define USE_PINGPONG_BUFFERS 1
#define USE_LOOPING_BUFFERS 1

#ifdef USE_STREAMING_BUFFERS
#define DSBUFFER_LENGTH_MS 100
#define NUM_FRAMES 6
#endif //USE_STREAMING_BUFFERS

#ifdef USE_PINGPONG_BUFFERS
#define DSBUFFER_LENGTH_MS 1000
#define NUM_FRAMES 2
#endif //USE_PINGPONG_BUFFERS

#ifdef USE_LOOPING_BUFFERS
#define DSBUFFER_LENGTH_MS 1000
#define NUM_FRAMES 1
#endif //USE_LOOPING_BUFFERS

class CAudioSink;
class CBuffer;

class CDSLink 
{
public:
    // IUnknown
    //
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

public:
						CDSLink();
						~CDSLink();
	BOOL				OpenUp(CAudioSink *pSink);
    void				CloseDown();
    void                SetWaveFormat(WAVEFORMATEX *pWaveFormat);
    ULONGLONG			GetAbsPlaySample(void);
    ULONGLONG			GetAbsWriteSample(void) {return ByteToSample(m_llAbsWrite);}
private:
	static DWORD WINAPI SynthThread (LPVOID lpThreadParameter);
	void				SynthProc();
	void				Clear();
	void				Activate();
	void				Deactivate();			
	void				ReadBuffer();
	BOOL				m_fPleaseDie;		// Triggers exit.
    DWORD               m_dwResolution;     // Synth thread timeout (ms)
    BOOL                m_fOpened;
    HANDLE				m_hThread;          // Handle for synth thread.
    DWORD				m_dwThread;         // ID for thread.
    HANDLE				m_hEvent;           // Used to signal thread.

    CAudioSink *        m_pAudioSink;       // Pointer to parent sink.
	long				m_cRef;
	WAVEFORMATEX		m_wfSynth;		// Waveform requested by synth.

	CPCMAudioPin *		m_pPCMAudioPin;		// PCM Audio pin

    CRITICAL_SECTION	m_CriticalSection;	// Critical section to manage access.
	ULONGLONG			m_llAbsPlay;		// Absolute point where play head is.
	ULONGLONG			m_llAbsWrite;	    // Absolute point we've written up to.
#ifdef USE_LOOPING_BUFFERS
	DWORD				m_dwLastPlay;		// Last point where play head is.
#endif
	DWORD				m_dwWriteTo;		// Distance between write head and where we are writing.
	DWORD               m_dwWriteFromMax;   // Max distance observed between play and write head.
	BOOL				m_fActive;			// Currently active.
	DWORD				m_dwFrame;			// Current frame in use
	BYTE				*m_apbData[NUM_FRAMES];// Array of pointers to the buffers

	HRESULT				Connect();
	HRESULT				Disconnect();

    // helpers
    ULONGLONG SampleToByte(ULONGLONG llSamples) {return llSamples << m_wfSynth.nChannels;}   // REVIEW: dwSamples * m_wfSynth.nBlockAlign
    DWORD SampleToByte(DWORD dwSamples) {return dwSamples << m_wfSynth.nChannels;}   // REVIEW: dwSamples * m_wfSynth.nBlockAlign
    ULONGLONG ByteToSample(ULONGLONG llBytes)   {return llBytes >> m_wfSynth.nChannels;}     // REVIEW: dwBytes / m_wfSynth.nBlockAlign
    DWORD ByteToSample(DWORD dwBytes)   {return dwBytes >> m_wfSynth.nChannels;}     // REVIEW: dwBytes / m_wfSynth.nBlockAlign
    ULONGLONG SampleAlign(ULONGLONG llBytes)    {return SampleToByte(ByteToSample(llBytes));}
    DWORD SampleAlign(DWORD dwBytes)    {return SampleToByte(ByteToSample(dwBytes));}
    
};

#ifdef _DEBUG
class LogClass : public LogStub
{
public:
    virtual int      _cdecl Log( UINT nLevel, LPSTR pszFormat, ...);
    virtual int      _cdecl LogEx( DWORD dwType, UINT nLevel, LPSTR pszFormat, ...);
    virtual int      _cdecl LogSummary( UINT nLevel, LPSTR pszFormat, ...);
    virtual int      _cdecl LogStatusBar( LPSTR pszFormat, ...);
    virtual void     LogStdErrorString(DWORD dwErrorCode);
};
#endif

#endif // __DS_LINK__
