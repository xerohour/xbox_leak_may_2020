// PropPageBand.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "BandMgr.h"
#include "PropBand.h"
#include "PropPageMgr.h"
#include "PropPageBand.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr constructor/destructor

CBandPropPageMgr::CBandPropPageMgr( CBandMgr* pBandMgr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pBandMgr != NULL );
	m_pBandMgr = pBandMgr;

	m_pPropPageBand = NULL;
	
	CStaticPropPageManager::CStaticPropPageManager();
}

CBandPropPageMgr::~CBandPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageBand )
	{
		delete m_pPropPageBand;
		m_pPropPageBand = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CBandPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CBandPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	strTitle.LoadString( IDS_PROPPAGE_BAND );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CBandPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add Band tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageBand)
		m_pPropPageBand = new PropPageBand();

	if( m_pPropPageBand )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageBand->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageBand->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CBandPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CBandPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropBand* pBand = NULL;
	HRESULT hr = S_OK;

	if( m_pIPropPageObject == NULL )
	{
		pBand = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pBand ) ) )
	{
		pBand = NULL;
		hr = E_FAIL;
	}

	if( m_pPropPageBand )
	{
		m_pPropPageBand->m_fMultipleBandsSelected = FALSE;
		m_pPropPageBand->m_fHaveData = FALSE;

		if( pBand )
		{
			if( pBand->m_dwMeasure == 0xFFFFFFFF )
			{
				m_pPropPageBand->m_fMultipleBandsSelected = TRUE;
			}
			else
			{
				m_pPropPageBand->m_fHaveData = TRUE;
			}
			m_pPropPageBand->CopyDataToBand( pBand );
		}
		else
		{
			CPropBand Band;
			m_pPropPageBand->CopyDataToBand( &Band );
		}

		m_pPropPageBand->UpdateControls();
	}

	if( pBand )
	{
		delete pBand;
	}

	return hr;
};

void CBandPropPageMgr::UpdateObjectWithBandData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropPageObject != NULL );

	CPropBand Band;

	// Populate the Band structure
	m_pPropPageBand->GetDataFromBand( &Band );

	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&Band );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageBand property page

IMPLEMENT_DYNCREATE(PropPageBand, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageBand constructor/destructor

PropPageBand::PropPageBand(): CPropertyPage(PropPageBand::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_BAND_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageBand)
	//}}AFX_DATA_INIT
	m_pPropPageMgr = NULL;
	m_fHaveData = FALSE;
	m_fMultipleBandsSelected = FALSE;
	m_fNeedToDetach = FALSE;
	m_pBand = new CPropBand;
}

PropPageBand::~PropPageBand()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	if( m_pBand )
	{
		delete m_pBand;
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageBand::DoDataExchange

void PropPageBand::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageBand)
	DDX_Control(pDX, IDC_SPIN_BELONGS_MEASURE, m_spinLogicalMeasure);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BEAT, m_spinLogicalBeat);
	DDX_Control(pDX, IDC_EDIT_BELONGS_MEASURE, m_editLogicalMeasure);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BEAT, m_editLogicalBeat);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_BAND, m_editBand);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageBand, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageBand)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BAND, OnKillfocusEditBand)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BEAT, OnKillFocusEditLogicalBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_MEASURE, OnKillFocusEditLogicalMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BEAT, OnDeltaPosSpinLogicalBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_MEASURE, OnDeltaPosSpinLogicalMeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageBand custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageBand::UpdateControls

void PropPageBand::UpdateControls( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pBand );
	if( m_pBand == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.

	// Set enable state of controls
	EnableControls( m_fHaveData );

	// Band name
	CString strTemp;
	CString strBandName;
	if( m_fMultipleBandsSelected )
	{
		strBandName.LoadString( IDS_MULTIPLE_SELECT );
	}
	else
	{
		if( m_fHaveData )
		{
			strBandName = m_pBand->m_strText;
		}
	}
	m_editBand.GetWindowText( strTemp );
	if( strBandName != strTemp )
	{
		m_editBand.SetWindowText( strBandName );
	}

	if( m_fHaveData == FALSE )
	{
		return;
	}

	// Measure
	if( m_pBand->m_dwMeasure != (DWORD)(m_spinMeasure.GetPos() - 1) )
	{
		m_spinMeasure.SetPos( m_pBand->m_dwMeasure + 1 );
	}

	// Beat
	if( m_pBand->m_bBeat != (m_spinBeat.GetPos() - 1) )
	{
		m_spinBeat.SetPos( m_pBand->m_bBeat + 1 );
	}

	// Tick
	if( m_pBand->m_lTick != m_spinTick.GetPos() )
	{
		m_spinTick.SetPos( m_pBand->m_lTick );
	}

	// Logical Measure
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pBandMgr
	&&  m_pPropPageMgr->m_pBandMgr->IsRefTimeTrack() == false )
	{
		if( m_pBand->m_lLogicalMeasure != (m_spinLogicalMeasure.GetPos() - 1) )
		{
			m_spinLogicalMeasure.SetPos( m_pBand->m_lLogicalMeasure + 1 );
		}

		// Logical Beat
		if( m_pBand->m_lLogicalBeat != (m_spinLogicalBeat.GetPos() - 1) )
		{
			m_spinLogicalBeat.SetPos( m_pBand->m_lLogicalBeat + 1 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageBand message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageBand::OnCreate

int PropPageBand::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageBand::OnDestroy

void PropPageBand::OnDestroy() 
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
// PropPageBand::OnInitDialog

BOOL PropPageBand::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	m_editBand.LimitText( DMUS_MAX_NAME );
	
	m_spinMeasure.SetRange( 1, 32767 );
	m_editMeasure.LimitText( 5 );

	m_spinBeat.SetRange( 1, 256 );
	m_editBeat.LimitText( 3 );
	
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editTick.LimitText( 5 );
	
	m_spinLogicalMeasure.SetRange( 1, 32767 );
	m_editLogicalMeasure.LimitText( 5 );

	m_spinLogicalBeat.SetRange( 1, 256 );
	m_editLogicalBeat.LimitText( 3 );

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageBand, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageBand)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageBand::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditBeat();

		int nNewValue = m_pBand->m_bBeat + pNMUpDown->iDelta;

		if( nNewValue != m_pBand->m_bBeat )
		{
			m_pBand->m_bBeat = (BYTE)nNewValue;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}

	*pResult = 1;
}

void PropPageBand::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditMeasure();

		int nNewValue = LOWORD(m_spinMeasure.GetPos()) + pNMUpDown->iDelta;
		if( nNewValue < 1 )
		{
			nNewValue = 1;
		}
		else if( nNewValue > 32767 )
		{
			nNewValue = 32767;
		}
		nNewValue--;

		if( (DWORD)nNewValue != m_pBand->m_dwMeasure )
		{
			m_pBand->m_dwMeasure = nNewValue;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}

	*pResult = 1;
}

void PropPageBand::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditTick();

		int nNewValue = m_pBand->m_lTick + pNMUpDown->iDelta;
		if( nNewValue < -MAX_TICK )
		{
			nNewValue = -MAX_TICK;
		}

		if( nNewValue != m_pBand->m_lTick )
		{
			m_pBand->m_lTick = nNewValue;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}

	*pResult = 1;
}

void PropPageBand::OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillFocusEditLogicalBeat();

		int nNewValue = m_pBand->m_lLogicalBeat + pNMUpDown->iDelta;

		if( nNewValue != m_pBand->m_lLogicalBeat )
		{
			m_pBand->m_lLogicalBeat = nNewValue;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}

	*pResult = 1;
}

void PropPageBand::OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillFocusEditLogicalMeasure();

		int nNewValue = LOWORD(m_spinLogicalMeasure.GetPos()) + pNMUpDown->iDelta;
		if( nNewValue < 1 )
		{
			nNewValue = 1;
		}
		else if( nNewValue > 32767 )
		{
			nNewValue = 32767;
		}
		nNewValue--;

		if( nNewValue != m_pBand->m_lLogicalMeasure )
		{
			m_pBand->m_lLogicalMeasure = nNewValue;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}

	*pResult = 1;
}

void PropPageBand::OnKillFocusEditLogicalBeat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewLogicalBeat;

	m_editLogicalBeat.GetWindowText( strNewLogicalBeat );

	// Strip leading and trailing spaces
	strNewLogicalBeat.TrimRight();
	strNewLogicalBeat.TrimLeft();

	if( strNewLogicalBeat.IsEmpty() )
	{
		m_spinLogicalBeat.SetPos( m_pBand->m_lLogicalBeat + 1 );
	}
	else
	{
		int iNewLogicalBeat = _ttoi( strNewLogicalBeat );
		if( iNewLogicalBeat > 256 )
		{
			iNewLogicalBeat = 256;
		}
		else if( iNewLogicalBeat < 1 )
		{
			iNewLogicalBeat = 1;
		}

		m_spinLogicalBeat.SetPos( iNewLogicalBeat );

		iNewLogicalBeat--;
		if( iNewLogicalBeat != m_pBand->m_lLogicalBeat )
		{
			m_pBand->m_lLogicalBeat = iNewLogicalBeat;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

void PropPageBand::OnKillFocusEditLogicalMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewLogicalMeasure;

	m_editLogicalMeasure.GetWindowText( strNewLogicalMeasure );

	// Strip leading and trailing spaces
	strNewLogicalMeasure.TrimRight();
	strNewLogicalMeasure.TrimLeft();

	if( strNewLogicalMeasure.IsEmpty() )
	{
		m_spinLogicalMeasure.SetPos( m_pBand->m_lLogicalMeasure + 1 );
	}
	else
	{
		int iNewLogicalMeasure = _ttoi( strNewLogicalMeasure );
		if( iNewLogicalMeasure > 32767 )
		{
			iNewLogicalMeasure = 32767;
		}
		else if( iNewLogicalMeasure < 1 )
		{
			iNewLogicalMeasure = 1;
		}

		m_spinLogicalMeasure.SetPos( iNewLogicalMeasure );
		
		iNewLogicalMeasure--;
		if( iNewLogicalMeasure != m_pBand->m_lLogicalMeasure )
		{
			m_pBand->m_lLogicalMeasure = iNewLogicalMeasure;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

void PropPageBand::OnKillfocusEditBeat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewBeat;

	m_editBeat.GetWindowText( strNewBeat );

	// Strip leading and trailing spaces
	strNewBeat.TrimRight();
	strNewBeat.TrimLeft();

	if( strNewBeat.IsEmpty() )
	{
		m_spinBeat.SetPos( m_pBand->m_bBeat + 1 );
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
		if( iNewBeat != m_pBand->m_bBeat )
		{
			m_pBand->m_bBeat = (BYTE)iNewBeat;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

void PropPageBand::OnKillfocusEditMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewMeasure;

	m_editMeasure.GetWindowText( strNewMeasure );

	// Strip leading and trailing spaces
	strNewMeasure.TrimRight();
	strNewMeasure.TrimLeft();

	if( strNewMeasure.IsEmpty() )
	{
		m_spinMeasure.SetPos( m_pBand->m_dwMeasure + 1 );
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
		if( (DWORD)iNewMeasure != m_pBand->m_dwMeasure )
		{
			m_pBand->m_dwMeasure = iNewMeasure;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

void PropPageBand::OnKillfocusEditTick() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewTick;

	m_editTick.GetWindowText( strNewTick );

	// Strip leading and trailing spaces
	strNewTick.TrimRight();
	strNewTick.TrimLeft();

	if( strNewTick.IsEmpty() )
	{
		m_spinTick.SetPos( m_pBand->m_lTick );
	}
	else
	{
		CString strMinus;
		strMinus.LoadString( IDS_MINUS_SIGN );

		CString strLeftChar = strNewTick.Left( 1 );
		CString strRightChars = strNewTick.Right( strNewTick.GetLength() - 1 );

		int iNewTick;

		// 1st char of '-' means negative number
		if( strMinus.CompareNoCase( strLeftChar ) == 0 )
		{
			iNewTick = _ttoi( strRightChars );
			iNewTick *= -1;
		}
		else
		{
			iNewTick = _ttoi( strNewTick );
		}
			
		if( iNewTick > MAX_TICK )
		{
			iNewTick = MAX_TICK;
		}
		else if( iNewTick < -MAX_TICK )
		{
			iNewTick = -MAX_TICK;
		}

		m_spinTick.SetPos( iNewTick );

		if( iNewTick != m_pBand->m_lTick )
		{
			m_pBand->m_lTick = iNewTick;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

void PropPageBand::OnKillfocusEditBand() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strText;
	m_editBand.GetWindowText( strText );

	// Strip leading and trailing spaces
	strText.TrimRight();
	strText.TrimLeft();

	if( strText.IsEmpty() )
	{
		m_editBand.SetWindowText( m_pBand->m_strText );
	}
	else
	{
		if( strText != m_pBand->m_strText )
		{
			m_pBand->m_strText = strText;
			m_pPropPageMgr->UpdateObjectWithBandData();
		}
	}
}

BOOL PropPageBand::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}

void PropPageBand::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	m_spinMeasure.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_spinTick.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_editBand.EnableWindow( fEnable );

	if( fEnable == FALSE )
	{
		m_editMeasure.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editTick.SetWindowText( NULL );
	}

	BOOL fEnableBelongsTo = FALSE;
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pBandMgr
	&&  m_pPropPageMgr->m_pBandMgr->IsRefTimeTrack() == false )
	{
		fEnableBelongsTo = fEnable;
	}

	m_spinLogicalMeasure.EnableWindow( fEnableBelongsTo );
	m_spinLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_editLogicalMeasure.EnableWindow( fEnableBelongsTo );
	m_editLogicalBeat.EnableWindow( fEnableBelongsTo );

	if( fEnableBelongsTo == FALSE )
	{
		m_editLogicalMeasure.SetWindowText( NULL );
		m_editLogicalBeat.SetWindowText( NULL );
	}
}

void PropPageBand::CopyDataToBand( CPropBand* pBand )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pBand != NULL );

	m_pBand->m_mtTimePhysical  = pBand->m_mtTimePhysical;
	m_pBand->m_mtTimeLogical   = pBand->m_mtTimeLogical;
	m_pBand->m_dwMeasure	   = pBand->m_dwMeasure;
	m_pBand->m_bBeat		   = pBand->m_bBeat;
	m_pBand->m_lTick		   = pBand->m_lTick;
	m_pBand->m_lLogicalMeasure = pBand->m_lLogicalMeasure;
	m_pBand->m_lLogicalBeat	   = pBand->m_lLogicalBeat;
	m_pBand->m_dwBits		   = pBand->m_dwBits;
	m_pBand->m_wFlags		   = pBand->m_wFlags;
	m_pBand->m_strText		   = pBand->m_strText;

	if( m_pBand->m_pIBandNode )
	{
		m_pBand->m_pIBandNode->Release();
		m_pBand->m_pIBandNode = NULL;
	}
	m_pBand->m_pIBandNode = pBand->m_pIBandNode;
	if( m_pBand->m_pIBandNode )
	{
		m_pBand->m_pIBandNode->AddRef();
	}
}

void PropPageBand::GetDataFromBand( CPropBand* pBand )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pBand != NULL );

	pBand->m_mtTimePhysical	 = m_pBand->m_mtTimePhysical;
	pBand->m_mtTimeLogical	 = m_pBand->m_mtTimeLogical;
	pBand->m_dwMeasure		 = m_pBand->m_dwMeasure;
	pBand->m_bBeat			 = m_pBand->m_bBeat;
	pBand->m_lTick			 = m_pBand->m_lTick;
	pBand->m_lLogicalMeasure = m_pBand->m_lLogicalMeasure;
	pBand->m_lLogicalBeat	 = m_pBand->m_lLogicalBeat;
	pBand->m_dwBits			 = m_pBand->m_dwBits;
	pBand->m_wFlags			 = m_pBand->m_wFlags;
	pBand->m_strText		 = m_pBand->m_strText;

	if( pBand->m_pIBandNode )
	{
		pBand->m_pIBandNode->Release();
		pBand->m_pIBandNode = NULL;
	}
	pBand->m_pIBandNode = m_pBand->m_pIBandNode;
	if( pBand->m_pIBandNode )
	{
		pBand->m_pIBandNode->AddRef();
	}
}

BOOL PropPageBand::PreTranslateMessage(MSG* pMsg) 
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
							case IDC_EDIT_BAND: 
								m_editBand.SetWindowText( m_pBand->m_strText );
								break;

							case IDC_EDIT_MEASURE: 
								m_spinMeasure.SetPos( m_pBand->m_dwMeasure + 1 );
								break;

							case IDC_EDIT_BEAT:
								m_spinBeat.SetPos( m_pBand->m_bBeat + 1 );
								break;

							case IDC_EDIT_TICK:
								m_spinTick.SetPos( m_pBand->m_lTick );
								break;

							case IDC_EDIT_BELONGS_MEASURE: 
								m_spinLogicalMeasure.SetPos( m_pBand->m_lLogicalMeasure + 1 );
								break;

							case IDC_EDIT_BELONGS_BEAT:
								m_spinLogicalBeat.SetPos( m_pBand->m_lLogicalBeat + 1 );
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
