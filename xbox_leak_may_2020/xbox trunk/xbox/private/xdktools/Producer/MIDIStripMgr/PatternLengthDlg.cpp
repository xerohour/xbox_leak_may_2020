// PatternLengthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Pattern.h"
#include "resource.h"
#include "MIDIStripMgrApp.h"
#include "PatternLengthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatternLengthDlg dialog


CPatternLengthDlg::CPatternLengthDlg( CDirectMusicPattern* pPattern, CWnd* pParent /*=NULL*/)
	: CDialog(CPatternLengthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatternLengthDlg)
	//}}AFX_DATA_INIT

	ASSERT( pPattern != NULL );

	m_pPattern = pPattern;
}


void CPatternLengthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternLengthDlg)
	DDX_Control(pDX, IDC_LENGTH, m_editLength);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LENGTH_SPIN, m_spinLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatternLengthDlg, CDialog)
	//{{AFX_MSG_MAP(CPatternLengthDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternLengthDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CPatternLengthDlg::OnInitDialog

BOOL CPatternLengthDlg::OnInitDialog() 
{
	ASSERT( m_pPattern != NULL );

	CDialog::OnInitDialog();

	// Set title
	CString strTitle;
	CString strContext;

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );
	if( m_pPattern->m_wEmbellishment & EMB_MOTIF )
	{
		strContext.LoadString( IDS_MOTIF_TEXT );
	}
	else
	{
		strContext.LoadString( IDS_PATTERN_TEXT );
	}
	AfxFormatString1( strTitle, IDS_SET_LENGTH, strContext );
	AfxSetResourceHandle( hInstance );
	SetWindowText( strTitle );

	// Set length
	m_editLength.LimitText( 3 );
	m_spinLength.SetRange( MIN_PATTERN_LENGTH, MAX_PATTERN_LENGTH );
	m_spinLength.SetPos( m_pPattern->m_wNbrMeasures );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CPatternLengthDlg::OnOK

void CPatternLengthDlg::OnOK() 
{
	ASSERT( m_pPattern != NULL );

	CString strNewNbrMeasures;

	m_editLength.GetWindowText( strNewNbrMeasures );

	// Strip leading and trailing spaces
	strNewNbrMeasures.TrimRight();
	strNewNbrMeasures.TrimLeft();

	WORD wNewNbrMeasures = (WORD)( _ttoi( strNewNbrMeasures ) );

	// Validate new length
	if( wNewNbrMeasures < MIN_PATTERN_LENGTH
	||  wNewNbrMeasures > MAX_PATTERN_LENGTH )
	{
		CString strMsg;
		CString strMin;
		CString strMax;
		
		strMin.Format( "%d", MIN_PATTERN_LENGTH );
		strMax.Format( "%d", MAX_PATTERN_LENGTH );
		
		HINSTANCE hInstance = AfxGetResourceHandle();
		AfxSetResourceHandle( theApp.m_hInstance );
		AfxFormatString2( strMsg, IDS_ERR_MIN_MAX, strMin, strMax );
		AfxSetResourceHandle( hInstance );
		AfxMessageBox( strMsg );
		return;
	}

	m_pPattern->SetNbrMeasures( wNewNbrMeasures );

	CDialog::OnOK();
}
