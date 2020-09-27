// DlgMIDI.cpp : implementation file
//

#include "stdafx.h"
#include "directks.h"

#include "DlgMIDI.h"
#include "cconduct.h"
#include <mmsystem.h>
#include "Toolbar.h"
#include "OutputTool.h"
#include "dslink.h"
#include "audiosink.h"
#include "DlgEchoAdvanced.h"
#ifdef USE_LATENCY_HELPTEXT
#include <dmksctrl.h>
#endif // USE_LATENCY_HELPTEXT
#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMIDIInputContainer* g_pMIDIInputContainer;
extern bool g_fKslInitialized;
#ifdef _DEBUG
extern LogClass g_publicLogClass;
#endif

struct LowLatencyPortItem
{
	char *szFilterName;
	DWORD dwMinimumSampleFrequency;
	DWORD dwMaximumSampleFrequency;
};

static s_dwLastRealDefaultLatency = LATENCY_UNSUPPORTED;
#ifdef USE_LATENCY_HELPTEXT
static s_dwLastPhoneyBaseLatency = LATENCY_UNSUPPORTED;
static s_dwLastRealBaseLatency = LATENCY_UNSUPPORTED;

REFERENCE_TIME SampleLatency( IDirectMusicPort *pIDMOutputPort, IDirectMusicPerformance *pDMPerformance );

DWORD DialogSampleLatency( bool fUsePhoneyDSound )
{
	IDirectMusicAudioPath *pDMAudiopath = g_pconductor->m_pDMAudiopath;
	IDirectMusicPerformance *pDMPerformance = g_pconductor->m_pDMPerformance;

	if( !pDMPerformance
	||	!pDMAudiopath )
	{
		return 0;
	}

	DWORD dwSampledLatency = 0;

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
				DWORD dwLatency = DWORD((5000 + SampleLatency( pDMPort, pDMPerformance)) / 10000);
				dwSampledLatency = max( dwSampledLatency, dwLatency );
			}

			pIKsControl->Release();
		}

		// Release the port
		pDMPort->Release();

		// Go on to the next port in the audio path
		dwIndex++;
	}

	if( fUsePhoneyDSound )
	{
		if( S_OK == pDMAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, CLSID_DirectMusicSynth, 0, IID_IDirectMusicPort, (void **)&pDMPort ) )
		{
			DWORD dwLatency = DWORD((5000 + SampleLatency( pDMPort, pDMPerformance)) / 10000);
			dwSampledLatency = max( dwSampledLatency, dwLatency );

			// Release the port
			pDMPort->Release();
		}
	}
	return dwSampledLatency;
}
#endif // USE_LATENCY_HELPTEXT

/////////////////////////////////////////////////////////////////////////////
// CROEdit

/*
CROEdit::CROEdit()
{
}

CROEdit::~CROEdit()
{
}


BEGIN_MESSAGE_MAP(CROEdit, CEdit)
	//{{AFX_MSG_MAP(CROEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define MY_BACKGROUND_COLOR ::GetSysColor( COLOR_WINDOW )
/////////////////////////////////////////////////////////////////////////////
// CROEdit message handlers

HBRUSH CROEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	UNREFERENCED_PARAMETER(nCtlColor);
	if( ::IsWindowEnabled( this->m_hWnd ) )
	{
		// TODO: Change any attributes of the DC here
		pDC->SetBkColor( MY_BACKGROUND_COLOR );

		// TODO: Return a non-NULL brush if the parent's handler should not be called
		LOGBRUSH lb;
		lb.lbColor = MY_BACKGROUND_COLOR;
		lb.lbHatch = 0;
		lb.lbStyle = BS_SOLID;
		return ::CreateBrushIndirect( &lb );
	}
	else
	{
		return NULL;
	}
}
*/

/////////////////////////////////////////////////////////////////////////////
// DlgMIDI dialog


DlgMIDI::DlgMIDI(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMIDI::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgMIDI)
	//}}AFX_DATA_INIT

	m_nInitialMIDIInIndex = -1;
	m_nInitialEchoMIDIInIndex = -1;

	if( g_pMIDIInputContainer )
	{
		for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
		{
			m_anCurrentBlock[i] = g_pMIDIInputContainer->m_aMIDIInputContainer[i+1].m_dwPChannelBase / 16;
			m_aguidEchoMIDIInPort[i] = g_pMIDIInputContainer->m_aMIDIInputContainer[i+1].m_guidPort;
		}
	}
	else
	{
		ZeroMemory( m_anCurrentBlock, sizeof(int) * ECHO_ADVANCED_PORTS );
		ZeroMemory( m_aguidEchoMIDIInPort, sizeof(GUID) * ECHO_ADVANCED_PORTS );
	}

	m_dwLatency = 0;
#ifdef USE_LATENCY_HELPTEXT
	m_dwBaseLatency = 0;
#endif // USE_LATENCY_HELPTEXT
	m_fLatencyAppliesToAllAudioPaths = FALSE;

#ifndef DMP_XBOX
	m_dwSampleRate = 22050;
#else
	m_dwSampleRate = 48000;
#endif
	m_dwOtherSampleRate = 22050;
	m_dwVoices = 64;
	m_clsidDefaultSynth = GUID_Synth_Default;

#ifndef DMP_XBOX
	m_fDownloadGM = true;
#else
	m_fDownloadGM = false;
#endif
	m_fDownloadDLS = true;

	m_fUsePhoneyDSound = false;
#ifdef USE_LATENCY_HELPTEXT
	m_fOrigUsePhoneyDSound = false;
#endif // USE_LATENCY_HELPTEXT
	ZeroMemory( m_strPhoneyFilterName, sizeof( TCHAR ) * MAX_PATH );
}


void DlgMIDI::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgMIDI)
	DDX_Control(pDX, IDC_EDIT_OTHER, m_editOther);
	DDX_Control(pDX, IDC_COMBO_LOW_LAT_DEVICE, m_comboLowLatDevice);
	DDX_Control(pDX, IDC_DEFAULT_SYNTH, m_comboDefaultSynth);
	DDX_Control(pDX, IDC_EDIT_VOICES, m_editVoices);
	DDX_Control(pDX, IDC_SPIN_VOICES, m_spinVoices);
	DDX_Control(pDX, IDC_EDIT_LATENCY, m_editLatency);
	DDX_Control(pDX, IDC_SPIN_LATENCY, m_spinLatency);
	DDX_Control(pDX, IDC_CHECK_ECHO_MIDI, m_btnEchoMidi);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT, m_comboEchoInput);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS, m_spinEchoPChannels);
	DDX_Control(pDX, IDC_MIDI_INPUT, m_comboInput);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS, m_editEchoPChannels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgMIDI, CDialog)
	//{{AFX_MSG_MAP(DlgMIDI)
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS, OnDeltaposSpinEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS, OnKillfocusEditEchoPchannels)
	ON_CBN_SELCHANGE(IDC_COMBO_ECHO_INPUT, OnSelchangeComboEchoInput)
	ON_BN_CLICKED(IDC_BUTTON_LATENCY_DEFAULT, OnButtonLatencyDefault)
	ON_BN_CLICKED(IDC_CHECK_LOW_LATENCY, OnCheckLowLatency)
	ON_CBN_SELCHANGE(IDC_COMBO_LOW_LAT_DEVICE, OnSelchangeComboLowLatDevice)
	ON_CBN_SELCHANGE(IDC_DEFAULT_SYNTH, OnSelchangeDefaultSynth)
	ON_BN_CLICKED(IDC_11, On1122444896)
	ON_BN_CLICKED(IDC_OTHER, OnOther)
	ON_BN_CLICKED(IDC_22, On1122444896)
	ON_BN_CLICKED(IDC_44, On1122444896)
	ON_BN_CLICKED(IDC_48, On1122444896)
	ON_BN_CLICKED(IDC_96, On1122444896)
	ON_BN_CLICKED(IDC_ECHO_ADVANCED, OnEchoAdvanced)
	//}}AFX_MSG_MAP
#ifdef USE_LATENCY_HELPTEXT
	ON_EN_CHANGE(IDC_EDIT_LATENCY, OnChangeEditLatency)
#endif // USE_LATENCY_HELPTEXT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgMIDI message handlers

BOOL DlgMIDI::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if ( m_comboInput.GetSafeHwnd() != NULL )
	{
		bool fFoundMIDIInput = false;
		bool fFoundEchoInput = false;
		bool fFoundDefaultOutput = false;
#ifdef USE_LATENCY_HELPTEXT
		m_fOrigUsePhoneyDSound = m_fUsePhoneyDSound;
#endif // USE_LATENCY_HELPTEXT

		// Initialize the port structures
		GUID guidInputPortGUID;
		ZeroMemory( &guidInputPortGUID, sizeof(GUID) );

		// If there is a DirectMusic MIDI In port, get its GUID
		if( g_pMIDIInputContainer
		&&	g_pMIDIInputContainer->m_aMIDIInputContainer[0].m_pPortContainer
		&&	g_pMIDIInputContainer->m_aMIDIInputContainer[0].m_pPortContainer->m_pPort )
		{
			guidInputPortGUID = g_pMIDIInputContainer->m_aMIDIInputContainer[0].m_guidPort;
		}

		// Loop through all available DMusic ports
		HRESULT	hr;
		DWORD dwDMIndex;
		DMUS_PORTCAPS dmpc;
		for( dwDMIndex = 0; ; dwDMIndex++ )
		{
			// Initialize the PORTCAPS structure
			ZeroMemory(&dmpc, sizeof(dmpc));
			dmpc.dwSize = sizeof(DMUS_PORTCAPS);

			// Enumerate the next port
			hr = g_pconductor->m_pDMusic->EnumPort( dwDMIndex, &dmpc );
			if(SUCCEEDED(hr) && hr != S_FALSE)
			{
				if( dmpc.dwClass == DMUS_PC_INPUTCLASS )
				{
					// Found an input port - add it to the combo box

					// Create a GUID to save this port's GUID
					GUID *pGuid;
					pGuid = new GUID;
					ASSERT( pGuid );
					CopyMemory( pGuid, &dmpc.guidPort, sizeof(GUID) );

					//TRACE("Adding %S %x\n", dmpc.wszDescription, dmpc.guidPort.Data1 );

					// Point the combo box item's data ptr at the port's GUID
					CString strDesc = dmpc.wszDescription;
					DWORD dwComboIndex = m_comboInput.AddString( strDesc );
					m_comboInput.SetItemDataPtr( dwComboIndex, (void *)pGuid );

					// If we found the currently selected port, select it
					if ( ::IsEqualGUID( guidInputPortGUID, dmpc.guidPort ) )
					{
						fFoundMIDIInput = true;
						m_comboInput.SetCurSel( dwComboIndex );
					}

					// Add the item to the Echo input combo box
					dwComboIndex = m_comboEchoInput.AddString( strDesc );
					m_comboEchoInput.SetItemDataPtr( dwComboIndex, (void *)pGuid );

					// If we found the currently selected port, select it
					if ( ::IsEqualGUID( g_pMIDIInputContainer->m_aMIDIInputContainer[1].m_guidPort, dmpc.guidPort ) )
					{
						fFoundEchoInput = true;
						m_comboEchoInput.SetCurSel( dwComboIndex );
					}
				}
				else if( dmpc.dwClass == DMUS_PC_OUTPUTCLASS )
				{
					// Found an output port - add it to the combo box

					// Create a GUID to save this port's GUID
					GUID *pGuid;
					pGuid = new GUID;
					ASSERT( pGuid );
					CopyMemory( pGuid, &dmpc.guidPort, sizeof(GUID) );

					//TRACE("Adding %S %x\n", dmpc.wszDescription, dmpc.guidPort.Data1 );

					// Point the combo box item's data ptr at the port's GUID
					CString strDesc = dmpc.wszDescription;
					DWORD dwComboIndex = m_comboDefaultSynth.AddString( strDesc );
					m_comboDefaultSynth.SetItemDataPtr( dwComboIndex, (void *)pGuid );

					// If we found the currently selected port, select it
					if ( ::IsEqualGUID( m_clsidDefaultSynth, dmpc.guidPort ) )
					{
						fFoundDefaultOutput = true;
						m_comboDefaultSynth.SetCurSel( dwComboIndex );
					}
				}
			}
			else
			{
				break;
			}
		}

		// Add a 'none' option - it has the data UINT_MAX
		CString strNoMIDI;
		strNoMIDI.LoadString( IDS_NO_MIDI_INPUT );

		// Add the 'none' option to the MIDI input box
		DWORD	dwComboIndex;
		dwComboIndex = m_comboInput.InsertString( 0, strNoMIDI );
		m_comboInput.SetItemData( dwComboIndex, UINT_MAX );
		m_comboInput.SetItemDataPtr( dwComboIndex, (void *)UINT_MAX );

		// By default, select 'None'
		if( !fFoundMIDIInput )
		{
			m_comboInput.SetCurSel(dwComboIndex);
		}

		// Add the 'none' option to the Echo MIDI input box
		dwComboIndex = m_comboEchoInput.InsertString( 0, strNoMIDI );
		m_comboEchoInput.SetItemData( dwComboIndex, UINT_MAX );
		m_comboEchoInput.SetItemDataPtr( dwComboIndex, (void *)UINT_MAX );

		// By default, select 'None'
		if( !fFoundEchoInput )
		{
			m_comboEchoInput.SetCurSel(dwComboIndex);
		}


		// Add the 'none' option to the Default Synth box
		strNoMIDI.LoadString( IDS_NO_DEFAULT_SYNTH );
		dwComboIndex = m_comboDefaultSynth.InsertString( 0, strNoMIDI );
		m_comboDefaultSynth.SetItemData( dwComboIndex, UINT_MAX );
		m_comboDefaultSynth.SetItemDataPtr( dwComboIndex, (void *)UINT_MAX );

		// By default, select 'None'
		if( !fFoundDefaultOutput )
		{
			m_comboDefaultSynth.SetCurSel(dwComboIndex);
		}


		// Save the initial MIDI input settings
		m_nInitialMIDIInIndex = m_comboInput.GetCurSel();
		m_nInitialEchoMIDIInIndex = m_comboEchoInput.GetCurSel();


		// Set the Enable/Disable checkbox correctly
		/*
		int iCurSel = m_comboEchoInput.GetCurSel();
		if( (iCurSel == -1)
		||	(m_comboEchoInput.GetItemDataPtr( iCurSel ) == (void *)UINT_MAX) )
		{
			m_btnEchoMidi.EnableWindow( FALSE );
			m_btnEchoMidi.SetCheck( 0 );
		}
		else
		*/
		{
			m_btnEchoMidi.EnableWindow( TRUE );
			m_btnEchoMidi.PostMessage( BM_SETCHECK, g_pMIDIInputContainer->m_fWantToEcho ? 1 : 0, 0 );
		}

		m_spinEchoPChannels.SetRange( 0, 250 );
		UpdatePChannelEdit( g_pMIDIInputContainer->m_aMIDIInputContainer[1].m_dwPChannelBase / 16 );

		// Set the latency settings
		if( m_fUsePhoneyDSound
		||	(g_pconductor->m_dwDefaultLatency != LATENCY_UNSUPPORTED) )
		{
			m_spinLatency.SetRange( 0, 9999 );
			m_editLatency.SetLimitText( 4 );
			m_spinLatency.SetPos( m_dwLatency );
			SetDlgItemInt( IDC_EDIT_LATENCY, m_dwLatency, FALSE );
			CheckDlgButton( IDC_CHECK_LATENCY_PERSIST, m_fLatencyAppliesToAllAudioPaths ? BST_CHECKED : BST_UNCHECKED );

			if( !m_fUsePhoneyDSound )
			{
				s_dwLastRealDefaultLatency = g_pconductor->m_dwDefaultLatency;
			}

#ifdef USE_LATENCY_HELPTEXT
			m_dwBaseLatency = DialogSampleLatency( m_fOrigUsePhoneyDSound ) - m_dwLatency;

			if( m_fOrigUsePhoneyDSound )
			{
				s_dwLastPhoneyBaseLatency = m_dwBaseLatency;
			}
			else
			{
				s_dwLastRealBaseLatency = m_dwBaseLatency;
			}

			UpdateLatencyText();
#endif // USE_LATENCY_HELPTEXT
		}
		else
		{
			// Latency is unsupported - disable the controls
			m_spinLatency.EnableWindow( FALSE );
			m_editLatency.EnableWindow( FALSE );
			EnableDlgItem( IDC_CHECK_LATENCY_PERSIST, false );
			EnableDlgItem( IDC_BUTTON_LATENCY_DEFAULT, false );
		}

		// Set the voices settings
		m_spinVoices.SetRange( 1, 999 );
		m_editVoices.SetLimitText( 3 );
		m_spinVoices.SetPos( m_dwVoices );
		SetDlgItemInt( IDC_EDIT_VOICES, m_dwVoices, FALSE );

		// Set the sample rate setting
		int nRadioButton = IDC_OTHER;
		switch( m_dwSampleRate )
		{
		case 11025:
			nRadioButton = IDC_11;
			break;
		case 22050:
			nRadioButton = IDC_22;
			break;
		case 44100:
			nRadioButton = IDC_44;
			break;
		case 48000:
			nRadioButton = IDC_48;
			break;
		case 96000:
			nRadioButton = IDC_96;
			break;
		default:
			m_dwOtherSampleRate = m_dwSampleRate;
			break;
		}
		CheckRadioButton( IDC_11, IDC_OTHER, nRadioButton );
		m_editOther.EnableWindow( nRadioButton == IDC_OTHER );

		m_editOther.SetLimitText( 5 );
		SetDlgItemInt( IDC_EDIT_OTHER, m_dwOtherSampleRate, FALSE );

		CheckRadioButton( IDC_RADIO_GM_DOWNLOAD, IDC_RADIO_GM_UNLOAD, m_fDownloadGM ? IDC_RADIO_GM_DOWNLOAD : IDC_RADIO_GM_UNLOAD );
		CheckRadioButton( IDC_RADIO_DLS_DOWNLOAD, IDC_RADIO_DLS_UNLOAD, m_fDownloadDLS ? IDC_RADIO_DLS_DOWNLOAD : IDC_RADIO_DLS_UNLOAD );

		// Get list of devices that support WDM audio
		if( !g_fKslInitialized  )
		{
#ifdef _DEBUG
			KslRegisterLog( &g_publicLogClass );
#endif
			KslInitKsLib();
			g_fKslInitialized = true;
		}

		WAVEFORMATEX wFormatEx;
		wFormatEx.wFormatTag = WAVE_FORMAT_PCM;
		wFormatEx.nChannels = (USHORT)2;
		wFormatEx.nSamplesPerSec = 22050;
		wFormatEx.nAvgBytesPerSec = 22050*2*16/8;
		wFormatEx.wBitsPerSample = (USHORT)16;
		wFormatEx.nBlockAlign = (USHORT)(2*16/8);
		wFormatEx.cbSize = 0;

		m_comboLowLatDevice.SetCurSel( -1 );

		bool fFoundFilter = false;

		CList<CKsFilter> lstFilters;
		GUID *pGuid = new GUID;
		*pGuid = KSCATEGORY_AUDIO_DEVICE;
		KslEnumFilters( &lstFilters, ePCMAudio, &pGuid, 1, TRUE, TRUE, FALSE );
		CNode<CKsFilter> *pNode = lstFilters.GetHead();
		while( pNode )
		{
			CKsFilter *pCKsFilter = (CKsFilter *)pNode->pData;

			CPCMAudioFilter PCMAudioFilter( pCKsFilter->m_szFilterName, pCKsFilter->m_szFriendlyName, pCKsFilter->m_szCLSID, pCKsFilter->m_szService, pCKsFilter->m_szBinary );
			CPCMAudioPin *pCPCMAudioPin = PCMAudioFilter.FindViablePin( &(pCKsFilter->m_listRenderSinkPins), &wFormatEx );
			if( pCPCMAudioPin )
			{
				pCPCMAudioPin->SetFormat( &wFormatEx );
				if( pCPCMAudioPin->Instantiate( TRUE ) )
				{
					pCPCMAudioPin->ClosePin();
					int nIndex = m_comboLowLatDevice.AddString( pCKsFilter->m_szFriendlyName );

					LowLatencyPortItem *pLowLatencyPortItem = new LowLatencyPortItem;
					if( pLowLatencyPortItem )
					{
						pLowLatencyPortItem->szFilterName = _strdup( pCKsFilter->m_szFilterName );
						pLowLatencyPortItem->dwMinimumSampleFrequency = pCPCMAudioPin->m_dwMinSampleFreq;
						pLowLatencyPortItem->dwMaximumSampleFrequency = pCPCMAudioPin->m_dwMaxSampleFreq;

						m_comboLowLatDevice.SetItemDataPtr( nIndex, pLowLatencyPortItem );

						if( strcmp( m_strPhoneyFilterName, pCKsFilter->m_szFilterName ) == 0 )
						{
							m_comboLowLatDevice.SetCurSel( nIndex );
						}
						else if( !fFoundFilter && m_strPhoneyFilterName[0] == 0 )
						{
							fFoundFilter = true;
							m_comboLowLatDevice.SetCurSel( nIndex );
						}
					}
				}
			}

			pNode = lstFilters.GetNext( pNode );
		}

		pNode = lstFilters.GetHead();
		while(pNode)
		{
			if (pNode->pData)
			{
				delete pNode->pData;
			}
			pNode = lstFilters.GetNext(pNode);
		}
		lstFilters.Empty();

		delete pGuid;

		if( m_comboLowLatDevice.GetCurSel() == CB_ERR )
		{
			m_comboLowLatDevice.SetCurSel( 0 );
		}

		if( m_comboLowLatDevice.GetCount() == 0 )
		{
			EnableDlgItem( IDC_CHECK_LOW_LATENCY, false );
			m_comboLowLatDevice.EnableWindow( FALSE );
			m_fUsePhoneyDSound = FALSE;
		}
		else
		{
			CheckDlgButton( IDC_CHECK_LOW_LATENCY, m_fUsePhoneyDSound ? BST_CHECKED : BST_UNCHECKED );
			m_comboLowLatDevice.EnableWindow( m_fUsePhoneyDSound );
		}

#ifdef DMP_XBOX
		CString strPCAudioSettings;
		strPCAudioSettings.LoadString( IDS_PC_AUDIO_INIT_SETTINGS );
		SetDlgItemText( IDC_STATIC_AUDIO_SETTINGS, strPCAudioSettings );
#endif

		// Restrict the Sample rate buttons, if necessary
		OnSelchangeComboLowLatDevice(); // From Low-latency device
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgMIDI::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if( m_comboDefaultSynth.GetSafeHwnd() != NULL )
	{
		GUID *pGuid;
		while( m_comboDefaultSynth.GetCount() > 0 )
		{
			pGuid = (GUID *) m_comboDefaultSynth.GetItemDataPtr( 0 );
			if( pGuid != (GUID *)UINT_MAX)
			{
				delete pGuid;
			}
			m_comboDefaultSynth.DeleteString( 0 );
		}
	}

	if ( m_comboInput.GetSafeHwnd() != NULL )
	{
		GUID *pGuid;
		while( m_comboInput.GetCount() > 0 )
		{
			pGuid = (GUID *) m_comboInput.GetItemDataPtr( 0 );
			if( pGuid != (GUID *)UINT_MAX)
			{
				delete pGuid;
			}
			m_comboInput.DeleteString( 0 );
		}
	}
	// Already deleted these GUIDs above (both comboboxes contain the same
	// strings and pointers to the same GUIDs).
	/*
	if ( m_comboEchoInput.GetSafeHwnd() != NULL )
	{
		while( m_comboEchoInput.GetCount() > 0 )
		{
			GUID *pGuid = (GUID *) m_comboEchoInput.GetItemDataPtr( 0 );
			if( pGuid != (GUID *)UINT_MAX)
			{
				delete pGuid;
			}
			m_comboEchoInput.DeleteString( 0 );
		}
	}
	*/

	if ( m_comboLowLatDevice.GetSafeHwnd() != NULL )
	{
		while( m_comboLowLatDevice.GetCount() > 0 )
		{
			LowLatencyPortItem *pLowLatencyPortItem = static_cast<LowLatencyPortItem *>(m_comboLowLatDevice.GetItemDataPtr( 0 ));
			if( pLowLatencyPortItem )
			{
				delete []pLowLatencyPortItem->szFilterName;
			}
			delete pLowLatencyPortItem;
			m_comboLowLatDevice.DeleteString( 0 );
		}
	}
}

void DlgMIDI::OnDeltaposSpinEchoPchannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// Calculate what the block # will change to
	int nNewBlock = m_anCurrentBlock[0] + pNMUpDown->iDelta;

	// If the block number is invalid (negative), return
	if( nNewBlock < 0 )
	{
		*pResult = 1;
		return;
	}

	// Update the text in the PChannel edit box
	UpdatePChannelEdit( nNewBlock );

	*pResult = 1;
}

void DlgMIDI::OnOK() 
{
	if (m_comboInput.GetSafeHwnd() == NULL)
	{
		CDialog::OnOK();
	}

	// Get the index of the currently selected item
	BOOL fContinue = TRUE;
	int index = m_comboInput.GetCurSel();
	if (index == CB_ERR)
	{
		// Error getting index of selected item - perhaps nothing's selected?
		ASSERT( FALSE );
		// Unexpected error, might as well close the dialog box
		//fContinue = FALSE;
		DisplayPortCreationError( 0, m_comboInput );
	}
	else if( index != m_nInitialMIDIInIndex )
	{
		// Change to the hourglass cursor
		CWaitCursor cWait;
		
		GUID *pGuid;
		if( m_comboInput.GetItemDataPtr( index ) == (void *)UINT_MAX )
		{
			g_pMIDIInputContainer->SetDMusicEchoMIDIIn( 0, 0, GUID_AllZeros );
		}
		else
		{
			pGuid = (GUID *) m_comboInput.GetItemDataPtr( index );
			HRESULT hrError = E_FAIL;
			if( pGuid )
			{
				hrError = g_pMIDIInputContainer->SetDMusicEchoMIDIIn( 0, 0, *pGuid );
			}
			else
			{
				hrError = g_pMIDIInputContainer->SetDMusicEchoMIDIIn( 0, 0, GUID_AllZeros );
			}
			if( FAILED( hrError ) )
			{
				TRACE("Failed to set MIDI In.\n");
				fContinue = FALSE;
				DisplayPortCreationError( hrError, m_comboInput );
			}
			else
			{
				m_nInitialMIDIInIndex = index;
			}
		}
	}

	if( fContinue )
	{
		// Change to the hourglass cursor
		CWaitCursor cWait;

		// Get the index of the currently selected item
		index = m_comboEchoInput.GetCurSel();
		if (index == CB_ERR)
		{
			// Error getting index of selected item - perhaps nothing's selected?
			// DisplayPortCreationError( 0, m_comboEchoInput );
			// Unexpected error, might as well close the dialog box
			//fContinue = FALSE;
			ASSERT( FALSE );
		}
		else
		{
			// Get the selected port GUID
			if( m_comboEchoInput.GetItemDataPtr( index ) == (void *)UINT_MAX )
			{
				ZeroMemory( &m_aguidEchoMIDIInPort[0], sizeof(GUID) );
			}
			else
			{
				GUID *pGuid = (GUID *) m_comboEchoInput.GetItemDataPtr( index );
				memcpy( &m_aguidEchoMIDIInPort[0], pGuid, sizeof(GUID) );
			}

			// Update the echo tool
			HRESULT hrError = g_pMIDIInputContainer->EnableEchoMIDI( m_btnEchoMidi.GetCheck() );

			if( SUCCEEDED( hrError ) )
			{
				for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
				{
					// Only do the update (pass true) for the last port
					hrError = g_pMIDIInputContainer->SetDMusicEchoMIDIIn( i + 1, (unsigned)m_anCurrentBlock[i], m_aguidEchoMIDIInPort[i], (ECHO_ADVANCED_PORTS - 1) == i );
				}
			}

			if( SUCCEEDED( hrError )
			&&	g_pMIDIInputContainer->m_fWantToEcho )
			{
				g_pMIDIInputContainer->EnableEchoMIDI( FALSE );
				hrError = g_pMIDIInputContainer->EnableEchoMIDI( TRUE );
			}

			if( FAILED( hrError ) )
			{
				TRACE("Failed to set Echo MIDI Input.\n");
				fContinue = FALSE;
				DisplayPortCreationError( hrError, m_comboEchoInput );
			}
		}
	}

	if( fContinue )
	{
		if( m_fUsePhoneyDSound
		||	g_pconductor->m_dwDefaultLatency != LATENCY_UNSUPPORTED )
		{
			BOOL fTrans;
			DWORD dwLatency = GetDlgItemInt( IDC_EDIT_LATENCY, &fTrans, FALSE );
			if( !fTrans )
			{
				fContinue = FALSE;
			}
			else
			{
				m_dwLatency = dwLatency;

				m_fLatencyAppliesToAllAudioPaths = IsDlgButtonChecked( IDC_CHECK_LATENCY_PERSIST ) ? true : false;
			}
		}
	}

	if( fContinue )
	{
		BOOL fTrans;
		DWORD dwVoices = GetDlgItemInt( IDC_EDIT_VOICES, &fTrans, FALSE );
		if( !fTrans
		||	(dwVoices < 1) )
		{
			fContinue = FALSE;
			AfxMessageBox( IDS_ERR_INVALID_VOICES );
		}
		else
		{
			m_dwVoices = dwVoices;
		}
	}

	if( fContinue )
	{
		// Get the index of the currently selected item
		index = m_comboDefaultSynth.GetCurSel();
		if (index == CB_ERR)
		{
			// Error getting index of selected item - perhaps nothing's selected?
			// DisplayPortCreationError( 0, m_comboEchoInput );
			// Unexpected error, might as well close the dialog box
			//fContinue = FALSE;
			ASSERT( FALSE );
		}
		else
		{
			// Get the selected default port GUID
			if( m_comboDefaultSynth.GetItemDataPtr( index ) == (void *)UINT_MAX )
			{
				m_clsidDefaultSynth = GUID_Synth_Default;
			}
			else
			{
				GUID *pGuid = (GUID *) m_comboDefaultSynth.GetItemDataPtr( index );
				memcpy( &m_clsidDefaultSynth, pGuid, sizeof(GUID) );
			}
		}

		switch( GetCheckedRadioButton( IDC_11, IDC_OTHER ) )
		{
		case IDC_11:
			m_dwSampleRate = 11025;
			break;
		default:
		case IDC_22:
			m_dwSampleRate = 22050;
			break;
		case IDC_44:
			m_dwSampleRate = 44100;
			break;
		case IDC_48:
			m_dwSampleRate = 48000;
			break;
		case IDC_96:
			m_dwSampleRate = 96000;
			break;
		case IDC_OTHER:
			{
				BOOL fTrans;
				DWORD dwOtherSampleRate = GetDlgItemInt( IDC_EDIT_OTHER, &fTrans, FALSE );

				DWORD dwLowSampleRate = 11025;
				DWORD dwHighSampleRate = 96000;
				if( m_fUsePhoneyDSound )
				{
					int nIndex = m_comboLowLatDevice.GetCurSel();
					ASSERT( nIndex != CB_ERR );

					LowLatencyPortItem *pLowLatencyPortItem = static_cast<LowLatencyPortItem *>(m_comboLowLatDevice.GetItemDataPtr( nIndex ));
					if( pLowLatencyPortItem )
					{
						dwLowSampleRate = max( dwLowSampleRate, pLowLatencyPortItem->dwMinimumSampleFrequency );
						dwHighSampleRate = min( dwHighSampleRate, pLowLatencyPortItem->dwMaximumSampleFrequency );
					}
				}

				if( !fTrans || dwOtherSampleRate < dwLowSampleRate || dwOtherSampleRate > dwHighSampleRate )
				{
					CString strInvalidSampleRate;
					strInvalidSampleRate.FormatMessage( IDS_ERR_INVALID_SAMPLERATE, dwLowSampleRate, dwHighSampleRate );
					AfxMessageBox( strInvalidSampleRate );
					return;
				}
				m_dwSampleRate = dwOtherSampleRate;
			}
			break;
		}

		m_fDownloadGM = (GetCheckedRadioButton( IDC_RADIO_GM_DOWNLOAD, IDC_RADIO_GM_UNLOAD ) == IDC_RADIO_GM_DOWNLOAD);
		m_fDownloadDLS = (GetCheckedRadioButton( IDC_RADIO_DLS_DOWNLOAD, IDC_RADIO_DLS_UNLOAD ) == IDC_RADIO_DLS_DOWNLOAD);

		m_fUsePhoneyDSound = IsDlgButtonChecked( IDC_CHECK_LOW_LATENCY ) ? true : false;

		if( m_fUsePhoneyDSound )
		{
			int nIndex = m_comboLowLatDevice.GetCurSel();
			ASSERT( nIndex != CB_ERR );
			LowLatencyPortItem *pLowLatencyPortItem = static_cast<LowLatencyPortItem *>(m_comboLowLatDevice.GetItemDataPtr( nIndex ));
			if( pLowLatencyPortItem
			&&	pLowLatencyPortItem->szFilterName )
			{
				strcpy( m_strPhoneyFilterName, pLowLatencyPortItem->szFilterName );
			}
		}

		CDialog::OnOK();
	}
}

void DlgMIDI::OnKillfocusEditEchoPchannels() 
{
	CString strText;
	m_editEchoPChannels.GetWindowText( strText );
	
	// Strip leading and trailing spaces
	strText.TrimRight();
	strText.TrimLeft();

	// If empty set value to previous one
	if( strText.IsEmpty() )
	{
		UpdatePChannelEdit( m_anCurrentBlock[0] );
	}
	else
	{
		// Convert from text to int
		int nNewBlock;
		TCHAR *tcstrTmp;
		tcstrTmp = new TCHAR[strText.GetLength() + 1];
		tcstrTmp[0] = 0;
		if( _stscanf( strText, "%d%s", &nNewBlock, tcstrTmp ) )
		{
			// Check bounds
			if( nNewBlock < 0 )
			{
				nNewBlock = 0;
			}
			else if( nNewBlock > 999 )
			{
				nNewBlock = 999;
			}

			nNewBlock /= 16;
			UpdatePChannelEdit( nNewBlock );
		}
		else
		{
			// Invalid data - reset control with previous value
			UpdatePChannelEdit( m_anCurrentBlock[0] );
		}

		delete tcstrTmp;
	}
}

void DlgMIDI::UpdatePChannelEdit( int nBlock )
{
	// Validate the block number
	if( nBlock < 0 )
	{
		return;
	}

	// Copy it to our local variable (since the spin control doesn't store it)
	m_anCurrentBlock[0] = nBlock;

	// Compute the text to display, and then display it
	CString strText, strFormat;
	strFormat.LoadString( IDS_PCHANNEL_GROUP_FORAMT );
	strText.Format( strFormat, nBlock * 16 + 1, nBlock * 16 + 16 );
	m_editEchoPChannels.SetWindowText( strText );
}

void DlgMIDI::DisplayPortCreationError( HRESULT hr, CComboBox &comboBox )
{
	// Failed to set Conductor's MIDI input
	CString strError, strFormat;
	strFormat.LoadString( IDS_ERR_UNKNOWN_CREATE_PORT );

	CString strName;
	comboBox.GetLBText( comboBox.GetCurSel(), strName );
	strError.Format( strFormat, hr, strName );

	AfxMessageBox( strError, MB_ICONEXCLAMATION | MB_OK );
}

void DlgMIDI::OnSelchangeComboEchoInput() 
{
	/*
	// Set the Enable/Disable checkbox correctly
	int iCurSel = m_comboEchoInput.GetCurSel();
	if( (iCurSel == -1)
	||	(m_comboEchoInput.GetItemDataPtr( iCurSel ) == (void *)UINT_MAX) )
	{
		m_btnEchoMidi.EnableWindow( FALSE );
		m_btnEchoMidi.SetCheck( 0 );
	}
	else
	{
		m_btnEchoMidi.EnableWindow( TRUE );
		m_btnEchoMidi.SetCheck( g_pMIDIInputContainer->m_fWantToEcho ? 1 : 0 );
	}
	*/
}

void DlgMIDI::OnButtonLatencyDefault() 
{
	DWORD dwDefaultLatency;
	if( m_fUsePhoneyDSound )
	{
		// If we're using Phoney DSound, get the default latency directly
		dwDefaultLatency = DEFAULT_PHONEY_DS_LATENCY;
	}
	else
	{
		// Otherwise, we're not using Phoney DSound, so get the last default
		// latency value we've seen.  If we started out using real DSound, then
		// this value is the current default latency

		dwDefaultLatency = s_dwLastRealDefaultLatency;
	}

	// Only update the values if we have a valid default latency value
	if( dwDefaultLatency != LATENCY_UNSUPPORTED )
	{
		m_spinLatency.SetPos( dwDefaultLatency );
		SetDlgItemInt( IDC_EDIT_LATENCY, dwDefaultLatency, FALSE );
	}
}

void DlgMIDI::OnCheckLowLatency() 
{
	// Set m_fUsePhoneyDSound if the button is checked
	m_fUsePhoneyDSound = IsDlgButtonChecked( IDC_CHECK_LOW_LATENCY ) ? true : false;

	// Enable the combobox if m_fUsePhoneyDSound is set 
	m_comboLowLatDevice.EnableWindow( m_fUsePhoneyDSound );

	// Update the latency value
	OnButtonLatencyDefault();

	// Update the sample rate buttons
	OnSelchangeComboLowLatDevice();

#ifdef USE_LATENCY_HELPTEXT
	// Update the helper text displayed next to the latency value
	UpdateLatencyText();
#endif // USE_LATENCY_HELPTEXT
}

#ifdef USE_LATENCY_HELPTEXT
void DlgMIDI::UpdateLatencyText( void )
{
	if( m_fUsePhoneyDSound
	||	g_pconductor->m_dwDefaultLatency != LATENCY_UNSUPPORTED )
	{
		// If we're using Phoney DSound, or if we're using normal DSound
		// and the default latency is not 'Unsupported'

		CString strText1, strText2;
		BOOL fTrans;
		DWORD dwLatency = GetDlgItemInt( IDC_EDIT_LATENCY, &fTrans, FALSE );
		if( !fTrans )
		{
			dwLatency = LATENCY_UNSUPPORTED;
		}

		DWORD dwBaseLatency = LATENCY_UNSUPPORTED;
		strText2.LoadString(IDS_UNKNOWN_LATENCY);

		if( m_fOrigUsePhoneyDSound == m_fUsePhoneyDSound )
		{
			dwBaseLatency = m_dwBaseLatency;
		}
		else if( m_fUsePhoneyDSound && s_dwLastPhoneyBaseLatency )
		{
			dwBaseLatency = s_dwLastPhoneyBaseLatency;
		}
		else if( !m_fUsePhoneyDSound && s_dwLastRealBaseLatency )
		{
			dwBaseLatency = s_dwLastRealBaseLatency;
		}
		if( dwBaseLatency == LATENCY_UNSUPPORTED )
		{
			strText1.LoadString(IDS_UNKNOWN_LATENCY);
		}
		else
		{
			strText1.Format( "%d", dwBaseLatency );
		}
		if( dwLatency == LATENCY_UNSUPPORTED )
		{
			strText2.LoadString(IDS_UNKNOWN_LATENCY);
		}
		else
		{
			strText2.Format( "%d", dwBaseLatency + dwLatency );
		}

		CString strDialog;
		AfxFormatString2( strDialog, IDS_LATENCY_TEXT, strText1, strText2 );
		SetDlgItemText( IDC_STATIC_MS_CAPTION, strDialog );
	}
	else
	{
		CString strDialog;
		if( strDialog.LoadString( IDS_MS_TEXT ) )
		{
			SetDlgItemText( IDC_STATIC_MS_CAPTION, strDialog );
		}
	}
}

void DlgMIDI::OnChangeEditLatency() 
{
	UpdateLatencyText();
}
#endif // USE_LATENCY_HELPTEXT

void DlgMIDI::RestrictFrequencyRange( const DWORD dwLowFreq, const DWORD dwHighFreq )
{
	EnableDlgItem( IDC_11, dwLowFreq <= 11025 && dwHighFreq >= 11025 );
	EnableDlgItem( IDC_22, dwLowFreq <= 22050 && dwHighFreq >= 22050 );
	EnableDlgItem( IDC_44, dwLowFreq <= 44100 && dwHighFreq >= 44100 );
	EnableDlgItem( IDC_48, dwLowFreq <= 48000 && dwHighFreq >= 48000 );
	EnableDlgItem( IDC_96, dwLowFreq <= 96000 && dwHighFreq >= 96000 );

	// Ensure that an enabled radio button is checked
	int nID = GetCheckedRadioButton( IDC_11, IDC_OTHER );

	// First, try and lower the frequency
	while( !IsDlgItemEnabled( nID ) && (nID > IDC_11) )
	{
		nID--;
	}

	// Then try and raise the frequency
	while( !IsDlgItemEnabled( nID ) && (nID < IDC_96) )
	{
		nID++;
	}

	CheckRadioButton( IDC_11, IDC_OTHER, nID );
}

void DlgMIDI::OnSelchangeComboLowLatDevice() 
{
	// Restrict range based on synth
	OnSelchangeDefaultSynth();

	if( m_fUsePhoneyDSound )
	{
		int nIndex = m_comboLowLatDevice.GetCurSel();
		if( nIndex != CB_ERR )
		{
			LowLatencyPortItem *pLowLatencyPortItem = static_cast<LowLatencyPortItem *>(m_comboLowLatDevice.GetItemDataPtr( nIndex ));
			if( pLowLatencyPortItem )
			{
				RestrictFrequencyRange( pLowLatencyPortItem->dwMinimumSampleFrequency, pLowLatencyPortItem->dwMaximumSampleFrequency );
			}
		}
	}
}

void DlgMIDI::EnableDlgItem( int nID, bool fEnable )
{
	CWnd *pWnd = GetDlgItem( nID );
	if( pWnd )
	{
		pWnd->EnableWindow( fEnable );
	}
}

bool DlgMIDI::IsDlgItemEnabled( int nID )
{
	CWnd *pWnd = GetDlgItem( nID );
	if( pWnd )
	{
		if( pWnd->IsWindowEnabled() )
		{
			return true;
		}
	}
	return false;
}

void DlgMIDI::OnSelchangeDefaultSynth() 
{
	RestrictFrequencyRange( 0, UINT_MAX );

	EnableDlgItem( IDC_OTHER, TRUE );
	m_editOther.EnableWindow( GetCheckedRadioButton( IDC_11, IDC_OTHER ) == IDC_OTHER );

	// Get the index of the currently selected item
	int index = m_comboDefaultSynth.GetCurSel();
	if (index != CB_ERR)
	{
		// Get the selected default port GUID
		if( m_comboDefaultSynth.GetItemDataPtr( index ) != (void *)UINT_MAX )
		{
			GUID *pGuid = (GUID *) m_comboDefaultSynth.GetItemDataPtr( index );
			if( *pGuid == CLSID_XboxSynth )
			{
				// DMusic's DSound sink only supports 11025, 22050, and 44100
				RestrictFrequencyRange( 11025, 44100 );

				if( GetCheckedRadioButton( IDC_11, IDC_OTHER ) == IDC_OTHER )
				{
					BOOL fTrans;
					DWORD dwOtherSampleRate = GetDlgItemInt( IDC_EDIT_OTHER, &fTrans, FALSE );

					if( !fTrans )
					{
						// If invalid value, check 22 kHz
						CheckRadioButton( IDC_11, IDC_OTHER, IDC_22 );
					}
					else if( dwOtherSampleRate < (11025+22050)/2 )
					{
						// If less than halfway to 22kHz, check 11kHz
						CheckRadioButton( IDC_11, IDC_OTHER, IDC_11 );
					}
					else if( dwOtherSampleRate < (22050+44100)/2 )
					{
						// If less than halfway to 44kHz, check 22kHz
						CheckRadioButton( IDC_11, IDC_OTHER, IDC_22 );
					}
					else
					{
						// Otherwise, check 44kHz
						CheckRadioButton( IDC_11, IDC_OTHER, IDC_44 );
					}
				}

				EnableDlgItem( IDC_OTHER, FALSE );
				m_editOther.EnableWindow( FALSE );
			}
		}
	}
}

void DlgMIDI::On1122444896() 
{
	m_editOther.EnableWindow( FALSE );
}

void DlgMIDI::OnOther() 
{
	m_editOther.EnableWindow( TRUE );
}

void DlgMIDI::OnEchoAdvanced() 
{
	DlgEchoAdvanced dlgEchoAdvanced;
	dlgEchoAdvanced.m_pDlgMIDI = this;
	int nRes = dlgEchoAdvanced.DoModal();
	if( nRes == IDOK )
	{
		// Update our Echo MIDI input options

		// Update the MIDI input port
		for( int i=0; i<m_comboEchoInput.GetCount(); i++ )
		{
			GUID guid = GUID_AllZeros;
			if( m_comboEchoInput.GetItemDataPtr( i ) != (void *)UINT_MAX )
			{
				GUID *pGuid = (GUID *) m_comboEchoInput.GetItemDataPtr( i );
				if( pGuid )
				{
					guid = *pGuid;
				}
			}

			if( m_aguidEchoMIDIInPort[0] == guid )
			{
				m_comboEchoInput.SetCurSel( i );
				break;
			}
		}

		// Update the output PChannels
		UpdatePChannelEdit( m_anCurrentBlock[0] );
	}
}
