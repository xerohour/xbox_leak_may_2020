// IllegalChord.cpp : implementation file
//
#include "stdafx.h"
#pragma warning(disable:4201)
#include "chordstripmgr.h"
#include "resource.h"
#include "IllegalChord.h"
#pragma warning(default:4201)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIllegalChord dialog


CIllegalChord::CIllegalChord(CWnd* pParent /*=NULL*/)
	: CDialog(CIllegalChord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIllegalChord)
	m_bDontWarn = FALSE;
	//}}AFX_DATA_INIT

}


void CIllegalChord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIllegalChord)
	DDX_Control(pDX, IDC_ILLEGALCHORD, m_infotext);
	DDX_Check(pDX, IDC_DONTWARN, m_bDontWarn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIllegalChord, CDialog)
	//{{AFX_MSG_MAP(CIllegalChord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIllegalChord message handlers

BOOL CIllegalChord::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CString s1,s2;

	s1.LoadString(IDS_ILLEGALCHORD2);
	s2.LoadString(IDS_DONTWARN);

	m_infotext.SetWindowText(s1);
	
	SetDlgItemText(IDC_DONTWARN, s2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
