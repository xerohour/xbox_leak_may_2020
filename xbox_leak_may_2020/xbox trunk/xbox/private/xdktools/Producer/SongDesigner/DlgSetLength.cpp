// DlgSetLength.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "DlgSetLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgSetLength dialog

CDlgSetLength::CDlgSetLength( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetLength::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetLength)
	//}}AFX_DATA_INIT

	m_dwNbrMeasures = 0;
}


void CDlgSetLength::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetLength)
	DDX_Control(pDX, IDC_LENGTH, m_editLength);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LENGTH_SPIN, m_spinLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetLength, CDialog)
	//{{AFX_MSG_MAP(CDlgSetLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetLength message handlers


/////////////////////////////////////////////////////////////////////////////
// CDlgSetLength::OnInitDialog

BOOL CDlgSetLength::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set length
	m_editLength.LimitText( 3 );
	m_spinLength.SetRange( MIN_VIRTUAL_SEGMENT_LENGTH, MAX_VIRTUAL_SEGMENT_LENGTH );
	m_spinLength.SetPos( m_dwNbrMeasures );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDlgSetLength::OnOK

void CDlgSetLength::OnOK() 
{
	CString strNewNbrMeasures;
	m_editLength.GetWindowText( strNewNbrMeasures );

	// Strip leading and trailing spaces
	strNewNbrMeasures.TrimRight();
	strNewNbrMeasures.TrimLeft();

	DWORD dwNewNbrMeasures = (DWORD)( _ttoi( strNewNbrMeasures ) );

	// Validate new length
	if( dwNewNbrMeasures < MIN_VIRTUAL_SEGMENT_LENGTH
	||  dwNewNbrMeasures > MAX_VIRTUAL_SEGMENT_LENGTH )
	{
		CString strMsg;
		CString strMin;
		CString strMax;
		
		strMin.Format( "%d", MIN_VIRTUAL_SEGMENT_LENGTH );
		strMax.Format( "%d", MAX_VIRTUAL_SEGMENT_LENGTH );
		
		HINSTANCE hInstance = AfxGetResourceHandle();
		AfxSetResourceHandle( theApp.m_hInstance );
		AfxFormatString2( strMsg, IDS_ERR_MIN_MAX, strMin, strMax );
		AfxSetResourceHandle( hInstance );
		AfxMessageBox( strMsg );
		return;
	}

	m_dwNbrMeasures = dwNewNbrMeasures;

	CDialog::OnOK();
}
