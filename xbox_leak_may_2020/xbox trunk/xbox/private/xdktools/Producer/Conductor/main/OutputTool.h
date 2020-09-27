#if !defined __OUTPUTTOOL_H__
#define __OUTPUTTOOL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <dmusici.h>
//#include "fstream.h"
#include <afxtempl.h>
#include <dmusprod.h>

// One normal MIDI input port, plus 8 Echo ports
#define MIDI_IN_PORTS (9)

#define ECHO_ADVANCED_PORTS (MIDI_IN_PORTS - 1)

typedef struct
{
    IDirectMusicPort*	m_pPort;
	HANDLE				m_hPortEvent;
	long				m_lRef;
} PortContainer;

typedef struct
{
	PortContainer*		m_pPortContainer;
	GUID				m_guidPort;
	DWORD				m_dwPChannelBase;
	bool				m_fEchoMIDI;
	REFERENCE_TIME		m_rtLatencyOffset;
} MIDIInputContainer;

class CMIDIInputContainer
{
public:
	CMIDIInputContainer(IDirectMusic* pDMusic );
	~CMIDIInputContainer();

	BOOL						m_fWantToEcho;

	CTypedPtrList<CPtrList, PortContainer *> m_lstMIDIInPorts;

	// 0 is normal MIDI input, 1 - MIDI_IN_PORTS-1 are the echo MIDI input ports
	MIDIInputContainer			m_aMIDIInputContainer[MIDI_IN_PORTS];
	HANDLE						m_ahMIDIInputHandles[MIDI_IN_PORTS];
	DWORD						m_dwValidInputHandles;

	CRITICAL_SECTION			m_csEchoMIDI;
	HANDLE						m_hEchoMIDIInputExitEvent;

	HRESULT OnOutputEnabled( void );
	HRESULT OnOutputDisabled( void );

	HRESULT SetDMusicEchoMIDIIn( DWORD dwEchoID, DWORD dwNewOutputBlock, REFGUID guidEchoInputPort, bool fUpdateNow = true );

	HRESULT EnableEchoMIDI( BOOL fEnable );

	HRESULT SetPChannelThru( DWORD dwInputChannel, DWORD dwPChannel, IDirectMusicAudioPath *pDMAudiopath );
	HRESULT CancelPChannelThru( DWORD dwInputChannel );
    void UpdateMIDIThruChannels( IDirectMusicAudioPath *pDMAudiopath );
	void ClearThruConnections( void );

	void ResetLatencyOffset( void );
	void UpdateRegistry( void );
	void RestartMIDIInThreadIfNecessary( void );

	void InitializeFromRegistry( void );

protected:
	HRESULT	StartMIDIInputThreadAndCreatePorts( void );
	HRESULT	StopMIDIInputThreadAndRemovePorts( void );
	HRESULT UpdateMIDIInputThreadAndCreatePorts( void );
	PortContainer *FindPort( const REFGUID guidPort );

	bool AnyMIDIInputValid( void );

	IDirectMusic*				m_pDMusic;
	bool						m_afThruingMIDI[16];
	DWORD						m_adwThruingMIDI[16];
	BOOL						m_fMIDIInputRunning;
	HANDLE						m_hEchoMIDIInputThread;
};

class COutputTool :	public IDirectMusicTool
{
public:
	COutputTool();
	~COutputTool();

	// IUnknown methods
	virtual STDMETHODIMP QueryInterface( const IID &iid, void **ppv );
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// IDirectMusicTool methods
	HRESULT STDMETHODCALLTYPE Init(IDirectMusicGraph* pGraph);
	HRESULT STDMETHODCALLTYPE GetMsgDeliveryType(DWORD* pdwDeliveryType);
	HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize(DWORD* pdwNumElements);
	HRESULT STDMETHODCALLTYPE GetMediaTypes(DWORD** padwMediaTypes, DWORD dwNumElements);
	HRESULT STDMETHODCALLTYPE ProcessPMsg(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG);
	HRESULT STDMETHODCALLTYPE Flush(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime);

public:
	void SetPChannelMute( DWORD dwPChannel, BOOL fMute );
	BOOL GetPChannelMute( DWORD dwPChannel );
	CArray<BOOL, BOOL> m_afActivePChannels;
	CArray<long, long> m_alPChannelStates;
	CArray<long, long> m_alPChannelNoteOnStates;
	CArray<long, long> m_alNoteOnStates;
	CArray<BYTE, BYTE> m_abPChannelNoteOn;

	void MarkAllChannelsUnused( void );

protected:
	void AddNew( DWORD dwPChannel );
	CArray<BOOL, BOOL> m_afMute;
	LONG	m_cRef;
//	ofstream ofsOutput;
};

class CNotifyTool :	public IDirectMusicTool
{
public:
	CNotifyTool();
	~CNotifyTool();

	// IUnknown methods
	virtual STDMETHODIMP QueryInterface( const IID &iid, void **ppv );
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// IDirectMusicTool methods
	HRESULT STDMETHODCALLTYPE Init(IDirectMusicGraph* pGraph);
	HRESULT STDMETHODCALLTYPE GetMsgDeliveryType(DWORD* pdwDeliveryType);
	HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize(DWORD* pdwNumElements);
	HRESULT STDMETHODCALLTYPE GetMediaTypes(DWORD** padwMediaTypes, DWORD dwNumElements);
	HRESULT STDMETHODCALLTYPE ProcessPMsg(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG);
	HRESULT STDMETHODCALLTYPE Flush(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime);

protected:
	LONG	m_cRef;
};


#endif // !defined __OUTPUTTOOL_H__