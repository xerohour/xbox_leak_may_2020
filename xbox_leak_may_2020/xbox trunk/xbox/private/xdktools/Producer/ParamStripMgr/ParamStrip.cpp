// ParamStrip.cpp : Implementation of CParamStrip
#include "stdafx.h"
#include <math.h>
#include <RiffStrm.h>
#include "ParamStripMgrApp.h"
#include "GridsPerSecondDlg.h"
#include "TrackItem.h"
#include "ParamStripMgr.h"
#include "TrackMgr.h"
#include "ParamStrip.h"
#include "Tracker.h"
#include "CurvePropPageMgr.h"
#include "GroupBitsPPG.h"
#include <BaseMgr.h>
#include <SegmentDesigner.h>
#include <SegmentIO.h>
#include <DLLJazzDataObject.h>
#include <PChannelName.h>
#include <dmusicf.h>
#include "GrayOutRect.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This sets the strip's height
#define DEFAULT_STRIP_HEIGHT		20	// TODO: get rid of this later		

// Bitmaps
long CParamStrip::sm_lBitmapRefCount = 0;
CBitmap	CParamStrip::sm_bmpZoomInBtnUp;
CBitmap	CParamStrip::sm_bmpZoomInBtnDown;
CBitmap	CParamStrip::sm_bmpZoomOutBtnUp;
CBitmap	CParamStrip::sm_bmpZoomOutBtnDown;


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

void NormalizeList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset )
{
	UNREFERENCED_PARAMETER(pITimeline);
	UNREFERENCED_PARAMETER(dwGroupBits);

	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		CTrackItem* pItem = list.GetNext(pos);
		ASSERT(pItem);
		if(pItem)
		{
			// This method only called for drag/drop and cut/copy/paste
			// so it is safe to mess with the values that are stored in time fields
			// Keep the duration in the end time
			// Keep the physical tiome in start time
			pItem->m_mtEndTime = (MUSIC_TIME) pItem->m_rtDuration;
			pItem->m_mtStartTime = (MUSIC_TIME) (pItem->m_rtTimePhysical - rtOffset);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip constructor/destructor

CParamStrip::CParamStrip( CTrackMgr* pTrackMgr, CTrackObject* pTrackObject ) : CBaseStrip( pTrackMgr )
{
	if( pTrackMgr == NULL 
	||  pTrackObject == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// Handle to the status bar
	m_hKeyStatusBar = NULL;

	// Set our pointer to our strip manager
	m_pTrackMgr = pTrackMgr;

	// Set our pointer to our strip object
	m_pTrackObject = pTrackObject;

	// Initialize our clipboard formats to 0
	m_cfParamTrack = 0;

	m_fInsertingCurve = FALSE;

	// Initialize our state variables
	m_lVerticalScroll = 0;
	m_fLeftMouseDown = false;
	m_fZoomInDown = false;
	m_fZoomOutDown = false;

	// Initialize the item to toggle (when CTRL-clicking) to NULL
	m_pItemToToggle = NULL;

	// Initialize fields that are persisted (DirectMusic parameter control track data)
	m_dwParamIndex = 0xFFFFFFFF;
	m_dwParamFlagsDM = 0;
}

CParamStrip::~CParamStrip()
{
	CleanUp();
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CleanUp

void CParamStrip::CleanUp( void )
{
	// Clear our pointer to our strip manager
	if( m_pTrackMgr )
	{
		m_pTrackMgr->SetParamStripForPropSheet(this, TRUE);
		m_pTrackMgr = NULL;
	}

	// Delete all the items in m_lstTrackItems
	EmptyList( m_lstTrackItems );
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetTopItemFromMeasureBeat

CTrackItem* CParamStrip::GetTopItemFromMeasureBeat( long lMeasure, long lBeat )
{
	CTrackItem* pTheItem = NULL;
	CTrackItem* pFirstItem = NULL;
	CTrackItem* pItem;

	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat )
		{
			if( pFirstItem == NULL )
			{
				pFirstItem = pItem;
			}

			if( pItem->m_wFlagsUI & RF_TOP_ITEM )
			{
				pTheItem = pItem;
				break;
			}
		}

		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}
	}

	if( pTheItem == NULL )
	{
		pTheItem = pFirstItem;
	}

	return pTheItem;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip IDMUSProdStrip implementation

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
// CParamStrip::AdjustTopItem - helper method for Draw()

void CParamStrip::AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pFirstSelectedItem = NULL;
	CTrackItem* pTopItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			break;
		}

		if( pItem->m_fSelected )
		{
			if( pFirstSelectedItem == NULL )
			{
				pFirstSelectedItem = pItem;
			}
		}

		if( pItem->m_wFlagsUI & RF_TOP_ITEM )
		{
			if( pTopItem == NULL )
			{
				pTopItem = pItem;
			}
			else
			{
				// Can only have one top item
				pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
			}
		}
	}

	if( pTopItem )
	{
		if( pTopItem->m_fSelected == FALSE )
		{
			if( pFirstSelectedItem )
			{
				// pTopItem is NOT selected so it shouldn't be the top item
				pTopItem->m_wFlagsUI &= ~RF_TOP_ITEM;
						
				// pFirstSelectedItem IS selected so make it the top item
				pFirstSelectedItem->m_wFlagsUI |= RF_TOP_ITEM;
			}
		}
	}
	else if( pFirstSelectedItem )
	{
		// Make the selected item the top item
		pFirstSelectedItem->m_wFlagsUI |= RF_TOP_ITEM;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetNextSelectedTopItem - helper method for Draw()

CTrackItem* CParamStrip::GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_fSelected )
		{
			if( pItem->m_lMeasure != lCurrentMeasure
			||  pItem->m_lBeat != lCurrentBeat )
			{
				pNextItem = GetTopItemFromMeasureBeat( pItem->m_lMeasure, pItem->m_lBeat );
				ASSERT( pNextItem != NULL );	// Should not happen!
				break;
			}
		}
	}

	return pNextItem;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetNextTopItem - helper method for Draw()

CTrackItem* CParamStrip::GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat )
{
	CTrackItem* pNextItem = NULL;
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure != lCurrentMeasure
		||  pItem->m_lBeat != lCurrentBeat )
		{
			pNextItem = GetTopItemFromMeasureBeat( pItem->m_lMeasure, pItem->m_lBeat );
			ASSERT( pNextItem != NULL );	// Should not happen!
			break;
		}
	}

	return pNextItem;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetNextSelectedItem - helper method for Draw()

CTrackItem* CParamStrip::GetNextSelectedItem( POSITION pos )
{
		// Get a pointer to the selected item
//	CTrackItem* pItem = m_lstTrackItems.GetAt( posItem );

	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_fSelected )
		{
			return pItem;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetNextItem - helper method for Draw()

CTrackItem* CParamStrip::GetNextItem( POSITION pos )
{
	CTrackItem* pItem;

	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		return pItem;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DrawMinimizedStrip

HRESULT	CParamStrip::DrawMinimizedStrip( HDC hDC, RECT* pRectStrip )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL || m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return E_UNEXPECTED;
	}

	CString strText;
	// Format text depicting type of curves in this Part
	if(m_pTrackObject->IsEmptyTrackObject() == TRUE)
	{
		strText.LoadString(IDS_NO_PARAM_TRACKS_TEXT);
	}
	else
	{
		strText = m_pTrackObject->GetMinimizedStripText();
	}

	// Set font
	HFONT pFontOld = NULL;

	if( theApp.m_pParamStripFont == NULL )
	{
		theApp.m_pParamStripFont = new CFont;

		if( theApp.m_pParamStripFont )
		{
			CString strFontName;
			if( !strFontName.LoadString( IDS_PARAM_STRIP_FONTNAME ) )
			{
				strFontName = _T("Arial");
			}
			if( theApp.m_pParamStripFont->CreatePointFont( 80, strFontName, CDC::FromHandle(hDC) ) == FALSE )
			{
				delete theApp.m_pParamStripFont;
				theApp.m_pParamStripFont = NULL;
			}
		}
	}
	if( theApp.m_pParamStripFont )
	{
		pFontOld = (HFONT)::SelectObject( hDC, theApp.m_pParamStripFont->GetSafeHandle() );
	}

	// Draw text depicting type of curves in this Part
	pRectStrip->left = 4;
	::DrawText( hDC, strText, -1, pRectStrip, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	if( pFontOld )
	{
		::SelectObject( hDC, pFontOld );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DrawCurve

void CParamStrip::DrawCurve( CTrackItem* pTrackItem, HDC hDC, RECT* pRectStrip, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	COLORREF crNoteColor = RGB(0, 0, 255);
	COLORREF crSelectedNoteColor = RGB(255, 0, 0);

	if(m_pTrackObject->IsDisabled())
	{
		crNoteColor = RGB(166, 166, 166);
	}

	float fltLastValue = (float)0xFFFFFFFF;
	short nPosValue = 0;
	long lCurveStartClock = 0;
	
	VARIANT var;
	CRect rect;

	lCurveStartClock  = pTrackItem->m_mtStartTime;

	// Get Strip height and center
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}
	int nStripCenter = pRectStrip->top + (nStripHeight >> 1);

	float fValueRange = m_ParamInfo.mpdMaxValue - m_ParamInfo.mpdMinValue;
	float fltYScale = (float)nStripHeight / fValueRange;
	float fltZeroValue = m_ParamInfo.mpdNeutralValue;
	
	// Get MUSIC_TIME increment
	MUSIC_TIME mtIncrement;
	pTimeline->PositionToClocks( 1, &mtIncrement );

	// Initialize rectFrame and rectSelect
	pTrackItem->m_rectFrame.SetRectEmpty();
	pTrackItem->m_rectSelect.SetRectEmpty();

	pTrackItem->m_mtCurrent = 0;
	pTrackItem->m_mtDuration = pTrackItem->m_mtEndTime - pTrackItem->m_mtStartTime;
	pTimeline->ClocksToPosition( lCurveStartClock + pTrackItem->m_mtCurrent, &rect.right );

	BOOL fFirstTime = TRUE;

	// Draw the curve
	while( pTrackItem->m_mtCurrent <= pTrackItem->m_mtDuration )
	{
		// Compute value of curve at m_mtCurrent
		float fltValue = pTrackItem->ComputeCurve( NULL );
		
		nPosValue = ValueToYPos(fltValue);

		// Increment time offset into Curve
		pTrackItem->m_mtCurrent += mtIncrement;

		if( fltValue != fltLastValue)
		{
			fltLastValue = fltValue;

			// Set rect left
			rect.left = rect.right;

			// Set rect right
			pTimeline->ClocksToPosition( lCurveStartClock + pTrackItem->m_mtCurrent, &rect.right );


			if( UseCenterLine() )
			{
				// Set rect top and bottom
				nPosValue = nStripCenter - nPosValue;

				// Draw Curves around central axis
				if( fltValue < 0 )
				{
					rect.top = nStripCenter;
					rect.bottom = nStripHeight - nPosValue;
				}
				else
				{
					rect.top = nPosValue;
					rect.bottom = nStripCenter;
				}
			}
			else
			{
				// Set rect top and bottom
				nPosValue = nStripHeight - nPosValue;

				rect.top = nPosValue;
				rect.bottom = nStripHeight;
			}

			if( rect.bottom <= rect.top )
			{
				rect.top = rect.bottom - 1;
			}

			rect.left -= lXOffset;
			rect.right -= lXOffset;

			int nOrigRectRight = rect.right;
			rect.right = max( rect.right, rect.left + 2 );

			// Draw this part of the Curve
			if( pTrackItem->m_fSelected )
			{
				::SetBkColor( hDC, crSelectedNoteColor );
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			}
			else
			{
				::SetBkColor( hDC, crNoteColor );
				::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			}
			
			rect.right = nOrigRectRight;

			rect.left += lXOffset;
			rect.right += lXOffset;

			// Adjust rectSelect right, top, and bottom
			if( fFirstTime )
			{
				fFirstTime = FALSE;
				pTrackItem->m_rectSelect.SetRect( rect.left, rect.top, rect.right, rect.bottom );
			}
			else
			{
				pTrackItem->m_rectSelect.right = rect.right;
				pTrackItem->m_rectSelect.top = min( pTrackItem->m_rectSelect.top, rect.top );
				pTrackItem->m_rectSelect.bottom = max( pTrackItem->m_rectSelect.bottom, rect.bottom );
			}
		}
	}

	// Set rectFrame
	pTrackItem->m_rectFrame.left = pTrackItem->m_rectSelect.left;
	pTrackItem->m_rectFrame.right = pTrackItem->m_rectSelect.right;
	if( pTrackItem->m_fltStartValue < pTrackItem->m_fltEndValue )
	{
		pTrackItem->m_rectFrame.top = nStripHeight - ValueToYPos( pTrackItem->m_fltEndValue );
		pTrackItem->m_rectFrame.bottom = nStripHeight - ValueToYPos( pTrackItem->m_fltStartValue );
	}
	else
	{
		pTrackItem->m_rectFrame.top = nStripHeight - ValueToYPos( pTrackItem->m_fltStartValue );
		pTrackItem->m_rectFrame.bottom = nStripHeight - ValueToYPos( pTrackItem->m_fltEndValue );
	}

	pTrackItem->m_rectFrame.left -= lXOffset;
	pTrackItem->m_rectFrame.right -= lXOffset;

	::FrameRect( hDC, &pTrackItem->m_rectFrame, (HBRUSH)GetStockObject(DKGRAY_BRUSH) );

	pTrackItem->m_rectFrame.left += lXOffset;
	pTrackItem->m_rectFrame.right += lXOffset;
	
	// draw selection handles if curve is selected
	if( pTrackItem->m_fSelected )
	{
		CRect selRect = pTrackItem->m_rectFrame;
		VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
		VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
		
		CCurveTracker curveTracker(&selRect);
		curveTracker.Draw( CDC::FromHandle(hDC) );
	}
}



/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DrawMaximizedStrip

HRESULT	CParamStrip::DrawMaximizedStrip(HDC hDC, STRIPVIEW sv, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}
	
	// Save the DC
	int iSavedDC = ::SaveDC( hDC );

	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Draw Bar, Beat, Grid lines
	if(m_pTrackMgr->IsRefTimeTrack() )
	{
		DrawClockVerticalLines(hDC, lXOffset);
	}
	else
	{
		DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
		pTimeline->DrawMusicLines(hDC, ML_DRAW_MEASURE_BEAT_GRID, dwGroupBits, 0, lXOffset);
	}

	// Draw horizontal lines
	DrawHorizontalLines( hDC, rectClip );

	// Highlight range selected in timeline
	HighlightTimelineRange( hDC, lXOffset );

	VARIANT var;

	// Store strip height
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}

	// DRAW CENTER LINE
	if( UseCenterLine() )
	{
		HPEN penShadow = ::CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) );
		if( penShadow )
		{
			int nStripCenter = pRectStrip->top + (nStripHeight >> 1);

			HPEN penOld = (HPEN)::SelectObject( hDC, penShadow );
			::SetROP2( hDC, R2_COPYPEN );

			::MoveToEx( hDC, pRectClip->left, nStripCenter, NULL );
			::LineTo( hDC, pRectClip->right, nStripCenter );

			if( penOld )
			{
				::SelectObject( hDC, penOld );
				::DeleteObject( penShadow );
			}
		}
	}

	// Create overlapping Curve pen
	HPEN penOld = NULL;
	HPEN penOverlappingLine = ::CreatePen( PS_DOT, 1, ::GetSysColor(COLOR_WINDOW) );
	if( penOverlappingLine )
	{
		penOld = (HPEN)::SelectObject( hDC, penOverlappingLine );
	}

	// Set up work fields
	long lStripStartClock =0;
	long lStripEndClock = 0;
	long lTimelineClockLength = 0;

	pTimeline->PositionToClocks( pRectClip->left + lXOffset, &lStripStartClock );
	pTimeline->PositionToClocks( pRectClip->right + lXOffset, &lStripEndClock );
	
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	lTimelineClockLength = V_I4(&var);

	lStripEndClock = min( lStripEndClock, lTimelineClockLength );
	lStripStartClock = min( lStripStartClock, lStripEndClock - 1 );

	CRect rect;
	CRect* pCurveRect;
	CPtrList lstRects;

	long lCurveStartClock = 0;
	long lCurveEndClock = 0;

	RECT rectFrame;
	CRect rectSelect;

	BOOL fHaveSelected = FALSE;

	// DRAW UNSELECTED CURVES
	CTrackItem* pTrackItem = NULL;
	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		pTrackItem = (CTrackItem*) m_lstTrackItems.GetNext(position);
		ASSERT(pTrackItem);

		if(pTrackItem->m_fSelected)
		{
			fHaveSelected = TRUE;
		}
		else
		{


			lCurveStartClock = pTrackItem->m_mtStartTime;
			lCurveEndClock = pTrackItem->m_mtEndTime;

			rectFrame = pTrackItem->m_rectFrame;
			rectSelect = pTrackItem->m_rectSelect;

			if( lCurveStartClock <= lStripEndClock )
			{
				// Draw this Curve
				DrawCurve( pTrackItem, hDC, pRectStrip, lXOffset );
				rectFrame = pTrackItem->m_rectFrame;
				rectSelect = pTrackItem->m_rectSelect;

				// Frame intersecting rectangles
				POSITION posRect = lstRects.GetHeadPosition();
				while(posRect)
				{
					CRect* pRect = (CRect*) lstRects.GetNext(posRect);

					// Compute intersection with other Curves
					rect.IntersectRect( pCurveRect, &pTrackItem->m_rectSelect );
									
					// Draw it
					if( rect.left != rect.right || rect.top != rect.bottom )
					{
						rect.left -= lXOffset;
						rect.right -= lXOffset;
						::MoveToEx( hDC, rect.left, rect.bottom, NULL );
						::LineTo( hDC, rect.left, rect.top );
						::LineTo( hDC, rect.right, rect.top );
						::LineTo( hDC, rect.right, rect.bottom );
						::LineTo( hDC, rect.left, rect.bottom );
					}
				}

				// Store this Curve's rectangle
				pCurveRect = new CRect( &pTrackItem->m_rectSelect );
				if( pCurveRect )
				{
					lstRects.AddHead( pCurveRect );
				}
			}

			// Set Curve rectFrame and rectSelect
			pTrackItem->m_rectFrame = rectFrame;
			pTrackItem->m_rectSelect = rectSelect;
		}
	}

	// DRAW SELECTED CURVES
	if( fHaveSelected )
	{
		position = m_lstTrackItems.GetHeadPosition();
		while(position)
		{
			CTrackItem* pTrackItem = (CTrackItem*)m_lstTrackItems.GetNext(position);
			ASSERT(pTrackItem);
			if( pTrackItem->m_fSelected )
			{
				// Make sure Curve is showing
				lCurveStartClock = pTrackItem->m_mtStartTime;
				lCurveEndClock = pTrackItem->m_mtEndTime;

				rectFrame = pTrackItem->m_rectFrame;
				rectSelect = pTrackItem->m_rectSelect;

				if( lCurveEndClock >= lStripStartClock )
				{
					// Draw this Curve
					DrawCurve( pTrackItem, hDC, pRectStrip, lXOffset );
					rectFrame = pTrackItem->m_rectFrame;
					rectSelect = pTrackItem->m_rectSelect;
					
					// Frame intersecting rectangles
					POSITION posRect = lstRects.GetHeadPosition();
					while(posRect)
					{
						pCurveRect = (CRect*)lstRects.GetNext(posRect);

						// Compute intersection with other Curves
						rect.IntersectRect( pCurveRect, &pTrackItem->m_rectSelect );
										
						// Draw it
						if( rect.left != rect.right	|| rect.top != rect.bottom )
						{
							rect.left -= lXOffset;
							rect.right -= lXOffset;
							::MoveToEx( hDC, rect.left, rect.bottom, NULL );
							::LineTo( hDC, rect.left, rect.top );
							::LineTo( hDC, rect.right, rect.top );
							::LineTo( hDC, rect.right, rect.bottom );
							::LineTo( hDC, rect.left, rect.bottom );
						}
					}

					// Store this Curve's rectangle
					pCurveRect = new CRect( &pTrackItem->m_rectSelect );
					if( pCurveRect )
					{
						lstRects.AddTail( pCurveRect );
					}
				}

				// Set Curve rectFrame and rectSelect
				pTrackItem->m_rectFrame = rectFrame;
				pTrackItem->m_rectSelect = rectSelect;
			}
		}
	}

	if( penOld )
	{
		::SelectObject( hDC, penOld );
		::DeleteObject( penOverlappingLine );
	
	}

	while( !lstRects.IsEmpty() )
	{
		pCurveRect = (CRect*)lstRects.RemoveHead();
		delete pCurveRect;
	}


	return S_OK;
}


#define SECOND_LINE_PENSTYLE	PS_SOLID
#define SECOND_LINE_WIDTH		2
#define SECOND_LINE_COLOR		RGB(0,0,0)
#define GRID_LINE_PENSTYLE		PS_SOLID
#define GRID_LINE_WIDTH			1
#define GRID_LINE_COLOR			RGB(204,204,255)

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DrawClockVerticalLines

void CParamStrip::DrawClockVerticalLines( HDC hDC, long lXOffset ) 
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

	} while( lCurPosition < rectClip.right + lXOffset );

	::SetBkMode( hDC, nOldBkMode);
	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpenSecond );
	::DeleteObject( hpenGrid );
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DrawHorizontalLines

void CParamStrip::DrawHorizontalLines(HDC hDC, const RECT &rectClip)
{
	HPEN hpen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	if(hpen == NULL )
	{
		return;
	}

	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpen));

	// Initialize rectangle
	RECT rectLayer;
	rectLayer.left = rectClip.left;
	rectLayer.right = rectClip.right;
	rectLayer.bottom = m_StripUI.m_lVerticalScrollYPos;

	// Now, draw all the lines
	rectLayer.top = rectLayer.bottom;
	rectLayer.bottom = rectLayer.top + m_StripUI.m_lStripHeight;

	::MoveToEx( hDC, rectLayer.left, rectLayer.top, NULL );
	::LineTo( hDC, rectLayer.right, rectLayer.top );

	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpen );
}



/////////////////////////////////////////////////////////////////////////////
// CParamStrip::InvertGutterRange

void CParamStrip::InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset )
{
	// Set a flag if we should use the gutter range for our selection
	bool fUseGutterSelectRange = m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect);

	// Highlight the selected range (if there is one)
	if(fUseGutterSelectRange)
	{
		if( m_pTrackMgr->m_pTimeline )
		{
			RECT rectHighlight;

			rectHighlight.top = pRectClip->top;
			rectHighlight.bottom = pRectClip->bottom;

			m_pTrackMgr->m_pTimeline->ClocksToPosition( m_lGutterBeginSelect > m_lGutterEndSelect ?
													   m_lGutterEndSelect : m_lGutterBeginSelect,
													   &rectHighlight.left );

			m_pTrackMgr->m_pTimeline->ClocksToPosition( m_lGutterBeginSelect > m_lGutterEndSelect ?
													   m_lGutterBeginSelect : m_lGutterEndSelect,
													   &rectHighlight.right );

			rectHighlight.left -= lXOffset;
			rectHighlight.right -= lXOffset;

			GrayOutRect( hDC, &rectHighlight );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::Draw

HRESULT	STDMETHODCALLTYPE CParamStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that our timeline pointer is valid
	if( m_pTrackMgr == NULL
	||	m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

		// Get Function Bar rectangle
	VARIANT var;
	RECT rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
	{
		return E_FAIL;
	}

	// Get Curve Strip rectangle
	RECT rectStrip;
	if (!GetStripRect(&rectStrip))
	{
		return E_FAIL;
	}

	// Get Curve Strip left position
	long lLeftPosition;
	m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

	::OffsetRect( &rectStrip, -rectFBar.right, -rectStrip.top );
	::OffsetRect( &rectStrip, lLeftPosition, 0 );


	// Set a flag if we should use the gutter range for our selection
	bool fUseGutterSelectRange = m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect);

	// Get clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		InvertGutterRange( hDC, &rectClip, lXOffset);
		DrawMinimizedStrip( hDC, &rectStrip );
	}
	else
	{
		DrawMaximizedStrip( hDC, sv, &rectStrip, &rectClip, lXOffset );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::HighlightTimelineRange

void CParamStrip::HighlightTimelineRange(HDC hDC, long lXOffset)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

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
// CParamStrip::GetItemFromPoint

CTrackItem* CParamStrip::GetItemFromPoint( long lPos )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return NULL;
	}

	CTrackItem* pItemReturn = NULL;

	long lMeasure = 0;
	long lBeat = 0;
	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
	if( SUCCEEDED( pTimeline->PositionToMeasureBeat( dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
	{
		pItemReturn = GetTopItemFromMeasureBeat( lMeasure, lBeat );
	}

	return pItemReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetFBName

void CParamStrip::GetFBName( CString& strFBName )
{
	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return;
	}

	CString strTrackGroup, strPChannelName, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	strFBName.Empty();
	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

	// Determine track group text
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
	strPChannelName = m_pTrackObject->GetPChannelText();

	// Put it all together
	CString sObjectName = m_pTrackObject->GetObjectName();
	CString sStripName = sObjectName + ":" + m_strStripName;
	if(m_StripUI.m_nStripView == SV_MINIMIZED)
	{
		if(sObjectName.IsEmpty())
		{
			sStripName = m_strStripName;
		}
		else
		{
			sStripName = sObjectName;
		}
	}

	if( m_pTrackObject->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_SEGMENT )
	{
		if( strPChannelName.IsEmpty() )
		{
			strFBName.Format( "%s: %s", strTrackGroup,  sStripName );
		}
		else
		{
			strFBName.Format( "%s: %s: %s", strTrackGroup, strPChannelName, sStripName );
		}
	}
	else
	{
		strTmp.Empty();
		if( m_pTrackObject->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			strTmp.LoadString( IDS_BROADCAST_SEG );
		}
		else if( m_pTrackObject->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			strTmp.LoadString( IDS_BROADCAST_PERF );
		}
		if( strPChannelName.IsEmpty() )
		{
			strFBName.Format( "%s: %s: %s", strTrackGroup, strTmp, sStripName );
		}
		else
		{
			strFBName.Format( "%s: %s (%s): %s", strTrackGroup, strTmp, strPChannelName, sStripName );
		}
	}
	
	ASSERT( strFBName.IsEmpty() == FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CParamStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar )
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
			//V_BOOL(pvar) = FALSE;
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
			pvar->vt = VT_INT;
			V_INT(pvar) = STRIP_MAX_HEIGHT;
			break;

		case SP_DEFAULTHEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = CRV_DEFAULT_HEIGHT;
			break;

		case SP_CURSOR_HANDLE:
			pvar->vt = VT_I4;
			V_I4(pvar) = (int) m_hCursor;
			break;

		default:
			return CBaseStrip::GetStripProperty( sp, pvar );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CParamStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate our timeline pointer
	if( m_pTrackMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Initialize the return code to SUCCESS
	HRESULT hr = S_OK;

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		switch( nMsg )
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_MOUSEMOVE:
			case WM_KEYDOWN:
				// Do nothing if strip is minimized
				return S_OK;

			case WM_RBUTTONUP:
				// Only displai right-click context menu if strip is minimized
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

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fLeftMouseDown = true;
		hr = OnLButtonDown( wParam, lXPos, lYPos );
		m_lXPos = lXPos;	// need to start drag drop from where selection started
		break;

	/*case WM_LBUTTONDBLCLK:
		m_fLeftMouseDown = true;
		// Use default message handler for WM_LBUTTONDBLCLK
		hr = CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;*/

	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_MOUSEMOVE:
		hr = OnMouseMove( wParam, lParam, lXPos, lYPos );
		break;


	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
		m_fLeftMouseDown = false;
		hr = OnLButtonUp( wParam, lParam, lXPos, lYPos );
		//m_lXPos = lXPos;
		break;

	case WM_RBUTTONUP:
		// Use default message handler for WM_RBUTTONUP
		hr = CBaseStrip::OnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
		break;

	case WM_SETCURSOR:
		hr = OnSetCursor( lXPos, lYPos );
		break;

	case WM_SIZE:
		if( wParam == SIZE_MAXIMIZED )
		{
			m_pTrackObject->OnChangeParamStripView(SV_NORMAL);
		}
		else if( wParam == SIZE_MINIMIZED )
		{
			m_pTrackObject->OnChangeParamStripView(SV_MINIMIZED);
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
						m_pTrackMgr->NotifyDataChange(IDS_UNDO_STRIP_RESIZE);
					}
				}
			}
		}
		break;

	case WM_COMMAND:
	{
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode = HIWORD( wParam );	// notification code 
		WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{

			case IDM_SNAP_NONE:
			case IDM_SNAP_GRID:
			case IDM_SNAP_BEAT:
			case IDM_SNAP_BAR:
			case IDM_SNAP_SECOND:
			{
				SetSnapTo( wID );
				break;
			}

			case IDM_SNAP_SET_GRID:
			{
				DoGridsPerSecond();
				break;
			}

			case ID_NEW_PARAM:
			{
				m_pTrackMgr->AddNewObject();
				break;
			}

			case ID_DELETE_PARAM:
			{
				m_pTrackMgr->DeleteSelectedStripsFromObjects();
				break;
			}

			case ID_EDIT_INSERT_CURVE:
			{
				hr = Insert();
				break;
			}
			
			case ID_VIEW_PROPERTIES:
				hr = ShowPropertySheet();
				if( SUCCEEDED ( hr ) )
				{
					if( m_fShowItemProps )
					{
						// Change to the item property page
						OnShowItemProperties();

						// Update the property page, if it exists
						if( m_pPropPageMgr )
						{
							m_pPropPageMgr->RefreshData();
						}
					}
					else
					{
						// Change to the track group property page
						OnShowStripProperties();

						// Update the property page, if it exists
						IDMUSProdPropPageManager* pTrackPropPageMgr = m_pTrackMgr->GetPropPageMgr();
						if( pTrackPropPageMgr )
						{
							pTrackPropPageMgr->RefreshData();
						}
					}
				}
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
// CParamStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::FBOnWMMessage

HRESULT CParamStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		// Change the property page to the strip (group bits) property page
		OnShowStripProperties();
		StopInsertingCurve();
		break;

	case WM_LBUTTONUP:
		hr = OnLButtonUp(wParam, lParam, lXPos, lYPos);
		break;

	/*case WM_RBUTTONUP:
		hr = OnRButtonUp();
		break;*/

	case WM_MOUSEMOVE:
		hr = OnMouseMove(wParam, lParam, lXPos, lYPos );
		break;

	case WM_RBUTTONUP:
		// Change the property page to the strip (group bits) property page
		OnShowStripProperties();

		// Set the cursor position for the insert operation to -1
		m_lXPos = -1;

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

	default:
		return CBaseStrip::FBOnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::Copy

HRESULT CParamStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if there is anything to copy
	HRESULT hr;
	hr = CanCopy();
	if( hr != S_OK )
	{
		// Nothing to copy - return error code
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}
	
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}


	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get the span of the selected items
	long lStartTime = 0;
	long lEndTime = 0;
	m_pSelectedRegions->GetSpan(lStartTime, lEndTime);

	// Compute the start offset
	REFERENCE_TIME rtStartOffset = 0;
	long lMeasure = 0;
	long lBeat = 0;
	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
	
	pTimeline->ClocksToMeasureBeat(dwGroupBits, 0, lStartTime, &lMeasure, &lBeat);
	m_pTrackMgr->MeasureBeatGridTickToUnknownTime( lMeasure, lBeat, 0, 0, &rtStartOffset);
	CTrackItem* pFirstSelectedItem = GetFirstSelectedItem();

	if(pFirstSelectedItem)
	{
		rtStartOffset = pFirstSelectedItem->m_rtTimePhysical - rtStartOffset;
	}

	// Create an IStream to save the selected items in.
	IStream* pStreamCopy = NULL;

	// Verify that we were able to create a stream
	if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStreamCopy)))
	{
		return E_OUTOFMEMORY;
	}


	// Save the selected items into the stream
	if(FAILED(SaveSelectedItems(pStreamCopy, rtStartOffset, NULL)))
	{
		// Save failed
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	// The save succeeded - copy the stream to the clipboard
	hr = CopyDataToClipboard(pITimelineDataObject, pStreamCopy, m_cfParamTrack, m_pTrackMgr, this);

	// Release our pointer to the stream
	pStreamCopy->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CalculateDropTime

REFERENCE_TIME CParamStrip::CalculateDropTime(MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}
	

	REFERENCE_TIME rtDropTime = 0;
	if(m_pTrackMgr->IsRefTimeTrack())
	{
		REFERENCE_TIME rtTime = 0;
		pTimeline->ClocksToRefTime( mtTime, &rtTime );
		rtTime += pCopyPasteUI->m_rtOffset;

		long lMinute, lSec, lGrid, lMs;
		m_pTrackMgr->RefTimeToMinSecGridMs( this, rtTime, &lMinute, &lSec, &lGrid, &lMs );

		switch(m_StripUI.m_nSnapToRefTime)
		{
			case IDM_SNAP_NONE:
			{
				// Nothing to do
				break;
			}

			case IDM_SNAP_GRID:
			{
				m_pTrackMgr->MinSecGridMsToRefTime(this, lMinute, lSec, lGrid, pCopyPasteUI->m_lRefTimeMs, &rtTime);
				break;
			}
			
			case IDM_SNAP_SECOND:
			{
				m_pTrackMgr->MinSecGridMsToRefTime(this, lMinute, lSec, pCopyPasteUI->m_lRefTimeGrid, pCopyPasteUI->m_lRefTimeMs, &rtTime);
				break;
			}
		}

		m_pTrackMgr->RefTimeToUnknownTime(rtTime, &rtDropTime);
	}
	else
	{
		mtTime += pCopyPasteUI->m_mtOffset;

		long lMeasure = 0;
		long lBeat = 0;
		long lGrid = 0;
		long lTick = 0;

		m_pTrackMgr->ClocksToMeasureBeatGridTick(mtTime, &lMeasure, &lBeat, &lGrid, &lTick);

		switch( m_StripUI.m_nSnapToMusicTime )
		{
			case IDM_SNAP_NONE:
			{
				// Nothing to do
				break;
			}

			case IDM_SNAP_GRID:
			{
				m_pTrackMgr->MeasureBeatGridTickToClocks(lMeasure, lBeat, lGrid, pCopyPasteUI->m_lMusicTimeTick, &mtTime);
				break;
			}
			
			case IDM_SNAP_BEAT:
			{
				m_pTrackMgr->MeasureBeatGridTickToClocks(lMeasure, lBeat, pCopyPasteUI->m_lMusicTimeGrid, pCopyPasteUI->m_lMusicTimeTick, &mtTime);
				break;
			}
			
			case IDM_SNAP_BAR:
			{
				m_pTrackMgr->MeasureBeatGridTickToClocks(lMeasure, pCopyPasteUI->m_lMusicTimeBeat, 
															pCopyPasteUI->m_lMusicTimeGrid, pCopyPasteUI->m_lMusicTimeTick,&mtTime);
				break;
			}
		}

		m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtDropTime );
	}

	return rtDropTime;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CalculatePasteTime

REFERENCE_TIME CParamStrip::CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI )
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
// CParamStrip::PasteAt

HRESULT CParamStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lPasteTime, bool bDropNotPaste, BOOL &fChanged )
{
	// Validate the data object pointer
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}
		
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}


	// Calc rtSnapPasteTime 
	REFERENCE_TIME rtSnapPasteTime = 0;
	if(FAILED(m_pTrackMgr->ClocksToUnknownTime(lPasteTime, &rtSnapPasteTime)))
	{
		return E_FAIL;
	}

	if(FAILED(SnapUnknownTime( rtSnapPasteTime, &rtSnapPasteTime)))
	{
		return E_FAIL;
	}

	// Handle CF_PARAMTRACK
	if(pITimelineDataObject->IsClipFormatAvailable(m_cfParamTrack) == S_OK)
	{
		// Try and read the stream that contains the items
		IStream* pIStream = NULL;
		if(SUCCEEDED(pITimelineDataObject->AttemptRead(m_cfParamTrack, &pIStream)))
		{
			// Read DMUS_FOURCC_COPYPASTE_UI_CHUNK
			ioCopyPasteUI iCopyPasteUI;
			ZeroMemory( &iCopyPasteUI, sizeof(ioCopyPasteUI) );

			IDMUSProdRIFFStream* pIRiffStream = NULL;
			if(FAILED(AllocRIFFStream( pIStream, &pIRiffStream)) || pIRiffStream == NULL)
			{
				return E_OUTOFMEMORY;
			}

			// Remember the stream position
			DWORD dwPos = StreamTell( pIStream );
			
			MMCKINFO ck;
			ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
			if(pIRiffStream->Descend(&ck, NULL, MMIO_FINDCHUNK) == 0)
			{
				DWORD dwByteCount = 0;
				DWORD dwSize = min(ck.cksize, sizeof(ioCopyPasteUI));

				if(FAILED(pIStream->Read( &iCopyPasteUI, dwSize, &dwByteCount)) || dwByteCount != dwSize)
				{
					pIStream->Release();
					return E_FAIL;
				}
			}

			// Release the RIFF stream and reset the stream pointer
			pIRiffStream->Release();
			StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

			// If we're in a drag-drop operation, check if we're dragging from and dropping to this strip
			if(bDropNotPaste && (m_pITargetDataObject == m_pISourceDataObject))
			{
				// Make sure we are /dropping to a different location
				REFERENCE_TIME rtSnapTime;
				if(SUCCEEDED(SnapXPosToUnknownTime(m_lStartDragPosition, &rtSnapTime)))
				{
					if( rtSnapPasteTime == rtSnapTime )
					{
						// Didn't move - exit early
						pIStream->Release();
						return E_FAIL;
					}
				}
			}

			// Load the stream into a list of items
			CTypedPtrList<CPtrList, CTrackItem*> list;
			if(FAILED(LoadList(list, pIStream)))
			{
				pIStream->Release();
				return E_FAIL;
			}

			// Do we need to convert times?
			if(iCopyPasteUI.m_fRefTimeTrack != m_pTrackMgr->IsRefTimeTrack())
			{
				POSITION position = list.GetHeadPosition();
				while(position)
				{
					CTrackItem* pItem = list.GetNext(position);
					ASSERT(pItem);
					if(pItem)
					{
						pItem->SwitchTimeBase();
                        pItem->SetPhysicalTime(pItem->m_rtTimePhysical, pItem->m_rtDuration);
					}
				}
			}

			// Remember the group bits
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

			// Calc rtPasteTime 
			REFERENCE_TIME rtPasteTime;
			if( bDropNotPaste )
			{
				rtPasteTime = CalculateDropTime(lPasteTime, &iCopyPasteUI);
			}
			else
			{
				rtPasteTime = CalculatePasteTime(lPasteTime, &iCopyPasteUI);
			}


			// The length of the segment
			MUSIC_TIME mtSegmentLength = LONG_MAX;
			REFERENCE_TIME rtSegmentLength = 0;
			pTimeline->ClocksToRefTime(mtSegmentLength, &rtSegmentLength); 

			// Check if there are any items to paste
			if(!list.IsEmpty())
			{
				long lMeasure = 0;
				long lBeat = 0;
				long lGrid = 0;
				long lTick= 0;

				// Unselect all items so the only selected items are the dropped or pasted ones
				UnselectAll();

				// Make sure the last item lands in the last beat or sooner
				VARIANT varLength;
				if(SUCCEEDED(pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength)))
				{
					mtSegmentLength = V_I4(&varLength);
					m_pTrackMgr->ClocksToUnknownTime(mtSegmentLength, &rtSegmentLength);

					// Make sure the last item lands in the last beat or sooner
					REFERENCE_TIME rtTimeLastItem = list.GetTail()->m_rtTimePhysical + rtPasteTime;
					if(rtTimeLastItem >= rtSegmentLength)
					{
						m_pTrackMgr->UnknownTimeToMeasureBeatGridTick(rtPasteTime - (rtTimeLastItem - rtSegmentLength),
																	  &lMeasure, &lBeat, &lGrid, &lTick);

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
			if(!bDropNotPaste)
			{
				// We're doing a paste, so get the paste type
				TIMELINE_PASTE_TYPE tlPasteType;
				if( FAILED( pTimeline->GetPasteType( &tlPasteType ) ) )
				{
					// Failed to get the paste type, so return with error code
					pIStream->Release();
					return E_FAIL;
				}

				// Check if we're doing a paste->overwrite
				if( tlPasteType == TL_PASTE_OVERWRITE )
				{
					REFERENCE_TIME rtTimeFirstItem = list.GetHead()->m_rtTimePhysical + rtPasteTime;

					long lMeasureStart = 0;
					long lMeasureEnd = 0;
					long lBeatStart = 0;
					long lBeatEnd = 0;
					long lGrid = 0;
					long lTick = 0;
					
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
				ASSERT(pItem);

				// Check if the item will land before the end of the segment
				pItem->m_rtTimePhysical += rtPasteTime;
				if(pItem->m_rtTimePhysical < rtSegmentLength)
				{
					// Recalc m_mtTime fields
					/*MUSIC_TIME mtDuration = pItem->m_mtEndTime - pItem->m_mtStartTime;*/
					
					REFERENCE_TIME rtDuration = pItem->m_rtDuration;
					/*m_pTrackMgr->ClocksToUnknownTime(mtDuration, &rtDuration);*/

					pItem->SetPhysicalTime(pItem->m_rtTimePhysical, rtDuration);
	
					// When pasted or dropped, each item is selected
					pItem->m_fSelected = TRUE;

					InsertByAscendingTime( pItem, TRUE );

					// We changed
					fChanged = TRUE;

				}
				else
				{
					// Item would be pasted beyond the end of the segment - delete it
					delete pItem;
				}
			}
			pIStream->Release();
		}
	}

	if(m_nStripIsDragDropSource)
	{
		// Drag/drop Target and Source are the same strip
		m_nStripIsDragDropSource = 2;
	}

	if( CParamStrip::m_pIDocRootOfDragDropSource &&  CParamStrip::m_pIDocRootOfDragDropSource == m_pTrackMgr->GetDocRootNode())
	{
		// Drag/drop Target and Source are the same file (i.e. Segment)
		CParamStrip::m_fDragDropIntoSameDocRoot = TRUE;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::Paste

HRESULT CParamStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}	

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Get a TimelineDataObject, if we don't already have one
	hr = GetTimelineDataObject( pITimelineDataObject, pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if(FAILED(hr) || (pITimelineDataObject == NULL))
	{
		return hr;
	}

	// Convert the paste position to clocks
	long lClocks = 0;
	if(FAILED(pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lClocks)))
	{
		return E_UNEXPECTED;
	}

	// Do the paste
	BOOL fChanged = FALSE;
	hr = PasteAt(pITimelineDataObject, lClocks, false, fChanged);

	// Check if the paste succeeded AND if something was actually pasted
	if(SUCCEEDED(hr) && fChanged)
	{
		// Update the selection regions to only include the selected items
		SelectRegionsFromSelectedItems();

		// Notify the containing segment that we did a paste operation
		m_pTrackMgr->SetLastEdit(IDS_PASTE);
		m_pTrackMgr->TrackDataChanged();

		// Redraw our strip
		pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

		// Ensure the track item property page is visible
		OnShowItemProperties();

		// Refresh the track item property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}

		// Sync with DirectMusic
		m_pTrackMgr->SyncWithDirectMusic();
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return the success/failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::InsertCurve

void CParamStrip::InsertCurve( long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}


	// Get CurveStrip rect
	CRect rectStrip;
	if (!GetStripRect(rectStrip))
	{
		return;
	}

	// Convert points into Window coordinates
	CPoint ptWnd( lXPos, lYPos );
	pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWnd );

	// Make sure YPos is in this CurveStrip
	if( ptWnd.y < rectStrip.top || ptWnd.y > rectStrip.bottom )
	{
		// YPos is NOT in this CurveStrip
		return;
	}

	// Make sure XPos is in this CurveStrip
	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lClockLength = V_I4(&var);
	long lXPosClock = 0;

	pTimeline->PositionToClocks( lXPos, &lXPosClock );
	if( lXPosClock < 0
	||  lXPosClock > lClockLength
	||  ptWnd.x < rectStrip.left 
	||  ptWnd.x > rectStrip.right )
	{
		// XPos is NOT in this CurveStrip
		return ;
	}

	IOleWindow* pIOleWindow;

	pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
	if( pIOleWindow )
	{
		CWnd wnd;
		HWND hWnd;

		if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
		{
			CPoint point( lXPos, lYPos );

			if( SUCCEEDED ( pTimeline->StripToWindowPos( this, &point ) ) )
			{
				// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
				// We have to call ReleaseCapture(), or the CurveTracker won't work.
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
				::ReleaseCapture();

				m_fInsertingCurve = INSERT_CURVE_END_VALUE;

				wnd.Attach( hWnd );
				CCurveTracker curveTracker( &wnd, pTimeline, this, CTRK_INSERTING );
				if( curveTracker.TrackRubberBand( &wnd, point, TRUE ) )
				{
					// Create Direct Music Curve
					CTrackItem* pCurve = new CTrackItem(m_pTrackMgr, this);
					if( pCurve )
					{
						// Fill in Curve values
						pCurve->SetStartEndTime(m_lInsertingStartClock, m_lInsertingEndClock);
						pCurve->SetStartEndValue(m_fltInsertingStartValue, m_fltInsertingEndValue);

						SelectAllCurves( FALSE );
						pCurve->m_fSelected = TRUE;

						m_lstTrackItems.AddTail(pCurve);

						// Redraw CurveStrips
						RefreshCurveStrips();
						RefreshCurvePropertyPage();

						// Let the object know about the changes
						m_pTrackMgr->NotifyDataChange( IDS_UNDO_INSERT_CURVE ); 
					}
				}

				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

				wnd.Detach();
			}
		}

		pIOleWindow->Release();
	}
}



/////////////////////////////////////////////////////////////////////////////
// CParamStrip::Delete

HRESULT CParamStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}
	
	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Delete all selected items
	DeleteSelectedItems();

	// Clear all selection ranges
	m_pSelectedRegions->Clear();

	// Redraw our strip
	pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

	// Refresh the track item property page, if it exists
	if(m_pPropPageMgr)
	{
		m_pPropPageMgr->RefreshData();
	}

	// Notify the containing segment that a delete operation occurred
	m_pTrackMgr->SetLastEdit(IDS_DELETE);
	m_pTrackMgr->TrackDataChanged();

	// Sync with DirectMusic
	m_pTrackMgr->SyncWithDirectMusic();

	// Return that the delete succeeded
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CanPaste

HRESULT CParamStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid timeline pointer
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Make sure the clipboard formats have been registered
	if( RegisterClipboardFormats() == FALSE )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a TimelineDataObject, if we don't already have one
	HRESULT hr = GetTimelineDataObject( pITimelineDataObject, pTimeline );

	// Verify that GetTimelineDataObject succeeded, and actually returned a TimelineDataObject pointer
	if(FAILED(hr) || pITimelineDataObject == NULL)
	{
		return hr;
	}

	// Check the timeline data object for our clipboard formats
	hr = S_FALSE;
	if(pITimelineDataObject->IsClipFormatAvailable(m_cfParamTrack) == S_OK)
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


HRESULT CParamStrip::CanDeleteParameter( void )
{
	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return S_FALSE;
	}

	// If it's the empty dummy track object then we can't delete it...
	if(m_pTrackObject->IsEmptyTrackObject())
	{
		return S_FALSE;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CanInsert

HRESULT CParamStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we have a valid Timeline pointer
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// Do nothing if strip is minimized
	if( m_StripUI.m_nStripView == SV_MINIMIZED)
	{
		return S_FALSE;
	}


	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the position to paste at
	long lInsertTime = 0;
	if(FAILED(pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertTime)))
	{
		return E_UNEXPECTED;
	}

	// Check if the insert position is invalid, or if the user clicked in the function bar
	if(lInsertTime < 0)
	{
		// It's invalid - we can't insert
		return S_FALSE;
	}

	// Convert from a time to a pixel position
	long lPosition = 0;
	if(FAILED(pTimeline->ClocksToPosition( lInsertTime, &lPosition)))
	{
		return E_UNEXPECTED;
	}

	// Get the length of the segment, in clocks
	VARIANT var;
	if( FAILED(pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var)))
	{
		return E_UNEXPECTED;
	}

	// If the position to insert is beyond the end of the segment, return S_FALSE since we can't insert there
	if(lInsertTime >= V_I4(&var))
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
// CParamStrip::DragOver

HRESULT CParamStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pt);

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( m_pITargetDataObject == NULL )
	{
		ASSERT( FALSE ); // Shouldn't happen - CanPasteFromData will return E_POINTER.
	}

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
// CParamStrip::Drop

HRESULT CParamStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

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

						if( CParamStrip::m_fDragDropIntoSameDocRoot == FALSE
						||  m_dwOverDragEffect != DROPEFFECT_MOVE )
						{
							// Either we are dropping into a different file,
							// or we are doing a "copy" and there will be no change to the source strip, so....
							// Notify the containing segment that a paste operation occurred
							m_pTrackMgr->SetLastEdit(IDS_PASTE);
							m_pTrackMgr->TrackDataChanged();
						}

						// Redraw our strip
						m_pTrackMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
						
						// If the property sheet is visible, make it display the track item's property page
						OnShowItemProperties();

						// Update the track item's property page, if it exists
						if( m_pPropPageMgr )
						{
							m_pPropPageMgr->RefreshData();
						}

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
// CParamStrip::GetData

HRESULT CParamStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	CPropCurve* pPropCurve = (CPropCurve *)*ppData;

	int nNbrSelectedCurves = 0;

	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		CTrackItem* pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);
		ASSERT(pCurve);
		if( pCurve->m_fSelected )
		{

			if( nNbrSelectedCurves == 0 )
			{
				pPropCurve->GetValuesFromCurve( this, pCurve );
			}
			else
			{
				CPropCurve propcurve;
				propcurve.GetValuesFromCurve( this, pCurve );
				*pPropCurve += propcurve;
			}

			nNbrSelectedCurves++;
		}
	}

	if( nNbrSelectedCurves == 0 )
	{
		return S_FALSE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SetData

HRESULT CParamStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_POINTER;
	}

	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	CPropCurve* pPropCurve = (CPropCurve *)pData;

	CTrackItem* pCurve = NULL;
	DWORD dwChanged = 0;


	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		CTrackItem* pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);
		ASSERT(pCurve);
		if( pCurve->m_fSelected )
		{
			dwChanged |= pPropCurve->ApplyValuesToCurve( this, pCurve );
		}
	}

	// Always refresh property page. (If user tries to change duration to value less than 1,
	// we need to reset the property page to valid values)
	RefreshCurvePropertyPage();

	if( dwChanged )
	{
		// Redraw CurveStrips
		RefreshCurveStrips();

		// Let the object know about the changes
		m_pTrackMgr->NotifyDataChange( IDS_UNDO_CHANGE_CURVE ); 
	}

	return S_OK;

}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnShowProperties

HRESULT STDMETHODCALLTYPE CParamStrip::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdFramework* pDMProdFramework = m_pTrackMgr->GetFramework();
	ASSERT(pDMProdFramework);
	if(pDMProdFramework == NULL )
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
	pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
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
		CCurvePropPageMgr* pPPM = new CCurvePropPageMgr( pDMProdFramework, this );

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
	short nActiveTab = CCurvePropPageMgr::sm_nActiveTab;

	// Set the displayed property page to our property page
	pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Set the active tab
	if( SUCCEEDED ( pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
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
// CParamStrip::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CParamStrip::OnRemoveFromPageManager( void )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CanPasteFromData

HRESULT CParamStrip::CanPasteFromData(IDataObject* pIDataObject)
{
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
	if( pITimelineDataObject->IsClipFormatAvailable( m_cfParamTrack ) == S_OK )
	{
		// If we found our clipboard format, return S_OK
		hr = S_OK;
	}

	// Release our reference on the Timeline DataObject
	pITimelineDataObject->Release();

	// Return either S_OK or S_FALSE
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnLButtonDown

HRESULT CParamStrip::OnLButtonDown( WPARAM wParam, long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		return FALSE;
	}

	UnselectGutterRange();

	// Handle inserting a new Curve
	if( m_fInsertingCurve )
	{
		InsertCurve( lXPos, lYPos );
		StopInsertingCurve();
		return S_OK;
	}

	// Get a pointer to the Timeline
	if( pTimeline == NULL )
	{
		return E_FAIL;
	}
	
	CTrackItem* pTrackItem = GetCurveFromPoint( lXPos, lYPos, TRUE );
	WORD wRefreshUI = FALSE;
	
	if (pTrackItem) 
	{		
		// Have we clicked on a curve?
		if (pTrackItem->m_fSelected) 
		{	
			// Convert the frame rect of the curve to window coordinates
			CRect selRect = pTrackItem->m_rectFrame;
			VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
			VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
			
			// create a tracker and find out if we are on a handle or somewhere within the curve
			CCurveTracker curveTracker(&selRect);
			UINT hit = HitTest( curveTracker, lXPos, lYPos );

			switch(hit) 
			{
				case CRectTracker::hitMiddle:
				{
					if (wParam & MK_CONTROL) 
					{
						wRefreshUI |= SelectCurve(pTrackItem, FALSE);
						DragModify(pTrackItem, lXPos, lYPos, CTRK_DRAGGING, true);
					}
					else 
					{
						DragModify(pTrackItem, lXPos, lYPos, CTRK_DRAGGING);
					}
					m_fShowItemProps = true;
					break;
				}

				default:
				{
					wRefreshUI |= SelectAllCurves(FALSE);
					wRefreshUI |= SelectCurve(pTrackItem, TRUE);
					RefreshUI(wRefreshUI);
					DragModify(pTrackItem, lXPos, lYPos, CTRK_DRAGGING_EDGE);
				}
			}
		}
		else {
			
			if (!(wParam & MK_CONTROL)) 
			{
				wRefreshUI |= SelectAllCurves(FALSE);
			}

			wRefreshUI |= SelectCurve(pTrackItem, TRUE);
			RefreshUI(wRefreshUI);
			DragModify(pTrackItem, lXPos, lYPos, CTRK_DRAGGING);
		}

		RefreshUI(wRefreshUI);
		RefreshCurvePropertyPage();
	}
	else 
	{
		
		// user might have clicked on the curve outside of the frame
		// in which case we select the curve only
		CTrackItem* pTrackItem = GetCurveFromPoint( lXPos, lYPos, FALSE );
		if (pTrackItem) 
		{
			if (!(wParam & MK_CONTROL)) 
			{
				wRefreshUI |= SelectAllCurves(FALSE);
			}

			wRefreshUI |= SelectCurve(pTrackItem, !((wParam & MK_CONTROL) && pTrackItem->m_fSelected));
			RefreshUI(wRefreshUI);
		}
		else if( wParam & MK_SHIFT )
		{
			// Shift key is down
			// AMC TODO: Implement shift select
		}
		else
		{
			// Handle curve bounding box selection
			wRefreshUI = SelectAllCurves( FALSE );

			if( pTrackItem )
			{
				wRefreshUI |= SelectCurve( pTrackItem, TRUE );
			}
			else
			{
				// Refresh UI
				RefreshUI(wRefreshUI);

				// Switch to curve strip property page
				RefreshCurvePropertyPage();

				// Create bounding box
				IOleWindow* pIOleWindow = NULL;
				pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
				if( pIOleWindow )
				{
					CWnd wnd;
					HWND hWnd;

					if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
					{
						CPoint point( lXPos, lYPos );

						if( SUCCEEDED ( pTimeline->StripToWindowPos( this, &point ) ) )
						{
							wnd.Attach( hWnd );
							CCurveTracker curveTracker( &wnd, pTimeline, this, CTRK_SELECTING );
							curveTracker.TrackRubberBand( &wnd, point, TRUE );
							wnd.Detach();
						}
					}

					pIOleWindow->Release();
				}
			}
		}

		RefreshUI(wRefreshUI);
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnLButtonUp

HRESULT CParamStrip::OnLButtonUp( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Finish inserting new Curve
	if( m_fInsertingCurve )
	{
		StopInsertingCurve();
		return S_OK;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnRButtonDown

HRESULT CParamStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Unselect all items in the other strips
	UnselectGutterRange();

	// Get the item at the mouse click.
	CTrackItem* pItem = GetCurveFromPoint( lXPos, lYPos, TRUE );

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
				m_pSelectedRegions->AddRegion(lXPos);
			}
			// If the item was already selected, do nothing
		}
		else
		{
			// if region not selected, select it, otherwise do nothing

			// Convert the pixel position to a measure and beat
			long lMeasure = 0;
			long lBeat = 0;
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
			if( SUCCEEDED( pTimeline->PositionToMeasureBeat(dwGroupBits, 0,lXPos, &lMeasure, &lBeat) ) )
			{
				// Check if this beat is not already selected
				if( !m_pSelectedRegions->Contains(lMeasure, lBeat))
				{
					// This beat is not selected - unselect all beats
					UnselectAll();

					// Now, select just this beat
					m_pSelectedRegions->AddRegion(lXPos);
				}
			}
		}
	}

	// Redraw our strip
	pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	// Switch the property page to the track item's property page
	OnShowItemProperties();

	// Refresh the  property page, if it exists
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnMouseMove

HRESULT CParamStrip::OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Handle inserting a new Curve
	if( m_fInsertingCurve == INSERT_CURVE_START_VALUE )
	{
		CPoint ptLeft( lXPos, lYPos );
		CPoint ptRight( -1, -1 );

		OnUpdateInsertCurveValue( ptLeft, ptRight );
		return S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CanCycle

BOOL CParamStrip::CanCycle( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Can't cycle if strip is minimized
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		return FALSE;
	}

	long lMeasure = 0;
	long lBeat = 0;

	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
	if( SUCCEEDED ( pTimeline->PositionToMeasureBeat( dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat ) ) )
	{
		int nCount = 0;
		CTrackItem* pItem;

		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			pItem = m_lstTrackItems.GetNext( pos );

			if( pItem->m_lMeasure == lMeasure
			&&  pItem->m_lBeat == lBeat )
			{
				if( ++nCount > 1 )
				{
					return TRUE;
				}
			}

			if( pItem->m_lMeasure > lMeasure )
			{
				break;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::PostRightClickMenu

HRESULT CParamStrip::PostRightClickMenu( POINT pt )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	m_fInRightClickMenu = TRUE;

	HMENU hMenu = ::LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU));
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

	::EnableMenuItem(hMenuPopup, ID_EDIT_CUT, (CanCut() == S_OK) ? MF_ENABLED : MF_GRAYED);
	::EnableMenuItem(hMenuPopup, ID_EDIT_COPY, (CanCopy() == S_OK) ? MF_ENABLED : MF_GRAYED);

	::EnableMenuItem(hMenuPopup, 2, (CanPaste(NULL) == S_OK) ? MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION);

	::EnableMenuItem(hMenuPopup, ID_EDIT_INSERT_CURVE, (CanInsert() == S_OK) ? MF_ENABLED : MF_GRAYED);
	::EnableMenuItem(hMenuPopup, ID_DELETE_PARAM, (CanDeleteParameter() == S_OK) ? MF_ENABLED : MF_GRAYED);
	::EnableMenuItem(hMenuPopup, ID_EDIT_DELETE, (CanDelete() == S_OK) ? MF_ENABLED : MF_GRAYED);
	::EnableMenuItem(hMenuPopup, ID_EDIT_SELECT_ALL, (IsEmpty()) ? MF_GRAYED : MF_ENABLED);

	::EnableMenuItem(hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED);
	::EnableMenuItem(hMenuPopup, IDM_CYCLE_ITEMS, (CanCycle() == TRUE) ? MF_ENABLED : MF_GRAYED);

	
	// Initialize 'Snap To' popup menu
	HMENU hMenuPopupSnapTo = ::GetSubMenu(hMenuPopup, 11);	// 11 is index of 'Snap To' popup menu
	ASSERT(hMenuPopupSnapTo != NULL);
	if(hMenuPopupSnapTo)
	{
		CString strMenuText;
		if(m_pTrackMgr->IsRefTimeTrack())
		{
			strMenuText.LoadString(IDS_SECOND_TEXT);
			::InsertMenu( hMenuPopupSnapTo, 2, MF_BYPOSITION | MF_STRING, IDM_SNAP_SECOND, strMenuText ); 
			::InsertMenu( hMenuPopupSnapTo, 3, MF_BYPOSITION | MF_SEPARATOR, 0, NULL ); 

			strMenuText.LoadString(IDS_SET_GRID_TEXT);
			::InsertMenu(hMenuPopupSnapTo, 4, MF_BYPOSITION | MF_STRING, IDM_SNAP_SET_GRID, strMenuText); 

			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_NONE, (m_StripUI.m_nSnapToRefTime == IDM_SNAP_NONE) ? MF_CHECKED : MF_UNCHECKED);
			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_GRID, (m_StripUI.m_nSnapToRefTime == IDM_SNAP_GRID) ? MF_CHECKED : MF_UNCHECKED);
			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_SECOND, (m_StripUI.m_nSnapToRefTime == IDM_SNAP_SECOND) ? MF_CHECKED : MF_UNCHECKED);
		}
		else
		{
			strMenuText.LoadString(IDS_BEAT_MENU_TEXT);
			::InsertMenu(hMenuPopupSnapTo, 2, MF_BYPOSITION | MF_STRING, IDM_SNAP_BEAT, strMenuText); 

			strMenuText.LoadString(IDS_BAR_MENU_TEXT);
			::InsertMenu(hMenuPopupSnapTo, 3, MF_BYPOSITION | MF_STRING, IDM_SNAP_BAR, strMenuText); 

			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_NONE, (m_StripUI.m_nSnapToMusicTime == IDM_SNAP_NONE) ? MF_CHECKED : MF_UNCHECKED);
			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_GRID, (m_StripUI.m_nSnapToMusicTime == IDM_SNAP_GRID) ? MF_CHECKED : MF_UNCHECKED);
			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_BEAT, (m_StripUI.m_nSnapToMusicTime == IDM_SNAP_BEAT) ? MF_CHECKED : MF_UNCHECKED);
			::CheckMenuItem(hMenuPopupSnapTo, IDM_SNAP_BAR, (m_StripUI.m_nSnapToMusicTime == IDM_SNAP_BAR) ? MF_CHECKED : MF_UNCHECKED);
		}
	}

	pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
	::DestroyMenu( hMenu );

	m_fInRightClickMenu = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::UnselectGutterRange

void CParamStrip::UnselectGutterRange( void )
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
// CParamStrip::CreateDataObject

HRESULT	CParamStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that we're passed a valid location to store the IDataObject pointer in
	ASSERT(ppIDataObject);
	if(ppIDataObject == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
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
	IStream* pIStream = NULL;
	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return E_FAIL;
	}

	if(SUCCEEDED(pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream)))
	{
		// Get the item at the drag point
		CTrackItem* pItemAtDragPoint = GetItemFromPoint( lPosition );

		// Compute offset
		REFERENCE_TIME rtOffset = 0;
		MUSIC_TIME mtTimeXPos = 0;
		pTimeline->PositionToClocks( lPosition, &mtTimeXPos );
		m_pTrackMgr->ClocksToUnknownTime( mtTimeXPos, &rtOffset );
		rtOffset = pItemAtDragPoint->m_rtTimePhysical - rtOffset;

		// mark the items as being dragged: this used later for deleting items in drag move
		MarkSelectedItems( UD_DRAGSELECT );

		// Save the selected items into a stream
		if(SUCCEEDED(SaveSelectedItems( pIStream, rtOffset, pItemAtDragPoint)))
		{
			// Add the stream to the Timeline DataObject
			if(SUCCEEDED(pITimelineDataObject->AddExternalClipFormat( m_cfParamTrack, pIStream)))
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
// CParamStrip::DoDragDrop

HRESULT CParamStrip::DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos)
{
	// Do a drag'n'drop operation

	// Get the item at the point we're dragging from
	CTrackItem* pItem = GetItemFromPoint( lXPos );

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
		hr = CreateDataObject( &m_pISourceDataObject, lXPos );

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
			CParamStrip::m_pIDocRootOfDragDropSource = m_pTrackMgr->GetDocRootNode();
			CParamStrip::m_fDragDropIntoSameDocRoot = FALSE;

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
						m_pTrackMgr->SetLastEdit(IDS_UNDO_MOVE);
					}
					else
					{
						// We did a copy - use the 'Insert' undo text
						m_pTrackMgr->SetLastEdit(IDS_INSERT);
					}

					// Notify the containing segment that we changed
					m_pTrackMgr->TrackDataChanged();

					// Update the selection regions to include the selected items
					SelectRegionsFromSelectedItems();

					// If visible, switch the property sheet to the track item property page
					OnShowItemProperties();

					// If it exists, refresh the track item property page
					if( m_pPropPageMgr )
					{
						m_pPropPageMgr->RefreshData();
					}

					// Sync with DirectMusic
					m_pTrackMgr->SyncWithDirectMusic();
				}
				else
				{
					// Target strip is different from source strip
					if( dwEffect & DROPEFFECT_MOVE )
					{
						// Check if we dropped in the same segment
						if( CParamStrip::m_fDragDropIntoSameDocRoot == TRUE )
						{
							// We did a move operation to another strip in the same segment - use the 'Move' undo text
							m_pTrackMgr->SetLastEdit(IDS_UNDO_MOVE);
						}
						else
						{
							// We did a move operation to another strip in a different - use the 'Delete' undo text
							m_pTrackMgr->SetLastEdit(IDS_DELETE);
						}

						// Let our hosting editor know about the changes
						m_pTrackMgr->TrackDataChanged();

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
	CParamStrip::m_pIDocRootOfDragDropSource = NULL;
	CParamStrip::m_fDragDropIntoSameDocRoot = FALSE;
	
	// Return a success or failure code
	return hr;
}



//////////////////////////////////////////////////////////////////////////////
// CParamStrip::SelectCurvesInRect

WORD CParamStrip::SelectCurvesInRect( CRect* pRect )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return 0;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return 0;
	}

	WORD wRefreshUI = 0;

	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		CTrackItem* pItem = (CTrackItem*)m_lstTrackItems.GetNext(position);
		ASSERT(pItem);
		if(pItem)
		{
			BOOL bSelect = FALSE;
			CRect rectFrame = pItem->m_rectFrame;
			if(rectFrame.IntersectRect(&rectFrame, pRect))
			{
				bSelect = TRUE;
			}
			else
			{
				bSelect = FALSE;
			}

			wRefreshUI |= SelectCurve(pItem, bSelect);
		}
	}

	return wRefreshUI;
}


////////////////////////////////////////////////////////////////////////////////
// CParamStrip::SelectItemsInSelectedRegions

bool CParamStrip::SelectItemsInSelectedRegions()
{
	// Flag set to true if anything changes
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if this item is within a selection region
		if(CListSelectedRegion_Contains(pItem->m_rtTimePhysical))
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
// CParamStrip::SelectRegionsFromSelectedItems

void CParamStrip::SelectRegionsFromSelectedItems( void )
{
	// Clear the list of selected regions
	m_pSelectedRegions->Clear();

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// Add the beat the item is in to the list of selected regions.
			CListSelectedRegion_AddRegion(*m_pSelectedRegions, *pItem);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// CParamStrip::RegisterClipboardFormats

BOOL CParamStrip::RegisterClipboardFormats( void )
{
	// CF_PARAMTRACK
	if( m_cfParamTrack == 0 )
	{
		m_cfParamTrack = RegisterClipboardFormat( CF_PARAMTRACK );
	}

	if( m_cfParamTrack == 0 )
	{ 
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::LoadList

HRESULT CParamStrip::LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO		ck;
	MMCKINFO		ckListTrack;
	MMCKINFO		ckListItem;
	DWORD			dwByteCount;
	DWORD			dwSize;
	HRESULT			hr = E_FAIL;

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

	// Load the items
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK:
					{
						DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER iObjectHeader;

						// Read in the item's header structure
						dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER ), ck.cksize );
						hr = pIStream->Read( &iObjectHeader, dwSize, &dwByteCount );

						// Handle any I/O error by returning a failure code
						if( FAILED( hr ) ||  dwByteCount != dwSize )
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
						
						GUID guidObject = m_pTrackObject->GetCLSID();
						if(::IsEqualGUID(guidObject, iObjectHeader.guidObject) == FALSE)
						{
							// This is not our curve data
							hr = E_FAIL;
							goto ON_ERROR;
						}

						break;
					}


					case DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST:
						while( pIRiffStream->Descend( &ckListTrack, &ck, 0 ) == 0 )
						{
							switch( ckListTrack.ckid )
							{
								case DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK:
								{
									DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER iParamHeader;

									// Read in the item's header structure
									dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER ), ckListTrack.cksize );
									hr = pIStream->Read( &iParamHeader, dwSize, &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									// This method called when pasting or dropping items
									// Do not overwrite strip parameters

									if(m_dwParamIndex != iParamHeader.dwIndex)
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									break;
								}

								case DMUS_FOURCC_PARAMCONTROLTRACK_CURVES_CHUNK:
								{
									// Store size of this chunk
									dwSize = ckListTrack.cksize - 4;

									// Read size of struct
									DWORD dwStructSize;
									hr = pIStream->Read( &dwStructSize, sizeof(DWORD), &dwByteCount );
									//dwSize -= sizeof(DWORD);

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != sizeof(DWORD) )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									// Read all items
									while( dwSize )
									{
										CTrackItem* pNewItem = new CTrackItem( m_pTrackMgr, this );
										if( pNewItem == NULL )
										{
											hr = E_OUTOFMEMORY;
											goto ON_ERROR;
										}
										hr = pNewItem->LoadListItem( pIStream, dwStructSize );
										if( FAILED ( hr ) )
										{
											delete pNewItem;
											goto ON_ERROR;
										}
										list.AddTail( pNewItem );
										dwSize -= dwStructSize;
									}
									break;
								}

								case FOURCC_LIST:
									switch( ckListTrack.fccType )
									{
										case DMUS_FOURCC_STRIP_UI_LIST:
											while( pIRiffStream->Descend( &ckListItem, &ckListTrack, 0 ) == 0 )
											{
												switch( ckListItem.ckid )
												{
													case DMUS_FOURCC_STRIP_UI_CHUNK:
													{
														ioStripUI iStripUI;

														// Read in the item's header structure
														dwSize = min( sizeof( ioStripUI ), ck.cksize );
														hr = pIStream->Read( &iStripUI, dwSize, &dwByteCount );

														// Handle any I/O error by returning a failure code
														if( FAILED( hr )
														||  dwByteCount != dwSize )
														{
															hr = E_FAIL;
															goto ON_ERROR;
														}
														break;
													}
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
// CParamStrip::SaveList

HRESULT CParamStrip::SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream )
{
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	MMCKINFO ckStrip;
	MMCKINFO ck;
	HRESULT hr = E_FAIL;
	DWORD dwBytesWritten = 0;

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

	// Save the Object header
	MMCKINFO ckObject;
	ckObject.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK;
	if( pIRiffStream->CreateChunk( &ckObject, 0 ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Clear out the structure (clears out the padding bytes as well).
	DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER oObjectHeader;
	m_pTrackObject->GetObjectHeader(oObjectHeader);

	// Write the structure out to the stream
	hr = pIStream->Write( &oObjectHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER) )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK chunk
	if( pIRiffStream->Ascend(&ckObject, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}


	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST list chunk
	ckStrip.fccType = DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST;
	if( pIRiffStream->CreateChunk( &ckStrip, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER oParamHeader;
		ZeroMemory( &oParamHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER) );

		// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER structure
		oParamHeader.dwIndex = m_dwParamIndex;
		oParamHeader.dwFlags = m_dwParamFlagsDM;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oParamHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK chunk
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
		// Create a chunk to store the list of items 
		MMCKINFO ckItems;
		ckItems.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_CURVES_CHUNK;
		if( pIRiffStream->CreateChunk( &ckItems, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write size of the DMUS_IO_PARAMCONTROLTRACK_CURVEINFO struct
		DWORD dwStructSize = sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO);
		DWORD dwBytesWritten;
		hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			// Handle I/O errors by return an error code
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
			hr = pItem->SaveListItem( pIStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		pIRiffStream->Ascend( &ckItems, 0 );
	}

	// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST list chunk
	if( pIRiffStream->Ascend(&ckStrip, 0) != 0 )
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
// CParamStrip::LoadStrip

HRESULT CParamStrip::LoadStrip( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent )
{
	MMCKINFO		ck;
	MMCKINFO		ckList;
	DWORD			dwByteCount;
	DWORD			dwSize;
	HRESULT			hr = E_FAIL;

	if( pIRiffStream == NULL 
	||  pckParent == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	m_fInsertingCurve = FALSE;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Load the items
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK:
			{
				DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER iParamHeader;

				// Read in the item's header structure
				dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER ), ck.cksize );
				hr = pIStream->Read( &iParamHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwParamIndex = iParamHeader.dwIndex;
				m_dwParamFlagsDM = iParamHeader.dwFlags;
				break;
			}

			case DMUS_FOURCC_PARAMCONTROLTRACK_CURVES_CHUNK:
			{
				// Store size of this chunk
				dwSize = ck.cksize - 4;

				// Read size of struct
				DWORD dwStructSize = 0;
				hr = pIStream->Read( &dwStructSize, sizeof(DWORD), &dwByteCount );
				//dwSize -= sizeof(DWORD);

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != sizeof(DWORD) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Read all items
				while( dwSize )
				{
					CTrackItem* pNewItem = new CTrackItem( m_pTrackMgr, this );
					if( pNewItem == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}
					hr = pNewItem->LoadTrackItem( pIStream, dwStructSize );
					if( FAILED ( hr ) )
					{
						delete pNewItem;
						goto ON_ERROR;
					}
					InsertByAscendingTime( pNewItem, FALSE );
					dwSize -= dwStructSize;
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_STRIP_UI_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_STRIP_UI_CHUNK:
								{
									ioStripUI iStripUI;

									// Read in the item's header structure
									dwSize = min( sizeof( ioStripUI ), ck.cksize );
									hr = pIStream->Read( &iStripUI, dwSize, &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									m_StripUI.m_dblVerticalZoom = iStripUI.m_dblVerticalZoom;
									m_StripUI.m_lStripHeight = iStripUI.m_lStripHeight;
									m_StripUI.m_nStripView = iStripUI.m_nStripView;
									m_StripUI.m_nSnapToRefTime = iStripUI.m_nSnapToRefTime;
									m_StripUI.m_nSnapToMusicTime = iStripUI.m_nSnapToMusicTime;
									m_StripUI.m_dwFlagsUI = iStripUI.m_dwFlagsUI;
									m_StripUI.m_nSnapToRefTime_GridsPerSecond = iStripUI.m_nSnapToRefTime_GridsPerSecond;

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
				}
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	RELEASE( pIStream );

	hr = GetParamInfoFromCLSID(&m_ParamInfo);

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SaveStrip

HRESULT CParamStrip::SaveStrip( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ckStrip;
	MMCKINFO ck;
	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

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

	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST list chunk
	ckStrip.fccType = DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST;
	if( pIRiffStream->CreateChunk( &ckStrip, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER oParamHeader;
		ZeroMemory( &oParamHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER) );

		// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER structure
		oParamHeader.dwIndex = m_dwParamIndex;
		oParamHeader.dwFlags = m_dwParamFlagsDM;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oParamHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_CHUNK chunk
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
		// Create a chunk to store the list of items 
		MMCKINFO ckItems;
		ckItems.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_CURVES_CHUNK;
		if( pIRiffStream->CreateChunk( &ckItems, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write size of the DMUS_IO_PARAMCONTROLTRACK_CURVEINFO struct
		DWORD dwStructSize = sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO);
		DWORD dwBytesWritten;
		hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the item list
		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

			// Save each item
			hr = pItem->SaveTrackItem( pIStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		pIRiffStream->Ascend( &ckItems, 0 );
	}

	// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST list chunk
	if( pIRiffStream->Ascend(&ckStrip, 0) != 0 )
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
// CParamStrip::SaveStripDesignData

HRESULT CParamStrip::SaveStripDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ckMain;
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Create the DMUS_FOURCC_STRIP_UI_LIST list chunk
	ckMain.fccType = DMUS_FOURCC_STRIP_UI_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_STRIP_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_STRIP_UI_CHUNK;
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
			TimelineToStripUI();
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioStripUI oStripUI;
		ZeroMemory( &oStripUI, sizeof(ioStripUI) );

		// Fill in the members of the ioStripUI structure
		oStripUI.m_dblVerticalZoom = m_StripUI.m_dblVerticalZoom;
		oStripUI.m_lStripHeight = m_StripUI.m_lStripHeight;
		oStripUI.m_nStripView = m_StripUI.m_nStripView;
		oStripUI.m_dwFlagsUI = m_StripUI.m_dwFlagsUI;
		oStripUI.m_nSnapToRefTime = m_StripUI.m_nSnapToRefTime;
		oStripUI.m_nSnapToMusicTime = m_StripUI.m_nSnapToMusicTime;
		oStripUI.m_nSnapToRefTime_GridsPerSecond = m_StripUI.m_nSnapToRefTime_GridsPerSecond;


		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oStripUI, sizeof(ioStripUI), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioStripUI) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_STRIP_UI_CHUNK chunk
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

	// Ascend out of the DMUS_FOURCC_STRIP_UI_LIST list chunk
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
// CParamStrip::OnShowStripProperties

void CParamStrip::OnShowStripProperties()
{
	m_fShowItemProps = false;

	if( m_pTrackMgr )
	{
		m_pTrackMgr->SetParamStripForPropSheet(this);
		m_pTrackMgr->OnShowProperties();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnShowItemProperties

void CParamStrip::OnShowItemProperties()
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
// CParamStrip::IsSelected

bool CParamStrip::IsSelected()
{
	// If anything is selected, return true
	
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

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
// CParamStrip::IsEmpty

bool CParamStrip::IsEmpty()
{
	return m_lstTrackItems.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SelectSegment

// Return true if anything changed
bool CParamStrip::SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime )
{
	ASSERT(m_pTrackMgr);
	if( m_pTrackMgr == NULL)
	{
		return false;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL )
	{
		return false;
	}

	REFERENCE_TIME rtTime = 0;

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
	CSelectedRegion region(cmtBeg, cmtEnd);

	// Add the region to the list of selected regions
	m_pSelectedRegions->AddRegion(region);

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return SelectItemsInSelectedRegions();
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::UnselectAll

void CParamStrip::UnselectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item 
		m_lstTrackItems.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_pSelectedRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DeleteBetweenMeasureBeats

bool CParamStrip::DeleteBetweenMeasureBeats( long lmStart, long lbStart, long lmEnd, long lbEnd )
{
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

		// Check if the item's measure value is before the start measure
		if( pItem->m_lMeasure < lmStart )
		{
			// Too early - keep looking
			continue;
		}

		// Check if the item's measure value is after the end measure
		if( pItem->m_lMeasure > lmEnd )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// If the item is in the start measure, check if it is before the start beat
		if( ( pItem->m_lMeasure == lmStart )
		&&	( pItem->m_lBeat < lbStart ) )
		{
			// Too early - keep looking
			continue;
		}

		// If the item is in the end measure, check if it is after the end beat
		if( ( pItem->m_lMeasure == lmEnd)
		&&	( pItem->m_lBeat > lbEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// Within the given range, delete the item
		m_lstTrackItems.RemoveAt(posTemp);
		delete pItem;
		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::InsertByAscendingTime

void CParamStrip::InsertByAscendingTime( CTrackItem* pItemToInsert, BOOL fPaste )
{
	// Ensure the pItemToInsert pointer is valid
	if ( pItemToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CTrackItem* pItem;
	POSITION posCurrent, posNext = m_lstTrackItems.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pItem = m_lstTrackItems.GetNext( posNext );

		if( fPaste )
		{
			if( pItem->m_mtStartTime == pItemToInsert->m_mtStartTime )
			{
				// Replace item
				m_lstTrackItems.InsertBefore( posCurrent, pItemToInsert );
				m_lstTrackItems.RemoveAt( posCurrent );
				delete pItem;
				return;
			}
		}

		if( pItem->m_mtStartTime > pItemToInsert->m_mtStartTime )
		{
			// Insert before posCurrent (which is the position of pItem)
			m_lstTrackItems.InsertBefore( posCurrent, pItemToInsert );
			return;
		}
	}

	// pItemToInsert is later than all items in the list, add it at the end of the list
	m_lstTrackItems.AddTail( pItemToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnUpdate

HRESULT CParamStrip::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	HRESULT hr = E_INVALIDARG;
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If the update isn't for our strip, exit
	DWORD dwTrackGroupBits = m_pTrackMgr->GetGroupBits();
	if( !(dwGroupBits & dwTrackGroupBits) )
	{
		return E_INVALIDARG;
	}

// TODO - Do we need Framework messages?????
/*
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
				// Find the items that changed and update them
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
				// Find the items that changed and update them
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
				// Find the items that changed and update them
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
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}

			m_pTrackMgr->SyncWithDirectMusic();
		}
	}
*/

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SaveSelectedItems

HRESULT CParamStrip::SaveSelectedItems( LPSTREAM pIStream, REFERENCE_TIME rtOffset, CTrackItem* pItemAtDragPoint )
{
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Create a list to store the items to save in
	CTypedPtrList<CPtrList, CTrackItem*> lstItemsToSave;

	// Iterate throught the list of items
	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		// Get a pointer to each item
		CTrackItem* pItem = m_lstTrackItems.GetNext(position);

		// Check if the item is selected
		if(pItem->m_fSelected )
		{
			// Add the item to the list of items to save
			CTrackItem* pNewItem = new CTrackItem(m_pTrackMgr, this, *pItem);
			if(pNewItem)
			{
				// TODO!!!   Put back
				//mtPasteOverwriteRange = max( mtPasteOverwriteRange, );
				lstItemsToSave.AddTail( pNewItem );
			}
		}
	}

	// Get item for DMUS_FOURCC_COPYPASTE_UI_CHUNK
	CTrackItem* pCopyPasteItem = pItemAtDragPoint;
	if(pCopyPasteItem == NULL)
	{
		if( lstItemsToSave.IsEmpty() == FALSE )
		{
			pCopyPasteItem = lstItemsToSave.GetHead();
		}
	}

	// Try and allocate a RIFF stream
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	if(FAILED(AllocRIFFStream( pIStream, &pIRiffStream)) || pIRiffStream == NULL)
	{
		return E_OUTOFMEMORY;
	}

	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

	// Create DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
	if(pIRiffStream->CreateChunk(&ck, 0) != 0)
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	MUSIC_TIME mtPasteOverwriteRange = -1;

	// If range selected via timeline, use it
	// TODO!!!   Put back
	//if( m_bGutterSelected && (m_lGutterBeginSelect != m_lGutterEndSelect) )
	{
		long lStartTime = 0;
		long lEndTime = 0;
		m_pSelectedRegions->GetSpan( lStartTime, lEndTime );

		// Compute the start offset
		MUSIC_TIME mtStartOffset = 0;
		long lMeasure = 0;
		long lBeat = 0;
		pTimeline->ClocksToMeasureBeat(dwGroupBits, 0, lStartTime, &lMeasure, &lBeat);
		m_pTrackMgr->MeasureBeatGridTickToClocks(lMeasure, lBeat, 0, 0, &mtStartOffset);

		// Compute the end offset
		MUSIC_TIME mtEndOffset = 0;
		pTimeline->ClocksToMeasureBeat(dwGroupBits, 0, lEndTime, &lMeasure, &lBeat);
		m_pTrackMgr->MeasureBeatGridTickToClocks(lMeasure, (lBeat + 1), 0, 0, &mtEndOffset);

		mtPasteOverwriteRange = mtEndOffset - mtStartOffset;
	}

	// Clear out the structure (clears out the padding bytes as well).
	ioCopyPasteUI oCopyPasteUI;
	ZeroMemory( &oCopyPasteUI, sizeof(ioCopyPasteUI) );

	// Fill in the members of the ioCopyPasteUI structure
	if(pCopyPasteItem)
	{
		long lMinute = 0;
		long lMeasure = 0;

		m_pTrackMgr->UnknownTimeToRefTime( rtOffset, &oCopyPasteUI.m_rtOffset );
		m_pTrackMgr->UnknownTimeToRefTime( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_rtStartTime );
		m_pTrackMgr->RefTimeToMinSecGridMs( this, oCopyPasteUI.m_rtStartTime, &lMinute, &oCopyPasteUI.m_lRefTimeSec,
										   &oCopyPasteUI.m_lRefTimeGrid, &oCopyPasteUI.m_lRefTimeMs );

		m_pTrackMgr->UnknownTimeToClocks( rtOffset, &oCopyPasteUI.m_mtOffset );
		m_pTrackMgr->UnknownTimeToClocks( pCopyPasteItem->m_rtTimePhysical, &oCopyPasteUI.m_mtStartTime );
		m_pTrackMgr->ClocksToMeasureBeatGridTick( oCopyPasteUI.m_mtStartTime, &lMeasure, &oCopyPasteUI.m_lMusicTimeBeat,
												 &oCopyPasteUI.m_lMusicTimeGrid, &oCopyPasteUI.m_lMusicTimeTick );

	}
	
	pTimeline->ClocksToRefTime(mtPasteOverwriteRange, &oCopyPasteUI.m_rtPasteOverwriteRange);
	oCopyPasteUI.m_mtPasteOverwriteRange = mtPasteOverwriteRange;
	oCopyPasteUI.m_fRefTimeTrack = m_pTrackMgr->IsRefTimeTrack();

	// Write the structure out to the stream
	DWORD dwBytesWritten = 0;
	HRESULT hr = pIStream->Write( &oCopyPasteUI, sizeof(ioCopyPasteUI), &dwBytesWritten);
	if(FAILED(hr) ||  dwBytesWritten != sizeof(ioCopyPasteUI))
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Ascend out of the DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
	if(pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Check that anything is selected
	if(lstItemsToSave.IsEmpty())
	{
		pIRiffStream->Release();
		return S_FALSE;
	}

	// For each item in lstItemsToSave, subtract rtOffset from m_rtTimePhysical
	REFERENCE_TIME rtNormalizeOffset = 0;
	if(pCopyPasteItem == NULL)
	{
		ASSERT( 0 );
		rtNormalizeOffset = 0;
	}
	else
	{
		rtNormalizeOffset = pCopyPasteItem->m_rtTimePhysical;
	}

	NormalizeList(pTimeline, dwGroupBits, lstItemsToSave, rtNormalizeOffset );

	// Save the list of items into pIStream
	hr = SaveList(lstItemsToSave, pIStream);

	// Empty the temporary list of items
	EmptyList(lstItemsToSave);

	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::CycleItems

HRESULT CParamStrip::CycleItems( long lXPos  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if( m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure = 0;
	long lBeat = 0;
	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
	HRESULT hr = pTimeline->PositionToMeasureBeat( dwGroupBits, 0, lXPos, &lMeasure, &lBeat );

	if( SUCCEEDED ( hr ) ) 
	{
		CTrackItem* pFirstItem = NULL;
		CTrackItem* pSecondItem = NULL;
		CTrackItem* pItem;

		hr = E_FAIL;

		POSITION pos = m_lstTrackItems.GetHeadPosition();
		while( pos )
		{
			pItem = m_lstTrackItems.GetNext( pos );

			if( pItem->m_lMeasure == lMeasure
			&&  pItem->m_lBeat == lBeat )
			{
				if( pFirstItem == NULL )
				{
					pFirstItem = pItem;
				}
				else if( pSecondItem == NULL )
				{
					pSecondItem = pItem;
				}

				if( pItem->m_wFlagsUI & RF_TOP_ITEM )
				{
					if( pos )
					{
						// Cycle to next item if on same measure/beat
						CTrackItem* pNextItem = GetNextItem( pos );
						if( pNextItem )
						{
							if( pNextItem->m_lMeasure == lMeasure
							&&  pNextItem->m_lBeat == lBeat )
							{
								UnselectAll();
								pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
								pNextItem->m_fSelected = TRUE;
								pNextItem->m_wFlagsUI |= RF_TOP_ITEM;
								hr = S_OK;
								break;
							}
						}
					}

					// Cycle to first item on same measure/beat
					UnselectAll();
					pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
					break;
				}
			}

			if( pItem->m_lMeasure > lMeasure
			||  pos == NULL )
			{
				UnselectAll();
				if( pSecondItem )
				{
					pSecondItem->m_fSelected = TRUE;
					pSecondItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				else if( pFirstItem )
				{
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				break;
			}
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Update the selection regions to include just this selected item
		SelectRegionsFromSelectedItems();

		// Redraw the strip
		m_pTrackMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::RecomputeMeasureBeats

void CParamStrip::RecomputeMeasureBeats()
{
	CTrackItem* pItem;
	long lMeasure;
	long lBeat;
	long lTick;

	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// Recompute measure/beat of all items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		pItem = m_lstTrackItems.GetNext( pos );

		m_pTrackMgr->ClocksToMeasureBeatTick( pItem->m_mtStartTime, &lMeasure, &lBeat, &lTick ); 
		pItem->m_lMeasure = lMeasure;
		pItem->m_lBeat = lBeat;
		pItem->m_lTick = lTick;
		
		pItem->UpdateTimeFieldsFromMeasureBeatTick();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetNextGreatestUniqueTime

MUSIC_TIME CParamStrip::GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick )
{
	DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime = 0;

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return 0;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return 0;
	}

	// Get next greatest unique time
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}

		m_pTrackMgr->MeasureBeatTickToClocks( pItem->m_lMeasure, 0, 0, &mtTime );

		DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
		pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, mtTime, NULL, &dmTimeSig );
		if( pItem->m_lBeat > dmTimeSig.bBeatsPerMeasure )
		{
			break;
		}

		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat )
		{
			lTick = pItem->m_lTick + 1;

			// Take care of measure/beat rollover
			m_pTrackMgr->MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );  
		}
	}

	m_pTrackMgr->MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::RecomputeTimes

bool CParamStrip::RecomputeTimes()
{
	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;
	long lTick;

	bool fChanged = false;

	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return FALSE;
	}

	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Save our current position
		POSITION pos2 = pos;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Make sure measure and beat are valid
		if( pItem->m_lMeasure >= 0
		&&  pItem->m_lBeat >= 0 )
		{
			// Using the item's current measure, beat, tick settings, determine which measure, bet and tick
			// the item will end up on
			m_pTrackMgr->MeasureBeatTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lTick, &mtTime );  
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 

			// Check if either the measure or beat were changed
			if( pItem->m_mtStartTime != mtTime
			||	pItem->m_lMeasure != lMeasure
			||	pItem->m_lBeat != lBeat
			||	pItem->m_lTick != lTick )
			{
				// Remove the itme from the list
				m_lstTrackItems.RemoveAt( pos2 );

				// The measure of the item changed
				if( pItem->m_lMeasure != lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// items on beat 7 would end up on next measure's beat 3
					while( pItem->m_lMeasure != lMeasure )
					{
						// Keep moving back a beat until the measure does not change
						m_pTrackMgr->MeasureBeatTickToClocks( lMeasure, --lBeat, lTick, &mtTime );  
						m_pTrackMgr->ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 
					}
				}

				mtTime = GetNextGreatestUniqueTime( lMeasure, lBeat, lTick ); 
				m_pTrackMgr->ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 

				pItem->m_mtStartTime = mtTime;
				pItem->m_lMeasure = lMeasure;
				pItem->m_lBeat = lBeat;
				pItem->m_lTick = lTick;
				fChanged = TRUE;

				InsertByAscendingTime( pItem, FALSE );
			}
		}
	}

	// Return whether or not anything changed
	return fChanged;
}




/////////////////////////////////////////////////////////////////////////////
// CParamStrip::MarkSelectedItems

// ORs dwFlags with the m_dwBits of each selected item
void CParamStrip::MarkSelectedItems( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

		// Check if the item is selected
		if( pItem->m_fSelected )
		{
			// It's selected - update m_dwBits
			pItem->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DeleteMarked

// deletes items marked by given flag
void CParamStrip::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this item
		if( pItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item
			m_lstTrackItems.RemoveAt(pos2);
			delete pItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DeleteSelectedItems

void CParamStrip::DeleteSelectedItems()
{
	// Start iterating through the list of items
	POSITION pos1 = m_lstTrackItems.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos1 );

		// Check if the current item is selected
		if( pItem->m_fSelected )
		{
			// This item is selected, remove it from the list
			m_lstTrackItems.RemoveAt( pos2 );
			delete pItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetFirstSelectedItem

CTrackItem* CParamStrip::GetFirstSelectedItem()
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );

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
// CParamStrip::UnMarkItems

// unmarks flag m_dwUndermined field CTrackItems in list
void CParamStrip::UnMarkItems( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lstTrackItems.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::TimelineToStripUI

void CParamStrip::TimelineToStripUI( void )
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
					m_StripUI.m_dwFlagsUI |= STRIPUI_ACTIVESTRIP;
				}

				RELEASE( pIActiveStrip );
			}
			
			RELEASE( pIUnknown );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::StripUIToTimeline

void CParamStrip::StripUIToTimeline( void )
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

	// Set active strip
	// Other strips do not reset the active strip
//	if( m_StripUI.m_dwFlagsUI & STRIPUI_ACTIVESTRIP )
//	{
//		var.vt = VT_UNKNOWN;
//		if( SUCCEEDED( QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
//		{
//			m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
//		}
//	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnCreate

HRESULT CParamStrip::OnCreate( void )
{
	// Register our clipboard formats
	RegisterClipboardFormats();

	// Load button bitmaps
	if( InterlockedIncrement( &sm_lBitmapRefCount ) == 1 )
	{
		BITMAP bmParam;
		if( sm_bmpZoomInBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpZoomInBtnUp.LoadBitmap( IDB_ZOOMIN_UP );
			sm_bmpZoomInBtnUp.GetBitmap( &bmParam );
			sm_bmpZoomInBtnUp.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( sm_bmpZoomInBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpZoomInBtnDown.LoadBitmap( IDB_ZOOMIN_DOWN );
			sm_bmpZoomInBtnDown.GetBitmap( &bmParam );
			sm_bmpZoomInBtnDown.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( sm_bmpZoomOutBtnUp.GetSafeHandle() == NULL )
		{
			sm_bmpZoomOutBtnUp.LoadBitmap( IDB_ZOOMOUT_UP );
			sm_bmpZoomOutBtnUp.GetBitmap( &bmParam );
			sm_bmpZoomOutBtnUp.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
		if( sm_bmpZoomOutBtnDown.GetSafeHandle() == NULL )
		{
			sm_bmpZoomOutBtnDown.LoadBitmap( IDB_ZOOMOUT_DOWN );
			sm_bmpZoomOutBtnDown.GetBitmap( &bmParam );
			sm_bmpZoomOutBtnDown.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnDestroy

HRESULT CParamStrip::OnDestroy( void )
{
	// Delete the Bitmap buttons
	if( InterlockedDecrement( &sm_lBitmapRefCount ) == 0 )
	{
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

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetStripRect
//
BOOL CParamStrip::GetStripRect(LPRECT pRectStrip)
{

	ASSERT(pRectStrip);
	if(pRectStrip == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_FAIL;
	}

	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = pRectStrip;
	if( FAILED ( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
	{
		return FALSE;
	}

	// Adjust the strip rect to encompass the WHOLE strip, not only the part
	// that is visible on the screen
	long lHeight;
	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		lHeight = STRIP_MINIMIZE_HEIGHT;
	}
	else
	{

		if( FAILED ( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
		{
			return FALSE;
		}
		lHeight = V_I4(&var);
	}

	POINT ptTop;
	POINT ptBottom;
	
	ptTop.x = 0;
	ptTop.y = 0;
	ptBottom.x = 0;
	ptBottom.y = lHeight;
	
	m_pTrackMgr->m_pTimeline->StripToWindowPos( this, &ptTop );
	m_pTrackMgr->m_pTimeline->StripToWindowPos( this, &ptBottom );

	pRectStrip->top = ptTop.y;
	pRectStrip->bottom = ptBottom.y;
	
	return TRUE;
}


HRESULT CParamStrip::SetProperties(StripInfo* pStripInfo)
{
	ASSERT(pStripInfo);
	if(pStripInfo == NULL)
	{
		return E_POINTER;
	}

	m_dwParamIndex = pStripInfo->m_dwParamIndex;
	m_strStripName = pStripInfo->m_sParamName;

	// Copy the param info
	CopyMemory(&m_ParamInfo, &pStripInfo->m_ParamInfo, sizeof(MP_PARAMINFO));

	return S_OK;
}


CString CParamStrip::GetName()
{
	return m_strStripName;
}

void CParamStrip::SetName(CString sName)
{
	m_strStripName = sName;
}

HRESULT CParamStrip::Insert()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	StartInsertingCurve();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::StartInsertingCurve

void CParamStrip::StartInsertingCurve()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pITimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pITimeline);
	if(pITimeline == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	m_fltInsertingStartValue = 0;
	m_fltInsertingEndValue = 0;
	m_lInsertingStartClock = 0;
	m_lInsertingEndClock = 0;

	m_fInsertingCurve = INSERT_CURVE_START_VALUE;

	// Ensure the arrow cursor is displayed
	m_hCursor = LoadCursor(NULL, IDC_ARROW);
	::SetCursor( m_hCursor );

	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	pITimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);

	if(pIFramework)
	{
		// Remove the old statuc bar first
		if(m_hKeyStatusBar)
		{
			pIFramework->RestoreStatusBar(m_hKeyStatusBar);	
			m_hKeyStatusBar = NULL;
		}
		pIFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar  );
		pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 25 );
	}

	CPoint pt( -1, -1 );
	OnUpdateInsertCurveValue( pt, pt );
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::StopInsertingCurve

void CParamStrip::StopInsertingCurve()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pITimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pITimeline);
	if(pITimeline == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	if(m_fInsertingCurve)
	{
		if(m_fInsertingCurve == INSERT_CURVE_START_VALUE)
		{
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			pITimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
		}

		m_fInsertingCurve = INSERT_CURVE_FALSE;

		if(pIFramework)
		{
			pIFramework->RestoreStatusBar(m_hKeyStatusBar);	
			m_hKeyStatusBar = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::YPosToValue

float CParamStrip::YPosToValue( long lYPos )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return 0;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return 0;
	}

	long lMaxValue = 0;
	float fltValue = 0;

	// Store maximum value
	lMaxValue = (long) m_ParamInfo.mpdMaxValue;

	// Get Strip height
	VARIANT var;
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}

	int nStripCenter = (nStripHeight >> 1);

	float fValueRange = m_ParamInfo.mpdMaxValue - m_ParamInfo.mpdMinValue;
	float fltYScale = (float)nStripHeight / fValueRange;
	float fltZeroValue = m_ParamInfo.mpdNeutralValue;

	float fltYPos  = (float)nStripHeight - lYPos;
	if(UseCenterLine())
	{
		fltYPos = (float) nStripCenter - lYPos;
	}

	fltValue = m_ParamInfo.mpdMinValue + (fltYPos / fltYScale);

	return fltValue ;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::ValueToYPos

short CParamStrip::ValueToYPos( float fltValue )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return 0;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return 0;
	}

	DWORD dwYPos = 0;

	// Get Strip height
	VARIANT var;
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}

	int nStripCenter = (nStripHeight >> 1);

	float fValueRange = (m_ParamInfo.mpdMaxValue - m_ParamInfo.mpdMinValue);
	float fltYScale = (float)nStripHeight / fValueRange;
	float fltZeroValue = m_ParamInfo.mpdNeutralValue;

	fltValue = (m_ParamInfo.mpdMaxValue - fltValue);

	if(UseCenterLine())
	{
		float fScaledYPos = fltValue * fltYScale;
		fScaledYPos = fScaledYPos < 0 ? (-fScaledYPos) : fScaledYPos;
		dwYPos = (DWORD) (nStripCenter - fScaledYPos);
	}
	else
	{
		float fScaledYPos = fltValue * fltYScale;
		fScaledYPos = fScaledYPos < 0 ? (-fScaledYPos) : fScaledYPos;
		dwYPos = (DWORD) (nStripHeight - floor(fScaledYPos + 0.5));
	}

	return (short)dwYPos;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnUpdateInsertCurveValue
//
// Called when tracker is resized during curve insertion.
// Updates the status bar and computes curve start and end values and
// start and end times.
void CParamStrip::OnUpdateInsertCurveValue( POINT ptLeft, POINT ptRight )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}
	
	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}
	
	CString strStart;
	CString strEnd;
	CString strStatus;

	strStart.LoadString(IDS_START_TEXT);
	strEnd.LoadString(IDS_END_TEXT);

	// Get CurveStrip rect
	CRect rectStrip;
	if(!GetStripRect(rectStrip))
	{
		return;
	}

	// Convert points into Window coordinates
	CPoint ptWndLeft(ptLeft);
	CPoint ptWndRight(ptRight);
	pTimeline->StripToWindowPos((IDMUSProdStrip *)this, &ptWndLeft);
	pTimeline->StripToWindowPos((IDMUSProdStrip *)this, &ptWndRight);

	// Make sure ptLeft.y is in this ParamStrip
	if( ptWndLeft.y < rectStrip.top
	||  ptWndLeft.y > rectStrip.bottom )
	{
		// ptLeft.y is NOT in this ParamStrip
		strStatus.Format( "%s, %s", strStart, strEnd );
	}
	else
	{
		// Get clock length of this ParamStrip
		VARIANT var;
		pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lClockLength = V_I4(&var);

		// Make sure ptLeft.x is in this CurveStrip
		long lStartClock = 0;

		pTimeline->PositionToClocks( ptLeft.x, &lStartClock );
		if( lStartClock < 0
		||  lStartClock > lClockLength )
		{
			// ptLeft.x is NOT in this CurveStrip
			strStatus.Format( "%s, %s", strStart, strEnd );
		}
		else
		{
	
			// start and end values that will be *displayed*
			float fltDispStartValue = 0;
			float fltDispEndValue = 0;

			// Set StartValue
			m_lInsertingStartClock = lStartClock % lClockLength;
			m_fltInsertingStartValue = YPosToValue( ptLeft.y );
			
			fltDispStartValue = m_fltInsertingStartValue;
			
			if(m_ParamInfo.mpType != MPT_FLOAT)
			{
				if(fltDispStartValue < 0)
				{
					fltDispStartValue = (int)(fltDispStartValue - 0.5);
				}
				else
				{
					fltDispStartValue = (int)(fltDispStartValue + 0.5);
				}
			}

			// See if ptRight.y is in this CurveStrip
			if( ptWndRight.y >= rectStrip.top
			&&  ptWndRight.y <= rectStrip.bottom )
			{
				long lEndClock;

				// ptRight.y is in this CurveStrip so set EndValue
				pTimeline->PositionToClocks( ptRight.x, &lEndClock );
				m_lInsertingEndClock = m_lInsertingStartClock + (lEndClock - lStartClock);
				m_fltInsertingEndValue = YPosToValue( ptRight.y );
				fltDispEndValue = m_fltInsertingEndValue;

				if(m_ParamInfo.mpType != MPT_FLOAT)
				{
					if(fltDispEndValue < 0)
					{
						fltDispEndValue = (int)(fltDispEndValue - 0.5);
					}
					else
					{
						fltDispEndValue = (int)(fltDispEndValue + 0.5);
					}
				}

				strStatus.Format( "%s %.03f, %s %.03f", strStart, fltDispStartValue, strEnd, fltDispEndValue );

			}
			else
			{
				strStatus.Format( "%s %.03f, %s", strStart, fltDispStartValue, strEnd );
			}
		}
	}

	// Refresh status bar
	if(pIFramework )
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnUpdateDragCurveValue
//
// Called when the tracker is resized during curve dragging/resizing.
// Updates the status bar with start, end values.
//
void CParamStrip::OnUpdateDragCurveValue(CCurveTracker& tracker, int nAction)
{

	CRect newRect = GetUpdatedCurveRect(tracker, tracker.m_pTrackItem, nAction);

	// Figure out start and end values and positions
	long lStartClock = 0;
	long lEndClock = 0;

	m_pTrackMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
	m_pTrackMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);

	float fltStartValue = YPosToValue(newRect.top);
	float fltEndValue = YPosToValue(newRect.bottom);

	// Check for the type for the values
	if(m_ParamInfo.mpType != MPT_FLOAT)
	{
		if(fltStartValue < 0)
		{
			fltStartValue = (int)(fltStartValue - 0.5);
		}
		else
		{
			fltStartValue = (int)(fltStartValue + 0.5);
		}

		if(fltEndValue < 0)
		{
			fltEndValue = (int)(fltEndValue - 0.5);
		}
		else
		{
			fltEndValue = (int)(fltEndValue + 0.5);
		}
	}


	// create the status bar string
	CString strStart;
	CString strEnd;
	CString strStatus;

	strStart.LoadString( IDS_START_TEXT );
	strEnd.LoadString( IDS_END_TEXT );

	strStatus.Format("%s %.03f, %s %.03f", strStart, fltStartValue, strEnd, fltEndValue);
	
	// Refresh status bar
	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	if(pIFramework)
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetUpdatedCurveRect
//
// Returns an updated rect for the given curve based on the tracker's rect
// and offset.  The rect returned is relative to the curve strip.
// This method is used in the process dragging existing curves.
CRect CParamStrip::GetUpdatedCurveRect(CCurveTracker& curveTracker, 
									   CTrackItem* pCurve,
									   int nAction)
{
	ASSERT(pCurve != NULL);

	CRect newRect;

	if (nAction == CTRK_DRAGGING) 

	{
		newRect = pCurve->m_rectFrame;
		newRect.OffsetRect(curveTracker.m_offset);
	}
	else
	{
		// Get Curve Strip rectangle
		CRect rectStrip;
		if (!GetStripRect(rectStrip))
		{
			return newRect;
		}

		// Get Curve Strip left position
		long lLeftPosition;
		m_pTrackMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
		m_pTrackMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

		// get the new curve bounds relative to the curve strip
		newRect = curveTracker.m_rect;
		newRect.OffsetRect(-rectStrip.TopLeft());
		newRect.OffsetRect(lLeftPosition, 0);
		newRect.OffsetRect(-curveTracker.m_nXOffset, 0);
	}

	// swap values of the rect if necessary
	if (pCurve->m_fltEndValue > pCurve->m_fltStartValue) 
	{
		int temp = newRect.top;
		newRect.top = newRect.bottom;
		newRect.bottom = temp;
	}

	return newRect;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DragModify
//
// Handles move operation on the selected curves.
void CParamStrip::DragModify(CTrackItem* pTrackItem, int nStripXPos, int nStripYPos, int nAction, bool bMakeCopy)
{
	ASSERT(pTrackItem);
	if(pTrackItem == NULL)
	{
		return;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return;
	}

	// setup the status bar
	if( pIFramework )
	{
		// Remove the old statuc bar first
		if(m_hKeyStatusBar)
		{
			pIFramework->RestoreStatusBar(m_hKeyStatusBar);	
			m_hKeyStatusBar = NULL;
		}
		
		pIFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar );
		pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 25 );

	}

	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	long lXOffset = 0;
	CRect selRect = pTrackItem->m_rectFrame;
	CPoint point( nStripXPos, nStripYPos );

	// convert point to window coordinates
	VERIFY( SUCCEEDED( pTimeline->StripToWindowPos(this, &point) ) );
	
	// convert the frame rect of the curve to window coordinates
	VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
	VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
	
	// create a list of selected curves (not including the one clicked on)
	CCurveList curveList;

	if (nAction == CTRK_DRAGGING)
	{
		POSITION position = m_lstTrackItems.GetHeadPosition();
		while(position)
		{
			CTrackItem* pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);

			if(pCurve != pTrackItem)
			{
				if (pCurve->m_fSelected)
				{
					curveList.AddTail(pCurve);
				}
			}
		}
	}

	CWnd wnd;
	wnd.Attach(GetMyWindow());

	// create a tracker to handle the move
	CCurveTracker curveTracker(&wnd, pTimeline, this, (short)nAction, &selRect);
	curveTracker.SetCurve(pTrackItem);
	curveTracker.SetCurveList(&curveList);
	curveTracker.SetXOffset( lXOffset );
	curveTracker.m_offset = 0;

	if(pTrackItem == NULL || curveList.GetCount() > 1)
	{
		OnUpdateDragCurveValue(curveTracker, nAction);
	}
	else
	{
		ShowCurveValueOnStausBar(pTrackItem);
	}


	// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
	// We have to call ReleaseCapture(), or the CurveTracker won't work.
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
	::ReleaseCapture();

	if (curveTracker.Track(&wnd, point, FALSE))
	{
		curveList.AddTail(pTrackItem);

		POSITION listPos = curveList.GetHeadPosition();
		while (listPos != NULL) 
		{
			CTrackItem* pCurve = curveList.GetNext(listPos);
			
			CRect newRect = GetUpdatedCurveRect(curveTracker, pCurve, nAction);

			// Make copies of the curves if required
			if(bMakeCopy)
			{
				CreateCurveCopy(pCurve);
			}

			// Figure out start and end values and positions
			long lStartClock = 0;
			long lEndClock = 0;

			pTimeline->PositionToClocks(newRect.left, &lStartClock);
			pTimeline->PositionToClocks(newRect.right, &lEndClock);

			float fltStartValue = YPosToValue(newRect.top);
			float fltEndValue = YPosToValue(newRect.bottom);

			if (nAction != CTRK_DRAGGING) 
			{
				pCurve->m_mtDuration = lEndClock - lStartClock;
			}

			pCurve->SetStartEndTime(lStartClock, lEndClock);
			pCurve->SetStartEndValue(fltStartValue, fltEndValue);			
		}

		// Redraw CurveStrips
		RefreshCurveStrips();
		RefreshCurvePropertyPage();

		// Let the object know about the changes
		UINT uStrRes = IDS_UNDO_CHANGE_CURVE;
		if(bMakeCopy)
		{
			uStrRes = IDS_UNDO_COPY_CURVE;
		}

		m_pTrackMgr->NotifyDataChange(uStrRes); 
	}

	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTrackMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	if( pIFramework )
	{
		pIFramework->RestoreStatusBar( m_hKeyStatusBar );	
		m_hKeyStatusBar = NULL;
	}
	
	wnd.Detach();
}


void CParamStrip::CreateCurveCopy(CTrackItem* pCurve)
{
	ASSERT(pCurve);
	if(pCurve == NULL)
	{
		return;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	// Make a copy
	CTrackItem* pNewCurve = new CTrackItem(m_pTrackMgr, this);
	pNewCurve->Copy(pCurve);

	// Unselect it and add it to the strip
	pNewCurve->m_fSelected = FALSE;
	m_lstTrackItems.AddTail(pNewCurve);
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetMyWindow
//
// Retrieves the handle of the window which this strip is in.
HWND CParamStrip::GetMyWindow()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return NULL;
	}

	// Create bounding box
	IOleWindow* pIOleWindow = NULL;
	pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
	ASSERT(pIOleWindow);
	
	HWND hWnd;

	VERIFY(pIOleWindow->GetWindow(&hWnd) == S_OK);
	pIOleWindow->Release();
	return hWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::GetCurveFromPoint

CTrackItem*	CParamStrip::GetCurveFromPoint( int nStripXPos, int nStripYPos, BOOL bFrameOnly )
{
	CTrackItem* pTheCurve = NULL;
	BOOL fOnTheCurveHandles = FALSE;

	CTrackItem* pCurve = NULL;
	BOOL fOnCurveHandles = FALSE;
	CRect rect;

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return NULL;
	}


	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	int nAdjustedXPos = AdjustXPos( nStripXPos );

	CPoint point1( nAdjustedXPos, nStripYPos );
	CPoint point2( nAdjustedXPos, nStripYPos );
	CPoint point3( nAdjustedXPos, nStripYPos );

	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);
		CRect rectCurve = bFrameOnly ? pCurve->m_rectFrame : pCurve->m_rectSelect;

		if( rectCurve.PtInRect( point1 )   
		||  rectCurve.PtInRect( point2 ) 
		||  rectCurve.PtInRect( point3 ) )  
		{
			// Need to set fOnCurveHandles
			fOnCurveHandles = FALSE;
			if( pCurve->m_fSelected )
			{
				// Convert the frame rect of the curve to window coordinates
				CRect selRect = pCurve->m_rectFrame;
				VERIFY( SUCCEEDED ( pTimeline->StripToWindowPos( this, &selRect.TopLeft() ) ) );
				VERIFY( SUCCEEDED ( pTimeline->StripToWindowPos( this, &selRect.BottomRight() ) ) );

				// Create a tracker
				CCurveTracker curveTracker( &selRect );
				UINT nHit = HitTest( curveTracker, nStripXPos, nStripYPos );
			
				switch( nHit )
				{
					case CRectTracker::hitTopLeft:
					case CRectTracker::hitBottomRight:
					case CRectTracker::hitTopRight:
					case CRectTracker::hitBottomLeft:
					case CRectTracker::hitTop:
					case CRectTracker::hitBottom:
					case CRectTracker::hitRight:
					case CRectTracker::hitLeft:
						// Cursor is over a RectTracker handle
						fOnCurveHandles = TRUE;
						break;
				}
			}

			if( pTheCurve == NULL )
			{
				pTheCurve = pCurve;
				fOnTheCurveHandles = fOnCurveHandles;
			}
			else
			{
				if( fOnTheCurveHandles == FALSE )
				{
					if( fOnCurveHandles == TRUE )
					{
						// Use the Curve whose handle is being hit
						pTheCurve = pCurve;
						fOnTheCurveHandles = TRUE;
					}
					else
					{
						// Use the Curve with the narrowest width
						if( pCurve->m_rectSelect.Width() < pTheCurve->m_rectSelect.Width() ) 
						{
							pTheCurve = pCurve;
						}
					}
				}
				else
				{
					if( fOnCurveHandles == TRUE )
					{
						// Use the Curve with the narrowest width
						if( pCurve->m_rectSelect.Width() < pTheCurve->m_rectSelect.Width() ) 
						{
							pTheCurve = pCurve;
						}
					}
				}
			}
		}
	}

	return pTheCurve;
}



/////////////////////////////////////////////////////////////////////////////
// CParamStrip::AdjustXPos

int CParamStrip::AdjustXPos( int nStripXPos )
{
	long lNewXPos = 0;

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return NULL;
	}

	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);


	MUSIC_TIME mtTime;

	pTimeline->PositionToClocks( nStripXPos, &mtTime );
	pTimeline->ClocksToPosition( mtTime, &lNewXPos );

	return lNewXPos;
}

//////////////////////////////////////////////////////////////////////////////
// CParamStrip::SelectCurve

WORD CParamStrip::SelectCurve( CTrackItem* pTrackItem, BOOL fState )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return 0;
	}
	
	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return NULL;
	}


	WORD wRefreshUI = FALSE;

	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		CTrackItem* pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);
		if( pCurve == pTrackItem)
		{
			if( pTrackItem->m_fSelected != fState )
			{
				pTrackItem->m_fSelected = fState;
				if(fState)
				{
					long lDurMeasure = 0;
					long lDurBeat = 0;
					long lDurTick = 0;

					MUSIC_TIME mtDuration = pCurve->m_mtEndTime - pCurve->m_mtStartTime;
					m_pTrackMgr->ClocksToMeasureBeatTick(mtDuration, &lDurMeasure, &lDurBeat, &lDurTick);

					long lPosition = 0;
					pTimeline->ClocksToPosition(pCurve->m_mtStartTime, &lPosition);
					m_pSelectedRegions->AddRegion(lPosition, lDurMeasure, lDurBeat);
				}
				else
				{
					long lPosition = 0;
					pTimeline->ClocksToPosition(pCurve->m_mtStartTime, &lPosition);
					m_pSelectedRegions->ToggleRegion(lPosition);
				}

				wRefreshUI = TRUE;
			}
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CParamStrip::SelectAllCurves

WORD CParamStrip::SelectAllCurves( BOOL fState )
{
	WORD wRefreshUI = FALSE;

	POSITION position = m_lstTrackItems.GetHeadPosition();
	while(position)
	{
		CTrackItem* pCurve = (CTrackItem*)m_lstTrackItems.GetNext(position);
		if( pCurve->m_fSelected != fState )
		{
			pCurve->m_fSelected = fState;
			wRefreshUI = TRUE;
		}
	}

	return wRefreshUI;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::HitTest

UINT CParamStrip::HitTest( CCurveTracker& tracker, long lStripXPos, long lStripYPos )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return NULL;
	}

	// Adjust X pos (for repeating Parts)
	int nAdjustedXPos = AdjustXPos( lStripXPos );

	CPoint point( nAdjustedXPos, lStripYPos );
	VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &point)));

	// Try adjusted point
	UINT hit = tracker.HitTest( point );

	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	return hit;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::RefreshUI
//
// Refreshes the curve strip if wRefreshUI is true.  Sets wRefreshUI to
// false.
void CParamStrip::RefreshUI(WORD& wRefreshUI)
{
	if( wRefreshUI )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
		wRefreshUI = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::RefreshCurvePropertyPage	

void CParamStrip::RefreshCurvePropertyPage( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return;
	}

	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	ASSERT(pIFramework);
	if(pIFramework == NULL)
	{
		return; 
	}

	IDMUSProdStrip* pIActiveStrip = NULL;
	VARIANT var;

	// Get the active Strip
	if( SUCCEEDED ( pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
	{
		pIActiveStrip = (IDMUSProdStrip *)V_UNKNOWN( &var );
	}
	else
	{
		pIActiveStrip = NULL;
	}

	if( pIFramework )
	{
		IDMUSProdPropSheet* pIPropSheet = NULL;

		// Get IDMUSProdPropSheet interface
		if( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				if( (pIActiveStrip == this)
				||  (pIPropSheet->IsEqualPageManagerObject(this) == S_OK) )
				{
					m_fShowItemProps = true;
					OnShowProperties();
				}
			}
			
			pIPropSheet->Release();
		}
	}

	if( pIActiveStrip )
	{
		pIActiveStrip->Release();
	}

}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::RefreshCurveStrips
	
void CParamStrip::RefreshCurveStrips( void )
{
	ASSERT(m_pTrackObject);
	if(m_pTrackObject)
	{
		m_pTrackObject->RefreshAllStrips();
	}
}



CTrackMgr* CParamStrip::GetTrackManager()
{
	return m_pTrackMgr;
}

CTrackObject*	CParamStrip::GetTrackObject()
{
	return m_pTrackObject;
}

HRESULT	CParamStrip::GetTimeSignature(DirectMusicTimeSig* pTimeSig, MUSIC_TIME mtTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(pTimeSig);
	if(pTimeSig == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	long lMeasure = 0;
	long lBeat = 0;
	long lGrid = 0;
	long lTick = 0;
	if(FAILED(m_pTrackMgr->ClocksToMeasureBeatGridTick(mtTime, &lMeasure, &lBeat, &lGrid, &lTick)))
	{
		return E_FAIL;
	}

	DMUS_TIMESIGNATURE TimeSig;
	DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
	if(FAILED(pTimeline->GetParam(GUID_TimeSignature, dwGroupBits, 0, lMeasure, NULL, &TimeSig)))
	{
		return E_FAIL;
	}

	pTimeSig->m_wGridsPerBeat = TimeSig.wGridsPerBeat;
	pTimeSig->m_bBeat = TimeSig.bBeat;
	pTimeSig->m_bBeatsPerMeasure = TimeSig.bBeatsPerMeasure;

	return S_OK;
}

void CParamStrip::OnCurvePropPageMgrDeleted()
{
	if(m_pPropPageMgr)
	{
		m_pPropPageMgr = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::OnSetCursor
//
// Sets the mouse cursor based on where the mouse is relative to the curves.
HRESULT CParamStrip::OnSetCursor( long lXPos, long lYPos)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	if( m_StripUI.m_nStripView == SV_MINIMIZED )
	{
		m_hCursor = LoadCursor(NULL, IDC_ARROW);
		return S_OK;
	}

	CTrackItem* pCurve = GetCurveFromPoint( lXPos, lYPos, TRUE );

	if (pCurve)
	{
		if (pCurve->m_fSelected)
		{
			// convert the frame rect of the curve to window coordinates
			CRect selRect = pCurve->m_rectFrame;
			VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
			VERIFY(SUCCEEDED(pTimeline->StripToWindowPos(this, &selRect.BottomRight())));

			// create a tracker
			CCurveTracker curveTracker(&selRect);
			UINT hit = HitTest( curveTracker, lXPos, lYPos );
		
			// I had to do this because CRectTracker.LoadCursor doesn't return an HCURSOR
			switch(hit)
			{
				case CRectTracker::hitTopLeft:
				case CRectTracker::hitBottomRight:
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
					break;
				}

				case CRectTracker::hitTopRight:
				case CRectTracker::hitBottomLeft:
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
					break;
				}

				case CRectTracker::hitTop:
				case CRectTracker::hitBottom:
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENS);
					break;
				}

				case CRectTracker::hitRight:
				case CRectTracker::hitLeft:
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
					break;
				}

				case CRectTracker::hitMiddle:
				{

					m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
					break;
				}

				default:
				{
					m_hCursor = LoadCursor(NULL, IDC_ARROW);
					break;
				}
			}
		}
		else
		{
			m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
		}
	}
	else
	{
		m_hCursor = LoadCursor(NULL, IDC_ARROW);
	}

	return S_OK;
}

HRESULT	CParamStrip::GetParamInfoFromCLSID(MP_PARAMINFO* pParamInfo)
{
	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(pParamInfo);
	if(pParamInfo == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;
	IUnknown* pIUnkObject = NULL;
	CLSID clsidObject = m_pTrackObject->m_guidObject;
	if(SUCCEEDED(hr = ::CoCreateInstance(clsidObject, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**) &pIUnkObject)))
	{
		IMediaParamInfo* pIParamInfo = NULL;
		if(SUCCEEDED(hr = pIUnkObject->QueryInterface(IID_IMediaParamInfo, (void**)&pIParamInfo)))
		{
			hr = pIParamInfo->GetParamInfo(m_dwParamIndex, pParamInfo);
			pIParamInfo->Release();
		}

		pIUnkObject->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::UseCenterLine

BOOL CParamStrip::UseCenterLine( void )
{
	/*if(m_ParamInfo.mpdMaxValue > 0 && m_ParamInfo.mpdMinValue < 0)
	{
		return TRUE;
	}*/

    return FALSE;
}


DWORD CParamStrip::GetParamIndex()
{
	return m_dwParamIndex;
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SwitchTimeBase

void CParamStrip::SwitchTimeBase( void )
{
	POSITION pos = m_lstTrackItems.GetHeadPosition();
	while( pos )
	{
		CTrackItem* pItem = m_lstTrackItems.GetNext( pos );
		pItem->SwitchTimeBase();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::InvalidateFBar

void CParamStrip::InvalidateFBar( void )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	// Invalidate the function bar
	RECT rect;
	rect.right = 0;
	rect.top = m_StripUI.m_lVerticalScrollYPos;
	rect.bottom = m_StripUI.m_lVerticalScrollYPos + m_StripUI.m_lStripHeight;
	VARIANT var;
	var.vt = VT_I4;
	if( FAILED ( pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
	{
		return;
	}
	rect.left = -V_I4(&var);

	// Need to offset it if we're horizontally scrolled
	long lLeftDisplay;
	pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
	pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

	rect.left += lLeftDisplay;
	rect.right += lLeftDisplay;

	// Really invalidate the function bar
	pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, &rect, FALSE );
}

DWORD CParamStrip::GetDMParamFlags()
{
	return m_dwParamFlagsDM;
}

void CParamStrip::SetDMParamFlags(DWORD dwFlagsDM)
{
	m_dwParamFlagsDM = dwFlagsDM;
}

IDMUSProdPropPageManager* CParamStrip::GetPropPageMgr()
{
	return m_pPropPageMgr;
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::DoGridsPerSecond

void CParamStrip::DoGridsPerSecond( void )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	ASSERT(m_pTrackObject);
	if(m_pTrackObject == NULL)
	{
		return;
	}

	CGridsPerSecondDlg dlgGridsPerSecond;

	dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond = m_StripUI.m_nSnapToRefTime_GridsPerSecond;

	if( dlgGridsPerSecond.DoModal() == IDOK )
	{
		if( dlgGridsPerSecond.m_fAllParts )
		{
			if( m_pTrackObject->AllStrips_SetNbrGridsPerSecond(dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond) )
			{
				m_pTrackMgr->SetLastEdit(IDS_UNDO_NBR_GRIDS_PER_SECOND);
				m_pTrackMgr->TrackDataChanged();
				m_pTrackObject->AllStrips_Invalidate();
			}
		}
		else
		{
			if( m_StripUI.m_nSnapToRefTime_GridsPerSecond != dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond )
			{
				m_StripUI.m_nSnapToRefTime_GridsPerSecond = dlgGridsPerSecond.m_nSnapToRefTime_GridsPerSecond;
				m_pTrackMgr->SetLastEdit(IDS_UNDO_NBR_GRIDS_PER_SECOND);
				m_pTrackMgr->TrackDataChanged();
				pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
			}
		}

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SetSnapTo

void CParamStrip::SetSnapTo( WORD wID )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

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

	// Save undo state and redraw our strip
	m_pTrackMgr->SetLastEdit(IDS_UNDO_SNAP_TO);
	m_pTrackMgr->TrackDataChanged();
	pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SnapXPosToUnknownTime

HRESULT CParamStrip::SnapXPosToUnknownTime(long lXPos, REFERENCE_TIME* prtSnapTime)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	if(prtSnapTime == NULL)
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	*prtSnapTime = 0;

	if(lXPos < 0)
	{
		ASSERT(0);
		return E_INVALIDARG;
	}

	MUSIC_TIME mtTime = 0;
	if(FAILED(m_pTrackMgr->m_pTimeline->PositionToClocks(lXPos, &mtTime)))
	{
		ASSERT(0);
		return E_FAIL;
	}

	REFERENCE_TIME rtTime = 0;
	if(FAILED(m_pTrackMgr->ClocksToUnknownTime(mtTime, &rtTime)))
	{
		ASSERT(0);
		return E_FAIL;
	}

	return SnapUnknownTime(rtTime, prtSnapTime);
}


/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SnapUnknownTime

HRESULT CParamStrip::SnapUnknownTime(REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime)
{
	if(prtSnapTime == NULL)
	{
		ASSERT(0);
		return E_INVALIDARG;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	*prtSnapTime = 0;

	if(rtTime < 0)
	{
		ASSERT(0);
		return E_INVALIDARG;
	}

	if(m_pTrackMgr->IsRefTimeTrack())
	{
		long lMinute;
		long lSec;
		long lGrid;
		long lMs;
		m_pTrackMgr->RefTimeToMinSecGridMs(this, rtTime, &lMinute, &lSec, &lGrid, &lMs);

		if(m_StripUI.m_nSnapToRefTime != IDM_SNAP_NONE)
		{
			if(lMs < 0)
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

BOOL CParamStrip::IsOneGrid()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return FALSE;
	}

	if(m_pTrackMgr->IsRefTimeTrack() )
	{
		if(m_StripUI.m_nSnapToRefTime_GridsPerSecond == 1 )
		{
			return TRUE;
		}
	}

	return FALSE;

}

void CParamStrip::ShowCurveValueOnStausBar(CTrackItem* pItem)
{
	ASSERT(pItem);
	if(pItem == NULL)
	{
		return;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}
	
	// create the status bar string
	CString strStart;
	CString strEnd;
	CString strStatus;

	strStart.LoadString( IDS_START_TEXT );
	strEnd.LoadString( IDS_END_TEXT );

	float fltStart = 0;
	float fltEnd = 0;
	pItem->GetStartEndValue(fltStart, fltEnd);

	strStatus.Format("%s %.03f, %s %.03f", strStart, fltStart, strEnd, fltEnd);
	
	// Refresh status bar
	IDMUSProdFramework* pIFramework = m_pTrackMgr->GetFramework();
	if(pIFramework)
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}

}

/////////////////////////////////////////////////////////////////////////////
// CParamStrip::SnapUnknownToNextIncrement

HRESULT CParamStrip::SnapUnknownToNextIncrement( REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	SnapUnknownTime(rtTime, &rtTime);

	if(m_pTrackMgr->IsRefTimeTrack())
	{
		long lMinute = 0;
		long lSecond = 0;
		long lGrid = 0;
		long lMs = 0;
	
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
// CParamStrip::CListSelectedRegion_Contains

bool CParamStrip::CListSelectedRegion_Contains( REFERENCE_TIME rtTime )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return false;
	}

	ASSERT(m_pSelectedRegions);
	if(m_pSelectedRegions == NULL)
	{
		return false;
	}

	MUSIC_TIME mtTime;
	m_pTrackMgr->UnknownTimeToClocks( rtTime, &mtTime );

	if( m_pSelectedRegions->Contains( mtTime ) )
	{
		return true;
	}

	return false;
}
