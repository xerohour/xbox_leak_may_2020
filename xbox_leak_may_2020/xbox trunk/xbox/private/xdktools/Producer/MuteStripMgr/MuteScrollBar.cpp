// MuteScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include "MuteStripMgr.h"
#include "MuteScrollBar.h"
#include "MuteMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMuteScrollBar

CMuteScrollBar::CMuteScrollBar()
{
	m_pMuteStrip = NULL;
}

CMuteScrollBar::~CMuteScrollBar()
{
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CMuteScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CMuteScrollBar)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMuteScrollBar message handlers

void CMuteScrollBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollBar::OnVScroll(nSBCode, nPos, pScrollBar);

	if (pScrollBar != this) {
		return;
	}

	if (m_pMuteStrip) {
		m_pMuteStrip->OnVScroll(nSBCode, nPos, pScrollBar);
	}	
}

void CMuteScrollBar::SetMuteStrip(class CMuteStrip* pMuteStrip)
{
	ASSERT(pMuteStrip != NULL);
	m_pMuteStrip = pMuteStrip;	
}