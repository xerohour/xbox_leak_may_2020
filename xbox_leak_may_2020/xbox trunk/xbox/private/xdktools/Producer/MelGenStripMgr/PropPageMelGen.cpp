// PropPageMelGen.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropMelGen.h"
#include "PropPageMgr.h"
#include "PropPageMelGen.h"
#include "PropPagePlay.h"
#include "PropPageConRule.h"
#include "PropPageCommand.h"
#include "MelGenMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DMUS_FRAGMENTF_USE_LABEL not supported in DX8
//#define MELGEN_FRAGMENT_MASK (DMUS_FRAGMENTF_USE_REPEAT | DMUS_FRAGMENTF_REJECT_REPEAT | DMUS_FRAGMENTF_USE_LABEL)
#define MELGEN_FRAGMENT_MASK (DMUS_FRAGMENTF_USE_REPEAT | DMUS_FRAGMENTF_REJECT_REPEAT)


/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr constructor/destructor

CMelGenPropPageMgr::CMelGenPropPageMgr(IDMUSProdFramework* pIFramework) : CStaticPropPageManager()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();
	m_pPropPageMelGen = NULL;
//	m_pPropPagePlay = NULL;		Not supported in DX8		
	m_pPropPageConRule = NULL;
	m_pPropPageCommand = NULL;
}

CMelGenPropPageMgr::~CMelGenPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageMelGen )
	{
		delete m_pPropPageMelGen;
		m_pPropPageMelGen = NULL;
	}
//	if( m_pPropPagePlay )		Not supported in DX8
//	{
//		delete m_pPropPagePlay;
//		m_pPropPagePlay = NULL;
//	}
	if( m_pPropPageConRule )
	{
		delete m_pPropPageConRule;
		m_pPropPageConRule = NULL;
	}
	if( m_pPropPageCommand )
	{
		delete m_pPropPageCommand;
		m_pPropPageCommand = NULL;
	}
	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CMelGenPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CMelGenPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	strTitle.LoadString( IDS_PROPPAGE_MELGEN );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CMelGenPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add MelGen tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageMelGen)
	{
		m_pPropPageMelGen = new PropPageMelGen;
		if( m_pPropPageMelGen )
		{
			m_pPropPageMelGen->m_pIFramework = m_pIFramework;
			m_pPropPageMelGen->m_pIFramework->AddRef();

			m_pPropPageMelGen->m_pPropPageMgr = this;
		}
	}

	if( m_pPropPageMelGen )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageMelGen->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageMelGen->m_pPropPageMgr = this;
	}

	// Add Play tab
	/* Not supported in DX8
	{
		if(!m_pPropPagePlay)
		{
			m_pPropPagePlay = new PropPagePlay;
			if( m_pPropPagePlay )
			{
				m_pPropPagePlay->m_pIFramework = m_pIFramework;
				m_pPropPagePlay->m_pIFramework->AddRef();

				m_pPropPagePlay->m_pPropPageMgr = this;
			}
		}

		if( m_pPropPagePlay )
		{
			PROPSHEETPAGE psp;
			memcpy( &psp, &m_pPropPagePlay->m_psp, sizeof(PROPSHEETPAGE) );

			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}
			m_pPropPagePlay->m_pPropPageMgr = this;
		}
	}
	*/

	// Add ConRule tab
	if(!m_pPropPageConRule)
	{
		m_pPropPageConRule = new PropPageConRule;
		if( m_pPropPageConRule )
		{
			m_pPropPageConRule->m_pIFramework = m_pIFramework;
			m_pPropPageConRule->m_pIFramework->AddRef();

			m_pPropPageConRule->m_pPropPageMgr = this;
		}
	}

	if( m_pPropPageConRule )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageConRule->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageConRule->m_pPropPageMgr = this;
	}

	// Add Command tab
	if(!m_pPropPageCommand)
	{
		m_pPropPageCommand = new PropPageCommand;
		if( m_pPropPageCommand )
		{
			m_pPropPageCommand->m_pIFramework = m_pIFramework;
			m_pPropPageCommand->m_pIFramework->AddRef();

			m_pPropPageCommand->m_pPropPageMgr = this;
		}
	}

	if( m_pPropPageCommand )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageCommand->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageCommand->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};

/////////////////////////////////////////////////////////////////////////////
// CMelGenPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CMelGenPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropMelGen* pMelGen = NULL;
	HRESULT hr = S_OK;

	if( m_pIPropPageObject == NULL )
	{
		pMelGen = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pMelGen ) ) )
	{
		pMelGen = NULL;
		hr = E_FAIL;
	}

	RefreshPropPageObject(m_pPropPageMelGen, pMelGen);
//	RefreshPropPageObject(m_pPropPagePlay, pMelGen);	Not supported in DX8
	RefreshPropPageObject(m_pPropPageConRule, pMelGen);
	RefreshPropPageObject(m_pPropPageCommand, pMelGen);
/*	if( m_pPropPageMelGen )
	{
		m_pPropPageMelGen->m_fMultipleMelGensSelected = FALSE;
		m_pPropPageMelGen->m_fHaveData = FALSE;

		if( pMelGen )
		{
			if( pMelGen->m_dwMeasure == 0xFFFFFFFF )
			{
				m_pPropPageMelGen->m_fMultipleMelGensSelected = TRUE;
			}
			else
			{
				m_pPropPageMelGen->m_fHaveData = TRUE;
			}
			m_pPropPageMelGen->CopyDataToMelGen( pMelGen );
		}
		else
		{
			CPropMelGen MelGen;
			m_pPropPageMelGen->CopyDataToMelGen( &MelGen );
		}

		m_pPropPageMelGen->UpdateControls();
	}*/

	if( pMelGen )
	{
		delete pMelGen;
	}

	return hr;
};

void CMelGenPropPageMgr::UpdateObjectWithMelGenData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropPageObject != NULL );

	CPropMelGen MelGen;
	CPropMelGen* pMelGen = NULL;

	// Populate the MelGen structure
	ZeroMemory(&MelGen, sizeof(CPropMelGen));

//////////////////////////////////////////////////////////////////////////////
//	Remove this code when m_pPropPagePlay put back
	if( SUCCEEDED ( m_pIPropPageObject->GetData( (void **)&pMelGen ) ) )
	{
		// Get values from selected object instead of from prop page
		MelGen.m_MelGen.dwFragmentFlags |= pMelGen->m_MelGen.dwFragmentFlags & MELGEN_PLAY_MASK;
		MelGen.m_MelGen.dwPlayModeFlags = pMelGen->m_MelGen.dwPlayModeFlags;
		MelGen.m_MelGen.dwTransposeIntervals = pMelGen->m_MelGen.dwTransposeIntervals;
		delete pMelGen;
	}
//////////////////////////////////////////////////////////////////////////////

	m_pPropPageMelGen->GetDataFromMelGen( &MelGen );
//	m_pPropPagePlay->GetDataFromMelGen( &MelGen );		Not supported in DX8
	m_pPropPageConRule->GetDataFromMelGen( &MelGen );
	m_pPropPageCommand->GetDataFromMelGen( &MelGen );
	
	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&MelGen );

	if( SUCCEEDED ( m_pIPropPageObject->GetData( (void **)&pMelGen ) ) )
	{
		m_pPropPageMelGen->CopyDataToMelGen( pMelGen );
//		m_pPropPagePlay->CopyDataToMelGen( pMelGen );	Not supported in DX8
		m_pPropPageConRule->CopyDataToMelGen( pMelGen );
		m_pPropPageCommand->CopyDataToMelGen( pMelGen );
		delete pMelGen;
	}
	m_pPropPageMelGen->UpdateID();

}


/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen property page

IMPLEMENT_DYNCREATE(PropPageMelGen, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen constructor/destructor

PropPageMelGen::PropPageMelGen() : CPropertyPage(PropPageMelGen::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_MELGEN_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageMelGen)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fHaveData = FALSE;
	m_fMultipleMelGensSelected = FALSE;
	m_pMelGen = new CPropMelGen;
	m_fNeedToDetach = FALSE;
}

PropPageMelGen::~PropPageMelGen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
	
	if( m_pMelGen )
	{
		delete m_pMelGen;
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen::DoDataExchange

void PropPageMelGen::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageMelGen)
	DDX_Control(pDX, IDC_VARIATIONS_PROMPT, m_staticVariationsPrompt);
	DDX_Control(pDX, IDC_REPEAT_ID_PROMPT, m_staticRepeatIdPrompt);
	DDX_Control(pDX, IDC_COMBO_OPTIONS, m_comboOptions);
	DDX_Control(pDX, IDC_LIST_VARIATIONS, m_listVariations);
	DDX_Control(pDX, IDC_EDIT_LABEL, m_editLabel);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_COMBO_REPEAT, m_comboRepeat);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageMelGen, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageMelGen)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_LBN_SELCHANGE(IDC_LIST_VARIATIONS, OnSelchangeListVariations)
	ON_CBN_SELCHANGE(IDC_COMBO_REPEAT, OnSelchangeComboRepeat)
	ON_EN_KILLFOCUS(IDC_EDIT_ID, OnKillfocusEditId)
	ON_EN_KILLFOCUS(IDC_EDIT_LABEL, OnKillfocusEditLabel)
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_COMBO_OPTIONS, OnSelchangeComboOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen::UpdateTime
void PropPageMelGen::UpdateID()
{
	char sz[20];
	_itoa( m_pMelGen->m_MelGen.dwID, sz, 10 );
	m_editID.SetWindowText(sz);
}

// PropPageMelGen::UpdateControls

void PropPageMelGen::UpdateControls()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pMelGen );
	if( m_pMelGen == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.

	// Set enable state of controls
	EnableControls( m_fHaveData );

	// Measure
	if( m_pMelGen->m_dwMeasure != (DWORD)(m_spinMeasure.GetPos() - 1) )
	{
		m_spinMeasure.SetPos( m_pMelGen->m_dwMeasure + 1 );
	}

	// Beat
	if( m_pMelGen->m_bBeat != (m_spinBeat.GetPos() - 1) )
	{
		m_spinBeat.SetPos( m_pMelGen->m_bBeat + 1 );
	}

	// Label	// Not supported in DX8
	char sz[20];
//	WideCharToMultiByte( CP_ACP, 0, m_pMelGen->m_MelGen.wszVariationLabel, -1, sz, 19, NULL, NULL );
//	m_editLabel.SetWindowText(sz);

	// ID
	_itoa( m_pMelGen->m_MelGen.dwID, sz, 10 );
	m_editID.SetWindowText(sz);

	// Options
	if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_REJECT_REPEAT)
	{
		ASSERT( m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT );
		m_comboOptions.SetCurSel(2);
	}
	else if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT)
	{
		m_comboOptions.SetCurSel(1);
	}
//	else if (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_LABEL)	// Not supported in DX8
//	{
//		m_comboOptions.SetCurSel(3);
//	}
	else
	{
		m_comboOptions.SetCurSel(0);
	}

	// Variations
	m_listVariations.SelItemRange(FALSE, 0, 31);
	if( !(m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT) )
	{
		for (int n = 0; n < 32; n++)
		{
			if ( m_pMelGen->m_MelGen.dwVariationFlags & (1 << n) )
			{
				m_listVariations.SetSel(n);
			}
		}
	}

	// Repeats
	m_comboRepeat.ResetContent();
	CString strTextUI;
	int nPos;
	int nSelPos = -1;
	char szR[10];
	char szMeasure[10];
	IMelGenMgr* pIMG = NULL;
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		HRESULT h = m_pPropPageMgr->m_pIPropPageObject->QueryInterface(IID_IMelGenMgr, (void**)&pIMG);
		if (SUCCEEDED(h))
		{
			CPropMelGen* pItem = NULL;
			for( DWORD dw = 0;  S_OK == pIMG->EnumMelGens(dw, (void**)&pItem);  dw++ )
			{
				_itoa( pItem->m_MelGen.dwID, szR, 10 );
				_itoa( (pItem->m_dwMeasure + 1), szMeasure, 10 );
				strTextUI.Format( "Bar %s: %s", szMeasure, szR );
				if( pItem->m_pRepeat
				&&  pItem->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT )
				{
					CString strText;
					pItem->FormatTextForStrip( strText );
					strTextUI = strTextUI + _T( " (" ) + strText + _T( ")" );
				}
				nPos = m_comboRepeat.AddString(strTextUI);
				m_comboRepeat.SetItemData( nPos, pItem->m_MelGen.dwID );

				if( nSelPos == -1 )
				{
					if( m_pMelGen->m_pRepeat
					&&  m_pMelGen->m_pRepeat->dwID == pItem->m_MelGen.dwID )
					{
						nSelPos = nPos;
					}
				}
			}
			pIMG->Release();
		}

		if( m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT )
		{
			m_comboRepeat.SetCurSel( nSelPos );
		}
	}

}


/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageMelGen::OnCreate

int PropPageMelGen::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageMelGen::OnDestroy

void PropPageMelGen::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

/*
	// Delete the time signature button's bitmap
	HBITMAP hBitmap = m_btnMelGen.GetBitmap();
	if( hBitmap )
	{
		::DeleteObject( hBitmap );
	}
*/

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
// PropPageMelGen::OnInitDialog

BOOL PropPageMelGen::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	m_spinMeasure.SetRange( 1, 32767 );
	m_spinBeat.SetRange( 1, 256 );
	m_editID.LimitText( 5 );
	m_editMeasure.LimitText( 5 );
	m_editBeat.LimitText( 3 );

	// Init Repeat
	// No init necessary --- values will be drawn from IDs currently in use

	// Init Variations
	m_listVariations.SetColumnWidth(20);
	char sz[3];
	for (int i = 1; i <= 32; i++)
	{
		_itoa( i, sz, 10 );
		m_listVariations.AddString(sz);
	}

	// Init Options
	m_comboOptions.AddString("Use Variations");
	m_comboOptions.AddString("Repeat Composed Variation from");
	m_comboOptions.AddString("Don't Repeat Composed Variation from");
//	m_comboOptions.AddString("Use Label");	// Not supported in DX8

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageMelGen, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageMelGen)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageMelGen::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	*pResult = 1;

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		long lNewMeasure = m_spinMeasure.GetPos() + pNMUpDown->iDelta;
		lNewMeasure--;

		if( (DWORD)lNewMeasure != m_pMelGen->m_dwMeasure )
		{
			long lNewBeat = (char)m_pMelGen->m_bBeat;
			
			// Skip over existing fragments
			if( AdjustTime( lNewMeasure, lNewBeat, pNMUpDown->iDelta, true ) )
			{
				if( (DWORD)lNewMeasure != m_pMelGen->m_dwMeasure 
				||  (BYTE)lNewBeat != m_pMelGen->m_bBeat )
				{
					m_pMelGen->m_dwMeasure = lNewMeasure;
					m_pMelGen->m_bBeat = (BYTE)lNewBeat;
					m_pPropPageMgr->UpdateObjectWithMelGenData();
					return;
				}
			}
		}

		// We can't change anything so sync controls
		if( m_pMelGen->m_dwMeasure != (DWORD)(m_spinMeasure.GetPos() - 1) )
		{
			m_spinMeasure.SetPos( m_pMelGen->m_dwMeasure + 1 );
		}
		if( m_pMelGen->m_bBeat != (m_spinBeat.GetPos() - 1) )
		{
			m_spinBeat.SetPos( m_pMelGen->m_bBeat + 1 );
		}
	}
}


void PropPageMelGen::OnKillfocusEditMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewMeasure;

	m_editMeasure.GetWindowText( strNewMeasure );

	// Strip leading and trailing spaces
	strNewMeasure.TrimRight();
	strNewMeasure.TrimLeft();

	if( strNewMeasure.IsEmpty() )
	{
		m_spinMeasure.SetPos( m_pMelGen->m_dwMeasure + 1 );
	}
	else
	{
		int iNewMeasure = _ttoi( strNewMeasure );
		if( iNewMeasure > 32767 )
		{
			iNewMeasure = 32767;
			m_spinMeasure.SetPos( iNewMeasure );
		}
		else if( iNewMeasure < 1 )
		{
			iNewMeasure = 1;
			m_spinMeasure.SetPos( iNewMeasure );
		}
		iNewMeasure--;
		if( (DWORD)iNewMeasure != m_pMelGen->m_dwMeasure )
		{
			m_pMelGen->m_dwMeasure = iNewMeasure;
			m_pPropPageMgr->UpdateObjectWithMelGenData();
		}
	}
}

BOOL PropPageMelGen::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}

void PropPageMelGen::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_editMeasure.GetSafeHwnd() == NULL  || m_editMeasure.GetSafeHwnd() == NULL )
	{
		return;
	}
	bool fLabel = (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_LABEL) ? true : false;
	bool fRepeat = (m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT) ? true : false;

	m_spinMeasure.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );

	m_staticVariationsPrompt.EnableWindow( fEnable && !fLabel && !fRepeat);
	m_listVariations.EnableWindow( fEnable && !fLabel && !fRepeat);
	m_comboOptions.EnableWindow( fEnable );
//	m_editLabel.EnableWindow( fEnable  && fLabel && !fRepeat);	Not supported in DX8
	m_editID.EnableWindow( FALSE ); // this window cannot be edited
	m_staticRepeatIdPrompt.EnableWindow( fEnable && fRepeat);
	m_comboRepeat.EnableWindow( fEnable && fRepeat);
}

void PropPageMelGen::CopyDataToMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	m_pMelGen->m_dwMeasure	= pMelGen->m_dwMeasure;
	m_pMelGen->m_bBeat		= pMelGen->m_bBeat;
	m_pMelGen->m_dwBits		= pMelGen->m_dwBits;
	m_pMelGen->m_pRepeat	= pMelGen->m_pRepeat;
	m_pMelGen->m_MelGen		= pMelGen->m_MelGen;
}


void PropPageMelGen::GetDataFromMelGen( CPropMelGen* pMelGen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pMelGen != NULL );

	pMelGen->m_dwMeasure		= m_pMelGen->m_dwMeasure;
	pMelGen->m_bBeat			= m_pMelGen->m_bBeat;
	pMelGen->m_dwBits			= m_pMelGen->m_dwBits;
	pMelGen->m_pRepeat			= m_pMelGen->m_pRepeat;
	pMelGen->m_MelGen.dwFragmentFlags |= m_pMelGen->m_MelGen.dwFragmentFlags & MELGEN_FRAGMENT_MASK;
	pMelGen->m_MelGen.mtTime	= m_pMelGen->m_MelGen.mtTime;
	pMelGen->m_MelGen.dwID		= m_pMelGen->m_MelGen.dwID;
	if (m_pMelGen->m_MelGen.wszVariationLabel)
	{
		wcscpy(pMelGen->m_MelGen.wszVariationLabel, m_pMelGen->m_MelGen.wszVariationLabel);
	}
	pMelGen->m_MelGen.dwVariationFlags = m_pMelGen->m_MelGen.dwVariationFlags;
	pMelGen->m_MelGen.dwRepeatFragmentID = m_pMelGen->m_MelGen.dwRepeatFragmentID;
}

BOOL PropPageMelGen::PreTranslateMessage(MSG* pMsg) 
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
								m_spinMeasure.SetPos( m_pMelGen->m_dwMeasure + 1 );
								break;
							case IDC_EDIT_BEAT: 
								m_spinBeat.SetPos( m_pMelGen->m_bBeat+ 1 );
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
// PropPageMelGen::OnMelGen

void PropPageMelGen::OnMelGen() 
{

}


void PropPageMelGen::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	*pResult = 1;

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		long lNewBeat = m_spinBeat.GetPos() + pNMUpDown->iDelta;
		lNewBeat--;

		if( lNewBeat != m_pMelGen->m_bBeat )
		{
			long lNewMeasure = m_pMelGen->m_dwMeasure;
			
			// Skip over existing fragments
			if( AdjustTime( lNewMeasure, lNewBeat, pNMUpDown->iDelta, false ) )
			{
				if( (DWORD)lNewMeasure != m_pMelGen->m_dwMeasure 
				||  (BYTE)lNewBeat != m_pMelGen->m_bBeat )
				{
					m_pMelGen->m_dwMeasure = lNewMeasure;
					m_pMelGen->m_bBeat = (BYTE)lNewBeat;
					m_pPropPageMgr->UpdateObjectWithMelGenData();
					return;
				}
			}
		}

		// We can't change anything so sync controls
		if( m_pMelGen->m_dwMeasure != (DWORD)(m_spinMeasure.GetPos() - 1) )
		{
			m_spinMeasure.SetPos( m_pMelGen->m_dwMeasure + 1 );
		}
		if( m_pMelGen->m_bBeat != (m_spinBeat.GetPos() - 1) )
		{
			m_spinBeat.SetPos( m_pMelGen->m_bBeat + 1 );
		}
	}
}

void PropPageMelGen::OnKillfocusEditBeat() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewBeat;

	m_editBeat.GetWindowText( strNewBeat );

	// Strip leading and trailing spaces
	strNewBeat.TrimRight();
	strNewBeat.TrimLeft();

	if( strNewBeat.IsEmpty() )
	{
		m_spinBeat.SetPos( m_pMelGen->m_bBeat + 1 );
	}
	else
	{
		int iNewBeat = _ttoi( strNewBeat );
		if( iNewBeat > 128 )
		{
			iNewBeat = 128;
			m_spinBeat.SetPos( iNewBeat );
		}
		else if( iNewBeat < 1 )
		{
			iNewBeat = 1;
			m_spinBeat.SetPos( iNewBeat );
		}
		iNewBeat--;
		if( (BYTE)iNewBeat != m_pMelGen->m_bBeat )
		{
			m_pMelGen->m_bBeat = iNewBeat;
			m_pPropPageMgr->UpdateObjectWithMelGenData();
		}
	}
	
}

void PropPageMelGen::OnKillfocusEditId() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewID;

	m_editID.GetWindowText( strNewID );

	// Strip leading and trailing spaces
	strNewID.TrimRight();
	strNewID.TrimLeft();

	if( !strNewID.IsEmpty() )
	{
		int iNewID = _ttoi( strNewID );
		if( iNewID > 32767 )
		{
			iNewID = 32767;
		}
		else if( iNewID < 0 )
		{
			iNewID = 0;
		}
		if( (DWORD)iNewID != m_pMelGen->m_MelGen.dwID )
		{
			m_pMelGen->m_MelGen.dwID = iNewID;
			m_pPropPageMgr->UpdateObjectWithMelGenData();
		}
	}
}

void PropPageMelGen::OnKillfocusEditLabel() 
{
// Not supported in DX8

//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//
//	CString strNewLabel;
//
//	m_editLabel.GetWindowText( strNewLabel );
//
//	// Strip leading and trailing spaces
//	strNewLabel.TrimRight();
//	strNewLabel.TrimLeft();
//	if( !strNewLabel.IsEmpty() )
//	{
//		MultiByteToWideChar( CP_ACP, 0, strNewLabel, -1, m_pMelGen->m_MelGen.wszVariationLabel, DMUS_MAX_FRAGMENTLABEL );
//		m_pPropPageMgr->UpdateObjectWithMelGenData();
//	}
}

void PropPageMelGen::OnSelchangeComboRepeat() 
{
	CString stringRepeat;
	int nSelect = m_comboRepeat.GetCurSel();
	if (nSelect != LB_ERR)
	{
		DWORD dwNewRepeatID = m_comboRepeat.GetItemData( nSelect );

		IMelGenMgr* pIMG = NULL;
		HRESULT h = m_pPropPageMgr->m_pIPropPageObject->QueryInterface(IID_IMelGenMgr, (void**)&pIMG);
		if (SUCCEEDED(h))
		{
			CPropMelGen* pItem = NULL;
			for (DWORD dw = 0; S_OK == pIMG->EnumMelGens(dw, (void**)&pItem); dw++)
			{
				if (pItem->m_MelGen.dwID == dwNewRepeatID)
				{
					m_pMelGen->m_pRepeat = &pItem->m_MelGen;
					m_pMelGen->m_MelGen.dwRepeatFragmentID = pItem->m_MelGen.dwID;
					break;
				}
			}
			m_pPropPageMgr->UpdateObjectWithMelGenData();
			pIMG->Release();
		}
	}
}

void PropPageMelGen::OnSelchangeListVariations() 
{
	m_pMelGen->m_MelGen.dwVariationFlags = 0;
	int nSelect = m_listVariations.GetSelCount();
	if (nSelect > 0)
	{
		int* anSelections = new int[nSelect];
		if (anSelections)
		{
			m_listVariations.GetSelItems(nSelect, anSelections);
			for (int n = 0; n < nSelect; n++)
			{
				m_pMelGen->m_MelGen.dwVariationFlags |= 1 << anSelections[n];
			}

			delete [] anSelections;
		}
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
}


void PropPageMelGen::OnSelchangeComboOptions() 
{
	char szR[10];

	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_USE_REPEAT;
	m_pMelGen->m_MelGen.dwFragmentFlags &= ~DMUS_FRAGMENTF_REJECT_REPEAT;
	int nSelect = m_comboOptions.GetCurSel();
	switch (nSelect)
	{
	// case 0: use variations, so no flags are set
	case 0:
		m_comboRepeat.SetCurSel( -1 );
		if( !(m_pMelGen->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT) )
		{
			for (int n = 0; n < 32; n++)
			{
				if ( m_pMelGen->m_MelGen.dwVariationFlags & (1 << n) )
				{
					m_listVariations.SetSel(n);
				}
			}
		}
		break;

	case 1: 
		m_pMelGen->m_MelGen.dwFragmentFlags |= DMUS_FRAGMENTF_USE_REPEAT;
		m_listVariations.SelItemRange(FALSE, 0, 31);
		if( m_pMelGen->m_pRepeat )
		{
			_itoa( m_pMelGen->m_pRepeat->dwID, szR, 10 );
			m_comboRepeat.SelectString( -1, szR );
		}
		break;

	case 2: 
		m_pMelGen->m_MelGen.dwFragmentFlags |= (DMUS_FRAGMENTF_USE_REPEAT | DMUS_FRAGMENTF_REJECT_REPEAT );
		m_listVariations.SelItemRange(FALSE, 0, 31);
		if( m_pMelGen->m_pRepeat )
		{
			_itoa( m_pMelGen->m_pRepeat->dwID, szR, 10 );
			m_comboRepeat.SelectString( -1, szR );
		}
		break;
	}
	m_pPropPageMgr->UpdateObjectWithMelGenData();
	EnableControls(TRUE);
}


bool PropPageMelGen::AdjustTime( long& lNewMeasure, long& lNewBeat, int nDelta, bool fByMeasure )
{
	bool fSuccess = false;

	IMelGenMgr* pIMelGenMgr;

	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		if( SUCCEEDED ( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IMelGenMgr, (void**)&pIMelGenMgr ) ) )
		{
			while( TRUE )
			{
				HRESULT hr = pIMelGenMgr->IsMeasureBeatOpen( lNewMeasure, (BYTE)lNewBeat );
				if( hr == E_FAIL )
				{
					// Outside of segment boundaries
					break;
				}
				if( hr == S_OK )
				{
					// We found an empty measure/beat
					fSuccess = true;	
					break;
				}
				else
				{
					if( fByMeasure )
					{
						lNewMeasure += nDelta > 0 ? 1 : -1;
					}
					else
					{
						lNewBeat += nDelta > 0 ? 1 : -1;
					}
				}
			}

			pIMelGenMgr->Release();
		}
	}

	return fSuccess;
}
