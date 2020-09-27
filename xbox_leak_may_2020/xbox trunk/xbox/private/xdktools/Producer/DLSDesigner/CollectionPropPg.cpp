// CollectionPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "Collection.h"
#include "CollectionPropPgMgr.h"
#include "CollectionPropPg.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPg property page

IMPLEMENT_DYNCREATE(CCollectionPropPg, CPropertyPage)

CCollectionPropPg::CCollectionPropPg() : CPropertyPage(CCollectionPropPg::IDD), m_pCollection(NULL)
{
	//{{AFX_DATA_INIT(CCollectionPropPg)
	m_csComment = _T("");
	m_csCopyright = _T("");
	m_csEngineer = _T("");
	m_csName = _T("");
	m_csSubject = _T("");
	m_dwVersion1 = 0;
	m_dwVersion2 = 0;
	m_dwVersion3 = 0;
	m_dwVersion4 = 0;
	//}}AFX_DATA_INIT

	m_fNeedToDetach = FALSE;
}

CCollectionPropPg::~CCollectionPropPg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPg::EnableControls

void CCollectionPropPg::EnableControls(BOOL fEnable) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CWnd * pCtrl = GetDlgItem(IDC_NAME);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_SUBJECT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_ENGINEER);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_COPYRIGHT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_VER_1);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_VER_2);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_VER_3);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_VER_4);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}
}

void CCollectionPropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollectionPropPg)
	DDX_Text(pDX, IDC_COMMENT, m_csComment);
	DDX_Text(pDX, IDC_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDC_ENGINEER, m_csEngineer);
	DDX_Text(pDX, IDC_NAME, m_csName);
	DDX_Text(pDX, IDC_SUBJECT, m_csSubject);
	DDX_Text(pDX, IDC_VER_1, m_dwVersion1);
	DDV_MinMaxDWord(pDX, m_dwVersion1, 0, 65535);
	DDX_Text(pDX, IDC_VER_2, m_dwVersion2);
	DDV_MinMaxDWord(pDX, m_dwVersion2, 0, 65535);
	DDX_Text(pDX, IDC_VER_3, m_dwVersion3);
	DDV_MinMaxDWord(pDX, m_dwVersion3, 0, 65535);
	DDX_Text(pDX, IDC_VER_4, m_dwVersion4);
	DDV_MinMaxDWord(pDX, m_dwVersion4, 0, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCollectionPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CCollectionPropPg)
	ON_EN_CHANGE(IDC_COMMENT, OnChangeComment)
	ON_EN_CHANGE(IDC_COPYRIGHT, OnChangeCopyright)
	ON_EN_CHANGE(IDC_ENGINEER, OnChangeEngineer)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_SUBJECT, OnChangeSubject)
	ON_EN_CHANGE(IDC_VER_1, OnChangeVer1)
	ON_EN_CHANGE(IDC_VER_2, OnChangeVer2)
	ON_EN_CHANGE(IDC_VER_3, OnChangeVer3)
	ON_EN_CHANGE(IDC_VER_4, OnChangeVer4)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPg message handlers

BOOL CCollectionPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pCollection == NULL)
	{
		EnableControls(FALSE);
		CCollectionPropPgMgr::m_dwLastSelPage = COLLECTION_PROP_PAGE;
		return CPropertyPage::OnSetActive();
	}
	
	EnableControls(TRUE);
	
	m_csName = m_pCollection->m_Info.m_csName;
	m_csCopyright = m_pCollection->m_Info.m_csCopyright;
	m_csEngineer = m_pCollection->m_Info.m_csEngineer;
	m_csSubject = m_pCollection->m_Info.m_csSubject;
	m_csComment = m_pCollection->m_Info.m_csComment;
	m_dwVersion1 = HIWORD(m_pCollection->m_rVersion.dwVersionMS);
	m_dwVersion2 = LOWORD(m_pCollection->m_rVersion.dwVersionMS);
	m_dwVersion3 = HIWORD(m_pCollection->m_rVersion.dwVersionLS);
	m_dwVersion4 = LOWORD(m_pCollection->m_rVersion.dwVersionLS);
	
	CCollectionPropPgMgr::m_dwLastSelPage = COLLECTION_PROP_PAGE;
	return CPropertyPage::OnSetActive();
}

void CCollectionPropPg::OnChangeComment() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{	
		if(m_pCollection->m_Info.m_csComment != m_csComment)
		{
			m_pCollection->m_Info.m_csComment = m_csComment;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_Info.m_csCopyright != m_csCopyright)
		{
			m_pCollection->m_Info.m_csCopyright = m_csCopyright;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeEngineer() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_Info.m_csEngineer != m_csEngineer)
		{
			m_pCollection->m_Info.m_csEngineer = m_csEngineer;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_Info.m_csName != m_csName)
		{
			m_pCollection->m_Info.m_csName = m_csName;
			
			BSTR bstrName = m_pCollection->m_Info.m_csName.AllocSysString();
			m_pCollection->SetNodeName(bstrName);
			m_pCollection->m_pComponent->m_pIFramework->RefreshNode(m_pCollection);			
			
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_Info.m_csSubject != m_csSubject)
		{
			m_pCollection->m_Info.m_csSubject = m_csSubject;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeVer1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_rVersion.dwVersionMS != ((m_dwVersion1 << 16) | m_dwVersion2))
		{
			m_pCollection->m_rVersion.dwVersionMS = (m_dwVersion1 << 16) | m_dwVersion2;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeVer2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_rVersion.dwVersionMS != ((m_dwVersion1 << 16) | m_dwVersion2))
		{
			m_pCollection->m_rVersion.dwVersionMS = (m_dwVersion1 << 16) | m_dwVersion2;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeVer3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_rVersion.dwVersionLS != ((m_dwVersion3 << 16) | m_dwVersion4))
		{
			m_pCollection->m_rVersion.dwVersionLS = (m_dwVersion3 << 16) | m_dwVersion4;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnChangeVer4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pCollection->m_rVersion.dwVersionLS != ((m_dwVersion3 << 16) | m_dwVersion4))
		{
			m_pCollection->m_rVersion.dwVersionLS = (m_dwVersion3 << 16) | m_dwVersion4;
			m_pCollection->SetDirtyFlag();
		}
	}
}

void CCollectionPropPg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd != NULL )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}

int CCollectionPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CCollectionPropPg::OnKillfocusName() 
{
//	IDMUSProdPropSheet* pIPropSheet;
//	if(SUCCEEDED(m_pCollection->m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
//	{
		// Refresh title
//		pIPropSheet->RefreshTitleByObject(m_pCollection);
//		pIPropSheet->Release();
//	}
}

void CCollectionPropPg::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pCollection )
	{
		CGuidDlg dlg;
		memcpy( &dlg.m_guid, &m_pCollection->m_Guid, sizeof(GUID) );
		if( dlg.DoModal() == IDOK )
		{
			//m_pCollection->m_pUndoMgr->SaveState( m_pCollection, theApp.m_hInstance, IDS_UNDO_STYLE_GUID );
			memcpy( &m_pCollection->m_Guid, &dlg.m_guid, sizeof(GUID) );
			m_pCollection->SetDirtyFlag();

			// Notify connected nodes that DLS Collection GUID has changed
			m_pCollection->m_pComponent->m_pIFramework->NotifyNodes( m_pCollection, DOCROOT_GuidChange, NULL );
		}
	}
}

BOOL CCollectionPropPg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CWnd* pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
		((CEdit*)pCtrl)->SetLimitText(COMMENT_TEXT_LIMIT);

	
	return FALSE;  // return TRUE unless you set the focus to a control
				   // EXCEPTION: OCX Property Pages should return FALSE
}
