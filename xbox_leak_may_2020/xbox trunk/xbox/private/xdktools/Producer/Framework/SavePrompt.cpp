// SavePrompt.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "SavePrompt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg implementation

/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::CSavePromptDlg

CSavePromptDlg::CSavePromptDlg( CString* pstrPrompt ) : CDialog(CSavePromptDlg::IDD)
{
	//{{AFX_DATA_INIT(CSavePromptDlg)
	//}}AFX_DATA_INIT

	m_strPrompt = *pstrPrompt;
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::DoDataExchange

void CSavePromptDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSavePromptDlg)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnInitDialog

BOOL CSavePromptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

//	CFont* pFont;
//	SetFont( pFont );
	
	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_PROMPT );
	if( pWnd )
	{
		pWnd->SetWindowText( m_strPrompt );
	}

	return TRUE;  
}


BEGIN_MESSAGE_MAP( CSavePromptDlg, CDialog )
	//{{AFX_MSG_MAP(CSavePromptDlg)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(ID_YESALL, OnYesAll)
	ON_BN_CLICKED(ID_NOALL, OnNoAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnNo

void CSavePromptDlg::OnNo() 
{
	EndDialog( IDNO );
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnNoAll

void CSavePromptDlg::OnNoAll() 
{
	EndDialog( ID_NOALL );
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnYes

void CSavePromptDlg::OnYes() 
{
	EndDialog( IDYES );
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnYesAll

void CSavePromptDlg::OnYesAll() 
{
	EndDialog( ID_YESALL );
}


/////////////////////////////////////////////////////////////////////////////
// CSavePromptDlg::OnDrawItem

void CSavePromptDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_ICON_EXCLAMATION:
			HICON hIcon = theApp.LoadStandardIcon( MAKEINTRESOURCE(IDI_EXCLAMATION) );
			::DrawIcon( lpDrawItemStruct->hDC, 0, 0, hIcon );
			return;
	}
	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}
