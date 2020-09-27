#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "WaveStripLayer.h"
#include "TrackMgr.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CWaveStripLayer Constructors/Destructor

CWaveStripLayer::CWaveStripLayer( CWaveStrip* pWaveStrip )
{
	ASSERT( pWaveStrip != NULL );
	m_pWaveStrip = pWaveStrip;

	m_fSelected = FALSE;
}

CWaveStripLayer::~CWaveStripLayer( void )
{
}
