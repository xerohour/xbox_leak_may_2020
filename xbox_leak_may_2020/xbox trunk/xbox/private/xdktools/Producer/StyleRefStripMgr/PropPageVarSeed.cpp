// PropPageVarSeed.cpp : implementation file
//

#include "stdafx.h"
#include "PropPageVarSeed.h"
#include "DMusProd.h"
#include <time.h>
#include "DialogEditSeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPageVarSeed property page

short *CPropPageVarSeed::sm_pnActiveTab = NULL;

IMPLEMENT_DYNCREATE(CPropPageVarSeed, CPropertyPage)

CPropPageVarSeed::CPropPageVarSeed() : CPropertyPage(CPropPageVarSeed::IDD)
{
	//{{AFX_DATA_INIT(CPropPageVarSeed)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_fNeedToDetach = FALSE;
	m_fSeedVariationsEnabled = FALSE;
	time( reinterpret_cast<long *>(&m_dwVariationSeed) );
	srand(m_dwVariationSeed);
}

CPropPageVarSeed::~CPropPageVarSeed()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
		m_pPPO = NULL;
	}
}

void CPropPageVarSeed::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageVarSeed)
	DDX_Control(pDX, IDC_CHECK_ENABLE_SEED, m_checkEnableSeed);
	DDX_Control(pDX, IDC_BUTTON_NEW_SEED, m_btnNewSeed);
	DDX_Control(pDX, IDC_BUTTON_EDIT_SEED, m_btnEditSeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageVarSeed, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageVarSeed)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_SEED, OnCheckEnableSeed)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_SEED, OnButtonEditSeed)
	ON_BN_CLICKED(IDC_BUTTON_NEW_SEED, OnButtonNewSeed)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CPropPageVarSeed::SetObject( IDMUSProdPropPageObject* pPPO )
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

void CPropPageVarSeed::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if(::IsWindow(m_checkEnableSeed.m_hWnd))
	{
		m_checkEnableSeed.EnableWindow(fEnable);
	}

	if( ::IsWindow(m_btnNewSeed.m_hWnd)
	&&	::IsWindow(m_btnEditSeed.m_hWnd) )
	{
		m_btnNewSeed.EnableWindow(fEnable && m_fSeedVariationsEnabled);
		m_btnEditSeed.EnableWindow(fEnable && m_fSeedVariationsEnabled);
	}
}

void CPropPageVarSeed::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioVarSeedPPGData iVarSeedPPGData;
	iVarSeedPPGData.dwPageIndex = 2;
	ioVarSeedPPGData *pioVarSeedPPGData = &iVarSeedPPGData;
	if( FAILED( m_pPPO->GetData( (void**)&pioVarSeedPPGData ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	m_fSeedVariationsEnabled = iVarSeedPPGData.fVariationSeedEnabled;
	m_dwVariationSeed = iVarSeedPPGData.dwVariationSeed;

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

	m_checkEnableSeed.SetCheck( m_fSeedVariationsEnabled );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;

}

/////////////////////////////////////////////////////////////////////////////
// CPropPageVarSeed message handlers

void CPropPageVarSeed::OnCheckEnableSeed() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_fSeedVariationsEnabled = m_checkEnableSeed.GetCheck();

	if( m_pPPO )
	{
		EnableControls( TRUE );
		UpdatePPO();
	}
}

void CPropPageVarSeed::OnButtonEditSeed() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialogEditSeed dlgEditSeed;
	dlgEditSeed.m_dwVariationSeed = m_dwVariationSeed;
	if( dlgEditSeed.DoModal() == IDOK )
	{
		m_dwVariationSeed = dlgEditSeed.m_dwVariationSeed;

		UpdatePPO();
	}

	m_btnEditSeed.SetFocus();
}

void CPropPageVarSeed::OnButtonNewSeed() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_dwVariationSeed = rand() * rand();

	UpdatePPO();
}

BOOL CPropPageVarSeed::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( sm_pnActiveTab && m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( sm_pnActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

void CPropPageVarSeed::UpdatePPO()
{
	if( m_pPPO )
	{
		ioVarSeedPPGData oVarSeedPPGData;
		oVarSeedPPGData.dwPageIndex = 2;
		oVarSeedPPGData.dwVariationSeed = m_dwVariationSeed;
		oVarSeedPPGData.fVariationSeedEnabled = m_fSeedVariationsEnabled;
		VERIFY( SUCCEEDED( m_pPPO->SetData(&oVarSeedPPGData) ) );
	}
}

int CPropPageVarSeed::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CPropPageVarSeed::OnDestroy() 
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

BOOL CPropPageVarSeed::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}
