// SecondaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "SecondaryDlg.h"
#include "Segment.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )
#include "XboxAddinComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDlg dialog

CSecondaryDlg::CSecondaryDlg(CWnd* pParent /*=NULL*/)
	: CSegmentDlg(pParent)
{
	//{{AFX_DATA_INIT(CSecondaryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_plstSegments = &theApp.m_pXboxAddinComponent->m_lstSecondarySegments;
}


void CSecondaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CSegmentDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecondaryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSecondaryDlg, CSegmentDlg)
	//{{AFX_MSG_MAP(CSecondaryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDlg message handlers

BOOL CSecondaryDlg::OnInitDialog() 
{
	CSegmentDlg::OnInitDialog();

    m_staticSegment.SetWindowText( TEXT("Secondary Segments") );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT CSecondaryDlg::AddSegmentToList( CSegment *pSegment )
{
	if( pSegment )
	{
		pSegment->m_dwPlayFlags |= DMUS_SEGF_SECONDARY;
	}

	return CSegmentDlg::AddSegmentToList( pSegment );
}
