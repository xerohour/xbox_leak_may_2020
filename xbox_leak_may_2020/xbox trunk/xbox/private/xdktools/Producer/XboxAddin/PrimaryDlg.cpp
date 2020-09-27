// PrimaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "PrimaryDlg.h"
#include "Segment.h"
#include "XboxAddinComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrimaryDlg dialog

CPrimaryDlg::CPrimaryDlg(CWnd* pParent /*=NULL*/)
	: CSegmentDlg(pParent)
{
	//{{AFX_DATA_INIT(CPrimaryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_plstSegments = &theApp.m_pXboxAddinComponent->m_lstPrimarySegments;
}


void CPrimaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CSegmentDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrimaryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrimaryDlg, CSegmentDlg)
	//{{AFX_MSG_MAP(CPrimaryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrimaryDlg message handlers

BOOL CPrimaryDlg::OnInitDialog() 
{
	CSegmentDlg::OnInitDialog();

    m_staticSegment.SetWindowText( TEXT("Primary Segments") );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrimaryDlg::HandleSegmentNotification( CSegment *pSegment, DWORD dwNotification )
{
	CSegmentDlg::HandleSegmentNotification( pSegment, dwNotification );
	
	// TODO: Do more work to handle transition buttons
}