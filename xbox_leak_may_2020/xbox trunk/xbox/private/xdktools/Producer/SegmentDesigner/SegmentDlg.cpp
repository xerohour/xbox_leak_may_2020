// SegmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDLL.h"
#include "SegmentDlg.h"
#include "SegmentPPGmgr.h"
#include "Track.h"
#include <dmusici.h>
#include <SegmentGuids.h>
#include "SegmentLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MUSIC_TIME GetTimeOffset( const MUSIC_TIME mtNow, const MUSIC_TIME mtCurrentStartTime, const MUSIC_TIME mtCurrentStartPoint,
						  const MUSIC_TIME mtCurrentLoopStart, const MUSIC_TIME mtCurrentLoopEnd, const MUSIC_TIME mtLength,
						  const DWORD dwCurrentMaxLoopRepeats)
{
	// Convert mtNow from absolute time to an offset from when the segment started playing
	LONGLONG llBigNow = mtNow - (mtCurrentStartTime - mtCurrentStartPoint);

	// If mtLoopEnd is non zero, set lLoopEnd to mtLoopEnd, otherwise use the segment length
	LONGLONG llLoopEnd = mtCurrentLoopEnd ? mtCurrentLoopEnd : mtLength;

	LONGLONG llLoopStart = mtCurrentLoopStart;

	if( (dwCurrentMaxLoopRepeats != 0)
	&&	(llLoopStart < llLoopEnd)
	&&	(llLoopEnd > mtCurrentStartPoint) )
	{
		if( (dwCurrentMaxLoopRepeats != DMUS_SEG_REPEAT_INFINITE)
		&&	(llBigNow > (llLoopStart + (llLoopEnd - llLoopStart) * (signed)dwCurrentMaxLoopRepeats)) )
		{
			llBigNow -= (llLoopEnd - llLoopStart) * dwCurrentMaxLoopRepeats;
		}
		else if( llBigNow > llLoopStart )
		{
			llBigNow = llLoopStart + (llBigNow - llLoopStart) % (llLoopEnd - llLoopStart);
		}
	}

	llBigNow = min( llBigNow, LONG_MAX );
	return long(llBigNow);
}

REFERENCE_TIME GetTimeOffset( const REFERENCE_TIME rtNow, 
                          const REFERENCE_TIME rtCurrentStartTime, 
                          const REFERENCE_TIME rtCurrentStartPoint,
						  const REFERENCE_TIME rtCurrentLoopStart, 
                          const REFERENCE_TIME rtCurrentLoopEnd, 
                          const REFERENCE_TIME rtLength,
						  const DWORD dwCurrentMaxLoopRepeats )
{
	// Convert rtNow from absolute time to an offset from when the segment started playing
	LONGLONG llBigNow = rtNow - (rtCurrentStartTime - rtCurrentStartPoint);

	// If rtLoopEnd is non zero, set lLoopEnd to rtLoopEnd, otherwise use the segment length
	LONGLONG llLoopEnd = rtCurrentLoopEnd ? rtCurrentLoopEnd : rtLength;

	LONGLONG llLoopStart = rtCurrentLoopStart;

	if( (dwCurrentMaxLoopRepeats != 0)
	&&	(llLoopStart < llLoopEnd)
	&&	(llLoopEnd > rtCurrentStartPoint) )
	{
		if( (dwCurrentMaxLoopRepeats != DMUS_SEG_REPEAT_INFINITE)
		&&	(llBigNow > (llLoopStart + (llLoopEnd - llLoopStart) * (signed)dwCurrentMaxLoopRepeats)) )
		{
			llBigNow -= (llLoopEnd - llLoopStart) * dwCurrentMaxLoopRepeats;
		}
		else if( llBigNow > llLoopStart )
		{
			llBigNow = llLoopStart + (llBigNow - llLoopStart) % (llLoopEnd - llLoopStart);
		}
	}

	llBigNow = min( llBigNow, LONG_MAX );

	return long(llBigNow);
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg dialog

CSegmentDlg::CSegmentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSegmentDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSegmentCtrl		= NULL;
	m_pTimeline			= NULL;
	m_fActive			= FALSE;
	m_pITransport		= NULL;
	m_pSegment			= NULL;
	m_nTimerID			= 0;
	m_pActiveStripWhenWindowDeactivated = NULL;
}

CSegmentDlg::~CSegmentDlg()
{
	ASSERT(m_pTimeline == NULL);
	ASSERT(m_pSegment == NULL);
	ASSERT(m_pITransport == NULL);
	ASSERT(m_pActiveStripWhenWindowDeactivated == NULL);
}

void CSegmentDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSegmentDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSegmentDlg, CDialog)
	//{{AFX_MSG_MAP(CSegmentDlg)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_APP, OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg message handlers

BOOL CSegmentDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	UNREFERENCED_PARAMETER(lpszClassName);
	UNREFERENCED_PARAMETER(lpszWindowName);
	UNREFERENCED_PARAMETER(dwStyle);
	UNREFERENCED_PARAMETER(rect);
	UNREFERENCED_PARAMETER(nID);
	UNREFERENCED_PARAMETER(pContext);
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CSegmentDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CDialog::OnInitDialog();
	
	CWnd		*pWnd;
	IUnknown	*pUnk = NULL;

	ASSERT(m_pTimeline == NULL);
	ASSERT(m_pSegmentCtrl);
	ASSERT(m_pSegmentCtrl->m_pSegment);
	ASSERT(m_pSegmentCtrl->m_pSegment->m_pComponent);
	
	m_pSegment = m_pSegmentCtrl->m_pSegment;
	m_pSegment->AddRef();

	pWnd = GetDlgItem(IDC_TIMELINECTL1);
	if(pWnd)
	{
		pUnk = pWnd->GetControlUnknown();
		if(pUnk)
		{
			pUnk->QueryInterface(IID_IDMUSProdTimeline, (void **)&m_pTimeline);
		}
	}

	if( !m_pTimeline )
	{
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	{
		LPUNKNOWN punk;

		if(SUCCEEDED(m_pSegmentCtrl->m_pSegment->QueryInterface(IID_IUnknown, (void **) &punk)))
		{
			HRESULT hr;
			VARIANT var;
			var.vt = VT_UNKNOWN;
			V_UNKNOWN(&var) = punk;
			hr = m_pTimeline->SetTimelineProperty(TP_TIMELINECALLBACK, var);
			ASSERT(SUCCEEDED(hr));
			punk->Release();
		}

		// set the framework pointer in the timeline control
		ASSERT(m_pSegment->m_pComponent);
		VARIANT vtInit;
		V_UNKNOWN(&vtInit) = m_pSegment->m_pComponent->m_pIFramework;
		vtInit.vt = VT_UNKNOWN;
		m_pTimeline->SetTimelineProperty( TP_DMUSPRODFRAMEWORK, vtInit );
	
		// Initialize the Timeline with the Segment length.
		SetTimelineLength( m_pSegment->m_mtLength );

		// Initialize the default tempo
		DMUS_TEMPO_PARAM tempo;
		tempo.mtTime = 0;
		tempo.dblTempo = 120.0;
		m_pTimeline->SetParam(GUID_TempoParam, 0xffffffff, 0, 0, &tempo );

		// Resize ourself so the Timeline is the correct size when the pattern is loaded
		// BUGBUG: This is a major hack, but it works.  The real fix should be in the
		// Framework's CComponentDoc::OpenEditorWindow method.
		RECT rectTmp;
		::GetWindowRect( ::GetParent( ::GetParent( ::GetParent( GetSafeHwnd() ))), &rectTmp );
		m_pSegmentCtrl->OnSize( SIZE_RESTORED, rectTmp.right - rectTmp.left - 4, rectTmp.bottom - rectTmp.top - 4 );

		// Add the Proxy StripMgr to the Timeline
		m_pTimeline->AddStripMgr( &m_pSegment->m_ProxyStripMgr, 0xFFFFFFFF );

		// Add the other StripMgrs to the Timeline
		CTrack* pTrack;
		POSITION position;
		position = m_pSegment->m_lstTracks.GetHeadPosition();
		while( position )
		{
			pTrack = m_pSegment->m_lstTracks.GetNext(position);
			ASSERT( pTrack );

			if( FAILED( AddTrack( pTrack ) ) )
			{
				TRACE("CSegmentDlg::OnInitDialog failed to add track.\n");
				// BUGBUG: Display message box saying unable to create track?
			}
		}

		// Tell the timeline to not display the Time Signature
		BOOL fFlag = FALSE;
		m_pTimeline->SetParam( GUID_TimelineShowTimeSig, 0xFFFFFFFF, 0, 0, (void *)&fFlag );

		// If we're playing, tell the Timeline
		if( m_pSegment->m_rtCurrentStartTime != 0 )
		{
			m_pTimeline->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pSegment->m_rtCurrentStartTime );
		}

		// Update the horizontal zoom, horizontal scroll, and vertical scroll
		UpdateZoomAndScroll();

		// This is automatically handled by CSegmentCtl::OnCreate, which
		// is called AFTER this method.
		//fShowTimeSig = TRUE;
		//m_pTimeline->NotifyStripMgrs( GUID_Segment_WindowActive, 0xFFFFFFFF, &fShowTimeSig );

		if( m_pSegment->m_fRecordPressed )
		{
			fFlag = m_pSegment->m_fRecordPressed;
			m_pTimeline->NotifyStripMgrs( GUID_Segment_RecordButton, 0xFFFFFFFF, &fFlag );
		}
	}

	m_pSegment->SetDialog(this);

	// Notify the StripMgrs that all tracks have been added, so they can now
	// safely initialize.
	m_pTimeline->NotifyStripMgrs( GUID_Segment_AllTracksAdded, 0xFFFFFFFF, NULL );

	// If the segment is empty, or only contains timesig tracks,
	// open up the add new tracks dialog
	bool fOpenAddNewTracksDialog = true;
	POSITION pos = m_pSegment->m_lstTracks.GetHeadPosition();
	while( pos )
	{
		if( CLSID_DirectMusicTimeSigTrack != m_pSegment->m_lstTracks.GetNext( pos )->m_guidClassID )
		{
			fOpenAddNewTracksDialog = false;
			break;
		}
	}

	if( m_pSegment->m_fBrandNew )
	{
		m_pSegment->m_fBrandNew = false;

		CSegmentLength dlg;
		dlg.m_dwNbrExtraBars = m_pSegment->m_PPGSegment.dwExtraBars;
		dlg.m_fPickupBar = m_pSegment->m_PPGSegment.fPickupBar;
		dlg.m_fClockTime = m_pSegment->m_dwSegmentFlags & DMUS_SEGIOF_REFLENGTH;
		dlg.m_pTimeline = m_pTimeline;
		dlg.m_dwGroupBits = m_pSegment->m_PPGSegment.dwTrackGroup;

		if( dlg.m_fClockTime )
		{
			dlg.m_rtLength = m_pSegment->m_rtLength;
		}
		else
		{
			// If not in reference time, convert
			m_pTimeline->ClocksToRefTime( m_pSegment->m_mtLength, &dlg.m_rtLength );
		}

		LONG lMeasure, lBeat;
		if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( dlg.m_dwGroupBits, 0, m_pSegment->m_mtLength, &lMeasure, NULL ) ) )
		{
			if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( dlg.m_dwGroupBits, 0, lMeasure, 0, &lBeat ) ) )
			{
				if( lBeat < m_pSegment->m_mtLength )
				{
					lMeasure++;
				}
			}

			// Ensure the segment is at least one measure long
			dlg.m_dwLength = max( 1, lMeasure );
		}

		const MUSIC_TIME mtOldLength = m_pSegment->m_mtLength;

		if(dlg.DoModal() == IDOK)
		{
			m_pSegment->m_PPGSegment.dwExtraBars = dlg.m_dwNbrExtraBars;
			m_pSegment->m_PPGSegment.fPickupBar = dlg.m_fPickupBar;

			if( dlg.m_fClockTime )
			{
				m_pSegment->m_PPGSegment.dwSegmentFlags |= DMUS_SEGIOF_REFLENGTH;
				m_pSegment->m_dwSegmentFlags |= DMUS_SEGIOF_REFLENGTH;
			}
			else
			{
				m_pSegment->m_PPGSegment.dwSegmentFlags &= ~DMUS_SEGIOF_REFLENGTH;
				m_pSegment->m_dwSegmentFlags &= ~DMUS_SEGIOF_REFLENGTH;
			}

			// Setting reference time length
			if( dlg.m_fClockTime )
			{
				m_pTimeline->RefTimeToClocks( dlg.m_rtLength, &m_pSegment->m_mtLength );

				m_pSegment->m_PPGSegment.rtSegmentLength = dlg.m_rtLength;
				m_pSegment->m_rtLength = dlg.m_rtLength;
			}
			// Setting musictime length
			else
			{
				m_pTimeline->MeasureBeatToClocks( m_pSegment->m_PPGSegment.dwTrackGroup, 0, dlg.m_dwLength, 0, &m_pSegment->m_mtLength );
				m_pTimeline->ClocksToRefTime( m_pSegment->m_mtLength, &m_pSegment->m_rtLength );
				m_pSegment->m_PPGSegment.rtSegmentLength = m_pSegment->m_rtLength;
			}

			// Ensure the segment is at least one measure long
			m_pTimeline->ClocksToMeasureBeat( m_pSegment->m_PPGSegment.dwTrackGroup, 0, m_pSegment->m_mtLength, (long *)(&m_pSegment->m_PPGSegment.dwMeasureLength), NULL );
			m_pSegment->m_PPGSegment.dwMeasureLength = max( 1, m_pSegment->m_PPGSegment.dwMeasureLength );

			// Update m_pIDMSegment and the Timeline
			m_pSegment->OnLengthChanged( mtOldLength );
		}
	}

	if( fOpenAddNewTracksDialog )
	{
		::PostMessage( GetSafeHwnd(), WM_COMMAND, ID_EDIT_ADD_TRACK, 0 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSegmentDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remove our reference to the active strip
	RELEASE(m_pActiveStripWhenWindowDeactivated);

	// Remove the Proxy StripMgr from the Timeline
	m_pTimeline->RemoveStripMgr( &m_pSegment->m_ProxyStripMgr );

	if(m_pTimeline && m_pSegment)
	{
		VARIANT var;
		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_ZOOM, &var ) ) )
		{
			if( V_R8(&var) != m_pSegment->m_dblZoom )
			{
				m_pSegment->m_dblZoom = V_R8(&var);
				m_pSegment->SetModifiedFlag( TRUE );
			}
		}

		if( (m_pSegment->m_dblZoom > 0.0)
		&&	SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			if( V_I4(&var) != m_pSegment->m_dblHorizontalScroll )
			{
				m_pSegment->m_dblHorizontalScroll = V_I4(&var) / m_pSegment->m_dblZoom;
				m_pSegment->SetModifiedFlag( TRUE );
			}
		}

		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var ) ) )
		{
			if( V_I4(&var) != m_pSegment->m_lVerticalScroll )
			{
				m_pSegment->m_lVerticalScroll = V_I4(&var);
				m_pSegment->SetModifiedFlag( TRUE );
			}
		}

		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
		{
			if( V_I4(&var) != m_pSegment->m_tlSnapTo )
			{
				m_pSegment->m_tlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO)V_I4(&var);
				m_pSegment->SetModifiedFlag( TRUE );
			}
		}

		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
		{
			if( V_I4(&var) != m_pSegment->m_lFunctionbarWidth )
			{
				m_pSegment->m_lFunctionbarWidth = V_I4(&var);
				m_pSegment->SetModifiedFlag( TRUE );
			}
		}

		m_pTimeline->NotifyStripMgrs( GUID_Segment_WindowClosing, 0xFFFFFFFF, NULL );

		// If we're playing, tell the Timeline we stopped
		if( m_pSegment->m_rtCurrentStartTime != 0 )
		{
			REFERENCE_TIME rtTime = 0;
			m_pTimeline->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &rtTime );
		}

		// Release the Timeline
		m_pTimeline->Release();
		m_pTimeline = NULL;
	}

	if(m_pSegment)
	{
		m_pSegment->RemoveDialog();
		m_pSegment->Release();
		m_pSegment = NULL;
	}

	if(m_pITransport)
	{
		m_pITransport->Release();
		m_pITransport = NULL;
	}

	if( m_nTimerID )
	{
		KillTimer( m_nTimerID );
		m_nTimerID = 0;
	}

	CDialog::OnDestroy();
}

void CSegmentDlg::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pSegmentCtrl);

	CDialog::OnSize(nType, cx, cy);
	
	CWnd *pTimeline = GetDlgItem(IDC_TIMELINECTL1);
	if(pTimeline)
	{
		pTimeline->MoveWindow(0, 0, cx, cy, TRUE);
	}
}

BOOL CSegmentDlg::OnEraseBkgnd( CDC* pDC ) 
{
	UNREFERENCED_PARAMETER(pDC);
	return FALSE;
}

void CSegmentDlg::OnTimer(UINT nIDEvent) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(nIDEvent);

	/*
	ASSERT(m_pSegment);
	ASSERT(m_pSegment->m_pComponent);
	ASSERT(m_pSegment->m_pComponent->m_pIFramework);
	ASSERT(m_pSegment->m_pComponent->m_pIDMPerformance);
	CString strTime;
	MUSIC_TIME mtNow;
	REFERENCE_TIME rtNow;
	BSTR bstrText;
	m_pSegment->m_pComponent->m_pIDMPerformance->GetTime( &rtNow, &mtNow );
	strTime.Format("Music: %ld",mtNow);
	bstrText = strTime.AllocSysString();
	m_pSegment->m_pComponent->m_pIFramework->SetStatusBarPaneText( 0, bstrText, TRUE );
	char ac[100];
	sprintf( ac, "Real: %I64d", rtNow );
	strTime = ac;
	bstrText = strTime.AllocSysString();
	m_pSegment->m_pComponent->m_pIFramework->SetStatusBarPaneText( 1, bstrText, TRUE );
	*/

	if( m_pTimeline )
	{
		ASSERT(m_pSegment);
		EnterCriticalSection( &m_pSegment->m_csSegmentState );
		if( m_pSegment->m_pIDMCurrentSegmentState )
		{
			ASSERT(m_pSegment->m_pComponent);
			ASSERT(m_pSegment->m_pComponent->m_pIDMPerformance);
			MUSIC_TIME mtNow;
            REFERENCE_TIME rtNow;
			if ( SUCCEEDED( m_pSegment->m_pComponent->m_pIDMPerformance->GetTime( &rtNow, &mtNow ) ) )
			{
                if ((m_pSegment->m_dwSegmentFlags & DMUS_SEGIOF_REFLOOP) && m_pSegment->m_pComponent)
                {
                    rtNow = GetTimeOffset( rtNow, 
                        m_pSegment->m_rtCurrentStartTime, 
                        0,  // We currently don't have a way to manage the start position in reference time. 
					    m_pSegment->m_rtCurrentLoopStart, 
                        m_pSegment->m_rtCurrentLoopEnd, 
                        m_pSegment->m_rtLength,
					    m_pSegment->m_dwCurrentMaxLoopRepeats);
                    m_pSegment->m_pComponent->m_pIDMPerformance->ReferenceToMusicTime( rtNow + m_pSegment->m_rtCurrentStartTime, &mtNow);
                    mtNow -= m_pSegment->m_mtCurrentStartTime;
                }
                else
                {
				    mtNow = GetTimeOffset( mtNow, 
                        m_pSegment->m_mtCurrentStartTime, 
                        m_pSegment->m_mtCurrentStartPoint,
					    m_pSegment->m_mtCurrentLoopStart, 
                        m_pSegment->m_mtCurrentLoopEnd, 
                        m_pSegment->m_mtLength,
					    m_pSegment->m_dwCurrentMaxLoopRepeats);
                }

				m_pTimeline->SetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, mtNow );
				LeaveCriticalSection( &m_pSegment->m_csSegmentState );
				return;
			}
		}
		else
		{
			EnableTimer( FALSE );
		}
		LeaveCriticalSection( &m_pSegment->m_csSegmentState );
	}

	CDialog::OnTimer(nIDEvent);
}

void CSegmentDlg::SetTimelineLength(int iLength)
{
	ASSERT(m_pTimeline);
	if ( !m_pTimeline )
	{
		return;
	}

	// Update the timeline with the new length.
	VARIANT vtInit;
	vtInit.vt = VT_I4;

	V_I4(&vtInit) = iLength;
	m_pTimeline->SetTimelineProperty(TP_CLOCKLENGTH, vtInit);
	m_pTimeline->Refresh();
}

void CSegmentDlg::RefreshTimeline()
{
	ASSERT(m_pTimeline);
	if( !m_pTimeline )
	{
		return;
	}

	m_pTimeline->Refresh();
}

HRESULT CSegmentDlg::GetTimeline(IDMUSProdTimeline **ppTimeline)
{
	if(ppTimeline == NULL)
	{
		return E_POINTER;
	}
	*ppTimeline = m_pTimeline;
	ASSERT(m_pTimeline);
	if( m_pTimeline )
	{
		m_pTimeline->AddRef();
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::EnableTimer

void CSegmentDlg::EnableTimer( BOOL fEnable )
{
	if( fEnable )
	{
		if( m_nTimerID == 0 )
		{
			m_nTimerID = ::SetTimer( m_hWnd, 1, 40, NULL );
		}
	}
	else
	{
		if( m_nTimerID )
		{
			::KillTimer( m_hWnd, m_nTimerID );
			m_nTimerID = 0;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::AddTrack

HRESULT CSegmentDlg::AddTrack( CTrack* pTrack )
{
	ASSERT( pTrack );
	if( pTrack == NULL )
	{
		return E_NOTIMPL;
	}

	IDMUSProdStripMgr *pStripMgr = NULL;

	pTrack->GetStripMgr( &pStripMgr );
	if( pStripMgr == NULL )
	{
		TRACE("CSegmentDlg::AddTrack: Unable to get track's StripMgr.\n");
		return E_FAIL;
		// BUGBUG: Display message box saying unable to create track's editor
	}
	else
	{
		// Freeze undo queue
		BOOL fOrigFreezeState = FALSE;
		VARIANT var;
		if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
		{
			fOrigFreezeState = V_BOOL(&var);
		}

		if( !fOrigFreezeState )
		{
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		// Must add the strip to the Timeline before loading in its data
		m_pTimeline->AddStripMgr( pStripMgr, pTrack->m_dwGroupBits );

		// Tell the StripMgr our current state
		EnterCriticalSection( &m_pSegment->m_csSegmentState );
		if( m_pSegment->m_pIDMCurrentSegmentState )
		{
			pStripMgr->OnUpdate( GUID_Segment_Start, 0, m_pSegment->m_pIDMCurrentSegmentState );
		}
		LeaveCriticalSection( &m_pSegment->m_csSegmentState );

		BOOL fFlag = m_fActive;
		pStripMgr->OnUpdate( GUID_Segment_WindowActive, 0, &fFlag );
		fFlag = m_pSegment->m_fRecordPressed;
		pStripMgr->OnUpdate( GUID_Segment_RecordButton, 0, &fFlag );

		// Re-enable undo queue, if neccessary
		if( !fOrigFreezeState )
		{
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		pStripMgr->Release();
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::RemoveTrack

HRESULT CSegmentDlg::RemoveTrack( CTrack* pTrack )
{
	ASSERT( pTrack );
	if( pTrack == NULL )
	{
		return E_NOTIMPL;
	}

	IDMUSProdStripMgr *pStripMgr = NULL;

	pTrack->GetStripMgr( &pStripMgr );
	if( pStripMgr == NULL )
	{
		TRACE("CSegmentDlg::AddTrack: Unable to get track's StripMgr.\n");
		return E_FAIL;
		// BUGBUG: Display message box saying unable to create track's editor
	}
	else
	{
		m_pTimeline->RemoveStripMgr( pStripMgr );
		pStripMgr->Release();
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::Activate

void CSegmentDlg::Activate( BOOL fActive )
{
	if( m_fActive != fActive )
	{
		m_fActive = fActive;

		if( m_pTimeline )
		{
			if( !m_fActive )
			{
				// Initialize the strip reference pointer to NULL
				m_pActiveStripWhenWindowDeactivated = NULL;

				// Determine which strip (if any) is active
				VARIANT var;
				if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) )
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
					m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
				}
			}
			else if( m_pActiveStripWhenWindowDeactivated )
			{
				// Activate the strip that was active when we our window was deactivated
				VARIANT var;
				var.vt = VT_UNKNOWN;
				V_UNKNOWN(&var) = m_pActiveStripWhenWindowDeactivated;
				V_UNKNOWN(&var)->AddRef();
				m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );

				m_pActiveStripWhenWindowDeactivated->Release();
				m_pActiveStripWhenWindowDeactivated = NULL;
			}

			// Notify the strip managers of the new window activation state
			m_pTimeline->NotifyStripMgrs( GUID_Segment_WindowActive, 0xFFFFFFFF, &fActive );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::UpdateZoomAndScroll

void CSegmentDlg::UpdateZoomAndScroll( void )
{
	ASSERT( m_pSegment );
	if( m_pSegment && m_pTimeline )
	{
		VARIANT vtInit;
		if( m_pSegment->m_dblZoom == 0.0 )
		{
			m_pSegment->m_dblZoom = 0.03125;
		}
		V_R8(&vtInit) = m_pSegment->m_dblZoom;
		vtInit.vt = VT_R8;
		m_pTimeline->SetTimelineProperty( TP_ZOOM, vtInit );

		V_I4(&vtInit) = m_pSegment->m_lVerticalScroll;
		vtInit.vt = VT_I4;
		m_pTimeline->SetTimelineProperty( TP_VERTICAL_SCROLL, vtInit );

		V_I4(&vtInit) = long(m_pSegment->m_dblHorizontalScroll * m_pSegment->m_dblZoom);
		vtInit.vt = VT_I4;
		m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, vtInit );

		V_I4(&vtInit) = m_pSegment->m_tlSnapTo;
		vtInit.vt = VT_I4;
		m_pTimeline->SetTimelineProperty( TP_SNAP_TO, vtInit );

		V_I4(&vtInit) = m_pSegment->m_lFunctionbarWidth;
		vtInit.vt = VT_I4;
		m_pTimeline->SetTimelineProperty( TP_FUNCTIONBAR_WIDTH, vtInit );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg::OnApp

LRESULT CSegmentDlg::OnApp( WPARAM wParam, LPARAM lParam )
{
	// wParam == dwNotificationOption
	// lParam == mtTime

	switch( wParam )
	{
	case DMUS_NOTIFICATION_SEGSTART:
		// If the timeline exists, notify it that we started playing
		if( m_pTimeline )
		{
			m_pTimeline->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pSegment->m_rtCurrentStartTime );
		}

		// Start the cursor, since we're starting to play
		m_pSegment->EnableDialogTimer( TRUE );
		break;

	case DMUS_NOTIFICATION_SEGABORT:
	case DMUS_NOTIFICATION_SEGEND:
		// Stop the cursor, since we're stopped
		m_pSegment->EnableDialogTimer( FALSE );

		// If the timeline exists, notify it that we stopped playing
		if( m_pTimeline )
		{
			// If the timeline exists, notify all stripmgrs that we stopped playing
			m_pTimeline->NotifyStripMgrs( GUID_Segment_Stop, 0xFFFFFFFF, NULL );

			// The start time may not be zero if we transitioned to ourself from ourself zero
			//ASSERT( m_pSegment->m_rtCurrentStartTime == 0 );
			if( m_pSegment->m_rtCurrentStartTime != 0 )
			{
				// Don't do anything, since we should very soon get a SegStart message
				break;
			}

			if( wParam == DMUS_NOTIFICATION_SEGEND )
			{
				// This must happen before SetMarkerTime to ensure the correct time is displayed in the status bar
				m_pTimeline->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pSegment->m_rtCurrentStartTime );

				// Reset cursor to the start
				m_pTimeline->SetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, 0);
			}
			else
			{
				// If the start time and/or point are valid
				if( (m_pSegment->m_mtCurrentStartTime != 0)
				||	(m_pSegment->m_mtCurrentStartPoint != 0) )
				{
					// Move the cursor 
					MUSIC_TIME mtNow = lParam;
                    REFERENCE_TIME rtNow;

                    if ((m_pSegment->m_dwSegmentFlags & DMUS_SEGIOF_REFLOOP) && m_pSegment->m_pComponent)
                    {
                        m_pSegment->m_pComponent->m_pIDMPerformance->MusicToReferenceTime(mtNow,&rtNow);
                        rtNow -= m_pSegment->m_rtCurrentStartTime;
                        rtNow = GetTimeOffset( rtNow, 
                            m_pSegment->m_rtCurrentStartTime, 
                            0,  // We currently don't have a way to manage the start position in reference time. 
					        m_pSegment->m_rtCurrentLoopStart, 
                            m_pSegment->m_rtCurrentLoopEnd, 
                            m_pSegment->m_rtLength,
					        m_pSegment->m_dwCurrentMaxLoopRepeats);
                        m_pSegment->m_pComponent->m_pIDMPerformance->ReferenceToMusicTime( rtNow + m_pSegment->m_rtCurrentStartTime, &mtNow);
                        mtNow -= m_pSegment->m_mtCurrentStartTime;
                    }
                    else
                    {
					    // Convert mtNow from absolute time to an offset from when the segment started playing
					    mtNow -= m_pSegment->m_mtCurrentStartTime - m_pSegment->m_mtCurrentStartPoint;

					    if( mtNow > m_pSegment->m_mtCurrentStartPoint )
					    {
						    mtNow = GetTimeOffset( lParam, m_pSegment->m_mtCurrentStartTime, m_pSegment->m_mtCurrentStartPoint,
									    m_pSegment->m_mtCurrentLoopStart, m_pSegment->m_mtCurrentLoopEnd, m_pSegment->m_mtLength,
									    m_pSegment->m_dwCurrentMaxLoopRepeats);

					    }
                    }
                    m_pTimeline->SetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, mtNow );
				}

				// This must happen after SetMarkerTime to ensure the correct time is displayed in the status bar
				m_pTimeline->SetParam( GUID_TimelineSetSegStartTime, 1, 0, 0, &m_pSegment->m_rtCurrentStartTime );
			}
		}
		break;
	}

	return 1;
}
