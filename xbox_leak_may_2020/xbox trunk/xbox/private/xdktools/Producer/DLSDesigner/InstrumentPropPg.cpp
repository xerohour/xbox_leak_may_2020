//
// InstrumentPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "Instrument.h"
#include "InstrumentPropPg.h"
#include "Collection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPg property page

IMPLEMENT_DYNCREATE(CInstrumentPropPg, CPropertyPage)

CInstrumentPropPg::CInstrumentPropPg() : CPropertyPage(CInstrumentPropPg::IDD), m_pInstrument(NULL), m_bSaveUndoState(true)
{
	//{{AFX_DATA_INIT(CInstrumentPropPg)
	m_csComment = _T("");
	m_csCopyright = _T("");
	m_csEngineer = _T("");
	m_csSubject = _T("");
	m_csName = _T("");
	//}}AFX_DATA_INIT

	m_fNeedToDetach = FALSE;
}

CInstrumentPropPg::~CInstrumentPropPg()
{
}

void CInstrumentPropPg::SetObject(CInstrument* pInstrument) 
{
	m_pInstrument = pInstrument;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPg::EnableControls

void CInstrumentPropPg::EnableControls(BOOL fEnable) 
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

	pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}
}

void CInstrumentPropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstrumentPropPg)
	DDX_Text(pDX, IDC_COMMENT, m_csComment);
	DDX_Text(pDX, IDC_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDC_ENGINEER, m_csEngineer);
	DDX_Text(pDX, IDC_SUBJECT, m_csSubject);
	DDX_Text(pDX, IDC_NAME, m_csName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInstrumentPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CInstrumentPropPg)
	ON_EN_CHANGE(IDC_COMMENT, OnChangeComment)
	ON_EN_CHANGE(IDC_COPYRIGHT, OnChangeCopyright)
	ON_EN_CHANGE(IDC_ENGINEER, OnChangeEngineer)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_SUBJECT, OnChangeSubject)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_COMMENT, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_ENGINEER, OnKillfocusEdits)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillfocusEdits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPg message handlers

void CInstrumentPropPg::OnChangeComment() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pInstrument->m_Info.m_csComment != m_csComment)
		{
			if(m_bSaveUndoState)
			{
				m_bSaveUndoState = false;
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_COMMENT)))
				{
					m_csComment = m_pInstrument->m_Info.m_csComment;
					UpdateData(FALSE);
					return;
				}
			}

			m_pInstrument->m_Info.m_csComment = m_csComment;
			m_pInstrument->m_pCollection->SetDirtyFlag();
		}
	}
}

void CInstrumentPropPg::OnChangeCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pInstrument->m_Info.m_csCopyright != m_csCopyright)
		{
			if(m_bSaveUndoState)
			{
				m_bSaveUndoState = false;
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_COPYRIGHT)))
				{
					m_csCopyright = m_pInstrument->m_Info.m_csCopyright; 
					UpdateData(FALSE);
					return;
				}
			}

			m_pInstrument->m_Info.m_csCopyright = m_csCopyright;
			m_pInstrument->m_pCollection->SetDirtyFlag();
		}
	}
}

void CInstrumentPropPg::OnChangeEngineer() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pInstrument->m_Info.m_csEngineer != m_csEngineer)
		{
			if(m_bSaveUndoState)
			{
				m_bSaveUndoState = false;
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_ENGINEER)))
				{
					m_csEngineer = m_pInstrument->m_Info.m_csEngineer;
					UpdateData(FALSE);
					return;
				}
			}

			m_pInstrument->m_Info.m_csEngineer = m_csEngineer;
			m_pInstrument->m_pCollection->SetDirtyFlag();
		}
	}
}

void CInstrumentPropPg::OnChangeName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pInstrument);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		// Remove the colons from the name string
		if(m_csName.Remove(TCHAR(':')))
		{
			UpdateData(FALSE);
			CEdit* pEdit = (CEdit*)GetDlgItem(IDC_NAME);
			if(pEdit)
			{
				int nLineLength = pEdit->LineLength();
				pEdit->SetSel(nLineLength, -1, TRUE);
			}
		}

		if(m_pInstrument->m_Info.m_csName != m_csName)
		{
			if(m_bSaveUndoState)
			{
				m_bSaveUndoState = false;
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_NAME)))
				{
					m_csName = m_pInstrument->m_Info.m_csName;
					UpdateData(FALSE);
					return;
				}
			}

			m_pInstrument->m_Info.m_csName = m_csName;
		
			BSTR bstrName = m_pInstrument->m_Info.m_csName.AllocSysString();
			m_pInstrument->SetNodeName(bstrName);
			m_pInstrument->m_pComponent->m_pIFramework->RefreshNode(m_pInstrument);
			m_pInstrument->m_pComponent->m_pIFramework->NotifyNodes(m_pInstrument->m_pCollection, INSTRUMENT_NameChange, NULL);
			m_pInstrument->m_pCollection->SetDirtyFlag();


		}
	}
}

void CInstrumentPropPg::OnChangeSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pInstrument);

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		if(m_pInstrument->m_Info.m_csSubject != m_csSubject)
		{
			if(m_bSaveUndoState)
			{
				m_bSaveUndoState = false;
				if(FAILED(m_pInstrument->SaveStateForUndo(IDS_UNDO_SUBJECT)))
				{
					m_csSubject = m_pInstrument->m_Info.m_csSubject;
					UpdateData(FALSE);
					return;
				}
			}
			m_pInstrument->m_Info.m_csSubject = m_csSubject;
			m_pInstrument->m_pCollection->SetDirtyFlag();
		}
	}

}

BOOL CInstrumentPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check if we're a good valid window yet....
	if(::IsWindow(m_hWnd) == FALSE)
		return FALSE;

	if(m_pInstrument == NULL)
	{
		EnableControls(FALSE);
		return CPropertyPage::OnSetActive();
	}
	
	EnableControls(TRUE);
	
	m_csName = m_pInstrument->m_Info.m_csName;
	int nSeparatorIndex  = m_csName.Find(_T(":"));
	if(nSeparatorIndex != -1)
		m_csName = m_csName.Left(nSeparatorIndex);

	m_csCopyright = m_pInstrument->m_Info.m_csCopyright;
	m_csEngineer = m_pInstrument->m_Info.m_csEngineer;
	m_csSubject = m_pInstrument->m_Info.m_csSubject;
	m_csComment = m_pInstrument->m_Info.m_csComment;
	
	return CPropertyPage::OnSetActive();
}

int CInstrumentPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CInstrumentPropPg::OnDestroy() 
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

void CInstrumentPropPg::OnKillfocusEdits() 
{
	m_bSaveUndoState = true;
}

BOOL CInstrumentPropPg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CWnd* pCtrl = GetDlgItem(IDC_COMMENT);
	if(pCtrl)
		((CEdit*)pCtrl)->SetLimitText(COMMENT_TEXT_LIMIT);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}
