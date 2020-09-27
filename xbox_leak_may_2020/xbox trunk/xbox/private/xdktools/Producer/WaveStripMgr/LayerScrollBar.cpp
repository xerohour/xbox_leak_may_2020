// LayerScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "WaveStripMgr.h"
#include "TrackMgr.h"
#include "LayerScrollBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerScrollBar

CLayerScrollBar::CLayerScrollBar()
{
	m_pWaveStrip = NULL;
}

CLayerScrollBar::~CLayerScrollBar()
{
	m_pWaveStrip = NULL;
	DestroyWindow();
}



/////////////////////////////////////////////////////////////////////////////
// CLayerScrollBar::SetWaveStrip

void CLayerScrollBar::SetWaveStrip( CWaveStrip* pWaveStrip )
{
	ASSERT( pWaveStrip != NULL );
	m_pWaveStrip = pWaveStrip;
}


BEGIN_MESSAGE_MAP(CLayerScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CLayerScrollBar)
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerScrollBar message handlers

/////////////////////////////////////////////////////////////////////////////
// CLayerScrollBar::OnVScroll

void CLayerScrollBar::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pScrollBar != this )
	{
		return;
	}
	
	if( m_pWaveStrip )
	{
		m_pWaveStrip->OnVScrollFromScrollbar( nSBCode, nPos );
	}
}

void CLayerScrollBar::OnTimer(UINT nIDEvent) 
{
	if( m_pWaveStrip )
	{
		m_pWaveStrip->OnVariationTimer();
	}
}
