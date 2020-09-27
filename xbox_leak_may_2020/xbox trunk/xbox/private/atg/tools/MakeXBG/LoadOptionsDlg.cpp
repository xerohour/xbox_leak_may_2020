//-----------------------------------------------------------------------------
// File: LoadOptionsDlg.cpp
//
// Desc: Code to implement a dialog to let the user specifiy load options.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"
#include "LoadOptionsDlg.h"
#include "FVF.h"




//-----------------------------------------------------------------------------
// Name: CLoadOptionsDlg()
// Desc: Constructor
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CLoadOptionsDlg, CDialog)

CLoadOptionsDlg::CLoadOptionsDlg( CWnd* pParent /*=NULL*/ )
	      :CDialog(CLoadOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadOptionsDlg)
	//}}AFX_DATA_INIT
}




//-----------------------------------------------------------------------------
// Name: DoDataExchange()
// Desc: 
//-----------------------------------------------------------------------------
void CLoadOptionsDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadOptionsDlg)
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate )
	{
		m_bCollapseMesh = ((CButton*)GetDlgItem(IDC_COLLAPSEMESH))->GetCheck();
	}
	else
	{
		((CButton*)GetDlgItem(IDC_COLLAPSEMESH))->SetCheck( TRUE );
	}
}




BEGIN_MESSAGE_MAP(CLoadOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


