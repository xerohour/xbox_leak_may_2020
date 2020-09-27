// NameUnknownQueryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "DLSComponent.h"
#include "NameUnknownQueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameUnknownQueryDialog dialog


CNameUnknownQueryDialog::CNameUnknownQueryDialog(CWnd* pParent /*=NULL*/, CDLSComponent* pComponent)
	: CDialog(CNameUnknownQueryDialog::IDD, pParent), m_pComponent(pComponent)
{
	ASSERT(pComponent);

	//{{AFX_DATA_INIT(CNameUnknownQueryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNameUnknownQueryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameUnknownQueryDialog)
	DDX_Control(pDX, IDC_QUERY_NAME_EDIT, m_QueryNameEdit);
	DDX_Control(pDX, IDC_QUERY_GUID_STATIC, m_QueryGUIDStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameUnknownQueryDialog, CDialog)
	//{{AFX_MSG_MAP(CNameUnknownQueryDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameUnknownQueryDialog message handlers

BOOL CNameUnknownQueryDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetGUIDString();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNameUnknownQueryDialog::SetQueryGUID(const GUID& guidQuery)
{
	m_guidQuery = guidQuery;
}

void CNameUnknownQueryDialog::SetGUIDString()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);

	LPOLESTR psz;
	if( SUCCEEDED( ::StringFromIID(m_guidQuery, &psz) ) )
	{
		TCHAR szGuid[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );
		
		m_QueryGUIDStatic.SetWindowText(szGuid);
	}
}

void CNameUnknownQueryDialog::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pComponent);
	if(m_pComponent == NULL)
	{
		CDialog::OnOK();
		return;
	}

	m_QueryNameEdit.GetWindowText(m_sQueryName);
	
	m_sQueryName.TrimLeft();
	m_sQueryName.TrimRight();

	if(m_sQueryName.IsEmpty())
	{
		return;
	}

	int nLength = m_sQueryName.GetLength();
	for(int nIndex = 0; nIndex < nLength; nIndex++)
	{
		if(!isalnum(m_sQueryName[nIndex]))
		{
			return;
		}
	}

	// Check if the query name already exists
	GUID guidQuery;
	if(m_pComponent->IsAPresetQuery(m_sQueryName, guidQuery) == TRUE)
	{
		AfxMessageBox(IDS_ERR_QUERYNAME_CONFLICT);
		return;
	}

	
	CDialog::OnOK();
}
