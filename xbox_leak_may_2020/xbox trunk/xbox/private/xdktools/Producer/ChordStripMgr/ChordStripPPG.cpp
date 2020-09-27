// ChordStripPPG.cpp : implementation file
//

#include "stdafx.h"
#include "ChordStripPPG.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const cbKeySize = 2;
//												  0 1 2 3 4 5 6 7 8 9 1011
extern const TCHAR SharpKey[12*cbKeySize+1] = _T("C C#D D#E F F#G G#A A#B ");
extern const TCHAR FlatKey[12*cbKeySize +1] = _T("C DbD EbE F GbG AbA BbB ");

// Mapping from 'a-g' to Key Root number.(0-11)
static const int CharToKey[7] = { 9, 11, 0, 2, 4, 5, 7 };

short CChordStripPropPageMgr::sm_nActiveTab = 0;


IMPLEMENT_DYNCREATE( CKeyRootEdit, CEdit )

CKeyRootEdit::CKeyRootEdit() : CEdit()
{
}

LRESULT CKeyRootEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		//TRACE("KeyRootEdit: %d '%c' %x %x\n", wParam, (char)wParam, wParam, lParam );
		switch( wParam )
		{
		case 8:  // Backspace
		case '#':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
			// These are handled by the Edit control directly
			/*
		case 71: // Home
		case 75: // Left
		case 77: // Right
		case 79: // End
		case 82: // Ins
		case 83: // Del
			*/
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CChordStripPropPageMgr property page

CChordStripPropPageMgr::CChordStripPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pChordStripPPG = NULL;
	m_pGroupBitsPPG = NULL;
	m_pTrackFlagsPPG = NULL;
	m_fShowGroupBits = TRUE;
	CStaticPropPageManager::CStaticPropPageManager();
}

CChordStripPropPageMgr::~CChordStripPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pChordStripPPG )
	{
		delete m_pChordStripPPG;
		m_pChordStripPPG = NULL;
	}
	if( m_pGroupBitsPPG )
	{
		delete m_pGroupBitsPPG;
		m_pGroupBitsPPG = NULL;
	}
	if( m_pTrackFlagsPPG )
	{
		delete m_pTrackFlagsPPG;
		m_pTrackFlagsPPG = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT CChordStripPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
}

HRESULT CChordStripPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString cstrPart;
	cstrPart.LoadString( IDS_PROPPAGE_GROUP_BITS );
	*pbstrTitle = cstrPart.AllocSysString();

	return S_OK;
}

HRESULT CChordStripPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;
	// Add Group Bits tab, if necessary
	if( m_fShowGroupBits )
	{
		if( NULL == m_pGroupBitsPPG )
		{
			m_pGroupBitsPPG = new CGroupBitsPPG();
		}
		if( m_pGroupBitsPPG )
		{
			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pGroupBitsPPG->m_psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}

			// Weak reference
			m_pGroupBitsPPG->m_pIPropSheet = m_pIPropSheet;

			// Tell the property page to update the active tab setting
			CGroupBitsPPG::sm_pnActiveTab = &CChordStripPropPageMgr::sm_nActiveTab;
		}

		// Add track flags tab
		if( NULL == m_pTrackFlagsPPG )
		{
			m_pTrackFlagsPPG = new CTrackFlagsPPG();
		}
		if( m_pTrackFlagsPPG )
		{
			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTrackFlagsPPG->m_psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}			

			// Weak reference
			m_pTrackFlagsPPG->m_pIPropSheet = m_pIPropSheet;

			// Tell the property page to update the active tab setting
			CTrackFlagsPPG::sm_pnActiveTab = &CChordStripPropPageMgr::sm_nActiveTab;
		}
	}

	// Add Key tab
	if( NULL == m_pChordStripPPG )
	{
		m_pChordStripPPG = new CChordStripPPG();
	}
	if( m_pChordStripPPG )
	{
		m_pChordStripPPG->m_psp.pszTitle = (TCHAR *)IDS_KEY_TITLE;
		m_pChordStripPPG->m_psp.dwFlags |= PSP_USETITLE;

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pChordStripPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		
		// Weak reference
		m_pChordStripPPG->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}

HRESULT CChordStripPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pChordStripPPG )
	{
		m_pChordStripPPG->RefreshData();
	}
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->RefreshData();
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->RefreshData();
	}
	return S_OK;
}

HRESULT CChordStripPropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pChordStripPPG )
	{
		m_pChordStripPPG->SetObject( pINewPropPageObject );
	}
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->SetObject( pINewPropPageObject );
	}
	return CBasePropPageManager::SetObject( pINewPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CChordStripPPG property page

IMPLEMENT_DYNCREATE(CChordStripPPG, CPropertyPage)

CChordStripPPG::CChordStripPPG() : CPropertyPage(CChordStripPPG::IDD)
{
	//{{AFX_DATA_INIT(CChordStripPPG)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_pPropPageMgr = NULL;
	m_fNeedToDetach = FALSE;
	m_fUseFlats = FALSE;
	m_nKeyRoot = 0;
	m_nNumAccidentals = 0;
	m_fChangedKeyRoot = FALSE;
}

CChordStripPPG::~CChordStripPPG()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
}

void CChordStripPPG::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordStripPPG)
	DDX_Control(pDX, IDC_SPIN_KEY_ROOT, m_spinKeyRoot);
	DDX_Control(pDX, IDC_SPIN_ACCIDENTALS, m_spinAccidentals);
	DDX_Control(pDX, IDC_EDIT_ACCIDENTALS, m_editAccidentals);
	DDX_Control(pDX, IDC_EDIT_KEY_ROOT, m_editKeyRoot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChordStripPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CChordStripPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_FLAT, OnRadioFlat)
	ON_BN_CLICKED(IDC_RADIO_SHARP, OnRadioSharp)
	ON_EN_CHANGE(IDC_EDIT_ACCIDENTALS, OnChangeEditAccidentals)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KEY_ROOT, OnDeltaposSpinKeyRoot)
	ON_EN_KILLFOCUS(IDC_EDIT_KEY_ROOT, OnKillfocusEditKeyRoot)
	ON_EN_KILLFOCUS(IDC_EDIT_ACCIDENTALS, OnKillfocusEditAccidentals)
	ON_EN_CHANGE(IDC_EDIT_KEY_ROOT, OnChangeEditKeyRoot)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ACCIDENTALS, OnDeltaposSpinAccidentals)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CChordStripPPG::SetObject( IDMUSProdPropPageObject* pPPO )
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

void CChordStripPPG::EnableItem( int nID, BOOL fEnable )
{
	CWnd *pWnd = GetDlgItem( nID );
	if( pWnd )
	{
		pWnd->EnableWindow(fEnable);
	}
}

void CChordStripPPG::ShowItem( int nID, BOOL fEnable )
{
	CWnd *pWnd = GetDlgItem( nID );
	if( pWnd )
	{
		pWnd->ShowWindow(fEnable);
	}
}

void CChordStripPPG::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( ::IsWindow(m_editAccidentals.m_hWnd))
	{
		m_editAccidentals.EnableWindow( fEnable );
		m_spinAccidentals.EnableWindow( fEnable );
		m_editKeyRoot.EnableWindow( fEnable );
		m_spinKeyRoot.EnableWindow( fEnable );
		EnableItem( IDC_RADIO_SHARP, fEnable );
		EnableItem( IDC_RADIO_FLAT, fEnable );
		EnableItem( IDC_STATIC_KEY_ROOT, fEnable );
		EnableItem( IDC_STATIC_KEY_SIG, fEnable );
		EnableItem( IDC_STATIC_ACCIDENTALS, fEnable );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordStripPPG message handlers

int CChordStripPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CChordStripPPG::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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

BOOL CChordStripPPG::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	m_editAccidentals.SetLimitText( 1 );
	m_spinAccidentals.SetRange( 0, 7 );

	m_editKeyRoot.SetLimitText( 2 );
	m_spinKeyRoot.SetRange( 0, 11 );
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CChordStripPPG::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioChordStripPPG iChordStripPPG, *pioChordStripPPG;
	iChordStripPPG.dwPageIndex = 2;
	pioChordStripPPG = &iChordStripPPG;
	if( FAILED( m_pPPO->GetData( (void**)&pioChordStripPPG ) ) )
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

	m_fUseFlats = iChordStripPPG.fUseFlats;
	m_nKeyRoot = iChordStripPPG.nKeyRoot;
	CString strText;
	if( m_fUseFlats )
	{
		CheckRadioButton( IDC_RADIO_SHARP, IDC_RADIO_FLAT, IDC_RADIO_FLAT );
		strText.LoadString( IDS_FLAT_PROPPAGE_TEXT );
		m_editKeyRoot.SetWindowText( CString(FlatKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
	}
	else
	{
		CheckRadioButton( IDC_RADIO_SHARP, IDC_RADIO_FLAT, IDC_RADIO_SHARP );
		strText.LoadString( IDS_SHARP_PROPPAGE_TEXT );
		m_editKeyRoot.SetWindowText( CString(SharpKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
	}
	SetDlgItemText( IDC_STATIC_ACCIDENTALS, strText );

	m_nNumAccidentals = iChordStripPPG.nNumAccidentals;
	m_spinAccidentals.SetPos( m_nNumAccidentals );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

BOOL CChordStripPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( m_pPropPageMgr && m_pPropPageMgr->m_pIPropSheet )
	{
		m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CChordStripPropPageMgr::sm_nActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

void CChordStripPPG::UpdatePPO()
{
	if( m_pPPO )
	{
		ioChordStripPPG oChordStripPPG;
		oChordStripPPG.dwPageIndex = 2;
		oChordStripPPG.fUseFlats = m_fUseFlats;
		oChordStripPPG.nKeyRoot = m_nKeyRoot;
		oChordStripPPG.nNumAccidentals = m_nNumAccidentals;

		m_pPPO->SetData( (void *)&oChordStripPPG );
	}
}

BOOL CChordStripPPG::PreTranslateMessage(MSG* pMsg) 
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
				/*
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_EDIT_PCHANNEL:
								m_spinPChannel.SetPos( m_dwPChannel + 1 );
								break;
						}
					}
					return TRUE;
				}
				*/

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

void CChordStripPPG::OnRadioFlat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_fUseFlats = TRUE;
	CString strText;
	strText.LoadString( IDS_FLAT_PROPPAGE_TEXT );
	SetDlgItemText( IDC_STATIC_ACCIDENTALS, strText );
	m_editKeyRoot.SetWindowText( CString(FlatKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
	UpdatePPO();
}

void CChordStripPPG::OnRadioSharp() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_fUseFlats = FALSE;
	CString strText;
	strText.LoadString( IDS_SHARP_PROPPAGE_TEXT );
	SetDlgItemText( IDC_STATIC_ACCIDENTALS, strText );
	m_editKeyRoot.SetWindowText( CString(SharpKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
	UpdatePPO();
}

void CChordStripPPG::OnDeltaposSpinKeyRoot(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( m_pPPO )
	{
		if( m_fChangedKeyRoot )
		{
			m_fChangedKeyRoot = FALSE;
			if (m_editKeyRoot.GetSafeHwnd() != NULL)
			{
				CString strNewKeyRoot;
				m_editKeyRoot.GetWindowText( strNewKeyRoot );
				if ( !strNewKeyRoot.IsEmpty() )
				{
					strNewKeyRoot.MakeLower();
					char cKey = strNewKeyRoot[0] - 'a';
					char cAcc = 0;
					BOOL fNeedToRedraw = FALSE;
					if( strNewKeyRoot.GetLength() > 1 )
					{
						if( strNewKeyRoot[1] == 'b' )
						{
							cAcc = -1;
						}
						else if( strNewKeyRoot[1] == '#' )
						{
							cAcc = 1;
						}
					}
					if( cKey >= 0 && cKey <= 6 )
					{
						// Need to add 12 and %12 to handle B# and Cb
						m_nKeyRoot = (CharToKey[cKey] + cAcc + 12) % 12;
					}
				}
			}
		}
		int nNewKeyRoot = m_nKeyRoot + pNMUpDown->iDelta;
		if( nNewKeyRoot < 0 )
		{
			nNewKeyRoot = 11 - ((abs(nNewKeyRoot) - 1) % 12);
		}
		nNewKeyRoot %= 12;

		if( nNewKeyRoot != m_nKeyRoot )
		{
			m_nKeyRoot = nNewKeyRoot;

			if( m_fUseFlats )
			{
				m_editKeyRoot.SetWindowText( CString(FlatKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
			}
			else
			{
				m_editKeyRoot.SetWindowText( CString(SharpKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
			}
			UpdatePPO();
		}
	}
	
	*pResult = 1;
}

void CChordStripPPG::OnKillfocusEditKeyRoot() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editKeyRoot.GetSafeHwnd() != NULL)
	{
		m_fChangedKeyRoot = FALSE;

		CString strNewKeyRoot;
		m_editKeyRoot.GetWindowText( strNewKeyRoot );
		if ( !strNewKeyRoot.IsEmpty() )
		{
			strNewKeyRoot.MakeLower();
			char cKey = strNewKeyRoot[0] - 'a';
			char cAcc = 0;
			BOOL fNeedToRedraw = FALSE;
			if( strNewKeyRoot.GetLength() > 1 )
			{
				if( strNewKeyRoot[1] == 'b' )
				{
					cAcc = -1;
				}
				else if( strNewKeyRoot[1] == '#' )
				{
					cAcc = 1;
				}
			}
			if( cKey >= 0 && cKey <= 6 )
			{
				// Need to add 12 and %12 to handle B# and Cb
				m_nKeyRoot = (CharToKey[cKey] + cAcc + 12) % 12;
			}
		}

		// Redraw the Edit control with the correctly formatted key
		if( m_fUseFlats )
		{
			m_editKeyRoot.SetWindowText( CString(FlatKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
		}
		else
		{
			m_editKeyRoot.SetWindowText( CString(SharpKey).Mid(m_nKeyRoot * cbKeySize, cbKeySize) );
		}
	}
}

void CChordStripPPG::OnDeltaposSpinAccidentals(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( m_pPPO )
	{
		// Update the value
		int nNewNumAccidentals = m_nNumAccidentals;
		nNewNumAccidentals += ((NM_UPDOWN* )pNMHDR)->iDelta;
        if( nNewNumAccidentals < 0 )
        {
            nNewNumAccidentals = 7;
        }
        else if( nNewNumAccidentals > 7 )
        {
            nNewNumAccidentals = 0;
        }
		if( nNewNumAccidentals != m_nNumAccidentals )
		{
			m_nNumAccidentals = nNewNumAccidentals;
			m_spinAccidentals.SetPos( m_nNumAccidentals );
			UpdatePPO();
		}
	}
	
	*pResult = 1;
}

void CChordStripPPG::OnChangeEditAccidentals() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO )
	{
		if( m_editAccidentals.GetSafeHwnd() != NULL)
		{
			BOOL fTransSuccess = FALSE;
			int nNewNumAccidentals = GetDlgItemInt( IDC_EDIT_ACCIDENTALS, &fTransSuccess );
			if( fTransSuccess )
			{
				if( nNewNumAccidentals > 7 )
				{
					nNewNumAccidentals %= 8;
					m_spinAccidentals.SetPos( nNewNumAccidentals );
				}
				if( nNewNumAccidentals != m_nNumAccidentals )
				{
					m_nNumAccidentals = nNewNumAccidentals;
					m_spinAccidentals.SetPos( m_nNumAccidentals );
					UpdatePPO();
				}
			}
		}
	}
}

void CChordStripPPG::OnKillfocusEditAccidentals() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editAccidentals.GetSafeHwnd() != NULL)
	{
		CString strNewAccidentals;
		m_editAccidentals.GetWindowText( strNewAccidentals );
		if( strNewAccidentals.IsEmpty() )
		{
			m_spinAccidentals.SetPos( m_nNumAccidentals );
		}
		else
		{
			// Strip leading and trailing spaces
			strNewAccidentals.TrimRight();
			strNewAccidentals.TrimLeft();

			// Convert from text to dword;
			int nNewNumAccidentals = atoi( strNewAccidentals );
			nNewNumAccidentals %= 8;
			if( nNewNumAccidentals != m_nNumAccidentals )
			{
				m_nNumAccidentals = nNewNumAccidentals;
				m_spinAccidentals.SetPos( m_nNumAccidentals );
				UpdatePPO();
			}
		}
	}
}

void CChordStripPPG::OnChangeEditKeyRoot() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_fChangedKeyRoot = TRUE;
}
