// SequenceScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include "SequenceStripMgr.h"
#include "SequenceScrollBar.h"
#include "SequenceMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSequenceScrollBar

CSequenceScrollBar::CSequenceScrollBar()
{
	m_pSequenceStrip = NULL;
}

CSequenceScrollBar::~CSequenceScrollBar()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pSequenceStrip = NULL;
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSequenceScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CSequenceScrollBar)
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_APP, OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceScrollBar message handlers

void CSequenceScrollBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CScrollBar::OnVScroll(nSBCode, nPos, pScrollBar);

	if (pScrollBar != this)
	{
		return;
	}

	if (m_pSequenceStrip)
	{
		m_pSequenceStrip->OnVScroll(nSBCode, nPos);
	}	
}

void CSequenceScrollBar::SetSequenceStrip(class CSequenceStrip* pSequenceStrip)
{
	ASSERT(pSequenceStrip != NULL);
	m_pSequenceStrip = pSequenceStrip;	
}

LRESULT CSequenceScrollBar::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pSequenceStrip )
	{
		return m_pSequenceStrip->OnApp( wParam, lParam );
	}
	else
	{
		return 0;
	}
}
