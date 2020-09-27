// PianoRollScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include "midistripmgr.h"
#include "PianoRollScrollBar.h"
#include "MIDIMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPianoRollScrollBar

CPianoRollScrollBar::CPianoRollScrollBar()
{
	m_pPRS = NULL;
}

CPianoRollScrollBar::~CPianoRollScrollBar()
{
	m_pPRS = NULL;
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CPianoRollScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CPianoRollScrollBar)
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_APP, OnApp)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPianoRollScrollBar message handlers

void CPianoRollScrollBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pScrollBar != this)
	{
		return;
	}
	
	if (m_pPRS)
	{
		m_pPRS->OnVScrollFromScrollbar( nSBCode, nPos );
	}
}

void CPianoRollScrollBar::SetPianoRollStrip( CPianoRollStrip* pPRS )
{
	ASSERT( pPRS != NULL);
	m_pPRS = pPRS;
}

LRESULT CPianoRollScrollBar::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPRS )
	{
		return m_pPRS->OnApp( wParam, lParam );
	}
	else
	{
		return 0;
	}
}

void CPianoRollScrollBar::OnTimer(UINT nIDEvent) 
{
	UNREFERENCED_PARAMETER( nIDEvent );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPRS )
	{
		m_pPRS->OnVariationTimer();
	}
}
