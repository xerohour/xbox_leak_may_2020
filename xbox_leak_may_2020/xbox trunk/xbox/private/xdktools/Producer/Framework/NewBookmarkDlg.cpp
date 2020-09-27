// NewBookmarkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "NewBookmarkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg dialog


CNewBookmarkDlg::CNewBookmarkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewBookmarkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewBookmarkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewBookmarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewBookmarkDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_BOOKMARK_NAME, m_editBookmarkName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewBookmarkDlg, CDialog)
	//{{AFX_MSG_MAP(CNewBookmarkDlg)
	ON_EN_CHANGE(IDC_BOOKMARK_NAME, OnChangeBookmarkName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg OnInitDialog

BOOL CNewBookmarkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_editBookmarkName.LimitText( MAX_LENGTH_BOOKMARK_NAME );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg OnInitDialog

void CNewBookmarkDlg::OnChangeBookmarkName() 
{
	CString strBookmarkName;

	m_editBookmarkName.GetWindowText( strBookmarkName );

	// Strip leading and trailing spaces
	strBookmarkName.TrimRight();
	strBookmarkName.TrimLeft();

	m_btnOK.EnableWindow( strBookmarkName.IsEmpty() == FALSE ); 
}


/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg OnOK

void CNewBookmarkDlg::OnOK() 
{
	m_editBookmarkName.GetWindowText( m_strName );

	// Strip leading and trailing spaces
	m_strName.TrimRight();
	m_strName.TrimLeft();

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( pMainFrame->m_wndBookmarkToolBar.BookmarkNameExists( m_strName ) )
	{
		CString strMsg;

		AfxFormatString1( strMsg, IDS_ERR_BOOKMARK_EXISTS, m_strName );
		AfxMessageBox( strMsg, MB_OK );
		m_editBookmarkName.SetWindowText( _T( "") );
		m_editBookmarkName.SetFocus();
		return;
	}

	CDialog::OnOK();
}
