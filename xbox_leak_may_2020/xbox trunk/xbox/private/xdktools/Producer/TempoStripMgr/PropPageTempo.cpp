// PropPageTempo.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropTempo.h"
#include "PropPageMgr.h"
#include "PropPageTempo.h"
#include <dmusici.h>
#include <math.h>
#include "TempoMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DIALOG_LEN 15

// CLockoutNotification class

CLockoutNotification::CLockoutNotification( HWND hWnd )
{
	// prevent control notifications from being dispatched
	m_pThreadState = AfxGetThreadState();
	m_hWndOldLockout = m_pThreadState->m_hLockoutNotifyWindow;
	m_fReset = FALSE;
	if( m_hWndOldLockout != hWnd )
	{
		m_fReset = TRUE;
		m_pThreadState->m_hLockoutNotifyWindow = hWnd;
	}
}

CLockoutNotification::~CLockoutNotification()
{
	// Reinstate control notifications
	if( m_fReset )
	{
		m_pThreadState->m_hLockoutNotifyWindow = m_hWndOldLockout;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr constructor/destructor

CTempoPropPageMgr::CTempoPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPageTempo = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CTempoPropPageMgr::~CTempoPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageTempo )
	{
		delete m_pPropPageTempo;
		m_pPropPageTempo = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CTempoPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CTempoPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;

	// Get and store the title.
	strTitle.LoadString( IDS_PROPPAGE_TEMPO );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CTempoPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
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

	// Add Tempo tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageTempo)
		m_pPropPageTempo = new PropPageTempo();

	if( m_pPropPageTempo )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageTempo->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageTempo->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CTempoPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CTempoPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropTempo* pTempo;
	if( m_pIPropPageObject == NULL )
	{
		pTempo = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pTempo ) ) )
	{
		return E_FAIL;
	}
	if (m_pPropPageTempo )
	{
		// pTempo may be NULL, meaning multiple items are selected
		m_pPropPageTempo->SetTempo( pTempo );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
};


/////////////////////////////////////////////////////////////////////////////
// PropPageTempo property page

IMPLEMENT_DYNCREATE(PropPageTempo, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageTempo constructor/destructor

PropPageTempo::PropPageTempo(): CPropertyPage(PropPageTempo::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_TEMPO_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageTempo)
	//}}AFX_DATA_INIT
	m_pPropPageMgr = NULL;
	m_fValidTempo = FALSE;
	m_fNeedToDetach = FALSE;
	m_fMultipleSelect = FALSE;
}

PropPageTempo::~PropPageTempo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTempo::DoDataExchange

void PropPageTempo::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageTempo)
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_SPIN_TEMPO, m_spinTempo);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_TEMPO, m_editTempo);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageTempo, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageTempo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_TEMPO, OnKillfocusEditTempo)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEMPO, OnDeltaposSpinTempo)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_WM_KILLFOCUS()
	ON_EN_UPDATE(IDC_EDIT_TEMPO, OnUpdateEditTempo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageTempo custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageTempo::SetTempo

void PropPageTempo::SetTempo( const CPropTempo* pTempo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pTempo == NULL || (pTempo->m_wFlags & BF_MULTIPLE) )
	{
		// prevent control notifications from being dispatched when changing edit text
		CLockoutNotification LockoutNotification( m_hWnd );
		m_fValidTempo = FALSE;
		m_fMultipleSelect = (pTempo != NULL) ? TRUE : FALSE;
		EnableControls( FALSE );
		m_Tempo.Clear();
		return;
	}

	m_fMultipleSelect = FALSE;

	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		m_fValidTempo = TRUE;
		m_Tempo.Copy( pTempo );
		return;
	}

	// Here you update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.
	EnableControls( TRUE );

	// prevent control notifications from being dispatched when changing edit text
	CLockoutNotification LockoutNotification( m_hWnd );

	CString strTemp, strTemp2;
	strTemp.Format("%.2f", pTempo->m_dblTempo );
	m_editTempo.GetWindowText( strTemp2 );
	if( strTemp2 != strTemp )
	{
		m_editTempo.SetWindowText( strTemp );
	}

	if( m_spinMeasure.GetPos() != pTempo->m_lMeasure + 1 )
	{
		m_spinMeasure.SetPos( pTempo->m_lMeasure + 1 );
	}
	if( m_spinBeat.GetPos() != pTempo->m_lBeat + 1 )
	{
		m_spinBeat.SetPos( pTempo->m_lBeat + 1 );
	}
	if( (m_editTick.GetWindowTextLength() == 0) || (m_spinTick.GetPos() != pTempo->m_lOffset) )
	{
		m_editTick.SetWindowText("0");
		m_spinTick.SetPos( pTempo->m_lOffset );
	}

	m_fValidTempo = TRUE;
	m_Tempo.Copy( pTempo );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTempo message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageTempo::OnCreate

int PropPageTempo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTempo::OnDestroy

void PropPageTempo::OnDestroy() 
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


/////////////////////////////////////////////////////////////////////////////
// PropPageTempo::OnInitDialog

BOOL PropPageTempo::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// TODO: Implement range limits for edit controls?
	/*
	if( m_pTempoMgr && m_pTempoMgr->m_pTimeline )
	{
		VARIANT var;
		if( FAILED( m_pTempoMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var ) ) )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		long lMeasure;
		if( FAILED( m_pTempoMgr->m_pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, V_I4(&var), &lMeasure, NULL ) ) )
		{
			ASSERT(FALSE);
			return FALSE;
		}
		// Ensure lMeasure is at least 1
		lMeasure = max( 1, lMaxMeasure );

		// Set range of Measure control
		TCHAR tcstr[DIALOG_LEN];
		_itot( lMeasure + 1, tcstr, 10 );
		m_editMeasure.LimitText( _tcslen( tcstr ) );
		m_spinMeasure.SetRange( 1, lMeasure );

		m_spinBeat.SetRange( 1, 256 );
		m_editBeat.LimitText( 3 );

		m_spinTick.SetRange( -32768, 32767 );
		m_editTick.LimitText( 5 );
	}
	else
	*/
	{
		//ASSERT(FALSE);
		m_spinMeasure.SetRange( 1, 32767 );
		m_editMeasure.LimitText( 5 );

		m_spinBeat.SetRange( 1, 256 );
		m_editBeat.LimitText( 3 );

		m_spinTick.SetRange( -32768, 32767 );
		m_editTick.LimitText( 5 );
	}

	m_spinTempo.SetRange( DMUS_TEMPO_MIN, DMUS_TEMPO_MAX );
	m_editTempo.LimitText( 6 ); // 350.00

	if(	m_fValidTempo )
	{
		m_fValidTempo = FALSE;
		// Update the dialog
		SetTempo( &m_Tempo );
	}

	EnableControls( m_fValidTempo );

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageTempo, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageTempo)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageTempo::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditBeat();

	int nNewValue = m_Tempo.m_lBeat + pNMUpDown->iDelta;

	if( nNewValue != m_Tempo.m_lBeat )
	{
		m_Tempo.m_lBeat = nNewValue;
		UpdateObject();
	}

	*pResult = 1;
}

void PropPageTempo::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditMeasure();

	int nNewValue = m_Tempo.m_lMeasure + pNMUpDown->iDelta;

	if( nNewValue != m_Tempo.m_lMeasure )
	{
		m_Tempo.m_lMeasure = nNewValue;
		UpdateObject();
	}

	*pResult = 1;
}

void PropPageTempo::OnKillfocusEditBeat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewBeat;

	m_editBeat.GetWindowText( strNewBeat );

	// Strip leading and trailing spaces
	strNewBeat.TrimRight();
	strNewBeat.TrimLeft();

	if( strNewBeat.IsEmpty() )
	{
		m_spinBeat.SetPos( m_Tempo.m_lBeat + 1 );
	}
	else
	{
		int iNewBeat = _ttoi( strNewBeat );
		if( iNewBeat > 256 )
		{
			iNewBeat = 256;
		}
		else if( iNewBeat < 1 )
		{
			iNewBeat = 1;
		}

		m_spinBeat.SetPos( iNewBeat );
	
		iNewBeat--;
		if( iNewBeat != m_Tempo.m_lBeat )
		{
			m_Tempo.m_lBeat = (BYTE)iNewBeat;
			UpdateObject();
		}
	}
}

void PropPageTempo::OnKillfocusEditMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewMeasure;

	m_editMeasure.GetWindowText( strNewMeasure );

	// Strip leading and trailing spaces
	strNewMeasure.TrimRight();
	strNewMeasure.TrimLeft();

	if( strNewMeasure.IsEmpty() )
	{
		m_spinMeasure.SetPos( m_Tempo.m_lMeasure + 1 );
	}
	else
	{
		int iNewMeasure = _ttoi( strNewMeasure );
		if( iNewMeasure > 32767 )
		{
			iNewMeasure = 32767;
		}
		else if( iNewMeasure < 1 )
		{
			iNewMeasure = 1;
		}

		m_spinMeasure.SetPos( iNewMeasure );
		
		iNewMeasure--;
		if( iNewMeasure != m_Tempo.m_lMeasure )
		{
			m_Tempo.m_lMeasure = iNewMeasure;
			UpdateObject();
		}
	}
}

void PropPageTempo::OnKillfocusEditTempo() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewTempo;
	m_editTempo.GetWindowText( strNewTempo );

	// Strip leading and trailing spaces
	strNewTempo.TrimRight();
	strNewTempo.TrimLeft();

	// If empty set value to previous one
	if( strNewTempo.IsEmpty() )
	{
		strNewTempo.Format( "%.2f",m_Tempo.m_dblTempo );
		m_editTempo.SetWindowText( strNewTempo );
	}
	else
	{
		// Convert from text to double;
		double dblNewTempo;
		TCHAR *tcstrTmp;
		tcstrTmp = new TCHAR[strNewTempo.GetLength() + 1];
		tcstrTmp[0] = 0;
		if( _stscanf( strNewTempo, "%lf%s", &dblNewTempo, tcstrTmp ) )
		{
			// Check bounds
			if( dblNewTempo > DMUS_TEMPO_MAX )
			{
				dblNewTempo = DMUS_TEMPO_MAX;
				strNewTempo.Format( "%.2f",dblNewTempo );
				m_editTempo.SetWindowText( strNewTempo );
			}
			else if( dblNewTempo < DMUS_TEMPO_MIN )
			{
				dblNewTempo = DMUS_TEMPO_MIN;
				strNewTempo.Format( "%.2f",dblNewTempo );
				m_editTempo.SetWindowText( strNewTempo );
			}

			// If invalid text after the number, reset m_editTempo
			if( tcstrTmp[0] != 0 )
			{
				strNewTempo.Format( "%.2f",dblNewTempo );
				m_editTempo.SetWindowText( strNewTempo );
			}

			// Check if value changed
			if( dblNewTempo != m_Tempo.m_dblTempo )
			{
				m_Tempo.m_dblTempo = dblNewTempo;
				UpdateObject();
			}
		}
		else
		{
			// Invalid data - reset control with previous value
			strNewTempo.Format( "%.2f",m_Tempo.m_dblTempo );
			m_editTempo.SetWindowText( strNewTempo );
		}

		delete tcstrTmp;
	}
}

void PropPageTempo::UpdateObject( void )
{
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Tempo);
	}
}

void PropPageTempo::EnableControls( BOOL fEnable )
{
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_editTempo.EnableWindow( fEnable );
	m_spinTempo.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_spinTick.EnableWindow( fEnable );

	if( !fEnable )
	{
		// If multiply selected, display the 'multiple select' string.
		// If not multiply selected, display the 'none' string.
		CString strText;
		if( (m_fMultipleSelect && strText.LoadString( IDS_MULTIPLE_SELECT ))
		||	(!m_fMultipleSelect && strText.LoadString( IDS_NONE_SELECTED )) )
		{
			m_editTempo.SetWindowText( strText );
		}
		else
		{
			m_editTempo.SetWindowText( NULL );
		}

		m_editMeasure.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editTick.SetWindowText( NULL );
	}
}

BOOL PropPageTempo::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_fValidTempo )
	{
		return CPropertyPage::PreTranslateMessage( pMsg );
	}

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
							case IDC_EDIT_TEMPO: 
								{
									CString strNewTempo;
									strNewTempo.Format( "%.2f",m_Tempo.m_dblTempo );
									m_editTempo.SetWindowText( strNewTempo );
								}
								break;

							case IDC_EDIT_MEASURE: 
								m_spinMeasure.SetPos( m_Tempo.m_lMeasure + 1 );
								break;

							case IDC_EDIT_BEAT:
								m_spinBeat.SetPos( m_Tempo.m_lBeat + 1 );								break;
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

void PropPageTempo::OnDeltaposSpinTempo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fValidTempo )
	{
		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditTempo();

		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Modify the current tempo
		double dblNewValue = floor(m_Tempo.m_dblTempo + pNMUpDown->iDelta);

		// If too small, wrap to DMUS_TEMPO_MAX
		if( dblNewValue < DMUS_TEMPO_MIN )
		{
			dblNewValue = DMUS_TEMPO_MIN;
			m_spinTempo.SetPos( DMUS_TEMPO_MIN );
		}
		// If too large, wrap to DMUS_TEMPO_MIN
		else if( dblNewValue > DMUS_TEMPO_MAX )
		{
			dblNewValue = DMUS_TEMPO_MAX;
			m_spinTempo.SetPos( DMUS_TEMPO_MAX );
		}

		// If value changed
		if( dblNewValue != m_Tempo.m_dblTempo )
		{
			// Set current value
			m_Tempo.m_dblTempo = dblNewValue;
			
			// Update m_editTempo
			CString strNewTempo;
			strNewTempo.Format( "%.2f",m_Tempo.m_dblTempo );
			m_editTempo.SetWindowText( strNewTempo );

			// Update the Tempo strip
			UpdateObject();
		}
	}

	*pResult = 0;
}

void PropPageTempo::OnKillfocusEditTick() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewTick;

	m_editTick.GetWindowText( strNewTick );

	// Strip leading and trailing spaces
	strNewTick.TrimRight();
	strNewTick.TrimLeft();

	if( strNewTick.IsEmpty() )
	{
		m_spinTick.SetPos( m_Tempo.m_lOffset );
	}
	else
	{
		int iNewTick = _ttoi( strNewTick );
		if( iNewTick > 32767 )
		{
			iNewTick = 32767;
		}
		else if( (iNewTick < 0) && (m_Tempo.m_lBeat > 0 || m_Tempo.m_lMeasure > 0) )
		{
			iNewTick = 0;
		}

		m_spinTick.SetPos( iNewTick );

		if( iNewTick != m_Tempo.m_lOffset )
		{
			m_Tempo.m_lOffset = iNewTick;
			UpdateObject();
		}
	}
}

void PropPageTempo::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fValidTempo )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditTick();

		int nNewValue = m_Tempo.m_lOffset + pNMUpDown->iDelta;

		if( nNewValue != m_Tempo.m_lOffset )
		{
			m_Tempo.m_lOffset = nNewValue;
			UpdateObject();
		}
	}

	*pResult = 1;
}

void PropPageTempo::OnUpdateEditTempo() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editTempo.GetSafeHwnd() != NULL)
	{
		CString strNewTempo;
		m_editTempo.GetWindowText( strNewTempo );
		if ( !strNewTempo.IsEmpty() )
		{
			// Strip leading and trailing spaces
			strNewTempo.TrimRight();
			strNewTempo.TrimLeft();

			// Convert from text to double;
			double dblNewTempo;
			TCHAR tcstrTmp[DIALOG_LEN];
			tcstrTmp[0] = 0;
			if( _stscanf( strNewTempo, "%lf%s", &dblNewTempo, tcstrTmp ) )
			{
				// Check if value changed and is valid
				if( (dblNewTempo >= DMUS_TEMPO_MIN) && (dblNewTempo <= DMUS_TEMPO_MAX) &&
					(dblNewTempo != m_Tempo.m_dblTempo) )
				{
					m_Tempo.m_dblTempo = dblNewTempo;
					UpdateObject();
				}
			}
		}
	}
}


IMPLEMENT_DYNCREATE( CMyEdit, CEdit )

CMyEdit::CMyEdit() : CEdit()
{
}

LRESULT CMyEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8:  // Backspace
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 71: // Home
		case 75: // Left
		case 77: // Right
		case 79: // End
		case 82: // Ins
		case 83: // Del
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}
