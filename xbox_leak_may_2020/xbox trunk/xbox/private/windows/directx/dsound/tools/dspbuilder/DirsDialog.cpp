/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DirsDialog.cpp

Abstract:

	Dialog for setting the directories needed by dspbuilder

Author:

	Robert Heitkamp (robheit) 20-Jul-2001

Revision History:

	20-Jul-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "DirsDialog.h"
#include "DirectoryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	CDirsDialog::CDirsDialog
//------------------------------------------------------------------------------
CDirsDialog::CDirsDialog(
						 IN CWnd*	pParent
						 ) : CDialog(CDirsDialog::IDD, pParent)
/*++

Routine Description:

	Constructor

Arguments:

	IN pParent -	Parent

Return Value:

	None

--*/
{
	//{{AFX_DATA_INIT(CDirsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pIniEdit		= NULL;
	m_pDspCodeEdit	= NULL;
}

//------------------------------------------------------------------------------
//	CDirsDialog::DoDataExchange
//------------------------------------------------------------------------------
void 
CDirsDialog::DoDataExchange(
							IN CDataExchange* pDX
							)
/*++

Routine Description:

	Handles member variable changes

Arguments:

	IN pDX

Return Value:

	None

--*/
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirsDialog, CDialog)
	//{{AFX_MSG_MAP(CDirsDialog)
	ON_BN_CLICKED(IDC_INI_FILE_PATH_BROWSE, OnIniFilePathBrowse)
	ON_BN_CLICKED(IDC_DSP_CODE_PATH_BROWSE, OnDspCodePathBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirsDialog message handlers

//------------------------------------------------------------------------------
//	CDirsDialog::OnIniFilePathBrowse
//------------------------------------------------------------------------------
void 
CDirsDialog::OnIniFilePathBrowse(void) 
/*++

Routine Description:

	Pops up a directory browser to select a directory

Arguments:

	None

Return Value:

	None

--*/
{
	CDirectoryDialog	directoryDialog;
	
	if(directoryDialog.DoModal() == IDOK)
	{
		m_iniPath = directoryDialog.GetDirectory();
		if(m_pIniEdit)
			m_pIniEdit->SetWindowText((LPCTSTR)m_iniPath);
	}
}

//------------------------------------------------------------------------------
//	CDirsDialog::OnDspCodePathBrowse
//------------------------------------------------------------------------------
void 
CDirsDialog::OnDspCodePathBrowse(void) 
/*++

Routine Description:

	Pops up a directory browser to select a directory

Arguments:

	None

Return Value:

	None

--*/
{
	CDirectoryDialog	directoryDialog;
	
	if(directoryDialog.DoModal() == IDOK)
	{
		m_dspCodePath = directoryDialog.GetDirectory();
		if(m_pDspCodeEdit)
			m_pDspCodeEdit->SetWindowText((LPCTSTR)m_dspCodePath);
	}
}


//------------------------------------------------------------------------------
//	CDirsDialog::OnInitDialog
//------------------------------------------------------------------------------
BOOL
CDirsDialog::OnInitDialog(void)
/*++

Routine Description:

	Initializes the dialog

Arguments:

	None

Return Value:

	None

--*/
{
	m_pIniEdit		= (CEdit*)GetDlgItem(IDC_INI_FILE_PATH_EDIT);
	m_pDspCodeEdit	= (CEdit*)GetDlgItem(IDC_DSP_CODE_PATH_EDIT);

	if(m_pIniEdit)
		m_pIniEdit->SetWindowText((LPCTSTR)m_iniPath);
	if(m_pDspCodeEdit)
		m_pDspCodeEdit->SetWindowText((LPCTSTR)m_dspCodePath);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CDirsDialog::OnOK
//------------------------------------------------------------------------------
void 
CDirsDialog::OnOK() 
{
	if(m_pIniEdit)
		m_pIniEdit->GetWindowText(m_iniPath);
	if(m_pDspCodeEdit)
		m_pDspCodeEdit->GetWindowText(m_dspCodePath);
	CDialog::OnOK();
}
