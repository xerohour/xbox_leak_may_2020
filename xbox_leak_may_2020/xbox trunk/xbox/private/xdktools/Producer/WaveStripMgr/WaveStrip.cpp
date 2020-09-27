// WaveStrip.cpp : Implementation of CWaveStrip
#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "WaveStripMgr.h"
#include "TrackMgr.h"
#include "PropPageMgr_Item.h"
#include "GroupBitsPPG.h"
#include "GridsPerSecondDlg.h"
#include <BaseMgr.h>
#include <SegmentDesigner.h>
#include <SegmentIO.h>
#include <DLLJazzDataObject.h>
#include <dmusicf.h>
#include "GrayOutRect.h"
#include <dmusicp.h>

// TODO - eliminate this need (Needed by RefreshVerticalScrollBarUI())
#include "..\Timeline\TimelineDraw.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Bitmaps
int	 CWaveStrip::sm_nZoomBtnHeight = 0;
int	 CWaveStrip::sm_nZoomBtnWidth = 0;
int	 CWaveStrip::sm_nLayerBtnWidth = 0;
long CWaveStrip::sm_lBitmapRefCount = 0;
CBitmap	CWaveStrip::sm_bmpLocked;
CBitmap	CWaveStrip::sm_bmpUnlocked;
CBitmap	CWaveStrip::sm_bmpVarBtnUp;
CBitmap	CWaveStrip::sm_bmpVarBtnDown;
CBitmap	CWaveStrip::sm_bmpVarInactiveBtnUp;
CBitmap	CWaveStrip::sm_bmpVarInactiveBtnDown;
CBitmap	CWaveStrip::sm_bmpVarGutterBtnUp;
CBitmap	CWaveStrip::sm_bmpVarGutterBtnDown;
CBitmap	CWaveStrip::sm_bmpZoomInBtnUp;
CBitmap	CWaveStrip::sm_bmpZoomInBtnDown;
CBitmap	CWaveStrip::sm_bmpZoomOutBtnUp;
CBitmap	CWaveStrip::sm_bmpZoomOutBtnDown;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// EmptyList

void EmptyList( CTypedPtrList<CPtrList, CTrackItem*>& list )
{
	// Remove and delete all the CTrackItems from the given list
	while( !list.IsEmpty() )
	{
		delete list.RemoveHead();
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// NormalizeList

void NormalizeList( CTrackMgr* pTrackMgr, 
				    CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset )
{
	long lMeasure, lBeat, lGrid, lTick;

	DWORD dwGroupBits = pTrackMgr->GetGroupBits();

	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CTrackItem* pItem = list.GetNext(pos);

		// This method only called for drag/drop and cut/copy/paste
		// so it is safe to mess with the values that are stored in time fields

		// Snap physical time to number of beats
		long lPhysicalBeats;
		MUSIC_TIME mtTimePhysical;
		pTrackMgr->UnknownTimeToClocks( pItem->m_rtTimePhysical, &mtTimePhysical );
		pTrackMgr->ClocksToMeasureBeatGridTick( mtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
		MeasureBeatToBeats( pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

		// Convert logical time to number of beats
		if( pTrackMgr->IsRefTimeTrack() )
		{
			// Use m_mtTimeLogical to store beat difference between physical time and logical time
			ASSERT( pItem->m_rtTimePhysical == pItem->m_rtTimeLogical );
			pItem->m_rtTimeLogical = 0;
		}
		else
		{
			long lLogicalBeats;
			pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)pItem->m_rtTimeLogical, &lMeasure, &lBeat, &lGrid, &lTick );
			MeasureBeatToBeats( pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Use m_mtTimeLogical to store beat difference between physical time and logical time
			pItem->m_rtTimeLogical = lLogicalBeats - lPhysicalBeats;
		}

		// Use m_rtTimePhysical to store an offset
		pItem->m_rtTimePhysical -= rtOffset;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip constructor/destructor

CWaveStrip::CWaveStrip( CTrackMgr* pTrackMgr ) : CBaseStrip( pTrackMgr )
{
	// Validate the pointer to our strip manager
	ASSERT( pTrackMgr );
	if ( pTrackMgr == NULL )
	{
		return;
	}

	// Set our pointer to our strip manager
	m_pTrackMgr = pTrackMgr;

	// Initialize our clipboard formats to 0
	m_cfWaveTrack = 0;
	m_cfWave = 0;
	m_cfDMUSProdFile = 0;

	// Initialize our state variables
	m_lYPos = -1;
	m_fLeftMouseDown = false;
	m_fZoomInDown = false;
	m_fZoomOutDown = false;
	m_nWaveHeight = 0;
	m_wMouseMode = 0;
	m_dwSourceVariationBtns = 0;

	m_nVarAction = VA_NONE;
	m_fVarSolo = false;
	m_nLastVariation = -1;
	m_nSoloVariation = -1;
	m_dwPlayingVariation = 0;
	m_fVariationsTimerActive = false;

	m_fLayerSelected = FALSE;
	m_nVerticalScrollTimer = 0;
	m_dwScrollTick = 0;

	// Initialize the item to toggle (when CTRL-clicking) to NULL
	m_pItemToToggle = NULL;
	m_pLayerForShiftClick = NULL;

	// Initialize fields that are persisted (DirectMusic wave track data)
	m_dwVariationsMask = 0xFFFFFFFF;
	m_dwPChannel = 0;
	m_dwIndex = 0;
	m_lVolume = 0;
	m_dwLockToPart = 0;
	m_dwPartFlagsDM = DMUS_VARIATIONT_RANDOM;
}

CWaveStrip::~CWaveStrip()
{
	CleanUp();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CleanUp

void CWaveStrip::CleanUp( void )
{
	// Clear our pointer to our strip manager
	if( m_pTrackMgr )
	{
		if( m_pTrackMgr->m_pWaveStripForPropSheet == this )
		{
			m_pTrackMgr->m_pWaveStripForPropSheet = NULL;
		}

		m_pTrackMgr = NULL;
	}

	// Delete all the items in m_lstTrackItems (all variations)
	EmptyList( m_lstTrackItems );

	// Delete all the items in m_lstLayers
	while( !m_lstLayers.IsEmpty() )
	{
		CWaveStripLayer* pLayer = m_lstLayers.RemoveHead();
		delete pLayer;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InvalidateVariationBtns

void CWaveStrip::InvalidateVariationBtns( void )
{
	// Invalidate the variation selection bar
	long lLeftDisplay;
	m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	RECT rect;
	rect.left = lLeftDisplay;
	rect.right = rect.left + VARIATION_GUTTER_WIDTH + VARIATION_BUTTONS_WIDTH;
	rect.top = m_StripUI.m_lVerticalScrollYPos;
	rect.bottom = rect.top + VARIATION_BUTTONS_HEIGHT;

	// Don't need to erase, since the variation selection bar will overwrite the area
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InvalidateWaves

void CWaveStrip::InvalidateWaves( void )
{
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, FALSE );
		return;
	}

	// Invalidate the wave portion of the track
	long lLeftDisplay;
	m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	RECT rect;
	rect.left = lLeftDisplay;
	rect.top = m_StripUI.m_lVerticalScrollYPos + VARIATION_BUTTONS_HEIGHT;
	
	// Find the right and bottom boundaries of our strip					
	CDC cDC;
	VARIANT var;
	var.vt = VT_I4;
	if( FAILED ( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_GET_HDC, &var ) ) )
	{
		return;
	}

	CWnd* pWnd = NULL;
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

	rect.bottom = m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight;

	// Really invalidate the waves
	// Don't need to erase, since the horizontal bars will overwrite the area
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InvalidateFBar

void CWaveStrip::InvalidateFBar( void )
{
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, FALSE );
		return;
	}

	// Invalidate the function bar
	RECT rect;
	rect.right = 0;
	rect.top = m_StripUI.m_lVerticalScrollYPos;
	rect.bottom = m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight;
	VARIANT var;
	var.vt = VT_I4;
	if( FAILED ( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
	{
		return;
	}
	rect.left = -V_I4(&var);

	// Need to offset it if we're horizontally scrolled
	long lLeftDisplay;
	m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	rect.left += lLeftDisplay;
	rect.right += lLeftDisplay;

	// Really invalidate the function bar
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DrawVariationButtonBar

void CWaveStrip::DrawVariationButtonBar( HDC hDC )
{
	if( hDC == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// Set the currently playing variation
	UpdatePlayingVariation( false );

	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	if( rectClip.top > m_StripUI.m_lVerticalScrollYPos + VARIATION_BUTTONS_HEIGHT )		// 2 rows	
	{
		return;
	}

	if( rectClip.left > VARIATION_GUTTER_WIDTH + VARIATION_BUTTONS_WIDTH )	// 16 buttons per row
	{
		return;
	}

	// Figure out which variations have waves in them (all variations)
	DWORD dwVariationHasWaves = 0;
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		dwVariationHasWaves |= pItem->m_dwVariations;
	}

	// Draw variation gutter
	if( m_StripUI.m_fVarGutterDown )
	{
		::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpVarGutterBtnDown.GetSafeHandle() ), NULL, 0, m_StripUI.m_lVerticalScrollYPos,
					 VARIATION_GUTTER_WIDTH, VARIATION_GUTTER_HEIGHT, DST_BITMAP | DSS_NORMAL );
	}
	else
	{
		::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpVarGutterBtnUp.GetSafeHandle() ), NULL, 0, m_StripUI.m_lVerticalScrollYPos,
					 VARIATION_GUTTER_WIDTH, VARIATION_GUTTER_HEIGHT, DST_BITMAP | DSS_NORMAL );
	}

	// Create fonts for variation buttons
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
	if( hfontBold == NULL ) 
	{
		return;
	}
	lf.lfWeight = FW_MEDIUM;
	hfontNormal = ::CreateFontIndirect( &lf );
	if( hfontNormal == NULL )
	{
		::DeleteObject( hfontBold );
		return;
	}

	int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );

	COLORREF oldColor = ::GetTextColor( hDC );
	HFONT hfontOld= static_cast<HFONT> ( ::GetCurrentObject( hDC, OBJ_FONT ) );
	
	// Now draw the variation buttons
	TCHAR achText[10];
	int nXpos, nYpos;
	for( nYpos = 0; nYpos < 2; nYpos++ )
	{
		for( nXpos = 0; nXpos < 16; nXpos++ )
		{
			const int nVariation = nXpos + (nYpos << 4);
			const bool fPressed = m_StripUI.m_dwVariationBtns & (1 << nVariation) ? true : false;

			LPARAM lParam;
			if( m_dwVariationsMask & (1 << nVariation) )
			{
				if( fPressed )
				{
					lParam = reinterpret_cast<LPARAM>( sm_bmpVarBtnDown.GetSafeHandle() );
				}
				else
				{
					lParam = reinterpret_cast<LPARAM>( sm_bmpVarBtnUp.GetSafeHandle() );
				}
			}
			else
			{
				if( fPressed )
				{
					lParam = reinterpret_cast<LPARAM>( sm_bmpVarInactiveBtnDown.GetSafeHandle() );
				}
				else
				{
					lParam = reinterpret_cast<LPARAM>( sm_bmpVarInactiveBtnUp.GetSafeHandle() );
				}
			}
			::DrawState( hDC, NULL, NULL, lParam, NULL,
						 VARIATION_GUTTER_WIDTH + nXpos*VARIATION_BUTTON_WIDTH, m_StripUI.m_lVerticalScrollYPos + nYpos*VARIATION_BUTTON_HEIGHT,
						 VARIATION_BUTTON_WIDTH, VARIATION_BUTTON_HEIGHT, DST_BITMAP | DSS_NORMAL );
			
			RECT rect;
			rect.top = fPressed + m_StripUI.m_lVerticalScrollYPos + nYpos*VARIATION_BUTTON_HEIGHT + 3;
			rect.bottom = rect.top + VARIATION_BUTTON_HEIGHT - 6;
			rect.left = fPressed + VARIATION_GUTTER_WIDTH + nXpos*VARIATION_BUTTON_WIDTH + 2;
			rect.right = rect.left + VARIATION_BUTTON_WIDTH - 6;
		
			const bool fPlaying = m_dwPlayingVariation & (1 << nVariation) ? true : false;
			
			_itot( nXpos + nYpos * 16 + 1, achText, 10 );
			if( dwVariationHasWaves & (1 << nVariation) )
			{
				::SetTextColor( hDC, fPlaying ? RGB(255,0,0) : RGB(0, 0, 0) );
				::SelectObject( hDC, hfontBold );
			}
			else
			{
				::SetTextColor( hDC, fPlaying ? RGB(255,0,0) : RGB(255, 255, 255) );
				::SelectObject( hDC, hfontNormal );
			}
			::DrawText( hDC, achText, -1, &rect, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOPREFIX );
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


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// GetNextSelectedItem - helper method for Draw()

void GetNextSelectedItem( CTypedPtrList<CPtrList, CTrackItem*>& list, POSITION &posItem )
{
	// Note that if the item at posItem is selected, this method doesn't do anything useful.
	if( posItem )
	{
		// Save the current position
		POSITION posToSave = posItem;

		// Check if we have not run off the end of the list, and if the currently item is unselected
		while( posItem && !list.GetNext( posItem )->m_fSelected )
		{
			// Current item is unselected, save the position of the next item
			posToSave = posItem;
		}

		// Check if we found a selected item
		if( posToSave )
		{
			// Save the position of the selected item
			posItem = posToSave;

		}
		// Otherwise both posToSave and posItem are NULL
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetNextSelectedItem - helper method for Draw()

CTrackItem* CWaveStrip::GetNextSelectedItem( POSITION pos )
{
		// Get a pointer to the selected item
//	CTrackItem* pItem = m_lstTrackItems.GetAt( posItem );

	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		if( pItem->m_fSelected )
		{
			return pItem;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetNextItem - helper method for Draw()

CTrackItem* CWaveStrip::GetNextItem( POSITION pos )
{
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		return pItem;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetNextItemInLayer - helper method for Draw()

CTrackItem* CWaveStrip::GetNextItemInLayer( POSITION pos, CWaveStripLayer* pLayer )
{
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is in a different layer, skip it
		if( pItem->m_pLayer != pLayer )
		{
			continue;
		}

		return pItem;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetNextSelectedItemInLayer - helper method for Draw()

CTrackItem* CWaveStrip::GetNextSelectedItemInLayer( POSITION pos, CWaveStripLayer* pLayer )
{
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is in a different layer, skip it
		if( pItem->m_pLayer != pLayer )
		{
			continue;
		}

		// If this item is not selected, skip it
		if( pItem->m_fSelected == FALSE )
		{
			continue;
		}

		return pItem;
	}

	return NULL;
}


#define COLOR_UNSELECTED_WAVE_NAME	RGB(0,0,0)
#define COLOR_SELECTED_WAVE_NAME	RGB(255,0,0)

#define COLOR_UNSELECTED_WAVE		RGB(10,10,255)
#define COLOR_SELECTED_WAVE			RGB(255,0,0)
#define COLOR_BG					RGB(255,255,255)
#define COLOR_START_MARKER			RGB(30, 160, 255)
#define COLOR_END_MARKER			RGB(255, 160, 35)
#define COLOR_CLIP_MARKER			RGB(30, 160, 35)

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DrawMinimizedStrip

HRESULT	CWaveStrip::DrawMinimizedStrip( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Prepare WaveDrawParams struct
	WaveDrawParams	wdp;
	memset( &wdp, 0, sizeof(WaveDrawParams) );
	
	wdp.cbSize = sizeof(WaveDrawParams);
	wdp.bErase = FALSE;
	wdp.clrBackground = COLOR_BG;
	wdp.clrForeground = COLOR_UNSELECTED_WAVE;
	wdp.clrStartMarker = COLOR_START_MARKER;
	wdp.clrEndMarker = 0;
	wdp.clrClipMarker = 0;
	wdp.dwDrawOptions = WAVE_MARKSTART;	 // Can be WAVE_MARKBOTHENDS, WAVE_MARKSTART, WAVE_MARKEND

	// Prepare WaveTimelineInfoParams struct
	WaveTimelineInfoParams	wtip;
	memset( &wtip, 0, sizeof(WaveTimelineInfoParams) );

	wtip.cbSize = sizeof(WaveTimelineInfoParams);
	wtip.dwGroupBits = m_pTrackMgr->m_dwGroupBits;

	// Draw bar and beat lines
	m_pTrackMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pTrackMgr->m_dwGroupBits, 0, lXOffset );

	// Get our clipping rectange
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Highlight range selected in timeline
	HighlightTimelineRange( hDC, lXOffset );

	/////////////////////////////////////////////////////////////////////////
	// First, draw the unselected items 

	// Iterate through the list of items
	POSITION posItem = m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( posItem );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is selected, skip it
		if( pItem->m_fSelected == TRUE )
		{
			continue;
		}

		pItem->DrawWave( hDC, lXOffset, -1, &wdp, &wtip );

		// Check if the left edge of the item is beyond the region we're displaying
		if( pItem->m_rectWave.left > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Now, draw the selected items

	// Iterate through the list of items
	wdp.clrForeground = COLOR_SELECTED_WAVE;
	posItem = m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( posItem );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is unselected, skip it
		if( pItem->m_fSelected == FALSE )
		{
			continue;
		}

		pItem->DrawWave( hDC, lXOffset, -1, &wdp, &wtip );

		// Check if the left edge of the item is beyond the region we're displaying
		if( pItem->m_rectWave.left > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DrawHorizontalLines

void CWaveStrip::DrawHorizontalLines( HDC hDC, const RECT &rectClip )
{
	HPEN hpen = ::CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	if( hpen == NULL )
	{
		return;
	}

	HPEN hpenOld = static_cast<HPEN> ( ::SelectObject( hDC, hpen ) );

	// Get nbr of layers
	int nNbrLayers = GetNbrLayers();

	// Determine first layer
	int nCurLayer = m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight;

	// Initialize rectangle
	RECT rectLayer;
	rectLayer.left = rectClip.left;
	rectLayer.right = rectClip.right;
	rectLayer.bottom = m_StripUI.m_lVerticalScrollYPos;

	// Now, draw all the lines
	for( ; nCurLayer <= nNbrLayers ;  nCurLayer++ )
	{
		rectLayer.top = rectLayer.bottom;
		rectLayer.bottom = rectLayer.top + m_nWaveHeight;
		if( rectLayer.top >= rectClip.bottom )
		{
			break;
		}

		::MoveToEx( hDC, rectLayer.left, rectLayer.top, NULL );
		::LineTo( hDC, rectLayer.right, rectLayer.top );
	}

	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpen );
}


#define SECOND_LINE_PENSTYLE	PS_SOLID
#define SECOND_LINE_WIDTH		2
#define SECOND_LINE_COLOR		RGB(0,0,0)
#define GRID_LINE_PENSTYLE		PS_SOLID
#define GRID_LINE_WIDTH			1
#define GRID_LINE_COLOR			RGB(204,204,255)

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DrawClockVerticalLines

void CWaveStrip::DrawClockVerticalLines( HDC hDC, long lXOffset ) // const RECT &rectClip )
{
	// Create the "second" pen
	HPEN hpenSecond = ::CreatePen( SECOND_LINE_PENSTYLE, SECOND_LINE_WIDTH, SECOND_LINE_COLOR );
	if( hpenSecond == NULL )
	{
		return;
	}

	// Create the "grid" pen
	HPEN hpenGrid = ::CreatePen( GRID_LINE_PENSTYLE, GRID_LINE_WIDTH, GRID_LINE_COLOR );
	if( hpenGrid == NULL )
	{
		::DeleteObject( hpenSecond );
		return;
	}

	// Save the current pen and switch to the "second" pen
	HPEN hpenOld = static_cast<HPEN>( ::SelectObject( hDC, hpenSecond ) );

	// Set BkMode
	int nOldBkMode = ::SetBkMode( hDC, TRANSPARENT );

	// Get clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Determine end position
	VARIANT var;
	long lEndPosition;
	m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &lEndPosition );

	// Determine start time
	REFERENCE_TIME refTime, refSecond;
	REFERENCE_TIME refGrid = REFCLOCKS_PER_SECOND / m_StripUI.m_nSnapToRefTime_GridsPerSecond;
	m_pTrackMgr->m_pTimeline->PositionToRefTime( rectClip.left + lXOffset, &refTime );
	refSecond = refTime / REFCLOCKS_PER_SECOND;

	char achText[20];
	long lCurPosition;
	long lSecondPosition;

	do
	{
		refTime = refSecond * REFCLOCKS_PER_SECOND;
		m_pTrackMgr->m_pTimeline->RefTimeToPosition( refTime, &lCurPosition );

		if( lCurPosition < lEndPosition )
		{
			// Save position for "second" text
			lSecondPosition = lCurPosition - lXOffset;

			// Draw "second" line
			::SelectObject( hDC, hpenSecond );
			::MoveToEx( hDC, lSecondPosition, 0, NULL );
			::LineTo( hDC, lSecondPosition, m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight );
		
			// Draw "grid" lines
			for( int i = 0 ;  i < m_StripUI.m_nSnapToRefTime_GridsPerSecond ;  i++ )
			{
				refTime += refGrid;
				m_pTrackMgr->m_pTimeline->RefTimeToPosition( refTime, &lCurPosition );

				if( lCurPosition >= lEndPosition )
				{
					break;
				}

				::SelectObject( hDC, hpenGrid );
				::MoveToEx( hDC, lCurPosition - lXOffset, 0, NULL );
				::LineTo( hDC, lCurPosition - lXOffset, m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight );
			}

			// Draw "second" text
			if( refSecond < 60 )
			{
				_snprintf( achText, 10, "%02d", (int)refSecond );
			}
			else
			{
				_snprintf( achText, 10, "%d:%02d", (int)refSecond/60, (int)refSecond%60 );
			}
			::TextOut( hDC, lSecondPosition, m_StripUI.m_lVerticalScrollYPos, achText, strlen(achText) );
		}

		refSecond++;

	} while( lCurPosition <= rectClip.right + lXOffset );

	::SetBkMode( hDC, nOldBkMode);
	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpenSecond );
	::DeleteObject( hpenGrid );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DrawMaximizedStrip

HRESULT	CWaveStrip::DrawMaximizedStrip( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	CRect rect;
	CWaveRectItem* pWaveRect;
	CWaveRectList lstRects;

	// Prepare WaveDrawParams struct
	WaveDrawParams	wdp;
	memset( &wdp, 0, sizeof(WaveDrawParams) );
	
	wdp.cbSize = sizeof(WaveDrawParams);
	wdp.bErase = FALSE;
	wdp.clrBackground = COLOR_BG;
	wdp.clrForeground = COLOR_UNSELECTED_WAVE;
	wdp.clrStartMarker = COLOR_START_MARKER;
	wdp.clrEndMarker = 0;
	wdp.clrClipMarker = 0;
	wdp.dwDrawOptions = WAVE_MARKSTART;	 // Can be WAVE_MARKBOTHENDS, WAVE_MARKSTART, WAVE_MARKEND

	// Prepare WaveTimelineInfoParams struct
	WaveTimelineInfoParams	wtip;
	memset( &wtip, 0, sizeof(WaveTimelineInfoParams) );

	wtip.cbSize = sizeof(WaveTimelineInfoParams);
	wtip.dwGroupBits = m_pTrackMgr->m_dwGroupBits;

	// Save the DC
	int iSavedDC = ::SaveDC( hDC );

	// Exclude the variation bar from the clipping region
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );
	::ExcludeClipRect( hDC, rectClip.left, m_StripUI.m_lVerticalScrollYPos, rectClip.right, m_StripUI.m_lVerticalScrollYPos + VARIATION_BUTTONS_HEIGHT );

	// Exclude the area below the last layer from the clipping region
	::GetClipBox( hDC, &rectClip );
	int nNbrLayers = GetNbrLayers();
	int nBottom = (nNbrLayers * m_nWaveHeight) + 1;
	if( nBottom < rectClip.bottom )
	{
		::ExcludeClipRect( hDC, rectClip.left, nBottom + VARIATION_BUTTONS_HEIGHT, rectClip.right, rectClip.bottom );
	}

	// Offset the origin so it's below the variation selectionbar
	POINT pt;
	::GetWindowOrgEx( hDC, &pt );
	pt.y -= VARIATION_BUTTONS_HEIGHT;
	::SetWindowOrgEx( hDC, pt.x, pt.y, NULL );

	// Get the revised clipping rectange
	::GetClipBox( hDC, &rectClip );

	// Draw bar, beat, grid lines
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		DrawClockVerticalLines( hDC, lXOffset );
	}
	else
	{
		m_pTrackMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT_GRID, m_pTrackMgr->m_dwGroupBits, 0, lXOffset );
	}

	// Draw horizontal lines
	DrawHorizontalLines( hDC, rectClip );

	// Highlight range selected in timeline
	HighlightTimelineRange( hDC, lXOffset );

	// Create overlapping Wave pen
	HPEN hPenOld = NULL;
	int nModeOld;
	HPEN hPenOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetSysColor(COLOR_WINDOW) );
	if( hPenOverlappingLine )
	{
		hPenOld = (HPEN)::SelectObject( hDC, hPenOverlappingLine );
	}

	CString strText;
	CSize sizeText;
	RECT rectText;
	REFERENCE_TIME rtStart;

	/////////////////////////////////////////////////////////////////////////
	// Draw unselected items 
	wdp.clrForeground = COLOR_UNSELECTED_WAVE;
	::SetTextColor( hDC, COLOR_UNSELECTED_WAVE_NAME );
	if( m_pTrackMgr->IsRefTimeTrack() == false )
	{
		::SetBkMode( hDC, TRANSPARENT );
	}
	
	// Iterate through the list of items
	POSITION posItem = m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( posItem );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is selected, skip it
		if( pItem->m_fSelected == TRUE )
		{
			continue;
		}

		pItem->DrawWave( hDC, lXOffset, LayerToIndex(pItem->m_pLayer), &wdp, &wtip );

		// Determine text
		pItem->FormatUIText( strText );
		::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

		// Adjust pItem->m_rectSelect to accomodate text
		::SetRect( &pItem->m_rectSelect, 
				   pItem->m_rectWave.left, pItem->m_rectWave.top, pItem->m_rectWave.right, pItem->m_rectWave.bottom );
		pItem->m_rectSelect.right = max( pItem->m_rectSelect.right, pItem->m_rectSelect.left + sizeText.cx );

		// Determine rectangle for drawing text
		::SetRect( &rectText, 
				   pItem->m_rectSelect.left, pItem->m_rectSelect.top, pItem->m_rectSelect.right, pItem->m_rectSelect.bottom );
		CTrackItem* pNextItem = GetNextItemInLayer( posItem, pItem->m_pLayer );
		if( pNextItem ) 
		{
			m_pTrackMgr->UnknownTimeToRefTime( pNextItem->m_rtTimePhysical, &rtStart );
			m_pTrackMgr->m_pTimeline->RefTimeToPosition( rtStart, &rectText.right );
			rectText.right -= lXOffset;
		}

		// Draw text
		::DrawText( hDC, strText, strText.GetLength(), &rectText, (DT_LEFT | DT_NOPREFIX) );

		// Offset m_rectSelect to account for horizontal scrolling
		::OffsetRect( &pItem->m_rectSelect, lXOffset, 0 );

		// Frame intersecting rectangles
		{
			pWaveRect = lstRects.GetHead(); 
			for( ;  pWaveRect ;  pWaveRect = pWaveRect->GetNext() )
			{
				// Compute intersection with other Waves
				rect.IntersectRect( &pWaveRect->m_rect, &pItem->m_rectSelect );
				rect.InflateRect( 0, -2 );
								
				// Draw it
				if( rect.left != rect.right
				||  rect.top != rect.bottom )
				{
					nModeOld = ::SetROP2( hDC, R2_XORPEN );
	
					rect.left -= lXOffset;
					rect.right -= lXOffset;
					::MoveToEx( hDC, rect.left, rect.bottom, NULL );
					::LineTo( hDC, rect.left, rect.top );
					::LineTo( hDC, rect.right, rect.top );
					::LineTo( hDC, rect.right, rect.bottom );
					::LineTo( hDC, rect.left, rect.bottom );

					::SetROP2( hDC, nModeOld );
				}
			}

			// Store this Wave's rectangle
			pWaveRect = new CWaveRectItem( &pItem->m_rectSelect );
			if( pWaveRect )
			{
				lstRects.AddHead( pWaveRect );
			}
		}

		// Check if the left edge of the item is beyond the region we're displaying
		if( pItem->m_rectWave.left > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Now, draw the selected items
	wdp.clrForeground = COLOR_SELECTED_WAVE;
	::SetTextColor( hDC, COLOR_SELECTED_WAVE_NAME );

	// Iterate through the list of items
	posItem = m_lstTrackItems.GetHeadPosition();
	while( posItem )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( posItem );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// If this item is not selected, skip it
		if( pItem->m_fSelected == FALSE )
		{
			continue;
		}

		pItem->DrawWave( hDC, lXOffset, LayerToIndex(pItem->m_pLayer), &wdp, &wtip );

		// Determine text
		pItem->FormatUIText( strText );
		::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );

		// Adjust pItem->m_rectSelect to accomodate text
		::SetRect( &pItem->m_rectSelect, 
				   pItem->m_rectWave.left, pItem->m_rectWave.top, pItem->m_rectWave.right, pItem->m_rectWave.bottom );
		pItem->m_rectSelect.right = max( pItem->m_rectSelect.right, pItem->m_rectSelect.left + sizeText.cx );

		// Determine rectangle for drawing text
		::SetRect( &rectText, 
				   pItem->m_rectSelect.left, pItem->m_rectSelect.top, pItem->m_rectSelect.right, pItem->m_rectSelect.bottom );
		CTrackItem* pNextItem = GetNextSelectedItemInLayer( posItem, pItem->m_pLayer );
		if( pNextItem ) 
		{
			m_pTrackMgr->UnknownTimeToRefTime( pNextItem->m_rtTimePhysical, &rtStart );
			m_pTrackMgr->m_pTimeline->RefTimeToPosition( rtStart, &rectText.right );
			rectText.right -= lXOffset;
		}

		// Draw text
		::DrawText( hDC, strText, strText.GetLength(), &rectText, (DT_LEFT | DT_NOPREFIX) );

		// Offset m_rectSelect to account for horizontal scrolling
		::OffsetRect( &pItem->m_rectSelect, lXOffset, 0 );

		// Frame intersecting rectangles
		{
			pWaveRect = lstRects.GetHead(); 
			for( ;  pWaveRect ;  pWaveRect = pWaveRect->GetNext() )
			{
				// Compute intersection with other Waves
				rect.IntersectRect( &pWaveRect->m_rect, &pItem->m_rectSelect );
				rect.InflateRect( 0, -2 );
				
				// Draw it
				if( rect.left != rect.right
				||  rect.top != rect.bottom )
				{
					nModeOld = ::SetROP2( hDC, R2_XORPEN );

					rect.left -= lXOffset;
					rect.right -= lXOffset;
					::MoveToEx( hDC, rect.left, rect.bottom, NULL );
					::LineTo( hDC, rect.left, rect.top );
					::LineTo( hDC, rect.right, rect.top );
					::LineTo( hDC, rect.right, rect.bottom );
					::LineTo( hDC, rect.left, rect.bottom );

					::SetROP2( hDC, nModeOld );
				}
			}

			// Store this Wave's rectangle
			pWaveRect = new CWaveRectItem( &pItem->m_rectSelect );
			if( pWaveRect )
			{
				lstRects.AddHead( pWaveRect );
			}
		}

		// Check if the left edge of the item is beyond the region we're displaying
		if( pItem->m_rectWave.left > rectClip.right )
		{
			// We've gone beyond the right edge of the clipping region - break out of the loop
			break;
		}
	}

	if( hPenOld )
	{
		::SelectObject( hDC, hPenOld );
		::DeleteObject( hPenOverlappingLine );
	}

	// Restore the DC
	::RestoreDC( hDC, iSavedDC );

	// Draw variation buttons
	DrawVariationButtonBar( hDC );

	// Cleanup
	while( lstRects.GetHead() )
	{
		pWaveRect = (CWaveRectItem *)lstRects.RemoveHead();
		delete pWaveRect;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Draw

HRESULT	STDMETHODCALLTYPE CWaveStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_StripUI.m_nStripView == sv );
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		DrawMinimizedStrip( hDC, sv, lXOffset );
	}
	else
	{
		DrawMaximizedStrip( hDC, sv, lXOffset );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::HighlightTimelineRange

void CWaveStrip::HighlightTimelineRange( HDC hDC, long lXOffset )
{
	// Highlight the selected range, if there is one.
	bool fUseGutterSelectRange = m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect);
	if( fUseGutterSelectRange )
	{
		RECT rectClip;
		::GetClipBox( hDC, &rectClip );

		RECT rectHighlight;
		rectHighlight.top = m_StripUI.m_lVerticalScrollYPos + 0;
		rectHighlight.bottom = m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight;

		REFERENCE_TIME rtTime;
		MUSIC_TIME mtTime;

		// Convert the selection start point to a pixel position
		m_pTrackMgr->ClocksToUnknownTime( m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterEndSelect : m_lGutterBeginSelect, &rtTime );
		SnapUnknownTime( rtTime, &rtTime );
		m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtTime );
		m_pTrackMgr->m_pTimeline->ClocksToPosition( mtTime, &(rectHighlight.left) );

		// Offset the rectangle by the horizontal ofset
		rectHighlight.left -= lXOffset;

		// Check that the highlight rectange is visible
		if( rectHighlight.left < rectClip.right )
		{
			// Convert the selection end point to a pixel position
			m_pTrackMgr->ClocksToUnknownTime( m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterBeginSelect : m_lGutterEndSelect, &rtTime );
			SnapUnknownTime( rtTime, &rtTime );
			SnapUnknownToNextIncrement( rtTime, &rtTime );
			m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtTime );
			m_pTrackMgr->m_pTimeline->ClocksToPosition( mtTime, &(rectHighlight.right) );

			// Offset the rectangle by the horizontal ofset
			rectHighlight.right -= lXOffset;

			// Check that the highlight rectange is visible
			if( rectHighlight.right > rectClip.left )
			{
				// Invert the selected rectangle
				GrayOutRect( hDC, &rectHighlight );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetItemFromPoint

CTrackItem* CWaveStrip::GetItemFromPoint( long lXPos, long lYPos )
{
	CTrackItem* pTheItem = NULL;
	BOOL fOnTheWaveHandles = FALSE;

	BOOL fOnWaveHandles;

	CPoint point( lXPos, (lYPos - VARIATION_BUTTONS_HEIGHT) );

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}
		
		if( ::PtInRect( &pItem->m_rectSelect, point ) )
		{
			// Need to set fOnWaveHandles
			fOnWaveHandles = FALSE;
			if( pItem->m_fSelected )
			{
				// Convert the rect of the wave to window coordinates
				CRect selRect = pItem->m_rectSelect;
				VERIFY( SUCCEEDED ( m_pTrackMgr->m_pTimeline->StripToWindowPos( this, &selRect.TopLeft() ) ) );
				VERIFY( SUCCEEDED ( m_pTrackMgr->m_pTimeline->StripToWindowPos( this, &selRect.BottomRight() ) ) );

// TODO: USE THIS CODE!!!
				// Create a tracker
//				CWaveTracker waveTracker( &selRect );
//				UINT nHit = HitTest( waveTracker, lXPos, lYPos );
//			
//				switch( nHit )
//				{
//					case CRectTracker::hitTopLeft:
//					case CRectTracker::hitBottomRight:
//					case CRectTracker::hitTopRight:
//					case CRectTracker::hitBottomLeft:
//					case CRectTracker::hitTop:
//					case CRectTracker::hitBottom:
//					case CRectTracker::hitRight:
//					case CRectTracker::hitLeft:
//						// Cursor is over a RectTracker handle
//						fOnWaveHandles = TRUE;
//						break;
//				}
			}

			if( pTheItem == NULL )
			{
				pTheItem = pItem;
				fOnTheWaveHandles = fOnWaveHandles;
			}
			else
			{
				if( fOnTheWaveHandles == FALSE )
				{
					if( fOnWaveHandles == TRUE )
					{
						// Use the Wave whose handle is being hit
						pTheItem = pItem;
						fOnTheWaveHandles = TRUE;
					}
					else
					{
						// Use the Wave with the narrowest width
						if( (pItem->m_rectSelect.right - pItem->m_rectSelect.left) < (pTheItem->m_rectSelect.right - pTheItem->m_rectSelect.left) ) 
						{
							pTheItem = pItem;
						}
					}
				}
				else
				{
					if( fOnWaveHandles == TRUE )
					{
						// Use the Wave with the narrowest width
						if( (pItem->m_rectSelect.right - pItem->m_rectSelect.left) < (pTheItem->m_rectSelect.right - pTheItem->m_rectSelect.left) ) 
						{
							pTheItem = pItem;
						}
					}
				}
			}
		}
	}

	return pTheItem;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetFBName

void CWaveStrip::GetFBName( CString& strFBName )
{
	CString strTrackGroup, strPChannelName, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	strFBName.Empty();

	// Determine track group text
	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_pTrackMgr->m_dwGroupBits & (1 << i) )
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
				strTrackGroup += strTmp;
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
		strTrackGroup += strTmp;
	}

	// Determine PChannel text
	m_pTrackMgr->GetPChannelName( m_dwPChannel, strPChannelName );

	// Put it all together
	if( strPChannelName.IsEmpty() )
	{
		strFBName.Format( "%s: %d: %s", strTrackGroup, (m_dwPChannel + 1), (m_dwPChannel + 1), m_strStripName );
	}
	else
	{
		strFBName.Format( "%s: %d (%s): %s", strTrackGroup, (m_dwPChannel + 1), strPChannelName, m_strStripName );
	}
	
	ASSERT( strFBName.IsEmpty() == FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CWaveStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
		case SP_NAME:
		{
			if( m_strStripName.IsEmpty() )
			{
				m_strStripName.LoadString( IDS_STRIP_NAME );
			}

			pvar->vt = VT_BSTR; 

			CString strFBName;
			GetFBName( strFBName );
			V_BSTR(pvar) = strFBName.AllocSysString();
			break;
		}

		case SP_RESIZEABLE:
			// We are resizable
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_MINMAXABLE:
			// We support Minimize/Maximize
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_MINIMIZE_HEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = STRIP_MINIMIZE_HEIGHT;
			break;

		case SP_MINHEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = STRIP_MIN_HEIGHT;
			break;

		case SP_MAXHEIGHT:
		{
			int nNbrLayers = GetNbrLayers();

			pvar->vt = VT_INT;
			V_INT(pvar) = VARIATION_BUTTONS_HEIGHT + (nNbrLayers * m_nWaveHeight);
			break;
		}

		case SP_DEFAULTHEIGHT:
		{
			int nNbrLayers = 2;

			if( m_nWaveHeight == 0 )
			{
				// Determine wave height (necessary when creating new strips)
				m_nWaveHeight = long( MAX_WAVE_HEIGHT * m_StripUI.m_dblVerticalZoom + HORIZ_LINE_HEIGHT );
			}

			pvar->vt = VT_INT;
			V_INT(pvar) = VARIATION_BUTTONS_HEIGHT + (nNbrLayers * m_nWaveHeight);
			break;
		}

		case SP_FUNCTIONBAR_EXCLUDE_WIDTH:
			pvar->vt = VT_I4;
			V_I4(pvar) = sm_nZoomBtnWidth;
			break;

		default:
			return CBaseStrip::GetStripProperty( sp, pvar );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CWaveStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate our timeline pointer
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	if( nMsg == WM_KEYDOWN )
	{
		if( (wParam == VK_INSERT)
		||	(wParam == VK_DELETE) )
		{
			if( (0x8000 & GetKeyState( VK_CONTROL ))
			&&	(0x8000 & GetKeyState( VK_SHIFT )) )
			{
				if( wParam == VK_INSERT )
				{
					return m_pTrackMgr->OnNewPart();
				}
				else
				{
					m_pTrackMgr->AllStrips_DeleteSelectedParts();
					return S_OK;
				}
			}
		}
	}

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		switch( nMsg )
		{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				// Change to the track group property page
				OnShowStripProperties();
				if( m_pTrackMgr->m_pPropPageMgr )
				{
					m_pTrackMgr->m_pPropPageMgr->RefreshData();
				}
				return S_OK;

			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
			case WM_KEYDOWN:
				// Do nothing if strip is minimized
				return S_OK;

			case WM_RBUTTONUP:
				// Only display right-click context menu if strip is minimized
				POINT pt;
				if( !GetCursorPos( &pt ) )
				{
					hr = E_UNEXPECTED;
					break;
				}
				m_fInFunctionBarMenu = true;
				hr = PostRightClickMenu( pt );
				m_fInFunctionBarMenu = false;
				return S_OK;
		}
	}

	// Adjust lXPos and lYPos for processing variation buttons
	long lLeftDisplay;
	m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	lLeftDisplay = max( 0, lLeftDisplay );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );
	long lXPosVarBtns = (lXPos - lLeftDisplay);
	long lYPosVarBtns = (lYPos - m_StripUI.m_lVerticalScrollYPos);

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fLeftMouseDown = true;
		m_pLayerForShiftClick = NULL;
		if( lYPosVarBtns < VARIATION_BUTTONS_HEIGHT )
		{
			m_wMouseMode = MM_VARBTNS;
			hr = OnLButtonDownVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
			m_lXPos = -1;
			m_lYPos = -1;
		}
		else
		{
			hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
			m_lXPos = lXPos;	// need to start drag drop from where selection started
			m_lYPos = lYPos;
		}
		break;

	case WM_LBUTTONDBLCLK:
		m_fLeftMouseDown = true;
		m_pLayerForShiftClick = NULL;
		if( lYPosVarBtns < VARIATION_BUTTONS_HEIGHT )
		{
			m_wMouseMode = MM_VARBTNS;
			hr = OnLButtonDblClkVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
			m_lXPos = -1;
			m_lYPos = -1;
		}
		else
		{
			// Use default message handler for WM_LBUTTONDBLCLK
			hr = CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
			m_lYPos = lYPos;
		}
		break;

	case WM_RBUTTONDOWN:
		m_pLayerForShiftClick = NULL;
		if( lYPosVarBtns < VARIATION_BUTTONS_HEIGHT )
		{
			m_wMouseMode = MM_VARBTNS;
			hr = OnRButtonDownVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
		}
		else
		{
			hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		}
		m_lYPos = lYPos;
		break;

	case WM_MOUSEMOVE:
		if( m_wMouseMode == MM_VARBTNS )
		{
			hr = OnMouseMoveVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
		}
		else
		{
			hr = OnMouseMove( wParam, lParam, lXPos, lYPos );
		}
		break;

	case WM_LBUTTONUP:
		m_fLeftMouseDown = false;
		if( m_wMouseMode == MM_VARBTNS )
		{
			hr = OnLButtonUpVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
		}
		else
		{
			hr = OnLButtonUp( wParam, lParam, lXPos, lYPos );
			m_lXPos = lXPos;
			m_lYPos = lYPos;
		}
		m_wMouseMode = 0;
		break;

	case WM_RBUTTONUP:
		if( m_wMouseMode == MM_VARBTNS )
		{
			hr = OnRButtonUpVarBtns( wParam, lParam, lXPosVarBtns, lYPosVarBtns );
		}
		else
		{
			// Use default message handler for WM_RBUTTONUP
			hr = CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		}
		m_wMouseMode = 0;
		break;

	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		m_pTrackMgr->SetAuditionVariations( false );
		m_pTrackMgr->UpdateStatusBarDisplay();
		break;

	case WM_CHAR:
		hr = OnChar( wParam );
		break;

	case WM_VSCROLL:
		hr = RefreshVerticalScrollBarUI();
		break;

	case WM_TIMER:
	{
		HWND hwnd = GetTimelineHWND();
		if( hwnd )
		{
			switch( wParam )
			{
				case TIMER_ZOOM_1:
					::KillTimer( hwnd, TIMER_ZOOM_1 );
					::SetTimer( hwnd, TIMER_ZOOM_2, 100, NULL );
					// Intentional fall-through to TIMER_ZOOM_2

				case TIMER_ZOOM_2:
					if( m_fZoomInDown )
					{
						SetZoom( m_StripUI.m_dblVerticalZoom + 0.01 );
					}
					else
					{
						if( m_StripUI.m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
						{
							SetZoom( m_StripUI.m_dblVerticalZoom - 0.01 );
						}
					}
					break;

				case TIMER_VSCROLL_LAYER_1:
					::KillTimer( hwnd, TIMER_VSCROLL_LAYER_1 );
					::SetTimer( hwnd, TIMER_VSCROLL_LAYER_2, 100, NULL );
					m_nVerticalScrollTimer = TIMER_VSCROLL_LAYER_2;
					// Intentional fall-through to TIMER_VSCROLL_LAYER_2

				case TIMER_VSCROLL_LAYER_2:
				{
					POINT pt;
					::GetCursorPos( &pt );
					m_pTrackMgr->m_pTimeline->ScreenToStripPosition( (IDMUSProdStrip *)this, &pt );
					if( pt.y > (m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight) )
					{
						OnVScrollFromScrollbar( SB_LINEDOWN, (UINT)-1 );
						FBOnWMMessage( WM_MOUSEMOVE, 0, 0, pt.x, (m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight - 1) );
					}
					else
					{
						OnVScrollFromScrollbar( SB_LINEUP, (UINT)-1 );
						FBOnWMMessage( WM_MOUSEMOVE, 0, 0, pt.x, (m_StripUI.m_lVerticalScrollYPos + VARIATION_BUTTONS_HEIGHT + 1) );
					}
					break;
				}
			}
		}
		break;
	}

	case WM_MOVE:
		RefreshVerticalScrollBarUI();
		break;

	case WM_SIZE:
		if( wParam == SIZE_MAXIMIZED )
		{
			m_StripUI.m_nStripView = SV_NORMAL;
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_STRIP_MAXIMIZED;
			m_pTrackMgr->OnDataChanged();
		}
		else if( wParam == SIZE_MINIMIZED )
		{
			m_StripUI.m_nStripView = SV_MINIMIZED;
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_STRIP_MINIMIZED;
			m_pTrackMgr->OnDataChanged();
		}
		else
		{
			if( m_pTrackMgr->m_pTimeline )
			{
				VARIANT var;
				if( SUCCEEDED( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var) ) )
				{
					if( m_StripUI.m_lStripHeight != V_I4(&var) )
					{
						m_StripUI.m_lStripHeight = V_I4(&var);
						m_pTrackMgr->m_nLastEdit = IDS_UNDO_STRIP_RESIZE;
						m_pTrackMgr->OnDataChanged();
					}
				}
			}
		}
		RefreshVerticalScrollBarUI();
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
			case IDM_SNAP_NONE:
			case IDM_SNAP_GRID:
			case IDM_SNAP_BEAT:
			case IDM_SNAP_BAR:
			case IDM_SNAP_SECOND:
				SetSnapTo( wID );
				break;

			case IDM_SNAP_SET_GRID:
				DoGridsPerSecond();
				break;

			case IDM_ZOOM_IN:
				SetZoom( m_StripUI.m_dblVerticalZoom + 0.01);
				break;

			case IDM_ZOOM_OUT:
				if( m_StripUI.m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
				{
					SetZoom( m_StripUI.m_dblVerticalZoom - 0.01);
				}
				break;

			case IDM_ADD_PART:
				hr = m_pTrackMgr->OnNewPart();
				break;

			case IDM_DELETE_PART:
				m_pTrackMgr->AllStrips_DeleteSelectedParts();
				hr = S_OK;
				break;

			case IDM_MERGE_VARIATIONS:
				MergeVariations();
				hr = S_OK;
				break;
			
			case ID_VIEW_PROPERTIES:
				hr = ShowPropertySheet();
				if( SUCCEEDED ( hr ) )
				{
					if( m_fShowItemProps )
					{
						// Change to the item property page
						OnShowItemProperties();

						// Update the property page, if it exists
						RefreshPropertyPage();
					}
					else
					{
						// Change to the track group property page
						OnShowStripProperties();

						// Update the property page, if it exists
						if( m_pTrackMgr->m_pPropPageMgr )
						{
							m_pTrackMgr->m_pPropPageMgr->RefreshData();
						}
					}
				}
				break;

			case IDM_INSERT_LAYER:
			{
				int nLayerIndex = YPosToLayerIndex( m_lYPos );
				hr = InsertLayers( nLayerIndex );
				break;
			}

			case IDM_DELETE_LAYERS:
				hr = DeleteSelectedLayers();
				break;

			case IDM_ENABLE_VARBTNS:
			case IDM_DISABLE_VARBTNS:
				hr = OnEnableVariations( wID );
				break;

			default:
				// Try the default message handler
				CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
			break;
		}
		break;
	}

	case WM_DESTROY:
		hr = OnDestroy();
		break;

	case WM_CREATE:
		hr = OnCreate();

		// Fall through to CBaseStrip's message handler, since we want it to also
		// handle the WM_CREATE message

	default:
		// Try the default message handler
		// This handles WM_COMMAND and WM_CREATE
		CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::FBDraw

HRESULT CWaveStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	if( sv != SV_FUNCTIONBAR_NORMAL )
	{
		return S_OK;
	}

	RECT rectExcludeClip = { 0, 0, 0, 0 };
	
	int iSavedDC = ::SaveDC( hDC );
	
	// Get function bar rect
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var ) ) )
	{
		return E_FAIL;
	}

	// fix the clipping region
	{
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
			return E_FAIL;
		}
		::SelectClipRgn( hDC, hRgn );
		::DeleteObject( hRgn );
	}

	HFONT hfont = NULL;
	HFONT hfontOld = NULL;
	int nBkModeOld = 0;

	// Create font for Layer buttons
	{
		LOGFONT lf;
		memset( &lf, 0 , sizeof(LOGFONT));
		if( m_StripUI.m_dblVerticalZoom < .18 )
		{
			lf.lfHeight = long(MAX_WAVE_HEIGHT * .8 * m_StripUI.m_dblVerticalZoom);
		}
		else
		{
			lf.lfHeight = long(MAX_WAVE_HEIGHT * .8 * .18);
		}
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
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		//lf.lfFaceName = NULL;

		hfont = ::CreateFontIndirect( &lf );
		if( hfont )
		{
			nBkModeOld = ::SetBkMode( hDC, TRANSPARENT );
			hfontOld = static_cast<HFONT>( ::SelectObject( hDC, hfont ) );
		}
	}

	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Draw Layer buttons
	{
		// Get nbr of layers
		int nNbrLayers = GetNbrLayers();

		// Determine first layer
		int nCurLayer = (m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight) + 1;

		// Initialize rectangle
		RECT rectLayer;
		rectLayer.left = rectFBar.right - sm_nLayerBtnWidth - 3;
		rectLayer.right = rectLayer.left + sm_nLayerBtnWidth;
		rectLayer.bottom = m_StripUI.m_lVerticalScrollYPos + VARIATION_BUTTONS_HEIGHT;

		// Create a brush
		HBRUSH brush = ::CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );

		// Now, draw all the buttons
		CString strText;
		int nFirst;
		int nSecond;
		POSITION pos;
		BOOL fSelected;
		for( ; nCurLayer <= nNbrLayers ;  nCurLayer++ )
		{
			rectLayer.top = rectLayer.bottom;
			rectLayer.bottom = rectLayer.top + m_nWaveHeight;
			if( rectLayer.top >= rectClip.bottom )
			{
				break;
			}

			fSelected = FALSE;
			pos = m_lstLayers.FindIndex( nCurLayer - 1 );
			if( pos )
			{
				CWaveStripLayer* pLayer = m_lstLayers.GetAt( pos );

				pLayer->m_fSelected = SyncLayerSelectFlag( pLayer );
				if( pLayer->m_fSelected )
				{
					fSelected = TRUE;
				}
	
				if( IsLayerEmpty( pLayer ) )
				{
					::SetTextColor( hDC, RGB(255,255,255) );
				}
				else
				{
					::SetTextColor( hDC, RGB(0,0,0) );
				}
			}

			::FillRect( hDC, &rectLayer, brush );

			// Determine layer button text
			strText.Empty();
			nFirst = (nCurLayer - 1) / 26;
			nSecond = (nCurLayer - 1) % 26;
			if( nFirst )
			{
				strText = (char)( _T('a' ) + nFirst - 1 );
			}
			strText += (char)( _T('a' ) + nSecond );

			if( nCurLayer == nNbrLayers )
			{
				// 'New' layer
				::FillRect( hDC, &rectLayer, brush );
				::DrawEdge( hDC, &rectLayer, EDGE_BUMP, BF_RECT );
			}
			else if( fSelected )
			{
				::DrawEdge( hDC, &rectLayer, EDGE_SUNKEN, BF_RECT );
				::InflateRect( &rectLayer, 0, -1 );
				::OffsetRect( &rectLayer, 1, 1 );
				::DrawText( hDC, strText, -1, &rectLayer, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOPREFIX );
				::OffsetRect( &rectLayer, -1, -1 );
				::InflateRect( &rectLayer, 0, 1 );
			}
			else
			{
				::DrawEdge( hDC, &rectLayer, EDGE_RAISED, BF_RECT );
				::InflateRect( &rectLayer, 0, -1 );
				::DrawText( hDC, strText, -1, &rectLayer, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOPREFIX );
				::InflateRect( &rectLayer, 0, 1 );
			}
		}

		// Delete the brush
		if( brush )
		{
			::DeleteObject( brush );
		}
	}

	// Cleanup font used for Layer buttons
	if( hfontOld )
	{
		::SelectObject( hDC, hfontOld );
		hfontOld = NULL;
	}
	if( hfont )
	{
		::SetBkMode( hDC, nBkModeOld );
		::DeleteObject( hfont );
		hfont = NULL;
	}

	// Draw Zoom buttons
	{
		if( m_fZoomInDown )
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpZoomInBtnDown.GetSafeHandle() ), NULL,
				rectFBar.right - sm_nZoomBtnWidth - 3, m_StripUI.m_lVerticalScrollYPos,
				sm_nZoomBtnWidth, sm_nZoomBtnHeight, DST_BITMAP | DSS_NORMAL );
		}
		else
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpZoomInBtnUp.GetSafeHandle() ), NULL,
				rectFBar.right - sm_nZoomBtnWidth - 3, m_StripUI.m_lVerticalScrollYPos,
				sm_nZoomBtnWidth, sm_nZoomBtnHeight, DST_BITMAP | DSS_NORMAL );
		}
		if( m_fZoomOutDown )
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpZoomOutBtnDown.GetSafeHandle() ), NULL,
				rectFBar.right - sm_nZoomBtnWidth - 3, sm_nZoomBtnHeight + m_StripUI.m_lVerticalScrollYPos,
				sm_nZoomBtnWidth, sm_nZoomBtnHeight, DST_BITMAP | DSS_NORMAL );
		}
		else
		{
			::DrawState( hDC, NULL, NULL, reinterpret_cast<LPARAM>( sm_bmpZoomOutBtnUp.GetSafeHandle() ), NULL,
				rectFBar.right - sm_nZoomBtnWidth - 3, sm_nZoomBtnHeight + m_StripUI.m_lVerticalScrollYPos,
				sm_nZoomBtnWidth, sm_nZoomBtnHeight, DST_BITMAP | DSS_NORMAL );
		}
	}

	// Set up the rect to exclude
	rectExcludeClip.right = rectFBar.right - 3;
	rectExcludeClip.left = rectExcludeClip.right - sm_nZoomBtnWidth;
	rectExcludeClip.top = m_StripUI.m_lVerticalScrollYPos;
	rectExcludeClip.bottom = m_StripUI.m_lVerticalScrollYPos + sm_nZoomBtnHeight + sm_nZoomBtnHeight;

	::RestoreDC( hDC, iSavedDC );

	// Exclude the zoom buttons
	::ExcludeClipRect( hDC, rectExcludeClip.left, rectExcludeClip.top, rectExcludeClip.right, rectExcludeClip.bottom );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::FBOnWMMessage

HRESULT CWaveStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		switch( nMsg )
		{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				// Change to the track group property page
				OnShowStripProperties();
				if( m_pTrackMgr->m_pPropPageMgr )
				{
					m_pTrackMgr->m_pPropPageMgr->RefreshData();
				}
				return S_OK;

			case WM_LBUTTONDBLCLK:
			case WM_KEYDOWN:
				// Not much to do if strip is minimized
				m_pLayerForShiftClick = NULL;
				return S_OK;
		}
	}

	// Process the window message
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		ASSERT( m_wMouseMode == 0 );
		if( lYPos <= m_StripUI.m_lVerticalScrollYPos + sm_nZoomBtnHeight + sm_nZoomBtnHeight )
		{
			VARIANT varXS;
			if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
			{
				if( lXPos >= V_I4( &varXS ) - sm_nZoomBtnWidth - 3 )
				{
					m_wMouseMode = MM_ZOOMBTNS;
					hr = OnLButtonDownZoomBtns( wParam, lParam, lXPos, lYPos );
				}
			}
		}
		else
		{
			int nLayerIndex = YPosToLayerIndex( lYPos );
			if( nLayerIndex >= 0
			&&  nLayerIndex < m_lstLayers.GetCount() )
			{
				VARIANT varXS;
				if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
				{
					if( lXPos >= V_I4( &varXS ) - sm_nLayerBtnWidth - 3 )
					{
						m_wMouseMode = MM_LAYERBTNS;
						hr = OnLButtonDownLayerBtns( wParam, lParam, lXPos, lYPos );
					}
				}
			}
		}
		// Change the property page to the strip (group bits) property page
		if( m_wMouseMode != MM_ZOOMBTNS
		&&  m_wMouseMode != MM_LAYERBTNS )
		{
			OnShowStripProperties();
		}
		m_lXPos = -1;
		m_lYPos = -1;
		break;
	}

	case WM_LBUTTONUP:
		if( m_wMouseMode == MM_ZOOMBTNS )
		{
			hr = OnLButtonUpZoomBtns( wParam, lParam, lXPos, lYPos );
		}
		else if( m_wMouseMode == MM_LAYERBTNS )
		{
			hr = OnLButtonUpLayerBtns( wParam, lParam, lXPos, lYPos );
		}
		m_lXPos = -1;
		m_lYPos = -1;
		m_wMouseMode = 0;
		break;

	case WM_MOUSEMOVE:
		if( m_wMouseMode == MM_LAYERBTNS )
		{
			hr = OnMouseMoveLayerBtns( wParam, lParam, lXPos, lYPos );
		}
		else
		{
			// Use default message handler for WM_MOUSEMOVE
			hr = CBaseStrip::FBOnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		}
		break;

	case WM_RBUTTONDOWN:
	{
		m_pLayerForShiftClick = NULL;
		int nLayerIndex = YPosToLayerIndex( lYPos );
		if( nLayerIndex >= 0
		&&  nLayerIndex < m_lstLayers.GetCount() )
		{
			VARIANT varXS;
			if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
			{
				if( lXPos >= V_I4( &varXS ) - sm_nLayerBtnWidth - 3 )
				{
					hr = OnRButtonDownLayerBtns( wParam, lParam, lXPos, lYPos );
				}
			}
		}
		break;
	}

	case WM_RBUTTONUP:
	{
		int nLayerIndex = YPosToLayerIndex( lYPos );
		if( nLayerIndex >= 0
		&&  nLayerIndex < GetNbrLayers() )
		{
			VARIANT varXS;
			if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
			{
				if( lXPos >= V_I4( &varXS ) - sm_nLayerBtnWidth - 3 )
				{
					hr = OnRButtonUpLayerBtns( wParam, lParam, lXPos, lYPos );
					m_lXPos = -1;
					m_lYPos = lYPos;	// Used by InsertLayers()	
					break;
				}
			}
		}

		// Set the cursor position for the insert operation to -1
		m_lXPos = -1;
		m_lYPos = -1;

		// Get the cursor position (To put the menu there)
		POINT pt;
		if( !GetCursorPos( &pt ) )
		{
			hr = E_UNEXPECTED;
			break;
		}

		// Display a right-click context menu.
		m_fInFunctionBarMenu = true;
		hr = PostRightClickMenu( pt );
		m_fInFunctionBarMenu = false;
		break;
	}

	default:
		return CBaseStrip::FBOnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Copy

HRESULT CWaveStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if there is anything to copy
	HRESULT hr = CanCopy();
	if( hr != S_OK )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Validate our pointer to the timeline
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	long lStartTime;
	long lEndTime;
	m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

	// Compute the start offset
	REFERENCE_TIME rtStartOffset;
	long lMeasure;
	long lBeat;
	m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lStartTime, &lMeasure, &lBeat );
	m_pTrackMgr->MeasureBeatGridTickToUnknownTime( lMeasure, lBeat, 0, 0, &rtStartOffset);
	CTrackItem* pFirstSelectedItem = GetFirstSelectedItem();
	if( pFirstSelectedItem )
	{
		rtStartOffset = pFirstSelectedItem->m_rtTimePhysical - rtStartOffset;
	}

	// Create an IStream to save the selected items in.
	IStream* pIStreamCopy;
	if( FAILED ( m_pTrackMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStreamCopy) ) )
	{
		return hr;
	}

	// Save the selected items into the stream
	if( FAILED( hr = SaveSelectedItems( pIStreamCopy, rtStartOffset, NULL ) ) )
	{
		pIStreamCopy->Release();
		return hr;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard( pITimelineDataObject, pIStreamCopy, m_cfWaveTrack, m_pTrackMgr, this );

	pIStreamCopy->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::MergeVariations

void CWaveStrip::MergeVariations( void )
{
	bool fChanged = false;

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// For all items that are in the affected variations,
		// mask out all unaffected variations, and check if any of the affected
		// variations are unselected.
		if( pItem->m_dwVariations & m_StripUI.m_dwVariationBtns )
		{
			if( (pItem->m_dwVariations & m_StripUI.m_dwVariationBtns)^m_StripUI.m_dwVariationBtns )
			{
				fChanged = true;
				pItem->m_dwVariations |= m_StripUI.m_dwVariationBtns;
			}
		}
	}

	if( fChanged )
	{
		m_pTrackMgr->m_nLastEdit = IDS_UNDO_MERGE_VARIATIONS;
		m_pTrackMgr->OnDataChanged();

		if( m_pTrackMgr->m_pTimeline )
		{
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
		}

		// Update the property page
		RefreshPropertyPage();

		m_pTrackMgr->SyncWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::MapVariations

DWORD CWaveStrip::MapVariations( DWORD dwOrigVaritions, DWORD dwVariationMap[32] )
{
	// Initialize the variation bits to return to 0
	DWORD dwVarsToReturn = 0;

	// Iterate through all 32 variations
	for( int nClip = 0; nClip < 32; nClip++ )
	{
		// Check if the original variation bit nClip is set
		if( dwOrigVaritions & (1 << nClip) )
		{
			// Set the bit in dwVarsToReturn that matches original bit nClip
			dwVarsToReturn |= dwVariationMap[nClip];
		}
	}

	// Return the new variation flags
	return dwVarsToReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CalculateDropTime

REFERENCE_TIME CWaveStrip::CalculateDropTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI )
{
	REFERENCE_TIME rtDropTime;

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime );
		rtTime += pCopyPasteUI->m_rtOffset;

		long lMinute, lSec, lGrid, lMs;
		m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );

		if( m_StripUI.m_nSnapToRefTime != IDM_SNAP_NONE )
		{
			if( lMs < 0 )
			{
				m_pTrackMgr->MinSecGridMsToRefTime( this, lMinute, lSec, (lGrid - 1), 0, &rtTime );
				m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );
			}
		}

		switch( m_StripUI.m_nSnapToRefTime )
		{
			case IDM_SNAP_NONE:
				// Nothing to do
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MinSecGridMsToRefTime( this,
													lMinute,
												    lSec,
													lGrid,
													pCopyPasteUI->m_lRefTimeMs,
													&rtTime );
				break;
			
			case IDM_SNAP_SECOND:
				m_pTrackMgr->MinSecGridMsToRefTime( this,
													lMinute,
													lSec,
													pCopyPasteUI->m_lRefTimeGrid,
													pCopyPasteUI->m_lRefTimeMs,
													&rtTime );
				break;
		}

		m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtDropTime );
	}
	else
	{
		mtTime += pCopyPasteUI->m_mtOffset;

		long lMeasure, lBeat, lGrid, lTick;
		m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );

		if( m_StripUI.m_nSnapToMusicTime != IDM_SNAP_NONE )
		{
			if( lTick < 0 )
			{
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, (lGrid - 1), 0, &mtTime );
				m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );
			}
		}

		switch( m_StripUI.m_nSnapToMusicTime )
		{
			case IDM_SNAP_NONE:
				// Nothing to do
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  lBeat,
														  lGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
			
			case IDM_SNAP_BEAT:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  lBeat,
														  pCopyPasteUI->m_lMusicTimeGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
			
			case IDM_SNAP_BAR:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  pCopyPasteUI->m_lMusicTimeBeat,
														  pCopyPasteUI->m_lMusicTimeGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
		}

		m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtDropTime );
	}

	return rtDropTime;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CalculatePasteTime

REFERENCE_TIME CWaveStrip::CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI )
{
	REFERENCE_TIME rtDropTime;

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtTime );

		long lMinute, lSec, lGrid, lMs;
		m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );

		switch( m_StripUI.m_nSnapToRefTime )
		{
			case IDM_SNAP_NONE:
				// Nothing to do
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MinSecGridMsToRefTime( this,
													lMinute,
												    lSec,
													lGrid,
													pCopyPasteUI->m_lRefTimeMs,
													&rtTime );
				break;
			
			case IDM_SNAP_SECOND:
				m_pTrackMgr->MinSecGridMsToRefTime( this,
													lMinute,
													lSec,
													pCopyPasteUI->m_lRefTimeGrid,
													pCopyPasteUI->m_lRefTimeMs,
													&rtTime );
				break;
		}

		m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtDropTime );
	}
	else
	{
		long lMeasure, lBeat, lGrid, lTick;
		m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );

		switch( m_StripUI.m_nSnapToMusicTime )
		{
			case IDM_SNAP_NONE:
				// Nothing to do
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  lBeat,
														  lGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
			
			case IDM_SNAP_BEAT:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  lBeat,
														  pCopyPasteUI->m_lMusicTimeGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
			
			case IDM_SNAP_BAR:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure,
														  pCopyPasteUI->m_lMusicTimeBeat,
														  pCopyPasteUI->m_lMusicTimeGrid,
														  pCopyPasteUI->m_lMusicTimeTick,
														  &mtTime );
				break;
		}

		m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtDropTime );
	}

	return rtDropTime;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::PasteAt

HRESULT CWaveStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
{
	HRESULT hr = E_FAIL;

	// Validate the data object pointer
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Calc rtSnapPasteTime 
	REFERENCE_TIME rtSnapPasteTime;
	if( FAILED( m_pTrackMgr->ClocksToUnknownTime( lPasteTime, &rtSnapPasteTime ) ) 
	||  FAILED( SnapUnknownTime( rtSnapPasteTime, &rtSnapPasteTime ) ) )
	{
		return E_UNEXPECTED;
	}

	// Handle CF_WAVETRACK
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfWaveTrack ) == S_OK )
	{
		// Try and read the stream that contains the items
		IStream* pIStream;
		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( m_cfWaveTrack, &pIStream) ) )
		{
			// Read DMUS_FOURCC_COPYPASTE_UI_CHUNK
			IDMUSProdRIFFStream* pIRiffStream;
			MMCKINFO ck;
			DWORD dwByteCount;
			DWORD dwSize;

			ioCopyPasteUI iCopyPasteUI;
			ZeroMemory( &iCopyPasteUI, sizeof(ioCopyPasteUI) );

			hr = AllocRIFFStream( pIStream, &pIRiffStream );
			if( SUCCEEDED( hr ) )
			{
				DWORD dwPos = StreamTell( pIStream );

				ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
				if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) == 0 )
				{
					dwSize = min( ck.cksize, sizeof(ioCopyPasteUI) );
					if( FAILED ( pIStream->Read( &iCopyPasteUI, dwSize, &dwByteCount ) )
					||  dwByteCount != dwSize )
					{
						hr = E_FAIL;
					}
				}
				RELEASE( pIRiffStream );

				StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
			}

			if( SUCCEEDED ( hr ) )
			{
				// If we're in a drag-drop operation 
				if( bDropNotPaste )
				{
					// And we are dropping in the same strip
					if( m_pITargetDataObject == m_pISourceDataObject )
					{
						// Make sure we are /dropping to a different location
						REFERENCE_TIME rtSnapTime;
						if( SUCCEEDED ( SnapXPosToUnknownTime( m_lStartDragPosition, &rtSnapTime ) ) )
						{
							if( rtSnapPasteTime == rtSnapTime )
							{
								ASSERT( m_lYPos > 0 );  // m_lYPos not being set!!!
								if( YPosToLayerIndex(m_lYPos) == iCopyPasteUI.m_nStartLayerIndex )
								{
									// Didn't move - exit early
									hr = E_FAIL;
									goto Leave_1;
								}
							}
						}
					}
				}

				// Load the stream into a list of items
				CTypedPtrList<CPtrList, CTrackItem*> list;
				hr = LoadList( list, pIStream );

				// Do we need to convert times?
				if( iCopyPasteUI.m_fRefTimeTrack != m_pTrackMgr->IsRefTimeTrack() )
				{
					REFERENCE_TIME rtOrigTimeLogical;
					POSITION pos = list.GetHeadPosition();
					while( pos )
					{
						CTrackItem* pItem = list.GetNext( pos );

						rtOrigTimeLogical = pItem->m_rtTimeLogical;
						pItem->SwitchTimeBase();
						pItem->m_rtTimeLogical = rtOrigTimeLogical;
					}
				}

				// Check if the load operation succeeded
				if ( SUCCEEDED( hr ) )
				{
					// Create a mapping of source variations to destination variations
					ASSERT( m_dwSourceVariationBtns != 0 );	
					DWORD dwVariationMap[32];
					ZeroMemory( dwVariationMap, sizeof(DWORD) * 32 );
					int nClip, nCur, nTmp;
					nCur = 0;
					for( nClip = 0 ;  nClip < 32 ;  nClip++ )
					{
						if( m_dwSourceVariationBtns & (1 << nClip) )
						{
							for( nTmp = nCur ;  nTmp < 32 ;  nTmp++ )
							{
								if( m_StripUI.m_dwVariationBtns & (1 << nTmp) )
								{
									dwVariationMap[nClip] = (1 << nTmp);
									nCur = nTmp + 1;
									break;
								}
							}
							if ( nTmp == 32 )
							{
								EmptyList( list );
								AfxMessageBox( IDS_ERROR_VAR_MISMATCH );
								hr = E_FAIL;
								goto Leave_1;
							}
						}
					}

					// Make sure we have enough layers
					int nNbrPasteLayers = (iCopyPasteUI.m_nMaxLayerIndex - iCopyPasteUI.m_nMinLayerIndex) + 1;
					if( nNbrPasteLayers > m_lstLayers.GetCount() )
					{
						// Not enough layers in target strip
						EmptyList( list );
						AfxMessageBox( IDS_ERROR_LAYER_MISMATCH );
						hr = E_FAIL;
						goto Leave_1;
					}

					// Determine offset for mapping source layers to destination layers
					int nLayerPasteOffset = 0;
					if( m_lYPos > 0 )
					{
						nLayerPasteOffset = YPosToLayerIndex( m_lYPos );
						if( nLayerPasteOffset == -1 )
						{
							// Not dropping on any layers
							EmptyList( list );
							hr = E_FAIL;
							goto Leave_1;
						}
						else
						{
							nLayerPasteOffset -= iCopyPasteUI.m_nStartLayerIndex;
						}
					}
					if( (iCopyPasteUI.m_nMaxLayerIndex + nLayerPasteOffset) > m_lstLayers.GetCount() )
					{
						nLayerPasteOffset -= (iCopyPasteUI.m_nMaxLayerIndex + nLayerPasteOffset) - m_lstLayers.GetCount();
					}
					if( (iCopyPasteUI.m_nMinLayerIndex + nLayerPasteOffset) < 0 )
					{
						nLayerPasteOffset += -(iCopyPasteUI.m_nMinLayerIndex + nLayerPasteOffset);
					}

					// Calc rtPasteTime 
					REFERENCE_TIME rtPasteTime;
					if( bDropNotPaste )
					{
						rtPasteTime = CalculateDropTime( lPasteTime, &iCopyPasteUI );
					}
					else
					{
						rtPasteTime = CalculatePasteTime( lPasteTime, &iCopyPasteUI );
					}

					if( m_pTrackMgr->m_pTimeline )
					{
						// The length of the segment, in beats, in music time
						MUSIC_TIME mtSegmentLength = LONG_MAX;
						REFERENCE_TIME rtSegmentLength;
						m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtSegmentLength, &rtSegmentLength ); 

						// Check if there are any items to paste
						if( !list.IsEmpty() )
						{
							long lMeasure, lBeat, lGrid, lTick;

							// Unselect all items so the only selected items are the dropped or pasted ones
							UnselectAll();

							// Make sure the last item lands in the last beat or sooner
							VARIANT varLength;
							if( SUCCEEDED( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
							{
								mtSegmentLength = V_I4(&varLength);
								m_pTrackMgr->ClocksToUnknownTime( mtSegmentLength, &rtSegmentLength );
	
								// Make sure the last item lands in the last beat or sooner
								REFERENCE_TIME rtTimeLastItem = list.GetTail()->m_rtTimePhysical + rtPasteTime;
								if( rtTimeLastItem >= rtSegmentLength )
								{
									m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( rtPasteTime - (rtTimeLastItem - rtSegmentLength),
																				  &lMeasure, &lBeat, &lGrid, &lTick );
									m_pTrackMgr->MeasureBeatGridTickToUnknownTime( lMeasure, lBeat, lGrid, lTick, 
																				  &rtPasteTime );
									SnapUnknownTime( rtPasteTime, &rtPasteTime );
								}
							}
	
							// Make sure the first item lands in the first beat or later
							REFERENCE_TIME rtTimeFirstItem = list.GetHead()->m_rtTimePhysical + rtPasteTime;
							if( rtTimeFirstItem < 0 )
							{
								m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( rtPasteTime + -rtTimeFirstItem,
																			  &lMeasure, &lBeat, &lGrid, &lTick );
								m_pTrackMgr->MeasureBeatGridTickToUnknownTime( lMeasure, lBeat, lGrid, lTick, 
																			  &rtPasteTime );
								SnapUnknownTime( rtPasteTime, &rtPasteTime );
							}
	
						}

						// Check if we're doing a paste
						if( bDropNotPaste == FALSE )
						{
							// We're doing a paste, so get the paste type
							TIMELINE_PASTE_TYPE tlPasteType;
							if( FAILED( m_pTrackMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
							{
								EmptyList( list );
								hr = E_FAIL;
								goto Leave_1;
							}
	
							// Check if we're doing a paste->overwrite
							if( tlPasteType == TL_PASTE_OVERWRITE )
							{
								REFERENCE_TIME rtTimeFirstItem = list.GetHead()->m_rtTimePhysical + rtPasteTime;
	
								long lMeasureStart, lMeasureEnd;
								long lBeatStart, lBeatEnd;
								long lGrid;
								long lTick;
								m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( rtTimeFirstItem,
																			  &lMeasureStart, &lBeatStart, &lGrid, &lTick );
								m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( rtTimeFirstItem + iCopyPasteUI.m_rtPasteOverwriteRange,
																			  &lMeasureEnd, &lBeatEnd, &lGrid, &lTick );

								// Now, delete any items that fall between the first and last beats we're pasting in
								fChanged = DeleteBetweenMeasureBeats( lMeasureStart, lBeatStart, lMeasureEnd, lBeatEnd );
							}
	
						}

						// Iterate throught the list of items we loaded
						while( !list.IsEmpty() )
						{
							// Remove the head of the list
							CTrackItem* pItem = list.RemoveHead();

							// Check if the item will land before the end of the segment
							pItem->m_rtTimePhysical += rtPasteTime;
							pItem->m_rtTimePhysical  = max( 0, pItem->m_rtTimePhysical);
							if( pItem->m_rtTimePhysical < rtSegmentLength )
							{
								// Set layer  (m_nPasteLayerIndex stores the orig layer index)
								pItem->m_pLayer = CreateLayerForIndex( pItem->m_nPasteLayerIndex + nLayerPasteOffset );
								if( pItem->m_pLayer == NULL )
								{
									delete pItem;
								}
								else
								{
									// Recalc m_mtTime fields
									pItem->SetTimePhysical( pItem->m_rtTimePhysical, STP_LOGICAL_FROM_BEAT_OFFSET );
					
									// When pasted or dropped, each item is selected
									pItem->m_fSelected = TRUE;

									pItem->m_dwVariations = MapVariations( pItem->m_dwVariations, dwVariationMap );
									InsertByAscendingTime( pItem, TRUE );

									// We changed
									fChanged = TRUE;
								}
							}
							else
							{
								// Item would be pasted beyond the end of the segment - delete it
								delete pItem;
							}
						}
					}
				}
			}
Leave_1:
			pIStream->Release();
		}
	}

	// Handle CF_DLS_WAVE and CF_DMUSPROD_FILE
	else if( (pITimelineDataObject->IsClipFormatAvailable( m_cfWave ) == S_OK)
		 ||	 (pITimelineDataObject->IsClipFormatAvailable( m_cfDMUSProdFile ) == S_OK) )
	{
		IDMUSProdNode* pIDocRootNode = NULL;
		if( pITimelineDataObject->IsClipFormatAvailable( m_cfWave ) == S_OK )
		{
			IDataObject* pIDataObject;
			hr = pITimelineDataObject->Export( &pIDataObject );
			if( SUCCEEDED ( hr ) )
			{
				hr = m_pTrackMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
				pIDataObject->Release();
			}
		}
		else if( pITimelineDataObject->IsClipFormatAvailable( m_cfDMUSProdFile ) == S_OK )
		{
			// Ensure the file is open
			pIDocRootNode = EnsureFileIsOpen( pITimelineDataObject );
		}

		if( pIDocRootNode )
		{
			// Unselect all existing items in this strip so the only selected items are the dropped
			// or pasted ones
			UnselectAll();

			GUID guidNodeId;
			if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
			{
				CTrackItem* pItem = new CTrackItem( m_pTrackMgr, this );
				if( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					pItem->m_pLayer = YPosToLayer( m_lYPos, true );
					if( pItem->m_pLayer == NULL )
					{
						hr = E_OUTOFMEMORY;
					}
					else
					{
						hr = pItem->SetFileReference( pIDocRootNode );
						if( SUCCEEDED ( hr ) )
						{
							pItem->m_fLoopedUI = pItem->m_WaveInfo.fIsLooped;

							rtSnapPasteTime = max( rtSnapPasteTime, 0 );

							hr = pItem->SetTimePhysical( rtSnapPasteTime, STP_LOGICAL_SET_DEFAULT );
							if( SUCCEEDED ( hr ) )
							{
								// When pasted or dropped, each item is selected
								pItem->m_fSelected = TRUE;

								pItem->m_dwVariations = m_StripUI.m_dwVariationBtns;
								InsertByAscendingTime( pItem, TRUE );

								// We changed
								fChanged = TRUE;
							}
						}
					}
					if( FAILED ( hr ) )
					{
						delete pItem;
					}
				}
			}

			RELEASE( pIDocRootNode );
		}
	}

	if( m_nStripIsDragDropSource )
	{
		// Drag/drop Target and Source are the same strip
		m_nStripIsDragDropSource = 2;
	}

	if( CWaveStrip::m_pIDocRootOfDragDropSource
	&&  CWaveStrip::m_pIDocRootOfDragDropSource == m_pTrackMgr->m_pIDocRootNode )
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CWaveStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Paste

HRESULT CWaveStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that there is something to paste
	HRESULT hr;
	hr = CanPaste( pITimelineDataObject );
	if( hr != S_OK )
	{
		// Nothing to paste - return an error
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Validate our TrackMgr and Timeline pointers
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks) ) )
	{
		return E_UNEXPECTED;
	}

	// Do the paste
	BOOL fChanged = FALSE;
	hr = PasteAt(pITimelineDataObject, lClocks, false, fChanged);

	// Check if the paste succeeded AND if something was actually pasted
	if( SUCCEEDED(hr) && fChanged )
	{
		// Update the selection regions to only include the selected items
		SelectRegionsFromSelectedItems();

		// Notify the containing segment that we did a paste operation
		m_pTrackMgr->m_nLastEdit = IDS_PASTE;
		m_pTrackMgr->OnDataChanged();

		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the track item property page is visible
		OnShowItemProperties();

		// Refresh the track item property page
		RefreshPropertyPage();

		// Sync with DirectMusic
		m_pTrackMgr->SyncWithDirectMusic();
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InsertItem

HRESULT CWaveStrip::InsertItem( CTrackItem* pItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTrackMgr
	&&	m_pTrackMgr->m_pTimeline )
	{
		// Unselect all items in this strip 
		UnselectAll();

		// Unselect all items in other strips
		UnselectGutterRange();

		// By default, a newly selected item is selected
		pItem->m_fSelected = TRUE;
	}

	// Insert the item into our list of items
	pItem->m_dwVariations = m_StripUI.m_dwVariationBtns;
	InsertByAscendingTime( pItem, FALSE );

	if( m_pTrackMgr
	&&	m_pTrackMgr->m_pTimeline )
	{
		// If it's not already selected, add the beat the item was inserted on
		// to the list of selected regions
		if( !CListSelectedRegion_Contains( pItem->m_rtTimePhysical ) )
		{
			CListSelectedRegion_AddRegion( m_lXPos );
		}

		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );

		// Show the property sheet
		ShowPropertySheet();
		
		// Switch to the track item's property page
		CPropPageMgrItem::sm_nActiveTab = 0;
		OnShowItemProperties();

		// Refresh the track item property page
		RefreshPropertyPage();

		// Notify the containing segment that an insert occurred
		m_pTrackMgr->m_nLastEdit = IDS_INSERT;
		m_pTrackMgr->OnDataChanged();
	}

	// Sync changes with DirectMusic
	m_pTrackMgr->SyncWithDirectMusic();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Insert

HRESULT CWaveStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that our timeline pointer is valid
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	if( m_StripUI.m_nStripView == SV_MINIMIZED
	||  m_StripUI.m_dwVariationBtns == 0 
	||  m_lXPos < 0 )
	{
		return E_FAIL;
	}

	// Unselect all items in this strip 
	UnselectAll();

	// Unselect all items in other strips
	UnselectGutterRange();

	// Determine clocks for snapped XPos
	REFERENCE_TIME rtSnapTime;
	if( FAILED ( SnapXPosToUnknownTime( m_lXPos, &rtSnapTime ) ) )
	{
		return E_FAIL;
	}

	// Convert the insert position to a layer
	CWaveStripLayer* pLayer = YPosToLayer( m_lYPos, true );
	if( pLayer == NULL )
	{
		return E_OUTOFMEMORY;
	}

	CTrackItem* pItem = new CTrackItem( m_pTrackMgr, this );
	if( pItem == NULL )
	{
		delete pLayer;
		return E_OUTOFMEMORY;
	}

	// Set the measure and beat the item was inserted in
	if( FAILED( pItem->SetTimePhysical( rtSnapTime, STP_LOGICAL_SET_DEFAULT ) ) )
	{
		delete pLayer;
		delete pItem;
		return E_UNEXPECTED;
	}

	// Set the layer
	pItem->m_pLayer = pLayer;

	// By default, a newly selected item is selected
	pItem->m_fSelected = TRUE;

	// Insert the item into our list of items
	pItem->m_dwVariations = m_StripUI.m_dwVariationBtns;
	InsertByAscendingTime( pItem, FALSE );

	// If it's not already selected, add the beat the item was inserted on
	// to the list of selected regions
	if( !CListSelectedRegion_Contains( pItem->m_rtTimePhysical ) )
	{
		CListSelectedRegion_AddRegion( m_lXPos );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	// Show the property sheet
	ShowPropertySheet();

	// Switch to the track item's property page
	CPropPageMgrItem::sm_nActiveTab = 0;
	OnShowItemProperties();

	// Refresh the track item property page
	RefreshPropertyPage();

	// Notify the containing segment that an insert occurred
	m_pTrackMgr->m_nLastEdit = IDS_INSERT;
	m_pTrackMgr->OnDataChanged();

	// No need to sync with DirectMusic - the item starts out as empty

	// Return the success/failure code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Delete

HRESULT CWaveStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Delete all selected items
	DeleteSelectedItems();

	// Clear all selection ranges
	m_pSelectedRegions->Clear();

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	// Refresh the track item property page, if it exists
	RefreshPropertyPage();

	// Notify the containing segment that a delete operation occurred
	m_pTrackMgr->m_nLastEdit = IDS_DELETE;
	m_pTrackMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pTrackMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SelectAll

HRESULT CWaveStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTrackMgr != NULL );
	if( m_pTrackMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTrackMgr->m_pTimeline != NULL );
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Select everything
	m_pSelectedRegions->SelectAll();

	// Update the selection state of the items
	if( SelectItemsInSelectedRegions() )
	{
		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Ensure the track item property page is visible
		OnShowItemProperties();

		// Refresh the track item property page
		RefreshPropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CanPaste

HRESULT CWaveStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Do nothing if strip is minimized
	// Do nothing if no variations are selected
	if( m_StripUI.m_nStripView == SV_MINIMIZED
	||  m_StripUI.m_dwVariationBtns == 0 )
	{
		return S_FALSE;
	}

	// Verify that we have a valid timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard formats
	hr = S_FALSE;

	// CF_WAVETRACK
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfWaveTrack ) == S_OK )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}
	// CF_DLS_WAVE
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfWave ) == S_OK )
	{
		IDataObject* pIDataObject;
		if( SUCCEEDED ( pITimelineDataObject->Export( &pIDataObject ) ) )
		{
			IDMUSProdNode* pIDocRootNode;
			if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
			{
				// Cannot drop waves from DLS Collections
				// Existence of DocRoot means this is a wave file
				hr = S_OK;

				RELEASE( pIDocRootNode );
			}

			RELEASE( pIDataObject );
		}
	}
	// CF_DMUSPROD_FILE
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfDMUSProdFile ) == S_OK )
	{
		IStream *pStream;
		if( SUCCEEDED( pITimelineDataObject->AttemptRead( m_cfDMUSProdFile, &pStream ) ) )
		{
			CFProducerFile cfProducerFile;
			if( SUCCEEDED( LoadCF_DMUSPROD_FILE( pStream, &cfProducerFile ) ) )
			{
				int nDot = cfProducerFile.strFileName.ReverseFind( _T('.') );
				if( nDot >= 0 )
				{
					CString strExt = cfProducerFile.strFileName.Right( cfProducerFile.strFileName.GetLength() - nDot );
					if( 0 == strExt.CompareNoCase( _T(".wav") )
					||	0 == strExt.CompareNoCase( _T(".wvp") ) )
					{
						// Can drop .wav and .wvp files
						hr = S_OK;
					}
				}
			}
			pStream->Release();
		}
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CanInsert

HRESULT CWaveStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Do nothing if strip is minimized
	// Do nothing if no variations are selected
	if( m_StripUI.m_nStripView == SV_MINIMIZED
	||  m_StripUI.m_dwVariationBtns == 0 )
	{
		return S_FALSE;
	}

	// Verify that we have a valid Timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime) ) )
	{
		return E_UNEXPECTED;
	}

	// Check if the insert position is invalid, or if the user clicked in the function bar
	if( m_fInFunctionBarMenu
	|| (lInsertTime < 0)
	|| (m_lYPos < 0) )
	{
		// It's invalid - we can't insert
		return S_FALSE;
	}

	// Convert from a time to a pixel position
	long lPosition;
	if( FAILED( m_pTrackMgr->m_pTimeline->ClocksToPosition( lInsertTime, &lPosition ) ) )
	{
		return E_UNEXPECTED;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED( m_pTrackMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var) ) )
	{
		return E_UNEXPECTED;
	}

	// If the position to insert is beyond the end of the segment, return S_FALSE since we can't insert there
	if( lInsertTime >= V_I4(&var) )
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
// CWaveStrip::DragEnter

HRESULT CWaveStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_dwScrollTick = 0;

	return CBaseStrip::DragEnter( pIDataObject, grfKeyState, pt, pdwEffect );
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DragOver

HRESULT CWaveStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int m_nScrollArea = min( (m_nWaveHeight >> 1), 12 ); 
	int nLayerIndex = YPosToLayerIndex( pt.y );

	if( nLayerIndex < m_lstLayers.GetCount() )
	{
		if( (pt.y - VARIATION_BUTTONS_HEIGHT) < (m_StripUI.m_lVerticalScrollYPos + m_nScrollArea)
		||   pt.y > ((m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight) - m_nScrollArea) )
		{
			if( m_dwScrollTick == 0 )
			{
				m_dwScrollTick = GetTickCount() + 500;
			}
			else if( m_dwScrollTick < GetTickCount() )
			{
				if( (pt.y - VARIATION_BUTTONS_HEIGHT) < (m_StripUI.m_lVerticalScrollYPos + m_nScrollArea) )
				{
					OnVScrollFromScrollbar( SB_LINEUP, (UINT)-1 );
				}
				else
				{
					OnVScrollFromScrollbar( SB_LINEDOWN, (UINT)-1 );
				}

				m_dwScrollTick += 300;
			}
		}
		else
		{
			m_dwScrollTick = 0;
		}
	}
	else
	{
		m_dwScrollTick = 0;
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( m_pITargetDataObject == NULL )
	{
		ASSERT( FALSE ); // Shouldn't happen - CanPasteFromData will return E_POINTER.
	}

	if( nLayerIndex >= 0
	&&  nLayerIndex <= m_lstLayers.GetCount() 
	&&  CanPasteFromData( m_pITargetDataObject ) == S_OK )
	{
		BOOL fCopy = FALSE;

		// Does m_pITargetDataObject contain format CF_DLS_WAVE or CF_DMUSPROD_FILE
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject )
		{
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfWave ) )
			||	SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfDMUSProdFile ) ) )
			{
				fCopy = TRUE;
			}
			pDataObject->Release();
		}

		// Can only copy CF_DLS_WAVE data!
		if( fCopy )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else
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
// CWaveStrip::DragLeave

HRESULT CWaveStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_dwScrollTick = 0;

	return CBaseStrip::DragLeave();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::Drop

HRESULT CWaveStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(grfKeyState);

	// Verify that we're passed the data object we're expecting
	if( (m_pITargetDataObject == NULL)
	||	(m_pITargetDataObject != pIDataObject) )
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}

	// Set default values
	*pdwEffect = DROPEFFECT_NONE;

	// Do nothing if strip is minimized
	// Do nothing if no variations are selected
	if( m_StripUI.m_nStripView == SV_MINIMIZED
	||  m_StripUI.m_dwVariationBtns == 0 )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		// Try and load the arrow cursor
		HCURSOR hCursor = ::LoadCursor( AfxGetInstanceHandle(), IDC_ARROW );
		if( hCursor )
		{
			// The the mouse cursor to an arrow cursor
			HCURSOR hCursorOld = ::SetCursor( hCursor );
	
			// Prepare context menu
			HMENU hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_DRAG_RMENU) );
			if( hMenu )
			{
				// Initialize the drop effect to DROPEFFECT_NONE
				m_dwDragRMenuEffect = DROPEFFECT_NONE;

				// Track right context menu for drag-drop via TrackPopupMenu
				HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );

				// Init state of menu items
				if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
				{
					::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
				}

				// Get a window to attach menu to
				HWND hwnd = GetTimelineHWND();
				if( hwnd )
				{
					// Display and track menu
	 				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
								  pt.x, pt.y, 0, hwnd, NULL );

					// Destroy the popup menu
					::DestroyMenu( hMenu );

					// Need to process WM_COMMAND from TrackPopupMenu
					MSG msg;
					while( ::PeekMessage( &msg, hwnd, NULL, NULL, PM_REMOVE) )
					{
						if( msg.message == WM_COMMAND )
						{
							OnWMMessage( msg.message, msg.wParam, msg.lParam, 0, 0 );
						}
						else
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
					}

					// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
					m_dwOverDragEffect = m_dwDragRMenuEffect;

					// Reset the RMenu's effect to DROPEFFECT_NONE
					m_dwDragRMenuEffect = DROPEFFECT_NONE;
				}
			}

			// Reset the cursor to its previous image
			::SetCursor( hCursorOld );

			// Don't destory the hCursor, since it is a shared cursor
		}
	}

	// Check that the user didn't cancel the drag-drop operation
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		// Allocate a Timleine DataObject
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pTrackMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			// Import the DataObject that was dragged
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				// Compute the time to drop at
				long lPasteTime;
				m_pTrackMgr->m_pTimeline->PositionToClocks( pt.x, &lPasteTime );
				m_lXPos = pt.x;
				m_lYPos = pt.y;

				// Actually paste the data
				BOOL fChanged = FALSE;
				hr = PasteAt( pITimelineDataObject, lPasteTime, true, fChanged );

				// Check that the paste succeeded, and that something was actually pasted
				if( SUCCEEDED ( hr ) && fChanged )
				{
					*pdwEffect = m_dwOverDragEffect;

					// Check if the target strip is different from the source strip
					if( m_nStripIsDragDropSource != 2 )
					{
						// Update the list of selected regions to include only the selected items
						SelectRegionsFromSelectedItems();

						if( CWaveStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the containing segment that a paste operation occurred
							m_pTrackMgr->m_nLastEdit = IDS_PASTE;
							m_pTrackMgr->OnDataChanged();
						}

						// Redraw our strip
						m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the track item's property page
						OnShowItemProperties();

						// Update the track item's property page, if it exists
						RefreshPropertyPage();

						// Sync with DirectMusic
						m_pTrackMgr->SyncWithDirectMusic();
					}
				}
			}

			// Release our reference to the Timeline DataObject we allocated
			pITimelineDataObject->Release();
		}
	}

	// Cleanup
	DragLeave();

	// Return the success/failure code
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetData

HRESULT CWaveStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the ppData pointer
	if( ppData == NULL )
	{
		return E_POINTER;
	}

	// Validate the pPropItem pointer
	CPropTrackItem* pPropItem = (CPropTrackItem *)*ppData;
	if( pPropItem == NULL )
	{
		return E_INVALIDARG;
	}

	// Initialize pPropItem
	pPropItem->Clear();

	// Initialize the pointer to the first selected item to NULL
	CTrackItem* pFirstSelectedTrackItem = NULL;

	// Start iterating through the item list
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			if( pFirstSelectedTrackItem == NULL )
			{
				// Save a pointer to first selected item in pFirstSelectedTrackItem
				pFirstSelectedTrackItem = pItem;

				// Copy values fom the selected item to CPropTrackItem
				pPropItem->m_dwBits &= ~PROPF_MULTIPLESELECT;
				pPropItem->CopyTrackItem( pFirstSelectedTrackItem );
			}
			else
			{
				// Merge values fom the selected item with existing values in CPropTrackItem
				pPropItem->m_dwBits |= PROPF_MULTIPLESELECT;
				pPropItem->MergeTrackItem( pItem );
			}
		}
	}

	// Were any items selected?
	if( pFirstSelectedTrackItem == NULL )
	{
		// Nothing selected, so clear the passed in pointer
		*ppData = NULL;
	}

	// Return a success code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetData

HRESULT CWaveStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the pData pointer
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Convert the passed-in pointer to a CPropTrackItem*
	CPropTrackItem* pPropItem = static_cast<CPropTrackItem*>(pData);
// TODO - Put in this ASSERT!!!	ASSERT( m_PropItem.m_dwChanged != 0 );

	BOOL fChanged = FALSE;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Store original variations
			DWORD dwOrigVariations = pItem->m_dwVariations;

			// Get pointer to the item to be updated
			CTrackItem* pTheItem;
			if( pPropItem->m_nPropertyTab == TAB_VARIATIONS
			&&  pPropItem->m_dwChanged == CHGD_VARIATIONS )
			{
				// Variations have changed so do not split into two items
				pTheItem = pItem;
			}
			else if( pItem->m_dwVariations & ~m_StripUI.m_dwVariationBtns )
			{
				// Item belongs to other variations so split it into two items
				pTheItem = new CTrackItem( m_pTrackMgr, this );
				if( pTheItem == NULL )
				{
					return E_OUTOFMEMORY;
				}
				pTheItem->Copy( pItem );
				pItem->m_dwVariations &= ~m_StripUI.m_dwVariationBtns;
				pTheItem->m_dwVariations &= m_StripUI.m_dwVariationBtns;
				InsertByAscendingTime( pTheItem, FALSE );
			}
			else
			{
				pTheItem = pItem;
			}

			BOOL fItemChanged = FALSE;

			switch( pPropItem->m_nPropertyTab )
			{
				case TAB_FILEREF:
					fItemChanged = SetData_TabFileRef( pPropItem, pTheItem );
					break;
				
				case TAB_LOOP:
					fItemChanged = SetData_TabLoop( pPropItem, pTheItem );
					break;
				
				case TAB_PERFORMANCE:
					fItemChanged = SetData_TabPerformance( pPropItem, pTheItem );
					break;

				case TAB_VARIATIONS:
					fItemChanged = SetData_TabVariations( pPropItem, pTheItem );
					break;

				default:
					ASSERT( 0 );	// Should not happen!
					break;
			}

			// Item did not change, so...
			if( fItemChanged == FALSE )
			{
				// Restore variations
				pItem->m_dwVariations = dwOrigVariations;

				// Remove pTheItem from the list
				if( pTheItem != pItem )
				{
					CTrackItem* pItemTmp;
					POSITION pos2, pos1 = m_lstTrackItems.GetHeadPosition();
					while( pos1 )
					{
						pos2 = pos1;
						pItemTmp = m_lstTrackItems.GetNext( pos1 );
						if( pItemTmp == pTheItem )
						{
							m_lstTrackItems.RemoveAt( pos2 );
							break;
						}
					}
				}
			}

			fChanged |= fItemChanged;
		}
	}

	// Always refresh the property page
	RefreshPropertyPage();

	// Check to see if one of the items changed
	if( fChanged )
	{
		// Redraw the strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );

		// Let our hosting editor know about the changes
		m_pTrackMgr->OnDataChanged();

		// Sync track with DirectMusic
		m_pTrackMgr->SyncWithDirectMusic();

		return S_OK;
	}

	// Nothing changed
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetData_TabFileRef

BOOL CWaveStrip::SetData_TabFileRef( CPropTrackItem* pPropItem, CTrackItem* pItem )
{
	BOOL fChanged = FALSE;

	switch( pPropItem->m_dwChanged )
	{
		case CHGD_DOCROOT:
			if( pPropItem->m_Item.m_FileRef.pIDocRootNode != pItem->m_FileRef.pIDocRootNode )
			{
				pItem->SetFileReference( pPropItem->m_Item.m_FileRef.pIDocRootNode );
				pItem->m_fLoopedUI = pItem->m_WaveInfo.fIsLooped;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_CHANGE;
				fChanged = TRUE;
			}
			break;

		case CHGD_TIME_OR_DURATION:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_rtTimePhysical != pItem->m_rtTimePhysical )
			{
				// Update the item's time
				pItem->SetTimePhysical( pPropItem->m_Item.m_rtTimePhysical, STP_LOGICAL_ADJUST );
				pItem->m_rtDuration = pPropItem->m_Item.m_rtDuration;

				// Update m_fSyncDuration flag
				REFERENCE_TIME rtActualLength;
				pItem->SourceWaveLengthToUnknownTime( m_pTrackMgr, &rtActualLength );
				pItem->m_fSyncDuration = (pItem->m_rtDuration == rtActualLength) ? TRUE : FALSE;

				// We just moved the item
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_MOVE;

				// Remove the TrackItem from the list
				CTrackItem* pItemTmp;
				POSITION pos2, pos1 = m_lstTrackItems.GetHeadPosition();
				while( pos1 )
				{
					pos2 = pos1;
					pItemTmp = m_lstTrackItems.GetNext( pos1 );
					if( pItemTmp == pItem )
					{
						m_lstTrackItems.RemoveAt( pos2 );
						break;
					}
				}

				// Re-add the item at its new position - this will overwrite any existing
				// item at this position
				InsertByAscendingTime( pItem, FALSE );

				// Clear all selections
				m_pSelectedRegions->Clear();

				// Select just the changed time of item
				CListSelectedRegion_AddRegion( pItem );
				fChanged = TRUE;
			}
			else if( pPropItem->m_Item.m_rtDuration != pItem->m_rtDuration )
			{
				pItem->m_rtDuration = pPropItem->m_Item.m_rtDuration;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_DURATION;

				// Update m_fSyncDuration flag
				REFERENCE_TIME rtActualLength;
				pItem->SourceWaveLengthToUnknownTime( m_pTrackMgr, &rtActualLength );
				pItem->m_fSyncDuration = (pItem->m_rtDuration == rtActualLength) ? TRUE : FALSE;
				fChanged = TRUE;
			}
			break;

		case CHGD_START_OFFSET:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_rtStartOffset != pItem->m_rtStartOffset )
			{
				pItem->m_rtStartOffset = pPropItem->m_Item.m_rtStartOffset;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_START_OFFSET;
				// We must unload/download wave start ofset changes
				m_pTrackMgr->QueueWaveForUnload( pItem );
				m_pTrackMgr->UnloadQueuedWaves();
				m_pTrackMgr->QueueWaveForDownload( pItem );
				fChanged = TRUE;
			}
			break;

		case CHGD_LOCK_END:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_fLockEndUI != pItem->m_fLockEndUI ) 
			{
				pItem->m_fLockEndUI = pPropItem->m_Item.m_fLockEndUI;
				if( pItem->m_fLockEndUI )
				{
					pItem->m_fLockLengthUI = FALSE;
				}
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOCK_END;
				fChanged = TRUE;
			}
			break;

		case CHGD_LOCK_LENGTH:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_fLockLengthUI != pItem->m_fLockLengthUI ) 
			{
				pItem->m_fLockLengthUI = pPropItem->m_Item.m_fLockLengthUI;
				if( pItem->m_fLockLengthUI )
				{
					pItem->m_fLockEndUI = FALSE;
				}
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOCK_LENGTH;
				fChanged = TRUE;
			}
			break;

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetData_TabLoop

BOOL CWaveStrip::SetData_TabLoop( CPropTrackItem* pPropItem, CTrackItem* pItem )
{
	BOOL fChanged = FALSE;

	switch( pPropItem->m_dwChanged )
	{
		case CHGD_IS_LOOPED:
			if( pPropItem->m_Item.m_fLoopedUI != pItem->m_fLoopedUI )
			{
				if( pPropItem->m_Item.m_fLoopedUI == FALSE
				&&  pItem->m_fLoopedUI == TRUE )
				{
					// Turning off loop
					// Reset duration if it is longer than the wave
					REFERENCE_TIME rtActualLength;
					pItem->SourceWaveLengthToUnknownTime( m_pTrackMgr, &rtActualLength );
					if( pItem->m_rtDuration > rtActualLength )
					{
						pItem->m_rtDuration = rtActualLength;
						pItem->m_fSyncDuration = TRUE;
					}
				}
				pItem->m_fLoopedUI = pPropItem->m_Item.m_fLoopedUI;
				pItem->m_dwLoopStartUI = pPropItem->m_Item.m_dwLoopStartUI;
				pItem->m_dwLoopEndUI = pPropItem->m_Item.m_dwLoopEndUI;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOOP;
				fChanged = TRUE;
			}
			break;

		case CHGD_LOOP:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_dwLoopStartUI != pItem->m_dwLoopStartUI 
			||  pPropItem->m_Item.m_dwLoopEndUI != pItem->m_dwLoopEndUI )
			{
				pItem->m_fLoopedUI = pPropItem->m_Item.m_fLoopedUI;
				pItem->m_dwLoopStartUI = pPropItem->m_Item.m_dwLoopStartUI;
				pItem->m_dwLoopEndUI = pPropItem->m_Item.m_dwLoopEndUI;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOOP;
				fChanged = TRUE;
			}
			break;

		case CHGD_LOOP_LOCK_LENGTH:
			ASSERT( !(pPropItem->m_dwBits & PROPF_MULTIPLESELECT) );	// Should not happen (yet)!
			if( pPropItem->m_Item.m_fLockLoopLengthUI != pItem->m_fLockLoopLengthUI ) 
			{
				pItem->m_fLockLoopLengthUI = pPropItem->m_Item.m_fLockLoopLengthUI;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOCK_LOOP_LENGTH;
				fChanged = TRUE;
			}
			break;

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetData_TabPerformance

BOOL CWaveStrip::SetData_TabPerformance( CPropTrackItem* pPropItem, CTrackItem* pItem )
{
	BOOL fChanged = FALSE;

	switch( pPropItem->m_dwChanged )
	{
		case CHGD_VOLUME:
			if( pItem->m_lVolume != pPropItem->m_Item.m_lVolume )
			{
				pItem->m_lVolume = pPropItem->m_Item.m_lVolume;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_VOLUME;
				fChanged = TRUE;
			}
			break;
		
		case CHGD_PITCH:
			if( pItem->m_lPitch != pPropItem->m_Item.m_lPitch )
			{
				pItem->m_lPitch = pPropItem->m_Item.m_lPitch;
				// Changing fine tune will change durations
				// Call GetWaveInfo() to force recalc of durations
//				pItem->GetWaveInfo();	
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_PITCH;
				fChanged = TRUE;
			}
			break;
		case CHGD_VOLUME_RANGE:
			if( pItem->m_lVolumeRange != pPropItem->m_Item.m_lVolumeRange )
			{
				pItem->m_lVolumeRange = pPropItem->m_Item.m_lVolumeRange;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_VOLUME_RANGE;
				fChanged = TRUE;
			}
			break;
		
		case CHGD_PITCH_RANGE:
			if( pItem->m_lPitchRange != pPropItem->m_Item.m_lPitchRange )
			{
				pItem->m_lPitchRange = pPropItem->m_Item.m_lPitchRange;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_PITCH_RANGE;
				fChanged = TRUE;
			}
			break;

		case CHGD_DM_FLAGS:
			if( pItem->m_dwFlagsDM != pPropItem->m_Item.m_dwFlagsDM )
			{
				pItem->m_dwFlagsDM = pPropItem->m_Item.m_dwFlagsDM;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_FLAGS;
				fChanged = TRUE;
			}
			break;

		case CHGD_TIME_LOGICAL:
			if( pItem->m_rtTimeLogical != pPropItem->m_Item.m_rtTimeLogical )
			{
				pItem->SetTimeLogical( pPropItem->m_Item.m_rtTimeLogical );
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_LOGICAL_TIME;
				fChanged = TRUE;
			}
			break;

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetData_TabVariations

BOOL CWaveStrip::SetData_TabVariations( CPropTrackItem* pPropItem, CTrackItem* pItem )
{
	BOOL fChanged = FALSE;

	switch( pPropItem->m_dwChanged )
	{
		case CHGD_VARIATIONS:
			if( pItem->m_dwVariations != pPropItem->m_dwChangedVar_TabVariations )
			{
				// Previous check ensures an item's last variation is not removed
				DWORD dwNewVariations = pItem->m_dwVariations & ~pPropItem->m_dwChangedVar_TabVariations;
				dwNewVariations |= pPropItem->m_Item.m_dwVariations & pPropItem->m_dwChangedVar_TabVariations;
				if( pItem->m_dwVariations != dwNewVariations )
				{
					pItem->m_dwVariations = dwNewVariations;
					m_pTrackMgr->m_nLastEdit = IDS_UNDO_VARIATIONS;
					fChanged = TRUE;
				}
			}
			break;

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnShowProperties

HRESULT STDMETHODCALLTYPE CWaveStrip::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL 
	||  m_pTrackMgr->m_pDMProdFramework == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	if( m_fShowItemProps == FALSE )
	{
		OnShowStripProperties();
		return S_OK;
	}

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	m_pTrackMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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

	// If item property page manager already in propsheet, refresh and exit
	if( pIPropSheet->IsEqualPageManagerObject( (IDMUSProdPropPageObject*)this ) == S_OK )
	{
		ASSERT( m_pPropPageMgr != NULL );
		RefreshPropertyPage();
		pIPropSheet->Release();
		return S_OK;
	}

	// release our reference to the property sheet
	pIPropSheet->Release();

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		// Create a new "item" property page manager
		CPropPageMgrItem* pPPM = new CPropPageMgrItem( m_pTrackMgr->m_pDMProdFramework, m_pTrackMgr );

		// Verify that we're not out of memory
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Get the IDMUSProdPropPageManager interface from the property page
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		// Release the reference created by the contructor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		// If we failed, return a failure code
		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Store the last active tab
	short nActiveTab = CPropPageMgrItem::sm_nActiveTab;

	// Set the displayed property page to our property page
	m_pTrackMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Set the active tab
	if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		CWnd* pWndHadFocus = CWnd::GetFocus();

		pIPropSheet->SetActivePage( nActiveTab ); 
		RELEASE( pIPropSheet );

		if( pWndHadFocus )
		{
			pWndHadFocus->SetFocus();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CWaveStrip::OnRemoveFromPageManager( void )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CanPasteFromData

HRESULT CWaveStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	// Do nothing if strip is minimized
	// Do nothing if no variations are selected
	if( m_StripUI.m_nStripView == SV_MINIMIZED
	||  m_StripUI.m_dwVariationBtns == 0 )
	{
		return S_FALSE;
	}

	// Check that we're passed a valid IDataObject pointer
	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Get a Timeline DataObject that encapsulates the pIDataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject = NULL;
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, m_pTrackMgr->m_pTimeline, pIDataObject );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if( FAILED(hr)
	||	(pITimelineDataObject == NULL) )
	{
		return hr;
	}

	// Check the timeline data object for our clipboard formats
	hr = S_FALSE;

	// CF_WAVETRACK
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfWaveTrack ) == S_OK )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}
	// CF_DLS_WAVE
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfWave ) == S_OK )
	{
		IDataObject* pIDataObject;
		if( SUCCEEDED ( pITimelineDataObject->Export( &pIDataObject ) ) )
		{
			IDMUSProdNode* pIDocRootNode;
			if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
			{
				// Cannot drop waves from DLS Collections
				// Existence of DocRoot means this is a wave file
				hr = S_OK;

				RELEASE( pIDocRootNode );
			}

			RELEASE( pIDataObject );
		}
	}
	// CF_DMUSPROD_FILE
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfDMUSProdFile ) == S_OK )
	{
		IStream *pStream;
		if( SUCCEEDED( pITimelineDataObject->AttemptRead( m_cfDMUSProdFile, &pStream ) ) )
		{
			CFProducerFile cfProducerFile;
			if( SUCCEEDED( LoadCF_DMUSPROD_FILE( pStream, &cfProducerFile ) ) )
			{
				int nDot = cfProducerFile.strFileName.ReverseFind( _T('.') );
				if( nDot >= 0 )
				{
					CString strExt = cfProducerFile.strFileName.Right( cfProducerFile.strFileName.GetLength() - nDot );
					if( 0 == strExt.CompareNoCase( _T(".wav") )
					||	0 == strExt.CompareNoCase( _T(".wvp") ) )
					{
						// Can drop .wav and .wvp files
						hr = S_OK;
					}
				}
			}
			pStream->Release();
		}
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::XYPosToVariation

int	CWaveStrip::XYPosToVariation( long lXPos, long lYPos )
{
	int nVariation = -1;

	lXPos -= VARIATION_GUTTER_WIDTH;

	if( lXPos > 0
	&&  lXPos < VARIATION_BUTTONS_WIDTH
	&&  lYPos > 0
	&&  lYPos < VARIATION_BUTTONS_HEIGHT )
	{
		nVariation = lXPos / VARIATION_BUTTON_WIDTH;

		if( (lYPos / VARIATION_BUTTON_HEIGHT) > 0 )
		{
			nVariation += 16;
		}

		if( nVariation < 0
		||  nVariation > 31 )
		{
			ASSERT( 0 );
			nVariation = -1;
		}
	}

	return nVariation;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonDblClkVarBtns

HRESULT CWaveStrip::OnLButtonDblClkVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	// Handle gutter
	if( lXPos <= VARIATION_GUTTER_WIDTH )
	{
		return OnLButtonDownVarBtns( wParam, lParam, lXPos, lYPos );
	}

	// Handle variation buttons
	else
	{
		int nVariation = XYPosToVariation( lXPos, lYPos );
		if( nVariation != -1 )
		{
			// Go through soloing logic
			if( m_fVarSolo )
			{
				m_fVarSolo = FALSE;
				m_StripUI.m_dwVariationBtns = m_StripUI.m_dwOldVariationBtns;
			}
			else
			{
				m_fVarSolo = TRUE;
				m_nSoloVariation = nVariation;
				m_StripUI.m_dwVariationBtns ^= 1 << m_nSoloVariation;
				m_StripUI.m_dwOldVariationBtns = m_StripUI.m_dwVariationBtns;
				m_StripUI.m_dwVariationBtns = 1 << m_nSoloVariation;
			}

			// See if we have to pop up the gutter
			if( (m_StripUI.m_dwVariationBtns & m_dwVariationsMask) == m_dwVariationsMask )
			{
				m_StripUI.m_fVarGutterDown = TRUE;
			}
			else
			{
				m_StripUI.m_fVarGutterDown = FALSE;
			}

			// Redraw our strip
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

			// Notify the containing segment that we changed
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_SET_VARIATIONS;
			m_pTrackMgr->OnDataChanged();
			m_pTrackMgr->SetAuditionVariations( false );

			// Update the selection regions to include the selected items
			SelectRegionsFromSelectedItems();

			// If visible, switch the property sheet to the track item property page
			OnShowItemProperties();

			// If it exists, refresh the track item property page
			RefreshPropertyPage();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonDownVarBtns

HRESULT CWaveStrip::OnLButtonDownVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	// Handle gutter
	if( lXPos <= VARIATION_GUTTER_WIDTH )
	{
		m_StripUI.m_fVarGutterDown = !m_StripUI.m_fVarGutterDown;

		DWORD dwOrigVariations = m_StripUI.m_dwVariationBtns;
		if( m_StripUI.m_fVarGutterDown )
		{
			// Select all active variations
			m_StripUI.m_dwVariationBtns |= m_dwVariationsMask;
		}
		else
		{
			// Deselect all active variations
			m_StripUI.m_dwVariationBtns &= ~m_dwVariationsMask;
		}

		InvalidateVariationBtns();
		
		if( m_StripUI.m_dwVariationBtns != dwOrigVariations )
		{
			InvalidateWaves();

			// Notify the containing segment that we changed
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_SET_VARIATIONS;
			m_pTrackMgr->OnDataChanged();
			m_pTrackMgr->SetAuditionVariations( false );

			// Update the selection regions to include the selected items
			SelectRegionsFromSelectedItems();

			// If visible, switch the property sheet to the track item property page
			OnShowItemProperties();

			// If it exists, refresh the track item property page
			RefreshPropertyPage();
		}
	}

	// Handle variation buttons
	else
	{
		int nVariation = XYPosToVariation( lXPos, lYPos );
		if( nVariation != -1 )
		{
			m_nLastVariation = nVariation;
			if( m_nLastVariation != m_nSoloVariation )
			{
				m_fVarSolo = FALSE;
			}

			// Toggle variation button
			m_StripUI.m_dwVariationBtns ^= (1 << m_nLastVariation);

			// If button is down we are enabling
			if( (1 << m_nLastVariation) & m_StripUI.m_dwVariationBtns )
			{
				m_nVarAction = VA_ENABLING;
			}
			else
			{
				m_nVarAction = VA_DISABLING;
			}

			// See if we need to pop up gutter
			if( (m_StripUI.m_dwVariationBtns & m_dwVariationsMask) == m_dwVariationsMask )
			{
				m_StripUI.m_fVarGutterDown = true;
			}
			else
			{
				m_StripUI.m_fVarGutterDown = false;
			}

			// Redraw our strip
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

			// Update the selection regions to include the selected items
			SelectRegionsFromSelectedItems();

			// If visible, switch the property sheet to the track item property page
			OnShowItemProperties();

			// If it exists, refresh the track item property page
			RefreshPropertyPage();

			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonDownZoomBtns

HRESULT CWaveStrip::OnLButtonDownZoomBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);

	// Capture the mouse
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Set the zoom timer
	HWND hwnd = GetTimelineHWND();
	if( hwnd )
	{
		::SetTimer( hwnd, TIMER_ZOOM_1, 400, NULL );
	}

	// Change the zoom factor
	if( lYPos <= m_StripUI.m_lVerticalScrollYPos + sm_nZoomBtnHeight )
	{
		m_fZoomInDown = true;
		InvalidateFBar();  
		SetZoom( m_StripUI.m_dblVerticalZoom + 0.01 );
	}
	else
	{
		m_fZoomOutDown = true;
		InvalidateFBar();  
		if( m_StripUI.m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
		{
			SetZoom( m_StripUI.m_dblVerticalZoom - 0.01 );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonDownLayerBtns

HRESULT CWaveStrip::OnLButtonDownLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture the mouse
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	CWaveStripLayer* pLayer = YPosToLayer( lYPos, false );
	if( pLayer == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Check if the shift key is pressed
	if( wParam & MK_SHIFT )
	{
		UnselectAll();

		// Figure out range
		int nStartIndex;
		int nEndIndex;
		if( m_pLayerForShiftClick == NULL )
		{
			nStartIndex = 0;
		}
		else
		{
			nStartIndex = LayerToIndex( m_pLayerForShiftClick );
		}
		nEndIndex = LayerToIndex( pLayer );
		if( nStartIndex > nEndIndex )
		{
			int nTempIndex = nStartIndex;
			nStartIndex = nEndIndex;
			nEndIndex = nTempIndex;
		}

		// Select layers in range
		POSITION pos = m_lstLayers.FindIndex( nStartIndex );
		while( pos )
		{
			CWaveStripLayer* pLayerList = m_lstLayers.GetNext( pos );

			pLayerList->m_fSelected = TRUE;
			m_fLayerSelected = pLayerList->m_fSelected;

			// Select/unselect waves in this layer
			SelectLayer( pLayerList );

			if( ++nStartIndex > nEndIndex )
			{
				break;
			}
		}
	}
	else
	{
		// Check if the control key is down
		if( wParam & MK_CONTROL )
		{
			pLayer->m_fSelected = !pLayer->m_fSelected;
			m_fLayerSelected = pLayer->m_fSelected;

			// Select/unselect waves in this layer
			SelectLayer( pLayer );
		}
		// No control key (works same as control key path, however)
		else 
		{
			pLayer->m_fSelected = !pLayer->m_fSelected;
			m_fLayerSelected = pLayer->m_fSelected;

			// Select/unselect waves in this layer
			SelectLayer( pLayer );
		}

		// Set anchor for future shift-select operations
		m_pLayerForShiftClick = pLayer;
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the track item's property page
	OnShowItemProperties();

	// Refresh the property page, if it exists
	RefreshPropertyPage();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonDown

HRESULT CWaveStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lParam);

	// Validate our timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Initialize the item to toggle to NULL
	m_pItemToToggle = NULL;
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Check if the shift key is pressed
	if( wParam & MK_SHIFT )
	{
		CListSelectedRegion_ShiftAddRegion( lXPos );
		SelectItemsInSelectedRegions();
	}
	else
	{
		// See if there is an item under the cursor.
		CTrackItem* pItem = GetItemFromPoint( lXPos, lYPos );
		if( pItem )
		{
			// Found an item under the cursor

			// Check if the control key is down
			if( wParam & MK_CONTROL )
			{
				// Check if the item is not yet selected
				if( pItem->m_fSelected )
				{
					// Set up to unselect this item later, either when we receive a
					// left-button up, or when the user completes a drag-drop operation
					// that does nothing.
					m_pItemToToggle = pItem;
				}
				else
				{
					// Mark the clicked on item as selected
					pItem->m_fSelected = TRUE;
					CListSelectedRegion_AddRegion( lXPos );
				}
			}
			// Check if the item is unselected (the shift key is up)
			else if( !pItem->m_fSelected )
			{
				// Mark the clicked on item as selected
				UnselectAll();
				pItem->m_fSelected = TRUE;

				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedItems();
			}
		}
		// Didn't find an item - check if the Ctrl key is down
		else if( wParam & MK_CONTROL )
		{
			// Toggle the selection state in the region under the cursor
			CListSelectedRegion_ToggleRegion( lXPos );
		}
		// The shift key is up
		else
		{
			// The user left-clicked on blank space without holding the shift or Ctrl keys down

			// Clear all selection regions
			m_pSelectedRegions->Clear();
			UnselectAll();

			// Select only the beat clicked on
			CListSelectedRegion_AddRegion( lXPos );
		}

		// Set anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( lXPos );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Switch the property page to the track item's property page
	OnShowItemProperties();

	// Refresh the property page, if it exists
	RefreshPropertyPage();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonUpVarBtns

HRESULT CWaveStrip::OnLButtonUpVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Notify the containing segment that we changed
	if( m_nVarAction == VA_ENABLING
	||  m_nVarAction == VA_DISABLING )
	{
		m_pTrackMgr->m_nLastEdit = IDS_UNDO_SET_VARIATIONS;
		m_pTrackMgr->OnDataChanged();
		m_pTrackMgr->SetAuditionVariations( false );
	}

	// Reset flags
	m_nLastVariation = -1;
	m_nVarAction = VA_NONE;

	// Release mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonUpZoomBtns

HRESULT CWaveStrip::OnLButtonUpZoomBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Notify the containing segment that we changed
// TODO - Do we need this?
//	m_pTrackMgr->m_nLastEdit = IDS_UNDO_SET_VARIATIONS;
//	m_pTrackMgr->OnDataChanged();

	// Release mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Kill the zoom timer
	HWND hwnd = GetTimelineHWND();
	if( hwnd )
	{
		::KillTimer( hwnd, TIMER_ZOOM_1 );
		::KillTimer( hwnd, TIMER_ZOOM_2 );
	}

	// Reset button state flags
	m_fZoomInDown = false;
	m_fZoomOutDown = false;

	// Redraw function bar area
	InvalidateFBar();  

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonUpLayerBtns

HRESULT CWaveStrip::OnLButtonUpLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Release mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Kill the vertical scroll timer
	HWND hwnd = GetTimelineHWND();
	if( hwnd )
	{
		::KillTimer( hwnd, TIMER_VSCROLL_LAYER_1 );
		::KillTimer( hwnd, TIMER_VSCROLL_LAYER_2 );
		m_nVerticalScrollTimer = 0;
	}

	// Redraw function bar area
	InvalidateFBar();  

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnLButtonUp

HRESULT CWaveStrip::OnLButtonUp( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lParam);

	// Check if the Ctrl key was pressed
	if( wParam & MK_CONTROL )
	{
		// Ctrl key pressed - toggle the selection state of the item that was clicked on
		if( m_pItemToToggle )
		{
			m_pItemToToggle->m_fSelected = !m_pItemToToggle->m_fSelected;
			m_pItemToToggle = NULL;

			// Update the selection regions to include just this selected item
			SelectRegionsFromSelectedItems();
		}
	}
	// Check if the Shift key was not pressed
	else if( !(wParam & MK_SHIFT) )
	{
		// Look for an item at the position clicked on
		CTrackItem* pItem = GetItemFromPoint( lXPos, lYPos );
		if(pItem)
		{
			// Found an item
			// Unselect all items
			UnselectAll();

			// Mark the clicked on item as selected
			pItem->m_fSelected = TRUE;

			// Update the selection regions to include just this selected item
			SelectRegionsFromSelectedItems();
		}
	}


	// Ensure all other strips are unselected
	UnselectGutterRange();

	// Redraw ourself
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	// Refresh the track item property page, if it exists
	RefreshPropertyPage();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRButtonDownVarBtns

HRESULT CWaveStrip::OnRButtonDownVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRButtonDownLayerBtns

HRESULT CWaveStrip::OnRButtonDownLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	
	// Unselect all items in the other strips
	UnselectGutterRange();

	CWaveStripLayer* pLayer = YPosToLayer( lYPos, false );
	if( pLayer == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Check if the item is unselected (the shift key is up)
	if( pLayer->m_fSelected == FALSE )
	{
		UnselectAll();

		pLayer->m_fSelected = TRUE;
		m_fLayerSelected = pLayer->m_fSelected;

		// Select/unselect waves in this layer
		SelectLayer( pLayer );

		// Redraw our strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

		// Switch the property page to the track item's property page
		OnShowItemProperties();

		// Refresh the property page, if it exists
		RefreshPropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRButtonDown

HRESULT CWaveStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lParam);
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	CTrackItem* pItem = GetItemFromPoint( lXPos, lYPos );

	if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		// Neither control or shift were pressed

		// Check if an item was clicked on
		if( pItem )
		{
			// Check if the item is selected
			if( pItem->m_fSelected == FALSE )
			{
				// Item is not selected, unselect all items
				UnselectAll();

				// Select just this item
				pItem->m_fSelected = TRUE;

				// Add just this beat to the list of selected regions
				CListSelectedRegion_AddRegion( lXPos );
			}
			// If the item was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure and beat
			REFERENCE_TIME rtSnapTime;
			if( SUCCEEDED ( SnapXPosToUnknownTime( lXPos, &rtSnapTime ) ) )
			{
				// Check if this beat is not already selected
				if( !CListSelectedRegion_Contains(rtSnapTime))
				{
					// This beat is not selected - unselect all beats
					UnselectAll();

					// Now, select just this beat
					CListSelectedRegion_AddRegion( lXPos );
				}
			}
		}
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch the property page to the track item's property page
	OnShowItemProperties();

	// Refresh the  property page, if it exists
	RefreshPropertyPage();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRButtonUpVarBtns

HRESULT CWaveStrip::OnRButtonUpVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lYPos);

	// Validate our timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	if( (lXPos > VARIATION_GUTTER_WIDTH)
	&&  (lXPos < VARIATION_GUTTER_WIDTH + VARIATION_BUTTONS_WIDTH) )
	{
		// Get the cursor position (To put the menu there)
		POINT pt;
		BOOL bResult = GetCursorPos( &pt );
		ASSERT( bResult );
		if( bResult == FALSE )
		{
			return E_UNEXPECTED;
		}

		HMENU hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_VARBTNS_RMENU) );
		if( hMenu == NULL )
		{
			return E_UNEXPECTED;
		}

		HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
		if( hMenuPopup == NULL )
		{
			return E_UNEXPECTED;
		}

		m_pTrackMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );

		::DestroyMenu( hMenu );	// This will destroy the submenu as well.
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnRButtonUpLayerBtns

HRESULT CWaveStrip::OnRButtonUpLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Validate our timeline pointer
	if( (m_pTrackMgr == NULL)
	||	(m_pTrackMgr->m_pTimeline == NULL) )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Get the cursor position (To put the menu there)
	POINT pt;
	BOOL bResult = GetCursorPos( &pt );
	ASSERT( bResult );
	if( bResult == FALSE )
	{
		return E_UNEXPECTED;
	}

	HMENU hMenu = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDM_LAYERBTNS_RMENU) );
	if( hMenu == NULL )
	{
		return E_UNEXPECTED;
	}

	HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
	if( hMenuPopup == NULL )
	{
		return E_UNEXPECTED;
	}

	::EnableMenuItem( hMenuPopup, IDM_DELETE_LAYERS, ( CanDeleteLayers() == S_OK ) ? MF_ENABLED : MF_GRAYED );
	m_pTrackMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );

	::DestroyMenu( hMenu );	// This will destroy the submenu as well.

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnMouseMoveVarBtns

HRESULT CWaveStrip::OnMouseMoveVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if( m_nVarAction == VA_ENABLING
	||  m_nVarAction == VA_DISABLING )
	{
		int nVariation = XYPosToVariation( lXPos, lYPos );
		if( nVariation != -1 )
		{
			m_nLastVariation = nVariation;
			if( m_nVarAction == VA_ENABLING )
			{
				m_StripUI.m_dwVariationBtns |= 1 << m_nLastVariation;
			}
			else
			{
				m_StripUI.m_dwVariationBtns &= ~(1 << m_nLastVariation);
			}

			if( (m_StripUI.m_dwVariationBtns & m_dwVariationsMask) == m_dwVariationsMask )
			{
				m_StripUI.m_fVarGutterDown = true;
			}
			else
			{
				m_StripUI.m_fVarGutterDown = false;
			}

			// Redraw our strip
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

			// Update the selection regions to include the selected items
			SelectRegionsFromSelectedItems();

			// If visible, switch the property sheet to the track item property page
			OnShowItemProperties();

			// If it exists, refresh the track item property page
			RefreshPropertyPage();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnMouseMoveLayerBtns

HRESULT CWaveStrip::OnMouseMoveLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);

	// Set the vertical scroll timer
	if( (lYPos - VARIATION_BUTTONS_HEIGHT) < m_StripUI.m_lVerticalScrollYPos 
	||  lYPos > (m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight) )
	{
		if( m_nVerticalScrollTimer == 0 )
		{
			HWND hwnd = GetTimelineHWND();
			if( hwnd )
			{
				::SetTimer( hwnd, TIMER_VSCROLL_LAYER_1, 400, NULL );
				m_nVerticalScrollTimer = TIMER_VSCROLL_LAYER_1;
			}
		}

		return S_OK;
	}

	// Kill the vertical scroll timer
	HWND hwnd = GetTimelineHWND();
	if( hwnd )
	{
		::KillTimer( hwnd, TIMER_VSCROLL_LAYER_1 );
		::KillTimer( hwnd, TIMER_VSCROLL_LAYER_2 );
		m_nVerticalScrollTimer = 0;
	}

	// Select the layer button
	CWaveStripLayer* pLayer = YPosToLayer( lYPos, false );
	if( pLayer )
	{
		if( pLayer->m_fSelected != m_fLayerSelected )
		{
			pLayer->m_fSelected = m_fLayerSelected;

			// Select/unselect waves in this layer
			if( SelectLayer(pLayer) )
			{
				// Update the selection regions to include just this selected item
				SelectRegionsFromSelectedItems();

				// Redraw our strip
				m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
			}
			else
			{
				// Redraw function bar area
				InvalidateFBar();  
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnMouseMove

HRESULT CWaveStrip::OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	HRESULT hr = S_OK;

	// Check if the left mouse button is down
	if( m_fLeftMouseDown )
	{
		// The user moved the mouse while the left mouse button was down -
		// do a drag-drop operation.
		hr = DoDragDrop( m_pTrackMgr->m_pTimeline, wParam, m_lXPos, m_lYPos );

		// The above method returns after a drop, or after the user
		// cancels the operation.  In either case, we don't want to do
		// a drag-drop operation again.
		m_fLeftMouseDown = false;

		// If the drag-drop operatoin didn't complete
		if( hr != S_OK || m_dwDropEffect == DROPEFFECT_NONE )
		{
			// Store the position that the drag started at, because drag drop eats mouse up
			m_lXPos = lXPos;
			m_lYPos = lYPos;

			// Check if the control key was down
			if( wParam & MK_CONTROL )
			{
				// Finish the toggle operation on the item that was clicked on
				// when the left mouse button was pressed.
				if( m_pItemToToggle )
				{
					m_pItemToToggle->m_fSelected = !m_pItemToToggle->m_fSelected;
					m_pItemToToggle = NULL;

					// Update the selection regions to include just this selected item
					SelectRegionsFromSelectedItems();
				}
			}
			// Check if the shift key was not down
			else if( !(wParam & MK_SHIFT) )
			{
				// If an item was clicked on
				if(GetItemFromPoint( lXPos, lYPos ))
				{
					// Update the selection regions to include only the selected items
					SelectRegionsFromSelectedItems();
				}
			}

			// Redraw the strip
			m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

			// Switch the property page to the track item property page
			OnShowItemProperties();
		}
		else
		{
			// successful drag drop--make sure that only the regions with
			// selected items are selected
			SelectRegionsFromSelectedItems();
		}

		// Refresh the track item property page, if it exists
		RefreshPropertyPage();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnChar

HRESULT CWaveStrip::OnChar( WPARAM wParam )
{
	if( m_StripUI.m_nStripView == SV_NORMAL )
	{
		if( (wParam == 'a')
		||  (wParam == 'A') )
		{
			SetZoom( m_StripUI.m_dblVerticalZoom + 0.01);
		}
		else if( (wParam == 'z')
			 ||  (wParam == 'Z') )
		{
			if( m_StripUI.m_dblVerticalZoom > MINIMUM_ZOOM_LEVEL )
			{
				SetZoom( m_StripUI.m_dblVerticalZoom - 0.01);
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::PostRightClickMenu

HRESULT CWaveStrip::PostRightClickMenu( POINT pt )
{
	m_fInRightClickMenu = TRUE;

	// Load menu
	HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

	// Initialize menu
	::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
			MF_GRAYED | MF_BYPOSITION );
	::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( IsEmpty() ) ? MF_GRAYED :
			MF_ENABLED );
	::EnableMenuItem( hMenuPopup, IDM_MERGE_VARIATIONS,  m_StripUI.m_dwVariationBtns ? MF_ENABLED : MF_GRAYED );
	::EnableMenuItem( hMenuPopup, IDM_ZOOM_IN, ( m_StripUI.m_nStripView == SV_NORMAL ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, IDM_ZOOM_OUT, ( m_StripUI.m_nStripView == SV_NORMAL ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );

	// Initialize 'Snap To' popup menu
	HMENU hMenuPopupSnapTo = ::GetSubMenu( hMenuPopup, 8 );	// 8 is index of 'Snap To' popup menu
	ASSERT( hMenuPopupSnapTo != NULL );
	if( hMenuPopupSnapTo )
	{
		CString strMenuText;
		if( m_pTrackMgr->IsRefTimeTrack() )
		{
			strMenuText.LoadString( IDS_SECOND_TEXT );
			::InsertMenu( hMenuPopupSnapTo, 2, MF_BYPOSITION | MF_STRING, IDM_SNAP_SECOND, strMenuText ); 
			::InsertMenu( hMenuPopupSnapTo, 3, MF_BYPOSITION | MF_SEPARATOR, 0, NULL ); 
			strMenuText.LoadString( IDS_SET_GRID_TEXT );
			::InsertMenu( hMenuPopupSnapTo, 4, MF_BYPOSITION | MF_STRING, IDM_SNAP_SET_GRID, strMenuText ); 

			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_NONE,
				(m_StripUI.m_nSnapToRefTime == IDM_SNAP_NONE) ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_GRID,
				(m_StripUI.m_nSnapToRefTime == IDM_SNAP_GRID) ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_SECOND,
				(m_StripUI.m_nSnapToRefTime == IDM_SNAP_SECOND) ? MF_CHECKED : MF_UNCHECKED );
		}
		else
		{
			strMenuText.LoadString( IDS_BEAT_MENU_TEXT );
			::InsertMenu( hMenuPopupSnapTo, 2, MF_BYPOSITION | MF_STRING, IDM_SNAP_BEAT, strMenuText ); 
			strMenuText.LoadString( IDS_BAR_MENU_TEXT );
			::InsertMenu( hMenuPopupSnapTo, 3, MF_BYPOSITION | MF_STRING, IDM_SNAP_BAR, strMenuText ); 

			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_NONE,
				(m_StripUI.m_nSnapToMusicTime == IDM_SNAP_NONE) ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_GRID,
				(m_StripUI.m_nSnapToMusicTime == IDM_SNAP_GRID) ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_BEAT,
				(m_StripUI.m_nSnapToMusicTime == IDM_SNAP_BEAT) ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenuPopupSnapTo, IDM_SNAP_BAR,
				(m_StripUI.m_nSnapToMusicTime == IDM_SNAP_BAR) ? MF_CHECKED : MF_UNCHECKED );
		}
	}

	m_pTrackMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::UnselectGutterRange

void CWaveStrip::UnselectGutterRange( void )
{
	// Make sure everything on the timeline is deselected.

	// Flag that we're unselecting all other strips
	m_fSelecting = TRUE;

	// Set the begin and end selection time to 0
	m_pTrackMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pTrackMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );

	// Flags that we're no longer unselecting all other strips
	m_fSelecting = FALSE;
}


// IDropSource helpers

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CreateDataObject

HRESULT	CWaveStrip::CreateDataObject( IDataObject** ppIDataObject, long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we're passed a valid location to store the IDataObject pointer in
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize the IDataObject pointer to NULL
	*ppIDataObject = NULL;

	// Allocate a Timleine DataObject
	IDMUSProdTimelineDataObject *pITimelineDataObject;
	if( FAILED( m_pTrackMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
	{
		return E_OUTOFMEMORY;
	}

	// Save Selected items into stream
	HRESULT hr = E_FAIL;

	// Allocate a memory stream
	IStream* pIStream;
	if( SUCCEEDED ( m_pTrackMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		REFERENCE_TIME rtOffset = 0;

		// Get the item at the drag point
		CTrackItem* pItemAtDragPoint = GetItemFromPoint( lXPos, lYPos );
		if( pItemAtDragPoint == NULL )
		{
			// If no pItemAtDragPoint use the first selected item
			pItemAtDragPoint = GetFirstSelectedItem();
		}
		ASSERT( pItemAtDragPoint->m_fSelected );

		// Compute offset
//		MUSIC_TIME mtTimeXPos;
//		m_pTrackMgr->m_pTimeline->PositionToClocks( lXPos, &mtTimeXPos );
//		m_pTrackMgr->ClocksToUnknownTime( mtTimeXPos, &rtOffset );
//		rtOffset = pItemAtDragPoint->m_rtTimePhysical - rtOffset;
		rtOffset = 0;

		// mark the items as being dragged: this used later for deleting items in drag move
		MarkSelectedItems( UD_DRAGSELECT );

		// Save the selected items into a stream
		if( SUCCEEDED ( SaveSelectedItems( pIStream, rtOffset, pItemAtDragPoint ) ) )
		{
			// Add the stream to the Timeline DataObject
			if( SUCCEEDED ( pITimelineDataObject->AddExternalClipFormat( m_cfWaveTrack, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		// Releas the stream
		pIStream->Release();
	}

	// If we were successful, export the Timeline DataObject into an IDataObject
	if( SUCCEEDED ( hr ) )
	{
		pITimelineDataObject->Export( ppIDataObject );
	}

	// Release the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DoDragDrop

HRESULT CWaveStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos, long lYPos )
{
	// Do a drag'n'drop operation

	// Get the item at the point we're dragging from
	CTrackItem* pItem = GetItemFromPoint( lXPos, lYPos );

	// Check that we found a item, and that it is selected
	if(!pItem || !pItem->m_fSelected)
	{
		// Didn't find a selected item - return that nothing happened
		return S_FALSE;
	}

	// Initialize flag that determines when a successful drop occurs
	bool fDrop = false;

	// Initialize our result code
	HRESULT hr = S_FALSE;

	// Drag-drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Initialize the drop effect to 'none'
	m_dwDropEffect = DROPEFFECT_NONE;

	// Query ourself for our IDropSource interface
	IDropSource*	pIDropSource;
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// Ensure m_pISourceDataObject is empty
		if( m_pISourceDataObject )
		{
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;
		}

		// Create a data object from the selected items
		hr = CreateDataObject( &m_pISourceDataObject, lXPos, lYPos );

		// Check if we successfully created a data object
		if(SUCCEEDED(hr))
		{
			// We can always copy items.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;

			// Check if we can cut items
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the items as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.

			// Initialize our DragDropSource flag to 1 (means we are the source of a drag-drop)
			m_nStripIsDragDropSource = 1;

			// Initialize our DragDropSource DocRoot fields
			CWaveStrip::m_pIDocRootOfDragDropSource = m_pTrackMgr->m_pIDocRootNode;
			CWaveStrip::m_fDragDropIntoSameDocRoot = FALSE;

			// Save the state of the mouse buttons
			m_dwStartDragButton = (unsigned long)wParam;

			// Save the position the drag started at
			// (used in PasteAt to determine if we dropped in the same beat we dragged from)
			m_lStartDragPosition = lXPos;

			// Initialize the drop effect to 'none'
			DWORD dwEffect = DROPEFFECT_NONE;

			// Actually do the drag-drop operation
			hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

			// Drag/Drop completed, clean up
			m_dwStartDragButton = 0;
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;

			// Check what happened during the drag-drop operation
			switch(hr)
			{
			case DRAGDROP_S_DROP:
				// A drop occurred - save the effect
				m_dwDropEffect = dwEffect;

				// Check if we did a move
				if(dwEffect & DROPEFFECT_MOVE)
				{
					// We moved some items - delete the items we marked earlier
					DeleteMarked(UD_DRAGSELECT);
				}

				// If the effect wasn't 'None', set the drop flag to true
				if(dwEffect != DROPEFFECT_NONE)
				{
					fDrop = true;
				}
				break;
			default:
				// User cancelled the operation, or dropped the items somewhere they didn't belong
				m_dwDropEffect = DROPEFFECT_NONE;
				break;
			}

			// Check if we really did a drop
			if( fDrop )
			{
				// Check if we dropped in the same strip
				if( m_nStripIsDragDropSource == 2 )
				{
					// Source and target are the same strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// We did a move - use the 'Move' undo text
						m_pTrackMgr->m_nLastEdit = IDS_UNDO_MOVE;
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_pTrackMgr->m_nLastEdit = IDS_INSERT;
					}

					// Notify the containing segment that we changed
					m_pTrackMgr->OnDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelectedItems();

					// If visible, switch the property sheet to the track item property page
					OnShowItemProperties();

					// If it exists, refresh the track item property page
					RefreshPropertyPage();

					// Sync with DirectMusic
					m_pTrackMgr->SyncWithDirectMusic();
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// Check if we dropped in the same segment
						if( CWaveStrip::m_fDragDropIntoSameDocRoot == TRUE )
						{
							// We did a move operation to another strip in the same segment - use the 'Move' undo text
							m_pTrackMgr->m_nLastEdit = IDS_UNDO_MOVE;
						}
						else
						{
							// We did a move operation to another strip in a different - use the 'Delete' undo text
							m_pTrackMgr->m_nLastEdit = IDS_DELETE;
						}

						// Notify the Wave Designer that we changed
						m_pTrackMgr->OnDataChanged();

						// Sync with DirectMusic
						m_pTrackMgr->SyncWithDirectMusic();
					}
					else
					{
						// Unselect all items, since the user did a copy operation
						UnselectAll();
					}
				}

				// Refresh our strip display
				m_pTrackMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

				// Return that we actually did a drop
				hr = S_OK;
			}
		}

		// Release the IDropSource interface
		pIDropSource->Release();

		// Clear the DragSelect flag from all items
		UnMarkItems(UD_DRAGSELECT); 
	}
	else
	{
		// Couldn't query for IDropSource interface - return failure code
		hr = E_FAIL;
	}

	// Clear the DragDropSource fields
	m_nStripIsDragDropSource = 0;
	CWaveStrip::m_pIDocRootOfDragDropSource = NULL;
	CWaveStrip::m_fDragDropIntoSameDocRoot = FALSE;
	
	// Return a success or failure code
	return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SelectItemsInSelectedRegions

bool CWaveStrip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if this item is within a selection region
		if( CListSelectedRegion_Contains( pItem->m_rtTimePhysical ) )
		{
			// Check if the item is not selected
			if( !pItem->m_fSelected )
			{
				// Item isn't yet selected - select it and set fChanged to true
				pItem->m_fSelected = TRUE;
				fChanged = true;
			}
		}
		else
		{
			// Check if the item is selected
			if( pItem->m_fSelected )
			{
				// Item is selected - deselect it and set fChagned to true
				pItem->m_fSelected = FALSE;
				fChanged = true;
			}
		}
	}

	// return whether or not we changed the selection state of any items
	return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SelectRegionsFromSelectedItems

void CWaveStrip::SelectRegionsFromSelectedItems( void )
{
	// Clear the list of selected regions
	m_pSelectedRegions->Clear();

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Add the beat the item is in to the list of selected regions.
			CListSelectedRegion_AddRegion( pItem );
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// CWaveStrip::RegisterClipboardFormats

BOOL CWaveStrip::RegisterClipboardFormats( void )
{
	// CF_WAVETRACK
	if( m_cfWaveTrack == 0 )
	{
		m_cfWaveTrack = RegisterClipboardFormat( CF_WAVETRACK );
	}
	
	// CF_DLS_WAVE
	if( m_cfWave == 0 )
	{
		m_cfWave = RegisterClipboardFormat( CF_DLS_WAVE );
	}

	// CF_DMUSPROD_FILE
	if( m_cfDMUSProdFile == 0 )
	{
		m_cfDMUSProdFile = RegisterClipboardFormat( CF_DMUSPROD_FILE );
	}

	if( m_cfWaveTrack == 0 
	||  m_cfWave == 0
	||	m_cfDMUSProdFile == 0 )
	{ 
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::LoadList

HRESULT CWaveStrip::LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO		ck;
	MMCKINFO		ckListTrack;
	MMCKINFO		ckListItem;
	DWORD			dwByteCount;
	DWORD			dwSize;
	CString			strObjectName;
	HRESULT			hr = E_FAIL;

	// Initialize field containing state of variation buttons at time this list was created
	m_dwSourceVariationBtns = 0;

	// LoadList does not expect to be called twice on the same object!

	if( pIStream == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Load the track item
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_WAVEPART_LIST:
						while( pIRiffStream->Descend( &ckListTrack, &ck, 0 ) == 0 )
						{
							switch( ckListTrack.ckid )
							{
								case DMUS_FOURCC_WAVEPART_CHUNK:
								{
									DMUS_IO_WAVE_PART_HEADER iPartHeader;

									// Read in the item's header structure
									dwSize = min( sizeof( DMUS_IO_WAVE_PART_HEADER ), ckListTrack.cksize );
									hr = pIStream->Read( &iPartHeader, dwSize, &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									// This method called when pasting or dropping items
									// Do not overwrite strip parameters
									break;
								}

								case FOURCC_LIST:
									switch( ckListTrack.fccType )
									{
										case DMUS_FOURCC_WAVEPART_UI_LIST:
											while( pIRiffStream->Descend( &ckListItem, &ckListTrack, 0 ) == 0 )
											{
												switch( ckListItem.ckid )
												{
													case DMUS_FOURCC_WAVEPART_UI_CHUNK:
													{
														ioWavePartUI iWavePartUI;
														ZeroMemory( &iWavePartUI, sizeof(ioWavePartUI) );

														// Read in the item's header structure
														dwSize = min( sizeof( ioWavePartUI ), ck.cksize );
														hr = pIStream->Read( &iWavePartUI, dwSize, &dwByteCount );

														// Handle any I/O error by returning a failure code
														if( FAILED( hr )
														||  dwByteCount != dwSize )
														{
															hr = E_FAIL;
															goto ON_ERROR;
														}

														m_dwSourceVariationBtns = iWavePartUI.m_dwVariationBtns;
														break;
													}
												}

												pIRiffStream->Ascend( &ckListItem, 0 );
											}
											break;

										case DMUS_FOURCC_WAVEITEM_LIST:
											while( pIRiffStream->Descend( &ckListItem, &ckListTrack, 0 ) == 0 )
											{
												switch( ckListItem.ckid )
												{
													case FOURCC_LIST:
														switch( ckListItem.fccType )
														{
															case DMUS_FOURCC_WAVE_LIST:
															{
																CTrackItem* pNewItem = new CTrackItem( m_pTrackMgr, this );
																if( pNewItem == NULL )
																{
																	hr = E_OUTOFMEMORY;
																	goto ON_ERROR;
																}
																hr = pNewItem->LoadListItem( pIRiffStream, &ckListItem, m_pTrackMgr->m_pDMProdFramework, m_pTrackMgr );
																if( FAILED ( hr ) )
																{
																	delete pNewItem;
																	goto ON_ERROR;
																}
																list.AddTail( pNewItem );
																break;
															}
														}
														break;
												}

												pIRiffStream->Ascend( &ckListItem, 0 );
											}
											break;
									}
									break;
							}

							pIRiffStream->Ascend( &ckListTrack, 0 );
						}
						break;
				}
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	RELEASE( pIRiffStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SaveList

HRESULT CWaveStrip::SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO ckPart;
	MMCKINFO ck;
	HRESULT hr;

	if( pIStream == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// Create the DMUS_FOURCC_WAVEPART_LIST list chunk
	ckPart.fccType = DMUS_FOURCC_WAVEPART_LIST;
	if( pIRiffStream->CreateChunk( &ckPart, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_WAVEPART_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEPART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_WAVE_PART_HEADER oPartHeader;
		ZeroMemory( &oPartHeader, sizeof(DMUS_IO_WAVE_PART_HEADER) );

		// Fill in the members of the DMUS_IO_WAVE_PART_HEADER structure
		oPartHeader.dwPChannel = m_dwPChannel;
		oPartHeader.dwIndex = m_dwIndex;
		oPartHeader.dwVariations = m_dwVariationsMask;

		oPartHeader.lVolume = m_lVolume;
		oPartHeader.dwLockToPart = m_dwLockToPart;
		oPartHeader.dwFlags = m_dwPartFlagsDM;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oPartHeader, sizeof(DMUS_IO_WAVE_PART_HEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_WAVE_PART_HEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEPART_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Save the strip's design data
	hr = SaveStripDesignData( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save the track items in this PChannel
	if( !list.IsEmpty() )
	{

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckItemList;
		ckItemList.fccType = DMUS_FOURCC_WAVEITEM_LIST;
		if( pIRiffStream->CreateChunk( &ckItemList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the item list
		POSITION pos = list.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackItem* pItem = list.GetNext( pos );

			// Save each item
			hr = pItem->SaveListItem( pIRiffStream, m_pTrackMgr->m_pDMProdFramework, m_pTrackMgr );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		pIRiffStream->Ascend( &ckItemList, 0 );
	}

	// Ascend out of the DMUS_FOURCC_WAVEPART_LIST list chunk
	if( pIRiffStream->Ascend(&ckPart, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIRiffStream );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::LoadStrip

HRESULT CWaveStrip::LoadStrip( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent )
{
	MMCKINFO		ck;
	MMCKINFO		ckList;
	DWORD			dwByteCount;
	DWORD			dwSize;
	CString			strObjectName;
	HRESULT			hr = E_FAIL;

	// LoadPChannel does not expect to be called twice on the same object!

	if( pIRiffStream == NULL 
	||  pckParent == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Load the PChannel
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_WAVEPART_CHUNK:
			{
				DMUS_IO_WAVE_PART_HEADER iPartHeader;

				// Read in the item's header structure
				dwSize = min( sizeof( DMUS_IO_WAVE_PART_HEADER ), ck.cksize );
				hr = pIStream->Read( &iPartHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwPChannel = iPartHeader.dwPChannel;
				m_dwIndex = iPartHeader.dwIndex;
				m_dwVariationsMask = iPartHeader.dwVariations;

				m_lVolume = iPartHeader.lVolume;
				m_dwLockToPart = iPartHeader.dwLockToPart;
				m_dwPartFlagsDM = iPartHeader.dwFlags;
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_WAVEPART_UI_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_WAVEPART_UI_CHUNK:
								{
									ioWavePartUI iWavePartUI;
									ZeroMemory( &iWavePartUI, sizeof(ioWavePartUI) );

									// Read in the item's header structure
									dwSize = min( sizeof( ioWavePartUI ), ck.cksize );
									hr = pIStream->Read( &iWavePartUI, dwSize, &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									m_StripUI.m_dwVariationBtns = iWavePartUI.m_dwVariationBtns;
									m_StripUI.m_dblVerticalZoom = iWavePartUI.m_dblVerticalZoom;
									m_StripUI.m_lVerticalScrollYPos = iWavePartUI.m_lVerticalScrollYPos;
									m_StripUI.m_lStripHeight = iWavePartUI.m_lStripHeight;
									m_StripUI.m_nStripView = iWavePartUI.m_nStripView;
									m_StripUI.m_nSnapToRefTime = iWavePartUI.m_nSnapToRefTime;
									m_StripUI.m_nSnapToMusicTime = iWavePartUI.m_nSnapToMusicTime;
									m_StripUI.m_dwFlagsUI = iWavePartUI.m_dwFlagsUI;
									m_StripUI.m_dwOldVariationBtns = iWavePartUI.m_dwOldVariationBtns;
									m_StripUI.m_fVarGutterDown = iWavePartUI.m_fVarGutterDown;
									m_StripUI.m_nNbrLayers = iWavePartUI.m_nNbrLayers;
									m_StripUI.m_nSnapToRefTime_GridsPerSecond = iWavePartUI.m_nSnapToRefTime_GridsPerSecond;

									if( m_StripUI.m_nSnapToRefTime == 0 )
									{
										m_StripUI.m_nSnapToRefTime = IDM_SNAP_GRID;
									}
									if( m_StripUI.m_nSnapToMusicTime == 0 )
									{
										m_StripUI.m_nSnapToMusicTime = IDM_SNAP_GRID;
									}
									if( m_StripUI.m_nSnapToRefTime_GridsPerSecond == 0 )
									{
										m_StripUI.m_nSnapToRefTime_GridsPerSecond = DEFAULT_NBR_GRIDS_PER_SECOND;
									}
									break;
								}
								
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strStripName );
									break;
							}

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_WAVEITEM_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_WAVE_LIST:
										{
											CTrackItem* pNewItem = new CTrackItem( m_pTrackMgr, this );
											if( pNewItem == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}
											hr = pNewItem->LoadTrackItem( pIRiffStream, &ckList, m_pTrackMgr->m_pDMProdFramework );
											if( FAILED ( hr ) )
											{
												delete pNewItem;
												goto ON_ERROR;
											}
											InsertByAscendingTime( pNewItem, FALSE );
											break;
										}
									}
							}

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SaveStrip

HRESULT CWaveStrip::SaveStrip( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ckPart;
	MMCKINFO ck;
	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;
	bool fSavingFile = false;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;
		if( SUCCEEDED ( pPersistInfo->GetFileName( &bstrFileName ) ) )
		{
			::SysFreeString( bstrFileName );
			fSavingFile = true;
		}
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// Create the DMUS_FOURCC_WAVEPART_LIST list chunk
	ckPart.fccType = DMUS_FOURCC_WAVEPART_LIST;
	if( pIRiffStream->CreateChunk( &ckPart, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_WAVEPART_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEPART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_WAVE_PART_HEADER oPartHeader;
		ZeroMemory( &oPartHeader, sizeof(DMUS_IO_WAVE_PART_HEADER) );

		// Fill in the members of the DMUS_IO_WAVE_PART_HEADER structure
		oPartHeader.dwPChannel = m_dwPChannel;
		oPartHeader.dwIndex = m_dwIndex;
		oPartHeader.dwVariations = m_dwVariationsMask;

		oPartHeader.lVolume = m_lVolume;
		oPartHeader.dwLockToPart = m_dwLockToPart;
		oPartHeader.dwFlags = m_dwPartFlagsDM;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oPartHeader, sizeof(DMUS_IO_WAVE_PART_HEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_WAVE_PART_HEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEPART_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Save the strip's design data
	if( ftFileType == FT_DESIGN )
	{
		hr = SaveStripDesignData( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save the track items in this PChannel
	if( !m_lstTrackItems.IsEmpty() )
	{
		REFERENCE_TIME rtSegmentLength = _I64_MAX;

		if( fSavingFile )
		{
			// Get the segment's length
			rtSegmentLength = m_pTrackMgr->GetSegmentLength();
		}

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckItemList;
		ckItemList.fccType = DMUS_FOURCC_WAVEITEM_LIST;
		if( pIRiffStream->CreateChunk( &ckItemList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the item list (all variations)
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			// When saving a file,
			// Only save item's that fall within the segment
			if( pItem->m_rtTimeLogical < rtSegmentLength
			&&  pItem->m_rtTimePhysical < rtSegmentLength )
			{
				// Save each item (all variations)
				hr = pItem->SaveTrackItem( pIRiffStream, m_pTrackMgr->m_pDMProdFramework );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
		
		pIRiffStream->Ascend( &ckItemList, 0 );
	}

	// Ascend out of the DMUS_FOURCC_WAVEPART_LIST list chunk
	if( pIRiffStream->Ascend(&ckPart, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SaveStripDesignData

HRESULT CWaveStrip::SaveStripDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ckMain;
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Create the DMUS_FOURCC_WAVEPART_UI_LIST list chunk
	ckMain.fccType = DMUS_FOURCC_WAVEPART_UI_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_WAVEPART_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEPART_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Make sure we are saving the latest information
		if( m_pTrackMgr
		&&  m_pTrackMgr->m_pTimeline )
		{
			TimelineToWavePartUI();
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioWavePartUI oWavePartUI;
		ZeroMemory( &oWavePartUI, sizeof(ioWavePartUI) );

		// Fill in the members of the ioWavePartUI structure
		oWavePartUI.m_dwVariationBtns = m_StripUI.m_dwVariationBtns;
		oWavePartUI.m_dblVerticalZoom = m_StripUI.m_dblVerticalZoom;
		oWavePartUI.m_lVerticalScrollYPos = m_StripUI.m_lVerticalScrollYPos;
		oWavePartUI.m_lStripHeight = m_StripUI.m_lStripHeight;
		oWavePartUI.m_nStripView = m_StripUI.m_nStripView;
		oWavePartUI.m_nSnapToRefTime = m_StripUI.m_nSnapToRefTime;
		oWavePartUI.m_nSnapToMusicTime = m_StripUI.m_nSnapToMusicTime;
		oWavePartUI.m_dwFlagsUI = m_StripUI.m_dwFlagsUI;
		oWavePartUI.m_dwOldVariationBtns = m_StripUI.m_dwOldVariationBtns;
		oWavePartUI.m_fVarGutterDown = m_StripUI.m_fVarGutterDown;
		oWavePartUI.m_nNbrLayers = m_StripUI.m_nNbrLayers;
		oWavePartUI.m_nSnapToRefTime_GridsPerSecond = m_StripUI.m_nSnapToRefTime_GridsPerSecond;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oWavePartUI, sizeof(ioWavePartUI), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioWavePartUI) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEPART_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Save the strip's name
	if( !m_strStripName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strStripName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the DMUS_FOURCC_WAVEPART_LIST list chunk
	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetWaveInfo

HRESULT CWaveStrip::GetWaveInfo( void )
{
	HRESULT hr = S_OK;

	// Ask DLS Designer for info about waves (all variations)
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Get WaveInfo
		if( FAILED ( pItem->GetWaveInfo() ) )
		{
			hr = E_FAIL;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SwitchTimeBase

void CWaveStrip::SwitchTimeBase( void )
{
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		pItem->SwitchTimeBase();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnShowStripProperties

void CWaveStrip::OnShowStripProperties()
{
	m_fShowItemProps = false;

	if( m_pTrackMgr )
	{
		m_pTrackMgr->m_pWaveStripForPropSheet = this;
		m_pTrackMgr->OnShowProperties();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnShowItemProperties

void CWaveStrip::OnShowItemProperties()
{
	if( IsSelected() == FALSE )
	{
		// Switch the property page to the strip's property page
		OnShowStripProperties();
		return;
	}

	m_fShowItemProps = true;

	OnShowProperties();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetZoom

void CWaveStrip::SetZoom( double dblNewVerticalZoom )
{
	ASSERT( m_pTrackMgr != NULL );
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	int nNewWaveHeight = long( MAX_WAVE_HEIGHT * dblNewVerticalZoom + HORIZ_LINE_HEIGHT );

	if( m_StripUI.m_dblVerticalZoom != dblNewVerticalZoom  
	||  m_nWaveHeight != nNewWaveHeight ) 
	{
		UINT nCurScrollBarPos = m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight;
		m_StripUI.m_dblVerticalZoom = dblNewVerticalZoom;
		m_nWaveHeight = nNewWaveHeight;
		long lNewVerticalScrollYPos = nCurScrollBarPos * m_nWaveHeight;

		SetVerticalScrollYPos( lNewVerticalScrollYPos );

		RefreshVerticalScrollBarUI();
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IsSelected

bool CWaveStrip::IsSelected()
{
	// If anything is selected in displayed variations, return true
	
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Item is selected - return true
			return true;
		}
	}

	// No items selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IsEmpty

bool CWaveStrip::IsEmpty()
{
	return m_lstTrackItems.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SelectSegment

// Return true if anything changed
bool CWaveStrip::SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime )
{
	// Verify that we have a pointer to the Timeline
	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return false;
	}

	REFERENCE_TIME rtTime;

	// Snap the start point
	m_pTrackMgr->ClocksToUnknownTime( mtBeginTime, &rtTime );
	SnapUnknownTime( rtTime, &rtTime );
	m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtBeginTime );

	// Snap the end point
	m_pTrackMgr->ClocksToUnknownTime( mtEndTime, &rtTime );
	SnapUnknownTime( rtTime, &rtTime );
	SnapUnknownToNextIncrement( rtTime, &rtTime );
	m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtEndTime );
	mtEndTime--;

	// Convert the passed in times to a generic time class
	CMusicTimeConverter cmtBeg(mtBeginTime);
	CMusicTimeConverter cmtEnd(mtEndTime);

	// Create a region that contains the selected time
	CSelectedRegion region( cmtBeg, cmtEnd );

	// Add the region to the list of selected regions
	m_pSelectedRegions->AddRegion( region );

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return SelectItemsInSelectedRegions();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::UnselectAll

void CWaveStrip::UnselectAll()
{
	// Unselect all layers
	POSITION pos = m_lstLayers.GetHeadPosition();
	while( pos )
	{
		CWaveStripLayer* pLayer = m_lstLayers.GetNext( pos );

		pLayer->m_fSelected = FALSE;
	}

	// Unselect all items ( all variations)
	pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item 
		m_lstTrackItems.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_pSelectedRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DeleteBetweenMeasureBeats

bool CWaveStrip::DeleteBetweenMeasureBeats( long lmStart, long lbStart, long lmEnd, long lbEnd )
{
	long lItemMeasure, lItemBeat, lItemGrid, lItemTick;

	// Initially, nothing changed
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while(pos)
	{
		// Save current position
		POSITION posTemp = pos;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( pItem->m_rtTimePhysical,
													   &lItemMeasure, &lItemBeat, &lItemGrid, &lItemTick );

		// Check if the item's measure value is before the start measure
		if( lItemMeasure < lmStart )
		{
			// Too early - keep looking
			continue;
		}

		// Check if the item's measure value is after the end measure
		if( lItemMeasure > lmEnd )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// If the item is in the start measure, check if it is before the start beat
		if( ( lItemMeasure == lmStart )
		&&	( lItemBeat < lbStart ) )
		{
			// Too early - keep looking
			continue;
		}

		// If the item is in the end measure, check if it is after the end beat
		if( ( lItemMeasure == lmEnd)
		&&	( lItemBeat > lbEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// Within the given range, delete the item from the selected variations
		pItem->m_dwVariations &= ~m_StripUI.m_dwVariationBtns;
		if( pItem->m_dwVariations == 0 )
		{
			m_lstTrackItems.RemoveAt(posTemp);
			delete pItem;
		}

		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InsertByAscendingTime

void CWaveStrip::InsertByAscendingTime( CTrackItem* pItemToInsert, BOOL fPaste )
{
	UNREFERENCED_PARAMETER(fPaste);

	// Ensure the pItemToInsert pointer is valid
	if ( pItemToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Make sure we have variations!
	ASSERT( pItemToInsert->m_dwVariations != 0 );

	// Make sure time is greater than zero!
	ASSERT( pItemToInsert->m_rtTimePhysical >= 0 );

	// Make sure item has been assigned to a layer!
	ASSERT( pItemToInsert->m_pLayer != NULL );

	// Make sure item's layer is in m_lstLayers
	ASSERT( m_lstLayers.Find(pItemToInsert->m_pLayer) );

	// Get wave information from DLS designer
	pItemToInsert->GetWaveInfo();

	CTrackItem* pItem;
	POSITION posCurrent, posNext = m_lstTrackItems.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pItem = m_lstTrackItems.GetNext( posNext );

		if( pItem->m_rtTimePhysical > pItemToInsert->m_rtTimePhysical )
		{
			// insert before posCurrent (which is the position of pItem)
			m_lstTrackItems.InsertBefore( posCurrent, pItemToInsert );
			return;
		}
	}

	// pItemToInsert is later than all items in the list, add it at the end of the list
	m_lstTrackItems.AddTail( pItemToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnUpdate

HRESULT CWaveStrip::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	HRESULT hr = E_INVALIDARG;

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_pTrackMgr->m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// Framework message
	if( ::IsEqualGUID(rguidType, GUID_Segment_FrameworkMsg) )
	{
		DMUSProdFrameworkMsg* pFrameworkMsg =  (DMUSProdFrameworkMsg *)pData;
		ASSERT( pFrameworkMsg != NULL );

		IDMUSProdNode* pINode = NULL;

		if( pFrameworkMsg->punkIDMUSProdNode )
		{
			if( FAILED ( pFrameworkMsg->punkIDMUSProdNode->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
			{
				pINode = NULL;
			}
		}

		if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileDeleted)  
		||  ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileClosed) )
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them (all variations)
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This file was removed from the Project Tree
						pItem->SetFileReference( NULL );

						// Set undo text resource id
						if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileDeleted) )
						{
							m_pTrackMgr->m_nLastEdit = IDS_UNDO_DELETE_WAVE;
							hr = S_OK;
						}
						else
						{
							hr = S_FALSE;
						}
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileReplaced) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them (all variations)
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This file was replaced in the Project Tree, re-establish the link
						pItem->SetFileReference( (IDMUSProdNode *)pFrameworkMsg->pData );

						m_pTrackMgr->m_nLastEdit = IDS_UNDO_REPLACE_WAVE;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, DOCROOT_GuidChange) 
			 ||  ::IsEqualGUID(pFrameworkMsg->guidUpdateType, FRAMEWORK_FileNameChange) ) 
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them (all variations)
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// Set undo text resource id
						m_pTrackMgr->m_nLastEdit = IDS_UNDO_CHANGE_WAVE_LINK;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, WAVENODE_NameChange) )
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them (all variations)
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					if( pItem
					&&  pItem->m_FileRef.pIDocRootNode == pINode )
					{
						// This file was renamed so update the file's list info fields
						hr = pItem->SetListInfo( m_pTrackMgr->m_pDMProdFramework );

						// Set undo text resource id
						m_pTrackMgr->m_nLastEdit = IDS_UNDO_CHANGE_WAVE_NAME;
						hr = S_OK;
					}
				}
			}
		}

		else if( ::IsEqualGUID(pFrameworkMsg->guidUpdateType, WAVENODE_DataChange) )
		{
			hr = E_FAIL;

			if( pINode )
			{
				// Find the items that changed and update them (all variations)
				POSITION pos = m_lstTrackItems.GetHeadPosition();
				while( pos )
				{
					CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

					// Get wave information from DLS designer
					pItem->GetWaveInfo();
					m_pTrackMgr->QueueWaveForDownload( pItem );

					// Set undo text resource id
					m_pTrackMgr->m_nLastEdit = IDS_UNDO_CHANGE_WAVE_DATA;
					hr = S_OK;
				}
			}
		}

		RELEASE( pINode );

		if( SUCCEEDED ( hr ) )
		{
			// Let our hosting editor know about the changes
			if( hr == S_OK )
			{
				m_pTrackMgr->OnDataChanged();
			}

			if( m_pTrackMgr->m_pTimeline )
			{
				m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
			}

			// Update the property page
			RefreshPropertyPage();

			m_pTrackMgr->SyncWithDirectMusic();
		}
	}

	// GUID_ConductorUnloadWaves
	else if(::IsEqualGUID(rguidType, GUID_ConductorUnloadWaves))
	{
		// Unload all waves
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			m_pTrackMgr->QueueWaveForUnload( pItem );
		}
		hr = S_OK;
	}

	// GUID_ConductorDownloadWaves
	else if(::IsEqualGUID(rguidType, GUID_ConductorDownloadWaves))
	{
		// Download all waves
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			m_pTrackMgr->QueueWaveForDownload( pItem );
		}
		hr = S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SaveSelectedItems

HRESULT CWaveStrip::SaveSelectedItems( IStream* pIStream, REFERENCE_TIME rtOffset, CTrackItem* pItemAtDragPoint )
{
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	CWaveStripLayer* pLastLayer = NULL;
	MUSIC_TIME mtPasteOverwriteRange = -1;
	int		 nLayerIndex;
	int		 nMinLayerIndex = -1;
	int		 nMaxLayerIndex = -1;
	MMCKINFO ck;
	DWORD	 dwBytesWritten;

	// Verify that the pIStream pointer is valid
	if( pIStream == NULL )
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Create a list to store the items to save in
	CTypedPtrList<CPtrList, CTrackItem*> lstItemsToSave;

	// Iterate throught the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext(pos);

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Add the item to the list of items to save
			CTrackItem* pNewItem = new CTrackItem( m_pTrackMgr, this, *pItem );
			if( pNewItem )
			{
				// TODO!!!   Put back
//				mtPasteOverwriteRange = max( mtPasteOverwriteRange, );

				if( pNewItem->m_pLayer != pLastLayer )
				{
					pLastLayer = pNewItem->m_pLayer;
					nLayerIndex = LayerToIndex( pLastLayer );
					if( nMinLayerIndex == -1 )
					{
						nMinLayerIndex = nLayerIndex;
						nMaxLayerIndex = nLayerIndex;
					}
					else
					{
						nMinLayerIndex = min( nMinLayerIndex, nLayerIndex );
						nMaxLayerIndex = max( nMaxLayerIndex, nLayerIndex );
					}
				}

				pNewItem->m_dwVariations &= m_StripUI.m_dwVariationBtns;
				lstItemsToSave.AddTail( pNewItem );
			}
		}
	}

	// Get item for DMUS_FOURCC_COPYPASTE_UI_CHUNK
	CTrackItem* pCopyPasteItem = pItemAtDragPoint;
	if( pCopyPasteItem == NULL )
	{
		if( lstItemsToSave.IsEmpty() == FALSE )
		{
			pCopyPasteItem = lstItemsToSave.GetHead();
		}
	}

	// Try and allocate a RIFF stream
	HRESULT hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Create DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// If range selected via timeline, use it
		// TODO!!!   Put back
//		if( m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect) )
		{
			long lStartTime;
			long lEndTime;
			m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

			// Compute the start offset
			MUSIC_TIME mtStartOffset;
			long lMeasure;
			long lBeat;
			m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lStartTime, &lMeasure, &lBeat );
			m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtStartOffset );

			// Compute the end offset
			MUSIC_TIME mtEndOffset;
			m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( m_pTrackMgr->m_dwGroupBits, 0, lEndTime, &lMeasure, &lBeat );
			m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, (lBeat + 1), 0, 0, &mtEndOffset );

			mtPasteOverwriteRange = mtEndOffset - mtStartOffset;
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioCopyPasteUI oCopyPasteUI;
		ZeroMemory( &oCopyPasteUI, sizeof(ioCopyPasteUI) );

		// Fill in the members of the ioCopyPasteUI structure
		if( pCopyPasteItem )
		{
			long lMinute, lMeasure;

			m_pTrackMgr->UnknownTimeToRefTime( rtOffset, &oCopyPasteUI.m_rtOffset );
			m_pTrackMgr->UnknownTimeToRefTime( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_rtStartTime );
			m_pTrackMgr->RefTimeToMinSecGridMs( this, oCopyPasteUI.m_rtStartTime, &lMinute, &oCopyPasteUI.m_lRefTimeSec,
											   &oCopyPasteUI.m_lRefTimeGrid, &oCopyPasteUI.m_lRefTimeMs );

			m_pTrackMgr->UnknownTimeToClocks( rtOffset, &oCopyPasteUI.m_mtOffset );
			m_pTrackMgr->UnknownTimeToClocks( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_mtStartTime );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( oCopyPasteUI.m_mtStartTime, &lMeasure, &oCopyPasteUI.m_lMusicTimeBeat,
													 &oCopyPasteUI.m_lMusicTimeGrid, &oCopyPasteUI.m_lMusicTimeTick );

		}
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtPasteOverwriteRange, &oCopyPasteUI.m_rtPasteOverwriteRange );
		oCopyPasteUI.m_mtPasteOverwriteRange = mtPasteOverwriteRange;
		oCopyPasteUI.m_nStartLayerIndex = pCopyPasteItem ? LayerToIndex(pCopyPasteItem->m_pLayer) : 0;
		oCopyPasteUI.m_nMinLayerIndex = nMinLayerIndex;
		oCopyPasteUI.m_nMaxLayerIndex = nMaxLayerIndex;
		oCopyPasteUI.m_fRefTimeTrack = m_pTrackMgr->IsRefTimeTrack();

		// Write the structure out to the stream
		hr = pIStream->Write( &oCopyPasteUI, sizeof(ioCopyPasteUI), &dwBytesWritten );
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioCopyPasteUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	//check that anything is selected
	if( lstItemsToSave.IsEmpty() )
	{
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// For each item in lstItemsToSave, subtract rtOffset from m_rtTimePhysical
	REFERENCE_TIME rtNormalizeOffset;
	if( pCopyPasteItem == NULL )
	{
		ASSERT( 0 );
		rtNormalizeOffset = 0;
	}
	else
	{
		rtNormalizeOffset = pCopyPasteItem->m_rtTimePhysical;
	}
	NormalizeList( m_pTrackMgr, lstItemsToSave, rtNormalizeOffset );

	// Save the list of items into pIStream
	hr = SaveList( lstItemsToSave, pIStream );

	// Empty the temporary list of items
	EmptyList( lstItemsToSave );

ON_ERROR:
	RELEASE( pIRiffStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnEnableVariations

HRESULT CWaveStrip::OnEnableVariations( WORD wID )
{
	DWORD dwOldVariationsMask = m_dwVariationsMask;
	int i;

	switch( wID )
	{
		case IDM_ENABLE_VARBTNS:
			// Enable all selected variations that are disabled
			for( i = 0 ;   i < 32 ;   i++ )
			{
				// if variation is selected and disabled
				if( (m_StripUI.m_dwVariationBtns & (1 << i)) & ~m_dwVariationsMask )
				{
					m_dwVariationsMask ^= (1 << i);
				}
			}
			break;

		case IDM_DISABLE_VARBTNS:
			// Disable all selected variations that are enabled
			for( i = 0 ;  i < 32 ;  i++ )
			{
				// if variation is selected and enabled
				if( (m_StripUI.m_dwVariationBtns & (1 << i)) & m_dwVariationsMask )
				{
					m_dwVariationsMask ^= 1 << i;
				}
			}
			// if everything is disabled, re-enable the first selected variation
			if( m_dwVariationsMask == 0 )
			{
				for( i = 0 ;  i < 32 ;  i++ )
				{
					// If variation is selected, re-enable it
					if( m_StripUI.m_dwVariationBtns & (1 << i) )
					{
						m_dwVariationsMask ^= 1 << i;
						break;
					}
				}
			}
			m_StripUI.m_dwVariationBtns &= m_dwVariationsMask;
			break;
	}

	// Always redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	if( m_dwVariationsMask != dwOldVariationsMask )
	{
		// Notify the containing segment that we changed
		if( wID == IDM_ENABLE_VARBTNS )
		{
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_ENABLE_VARCHOICES;
		}
		else
		{
			m_pTrackMgr->m_nLastEdit = IDS_UNDO_DISABLE_VARCHOICES;
		}
		m_pTrackMgr->OnDataChanged();

		// Update the selection regions to include the selected items
		SelectRegionsFromSelectedItems();

		// If visible, switch the property sheet to the track item property page
		OnShowItemProperties();

		// If it exists, refresh the track item property page
		RefreshPropertyPage();

		// Sync with DirectMusic
		m_pTrackMgr->SyncWithDirectMusic();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::MarkSelectedItems

// ORs dwFlags with the m_dwBits of each selected item
void CWaveStrip::MarkSelectedItems( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// It's selected - update m_dwBits
			pItem->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DeleteMarked

// deletes items marked by given flag
void CWaveStrip::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if any of the specified dwFlags are set in this item
		if( pItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item from the selected variations
			pItem->m_dwVariations &= ~m_StripUI.m_dwVariationBtns;
			if( pItem->m_dwVariations == 0 )
			{
				m_lstTrackItems.RemoveAt(pos2);
				delete pItem;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DeleteSelectedItems

void CWaveStrip::DeleteSelectedItems()
{
	// Start iterating through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the current item is selected
		if( pItem->m_fSelected )
		{
			// This item is selected, remove it from the selected variations
			pItem->m_dwVariations &= ~m_StripUI.m_dwVariationBtns;
			if( pItem->m_dwVariations == 0 )
			{
				m_lstTrackItems.RemoveAt(pos2);
				delete pItem;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetFirstSelectedItem

CTrackItem* CWaveStrip::GetFirstSelectedItem()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Item is selected, return a pointer to it
			return pItem;
		}
	}

	// No items are selected, return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::UnMarkItems

// unmarks flag m_dwUndermined field CTrackItems in list
void CWaveStrip::UnMarkItems( DWORD dwFlags )
{
	// Iterate through the list of items (all variations)
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lstTrackItems.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::TimelineToWavePartUI

void CWaveStrip::TimelineToWavePartUI( void )
{
	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return;
	}

	VARIANT var;

	// m_StripUI.m_nStripView handled by WM_SIZE 
	// m_StripUI.m_lStripHeight handled by WM_SIZE 

	// Initialize state flags
	m_StripUI.m_dwFlagsUI = 0;

	// Get nbr layers
	m_StripUI.m_nNbrLayers = m_lstLayers.GetCount();
		
	// Active strip?
	if( SUCCEEDED ( m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
	{
		IUnknown* pIUnknown = V_UNKNOWN(&var);
		if( pIUnknown )
		{
			IDMUSProdStrip* pIActiveStrip;
			if( SUCCEEDED ( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIActiveStrip ) ) )
			{
				if( pIActiveStrip == (IDMUSProdStrip *)this )
				{
					m_StripUI.m_dwFlagsUI |= PARTUI_ACTIVESTRIP;
				}

				RELEASE( pIActiveStrip );
			}
			
			RELEASE( pIUnknown );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::WavePartUIToTimeline

void CWaveStrip::WavePartUIToTimeline( void )
{
	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return;
	}

	VARIANT var;

	// Set strip view
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		var.vt = VT_I4;
		V_I4(&var) = m_StripUI.m_nStripView;
		m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIPVIEW, var );
	}

	// Set strip height
	if( m_StripUI.m_nStripView != SV_MINIMIZED )
	{
		var.vt = VT_I4;
		V_I4(&var) = m_StripUI.m_lStripHeight;
		m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, var );
	}

	// Determine wave height
	m_nWaveHeight = long( MAX_WAVE_HEIGHT * m_StripUI.m_dblVerticalZoom + HORIZ_LINE_HEIGHT );

	// Set the scrollbar
	long lOrigVerticalScrollPos = m_StripUI.m_lVerticalScrollYPos;
	m_StripUI.m_lVerticalScrollYPos = -1;	// Force change
	SetVerticalScrollYPos( lOrigVerticalScrollPos );
	RefreshVerticalScrollBarUI();

	// Set nbr of layers
	if( m_StripUI.m_nNbrLayers <= 0 )
	{
		m_StripUI.m_nNbrLayers = 1;
	}
	CreateLayerForIndex( m_StripUI.m_nNbrLayers - 1 );

	// Set active strip
	// Other strips do not reset the active strip
//	if( m_StripUI.m_dwFlagsUI & PARTUI_ACTIVESTRIP )
//	{
//		var.vt = VT_UNKNOWN;
//		if( SUCCEEDED( QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
//		{
//			m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
//		}
//	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::RefreshVerticalScrollBarUI

HRESULT	CWaveStrip::RefreshVerticalScrollBarUI( void )
{
	// Reposition and resize vertical scrollbar
	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// SV_MINIMIZED
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		m_VerticalScrollBar.ShowWindow( FALSE );
		return S_OK;
	}

	// SV_NORMAL
	VARIANT var;
	RECT rectFBar;
	RECT rectEntireStrip;

	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED (m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
	{
		return E_FAIL;
	}

	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectEntireStrip;
	if( FAILED (m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_ENTIRE_STRIP_RECT, &var ) ) )
	{
		return E_FAIL;
	}

	// Store the original position
	CRect rectScrollBarOrig;
	m_VerticalScrollBar.GetClientRect( &rectScrollBarOrig );

	// Figure out the new position
	CRect rectScrollBar;
	rectScrollBar.left = rectFBar.right - sm_nZoomBtnWidth - BORDER_HORIZ_WIDTH - rectScrollBarOrig.right;
	rectScrollBar.right = rectFBar.right - sm_nZoomBtnWidth - BORDER_HORIZ_WIDTH;
	rectScrollBar.bottom = rectFBar.bottom;
	rectScrollBar.top = rectFBar.top;

	// Move the scroll bar
	if( rectScrollBar != rectScrollBarOrig )
	{
		m_VerticalScrollBar.MoveWindow( &rectScrollBar, TRUE );
	}

	// Get the strip's height
	int nStripHeight = m_StripUI.m_lStripHeight - VARIATION_BUTTONS_HEIGHT;
	nStripHeight = max( 0, nStripHeight );

	if( nStripHeight >= GetNbrLayers() * m_nWaveHeight )
	{
		// If we can view all layers, disable the scrollbar
		SetVerticalScrollYPos( 0 );
		m_VerticalScrollBar.EnableScrollBar( ESB_DISABLE_BOTH );
	}
	else
	{
		// Enable the scrollbar
		m_VerticalScrollBar.EnableScrollBar( ESB_ENABLE_BOTH );

		// Set the new page size, min, max
		SCROLLINFO si;
		si.cbSize = sizeof( SCROLLINFO );
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nMin = 0;
		si.nMax = m_lstLayers.GetCount();
		si.nPage = nStripHeight / m_nWaveHeight;
		m_VerticalScrollBar.SetScrollInfo( &si, TRUE );

		// Display as many layers as possible
		int nCurPos = m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight;
		if( (si.nMax - (UINT)nCurPos) < si.nPage )
		{
			SetVerticalScrollYPos( ((si.nMax - si.nPage) + 1) * m_nWaveHeight );
		}
	}

	m_VerticalScrollBar.ShowWindow( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnCreate

HRESULT CWaveStrip::OnCreate( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Register our clipboard formats
	RegisterClipboardFormats();

	// Create the vertical scrollbar
	if( m_VerticalScrollBar.GetSafeHwnd() == NULL )
	{
		HWND hwnd = GetTimelineHWND();
		if( hwnd )
		{
			CWnd wnd;
			wnd.Attach( hwnd );

			CRect rectScrollBar = CRect(0, 0, 40, 100);
			m_VerticalScrollBar.Create( SBS_RIGHTALIGN | SBS_VERT | WS_CHILD | WS_CLIPSIBLINGS,
										rectScrollBar, &wnd, IDC_LAYER_SCROLLBAR );
			m_VerticalScrollBar.SetWaveStrip( this );

			wnd.Detach();
		}
	}

	// Load button bitmaps
	if( InterlockedIncrement( &sm_lBitmapRefCount ) == 1 )
	{
		if( sm_bmpLocked.GetSafeHandle() == NULL )
		{
			sm_bmpLocked.LoadBitmap( IDB_LOCKED );
		}
		if( sm_bmpUnlocked.GetSafeHandle() == NULL )
		{
			sm_bmpUnlocked.LoadBitmap( IDB_UNLOCKED );
		}
		if( sm_bmpVarBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpVarBtnUp.LoadBitmap( IDB_VARBTN_UP );
		}
		if( sm_bmpVarBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpVarBtnDown.LoadBitmap( IDB_VARBTN_DOWN );
		}
		if( sm_bmpVarInactiveBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpVarInactiveBtnUp.LoadBitmap( IDB_VARBTN_INACTIVE_UP );
		}
		if( sm_bmpVarInactiveBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpVarInactiveBtnDown.LoadBitmap( IDB_VARBTN_INACTIVE_DOWN );
		}
		if( sm_bmpVarGutterBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpVarGutterBtnUp.LoadBitmap( IDB_VARGUTTER_UP );
		}
		if( sm_bmpVarGutterBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpVarGutterBtnDown.LoadBitmap( IDB_VARGUTTER_DOWN );
		}

		BITMAP bmParam;
		if( sm_bmpZoomInBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpZoomInBtnUp.LoadBitmap( IDB_ZOOMIN_UP );
			sm_bmpZoomInBtnUp.GetBitmap( &bmParam );
			sm_nZoomBtnHeight = bmParam.bmHeight;
			sm_nZoomBtnWidth = bmParam.bmWidth;
			sm_nLayerBtnWidth = bmParam.bmWidth;
		}
		if( sm_bmpZoomInBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpZoomInBtnDown.LoadBitmap( IDB_ZOOMIN_DOWN );
		}
		if( sm_bmpZoomOutBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpZoomOutBtnUp.LoadBitmap( IDB_ZOOMOUT_UP );
		}
		if( sm_bmpZoomOutBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpZoomOutBtnDown.LoadBitmap( IDB_ZOOMOUT_DOWN );
		}
	}
	
	// If we're currently playing, enable the variation timer.
	if( m_pTrackMgr->m_pIDMSegmentState )
	{
		EnableVariationTimer();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnDestroy

HRESULT CWaveStrip::OnDestroy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete the Bitmap buttons
	if( InterlockedDecrement( &sm_lBitmapRefCount ) == 0 )
	{
		if( sm_bmpLocked.GetSafeHandle() != NULL )
		{
			sm_bmpLocked.DeleteObject();
		}
		if( sm_bmpUnlocked.GetSafeHandle() != NULL )
		{
			sm_bmpUnlocked.DeleteObject();
		}
		if( sm_bmpVarBtnUp.GetSafeHandle() != NULL )
		{
			sm_bmpVarBtnUp.DeleteObject();
		}
		if( sm_bmpVarBtnDown.GetSafeHandle() != NULL )
		{
			sm_bmpVarBtnDown.DeleteObject();
		}
		if( sm_bmpVarInactiveBtnUp.GetSafeHandle() != NULL )
		{
			sm_bmpVarInactiveBtnUp.DeleteObject();
		}
		if( sm_bmpVarInactiveBtnDown.GetSafeHandle() != NULL )
		{
			sm_bmpVarInactiveBtnDown.DeleteObject();
		}
		if( sm_bmpVarGutterBtnUp.GetSafeHandle() != NULL )
		{
			sm_bmpVarGutterBtnUp.DeleteObject();
		}
		if( sm_bmpVarGutterBtnDown.GetSafeHandle() != NULL )
		{
			sm_bmpVarGutterBtnDown.DeleteObject();
		}
		if( sm_bmpZoomInBtnUp.GetSafeHandle() != NULL )
		{
			sm_bmpZoomInBtnUp.DeleteObject();
		}
		if( sm_bmpZoomInBtnDown.GetSafeHandle() != NULL )
		{
			sm_bmpZoomInBtnDown.DeleteObject();
		}
		if( sm_bmpZoomOutBtnUp.GetSafeHandle() != NULL )
		{
			sm_bmpZoomOutBtnUp.DeleteObject();
		}
		if( sm_bmpZoomOutBtnDown.GetSafeHandle() != NULL )
		{
			sm_bmpZoomOutBtnDown.DeleteObject();
		}
	}

	if( m_VerticalScrollBar.GetSafeHwnd() )
	{
		m_VerticalScrollBar.ShowWindow( FALSE );
		m_VerticalScrollBar.DestroyWindow();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetVerticalScrollYPos

void CWaveStrip::SetVerticalScrollYPos( long lNewVertScrollYPos )
{
	// Make sure lNewVertScrollYPos is multiple of m_nWaveHeight
	lNewVertScrollYPos = lNewVertScrollYPos / m_nWaveHeight;
	lNewVertScrollYPos = lNewVertScrollYPos * m_nWaveHeight;

	// Enforce min/max values
	lNewVertScrollYPos = max( lNewVertScrollYPos, 0 );
	lNewVertScrollYPos = min( lNewVertScrollYPos, m_lstLayers.GetCount() * m_nWaveHeight );
	
	if( lNewVertScrollYPos != m_StripUI.m_lVerticalScrollYPos )
	{
		// Pospone drawing operations - this avoids flickering
		// in the strip's title and maximize/minimize button
		HWND hwnd = GetTimelineHWND();
		if( hwnd )
		{
			::LockWindowUpdate( hwnd );
		}

		// Store the new YPos
		m_StripUI.m_lVerticalScrollYPos = lNewVertScrollYPos;
		
		// Update the timeline
		VARIANT var;
		var.vt = VT_I4;
		V_I4(&var) = m_StripUI.m_lVerticalScrollYPos;
		m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var );

		// Restore drawing operations (and redraw the strip)
		if( hwnd )
		{
			::LockWindowUpdate( NULL );
		}

		// Update the scrollbar position
		if( m_VerticalScrollBar.GetSafeHwnd() )
		{
			m_VerticalScrollBar.SetScrollPos( m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnVScrollFromScrollbar

void CWaveStrip::OnVScrollFromScrollbar( UINT nSBCode, UINT nPos )
{
	int nStripHeight = m_StripUI.m_lStripHeight - VARIATION_BUTTONS_HEIGHT;
	nStripHeight = max( 0, nStripHeight );

	UINT nCurPos = m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight;
	int nNewPos;

	switch( nSBCode )
	{
		case SB_TOP:
			nNewPos = 0;
			break;
		
		case SB_LINEDOWN:
			nNewPos = nCurPos + 1;
			break;
		
		case SB_LINEUP:
			nNewPos = nCurPos - 1;
			break;
		
		case SB_PAGEDOWN:
			nNewPos = nCurPos + (nStripHeight / m_nWaveHeight);
			break;
		
		case SB_PAGEUP:
			nNewPos = nCurPos - (nStripHeight / m_nWaveHeight);
			break;
		
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nNewPos = nPos;
			break;
		
		case SB_BOTTOM:
			nNewPos = GetNbrLayers();
			break;

		case SB_ENDSCROLL:
		default:
			return;
	}

	nNewPos = max( nNewPos, 0 );
	nNewPos = min( nNewPos, GetNbrLayers() );

	long lNewVerticalScrollYPos = nNewPos * m_nWaveHeight;

	if( lNewVerticalScrollYPos != m_StripUI.m_lVerticalScrollYPos )
	{
		SetVerticalScrollYPos( lNewVerticalScrollYPos );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CreateLayerForIndex

CWaveStripLayer* CWaveStrip::CreateLayerForIndex( int nLayerIndex )
{
	if( nLayerIndex < 0 )
	{
		return NULL;
	}

	CWaveStripLayer* pTheLayer = NULL;

	int nNbrLayers = m_lstLayers.GetCount();

	if( nLayerIndex < nNbrLayers )
	{
		// Layer already in list
		POSITION pos = m_lstLayers.FindIndex( nLayerIndex );
		if( pos )
		{
			pTheLayer = m_lstLayers.GetAt( pos );
		}
	}
	else
	{
		// Layer not in list so it needs to be created
		for( ; ; )
		{
			CWaveStripLayer* pLayer = new CWaveStripLayer( this );
			if( pLayer == NULL )
			{
				break;
			}

			m_lstLayers.AddTail( pLayer );
			if( nNbrLayers == nLayerIndex )
			{
				pTheLayer = pLayer;
				break;
			}

			nNbrLayers++;
		}

		RefreshVerticalScrollBarUI();
	}

	return pTheLayer;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IndexToLayer

CWaveStripLayer* CWaveStrip::IndexToLayer( int nLayerIndex )
{
	if( nLayerIndex < 0 )
	{
		return NULL;
	}

	CWaveStripLayer* pTheLayer = NULL;

	int nNbrLayers = m_lstLayers.GetCount();

	if( nLayerIndex < nNbrLayers )
	{
		// Layer already in list
		POSITION pos = m_lstLayers.FindIndex( nLayerIndex );
		if( pos )
		{
			pTheLayer = m_lstLayers.GetAt( pos );
		}
	}

	return pTheLayer;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::LayerToIndex

int CWaveStrip::LayerToIndex( CWaveStripLayer* pLayer )
{
	int nTheIndex = 0;

	if( pLayer )
	{
		int nIndex = 0;

		POSITION pos = m_lstLayers.GetHeadPosition();
		while( pos )
		{
			CWaveStripLayer* pLayerList = m_lstLayers.GetNext( pos );

			if( pLayerList == pLayer )
			{
				nTheIndex = nIndex;
				break;
			}

			nIndex++;
		}

	}

	return nTheIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::YPosToLayer

CWaveStripLayer* CWaveStrip::YPosToLayer( long lYPos, bool fNewLayerIsValid )
{
	CWaveStripLayer* pTheLayer = NULL;

	int nLayerIndex = YPosToLayerIndex( lYPos );

	if( nLayerIndex >= 0 )
	{
		if( nLayerIndex < m_lstLayers.GetCount() )
		{
			pTheLayer = IndexToLayer( nLayerIndex );
		}
		else if( nLayerIndex == m_lstLayers.GetCount() )
		{
			if( fNewLayerIsValid )
			{
				pTheLayer = CreateLayerForIndex( nLayerIndex );
			}
		}
	}

	return pTheLayer;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::YPosToLayerIndex

int CWaveStrip::YPosToLayerIndex( long lYPos )
{
	int nLayerIndex = -1;

	lYPos -= VARIATION_BUTTONS_HEIGHT;

	if( lYPos >= m_StripUI.m_lVerticalScrollYPos )
	{
		int nIndex = lYPos / m_nWaveHeight;

		if( nIndex >= 0
		&&  nIndex < GetNbrLayers() )
		{
			nLayerIndex = nIndex;
		}
	}

	return nLayerIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetNbrLayers

int CWaveStrip::GetNbrLayers( void )
{
	// make sure there is at least one layer in the list
	if( m_lstLayers.IsEmpty() )
	{
		CreateLayerForIndex( 0 );
	}

	return m_lstLayers.GetCount() + 1;	// Add 1 for 'New' layer
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SelectLayer

bool CWaveStrip::SelectLayer( CWaveStripLayer* pLayer )
{
	bool fChanged = false;

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		if( pItem->m_pLayer == pLayer )
		{
			if( pItem->m_fSelected != pLayer->m_fSelected )
			{
				pItem->m_fSelected = pLayer->m_fSelected;
				fChanged = true;
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IsLayerEmpty

bool CWaveStrip::IsLayerEmpty( CWaveStripLayer* pLayer )
{
	bool fEmpty = true;

	// Do any items exist in this layer?  (all variations)
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_pLayer == pLayer )
		{
			fEmpty = false;
		}
	}

	return fEmpty;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SyncLayerSelectFlag

BOOL CWaveStrip::SyncLayerSelectFlag( CWaveStripLayer* pLayer )
{
	BOOL fNewSelectFlag = pLayer->m_fSelected;
	BOOL fFirstTime = TRUE;

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// If this item's variations are not displaying, skip it
		if( !(pItem->m_dwVariations & m_StripUI.m_dwVariationBtns) )
		{
			continue;
		}

		if( pItem->m_pLayer == pLayer )
		{
			if( fFirstTime )
			{
				fFirstTime = FALSE;
				fNewSelectFlag = pItem->m_fSelected;
			}

			if( pItem->m_fSelected != fNewSelectFlag )
			{
				fNewSelectFlag = FALSE;
				break;
			}
		}
	}

	return fNewSelectFlag;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CanDeleteLayers

HRESULT CWaveStrip::CanDeleteLayers( void )
{
	POSITION pos = m_lstLayers.GetHeadPosition();
	while( pos )
	{
		CWaveStripLayer* pLayer = m_lstLayers.GetNext( pos );

		if( pLayer->m_fSelected )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DeleteSelectedLayers

HRESULT CWaveStrip::DeleteSelectedLayers( void )
{
	// Display delete prompt when deleting layer will cause
	// items in non-visible variations to be deleted
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		ASSERT( pItem->m_pLayer != NULL );
		if( pItem->m_pLayer 
		&&  pItem->m_pLayer->m_fSelected )
		{
			if( pItem->m_dwVariations & ~m_StripUI.m_dwVariationBtns )
			{
				// Item belongs to variations not displaying
				if( AfxMessageBox( IDS_DELETE_LAYER_PROMPT, MB_OKCANCEL ) == IDCANCEL )
				{
					return S_FALSE;
				}
				break;
			}
		}
	}

	bool fSyncWithDirectMusic = false;

	// Delete items in selected layers (all variations)
	pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		POSITION posCurrent = pos;
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_pLayer 
		&&  pItem->m_pLayer->m_fSelected )
		{
			m_lstTrackItems.RemoveAt( posCurrent );
			delete pItem;

			fSyncWithDirectMusic = true;
		}
	}

	// Delete selected layers
	pos = m_lstLayers.GetHeadPosition();
	while( pos )
	{
		POSITION posCurrent = pos;
		CWaveStripLayer* pLayer = m_lstLayers.GetNext( pos );

		if( pLayer->m_fSelected )
		{
			m_lstLayers.RemoveAt( posCurrent );
			delete pLayer;
		}
	}

	// Update the selection regions to include just this selected item
	SelectRegionsFromSelectedItems();

	// Notify the containing segment that we did a paste operation
	m_pTrackMgr->m_nLastEdit = IDS_UNDO_DELETE_LAYERS;
	m_pTrackMgr->OnDataChanged();

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Adjust vertical scroll bar
	if( (m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight) >= m_lstLayers.GetCount() )
	{
		// Get the strip's height
		int nStripHeight = m_StripUI.m_lStripHeight - VARIATION_BUTTONS_HEIGHT;
		nStripHeight = max( 0, nStripHeight );

		// Display last page
		int nPage = nStripHeight / m_nWaveHeight;
		SetVerticalScrollYPos( max(0, (m_lstLayers.GetCount() - nPage) * m_nWaveHeight) );
	}
	RefreshVerticalScrollBarUI();

	// Ensure the track item property page is visible
	OnShowItemProperties();

	// Refresh the track item property page
	RefreshPropertyPage();

	// Sync with DirectMusic
	if( fSyncWithDirectMusic )
	{
		m_pTrackMgr->SyncWithDirectMusic();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InsertTheLayer

HRESULT CWaveStrip::InsertTheLayer( int nLayerIndex )
{
	if( nLayerIndex < 0
	||  nLayerIndex > m_lstLayers.GetCount() )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Create new layer
	CWaveStripLayer* pLayer = new CWaveStripLayer( this );
	if( pLayer == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Place in m_lstLayers
	POSITION pos = m_lstLayers.FindIndex( nLayerIndex );
	if( pos )
	{
		m_lstLayers.InsertBefore( pos, pLayer );
	}
	else
	{
		m_lstLayers.AddTail( pLayer );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::InsertLayers

HRESULT CWaveStrip::InsertLayers( int nLayerIndex )
{
	if( nLayerIndex < 0
	||  nLayerIndex > m_lstLayers.GetCount() )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Create the new layer(s)
	HRESULT hr = InsertTheLayer( nLayerIndex );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Notify the containing segment that we inserted layer(s)
	m_pTrackMgr->m_nLastEdit = IDS_UNDO_INSERT_LAYER;
	m_pTrackMgr->OnDataChanged();
	
	// Compute page size
	int nStripHeight = m_StripUI.m_lStripHeight - VARIATION_BUTTONS_HEIGHT;
	nStripHeight = max( 0, nStripHeight );
	int nPageSize = nStripHeight / m_nWaveHeight;

	// Determine the current scroll position
	int nCurPos = m_StripUI.m_lVerticalScrollYPos / m_nWaveHeight;

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	RefreshVerticalScrollBarUI();

	// Set new vertical scroll position
	if( nCurPos == nLayerIndex )	// top of page
	{
		if( nCurPos > 0 )
		{
			SetVerticalScrollYPos( (nCurPos - 1) * m_nWaveHeight );
		}
	}
	else if( (nCurPos + nPageSize) == nLayerIndex )	// bottom of page
	{
		SetVerticalScrollYPos( (nCurPos + 1) * m_nWaveHeight );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IndexToLayerYCoords

void CWaveStrip::IndexToLayerYCoords( int nLayerIndex, RECT* pRect )
{
	if( nLayerIndex >= 0 )
	{
		pRect->top = nLayerIndex * m_nWaveHeight;
		pRect->bottom = pRect->top + m_nWaveHeight;
	}
	else
	{
		pRect->top = 1;
		pRect->bottom = STRIP_MINIMIZE_HEIGHT -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::DoGridsPerSecond

void CWaveStrip::DoGridsPerSecond( void )
{
	CGridsPerSecondDlg dlgGridsPerSecond;

	dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond = m_StripUI.m_nSnapToRefTime_GridsPerSecond;

	if( dlgGridsPerSecond.DoModal() == IDOK )
	{
		if( dlgGridsPerSecond.m_fAllParts )
		{
			if( m_pTrackMgr->AllStrips_SetNbrGridsPerSecond(dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond) )
			{
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_NBR_GRIDS_PER_SECOND;
				m_pTrackMgr->OnDataChanged();
				m_pTrackMgr->AllStrips_Invalidate();
			}
		}
		else
		{
			if( m_StripUI.m_nSnapToRefTime_GridsPerSecond != dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond )
			{
				m_StripUI.m_nSnapToRefTime_GridsPerSecond = dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond;
				m_pTrackMgr->m_nLastEdit = IDS_UNDO_NBR_GRIDS_PER_SECOND;
				m_pTrackMgr->OnDataChanged();
				m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
			}
		}

		// Update the property page
		RefreshPropertyPage();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SetSnapTo

void CWaveStrip::SetSnapTo( WORD wID )
{
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		m_StripUI.m_nSnapToRefTime = wID;

		// Update the Timeline with the snap-to setting (if possible)
		if( (wID == IDM_SNAP_NONE)
		||	(wID == IDM_SNAP_GRID) )
		{
			// Create a variant to store the timeline snap-to setting in
			VARIANT var;
			var.vt = VT_I4;

			switch( wID )
			{
			case IDM_SNAP_NONE:
				V_I4(&var) = DMUSPROD_TIMELINE_SNAP_NONE;
				break;
			case IDM_SNAP_GRID:
				V_I4(&var) = DMUSPROD_TIMELINE_SNAP_GRID;
				break;
			}

			// Now, set the TP_SNAP_TO property
			m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_SNAP_TO, var );
		}
	}
	else
	{
		m_StripUI.m_nSnapToMusicTime = wID;

		// Update the Timeline with the snap-to setting

		// Convert from the our own SNAP_TO setting to the timeline's SNAP_TO setting
		DMUSPROD_TIMELINE_SNAP_TO tlSnapTo;

		switch( wID )
		{
		case IDM_SNAP_NONE:
			tlSnapTo = DMUSPROD_TIMELINE_SNAP_NONE;
			break;
		case IDM_SNAP_GRID:
			tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
			break;
		case IDM_SNAP_BEAT:
			tlSnapTo = DMUSPROD_TIMELINE_SNAP_BEAT;
			break;
		case IDM_SNAP_BAR:
			tlSnapTo = DMUSPROD_TIMELINE_SNAP_BAR;
			break;
		default:
			ASSERT(FALSE);
			tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
			break;
		}

		// Now, set the TP_SNAP_TO property
		VARIANT var;
		var.vt = VT_I4;
		V_I4(&var) = tlSnapTo;
		m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_SNAP_TO, var );
	}

	// Redraw our strip
	m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SnapXPosToUnknownTime

HRESULT CWaveStrip::SnapXPosToUnknownTime( long lXPos, REFERENCE_TIME* prtSnapTime )
{
	if( prtSnapTime == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}
	*prtSnapTime = 0;

	if( lXPos < 0 )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	MUSIC_TIME mtTime;
	if( FAILED ( m_pTrackMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime ) ) )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	REFERENCE_TIME rtTime;
	if( FAILED ( m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtTime ) ) )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	return SnapUnknownTime( rtTime, prtSnapTime );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SnapUnknownTime

HRESULT CWaveStrip::SnapUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime )
{
	if( prtSnapTime == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}
	*prtSnapTime = 0;

	if( rtTime < 0 )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		long lMinute;
		long lSec;
		long lGrid;
		long lMs;
		m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );

		if( m_StripUI.m_nSnapToRefTime != IDM_SNAP_NONE )
		{
			if( lMs < 0 )
			{
				m_pTrackMgr->MinSecGridMsToRefTime( this, lMinute, lSec, (lGrid - 1), 0, &rtTime );
				m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );
			}
		}

		switch( m_StripUI.m_nSnapToRefTime )
		{
			case IDM_SNAP_NONE:
				// Nothing to do
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MinSecGridMsToRefTime( this, lMinute, lSec, lGrid, 0, &rtTime );
				break;
			
			case IDM_SNAP_SECOND:
				m_pTrackMgr->MinSecGridMsToRefTime( this, lMinute, lSec, 0, 0, &rtTime );
				break;
		}
	}
	else
	{
		long lMeasure;
		long lBeat;
		long lGrid;
		long lTick;
		m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)rtTime, &lMeasure, &lBeat, &lGrid, &lTick );

		MUSIC_TIME mtTime;
		if( m_StripUI.m_nSnapToMusicTime != IDM_SNAP_NONE )
		{
			if( lTick < 0 )
			{
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, (lGrid - 1), 0, &mtTime );
				m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTime, &lMeasure, &lBeat, &lGrid, &lTick );
			}
		}

		switch( m_StripUI.m_nSnapToMusicTime )
		{
			case IDM_SNAP_NONE:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );
				break;

			case IDM_SNAP_GRID:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, 0, &mtTime );
				break;
			
			case IDM_SNAP_BEAT:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTime );
				break;
			
			case IDM_SNAP_BAR:
				m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, 0, 0, 0, &mtTime );
				break;

			default:
				ASSERT( 0 );	// Should not happen!
				mtTime = 0;
		}

		rtTime = mtTime;
	}

	*prtSnapTime = rtTime;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SnapUnknownTimeToDisplayPosition

HRESULT	CWaveStrip::SnapUnknownTimeToDisplayPosition( REFERENCE_TIME rtTime, long* plPosition )
{
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	SnapUnknownTime( rtTime, &rtTime );

	if( m_pTrackMgr->IsRefTimeTrack() == false )
	{
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)rtTime, &rtTime );
	}

	m_pTrackMgr->m_pTimeline->RefTimeToPosition( rtTime, plPosition );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SnapPositionToStartEndClocks

HRESULT CWaveStrip::SnapPositionToStartEndClocks( long lXPos, MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd )
{
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Determine start time
	REFERENCE_TIME rtStartTime;
	SnapXPosToUnknownTime( lXPos, &rtStartTime );

	// Determine end time
	REFERENCE_TIME rtEndTime;
	SnapUnknownToNextIncrement( rtStartTime, &rtEndTime );

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtStartTime, pmtStart );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtEndTime, pmtEnd );
	}
	else
	{
		*pmtStart = (MUSIC_TIME)rtStartTime;
		*pmtEnd = (MUSIC_TIME)rtEndTime;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::SnapUnknownToNextIncrement

HRESULT CWaveStrip::SnapUnknownToNextIncrement( REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime )
{
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	SnapUnknownTime( rtTime, &rtTime );

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		long lMinute, lSecond, lGrid, lMs;
	
		m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSecond, &lGrid, &lMs );
		switch( m_StripUI.m_nSnapToRefTime )
		{
			case IDM_SNAP_NONE:
				lMs++;
				break;

			case IDM_SNAP_GRID:
				lGrid++;
				break;
			
			case IDM_SNAP_SECOND:
				lSecond++;
				break;
		}
		m_pTrackMgr->MinSecGridMsToRefTime( this, lMinute, lSecond, lGrid, lMs, &rtTime );
	}
	else
	{
		long lMeasure, lBeat, lGrid, lTick;
		MUSIC_TIME mtTime;

		m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)rtTime, &lMeasure, &lBeat, &lGrid, &lTick );
		switch( m_StripUI.m_nSnapToMusicTime )
		{
			case IDM_SNAP_NONE:
				lTick++;
				break;

			case IDM_SNAP_GRID:
				lGrid++;
				break;
			
			case IDM_SNAP_BEAT:
				lBeat++;
				break;
			
			case IDM_SNAP_BAR:
				lMeasure++;
				break;
		}
		m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, lGrid, lTick, &mtTime );
		rtTime = mtTime;
	}

	*prtSnapTime = rtTime;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::IsOneGrid

bool CWaveStrip::IsOneGrid( void )
{
	if( m_pTrackMgr
	&& 	m_pTrackMgr->IsRefTimeTrack() )
	{
		if( m_StripUI.m_nSnapToRefTime_GridsPerSecond == 1 )
		{
			return true;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetPChannel

DWORD CWaveStrip::GetPChannel( void )
{
	return m_dwPChannel;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::RefreshAllWaves

void CWaveStrip::RefreshAllWaves()
{
	CWaitCursor wait;

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		pItem->RefreshWave();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::GetReferencedNodes

void CWaveStrip::GetReferencedNodes( DWORD *pdwIndex, DMUSProdReferencedNodes *pDMUSProdReferencedNodes )
{
	// Iterate through all wave items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_FileRef.pIDocRootNode )
		{
			if( pDMUSProdReferencedNodes->apIDMUSProdNode
			&&	pDMUSProdReferencedNodes->dwArraySize > *pdwIndex )
			{
				pDMUSProdReferencedNodes->apIDMUSProdNode[*pdwIndex] = pItem->m_FileRef.pIDocRootNode;
				pDMUSProdReferencedNodes->apIDMUSProdNode[*pdwIndex]->AddRef();
			}

			// Increment the number of nodes we've found
			(*pdwIndex)++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::RefreshPropertyPage

void CWaveStrip::RefreshPropertyPage( void )
{
	// Update the property page, if it exists
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}

	// Always update the status bar whenever the property page is refreshed
	if( m_pTrackMgr )
	{
		m_pTrackMgr->UpdateStatusBarDisplay();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CListSelectedRegion_AddRegion

void CWaveStrip::CListSelectedRegion_AddRegion( long lXPos )
{
	ASSERT( m_pTrackMgr != NULL );

	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegX;
	CMusicTimeConverter cmtEndX;
	SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBegX = mtBeg;
	cmtEndX = mtEnd;

	CSelectedRegion* psr = new CSelectedRegion( cmtBegX, cmtEndX );
	m_pSelectedRegions->AddHead( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CListSelectedRegion_AddRegion

void CWaveStrip::CListSelectedRegion_AddRegion( CTrackItem* pItem )
{
	CMusicTimeConverter cmtBeg;
	CMusicTimeConverter cmtEnd;

	MUSIC_TIME mtTime;
	REFERENCE_TIME rtTime;

	ASSERT( m_pTrackMgr != NULL );
	ASSERT( m_pTrackMgr->m_pTimeline != NULL );

	long lMeasure, lBeat, lGrid, lTick;

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Determine start time
		m_pTrackMgr->RefTimeToMinSecGridMs( this, pItem->m_rtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
		m_pTrackMgr->MinSecGridMsToRefTime( this, lMeasure, lBeat, 0, 0, &rtTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &mtTime );
		cmtBeg = mtTime;

		// Determine end time
		m_pTrackMgr->MinSecGridMsToRefTime( this, lMeasure, (lBeat + 1), 0, 0, &rtTime );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( rtTime, &mtTime );
		cmtEnd = mtTime;
	}
	else
	{
		// Determine start time
		m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)pItem->m_rtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
		m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTime );
		cmtBeg = mtTime;

		// Determine end time
		m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, (lBeat + 1), 0, 0, &mtTime );
		cmtEnd = mtTime;
	}

	CSelectedRegion* psr = new CSelectedRegion( cmtBeg, cmtEnd );
	m_pSelectedRegions->AddHead( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CListSelectedRegion_ToggleRegion

void CWaveStrip::CListSelectedRegion_ToggleRegion( long lXPos )
{
	ASSERT( m_pTrackMgr != NULL );

	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBeg;
	CMusicTimeConverter cmtEnd;
	SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBeg = mtBeg;
	cmtEnd = mtEnd;

	bool fToggled = false;

	// Get a pointer to the first item in the list
	POSITION posCur;
	POSITION pos = m_pSelectedRegions->GetHeadPosition();
	while( pos )
	{
		posCur = pos;
		CSelectedRegion* psr = m_pSelectedRegions->GetNext( pos );

		if( cmtBeg >= psr->Beg()
		&&  cmtBeg < psr->End() )
		{
			if( (psr->Beg() == cmtBeg) 
			&&  (psr->End() == cmtEnd) )
			{
				// Only this beat (or second) is in the selected regions, delete it
				m_pSelectedRegions->RemoveAt( posCur );
				delete psr;
			}
			else if( psr->Beg() == cmtBeg )
			{
				// The region to toggle is at the start of psr - shorten psr
				psr->Beg() = cmtEnd;

				// Check if this region is empty.  If so, delete it
				if( psr->Beg() == psr->End() )
				{
					m_pSelectedRegions->RemoveAt( posCur );
					delete psr;
				}
			}
			else
			{
				// Create a new CSelectedRegion for the first half
				CSelectedRegion *psrNew = new CSelectedRegion( psr->Beg(), cmtBeg );
				m_pSelectedRegions->InsertBefore( posCur, psrNew );

				// Modify the existing CSelectedRegion to display the second half
				psr->Beg() = cmtEnd;
			}
			fToggled = true;
			break;
		}
	}

	if( fToggled == false )
	{
		CListSelectedRegion_AddRegion( lXPos );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CListSelectedRegion_ShiftAddRegion

void CWaveStrip::CListSelectedRegion_ShiftAddRegion( long lXPos )
{
	MUSIC_TIME mtBeg;
	MUSIC_TIME mtEnd;

	// Snap XPos to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegX;
	CMusicTimeConverter cmtEndX;
	SnapPositionToStartEndClocks( lXPos, &mtBeg, &mtEnd );
	cmtBegX = mtBeg;
	cmtEndX = mtEnd;

	// Get the shift-select anchor position
	if( m_pSelectedRegions->IsEmpty() )
	{
		m_pSelectedRegions->SetShiftSelectAnchor( 0 );
	}
	long lShiftSelectAnchorPosition = m_pSelectedRegions->GetShiftSelectAnchor();

	// Snap anchor to measure/beat (or minute/second) boundary
	CMusicTimeConverter cmtBegAnchor;
	CMusicTimeConverter cmtEndAnchor;
	SnapPositionToStartEndClocks( lShiftSelectAnchorPosition, &mtBeg, &mtEnd );
	cmtBegAnchor = mtBeg;
	cmtEndAnchor = mtEnd;

	// Determine shift-select range
	if( cmtBegAnchor > cmtBegX )
	{
		cmtBegAnchor = cmtBegX;
	}
	if( cmtEndAnchor < cmtEndX )
	{
		cmtEndAnchor = cmtEndX;
	}

	// Delete all items in the selected region list
	while( !m_pSelectedRegions->IsEmpty() )
	{
		CSelectedRegion* psr = m_pSelectedRegions->RemoveHead();
		delete psr;
	}

	// Create a selected region and add to list
	CSelectedRegion* psr = new CSelectedRegion( cmtBegAnchor, cmtEndAnchor );
	m_pSelectedRegions->AddTail( psr );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::CListSelectedRegion_Contains

bool CWaveStrip::CListSelectedRegion_Contains( REFERENCE_TIME rtTime )
{
	MUSIC_TIME mtTime;
	m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtTime );

	if( m_pSelectedRegions->Contains( mtTime ) )
	{
		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::KillVariationTimer

void CWaveStrip::KillVariationTimer( void )
{
	if( m_fVariationsTimerActive )
	{
		::KillTimer( m_VerticalScrollBar.GetSafeHwnd(), 1 );
		m_fVariationsTimerActive = false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::EnableVariationTimer

void CWaveStrip::EnableVariationTimer( void )
{
	if( !m_fVariationsTimerActive )
	{
		HWND hwnd = GetTimelineHWND();
		if( hwnd )
		{
			// If the scroll bar exists
			if( m_VerticalScrollBar.GetSafeHwnd() )
			{
				// Update the variations every 400ms
				if( ::SetTimer( m_VerticalScrollBar.GetSafeHwnd(), 1, 400, NULL ) )
				{
					m_fVariationsTimerActive = true;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::UpdatePlayingVariation

void CWaveStrip::UpdatePlayingVariation( bool fRefreshDisplayIfNecessary )
{
	const DWORD dwCurPlayingVariation = m_dwPlayingVariation;

	m_dwPlayingVariation = 0;

	if( m_pTrackMgr->m_pIDMSegmentState
	&&	m_pTrackMgr->m_pIDMPerformance
	&&	m_pTrackMgr->m_pIDMTrack )
	{
		MUSIC_TIME mtNow;
		if( SUCCEEDED( m_pTrackMgr->m_pIDMPerformance->GetTime( NULL, &mtNow ) ) )
		{
			// Retrieve the track state parameter for our track
			DMUS_TRACK_STATE_PARAM trackStateParam;
			trackStateParam.pSegState = m_pTrackMgr->m_pIDMSegmentState;
			trackStateParam.pTrack = m_pTrackMgr->m_pIDMTrack;
			trackStateParam.pTrackState = NULL;
			if( SUCCEEDED( m_pTrackMgr->m_pIDMPerformance->GetParam(GUID_TrackState, 0xFFFFFFFF, 0,  mtNow, NULL, &trackStateParam) ) )
			{
				// Retrieve the currently playing variation for this track
				DMUS_CURRENT_VARIATION_PARAM curVarParam;
				curVarParam.dwPChannel = m_dwPChannel;
				curVarParam.dwVariation = 0;

				// Compute the index of this part
				curVarParam.dwIndex = 0;
				POSITION pos = m_pTrackMgr->m_lstWaveStrips.GetHeadPosition();
				while( pos )
				{
					// Get a pointer to the part ref
					CWaveStrip *pCWaveStrip = m_pTrackMgr->m_lstWaveStrips.GetNext( pos );

					// If we found ourself, exit the loop
					if( pCWaveStrip == this )
					{
						break;
					}

					// If this part ref is on the same PChannel, increment the index value
					if( pCWaveStrip->m_dwPChannel == m_dwPChannel )
					{
						curVarParam.dwIndex++;
					}
				}

				// Finally, try and retrieve the currently playing variation
				IDirectMusicTrack8 *pTrack8;
				if( SUCCEEDED( m_pTrackMgr->m_pIDMTrack->QueryInterface( IID_IDirectMusicTrack8, (void **)&pTrack8 ) ) )
				{
					if( SUCCEEDED( pTrack8->GetParamEx(GUID_CurrentVariation, 0, NULL, &curVarParam, trackStateParam.pTrackState, 0) ) )
					{
						m_dwPlayingVariation = curVarParam.dwVariation;
					}
					pTrack8->Release();
				}
			}
		}
	}

	if( fRefreshDisplayIfNecessary
	&&	(dwCurPlayingVariation != m_dwPlayingVariation) )
	{
		InvalidateVariationBtns();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::OnVariationTimer

void CWaveStrip::OnVariationTimer( void )
{
	UpdatePlayingVariation();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::EnsureFileIsOpen

IDMUSProdNode *CWaveStrip::EnsureFileIsOpen( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	IDMUSProdNode *pIDMUSProdNode = NULL;

	IStream *pStream;
	if( SUCCEEDED( pITimelineDataObject->AttemptRead( m_cfDMUSProdFile, &pStream ) ) )
	{
		CFProducerFile cfProducerFile;
		if( SUCCEEDED( LoadCF_DMUSPROD_FILE( pStream, &cfProducerFile ) ) )
		{
			m_pTrackMgr->m_pDMProdFramework->FindDocRootNodeByFileGUID( cfProducerFile.guidFile, &pIDMUSProdNode );
		}
	}

	return pIDMUSProdNode;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip::LoadCF_DMUSPROD_FILE

HRESULT CWaveStrip::LoadCF_DMUSPROD_FILE( IStream* pIStream, CFProducerFile* pProducerFile )
{
	HRESULT hr = E_FAIL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;
	CString strFileName;
    MMCKINFO ck;
	DWORD dwSize;
    WORD wStructSize;
	ioCFProducerFile iCFProducerFile;
	
	if( pIStream == NULL
	||  pProducerFile == NULL )
	{
		return E_INVALIDARG;
	}

    // Create RIFF stream
	hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Find the FOURCC_CF_DMUSPROD_FILE chunk header
	ck.ckid = FOURCC_CF_DMUSPROD_FILE;
    if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) != 0 )
    {
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Read size of ioCFProducerFile structure
    dwSize = ck.cksize;
    pIStream->Read( &wStructSize, sizeof( wStructSize ), NULL );
    dwSize -= sizeof( wStructSize );

	// Initialize ioCFProducerFile structure
	memset( &iCFProducerFile, 0, sizeof(ioCFProducerFile) );

	// Read ioCFProducerFile structure
    if( wStructSize > sizeof(ioCFProducerFile) )
    {
        pIStream->Read( &iCFProducerFile, sizeof(ioCFProducerFile), NULL );
        StreamSeek( pIStream, wStructSize - sizeof(ioCFProducerFile), STREAM_SEEK_CUR );
    }
    else
    {
        pIStream->Read( &iCFProducerFile, wStructSize, NULL );
    }
    dwSize -= wStructSize;

	// Read filename (unicode format)
	if( dwSize > 0 )
	{
		ReadMBSfromWCS( pIStream, dwSize, &strFileName );
	}

	pProducerFile->guidFile = iCFProducerFile.guidFile;
	pProducerFile->strFileName = strFileName;
	hr = S_OK;

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	return hr;
}
