// PianoRollStrip.cpp : Implementation of CPianoRollStrip
#include "stdafx.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "DialogNewCCTrack.h"
#include "QuantizeDlg.h"
#include "CurveStrip.h"
#include "DllJazzDataObject.h"
#include "ioDMStyle.h"
#include <winbase.h>
#include "NoteTracker.h"
#include <dmusicf.h>
#include <dmusici.h>
#include <PChannelName.h>
#include "SequenceStripMgr.h"
#include "StyleDesigner.h"
#include "BandEditor.h"
#include <riffstrm.h>
#include "PropPageMgr.h"
#include "VarSwitchStrip.h"
#include "TabPatternPattern.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"
#include "SharedPianoRoll.h"
#include "DialogVelocity.h"
#include "GrayOutRect.h"
#include "windowsx.h"
#include "dmusicp.h"

#define INSTRUMENT_BUTTON_HEIGHT 20
#define NEWBAND_BUTTON_WIDTH 20

#define WM_APP_BUMPTIMECURSOR 0
#define WM_APP_INVALIDATEPIANOROLL 1
#define WM_APP_INSTRUMENTMENU 2
#define WM_APP_BANDMENU 3

// Needed by GetStripProperty(SP_MAXHEIGHT) and ComputeVScrollBar()
#include "..\Timeline\TimelineDraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

long CPianoRollStrip::m_lBitmapRefCount = 0;
CBitmap	CPianoRollStrip::m_BitmapBlankButton;
CBitmap	CPianoRollStrip::m_BitmapPressedButton;
CBitmap	CPianoRollStrip::m_BitmapBlankInactiveButton;
CBitmap	CPianoRollStrip::m_BitmapPressedInactiveButton;
CBitmap	CPianoRollStrip::m_BitmapGutter;
CBitmap	CPianoRollStrip::m_BitmapPressedGutter;
CBitmap	CPianoRollStrip::m_BitmapMoaw;
CBitmap	CPianoRollStrip::m_BitmapPressedMoaw;
CBitmap	CPianoRollStrip::m_BitmapZoomInUp;
CBitmap	CPianoRollStrip::m_BitmapZoomInDown;
CBitmap	CPianoRollStrip::m_BitmapZoomOutUp;
CBitmap	CPianoRollStrip::m_BitmapZoomOutDown;
CBitmap CPianoRollStrip::m_BitmapNewBand;

DWORD PreviousNumericalVarition( DWORD dwCurrentVariation, DWORD dwValidVariations )
{
	// Search for the one variation that is active
	BOOL fFoundVariation = FALSE;
	int nFoundVariation = -1;
	int nVariation;
	for( nVariation = 0; nVariation < 32; nVariation++ )
	{
		if( dwCurrentVariation & (1 << nVariation) )
		{
			// Exit if more than one variation is active
			if( fFoundVariation )
			{
				return dwCurrentVariation;
			}
			nFoundVariation = nVariation;
			fFoundVariation = TRUE;
		}
	}

	// Exit if no variations are active
	if( !fFoundVariation )
	{
		return dwCurrentVariation;
	}

	// Search for a non-disabled variation below nFoundVariation
	for( nVariation = nFoundVariation - 1; nVariation >= 0; nVariation-- )
	{
		if( !(dwValidVariations & (1 << nVariation)) )
		{
			// Found a non-disabled variation - make it active
			return 1 << nVariation;
		}
	}

	// Search for a non-disabled variation above nFoundVariation
	for( nVariation = 31; nVariation > nFoundVariation; nVariation-- )
	{
		if( !(dwValidVariations & (1 << nVariation)) )
		{
			// Found a non-disabled variation - make it active
			return 1 << nVariation;
		}
	}

	return dwCurrentVariation;
}

DWORD NextNumericalVarition( DWORD dwCurrentVariation, DWORD dwValidVariations )
{
	// Search for the one variation that is active
	BOOL fFoundVariation = FALSE;
	int nFoundVariation = -1;
	int nVariation;
	for( nVariation = 0; nVariation < 32; nVariation++ )
	{
		if( dwCurrentVariation & (1 << nVariation) )
		{
			// Exit if more than one variation is active
			if( fFoundVariation )
			{
				return dwCurrentVariation;
			}
			nFoundVariation = nVariation;
			fFoundVariation = TRUE;
		}
	}

	// Exit if no variations are active
	if( !fFoundVariation )
	{
		return dwCurrentVariation;
	}

	// Search for a non-disabled variation above nFoundVariation
	for( nVariation = nFoundVariation + 1; nVariation < 32; nVariation++ )
	{
		if( !(dwValidVariations & (1 << nVariation)) )
		{
			// Found a non-disabled variation - make it active
			return 1 << nVariation;
		}
	}

	// Search for a non-disabled variation below nFoundVariation
	for( nVariation = 0; nVariation < nFoundVariation; nVariation++ )
	{
		if( !(dwValidVariations & (1 << nVariation)) )
		{
			// Found a non-disabled variation - make it active
			return 1 << nVariation;
		}
	}

	return dwCurrentVariation;
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip

CPianoRollStrip::CPianoRollStrip( CMIDIMgr* pMIDIMgr, CDirectMusicPartRef *pPartRef )
{

	ASSERT( pMIDIMgr != NULL );
	m_pMIDIMgr = pMIDIMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pMIDIMgr;
	m_pStripMgr->AddRef();

	ASSERT( pPartRef != NULL && pPartRef->m_pDMPart != NULL );
	m_pPartRef = pPartRef;
	ValidPartRefPtr();

	m_cRef = 0;
	AddRef();

	m_MouseMode				= PRS_MM_NORMAL;
	m_dwVariations			= ALL_VARIATIONS;
	m_crUnselectedNoteColor	= pMIDIMgr->m_PianoRollData.crUnselectedNoteColor;
	m_crSelectedNoteColor	= pMIDIMgr->m_PianoRollData.crSelectedNoteColor;
	m_crOverlappingNoteColor= pMIDIMgr->m_PianoRollData.crOverlappingNoteColor;
	m_crAccidentalColor		= pMIDIMgr->m_PianoRollData.crAccidentalColor;
	m_dblVerticalZoom		= 0.1;
	m_lMaxNoteHeight		= long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
	m_lVerticalScroll		= -1;
	m_StripView				= SV_NORMAL;
	m_fHybridNotation		= FALSE;
	m_fZoomInPressed		= FALSE;
	m_fZoomOutPressed		= FALSE;
	m_fNewBandPressed		= FALSE;
	m_fInstrumentPressed	= FALSE;
	m_fInstrumentEnabled	= FALSE;
	m_dwExtraBars			= 0;
	m_fPickupBar			= FALSE;
	m_pActiveCurveStrip		= NULL;
	m_dwPlayingVariation	= 0;

	DirectMusicTimeSig ts;

	// Set fallback TimeSig
	ts = pMIDIMgr->m_PianoRollData.ts;

	if (m_pPartRef != NULL)
	{
		if( m_pPartRef->m_pDMPart != NULL )
		{
			ts = m_pPartRef->m_pDMPart->m_TimeSignature;

			if ((m_dwVariations & ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled) == ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
			{
				m_fGutterPressed = TRUE;
			}
			else
			{
				m_fGutterPressed = FALSE;
			}
		}
	}
	m_fVariationChoicesPressed = FALSE;

	if ((ts.m_bBeat != 0) &&
		(ts.m_wGridsPerBeat != 0))
	{
		m_lBeatClocks = DM_PPQNx4 / ts.m_bBeat;
		m_lMeasureClocks = m_lBeatClocks * ts.m_bBeatsPerMeasure;
		m_lGridClocks = m_lBeatClocks / ts.m_wGridsPerBeat;
	}

	if( m_pPartRef )
	{
		m_OriginalPropNote.SetPart( m_pPartRef->m_pDMPart );
	}

	m_fGutterSelected = FALSE;
	m_hCursor = GetArrowCursor();
	m_prsSelecting = PRS_NO_SELECT;
	m_pActiveDMNote = NULL;
	m_pShiftDMNote = NULL;
	m_pPropPageMgr = NULL;
	m_lBeginTimelineSelection = -1;
	m_lEndTimelineSelection = -1;
	m_fPropPageActive = FALSE;
	m_fDontRedraw = FALSE;
	m_fRefreshCurveProperties = TRUE;
	m_fRefreshVarSwitchProperties = TRUE;
	m_fPianoRollMenu = TRUE;
	m_fScrollTimerActive = FALSE;
	m_fVariationsTimerActive = false;
	m_nLastVariation = -1;
	m_nSoloVariation = -1;
	m_fSolo = FALSE;
	m_dwOldVariations = m_dwVariations;
	m_fMIDIInEnabled = FALSE;
	m_pIConductor = NULL;
	memset(m_mtStartTimeOffset, 0, sizeof(MUSIC_TIME) * 128);
	memset(m_mtStartTime, 0, sizeof(MUSIC_TIME) * 128);
	memset(m_bVelocity, 0, sizeof(BYTE) * 128);
	m_fMIDIThruEnabled = FALSE;
	m_fInShowProperties = FALSE;
	m_fCtrlKeyDown = 0;
	m_lLastDeltaStart = 0;
	m_cLastDeltaValue = 0;
	m_lLastDeltaVel = 0;
	m_lLastDeltaDur = 0;
	ZeroMemory(m_aiAccidentals, sizeof(int) * 75);
	m_lInsertVal = 60;
	m_lInsertTime = 0;
	m_mtTimelineLengthForGetNoteRect = 0;
	m_lLastHeight = 0;
	m_mtLastEarlyTime = 0;
	m_mtLastLateTime = 0;
	m_fInsertingNoteFromMouseClick = false;
	m_dwLastPatch = INVALID_PATCH;


	OSVERSIONINFO osVI;
	osVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if ( GetVersionEx( &osVI ) )
	{
		m_dwPlatformId = osVI.dwPlatformId;
	}

	m_bMinimizeTopNote = 127;
	m_bMinimizeNoteRange = 127;
	m_fInSetData = FALSE;

	// Curve Strip fields
	m_CurveStripView = SV_MINIMIZED;

	m_pVarSwitchStrip = new CVarSwitchStrip( m_pMIDIMgr, this );
}

CPianoRollStrip::~CPianoRollStrip()
{
	ASSERT( m_pStripMgr != NULL );

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
		if( m_pMIDIMgr && m_pMIDIMgr->m_pTimeline )
		{
			m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );
		}
		pCurveStrip->Release();
	}

	// Release m_pVarSwitchStrip
	if( m_pVarSwitchStrip )
	{
		m_pVarSwitchStrip->Release();
		m_pVarSwitchStrip = NULL;
	}

	UnRegisterMidi();

	if (m_pIConductor)
	{
		m_pIConductor->Release();
		m_pIConductor = NULL;
	}
	// This is handled in CMIDIMgr::SetClientTimeline
	/*
	if( m_pMIDIMgr->m_pTimeline )
	{
		m_pMIDIMgr->m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
	}
	*/
	m_pStripMgr->Release();
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip IUnknown
STDMETHODIMP CPianoRollStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	ASSERT( ppv != NULL );
    *ppv = NULL;
    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
		*ppv = (IDMUSProdStripFunctionBar *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdPropPageObject))
	{
		*ppv = (IDMUSProdPropPageObject *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdTimelineEdit))
	{
		*ppv = (IDMUSProdTimelineEdit *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdMidiInCPt))
	{
		*ppv = (IDMUSProdMidiInCPt *) this;
	}
	else
	{
		return E_NOTIMPL;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CPianoRollStrip::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CPianoRollStrip::Release(void)
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
// CPianoRollStrip custom functions

IDMUSProdBandEdit8a *GetBandEditInterface( IDMUSProdNode *pStyleNode, IUnknown *punkMIDIMgr, IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, DWORD dwPChannel )
{
	IDMUSProdBandEdit8a *pIBandEdit = NULL;
	if( pStyleNode )
	{
		IDMUSProdStyleInfo* pIStyleInfo;
		if( SUCCEEDED ( pStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void **)&pIStyleInfo ) ) )
		{
			IUnknown* pIActiveBandNode;
			if( SUCCEEDED ( pIStyleInfo->GetActiveBandForObject( punkMIDIMgr, &pIActiveBandNode ) ) )
			{
				if( FAILED ( pIActiveBandNode->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIBandEdit ) ) )
				{
					pIBandEdit = NULL;
				}

				pIActiveBandNode->Release();
			}
			pIStyleInfo->Release();
		}
	}
	else
	{
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
	}

	return pIBandEdit;
}

IDMUSProdBandMgrEdit *GetBandMgrEditInterface( IDMUSProdNode *pStyleNode, IUnknown *punkMIDIMgr, IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, DWORD dwPChannel )
{
	// Initialize the interface to NULL
	IDMUSProdBandMgrEdit *pIDMUSProdBandMgrEdit = NULL;

	if( pStyleNode )
	{
		IDMUSProdStyleInfo* pIStyleInfo;
		if( SUCCEEDED ( pStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void **)&pIStyleInfo ) ) )
		{
			IUnknown* pIActiveBandNode;
			if( SUCCEEDED ( pIStyleInfo->GetActiveBandForObject( punkMIDIMgr, &pIActiveBandNode ) ) )
			{
				if( FAILED ( pIActiveBandNode->QueryInterface( IID_IDMUSProdBandMgrEdit, (void **)&pIDMUSProdBandMgrEdit ) ) )
				{
					pIDMUSProdBandMgrEdit = NULL;
				}

				pIActiveBandNode->Release();
			}
			pIStyleInfo->Release();
		}
	}
	else
	{
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
	}

	return pIDMUSProdBandMgrEdit;
}

void CPianoRollStrip::DrawFunctionBar( HDC hDC, STRIPVIEW sv )
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
		if (FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
		{
			return;
		}

		// Fill the area corresponding to the variation buttons with white, if the strip is not active
		if( (m_pMIDIMgr->m_pActivePianoRollStrip != this)
		||	(m_pMIDIMgr->m_iFocus != 1) )
		{
			COLORREF crOldBkColor = ::SetBkColor( hDC, RGB(255, 255, 255) );
			RECT rectOverwrite = { 0, m_lVerticalScroll, rectFBar.right, m_lVerticalScroll + 2 * VARIATION_BUTTON_HEIGHT };
			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectOverwrite, NULL, 0, NULL);
			::SetBkColor( hDC, crOldBkColor );

		}

		// Draw Zoom buttons
		// Save DC
		RECT rectExcludeClip = { 0, 0, 0, 0 };
		int iSavedDC = ::SaveDC( hDC );
		{
			// get function bar rect
			CRect rectFBar;
			VARIANT var;
			var.vt = VT_BYREF;
			V_BYREF(&var) = &rectFBar;
			if (FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
			{
				return;
			}

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
					rectFBar.right - m_BitmapZoomInDown.GetBitmapDimension().cx - 2, m_lVerticalScroll,
					m_BitmapZoomInDown.GetBitmapDimension().cx, m_BitmapZoomInDown.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			else
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomInUp.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomInUp.GetBitmapDimension().cx - 2, m_lVerticalScroll,
					m_BitmapZoomInUp.GetBitmapDimension().cx, m_BitmapZoomInUp.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			if( m_fZoomOutPressed )
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomOutDown.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomOutDown.GetBitmapDimension().cx - 2, m_BitmapZoomInUp.GetBitmapDimension().cy + m_lVerticalScroll,
					m_BitmapZoomOutDown.GetBitmapDimension().cx, m_BitmapZoomOutDown.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}
			else
			{
				::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapZoomOutUp.GetSafeHandle() ), NULL,
					rectFBar.right - m_BitmapZoomOutUp.GetBitmapDimension().cx - 2, m_BitmapZoomInUp.GetBitmapDimension().cy + m_lVerticalScroll,
					m_BitmapZoomOutUp.GetBitmapDimension().cx, m_BitmapZoomOutUp.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );
			}

			// Set up the rect to exclude
			rectExcludeClip.right = rectFBar.right - 2;
			rectExcludeClip.left = rectExcludeClip.right - m_BitmapZoomInDown.GetBitmapDimension().cx;
			rectExcludeClip.top = m_lVerticalScroll;
			rectExcludeClip.bottom = m_lVerticalScroll + m_BitmapZoomInDown.GetBitmapDimension().cy + m_BitmapZoomOutDown.GetBitmapDimension().cy;
		}
		// Restore the DC
		::RestoreDC( hDC, iSavedDC );

		// Exclude the zoom buttons
		::ExcludeClipRect( hDC, rectExcludeClip.left, rectExcludeClip.top, rectExcludeClip.right, rectExcludeClip.bottom );

		// Get the clipping rectangle
		RECT rectClip, rectNote;
		::GetClipBox( hDC, &rectClip );

		// Pointer to interface from which DLS region text can be obtained
		IDMUSProdBandEdit8a* pIBandEdit = NULL;

		// If the instrument button is visible
		VARIANT varFNHeight;
		m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight );
		if( rectClip.bottom > m_lVerticalScroll + V_I4(&varFNHeight)
		&&	rectClip.top < m_lVerticalScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
		{
			// Try to get the band edit interface
			if( !pIBandEdit )
			{
				pIBandEdit = GetBandEditInterface( m_pMIDIMgr->m_pIStyleNode, static_cast<IDMUSProdStripMgr *>(m_pMIDIMgr), m_pMIDIMgr->m_pTimeline, m_pMIDIMgr->m_dwGroupBits, m_pPartRef->m_dwPChannel );
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
				if( SUCCEEDED( pIBandEdit->GetInstNameForPChannel( m_pPartRef->m_dwPChannel, &bstrInstrumentName ) ) )
				{
					strInstrumentName = bstrInstrumentName;
					::SysFreeString( bstrInstrumentName );

					fNoInstrument = false;
				}

				pIBandEdit->GetPatchForPChannel( m_pPartRef->m_dwPChannel, &m_dwLastPatch );
			}

			// Compute the rectangle for the button
			RECT rectButton;
			rectButton.left = 0;
			rectButton.right = (rectFBar.right - rectFBar.left) - m_rectScrollBar.Width() - NEWBAND_BUTTON_WIDTH - 1;
			rectButton.top = m_lVerticalScroll + V_I4(&varFNHeight);
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

		// Mask out the area corresponding to the variation selection buttons
		::ExcludeClipRect( hDC, 0, m_lVerticalScroll, rectFBar.right, m_lVerticalScroll + 2 * VARIATION_BUTTON_HEIGHT );

		// Move the window origin down, to account for the variation selection buttons
		POINT point;
		::GetWindowOrgEx( hDC, &point );
		point.y -= 2 * VARIATION_BUTTON_HEIGHT;
		::SetWindowOrgEx( hDC, point.x, point.y, NULL );

		// Get the new clipping rectangle
		::GetClipBox( hDC, &rectClip );

		if( m_fHybridNotation )
		{
			//::SetBkColor( hDC, ::GetNearestColor( hDC, PIANOROLL_WHITEKEY_COLOR ) );
			//::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectClip, NULL, 0, NULL);

			COLORREF oldColor = ::SetTextColor( hDC, PIANOROLL_BLACKKEY_COLOR );

			int nTopNote, nBottomNote;
			nTopNote = 38 - (rectClip.top / m_lMaxNoteHeight);
			nBottomNote = 38 - (rectClip.bottom / m_lMaxNoteHeight);

			nBottomNote = max( nBottomNote, 13 );
			nTopNote = min( nTopNote, 23 );

			DrawHybridLines( hDC, rectClip, nBottomNote, nTopNote );

			if( m_pMIDIMgr->m_fHasNotationStation )
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

				// Create and select the font to draw the sharps and flats with
				hfontMusic = GetAccidentalFont();
				if( hfontMusic )
				{
					HFONT hNewOldFont = static_cast<HFONT>(::SelectObject( hDC, hfontMusic ));

					{
						// Load sharp or flat text
						CString strSymbol;
						if( m_pMIDIMgr->m_fDisplayingFlats )
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
						for( int i = 0; i < m_pMIDIMgr->m_nNumAccidentals; i++ )
						{
							long lXSymbolOffset = (m_lMaxNoteHeight * (10 + i) ) / 3;
							if( m_pMIDIMgr->m_fDisplayingFlats )
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

				int nVertPos = (38 - nBottomNote) * ( m_lMaxNoteHeight );
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
				pIBandEdit = GetBandEditInterface( m_pMIDIMgr->m_pIStyleNode, static_cast<IDMUSProdStripMgr *>(m_pMIDIMgr), m_pMIDIMgr->m_pTimeline, m_pMIDIMgr->m_dwGroupBits, m_pPartRef->m_dwPChannel );
			}

			HPEN hpenOld = static_cast<HPEN>(::SelectObject( hDC, hpenNoteLine ));
			// TODO: Optimize this to only draw the visible notes (get top and bottom notes).
			// The continue and break statements can then be taken out
			// Also optimize in CSequenceStrip.cpp
			for( value = 0; value < 128; value++ )
			{
				lTop = ( 127 - value ) * ( m_lMaxNoteHeight );
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
					if( SUCCEEDED ( pIBandEdit->GetDLSRegionName( m_pPartRef->m_dwPChannel, (BYTE)value, &bstrRegionName ) ) )
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
				else if ( ((m_pPartRef->m_dwPChannel & 0xF) == 9) &&
					 (value >=27) && (value <= 87) )
				{
					// Draw GM drum kit names
					::SetTextColor( hDC, (PianoKeyColor[ value % 12 ] == BLACK) ? PIANOROLL_WHITEKEY_COLOR : PIANOROLL_BLACKKEY_COLOR );

					rectNote.left = 0;
					::DrawText( hDC, acDrums[value-27], -1, &rectNote,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
				}
				else if (value % 12 == 0)
				{
					// Draw CX where "X" is octave
					::SetTextColor( hDC, PIANOROLL_BLACKKEY_COLOR );

					CString cstrTxt;
					cstrTxt.Format( "C %d", value/12 );
					rectNote.left = 0;
					::DrawText( hDC, cstrTxt, -1, &rectNote,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
				}
				::SetTextColor( hDC, oldColor );
			}
			::SelectObject( hDC, hpenOld );
			::DeleteObject( hpenNoteLine );

			if (rectClip.top <= m_lVerticalScroll)
			{
				HPEN	hpenTopLine = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
				if( hpenTopLine )
				{
					HPEN hpenOld = static_cast<HPEN>(::SelectObject( hDC, hpenTopLine ));
					::MoveToEx( hDC, rectClip.left, m_lVerticalScroll, NULL );
					::LineTo( hDC, rectClip.right, m_lVerticalScroll );
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

void CPianoRollStrip::DrawDarkHybridLines( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote )
{
	DrawHybridLinesHelper( hDC, rectClip, nOldBottomNote, nOldTopNote, RGB(230,230,230), m_lMaxNoteHeight, true );
}

void CPianoRollStrip::DrawHybridLines( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote )
{
	DrawHybridLinesHelper( hDC, rectClip, nOldBottomNote, nOldTopNote, RGB(255,255,255), m_lMaxNoteHeight, (m_pMIDIMgr->m_pActivePianoRollStrip != this) || (m_pMIDIMgr->m_iFocus != 1) );
}

void CPianoRollStrip::DrawDarkHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote )
{
	DrawHorizontalLinesHelper( hDC, rectClip, nBottomNote, nTopNote, RGB(230, 230, 230), RGB(200, 200, 200), m_lMaxNoteHeight, true );
}

void CPianoRollStrip::DrawHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote )
{
	DrawHorizontalLinesHelper( hDC, rectClip, nBottomNote, nTopNote, RGB(255, 255, 255), RGB(230, 230, 230), m_lMaxNoteHeight, (m_pMIDIMgr->m_pActivePianoRollStrip != this) || (m_pMIDIMgr->m_iFocus != 1) );
}

void CPianoRollStrip::GetHybridTopBottomRect( RECT* pRect, int iHybridPos, int iVelocity )
{
	ASSERT( pRect );
	ASSERT( m_pMIDIMgr->m_pTimeline );

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
}

void CPianoRollStrip::GetHybridRect( RECT* pRect, int iHybridPos, int iVelocity, MUSIC_TIME mtStart, MUSIC_TIME mtDuration )
{
	ASSERT( pRect );

	if ( m_pMIDIMgr->m_pTimeline )
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
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtStart, &pRect->left );

		// Convert the duration into a pixel value
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtDuration, &mtDuration );

		// Ensure the note is at least one pixel long
		if ( mtDuration < 1 )
		{
			mtDuration = 1;
		}

		// Set the end time to the start position plus the duration length
		pRect->right = pRect->left + mtDuration;
	}
	else
	{
		memset( pRect, 0, sizeof(RECT) );
	}
}

void CPianoRollStrip::GetNoteTopBottomRect( CDirectMusicStyleNote* pDMNote, RECT* pRect )
{
	ASSERT( m_pPartRef != NULL );
	ASSERT( pRect );
	ASSERT( pDMNote );
	ASSERT( m_pMIDIMgr->m_pTimeline );

	if( m_fHybridNotation )
	{
		int iHybridPos;
		// FLATS
		if( m_pMIDIMgr->m_fDisplayingFlats )
		{
			iHybridPos = MIDIToHybridPos( pDMNote->m_bMIDIValue, aChromToScaleFlats);
		}
		// SHARPS
		else
		{
			iHybridPos = MIDIToHybridPos( pDMNote->m_bMIDIValue, aChromToScaleSharps);
		}

		// This takes care of ensuring the note is at least one pixel high
		GetHybridTopBottomRect( pRect, iHybridPos, pDMNote->m_bVelocity );
	}
	else
	{
		// set the bottom
		pRect->bottom = ( 128 - pDMNote->m_bMIDIValue ) * m_lMaxNoteHeight - HORIZ_LINE_HEIGHT;

		// Find the top of the note's rectangle
		// The velocity offset isn't precisely correct - but it's close enough if m_lMaxNoteHeight < 127.
		pRect->top = pRect->bottom - ((pDMNote->m_bVelocity * m_lMaxNoteHeight) >> 7);
	
		// Ensure the note is at least one pixel high
		if( pRect->top == pRect->bottom )
		{
			pRect->top--;
		}
	}
}

void CPianoRollStrip::GetNoteRect( CDirectMusicStyleNote* pDMNote, RECT* pRect )
{
	long position;

	ASSERT( m_pPartRef != NULL );
	ASSERT( pRect );
	ASSERT( pDMNote );

	if ( pDMNote && m_pMIDIMgr->m_pTimeline )
	{
		// Compute top and bottom edges of the note
		GetNoteTopBottomRect( pDMNote, pRect );

		// Compute the start time of the  note
		position = GRID_TO_CLOCKS( pDMNote->m_mtGridStart, m_pPartRef->m_pDMPart) + pDMNote->m_nTimeOffset;

		// Otherwise, convert its starting time to a position
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( position, &pRect->left );

		// Convert the end time of the note (or m_mtTimelineLengthForGetNoteRect, if it's lesser) into a pixel value
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( min( m_mtTimelineLengthForGetNoteRect + m_mtLastLateTime, position + pDMNote->m_mtDuration), &pRect->right );

		// Ensure the note is at least one pixel long
		if( pRect->right <= pRect->left )
		{
			pRect->right = pRect->left + 1;
		}
	}
	else
	{
		memset( pRect, 0, sizeof(RECT) );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip IDMUSProdStrip
HRESULT	STDMETHODCALLTYPE CPianoRollStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPartRef != NULL );

	if (!hDC)
	{
		return E_INVALIDARG;
	}

	// Validate pointer to the Timeline
	if( m_pMIDIMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	int iSavedDC = ::SaveDC( hDC );
	if (sv == SV_NORMAL)
	{
		// Exclude the variation bar from the clipping region
		RECT rectClip;
		::GetClipBox( hDC, &rectClip );
		::ExcludeClipRect( hDC, rectClip.left, m_lVerticalScroll, rectClip.right, m_lVerticalScroll + (VARIATION_BUTTON_HEIGHT << 1) );

		// Offset the origin so it's below the variation selectionbar
		POINT point;
		::GetWindowOrgEx( hDC, &point );
		point.y -= (VARIATION_BUTTON_HEIGHT << 1);
		::SetWindowOrgEx( hDC, point.x, point.y, NULL );
	}

	// Get the clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Initialize m_mtTimelineLengthForGetNoteRect
	{
		VARIANT varLength;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
		{
			m_mtTimelineLengthForGetNoteRect = V_I4( &varLength );
		}
	}

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
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_mtTimelineLengthForGetNoteRect, &lTimelinePixelLength ) ) )
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

			// TODO: Fix this so it's more accurate (Also in SequenceStrip.cpp)
			nTopNote = min( nTopNote * 4, 127 );
			nBottomNote = max( nBottomNote * 3, 0 );
		}
		else
		{
			nTopNote = 127 - (rectClip.top / m_lMaxNoteHeight);
			nBottomNote = 127 - (rectClip.bottom / m_lMaxNoteHeight);

			nTopNote = min( nTopNote, 127 );
			nBottomNote = max( nBottomNote, 0 );

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

		// Draw a line between the variation selection bar and the rest of the piano roll
		HPEN penTopLine = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
		if( penTopLine )
		{
			// Draw the line
			HPEN penOld = (HPEN)::SelectObject( hDC, penTopLine );
			::MoveToEx( hDC, rectClip.left, m_lVerticalScroll, NULL );
			::LineTo( hDC, rectClip.right, m_lVerticalScroll );
			::SelectObject( hDC, penOld );
			::DeleteObject( penTopLine );
		}
	}
	else if (sv == SV_MINIMIZED)
	{
		// Compute the top and bottom notes to display so we can scale the display accordingly
		BYTE bBottomNote = 127;
		m_bMinimizeTopNote = 0;
		CDirectMusicEventItem* pEvent = GetFirstNote();
		for( ; pEvent; pEvent = pEvent->GetNext() )
		{
			if ( pEvent->m_dwVariation & m_dwVariations )
			{
				// Assume the m_bMIDIValue is up-to-date (it better be)
				if ( m_bMinimizeTopNote < ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue )
				{
					m_bMinimizeTopNote = ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue;
				}
				if ( bBottomNote > ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue )
				{
					bBottomNote = ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue;
				}
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
	}

	// Draw bar, beat, and grid lines
	DrawVerticalines( hDC, lXOffset );

	// invertrect selected time
	if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) &&
		(m_lEndTimelineSelection >=0))
	{
		long beginPos, endPos;
		RECT rectInvert;
		
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_lBeginTimelineSelection, &beginPos );
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_lEndTimelineSelection, &endPos );
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

	// Compute the first and last times displayed
	MUSIC_TIME mtStartTime, mtEndTime;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &mtStartTime );
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &mtEndTime );

	// Compute the length of the part, in ticks
	const long lPartClockLength = m_pPartRef->m_pDMPart->GetClockLength();

	// If we are asked to draw a time that is beyond the end of our part, and
	// the part is shorter than the Timeline, then we should loop
	const bool fLoopingPart = lPartClockLength < m_mtTimelineLengthForGetNoteRect;

	CDirectMusicStyleNote *pDMNote;
	if( fLoopingPart )
	{
		pDMNote = GetFirstVisibleNote( -m_lMeasureClocks + 1, mtEndTime, nTopNote, nBottomNote );
	}
	else
	{
		pDMNote = GetFirstVisibleNote( mtStartTime, mtEndTime, nTopNote, nBottomNote );
	}

	if (sv == SV_NORMAL)
	{
		if( m_fHybridNotation )
		{
			// Draw unselected notes
			DrawNotesHelper( FALSE, PRS_DT_HYBRID, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawHybridMaximizedNotes);

			// Draw selected notes
			DrawNotesHelper( TRUE, PRS_DT_HYBRID, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawHybridMaximizedNotes);

			// Draw accidentals
			DrawNotesHelper( TRUE, PRS_DT_ACCIDENTAL, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawHybridMaximizedAccidentals);
		}
		else
		{
			// Draw unselected notes
			DrawNotesHelper( FALSE, PRS_DT_MAXIMIZED, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawMaximizedNotes);

			// Draw selected notes
			DrawNotesHelper( TRUE, PRS_DT_MAXIMIZED, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawMaximizedNotes);
		}

		if( (this == m_pMIDIMgr->m_pActivePianoRollStrip) && (m_pMIDIMgr->m_iFocus == 1) && (m_lInsertVal <= nTopNote) && (m_lInsertVal >= nBottomNote) )
		{
			DrawNoteInsertionMark( hDC, mtStartTime, mtEndTime, lXOffset );
		}
	}
	else if( sv == SV_MINIMIZED )
	{
		// Draw unselected notes
		DrawNotesHelper( FALSE, PRS_DT_MINIMIZED, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawMinimizedNotes);

		// Draw selected notes
		DrawNotesHelper( TRUE, PRS_DT_MINIMIZED, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, fLoopingPart, DrawMinimizedNotes);
	}

	if( fLoopingPart )
	{
		// If the right side of the clipping region is after the end of the part, draw hatch marks

		// Compute the rectangle to display the hatch marks in
		RECT rectRepeat;
		rectRepeat.top = rectClip.top;
		rectRepeat.bottom = rectClip.bottom;
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_pPartRef->m_pDMPart->GetClockLength(), &rectRepeat.left );
		rectRepeat.left = max( rectRepeat.left - lXOffset, rectClip.left );
		rectRepeat.right = rectClip.right;

		// draw it
		if ((rectRepeat.left != rectRepeat.right) && (rectRepeat.top != rectRepeat.bottom))
		{
			HBRUSH brushRepeatHatch;
			brushRepeatHatch = ::CreateHatchBrush( HS_DIAGCROSS, ::GetNearestColor(hDC, RGB(50,50,50)) );
			if( brushRepeatHatch )
			{
				int nOldBackgroundMode = ::SetBkMode( hDC, TRANSPARENT );
				HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, brushRepeatHatch );
				COLORREF crOldBkColor = ::SetBkColor( hDC, 0 );
				::PatBlt( hDC, rectRepeat.left, rectRepeat.top, rectRepeat.right - rectRepeat.left, rectRepeat.bottom - rectRepeat.top, PATINVERT );
				::SetBkColor( hDC, crOldBkColor );
				::SetBkMode( hDC, nOldBackgroundMode );
				::SelectObject( hDC, hOldBrush );
				::DeleteObject( brushRepeatHatch );
			}
		}
	}

	::RestoreDC( hDC, iSavedDC );

	if (sv == SV_NORMAL)
	{
		DrawVariationButtonBar(hDC);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case PRIVATE_SP_CLSID:
		// CLSID used to identify type of strip
		if( (pvar->vt == VT_BYREF)
		&&  (V_BYREF(pvar) != NULL) )
		{
			CLSID* pCLSID = (CLSID *)V_BYREF(pvar);
		   *pCLSID = CLSID_PianoRollStrip;
		}
		else
		{
			return E_FAIL;
		}
		break;
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
		V_INT(pvar) = m_lMaxNoteHeight * (m_fHybridNotation ? 38 : 128) - BORDER_VERT_WIDTH + 2 * VARIATION_BUTTON_HEIGHT;
		break;
	case SP_MINHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = (2 * VARIATION_BUTTON_HEIGHT);
		break;
	case SP_MINIMIZE_HEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = MINIMIZE_HEIGHT;
		break;
	case SP_NAME:
		if( ValidPartRefPtr() )
		{
			BSTR bstr;
			pvar->vt = VT_BSTR;
			try
			{
				bstr = GetName().AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;
	case MSP_PIANOROLL_VARIATIONS:
		pvar->vt = VT_UI4;
		V_UI4(pvar) = m_dwVariations;
		break;
	case MSP_PIANOROLL_TRACK:
		if( ValidPartRefPtr() )
		{
			pvar->vt = VT_I4;
			V_I4(pvar) = m_pPartRef->m_dwPChannel;
		}
		break;
	case MSP_PIANOROLL_GUID:
		if( ValidPartRefPtr() )
		{
			GUID *pGuid = (GUID *)V_BYREF(pvar);
			if( pGuid )
			{
				*pGuid = m_pPartRef->m_pDMPart->m_guidPartID;
			}
			else
			{
				return E_INVALIDARG;
			}
		}
		break;

	case SP_CURSOR_HANDLE:
		pvar->vt = VT_I4;
		V_I4(pvar) = (int) m_hCursor;
		break;

	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pMIDIMgr )
		{
			m_pMIDIMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		}
		else
		{
			V_UNKNOWN(pvar) = NULL;
		}
		break;

	case SP_FUNCTIONBAR_EXCLUDE_WIDTH:
		pvar->vt = VT_I4;
		V_I4(pvar) = m_BitmapZoomInUp.GetBitmapDimension().cx;
		break;

	case SP_EARLY_TIME:
		m_fPickupBar = ShouldDisplayPickupBar();
		if( m_fPickupBar )
		{
			// Display a pick-up bar
			pvar->vt = VT_I4;
			V_I4(pvar) = m_lMeasureClocks;
			m_mtLastEarlyTime = m_lMeasureClocks;
			return S_OK;
		}

		// No early notes
		m_mtLastEarlyTime = 0;
		return E_FAIL;
		break;

	case SP_LATE_TIME:
		{
			m_dwExtraBars = GetNumExtraBars();

			VARIANT varLength;
			if( m_dwExtraBars
			&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
			{
				long lTimelineLength = V_I4( &varLength );

				// Compute the length of the pattern, in ticks
				const long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();
				lTimelineLength %= lPatternLength;
				if( lTimelineLength == 0 )
				{
					lTimelineLength = lPatternLength;
				}

				// Compute the start time of the last visible measure
				lTimelineLength %= m_lMeasureClocks;
				lTimelineLength = V_I4( &varLength ) - lTimelineLength;

				// Now, add on the number of extra bars
				lTimelineLength += m_dwExtraBars * m_lMeasureClocks;

				// Found a late note
				pvar->vt = VT_I4;
				V_I4(pvar) = lTimelineLength - V_I4( &varLength );
				m_mtLastLateTime = V_I4(pvar);
				return S_OK;
			}
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

HRESULT STDMETHODCALLTYPE CPianoRollStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
	{
		long lNewSelection;
		BOOL fChanged;
		
		fChanged = FALSE;

		// Validate parameter
		if( var.vt != VT_I4 )
		{
			return E_FAIL;
		}
		if( !ValidPartRefPtr() )
		{
			return E_FAIL;
		}

		// Set selection
		lNewSelection = V_I4( &var );
		if( sp == SP_BEGINSELECT )
		{
			if( m_lBeginTimelineSelection != lNewSelection )
			{
				m_lBeginTimelineSelection = lNewSelection;
				fChanged = TRUE;
			}
		}
		else
		{
			if( m_lEndTimelineSelection != lNewSelection )
			{
				m_lEndTimelineSelection = lNewSelection;
				fChanged = TRUE;
			}
		}

		// Update our curve strips with the new selection
		UpdateCurveStripGutterSelection( fChanged );

		// Update variation switch strip with the new selection
		m_pVarSwitchStrip->OnGutterSelectionChange( fChanged );

		// Exit early if we're just clearing the Timeline selection
		if( m_pPartRef->m_pDMPart->m_fSelecting && (m_pPartRef->m_pDMPart->m_bSelectionCC == 0xFF) )
		{
			if( m_fGutterSelected )
			{
				// Redraw this piano roll
				InvalidatePianoRoll();
			}
			break;
		}

		// If start time == end time, deselect everything
		if( m_lBeginTimelineSelection == m_lEndTimelineSelection )
		{
			if ( m_prsSelecting != PRS_NO_SELECT )
			{
				// This will refresh the property page (if needed) and all tracks
				// that display this part and variations.
				m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, m_dwVariations );
			}
			break;
		}

		if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0))
		{
			int nSelected;
			// This may be true if only curves were selected, but we'd need to refresh the part display anyways
			if( SelectEventsBetweenTimes( m_lBeginTimelineSelection, m_lEndTimelineSelection, &nSelected ) ) 
			{
				m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations, TRUE, TRUE );
				m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
				m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
			}

			// Redraw this piano roll
			InvalidatePianoRoll();
		}
		break;
	}

	case SP_GUTTERSELECT:
		if( m_fGutterSelected != V_BOOL(&var) )
		{
			if( !ValidPartRefPtr() )
			{
				return E_FAIL;
			}

			m_fGutterSelected = V_BOOL(&var);

			// Make sure timeline is in sync because change may be passed through from Curve strip
			if( m_pMIDIMgr->m_pTimeline )
			{
				var.vt = VT_BOOL;
				V_BOOL(&var) = (short)m_fGutterSelected;
				m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_GUTTER_SELECTED, var );
			}
			
			// Update our curve strips with the new selection state
			UpdateCurveStripGutterSelection( TRUE );

			// Update variation switch strip with the new selection
			m_pVarSwitchStrip->OnGutterSelectionChange( TRUE );

			if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0))
			{
				int nSelected;
				// This may be true if only curves were selected, but we'd need to refresh the part display anyways
				if( SelectEventsBetweenTimes( m_lBeginTimelineSelection, m_lEndTimelineSelection, &nSelected ) ) 
				{
					m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations, TRUE, TRUE );
					m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
					m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
				}
			}
			else
			{
				m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, m_dwVariations );
			}

			// Redraw this piano roll
			InvalidatePianoRoll();
		}
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

CDirectMusicStyleNote* CPianoRollStrip::GetDMNoteAndRectFromPoint( long lXPos, long lYPos, RECT *pRectSave )
{
	RECT rectNote, rectSave;

	CDirectMusicStyleNote *pNoteSave = NULL;
	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr != NULL)
	{
		// Initialize m_mtTimelineLengthForGetNoteRect
		{
			VARIANT varLength;
			if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
			{
				m_mtTimelineLengthForGetNoteRect = V_I4( &varLength );
			}
		}

		// Compute the length of a part
		long lPartLength = min( m_mtTimelineLengthForGetNoteRect, m_pPartRef->m_pDMPart->GetClockLength());

		// Calculate the number of grids to search through
		int nGridLength = CLOCKS_TO_GRID( lPartLength + m_lGridClocks - 1, m_pPartRef->m_pDMPart );

		// See which MIDINote grid lYPos is in
		// TODO: Optimize this for Hybrid notation
		// Also optimize in SequenceStrip.cpp
		int nMIDINote = 127 - (lYPos / m_lMaxNoteHeight);

		// Compute the time of the point asked far
		MUSIC_TIME mtPointTime;

		{
			// Just look at the visible notes
			m_pMIDIMgr->m_pTimeline->PositionToClocks( lXPos, &mtPointTime );

			// Compute the width of a pixel
			MUSIC_TIME mtPixelWidth;
			m_pMIDIMgr->m_pTimeline->PositionToClocks( 1, &mtPixelWidth );
			// This is a faster way of doing AbsTime() - (mtPixelWidth / 2) - 1 < mtPointTime
			mtPointTime += (mtPixelWidth / 2) + 1;
		}

		// Compute the length of the pattern
		long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

		// If we are asked to find a time that is beyond the end of our part (and our part is shorter
		// than the timeline), then we should loop.  (lPartLength will never be more than lPatternLength)
		bool fLoopingPart = lPartLength < m_mtTimelineLengthForGetNoteRect;

		CDirectMusicEventItem* pEvent = GetFirstNote();
		for( ; pEvent; pEvent = pEvent->GetNext() )
		{
			if( !fLoopingPart && (m_pPartRef->m_pDMPart->AbsTime( pEvent ) > mtPointTime) )
			{
				// Gone too far in list - no need to check further
				break;
			}

			// Check to see if:
			//    it belongs to the correct variation
			//    it starts before the end of this part (or pattern or timeline, if the pattern or timeline is shorter)
			if( !( pEvent->m_dwVariation & m_dwVariations ) ||
				(pEvent->m_mtGridStart >= nGridLength) )
			{
				continue;
			}

			// Check to see if:
			//    its MIDI value is the one we're looking for
			if( !m_fHybridNotation && (((CDirectMusicStyleNote *)pEvent)->m_bMIDIValue != nMIDINote) )
			{
				continue;
			}

			GetNoteRect( (CDirectMusicStyleNote *)pEvent, &rectNote );

			// if yPos is above or below the note, continue
			if( ( lYPos < rectNote.top ) || ( lYPos > rectNote.bottom ) )
			{
				continue;
			}

			/*
			// Currently a note that's one pixel high can be selected over two vertical
			// pixels (rectNote.top and rectNote.bottom).
			// If we need to expand the vertical selection area of a note that's one pixel high,
			// change the above code to just check against rectNote.bottom, and uncomment the below
			// code.
			
			// if lYPos is above the note and if the note is only one pixel high lYPos is not just above the note 
			if( lYPos < rectNote.top )
			{
				if( (rectNote.bottom - rectNote.top != 1)
				||	(lYPos != rectNote.top - 1) )
				{
					continue;
				}
			}
			*/

			// If lXPos is inside the note, save it
			if( ( lXPos >= rectNote.left ) && ( lXPos <= rectNote.right ) )
			{
				if( !pNoteSave )
				{
					pNoteSave = (CDirectMusicStyleNote *)pEvent;
					rectSave = rectNote;
					continue;
				}
				else
				{
					// Use the selected note
					if( pEvent->m_fSelected && !pNoteSave->m_fSelected ) 
					{
						pNoteSave = (CDirectMusicStyleNote *)pEvent;
						rectSave = rectNote;
						continue;
					}

					// If selection state is the same
					if( pNoteSave->m_fSelected == pEvent->m_fSelected )
					{
						// If start time is the same
						if( m_pPartRef->m_pDMPart->AbsTime( pEvent ) == m_pPartRef->m_pDMPart->AbsTime( pNoteSave ) )
						{
							// Use note with least velocity
							if ( ((CDirectMusicStyleNote *)pEvent)->m_bVelocity <= pNoteSave->m_bVelocity ) 
							{
								pNoteSave = (CDirectMusicStyleNote *)pEvent;
								rectSave = rectNote;
								continue;
							}
						}
						else
						{
							// Use note with later start time
							pNoteSave = (CDirectMusicStyleNote *)pEvent;
							rectSave = rectNote;
							continue;
						}
					}
				}
			}
			// Not in first copy of note - check other ones if this is a looping part
			else if( fLoopingPart )
			{
				// Compute the note start time
				const MUSIC_TIME mtOrigStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );

				// Use the note's duration for the length
				const MUSIC_TIME mtOrigEnd = mtOrigStart + ((CDirectMusicStyleNote *)pEvent)->m_mtDuration;

				// Compute the time of the note's grid position
				const MUSIC_TIME mtOrigGridStart = GRID_TO_CLOCKS( pEvent->m_mtGridStart, m_pPartRef->m_pDMPart );

				bool fDisplayNoteInShortPart;
				const MUSIC_TIME mtLastPartLen = lPatternLength % lPartLength;

				// If the part repeats, and the last repeat of the the part is less than the part's length
				if( mtLastPartLen )
				{
					long lLastGrid = CLOCKS_TO_GRID(mtLastPartLen - 1, m_pPartRef->m_pDMPart);

					// If the note will not play in the last part repeat
					if( pEvent->m_mtGridStart >= lLastGrid )
					{
						fDisplayNoteInShortPart = false;
					}
					else
					{
						fDisplayNoteInShortPart = true;
					}
				}
				else
				{
					fDisplayNoteInShortPart = true;
				}

				MUSIC_TIME mtStart = 0, mtEnd = 0, mtOffset = 0;
				bool fFoundEnd = false;
				long lStartPosition = LONG_MIN, lEndPosition = LONG_MIN;
				const long lNumPartRepeats = (lPatternLength - 1) / lPartLength;

				while( mtOffset < m_mtTimelineLengthForGetNoteRect )
				{
					const MUSIC_TIME mtOffsetOrig = mtOffset;
					for( int i=0; i <= lNumPartRepeats; i++ )
					{
						// If we're not in the last part repeat, or if the note displays in the
						// last (short) part repeat
						if( (i != lNumPartRepeats)
						||	fDisplayNoteInShortPart )
						{
							// Check that the note will actually play
							if( mtOffset + mtOrigGridStart < m_mtTimelineLengthForGetNoteRect )
							{
								// Compute the start and end positions of the note
								mtStart = mtOffset + mtOrigStart;
								m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtStart, &lStartPosition );
								mtEnd = mtOffset + mtOrigEnd;
								m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtEnd, &lEndPosition );

								// Ensure the end position is at least the pixel after the start position
								lEndPosition = max( lEndPosition, lStartPosition + 1);
							}
						}
						if( lEndPosition >= lXPos )
						{
							fFoundEnd = true;
							break;
						}
						mtOffset += lPartLength;
					}
					if( fFoundEnd )
					{
						break;
					}
					mtOffset = mtOffsetOrig + lPatternLength;
				}

				if( fFoundEnd )
				{
					if( lStartPosition <= lXPos )
					{
						rectNote.left = lStartPosition;
						rectNote.right = lEndPosition;
						if( !pNoteSave )
						{
							pNoteSave = (CDirectMusicStyleNote *)pEvent;
							rectSave = rectNote;
							continue;
						}
						else
						{
							// Use the selected note
							if( pEvent->m_fSelected && !pNoteSave->m_fSelected ) 
							{
								pNoteSave = (CDirectMusicStyleNote *)pEvent;
								rectSave = rectNote;
								continue;
							}

							// If selection state is the same
							if( pNoteSave->m_fSelected == pEvent->m_fSelected )
							{
								// If start time is the same
								if( m_pPartRef->m_pDMPart->AbsTime( pEvent ) == m_pPartRef->m_pDMPart->AbsTime( pNoteSave ) )
								{
									// Use note with least velocity
									if ( ((CDirectMusicStyleNote *)pEvent)->m_bVelocity <= pNoteSave->m_bVelocity ) 
									{
										pNoteSave = (CDirectMusicStyleNote *)pEvent;
										rectSave = rectNote;
										continue;
									}
								}
								else
								{
									// Use note with later start time
									pNoteSave = (CDirectMusicStyleNote *)pEvent;
									rectSave = rectNote;
									continue;
								}
							}
						}
					}
				}
			}
		}
	}

	if( pRectSave && pNoteSave )
	{
		memcpy( pRectSave, &rectSave, sizeof(RECT) );
	}

	return pNoteSave;
}

void CPianoRollStrip::SetMouseMode( long lXPos, long lYPos)
{
	if (m_MouseMode == PRS_MM_ACTIVESELECT_VARIATIONS)
	{
		return;
	}

	CDirectMusicStyleNote* pDMNote = NULL;
	RECT rectNote; // left, right, top, bottom
	pDMNote = GetDMNoteAndRectFromPoint( lXPos, lYPos, &rectNote );
	m_MouseMode = PRS_MM_NORMAL;
	if( pDMNote )
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
				m_MouseMode = PRS_MM_RESIZE_END;
			}
			else if ( lXPos - rectNote.left <= lWidth )
			{
				m_MouseMode = PRS_MM_RESIZE_START;
			}
			else if ( lYPos <= rectNote.top + lHeight/2 )
			{
				m_MouseMode = PRS_MM_RESIZE_VELOCITY;
			}
			else
			{
				m_MouseMode = PRS_MM_MOVE;
			}
		}
		else //if ( lWidth <= 2 )
		{
			if ( lXPos >= rectNote.right ) //- lWidth/4 is always 0
			{
				m_MouseMode = PRS_MM_RESIZE_END;
			}
			// If the note is just one pixel high, then this is true when the mouse is on the note and
			// PRS_MM_MOVE will be set when the mouse is just below the note.
			else if ( lYPos <= rectNote.top + lHeight/2 )
			{
				m_MouseMode = PRS_MM_RESIZE_VELOCITY;
			}
			else
			{
				m_MouseMode = PRS_MM_MOVE;
			}
		}
	}
}

HRESULT CPianoRollStrip::OnLButtonDown( WPARAM wParam, long lXPos, long lYPos)
{
	UpdateNoteCursorTime();

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	UnselectGutterRange();

	if (m_StripView == SV_MINIMIZED)
	{
		return S_OK;
	}

	CDirectMusicStyleNote* pDMNote = NULL;
	HRESULT hr = S_OK;
	VARIANT var;

	pDMNote = GetDMNoteAndRectFromPoint( lXPos, lYPos, NULL );
	if( pDMNote != NULL )
	{
		// Update the insert cursor position
		/*
		if( m_lInsertVal != pDMNote->m_bMIDIValue )
		{
			m_lInsertVal = pDMNote->m_bMIDIValue;
			InvalidatePianoRoll();
		}
		*/

		if (wParam & MK_CONTROL)
		{
			m_pActiveDMNote = pDMNote;
			if( pDMNote->m_fSelected )
			{
				m_fCtrlKeyDown = 1;
			}
			else
			{
				m_fCtrlKeyDown = 2;
				// Select
				m_pShiftDMNote = pDMNote;
				m_pMIDIMgr->SelectNote( this, pDMNote );
			}
		}
		else if (wParam & MK_SHIFT)
		{
			m_pActiveDMNote = pDMNote;
			int nSelected;
			RECT rect1, rect2;

			if ( m_pShiftDMNote == NULL )
			{
				m_pShiftDMNote = pDMNote;
			}

			// Initialize m_mtTimelineLengthForGetNoteRect
			{
				VARIANT varLength;
				if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
				{
					m_mtTimelineLengthForGetNoteRect = V_I4( &varLength );
				}
			}

			// BUGBUG: This doesn't work with looped parts
			GetNoteRect( m_pShiftDMNote, &rect1 );
			GetNoteRect( pDMNote, &rect2 );

			// The '+1' is because CRect::PtInRect makes the left and top side 'inside' the rect, but the
			// right and bottom sides are 'outside' the rect.  By adding one to the right and bottom, the
			// real edge is now 'inside' the rect.
			CRect rect( min(rect1.left, rect2.left), min(rect1.top, rect2.top), max(rect1.left, rect2.left) + 1, max(rect1.bottom, rect2.bottom) + 1);
			
			if( SelectNotesInRect( &rect, &nSelected ) )
			{
				// If something changed, invalidate the strip
				m_pMIDIMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, TRUE);
			}
		}
		else if (pDMNote->m_fSelected == FALSE)
		{
			m_pMIDIMgr->StopDisplayUpdate();
			m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );
			m_pMIDIMgr->StartDisplayUpdate();
			m_pMIDIMgr->SelectNote( this, pDMNote );
			m_pActiveDMNote = pDMNote;
			m_pShiftDMNote = pDMNote;
		}
		else
		{
			// The note's already selected, why re-select it? -jd
			//m_pMIDIMgr->SelectNote( pDMNote );
			m_pActiveDMNote = pDMNote;
			m_pShiftDMNote = pDMNote;
		}
		
		// We don't need to refresh our display and the note property page
		// because the MIDIMgr should take care of it automatically.
		m_pMIDIMgr->OnShowProperties();

		// Set the focus back to the timeline
		// Removed, not needed anymore -jd

		// Play the note
		CPropNote propNote;
		propNote.m_mtDuration =  pDMNote->m_mtDuration;
		propNote.m_bVelocity = pDMNote->m_bVelocity;
		propNote.m_bMIDIValue = pDMNote->m_bMIDIValue;
		propNote.m_mtGridStart = pDMNote->m_mtGridStart;
		propNote.m_nOffset = pDMNote->m_nTimeOffset;
		propNote.SetPart( m_pPartRef->m_pDMPart );
		m_pMIDIMgr->PlayNote( &propNote, m_pPartRef->m_dwPChannel );
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
			// Insert note

			// Make sure everything on the timeline is deselected first.
			UnselectGutterRange();

			pDMNote = CreateNoteToInsert();
			if( pDMNote != NULL )
			{
				m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, m_dwVariations );

				// Don't do the update now
				BOOL fOldInsertionFlag = m_pMIDIMgr->m_fNoteInserted;
				m_pMIDIMgr->InsertNote( m_pPartRef, pDMNote, FALSE );
				m_pMIDIMgr->m_fNoteInserted = fOldInsertionFlag;
				m_fInsertingNoteFromMouseClick = true;

				// Update m_prsSelecting
				m_pActiveDMNote = pDMNote;

				// Refresh note property page
				m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
				m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );

				// Bump Time cursor to the right
				SNAPTO snapTo = GetSnapToBoundary();
				if( snapTo == SNAP_NONE )
				{
					snapTo = SNAP_GRID;
				}
				BumpTimeCursor( TRUE, snapTo );

				// Set the mouse mode for duration resize
				m_MouseMode = PRS_MM_RESIZE_END;

				m_lLastDeltaDur = 0;
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
	case PRS_MM_NORMAL:
		if (!(wParam & MK_CONTROL))
		{
			m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );
			m_pActiveDMNote = NULL;
			m_prsSelecting = PRS_NO_SELECT;

			// Create bounding box
			IOleWindow* pIOleWindow;
			m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
			if( pIOleWindow )
			{
				CWnd wnd;
				HWND hWnd;

				if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
				{
					CPoint point( lXPos, lYPos + VARIATION_BUTTON_HEIGHT * 2);

					if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &point ) ) )
					{
						wnd.Attach( hWnd );
						CNoteTracker noteTracker( &wnd, m_pMIDIMgr->m_pTimeline, this, NTRK_SELECTING );
						noteTracker.TrackRubberBand( &wnd, point, TRUE );
						wnd.Detach();
					}
				}

				pIOleWindow->Release();
			}
		}

		// leave m_pShiftDMNote at whatever it last was
		//m_pShiftDMNote = NULL;
		
		// These updates already taken care of by the MIDIMgr
		// update our display and the property page
		//m_pMIDIMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, TRUE);
		m_pMIDIMgr->OnShowProperties();
		break;
	case PRS_MM_MOVE:
		if (pDMNote)
		{
//			m_OriginalMNote = *pDMNote;
			m_OriginalPropNote.ImportFromDMNote(pDMNote, m_pPartRef);
			m_lLastDeltaStart = 0;
			m_cLastDeltaValue = 0;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = PRS_MM_ACTIVEMOVE;
		}
		break;
	case PRS_MM_RESIZE_START:
		if (pDMNote)
		{
//			m_OriginalMNote = *pDMNote;
			m_OriginalPropNote.ImportFromDMNote(pDMNote, m_pPartRef);
			m_lLastDeltaStart = 0;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = PRS_MM_ACTIVERESIZE_START;
		}
		break;
	case PRS_MM_RESIZE_END:
		if (pDMNote)
		{
//			m_OriginalMNote = *pDMNote;
			m_OriginalPropNote.ImportFromDMNote(pDMNote, m_pPartRef);
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = PRS_MM_ACTIVERESIZE_END;
			m_lLastDeltaDur = 0;
		}
		break;
	case PRS_MM_RESIZE_VELOCITY:
		if (pDMNote)
		{
//			m_OriginalMNote = *pDMNote;
			m_OriginalPropNote.ImportFromDMNote(pDMNote, m_pPartRef);
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			m_MouseMode = PRS_MM_ACTIVERESIZE_VELOCITY;
			m_lLastDeltaVel = 0;
		}
		break;
	case PRS_MM_ACTIVERESIZE_VELOCITY:
	case PRS_MM_ACTIVERESIZE_END:
	case PRS_MM_ACTIVERESIZE_START:
	case PRS_MM_ACTIVEMOVE:
	case PRS_MM_ACTIVESELECT_VARIATIONS:
		// This shouldn't happen...
		ASSERT(FALSE);
		break;
	default:
		break;
	}

	m_pMIDIMgr->UpdateStatusBarDisplay();
	return hr;
}

HRESULT CPianoRollStrip::OnMouseMove( long lXPos, long lYPos)
{
	ASSERT( m_pPartRef != NULL );

	if (m_StripView == SV_MINIMIZED)
	{
		return S_OK;
	}

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	long xDelta, yDelta;
	
	switch(m_MouseMode)
	{
	case PRS_MM_ACTIVERESIZE_VELOCITY:
		// TODO: Optimize for single note selection
		// compute the new velocity
		yDelta = m_pointClicked.y - lYPos;
		yDelta = long((double)yDelta/m_dblVerticalZoom);

		// Update the note(s) if the velocity changed
		if( (yDelta / 2) - m_lLastDeltaVel != 0 )
		{
			BYTE bOrigVelocity = 0;
			if( m_pActiveDMNote )
			{
				bOrigVelocity = m_pActiveDMNote->m_bVelocity;
			}

			if( m_pMIDIMgr->OffsetSelectedNoteDurationAndVelocity( m_pPartRef, m_dwVariations, 0, (yDelta / 2) - m_lLastDeltaVel, FALSE ) )
			{
				m_lLastDeltaVel = yDelta / 2;

				if( m_pActiveDMNote
				&&	(bOrigVelocity != m_pActiveDMNote->m_bVelocity) )
				{
					CPropNote propNote;
					propNote.m_mtDuration =  m_pActiveDMNote->m_mtDuration;
					propNote.m_bVelocity = m_pActiveDMNote->m_bVelocity;
					propNote.m_bMIDIValue = m_pActiveDMNote->m_bMIDIValue;
					propNote.m_mtGridStart = m_pActiveDMNote->m_mtGridStart;
					propNote.m_nOffset = m_pActiveDMNote->m_nTimeOffset;
					propNote.SetPart( m_pPartRef->m_pDMPart );
					m_pMIDIMgr->PlayNote( &propNote, m_pPartRef->m_dwPChannel );
				}
			}
		}
		break;

	case PRS_MM_ACTIVERESIZE_END:
		// TODO: Optimize for single note selection
		if ( (m_prsSelecting == PRS_MULTIPLE_SELECT) ||
			 (m_prsSelecting == PRS_SINGLE_SELECT) && m_pActiveDMNote)
		{
			// TODO: Optimize for single note selection
			// compute the original and new end points
			long lTemp1, lTemp2;
			m_pMIDIMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pMIDIMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);

			// compute the change
			xDelta = lTemp2 - lTemp1;

			// Update the note(s) if the duration changed
			if( xDelta - m_lLastDeltaDur != 0 )
			{
				// scroll if necessary
				AdjustScroll(lXPos, lYPos);

				if( m_pMIDIMgr->OffsetSelectedNoteDurationAndVelocity( m_pPartRef, m_dwVariations, xDelta - m_lLastDeltaDur, 0, FALSE ) )
				{
					m_lLastDeltaDur = xDelta;
				}
			}
		}
		break;

	case PRS_MM_ACTIVERESIZE_START:
		// TODO: Optimize for single note selection
		if ( (m_prsSelecting == PRS_SINGLE_SELECT) || (m_prsSelecting == PRS_MULTIPLE_SELECT) )
		{
			// compute the original and new start points
			long lTemp1, lTemp2;
			m_pMIDIMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pMIDIMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);
			
			// compute the change
			long lNewDeltaStart;
			xDelta = abs(lTemp2 - lTemp1);

			// Get the snap amound
			long lSnapAmount = GetSnapAmount();

			// Quantize lNewDeltaStart to lSnapAmount
			if( lSnapAmount > 1 )
			{
				lNewDeltaStart = lSnapAmount * ((xDelta + (lSnapAmount / 2)) / lSnapAmount);
				lNewDeltaStart *= lTemp2 > lTemp1 ? 1 : -1;
			}
			else
			{
				lNewDeltaStart = lTemp2 - lTemp1;
			}
			
			// lDeltaStart is in units of ticks, and is the amount we're moving the start time
			long lDeltaStart = lNewDeltaStart - m_lLastDeltaStart;
			long lDeltaGridStart = (lNewDeltaStart / m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);

			if( lDeltaStart != 0 )
			{
				int nGridLength = m_pPartRef->m_pDMPart->GetGridLength();

				CDirectMusicEventItem* pEvent = GetFirstNote();

				for( ; pEvent; pEvent = pEvent->GetNext() )
				{
					if ( ( pEvent->m_dwVariation & m_dwVariations ) &&
						( pEvent->m_fSelected == TRUE ) )
					{
						// Make sure the start time doesn't go out of bounds
						if ( pEvent->m_mtGridStart + lDeltaGridStart < 0 )
						{
							if( lSnapAmount >= m_lGridClocks )
							{
								lDeltaStart = lSnapAmount * ((-pEvent->m_mtGridStart * m_lGridClocks) / lSnapAmount);
								lDeltaGridStart = lDeltaStart / m_lGridClocks;
							}
							else
							{
								lDeltaStart = ((m_lLastDeltaStart / m_lGridClocks) - pEvent->m_mtGridStart) * m_lGridClocks - (m_lGridClocks - 1) - m_lLastDeltaStart;
								lDeltaGridStart = -pEvent->m_mtGridStart;
							}
						}
						else if ( pEvent->m_mtGridStart + lDeltaGridStart >= nGridLength )
						{
							if( lSnapAmount >= m_lGridClocks )
							{
								lDeltaStart = lSnapAmount * (((nGridLength - 1 - pEvent->m_mtGridStart) * m_lGridClocks) / lSnapAmount);
								lDeltaGridStart = lDeltaStart / m_lGridClocks;
							}
							else
							{
								lDeltaStart = ((m_lLastDeltaStart / m_lGridClocks) + nGridLength - 1 - pEvent->m_mtGridStart) * m_lGridClocks + (m_lGridClocks - 1) - m_lLastDeltaStart;
								lDeltaGridStart = nGridLength - 1 - pEvent->m_mtGridStart;
							}
						}

						// Make sure the duration does not go below 1
						if( lDeltaStart >= ((CDirectMusicStyleNote *)pEvent)->m_mtDuration )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((((CDirectMusicStyleNote *)pEvent)->m_mtDuration - 1) / lSnapAmount);
							}
							else
							{
								lDeltaStart = ((CDirectMusicStyleNote *)pEvent)->m_mtDuration - 1;
							}
							lDeltaGridStart = ((m_lLastDeltaStart + lDeltaStart)/ m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
						}

						// Make sure the start time is at least one measure before the part
						// Now ensure lDeltaStart does not put the event more than one measure ahead of the part
						if( lDeltaStart + m_pPartRef->m_pDMPart->AbsTime(pEvent)
						<	-m_lMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((-m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent)) / lSnapAmount);
							}
							else
							{
								lDeltaStart = -m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent);
							}
							lDeltaGridStart = ((m_lLastDeltaStart + lDeltaStart)/ m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
						}

						// Now ensure lStartTime does not put the event more than one measure after the part
						if( lDeltaStart + m_pPartRef->m_pDMPart->AbsTime(pEvent)
						>	m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks )
						{
							if( lSnapAmount > 1 )
							{
								lDeltaStart = lSnapAmount * ((m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent)) / lSnapAmount);
							}
							else
							{
								lDeltaStart = m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent);
							}
							lDeltaGridStart = ((m_lLastDeltaStart + lDeltaStart)/ m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
						}
					}
				}

				// scroll if necessary
				AdjustScroll(lXPos, lYPos);

				if( lDeltaStart )
				{
					long lGridDiff = (lDeltaStart + m_lLastDeltaStart) / m_lGridClocks - m_lLastDeltaStart / m_lGridClocks;

					// If lGridDiff is 0, this does nothing
					lDeltaStart -= lGridDiff * m_lGridClocks;

					// This refreshes the property page and our strip, if the data changes
					if( m_pMIDIMgr->OffsetSelectedNoteStart( m_pPartRef, m_dwVariations, lDeltaStart, lGridDiff ) )
					{
						m_lLastDeltaStart += lDeltaStart + lGridDiff * m_lGridClocks;
					}
				}
			}
		}
		break;

	case PRS_MM_ACTIVEMOVE:
		// TODO: Optimize for single note selection
		if ( (m_prsSelecting == PRS_SINGLE_SELECT) || (m_prsSelecting == PRS_MULTIPLE_SELECT) )
		{
			// compute the original and new start points
			long lTemp1, lTemp2;
			m_pMIDIMgr->m_pTimeline->PositionToClocks(m_pointClicked.x,&lTemp1);
			m_pMIDIMgr->m_pTimeline->PositionToClocks(lXPos,&lTemp2);
			
			// compute the change
			long lNewDeltaStart;
			xDelta = abs(lTemp2 - lTemp1);

			// Get the snap amound
			long lSnapAmount = GetSnapAmount();

			// Quantize lNewDeltaStart to lSnapAmount
			if( lSnapAmount > 1 )
			{
				lNewDeltaStart = lSnapAmount * ((xDelta + (lSnapAmount / 2)) / lSnapAmount);
				lNewDeltaStart *= lTemp2 > lTemp1 ? 1 : -1;
			}
			else
			{
				lNewDeltaStart = lTemp2 - lTemp1;
			}

			// lDeltaStart is in units of ticks, and is the amount we're moving the note
			long lDeltaStart = lNewDeltaStart - m_lLastDeltaStart;
			long lDeltaGridStart = (lNewDeltaStart / m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);

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
				// Always do this if lDeltaStart || lDeltaValue
				//if( lDeltaGridStart || lDeltaValue )
				{
					int nGridLength = m_pPartRef->m_pDMPart->GetGridLength();

					CDirectMusicEventItem* pEvent = GetFirstNote();

					for( ; pEvent; pEvent = pEvent->GetNext() )
					{
						if ( ( pEvent->m_dwVariation & m_dwVariations ) &&
							( pEvent->m_fSelected == TRUE ) )
						{
							// Make sure the start time doesn't go out of bounds
							if ( pEvent->m_mtGridStart + lDeltaGridStart < 0 )
							{
								if( lSnapAmount >= m_lGridClocks )
								{
									lDeltaStart = lSnapAmount * ((-pEvent->m_mtGridStart * m_lGridClocks) / lSnapAmount);
									lDeltaGridStart = lDeltaStart / m_lGridClocks;
								}
								else
								{
									lDeltaStart = ((m_lLastDeltaStart / m_lGridClocks) - pEvent->m_mtGridStart) * m_lGridClocks - (m_lGridClocks - 1) - m_lLastDeltaStart;
									lDeltaGridStart = -pEvent->m_mtGridStart;
								}
							}
							else if ( pEvent->m_mtGridStart + lDeltaGridStart >= nGridLength )
							{
								if( lSnapAmount >= m_lGridClocks )
								{
									lDeltaStart = lSnapAmount * (((nGridLength - 1 - pEvent->m_mtGridStart) * m_lGridClocks) / lSnapAmount);
									lDeltaGridStart = lDeltaStart / m_lGridClocks;
								}
								else
								{
									lDeltaStart = ((m_lLastDeltaStart / m_lGridClocks) + nGridLength - 1 - pEvent->m_mtGridStart) * m_lGridClocks + (m_lGridClocks - 1) - m_lLastDeltaStart;
									lDeltaGridStart = nGridLength - 1 - pEvent->m_mtGridStart;
								}
							}

							// Make sure the start time is at least one measure before the part
							// Now ensure lDeltaStart does not put the event more than one measure ahead of the part
							if( lDeltaStart + m_pPartRef->m_pDMPart->AbsTime(pEvent)
							<	-m_lMeasureClocks )
							{
								if( lSnapAmount > 1 )
								{
									lDeltaStart = lSnapAmount * ((-m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent)) / lSnapAmount);
								}
								else
								{
									lDeltaStart = -m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent);
								}
								lDeltaGridStart = ((m_lLastDeltaStart + lDeltaStart)/ m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
							}

							// Now ensure lStartTime does not put the event more than one measure after the part
							if( lDeltaStart + m_pPartRef->m_pDMPart->AbsTime(pEvent)
							>	m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks )
							{
								if( lSnapAmount > 1 )
								{
									lDeltaStart = lSnapAmount * ((m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent)) / lSnapAmount);
								}
								else
								{
									lDeltaStart = m_pPartRef->m_pDMPart->GetClockLength() - 1 + m_lMeasureClocks - m_pPartRef->m_pDMPart->AbsTime(pEvent);
								}
								lDeltaGridStart = ((m_lLastDeltaStart + lDeltaStart)/ m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
							}

							// Make sure the value doesn't go out of bounds
							CDirectMusicStyleNote *pNote = ((CDirectMusicStyleNote *) pEvent);
							long lValue = pNote->m_bMIDIValue + lDeltaValue;

							// Check if the DMUS_PLAYMODE_CHORD_ROOT flag is set.
							/*
							BOOL bPlayModeFlags = (pNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE) ? m_pPartRef->m_pDMPart->m_bPlayModeFlags : pNote->m_bPlayModeFlags;
							if( bPlayModeFlags == DMUS_PLAYMODE_PEDALPOINT )
							{
								DMUS_CHORD_PARAM dmChordParam;
								if( SUCCEEDED( m_pMIDIMgr->GetChord( GRID_TO_CLOCKS( pEvent->m_mtGridStart + lDeltaGridStart, m_pPartRef->m_pDMPart ), &dmChordParam ) ) )
								{
									// DMUS_PLAYMODE_CHORD_ROOT is set, ensure bottom does not go below the chord root
									if( (dmChordParam.bSubChordCount > m_pPartRef->m_bSubChordLevel) &&
										 (lValue < dmChordParam.SubChordList[m_pPartRef->m_bSubChordLevel].bChordRoot) )
									{
										lDeltaValue += dmChordParam.SubChordList[m_pPartRef->m_bSubChordLevel].bChordRoot - lValue;
										lValue = pNote->m_bMIDIValue + lDeltaValue;
									}
								}
							}
							*/

							if( lValue < 0 )
							{
								// Ensure bottom does not go below 0
								lDeltaValue += -lValue;
								// Not necessary:
								//lValue = pNote->m_bMIDIValue + lDeltaValue;
							}

							else if ( lValue > 127 )
							{
								// Ensure top does not go above 127
								lDeltaValue += 127 - lValue;
							}
						}
					}
				}

				// Already handled above
				//lDeltaStart += (lDeltaGridStart - ((lNewDeltaStart / m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks))) * m_lGridClocks;

				// scroll if necessary
				AdjustScroll(lXPos, lYPos);

				if( lDeltaStart || lDeltaValue )
				{
					BYTE bOrigMIDIValue = 0;
					if( m_pActiveDMNote )
					{
						bOrigMIDIValue = m_pActiveDMNote->m_bMIDIValue;
					}

					// This refreshes the property page and our strip, if the data changes
					long lGridDiff = (lDeltaStart + m_lLastDeltaStart) / m_lGridClocks - m_lLastDeltaStart / m_lGridClocks;

					// If lGridDiff is 0, this does nothing
					lDeltaStart -= lGridDiff * m_lGridClocks;

					if( m_pMIDIMgr->OffsetSelectedNoteValuePosition( m_pPartRef, m_dwVariations, lDeltaStart, lGridDiff, lDeltaValue, m_fCtrlKeyDown ) )
					{
						m_lLastDeltaStart += lDeltaStart + lGridDiff * m_lGridClocks;
						m_cLastDeltaValue = char(m_cLastDeltaValue + lDeltaValue);
						if( m_fCtrlKeyDown )
						{
							m_fCtrlKeyDown = 0;
						}
					}

					if( m_pActiveDMNote && (bOrigMIDIValue != m_pActiveDMNote->m_bMIDIValue) )
					{
						CPropNote propNote;
						propNote.m_mtDuration =  m_pActiveDMNote->m_mtDuration;
						propNote.m_bVelocity = m_pActiveDMNote->m_bVelocity;
						propNote.m_bMIDIValue = m_pActiveDMNote->m_bMIDIValue;
						propNote.m_mtGridStart = m_pActiveDMNote->m_mtGridStart;
						propNote.m_nOffset = m_pActiveDMNote->m_nTimeOffset;
						propNote.SetPart( m_pPartRef->m_pDMPart );
						m_pMIDIMgr->PlayNote( &propNote, m_pPartRef->m_dwPChannel );
					}
				}
			}
		}
		break;

	case PRS_MM_NORMAL:
	case PRS_MM_MOVE:
	case PRS_MM_RESIZE_START:
	case PRS_MM_RESIZE_END:
	case PRS_MM_RESIZE_VELOCITY:
	case PRS_MM_ACTIVESELECT_VARIATIONS:
		break;

	default:
		break;
	}
	return S_OK;
}

HRESULT CPianoRollStrip::OnSetCursor( long lXPos, long lYPos)
{
	if (m_StripView == SV_MINIMIZED)
	{
		m_hCursor = GetArrowCursor();
		return S_OK;
	}

	SetMouseMode( lXPos, lYPos );
	switch(m_MouseMode)
	{
	case PRS_MM_ACTIVESELECT_VARIATIONS:
	case PRS_MM_NORMAL:
		m_hCursor = GetArrowCursor();
		break;
	case PRS_MM_ACTIVEMOVE:
	case PRS_MM_MOVE:
		m_hCursor = GetAllCursor();
		break;
	case PRS_MM_ACTIVERESIZE_START:
	case PRS_MM_RESIZE_START:
		m_hCursor = GetResizeStartCursor();
		break;
	case PRS_MM_ACTIVERESIZE_END:
	case PRS_MM_RESIZE_END:
		m_hCursor = GetResizeEndCursor();
		break;
	case PRS_MM_ACTIVERESIZE_VELOCITY:
	case PRS_MM_RESIZE_VELOCITY:
		m_hCursor = GetNSCursor();
		break;
	default:
		break;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnChar
//
// Message handler for strip resizing.
//
HRESULT CPianoRollStrip::OnChar(WPARAM wParam)
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
	if( m_StripView == SV_NORMAL )
	{
		// TODO: Undo Zoom?
		//CString strUndoText;

		if( (wParam == 'a') || (wParam == 'A') )
		{
			ChangeZoom(	m_dblVerticalZoom + 0.01 );

			//strUndoText.LoadString( IDS_UNDO_ZOOM_IN );
		}
		else if( (wParam == 'z') || (wParam == 'Z') )
		{
			if( m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
			{
				ChangeZoom(	m_dblVerticalZoom - 0.01 );

				//strUndoText.LoadString( IDS_UNDO_ZOOM_OUT );
			}
		}
		else if( (wParam == 'h') || (wParam == 'H') )
		{
			// This handles notifying the other strips, and adding an Undo state
			ChangeNotationType( !m_fHybridNotation, TRUE );
		}

		/*
		if( !strUndoText.IsEmpty() )
		{
			// Let the object know about the changes
			// No need to update performance engine
			m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
			m_pMIDIMgr->UpdateOnDataChanged( &strUndoText );
		}
		*/
	}

	return S_OK;
}

HRESULT CPianoRollStrip::OnKeyDown( WPARAM wParam, LPARAM /*lParam*/)
{
	ASSERT( m_pPartRef != NULL );

	if( (wParam == VK_INSERT)
	||	(wParam == VK_DELETE) )
	{
		if( (0x8000 & GetKeyState( VK_CONTROL ))
		&&	(0x8000 & GetKeyState( VK_SHIFT )) )
		{
			if( wParam == VK_INSERT )
			{
				return m_pMIDIMgr->AddNewStrip();
			}
			else
			{
				return m_pMIDIMgr->OnDeletePart( this );
			}
		}
	}

	switch(m_MouseMode)
	{
	case PRS_MM_NORMAL:
	case PRS_MM_MOVE:
	case PRS_MM_RESIZE_START:
	case PRS_MM_RESIZE_END:
	case PRS_MM_RESIZE_VELOCITY:
	case PRS_MM_ACTIVESELECT_VARIATIONS:
		// Up arrow / Down arrow / Page Up / Page Down
		if ( (wParam == VK_UP) || (wParam == VK_DOWN ) || (wParam == VK_PRIOR) || (wParam == VK_NEXT ) )
		{
			long lNewVertScroll = m_lVerticalScroll;
			long lNewInsertVal = m_lInsertVal;

			// scroll the piano roll up and down while moving the note cursor
			// DOWN arrow
			if (wParam == VK_DOWN)
			{
				if (m_StripView == SV_MINIMIZED)
				{
					return S_OK;
				}
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
						if( m_pMIDIMgr->m_fDisplayingFlats )
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleFlats);
							iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleFlats);
						}
						// SHARPS
						else
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleSharps);
							iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleSharps);
						}

						if( iNewHybridPos / 2 != iOldHybridPos / 2 )
						{
							lNewVertScroll += m_lMaxNoteHeight;
						}

						if ( lNewVertScroll / ( m_lMaxNoteHeight ) > 37)
						{
							lNewVertScroll = ( m_lMaxNoteHeight ) * 37;
						}
					}
					else
					{
						lNewVertScroll += m_lMaxNoteHeight;
						if ( lNewVertScroll / ( m_lMaxNoteHeight ) > 127)
						{
							lNewVertScroll = ( m_lMaxNoteHeight ) * 127;
						}
					}
				}
			}

			// UP arrow
			else if (wParam == VK_UP)
			{
				if (m_StripView == SV_MINIMIZED)
				{
					return S_OK;
				}
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
						if( m_pMIDIMgr->m_fDisplayingFlats )
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleFlats);
							iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleFlats);
						}
						// SHARPS
						else
						{
							iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleSharps);
							iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleSharps);
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
			else if (wParam == VK_NEXT)
			{
				if (m_StripView == SV_MINIMIZED)
				{
					return S_OK;
				}
				lNewInsertVal -= 12;
				if( lNewInsertVal < 0 )
				{
					lNewInsertVal = 0;
				}

				if( m_fHybridNotation )
				{
					int iNewHybridPos, iOldHybridPos;
					// FLATS
					if( m_pMIDIMgr->m_fDisplayingFlats )
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleFlats);
						iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleFlats);
					}
					// SHARPS
					else
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleSharps);
						iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleSharps);
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
			else if (wParam == VK_PRIOR)
			{
				if (m_StripView == SV_MINIMIZED)
				{
					return S_OK;
				}
				lNewInsertVal += 12;
				if( lNewInsertVal > 127 )
				{
					lNewInsertVal = 127;
				}

				if( m_fHybridNotation )
				{
					int iNewHybridPos, iOldHybridPos;
					// FLATS
					if( m_pMIDIMgr->m_fDisplayingFlats )
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleFlats);
						iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleFlats);
					}
					// SHARPS
					else
					{
						iNewHybridPos = MIDIToHybridPos( (BYTE)lNewInsertVal, aChromToScaleSharps);
						iOldHybridPos = MIDIToHybridPos( (BYTE)m_lInsertVal, aChromToScaleSharps);
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

			if (lNewVertScroll != m_lVerticalScroll)
			{
				SetNewVerticalScroll( lNewVertScroll );
			}

			// Redraw the strip
			if( lNewInsertVal != m_lInsertVal )
			{
				m_lInsertVal = lNewInsertVal;
				InvalidatePianoRoll();
			}
		}

		else if ( (wParam == VK_RIGHT) || (wParam == VK_LEFT ) )
		{
			// If we've inserted a note, add an undo state
			if( m_pMIDIMgr->m_fNoteInserted )
			{
				m_pMIDIMgr->UpdateOnDataChanged( IDS_EDIT_INSERT );
				// Handled by UpdateOnDataChanged
				//m_fNoteInserted = FALSE;
			}

			// Bump to the right if the user  hit the right arrow

			// If the Ctrl key is down, move by a measure
			bool fCtrlKeyUp = (GetKeyState( VK_CONTROL ) & 0x8000) == 0;
			BumpTimeCursor( wParam == VK_RIGHT, fCtrlKeyUp ? GetSnapToBoundary() : SNAP_BAR );
		}
		break;
	case PRS_MM_ACTIVERESIZE_VELOCITY:
		if (m_StripView == SV_MINIMIZED)
		{
			return S_OK;
		}
		if ( (wParam == VK_DOWN) || (wParam == VK_UP ) )
		{
			// change the velocity by one
			long lDelta = 0;
			// UP
			if (wParam == VK_UP) lDelta = 1;
			// DOWN
			if (wParam == VK_DOWN) lDelta = -1;
			// TODO: Optimize for single note selection
			/*
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
					m_pMIDIMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
					m_pMIDIMgr->ComputeSelectedPropNote();
					m_pMIDIMgr->RefreshPropertyPage();
					m_lLastDeltaVel += lDelta;
				}
			}
			else if(m_SelectionMode == SEQ_MULTIPLE_SELECT)
			*/
			{
				if( m_pMIDIMgr->OffsetSelectedNoteDurationAndVelocity( m_pPartRef, m_dwVariations, 0, lDelta, FALSE ) )
				{
					m_lLastDeltaVel += lDelta;
				}
			}
		}
		break;
	case PRS_MM_ACTIVERESIZE_END:
		if (m_StripView == SV_MINIMIZED)
		{
			return S_OK;
		}
		if ( (wParam == VK_RIGHT) || (wParam == VK_LEFT ) )
		{
			// change the duration by one
			long lDelta = 0;
			// LEFT
			if (wParam == VK_LEFT) lDelta = -1;
			// RIGHT
			if (wParam == VK_RIGHT) lDelta = 1;

			// TODO: Optimize for single note selection
			/*
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
					m_pMIDIMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
					m_pMIDIMgr->ComputeSelectedPropNote();
					m_pMIDIMgr->RefreshPropertyPage();
					m_lLastDeltaDur += lDelta;
				}
			}
			else if(m_SelectionMode == SEQ_MULTIPLE_SELECT)
			*/
			{
				if( m_pMIDIMgr->OffsetSelectedNoteDurationAndVelocity( m_pPartRef, m_dwVariations, lDelta, 0, FALSE ) )
				{
					m_lLastDeltaDur += lDelta;
				}
			}
		}
		break;

	case PRS_MM_ACTIVERESIZE_START:
		if (m_StripView == SV_MINIMIZED)
		{
			return S_OK;
		}
		// TODO: Optimize for single note selection
		if ( (m_prsSelecting == PRS_SINGLE_SELECT) || (m_prsSelecting == PRS_MULTIPLE_SELECT) )
		{
			long lDeltaStart = 0;

			// LEFT
			// RIGHT
			if ( (wParam == VK_LEFT) || (wParam == VK_RIGHT) )
			{
				if( m_pActiveDMNote )
				{
					lDeltaStart = m_pPartRef->m_pDMPart->AbsTime( m_pActiveDMNote );
				}
				else
				{
					CDirectMusicStyleNote *pItem = GetFirstSelectedNote();
					if( pItem )
					{
						lDeltaStart = m_pPartRef->m_pDMPart->AbsTime( pItem );
					}
					else
					{
						ASSERT(FALSE);
						lDeltaStart = 0;
					}
				}
				lDeltaStart = GetSnapAmount();
				lDeltaStart *= (wParam == VK_RIGHT) ? 1 : -1;
			}

			// TODO: Optimize for single note selection

			if( lDeltaStart )
			{
				long lNewDeltaStart = m_lLastDeltaStart + lDeltaStart;

				long lTotalOffset = lNewDeltaStart - (m_lLastDeltaStart / m_lGridClocks) * m_lGridClocks;

				// scroll if necessary
				// TODO: need to do something smarter than this
				//AdjustScroll(lXPos, lYPos);

				// This refreshes the property page and our strip, if the data changes
				if( m_pMIDIMgr->OffsetSelectedNoteStart( m_pPartRef, m_dwVariations, lDeltaStart, lTotalOffset ) )
				{
					m_lLastDeltaStart += lDeltaStart;
				}
			}
		}
		break;

	case PRS_MM_ACTIVEMOVE:
		if (m_StripView == SV_MINIMIZED)
		{
			return S_OK;
		}
		// TODO: Optimize for single note selection
		if ( (m_prsSelecting == PRS_SINGLE_SELECT) || (m_prsSelecting == PRS_MULTIPLE_SELECT) )
		{
			long lDeltaValue = 0;
			long lDeltaStart = 0;

			// UP
			if (wParam == VK_UP)
			{
				lDeltaValue = +1;
			}
			// DOWN
			if (wParam == VK_DOWN)
			{
				lDeltaValue = -1;
			}

			// LEFT
			// RIGHT
			if ( (wParam == VK_LEFT) || (wParam == VK_RIGHT) )
			{
				if( m_pActiveDMNote )
				{
					lDeltaStart = m_pPartRef->m_pDMPart->AbsTime( m_pActiveDMNote );
				}
				else
				{
					CDirectMusicStyleNote *pItem = GetFirstSelectedNote();
					if( pItem )
					{
						lDeltaStart = m_pPartRef->m_pDMPart->AbsTime( pItem );
					}
					else
					{
						ASSERT(FALSE);
						lDeltaStart = 0;
					}
				}
				lDeltaStart = GetSnapAmount();
				lDeltaStart *= (wParam == VK_RIGHT) ? 1 : -1;
			}
			
			if ( lDeltaStart != 0 || lDeltaValue != 0 )
			{
				long lDeltaGridStart = ((lDeltaStart + m_lLastDeltaStart) / m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks);
				if( lDeltaGridStart || lDeltaValue )
				{
					int nGridLength = m_pPartRef->m_pDMPart->GetGridLength();

					CDirectMusicEventItem* pEvent = GetFirstNote();

					for( ; pEvent; pEvent = pEvent->GetNext() )
					{
						if ( ( pEvent->m_dwVariation & m_dwVariations ) &&
							( pEvent->m_fSelected == TRUE ) )
						{
							// Make sure the start time doesn't go out of bounds
							if ( pEvent->m_mtGridStart + lDeltaGridStart < 0 )
							{
								lDeltaGridStart += -( pEvent->m_mtGridStart + lDeltaGridStart );
							}
							else if ( pEvent->m_mtGridStart + lDeltaGridStart >= nGridLength )
							{
								lDeltaGridStart += nGridLength - 1 -
											   (pEvent->m_mtGridStart + lDeltaGridStart);
							}

							// Make sure the value doesn't go out of bounds
	//						long lValue = m_pPartRef->DMNoteToMIDIValue( (CDirectMusicStyleNote *) pEvent, DMUS_PLAYMODE_NONE );
	//						ASSERT( (lValue <= 127) && (lValue >=0) );
							CDirectMusicStyleNote *pNote = ((CDirectMusicStyleNote *) pEvent);
							long lValue = pNote->m_bMIDIValue + lDeltaValue;

							// Check if the DMUS_PLAYMODE_CHORD_ROOT flag is set.
							/*
							BOOL bPlayModeFlags = (pNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE) ? m_pPartRef->m_pDMPart->m_bPlayModeFlags : pNote->m_bPlayModeFlags;
							if( bPlayModeFlags & DMUS_PLAYMODE_CHORD_ROOT )
							{
								DMUS_CHORD_PARAM dmChordParam;
								if( SUCCEEDED( m_pMIDIMgr->GetChord( GRID_TO_CLOCKS( pEvent->m_mtGridStart + lDeltaGridStart, m_pPartRef->m_pDMPart ), &dmChordParam ) ) )
								{
									// DMUS_PLAYMODE_CHORD_ROOT is set, ensure bottom does not go below the chord root
									if( (dmChordParam.bSubChordCount > m_pPartRef->m_bSubChordLevel) &&
										 (lValue < dmChordParam.SubChordList[m_pPartRef->m_bSubChordLevel].bChordRoot) )
									{
										lDeltaValue += dmChordParam.SubChordList[m_pPartRef->m_bSubChordLevel].bChordRoot - lValue;
										lValue = pNote->m_bMIDIValue + lDeltaValue;
									}
								}
							}
							*/

							if( lValue < 0 )
							{
								// Ensure bottom does not go below 0
								lDeltaValue += -lValue;
								// Probably not necessary:
								lValue = pNote->m_bMIDIValue + lDeltaValue;
							}

							if ( lValue > 127 )
							{
								// Ensure top does not go above 127
								lDeltaValue += 127 - lValue;
							}
						}
					}
				}

				// scroll if necessary
				// TODO: need to do something smarter than this
				//AdjustScroll(lXPos, lYPos);

				lDeltaStart += (lDeltaGridStart - (((lDeltaStart + m_lLastDeltaStart) / m_lGridClocks) - (m_lLastDeltaStart / m_lGridClocks))) * m_lGridClocks;

				if( lDeltaStart || lDeltaValue )
				{
					long lGridDiff = (lDeltaStart + m_lLastDeltaStart) / m_lGridClocks - m_lLastDeltaStart / m_lGridClocks;
					if( lGridDiff != 0 )
					{
						lDeltaStart -= lGridDiff * m_lGridClocks;
					}

					// This refreshes the property page and our strip, if the data changes
					if ( m_pMIDIMgr->OffsetSelectedNoteValuePosition( m_pPartRef, m_dwVariations, lDeltaStart, lGridDiff, lDeltaValue, m_fCtrlKeyDown ) )
					{
						m_lLastDeltaStart = m_lLastDeltaStart + lDeltaStart + lGridDiff * m_lGridClocks;
						m_cLastDeltaValue = char(m_cLastDeltaValue + lDeltaValue);

						if( m_fCtrlKeyDown )
						{
							m_fCtrlKeyDown = 0;
						}
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

HRESULT CPianoRollStrip::OnLButtonUp( long lXPos, long lYPos)
{
	if (m_StripView == SV_MINIMIZED)
	{
		return S_OK;
	}

	// Stop any playing note
	m_pMIDIMgr->StopNote();

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

	if( m_fCtrlKeyDown == 1)
	{
		// Unselect
		m_pMIDIMgr->UnselectNote( this, m_pActiveDMNote );
		m_pMIDIMgr->UpdateStatusBarDisplay();
	}
	m_fCtrlKeyDown = 0;

	if( m_fInsertingNoteFromMouseClick )
	{
		// update the corresponding DirectMusicPart
		m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_EDIT_INSERT );
	}

	switch(m_MouseMode)
	{
	case PRS_MM_NORMAL:
		// perhaps create a new note?
		break;
	case PRS_MM_MOVE:
	case PRS_MM_RESIZE_START:
	case PRS_MM_RESIZE_END:
	case PRS_MM_RESIZE_VELOCITY:
		break;
	case PRS_MM_ACTIVERESIZE_VELOCITY:
	case PRS_MM_ACTIVERESIZE_END:
	case PRS_MM_ACTIVERESIZE_START:
	case PRS_MM_ACTIVEMOVE:
	{
		BOOL fChanged = FALSE;

		switch( m_MouseMode )
		{
		case PRS_MM_ACTIVERESIZE_VELOCITY:
			m_MouseMode = PRS_MM_RESIZE_VELOCITY;
			if( m_lLastDeltaVel != 0 )
			{
				fChanged = TRUE;
			}
			break;
		case PRS_MM_ACTIVERESIZE_END:
			m_MouseMode = PRS_MM_RESIZE_END;
			if( m_lLastDeltaDur != 0 )
			{
				fChanged = TRUE;
			}
			break;
		case PRS_MM_ACTIVERESIZE_START:
			m_MouseMode = PRS_MM_RESIZE_START;
			if( m_lLastDeltaStart != 0 )
			{
				fChanged = TRUE;
			}
			break;
		case PRS_MM_ACTIVEMOVE:
			m_MouseMode = PRS_MM_MOVE;
			if( m_lLastDeltaStart != 0
			||  m_cLastDeltaValue != 0 )
			{
				fChanged = TRUE;
			}
			break;
		}

		/* Already done in the Timeline.
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		*/

		if( !m_fInsertingNoteFromMouseClick
		&&	fChanged )
		{
			// update the corresponding DirectMusicPart
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_NOTE_CHANGE );
		}
		break;
	}
	case PRS_MM_ACTIVEPRESS_VARIATIONCHOICES:
		m_fVariationChoicesPressed = FALSE;
		InvalidateVariationBar();
		break;
	case PRS_MM_ACTIVESELECT_VARIATIONS:
		m_MouseMode = PRS_MM_NORMAL;
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

HRESULT CPianoRollStrip::OnRButtonUp( void )
{
	HRESULT hr = S_OK;
	POINT pt;
	BOOL  bResult;

	// Get the cursor position (To put the menu there)
	bResult = GetCursorPos( &pt );
	ASSERT( bResult );
	if( !bResult )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pStripMgr != NULL );
	if ( m_pStripMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	if ( m_pMIDIMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	HMENU hMenu, hMenuPopup;
	hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_EDIT_MENU));
	if (hMenu == NULL)
	{
		return E_UNEXPECTED;
	}

	hMenuPopup = ::GetSubMenu( hMenu, 0 );
	if (hMenuPopup == NULL)
	{
		::DestroyMenu( hMenu );
		return E_UNEXPECTED;
	}

	// Remove add/delete track menu items, if we're not pattern track
	if( !m_pMIDIMgr->m_pIDMTrack )
	{
		::DeleteMenu( hMenuPopup, ID_EDIT_ADD_TRACK, MF_BYCOMMAND );
		::DeleteMenu( hMenuPopup, ID_EDIT_DELETE_TRACK, MF_BYCOMMAND );
		::DeleteMenu( hMenuPopup, 20, MF_BYPOSITION );
	}

	// update items
	::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, 2, ( CanPaste() == S_OK ) ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( GetFirstNote() != NULL ) ?
					MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( (m_StripView == SV_NORMAL) && (CanInsert() == S_OK) ) ?
					MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ?
					MF_ENABLED : MF_GRAYED );

	// Views menu
	::EnableMenuItem( hMenuPopup, 13, ( m_StripView == SV_NORMAL ) ?
					MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
	::CheckMenuItem( hMenuPopup, ID_EDIT_VIEW_PIANOROLL, m_fHybridNotation ? MF_UNCHECKED : MF_CHECKED );
	::CheckMenuItem( hMenuPopup, ID_EDIT_VIEW_HYBRID, m_fHybridNotation ? MF_CHECKED : MF_UNCHECKED );

	// Zoom items
	::EnableMenuItem( hMenuPopup, ID_EDIT_ZOOMIN, ( m_StripView == SV_NORMAL ) ?
					MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_ZOOMOUT, ( m_StripView == SV_NORMAL ) ?
					MF_ENABLED : MF_GRAYED );

	// Merge Variations
	::EnableMenuItem( hMenuPopup, ID_EDIT_MERGE_VARIATIONS, m_dwVariations ? MF_ENABLED : MF_GRAYED );

	// Snap-to menu
	VARIANT var;
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
	{
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

	if( ValidPartRefPtr() )
	{
		::EnableMenuItem( hMenuPopup, ID_EDIT_UNLINK, m_pPartRef->m_fHardLink ?
						MF_ENABLED : MF_GRAYED );
	}

	m_pMIDIMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
	DestroyMenu(hMenu); // This will destroy the submenu as well.

	return hr;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate state
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( ValidPartRefPtr() );

	if ( (m_StripView == SV_NORMAL) && (lYPos < m_lVerticalScroll + VARIATION_BUTTON_HEIGHT * 2) )
	{
		if ((nMsg == WM_MOUSEMOVE) || (nMsg == WM_LBUTTONUP))
		{
			switch(m_MouseMode)
			{
			case PRS_MM_ACTIVESELECT_VARIATIONS:
			case PRS_MM_ACTIVEPRESS_VARIATIONCHOICES:
			case PRS_MM_NORMAL:
			case PRS_MM_MOVE:
			case PRS_MM_RESIZE_START:
			case PRS_MM_RESIZE_END:
			case PRS_MM_RESIZE_VELOCITY:
				return OnVariationBarMsg( nMsg, wParam, lParam, lXPos, lYPos );
				break;
			//case PRS_MM_ACTIVEMOVE:
			//case PRS_MM_ACTIVERESIZE_START:
			//case PRS_MM_ACTIVERESIZE_END:
			//case PRS_MM_ACTIVERESIZE_VELOCITY:
			//	break;
			}
		}
		else if ((nMsg == WM_SETCURSOR) || (nMsg == WM_LBUTTONDOWN) || (nMsg == WM_RBUTTONUP) ||
			(nMsg == WM_LBUTTONDBLCLK))
		{
			return OnVariationBarMsg( nMsg, wParam, lParam, lXPos, lYPos );
		}
	}

	lYPos -= VARIATION_BUTTON_HEIGHT * 2;
	lParam = MAKELONG( lXPos, lYPos );

	HRESULT hr = S_OK;

	switch( nMsg )
	{
	case WM_TIMER:
		OnTimer();
		break;
	case WM_SETFOCUS:
		if( m_pMIDIMgr->m_pDMPattern->m_fInLoad == FALSE )
		{
			// no need to update performance engine
			m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
			m_pMIDIMgr->UpdateOnDataChanged( NULL ); 
		}
		m_pMIDIMgr->SetFocus(this, 1);
		//m_ToolTipCtrl.Activate( TRUE );
		break;
	case WM_KILLFOCUS:
		// Stop any playing note
		m_pMIDIMgr->StopNote();

		if( m_pMIDIMgr->m_pDMPattern
		&&	m_pMIDIMgr->m_pDMPattern->m_fInLoad == FALSE )
		{
			// no need to update performance engine
			m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
			m_pMIDIMgr->UpdateOnDataChanged( NULL ); 
		}
		m_pMIDIMgr->KillFocus(this);
		//m_ToolTipCtrl.Activate( FALSE );
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		hr = OnLButtonDown( wParam, lXPos, lYPos );
		break;
	case WM_LBUTTONUP:
		hr = OnLButtonUp( lXPos, lYPos );
		break;
	case WM_SETCURSOR:
		hr = OnSetCursor( lXPos, lYPos );
		break;
	case WM_MOUSEMOVE:
		hr = OnMouseMove( lXPos, lYPos );
		break;
	case WM_KEYDOWN:
		hr = OnKeyDown( wParam, lParam );
		break;
	case WM_CHAR:
		hr = OnChar( wParam );
		break;
	case WM_VSCROLL:
		hr = OnVScroll();
		break;
	case WM_MOVE:
		hr = OnSize();
		break;
	case WM_DESTROY:
		hr = OnDestroy();
		break;
	case WM_CREATE:
		hr = OnCreate();
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
		hr = OnSize();
		if( wParam == SIZE_RESTORED )
		{
			VARIANT var;
			if( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )) )
			{
				if( m_lLastHeight != V_I4(&var) )
				{
					// We were resized, so update the pattern editor with our new size
					// No need to update performance engine
					m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
					m_pMIDIMgr->UpdateOnDataChanged( NULL );
				}
			}
		}
		break;
	case WM_MOUSEWHEEL:
		if (m_StripView == SV_NORMAL)
		{
			long lNewVertScroll = m_lVerticalScroll;
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

			if ( lNewVertScroll / ( m_lMaxNoteHeight ) > 127)
			{
				lNewVertScroll = ( m_lMaxNoteHeight ) * 127;
			}
			else if ( lNewVertScroll < 0)
			{
				lNewVertScroll = 0;
			}

			if ( lNewVertScroll != m_lVerticalScroll )
			{
				SetNewVerticalScroll( lNewVertScroll );
			}
		}
		break;
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
			m_fPianoRollMenu = TRUE;

			UnselectGutterRange();
			if( m_StripView != SV_MINIMIZED )
			{
				CDirectMusicStyleNote* pDMNote = GetDMNoteAndRectFromPoint( lXPos, lYPos, NULL );
				// Select the note clicked on iff 
				if( (pDMNote != NULL) && (m_prsSelecting != PRS_MULTIPLE_SELECT) )
				{
					if (m_pActiveDMNote != pDMNote)
					{
						m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );
						m_pMIDIMgr->SelectNote( this, pDMNote );
						m_pActiveDMNote = pDMNote;
						m_pShiftDMNote = pDMNote;
						m_pMIDIMgr->UpdateStatusBarDisplay();
					}
				}
				else if( pDMNote == NULL )
				{
					m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );
				}
			}

			//			Note note;
			//			void* pVoid = (void*)&note;
			//			m_pMIDIMgr->GetData(&pVoid);
			//			check if notes are identical to reduce flicker. Leaving this out for now.
			m_pMIDIMgr->OnShowProperties();
			// Set the focus back to the timeline
			// Not needed anymore -jd
			
			// We don't need to refresh the property page here because the MIDIMgr 
			// should be the one talking to the note property page.
			hr = OnRButtonUp();
		}
		break;
	case WM_COMMAND:
		WORD wID;

		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_ENABLE:
		case ID_DISABLE:
			hr = OnVariationBarMsg( nMsg, wParam, lParam, lXPos, lYPos );
			break;
		case ID_VIEW_PROPERTIES:
			{	// Show the note property page
				// If from menu, note prop page if note selected, else strip's page.
				ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
				if (m_pMIDIMgr->m_pTimeline == NULL)
				{
					hr = E_UNEXPECTED;
					break;
				}
				
				// Get a framework pointer
				VARIANT var;
				m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
				if( var.vt == VT_UNKNOWN )
				{
					LPUNKNOWN punk;
					punk = V_UNKNOWN( &var );
					if( punk )
					{
						// Get a pointer to the property sheet
						IDMUSProdPropSheet* pIPropSheet;
						hr = punk->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
						ASSERT( SUCCEEDED( hr ));
						if( FAILED( hr ))
						{
							punk->Release();
							hr = E_UNEXPECTED;
							break;
						}
						// Make the property sheet visible
						pIPropSheet->Show( TRUE );
						pIPropSheet->Release();
						punk->Release();
					}
				}

				if (m_fPianoRollMenu)
				{
					// Change to note property page
					m_pMIDIMgr->OnShowProperties();
				}
				else
				{
					// Change to our property page
					OnShowProperties();
				}
				
				hr = S_OK;
			}
			break;
		case ID_EDIT_CUT:
			hr = Cut( NULL );
			break;
		case ID_EDIT_COPY:
			hr = Copy( NULL );
			break;
		case ID_EDIT_DELETE:
			hr = Delete();
			break;
		case ID_EDIT_QUANTIZE:
			hr = DoQuantize();
			break;
		case ID_EDIT_VELOCITY:
			hr = DoVelocity();
			break;
		case ID_EDIT_PASTE:
			hr = Paste( NULL );
			break;
		case ID_EDIT_PASTE_MERGE:
			m_pMIDIMgr->m_pTimeline->SetPasteType( TL_PASTE_MERGE );
			hr = Paste( NULL );
			break;
		case ID_EDIT_PASTE_OVERWRITE:
			m_pMIDIMgr->m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
			hr = Paste( NULL );
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
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
		case ID_EDIT_NEW_PART:
			hr = m_pMIDIMgr->AddNewStrip();
			break;
		case ID_EDIT_DELETE_PART:
			hr = m_pMIDIMgr->OnDeletePart( this );
			break;
		case ID_EDIT_MERGE_VARIATIONS:
			if (m_pMIDIMgr != NULL && ValidPartRefPtr() )
			{
				m_pMIDIMgr->MergeVariations( m_pPartRef, m_dwVariations );
				hr = S_OK;
			}
			else
			{
				hr = E_UNEXPECTED;
			}
			break;
		case ID_EDIT_UNLINK:
			// Should only be enabled if this is a linked PartRef
			ASSERT( m_pPartRef->m_fHardLink );
			if( m_pPartRef->m_fHardLink )
			{
				m_pPartRef->m_fHardLink = FALSE;
				m_pMIDIMgr->m_fDirty = TRUE;

				// If we're a pattern track
				if( m_pMIDIMgr->m_pIDMTrack )
				{
					// Check if there is only one other partref that links to our part
					if( 1 == m_pPartRef->m_pPattern->GetNumHardLinks( m_pPartRef->m_pDMPart ) )
					{
						// Mark the other PartRef as unlinked as well (since it can't link to itself)
						POSITION pos = m_pPartRef->m_pPattern->m_lstPartRefs.GetHeadPosition();
						while( pos )
						{
							CDirectMusicPartRef *pPartRef = m_pPartRef->m_pPattern->m_lstPartRefs.GetNext( pos );
							if( (m_pPartRef != pPartRef)
							&&	(pPartRef->m_pDMPart == m_pPartRef->m_pDMPart) )
							{
								// Unlink this partref
								pPartRef->m_fHardLink = FALSE;

								// Refresh all strips that use this PartRef
								m_pMIDIMgr->RefreshPartRefDisplay( pPartRef );

								// Break, since there's only one
								break;
							}
						}
					}
				}

				// Let the object know about the changes
				// Can only undo when in a Pattern Track
				m_pMIDIMgr->UpdateOnDataChanged( (m_pMIDIMgr->m_pIDMTrack == NULL) ? NULL : IDS_UNDO_UNLINK_PARTREF );

				// Refresh all strips that use this PartRef
				m_pMIDIMgr->RefreshPartRefDisplay( m_pPartRef );
			}
			break;
		case ID_SNAP_NONE:
		case ID_SNAP_GRID:
		case ID_SNAP_BEAT:
		case ID_SNAP_BAR:
		{
			DMUSPROD_TIMELINE_SNAP_TO tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
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
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_SNAP_TO, var );
			break;
		}
		case ID_EDIT_VIEW_PIANOROLL:
		case ID_EDIT_VIEW_HYBRID:
		case ID_EDIT_ZOOMIN:
		case ID_EDIT_ZOOMOUT:
			// The following code is very similar to OnChar()
			//if( m_StripView == SV_NORMAL )
			{
				// TODO: Undo Zoom?
				//CString strUndoText;

				if( wID == ID_EDIT_ZOOMIN )
				{
					ChangeZoom( m_dblVerticalZoom + 0.01 );

					//strUndoText.LoadString( IDS_UNDO_ZOOM_IN );
				}
				else if( wID == ID_EDIT_ZOOMOUT )
				{
					if( m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
					{
						ChangeZoom( m_dblVerticalZoom - 0.01 );

						//strUndoText.LoadString( IDS_UNDO_ZOOM_OUT );
					}
				}
				else if( wID == ID_EDIT_VIEW_HYBRID )
				{
					// This handles notifying the other strips, and adding an Undo state
					ChangeNotationType( TRUE, TRUE );
				}
				else if( m_fHybridNotation && (wID == ID_EDIT_VIEW_PIANOROLL) )
				{
					// This handles notifying the other strips, and adding an Undo state
					ChangeNotationType( FALSE, TRUE );
				}

				/*
				if( !strUndoText.IsEmpty() )
				{
					// Let the object know about the changes
					// No need to update performance engine
					m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
					m_pMIDIMgr->UpdateOnDataChanged( &strUndoText );
				}
				*/
			}
			break;

		case ID_INSERT_NEWINSTRUMENT:
			{
				// Need to insert a new band track
				IDMUSProdSegmentEdit *pIDMUSProdSegmentEdit;
				if( m_pMIDIMgr->m_pDMProdSegmentNode
				&&	SUCCEEDED( m_pMIDIMgr->m_pDMProdSegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit, (void **)&pIDMUSProdSegmentEdit ) ) )
				{
					// Assume the strip creation succeeds
					// BUGBUG: We will add an extra undo stip of AddStrip fails.  Oh well.
					m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_ADD_BANDTRACK );

					IUnknown *punkStripMgr;
					if( SUCCEEDED( pIDMUSProdSegmentEdit->AddStrip( CLSID_DirectMusicBandTrack, m_pMIDIMgr->m_dwGroupBits, &punkStripMgr ) ) )
					{
						IUnknown *pStripMgr;
						if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IUnknown, (void **)&pStripMgr) ) )
						{
							// Now, find the band strip for this band track
							IDMUSProdStrip *pIDMUSProdStrip;
							DWORD dwEnum = 0;
							VARIANT varStripMgr;
							while( SUCCEEDED( m_pMIDIMgr->m_pTimeline->EnumStrip( dwEnum, &pIDMUSProdStrip ) ) )
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
										if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
										{
											fFreezeUndo = V_BOOL(&var);
										}

										if( !fFreezeUndo )
										{
											// Need to set TP_FREEZE_UNDO or the segment will add an undo state for us
											var.vt = VT_BOOL;
											V_BOOL(&var) = TRUE;
											m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
										}

										// Found the band strip - now insert a new band
										pIDMUSProdStrip->OnWMMessage( WM_COMMAND, ID_INSERT_NEWINSTRUMENT, 0, 0, 0 );

										if( !fFreezeUndo )
										{
											// Need to reset TP_FREEZE_UNDO or the segment will add an undo state for us
											var.vt = VT_BOOL;
											V_BOOL(&var) = FALSE;
											m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
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

HRESULT CPianoRollStrip::SendEditToTimeline(WPARAM wId)
{
	HRESULT				  hr = E_FAIL;
	IDMUSProdTimeline	  *pITimeline;
	IDMUSProdTimelineEdit *pITimelineEdit;

	ASSERT( m_pMIDIMgr != NULL );
	if(m_pMIDIMgr != NULL)
	{
		pITimeline = m_pMIDIMgr->m_pTimeline;
		ASSERT( pITimeline != NULL );
		if( pITimeline != NULL)
		{
			pITimeline->AddRef();
			if(SUCCEEDED(pITimeline->QueryInterface(IID_IDMUSProdTimelineEdit, (void **) &pITimelineEdit)))
			{
				switch(wId)
				{
				case ID_EDIT_CUT:
					hr = pITimelineEdit->Cut(NULL);
					break;
				case ID_EDIT_COPY:
					hr = pITimelineEdit->Copy(NULL);
					break;
				case ID_EDIT_PASTE:
					hr = pITimelineEdit->Paste(NULL);
					break;
				case ID_EDIT_INSERT:
					hr = pITimelineEdit->Insert();
					break;
				case ID_EDIT_DELETE:
					hr = pITimelineEdit->Delete();
					break;
				default:
					break;
				}
				pITimelineEdit->Release();
			}
			pITimeline->Release();
		}
	}
	if ( hr != E_NOTIMPL )
	{
		ASSERT(SUCCEEDED(hr));
	}
	return hr;
}

HRESULT CPianoRollStrip::OnCreate(void)
{
	m_fGutterSelected = FALSE;

	ASSERT( m_pPartRef != NULL );
	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr != NULL)
	{
		if( m_pMIDIMgr->m_pTimeline )
		{
			long lValue = -1;
			if( m_lVerticalScroll == -1 )
			{
				// since this is a new strip, let's auto-scroll so the data is showing
				CDirectMusicEventItem* pEvent = GetFirstNote();
				for( ; pEvent; pEvent = pEvent->GetNext() )
				{
					if ( pEvent->m_dwVariation & m_dwVariations )
					{
						CDirectMusicStyleNote* pDMNote = (CDirectMusicStyleNote*) pEvent;
	//					if( m_pPartRef->DMNoteToMIDIValue(pDMNote, DMUS_PLAYMODE_NONE) > lValue )
						if( pDMNote->m_bMIDIValue > lValue )
						{
	//						lValue = m_pPartRef->DMNoteToMIDIValue(pDMNote, DMUS_PLAYMODE_NONE);
							lValue = pDMNote->m_bMIDIValue;
						}
					}
				}
				if( ( lValue >= 0 ) && ( lValue < 128 ) )
				{
					VARIANT var;
					var.vt = VT_I4;
					m_lVerticalScroll = ( 127 - lValue ) * m_lMaxNoteHeight;
					V_I4(&var) = m_lVerticalScroll;
					m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );
				}
				else if ( lValue == -1 )
				{
					VARIANT var;
					var.vt = VT_I4;
					// Default scroll position is C4
					m_lVerticalScroll = ( 127 - DEFAULT_VERTICAL_SCROLL ) * m_lMaxNoteHeight;
					V_I4(&var) = m_lVerticalScroll;
					m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );
				}
			}

			if( m_prScrollBar.GetSafeHwnd() == NULL )
			{
				IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
				m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
				if (pIOleInPlaceObjectWindowless)
				{
					HWND hWnd;
					if (pIOleInPlaceObjectWindowless->GetWindow(&hWnd) == S_OK)
					{
						CWnd wnd;
						wnd.Attach(hWnd);

						m_rectScrollBar = CRect(0, 0, 40, 100);
						m_prScrollBar.Create(SBS_RIGHTALIGN | SBS_VERT | WS_CHILD | WS_CLIPSIBLINGS,
							m_rectScrollBar, &wnd, IDC_PRSCOLL_BAR );
						m_prScrollBar.SetPianoRollStrip( this );

						SCROLLINFO si;
						si.cbSize = sizeof( SCROLLINFO );
						si.fMask = SIF_POS | SIF_RANGE;
						si.nMin = 0;
						si.nMax = 127;
						if ( ( lValue >= 0 ) && ( lValue < 128 ) )
						{
							si.nPos = 127 - lValue;
						}
						else if ( lValue == -1 )
						{
							si.nPos = 127 - DEFAULT_VERTICAL_SCROLL;
						}
						m_prScrollBar.SetScrollInfo( &si, TRUE );

						/*
						m_ToolTipCtrl.Create( &wnd, 0 );

						RECT rect;
						rect.left = 10;
						rect.right = 100;
						rect.top = 10;
						rect.bottom = 100;
						m_ToolTipCtrl.AddTool( &wnd, IDS_VARIATION_CHOICES, &rect, IDC_PR_TOOL_TIP );

						TOOLINFO ti;
						ti.cbSize = sizeof( TOOLINFO );
						ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
						ti.hwnd = hWnd;
						ti.uId = IDC_PR_TOOL_TIP;
						ti.rect = rect;
						ti.hinst = AfxGetStaticModuleState( )->m_hCurrentResourceHandle;
						ti.lpszText = MAKEINTRESOURCE(IDS_VARIATION_CHOICES);
						ti.lParam = 0;
						m_ToolTipCtrl.SetToolInfo( &ti );

						m_ToolTipCtrl.SetToolRect( &wnd, IDC_PR_TOOL_TIP, &rect );
						*/

						wnd.Detach();
					}
					pIOleInPlaceObjectWindowless->Release();
				}
			}

			// Set the position and page size of the scroll bar
			ComputeVScrollBar();

			// Get Left and right selection boundaries
			m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginTimelineSelection );
			m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndTimelineSelection );

			// Get a pointer to the Conductor (if we don't already have one)
			if (!m_pIConductor)
			{
				VARIANT var;
				if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var )))
				{
					IDMUSProdFramework* pIFramework = NULL;
					pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
					ASSERT( pIFramework != NULL );
					
					IDMUSProdComponent* pIComponent = NULL;
					if( SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
					{
						IDMUSProdConductor *pConductor = NULL;
						pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
						if (pConductor)
						{
							m_pIConductor = pConductor;
							// Intentionally commented out
							//pConductor->Release();
						}
						pIComponent->Release();
					}
					pIFramework->Release();
				}
			}

			// If we're currently playing, enable the variation timer.
			if( m_pMIDIMgr->m_pSegmentState )
			{
				EnableVariationTimer();
			}
		}
	}

	// Load button bitmaps
	if( InterlockedIncrement( &m_lBitmapRefCount ) == 1 )
	{
		if( m_BitmapBlankButton.GetSafeHandle() == NULL )
		{
			m_BitmapBlankButton.LoadBitmap( IDB_BLANK_BUTTON );
		}
		if( m_BitmapPressedButton.GetSafeHandle() == NULL )
		{
			m_BitmapPressedButton.LoadBitmap( IDB_PRESSED_BUTTON );
		}
		if( m_BitmapBlankInactiveButton.GetSafeHandle() == NULL )
		{
			m_BitmapBlankInactiveButton.LoadBitmap( IDB_BLANK_INACTIVE_BUTTON );
		}
		if( m_BitmapPressedInactiveButton.GetSafeHandle() == NULL )
		{
			m_BitmapPressedInactiveButton.LoadBitmap( IDB_PRESSED_INACTIVE_BUTTON );
		}
		if( m_BitmapGutter.GetSafeHandle() == NULL )
		{
			m_BitmapGutter.LoadBitmap( IDB_GUTTER_BITMAP );
		}
		if( m_BitmapPressedGutter.GetSafeHandle() == NULL )
		{
			m_BitmapPressedGutter.LoadBitmap( IDB_GUTTER_PRESSED_BITMAP );
		}
		if( m_BitmapMoaw.GetSafeHandle() == NULL )
		{
			m_BitmapMoaw.LoadBitmap( IDB_MOAW_UP );
		}
		if( m_BitmapPressedMoaw.GetSafeHandle() == NULL )
		{
			m_BitmapPressedMoaw.LoadBitmap( IDB_MOAW_DOWN );
		}

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

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip IDMUSProdStripFunctionBar
HRESULT	STDMETHODCALLTYPE CPianoRollStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	DrawFunctionBar( hDC, sv );

	if( !m_pMIDIMgr->m_pTimeline )
	{
		return E_UNEXPECTED;
	}

	if (!m_pIConductor)
	{
		VARIANT var;
		if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var )))
		{
			IDMUSProdFramework* pIFramework = NULL;
			pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
			ASSERT( pIFramework != NULL );
			
			IDMUSProdComponent* pIComponent = NULL;
			if( SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				IDMUSProdConductor *pConductor = NULL;
				pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pConductor );
				if (pConductor)
				{
					m_pIConductor = pConductor;
					// Intentionally commented out
					//pConductor->Release();
				}
				pIComponent->Release();
			}
			pIFramework->Release();
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::FBOnWMMessage( UINT nMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;

	switch( nMsg )
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		{
			BOOL fShowProps = TRUE;
			if( lYPos <= m_lVerticalScroll + m_BitmapZoomInDown.GetBitmapDimension().cy + m_BitmapZoomOutDown.GetBitmapDimension().cy )
			{
				VARIANT varXS;
				if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
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
						m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

						// TODO: Undo Zoom?
						//CString strUndoText;

						if( lYPos <= m_lVerticalScroll + m_BitmapZoomInDown.GetBitmapDimension().cy )
						{
							m_fZoomInPressed = TRUE;
							ChangeZoom(	m_dblVerticalZoom + 0.01 );
							//strUndoText.LoadString( IDS_UNDO_ZOOM_IN );
						}
						else
						{
							m_fZoomOutPressed = TRUE;
							if( m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
							{
								ChangeZoom(	m_dblVerticalZoom - 0.01 );
								//strUndoText.LoadString( IDS_UNDO_ZOOM_OUT );
							}
						}

						/*
						if( !strUndoText.IsEmpty() )
						{
							// Let the object know about the changes
							// No need to update performance engine
							m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
							m_pMIDIMgr->UpdateOnDataChanged( &strUndoText );
						}
						*/
					}
				}
			}

			VARIANT varFNHeight;
			if( fShowProps
			&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight ) ) )
			{
				if( lYPos > m_lVerticalScroll + V_I4(&varFNHeight)
				&&	lYPos < m_lVerticalScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
				{
					VARIANT varXS;
					if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
					{
						if( lXPos < V_I4( &varXS ) - m_rectScrollBar.Width() - NEWBAND_BUTTON_WIDTH - 1 )
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
				//BUGBUG: Is SetData() call needed?
				//m_fDontRedraw = TRUE;
				//SetData( &m_PRD );
				//m_fDontRedraw = FALSE;
				OnShowProperties();
				// Set the focus back to the timeline
				// Not needed anymore -jd
				
				// We don't need to refresh the property page here because there's currently
				// no way to change values in the piano roll property page from the piano roll.

				if( (m_StripView == SV_NORMAL) && (lYPos >= m_lVerticalScroll + 2 * VARIATION_BUTTON_HEIGHT) )
				{
					// Play note with the value clicked on
					CPropNote propNote;
					propNote.m_mtDuration =  m_lBeatClocks;
					propNote.m_bVelocity = 100;
					propNote.m_bMIDIValue = PositionToMIDIValue( lYPos - 2 * VARIATION_BUTTON_HEIGHT );

					MUSIC_TIME mtTime = 0;
					m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime );
					propNote.m_mtGridStart = CLOCKS_TO_GRID( mtTime, m_pPartRef->m_pDMPart );
					propNote.m_nOffset = short(mtTime - GRID_TO_CLOCKS( propNote.m_mtGridStart, m_pPartRef->m_pDMPart ));
					propNote.SetPart( m_pPartRef->m_pDMPart );
					m_pMIDIMgr->PlayNote( &propNote, m_pPartRef->m_dwPChannel );

					if( propNote.m_bMIDIValue != m_lInsertVal )
					{
						m_lInsertVal = propNote.m_bMIDIValue;
						InvalidatePianoRoll();
					}

					VARIANT var;
					var.vt = VT_BOOL;
					V_BOOL(&var) = TRUE;
					m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			VARIANT varFNHeight;
			if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONNAME_HEIGHT, &varFNHeight ) ) )
			{
				if( lYPos > m_lVerticalScroll + V_I4(&varFNHeight)
				&&	lYPos < m_lVerticalScroll + V_I4(&varFNHeight) + INSTRUMENT_BUTTON_HEIGHT )
				{
					VARIANT varXS;
					if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
					{
						POINT point;
						GetCursorPos( &point );
						LPARAM lParam = MAKELPARAM(point.x, point.y);
						if( lXPos < V_I4( &varXS ) - m_rectScrollBar.Width() - NEWBAND_BUTTON_WIDTH - 1 )
						{
							if( m_fInstrumentPressed )
							{
								::PostMessage( m_prScrollBar.m_hWnd, WM_APP, WM_APP_INSTRUMENTMENU, lParam );
							}
						}
						else
						{
							if( m_fNewBandPressed )
							{
								::PostMessage( m_prScrollBar.m_hWnd, WM_APP, WM_APP_BANDMENU, lParam );
							}
						}
					}
				}
			}
		}
		// Zoom buttons and StopNote handled on OnLButtonUp
		hr = OnLButtonUp( lXPos, lYPos );
		m_MouseMode = PRS_MM_NORMAL;
		break;

	case WM_SETCURSOR:
		m_hCursor = GetArrowCursor();
		break;
	case WM_MOUSEWHEEL:
		// Should never be called
		ASSERT(FALSE);
		break;
	case WM_MOUSEMOVE:
		if (m_StripView != SV_MINIMIZED)
		{
			/* Leave the X position alone - it may be before a pick-up measure
			// If the x position is less than zero, set it to zero
			if (lXPos < 0)
			{
				lXPos = 0;
			}
			*/
			lYPos -= VARIATION_BUTTON_HEIGHT * 2;
			//lParam = MAKELONG( lXPos, lYPos );
			hr = OnMouseMove( lXPos, lYPos );
		}
		break;
	case WM_RBUTTONUP:
		m_fPianoRollMenu = FALSE;

		OnShowProperties();

		hr = OnRButtonUp();
		break;
	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CPianoRollStrip::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return GetDataConst( ppData );
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::GetDataConst( void **ppData) const
{
	if( (ppData == NULL)
	||	(*ppData == NULL) )
	{
		return E_POINTER;
	}

	DWORD *pdwData = static_cast<DWORD *>(*ppData);
	switch( *pdwData )
	{
	case 0:
	{
		ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(*ppData);
		pioGroupBitsPPG->dwGroupBits = m_pMIDIMgr->m_dwGroupBits;;
	}
		break;
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = static_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pMIDIMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pMIDIMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
	}
		break;
	case 2:
	{
		PianoRollData* pPRD = (PianoRollData*)*ppData;

		InitializePianoRollData( pPRD );

		CDirectMusicPart *pDMPart = NULL;
		if( m_pPartRef )
		{
			pDMPart = m_pPartRef->m_pDMPart;
		}
		if( pDMPart != NULL )
		{
			pPRD->bPlayModeFlags	= pDMPart->m_bPlayModeFlags;
			pPRD->wNbrMeasures		= pDMPart->m_wNbrMeasures;
			pPRD->bInvertUpper		= pDMPart->m_bInvertUpper;
			pPRD->bInvertLower		= pDMPart->m_bInvertLower;
			pPRD->bAutoInvert		= pDMPart->m_bAutoInvert;
		}

		if( m_pPartRef != NULL )
		{
			pPRD->dwPChannel		= m_pPartRef->m_dwPChannel;
			pPRD->cstrPartRefName	= m_pPartRef->m_strName;
			pPRD->bSubChordLevel	= m_pPartRef->m_bSubChordLevel;
			pPRD->bVariationLockID	= m_pPartRef->m_bVariationLockID;
			pPRD->bRandomVariation	= m_pPartRef->m_bRandomVariation;

			WCHAR wszName[MAX_PATH];
			if( SUCCEEDED( m_pMIDIMgr->m_pIPChannelName->GetPChannelName( m_pPartRef->m_dwPChannel, wszName ) ) )
			{
				pPRD->cstrPChannelName = wszName;
			}
		}
	}
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( m_fInSetData )
	{
		return S_OK;
	}

	if ( pData == NULL )
	{
		return E_POINTER;
	}

	if ( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	m_fInSetData = TRUE;

	DWORD *pdwData = static_cast<DWORD *>(pData);
	switch( *pdwData )
	{
	case 0:
	{
		ioGroupBitsPPG *pioGroupBitsPPG = static_cast<ioGroupBitsPPG *>(pData);

		if( pioGroupBitsPPG->dwGroupBits != m_pMIDIMgr->m_dwGroupBits )
		{
			CMIDIMgr *pMIDIMgr = m_pMIDIMgr;
			pMIDIMgr->m_dwGroupBits = pioGroupBitsPPG->dwGroupBits;
			pMIDIMgr->UpdateFlatsAndKey();
			pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_TRACK_GROUP );

			// Notify the chord track that a Pattern track was removed/added
			pMIDIMgr->m_pTimeline->NotifyStripMgrs( CLSID_DirectMusicPatternTrack, pMIDIMgr->m_dwOldGroupBits | pioGroupBitsPPG->dwGroupBits, NULL );

			// Update m_dwOldGroupBits
			pMIDIMgr->m_dwOldGroupBits = pioGroupBitsPPG->dwGroupBits;

			// Set the Chord track's EditorMode (if it's not already set)
			pMIDIMgr->UpdateChordTrackEditorMode();

			// No need to reset m_fInSetData, since "this" has been deleted
			return S_OK;
		}

		m_fInSetData = FALSE;

		return S_OK;
	}
		break;
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = static_cast<PPGTrackFlagsParams *>(pData);

		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pMIDIMgr->m_dwTrackExtrasFlags )
		{
			m_pMIDIMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_TRACKEXTRAS );
		}
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pMIDIMgr->m_dwProducerOnlyFlags )
		{
			m_pMIDIMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PRODUCERONLY );
		}

		m_fInSetData = FALSE;

		return S_OK;
	}
		break;
	case 2:
	{
		PianoRollData* pPRD = (PianoRollData*)pData;
		BOOL fRedrawAll = FALSE;
		BOOL fRedrawFunctionBar = FALSE;
		BOOL fPartChange = FALSE;

//		m_pPartRef->m_dwPChannel	= pPRD->dwPChannel; // Handled below
		m_crUnselectedNoteColor	= pPRD->crUnselectedNoteColor;
		m_crSelectedNoteColor	= pPRD->crSelectedNoteColor;
		m_crOverlappingNoteColor= pPRD->crOverlappingNoteColor;
		m_crAccidentalColor		= pPRD->crAccidentalColor;

		if( pPRD->dwVariations != m_dwVariations )
		{
			m_dwVariations = pPRD->dwVariations;
			if (!m_fDontRedraw)
			{
				InvalidateCurveStrips();
				InvalidateMarkerStrip();
			}
			fRedrawAll = TRUE;
		}

		if (pPRD->dblVerticalZoom != m_dblVerticalZoom)
		{
			fRedrawAll = TRUE;
			m_dblVerticalZoom		= pPRD->dblVerticalZoom;
			m_lMaxNoteHeight		= long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
		}

		if (pPRD->lVerticalScroll != m_lVerticalScroll)
		{
			fRedrawAll = TRUE;
			m_lVerticalScroll		= pPRD->lVerticalScroll;
		}

		if (pPRD->StripView != m_StripView)
		{
			fRedrawAll = TRUE;
			m_StripView				= pPRD->StripView;
		}

		m_lBeatClocks = DM_PPQNx4 / pPRD->ts.m_bBeat;
		m_lMeasureClocks = m_lBeatClocks * pPRD->ts.m_bBeatsPerMeasure;
		m_lGridClocks = m_lBeatClocks / pPRD->ts.m_wGridsPerBeat;

		if ((m_dwVariations & ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled) == ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
		{
			m_fGutterPressed = TRUE;
		}
		else
		{
			m_fGutterPressed = FALSE;
		}

		// Length
		if( m_pPartRef->m_pDMPart->m_wNbrMeasures != pPRD->wNbrMeasures )
		{
			if( !fPartChange )
			{
				fPartChange = TRUE;
				m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			}

			m_pPartRef->m_pDMPart->SetNbrMeasures( pPRD->wNbrMeasures );

			// update the corresponding DirectMusicPart
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Refresh the other strips
			m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations, TRUE, TRUE );

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_LENGTH );
		}

		// Time signature
		if( m_pPartRef->m_pDMPart->m_TimeSignature.m_bBeatsPerMeasure != pPRD->ts.m_bBeatsPerMeasure
		||  m_pPartRef->m_pDMPart->m_TimeSignature.m_bBeat != pPRD->ts.m_bBeat
		||  m_pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat != pPRD->ts.m_wGridsPerBeat )
		{
			if( !fPartChange )
			{
				fPartChange = TRUE;
				m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			}

			// Change Part's time signature
			m_pPartRef->m_pDMPart->SetTimeSignature( pPRD->ts );
			pPRD->wNbrMeasures = m_pPartRef->m_pDMPart->m_wNbrMeasures;
			
			// Update the corresponding DirectMusicPart
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Refresh the other strips
			m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations, TRUE, TRUE );

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_TIMESIG );
		}

		// Inversion boundaries
		if( m_pPartRef->m_pDMPart->m_bInvertUpper != pPRD->bInvertUpper
		||  m_pPartRef->m_pDMPart->m_bInvertLower != pPRD->bInvertLower
		||  m_pPartRef->m_pDMPart->m_bAutoInvert!= pPRD->bAutoInvert )
		{
			if( !fPartChange )
			{
				fPartChange = TRUE;
				m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			}

			// Change Part's inversion boundaries
			m_pPartRef->m_pDMPart->m_bInvertUpper = pPRD->bInvertUpper;
			m_pPartRef->m_pDMPart->m_bInvertLower = pPRD->bInvertLower;
			m_pPartRef->m_pDMPart->m_bAutoInvert = pPRD->bAutoInvert;

			// update the corresponding DirectMusicPart
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Refresh the other strips
//			m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations );
			
			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_INV_BOUNDARIES );
		}

		// Play mode
		if (m_pPartRef->m_pDMPart->m_bPlayModeFlags != pPRD->bPlayModeFlags)
		{
			if( !fPartChange )
			{
				fPartChange = TRUE;
				m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			}

			// Update the Part playmode
			m_pPartRef->m_pDMPart->m_bPlayModeFlags = pPRD->bPlayModeFlags;

			if ( pPRD->bPlayModeFlags == DMUS_PLAYMODE_FIXED )
			{
				// Change to DMUS_PLAYMODE_FIXED
				CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
				for( ; pEvent; pEvent = pEvent->GetNext() )
				{
					CDirectMusicStyleNote *pDMNote = (CDirectMusicStyleNote *)pEvent;
					if ( pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
					{
						//pDMNote->m_wMusicValue = m_pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
						pDMNote->m_wMusicValue = pDMNote->m_bMIDIValue;
					}
				}
			}
			else
			{
				// Change to anything other than DMUS_PLAYMODE_FIXED
				CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
				for( ; pEvent; pEvent = pEvent->GetNext() )
				{
					CDirectMusicStyleNote *pDMNote = (CDirectMusicStyleNote *)pEvent;
					if ( pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
					{
						//pDMNote->m_bMIDIValue = (BYTE)pDMNote->m_wMusicValue;
						pDMNote->m_wMusicValue = m_pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
						pDMNote->m_bMIDIValue = m_pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
					}
				}
			}

			// update the corresponding DirectMusicPart
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Refresh the other strips
			m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, m_dwVariations, FALSE, FALSE );

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PLAYMODE );
			
			// Update the Note property page, if there is one
			// This update will occur when the Note property page gains focus (the part
			// property page must currently has focus).
			//m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
			//m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
		}

		// PChannel
		if( m_pPartRef->m_dwPChannel != pPRD->dwPChannel )
		{
			m_pPartRef->m_dwPChannel = pPRD->dwPChannel;

			m_fInSetData = FALSE;
			m_pMIDIMgr->UpdatePositionOfStrips( m_pPartRef );
			m_fInSetData = TRUE;

			// Let the object know about the changes
			m_pMIDIMgr->m_fPChannelChange = TRUE;
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PCHANNEL );
		}
		
		// PChannel name
		WCHAR wstrName[MAX_PATH];
		if( m_pMIDIMgr->m_pIPChannelName &&
			SUCCEEDED( m_pMIDIMgr->m_pIPChannelName->GetPChannelName( m_pPartRef->m_dwPChannel, wstrName) ) )
		{
			CString strPChName = wstrName;
			if( strPChName.Compare( pPRD->cstrPChannelName ) != 0 )
			{
				fRedrawFunctionBar = TRUE;

				if( m_pMIDIMgr->m_pIPChannelName )
				{
					MultiByteToWideChar( CP_ACP, 0, pPRD->cstrPChannelName, -1, wstrName, MAX_PATH );

					m_pMIDIMgr->m_pIPChannelName->SetPChannelName( m_pPartRef->m_dwPChannel, wstrName );
					fRedrawFunctionBar = TRUE;
				}

				// Let the object know about the changes
				// This doesn't affect anything in the style - just the PChannel names
				// in the Producer project file.
				/*
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_NAME );
				*/
			}
		}

		// PartRef name
		if( m_pPartRef->m_strName.Compare( pPRD->cstrPartRefName ) != 0 )
		{
			m_pPartRef->m_strName = pPRD->cstrPartRefName;
			// No need - the strip will be redrawn when it is removed and re-added below
			//fRedrawFunctionBar = TRUE;

			m_fInSetData = FALSE;
			m_pMIDIMgr->UpdatePositionOfStrips( m_pPartRef );
			m_fInSetData = TRUE;

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_NAME );
		}

		// SubChordLevel
		if( m_pPartRef->m_bSubChordLevel != pPRD->bSubChordLevel )
		{
			// Assume this will change the note values, so make a new Part if we need to
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );

			m_pPartRef->m_bSubChordLevel = pPRD->bSubChordLevel;
			m_pPartRef->RecalcMusicValues();
			// Need to update MIDI values as well - they may have chanaged (if notes
			// are now below the bottom of the chord, aren't in the key, etc.)
			m_pPartRef->RecalcMIDIValues();

			// Update inversion boundaries
			m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PLAYMODE );
		}

		// Variation lock ID
		if( m_pPartRef->m_bVariationLockID != pPRD->bVariationLockID )
		{
			m_pPartRef->m_bVariationLockID = pPRD->bVariationLockID;

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_VAR_LOCK ); 
		}

		// Variation order
		if( m_pPartRef->m_bRandomVariation != pPRD->bRandomVariation )
		{
			m_pPartRef->m_bRandomVariation = pPRD->bRandomVariation;

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_VAR_RANDOM ); 
		}

		// Extension or pickup bars
		if( (m_dwExtraBars != pPRD->dwExtraBars)
		||	(m_fPickupBar != pPRD->fPickupBar) )
		{
			// Update the number of extra bars
			DWORD dwOldNumber = m_dwExtraBars;
			m_dwExtraBars = pPRD->dwExtraBars;
			m_dwExtraBars = GetNumExtraBars();

			// Update the pickup bar flag
			m_fPickupBar = pPRD->fPickupBar;
			m_fPickupBar = ShouldDisplayPickupBar();

			// Only redraw and update if the number or flag actually changed
			if( (m_dwExtraBars != dwOldNumber)
			||	(m_fPickupBar == pPRD->fPickupBar) )
			{
				// Let the object know about the changes
				int nChange;

				// If extension bar length changed
				if( m_dwExtraBars != dwOldNumber )
				{
					nChange = IDS_UNDO_EXTENSION;
				}
				// Otherwise, pickup bar flag changed
				else
				{
					nChange = m_fPickupBar ? IDS_UNDO_SHOW_PICKUP : IDS_UNDO_HIDE_PICKUP;
				}
				m_pMIDIMgr->UpdateOnDataChanged( nChange ); 
				fRedrawAll = TRUE;
			}
		}

		if (!m_fDontRedraw)
		{
			if (fRedrawAll)
			{
				m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			else
			{
				if( fRedrawFunctionBar )
				{
					InvalidateFunctionBar();
				}
				InvalidatePianoRoll();
				InvalidateVariationBar();
			}
		}

		m_fInSetData = FALSE;

		return S_OK;
		break;
	}
	}

	m_fInSetData = FALSE;
	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_fPianoRollMenu && !m_fInShowProperties)
	{
		m_fInShowProperties = TRUE;
		HRESULT hr = m_pMIDIMgr->OnShowProperties();
		m_fInShowProperties = FALSE;
		return hr;
	}

	// Get a pointer to the Timeline
	ASSERT( m_pMIDIMgr->m_pTimeline );
	if( !m_pMIDIMgr->m_pTimeline )
	{
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		// release our reference to the property sheet
		pIPropSheet->Release();
		ASSERT( m_pPropPageMgr != NULL );
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		m_pPropPageMgr = new CPianoRollPropPageMgr;
		if( NULL == m_pPropPageMgr )
		{
			// release our reference to the property sheet
			pIPropSheet->Release();
			return E_OUTOFMEMORY;
		}
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus;
	hwndHadFocus = ::GetFocus();

	// Set the Pattern page flag appropriately
	m_pPropPageMgr->m_fShowPatternPropPage = (m_pMIDIMgr->m_pIDMTrack == NULL) ? FALSE : TRUE;

	// Set the property page to refer to the Piano Roll property page.
	short nActiveTab;
	nActiveTab = CPianoRollPropPageMgr::sm_nActiveTab;
	m_pMIDIMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// If the pattern tab exists, point it to m_pMIDIMgr->m_pDMPattern
	if( m_pPropPageMgr->m_pTabPatternPattern )
	{
		m_pPropPageMgr->m_pTabPatternPattern->SetPattern( m_pMIDIMgr->m_pDMPattern );
	}

	// Set the property page to refer to the groupbits, part, or pattern property page.
	pIPropSheet->SetActivePage( nActiveTab ); 
	pIPropSheet->Release();

	// Restore the focus if it has changed
	if( hwndHadFocus != ::GetFocus() )
	{
		::SetFocus( hwndHadFocus );
	}

	m_fPropPageActive = TRUE;

	return hr;
}

HRESULT STDMETHODCALLTYPE CPianoRollStrip::OnRemoveFromPageManager( void)
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
// CPianoRollStrip IDMUSProdTimelineEdit

HRESULT CPianoRollStrip::Cut( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr;

	hr = CanCut();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	hr = Copy( pITimelineDataObject );
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}

HRESULT CPianoRollStrip::Copy( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr;

	hr = CanCopy();
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMIDIMgr != NULL );
	if(m_pMIDIMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_UNEXPECTED;
	}

	// Get a framework pointer
	VARIANT var;
	if (FAILED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var )))
	{
		return E_UNEXPECTED;
	}

	IDMUSProdFramework* pIFramework = NULL;
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	ASSERT( pIFramework != NULL );

	// Create an IStream to save the selected notes in.
	IStream	*pStreamCopy;
	hr = pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
	if(FAILED(hr))
	{
		pIFramework->Release();
		return E_OUTOFMEMORY;
	}

	// Create an IStream to save the selected notes for the sequence track in
	IStream	*pStreamSeq;
	hr = pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamSeq );
	if(FAILED(hr))
	{
		pStreamCopy->Release();
		pIFramework->Release();
		return E_OUTOFMEMORY;
	}

	// Create an IStream to save the selected notes for a standard MIDI file in
	IStream	*pStreamMIDI;
	hr = pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamMIDI );
	pIFramework->Release();
	if(FAILED(hr))
	{
		pStreamCopy->Release();
		pStreamSeq->Release();
		return E_OUTOFMEMORY;
	}

	ULONG cbWritten;
	PianoRollClipboardData prcd;

	prcd.dwPChannel = m_pPartRef->m_dwPChannel;
	prcd.dwVariations = m_dwVariations;
	prcd.ts = m_pPartRef->m_pDMPart->m_TimeSignature;
	prcd.bPlayModeFlags = m_pPartRef->m_pDMPart->m_bPlayModeFlags;

	hr = pStreamCopy->Write( &prcd, sizeof(PianoRollClipboardData), &cbWritten);
	if (cbWritten != sizeof(PianoRollClipboardData))
	{
		pStreamCopy->Release();
		pStreamSeq->Release();
		pStreamMIDI->Release();
		return E_UNEXPECTED;
	}

	UINT cfClipboardFormat;

	if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0))
	{
		// Save the events into the stream
		hr = m_pMIDIMgr->SaveSelectedEvents(m_pPartRef, m_dwVariations, m_lBeginTimelineSelection, m_lEndTimelineSelection, pStreamCopy);
		if(FAILED(hr))
		{
			pStreamCopy->Release();
			pStreamSeq->Release();
			pStreamMIDI->Release();
			return E_UNEXPECTED;
		}

		// Save the events into the sequence event stream
		hr = m_pMIDIMgr->SaveSelectedEventsForSeqTrack(m_pPartRef, m_dwVariations, m_lBeginTimelineSelection, pStreamSeq);
		if(FAILED(hr))
		{
			pStreamCopy->Release();
			pStreamSeq->Release();
			pStreamMIDI->Release();
			return E_UNEXPECTED;
		}

		// If we're the first selected strip, save events from all tracks into a MIDI file
		if( FirstGutterSelectedPianoRollStrip() )
		{
			long lStartGrid = CLOCKS_TO_GRID(m_lBeginTimelineSelection, m_pPartRef->m_pDMPart);
			hr = m_pMIDIMgr->SaveSelectedEventsInAllSelectedStripsForMidi(lStartGrid, pStreamMIDI);
			if(FAILED(hr))
			{
				pStreamCopy->Release();
				pStreamSeq->Release();
				pStreamMIDI->Release();
				return E_UNEXPECTED;
			}
		}
		else
		{
			pStreamMIDI->Release();
			pStreamMIDI = NULL;
		}

		// Set the clipboard format to add
		ASSERT( theApp.m_cfAllEventFormat != 0 );
		cfClipboardFormat = theApp.m_cfAllEventFormat;

		ASSERT( theApp.m_cfSeqTrack != 0 );
		ASSERT( theApp.m_cfMIDIFile != 0 );
	}
	else
	{
		// Compute the earliest grid of the selected notes
		long lStartGrid = m_pMIDIMgr->EarliestSelectedNote( m_pPartRef->m_pDMPart, m_dwVariations);

		// Save just the notes into the stream.
		hr = m_pMIDIMgr->SaveSelectedNoteList(m_pPartRef, m_dwVariations, lStartGrid, pStreamCopy);
		if(FAILED(hr))
		{
			pStreamCopy->Release();
			pStreamSeq->Release();
			pStreamMIDI->Release();
			return E_UNEXPECTED;
		}

		// Save just the notes into the stream.
		hr = m_pMIDIMgr->SaveSelectedNoteListForSeqTrack(m_pPartRef, m_dwVariations, lStartGrid, pStreamSeq);
		if(FAILED(hr))
		{
			pStreamCopy->Release();
			pStreamSeq->Release();
			pStreamMIDI->Release();
			return E_UNEXPECTED;
		}

		// Save just the notes into the stream.
		hr = m_pMIDIMgr->SaveSelectedNotesForMidi(m_pPartRef, m_dwVariations, lStartGrid, pStreamMIDI);
		if(FAILED(hr))
		{
			pStreamCopy->Release();
			pStreamSeq->Release();
			pStreamMIDI->Release();
			return E_UNEXPECTED;
		}

		// Set the clipboard format to add
		ASSERT( theApp.m_cfNoteFormat != 0 );
		cfClipboardFormat = theApp.m_cfNoteFormat;

		ASSERT( theApp.m_cfSeqTrack != 0 );
		ASSERT( theApp.m_cfMIDIFile != 0 );
	}

	// add the stream to the passed IDMUSProdTimelineDataObject
	if(pITimelineDataObject != NULL)
	{
		hr = pITimelineDataObject->AddInternalClipFormat( cfClipboardFormat, pStreamCopy );
		pStreamCopy->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pStreamSeq->Release();
			if( pStreamMIDI )
			{
				pStreamMIDI->Release();
			}
			return E_FAIL;
		}

		hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfSeqTrack, pStreamSeq );
		pStreamSeq->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			if( pStreamMIDI )
			{
				pStreamMIDI->Release();
			}
			return E_FAIL;
		}

		if( pStreamMIDI )
		{
			hr = pITimelineDataObject->AddExternalClipFormat( theApp.m_cfMIDIFile, pStreamMIDI );
			pStreamMIDI->Release();
			ASSERT( hr == S_OK );
			if ( hr != S_OK )
			{
				return E_FAIL;
			}
		}
	}
	// Otherwise, add it to the clipboard
	else
	{
		// create timeline object
		hr = m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if( hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and end time of this copy
		long lStartTime, lEndTime;
		m_pPartRef->m_pDMPart->GetBoundariesOfSelectedNotes( m_dwVariations, lStartTime, lEndTime);
		hr = pITimelineDataObject->SetBoundaries(lStartTime, lEndTime);

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( cfClipboardFormat, pStreamCopy );
		pStreamCopy->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			pStreamSeq->Release();
			if( pStreamMIDI )
			{
				pStreamMIDI->Release();
			}
			return E_FAIL;
		}

		hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfSeqTrack, pStreamSeq );
		pStreamSeq->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			if( pStreamMIDI )
			{
				pStreamMIDI->Release();
			}
			return E_FAIL;
		}

		if( pStreamMIDI )
		{
			hr = pITimelineDataObject->AddExternalClipFormat( theApp.m_cfMIDIFile, pStreamMIDI );
			pStreamMIDI->Release();
			ASSERT( hr == S_OK );
			if ( hr != S_OK )
			{
				pITimelineDataObject->Release();
				return E_FAIL;
			}
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
		if( hr != S_OK )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// If we already have a CopyDataObject, release it
		if(m_pMIDIMgr->m_pCopyIDataObject)
		{
			m_pMIDIMgr->m_pCopyIDataObject->Release();
		}

		// set m_pCopyIDataObject to the object we just copied to the clipboard
		m_pMIDIMgr->m_pCopyIDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pCopyDataObject->AddRef
	}

	return hr;
}

HRESULT CPianoRollStrip::Paste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pMIDIMgr->m_pTimeline->GetPasteType( &m_pMIDIMgr->m_ptPasteType );

	HRESULT				hr;
	IStream*			pStreamPaste;

	// Make sure we can paste
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMIDIMgr != NULL );
	if(m_pMIDIMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_UNEXPECTED;
	}

	ASSERT( theApp.m_cfNoteFormat != 0 );

	// If true, split a MIDI file out into the gutter selected strips
	BOOL fPasteMIDIToMultipleStrips = FALSE;

	// The time to begin the paste at
	long lBeginTime;

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
		hr = m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		RELEASE( pIDataObject );
		if( FAILED(hr) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// If this is a single-strip paste, use our m_lInsertTime position, since it should already have
		// been set using the snap-to value
		lBeginTime = m_lInsertTime;
	}
	else
	{
		fPasteMIDIToMultipleStrips = TRUE;

		// If this is a multiple-strip paste, then use the Time Cursor, since it should already have
		// been set using the snap-to value
		if (FAILED(m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lBeginTime )))
		{
			return E_FAIL;
		}

		pITimelineDataObject->AddRef();
	}

	// Convert the time to insert at to a grid position
	lBeginTime = CLOCKS_TO_GRID( lBeginTime, m_pPartRef->m_pDMPart );

	// Check for NoteAndCurvesAndMarkers together
	if( pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfAllEventFormat) == S_OK )
	{
		hr = pITimelineDataObject->AttemptRead(theApp.m_cfAllEventFormat, &pStreamPaste);
		if(hr != S_OK)
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// 22989: Suck up the first copy of the data in Sequence format, if it's available.
		if( S_OK ==	pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfSeqTrack) )
		{
			IStream* pIStreamSequence;
			if(SUCCEEDED (pITimelineDataObject->AttemptRead( theApp.m_cfSeqTrack, &pIStreamSequence)))
			{
				pIStreamSequence->Release();
				pIStreamSequence = NULL;
			}
		}

		// 22989: Suck up the first copy of the data in MIDI format, if it's available.
		if( S_OK ==	pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfMIDIFile) )
		{
			IStream* pIStreamMIDI;
			if(SUCCEEDED (pITimelineDataObject->AttemptRead( theApp.m_cfMIDIFile, &pIStreamMIDI)))
			{
				pIStreamMIDI->Release();
				pIStreamMIDI = NULL;
			}
		}

		// Unselect all
		m_pMIDIMgr->UnselectAllEvents( m_pPartRef->m_pDMPart, ALL_VARIATIONS );

		PianoRollClipboardData prcd;
		if( FAILED( ReadPRCDFromStream( pStreamPaste, &prcd ) ) )
		{
			pITimelineDataObject->Release();
			pStreamPaste->Release();
			return E_FAIL;
		}

		BOOL fChanged = FALSE;

		// If doing paste->Overwrite, delete the notes and curves in the region that we're pasting over
		if ( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
		{
			long lStart, lEnd;
			if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
			{
				// Convert from ticks to grids for the end time
				lEnd = lBeginTime + CLOCKS_TO_GRID( lEnd - lStart, m_pPartRef->m_pDMPart );

				m_pMIDIMgr->DeleteNotesBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
				m_pMIDIMgr->DeleteCurvesBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
				m_pMIDIMgr->DeleteMarkersBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
			}
		}

		pITimelineDataObject->Release();

		// Paste notes and curves and markers
		// If necessary, this updates the pattern editor (based on fChanged)
		hr = m_pMIDIMgr->ImportEventList( m_pPartRef, m_dwVariations, &prcd, pStreamPaste, lBeginTime, fChanged );
	}
	// Check for just notes
	else if( pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfNoteFormat) == S_OK )
	{
		hr = pITimelineDataObject->AttemptRead(theApp.m_cfNoteFormat, &pStreamPaste);
		if(hr != S_OK)
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// Unselect all
		m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );

		PianoRollClipboardData prcd;
		if( FAILED( ReadPRCDFromStream( pStreamPaste, &prcd ) ) )
		{
			pITimelineDataObject->Release();
			pStreamPaste->Release();
			return E_FAIL;
		}

		BOOL fChanged = FALSE;

		// If doing paste->Overwrite, delete the notes in the region that we're pasting over
		if ( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
		{
			long lStart, lEnd;
			if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
			{
				// Convert from ticks to grids for the end time
				lEnd = lBeginTime + CLOCKS_TO_GRID( lEnd - lStart, m_pPartRef->m_pDMPart );

				m_pMIDIMgr->DeleteNotesBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
			}
		}

		pITimelineDataObject->Release();

		// Find out the size of this stream (statstg.cbSize.LowPart)
		STATSTG statstg;
		pStreamPaste->Stat( &statstg, STATFLAG_NONAME );

		// Paste just notes
		// If necessary, this updates the pattern editor (based on fChanged and TRUE parameter)
		hr = m_pMIDIMgr->ImportNoteList( m_pPartRef, m_dwVariations, &prcd, pStreamPaste, lBeginTime, statstg.cbSize.LowPart, fChanged, TRUE );
	}
	// Check for sequence format
	else if( pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfSeqTrack) == S_OK )
	{
		hr = pITimelineDataObject->AttemptRead(theApp.m_cfSeqTrack, &pStreamPaste);
		if(hr != S_OK)
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// 22989: Suck up the first copy of the data in MIDI format, if it's available.
		if( S_OK ==	pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfMIDIFile) )
		{
			IStream* pIStreamMIDI;
			if(SUCCEEDED (pITimelineDataObject->AttemptRead( theApp.m_cfMIDIFile, &pIStreamMIDI)))
			{
				pIStreamMIDI->Release();
				pIStreamMIDI = NULL;
			}
		}

		// Unselect all
		m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );

		BOOL fChanged = FALSE;

		// If doing paste->Overwrite, delete the notes and curves in the region that we're pasting over
		if ( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
		{
			long lStart, lEnd;
			if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
			{
				// Convert from ticks to grids for the end time
				lEnd = lBeginTime + CLOCKS_TO_GRID( lEnd - lStart, m_pPartRef->m_pDMPart );

				m_pMIDIMgr->DeleteNotesBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
				m_pMIDIMgr->DeleteCurvesBetweenBoundaries( m_pPartRef, m_dwVariations, lBeginTime, lEnd, fChanged );
				// BUGBUG: Should we delete markers here, even though none will be pasted?
			}
		}

		pITimelineDataObject->Release();

		// Paste notes and curves from sequence track
		// If necessary, this updates the pattern editor (based on fChanged)
		hr = m_pMIDIMgr->ImportSeqEventList( m_pPartRef, m_dwVariations, pStreamPaste, lBeginTime, fChanged);
	}
	// Check for Standard MIDI File format
	else if( pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfMIDIFile) == S_OK )
	{
		hr = pITimelineDataObject->AttemptRead(theApp.m_cfMIDIFile, &pStreamPaste);
		pITimelineDataObject->Release();
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		if( fPasteMIDIToMultipleStrips )
		{
			if( !FirstGutterSelectedPianoRollStrip() )
			{
				// Already handled by the first pianoroll strip.
				hr = S_FALSE;
			}
			else
			{
				// Split the MIDI file into multiple tracks and paste them in the correct spots
				hr = m_pMIDIMgr->ImportMIDIFileToMultipleStrips( pStreamPaste );

				// Update the note property page
				m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
				m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
			}
			pStreamPaste->Release();
			return hr;
		}

		// Unselect all
		m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, ALL_VARIATIONS );

		// Paste notes and curves from MIDI file
		// If doing paste->Overwrite, ImportEventsFromMIDIFile deletes the notes and curves in the region that we're pasting over
		// If necessary, this updates the pattern editor (based on fChanged)
		hr = m_pMIDIMgr->ImportEventsFromMIDIFile( m_pPartRef, m_dwVariations, pStreamPaste, m_pMIDIMgr->m_ptPasteType, lBeginTime );
	}
	// we don't understand this format - return E_FAIL
	else
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	// Clean up.
	pStreamPaste->Release(); 

	// Refresh property page
	RefreshPropertyPage();

	return hr;
}

CDirectMusicStyleNote *CPianoRollStrip::CreateNoteToInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr == NULL)
	{
		return NULL;
	}

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return NULL;
	}
	
	// Can't inset a note into 0 variations
	if( m_dwVariations == 0 )
	{
		return NULL;
	}

	MUSIC_TIME mtStartTime = m_lInsertTime;
	if( mtStartTime > 0 )
	{
		mtStartTime %= m_pPartRef->m_pPattern->CalcLength();
		mtStartTime %= m_pPartRef->m_pDMPart->GetClockLength();
	}

	long lGrid;

	if (mtStartTime < 0)
	{
		lGrid = 0;
	}
	else
	{
		lGrid = CLOCKS_TO_GRID( mtStartTime, m_pPartRef->m_pDMPart );;
	}

	long lValue = m_lInsertVal;
	if (lValue < 0)
	{
		lValue = 0;
	}

	if (lValue > 127)
	{
		lValue = 127;
	}

	CDirectMusicStyleNote* pDMNote;
	pDMNote = new CDirectMusicStyleNote;
	if( pDMNote == NULL )
	{
		return NULL;
	}

	if( m_lInsertTime < 0 )
	{
		pDMNote->m_nTimeOffset = short(max(-m_lMeasureClocks, m_lInsertTime));
	}
	else
	{
		pDMNote->m_nTimeOffset = short(mtStartTime - GRID_TO_CLOCKS( lGrid, m_pPartRef->m_pDMPart ));
	}
	pDMNote->m_mtGridStart = lGrid;
	pDMNote->m_dwVariation = m_dwVariations;
	pDMNote->m_bVelocity = 100;
	pDMNote->m_bTimeRange = 0;
	pDMNote->m_bDurRange = 0;
	pDMNote->m_bVelRange = 0;
	pDMNote->m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
	pDMNote->m_bInversionId = 0;
	pDMNote->m_bNoteFlags = 0;
	pDMNote->m_bMIDIValue = (BYTE)lValue;
	pDMNote->m_wMusicValue = m_pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
	pDMNote->m_bMIDIValue = m_pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
	pDMNote->m_fSelected = TRUE;

	/// Minimum note duration is 1/2 grid
	pDMNote->m_mtDuration = max( GetSnapAmount() - 1, m_lGridClocks / 2 );

	return pDMNote;
}

HRESULT CPianoRollStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();

	CDirectMusicStyleNote* pDMNote = CreateNoteToInsert();
	if( pDMNote == NULL )
	{
		return E_OUTOFMEMORY;
	}

	m_pMIDIMgr->UnselectAllNotes( m_pPartRef->m_pDMPart, m_dwVariations );

	m_pMIDIMgr->InsertNote( m_pPartRef, pDMNote, TRUE );

	// Update m_prsSelecting
	m_pActiveDMNote = pDMNote;

	// Refresh note property page
	m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
	m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );

	// Bump Time cursor to the right
	SNAPTO snapTo = GetSnapToBoundary();
	if( snapTo == SNAP_NONE )
	{
		snapTo = SNAP_GRID;
	}
	BumpTimeCursor( TRUE, snapTo );

	m_pMIDIMgr->UpdateStatusBarDisplay();

	return S_OK;
}

HRESULT CPianoRollStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	/* This may be part of a Cut operation. (You can cut empty space, but you can't delete it)
	HRESULT			hr;
	hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}
	*/

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	ASSERT( m_pMIDIMgr != NULL );

	if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0))
	{
		m_pMIDIMgr->DeleteSelectedEvents( m_pPartRef, m_dwVariations );
	}
	else
	{
		m_pMIDIMgr->DeleteSelectedNotes( m_pPartRef, m_dwVariations );
	}

	// Refresh property page
	RefreshPropertyPage();

	return S_OK;
}

HRESULT CPianoRollStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr != NULL)
	{
		m_pMIDIMgr->SelectAllNotes( this, m_dwVariations );
	}

	return S_OK;
}

HRESULT CPianoRollStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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

HRESULT CPianoRollStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	// Check if anything is selected
	return CanDelete( );
}

HRESULT CPianoRollStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;

	ASSERT( m_pMIDIMgr != NULL );
	if( m_pMIDIMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( theApp.m_cfNoteFormat != 0 );
	ASSERT( theApp.m_cfAllEventFormat != 0 );
	ASSERT( theApp.m_cfSeqTrack != 0 );
	ASSERT( theApp.m_cfMIDIFile != 0 );

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
		hr = m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		RELEASE( pIDataObject );
		if( FAILED(hr) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}
	}
	else
	{
		pITimelineDataObject->AddRef();
	}

	hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfNoteFormat );
	if( hr != S_OK )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfAllEventFormat );
		if( hr != S_OK )
		{
			hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfSeqTrack );
			if( hr != S_OK )
			{
				hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfMIDIFile );
			}
		}
	}

	pITimelineDataObject->Release();

	if (hr == S_OK)
	{
		return S_OK;
	}

	return S_FALSE;
}

// Insert iff no notes are selected
HRESULT CPianoRollStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( m_StripView == SV_MINIMIZED 
	||   m_dwVariations == 0 )
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

HRESULT CPianoRollStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr != NULL)
	{
		// Check for selected notes
		CDirectMusicEventItem* pEvent = GetFirstNote();
		for( ; pEvent; pEvent = pEvent->GetNext() )
		{
			if ( (pEvent->m_dwVariation & m_dwVariations) &&
				 ( ((CDirectMusicStyleNote *)pEvent)->m_fSelected == TRUE ))
			{
				return S_OK;
			}
		}

		// If necessary, check for selected curves and markers
		if( (m_StripView == SV_MINIMIZED) || (m_fGutterSelected &&
			(m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0)) )
		{
			pEvent = GetFirstCurve();
			for( ; pEvent; pEvent = pEvent->GetNext() )
			{
				if ( (pEvent->m_dwVariation & m_dwVariations) &&
					 ( pEvent->m_fSelected == TRUE ) )
				{
					return S_OK;
				}
			}

			pEvent = GetFirstMarker();
			for( ; pEvent; pEvent = pEvent->GetNext() )
			{
				if ( (pEvent->m_fSelected == TRUE)
				&&	 MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), m_dwVariations) )
				{
					return S_OK;
				}
			}
		}

		// Nothing selected
		return S_FALSE;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT CPianoRollStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_dwVariations )
	{
		// Check for notes
		if( GetFirstNote() )
		{
			return S_OK;
		}

		// If necessary, check for curves
		if( m_StripView == SV_MINIMIZED )
		{
			if( GetFirstCurve() )
			{
				return S_OK;
			}

			if( GetFirstMarker() )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

HCURSOR CPianoRollStrip::GetResizeStartCursor(void)
{
	static HCURSOR hCursorResizeStart;
	if (!hCursorResizeStart)
		hCursorResizeStart = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_STARTEDIT) );
	return hCursorResizeStart;
}

HCURSOR CPianoRollStrip::GetResizeEndCursor(void)
{
	static HCURSOR hCursorResizeEnd;
	if (!hCursorResizeEnd)
		hCursorResizeEnd = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_ENDEDIT) );
	return hCursorResizeEnd;
}

HCURSOR CPianoRollStrip::GetArrowCursor(void)
{
	static HCURSOR hCursorArrow;
	if (!hCursorArrow)
		hCursorArrow = LoadCursor( NULL, IDC_ARROW );
	return hCursorArrow;
}

HCURSOR CPianoRollStrip::GetNSCursor(void)
{
	static HCURSOR hCursorSizeNS;
	if (!hCursorSizeNS)
		hCursorSizeNS = LoadCursor( NULL, IDC_SIZENS ); // North/south resize
	return hCursorSizeNS;
}

HCURSOR CPianoRollStrip::GetAllCursor(void)
{
	static HCURSOR hCursorSizeALL;
	if (!hCursorSizeALL)
		//hCursorSizeALL = LoadCursor( NULL, IDC_SIZEALL ); // All resize
		hCursorSizeALL = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_CURSOR_MOVE) );
	return hCursorSizeALL;
}

// return: whether anything changed or not
BOOL CPianoRollStrip::SelectNotesInRect( CRect* pRect, int *pnSelected )
{
	ASSERT( pRect );
	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() || !pRect )
	{
		return 0;
	}

	BOOL fChange = FALSE;
	int iRes = 0;
	CDirectMusicEventItem* pEvent = GetFirstNote();

	if ( pEvent != NULL)
	{
		pRect->NormalizeRect();

		// Initialize m_mtTimelineLengthForGetNoteRect
		{
			VARIANT varLength;
			if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
			{
				m_mtTimelineLengthForGetNoteRect = V_I4( &varLength );
			}
		}

		// Convert the start and end positions to clock times
		MUSIC_TIME mtStartSel, mtEndSel;
		m_pMIDIMgr->m_pTimeline->PositionToClocks( pRect->left, &mtStartSel );
		m_pMIDIMgr->m_pTimeline->PositionToClocks( pRect->right, &mtEndSel );

		// Offset the start and end times so we includes all times that mapped to the
		// start and end pixels
		{
			long lPixelLength;
			m_pMIDIMgr->m_pTimeline->PositionToClocks( 1, &lPixelLength );

			mtStartSel -= lPixelLength / 2;
			mtEndSel += lPixelLength - (lPixelLength / 2); // Necessary becuase lPixelLength may not be even
		}

		// Compute the length of a part
		long lPartLength = min( m_mtTimelineLengthForGetNoteRect, m_pPartRef->m_pDMPart->GetClockLength());

		// Calculate the number of grids to search through
		int nGridLength = CLOCKS_TO_GRID( lPartLength + m_lGridClocks - 1, m_pPartRef->m_pDMPart );

		// Compute the low and high MIDI values that pRect touch
		long lMIDILow, lMIDIHigh;
		if( m_fHybridNotation )
		{
			// Convert from pRect.bottom to a MIDI value
			lMIDILow = PositionToMIDIValue( pRect->bottom );

			// lMIDILow could be 128, if pRect->bottom is in the top half of G10
			if( lMIDILow > 127 )
			{
				lMIDILow = 127;
			}

			// Notes a full scale value below may overlap onto our space
			// lMIDILow may be a sharped scale value, so must subtract 3
			lMIDILow -= 3;
			if( lMIDILow < 0 )
			{
				lMIDILow = 0;
			}

			// Convert from pRect->top to a MIDI value
			lMIDIHigh = PositionToMIDIValue( pRect->top );

			// Notes a full scale value above may overlap onto our space
			// lMIDIHigh may be a flatted scale value, so must add 3
			lMIDIHigh += 3;
			if( lMIDIHigh > 127 )
			{
				lMIDIHigh = 127;
			}
		}
		else
		{
			lMIDILow = 127 - (pRect->bottom / m_lMaxNoteHeight);
			lMIDIHigh = 127 - (pRect->top / m_lMaxNoteHeight);
		}

		// Compute the length of the pattern
		long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

		// If we are asked to find a time that is beyond the end of our part (and our part is shorter
		// than the timeline), then we should loop.  (lPartLength will never be more than lPatternLength)
		bool fLoopingPart = lPartLength < m_mtTimelineLengthForGetNoteRect;

		// A rectangle defining the note
		RECT rectNote;

		for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
		{
			if( !fLoopingPart && (m_pPartRef->m_pDMPart->AbsTime( pEvent ) > mtEndSel) )
			{
				// Gone too far in list - all events after this should be unselected
				if( pEvent->m_fSelected )
				{
					pEvent->m_fSelected = FALSE;
					fChange = TRUE;
				}
				continue;
			}

			// Check to see if:
			//    it belongs to the correct variation
			//    it starts before the end of this part (or pattern or timeline, if the pattern or timeline is shorter)
			if ((pEvent->m_dwVariation & m_dwVariations) &&
				(pEvent->m_mtGridStart < nGridLength) )
			{
				// If the note is within the MIDI value range
				if ( (((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue <= lMIDIHigh) &&
					 (((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue >= lMIDILow) )
				{
					// Get the rectangle defining the note
					GetNoteRect( (CDirectMusicStyleNote*) pEvent, &rectNote);

					// Check to see if note is within our vertical selection range
					if( ((pRect->top <= rectNote.top) && (rectNote.top <= pRect->bottom)) ||
						((pRect->top <= rectNote.bottom) && (rectNote.bottom <= pRect->bottom)) ||
						((rectNote.top <= pRect->top) && (pRect->top <= rectNote.bottom)) ||
						((rectNote.top <= pRect->bottom) && (pRect->bottom <= rectNote.bottom)) )
					{
						// Check to see if note is within our horizontal selection range in the original location
						if( (pRect->left <= rectNote.left) && (rectNote.left <= pRect->right) )
						{
							iRes ++;
							if( !pEvent->m_fSelected )
							{
								pEvent->m_fSelected = TRUE;
								fChange = TRUE;
								if (m_pActiveDMNote == NULL)
								{
									m_pActiveDMNote = (CDirectMusicStyleNote*) pEvent;
								}
							}
						}
						else if( fLoopingPart )
						{
							// Compute the note start time
							const MUSIC_TIME mtOrigStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );

							// Compute the time of the note's grid position
							const MUSIC_TIME mtOrigGridStart = GRID_TO_CLOCKS( pEvent->m_mtGridStart, m_pPartRef->m_pDMPart );

							bool fDisplayNoteInShortPart;
							const MUSIC_TIME mtLastPartLen = lPatternLength % lPartLength;

							// If the part repeats, and the last repeat of the the part is less than the part's length
							if( mtLastPartLen )
							{
								long lLastGrid = CLOCKS_TO_GRID(mtLastPartLen - 1, m_pPartRef->m_pDMPart);

								// If the note will not play in the last part repeat
								if( pEvent->m_mtGridStart >= lLastGrid )
								{
									fDisplayNoteInShortPart = false;
								}
								else
								{
									fDisplayNoteInShortPart = true;
								}
							}
							else
							{
								fDisplayNoteInShortPart = true;
							}

							MUSIC_TIME mtOffset = 0;
							bool fFoundNote = false;
							long lStartPosition = LONG_MIN;
							const long lNumPartRepeats = (lPatternLength - 1) / lPartLength;

							while( mtOffset < m_mtTimelineLengthForGetNoteRect )
							{
								const MUSIC_TIME mtOffsetOrig = mtOffset;
								for( int i=0; i <= lNumPartRepeats; i++ )
								{
									// If we're not in the last part repeat, or if the note displays in the
									// last (short) part repeat
									if( (i != lNumPartRepeats)
									||	fDisplayNoteInShortPart )
									{
										// Check that the note will actually play
										if( mtOffset + mtOrigGridStart < m_mtTimelineLengthForGetNoteRect )
										{
											// Compute the start position of the note
											m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtOffset + mtOrigStart, &lStartPosition );
										}
									}
									if( (pRect->left <= lStartPosition) && (lStartPosition <= pRect->right) )
									{
										fFoundNote = true;
										break;
									}
									mtOffset += lPartLength;
								}
								if( fFoundNote )
								{
									break;
								}
								mtOffset = mtOffsetOrig + lPatternLength;
							}

							if( fFoundNote )
							{
								iRes ++;
								if( !pEvent->m_fSelected )
								{
									pEvent->m_fSelected = TRUE;
									fChange = TRUE;
									if ( !m_pActiveDMNote )
									{
										m_pActiveDMNote = (CDirectMusicStyleNote*) pEvent;
									}
								}
							}
							else if( pEvent->m_fSelected )
							{
								pEvent->m_fSelected = FALSE;
								fChange = TRUE;
							}
						}
						else if( pEvent->m_fSelected )
						{
							pEvent->m_fSelected = FALSE;
							fChange = TRUE;
						}
					}
					else if( pEvent->m_fSelected )
					{
						pEvent->m_fSelected = FALSE;
						fChange = TRUE;
					}
				}
				else if( pEvent->m_fSelected )
				{
					pEvent->m_fSelected = FALSE;
					fChange = TRUE;
				}
			}
			else if( pEvent->m_fSelected )
			{
				pEvent->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
	}

	if( pnSelected )
	{
		*pnSelected = iRes;
	}
	if( fChange )
	{
		m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, ALL_VARIATIONS );
		m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
		m_pMIDIMgr->UpdateStatusBarDisplay();
	}
	return fChange;
}

void CPianoRollStrip::DrawVariationButtonBar( HDC hDC )
{
	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return;
	}

	// Set the currently playing variation
	UpdatePlayingVariation();

	ASSERT( hDC );

	if( hDC )
	{
		RECT rectClip;
		::GetClipBox( hDC, &rectClip );

		if (rectClip.top > m_lVerticalScroll + VARIATION_BUTTON_HEIGHT*2)
		{
			return;
		}

		// Fill the area corresponding to the variation buttons with white, if the strip is not active
		if( (rectClip.right >= VARIATION_GUTTER_WIDTH + 16 * VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH) 
		&&	((m_pMIDIMgr->m_pActivePianoRollStrip != this)
			 || (m_pMIDIMgr->m_iFocus != 1)) )
		{
			COLORREF crOldBkColor = ::SetBkColor( hDC, RGB(255, 255, 255) );
			RECT rectOverwrite = { rectClip.left, rectClip.top, rectClip.right, m_lVerticalScroll + 2 * VARIATION_BUTTON_HEIGHT };
			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectOverwrite, NULL, 0, NULL);
			::SetBkColor( hDC, crOldBkColor );

		}

		if (rectClip.left > VARIATION_GUTTER_WIDTH +
							16 * VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH)
		{
			return;
		}

		if (m_fGutterPressed)
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapPressedGutter.GetSafeHandle() ), NULL, 0, m_lVerticalScroll,
						 VARIATION_GUTTER_WIDTH, VARIATION_GUTTER_HEIGHT, DST_BITMAP | DSS_NORMAL );
		}
		else
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapGutter.GetSafeHandle() ), NULL, 0, m_lVerticalScroll,
						 VARIATION_GUTTER_WIDTH, VARIATION_GUTTER_HEIGHT, DST_BITMAP | DSS_NORMAL );
		}

		if (m_fVariationChoicesPressed)
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapPressedMoaw.GetSafeHandle() ), NULL, VARIATION_GUTTER_WIDTH + 16*VARIATION_BUTTON_WIDTH, m_lVerticalScroll,
						 VARIATION_MOAW_WIDTH, VARIATION_MOAW_HEIGHT, DST_BITMAP | DSS_NORMAL );
		}
		else
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapMoaw.GetSafeHandle() ), NULL, VARIATION_GUTTER_WIDTH + 16*VARIATION_BUTTON_WIDTH, m_lVerticalScroll,
						 VARIATION_MOAW_WIDTH, VARIATION_MOAW_HEIGHT, DST_BITMAP | DSS_NORMAL );
		}

		LOGFONT lf;
		memset( &lf, 0 , sizeof(LOGFONT));
		lf.lfHeight = VARIATION_BUTTON_HEIGHT - 2;
		//lf.lfWidth = 0;
		//lf.lfEscapement = 0;
		//lf.lfOrientation = 0;
		lf.lfWeight = FW_HEAVY;
		//lf.lfItalic = FALSE;
		//lf.lfUnderline = FALSE;
		//lf.lfStrikeOut = FALSE;
		//lf.lfCharSet = ANSI_CHARSET;
		//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		//lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		//lf.lfFaceName = NULL;
		
		HFONT hfontBold, hfontNormal;
		hfontBold = ::CreateFontIndirect( &lf );
		if ( !hfontBold ) 
		{
			return;
		}
		lf.lfWeight = FW_MEDIUM;
		hfontNormal = ::CreateFontIndirect( &lf );
		if( !hfontNormal )
		{
			::DeleteObject( hfontBold );
			return;
		}

		int nOldBkMode;
		nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );

		COLORREF oldColor = ::GetTextColor( hDC );
		HFONT hfontOld= static_cast<HFONT> ( ::GetCurrentObject( hDC, OBJ_FONT ) );
		
		TCHAR tcstrTxt[10];
		int nXpos, nYpos;
		for (nYpos = 0; nYpos < 2; nYpos++)
		{
			for (nXpos = 0; nXpos < 16; nXpos++)
			{
				const int nVariation = nXpos + nYpos * 16;
				const bool fPressed = m_dwVariations & (1 << nVariation) ? true : false;

				LPARAM lParam;
				if (m_pPartRef->m_pDMPart->m_dwVariationsDisabled & (1 << nVariation))
				{
					if (fPressed)
					{
						lParam = reinterpret_cast<LPARAM>( m_BitmapPressedInactiveButton.GetSafeHandle() );
					}
					else
					{
						lParam = reinterpret_cast<LPARAM>( m_BitmapBlankInactiveButton.GetSafeHandle() );
					}
				}
				else
				{
					if (fPressed)
					{
						lParam = reinterpret_cast<LPARAM>( m_BitmapPressedButton.GetSafeHandle() );
					}
					else
					{
						lParam = reinterpret_cast<LPARAM>( m_BitmapBlankButton.GetSafeHandle() );
					}
				}
				::DrawState( hDC, NULL, NULL, lParam, NULL,
							 VARIATION_GUTTER_WIDTH + nXpos*VARIATION_BUTTON_WIDTH, m_lVerticalScroll + nYpos*VARIATION_BUTTON_HEIGHT,
							 VARIATION_BUTTON_WIDTH, VARIATION_BUTTON_HEIGHT, DST_BITMAP | DSS_NORMAL );
				
				RECT rect;
				rect.top = fPressed + m_lVerticalScroll + nYpos*VARIATION_BUTTON_HEIGHT + 3;
				rect.bottom = rect.top + VARIATION_BUTTON_HEIGHT - 6;
				rect.left = fPressed + VARIATION_GUTTER_WIDTH + nXpos*VARIATION_BUTTON_WIDTH + 2;
				rect.right = rect.left + VARIATION_BUTTON_WIDTH - 6;
				
				const bool fPlaying = m_dwPlayingVariation & (1 << nVariation) ? true : false;

				_itot( nXpos + nYpos * 16 + 1, tcstrTxt, 10 );
				if ( m_pPartRef->m_pDMPart->m_dwVariationHasNotes & (1 << nVariation) )
				{
					::SetTextColor( hDC, fPlaying ? RGB(255,0,0) : RGB(0, 0, 0) );
					::SelectObject( hDC, hfontBold );
				}
				else
				{
					::SetTextColor( hDC, fPlaying ? RGB(255,0,0) : RGB(255, 255, 255) );
					::SelectObject( hDC, hfontNormal );
				}
				::DrawText( hDC, tcstrTxt, -1, &rect, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOPREFIX );
			}
		}

		if( hfontOld )
		{
			::SelectObject( hDC, hfontOld );
		}
		::SetTextColor( hDC, oldColor );
		::SetBkMode( hDC, nOldBkMode );
		::DeleteObject( hfontBold );
		::DeleteObject( hfontNormal );
	}
}

void CPianoRollStrip::AdjustScroll(long lXPos, long lYPos)
{
	VARIANT var;
	long lHeight = 0;
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_HEIGHT, &var)))
	{
		lHeight = V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2;
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

	if ((lYPos < m_lVerticalScroll) && (m_lVerticalScroll > 0))
	{
		// Start Scroll up
		EnableTimer();
	}
	else if ( (lHeight > 0) && (lYPos > m_lVerticalScroll + lHeight) &&
			  (m_lVerticalScroll < lMaxHeight) )
	{
		// Start Scroll down
		EnableTimer();
	}
	else
	{
		// Check horizontal ranges

		// Get current scroll position
		long lHScroll = 0;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
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
			m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_STRIP_RECT, &var);

			// Compute the right side of the display
			long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

			// Compute the maximum scroll position
			long lMaxHScroll = 0;
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_pMIDIMgr->m_pDMPattern->CalcLength(), &lMaxHScroll );

			lMaxHScroll -= rectStrip.right - rectStrip.left;

			// Check for need to scroll right
			if( (lHScroll < lMaxHScroll) && (lXPos > lMaxScreenPos) )
			{
				// Start Scroll right
				EnableTimer();
			}
			else
			{
				// Mouse within screen position - disable timer
				KillTimer();
			}
		}
	}
}

void CPianoRollStrip::SetNewVerticalScroll( long lNewVertScroll )
{
	VARIANT varHeight;
	VERIFY( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &varHeight )) );
	int nHeight = max( 0, V_I4(&varHeight) + BORDER_VERT_WIDTH - 2 * VARIATION_BUTTON_HEIGHT);

	long lMaxVertScroll;
	lMaxVertScroll = m_lMaxNoteHeight * max( 0, (m_fHybridNotation ? 38 : 128) - nHeight / m_lMaxNoteHeight);

	if ( lNewVertScroll > lMaxVertScroll )
	{
		lNewVertScroll = lMaxVertScroll;
	}
	else if( lNewVertScroll < 0 )
	{
		lNewVertScroll = 0;
	}

	// If the scroll position didn't change, return
	if ( m_lVerticalScroll == lNewVertScroll )
	{
		return;
	}

	m_pMIDIMgr->m_fDirty = TRUE;

	long lLeftDisplay, lEndOfTime;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	
	CDC cDC;

	lEndOfTime = lLeftDisplay;

	// Get the DC of our Strip
	VARIANT var;
	if( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_GET_HDC, &var )) )
	{
		CWnd *pWnd = NULL;
		if( cDC.Attach( (HDC)(V_I4(&var)) ) != 0 )
		{
			RECT	rtStrip;

			cDC.GetClipBox( &rtStrip );
			lEndOfTime = lLeftDisplay + rtStrip.right + 1;
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
	}

	VERIFY( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) );
	lLeftDisplay -= V_I4(&var);

	// Pospone drawing operations - this avoids flickering
	// in the strip's title and maximize/minimize button
	HWND hwnd = GetTimelineHWnd();
	if( hwnd )
	{
		::LockWindowUpdate( hwnd );
	}

	// Invalidate the region displaying the button bar and the horizontal black line below it
	CRect rect(lLeftDisplay, m_lVerticalScroll, lEndOfTime, m_lVerticalScroll + 2*VARIATION_BUTTON_HEIGHT + HORIZ_LINE_HEIGHT);
	m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE);
	
	// Invalide the rectangle displaying the vertical scroll bar
	IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
	m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
	if (pIOleInPlaceObjectWindowless)
	{
		HWND hWnd;
		if (pIOleInPlaceObjectWindowless->GetWindow(&hWnd) == S_OK)
		{
			::InvalidateRect( hWnd, &m_rectScrollBar, FALSE );
		}
		pIOleInPlaceObjectWindowless->Release();
	}

	m_lVerticalScroll = lNewVertScroll;
	
	var.vt = VT_I4;
	V_I4(&var) = m_lVerticalScroll;
	m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );

	// restore drawing operations (and redraw the strip)
	if( hwnd )
	{
		::LockWindowUpdate( NULL );
	}

	if (m_prScrollBar.GetSafeHwnd())
	{
		m_prScrollBar.SetScrollPos( m_lVerticalScroll / m_lMaxNoteHeight );
	}
}

HRESULT	CPianoRollStrip::OnVariationBarMsg( UINT nMsg, WPARAM wParam, LPARAM lParam, long lXPos, long lYPos )
{
	UNREFERENCED_PARAMETER( lParam );
	HRESULT hr = S_OK;

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	long lLeftDisplay;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );

	// If there is no pickup bar, the variation bar is at least at time 0
	if( !m_fPickupBar )
	{
		lLeftDisplay = max( 0, lLeftDisplay );
	}

	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	const long lOrigXPos = lXPos;
	const long lOrigYPos = lYPos;

	lXPos = (lXPos - lLeftDisplay - VARIATION_GUTTER_WIDTH) / VARIATION_BUTTON_WIDTH;
	lYPos = (lYPos - m_lVerticalScroll) / VARIATION_BUTTON_HEIGHT;

	if ( (lOrigXPos < lLeftDisplay + VARIATION_GUTTER_WIDTH ) &&
		 (lOrigXPos > lLeftDisplay ) )
	{
		lXPos = -1;
	}
	if ( (lOrigXPos < lLeftDisplay + VARIATION_GUTTER_WIDTH + 16*VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH) &&
		 (lOrigXPos > lLeftDisplay + VARIATION_GUTTER_WIDTH + 16*VARIATION_BUTTON_WIDTH ) )
	{
		lXPos = -2;
	}
	if (lOrigYPos - m_lVerticalScroll < 0)
	{
		lYPos = -1;
	}

	switch( nMsg )
	{
	case WM_RBUTTONUP:
		if ((lXPos >= 0) && (lXPos < 16) && ((lYPos == 0) || (lYPos == 1)))
		{
			POINT pt;
			BOOL  bResult;
			
			// Get the cursor position (To put the menu there)
			bResult = GetCursorPos( &pt );
			ASSERT( bResult );
			if( !bResult )
			{
				hr = E_UNEXPECTED;
				break;
			}
			
			ASSERT( m_pMIDIMgr->m_pTimeline );
			if ( !m_pMIDIMgr->m_pTimeline)
			{
				hr = E_UNEXPECTED;
				break;
			}

			HMENU hMenu, hMenuPopup;
			hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_VARIATIONBAR_RMENU));
			if (hMenu == NULL)
			{
				hr = E_UNEXPECTED;
				break;
			}
			hMenuPopup = ::GetSubMenu( hMenu, 0 );
			if (hMenuPopup == NULL)
			{
				hr = E_UNEXPECTED;
				break;
			}

			m_pMIDIMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);

			DestroyMenu(hMenu); // This will destroy the submenu as well.
		}
		break;
	case WM_COMMAND:
		{
			WORD wID;
			wID			= LOWORD( wParam );	// item, control, or accelerator identifier 

			switch( wID )
			{
			case ID_ENABLE:
				// If the variations aren't already enabled
				if (m_pPartRef->m_pDMPart->m_dwVariationsDisabled & m_dwVariations)
				{
					// Update the Part's GUID (and create a new Part if we need to)
					m_pMIDIMgr->PreChangePartRef( m_pPartRef );

					// Enable all selected variations
					for (int i=0; i<32; i++)
					{
						// if variation is selected and disabled
						if ((m_dwVariations & (1 << i)) & m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
						{
							m_pPartRef->m_pDMPart->m_dwVariationsDisabled ^= 1 << i;
							m_pPartRef->m_pDMPart->m_dwVariationChoices[i] = m_pPartRef->m_pDMPart->m_dwDisabledChoices[i];
							// If all variation choices are turned off, then turn them all on
							if( (m_pPartRef->m_pDMPart->m_dwVariationChoices[i] & DM_VF_MODE_BITS) == DM_VF_MODE_IMA )
							{
								if( (m_pPartRef->m_pDMPart->m_dwVariationChoices[i] & IMA_VF_FLAG_BITS) == 0 )
								{
									m_pPartRef->m_pDMPart->m_dwVariationChoices[i] |= IMA_VF_FLAG_BITS;
								}
							}
							else
							{
								if( (m_pPartRef->m_pDMPart->m_dwVariationChoices[i] & DM_VF_FLAG_BITS) == 0 )
								{
									m_pPartRef->m_pDMPart->m_dwVariationChoices[i] |= DM_VF_FLAG_BITS;
								}
							}
							m_pPartRef->m_pDMPart->m_dwDisabledChoices[i] = 0;
						}
					}

					// update the corresponding DirectMusicPart
					m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

					// Update the other strips displaying this part
					m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, ALL_VARIATIONS, TRUE, TRUE );

					// Update the variation choices editor
					m_pMIDIMgr->RefreshVarChoicesEditor( m_pPartRef->m_pDMPart );

					// Let the object know about the changes
					m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_ENABLE_VARCHOICES );
				}
				break;
			case ID_DISABLE:
				// If the variations aren't already disabled
				if (~m_pPartRef->m_pDMPart->m_dwVariationsDisabled & m_dwVariations)
				{
					// Update the Part's GUID (and create a new Part if we need to)
					m_pMIDIMgr->PreChangePartRef( m_pPartRef );

					int nChanged = 0;
					for (int i=0; i<32; i++)
					{
						// if variation is selected and enabled
						if ((m_dwVariations & (1 << i)) & ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
						{
							nChanged++;
							m_pPartRef->m_pDMPart->m_dwVariationsDisabled ^= 1 << i;
							m_pPartRef->m_pDMPart->m_dwDisabledChoices[i] = m_pPartRef->m_pDMPart->m_dwVariationChoices[i];
							m_pPartRef->m_pDMPart->m_dwVariationChoices[i] &= DM_VF_MODE_BITS;
						}
					}

					// if everything is disabled, re-enable the first selected variation
					if (!~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
					{
						for (int i=0; i<32; i++)
						{
							// If variation is selected, re-enable it
							if (m_dwVariations & (1 << i))
							{
								nChanged--;
								m_pPartRef->m_pDMPart->m_dwVariationsDisabled ^= 1 << i;
								m_pPartRef->m_pDMPart->m_dwVariationChoices[i] = m_pPartRef->m_pDMPart->m_dwDisabledChoices[i];
								m_pPartRef->m_pDMPart->m_dwDisabledChoices[i] = 0;
								break;
							}
						}
					}

					// Always update the displayed variations to display only enabled variations
					m_dwVariations &= ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled;

					// if anything changed
					if (nChanged)
					{
						// update the corresponding DirectMusicPart
						m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

						// Update the other strips displaying this part
						m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, ALL_VARIATIONS, TRUE, TRUE );

						if( m_dwVariations == 0 )
						{
							// Specifically update this part.  This isn't handled by the above call, since we no
							// longer are displaying any variations

							// Tell the strip to recompute its selection state, since notes may
							// have been selected/unselected
							UpdateSelectionState();

							// Redraw the note region of the strip, curve strip(s), and marker strip
							InvalidatePianoRoll();
							InvalidateVariationBar();
							InvalidateCurveStrips();
							InvalidateMarkerStrip();
						}

						// Update the variation choices editor
						m_pMIDIMgr->RefreshVarChoicesEditor( m_pPartRef->m_pDMPart );

						// Let the object know about the changes
						m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_DISABLE_VARCHOICES );
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if ((lXPos >= 0) && (lXPos < 16) && ((lYPos == 0) || (lYPos == 1)))
		{
			m_nLastVariation = lXPos + lYPos * 16;
			if (m_nLastVariation != m_nSoloVariation)
			{
				m_fSolo = FALSE;
			}
			m_dwVariations ^= 1 << m_nLastVariation;
			m_fEnablingVariations = (1 << m_nLastVariation) & m_dwVariations;
			if ( m_fEnablingVariations != 0 )
			{
				m_fEnablingVariations = TRUE;
			}

			m_pMIDIMgr->SetVarsForPartPChannel( m_dwVariations, m_pPartRef );

			m_MouseMode = PRS_MM_ACTIVESELECT_VARIATIONS;

			// Refresh property page
			RefreshPropertyPage();

			// Let the object know about the changes
			m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
			m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
			m_pMIDIMgr->m_fDirty = TRUE;

			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		}
		else if ((lXPos == -1) && ((lYPos == 0) || (lYPos == 1)))
		{
			// Gutter clicked on
			m_fGutterPressed = !m_fGutterPressed;

			DWORD dwOrigVariations = m_dwVariations;
			if (m_fGutterPressed)
			{
				// Select all active variations
				m_dwVariations |= ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled;
			}
			else
			{
				// Deselect all active variations
				m_dwVariations &= m_pPartRef->m_pDMPart->m_dwVariationsDisabled;
			}

			m_pMIDIMgr->SetVarsForPartPChannel( m_dwVariations, m_pPartRef );

			if( m_dwVariations != dwOrigVariations )
			{
				// Refresh property page
				RefreshPropertyPage();

				// Let the object know about the changes
				m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
				m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
				m_pMIDIMgr->m_fDirty = TRUE;

				// No need to update performance engine
				m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_SET_VARIATIONS );
			}
		}
		else if ((lXPos == -2) && ((lYPos == 0) || (lYPos == 1)))
		{
			// Variation Choices clicked on
			m_fVariationChoicesPressed = TRUE;


			InvalidateVariationBar();
			m_MouseMode = PRS_MM_ACTIVEPRESS_VARIATIONCHOICES;

			// Tell the Pattern editor to display the variation choices dialog
			VARIANT var;
			if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )))
			{
				IPatternNodePrivate *pPatternNodePrivate;
				if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IPatternNodePrivate, (void**)&pPatternNodePrivate ) ) )
				{
					pPatternNodePrivate->DisplayVariationChoicesDlg( m_pPartRef->m_pDMPart->m_guidPartID, m_pPartRef->m_dwPChannel );
					pPatternNodePrivate->Release();
				}
				else
				{
					SegmentDisplayVarChoicesDlg();
				}
				V_UNKNOWN(&var)->Release();
			}

			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		}
		break;
	case WM_LBUTTONDBLCLK:
		if ((lXPos >= 0) && (lXPos < 16) && ((lYPos == 0) || (lYPos == 1)))
		{
			if (m_fSolo)
			{
				m_fSolo = FALSE;
				m_dwVariations = m_dwOldVariations;
			}
			else
			{
				m_fSolo = TRUE;
				m_nSoloVariation = lXPos + lYPos * 16;
				m_dwVariations ^= 1 << m_nSoloVariation;
				m_dwOldVariations = m_dwVariations;
				m_dwVariations = 1 << m_nSoloVariation;
			}

			m_pMIDIMgr->SetVarsForPartPChannel( m_dwVariations, m_pPartRef );

			// Refresh property page
			RefreshPropertyPage();

			// Let the object know about the changes
			m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
			m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
			m_pMIDIMgr->m_fDirty = TRUE;

			// No need to update performance engine
			m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_SET_VARIATIONS );
		}
		break;
	case WM_LBUTTONUP:
		{
			// Stop any playing note
			m_pMIDIMgr->StopNote();

			KillTimer();

			// Pop the variation choices button back up if it was pressed
			if ( m_MouseMode == PRS_MM_ACTIVEPRESS_VARIATIONCHOICES )
			{
				m_fVariationChoicesPressed = FALSE;
				InvalidateVariationBar();
			}
			m_nLastVariation = -1;
			if( m_MouseMode == PRS_MM_ACTIVESELECT_VARIATIONS )
			{
				// BUGBUG: Need to update which variations are played!
				// No need to update performance engine
				m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_SET_VARIATIONS );
			}
			/* Already done in the Timeline.
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			*/
			m_MouseMode = PRS_MM_NORMAL;
		}
		break;
	case WM_SETCURSOR:
		m_hCursor = GetArrowCursor();
		break;
	case WM_MOUSEMOVE:
		switch(m_MouseMode)
		{
		case PRS_MM_ACTIVESELECT_VARIATIONS:
			if ((lXPos >= 0) && (lXPos < 16) && ((lYPos == 0) || (lYPos == 1)) &&
				(m_nLastVariation != lXPos + lYPos * 16))
			{
				m_nLastVariation = lXPos + lYPos * 16;
				if ( m_fEnablingVariations )
				{
					m_dwVariations |= 1 << m_nLastVariation;
				}
				else
				{
					m_dwVariations &= ~(1 << m_nLastVariation);
				}

				m_pMIDIMgr->SetVarsForPartPChannel( m_dwVariations, m_pPartRef );

				// Refresh property page
				RefreshPropertyPage();

				// Let the object know about the changes
				m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
				m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
				m_pMIDIMgr->m_fDirty = TRUE;
				// Update on LButtonUp
//				m_pMIDIMgr->UpdateOnDataChanged( NULL );
			}
			if ((m_nLastVariation != -1) && ((lXPos < 0) || (lXPos > 15) || (lYPos < 0) || (lYPos > 1)))
			{
				m_nLastVariation = -1;
			}
			break;
		default:
			/*
			if ((lXPos == -2) && ((lYPos == 0) || (lYPos == 1)))
			{
				//BUGBUG: Display ToolTip
			}
			*/
			break;
		}
		break;
	default:
		break;
	}

	return S_OK;
}

void CPianoRollStrip::InvalidatePianoRoll( void )
{
	if ( m_StripView == SV_MINIMIZED )
	{
		m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, FALSE );
		return;
	}

	// Invalidate the piano roll
	RECT rect;
	long lLeftDisplay;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	rect.left = lLeftDisplay;
	rect.top = m_lVerticalScroll + 2 * VARIATION_BUTTON_HEIGHT;
	
	// Find the right and bottom boundaries of our strip					
	CDC cDC;
	VARIANT var;
	var.vt = VT_I4;
	if( FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_GET_HDC, &var )) )
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

	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )))
	{
		rect.bottom = m_lVerticalScroll + V_I4(&var);
	}

	// Really invalidate the piano roll
	// Don't need to erase, since the horizontal bars will overwrite the area
	m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

void CPianoRollStrip::InvalidateFunctionBar( void )
{
	if ( m_StripView == SV_MINIMIZED )
	{
		m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, FALSE );
		return;
	}

	// Invalidate the function bar
	RECT rect;
	rect.right = 0;
	rect.top = m_lVerticalScroll;
	VARIANT var;
	var.vt = VT_I4;
	if (FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )))
	{
		return;
	}
	rect.bottom = m_lVerticalScroll + V_I4(&var);
	if( FAILED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var )) )
	{
		return;
	}
	rect.left = -V_I4(&var);

	// Need to offset it if we're horizontally scrolled
	long lLeftDisplay;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	rect.left += lLeftDisplay;
	rect.right += lLeftDisplay;

	// Really invalidate the function bar
	m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

void CPianoRollStrip::InvalidateVariationBar( void )
{
	// Invalidate the variation selection bar
	long lLeftDisplay;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );

	// If there is no pickup bar, the variation bar is at least at time 0
	if( !m_fPickupBar )
	{
		lLeftDisplay = max( 0, lLeftDisplay );
	}

	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	RECT rect;
	rect.left = lLeftDisplay;
	rect.right = rect.left + VARIATION_GUTTER_WIDTH + 16 * VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH;
	rect.top = m_lVerticalScroll;
	rect.bottom = rect.top + 2 * VARIATION_BUTTON_HEIGHT;
	// Don't need to erase, since the variation selection bar will overwrite the area
	m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

void CPianoRollStrip::SetDesignInfo( ioPianoRollDesign* pPianoRollDesign )
{
	ASSERT( pPianoRollDesign != NULL );
	if (pPianoRollDesign == NULL)
	{
		return;
	}
	BOOL fChanged = FALSE;
	if ( pPianoRollDesign->m_dwVariations != m_dwVariations )
	{
		fChanged = TRUE;
		m_dwVariations = pPianoRollDesign->m_dwVariations;

		if ((m_dwVariations & ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled) == ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
		{
			m_fGutterPressed = TRUE;
		}
		else
		{
			m_fGutterPressed = FALSE;
		}
	}
	if ( pPianoRollDesign->m_crUnselectedNoteColor != m_crUnselectedNoteColor )
	{
		fChanged = TRUE;
		m_crUnselectedNoteColor = pPianoRollDesign->m_crUnselectedNoteColor;
	}
	if ( pPianoRollDesign->m_crSelectedNoteColor != m_crSelectedNoteColor )
	{
		fChanged = TRUE;
		m_crSelectedNoteColor = pPianoRollDesign->m_crSelectedNoteColor;
	}
	if ( pPianoRollDesign->m_crOverlappingNoteColor != m_crOverlappingNoteColor )
	{
		fChanged = TRUE;
		m_crOverlappingNoteColor = pPianoRollDesign->m_crOverlappingNoteColor;
	}
	if ( pPianoRollDesign->m_crAccidentalColor != m_crAccidentalColor )
	{
		fChanged = TRUE;
		m_crAccidentalColor = pPianoRollDesign->m_crAccidentalColor;
	}
	if ( pPianoRollDesign->m_fHybridNotation != m_fHybridNotation )
	{
		fChanged = TRUE;
		m_fHybridNotation = pPianoRollDesign->m_fHybridNotation;
	}
	if ( pPianoRollDesign->m_dblVerticalZoom != m_dblVerticalZoom )
	{
		fChanged = TRUE;
		m_dblVerticalZoom = pPianoRollDesign->m_dblVerticalZoom;
		m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
	}
	if ( pPianoRollDesign->m_lVerticalScroll != m_lVerticalScroll )
	{
		fChanged = TRUE;
		SetNewVerticalScroll( pPianoRollDesign->m_lVerticalScroll );
	}
	VARIANT var;
	m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_HEIGHT, &var );
	ASSERT(	var.vt == VT_I4 );
	if ( pPianoRollDesign->m_lHeight != V_I4(&var) )
	{
		fChanged = TRUE;
		var.vt = VT_I4;
		V_I4(&var) = pPianoRollDesign->m_lHeight;
		m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_HEIGHT, var );

		// Resize the vertical scroll bar
		OnSize();
	}
	// Fix a problem with the strip causing an entry to the undo queue when the timeline
	// window is opened (and sends an WM_SIZE, restored, to each strip)
	m_lLastHeight = pPianoRollDesign->m_lHeight;

	m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIPVIEW, &var );
	ASSERT(	var.vt == VT_I4 );
	if ( pPianoRollDesign->m_nStripView != m_StripView ||
		 pPianoRollDesign->m_nStripView != V_I4(&var) )
	{
		fChanged = TRUE;
		m_StripView = (STRIPVIEW) pPianoRollDesign->m_nStripView;
		var.vt = VT_I4;
		V_I4(&var) = pPianoRollDesign->m_nStripView;
		m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIPVIEW, var );
	}
	if( pPianoRollDesign->m_dwFlags & STPST_ACTIVESTRIP )
	{
		BOOL fActivateStrip = FALSE;

		if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			IUnknown* pIUnknown = V_UNKNOWN(&var);
			if( pIUnknown )
			{
				IDMUSProdStrip* pIActiveStrip;
				if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
				{
					if( pIActiveStrip != (IDMUSProdStrip *)this )
					{
						fActivateStrip = TRUE;
					}
					pIActiveStrip->Release();
				}
				pIUnknown->Release();
			}
		}
		else
		{
			fActivateStrip = TRUE;
		}

		if( fActivateStrip )
		{
			IUnknown* punk;
			if( SUCCEEDED ( QueryInterface( IID_IUnknown, (void**)&punk ) ) )
			{
				fChanged = TRUE;
				var.vt = VT_UNKNOWN;
				V_UNKNOWN(&var) = punk;
				m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
			}
		}
	}

	// Show/Hide the scrollbar appropriately
	if( m_StripView == SV_NORMAL )
	{
		m_prScrollBar.ShowWindow(TRUE);

		// Insert VarSwitch strip
		// Determine proper position for the VarSwitchStrip
		VARIANT varPos;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_POSITION, &varPos) ) )
		{
			// Add VarSwitch Strip to the Timeline
			// This will return E_FAIL if the strip already is in the Timeline
			m_pMIDIMgr->m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)m_pVarSwitchStrip, V_I4(&varPos) + 1 );
		}
	}
	else // m_svView == SV_MINIMIZED
	{
		m_prScrollBar.ShowWindow(FALSE);

		// This will return E_FAIL if the strip is already removed.
		m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pVarSwitchStrip );
	}

	// Change number of 'extra' bars
	if( m_dwExtraBars != pPianoRollDesign->m_dwExtraBars )
	{
		m_dwExtraBars = pPianoRollDesign->m_dwExtraBars;

		// The Timeline is smart enough to redraw the entire display if the amount of
		// 'extra' time is changed.
		fChanged = TRUE;
	}

	// Show/hide pickup bar
	if( !(pPianoRollDesign->m_dwFlags & STPST_PICKUPBAR) != !m_fPickupBar )
	{
		if( pPianoRollDesign->m_dwFlags & STPST_PICKUPBAR )
		{
			m_fPickupBar = TRUE;
		}
		else
		{
			m_fPickupBar = FALSE;
		}

		// The Timeline is smart enough to redraw the entire display if the amount of
		// 'extra' time is changed.
		fChanged = TRUE;
	}

	if ( fChanged )
	{
		//BUGBUG: should be smarter about what we invalidate here..
		m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, FALSE );
	}
}

void CPianoRollStrip::GetDesignInfo( ioPianoRollDesign* pPianoRollDesign )
{
	ASSERT( pPianoRollDesign != NULL );
	if (pPianoRollDesign == NULL)
	{
		return;
	}
	
	pPianoRollDesign->m_dwVariations = m_dwVariations;
	pPianoRollDesign->m_crUnselectedNoteColor = m_crUnselectedNoteColor;
	pPianoRollDesign->m_crSelectedNoteColor = m_crSelectedNoteColor;	
	pPianoRollDesign->m_crOverlappingNoteColor = m_crOverlappingNoteColor;	
	pPianoRollDesign->m_crAccidentalColor = m_crAccidentalColor;	
	pPianoRollDesign->m_dblVerticalZoom = m_dblVerticalZoom;
	pPianoRollDesign->m_lVerticalScroll = m_lVerticalScroll;
	VARIANT var;
	m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_HEIGHT, &var );
	ASSERT(	var.vt == VT_I4 );
	pPianoRollDesign->m_lHeight = V_I4(&var);
	m_lLastHeight = pPianoRollDesign->m_lHeight;
	pPianoRollDesign->m_nStripView = m_StripView;
	pPianoRollDesign->m_fHybridNotation = m_fHybridNotation;
	pPianoRollDesign->m_dwExtraBars = m_dwExtraBars;

	// Initialize state flags
	pPianoRollDesign->m_dwFlags = 0;
		
	// Active strip?
	if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
	{
		IUnknown* pIUnknown = V_UNKNOWN(&var);
		if( pIUnknown )
		{
			IDMUSProdStrip* pIActiveStrip;
			if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
			{
				if( pIActiveStrip == (IDMUSProdStrip *)this )
				{
					pPianoRollDesign->m_dwFlags |= STPST_ACTIVESTRIP;
				}
				pIActiveStrip->Release();
			}
			pIUnknown->Release();
		}
	}

	// Pickup bar?
	if( m_fPickupBar )
	{
		pPianoRollDesign->m_dwFlags |= STPST_PICKUPBAR;
	}
}

void CPianoRollStrip::ClipRectToWindow( const RECT* pSourceRect, RECT* pDestRect )
{
	ASSERT( pSourceRect != NULL );
	if ( !pSourceRect )
	{
		return;
	}

	ASSERT( pDestRect != NULL );
	if ( !pDestRect )
	{
		return;
	}

	VARIANT var;
	long lHeight = 0;
	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_HEIGHT, &var)))
	{
		lHeight = V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2;
	}

	long lLeftDisplay, lEndOfTime = 0;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	
	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var)))
	{
		lEndOfTime = V_I4(&var);
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( lEndOfTime, &lEndOfTime );
	}

	if (lEndOfTime >= lLeftDisplay)
	{
		pDestRect->left = __max(pSourceRect->left, lLeftDisplay);
		pDestRect->right = __min(pSourceRect->right, lEndOfTime);
		pDestRect->top = __max(pSourceRect->top, m_lVerticalScroll);
		if (pSourceRect->bottom >= m_lVerticalScroll + lHeight)
		{
			pDestRect->bottom = __min(pSourceRect->bottom, m_lVerticalScroll + lHeight);
		}
		else
		{
			pDestRect->bottom = __min(pSourceRect->bottom, m_lVerticalScroll + lHeight);
		}
	}
	else
	{
		*pDestRect = CRect(0, 0, 0, 0);
	}
	//	pSourceRect->left, pSourceRect->right, pSourceRect->top, pSourceRect->bottom,
	//	pDestRect->left, pDestRect->right, pDestRect->top, pDestRect->bottom);
	pDestRect->left -= lLeftDisplay;
	pDestRect->right -= lLeftDisplay;
	pDestRect->top -= m_lVerticalScroll;
	pDestRect->bottom -= m_lVerticalScroll;
}

/*
Step Recording:

  Advance to the next Grid position when:
  * A NoteOff is received and no notes are still on.
  * The space bar is pressed.

  Enter a note when:
  * A NoteOff is received for that note.

*/

HRESULT CPianoRollStrip::OnMidiMsg(REFERENCE_TIME rtTime, 
									   BYTE bStatus, 
									   BYTE bData1, 
									   BYTE bData2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	if (m_pMIDIMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	if (m_pMIDIMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// If shutting down, ignore all MIDI messages.
	if( m_pMIDIMgr->m_fShuttingDown )
	{
		return E_UNEXPECTED;
	}

	ASSERT( ValidPartRefPtr() );
	if ( !ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	// If not recording, just pass through the MIDI input
	if( !m_pMIDIMgr->m_fRecordEnabled || (m_dwVariations == 0) )
	{
		// Clear m_bVelocity and m_mtStartTime for this note
		if((int)(bStatus & 0xF0) == (int)MIDI_NOTEOFF)
		{
			m_bVelocity[bData1] = 0;
			m_mtStartTime[bData1] = 0;
		}

		// If we're not already thruing, output a message to the performance engine
		if ( !m_fMIDIThruEnabled )
		{
			DMUS_MIDI_PMSG *pDMMIDIEvent = NULL;
			if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
			{
				ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
				// PMSG fields
				pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			//	pDMNoteEvent->rtTime = 0;
			//	pDMNoteEvent->mtTime = 0;
				pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
				//m_pMIDIMgr->m_pIDMPerformance->GetTime( &pDMMIDIEvent->rtTime, &pDMMIDIEvent->mtTime );
				//pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_MUSICTIME;
				pDMMIDIEvent->dwPChannel = m_pPartRef->m_dwPChannel;
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

				m_pMIDIMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
				// The playback engine will release the event
			}
		}
		return S_OK;
	}

	// Note On
	if((int)(bStatus & 0xF0) == (int)MIDI_NOTEON)
	{
		//TRACE("Inside  CPianoRollStrip::OnMidiMsg going to play %d note on with %d at %d\n", bData1, bData2, ::timeGetTime());
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
	else if((int)(bStatus & 0xF0) == (int)MIDI_NOTEOFF)
	{
		//TRACE("Inside  CPianoRollStrip::OnMidiMsg going to play %d note off at %d\n", bData1, ::timeGetTime());
		if (!IsEnginePlaying() && (m_mtStartTimeOffset[bData1] == 0) )
		{
			return RecordStepNoteOff( bData1 );
		}
		else
		{
			return RecordRealTimeNoteOff( rtTime, bData1 );
		}
	}

	// Curves
	else if( (int)(bStatus & 0xF0) == (int)MIDI_PBEND 
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

HRESULT CPianoRollStrip::RecordStepNoteOn(BYTE bData1, BYTE bData2 )
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
	m_mtStartTimeOffset[bData1] = 0;

	// Can't call m_pMIDIMgr->PlayNote, because that will clip off any notes that are already playing

	// Play the entire note
	if( m_pMIDIMgr->m_pIDMPerformance )
	{
		REFERENCE_TIME rtLatency;
		m_pMIDIMgr->m_pIDMPerformance->GetLatencyTime( &rtLatency );

		DMUS_NOTE_PMSG *pDMNoteEvent = NULL;
		if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_NOTE_PMSG), (DMUS_PMSG **)&pDMNoteEvent ) ) )
		{
			ASSERT( pDMNoteEvent != NULL );
			ZeroMemory( pDMNoteEvent, sizeof(DMUS_NOTE_PMSG) );
			// PMSG fields
			pDMNoteEvent->dwSize = sizeof(DMUS_NOTE_PMSG);
			pDMNoteEvent->rtTime = rtLatency;
		//	pDMNoteEvent->mtTime = 0;
			pDMNoteEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMNoteEvent->dwPChannel = m_pPartRef->m_dwPChannel;
			pDMNoteEvent->dwVirtualTrackID = 1;
		//	pDMNoteEvent->pTool = NULL;
		//	pDMNoteEvent->pGraph = NULL;
			pDMNoteEvent->dwType = DMUS_PMSGT_NOTE;
		//	pDMNoteEvent->punkUser = 0;
			// DMNoteEvent fields
			pDMNoteEvent->mtDuration = max( GetSnapAmount() - 1, m_lGridClocks / 2 );
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
		//	pDMNoteEvent->bNoteFlags = pPropNote->m_bNoteFlags;
			pDMNoteEvent->bMidiValue = bData1;

			DMUS_PMSG *pPMsg = (DMUS_PMSG *)pDMNoteEvent;
			m_pMIDIMgr->SendPMsg( pPMsg );
			// The playback engine will release the event
		}
	}

	// Move the insert cursor to a new position
	m_lInsertVal = bData1;

	::PostMessage( m_prScrollBar.m_hWnd, WM_APP, WM_APP_INVALIDATEPIANOROLL, 0 );

	return S_OK;
}

HRESULT CPianoRollStrip::RecordStepNoteOff( BYTE bData1 )
{
	// if the velocity is 0, we didn't receive a NOTEON message, so exit
	if (m_bVelocity[bData1] == 0)
	{
		return E_UNEXPECTED;
	}

	long mtStartTime = m_mtStartTime[bData1];

	BYTE bVelocity = m_bVelocity[bData1];

	long lSnapAmount = GetSnapAmount();
	MUSIC_TIME mtEndTime = m_lInsertTime + lSnapAmount;

	// Get Part and pattern length
	MUSIC_TIME mtPartLength, mtPatternLength;
	mtPartLength = m_pPartRef->m_pDMPart->GetClockLength();
	mtPatternLength = m_pPartRef->m_pPattern->CalcLength();

	while ( mtEndTime < mtStartTime)
	{
		mtEndTime += mtPatternLength;
	}

	CDirectMusicStyleNote* pDMNote;
	pDMNote = new CDirectMusicStyleNote;
	
	if( mtStartTime < 0 )
	{
		pDMNote->m_mtGridStart = 0;
		pDMNote->m_nTimeOffset = short (max( mtStartTime, -m_lMeasureClocks ));
	}
	else
	{
		mtStartTime %= mtPatternLength;
		mtStartTime %= mtPartLength;
		pDMNote->m_mtGridStart = CLOCKS_TO_GRID( mtStartTime, m_pPartRef->m_pDMPart );
		pDMNote->m_nTimeOffset = short(mtStartTime - GRID_TO_CLOCKS( pDMNote->m_mtGridStart, m_pPartRef->m_pDMPart ));
	}
	pDMNote->m_dwVariation = m_dwVariations;
	pDMNote->m_bVelocity = bVelocity;
	pDMNote->m_bTimeRange = 0;
	pDMNote->m_bDurRange = 0;
	pDMNote->m_bVelRange = 0;
	pDMNote->m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
	pDMNote->m_bInversionId = 0;
	pDMNote->m_bNoteFlags = 0;
	pDMNote->m_bMIDIValue = bData1;
	pDMNote->m_wMusicValue = m_pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
	pDMNote->m_fSelected = FALSE;
	pDMNote->m_mtDuration = max( mtEndTime - mtStartTime - 1, m_lGridClocks / 2 );

	ASSERT( pDMNote->m_mtDuration != 0 );

	m_pMIDIMgr->InsertNote( m_pPartRef, pDMNote, FALSE );

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
		CoInitialize( NULL );
		m_pMIDIMgr->UpdateOnDataChanged( IDS_EDIT_INSERT );
		CoUninitialize();
		// Handled by UpdateOnDataChanged
		//m_fNoteInserted = FALSE;

		// Bump to the right (this eventually causes BumpTimeCursor( TRUE ) to be
		// called in a message handler thread
		::PostMessage( m_prScrollBar.m_hWnd, WM_APP, WM_APP_BUMPTIMECURSOR, 0 );
	}
	return S_OK;
}

HRESULT CPianoRollStrip::RecordRealTimeNoteOn(REFERENCE_TIME rtTime, BYTE bData1, BYTE bData2 )
{
	if (m_pIConductor != NULL)
	{
		if (m_bVelocity[bData1] != 0)
		{
			return E_UNEXPECTED;
		}

			if( m_pMIDIMgr->m_pSegmentState )
			{
				MUSIC_TIME mtSegStartTime;
				if( SUCCEEDED( m_pMIDIMgr->m_pSegmentState->GetStartTime( &mtSegStartTime ) ) )
				{
					MUSIC_TIME mtSegStartPoint;
					if( SUCCEEDED( m_pMIDIMgr->m_pSegmentState->GetStartPoint( &mtSegStartPoint ) ) )
					{
						MUSIC_TIME mtTime;
						m_pMIDIMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );

						//TRACE("Start time: %d\n",mtTime);
						m_mtStartTime[bData1] = mtTime;

						// Convert mtTime from absolute time to an offset from when the segment started playing
						mtTime = GetTimeOffset( mtTime, mtSegStartTime, mtSegStartPoint,
									m_pMIDIMgr->m_mtCurrentLoopStart, m_pMIDIMgr->m_mtCurrentLoopEnd, m_pMIDIMgr->m_mtCurrentLength,
									m_pMIDIMgr->m_dwCurrentMaxLoopRepeats );

						m_mtStartTimeOffset[bData1] = mtTime;

						if (bData2 > 0)
						{
							m_bVelocity[bData1] = bData2;
						}
						else
						{
							m_bVelocity[bData1] = 127;
						}
					}
				}
			}
			else
			{
				return E_FAIL;
			}
	}
	return S_OK;
}

DWORD ComputeOriginalVariation( DWORD dwCurrentVariation, MUSIC_TIME mtDuration, MUSIC_TIME mtNoteStartOffset, MUSIC_TIME mtPatternLength )
{
	// Search for the one variation that is active
	int nCurrentVariation = -1;
	for( int nVariation = 0; nVariation < 32; nVariation++ )
	{
		if( dwCurrentVariation & (1 << nVariation) )
		{
			// Exit if more than one variation is active
			if( nCurrentVariation != -1 )
			{
				return dwCurrentVariation;
			}
			nCurrentVariation = nVariation;
		}
	}

	// Exit if no variations are active
	if( nCurrentVariation == -1 )
	{
		return dwCurrentVariation;
	}

	// Compute where the note end occurs, relative to the start of the segment.
	MUSIC_TIME mtNoteEndOffset = mtDuration + mtNoteStartOffset;
	// Compute how far into the pattern the note end occurs
	MUSIC_TIME mtOffsetIntoPattern = mtNoteEndOffset % mtPatternLength;

	// Check if we started and ended in the same pattern
	if( mtDuration <= mtOffsetIntoPattern )
	{
		return dwCurrentVariation;
	}

	// Compute when the pattern started, that the note start is in
	MUSIC_TIME mtPatternStart = mtNoteStartOffset - (mtNoteStartOffset % mtPatternLength);

	// Update the note start and note end offset to be from the start of the pattern the note start is in
	mtNoteEndOffset -= mtPatternStart;

	// Compute the relative variation the note end is in
	int nEndVariation = mtNoteEndOffset / mtPatternLength;

	// Compute the real variation the note start is in
	nCurrentVariation -= nEndVariation;

	// Now, make sure it is a positive number
	while( nCurrentVariation < 0 )
	{
		nCurrentVariation += 32;
	}

	// Return the variation the note start was in
	return 1 << nCurrentVariation;
}

HRESULT CPianoRollStrip::RecordRealTimeNoteOff(REFERENCE_TIME rtTime, BYTE bData1 )
{
	if (m_pIConductor != NULL)
	{
		// if the velocity is 0, we didn't receive a NOTEON message, so exit
		if (m_bVelocity[bData1] == 0)
		{
			//TRACE("Oops: velocity 0\n");
			return E_UNEXPECTED;
		}

		{
			MUSIC_TIME mtTime, mtPartLength, mtPatternLength;
			m_pMIDIMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );
			mtPartLength = m_pPartRef->m_pDMPart->GetClockLength();
			mtPatternLength = m_pPartRef->m_pPattern->CalcLength();

			CDirectMusicStyleNote* pDMNote;
			pDMNote = new CDirectMusicStyleNote;

			pDMNote->m_mtDuration = min( mtPartLength + m_lMeasureClocks, mtTime - m_mtStartTime[bData1] );

			DWORD dwOrigVariation = m_dwVariations;
			if( m_pPartRef->m_bRandomVariation == DMUS_VARIATIONT_SEQUENTIAL )
			{
				dwOrigVariation = ComputeOriginalVariation( m_dwVariations, pDMNote->m_mtDuration, m_mtStartTimeOffset[bData1], mtPatternLength );
			}

			MUSIC_TIME mtStartTime = m_mtStartTimeOffset[bData1];
			if( mtStartTime > 0 )
			{
				// Note started after the pattern began playing
				mtStartTime %= mtPatternLength;
			}
			else
			{
				// Note started before the pattern began playing
			}

			if( mtStartTime < 0 )
			{
				pDMNote->m_mtGridStart = 0;
				pDMNote->m_dwVariation = m_dwVariations;
				pDMNote->m_nTimeOffset = short (max( mtStartTime, -m_lMeasureClocks ));
			}
			else if( mtStartTime - mtPatternLength + pDMNote->m_mtDuration / 2  > 0 )
			{
				// At least half the note is in the current pattern repeat - consider it a 'pick-up' note
				pDMNote->m_mtGridStart = 0;
				pDMNote->m_dwVariation = m_dwVariations;
				pDMNote->m_nTimeOffset = short (mtStartTime - mtPatternLength);
			}
			else
			{
				// Now, clip it to the part length
				mtStartTime %= mtPartLength;

				// If the note extends over a beat boundary, and most of the note is on the second beat
				if( ((mtStartTime % m_lBeatClocks) > (m_lBeatClocks / 2)) &&
					((pDMNote->m_mtDuration / 2) > ( m_lBeatClocks - (mtStartTime % m_lBeatClocks ) )) )
				{
					// Set the grid start to the next beat, and the offset as a time from then
					pDMNote->m_mtGridStart = ((mtStartTime / m_lBeatClocks) + 1) * m_pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat;
					pDMNote->m_nTimeOffset = short ((mtStartTime % m_lBeatClocks ) - m_lBeatClocks);

					// If the grid start is beyond the end of the part, subtract off one part length.
					if( pDMNote->m_mtGridStart >= m_pPartRef->m_pDMPart->GetGridLength() )
					{
						pDMNote->m_mtGridStart -= m_pPartRef->m_pDMPart->GetGridLength();
					}
				}
				else
				{
					// Otherwise, just convert to the nearest Grid plus an offset
					pDMNote->m_mtGridStart = CLOCKS_TO_GRID( mtStartTime, m_pPartRef->m_pDMPart );
					pDMNote->m_nTimeOffset = short(mtStartTime - GRID_TO_CLOCKS( pDMNote->m_mtGridStart, m_pPartRef->m_pDMPart ));
				}
				pDMNote->m_dwVariation = dwOrigVariation;
			}

			pDMNote->m_bVelocity = m_bVelocity[bData1];
			pDMNote->m_bTimeRange = 0;
			pDMNote->m_bDurRange = 0;
			pDMNote->m_bVelRange = 0;
			pDMNote->m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
			pDMNote->m_bInversionId = 0;
			pDMNote->m_bNoteFlags = 0;
			pDMNote->m_bMIDIValue = bData1;
			pDMNote->m_wMusicValue = m_pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
			pDMNote->m_fSelected = FALSE;

			if (pDMNote->m_mtDuration == 0)
			{
				//TRACE("Duration == 0 at %d\n", ::timeGetTime());
				pDMNote->m_mtDuration = m_lGridClocks / 4;
			}
			m_pMIDIMgr->InsertNote( m_pPartRef, pDMNote, FALSE );

			m_bVelocity[bData1] = 0;
			m_mtStartTime[bData1] = 0;
			m_mtStartTimeOffset[bData1] = 0;
		}
	}
	return S_OK;
}

HRESULT CPianoRollStrip::RecordRealTimeCurve( REFERENCE_TIME rtTime, BYTE bStatus, BYTE bData1, BYTE bData2 )
{
	// Make sure Conductor exists
	if( m_pIConductor )
	{
		BOOL fContinue = FALSE;

		// Get Part and pattern length
		MUSIC_TIME mtPartLength, mtPatternLength;
		mtPartLength = m_pPartRef->m_pDMPart->GetClockLength();
		mtPatternLength = m_pPartRef->m_pPattern->CalcLength();

		// Get the MUSIC_TIME
		MUSIC_TIME mtTime;
		{
			m_pMIDIMgr->m_pIDMPerformance->ReferenceToMusicTime( rtTime, &mtTime );

			if( m_pMIDIMgr->m_pSegmentState )
			{
				MUSIC_TIME mtSegStartTime;
				if( SUCCEEDED ( m_pMIDIMgr->m_pSegmentState->GetStartTime( &mtSegStartTime ) ) )
				{
					MUSIC_TIME mtSegStartPoint;
					if( SUCCEEDED ( m_pMIDIMgr->m_pSegmentState->GetStartPoint( &mtSegStartPoint ) ) )
					{
						MUSIC_TIME  mtStartTime = mtSegStartTime - mtSegStartPoint;
						mtTime = ((mtTime - mtStartTime) % mtPatternLength) % mtPartLength;
						fContinue = TRUE;
					}
				}
			}
		}

		if( fContinue == FALSE )
		{
			return E_FAIL;
		}

		// Create the Curve
		CDirectMusicStyleCurve* pDMCurve = new CDirectMusicStyleCurve;
		if( pDMCurve )
		{
			switch( (int)(bStatus & 0xF0) )
			{
				case MIDI_PBEND:
					pDMCurve->m_bEventType = DMUS_CURVET_PBCURVE;
					pDMCurve->m_nStartValue = short(((bData2 & 0x7F) << 7) + (bData1 & 0x7F));
					break;

				case MIDI_CCHANGE:
					pDMCurve->m_bEventType = DMUS_CURVET_CCCURVE;
					pDMCurve->m_bCCData = bData1;
					pDMCurve->m_nStartValue = bData2;
					break;

				case MIDI_PTOUCH:
					pDMCurve->m_bEventType = DMUS_CURVET_PATCURVE;
					pDMCurve->m_bCCData = bData1;
					pDMCurve->m_nStartValue = bData2;
					break;

				case MIDI_MTOUCH:
					pDMCurve->m_bEventType = DMUS_CURVET_MATCURVE;
					pDMCurve->m_nStartValue = bData1;
					break;
			}

			// Need to ensure that m_mtGridStart is always positive
			pDMCurve->m_mtGridStart = short (max( 0, mtTime / m_lGridClocks ));
			pDMCurve->m_nTimeOffset = short ( mtTime - pDMCurve->m_mtGridStart * m_lGridClocks );

			pDMCurve->m_dwVariation = m_dwVariations;
			pDMCurve->m_mtDuration = 1;
			pDMCurve->m_bCurveShape = DMUS_CURVES_INSTANT;
			pDMCurve->m_nEndValue = pDMCurve->m_nStartValue;

			pDMCurve->SetDefaultResetValues( min(mtPartLength, mtPatternLength) );

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

			// Update the Part's GUID (and create a new Part if we need to)
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );

			// Place curve in Part's event list
			m_pPartRef->m_pDMPart->InsertCurveInAscendingOrder( pDMCurve );
			m_pMIDIMgr->m_fNoteInserted = TRUE;
			m_pMIDIMgr->m_fDirty = TRUE;

			// Update all views for this Part
			m_fRefreshCurveProperties = FALSE;
			m_fRefreshVarSwitchProperties = FALSE;
			m_pMIDIMgr->RefreshPartDisplay( m_pPartRef->m_pDMPart, pDMCurve->m_dwVariation, TRUE, FALSE );
			m_fRefreshCurveProperties = TRUE;
			m_fRefreshVarSwitchProperties = TRUE;
		}
	}
	return S_OK;
}

void CPianoRollStrip::RegisterMidi() 
{
	if (!m_fMIDIInEnabled && (m_pIConductor != NULL))
	{
		REGISTER_MIDI_IN(m_pIConductor, m_dwCookie);
		m_fMIDIInEnabled = TRUE;
		// Release the Reference gained in the REGISTER_MIDI_IN call.
		// If we don't do this, we will never be destroyed (because our RefCount
		// will not go down to zero.)
		Release();
	}
}

void CPianoRollStrip::UnRegisterMidi()
{
	if (m_fMIDIInEnabled && (m_pIConductor != NULL) && (m_pMIDIMgr != NULL))
	{
		// Send fake MIDI_NOTEOFF messages for all currently playing notes
		// But only if we're not shutting down
		if( !m_pMIDIMgr->m_fShuttingDown )
		{
			REFERENCE_TIME rtTime;
			if( SUCCEEDED(m_pMIDIMgr->m_pIDMPerformance->GetTime( &rtTime, NULL )) )
			{
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

		// Add a reference, since UNREGISTER_MIDI_IN will cause one to be taken away.
		if ( m_cRef > 0 )
		{
			AddRef();
			UNREGISTER_MIDI_IN(m_pIConductor, m_dwCookie);
		}
		else
		{
			m_cRef = 2;
			UNREGISTER_MIDI_IN(m_pIConductor, m_dwCookie);
			m_cRef = 0;
		}
		m_fMIDIInEnabled = FALSE;
	}
}

BOOL CPianoRollStrip::IsEnginePlaying( void )
{
	return (m_pMIDIMgr->m_pSegmentState != NULL) ? TRUE : FALSE;
}

void CPianoRollStrip::OnVScrollFromScrollbar(UINT nSBCode, UINT nPos)
{
	UINT nNewPos;
	UINT nTestPos;
	nTestPos = m_lVerticalScroll / (m_lMaxNoteHeight);

	VARIANT varHeight;
	VERIFY( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &varHeight )) );
	int nHeight = max( 0, V_I4(&varHeight) + BORDER_VERT_WIDTH - 2 * VARIATION_BUTTON_HEIGHT);

	switch( nSBCode )
	{
	case SB_TOP:
		nNewPos = 0;
		break;
	case SB_LINEDOWN:
		nNewPos = nTestPos + 1;
		break;
	case SB_LINEUP:
		nNewPos = nTestPos - 1;
		break;
	case SB_PAGEDOWN:
		nNewPos = nTestPos + (nHeight / m_lMaxNoteHeight);
		break;
	case SB_PAGEUP:
		nNewPos = nTestPos - (nHeight / m_lMaxNoteHeight);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewPos = nPos;
		break;
	case SB_BOTTOM:
		nNewPos = ( m_fHybridNotation ? 38 : 127 )- (nHeight / m_lMaxNoteHeight);
		break;
	case SB_ENDSCROLL:
	default:
		return;
	}

	long lNewVertScroll;
	lNewVertScroll = nNewPos * (m_lMaxNoteHeight);
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
	if (lNewVertScroll != m_lVerticalScroll)
	{
		SetNewVerticalScroll( lNewVertScroll );
	}
}

HRESULT CPianoRollStrip::OnVScroll( void )
{
	// reposition vertical scrollbar
	return ComputeVScrollBar();
}

HRESULT CPianoRollStrip::OnSize( void )
{
	long lMaxVertScroll;
	if( m_fHybridNotation )
	{
		lMaxVertScroll = 38 * m_lMaxNoteHeight;
	}
	else
	{
		lMaxVertScroll = 128 * (m_lMaxNoteHeight);
	}

	VARIANT var;
	if( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var )) )
	{
		lMaxVertScroll -= V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2;
	}

	m_pMIDIMgr->m_fDirty = TRUE;

	if ( m_lVerticalScroll > lMaxVertScroll )
	{
		SetNewVerticalScroll( lMaxVertScroll );
	}

	// reposition and resize vertical scrollbar
	return ComputeVScrollBar();
}

HRESULT	CPianoRollStrip::ComputeVScrollBar( void )
{
	// reposition and resize vertical scrollbar
	if( !m_pMIDIMgr->m_pTimeline )
	{
		return E_UNEXPECTED;
	}

	if( m_StripView == SV_MINIMIZED )
	{
		return S_FALSE;
	}

	RECT rectVisible, rectAll;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectVisible;

	if( FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var )))
	{
		return E_FAIL;
	}

	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectAll;

	if( FAILED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_ENTIRE_STRIP_RECT, &var )))
	{
		return E_FAIL;
	}

	RECT oldRect = m_rectScrollBar;

	// move the vertical scroll bar to where it needs to go
	m_prScrollBar.GetClientRect(&m_rectScrollBar);
	m_rectScrollBar.left = rectVisible.right - m_rectScrollBar.right - BORDER_HORIZ_WIDTH;
	m_rectScrollBar.right = rectVisible.right - BORDER_HORIZ_WIDTH;
	m_rectScrollBar.bottom = rectVisible.bottom;
	m_rectScrollBar.top = min( rectVisible.bottom, max( rectVisible.top, rectAll.top + VARIATION_BUTTON_HEIGHT * 2 ) );

	// Always update
	//if (m_rectScrollBar != oldRect)
	{
		// move the scroll bar
		if (m_rectScrollBar != oldRect)
		{
			m_prScrollBar.MoveWindow( &m_rectScrollBar, TRUE );
		}

		// Get the strip's height
		VARIANT varHeight;
		VERIFY( SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &varHeight )) );

		int nHeight = max( 0, V_I4(&varHeight) + BORDER_VERT_WIDTH - 2 * VARIATION_BUTTON_HEIGHT);
		if ( (m_fHybridNotation && (nHeight / m_lMaxNoteHeight >= 38)) ||
			 (!m_fHybridNotation && (nHeight / m_lMaxNoteHeight >= 127)) )
		{
			// If we can view all 128 notes, disable the scrollbar
			m_prScrollBar.EnableScrollBar(ESB_DISABLE_BOTH);
		}
		else
		{
			// Compute the new page size
			SCROLLINFO si;
			si.cbSize = sizeof( SCROLLINFO );

			// enable the scrollbar
			m_prScrollBar.EnableScrollBar(ESB_ENABLE_BOTH);

			// set the new page size
			si.fMask = SIF_PAGE;
			si.nPage = nHeight / m_lMaxNoteHeight;
			m_prScrollBar.SetScrollInfo( &si, TRUE );
		}
	}

	return S_OK;
}

BOOL CPianoRollStrip::InitializeNotePropData( NotePropData *pNPD )
{
	ASSERT( pNPD != NULL );
	if( pNPD == NULL )
	{
		return FALSE;
	}

	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return FALSE;
	}

	pNPD->m_Param.pPart = m_pPartRef->m_pDMPart;
	pNPD->m_Param.dwVariations = m_dwVariations;
	pNPD->m_PropNote.SetPart( m_pPartRef->m_pDMPart );
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::InitializePianoRollData

void CPianoRollStrip::InitializePianoRollData( PianoRollData* pPRD ) const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );

	//memcpy( &pPRD->guid, &m_guid, sizeof(GUID) );
	m_pMIDIMgr->SetVarLocksArray( pPRD );

	pPRD->dwPageIndex			= 2;
	pPRD->dwVariations			= m_dwVariations;
	pPRD->crUnselectedNoteColor	= m_crUnselectedNoteColor;
	pPRD->crSelectedNoteColor	= m_crSelectedNoteColor;
	pPRD->crOverlappingNoteColor= m_crOverlappingNoteColor;	
	pPRD->crAccidentalColor		= m_crAccidentalColor;	
	pPRD->dblVerticalZoom		= m_dblVerticalZoom;
	pPRD->lVerticalScroll		= m_lVerticalScroll;
	pPRD->StripView				= m_StripView;
	pPRD->dwExtraBars			= m_dwExtraBars;
	pPRD->fPickupBar			= m_fPickupBar;
	
	pPRD->ts.m_bBeat			= unsigned char(DM_PPQNx4 / m_lBeatClocks);
	pPRD->ts.m_bBeatsPerMeasure	= unsigned char(m_lMeasureClocks / m_lBeatClocks);
	pPRD->ts.m_wGridsPerBeat	= unsigned char(m_lBeatClocks / m_lGridClocks);

	// Part fields
	pPRD->bPlayModeFlags	= DMUS_PLAYMODE_PURPLEIZED;
	pPRD->wNbrMeasures		= 1;
	pPRD->bInvertUpper		= 127;
	pPRD->bInvertLower		= 0;
	pPRD->bAutoInvert		= 0;

	// PartRef fields
	pPRD->dwPChannel		= 0;
	pPRD->cstrPartRefName.Empty();
	pPRD->cstrPChannelName.Empty();
	pPRD->bSubChordLevel	= SUBCHORD_BASS;
	pPRD->bVariationLockID	= 0;
	pPRD->bRandomVariation	= 1;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::CurveStripExists
	
BOOL CPianoRollStrip::CurveStripExists( void *pVoid, BYTE bCCType, WORD wRPNType )
{
	ioCurveStripState* pCurveStripState;

	ASSERT( pVoid );
	if( pVoid == NULL )
	{
		return FALSE;
	}

	// Always return FALSE for generic RPN and NRPN strips
	if( (wRPNType == 0xFFFF)
	&&	((bCCType == CCTYPE_RPN_CURVE_STRIP) || (bCCType == CCTYPE_NRPN_CURVE_STRIP)) )
	{
		return FALSE;
	}

	CPianoRollStrip *pThis = static_cast<CPianoRollStrip *>(pVoid);

	// Use CurveStripState list instead of CurveStrip list
	// because it contains accurate info whether or not
	// CurveStrip(s) are minimized
	POSITION pos = pThis->m_lstCurveStripStates.GetHeadPosition();
	while( pos )
	{
		pCurveStripState = pThis->m_lstCurveStripStates.GetNext( pos );

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
// CPianoRollStrip::CurveTypeToStripCCType

BYTE CPianoRollStrip::CurveTypeToStripCCType( CDirectMusicStyleCurve* pDMCurve )
{
	return ::CurveTypeToStripCCType( pDMCurve->m_bEventType, pDMCurve->m_bCCData );
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SyncCurveStripStateList

void CPianoRollStrip::SyncCurveStripStateList( void )
{
	ASSERT( m_pPartRef != NULL );
	ASSERT( m_pPartRef->m_pDMPart != NULL );

	BYTE bCCType;

	CDirectMusicEventItem* pDMEvent = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	while( pDMEvent )
	{
		bCCType = CurveTypeToStripCCType( (CDirectMusicStyleCurve *)pDMEvent );
		GetCurveStripState( bCCType, ((CDirectMusicStyleCurve *)pDMEvent)->m_wParamType);

		pDMEvent = pDMEvent->GetNext();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetCurveStripState

ioCurveStripState* CPianoRollStrip::GetCurveStripState( BYTE bCCType, WORD wRPNType )
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
// CPianoRollStrip::GetCurveStrip
	
CCurveStrip* CPianoRollStrip::GetCurveStrip( BYTE bCCType, WORD wRPNType )
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
// CPianoRollStrip::AddCurveStrip
	
HRESULT CPianoRollStrip::AddCurveStrip( BYTE bCCType, WORD wRPNType )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Make sure Curve Strip does not already exist
	CCurveStrip* pCurveStrip = GetCurveStrip( bCCType, wRPNType );
	if( pCurveStrip )
	{
		// Curve Strip already exists
		return S_OK;
	}
	
	// Create the Curve strip
	pCurveStrip = new CCurveStrip( m_pMIDIMgr, this, bCCType, wRPNType );
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
		m_pMIDIMgr->m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pCurveStrip, dwPosition );

		VARIANT var;
		
		// Set Curve Strip StripView
		if( m_CurveStripView == SV_MINIMIZED )
		{
			ASSERT( bCCType == CCTYPE_MINIMIZED_CURVE_STRIP );

			var.vt = VT_I4;
			V_I4(&var) = m_CurveStripView;
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_STRIPVIEW, var );
		}

		if( bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
		{
			// Set Curve Strip Height
			var.vt = VT_I4;
			V_I4(&var) = pTheCurveStripState->m_nStripHeight;
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_HEIGHT, var );
		}

		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::AddCurveStrips
	
HRESULT CPianoRollStrip::AddCurveStrips( BOOL fUseActivateFlag )
{
	HRESULT hr = S_OK;

	// Only add curve strips when PianoRoll is not minimized
	if( m_StripView == SV_NORMAL )
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

				if( fUseActivateFlag )
				{
					if( pCurveStripState->m_dwFlags & STPST_ACTIVESTRIP )
					{
						VARIANT var;
						BOOL fActivateStrip = FALSE;

						if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
						{
							IUnknown* pIUnknown = V_UNKNOWN(&var);
							if( pIUnknown )
							{
								IDMUSProdStrip* pIActiveStrip;
								if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
								{
									if( pIActiveStrip != (IDMUSProdStrip *)this )
									{
										fActivateStrip = TRUE;
									}
									pIActiveStrip->Release();
								}
								pIUnknown->Release();
							}
						}
						else
						{
							fActivateStrip = TRUE;
						}

						if( fActivateStrip )
						{
							CCurveStrip* pCurveStrip = GetCurveStrip( pCurveStripState->m_bCCType, pCurveStripState->m_wRPNType );
							if( pCurveStrip )
							{
								IUnknown* punk;
								if( SUCCEEDED ( pCurveStrip->QueryInterface( IID_IUnknown, (void**)&punk ) ) )
								{
									var.vt = VT_UNKNOWN;
									V_UNKNOWN(&var) = punk;
									m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
									m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, FALSE );
								}
							}
						}
					}
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::RemoveCurveStrip
	
HRESULT CPianoRollStrip::RemoveCurveStrip( CCurveStrip* pCurveStrip )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Remove from list of Curve Strips
	POSITION pos = m_lstCurveStrips.Find( pCurveStrip );
	if( pos )
	{
		m_lstCurveStrips.RemoveAt( pos );

		// If not doing gutter selection, unselect all curves
		if( !m_fGutterSelected )
		{
			pCurveStrip->SelectAllCurves( FALSE );
		}

		pCurveStrip->Release();
	}

	// Remove Curve Strip from the Timeline
	m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnDeleteCurveStrip
	
HRESULT CPianoRollStrip::OnDeleteCurveStrip( CCurveStrip* pCurveStrip )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pPartRef != NULL );
	ASSERT( m_pPartRef->m_pDMPart != NULL );

	// Update the Part's GUID (and create a new Part if we need to)
	// This must happen so that m_pPartRef->m_pDMPart->m_bHasCurveTypes
	// is correctly updated
	m_pMIDIMgr->PreChangePartRef( m_pPartRef );

	BYTE bCCType = pCurveStrip->m_bCCType;
	WORD wRPNType = pCurveStrip->m_wRPNType;

	// Update all PianoRoll strips that are hard linked to this part.
	// Also update ourself
	POSITION position = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		CPianoRollStrip *pPRS = m_pMIDIMgr->m_pPRSList.GetNext(position);
		if( ((pPRS == this) || pPRS->m_pPartRef->m_fHardLink) &&
			(pPRS->m_pPartRef->m_pDMPart == m_pPartRef->m_pDMPart) )
		{
			// Remove from list of Curve Strip states
			POSITION pos = pPRS->m_lstCurveStripStates.GetHeadPosition();
			while( pos )
			{
				ioCurveStripState* pCurveStripState = pPRS->m_lstCurveStripStates.GetNext( pos );

				if( pCurveStripState->m_bCCType == bCCType )
				{
					if( (bCCType == CCTYPE_RPN_CURVE_STRIP)
					||	(bCCType == CCTYPE_NRPN_CURVE_STRIP) )
					{
						if( pCurveStripState->m_wRPNType == wRPNType )
						{
							POSITION pos2 = pPRS->m_lstCurveStripStates.Find( pCurveStripState );
							if( pos2 )
							{
								pPRS->m_lstCurveStripStates.RemoveAt( pos2 );
								delete pCurveStripState;
							}

							break;
						}
					}
					else
					{
						POSITION pos2 = pPRS->m_lstCurveStripStates.Find( pCurveStripState );
						if( pos2 )
						{
							pPRS->m_lstCurveStripStates.RemoveAt( pos2 );
							delete pCurveStripState;
						}

						break;
					}
				}
			}

			// Get a pointer to the pPRS's curve strip of type bCCType
			CCurveStrip *pTmpCurveStrip = pPRS->GetCurveStrip( bCCType, wRPNType );

			// Remove from list of Curve Strips
			if( pTmpCurveStrip )
			{
				pTmpCurveStrip->DeleteAllCurves();
				pPRS->RemoveCurveStrip( pTmpCurveStrip );
			}

			// Insert minimized CurveStrip if there are no other CurveStrips
			if( pPRS->m_lstCurveStripStates.IsEmpty() )
			{
				pPRS->m_CurveStripView = SV_MINIMIZED;
				pPRS->AddCurveStrips( FALSE );
			}
		}
	}

	// Tell the part it lost a curve strip
	m_pPartRef->m_pDMPart->m_bHasCurveTypes[ bCCType>>3 ] &= ~(1 << (bCCType & 0x07));

	// Let the object know about the changes
	m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );
	m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_DELETE_CC_TRACK ); 

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnNewCurveStrip
	
HRESULT CPianoRollStrip::OnNewCurveStrip( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPartRef != NULL );
	ASSERT( m_pPartRef->m_pDMPart != NULL );

	// Have user select the type of CC strip
	CDialogNewCCTrack dlgNewCCTrack;
	dlgNewCCTrack.m_pfCurveStripExists = CurveStripExists;
	dlgNewCCTrack.m_pVoid = this;
	if( dlgNewCCTrack.DoModal() == IDCANCEL )
	{
		return S_FALSE;
	}

	HRESULT hr = E_FAIL;


	// Update the Part's GUID (and create a new Part if we need to)
	// This must happen so that m_pPartRef->m_pDMPart->m_bHasCurveTypes
	// is correctly updated
	m_pMIDIMgr->PreChangePartRef( m_pPartRef );

	// Update all PianoRoll strips that are hard linked to this part.
	// Also update ourself
	POSITION position = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		CPianoRollStrip *pPRS = m_pMIDIMgr->m_pPRSList.GetNext(position);
		if( ((pPRS == this) || pPRS->m_pPartRef->m_fHardLink) &&
			(pPRS->m_pPartRef->m_pDMPart == m_pPartRef->m_pDMPart) )
		{
			if( pPRS->m_CurveStripView == SV_MINIMIZED )
			{
				if( pPRS->GetCurveStripState( dlgNewCCTrack.m_bCCType, dlgNewCCTrack.m_wRPNType ) )
				{
					DWORD dwPosition;
					VARIANT var;

					// Redraw minimized strip
					if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)pPRS, STP_POSITION, &var) ) )
					{
						IDMUSProdStrip* pIStrip;

						// The minimized curve strip shows up after the variation switch strip,
						// so add 2 to position of the piano roll strip
						dwPosition = V_I4(&var) + 2;

						if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->EnumStrip( dwPosition, &pIStrip ) ) )
						{
							m_pMIDIMgr->m_pTimeline->StripInvalidateRect( pIStrip, NULL, TRUE );
							pIStrip->Release();
						}
					}

					hr = S_OK;
				}
			}
			else
			{
				// Add the Curve Strip
				hr = pPRS->AddCurveStrip( dlgNewCCTrack.m_bCCType, dlgNewCCTrack.m_wRPNType );
			}
		}
	}

	if( hr == S_OK )
	{
		// Tell the part that is has another curve strip
		m_pPartRef->m_pDMPart->m_bHasCurveTypes[ dlgNewCCTrack.m_bCCType>>3 ] |= 1 << (dlgNewCCTrack.m_bCCType & 0x07);

		// Let the object know about the changes
		m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_ADD_CC_TRACK ); 
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DeterminePositionForCurveStrip
	
DWORD CPianoRollStrip::DeterminePositionForCurveStrip( BYTE bCCType, WORD wRPNType )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	WORD wCCTypeSortValue = GetCCTypeSortValue( bCCType, wRPNType );

	VARIANT var;

	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_POSITION, &var) ) )
	{
		CCurveStrip* pCurveStrip;
		IDMUSProdStrip* pIStrip;
		CLSID clsid;

		DWORD dwPosition = V_I4(&var);

		BOOL fContinue = TRUE;

		while( fContinue  &&  SUCCEEDED( m_pMIDIMgr->m_pTimeline->EnumStrip( ++dwPosition, &pIStrip ) ) )
		{
			var.vt = VT_BYREF;
			V_BYREF(&var) = &clsid;

			// Is this strip a Curve or MIDI strip?
			if( SUCCEEDED ( pIStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &var ) ) )
			{
				if( ::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
				{
					fContinue = FALSE;
				}
				// Is this a curve strip?
				else if( ::IsEqualCLSID( clsid, CLSID_CurveStrip ) )
				{
					// Is this curve strip supposed to go before ours?
					pCurveStrip = (CCurveStrip *)pIStrip;

					WORD wCCTypeListSortValue = GetCCTypeSortValue( pCurveStrip->m_bCCType, pCurveStrip->m_wRPNType );
					if( wCCTypeListSortValue > wCCTypeSortValue )
					{
						fContinue = FALSE;
					}
				}
				// Is this a variation switch strip?
				else if( ::IsEqualCLSID( clsid, CLSID_VarSwitchStrip ) )
				{
					// Skip over the variation switch strip
				}
				else
				{
					// Some other strip type
					fContinue = FALSE;
				}
			}
			else
			{
				fContinue = FALSE;
			}

			pIStrip->Release();
		}

		return dwPosition;
	}

	return 0xFFFFFFFF;

}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnChangeStripView

void CPianoRollStrip::OnChangeStripView( STRIPVIEW svNewStripView )
{
	if( m_StripView != svNewStripView )
	{
		CCurveStrip *pCurveStrip;

		// Change StripView field
		m_StripView = svNewStripView;

		// Remove all existing Curve Strips
		while( !m_lstCurveStrips.IsEmpty() )
		{
			pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.GetHead() );
			RemoveCurveStrip( pCurveStrip );
		}

		// If minimizing and gutter is not selected, unselect all events in our variations
		if( (m_StripView == SV_MINIMIZED) && m_fGutterSelected &&
			(m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0) )
		{
			if ( m_prsSelecting != PRS_NO_SELECT )
			{
				m_pMIDIMgr->UnselectAllEvents( m_pPartRef->m_pDMPart, m_dwVariations );
			}
		}

		AddCurveStrips( FALSE );

		// Let the object know about the changes
		//int nUndoString;
		if( m_StripView == SV_NORMAL )
		{
			//nUndoString = IDS_UNDO_STRIP_MAXIMIZED;
			m_prScrollBar.ShowWindow(TRUE);

			// Insert VarSwitch strip
			// Determine proper position for the VarSwitchStrip
			VARIANT varPos;
			if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_POSITION, &varPos) ) )
			{
				// Add VarSwitch Strip to the Timeline
				// This will return E_FAIL if the strip already is in the Timeline
				m_pMIDIMgr->m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)m_pVarSwitchStrip, V_I4(&varPos) + 1 );
			}
		}
		else // m_StripView == SV_MINIMIZED
		{
			//nUndoString = IDS_UNDO_STRIP_MINIMIZED;
			m_prScrollBar.ShowWindow(FALSE);

			// Remove VarSwitch strip
			// This will return E_FAIL if the strip is already removed.
			m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pVarSwitchStrip );
		}

		// No need to update performance engine
		m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;

		// Fix 27282: Minimize/Maximize should not be Undo/Redo events
		m_pMIDIMgr->UpdateOnDataChanged( 0 ); //nUndoString
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnChangeCurveStripView

void CPianoRollStrip::OnChangeCurveStripView( STRIPVIEW svNewStripView )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pPartRef != NULL );
	ASSERT( m_pPartRef->m_pDMPart != NULL );

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
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pCurveStrip, STP_STRIPVIEW, var );
		}

		return;
	}

	// Change Curve StripView field
	m_CurveStripView = svNewStripView;

	// Remove all existing Curve Strips
	while( m_lstCurveStrips.IsEmpty() == FALSE )
	{
		pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.GetHead() );

		// Unselect all curves if the curve strips are being minimzed
		if( svNewStripView == SV_MINIMIZED )
		{
			pCurveStrip->UnselectAllCurvesInPart();
		}
		RemoveCurveStrip( pCurveStrip );
	}

	// If the gutter selection is active, always select all curves in the gutter range.
	// This overrides the unselection that may have been done above
	if( m_fGutterSelected && (m_lBeginTimelineSelection >= 0) && (m_lEndTimelineSelection > 0))
	{
		SelectEventsBetweenTimes( m_lBeginTimelineSelection, m_lEndTimelineSelection, NULL );
	}

	// Add new Curve Strips
	AddCurveStrips( FALSE );

	// Let the object know about the changes
	m_pMIDIMgr->m_fDirty = TRUE;
	m_pMIDIMgr->UpdatePartParamsAfterChange( m_pPartRef->m_pDMPart );

	// No need to update performance engine
	m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
	// Fix 27282: Minimize/Maximize should not be Undo/Redo events
	m_pMIDIMgr->UpdateOnDataChanged( 0 );  //IDS_UNDO_STRIPVIEW
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::InvalidateCurveStrips

void CPianoRollStrip::InvalidateCurveStrips( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	CCurveStrip* pCurveStrip;

    POSITION pos = m_lstCurveStrips.GetHeadPosition();
    while( pos )
    {
        pCurveStrip = m_lstCurveStrips.GetNext( pos );

		m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)pCurveStrip, NULL, FALSE );
		pCurveStrip->RefreshCurvePropertyPage();
    }
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::InvalidateMarkerStrip

void CPianoRollStrip::InvalidateMarkerStrip( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pVarSwitchStrip != NULL );

	m_pMIDIMgr->m_pTimeline->StripInvalidateRect( m_pVarSwitchStrip, NULL, FALSE );
	m_pVarSwitchStrip->RefreshPropertyPage();
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SaveCurveStripStateData

HRESULT CPianoRollStrip::SaveCurveStripStateData( IDMUSProdRIFFStream* pIRiffStream )
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

		// Initialize state flags
		pCurveStripState->m_dwFlags = 0;
			
		// Active strip?
		VARIANT var;
		if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			IUnknown* pIUnknown = V_UNKNOWN(&var);
			if( pIUnknown )
			{
				IDMUSProdStrip* pIActiveStrip;
				if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
				{
					CCurveStrip* pCurveStrip = GetCurveStrip( pCurveStripState->m_bCCType, pCurveStripState->m_wRPNType );
					if( pCurveStrip 
					&&  (IDMUSProdStrip *)pCurveStrip == pIActiveStrip )
					{
						pCurveStripState->m_dwFlags |= STPST_ACTIVESTRIP;
					}
					pIActiveStrip->Release();
				}
				pIUnknown->Release();
			}
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
// CPianoRollStrip::FixPartRefPtr

BOOL CPianoRollStrip::FixPartRefPtr( void ) const
{
	// BUGBUG: This method should contain code to try and re-acquire a valid PartRef pointer
	// and to reset m_pPartRef
	// For now, just fail
	ASSERT(FALSE);
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ValidPartRefPtr

BOOL CPianoRollStrip::ValidPartRefPtr( void ) const
{
	if( m_pPartRef != NULL )
	{
		if( m_pPartRef->m_pDMPart != NULL )
		{
			return TRUE;
		}
	}
	return FixPartRefPtr();
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DoQuantize

HRESULT CPianoRollStrip::DoQuantize( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store window that has focus
	HWND hwndFocus = ::GetFocus();

	// Initialize the dialog's settings
	CQuantizeDlg dlgQuantize;
	dlgQuantize.m_qtTarget = m_pMIDIMgr->m_qtQuantTarget;
	dlgQuantize.m_dwFlags = m_pMIDIMgr->m_dwQuantFlags;
	dlgQuantize.m_lResolution = m_pMIDIMgr->m_bQuantResolution;
	dlgQuantize.m_lStrength = m_pMIDIMgr->m_bQuantStrength;
	dlgQuantize.m_TimeSignature = m_pPartRef->m_pDMPart->m_TimeSignature;

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
		// Set the MIDIMgr's quantization settings
		m_pMIDIMgr->m_qtQuantTarget = dlgQuantize.m_qtTarget;
		m_pMIDIMgr->m_dwQuantFlags = dlgQuantize.m_dwFlags;

		ASSERT( (dlgQuantize.m_lResolution < UCHAR_MAX) && (dlgQuantize.m_lResolution > 0 ) );
		m_pMIDIMgr->m_bQuantResolution = (BYTE)dlgQuantize.m_lResolution;

		ASSERT( (dlgQuantize.m_lStrength <= 100) && (dlgQuantize.m_lStrength >= 0 ) );
		m_pMIDIMgr->m_bQuantStrength = (BYTE)dlgQuantize.m_lStrength;

		// Quantize either selected notes, entire Part, or entire Pattern
		switch( m_pMIDIMgr->m_qtQuantTarget )
		{
		case QUANTIZE_TARGET_SELECTED:
			// Assume something will change
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			m_pMIDIMgr->QuantizeSelectedNotes( m_pPartRef->m_pDMPart, m_dwVariations );
			break;
		case QUANTIZE_TARGET_PART:
			// Assume something will change
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			m_pMIDIMgr->QuantizeEntirePart( m_pPartRef->m_pDMPart );
			break;
		case QUANTIZE_TARGET_PATTERN:
			m_pMIDIMgr->QuantizeEntirePattern( );
			break;
		default:
			break;
		}
		// m_pMIDIMgr->Quantize*() takes care of updates automatically.

		// Always recompute m_SelectedPropNote and update the property page
		m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
		m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
	}

	// Restore focus
	if( hwndFocus )
	{
		::SetFocus( hwndFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DoVelocity

HRESULT CPianoRollStrip::DoVelocity( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store window that has focus
	HWND hwndFocus = ::GetFocus();

	// Initialize the dialog's settings
	CDialogVelocity dlgVelocity;
	dlgVelocity.m_vtTarget = m_pMIDIMgr->m_vtVelocityTarget;
	dlgVelocity.m_fAbsolute = m_pMIDIMgr->m_fVelocityAbsolue;
	dlgVelocity.m_fPercent = m_pMIDIMgr->m_fVelocityPercent;
	dlgVelocity.m_lAbsoluteChangeStart = m_pMIDIMgr->m_lVelocityAbsChangeStart;
	dlgVelocity.m_lAbsoluteChangeEnd = m_pMIDIMgr->m_lVelocityAbsChangeEnd;
	dlgVelocity.m_bCompressMin = m_pMIDIMgr->m_bVelocityCompressMin;
	dlgVelocity.m_bCompressMax = m_pMIDIMgr->m_bVelocityCompressMax;

	// Check if we should enable the 'Selected Note(s)' option
	if( CanCopy() == S_OK )
	{
		dlgVelocity.m_fEnableSelected = TRUE;
	}
	else
	{
		dlgVelocity.m_fEnableSelected = FALSE;
		if( dlgVelocity.m_vtTarget == VELOCITY_TARGET_SELECTED )
		{
			dlgVelocity.m_vtTarget = VELOCITY_TARGET_PART;
		}
	}

	// Display the dialog.  Only act if the user clicked on 'OK'.
	if( dlgVelocity.DoModal() == IDOK )
	{
		// Set the MIDIMgr's quantization settings
		m_pMIDIMgr->m_vtVelocityTarget = dlgVelocity.m_vtTarget;
		m_pMIDIMgr->m_fVelocityAbsolue = dlgVelocity.m_fAbsolute;
		m_pMIDIMgr->m_fVelocityPercent = dlgVelocity.m_fPercent;
		m_pMIDIMgr->m_lVelocityAbsChangeStart = dlgVelocity.m_lAbsoluteChangeStart;
		m_pMIDIMgr->m_lVelocityAbsChangeEnd = dlgVelocity.m_lAbsoluteChangeEnd;
		m_pMIDIMgr->m_bVelocityCompressMin = dlgVelocity.m_bCompressMin;
		m_pMIDIMgr->m_bVelocityCompressMax = dlgVelocity.m_bCompressMax;

		// Quantize either selected notes, entire Part, or entire Pattern
		switch( m_pMIDIMgr->m_vtVelocityTarget )
		{
		case VELOCITY_TARGET_SELECTED:
			// Assume something will change
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			m_pMIDIMgr->VelocitizeSelectedNotes( m_pPartRef->m_pDMPart, m_dwVariations );
			break;
		case VELOCITY_TARGET_PART:
			// Assume something will change
			m_pMIDIMgr->PreChangePartRef( m_pPartRef );
			m_pMIDIMgr->VelocitizeEntirePart( m_pPartRef->m_pDMPart );
			break;
		case VELOCITY_TARGET_PATTERN:
			m_pMIDIMgr->VelocitizeEntirePattern( );
			break;
		default:
			break;
		}
		// m_pMIDIMgr->Velocitize*() takes care of updates automatically.

		// Always recompute m_SelectedPropNote and update the property page
		m_pMIDIMgr->ComputeSelectedPropNote( m_pPartRef->m_pDMPart, m_dwVariations );
		m_pMIDIMgr->RefreshPropertyPage( m_pPartRef->m_pDMPart );
	}

	// Restore focus
	if( hwndFocus )
	{
		::SetFocus( hwndFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetStripRect

BOOL CPianoRollStrip::GetStripRect(CRect& rectStrip)
{
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectStrip;
	if( FAILED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
	{
		return FALSE;
	}

	// adjust the strip rect to encompass the WHOLE strip, not only the part
	// that is visible on the screen
	POINT ptTop;
	POINT ptBottom;
	ptTop.x = 0;
	ptBottom.x = 0;
	if( m_StripView == SV_MINIMIZED )
	{
		ptTop.y = 0;
		ptBottom.y = MINIMIZE_HEIGHT;
	}
	else
	{
		if( m_fHybridNotation )
		{
			ptBottom.y = m_lMaxNoteHeight * 38;
		}
		else
		{
			ptBottom.y = m_lMaxNoteHeight * 128;
		}

		ptTop.y = VARIATION_BUTTON_HEIGHT * 2;
		ptBottom.y += VARIATION_BUTTON_HEIGHT * 2;
	}

	
	VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &ptTop)));
	VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &ptBottom)));

	rectStrip.top = ptTop.y;
	rectStrip.bottom = ptBottom.y;
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// GroupBitsToString

CString GroupBitsToString( DWORD dwGroupBits )
{
	CString strText, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( dwGroupBits & (1 << i) )
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
	
	return strText;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetName

CString CPianoRollStrip::GetName( void )
{
	// Initialize the name to return
	CString strName, strPartRef;

	// Validate MIDIMgr and PartRef pointers
	if( m_pMIDIMgr && m_pPartRef )
	{

		// Set strPartRef to 'unnamed' if the partref has no name, otherwise use the
		// PartRef's name
		if( m_pPartRef->m_strName.IsEmpty() )
		{
			// If PartRef has no name, display IDS_UNNAMED
			strPartRef.LoadString( IDS_UNNAMED );
		}
		else
		{
			strPartRef = m_pPartRef->m_strName;
		}

		// If available, query the IDMUSProdPChannelName interface for the PChannel name
		WCHAR wszName[MAX_PATH];
		if( m_pMIDIMgr->m_pIPChannelName 
		&&	SUCCEEDED( m_pMIDIMgr->m_pIPChannelName->GetPChannelName( m_pPartRef->m_dwPChannel, wszName ) ) )
		{
			if( m_pPartRef->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strName.Format( "%d (%S): %s", m_pPartRef->m_dwPChannel + 1, wszName, strPartRef );
			}
			else
			{
				CString strNewName;
				if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					strNewName.LoadString( IDS_BROADCAST_SEG );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					strNewName.LoadString( IDS_BROADCAST_PERF );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
				{
					strNewName.LoadString( IDS_BROADCAST_APATH );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
				{
					strNewName.LoadString( IDS_BROADCAST_GRP );
				}
				strName.Format( "%s (%S): %s", strNewName, wszName, strPartRef );
			}
		}
		// If unable to get name from IDMUSProdPChannelName interface, default to just the #
		else
		{
			if( m_pPartRef->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strName.Format( "%d: %s", m_pPartRef->m_dwPChannel + 1, strPartRef );
			}
			else
			{
				CString strNewName;
				if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					strNewName.LoadString( IDS_BROADCAST_SEG );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					strNewName.LoadString( IDS_BROADCAST_PERF );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
				{
					strNewName.LoadString( IDS_BROADCAST_APATH );
				}
				else if( m_pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
				{
					strNewName.LoadString( IDS_BROADCAST_GRP );
				}
				strName.Format( "%s: %s", strNewName, strPartRef );
			}
		}

		if( m_pPartRef->m_fHardLink )
		{
			CString strLink;
			strLink.LoadString(IDS_LINKED);
			strName = strLink + strName;
		}

		if( m_pMIDIMgr->m_pIDMTrack )
		{
			strName = GroupBitsToString( m_pMIDIMgr->m_dwGroupBits ) + CString(": ") + strName;
		}
	}

	return strName;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UpdateCurveStripGutterSelection

void CPianoRollStrip::UpdateCurveStripGutterSelection( BOOL fChanged )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	CCurveStrip* pCurveStrip;

    POSITION pos = m_lstCurveStrips.GetHeadPosition();
    while( pos )
    {
        pCurveStrip = m_lstCurveStrips.GetNext( pos );

		pCurveStrip->OnGutterSelectionChange( fChanged );
    }
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SelectEventsBetweenTimes

BOOL CPianoRollStrip::SelectEventsBetweenTimes( long lStart, long lEnd, int *pnSelected )
{
	BOOL fChange = FALSE;
	int iRes = 0;

	// Make lStart be within the first repetition of this part
	long lPartLength = m_pPartRef->m_pDMPart->GetClockLength();

	if( lStart >= lPartLength )
	{
		long lOffset = (lStart / lPartLength) * lPartLength;
		lStart -= lOffset;
		lEnd -= lOffset;
	}

	if( lEnd - lStart >= lPartLength )
	{
		// Select the entire part
		fChange = SelectOnlyEventsBetweenTimesHelper( 0, lPartLength, &iRes );
	}
	else
	{
		if( lEnd <= lPartLength )
		{
			// Select only between the selected times
			fChange = SelectOnlyEventsBetweenTimesHelper( lStart, lEnd, &iRes );
		}
		else
		{
			ASSERT( lEnd > lPartLength );

			// Select the start time
			fChange = SelectEventsBetweenTimesHelper( 0, lEnd - lPartLength, &iRes );

			// Unselect the area between the two
			fChange |= UnSelectEventsBetweenTimesHelper( lEnd - lPartLength, lStart );

			// Select the end time
			fChange |= SelectEventsBetweenTimesHelper( lStart, lPartLength, &iRes );
		}
	}

	if( pnSelected )
	{
		*pnSelected = iRes;
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SelectOnlyEventsBetweenTimesHelper

BOOL CPianoRollStrip::SelectOnlyEventsBetweenTimesHelper( long lStart, long lEnd, int *pnSelected )
{
	int iRes = 0;
	BOOL fChange = FALSE;

	// Check notes
	CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		// If the event belongs to a displayed variation
		if ( pEvent->m_dwVariation & m_dwVariations )
		{
			long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
			if( ( lNoteStart >= lStart ) &&
				( lNoteStart <= lEnd ) )
			{
				// If NOTE event, update our counter
				iRes ++;

				if( !pEvent->m_fSelected )
				{
					pEvent->m_fSelected = TRUE;
					fChange = TRUE;

					if ( m_pActiveDMNote == NULL )
					{
						m_pActiveDMNote = (CDirectMusicStyleNote*) pEvent;
						m_pMIDIMgr->UpdateStatusBarDisplay();
					}
				}
			}
			else if( pEvent->m_fSelected )
			{
				pEvent->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
		else if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	// Check curves
	pEvent = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		if( m_pPartRef->m_pDMPart->m_fSelecting
		&&	(m_pPartRef->m_pDMPart->m_bSelectionCC == CurveTypeToStripCCType( static_cast<CDirectMusicStyleCurve *>(pEvent)))
		&&	(m_pPartRef->m_pDMPart->m_wSelectionParamType == static_cast<CDirectMusicStyleCurve *>(pEvent)->m_wParamType) )
		{
			// Don't touch this curve - its strip was clicked on
			continue;
		}

		// If the event belongs to a displayed variation
		if ( pEvent->m_dwVariation & m_dwVariations )
		{
			long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
			if( ( lNoteStart >= lStart ) &&
				( lNoteStart <= lEnd ) )
			{
				if( !pEvent->m_fSelected )
				{
					pEvent->m_fSelected = TRUE;
					fChange = TRUE;
				}
			}
			else if( pEvent->m_fSelected )
			{
				pEvent->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
		else if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	// Don't look at markers if their strip was clicked on
	if( !m_pPartRef->m_pDMPart->m_fSelecting
	||	(m_pPartRef->m_pDMPart->m_bSelectionCC != 0xFE) )
	{
		// Check markers
		pEvent = m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();
		for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
		{
			// If the event belongs to a displayed variation
			if ( MARKER_AND_VARIATION( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), m_dwVariations) )
			{
				long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
				if( ( lNoteStart >= lStart ) &&
					( lNoteStart <= lEnd ) )
				{
					if( !pEvent->m_fSelected )
					{
						pEvent->m_fSelected = TRUE;
						fChange = TRUE;
					}
				}
				else if( pEvent->m_fSelected )
				{
					pEvent->m_fSelected = FALSE;
					fChange = TRUE;
				}
			}
			else if( pEvent->m_fSelected )
			{
				pEvent->m_fSelected = FALSE;
				fChange = TRUE;
			}
		}
	}

	if( pnSelected )
	{
		*pnSelected += iRes;
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SelectEventsBetweenTimesHelper

BOOL CPianoRollStrip::SelectEventsBetweenTimesHelper( long lStart, long lEnd, int *pnSelected )
{
	int iRes = 0;
	BOOL fChange = FALSE;

	// Check notes
	CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( lNoteStart < lStart )
		{
			continue;
		}
		else if(  lNoteStart > lEnd)
		{
			// Exit early
			break;
		}

		// If the event belongs to a displayed variation
		if ( pEvent->m_dwVariation & m_dwVariations )
		{
			// If NOTE event, update our counter
			iRes ++;

			if( !pEvent->m_fSelected )
			{
				pEvent->m_fSelected = TRUE;
				fChange = TRUE;

				if ( m_pActiveDMNote == NULL )
				{
					m_pActiveDMNote = (CDirectMusicStyleNote*) pEvent;
					m_pMIDIMgr->UpdateStatusBarDisplay();
				}
			}
			// If already selected, no change necessary
		}
		else if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	// Check curves
	pEvent = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( lNoteStart < lStart )
		{
			continue;
		}
		else if( lNoteStart > lEnd )
		{
			// Exit early
			break;
		}

		// If the event belongs to a displayed variation
		if ( pEvent->m_dwVariation & m_dwVariations )
		{
			if( !pEvent->m_fSelected )
			{
				pEvent->m_fSelected = TRUE;
				fChange = TRUE;
			}
			// If already selected, no change necessary
		}
		else if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	// Check markers
	pEvent = m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( lNoteStart < lStart )
		{
			continue;
		}
		else if( lNoteStart > lEnd )
		{
			// Exit early
			break;
		}

		// If the event belongs to a displayed variation
		if ( MARKER_AND_VARIATION( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), m_dwVariations ) )
		{
			if( !pEvent->m_fSelected )
			{
				pEvent->m_fSelected = TRUE;
				fChange = TRUE;
			}
			// If already selected, no change necessary
		}
		else if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
	}

	if( pnSelected )
	{
		*pnSelected += iRes;
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UnSelectEventsBetweenTimesHelper

BOOL CPianoRollStrip::UnSelectEventsBetweenTimesHelper( long lStart, long lEnd )
{
	BOOL fChange = FALSE;

	// Check notes
	CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( lNoteStart < lStart )
		{
			continue;
		}
		if(  lNoteStart > lEnd)
		{
			// Exit early
			break;
		}

		// If the event is selected, unselect it
		if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;

			if ( m_pActiveDMNote == pEvent )
			{
				m_pActiveDMNote = NULL;
				m_pMIDIMgr->UpdateStatusBarDisplay();
			}
		}
		// If already UnSelected, no change necessary
	}

	// Check curves
	pEvent = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( (lNoteStart < lStart) || (lNoteStart > lEnd) )
		{
			continue;
		}

		// If the event is selected, unselect it
		if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
		// If already UnSelected, no change necessary
	}

	// Check markers
	pEvent = m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();
	for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
	{
		long lNoteStart = m_pPartRef->m_pDMPart->AbsTime( pEvent );
		if( (lNoteStart < lStart) || (lNoteStart > lEnd) )
		{
			continue;
		}

		// If the event is selected, unselect it
		if( pEvent->m_fSelected )
		{
			pEvent->m_fSelected = FALSE;
			fChange = TRUE;
		}
		// If already UnSelected, no change necessary
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ValidPasteVariations

BOOL CPianoRollStrip::ValidPasteVariations( DWORD dwVariations )
{
	// Create a mapping of source variations to destination variations
	int nClip, nCur, nTmp;

	nCur = 0;
	for ( nClip = 0; nClip < 32; nClip++ )
	{
		if ( dwVariations & (1 << nClip) )
		{
			for ( nTmp = nCur; nTmp < 32; nTmp++ )
			{
				if ( m_dwVariations & (1 << nTmp) )
				{
					nCur = nTmp + 1;
					break;
				}
			}
			if ( nTmp == 32 )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


/*
/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DrawNote

void CPianoRollStrip::DrawNote( HDC hDC, RECT *pRectNote, char cValue,
							    BOOL fEarly, HBRUSH brushHatchVert,
								BOOL fInverstionId, HBRUSH brushHatchDiag,
								HPEN penOverlapping )
{
	// Validate parameters
	ASSERT( pRectNote );

	if( fEarly )
	{
		ASSERT( brushHatchVert );
		::FillRect( hDC, pRectNote, brushHatchVert );
	}
	else if ( fInverstionId )
	{
		ASSERT( brushHatchDiag );
		::FillRect( hDC, pRectNote, brushHatchDiag );
	}
	else
	{
		// Normal
		::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, pRectNote, NULL, 0, NULL);
	}

	// draw overlapping notes with an outline of dashed lines
	if( (cValue >= 0) && (cValue < 128) )
	{
		ASSERT( penOverlapping );
		const RECT* pRect = m_aNoteRectList[cValue].GetFirst();
		::SelectObject( hDC, penOverlapping );
		while (pRect)
		{
			// compute the intersection of the notes
			CRect rect;
			rect.IntersectRect( pRect, pRectNote);

			// draw it
			if ((rect.left != rect.right) || (rect.top != rect.bottom))
			{
				// These rects have already been offset by -lXOffset
				::MoveToEx( hDC, rect.left, rect.bottom, NULL );
				::LineTo( hDC, rect.left, rect.top );
				::LineTo( hDC, rect.right, rect.top );
				::LineTo( hDC, rect.right, rect.bottom );
			}
			// Get the next overlapping note
			pRect = m_aNoteRectList[cValue].GetNext();
		}
		m_aNoteRectList[cValue].InsertRect( *pRectNote );
	}
}
*/


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnLoop

void CPianoRollStrip::OnLoop( void )
{
	// Validate PartRef pointer
	if( ValidPartRefPtr() && m_pMIDIMgr->m_fRecordEnabled && IsEnginePlaying() )
	{
		// If we're playing numerical variations
		if( m_pPartRef->m_bRandomVariation == DMUS_VARIATIONT_SEQUENTIAL )
		{
			DWORD dwNextVariation = NextNumericalVarition( m_dwVariations, m_pPartRef->m_pDMPart->m_dwVariationsDisabled );
			if( dwNextVariation == m_dwVariations )
			{
				// No change
				return;
			}

			m_dwVariations = dwNextVariation;

			InvalidateVariationBar();
			InvalidatePianoRoll();
			InvalidateCurveStrips();
			InvalidateMarkerStrip();

			// If the MIDI Mgr won't update the pattern editor (because of a note insertion),
			// update the pattern editor because of the variation change
			if( !m_pMIDIMgr->m_fNoteInserted )
			{
				m_pMIDIMgr->m_fDirty = TRUE;
				m_pMIDIMgr->UpdateOnDataChanged( NULL );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UnselectGutterRange

void CPianoRollStrip::UnselectGutterRange( void )
{
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_pPartRef->m_pDMPart->m_fSelecting = TRUE;
	m_pPartRef->m_pDMPart->m_bSelectionCC = 0xFF;
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_pPartRef->m_pDMPart->m_fSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SendAllNotesOffIfNotPlaying

void CPianoRollStrip::SendAllNotesOffIfNotPlaying( void )
{
	// If we're not playing, send all notes off to the performance engine
	if ( !IsEnginePlaying() )
	{
		DMUS_MIDI_PMSG *pDMMIDIEvent = NULL;
		if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
		{
			ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
			// PMSG fields
			pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
			pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
			pDMMIDIEvent->dwPChannel = m_pPartRef->m_dwPChannel;
			pDMMIDIEvent->dwVirtualTrackID = 1;
			pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

			// DMMIDIEvent fields
			pDMMIDIEvent->bStatus = MIDI_CCHANGE;
			// Send Reset All Controllers (121)
			pDMMIDIEvent->bByte1 = 121;

			m_pMIDIMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
			// The playback engine will release the event

			if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pDMMIDIEvent ) ) )
			{
				ZeroMemory( pDMMIDIEvent, sizeof(DMUS_MIDI_PMSG) );
				// PMSG fields
				pDMMIDIEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
				pDMMIDIEvent->dwFlags = DMUS_PMSGF_REFTIME;
				pDMMIDIEvent->dwPChannel = m_pPartRef->m_dwPChannel;
				pDMMIDIEvent->dwVirtualTrackID = 1;
				pDMMIDIEvent->dwType = DMUS_PMSGT_MIDI;

				// DMMIDIEvent fields
				pDMMIDIEvent->bStatus = MIDI_CCHANGE;
				// Send All Notes Off (123)
				pDMMIDIEvent->bByte1 = 123;

				m_pMIDIMgr->SendPMsg( (DMUS_PMSG *)pDMMIDIEvent );
				// The playback engine will release the event
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::OnDestroy

HRESULT CPianoRollStrip::OnDestroy( void )
{
	ASSERT( m_pStripMgr != NULL );

	// Need to save the Curve Strip states (See bug 47337)
	//ioCurveStripState* pCurveStripState;
	//while( !m_lstCurveStripStates.IsEmpty() )
	//{
	//	pCurveStripState = static_cast<ioCurveStripState*>( m_lstCurveStripStates.RemoveHead() );
	//	delete pCurveStripState;
	//}

	CCurveStrip *pCurveStrip;
	while( !m_lstCurveStrips.IsEmpty() )
	{
		pCurveStrip = static_cast<CCurveStrip*>( m_lstCurveStrips.RemoveHead() );
		if( m_pMIDIMgr && m_pMIDIMgr->m_pTimeline )
		{
			m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );
		}
		pCurveStrip->Release();
	}

	// Remove VarSwitch strip
	// This will return E_FAIL if the strip is already removed.
	m_pMIDIMgr->m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pVarSwitchStrip );

	// Delete the Bitmap buttons
	if( InterlockedDecrement( &m_lBitmapRefCount ) == 0 )
	{
		if( m_BitmapBlankButton.GetSafeHandle() != NULL )
		{
			m_BitmapBlankButton.DeleteObject();
		}
		if( m_BitmapPressedButton.GetSafeHandle() != NULL )
		{
			m_BitmapPressedButton.DeleteObject();
		}
		if( m_BitmapBlankInactiveButton.GetSafeHandle() != NULL )
		{
			m_BitmapBlankInactiveButton.DeleteObject();
		}
		if( m_BitmapPressedInactiveButton.GetSafeHandle() != NULL )
		{
			m_BitmapPressedInactiveButton.DeleteObject();
		}
		if( m_BitmapGutter.GetSafeHandle() != NULL )
		{
			m_BitmapGutter.DeleteObject();
		}
		if( m_BitmapPressedGutter.GetSafeHandle() != NULL )
		{
			m_BitmapPressedGutter.DeleteObject();
		}
		if( m_BitmapMoaw.GetSafeHandle() != NULL )
		{
			m_BitmapMoaw.DeleteObject();
		}
		if( m_BitmapPressedMoaw.GetSafeHandle() != NULL )
		{
			m_BitmapPressedMoaw.DeleteObject();
		}
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

	if (m_prScrollBar.GetSafeHwnd())
	{
		m_prScrollBar.ShowWindow( FALSE );
		//m_prScrollBar.Detach();
		m_prScrollBar.DestroyWindow();
	}

	// Kill timer, if active
	KillTimer();
	KillVariationTimer();
	return S_OK;
}

void CPianoRollStrip::DrawMinimizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset )
{
	UNREFERENCED_PARAMETER( nTopNote );
	UNREFERENCED_PARAMETER( nBottomNote );

	long lNoteHeight = MINIMIZE_HEIGHT / m_bMinimizeNoteRange;
	if( lNoteHeight == 0 )
	{
		lNoteHeight++;
	}

	// Compute the length of the part, in grids
	long lMaxGridValue = CLOCKS_TO_GRID( mtPartLength - 1, m_pPartRef->m_pDMPart);

	RECT rectNote;
	CDirectMusicEventItem* pEvent = pDMNote;
	while( pEvent )
	{
		if ( (fSelected == pEvent->m_fSelected) &&
			(pEvent->m_dwVariation & m_dwVariations) &&
			(pEvent->m_mtGridStart <= lMaxGridValue) )
		{
			// Get position to draw at
			rectNote.top = ((m_bMinimizeTopNote - ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue) * MINIMIZE_HEIGHT) / m_bMinimizeNoteRange;
			rectNote.bottom = rectNote.top + lNoteHeight;

			// Get start and end times of note
			MUSIC_TIME mtAbsStartTime = m_pPartRef->m_pDMPart->AbsTime( pEvent );

			MUSIC_TIME mtStart = mtOffset + mtAbsStartTime;
			MUSIC_TIME mtEnd = mtStart + ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;

			// Check if note is visible - if so, draw it
			if( (mtStart <= lEndTime) && (mtEnd >= lStartTime) )
			{
				// Compute the note's start and end position
				m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtEnd, &rectNote.right );
				m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtStart, &rectNote.left );
				rectNote.right -= lXOffset;
				rectNote.left -= lXOffset;

				// Ensure the note is at least one pixel long
				if ( rectNote.left == rectNote.right )
				{
					rectNote.right++;
				}

				// Draw it
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
			}
			// Note is not visible
			else if( mtStart > lEndTime )
			{
				// We've gone past the end of the display - exit early
				break;
			}
		}

		pEvent = pEvent->GetNext();
	}
}


void CPianoRollStrip::DrawMaximizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset )
{
	// Set the note color
	COLORREF crNoteColor;
	if( fSelected )
	{
		crNoteColor = ::GetNearestColor(hDC, m_crSelectedNoteColor);
	}
	else
	{
		crNoteColor = ::GetNearestColor(hDC, m_crUnselectedNoteColor);
	}

	// Create pens and brushes for drawing the notes
	HPEN hpenOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetNearestColor(hDC, GetSysColor(COLOR_WINDOW)) );
	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenOverlappingLine ));
	HBRUSH hbrushHatchDiag = ::CreateHatchBrush( HS_DIAGCROSS, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 
	HBRUSH hbrushHatchVert = ::CreateHatchBrush( HS_VERTICAL, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 
	HBRUSH hbrushHatchOverlapping = ::CreateHatchBrush( HS_FDIAGONAL, ::GetNearestColor(hDC, COLOR_HATCH_OVERLAPPING) ); 

	/* For line from note's position to note's grid
	HPEN hpenOffsetLine = ::CreatePen( PS_SOLID, 1, ::GetNearestColor(hDC, RGB(160, 0, 160) );
	*/

	// Compute the length of the part, in grids
	long lMaxGridValue = CLOCKS_TO_GRID( mtPartLength - 1, m_pPartRef->m_pDMPart);

	RECT rectNote;
	CDirectMusicEventItem* pEvent = pDMNote;

	// Seek to the first note that matches our selection criteria
	while( pEvent && (!(fSelected == pEvent->m_fSelected) ||
					  !(pEvent->m_dwVariation & m_dwVariations) ) )
	{
		pEvent = pEvent->GetNext();
	}

	while( pEvent )
	{
		if ((pEvent->m_mtGridStart <= lMaxGridValue) &&
			(((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue <= nTopNote) &&
			(((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue >= nBottomNote) )
		{
			// Get start and end times of note
			MUSIC_TIME mtAbsStartTime = m_pPartRef->m_pDMPart->AbsTime( pEvent );

			MUSIC_TIME mtStart = mtOffset + mtAbsStartTime;
			MUSIC_TIME mtEnd = mtStart + ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;

			// If note is visible, draw it
			if( (mtStart <= lEndTime) && (mtEnd >= lStartTime) )
			{
				// Get position to draw at
				GetNoteRect( (CDirectMusicStyleNote*)pEvent, &rectNote );

				// If necessary, Compute the note's start and end position
				if( mtOffset )
				{
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtEnd, &rectNote.right );
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtStart, &rectNote.left );
				}

				// Offset the rect so we draw it correctly
				rectNote.left -= lXOffset;
				rectNote.right -= lXOffset;

				// Draw the note
				if( ( (mtAbsStartTime < 0) || (mtAbsStartTime >= mtPartLength) )
				&&	hbrushHatchVert )
				{
					// Early (or late)
					::FillRect( hDC, &rectNote, hbrushHatchVert );
				}
				else if( ((CDirectMusicStyleNote*)pEvent)->m_bInversionId && hbrushHatchDiag )
				{
					// has an inversion ID
					::FillRect( hDC, &rectNote, hbrushHatchDiag );
				}
				else
				{
					// Normal
					::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
				}

				DrawOverlapping( hDC, ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue, &rectNote, hbrushHatchOverlapping, crNoteColor );

				/* For line from note's position to note's grid
				// Draw a line to the beat the grid the note belongs to
				if( (pEvent->m_nTimeOffset < 0) || (pEvent->m_nTimeOffset >= m_lGridClocks) )
				{
					::SelectObject( hDC, hpenOffsetLine );
					::MoveToEx( hDC, rectNote.left, (rectNote.top + rectNote.bottom) / 2, NULL );
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtOffset + GRID_TO_CLOCKS( pEvent->m_mtGridStart, m_pPartRef->m_pDMPart ), &rectNote.left );
					::LineTo( hDC, rectNote.left, rectNote.bottom );
					::LineTo( hDC, rectNote.left, rectNote.top );
					::SelectObject( hDC, hpenOverlappingLine );
				}
				*/
			}
			// Note is not visible
			else if( mtStart > lEndTime )
			{
				// We've gone past the end of the display - exit early
				break;
			}
		}

		pEvent = pEvent->GetNext();

		while( pEvent && (!(fSelected == pEvent->m_fSelected) ||
						  !(pEvent->m_dwVariation & m_dwVariations) ) )
		{
			pEvent = pEvent->GetNext();
		}
	}

	// Re-select the old pen
	::SelectObject( hDC, hpenOld );

	// Delete all pens and brushes we created
	/* For line from note's position to note's grid
	if( hpenOffsetLine )
	{
		::DeleteObject( hpenOffsetLine );
	}
	*/
	if( hpenOverlappingLine )
	{
		::DeleteObject( hpenOverlappingLine );
	}
	if( hbrushHatchDiag )
	{
		::DeleteObject( hbrushHatchDiag );
	}
	if( hbrushHatchVert )
	{
		::DeleteObject( hbrushHatchVert );
	}
	if( hbrushHatchOverlapping )
	{
		::DeleteObject( hbrushHatchOverlapping );
	}
}

void CPianoRollStrip::DrawHybridMaximizedAccidentals( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset )
{
	UNREFERENCED_PARAMETER(fSelected);

	// Create and select the font to draw the sharps and flats with
	HFONT hOldFont, hfont;
	hfont = GetAccidentalFont();
	hOldFont = static_cast<HFONT>(::SelectObject( hDC, hfont ));

	// Load the sharp/flat strings
	CString strSharp, strFlat, strNatural;
	strSharp.LoadString(IDS_SHARP_TEXT);
	strFlat.LoadString(IDS_FLAT_TEXT);
	strNatural.LoadString(IDS_NATURAL_TEXT);

	// Set up the text drawing modes
	int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
	COLORREF crOldColor = ::SetTextColor( hDC, ::GetNearestColor(hDC, m_crAccidentalColor) );

	long lAccidentalWidth;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( 20, &lAccidentalWidth );

	// Compute the length of the part, in grids
	long lMaxGridValue = CLOCKS_TO_GRID( mtPartLength - 1, m_pPartRef->m_pDMPart);

	const long lTopAdjust = TOPADJUST;
	const long lBottomAdjust = BOTTOMADJUST;

	RECT rectNote;
	long lCurrentMeasure = LONG_MIN; // Initialize to LONG_MIN, since -1 is a valid measure
	CDirectMusicEventItem *pEvent = pDMNote;
	while( pEvent )
	{
		pDMNote = (CDirectMusicStyleNote *)pEvent;
		if ((pEvent->m_mtGridStart <= lMaxGridValue) &&
			(pDMNote->m_bMIDIValue <= nTopNote) &&
			(pDMNote->m_bMIDIValue >= nBottomNote) )
		{
			MUSIC_TIME mtAbsStartTime = m_pPartRef->m_pDMPart->AbsTime( pEvent );
			MUSIC_TIME mtStartTime = mtOffset + mtAbsStartTime;

			// BUGBUG: This will sometimes cause accidentals to be drawn that aren't actually visible.
			// If mtStartTime is just before lStartTime, and lStartTime is greater than a beat or so,
			// this check will succeed, even thought he accidental is not visible.

			// BUGBUG: If mtStartTime is just greater than lEndTime, this check will fail, even
			// though the accidental should be visible.  DavidY would like to have the accidental
			// visible in this case, though since it's easier not to I've left it as-is.
			if( (mtStartTime + lAccidentalWidth > lStartTime) &&
				(mtStartTime < lEndTime) )
			{
				long lMeasure;
				if( mtStartTime < 0 )
				{
					lMeasure = -1;
				}
				else
				{
					lMeasure = mtStartTime / m_lMeasureClocks;
				}

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
				if( m_pMIDIMgr->m_fDisplayingFlats )
				{
					iHybridPos = MIDIToHybridPos( pDMNote->m_bMIDIValue, aChromToScaleFlats);
					iAccidental = aChromToFlatAccidentals[pDMNote->m_bMIDIValue % 12];
				}
				// SHARPS
				else
				{
					iHybridPos = MIDIToHybridPos( pDMNote->m_bMIDIValue, aChromToScaleSharps);
					iAccidental = aChromToSharpAccidentals[pDMNote->m_bMIDIValue % 12];
				}

				// Check if we need to draw an accidental
				if( iAccidental != m_aiAccidentals[iHybridPos] )
				{
					// Get position to draw at
					GetHybridRect( &rectNote, iHybridPos, 127, mtStartTime, pDMNote->m_mtDuration );

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

					if( m_pMIDIMgr->m_fHasNotationStation )
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
					m_aiAccidentals[iHybridPos] = iAccidental;
				}
			}
			else if ( mtStartTime > lEndTime )
			{
				break;
			}
		}

		pEvent = pEvent->GetNext();

		while( pEvent && !(pEvent->m_dwVariation & m_dwVariations) )
		{
			pEvent = pEvent->GetNext();
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


void CPianoRollStrip::DrawHybridMaximizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset )
{
	// Set the note color
	COLORREF crNoteColor;
	if( fSelected )
	{
		crNoteColor = ::GetNearestColor(hDC, m_crSelectedNoteColor);
	}
	else
	{
		crNoteColor = ::GetNearestColor(hDC, m_crUnselectedNoteColor);
	}

	// Create the brush to draw the early (and late) notes with
	HBRUSH hbrushHatchVert = ::CreateHatchBrush( HS_VERTICAL, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 

	// Create pen for drawing the overlapping note lines
	HPEN hpenOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetNearestColor(hDC, GetSysColor(COLOR_WINDOW)) );
	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenOverlappingLine ));
	HBRUSH hbrushHatchOverlapping = ::CreateHatchBrush( HS_FDIAGONAL, ::GetNearestColor(hDC, COLOR_HATCH_OVERLAPPING) ); 

	// Create brush for drawing the notes
	HBRUSH hbrushHatchDiag = ::CreateHatchBrush( HS_DIAGCROSS, ::GetNearestColor(hDC, COLOR_EARLY_NOTES) ); 

	// Compute the length of the part, in grids
	long lMaxGridValue = CLOCKS_TO_GRID( mtPartLength - 1, m_pPartRef->m_pDMPart);

	RECT rectNote;
	CDirectMusicEventItem* pEvent = pDMNote;

	// Seek to the first note that matches our selection criteria
	while( pEvent && (!(fSelected == pEvent->m_fSelected) ||
					  !(pEvent->m_dwVariation & m_dwVariations) ) )
	{
		pEvent = pEvent->GetNext();
	}

	// If we are asked to draw a time that is beyond the end of our part, and
	// the part is shorter than the Timeline, then we should loop
	const bool fLoopingPart = m_pPartRef->m_pDMPart->GetClockLength() < m_mtTimelineLengthForGetNoteRect;

	int iRectListCounter = 0;

	while( pEvent )
	{
		if ((pEvent->m_mtGridStart <= lMaxGridValue) &&
			(((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue <= nTopNote) &&
			(((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue >= nBottomNote) )
		{
			// Get start and end times of note
			MUSIC_TIME mtAbsStartTime = m_pPartRef->m_pDMPart->AbsTime( pEvent );

			MUSIC_TIME mtStart = mtOffset + mtAbsStartTime;
			MUSIC_TIME mtEnd = mtStart + ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;

			// If note is visible, draw it
			if( (mtStart <= lEndTime) && (mtEnd >= lStartTime) )
			{
				int iHybridPos;
				// FLATS
				if( m_pMIDIMgr->m_fDisplayingFlats )
				{
					iHybridPos = MIDIToHybridPos( ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue, aChromToScaleFlats);
				}
				// SHARPS
				else
				{
					iHybridPos = MIDIToHybridPos( ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue, aChromToScaleSharps);
				}

				// Get position to draw at
				GetHybridRect( &rectNote, iHybridPos, ((CDirectMusicStyleNote*)pEvent)->m_bVelocity, mtStart, ((CDirectMusicStyleNote*)pEvent)->m_mtDuration );

				// Offset the rect so we draw it correctly
				rectNote.left -= lXOffset;
				rectNote.right -= lXOffset;

				// Draw the note
				if( ( (mtAbsStartTime < 0) || (mtAbsStartTime >= mtPartLength) )
				&&	hbrushHatchVert )
				{
					// Early (or late)
					::FillRect( hDC, &rectNote, hbrushHatchVert );
				}
				else if( ((CDirectMusicStyleNote*)pEvent)->m_bInversionId && hbrushHatchDiag )
				{
					// has an inversion ID
					::FillRect( hDC, &rectNote, hbrushHatchDiag );
				}
				else
				{
					// Normal
					::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
				}

				if( !fLoopingPart )
				{
					if( iRectListCounter == 10 )
					{
						iRectListCounter = 0;
						m_aNoteRectList[0].RemoveBefore( rectNote );
					}
					else
					{
						iRectListCounter++;
					}
				}

				DrawOverlapping( hDC, 0, &rectNote, hbrushHatchOverlapping, crNoteColor );
			}
			else if( mtStart > lEndTime )
			{
				// No more notes to display - exit early
				break;
			}
		}

		pEvent = pEvent->GetNext();

		while( pEvent && (!(fSelected == pEvent->m_fSelected) ||
						  !(pEvent->m_dwVariation & m_dwVariations) ) )
		{
			pEvent = pEvent->GetNext();
		}
	}

	// Re-select the old pen
	::SelectObject( hDC, hpenOld );

	// Delete all pens and brushes we created
	if( hpenOverlappingLine )
	{
		::DeleteObject( hpenOverlappingLine );
	}
	if( hbrushHatchOverlapping )
	{
		::DeleteObject( hbrushHatchOverlapping );
	}
	if( hbrushHatchDiag )
	{
		::DeleteObject( hbrushHatchDiag );
	}
	if( hbrushHatchVert )
	{
		::DeleteObject( hbrushHatchVert );
	}
}

BYTE CPianoRollStrip::PositionToMIDIValue( long lYPos )
{
	lYPos = max( 0, lYPos, );
	if( m_fHybridNotation )
	{
		// Convert from ypos to a scale position (with B10 as 0)
		long lValue = 1 + ((2 * (lYPos + m_lMaxNoteHeight / 4)) / m_lMaxNoteHeight);

		lValue = min( 76, lValue );

		// Convert from a scale position to a MIDI value
		return BYTE(120 - (lValue / 7) * 12 + aScaleToChromNat[6 - (lValue % 7)] + m_pMIDIMgr->m_aiScalePattern[6 - (lValue % 7)]);
	}
	else
	{
		return BYTE(127 - min( 127, lYPos / m_lMaxNoteHeight ));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetTimelineHWnd

HWND CPianoRollStrip::GetTimelineHWnd()
{
	// Get the DC of our Strip
	if( m_pMIDIMgr->m_pTimeline )
	{
		IOleWindow *pIOleWindow;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void **)&pIOleWindow ) ) )
		{
			HWND hwnd = NULL;
			pIOleWindow->GetWindow( &hwnd );
			pIOleWindow->Release();
			return hwnd;
		}
	}
	
	return NULL;
}

void CPianoRollStrip::DrawOverlapping( HDC hDC, char cValue, const RECT *pRectNote, HBRUSH hbrushHatchOverlapping, COLORREF crNoteColor )
{
	RECT rectIntersect;
	POINT aPoint[4];
	CRectList *pRectList = &(m_aNoteRectList[cValue]);
	const RECT* pRect = pRectList->GetFirst();

	while (pRect)
	{
		// compute the intersection of the notes
		CRect rect;
		rect.IntersectRect( pRect, pRectNote);

		// draw it
		if( IntersectRect( &rectIntersect, pRect, pRectNote ) )
		{
			ASSERT((rectIntersect.left != rectIntersect.right) || (rectIntersect.top != rectIntersect.bottom));
			aPoint[0].x = rectIntersect.left;
			aPoint[0].y = rectIntersect.bottom;
			aPoint[1].x = rectIntersect.left;
			aPoint[1].y = rectIntersect.top;
			aPoint[2].x = rectIntersect.right;
			aPoint[2].y = rectIntersect.top;
			aPoint[3].x = rectIntersect.right;
			aPoint[3].y = rectIntersect.bottom;
			::Polyline( hDC, aPoint, 4 );

			// Fill the rect with another color
			rectIntersect.top++;
			rectIntersect.left++;
			if( (rectIntersect.top < rectIntersect.bottom) && (rectIntersect.left < rectIntersect.right) )
			{
				::SetBkColor( hDC, m_crOverlappingNoteColor );
				::FillRect( hDC, &rectIntersect, hbrushHatchOverlapping );
				::SetBkColor( hDC, crNoteColor );
			}
		}

		// Get the next overlapping note
		pRect = pRectList->GetNext();
	}
	pRectList->InsertRect( *pRectNote );
}

// stuff for music line drawing
#define MEASURE_LINE_PENSTYLE	PS_SOLID
#define MEASURE_LINE_WIDTH		2
#define MEASURE_LINE_COLOR		RGB(0,0,0)
#define BEAT_LINE_PENSTYLE		PS_SOLID
#define BEAT_LINE_WIDTH			1
#define BEAT_LINE_COLOR			RGB(51,51,153)
#define SUBBEAT_LINE_PENSTYLE	PS_SOLID
#define SUBBEAT_LINE_WIDTH		1
#define SUBBEAT_LINE_COLOR		RGB(204,204,255)

HRESULT CPianoRollStrip::DrawVerticalines( HDC hdc, long lXOffset )
{
	// Create the pens
	HPEN hPenMeasureLine;
	hPenMeasureLine = ::CreatePen( MEASURE_LINE_PENSTYLE, MEASURE_LINE_WIDTH, MEASURE_LINE_COLOR );
	if( hPenMeasureLine == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HPEN hPenBeatLine = ::CreatePen( BEAT_LINE_PENSTYLE, BEAT_LINE_WIDTH, BEAT_LINE_COLOR );
	if( hPenBeatLine == NULL )
	{
		::DeleteObject( hPenMeasureLine );
		return E_OUTOFMEMORY;
	}

	HPEN hPenSubBeatLine = ::CreatePen( SUBBEAT_LINE_PENSTYLE, SUBBEAT_LINE_WIDTH, SUBBEAT_LINE_COLOR );
	if( hPenSubBeatLine == NULL )
	{
		::DeleteObject( hPenMeasureLine );
		::DeleteObject( hPenBeatLine );
		return E_OUTOFMEMORY;
	}

	long lPosition = 0; // integer position

	// Save the current pen and switch to the Measure Line pen
	const HPEN hPenOld = static_cast<HPEN>( ::SelectObject( hdc, hPenMeasureLine ) );

	const CDirectMusicPart* pDMPart = m_pPartRef->m_pDMPart;
	ASSERT( pDMPart != NULL );

	const DirectMusicTimeSig TimeSig = pDMPart->m_TimeSignature;
	BYTE bBeat;

	const MUSIC_TIME mtMeasureLength = m_lMeasureClocks;;

	// Get the clipping rectangle
	RECT rectClip;
	::GetClipBox( hdc, &rectClip );

	// Find the time of the first visible pixel
	MUSIC_TIME mtLeft;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &mtLeft );

	// Find the time of the last visible pixel
	MUSIC_TIME mtRight;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &mtRight );

	// Get the clock length of the pattern
	const MUSIC_TIME mtPatternLength = m_pPartRef->m_pPattern->CalcLength();
	MUSIC_TIME mtPatternStart = max( 0, mtPatternLength * (mtLeft / mtPatternLength) );
	MUSIC_TIME mtPatternEnd = mtPatternStart + mtPatternLength;

	// Get the length of the timeline
	MUSIC_TIME mtTimelineLength = mtPatternLength;
	{
		VARIANT varLength;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
		{
			mtTimelineLength = V_I4( &varLength );
		}
	}

	// Set mtTSCur to the start of the first measure visible
	MUSIC_TIME mtTSCur;
	if( mtLeft < 0 )
	{
		mtTSCur = -mtMeasureLength;
	}
	else
	{
		mtTSCur = mtPatternStart + (mtMeasureLength * ((mtLeft - mtPatternStart) / mtMeasureLength));
	}

	while( mtTSCur < mtRight )
	{
		// If we're asked to draw a pattern that ends on or after the timeline does
		if( mtPatternEnd >= mtTimelineLength )
		{
			// Change the pattern's end time to either the last time we're asked to draw
			// or the end of the pattern, whichever is later.
			// This ensures that any 'extension' measures are drawn correctly
			mtPatternEnd = max( mtPatternEnd, mtRight );
		}

		// Draw measure line
		if ( mtTSCur >= mtLeft )
		{
			::SelectObject( hdc, hPenMeasureLine );
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtTSCur, &lPosition );
			::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
			::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );
		}

		// Save the start of the measure
		MUSIC_TIME mtMeasureStart = mtTSCur;

		// Draw beats
		bBeat = 1; // Skip beat 0, since that is the measure line
		::SelectObject( hdc, hPenBeatLine );
		while ( bBeat < TimeSig.m_bBeatsPerMeasure )
		{
			mtTSCur += pDMPart->m_mtClocksPerBeat;

			// Draw beat line
			if( mtTSCur >= mtLeft )
			{
				// Check if we've gone past the end of the pattern
				if( mtTSCur < mtPatternEnd )
				{
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtTSCur, &lPosition );
					::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
					::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );
				}
				else
				{
					break;
				}
			}

			bBeat++;
		}

		// Draw Grids
		bBeat = 0;
		mtTSCur = mtMeasureStart;
		::SelectObject( hdc, hPenSubBeatLine );
		while ( bBeat < TimeSig.m_bBeatsPerMeasure )
		{
			// Draw Grids

			// Save next beat position
			MUSIC_TIME mtNextBeat = mtTSCur + pDMPart->m_mtClocksPerBeat;

			BYTE bGrid = 1; // Skip grid 0, since that is the beat line
			mtTSCur += pDMPart->m_mtClocksPerGrid;

			// Draw grid lines
			while ( bGrid < TimeSig.m_wGridsPerBeat )
			{
				if( mtTSCur >= mtLeft )
				{
					// Check if we've gone past the end of the pattern
					if( mtTSCur < mtPatternEnd )
					{
						m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtTSCur, &lPosition );
						::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
						::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );
					}
					else
					{
						break;
					}
				}

				bGrid++;
				mtTSCur += pDMPart->m_mtClocksPerGrid;
			}

			mtTSCur = mtNextBeat;

			// Check if we've gone past the end of the pattern
			if( mtTSCur >= mtPatternEnd )
			{
				break;
			}

			bBeat++;
		}

		// Check if we've gone past the end of the pattern
		if( mtTSCur >= mtPatternEnd )
		{
			// Yes - reset mtTSCur to the start of the next pattern
			mtTSCur = mtPatternEnd;
			mtPatternEnd += mtPatternLength;
		}
	}

	// Restore the previous pen
	::SelectObject( hdc, hPenOld );

	::DeleteObject( hPenMeasureLine );
	::DeleteObject( hPenBeatLine );
	::DeleteObject( hPenSubBeatLine );

	return S_OK;
}

HRESULT CPianoRollStrip::ReadPRCDFromStream( IStream *pIStream, PianoRollClipboardData *pPRCD )
{
	// Read the PianoRollClipboardData structure
	ULONG cbRead;

	HRESULT hr = pIStream->Read( pPRCD, sizeof(PianoRollClipboardData), &cbRead);
	if( FAILED(hr) || cbRead != sizeof(PianoRollClipboardData))
	{
		return E_UNEXPECTED;
	}

	if( !ValidPasteVariations( pPRCD->dwVariations ) )
	{
		AfxMessageBox( IDS_ERROR_VAR_MISMATCH );
		return E_UNEXPECTED;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetFirstVisibleNote

CDirectMusicStyleNote* CPianoRollStrip::GetFirstVisibleNote( long lStartTime, long lEndTime, int nTopNote, int nBottomNote )
{
	CDirectMusicEventItem *pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
	while( pEvent )
	{
		// Check vertical range, then horizontal range
		if( (pEvent->m_dwVariation & m_dwVariations) &&
			(((CDirectMusicStyleNote *)pEvent)->m_bMIDIValue <= nTopNote) &&
			(((CDirectMusicStyleNote *)pEvent)->m_bMIDIValue >= nBottomNote) )
		{
			// calculate the start time of the note
			const long lNoteStartTime = m_pPartRef->m_pDMPart->AbsTime( pEvent );

			// Check if the note starts before lEndTime
			if( lNoteStartTime <= lEndTime )
			{
				// Check if the note ends after lStartTime
				if( lNoteStartTime + ((CDirectMusicStyleNote *)pEvent)->m_mtDuration >= lStartTime )
				{
					// Yes - return the note
					return (CDirectMusicStyleNote *)pEvent;
				}
			}
		}
		pEvent = pEvent->GetNext();
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::InitializeScaleAccidentals

void CPianoRollStrip::InitializeScaleAccidentals( void )
{
	for( int i=0; i < 10; i++ )
	{
		memcpy( &(m_aiAccidentals[i * 7]), m_pMIDIMgr->m_aiScalePattern, sizeof(int) * 7);
	}

	memcpy( &(m_aiAccidentals[70]), m_pMIDIMgr->m_aiScalePattern, sizeof(int) * 5);
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetFirstNote

CDirectMusicStyleNote* CPianoRollStrip::GetFirstNote( void ) const
{
	if( m_pPartRef && m_pPartRef->m_pDMPart )
	{
		return m_pPartRef->m_pDMPart->GetFirstNote( m_dwVariations );
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetFirstCurve

CDirectMusicStyleCurve* CPianoRollStrip::GetFirstCurve( void ) const
{
	if( m_pPartRef && m_pPartRef->m_pDMPart )
	{
		return m_pPartRef->m_pDMPart->GetFirstCurve( m_dwVariations );
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetFirstMarker

CDirectMusicStyleMarker* CPianoRollStrip::GetFirstMarker( void ) const
{
	if( m_pPartRef && m_pPartRef->m_pDMPart )
	{
		return m_pPartRef->m_pDMPart->GetFirstMarker( m_dwVariations );
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetLastNote

CDirectMusicStyleNote* CPianoRollStrip::GetLastNote( void ) const
{
	if( m_pPartRef && m_pPartRef->m_pDMPart )
	{
		return m_pPartRef->m_pDMPart->GetLastNote( m_dwVariations );
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DrawSymbol

void CPianoRollStrip::DrawSymbol( HDC hDC, const TCHAR *pstrText, int iHybridPos, long lRightPos, long lTopAdjust, long lBottomAdjust )
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
// CPianoRollStrip::OnTimer

void CPianoRollStrip::OnTimer( void )
{
	POINT point;
	if( ::GetCursorPos( &point ) && SUCCEEDED(m_pMIDIMgr->m_pTimeline->ScreenToStripPosition((IDMUSProdStrip *)this, &point)) )
	{
		// Subtract off the Variation button bar
		point.y -= VARIATION_BUTTON_HEIGHT * 2;

		if( (m_MouseMode == PRS_MM_ACTIVEMOVE)
		||	(m_MouseMode == PRS_MM_ACTIVERESIZE_START)
		||	(m_MouseMode == PRS_MM_ACTIVERESIZE_END) )
		{
			OnMouseMove( point.x, point.y );
		}

		// Get the height of the strip
		VARIANT var;
		long lHeight = 0;
		if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_HEIGHT, &var)))
		{
			lHeight = V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2;
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

		if ((point.y < m_lVerticalScroll) && (m_lVerticalScroll > 0))
		{
			// Scroll up
			
			SetNewVerticalScroll( max( 0, m_lVerticalScroll - SCROLL_VERT_AMOUNT * ((SCROLL_VERT_RANGE + m_lVerticalScroll - point.y) / SCROLL_VERT_RANGE) ) );
		}
		else if ( (lHeight > 0) && (point.y > m_lVerticalScroll + lHeight) &&
				  (m_lVerticalScroll < lMaxVScroll) )
		{
			// Scroll down
			SetNewVerticalScroll( min( lMaxVScroll, m_lVerticalScroll + SCROLL_VERT_AMOUNT * ((SCROLL_VERT_RANGE + point.y - m_lVerticalScroll - lHeight) / SCROLL_VERT_RANGE) ) );
		}

		// Update horizontal scroll, if necessary
		long lHScroll = 0;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			lHScroll = V_I4(&var);
		}

		// Check for need to scroll left
		if( (lHScroll > 0) && (point.x < lHScroll) )
		{
			// Scroll left
			var.vt = VT_I4;
			V_I4(&var) = max( lHScroll - SCROLL_HORIZ_AMOUNT * ((SCROLL_HORIZ_RANGE + lHScroll - point.x) / SCROLL_HORIZ_RANGE), 0 );
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

			// No more checks necessary - return
			return;
		}

		// Get rectangle defining strip position
		var.vt = VT_BYREF;
		RECT rectStrip;
		V_BYREF(&var) = &rectStrip;
		if ( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
		{
			// Compute the right side of the display
			long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

			// Compute the maximum scroll position
			long lMaxHScroll = 0;
			if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var ) ) )
			{
				lMaxHScroll = V_I4(&var) - rectStrip.right - rectStrip.left;

				// Check for need to scroll right
				if( (lHScroll < lMaxHScroll) && (point.x > lMaxScreenPos) )
				{
					// Scroll right
					var.vt = VT_I4;
					V_I4(&var) = min( lHScroll + SCROLL_HORIZ_AMOUNT * ((SCROLL_HORIZ_RANGE + point.x - lMaxScreenPos) / SCROLL_HORIZ_RANGE), lMaxHScroll);
					m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

					// No more checks necessary - return
					return;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::KillTimer

void CPianoRollStrip::KillTimer( void )
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
// CPianoRollStrip::EnableTimer

void CPianoRollStrip::EnableTimer( void )
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
// CPianoRollStrip::KillVariationTimer

void CPianoRollStrip::KillVariationTimer( void )
{
	if( m_fVariationsTimerActive )
	{
		::KillTimer( m_prScrollBar.GetSafeHwnd(), 1 );
		m_fVariationsTimerActive = false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::EnableVariationTimer

void CPianoRollStrip::EnableVariationTimer( void )
{
	if( !m_fVariationsTimerActive )
	{
		HWND hwnd = GetTimelineHWnd();
		if( hwnd )
		{
			// If the scroll bar exists
			if( m_prScrollBar.GetSafeHwnd() )
			{
				// Update the variations every 400ms
				if( ::SetTimer( m_prScrollBar.GetSafeHwnd(), 1, 400, NULL ) )
				{
					m_fVariationsTimerActive = true;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetFirstSelectedNote

CDirectMusicStyleNote* CPianoRollStrip::GetFirstSelectedNote( void )
{
	if( m_pPartRef && m_pPartRef->m_pDMPart )
	{
		// Get the first note in this variation
		CDirectMusicEventItem *pNote = m_pPartRef->m_pDMPart->GetFirstNote( m_dwVariations );

		// While we have a note, and either the note is unselected, or it doesn't belong to our variations
		while( pNote && (!pNote->m_fSelected || !(pNote->m_dwVariation & m_dwVariations)) )
		{
			// Get the next note
			pNote = pNote->GetNext();
		}

		// Return what we found
		return (CDirectMusicStyleNote*)pNote;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::BumpTimeCursor

void TimeToPatternPartIndex( MUSIC_TIME mtTime, MUSIC_TIME mtPatternLength, MUSIC_TIME mtPartLength, long &lPatternIndex, long &lPartIndex )
{
	lPatternIndex = mtTime / mtPatternLength;
	lPartIndex = (mtTime - lPatternIndex * mtPatternLength) / mtPartLength;
}

void CPianoRollStrip::BumpTimeCursor( BOOL fBumpRight, SNAPTO stSnapSetting )
{
	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return;
	}

	// Get the time of the time cursor
	long lTime;
	if (FAILED(m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return;
	}

	VARIANT var;
	if( FAILED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var ) ) )
	{
		return;
	}
	const long lTimelineLength = V_I4(&var) - 1;

	// Compute the length of the pattern
	const long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

	// Compute the length of the part
	const long lPartLength = m_pPartRef->m_pDMPart->GetClockLength();

	// Get the start of the last pattern repeat in the segment
	//const long lLastPatternStart = lPatternLength * (lTimelineLength / lPatternLength);

	// Get the start of the last part repeat in the segment
	//const long lLastPartStart = lLastPatternStart + lPartLength * ((lTimelineLength - lLastPatternStart) / lPartLength);

	// Get the length of the last part repeat in the segment
	//const long lLastPartLength = lTimelineLength - lLastPartStart;

	long lOldPatternIndex, lOldPartIndex;
	TimeToPatternPartIndex( lTime, lPatternLength, lPartLength, lOldPatternIndex, lOldPartIndex );
	const MUSIC_TIME mtOldPartStart = lOldPatternIndex * lPatternLength + lOldPartIndex * lPartLength;
	MUSIC_TIME mtOffset = lTime - mtOldPartStart;

	// Get the amount to move the note cursor and time cursor by
	long lAmountToMove;
	switch( stSnapSetting )
	{
	default:
		ASSERT(FALSE);
	case SNAP_GRID:
		// Snap to Grid
		lAmountToMove = m_lGridClocks;
		mtOffset -= GRID_TO_CLOCKS( CLOCKS_TO_GRID( mtOffset, m_pPartRef->m_pDMPart ), m_pPartRef->m_pDMPart);
		break;
	case SNAP_BEAT:
		// Snap to Beat
		lAmountToMove = m_lBeatClocks;
		mtOffset -= (mtOffset / m_lBeatClocks) * m_lBeatClocks;
		break;
	case SNAP_BAR:
		// Snap to Bar
		lAmountToMove = m_lMeasureClocks;
		mtOffset -= (mtOffset / m_lMeasureClocks) * m_lMeasureClocks;
		break;
	case SNAP_NONE:
		// Snap to None
		lAmountToMove = 1;
		mtOffset = 0;
		break;
	}

	if( !fBumpRight )
	{
		lAmountToMove *= -1;
	}

	long lNewPatternIndex, lNewPartIndex;
	TimeToPatternPartIndex( lTime + lAmountToMove, lPatternLength, lPartLength, lNewPatternIndex, lNewPartIndex );

	if( (lOldPatternIndex != lNewPatternIndex)
	||	(lOldPartIndex != lNewPartIndex) )
	{
		// Don't move before the start of time
		if( fBumpRight
		||	(lOldPatternIndex != 0)
		||	(lOldPartIndex != 0) )
		{
			const MUSIC_TIME mtOldPartStart = lOldPatternIndex * lPatternLength + lOldPartIndex * lPartLength;
			MUSIC_TIME mtNewPartLength;
			MUSIC_TIME mtNewPartStart;

			if( fBumpRight )
			{
				// Get the length of the old part
				mtNewPartLength = min( (lOldPatternIndex + 1) * lPatternLength - mtOldPartStart, lPartLength );

				// Get the index of the next part repeat
				TimeToPatternPartIndex( mtOldPartStart + mtNewPartLength, lPatternLength, lPartLength, lNewPatternIndex, lNewPartIndex );

				// Compute the start of the new part repeat
				mtNewPartStart = lNewPatternIndex * lPatternLength + lNewPartIndex * lPartLength;

				const MUSIC_TIME mtOldPartLength = mtNewPartLength;

				MUSIC_TIME mtBase;
				switch( stSnapSetting )
				{
				default:
					ASSERT(FALSE);
				case SNAP_GRID:
					// Snap to Grid
					mtBase = GRID_TO_CLOCKS( CLOCKS_TO_GRID( lTime - mtOldPartStart, m_pPartRef->m_pDMPart ), m_pPartRef->m_pDMPart);
					break;
				case SNAP_BEAT:
					// Snap to Beat
					mtBase = (lTime - mtOldPartStart) / m_lBeatClocks;
					mtBase *= m_lBeatClocks;
					break;
				case SNAP_BAR:
					// Snap to Bar
					mtBase = (lTime - mtOldPartStart) / m_lMeasureClocks;
					mtBase *= m_lMeasureClocks;
					break;
				case SNAP_NONE:
					// Snap to None
					mtBase = lTime - mtOldPartStart;
					break;
				}

				mtBase += lAmountToMove;

				TimeToPatternPartIndex( mtBase + mtOldPartStart, lPatternLength, lPartLength, lNewPatternIndex, lNewPartIndex );

				if( (lOldPatternIndex != lNewPatternIndex)
				||	(lOldPartIndex != lNewPartIndex) )
				{
					// We start at the start of the new part
					lTime = 0;
					// Compute the length of the new part repeat
					mtNewPartLength = min( (lNewPatternIndex + 1) * lPatternLength - mtNewPartStart, lPartLength ) - 1;
				}
				else
				{
					lTime = min( lAmountToMove + lTime, mtOldPartLength + mtOldPartStart - 1 );
					mtNewPartStart = 0;
					mtNewPartLength = LONG_MAX;
					mtOffset = 0;
				}
			}
			else
			{
				TimeToPatternPartIndex( mtOldPartStart - 1, lPatternLength, lPartLength, lNewPatternIndex, lNewPartIndex );

				mtNewPartStart = lNewPatternIndex * lPatternLength + lNewPartIndex * lPartLength;
				mtNewPartLength = mtOldPartStart - mtNewPartStart - 1;

				switch( stSnapSetting )
				{
				default:
					ASSERT(FALSE);
				case SNAP_GRID:
					// Snap to Grid
					lTime = GRID_TO_CLOCKS( CLOCKS_TO_GRID( mtNewPartLength, m_pPartRef->m_pDMPart ), m_pPartRef->m_pDMPart);
					break;
				case SNAP_BEAT:
					// Snap to Beat
					lTime = mtNewPartLength / m_lBeatClocks;
					lTime *= m_lBeatClocks;
					break;
				case SNAP_BAR:
					// Snap to Bar
					lTime = mtNewPartLength / m_lMeasureClocks;
					lTime *= m_lMeasureClocks;
					break;
				case SNAP_NONE:
					// Snap to None
					lTime = mtNewPartLength;
					break;
				}
			}

			lTime += mtNewPartStart + min( mtNewPartLength - lTime, mtOffset );
		}
	}
	else
	{
		lTime = lTime + lAmountToMove;
	}

	/*
	// If the Time cursor would go beyond the end of the pattern, move the cursor back to the beginning
	if( fBumpRight && (lTime + lSnapAmount >= lPatternLength) )
	{
		ASSERT( lSnapAmount > 0 );
		lTime %= lSnapAmount;
		m_lInsertTime = 0;

		// Force the Timeline to scroll all the way to the beginning
		m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, 0 );
	}
	// If the Time cursor would go below 0, move the cursor to the end
	else if( !fBumpRight && (lTime < lSnapAmount) && (lTime >= 0) )
	{
		// Snap to nearest value from lPatternLength - 1;
		m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lPatternLength - 1 );

		// This sets m_lInsertTime to the correct value
		UpdateNoteCursorTime();

		// Now reset the time cursor to the correct value
		m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS,
			min( lPatternLength - 1, (lPatternLength / m_lMeasureClocks) * m_lMeasureClocks + lTime + m_lInsertTime ) );

		// Redraw
		InvalidatePianoRoll();
		return;
	}
	else
	{
		if( !fBumpRight )
		{
			lSnapAmount *= -1;
		}

		m_lInsertTime += lSnapAmount;
		lTime += lSnapAmount;

		// If the cursor is now negative
		if( m_lInsertTime < 0 )
		{
			// and was positive
			if( m_lInsertTime - lSnapAmount >= 0 )
			{
				// Wrap it forward by the length of one part
				m_lInsertTime += m_pPartRef->m_pDMPart->GetClockLength();
				lTime += m_pPartRef->m_pDMPart->GetClockLength();
			}
			else // and was negative
			{
				// And is more than a measure in the past
				if( m_lInsertTime < -m_lMeasureClocks )
				{
					// Wrap it forward by one measure
					m_lInsertTime += m_lMeasureClocks;
					lTime += m_lMeasureClocks;
				}
			}
		}
		else
		{
			m_lInsertTime %= m_pPartRef->m_pDMPart->GetClockLength();
		}
	}
	*/

	// Only update the cursor if the new time is valid
	if( (lTime < lTimelineLength)
	&&	(lTime >= 0) )
	{
		m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, lTime );
		UpdateNoteCursorTime();
		InvalidatePianoRoll();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DrawNoteInsertionMark

void CPianoRollStrip::DrawNoteInsertionMark( HDC hDC, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset )
{
	// Ensure we don't draw a note insertion marker after the end of the timeline
	mtEndTime = min( mtEndTime, m_mtTimelineLengthForGetNoteRect );

	RECT rectMark;
	ComputeNoteMarkerVerticalRect( rectMark, m_fHybridNotation, m_pMIDIMgr->m_fDisplayingFlats, m_lMaxNoteHeight, m_lInsertVal );

	// Get the snap amount
	long lDuration = GetSnapAmount();

	// Compute the mark's length
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lDuration, &rectMark.right );
	lDuration = max( rectMark.right, 5 ); // Minimum of 5 pixels wide

	// Compute the length of the part, in ticks
	const long lPartClockLength = m_pPartRef->m_pDMPart->GetClockLength();

	// Compute the length of the pattenr, in ticks
	const long lPatternClockLength = m_pPartRef->m_pPattern->CalcLength();

	// Compute the number of times a part repeats in the pattern
	const long lMaxPartRepeat = (lPatternClockLength - 1) / lPartClockLength;

	// Compute the length of the last part repeat in the pattern
	const long lLastPartLength = lPatternClockLength - lMaxPartRepeat * lPartClockLength;

	// Set up the accidental font, if necessary
	if( m_fHybridNotation )
	{
		// Compute the hybrid position
		int iHybridPos;
		if( m_pMIDIMgr->m_fDisplayingFlats )
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
		if( m_pMIDIMgr->m_fDisplayingFlats )
		{
			iAccidental = aChromToFlatAccidentals[m_lInsertVal % 12];
		}
		// SHARPS
		else
		{
			iAccidental = aChromToSharpAccidentals[m_lInsertVal % 12];
		}

		if( iAccidental != m_pMIDIMgr->m_aiScalePattern[iHybridPos % 7] )
		{
			const int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );
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

				// Iterate through all pattern repeats
				for( int j = max( 0, mtStartTime / lPatternClockLength ); j <= mtEndTime / lPatternClockLength; j++ )
				{
					// Iterate through all part repeats
					for( int i = 0; i <= lMaxPartRepeat; i++ )
					{
						const long lOffset = j * lPatternClockLength + i * lPartClockLength;

						// If in the last part repeat, and the cursor would start beyond the end of this part
						if( (i == lMaxPartRepeat)
						&&	(m_lInsertTime >= lLastPartLength) )
						{
							// Skip drawing this copy
							continue;
						}

						if( lOffset < mtEndTime )
						{
							// Compute the mark's start position
							m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_lInsertTime + lOffset, &rectMark.left );

							DrawSymbol( hDC, strSymbol, iHybridPos, rectMark.left - 2 - lXOffset, lTopAdjust, lBottomAdjust );
						}
					}
				}

				::SelectObject( hDC, hNewOldFont );
				::DeleteObject( hfontMusic );
			}

			// Reset the color and mode
			::SetTextColor( hDC, oldColor );
			::SetBkMode( hDC, nOldBkMode );
		}
	}


	int nCaps = ::GetDeviceCaps( hDC, RASTERCAPS );
	if( (nCaps & RC_BITBLT) && !(nCaps & RC_PALETTE) )
	{
		// Device support BitBlt and is not palette-based - draw transparent box
		// Calculate the source rectangle
		RECT rectNewRect;
		rectNewRect.left = 0;
		rectNewRect.top = 0;
		rectNewRect.right = lDuration;// rectMark.left/right are invalid - lDuration is the width 
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

		// Iterate through all pattern repeats
		for( int j = max( 0, mtStartTime / lPatternClockLength ); j <= mtEndTime / lPatternClockLength; j++ )
		{
			// Iterate through all part repeats
			for( int i = 0; i <= lMaxPartRepeat; i++ )
			{
				const long lOffset = j * lPatternClockLength + i * lPartClockLength;

				// If in the last part repeat, and the cursor would start beyond the end of this part
				if( (i == lMaxPartRepeat)
				&&	(m_lInsertTime >= lLastPartLength) )
				{
					// Skip drawing this copy
					continue;
				}

				if( lOffset < mtEndTime )
				{
					// Compute the mark's start and end position
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_lInsertTime + lOffset, &rectMark.left );
					rectMark.left -= lXOffset;
					rectMark.right = lDuration + rectMark.left;

					// Draw it
					::BitBlt( hDC, rectMark.left, rectMark.top, rectNewRect.right, rectNewRect.bottom, hNewDC, 0, 0, SRCAND);
				}
			}
		}

		// Clean up
		::SelectObject( hNewDC, hOldBitMap );
		::DeleteDC( hNewDC );
		::DeleteObject( hBitMap );
	}
	else
	{
		// Device doesn't support BitBlt or is palette-based -  draw black box
		// Save the old background color
		const COLORREF crOldBkColor = ::SetBkColor( hDC, ::GetNearestColor(hDC, 0) );

		// Iterate through all pattern repeats
		for( int j = max( 0, mtStartTime / lPatternClockLength ); j <= mtEndTime / lPatternClockLength; j++ )
		{
			// Iterate through all part repeats
			for( int i = 0; i <= lMaxPartRepeat; i++ )
			{
				const long lOffset = j * lPatternClockLength + i * lPartClockLength;
				if( lOffset < mtEndTime )
				{
					// Compute the mark's start and end position
					m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_lInsertTime + lOffset, &rectMark.left );
					rectMark.left -= lXOffset;
					rectMark.right = lDuration + rectMark.left;

					// Draw it
					::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectMark, NULL, 0, NULL);
				}
			}
		}

		// Reset the old background color
		::SetBkColor( hDC, crOldBkColor );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UpdateNoteCursorTime

void CPianoRollStrip::UpdateNoteCursorTime( void )
{
	ASSERT( ValidPartRefPtr() );
	if( !ValidPartRefPtr() )
	{
		return;
	}

	long lTime;
	if (FAILED(m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return;
	}

	if( lTime > 0 )
	{
		lTime %= m_pMIDIMgr->m_pDMPattern->CalcLength();
		lTime %= m_pPartRef->m_pDMPart->GetClockLength();
	}

	switch( GetSnapToBoundary( ) )
	{
	case SNAP_GRID:
		// Snap to Grid
		if( lTime >= 0 )
		{
			lTime = CLOCKS_TO_GRID( lTime, m_pPartRef->m_pDMPart );
		}
		else
		{
			lTime = CLOCKS_TO_GRID( lTime, m_pPartRef->m_pDMPart ) - 1;
		}
		lTime = GRID_TO_CLOCKS( lTime, m_pPartRef->m_pDMPart );
		break;
	case SNAP_BEAT:
		// Snap to Beat
		if( lTime >= 0 )
		{
			lTime /= m_lBeatClocks;
		}
		else
		{
			lTime /= m_lBeatClocks;
			lTime -= 1;
		}
		lTime *= m_lBeatClocks;
		break;
	case SNAP_BAR:
		// Snap to Bar
		if( lTime >= 0 )
		{
			lTime /= m_lMeasureClocks;
		}
		else
		{
			lTime /= m_lMeasureClocks;
			lTime -= 1;
		}
		lTime *= m_lMeasureClocks;
		break;
	case SNAP_NONE:
		// Snap to None
		// No need to update lTime, nothing changed
		break;
	default:
		ASSERT(FALSE);
		return;
	}

	if( lTime != m_lInsertTime )
	{
		m_lInsertTime = lTime;
		InvalidatePianoRoll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetSnapToBoundary

SNAPTO CPianoRollStrip::GetSnapToBoundary( void )
{
	SNAPTO tlSnapTo = SNAP_NONE;

	VARIANT var;
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
	{
		switch( (DMUSPROD_TIMELINE_SNAP_TO) V_I4( &var ) )
		{
		
			case DMUSPROD_TIMELINE_SNAP_NONE:
				tlSnapTo = SNAP_NONE;
				break;
			
			case DMUSPROD_TIMELINE_SNAP_GRID:
				tlSnapTo = SNAP_GRID;
				break;
			
			case DMUSPROD_TIMELINE_SNAP_BEAT:
				tlSnapTo = SNAP_BEAT;
				break;

			case DMUSPROD_TIMELINE_SNAP_BAR:
				tlSnapTo = SNAP_BAR;
				break;
			
			default:
				ASSERT(FALSE);
				break;
		}
	}

	return tlSnapTo;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetSnapAmount

long CPianoRollStrip::GetSnapAmount( void )
{
	switch( GetSnapToBoundary( ) )
	{
	case SNAP_GRID:
		// Snap to Grid
		return m_lGridClocks;
	case SNAP_BEAT:
		// Snap to Beat
		return m_lBeatClocks;
	case SNAP_BAR:
		// Snap to Bar
		return m_lMeasureClocks;
	case SNAP_NONE:
		// Snap to None
		return 1;
	default:
		ASSERT(FALSE);
		break;
	}

	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetAccidentalFont

HFONT CPianoRollStrip::GetAccidentalFont( void )
{
	// Create the font to draw the sharps and flats with
	LOGFONT lf;
	memset( &lf, 0 , sizeof(LOGFONT));
	if( m_pMIDIMgr->m_fHasNotationStation )
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
// CPianoRollStrip::OnApp

LRESULT CPianoRollStrip::OnApp( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( wParam == WM_APP_BUMPTIMECURSOR )
	{
		SNAPTO snapTo = GetSnapToBoundary();
		if( SNAP_NONE == snapTo )
		{
			snapTo = SNAP_GRID;
		}
		BumpTimeCursor( TRUE, snapTo );
	}
	else if( wParam == WM_APP_INVALIDATEPIANOROLL )
	{
		Sleep( 50 );
		MSG msg;
		while( 0 != PeekMessage( &msg, m_prScrollBar.m_hWnd, WM_APP, WM_APP, PM_REMOVE ) )
		{
			if( msg.wParam == 0 )
			{
				OnApp( 0, 0 );
			}
		}

		const long lOldVScroll = m_lVerticalScroll;
		EnsureNoteCursorVisible();
		if( lOldVScroll == m_lVerticalScroll )
		{
			InvalidatePianoRoll();
		}
	}
	else if( wParam == WM_APP_INSTRUMENTMENU )
	{
		// Pointer to interface from which DLS region text can be obtained
		IDMUSProdBandEdit8a* pIBandEdit;

		// Try to get the band edit interface
		pIBandEdit = GetBandEditInterface( m_pMIDIMgr->m_pIStyleNode, static_cast<IDMUSProdStripMgr *>(m_pMIDIMgr), m_pMIDIMgr->m_pTimeline, m_pMIDIMgr->m_dwGroupBits, m_pPartRef->m_dwPChannel );

		if( pIBandEdit )
		{
			pIBandEdit->DisplayInstrumentButton(m_pPartRef->m_dwPChannel, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
		pIDMUSProdBandMgrEdit = GetBandMgrEditInterface( m_pMIDIMgr->m_pIStyleNode, static_cast<IDMUSProdStripMgr *>(m_pMIDIMgr), m_pMIDIMgr->m_pTimeline, m_pMIDIMgr->m_dwGroupBits, m_pPartRef->m_dwPChannel );

		if( pIDMUSProdBandMgrEdit )
		{
			pIDMUSProdBandMgrEdit->DisplayEditBandButton(m_pPartRef->m_dwPChannel, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			pIDMUSProdBandMgrEdit->Release();
		}
		else if( m_pMIDIMgr->m_pDMProdSegmentNode )
		{
			// If no band manager, display our own menu
			HMENU hMenu, hMenuPopup;
			hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_BANDMENU));
			if (hMenu)
			{
				hMenuPopup = ::GetSubMenu( hMenu, 0 );
				if (hMenuPopup)
				{
					m_pMIDIMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (IDMUSProdStrip *)this, FALSE);
				}
				DestroyMenu(hMenu); // This will destroy the submenu as well.
			}
		}
		else
		{
			// Shouldn't happen - the Style should always have a default band
			ASSERT(FALSE);
		}
		
		m_fNewBandPressed = FALSE;
		InvalidateFunctionBar();
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ChangeNotationType

void CPianoRollStrip::ChangeNotationType( BOOL fHybridNotation, BOOL fUpdatePatternEditor )
{
	if( m_fHybridNotation == fHybridNotation )
	{
		return;
	}

	int nUndoString;

	if( fHybridNotation )
	{
		m_fHybridNotation = TRUE;

		// Update the range
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = 37;
		m_prScrollBar.SetScrollInfo( &si, TRUE );

		// Fix 21001: Don't change zoom level when changing notation type.
		//m_dblVerticalZoom *= 1.6;
		nUndoString = IDS_UNDO_DISP_HYBRID;
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
		m_prScrollBar.SetScrollInfo( &si, TRUE );

		// Fix 21001: Don't change zoom level when changing notation type.
		//m_dblVerticalZoom /= 1.6;
		nUndoString = IDS_UNDO_DISP_NORMAL;
	}

	VARIANT var;
	if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		int nHeightDiv2 = (V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2) / 2;
		int nMiddle = (m_lVerticalScroll + nHeightDiv2 ) / m_lMaxNoteHeight;
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

		SetNewVerticalScroll( nMiddle * m_lMaxNoteHeight - nHeightDiv2 );
		if( m_StripView == SV_NORMAL )
		{
			ComputeVScrollBar();
			m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}

		// If we're not a drum track, notify the other sequence strips so they change also.
		if( (m_pPartRef->m_dwPChannel & 0xF) != 9 )
		{
			m_pMIDIMgr->ChangeNotationType( m_fHybridNotation );
		}

		if( fUpdatePatternEditor )
		{
			// Let the object know about the changes
			// No need to update performance engine
			m_pMIDIMgr->m_fUpdateDirectMusic = FALSE;
			m_pMIDIMgr->UpdateOnDataChanged( nUndoString );

			// If we're not a drum track, notify the other patterns so they change also.
			if( (m_pPartRef->m_dwPChannel & 0xF) != 9 )
			{
				IDMUSProdNotifySink *pStyleSink;
				if( m_pMIDIMgr->m_pIStyleNode
				&&	SUCCEEDED( m_pMIDIMgr->m_pIStyleNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pStyleSink ) ) )
				{
					pStyleSink->OnUpdate( m_pMIDIMgr->m_pIStyleNode, STYLE_NotationTypeChange, &m_fHybridNotation );
					pStyleSink->Release();
				}
			}

			EnsureNoteCursorVisible();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ChangeZoom

void CPianoRollStrip::ChangeZoom( double dblVerticalZoom )
{
	if( m_dblVerticalZoom == dblVerticalZoom )
	{
		return;
	}

	m_dblVerticalZoom = dblVerticalZoom;

	m_pMIDIMgr->m_fDirty = TRUE;

	VARIANT var;
	if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		int nHeightDiv2 = (V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2) / 2;
		int nMiddle = (m_lVerticalScroll + nHeightDiv2 ) / m_lMaxNoteHeight;
		m_lMaxNoteHeight = long( MAX_NOTE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT );

		SetNewVerticalScroll( nMiddle * m_lMaxNoteHeight - nHeightDiv2 );
		if( m_StripView == SV_NORMAL )
		{
			ComputeVScrollBar();
			m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}

		EnsureNoteCursorVisible();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UpdateSelectionState

void CPianoRollStrip::UpdateSelectionState( void )
{
	if( m_pActiveDMNote && m_pActiveDMNote->m_fSelected )
	{
		// Set m_prsSelecting based on the number of selected notes
		int nSelected = m_pPartRef->m_pDMPart->GetNumSelected( ET_NOTE, m_dwVariations );
		if (nSelected == 1)
		{
			m_prsSelecting = PRS_SINGLE_SELECT;
		}
		else if (nSelected > 1)
		{
			m_prsSelecting = PRS_MULTIPLE_SELECT;
		}
		else if (nSelected == 0)
		{
			// May happen when editing the variations of a note, and the note
			// no longer belongs to any visible variations
			//ASSERT(FALSE);
			m_prsSelecting = PRS_NO_SELECT;
			m_pActiveDMNote = NULL;
		}
	}
	else
	{
		m_pActiveDMNote = NULL;

		int nNumSelected = 0;
		CDirectMusicEventItem* pDMEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
		while (pDMEvent)
		{
			if( (pDMEvent->m_fSelected) &&
				(pDMEvent->m_dwVariation & m_dwVariations))
			{
				nNumSelected++;
				if (!m_pActiveDMNote)
				{
					m_pActiveDMNote = (CDirectMusicStyleNote *)pDMEvent;
				}
				if (nNumSelected > 1)
				{
					m_prsSelecting = PRS_MULTIPLE_SELECT;
					m_pMIDIMgr->UpdateStatusBarDisplay();
					return;
				}
			}
			pDMEvent = pDMEvent->GetNext();
		}
		if (nNumSelected == 1)
		{
			m_prsSelecting = PRS_SINGLE_SELECT;
		}
		else
		{
			m_prsSelecting = PRS_NO_SELECT;
		}
	}
	m_pMIDIMgr->UpdateStatusBarDisplay();
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::EnsureNoteCursorVisible

void CPianoRollStrip::EnsureNoteCursorVisible( void )
{
	RECT rectMark;
	ComputeNoteMarkerVerticalRect( rectMark, m_fHybridNotation, m_pMIDIMgr->m_fDisplayingFlats, m_lMaxNoteHeight, m_lInsertVal );

	if( rectMark.bottom <= m_lVerticalScroll )
	{
		// Need to scroll up
		SetNewVerticalScroll( ((rectMark.top / m_lMaxNoteHeight) - 1) * m_lMaxNoteHeight );
	}
	else
	{
		VARIANT var;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) )
		&&	(rectMark.top > V_I4(&var) - VARIATION_BUTTON_HEIGHT * 2 + m_lVerticalScroll) )
		{
			// Need to scroll down
			SetNewVerticalScroll( (((rectMark.top - V_I4(&var) + VARIATION_BUTTON_HEIGHT * 2) / m_lMaxNoteHeight) + 2) * m_lMaxNoteHeight );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::FirstGutterSelectedPianoRollStrip

BOOL CPianoRollStrip::FirstGutterSelectedPianoRollStrip( void )
{
	IDMUSProdStrip *pStrip;
	DWORD dwEnum = 0;
	BOOL fFirstSelectedStrip = FALSE;
	BOOL fContinueEnum = TRUE;

	// Iterate through all the strips
	while( fContinueEnum && SUCCEEDED( m_pMIDIMgr->m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get the strip's CLSID
			CLSID clsid;
			VARIANT  varClsid;
			varClsid.vt = VT_BYREF;
			V_BYREF(&varClsid) = &clsid;

			if( SUCCEEDED ( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) )
			&&	::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				// This strip is a PianoRollStrip - check if it is this piano roll strip
				IDMUSProdStrip *pThisStrip;
				if( SUCCEEDED( QueryInterface( IID_IDMUSProdStrip, (void **)&pThisStrip ) ) )
				{
					if( pThisStrip == pStrip )
					{
						// We are the first selected strip - do the save to MIDI file operation
						fFirstSelectedStrip = TRUE;
					}
					pThisStrip->Release();
				}
				fContinueEnum = FALSE;
			}
		}
		pStrip->Release();
		dwEnum++;
	}

	return fFirstSelectedStrip;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::RefreshPropertyPage

void CPianoRollStrip::RefreshPropertyPage( void )
{
	if (m_pPropPageMgr != NULL)
	{
		// Refresh property page
		m_pPropPageMgr->RefreshData();

		// If the pattern tab exists, point it to m_pMIDIMgr->m_pDMPattern
		if( m_pPropPageMgr->m_pTabPatternPattern )
		{
			m_pPropPageMgr->m_pTabPatternPattern->SetPattern( m_pMIDIMgr->m_pDMPattern );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::DrawNotesHelper

void CPianoRollStrip::DrawNotesHelper( BOOL fSelected, PRS_DRAWTYPE prsDrawType, HDC hDC, CDirectMusicStyleNote *pDMNote, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fLoopingPart, DRAW_NOTE_FUNC DrawNoteFunc )
{
	// Clear out the list that keeps track of overlapping notes
	switch( prsDrawType )
	{
	case PRS_DT_MAXIMIZED:
		{
			for( int i=nBottomNote; i<=nTopNote; i++ )
			{
				m_aNoteRectList[i].RemoveAll();
			}
		}
		break;
	case PRS_DT_HYBRID:
		m_aNoteRectList[0].RemoveAll();
		break;
	case PRS_DT_ACCIDENTAL:
	case PRS_DT_MINIMIZED:
	default:
		break;
	}

	// Compute the length of the part, in ticks
	const long lPartClockLength = m_pPartRef->m_pDMPart->GetClockLength();

	// Save the old background color
	COLORREF crOldBkColor;

	// Set the note color
	if( fSelected )
	{
		crOldBkColor = ::SetBkColor( hDC, ::GetNearestColor(hDC, m_crSelectedNoteColor) );
	}
	else
	{
		crOldBkColor = ::SetBkColor( hDC, ::GetNearestColor(hDC, m_crUnselectedNoteColor) );
	}

	// Draw notes
	if( fLoopingPart )
	{
		// Compute the length of the pattern, in ticks
		const long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

		// Compute the number of times a part repeats in the pattern
		const long lMaxPartRepeat = (lPatternLength - 1) / lPartClockLength;

		// Compute the length of the last repeat of a part in a pattern
		MUSIC_TIME mtLastPartRepeatLength = lPatternLength % lPartClockLength;
		if( mtLastPartRepeatLength == 0 )
		{
			mtLastPartRepeatLength = lPartClockLength;
		}

		// A good test to make sure this code works right is to create a pattern of 3/8,
		// with a part of 10/4, then insert a note in the part to it starts at bar 2, beat 10
		// and another in bar 0, beat 1.

		// Set the last time to draw (different from mtEndTime, which may be later)
		// Add m_lMeasureClocks to mtEndTime since notes may be offset backwards by up to a measure
		const MUSIC_TIME mtLastTimeToDraw = min( m_mtTimelineLengthForGetNoteRect - 1, mtEndTime + m_lMeasureClocks);

		// Set the amount of time a note may be offset into the future
		// This is the actual length of the part, plus two measures (one for a note being a measure long
		// and one for setting the note to play in the measure after the part ends), minus the displayed
		// length of the part.
		const MUSIC_TIME mtMaxNoteOffset = max( 0, m_lMeasureClocks * (2 + m_pPartRef->m_pDMPart->m_wNbrMeasures) - lPatternLength);

		// Iterate through all pattern repeats
		for( int j = max( 0, (mtStartTime - mtMaxNoteOffset) / lPatternLength); j <= mtLastTimeToDraw / lPatternLength; j++ )
		{
			// Iterate through all part repeats
			for( int i = 0; i <= lMaxPartRepeat; i++ )
			{
				// Compute the start of this part repeat
				const MUSIC_TIME mtOffset = j * lPatternLength + i * lPartClockLength;

				// Check if the last visible time is later than the first visible time of this part repeat
				if( mtLastTimeToDraw + mtMaxNoteOffset >= mtOffset )
				{
					// Compute the length of the part to draw (different if 
					// we're on the last repeat of a part in the pattern).
					long lThisPartLength = (i == lMaxPartRepeat) ? mtLastPartRepeatLength : lPartClockLength;

					// Check if the earliest visible time is earlier than the last visible time of this part repeat
					if( mtStartTime - mtMaxNoteOffset <= mtOffset + lThisPartLength - 1 )
					{
						// Some amount of this part repeat is visible, so draw it
						// Make sure that the part length doesn't allow notes mapped beyond the end of the
						// segment/pattern to draw
						(this->*DrawNoteFunc)( fSelected, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, max( 0, min( lThisPartLength, m_mtTimelineLengthForGetNoteRect - mtOffset) ), mtOffset );
					}
				}
			}
		}
	}
	else
	{
		(this->*DrawNoteFunc)( fSelected, hDC, pDMNote, mtStartTime, mtEndTime, lXOffset, nTopNote, nBottomNote, lPartClockLength, 0 );
	}

	// Restore the background color
	::SetBkColor( hDC, crOldBkColor );
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::GetNumExtraBars

DWORD CPianoRollStrip::GetNumExtraBars( void ) const
{
	// Get the time of the last note off from the part
	MUSIC_TIME mtLastNoteOff = m_pPartRef->m_pDMPart->GetLastNoteOff();

	mtLastNoteOff = max( mtLastNoteOff, m_pPartRef->m_pDMPart->GetLastCurveEnd() );

	// If the last note off is before the start of the pattern (or there are no notes)
	if( mtLastNoteOff < 0 )
	{
		// Just return the existing number of extra bars
		return m_dwExtraBars;
	}

	// Compute the length of the part, in ticks
	const long lPartClockLength = m_pPartRef->m_pDMPart->GetClockLength();

	// Compute the length of the pattern, in ticks
	const long lPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

	// Check if the part repeats
	VARIANT varLength;
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) )
	&&	(lPartClockLength < V_I4( &varLength )) )
	{
		// The part repeats, so figure out when the last note off really is

		// Copy the length of the timeline into a working variable
		long lTimelineLength = V_I4( &varLength );

		// Mod by the pattern length
		lTimelineLength %= lPatternLength;

		// If evenly divisible, set to the entire pattern length
		if( lTimelineLength == 0 )
		{
			lTimelineLength = lPatternLength;
		}

		// Mod by the part length
		lTimelineLength %= lPartClockLength;

		// Check if not evenly divisible
		if( lTimelineLength != 0 )
		{
			// Find the last grid that plays
			const long lGridLength = CLOCKS_TO_GRID(lTimelineLength + m_pPartRef->m_pDMPart->m_mtClocksPerGrid, m_pPartRef->m_pDMPart);

			// Initialize to the smallest long
			MUSIC_TIME mtLastSmallNoteOff = LONG_MIN;

			// Get the first note in our list
			CDirectMusicEventItem* pEvent = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
			while( pEvent )
			{
				// Only look at notes that actually play
				if( pEvent->m_mtGridStart <= lGridLength )
				{
					// Get the time of the note's note-off
					const MUSIC_TIME mtNoteOffTime = m_pPartRef->m_pDMPart->AbsTime( pEvent ) + ((CDirectMusicStyleNote *)pEvent)->m_mtDuration;;

					// Only keep the latest note-off event
					if( mtLastSmallNoteOff < mtNoteOffTime )
					{
						mtLastSmallNoteOff = mtNoteOffTime;
					}
				}

				// Iterate through all notes
				pEvent = pEvent->GetNext();
			}

			// Get the first curve in our list
			pEvent = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
			while( pEvent )
			{
				// Only look at curves that actually play
				if( pEvent->m_mtGridStart <= lGridLength )
				{
					// Get the time of the curve's end
					const MUSIC_TIME mtNoteOffTime = m_pPartRef->m_pDMPart->AbsTime( pEvent ) + ((CDirectMusicStyleCurve *)pEvent)->m_mtDuration;;

					// Only keep the latest curve end event
					if( mtLastSmallNoteOff < mtNoteOffTime )
					{
						mtLastSmallNoteOff = mtNoteOffTime;
					}
				}

				// Iterate through all curves
				pEvent = pEvent->GetNext();
			}

			// Set the last note off time to either a note from the last full part repeat, or 
			// a note from the final, partial part repeat
			mtLastNoteOff = max( mtLastNoteOff + V_I4( &varLength ) - lPartClockLength - lTimelineLength,
								 mtLastSmallNoteOff + V_I4( &varLength ) - lTimelineLength );
		}
		else
		{
			// Even number of parts in timeline - just return the time of the note off,
			// plus the start time of the last part repeat.
			mtLastNoteOff = mtLastNoteOff + V_I4( &varLength ) - lPartClockLength;
		}
	}

	// Check if we have extra bars, or if the last note off is after the end of the timeline
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) )
	&&	(m_dwExtraBars || (mtLastNoteOff > V_I4( &varLength ))) )
	{
		// Copy the timeline length to a temporary variable
		long lTimelineLength = V_I4( &varLength );

		// Mod by the pattern length
		lTimelineLength %= lPatternLength;

		// If evenly divisible, set to the entire pattern length
		if( lTimelineLength == 0 )
		{
			lTimelineLength = lPatternLength;
		}

		// Mod by the measure length
		lTimelineLength %= m_lMeasureClocks;

		// Compute the start time of the last visible measure
		lTimelineLength = V_I4( &varLength ) - lTimelineLength;

		// Compute how many extra bars must be shown
		return max( m_dwExtraBars, (DWORD)max( 0, (mtLastNoteOff - lTimelineLength + m_lMeasureClocks - 1) / m_lMeasureClocks) );
	}

	return m_dwExtraBars;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ShouldDisplayPickupBar

BOOL CPianoRollStrip::ShouldDisplayPickupBar( void ) const
{
	if( m_fPickupBar )
	{
		return TRUE;
	}

	if( !ValidPartRefPtr() )
	{
		return FALSE;
	}

	CDirectMusicStyleNote* pDMNote = m_pPartRef->m_pDMPart->m_lstNotes.GetHead();
	if( pDMNote )
	{
		MUSIC_TIME mtTime = m_pPartRef->m_pDMPart->AbsTime( pDMNote );
		if( mtTime < 0 )
		{
			// Should display a pick-up bar

			// Invalidate the variation selection bar, if necessary
			long lLeftDisplay;
			m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
			if( lLeftDisplay < 0 )
			{
				m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

				RECT rect;
				rect.left = lLeftDisplay;
				rect.right = VARIATION_GUTTER_WIDTH + 16 * VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH;
				rect.top = m_lVerticalScroll;
				rect.bottom = rect.top + 2 * VARIATION_BUTTON_HEIGHT;

				// Need to erase, since the variation selection bar moved
				m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, TRUE );
			}

			// Found an early note - return TRUE
			return TRUE;
		}
	}

	CDirectMusicStyleCurve* pDMCurve = m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	if( pDMCurve )
	{
		MUSIC_TIME mtTime = m_pPartRef->m_pDMPart->AbsTime( pDMCurve );
		if( mtTime < 0 )
		{
			// Should display a pick-up bar

			// Invalidate the variation selection bar, if necessary
			long lLeftDisplay;
			m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
			if( lLeftDisplay < 0 )
			{
				m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

				RECT rect;
				rect.left = lLeftDisplay;
				rect.right = VARIATION_GUTTER_WIDTH + 16 * VARIATION_BUTTON_WIDTH + VARIATION_MOAW_WIDTH;
				rect.top = m_lVerticalScroll;
				rect.bottom = rect.top + 2 * VARIATION_BUTTON_HEIGHT;

				// Need to erase, since the variation selection bar moved
				m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, TRUE );
			}

			// Found an early curve - return TRUE
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SegmentDisplayVarChoicesDlg

void CPianoRollStrip::SegmentDisplayVarChoicesDlg( void )
{
	// TODO: Check if this editor is still valid
	if( m_pPartRef->m_pVarChoicesNode )
	{
		m_pMIDIMgr->m_pIFramework->CloseEditor( m_pPartRef->m_pVarChoicesNode );
		RELEASE( m_pPartRef->m_pVarChoicesNode );
	}

	if( m_pPartRef->m_pVarChoicesNode == NULL )
	{
		IDMUSProdComponent *pStyleComponent;
		if( SUCCEEDED( m_pMIDIMgr->m_pIFramework->FindComponent( CLSID_StyleComponent, &pStyleComponent ) ) )
		{
			IAllocVarChoices *pIAllocVarChoices;
			if( SUCCEEDED( pStyleComponent->QueryInterface( IID_IAllocVarChoices, (void**)&pIAllocVarChoices ) ) )
			{
				IUnknown *pIUnknown;
				if( SUCCEEDED( pIAllocVarChoices->GetVarChoicesNode( &pIUnknown ) ) )
				{
					pIUnknown->QueryInterface( IID_IDMUSProdNode, (void**)&m_pPartRef->m_pVarChoicesNode );
					pIUnknown->Release();
				}
				pIAllocVarChoices->Release();
			}
			pStyleComponent->Release();
		}
		
		if( m_pPartRef->m_pVarChoicesNode == NULL )
		{
			return;
		}
	}

	// Set the dialog's title and callback and data
	if( m_pPartRef->m_pVarChoicesNode )
	{
		m_pPartRef->InitializeVarChoicesEditor();

		HRESULT hr = m_pMIDIMgr->m_pIFramework->OpenEditor( m_pPartRef->m_pVarChoicesNode );

		if( SUCCEEDED ( hr ) )
		{
			// Set WINDOWPLACEMENT
			// TODO: ?
			/*
			if( m_pPartRef->m_pVarChoicesNode->m_wp.length )
			{
				if( ::IsWindow( m_pPartRef->m_pVarChoicesNode->m_hWndEditor ) )
				{
					::SetWindowPlacement( m_pPartRef->m_pVarChoicesNode->m_hWndEditor,
										 &m_pPartRef->m_pVarChoicesNode->m_wp );
				}
			}
			*/
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::ConvertAbsTimeToPartOffset

MUSIC_TIME CPianoRollStrip::ConvertAbsTimeToPartOffset( MUSIC_TIME mtTime ) const
{
	return (mtTime % m_pMIDIMgr->m_pDMPattern->CalcLength()) % m_pPartRef->m_pDMPart->GetClockLength();
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SetSelectedVariations

void CPianoRollStrip::SetSelectedVariations( const DWORD dwVariations )
{
	m_dwVariations = dwVariations;

	if ((m_dwVariations & ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled) == ~m_pPartRef->m_pDMPart->m_dwVariationsDisabled)
	{
		m_fGutterPressed = TRUE;
	}
	else
	{
		m_fGutterPressed = FALSE;
	}

	InvalidateVariationBar();
	InvalidatePianoRoll();
	InvalidateCurveStrips();
	InvalidateMarkerStrip();
	UpdateSelectionState();
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::SetPChannelThru

void CPianoRollStrip::SetPChannelThru( void )
{
	int iChannel;
	for( iChannel=0; iChannel < 16; iChannel++ )
	{
		if( FAILED( m_pIConductor->SetPChannelThru( iChannel, m_pPartRef->m_dwPChannel ) ) )
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
// CPianoRollStrip::CancelPChannelThru

void CPianoRollStrip::CancelPChannelThru( void )
{
	for( int iChannel=0; iChannel < 16; iChannel++ )
	{
		m_pIConductor->CancelPChannelThru( iChannel );
	}

	m_fMIDIThruEnabled = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPianoRollStrip::UpdateInstrumentName

void CPianoRollStrip::UpdateInstrumentName( void )
{
	// Initialize our temporary patch to an invalid value
	DWORD dwPatch = INVALID_PATCH;

	// Pointer to interface from which DLS region text can be obtained
	IDMUSProdBandEdit8a* pIBandEdit;

	// Try to get the band edit interface
	pIBandEdit = GetBandEditInterface( m_pMIDIMgr->m_pIStyleNode, static_cast<IDMUSProdStripMgr *>(m_pMIDIMgr), m_pMIDIMgr->m_pTimeline, m_pMIDIMgr->m_dwGroupBits, m_pPartRef->m_dwPChannel );

	if( pIBandEdit )
	{
		pIBandEdit->GetPatchForPChannel( m_pPartRef->m_dwPChannel, &dwPatch );
		pIBandEdit->Release();
	}

	if( m_dwLastPatch != dwPatch )
	{
		m_dwLastPatch = dwPatch;

		if( SV_NORMAL == m_StripView )
		{
			InvalidateFunctionBar();
		}
	}
}

void CPianoRollStrip::UpdatePlayingVariation( void )
{
	m_dwPlayingVariation = 0;

	if( m_pMIDIMgr->m_pSegmentState
	&&	m_pMIDIMgr->m_pIDMPerformance )
	{
		IDirectMusicTrack* pTrack = m_pMIDIMgr->m_pIDMTrack;

		if( pTrack )
		{
			pTrack->AddRef();
		}
		else
		{
			IDirectMusicSegment *pIDirectMusicSegment;
			if( SUCCEEDED( m_pMIDIMgr->m_pSegmentState->GetSegment( &pIDirectMusicSegment ) ) )
			{
				//IDirectMusicTrack *pTrack7;
				if( SUCCEEDED( pIDirectMusicSegment->GetTrack(CLSID_DirectMusicPatternTrack, 0xFFFFFFFF, 0, &pTrack) ) )
				{
					//pTrack7->QueryInterface( IID_IDirectMusicTrack8, (void **)&pTrack );
					//pTrack7->Release();
				}
				pIDirectMusicSegment->Release();
			}
		}

		if( pTrack )
		{
			MUSIC_TIME mtNow;
			if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->GetTime( NULL, &mtNow ) ) )
			{
				// Retrieve the track state parameter for our track
				DMUS_TRACK_STATE_PARAM trackStateParam;
				trackStateParam.pSegState = m_pMIDIMgr->m_pSegmentState;
				trackStateParam.pTrack = pTrack;
				trackStateParam.pTrackState = NULL;
				if( SUCCEEDED( m_pMIDIMgr->m_pIDMPerformance->GetParam(GUID_TrackState, 0xFFFFFFFF, 0,  mtNow, NULL, &trackStateParam) ) )
				{
					// Retrieve the currently playing variation for this track
					DMUS_CURRENT_VARIATION_PARAM curVarParam;
					curVarParam.dwPChannel = m_pPartRef->m_dwPChannel;
					curVarParam.dwVariation = 0;

					// Compute the index of this part reference
					curVarParam.dwIndex = 0;
					POSITION pos = m_pPartRef->m_pPattern->m_lstPartRefs.GetHeadPosition();
					while( pos )
					{
						// Get a pointer to the part ref
						CDirectMusicPartRef *pCDirectMusicPartRef = m_pPartRef->m_pPattern->m_lstPartRefs.GetNext( pos );

						// If we found ourself, exit the loop
						if( pCDirectMusicPartRef == m_pPartRef )
						{
							break;
						}

						// If this part ref is on the same PChannel, increment the index value
						if( pCDirectMusicPartRef->m_dwPChannel == m_pPartRef->m_dwPChannel )
						{
							curVarParam.dwIndex++;
						}
					}

					// Finally, try and retrieve the currently playing variation
					IDirectMusicTrack8 *pTrack8;
					if( SUCCEEDED( pTrack->QueryInterface( IID_IDirectMusicTrack8, (void **)&pTrack8 ) ) )
					{
						if( SUCCEEDED( pTrack8->GetParamEx(GUID_CurrentVariation, 0, NULL, &curVarParam, trackStateParam.pTrackState, 0) ) )
						{
							m_dwPlayingVariation = curVarParam.dwVariation;
						}
						pTrack8->Release();
					}
				}

				pTrack->Release();
			}
		}
	}
}

void CPianoRollStrip::OnVariationTimer( void )
{
	const DWORD dwCurPlayingVariation = m_dwPlayingVariation;
	UpdatePlayingVariation();

	if( dwCurPlayingVariation != m_dwPlayingVariation )
	{
		InvalidateVariationBar();
	}
}
