// newgrp.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "newgrp.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewGroupDlg dialog

CNewGroupDlg::CNewGroupDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(IDD_NEW_PROJ_GRP, pParent)
{
	//{{AFX_DATA_INIT(CNewGroupDlg)
		m_strGroup = "";
		m_strGroupFilter = "";
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNewGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewGroupDlg)
		DDX_Text(pDX, IDC_GROUPNAME, m_strGroup);
		DDX_Text(pDX, IDC_GROUPFILTER, m_strGroupFilter);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewGroupDlg, C3dDialog)
	//{{AFX_MSG_MAP(CNewGroupDlg)
	ON_EN_CHANGE(IDC_GROUPNAME, OnChangeGroup)
	ON_BN_CLICKED(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewGroupDlg message handlers

BOOL CNewGroupDlg::OnInitDialog()
{
	C3dDialog::OnInitDialog();
	CenterWindow();

	((CEdit *)GetDlgItem( IDC_GROUPNAME ))->LimitText( 30 );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNewGroupDlg::OnChangeGroup()
{
	// If there is text in the new group control, enable the OK button
	CString strGroup;
	ASSERT( GetDlgItem( IDC_GROUPNAME ) );
	GetDlgItem( IDC_GROUPNAME )->GetWindowText( strGroup );

	ASSERT( GetDlgItem( IDOK ) );
	GetDlgItem( IDOK )->EnableWindow( strGroup.GetLength() );
}

void CNewGroupDlg::OnOK()
{
	UpdateData();

	CString strExternalDeps; strExternalDeps.LoadString(IDS_EXT_DEPENDENCIES);
	StripLeadingAndTrailingWhite( m_strGroup );
	if (m_strGroup.IsEmpty())
	{
		MsgBox (Error, IDS_NO_EMPTY_GROUP) ;
		CEdit *pEdit = (CEdit *) GetDlgItem( IDC_GROUPNAME );

		pEdit->SetWindowText( "" );
		GotoDlgCtrl( pEdit );
	}
	else if (m_strGroup.CompareNoCase(strExternalDeps)==0)
	{
		
		CString	strError;

		MsgBox(Error, MsgText(strError, IDS_ERR_BAD_FOLDER, m_strGroup));
		CEdit *pEdit = (CEdit *) GetDlgItem( IDC_GROUPNAME );

		pEdit->SetWindowText( "" );
		GotoDlgCtrl( pEdit );
	}
	else
	{
		// UNDONE: validate filter
		C3dDialog::OnOK();
	}
}
