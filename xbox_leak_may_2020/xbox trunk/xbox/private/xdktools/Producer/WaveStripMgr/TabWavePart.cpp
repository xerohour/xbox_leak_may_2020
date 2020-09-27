// TabWavePart.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TabWavePart.h"
#include "GroupBitsPPG.h"
#include "Timeline.h"
#include <dmusicf.h>
#include <dmusici.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define VARMODE_MASK	0x0000000F

/////////////////////////////////////////////////////////////////////////////
// CTabWavePart property page

CTabWavePart::CTabWavePart() :
	CPropertyPage(CTabWavePart::IDD),
	CSliderCollection(1),
	m_pmsAttenuation(NULL)
{
	//{{AFX_DATA_INIT(CTabWavePart)
	//}}AFX_DATA_INIT

	m_pPPO = NULL;
	m_pPropPageMgr = NULL;
	m_pIPropSheet = NULL;

	m_PPGPartParams.dwPageIndex = 3;
	m_PPGPartParams.dwPChannel = 0;
	m_PPGPartParams.lVolume = 0;
	m_PPGPartParams.dwLockToPart = 0;
	m_PPGPartParams.dwPartFlagsDM = 0;

	CSliderCollection::Init(this);
	m_lAttenuation = 0;

	m_fNeedToDetach = FALSE;
}

CTabWavePart::~CTabWavePart()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
}

void CTabWavePart::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabWavePart)
	DDX_Control(pDX, IDC_COMBO_VAR_MODE, m_comboVarMode);
	DDX_Control(pDX, IDC_COMBO_VAR_LOCK, m_comboVarLockID);
	DDX_Control(pDX, IDC_EDIT_PCHANNELNAME, m_editPChannelName);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL, m_editPChannel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabWavePart, CPropertyPage)
	//{{AFX_MSG_MAP(CTabWavePart)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_PCHANNEL, OnChangeEditPchannel)
	ON_EN_KILLFOCUS(IDC_EDIT_PCHANNEL, OnKillfocusEditPchannel)
	ON_EN_CHANGE(IDC_EDIT_PCHANNELNAME, OnChangeEditPChannelName)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_VAR_LOCK, OnSelChangeComboVarLock)
	ON_CBN_SELCHANGE(IDC_COMBO_VAR_MODE, OnSelChangeComboVarMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CTabWavePart::SetObject( IDMUSProdPropPageObject* pPPO )
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void CTabWavePart::EnableControls( BOOL fEnable )
{
	// Make sure controls have been created
	if (m_pmsAttenuation == NULL)
		return;

	m_editPChannel.EnableWindow( fEnable );
	m_spinPChannel.EnableWindow( fEnable );
	m_editName.EnableWindow( fEnable );
	m_editPChannelName.EnableWindow( fEnable );
	m_pmsAttenuation->EnableControl(this, fEnable != 0);
	m_comboVarMode.EnableWindow( fEnable );
	m_comboVarLockID.EnableWindow( fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CTabWavePart message handlers

int CTabWavePart::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
		m_fNeedToDetach = TRUE;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CTabWavePart::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// free the sliders
	CSliderCollection::Free();
	m_pmsAttenuation = NULL;

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
	}

	CPropertyPage::OnDestroy();
}

BOOL CTabWavePart::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// Fill variation play mode combo box
	CString strVarMode;
	m_comboVarMode.ResetContent();
	for( int nID = IDS_VARMODE_RANDOM ;  nID <= IDS_VARMODE_RANDOM_ROW ;  nID++ )
	{
		if( strVarMode.LoadString( nID ) )
		{
			m_comboVarMode.AddString( strVarMode );
		}
	}
	
	m_editPChannel.SetLimitText( 3 ); // '999'
	m_spinPChannel.SetRange( MIN_PCHANNEL, MAX_PCHANNEL );

	// Limit PChannel names to DMUS_MAX_NAME - 1 characters
	m_editPChannelName.LimitText( DMUS_MAX_NAME - 1 );

	// Limit track names to DMUS_MAX_NAME - 1 characters
	m_editName.LimitText( DMUS_MAX_NAME - 1 );

	ASSERT(m_pmsAttenuation == NULL);
	m_pmsAttenuation = Insert(
		IDC_ATTENUATION,
		IDC_DB_ATTENUATION,	
		IDC_DB_ATTENUATION_SPIN,
		MYSLIDER_VOLUME,
		0,
		0, // no undo
		&m_lAttenuation );

	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabWavePart::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioWavePartPPG* pioWavePartPPG = &m_PPGPartParams;
	if( FAILED( m_pPPO->GetData( (void**)&pioWavePartPPG ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// Set PChannel
	m_spinPChannel.SetPos( m_PPGPartParams.dwPChannel + 1 );

	// Set strip name
	m_editName.SetWindowText( m_PPGPartParams.strStripName );

	// Set PChannel name
	m_editPChannelName.SetWindowText( m_PPGPartParams.strPChannelName );

	// Set volume control
	m_lAttenuation = (m_PPGPartParams.lVolume << 16) / 10;
	m_pmsAttenuation->SetValue(this, m_lAttenuation);

	// Set variation order
	int nVarMode = m_PPGPartParams.dwPartFlagsDM & VARMODE_MASK;
	switch( nVarMode )
	{
		case DMUS_VARIATIONT_RANDOM:
			m_comboVarMode.SetCurSel( 0 );
			break;

		case DMUS_VARIATIONT_SEQUENTIAL:
			m_comboVarMode.SetCurSel( 1 );
			break;
		
		case DMUS_VARIATIONT_RANDOM_START:
		case DMUS_VARIATIONT_NO_REPEAT:
		case DMUS_VARIATIONT_RANDOM_ROW:
			m_comboVarMode.SetCurSel( nVarMode );
			break;
		
		default:
			ASSERT(FALSE);
			m_comboVarMode.SetCurSel( 0 );
			break;
	}

	// Set variation lock ID
	FillVarLockComboBox();
	m_comboVarLockID.SetCurSel( m_PPGPartParams.dwLockToPart );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

BOOL CTabWavePart::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	m_pIPropSheet->GetActivePage( &CGroupBitsPropPageMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}

void CTabWavePart::UpdatePPO()
{
	if( m_pPPO )
	{
		VERIFY( SUCCEEDED( m_pPPO->SetData(&m_PPGPartParams) ) );
	}
}

void CTabWavePart::OnChangeEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString strNewPChannel;
		m_editPChannel.GetWindowText( strNewPChannel );

		// Strip leading and trailing spaces
		strNewPChannel.TrimRight();
		strNewPChannel.TrimLeft();

		if ( !strNewPChannel.IsEmpty() )
		{
			BOOL fTransSucceeded;
			int nPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			if ( !fTransSucceeded || (nPChannel < MIN_PCHANNEL) )
			{
				nPChannel = MIN_PCHANNEL;
				m_spinPChannel.SetPos( nPChannel );
			}
			else if( nPChannel > MAX_PCHANNEL )
			{
				nPChannel = MAX_PCHANNEL;
				m_spinPChannel.SetPos( nPChannel );
			}

			m_editPChannelName.EnableWindow( TRUE );

			if( (unsigned)nPChannel != (m_PPGPartParams.dwPChannel + 1) )
			{
				m_PPGPartParams.dwPChannel = nPChannel - 1;
				GetPChannelName( m_PPGPartParams.dwPChannel, m_PPGPartParams.strPChannelName );
				UpdatePPO();

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( m_PPGPartParams.strPChannelName );
				}
			}
		}
	}
}

void CTabWavePart::OnKillfocusEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString strNewPChannel;
		m_editPChannel.GetWindowText( strNewPChannel );

		// Strip leading and trailing spaces
		strNewPChannel.TrimRight();
		strNewPChannel.TrimLeft();

		if ( strNewPChannel.IsEmpty() )
		{
			m_spinPChannel.SetPos( m_PPGPartParams.dwPChannel + 1 );
			m_editPChannelName.EnableWindow( TRUE );
		}
		else
		{
			// Convert from text to dword;
			BOOL fTransSucceeded;
			int nPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			ASSERT( fTransSucceeded );
			/*
			if ( !fTransSucceeded || (nPChannel < 1) )
			{
				m_dwPChannel = 0;
				m_strPChannelName = GetPChannelName( m_dwPChannel );
				UpdatePPO();

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( m_strPChannelName );
				}
			}
			else */ if( unsigned(nPChannel - 1) != m_PPGPartParams.dwPChannel )
			{
				m_PPGPartParams.dwPChannel = nPChannel - 1;
				GetPChannelName( m_PPGPartParams.dwPChannel, m_PPGPartParams.strPChannelName );
				UpdatePPO();

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( m_PPGPartParams.strPChannelName );
				}
			}
			m_editPChannelName.EnableWindow( TRUE );
		}
	}
}

void CTabWavePart::OnChangeEditPChannelName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	CString cstrNewName;

	m_editPChannelName.GetWindowText( cstrNewName );

	// Strip leading and trailing spaces
	cstrNewName.TrimRight();
	cstrNewName.TrimLeft();

	if( cstrNewName.Compare( m_PPGPartParams.strPChannelName ) != 0 )
	{
		m_PPGPartParams.strPChannelName = cstrNewName;
		UpdatePPO();
	}
}

void CTabWavePart::OnChangeEditName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	CString cstrNewStripName;

	m_editName.GetWindowText( cstrNewStripName );

	// Strip leading and trailing spaces
	cstrNewStripName.TrimRight();
	cstrNewStripName.TrimLeft();

	if( cstrNewStripName.Compare( m_PPGPartParams.strStripName ) != 0 )
	{
		m_PPGPartParams.strStripName = cstrNewStripName;
		UpdatePPO();
	}
}

BOOL CTabWavePart::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_EDIT_PCHANNEL:
								// Set PChannel
								m_editPChannelName.EnableWindow( TRUE );
								m_spinPChannel.SetPos( m_PPGPartParams.dwPChannel + 1 );
								break;
						}
					}
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						CWnd* pWndNext = GetNextDlgTabItem( pWnd );
						if( pWndNext )
						{
							pWndNext->SetFocus();
						}
					}
					return TRUE;
				}
			}
			break;
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}

HRESULT CTabWavePart::GetPChannelName( DWORD dwPChannel, CString& strPChannelName )
{
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		return m_pPropPageMgr->m_pTrackMgr->GetPChannelName( dwPChannel, strPChannelName ); 
	}

	// Should not happen!
	ASSERT( 0 );
	strPChannelName.Empty();
	return E_FAIL;
}

void CTabWavePart::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTabWavePart::OnSelChangeComboVarMode() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	// Get variation order from combo box
	int nNewVarOrder = m_comboVarMode.GetCurSel();
	if( nNewVarOrder == CB_ERR )
	{
		return;
	}

	switch( nNewVarOrder )
	{
		case 0:
			nNewVarOrder = DMUS_VARIATIONT_RANDOM;
			break;
		
		case 1:
			nNewVarOrder = DMUS_VARIATIONT_SEQUENTIAL;
			break;
		
		default:
			// Do nothing - the index is the same as the value for
			// 2 - DMUS_VARIATIONT_RANDOM_START:
			// 3 - DMUS_VARIATIONT_NO_REPEAT:
			// 4 - DMUS_VARIATIONT_RANDOM_ROW:
			break;
	}

	// Determine new value for m_PPGPartParams.dwPartFlagsDM
	DWORD dwNewPartFlagsDM = (m_PPGPartParams.dwPartFlagsDM & ~VARMODE_MASK);
	dwNewPartFlagsDM |= (nNewVarOrder & VARMODE_MASK);
		
	if( m_PPGPartParams.dwPartFlagsDM != dwNewPartFlagsDM )
	{
		m_PPGPartParams.dwPartFlagsDM = dwNewPartFlagsDM;
		UpdatePPO();
	}
}

void CTabWavePart::OnSelChangeComboVarLock() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	// Get variation lock ID from combo box
	int nNewVarLockID = m_comboVarLockID.GetCurSel();
	if( nNewVarLockID == CB_ERR )
	{
		return;
	}

	ASSERT( nNewVarLockID >= 0 );
	if( m_PPGPartParams.dwLockToPart != (DWORD)nNewVarLockID )
	{
		m_PPGPartParams.dwLockToPart = (DWORD)nNewVarLockID;
		UpdatePPO();
	}
}

void CTabWavePart::FillVarLockComboBox( void )
{
	// Don't redraw until we are finished building the list
	m_comboVarLockID.SetRedraw( FALSE );
	
	// Remove all items
	m_comboVarLockID.ResetContent();

	// Load strings
	CString strCreate; 
	CString strNone;
	strNone.LoadString( IDS_NONE_TEXT );
	strCreate.LoadString( IDS_CREATE_TEXT );

	// Insert '<None>'
	m_comboVarLockID.AddString( strNone );

	TCHAR achText[100];

	// Rebuild InversionId list
	for( int i = 1 ;  i < 256 ;  i++ )
	{
		_itot( i, achText, 10 );

		if( m_pPropPageMgr
		&&  m_pPropPageMgr->m_pTrackMgr )
		{
			if( m_pPropPageMgr->m_pTrackMgr->IsValidLockID( (DWORD)i ) == false )
			{
				_tcscat( achText, strCreate );
			}
		}

		m_comboVarLockID.AddString( achText );
	}

	// Redraw the new list
	m_comboVarLockID.SetRedraw( TRUE );
}

BOOL CTabWavePart::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CTabWavePart::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

// CSliderCollection override
bool CTabWavePart::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			return true; // save state is done on End of updates

		case dwmscupdfEnd:
			ASSERT(pms == m_pmsAttenuation); // slider not handled
			m_PPGPartParams.lVolume = (m_lAttenuation >> 16) * 10;
			UpdatePPO();
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

