// CConductor.cpp : Implementation of CConductorApp and DLL registration.
// @doc Conductor

#include "stdafx.h"
#include <mmsystem.h>
#include "dsoundp.h"
#include "CConduct.h"
#include "phoneyds.h"
#include <atlwin.cpp>
#include <mmsystem.h>
#include <oleauto.h>
#include <dmksctrl.h>
#include <dsound.h>
#include "OutputTool.h"
#include "StatusToolbar.h"
#include "Toolbar.h"
#include "SecondaryToolbar.h"
#include "TREntry.h"
#include <RiffStrm.h>
#include "OptionsToolbar.h"
#include "SynthStatusToolbar.h"
#include "WaveRecordToolbar.h"
#include "MIDISave.h"
#include "AudiopathDesigner.h"
#include "..\WaveSaveDmo\testdmo.h"
#include "DMOInfoProxy.h"
#include "detours.h"
#include "dsbufcfg.h"
#ifdef DMP_XBOX
#include "xboxdbg.h"
#include "..\shared\xguids.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Defined in notify.cpp
extern void AddToolsAndSetupWaveSaveForSegState( IUnknown *punk );
extern bool AudiopathHasWaveSaveDMO( IDirectMusicAudioPath *pIDirectMusicAudioPath );

// Defined in dslink.cpp
extern bool g_fKslInitialized;
extern TCHAR g_szPhoneyDSoundFilterName[MAX_PATH];
extern DWORD g_dwLatency;

extern IDirectMusicPort *g_rpLastSampledPort;

static BOOL g_fUsePhoneyDSound = FALSE;

//#define CHECK_FOR_EXTRA_PORT_REFS

#define DEFAULT_NUM_PCHANNELS 128

// Detours stuff ULONG_PTR
extern "C" {
#define FUNCPTR_CoCreate HRESULT (STDAPICALLTYPE *)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter, IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv)


HRESULT (STDAPICALLTYPE *DynamicTrampoline)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = NULL;
}

STDAPI DynamicDetour(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	if( rclsid == CLSID_DirectSoundBufferConfig )
	{
#ifdef DMP_XBOX
        if (g_fUsePhoneyDSound)
#endif
        {
		    CDirectSoundBufferConfig *pCDirectSoundBufferConfig = new CDirectSoundBufferConfig;
		    pCDirectSoundBufferConfig->QueryInterface( riid, ppv );
		    pCDirectSoundBufferConfig->Release();
		    return S_OK;
        }
	}
    CLSID clsid = rclsid;
#ifdef DMP_XBOX
    // These bogus classids force the use of our alternate xdmime.dll.
    if (clsid == CLSID_DirectMusicSegmentState)
    {
        clsid = CLSID_XDirectMusicSegmentState;
    }
    else if (clsid == CLSID_DirectMusicPerformance)
    {
        clsid = CLSID_XDirectMusicPerformance;
    }
    else if (clsid == CLSID_DirectMusicSegment)
    {
        clsid = CLSID_XDirectMusicSegment;
    }
    else if (clsid == CLSID_DirectMusicSong)
    {
        clsid = CLSID_XDirectMusicSong;
    }
    else if (clsid == CLSID_DirectMusicAudioPathConfig)
    {
        clsid = CLSID_XDirectMusicAudioPathConfig;
    }
    else if (clsid == CLSID_DirectMusicSeqTrack)
    {
        clsid = CLSID_XDirectMusicSeqTrack;
    }
    else if (clsid == CLSID_DirectMusicGraph)
    {
        clsid = CLSID_XDirectMusicGraph;
    }
    else if (clsid == CLSID_DirectMusicSysExTrack)
    {
        clsid = CLSID_XDirectMusicSysExTrack;
    }
    else if (clsid == CLSID_DirectMusicTempoTrack)
    {
        clsid = CLSID_XDirectMusicTempoTrack;
    }
    else if (clsid == CLSID_DirectMusicTimeSigTrack)
    {
        clsid = CLSID_XDirectMusicTimeSigTrack;
    }
    else if (clsid == CLSID_DirectMusicMarkerTrack)
    {
        clsid = CLSID_XDirectMusicMarkerTrack;
    }
    else if (clsid == CLSID_DirectMusicWaveTrack)
    {
        clsid = CLSID_XDirectMusicWaveTrack;
    }
    else if (clsid == CLSID_DirectMusicSegmentTriggerTrack)
    {
        clsid = CLSID_XDirectMusicSegmentTriggerTrack;
    }
    else if (clsid == CLSID_DirectMusicLyricsTrack)
    {
        clsid = CLSID_XDirectMusicLyricsTrack;
    }
    else if (clsid == CLSID_DirectMusicParamControlTrack)
    {
        clsid = CLSID_XDirectMusicParamControlTrack;
    }
#endif
    return DynamicTrampoline( clsid, pUnkOuter, dwClsContext, riid, ppv );
}

/////////////////////////////////////////////////////////////////////////////
//
// @interface IDMUSProdConductor | Coordinates global access to the DirectMusic Engine for other DirectMusic Producer components.
//
// @base public | IUnknown
//
// @meth HRESULT | GetPerformanceEngine | Returns an AddRef()'d reference to the
//		IDirectMusicPerformance interface.
// @meth HRESULT | GetPort | Returns an AddRef()'d pointer to the first active
//		port that supports DLS.
// @meth HRESULT | RegisterTransport | Adds a transport to the list of items in
//		the Transport Control toolbar's combo box.  
// @meth HRESULT | UnRegisterTransport | Removes a transport from the list of items
//		in the Transport Control toolbar's combo box.
// @meth HRESULT | SetActiveTransport | Sets the owner of the Transport Control toolbar's
//		controls to a specified transport.
// @meth HRESULT | IsTransportPlaying | Helps determine whether or not music is playing.
// @meth HRESULT | PlayMIDIEvent | Plays a MIDI event.
// @meth HRESULT | SetBtnStates | Sets the button states of a transport.
// @meth HRESULT | SetTempo | Sets the tempo displayed in the Transport Control toolbar.
// @meth HRESULT | RegisterNotify | Registers an interface to receive a specific notification
//		from the performance engine.
// @meth HRESULT | UnregisterNotify | Stops a specific notification from being sent to an
//		interface.
// @meth HRESULT | SetTransportName | Updates the name of a transport that is displayed in
//		the Transport Control toolbar's combo box.
// @meth HRESULT | TransportStopped | Notifies Conductor that a transport has stopped
//		playing.
// @meth HRESULT | GetTransitionOptions | Returns a structure containing the transition options
//		set for the Transport Control toolbar's Transition button.
// @meth HRESULT | RegisterSecondaryTransport | Adds a secondary transport to the list of items in
//		the Secondary Segment toolbar's combo boxes.
// @meth HRESULT | UnRegisterSecondaryTransport | Removes a secondary transport from the list of items in
//		the Secondary Segment toolbar's combo boxes.
// @meth HRESULT | SetSecondaryTransportName | Update the name of a secondary transport that is
//		displayed in the Secondary Segment toolbar's combo boxes.
// @meth HRESULT | IsSecondaryTransportPlaying | Helps determine whether or not a secondary segment
//		is playing.
// @meth HRESULT | SetPChannelThru | Starts a thru connection from a specified MIDI input channel to a
//		specified output PChannel.
// @meth HRESULT | CancelPChannelThru | Stops the thru connection from a specified MIDI input channel.
// @meth HRESULT | IsTrackCursorEnabled | Queries the state of the Transport Options toolbar's Track Cursor button.
//
// @xref  <i IDMUSProdTransport>, <i IDMUSProdSecondaryTransport>, <i IDMUSProdNotifyCPt>
//
/////////////////////////////////////////////////////////////////////////////
//
// @enum ButtonStateFlags | State flags for the toolbar buttons
//
//      @emem BS_NO_AUTO_UPDATE | Don't automatigically update the transport buttons from the state
//			of the DirectMusic Performance engine.
//      @emem BS_PLAY_ENABLED | Enable the play button
//      @emem BS_PLAY_CHECKED | Depress the play button
//      @emem BS_STOP_ENABLED | Enable the stop button
//      @emem BS_STOP_CHECKED | Depress the stop button
//      @emem BS_REC_ENABLED | Enable the record button
//      @emem BS_REC_CHECKED | Depress the record button
//      @emem BS_TRANS_ENABLED	| Enable the transition button
//      @emem BS_TRANS_CHECKED	| Depress the transition button
//
// @comm If BS_NO_AUTO_UPDATE is not set, then only the BS_REC_* flags are used.  If BS_NO_AUTO_UPDATE
//		is set, then all BS_* flags are used.
//
// @xref <om IDMUSProdConductor.RegisterTransport>, <om IDMUSProdConductor.SetActiveTransport>,
//		<om IDMUSProdConductor.SetBtnStates>
//
/////////////////////////////////////////////////////////////////////////////
//
// @enum ConductorTransitionFlags | Flags used for setting the transition type
//		@emem TRANS_END_OF_SEGMENT | Transition at the end of the segment queue.
//		@emem TRANS_REGULAR_PATTERN	| Transition using a regular pattern, not an Intro/End/Fill/Break.
//		@emem TRANS_NO_TRANSITION | Don't compose a transition - just switch to
//			the next segment at the specified boundary.
//		@emem TRANS_SEGF_DEFAULT | Transition using the DMUS_SEGF_DEFAULT flag.
//		@emem TRANS_SEGF_NOINVALIDATE | Transition using the DMUS_SEGF_NOINVALIDATE flag.
//
// @xref  <om IDMUSProdConductor.GetTransitionOptions>
//
/////////////////////////////////////////////////////////////////////////////
//
// @struct ConductorTransitionOptions | Contains the transition options set via
//		the Transport Control toolbar's Transition button.
//
// @comm <p wPatternType> may either contain one of the DMUS_COMMANDT_TYPES flags or
//		a custom embellishment id.  Values of 100 through 199 signify a custom
//		embellishment id.
//
// @field DWORD | dwBoundaryFlags | One of DMUS_COMPOSEF_IMMEDIATE, DMUS_COMPOSEF_GRID,
//		DMUS_COMPOSEF_BEAT, DMUS_COMPOSEF_ALIGN, DMUS_COMPOSEF_MEASURE, DMUS_COMPOSEF_DEFAULT,
//		or DMUS_COMPOSEF_NONE.
//      Previous flags are mutually exclusive.  One or more of the following flags may also
//		be set: DMUS_COMPOSEF_ENTIRE_TRANSITION, DMUS_COMPOSEF_1BAR_TRANSITION, 
//		DMUS_COMPOSEF_ENTIRE_ADDITION, DMUS_COMPOSEF_1BAR_ADDITION.
// @field WORD | wPatternType | One of the DMUS_COMMANDT_TYPES flags or the custom embellishment id.
// @field DWORD | dwFlags | More flags, defined in the <t ConductorTransitionFlags> enumeration.
// @field DWORD | pDMUSProdNodeSegmentTransition | A pointer to the <i IDMUSProdNode> interface of
//		the node that contains the transition segment to be used.
//
// @xref  <om IDMUSProdConductor.GetTransitionOptions>
//
/////////////////////////////////////////////////////////////////////////////


//#include <fstream.h>
CMIDIInputContainer* g_pMIDIInputContainer = NULL;
CConductor* g_pconductor = NULL;
#define MIDI_CCHANGE    0xB0

CDownloadedInstrumentList::CDownloadedInstrumentList()
{
	m_pDMPort = NULL;
}

CDownloadedInstrumentList::~CDownloadedInstrumentList()
{
	if( m_pDMPort )
	{
		while( !m_lstDownloadedInstruments.IsEmpty() )
		{
			IDirectMusicDownloadedInstrument *pDownloadedInstrument = m_lstDownloadedInstruments.RemoveHead();
			m_pDMPort->UnloadInstrument( pDownloadedInstrument );
			pDownloadedInstrument->Release();
		}
		RELEASE( m_pDMPort );
	}
}

// GetPortCaps - Gets a port's DMUS_PORTCAPS structure

bool GetPortCaps( IDirectMusic *pDMusic, const REFGUID rguidPort, DMUS_PORTCAPS *pdmPortCaps )
{
	// Try and find the portcaps for the default synth
	DMUS_PORTCAPS dmPortCaps;

	DWORD dwIndex = 0;
	while( true )
	{
		ZeroMemory( &dmPortCaps, sizeof( DMUS_PORTCAPS ) );
		dmPortCaps.dwSize = sizeof( DMUS_PORTCAPS );
		if( S_OK == pDMusic->EnumPort( dwIndex, &dmPortCaps ) )
		{
			if( dmPortCaps.guidPort == rguidPort )
			{
				if( pdmPortCaps )
				{
					memcpy( pdmPortCaps, &dmPortCaps, sizeof( DMUS_PORTCAPS ) );
				}
				return true;
			}
			dwIndex++;
		}
		else
		{
			break;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// GetRegString - Gets a string from the system registry 

BOOL GetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize )
{
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType;

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)lpszString, lpdwSize );

		if( (dwType == REG_SZ) && (lResult == ERROR_SUCCESS) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}

BOOL GetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD *pdwData )
{
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType = 0, dwSize = sizeof(DWORD);

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)pdwData, &dwSize );

		if( (dwType == REG_DWORD) && (lResult == ERROR_SUCCESS) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}

BOOL GetRegData( HKEY hKey, LPCTSTR lpValueName, BYTE **ppbData, DWORD *pdwSize )
{
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType = 0;

	lResult = ::RegQueryValueEx( hKey, lpValueName, 0, &dwType, NULL, pdwSize );

	if( (dwType == REG_BINARY) && (lResult == ERROR_SUCCESS) )
	{
		*ppbData = new BYTE[ *pdwSize ];
		lResult = ::RegQueryValueEx( hKey, lpValueName, 0, &dwType, *ppbData, pdwSize );

		if( lResult == ERROR_SUCCESS )
		{
			fSuccess = TRUE;
		}
	}

	return fSuccess;
}

int GetIndexByDataPtr( CComboBox &combobox, void *pData )
{
	if( combobox.GetSafeHwnd() )
	{
		for( int i=0; i < combobox.GetCount(); i++ )
		{
			if( combobox.GetItemDataPtr( i ) == pData )
			{
				return i;
			}
		}
	}
	return -1;
}

CWnd *CreateParentWindow( void )
{
	CWnd* pWndParent = new CWnd;
	if( pWndParent )
	{
		if( !pWndParent->CreateEx(0, AfxRegisterWndClass(0), NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL) )
		{
			delete pWndParent;
			pWndParent = NULL;
		}
	}
	return pWndParent;
}

CConductor::CConductor()
{
    m_pFrameWork = NULL;
	m_hWndFramework = NULL;

	m_pDMPerformance = NULL;
	m_pDMusic = NULL;
	m_pDMAudiopath = NULL;
	m_pAudiopathListItem = 0;

	m_hNotifyEvent = NULL;
	m_hNotifyExitEvent = NULL;
	m_pOutputTool = NULL;
	m_pMIDISaveTool = NULL;
	m_pNotifyTool = NULL;

	m_pIUnknownMIDIExport = NULL;
	m_pISegStateMIDIExport = NULL;
	m_fLeadInMeasureMIDIExport = FALSE;

	m_pToolbarHandler = NULL;
	m_pOptionsToolbarHandler = NULL;
	m_pStatusToolbarHandler = NULL;
	m_pSynthStatusToolbar = NULL;
	m_pWaveRecordToolbar = NULL;
	//m_fAudiopathHasDumpDMOs = false;
    g_pconductor = this;
	m_rpActive = NULL;
	m_rpPlaying = NULL;
	m_fShutdown = FALSE;
	m_dblTempo = 120;
    m_fOutputEnabled = FALSE;
	m_fLatencyAppliesToAllAudiopaths = false;
	m_dwLatency = 100;
	m_dwDefaultLatency = LATENCY_UNSUPPORTED;
	m_pPhoneyDSound = NULL;

	// Set tempo ratio
	DWORD dwData;
	if( GetNewRegDWORD( _T("TempoRatio"), &dwData ) )
	{
		if( dwData < int( DMUS_MASTERTEMPO_MIN * 100.0 ) )
		{
			m_nRatio = int( DMUS_MASTERTEMPO_MIN * 100.0 );
		}
		else if( dwData > int( DMUS_MASTERTEMPO_MAX * 100.0 ) )
		{
			m_nRatio = int( DMUS_MASTERTEMPO_MAX * 100.0 );
		}
		else
		{
			m_nRatio = dwData;
		}
	}
	else
	{
		m_nRatio = 50;
	}

	// Set Transition options
	m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_DEFAULT | DMUS_COMPOSEF_AFTERPREPARETIME;
	if( GetNewRegDWORD( _T("TransitionBoundary"), &dwData ) )
	{
		m_TransitionOptions.dwBoundaryFlags = dwData;
	}
	m_TransitionOptions.wPatternType = 0;
	if( GetNewRegDWORD( _T("TransitionPattern"), &dwData ) )
	{
		m_TransitionOptions.wPatternType = (WORD)dwData;
	}
	m_TransitionOptions.dwFlags = TRANS_NO_TRANSITION;
	if( GetNewRegDWORD( _T("TransitionFlags"), &dwData ) )
	{
		m_TransitionOptions.dwFlags = dwData;
	}
	m_TransitionOptions.pDMUSProdNodeSegmentTransition = NULL;
	// TODO: Load transition segment?

	// Set metronome options
	m_fMetronomeEnabled = FALSE;
	if( GetNewRegDWORD( _T("MetronomeEnabled"), &dwData ) && (dwData != 0) )
	{
		m_fMetronomeEnabled = TRUE;
	}
 
	m_bMetronomeNoteOther = 37;
	if( GetNewRegDWORD( _T("MetronomeNote"), &dwData ) )
	{
		m_bMetronomeNoteOther = BYTE( min( dwData, 127 ) );
	}

	m_bMetronomeNoteOne = 36;
	if( GetNewRegDWORD( _T("MetronomeOneNote"), &dwData ) )
	{
		m_bMetronomeNoteOne = BYTE( min( dwData, 127 ) );
	}

	m_bMetronomeVelocityOther = 100;
	if( GetNewRegDWORD( _T("MetronomeVelocity"), &dwData ) )
	{
		m_bMetronomeVelocityOther = BYTE( min( dwData, 127 ) );
	}

	m_bMetronomeVelocityOne = 117;
	if( GetNewRegDWORD( _T("MetronomeOneVelocity"), &dwData ) )
	{
		m_bMetronomeVelocityOne = BYTE( min( dwData, 127 ) );
	}

	m_dwMetronomePChannel = 9;
	if( GetNewRegDWORD( _T("MetronomePChannel"), &dwData ) )
	{
		m_dwMetronomePChannel = min( dwData,  998 );
	}

	// Set Audio Parameters
#ifndef DMP_XBOX
	m_dwSampleRate = 22050;
#else
	m_dwSampleRate = 48000;
#endif
	if( GetNewRegDWORD( _T("SampleRate"), &dwData ) )
	{
		// Limit sample rate to valid range (set by DMusic performance)
		m_dwSampleRate = min( max( 11025, dwData ), 96000);
	}

	m_dwVoices = 64;
	if( GetNewRegDWORD( _T("Voices"), &dwData ) )
	{
		m_dwVoices = max( 1, min( dwData, 999));
	}

	m_clsidDefaultSynth = GUID_Synth_Default;
	DWORD dwCbData = sizeof(TCHAR) * MAX_PATH;
	TCHAR szGuid[MAX_PATH];
	if( GetNewRegString( _T("DMDefaultPort"), szGuid, &dwCbData ) )
	{
		// Parse port GUID
		GUID guidPortGUID;
		OLECHAR psz[MAX_PATH];
		MultiByteToWideChar( CP_ACP, 0, szGuid, -1, psz, MAX_PATH);

		if( SUCCEEDED( CLSIDFromString(psz, &guidPortGUID) ) )
		{
			// Copy the guid to m_clsidDefaultSynth
			m_clsidDefaultSynth = guidPortGUID;

			// Ensure it's not all zeros
			if( GUID_AllZeros == m_clsidDefaultSynth )
			{
				m_clsidDefaultSynth = GUID_Synth_Default;
			}
		}
	}

	// Set Download flags
#ifndef DMP_XBOX
	m_fDownloadGM = TRUE;
#else
	m_fDownloadGM = FALSE;
#endif
	m_fDownloadDLS = TRUE;
	if( GetNewRegDWORD( _T("DownloadGM"), &dwData ) )
	{
		m_fDownloadGM = (dwData != 0);
	}
	if( GetNewRegDWORD( _T("DownloadDLS"), &dwData ) )
	{
		m_fDownloadDLS = (dwData != 0);
	}


	// Set Phoney DSound option
	g_fUsePhoneyDSound = m_fUsePhoneyDSound = false;
	if( GetNewRegDWORD( _T("UsePhoneyDSound"), &dwData ) && (dwData != 0) )
	{
		g_fUsePhoneyDSound = m_fUsePhoneyDSound = true;
	}
	ZeroMemory( g_szPhoneyDSoundFilterName, sizeof(TCHAR) * MAX_PATH );
	dwCbData = sizeof(TCHAR) * MAX_PATH;
	if( GetNewRegString( _T("PhoneyDSoundDefaultPort"), szGuid, &dwCbData ) )
	{
		// Copy the path to g_szPhoneyDSoundFilterName
		strcpy( g_szPhoneyDSoundFilterName, szGuid );
	}

	::InitializeCriticalSection( &m_csNotifyEntry );
	::InitializeCriticalSection( &m_csAudiopath );
	::InitializeCriticalSection( &m_csAudiopathList );
}

CConductor::~CConductor()
{
	ASSERT( m_fShutdown );

	ReleaseAll();

	if( DynamicTrampoline )
	{
		DetourRemove((PBYTE)DynamicTrampoline, (PBYTE)DynamicDetour);
		DynamicTrampoline = NULL;
	}

	if( g_fKslInitialized )
	{
		KslCloseKsLib();
		g_fKslInitialized = false;
	}

	::DeleteCriticalSection( &m_csAudiopathList );
	::DeleteCriticalSection( &m_csAudiopath );
	::DeleteCriticalSection( &m_csNotifyEntry );
}

void CConductor::ReleaseAll( void )
{
    CleanUp();

	RELEASE( m_pOutputTool );
	RELEASE( m_pMIDISaveTool );
	RELEASE( m_pNotifyTool );

	// Remove any existing default Audio Path
	if (m_pDMPerformance)
	{
		m_pDMPerformance->SetDefaultAudioPath( NULL );
	}

	// Unload any instruments previously downloaded
	while( !m_lstDownloadedInstrumentList.IsEmpty() )
	{
		delete m_lstDownloadedInstrumentList.RemoveHead();
	}

	//RELEASE( m_pAudiopathNode ); // Just a weak reference!

	::EnterCriticalSection( &m_csAudiopath );
	RELEASE( m_pDMAudiopath );
	::LeaveCriticalSection( &m_csAudiopath );

	while( !m_lstAudiopaths.IsEmpty() )
	{
		delete m_lstAudiopaths.RemoveHead();
	}

	RELEASE( m_pDMPerformance );

	RELEASE( m_pDMusic );

	RELEASE( m_pFrameWork );

	RELEASE( m_pPhoneyDSound );

#ifdef _DEBUG
	/*
	while( !m_lstITransportRegEntry.IsEmpty() )
	{
		BSTR name;
		char buf[100];
		IDMUSProdTransport *pTransport;
		m_lstITransportRegEntry.RemoveHead()->GetTransport( &pTransport );
		if( pTransport )
		{
			pTransport->GetName( &name );
			lstrcpy( buf, (char *)name );
			SysFreeString( name );
			lstrcat( buf, " did not unregister its transport interface with Conductor." );
			AfxMessageBox( buf );
		}
	}
	*/
#endif
	// BUGBUG: This will leak memory if not all transports removed themselves?
	m_lstITransportRegEntry.RemoveAll();
    g_pconductor = NULL;
}

HRESULT CConductor::InitializeDirectMusic( CString &strError )
{
	ASSERT( m_pFrameWork != NULL );
	if ( m_pFrameWork == NULL )
	{
		strError.LoadString( IDS_ERROR_NULLFRAMEWORK );
		return E_UNEXPECTED;
	}
	
#ifdef DMP_XBOX 
    // XBOX always needs the CoCreate reroute so it can use its own dmime.dll.
	if( DynamicTrampoline == NULL )
	{
		HRESULT (STDAPICALLTYPE *DynamicTarget)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
						IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = &CoCreateInstance;
		DynamicTrampoline = (FUNCPTR_CoCreate)DetourFunction( (PBYTE)DynamicTarget, (PBYTE)DynamicDetour );
	}
#endif

	// Get the DirectMusic object
	HRESULT hr = m_pFrameWork->GetSharedObject( CLSID_DirectMusic, IID_IDirectMusic8, 
												(LPVOID*)&m_pDMusic);
	if ( FAILED(hr) )
	{
		hr = CoCreateInstance(CLSID_DirectMusic,
								  NULL,
								  CLSCTX_INPROC_SERVER,
								  IID_IDirectMusic8,
								  (LPVOID*)&m_pDMusic);
		if ( FAILED(hr) )
		{
			strError.LoadString( IDS_ERR_CREATE_DMUSIC );
			return hr;
		}
	}

	// Get Producer's window handle
	ASSERT( m_pToolbarHandler );
	if( !m_pToolbarHandler )
	{
		strError.LoadString( IDS_ERROR_NULLTOOLBAR );
		return E_UNEXPECTED;
	}
	m_hWndFramework = ::GetParent( m_pToolbarHandler->m_hWnd );

	// Set up DirectSound
	hr = m_pDMusic->SetDirectSound( NULL, m_hWndFramework );
#ifdef _DEBUG
	if( FAILED(hr) )
	{
		TRACE("Conductor: m_pDMusic->SetDirectSound() failed with %x.\n", hr);
	}
#endif

	// DeActivate DirectMusic
	hr = m_pDMusic->Activate( FALSE );
#ifdef _DEBUG
	if( FAILED(hr) )
	{
		TRACE("Conductor: m_pDMusic->Activate() failed with %x.\n", hr);
	}
#endif

	// Create and initialize the performance
	hr = CoCreateInstance( CLSID_DirectMusicPerformance, 
						  NULL, 
						  CLSCTX_ALL, 
						  IID_IDirectMusicPerformance8,
						  (void**)&m_pDMPerformance );
	if ( FAILED(hr) )
	{
		strError.LoadString( IDS_ERR_CREATE_PERF );
		return hr;
	}

	// Query for the base IDirectMusic interface
	IDirectMusic *pDirectMusic;
	if( FAILED( m_pDMusic->QueryInterface( IID_IDirectMusic, (void **)&pDirectMusic ) ) )
	{
		strError.LoadString( IDS_ERR_INIT_PERF );
		return hr;
	}

	// Initialize the performance
	DMUS_AUDIOPARAMS dmAudioParams;
	ZeroMemory( &dmAudioParams, sizeof( DMUS_AUDIOPARAMS ) );
	dmAudioParams.dwSize = sizeof( DMUS_AUDIOPARAMS );
	GetAudioParams( &dmAudioParams );

	// Initialize default latency
	m_dwDefaultLatency = LATENCY_UNSUPPORTED;
    g_fUsePhoneyDSound = m_fUsePhoneyDSound;
	if( m_fUsePhoneyDSound )
	{
		if( !m_pPhoneyDSound )
		{
			m_pPhoneyDSound = new CPhoneyDSound;

			if( DynamicTrampoline == NULL )
			{
				HRESULT (STDAPICALLTYPE *DynamicTarget)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
								IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = &CoCreateInstance;
				DynamicTrampoline = (FUNCPTR_CoCreate)DetourFunction( (PBYTE)DynamicTarget, (PBYTE)DynamicDetour );
			}
		}

		if( m_pPhoneyDSound )
		{
			IDirectSound *pDSound;
			m_pPhoneyDSound->QueryInterface(IID_IDirectSound,(void **)&pDSound);
			hr = m_pDMPerformance->InitAudio( &pDirectMusic, &pDSound, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams);
			pDSound->Release();
		}
		else
		{
			hr = m_pDMPerformance->InitAudio( &pDirectMusic, NULL, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams );
		}
	}
	else
	{
		hr = m_pDMPerformance->InitAudio( &pDirectMusic, NULL, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams );
	}
	
	// Release the base IDirectMusic interface
	RELEASE( pDirectMusic );

	// Check to see if the performance initialized correctly.
	if( FAILED(hr) )
	{
		strError.LoadString( IDS_ERR_INIT_PERF );
		return hr;
	}

	// Ensure AutoDownload is NOT set
	BOOL fAutoDownload;
	fAutoDownload = FALSE;
	m_pDMPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAutoDownload, sizeof(BOOL) );

	DWORD dwData;
	if( GetNewRegDWORD( _T("TempoRatioEnabled"), &dwData ) && (dwData != 0) )
	{
		float fModifier;
		fModifier = float(m_nRatio) / 100.0f;
		m_pDMPerformance->SetGlobalParam( GUID_PerfMasterTempo, &fModifier, sizeof(float) );
	}

	// Read in the default DirectMusic audio path from the registry
	// If we don't have a DirectMusic MIDI input port, try and find one.
	IDirectMusicAudioPath *pAudiopath = NULL;
	ReadDefaultDirectMusicAudiopath( &pAudiopath );

	// We may not have an audio path, if the default synth doesn't support audiopaths
	// ASSERT( pAudiopath );

	// Get 'Latency' value
	if( GetNewRegDWORD( _T("Latency"), &dwData ) && (dwData != 0) )
	{
		m_dwLatency = dwData;

		if( m_fUsePhoneyDSound )
		{
			g_dwLatency = dwData;
		}
	}

	// Get 'Update Latency' state
	if( GetNewRegDWORD(_T("ApplyLatencyToAllAudiopaths"), &dwData ) )
	{
		m_fLatencyAppliesToAllAudiopaths = (dwData != 0) ? true : false;
	}

	// Activate the Audio path.  This will overwrite m_dwLatency if m_fLatencyApplies...Paths is not set
	ActivateAudiopath( pAudiopath );

	// Release our reference to the Audio Path
	RELEASE( pAudiopath );

	// Create the output status tool and the MIDI save tool
	m_pOutputTool = new COutputTool;
	m_pMIDISaveTool = new MIDISaveTool;
	m_pNotifyTool = new CNotifyTool;

	// Add output status tool and MIDI save tool to the audio path
	AddTools( m_pDMAudiopath, m_pOutputTool, m_pMIDISaveTool );

	IDirectMusicGraph *pGraph = NULL;
	if( FAILED( m_pDMPerformance->GetGraph( &pGraph ) ) )
	{
		if( SUCCEEDED( ::CoCreateInstance( CLSID_DirectMusicGraph, NULL, CLSCTX_INPROC_SERVER, IID_IDirectMusicGraph, (void **)&pGraph ) ) )
		{
			if( FAILED( m_pDMPerformance->SetGraph( pGraph ) ) )
			{
				pGraph->Release();
				pGraph = NULL;
			}
		}
	}

	if( pGraph )
	{
		pGraph->InsertTool( m_pNotifyTool, NULL, 0, 0 );
		pGraph->Release();
	}

	// Initialize the notification event handler
	m_hNotifyEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	if( m_hNotifyEvent )
	{
		m_pDMPerformance->SetNotificationHandle( m_hNotifyEvent, 0 );
		m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_SEGMENT );
		m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_MEASUREANDBEAT );
		m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_PERFORMANCE );
		// Start Notify thread
		CWinThread *pThread = ::AfxBeginThread( NotifyThreadProc, this );
		if( pThread == NULL )
		{
			TRACE("Conductor: Failed to start Notify thread.\n");
		}
		else
		{
			pThread->m_bAutoDelete = TRUE;
		}
	}

	m_fOutputEnabled = TRUE;

	return S_OK;
	//return hr;
}

HRESULT CConductor::ActivateXboxPath(DWORD dwXboxPath)

{
    // Create the equivalent audiopath on the XBOX itself. 
	IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
	HRESULT hr = m_pDMPerformance->GetDefaultAudioPath( &pIDirectMusicAudioPath );
	IKsControl *pIKsControl = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = pIDirectMusicAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, CLSID_XboxSynth, 0, IID_IKsControl, (void **)&pIKsControl );
	    if( SUCCEEDED(hr) )
	    {
		    KSPROPERTY ksProperty;
		    ksProperty.Set = GUID_Xbox_PROP_XboxAudioPath;
            ksProperty.Flags = KSPROPERTY_TYPE_SET;
		    ksProperty.Id = 0;
		    DWORD dwDataSize = 0;
		    hr = pIKsControl->KsProperty( &ksProperty, sizeof(KSPROPERTY), &dwXboxPath, sizeof(DWORD), &dwDataSize ); 
	        pIKsControl->Release();
        }
        pIDirectMusicAudioPath->Release();
    }
    return hr;
}



HRESULT CConductor::ActivateAudiopath( IDirectMusicAudioPath *pAudiopath, bool fSendNotificationsAndDownloadGM )
{
	if( !m_pDMPerformance )
	{
		ASSERT( FALSE );
		return E_FAIL;
	}

	// Double-check to make sure something actually should change
	if( pAudiopath == m_pDMAudiopath )
	{
		return S_FALSE;
	}

	// Stop everything to get rid of stuck notes
	StopAllNotesAndSegments();

	// Stop exporting wave and/or MIDI files
	if( m_pWaveRecordToolbar )
	{
		m_pWaveRecordToolbar->StopAll();
	}

	if( fSendNotificationsAndDownloadGM )
	{
		BroadCastPortRemovalNotification();
	}

	// Try and set pAudiopath as the default audio path
	if( SUCCEEDED( m_pDMPerformance->SetDefaultAudioPath( pAudiopath ) ) )
	{
		::EnterCriticalSection( &m_csAudiopath );

		// Release the existing audio path
		RELEASE( m_pDMAudiopath );

		// Clear the pointer to the last sampled output port
		g_rpLastSampledPort = NULL;

		// Save a pointer to the new audio path
		m_pDMAudiopath = pAudiopath;
		if( m_pDMAudiopath )
		{
			m_pDMAudiopath->AddRef();
		}

		::LeaveCriticalSection( &m_csAudiopath );

		// Add output status tool and MIDI save tool to the audio path
		if( pAudiopath )
		{
			AddTools( pAudiopath, m_pOutputTool, m_pMIDISaveTool );
		}

		// Now, download GM (if necessary)
		if( fSendNotificationsAndDownloadGM )
		{
			DownOrUnLoadGM();
		}

		// Initialize the performance
		DMUS_AUDIOPARAMS dmAudioParams;
		ZeroMemory( &dmAudioParams, sizeof( DMUS_AUDIOPARAMS ) );
		dmAudioParams.dwSize = sizeof( DMUS_AUDIOPARAMS );
		GetAudioParams( &dmAudioParams );

		// If the latency value has not yet been set,
		// or if the default synth doesn't support audiopaths
		if( (m_dwDefaultLatency == LATENCY_UNSUPPORTED)
		||	(dmAudioParams.dwFeatures == 0) )
		{
		// Get the default latency
		m_dwDefaultLatency = LATENCY_UNSUPPORTED;

		if( pAudiopath )
		{
			// A flag to show whether or not we have the latency
			bool fGotLatency = false;

			// Iterate through the default Audiopath's ports
			IDirectMusicPort *pDMPort = NULL;
			DWORD dwIndex = 0;
			while( S_OK == pAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
			{
				// Verify we have a valid port pointer
				ASSERT( pDMPort );

				IKsControl *pIKsControl;
				HRESULT hr = pDMPort->QueryInterface(IID_IKsControl, (void**)&pIKsControl);
				if (SUCCEEDED(hr)) 
				{
					KSPROPERTY ksp;
					ULONG cb;
					DWORD dwLatency;

					// Get the latency
					ZeroMemory(&ksp, sizeof(ksp));
					ksp.Set   = GUID_DMUS_PROP_WriteLatency;
					ksp.Id    = 0;
					ksp.Flags = KSPROPERTY_TYPE_GET;

					if( SUCCEEDED( pIKsControl->KsProperty(&ksp,
										 sizeof(ksp),
										 (LPVOID)&dwLatency,
										 sizeof(dwLatency),
										 &cb) ) )
					{
						if( !fGotLatency )
						{
							m_dwDefaultLatency = dwLatency;
							fGotLatency = true;
						}
						else
						{
							m_dwDefaultLatency = max( m_dwDefaultLatency, dwLatency );
						}
					}

					pIKsControl->Release();
				}

				// Release the port
				pDMPort->Release();

				// Go on to the next port in the audio path
				dwIndex++;
			}
		}

		// End latency and default synth check
		}

		// Update the ports' latency, if necessary
		if( m_fUsePhoneyDSound
		||	m_dwDefaultLatency != LATENCY_UNSUPPORTED )
		{
			// If the latency value should not be applied to all audiopaths
			if( !m_fLatencyAppliesToAllAudiopaths )
			{
				// If the default latency is valid
				if( m_dwDefaultLatency != LATENCY_UNSUPPORTED )
				{
					// Reset the current latency value
					m_dwLatency = m_dwDefaultLatency;
				}
				else
				{
					// Otherwise, we're using the Phoney DSound driver
					ASSERT(m_fUsePhoneyDSound);
					// Reset the current latency value
					m_dwLatency = DEFAULT_PHONEY_DS_LATENCY;
				}
			}

			// In all cases, reset the synth's latency
			UpdateLatency();
		}

		// TODO: Write the ID to the registry
		//SetNewRegDWORD(_T("OutputDevice"), pAudiopath, TRUE);

		// Write the configuration to the registry
		WriteDefaultDirectMusicAudiopath();

		// Check if the audiopath has any dump DMOs in it
		/*
		m_fAudiopathHasDumpDMOs = false;
		if( pAudiopath )
		{
			DWORD dwBufferIndex = 0;
			IDirectSoundBuffer* pIDirectSoundBuffer = NULL;
			while( !m_fAudiopathHasDumpDMOs
			&&	(S_OK == pAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, dwBufferIndex,
				 GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer )) )
			{
				IUnknown* pIUnknown = NULL;
				if( S_OK == pAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER_DMO, dwBufferIndex,
					GUID_DSFX_STANDARD_DUMP, 0, IID_IUnknown, (void**) &pIUnknown ) )
				{
					m_fAudiopathHasDumpDMOs = true;
					pIUnknown->Release();
				}

				dwBufferIndex++;
				pIDirectSoundBuffer->Release();
			}
			dwBufferIndex = 0;
			while( !m_fAudiopathHasDumpDMOs
			&&	(S_OK == pAudiopath->GetObjectInPath( 0, DMUS_PATH_MIXIN_BUFFER, dwBufferIndex,
				 GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer )) )
			{
				IUnknown* pIUnknown = NULL;
				if( S_OK == pAudiopath->GetObjectInPath( 0, DMUS_PATH_MIXIN_BUFFER_DMO, dwBufferIndex,
					GUID_DSFX_STANDARD_DUMP, 0, IID_IUnknown, (void**) &pIUnknown ) )
				{
					m_fAudiopathHasDumpDMOs = true;
					pIUnknown->Release();
				}

				dwBufferIndex++;
				pIDirectSoundBuffer->Release();
			}
		}
		*/

		// Update the wave record button state
		if( m_pWaveRecordToolbar )
		{
			m_pWaveRecordToolbar->UpdateRecordButtonState();
		}

		// Return that we succeeded
		return S_OK;
	}
	else
	{
		AfxMessageBox( IDS_ERR_APATH_ACTIVATE, MB_ICONEXCLAMATION | MB_OK );
		return E_FAIL;
	}
}

// add the tools to the toolgraph
void CConductor::AddToolsToToolgraph(IDirectMusicGraph *pGraph, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool )
{
	ASSERT( pGraph );
	if( pGraph == NULL )
	{
		return;
	}

	HRESULT hr = S_OK;

	// add the tools to the graph - they will be AddRef()'d inside the
	// InsertTool function. The second parameter, NULL, means to apply
	// the tool to all PChannel's. The third parameter, 0, is only
	// applicable if the second parameter is non-NULL. The fourth
	// parameter, 0, means to insert the tool at the beginning of
	// any list of tools inside the graph.
	if( pMIDISaveTool )
	{
		hr = pGraph->InsertTool( (IDirectMusicTool*)pMIDISaveTool, NULL, 0, 0 );

		// It's ok if the tool already exists
		if( hr == DMUS_E_ALREADY_EXISTS )
		{
			hr = S_OK;
		}
	}
	if( pCOutputTool
	&&	SUCCEEDED ( hr ) )
	{
		hr = pGraph->InsertTool( (IDirectMusicTool*)pCOutputTool, NULL, 0, 0 );

		// It's ok if the tool already exists
		/*
		if( hr == DMUS_E_ALREADY_EXISTS )
		{
			hr = S_OK;
		}
		*/
	}
}

// add the tools to the segment state
void CConductor::AddToolsToSegState(IDirectMusicSegmentState8 *pIDirectMusicSegmentState, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool )
{
	ASSERT( pIDirectMusicSegmentState );
	if( pIDirectMusicSegmentState == NULL )
	{
		return;
	}

	// First, query for an IDirectMusicGraph object to hold the tool
	IDirectMusicGraph* pGraph;
	if( SUCCEEDED( pIDirectMusicSegmentState->GetObjectInPath( 0, DMUS_PATH_SEGMENT_GRAPH, 0, GUID_All_Objects, 0,
												IID_IDirectMusicGraph, (void**)&pGraph ) ) )
	{
		AddToolsToToolgraph( pGraph, pCOutputTool, pMIDISaveTool );
		pGraph->Release();
	}
	else if( SUCCEEDED( pIDirectMusicSegmentState->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH_GRAPH, 0, GUID_All_Objects, 0,
												IID_IDirectMusicGraph, (void**)&pGraph ) ) )
	{
		AddToolsToToolgraph( pGraph, pCOutputTool, pMIDISaveTool );
		pGraph->Release();
	}
}

// add the tools to the audio path
void CConductor::AddTools(IDirectMusicAudioPath *pAudiopath, COutputTool* pCOutputTool, MIDISaveTool* pMIDISaveTool)
{
	if( pAudiopath == NULL )
	{
		return;
	}

	// First, query for an IDirectMusicGraph object to hold the tool
	IDirectMusicGraph* pGraph;
	if( SUCCEEDED( pAudiopath->GetObjectInPath( 0, DMUS_PATH_SEGMENT_GRAPH, 0, GUID_All_Objects, 0,
												IID_IDirectMusicGraph, (void**)&pGraph ) ) )
	{
		AddToolsToToolgraph( pGraph, pCOutputTool, pMIDISaveTool );
		pGraph->Release();
	}
	else if( SUCCEEDED( pAudiopath->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH_GRAPH, 0, GUID_All_Objects, 0,
												IID_IDirectMusicGraph, (void**)&pGraph ) ) )
	{
		AddToolsToToolgraph( pGraph, pCOutputTool, pMIDISaveTool );
		pGraph->Release();
	}
}

void CConductor::SetupWaveSaveDMO( IDirectMusicAudioPath *pAudiopath )
{
	ASSERT( pAudiopath );
	if( pAudiopath == NULL )
	{
		return;
	}

	if( m_pWaveRecordToolbar )
	{
		m_pWaveRecordToolbar->StartOrStopDumpDMOsOnAudiopath( m_pWaveRecordToolbar->m_fRecordingWave ? true : false, pAudiopath );
	}
}

void ActivatePortsInAudiopath( IDirectMusicAudioPath *pAudiopath, BOOL fActivate )
{
	if( pAudiopath )
	{
		// Iterate through the default Audiopath's ports
		IDirectMusicPort *pDMPort = NULL;
		DWORD dwIndex = 0;
		while( S_OK == pAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
		{
			// Verify we have a valid port pointer
			if( pDMPort )
			{
				// Activate the port
				pDMPort->Activate( fActivate );

				// Release the port
				pDMPort->Release();
			}

			// Go on to the next port in the audio path
			dwIndex++;
		}
	}
}

HRESULT CConductor::SuspendOutput()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	if( m_pWaveRecordToolbar )
	{
		// Stop exporting wave and/or MIDI files
		m_pWaveRecordToolbar->StopAll();

		// Disable recording wave/MIDI files
		::SendMessage( m_pWaveRecordToolbar->m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_WAVE_RECORD, (LPARAM) MAKELONG(FALSE, 0) );
		::SendMessage( m_pWaveRecordToolbar->m_hWndToolbar, TB_ENABLEBUTTON, (WPARAM)ID_MIDI_EXPORT, (LPARAM) MAKELONG(FALSE, 0) );
	}

	// Suspend DMusic MIDI input
	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->OnOutputDisabled();
	}

	hr = S_OK;

	// Invalidate the performance to stop stuck notes
	/*
	MUSIC_TIME mtNow;
	if( SUCCEEDED( m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
	{
		m_pDMPerformance->Invalidate( mtNow, 0 );
	}
	*/

	// Deactivate all Secondary Segments
	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		m_lstSecondaryToolbars.GetNext( pos )->Activate( FALSE );
	}

	MUSIC_TIME mtNow;
	if( SUCCEEDED( m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
	{
		IDirectMusicSegmentState *pIDirectMusicSegmentState;
		if( SUCCEEDED( m_pDMPerformance->GetSegmentState( &pIDirectMusicSegmentState, mtNow ) ) )
		{
			IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
			if( SUCCEEDED( pIDirectMusicSegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
			{
				IDirectMusicAudioPath *pIDirectMusicAudioPath;
				if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pIDirectMusicAudioPath ) ) )
				{
					pIDirectMusicAudioPath->Activate( FALSE );
					pIDirectMusicAudioPath->Release();
				}
				pIDirectMusicSegmentState8->Release();
			}
			pIDirectMusicSegmentState->Release();
		}
	}

	::EnterCriticalSection( &m_csAudiopath );
	if( m_pDMAudiopath )
	{
		m_pDMAudiopath->Activate( FALSE );
		//ActivatePortsInAudiopath( m_pDMAudioPath, FALSE );
	}
	::LeaveCriticalSection( &m_csAudiopath );

	// Deactivate DirectSound (In this case, NULL means to deactivate DirectSound).
	// In InitializeDirectMusic(), it meant to create a default DirectSound object..
	// Not needed - DirectMusic does this automatically when all ports become inactive.
	//hr = m_pDMusic->SetDirectSound( NULL, m_hWndFramework );

	m_fOutputEnabled = FALSE;

	// Notify all other components that output has been disabled
	BroadCastPortRemovalNotification();

	return hr;
}

HRESULT CConductor::ResumeOutput()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	hr = E_FAIL;
	
	// Resume DMusic MIDI input
	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->OnOutputEnabled();
	}

	::EnterCriticalSection( &m_csAudiopath );
	if( m_pDMAudiopath )
	{
		m_pDMAudiopath->Activate( TRUE );
		//ActivatePortsInAudiopath( m_pDMAudioPath, TRUE );
	}
	::LeaveCriticalSection( &m_csAudiopath );

	MUSIC_TIME mtNow;
	if( SUCCEEDED( m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
	{
		IDirectMusicSegmentState *pIDirectMusicSegmentState;
		if( SUCCEEDED( m_pDMPerformance->GetSegmentState( &pIDirectMusicSegmentState, mtNow ) ) )
		{
			IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
			if( SUCCEEDED( pIDirectMusicSegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
			{
				IDirectMusicAudioPath *pIDirectMusicAudioPath;
				if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pIDirectMusicAudioPath ) ) )
				{
					pIDirectMusicAudioPath->Activate( TRUE );
					pIDirectMusicAudioPath->Release();
				}
				pIDirectMusicSegmentState8->Release();
			}
			pIDirectMusicSegmentState->Release();
		}
	}

	// Activate all Secondary Segments
	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		m_lstSecondaryToolbars.GetNext( pos )->Activate( TRUE );
	}

	m_fOutputEnabled = TRUE;

	// Clear all thru connections
	ClearThruConnections();

	// Notify all other components that output has been enabled
	BroadCastPortChangeNotification();

	// Enable recording wave/MIDI files
	if( m_pWaveRecordToolbar )
	{
		m_pWaveRecordToolbar->UpdateRecordButtonState();
		m_pWaveRecordToolbar->UpdateExportMIDIButtonState();
	}
	return hr;
}



//  @method HRESULT | IDMUSProdConductor | GetPerformanceEngine | This function returns an AddRef()'d
//		reference to the IDirectMusicPerformance interface.
//
//  @parm   IUnknown** | ppIPerformance | Location of an IUnknown interface from
//		which an IDirectMusicPerformance interface can be obtained. On success, the caller is responsible
//		for calling <om IUnknown::Release>() when this pointer is no longer needed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p ppIPerformance>
//
//  @xref  <i IDMUSProdConductor>
//
HRESULT STDMETHODCALLTYPE CConductor::GetPerformanceEngine( IUnknown __RPC_FAR* __RPC_FAR* ppPerformance )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( ppPerformance == NULL )
    {
        return E_POINTER;
    }
    *ppPerformance = m_pDMPerformance;
	if ( m_pDMPerformance != NULL )
	{
		(*ppPerformance)->AddRef();
	}
    return S_OK;
}

//  @method HRESULT | IDMUSProdConductor | GetPort | This function returns an AddRef()'d pointer to
//		the first active port that supports DLS level 2.
//
//  @parm   IUnknown** | ppIPort | Location of an IUnknown interface from which an
//		IDirectMusicPort interface can be obtained. On success, the caller is responsible for calling
//		<om IUnknown::Release>() when this pointer is no longer needed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | A NULL was passed as <p ppIPort>
//	@rvalue DMUS_E_TYPE_UNSUPPORTED | No currently active port supports DLS level 2.
//	@rvalue DMUS_E_TYPE_DISABLED | The output is currently disabled.
//
//  @xref  <i IDMUSProdConductor>
//
HRESULT STDMETHODCALLTYPE CConductor::GetPort( IUnknown __RPC_FAR *__RPC_FAR* ppPort )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ppPort == NULL )
	{
		return E_POINTER;
	}

	::EnterCriticalSection( &m_csAudiopath );
	IDirectMusicAudioPath *pDMAudioPath = m_pDMAudiopath;
	if( pDMAudioPath )
	{
		pDMAudioPath->AddRef();
	}
	::LeaveCriticalSection( &m_csAudiopath );

	if( !m_fOutputEnabled || !pDMAudioPath )
	{
		*ppPort = NULL;
		if( pDMAudioPath )
		{
			pDMAudioPath->Release();
		}
		return DMUS_E_TYPE_DISABLED;
	}

	// Iterate through the default Audiopath's port
	DMUS_PORTCAPS dmPortCaps;
	DWORD dwIndex = 0;
	IDirectMusicPort *pDMPort = NULL;
	while( S_OK == pDMAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void**)&pDMPort ) )
	{
		// Verify we have a valid port pointer
		ASSERT( pDMPort );

		// Initialize the DMUS_PORTCAPS structure
		dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);

		// Try and get the capabilities of this port, and check if it supports DLS2
		if( SUCCEEDED( pDMPort->GetCaps( &dmPortCaps ) )
		&&	(dmPortCaps.dwFlags & DMUS_PC_DLS2) )
		{
			*ppPort = pDMPort;
			pDMAudioPath->Release();
			return S_OK;
		}

		// Release the port
		pDMPort->Release();

		// Go on to the next port in the audio path
		dwIndex++;
	}

	pDMAudioPath->Release();

	*ppPort = NULL;
	return DMUS_E_TYPE_UNSUPPORTED;
}

/*======================================================================================
METHOD:  ICONDUCTOR::REGISTERTRANSPORT
========================================================================================
@method HRESULT | IDMUSProdConductor| RegisterTransport | Add <p pITransport>
	to the list of items in the Transport Control toolbar's combo box.
	If <p pITransport> is already in the combo box, increment its reference count.

@parm   IDMUSProdTransport* | pITransport | Pointer to the <i IDMUSProdTransport> interface which
		will be added to the Transport Control toolbar's combo box.
@parm	DWORD | dwFlags | A combination of <t ButtonStateFlags> determining the initial state of the
		transport buttons when this transport is active.

@rvalue S_OK | <p pITransport> was successfully added.
@rvalue E_FAIL | An error occurred, and <p pITransport> was not added to the Transport Control
	toolbar's combo box.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.UnRegisterTransport>,
	   <om IDMUSProdConductor.SetActiveTransport>, <om IDMUSProdConductor.SetTransportName>,
	   <om IDMUSProdConductor.IsTransportPlaying>, <om IDMUSProdConductor.TransportStopped>,
	   <i IDMUSProdTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::RegisterTransport( IDMUSProdTransport *pTransport, DWORD dwFlags )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	POSITION pos = m_lstITransportRegEntry.GetHeadPosition();
	IDMUSProdTransport* pTrans;
	while( pos )
	{
		const POSITION posCurrent = pos;
		m_lstITransportRegEntry.GetNext( pos )->GetTransport( &pTrans );;
		if( pTrans == pTransport )
		{
			// Increment reference count on existing entry
			m_lstITransportRegEntry.GetAt( posCurrent )->AddRef();
			return S_OK;
		}
	}
	// Transport was not found so create and add one
	CTransportRegEntry*  pCEntry = new CTransportRegEntry( pTransport );
	if( pCEntry )
	{
		ITransportRegEntry*  pIEntry;
		if( SUCCEEDED(pCEntry->QueryInterface( IID_ITransportRegEntry, (void**) &pIEntry )) )
		{
			m_lstITransportRegEntry.AddHead( pIEntry );

			if (dwFlags & BS_NO_AUTO_UPDATE)
			{
				DWORD dwNewFlags;
				pIEntry->GetFlags( &dwNewFlags );
				dwNewFlags &= ~(B_USER_FLAGS | B_TRANS_ENABLED | B_TRANS_CHECKED);
				dwNewFlags |= B_NOT_USING_ENGINE | (dwFlags & B_USER_FLAGS);
				pIEntry->SetFlags( dwNewFlags );
			}
			else
			{
				// For setting the record button state
				DWORD dwNewFlags;
				pIEntry->GetFlags( &dwNewFlags );
				dwNewFlags &= ~(B_NOT_USING_ENGINE | B_REC_ENABLED | B_REC_CHECKED);
				dwNewFlags |= dwFlags & (B_REC_ENABLED | B_REC_CHECKED);
				if ( m_pToolbarHandler )
				{
					m_pToolbarHandler->SetStateFromEngine( &dwNewFlags );
				}
				pIEntry->SetFlags( dwNewFlags );
			}

			if ( m_pToolbarHandler )
			{
				m_pToolbarHandler->AddTransport( pIEntry );
			}

			return S_OK;
		}
	}
	return E_FAIL;
}

/*======================================================================================
METHOD:  ICONDUCTOR::UNREGISTERTRANSPORT
========================================================================================
@method HRESULT | IDMUSProdConductor| UnRegisterTransport | Remove <p pITransport> from
	the Transport Control toolbar's combo box, and stop it if it is playing.  If it was
	added more than once, remove one reference to it.  When all references are removed,
	it will then be removed from the Transport Control toolbar's combo box and stopped
	if it is playing.

@parm   IDMUSProdTransport* | pITransport | Pointer to the <i IDMUSProdTransport> interface to
		be removed from the Transport Control toolbar's combo box.

@rvalue S_OK | <p pITransport> was successfully removed.
@rvalue E_FAIL | <p pITransport> was not previously added via <om IDMUSProdConductor.RegisterTransport>.
@rvalue E_POINTER | <p pITransport> is NULL.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>,
	   <om IDMUSProdConductor.SetActiveTransport>, <om IDMUSProdConductor.SetTransportName>,
	   <om IDMUSProdConductor.IsTransportPlaying>, <om IDMUSProdConductor.TransportStopped>,
	   <i IDMUSProdTransport>
	
--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::UnRegisterTransport( IDMUSProdTransport *pTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pTransport == NULL )
	{
		return E_POINTER;
	}

	POSITION pos = m_lstITransportRegEntry.GetHeadPosition();
	IDMUSProdTransport*  pTrans;
	while( pos )
	{
		const POSITION posCurrent = pos;
		ITransportRegEntry *pITransportRegEntry = m_lstITransportRegEntry.GetNext( pos );
		pITransportRegEntry->GetTransport( &pTrans );
		if( pTrans == pTransport )
		{
			if( pITransportRegEntry == m_rpPlaying )
			{
				pITransportRegEntry->AddRef();
				if ( pITransportRegEntry->Release() == 1 )
				{
					if (FAILED(pTransport->Stop( TRUE )))
					{
						TRACE("UnRegisterTransport: Transport failed to Stop() when unregistered\n");
					}
					else
					{
						DWORD dwFlags;
						pITransportRegEntry->GetFlags( &dwFlags );

						// If we've stopped the engine, wait for the engine to really stop
						if (!(dwFlags & B_NOT_USING_ENGINE))
						{
							short n = 0;
							while( n++ < 100 )
							{
								if( !IsEnginePlaying() )
								{
									break;
								}
								Sleep( 50 );
							}
							if (n==100)
							{
								TRACE("UnRegisterTransport Timed out while stopping playing transport\n");
							}
						}
					}
				}
			}
			if( pITransportRegEntry->Release() == 0 )
			{
				// Remove entry from registry pointer list if no more references
				m_lstITransportRegEntry.RemoveAt( posCurrent );

				if( pITransportRegEntry == m_rpActive )
				{
					m_rpActive = NULL;
				}
				if( pITransportRegEntry == m_rpPlaying )
				{
					m_rpPlaying = NULL;
				}
				if ( m_pToolbarHandler )
				{
					m_pToolbarHandler->RemoveTransport( pITransportRegEntry );
				}
				return S_OK;
			}
		}
	}
	// Entry was not found
	return E_FAIL;
}


/*======================================================================================
METHOD:  ICONDUCTOR::SETACTIVETRANSPORT
========================================================================================
@method HRESULT | IDMUSProdConductor| SetActiveTransport | Sets <p pITransport> as the active
	transport and uses <p dwFlags> to set the initial state of the Transport Control buttons.

@parm   IDMUSProdTransport* | pITransport | Pointer to the <i IDMUSProdTransport> interface to
		set as active in the Transport Control toolbar.
@parm	DWORD | dwFlags | A combination of <t ButtonStateFlags> determining the state of the
		transport buttons.

@rvalue S_OK | The active transport was successfully set.
@rvalue E_INVALIDARG | <p pITransport> was not previously added by calling
		<om IDMUSProdConductor.RegisterTransport>.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>,
	   <om IDMUSProdConductor.UnRegisterTransport>, <om IDMUSProdConductor.SetTransportName>,
	   <om IDMUSProdConductor.IsTransportPlaying>, <om IDMUSProdConductor.TransportStopped>,
	   <i IDMUSProdTransport>
  
--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetActiveTransport(
	IDMUSProdTransport* pTransport,
	DWORD btnFlags
)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( pTransport == NULL )
    {
        m_rpActive = NULL;
		if ( m_pToolbarHandler )
		{
			m_pToolbarHandler->m_comboActive.SetCurSel(-1);
			m_pToolbarHandler->SetStateAuto();
		}
		// If it exists, redraw the status toolbar
		if( m_pStatusToolbarHandler )
		{
			m_pStatusToolbarHandler->RefreshAllButtons();
		}
        return S_OK;
    }

	// Initialize to NULL
	m_rpActive = NULL;

	POSITION pos = m_lstITransportRegEntry.GetHeadPosition();
	IDMUSProdTransport*  pTmpTransport;
	while( pos )
	{
		ITransportRegEntry *pITransportRegEntry = m_lstITransportRegEntry.GetNext( pos );
		pITransportRegEntry->GetTransport( &pTmpTransport );
		if( pTmpTransport == pTransport )
		{
			// Set active transport index to this entry in the registry
			m_rpActive = pITransportRegEntry;
			break;
		}
	}

	if (m_rpActive == NULL)
	{
		// The transport was not found
		if ( m_pToolbarHandler )
		{
			m_pToolbarHandler->m_comboActive.SetCurSel(-1);
			m_pToolbarHandler->SetStateAuto();
		}
		// If it exists, redraw the status toolbar
		if( m_pStatusToolbarHandler )
		{
			m_pStatusToolbarHandler->RefreshAllButtons();
		}
		return E_INVALIDARG;
	}

	if ( m_pToolbarHandler )
	{
		m_pToolbarHandler->SetActiveTransport( m_rpActive );
	}
	m_rpActive->SetFlags( 0 );

	if (btnFlags & BS_NO_AUTO_UPDATE)
	{
		// For setting the play, stop, and record button states
		SetBtnStates( pTransport, btnFlags );

		DWORD dwFlags;
		m_rpActive->GetFlags( &dwFlags );
		dwFlags &= ~(B_TRANS_ENABLED | B_TRANS_CHECKED);
		m_rpActive->SetFlags( dwFlags );
	}
	else
	{
		// For setting the record button state
		SetBtnStates( pTransport, btnFlags );
		DWORD dwFlags;
		m_rpActive->GetFlags( &dwFlags );
		dwFlags &= ~B_NOT_USING_ENGINE;
		if ( m_pToolbarHandler )
		{
		    m_pToolbarHandler->SetStateFromEngine( &dwFlags );
		}
		m_rpActive->SetFlags( dwFlags );
	}

	if ( m_pToolbarHandler )
	{
	    m_pToolbarHandler->SetStateAuto();
	}

	// If it exists, redraw the status toolbar
	if( m_pStatusToolbarHandler )
	{
		m_pStatusToolbarHandler->RefreshAllButtons();
	}

    return S_OK;
}

/*======================================================================================
METHOD:  ICONDUCTOR::SETTRANSPORTNAME
========================================================================================
@method HRESULT | IDMUSProdConductor| SetTransportName | Sets the name of <p pITransport>
	to <p bstrName>.

@parm   IDMUSProdTransport* | pITransport | Pointer to an <i IDMUSProdTransport> interface.
@parm	BSTR | bstrName | Text to display in the Transport Control toolbar's combo box for <p pITransport>.

@rvalue S_OK | Successfully set the name of <p pITransport> to <p pbstrName>.
@rvalue E_POINTER | Either <p pITransport> or <p bstrName> is NULL.
@rvalue E_INVALIDARG | <p pITransport> was not previously added by calling
		<om IDMUSProdConductor.RegisterTransport>.


@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>,
	   <om IDMUSProdConductor.UnRegisterTransport>, <om IDMUSProdConductor.SetActiveTransport>,
	   <om IDMUSProdConductor.IsTransportPlaying>, <om IDMUSProdConductor.TransportStopped>,
	   <i IDMUSProdTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetTransportName( IDMUSProdTransport *pTransport, BSTR bstrName )
{
	if ( pTransport == NULL )
	{
		return E_POINTER;
	}
	if ( bstrName == NULL )
	{
		return E_POINTER;
	}
	IDMUSProdTransport*  pTmpTransport;
	BOOL fTransportFound = FALSE;
	POSITION pos = m_lstITransportRegEntry.GetHeadPosition();
	while( pos )
	{
		ITransportRegEntry *pITransportRegEntry = m_lstITransportRegEntry.GetNext( pos );
		pITransportRegEntry->GetTransport( &pTmpTransport );
		if( pTmpTransport == pTransport )
		{
			fTransportFound = TRUE;

			if ( m_pToolbarHandler )
			{
				// Set transport index to the specified name
				CString strName = bstrName;
				m_pToolbarHandler->RemoveTransport( pITransportRegEntry );
				m_pToolbarHandler->AddTransport( pITransportRegEntry );
				if( m_rpActive == pITransportRegEntry )
				{
					m_pToolbarHandler->SetActiveTransport( pITransportRegEntry );
				}
			}
			break;
		}
	}
	SysFreeString( bstrName );
	if ( fTransportFound )
	{
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}

/*======================================================================================
METHOD:  ICONDUCTOR::ISTRANSPORTPLAYING
========================================================================================
@method HRESULT | IDMUSProdConductor| IsTransportPlaying | Returns S_OK if <p pITransport> is playing, and
	S_FALSE if it is not.  If <p pITransport> is NULL, returns S_OK if nothing is playing, and
	S_FALSE if something is playing.

@comm This method will return S_OK if the Conductor thinks the <p pITransport> is the currently
	playing transport.  This may not be the case, so ideally transports should query
	<om IDirectMusicPerformance::IsPlaying>	with their segment state to truly know
	if they are playing or not.

@parm   IDMUSProdTransport* | pITransport | A pointer to the transport to check.  If NULL,
	check whether any transport is playing.

@rvalue S_OK | If <p pITransport> is not NULL, the transport is playing.  Otherwise, no transport is playing.
@rvalue S_FALSE | If <p pITransport> is not NULL, the transport is not playing.  Otherwise, a transport is playing.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>,
	   <om IDMUSProdConductor.UnRegisterTransport>, <om IDMUSProdConductor.SetActiveTransport>,
	   <om IDMUSProdConductor.SetTransportName>, <om IDMUSProdConductor.TransportStopped>,
	   <i IDMUSProdTransport>
	
--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::IsTransportPlaying( IDMUSProdTransport *pTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( pTransport == NULL )
    {
        return m_rpPlaying == NULL ? S_OK : S_FALSE;
    }
    if( m_rpPlaying != NULL )
	{
        IDMUSProdTransport *pRegTransport;
        m_rpPlaying->GetTransport( &pRegTransport );
        return pRegTransport == pTransport ? S_OK : S_FALSE;
	}
    return S_FALSE;
}

/*======================================================================================
METHOD:  ICONDUCTOR::PLAYMIDIEVENT
========================================================================================
@method HRESULT | IDMUSProdConductor| PlayMIDIEvent | Play a MIDI event now, or in the future.

@comm	The MIDI event is played on PChannel 0.

@parm   BYTE | bStatus | Status byte
@parm   BYTE | bData1 | Data byte 1
@parm   BYTE | bData2 | Data byte 2
@parm   DWORD | dwTime | The number of milliseconds into the future to play this
	event.  If zero, play the event as soon as possible.

@rvalue S_OK | The event was successfully played or queued.
@rvalue E_UNEXPECTED | The Conductor was unable to initialize DirectMusic.

@xref  <i IDMUSProdConductor>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::PlayMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2, DWORD dwTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( m_pDMPerformance != NULL );
	if ( m_pDMPerformance == NULL )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr;
	DMUS_PMSG *pPipelineEvent = NULL;

	DMUS_MIDI_PMSG *pDMMidiEvent = NULL;
	hr = m_pDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent );
	if ( FAILED(hr) )
	{
		return hr;
	}
	memset( pDMMidiEvent, 0, sizeof(DMUS_MIDI_PMSG) );

	pDMMidiEvent->bStatus = bStatus;
	pDMMidiEvent->bByte1 = bData1;
	pDMMidiEvent->bByte2 = bData2;
	if ( dwTime == 0 )
	{
		//pDMMidiEvent->m_rtTime = 0;
	}
	else
	{
		REFERENCE_TIME rtNow;
		m_pDMPerformance->GetLatencyTime( &rtNow );
		pDMMidiEvent->rtTime = dwTime * 10000 + rtNow;
	}
	pDMMidiEvent->dwFlags = DMUS_PMSGF_REFTIME;
	//pDMMidiEvent->dwPChannel = 0;
	pDMMidiEvent->dwVirtualTrackID = 1;
	pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;
	pPipelineEvent = (DMUS_PMSG*) pDMMidiEvent;

	return m_pDMPerformance->SendPMsg( pPipelineEvent );
}

/*======================================================================================
METHOD:  ICONDUCTOR::SETBTNSTATES
========================================================================================
@method HRESULT | IDMUSProdConductor| SetBtnStates | Sets the button states of <p pITransport> 
	according to the flags in <p dwFlags>.  The method is typically only used if the transport
	is not using the DirectMusic performance engine for playback, or if the transport needs to
	enable or disable the record button.

@parm   IDMUSProdTransport* | pITransport | A pointer to the transport to change the button states for.
@parm	DWORD | dwFlags | A combination of <t ButtonStateFlags> determining the state of the
		transport buttons.

@rvalue S_OK | The button states were successfully set.
@rvalue E_POINTER | <p pITransport> is NULL.
@rvalue E_INVALIDARG | <p pITransport> is not the currently active or currently playing transport.

@xref  <i IDMUSProdConductor>, <i IDMUSProdTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetBtnStates( IDMUSProdTransport *pTransport, DWORD btnFlags )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( pTransport == NULL )
	{
		return E_POINTER;
	}

	IDMUSProdTransport* pITransport = NULL;

	DWORD dwFlags;
	BOOL fFound = FALSE;
    if( m_rpPlaying != NULL )
	{
		m_rpPlaying->GetTransport( &pITransport );
		if (pITransport == pTransport)
		{
			fFound = TRUE;
			m_rpPlaying->GetFlags( &dwFlags );
		}
	}
    if( m_rpActive != NULL )
	{
		m_rpActive->GetTransport( &pITransport );
		if (pITransport == pTransport)
		{
			fFound = TRUE;
			m_rpActive->GetFlags( &dwFlags );
		}
	}
	if (!fFound)
	{
		// The transport must either be the active transport, or currently playing to
		// be able to set the button states
		return E_INVALIDARG;
	}

	if (btnFlags & BS_PLAY_ENABLED)
	{
		dwFlags |= B_PLAY_ENABLED;
	}
	else
	{
		dwFlags &= ~B_PLAY_ENABLED;
	}
	if (btnFlags & BS_PLAY_CHECKED)
	{
		dwFlags |= B_PLAY_CHECKED;
	}
	else
	{
		dwFlags &= ~B_PLAY_CHECKED;
	}
	
	if (btnFlags & BS_STOP_ENABLED)
	{
		dwFlags |= B_STOP_ENABLED;
	}
	else
	{
		dwFlags &= ~B_STOP_ENABLED;
	}
	if (btnFlags & BS_STOP_CHECKED)
	{
		dwFlags |= B_STOP_CHECKED;
	}
	else
	{
		dwFlags &= ~B_STOP_CHECKED;
	}

	if (btnFlags & BS_RECORD_ENABLED)
	{
		dwFlags |= B_REC_ENABLED;
	}
	else
	{
		dwFlags &= ~B_REC_ENABLED;
	}
	if (btnFlags & BS_RECORD_CHECKED)
	{
		dwFlags |= B_REC_CHECKED;
	}
	else
	{
		dwFlags &= ~B_REC_CHECKED;
	}

	if (btnFlags & BS_NO_AUTO_UPDATE)
	{
		dwFlags |= B_NOT_USING_ENGINE;
	}
	else
	{
		dwFlags &= ~B_NOT_USING_ENGINE;
	}

	if( m_rpPlaying != NULL )
	{
		m_rpPlaying->GetTransport( &pITransport );
		if (pITransport == pTransport)
		{
			m_rpPlaying->SetFlags( dwFlags );
		}
	}
	if( m_rpActive != NULL )
	{
		m_rpActive->GetTransport( &pITransport );
		if (pITransport == pTransport)
		{
			m_rpActive->SetFlags( dwFlags );
		}
	}
	if ( m_pToolbarHandler )
	{
		m_pToolbarHandler->SetStateAuto();
	}
	return S_OK;
}

/*======================================================================================
METHOD:  ICONDUCTOR::SETTEMPO
========================================================================================
@method HRESULT | IDMUSProdConductor| SetTempo | Sets the tempo displayed in the toolbar if
	<p pITransport> is the transport that currently "owns" the toolbar.

@parm	IDMUSProdTransport* | pITransport | A pointer to the transport that currently "owns" the toolbar.
@parm	double | dblTempo | The tempo to change to, in the range DMUS_TEMPO_MIN to DMUS_TEMPO_MAX. 
@parm	BOOL | fEnable | If TRUE, enable editing of the tempo.  If FALSE, disable editing of the tempo.

@comm	If <p fEnable> is FALSE and <p dblTempo> is negative, the tempo edit box will be
	display as empty and disabled.<nl>
	If a transport is currently playing, that transport "owns" the toolbar.  If no transport is currently
	playing, the active transport "owns" the toolbar.

@rvalue S_OK | The tempo was successfully set.
@rvalue E_POINTER | <p pITransport> is NULL.
@rvalue E_INVALIDARG | The transport does not currently "own" the toolbar, or <p dblTempo>
	is out of range.

@xref  <i IDMUSProdConductor>, <i IDMUSProdTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetTempo( IDMUSProdTransport *pTransport, double dblTempo, BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pTransport == NULL )
	{
		return E_POINTER;
	}

	if( !((dblTempo >= DMUS_TEMPO_MIN && dblTempo <= DMUS_TEMPO_MAX)
		  || (!fEnable && dblTempo < 0.0)) )
	{
		return E_INVALIDARG;
	}

	IDMUSProdTransport *pITransport;
	ITransportRegEntry *pDisplayedRegEntry = GetDisplayedTransport();
    if( pDisplayedRegEntry != NULL )
	{
		pDisplayedRegEntry->GetTransport( &pITransport );
		if ((pITransport == pTransport) && (m_dblTempo != dblTempo))
		{
			m_dblTempo = dblTempo;
			if (m_pToolbarHandler && m_pToolbarHandler->m_editTempo.GetSafeHwnd())
			{
				if(dblTempo >= DMUS_TEMPO_MIN && dblTempo <= DMUS_TEMPO_MAX)
				{
					CString str;
					str.Format("%.2f",dblTempo);
					m_pToolbarHandler->m_editTempo.SetWindowText(str);
				}
				else
				{
					m_pToolbarHandler->m_editTempo.SetWindowText(NULL);
				}

				m_pToolbarHandler->m_editTempo.EnableWindow(fEnable);
				m_pToolbarHandler->m_spinTempo.EnableWindow(fEnable);
			}
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

/*======================================================================================
METHOD:  ICONDUCTOR::REGISTERNOTIFY
========================================================================================
@method HRESULT | IDMUSProdConductor| RegisterNotify | Adds a notification type to the list of
	types that should be sent to this <i IDMUSProdTransport> from the performance engine.

@parm   IDMUSProdNotifyCPt* | pINotifyCPt | The notification interface to send the notification event to.
@parm   REFGUID | rguidNotify | The guid corresponding to the type of notifications to send.

@comm	The parameter <p rguidNotify> is passed to the DirectMusic method
	<om IDirectMusicPerformance::AddNotificationType>.  Valid values are listed in the DirectX SDK
	documentation.

@rvalue S_OK | <p pINotifyCPt> was successfully registered to received the specified notifications.
@rvalue E_POINTER | <p pINotifyCPt> is NULL.
@rvalue E_UNEXPECTED | The Conductor was unable to initialize DirectMusic.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.UnregisterNotify>, <i IDMUSProdNotifyCPt>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::RegisterNotify( IDMUSProdNotifyCPt *pNotifyCPt, REFGUID guidNotify )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pNotifyCPt == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_pDMPerformance != NULL );
	if ( m_pDMPerformance == NULL )
	{
		return E_UNEXPECTED;
	}

	CNotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;

	::EnterCriticalSection( &m_csNotifyEntry );

	// Look through m_lstNotifyEntry for guidNotify
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, guidNotify ) )
		{
			// Found guidNotify, add pNotifyCPt to the end of the list
			fFound = TRUE;
			pNotifyEntry->m_lstNotifyCPt.AddTail( pNotifyCPt );
			//pNotifyCPt->AddRef();
		}
	}

	// Didn't find guidNotify, add a new CNotifyEntry to the end
	// of m_lstNotifyEntry and add the GUID to the performance
	if ( !fFound )
	{
		pNotifyEntry = new CNotifyEntry( pNotifyCPt, guidNotify );
		//pNotifyCPt->AddRef();
		m_lstNotifyEntry.AddTail( pNotifyEntry );
		m_pDMPerformance->AddNotificationType( guidNotify );
	}

	::LeaveCriticalSection( &m_csNotifyEntry );

	return S_OK;
}

/*======================================================================================
METHOD:  ICONDUCTOR::UNREGISTERNOTIFY
========================================================================================
@method HRESULT | IDMUSProdConductor| UnregisterNotify | Removes a notification type from the list of
	notifications that should be sent to this <i IDMUSProdTransport>.

@parm   IDMUSProdNotifyCPt* | pINotifyCPt | The notification interface to stop sending notification events to
@parm   REFGUID | rguidNotify | The guid corresponding to the type of notifications to stop sending

@rvalue S_OK | <p pINotifyCPt> was successfully removed from receiving the specified events
@rvalue E_POINTER | <p pINotifyCPt> is NULL.
@rvalue E_UNEXPECTED | The Conductor was unable to initialize DirectMusic.
@rvalue E_INVALIDARG | <om IDMUSProdConductor.RegisterNotify> was not previously called for this
	<p pINotifyCPt> and <p rguidNotify>.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterNotify>, <i IDMUSProdNotifyCPt>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::UnregisterNotify( IDMUSProdNotifyCPt *pNotifyCPt, REFGUID guidNotify )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pNotifyCPt == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_pDMPerformance != NULL );
	if ( m_pDMPerformance == NULL )
	{
		return E_UNEXPECTED;
	}

	CNotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;

	::EnterCriticalSection( &m_csNotifyEntry );

	// Look through m_lstNotifyEntry for guidNotify
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, guidNotify ) )
		{
			// Found guidNotify, see if pNotifyCPt is in the list
			POSITION pos2;
			pos2 = pNotifyEntry->m_lstNotifyCPt.Find( pNotifyCPt );

			if ( pos2 != NULL )
			{
				// Found pNotifyCPt in the list, remove it
				fFound = TRUE;
				pNotifyEntry->m_lstNotifyCPt.RemoveAt( pos2 );

				if ( pNotifyEntry->m_lstNotifyCPt.IsEmpty() )
				{
					// If the list is empty, remove it from m_lstNotify Entry and remove
					// guidNotify from the list of notifications in the performance
					pos2 = m_lstNotifyEntry.Find( pNotifyEntry );
					ASSERT( pos2 != NULL );
					if ( pos2 != NULL )
					{
						m_lstNotifyEntry.RemoveAt( pos2 );
						delete pNotifyEntry;
					}
					// Don't remove segment or measurebeat notifications, since we use them to
					// update the toolbar's buttons and for the metronome
					if ( !InlineIsEqualGUID( GUID_NOTIFICATION_SEGMENT, guidNotify ) &&
						 !InlineIsEqualGUID( GUID_NOTIFICATION_MEASUREANDBEAT, guidNotify ) &&
						 !InlineIsEqualGUID( GUID_NOTIFICATION_PERFORMANCE, guidNotify ))
					{
						m_pDMPerformance->RemoveNotificationType( guidNotify );
					}
				}
			}
		}
	}

	::LeaveCriticalSection( &m_csNotifyEntry );

	if ( !fFound )
	{
		return E_INVALIDARG;
	}
	else
	{
		return S_OK;
	}
}

/*======================================================================================
METHOD:  ICONDUCTOR::TRANSPORTSTOPPED
========================================================================================
@method HRESULT | IDMUSProdConductor| TransportStopped | Notifies the Conductor that a transport has stopped playing.

@parm   IDMUSProdTransport* | pITransport | A pointer to the transport that has stopped playing

@comm	This method must be called when a transport stops playing so the Transport Control toolbar
	can switch from displaying a Stop button to displaying a Play button.

@rvalue S_OK | The operation succeeded.
@rvalue E_POINTER | <p pITransport> is NULL.
@rvalue E_INVALIDARG | <p pITransport> does not point to the currently playing transport.
@rvalue E_FAIL | An error occurred.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>,
	   <om IDMUSProdConductor.UnRegisterTransport>, <om IDMUSProdConductor.SetActiveTransport>,
	   <om IDMUSProdConductor.SetTransportName>, <om IDMUSProdConductor.IsTransportPlaying>,
	   <i IDMUSProdTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::TransportStopped( IDMUSProdTransport *pTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( pTransport == NULL )
    {
		return E_POINTER;
    }

	if( m_rpPlaying == NULL )
	{
		return E_INVALIDARG;
	}


	IDMUSProdTransport*  pTmpTransport;
	m_rpPlaying->GetTransport( &pTmpTransport );
	if( pTmpTransport != pTransport )
	{
		return E_INVALIDARG;
	}

	// Pop up the record button
	DWORD dwFlags;
	m_rpPlaying->GetFlags( &dwFlags );
	if( ((dwFlags & B_REC_CHECKED) != 0) && SUCCEEDED( pTransport->Record( FALSE ) ) )
	{
		dwFlags &= ~B_REC_CHECKED;
		m_rpPlaying->SetFlags( dwFlags );
		//SetStateAuto(); // Happens later.
	}

	m_rpPlaying = NULL;
	if ( m_pToolbarHandler )
	{
		// 22125: Can't send window messages - we may be in a notification thread
		if( m_pToolbarHandler->m_comboActive.GetSafeHwnd() != NULL )
		{
			::PostMessage( m_pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_ACTIVE_FROM_POINTER ), (LPARAM) m_pToolbarHandler->m_hWndToolbar );
			::PostMessage( m_pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) m_pToolbarHandler->m_hWndToolbar );
		}
	}

	// Flag the wave record toolbar that playback has stopped, so it should stop
	// exporting MIDI
	if( m_pWaveRecordToolbar )
	{
		m_pWaveRecordToolbar->PostMessage( WM_USER + 3, NULL, NULL );
	}

    return S_OK;
}


/*======================================================================================
METHOD:  ICONDUCTOR::GETTRANSITIONOPTIONS
========================================================================================
@method HRESULT | IDMUSProdConductor| GetTransitionOptions | Returns a structure containing parameters
			defining what type of transition the user desires.

@comm	This method should be called in response to a call to the <om IDMUSProdTransport.Transition> method.

@parm   ConductorTransitionOptions* | pTransitionOptions | A pointer to the <t ConductorTransitionOptions> structure to fill in

@rvalue S_OK | The operation succeeded.
@rvalue E_POINTER | <p pTransitionOptions> is NULL.

@xref  <i IDMUSProdConductor>, <t ConductorTransitionOptions>, <om IDMUSProdTransport.Transition>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::GetTransitionOptions( ConductorTransitionOptions *pTransitionOptions )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !pTransitionOptions )
	{
		return E_POINTER;
	}

	pTransitionOptions->dwBoundaryFlags = m_TransitionOptions.dwBoundaryFlags;
	pTransitionOptions->wPatternType = m_TransitionOptions.wPatternType;
	pTransitionOptions->dwFlags = m_TransitionOptions.dwFlags & ~TRANS_PRIVATE_FLAGS;
	pTransitionOptions->pDMUSProdNodeSegmentTransition = m_TransitionOptions.pDMUSProdNodeSegmentTransition;

	return S_OK;
}


/*======================================================================================
METHOD:  ICONDUCTOR::REGISTERSECONDARYTRANSPORT
========================================================================================
@method HRESULT | IDMUSProdConductor| RegisterSecondaryTransport | Add <p pISecondaryTransport>
	to the Secondary Segment toolbar's combo boxes. 	If <p pISecondaryTransport> is already
	listed, increment its reference count.

@parm   <i IDMUSProdSecondaryTransport>* | pISecondaryTransport | Pointer to the <i IDMUSProdSecondaryTransport> 
		interface to add to the Secondary Segment toolbar's combo boxes.

@rvalue S_OK | The secondary segment transport was successfully added.
@rvalue E_POINTER | <p pISecondaryTransport> is NULL.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.UnRegisterSecondaryTransport>,
	   <om IDMUSProdConductor.SetSecondaryTransportName>, <om IDMUSProdConductor.IsSecondaryTransportPlaying>,
	   <i IDMUSProdSecondaryTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::RegisterSecondaryTransport( IDMUSProdSecondaryTransport *pSecondaryTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	POSITION pos = m_lstISecondaryTransportRegEntry.GetHeadPosition();
	IDMUSProdSecondaryTransport*  pSecondaryTrans;
	while( pos )
	{
		POSITION pos2 = pos;
		m_lstISecondaryTransportRegEntry.GetNext( pos )->GetSecondaryTransport( &pSecondaryTrans );
		if( pSecondaryTrans == pSecondaryTransport )
		{
			// Increment reference count on existing entry
			m_lstISecondaryTransportRegEntry.GetAt( pos2 )->AddRef();
			return S_OK;
		}
	}

	// Transport was not found so create and add one
	CSecondaryTransportRegEntry*  pCEntry = new CSecondaryTransportRegEntry( pSecondaryTransport );
	if( pCEntry )
	{
		ISecondaryTransportRegEntry*  pIEntry;
		if( SUCCEEDED(pCEntry->QueryInterface( IID_ISecondaryTransportRegEntry, (void**) &pIEntry )) )
		{
			m_lstISecondaryTransportRegEntry.AddTail( pIEntry );

			BSTR  pbstrName;
			char buf[128];
			// If the transport doens't have a name, use a default name
			if ( SUCCEEDED(	pSecondaryTransport->GetSecondaryTransportName( &pbstrName ) ) )
			{
				WideCharToMultiByte( CP_ACP, 0, pbstrName, -1, buf, 128, NULL, NULL );
				SysFreeString( pbstrName );
			}
			else
			{
				CString strTmp;
				strTmp.LoadString( IDS_DEFAULT_TRANSPORT_NAME );
				strncpy( buf, strTmp, 127 );
			}

			if ( !m_lstSecondaryToolbars.IsEmpty() )
			{
				pos = m_lstSecondaryToolbars.GetHeadPosition();
				while( pos )
				{
					CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

					int nIndex;
					for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
					{
						nIndex = pToolbarHandler->m_arrayButtons[i]->comboActive.AddString( buf );
						pToolbarHandler->m_arrayButtons[i]->comboActive.SetItemDataPtr( nIndex, pIEntry );
					}

					pToolbarHandler->SetStateAuto();
				}
			}

			return S_OK;
		}
	}
	return E_FAIL;
}


/*======================================================================================
METHOD:  ICONDUCTOR::UNREGISTERSECONDARYTRANSPORT
========================================================================================
@parm   IDMUSProdTransport* | pITransport | Pointer to the <i IDMUSProdTransport> interface to
		be removed from the Transport Control toolbar's combo box.

@method HRESULT | IDMUSProdConductor| UnRegisterSecondaryTransport | Remove <p pISecondaryTransport>
	from the Secondary Segment toolbar's combo boxes, and stop it if it is playing.  If it was
	added more than once, remove one reference to it.  When all references are removed,
	it will then be removed from the Secondary Segment toolbar's combo boxes and stopped
	if it is playing.

@parm   IDMUSProdSecondaryTransport* | pISecondaryTransport | Pointer to the <i IDMUSProdSecondaryTransport> 
		interface to be removed from the Secondary Segment toolbar's combo boxes.

@rvalue S_OK | <p pISecondaryTransport> was successfully removed.
@rvalue E_INVALIDARG | <p pISecondaryTransport> was not previously added via <om IDMUSProdConductor.RegisterSecondaryTransport>.
@rvalue E_POINTER | <p pISecondaryTransport> is NULL.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterSecondaryTransport>,
	   <om IDMUSProdConductor.SetSecondaryTransportName>, <om IDMUSProdConductor.IsSecondaryTransportPlaying>,
	   <i IDMUSProdSecondaryTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::UnRegisterSecondaryTransport( IDMUSProdSecondaryTransport *pSecondaryTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pSecondaryTransport == NULL )
	{
		return E_POINTER;
	}

	POSITION pos = m_lstISecondaryTransportRegEntry.GetHeadPosition();
	IDMUSProdSecondaryTransport*  pTrans;
	while( pos )
	{
		POSITION pos2 = pos;
		m_lstISecondaryTransportRegEntry.GetNext( pos )->GetSecondaryTransport( &pTrans );
		if( pTrans == pSecondaryTransport )
		{
			// Check if this is the last reference
			ISecondaryTransportRegEntry *pISecondaryRegEntry = m_lstISecondaryTransportRegEntry.GetAt( pos2 );
			pISecondaryRegEntry->AddRef();
			if ( pISecondaryRegEntry->Release() == 1 )
			{
				pos = m_lstSecondaryToolbars.GetHeadPosition();
				while( pos )
				{
					CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

					for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
					{
						if( pISecondaryRegEntry == pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry )
						{
							// Fake a click on the Stop button so we stop this transport
							// if it's currently playing
							BOOL bHandled;
							pToolbarHandler->OnButtonClicked( 0, WORD(ID_TRANSP_STOP1 + i), 0, bHandled );
							if( pToolbarHandler->m_arrayButtons[i]->pISegmentState )
							{
								TRACE("UnRegisterSecondaryTransport: Transport #%d failed to Stop() when unregistered\n", i);
								RELEASE( pToolbarHandler->m_arrayButtons[i]->pISegmentState );
							}
						}
					}
				}
			}

			// If no more references, remove this secondary transport from all combo boxes
			if( pISecondaryRegEntry->Release() == 0 )
			{
				pos = m_lstSecondaryToolbars.GetHeadPosition();
				while( pos )
				{
					CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

					// Clear selections if the entry is currenly active, or if the active selection
					// is greater than the one being removed, decrease the index of the active selection
					for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
					{
						if( pISecondaryRegEntry == pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry )
						{
							pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry = NULL;
							pToolbarHandler->m_arrayButtons[i]->comboActive.SetCurSel( -1 );
						}

						// Remove string from combo box
						int nIndex = GetIndexByDataPtr( pToolbarHandler->m_arrayButtons[i]->comboActive, pISecondaryRegEntry );
						if( nIndex != -1 )
						{
							pToolbarHandler->m_arrayButtons[i]->comboActive.DeleteString( nIndex );
						}
					}


					for( i=0; i < MAX_BUTTONS; i++ )
					{
						if( pToolbarHandler->m_apOldButtonInfo[i] )
						{
							if( pISecondaryRegEntry == pToolbarHandler->m_apOldButtonInfo[i]->pActiveRegEntry )
							{
								pToolbarHandler->m_apOldButtonInfo[i]->pActiveRegEntry = NULL;
							}
						}
					}

					// Refresh the display
					pToolbarHandler->SetStateAuto();
				}

				// Remove entry from registry pointer array if no more references
				m_lstISecondaryTransportRegEntry.RemoveAt( pos2 );

				// If no secondary segment transports, need to disable all comboboxes in
				// the secondary segment toolbars
				if( m_lstISecondaryTransportRegEntry.IsEmpty() )
				{
					pos = m_lstSecondaryToolbars.GetHeadPosition();
					while( pos )
					{
						// This updates all the button states
						m_lstSecondaryToolbars.GetNext( pos )->SetStateAuto();
					}
				}
			}

			// return - found the secondary segment transport
			return S_OK;
		}
	}

	// Entry was not found
	return E_INVALIDARG;
}


/*======================================================================================
METHOD:  ICONDUCTOR::SETSECONDARYTRANSPORTNAME
========================================================================================
@method HRESULT | IDMUSProdConductor| SetSecondaryTransportName | Update the name displayed for
	<p pISecondaryTransport> with the text in <p bstrName>.

@parm   IDMUSProdSecondaryTransport* | pISecondaryTransport | Pointer to an <i IDMUSProdSecondaryTransport>
	interface.
@parm	BSTR | bstrName | Text to display in the Secondary Segment toolbar's combo boxes
	for <p pISecondaryTransport>.

@rvalue S_OK | Successfully set the name of <p pISecondaryTransport> to <p pbstrName>.
@rvalue E_POINTER | Either <p pISecondaryTransport> or <p bstrName> is NULL.
@rvalue E_INVALIDARG | <p pISecondaryTransport> was not previously added by calling <om IDMUSProdConductor.RegisterSecondaryTransport>.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterSecondaryTransport>,
	   <om IDMUSProdConductor.UnRegisterSecondaryTransport>, <om IDMUSProdConductor.IsSecondaryTransportPlaying>,
	   <i IDMUSProdSecondaryTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetSecondaryTransportName( IDMUSProdSecondaryTransport *pSecondaryTransport, BSTR bstrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( (pSecondaryTransport == NULL) || (bstrName == NULL) )
	{
		return E_POINTER;
	}

	IDMUSProdSecondaryTransport*  pTmpTransport = NULL;
	BOOL fTransportFound = FALSE;
	POSITION pos = m_lstISecondaryTransportRegEntry.GetHeadPosition();

	// Search for the trasnport in the registry
	while( pos )
	{
		POSITION pos2 = pos;
		m_lstISecondaryTransportRegEntry.GetNext( pos )->GetSecondaryTransport( &pTmpTransport );
		if( pTmpTransport == pSecondaryTransport )
		{
			// Found the transport
			fTransportFound = TRUE;

			// Set transport index to the specified name
			char buf[128];
			WideCharToMultiByte( CP_ACP, 0, bstrName, -1, buf, 128, NULL, NULL );

			// Save a pointer to the transport
			ISecondaryTransportRegEntry* pIEntry = m_lstISecondaryTransportRegEntry.GetAt( pos2 );

			// Update the secondary toolbar combo boxes
			POSITION pos2 = m_lstSecondaryToolbars.GetHeadPosition();
			while( pos2 )
			{
				CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos2 );

				for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
				{
					// Update combo box
					int nIndex = GetIndexByDataPtr( pToolbarHandler->m_arrayButtons[i]->comboActive, pIEntry );
					if( nIndex != -1 )
					{
						pToolbarHandler->m_arrayButtons[i]->comboActive.DeleteString( nIndex );
						nIndex = pToolbarHandler->m_arrayButtons[i]->comboActive.AddString( buf );
						pToolbarHandler->m_arrayButtons[i]->comboActive.SetItemDataPtr( nIndex, pIEntry );

						// Reset the active item
						pToolbarHandler->m_arrayButtons[i]->comboActive.SetCurSel( GetIndexByDataPtr( pToolbarHandler->m_arrayButtons[i]->comboActive, pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry ) );
					}
				}
			}
			break;
		}
	}

	SysFreeString( bstrName );

	if ( fTransportFound )
	{
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}


/*======================================================================================
METHOD:  ICONDUCTOR::ISSECONDARYTRANSPORTPLAYING
========================================================================================
@method HRESULT | IDMUSProdConductor| IsSecondaryTransportPlaying | Returns S_OK if
	<p pISecondaryTransport> is playing, and S_FALSE if it is not.  If <p pISecondaryTransport>
	is NULL, returns S_OK if nothing is playing, and S_FALSE if something is playing.

@parm   IDMUSProdSecondaryTransport* | pISecondaryTransport | A pointer to the secondary transport to check.
	If NULL, check whether anything is playing.

@rvalue S_OK | If <p pISecondaryTransport> is not NULL, the transport is playing.
	Otherwise, no transport is playing.
@rvalue S_FALSE | If <p pISecondaryTransport> is not NULL, the transport is not playing.
	Otherwise, a transport is playing.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterSecondaryTransport>,
	   <om IDMUSProdConductor.UnRegisterSecondaryTransport>, <om IDMUSProdConductor.SetSecondaryTransportName>,
	   <i IDMUSProdSecondaryTransport>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::IsSecondaryTransportPlaying( IDMUSProdSecondaryTransport *pSecondaryTransport )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// If pSecondary Transport is NULL, check if anything is playing
    if( pSecondaryTransport == NULL )
    {
		POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
		while( pos )
		{
			CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

			for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
			{
				if( pToolbarHandler->m_arrayButtons[i]->pISegmentState )
				{
					// Something's playing
					return S_OK;
				}
			}
		}
		// Nothing's playing
		return S_FALSE;
    }

	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

		for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
		{
			if( pToolbarHandler->m_arrayButtons[i]->pISegmentState && pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry )
			{
				IDMUSProdSecondaryTransport *pRegTransport;
				pToolbarHandler->m_arrayButtons[i]->pActiveRegEntry->GetSecondaryTransport( &pRegTransport );
				if( pRegTransport == pSecondaryTransport )
				{
					return S_OK;
				}
			}
		}
	}

	return S_FALSE;
}


/*======================================================================================
METHOD:  ICONDUCTOR::SETPCHANNELTHRU
========================================================================================
@method HRESULT | IDMUSProdConductor| SetPChannelThru | Starts a thru connection from the
		specified MIDI input channel to the specified output PChannel.

@parm   DWORD | dwInputChannel | MIDI Input channel number. Must be between 0 and 15, inclusive.
@parm   DWORD | dwPChannel | Output PChannel number.

@rvalue S_OK | The operation succeeded.
@rvalue S_FALSE | Already thruing <p dwInputChannel> to <p dwPChannel>
@rvalue E_UNEXPECTED | The Performance engine does not exist.
@rvalue E_FAIL | There is no DirectMusic MIDI input port, <p dwPChannel> does not exist in the
	current port configuration, or the port that <p dwPChannel> plays on has a latency greater
	than 40ms.
@rvalue E_INVALIDARG | <p dwInputChannel> is greater than 15.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.CancelPChannelThru>

--------------------------------------------------------------------------------------*/

HRESULT STDMETHODCALLTYPE CConductor::SetPChannelThru( DWORD dwInputChannel, DWORD dwPChannel )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( dwInputChannel > 15 )
	{
		return E_INVALIDARG;
	}

	::EnterCriticalSection( &m_csAudiopath );
	IDirectMusicAudioPath *pDMAudiopath = m_pDMAudiopath;
	if( pDMAudiopath )
	{
		pDMAudiopath->AddRef();
	}
	::LeaveCriticalSection( &m_csAudiopath );

	HRESULT hr = E_UNEXPECTED;

	if( m_pDMPerformance
	&&	pDMAudiopath
	&&	g_pMIDIInputContainer )
	{
		hr = g_pMIDIInputContainer->SetPChannelThru( dwInputChannel, dwPChannel, pDMAudiopath );
	}

	if( pDMAudiopath )
	{
		pDMAudiopath->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  ICONDUCTOR::CANCELPCHANNELTHRU
========================================================================================
@method HRESULT | IDMUSProdConductor| CancelPChannelThru | Stops the thru connection from the
		specified MIDI input channel.

@parm   DWORD | dwInputChannel | MIDI Input channel number. Must be between 0 and 15, inclusive.

@rvalue S_OK | The operation succeeded.
@rvalue S_FALSE | No PChannel Thru is already active on that input channel
@rvalue E_UNEXPECTED | The Performance engine does not exist.
@rvalue E_FAIL | There is no DirectMusic MIDI input port.
@rvalue E_INVALIDARG | <p dwInputChannel> is greater than 15.

@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.SetPChannelThru>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::CancelPChannelThru( DWORD dwInputChannel )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( dwInputChannel > 15 )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_UNEXPECTED;

	if( m_pDMPerformance
	&&	g_pMIDIInputContainer )
	{
		hr = g_pMIDIInputContainer->CancelPChannelThru( dwInputChannel );
	}

	return hr;
}


/*======================================================================================
METHOD:  ICONDUCTOR::IsTrackCursorEnabled
========================================================================================
@method HRESULT | IDMUSProdConductor| IsTrackCursorEnabled | Queries the state of the Transport Options toolbar's Track Cursor button.

@comm	When the Track Cursor button is down, the playing transport should display an indication
	of the current play position and the display should scroll to keep the current play position
	in view.  When the Track Cursor button is up, the display should not automatically scroll. 

@rvalue S_OK | The Track Cursor button is down (Time cursor should be tracked).
@rvalue S_FALSE | The Track Cursor button is up (Time cursor should not be tracked).
@rvalue E_FAIL | An error occurred.

@xref  <i IDMUSProdConductor>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::IsTrackCursorEnabled( )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_pOptionsToolbarHandler )
	{
		return E_FAIL;
	}

	return m_pOptionsToolbarHandler->m_fCursorEnabled ? S_OK : S_FALSE;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::RegisterAudiopath
========================================================================================
@method HRESULT | IDMUSProdConductor8| RegisterAudiopath | Registers an Audiopath node.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::RegisterAudiopath( IDMUSProdNode *pAudiopathNode, BSTR bstrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( bstrName == NULL )
	{
		return E_INVALIDARG;
	}
	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( pAudiopathNode == NULL )
	{
		return E_POINTER;
	}

	// Find the item in our list of Audiopaths
	POSITION pos = m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		CAudiopathListItem* pAPListItem = m_lstAudiopaths.GetNext( pos );
		if( pAPListItem->pNode == pAudiopathNode )
		{
			// Already exists!
			return E_INVALIDARG;
		}
	}

	// Create and add an item to our list of Audiopaths
	CAudiopathListItem* pAPListItem = new CAudiopathListItem( pAudiopathNode, 0, 0 );
	if( pAPListItem )
	{
		pAPListItem->strName = strName;
		m_lstAudiopaths.AddTail( pAPListItem );

		return S_OK;
	}
	return E_OUTOFMEMORY;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::UnRegisterAudiopath
========================================================================================
@method HRESULT | IDMUSProdConductor8| UnRegisterAudiopath | Unregisters an Audiopath node.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::UnRegisterAudiopath( IDMUSProdNode *pAudiopathNode )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pAudiopathNode == NULL )
	{
		return E_POINTER;
	}

	// Find the item in our list of Audiopaths
	POSITION pos = m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		POSITION posCurrent = pos;
		CAudiopathListItem* pAPListItem = m_lstAudiopaths.GetNext( pos );
		if( pAPListItem->pNode == pAudiopathNode )
		{
			// Check if the audiopath is in use
			if( m_pAudiopathListItem == pAPListItem )
			{
				// Yes - need to change to 'none'

				// Display an hourglass cursor
				CWaitCursor waitCursor;

				// Broadcast the WaveUnload notification
				BroadCastWaveNotification( GUID_ConductorUnloadWaves );

				// Set the old audio path to be inactive
				// Try and get the IDMUSProdAudioPathInUse interface for the node
				IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
				if( SUCCEEDED( pAudiopathNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
				{
					// Notify the Audiopath that it is no longer in use
					//pIDMUSProdAudiopathInUse->UsingAudioPath( pOldAudiopath, FALSE );
					pIDMUSProdAudiopathInUse->UsingAudioPath( m_pDMAudiopath, FALSE );
					pIDMUSProdAudiopathInUse->Release();
				}

				m_pFrameWork->RemoveFromNotifyList( pAudiopathNode, this );

				// Initialize the ListItem pointer to NULL
				m_pAudiopathListItem = NULL;

				// Ensure that there is no default audiopath
				// This also releases our pointer to the old audio path
				ActivateAudiopath( NULL );

				// Update the transport toolbar
				if( m_pToolbarHandler )
				{
					m_pToolbarHandler->UpdateActiveFromEngine();
					m_pToolbarHandler->SetStateAuto();
				}

				// Remove all items from the combo box, then add and select the Audiopath used by the conductor
				m_pOptionsToolbarHandler->UpdateComboBoxFromConductor();

				// Notify all other components that output has been changed
				BroadCastPortChangeNotification();
			}

			m_lstAudiopaths.RemoveAt( posCurrent );
			delete pAPListItem;
			return S_OK;
		}
	}

	return E_INVALIDARG;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::SetAudiopathName
========================================================================================
@method HRESULT | IDMUSProdConductor8| SetAudiopathName | Update the name of an Audiopath node.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetAudiopathName( IDMUSProdNode *pAudiopathNode, BSTR bstrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( bstrName == NULL )
	{
		return E_INVALIDARG;
	}
	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( pAudiopathNode == NULL )
	{
		return E_POINTER;
	}

	if( strName.IsEmpty() )
	{
		return E_INVALIDARG;
	}

	// Find the item in our list of Audiopaths
	POSITION pos = m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		CAudiopathListItem* pAPListItem = m_lstAudiopaths.GetNext( pos );
		if( pAPListItem->pNode == pAudiopathNode )
		{
			pAPListItem->strName = strName;

			if( m_pAudiopathListItem == pAPListItem )
			{
				m_pOptionsToolbarHandler->UpdateComboBoxFromConductor();
			}

			return S_OK;
		}
	}

	return E_INVALIDARG;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::GetDownloadCustomDLSStatus
========================================================================================
@method HRESULT | IDMUSProdConductor8| GetDownloadCustomDLSStatus | Get whether or not
	custom DLS collections should be downloaded.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::GetDownloadCustomDLSStatus( BOOL *pfDownloadCustomDLS )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pfDownloadCustomDLS == NULL )
	{
		return E_POINTER;
	}

	*pfDownloadCustomDLS = m_fDownloadDLS;

	return S_OK;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::GetDownloadGMStatus
========================================================================================
@method HRESULT | IDMUSProdConductor8| GetDownloadGMStatus | Get whether or not the
	standard GM collection should be downloaded.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::GetDownloadGMStatus( BOOL *pfDownloadGM )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pfDownloadGM == NULL )
	{
		return E_POINTER;
	}

	*pfDownloadGM = m_fDownloadGM;

	return S_OK;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::SetupMIDIAndWaveSave
========================================================================================
@method HRESULT | IDMUSProdConductor8| SetupMIDIAndWaveSave | Initialize the MIDI Save
	and Wave export objects in the given segment state.

@xref  <i IDMUSProdConductor8>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::SetupMIDIAndWaveSave( IUnknown *punkSegmentState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( punkSegmentState )
	{
		IDirectMusicSegment8 *pIDirectMusicSegment8;
		IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
		if( SUCCEEDED( punkSegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void**)&pIDirectMusicSegmentState8 ) ) )
		{
			AddToolsAndSetupWaveSaveForSegState( punkSegmentState );
			pIDirectMusicSegmentState8->Release();
			return S_OK;
		}
		else if( SUCCEEDED( punkSegmentState->QueryInterface( IID_IDirectMusicSegment8, (void**)&pIDirectMusicSegment8 ) ) )
		{
			IUnknown *punkAudiopathConfig;
			if( SUCCEEDED( pIDirectMusicSegment8->GetAudioPathConfig( &punkAudiopathConfig ) ) )
			{
				IDirectMusicGraph* pGraph;
				if( SUCCEEDED( pIDirectMusicSegment8->GetGraph( &pGraph ) ) )
				{
					AddToolsToToolgraph( pGraph, m_pOutputTool, m_pMIDISaveTool );
					pGraph->Release();
				}
				else if( SUCCEEDED( ::CoCreateInstance( CLSID_DirectMusicGraph, NULL, CLSCTX_INPROC_SERVER, IID_IDirectMusicGraph, (void **)&pGraph ) ) )
				{
					if( SUCCEEDED( pIDirectMusicSegment8->SetGraph( pGraph ) ) )
					{
						AddToolsToToolgraph( pGraph, m_pOutputTool, m_pMIDISaveTool );
					}
					pGraph->Release();
				}
				punkAudiopathConfig->Release();
			}
			pIDirectMusicSegment8->Release();
			return S_OK;
		}
	}

	return E_POINTER;
}


/*======================================================================================
METHOD:  ICONDUCTOR8::GetAudioParams
========================================================================================
@method HRESULT | IDMUSProdConductor8| GetAudioParams | Fill out the <p pDMUSAudioParams>
	structure with the current performance's audio parameters.

@parm   <t DMUS_AUDIOPARAMS> | pDMUSAudioParams | Pointer to a <t DMUS_AUDIOPARAMS> structure.

@rvalue S_OK | The operation succeeded.
@rvalue E_POINTER | <p pDMUSAudioParams> is NULL.
@rvalue E_INVALIDARG | The dwSize member of <p pDMUSAudioParams> is invalid.

@xref  <i IDMUSProdConductor8>, <t DMUS_AUDIOPARAMS>

--------------------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE CConductor::GetAudioParams( DMUS_AUDIOPARAMS *pDMUSAudioParams )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( pDMUSAudioParams == NULL )
	{
		return E_POINTER;
	}

	ASSERT( pDMUSAudioParams->dwSize == sizeof( DMUS_AUDIOPARAMS ) );
	if( pDMUSAudioParams->dwSize != sizeof( DMUS_AUDIOPARAMS ) )
	{
		return E_INVALIDARG;
	}

	pDMUSAudioParams->fInitNow = FALSE;
	pDMUSAudioParams->dwValidData = DMUS_AUDIOPARAMS_VOICES | DMUS_AUDIOPARAMS_SAMPLERATE | DMUS_AUDIOPARAMS_FEATURES | DMUS_AUDIOPARAMS_DEFAULTSYNTH;
	pDMUSAudioParams->dwFeatures = 0;
	pDMUSAudioParams->dwVoices = m_dwVoices;
	pDMUSAudioParams->dwSampleRate = m_dwSampleRate;
	pDMUSAudioParams->clsidDefaultSynth = m_clsidDefaultSynth;

	// The capabilities of the default synth
	DWORD dwDefaultSynthFlags = 0;

	// Initialize dwDefaultSynthFlags
	if( GUID_Synth_Default == m_clsidDefaultSynth )
	{
		dwDefaultSynthFlags = DMUS_PC_DLS | DMUS_PC_SOFTWARESYNTH | DMUS_PC_DIRECTSOUND | DMUS_PC_DLS2 | DMUS_PC_AUDIOPATH | DMUS_PC_WAVE;
	}
	else
	{
		// Try and find the portcaps for the default synth
		DMUS_PORTCAPS dmPortCaps;
		if( GetPortCaps( m_pDMusic, m_clsidDefaultSynth, &dmPortCaps ) )
		{
			dwDefaultSynthFlags = dmPortCaps.dwFlags;
		}
		else
		{
			// Port doesn't exist - fallback to the default synth
			m_clsidDefaultSynth = GUID_Synth_Default;
			dwDefaultSynthFlags = DMUS_PC_DLS | DMUS_PC_SOFTWARESYNTH | DMUS_PC_DIRECTSOUND | DMUS_PC_DLS2 | DMUS_PC_AUDIOPATH | DMUS_PC_WAVE;
		}
	}

	// If the default synth supports audiopaths
	if( dwDefaultSynthFlags & DMUS_PC_AUDIOPATH )
	{
		// If the default synth does not support waves, clear the DMUS_AUDIOF_STREAMING flag
		if( !(dwDefaultSynthFlags & DMUS_PC_WAVE) )
		{
			pDMUSAudioParams->dwFeatures = (DMUS_AUDIOF_ALL ^ DMUS_AUDIOF_STREAMING);
		}
		else
		{
			// Otherwise, request all features
			pDMUSAudioParams->dwFeatures = DMUS_AUDIOF_ALL;
		}
	}

	// If the default synth is not specified, clear the DMUS_AUDIOPARAMS_DEFAULTSYNTH flag
	if( GUID_Synth_Default == pDMUSAudioParams->clsidDefaultSynth )
	{
		pDMUSAudioParams->dwValidData &= ~DMUS_AUDIOPARAMS_DEFAULTSYNTH;
	}

	return S_OK;
}


// IDMUSProdNotifySink
HRESULT STDMETHODCALLTYPE CConductor::OnUpdate( IDMUSProdNode *pIDocRootNode, GUID guidUpdateType, void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(pData);

	if( pIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	if( AUDIOPATH_NeedToRebuildNotification != guidUpdateType )
	{
		return E_INVALIDARG;
	}

	if( pData == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	DWORD dwFlags = *((DWORD *)pData);

	// Find the item in our list of Audiopaths
	POSITION pos = m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		CAudiopathListItem* pAPListItem = m_lstAudiopaths.GetNext( pos );
		if( pAPListItem->pNode == pIDocRootNode )
		{
			// Check if the audiopath is in use
			if( m_pAudiopathListItem == pAPListItem )
			{
				// Display an hourglass cursor
				CWaitCursor waitCursor;

				// Try and get the IDMUSProdAudioPathInUse interface for the node
				IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
				if( SUCCEEDED( pIDocRootNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
				{
					// Notify the Audiopath that the old audiopath is no longer in use
					pIDMUSProdAudiopathInUse->UsingAudioPath( m_pDMAudiopath, FALSE );
					pIDMUSProdAudiopathInUse->Release();
				}

				CTypedPtrList<CPtrList, CDownloadedInstrumentList*> lstDownloadedInstrumentList;
				while( !m_lstDownloadedInstrumentList.IsEmpty() )
				{
					lstDownloadedInstrumentList.AddTail( m_lstDownloadedInstrumentList.RemoveHead() );
				}

				// Broadcast the WaveUnload notification
				if( dwFlags & AUDIOPATH_UNLOAD_DOWNLOAD_WAVES )
				{
					BroadCastWaveNotification( GUID_ConductorUnloadWaves );
				}

				// Remove the existing audiopath - this ensures that the 'shared'
				// buffers are created properly
				ActivateAudiopath( NULL, false );

				// Create the new audiopath
				IDirectMusicAudioPath *pAudiopath = NULL;
				HRESULT hrCreate = CreateAudiopath( m_pAudiopathListItem, &pAudiopath );

				// Activate the Audio path (if creation failed, this will set the active audiopath to 'none'
				HRESULT hrActivate = ActivateAudiopath( pAudiopath, false );

				// Flag whether or not to send the port changed notification
				bool fSendPortChangedNotify = false;

				// If either activation or creation of the new audiopath failed
				if( FAILED( hrCreate )
				||	FAILED( hrActivate ) )
				{
					// Broadcast the WaveFlush notification
					BroadCastWaveNotification( GUID_ConductorFlushWaves );

					// Unload the instruments from the old audiopath
					while( !lstDownloadedInstrumentList.IsEmpty() )
					{
						delete lstDownloadedInstrumentList.RemoveHead();
					}

					// Remove from the audiopath's notification list
					m_pFrameWork->RemoveFromNotifyList( pIDocRootNode, g_pconductor );

					// Initialize the ListItem pointer to NULL
					m_pAudiopathListItem = NULL;

					// Ensure that no audiopath is set as the default
					// This will also release our pointer to the existing audio path
					ActivateAudiopath( NULL, true );

					// Need to send the port changed notification
					fSendPortChangedNotify = true;
				}
				else
				{
					// Otherwise, keep the ListItem pointer the same
					// Notify the Audiopath node that it now in use
					// Try and get the IDMUSProdAudioPathInUse interface for the node
					if( SUCCEEDED( m_pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
					{
						pIDMUSProdAudiopathInUse->UsingAudioPath( pAudiopath, TRUE );
						pIDMUSProdAudiopathInUse->Release();
					}
					
					// Check if we need to unload and download the instruments
					// Make a list of the ports in the current audiopath
					CTypedPtrList<CPtrList, IDirectMusicPort*> lstPorts;
					IDirectMusicPort *pDMPort = NULL;

					// Iterate through the default Audiopath's port
					DMUS_PORTCAPS dmPortCaps;
					DWORD dwIndex = 0;
					while( S_OK == pAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
					{
						// Verify we have a valid port pointer
						ASSERT( pDMPort );

						// Initialize the DMUS_PORTCAPS structure
						dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);

						// Try and get the capabilities of this port, and check if it supports DLS
						// and doesn't already have GS in hardware
						if( SUCCEEDED( pDMPort->GetCaps( &dmPortCaps ) )
						&&	(dmPortCaps.dwFlags & DMUS_PC_DLS)
						&&	!(dmPortCaps.dwFlags & DMUS_PC_GSINHARDWARE) )
						{
							lstPorts.AddTail( pDMPort );
							pDMPort->AddRef();
						}

						// Release the port
						pDMPort->Release();

						// Go on to the next port in the audio path
						dwIndex++;
					}

					// Iterate through the list of downloaded instrument lists
					POSITION posDL = lstDownloadedInstrumentList.GetHeadPosition();
					while( posDL )
					{
						// Save pointer to our current location
						const POSITION posCurrentDL = posDL;

						// Get a pointer to each downloaded instrument list
						CDownloadedInstrumentList *pCDownloadedInstrumentList = lstDownloadedInstrumentList.GetNext( posDL );

						// Iterate through all the ports in the new audiopath
						POSITION posPort = lstPorts.GetHeadPosition();
						while( posPort )
						{
							// Save a pointer to our current port position
							const POSITION posCurrentPort = posPort;

							// Get a poitner to each port
							pDMPort = lstPorts.GetNext( posPort );

							// Check if this is the port we're looking for
							if( pDMPort == pCDownloadedInstrumentList->m_pDMPort )
							{
								// yes - add it back to the mail downloaded instruments list
								m_lstDownloadedInstrumentList.AddTail( pCDownloadedInstrumentList );

								// Remove it from our private downloaded instruments list
								lstDownloadedInstrumentList.RemoveAt( posCurrentDL );

								// Remove the port from lstPorts
								lstPorts.RemoveAt( posCurrentPort );

								// Release our pointer to the port
								pDMPort->Release();

								break;
							}
						}
					}

					if( !lstDownloadedInstrumentList.IsEmpty()
					||	!lstPorts.IsEmpty() )
					{
						while( !lstDownloadedInstrumentList.IsEmpty() )
						{
							delete lstDownloadedInstrumentList.RemoveHead();
						}
						while( !lstPorts.IsEmpty() )
						{
							lstPorts.RemoveHead()->Release();
						}

						// Send the port removal notification
						BroadCastPortRemovalNotification();

						// Download GM
						DownOrUnLoadGM();

						// Need to send the port changed notification
						fSendPortChangedNotify = true;
					}
				}

				// Release our pointer to the new audio path
				if( pAudiopath )
				{
					pAudiopath->Release();
				}

				// Update the transport toolbar
				if( m_pToolbarHandler )
				{
					m_pToolbarHandler->UpdateActiveFromEngine();
					m_pToolbarHandler->SetStateAuto();
				}

				// Remove all items from the combo box, then add and select the Audiopath used by the conductor
				m_pOptionsToolbarHandler->UpdateComboBoxFromConductor();

				// Broadcast the WaveDownload notification
				if( dwFlags & AUDIOPATH_UNLOAD_DOWNLOAD_WAVES )
				{
					BroadCastWaveNotification( GUID_ConductorDownloadWaves );
				}

				// Notify all other components that output has been changed
				if( fSendPortChangedNotify )
				{
					BroadCastPortChangeNotification();
				}

				// If either activation or creation of the new audiopath failed
				if( FAILED( hrCreate )
				||	FAILED( hrActivate ) )
				{
					ASSERT( m_pAudiopathListItem == NULL );

					// Iterate through all registered audiopaths
					POSITION pos = m_lstAudiopaths.GetHeadPosition();
					while( pos )
					{
						CAudiopathListItem *pAPListItem = m_lstAudiopaths.GetNext( pos );

						if( pAPListItem->dwStandardID == DMUS_APATH_SHARED_STEREOPLUSREVERB
						&&	pAPListItem->pNode == NULL )
						{
							// Add the text to the combo box
							int nStringIndex = m_pOptionsToolbarHandler->m_comboAudiopath.AddString( pAPListItem->strName );

							// Create a new AudiopathListItem to store information about this Audiopath in
							// Set the combobox item to point to this AudiopathListItem
							m_pOptionsToolbarHandler->m_comboAudiopath.SetItemDataPtr( nStringIndex, pAPListItem );

							m_pOptionsToolbarHandler->m_comboAudiopath.SetCurSel( nStringIndex );
							break;
						}
					}

					BOOL bHandled = FALSE;
					m_pOptionsToolbarHandler->OnCloseUpComboAudiopath( 0, 0, 0, bHandled );
				}
			}

			return S_OK;
		}
	}

	return E_INVALIDARG;
}


// IDMUSProdComponent
HRESULT STDMETHODCALLTYPE CConductor::Initialize( IDMUSProdFramework __RPC_FAR* pFrameWork, BSTR __RPC_FAR* pbstrErrorText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( pbstrErrorText != NULL )
    {
        *pbstrErrorText = NULL;
    }
    if( pFrameWork == NULL )
    {
        return E_INVALIDARG;
    }
    if( m_pFrameWork != NULL )
    {
		return S_OK;  // only initialize once
	}

	HRESULT hr;

	// Verify we're running on the DX8 framework
	hr = pFrameWork->QueryInterface( IID_IDMUSProdFramework8, (void **)&m_pFrameWork );
	if( m_pFrameWork == NULL )
	{
		CString strError;
		strError.LoadString( IDS_ERR_NEED_DX8_FRAMEWORK );
		*pbstrErrorText = strError.AllocSysString();
		return hr;
	}

	// create an invisible window that is used to catch toolbar messages
	CRect rect( 0, 0, 0, 0 );
	CWnd* pWndParent = new CWnd;
	if( !pWndParent )
	{
		ReleaseAll();
		return E_OUTOFMEMORY;
	}
	if( !pWndParent->CreateEx(0, AfxRegisterWndClass(0), NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL) )
	{
		delete pWndParent;
		ReleaseAll();
		return E_FAIL;
	}

	m_pToolbarHandler = new CComObject<CToolbarHandler>();
	if ( !m_pToolbarHandler )
	{
		pWndParent->DestroyWindow();
		delete pWndParent;
		ReleaseAll();
		return E_OUTOFMEMORY;
	}
	if( m_pToolbarHandler->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
	{
		pWndParent->DestroyWindow();
		delete pWndParent;
		delete m_pToolbarHandler;
		m_pToolbarHandler = NULL;
		ReleaseAll();
		return E_FAIL;
	}
	hr = m_pFrameWork->AddToolBar(m_pToolbarHandler);

	//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
	if( pWndParent->GetSafeHwnd() )
	{
		pWndParent->DestroyWindow();
	}
	delete pWndParent;
	pWndParent = NULL;

	if( FAILED( hr ) )
	{
		// clean up
		if( m_pToolbarHandler->m_hWnd )
		{
			m_pToolbarHandler->DestroyWindow();
		}
		delete m_pToolbarHandler;
		m_pToolbarHandler = NULL;
		ReleaseAll();
		return E_FAIL;
	}

	CString strError;
	hr = InitializeDirectMusic( strError );
	if( FAILED( hr ) )
	{
		if( ::IsWindow(m_pToolbarHandler->m_hWnd) )
		{
			m_pToolbarHandler->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar(m_pToolbarHandler);
		m_pToolbarHandler = NULL;

		if( !strError.IsEmpty() )
		{
			*pbstrErrorText = strError.AllocSysString();
		}
		ReleaseAll();
		return hr;
	}

	////////////////////////////////////////////////////////////////////////
	// S_OK will be returned if we make it to this point.
	// Any failures from this point forward are not considered important
	// enough to cause Initilaize() to fail.
	////////////////////////////////////////////////////////////////////////

	// Create Options toolbar
	////////////////////////////////////////////////////////////////////////
	m_pOptionsToolbarHandler = new CComObject<COptionsToolbarHandler>();
	if ( m_pOptionsToolbarHandler )
	{
		pWndParent = CreateParentWindow();
		if( !pWndParent )
		{
			// clean up
			delete m_pOptionsToolbarHandler;
			m_pOptionsToolbarHandler = NULL;
		}
		else
		{
			rect = CRect( 0, 0, 0, 0 );
			if( m_pOptionsToolbarHandler->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
			{
				// clean up
				pWndParent->DestroyWindow();
				delete m_pOptionsToolbarHandler;
				m_pOptionsToolbarHandler = NULL;
			}
			else
			{
				hr = m_pFrameWork->AddToolBar(m_pOptionsToolbarHandler);

				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if( pWndParent->GetSafeHwnd() )
				{
					pWndParent->DestroyWindow();
				}

				if( FAILED( hr ) )
				{
					// clean up
					if( m_pOptionsToolbarHandler->m_hWnd )
					{
						m_pOptionsToolbarHandler->DestroyWindow();
					}
					delete m_pOptionsToolbarHandler;
					m_pOptionsToolbarHandler = NULL;
				}
				else
				{
					// Succeeded
				}
			}
			delete pWndParent;
			pWndParent = NULL;
		}
	}

	// Create Status toolbar
	////////////////////////////////////////////////////////////////////////
	m_pStatusToolbarHandler = new CComObject<CStatusToolbarHandler>();
	if ( m_pStatusToolbarHandler )
	{
		pWndParent = CreateParentWindow();
		if( !pWndParent )
		{
			// clean up
			delete m_pStatusToolbarHandler;
			m_pStatusToolbarHandler = NULL;
		}
		else
		{
			rect = CRect( 0, 0, 0, 0 );
			if( m_pStatusToolbarHandler->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
			{
				// clean up
				pWndParent->DestroyWindow();
				delete m_pStatusToolbarHandler;
				m_pStatusToolbarHandler = NULL;
			}
			else
			{
				hr = m_pFrameWork->AddToolBar(m_pStatusToolbarHandler);

				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if( pWndParent->GetSafeHwnd() )
				{
					pWndParent->DestroyWindow();
				}

				if( FAILED( hr ) )
				{
					// clean up
					if( m_pStatusToolbarHandler->m_hWnd )
					{
						m_pStatusToolbarHandler->DestroyWindow();
					}
					delete m_pStatusToolbarHandler;
					m_pStatusToolbarHandler = NULL;
				}
				else
				{
					m_pStatusToolbarHandler->EnableTimer(TRUE);
				}
			}
			delete pWndParent;
			pWndParent = NULL;
		}
	}

	// Create Secondary Segment toolbar
	////////////////////////////////////////////////////////////////////////
	CSecondaryToolbarHandler *pToolbarHandler = new CComObject<CSecondaryToolbarHandler>();
	if ( pToolbarHandler )
	{
		pWndParent = CreateParentWindow();
		if( !pWndParent )
		{
			// clean up
			delete pToolbarHandler;
		}
		else
		{
			rect = CRect( 0, 0, 0, 0 );
			if( pToolbarHandler->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
			{
				// clean up
				pWndParent->DestroyWindow();
				delete pToolbarHandler;
			}
			else
			{
				hr = m_pFrameWork->AddToolBar(pToolbarHandler);

				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if( pWndParent->GetSafeHwnd() )
				{
					pWndParent->DestroyWindow();
				}

				if( FAILED( hr ) )
				{
					// clean up
					if( pToolbarHandler->m_hWnd )
					{
						pToolbarHandler->DestroyWindow();
					}
					delete pToolbarHandler;
				}
				else
				{
					m_lstSecondaryToolbars.AddHead( pToolbarHandler );
				}
			}
			delete pWndParent;
			pWndParent = NULL;
		}
	}

	// Create Synth Status toolbar
	////////////////////////////////////////////////////////////////////////
	m_pSynthStatusToolbar = new CComObject<CSynthStatusToolbar>();
	if ( m_pSynthStatusToolbar )
	{
		pWndParent = CreateParentWindow();
		if( !pWndParent )
		{
			// clean up
			delete m_pSynthStatusToolbar;
			m_pSynthStatusToolbar = NULL;
		}
		else
		{
			rect = CRect( 0, 0, 0, 0 );
			if( m_pSynthStatusToolbar->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
			{
				// clean up
				pWndParent->DestroyWindow();
				delete m_pSynthStatusToolbar;
				m_pSynthStatusToolbar = NULL;
			}
			else
			{
				hr = m_pFrameWork->AddToolBar(m_pSynthStatusToolbar);

				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if( pWndParent->GetSafeHwnd() )
				{
					pWndParent->DestroyWindow();
				}

				if( FAILED( hr ) )
				{
					// clean up
					if( m_pSynthStatusToolbar->m_hWnd )
					{
						m_pSynthStatusToolbar->DestroyWindow();
					}
					delete m_pSynthStatusToolbar;
					m_pSynthStatusToolbar = NULL;
				}
				else
				{
					m_pSynthStatusToolbar->EnableTimer(TRUE);
				}
			}
			delete pWndParent;
			pWndParent = NULL;
		}
	}

	// Create Wave Recording toolbar
	////////////////////////////////////////////////////////////////////////
	m_pWaveRecordToolbar = new CComObject<CWaveRecordToolbar>();
	if ( m_pWaveRecordToolbar )
	{
		pWndParent = CreateParentWindow();
		if( !pWndParent )
		{
			// clean up
			delete m_pWaveRecordToolbar;
			m_pWaveRecordToolbar = NULL;
		}
		else
		{
			rect = CRect( 0, 0, 0, 0 );
			if( m_pWaveRecordToolbar->Create( pWndParent->GetSafeHwnd(), rect, NULL, WS_CHILD ) == 0 )
			{
				// clean up
				pWndParent->DestroyWindow();
				delete m_pWaveRecordToolbar;
				m_pWaveRecordToolbar = NULL;
			}
			else
			{
				hr = m_pFrameWork->AddToolBar(m_pWaveRecordToolbar);

				//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
				if( pWndParent->GetSafeHwnd() )
				{
					pWndParent->DestroyWindow();
				}

				if( FAILED( hr ) )
				{
					// clean up
					if( m_pWaveRecordToolbar->m_hWnd )
					{
						m_pWaveRecordToolbar->DestroyWindow();
					}
					delete m_pWaveRecordToolbar;
					m_pWaveRecordToolbar = NULL;
				}
				else
				{
					//m_pWaveRecordToolbar->EnableTimer(TRUE);
				}
			}
			delete pWndParent;
			pWndParent = NULL;
		}
	}

	// Create loopback tool
	g_pMIDIInputContainer = new CMIDIInputContainer( m_pDMusic );

	g_pMIDIInputContainer->InitializeFromRegistry();

	// Always enable and check the MIDI and Synth buttons
	if( m_pOptionsToolbarHandler )
	{
		m_pOptionsToolbarHandler->SetBtnState( ID_TRANSP_MIDI, TBSTATE_ENABLED | TBSTATE_CHECKED );
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CConductor::CleanUp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_hNotifyEvent )
	{
		ASSERT( !m_hNotifyExitEvent );
		m_hNotifyExitEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	}

	m_fShutdown = TRUE;

	::EnterCriticalSection( &m_csNotifyEntry );

	CNotifyEntry* pNotifyEntry;
	while( !m_lstNotifyEntry.IsEmpty() )
	{
		pNotifyEntry = m_lstNotifyEntry.RemoveHead();
		delete pNotifyEntry;
	}

	::LeaveCriticalSection( &m_csNotifyEntry );

	// First, de-activate DirectMusic and all of the ports registered with it
	if (m_pDMusic)
	{
		m_pDMusic->Activate( FALSE );
	}

	if (m_pDMPerformance)
	{
		m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_SEGMENT );
		m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_MEASUREANDBEAT );
		m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_PERFORMANCE );

		// Signal the notification event handler thread and wait for it to exit
		if ( m_hNotifyEvent )
		{
			m_pDMPerformance->SetNotificationHandle( NULL, 0 );
			::SetEvent( m_hNotifyEvent );

			::WaitForSingleObject( m_hNotifyExitEvent, 5000 );
			::CloseHandle( m_hNotifyEvent );
			m_hNotifyEvent = NULL;
			::CloseHandle( m_hNotifyExitEvent );
			m_hNotifyExitEvent = NULL;
		}

		m_pDMPerformance->Stop( NULL, NULL, 0, 0 );

		m_pDMPerformance->CloseDown();
	}

	if( m_pToolbarHandler != NULL )
	{
		if( ::IsWindow(m_pToolbarHandler->m_hWnd) )
		{
			m_pToolbarHandler->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( m_pToolbarHandler );
		m_pToolbarHandler = NULL;
	}
	if( m_pOptionsToolbarHandler != NULL )
	{
		if( ::IsWindow(m_pOptionsToolbarHandler->m_hWnd) )
		{
			m_pOptionsToolbarHandler->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( m_pOptionsToolbarHandler );
		m_pOptionsToolbarHandler = NULL;
	}
	if( m_pStatusToolbarHandler != NULL )
	{
		if( ::IsWindow(m_pStatusToolbarHandler->m_hWnd) )
		{
			m_pStatusToolbarHandler->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( m_pStatusToolbarHandler );
		m_pStatusToolbarHandler = NULL;
	}
	if( m_pSynthStatusToolbar != NULL )
	{
		if( ::IsWindow(m_pSynthStatusToolbar->m_hWnd) )
		{
			m_pSynthStatusToolbar->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( m_pSynthStatusToolbar );
		m_pSynthStatusToolbar = NULL;
	}
	if( m_pWaveRecordToolbar != NULL )
	{
		if( ::IsWindow(m_pWaveRecordToolbar->m_hWnd) )
		{
			m_pWaveRecordToolbar->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( m_pWaveRecordToolbar );
		m_pWaveRecordToolbar = NULL;
	}
	while( !m_lstSecondaryToolbars.IsEmpty() )
	{
		CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.RemoveHead();

		if( ::IsWindow(pToolbarHandler->m_hWnd) )
		{
			pToolbarHandler->DestroyWindow();
		}
		m_pFrameWork->RemoveToolBar( pToolbarHandler );
	}
	if( g_pMIDIInputContainer != NULL )
	{
		delete g_pMIDIInputContainer;
		g_pMIDIInputContainer = NULL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CConductor::GetName( BSTR __RPC_FAR* pbstrName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( pbstrName == NULL )
    {
        return E_INVALIDARG;
    }

    CComBSTR bstrName( "Conductor" );

    *pbstrName = bstrName.Detach();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CConductor::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	UNREFERENCED_PARAMETER(guidRefNodeId);
	UNREFERENCED_PARAMETER(ppIRefNode);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CConductor::OnActivateApp( BOOL fActivate )
{
	UNREFERENCED_PARAMETER(fActivate);
	// Commented out, per bug #14271
	/*
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( fActivate )
	{
		return ResumeOutput();
	}
	else
	{
		return SuspendOutput();
	}
	*/
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CConductor::PlayFromStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pToolbarHandler )
	{
		BOOL bHandled;
		if( m_pToolbarHandler->m_fDisplayingPlay )
		{
			m_pToolbarHandler->OnRewPlayClicked( 0, 0, NULL, bHandled );
		}
		else
		{
			m_pToolbarHandler->OnStopImmediateClicked( 0, 0, NULL, bHandled );
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CConductor::PlayFromCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pToolbarHandler )
	{
		BOOL bHandled;
		if( m_pToolbarHandler->m_fDisplayingPlay )
		{
			m_pToolbarHandler->OnPlayClicked( 0, 0, NULL, bHandled );
		}
		else
		{
			m_pToolbarHandler->OnStopClicked( 0, 0, NULL, bHandled );
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CConductor::Record()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pToolbarHandler )
	{
		BOOL bHandled;
		m_pToolbarHandler->OnRecordClicked( 0, 0, NULL, bHandled );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CConductor::Transition()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_pToolbarHandler )
	{
		return E_FAIL;
	}

	// Get the index of the Transition button
	int nIndex = ::SendMessage( m_pToolbarHandler->m_hWndToolbar, TB_COMMANDTOINDEX, ID_TRANSP_TRANSITION, 0 );

	// Get the state information of the Transition button
	TBBUTTON tbbutton;
	::SendMessage( m_pToolbarHandler->m_hWndToolbar, TB_GETBUTTON, nIndex, (LPARAM)&tbbutton );

	// If it's enabled
	if( tbbutton.fsState & TBSTATE_ENABLED )
	{
		// Pretend the Transition button was pressed
		BOOL bHandled;
		m_pToolbarHandler->OnTransitionClicked( 0, 0, NULL, bHandled );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CConductor::SpaceBarPress()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pToolbarHandler )
	{
		BOOL bHandled;
		if( m_pToolbarHandler->m_fDisplayingPlay )
		{
			m_pToolbarHandler->OnPlayClicked( 0, 0, NULL, bHandled );
		}
		else
		{
			m_pToolbarHandler->OnStopImmediateClicked( 0, 0, NULL, bHandled );
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CConductor::PlaySecondarySegment( int nSecondarySegment )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( nSecondarySegment < 0 )
	{
		return E_INVALIDARG;
	}

	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

		if( nSecondarySegment < (signed)pToolbarHandler->m_dwNumButtons )
		{
			if( pToolbarHandler->m_arrayButtons[nSecondarySegment]->pISegmentState )
			{
				pToolbarHandler->OnStopClicked( WORD(nSecondarySegment) );
			}
			else
			{
				pToolbarHandler->OnPlayClicked( WORD(nSecondarySegment) );
			}
		}
	}
	return E_FAIL;
}


BOOL CConductor::IsActiveUsingEngine( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_rpActive != NULL )
	{
		DWORD dwFlags;
		m_rpActive->GetFlags( &dwFlags );
		if (!(dwFlags & B_NOT_USING_ENGINE))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CConductor::IsPlayingUsingEngine( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_rpPlaying != NULL )
	{
		DWORD dwFlags;
		m_rpPlaying->GetFlags( &dwFlags );
		if (!(dwFlags & B_NOT_USING_ENGINE))
		{
			return TRUE;
		}
	}
	return FALSE;
}

ITransportRegEntry *CConductor::GetDisplayedTransport( void )
{
	/*
	if ( !m_fOutputEnabled )
	{
		return NULL;
	}
	*/

	ITransportRegEntry *pRegEntry;

	if( m_rpPlaying != NULL )
	{
		// m_rpPlaying is valid
		pRegEntry = m_rpPlaying;
		/*
		if( m_rpActive != NULL )
		{
			// m_rpActive is valid
			if (m_rpActive == m_rpPlaying)
			{
				pRegEntry = m_rpActive;
			}
			else
			{
				pRegEntry = m_rpPlaying;
			}
		}
		else
		{
			// m_rpActive is invalid
			pRegEntry = m_rpPlaying;
		}
		*/
	}
	else
	{
		// m_rpPlaying is invalid
		if( m_rpActive != NULL )
		{
			// m_iActive is valid
			pRegEntry = m_rpActive;
		}
		else
		{
			// m_rpActive is invalid
			pRegEntry = NULL;
		}
	}
	return pRegEntry;
}

BOOL CConductor::IsEnginePlaying( void )
{
	ASSERT( m_pDMPerformance != NULL );
	if ( m_pDMPerformance == NULL)
	{
		return FALSE;
	}

	MUSIC_TIME mtNow;
	m_pDMPerformance->GetTime( NULL, &mtNow );
	IDirectMusicSegmentState *pSegmentState = NULL;
	if ( FAILED( m_pDMPerformance->GetSegmentState( &pSegmentState, mtNow ) ) )
	{
		return FALSE;
	}

	RELEASE( pSegmentState );

	return TRUE;
}

void AddStandardAudiopath( CTypedPtrList<CPtrList, CAudiopathListItem*> *plstAudiopaths, const DWORD dwStandardID, const DWORD dwXboxID, const int nStringID )
{
	CString strText;
	if( strText.LoadString( nStringID ) )
	{
		// Create a new CAudiopathListItem to store information about this Audiopath in
		CAudiopathListItem *pAudiopathListItem = new CAudiopathListItem(NULL, dwStandardID, dwXboxID);
		if( pAudiopathListItem )
		{
			pAudiopathListItem->strName = strText;
			plstAudiopaths->AddTail( pAudiopathListItem );
		}
	}
}

void CConductor::ReadDefaultDirectMusicAudiopath( IDirectMusicAudioPath **ppAudiopath )
{
	if( ppAudiopath == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	// Add the standard audio paths
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_SHARED_STEREOPLUSREVERB, XBOX_APATH_SHARED_STEREOPLUSREVERB, IDS_APATH_MUSIC );
	// Don't enable, since you can't play any music on it
	//AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_GLOBALFX_ENV, IDS_APATH_ENV );
	//AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_ENV3D, IDS_APATH_3D );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_DYNAMIC_MONO, IDS_APATH_MONO );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_STEREO, XBOX_APATH_SHARED_STEREO, IDS_APATH_STEREO  );
#ifdef DMP_XBOX
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_QUAD, IDS_APATH_MIXBIN_QUAD );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_QUAD_ENV, IDS_APATH_MIXBIN_QUAD_ENV );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_QUAD_MUSIC, IDS_APATH_MIXBIN_QUAD_MUSIC );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_5DOT1, IDS_APATH_MIXBIN_5DOT1 );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_5DOT1_ENV, IDS_APATH_MIXBIN_5DOT1_ENV );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_5DOT1_MUSIC, IDS_APATH_MIXBIN_5DOT1_MUSIC );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_MONO, XBOX_APATH_MIXBIN_STEREO_EFFECTS, IDS_APATH_MIXBIN_STEREO_EFFECTS );
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_3D, XBOX_APATH_DYNAMIC_3D, IDS_APATH_3D_WET );
#else
	AddStandardAudiopath( &m_lstAudiopaths, DMUS_APATH_DYNAMIC_3D, XBOX_APATH_DYNAMIC_3D, IDS_APATH_3D_DRY );
#endif
    
    POSITION pos = m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		CAudiopathListItem *pAudiopathListItem = m_lstAudiopaths.GetNext( pos );
		if( !pAudiopathListItem->pNode
		&&	pAudiopathListItem->dwStandardID == DMUS_APATH_SHARED_STEREOPLUSREVERB )
		{
			m_pAudiopathListItem = pAudiopathListItem;
			break;
		}
	}

	ASSERT( m_pAudiopathListItem );

	// Initialize dwNumPChannelsToAskFor
	DWORD dwNumPChannelsToAskFor = DEFAULT_NUM_PCHANNELS;

	if( GUID_Synth_Default != m_clsidDefaultSynth )
	{
		// Try and find the portcaps for the default synth
		DMUS_PORTCAPS dmPortCaps;
		if( GetPortCaps( m_pDMusic, m_clsidDefaultSynth, &dmPortCaps ) )
		{
			dwNumPChannelsToAskFor = min( dwNumPChannelsToAskFor, dmPortCaps.dwMaxChannelGroups * 16 );
		}
	}

	HRESULT hr = m_pDMPerformance->CreateStandardAudioPath( DMUS_APATH_SHARED_STEREOPLUSREVERB, dwNumPChannelsToAskFor, TRUE, ppAudiopath );
#ifdef DMP_XBOX
    if( (XBDM_CANNOTCONNECT == hr)
	||	(XBDM_CANNOTACCESS == hr) )
    {
		CString strError;
        strError.LoadString( (XBDM_CANNOTCONNECT == hr) ? IDS_ERR_CANNOT_CONNECT_XBOX : IDS_ERR_WRONG_VERSION_XBOX );
    	if( IDYES == AfxMessageBox( strError, MB_ICONWARNING | MB_YESNO ) )
        {
            DWORD dwValue = 0;
            SetRegDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"), TEXT("EnableXboxSynth"), &dwValue, TRUE);
            dwValue = TRUE;
            SetRegDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"), TEXT("EnablePCSynth"), &dwValue, TRUE);
			hr = m_pDMPerformance->CreateStandardAudioPath( DMUS_APATH_SHARED_STEREOPLUSREVERB, dwNumPChannelsToAskFor, TRUE, ppAudiopath );
        }
    }
#endif
	if( FAILED(hr) )
	{
		m_pAudiopathListItem = NULL;
	}
}

// Notify all components in the framework that the output port configuration changed
void CConductor::BroadCastPortChangeNotification( void )
{
	// Reset all thru connections
	UpdateMIDIThruChannels();

	// Reset Echo MIDI in state
	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->ResetLatencyOffset();
	}

	if( m_pFrameWork )
	{
		HRESULT hr;
		IDMUSProdComponent* pIComponent;
		IDMUSProdComponent* pINextComponent;

		hr = m_pFrameWork->GetFirstComponent( &pINextComponent );

		while( SUCCEEDED( hr ) && pINextComponent )
		{
			pIComponent = pINextComponent;

			IDMUSProdPortNotify *pIDMUSProdPortNotify;
			if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIDMUSProdPortNotify ) ) )
			{
				pIDMUSProdPortNotify->OnOutputPortsChanged();
				pIDMUSProdPortNotify->Release();
			}

			hr = m_pFrameWork->GetNextComponent( pIComponent, &pINextComponent );
			pIComponent->Release();
		}
	}
}

// Notify all components in the framework that the output ports were removed
void CConductor::BroadCastPortRemovalNotification( void )
{
	// Clear all thru connections
	ClearThruConnections();

	if( m_pFrameWork )
	{
		HRESULT hr;
		IDMUSProdComponent* pIComponent;
		IDMUSProdComponent* pINextComponent;

		hr = m_pFrameWork->GetFirstComponent( &pINextComponent );

		while( SUCCEEDED( hr ) && pINextComponent )
		{
			pIComponent = pINextComponent;

			IDMUSProdPortNotify *pIDMUSProdPortNotify;
			if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIDMUSProdPortNotify ) ) )
			{
				pIDMUSProdPortNotify->OnOutputPortsRemoved();
				pIDMUSProdPortNotify->Release();
			}

			hr = m_pFrameWork->GetNextComponent( pIComponent, &pINextComponent );
			pIComponent->Release();
		}
	}
}

void CConductor::WriteDefaultDirectMusicAudiopath( void )
{
	/*
	TCHAR szComponentPath[MAX_PATH];
	TCHAR szinport[32];

	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\") );
	_tcscpy( szinport, _T("1\\") );
	_tcscat( szComponentPath, szinport );

	WritePortListToRegistry( szComponentPath, &m_lstPortOptions1 );

	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\") );
	_tcscpy( szinport, _T("2\\") );
	_tcscat( szComponentPath, szinport );

	WritePortListToRegistry( szComponentPath, &m_lstPortOptions2 );
	*/

	return;
}

/*
void CConductor::ImportPortListFromRegistry( TCHAR szBasePath[], CTypedPtrList<CPtrList, CPortOptions*> *plstPortOptions )
{
	HKEY  hKeyOpen;
	LONG  lResult;

	// Open the base key
	lResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, szBasePath, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		// Enumerate through the subkeys - each subkey is a different port GUID
		DWORD dwIndex = 0;
		TCHAR szSubKey[MAX_PATH + 1];
		lResult = ::RegEnumKey( hKeyOpen, dwIndex, szSubKey, MAX_PATH + 1 );
		while( lResult == ERROR_SUCCESS )
		{
			// Open the subkey
			HKEY hKeyPort;
			lResult = ::RegOpenKeyEx( hKeyOpen, szSubKey, 0, KEY_READ, &hKeyPort );
			if( lResult == ERROR_SUCCESS )
			{
				// Parse port GUID
				OLECHAR *pszGuid = new OLECHAR[100];
				::MultiByteToWideChar( CP_ACP, 0, szSubKey, -1, pszGuid, 100);

				GUID guidPortGUID;
				lResult = ::CLSIDFromString(pszGuid, &guidPortGUID);
				delete pszGuid;
				if( lResult == NOERROR )
				{
					// Get the DMUS_PORTCAPS structure for this port.
					DMUS_PORTCAPS dmPortCaps;
					if( GetDMusPortCaps( guidPortGUID, &dmPortCaps ) )
					{
						CPortOptions *pPortOptions;
						pPortOptions = new CPortOptions( &dmPortCaps );

						DWORD *pdwPChannelArray;
						DWORD dwSize;
						if( GetRegData( hKeyPort, _T("PChannelBlocks"), (BYTE **)&pdwPChannelArray, &dwSize ) )
						{
							pPortOptions->m_adwBlocks.SetSize( dwSize / sizeof(DWORD) );
							// Assign them to the first 'n' blocks
							for( DWORD i=0; i <dwSize / sizeof(DWORD); i++ )
							{
								pPortOptions->m_adwBlocks[i] = pdwPChannelArray[i];
							}

							delete pdwPChannelArray;
						}

						DWORD dwEffects;
						if( GetRegDWORD( hKeyPort, NULL, _T("Effects"), &dwEffects ) )
						{
							pPortOptions->m_dwEffects = dwEffects;
						}

						BYTE *pbData;
						if( GetRegData( hKeyPort, _T("WavesReverbParams"), (BYTE **)&pbData, &dwSize ) )
						{
							ASSERT( dwSize == sizeof( DMUS_WAVES_REVERB_PARAMS ) );
							if( dwSize == sizeof( DMUS_WAVES_REVERB_PARAMS ) )
							{
								memcpy( &pPortOptions->m_ReverbParams, pbData, sizeof( DMUS_WAVES_REVERB_PARAMS ) );
							}

							delete pbData;
						}

						DWORD dwDownloadGM;
						if( GetRegDWORD( hKeyPort, NULL, _T("DownloadGM"), &dwDownloadGM ) )
						{
							pPortOptions->m_fDownloadGM = (dwDownloadGM != 0);
						}

						DWORD dwSampleRate;
						if( GetRegDWORD( hKeyPort, NULL, _T("SampleRate"), &dwSampleRate ) )
						{
							pPortOptions->m_dwSampleRate = dwSampleRate;
						}

						DWORD dwVoices;
						if( GetRegDWORD( hKeyPort, NULL, _T("Voices"), &dwVoices ) )
						{
							pPortOptions->m_dwVoices = dwVoices;
						}

						plstPortOptions->AddTail( pPortOptions );
					}
				}
				::RegCloseKey( hKeyPort );
			}

			dwIndex++;
			lResult = ::RegEnumKey( hKeyOpen, dwIndex, szSubKey, MAX_PATH + 1 );
		}
		::RegCloseKey( hKeyOpen );
	}

	return;
}

void CConductor::WritePortListToRegistry( TCHAR szBasePath[], CTypedPtrList<CPtrList, CPortOptions*> *plstPortOptions )
{
	HKEY  hKeyOpen;
	LONG  lResult;

	// Open the base key
	lResult = ::RegCreateKeyEx( HKEY_CURRENT_USER, szBasePath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyOpen, NULL );
	if( lResult == ERROR_SUCCESS )
	{
		// Enumerate through the subkeys - each subkey is a different port GUID
		TCHAR szSubKey[MAX_PATH + 1];
		lResult = ::RegEnumKey( hKeyOpen, 0, szSubKey, MAX_PATH + 1 );
		while( lResult == ERROR_SUCCESS )
		{
			// Delete the subkey
			lResult = ::RegDeleteKey( hKeyOpen, szSubKey );

			lResult = ::RegEnumKey( hKeyOpen, 0, szSubKey, MAX_PATH + 1 );
		}

		// Enumerate throught the ports - create a subkey for each port GUID
		CPortOptions *pPortOptions;
		POSITION pos = plstPortOptions->GetHeadPosition();
		while( pos )
		{
			pPortOptions = plstPortOptions->GetNext( pos );

			// Convert port GUID to string
			LPOLESTR psz;
		    if( SUCCEEDED( StringFromIID(pPortOptions->m_guidPort, &psz) ) )
			{
				WideCharToMultiByte( CP_ACP, 0, psz, -1, szSubKey, sizeof(szSubKey), NULL, NULL );
				CoTaskMemFree( psz );

				// Create the subkey
				HKEY hKeyPort;
				lResult = ::RegCreateKeyEx( hKeyOpen, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKeyPort, NULL );
				if( lResult == ERROR_SUCCESS )
				{
					if( pPortOptions->m_adwBlocks.GetSize() )
					{
						DWORD *padwBlocks;
						padwBlocks = new DWORD[pPortOptions->m_adwBlocks.GetSize()];

						for( DWORD i=0; i < (unsigned)pPortOptions->m_adwBlocks.GetSize(); i++ )
						{
							padwBlocks[i] = pPortOptions->m_adwBlocks[i];
						}

						::RegSetValueEx( hKeyPort, _T("PChannelBlocks"), 0, REG_BINARY, (LPBYTE)padwBlocks, sizeof(DWORD) * pPortOptions->m_adwBlocks.GetSize() );

						delete[] padwBlocks;
					}

					::RegSetValueEx( hKeyPort, _T("Effects"), 0, REG_DWORD, (LPBYTE)&pPortOptions->m_dwEffects, sizeof(DWORD) );

					::RegSetValueEx( hKeyPort, _T("WavesReverbParams"), 0, REG_BINARY, (LPBYTE)&pPortOptions->m_ReverbParams, sizeof(DMUS_WAVES_REVERB_PARAMS) );

					::RegSetValueEx( hKeyPort, _T("DownloadGM"), 0, REG_DWORD, (LPBYTE)&pPortOptions->m_fDownloadGM, sizeof(DWORD) );

					::RegSetValueEx( hKeyPort, _T("SampleRate"), 0, REG_DWORD, (LPBYTE)&pPortOptions->m_dwSampleRate, sizeof(DWORD) );

					::RegSetValueEx( hKeyPort, _T("Voices"), 0, REG_DWORD, (LPBYTE)&pPortOptions->m_dwVoices, sizeof(DWORD) );

					::RegCloseKey( hKeyPort );
				}
			}
		}
		::RegCloseKey( hKeyOpen );
	}

	return;
}

BOOL CConductor::GetDMusPortCaps( GUID guidPortGUID, DMUS_PORTCAPS *pdmPortCaps )
{
	ASSERT( pdmPortCaps );
	if( !pdmPortCaps )
	{
		return FALSE;
	}

	// Search for guidPortGUID
	HRESULT hr;
	for(DWORD dwIndex = 0; ; dwIndex++)
	{
		// Initialize dmpc
		DMUS_PORTCAPS dmpc;
		ZeroMemory(&dmpc, sizeof(dmpc));
		dmpc.dwSize = sizeof(DMUS_PORTCAPS);

		// Get the port's capabilities
		hr = m_pDMusic->EnumPort(dwIndex, &dmpc);

		// If we succeeded and didn't hit the end of the list
		if(SUCCEEDED(hr) && (hr != S_FALSE) )
		{
			// If this is the port we want
			if ( ::IsEqualGUID( dmpc.guidPort, guidPortGUID ) )
			{
				memcpy( pdmPortCaps, &dmpc, sizeof( DMUS_PORTCAPS ) );
				return TRUE;
			}
		}
		else
		{
			break;
		}
	}
	return FALSE;
}
*/

void CConductor::SendAllNotesOff( long lTime, BOOL fSendGMReset )
{
	UNREFERENCED_PARAMETER(lTime);
	UNREFERENCED_PARAMETER(fSendGMReset);

	// Iterate through 1000 PChannels
	for( int i=0; i<1000/16; i++)
	{
		for( int j=0; j<16; j++ )
		{
			/* Commented out per Todor's request on 2/11/00
			// Send Reset All Controllers (121)
			SendMIDIMessage( i * 16 + j, lTime, MIDI_CCHANGE, 121 );

			// Send All Notes Off (123)
			SendMIDIMessage( i * 16 + j, lTime, MIDI_CCHANGE, 123 );

			// Send All Sound Off (127)
			SendMIDIMessage( i * 16 + j, lTime, MIDI_CCHANGE, 127 );

			if( fSendGMReset )
			{
				SendGMReset( i * 16 + j );
			}
			*/
			/*
			// Send note off messages
			for( BYTE bNote = 0; bNote < 127; bNote ++)
			{
				SendMIDIMessage( m_pDMPerformance, i * 16 + j, lTime, MIDI_NOTEOFF, bNote );
			}
			*/
		}
	}
}

/*
  GM Reset    F0 7E 7F 09 01 F7
  GS Reset    F0 0A 41 10 42 12 40 00 7F 00 41 F7
  XG Reset    F0 08 43 10 4C 00 00 7E 00 F7
  GS Exit     F0 0A 41 10 42 12 40 00 7F 7F 42 F7
*/
HRESULT CConductor::SendGMReset( DWORD dwPChannel )
{
	ASSERT( m_pDMPerformance );

	HRESULT hr;
	DMUS_SYSEX_PMSG *pDMSysexEvent = NULL;

	// Allocate MIDI message
	hr = m_pDMPerformance->AllocPMsg( sizeof(DMUS_PMSG) + sizeof(DWORD) + 6, (DMUS_PMSG**)&pDMSysexEvent );
	if ( FAILED(hr) )
	{
		return hr;
	}
	memset( pDMSysexEvent, 0, sizeof(DMUS_PMSG) + sizeof(DWORD) + 6 );

	pDMSysexEvent->dwLen = 6;
	pDMSysexEvent->abData[0] = 0xF0;
	pDMSysexEvent->abData[1] = 0x7E;
	pDMSysexEvent->abData[2] = 0x7F;
	pDMSysexEvent->abData[3] = 0x09;
	pDMSysexEvent->abData[4] = 0x01;
	pDMSysexEvent->abData[5] = 0xF7;
	//pDMSysexEvent->bByte2 = 0;
	//pDMSysexEvent->m_rtTime = 0;
	pDMSysexEvent->dwFlags = DMUS_PMSGF_TOOL_IMMEDIATE | DMUS_PMSGF_REFTIME;
	pDMSysexEvent->dwPChannel = dwPChannel;
	//pDMSysexEvent->dwVirtualTrackID = 0;
	pDMSysexEvent->dwType = DMUS_PMSGT_SYSEX;
	pDMSysexEvent->dwSize = sizeof(DMUS_PMSG) + sizeof(DWORD) + 6;

	DMUS_PMSG *pPipelineEvent = NULL;
	pPipelineEvent = (DMUS_PMSG*) pDMSysexEvent;

	return m_pDMPerformance->SendPMsg( pPipelineEvent );
}

HRESULT CConductor::SendMIDIMessage( DWORD dwPChannel, long lTime, BYTE bStatus, BYTE bByte1 )
{
	ASSERT( m_pDMPerformance );

	HRESULT hr;
	DMUS_PMSG *pPipelineEvent = NULL;
	DMUS_MIDI_PMSG *pDMMidiEvent = NULL;

	// Allocate MIDI message
	hr = m_pDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent );
	if ( FAILED(hr) )
	{
		return hr;
	}
	memset( pDMMidiEvent, 0, sizeof(DMUS_MIDI_PMSG) );

	pDMMidiEvent->bStatus = bStatus;
	pDMMidiEvent->bByte1 = bByte1;
	//pDMMidiEvent->bByte2 = 0;
	if( lTime )
	{
		REFERENCE_TIME rtNow;
		m_pDMPerformance->GetLatencyTime( &rtNow );
		pDMMidiEvent->rtTime = lTime * 10000 + rtNow;
		pDMMidiEvent->dwFlags = DMUS_PMSGF_TOOL_QUEUE;
	}
	else
	{
		//pDMMidiEvent->m_rtTime = 0;
		pDMMidiEvent->dwFlags = DMUS_PMSGF_TOOL_IMMEDIATE;
	}
	pDMMidiEvent->dwFlags |= DMUS_PMSGF_REFTIME;
	pDMMidiEvent->dwPChannel = dwPChannel;
	//pDMMidiEvent->dwVirtualTrackID = 0;
	pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;
	pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	pPipelineEvent = (DMUS_PMSG*) pDMMidiEvent;

	return m_pDMPerformance->SendPMsg( pPipelineEvent );
}


HRESULT STDMETHODCALLTYPE CConductor::IsDirty(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CConductor::Load(
	/* [unique][in] */ IStream *pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !m_lstSecondaryToolbars.IsEmpty() )
	{
		IDMUSProdRIFFStream* pIRiffStream;
		if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			return E_FAIL;
		}

		// Try and find the toolbar list chunk
		MMCKINFO ckMain;
		ckMain.fccType = FOURCC_2NDARY_TOOLBAR_LIST;
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) != 0 )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Descend into the toolbar size chunk
		MMCKINFO ckSubChunk;
		if( pIRiffStream->Descend( &ckSubChunk, &ckMain, 0 ) != 0 )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Load number of toolbars
		if( ckSubChunk.ckid != FOURCC_2NDARY_TOOLBAR_SIZE )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}
		ASSERT( ckSubChunk.cksize == sizeof(DWORD) );

		// Read the number of segments to create
		DWORD cbRead;
		HRESULT hr;
		DWORD dwNumSegmentToolbars = 0;
		hr = pIStream->Read( &dwNumSegmentToolbars, sizeof(DWORD), &cbRead );
		if( FAILED(hr) || (cbRead != sizeof(DWORD)) )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Ascend out of the size chunk
		pIRiffStream->Ascend( &ckSubChunk, 0 );

		// TODO: resize m_lstSecondaryToolbars to match number of requested toolbars
		POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
		while( pos && SUCCEEDED(hr) )
		{
			CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

			// Set number of buttons to display
			pToolbarHandler->SetNumButtons( dwNumSegmentToolbars );
			pToolbarHandler->m_spinControl.SetPos( dwNumSegmentToolbars );

			hr = pToolbarHandler->Load( pIRiffStream, &ckMain );
		}

		// Ascend out of the toolbar list chunk and clean up
		pIRiffStream->Ascend( &ckMain, 0 );
		pIRiffStream->Release();
		return hr;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT STDMETHODCALLTYPE CConductor::Save(
	/* [unique][in] */ IStream *pIStream,
	/* [in] */ BOOL fClearDirty)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(fClearDirty);

	if( !m_lstSecondaryToolbars.IsEmpty() )
	{
		IDMUSProdRIFFStream* pIRiffStream;
		if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			return E_FAIL;
		}

		// Create the toolbar list chunk
		MMCKINFO ckMain;
		ckMain.fccType = FOURCC_2NDARY_TOOLBAR_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Create toolbar size chunk
		MMCKINFO ckSubChunk;
		ckSubChunk.ckid = FOURCC_2NDARY_TOOLBAR_SIZE;
		if( pIRiffStream->CreateChunk( &ckSubChunk, 0 ) != 0 )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// write number of segments
		HRESULT hr;
		DWORD cbWritten, dwNumSegmentToolbars = 0;
		// Compute number of toolbar button states
		POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
		while( pos )
		{
			dwNumSegmentToolbars += m_lstSecondaryToolbars.GetNext( pos )->m_dwNumButtons;
		}
		hr = pIStream->Write( &dwNumSegmentToolbars, sizeof(DWORD), &cbWritten );
		if( FAILED(hr) || (cbWritten != sizeof(DWORD)) )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Ascend out of the size chunk
		pIRiffStream->Ascend( &ckSubChunk, 0 );

		// Write the toolbar states
		pos = m_lstSecondaryToolbars.GetHeadPosition();
		while( pos && SUCCEEDED(hr) )
		{
			hr = m_lstSecondaryToolbars.GetNext( pos )->Save( pIRiffStream );
		}

		// Ascend out of the list chunk
		pIRiffStream->Ascend( &ckMain, 0 );
		pIRiffStream->Release();

		return hr;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT STDMETHODCALLTYPE CConductor::GetSizeMax(
	/* [out] */ ULARGE_INTEGER *pcbSize)
{
	UNREFERENCED_PARAMETER(pcbSize);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetClassID( 
	/* [out] */ CLSID *pClassID)
{
	UNREFERENCED_PARAMETER(pClassID);
	return E_NOTIMPL;
}

void CConductor::UpdateMIDIThruChannels( void )
{
	::EnterCriticalSection( &m_csAudiopath );
	IDirectMusicAudioPath *pDMAudiopath = m_pDMAudiopath;
	if( pDMAudiopath )
	{
		pDMAudiopath->AddRef();
	}
	::LeaveCriticalSection( &m_csAudiopath );

	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->UpdateMIDIThruChannels( pDMAudiopath );
	}

	if( pDMAudiopath )
	{
		pDMAudiopath->Release();
	}
}	

void CConductor::ClearThruConnections( void )
{
	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->ClearThruConnections();
	}
}

void CConductor::StopAllNotesAndSegments( void )
{
	// Stop all Secondary Segments
	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );
		BOOL bHandled;
		for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
		{
			pToolbarHandler->OnButtonClicked( 0, WORD(ID_TRANSP_STOP1 + i), 0, bHandled );
		}
	}

	// Stop the Count-In segment state
	if( m_pToolbarHandler->m_pCountInSegmentState )
	{
		// Stop the Count-In segment state
		// Ignore the m_pToolbarHandler->m_fStopImmediate - stop immediately.
		m_pDMPerformance->Stop( NULL, m_pToolbarHandler->m_pCountInSegmentState, 0, 0 );

		// Release the Count-In segment state
		m_pToolbarHandler->m_pCountInSegmentState->Release();
		m_pToolbarHandler->m_pCountInSegmentState = NULL;
	}

	// Stop the current transport.
	if( m_pToolbarHandler->IsValid( m_rpPlaying ) )
	{
		IDMUSProdTransport*  pITransport;
		m_rpPlaying->GetTransport( &pITransport );
		if (pITransport)
		{
			// We'll assume this succeeded.  If not, the subsequent Performance calls below should cause
			// everything to stop playing.
			pITransport->Stop( TRUE );
			m_rpPlaying = NULL;

			// Update the active flags, if they're using the engine
			m_pToolbarHandler->UpdateActiveFromEngine();

			// Switch to displaying the active buttons
			m_pToolbarHandler->m_fPlayFromStart = FALSE;
		}
	}

	// Stop the performance engine and all notes on all PChannels
	if( m_pDMPerformance )
	{
		m_pDMPerformance->Stop( NULL, NULL, 0, 0 );

		SendAllNotesOff( 250, TRUE );
	}

	::Sleep( 250 );
	m_pToolbarHandler->SetStateAuto();
}
	
void CConductor::SetMIDIExportFields( IUnknown* pIUnknown )
{
	// pIUnknown should be either IDMUSProdTransport* or IDMUSProdSecondaryTransport*
	if( m_pDMPerformance )
	{
		// Set fields needed for MIDI export
		if( m_pIUnknownMIDIExport == NULL
		&&  m_pISegStateMIDIExport == NULL )
		{
			// Set m_pIUnknownMIDIExport 
			m_pIUnknownMIDIExport = pIUnknown; 
			m_pIUnknownMIDIExport->AddRef();

			// Set m_pISegStateMIDIExport
			MUSIC_TIME mtNow;
			if( SUCCEEDED( m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
			{
				m_pDMPerformance->GetSegmentState( &m_pISegStateMIDIExport, mtNow );
			}
		}
	}
}
	
BOOL CConductor::AllTransportsSetToPlay( void )
{
	// Check primary transport
	if( m_pToolbarHandler )
	{
		if( m_pToolbarHandler->m_fDisplayingPlay == FALSE )
		{
			return FALSE;
		}
	}
	
	// Check secondary toolbar
	POSITION pos = m_lstSecondaryToolbars.GetHeadPosition();
	while( pos )
	{
		CSecondaryToolbarHandler *pToolbarHandler = m_lstSecondaryToolbars.GetNext( pos );

		for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
		{
			if( pToolbarHandler->m_arrayButtons[i]->fDisplayingPlay == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*
CPortOptions *CConductor::FindPortByGUID( CTypedPtrList<CPtrList, CPortOptions*> *plstDlgPortOptions, GUID guidPort )
{
	// Validate parameters
	ASSERT( plstDlgPortOptions );

	// Search for a CDlgPortItem that matches guidPort
	POSITION pos = plstDlgPortOptions->GetHeadPosition();
	while( pos )
	{
		CPortOptions *pPortOptions = plstDlgPortOptions->GetNext( pos );
		if( ::IsEqualGUID( pPortOptions->m_guidPort, guidPort ) )
		{
			// Found a matching one - return it
			return pPortOptions;
		}
	}

	return NULL;
}
*/

HRESULT CConductor::CreateAudiopath( CAudiopathListItem *pAudiopathListItem, IDirectMusicAudioPath **ppIDirectMusicAudioPath )
{
	ASSERT( ppIDirectMusicAudioPath );
	*ppIDirectMusicAudioPath = NULL;

	// Nothing to create
	if( pAudiopathListItem == NULL )
	{
		return S_FALSE;
	}

	HRESULT hr;

	if( pAudiopathListItem->pNode )
	{
		// Try and get an AudiopathConfig object from the node
		IUnknown *punkAudiopathConfig;
		hr = pAudiopathListItem->pNode->GetObject( CLSID_DirectMusicAudioPathConfig, IID_IUnknown, (void**)&punkAudiopathConfig );
		if( FAILED( hr ) )
		{
			AfxMessageBox( IDS_ERR_APATH_GETCONFIG, MB_ICONEXCLAMATION | MB_OK );
		}
		else
		{
			hr = m_pDMPerformance->CreateAudioPath( punkAudiopathConfig, TRUE, ppIDirectMusicAudioPath );
			if( FAILED( hr ) )
			{
				if( hr == DSERR_BADSENDBUFFERGUID )
				{
					AfxMessageBox( IDS_ERR_APATH_CREATE_USER_NOSENDDEST, MB_ICONWARNING | MB_OK );
				}
				else if( hr == E_INVALIDARG )
				{
					AfxMessageBox( IDS_ERR_APATH_CREATE_USER_INVALIDARG, MB_ICONWARNING | MB_OK );
				}
				else if( hr == DMUS_E_AUDIOPATH_NOGLOBALFXBUFFER )
				{
					AfxMessageBox( IDS_ERR_APATH_CREATE_USER_NOENVDEST, MB_ICONWARNING | MB_OK );
				}
				else if( hr == E_NOINTERFACE )
				{
					AfxMessageBox( IDS_ERR_APATH_CREATE_USER_NOSYNTH, MB_ICONWARNING | MB_OK );
				}
				else
				{
					CString strError, strNumber;
					strNumber.Format("0x%x", hr );
					AfxFormatString1( strError, IDS_ERR_APATH_CREATE_USER, strNumber );
					AfxMessageBox( strError, MB_ICONWARNING | MB_OK );
				}
			}
			punkAudiopathConfig->Release();
		}
	}
	else
	{
		// Initialize dwNumPChannelsToAskFor
		DWORD dwNumPChannelsToAskFor = DEFAULT_NUM_PCHANNELS;

		if( GUID_Synth_Default != m_clsidDefaultSynth )
		{
			// Try and find the portcaps for the default synth
			DMUS_PORTCAPS dmPortCaps;
			if( GetPortCaps( m_pDMusic, m_clsidDefaultSynth, &dmPortCaps ) )
			{
				dwNumPChannelsToAskFor = min( dwNumPChannelsToAskFor, dmPortCaps.dwMaxChannelGroups * 16 );
			}
		}

		hr = m_pDMPerformance->CreateStandardAudioPath( pAudiopathListItem->dwStandardID, dwNumPChannelsToAskFor, TRUE, ppIDirectMusicAudioPath );
		if( FAILED( hr ) )
		{
			CString strError;
#ifdef DMP_XBOX
            if( (XBDM_CANNOTCONNECT == hr)
			||	(XBDM_CANNOTACCESS == hr) )
            {
                strError.LoadString( (XBDM_CANNOTCONNECT == hr) ? IDS_ERR_CANNOT_CONNECT_XBOX : IDS_ERR_WRONG_VERSION_XBOX );
    			if( IDYES == AfxMessageBox( strError, MB_ICONWARNING | MB_YESNO ) )
                {
                    DWORD dwValue = 0;
                    SetRegDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"), TEXT("EnableXboxSynth"), &dwValue, TRUE);
                    dwValue = TRUE;
                    SetRegDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"), TEXT("EnablePCSynth"), &dwValue, TRUE);
                    return CreateAudiopath( pAudiopathListItem, ppIDirectMusicAudioPath );
                }
            }
            else
#endif
            {
                strError.FormatMessage( IDS_ERR_APATH_CREATE_STANDARD, hr );
    			AfxMessageBox( strError, MB_ICONWARNING | MB_OK );
            }
		}
	}

	return hr;
}

void CConductor::UpdateLatency( void )
{
    g_fUsePhoneyDSound = m_fUsePhoneyDSound;
	if( m_fUsePhoneyDSound )
	{
		// Set the Phoney DSound latency
		g_dwLatency = m_dwLatency;

		// Clear the pointer to the last sampled output port
		g_rpLastSampledPort = NULL;
	}

	if( m_pSynthStatusToolbar )
	{
		BOOL bHandled = FALSE;
		m_pSynthStatusToolbar->OnResetClicked(0,0,0,bHandled);
	}

	if( m_dwDefaultLatency == LATENCY_UNSUPPORTED )
	{
		// Latency is unsupported - just return
		return;
	}

	::EnterCriticalSection( &m_csAudiopath );
	IDirectMusicAudioPath *pDMAudiopath = m_pDMAudiopath;
	if( pDMAudiopath )
	{
		pDMAudiopath->AddRef();
	}
	::LeaveCriticalSection( &m_csAudiopath );

	if( pDMAudiopath == NULL )
	{
		// No audiopath - just return
		return;
	}

	// Iterate through the default Audiopath's ports
	IDirectMusicPort *pDMPort = NULL;
	DWORD dwIndex = 0;
	while( S_OK == pDMAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
	{
		// Verify we have a valid port pointer
		ASSERT( pDMPort );

		IKsControl *pIKsControl;
		HRESULT hr = pDMPort->QueryInterface(IID_IKsControl, (void**)&pIKsControl);
		if (SUCCEEDED(hr)) 
		{
			KSPROPERTY ksp;
			ULONG cb;
			DWORD dwLatency = m_dwLatency;

			// Set the latency
			ZeroMemory(&ksp, sizeof(ksp));
			ksp.Set   = GUID_DMUS_PROP_WriteLatency;
			ksp.Id    = 0;
			ksp.Flags = KSPROPERTY_TYPE_SET;

			pIKsControl->KsProperty(&ksp,
								 sizeof(ksp),
								 (LPVOID)&dwLatency,
								 sizeof(dwLatency),
								 &cb);

			// Set the write period
			dwLatency = m_dwLatency / 2;
			dwLatency = max( 2, dwLatency );
			ZeroMemory(&ksp, sizeof(ksp));
			ksp.Set   = GUID_DMUS_PROP_WritePeriod;
			ksp.Id    = 0;
			ksp.Flags = KSPROPERTY_TYPE_SET;

			pIKsControl->KsProperty(&ksp,
								 sizeof(ksp),
								 (LPVOID)&dwLatency,
								 sizeof(dwLatency),
								 &cb);

			pIKsControl->Release();
		}

		// Release the port
		pDMPort->Release();

		// Go on to the next port in the audio path
		dwIndex++;
	}

	pDMAudiopath->Release();

	// Clear the pointer to the last sampled output port
	g_rpLastSampledPort = NULL;
}

void CConductor::UpdateAudioSettings( void )
{
	// Display an hourglass cursor
	CWaitCursor waitCursor;

	// Save an old pointer to the audio path
	EnterCriticalSection( &m_csAudiopath );
	IDirectMusicAudioPath *pOldAudiopath = m_pDMAudiopath;
	if( pOldAudiopath )
	{
		pOldAudiopath->AddRef();
	}

	// Ensure that there is no default audiopath
	// This also releases our pointer to the old audio path
	ActivateAudiopath( NULL, false );

	// Unload the GM set
	DownOrUnLoadGM();

	// Clean up the performance
	RELEASE( m_pDMAudiopath );
	LeaveCriticalSection( &m_csAudiopath );

	// Broadcast the WaveUnload notification
	if( pOldAudiopath )
	{
		BroadCastWaveNotification( GUID_ConductorUnloadWaves );
	}

	// Clear the pointer to the last sampled output port
	g_rpLastSampledPort = NULL;

	// If we were previously using an audiopath from a node, set the old audio path
	// to be inactive
	if( m_pAudiopathListItem
	&&	m_pAudiopathListItem->pNode )
	{
		// Try and get the IDMUSProdAudioPathInUse interface for the node
		IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
		if( SUCCEEDED( m_pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
		{
			// Notify the Audiopath that the old audio path is no longer in use
			pIDMUSProdAudiopathInUse->UsingAudioPath( pOldAudiopath, FALSE );
			pIDMUSProdAudiopathInUse->Release();
		}

		m_pFrameWork->RemoveFromNotifyList( m_pAudiopathListItem->pNode, this );
	}

	RELEASE( pOldAudiopath );

	BroadCastPortRemovalNotification();

	m_pDMPerformance->Stop( NULL, NULL, 0, 0 );

	Sleep( 500 );

	/* Are these calls necessary ?
	m_pDMPerformance->SetDefaultAudiopath( NULL );

	m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_SEGMENT );
	m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_MEASUREANDBEAT );
	m_pDMPerformance->RemoveNotificationType( GUID_NOTIFICATION_PERFORMANCE );

	m_pDMPerformance->SetNotificationHandle( NULL, 0 );
	*/

	m_pDMPerformance->CloseDown();

	// Re-Initialize the performance
	IDirectMusic *pDMusic;
	if( FAILED( m_pDMusic->QueryInterface( IID_IDirectMusic, (void **)&pDMusic ) ) )
	{
		ASSERT(FALSE);
		return;
	}

	// Initialize the performance
	DMUS_AUDIOPARAMS dmAudioParams;
	ZeroMemory( &dmAudioParams, sizeof( DMUS_AUDIOPARAMS ) );
	dmAudioParams.dwSize = sizeof( DMUS_AUDIOPARAMS );
	GetAudioParams( &dmAudioParams );

	// Initialize default latency
	m_dwDefaultLatency = LATENCY_UNSUPPORTED;

	HRESULT hr;
    g_fUsePhoneyDSound = m_fUsePhoneyDSound;
	if( m_fUsePhoneyDSound )
	{
		if( !m_pPhoneyDSound )
		{
			m_pPhoneyDSound = new CPhoneyDSound;
		}

		if( m_pPhoneyDSound )
		{
			// Need to intercept CoCreateInstance so that we can use our own BufferConfig object
			if( DynamicTrampoline == NULL )
			{
				HRESULT (STDAPICALLTYPE *DynamicTarget)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
								IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = &CoCreateInstance;
				DynamicTrampoline = (FUNCPTR_CoCreate)DetourFunction( (PBYTE)DynamicTarget, (PBYTE)DynamicDetour );
			}

			IDirectSound *pDSound;
			m_pPhoneyDSound->QueryInterface(IID_IDirectSound,(void **)&pDSound);
			hr = m_pDMPerformance->InitAudio( &pDMusic, &pDSound, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams);
			pDSound->Release();
		}
		else
		{
			g_fUsePhoneyDSound = m_fUsePhoneyDSound = FALSE;
#ifndef DMP_XBOX 
			// First, remove the trampoline function for PC version.
			if( DynamicTrampoline )
			{
				DetourRemove((PBYTE)DynamicTrampoline, (PBYTE)DynamicDetour);
				DynamicTrampoline = NULL;
			}
#endif

			hr = m_pDMPerformance->InitAudio( &pDMusic, NULL, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams );
		}
	}
	else
	{
#ifndef DMP_XBOX 
		// First, remove the trampoline function for PC version
		if( DynamicTrampoline )
		{
			DetourRemove((PBYTE)DynamicTrampoline, (PBYTE)DynamicDetour);
			DynamicTrampoline = NULL;
		}
#endif

		hr = m_pDMPerformance->InitAudio( &pDMusic, NULL, m_hWndFramework, 0, 0, DMUS_AUDIOF_ALL, &dmAudioParams );
	}
	
	// Release the base IDirectMusic interface
	RELEASE( pDMusic );

	// Check to see if the performance initialized correctly.
	if( FAILED(hr) )
	{
		AfxMessageBox( IDS_ERR_INIT_PERF, MB_ICONEXCLAMATION | MB_OK );
		//strError.LoadString( IDS_ERR_INIT_PERF );
		//return hr;
	}

	// Ensure AutoDownload is NOT set
	BOOL fAutoDownload;
	fAutoDownload = FALSE;
	m_pDMPerformance->SetGlobalParam( GUID_PerfAutoDownload, &fAutoDownload, sizeof(BOOL) );

	// If the ratio button is checked, update GUID_PerfMasterTempo
	if ( (m_pToolbarHandler->m_buttonRatio.GetState() & 1) )
	{
		float fModifier = float(m_nRatio) / 100.0f;
		m_pDMPerformance->SetGlobalParam( GUID_PerfMasterTempo, &fModifier, sizeof(float) );
	}

	IDirectMusicGraph *pGraph = NULL;
	if( FAILED( m_pDMPerformance->GetGraph( &pGraph ) ) )
	{
		if( SUCCEEDED( ::CoCreateInstance( CLSID_DirectMusicGraph, NULL, CLSCTX_INPROC_SERVER, IID_IDirectMusicGraph, (void **)&pGraph ) ) )
		{
			if( FAILED( m_pDMPerformance->SetGraph( pGraph ) ) )
			{
				pGraph->Release();
				pGraph = NULL;
			}
		}
	}

	if( pGraph )
	{
		pGraph->InsertTool( m_pNotifyTool, NULL, 0, 0 );
		pGraph->Release();
	}

	m_pDMPerformance->SetNotificationHandle( m_hNotifyEvent, 0 );
	m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_SEGMENT );
	m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_MEASUREANDBEAT );
	m_pDMPerformance->AddNotificationType( GUID_NOTIFICATION_PERFORMANCE );

	if( m_pAudiopathListItem )
	{
		IDirectMusicAudioPath *pAudiopath = NULL;
		hr = CreateAudiopath( m_pAudiopathListItem, &pAudiopath );
		hr = ActivateAudiopath( pAudiopath );

		// If there is a new audio path, notify the Audiopath node that it now in use
		if( pAudiopath
		&&	SUCCEEDED( hr )
		&&	m_pAudiopathListItem
		&&	m_pAudiopathListItem->pNode )
		{
			// Try and get the IDMUSProdAudioPathInUse interface for the node
			IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
			if( SUCCEEDED( m_pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
			{
				pIDMUSProdAudiopathInUse->UsingAudioPath( pAudiopath, TRUE );
				pIDMUSProdAudiopathInUse->Release();
			}

			m_pFrameWork->AddToNotifyList( m_pAudiopathListItem->pNode, this );
		}

		if( !pAudiopath
		||	FAILED( hr ) )
		{
			// Initialize the ListItem pointer to NULL
			m_pAudiopathListItem = NULL;

			// Ensure that no audiopath is set as the default
			// This will also release our pointer to the existing audio path
			ActivateAudiopath( NULL, true );

			// Remove all items from the combo box, then add and select the Audiopath used by the conductor
			m_pOptionsToolbarHandler->UpdateComboBoxFromConductor();
		}
		else
		{
			// Broadcast the WaveDownload notification
			BroadCastWaveNotification( GUID_ConductorDownloadWaves );
		}

		RELEASE( pAudiopath );
	}

	// Notify all other components that output has been changed
	BroadCastPortChangeNotification();
}

HRESULT STDMETHODCALLTYPE CConductor::GetDefaultAudiopathNode( IDMUSProdNode **ppAudiopathNode )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ppAudiopathNode == NULL )
	{
		return E_POINTER;
	}

	if( m_pAudiopathListItem == NULL )
	{
		return E_FAIL;
	}

	if( m_pAudiopathListItem->pNode == NULL )
	{
		CDMOInfoProxy *pDMOInfoProxy = new CDMOInfoProxy( m_pAudiopathListItem->dwStandardID );
		if( pDMOInfoProxy )
		{
			// QI for the IDMUSProdNode interface
			HRESULT hr = pDMOInfoProxy->QueryInterface( IID_IDMUSProdNode, (void**)ppAudiopathNode );

			// Release our reference, leaving the one added by the QI
			pDMOInfoProxy->Release();

			// Return the result of the QI
			return hr;
		}
		return E_OUTOFMEMORY;
	}

	return m_pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdNode, (void **)ppAudiopathNode );
}

HRESULT STDMETHODCALLTYPE CConductor::PleaseRedownload( void )
{
	// Broadcast the WaveUnload notification
	BroadCastWaveNotification( GUID_ConductorUnloadWaves );

	// Broadcast the WaveDownload notification
	BroadCastWaveNotification( GUID_ConductorDownloadWaves );

	if( m_fDownloadGM )
	{
		m_fDownloadGM = FALSE;
		DownOrUnLoadGM();
		m_fDownloadGM = TRUE;
		DownOrUnLoadGM();
	}

	if( m_fDownloadDLS )
	{
		m_fDownloadDLS = FALSE;
		DownOrUnLoadDLS();
		m_fDownloadDLS = TRUE;
		DownOrUnLoadDLS();
	}

	return S_OK;
}

void CConductor::DownOrUnLoadGM( void )
{
	// Unload any instruments previously downloaded
	while( !m_lstDownloadedInstrumentList.IsEmpty() )
	{
		delete m_lstDownloadedInstrumentList.RemoveHead();
	}

	IDirectMusicAudioPath *pDMAudioPath;
	EnterCriticalSection( &m_csAudiopath );
	pDMAudioPath = m_pDMAudiopath;
	if( pDMAudioPath )
	{
		pDMAudioPath->AddRef();
	}
	LeaveCriticalSection( &m_csAudiopath );


	// Now, download GM (if necessary)
	if( m_fDownloadGM && pDMAudioPath )
	{
		// Find out the list of ports to download to
		IDirectMusicPort *pDMPort = NULL;

		// Iterate through the default Audiopath's port
		DMUS_PORTCAPS dmPortCaps;
		DWORD dwIndex = 0;
		while( S_OK == pDMAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void **)&pDMPort ) )
		{
			// Verify we have a valid port pointer
			ASSERT( pDMPort );

			// Initialize the DMUS_PORTCAPS structure
			dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);

			// Try and get the capabilities of this port, and check if it supports DLS
			// and doesn't already have GS in hardware
			if( SUCCEEDED( pDMPort->GetCaps( &dmPortCaps ) )
			&&	(dmPortCaps.dwFlags & DMUS_PC_DLS)
			&&	!(dmPortCaps.dwFlags & DMUS_PC_GSINHARDWARE) )
			{
				CDownloadedInstrumentList *pDownloadedInstrumentList = new CDownloadedInstrumentList;
				if( pDownloadedInstrumentList )
				{
					pDownloadedInstrumentList->m_pDMPort = pDMPort;
					pDownloadedInstrumentList->m_pDMPort->AddRef();
					m_lstDownloadedInstrumentList.AddTail( pDownloadedInstrumentList );
				}
			}

			// Release the port
			pDMPort->Release();

			// Go on to the next port in the audio path
			dwIndex++;
		}

		// Now, download the GM set (if the port list is not empty)
		if( !m_lstDownloadedInstrumentList.IsEmpty() )
		{
			// Get the DirectMusic loader
			IDirectMusicLoader *pILoader = NULL;
			HRESULT hr = m_pFrameWork->GetSharedObject( CLSID_DirectMusicLoader, IID_IDirectMusicLoader, 
														(LPVOID*)&pILoader);
			if ( FAILED(hr) )
			{
				hr = CoCreateInstance(CLSID_DirectMusicLoader,
										  NULL,
										  CLSCTX_INPROC_SERVER,
										  IID_IDirectMusicLoader,
										  (LPVOID*)&pILoader);
			}

			/*
			if ( FAILED(hr) )
			{
				//strError.LoadString( IDS_ERR_CREATE_COLLEC );
			}
			else*/if( SUCCEEDED( hr ) )
			{
				// Download the GM collection
				IDirectMusicCollection *pDMCollection;
				DMUS_OBJECTDESC desc;
				
				desc.dwSize = sizeof(desc);
				desc.guidClass = CLSID_DirectMusicCollection;
				desc.guidObject = GUID_DefaultGMCollection;
				desc.dwValidData = (DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT);
				hr = pILoader->GetObject(&desc, IID_IDirectMusicCollection, (void**) &pDMCollection);

				RELEASE( pILoader );

				if( FAILED( hr ) )
				{
					AfxMessageBox( IDS_ERR_LOAD_GMDLS, MB_ICONEXCLAMATION | MB_OK );
					//return S_OK;
				}
				else //if( SUCCEEDED( hr ) )
				{
					IDirectMusicInstrument* pInstrument;
					IDirectMusicDownloadedInstrument *pDownloadedInstrument;

					// Initialize progress bar
					HANDLE hKeyProgressBar;
					CString strPrompt;
					strPrompt.LoadString( IDS_DOWNLOADING_GM );
					BSTR bstrPrompt = strPrompt.AllocSysString();
					m_pFrameWork->StartProgressBar( 0, 235, bstrPrompt, &hKeyProgressBar );

					int nInst = 0;
					DWORD dwPatch;
					hr = pDMCollection->EnumInstrument( nInst, &dwPatch, NULL, NULL );
					while ( hr == S_OK )
					{
						// Update Progress bar
						m_pFrameWork->SetProgressBarPos( hKeyProgressBar, nInst );

						// Get instrument
						if( SUCCEEDED( pDMCollection->GetInstrument(dwPatch, &pInstrument) ) )
						{
							POSITION pos = m_lstDownloadedInstrumentList.GetHeadPosition();
							while( pos )
							{
								CDownloadedInstrumentList *pDownloadedInstrumentList = m_lstDownloadedInstrumentList.GetNext( pos );
								// Download instrument
								if( pDownloadedInstrumentList->m_pDMPort &&
									SUCCEEDED(pDownloadedInstrumentList->m_pDMPort->DownloadInstrument( pInstrument, &pDownloadedInstrument, NULL, 0 )))
								{
									pDownloadedInstrumentList->m_lstDownloadedInstruments.AddHead( pDownloadedInstrument );
								}
							}

							// If this is a drum kit
							if( dwPatch & 0x80000000 )
							{
								IDirectMusicInstrument* pInstrumentTmp;
								// And a corresponding XG drum kit isn't in the collection
								if( FAILED( pDMCollection->GetInstrument((dwPatch | 0x007F0000) & 0x7FFFFFFF, &pInstrumentTmp) ) )
								{
									// Change the patch to be an XG drum kit
									pInstrument->SetPatch( ((dwPatch | 0x007F0000) & 0x7FFFFFFF) );

									pos = m_lstDownloadedInstrumentList.GetHeadPosition();
									while( pos )
									{
										CDownloadedInstrumentList *pDownloadedInstrumentList = m_lstDownloadedInstrumentList.GetNext( pos );
										// Download instrument
										if( pDownloadedInstrumentList->m_pDMPort &&
											SUCCEEDED(pDownloadedInstrumentList->m_pDMPort->DownloadInstrument( pInstrument, &pDownloadedInstrument, NULL, 0 )))
										{
											pDownloadedInstrumentList->m_lstDownloadedInstruments.AddHead( pDownloadedInstrument );
										}
									}
								}
								else
								{
									// A corresponding XG drum kit already exists - don't download
									pInstrumentTmp->Release();
								}
							}
							RELEASE( pInstrument );
						}
						nInst++;

						hr = pDMCollection->EnumInstrument( nInst, &dwPatch, NULL, NULL );
					}

					// End progress bar
					m_pFrameWork->EndProgressBar( hKeyProgressBar );

					hr = S_OK;

					RELEASE( pDMCollection );
				}
			}
		}
	}

	if( pDMAudioPath )
	{
		pDMAudioPath->Release();
	}

	if( m_pFrameWork )
	{
		HRESULT hr;
		IDMUSProdComponent* pIComponent;
		IDMUSProdComponent* pINextComponent;

		hr = m_pFrameWork->GetFirstComponent( &pINextComponent );

		while( SUCCEEDED( hr ) && pINextComponent )
		{
			pIComponent = pINextComponent;

			IDMUSProdDLSNotify *pIDMUSProdDLSNotify;
			if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdDLSNotify, (void**)&pIDMUSProdDLSNotify ) ) )
			{
				pIDMUSProdDLSNotify->OnDownloadGM( m_fDownloadGM );
				pIDMUSProdDLSNotify->Release();
			}

			hr = m_pFrameWork->GetNextComponent( pIComponent, &pINextComponent );
			pIComponent->Release();
		}
	}
}

void CConductor::DownOrUnLoadDLS( void )
{
	if( m_pFrameWork )
	{
		HRESULT hr;
		IDMUSProdComponent* pIComponent;
		IDMUSProdComponent* pINextComponent;

		hr = m_pFrameWork->GetFirstComponent( &pINextComponent );

		while( SUCCEEDED( hr ) && pINextComponent )
		{
			pIComponent = pINextComponent;

			IDMUSProdDLSNotify *pIDMUSProdDLSNotify;
			if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdDLSNotify, (void**)&pIDMUSProdDLSNotify ) ) )
			{
				pIDMUSProdDLSNotify->OnDownloadCustomDLS( m_fDownloadDLS );
				pIDMUSProdDLSNotify->Release();
			}

			hr = m_pFrameWork->GetNextComponent( pIComponent, &pINextComponent );
			pIComponent->Release();
		}
	}
}

// IDMUSProdNode
HRESULT STDMETHODCALLTYPE CConductor::GetNodeImageIndex( short* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetFirstChild( IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetNextChild( IDMUSProdNode*, IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetComponent( IDMUSProdComponent** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetDocRootNode( IDMUSProdNode** )
{
	//ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::SetDocRootNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetParentNode( IDMUSProdNode** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::SetParentNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetNodeId( GUID* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetNodeName( BSTR* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetNodeNameMaxLength( short* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::ValidateNodeName( BSTR )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::SetNodeName( BSTR )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetNodeListInfo( DMUSProdListInfo* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetEditorClsId( CLSID* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetEditorTitle( BSTR* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetEditorWindow( HWND* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::SetEditorWindow( HWND )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::UseOpenCloseImages( BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CConductor::GetRightClickMenuId( HINSTANCE*, UINT* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::OnRightClickMenuInit( HMENU )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::OnRightClickMenuSelect( long )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::DeleteChildNode( IDMUSProdNode*, BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::InsertChildNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::DeleteNode( BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::OnNodeSelChanged( BOOL )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CreateDataObject( IDataObject** )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanCut()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanCopy()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanDelete()
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanDeleteChildNode( IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanPasteFromData( IDataObject*, BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::PasteFromData( IDataObject* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::CanChildPasteFromData( IDataObject*, IDMUSProdNode*, BOOL* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::ChildPasteFromData( IDataObject*, IDMUSProdNode* )
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CConductor::GetObject( REFCLSID, REFIID, void** ) 
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}
void CConductor::BroadCastWaveNotification( REFGUID rguidNotification )
{
	if( m_pFrameWork )
	{
		HRESULT hr;
		IDMUSProdComponent* pIComponent;
		IDMUSProdComponent* pINextComponent;

		hr = m_pFrameWork->GetFirstComponent( &pINextComponent );

		while( SUCCEEDED( hr ) && pINextComponent )
		{
			pIComponent = pINextComponent;

			IDMUSProdNotifySink *pIDMUSProdNotifySink;
			if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pIDMUSProdNotifySink ) ) )
			{
				pIDMUSProdNotifySink->OnUpdate( NULL, rguidNotification, NULL );
				pIDMUSProdNotifySink->Release();
			}

			hr = m_pFrameWork->GetNextComponent( pIComponent, &pINextComponent );
			pIComponent->Release();
		}
	}
}

void CConductor::UsePhoneyDSound( bool fUsePhoneyDSound )
{
	if( m_fUsePhoneyDSound != fUsePhoneyDSound )
	{
		// Stop exporting wave and/or MIDI files
		if( m_pWaveRecordToolbar )
		{
			m_pWaveRecordToolbar->StopAll();
		}

		m_fUsePhoneyDSound = fUsePhoneyDSound;
	}
}
