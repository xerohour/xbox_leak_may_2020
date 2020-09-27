// PrivateTimelineCtl.cpp : Implementation of CPrivateTimelineCtl
// @doc Timeline
#include "stdafx.h"
#include <math.h>
#include <SegmentGuids.h>
#include "Timeline.h"
#include "PrivateTimelineCtl.h"
#include "TimelineCtl.h"
#include "TimeStripMgr.h"
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <winresrc.h>
#pragma warning( pop )
#include <Conductor.h>
#include <initguid.h>
#include <dmusici.h>
#include <dmusicf.h>
#include "TimelineDataObject.h"
#include "SegmentIO.h"
#include "windowsx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CPrivateTimelineCtl::CPrivateTimelineCtl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pDMUSProdFramework = NULL;
	m_pTimelineCtl = NULL;
	m_pStripMgrList = NULL;
	m_pStripList = NULL;
	m_pActiveStripList = NULL;
	m_pTempoMapMgr = NULL;
	m_dblZoom = .125; // set initial zoom factor
	m_lLength = DMUS_PPQ * 4 * 20; // start at 20 measures, four quarter notes per measure
	m_lXScrollPos = 0;
	m_lYScrollPos = 0;
	m_fFreezeUndo = FALSE;
	m_lFunctionBarWidth = LEFT_DRAW_OFFSET + 80;
	m_pTimelineCallback = NULL;
	m_fMusicTime = TRUE;
	m_ptPasteType = TL_PASTE_OVERWRITE;
	m_lstNotifyEntry.RemoveAll();
	InitializeCriticalSection( &m_csOnDataChanged );
	m_pTimeStrip = NULL;
	m_pIDMPerformance = NULL;
	m_fInSetTimelineOleCtl = false;
}

HRESULT CPrivateTimelineCtl::FinalConstruct()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// create the time display strip
	IDMUSProdStripMgr*	pSM;
	if( SUCCEEDED( CoCreateInstance( CLSID_TimeStripMgr, NULL, CLSCTX_INPROC_SERVER,
		IID_IDMUSProdStripMgr, (void**)&pSM )))
	{
		AddStripMgr( pSM, 0xffffffff );
		pSM->Release();
	}
	else
	{
		ASSERT(FALSE); // couldn't create time display mgr.
	}

	return CComObjectRootEx<CComSingleThreadModel>::FinalConstruct();
}

CPrivateTimelineCtl::~CPrivateTimelineCtl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimelineCtl == NULL );

	FinalCleanUp();

	DeleteCriticalSection( &m_csOnDataChanged );
}

//  @method HRESULT | IDMUSProdTimeline | DrawMusicLines | This method draws
//		vertical bar, beat, and grid lines in the specified device context.
//
//  @parm   HDC | hdc | A handle to the device context to draw in
//  @parm   <t MUSICLINE_PROPERTY> | mlp | Which type of lines to draw.  Must be one of <t MUSICLINE_PROPERTY>
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_FAIL | Unable to attach to <p hdc>.
//	@rvalue E_INVALIDARG | <p mlp> does not contain a valid value.
//
//	@xref	<i IDMUSProdTimeline>, <t MUSICLINE_PROPERTY>
HRESULT CPrivateTimelineCtl::DrawMusicLines( HDC hdc, MUSICLINE_PROPERTY mlp, DWORD dwGroupBits, DWORD dwIndex, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	ASSERT( hdc );
	ASSERT( (mlp == ML_DRAW_MEASURE_BEAT_GRID) || (mlp == ML_DRAW_MEASURE_BEAT) );
	if ( !hdc || ((mlp != ML_DRAW_MEASURE_BEAT_GRID) && (mlp != ML_DRAW_MEASURE_BEAT)) )
	{
		return E_INVALIDARG;
	}

	// Time signature behavior:  If the time signature changes during a measure, the current time
	// signature will continute to apply for that measure.  At the end of the measure, the time
	// signature will change to the most recent one available.  Repeat this process for each
	// time signature change.

	// Create the pens
	HPEN hPenMeasureLine, hPenBeatLine, hPenSubBeatLine;
	hPenMeasureLine = ::CreatePen( MEASURE_LINE_PENSTYLE, MEASURE_LINE_WIDTH, MEASURE_LINE_COLOR );
	if( hPenMeasureLine == NULL )
	{
		return E_OUTOFMEMORY;
	}

	hPenBeatLine = ::CreatePen( BEAT_LINE_PENSTYLE, BEAT_LINE_WIDTH, BEAT_LINE_COLOR );
	if( hPenBeatLine == NULL )
	{
		::DeleteObject( hPenMeasureLine );
		return E_OUTOFMEMORY;
	}

	if ( mlp == ML_DRAW_MEASURE_BEAT_GRID )
	{
		hPenSubBeatLine = ::CreatePen( SUBBEAT_LINE_PENSTYLE, SUBBEAT_LINE_WIDTH, SUBBEAT_LINE_COLOR );
		if( hPenSubBeatLine == NULL )
		{
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		hPenSubBeatLine = NULL;
	}

	// Set up our start time, end time
	RECT rectClip;
	MUSIC_TIME mtLeft, mtRight, mtTemp;
	::GetClipBox( hdc, &rectClip );
	PositionToClocks( rectClip.left + lXOffset, &mtLeft );
	// Since the bar lines are two pixels wide, we need to add 1 here
	PositionToClocks( rectClip.right + lXOffset + 1, &mtRight );

	long lPosition = 0; // integer position

	// Save the current pen and switch to the Measure Line pen
	HPEN hPenOld;
	hPenOld = static_cast<HPEN>( ::SelectObject( hdc, hPenMeasureLine ) );

	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext = 1;
	BYTE bBeat;

	// Handle drawing in negative times
	if( mtLeft < 0 )
	{
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, 0, NULL, &TimeSig ) ) )
		{
			::SelectObject( hdc, hPenOld );
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			if( hPenSubBeatLine )
			{
				::DeleteObject( hPenSubBeatLine );
			}
			return E_UNEXPECTED;
		}

		// Compute the number of clocks per measure
		const MUSIC_TIME mtClocksPerMeasure = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		mtTSCur = (mtLeft - mtClocksPerMeasure + 1) / mtClocksPerMeasure;
		mtTSCur *= mtClocksPerMeasure;
	}

	while( mtTSCur < mtRight )
	{
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max( 0, mtTSCur), &mtTSNext, &TimeSig ) ) )
		{
			::SelectObject( hdc, hPenOld );
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			if( hPenSubBeatLine )
			{
				::DeleteObject( hPenSubBeatLine );
			}
			return E_UNEXPECTED;
		}

		MUSIC_TIME mtNext;
		if( mtTSNext == 0 )
		{
			mtTSNext = m_lLength;
			mtNext = LONG_MAX;
		}
		else
		{
			mtTSNext += max( 0, mtTSCur );
			mtNext = mtTSNext;
		}

		while( (mtTSCur < mtNext) && (mtTSCur < mtRight) )
		{
			// For this measure, compute clocks per beat and clocks per grid
			const MUSIC_TIME mtClocksPerBeat = NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

			// Draw this measure
			if ( mtTSCur + TimeSig.bBeatsPerMeasure * mtClocksPerBeat >= mtLeft )
			{

				// Draw measure line
				if ( mtTSCur >= mtLeft )
				{
					::SelectObject( hdc, hPenMeasureLine );
					ClocksToPosition( mtTSCur, &lPosition );
					::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
					::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );
				}

				// Draw beats
				bBeat = 1;
				::SelectObject( hdc, hPenBeatLine );
				while ( bBeat < TimeSig.bBeatsPerMeasure )
				{
					mtTSCur += mtClocksPerBeat;

					// Draw beat line
					ClocksToPosition( mtTSCur, &lPosition );
					::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
					::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );

					bBeat++;
				}

				// Draw Grids
				if ( mlp == ML_DRAW_MEASURE_BEAT_GRID )
				{
					const MUSIC_TIME mtClocksPerGrid = mtClocksPerBeat / TimeSig.wGridsPerBeat;
					bBeat = 0;
					mtTSCur = mtTSCur - mtClocksPerBeat * (TimeSig.bBeatsPerMeasure - 1);
					::SelectObject( hdc, hPenSubBeatLine );
					while ( bBeat < TimeSig.bBeatsPerMeasure )
					{
						// Draw Grids

						// Save next beat position
						mtTemp = mtTSCur + mtClocksPerBeat;

						BYTE bGrid = 1;
						mtTSCur += mtClocksPerGrid;

						// Draw grid lines
						while ( bGrid < TimeSig.wGridsPerBeat )
						{
							ClocksToPosition( mtTSCur, &lPosition );
							::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
							::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );

							bGrid++;
							mtTSCur += mtClocksPerGrid;
						}
						mtTSCur = mtTemp;
						bBeat++;
					}
				}
				else
				{
					mtTSCur += mtClocksPerBeat;
				}
			}
			else
			{	// Advance time by one measure
				mtTSCur += TimeSig.bBeatsPerMeasure * mtClocksPerBeat;
			}
		}
	}

	// Restore the previous pen
	::SelectObject( hdc, hPenOld );

	::DeleteObject( hPenMeasureLine );
	::DeleteObject( hPenBeatLine );
	if( hPenSubBeatLine )
	{
		::DeleteObject( hPenSubBeatLine );
	}

	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | AddStripMgr | This method adds an <i IDMUSProdStripMgr> to the
//		list of strip managers within the Timeline.
//
//	@comm	Using the <p dwGroupBits> parameter and the Strip manager's FourCCCKIds
//		(returned by the method <om IDMUSProdStripMgr::GetFourCCCKIDs>), the Timeline determines a
//		position for the StripMgr in its internal list.<nl>
//		They are ordered first by the least track group number they belong to, then in the following order,
//		then by the order they were inserted:<nl>
//		Chord<nl>
//		Signpost<nl>
//		ChordMap Reference<nl>
//		Groove<nl>
//		Tempo<nl>
//		Style Reference<nl>
//		Sequence<nl>
//		Time Signature<nl>
//		Band<nl>
//		Mute<nl>
//		All other strips
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to add
//  @parm	DWORD | dwGroupBits | Which track group(s) this strip manager belongs to.  A value
//		of 0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStripMgr>
//	@rvalue E_INVALIDARG | The strip manager was previously added to the Timeline
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the strip manager
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::RemoveStripMgr>
HRESULT CPrivateTimelineCtl::AddStripMgr( IDMUSProdStripMgr* pIStripMgr, DWORD dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pIStripMgr )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStripMgr == pIStripMgr )
		{
			return E_INVALIDARG;
		}
	}

	StripMgrList*	pSML = new StripMgrList( pIStripMgr, dwGroupBits );
	if( pSML )
	{
		// Empty list
		if( !m_pStripMgrList )
		{
			m_pStripMgrList = pSML;
			pSML->m_pNext = NULL;

			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
			pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
			Release();
			return S_OK;
		}

		DMUS_IO_TRACK_HEADER ioTrackHeader;
		ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
		VARIANT varTrackHeader;
		varTrackHeader.vt = VT_BYREF;
		V_BYREF(&varTrackHeader) = &ioTrackHeader;
		if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
		{
			TRACE("Timeline: Unable to get StripMgr's FourCCCKIDs\n");
		}

		int nType1, nType2;
		nType1 = StripCLSIDToInt( ioTrackHeader.guidClassID );

		StripMgrList*	pScanOld = NULL;
		for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
		{
			ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if( FAILED( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				TRACE("Timeline: Unable to get StripMgr's FourCCCKIDs\n");
			}

			// Ensure the TimeStripMgr is always the last strip
			if( pScan->m_pStripMgr->IsParamSupported( CLSID_TimeStripMgr ) == S_OK )
			{
				// if pSML should go before pScan, insert it there
				if( pScanOld )
				{
					pScanOld->m_pNext = pSML;
				}
				else
				{
					// Head of the list
					ASSERT( pScan == m_pStripMgrList );
					m_pStripMgrList = pSML;
				}
				pSML->m_pNext = pScan;

				VARIANT varTimeline;
				varTimeline.vt = VT_UNKNOWN;
				QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
				pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
				Release();
				return S_OK;
			}
			nType2 = StripCLSIDToInt( ioTrackHeader.guidClassID );

			if( CompareStrips( nType2, pScan->m_dwGroupBits, 0, NULL,
							   nType1, dwGroupBits, 0, NULL ) == 2 )
			{
				// if pSML should go before pScan, insert it there
				if( pScanOld )
				{
					pScanOld->m_pNext = pSML;
				}
				else
				{
					// Head of the list
					ASSERT( pScan == m_pStripMgrList );
					m_pStripMgrList = pSML;
				}
				pSML->m_pNext = pScan;

				VARIANT varTimeline;
				varTimeline.vt = VT_UNKNOWN;
				QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
				pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
				Release();
				return S_OK;
			}
			pScanOld = pScan;
		}
		
		// End of the list
		pScanOld->m_pNext = pSML;
		pSML->m_pNext = NULL;

		VARIANT varTimeline;
		varTimeline.vt = VT_UNKNOWN;
		QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
		pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
		Release();
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

//  @method HRESULT | IDMUSProdTimeline | RemoveStripMgr | This method removes a strip manager
//		from the Timeline.
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to remove
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStripMgr>
//	@rvalue E_INVALIDARG | <p pIStripMgr> was not previously added to the Timeline via
//		<om IDMUSProdTimeline::AddStripMgr>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::AddStripMgr>
HRESULT CPrivateTimelineCtl::RemoveStripMgr(
	/* [in] */	IDMUSProdStripMgr* pIStripMgr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT			hr;

	if( NULL == pIStripMgr )
	{
		return E_POINTER;
	}
	StripMgrList	*pSML, *pSMLold;
	pSML = m_pStripMgrList;
	pSMLold = NULL;
	hr = E_INVALIDARG;
	// Remove pIStripMgr from m_pStripMgrList
	while ( pSML )
	{
		if ( pSML->m_pStripMgr == pIStripMgr )
		{
			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			V_UNKNOWN(&varTimeline) = NULL;
			pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
			if ( pSMLold )
			{
				pSMLold->m_pNext = pSML->m_pNext;
			}
			else
			{
				m_pStripMgrList = pSML->m_pNext;
			}
			delete pSML;
			pSML = NULL;
			hr = S_OK;
		}
		else
		{
			pSMLold = pSML;
			pSML = pSML->m_pNext;
		}
	}

	// Remove the Stripmgr from the list of Notify Entries
	POSITION pos, pos2;
	NotifyEntry *pNotifyEntry;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		// Found guidNotify, now find pIStripMgr
		NotifyListEntry* pNotifyListEntry = NULL;
		pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition( );
		while ( pos2 != NULL )
		{
			POSITION pos3 = pos2;
			pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
			if( pNotifyListEntry->pIStripMgr == pIStripMgr )
			{
				// We've found it, now remove it from the list and delete our entry
				pNotifyEntry->m_lstNotifyListEntry.RemoveAt( pos3 );
				delete pNotifyListEntry;

				// Check if the list is empty
				if ( pNotifyEntry->m_lstNotifyListEntry.IsEmpty() )
				{
					// If the list is empty, remove it from m_lstNotifyEntry
					pos2 = m_lstNotifyEntry.Find( pNotifyEntry );
					ASSERT( pos2 );
					if ( pos2 )
					{
						m_lstNotifyEntry.RemoveAt( pos2 );
						delete pNotifyEntry;
						pos2 = NULL;
					}
				}
			}
		}
	}

	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | AddStrip | This method adds a strip at the
//		bottom of the strips displayed by the Timeline.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStrip>
//	@rvalue E_FAIL | The strip is already displayed by the Timeline
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the strip
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtDefaultPos>, <om IDMUSProdTimeline::InsertStripAtPos>.
HRESULT CPrivateTimelineCtl::AddStrip( 
    /* [in] */ IDMUSProdStrip* pIStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}

	if( m_pTimeStrip == NULL )
	{
		m_pTimeStrip = static_cast<CTimeStrip *>(pIStrip);
		ASSERT( m_pTimeStrip );
	}

	return InternalInsertStripAtPos( pIStrip, 0xffffffff, GUID_AllZeros, 1, 0 );
}

//  @method HRESULT | IDMUSProdTimeline | RemoveStrip | This method removes a strip from
//		the Timeline.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to remove
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline via
//		<om IDMUSProdTimeline::AddStrip>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::AddStrip>
HRESULT CPrivateTimelineCtl::RemoveStrip(
	/* [in] */	IDMUSProdStrip* pIStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT			hr = S_OK;
	RECT			rect, rectWin;

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}
	// if the strip is already in the list, return E_FAIL
	StripList* pSL = FindStripList(pIStrip);
	if( !pSL )
	{
		hr = E_INVALIDARG;
	}
	else
	{
		// If this strip is active, sent it a killfocus
		if ( m_pActiveStripList == pSL )
		{
			CallStripWMMessage( pSL, WM_KILLFOCUS, 0, 0 );
			m_pActiveStripList = NULL;
		}

		// If a strip is removed after the Timeline window is destroyed,
		// don't recompute the positions of the strips and scrollbars.
		if( m_pTimelineCtl
		&&	::IsWindow( m_pTimelineCtl->m_hWnd ) )
		{
			m_pTimelineCtl->GetClientRect( &rectWin );
			m_pTimelineCtl->GetStripClientRect( pSL, &rect );
			rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
			m_pTimelineCtl->InvalidateRect( &rectWin, FALSE );
			m_pTimelineCtl->ComputeScrollBars();
		}

		// remove the strip from m_pStripList
		ASSERT(m_pStripList);
		if( m_pStripList )
		{
			StripList*	pTemp = m_pStripList;
			StripList*	pTempOld = NULL;
			while( pTemp )
			{
				if ( pTemp == pSL )
				{
					if (pTempOld)
					{
						pTempOld->m_pNext = pTemp->m_pNext;
					}
					else
					{
						m_pStripList = pTemp->m_pNext;
					}

					// Send WM_MOVE messages to all strips that were moved
					if( m_pTimelineCtl
					&&	::IsWindow(m_pTimelineCtl->m_hWnd) )
					{
						pTemp = pTemp->m_pNext;
						while( pTemp )
						{
							pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
							pTemp = pTemp->m_pNext;
						}
					}
					else
					{
						pTemp = NULL;
					}
				}
				else
				{
					pTempOld = pTemp;
					pTemp = pTemp->m_pNext;
				}
			}
		}

		if( m_pTimelineCtl )
		{
			if ( m_pTimelineCtl->m_pMouseStripList == pSL )
			{
				m_pTimelineCtl->m_pMouseStripList = NULL;
			}
			if ( m_pTimelineCtl->m_pLastDragStripList == pSL )
			{
				m_pTimelineCtl->m_pLastDragStripList = NULL;
			}
		}
		IDMUSProdTimelineEdit* pITimelineEdit;
		if ( SUCCEEDED(pIStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**)&pITimelineEdit )))
		{
			pITimelineEdit->Release();
		}

		pIStrip->OnWMMessage( WM_DESTROY, 0, 0, 0, 0 );

		delete pSL;

		// If we need to scroll up, do so
		if( m_lYScrollPos
		&&	m_pTimelineCtl
		&&	::IsWindow( m_pTimelineCtl->m_hWnd ) )
		{
			RECT rect, rectS;
			m_pTimelineCtl->GetClientRect(&rect);
			m_pTimelineCtl->m_ScrollHorizontal.GetClientRect(&rectS);
			rect.bottom -= rectS.bottom;

			long lMaxScroll = TotalStripHeight() - rect.bottom;
			if( lMaxScroll < m_lYScrollPos )
			{
				int nPos;
				if( lMaxScroll < 1 )
				{
					nPos = 0;
				}
				else
				{
					nPos = ( lMaxScroll * m_pTimelineCtl->m_ScrollVertical.GetScrollLimit()) / lMaxScroll;
				}
				int iTemp = 0;
				m_pTimelineCtl->OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, nPos ), (LPARAM) ((HWND) m_pTimelineCtl->m_ScrollVertical), iTemp );
			}
		}
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | GetParam | This method retrieves data of the specified type
//		from a strip manager in the Timeline.
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to obtain.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
///  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all strip managers
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Index of the strip manager in the group(s) from which to obtain the data. 
//  @parm   MUSIC_TIME | mtTime | Time from which to obtain the data.
//	@parm   MUSIC_TIME* | pmtNext | Address of a variable to receive the time (relative to
//		the current time) until which the data is valid. If this returns a value of 0, it means
//		either that the data will always be valid, or that it is unknown when it will become
//		invalid. If this information is not needed, <p pmtNext> may be set to NULL. 
//  @parm   void* | pData | Address of an allocated structure in which the data is to be returned. This
//		structure must be of the appropriate kind and size for the data type identified by <p rguidType>.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::GetParam>.
//
//	@comm	Strip managers are searched in the order that they are listed in the Timeline.  See
//		<om IDMUSProdTimeline::AddStripMgr> for the order they are listed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pData>
//	@rvalue E_INVALIDARG | No strip managers support the requested <p rguidType>.
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::SetParam>, <om IDMUSProdStripMgr::GetParam>
HRESULT CPrivateTimelineCtl::GetParam(/* [in] */  REFGUID		guidType,
				/* [in] */  DWORD		dwGroupBits,
				/* [in] */  DWORD		dwIndex,
				/* [in] */  MUSIC_TIME	mtTime,
				/* [out] */ MUSIC_TIME*	pmtNext,
				/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	HRESULT hr = E_INVALIDARG;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				/*
				// Get the track extras flags
				VARIANT varTrackExtras;
				varTrackExtras.vt = VT_BYREF;
				DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
				if( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackExtras ) == S_OK )
				{
					if( !(ioTrackExtrasHeader.dwFlags & DMUS_TRACKCONFIG_CONTROL_ENABLED) )
					{
						continue;
					}
				}
				*/

				if ( dwIndex == 0 )
				{
					if ( pData == NULL )
					{
						return E_POINTER;
					}
					else
					{
						if( SUCCEEDED( pScan->m_pStripMgr->GetParam( guidType, mtTime, pmtNext, pData ) ) )
						{
							return S_OK;
						}
						else
						{
							hr = E_UNEXPECTED;
						}
					}
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | SetParam | This method sets data on a
//		strip manager in the timeline
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to set.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
//  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all strip managers
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Index of the strip manager in the group(s) identified by <p dwGroupBits> where data is to be set. 
//  @parm   MUSIC_TIME | mtTime | Time at which to set the data.
//  @parm   void* | pData | Address of structure containing the data. This structure must be of
//		the appropriate kind and size for the data type identified by rguidType.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::SetParam>.
//
//	@comm	Strip managers are searched in the order that they are listed in the Timeline.  See
//		<om IDMUSProdTimeline::AddStripMgr> for the order they are listed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pData>
//	@rvalue E_INVALIDARG | No strip managers support the requested <p rguidType>.
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::GetParam>, <om IDMUSProdStripMgr::SetParam>
HRESULT CPrivateTimelineCtl::SetParam(/* [in] */ REFGUID		guidType,
				/* [in] */ DWORD		dwGroupBits,
				/* [in] */ DWORD		dwIndex,
				/* [in] */ MUSIC_TIME	mtTime,
				/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// Handle the timeline-specific Setparams
	if( m_pTimelineCtl )
	{
		if( SUCCEEDED( m_pTimelineCtl->SetParam( guidType, dwGroupBits, dwIndex, mtTime, pData ) ) )
		{
			return S_OK;
		}
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				if ( dwIndex == 0 )
				{
					if ( pData == NULL )
					{
						return E_POINTER;
					}
					else
					{
						return pScan->m_pStripMgr->SetParam( guidType, mtTime, pData );
					}
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}
	return E_INVALIDARG;
}

//  @method HRESULT | IDMUSProdTimeline | GetStripMgr | This method retrieves a pointer to the specified
//		strip manager.
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to search for.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
//  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all tracks
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Zero-based index into the list of tracks of type <p rguidType>
//		and in group <p dwGroupBits> to return. If multiple groups are selected in <p dwGroupBits>,
//		this index will indicate the nth track of type <p rguidType> encountered in the union of
//		the groups selected. 
//  @parm   <i IDMUSProdStripMgr>** | ppIStripMgr | Address of a variable to receive a pointer to
//		the strip manager.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::GetTrack>.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p ppIStripMgr>
//	@rvalue E_INVALIDARG | No matching strip manager was found
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdStripMgr::IsParamSupported>
HRESULT CPrivateTimelineCtl::GetStripMgr(/* [in] */ REFGUID				guidType,
					/* [in] */ DWORD				dwGroupBits,
					/* [in] */ DWORD				dwIndex,
					/* [out,retval] */ IDMUSProdStripMgr**	ppStripMgr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				if ( dwIndex == 0 )
				{
					*ppStripMgr = pScan->m_pStripMgr;
					pScan->m_pStripMgr->AddRef();
					return S_OK;
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}

	DMUS_IO_TRACK_HEADER ioTrackHeader;
	VARIANT varTrackHeader;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}

			ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if ( SUCCEEDED( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				if( ::IsEqualGUID( guidType, ioTrackHeader.guidClassID ) )
				{
					if ( dwIndex == 0 )
					{
						*ppStripMgr = pScan->m_pStripMgr;
						pScan->m_pStripMgr->AddRef();
						return S_OK;
					}
					else
					{
						dwIndex--;
					}
				}
			}
		}
	}

	return E_INVALIDARG;
}

//  @method HRESULT | IDMUSProdTimeline | InsertStripAtDefaultPos | This method inserts a strip into
//		timeline, ordering them by using <p rclsidType>, <p dwGroupBits> and <p dwIndex>.
//
//	@comm	Using the <p dwGroupBits> parameter and the <p rclsidType> parameter, the Timeline determines a
//		position for <p pIStrip> in its display list.<nl>
//		They are ordered first by the least track group number they belong to, then in the following order,
//		then by the value of <p dwIndex>, then by the order they were inserted:<nl>
//		Chord<nl>
//		Signpost<nl>
//		ChordMap Reference<nl>
//		Groove<nl>
//		Tempo<nl>
//		Style Reference<nl>
//		Sequence<nl>
//		Time Signature<nl>
//		Band<nl>
//		Mute<nl>
//		All other strips
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add.
//  @parm	REFCLSID | rclsidType | The CLSID of the strip to add.
//  @parm   DWORD | dwGroupBits | Which track group(s) to add the strip in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group.
//  @parm   DWORD | dwIndex | Where in the list of matching strips to add the strip..
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_FAIL | <p pIStrip> was previously added.
//	@rvalue E_INVALIDARG | 0 was passed as <p dwGroupBits>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtPos>, <om IDMUSProdTimeline::AddStrip>
HRESULT CPrivateTimelineCtl::InsertStripAtDefaultPos(
		/* [in] */ IDMUSProdStrip* pStrip,
		/* [in] */ REFCLSID		clsidType,
		/* [in] */ DWORD		dwGroupBits,
		/* [in] */ DWORD		dwIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	DWORD		dwPosition = 0;

	if( NULL == pStrip )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// if the strip is already in the list, return E_FAIL
	StripList*	pScan;
	for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStrip == pStrip )
		{
			pStrip->Release();
			return E_FAIL;
		}
	}

	// determine the default position
	dwPosition = 1;
	if( m_pStripList )
	{
		if( m_pStripList->m_pNext != NULL )
		{
			// Remember to skip over the first strip.
			StripList*	pTemp = m_pStripList->m_pNext;
			int nMyId = StripCLSIDToInt( clsidType );
			BSTR bstrMyName = NULL;
			VARIANT var;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_NAME, &var ) ) && (var.vt == VT_BSTR) )
			{
				bstrMyName = V_BSTR(&var);
			}
			IUnknown *punkMyStripMgr = NULL;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &var ) ) && (var.vt == VT_UNKNOWN) )
			{
				punkMyStripMgr = V_UNKNOWN(&var);
			}

			IUnknown *punkLastStripMgr = NULL;
			IUnknown *punkTempStripMgr = NULL;
			while( pTemp )
			{
				BSTR bstrTempName = NULL;
				if( SUCCEEDED( pTemp->m_pStrip->GetStripProperty( SP_NAME, &var ) ) && (var.vt == VT_BSTR) )
				{
					bstrTempName = V_BSTR(&var);
				}
				if( SUCCEEDED( pTemp->m_pStrip->GetStripProperty( SP_STRIPMGR, &var ) ) && (var.vt == VT_UNKNOWN) )
				{
					punkTempStripMgr = V_UNKNOWN(&var);
				}
				// (If the last StripMgr and the temp StripMgr are different and the last StripMgr and my
				// StripMgr are the same)
				// OR
				// (If the last StripMgr and the temp StripMgr are different, or if the last, temp, and my
				// StripMgr are all the same, check if the strip being inserted should go BEFORE pTemp.)
				// If so, break.
				if( ((punkLastStripMgr != punkTempStripMgr) && (punkLastStripMgr == punkMyStripMgr))
				||	(((punkLastStripMgr != punkTempStripMgr) || (punkMyStripMgr == punkTempStripMgr))
					 && CompareStrips( StripCLSIDToInt(pTemp->m_clsidType), pTemp->m_dwGroupBits, pTemp->m_dwIndex, bstrTempName,
								   nMyId, dwGroupBits, dwIndex, bstrMyName ) == 2) )
				{
					if( bstrTempName )
					{
						::SysFreeString( bstrTempName );
					}
					if( punkLastStripMgr )
					{
						punkLastStripMgr->Release();
					}
					break;
				}
				if( bstrTempName )
				{
					::SysFreeString( bstrTempName );
				}
				if( punkLastStripMgr )
				{
					punkLastStripMgr->Release();
				}
				punkLastStripMgr = punkTempStripMgr;
				dwPosition++;
				pTemp = pTemp->m_pNext;
			}

			if( punkTempStripMgr )
			{
				punkTempStripMgr->Release();
			}
			if( punkMyStripMgr )
			{
				punkMyStripMgr->Release();
			}

			if( bstrMyName )
			{
				::SysFreeString( bstrMyName );
			}
		}
	}

	return InternalInsertStripAtPos( pStrip, dwPosition, clsidType, dwGroupBits, dwIndex );
}

int StripCLSIDToInt( REFCLSID clsidType )
{
	if( memcmp( &clsidType, &CLSID_DirectMusicTempoTrack, sizeof(GUID) ) == 0 )
	{
		return 0;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicTimeSigTrack, sizeof(GUID) ) == 0 )
	{
		return 10;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordMapTrack, sizeof(GUID) ) == 0 )
	{
		return 20;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSignPostTrack, sizeof(GUID) ) == 0 )
	{
		return 30;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordTrack, sizeof(GUID) ) == 0 )
	{
		return 40;
	}
	else if( (memcmp( &clsidType, &CLSID_DirectMusicMotifTrack, sizeof(GUID) ) == 0)
		 ||	 (memcmp( &clsidType, &CLSID_DirectMusicSegmentTriggerTrack, sizeof(GUID) ) == 0) )
	{
		return 50;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicCommandTrack, sizeof(GUID) ) == 0 )
	{
		return 60;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicStyleTrack, sizeof(GUID) ) == 0 )
	{
		return 70;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMelodyFormulationTrack, sizeof(GUID) ) == 0 )
	{
		return 80;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicPatternTrack, sizeof(GUID) ) == 0 )
	{
		return 90;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSeqTrack, sizeof(GUID) ) == 0 )
	{
		return 100;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicWaveTrack, sizeof(GUID) ) == 0 )
	{
		return 110;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicBandTrack, sizeof(GUID) ) == 0 )
	{
		return 120;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicParamControlTrack, sizeof(GUID) ) == 0 )
	{
		return 130;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMuteTrack, sizeof(GUID) ) == 0 )
	{
		return 140;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicScriptTrack, sizeof(GUID) ) == 0 )
	{
		return 150;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicLyricsTrack, sizeof(GUID) ) == 0 )
	{
		return 160;
	}
	else
	{
		return 170;
	}
}

int CompareStrips( int nType1, DWORD dwGroups1, DWORD dwIndex1, BSTR bstrName1, int nType2, DWORD dwGroups2, DWORD dwIndex2, BSTR bstrName2 )
{
	// If the group bits are equal, check the types
	if( dwGroups1 == dwGroups2 )
	{
		if( nType1 == nType2 )
		{
			if( dwIndex1 == dwIndex2 )
			{
				if( bstrName1 && bstrName2 )
				{
					CString strName1 = bstrName1;
					CString strName2 = bstrName2;
					return strName1.CompareNoCase( strName2 ) < 0 ? 1 : 2;
				}
			}
			return long(dwIndex1) <= long(dwIndex2) ? 1 : 2;
		}
		return nType1 < nType2 ? 1 : 2;
	}

	// Continue while both group bits have at least one bit still set
	while( dwGroups1 && dwGroups2 )
	{
		if( (dwGroups1 & dwGroups2 & 1) == 0 )
		{
			// One of the first bits of dwGroups1 or dwGroups2 is zero
			if( dwGroups1 & 1 )
			{
				// The first bit of dwGroups1 is one and at least one bit in dwGroups2 is set -
				// #1 should go first
				return 1;
			}
			else if( dwGroups2 & 1 )
			{
				// The first bit of dwGroups2 is one and at least one bit in dwGroups1 is set -
				// #2 should go first
				return 2;
			}
			// else both of the first bits are zero - try the next bit
		}
		else
		{
			// Both of the first bits of dwGroups1 and dwGroups2 are one
			if( nType1 < nType2 )
			{
				return 1; // #1 should go first
			}
			else if ( nType1 > nType2 )
			{
				return 2; // #2 should go first
			}
			// else both are the same type - check the next bit
		}
		dwGroups1 = dwGroups1 >> 1;
		dwGroups2 = dwGroups2 >> 1;
	}

	if( dwGroups1 )
	{
		// Some of the bits in dwGroups1 are set - #2 should go first
		return 2;
	}
	else if( dwGroups2 )
	{
		// Some of the bits in dwGroups2 are set - #1 should go first
		return 1;
	}
	// dwGroups1 == dwGroups2 == 0 (Shouldn't happen!)
	ASSERT( FALSE );
	return 1;
}

HRESULT CPrivateTimelineCtl::InternalInsertStripAtPos( IDMUSProdStrip* pStrip, DWORD dwPosition, REFCLSID clsidType, DWORD dwGroupBits, DWORD dwIndex )
{
	HRESULT			hr = S_OK;
	StripList*		pSL;

	if( pStrip == NULL )
	{
		return E_POINTER;
	}

	if( dwPosition == 0 )
	{
		return E_INVALIDARG;
	}
	dwPosition--;

	// if the strip is already in the list, return E_FAIL
	StripList*	pScan;
	for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStrip == pStrip )
		{
			pStrip->Release();
			return E_FAIL;
		}
	}
	// add the strip to the list of strips
	pSL = new StripList( pStrip, clsidType );
	if( pSL == NULL )
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pSL->m_dwGroupBits = dwGroupBits;
		pSL->m_dwIndex = dwIndex;

		// ask the Strip how tall it wants to be
		VARIANT var;
		if( SUCCEEDED( pStrip->GetStripProperty( SP_DEFAULTHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}
		else if( SUCCEEDED( pStrip->GetStripProperty( SP_MAXHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}
		else if( SUCCEEDED( pStrip->GetStripProperty( SP_MINHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}

		// add the strip to the specified position
		if( m_pStripList )
		{
			StripList* pTemp = m_pStripList;
			while( pTemp->m_pNext && dwPosition > 0 )
			{
				pTemp = pTemp->m_pNext;
				dwPosition--;
			}
			pSL->m_pNext = pTemp->m_pNext;
			pTemp->m_pNext = pSL;

			// Send WM_CREATE to the strip that was added
			// Send WM_MOVE messages to all strips that were moved
			if( m_pTimelineCtl
			&&	::IsWindow(m_pTimelineCtl->m_hWnd) )
			{
				pStrip->OnWMMessage( WM_CREATE, 0, 0, 0, 0 );
				pTemp = pSL->m_pNext;
				while( pTemp )
				{
					pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
					pTemp = pTemp->m_pNext;
				}
			}
		}
		else
		{
			m_pStripList = pSL;

			// call the strip's callback that it has been added, but only if the
			// Timeline window exists
			if( m_pTimelineCtl
			&&	::IsWindow(m_pTimelineCtl->m_hWnd) )
			{
				pStrip->OnWMMessage( WM_CREATE, 0, 0, 0, 0 );
			}
		}

		if( m_pTimelineCtl )
		{
			// Compute what the scroll bars look like
			m_pTimelineCtl->ComputeScrollBars();

			// Refresh the display
			RECT rectWin, rect;
			m_pTimelineCtl->GetClientRect( &rectWin );
			m_pTimelineCtl->GetStripClientRect( pSL, &rect );
			rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
			m_pTimelineCtl->InvalidateRect( &rectWin, FALSE );
		}
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | EnumStrip | This method enumerates through all strips
//		displayed within the Timeline.
//
//  @parm   DWORD | dwEnum | Zero-based index into the Timeline's strip list. 
//  @parm   <i IDMUSProdStrip>** | ppIStrip | Address of a variable to receive a pointer to the strip.
//		The caller is responsible for Release()ing the pointer when it is done with it.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p ppIStrip>
//	@rvalue E_FAIL | There is no strip at the given index.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::EnumStrip(
		/* [in] */  DWORD			 dwEnum,
		/* [in] */  IDMUSProdStrip** ppStrip)
{
	if( ppStrip == NULL )
	{
		return E_POINTER;
	}
	StripList	*pSL;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( dwEnum == 0 )
		{
			*ppStrip = pSL->m_pStrip;
			if( pSL->m_pStrip )
			{
				pSL->m_pStrip->AddRef();
			}
			return S_OK;
		}
		dwEnum--;
	}
	*ppStrip = NULL;
	return E_FAIL;
}


//  @method HRESULT | IDMUSProdTimeline | InsertStripAtPos | This method inserts a strip at the
//		specified position.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add to the Timeline.  The strip must not
//		have been previously added to the Timeline.
//  @parm   DWORD | dwPosition | The 0-based position to add the strip at.
//
//	@comm	No strip may be inserted before the Time Strip, the first strip in the Timeline.
//		Consequently, 0 is an invalid value for <p dwPosition>.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_FAIL | The operation failed.
//	@rvalue E_OUTOFMEMORY | There was not enough available memory to complete the operation.
//	@rvalue E_INVALIDARG | dwPosition is 0.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtDefaultPos>, <om IDMUSProdTimeline::AddStrip>
HRESULT CPrivateTimelineCtl::InsertStripAtPos(
		/* [in] */  IDMUSProdStrip*	pStrip,
		/* [in] */  DWORD		    dwPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pStrip != NULL );
	if( pStrip == NULL )
	{
		return E_POINTER;
	}

	DMUS_IO_TRACK_HEADER ioTrackHeader;
	ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );

	VARIANT varStripMgr;
	if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
	&&	(varStripMgr.vt == VT_UNKNOWN) && (V_UNKNOWN(&varStripMgr) != NULL) )
	{
		IDMUSProdStripMgr* pIStripMgr;
		if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
		{
			VARIANT varTrackHeader;
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				TRACE("Timeline: Unable to get StripMgr's TrackHeader\n");
			}

			pIStripMgr->Release();
		}
		V_UNKNOWN(&varStripMgr)->Release();
	}

	return InternalInsertStripAtPos( pStrip, dwPosition, ioTrackHeader.guidClassID, ioTrackHeader.dwGroup, ioTrackHeader.dwPosition );
}


//  @method HRESULT | IDMUSProdTimeline | StripToWindowPos | This method converts a point from strip
//		coordinates to Timeline Window coordinates
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip from whose coordinates to convert from.
//  @parm   POINT* | pPoint | Address of the point to convert.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pPoint> or <p pIStrip>
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::StripToWindowPos(
		/* [in] */		IDMUSProdStrip*	pIStrip,
		/* [in,out] */	POINT*			pPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( NULL == pIStrip || NULL == pPoint )
	{
		return E_POINTER;
	}

	if (m_pStripList == NULL)
	{
		return E_INVALIDARG;
	}

	StripList* pTempSL;
	// subtract the heights of any strips above this one
	for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pIStrip == pTempSL->m_pStrip )
		{
			return StripPointToClient( pTempSL, pPoint );
		}
	}
	return E_INVALIDARG;
}


//  @method HRESULT | IDMUSProdTimeline | AddToNotifyList | This method enables a StripMgr to receive
//		notifications of type <p rguidType> send to at least one group of <p dwGroupBits>
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to add.
//  @parm   REFGUID | rguidType | Reference to the identifier of the notification type to start
//		receiving notifications for.  See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
//  @parm   DWORD | dwGroupBits | Which track group(s) to receive notifications for.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To receive all notifications
//		of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStripMgr>
//	@rvalue E_OUTOFMEMORY | Not enough memory available
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::RemoveFromNotifyList>,
//		<om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CPrivateTimelineCtl::AddToNotifyList(
		/* [in] */	IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */	REFGUID				rguidType,
		/* [in] */	DWORD				dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pIStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;
	NotifyListEntry* pNotifyListEntry = NULL;

	try
	{
		// Look through m_lstNotifyEntry for guidNotify
		POSITION pos;
		pos = m_lstNotifyEntry.GetHeadPosition();
		while ( pos != NULL && !fFound )
		{
			pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
			if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
			{
				// Found guidNotify, add pNotifyListEntry to the end of the list
				fFound = TRUE;

				pNotifyListEntry = new NotifyListEntry( pIStripMgr, dwGroupBits );
				pNotifyEntry->m_lstNotifyListEntry.AddTail( pNotifyListEntry );
			}
		}

		// Didn't find guidNotify, add a new NotifyEntry to the end of m_lstNotifyEntry
		if ( !fFound )
		{
			pNotifyEntry = new NotifyEntry( pIStripMgr, rguidType, dwGroupBits );
			m_lstNotifyEntry.AddTail( pNotifyEntry );
		}
	}
	catch( CMemoryException *pMemoryException )
	{
		if( pNotifyListEntry )
		{
			delete pNotifyListEntry;
		}
		if( pNotifyEntry )
		{
			delete pNotifyEntry;
		}
		pMemoryException->Delete();
		return E_OUTOFMEMORY;
	}
		
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | RemoveFromNotifyList | This method stops a StripMgr from
//		receiving notifications of type <p rguidType> for groups in <p dwGroupBits>
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to remove.
//  @parm   REFGUID | rguidType |  Reference to the identifier of the notification type to stop
//		receiving notifications for.  See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
//  @parm   DWORD | dwGroupBits | Which track group(s) to stop receiving notifications for.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To stop receiving all notifications
//		of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStripMgr>
//	@rvalue E_INVALIDARG | The specified <p pIStripMgr> can not be found
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::AddToNotifyList>,
//		<om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CPrivateTimelineCtl::RemoveFromNotifyList(
		/* [in] */	IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */	REFGUID				rguidType,
		/* [in] */	DWORD				dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pIStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;

	// Look through m_lstNotifyEntry for guidNotify
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
		{
			// Found guidNotify, now find pIStripMgr
			NotifyListEntry* pNotifyListEntry = NULL;
			POSITION pos2;
			pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition( );
			while ( pos2 != NULL && !fFound )
			{
				POSITION pos3 = pos2;
				pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
				if( pNotifyListEntry->pIStripMgr == pIStripMgr &&
					pNotifyListEntry->dwGroupBits == dwGroupBits )
				{
					// We've found it, now remove it from the list and delete our entry
					fFound = TRUE;
					pNotifyEntry->m_lstNotifyListEntry.RemoveAt( pos3 );
					delete pNotifyListEntry;

					// Check if the list is empty
					if ( pNotifyEntry->m_lstNotifyListEntry.IsEmpty() )
					{
						// If the list is empty, remove it from m_lstNotifyEntry
						pos2 = m_lstNotifyEntry.Find( pNotifyEntry );
						ASSERT( pos2 );
						if ( pos2 )
						{
							m_lstNotifyEntry.RemoveAt( pos2 );
							delete pNotifyEntry;
						}
					}
				}
			}
		}
	}

	// Didn't find guidNotify, add a new NotifyEntry to the end of m_lstNotifyEntry
	if ( !fFound )
	{
		return E_INVALIDARG;
	}
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | NotifyStripMgrs | This method broadcasts the specified
//		notification to all registered strip managers that belong to at least one of <p dwGroupBits>
//
//  @parm   REFGUID | rguidType |  Reference to the identifier of the notification type to send.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
///  @parm   DWORD | dwGroupBits | Which track group(s) to notify.  A value of 0 is invalid. Each bit in
//		<p dwGroupBits> corresponds to a track group.  To notify all strip managers that asked to hear
//		notifications of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_FAIL | No matching strip managers were found, so no notifications were sent
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::AddToNotifyList>,
//		<om IDMUSProdTimeline::RemoveFromNotifyList>
HRESULT CPrivateTimelineCtl::NotifyStripMgrs(
		/* [in] */	REFGUID		rguidType,
		/* [in] */	DWORD		dwGroupBits,
		/* [in] */  void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// Handle the timeline-specific notifications
	if( m_pTimelineCtl )
	{
		m_pTimelineCtl->NotifyStripMgrs( rguidType, dwGroupBits, pData );
	}

	// Find the NotifyEntry corresponding to rguidType
	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
		{
			VARIANT var;

			BOOL fOrigFreezeUndo = FALSE;
			if( SUCCEEDED( GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fOrigFreezeUndo = V_BOOL(&var);
			}

			// Freeze undo queue
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			SetTimelineProperty( TP_FREEZE_UNDO, var );

			// Found the NotifyEntry corresponding to rguidType
			// Now, call OnUpdate for all matching StripMgrs
			HRESULT hr = E_FAIL;
			POSITION pos2;
			NotifyListEntry *pNotifyListEntry;
			pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition();
			while ( pos2 != NULL )
			{
				pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
				ASSERT( pNotifyListEntry );
				if ( pNotifyListEntry && (pNotifyListEntry->dwGroupBits & dwGroupBits) )
				{
					ASSERT( pNotifyListEntry->pIStripMgr );
					if( pNotifyListEntry->pIStripMgr )
					{
						fFound = TRUE;
						HRESULT hr2;
						hr2 = pNotifyListEntry->pIStripMgr->OnUpdate( rguidType, pNotifyListEntry->dwGroupBits & dwGroupBits, pData );
						if( (hr != S_OK) && (FAILED( hr ) || SUCCEEDED( hr2 )) )
						{
							hr = hr2;
						}
					}
				}
			}

			// Restore undo queue
			var.vt = VT_BOOL;
			V_BOOL(&var) = (short)fOrigFreezeUndo;
			SetTimelineProperty( TP_FREEZE_UNDO, var );

			return hr;
		}
	}
	return E_FAIL;
}


//  @method HRESULT | IDMUSProdTimeline | AllocTimelineDataObject | This method allocates an object that
//		implements the <i IDMUSProdTimelineDataObject> interface.
//
//  @parm	<i IDMUSProdTimelineDataObject> | ppITimelineDataObject | Address of a variable to receive a pointer
//		to the TimelineDataObject.
//
//	@comm	A Timeline DataObject simplifies dealing with the clipboard, and allows the Timeline to manage
//		multiple-strip copy and paste operations.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_OUTOFMEMORY | Not enough memory available to allocate a TimelineDataObject
//	@rvalue E_POINTER | <p ppITimelineDataObject> is NULL
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdTimelineDataObject>
HRESULT CPrivateTimelineCtl::AllocTimelineDataObject(
		/* [out,retval] */ IDMUSProdTimelineDataObject**	ppITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Validate ppITimelineDataObject
	if( ppITimelineDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize variables
	HRESULT hr = S_OK;
	CTimelineDataObject *pTimelineDataObject = NULL;

	// Try and allocate a CTimelineDataObject object
	try
	{
		pTimelineDataObject = new CTimelineDataObject;
	}
	// Catch out of memory exception
	catch( CMemoryException *pMemException )
	{
		hr = E_OUTOFMEMORY;
		pMemException->Delete();
	}

	// If successful, QI for an IDMUSProdTimelineDataObject to store in ppITimelineDataObject
	if( SUCCEEDED(hr) && pTimelineDataObject )
	{
		hr = pTimelineDataObject->QueryInterface( IID_IDMUSProdTimelineDataObject, (void**)ppITimelineDataObject );
		pTimelineDataObject->Release();
	}
	
	return hr;
}


//  @method HRESULT | IDMUSProdTimeline | GetPasteType | This method returns the type of Paste operation
//		to perform.
//
//  @parm   <t TIMELINE_PASTE_TYPE>* | ptlptPasteType | Address of a variable to store the
//		<t TIMELINE_PASTE_TYPE> in.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_POINTER | <p ptlptPasteType> is NULL
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::SetPasteType>
HRESULT CPrivateTimelineCtl::GetPasteType(
		/* [out,retval] */ TIMELINE_PASTE_TYPE*	ptlptPasteType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Validate ptlptPasteType
	if( ptlptPasteType == NULL )
	{
		return E_POINTER;
	}

	*ptlptPasteType = m_ptPasteType;

	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | SetPasteType | This method sets the type of Paste operation
//		to perform.
//
//  @parm   <t TIMELINE_PASTE_TYPE> | tlptPasteType | The type of <t TIMELINE_PASTE_TYPE> to set
//
//  @rvalue S_OK | The operation was successful
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::GetPasteType>
HRESULT CPrivateTimelineCtl::SetPasteType(
		/* [in] */ TIMELINE_PASTE_TYPE tlptPasteType )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ptPasteType = tlptPasteType;

	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | SetMarkerTime | This method sets the location of
//		one of the markers in the time strip.
//
//  @parm   <t MARKERID> | idMarkerType | Which marker to get the location of.  Must be
//		one of <t MARKERID>.  MARKER_LEFTDISPLAY and MARKER_RIGHTDISPLAY are not supported.
//  @parm   <t TIMETYPE> | ttType | Which units to use when computing <p plTime>.  Must be
//		one of <t TIMETYPE>.
//  @parm   long | lTime | The time the marker should be set to.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p ttType> or <p idMarkerType> do not contain valid values, or
//		lTime is less than zero.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMETYPE>, <t MARKERID>, <om IDMUSProdTimeline::GetMarkerTime>
HRESULT CPrivateTimelineCtl::SetMarkerTime( 
    /* [in] */ MARKERID idMarkerType,
	/* [in] */ TIMETYPE	ttType,
    /* [in] */ long lTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimelineCtl )
	{
		return m_pTimelineCtl->SetMarkerTime( idMarkerType, ttType, lTime );
	}

	return E_FAIL;
}

//  @method HRESULT | IDMUSProdTimeline | GetMarkerTime | This method gets the location of
//		one of the markers in the time strip.
//
//  @parm   <t MARKERID> | idMarkerType | Which marker to get the location of.  Must be
//		one of <t MARKERID>
//  @parm   <t TIMETYPE> | ttType | Which units to use when computing <p plTime>.  Must be
//		one of <t TIMETYPE>.
//  @parm   long* | plTime | Address of a variable to receive the location of <p idMarkerType>.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p plTime> is NULL.
//	@rvalue E_INVALIDARG | <p ttType> or <p idMarkerType> do not contain valid values.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMETYPE>, <t MARKERID>, <om IDMUSProdTimeline::SetMarkerTime>
HRESULT CPrivateTimelineCtl::GetMarkerTime( 
    /* [in] */ MARKERID idMarkerType,
	/* [in] */ TIMETYPE	ttType,
    /* [out] */ long *plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimelineCtl )
	{
		return m_pTimelineCtl->GetMarkerTime( idMarkerType, ttType, plTime );
	}

	return E_FAIL;
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToPosition | This method converts
//		from a time in clocks to a horizontal pixel position
//
//	@comm	Negative values for <p lTime> are valid.
//
//	@comm	Due to rounding errors, converting from a time to a pixel position and back
//		will usually return a value different from the original one.
//
//  @parm   long | lTime | The time in clocks
//  @parm   long* | plPosition | Address of a variable to receive the pixel position this
//		time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plPosition>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToClocks>
HRESULT CPrivateTimelineCtl::ClocksToPosition( 
    /* [in] */ long   lTime,
    /* [out] */ long *plPosition)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == plPosition )
	{
		return E_POINTER;
	}
	if (m_fMusicTime == TRUE) 
	{
		*plPosition = (long)floor( double(lTime) * m_dblZoom + 0.5 );
		return S_OK;
	}
	else
	{
		REFERENCE_TIME rTime;
		HRESULT hr = ClocksToRefTime( lTime, &rTime);
		if (SUCCEEDED(hr))
		{
			return RefTimeToPosition( rTime, plPosition);
		}
		return hr;
	}
}

//  @method HRESULT | IDMUSProdTimeline | PositionToClocks | This method converts
//		from a horizontal pixel position to a time in clocks.
//
//	@comm	Negative values for <p lPosition> are valid.
//
//	@comm	Due to rounding errors, converting from a pixel position to a time and back
//		may return a value different from the original one.
//
//  @parm   long | lPosition | The horizontal position, in pixels.
//  @parm   long* | plTime | Address of a variable to receive the time in clocks this
//		position resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plTime>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToPosition>
HRESULT CPrivateTimelineCtl::PositionToClocks( 
    /* [in] */ long position,
    /* [out] */ long *plTime)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == plTime )
	{
		return E_POINTER;
	}
	if (m_fMusicTime == TRUE)
	{
		double dblPosition;
		double dblTime;
		dblPosition = position;
		dblTime = dblPosition / m_dblZoom;
		*plTime = (long)floor(dblTime + 0.5);
		return S_OK;
	}
	else
	{
		REFERENCE_TIME rTime;
		HRESULT hr = PositionToRefTime( position, &rTime);
		if (SUCCEEDED(hr))
		{
			return RefTimeToClocks( rTime, plTime);
		}
		return hr;
	}
}

static HRESULT GetVT_I4( VARIANT var, long* plVal )
{
	if( var.vt != VT_I4 )
		return E_FAIL;
	*plVal = V_I4(&var);
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | SetTimelineProperty | This method sets a property of
//		the Timeline.
//
//	@comm	The TP_SNAPAMOUNT property is not supported.
//
//  @parm   <t TIMELINE_PROPERTY> | tp | Which property to get.  Must be one of <t TIMELINE_PROPERTY>.
//  @parm   VARIANT | var | The data to set the property with.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p tp> does not contain a valid property type.
//	@rvalue E_FAIL | <p var> contained an invalid value for the specified property type.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMELINE_PROPERTY>, <om IDMUSProdTimeline::GetTimelineProperty>
HRESULT CPrivateTimelineCtl::SetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [in] */ VARIANT				var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Handle the timeline-specific properties
	if( m_pTimelineCtl
	&&	SUCCEEDED( m_pTimelineCtl->SetTimelineProperty( tp, var ) ) )
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;
	long	lVal;

	switch(tp)
	{
	case TP_CLOCKLENGTH:
		if( SUCCEEDED( hr = GetVT_I4( var, &lVal )))
		{
			if( (lVal >= 0) && (m_lLength != lVal) )
			{
				m_lLength = lVal;
				if( m_pTimelineCtl )
				{
					m_pTimelineCtl->OptimizeZoomFactor();
					m_pTimelineCtl->ComputeScrollBars();
				}

				// send WM_SIZE to all strips, letting them know they have changed size
				StripList* pSL = m_pStripList;
				while (pSL)
				{
					// BUGBUG: Should have meaningful values for lParam.
					CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
					pSL = pSL->m_pNext;
				}

				hr = S_OK;
			}
		}
		break;

	case TP_ACTIVESTRIP:
		{
			hr = E_INVALIDARG;
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					IDMUSProdStrip* pIStrip;
					if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdStrip, (void**)&pIStrip ) ) )
					{
						StripList* pSL = FindStripList( pIStrip );
						if( pSL )
						{
							ActivateStripList( pSL );
							hr = S_OK;
						}
						pIStrip->Release();
					}
					punk->Release();
				}
				else
				{
					DeactivateStripList();
					hr = S_OK;
				}
			}
		}
		break;

	case TP_DMUSPRODFRAMEWORK:
		{
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				if( m_pDMUSProdFramework )
				{
					m_pDMUSProdFramework->Release();
					m_pDMUSProdFramework = NULL;
				}
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMUSProdFramework ) ) )
					{
						IDMUSProdComponent* pIComponent = NULL;
						if( SUCCEEDED ( m_pDMUSProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
						{
							IDMUSProdConductor *pIConductor;
							if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
							{
								if( m_pIDMPerformance )
								{
									m_pIDMPerformance->Release();
									m_pIDMPerformance = NULL;
								}
								IUnknown* punkPerformance;
								if( SUCCEEDED( pIConductor->GetPerformanceEngine( &punkPerformance ) ) )
								{
									punkPerformance->QueryInterface( IID_IDirectMusicPerformance, (void **)&m_pIDMPerformance ) ;
									punkPerformance->Release();
								}
								pIConductor->Release();
							}
							pIComponent->Release();
						}
					}
				}
				hr = S_OK;
			}
		}
		break;

	case TP_TIMELINECALLBACK:
		{
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				if( m_pTimelineCallback )
				{
					m_pTimelineCallback->Release();
					m_pTimelineCallback = NULL;
				}
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					punk->QueryInterface( IID_IDMUSProdTimelineCallback, (void**)&m_pTimelineCallback );
				}
				hr = S_OK;
			}
		}
		break;

	case TP_ZOOM:
		if( var.vt == VT_R8 )
		{
			if ( V_R8(&var) > 0 )
			{
				m_dblZoom = V_R8(&var);
				hr = S_OK;
			}
		}
		break;

	case TP_FREEZE_UNDO:
		if( var.vt == VT_BOOL )
		{
			m_fFreezeUndo = V_BOOL(&var);
			hr = S_OK;
		}
		break;

	case TP_SNAP_TO:
		if( var.vt == VT_I4 )
		{
			m_pTimeStrip->SetSnapTo( (DMUSPROD_TIMELINE_SNAP_TO) V_I4(&var) );
			hr = S_OK;
		}
		break;

	case TP_HORIZONTAL_SCROLL:
		if( var.vt == VT_I4 )
		{
			hr = S_OK;
			if( m_lXScrollPos != V_I4(&var) )
			{
				if( m_pTimelineCtl )
				{
					m_pTimelineCtl->ScrollToPosition( V_I4(&var) );
				}
				else
				{
					m_lXScrollPos = V_I4(&var);
				}
			}
		}
		break;

	case TP_VERTICAL_SCROLL:
		if( var.vt == VT_I4 )
		{
			hr = S_OK;
			if( m_lYScrollPos != V_I4(&var) )
			{
				if( m_pTimelineCtl )
				{
					// Get the height of all strips
					long lMaxStripScroll = TotalStripHeight();

					RECT rect;
					m_pTimelineCtl->GetClientRect( &rect );

					RECT rectTmp;
					m_pTimelineCtl->m_ScrollHorizontal.GetClientRect( &rectTmp );
					rect.bottom -= rectTmp.bottom;

					if( m_pStripList )
					{
						// don't scroll top strip (usually the time strip)
						rect.top += m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH*2;

						// subtract off the top strip
						lMaxStripScroll -= m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH;
					}

					// subtract the height of the viewing region
					lMaxStripScroll -= rect.bottom - rect.top;

					int nPos;
					if (lMaxStripScroll < 1)
					{
						nPos = 0;
					}
					else
					{
						nPos = (V_I4(&var) * m_pTimelineCtl->m_ScrollVertical.GetScrollLimit()) / lMaxStripScroll;
					}

					int iTemp = 0;
					m_pTimelineCtl->OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, nPos ), (LPARAM) ((HWND) m_pTimelineCtl->m_ScrollVertical), iTemp );
				}
				else
				{
					m_lYScrollPos = V_I4(&var);
				}
			}
		}
		break;

	case TP_FUNCTIONBAR_WIDTH:
		if( var.vt == VT_I4 )
		{
			if(( V_I4(&var) >= MIN_FNBAR_WIDTH ) && ( V_I4(&var) <= MAX_FNBAR_WIDTH))
			{
				m_lFunctionBarWidth = V_I4(&var);
				hr = S_OK;

				// Probably should calculate the area that will be affected
				// and only invalidate it.
				// Perhaps we could just 'scroll' the affected area to the
				// right and invalidate the small new area to be displayed

				// send WM_SIZE to all strips, letting them know something has changed size
				StripList* pSL = m_pStripList;
				while (pSL)
				{
					// BUGBUG: Should have meaningful values for lParam.
					CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
					pSL = pSL->m_pNext;
				}

				if( m_pTimelineCtl )
				{
					m_pTimelineCtl->ComputeScrollBars();
					RECT rect;
					m_pTimelineCtl->GetClientRect( &rect );
					m_pTimelineCtl->InvalidateRect( &rect, FALSE );
				}
			}
		}
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | GetTimelineProperty | This method gets a property of
//		the Timeline.
//
//  @parm   <t TIMELINE_PROPERTY> | tp | Which property to get.  Must be one of <t TIMELINE_PROPERTY>.
//  @parm   VARIANT* | pVar | Address of the variant to return the property's data in.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pVar> is NULL.
//	@rvalue E_INVALIDARG | <p tp> does not contain a valid property type.
//
//	@xref	<i DMUSProdTimeline>, <t TIMELINE_PROPERTY>, <om IDMUSProdTimeline::SetTimelineProperty>
HRESULT CPrivateTimelineCtl::GetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [out] */ VARIANT*			pVar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pVar == NULL )
	{
		return E_POINTER;
	}

	// Handle the timeline-specific properties
	if( m_pTimelineCtl
	&&	SUCCEEDED( m_pTimelineCtl->GetTimelineProperty( tp, pVar ) ) )
	{
		return S_OK;
	}

	HRESULT hr = S_OK;

	switch(tp)
	{
	case TP_CLOCKLENGTH:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lLength;
		break;

	case TP_TIMELINECALLBACK:
		pVar->vt = VT_UNKNOWN;
		V_UNKNOWN(pVar) = m_pTimelineCallback;
		if( m_pTimelineCallback )
		{
			m_pTimelineCallback->AddRef();
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_DMUSPRODFRAMEWORK:
		pVar->vt = VT_UNKNOWN;
		V_UNKNOWN(pVar) = m_pDMUSProdFramework;
		if( m_pDMUSProdFramework )
		{
			m_pDMUSProdFramework->AddRef();
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_ACTIVESTRIP:
		pVar->vt = VT_UNKNOWN;
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip )
		{
			V_UNKNOWN(pVar) = m_pActiveStripList->m_pStrip;
			m_pActiveStripList->m_pStrip->AddRef();
		}
		else
		{
			V_UNKNOWN(pVar) = NULL;
			return E_FAIL;
		}
		break;
	
	case TP_FUNCTIONBAR_WIDTH:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lFunctionBarWidth;
		break;

	case TP_HORIZONTAL_SCROLL:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lXScrollPos;
		break;

	case TP_VERTICAL_SCROLL:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lYScrollPos;
		break;

	case TP_ZOOM:
		pVar->vt = VT_R8;
		V_R8(pVar) = m_dblZoom;
		break;

	case TP_SNAPAMOUNT:
		if( m_pTimeStrip )
		{
			V_I4(pVar) = m_pTimeStrip->SnapAmount( V_I4(pVar) );
			pVar->vt = VT_I4;
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_FREEZE_UNDO:
		pVar->vt = VT_BOOL;
		V_BOOL(pVar) = (short)m_fFreezeUndo;
		break;

	case TP_SNAP_TO:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_pTimeStrip->m_stSetting;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | Refresh | This method causes a redraw of the
//		entire Timeline.
//
//	@comm	This method should be used sparingly.  If at all possible,
//		<om IDMUSProdTimeline::StripInvalidateRect> and <om IDMUSProdTimeline::NotifyStripMgrs>
//		should be used instead.
//
//  @rvalue S_OK | The operation was successful
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::StripInvalidateRect>, <om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CPrivateTimelineCtl::Refresh(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimelineCtl == NULL
	||	!::IsWindow(m_pTimelineCtl->m_hWnd) )
	{
		// Our window has been destroyed - fail.
		return E_FAIL;
	}

	return m_pTimelineCtl->Refresh();
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToMeasureBeat | This method converts
//		from a time in clocks to a time in measures and beats
//
//	@comm	If <p lTime> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plMeasure> and <p plBeat>.  <p plMeasure> will
//			contain the negative measure number, and <p plBeat> will contain
//			the beat in the measure that <p lTime> falls in.
//
//	@comm	Either <p plMeasure> or <p lBeat> may be NULL, but not both.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lTime | The time in clocks
//  @parm   long* | plMeasure | Address of a variable to recieve the measure number, with
//		measure 0 as the first measure.
//  @parm   long* | plBeat | Address of a variable to receive the beat number, with beat
//		0 as the first beat in each measure.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plMeasure> and <p plBeat>
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToClocks>
HRESULT CPrivateTimelineCtl::ClocksToMeasureBeat( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long	  lTime,
    /* [out] */ long *plMeasure,
    /* [out] */ long *plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (NULL == plMeasure) && (NULL == plBeat) )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext;
	long lBeat = 0, lMeasure = 0;

	do
	{
		// Try and get the current time signature
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig ) ) )
		{
			return E_UNEXPECTED;
		}

		// If lTime is less than 0, only use the first TimeSig
		if( lTime < 0 )
		{
			lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			lTime = -(abs(lTime) % (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ )));
			break;
		}
		// If there is no next time signature, do the math to find how many more measures to add
		else if( mtTSNext == 0 )
		{
			lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			lTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
			break;
		}
		// Otherwise it's more complicated
		else
		{
			// If the next time signature is after the time we're looking for
			if( lTime < mtTSNext )
			{
				// Add the number of complete measures between here and there
				lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// lTime now stores an offset from the beginning of the measure
				lTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
				break;
			}
			// The next time signature is before the time we're looking for
			else
			{
				// Compute how many complete measures there are between now and the next Time signature
				long lMeasureDiff= mtTSNext / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// Add them to lMeasure
				lMeasure += lMeasureDiff;

				// Change lMeasureDiff from measures to clocks
				lMeasureDiff *= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

				// Subtract from the time left (lTime) and add to the current time (mtTSCur)
				lTime -= lMeasureDiff;
				mtTSCur += lMeasureDiff;
			}
		}
	}
	// While the time left is greater than 0
	while ( lTime > 0 );

	if ( lTime < 0 )
	{
		lTime += TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		lMeasure--;
	}

	if ( lTime != 0 && plBeat != NULL )
	{
		lBeat = lTime / NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
	}
	else
	{
		lBeat = 0;
	}

	if( plMeasure != NULL )
	{
		*plMeasure = lMeasure;
	}
	if( plBeat != NULL )
	{
		*plBeat = lBeat;
	}

	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | PositionToMeasureBeat | This method converts
//		from a horizontal pixel position to a measure and beat value.
//
//	@comm	If <p lPosition> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plMeasure> and <p plBeat>.  <p plMeasure> will
//			contain the negative measure number, and <p plBeat> will contain
//			the beat in the measure that <p lTime> falls in.
//
//	@comm	Either <p plMeasure> or <p lBeat> may be NULL, but not both.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lPosition | The horizontal pixel position.
//  @parm   long* | plMeasure | Address of a variable to receive the measure number, with
//		measure 0 as the first measure.
//  @parm   long* | plBeat | Address of a variable to receive the beat number, with beat
//		0 as the first beat in each measure.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plMeasure> and <p plBeat>
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToPosition>
HRESULT CPrivateTimelineCtl::PositionToMeasureBeat( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long   position,
    /* [out] */ long *plMeasure,
    /* [out] */ long *plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long lTime;
	HRESULT hr;

	// Since this uses PositionToClocks, we don't care if we're in music time
	// or real time.  PositionToClocks will deal with it appropriately.
	if( SUCCEEDED( hr = PositionToClocks( position, &lTime ) ))
	{
		hr = ClocksToMeasureBeat( dwGroupBits, dwIndex, lTime, plMeasure, plBeat );
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToClocks | This method converts
//		from a measure and beat to a time in clocks.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plTime>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm	long | lMeasure | The measure number, with measure 0 as the first measure.
//  @parm	long | lBeat | The beat number, with beat 0 as the first beat in each measure.
//  @parm	long* | plTime | Address of a variable to receive the converted time in clocks.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p plTime>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToMeasureBeat>
HRESULT CPrivateTimelineCtl::MeasureBeatToClocks( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long lMeasure,
    /* [in] */ long lBeat,
    /* [out] */ long *plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == plTime )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	HRESULT hr;
	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext = 1;
	do
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig );
		if ( FAILED( hr ) )
		{
			return E_UNEXPECTED;
		}

		long lMeasureClocks = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		if( mtTSNext == 0 )
		{
			mtTSCur += lMeasureClocks * lMeasure;
			break;
		}
		else
		{
			long lTmpMeasures = mtTSNext / lMeasureClocks;
			if( lMeasure <= lTmpMeasures )
			{
				mtTSCur += lMeasureClocks * lMeasure;
				break;
			}
			else
			{
				mtTSCur += lMeasureClocks * lTmpMeasures;
				lMeasure -= lTmpMeasures;
			}
		}
	}
	while( lMeasure > 0 );

	if( lBeat >= 0 )
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max(mtTSCur, 0), NULL, &TimeSig );
	}
	else
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max(mtTSCur - TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ), 0), NULL, &TimeSig );
	}
	if ( FAILED( hr ) )
	{
		return E_UNEXPECTED;
	}

	mtTSCur += NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ) * lBeat;
	*plTime = mtTSCur;
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToPosition | This method converts
//		from a measure and beat to a pixel position.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plPosition>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm	long | lMeasure | The measure number, with measure 0 as the first measure.
//  @parm	long | lBeat | The beat number, with beat 0 as the first beat in each measure.
//  @parm	long* | plPosition | Address of a variable to receive the pixel position.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p plPosition>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToMeasureBeat>
HRESULT CPrivateTimelineCtl::MeasureBeatToPosition( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long   lMeasure,
    /* [in] */ long   lBeat,
    /* [out] */ long *pPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long lTime;
	HRESULT hr;
	// Since this uses ClocksToPosition, we don't care if we're in music time
	// or real time.  ClocksToPosition will deal with it appropriately.
	if( SUCCEEDED( hr = MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, lBeat, &lTime ) ))
	{
		hr = ClocksToPosition( lTime, pPosition );
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | StripInvalidateRect | This method invalidates the
//		specified rectangle in a strip, causing the area to be redrawn.
//
//	@parm	<i IDMUSProdStrip>* | pStrip | Which strip to invalidate the rectangle in.
//  @parm   RECT* | pRect | The rectangle to invalidate.  If NULL, invalidate the entire strip.
//  @parm   BOOL | fErase | If TRUE, erase the background of the rectangle first.
//
//	@comm	The parameter <p fErase> is ignore, as the background of the strip is always filled
//		before <om IDMUSProdStrip::Draw> is called.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p pStrip> was not added to the timeline by calling one of
//		<om IDMUSProdTimeline::AddStrip>, <om IDMUSProdTimeline::InsertStripAtDefaultPos>, or
//		<om IDMUSProdTimeline::InsertStripAtPos>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::Refresh>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::StripInvalidateRect(
	/* [in] */ IDMUSProdStrip*	pStrip,
	/* [in] */ RECT*			pRect,
	/* [in] */ BOOL				fErase)
{
	UNREFERENCED_PARAMETER( fErase );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimelineCtl == NULL
	||	!::IsWindow(m_pTimelineCtl->m_hWnd) )
	{
		// Our window has been destroyed - fail.
		return E_FAIL;
	}

	return m_pTimelineCtl->StripInvalidateRect( pStrip, pRect, fErase );
}

//  @method HRESULT | IDMUSProdTimeline | RemovePropertyPageObject | This method removes
//		a property page object previously set by <om IDMUSProdTimeline::SetPropertyPage>
//
//	@comm <t TP_DMUSPRODFRAMEWORK> must be set to the DirectMusic Producer framework
//		object (via a call to <om IDMUSProdTimeline::SetTimelineProperty> prior to calling
//		this method.
//
//  @parm   IUnknown* | punkPropPageObj | Reference to the <i IDMUSProdPropPageObject>
//		to remove from the current property sheet.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p punkPropPageObj>
//	@rvalue E_FAIL | <t TP_DMUSPRODFRAMEWORK> was not set previous to calling this method.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdPropPageObject>,	<om IDMUSProdTimeline::SetPropertyPage>
HRESULT CPrivateTimelineCtl::RemovePropertyPageObject(	/* [in] */ IUnknown* punkPropPageObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( punkPropPageObj == NULL )
	{
		return E_POINTER;
	}

	if ( m_pDMUSProdFramework == NULL )
	{
		return E_FAIL;
	}

	IDMUSProdPropPageObject* pPPO;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED(hr = punkPropPageObj->QueryInterface( IID_IDMUSProdPropPageObject,
		(void**)&pPPO )))
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pDMUSProdFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			pJPS->RemovePageManagerByObject( pPPO );
			pJPS->Release();
		}
		pPPO->Release();
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | SetPropertyPage | This method changes the
//		currently displayed <i IDMUSProdPropSheet> to refer to <p punkPropPageMgr>
//		and <p punkPropPageObj>.
//
//	@comm <t TP_DMUSPRODFRAMEWORK> must be set to the DirectMusic Producer framework
//		object (via a call to <om IDMUSProdTimeline::SetTimelineProperty> prior to calling
//		this method.
//
//  @parm   IUnknown* | punkPropPageMgr | Reference to the <i IDMUSProdPropPageManager>
//		to set for the currently displayed property sheet.
//  @parm   IUnknown* | punkPropPageObj | Reference to the <i IDMUSProdPropPageObject>
//		to set for the currently displayed property sheet.
//
//	@rdesc	If the property sheet is hidden, this method returns S_FALSE and does not
//		set either <p punkPropPageMgr> or <p punkPropPageObj>.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue S_FALSE | The current property sheet is hidden.
//  @rvalue E_POINTER | NULL was passed as  <p punkPropPageObj> or <p punkPropPageMgr>
//	@rvalue E_FAIL | <t TP_DMUSPRODFRAMEWORK> was not set previous to calling this method.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>,
//		<i IDMUSProdPropSheet>, <om IDMUSProdTimeline::RemovePropertyPageObject>
HRESULT CPrivateTimelineCtl::SetPropertyPage(
		/* [in] */ IUnknown* punkPropPageMgr,
		/* [in] */ IUnknown* punkPropPageObj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !punkPropPageMgr || !punkPropPageObj )
	{
		return E_POINTER;
	}

	IDMUSProdPropPageManager* pPPM;
	IDMUSProdPropPageObject* pPPO;
	HRESULT hr = E_FAIL;

	if( m_pDMUSProdFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pDMUSProdFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			//  If the property sheet is hidden, exit
			if( pJPS->IsShowing() != S_OK )
			{
				pJPS->Release();
				return S_FALSE;
			}
			if( SUCCEEDED(hr = punkPropPageMgr->QueryInterface( IID_IDMUSProdPropPageManager, 
				(void**)&pPPM )))
			{
				if( SUCCEEDED(hr = punkPropPageObj->QueryInterface( IID_IDMUSProdPropPageObject,
					(void**)&pPPO )))
				{
					HWND hwnd = ::GetFocus();	// save window to setfocus to afterwards if necessary
					if( SUCCEEDED(pJPS->SetPageManager( pPPM )))
					{
						pPPM->SetObject(pPPO);
// don't open property page automatically	pJPS->Show(TRUE);
					}
					HWND hwnd2 = ::GetFocus();
					if(hwnd != hwnd2)
					{
						::SetFocus(hwnd);
					}
					pPPO->Release();
				}
				pPPM->Release();
			}
			pJPS->Release();
		}
	}
	return hr;
}

StripList* CPrivateTimelineCtl::FindStripList(IDMUSProdStrip* pStrip) const
{
	StripList* pSL;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_pStrip == pStrip )
		{
			return pSL;
		}
	}
	return NULL;
}

//  @method HRESULT | IDMUSProdTimeline | StripSetTimelineProperty | This method sets a strip property
//		that is controlled by the Timeline.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to set the property for.
//  @parm   <t STRIP_TIMELINE_PROPERTY> | stp | Which property to set.  Must be one of <t STRIP_TIMELINE_PROPERTY>.
//  @parm   VARIANT | variant | The data to set the property with.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pIStrip> is NULL.
//	@rvalue E_INVALIDARG | <p stp> does not contain a valid property type, <p variant>
//		contains invalid data for the specified property type, or <p pIStrip> was not previously
//		added to the Timeline.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <t STRIP_TIMELINE_PROPERTY>,
//		<om IDMUSProdTimeline::StripGetTimelineProperty>
HRESULT CPrivateTimelineCtl::StripSetTimelineProperty( 
    /* [in] */ IDMUSProdStrip* pIStrip,
    /* [in] */ STRIP_TIMELINE_PROPERTY stp,
    /* [in] */ VARIANT variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_INVALIDARG;

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}

	StripList* pSL = FindStripList(pIStrip);
	if( NULL == pSL )
	{
		return E_INVALIDARG;
	}

	switch( stp )
	{
	case STP_VERTICAL_SCROLL:
		{
				long lScrollAmount = pSL->m_lVScroll - V_I4(&variant);
				RECT rect;

				m_pTimelineCtl->GetStripClientRect(pSL, &rect);
				if (( lScrollAmount >= rect.bottom - rect.top ) || ( -lScrollAmount >= rect.bottom - rect.top ))
				{
					pSL->m_lVScroll = V_I4(&variant);
					StripInvalidateRect( pIStrip, NULL, TRUE );
				}
				else
				{
					// Scroll strip
					pSL->m_lVScroll = V_I4(&variant);

					if( m_pTimelineCtl )
					{
						m_pTimelineCtl->ScrollWindow( 0, lScrollAmount, &rect, &rect );
						if (lScrollAmount < 0) // scroll up
						{
							// bottom is invalidated automatically by ScrollWindow()
							RECT oldRect = rect;

							// invalidate region displaying the track's title
							rect.bottom = rect.top + FUNCTION_NAME_HEIGHT;
							rect.right = rect.left + m_lFunctionBarWidth;
							// Don't need to erase, since the title will overwrite it anyways
							m_pTimelineCtl->InvalidateRect( &rect, FALSE);

							// invalidate region displaying the track's minize icon
							VARIANT var;
							if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
							{
								if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
								{
									if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
									{
										rect.bottom += m_pTimelineCtl->m_sizeMinMaxButton.cy - FUNCTION_NAME_HEIGHT;
										rect.right = oldRect.right;
										long posLength;
										ClocksToPosition( m_lLength, &posLength );
										posLength++;
										if( posLength < rect.right - m_lFunctionBarWidth)
										{
											rect.right = posLength + m_lFunctionBarWidth;
										}
										rect.left = rect.right - m_pTimelineCtl->m_sizeMinMaxButton.cx;
										// Don't need to erase, since the button will overwrite it anyways
										m_pTimelineCtl->InvalidateRect( &rect, FALSE);
									}
								}
							}
						}
						else // scroll down
						{
							RECT oldRect = rect;
							// top is invalidated automatically by ScrollWindow()

							// invalidate region displaying the track's title
							rect.top += lScrollAmount;
							rect.bottom = rect.top + FUNCTION_NAME_HEIGHT;
							rect.right = rect.left + m_lFunctionBarWidth;
							m_pTimelineCtl->InvalidateRect( &rect, FALSE);

							// invalidate region displaying the track's minize icon
							VARIANT var;
							if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
							{
								if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
								{
									if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
									{
										rect.bottom += m_pTimelineCtl->m_sizeMinMaxButton.cy - FUNCTION_NAME_HEIGHT;
										rect.right = oldRect.right;
										long posLength;
										ClocksToPosition( m_lLength, &posLength );
										posLength++;
										if( posLength < rect.right - m_lFunctionBarWidth)
										{
											rect.right = posLength + m_lFunctionBarWidth;
										}
										rect.left = rect.right - m_pTimelineCtl->m_sizeMinMaxButton.cx;
										m_pTimelineCtl->InvalidateRect( &rect, FALSE);
									}
								}
							}
						}
					}
				}
		}
		break;

	case STP_HEIGHT:
		if (variant.vt != VT_I4)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			hr = S_OK;
			if ( pSL->m_sv == SV_NORMAL )
			{
				if( pSL->m_lHeight != V_I4(&variant) )
				{
					pSL->m_lHeight = V_I4(&variant);

					// Send WM_MOVE messages to all strips (below this one) that were moved
					if( m_pTimelineCtl
					&&	::IsWindow(m_pTimelineCtl->m_hWnd) )
					{
						StripList* pTemp = pSL->m_pNext;
						while( pTemp )
						{
							pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
							pTemp = pTemp->m_pNext;
						}
					}

					if( m_pTimelineCtl )
					{
						RECT rectWin, rect;
						m_pTimelineCtl->GetClientRect( &rectWin );
						m_pTimelineCtl->GetStripClientRect( pSL, &rect );
						rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
						m_pTimelineCtl->InvalidateRect( &rectWin, FALSE );
						m_pTimelineCtl->ComputeScrollBars();
						// If, as a result of the strip resize, the total height of the strips is less than the
						// height of the window, scroll back to the top.
						if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
						{
							int iTemp = 0;
							m_pTimelineCtl->OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_pTimelineCtl->m_ScrollVertical), iTemp );
						}
					}
				}
			}
			else if ( pSL->m_sv == SV_MINIMIZED )
			{
				pSL->m_lRestoreHeight = V_I4(&variant);
			}
			else
			{
				hr = E_UNEXPECTED;
			}
		}
		break;

	case STP_STRIPVIEW:
		if (variant.vt != VT_I4)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			if( (V_I4(&variant) <= (int) SV_FUNCTIONBAR_MINIMIZED))
			{
				if ( pSL->m_sv != (STRIPVIEW) V_I4(&variant) )
				{
					if ( (STRIPVIEW) V_I4(&variant) == SV_MINIMIZED)
					{
						pSL->m_lRestoreHeight = pSL->m_lHeight;
						pSL->m_sv = SV_MINIMIZED;
						VARIANT var;
						if (SUCCEEDED (pIStrip->GetStripProperty( SP_MINIMIZE_HEIGHT, &var)))
						{
							pSL->m_lHeight = V_INT(&var);
						}
						else
						{
							pSL->m_lHeight = MIN_STRIP_HEIGHT;
						}
					}
					else if ( (STRIPVIEW) V_I4(&variant) == SV_NORMAL)
					{
						pSL->m_lHeight = pSL->m_lRestoreHeight;
						pSL->m_sv = SV_NORMAL;
					}

					// Send WM_MOVE messages to all strips (below this one) that were moved
					if( m_pTimelineCtl
					&&	::IsWindow(m_pTimelineCtl->m_hWnd) )
					{
						StripList* pTemp = pSL->m_pNext;
						while( pTemp )
						{
							pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
							pTemp = pTemp->m_pNext;
						}
					}
	
					if( m_pTimelineCtl )
					{
						RECT rectWin, rect;
						m_pTimelineCtl->GetClientRect( &rectWin );
						m_pTimelineCtl->GetStripClientRect( pSL, &rect );
						rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
						m_pTimelineCtl->InvalidateRect( &rectWin, FALSE );
						m_pTimelineCtl->ComputeScrollBars();
						// If, as a result of the strip resize, the total height of the strips is less than the
						// height of the window, scroll back to the top.
						if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
						{
							int iTemp = 0;
							m_pTimelineCtl->OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_pTimelineCtl->m_ScrollVertical), iTemp );
						}
					}
				}
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		break;

	case STP_GUTTER_SELECTED:
		if (variant.vt != VT_BOOL)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			if( (V_BOOL(&variant) != pSL->m_fSelected) )
			{
				// This code is duplicated in SetStripGutter
				// Change its selection
				pSL->m_fSelected = V_BOOL(&variant);

				if( m_pTimelineCtl )
				{
					// Update the gutter display
					RECT rect;
					m_pTimelineCtl->GetStripClientRect( pSL, &rect );
					rect.left = 0;
					rect.right = rect.left + GUTTER_WIDTH;
					m_pTimelineCtl->InvalidateRect( &rect, FALSE );
				}

				// Notify the strip
				/*
				var.vt = VT_BOOL;
				V_BOOL(&var) = (short)m_pMouseStripList->m_fSelected;
				m_pMouseStripList->m_pStrip->SetStripProperty( SP_GUTTERSELECT, var );
				*/
				hr = S_OK;
			}
		}
		break;

	case STP_STRIP_INFO:
		if (variant.vt != VT_BYREF)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			DMUSPROD_TIMELINE_STRIP_INFO *pDMUSPROD_TIMELINE_STRIP_INFO = (DMUSPROD_TIMELINE_STRIP_INFO *)V_BYREF( &variant );
			if( NULL == pDMUSPROD_TIMELINE_STRIP_INFO )
			{
				hr = E_POINTER;
			}
			else
			{
				pSL->m_clsidType = pDMUSPROD_TIMELINE_STRIP_INFO->clsidType;
				pSL->m_dwGroupBits = pDMUSPROD_TIMELINE_STRIP_INFO->dwGroupBits;
				pSL->m_dwIndex = pDMUSPROD_TIMELINE_STRIP_INFO->dwIndex;
			}
		}
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | OnDataChanged | This method calls the registered
//		<om IDMUSProdTimelineCallback::OnDataChanged> method with <p punk> as a
//		parameter.
//
//	@comm	When used in the Segment Designer, an <i IDMUSProdStripMgr> interface must be passed
//		in <p punk>.  The Segment Designer will then call the strip manager's <om IStream::Save>
//		method to retrieve the new data.
//
//  @parm   IUnknown* | punk | The interface to pass.
//
//	@rdesc	If there is an <om IDMUSProdTimelineCallback::OnDataChanged> method registered, 
//		the return value is the value returned by <om IDMUSProdTimelineCallback::OnDataChanged>.
//		Otherwise, the return value is E_FAIL.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimelineCallback::OnDataChanged>
HRESULT CPrivateTimelineCtl::OnDataChanged( IUnknown* punk )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_FAIL;

	if( m_pTimelineCallback )
	{
		EnterCriticalSection( &m_csOnDataChanged );
		hr = m_pTimelineCallback->OnDataChanged( punk );
		LeaveCriticalSection( &m_csOnDataChanged );
	}
	else
	{
		return E_FAIL;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | TrackPopupMenu | This method displays a
//		context menu at the specified position.
//
//  @parm   HMENU | hMenu | The handle of the menu to add to the default menu.  If NULL, don't add
//		any custom items.
//  @parm   long | lXPos | The horizontal coordinate to display the menu at, in screen coordinates.
//  @parm   long | lYPos | The vertical coordinate to display the menu at, in screen coordinates.
//	@parm	<i DMUSProdStrip>* | pIStrip | Which strip to display the popup menu for.  If NULL, display the default
//		edit menu and send the results to the Timeline.
//  @parm   BOOL | fEditMenu | If TRUE, display the default edit menu.
//
//	@comm	If a custom menu is specified in <p hMenu> and <p fEditMenu> is TRUE, the items in <p hMenu>
//		will be added immediately before the "Properties" item in the edit menu.
//
//	@comm	If a valid <p pIStrip> is given, a WM_COMMAND message will be sent to <om IDMUSProdStrip::OnWMMessage>
//		if the user chooses a menu item.  No message is sent if the user cancels the context menu.
//
//	@comm	If <p hMenu> is non-NULL, <p pIStrip> should also be non-NULL to ensure the custom menu items
//		are properly handled.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p hMenu> is NULL and fEditMenu is FALSE.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::TrackPopupMenu(HMENU hMenu, long x, long y, IDMUSProdStrip *pIStrip, BOOL fEditMenu)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_pTimelineCtl )
	{
		return m_pTimelineCtl->TrackPopupMenu( hMenu, x,y, pIStrip, fEditMenu );
	}
	return E_FAIL;
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToRefTime | This method converts
//		from a time in clocks to a time in REFERENCE_TIME units
//
//	@comm	Negative values for <p lTime> are valid.  The tempo at time 0 will be used
//		to compute the value to return in <p pRefTime>.
//
//  @parm   long | lTime | The time in clocks
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the time in
//		REFERENCE_TIME units this time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p pRefTime>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToClocks>
HRESULT CPrivateTimelineCtl::ClocksToRefTime(
		/* [in] */  long		lTime,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	if (NULL != m_pTempoMapMgr)
	{
		return m_pTempoMapMgr->ClocksToRefTime( lTime, pRefTime);
	}

	REFERENCE_TIME rtTime = 0;

	MUSIC_TIME mtNext, mtCur = 0;
	do
	{
		mtNext = 0;

		BOOL fHaveTempoTrack = FALSE;
		double dblTempo = 120.0;
		MUSIC_TIME mtLatestTempoTime = LONG_MIN;
		DWORD dwIndex = 0;
		while( TRUE )
		{
			IDMUSProdStripMgr *pTempoStripMgr = NULL;
			if( SUCCEEDED( GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
			{
				if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
				{
					DMUS_TEMPO_PARAM tempo;
					MUSIC_TIME mtNextTemp;
					HRESULT hr = pTempoStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNextTemp, &tempo );

					if( SUCCEEDED( hr ) )
					{
						fHaveTempoTrack = TRUE;
						// If we found a real tempo and it's later than all other tempos,
						// save it to use for this time span
						if( (hr == S_OK) && (tempo.mtTime > mtLatestTempoTime) )
						{
							dblTempo = tempo.dblTempo;
							mtLatestTempoTime = tempo.mtTime;
						}
						else if( mtLatestTempoTime == LONG_MIN )
						{
							dblTempo = tempo.dblTempo;
						}

						// Look for the earliest next tempo (mtNextTempo==0 means no more tempo changes)
						if( mtNextTemp && (!mtNext || (mtNextTemp < mtNext)) )
						{
							mtNext = mtNextTemp;
						}
					}
				}

				pTempoStripMgr->Release();
			}
			else
			{
				// No more strips to check - exit
				break;
			}
			dwIndex++;
		}

		if( !fHaveTempoTrack )
		{
			DMUS_TEMPO_PARAM tempo;
			if( SUCCEEDED( m_pTimeStrip->m_pTimeStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNext, &tempo ) ) )
			{
				dblTempo = tempo.dblTempo;
			}
		}

		ASSERT(dblTempo > 0.0);
		if( dblTempo <= 0.0 )
		{
			return E_UNEXPECTED;
		}

		long double ldRes;
		if( !mtNext || mtNext + mtCur > lTime )
		{
			ldRes = (long double(lTime - mtCur) * long double(REFCLOCKS_PER_MINUTE) + ((dblTempo * (long double)DMUS_PPQ) / 2.0)) /
					(dblTempo * (long double)DMUS_PPQ);
		}
		else
		{
			ldRes = ((long double)mtNext * (long double)REFCLOCKS_PER_MINUTE) / (dblTempo * (long double)DMUS_PPQ);
		}
		rtTime += (REFERENCE_TIME) ldRes;
		mtCur += mtNext;
	}
	while( mtNext && mtCur < lTime );

	*pRefTime = rtTime;
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToClocks | This method converts
//		from a time in REFERENCE_TIME units to a time in clocks.
//
//	@comm	Negative values for <p RefTime> are valid, in which case the tempo at time 0 will
//		be used to compute the value of <p plTime>.
//
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_TIME units.
//  @parm   long* | plTime | Address of a variable to receive the time in clocks.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plTime>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToRefTime>
HRESULT CPrivateTimelineCtl::RefTimeToClocks(
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == plTime)
	{
		return E_POINTER;
	}

	if (NULL != m_pTempoMapMgr)
	{
		return m_pTempoMapMgr->RefTimeToClocks( RefTime, plTime );
	}

	MUSIC_TIME mtNext, mtCur = 0;
	do
	{
		mtNext = 0;

		BOOL fHaveTempoTrack = FALSE;
		double dblTempo = 120.0;
		MUSIC_TIME mtLatestTempoTime = LONG_MIN;
		DWORD dwIndex = 0;
		while( TRUE )
		{
			IDMUSProdStripMgr *pTempoStripMgr = NULL;
			if( SUCCEEDED( GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
			{
				if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
				{
					DMUS_TEMPO_PARAM tempo;
					MUSIC_TIME mtNextTemp;
					HRESULT hr = pTempoStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNextTemp, &tempo );

					if( SUCCEEDED( hr ) )
					{
						fHaveTempoTrack = TRUE;
						// If we found a real tempo and it's later than all other tempos,
						// save it to use for this time span
						if( (hr == S_OK) && (tempo.mtTime > mtLatestTempoTime) )
						{
							dblTempo = tempo.dblTempo;
							mtLatestTempoTime = tempo.mtTime;
						}
						else if( mtLatestTempoTime == LONG_MIN )
						{
							dblTempo = tempo.dblTempo;
						}

						// Look for the earliest next tempo (mtNextTempo==0 means no more tempo changes)
						if( mtNextTemp && (!mtNext || (mtNextTemp < mtNext)) )
						{
							mtNext = mtNextTemp;
						}
					}
				}

				pTempoStripMgr->Release();
			}
			else
			{
				// No more strips to check - exit
				break;
			}
			dwIndex++;
		}

		if( !fHaveTempoTrack )
		{
			DMUS_TEMPO_PARAM tempo;
			if( SUCCEEDED( m_pTimeStrip->m_pTimeStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNext, &tempo ) ) )
			{
				dblTempo = tempo.dblTempo;
			}
		}

		ASSERT(dblTempo > 0.0);
		if( dblTempo <= 0.0 )
		{
			return E_UNEXPECTED;
		}

		// Convert mtNext to ldRes
		long double ldRes = ((REFERENCE_TIME)mtNext * REFCLOCKS_PER_MINUTE) / (dblTempo * DMUS_PPQ);
		if( !mtNext || RefTime <= ldRes )
		{
			// Convert RefTime to ldblTmp
			long double ldblTmp = ((long double)RefTime * dblTempo * (long double)DMUS_PPQ + long double(REFCLOCKS_PER_MINUTE / 2)) /
								  (long double)REFCLOCKS_PER_MINUTE;
			*plTime = mtCur + (MUSIC_TIME)ldblTmp;
		}
		else
		{
			mtCur += mtNext;
		}
		RefTime -= (REFERENCE_TIME)ldRes;
	}
	while( mtNext && RefTime > 0 );
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | PositionToRefTime | This method converts
//		from a horizontal pixel position to a time in REFERENCE_TIME units.
//
//	@comm	Negative values for <p lPosition> are valid, in which case the tempo at time 0 will be used
//		to compute the value to return in <p pRefTime>.
//
//  @parm   long | lPosition | The horizontal pixel position.
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the time
//		in REFERENCE_TIME units this position resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p pRefTime>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToClocks>
HRESULT CPrivateTimelineCtl::PositionToRefTime(
		/* [in] */  long		position,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	if (m_fMusicTime == TRUE)
	{
		long lClocks;
		HRESULT hr;
		if (SUCCEEDED(hr = PositionToClocks( position, &lClocks)))
		{
			if (NULL != m_pTempoMapMgr)
			{
				hr = m_pTempoMapMgr->ClocksToRefTime( lClocks, pRefTime);
			}
			else
			{
				hr = ClocksToRefTime( lClocks, pRefTime);
			}
		}
		return hr;
	}
	else
	{
		DMUS_TEMPO_PARAM dmTempo;
		if( FAILED( GetParam( GUID_TempoParam, 0xffffffff, 0, 0, NULL, &dmTempo ) ) )
		{
			return E_FAIL;
		}
		double dblTmp = position;
		dblTmp /= m_dblZoom * (double)DMUS_PPQ * dmTempo.dblTempo / (double)REFCLOCKS_PER_MINUTE;
		*pRefTime = (REFERENCE_TIME) dblTmp;
		return S_OK;
	}
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToPosition | This method converts
//		from a time in REFERENCE_TIME units to a horizontal pixel position
//
//	@comm	Negative values for <p RefTime> are valid, in which case the tempo at time 0 will
//		be used to compute the value of <p plPosition>.
//
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_TIME units.
//  @parm   long* | plPosition | Address of a variable to receive the pixel position this
//		time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plPosition>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToRefTime>
HRESULT CPrivateTimelineCtl::RefTimeToPosition(
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*pPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pPosition)
	{
		return E_POINTER;
	}

	if (m_fMusicTime == TRUE)
	{
		long lClocks;
		// RefTimeToClocks will map this fn to m_pTempoMapMgr if necessary
		HRESULT hr;
		if (SUCCEEDED(hr = RefTimeToClocks( RefTime, &lClocks)))
		{
			hr = ClocksToPosition( lClocks, pPosition);
		}
		return hr;
	}
	else
	{
		DMUS_TEMPO_PARAM dmTempo;
		if( FAILED( GetParam( GUID_TempoParam, 0xffffffff, 0, 0, NULL, &dmTempo ) ) )
		{
			return E_FAIL;
		}
		double dblTmp = (double)RefTime * m_dblZoom * (double)DMUS_PPQ * dmTempo.dblTempo /
						(double)REFCLOCKS_PER_MINUTE;;
		*pPosition = (long) dblTmp;
		return S_OK;
	}
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToRefTime | This method converts
//		from a measure and beat to a time in REFERENCE_TIME units.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p pRefTime>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lMeasure | The measure.
//  @parm   long | lBeat | The beat.
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the
//		reference time.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p pRefTime>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToMeasureBeat>
HRESULT CPrivateTimelineCtl::MeasureBeatToRefTime(
		/* [in] */ DWORD		dwGroupBits,
		/* [in] */ DWORD		dwIndex,
		/* [in] */  long		lMeasure,
		/* [in] */  long		lBeat,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	long lClocks;
	HRESULT hr;
	if (SUCCEEDED( hr = MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, lBeat, &lClocks)))
	{
		if (NULL != m_pTempoMapMgr)
		{
			return m_pTempoMapMgr->ClocksToRefTime( lClocks, pRefTime);
		}
		hr = ClocksToRefTime( lClocks, pRefTime);
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToMeasureBeat | This method converts
//		from a time in REFERENCE_CLOCK units to a time in measures and beats
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_CLOCK units
//  @parm   long* | plMeasure | Address of a variable to receive the measure this
//							   time resolves to.
//  @parm   long* | plBeat | Address of a variable to receive the beat this
//							time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plMeasure> or <p plBeat>
//	@rvalue E_INVALIDARG | <p RefTime> is less than 0.  <p plMeasure> and <p plBeat> are
//						   set to -1.
//	@rvalue E_UNEXPECTED | The tempo is zero at some point or the Time Signature was unable
//		to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToRefTime>
HRESULT CPrivateTimelineCtl::RefTimeToMeasureBeat(
		/* [in] */  DWORD		 dwGroupBits,
		/* [in] */  DWORD		 dwIndex,
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*plMeasure,
		/* [out] */ long		*plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	long lClocks;
	HRESULT hr;
	if (NULL != m_pTempoMapMgr)
	{
		hr = m_pTempoMapMgr->RefTimeToClocks( RefTime, &lClocks );
	}
	else
	{
		hr = RefTimeToClocks( RefTime, &lClocks);
	}
	if ( SUCCEEDED(hr) )
	{
		hr = ClocksToMeasureBeat( dwGroupBits, dwIndex, lClocks, plMeasure, plBeat);
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | ScreenToStripPosition | This method converts from a
//		<t POINT> in screen coordinates to a <t POINT> in strip coordinates.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to get the new coordinates from.
//  @parm   <t POINT>* | pPoint | The screen point to convert.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_POINTER | <p pIStrip> or <p pPoint> are NULL.
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::ScreenToStripPosition(
		/* [in] */		 IDMUSProdStrip		*pIStrip,
		/* [in] [out] */ POINT				*pPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
	if( NULL == pIStrip || NULL == pPoint )
	{
		return E_POINTER;
	}

	if( m_pTimelineCtl )
	{
		return m_pTimelineCtl->ScreenToStripPosition( pIStrip, pPoint );
	}
	return E_FAIL;
}

//  @method HRESULT | IDMUSProdTimeline | StripGetTimelineProperty | This method gets a strip property
//		that is controlled by the Timeline.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to get the property of.
//  @parm   <t STRIP_TIMELINE_PROPERTY> | stp | Which property to get.  Must be one of <t STRIP_TIMELINE_PROPERTY>.
//  @parm   VARIANT* | pVariant | The address to return to property's data in.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pVariant> or <p pIStrip> are NULL.
//	@rvalue E_INVALIDARG | <p stp> does not contain a valid property type, <p pIStrip> was not previously
//		added to the Timeline,	or <p pVariant> contained an invalid value for the specified property type.
//
//	@xref	<i IDMUSProdTimeline>, <t STRIP_TIMELINE_PROPERTY>,
//		<om IDMUSProdTimeline::StripSetTimelineProperty>, <i IDMUSProdStrip>
HRESULT CPrivateTimelineCtl::StripGetTimelineProperty(
		/* [in] */	IDMUSProdStrip*	pIStrip,
		/* [in] */	STRIP_TIMELINE_PROPERTY stp,
		/* [out] */	VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_INVALIDARG;

	if( NULL == pIStrip || NULL == pVariant )
	{
		return E_POINTER;
	}

	if (m_pStripList == NULL)
	{
		return E_INVALIDARG;
	}

	StripList* pSL = FindStripList( pIStrip );
	if( pSL == NULL )
	{
		return E_INVALIDARG;
	}

	switch( stp )
	{
	case STP_VERTICAL_SCROLL:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = pSL->m_lVScroll;
		hr = S_OK;
		break;

	case STP_ENTIRE_STRIP_RECT:
		if( m_pTimelineCtl && (pVariant->vt == VT_BYREF) && (V_BYREF(pVariant) != NULL) )
		{
			RECT *pRect = (RECT *)V_BYREF(pVariant);
			m_pTimelineCtl->GetEntireStripClientRect( pSL, pRect );
			hr = S_OK;
		}
		break;
	case STP_STRIP_RECT:
	case STP_FBAR_RECT:
	case STP_FBAR_CLIENT_RECT:
		if( m_pTimelineCtl && (pVariant->vt == VT_BYREF) && (V_BYREF(pVariant) != NULL) )
		{
			RECT *pRect = (RECT *)V_BYREF(pVariant);
			m_pTimelineCtl->GetStripClientRect( pSL, pRect );
			if( stp == STP_STRIP_RECT )
			{
				pRect->left += m_lFunctionBarWidth;
			}
			else if( stp == STP_FBAR_RECT )
			{
				pRect->right = m_lFunctionBarWidth;
			}
			else // STP_FBAR_CLIENT_RECT
			{
				pRect->bottom = pRect->bottom - pRect->top;
				pRect->top = 0;
				pRect->right = m_lFunctionBarWidth - GUTTER_WIDTH - BORDER_VERT_WIDTH;
			}
			if( pRect->left > pRect->right )
			{
				// there is no client area to draw in, so fail
				pRect->left = 0;
				pRect->right = 0;
				pRect->bottom = 0;
				pRect->top = 0;
				hr = E_FAIL;
			}
			else
			{
				hr = S_OK;
			}
		}
		break;

	case STP_GET_HDC:
		// Only succeed if m_hWnd is actually a real window
		if( m_pTimelineCtl
		&&	::IsWindow( m_pTimelineCtl->m_hWnd ) )
		{
			HDC hdc;
			hdc = ::GetDC( m_pTimelineCtl->m_hWnd );

			if( hdc )
			{
				CDC dc;
				if( dc.Attach( hdc ) )
				{
					RECT rect;

					m_pTimelineCtl->GetStripClientRect( pSL, &rect );
					rect.top += BORDER_HORIZ_WIDTH;
					rect.left += m_lFunctionBarWidth;
					hr = S_OK;
					if( rect.left > rect.right )
					{
						// there is no client area to draw in, so return
						// a null hdc
						ReleaseDC(m_pTimelineCtl->m_hWnd, hdc);
						hdc = NULL;
						hr = E_FAIL;
					}
					else
					{
						CRgn rgn;
						// set window org so m_lXScrollPos,0 is top left of strip
						dc.SetWindowOrg( -rect.left, -rect.top );
						// set clip region so we can't plot outside of strip
						rgn.CreateRectRgn( rect.left, rect.top, rect.right, rect.bottom );
						dc.SelectClipRgn( &rgn );
						rgn.DeleteObject();
					}
					dc.Detach();
				}
				else
				{
					ReleaseDC(m_pTimelineCtl->m_hWnd,hdc);
					hdc = NULL;
					hr = E_FAIL;
				}
			}
			else
			{
				hr = E_FAIL;
			}
			pVariant->vt = VT_I4;
			V_I4(pVariant) = (long)hdc;
		}
		else
		{
			hr = E_FAIL;
		}
		break;

	case STP_HEIGHT:
		pVariant->vt = VT_I4;
		hr = S_OK;
		if ( pSL->m_sv == SV_NORMAL )
		{
			V_I4(pVariant) = pSL->m_lHeight;
		}
		else if ( pSL->m_sv == SV_MINIMIZED )
		{
			V_I4(pVariant) = pSL->m_lRestoreHeight;
		}
		else
		{
			V_I4(pVariant) = -1;
			hr = E_UNEXPECTED;
		}
		break;

	case STP_STRIPVIEW:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = (int) pSL->m_sv;
		hr = S_OK;
		break;

	case STP_POSITION:
		{
			pVariant->vt = VT_I4;
			StripList*	pScan;
			long lResult = -1, lPosition = 0;
			for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
			{
				if( pScan->m_pStrip == pIStrip )
				{
					lResult = lPosition;
					break;
				}
				lPosition++;
			}
			if( lResult == -1 )
			{
				hr = E_INVALIDARG;
				V_I4(pVariant) = lResult;
			}
			else
			{
				hr = S_OK;
				V_I4(pVariant) = lResult;
			}
		}
		break;

	case STP_GUTTER_SELECTED:
		pVariant->vt = VT_BOOL;
		V_BOOL(pVariant) = (short)pSL->m_fSelected;
		hr = S_OK;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

const StripList *GetStripList( const CPrivateTimelineCtl *pCPrivateTimelineCtl )
{
	ASSERT( pCPrivateTimelineCtl );
	if( pCPrivateTimelineCtl )
	{
		ASSERT( pCPrivateTimelineCtl->m_pStripList );
		return pCPrivateTimelineCtl->m_pStripList;
	}
	return NULL;
}

long CPrivateTimelineCtl::TotalStripHeight(void) const
{
	const StripList* pSL;
	long lReturn = 0;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		lReturn += pSL->m_lHeight + ( 2 * BORDER_HORIZ_DRAWWIDTH );
	}
	return lReturn;
}

void CPrivateTimelineCtl::CallStripWMMessage( const StripList* pSL, UINT nMsg, WPARAM wParam,
	LPARAM lParam )
{
	BOOL fFunctionBar = FALSE; // true if mouse is in function bar area

	if (pSL == NULL)
	{
		return;
	}

	const StripList* pMainStripList = GetStripList( this );

	long xPos, yPos;
	xPos = GET_X_LPARAM(lParam);
	yPos = GET_Y_LPARAM(lParam);

	switch(nMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_SETCURSOR:
	case WM_CONTEXTMENU:
		{
		// adjust lParam so the cursor position is relative to 0,0 of the strip

			if( xPos < m_lFunctionBarWidth )
			{
				fFunctionBar = TRUE;
			}

			// add horizontal scroll and subtract the function bar and any extras time at the start
			xPos = xPos + m_lXScrollPos - m_lFunctionBarWidth;

			// subtract away the border
			yPos -= BORDER_HORIZ_WIDTH;
			// if not minized, add any strip vertical scrolling
			if (pSL->m_sv != SV_MINIMIZED)
			{
				yPos += pSL->m_lVScroll;
			}

			// add timeline scroll for all except top strip, which doesn't scroll
			if( pSL != pMainStripList )
			{
				yPos += m_lYScrollPos;
			}

			// subtract the heights of any strips above this one
			for( const StripList* pTempSL = pMainStripList; pTempSL; pTempSL = pTempSL->m_pNext )
			{
				if( pTempSL == pSL )
				{
					break;
				}
				yPos -= ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
			}
			lParam = MAKELONG( xPos, yPos );
		}
		break;
	default:
		break;
	}
	if( fFunctionBar )
	{
		if( nMsg == WM_LBUTTONDOWN || nMsg == WM_RBUTTONDOWN )
		{
			// Unselect everything
			SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
			SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
		}

		// we're in the function bar area
		IDMUSProdStripFunctionBar* pSFB;
		if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdStripFunctionBar, (void**)&pSFB)))
		{
			pSFB->FBOnWMMessage( nMsg, wParam, lParam, xPos, yPos );
			pSFB->Release();
		}
	}
	else
	{
		if( m_pTimelineCtl
		&&	(nMsg == WM_LBUTTONDOWN || nMsg == WM_RBUTTONDOWN) )
		{
			// Set the time cursor to where the user clicked
			long lTime;
			PositionToClocks( xPos, &lTime );
			m_pTimelineCtl->SetTimeCursor( lTime, false, true );
		}
		pSL->m_pStrip->OnWMMessage( nMsg, wParam, lParam, xPos, yPos );
	}
}

void CPrivateTimelineCtl::DeactivateStripList(void)
{
	RECT rect;

	if( m_pActiveStripList )
	{
		if( m_pTimelineCtl )
		{
			m_pTimelineCtl->GetStripClientRect( m_pActiveStripList, &rect );
			//rect.left = 0;
			//rect.right = rect.left + GUTTER_WIDTH;
			m_pTimelineCtl->InvalidateRect( &rect, FALSE );
		}
		const StripList* pTempSL = m_pActiveStripList;
		m_pActiveStripList = NULL;
		CallStripWMMessage( pTempSL, WM_KILLFOCUS, 0, 0 );
	}
}

void CPrivateTimelineCtl::ActivateStripList( const StripList* pSL )
{
	if( pSL != m_pActiveStripList )
	{
		if( m_pActiveStripList )
		{
			DeactivateStripList();
		}
		m_pActiveStripList = pSL;

		if( m_pTimelineCtl )
		{
			RECT rect;
			m_pTimelineCtl->GetStripClientRect( pSL, &rect );
			//rect.left = 0;
			//rect.right = rect.left + GUTTER_WIDTH;
			m_pTimelineCtl->InvalidateRect( &rect, FALSE );
		}
		CallStripWMMessage( m_pActiveStripList, WM_SETFOCUS, 0, 0 );
	}
}

HRESULT CPrivateTimelineCtl::StripPointToClient( const StripList* pSL, POINT *pPoint) const
{
	// Validate parameters
	ASSERT( pSL && pPoint );
	if ((NULL == pSL)||(NULL == pPoint))
	{
		return E_INVALIDARG;
	}

	// Store the point's y coordinate in a working variable
	long yPos;
	yPos = pPoint->y;

	// subtract horizontal scroll and add the function bar and early time
	pPoint->x -= m_lXScrollPos - m_lFunctionBarWidth;

	// add any borders
	yPos += BORDER_HORIZ_WIDTH;

	const StripList *pMainStripList = GetStripList( this );
	// Check if we're looking in the first strip
	if (pSL == pMainStripList)
	{
		if (pSL->m_sv != SV_MINIMIZED)
		{
			// subtract any strip vertical scrolling, if not minized
			yPos -= pSL->m_lVScroll;
		}
		pPoint->y = yPos;
		return S_OK;
	}

	// subtract timeline scroll for all except top strip, which doesn't scroll
	yPos -= m_lYScrollPos;

	// add the heights of any strips above this one
	for( const StripList* pTempSL = pMainStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pSL == pTempSL )
		{
			break;
		}
		yPos += ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
	}

	if ( pTempSL == NULL )
	{
		// Strip not in our list.
		return E_INVALIDARG;
	}

	if (pSL->m_sv != SV_MINIMIZED)
	{
		// subtract any strip vertical scrolling, if not minized
		yPos -= pSL->m_lVScroll;
	}

	pPoint->y = yPos;
	return S_OK;
}

HRESULT CPrivateTimelineCtl::StripRectToClient( const StripList* pSL, LPRECT pRect) const
{
	POINT point;
	point.x = 0;
	point.y = 0;

	HRESULT hr;
	hr = StripPointToClient( pSL, &point );
	if ( SUCCEEDED(hr) )
	{
		pRect->top += point.y;
		pRect->bottom += point.y;
		pRect->left += point.x;
		pRect->right += point.x;
	}
	return hr;
}

bool CPrivateTimelineCtl::AnyGutterSelectedStrips( void ) const
{
	const StripList* pSL;
	for( pSL = GetStripList( this ); pSL; pSL = pSL->m_pNext )
	{
		if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
		{
			return true;
		}
	}

	return false;
}

bool CPrivateTimelineCtl::ComputeEarlyAndLateTime( long &lEarlyTime, long &lLateTime ) const
{
	// Initialize the times to 0.
	lEarlyTime = 0;
	lLateTime = 0;

	VARIANT varTime;
	for( const StripList* pList = GetStripList( this ); pList; pList = pList->m_pNext )
	{
		if( SUCCEEDED( pList->m_pStrip->GetStripProperty( SP_EARLY_TIME, &varTime ) ) )
		{
			lEarlyTime = max( lEarlyTime, V_I4(&varTime) );
		}

		if( SUCCEEDED( pList->m_pStrip->GetStripProperty( SP_LATE_TIME, &varTime ) ) )
		{
			lLateTime = max( lLateTime, V_I4(&varTime) );
		}
	}

	return (lEarlyTime != 0) || (lLateTime != 0);
}

HRESULT CPrivateTimelineCtl::SetTimelineOleCtl( IUnknown *punkTimelineOleCtl )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CTimelineCtl *pNewTimelineCtl = NULL;
	if( punkTimelineOleCtl
	&&	FAILED( punkTimelineOleCtl->QueryInterface( IID_ICTimelineCtl, (void **)&pNewTimelineCtl ) ) )
	{
		return E_INVALIDARG;
	}

	if( m_pTimelineCtl == pNewTimelineCtl
	||	m_fInSetTimelineOleCtl )
	{
		if( pNewTimelineCtl )
		{
			pNewTimelineCtl->Release();
		}
		return S_FALSE;
	}

	m_fInSetTimelineOleCtl = true;

	if( m_pTimelineCtl )
	{
		if( ::IsWindow(m_pTimelineCtl->m_hWnd)
		&&	m_pStripList )
		{
			StripList *pTemp = m_pStripList;
			while( pTemp )
			{
				pTemp->m_pStrip->OnWMMessage( WM_DESTROY, 0, 0, 0, 0 );
				pTemp = pTemp->m_pNext;
			}
		}

		m_pTimelineCtl->SetPrivateTimelineCtl( NULL );
		m_pTimelineCtl->Release();
	}

	// Set the new timeline pointer
	m_pTimelineCtl = pNewTimelineCtl;

	if( m_pTimelineCtl)
	{
		m_pTimelineCtl->AddRef();
		m_pTimelineCtl->SetPrivateTimelineCtl( this );

		if( ::IsWindow(m_pTimelineCtl->m_hWnd)
		&&	m_pStripList )
		{
			StripList *pTemp = m_pStripList;
			while( pTemp )
			{
				pTemp->m_pStrip->OnWMMessage( WM_CREATE, 0, 0, 0, 0 );
				pTemp = pTemp->m_pNext;
			}
		}
	}

	if( pNewTimelineCtl )
	{
		pNewTimelineCtl->Release();
	}
	m_fInSetTimelineOleCtl = false;
	return S_OK;
}

HRESULT CPrivateTimelineCtl::FinalCleanUp()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Release our reference to the Timeline OLE control
	SetTimelineOleCtl( NULL );

	// This should be NULL
	ASSERT( m_pTimelineCtl == NULL );

	IDMUSProdPropPageObject* pPPO;

	// Clean up the strip manager list
	while( m_pStripMgrList )
	{
		if( m_pStripMgrList->m_pStripMgr )
		{
			// make sure that there's no chance an object is left over
			if( SUCCEEDED( m_pStripMgrList->m_pStripMgr->QueryInterface( 
				IID_IDMUSProdPropPageObject, (void**)&pPPO )))
			{
				RemovePropertyPageObject(pPPO);
				pPPO->Release();
			}

			// Now, set the StripMgr's timeline pointer to NULL
			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			V_UNKNOWN(&varTimeline) = NULL;
			m_pStripMgrList->m_pStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
		}

		// Now, delete the StripMgr list item (which will release the
		// strip manager interface)
		StripMgrList* pSML = m_pStripMgrList->m_pNext;
		delete m_pStripMgrList;
		m_pStripMgrList = pSML;
	}

	// Clean up the strip list
	while( m_pStripList )
	{
		if( m_pStripList->m_pStrip )
		{
			// make sure that there's no chance an object is left over
			if( SUCCEEDED( m_pStripList->m_pStrip->QueryInterface( 
				IID_IDMUSProdPropPageObject, (void**)&pPPO )))
			{
				RemovePropertyPageObject(pPPO);
				pPPO->Release();
			}
		}

		// Now, delete the Strip list item (which will release the
		// strip interface)
		StripList* pSL = m_pStripList->m_pNext;
		delete m_pStripList;
		m_pStripList = pSL;
	}

	// Clean up the notification lists
	NotifyEntry* pNotifyEntry = NULL;
	while( !m_lstNotifyEntry.IsEmpty() )
	{
		pNotifyEntry = m_lstNotifyEntry.RemoveHead();
		if ( pNotifyEntry != NULL )
		{
			delete pNotifyEntry;
		}
	}

	// Clean up our pointers
	if( m_pDMUSProdFramework )
	{
		m_pDMUSProdFramework->Release();
		m_pDMUSProdFramework = NULL;
	}
	if( m_pIDMPerformance )
	{
		m_pIDMPerformance->Release();
		m_pIDMPerformance = NULL;
	}
	if( m_pTimelineCallback )
	{
		m_pTimelineCallback->Release();
		m_pTimelineCallback = NULL;
	}

	return S_OK;
}

HRESULT WINAPI CPrivateTimelineCtlQI(void* pv, REFIID riid, LPVOID* ppv, DWORD dw)
{
	UNREFERENCED_PARAMETER( dw );
	CPrivateTimelineCtl *pCPrivateTimelineCtl = (CPrivateTimelineCtl *)pv;
	ASSERT( pCPrivateTimelineCtl );
	if( pCPrivateTimelineCtl->m_pTimelineCtl )
	{
		return pCPrivateTimelineCtl->m_pTimelineCtl->QueryInterface( riid, ppv );
	}
	else
	{
		return E_NOINTERFACE;
	}
}
