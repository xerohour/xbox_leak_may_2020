//
// CollectionExtraPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "Collection.h"
#include "CollectionPropPgMgr.h"
#include "CollectionExtraPropPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollectionExtraPropPg property page

IMPLEMENT_DYNCREATE(CCollectionExtraPropPg, CPropertyPage)

CCollectionExtraPropPg::CCollectionExtraPropPg() : CPropertyPage(CCollectionExtraPropPg::IDD), m_pCollection(NULL)
{
	//{{AFX_DATA_INIT(CCollectionExtraPropPg)
	m_dwArticCount = 0;
	m_dwInstCount = 0;
	m_dwRegionCount = 0;
	m_dwSampleCount = 0;
	m_dwWaveCount = 0;
	//}}AFX_DATA_INIT

	m_fNeedToDetach = FALSE;
}

CCollectionExtraPropPg::~CCollectionExtraPropPg()
{
}

void CCollectionExtraPropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollectionExtraPropPg)
	DDX_Text(pDX, IDC_ARTIC_COUNT, m_dwArticCount);
	DDX_Text(pDX, IDC_INST_COUNT, m_dwInstCount);
	DDX_Text(pDX, IDC_REGION_COUNT, m_dwRegionCount);
	DDX_Text(pDX, IDC_SAMPLE_COUNT, m_dwSampleCount);
	DDX_Text(pDX, IDC_WAVE_COUNT, m_dwWaveCount);
	DDV_MinMaxDWord(pDX, m_dwWaveCount, 0, 1000000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCollectionExtraPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CCollectionExtraPropPg)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollectionExtraPropPg message handlers

void CCollectionExtraPropPg::OnDestroy() 
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
	}

	CPropertyPage::OnDestroy();
}

int CCollectionExtraPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.

	if(!FromHandlePermanent(m_hWnd))
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

BOOL CCollectionExtraPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pCollection == NULL)
	{
		EnableControls(FALSE);
		CCollectionPropPgMgr::m_dwLastSelPage = COLLECTION_EXTRA_PAGE;
		return CPropertyPage::OnSetActive();
	}
	
	EnableControls(TRUE);
	
	m_dwSampleCount = m_pCollection->SampleCount();
	m_dwArticCount = m_pCollection->ArticulationCount();
	m_dwInstCount = m_pCollection->InstrumentCount();
	m_dwWaveCount = m_pCollection->WaveCount();
	m_dwRegionCount = m_pCollection->RegionCount();

	UpdateData(FALSE);

	CCollectionPropPgMgr::m_dwLastSelPage = COLLECTION_EXTRA_PAGE;
	return CPropertyPage::OnSetActive();
}

void CCollectionExtraPropPg::EnableControls(BOOL fEnable) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CWnd * pCtrl = GetDlgItem(IDC_SAMPLE_COUNT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_REGION_COUNT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_WAVE_COUNT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_INST_COUNT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_ARTIC_COUNT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}
}
