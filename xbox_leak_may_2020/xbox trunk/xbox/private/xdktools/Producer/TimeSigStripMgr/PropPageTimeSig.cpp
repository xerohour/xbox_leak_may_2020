// PropPageTimeSig.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropTimeSig.h"
#include "PropPageMgr.h"
#include "PropPageTimeSig.h"
#include "TimeSignatureDlg.h"
#include "TimeSigStripMgr.h"
#include "Timeline.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT g_nGridsPerBeatBitmaps[MAX_GRIDS_PER_BEAT_ENTRIES] =
	{ 
	  IDB_GPB1,     IDB_GPB2,     IDB_GPB3,     IDB_GPB4,     IDB_GPB5,		// Beat = quarter note
	  IDB_GPB6,     IDB_GPB7,     IDB_GPB8,     IDB_GPB9,     IDB_GPBa10,
	  IDB_GPBa11,   IDB_GPBa12,   IDB_GPBa13,   IDB_GPBa14,   IDB_GPBa15,
	  IDB_GPBa16,   IDB_GPBa17,   IDB_GPBa18,   IDB_GPBa19,   IDB_GPBb20,
	  IDB_GPBb21,   IDB_GPBb22,   IDB_GPBb23,   IDB_GPBb24,
	  IDB_GPB1_ALT, IDB_GPB2_ALT, IDB_GPB3_ALT, IDB_GPB4_ALT, IDB_GPB5,		// Beat != quarter note
	  IDB_GPB6_ALT, IDB_GPB7,     IDB_GPB8_ALT, IDB_GPB9,     IDB_GPBa10,
	  IDB_GPBa11,   IDB_GPBa12,   IDB_GPBa13,   IDB_GPBa14,   IDB_GPBa15,
	  IDB_GPBa16,   IDB_GPBa17,   IDB_GPBa18,   IDB_GPBa19,   IDB_GPBb20,
	  IDB_GPBb21,   IDB_GPBb22,   IDB_GPBb23,   IDB_GPBb24
	};

/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr constructor/destructor

CTimeSigPropPageMgr::CTimeSigPropPageMgr(IDMUSProdFramework* pIFramework) : CStaticPropPageManager()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();
	m_pPropPageTimeSig = NULL;
//	CStaticPropPageManager::CStaticPropPageManager();
}

CTimeSigPropPageMgr::~CTimeSigPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageTimeSig )
	{
		delete m_pPropPageTimeSig;
		m_pPropPageTimeSig = NULL;
	}
	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CTimeSigPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CTimeSigPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	strTitle.LoadString( IDS_PROPPAGE_TIMESIG );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CTimeSigPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add TimeSig tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageTimeSig)
	{
		m_pPropPageTimeSig = new PropPageTimeSig;
		if( m_pPropPageTimeSig )
		{
			m_pPropPageTimeSig->m_pIFramework = m_pIFramework;
			m_pPropPageTimeSig->m_pIFramework->AddRef();

			m_pPropPageTimeSig->m_pPropPageMgr = this;
		}
	}

	if( m_pPropPageTimeSig )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageTimeSig->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageTimeSig->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CTimeSigPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CTimeSigPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropTimeSig* pTimeSig = NULL;
	HRESULT hr = S_OK;

	if( m_pIPropPageObject == NULL )
	{
		pTimeSig = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pTimeSig ) ) )
	{
		pTimeSig = NULL;
		hr = E_FAIL;
	}

	if( m_pPropPageTimeSig )
	{
		m_pPropPageTimeSig->m_fMultipleTimeSigsSelected = FALSE;
		m_pPropPageTimeSig->m_fHaveData = FALSE;

		if( pTimeSig )
		{
			if( pTimeSig->m_dwMeasure == 0xFFFFFFFF )
			{
				m_pPropPageTimeSig->m_fMultipleTimeSigsSelected = TRUE;
			}
			else
			{
				m_pPropPageTimeSig->m_fHaveData = TRUE;
			}
			m_pPropPageTimeSig->CopyDataToTimeSig( pTimeSig );
		}
		else
		{
			CPropTimeSig TimeSig;
			m_pPropPageTimeSig->CopyDataToTimeSig( &TimeSig );
		}

		m_pPropPageTimeSig->UpdateControls();
	}

	if( pTimeSig )
	{
		delete pTimeSig;
	}

	return hr;
};

void CTimeSigPropPageMgr::UpdateObjectWithTimeSigData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropPageObject != NULL );

	CPropTimeSig TimeSig;

	// Populate the TimeSig structure
	m_pPropPageTimeSig->GetDataFromTimeSig( &TimeSig );
	
	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&TimeSig );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig property page

IMPLEMENT_DYNCREATE(PropPageTimeSig, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig constructor/destructor

PropPageTimeSig::PropPageTimeSig() : CPropertyPage(PropPageTimeSig::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_TIMESIG_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageTimeSig)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fHaveData = FALSE;
	m_fMultipleTimeSigsSelected = FALSE;
	m_pTimeSig = new CPropTimeSig;
	m_fNeedToDetach = FALSE;
}

PropPageTimeSig::~PropPageTimeSig()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
	
	if( m_pTimeSig )
	{
		delete m_pTimeSig;
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::DoDataExchange

void PropPageTimeSig::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageTimeSig)
	DDX_Control(pDX, IDC_INFO_STATIC, m_InfoStatic);
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageTimeSig, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageTimeSig)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::UpdateControls

void PropPageTimeSig::UpdateControls()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pTimeSig );
	if( m_pTimeSig == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.
	m_bHasStyleTrack = HasStyleTrack(); 
	CString sInfoString("");
	if(m_bHasStyleTrack == TRUE)
		sInfoString.LoadString(IDS_EDITING_INFO);
	m_InfoStatic.SetWindowText(sInfoString);
	
	
	// Set enable state of controls
	EnableControls( m_fHaveData && !(m_pTimeSig->m_dwBits & UD_STYLEUPDATE) );

	// Measure
	if( (signed)m_pTimeSig->m_dwMeasure != (m_spinMeasure.GetPos() - 1) )
	{
		m_spinMeasure.SetPos( m_pTimeSig->m_dwMeasure + 1 );
	}

	// TimeSig button
	SetTimeSignatureBitmap();
}

BOOL PropPageTimeSig::HasStyleTrack()
{
	BOOL bHasStyleTrack = FALSE;
	IDMUSProdStripMgr *pIStripMgr;
	if( m_pPropPageMgr->m_pIPropPageObject && SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
	{
		DMUS_IO_TRACK_HEADER ioTrackHeader;
		ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
		VARIANT varTrackHeader;
		varTrackHeader.vt = VT_BYREF;
		V_BYREF(&varTrackHeader) = &ioTrackHeader;
		pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader );

		VARIANT varTimeline;
		if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline ) ) )
		{
			IDMUSProdTimeline* pITimelineCtl;
			if( SUCCEEDED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void **)&pITimelineCtl ) ) )
			{
				IDMUSProdStripMgr* pIStripMgr = NULL;
				if( SUCCEEDED( pITimelineCtl->GetStripMgr( GUID_IDirectMusicStyle, ioTrackHeader.dwGroup, 0, &pIStripMgr ) ) )
				{
					bHasStyleTrack = TRUE;
					pIStripMgr->Release();
				}
				pITimelineCtl->Release();
			}
			V_UNKNOWN( &varTimeline )->Release();
		}
		pIStripMgr->Release();
	}

	return bHasStyleTrack;
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::OnCreate

int PropPageTimeSig::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageTimeSig::OnDestroy

void PropPageTimeSig::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete the time signature button's bitmap
	HBITMAP hBitmap = m_btnTimeSignature.GetBitmap();
	if( hBitmap )
	{
		::DeleteObject( hBitmap );
	}

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
// PropPageTimeSig::OnInitDialog

BOOL PropPageTimeSig::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	m_spinMeasure.SetRange( 1, 32767 );
	m_editMeasure.LimitText( 5 );

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageTimeSig, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageTimeSig)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageTimeSig::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		CString strNewMeasure;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditMeasure();

		m_editMeasure.GetWindowText( strNewMeasure );

		// Strip leading and trailing spaces
		strNewMeasure.TrimRight();
		strNewMeasure.TrimLeft();

		if( !strNewMeasure.IsEmpty() )
		{
			m_pTimeSig->m_dwMeasure = max( _ttoi( strNewMeasure ) - 1, 0 );
		}

		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		int nNewValue = m_pTimeSig->m_dwMeasure + pNMUpDown->iDelta;
		int nDelta = pNMUpDown->iDelta;
		if( nNewValue < 0 )
		{
			nNewValue = 0;
			nDelta = -(signed(m_pTimeSig->m_dwMeasure));
		}
		else if( nNewValue > 32766 )
		{
			nNewValue = 32766;
			nDelta = 32766 - m_pTimeSig->m_dwMeasure;
		}

		if( (DWORD)nNewValue != m_pTimeSig->m_dwMeasure )
		{
			nNewValue = AdjustTime( nNewValue, nDelta );

			if( (DWORD)nNewValue != m_pTimeSig->m_dwMeasure )
			{
				m_pTimeSig->m_dwMeasure = nNewValue;
				m_spinMeasure.SetPos( nNewValue + 1 );
				m_pPropPageMgr->UpdateObjectWithTimeSigData();
			}
		}
	}

	// We handled this message - don't change the numbers further
	*pResult = 1;
}


void PropPageTimeSig::OnKillfocusEditMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewMeasure;

	m_editMeasure.GetWindowText( strNewMeasure );

	// Strip leading and trailing spaces
	strNewMeasure.TrimRight();
	strNewMeasure.TrimLeft();

	if( strNewMeasure.IsEmpty() )
	{
		m_spinMeasure.SetPos( m_pTimeSig->m_dwMeasure + 1 );
	}
	else
	{
		// Get maximum number of measures
		long lMaxMeasure = 32767;
		VARIANT var;
		IDMUSProdStripMgr *pIStripMgr;
		if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
		{
			DMUS_IO_TRACK_HEADER ioTrackHeader;
			ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
			VARIANT varTrackHeader;
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader );

			VARIANT varTimeline;
			if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline ) ) )
			{
				IDMUSProdTimeline *pITimelineCtl;
				if( SUCCEEDED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void **)&pITimelineCtl ) ) )
				{
					pITimelineCtl->GetTimelineProperty( TP_CLOCKLENGTH, &var );
					pITimelineCtl->ClocksToMeasureBeat( ioTrackHeader.dwGroup,
														0,
														V_I4( &var ),
														&lMaxMeasure,
														NULL );
					pITimelineCtl->Release();
				}
				V_UNKNOWN( &varTimeline )->Release();
			}
			pIStripMgr->Release();
		}

		// Ensure lMaxMeasure is at least 1
		lMaxMeasure = max( 1, lMaxMeasure );

		int iNewMeasure = _ttoi( strNewMeasure );
		if( iNewMeasure > lMaxMeasure )
		{
			iNewMeasure = lMaxMeasure;
		}
		else if( iNewMeasure < 1 )
		{
			iNewMeasure = 1;
		}

		m_spinMeasure.SetPos( iNewMeasure );
		
		iNewMeasure--;
		if( (DWORD)iNewMeasure != m_pTimeSig->m_dwMeasure )
		{
			m_pTimeSig->m_dwMeasure = iNewMeasure;
			m_pPropPageMgr->UpdateObjectWithTimeSigData();
		}
	}
}

BOOL PropPageTimeSig::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}

void PropPageTimeSig::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_editMeasure.GetSafeHwnd() == NULL )
	{
		return;
	}

	m_spinMeasure.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );
}

void PropPageTimeSig::CopyDataToTimeSig( CPropTimeSig* pTimeSig )
{
	ASSERT( pTimeSig != NULL );

	m_pTimeSig->m_dwMeasure		= pTimeSig->m_dwMeasure;
	m_pTimeSig->m_dwBits		= pTimeSig->m_dwBits;
	m_pTimeSig->m_TimeSignature = pTimeSig->m_TimeSignature;
}


void PropPageTimeSig::GetDataFromTimeSig( CPropTimeSig* pTimeSig )
{
	ASSERT( pTimeSig != NULL );

	pTimeSig->m_dwMeasure		= m_pTimeSig->m_dwMeasure;
	pTimeSig->m_dwBits			= m_pTimeSig->m_dwBits;
	pTimeSig->m_TimeSignature	= m_pTimeSig->m_TimeSignature;

}

BOOL PropPageTimeSig::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_fHaveData )
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
							case IDC_EDIT_MEASURE: 
								m_spinMeasure.SetPos( m_pTimeSig->m_dwMeasure + 1 );
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


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::OnTimeSignature

void PropPageTimeSig::OnTimeSignature() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimeSig )
	{
		CTimeSignatureDlg tsDlg;

		tsDlg.m_TimeSignature = m_pTimeSig->m_TimeSignature;

		if( tsDlg.DoModal() == IDOK )
		{
			// Update time signature
			m_pTimeSig->m_TimeSignature = tsDlg.m_TimeSignature;
			m_pPropPageMgr->UpdateObjectWithTimeSigData();

			// Update bitmap on time signature button
			SetTimeSignatureBitmap();
		}

		m_btnTimeSignature.SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::SetTimeSignatureBitmap

void PropPageTimeSig::SetTimeSignatureBitmap( void )
{
	HBITMAP hNewBits = NULL;

	ASSERT( m_pTimeSig != NULL );

	RECT rect;
	m_btnTimeSignature.GetClientRect( &rect );

	// Create a DC for the new bitmap
	// a DC for the 'Grids Per Beat' bitmap
	// a Bitmap for the new bits
	CDC cdcDest;
	CDC cdcGridsPerBeat;
	CBitmap bmpNewBits;
	CBitmap bmpGridsPerBeat;

	CDC* pDC = m_btnTimeSignature.GetDC();
	if( pDC )
	{

		if( cdcDest.CreateCompatibleDC( pDC ) == FALSE
		||  cdcGridsPerBeat.CreateCompatibleDC( pDC ) == FALSE
		||  bmpNewBits.CreateCompatibleBitmap( pDC, rect.right, rect.bottom ) == FALSE )
		{
			m_btnTimeSignature.ReleaseDC( pDC );
			return;
		}

		m_btnTimeSignature.ReleaseDC( pDC );
	}

	// Create the new bitmap
	CBitmap* pbmpOldMem = cdcDest.SelectObject( &bmpNewBits );

	// Fill Rect with button color
	if(m_btnTimeSignature.IsWindowEnabled())
		cdcDest.SetBkColor( ::GetSysColor(COLOR_BTNFACE) );

	cdcDest.ExtTextOut( 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	if(m_btnTimeSignature.IsWindowEnabled() )
	{
		// Write text
		CString strTimeSignature;

		CFont font;
		CFont* pfontOld = NULL;
		
		if( font.CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 							DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
		{
			pfontOld = cdcDest.SelectObject( &font );
		}

		strTimeSignature.Format( "%d/%d",
								 m_pTimeSig->m_TimeSignature.bBeatsPerMeasure,
								 m_pTimeSig->m_TimeSignature.bBeat );
		rect.left += 6;
		cdcDest.SetTextColor( COLOR_BTNTEXT );
		cdcDest.DrawText( strTimeSignature, -1, &rect, (DT_SINGLELINE | DT_LEFT | DT_VCENTER) );
		rect.left -= 6;

		if( pfontOld )
		{
			cdcDest.SelectObject( pfontOld );
			font.DeleteObject();
		}

		// Set x coord for 'Grids Per Beat' image
		CSize sizeText = cdcDest.GetTextExtent( strTimeSignature );
		int nX = max( 48, (sizeText.cx + 8) );

		// Draw "splitter"
		{
			CPen pen1;
			CPen pen2;
			CPen* ppenOld;

			int nPlace = nX - 6;
			int nModeOld = cdcDest.SetROP2( R2_COPYPEN );

			// Highlight
			if( pen1.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) ) )
			{
				ppenOld = cdcDest.SelectObject( &pen1 );
				cdcDest.MoveTo( nPlace, (rect.top + 3) );
				cdcDest.LineTo( nPlace, (rect.bottom - 3) );
				cdcDest.SelectObject( ppenOld );
			}

			// Shadow
			if( pen2.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT) ) )
			{
				ppenOld = cdcDest.SelectObject( &pen2 );
				cdcDest.MoveTo( ++nPlace, (rect.top + 3) );
				cdcDest.LineTo( nPlace, (rect.bottom - 3) );
				cdcDest.SelectObject( ppenOld );
			}

			if( nModeOld )
			{
				cdcDest.SetROP2( nModeOld );
			}
		}

		// Add 'Grids Per Beat' bitmap
		{
			int nResourceID = m_pTimeSig->m_TimeSignature.wGridsPerBeat - 1;
			if( m_pTimeSig->m_TimeSignature.bBeat != 4 )		// 4 = quarter note gets the beat
			{
				nResourceID += MAX_GRIDS_PER_BEAT;
			}
			ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) );

			if( bmpGridsPerBeat.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID] ) )
			{
				BITMAP bm;

				bmpGridsPerBeat.GetBitmap( &bm );

				int nY = ((rect.bottom - rect.top) - bm.bmHeight) >> 1;

				CBitmap* pbmpOld = cdcGridsPerBeat.SelectObject( &bmpGridsPerBeat );

				{
					CDC cdcMono;
					CBitmap bmpMono;

					if(cdcMono.CreateCompatibleDC( &cdcDest )
					&&  bmpMono.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL ) )
					{
						CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpMono );
						
						cdcGridsPerBeat.SetBkColor( RGB(255,255,255) );
						cdcDest.SetBkColor( RGB(255,255,255) );

						cdcMono.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight,
										&cdcGridsPerBeat, 0, 0, SRCCOPY);
						cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
										&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;
						cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
										&cdcMono, 0, 0, SRCAND ) ;
						cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
										&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;

						cdcMono.SelectObject( pbmpOldMono ) ;
					}
				}

				cdcGridsPerBeat.SelectObject( pbmpOld );
			}
		}
	}

	cdcDest.SelectObject( pbmpOldMem );

	// Set the new bitmap
	hNewBits = (HBITMAP)bmpNewBits.Detach();
	if( hNewBits )
	{
		HBITMAP hBitmapOld = m_btnTimeSignature.SetBitmap( hNewBits );
		if( hBitmapOld )
		{
			::DeleteObject( hBitmapOld );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageTimeSig::AdjustTime

int PropPageTimeSig::AdjustTime( int nNewValue, int nDelta )
{
	ITimeSigMgr *pITimeSigMgr;

	if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_ITimeSigMgr, (void**) &pITimeSigMgr ) ) )
	{
		// Save the original value
		const int nOrigValue = nNewValue - nDelta;

		while( TRUE )
		{
			HRESULT hr = pITimeSigMgr->IsMeasureOpen( nNewValue );
			if( FAILED(hr) )
			{
				// The measure is off the end of the segment - there are no open measures
				// after nNewValue.  Check between nOrigvalue and nNewValue
				ASSERT( nDelta > 0 );
				for( int i = nOrigValue + nDelta - 1; i > nOrigValue; i-- )
				{
					if( pITimeSigMgr->IsMeasureOpen( i ) == S_OK )
					{
						break;
					}
				}

				// If we found an open measure, i will point to it.
				// If we didn't find an open measure, i will be nOrigValue
				nNewValue = i;
				break;
			}
			else
			{
				if( hr == S_OK )
				{
					break;
				}
				else
				{
					nNewValue += nDelta > 0 ? 1 : -1;
					if( nNewValue < 0 )
					{
						// The measure is before the start of the segment - there are no open measures
						// before nNewValue.  Check between nOrigvalue and nNewValue
						ASSERT( nDelta < 0 );
						for( int i = nOrigValue - 1; i > nOrigValue + nDelta; i-- )
						{
							if( pITimeSigMgr->IsMeasureOpen( i ) == S_OK )
							{
								nNewValue = i;
								break;
							}
						}
						nNewValue = nOrigValue;
						break;
					}
				}
			}
		}
		pITimeSigMgr->Release();

		return nNewValue;
	}
	else
	{
		return nNewValue;
	}
}
