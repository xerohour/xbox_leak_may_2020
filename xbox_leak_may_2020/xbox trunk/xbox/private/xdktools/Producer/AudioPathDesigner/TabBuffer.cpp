// TabBuffer.cpp : implementation file
//

#include "stdafx.h"
#include "TabBuffer.h"
#include "BufferPPGMgr.h"
#include "dmusici.h"
#include "dmusicf.h"
#include "AudioPathDesignerDLL.h"
#include "AudioPathComponent.h"
#include "guiddlg.h"
#include "LockoutNotification.h"
#include "Dlg3DParam.h"
#include "DlgAddRemoveBuses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int BufferGUIDToIndex( GUID &rguidBuffer )
{
	if( rguidBuffer == GUID_Buffer_Reverb )
	{
		return 0;
	}
	/*
	if( rguidBuffer == GUID_Buffer_3D )
	{
		return 1;
	}
	*/
	if( rguidBuffer == GUID_Buffer_3D_Dry )
	{
		return 1;
	}
	if( rguidBuffer == GUID_Buffer_Mono )
	{
		return 2;
	}
	if( rguidBuffer == GUID_Buffer_Stereo )
	{
		return 3;
	}
	return -1;
}

static GUID BufferIndexToGUID( int nIndex )
{
	switch( nIndex )
	{
	case 0:
		return GUID_Buffer_Reverb;
		/*
	case 1:
		return GUID_Buffer_3D;
		*/
	case 1:
		return GUID_Buffer_3D_Dry;
	case 2:
		return GUID_Buffer_Mono;
	case 3:
		return GUID_Buffer_Stereo;
	default:
		return GUID_AllZeros;
	}
}

static void PanValueToString( int nValue, CString& strValue ) 
{
	CString strTemp;

	if( nValue < DSBPAN_CENTER )
	{
		strTemp.LoadString( IDS_PAN_LEFT );
		strValue.Format( "%s%d", strTemp, abs(nValue) );
	}
	else if( nValue > DSBPAN_CENTER )
	{
		strTemp.LoadString( IDS_PAN_RIGHT );
		strValue.Format( "%s%d", strTemp, nValue );
	}
	else
	{
		strValue.LoadString( IDS_PAN_MID );
	} 
}

static int StringToPanValue( LPCTSTR pszNewValue )
{
	CString strNewValue = pszNewValue;
	int nLength = strNewValue.GetLength();

	int nNewValue = INT_MIN; // Bad Value

	CString strTemp;
	strTemp.LoadString( IDS_PAN_MID );

	if( strTemp.CompareNoCase( strNewValue) == 0 )
	{
		nNewValue = DSBPAN_CENTER;
	}
	else
	{
		CString strMinus;
		strMinus.LoadString( IDS_MINUS_TEXT );

		CString strBalance = strNewValue.Left( 1 );
		CString strValue = strNewValue.Right( nLength - 1 );

		// 1st char of 'L' or negative number means LEFT
		strTemp.LoadString( IDS_PAN_LEFT);
		if( (strTemp.CompareNoCase( strBalance ) == 0)
		||  (strMinus.CompareNoCase( strBalance ) == 0) )
		{
			nNewValue = _ttoi( strValue );
			nNewValue = -nNewValue;
		}
		else
		{
			// 1st char of 'R' or positive number means RIGHT
			strTemp.LoadString( IDS_PAN_RIGHT);
			if( strTemp.CompareNoCase( strBalance ) == 0 )
			{
				nNewValue = _ttoi( strValue );
			}
			else
			{
				nNewValue = _ttoi( strNewValue );
			}
		}
	}

	return nNewValue;
}

/////////////////////////////////////////////////////////////////////////////
// CTabBuffer property page

IMPLEMENT_DYNCREATE(CTabBuffer, CPropertyPage)

CTabBuffer::CTabBuffer() : CPropertyPage(CTabBuffer::IDD)
{
	//{{AFX_DATA_INIT(CTabBuffer)
	//}}AFX_DATA_INIT
	m_fNeedToDetach = false;
	m_pIPropPageObject = NULL;
	m_pBufferPPGMgr = NULL;
	m_fWasShared = false;
}

CTabBuffer::~CTabBuffer()
{
	// Weak reference - don't release it!
	//RELEASE(m_pIPropPageObject)
	// Weak reference - don't release it!
	//RELEASE(m_pBufferPPGMgr)
}

void CTabBuffer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabBuffer)
	DDX_Control(pDX, IDC_SPIN_VOLUME, m_spinVolume);
	DDX_Control(pDX, IDC_SPIN_PAN, m_spinPan);
	DDX_Control(pDX, IDC_EDIT_VOLUME, m_editVolume);
	DDX_Control(pDX, IDC_EDIT_PAN, m_editPan);
	DDX_Control(pDX, IDC_COMBO_STANDARD_BUFFER, m_comboStandardBuffer);
	DDX_Control(pDX, IDC_CHECK_SHARE, m_checkShared);
	DDX_Control(pDX, IDC_CHECK_PREDEFINED, m_checkPredefined);
	DDX_Control(pDX, IDC_BUTTON_GUID, m_btnEditGuid);
	DDX_Control(pDX, IDC_EDIT_CHANNELS, m_editChannels);
	DDX_Control(pDX, IDC_SPIN_CHANNELS, m_spinChannels);
	DDX_Control(pDX, IDC_COMBO_3DAlg, m_combo3DAlg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabBuffer, CPropertyPage)
	//{{AFX_MSG_MAP(CTabBuffer)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_PREDEFINED, OnCheckPredefined)
	ON_CBN_SELCHANGE(IDC_COMBO_STANDARD_BUFFER, OnSelchangeComboStandardBuffer)
	ON_BN_CLICKED(IDC_CHECK_SHARE, OnCheckShare)
	ON_BN_CLICKED(IDC_BUTTON_GUID, OnButtonGuid)
	ON_CBN_SELCHANGE(IDC_COMBO_3DAlg, OnSelchangeCOMBO3DAlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CHANNELS, OnDeltaposSpinChannels)
	ON_EN_KILLFOCUS(IDC_EDIT_CHANNELS, OnKillfocusEditChannels)
	ON_BN_CLICKED(IDC_BUTTON_3D_POS, OnButton3dPos)
	ON_EN_KILLFOCUS(IDC_EDIT_PAN, OnKillfocusEditPan)
	ON_EN_KILLFOCUS(IDC_EDIT_VOLUME, OnKillfocusEditVolume)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PAN, OnDeltaposSpinPan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabBuffer message handlers

BOOL CTabBuffer::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->GetActivePage( &CBufferPPGMgr::sm_nActiveTab );
		RELEASE( pIPropSheet );
	}

	return CPropertyPage::OnSetActive();
}

int CTabBuffer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = true;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CTabBuffer::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;

		m_fNeedToDetach = false;
	}

	CPropertyPage::OnDestroy();
}

BOOL CTabBuffer::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();

	m_combo3DAlg.ResetContent();

	// DS3DALG_DEFAULT -- Default
	// DS3DALG_NO_VIRTUALIZATION -- No virtualization
	// DS3DALG_HRTF_FULL -- High quality HRTF
	// DS3DALG_HRTF_LIGHT -- Fast HRTF
	CString strText;
	int nIndex;

	strText.LoadString( IDS_3DALG_DEFAULT );
	nIndex = m_combo3DAlg.AddString( strText );
	m_combo3DAlg.SetItemDataPtr( nIndex, (void *)&DS3DALG_DEFAULT );

	strText.LoadString( IDS_3DALG_NO_VIRTUALIZATION );
	nIndex = m_combo3DAlg.AddString( strText );
	m_combo3DAlg.SetItemDataPtr( nIndex, (void *)&DS3DALG_NO_VIRTUALIZATION );

	strText.LoadString( IDS_3DALG_HRTF_FULL );
	nIndex = m_combo3DAlg.AddString( strText );
	m_combo3DAlg.SetItemDataPtr( nIndex, (void *)&DS3DALG_HRTF_FULL );

	strText.LoadString( IDS_3DALG_HRTF_LIGHT );
	nIndex = m_combo3DAlg.AddString( strText );
	m_combo3DAlg.SetItemDataPtr( nIndex, (void *)&DS3DALG_HRTF_LIGHT );

	m_spinChannels.SetRange( 1, MAX_AUDIO_CHANNELS );
	strText.Format( "%d", MAX_AUDIO_CHANNELS );
	m_editChannels.SetLimitText( strText.GetLength() );

	m_spinVolume.SetRange( DSBVOLUME_MIN, 0 );
	strText.Format( "%d", DSBVOLUME_MIN );
	m_editVolume.SetLimitText( strText.GetLength() );
	m_editVolume.m_fMinusValid = true;
	m_editVolume.m_fDotValid = false;

	m_spinPan.SetRange( DSBPAN_LEFT, DSBPAN_RIGHT );
	PanValueToString( DSBPAN_LEFT, strText );
	m_editPan.SetLimitText( strText.GetLength() );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabBuffer::SetBuffer

void CTabBuffer::SetBuffer( BufferInfoForPPG* pBufferInfoForPPG, IDMUSProdPropPageObject* pINewPropPageObject )
{
	if( pBufferInfoForPPG )
	{
		m_BufferInfoForPPG.Copy( *pBufferInfoForPPG );
	}
	else
	{
		m_BufferInfoForPPG.m_fValid = false;
	}

	m_pIPropPageObject = pINewPropPageObject;

	m_fWasShared = false;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabBuffer::EnableUserControls

void CTabBuffer::EnableUserControls( bool fEnable ) 
{
	if( fEnable
	&&	(m_BufferInfoForPPG.m_dwMinNumChannels < MAX_AUDIO_CHANNELS) )
	{
		m_editChannels.EnableWindow( TRUE );
		m_spinChannels.EnableWindow( TRUE );
	}
	else
	{
		m_editChannels.EnableWindow( FALSE );
		m_spinChannels.EnableWindow( FALSE );
	}

	EnableDlgItem( IDC_RADIO_CTRL3D, fEnable );
	EnableDlgItem( IDC_RADIO_PAN, fEnable );
	EnableDlgItem( IDC_CHECK_DOPPLER, fEnable );

	m_editVolume.EnableWindow( fEnable );

	/*
	EnableDlgItem( IDC_RADIO_GLOBALFOCUS, fEnable );
	EnableDlgItem( IDC_RADIO_STICKYFOCUS, fEnable );
	EnableDlgItem( IDC_RADIO_LOCALFOCUS, fEnable );
	*/

	if( fEnable )
	{
		if( m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_CTRL3D )
		{
			EnableDlgItem( IDC_CHECK_MUTE3DATMAXDISTANCE, TRUE );
			EnableDlgItem( IDC_BUTTON_3D_POS, TRUE );
			m_combo3DAlg.EnableWindow( TRUE );
			m_editPan.EnableWindow( FALSE );
		}
		else
		{
			EnableDlgItem( IDC_CHECK_MUTE3DATMAXDISTANCE, FALSE );
			EnableDlgItem( IDC_BUTTON_3D_POS, FALSE );
			m_combo3DAlg.EnableWindow( FALSE );
			m_editPan.EnableWindow( TRUE );
		}

	}
	else
	{
		EnableDlgItem( IDC_CHECK_MUTE3DATMAXDISTANCE, FALSE );
		EnableDlgItem( IDC_BUTTON_3D_POS, FALSE );
		m_combo3DAlg.EnableWindow( FALSE );
		m_editPan.EnableWindow( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBuffer::EnableDlgItem

void CTabBuffer::EnableDlgItem( int nItem, BOOL fEnable )
{
	CWnd* pWnd = GetDlgItem( nItem );
	if( pWnd )
	{
		pWnd->EnableWindow( fEnable );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBuffer::EnableControls

void CTabBuffer::EnableControls( bool fEnable ) 
{
	EnableUserControls( fEnable
		&& !(m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED) );

	m_checkShared.EnableWindow( fEnable );

	// Check if fEnable is set and we're not using the EnvReverb buffer type
	// and the mix group has PChannels
#ifndef DMP_XBOX
	if( fEnable
	&&	(m_BufferInfoForPPG.m_guidBufferID != GUID_Buffer_EnvReverb)
	&&	(m_BufferInfoForPPG.m_dwDesignFlags & DESIGN_BUFFER_HAS_PCHANNELS) )
	{
		// Allow the user to modify the "Use standard buffer" checkbox
		m_checkPredefined.EnableWindow( TRUE );
	}
	else
#endif
	{
		m_checkPredefined.EnableWindow( FALSE );
	}

	// Check if fEnable is set and we're using a defined buffer type,
	// but not using the EnvReverb buffer type
	if( fEnable
	&&	(m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED)
	&&	(m_BufferInfoForPPG.m_guidBufferID != GUID_Buffer_EnvReverb) )
	{
		// Allow the user to modify the "Predefined buffer" combobox
		m_comboStandardBuffer.EnableWindow( TRUE );
	}
	else
	{
		m_comboStandardBuffer.EnableWindow( FALSE );
	}

	// Check if fEnable is set and we're using a shared, non-standard buffer
	if( fEnable
	&&	!(m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
	{
		// Allow the user to modify the buffer's GUID
		m_btnEditGuid.EnableWindow( TRUE );
	}
	else
	{
		m_btnEditGuid.EnableWindow( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBuffer::UpdateControls

void CTabBuffer::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_comboStandardBuffer.m_hWnd) == FALSE )
	{
		return;
	}

	// Don't send notifications when updating the dialog
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Update controls
	if( m_BufferInfoForPPG.m_fValid && m_pIPropPageObject )
	{
		EnableControls( true );

		// Set the states of the checkboxes
		m_checkPredefined.SetCheck( ((m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED) != 0) ? 1 : 0 );
		m_checkShared.SetCheck( ((m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_SHARED) != 0) ? 1 : 0 );

		if( m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_DEFINED )
		{
			CString strText;
			m_comboStandardBuffer.ResetContent();

			// If using the environmental reverb type
			if( m_BufferInfoForPPG.m_guidBufferID == GUID_Buffer_EnvReverb )
			{
				// Set the combobox to say 'Environmental Reverb'
				strText.LoadString(IDS_PPG_BUFFER_ENV_REVERB);
				m_comboStandardBuffer.AddString( strText );
				m_comboStandardBuffer.SetCurSel( 0 );

				CheckDlgButton( IDC_RADIO_CTRL3D, 0 );
				CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 0 );
				CheckDlgButton( IDC_RADIO_PAN, 1 );
				CheckDlgButton( IDC_CHECK_DOPPLER, 0 );
				SetDlgItemInt( IDC_EDIT_CHANNELS, 2, FALSE );
			}
			else
			{
				// Otherwise, set the defined buffer type
				strText.LoadString(IDS_PPG_BUFFER_REVERB);
				m_comboStandardBuffer.AddString( strText );
				/*
				strText.LoadString(IDS_PPG_BUFFER_3D);
				m_comboStandardBuffer.AddString( strText );
				*/
				strText.LoadString(IDS_PPG_BUFFER_3D_DRY);
				m_comboStandardBuffer.AddString( strText );
				strText.LoadString(IDS_PPG_BUFFER_MONO);
				m_comboStandardBuffer.AddString( strText );
				strText.LoadString(IDS_PPG_BUFFER_STEREO);
				m_comboStandardBuffer.AddString( strText );

				m_comboStandardBuffer.SetCurSel( BufferGUIDToIndex( m_BufferInfoForPPG.m_guidBufferID ) );

				// Set the checkboxes
				int nIndex = BufferGUIDToIndex( m_BufferInfoForPPG.m_guidBufferID );
				switch( nIndex )
				{
				case 0: // Reverb
				case 3: // Stereo
					CheckDlgButton( IDC_RADIO_CTRL3D, 0 );
					CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 0 );
					CheckDlgButton( IDC_RADIO_PAN, 1 );
					SetDlgItemInt( IDC_EDIT_CHANNELS, 2, FALSE );
					break;
				//case 1: // 3D Wet
				case 1: // 3D Dry
					CheckDlgButton( IDC_RADIO_CTRL3D, 1 );
					CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 1 );
					CheckDlgButton( IDC_RADIO_PAN, 0 );
					SetDlgItemInt( IDC_EDIT_CHANNELS, 1, FALSE );
					break;
				default: // Everything else (Mono)
					CheckDlgButton( IDC_RADIO_CTRL3D, 0 );
					CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 0 );
					CheckDlgButton( IDC_RADIO_PAN, 1 );
					SetDlgItemInt( IDC_EDIT_CHANNELS, 1, FALSE );
					break;
				}

				// Set the doppler checkbox if anything except a Reverb buffer
				CheckDlgButton( IDC_CHECK_DOPPLER, (nIndex == 0) ? 0 : 1 );
			}

			// Set the 3d type to 'default'
			for( int nIndex = 0; nIndex < m_combo3DAlg.GetCount(); nIndex++ )
			{
				CLSID *pclsidItem = static_cast<CLSID *>(m_combo3DAlg.GetItemDataPtr( nIndex ));
				if( pclsidItem
				&&	(*pclsidItem == DS3DALG_DEFAULT) )
				{
					m_combo3DAlg.SetCurSel( nIndex );
					break;
				}
			}

			// Set the focus radio buttons
			//CheckRadioButton( IDC_RADIO_LOCALFOCUS, IDC_RADIO_GLOBALFOCUS, IDC_RADIO_GLOBALFOCUS );

			// Set the Pan and Volume
			SetDlgItemInt( IDC_EDIT_PAN, 0, TRUE );
			SetDlgItemInt( IDC_EDIT_VOLUME, 0, TRUE );
		}
		else
		{
			m_comboStandardBuffer.SetCurSel( -1 );

			// Get the 3d type
			for( int nIndex = 0; nIndex < m_combo3DAlg.GetCount(); nIndex++ )
			{
				CLSID *pclsidItem = static_cast<CLSID *>(m_combo3DAlg.GetItemDataPtr( nIndex ));
				if( pclsidItem
				&&	(*pclsidItem == m_BufferInfoForPPG.m_clsid3dAlgorithm) )
				{
					m_combo3DAlg.SetCurSel( nIndex );
					break;
				}
			}

			// Set the number of channels
			SetDlgItemInt( IDC_EDIT_CHANNELS, m_BufferInfoForPPG.m_dwNumChannels, FALSE );
			m_spinChannels.SetPos( m_BufferInfoForPPG.m_dwNumChannels );
			m_spinChannels.SetRange( m_BufferInfoForPPG.m_dwMinNumChannels, MAX_AUDIO_CHANNELS );

			// Set the focus radio buttons
			/*
			if( m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_STICKYFOCUS )
			{
				CheckRadioButton( IDC_RADIO_LOCALFOCUS, IDC_RADIO_GLOBALFOCUS, IDC_RADIO_STICKYFOCUS );
			}
			else if( m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_GLOBALFOCUS )
			{
				CheckRadioButton( IDC_RADIO_LOCALFOCUS, IDC_RADIO_GLOBALFOCUS, IDC_RADIO_GLOBALFOCUS );
			}
			else // None
			{
				CheckRadioButton( IDC_RADIO_LOCALFOCUS, IDC_RADIO_GLOBALFOCUS, IDC_RADIO_LOCALFOCUS );
			}
			*/

			// Set the checkboxes
			CheckDlgButton( IDC_RADIO_CTRL3D, (m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_CTRL3D) ? 1 : 0 );
			CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, (m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_MUTE3DATMAXDISTANCE) ? 1 : 0 );
			CheckDlgButton( IDC_RADIO_PAN, (m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_CTRLPAN) ? 1 : 0 );
			CheckDlgButton( IDC_CHECK_DOPPLER, (m_BufferInfoForPPG.m_dwBufferFlags & DSBCAPS_CTRLFREQUENCY) ? 1 : 0 );

			// Set the Volume value
			SetDlgItemInt( IDC_EDIT_VOLUME, m_BufferInfoForPPG.m_lVolume, TRUE );

			// Set the Pan value
			CString strPan;
			PanValueToString( m_BufferInfoForPPG.m_lPan, strPan );
			SetDlgItemText( IDC_EDIT_PAN, strPan );
		}
	}
	else
	{
		EnableControls( false );

		m_checkPredefined.SetCheck( 0 );
		m_checkShared.SetCheck( 0 );
		m_comboStandardBuffer.SetCurSel( -1 );

		m_combo3DAlg.SetCurSel( -1 );
		SetDlgItemText( IDC_EDIT_CHANNELS, NULL );

		// Set the focus radio buttons
		//CheckRadioButton( IDC_RADIO_LOCALFOCUS, IDC_RADIO_GLOBALFOCUS, IDC_RADIO_LOCALFOCUS );

		// Set the checkboxes
		CheckDlgButton( IDC_RADIO_CTRL3D, 0 );
		CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 0 );
		CheckDlgButton( IDC_RADIO_PAN, 0 );

		// Set the Pan and Volume
		SetDlgItemText( IDC_EDIT_PAN, NULL );
		SetDlgItemText( IDC_EDIT_VOLUME, NULL );
	}
}

void CTabBuffer::OnCheckPredefined() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_BufferInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	if( m_checkPredefined.GetCheck() )
	{
		m_fWasShared = (m_BufferInfoForPPG.m_dwHeaderFlags & DMUS_BUFFERF_SHARED) ? true : false;

		m_BufferInfoForPPG.m_dwHeaderFlags |= DMUS_BUFFERF_DEFINED | DMUS_BUFFERF_SHARED;

		// The Buffer GUID should not be one of the predefined ones
		ASSERT( BufferGUIDToIndex( m_BufferInfoForPPG.m_guidBufferID ) == -1);

		// Flag that we also changed the standard GUID
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_HEADERFLAGS | CH_BUFFER_STANDARDGUID;
		m_BufferInfoForPPG.m_guidBufferID = GUID_Buffer_Reverb;
	}
	else
	{
		m_BufferInfoForPPG.m_dwHeaderFlags &= ~DMUS_BUFFERF_DEFINED;
		if( m_fWasShared )
		{
			m_BufferInfoForPPG.m_dwHeaderFlags |= DMUS_BUFFERF_SHARED;
		}
		else
		{
			m_BufferInfoForPPG.m_dwHeaderFlags &= ~DMUS_BUFFERF_SHARED;
		}

		// Flag that we also changed the standard GUID
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_HEADERFLAGS | CH_BUFFER_STANDARDGUID;
		CoCreateGuid( &m_BufferInfoForPPG.m_guidBufferID );
	}

	// Update the editor with the new information
	m_pIPropPageObject->SetData( &m_BufferInfoForPPG );

	// Enable/disable the edit guid button and standard buffer droplist
	// Update the droplist to point to the correct buffer
	UpdateControls();
}

void CTabBuffer::OnSelchangeComboStandardBuffer() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_BufferInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	// Get the new index
	int nIndex = m_comboStandardBuffer.GetCurSel();
	if( nIndex != CB_ERR )
	{
		// Get a copy of the new GUID
		m_BufferInfoForPPG.m_guidBufferID = BufferIndexToGUID( nIndex );

		if( /*(GUID_Buffer_3D == m_BufferInfoForPPG.m_guidBufferID)
		||	*/(GUID_Buffer_3D_Dry == m_BufferInfoForPPG.m_guidBufferID) )
		{
			m_BufferInfoForPPG.m_dwHeaderFlags &= ~DMUS_BUFFERF_SHARED;
		}
		else // GUID_Buffer_Reverb, GUID_Buffer_Stereo, GUID_Buffer_Mono
		{
			m_BufferInfoForPPG.m_dwHeaderFlags |= DMUS_BUFFERF_SHARED;
		}

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_STANDARDGUID;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );

		// Refresh the controls
		UpdateControls();
	}
}

void CTabBuffer::OnCheckShare() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !m_BufferInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	if( m_checkShared.GetCheck() )
	{
		m_BufferInfoForPPG.m_dwHeaderFlags |= DMUS_BUFFERF_SHARED;
	}
	else
	{
		m_BufferInfoForPPG.m_dwHeaderFlags &= ~DMUS_BUFFERF_SHARED;
	}

	// Update the editor with the new information
	m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_HEADERFLAGS;
	m_pIPropPageObject->SetData( &m_BufferInfoForPPG );

	// Enable/disable the edit guid button
	EnableControls( true );
}

void CTabBuffer::OnButtonGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CGuidDlg dlgGuid;
	dlgGuid.m_guid = m_BufferInfoForPPG.m_guidBufferID;
	if( dlgGuid.DoModal() == IDOK )
	{
		// Get the new GUID
		m_BufferInfoForPPG.m_guidBufferID = dlgGuid.m_guid;

		// Update the editor with the new information
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_USERGUID;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}
}

void CTabBuffer::OnSelchangeCOMBO3DAlg() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_BufferInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return;
	}

	// Get the new index
	int nIndex = m_combo3DAlg.GetCurSel();
	if( nIndex != CB_ERR )
	{
		// Get a copy of the new GUID
		GUID *pGuid = (GUID *)m_combo3DAlg.GetItemDataPtr( nIndex );
		if( pGuid )
		{
			m_BufferInfoForPPG.m_clsid3dAlgorithm = *pGuid;

			// Nofiy the PPO that the data changed
			m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFER3DALG;
			m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
		}
	}
}

BOOL CTabBuffer::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_BufferInfoForPPG.m_fValid || !m_pIPropPageObject )
	{
		return CPropertyPage::OnCommand( wParam, lParam );
	}

	int nControlID = LOWORD( wParam );
	int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		// Get a pointer to the button that was pressed
		CWnd* pWnd = GetDlgItem( nControlID );
		if( pWnd )
		{
			bool fButtonChecked = (IsDlgButtonChecked( nControlID ) != 0);

			// Handle buttons with side effects:
			switch( nControlID )
			{
				/*
			case IDC_RADIO_LOCALFOCUS:
				// Clear sticky and global bits
				m_BufferInfoForPPG.m_dwBufferFlags &= ~(DSBCAPS_STICKYFOCUS | DSBCAPS_GLOBALFOCUS);

				// Nofiy the PPO that the data changed
				m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
				m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				break;
			case IDC_RADIO_STICKYFOCUS:
				// Set sticky bit
				m_BufferInfoForPPG.m_dwBufferFlags |= DSBCAPS_STICKYFOCUS;

				// Clear global bit
				m_BufferInfoForPPG.m_dwBufferFlags &= ~DSBCAPS_GLOBALFOCUS;

				// Nofiy the PPO that the data changed
				m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
				m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				break;
			case IDC_RADIO_GLOBALFOCUS:
				// Set global bit
				m_BufferInfoForPPG.m_dwBufferFlags |= DSBCAPS_GLOBALFOCUS;

				// Clear sticky bit
				m_BufferInfoForPPG.m_dwBufferFlags &= ~DSBCAPS_STICKYFOCUS;

				// Nofiy the PPO that the data changed
				m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
				m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				break;
				*/
			case IDC_RADIO_PAN:
				// Disable and uncheck Mute Sound button
				EnableDlgItem( IDC_CHECK_MUTE3DATMAXDISTANCE, FALSE );
				CheckDlgButton( IDC_CHECK_MUTE3DATMAXDISTANCE, 0 );
				m_BufferInfoForPPG.m_dwBufferFlags &= ~DSBCAPS_MUTE3DATMAXDISTANCE;

				// Disable the 3D position controls
				EnableDlgItem( IDC_BUTTON_3D_POS, FALSE );

				// Disable the 3D algorithm combobox
				m_combo3DAlg.EnableWindow( FALSE );

				// Clear bit
				m_BufferInfoForPPG.m_dwBufferFlags &= ~DSBCAPS_CTRL3D;

				// Enable the Pan edit box
				m_editPan.EnableWindow( TRUE );

				// Set the CtrlPan bit
				m_BufferInfoForPPG.m_dwBufferFlags |= DSBCAPS_CTRLPAN;

				// Nofiy the PPO that the data changed
				m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
				m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				break;
			case IDC_RADIO_CTRL3D:
				// Enable the Mute sound button
				EnableDlgItem( IDC_CHECK_MUTE3DATMAXDISTANCE, TRUE );

				// Enable the 3D position controls
				EnableDlgItem( IDC_BUTTON_3D_POS, TRUE );

				// Enable the 3D algorithm combobox
				m_combo3DAlg.EnableWindow( TRUE );

				// Set Ctrl3D bit
				m_BufferInfoForPPG.m_dwBufferFlags |= DSBCAPS_CTRL3D;

				// Disable the Pan edit box
				m_editPan.EnableWindow( FALSE );

				// Clear the CtrlPan bit
				m_BufferInfoForPPG.m_dwBufferFlags &= ~DSBCAPS_CTRLPAN;

				// Nofiy the PPO that the data changed
				m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
				m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				break;
			default:
			{	// Handle other buttons
				DWORD dwBufferFlag = 0;
				switch( nControlID )
				{
				case IDC_CHECK_MUTE3DATMAXDISTANCE:
					dwBufferFlag = DSBCAPS_MUTE3DATMAXDISTANCE;
					break;
				case IDC_CHECK_DOPPLER:
					dwBufferFlag = DSBCAPS_CTRLFREQUENCY;
				}

				if( dwBufferFlag )
				{
					if( fButtonChecked )
					{
						// Button was checked
						m_BufferInfoForPPG.m_dwBufferFlags |= dwBufferFlag;
					}
					else
					{
						// Button was unchecked
						m_BufferInfoForPPG.m_dwBufferFlags &= ~dwBufferFlag;
					}

					// Nofiy the PPO that the data changed
					m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERFLAGS;
					m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
				}
				break;
			}
			}
		}
	}

	return CPropertyPage::OnCommand( wParam, lParam );
}

void CTabBuffer::OnButton3dPos() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDlg3DParam dlg3DParam;
	dlg3DParam.m_ds3DBuffer = m_BufferInfoForPPG.m_ds3DBuffer;
	if( dlg3DParam.DoModal() == IDOK )
	{
		if( memcmp( &m_BufferInfoForPPG.m_ds3DBuffer, &dlg3DParam.m_ds3DBuffer, sizeof(DS3DBUFFER) ) != 0 )
		{
			m_BufferInfoForPPG.m_ds3DBuffer = dlg3DParam.m_ds3DBuffer;

			// Nofiy the PPO that the data changed
			m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFER3DPOS;
			m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
		}
	}
}

void CTabBuffer::OnDeltaposSpinChannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewValue = m_spinChannels.GetPos() + pNMUpDown->iDelta;
	if( unsigned(nNewValue) < m_BufferInfoForPPG.m_dwMinNumChannels )
	{
		nNewValue = m_BufferInfoForPPG.m_dwMinNumChannels;
	}
	else if( nNewValue > MAX_AUDIO_CHANNELS )
	{
		nNewValue = MAX_AUDIO_CHANNELS;
	}
	
	if( (unsigned) nNewValue != m_BufferInfoForPPG.m_dwNumChannels )
	{
		m_BufferInfoForPPG.m_dwNumChannels = nNewValue;

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERCHANNELS;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}

	*pResult = 0;
}

void CTabBuffer::OnKillfocusEditChannels() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fSucceeded;
	int nNumChannels = GetDlgItemInt( IDC_EDIT_CHANNELS, &fSucceeded, FALSE );
	if( !fSucceeded
	||	unsigned(nNumChannels) < m_BufferInfoForPPG.m_dwMinNumChannels )
	{
		// Set a minimum of m_BufferInfoForPPG.m_dwMinNumChannels
		nNumChannels = m_BufferInfoForPPG.m_dwMinNumChannels;

		// Reset the number of channels
		SetDlgItemInt( IDC_EDIT_CHANNELS, nNumChannels, FALSE );
		m_spinChannels.SetPos( nNumChannels );
	}
	else if( nNumChannels > MAX_AUDIO_CHANNELS )
	{
		// Set a maximum of MAX_AUDIO_CHANNELS
		nNumChannels = MAX_AUDIO_CHANNELS;

		// Reset the number of channels
		SetDlgItemInt( IDC_EDIT_CHANNELS, nNumChannels, FALSE );
		m_spinChannels.SetPos( nNumChannels );
	}

	if( (unsigned) nNumChannels != m_BufferInfoForPPG.m_dwNumChannels )
	{
		m_BufferInfoForPPG.m_dwNumChannels = nNumChannels;

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERCHANNELS;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}
}

void CTabBuffer::OnKillfocusEditVolume() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fSucceeded;
	int nVolume = GetDlgItemInt( IDC_EDIT_VOLUME, &fSucceeded, TRUE );
	if( !fSucceeded
	||	nVolume > DSBVOLUME_MAX )
	{
		// Set volume to default (0)
		nVolume = DSBVOLUME_MAX;

		// Reset the volume value
		SetDlgItemInt( IDC_EDIT_VOLUME, nVolume, TRUE );
	}
	else if( nVolume < DSBVOLUME_MIN )
	{
		// Set volume to minimum
		nVolume = DSBVOLUME_MIN;

		// Reset the volume value
		SetDlgItemInt( IDC_EDIT_VOLUME, nVolume, TRUE );
	}

	if( nVolume != m_BufferInfoForPPG.m_lVolume )
	{
		m_BufferInfoForPPG.m_lVolume = nVolume;

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERVOLUME;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}
}

void CTabBuffer::OnKillfocusEditPan() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get text from edit control
	CString strNewPan;
	m_editPan.GetWindowText( strNewPan );
	strNewPan.TrimRight();
	strNewPan.TrimLeft();

	int nNewPan;

	if( strNewPan.IsEmpty() )
	{
		// Set pan to default
		nNewPan = DSBPAN_CENTER;

		// Reset the pan value
		SetDlgItemInt( IDC_EDIT_PAN, nNewPan, TRUE );
	}
	else
	{
		nNewPan = StringToPanValue( strNewPan );
		if( nNewPan == INT_MIN )
		{
			// Bad value - no change
			nNewPan = m_BufferInfoForPPG.m_lPan;
		}
		else
		{
			// Make sure value is within range
			if( nNewPan < DSBPAN_LEFT )
			{
				nNewPan = DSBPAN_LEFT;
			}
			if( nNewPan > DSBPAN_RIGHT )
			{
				nNewPan = DSBPAN_RIGHT;
			}
		}
	}

	if( nNewPan != m_BufferInfoForPPG.m_lPan )
	{
		m_BufferInfoForPPG.m_lPan = nNewPan;

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERPAN;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}

	// Check if edit control's text needs to be updated
	CString strValue;
	CString strCurValue;
	PanValueToString( nNewPan, strValue );
	m_editPan.GetWindowText( strCurValue );
	if( strCurValue != strValue )
	{
		m_editPan.SetWindowText( strValue );
		// SetSel's needed to get caret to end of string
		m_editPan.SetSel( 0, -1 );
		m_editPan.SetSel( -1, -1 );
	}
}

void CTabBuffer::OnDeltaposSpinPan(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just in case user clicked on spin control immediately after typing text
	OnKillfocusEditPan();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewPan = m_BufferInfoForPPG.m_lPan + pNMUpDown->iDelta;

	// Make sure value is within range
	if( nNewPan < DSBPAN_LEFT )
	{
		nNewPan = DSBPAN_LEFT;
	}
	if( nNewPan > DSBPAN_RIGHT )
	{
		nNewPan = DSBPAN_RIGHT;
	}

	if( nNewPan != m_BufferInfoForPPG.m_lPan )
	{
		m_BufferInfoForPPG.m_lPan = nNewPan;

		// Nofiy the PPO that the data changed
		m_BufferInfoForPPG.m_dwChanged = CH_BUFFER_BUFFERPAN;
		m_pIPropPageObject->SetData( &m_BufferInfoForPPG );
	}

	CString strValue;

	PanValueToString( nNewPan, strValue );
	m_editPan.SetWindowText( strValue );
	
	*pResult = 1;
}
