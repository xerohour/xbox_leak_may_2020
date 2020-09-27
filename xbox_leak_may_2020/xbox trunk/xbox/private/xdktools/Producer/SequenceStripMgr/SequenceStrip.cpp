// SequenceStrip.cpp : Implementation of CSequenceStrip
#include "stdafx.h"
#include "SequenceIO.h"
#include "SequenceStripMgr.h"
#include "SequenceMgr.h"
//#include "PropPageMgr.h"
#include "QuantizeDlg.h"
#include "DialogVelocity.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include <RiffStrm.h>
#include <dmusici.h>
#include <dmusicf.h>
#include "midifileio.h"
#include "notetracker.h"
#include "CurveStrip.h"
#include "DialogNewCCTrack.h"
#include "BandEditor.h"
#include <PChannelName.h>
#include "PropPageSeqTrack.h"
#include "SegmentIO.h"
#include "SharedPianoRoll.h"
#include "TrackFlagsPPG.h"
#include "GrayOutRect.h"
#include "windowsx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INSTRUMENT_BUTTON_HEIGHT 20
#define NEWBAND_BUTTON_WIDTH 20

#define WM_APP_BUMPTIMECURSOR 0
#define WM_APP_INVALIDATEPIANOROLL 1
#define WM_APP_INSTRUMENTMENU 2
#define WM_APP_BANDMENU 3

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0


long	CSequenceStrip::m_lBitmapRefCount = 0;
CBitmap	CSequenceStrip::m_BitmapZoomInUp;
CBitmap	CSequenceStrip::m_BitmapZoomInDown;
CBitmap	CSequenceStrip::m_BitmapZoomOutUp;
CBitmap	CSequenceStrip::m_BitmapZoomOutDown;
CBitmap CSequenceStrip::m_BitmapNewBand;

MUSIC_TIME GetTimeOffset( const MUSIC_TIME mtNow, const MUSIC_TIME mtCurrentStartTime, const MUSIC_TIME mtCurrentStartPoint,
						  const MUSIC_TIME mtCurrentLoopStart, const MUSIC_TIME mtCurrentLoopEnd, const MUSIC_TIME mtLength,
						  const DWORD dwCurrentMaxLoopRepeats )
{
	// Convert mtNow from absolute time to an offset from when the segment started playing
	LONGLONG llBigNow = mtNow - (mtCurrentStartTime - mtCurrentStartPoint);

	// If mtLoopEnd is non zero, set lLoopEnd to mtLoopEnd, otherwise use the segment length
	LONGLONG llLoopEnd = mtCurrentLoopEnd ? mtCurrentLoopEnd : mtLength;

	LONGLONG llLoopStart = mtCurrentLoopStart;

	if( (dwCurrentMaxLoopRepeats != 0) &&
		(llLoopStart < llLoopEnd) )
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
// CSequenceStrip constructor/destructor

CSequenceStrip::CSequenceStrip( CSequenceMgr* pSequenceMgr )
{
	ASSERT( pSequenceMgr );
	if ( pSequenceMgr == NULL )
	{
		return;
	}

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_pSequenceMgr = pSequenceMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pSequenceMgr;
	//m_pStripMgr->AddRef();

	m_lBeginSelect = 0;
	m_lEndSelect = 0;
	m_bGutterSelected = FALSE;
	m_SelectionMode = SEQ_NO_SELECT;
	m_bSelectionCC = 0xFF;
	m_wSelectionParamType = 0xFFFF;

	ZeroMemory( &m_pointSelection, sizeof(POINT) );
	m_fSelecting = FALSE;

	UpdateName();
	m_iHaveFocus = 0;
	m_mtLength = 0;

	m_svView = SV_MINIMIZED;
	m_lVScroll = -1;
	m_fHybridNotation = FALSE;
	m_dblVerticalZoom = 0.1;
	ZeroMemory(m_aiAccidentals, sizeof(int) * 75);
	ZeroMemory(m_aiScalePattern, sizeof(int) * 7);

	m_fZoomInPressed		= FALSE;
	m_fZoomOutPressed		= FALSE;
	m_fNewBandPressed		= FALSE;
	m_fInstrumentPressed	= FALSE;
	m_fInstrumentEnabled	= FALSE;

	// Curve Strip fields
	m_CurveStripView = SV_MINIMIZED;
	m_lstCurveStripStates.RemoveAll();
	m_lstCurveStrips.RemoveAll();

	m_lXPos = -1;
	m_pSelectPivotSeq = NULL;
	m_pActiveNote = NULL;
	m_OriginalSeqItem.Clear();
	ZeroMemory( &m_pointClicked, sizeof(POINT) );
	m_nLastEdit = 0;
	m_fCtrlKeyDown = FALSE;
	m_lLastDeltaMove = 0;
	m_cLastDeltaValue = 0;
	m_lLastDeltaDur = 0;
	m_lLastDeltaVel = 0;
	m_lLastDeltaStart = 0;
	m_fScrollTimerActive = FALSE;
	m_crSelectedNoteColor = COLOR_DEFAULT_SELECTED;
	m_crUnselectedNoteColor = COLOR_DEFAULT_UNSELECTED;
	m_crOverlappingNoteColor = COLOR_DEFAULT_OVERLAPPING;
	m_crAccidentalColor = COLOR_DEFAULT_ACCIDENTAL;
	m_lInsertVal = 60;
	m_lInsertTime = 0;
	m_mtLastLateTime = 0;
	m_dwExtraBars = 0;
	m_fPickupBar = FALSE;
	m_fInsertingNoteFromMouseClick = false;
	m_pActiveCurveStrip = NULL;
	m_dwLastPatch = INVALID_PATCH;

	m_pITargetDataObject = NULL;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;

	m_cfSequenceList = ::RegisterClipboardFormat( CF_SEQUENCELIST );
	m_cfMidiFile = ::RegisterClipboardFormat( CF_MIDIFILE );

	m_bMinimizeTopNote = 127;
	m_bMinimizeNoteRange = 127;
	m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
	m_hCursor = GetArrowCursor();
	m_MouseMode = SEQ_MM_NORMAL;

	m_fShowSequenceProps = FALSE;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;

	m_rectVScroll.SetRectEmpty();
	//m_VScrollBar; // Initalized by contstructor

	// MIDI
	m_fMIDIInRegistered = FALSE;
	m_fMIDIThruEnabled = FALSE;
	m_dwCookie = 0;
	ZeroMemory( m_bVelocity, 128 * sizeof( BYTE ) );
	ZeroMemory( m_mtStartTime, 128 * sizeof( MUSIC_TIME ) );
}

CSequenceStrip::~CSequenceStrip()
{
	ASSERT( m_pStripMgr );

	// Curve Strip fields
	ioCurveStripState* pCurveStripState;
	while( !m_lstCurveStripStates.IsEmpty() )
	{
		pCurveStripState = static_cast<ioCurveStripState*>( m_lstCurveStripStates.RemoveHead() );
		delete pCurveStripState;
	}
	CCurveStrip *pCurveStrip;
	while( !m_lstCurveStrips.IsEmpty() )
	{
		pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.RemoveHead() );
		if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
		{
			m_pSequenceMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );
		}
		pCurveStrip->Release();
	}

	if (m_fMIDIInRegistered)
	{
		UnRegisterMidi();
	}

	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pSequenceMgr = NULL;
	}
	if ( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
	}
	if ( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::QueryInterface

STDMETHODIMP CSequenceStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	ASSERT( ppv );
	if ( ppv == NULL )
	{
		return E_INVALIDARG;
	}

    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdTimelineEdit ))
	{
		*ppv = (IDMUSProdTimelineEdit*) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdPropPageObject))
	{
        *ppv = (IUnknown *) (IDMUSProdPropPageObject *) this;
	}
	else if( IsEqualIID( riid, IID_IDropTarget ))
	{
		*ppv = (IUnknown *) (IDropTarget *) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdMidiInCPt ))
	{
		*ppv = (IUnknown *) (IDMUSProdMidiInCPt *) this;
	}
	else
	{
		return E_NOTIMPL;
	}

    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::AddRef

STDMETHODIMP_(ULONG) CSequenceStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Release

STDMETHODIMP_(ULONG) CSequenceStrip::Release(void)
{
	if( 0L == --m_cRef )
	{
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Draw

HRESULT	STDMETHODCALLTYPE CSequenceStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (!hDC)
	{
		return E_INVALIDARG;
	}

	// Validate pointer to the Timeline
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Get the clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Find the highest and lowest MIDI values to display
	int nTopNote = 127, nBottomNote = 0;
	if (sv == SV_NORMAL)
	{
		long lOrigRightClip = rectClip.right;
		long lOrigLeftClip = rectClip.left;
		if( lXOffset < 0 )
		{
			rectClip.left = max( -lXOffset, rectClip.left);
		}

		long lTimelinePixelLength = LONG_MIN;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_mtLength, &lTimelinePixelLength ) ) )
		{
			lTimelinePixelLength -= lXOffset;
			rectClip.right = min( rectClip.right, lTimelinePixelLength );
		}

		if( m_fHybridNotation )
		{
			nTopNote = 38 - (rectClip.top / m_lMaxNoteHeight);
			nBottomNote = 38 - (rectClip.bottom / m_lMaxNoteHeight);
			if( nBottomNote < 0 )
			{
				nBottomNote = 0;
			}

			// Draw the background horizontal lines
			DrawHybridLines( hDC, rectClip, nBottomNote, nTopNote );

			// TODO: Fix this so it's more accurate (Also in PianoRollStrip.cpp)
			nTopNote = min( nTopNote * 4, 127 );
			nBottomNote = max( nBottomNote * 3, 0);
		}
		else
		{
			nTopNote = 127 - (rectClip.top / m_lMaxNoteHeight);
			nBottomNote = 127 - (rectClip.bottom / m_lMaxNoteHeight);
			nTopNote = min( nTopNote, 127 );
			nBottomNote = max( nBottomNote, 0);

			// Draw the background horizontal lines (and white/grey bars)
			DrawHorizontalLines( hDC, rectClip, nBottomNote, nTopNote );
		}

		// Check if we are displaying part of a pick-up measure
		if( rectClip.left != lOrigLeftClip )
		{
			// Restore the clipping rectangle
			rectClip.left = lOrigLeftClip;

			// Save the old right edge of the clipping rectangle
			long lTmpRight = rectClip.right;

			// Update the right edge of the clipping rectangle to be at time 0
			rectClip.right = min( rectClip.right, -lXOffset );

			// Draw the dark background horizontal lines
			if( m_fHybridNotation )
			{
				DrawDarkHybridLines( hDC, rectClip, max( 0, 38 - (rectClip.bottom / m_lMaxNoteHeight) ), 38 - (rectClip.top / m_lMaxNoteHeight) );
			}
			else
			{
				DrawDarkHorizontalLines( hDC, rectClip, nBottomNote, nTopNote );
			}

			// Restore the clipping rectangle
			rectClip.right = lTmpRight;
		}

		// Check if we are displaying part of an extension measure
		if( rectClip.right != lOrigRightClip )
		{
			// Restore the clipping rectangle
			rectClip.right = lOrigRightClip;

			// Save the old left edge of the clipping rectangle
			long lTmpLeft = rectClip.left;

			// Update the left edge of the clipping rectangle to be at the end of normal time
			if( lTimelinePixelLength != LONG_MIN )
			{
				rectClip.left = max( rectClip.left, lTimelinePixelLength );

				// Draw the dark background horizontal lines
				if( m_fHybridNotation )
				{
					DrawDarkHybridLines( hDC, rectClip, max( 0, 38 - (rectClip.bottom / m_lMaxNoteHeight) ), 38 - (rectClip.top / m_lMaxNoteHeight) );
				}
				else
				{
					DrawDarkHorizontalLines( hDC, rectClip, nBottomNote, nTopNote );
				}
			}

			// Restore the clipping rectangle
			rectClip.left = lTmpLeft;
		}

		// Draw Measure and Beat lines in our strip
		m_pSequenceMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT_GRID, m_pSequenceMgr->m_dwGroupBits, 0, lXOffset );
	}
	else if (sv == SV_MINIMIZED)
	{
		// Compute the top and bottom notes to display so we can scale the display accordingly
		BYTE bBottomNote = 127;
		m_bMinimizeTopNote = 0;
		POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
		while( pos )
		{
			const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
			ASSERT( pSequenceItem );

			if ( m_bMinimizeTopNote < pSequenceItem->m_bByte1 )
			{
				m_bMinimizeTopNote = pSequenceItem->m_bByte1;
			}
			if ( bBottomNote > pSequenceItem->m_bByte1 )
			{
				bBottomNote = pSequenceItem->m_bByte1;
			}
		}

		if ( m_bMinimizeTopNote == 0 && bBottomNote == 127 )
		{
			m_bMinimizeNoteRange = 12;
			m_bMinimizeTopNote = 127;
		}
		else
		{
			m_bMinimizeNoteRange = BYTE(1 + m_bMinimizeTopNote - bBottomNote);
		}

		// If the range is less than 12, set it to 12
		ASSERT( m_bMinimizeNoteRange > 0 );
		if ( m_bMinimizeNoteRange < 12 )
		{
			m_bMinimizeNoteRange = 12;
		}

		// Draw Measure and Beat lines in our strip
		m_pSequenceMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pSequenceMgr->m_dwGroupBits, 0, lXOffset );
	}

	// invertrect selected time
	if( m_bGutterSelected && (m_lBeginSelect != m_lEndSelect) )
	{
		long beginPos, endPos;
		RECT rectInvert;
		
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_lBeginSelect, &beginPos );
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_lEndSelect, &endPos );
		if( ( endPos != 0 ) && ( beginPos == endPos ) )
		{
			endPos += 1;
		}
		rectInvert.left = beginPos - lXOffset;
		rectInvert.right = endPos - lXOffset;
		rectInvert.top = rectClip.top;
		rectInvert.bottom = rectClip.bottom;
		GrayOutRect( hDC, &rectInvert );
	}

	long	lStartTime, lEndTime;
	m_pSequenceMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );
	m_pSequenceMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &lEndTime );
	
	if( sv == SV_NORMAL )
	{
		POSITION pos = GetFirstVisibleNote( lStartTime, lEndTime, nTopNote, nBottomNote );

		if( pos )
		{
			if( m_fHybridNotation )
			{
				// Clear out the list that keeps track of overlapping notes
				m_aNoteRectList[0].RemoveAll();

				// Draw unselected notes
				DrawHybridMaximizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, nTopNote, nBottomNote, FALSE );

				// Clear out the list that keeps track of overlapping notes
				m_aNoteRectList[0].RemoveAll();

				// Draw selected notes
				DrawHybridMaximizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, nTopNote, nBottomNote, TRUE );

				// Draw the Accidentals
				DrawHybridMaximizedAccidentals( hDC, pos, lStartTime, lEndTime, lXOffset, nTopNote, nBottomNote );
			}
			else
			{
				// Clear out the list that keeps track of overlapping notes
				for( int i=nBottomNote; i<=nTopNote; i++ )
				{
					m_aNoteRectList[i].RemoveAll();
				}

				// Draw unselected notes
				DrawMaximizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, nTopNote, nBottomNote, FALSE );

				// Clear out the list that keeps track of overlapping notes
				for( i=nBottomNote; i<=nTopNote; i++ )
				{
					m_aNoteRectList[i].RemoveAll();
				}

				// Draw selected notes
				DrawMaximizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, nTopNote, nBottomNote, TRUE );
			}
		}

		if( (m_iHaveFocus == 1) && (m_lInsertVal <= nTopNote) && (m_lInsertVal >= nBottomNote) )
		{
			DrawNoteInsertionMark( hDC, lStartTime, lEndTime, lXOffset );
		}
	}
	else if( sv == SV_MINIMIZED )
	{
		POSITION pos = GetFirstVisibleNote( lStartTime, lEndTime, 127, 0 );

		if( !pos )
		{
			return S_OK;
		}

		DrawMinimizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, FALSE );
		DrawMinimizedNotes( hDC, pos, lStartTime, lEndTime, lXOffset, TRUE );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CSequenceStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_GUTTERSELECTABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_MINMAXABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_DEFAULTHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = DEFAULT_HEIGHT;
		break;
	case SP_MAXHEIGHT:
		pvar->vt = VT_INT;
		if( m_fHybridNotation )
		{
			V_INT(pvar) = m_lMaxNoteHeight * 38;
		}
		else
		{
			V_INT(pvar) = m_lMaxNoteHeight * 128;
		}
		break;
	case SP_MINHEIGHT:
	case SP_MINIMIZE_HEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = MINIMIZE_HEIGHT;
		break;
	case SP_NAME:
		{
			BSTR bstr;

			UpdateName();
			pvar->vt = VT_BSTR; 
			try
			{
				bstr = m_strName.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;
	case SP_CURSOR_HANDLE:
		pvar->vt = VT_I4;
		V_I4(pvar) = (int) m_hCursor;
		break;
	case PRIVATE_SP_CLSID:
		// CLSID used to identify type of strip
		if( (pvar->vt == VT_BYREF)
		&&  (V_BYREF(pvar) != NULL) )
		{
			CLSID* pCLSID = (CLSID *)V_BYREF(pvar);
		   *pCLSID = CLSID_DirectMusicSeqTrack;
		}
		else
		{
			return E_FAIL;
		}
		break;
	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		m_pSequenceMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		break;

	case SP_FUNCTIONBAR_EXCLUDE_WIDTH:
		pvar->vt = VT_I4;
		V_I4(pvar) = m_BitmapZoomInUp.GetBitmapDimension().cx;
		break;

	case PRIVATE_SP_PCHANNEL:
		pvar->vt = VT_I4;
		V_I4(pvar) = m_pSequenceMgr->m_dwPChannel;
		break;

	case SP_EARLY_TIME:
		m_fPickupBar = ShouldDisplayPickupBar();
		if( m_fPickupBar )
		{
			DMUS_TIMESIGNATURE dmTimeSig;
			GetTimeSig( 0, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );

			// Found an early note
			pvar->vt = VT_I4;
			V_I4(pvar) = dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat);
			//m_mtLastEarlyTime = m_lMeasureClocks;
			return S_OK;
		}

		// No early notes
		//m_mtLastEarlyTime = 0;
		return E_FAIL;
		break;

	case SP_LATE_TIME:
		m_dwExtraBars = GetNumExtraBars();

		if( m_dwExtraBars )
		{
			//this->m_mtLength;

			// Find out when the last measure starts
			long lLastMeasureStart;
			m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength, &lLastMeasureStart, NULL );
			m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lLastMeasureStart, 0, &lLastMeasureStart );

			// Find out the TimeSig of the last measure
			DMUS_TIMESIGNATURE dmTimeSig;
			GetTimeSig( m_mtLength - 1, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );
			const long lMeasureLength = dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat);

			// Compute the amount of time to display
			pvar->vt = VT_I4;
			V_I4(pvar) = m_dwExtraBars * lMeasureLength + lLastMeasureStart - m_mtLength;
			m_mtLastLateTime = V_I4(pvar);

			return S_OK;
		}

		// No late notes
		m_mtLastLateTime = 0;
		return E_FAIL;
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CSequenceStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
	{
		long lNewSelection;
		BOOL fChanged;
		
		fChanged = FALSE;

		// Validate parameter
		if( var.vt != VT_I4)
		{
			return E_FAIL;
		}

		// Set selection
		lNewSelection = V_I4( &var );
		if( sp == SP_BEGINSELECT )
		{
			if( m_lBeginSelect != lNewSelection )
			{
				m_lBeginSelect = lNewSelection;
				fChanged = TRUE;
			}
		}
		else
		{
			if( m_lEndSelect != lNewSelection )
			{
				m_lEndSelect = lNewSelection;
				fChanged = TRUE;
			}
		}

		// Update our curve strips with the new selection
		UpdateCurveStripGutterSelection( fChanged );

		// Exit early if we're just clearing the Timeline selection
		if( m_fSelecting )
		{
			if( m_bGutterSelected )
			{
				InvalidateStrip();
			}
			break;
		}

		// If start time == end time, deselect everything
		if( m_lBeginSelect == m_lEndSelect )
		{
			if( m_SelectionMode != SEQ_NO_SELECT )
			{
				// This refreshes the display, if necessary
				m_pSequenceMgr->UnselectAllNotes();
			}
			break;
		}

		if( m_bGutterSelected && (m_lBeginSelect >= 0) && (m_lEndSelect > 0))
		{
			// This may be true if only curves were selected
			if( SelectEventsBetweenTimes( m_lBeginSelect, m_lEndSelect ) )
			{
				m_pSequenceMgr->RefreshPropertyPage();
			}

			// Redraw this piano roll
			InvalidateStrip();
		}
		break;
	}

	case SP_GUTTERSELECT:
		if( m_bGutterSelected != V_BOOL(&var) )
		{
			m_bGutterSelected = V_BOOL(&var);

			// Make sure timeline is in sync because change may be passed through from Curve strip
			if( m_pSequenceMgr->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = (short)m_bGutterSelected;
				m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_GUTTER_SELECTED, var );
			}
			
			// Update our curve strips with the new selection state
			UpdateCurveStripGutterSelection( TRUE );

			if(m_bGutterSelected  && (m_lBeginSelect >= 0) && (m_lEndSelect > 0))
			{
				// This may be true if only curves were selected
				if( SelectEventsBetweenTimes( m_lBeginSelect, m_lEndSelect ) )
				{
					m_pSequenceMgr->RefreshPropertyPage();
				}
			}
			else
			{
				m_pSequenceMgr->UnselectAllNotes();
			}

			// Redraw this piano roll
			InvalidateStrip();
		}
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnRButtonUp

HRESULT CSequenceStrip::OnRButtonUp( void )
{
	HRESULT hr = S_OK;
	POINT pt;
	BOOL  bResult;

	// Get the cursor position (To put the menu there)
	bResult = ::GetCursorPos( &pt );
	ASSERT( bResult );
	if( !bResult )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr != NULL );
	if ( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	if ( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	HMENU hMenu, hMenuPopup;
	hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_EDIT_MENU));
	if (hMenu == NULL)
	{
		return E_UNEXPECTED;
	}

	hMenuPopup = ::GetSubMenu( hMenu, 0 );
	if (hMenuPopup == NULL)
	{
		DestroyMenu(hMenu);
		return E_UNEXPECTED;
	}

	// update items
	EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );
	EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );
	EnableMenuItem( hMenuPopup, 2, ( CanPaste() == S_OK ) ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
	EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );
	EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( (m_svView == SV_NORMAL) && (CanInsert() == S_OK) ) ?
					MF_ENABLED : MF_GRAYED );
	EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );

	// Views menu
	EnableMenuItem( hMenuPopup, 12, ( m_svView == SV_NORMAL ) ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
	::CheckMenuItem( hMenuPopup, ID_EDIT_VIEW_PIANOROLL, m_fHybridNotation ? MF_UNCHECKED : MF_CHECKED );
	::CheckMenuItem( hMenuPopup, ID_EDIT_VIEW_HYBRID, m_fHybridNotation ? MF_CHECKED : MF_UNCHECKED );

	// Snap-to menu
	VARIANT var;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
	{
		::EnableMenuItem( hMenuPopup, 10, MF_BYPOSITION | MF_ENABLED );
		switch( (DMUSPROD_TIMELINE_SNAP_TO) V_I4( &var ) )
		{
		case DMUSPROD_TIMELINE_SNAP_NONE:
			::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_CHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_UNCHECKED );
			break;
		case DMUSPROD_TIMELINE_SNAP_GRID:
			::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_CHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_UNCHECKED );
			break;
		case DMUSPROD_TIMELINE_SNAP_BEAT:
			::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_CHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_UNCHECKED );
			break;
		case DMUSPROD_TIMELINE_SNAP_BAR:
			::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_CHECKED );
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
		::EnableMenuItem( hMenuPopup, 10, MF_BYPOSITION | MF_GRAYED );
	}

	// Zoom items
	EnableMenuItem( hMenuPopup, ID_EDIT_ZOOMIN, ( m_svView == SV_NORMAL ) ?
					MF_ENABLED : MF_GRAYED );
	EnableMenuItem( hMenuPopup, ID_EDIT_ZOOMOUT, ( m_svView == SV_NORMAL ) ?
					MF_ENABLED : MF_GRAYED );

	m_pSequenceMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
	DestroyMenu(hMenu); // This will destroy the submenu as well.

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CSequenceStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);

	// Validate state
	if( !m_pSequenceMgr || !m_pSequenceMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Process the window message
	HRESULT hr = S_OK;

	switch( nMsg )
	{
	case WM_TIMER:
		OnTimer();
		break;

	case WM_SETFOCUS:
		hr = SetFocus( 1 );
		break;

	case WM_KILLFOCUS:
		// Stop any playing note
		m_pSequenceMgr->StopNote();

		hr = SetFocus( 0 );
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		hr = OnLButtonDown( wParam, lXPos, lYPos );
		break;

	case WM_LBUTTONUP:
		hr = OnLButtonUp( lXPos, lYPos );
		break;

/*
	case WM_LBUTTONUP:
		m_lXPos = lXPos;
		// Make sure everything on the timeline is deselected.
		m_fSelecting = TRUE;
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
		m_fSelecting = FALSE;
		hr = S_OK;
		break;*/

	case WM_RBUTTONDOWN:
		UpdateNoteCursorTime();

		{	// Update insert value
			long lNewPosition = PositionToMIDIValue( lYPos );
			if( m_lInsertVal != lNewPosition )
			{
				m_lInsertVal = lNewPosition;
				InvalidatePianoRoll();
			}
		}
		break;

	case WM_RBUTTONUP:
		{
			m_fShowSequenceProps = TRUE;

			UnselectGutterRange();
			if (m_svView != SV_MINIMIZED)
			{
				// Select the note click on iff
				CSequenceItem *pSeqItem = GetSeqItemAndRectFromPoint( lXPos, lYPos, NULL );
				if( (pSeqItem != NULL) && (m_SelectionMode != SEQ_MULTIPLE_SELECT) )
				{
					if (m_pActiveNote != pSeqItem)
					{
						m_pSequenceMgr->UnselectAllNotes();
						pSeqItem->m_fSelected = TRUE;
						m_pActiveNote = pSeqItem;
						m_pSelectPivotSeq = pSeqItem;
						m_SelectionMode = SEQ_SINGLE_SELECT;
						InvalidateStrip();
						m_pSequenceMgr->RefreshPropertyPage();
					}
				}
				else if( pSeqItem == NULL )
				{
					m_pSequenceMgr->UnselectAllNotes();
				}
			}

			m_pSequenceMgr->OnShowProperties();

			hr = OnRButtonUp( );
		}
		break;

	case WM_SETCURSOR:
		hr = OnSetCursor( lXPos, lYPos);
		break;

	case WM_MOUSEMOVE:
		hr = OnMouseMove( lXPos, lYPos );
		break;

	case WM_KEYDOWN:
		hr = OnKeyDown( wParam );
		break;

	case WM_CHAR:
		hr = OnChar( wParam );
		break;

	case WM_VSCROLL:
		hr = OnVScroll( );
		break;

	case WM_SIZE:
		if( wParam == SIZE_MAXIMIZED )
		{
			OnChangeStripView( SV_NORMAL );
		}
		else if( wParam == SIZE_MINIMIZED )
		{
			OnChangeStripView( SV_MINIMIZED );
		}
	
		{
			// Get the new length of the segment
			VARIANT varLength;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
			{
				if( varLength.vt == VT_I4 )
				{
					m_mtLength = V_I4( &varLength );
				}
			}
		}

		UpdateSequenceUIChunk();
	case WM_MOVE:
		hr = OnSize();
		break;

	case WM_DESTROY:
		hr = OnDestroy();
		break;

	case WM_CREATE:
		hr = OnCreate();
		break;

	case WM_MOUSEWHEEL:
		if (m_svView == SV_NORMAL)
		{
			long lNewVertScroll = m_lVScroll;
			// scroll the piano roll up and down
			short zDelta = HIWORD(wParam);
			if (zDelta>0)
			{
				lNewVertScroll -= 6 * ( m_lMaxNoteHeight);
			}
			else if (zDelta<0)
			{
				lNewVertScroll += 6 * ( m_lMaxNoteHeight);
			}

			if ( lNewVertScroll / m_lMaxNoteHeight > 127)
			{
				lNewVertScroll = m_lMaxNoteHeight * 127;
			}
			else if ( lNewVertScroll < 0)
			{
				lNewVertScroll = 0;
			}

			if ( lNewVertScroll != m_lVScroll )
			{
				SetVScroll( lNewVertScroll );
			}
		}
		break;

	case WM_COMMAND:
		// We should only get this message in response to a selection in the right-click context menu.
		//WORD wNotifyCode;
		WORD wID;

		//wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			hr = ShowPropertySheet(m_pSequenceMgr->m_pTimeline);
			if (m_fShowSequenceProps)
			{
				// Change to the sequence property page
				m_pSequenceMgr->OnShowProperties();
			}
			else
			{
				// Change to our property page
				OnShowProperties();
			}
			break;
		case ID_EDIT_CUT:
			hr = Cut();
			break;
		case ID_EDIT_COPY:
			hr = Copy();
			break;
		case ID_EDIT_DELETE:
			hr = Delete();
			break;
		case ID_EDIT_PASTE_MERGE:
			m_pSequenceMgr->m_pTimeline->SetPasteType( TL_PASTE_MERGE );
			hr = Paste();
			break;
		case ID_EDIT_PASTE_OVERWRITE:
			m_pSequenceMgr->m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
			hr = Paste();
			break;
		case ID_EDIT_PASTE:
			hr = Paste();
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		case ID_EDIT_QUANTIZE:
			hr = DoQuantize();
			break;
		case ID_EDIT_VELOCITY:
			hr = DoVelocity();
			break;
		case ID_EDIT_DELETE_TRACK:
		case ID_EDIT_ADD_TRACK:
			{
				HWND hwnd = GetTimelineHWnd();
				if( hwnd )
				{
					hwnd = ::GetParent( hwnd );
					if( hwnd )
					{
						::SendMessage( hwnd, WM_COMMAND, wParam, lParam );
					}
				}
			}
			break;

		case ID_EDIT_ADD_PART:
			hr = m_pSequenceMgr->AddNewPart();
			break;

		case ID_EDIT_DELETE_PART:
			{
				HWND hwnd = GetTimelineHWnd();
				if( hwnd )
				{
					hwnd = ::GetParent( hwnd );
					if( hwnd )
					{
						::SendMessage( hwnd, WM_COMMAND, MAKEWPARAM( ID_EDIT_DELETE_TRACK, HIWORD( wParam ) ), lParam );
					}
				}
			}
			break;

		case ID_EDIT_VIEW_PIANOROLL:
			// This handles notifying the other strips, and adding an Undo state
			ChangeNotationType( FALSE );
			break;

		case ID_EDIT_VIEW_HYBRID:
			// This handles notifying the other strips, and adding an Undo state
			ChangeNotationType( TRUE );
			break;

		case ID_SNAP_NONE:
		case ID_SNAP_GRID:
		case ID_SNAP_BEAT:
		case ID_SNAP_BAR:
		{
			DMUSPROD_TIMELINE_SNAP_TO tlSnapTo = DMUSPROD_TIMELINE_SNAP_NONE;
			switch( wID )
			{
			case ID_SNAP_NONE:
				tlSnapTo = DMUSPROD_TIMELINE_SNAP_NONE;
				break;
			case ID_SNAP_GRID:
				tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
				break;
			case ID_SNAP_BEAT:
				tlSnapTo = DMUSPROD_TIMELINE_SNAP_BEAT;
				break;
			case ID_SNAP_BAR:
				tlSnapTo = DMUSPROD_TIMELINE_SNAP_BAR;
				break;
			}
			VARIANT var;
			var.vt = VT_I4;
			V_I4( &var ) = tlSnapTo;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_SNAP_TO, var );
			break;
		}

		case ID_EDIT_ZOOMIN:
		case ID_EDIT_ZOOMOUT:
			// The following code is very similar to OnChar()
			//if( m_svView == SV_NORMAL )
			{
				if( wID == ID_EDIT_ZOOMIN )
				{
					ChangeZoom(	m_dblVerticalZoom + 0.01);
					// TODO: Add Undo?
					/*
					// Let the object know about the changes
					m_nLastEdit = nLastEdit;
					m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
					m_pSequenceMgr->OnDataChanged(); 
					*/
				}
				else if( wID == ID_EDIT_ZOOMOUT )
				{
					ChangeZoom(	m_dblVerticalZoom - 0.01);
					// TODO: Add Undo?
					/*
					// Let the object know about the changes
					m_nLastEdit = nLastEdit;
					m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
					m_pSequenceMgr->OnDataChanged(); 
					*/
				}
			}
			break;

		case ID_INSERT_NEWINSTRUMENT:
			{
				// Need to insert a new band track
				IDMUSProdSegmentEdit *pIDMUSProdSegmentEdit;
				if( SUCCEEDED( m_pSequenceMgr->m_pDMProdSegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit, (void **)&pIDMUSProdSegmentEdit ) ) )
				{
					// Assume the strip creation succeeds
					// BUGBUG: We will add an extra undo stip of AddStrip fails.  Oh well.
					m_nLastEdit = IDS_UNDO_ADD_BANDTRACK;
					m_pSequenceMgr->m_pTimeline->OnDataChanged( (ISequenceMgr*)m_pSequenceMgr );

					IUnknown *punkStripMgr;
					if( SUCCEEDED( pIDMUSProdSegmentEdit->AddStrip( CLSID_DirectMusicBandTrack, m_pSequenceMgr->m_dwGroupBits, &punkStripMgr ) ) )
					{
						IUnknown *pStripMgr;
						if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IUnknown, (void **)&pStripMgr) ) )
						{
							// Now, find the band strip for this band track
							IDMUSProdStrip *pIDMUSProdStrip;
							DWORD dwEnum = 0;
							VARIANT varStripMgr;
							while( SUCCEEDED( m_pSequenceMgr->m_pTimeline->EnumStrip( dwEnum, &pIDMUSProdStrip ) ) )
							{
								V_UNKNOWN(&varStripMgr) = NULL;
								if( SUCCEEDED( pIDMUSProdStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
								&&	V_UNKNOWN(&varStripMgr) )
								{
									if( V_UNKNOWN(&varStripMgr) == pStripMgr )
									{
										// Check if TP_FREEZE_UNDO is NOT set.
										BOOL fFreezeUndo = FALSE;
										VARIANT var;
										if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
										{
											fFreezeUndo = V_BOOL(&var);
										}

										if( !fFreezeUndo )
										{
											// Need to set TP_FREEZE_UNDO or the segment will add an undo state for us
											var.vt = VT_BOOL;
											V_BOOL(&var) = TRUE;
											m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
										}

										// Found the band strip - now insert a new band
										pIDMUSProdStrip->OnWMMessage( WM_COMMAND, ID_INSERT_NEWINSTRUMENT, 0, 0, 0 );

										if( !fFreezeUndo )
										{
											// Need to reset TP_FREEZE_UNDO or the segment will add an undo state for us
											var.vt = VT_BOOL;
											V_BOOL(&var) = FALSE;
											m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
										}

										// Flag to exit the loop (after doing the Release() calls below
										dwEnum = LONG_MAX;
									}
									V_UNKNOWN(&varStripMgr)->Release();
								}
								pIDMUSProdStrip->Release();

								dwEnum++;
							}
							pStripMgr->Release();
						}

						punkStripMgr->Release();
					}
					pIDMUSProdSegmentEdit->Release();
				}
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnCreate

HRESULT CSequenceStrip::OnCreate(void)
{
	UpdateName();
	m_iHaveFocus = 0;

	// Get Left and right selection boundaries
	m_bGutterSelected = FALSE;
	m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect );
	m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect );

	VARIANT var;
	m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIPVIEW, &var );
	m_svView = (STRIPVIEW) V_I4(&var);

	// Find the position to scroll to
	long lValue = -1;
	if( m_lVScroll == -1 )
	{
		// since this is a new strip, let's auto-scroll so the data is showing
		CSequenceItem *pSeqItem;
		POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
		while( pos )
		{
			pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
			if( pSeqItem->m_bByte1 > lValue )
			{
				lValue = pSeqItem->m_bByte1;
			}
		}

		// If the value is valid, 
		if( ( lValue >= 0 ) && ( lValue < 128 ) )
		{
			VARIANT var;
			var.vt = VT_I4;
			m_lVScroll = ( 127 - lValue ) * m_lMaxNoteHeight;
			V_I4(&var) = m_lVScroll;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );
		}
		else
		{
			VARIANT var;
			var.vt = VT_I4;
			// Default scroll position is C4
			m_lVScroll = ( 127 - DEFAULT_VERTICAL_SCROLL ) * m_lMaxNoteHeight;
			V_I4(&var) = m_lVScroll;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );
		}
	}
	else
	{
		m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, &var );
		m_lVScroll = V_I4(&var);
	}

	// Load button bitmaps
	if( InterlockedIncrement( &m_lBitmapRefCount ) == 1 )
	{
		// Load button bitmaps
		BITMAP bmParam;
		if( m_BitmapZoomInUp.GetSafeHandle() == NULL )
		{
			m_BitmapZoomInUp.LoadBitmap( IDB_ZOOMIN_UP );
			m_BitmapZoomInUp.GetBitmap( &bmParam );
			m_BitmapZoomInUp.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( m_BitmapZoomInDown.GetSafeHandle() == NULL )
		{
			m_BitmapZoomInDown.LoadBitmap( IDB_ZOOMIN_DOWN );
			m_BitmapZoomInDown.GetBitmap( &bmParam );
			m_BitmapZoomInDown.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( m_BitmapZoomOutUp.GetSafeHandle() == NULL )
		{
			m_BitmapZoomOutUp.LoadBitmap( IDB_ZOOMOUT_UP );
			m_BitmapZoomOutUp.GetBitmap( &bmParam );
			m_BitmapZoomOutUp.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( m_BitmapZoomOutDown.GetSafeHandle() == NULL )
		{
			m_BitmapZoomOutDown.LoadBitmap( IDB_ZOOMOUT_DOWN );
			m_BitmapZoomOutDown.GetBitmap( &bmParam );
			m_BitmapZoomOutDown.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( m_BitmapNewBand.GetSafeHandle() == NULL )
		{
			m_BitmapNewBand.LoadBitmap( IDB_NEWBAND );
			m_BitmapNewBand.GetBitmap( &bmParam );
			m_BitmapNewBand.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
	}

	// setup the vertical scrollbar
	if (m_VScrollBar.GetSafeHwnd() == NULL)
	{
		IOleWindow* pIOleWindow;
		m_pSequenceMgr->m_pTimeline->QueryInterface(IID_IOleWindow, (void**)&pIOleWindow);
		
		if (pIOleWindow)
		{
			HWND hWnd;
			if (pIOleWindow->GetWindow(&hWnd) == S_OK)
			{
				CWnd wnd;
				wnd.Attach(hWnd);

				m_rectVScroll = CRect(0, 0, 40, 100);
				m_VScrollBar.Create(SBS_RIGHTALIGN | SBS_VERT | WS_CHILD | WS_CLIPSIBLINGS,
					m_rectVScroll, &wnd, IDC_VSCROLL);
				m_VScrollBar.SetSequenceStrip(this);

				SCROLLINFO si;
				si.cbSize = sizeof( SCROLLINFO );
				si.fMask = SIF_POS | SIF_RANGE;
				si.nMin = 0;
				si.nMax = 127;
				if ( ( lValue >= 0 ) && ( lValue < 128 ) )
				{
					si.nPos = 127 - lValue;
				}
				else
				{
					si.nPos = 127 - DEFAULT_VERTICAL_SCROLL;
				}
				m_VScrollBar.SetScrollInfo( &si, TRUE );
				wnd.Detach();
			}
			pIOleWindow->Release();
			UpdateVScroll();
		}
	}

	VARIANT varLength;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
	{
		if( varLength.vt == VT_I4 )
		{
			m_mtLength = V_I4( &varLength );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::FBDraw

HRESULT CSequenceStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_pSequenceMgr->m_pTimeline )
	{
		return E_UNEXPECTED;
	}

	DrawFunctionBar( hDC, sv );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::FBOnWMMessage

HRESULT CSequenceStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		{
			BOOL fShowProps = TRUE;
			if( lYPos <= m_lVScroll + m_BitmapZoomInDown.GetBitmapDimension().cy + m_BitmapZoomOutDown.GetBitmapDimension().cy )
			{
				VARIANT varXS;
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
				{
					if( lXPos >= V_I4( &varXS ) - m_BitmapZoomInDown.GetBitmapDimension().cx - 2 )
					{
						fShowProps = FALSE;

						// capture mouse so we get the LBUTTONUP message as well
						// the timeline will release the capture when it receives the
						// LBUTTONUP message
						VARIANT var;
						var.vt = VT_BOOL;
						V_BOOL(&var) = TRUE;
						m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

						//CString strUndoText;

						if( lYPos <= m_lVScroll + m_BitmapZoomInDown.GetBitmapDimension().cy )
						{
							m_fZoomInPressed = TRUE;
							ChangeZoom(	m_dblVerticalZoom + 0.01);
							// TODO: Undo Zoom?
						}
						else
						{
							m_fZoomOutPressed = TRUE;
							ChangeZoom(	m_dblVerticalZoom - 0.01);
							// TODO: Undo Zoom?
						}
					}
				}
			}

			VARIANT varFNHeight;
			if( fShowProps
			&&	SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight ) ) )
			{
				if( lYPos > m_lVScroll + V_I4(&varFNHeight)
				&&	lYPos < m_lVScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
				{
					VARIANT varXS;
					if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
					{
						if( lXPos < V_I4( &varXS ) - m_rectVScroll.Width() - NEWBAND_BUTTON_WIDTH - 1 )
						{
							if( m_fInstrumentEnabled )
							{
								m_fInstrumentPressed = TRUE;
							}
						}
						else
						{
							m_fNewBandPressed = TRUE;
						}

						InvalidateFunctionBar();
						fShowProps = FALSE;
					}
				}
			}

			if( fShowProps )
			{
				m_fShowSequenceProps = FALSE;
				OnShowProperties();
				if( m_svView == SV_NORMAL )
				{
					// Play note with the value clicked on
					CSequenceItem seqItem;
					seqItem.m_mtDuration = 768;
					seqItem.m_bByte2 = 100;
					seqItem.m_bByte1 = BYTE(PositionToMIDIValue( lYPos ));
					m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &seqItem.m_mtTime );
					m_pSequenceMgr->PlayNote( &seqItem );

					// Move the insert cursor to the note the user clicked on
					if( seqItem.m_bByte1 != m_lInsertVal )
					{
						m_lInsertVal = seqItem.m_bByte1;
						InvalidatePianoRoll();
					}

					VARIANT var;
					var.vt = VT_BOOL;
					V_BOOL(&var) = TRUE;
					m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			VARIANT varFNHeight;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight ) ) )
			{
				if( lYPos > m_lVScroll + V_I4(&varFNHeight)
				&&	lYPos < m_lVScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
				{
					VARIANT varXS;
					if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
					{
						POINT point;
						GetCursorPos( &point );
						LPARAM lParam = MAKELPARAM(point.x, point.y);
						if( lXPos < V_I4( &varXS ) - m_rectVScroll.Width() - NEWBAND_BUTTON_WIDTH - 1 )
						{
							if( m_fInstrumentPressed )
							{
								::PostMessage( m_VScrollBar.m_hWnd, WM_APP, WM_APP_INSTRUMENTMENU, lParam );
							}
						}
						else
						{
							if( m_fNewBandPressed )
							{
								::PostMessage( m_VScrollBar.m_hWnd, WM_APP, WM_APP_BANDMENU, lParam );
							}
						}
					}
				}
			}
		}
		// Zoom buttons and StopNote handled on OnLButtonUp
		hr = OnLButtonUp( lXPos, lYPos );
		m_MouseMode = SEQ_MM_NORMAL;
		break;

	case WM_RBUTTONUP:
		m_fShowSequenceProps = FALSE;
		OnShowProperties();
		m_lXPos = -1;

		// Display a right-click context menu.
		OnRButtonUp();
		break;

	case WM_SETCURSOR:
		m_hCursor = GetArrowCursor();
		break;

	case WM_MOUSEMOVE:
		if (m_svView != SV_MINIMIZED)
		{
			hr = OnMouseMove( lXPos, lYPos );
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Cut

HRESULT CSequenceStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	hr = CanCut();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	// Cut is simply a Copy followed by a Delete.
	hr = Copy(pITimelineDataObject);
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Copy

HRESULT CSequenceStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamCopy;
	IStream*			pStreamMidiCopy;

#ifdef _DEBUG
	//TraceFormatsInClipboard();
#endif

	hr = CanCopy();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, fail
	// The formats are registered in OnAddedToTimeline
	if( m_cfSequenceList == 0 || m_cfMidiFile == 0)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Create an IStream to save the selected notes as a Seuqnce chunk in.
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Create an IStream to save the selected notes as a MIDI file in
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamMidiCopy);
	if( FAILED( hr ))
	{
		pStreamCopy->Release();
		return E_OUTOFMEMORY;
	}

	if( m_bGutterSelected && (m_lBeginSelect >= 0) && (m_lEndSelect > 0))
	{
		// Save the notes into the stream.
		hr = m_pSequenceMgr->SaveSelectedEvents( pStreamCopy, m_lBeginSelect );
		if( FAILED( hr ))
		{
			pStreamCopy->Release();
			pStreamMidiCopy->Release();
			return E_UNEXPECTED;
		}

		// Save into Midi Stream
		hr = m_pSequenceMgr->SaveSelectedEventsAsMidi( pStreamMidiCopy );
		if( FAILED( hr ))
		{
			pStreamCopy->Release();
			pStreamMidiCopy->Release();
			return E_UNEXPECTED;
		}
		else if( hr == S_FALSE )
		{
			// No data saved - don't add pStreamMidiCopy
			pStreamMidiCopy->Release();
			pStreamMidiCopy = NULL;
		}
	}
	else
	{
		// Save the notes into the stream.
		CSequenceItem* pSequenceAtDragPoint = m_pSequenceMgr->FirstSelectedSequence();
		if( pSequenceAtDragPoint == NULL )
		{
			pStreamCopy->Release();
			pStreamMidiCopy->Release();
			return E_UNEXPECTED;
		}

		hr = m_pSequenceMgr->SaveSelectedSequences( pStreamCopy, pSequenceAtDragPoint->AbsTime() );
		if( FAILED( hr ))
		{
			pStreamCopy->Release();
			pStreamMidiCopy->Release();
			return E_UNEXPECTED;
		}

		// Save into Midi Stream
		hr = m_pSequenceMgr->SaveSelectedSequencesAsMidi( pStreamMidiCopy, pSequenceAtDragPoint->AbsTime() );
		if( FAILED( hr ))
		{
			pStreamCopy->Release();
			pStreamMidiCopy->Release();
			return E_UNEXPECTED;
		}
		else if( hr == S_FALSE )
		{
			// No data saved - don't add pStreamMidiCopy
			pStreamMidiCopy->Release();
			pStreamMidiCopy = NULL;
		}
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfSequenceList, pStreamCopy );
		if( pStreamMidiCopy && (hr == S_OK) )
		{
			hr = pITimelineDataObject->AddExternalClipFormat( m_cfMidiFile, pStreamMidiCopy );
		}
		pStreamCopy->Release();
		if( pStreamMidiCopy )
		{
			pStreamMidiCopy->Release();
			//pStreamMidiCopy = NULL; // Not necessary - not used below this point
		}
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			return E_FAIL;
		}
	}
	// Otherwise, add it to the clipboard
	else
	{
		// create timeline object
		hr = m_pSequenceMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if( hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and end time of this copy
		long lStartTime, lEndTime;
		m_pSequenceMgr->GetBoundariesOfSelectedEvents(lStartTime, lEndTime);
		hr = pITimelineDataObject->SetBoundaries(lStartTime, lEndTime);

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfSequenceList, pStreamCopy );
		pStreamCopy->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}
		if( pStreamMidiCopy )
		{
			hr = pITimelineDataObject->AddExternalClipFormat( m_cfMidiFile, pStreamMidiCopy );
			pStreamMidiCopy->Release();
			//pStreamMidiCopy = NULL; // Not necessary - not used below this point
		}
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// get the new IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export(&pIDataObject);

		// Release the ITimelineDataObject
		pITimelineDataObject->Release();

		// Exit if the Export failed
		if(FAILED(hr))
		{
			return E_UNEXPECTED;
		}

		// Send the IDataObject to the clipboard
		hr = OleSetClipboard( pIDataObject );

		// Make sure the clipboard has a copy of the data
		OleFlushClipboard();

		// Release our reference to the data object
		pIDataObject->Release();

		if( hr != S_OK )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::PasteAt

HRESULT CSequenceStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lClocks, BOOL fDropNotEditPaste, BOOL fPasteMIDIToMultipleStrips)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(fDropNotEditPaste);

	if( (m_pSequenceMgr->m_pTimeline == NULL)
	||	(pITimelineDataObject == NULL) )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if(lClocks < 0)
		lClocks = 0;	// no negative time

	if( S_OK ==	pITimelineDataObject->IsClipFormatAvailable( m_cfSequenceList) )
	{
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfSequenceList, &pIStream)))
		{
			// 20171: Suck up the first copy of the data in MIDI format, if it's available.
			if( S_OK ==	pITimelineDataObject->IsClipFormatAvailable( m_cfMidiFile) )
			{
				IStream* pIStreamMIDI;
				if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfMidiFile, &pIStreamMIDI)))
				{
					pIStreamMIDI->Release();
					pIStreamMIDI = NULL;
				}
			}

			// Check for RIFF format
			if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				goto Leave;
			}

			ASSERT(hr == S_OK);

			MMCKINFO ck;
			ck.ckid = DMUS_FOURCC_SEQ_TRACK;
			if ( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) == 0 )
			{
				// Calculate the offset
				long lTimeOffset = m_pSequenceMgr->CalculatePasteTime( lClocks );

				// Load the Track
				MMCKINFO ck1;
				BOOL fUnselectedNotes = FALSE, fUnselectedCurves = FALSE;
				while( pIRiffStream->Descend( &ck1, NULL, 0 ) == 0 )
				{
					switch( ck1.ckid )
					{
					case DMUS_FOURCC_SEQ_LIST:
						// If necessary, unselect all existing notes
						if( !fUnselectedNotes )
						{
							fUnselectedNotes = TRUE;
							m_pSequenceMgr->UnselectAllNotes( );
						}

						hr = m_pSequenceMgr->ImportSequenceChunkData( pIStream, ck1.cksize, lTimeOffset );
						if( FAILED(hr) )
						{
							goto Leave;
						}
						break;
					case DMUS_FOURCC_CURVE_LIST:
						// If necessary, unselect all existing curves
						if( !fUnselectedCurves )
						{
							fUnselectedCurves = TRUE;
							m_pSequenceMgr->UnselectAllCurves( );
						}

						hr = m_pSequenceMgr->ImportCurveChunkData( pIStream, ck1.cksize, lTimeOffset );
						if( FAILED(hr) )
						{
							goto Leave;
						}
						break;
					}
					pIRiffStream->Ascend(&ck1, 0);
				}

				pIRiffStream->Ascend( &ck, 0 );
				if( SUCCEEDED(hr) )
				{
					SyncCurveStripStateList();
					AddCurveStrips();
					m_nLastEdit = IDS_UNDO_PASTE;
					m_pSequenceMgr->OnDataChanged();
					InvalidateStrip();
					InvalidateCurveStrips();
				}
			}
		}
	}
	else if( S_OK == pITimelineDataObject->IsClipFormatAvailable(m_cfMidiFile))
	{
		hr = PasteMidiFile(pITimelineDataObject, lClocks, fPasteMIDIToMultipleStrips);
		if( SUCCEEDED( hr ) )
		{
			SyncCurveStripStateList();
			AddCurveStrips();
			m_nLastEdit = IDS_UNDO_PASTE_MIDI;
			m_pSequenceMgr->OnDataChanged();
			InvalidateStrip();
			InvalidateCurveStrips();
		}
	}

	UpdateSelectionState();
Leave:
	if ( pIRiffStream )
	{
		pIRiffStream->Release();
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Paste

HRESULT CSequenceStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL || m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Make sure everything on the timeline is deselected.
	// JHD 8/5/98: I don't know why everybody does this..
	/*
	m_fSelecting = TRUE;
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_fSelecting = FALSE;
	*/
	
	// If the format hasn't been registered yet, fail
	// The formats are registered in OnAddedToTimeline
	if( m_cfSequenceList == 0 || m_cfMidiFile == 0)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// If true, split a MIDI file out into the gutter selected strips
	BOOL fPasteMIDIToMultipleStrips = FALSE;
	
	// Get the time to paste at
	MUSIC_TIME mtTime;

	if(pITimelineDataObject == NULL)
	{
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		hr = OleGetClipboard(&pIDataObject);
		if(FAILED(hr) || (pIDataObject == NULL))
		{
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		hr = m_pSequenceMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		pIDataObject->Release();
		if( FAILED(hr) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// If this is a single-strip paste, use our m_lInsertTime position, since it should already have
		// been set using the snap-to value
		mtTime = m_lInsertTime;
	}
	else
	{
		fPasteMIDIToMultipleStrips = TRUE;

		// If this is a multiple-strip paste, then use the Time Cursor, since it should already have
		// been set using the snap-to value
		if (FAILED(m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime )))
		{
			return E_FAIL;
		}

		pITimelineDataObject->AddRef();
	}

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pSequenceMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	// Quantize it to the nearest grid
	mtTime = FloorTimeToGrid( mtTime, NULL );

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		long lStart, lEnd;
		if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
		{
			m_pSequenceMgr->DeleteBetweenTimes( mtTime, mtTime + lEnd - lStart );
		}
	}

	hr = PasteAt(pITimelineDataObject, mtTime, false, fPasteMIDIToMultipleStrips);
	if(SUCCEEDED(hr))
	{
		InvalidateStrip();
		m_pSequenceMgr->RefreshPropertyPage();
		m_pSequenceMgr->OnShowProperties();
	}
	pITimelineDataObject->Release();

	return hr;
}

CSequenceItem *CSequenceStrip::CreateNoteToInsert( void ) const
{
	long lValue = m_lInsertVal;

	if (lValue < 0)
	{
		lValue = 0;
	}

	if (lValue > 127)
	{
		lValue = 127;
	}

	CSequenceItem *pSeqItem = new CSequenceItem;
	if( pSeqItem )
	{
		pSeqItem->m_bByte1 = BYTE(lValue);
		pSeqItem->m_bByte2 = 100;
		pSeqItem->m_bStatus = MIDI_NOTEON;
		if( m_lInsertTime < 0 )
		{
			// Set the time of the note to 0
			pSeqItem->m_mtTime = 0;

			// Find out the TimeSig of the first measure
			DMUS_TIMESIGNATURE dmTimeSig;
			GetTimeSig( 0, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );

			// Compute the smallest possible offset before time 0
			const long lNegativeMeasureLength = max( SHRT_MIN, -dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat) );

			// Set the note's offset
			pSeqItem->m_nOffset = short( max( m_lInsertTime, lNegativeMeasureLength) );
		}
		else
		{
			pSeqItem->m_mtTime = FloorTimeToGrid( m_lInsertTime, NULL );
			ASSERT( abs(m_lInsertTime - pSeqItem->m_mtTime) <= (SHRT_MAX + 1));
			pSeqItem->m_nOffset = short(m_lInsertTime - pSeqItem->m_mtTime);
		}
		/// Minimum note duration is 1/2 grid
		pSeqItem->m_mtDuration = max( GetSnapAmount(pSeqItem->AbsTime()) - 1, GetGridClocks(pSeqItem->AbsTime()) / 2 );
		pSeqItem->m_fSelected = TRUE;
	}
	return pSeqItem;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Insert

HRESULT CSequenceStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();

	// Unselect all notes, so only the new note is selected
	m_pSequenceMgr->UnselectAllNotes( );

	CSequenceItem *pSeqItem = CreateNoteToInsert();
	if( pSeqItem == NULL )
	{
		return E_FAIL;
	}

	if( !m_pActiveNote )
	{
		m_pActiveNote = pSeqItem;
	}

	m_pSequenceMgr->InsertNote( pSeqItem, TRUE );
	m_pSequenceMgr->RefreshPropertyPage();
	m_pSequenceMgr->OnShowProperties();

	UpdateSelectionState();

	// Bump Time cursor to the right
	SNAPTO snapTo = GetSnapToBoundary( m_lInsertTime );
	if( snapTo == SNAP_NONE )
	{
		snapTo = SNAP_GRID;
	}
	BumpTimeCursor( snapTo );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Delete

HRESULT CSequenceStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/* This may be part of a Cut operation. (You can cut empty space, but you can't delete it)
	if( CanDelete() != S_OK )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}
	*/

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	m_pSequenceMgr->DeleteSelectedSequences();

	m_SelectionMode = SEQ_NO_SELECT;
	m_pActiveNote = NULL;

	InvalidateStrip();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SelectAll

HRESULT CSequenceStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	// This takes care of updating the property page and redrawing the strip, if necessary
	m_pSequenceMgr->SelectAllNotes();

	UpdateSelectionState();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanCut

HRESULT CSequenceStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	if( CanCopy() == S_OK && CanDelete() == S_OK )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}

}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanCopy

HRESULT CSequenceStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pSequenceMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanPaste

HRESULT CSequenceStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;

	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, fail
	// The formats are registered in OnAddedToTimeline
	if( m_cfSequenceList == 0 || m_cfMidiFile == 0)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfSequenceList );
		if(hr != S_OK)
		{
			hr = pITimelineDataObject->IsClipFormatAvailable( m_cfMidiFile );
		}
	}
	// Otherwise, check the clipboard
	else
	{
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;

		hr = E_FAIL;
		if( SUCCEEDED( OleGetClipboard(&pIDataObject) ) )
		{
			// Create a new TimelineDataObject
			IDMUSProdTimelineDataObject *pITimelineDataObject;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfSequenceList);
				}
				if(hr != S_OK)
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfMidiFile);
				}
				pITimelineDataObject->Release();
			}
			pIDataObject->Release();
		}
	}

	if (hr == S_OK)
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanInsert

HRESULT CSequenceStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_svView == SV_MINIMIZED)
	{
		return S_FALSE;
	}
	if ( m_lInsertVal >= 0 )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanDelete

HRESULT CSequenceStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pSequenceMgr != NULL );
	if( m_pSequenceMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	return m_pSequenceMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanSelectAll

HRESULT CSequenceStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pSequenceMgr->m_lstSequences.IsEmpty() )
	{
		return S_FALSE;
	}
	else
	{
		return S_OK;
	}
}


// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DragEnter

HRESULT CSequenceStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DragOver

HRESULT CSequenceStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pt);
	UNREFERENCED_PARAMETER(grfKeyState);

	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( CanPasteFromData( m_pITargetDataObject ) == S_OK )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			dwEffect = *pdwEffect;
		}
		else
		{
			if( grfKeyState & MK_CONTROL )
			{
				dwEffect = DROPEFFECT_COPY;
			}
			else
			{
				if( *pdwEffect & DROPEFFECT_COPY
				&&  *pdwEffect & DROPEFFECT_MOVE )
				{
					dwEffect = DROPEFFECT_MOVE;
				}
				else
				{
					dwEffect = *pdwEffect;
				}
			}
		}
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
	}

	*pdwEffect = dwEffect;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DragLeave

HRESULT CSequenceStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::Drop

HRESULT CSequenceStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(grfKeyState);

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		HMENU hMenu;
		HMENU hMenuPopup;
		
		// Display arrow cursor
		::LoadCursor( AfxGetInstanceHandle(), IDC_ARROW );
	
		// Prepare context menu
		hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_DRAG_RMENU) );
		if( hMenu )
		{
			m_dwDragRMenuEffect = DROPEFFECT_NONE;

			// Track right context menu for drag-drop via TrackPopupMenu
			hMenuPopup = ::GetSubMenu( hMenu, 0 );

			// Init state of menu items
			if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
			{
				::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// Get a window to attach menu to
			HWND hwnd = GetTimelineHWnd();
			if( hwnd )
			{
				// Display and track menu
				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  pt.x, pt.y, 0, hwnd, NULL );
				DestroyMenu( hMenu );

				// Need to process WM_COMMAND from TrackPopupMenu
				MSG msg;
				while( ::PeekMessage( &msg, hwnd, NULL, NULL, PM_REMOVE) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}

				// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
				m_dwOverDragEffect = m_dwDragRMenuEffect;
				m_dwDragRMenuEffect = DROPEFFECT_NONE;
			}
		}
	}

	// Paste data
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->PositionToClocks( pt.x, &pt.x ) ) )
				{
					hr = PasteAt( pITimelineDataObject, pt.x, true, FALSE );
					if( SUCCEEDED ( hr ) )
					{
						*pdwEffect = m_dwOverDragEffect;
					}
				}
			}
			pITimelineDataObject->Release();
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetTimelineHWnd

HWND CSequenceStrip::GetTimelineHWnd()
{
	// Get the DC of our Strip
	if( m_pSequenceMgr->m_pTimeline )
	{
		IOleWindow *pIOleWindow;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow ) ) )
		{
			HWND hwnd = NULL;
			pIOleWindow->GetWindow( &hwnd );
			pIOleWindow->Release();
			return hwnd;
		}
	}
	return NULL;;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CanPasteFromData

HRESULT CSequenceStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfSequenceList ) ))
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetData

HRESULT CSequenceStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (ppData == NULL) || (ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case 0:
	{
		ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(*ppData);
		pioGroupBitsPPG->dwGroupBits = m_pSequenceMgr->m_dwGroupBits;;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = static_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pSequenceMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pSequenceMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	case 2:
	{
		ioSeqTrackPPG *pioSeqTrackPPG = static_cast<ioSeqTrackPPG *>(*ppData);

		pioSeqTrackPPG->dwPChannel = m_pSequenceMgr->m_dwPChannel;
		pioSeqTrackPPG->crSelectedNoteColor = m_crSelectedNoteColor;
		pioSeqTrackPPG->crUnselectedNoteColor = m_crUnselectedNoteColor;
		pioSeqTrackPPG->crOverlappingNoteColor = m_crOverlappingNoteColor;
		pioSeqTrackPPG->crAccidentalColor = m_crAccidentalColor;
		pioSeqTrackPPG->strName = m_pSequenceMgr->m_strName;
		pioSeqTrackPPG->dwExtraBars = m_dwExtraBars;
		pioSeqTrackPPG->fPickupBar = m_fPickupBar;

		// Get PChannel name
		WCHAR wstrName[MAX_PATH];
		if( m_pSequenceMgr->m_pIPChannelName &&
			SUCCEEDED( m_pSequenceMgr->m_pIPChannelName->GetPChannelName( m_pSequenceMgr->m_dwPChannel, wstrName) ) )
		{
			pioSeqTrackPPG->strPChannelName = wstrName;
		}
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SetData

HRESULT CSequenceStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	
	BOOL fNeedToRefresh = FALSE;
	BOOL fNeedToReposition = TRUE;

	switch( *pdwIndex )
	{
	case 0:
	{
		ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(pData);

		if( pioGroupBitsPPG->dwGroupBits != m_pSequenceMgr->m_dwGroupBits )
		{
			m_nLastEdit = IDS_UNDO_TRACK_GROUP;
			m_pSequenceMgr->m_dwGroupBits = pioGroupBitsPPG->dwGroupBits;
			m_pSequenceMgr->UpdateFlatsAndKey();
			UpdateName();
			m_pSequenceMgr->m_pTimeline->OnDataChanged( (ISequenceMgr*)m_pSequenceMgr );
			m_pSequenceMgr->m_dwOldGroupBits = pioGroupBitsPPG->dwGroupBits;
			// Need to reposition to make Curve strips show up in the correct place
			// Handled by the removing and re-adding of this strip by the segment designer.
			//fNeedToReposition = FALSE;
		}
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = static_cast<PPGTrackFlagsParams *>(pData);

		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pSequenceMgr->m_dwTrackExtrasFlags )
		{
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pSequenceMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			m_pSequenceMgr->m_pTimeline->OnDataChanged( (ISequenceMgr*)m_pSequenceMgr );
		}
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pSequenceMgr->m_dwProducerOnlyFlags )
		{
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pSequenceMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
			m_pSequenceMgr->m_pTimeline->OnDataChanged( (ISequenceMgr*)m_pSequenceMgr );
		}
		break;
	}
	case 2:
	{
		ioSeqTrackPPG *pioSeqTrackPPG = static_cast<ioSeqTrackPPG *>(pData);

		if( pioSeqTrackPPG->dwPChannel != m_pSequenceMgr->m_dwPChannel )
		{
			m_nLastEdit = IDS_UNDO_PCHANNEL_CHANGE;
			m_pSequenceMgr->m_dwPChannel = pioSeqTrackPPG->dwPChannel;
			UpdateName();
			fNeedToRefresh = TRUE;
		}
		else if( pioSeqTrackPPG->crSelectedNoteColor != m_crSelectedNoteColor ||
				 pioSeqTrackPPG->crUnselectedNoteColor != m_crUnselectedNoteColor ||
				 pioSeqTrackPPG->crOverlappingNoteColor != m_crOverlappingNoteColor ||
				 pioSeqTrackPPG->crAccidentalColor != m_crAccidentalColor )
		{
			m_crSelectedNoteColor = pioSeqTrackPPG->crSelectedNoteColor;
			m_crUnselectedNoteColor = pioSeqTrackPPG->crUnselectedNoteColor;
			m_crOverlappingNoteColor = pioSeqTrackPPG->crOverlappingNoteColor;
			m_crAccidentalColor = pioSeqTrackPPG->crAccidentalColor;

			m_nLastEdit = IDS_UNDO_CHANGE_COLOR;
			InvalidateCurveStrips();
			InvalidateStrip();
			fNeedToReposition = FALSE;

			// This must be done before NotifyStripMgrs is called
			m_pSequenceMgr->OnDataChanged();
			m_pSequenceMgr->m_pTimeline->NotifyStripMgrs( GUID_Sequence_Color_Change, 0xFFFFFFFF, pioSeqTrackPPG );
		}
		else if( pioSeqTrackPPG->strName != m_pSequenceMgr->m_strName )
		{
			m_nLastEdit = IDS_UNDO_NAME_CHANGE;
			m_pSequenceMgr->m_strName = pioSeqTrackPPG->strName;
			UpdateName();
			fNeedToRefresh = TRUE;
		}
		else if( (pioSeqTrackPPG->dwExtraBars != m_dwExtraBars)
			 ||  (pioSeqTrackPPG->fPickupBar != m_fPickupBar) )
		{
			const BOOL fOrigPickup = m_fPickupBar;
			m_fPickupBar = pioSeqTrackPPG->fPickupBar;
			m_fPickupBar = ShouldDisplayPickupBar();
			const DWORD dwOrigExtra = m_dwExtraBars;
			m_dwExtraBars = pioSeqTrackPPG->dwExtraBars;
			m_dwExtraBars = GetNumExtraBars();

			if( fOrigPickup != m_fPickupBar )
			{
				m_nLastEdit = IDS_UNDO_PICKUP_CHANGE;
				fNeedToRefresh = TRUE;
			}
			if ( dwOrigExtra != m_dwExtraBars )
			{
				m_nLastEdit = IDS_UNDO_EXTRABARS_CHANGE;
				fNeedToRefresh = TRUE;
			}

			if( (pioSeqTrackPPG->dwExtraBars != m_dwExtraBars)
			||	(pioSeqTrackPPG->fPickupBar != m_fPickupBar) )
			{
				m_pPropPageMgr->RefreshData();
			}
		}
		else if( m_pSequenceMgr->m_pIPChannelName )
		{
			// PChannel name
			WCHAR wstrName[MAX_PATH];
			if( m_pSequenceMgr->m_pIPChannelName &&
				SUCCEEDED( m_pSequenceMgr->m_pIPChannelName->GetPChannelName( m_pSequenceMgr->m_dwPChannel, wstrName) ) )
			{
				CString strPChName = wstrName;
				if( strPChName.Compare( pioSeqTrackPPG->strPChannelName ) != 0 )
				{
					MultiByteToWideChar( CP_ACP, 0, pioSeqTrackPPG->strPChannelName, -1, wstrName, MAX_PATH );

					m_pSequenceMgr->m_pIPChannelName->SetPChannelName( m_pSequenceMgr->m_dwPChannel, wstrName );

					UpdateName();
					InvalidateStrip();

					// Let the object know about the changes
					// This doesn't affect anything in the segment - just the PChannel names
					// in the Producer project file.
				}
			}
		}

		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}
	if( fNeedToReposition )
	{
		// Remove and re-add ourselves to make our position in the Timeline correct
		ASSERT( m_cRef > 1 );
		m_pSequenceMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)this );
		m_pSequenceMgr->m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)this, CLSID_DirectMusicSeqTrack, m_pSequenceMgr->m_dwGroupBits, PChannelToStripIndex( m_pSequenceMgr->m_dwPChannel ) );

		// Initialize our UI data
		ApplyUIChunk( &m_pSequenceMgr->m_SeqStripDesign );

		// Add Curve Strips to the Timeline
		AddCurveStrips();

		VARIANT var;
		var.vt = VT_UNKNOWN;
		if( SUCCEEDED( QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
		{
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
		}
	}

	if( fNeedToRefresh )
	{
		m_pSequenceMgr->OnDataChanged();
		InvalidateStrip();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnShowProperties

HRESULT CSequenceStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	if (pIFramework == NULL)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}


	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	pIFramework->Release();
	if( pIPropSheet == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//  If the property sheet is hidden, exit
	if( pIPropSheet->IsShowing() != S_OK )
	{
		pIPropSheet->Release();
		return S_OK;
	}

	// release our reference to the property sheet
	pIPropSheet->Release();

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pPropPageMgr != NULL );
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr;
		if( NULL == pPPM ) return E_OUTOFMEMORY;
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
		{
			goto EXIT;
		}
	}

	// Set the property page to refer to the groupbits or sequence track property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pSequenceMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnRemoveFromPageManager

HRESULT CSequenceStrip::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->SetObject(NULL);
	}
	m_fPropPageActive = FALSE;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::ShowPropertySheet

HRESULT CSequenceStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
{
	HRESULT hr = S_OK;

	// Get a pointer to the property sheet and show it
	VARIANT			var;
	LPUNKNOWN		punk;
	IDMUSProdPropSheet*	pIPropSheet;
	pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		punk = V_UNKNOWN( &var );
		if( punk )
		{
			hr = punk->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
			ASSERT( SUCCEEDED( hr ));
			if( FAILED( hr ))
			{
				hr = E_UNEXPECTED;
			}
			else
			{
				pIPropSheet->Show( TRUE );
				pIPropSheet->Release();
			}
			punk->Release();
		}
	}
//	m_pSequenceMgr->OnShowProperties();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnLButtonDown

HRESULT CSequenceStrip::OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos)
{
	// If we're already dragging, just return
	/* TODO: Need another check
	if( m_pDragImage )
	{
		return S_OK;
	}
	*/

	UpdateNoteCursorTime();

	// Validate reference to the timeline
	if( !m_pSequenceMgr->m_pTimeline )
	{
		return E_FAIL;
	}

	// Unselect all other strips
	UnselectGutterRange();

	if (m_svView == SV_MINIMIZED)
	{
		return S_OK;
	}

	HRESULT hr = S_OK;

	BOOL fNeedToRefresh = FALSE;

	// See if there is a sequence under the cursor.
	CSequenceItem* pSequence = GetSeqItemAndRectFromPoint( lXPos, lYPos, NULL);
	if( pSequence != NULL )
	{
		// Update the insert cursor position
		/*
		if( m_lInsertVal != pSequence->m_bByte1 )
		{
			m_lInsertVal = pSequence->m_bByte1;
			InvalidatePianoRoll();
		}
		*/

		if (wParam & MK_CONTROL)
		{
			m_pActiveNote = pSequence;
			if( pSequence->m_fSelected )
			{
				m_fCtrlKeyDown = 1;
			}
			else
			{
				m_fCtrlKeyDown = 2;
				// Select
				m_pSelectPivotSeq = pSequence;
				pSequence->m_fSelected = TRUE;
				UpdateSelectionState();
				fNeedToRefresh = TRUE;
			}
		}
		else if (wParam & MK_SHIFT)
		{
			m_pActiveNote = pSequence;
			RECT rect1, rect2;

			if ( m_pSelectPivotSeq == NULL )
			{
				m_pSelectPivotSeq = pSequence;
			}

			GetNoteRect( m_pSelectPivotSeq, &rect1 );
			GetNoteRect( pSequence, &rect2 );

			// The '+1' is because CRect::PtInRect makes the left and top side 'inside' the rect, but the
			// right and bottom sides are 'outside' the rect.  By adding one to the right and bottom, the
			// real edge is now 'inside' the rect.
			CRect rect( min(rect1.left, rect2.left), min(rect1.top, rect2.top), max(rect1.left, rect2.left) + 1, max(rect1.bottom, rect2.bottom) + 1);
			
			if( SelectInRect( &rect ) )
			{
				// If something changed, invalidate the strip
				fNeedToRefresh = TRUE;
			}
		}
		else if (pSequence->m_fSelected == FALSE)
		{
			m_pSequenceMgr->UnselectAllNotes( );
			pSequence->m_fSelected = TRUE;
			m_pActiveNote = pSequence;
			m_pSelectPivotSeq = pSequence;
			m_SelectionMode = SEQ_SINGLE_SELECT;
			fNeedToRefresh = TRUE;
		}
		else
		{
			// The note's already selected, why re-select it? -jd
			//pSequence->m_fSelected = TRUE;
			m_pSelectPivotSeq = pSequence;
			m_pActiveNote = pSequence;
		}

		// Update the status bar
		m_pSequenceMgr->UpdateStatusBarDisplay();

		// Play the note
		m_pSequenceMgr->PlayNote( pSequence );
	}
	else
	{
		long lNewPosition = PositionToMIDIValue( lYPos );
		if( m_lInsertVal != lNewPosition )
		{
			m_lInsertVal = lNewPosition;
			InvalidatePianoRoll();
		}

		if (wParam & MK_CONTROL)
		{
			// Insert note;

			// Make sure everything on the timeline is deselected first.
			UnselectGutterRange();

			// Unselect all notes, so only the new note is selected
			m_pSequenceMgr->UnselectAllNotes( );

			pSequence = CreateNoteToInsert();
			if( pSequence != NULL )
			{
				if( !m_pActiveNote )
				{
					m_pActiveNote = pSequence;
				}

				m_pSequenceMgr->InsertNote( pSequence, FALSE );
				m_pSequenceMgr->RefreshPropertyPage();
				m_pSequenceMgr->OnShowProperties();

				UpdateSelectionState();

				// Bump Time cursor to the right
				SNAPTO snapTo = GetSnapToBoundary( m_lInsertTime );
				if( snapTo == SNAP_NONE )
				{
					snapTo = SNAP_GRID;
				}
				BumpTimeCursor( snapTo );

				hr = S_OK;
				m_fInsertingNoteFromMouseClick = true;
				m_MouseMode = SEQ_MM_RESIZE_END;
			}
			else
			{
				hr = E_FAIL;
			}
		}
	}

	m_pointClicked.x = lXPos;
	m_pointClicked.y = lYPos;

	switch(m_MouseMode)
	{
	case SEQ_MM_NORMAL:
		if (!(wParam & MK_CONTROL))
		{
			m_pActiveNote = NULL;
			m_pSequenceMgr->UnselectAllNotes( );
			m_SelectionMode = SEQ_NO_SELECT;

			// Create bounding box
			IOleWindow* pIOleWindow;
			m_pSequenceMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
			if( pIOleWindow )
			{
				CWnd wnd;
				HWND hWnd;

				if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
				{
					CPoint point( lXPos, lYPos );

					if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->StripToWindowPos( this, &point ) ) )
					{
						wnd.Attach( hWnd );
						CNoteTracker noteTracker( &wnd, m_pSequenceMgr->m_pTimeline, this, NTRK_SELECTING );
						noteTracker.TrackRubberBand( &wnd, point, TRUE );
						wnd.Detach();

						// Set m_SelectionMode based on the number of selected notes
						UpdateSelectionState();
					}
				}

				pIOleWindow->Release();
			}
		}

		// leave m_pShiftDMNote at whatever it last was
		//m_pShiftDMNote = NULL;
		
		// update our display and the property page
		InvalidateStrip();
		m_pSequenceMgr->OnShowProperties();
		break;
	case SEQ_MM_MOVE:
		if (pSequence)
		{
			m_OriginalSeqItem = *pSequence;
			m_lLastDeltaMove = 0;
			m_cLastDeltaValue = 0;
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = SEQ_MM_ACTIVEMOVE;
		}
		break;
	case SEQ_MM_RESIZE_START:
		if (pSequence)
		{
			m_OriginalSeqItem = *pSequence;
			m_lLastDeltaStart = 0;
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = SEQ_MM_ACTIVERESIZE_START;
		}
		break;
	case SEQ_MM_RESIZE_END:
		if (pSequence)
		{
			m_OriginalSeqItem = *pSequence;
			m_lLastDeltaDur = 0;
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = SEQ_MM_ACTIVERESIZE_END;
		}
		break;
	case SEQ_MM_RESIZE_VELOCITY:
		if (pSequence)
		{
			m_OriginalSeqItem = *pSequence;
			m_lLastDeltaVel = 0;
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = SEQ_MM_ACTIVERESIZE_VELOCITY;
		}
		break;
	case SEQ_MM_ACTIVERESIZE_VELOCITY:
	case SEQ_MM_ACTIVERESIZE_END:
	case SEQ_MM_ACTIVERESIZE_START:
	case SEQ_MM_ACTIVEMOVE:
		// This shouldn't happen...
		ASSERT(FALSE);
		break;
	default:
		break;
	}

	m_fShowSequenceProps = TRUE;

	if( fNeedToRefresh )
	{
		InvalidateStrip();
		m_pSequenceMgr->RefreshPropertyPage();
	}

	m_pSequenceMgr->OnShowProperties();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnLButtonUp

HRESULT CSequenceStrip::OnLButtonUp( long lXPos, long lYPos)
{
	if (m_svView == SV_MINIMIZED)
	{
		return S_OK;
	}

	// Stop any playing note
	m_pSequenceMgr->StopNote();

	KillTimer();

	if( m_fZoomInPressed || m_fZoomOutPressed || m_fNewBandPressed || m_fInstrumentPressed )
	{
		m_fZoomInPressed = FALSE;
		m_fZoomOutPressed = FALSE;
		m_fNewBandPressed = FALSE;
		m_fInstrumentPressed = FALSE;

		// Redraw the fuction bar
		InvalidateFunctionBar();
		return S_OK;
	}

	if( m_fInsertingNoteFromMouseClick )
	{
		// Let the object know about the changes
		m_nLastEdit = IDS_UNDO_INSERT;
		m_pSequenceMgr->OnDataChanged();
	}

	VARIANT var;
	if( m_fCtrlKeyDown == 1)
	{
		// Unselect
		if( m_pActiveNote )
		{
			m_pActiveNote->m_fSelected = FALSE;
			InvalidateStrip();
		}

		UpdateSelectionState();
		m_pSequenceMgr->RefreshPropertyPage();
	}
	m_fCtrlKeyDown = 0;

	switch(m_MouseMode)
	{
	case SEQ_MM_NORMAL:
		break;
	case SEQ_MM_MOVE:
	case SEQ_MM_RESIZE_START:
	case SEQ_MM_RESIZE_END:
	case SEQ_MM_RESIZE_VELOCITY:
		break;

	case SEQ_MM_ACTIVERESIZE_VELOCITY:
	case SEQ_MM_ACTIVERESIZE_END:
	case SEQ_MM_ACTIVERESIZE_START:
	case SEQ_MM_ACTIVEMOVE:
		// Release the mouse capture
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

		// Reset m_MouseMode
		switch( m_MouseMode )
		{
		case SEQ_MM_ACTIVERESIZE_VELOCITY:
			m_MouseMode = SEQ_MM_RESIZE_VELOCITY;
			break;
		case SEQ_MM_ACTIVERESIZE_END:
			m_MouseMode = SEQ_MM_RESIZE_END;
			break;
		case SEQ_MM_ACTIVERESIZE_START:
			m_MouseMode = SEQ_MM_RESIZE_START;
			break;
		case SEQ_MM_ACTIVEMOVE:
			m_MouseMode = SEQ_MM_MOVE;
			break;
		}

		if( !m_fInsertingNoteFromMouseClick && m_pActiveNote && !m_OriginalSeqItem.IsEqual( m_pActiveNote ) )
		{
			// Let the object know about the changes
			m_nLastEdit = IDS_UNDO_CHANGE;
			m_pSequenceMgr->OnDataChanged();
		}
		break;
	default:
		break;
	}

	// Reset the insertion flag
	m_fInsertingNoteFromMouseClick = FALSE;

	// We don't need to refresh the property page here because it should already
	// be up-to-date from the last mouse message (LBUTTON_DOWN or MOUSE_MOVE).
	SetMouseMode( lXPos, lYPos );

	// Unselect all items in other strips
	UnselectGutterRange();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnMouseMove

HRESULT CSequenceStrip::OnMouseMove( long lXPos, long lYPos)
{
	if (m_svView == SV_MINIMIZED)
	{
		return S_OK;
	}

	long xDelta, yDelta;

	switch(m_MouseMode)
	{
	case SEQ_MM_ACTIVERESIZE_VELOCITY:
		// compute the new velocity
		yDelta = m_pointClicked.y - lYPos;
		yDelta = long((double)yDelta/m_dblVerticalZoom);
		if ( (m_SelectionMode == SEQ_SINGLE_SELECT) && m_pActiveNote )
		{
			long lVel;
			lVel = m_OriginalSeqItem.m_bByte2 + yDelta / 2;
			if ( lVel > 127 )
				lVel = 127;
			if ( lVel < 1 )
				lVel = 1;

			if( lVel != m_pActiveNote->m_bByte2 )
			{
				m_pActiveNote->m_bByte2 = (BYTE)lVel;
				InvalidateStrip();
				m_pSequenceMgr->RefreshPropertyPage();

				m_pSequenceMgr->PlayNote( m_pActiveNote );
			}
		}
		else if( (m_SelectionMode == SEQ_MULTIPLE_SELECT) && ((yDelta / 2) - m_lLastDeltaVel) )
		{
			if( m_pSequenceMgr->OffsetSelectedNoteDurationAndVelocity( 0, (yDelta / 2) - m_lLastDeltaVel, FALSE ) )
			{
				m_lLastDeltaVel = yDelta / 2;
			}
		}
		break;

	case SEQ_MM_ACTIVERESIZE_END:
		if ( (m_SelectionMode == SEQ_MULTIPLE_SELECT) ||
			 (m_SelectionMode == SEQ_SINGLE_SELECT) && m_pActiveNote)
		{
			// compute the original and new end points
			long lTemp1, lTemp2;
			m_pSequenceMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pSequenceMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);

			// compute the change
			xDelta = lTemp2 - lTemp1;

			// scroll if necessary
			AdjustScroll(lXPos, lYPos);

			if( m_SelectionMode == SEQ_SINGLE_SELECT )
			{
				// Resize in clocks
				long lDur;
				lDur = m_OriginalSeqItem.m_mtDuration + xDelta;
				if ( lDur < 1 )
				{
					lDur = 1;
				}

				if( lDur != m_pActiveNote->m_mtDuration )
				{
					m_pActiveNote->m_mtDuration = lDur;
					InvalidateStrip();
					m_pSequenceMgr->RefreshPropertyPage();
				}
			}
			else if( xDelta - m_lLastDeltaDur )
			{
				if( m_pSequenceMgr->OffsetSelectedNoteDurationAndVelocity( xDelta - m_lLastDeltaDur, 0, FALSE ) )
				{
					m_lLastDeltaDur = xDelta;
				}
			}
		}
		break;

	case SEQ_MM_ACTIVERESIZE_START:
		if ( (m_SelectionMode == SEQ_SINGLE_SELECT) || (m_SelectionMode == SEQ_MULTIPLE_SELECT) )
		{
			// compute the original and new start points
			long lTemp1, lTemp2;
			m_pSequenceMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pSequenceMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);
			
			// Get the snap amound
			long lSnapAmount = GetSnapAmount( lTemp1 + m_lLastDeltaStart );

			// compute the change
			long lDeltaStart;
			if( lSnapAmount > 1 )
			{
				lDeltaStart = lSnapAmount * ((abs(lTemp2 - lTemp1) + (lSnapAmount / 2)) / lSnapAmount);
				lDeltaStart *= lTemp2 > lTemp1 ? 1 : -1;
				lDeltaStart -= m_lLastDeltaStart;
			}
			else
			{
				lDeltaStart = lTemp2 - lTemp1 - m_lLastDeltaStart;
			}

			if ( lDeltaStart != 0 )
			{
				// Compute the length of the pickup bar
				long lEarlyMeasureClocks = 0;
				DMUS_TIMESIGNATURE ts;
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, 0, NULL, &ts ) ) )
				{
					lEarlyMeasureClocks = -ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
				}

				// Compute the length of the extension bar
				long lLateMeasureClocks = 0;
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength - 1, NULL, &ts ) ) )
				{
					lLateMeasureClocks = ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
				}

				POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
				while( pos )
				{
					CSequenceItem *pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
					if ( pSeqItem->m_fSelected )
					{
						// Make sure the start time doesn't go out of bounds
						/*
						if( (pSeqItem->MusicTime() >= 0)
						&&	(pSeqItem->MusicTime() + lDeltaStart < 0) )
						*/
						if ( pSeqItem->MusicTime() + lDeltaStart < 0 )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * (-pSeqItem->MusicTime() / lSnapAmount);
							}
							else
							{
								lDeltaStart = -pSeqItem->MusicTime();
							}
						}
						else if ( pSeqItem->MusicTime() + lDeltaStart >= m_mtLength )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((m_mtLength - pSeqItem->MusicTime() - 1) / lSnapAmount);
							}
							else
							{
								lDeltaStart = m_mtLength - pSeqItem->MusicTime() - 1;
							}
						}

						// Now ensure lDeltaStart does not put the event more than one measure ahead of the sequence
						if( lDeltaStart + pSeqItem->AbsTime() < lEarlyMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((lEarlyMeasureClocks - pSeqItem->AbsTime()) / lSnapAmount);
							}
							else
							{
								lDeltaStart = lEarlyMeasureClocks - pSeqItem->AbsTime();
							}
						}

						// Now ensure lStartTime does not put the event more than one measure after the segment
						if( lDeltaStart + pSeqItem->AbsTime() > m_mtLength - 1 + lLateMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((m_mtLength - 1 + lLateMeasureClocks - pSeqItem->AbsTime()) / lSnapAmount);
							}
							else
							{
								lDeltaStart = m_mtLength - 1 + lLateMeasureClocks - pSeqItem->AbsTime();
							}
						}

						// Make sure the duration stays in bounds
						if (  pSeqItem->m_mtDuration - lDeltaStart - 1 < 0 )
						{
							lDeltaStart = pSeqItem->m_mtDuration - 1;
						}
					}
				}

				// scroll if necessary
				AdjustScroll(lXPos, lYPos);

				if( (m_SelectionMode == SEQ_SINGLE_SELECT) && m_pActiveNote &&
					(m_pActiveNote->m_mtDuration - lDeltaStart <= 0) )
				{
					lDeltaStart = m_pActiveNote->m_mtDuration - 1;
				}

				if( lDeltaStart &&
					(((m_SelectionMode == SEQ_SINGLE_SELECT) &&
					  m_pActiveNote &&
					  (m_pActiveNote->m_mtDuration - lDeltaStart > 0)) ||
					 (m_SelectionMode == SEQ_MULTIPLE_SELECT)) )
				{
					if( m_pSequenceMgr->OffsetSelectedNoteValuePositionAndStart( 0, 0, lDeltaStart, FALSE ) )
					{
						m_lLastDeltaStart = m_lLastDeltaStart + lDeltaStart;
					}
				}
			}

		}
		break;

	case SEQ_MM_ACTIVEMOVE:
		if ( (m_SelectionMode == SEQ_SINGLE_SELECT) || (m_SelectionMode == SEQ_MULTIPLE_SELECT) )
		{
			// compute the original and new start points
			long lTemp1, lTemp2;
			m_pSequenceMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pSequenceMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);
			
			// Get the snap amound
			long lSnapAmount = GetSnapAmount( lTemp1 + m_lLastDeltaMove );

			// compute the change
			long lDeltaStart;
			if( lSnapAmount > 1 )
			{
				lDeltaStart = lSnapAmount * ((abs(lTemp2 - lTemp1) + (lSnapAmount / 2)) / lSnapAmount);
				lDeltaStart *= lTemp2 > lTemp1 ? 1 : -1;
				lDeltaStart -= m_lLastDeltaMove;
			}
			else
			{
				lDeltaStart = lTemp2 - lTemp1 - m_lLastDeltaMove;
			}

			// Compute the length of the pickup bar
			long lEarlyMeasureClocks = 0;
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, 0, NULL, &ts ) ) )
			{
				lEarlyMeasureClocks = -ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
			}

			// Compute the length of the extension bar
			long lLateMeasureClocks = 0;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength - 1, NULL, &ts ) ) )
			{
				lLateMeasureClocks = ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
			}

			// Compute the new note value
			yDelta = m_pointClicked.y - lYPos;
			long lDeltaValue;
			if( m_fHybridNotation )
			{
				// These divide m_lMaxNoteHeight by 8 instead of 4 because we want to decrease the
				// 'null' zone around the original note
				if ( yDelta < 0 )
				{
					// Convert from ypos to a scale position (with B#10 as 0)
					lDeltaValue = (4 * (-yDelta + m_lMaxNoteHeight/8)) / m_lMaxNoteHeight;

					// Convert from a scale position to a MIDI value
					lDeltaValue = (lDeltaValue / 14) * 12 + aDoubleScaleToChrom[lDeltaValue % 14];

					lDeltaValue = -lDeltaValue - m_cLastDeltaValue;
				}
				else
				{
					// Convert from ypos to a scale position (with B#10 as 0)
					lDeltaValue = (4 * (yDelta + m_lMaxNoteHeight/8)) / m_lMaxNoteHeight;

					// Convert from a scale position to a MIDI value
					lDeltaValue = (lDeltaValue / 14) * 12 + aDoubleScaleToChrom[lDeltaValue % 14];

					lDeltaValue = lDeltaValue - m_cLastDeltaValue;
				}
			}
			else
			{
				if ( yDelta < 0 )
				{
					lDeltaValue = ((yDelta - m_lMaxNoteHeight/2) / m_lMaxNoteHeight) - m_cLastDeltaValue;
				}
				else
				{
					lDeltaValue = ((yDelta + m_lMaxNoteHeight/2) / m_lMaxNoteHeight) - m_cLastDeltaValue;
				}
			}

			if ( lDeltaStart != 0 || lDeltaValue != 0 )
			{
				CSequenceItem *pSeqItem;
				POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();

				while( pos )
				{
					pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
					if ( pSeqItem->m_fSelected )
					{
						// Make sure the start time doesn't go out of bounds
						if ( pSeqItem->MusicTime() + lDeltaStart < 0 )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * (-pSeqItem->MusicTime() / lSnapAmount);
							}
							else
							{
								lDeltaStart = -pSeqItem->MusicTime();
							}
						}
						else if ( pSeqItem->MusicTime() + lDeltaStart >= m_mtLength )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((m_mtLength - pSeqItem->MusicTime() - 1) / lSnapAmount);
							}
							else
							{
								lDeltaStart = m_mtLength - pSeqItem->MusicTime() - 1;
							}
						}

						// Now ensure lDeltaStart does not put the event more than one measure ahead of the sequence
						if( lDeltaStart + pSeqItem->AbsTime() < lEarlyMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((lEarlyMeasureClocks - pSeqItem->AbsTime()) / lSnapAmount);
							}
							else
							{
								lDeltaStart = lEarlyMeasureClocks - pSeqItem->AbsTime();
							}
						}

						// Now ensure lStartTime does not put the event more than one measure after the segment
						if( lDeltaStart + pSeqItem->AbsTime() > m_mtLength - 1 + lLateMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((m_mtLength - 1 + lLateMeasureClocks - pSeqItem->AbsTime()) / lSnapAmount);
							}
							else
							{
								lDeltaStart = m_mtLength - 1 + lLateMeasureClocks - pSeqItem->AbsTime();
							}
						}

						// Make sure the value doesn't go out of bounds
						long lValue = pSeqItem->m_bByte1 + lDeltaValue;

						if( lValue < 0 )
						{
							// Ensure bottom does not go below 0
							lDeltaValue += -lValue;
							// TODO: Probably not necessary:
							lValue = pSeqItem->m_bByte1 + lDeltaValue;
						}

						if ( lValue > 127 )
						{
							// Ensure top does not go above 127
							lDeltaValue += 127 - lValue;
						}
					}
				}

				// scroll if necessary
				AdjustScroll(lXPos, lYPos);

				if( lDeltaStart || lDeltaValue )
				{
					if( m_pSequenceMgr->OffsetSelectedNoteValuePositionAndStart( lDeltaValue, lDeltaStart, 0, m_fCtrlKeyDown ) )
					{
						m_lLastDeltaMove = m_lLastDeltaMove + lDeltaStart;
						m_cLastDeltaValue = char (m_cLastDeltaValue + char (lDeltaValue));

						if( m_fCtrlKeyDown )
						{
							m_fCtrlKeyDown = 0;
						}
					}
				}
			}

		}
		break;
	case SEQ_MM_NORMAL:
	case SEQ_MM_MOVE:
	case SEQ_MM_RESIZE_START:
	case SEQ_MM_RESIZE_END:
	case SEQ_MM_RESIZE_VELOCITY:
		break;
	default:
		break;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnKeyDown

HRESULT CSequenceStrip::OnKeyDown( WPARAM wParam )
{
	if( (wParam == VK_INSERT)
	||	(wParam == VK_DELETE) )
	{
		if( (0x8000 & GetKeyState( VK_CONTROL ))
		&&	(0x8000 & GetKeyState( VK_SHIFT )) )
		{
			if( wParam == VK_INSERT )
			{
				return m_pSequenceMgr->AddNewPart();
			}
			else
			{
				HWND hwnd = GetTimelineHWnd();
				if( hwnd )
				{
					hwnd = ::GetParent( hwnd );
					if( hwnd )
					{
						::SendMessage( hwnd, WM_COMMAND, MAKEWPARAM( ID_EDIT_DELETE_TRACK, 0 ), 0 );
					}
				}
			}
		}
	}

	if (m_svView == SV_MINIMIZED)
	{
		return S_OK;
	}

	switch(m_MouseMode)
	{
	case SEQ_MM_NORMAL:
	case SEQ_MM_MOVE:
	case SEQ_MM_RESIZE_START:
	case SEQ_MM_RESIZE_END:
	case SEQ_MM_RESIZE_VELOCITY:
		// Up arrow / Down arrow / Page Up / Page Down
		if ( (wParam == 38) || (wParam == 40 ) || (wParam == 33) || (wParam == 34 ) )
		{
			long lNewVertScroll = m_lVScroll;
			long lNewInsertVal = m_lInsertVal;

			// scroll the piano roll up and down and move the note cursor
			// DOWN arrow
			if (wParam == 40)
			{
				lNewInsertVal--;
				if( lNewInsertVal < 0 )
				{
					lNewInsertVal = 0;
				}
				else
				{
					if( m_fHybridNotation )
					{
						int iNewHybridPos, iOldHybridPos;
						// FLATS
						if( m_pSequenceMgr->m_fDisplayingFlats )
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleFlats);
							iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleFlats);
						}
						// SHARPS
						else
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleSharps);
							iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleSharps);
						}

						if( iNewHybridPos / 2 != iOldHybridPos / 2 )
						{
							lNewVertScroll += m_lMaxNoteHeight;
						}

						if ( lNewVertScroll / m_lMaxNoteHeight > 37)
						{
							lNewVertScroll = m_lMaxNoteHeight * 37;
						}
					}
					else
					{
						lNewVertScroll += m_lMaxNoteHeight;
						if ( lNewVertScroll / m_lMaxNoteHeight > 127)
						{
							lNewVertScroll = m_lMaxNoteHeight * 127;
						}
					}
				}
			}

			// UP arrow
			else if (wParam == 38)
			{
				lNewInsertVal++;
				if( lNewInsertVal > 127 )
				{
					lNewInsertVal = 127;
				}
				else
				{
					if( m_fHybridNotation )
					{
						int iNewHybridPos, iOldHybridPos;
						// FLATS
						if( m_pSequenceMgr->m_fDisplayingFlats )
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleFlats);
							iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleFlats);
						}
						// SHARPS
						else
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleSharps);
							iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleSharps);
						}

						if( iNewHybridPos / 2 != iOldHybridPos / 2 )
						{
							lNewVertScroll -= m_lMaxNoteHeight;
						}
					}
					else
					{
						lNewVertScroll -= m_lMaxNoteHeight;
					}

					if ( lNewVertScroll < 0)
					{
						lNewVertScroll = 0;
					}
				}
			}

			// Page Down
			else if (wParam == 34)
			{
				lNewInsertVal -= 12;
				if( lNewInsertVal < 0 )
				{
					lNewInsertVal = 0;
				}

				if( m_fHybridNotation )
				{
					int iNewHybridPos, iOldHybridPos;
					// FLATS
					if( m_pSequenceMgr->m_fDisplayingFlats )
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleFlats);
						iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleFlats);
					}
					// SHARPS
					else
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleSharps);
						iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleSharps);
					}

					lNewVertScroll -= ((iNewHybridPos / 2) - (iOldHybridPos / 2)) * m_lMaxNoteHeight;

					if ( lNewVertScroll / m_lMaxNoteHeight > 37)
					{
						lNewVertScroll = m_lMaxNoteHeight * 37;
					}
				}
				else
				{
					lNewVertScroll += 12 * m_lMaxNoteHeight;
					if ( lNewVertScroll / m_lMaxNoteHeight > 127)
					{
						lNewVertScroll = m_lMaxNoteHeight * 127;
					}
				}
			}

			// Page Up
			else if (wParam == 33)
			{
				lNewInsertVal += 12;
				if( lNewInsertVal > 127 )
				{
					lNewInsertVal = 127;
				}

				if( m_fHybridNotation )
				{
					int iNewHybridPos, iOldHybridPos;
					// FLATS
					if( m_pSequenceMgr->m_fDisplayingFlats )
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleFlats);
						iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleFlats);
					}
					// SHARPS
					else
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE) lNewInsertVal, aChromToScaleSharps);
						iOldHybridPos = MIDIToHybridPos( (BYTE) m_lInsertVal, aChromToScaleSharps);
					}

					lNewVertScroll += ((iOldHybridPos / 2) - (iNewHybridPos / 2)) * m_lMaxNoteHeight;
				}
				else
				{
					lNewVertScroll -= 12 * m_lMaxNoteHeight;
				}

				if ( lNewVertScroll < 0)
				{
					lNewVertScroll = 0;
				}
			}

			if (lNewVertScroll != m_lVScroll)
			{
				SetVScroll( lNewVertScroll );
			}

			// Redraw the strip
			if( lNewInsertVal != m_lInsertVal )
			{
				m_lInsertVal = lNewInsertVal;
				InvalidatePianoRoll();
			}
		}
		break;
	case SEQ_MM_ACTIVERESIZE_VELOCITY:
		if( (wParam == 40) || (wParam == 38) )
		{
			long lDelta = 0;
			// UP
			if (wParam == 38) lDelta = 1;
			// DOWN
			if (wParam == 40) lDelta = -1;
			if ( (m_SelectionMode == SEQ_SINGLE_SELECT) && m_pActiveNote )
			{
				long lVel;
				lVel = m_OriginalSeqItem.m_bByte2 + lDelta;
				if ( lVel > 127 )
					lVel = 127;
				if ( lVel < 1 )
					lVel = 1;

				if( lVel != m_pActiveNote->m_bByte2 )
				{
					m_pActiveNote->m_bByte2 = (BYTE)lVel;
					InvalidateStrip();
					m_pSequenceMgr->RefreshPropertyPage();
				}
			}
			else if(m_SelectionMode == SEQ_MULTIPLE_SELECT)
			{
				m_pSequenceMgr->OffsetSelectedNoteDurationAndVelocity( 0, lDelta, FALSE ); 
			}
		}
		break;
	case SEQ_MM_ACTIVERESIZE_END:
		if ((wParam == 39) || (wParam == 37))
		{
			long lDelta = 0;
			// LEFT
			if (wParam == 37) lDelta = -1;
			// RIGHT
			if (wParam == 39) lDelta = 1;

			if( (m_SelectionMode == SEQ_SINGLE_SELECT) && m_pActiveNote )
			{
				// Resize in clocks
				long lDur;
				lDur = m_OriginalSeqItem.m_mtDuration + lDelta;
				if ( lDur < 1 )
				{
					lDur = 1;
				}

				if( lDur != m_pActiveNote->m_mtDuration )
				{
					m_pActiveNote->m_mtDuration = lDur;
					InvalidateStrip();
					m_pSequenceMgr->RefreshPropertyPage();
				}
			}
			else if(m_SelectionMode == SEQ_MULTIPLE_SELECT)
			{
				m_pSequenceMgr->OffsetSelectedNoteDurationAndVelocity( lDelta, 0, FALSE );
			}
		}
		break;

	case SEQ_MM_ACTIVERESIZE_START:
		if ( (m_SelectionMode == SEQ_SINGLE_SELECT) || (m_SelectionMode == SEQ_MULTIPLE_SELECT) )
		if ( ((m_SelectionMode == SEQ_SINGLE_SELECT) || (m_SelectionMode == SEQ_MULTIPLE_SELECT))
		  && ((wParam == 39) || (wParam == 37 )))
		{
			// compute the original and new start points
			long lTemp1;
			m_pSequenceMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			
			// Get the snap amound
			long lSnapAmount = GetSnapAmount( lTemp1 + m_lLastDeltaStart );

			// compute the change
			long lDeltaStart = lSnapAmount * ((wParam == 39) ? 1 : -1);

			if ( lDeltaStart != 0 )
			{
				CSequenceItem *pSeqItem;
				POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();

				while( pos )
				{
					pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
					if ( pSeqItem->m_fSelected )
					{
						// Make sure the start time doesn't go out of bounds
						if ( pSeqItem->MusicTime() + lDeltaStart < 0 )
						{
							lDeltaStart = 0;
						}
						else if ( pSeqItem->MusicTime() + lDeltaStart >= m_mtLength )
						{
							lDeltaStart = 0;
						}

						// Now ensure lDeltaStart does not put the event more than one measure ahead of the sequence
						DMUS_TIMESIGNATURE ts;
						if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, 0, NULL, &ts ) ) )
						{
							if( lDeltaStart + pSeqItem->AbsTime() < -(ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
							{
								lDeltaStart = 0;
							}
						}

						// Now ensure lStartTime does not put the event more than one measure after the segment
						if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength - 1, NULL, &ts ) ) )
						{
							if( lDeltaStart + pSeqItem->AbsTime() > m_mtLength - 1 + (ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
							{
								lDeltaStart = 0;
							}
						}
					}
				}

				// scroll if necessary
				//TODO: Implement
				//AdjustScroll(lXPos, lYPos);

				if( lDeltaStart )
				{
					if( m_pSequenceMgr->OffsetSelectedNoteValuePositionAndStart( 0, 0, lDeltaStart, FALSE ) )
					{
						m_lLastDeltaStart = m_lLastDeltaStart + lDeltaStart;
					}
				}
			}

		}
		break;

	case SEQ_MM_ACTIVEMOVE:
		if ( (m_SelectionMode == SEQ_SINGLE_SELECT) || (m_SelectionMode == SEQ_MULTIPLE_SELECT) )
		{
			long lDeltaValue = 0;
			long lDeltaStart = 0;

			// UP
			if (wParam == 38)
			{
				lDeltaValue = +1;
			}
			// DOWN
			if (wParam == 40)
			{
				lDeltaValue = -1;
			}

			// LEFT
			// RIGHT
			if ( (wParam == 37) || (wParam == 39) )
			{
				if( m_pActiveNote )
				{
					lDeltaStart = m_pActiveNote->AbsTime();
				}
				else
				{
					CSequenceItem *pItem = m_pSequenceMgr->FirstSelectedSequence();
					if( pItem )
					{
						lDeltaStart = pItem->AbsTime();
					}
					else
					{
						ASSERT(FALSE);
						lDeltaStart = 0;
					}
				}
				lDeltaStart = GetSnapAmount( lDeltaStart );
				lDeltaStart *= (wParam == 39) ? 1 : -1;
			}
			
			if ( lDeltaStart != 0 || lDeltaValue != 0 )
			{
				CSequenceItem *pSeqItem;
				POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
				while( pos )
				{
					pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
					if ( pSeqItem->m_fSelected )
					{
						// Make sure the start time doesn't go out of bounds
						if ( pSeqItem->MusicTime() + lDeltaStart < 0 )
						{
							lDeltaStart = 0;
						}
						else if ( pSeqItem->MusicTime() + lDeltaStart >= m_mtLength )
						{
							lDeltaStart = 0;
						}

						// Now ensure lDeltaStart does not put the event more than one measure ahead of the sequence
						DMUS_TIMESIGNATURE ts;
						if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, 0, NULL, &ts ) ) )
						{
							if( lDeltaStart + pSeqItem->AbsTime() < -(ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
							{
								lDeltaStart = 0;
							}
						}

						// Now ensure lStartTime does not put the event more than one measure after the segment
						if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength - 1, NULL, &ts ) ) )
						{
							if( lDeltaStart + pSeqItem->AbsTime() > m_mtLength - 1 + (ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
							{
								lDeltaStart = 0;
							}
						}

						// Make sure the value doesn't go out of bounds
						long lValue = pSeqItem->m_bByte1 + lDeltaValue;

						if( lValue < 0 )
						{
							// Ensure bottom does not go below 0
							lDeltaValue += -lValue;
							// Probably not necessary:
							lValue = pSeqItem->m_bByte1 + lDeltaValue;
						}

						if ( lValue > 127 )
						{
							// Ensure top does not go above 127
							lDeltaValue += 127 - lValue;
						}
					}
				}

				if( m_pSequenceMgr->OffsetSelectedNoteValuePositionAndStart( lDeltaValue, lDeltaStart, 0, m_fCtrlKeyDown ) )
				{
					m_lLastDeltaMove = m_lLastDeltaMove + lDeltaStart;
					m_cLastDeltaValue = char (m_cLastDeltaValue + char (lDeltaValue));

					if( m_fCtrlKeyDown )
					{
						m_fCtrlKeyDown = 0;
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnChangeStripView

void CSequenceStrip::OnChangeStripView( STRIPVIEW svNewStripView )
{
	if( m_svView != svNewStripView )
	{
		// Change StripView field
		m_svView = svNewStripView;

		// Remove all existing Curve Strips
		CCurveStrip *pCurveStrip;
		while( !m_lstCurveStrips.IsEmpty() )
		{
			pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.GetHead() );
			RemoveCurveStrip( pCurveStrip );
		}

		// If minimizing and gutter is not selected, unselect all events in our variations
		if( (m_svView == SV_MINIMIZED) && m_bGutterSelected &&
			(m_lBeginSelect >= 0) && (m_lEndSelect > 0) )
		{
			if ( m_SelectionMode != SEQ_NO_SELECT )
			{
				//BUGBUG: Should we unselect curves here as well?  The PianoRollStrip does...
				m_pSequenceMgr->UnselectAllNotes();
				m_SelectionMode = SEQ_NO_SELECT;
			}
		}

		AddCurveStrips();

		// Let the object know about the changes
		if( m_svView == SV_NORMAL )
		{
			m_nLastEdit = IDS_UNDO_STRIP_MAXIMIZED;
			m_VScrollBar.ShowWindow(TRUE);
		}
		else // m_svView == SV_MINIMIZED
		{
			m_nLastEdit = IDS_UNDO_STRIP_MINIMIZED;
			m_VScrollBar.ShowWindow(FALSE);
		}

		// Fix 27283: Don't store Undo states for Minimize/Maximize events

		// Check if TP_FREEZE_UNDO is NOT set.
		BOOL fFreezeUndo = FALSE;
		VARIANT var;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
		{
			fFreezeUndo = V_BOOL(&var);
		}

		if( !fFreezeUndo )
		{
			// Need to set TP_FREEZE_UNDO or the segment will add an undo state for us
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		// No need to update performance engine
		m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
		m_pSequenceMgr->OnDataChanged( );

		if( !fFreezeUndo )
		{
			// Need to reset TP_FREEZE_UNDO or the segment will add an undo state for us
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnDestroy

HRESULT CSequenceStrip::OnDestroy( void )
{
	ASSERT( m_pStripMgr != NULL );

	// Update the UI chunk
	UpdateSequenceUIChunk();

	// Don't delete curve strip states - delete them in the destructor

	CCurveStrip *pCurveStrip;
	while( !m_lstCurveStrips.IsEmpty() )
	{
		pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.RemoveHead() );
		if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
		{
			m_pSequenceMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );
		}
		pCurveStrip->Release();
	}

	// Delete the Bitmap buttons
	if( InterlockedDecrement( &m_lBitmapRefCount ) == 0 )
	{
		if( m_BitmapZoomInUp.GetSafeHandle() != NULL )
		{
			m_BitmapZoomInUp.DeleteObject();
		}
		if( m_BitmapZoomInDown.GetSafeHandle() != NULL )
		{
			m_BitmapZoomInDown.DeleteObject();
		}
		if( m_BitmapZoomOutUp.GetSafeHandle() != NULL )
		{
			m_BitmapZoomOutUp.DeleteObject();
		}
		if( m_BitmapZoomOutDown.GetSafeHandle() != NULL )
		{
			m_BitmapZoomOutDown.DeleteObject();
		}
		if( m_BitmapNewBand.GetSafeHandle() != NULL )
		{
			m_BitmapNewBand.DeleteObject();
		}
	}

	if (m_VScrollBar.GetSafeHwnd())
	{
		m_VScrollBar.ShowWindow( FALSE );
		//m_VScrollBar.Detach();
		m_VScrollBar.DestroyWindow();
	}

	// Kill timer, if active
	KillTimer();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SetFocus

HRESULT CSequenceStrip::SetFocus( int iFocus )
{
	// When changing strip focus, KillFocus is always sent to the original strip before
	// SetFocus is sent to the strip that gains focus.  As a result, we will only have
	// the transitions 0->1, 1->0, 0->2, and 2->0 in this function.
	if( m_iHaveFocus != iFocus )
	{
		ASSERT( ((m_iHaveFocus == 0) && ( (iFocus == 1) || (iFocus == 2) ))
			 || ((iFocus == 0) && ( (m_iHaveFocus == 1) || (m_iHaveFocus == 2) )) );

		switch( iFocus )
		{
		case 0:
			// Save the current state
			iFocus = m_iHaveFocus;

			// Update m_iHaveFocus
			m_iHaveFocus = 0;

			// If the sequence strip had focus, redraw
			if( iFocus == 1 )
			{
				// Need to erase note cursor
				InvalidatePianoRoll();
			}
			
			// Always unregister MIDI
			UnRegisterMidi();

			// Stop any playing note
			m_pSequenceMgr->StopNote();

			// Fake a mouse-up
			OnLButtonUp( 0, 0 );
			break;

		case 1:
			// If there currently is no primary segment playing, try and play the patch for this PChannel
			{
				REFERENCE_TIME rtLatency;
				m_pSequenceMgr->m_pIDMPerformance->GetLatencyTime( &rtLatency );
				MUSIC_TIME mtLatency;
				m_pSequenceMgr->m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

				IDirectMusicSegmentState *pSegState;
				if( FAILED( m_pSequenceMgr->m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
				{
					long lTime;
					if (SUCCEEDED(m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
					{
						m_pSequenceMgr->PlayPatch( lTime );
					}
				}
				else
				{
					pSegState->Release();
				}
			}

			// Always fall through to the case below
		case 2:
			// Register MIDI if both strips previously didn't have focus
			ASSERT( m_iHaveFocus == 0 );
			if( (m_iHaveFocus == 0) && m_pSequenceMgr->m_fWindowActive )
			{
				RegisterMidi();
			}

			// Update m_iHaveFocus
			m_iHaveFocus = iFocus;
			if( m_iHaveFocus == 1 )
			{
				UpdateNoteCursorTime();
				// Need to draw the note cursor
				InvalidatePianoRoll();
			}

			if( (m_pSequenceMgr->m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)
			&&	m_pSequenceMgr->m_pSegmentState && m_pSequenceMgr->m_fRecordEnabled
			&&	m_pSequenceMgr->m_fWindowActive )
			{
				// Try and set up MIDI thruing
				SetPChannelThru();
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	m_pSequenceMgr->UpdateStatusBarDisplay();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateName

void CSequenceStrip::UpdateName()
{
	CString strText, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_pSequenceMgr->m_dwGroupBits & (1 << i) )
		{
			if( !fLastSet )
			{
				fLastSet = TRUE;
				nStartGroup = i;
			}
		}
		else
		{
			if( fLastSet )
			{
				fLastSet = FALSE;
				if( nStartGroup == i - 1 )
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d", i);
					}
					else
					{
						strTmp.Format("%d", i);
						fFoundGroup = TRUE;
					}
				}
				else
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d-%d", nStartGroup + 1, i);
					}
					else
					{
						strTmp.Format("%d-%d", nStartGroup + 1, i);
						fFoundGroup = TRUE;
					}
				}
				strText += strTmp;
			}
		}
	}

	if( fLastSet )
	{
		fLastSet = FALSE;
		if( nStartGroup == i - 1 )
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d", i);
			}
			else
			{
				strTmp.Format("%d", i);
				fFoundGroup = TRUE;
			}
		}
		else
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d-%d", nStartGroup + 1, i);
			}
			else
			{
				strTmp.Format("%d-%d", nStartGroup + 1, i);
				fFoundGroup = TRUE;
			}
		}
		strText += strTmp;
	}

	// If available, query the IDMUSProdPChannelName interface for the PChannel name
	WCHAR wszName[MAX_PATH];
	if( m_pSequenceMgr->m_pIPChannelName &&
		SUCCEEDED( m_pSequenceMgr->m_pIPChannelName->GetPChannelName( m_pSequenceMgr->m_dwPChannel, wszName ) ) )
	{
		if( m_pSequenceMgr->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_strName.Format( "%s: %d (%S): %s", strText, m_pSequenceMgr->m_dwPChannel + 1, wszName, m_pSequenceMgr->m_strName );
		}
		else
		{
			CString strNewName;
			if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				strNewName.LoadString( IDS_BROADCAST_SEG );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				strNewName.LoadString( IDS_BROADCAST_PERF );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strNewName.LoadString( IDS_BROADCAST_GRP );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				strNewName.LoadString( IDS_BROADCAST_APATH );
			}
			m_strName.Format( "%s: %s (%S): %s", strText, strNewName, wszName, m_pSequenceMgr->m_strName );
		}
	}
	// If unable to get name from IPChannelName interface, default to just the #
	else
	{
		if( m_pSequenceMgr->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_strName.Format("%s: %d: %s", strText, m_pSequenceMgr->m_dwPChannel + 1, m_pSequenceMgr->m_strName );
		}
		else
		{
			CString strNewName;
			if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				strNewName.LoadString( IDS_BROADCAST_SEG );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				strNewName.LoadString( IDS_BROADCAST_PERF );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strNewName.LoadString( IDS_BROADCAST_GRP );
			}
			else if( m_pSequenceMgr->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				strNewName.LoadString( IDS_BROADCAST_APATH );
			}
			m_strName.Format( "%s: %s: %s", strText, strNewName, m_pSequenceMgr->m_strName );
		}
	}
}

#ifdef _DEBUG
/*
void CSequenceStrip::TraceFormatsInClipboard()
{
	char buf[MAX_PATH];
	IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
	m_pSequenceMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
	if (pIOleInPlaceObjectWindowless)
	{
		HWND hwnd;
		if (pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK)
		{
			if(OpenClipboard(hwnd))
			{
				UINT format = 0;
				while(format = EnumClipboardFormats(format))
				{
					GetClipboardFormatName(format, buf, MAX_PATH);
					TRACE("Format = %s\n", buf);
				}
				CloseClipboard();
			}
		}
		pIOleInPlaceObjectWindowless->Release();
	}
}
*/
#endif

HRESULT CSequenceStrip::PasteMidiFile(IDMUSProdTimelineDataObject * pITimelineDataObject, long lClocks, BOOL fPasteToMultipleStrips )
{
	IStream* pIStream = NULL;
	HRESULT hr = pITimelineDataObject->AttemptRead(m_cfMidiFile, &pIStream);
	if(SUCCEEDED(hr))
	{
		if( fPasteToMultipleStrips )
		{
			if( !m_pSequenceMgr->FirstGutterSelectedSequenceMgr() )
			{
				// Already handled by the first sequence strip mgr.
				hr = S_FALSE;
			}
			else
			{
				// Split the MIDI file into multiple tracks and paste them in the correct spots
				hr = m_pSequenceMgr->ImportMIDIFileToMultipleTracks( pIStream );
			}
			pIStream->Release();
			return hr;
		}

		// Paste the entire MIDI file into this track.

		// get offset of first sequence's drag position to normalize all sequences to offset 0
		long lTimeOffset;
		if(m_pSequenceMgr->m_pTimeline)
		{
			CTypedPtrList<CPtrList, CSequenceItem*> lstSequences;
			CTypedPtrList<CPtrList, CCurveItem*> lstCurves;
			lTimeOffset = lClocks;
			if(SUCCEEDED(hr))
			{
				DWORD dwLength;
				hr = ReadMidiSequenceFromStream(pIStream, lstSequences, lstCurves, lTimeOffset, dwLength);
				// BUGBUG: no way to propagate dwLength to segment, user will have to set
				// to see pasted sequence longer than segment

				ASSERT(hr == S_OK);

				// Get the paste type
				TIMELINE_PASTE_TYPE tlPasteType;
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetPasteType( &tlPasteType ) )
				&&	(tlPasteType == TL_PASTE_OVERWRITE) )
				{
					// Delete sequences
					m_pSequenceMgr->DeleteBetweenTimes( lClocks, lClocks + dwLength );

					// Delete curves
					POSITION pos2, pos;
					pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
					while(pos)
					{
						// Save the current position
						pos2 = pos;
						CCurveItem* pCurveItem = m_pSequenceMgr->m_lstCurves.GetNext(pos);

						// If the curve occurs between lStart and lEnd, delete it
						if( (pCurveItem->AbsTime() >= lClocks) && (pCurveItem->AbsTime() <= lClocks + (signed)dwLength) ) 
						{
							m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );
							delete pCurveItem;
						}
					}
				}

				POSITION pos = lstSequences.GetHeadPosition();
				while(pos)
				{
					CSequenceItem* pItem = lstSequences.GetNext(pos);

					pItem->m_fSelected = TRUE;
					m_pSequenceMgr->InsertByAscendingTime(pItem);
				}

				pos = lstCurves.GetHeadPosition();
				while(pos)
				{
					CCurveItem* pCurveItem = lstCurves.GetNext(pos);

					pCurveItem->SetDefaultResetValues( m_mtLength );
					pCurveItem->m_fSelected = TRUE;
					m_pSequenceMgr->InsertByAscendingTime( pCurveItem );
				}
			}
			lstSequences.RemoveAll();
			lstCurves.RemoveAll();
		}
	}
	return hr;
}

void CSequenceStrip::DrawMinimizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, BOOL fSelected )
{
	long lNoteHeight = MINIMIZE_HEIGHT / m_bMinimizeNoteRange;
	if( lNoteHeight == 0 )
	{
		lNoteHeight = 1;
	}

	// Save the old background color
	COLORREF crOldBkColor = ::GetBkColor( hDC );

	// Set the notes' color
	::SetBkColor( hDC, ::GetNearestColor(hDC, fSelected ? m_crSelectedNoteColor : m_crUnselectedNoteColor) );

	RECT rectNote;
	while( pos )
	{
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		const MUSIC_TIME mtStart = pSequenceItem->AbsTime();
		if ( (pSequenceItem->m_fSelected == fSelected) && 
			(mtStart + pSequenceItem->m_mtDuration > lStartTime) &&
			(mtStart < lEndTime) )
		{
			// Get position to draw at
			rectNote.top = ((m_bMinimizeTopNote - pSequenceItem->m_bByte1) * MINIMIZE_HEIGHT) / m_bMinimizeNoteRange;
			rectNote.bottom = rectNote.top + lNoteHeight;

			const MUSIC_TIME mtEnd = mtStart + pSequenceItem->m_mtDuration;

			m_pSequenceMgr->m_pTimeline->ClocksToPosition( mtEnd, &rectNote.right );
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( mtStart, &rectNote.left );
			rectNote.left -= lXOffset;
			rectNote.right -= lXOffset;

			if ( rectNote.left == rectNote.right )
			{
				rectNote.right++;
			}

			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
		}
		else if ( pSequenceItem->AbsTime() > lEndTime )
		{
			break;
		}
	}

	// Reset the old background color
	::SetBkColor( hDC, crOldBkColor );
}

void CSequenceStrip::DrawMaximizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fSelected )
{
	// Set the notes' color
	COLORREF crNoteColor;
	if( fSelected )
	{
		crNoteColor = ::GetNearestColor(hDC, m_crSelectedNoteColor);
	}
	else
	{
		crNoteColor = ::GetNearestColor(hDC, m_crUnselectedNoteColor);
	}
	COLORREF crOldBkColor = ::SetBkColor( hDC, crNoteColor );

	// Create the brush to draw the early notes with
	HBRUSH hbrushHatchVert = ::CreateHatchBrush( HS_VERTICAL, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 

	// Create pen for drawing the overlapping note lines
	HPEN hpenOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetNearestColor(hDC, GetSysColor(COLOR_WINDOW)) );
	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenOverlappingLine ));
	HBRUSH hbrushHatchOverlapping = ::CreateHatchBrush( HS_FDIAGONAL, ::GetNearestColor(hDC, COLOR_HATCH_OVERLAPPING) ); 

	RECT rectNote;
	while( pos )
	{
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		MUSIC_TIME mtStartTime = pSequenceItem->AbsTime();
		if ( (pSequenceItem->m_fSelected == fSelected) && (pSequenceItem->m_bByte1 <= nTopNote) &&
			(pSequenceItem->m_bByte1 >= nBottomNote) &&
			(mtStartTime + pSequenceItem->m_mtDuration > lStartTime) &&
			(mtStartTime < lEndTime) )
		{
			// Get position to draw at
			GetNoteRect( pSequenceItem, &rectNote );

			// Move the note so we draw it at the correct position
			rectNote.left -= lXOffset;
			rectNote.right -= lXOffset;

			// Check if the note is early or late
			if( ((mtStartTime >= m_mtLength) || (mtStartTime < 0)) && hbrushHatchVert )
			{
				// Yep - use a vertical hash
				::FillRect( hDC, &rectNote, hbrushHatchVert );
			}
			else
			{
				// Nope - just fill it
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
			}

			const RECT* pRect = m_aNoteRectList[pSequenceItem->m_bByte1].GetFirst();
			while (pRect)
			{
				// compute the intersection of the notes
				CRect rect;
				rect.IntersectRect( pRect, &rectNote);

				// draw it
				if ((rect.left != rect.right) || (rect.top != rect.bottom))
				{
					// These rects have already been offset by -lXOffset
					::MoveToEx( hDC, rect.left, rect.bottom, NULL );
					::LineTo( hDC, rect.left, rect.top );
					::LineTo( hDC, rect.right, rect.top );
					::LineTo( hDC, rect.right, rect.bottom );

					rect.top++;
					rect.left++;
					if( (rect.top < rect.bottom) && (rect.left < rect.right) )
					{
						::SetBkColor( hDC, m_crOverlappingNoteColor );
						//::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
						::FillRect( hDC, &rect, hbrushHatchOverlapping );
						::SetBkColor( hDC, crNoteColor );
					}
				}
				// Get the next overlapping note
				pRect = m_aNoteRectList[pSequenceItem->m_bByte1].GetNext();
			}
			m_aNoteRectList[pSequenceItem->m_bByte1].InsertRect( rectNote );
		}
		else if ( mtStartTime > lEndTime )
		{
			break;
		}
	}

	::SelectObject( hDC, hpenOld );
	if( hbrushHatchVert )
	{
		::DeleteObject( hbrushHatchVert );
	}
	if( hpenOverlappingLine )
	{
		::DeleteObject( hpenOverlappingLine );
	}
	if( hbrushHatchOverlapping )
	{
		::DeleteObject( hbrushHatchOverlapping );
	}

	// Reset the old background color
	::SetBkColor( hDC, crOldBkColor );
}

void CSequenceStrip::DrawHybridMaximizedAccidentals( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote )
{
	// Create and select the font to draw the sharps and flats with
	HFONT hOldFont, hfont;
	hfont = GetAccidentalFont();
	hOldFont = static_cast<HFONT>(::SelectObject( hDC, hfont ));

	// Load the sharp/flat strings
	CString strSharp, strFlat, strNatural;
	strSharp.LoadString(IDS_SHARP_TEXT);
	strFlat.LoadString(IDS_FLAT_TEXT);
	strNatural.LoadString(IDS_NATURAL_TEXT);

	const long lTopAdjust = TOPADJUST;
	const long lBottomAdjust = BOTTOMADJUST;

	// Set up the text drawing modes
	int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
	COLORREF crOldColor = ::SetTextColor( hDC, ::GetNearestColor(hDC, m_crAccidentalColor) );

	long lAccidentalWidth;
	m_pSequenceMgr->m_pTimeline->PositionToClocks( 20, &lAccidentalWidth );

	RECT rectNote;
	long lCurrentMeasure;

	// Determine the first measure we display
	m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lStartTime, &lCurrentMeasure, NULL );
	long lStartMeasureTime;
	lCurrentMeasure = max( lCurrentMeasure, 0 );
	m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lCurrentMeasure, 0, &lStartMeasureTime );

	// Need to search for the first event in this measure
	pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
	POSITION pos2 = NULL;
	while( pos )
	{
		pos2 = pos;
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		if( pSequenceItem->AbsTime() >= lStartMeasureTime )
		{
			// Found first item to use
			break;
		}
		pos2 = NULL;
	}

	// If we found something
	if( pos2 )
	{
		// Reset pos to the index of the first item to use
		pos = pos2;
	}

	// Initialize the accidental array
	InitializeScaleAccidentals();

	while( pos )
	{
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		MUSIC_TIME mtStartTime = max( 0, pSequenceItem->AbsTime());

		if ( mtStartTime > lEndTime )
		{
			break;
		}

		if ( (pSequenceItem->m_bByte1 <= nTopNote) &&
			(pSequenceItem->m_bByte1 >= nBottomNote) )
		{
			long lMeasure;
			m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, mtStartTime, &lMeasure, NULL );
			if( lMeasure != lCurrentMeasure )
			{
				lCurrentMeasure = lMeasure;

				InitializeScaleAccidentals();
			}

			// 0 == NATURAL
			// 1 == SHARP
			//-1 == FLAT
			int iHybridPos, iAccidental;
			// FLATS
			if( m_pSequenceMgr->m_fDisplayingFlats )
			{
				iHybridPos = MIDIToHybridPos( pSequenceItem->m_bByte1, aChromToScaleFlats);
				iAccidental = aChromToFlatAccidentals[pSequenceItem->m_bByte1 % 12];
			}
			// SHARPS
			else
			{
				iHybridPos = MIDIToHybridPos( pSequenceItem->m_bByte1, aChromToScaleSharps);
				iAccidental = aChromToSharpAccidentals[pSequenceItem->m_bByte1 % 12];
			}

			// Check if we need to draw an accidental
			// BUGBUG: This will sometimes cause accidentals to be drawn that aren't actually visible.
			// If mtStartTime is just before lStartTime, and lStartTime is greater than a beat or so,
			// this check will succeed, even thought he accidental is not visible.
			// BUGBUG: If mtStartTime is just greater than lEndTime, this check will fail, even
			// though the accidental should be visible.  DavidY would like to have the accidental
			// visible in this case, though since it's easier not to I've left it as-is.
			if( (iAccidental != m_aiAccidentals[iHybridPos]) && 
				(mtStartTime + lAccidentalWidth > lStartTime)  &&
				(mtStartTime < lEndTime) )
			{
				// Get position to draw at
				GetHybridRect( &rectNote, iHybridPos, 127, mtStartTime, pSequenceItem->m_mtDuration );

				// Put the accidental before the note, if possible
				UINT uFormat;
				rectNote.right = rectNote.left - 2;
				rectNote.left -= (m_lMaxNoteHeight * 3) / 2;
				if( rectNote.left < 0 )
				{
					rectNote.right = (m_lMaxNoteHeight * 3) / 2 - 2;
					rectNote.left = 0;
					uFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
				}
				else
				{
					uFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
				}

				if( m_pSequenceMgr->m_fHasNotationStation )
				{
					rectNote.top -= lTopAdjust;
					rectNote.bottom += lBottomAdjust;
				}

				// Move the note so we draw it at the correct position
				rectNote.left -= lXOffset;
				rectNote.right -= lXOffset;

				if( iAccidental )
				{
					// Draw iAccidental
					if( iAccidental > 0 )
					{
						// Draw sharp before note
						::DrawText( hDC, strSharp, 1, &rectNote, uFormat);
					}
					else
					{
						// Draw flat before note
						::DrawText( hDC, strFlat, 1, &rectNote, uFormat);
					}
				}
				else
				{
					// Draw natural before note
					::DrawText( hDC, strNatural, 1, &rectNote, uFormat);
				}
			}

			m_aiAccidentals[iHybridPos] = iAccidental;
		}
	}
	::SetTextColor( hDC, crOldColor );
	::SetBkMode( hDC, nOldBkMode );
	if( hfont )
	{
		::SelectObject( hDC, hOldFont );
		::DeleteObject( hfont );
	}
}

void CSequenceStrip::DrawHybridMaximizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fSelected )
{
	// Set the notes' color and save the old background color
	COLORREF crNoteColor;
	if( fSelected )
	{
		crNoteColor = ::GetNearestColor(hDC, m_crSelectedNoteColor);
	}
	else
	{
		crNoteColor = ::GetNearestColor(hDC, m_crUnselectedNoteColor);
	}
	COLORREF crOldBkColor = ::SetBkColor( hDC, crNoteColor );

	// Create the brush to draw the early notes with
	HBRUSH hbrushHatchVert = ::CreateHatchBrush( HS_VERTICAL, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 

	// Create pen for drawing the overlapping note lines
	HPEN hpenOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetNearestColor(hDC, GetSysColor(COLOR_WINDOW)) );
	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenOverlappingLine ));
	HBRUSH hbrushHatchOverlapping = ::CreateHatchBrush( HS_FDIAGONAL, ::GetNearestColor(hDC, COLOR_HATCH_OVERLAPPING) ); 

	RECT rectNote;
	while( pos )
	{
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		MUSIC_TIME mtStartTime = pSequenceItem->AbsTime();
		if ( (pSequenceItem->m_fSelected == fSelected) && (pSequenceItem->m_bByte1 <= nTopNote) &&
			(pSequenceItem->m_bByte1 >= nBottomNote) && 
			(mtStartTime + pSequenceItem->m_mtDuration > lStartTime) &&
			(mtStartTime < lEndTime) )
		{
			int iHybridPos;
			// FLATS
			if( m_pSequenceMgr->m_fDisplayingFlats )
			{
				iHybridPos = MIDIToHybridPos( pSequenceItem->m_bByte1, aChromToScaleFlats);
			}
			// SHARPS
			else
			{
				iHybridPos = MIDIToHybridPos( pSequenceItem->m_bByte1, aChromToScaleSharps);
			}

			// Get position to draw at
			GetHybridRect( &rectNote, iHybridPos, pSequenceItem->m_bByte2, mtStartTime, pSequenceItem->m_mtDuration );

			// Move the note so we draw it at the correct position
			rectNote.left -= lXOffset;
			rectNote.right -= lXOffset;

			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);

			if( (pSequenceItem->AbsTime() < 0) && hbrushHatchVert )
			{
				::FillRect( hDC, &rectNote, hbrushHatchVert );
			}

			const RECT* pRect = m_aNoteRectList[0].GetFirst();
			while (pRect)
			{
				// compute the intersection of the notes
				CRect rect;
				rect.IntersectRect( pRect, &rectNote);

				// draw it
				if ((rect.left != rect.right) || (rect.top != rect.bottom))
				{
					// These rects have already been offset by -lXOffset
					::MoveToEx( hDC, rect.left, rect.bottom, NULL );
					::LineTo( hDC, rect.left, rect.top );
					::LineTo( hDC, rect.right, rect.top );
					::LineTo( hDC, rect.right, rect.bottom );
					::LineTo( hDC, rect.left, rect.bottom );

					rect.top++;
					rect.left++;
					if( (rect.top < rect.bottom) && (rect.left < rect.right) )
					{
						::SetBkColor( hDC, m_crOverlappingNoteColor );
						//::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
						::FillRect( hDC, &rect, hbrushHatchOverlapping );
						::SetBkColor( hDC, crNoteColor );
					}
				}
				// Get the next overlapping note
				pRect = m_aNoteRectList[0].GetNext();
			}
			m_aNoteRectList[0].InsertRect( rectNote );
		}
		else if ( mtStartTime > lEndTime )
		{
			break;
		}
	}

	::SelectObject( hDC, hpenOld );
	if( hbrushHatchVert )
	{
		::DeleteObject( hbrushHatchVert );
	}
	if( hpenOverlappingLine )
	{
		::DeleteObject( hpenOverlappingLine );
	}
	if( hbrushHatchOverlapping )
	{
		::DeleteObject( hbrushHatchOverlapping );
	}

	// Reset the old background color
	::SetBkColor( hDC, crOldBkColor );
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateVScroll
//
// Updates the vertical scrollbar's position, range and page size.  This
// is called when the sequence strip is resized
//
void CSequenceStrip::UpdateVScroll()
{
	RECT rectVisible, rectAll;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectVisible;

	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_RECT, &var)));

	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectAll;
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_ENTIRE_STRIP_RECT, &var)));

	RECT oldRect = m_rectVScroll;
	
	// move the vertical scroll bar to where it needs to go
	m_VScrollBar.GetClientRect(&m_rectVScroll);
	m_rectVScroll.left = rectVisible.right - m_rectVScroll.right - 2;
	m_rectVScroll.right = m_rectVScroll.left + m_rectVScroll.right;
	m_rectVScroll.bottom = rectVisible.bottom;
	m_rectVScroll.top = min( rectVisible.bottom, max( rectVisible.top, rectAll.top + m_BitmapZoomInDown.GetBitmapDimension().cy + m_BitmapZoomOutDown.GetBitmapDimension().cy ) );
	
	// Always update
	//if( m_rectVScroll != oldRect )
	{
		// move the scroll bar
		if( m_rectVScroll != oldRect )
		{
			m_VScrollBar.MoveWindow(&m_rectVScroll, TRUE);
		}

		// Get the strip's height
		VARIANT varHeight;
		VERIFY( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &varHeight ) ) );

		if ( (m_fHybridNotation && (V_I4(&varHeight) / m_lMaxNoteHeight >= 38)) ||
			 (!m_fHybridNotation && (V_I4(&varHeight) / m_lMaxNoteHeight >= 127)) )
		{
			// If we can view all 128 notes, disable the scrollbar
			m_VScrollBar.EnableScrollBar(ESB_DISABLE_BOTH);
		}
		else
		{
			// compute the scrollbar pagesize
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);

			// enable the scrollbar
			m_VScrollBar.EnableScrollBar(ESB_ENABLE_BOTH);

			// set the new page size
			si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
			si.nPage = V_I4(&varHeight) / m_lMaxNoteHeight;

			// set the new position
			si.nPos = m_lVScroll / m_lMaxNoteHeight;

			if( m_fHybridNotation )
			{
				si.nMin = 0;
				si.nMax = 37;
			}
			else
			{
				si.nMin = 0;
				si.nMax = 127;
			}

			m_VScrollBar.SetScrollInfo(&si, TRUE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SetVScroll
//
// Set a new vertical scroll value for the sequence strip.  The scrollbar and
// strip are updated accordingly.
//
void CSequenceStrip::SetVScroll(long lNewValue)
{
	// Set the new scroll position
	long lOldScroll = m_lVScroll;
	m_lVScroll = lNewValue;

	// Limit it to the maximum and minimum values
	LimitVScroll();

	// If the scroll position didn't change, return
	if (lOldScroll == m_lVScroll)
	{
		return;
	}

	// Otherwise, pospone drawing operations - this avoids flickering
	// in the strip's title and maximize/minimize button
	HWND hwnd = GetTimelineHWnd();
	if( hwnd )
	{
		::LockWindowUpdate( hwnd );
	}

	// Tell the timeline the new VScroll position
	VARIANT var;
	var.vt = VT_I4;
	V_I4(&var) = m_lVScroll;
	m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var);

	// restore drawing operations (and redraw the strip)
	if( hwnd )
	{
		::LockWindowUpdate( NULL );
	}

	// Tell the new position to the scroll bar
	m_VScrollBar.SetScrollPos(m_lVScroll / m_lMaxNoteHeight);
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::LimitVScroll
//
// Limits the Y scroll value to be within acceptable range.
//
void CSequenceStrip::LimitVScroll() 
{
	VARIANT var;
	if( SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )) )
	{
		long lMax;
		if( m_fHybridNotation )
		{
			lMax = max( 0, 38 - V_I4(&var) / m_lMaxNoteHeight);
		}
		else
		{
			lMax = max( 0, 128 - V_I4(&var) / m_lMaxNoteHeight);
		}

		if (m_lVScroll < 0)
		{
			m_lVScroll = 0;
		}
		else if (m_lVScroll > lMax * m_lMaxNoteHeight)
		{
			m_lVScroll = lMax * m_lMaxNoteHeight;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnVScroll
//
// Called by the vertical scrollbar when the user scrolls.
//
void CSequenceStrip::OnVScroll(UINT nSBCode, UINT nPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lTestPos, lNewPos;
	lTestPos = m_lVScroll / m_lMaxNoteHeight;

	VARIANT var;
	VERIFY( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) );

	switch(nSBCode)
	{
	case SB_TOP:
		lNewPos = 0;
		break;
	case SB_LINEDOWN:
		lNewPos = lTestPos + 1;
		break;
	case SB_LINEUP:
		lNewPos = lTestPos - 1;
		break;
	case SB_PAGEDOWN:
		lNewPos = lTestPos + (V_I4(&var) / m_lMaxNoteHeight);
		break;
	case SB_PAGEUP:
		lNewPos = lTestPos - (V_I4(&var) / m_lMaxNoteHeight);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		lNewPos = nPos;
		break;
	case SB_BOTTOM:
		if( m_fHybridNotation )
		{
			lNewPos = 38 - (V_I4(&var) / m_lMaxNoteHeight);
		}
		else
		{
			lNewPos = 128 - (V_I4(&var) / m_lMaxNoteHeight);
		}
		break;
	case SB_ENDSCROLL:
	default:
		return;
	}

	long lNewVertScroll;
	lNewVertScroll = lNewPos * (m_lMaxNoteHeight);
	if( m_fHybridNotation )
	{
		if ( lNewVertScroll / m_lMaxNoteHeight > 37)
		{
			lNewVertScroll = m_lMaxNoteHeight * 37;
		}
	}
	else
	{
		if ( lNewVertScroll / m_lMaxNoteHeight > 127)
		{
			lNewVertScroll = m_lMaxNoteHeight * 127;
		}
	}
	if ( lNewVertScroll < 0)
	{
		lNewVertScroll = 0;
	}
	if (lNewVertScroll != m_lVScroll)
	{
		SetVScroll( lNewVertScroll );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnVScroll
//
// Windows message handler for vertical scrolling.
//
HRESULT CSequenceStrip::OnVScroll( void )
{
	// update position of scrollbar
	UpdateVScroll();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnSize
//
// Message handler for strip resizing.
//
HRESULT CSequenceStrip::OnSize( void )
{
	// make sure current scroll position is within range (since size was changed)
	long lMaxVertScroll;
	if( m_fHybridNotation )
	{
		lMaxVertScroll = 38 * m_lMaxNoteHeight;
	}
	else
	{
		lMaxVertScroll = 128 * m_lMaxNoteHeight;
	}

	VARIANT var;
	if( SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )) )
	{
		lMaxVertScroll -= V_I4(&var);
	}

	if ( m_lVScroll > lMaxVertScroll )
	{
		SetVScroll( lMaxVertScroll );
	}

	// update position of scrollbar
	UpdateVScroll();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnChar
//
// Message handler for strip resizing.
//
HRESULT CSequenceStrip::OnChar(WPARAM wParam)
{
	if( wParam == 0x11 ) // Ctrl+Q
	{
		return DoQuantize();
	}

	if( wParam == 0x0c ) // Ctrl+L
	{
		return DoVelocity();
	}

	// The following code is very similar to OnWMMessage()'s handler for ID_EDIT_VIEW_HYBRID, etc.
	if( m_svView == SV_NORMAL )
	{
		if( (wParam == 'h') || (wParam == 'H') )
		{
			// This handles notifying the other strips, and adding an Undo state
			ChangeNotationType( !m_fHybridNotation );
		}
		else if( (wParam == 'a') || (wParam == 'A') )
		{
			ChangeZoom(	m_dblVerticalZoom + 0.01);
			// TODO: Add Undo?
			/*
			// Let the object know about the changes
			m_nLastEdit = nLastEdit;
			m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
			m_pSequenceMgr->OnDataChanged(); 
			*/
		}
		else if( (wParam == 'z') || (wParam == 'Z') )
		{
			ChangeZoom(	m_dblVerticalZoom - 0.01);
			// TODO: Add Undo?
			/*
			// Let the object know about the changes
			m_nLastEdit = nLastEdit;
			m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
			m_pSequenceMgr->OnDataChanged(); 
			*/
		}
	}

	return S_OK;
}

HRESULT CSequenceStrip::OnSetCursor( long lXPos, long lYPos)
{
	if (m_svView == SV_MINIMIZED)
	{
		m_hCursor = GetArrowCursor();
		return S_OK;
	}

	SetMouseMode( lXPos, lYPos );
	switch(m_MouseMode)
	{
	case SEQ_MM_NORMAL:
		m_hCursor = GetArrowCursor();
		break;
	case SEQ_MM_ACTIVEMOVE:
	case SEQ_MM_MOVE:
		m_hCursor = GetAllCursor();
		break;
	case SEQ_MM_ACTIVERESIZE_START:
	case SEQ_MM_RESIZE_START:
		m_hCursor = GetResizeStartCursor();
		break;
	case SEQ_MM_ACTIVERESIZE_END:
	case SEQ_MM_RESIZE_END:
		m_hCursor = GetResizeEndCursor();
		break;
	case SEQ_MM_ACTIVERESIZE_VELOCITY:
	case SEQ_MM_RESIZE_VELOCITY:
		m_hCursor = GetNSCursor();
		break;
	default:
		break;
	}
	return S_OK;
}

IDMUSProdBandEdit8a *GetBandEditInterface( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, DWORD dwPChannel )
{
	IDMUSProdBandEdit8a *pIBandEdit = NULL;
	MUSIC_TIME mtTime;
	if( SUCCEEDED( pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
	{
		DMUSPROD_INTERFACEFORPCHANNEL dmpInterfaceForPChannel;
		dmpInterfaceForPChannel.dwPChannel = dwPChannel;
		dmpInterfaceForPChannel.punkInterface = NULL;
		if( SUCCEEDED ( pTimeline->GetParam( GUID_BandInterfaceForPChannel, dwGroupBits, 0, 
															   mtTime, NULL, (void *)&dmpInterfaceForPChannel ) ) )
		{
			if( FAILED ( dmpInterfaceForPChannel.punkInterface->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIBandEdit ) ) )
			{
				pIBandEdit = NULL;
			}

			dmpInterfaceForPChannel.punkInterface->Release();
		}
	}

	return pIBandEdit;
}

IDMUSProdBandMgrEdit *GetBandMgrEditInterface( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, DWORD dwPChannel )
{
	// Initialize the interface to NULL
	IDMUSProdBandMgrEdit *pIDMUSProdBandMgrEdit = NULL;

	// Get the cursor position
	MUSIC_TIME mtTime;
	if( SUCCEEDED( pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
	{
		// Try and find the band manager that has a band that has an instrument for this PChannel
		DMUSPROD_INTERFACEFORPCHANNEL dmpInterfaceForPChannel;
		dmpInterfaceForPChannel.dwPChannel = dwPChannel;
		dmpInterfaceForPChannel.punkInterface = NULL;
		if( SUCCEEDED ( pTimeline->GetParam( GUID_BandMgrEditForPChannel, dwGroupBits, 0, 
															   mtTime, NULL, (void *)&dmpInterfaceForPChannel ) ) )
		{
			// Ask for the edit interface
			if( FAILED ( dmpInterfaceForPChannel.punkInterface->QueryInterface( IID_IDMUSProdBandMgrEdit, (void **)&pIDMUSProdBandMgrEdit ) ) )
			{
				pIDMUSProdBandMgrEdit = NULL;
			}

			dmpInterfaceForPChannel.punkInterface->Release();
		}

		// If we didn't find a band manager
		if( pIDMUSProdBandMgrEdit == NULL )
		{
			// Just look for the first band manager
			IDMUSProdStripMgr *pStripMgr;
			if( SUCCEEDED ( pTimeline->GetStripMgr( GUID_BandMgrEditForPChannel, dwGroupBits, 0, &pStripMgr ) ) )
			{
				// Ask for the edit interface
				if( FAILED ( pStripMgr->QueryInterface( IID_IDMUSProdBandMgrEdit, (void **)&pIDMUSProdBandMgrEdit ) ) )
				{
					pIDMUSProdBandMgrEdit = NULL;
				}

				pStripMgr->Release();
			}
		}
	}

	return pIDMUSProdBandMgrEdit;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DrawFunctionBar

void CSequenceStrip::DrawFunctionBar( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( hDC );

	if( sv == SV_FUNCTIONBAR_NORMAL )
	{
		// get function bar rect
		RECT rectFBar;
		VARIANT var;
		var.vt = VT_BYREF;
		V_BYREF(&var) = &rectFBar;
		if (FAILED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
		{
			return;
		}

		// Draw Zoom buttons
		// Save DC
		RECT rectExcludeClip = { 0, 0, 0, 0 };
		int iSavedDC = ::SaveDC( hDC );
		{
			// fix the clipping region
			RECT rectClip;
			::GetClipBox( hDC, &rectClip );

			POINT point;
			::GetWindowOrgEx( hDC, &point );

			rectClip.left -= point.x;
			rectClip.right -= point.x;
			rectClip.top = -point.y;
			rectClip.bottom -= point.y;

			HRGN hRgn;
			hRgn = ::CreateRectRgnIndirect( &rectClip );
			if( !hRgn )
			{
				return;
			}
			::SelectClipRgn( hDC, hRgn );
			::DeleteObject( hRgn );

			if( m_fZoomInPressed )
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomInDown.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomInDown.GetBitmapDimension().cx - 2, m_lVScroll,
					m_BitmapZoomInDown.GetBitmapDimension().cx, m_BitmapZoomInDown.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			else
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomInUp.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomInUp.GetBitmapDimension().cx - 2, m_lVScroll,
					m_BitmapZoomInUp.GetBitmapDimension().cx, m_BitmapZoomInUp.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			if( m_fZoomOutPressed )
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomOutDown.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomOutDown.GetBitmapDimension().cx - 2, m_BitmapZoomInUp.GetBitmapDimension().cy + m_lVScroll,
					m_BitmapZoomOutDown.GetBitmapDimension().cx, m_BitmapZoomOutDown.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			else
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomOutUp.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomOutUp.GetBitmapDimension().cx - 2, m_BitmapZoomInUp.GetBitmapDimension().cy + m_lVScroll,
					m_BitmapZoomOutUp.GetBitmapDimension().cx, m_BitmapZoomOutUp.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}

			// Set up the rect to exclude
			rectExcludeClip.right = rectFBar.right - 2;
			rectExcludeClip.left = rectExcludeClip.right - m_BitmapZoomInDown.GetBitmapDimension().cx;
			rectExcludeClip.top = m_lVScroll;
			rectExcludeClip.bottom = m_lVScroll + m_BitmapZoomInDown.GetBitmapDimension().cy + m_BitmapZoomOutDown.GetBitmapDimension().cy;
		}
		// Restore the DC
		::RestoreDC( hDC, iSavedDC );

		// Exclude the zooom buttons
		::ExcludeClipRect( hDC, rectExcludeClip.left, rectExcludeClip.top, rectExcludeClip.right, rectExcludeClip.bottom );

		// Get the clipping rectangle
		RECT rectClip, rectNote;
		::GetClipBox( hDC, &rectClip );

		// Pointer to interface from which DLS region text can be obtained
		IDMUSProdBandEdit8a* pIBandEdit = NULL;

		// If the instrument button is visible
		VARIANT varFNHeight;
		m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight );
		if( rectClip.bottom > m_lVScroll + V_I4(&varFNHeight)
		&&	rectClip.top < m_lVScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
		{
			// Try to get the band edit interface
			if( !pIBandEdit )
			{
				pIBandEdit = GetBandEditInterface( m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwPChannel );
			}

			// Initialize the last (displayed) patch to an invalid value
			m_dwLastPatch = INVALID_PATCH;

			// Get the instrument's name
			CString strInstrumentName;
			bool fNoInstrument = true;
			strInstrumentName.LoadString( IDS_NOINSTRUMENT );
			if( pIBandEdit )
			{
				BSTR bstrInstrumentName;
				if( SUCCEEDED( pIBandEdit->GetInstNameForPChannel( m_pSequenceMgr->m_dwPChannel, &bstrInstrumentName ) ) )
				{
					strInstrumentName = bstrInstrumentName;
					::SysFreeString( bstrInstrumentName );

					fNoInstrument = false;
				}

				pIBandEdit->GetPatchForPChannel( m_pSequenceMgr->m_dwPChannel, &m_dwLastPatch );
			}

			// Compute the rectangle for the button
			RECT rectButton;
			rectButton.left = 0;
			rectButton.right = (rectFBar.right - rectFBar.left) - m_rectVScroll.Width() - NEWBAND_BUTTON_WIDTH - 1;
			rectButton.top = m_lVScroll + V_I4(&varFNHeight);
			rectButton.bottom = rectButton.top + INSTRUMENT_BUTTON_HEIGHT;

			// Compute the rectangle for the text
			RECT rectText;
			rectText.top = rectButton.top + ::GetSystemMetrics( SM_CYEDGE );
			rectText.bottom = rectButton.bottom - ::GetSystemMetrics( SM_CYEDGE );
			rectText.left = rectButton.left + ::GetSystemMetrics( SM_CXEDGE );
			rectText.right = rectButton.right - ::GetSystemMetrics( SM_CXEDGE );

			// If no instrument, modify the text to include ellipses (if necessary)
			if( fNoInstrument )
			{
				::DrawText( hDC, strInstrumentName, -1, &rectText, DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_MODIFYSTRING );
			}

			// Set the enable/disable flag for the instrument button
			m_fInstrumentEnabled = !fNoInstrument;

			// Draw the instrument button
			::DrawFrameControl( hDC, &rectButton, DFC_BUTTON, m_fInstrumentPressed ? DFCS_PUSHED | DFCS_BUTTONPUSH : DFCS_BUTTONPUSH );

			// Compute if we need to move the text (because the button is pressed)
			if( m_fInstrumentPressed )
			{
				rectText.left++;
				rectText.top++;
			}

			int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
			if( fNoInstrument )
			{
				// If no instrument, draw the text as embossed (disabled)
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>((const char *)strInstrumentName), NULL, rectText.left, rectText.top, rectText.right - rectText.left, rectText.bottom - rectText.top, DST_TEXT | DSS_DISABLED );
			}
			else
			{
				// Have an instrument, draw normally
				::DrawText( hDC, strInstrumentName, -1, &rectText, DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );
			}
			::SetBkMode( hDC, nOldBkMode );

			// Draw the new band button
			rectButton.left = rectButton.right;
			rectButton.right = rectButton.left + NEWBAND_BUTTON_WIDTH - 1;
			::DrawFrameControl( hDC, &rectButton, DFC_BUTTON, m_fNewBandPressed ? DFCS_PUSHED | DFCS_BUTTONPUSH : DFCS_BUTTONPUSH );

			// Compute if we need to move the bitmap (because the button is pressed)
			int nShiftAmount = m_fNewBandPressed ? 1 : 0;

			// Draw the band bitmap
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapNewBand.GetSafeHandle() ), NULL,
				rectButton.left + GetSystemMetrics( SM_CXEDGE ) + nShiftAmount, rectButton.top + GetSystemMetrics( SM_CYEDGE ) + nShiftAmount,
				m_BitmapNewBand.GetBitmapDimension().cx, m_BitmapNewBand.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );

			// Exclude the instrument buttons
			rectButton.left = 0;
			rectButton.right += 3;
			::ExcludeClipRect( hDC, rectButton.left, rectButton.top, rectButton.right, rectButton.bottom );
		}

		LOGFONT lf;
		memset( &lf, 0 , sizeof(LOGFONT));
		lf.lfHeight = long(MAX_NOTE_HEIGHT * 1.4 * m_dblVerticalZoom);
		//lf.lfWidth = 0;
		//lf.lfEscapement = 0;
		//lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		//lf.lfItalic = FALSE;
		//lf.lfUnderline = FALSE;
		//lf.lfStrikeOut = FALSE;
		//lf.lfCharSet = ANSI_CHARSET;
		//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		//lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
		//lf.lfFaceName = NULL;

		HFONT hfont;
		hfont = ::CreateFontIndirect( &lf );
		if ( !hfont )
		{
			return;
		}

		int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
		HFONT hOldFont = static_cast<HFONT>(::SelectObject( hDC, hfont ));

		if( m_fHybridNotation )
		{
			COLORREF oldColor = ::SetTextColor( hDC, PIANOROLL_BLACKKEY_COLOR );

			int nTopNote, nBottomNote;
			nTopNote = 38 - (rectClip.top / m_lMaxNoteHeight);
			nBottomNote = 38 - (rectClip.bottom / m_lMaxNoteHeight);

			nBottomNote = max( nBottomNote, 13 );
			nTopNote = min( nTopNote, 23 );

			DrawHybridLines( hDC, rectClip, nBottomNote, nTopNote );

			if( m_pSequenceMgr->m_fHasNotationStation )
			{
				// Create the font to draw the treble and bass clefs with
				LOGFONT lf;
				memset( &lf, 0 , sizeof(LOGFONT));
				lf.lfHeight = - long(NOTATION_FONT_CLEF_ZOOMFACTOR * MAX_NOTE_HEIGHT * m_dblVerticalZoom);
				lf.lfCharSet = SYMBOL_CHARSET;
				lf.lfPitchAndFamily = DEFAULT_PITCH;

				CString strFontName;
				strFontName.LoadString(IDS_NOTATION_FONT);
				_tcsncpy( lf.lfFaceName, strFontName, LF_FACESIZE );
				lf.lfWeight = FW_NORMAL;
				//lf.lfWidth = 0;
				//lf.lfEscapement = 0;
				//lf.lfOrientation = 0;
				//lf.lfItalic = FALSE;
				//lf.lfUnderline = FALSE;
				//lf.lfStrikeOut = FALSE;
				//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
				//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				//lf.lfQuality = DEFAULT_QUALITY;
				
				// Create and select the font
				HFONT hfontMusic;
				hfontMusic = ::CreateFontIndirect( &lf );
				if( hfontMusic )
				{
					HFONT hNewOldFont = static_cast<HFONT>(::SelectObject( hDC, hfontMusic ));

					// get function bar rect
					RECT rectLocalFBar = rectFBar;
					{
						// Draw the treble clef
						rectLocalFBar.top = TREBLE_TOP * m_lMaxNoteHeight;
						rectLocalFBar.bottom = TREBLE_BOTTOM * m_lMaxNoteHeight;

						// Load the treble clef string
						CString strClef;
						strClef.LoadString(IDS_TREBLE_CLEF);

						::DrawText( hDC, strClef, -1, &rectLocalFBar, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );

						// Draw the bass clef
						rectLocalFBar.top = BASS_TOP * m_lMaxNoteHeight;
						rectLocalFBar.bottom = BASS_BOTTOM * m_lMaxNoteHeight;

						// Load the bass clef string
						strClef.LoadString(IDS_BASS_CLEF);

						::DrawText( hDC, strClef, -1, &rectLocalFBar, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
					}

					::SelectObject( hDC, hNewOldFont );
					::DeleteObject( hfontMusic );
				}

				// Create the font to draw the sharps and flats with
				hfontMusic = GetAccidentalFont();
				if( hfontMusic )
				{
					HFONT hNewOldFont = static_cast<HFONT>(::SelectObject( hDC, hfontMusic ));

					{
						// Load sharp or flat text
						CString strSymbol;
						if( m_pSequenceMgr->m_fDisplayingFlats )
						{
							strSymbol.LoadString(IDS_FLAT_TEXT);
						}
						else
						{
							strSymbol.LoadString(IDS_SHARP_TEXT);
						}

						const long lTopAdjust = TOPADJUST;
						const long lBottomAdjust = BOTTOMADJUST;

						// Draw treble clef sharps/flats
						for( int i = 0; i < m_pSequenceMgr->m_nNumAccidentals; i++ )
						{
							long lXSymbolOffset = (m_lMaxNoteHeight * (10 + i) ) / 3;
							if( m_pSequenceMgr->m_fDisplayingFlats )
							{
								DrawSymbol( hDC, strSymbol, aTrebleFlatKeyToPosx2[i] + 1, lXSymbolOffset, lTopAdjust, lBottomAdjust );
								DrawSymbol( hDC, strSymbol, aBassFlatKeyToPosx2[i] + 1, lXSymbolOffset, lTopAdjust, lBottomAdjust );
							}
							else
							{
								DrawSymbol( hDC, strSymbol, aTrebleSharpKeyToPosx2[i] + 1, lXSymbolOffset, lTopAdjust, lBottomAdjust );
								DrawSymbol( hDC, strSymbol, aBassSharpKeyToPosx2[i] + 1, lXSymbolOffset, lTopAdjust, lBottomAdjust );
							}
						}
					}

					::SelectObject( hDC, hNewOldFont );
					::DeleteObject( hfontMusic );
				}
			}
			else
			{
				rectNote.left = 0;
				rectNote.right = rectClip.right;

				char strTxt[2];
				ZeroMemory( strTxt, sizeof( char ) * 2 );

				int nVertPos = (38 - nBottomNote) * m_lMaxNoteHeight;
				for( int nValue = nBottomNote; nValue < nTopNote; nValue++ )
				{
					// Skip the 'B' and 'D' next to middle C
					if( nValue == 18 || nValue == 17 )
					{
						nVertPos -= m_lMaxNoteHeight;
						continue;
					}

					rectNote.bottom = nVertPos;
					rectNote.top = nVertPos - m_lMaxNoteHeight;

					strTxt[0] = ScaleToName[ nValue - 13 ];

					::DrawText( hDC, strTxt, -1, &rectNote,	DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );

					nVertPos -= m_lMaxNoteHeight;
				}
			}
			::SetTextColor( hDC, oldColor );
		}
		else
		{
			short	value;
			long	lTop;

			::SetBkColor( hDC, ::GetNearestColor( hDC, GetSysColor(COLOR_WINDOW)) );
			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectClip, NULL, 0, NULL);
			
			HPEN hpenNoteLine;
			hpenNoteLine = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
			if( !hpenNoteLine )
			{
				::SelectObject( hDC, hOldFont );
				::DeleteObject( hfont );
				return;
			}

			// Get IDMUSProdBandEdit interface from which DLS region name can be obtained
			CString strRegionName;
			BSTR bstrRegionName;
			if( !pIBandEdit )
			{
				pIBandEdit = GetBandEditInterface( m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwPChannel );
			}

			HPEN hpenOld = static_cast<HPEN>(::SelectObject( hDC, hpenNoteLine ));
			// TODO: Optimize this to only draw the visible notes (get top and bottom notes)
			// The continue and break statements can then be taken out
			// Also optimize in CPianoRollStrip.cpp
			for( value = 0; value < 128; value++ )
			{
				lTop = ( 127 - value ) * m_lMaxNoteHeight;
				if( lTop > rectClip.bottom )
				{
					continue;
				}
				rectNote.top = lTop;
				rectNote.bottom = lTop + m_lMaxNoteHeight - HORIZ_LINE_HEIGHT;
				if( rectNote.bottom < rectClip.top )
				{
					break;
				}
				rectNote.left = rectClip.left;
				rectNote.right = rectClip.right;
				if( PianoKeyColor[ value % 12 ] == BLACK )
				{
					::SetBkColor( hDC, ::GetNearestColor( hDC, PIANOROLL_BLACKKEY_COLOR) );
				}
				else
				{
					::SetBkColor( hDC, ::GetNearestColor( hDC, PIANOROLL_WHITEKEY_COLOR) );
				}
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);

				::MoveToEx( hDC, rectNote.left, rectNote.bottom, NULL );
				::LineTo( hDC, rectNote.right, rectNote.bottom );


				// Get DLS Region name
				strRegionName.Empty();
				if( pIBandEdit )
				{
					if( SUCCEEDED ( pIBandEdit->GetDLSRegionName( m_pSequenceMgr->m_dwPChannel, (BYTE)value, &bstrRegionName ) ) )
					{
						strRegionName = bstrRegionName;
						::SysFreeString( bstrRegionName );
					}
				}

				const COLORREF oldColor = ::GetTextColor( hDC );

				if( strRegionName.IsEmpty() == FALSE )
				{
					// Draw DLS region name
					rectNote.left = 0;
					if( value % 12 == 0 )
					{
						CString cstrTxt;
						cstrTxt.Format( "C%d: ", value/12 );
						::SetTextColor( hDC, PIANOROLL_RED_COLOR );
						::DrawText( hDC, cstrTxt, -1, &rectNote,
							DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );

						SIZE size;
						::GetTextExtentPoint( hDC, cstrTxt, cstrTxt.GetLength(), &size );
						rectNote.left = size.cx;
					}

					::SetTextColor( hDC, (PianoKeyColor[ value % 12 ] == BLACK) ? PIANOROLL_WHITEKEY_COLOR : PIANOROLL_BLACKKEY_COLOR );

					::DrawText( hDC, strRegionName, -1, &rectNote,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
				}
				else if ( (m_pSequenceMgr->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS)
					 &&	  ((m_pSequenceMgr->m_dwPChannel & 0xF) == 9)
					 &&	  (value >=27) && (value <= 87) )
				{
					// Draw GM drum kit names
					::SetTextColor( hDC, (PianoKeyColor[ value % 12 ] == BLACK) ? PIANOROLL_WHITEKEY_COLOR : PIANOROLL_BLACKKEY_COLOR );

					rectNote.left = 0;
					::DrawText( hDC, acDrums[value-27], -1, &rectNote,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
				}
				else if (value % 12 == 0)
				{
					::SetTextColor( hDC, PIANOROLL_BLACKKEY_COLOR );

					CString cstrTxt;
					//cstrTxt.Format( "%s%d", CString(MidiValueToName).Mid((value % 12)*5, 5) , value/12 );
					cstrTxt.Format( "C %d", value/12 );
					rectNote.left = 0;
					::DrawText( hDC, cstrTxt, -1, &rectNote,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
				}

				::SetTextColor( hDC, oldColor );
			}
			::SelectObject( hDC, hpenOld );
			::DeleteObject( hpenNoteLine );

			if (rectClip.top <= m_lVScroll)
			{
				HPEN	hpenTopLine = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
				if( hpenTopLine )
				{
					HPEN hpenOld = static_cast<HPEN>(::SelectObject( hDC, hpenTopLine ));
					::MoveToEx( hDC, rectClip.left, m_lVScroll, NULL );
					::LineTo( hDC, rectClip.right, m_lVScroll );
					::SelectObject( hDC, hpenOld );
					::DeleteObject( hpenTopLine );
				}
			}
		}

		if( pIBandEdit )
		{
			pIBandEdit->Release();
		}

		::SetBkMode( hDC, nOldBkMode );
		::SelectObject( hDC, hOldFont );
		::DeleteObject( hfont );
	}
}

void CSequenceStrip::DrawDarkHybridLines( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote )
{
	DrawHybridLinesHelper( hDC, rectClip, nOldBottomNote, nOldTopNote, RGB(230,230,230), m_lMaxNoteHeight, true );
}

void CSequenceStrip::DrawHybridLines( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote )
{
	DrawHybridLinesHelper( hDC, rectClip, nOldBottomNote, nOldTopNote, RGB(255,255,255), m_lMaxNoteHeight, m_iHaveFocus != 1 );
}

void CSequenceStrip::DrawDarkHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote )
{
	DrawHorizontalLinesHelper( hDC, rectClip, nBottomNote, nTopNote, RGB(230, 230, 230), RGB(200, 200, 200), m_lMaxNoteHeight, true );
}

void CSequenceStrip::DrawHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote )
{
	DrawHorizontalLinesHelper( hDC, rectClip, nBottomNote, nTopNote, RGB(255, 255, 255), RGB(230, 230, 230), m_lMaxNoteHeight, m_iHaveFocus != 1 );
}

void CSequenceStrip::GetHybridRect( RECT* pRect, int iHybridPos, int iVelocity, MUSIC_TIME mtStart, MUSIC_TIME mtDuration )
{
	ASSERT( pRect );

	if ( m_pSequenceMgr->m_pTimeline )
	{
		pRect->bottom = ((76 - iHybridPos) * m_lMaxNoteHeight) >> 1;
		
		// Find the top of the note's rectangle
		// The velocity offset isn't precisely correct - but it's close enough if m_lMaxNoteHeight < 127.
		long lHeight = ((iVelocity * m_lMaxNoteHeight) >> 7);
		pRect->bottom -= (m_lMaxNoteHeight - lHeight) >> 1;
		pRect->top = pRect->bottom - lHeight;
		
		// Ensure the note is at least one pixel high
		if( pRect->top == pRect->bottom )
		{
			pRect->top--;
		}

		// Convert its starting time to a position
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( mtStart, &pRect->left );

		// Find the position of the note's end, or the end of the segment, whichever is sooner
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( min( m_mtLength, mtStart + mtDuration), &pRect->right );

		// Ensure the note is at least one pixel long
		if ( pRect->right <= pRect->left )
		{
			pRect->right = pRect->left + 1;
		}
	}
	else
	{
		memset( pRect, 0, sizeof(RECT) );
	}
}

void CSequenceStrip::GetNoteRect( const CSequenceItem* pSeqItem, RECT* pRect )
{
	long position;

	ASSERT( pRect );
	ASSERT( pSeqItem );

	if ( pSeqItem && m_pSequenceMgr->m_pTimeline )
	{
		if( m_fHybridNotation )
		{
			// FLATS
			int iHybridPos;
			if( m_pSequenceMgr->m_fDisplayingFlats )
			{
				iHybridPos = MIDIToHybridPos( pSeqItem->m_bByte1, aChromToScaleFlats);
			}
			// SHARPS
			else
			{
				iHybridPos = MIDIToHybridPos( pSeqItem->m_bByte1, aChromToScaleSharps);
			}

			// Get position to draw at
			GetHybridRect( pRect, iHybridPos, pSeqItem->m_bByte2, pSeqItem->AbsTime(), pSeqItem->m_mtDuration );
		}
		else
		{
			// set the bottom
			pRect->bottom = ( 128 - pSeqItem->m_bByte1 ) * m_lMaxNoteHeight - HORIZ_LINE_HEIGHT;

			// Find the top of the note's rectangle
			// The velocity offset isn't precisely correct - but it's close enough if m_lMaxNoteHeight < 127.
			pRect->top = pRect->bottom - ((pSeqItem->m_bByte2 * m_lMaxNoteHeight) >> 7);
		
			// Ensure the note is at least one pixel high
			if( pRect->top == pRect->bottom )
			{
				pRect->top--;
			}

			// Convert the note's starting time to a position
			position = pSeqItem->AbsTime();
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( position, &pRect->left );

			// Find the position of the note's end, or the end of the segment, whichever is sooner
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( min( m_mtLength + m_mtLastLateTime, position + pSeqItem->m_mtDuration), &pRect->right );

			// Ensure the note is at least one pixel long
			if ( pRect->right <= pRect->left )
			{
				pRect->right = pRect->left + 1;
			}
		}
	}
	else
	{
		memset( pRect, 0, sizeof(RECT) );
	}
}
void CSequenceStrip::SetMouseMode( long lXPos, long lYPos)
{
	CSequenceItem* pSeqItem = NULL;
	RECT rectNote; // left, right, top, bottom
	pSeqItem = GetSeqItemAndRectFromPoint( lXPos, lYPos, &rectNote );
	m_MouseMode = SEQ_MM_NORMAL;
	if( pSeqItem )
	{
		{
			// we're inside a note
			LONG lWidth, lHeight;
			lWidth = rectNote.right - rectNote.left + 1;
			lHeight = rectNote.bottom - rectNote.top;
			if( lWidth >= 3 )
			{
				if( lWidth >= 12 )
				{
					lWidth /= 4;
				}
				else
				{
					lWidth /= 3;
				}
				lWidth --; // Because rectNote.right and rectNote.left are selectable.
				if ( lXPos >= rectNote.right - lWidth )
				{
					m_MouseMode = SEQ_MM_RESIZE_END;
				}
				else if ( lXPos - rectNote.left <= lWidth )
				{
					m_MouseMode = SEQ_MM_RESIZE_START;
				}
				else if ( lYPos <= rectNote.top + lHeight/2 )
				{
					m_MouseMode = SEQ_MM_RESIZE_VELOCITY;
				}
				else
				{
					m_MouseMode = SEQ_MM_MOVE;
				}
			}
			else // if ( lWidth <= 2 )
			{
				if ( lXPos >= rectNote.right ) //- lWidth/4 is always 0
				{
					m_MouseMode = SEQ_MM_RESIZE_END;
				}
				// If the note is just one pixel high, then this is true when the mouse is on the note and
				// SEQ_MM_MOVE will be set when the mouse is just below the note.
				else if ( lYPos <= rectNote.top + lHeight/2 )
				{
					m_MouseMode = SEQ_MM_RESIZE_VELOCITY;
				}
				else
				{
					m_MouseMode = SEQ_MM_MOVE;
				}
			}
		}
	}
}
CSequenceItem* CSequenceStrip::GetSeqItemAndRectFromPoint( long lXPos, long lYPos, RECT *pRect )
{
	RECT rectNote, rectSave;

	CSequenceItem* pItemSave = NULL;
	ASSERT( m_pSequenceMgr != NULL );
	if (m_pSequenceMgr != NULL)
	{
		// See which MIDINote grid lYPos is in
		// TODO: Optimize this for Hybrid notation
		// Also optimize in PianoRollStrip.cpp
		int nMIDINote = 127 - (lYPos / m_lMaxNoteHeight);

		// Compute the time of the point asked far
		MUSIC_TIME mtPointTime = 0;

		// Compute the position of the point
		m_pSequenceMgr->m_pTimeline->PositionToClocks( lXPos, &mtPointTime );
		
		// Compute the width of a pixel
		MUSIC_TIME mtPixelWidth;
		m_pSequenceMgr->m_pTimeline->PositionToClocks( 1, &mtPixelWidth );
		// This is a faster way of doing AbsTime() - (mtPixelWidth / 2) - 1 < mtPointTime
		mtPointTime += (mtPixelWidth / 2) + 1;

		POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
		while( pos )
		{
			CSequenceItem *pTmpItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );

			if( pTmpItem->AbsTime() > mtPointTime )
			{
				// Gone too far in list - no need to check further
				break;
			}

			// Check to see if its MIDI value is the one we're looking for
			if( !m_fHybridNotation && (pTmpItem->m_bByte1 != nMIDINote) )
			{
				continue;
			}

			GetNoteRect( pTmpItem, &rectNote );

			// if yPos is above or below the note, continue
			if( ( lYPos < rectNote.top ) || ( lYPos > rectNote.bottom ) )
			{
				continue;
			}

			// Check if lXPos is before the right side of the note
			// Also check against the left side, since mtPointTime may be LONG_MAX.
			if( (lXPos <= rectNote.right) && (lXPos >= rectNote.left) )
			{
				if( !pItemSave )
				{
					pItemSave = pTmpItem;
					rectSave = rectNote;
					continue;
				}
				else
				{
					// Use the selected note
					if( pTmpItem->m_fSelected && !pItemSave->m_fSelected ) 
					{
						pItemSave = pTmpItem;
						rectSave = rectNote;
						continue;
					}

					// If selection state is the same
					if( pItemSave->m_fSelected == pTmpItem->m_fSelected )
					{
						// If start time is the same
						if( pTmpItem->AbsTime() == pItemSave->AbsTime() )
						{
							// Use note with least velocity
							if ( pTmpItem->m_bByte2 <= pItemSave->m_bByte2 ) 
							{
								pItemSave = pTmpItem;
								rectSave = rectNote;
								continue;
							}
						}
						else
						{
							// Use note with later start time
							pItemSave = pTmpItem;
							rectSave = rectNote;
							continue;
						}
					}
				}
			}
		}
	}

	if( pRect && pItemSave )
	{
		memcpy( pRect, &rectSave, sizeof(RECT) );
	}

	return pItemSave;
}

HCURSOR CSequenceStrip::GetResizeStartCursor(void)
{
	static HCURSOR hCursorResizeStart;
	if (!hCursorResizeStart)
		hCursorResizeStart = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_STARTEDIT) );
	return hCursorResizeStart;
}

HCURSOR CSequenceStrip::GetResizeEndCursor(void)
{
	static HCURSOR hCursorResizeEnd;
	if (!hCursorResizeEnd)
		hCursorResizeEnd = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_ENDEDIT) );
	return hCursorResizeEnd;
}

HCURSOR CSequenceStrip::GetArrowCursor(void)
{
	static HCURSOR hCursorArrow;
	if (!hCursorArrow)
		hCursorArrow = LoadCursor( NULL, IDC_ARROW );
	return hCursorArrow;
}

HCURSOR CSequenceStrip::GetNSCursor(void)
{
	static HCURSOR hCursorSizeNS;
	if (!hCursorSizeNS)
		hCursorSizeNS = LoadCursor( NULL, IDC_SIZENS ); // North/south resize
	return hCursorSizeNS;
}

HCURSOR CSequenceStrip::GetAllCursor(void)
{
	static HCURSOR hCursorSizeALL;
	if (!hCursorSizeALL)
		//hCursorSizeALL = LoadCursor( NULL, IDC_SIZEALL ); // All resize
		hCursorSizeALL = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_MOVE) );
	return hCursorSizeALL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UnselectGutterRange

void CSequenceStrip::UnselectGutterRange( void )
{
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_fSelecting = TRUE;
	m_bSelectionCC = 0xFF;
	m_wSelectionParamType = 0xFFFF;
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_fSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SelectInRect

// return: whether anything changed or not
BOOL CSequenceStrip::SelectInRect( CRect* pRect )
{
	BOOL fChange = FALSE;
	int iRes = 0;
	POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
	if( pos != NULL )
	{
		pRect->NormalizeRect();

		RECT rectSel = *pRect;

		// Compute the low and high MIDI values that rectSel touch
		long lMIDILow, lMIDIHigh;
		if( m_fHybridNotation )
		{
			// Convert from rectSel.bottom to a MIDI value
			lMIDILow = PositionToMIDIValue( rectSel.bottom );

			// lMIDILow could be 128, if rectSel.bottom is in the top half of G10
			if( lMIDILow > 127 )
			{
				lMIDILow = 127;
			}

			// Notes a full scale value below may overlap onto our space
			lMIDILow -= 3;
			if( lMIDILow < 0 )
			{
				lMIDILow = 0;
			}

			// Convert from rectSel.top to a MIDI value
			lMIDIHigh = PositionToMIDIValue( rectSel.top );

			// Notes a full scale value above may overlap onto our space
			lMIDIHigh += 3;
			if( lMIDIHigh > 127 )
			{
				lMIDIHigh = 127;
			}
		}
		else
		{
			lMIDILow = 127 - (rectSel.bottom / m_lMaxNoteHeight);
			lMIDIHigh = 127 - (rectSel.top / m_lMaxNoteHeight);
		}

		// m_pActiveNote will be updated by the method UpdateSelectionState, called
		// if any of the item's selection state changes
		CSequenceItem *pSeqItem;
		while( pos )
		{
			pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
			// If the note is within the MIDI value range
			if ( (pSeqItem->m_bByte1 <= lMIDIHigh) &&
				 (pSeqItem->m_bByte1 >= lMIDILow) )
			{
				// Get the rectangle defining the note
				RECT rect;
				GetNoteRect( pSeqItem, &rect );

				// Check to see if note is within our horizontal selection range
				if( (rectSel.left <= rect.left) && (rect.left <= rectSel.right) )
				{
					// Check to see if note is within our vertical selection range
					if( ((rectSel.top <= rect.top) && (rect.top <= rectSel.bottom)) ||
						((rectSel.top <= rect.bottom) && (rect.bottom <= rectSel.bottom)) ||
						((rect.top <= rectSel.top) && (rectSel.top <= rect.bottom)) ||
						((rect.top <= rectSel.bottom) && (rectSel.bottom <= rect.bottom)) )
					{
						iRes ++;
						if( !pSeqItem->m_fSelected )
						{
							pSeqItem->m_fSelected = TRUE;
							fChange = TRUE;
						}
					}
					else if( pSeqItem->m_fSelected )
					{
						pSeqItem->m_fSelected = FALSE;
						fChange = TRUE;
					}
				}
				else if( pSeqItem->m_fSelected )
				{
					pSeqItem->m_fSelected = FALSE;
					fChange = TRUE;
				}
			}
			else if( pSeqItem->m_fSelected )
			{
				pSeqItem->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
	}
	if( fChange )
	{
		m_pSequenceMgr->RefreshPropertyPage( );
		UpdateSelectionState();
	}
	return fChange;
}

/*
/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetStripRect

BOOL CSequenceStrip::GetStripRect(CRect& rectStrip)
{
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectStrip;
	if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
	{
		return FALSE;
	}

	// adjust the strip rect to encompass the WHOLE strip, not only the part
	// that is visible on the screen
	long lHeight;
	if( m_svView == SV_MINIMIZED )
	{
		lHeight = MINIMIZE_HEIGHT;
	}
	else
	{
		//if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
		//{
		//	return FALSE;
		//}
		//lHeight = V_I4(&var);
		lHeight = m_lMaxNoteHeight * 128;
	}

	CPoint ptTop(0, 0);
	CPoint ptBottom(0, lHeight);
	
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &ptTop)));
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &ptBottom)));

	rectStrip.top = ptTop.y;
	rectStrip.bottom = ptBottom.y;
	
	return TRUE;
}
*/


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetStripRect

BOOL CSequenceStrip::GetStripRect(CRect& rectStrip)
{
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectStrip;
	if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
	{
		return FALSE;
	}

	// adjust the strip rect to encompass the WHOLE strip, not only the part
	// that is visible on the screen
	long lHeight;
	if( m_svView == SV_MINIMIZED )
	{
		lHeight = MINIMIZE_HEIGHT;
	}
	else
	{
		if( m_fHybridNotation )
		{
			lHeight = m_lMaxNoteHeight * 38;
		}
		else
		{
			lHeight = m_lMaxNoteHeight * 128;
		}
	}

	CPoint ptTop(0, 0);
	CPoint ptBottom(0, lHeight);
	
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &ptTop)));
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &ptBottom)));

	rectStrip.top = ptTop.y;
	rectStrip.bottom = ptBottom.y;
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RegisterMidi

void CSequenceStrip::RegisterMidi() 
{
	//TRACE("CSequenceStrip::RegisterMidi %d %d\n", m_pSequenceMgr->m_dwPChannel, m_cRef);
	if ( (m_pSequenceMgr->m_pIConductor != NULL) && !m_fMIDIInRegistered )
	{
		//TRACE("Register\n");
		REGISTER_MIDI_IN(m_pSequenceMgr->m_pIConductor, m_dwCookie);
		m_fMIDIInRegistered = TRUE;
		// Release the Reference gained in the REGISTER_MIDI_IN call.
		// If we don't do this, we will never be destroyed (because our RefCount
		// will not go down to zero.)
		//Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UnRegisterMidi

void CSequenceStrip::UnRegisterMidi()
{
	//TRACE("CSequenceStrip::UnRegisterMidi %d %d\n", m_pSequenceMgr->m_dwPChannel, m_cRef);
	if ( (m_pSequenceMgr->m_pIConductor != NULL) && m_fMIDIInRegistered )
	{
		// Send fake MIDI_NOTEOFF messages for all currently playing notes
		// But only if we're not shutting down
		if( !m_pSequenceMgr->m_fShuttingDown )
		{
			REFERENCE_TIME rtTime;
			if( SUCCEEDED(m_pSequenceMgr->m_pIDMPerformance->GetTime( &rtTime, NULL )) )
			{
				//TRACE("NoteOffs\n");
				for( BYTE bNote = 0; bNote < 128; bNote++ )
				{
					if( m_bVelocity[bNote] )
					{
						OnMidiMsg( rtTime, MIDI_NOTEOFF, bNote, m_bVelocity[bNote] );
					}
				}
			}
		}

		if( m_fMIDIThruEnabled )
		{
			CancelPChannelThru();
		}

		// If we're not playing, send all notes off to the performance engine
		SendAllNotesOffIfNotPlaying();

		//TRACE("UnRegister\n");

		UNREGISTER_MIDI_IN(m_pSequenceMgr->m_pIConductor, m_dwCookie);
		/*
		// Add a reference, since UNREGISTER_MIDI_IN will cause one to be taken away.
		if ( m_cRef > 0 )
		{
			AddRef();
			UNREGISTER_MIDI_IN(m_pSequenceMgr->m_pIConductor, m_dwCookie);
		}
		else
		{
			m_cRef = 2;
			UNREGISTER_MIDI_IN(m_pSequenceMgr->m_pIConductor, m_dwCookie);
			m_cRef = 0;
		}
		*/
		m_fMIDIInRegistered = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::IsEnginePlaying

BOOL CSequenceStrip::IsEnginePlaying( void ) const
{
	return (m_pSequenceMgr->m_pSegmentState != NULL) ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SendAllNotesOffIfNotPlaying

void CSequenceStrip::SendAllNotesOffIfNotPlaying( void )
{
	// If we're not playing, send all notes off to the performance engine
	if ( !IsEnginePlaying() )
	{
		DMUS_MIDI_PMSG *pDMMIDIEvent = NULL;
		if( SUCCEEDED( m_pSequenceMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
		{
			ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
			// PMSG fields
			pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMMIDIEvent->dwPChannel = m_pSequenceMgr->m_dwPChannel;
			pDMMIDIEvent->dwVirtualTrackID = 1;
			pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

			// DMMIDIEvent fields
			pDMMIDIEvent->bStatus = MIDI_CCHANGE;
			// Send Reset All Controllers (121)
			pDMMIDIEvent->bByte1 = 121;

			m_pSequenceMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
			// The playback engine will release the event

			if( SUCCEEDED( m_pSequenceMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
			{
				ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
				// PMSG fields
				pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
				pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
				pDMMIDIEvent->dwPChannel = m_pSequenceMgr->m_dwPChannel;
				pDMMIDIEvent->dwVirtualTrackID = 1;
				pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

				// DMMIDIEvent fields
				pDMMIDIEvent->bStatus = MIDI_CCHANGE;
				// Send All Notes Off (123)
				pDMMIDIEvent->bByte1 = 123;

				m_pSequenceMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
				// The playback engine will release the event
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnMidiMsg

HRESULT CSequenceStrip::OnMidiMsg(REFERENCE_TIME rtTime, 
									   BYTE bStatus, 
									   BYTE bData1, 
									   BYTE bData2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceMgr != NULL );
	if (m_pSequenceMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	if (m_pSequenceMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// If shutting down, ignore all MIDI messages.
	if( m_pSequenceMgr->m_fShuttingDown )
	{
		return E_UNEXPECTED;
	}

	// If not recording, just pass through the MIDI input
	if( !m_pSequenceMgr->m_fRecordEnabled )
	{
		// If we're not already thruing, output a message to the performance engine
		if ( !m_fMIDIThruEnabled )
		{
			/* This sometimes makes the notes play forever.
			if( (bStatus & 0xF0) == MIDI_NOTEON )
			{
				DMUS_NOTE_PMSG *pDMNoteEvent = NULL;
				if( FAILED( m_pSequenceMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG **)&pDMNoteEvent ) ) )
				{
					return;
				}

				ASSERT( pDMNoteEvent != NULL );
				ZeroMemory( pDMNoteEvent, sizeof(DMUS_NOTE_PMSG) );
				// PMSG fields
				pDMNoteEvent->dwSize = sizeof(DMUS_NOTE_PMSG);
			//	pDMNoteEvent->rtTime = 0;
			//	pDMNoteEvent->mtTime = 0;
				pDMNoteEvent->dwFlags = DMUS_PMSGF_REFTIME;
				pDMNoteEvent->dwPChannel = m_pSequenceMgr->m_dwPChannel;
				pDMNoteEvent->dwVirtualTrackID = 1;
			//	pDMNoteEvent->pTool = NULL;
			//	pDMNoteEvent->pGraph = NULL;
				pDMNoteEvent->dwType = DMUS_PMSGT_NOTE;
			//	pDMNoteEvent->punkUser = 0;
				// DMNoteEvent fields
				pDMNoteEvent->mtDuration = LONG_MAX / 2;
			//	pDMNoteEvent->wMusicValue = pPropNote->m_wMusicvalue;
			//	pDMNoteEvent->wMeasure = (WORD) pPropNote->m_lStartBar;
			//	pDMNoteEvent->nOffset = pPropNote->m_nOffset;
			//	pDMNoteEvent->bBeat = (BYTE) pPropNote->m_lStartBeat;
			//	pDMNoteEvent->bGrid = (BYTE) pPropNote->m_lStartGrid;
				pDMNoteEvent->bVelocity = bData2;
				pDMNoteEvent->bFlags = DMUS_NOTEF_NOTEON;
			//	pDMNoteEvent->bTimeRange = pPropNote->m_bTimeRange;
			//	pDMNoteEvent->bDurRange = pPropNote->m_bDurRange;
			//	pDMNoteEvent->bVelRange = pPropNote->m_bVelRange;
			//	pDMNoteEvent->bInversionID = pPropNote->m_bInversionId;
			//	pDMNoteEvent->bPlayModeFlags = pPropNote->m_bPlayMode;
				pDMNoteEvent->bMidiValue = bData1;

				DMUS_PMSG *pPMsg = (DMUS_PMSG *)pDMNoteEvent;
				m_pSequenceMgr->SendPMsg( pPMsg );
			}
			else
			*/
			{
				DMUS_MIDI_PMSG *pDMMIDIEvent = NULL;
				if( SUCCEEDED( m_pSequenceMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
				{
					ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
					// PMSG fields
					pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
				//	pDMNoteEvent->rtTime = 0;
				//	pDMNoteEvent->mtTime = 0;
					pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
					//m_pSequenceMgr->m_pIDMPerformance->GetTime( &pDMMIDIEvent->rtTime, &pDMMIDIEvent->mtTime );
					//pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_MUSICTIME;
					pDMMIDIEvent->dwPChannel = m_pSequenceMgr->m_dwPChannel;
					pDMMIDIEvent->dwVirtualTrackID = 1;
				//	pDMNoteEvent->pTool = NULL;
				//	pDMNoteEvent->pGraph = NULL;
					pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;
				//	pDMNoteEvent->punkUser = 0;

					// DMMIDIEvent fields
					pDMMIDIEvent->bStatus = bStatus;
					pDMMIDIEvent->bByte1 = bData1;
					pDMMIDIEvent->bByte2 = bData2;
				//	pDMMIDIEvent->bPad[0] = 0;

					m_pSequenceMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
					// The playback engine will release the event
				}
			}
		}
		return S_OK;
	}

	// Note On
	if((int)(bStatus & 0xF0) == (int)MIDI_NOTEON)
	{
		//TRACE("Inside  CSequenceStrip::OnMidiMsg going to play %d note on with %d at %d\n", bData1, bData2, ::timeGetTime());
		if (!IsEnginePlaying())
		{
			return RecordStepNoteOn( bData1, bData2 );
		}
		else
		{
			if( FAILED( RecordRealTimeNoteOn( rtTime, bData1, bData2 ) ) )
			{
				TRACE("MIDIStripMgr: RecordRealTimeNoteOn failed, falling back to RecordStepNoteOn\n");
				return RecordStepNoteOn( bData1, bData2 );
			}
			return S_OK;
		}
	}

	// Note Off
	if((int)(bStatus & 0xF0) == (int)MIDI_NOTEOFF)
	{
		//TRACE("Inside  CSequenceStrip::OnMidiMsg going to play %d note off at %d\n", bData1, ::timeGetTime());
		if ( !IsEnginePlaying() )
		{
			return RecordStepNoteOff( bData1 );
		}
		else
		{
			return RecordRealTimeNoteOff( rtTime, bData1 );
		}
	}

	// Curves
	if( (int)(bStatus & 0xF0) == (int)MIDI_PBEND 
	||  (int)(bStatus & 0xF0) == (int)MIDI_CCHANGE 
	||  (int)(bStatus & 0xF0) == (int)MIDI_PTOUCH 
	||  (int)(bStatus & 0xF0) == (int)MIDI_MTOUCH )
	{
		// No step recording
		if( IsEnginePlaying() )
		{
			return RecordRealTimeCurve( rtTime, bStatus, bData1, bData2 );
		}
	}
	
	//TRACE("OnMidiMsg out at %d\n", ::timeGetTime());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RecordStepNoteOn

HRESULT CSequenceStrip::RecordStepNoteOn( BYTE bData1, BYTE bData2 )
{
	if (m_bVelocity[bData1] != 0)
	{
		return E_UNEXPECTED;
	}

	if (bData2 > 0)
	{
		m_bVelocity[bData1] = bData2;
	}
	else
	{
		m_bVelocity[bData1] = 127;
	}
	m_mtStartTime[bData1] = m_lInsertTime;

	// Can't call m_pSequenceMgr->PlayNote, because that will clip off any notes that are already playing

	// Play the entire note
	if( m_pSequenceMgr->m_pIDMPerformance )
	{
		REFERENCE_TIME rtLatency;
		m_pSequenceMgr->m_pIDMPerformance->GetLatencyTime( &rtLatency );

		DMUS_NOTE_PMSG *pDMNoteEvent = NULL;
		if( SUCCEEDED( m_pSequenceMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG **)&pDMNoteEvent ) ) )
		{
			ASSERT( pDMNoteEvent != NULL );
			ZeroMemory( pDMNoteEvent, sizeof(DMUS_NOTE_PMSG) );
			// PMSG fields
			pDMNoteEvent->dwSize = sizeof(DMUS_NOTE_PMSG);
			pDMNoteEvent->rtTime = rtLatency;
		//	pDMNoteEvent->mtTime = 0;
			pDMNoteEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMNoteEvent->dwPChannel = m_pSequenceMgr->m_dwPChannel;
			pDMNoteEvent->dwVirtualTrackID = 1;
		//	pDMNoteEvent->pTool = NULL;
		//	pDMNoteEvent->pGraph = NULL;
			pDMNoteEvent->dwType = DMUS_PMSGT_NOTE;
		//	pDMNoteEvent->punkUser = 0;
			// DMNoteEvent fields
			pDMNoteEvent->mtDuration = max( GetSnapAmount( m_lInsertTime ) - 1, GetGridClocks(m_lInsertTime) / 2 );
		//	pDMNoteEvent->wMusicValue = pPropNote->m_wMusicvalue;
		//	pDMNoteEvent->wMeasure = (WORD) pPropNote->m_lStartBar;
		//	pDMNoteEvent->nOffset = pPropNote->m_nOffset;
		//	pDMNoteEvent->bBeat = (BYTE) pPropNote->m_lStartBeat;
		//	pDMNoteEvent->bGrid = (BYTE) pPropNote->m_lStartGrid;
			pDMNoteEvent->bVelocity = bData2;
			pDMNoteEvent->bFlags = DMUS_NOTEF_NOTEON;
		//	pDMNoteEvent->bTimeRange = pPropNote->m_bTimeRange;
		//	pDMNoteEvent->bDurRange = pPropNote->m_bDurRange;
		//	pDMNoteEvent->bVelRange = pPropNote->m_bVelRange;
		//	pDMNoteEvent->bInversionID = pPropNote->m_bInversionId;
		//	pDMNoteEvent->bPlayModeFlags = pPropNote->m_bPlayMode;
			pDMNoteEvent->bMidiValue = bData1;

			DMUS_PMSG *pPMsg = (DMUS_PMSG *)pDMNoteEvent;
			m_pSequenceMgr->SendPMsg( pPMsg );
			// The playback engine will release the event
		}
	}

	// Move the insert cursor to a new position
	m_lInsertVal = bData1;

	::PostMessage( m_VScrollBar.m_hWnd, WM_APP, WM_APP_INVALIDATEPIANOROLL, 0 );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RecordStepNoteOff

HRESULT CSequenceStrip::RecordStepNoteOff( BYTE bData1 )
{
	// if the velocity is 0, we didn't receive a NOTEON message, so exit
	if (m_bVelocity[bData1] == 0)
	{
		return E_UNEXPECTED;
	}

	long mtStartTime = m_mtStartTime[bData1];

	BYTE bVelocity = m_bVelocity[bData1];

	MUSIC_TIME mtEndTime;

	SNAPTO snapTo = GetSnapToBoundary( mtStartTime );
	mtEndTime = m_lInsertTime + GetSnapAmount( mtStartTime, snapTo );

	while ( mtEndTime < mtStartTime)
	{
		mtEndTime += m_mtLength;
	}

	CSequenceItem* pSeqItem;
	pSeqItem = new CSequenceItem;
	
	if( mtStartTime < 0 )
	{
		// Set the time of the note to 0
		pSeqItem->m_mtTime = 0;

		// Find out the TimeSig of the first measure
		DMUS_TIMESIGNATURE dmTimeSig;
		GetTimeSig( 0, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );

		// Compute the smallest possible offset before time 0
		const long lNegativeMeasureLength = max( SHRT_MIN, -dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat) );

		// Set the note's offset
		pSeqItem->m_nOffset = short( max( mtStartTime, lNegativeMeasureLength) );
	}
	else
	{
		pSeqItem->m_mtTime = mtStartTime;
		pSeqItem->m_nOffset = 0;
	}
	pSeqItem->m_mtDuration = max( mtEndTime - mtStartTime - 1, GetGridClocks(pSeqItem->AbsTime()) / 2 );
	pSeqItem->m_bStatus = MIDI_NOTEON;
	pSeqItem->m_bByte1 = bData1;
	pSeqItem->m_bByte2 = bVelocity;
	pSeqItem->m_fSelected = FALSE;
	
	ASSERT( pSeqItem->m_mtDuration != 0 );

	m_pSequenceMgr->InsertNote( pSeqItem, FALSE );

	m_bVelocity[bData1] = 0;
	m_mtStartTime[bData1] = 0;

	BOOL fNotesOn = FALSE;
	for (int i=0; i < 128; i++)
	{
		if (m_mtStartTime[i] != 0 || m_bVelocity[i] != 0)
		{
			fNotesOn = TRUE;
			break;
		}
	}

	// If all notes are off, increment the time cursor by one grid and update the pattern editor
	if (!fNotesOn)
	{
		m_nLastEdit = IDS_UNDO_INSERT;
		CoInitialize( NULL );
		m_pSequenceMgr->OnDataChanged();
		CoUninitialize();
		// Handled by OnDataChanged
		//m_pSequenceMgr->m_fNoteInserted = FALSE;

		// Bump to the right (this eventually causes BumpTimeCursor() to be
		// called in a message handler thread
		::PostMessage( m_VScrollBar.m_hWnd, WM_APP, WM_APP_BUMPTIMECURSOR, 0 );
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RecordRealTimeNoteOn

HRESULT CSequenceStrip::RecordRealTimeNoteOn(REFERENCE_TIME rtTime, BYTE bData1, BYTE bData2 )
{
	if (m_bVelocity[bData1] != 0)
	{
		return E_UNEXPECTED;
	}

	if( m_pSequenceMgr->m_pSegmentState )
	{
		MUSIC_TIME mtTime;
		m_pSequenceMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );

		//TRACE("Start time: %d\n",mtTime);
		m_mtStartTime[bData1] = mtTime;

		if (bData2 > 0)
		{
			m_bVelocity[bData1] = bData2;
		}
		else
		{
			m_bVelocity[bData1] = 127;
		}
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RecordRealTimeNoteOff

HRESULT CSequenceStrip::RecordRealTimeNoteOff(REFERENCE_TIME rtTime, BYTE bData1 )
{
	// if the velocity is 0, we didn't receive a NOTEON message, so exit
	if (m_bVelocity[bData1] == 0)
	{
		//TRACE("Oops: velocity 0\n");
		return E_UNEXPECTED;
	}

	MUSIC_TIME mtTime;
	m_pSequenceMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );

	CSequenceItem* pSeqItem;
	pSeqItem = new CSequenceItem;

	pSeqItem->m_mtDuration = mtTime - m_mtStartTime[bData1];

	mtTime = m_mtStartTime[bData1];

	// Convert mtTime from absolute time to an offset from when the segment started playing
	mtTime = GetTimeOffset( mtTime, m_pSequenceMgr->m_mtCurrentStartTime, m_pSequenceMgr->m_mtCurrentStartPoint,
				m_pSequenceMgr->m_mtCurrentLoopStart, m_pSequenceMgr->m_mtCurrentLoopEnd, m_mtLength,
				m_pSequenceMgr->m_dwCurrentMaxLoopRepeats );

	if( mtTime < 0 )
	{
		// Set the time of the note to 0
		pSeqItem->m_mtTime = 0;

		// Find out the TimeSig of the first measure
		DMUS_TIMESIGNATURE dmTimeSig;
		GetTimeSig( 0, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );

		// Compute the smallest possible offset before time 0
		const long lNegativeMeasureLength = max( SHRT_MIN, -dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat) );

		// Set the note's offset
		pSeqItem->m_nOffset = short ( max( mtTime, lNegativeMeasureLength) );
	}
	else
	{
		pSeqItem->m_mtTime = mtTime;
		pSeqItem->m_nOffset = 0;
	}
	pSeqItem->m_bByte2 = m_bVelocity[bData1];
	pSeqItem->m_bByte1 = bData1;
	pSeqItem->m_bStatus = MIDI_NOTEON;
	pSeqItem->m_fSelected = FALSE;

	if (pSeqItem->m_mtDuration == 0)
	{
		//TRACE("Duration == 0 at %d\n", ::timeGetTime());
		// BUGBUG: Need to replace with a better value
		pSeqItem->m_mtDuration = 768 / 4;
	}
	m_pSequenceMgr->InsertNote( pSeqItem, FALSE );

	m_bVelocity[bData1] = 0;
	m_mtStartTime[bData1] = 0;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RecordRealTimeCurve

HRESULT CSequenceStrip::RecordRealTimeCurve( REFERENCE_TIME rtTime, BYTE bStatus, BYTE bData1, BYTE bData2 )
{
	UNREFERENCED_PARAMETER(bStatus);
	UNREFERENCED_PARAMETER(bData1);
	UNREFERENCED_PARAMETER(bData2);

	// Get the MUSIC_TIME
	MUSIC_TIME mtTime;
	m_pSequenceMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );

	// Create the Curve
	CCurveItem* pDMCurve = new CCurveItem;
	if( pDMCurve )
	{
		switch( (int)(bStatus & 0xF0) )
		{
			case MIDI_PBEND:
				pDMCurve->m_bType = DMUS_CURVET_PBCURVE;
				pDMCurve->m_nStartValue = short(((bData2 & 0x7F) << 7) + (bData1 & 0x7F));
				break;

			case MIDI_CCHANGE:
				pDMCurve->m_bType = DMUS_CURVET_CCCURVE;
				pDMCurve->m_bCCData = bData1;
				pDMCurve->m_nStartValue = bData2;
				break;

			case MIDI_PTOUCH:
				pDMCurve->m_bType = DMUS_CURVET_PATCURVE;
				pDMCurve->m_bCCData = bData1;
				pDMCurve->m_nStartValue = bData2;
				break;

			case MIDI_MTOUCH:
				pDMCurve->m_bType = DMUS_CURVET_MATCURVE;
				pDMCurve->m_nStartValue = bData1;
				break;
		}

		// Convert mtTime from absolute time to an offset from when the segment started playing
		mtTime -= m_pSequenceMgr->m_mtCurrentStartTime - m_pSequenceMgr->m_mtCurrentStartPoint;

		// If mtLoopEnd is non zero, set lLoopEnd to mtLoopEnd, otherwise use the segment length
		long lLoopEnd;
		lLoopEnd = m_pSequenceMgr->m_mtCurrentLoopEnd ? m_pSequenceMgr->m_mtCurrentLoopEnd : m_mtLength;

		// Subtract off the loops if we started before the loop end time
		// It should be '<=' - if the start point is the loop end point, it will loop.
		if( (lLoopEnd != m_pSequenceMgr->m_mtCurrentLoopStart) &&
			(m_pSequenceMgr->m_mtCurrentStartPoint <= lLoopEnd) )
		{

			// Subtract off time for the repeats already finished
			mtTime -= m_pSequenceMgr->m_dwCurrentLoopRepeats * (lLoopEnd - m_pSequenceMgr->m_mtCurrentLoopStart);

			// If we're beyond the loop end and there are loops left, subtract off
			// another loop and increment our current loops count
			if( (m_pSequenceMgr->m_dwCurrentLoopRepeats < m_pSequenceMgr->m_dwCurrentMaxLoopRepeats) &&
				(mtTime >= lLoopEnd) )
			{
				int nRepeats;
				nRepeats = (mtTime - m_pSequenceMgr->m_mtCurrentLoopStart) /
					(lLoopEnd - m_pSequenceMgr->m_mtCurrentLoopStart);
				m_pSequenceMgr->m_dwCurrentLoopRepeats += nRepeats;
				mtTime -= nRepeats * (lLoopEnd - m_pSequenceMgr->m_mtCurrentLoopStart);
			}
		}

		if( mtTime < 0 )
		{
			pDMCurve->m_mtTime = 0;
			pDMCurve->m_nOffset = short ( max( mtTime, SHRT_MIN) );
		}
		else
		{
			pDMCurve->m_mtTime = mtTime;
			pDMCurve->m_nOffset = 0;
		}

		pDMCurve->m_mtDuration = 1;
		pDMCurve->m_bCurveShape = DMUS_CURVES_INSTANT;
		pDMCurve->m_nEndValue = pDMCurve->m_nStartValue;

		pDMCurve->SetDefaultResetValues( m_mtLength );

		// Make sure a corresponding Curve strip exists
		BYTE bCCType = CurveTypeToStripCCType( pDMCurve ); 
		if( m_CurveStripView == SV_MINIMIZED )
		{
			GetCurveStripState( bCCType, pDMCurve->m_wParamType );
		}
		else
		{
			AddCurveStrip( bCCType, pDMCurve->m_wParamType );
		}

		// Place curve in Sequence's event list
		m_pSequenceMgr->InsertCurve( pDMCurve, FALSE );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::FloorTimeToGrid

MUSIC_TIME CSequenceStrip::FloorTimeToGrid( MUSIC_TIME mtTime, long *plGridClocks ) const
{
	ASSERT( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline );

	// Find out which measure we're in
	long lMeasure;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, mtTime, &lMeasure, NULL ) ) )
	{
		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, NULL, &ts ) ) )
			{
				long lBeat, lGrid;
				// Compute the number of clocks in a beat and a grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// BUGBUG: This doesn't work correctly if lGridClocks * wGridsPerBeat != lBeatClocks
				// Any notes in the second half of the last grid in a beat will have the WRONG tick
				// (since there are more ticks in the last grid of the beat than in the other grids).

				// Convert mtTime into an offset from the start of this measure
				mtTime -= lTime;

				lBeat = mtTime / lBeatClocks;
				lGrid = (mtTime % lBeatClocks) / lGridClocks;

				if( plGridClocks )
				{
					*plGridClocks = lGridClocks;
				}

				return lTime + (lBeat * lBeatClocks) + (lGrid * lGridClocks);
			}
		}
	}

	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CurveStripExists
	
BOOL CSequenceStrip::CurveStripExists( void *pThis, BYTE bCCType, WORD wRPNType )
{
	ASSERT( pThis );
	if( pThis == NULL )
	{
		return FALSE;
	}

	// Always return FALSE for generic RPN and NRPN strips
	if( (wRPNType == 0xFFFF)
	&&	((bCCType == CCTYPE_RPN_CURVE_STRIP) || (bCCType == CCTYPE_NRPN_CURVE_STRIP)) )
	{
		return FALSE;
	}

	CSequenceStrip *pSequenceStrip = static_cast<CSequenceStrip *>(pThis);

	ioCurveStripState* pCurveStripState;

	// Use CurveStripState list instead of CurveStrip list
	// because it contains accurate info whether or not
	// CurveStrip(s) are minimized
	POSITION pos = pSequenceStrip->m_lstCurveStripStates.GetHeadPosition();
	while( pos )
	{
		pCurveStripState = pSequenceStrip->m_lstCurveStripStates.GetNext( pos );

		if( pCurveStripState->m_bCCType == bCCType )
		{
			if( (wRPNType != 0xFFFF)
			&&	((bCCType == CCTYPE_RPN_CURVE_STRIP) || (bCCType == CCTYPE_NRPN_CURVE_STRIP)) )
			{
				if( pCurveStripState->m_wRPNType == wRPNType )
				{
					return TRUE;
				}
			}
			else
			{
				// CurveStrip already exists
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CurveTypeToStripCCType

BYTE CSequenceStrip::CurveTypeToStripCCType( CCurveItem* pDMCurve )
{
	return ::CurveTypeToStripCCType( pDMCurve->m_bType, pDMCurve->m_bCCData );
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SyncCurveStripStateList

void CSequenceStrip::SyncCurveStripStateList( void )
{
	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		GetCurveStripState( CurveTypeToStripCCType( pDMCurve ), pDMCurve->m_wParamType );
	}

	if( m_lstCurveStripStates.IsEmpty() )
	{
		m_CurveStripView = SV_MINIMIZED;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetCurveStripState

ioCurveStripState* CSequenceStrip::GetCurveStripState( BYTE bCCType, WORD wRPNType )
{
	ioCurveStripState* pNewCurveStripState;
	ioCurveStripState* pCurveStripState;

	// Minimized CurveStrips do not store state information
	if( bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		return NULL;
	}

	// See if CurveStripState info already exists
	POSITION pos = m_lstCurveStripStates.GetHeadPosition();
	while( pos )
	{
		pCurveStripState = m_lstCurveStripStates.GetNext( pos );

		if( pCurveStripState->m_bCCType == bCCType )
		{
			// Check if this is an RPN or an NRPN curve
			if( (pCurveStripState->m_bCCType == CCTYPE_NRPN_CURVE_STRIP)
			||	(pCurveStripState->m_bCCType == CCTYPE_RPN_CURVE_STRIP) )
			{
				// RPN/NRPN curve - check m_wRPNType also
				if( pCurveStripState->m_wRPNType == wRPNType )
				{
					// CurveStripState already exists
					return pCurveStripState;
				}
			}
			else
			{
				// CurveStripState already exists
				return pCurveStripState;
			}
		}
	}

	// Create a new CurveStripState struct	
	pNewCurveStripState = new ioCurveStripState;
	
	if( pNewCurveStripState )
	{
		pNewCurveStripState->m_nStripHeight = CRV_DEFAULT_HEIGHT;
		pNewCurveStripState->m_bCCType = bCCType;
		pNewCurveStripState->m_bPad = 0;
		if( (bCCType == CCTYPE_NRPN_CURVE_STRIP)
		||	(bCCType == CCTYPE_RPN_CURVE_STRIP) )
		{
			pNewCurveStripState->m_wRPNType = wRPNType;
		}
		else
		{
			pNewCurveStripState->m_wRPNType = 0;
		}

		WORD wCCTypeSortValue = GetCCTypeSortValue( pNewCurveStripState->m_bCCType, pNewCurveStripState->m_wRPNType );
		WORD wCCTypeListSortValue;

		// Add this state info to the list of PianoRoll CurveStripStates
		// Sort on bCCType (ascending)
		POSITION pos = m_lstCurveStripStates.GetHeadPosition();
		POSITION posPrev;

		while( pos )
		{
			posPrev = pos;
			pCurveStripState = m_lstCurveStripStates.GetNext( pos );

			wCCTypeListSortValue = GetCCTypeSortValue( pCurveStripState->m_bCCType, pCurveStripState->m_wRPNType );
			if( wCCTypeListSortValue > wCCTypeSortValue )
			{
				m_lstCurveStripStates.InsertBefore( posPrev, pNewCurveStripState );
				return pNewCurveStripState; 
			}
		}

		m_lstCurveStripStates.AddTail( pNewCurveStripState );
		return pNewCurveStripState; 
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetCurveStrip
	
CCurveStrip* CSequenceStrip::GetCurveStrip( BYTE bCCType, WORD wRPNType )
{
    POSITION pos = m_lstCurveStrips.GetHeadPosition();
    while( pos )
    {
		CCurveStrip* pCurveStrip = m_lstCurveStrips.GetNext( pos );

		if( pCurveStrip->m_bCCType == bCCType )
		{
			if( (pCurveStrip->m_bCCType == CCTYPE_NRPN_CURVE_STRIP)
			||	(pCurveStrip->m_bCCType == CCTYPE_RPN_CURVE_STRIP) )
			{
				if( pCurveStrip->m_wRPNType == wRPNType )
				{
					return pCurveStrip;
				}
			}
			else
			{
				return pCurveStrip;
			}
		}
    }

	return NULL; 
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::AddCurveStrip
	
HRESULT CSequenceStrip::AddCurveStrip( BYTE bCCType, WORD wRPNType )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	// Make sure Curve Strip does not already exist
	CCurveStrip* pCurveStrip = GetCurveStrip( bCCType, wRPNType );
	if( pCurveStrip )
	{
		// Curve Strip already exists
		return S_OK;
	}
	
	// Create the Curve strip
	pCurveStrip = new CCurveStrip( m_pSequenceMgr, this, bCCType, wRPNType );
	if( pCurveStrip )
	{
		ioCurveStripState* pTheCurveStripState = NULL;

		// Get/create state info struct for maximized Curve strips
		if( bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
		{
			pTheCurveStripState = GetCurveStripState( bCCType, wRPNType );

			if( pTheCurveStripState == NULL )
			{
				pCurveStrip->Release();
				return E_FAIL;
			}
		}

		// Add this strip to the list of PianoRoll CurveStrips
		m_lstCurveStrips.AddTail( pCurveStrip );
	
		// Determine proper position for the new CurveStrip
		DWORD dwPosition = DeterminePositionForCurveStrip( bCCType, wRPNType );

		// Add Curve Strip to the Timeline
		m_pSequenceMgr->m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pCurveStrip, dwPosition );

		VARIANT var;
		
		// Set Curve Strip StripView
		if( m_CurveStripView == SV_MINIMIZED )
		{
			ASSERT( bCCType == CCTYPE_MINIMIZED_CURVE_STRIP );

			var.vt = VT_I4;
			V_I4(&var) = m_CurveStripView;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_STRIPVIEW, var );
		}

		if( bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
		{
			// Set Curve Strip Height
			var.vt = VT_I4;
			V_I4(&var) = pTheCurveStripState->m_nStripHeight;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_HEIGHT, var );
		}

		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::AddCurveStrips
	
HRESULT CSequenceStrip::AddCurveStrips( void )
{
	HRESULT hr = S_OK;

	// Only add curve strips when PianoRoll is not minimized
	if( m_svView == SV_NORMAL )
	{
		if( m_CurveStripView == SV_MINIMIZED )
		{
			hr = AddCurveStrip( CCTYPE_MINIMIZED_CURVE_STRIP, 0 );
		}
		else
		{
			ioCurveStripState* pCurveStripState;

			POSITION pos = m_lstCurveStripStates.GetHeadPosition();
			while( pos )
			{
				pCurveStripState = m_lstCurveStripStates.GetNext( pos );

				// There should not be a CurveStripState for a minimized CurveStrip!
				ASSERT( pCurveStripState->m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP );

				hr = AddCurveStrip( pCurveStripState->m_bCCType, pCurveStripState->m_wRPNType );
				if( FAILED ( hr ) )
				{
					break;
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::RemoveCurveStrip
	
HRESULT CSequenceStrip::RemoveCurveStrip( CCurveStrip* pCurveStrip )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	// Remove from list of Curve Strips
	POSITION pos = m_lstCurveStrips.Find( pCurveStrip );
	if( pos )
	{
		m_lstCurveStrips.RemoveAt( pos );

		// If not doing gutter selection, unselect all curves
		if( !m_bGutterSelected )
		{
			pCurveStrip->SelectAllCurves( FALSE );
		}

		pCurveStrip->Release();
	}

	// Remove Curve Strip from the Timeline
	m_pSequenceMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnDeleteCurveStrip
	
HRESULT CSequenceStrip::OnDeleteCurveStrip( CCurveStrip* pCurveStrip )
{
	ASSERT( m_pSequenceMgr != NULL );

	ioCurveStripState* pCurveStripState;

	// Remove from list of Curve Strip states
	POSITION pos = m_lstCurveStripStates.GetHeadPosition();
	while( pos )
	{
		pCurveStripState = m_lstCurveStripStates.GetNext( pos );

		if( pCurveStripState->m_bCCType == pCurveStrip->m_bCCType )
		{
			if( (pCurveStripState->m_bCCType == CCTYPE_NRPN_CURVE_STRIP)
			||	(pCurveStripState->m_bCCType == CCTYPE_RPN_CURVE_STRIP) )
			{
				if( pCurveStripState->m_wRPNType == pCurveStrip->m_wRPNType )
				{
					POSITION pos2 = m_lstCurveStripStates.Find( pCurveStripState );
					if( pos2 )
					{
						m_lstCurveStripStates.RemoveAt( pos2 );
						delete pCurveStripState;
					}

					break;
				}
			}
			else
			{
				POSITION pos2 = m_lstCurveStripStates.Find( pCurveStripState );
				if( pos2 )
				{
					m_lstCurveStripStates.RemoveAt( pos2 );
					delete pCurveStripState;
				}

				break;
			}
		}
	}

	// Remove from list of Curve Strips
	pCurveStrip->DeleteAllCurves();
	RemoveCurveStrip( pCurveStrip );

	// Insert minimized CurveStrip if there are no other CurveStrips
	if( m_lstCurveStripStates.IsEmpty() )
	{
		m_CurveStripView = SV_MINIMIZED;
		AddCurveStrips();
	}

	// Let the object know about the changes
	m_nLastEdit = IDS_UNDO_DELETE_CC_TRACK;
	m_pSequenceMgr->OnDataChanged(); 

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnNewCurveStrip
	
HRESULT CSequenceStrip::OnNewCurveStrip( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	HRESULT hr = E_FAIL;

	// Have user select the type of CC strip
	CDialogNewCCTrack dlgNewCCTrack;
	dlgNewCCTrack.m_pfCurveStripExists = CSequenceStrip::CurveStripExists;
	dlgNewCCTrack.m_pVoid = this;
	if( dlgNewCCTrack.DoModal() == IDCANCEL )
	{
		return S_FALSE;
	}

	if( m_CurveStripView == SV_MINIMIZED )
	{
		if( GetCurveStripState( dlgNewCCTrack.m_bCCType, dlgNewCCTrack.m_wRPNType ) )
		{
			// Redraw minimized strip
			InvalidateCurveStrips();

			hr = S_OK;
		}
	}
	else
	{
		// Add the Curve Strip
		hr = AddCurveStrip( dlgNewCCTrack.m_bCCType, dlgNewCCTrack.m_wRPNType );
	}

	if( hr == S_OK )
	{
		// Let the object know about the changes
		m_nLastEdit = IDS_UNDO_ADD_CC_TRACK;
		m_pSequenceMgr->OnDataChanged(); 
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DeterminePositionForCurveStrip
	
DWORD CSequenceStrip::DeterminePositionForCurveStrip( BYTE bCCType, WORD wRPNType)
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	WORD wCCTypeSortValue = GetCCTypeSortValue( bCCType, wRPNType );

	VARIANT var;

	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_POSITION, &var) ) )
	{
		CCurveStrip* pCurveStrip;
		IDMUSProdStrip* pIStrip;
		VARIANT varClsid;
		CLSID clsid;

		DWORD dwPosition = V_I4(&var);

		BOOL fContinue = TRUE;

		while( fContinue  &&  SUCCEEDED( m_pSequenceMgr->m_pTimeline->EnumStrip( ++dwPosition, &pIStrip ) ) )
		{
			varClsid.vt = VT_BYREF;
			V_BYREF(&varClsid) = &clsid;
			fContinue = FALSE;

			// Is this strip a Curve or Sequence strip?
			if( SUCCEEDED ( pIStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) ) )
			{
				if( ::IsEqualCLSID( clsid, CLSID_DirectMusicSeqTrack ) )
				{
					// Is this a curve strip?
					if( SUCCEEDED ( pIStrip->GetStripProperty( SP_CURVESTRIP, &var ) ) )
					{
						if( V_BOOL(&var) == TRUE )
						{
							// Is this curve strip supposed to go before ours?
							pCurveStrip = (CCurveStrip *)pIStrip;

							WORD wCCTypeListSortValue = GetCCTypeSortValue( pCurveStrip->m_bCCType, pCurveStrip->m_wRPNType );
							if( wCCTypeListSortValue <= wCCTypeSortValue )
							{
								fContinue = TRUE;
							}
						}
					}
				}
			}

			pIStrip->Release();
		}

		return dwPosition;
	}

	return 0xFFFFFFFF;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnChangeCurveStripView

void CSequenceStrip::OnChangeCurveStripView( STRIPVIEW svNewStripView )
{
	ASSERT( m_pSequenceMgr != NULL );

	if( m_CurveStripView == svNewStripView )
	{
		return;
	}

	CCurveStrip *pCurveStrip;

	// Don't allow SV_NORMAL when there are no CurveStripStates
	if( svNewStripView == SV_NORMAL
	&&  m_lstCurveStripStates.IsEmpty() )
	{
		m_CurveStripView = SV_MINIMIZED;

		VARIANT var;
		var.vt = VT_I4;
		V_I4(&var) = m_CurveStripView;

		POSITION pos = m_lstCurveStrips.GetHeadPosition();
		while( pos )
		{
			pCurveStrip = m_lstCurveStrips.GetNext( pos );
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_STRIPVIEW, var );
		}

		return;
	}

	// Change Curve StripView field
	m_CurveStripView = svNewStripView;

	// Remove all existing Curve Strips
	while( m_lstCurveStrips.IsEmpty() == FALSE )
	{
		pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.GetHead() );
		RemoveCurveStrip( pCurveStrip );
	}

	// Add new Curve Strips
	AddCurveStrips();

	// Let the object know about the changes
	m_pSequenceMgr->m_fDirty = TRUE;
	if( m_CurveStripView == SV_NORMAL )
	{
		m_nLastEdit = IDS_UNDO_CURVE_MAXIMIZE;
	}
	else // m_CurveStripView == SV_MINIMIZED
	{
		m_nLastEdit = IDS_UNDO_CURVE_MINIMIZE;
	}

	// Fix 27283: Don't store Undo states for Minimize/Maximize events

	// Check if TP_FREEZE_UNDO is NOT set.
	BOOL fFreezeUndo = FALSE;
	VARIANT var;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
	{
		fFreezeUndo = V_BOOL(&var);
	}

	if( !fFreezeUndo )
	{
		// Need to set TP_FREEZE_UNDO or the segment will add an undo state for us
		var.vt = VT_BOOL;
		V_BOOL(&var) = TRUE;
		m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
	}

	// No need to update performance engine
	m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
	m_pSequenceMgr->OnDataChanged(); 

	if( !fFreezeUndo )
	{
		// Need to reset TP_FREEZE_UNDO or the segment will add an undo state for us
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::InvalidateCurveStrips

void CSequenceStrip::InvalidateCurveStrips( void )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	CCurveStrip* pCurveStrip;

    POSITION pos = m_lstCurveStrips.GetHeadPosition();
    while( pos )
    {
        pCurveStrip = m_lstCurveStrips.GetNext( pos );
		m_pSequenceMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, TRUE );
		pCurveStrip->RefreshCurvePropertyPage();
    }
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SaveCurveStripStateData

HRESULT CSequenceStrip::SaveCurveStripStateData( IDMUSProdRIFFStream* pIRiffStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	POSITION pos;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}
	
	// Write global CurveStripState information
	{
		ioGlobalCurveStripState oGlobalCurveStripState;

		// Write global CurveStripState chunk header
		ck.ckid = DMUS_FOURCC_ALLCURVES_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioGlobalCurveStripState structure
		memset( &oGlobalCurveStripState, 0, sizeof(ioGlobalCurveStripState) );
		
		oGlobalCurveStripState.m_nCurveStripView = m_CurveStripView;

		// Write global CurveStripState chunk data
		hr = pIStream->Write( &oGlobalCurveStripState, sizeof(ioGlobalCurveStripState), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioGlobalCurveStripState) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	ioCurveStripState* pCurveStripState;

	// Write CurveStripState information for each CurveStrip
	pos = m_lstCurveStripStates.GetHeadPosition();
	while( pos != NULL )
	{
		pCurveStripState = m_lstCurveStripStates.GetNext( pos );

		// Write CurveStripState chunk header
		ck.ckid = DMUS_FOURCC_CURVE_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write CurveStripState chunk data
		hr = pIStream->Write( pCurveStripState, sizeof(ioCurveStripState), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioCurveStripState) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	
	}

ON_ERROR:
	pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateCurveStripGutterSelection

void CSequenceStrip::UpdateCurveStripGutterSelection( BOOL fChanged )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	CCurveStrip* pCurveStrip;

    POSITION pos = m_lstCurveStrips.GetHeadPosition();
    while( pos )
    {
        pCurveStrip = m_lstCurveStrips.GetNext( pos );

		pCurveStrip->OnGutterSelectionChange( fChanged );
    }
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SelectEventsBetweenTimes

BOOL CSequenceStrip::SelectEventsBetweenTimes( long lStart, long lEnd )
{
	BOOL fChange = FALSE;

	if( lEnd < lStart )
	{
		long lTmp = lStart;
		lStart = lEnd;
		lEnd = lTmp;
	}

	// m_pActiveNote will be updated by the method UpdateSelectionState, called
	// if any of the item's selection state changes
	POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem *pSeqItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		if( ( lStart <= pSeqItem->AbsTime() ) &&
			( lEnd >= pSeqItem->AbsTime() ) )
		{
			if( !pSeqItem->m_fSelected )
			{
				pSeqItem->m_fSelected = TRUE;
				fChange = TRUE;
			}
		}
		else if( pSeqItem->m_fSelected )
		{
			pSeqItem->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	if( fChange )
	{
		UpdateSelectionState();
	}

	pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pCurveItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );

		// Only update curve item if its strip was clicked on
		if( (m_bSelectionCC != CurveTypeToStripCCType(pCurveItem))
		||	(m_wSelectionParamType != pCurveItem->m_wParamType) )
		{
			if( ( lStart <= pCurveItem->AbsTime() ) &&
				( lEnd >= pCurveItem->AbsTime() ) )
			{
				if( !pCurveItem->m_fSelected )
				{
					pCurveItem->m_fSelected = TRUE;
					fChange = TRUE;
				}
			}
			else if( pCurveItem->m_fSelected )
			{
				pCurveItem->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
	}

	return fChange;
}

int CSequenceStrip::PositionToMIDIValue( long lYPos )
{
	if( m_fHybridNotation )
	{
		// Convert from ypos to a scale position (with B10 as 0)
		long lValue = 1 + ((2 * (lYPos + m_lMaxNoteHeight / 4)) / m_lMaxNoteHeight);

		// Convert from a scale position to a MIDI value
		return 120 - (lValue / 7) * 12 + aScaleToChromNat[6 - (lValue % 7)] + m_aiScalePattern[6 - (lValue % 7)];
	}
	else
	{
		return 127 - (lYPos / m_lMaxNoteHeight);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetFirstVisibleNote

POSITION CSequenceStrip::GetFirstVisibleNote( long lStartTime, long lEndTime, int nTopNote, int nBottomNote )
{
	POSITION pos2, pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		const CSequenceItem *pSequenceItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		// Check vertical range, then horizontal range
		if ( (pSequenceItem->m_bByte1 <= nTopNote)
		&&	 (pSequenceItem->m_bByte1 >= nBottomNote)
		&&	 ((pSequenceItem->AbsTime() + pSequenceItem->m_mtDuration > lStartTime) ||
			  ((pSequenceItem->AbsTime() < 0) && (pSequenceItem->m_mtDuration > lStartTime)))
		&&	 (pSequenceItem->AbsTime() < lEndTime) )
		{
			return pos2;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::InitializeScaleAccidentals

void CSequenceStrip::InitializeScaleAccidentals( void )
{
	for( int i=0; i < 10; i++ )
	{
		memcpy( &(m_aiAccidentals[i * 7]), m_aiScalePattern, sizeof(int) * 7);
	}

	memcpy( &(m_aiAccidentals[70]), m_aiScalePattern, sizeof(int) * 5);
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateKeyPattern

void CSequenceStrip::UpdateKeyPattern( void )
{
	ZeroMemory( m_aiScalePattern, sizeof(int) * 7 );
	if( m_pSequenceMgr->m_fDisplayingFlats )
	{
		// Set the # of flats
		for( int i=0; i < m_pSequenceMgr->m_nNumAccidentals; i++ )
		{
			// Cirlce of fourths
			m_aiScalePattern[(6 + i * 3) % 7] = -1;
		}

		// Set the Key Root to the chosen accidental
		// (Say they want 3b with key of E - then this resets Eb back to Enat)
		m_aiScalePattern[aChromToScaleFlats[m_pSequenceMgr->m_nKeyRoot]] = aChromToFlatAccidentals[m_pSequenceMgr->m_nKeyRoot];
	}
	else
	{
		// Set the # of sharps
		for( int i=0; i < m_pSequenceMgr->m_nNumAccidentals; i++ )
		{
			// Cirlce of fifths
			m_aiScalePattern[(3 + i * 4) % 7] = 1;
		}

		// Set the Key Root to the chosen accidental
		// (Say they want 2# with key of F - then this resets F# back to Fnat)
		m_aiScalePattern[aChromToScaleSharps[m_pSequenceMgr->m_nKeyRoot]] = aChromToSharpAccidentals[m_pSequenceMgr->m_nKeyRoot];
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::InvalidateFunctionBar

void CSequenceStrip::InvalidateFunctionBar( void )
{
	if ( m_svView == SV_MINIMIZED )
	{
		InvalidateStrip();
		return;
	}

	// Invalidate the function bar
	VARIANT var;
	var.vt = VT_I4;
	if (FAILED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )))
	{
		InvalidateStrip();
		return;
	}

	RECT rect;
	rect.right = 0;
	rect.top = m_lVScroll;
	rect.bottom = m_lVScroll + V_I4(&var);
	if( FAILED(m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var )) )
	{
		InvalidateStrip();
		return;
	}
	rect.left = -V_I4(&var);

	long lLeftDisplay;
	m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	rect.left  += lLeftDisplay;
	rect.right += lLeftDisplay;

	// Really invalidate the function bar
	m_pSequenceMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::InvalidatePianoRoll

void CSequenceStrip::InvalidatePianoRoll( void )
{
	if ( m_svView == SV_MINIMIZED )
	{
		InvalidateStrip();
		return;
	}

	// Invalidate the piano roll
	RECT rect;
	long lLeftDisplay;
	m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	rect.left = lLeftDisplay;
	rect.top = m_lVScroll;
	
	// Find the right and bottom boundaries of our strip					
	CDC cDC;
	VARIANT var;
	var.vt = VT_I4;
	if( FAILED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_GET_HDC, &var )) )
	{
		return;
	}
	CWnd *pWnd = NULL;
	if( cDC.Attach( (HDC)(V_I4(&var)) ) != 0 )
	{
		RECT	rtStrip;
		
		cDC.GetClipBox( &rtStrip );
		rect.right = lLeftDisplay + rtStrip.right + 1;
		pWnd = cDC.GetWindow();
		cDC.Detach();
	}
	if( pWnd )
	{
		::ReleaseDC( pWnd->GetSafeHwnd(), (HDC)(V_I4(&var)) );
	}
	else
	{
		::ReleaseDC( NULL, (HDC)(V_I4(&var)) );
	}

	if (SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )))
	{
		rect.bottom = m_lVScroll + V_I4(&var);
	}

	// Really invalidate the piano roll
	// Don't need to erase, since the horizontal bars will overwrite the area
	m_pSequenceMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::InvalidateStrip

void CSequenceStrip::InvalidateStrip( void )
{
	m_pSequenceMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DrawSymbol

void CSequenceStrip::DrawSymbol( HDC hDC, const TCHAR *pstrText, int iHybridPos, long lRightPos, long lTopAdjust, long lBottomAdjust )
{
	// Get position to draw at
	RECT rectNote;
	GetHybridRect( &rectNote, iHybridPos, 127, 0, 0 );
	rectNote.top -= lTopAdjust;
	rectNote.bottom += lBottomAdjust;
	rectNote.left = lRightPos - (m_lMaxNoteHeight * 3) / 2;
	rectNote.right = lRightPos;

	// Draw sharp in treble clef
	::DrawText( hDC, pstrText, 1, &rectNote, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::AdjustScroll

void CSequenceStrip::AdjustScroll(long lXPos, long lYPos)
{
	VARIANT var;
	long lHeight = 0;
	if (SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_HEIGHT, &var)))
	{
		lHeight = V_I4(&var);
	}

	long lMaxHeight;
	if( m_fHybridNotation )
	{
		lMaxHeight = 38 * m_lMaxNoteHeight;
	}
	else
	{
		lMaxHeight = 128 * m_lMaxNoteHeight;
	}

	if ((lYPos < m_lVScroll) && (m_lVScroll > 0))
	{
		// Start Scroll up
		EnableTimer();
	}
	else if ( (lHeight > 0) && (lYPos > m_lVScroll + lHeight) &&
			  (m_lVScroll < lMaxHeight) )
	{
		// Start Scroll down
		EnableTimer();
	}
	else
	{
		// Check horizontal ranges

		// Get current scroll position
		long lHScroll = 0;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			lHScroll = V_I4(&var);
		}

		if ((lXPos < lHScroll) && (lHScroll > 0))
		{
			// Start Scroll left
			EnableTimer();
		}
		else
		{
			// Get rectangle defining strip position
			var.vt = VT_BYREF;
			RECT rectStrip;
			rectStrip.left = 0;
			rectStrip.right = 0;
			V_BYREF(&var) = &rectStrip;
			m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_STRIP_RECT, &var);

			// Compute the right side of the display
			long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

			// Compute the maximum scroll position
			long lMaxHScroll = 0;
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_mtLength, &lMaxHScroll );

			lMaxHScroll -= rectStrip.right - rectStrip.left;

			// Check for need to scroll right
			if( (lHScroll < lMaxHScroll) && (lXPos > lMaxScreenPos) )
			{
				// Start Scroll right
				EnableTimer();
			}
			else
			{
				// Mouse withing screen position - disable timer
				KillTimer();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnTimer

void CSequenceStrip::OnTimer( void )
{
	POINT point;
	if( ::GetCursorPos( &point ) && SUCCEEDED(m_pSequenceMgr->m_pTimeline->ScreenToStripPosition((IDMUSProdStrip *)this, &point)) )
	{
		// SEQ_MM_ACTIVERESIZE_END also?
		if( (m_MouseMode == SEQ_MM_ACTIVEMOVE)
		||	(m_MouseMode == SEQ_MM_ACTIVERESIZE_END)
		||	(m_MouseMode == SEQ_MM_ACTIVERESIZE_START) )
		{
			OnMouseMove( point.x, point.y );
		}

		// Get the height of the strip
		VARIANT var;
		long lHeight = 0;
		if (SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_HEIGHT, &var)))
		{
			lHeight = V_I4(&var);
		}

		// Calculate the maximum scroll position
		long lMaxVScroll;
		if( m_fHybridNotation )
		{
			lMaxVScroll = 38 * m_lMaxNoteHeight;
		}
		else
		{
			lMaxVScroll = 128 * m_lMaxNoteHeight;
		}

		if ((point.y < m_lVScroll) && (m_lVScroll > 0))
		{
			// Scroll up
			SetVScroll( max( 0, m_lVScroll - SCROLL_VERT_AMOUNT * ((SCROLL_VERT_RANGE + m_lVScroll - point.y) / SCROLL_VERT_RANGE) ) );
		}
		else if ( (lHeight > 0) && (point.y > m_lVScroll + lHeight) &&
				  (m_lVScroll < lMaxVScroll) )
		{
			// Scroll down
			SetVScroll( min( lMaxVScroll, m_lVScroll + SCROLL_VERT_AMOUNT * ((SCROLL_VERT_RANGE + point.y - m_lVScroll - lHeight) / SCROLL_VERT_RANGE) ) );
		}

		// Update horizontal scroll, if necessary
		long lHScroll = 0;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			lHScroll = V_I4(&var);
		}

		// Check for need to scroll left
		if( (lHScroll > 0) && (point.x < lHScroll) )
		{
			// Scroll left
			var.vt = VT_I4;
			V_I4(&var) = max( lHScroll - SCROLL_HORIZ_AMOUNT * ((SCROLL_HORIZ_RANGE + lHScroll - point.x) / SCROLL_HORIZ_RANGE), 0 );
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

			// No more checks necessary - return
			return;
		}

		// Get rectangle defining strip position
		var.vt = VT_BYREF;
		RECT rectStrip;
		V_BYREF(&var) = &rectStrip;
		if ( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
		{
			// Compute the right side of the display
			long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

			// Compute the maximum scroll position
			long lMaxHScroll;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_mtLength, &lMaxHScroll ) ) )
			{
				lMaxHScroll -= rectStrip.right - rectStrip.left;

				// Check for need to scroll right
				if( (lHScroll < lMaxHScroll) && (point.x > lMaxScreenPos) )
				{
					// Scroll right
					var.vt = VT_I4;
					V_I4(&var) = min( lHScroll + SCROLL_HORIZ_AMOUNT * ((SCROLL_HORIZ_RANGE + point.x - lMaxScreenPos) / SCROLL_HORIZ_RANGE), lMaxHScroll);
					m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

					// No more checks necessary - return
					return;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::KillTimer

void CSequenceStrip::KillTimer( void )
{
	if( m_fScrollTimerActive )
	{
		HWND hwnd = GetTimelineHWnd();
		if( hwnd )
		{
			::KillTimer(hwnd, 1);
			m_fScrollTimerActive = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::EnableTimer

void CSequenceStrip::EnableTimer( void )
{
	if( !m_fScrollTimerActive )
	{
		HWND hwnd = GetTimelineHWnd();
		if( hwnd )
		{
			::SetTimer(hwnd, 1, 100, NULL);
			m_fScrollTimerActive = TRUE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::ApplyUIChunk

void CSequenceStrip::ApplyUIChunk( const ioSeqStripDesign *pSeqStripDesign )
{
	ASSERT( pSeqStripDesign );
	if( !pSeqStripDesign )
	{
		return;
	}

	BOOL fChanged = FALSE;
	if ( pSeqStripDesign->m_crUnselectedNoteColor != m_crUnselectedNoteColor )
	{
		fChanged = TRUE;
		m_crUnselectedNoteColor = pSeqStripDesign->m_crUnselectedNoteColor;
	}
	if ( pSeqStripDesign->m_crSelectedNoteColor != m_crSelectedNoteColor )
	{
		fChanged = TRUE;
		m_crSelectedNoteColor = pSeqStripDesign->m_crSelectedNoteColor;
	}
	if ( pSeqStripDesign->m_crOverlappingNoteColor != m_crOverlappingNoteColor )
	{
		fChanged = TRUE;
		m_crOverlappingNoteColor = pSeqStripDesign->m_crOverlappingNoteColor;
	}
	if ( pSeqStripDesign->m_crAccidentalColor != m_crAccidentalColor )
	{
		fChanged = TRUE;
		m_crAccidentalColor = pSeqStripDesign->m_crAccidentalColor;
	}

	if ( pSeqStripDesign->m_fHybridNotation != m_fHybridNotation )
	{
		fChanged = TRUE;
		m_fHybridNotation = pSeqStripDesign->m_fHybridNotation;
	}
	if ( pSeqStripDesign->m_dblVerticalZoom != m_dblVerticalZoom )
	{
		fChanged = TRUE;
		m_dblVerticalZoom = pSeqStripDesign->m_dblVerticalZoom;
		m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
	}
	if ( (pSeqStripDesign->m_lVScroll >= 0) &&
		 (pSeqStripDesign->m_lVScroll != m_lVScroll) )
	{
		fChanged = TRUE;
		SetVScroll( pSeqStripDesign->m_lVScroll );
	}

	VARIANT var;
	m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_HEIGHT, &var );
	ASSERT(	var.vt == VT_I4 );
	if ( pSeqStripDesign->m_lHeight != V_I4(&var) )
	{
		fChanged = TRUE;
		var.vt = VT_I4;
		V_I4(&var) = pSeqStripDesign->m_lHeight;
		m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_HEIGHT, var );

		// Resize the vertical scroll bar
		OnSize();
	}
	m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIPVIEW, &var );
	ASSERT(	var.vt == VT_I4 );
	if ( pSeqStripDesign->m_svView != m_svView ||
		 pSeqStripDesign->m_svView != V_I4(&var) )
	{
		fChanged = TRUE;
		m_svView = pSeqStripDesign->m_svView;

		// Tell the Timeline what our stripview currently is
		var.vt = VT_I4;
		V_I4(&var) = pSeqStripDesign->m_svView;
		m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIPVIEW, var );
	}

	// Show/Hide the scrollbar appropriately
	if( m_svView == SV_NORMAL )
	{
		m_VScrollBar.ShowWindow(TRUE);
	}
	else // m_svView == SV_MINIMIZED
	{
		m_VScrollBar.ShowWindow(FALSE);
	}

	// Set number of extension bars
	if( m_dwExtraBars != pSeqStripDesign->m_dwExtraBars )
	{
		m_dwExtraBars = pSeqStripDesign->m_dwExtraBars;
		fChanged = TRUE;
	}

	// Set whether or not to display pickup bar
	if( m_fPickupBar != pSeqStripDesign->m_fPickupBar )
	{
		m_fPickupBar = pSeqStripDesign->m_fPickupBar;
		fChanged = TRUE;
	}

	// Set scroll bar range, page size, and screen position
	UpdateVScroll();

	// Update name (needs to happen here so the correct name is displayed when the
	// strip is first added).
	UpdateName();

	if ( fChanged )
	{
		//BUGBUG: should be smarter about what we invalidate here..
		InvalidateStrip();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateSequenceUIChunk
//
// Update design state data structure (m_pSequenceMgr->m_SeqStripDesign).
//
void CSequenceStrip::UpdateSequenceUIChunk()
{
	// fill the structure with data
	if( m_pSequenceMgr->m_pTimeline )
	{
		VARIANT var;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var) ) )
		{
			m_pSequenceMgr->m_SeqStripDesign.m_lHeight = V_I4(&var);
		}
	}

	m_pSequenceMgr->m_SeqStripDesign.m_lVScroll = m_lVScroll;
	m_pSequenceMgr->m_SeqStripDesign.m_svView = m_svView;
	m_pSequenceMgr->m_SeqStripDesign.m_fHybridNotation = m_fHybridNotation;
	m_pSequenceMgr->m_SeqStripDesign.m_dblVerticalZoom = m_dblVerticalZoom;
	m_pSequenceMgr->m_SeqStripDesign.m_crUnselectedNoteColor = m_crUnselectedNoteColor;
	m_pSequenceMgr->m_SeqStripDesign.m_crSelectedNoteColor = m_crSelectedNoteColor;
	m_pSequenceMgr->m_SeqStripDesign.m_crOverlappingNoteColor = m_crOverlappingNoteColor;
	m_pSequenceMgr->m_SeqStripDesign.m_crAccidentalColor = m_crAccidentalColor;
	m_pSequenceMgr->m_SeqStripDesign.m_dwPChannel = m_pSequenceMgr->m_dwPChannel;
	m_pSequenceMgr->m_SeqStripDesign.m_dwExtraBars = m_dwExtraBars;
	m_pSequenceMgr->m_SeqStripDesign.m_fPickupBar = m_fPickupBar;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::OnApp

LRESULT CSequenceStrip::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( wParam == WM_APP_BUMPTIMECURSOR )
	{
		// Get the time of the time cursor
		long lTime;
		if (FAILED(m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
		{
			return 0;
		}

		SNAPTO snapTo = GetSnapToBoundary( lTime );
		if( snapTo == SNAP_NONE )
		{
			snapTo = SNAP_GRID;
		}
		BumpTimeCursor( snapTo );
	}
	else if( wParam == WM_APP_INVALIDATEPIANOROLL )
	{
		Sleep( 50 );
		MSG msg;
		while( 0 != PeekMessage( &msg, m_VScrollBar.m_hWnd, WM_APP, WM_APP, PM_REMOVE ) )
		{
			if( msg.wParam == 0 )
			{
				OnApp( 0, 0 );
			}
		}

		const long lOldVScroll = m_lVScroll;
		EnsureNoteCursorVisible();
		if( lOldVScroll == m_lVScroll )
		{
			InvalidatePianoRoll();
		}
	}
	else if( wParam == WM_APP_INSTRUMENTMENU )
	{
		// Pointer to interface from which DLS region text can be obtained
		IDMUSProdBandEdit8a* pIBandEdit;

		// Try to get the band edit interface
		pIBandEdit = GetBandEditInterface( m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwPChannel );

		if( pIBandEdit )
		{
			pIBandEdit->DisplayInstrumentButton(m_pSequenceMgr->m_dwPChannel, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			pIBandEdit->Release();
		}

		m_fInstrumentPressed = FALSE;
		InvalidateFunctionBar();
	}
	else if( wParam == WM_APP_BANDMENU )
	{
		// Pointer to interface to display the band edit menu
		IDMUSProdBandMgrEdit* pIDMUSProdBandMgrEdit;

		// Try to get the band edit interface
		pIDMUSProdBandMgrEdit = GetBandMgrEditInterface( m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwPChannel );

		if( pIDMUSProdBandMgrEdit )
		{
			pIDMUSProdBandMgrEdit->DisplayEditBandButton(m_pSequenceMgr->m_dwPChannel, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			pIDMUSProdBandMgrEdit->Release();
		}
		else
		{
			// If no band manager, display our own menu
			HMENU hMenu, hMenuPopup;
			hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_BANDMENU));
			if (hMenu)
			{
				hMenuPopup = ::GetSubMenu( hMenu, 0 );
				if (hMenuPopup)
				{
					m_pSequenceMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (IDMUSProdStrip *)this, FALSE);
				}
				DestroyMenu(hMenu); // This will destroy the submenu as well.
			}
		}
		
		m_fNewBandPressed = FALSE;
		InvalidateFunctionBar();
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::BumpTimeCursor

void CSequenceStrip::BumpTimeCursor( SNAPTO snapTo )
{
	// Get the time of the time cursor
	long lTime;
	if (FAILED(m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return;
	}

	// Get the amount to move the note cursor and time cursor by
	long lSnapAmount = GetSnapAmount( lTime, snapTo );

	// If the Time cursor would go beyond the end of the pattern, move the cursor back to the beginning
	if( /*fBumpRight &&*/ (lTime + lSnapAmount >= m_mtLength) )
	{
		ASSERT( lSnapAmount > 0 );
		lTime %= lSnapAmount;
		m_lInsertTime = 0;

		// Force the Timeline to scroll all the way to the beginning
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lTime );

		// Redraw
		InvalidatePianoRoll();
	}
	/*
	// If the Time cursor would go below 0, move the cursor to the end
	else if( !fBumpRight && (lTime < lSnapAmount) )
	{
		// Snap to nearest value from m_mtLength - 1
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, m_mtLength - 1 );

		// This sets m_lInsertTime to the correct value and redraws the strip, if necessary
		UpdateNoteCursorTime();

		// Snap m_lInsertTime to Bar
		long lMeasureSnap;
		m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength - 1, &lMeasureSnap, NULL );
		m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lMeasureSnap, 0, &lMeasureSnap );

		// Now reset the time cursor to the correct value
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS,
			min( m_mtLength - 1, lMeasureSnap + lTime + m_lInsertTime ) );
	}
	*/
	else
	{
		long lOldInsertTime = m_lInsertTime;
		long lOldDuration = GetSnapAmount( m_lInsertTime );
		/*
		if( !fBumpRight )
		{
			lSnapAmount *= -1;
		}
		*/

		lTime += lSnapAmount;

		// Snap to nearest value from lTime
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lTime );

		// This sets m_lInsertTime to the correct value and redraws the strip, if necessary
		UpdateNoteCursorTime( FALSE );

		// Invalidate only this note
		RECT rect;
		if( m_svView == SV_MINIMIZED )
		{
			rect.top = 0;
			rect.bottom = MINIMIZE_HEIGHT;
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( min(lOldInsertTime, m_lInsertTime), &rect.left );
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( max(lOldInsertTime + lOldDuration, m_lInsertTime + GetSnapAmount( m_lInsertTime )), &rect.right );
		}
		else
		{
			CSequenceItem seqItem;
			seqItem.m_bByte1 = (BYTE)m_lInsertVal;
			seqItem.m_bByte2 = 127;
			seqItem.m_mtTime = min(lOldInsertTime, m_lInsertTime);
			seqItem.m_mtDuration = max(lOldInsertTime + lOldDuration, m_lInsertTime + GetSnapAmount( m_lInsertTime )) - seqItem.m_mtTime;
			GetNoteRect( &seqItem, &rect );
			// Fix off-by-one redraw problem
			rect.right++;
			// Note marker's width is is duration or 5 pixels, whichever is more.
			rect.right = max( 5 + rect.left, rect.right );

			if( m_fHybridNotation )
			{
				// Extend to the left so the accidental is drawn
				rect.left -= ((m_lMaxNoteHeight * 3) / 2) + 2;

				// Extend to the top and bottom so the parts of the accidental that extend outside
				// the note are drawn
				rect.top -= m_lMaxNoteHeight / 2;
				rect.bottom += m_lMaxNoteHeight / 2;
			}
		}


		m_pSequenceMgr->m_pTimeline->StripInvalidateRect( this, &rect, FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DrawNoteInsertionMark

void CSequenceStrip::DrawNoteInsertionMark( HDC hDC, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset )
{
	if( m_lInsertTime > mtEndTime )
	{
		return;
	}

	// Get the snap amount
	long lDuration = GetSnapAmount( m_lInsertTime );

	if( m_lInsertTime + lDuration  < mtStartTime )
	{
		return;
	}

	RECT rectMark;
	ComputeNoteMarkerVerticalRect( rectMark, m_fHybridNotation, m_pSequenceMgr->m_fDisplayingFlats, m_lMaxNoteHeight, m_lInsertVal );

	// Compute the mark's length
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lDuration, &rectMark.right );
	lDuration = max( rectMark.right, 5 ); // Minimum of 5 pixels wide

	// Set up the accidental font, if necessary
	if( m_fHybridNotation )
	{
		// Compute the hybrid position
		int iHybridPos;
		if( m_pSequenceMgr->m_fDisplayingFlats )
		{
			iHybridPos = MIDIToHybridPos( BYTE(m_lInsertVal), aChromToScaleFlats);
		}
		else
		{
			iHybridPos = MIDIToHybridPos( BYTE(m_lInsertVal), aChromToScaleSharps);
		}

		// Compute the accidental
		int iAccidental;
		// FLATS
		if( m_pSequenceMgr->m_fDisplayingFlats )
		{
			iAccidental = aChromToFlatAccidentals[m_lInsertVal % 12];
		}
		// SHARPS
		else
		{
			iAccidental = aChromToSharpAccidentals[m_lInsertVal % 12];
		}

		if( iAccidental != m_aiScalePattern[iHybridPos % 7] )
		{
			int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
			COLORREF oldColor = ::SetTextColor( hDC, PIANOROLL_BLACKKEY_COLOR );
			// Create and select the font to draw the sharps and flats with
			HFONT hfontMusic = GetAccidentalFont();
			if( hfontMusic )
			{
				HFONT hNewOldFont = static_cast<HFONT>(::SelectObject( hDC, hfontMusic ));

				// Load sharp or flat text
				CString strSymbol;
				if( iAccidental < 0 )
				{
					strSymbol.LoadString(IDS_FLAT_TEXT);
				}
				else if( iAccidental > 0 )
				{
					strSymbol.LoadString(IDS_SHARP_TEXT);
				}
				else
				{
					strSymbol.LoadString(IDS_NATURAL_TEXT);
				}

				const long lTopAdjust = TOPADJUST;
				const long lBottomAdjust = BOTTOMADJUST;

				// Compute the mark's start and end position
				m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_lInsertTime, &rectMark.left );

				DrawSymbol( hDC, strSymbol, iHybridPos, rectMark.left - 2 - lXOffset, lTopAdjust, lBottomAdjust );

				::SelectObject( hDC, hNewOldFont );
				::DeleteObject( hfontMusic );
			}

			// Reset the color and mode
			::SetTextColor( hDC, oldColor );
			::SetBkMode( hDC, nOldBkMode );
		}
	}

	// Compute the mark's start and end position
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_lInsertTime, &rectMark.left );
	rectMark.left -= lXOffset;
	rectMark.right = lDuration + rectMark.left;

	int nCaps = ::GetDeviceCaps( hDC, RASTERCAPS );
	if( (nCaps & RC_BITBLT) && !(nCaps & RC_PALETTE) )
	{
		// Device support BitBlt and is not palette-based - draw transparent box
		// Calculate the source rectangle
		RECT rectNewRect;
		rectNewRect.left = 0;
		rectNewRect.top = 0;
		rectNewRect.right = rectMark.right - rectMark.left;
		rectNewRect.bottom = rectMark.bottom - rectMark.top;

		// Create the source bitmap
		HBITMAP hBitMap = ::CreateCompatibleBitmap( hDC, rectNewRect.right, rectNewRect.bottom );
		ASSERT( hBitMap );
		// Create the source DC
		HDC hNewDC = ::CreateCompatibleDC( hDC );
		HBITMAP hOldBitMap = static_cast<HBITMAP> (::SelectObject( hNewDC, hBitMap ) );
		// Draw the source bitmsp
		::SetBkColor( hNewDC, RGB(127,127,127) );
		::ExtTextOut( hNewDC, 0, 0, ETO_OPAQUE, &rectNewRect, NULL, 0, NULL);

		// Draw it
		::BitBlt( hDC, rectMark.left, rectMark.top, rectNewRect.right, rectNewRect.bottom, hNewDC, 0, 0, SRCAND);

		// Clean up
		::SelectObject( hNewDC, hOldBitMap );
		::DeleteDC( hNewDC );
		::DeleteObject( hBitMap );
	}
	else
	{
		// Device doesn't support BitBlt or is palette-based -  draw black box
		// Save the old background color
		COLORREF crOldBkColor = ::SetBkColor( hDC, ::GetNearestColor(hDC, 0) );

		// Draw it
		::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectMark, NULL, 0, NULL);

		// Reset the old background color
		::SetBkColor( hDC, crOldBkColor );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateNoteCursorTime

void CSequenceStrip::UpdateNoteCursorTime( BOOL fRedraw )
{
	long lTime;
	if (FAILED(m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return;
	}

	switch( GetSnapToBoundary( lTime ) )
	{
	case SNAP_GRID:
		// Snap to Grid
		{
			long lBeat, lGrid;
			lGrid = lTime;
			m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, &lTime, &lBeat );
			m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, lBeat, &lTime );
			lGrid -= lTime; // lGrid is now offset from start of beat.

			long lGridClocks = GetGridClocks( lTime );
			lGrid /= lGridClocks;
			lTime += lGrid * lGridClocks;
		}
		break;
	case SNAP_BEAT:
		// Snap to Beat
		{
			long lBeat;
			m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, &lTime, &lBeat );
			m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, lBeat, &lTime );
		}
		break;
	case SNAP_BAR:
		// Snap to Bar
		m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, &lTime, NULL );
		m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, 0, &lTime );
		break;
	case SNAP_NONE:
		// Snap to None
		// No need to update lTime, nothing changed
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	if( lTime != m_lInsertTime )
	{
		m_lInsertTime = lTime;
		if( fRedraw )
		{
			InvalidatePianoRoll();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetSnapToBoundary

SNAPTO CSequenceStrip::GetSnapToBoundary( long lTime ) const
{
	VARIANT var;
	var.vt = VT_I4;
	V_I4( &var ) = lTime;
	m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_SNAPAMOUNT, &var );

	// Get TimeSig
	DMUS_TIMESIGNATURE TimeSig;
	if(SUCCEEDED(m_pSequenceMgr->m_pTimeline->GetParam(GUID_TimeSignature,
										0xFFFFFFFF,
										0,
										lTime, NULL,
										&TimeSig)))
	{
		long lBeatClocks = (DMUS_PPQ * 4) / TimeSig.bBeat;
		if( V_I4( &var ) == lBeatClocks )
		{
			// Snap to Beat
			return SNAP_BEAT;
		}
		if( V_I4( &var ) == lBeatClocks * TimeSig.bBeatsPerMeasure )
		{
			// Snap to Bar
			return SNAP_BAR;
		}
		if( V_I4( &var ) == lBeatClocks / TimeSig.wGridsPerBeat )
		{
			// Snap to Grid
			return SNAP_GRID;
		}
	}

	ASSERT( V_I4( &var ) == 1 );
	// Snap to 'none'
	return SNAP_NONE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetSnapAmount

long CSequenceStrip::GetSnapAmount( long lTime, SNAPTO st ) const 
{
	if( st == SNAP_UNKNOWN )
	{
		st = GetSnapToBoundary( lTime );
	}

	if( st == SNAP_NONE )
	{
		// Snap to None
		return 1;
	}

	// Get TimeSig
	DMUS_TIMESIGNATURE TimeSig;
	if(SUCCEEDED(m_pSequenceMgr->m_pTimeline->GetParam(GUID_TimeSignature,
										m_pSequenceMgr->m_dwGroupBits,
										m_pSequenceMgr->m_dwIndex,
										lTime, NULL,
										&TimeSig)))
	{
		long lBeatClocks = (DMUS_PPQ * 4) / TimeSig.bBeat;
		switch( st )
		{
		case SNAP_GRID:
			// Snap to Grid
			return lBeatClocks / TimeSig.wGridsPerBeat;
		case SNAP_BEAT:
			// Snap to Beat
			return lBeatClocks;
		case SNAP_BAR:
			// Snap to Bar
			return lBeatClocks * TimeSig.bBeatsPerMeasure;
		}
	}

	ASSERT(FALSE);
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetAccidentalFont

HFONT CSequenceStrip::GetAccidentalFont( void )
{
	// Create the font to draw the sharps and flats with
	LOGFONT lf;
	memset( &lf, 0 , sizeof(LOGFONT));
	if( m_pSequenceMgr->m_fHasNotationStation )
	{
		lf.lfHeight = - long(MAX_NOTE_HEIGHT * NOTATION_FONT_ZOOMFACTOR * m_dblVerticalZoom);
		lf.lfCharSet = SYMBOL_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;

		CString strFontName;
		strFontName.LoadString(IDS_NOTATION_FONT);
		_tcsncpy( lf.lfFaceName, strFontName, LF_FACESIZE );
		lf.lfWeight = FW_NORMAL;
	}
	else
	{
		lf.lfHeight = long(MAX_NOTE_HEIGHT * 1.4 * m_dblVerticalZoom);
		//lf.lfCharSet = ANSI_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
		//lf.lfFaceName = NULL;
		lf.lfWeight = FW_SEMIBOLD;
	}
	//lf.lfWidth = 0;
	//lf.lfEscapement = 0;
	//lf.lfOrientation = 0;
	//lf.lfItalic = FALSE;
	//lf.lfUnderline = FALSE;
	//lf.lfStrikeOut = FALSE;
	//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//lf.lfQuality = DEFAULT_QUALITY;

	return ::CreateFontIndirect( &lf );
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetGridClocks

long CSequenceStrip::GetGridClocks( long lTime ) const
{
	// Get TimeSig
	DMUS_TIMESIGNATURE TimeSig;
	if(SUCCEEDED(m_pSequenceMgr->m_pTimeline->GetParam(GUID_TimeSignature,
										m_pSequenceMgr->m_dwGroupBits,
										m_pSequenceMgr->m_dwIndex,
										lTime, NULL,
										&TimeSig)))
	{
		ASSERT( TimeSig.bBeat && TimeSig.wGridsPerBeat );
		if( TimeSig.bBeat && TimeSig.wGridsPerBeat )
		{
			return ((DMUS_PPQ * 4) / TimeSig.bBeat) / TimeSig.wGridsPerBeat;
		}
	}

	// Shouldn't happen
	ASSERT(FALSE);
	return DMUS_PPQ / 4;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::ChangeNotationType

void CSequenceStrip::ChangeNotationType( BOOL fHybridNotation )
{
	if( m_fHybridNotation == fHybridNotation )
	{
		return;
	}

	int nLastEdit = 0;

	if( fHybridNotation )
	{
		m_fHybridNotation = TRUE;

		// Update the range
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = 37;
		m_VScrollBar.SetScrollInfo( &si, TRUE );

		// Fix 21001: Don't change zoom level when changing notation type.
		//m_dblVerticalZoom *= 1.6;
		nLastEdit = IDS_UNDO_DISP_HYBRID;
	}
	else
	{
		m_fHybridNotation = FALSE;

		// Update the range
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = 127;
		m_VScrollBar.SetScrollInfo( &si, TRUE );

		// Fix 21001: Don't change zoom level when changing notation type.
		//m_dblVerticalZoom /= 1.6;
		nLastEdit = IDS_UNDO_DISP_NORMAL;
	}

	VARIANT var;
	if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		int nHeightDiv2 = V_I4( &var ) / 2;
		int nMiddle = (m_lVScroll + nHeightDiv2 ) / m_lMaxNoteHeight;
		m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );

		if( m_fHybridNotation )
		{
			// Changed to Hybrid Notation
			nMiddle = MulDiv( nMiddle, 38, 128 );
		}
		else
		{
			// Changed to PianoRoll notation
			nMiddle = MulDiv( nMiddle, 128, 38 );
		}

		SetVScroll( nMiddle * m_lMaxNoteHeight - nHeightDiv2 );
		if( m_svView == SV_NORMAL )
		{
			UpdateVScroll();
			InvalidateStrip();
		}

		ASSERT( nLastEdit != 0 );

		// Let the object know about the changes
		m_nLastEdit = nLastEdit;
		m_pSequenceMgr->m_fUpdateDirectMusic = FALSE;
		m_pSequenceMgr->OnDataChanged();

		// If we're not a drum track, notify the other sequence strips so they change also.
		if( (m_pSequenceMgr->m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)
		||	(m_pSequenceMgr->m_dwPChannel & 0xF) != 9 )
		{
			// Only notify if TP_FREEZE_UNDO is NOT set.
			BOOL fFreezeUndo = FALSE;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fFreezeUndo = V_BOOL(&var);
			}

			if( !fFreezeUndo )
			{
				m_pSequenceMgr->m_pTimeline->NotifyStripMgrs( GUID_Sequence_Notation_Change, 0xFFFFFFFF, &m_fHybridNotation );
			}
		}

		EnsureNoteCursorVisible();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::ChangeZoom

void CSequenceStrip::ChangeZoom( double dblVerticalZoom )
{
	if( dblVerticalZoom < MINIMUM_ZOOM_LEVEL )
	{
		return;
	}

	if( m_dblVerticalZoom == dblVerticalZoom )
	{
		return;
	}

	m_dblVerticalZoom = dblVerticalZoom;

	VARIANT var;
	if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		int nHeightDiv2 = V_I4( &var ) / 2;
		int nMiddle = (m_lVScroll + nHeightDiv2 ) / m_lMaxNoteHeight;
		m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );

		SetVScroll( nMiddle * m_lMaxNoteHeight - nHeightDiv2 );
		if( m_svView == SV_NORMAL )
		{
			UpdateVScroll();
			InvalidateStrip();
		}

		EnsureNoteCursorVisible();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DoQuantize

HRESULT CSequenceStrip::DoQuantize( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store window that has focus
	HWND hwndFocus = ::GetFocus();
	
	CQuantizeDlg dlgQuantize;

	// Initialize the dialog's settings
	SequenceQuantize sq;
	if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->GetParam( GUID_Sequence_QuantizeParams,
														   m_pSequenceMgr->m_dwGroupBits,
														   m_pSequenceMgr->m_dwIndex,
														   0,
														   NULL,
														   &sq ) ) )
	{
		dlgQuantize.m_qtTarget = (QUANTIZE_TARGET)sq.m_wQuantizeTarget;
		dlgQuantize.m_lResolution = sq.m_bResolution;
		dlgQuantize.m_lStrength = sq.m_bStrength;
		dlgQuantize.m_dwFlags = sq.m_dwFlags;
	}
	else
	{
		dlgQuantize.m_qtTarget = QUANTIZE_TARGET_SELECTED;
		dlgQuantize.m_lResolution = 4;
		dlgQuantize.m_lStrength = 100;
		dlgQuantize.m_dwFlags = SEQUENCE_QUANTIZE_START_TIME;
	}

	// Get time signature beat
	DMUS_TIMESIGNATURE TimeSig;
	if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature,
														   m_pSequenceMgr->m_dwGroupBits,
														   m_pSequenceMgr->m_dwIndex,
														   0,
														   NULL,
														   &TimeSig ) ) )
	{
		dlgQuantize.m_bBeat = TimeSig.bBeat;
	}
	else
	{
		dlgQuantize.m_bBeat = 4;
	}

	// Check if we should enable the 'Selected Note(s)' option
	if( CanCopy() == S_OK )
	{
		dlgQuantize.m_fEnableSelected = TRUE;
	}
	else
	{
		dlgQuantize.m_fEnableSelected = FALSE;
		if( dlgQuantize.m_qtTarget == QUANTIZE_TARGET_SELECTED )
		{
			dlgQuantize.m_qtTarget = QUANTIZE_TARGET_PART;
		}
	}

	// Display the dialog.  Only act if the user clicked on 'OK'.
	if( dlgQuantize.DoModal() == IDOK )
	{
		sq.m_wQuantizeTarget = (WORD)dlgQuantize.m_qtTarget;
		sq.m_dwFlags = dlgQuantize.m_dwFlags;		

		ASSERT( (dlgQuantize.m_lResolution < UCHAR_MAX) && (dlgQuantize.m_lResolution > 0 ) );
		sq.m_bResolution = (BYTE)dlgQuantize.m_lResolution;	

		ASSERT( (dlgQuantize.m_lStrength <= 100) && (dlgQuantize.m_lStrength >= 0 ) );
		sq.m_bStrength = (BYTE)dlgQuantize.m_lStrength;

		// Save the dialog's settings
		m_pSequenceMgr->m_pTimeline->SetParam( GUID_Sequence_QuantizeParams,
											   m_pSequenceMgr->m_dwGroupBits,
											   m_pSequenceMgr->m_dwIndex,
											   NULL,
											   &sq );

		// Quantize either selected notes, entire Part, or entire Sequence
		switch( sq.m_wQuantizeTarget )
		{
			case QUANTIZE_TARGET_SELECTED:
			case QUANTIZE_TARGET_PART:
				m_pSequenceMgr->Quantize( &sq );
				break;
			case QUANTIZE_TARGET_SEQUENCE:
				// Assume something will change
				// This must be done before NotifyStripMgrs is called
				m_nLastEdit = IDS_UNDO_QUANTIZE;
				m_pSequenceMgr->OnDataChanged();
				m_pSequenceMgr->m_pTimeline->NotifyStripMgrs( GUID_Sequence_Quantize, m_pSequenceMgr->m_dwGroupBits, &sq );
				break;
			default:
				break;
		}
	}

	// Restore focus
	if( hwndFocus )
	{
		::SetFocus( hwndFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::DoVelocity

HRESULT CSequenceStrip::DoVelocity( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store window that has focus
	HWND hwndFocus = ::GetFocus();
	
	CDialogVelocity dlgVelocity;

	// Initialize the dialog's settings
	SequenceVelocitize sv;
	if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->GetParam( GUID_Sequence_VelocitizeParams,
														   m_pSequenceMgr->m_dwGroupBits,
														   m_pSequenceMgr->m_dwIndex,
														   0,
														   NULL,
														   &sv ) ) )
	{
		dlgVelocity.m_vtTarget = (VELOCITY_TARGET)sv.m_wVelocityTarget;
		dlgVelocity.m_lAbsoluteChangeStart = sv.m_lAbsoluteChangeStart;
		dlgVelocity.m_lAbsoluteChangeEnd = sv.m_lAbsoluteChangeEnd;
		dlgVelocity.m_bCompressMax = sv.m_bCompressMax;
		dlgVelocity.m_bCompressMin = sv.m_bCompressMin;
		switch( sv.m_dwVelocityMethod & SEQUENCE_VELOCITIZE_METHOD_MASK )
		{
		case SEQUENCE_VELOCITIZE_PERCENT:
			dlgVelocity.m_fAbsolute = true;
			dlgVelocity.m_fPercent = true;
			break;
		case SEQUENCE_VELOCITIZE_LINEAR:
			dlgVelocity.m_fAbsolute = true;
			dlgVelocity.m_fPercent = false;
			break;
		case SEQUENCE_VELOCITIZE_COMPRESS:
			dlgVelocity.m_fAbsolute = false;
			dlgVelocity.m_fPercent = true;
			break;
		}
	}
	else
	{
		dlgVelocity.m_vtTarget = VELOCITY_TARGET_SELECTED;
		dlgVelocity.m_lAbsoluteChangeStart = 0;
		dlgVelocity.m_lAbsoluteChangeEnd = 0;
		dlgVelocity.m_bCompressMax = 127;
		dlgVelocity.m_bCompressMin = 0;
		dlgVelocity.m_fAbsolute = true;
		dlgVelocity.m_fPercent = true;
	}

	// Check if we should enable the 'Selected Note(s)' option
	if( CanCopy() == S_OK )
	{
		dlgVelocity.m_fEnableSelected = true;
	}
	else
	{
		dlgVelocity.m_fEnableSelected = false;
		if( dlgVelocity.m_vtTarget == VELOCITY_TARGET_SELECTED )
		{
			dlgVelocity.m_vtTarget = VELOCITY_TARGET_PART;
		}
	}

	// Display the dialog.  Only act if the user clicked on 'OK'.
	if( dlgVelocity.DoModal() == IDOK )
	{
		sv.m_wVelocityTarget = (WORD)dlgVelocity.m_vtTarget;
		sv.m_lAbsoluteChangeStart = dlgVelocity.m_lAbsoluteChangeStart;
		sv.m_lAbsoluteChangeEnd = dlgVelocity.m_lAbsoluteChangeEnd;
		sv.m_bCompressMax = dlgVelocity.m_bCompressMax;
		sv.m_bCompressMin = dlgVelocity.m_bCompressMin;
		if( dlgVelocity.m_fAbsolute )
		{
			sv.m_dwVelocityMethod = dlgVelocity.m_fPercent ? SEQUENCE_VELOCITIZE_PERCENT : SEQUENCE_VELOCITIZE_LINEAR;
		}
		else
		{
			sv.m_dwVelocityMethod = SEQUENCE_VELOCITIZE_COMPRESS;
		}

		// Save the dialog's settings
		m_pSequenceMgr->m_pTimeline->SetParam( GUID_Sequence_VelocitizeParams,
											   m_pSequenceMgr->m_dwGroupBits,
											   m_pSequenceMgr->m_dwIndex,
											   NULL,
											   &sv );

		// Quantize either selected notes, entire Part, or entire Sequence
		switch( sv.m_wVelocityTarget )
		{
			case VELOCITY_TARGET_SELECTED:
			case VELOCITY_TARGET_PART:
				m_pSequenceMgr->Velocitize( &sv );
				break;
			case VELOCITY_TARGET_PATTERN:
				// Assume something will change
				// This must be done before NotifyStripMgrs is called
				m_nLastEdit = IDS_UNDO_VELOCITY;
				m_pSequenceMgr->OnDataChanged();
				m_pSequenceMgr->m_pTimeline->NotifyStripMgrs( GUID_Sequence_Velocitize, m_pSequenceMgr->m_dwGroupBits, &sv );
				break;
			default:
				break;
		}
	}

	// Restore focus
	if( hwndFocus )
	{
		::SetFocus( hwndFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::EnsureNoteCursorVisible

void CSequenceStrip::EnsureNoteCursorVisible( void )
{
	RECT rectMark;
	ComputeNoteMarkerVerticalRect( rectMark, m_fHybridNotation, m_pSequenceMgr->m_fDisplayingFlats, m_lMaxNoteHeight, m_lInsertVal );

	if( rectMark.bottom <= m_lVScroll )
	{
		// Need to scroll up
		SetVScroll( ((rectMark.top / m_lMaxNoteHeight) - 1) * m_lMaxNoteHeight );
	}
	else
	{
		VARIANT var;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) )
		&&	(rectMark.top > V_I4(&var) + m_lVScroll) )
		{
			// Need to scroll down
			SetVScroll( (((rectMark.top - V_I4(&var)) / m_lMaxNoteHeight) + 2) * m_lMaxNoteHeight );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateSelectionState

void CSequenceStrip::UpdateSelectionState( void )
{
	if( !m_pActiveNote || !m_pActiveNote->m_fSelected )
	{
		m_pActiveNote = m_pSequenceMgr->FirstSelectedSequence( NULL );
	}

	if( !m_pActiveNote )
	{
		m_SelectionMode = SEQ_NO_SELECT;
		return;
	}

	// Set m_prsSelecting based on the number of selected notes
	int nSelected = m_pSequenceMgr->GetNumSelected();
	if (nSelected == 1)
	{
		m_SelectionMode = SEQ_SINGLE_SELECT;
	}
	else if (nSelected > 1)
	{
		m_SelectionMode = SEQ_MULTIPLE_SELECT;
	}
	else if (nSelected == 0)
	{
		ASSERT(FALSE);
		// Shouldn't ever happen
		//m_SelectionMode = SEQ_NO_SELECT;
		//m_pActiveNote = NULL;
	}

	m_pSequenceMgr->UpdateStatusBarDisplay();
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::GetNumExtraBars

DWORD CSequenceStrip::GetNumExtraBars( void ) const
{
	MUSIC_TIME mtLastNoteOff = LONG_MIN;
	POSITION pos = m_pSequenceMgr->m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem *pItem = m_pSequenceMgr->m_lstSequences.GetNext( pos );
		MUSIC_TIME mtOffTime = pItem->AbsTime() + pItem->m_mtDuration;
		if( mtOffTime > mtLastNoteOff )
		{
			mtLastNoteOff = mtOffTime;
		}
	}

	pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		MUSIC_TIME mtOffTime = pItem->AbsTime() + pItem->m_mtDuration;
		if( mtOffTime > mtLastNoteOff )
		{
			mtLastNoteOff = mtOffTime;
		}
	}

	if( mtLastNoteOff > m_mtLength )
	{
		// Find out when the last measure starts
		long lLastMeasureStart;
		m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtLength, &lLastMeasureStart, NULL );
		m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lLastMeasureStart, 0, &lLastMeasureStart );

		// Find out the TimeSig of the last measure
		DMUS_TIMESIGNATURE dmTimeSig;
		GetTimeSig( m_mtLength - 1, &dmTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );
		const long lMeasureLength = dmTimeSig.bBeatsPerMeasure * ((DMUS_PPQ * 4) / dmTimeSig.bBeat);

		// Compute how far away the last note off is from the start of the last measure
		MUSIC_TIME mtOffset = mtLastNoteOff - lLastMeasureStart;

		// Return how many measure we need to display
		return max( m_dwExtraBars, DWORD((mtOffset + lMeasureLength - 1) / lMeasureLength) );
	}

	return m_dwExtraBars;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::ShouldDisplayPickupBar

BOOL CSequenceStrip::ShouldDisplayPickupBar( void ) const
{
	if( m_fPickupBar )
	{
		return TRUE;
	}

	if( !m_pSequenceMgr->m_lstSequences.IsEmpty() )
	{
		if( m_pSequenceMgr->m_lstSequences.GetHead()->AbsTime() < 0 )
		{
			// Found an early note - return TRUE
			return TRUE;
		}
	}

	if( !m_pSequenceMgr->m_lstCurves.IsEmpty() )
	{
		if( m_pSequenceMgr->m_lstCurves.GetHead()->AbsTime() < 0 )
		{
			// Found an early curve - return TRUE
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::SetPChannelThru

void CSequenceStrip::SetPChannelThru( void )
{
	int iChannel;
	for( iChannel=0; iChannel < 16; iChannel++ )
	{
		if( FAILED( m_pSequenceMgr->m_pIConductor->SetPChannelThru( iChannel, m_pSequenceMgr->m_dwPChannel ) ) )
		{
			break;
		}
	}

	// If we didn't successfullly complete SetPChannelThru for all 16 channels,
	// cancel the MIDI thruing
	if( iChannel != 16 )
	{
		CancelPChannelThru();
		return;
	}

	// All sixteen completed successfully, set m_fMIDIThruEnabled
	m_fMIDIThruEnabled = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::CancelPChannelThru

void CSequenceStrip::CancelPChannelThru( void )
{
	for( int iChannel=0; iChannel < 16; iChannel++ )
	{
		m_pSequenceMgr->m_pIConductor->CancelPChannelThru( iChannel );
	}

	m_fMIDIThruEnabled = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceStrip::UpdateInstrumentName

void CSequenceStrip::UpdateInstrumentName( void )
{
	// Initialize our temporary patch to an invalid value
	DWORD dwPatch = INVALID_PATCH;

	// Pointer to interface from which DLS region text can be obtained
	IDMUSProdBandEdit8a* pIBandEdit;

	// Try to get the band edit interface
	pIBandEdit = GetBandEditInterface( m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwPChannel );

	if( pIBandEdit )
	{
		pIBandEdit->GetPatchForPChannel( m_pSequenceMgr->m_dwPChannel, &dwPatch );
		pIBandEdit->Release();
	}

	if( m_dwLastPatch != dwPatch )
	{
		m_dwLastPatch = dwPatch;

		if( SV_NORMAL == m_svView )
		{
			InvalidateFunctionBar();
		}
	}
}
