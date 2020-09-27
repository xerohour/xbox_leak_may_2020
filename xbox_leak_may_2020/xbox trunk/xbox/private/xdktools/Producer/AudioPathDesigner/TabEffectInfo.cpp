// TabEffectInfo.cpp : implementation file
//

#include "stdafx.h"
#include "dsoundp.h"
#include "audiopathdesignerdll.h"
#include "TabEffectInfo.h"
#include "EffectPPGMgr.h"
#include "EffectInfo.h"
#include "RiffStrm.h"
#include "OlePropSheet.h"
#include "OlePropPage.h"
#include "AudioPathComponent.h"
#include "AudioPath.h"
#include "guiddlg.h"
#include "DlgSendProperties.h"
#include "mediaobj.h"
#define _SYS_GUID_OPERATORS_
#include "dmoreg.h"
#include "uuids.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const IID IID_IMediaObject = _uuidof(IMediaObject);

/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo property page

IMPLEMENT_DYNCREATE(CTabEffectInfo, CPropertyPage)

void EmptyComboBox( CComboBox &comboBox )
{
	while( comboBox.GetCount() > 0 )
	{
		GUID *pGuid = static_cast<GUID *>(comboBox.GetItemDataPtr(0));
		if( pGuid )
		{
			delete pGuid;
		}
		comboBox.DeleteString( 0 );
	}
}

CTabEffectInfo::CTabEffectInfo() : CPropertyPage(CTabEffectInfo::IDD)
{
	//{{AFX_DATA_INIT(CTabEffectInfo)
	//}}AFX_DATA_INIT
	m_fHaveValidData = false;
	m_fNeedToDetach = false;
	m_pIPropPageObject = NULL;
}

CTabEffectInfo::~CTabEffectInfo()
{
	// Weak reference:
	//RELEASE(m_pIPropPageObject);
}

void CTabEffectInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabEffectInfo)
	DDX_Control(pDX, IDC_COMBO_SEND, m_comboSend);
	DDX_Control(pDX, IDC_EDIT_TYPE, m_editType);
	DDX_Control(pDX, IDC_EDIT_CUSTOMNAME, m_editCustomName);
	DDX_Control(pDX, IDC_CHECK_SOFTWARE, m_checkSoftware);
	DDX_Control(pDX, IDC_BUTTON_EDIT_PROPERTIES, m_btnEditProperties);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabEffectInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CTabEffectInfo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_SOFTWARE, OnCheckSoftware)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_PROPERTIES, OnButtonEditProperties)
	ON_EN_KILLFOCUS(IDC_EDIT_CUSTOMNAME, OnKillfocusEditCustomname)
	ON_CBN_SELCHANGE(IDC_COMBO_SEND, OnSelchangeComboSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND_GUID, OnButtonSendGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo message handlers

BOOL CTabEffectInfo::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->GetActivePage( &CEffectPPGMgr::sm_nActiveTab );
		RELEASE( pIPropSheet );
	}
	
	return CPropertyPage::OnSetActive();
}

int CTabEffectInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}

void CTabEffectInfo::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	EmptyComboBox( m_comboSend );

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

BOOL CTabEffectInfo::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabEffectInfo::OnCheckSoftware() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_fHaveValidData && m_pIPropPageObject );
	if( !m_fHaveValidData
	||	(m_pIPropPageObject == NULL) )
	{
		return;
	}

	if( m_checkSoftware.GetCheck() )
	{
		m_EffectInfoForPPG.m_dwFlags |= DSFX_LOCSOFTWARE;
	}
	else
	{
		m_EffectInfoForPPG.m_dwFlags &= ~DSFX_LOCSOFTWARE;
	}

	// Update the editor with the new information
	m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_FLAGS;
	m_pIPropPageObject->SetData( &m_EffectInfoForPPG );
}

long GetLongFromStream( IStream *pStream )
{
	DWORD cbRead = 0;
	LONG lResult;
	if( pStream
	&&	SUCCEEDED( StreamSeek( pStream, 0, STREAM_SEEK_SET ) )
	&&	SUCCEEDED( pStream->Read( &lResult, sizeof(LONG), &cbRead ) )
	&&	(cbRead == sizeof(LONG)) )
	{
		return lResult;
	}
	return 0;
}

IStream *GetStreamFromLong( LONG lValue )
{
	// Try and create a stream
	IStream *pIStream;
	if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
	{
		// Try and write the long to the stream
		DWORD cbWritten;
		if( SUCCEEDED( pIStream->Write( &lValue, sizeof(LONG), &cbWritten ) )
		&&	(cbWritten == sizeof(LONG)) )
		{
			// Seek back to the start of the stream
			StreamSeek( pIStream, 0, STREAM_SEEK_SET );

			// Return the stream
			return pIStream;
		}
		pIStream->Release();
	}

	// Failed - return NULL
	return NULL;
}

void CTabEffectInfo::OnButtonEditProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_fHaveValidData && m_pIPropPageObject );
	if( !m_fHaveValidData
	||	(m_pIPropPageObject == NULL) )
	{
		return;
	}

	// Check if this is a send effect
	if( m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_SEND )
	{
		// Save initial value
		long lInitialValue = GetLongFromStream( m_EffectInfoForPPG.m_pIStream );

		// Initialize the dialog
		CDlgSendProperties dlgSendProperties;
		dlgSendProperties.m_lVolume = lInitialValue;

		// Calculate the dialog's title
		CString strSendDest, strSend;
		m_comboSend.GetWindowText( strSendDest );
		strSend.LoadString( IDS_SEND_TEXT );
		strSend += _T(": ") + strSendDest;
		dlgSendProperties.m_strTitle = strSend;

		// Display the dialog
		if( (dlgSendProperties.DoModal() == IDOK)
		&&	(dlgSendProperties.m_lVolume != lInitialValue) )
		{
			// Release the existing stream, if there is one
			if( m_EffectInfoForPPG.m_pIStream )
			{
				m_EffectInfoForPPG.m_pIStream->Release();
			}

			// Create a new steram to store the volume value in
			m_EffectInfoForPPG.m_pIStream = GetStreamFromLong( dlgSendProperties.m_lVolume );

			// Update the editor with the new information
			m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_DATA;
			m_pIPropPageObject->SetData( &m_EffectInfoForPPG );
		}
		return;
	}

	// Try and create the effect, asking for an IMediaObject interface
	IMediaObject *pIMediaObject;
	if( FAILED( ::CoCreateInstance( m_EffectInfoForPPG.m_clsidObject, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (void **)&pIMediaObject ) ) )
	{
		return;
	}

	// Set the default sample rate to 22050
	DWORD dwSampleRate = 22050;

	// Initialize the DMUS_AUDIOPARAMS structure
	DMUS_AUDIOPARAMS dmAudioParams;
	ZeroMemory( &dmAudioParams, sizeof( DMUS_AUDIOPARAMS ) );
	dmAudioParams.dwSize = sizeof( DMUS_AUDIOPARAMS );

	// Try and get the current performance audio params, and check if we got
	// the sample rate
	if( SUCCEEDED( theApp.m_pAudioPathComponent->m_pIConductor->GetAudioParams( &dmAudioParams ) )
	&&	(dmAudioParams.dwValidData & DMUS_AUDIOPARAMS_SAMPLERATE) )
	{
		// Copy the sample rate, since it should be valid
		dwSampleRate = dmAudioParams.dwSampleRate;
	}

	// TODO: Set number of channels based on the buffer this effect is in
	const DWORD dwNumChannels = 1;
	WAVEFORMATEX waveFormat = {WAVE_FORMAT_PCM, dwNumChannels, dwSampleRate, dwSampleRate * 2 * dwNumChannels, 2 * dwNumChannels, 16, 0};

	DMO_MEDIA_TYPE dmt;
    ZeroMemory(&dmt, sizeof dmt);
    dmt.majortype               = MEDIATYPE_Audio;
    dmt.subtype                 = MEDIASUBTYPE_PCM;
    dmt.bFixedSizeSamples       = TRUE;
    dmt.bTemporalCompression    = FALSE;
    dmt.lSampleSize             = 2; // 16-bit samples
    dmt.formattype              = FORMAT_WaveFormatEx;
    dmt.cbFormat                = sizeof(WAVEFORMATEX);
    dmt.pbFormat                = PBYTE(&waveFormat);

	pIMediaObject->SetInputType(0, &dmt, 0);
	pIMediaObject->SetOutputType(0, &dmt, 0);

	// Ask for an IPersistStream interface
	IPersistStream *pIPersistStream;
	if( FAILED( pIMediaObject->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
	{
		pIMediaObject->Release();
		return;
	}

	pIMediaObject->Release();
	pIMediaObject = NULL;

	// Check if the effect has any data
	if( m_EffectInfoForPPG.m_pIStream )
	{
		// Seek back to the start of the stream
		if( SUCCEEDED( StreamSeek( m_EffectInfoForPPG.m_pIStream, 0 , STREAM_SEEK_SET ) ) )
		{
			// Try and load the effect
			pIPersistStream->Load( m_EffectInfoForPPG.m_pIStream );
		}
	}

	// Save the original state of the effect
	IStream *pIOriginalStream = NULL;
	if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &pIOriginalStream ) ) )
	{
		// Try and save the effect to a stream
		if( FAILED( pIPersistStream->Save( pIOriginalStream, TRUE ) ) )
		{
			pIOriginalStream->Release();
			pIOriginalStream = NULL;
		}
	}

	CString strTitle;
	strTitle= m_EffectInfoForPPG.m_strName + _T(" : ") + m_EffectInfoForPPG.m_strInstanceName;

	IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow = NULL;
	m_pIPropPageObject->QueryInterface( IID_IDMUSProdUpdateObjectNow, (void**)&pIDMUSProdUpdateObjectNow );

	COlePropSheet *pPropSheet = new COlePropSheet( pIPersistStream, strTitle, pIDMUSProdUpdateObjectNow );
	if( pPropSheet )
	{
		int nResult = pPropSheet->DoModal();
		if( IDOK == nResult )
		{
			// Create a stream to store the effect's data in
			IStream *pIStream;
			if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
			{
				// Try and save the effect to a stream
				if( SUCCEEDED( pIPersistStream->Save( pIStream, TRUE ) ) )
				{
					// Seek back to the start of the stream
					if( SUCCEEDED( StreamSeek( pIStream, 0 , STREAM_SEEK_SET ) ) )
					{
						// Check if the data changed at all
						if( !StreamsAreEqual( pIStream, m_EffectInfoForPPG.m_pIStream ) )
						{
							// Data changed

							// Release the existing stream
							if( m_EffectInfoForPPG.m_pIStream )
							{
								m_EffectInfoForPPG.m_pIStream->Release();
								m_EffectInfoForPPG.m_pIStream = NULL;
							}

							// Clone the stream for the EffectInfo
							VERIFY( SUCCEEDED( pIStream->Clone( &m_EffectInfoForPPG.m_pIStream ) ) );

							// Update the editor with the new information
							m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_DATA;
							m_pIPropPageObject->SetData( &m_EffectInfoForPPG );
						}
					}
				}

				// Release the stream
				pIStream->Release();
			}
		}
		else
		{
			// Ensure the object is reset to its old state
			if( pIDMUSProdUpdateObjectNow )
			{
				// Check if there is an original state stream
				if( pIOriginalStream )
				{
					// Seek back to the start of the stream
					if( SUCCEEDED( StreamSeek( pIOriginalStream, 0 , STREAM_SEEK_SET ) ) )
					{
						// Try and load the object with the stream
						if( SUCCEEDED( pIPersistStream->Load( pIOriginalStream ) ) )
						{
							pIDMUSProdUpdateObjectNow->UpdateObjectNow( pIPersistStream );
						}
					}
				}
			}
		}
	}

	// Release the object's original state stream
	if( pIOriginalStream )
	{
		pIOriginalStream->Release();
	}

	// Release the object's IDMUSProdUpdateObjectNow interface
	if( pIDMUSProdUpdateObjectNow )
	{
		pIDMUSProdUpdateObjectNow->Release();
	}

	// Release the effect's IPersistStream interface
	pIPersistStream->Release();
}

void CTabEffectInfo::OnKillfocusEditCustomname() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_fHaveValidData && m_pIPropPageObject );
	if( !m_fHaveValidData
	||	(m_pIPropPageObject == NULL) )
	{
		return;
	}

	CString strNewText;
	m_editCustomName.GetWindowText( strNewText );

	// Strip leading and trailing spaces
	strNewText.TrimLeft();
	strNewText.TrimRight();

	if( strNewText.IsEmpty() )
	{
		m_editCustomName.SetWindowText( m_EffectInfoForPPG.m_strInstanceName );
		return;
	}

	m_EffectInfoForPPG.m_strInstanceName = strNewText;

	// Update the editor with the new information
	m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_NAME;
	m_pIPropPageObject->SetData( &m_EffectInfoForPPG );
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::SetEffect

void CTabEffectInfo::SetEffect( EffectInfoForPPG* pEffectInfo, IDMUSProdPropPageObject* pIPropPageObject )
{
	m_pIPropPageObject = pIPropPageObject;
	if( (pEffectInfo == NULL)
	||	(pIPropPageObject == NULL) )
	{
		m_fHaveValidData = false;
	}
	else
	{
		m_fHaveValidData = true;
		m_EffectInfoForPPG.Copy( *pEffectInfo );
	}

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::EnableControls

void CTabEffectInfo::EnableControls( bool fEnable ) 
{
	// Check if this is a send effect
	if( m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_SEND )
	{
		m_editCustomName.EnableWindow( FALSE );
	}
	else
	{
		m_editCustomName.EnableWindow( fEnable );
	}
	m_editType.EnableWindow( fEnable );

	// If this is a send effect (or an I3DL2 source effect)
	if( fEnable
	&&	((m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_SEND)
		 /*|| (m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_STANDARD_I3DL2SOURCE)*/) )
	{
		// Show the send options
		EnableDlgItem( IDC_STATIC_SEND, TRUE );
		ShowDlgItem( IDC_STATIC_SEND, SW_SHOWNA );
		ShowDlgItem( IDC_BUTTON_SEND_GUID, SW_SHOWNA );
		m_comboSend.ShowWindow( SW_SHOWNA );

		// Disable the 'Always Process in Software' checkbox
		m_checkSoftware.EnableWindow( FALSE );
	}
	else
	{
		// Hide the send options
		EnableDlgItem( IDC_STATIC_SEND, FALSE );
		EnableDlgItem( IDC_BUTTON_SEND_GUID, FALSE );
		m_comboSend.EnableWindow( FALSE );
		ShowDlgItem( IDC_STATIC_SEND, SW_HIDE );
		ShowDlgItem( IDC_BUTTON_SEND_GUID, SW_HIDE );
		m_comboSend.ShowWindow( SW_HIDE );

		// Possibly enable the 'Always Process in Software' checkbox
		m_checkSoftware.EnableWindow( fEnable );
	}

	// Check if the effect has any property pages
	if( fEnable
	&&	(m_EffectInfoForPPG.m_clsidObject != GUID_DSFX_SEND) ) // Always enable the send effect
	{
		// Initialize to false
		fEnable = false;

		// Try and create the effect, asking for an ISpecifyPropertyPages interface
		ISpecifyPropertyPages *pISpecifyPropertyPages;
		if( SUCCEEDED( ::CoCreateInstance( m_EffectInfoForPPG.m_clsidObject, NULL, CLSCTX_INPROC_SERVER, IID_ISpecifyPropertyPages, (void **)&pISpecifyPropertyPages ) ) )
		{
			CAUUID cauuid;
			if( SUCCEEDED( pISpecifyPropertyPages->GetPages( &cauuid ) ) )
			{
				// At least one property page exists, enable the button
				if( cauuid.cElems > 0 )
				{
					for( ULONG i=0; !fEnable && (i < cauuid.cElems); i++ )
					{
						IPropertyPage *pIPropertyPage;
						if( SUCCEEDED( ::CoCreateInstance( cauuid.pElems[i], NULL, CLSCTX_INPROC_SERVER, IID_IPropertyPage, (void **)&pIPropertyPage ) ) )
						{
							fEnable = true;
							pIPropertyPage->Release();
						}
					}
				}
				::CoTaskMemFree( cauuid.pElems );
			}
			pISpecifyPropertyPages->Release();
		}
	}
	m_btnEditProperties.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::UpdateControls

void CTabEffectInfo::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editCustomName.m_hWnd) == FALSE )
	{
		return;
	}

	// Update controls
	m_editCustomName.LimitText( DMUS_MAX_NAME );

	// Empty the combo box
	EmptyComboBox( m_comboSend );

	if( m_fHaveValidData && m_pIPropPageObject )
	{
		EnableControls( true );

		// Check if this is a send effect
		if( m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_SEND )
		{
			// Set name
			m_editCustomName.SetWindowText( NULL );
		}
		else
		{
			// Set name
			m_editCustomName.SetWindowText( m_EffectInfoForPPG.m_strInstanceName );
		}

		// Get the effect type
		m_editType.SetWindowText( m_EffectInfoForPPG.m_strName );

		// Set the flags
		m_checkSoftware.SetCheck( (m_EffectInfoForPPG.m_dwFlags & DSFX_LOCSOFTWARE) ? 1 : 0 );

		// If a send effect, update the send combobox and 'edit guid' button
		// If an I3DL2 source, update the send combobox and 'edit guid' button
		if( (m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_SEND)
		/*||	(m_EffectInfoForPPG.m_clsidObject == GUID_DSFX_STANDARD_I3DL2SOURCE)*/ )
		{
			// Create the variables to use when iterating through the buffers
			CString strName;
			WCHAR wcstrBufferName[DMUS_MAX_NAME + 1];

			// Flag when we find the buffer
			bool fFoundBuffer = false;

			// Flag when we find our audiopath
			bool fFoundOurAudiopath = false;

			// Iterate through all audiopaths
			POSITION pos = theApp.m_pAudioPathComponent->m_lstAudioPaths.GetHeadPosition();
			while( pos )
			{
				// Get a pointer to each audiopath
				CDirectMusicAudioPath *pAudioPath = theApp.m_pAudioPathComponent->m_lstAudioPaths.GetNext( pos );

				// Copy the name
				strName = pAudioPath->m_strName;

				// Iterate through all mixin buffers
				DWORD dwIndex = 0;
				while( true )
				{
					// Create a new guid to store the buffer's GUID
					GUID *pguidBuffer = new GUID;

					// Ensure we could allocate memory
					if( pguidBuffer == NULL )
					{
						break;
					}

					// Try and enumerate a buffer
					if( FAILED( pAudioPath->EnumBufferInfo( dwIndex, wcstrBufferName, pguidBuffer ) ) )
					{
						delete pguidBuffer;
						break;
					}

					// Skip Environmental reverb buffers and our buffer
					if( (GUID_Buffer_EnvReverb == *pguidBuffer)
					||	(m_EffectInfoForPPG.m_guidMyBuffer == *pguidBuffer) )
					{
						delete pguidBuffer;
						dwIndex++;
						continue;
					}

					// Create the name to display
					CString strDisplayName;
					if( pAudioPath != m_EffectInfoForPPG.m_pAudioPath )
					{
						strDisplayName = strName + _T(": ") + CString( wcstrBufferName );
					}
					else
					{
						fFoundOurAudiopath = true;
						strDisplayName = CString( wcstrBufferName );
					}

					// Add the name to the combobox
					int nPos = m_comboSend.AddString( strDisplayName );

					// Point the name at the GUID
					m_comboSend.SetItemDataPtr( nPos, pguidBuffer );

					// Check if this is the buffer we're looking for
					if( *pguidBuffer == m_EffectInfoForPPG.m_clsidSendBuffer )
					{
						m_comboSend.SetCurSel( nPos );
						EnableDlgItem( IDC_BUTTON_SEND_GUID, FALSE );
						fFoundBuffer = true;
					}

					// Increment the buffer index
					dwIndex++;
				}
			}

			// If we didn't find our audiopath, we must be embedded in a segment
			if( !fFoundOurAudiopath )
			{
				// Add the buffers from this audiopath to the dropdown

				// Copy the name
				strName = m_EffectInfoForPPG.m_pAudioPath->m_strName;

				// Iterate through all mixin buffers
				DWORD dwIndex = 0;
				while( true )
				{
					// Create a new guid to store the buffer's GUID
					GUID *pguidBuffer = new GUID;

					// Ensure we could allocate memory
					if( pguidBuffer == NULL )
					{
						break;
					}

					// Try and enumerate a buffer
					if( FAILED( m_EffectInfoForPPG.m_pAudioPath->EnumBufferInfo( dwIndex, wcstrBufferName, pguidBuffer ) ) )
					{
						delete pguidBuffer;
						break;
					}

					// Skip Environmental reverb buffers and our buffer
					if( (GUID_Buffer_EnvReverb == *pguidBuffer)
					||	(m_EffectInfoForPPG.m_guidMyBuffer == *pguidBuffer) )
					{
						delete pguidBuffer;
						dwIndex++;
						continue;
					}

					// Create the name to display
					CString strDisplayName;
					strDisplayName = CString( wcstrBufferName );

					// Add the name to the combobox
					int nPos = m_comboSend.AddString( strDisplayName );

					// Point the name at the GUID
					m_comboSend.SetItemDataPtr( nPos, pguidBuffer );

					// Check if this is the buffer we're looking for
					if( *pguidBuffer == m_EffectInfoForPPG.m_clsidSendBuffer )
					{
						m_comboSend.SetCurSel( nPos );
						EnableDlgItem( IDC_BUTTON_SEND_GUID, FALSE );
						fFoundBuffer = true;
					}

					// Increment the buffer index
					dwIndex++;
				}
			}

			// Add the 'Environmental Reverb' text
			if( strName.LoadString( IDS_BUFFER_ENVREVERB ) )
			{
				// Create a new guid to store the buffer's GUID
				GUID *pguidBuffer = new GUID;

				// Ensure we could allocate memory
				if( pguidBuffer != NULL )
				{
					// Copy the GUID
					memcpy( pguidBuffer, &GUID_Buffer_EnvReverb, sizeof(GUID) );

					// Add the buffer text
					int nPos = m_comboSend.AddString( strName );

					// Point it at the EnvReverb GUID
					m_comboSend.SetItemDataPtr( nPos, pguidBuffer );

					// Check if this is the buffer we're looking for
					if( GUID_Buffer_EnvReverb == m_EffectInfoForPPG.m_clsidSendBuffer )
					{
						m_comboSend.SetCurSel( nPos );
						EnableDlgItem( IDC_BUTTON_SEND_GUID, FALSE );
						fFoundBuffer = true;
					}
				}
			}

			// Add the 'Other...' text
			if( strName.LoadString( IDS_SEND_OTHER ) )
			{
				int nPos = m_comboSend.AddString( strName );

				// Point it at a NULL GUID
				m_comboSend.SetItemDataPtr( nPos, NULL );

				// If we didn't find the buffer
				if( !fFoundBuffer )
				{
					m_comboSend.SetCurSel( nPos );
					EnableDlgItem( IDC_BUTTON_SEND_GUID, TRUE );
				}
			}

			// Disable the 'Edit GUID' button if we found the buffer
			EnableDlgItem( IDC_BUTTON_SEND_GUID, fFoundBuffer ? FALSE : TRUE );

			// Enable the combobox
			m_comboSend.EnableWindow( TRUE );
		}
	}
	else
	{
		m_editCustomName.SetWindowText( _T("") );
		m_editType.SetWindowText( _T("") );
		m_checkSoftware.SetCheck( 0 );

		EnableControls( false );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::OnSelchangeComboSend

void CTabEffectInfo::OnSelchangeComboSend() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nSel = m_comboSend.GetCurSel();
	if( nSel != CB_ERR )
	{
		GUID *pGuid = static_cast<GUID *>(m_comboSend.GetItemDataPtr(nSel));
		if( pGuid )
		{
			memcpy( &m_EffectInfoForPPG.m_clsidSendBuffer, pGuid, sizeof(GUID) );

			// Update the editor with the new information
			m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_SEND;
			m_pIPropPageObject->SetData( &m_EffectInfoForPPG );

			// Disable the 'Edit GUID' button
			EnableDlgItem( IDC_BUTTON_SEND_GUID, FALSE );
		}
		else
		{
			// Save a copy of the old GUID
			const GUID oldGuid = m_EffectInfoForPPG.m_clsidSendBuffer;

			// Display the 'Edit GUID' dialog
			OnButtonSendGuid();

			// Check if the GUID changed
			if( oldGuid == m_EffectInfoForPPG.m_clsidSendBuffer )
			{
				// No change - reset the combobox
				UpdateControls();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::OnButtonSendGuid

void CTabEffectInfo::OnButtonSendGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CGuidDlg dlg;

	memcpy( &dlg.m_guid, &m_EffectInfoForPPG.m_clsidSendBuffer, sizeof(GUID) );

	if( dlg.DoModal() == IDOK )
	{
		memcpy( &m_EffectInfoForPPG.m_clsidSendBuffer, &dlg.m_guid, sizeof(GUID) );

		// Update the editor with the new information
		m_EffectInfoForPPG.m_dwChanged = CH_EFFECT_SEND;
		m_pIPropPageObject->SetData( &m_EffectInfoForPPG );

		// Update the combobox selection
		UpdateControls();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::EnableDlgItem

void CTabEffectInfo::EnableDlgItem( int nItem, BOOL fEnable )
{
	CWnd* pWnd = GetDlgItem( nItem );
	if( pWnd )
	{
		pWnd->EnableWindow( fEnable );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo::ShowDlgItem

void CTabEffectInfo::ShowDlgItem( int nItem, int nCmdShow )
{
	CWnd* pWnd = GetDlgItem( nItem );
	if( pWnd )
	{
		pWnd->ShowWindow( nCmdShow );
	}
}
