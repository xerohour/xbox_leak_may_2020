// PatternDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "PatternCtl.h"
#include "PatternDlg.h"
#include "MIDIStripMgr.h"
#include <SegmentGUIDS.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg

IMPLEMENT_DYNCREATE(CPatternDlg, CFormView)

CPatternDlg::CPatternDlg()
	: CFormView(CPatternDlg::IDD)
{
	//{{AFX_DATA_INIT(CPatternDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pPatternCtrl = NULL;
	m_pITimelineCtl = NULL;
	m_pIMIDIStripMgr = NULL;
	m_punkMIDIStripMgr = NULL;
	m_pStyle = NULL;
	m_pActiveStripWhenWindowDeactivated = NULL;

	m_nTimerID = 0;
	m_fActive = false;
}

CPatternDlg::~CPatternDlg()
{
	ASSERT(m_pITimelineCtl == NULL);
	ASSERT(m_pIMIDIStripMgr == NULL);
	ASSERT(m_pStyle == NULL);
	ASSERT(m_pActiveStripWhenWindowDeactivated == NULL);
}

void CPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::EnableTimer

void CPatternDlg::EnableTimer( BOOL fEnable ) 
{
	if( fEnable )
	{
		if( m_nTimerID == 0 )
		{
			m_nTimerID = SetTimer( 1, 40, NULL );
		}
	}
	else
	{
		if( m_nTimerID )
		{
			KillTimer( m_nTimerID );
			m_nTimerID = 0;
		}
	}
}


BEGIN_MESSAGE_MAP(CPatternDlg, CFormView)
	//{{AFX_MSG_MAP(CPatternDlg)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_APP, OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternDlg diagnostics

#ifdef _DEBUG
void CPatternDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CPatternDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPatternDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::Create

BOOL CPatternDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	return CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::OnInitialUpdate

void CPatternDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( m_pITimelineCtl == NULL );
	ASSERT( m_pPatternCtrl != NULL );
	ASSERT( m_pPatternCtrl->m_pPattern != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CFormView::OnInitialUpdate();

	IUnknown* pIUnknown = NULL;

	CWnd* pWnd = GetDlgItem( IDC_TIMELINECTL );
	if( pWnd )
	{
		pIUnknown = pWnd->GetControlUnknown();
		if( pIUnknown )
		{
			pIUnknown->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pITimelineCtl );
		}
	}

	if( m_pITimelineCtl )
	{
		// Hand Timeline pointer to IDMUSProdFramework
		VARIANT vtInit;
		vtInit.vt = VT_UNKNOWN;
		V_UNKNOWN(&vtInit) = theApp.m_pStyleComponent->m_pIFramework;
		m_pITimelineCtl->SetTimelineProperty( TP_DMUSPRODFRAMEWORK, vtInit );

		// Set Timeline time signature
		DMUS_TIMESIGNATURE dmusTimeSig;

		dmusTimeSig.mtTime = 0;
		dmusTimeSig.bBeatsPerMeasure = m_pPatternCtrl->m_pPattern->m_TimeSignature.m_bBeatsPerMeasure;
		dmusTimeSig.bBeat = m_pPatternCtrl->m_pPattern->m_TimeSignature.m_bBeat;			
		dmusTimeSig.wGridsPerBeat = m_pPatternCtrl->m_pPattern->m_TimeSignature.m_wGridsPerBeat;	
		m_pITimelineCtl->SetParam( GUID_TimeSignature, 1, 0, 0, &dmusTimeSig );

		// Set Timeline length
		vtInit.vt = VT_I4;
		V_I4(&vtInit) = m_pPatternCtrl->m_pPattern->m_dwLength;
		m_pITimelineCtl->SetTimelineProperty( TP_CLOCKLENGTH, vtInit );

		// Set Timeline tempo
		m_pPatternCtrl->m_pPattern->SyncTempo();

		// Set the Timeline's callback member so we get callbacks when data changes
		if( SUCCEEDED( m_pPatternCtrl->m_pPattern->QueryInterface( IID_IDMUSProdTimelineCallback, (void**)&pIUnknown)) )
		{
			vtInit.vt = VT_UNKNOWN;
			V_UNKNOWN(&vtInit) = pIUnknown;
			m_pITimelineCtl->SetTimelineProperty( TP_TIMELINECALLBACK, vtInit );
			RELEASE( pIUnknown );
		}			

		// Resize ourself so the Timeline is the correct size when the pattern is loaded
		// BUGBUG: This is a major hack, but it works.  The real fix should be in the
		// Framework's CComponentDoc::OpenEditorWindow method.
		RECT rectTmp;
		::GetWindowRect( ::GetParent( ::GetParent( ::GetParent( GetSafeHwnd() ))), &rectTmp );
		// BUGBUG: 4 is a majic number - it is unrelated to the border width (tested with width of 10)
		m_pPatternCtrl->OnSize( SIZE_RESTORED, rectTmp.right - rectTmp.left - 4, rectTmp.bottom - rectTmp.top - 4 );

		// Initialize the MIDIStrip Manager
		if( SUCCEEDED( CoCreateInstance( CLSID_MIDIMgr, NULL, CLSCTX_INPROC_SERVER,
										 IID_IDMUSProdStripMgr, (void**)&m_pIMIDIStripMgr )))
		{
			// Hand MIDIStripMgr pointer to Style's IDMUSProdNode interface
			if( m_pIMIDIStripMgr->IsParamSupported( GUID_StyleNode ) == S_OK )
			{
				if( FAILED( m_pIMIDIStripMgr->SetParam( GUID_StyleNode, 0, m_pStyle ) ) )
				{
					TRACE("Pattern: Failed to set MIDIStripMgr's Style node pointer\n");
				}
			}

			// Add MIDIStripMgr to the Timeline
			m_pITimelineCtl->AddStripMgr( m_pIMIDIStripMgr, 1 );

			// Store IUnknown* for MIDIStrip Manager
			m_pIMIDIStripMgr->QueryInterface( IID_IUnknown, (void**)&m_punkMIDIStripMgr );

			// Persist this pattern's data to the manager
			// Piano Roll strips will be created for each PartRef using the Design-time data if
			// available.  If not available, they will be created with default settings.
			IPersistStream* pIPS;
			if( SUCCEEDED( m_pIMIDIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPS )))
			{
				IStream* pIStream;

				if( SUCCEEDED( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_SinglePattern, &pIStream )))
				{
					m_pPatternCtrl->m_pPattern->m_fIgnoreTimelineSync = true;
					if( SUCCEEDED( m_pPatternCtrl->m_pPattern->Save( pIStream, FALSE )))
					{
						StreamSeek( pIStream, 0, STREAM_SEEK_SET ); //seek to beginning
						pIPS->Load( pIStream );
					}
					m_pPatternCtrl->m_pPattern->m_fIgnoreTimelineSync = false;

					RELEASE( pIStream );
				}

				RELEASE( pIPS );
			}

			// Enable the record button
			DWORD dwButtonState = BS_AUTO_UPDATE | BS_RECORD_ENABLED;
			if( m_pPatternCtrl->m_pPattern->m_fRecordPressed )
			{
				dwButtonState |= BS_RECORD_CHECKED;
			}
			theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( m_pPatternCtrl->m_pPattern, dwButtonState );	

			// If Record is pressed, tell the MIDIMgr to enable MIDI input.
			if ( m_pPatternCtrl->m_pPattern->m_fRecordPressed )
			{
				IMIDIMgr* pIMIDIMgr;
				if( SUCCEEDED( m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**)&pIMIDIMgr ) ) )
				{
					pIMIDIMgr->OnRecord( TRUE );
					pIMIDIMgr->Release();
				}
			}

			// If currently playing, start our timer and tell the MIDIMgr that music is playing.
			if ( m_pPatternCtrl->m_pPattern->m_fPatternIsPlaying )
			{
				EnableTimer( m_pPatternCtrl->m_pPattern->m_fTrackTimeCursor );
				IMIDIMgr* pIMIDIMgr;
				if( SUCCEEDED( m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**)&pIMIDIMgr ) ) )
				{
					pIMIDIMgr->SetSegmentState( m_pPatternCtrl->m_pPattern->m_pIDMSegmentState );
					pIMIDIMgr->Release();
				}

				// If we're playing, tell the Timeline
				m_pITimelineCtl->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pPatternCtrl->m_pPattern->m_rtCurrentStartTime );
			}
		}

		// Update the horizontal zoom, horizontal scroll, and vertical scroll
		UpdateZoomAndScroll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::OnDestroy

void CPatternDlg::OnDestroy() 
{
	EnableTimer( FALSE );

	// Remove our reference to the active strip
	RELEASE(m_pActiveStripWhenWindowDeactivated);

	if( m_pITimelineCtl && m_pPatternCtrl && m_pPatternCtrl->m_pPattern )
	{
		VARIANT var;
		if( SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_ZOOM, &var ) ) )
		{
			if( V_R8(&var) != m_pPatternCtrl->m_pPattern->m_dblZoom )
			{
				m_pPatternCtrl->m_pPattern->m_dblZoom = V_R8(&var);
				m_pPatternCtrl->m_pPattern->SetModified( TRUE );
			}
		}

		if( (m_pPatternCtrl->m_pPattern->m_dblZoom > 0.0)
		&&	SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			if( (V_I4(&var) / m_pPatternCtrl->m_pPattern->m_dblZoom) != m_pPatternCtrl->m_pPattern->m_dblHorizontalScroll )
			{
				m_pPatternCtrl->m_pPattern->m_dblHorizontalScroll = V_I4(&var) / m_pPatternCtrl->m_pPattern->m_dblZoom;
				m_pPatternCtrl->m_pPattern->SetModified( TRUE );
			}
		}

		if( SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_VERTICAL_SCROLL, &var ) ) )
		{
			if( V_I4(&var) != m_pPatternCtrl->m_pPattern->m_lVerticalScroll )
			{
				m_pPatternCtrl->m_pPattern->m_lVerticalScroll = V_I4(&var);
				m_pPatternCtrl->m_pPattern->SetModified( TRUE );
			}
		}

		if( SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
		{
			if( V_I4(&var) != m_pPatternCtrl->m_pPattern->m_tlSnapTo )
			{
				m_pPatternCtrl->m_pPattern->m_tlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO)V_I4(&var);
				m_pPatternCtrl->m_pPattern->SetModified( TRUE );
			}
		}

		if( SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
		{
			if( V_I4(&var) != m_pPatternCtrl->m_pPattern->m_lFunctionbarWidth )
			{
				m_pPatternCtrl->m_pPattern->m_lFunctionbarWidth = V_I4(&var);
				m_pPatternCtrl->m_pPattern->SetModified( TRUE );
			}
		}

		// If we're playing, tell the Timeline we stopped
		if( m_pPatternCtrl->m_pPattern->m_rtCurrentStartTime != 0 )
		{
			REFERENCE_TIME rtTime = 0;
			m_pITimelineCtl->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &rtTime );
		}
	}

	RELEASE( m_pITimelineCtl );
	RELEASE( m_pIMIDIStripMgr );
	RELEASE( m_punkMIDIStripMgr );

	m_pStyle = NULL;

	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::OnSize

void CPatternDlg::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CFormView::OnSize( nType, cx, cy );

	CWnd *pWndTimeline = GetDlgItem( IDC_TIMELINECTL );
	if( pWndTimeline )
	{
		pWndTimeline->MoveWindow( 0, 0, cx, cy, FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::OnEraseBkgnd

BOOL CPatternDlg::OnEraseBkgnd( CDC* pDC ) 
{
	UNREFERENCED_PARAMETER(pDC);

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::OnTimer

void CPatternDlg::OnTimer( UINT nIDEvent ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pITimelineCtl != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );
	ASSERT( m_pPatternCtrl != NULL );
	ASSERT( m_pPatternCtrl->m_pPattern != NULL );
	
	IDirectMusicSegmentState* pIDMSegmentState = m_pPatternCtrl->m_pPattern->m_pIDMSegmentState ? m_pPatternCtrl->m_pPattern->m_pIDMSegmentState : m_pPatternCtrl->m_pPattern->m_rpIDMStoppedSegmentState;
	if( m_pITimelineCtl && pIDMSegmentState )
	{
		MUSIC_TIME mtTimeNow;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow ) ) )
		{
			MUSIC_TIME mtStartTime;
			MUSIC_TIME mtOffset;
			MUSIC_TIME mtStartPoint;

			if( SUCCEEDED ( pIDMSegmentState->GetStartTime( &mtStartTime ) ) && 
				SUCCEEDED ( pIDMSegmentState->GetStartPoint( &mtStartPoint ) ) )
			{
				mtOffset = mtTimeNow - mtStartTime + mtStartPoint;
				if( mtOffset >=0 )
				{
					mtOffset = mtOffset % m_pPatternCtrl->m_pPattern->m_dwLength;
					m_pITimelineCtl->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, mtOffset );
				}
			}
		}

		return;
	}
	
	CFormView::OnTimer(nIDEvent);
}

LRESULT CPatternDlg::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pPatternCtrl->m_pPattern == NULL )
	{
		return 0;
	}

	// wParam = dwNotificationOption
	// lParam = mtTime (for start/loop), mtOffset (for abort/end)
	switch( wParam )
	{
	case DMUS_NOTIFICATION_SEGSTART:
		// If the timeline exists, notify it that we started playing
		if( m_pITimelineCtl )
		{
			m_pITimelineCtl->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pPatternCtrl->m_pPattern->m_rtCurrentStartTime );
		}
	case DMUS_NOTIFICATION_SEGLOOP:
		// Make sure cursor is moving
		EnableTimer( m_pPatternCtrl->m_pPattern->m_fTrackTimeCursor );

		// Notify the MIDIMgr that the music started
		if( m_punkMIDIStripMgr )
		{
			IMIDIMgr* pIMIDIMgr;
			if( SUCCEEDED ( m_punkMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
			{
				pIMIDIMgr->SetSegmentState( m_pPatternCtrl->m_pPattern->m_pIDMSegmentState );
				pIMIDIMgr->Release();
			}
		}
		break;
	case DMUS_NOTIFICATION_SEGABORT:
	case DMUS_NOTIFICATION_SEGEND:
		// Make sure cursor stops moving
		EnableTimer( FALSE );

		// Notify the MIDIMgr that the music stopped
		if( m_punkMIDIStripMgr )
		{
			IMIDIMgr* pIMIDIMgr;
			if( SUCCEEDED ( m_punkMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
			{
				pIMIDIMgr->SetSegmentState( NULL );
				pIMIDIMgr->Release();
			}
		}

		// If the timeline exists, notify it that we stopped playing
		if( m_pITimelineCtl )
		{
			if( lParam >= 0 )
			{
				m_pITimelineCtl->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lParam );
			}

			ASSERT( m_pPatternCtrl->m_pPattern->m_rtCurrentStartTime == 0 );
			m_pITimelineCtl->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pPatternCtrl->m_pPattern->m_rtCurrentStartTime );
		}
		break;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::Activate

void CPatternDlg::Activate( BOOL fActive )
{
	if( m_fActive != fActive )
	{
		m_fActive = fActive;

		if( m_pITimelineCtl )
		{
			if( !m_fActive )
			{
				// Initialize the strip reference pointer to NULL
				m_pActiveStripWhenWindowDeactivated = NULL;

				// Determine which strip (if any) is active
				VARIANT var;
				if( SUCCEEDED( m_pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var ) )
				&&	(V_UNKNOWN(&var) != NULL) )
				{
					IDMUSProdStrip *pIStrip;
					if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdStrip, (void **) &pIStrip ) ) )
					{
						// Keep a copy of the strip
						m_pActiveStripWhenWindowDeactivated = pIStrip;
					}
					V_UNKNOWN(&var)->Release();

					// Deactivate the active strip
					V_UNKNOWN(&var) = NULL;
					m_pITimelineCtl->SetTimelineProperty( TP_ACTIVESTRIP, var );
				}
			}
			else if( m_pActiveStripWhenWindowDeactivated )
			{
				// Activate the strip that was active when we our window was deactivated
				VARIANT var;
				var.vt = VT_UNKNOWN;
				V_UNKNOWN(&var) = m_pActiveStripWhenWindowDeactivated;
				V_UNKNOWN(&var)->AddRef();
				m_pITimelineCtl->SetTimelineProperty( TP_ACTIVESTRIP, var );

				m_pActiveStripWhenWindowDeactivated->Release();
				m_pActiveStripWhenWindowDeactivated = NULL;
			}
		}

		if( m_pIMIDIStripMgr )
		{
			IMIDIMgr *pIMIDIMgr;
			if( SUCCEEDED( m_pIMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void **)&pIMIDIMgr ) ) )
			{
				pIMIDIMgr->Activate( m_fActive );
				pIMIDIMgr->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatternDlg::UpdateZoomAndScroll

void CPatternDlg::UpdateZoomAndScroll( void )
{
	ASSERT( m_pPatternCtrl && m_pPatternCtrl->m_pPattern );
	if( m_pPatternCtrl && m_pPatternCtrl->m_pPattern && m_pITimelineCtl )
	{
		VARIANT vtInit;
		if( m_pPatternCtrl->m_pPattern->m_dblZoom == 0.0 )
		{
			m_pPatternCtrl->m_pPattern->m_dblZoom = 0.125;
		}
		V_R8(&vtInit) = m_pPatternCtrl->m_pPattern->m_dblZoom;
		vtInit.vt = VT_R8;
		m_pITimelineCtl->SetTimelineProperty( TP_ZOOM, vtInit );

		V_I4(&vtInit) = m_pPatternCtrl->m_pPattern->m_lVerticalScroll;
		vtInit.vt = VT_I4;
		m_pITimelineCtl->SetTimelineProperty( TP_VERTICAL_SCROLL, vtInit );

		V_I4(&vtInit) = long(m_pPatternCtrl->m_pPattern->m_dblHorizontalScroll * m_pPatternCtrl->m_pPattern->m_dblZoom);
		vtInit.vt = VT_I4;
		m_pITimelineCtl->SetTimelineProperty( TP_HORIZONTAL_SCROLL, vtInit );

		V_I4(&vtInit) = m_pPatternCtrl->m_pPattern->m_tlSnapTo;
		vtInit.vt = VT_I4;
		m_pITimelineCtl->SetTimelineProperty( TP_SNAP_TO, vtInit );

		V_I4(&vtInit) = m_pPatternCtrl->m_pPattern->m_lFunctionbarWidth;
		vtInit.vt = VT_I4;
		m_pITimelineCtl->SetTimelineProperty( TP_FUNCTIONBAR_WIDTH, vtInit );
	}
}
