// CurveStrip.cpp : Implementation of CCurveStrip
#include "stdafx.h"
#include "SequenceStripMgr.h"
#include "SequenceMgr.h"
#include "CurveStrip.h"
#include "SequenceStripMgrApp.h"
#include "PropCurve.h"
#include "CurvePropPageMgr.h"
#include "Tracker.h"
#include "DLLJazzDataObject.h"
#include "RiffStrm.h"
#include "SharedPianoRoll.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {E7380900-DAB5-11d1-89B1-00C04FD912C8}
static const GUID GUID_CurvePropPageManager = 
{ 0xe7380900, 0xdab5, 0x11d1, { 0x89, 0xb1, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8 } };


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip constructor/destructor

CCurveStrip::CCurveStrip( CSequenceMgr* pSequenceMgr, CSequenceStrip* pSequenceStrip, BYTE bCCType, WORD wRPNType )
{
	ASSERT( pSequenceMgr != NULL );
	ASSERT( pSequenceStrip != NULL );

	// Initialize our reference count
	m_dwRef = 0;
	AddRef();

	m_pSequenceMgr = pSequenceMgr;
	m_pStripMgr = (IDMUSProdStripMgr *)pSequenceMgr;
	m_pStripMgr->AddRef();

	m_pSequenceStrip = pSequenceStrip;
	// m_pSequenceStrip->AddRef();		intentionally missing

	m_bCCType = bCCType;
	m_wRPNType = wRPNType;

	m_hCursor = m_pSequenceStrip->GetArrowCursor();
	m_hKeyStatusBar = NULL;

	m_fInsertingCurve = INSERT_CURVE_FALSE;
	m_nInsertingStartValue = 0;
	m_nInsertingEndValue = 0;
	m_lInsertingStartClock = 0;
	m_lInsertingEndClock = 0;

	m_pCCurveTracker = NULL;
	m_nTrackerAction = 0;
}

CCurveStrip::~CCurveStrip()
{
	// Clean up Property Sheet
	if( m_pSequenceMgr && m_pSequenceMgr->m_pDMProdFramework )
	{
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( m_pSequenceMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RemovePageManagerByObject( this );
			pIPropSheet->Release();
		}
	}

	// Release objects
	if( m_pStripMgr )
	{
		m_pStripMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::QueryInterface

STDMETHODIMP CCurveStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( ppv != NULL );
	if( ppv == NULL )
	{
		return E_POINTER;
	}

    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )
	||  IsEqualIID( riid, IID_IDMUSProdStrip ) )
	{
        *ppv = (IDMUSProdStrip *)this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdStripFunctionBar ) )
	{
        *ppv = (IDMUSProdStripFunctionBar *)this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdTimelineEdit ) )
	{
		*ppv = (IDMUSProdTimelineEdit *)this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdPropPageObject ) )
	{
        *ppv = (IDMUSProdPropPageObject *)this;
	}
	else
	{
		return E_NOINTERFACE;
	}

    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::AddRef

STDMETHODIMP_(ULONG) CCurveStrip::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Release

STDMETHODIMP_(ULONG) CCurveStrip::Release(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( m_dwRef != 0 );

	--m_dwRef;

	if( m_dwRef == 0 )
	{
		delete this;
		return 0;
	}
	else
	{
		return m_dwRef;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip Helper methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::RefreshUI
//
// Refreshes the curve strip if wRefreshUI is true.  Sets wRefreshUI to
// false.
void CCurveStrip::RefreshUI(WORD& wRefreshUI)
{
	if( wRefreshUI )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
		wRefreshUI = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetMyWindow
//
// Retrieves the handle of the window which this strip is in.
HWND CCurveStrip::GetMyWindow()
{
	// Create bounding box
	IOleWindow* pIOleWindow;
	m_pSequenceMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
	ASSERT(pIOleWindow);
	
	HWND hWnd;

	VERIFY(pIOleWindow->GetWindow(&hWnd) == S_OK);
	pIOleWindow->Release();
	return hWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetStripRect
//
BOOL CCurveStrip::GetStripRect(LPRECT pRectStrip)
{
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = pRectStrip;
	if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
	{
		return FALSE;
	}

	// adjust the strip rect to encompass the WHOLE strip, not only the part
	// that is visible on the screen
	long lHeight;
	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		lHeight = CRV_MINIMIZE_HEIGHT;
	}
	else
	{
		if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
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
	
	m_pSequenceMgr->m_pTimeline->StripToWindowPos( this, &ptTop );
	m_pSequenceMgr->m_pTimeline->StripToWindowPos( this, &ptBottom );

	pRectStrip->top = ptTop.y;
	pRectStrip->bottom = ptBottom.y;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::IsCurveForThisStrip

BOOL CCurveStrip::IsCurveForThisStrip( CCurveItem* pDMCurve )
{
	ASSERT( m_pSequenceStrip != NULL );

	// Make sure Curve should be displayed in this strip
	if( m_pSequenceStrip->CurveTypeToStripCCType(pDMCurve) == m_bCCType )
	{
		// If we are an RPN or NRPN, check that the wParamType matches
		if( ((m_bCCType == CCTYPE_RPN_CURVE_STRIP)
			 || (m_bCCType == CCTYPE_NRPN_CURVE_STRIP))
		&&	(pDMCurve->m_wParamType != m_wRPNType) )
		{
			return FALSE;
		}

		// Make sure curve is within segment
		if( pDMCurve->m_mtTime < m_pSequenceStrip->m_mtLength )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::IsAnyCurveSelected

BOOL CCurveStrip::IsAnyCurveSelected( void )
{
	ASSERT( m_pSequenceMgr != NULL );

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( pItem->m_fSelected && IsCurveForThisStrip( pItem ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::RefreshCurvePropertyPage	

void CCurveStrip::RefreshCurvePropertyPage( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pSequenceMgr
	&&  m_pSequenceMgr->m_pTimeline
	&&	m_pSequenceMgr->m_pDMProdFramework )
	{
		IDMUSProdStrip* pIActiveStrip;
		VARIANT var;

		// Get the active Strip
		if( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			pIActiveStrip = (IDMUSProdStrip *)V_UNKNOWN( &var );
		}
		else
		{
			pIActiveStrip = NULL;
		}

		IDMUSProdPropSheet* pIPropSheet;

		// Get IDMUSProdPropSheet interface
		if( SUCCEEDED ( m_pSequenceMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				if( (pIActiveStrip == this)
				||  (pIPropSheet->IsEqualPageManagerObject(this) == S_OK) )
				{
					OnShowProperties();
				}
			}
			
			pIPropSheet->Release();
		}

		if( pIActiveStrip )
		{
			pIActiveStrip->Release();
		}

		m_pSequenceMgr->UpdateStatusBarDisplay();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::RefreshCurveStrips
	
void CCurveStrip::RefreshCurveStrips( void )
{
	ASSERT( m_pSequenceStrip != NULL );

	if( m_pSequenceMgr
	&&  m_pSequenceMgr->m_pTimeline )
	{
		POSITION pos2 = m_pSequenceStrip->m_lstCurveStrips.GetHeadPosition();
		while( pos2 )
		{
			CCurveStrip* pCurveStrip = m_pSequenceStrip->m_lstCurveStrips.GetNext( pos2 );

			if( pCurveStrip->m_bCCType == m_bCCType )
			{
				m_pSequenceMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, TRUE );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip Drawing methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::UseCenterLine

BOOL CCurveStrip::UseCenterLine( void )
{
    if( m_bCCType == CCTYPE_PAN_CURVE_STRIP
    ||  m_bCCType == CCTYPE_PB_CURVE_STRIP )
    {
		// TRUE for PitchBend and Pan, which are drawn from the center
    	return TRUE;
    }

    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DrawCurve

void CCurveStrip::DrawCurve( CCurveItem* pDMCurve, HDC hDC,
							 RECT* pRectStrip, LONG lXOffset )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	ASSERT( m_pSequenceStrip != NULL );

	COLORREF crNoteColor = ::GetNearestColor( hDC, m_pSequenceStrip->m_crUnselectedNoteColor );
	COLORREF crSelectedNoteColor = ::GetNearestColor( hDC, m_pSequenceStrip->m_crSelectedNoteColor );

	DWORD dwLastValue = 0xFFFFFFFF;
	DWORD dwValue;
	long lCurveStartClock;
	VARIANT var;
	CRect rect;

	lCurveStartClock  = pDMCurve->AbsTime();

	// Get Strip height and center
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}
	int nStripCenter = pRectStrip->top + (nStripHeight >> 1);

	// Get MUSIC_TIME increment
	MUSIC_TIME mtIncrement;
	m_pSequenceMgr->m_pTimeline->PositionToClocks( 1, &mtIncrement );

	// Ensure mtIncrement is at least 1
	mtIncrement = max( 1, mtIncrement );

	// Initialize rectFrame and rectSelect
	ZeroMemory( &pDMCurve->m_rectFrame, sizeof(RECT) );
	ZeroMemory( &pDMCurve->m_rectSelect, sizeof(RECT) );

	pDMCurve->m_mtCurrent = 0;
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lCurveStartClock + pDMCurve->m_mtCurrent, &rect.right );

	BOOL fFirstTime = TRUE;

	// Draw the curve
	while( pDMCurve->m_mtCurrent <= pDMCurve->m_mtDuration )
	{
		// Compute value of curve at m_mtCurrent
		dwValue = pDMCurve->ComputeCurve( NULL );
		if( (m_bCCType != CCTYPE_PB_CURVE_STRIP)
		&&	(m_bCCType != CCTYPE_RPN_CURVE_STRIP)
		&&	(m_bCCType != CCTYPE_NRPN_CURVE_STRIP))
		{
			dwValue = dwValue << 7;
		}
		dwValue = 16384 - dwValue;

		// Increment time offset into Curve
		pDMCurve->m_mtCurrent += mtIncrement;

		if( dwValue != dwLastValue )
		{
			dwLastValue = dwValue;

			// Set rect left
			rect.left = rect.right;

			// Set rect right
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( lCurveStartClock + pDMCurve->m_mtCurrent, &rect.right );

			// Set rect top and bottom
			dwValue *= (DWORD)nStripHeight;
			dwValue /= 16384;
			if( UseCenterLine() )
			{
				// Draw Curves around central axis
				if( dwValue > (DWORD)nStripCenter )
				{
					rect.top = nStripCenter;
					rect.bottom = dwValue;
				}
				else
				{
					rect.top = dwValue;
					rect.bottom = nStripCenter;
				}
			}
			else
			{
				rect.top = dwValue;
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
			if( pDMCurve->m_fSelected )
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
				pDMCurve->m_rectSelect = rect;
			}
			else
			{
				pDMCurve->m_rectSelect.right = rect.right;
				pDMCurve->m_rectSelect.top = min( pDMCurve->m_rectSelect.top, rect.top );
				pDMCurve->m_rectSelect.bottom = max( pDMCurve->m_rectSelect.bottom, rect.bottom );
			}
		}
	}

	// Set rectFrame
	pDMCurve->m_rectFrame.left = pDMCurve->m_rectSelect.left;
	pDMCurve->m_rectFrame.right = pDMCurve->m_rectSelect.right;
	if( pDMCurve->m_nStartValue < pDMCurve->m_nEndValue )
	{
		pDMCurve->m_rectFrame.top = ValueToYPos( pDMCurve->m_nEndValue );
		pDMCurve->m_rectFrame.bottom = ValueToYPos( pDMCurve->m_nStartValue );
	}
	else
	{
		pDMCurve->m_rectFrame.top = ValueToYPos( pDMCurve->m_nStartValue );
		pDMCurve->m_rectFrame.bottom = ValueToYPos( pDMCurve->m_nEndValue );
	}

	// Enforce minimum size for rectSelect
	if( (pDMCurve->m_rectSelect.right - pDMCurve->m_rectSelect.left) < CURVE_MIN_SIZE )
	{
		pDMCurve->m_rectSelect.right = pDMCurve->m_rectSelect.left + CURVE_MIN_SIZE;
	}
	if( (pDMCurve->m_rectSelect.bottom - pDMCurve->m_rectSelect.top) < CURVE_MIN_SIZE )
	{
		if( pDMCurve->m_rectSelect.bottom >= nStripCenter )
		{
			pDMCurve->m_rectSelect.top = pDMCurve->m_rectSelect.bottom - CURVE_MIN_SIZE;
		}
		else
		{
			pDMCurve->m_rectSelect.bottom = pDMCurve->m_rectSelect.top + CURVE_MIN_SIZE;
		}
	}

	// Enforce minimum height for rectFrame
	if( (pDMCurve->m_rectFrame.bottom - pDMCurve->m_rectFrame.top) < CURVE_MIN_SIZE )
	{
		if( pDMCurve->m_rectFrame.bottom >= nStripCenter )
		{
			pDMCurve->m_rectFrame.top = pDMCurve->m_rectFrame.bottom - CURVE_MIN_SIZE;
		}
		else
		{
			pDMCurve->m_rectFrame.bottom = pDMCurve->m_rectFrame.top + CURVE_MIN_SIZE;
		}
	}

	pDMCurve->m_rectFrame.left -= lXOffset;
	pDMCurve->m_rectFrame.right -= lXOffset;

	::FrameRect( hDC, &pDMCurve->m_rectFrame, (HBRUSH)GetStockObject(DKGRAY_BRUSH) );

	pDMCurve->m_rectFrame.left += lXOffset;
	pDMCurve->m_rectFrame.right += lXOffset;
	
	// draw selection handles if curve is selected
	if( pDMCurve->m_fSelected )
	{
		CRect selRect = pDMCurve->m_rectFrame;
		VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
		VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
		
		CCurveTracker curveTracker(&selRect);
		curveTracker.Draw( CDC::FromHandle(hDC) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DrawMaximizedStrip

HRESULT CCurveStrip::DrawMaximizedStrip( HDC hDC, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	ASSERT( m_pSequenceStrip != NULL );

	VARIANT var;

	// Store strip height
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}
	ioCurveStripState* pCurveStripState = m_pSequenceStrip->GetCurveStripState( m_bCCType, m_wRPNType );
	if( pCurveStripState )
	{
		pCurveStripState->m_nStripHeight = nStripHeight;
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
	long lPartClockLength = CalcPartClockLength();
	long lStripStartClock, lStripEndClock;
	long lTimelineClockLength;
	long lPartMaxXPos;
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lPartClockLength, &lPartMaxXPos );

	m_pSequenceMgr->m_pTimeline->PositionToClocks( pRectClip->left + lXOffset, &lStripStartClock );
	m_pSequenceMgr->m_pTimeline->PositionToClocks( pRectClip->right + lXOffset, &lStripEndClock );
	m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	lTimelineClockLength = V_I4(&var);
	lStripEndClock = min( lStripEndClock, lTimelineClockLength );
	lStripStartClock = min( lStripStartClock, lStripEndClock - 1 );

	CRect rect;

	CTypedPtrList<CPtrList, CRect*> lstRects;

	long lCurveStartClock, lCurveEndClock;
	CRect rectFrame;
	CRect rectSelect;

	BOOL fHaveSelected = FALSE;

	// DRAW UNSELECTED CURVES
	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pCurveItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( pCurveItem->m_fSelected )
		{
			if( fHaveSelected == FALSE )
			{
				if( IsCurveForThisStrip( pCurveItem ) )
				{
					fHaveSelected = TRUE;
				}
			}
		}
		else
		{
			if( IsCurveForThisStrip( pCurveItem ) )
			{
				// Make sure Curve is showing
				lCurveStartClock  = pCurveItem->AbsTime();
				lCurveEndClock = lCurveStartClock + pCurveItem->m_mtDuration;

				rectFrame = pCurveItem->m_rectFrame;
				rectSelect = pCurveItem->m_rectSelect;

				if( lCurveStartClock <= lStripEndClock )
				{
					if( lCurveEndClock >= lStripStartClock )
					{
						// Draw this Curve
						DrawCurve( pCurveItem, hDC, pRectStrip, lXOffset );
						rectFrame = pCurveItem->m_rectFrame;
						rectSelect = pCurveItem->m_rectSelect;

						// Frame intersecting rectangles
						POSITION posRect = lstRects.GetHeadPosition();
						while( posRect )
						{
							CRect *pRectList = lstRects.GetNext( posRect );

							// Compute intersection with other Curves
							rect.IntersectRect( pRectList, &pCurveItem->m_rectSelect );
											
							// Draw it
							if( rect.left != rect.right
							||  rect.top != rect.bottom )
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
						CRect *pRect = new CRect( &pCurveItem->m_rectSelect );
						if( pRect )
						{
							lstRects.AddHead( pRect );
						}
					}
				}

				// Set Curve rectFrame and rectSelect
				pCurveItem->m_rectFrame = rectFrame;
				pCurveItem->m_rectSelect = rectSelect;
			}
		}
	}

	// DRAW SELECTED CURVES
	if( fHaveSelected )
	{
		POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
		while( pos )
		{
			CCurveItem *pCurveItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );
			if( pCurveItem->m_fSelected )
			{
				if( IsCurveForThisStrip( pCurveItem ) )
				{
					// Make sure Curve is showing
					lCurveStartClock  = pCurveItem->AbsTime();
					lCurveEndClock = lCurveStartClock + pCurveItem->m_mtDuration;

					rectFrame = pCurveItem->m_rectFrame;
					rectSelect = pCurveItem->m_rectSelect;

					if( lCurveStartClock <= lStripEndClock )
					{
						if( lCurveEndClock >= lStripStartClock )
						{
							// Draw this Curve
							DrawCurve( pCurveItem, hDC, pRectStrip, lXOffset );
							rectFrame = pCurveItem->m_rectFrame;
							rectSelect = pCurveItem->m_rectSelect;

							// Frame intersecting rectangles
							POSITION posRect = lstRects.GetHeadPosition();
							while( posRect )
							{
								CRect *pRectList = lstRects.GetNext( posRect );

								// Compute intersection with other Curves
								rect.IntersectRect( pRectList, &pCurveItem->m_rectSelect );
												
								// Draw it
								if( rect.left != rect.right
								||  rect.top != rect.bottom )
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
							CRect *pRect = new CRect( &pCurveItem->m_rectSelect );
							if( pRect )
							{
								lstRects.AddTail( pRect );
							}
						}
					}

					// Set Curve rectFrame and rectSelect
					pCurveItem->m_rectFrame = rectFrame;
					pCurveItem->m_rectSelect = rectSelect;
				}
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
		delete lstRects.RemoveHead();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DrawMinimizedStrip

HRESULT CCurveStrip::DrawMinimizedStrip( HDC hDC, RECT* pRectStrip )
{
	ioCurveStripState* pCurveStripState;
	CString strCCText;
	CString strText;
	CString strTemp;

	// Format text depicting type of curves in this Part
	if( m_pSequenceStrip->m_lstCurveStripStates.IsEmpty() )
	{
		strText.LoadString( IDS_NO_CC_TRACKS_TEXT );
	}
	else
	{
		POSITION pos = m_pSequenceStrip->m_lstCurveStripStates.GetHeadPosition();
		while( pos )
		{
			pCurveStripState = m_pSequenceStrip->m_lstCurveStripStates.GetNext( pos );

			// If this is an RPN from 0-4, use a user-friendly name
			if( (pCurveStripState->m_bCCType == CCTYPE_RPN_CURVE_STRIP)
			&&	(pCurveStripState->m_wRPNType < 5) )
			{
				strTemp.LoadString( pCurveStripState->m_wRPNType + IDS_RPN_PITCHBEND );
				AfxExtractSubString( strCCText, strTemp, 1, '\n' );
			}
			else 
			{
				// Load the text to display for this CC type
				strTemp.LoadString( pCurveStripState->m_bCCType + IDS_ControlChange0 );
				AfxExtractSubString( strCCText, strTemp, 1, '\n' );

				// If this is an NRPN or an RPN greater than 4, use a generic name
				if( (pCurveStripState->m_bCCType == CCTYPE_NRPN_CURVE_STRIP)
				||	(pCurveStripState->m_bCCType == CCTYPE_RPN_CURVE_STRIP) )
				{
					strTemp = strCCText;
					strCCText.Format( strTemp, int(pCurveStripState->m_wRPNType) );
				}
			}

			if( strText.IsEmpty() == FALSE )
			{
				strText += ", ";
			}
			strText += strCCText; 
		}
	}

	// Set font
	HFONT pFontOld = NULL;

	CFont fontCurveStrip;

	CString strFontName;
	if( !strFontName.LoadString( IDS_CURVE_STRIP_FONTNAME ) )
	{
		strFontName = _T("Arial");
	}
	if( fontCurveStrip.CreatePointFont( 80, strFontName, CDC::FromHandle(hDC) ) != FALSE )
	{
		pFontOld = (HFONT)::SelectObject( hDC, fontCurveStrip.GetSafeHandle() );
	}

	// Draw text depicting type of curves in this Part
	pRectStrip->left = 4;
	::DrawText( hDC, strText, -1, pRectStrip, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	if( pFontOld )
	{
		::SelectObject( hDC, pFontOld );
	}

	fontCurveStrip.DeleteObject();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::InvertGutterRange

void CCurveStrip::InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset )
{
	// Highlight the selected range (if there is one)
	if( m_pSequenceStrip->m_bGutterSelected
	&&  m_pSequenceStrip->m_lBeginSelect != m_pSequenceStrip->m_lEndSelect )
	{
		if( m_pSequenceMgr->m_pTimeline )
		{
			RECT rectHighlight;

			rectHighlight.top = pRectClip->top;
			rectHighlight.bottom = pRectClip->bottom;

			m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_pSequenceStrip->m_lBeginSelect > m_pSequenceStrip->m_lEndSelect ?
													   m_pSequenceStrip->m_lEndSelect : m_pSequenceStrip->m_lBeginSelect,
													   &rectHighlight.left );

			m_pSequenceMgr->m_pTimeline->ClocksToPosition( m_pSequenceStrip->m_lBeginSelect > m_pSequenceStrip->m_lEndSelect ?
													   m_pSequenceStrip->m_lBeginSelect : m_pSequenceStrip->m_lEndSelect,
													   &rectHighlight.right );

			rectHighlight.left -= lXOffset;
			rectHighlight.right -= lXOffset;

			GrayOutRect( hDC, &rectHighlight );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Draw

HRESULT	STDMETHODCALLTYPE CCurveStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	ASSERT( m_pSequenceStrip != NULL );
	
	if( hDC == NULL )
	{
		return E_INVALIDARG;
	}

	// Needed to prevent unsteady rhythm and synth breakups
	Sleep(2);

	// Get Function Bar rectangle
	VARIANT var;
	RECT rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
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
	m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

	::OffsetRect( &rectStrip, -rectFBar.right, -rectStrip.top );
	::OffsetRect( &rectStrip, lLeftPosition, 0 );

	// Get clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );
//	TRACE( "%i %i %i %i\n", rectClip.top, rectClip.bottom, rectClip.left, rectClip.right );

	// DRAW CURVE STRIP
	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		DrawMinimizedStrip( hDC, &rectStrip );
		InvertGutterRange( hDC, &rectClip, lXOffset );
	}
	else
	{
		m_pSequenceMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT_GRID, m_pSequenceMgr->m_dwGroupBits, 0, lXOffset );
		InvertGutterRange( hDC, &rectClip, lXOffset );
		DrawMaximizedStrip( hDC, &rectStrip, &rectClip, lXOffset );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CCurveStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceStrip != NULL );

	if( pvar == NULL )
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
			   *pCLSID = CLSID_DirectMusicSeqTrack;
			}
			else
			{
				return E_FAIL;
			}
			break;

		case SP_CURVESTRIP:
			// We are a curve strip
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_RESIZEABLE:
			// We are resizable
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_GUTTERSELECTABLE:
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_MINMAXABLE:
			// We support Minimize/Maximize
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_DEFAULTHEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = CRV_DEFAULT_HEIGHT;
			break;

		case SP_MAXHEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = CRV_MAXHEIGHT_HEIGHT;
			break;

		case SP_MINIMIZE_HEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = CRV_MINIMIZE_HEIGHT;
			break;

		case SP_CURSOR_HANDLE:
			pvar->vt = VT_I4;
			V_I4(pvar) = (int) m_hCursor;
			break;

		case SP_NAME:
		{
			BSTR bstrName;
			CString strName;
				
			strName.LoadString( IDS_CC_TEXT );
			if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
			{
				CString strTemp, strCCText;

				// If this is an RPN from 0-4, use a user-friendly name
				if( (m_bCCType == CCTYPE_RPN_CURVE_STRIP)
				&&	(m_wRPNType < 5) )
				{
					strTemp.LoadString( m_wRPNType + IDS_RPN_PITCHBEND );
					AfxExtractSubString( strCCText, strTemp, 0, '\n' );
				}
				// If this is an RPN greater than 4, or an NRPN, use a generic name
				else if( (m_bCCType == CCTYPE_RPN_CURVE_STRIP)
					 ||  (m_bCCType == CCTYPE_NRPN_CURVE_STRIP) )
				{
					// Load the text to display for this CC type
					strCCText.LoadString( m_bCCType + IDS_ControlChange0 );

					// Get the second string
					AfxExtractSubString( strTemp, strCCText, 1, '\n' );

					// Format the string to include the RPN/NRPN number
					strCCText.Format( strTemp, int(m_wRPNType) );
				}
				else
				{
					// Load the text to display for this CC type
					strTemp.LoadString( m_bCCType + IDS_ControlChange0 );
					AfxExtractSubString( strCCText, strTemp, 0, '\n' );
				}

				strName += _T(": ");
				strName += strCCText;
			}

			pvar->vt = VT_BSTR;
			
			try
			{
				bstrName = strName.AllocSysString();
			}

			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}

			V_BSTR(pvar) = bstrName;
			break;
		}

		case SP_STRIPMGR:
			pvar->vt = VT_UNKNOWN;
			if( m_pSequenceMgr )
			{
				m_pSequenceMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
			}
			else
			{
				V_UNKNOWN(pvar) = NULL;
			}
			break;

		case SP_EARLY_TIME:
		case SP_LATE_TIME:
			if( m_pSequenceStrip )
			{
				return m_pSequenceStrip->GetStripProperty( sp, pvar );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CCurveStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	WORD wRefreshUI = FALSE;

	switch( sp )
	{
		case SP_BEGINSELECT:
		case SP_ENDSELECT:
		case SP_GUTTERSELECT:
			m_pSequenceStrip->SetStripProperty( sp, var );
			break;

		default:
			return E_FAIL;
	}

	if( wRefreshUI )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CCurveStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceStrip != NULL );

	// Process the window message
	HRESULT hr = S_OK;

	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	switch( nMsg )
	{
		case WM_SETFOCUS:
			if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
			{
				RefreshCurvePropertyPage();
			}
			// No need for this to happen
			//m_pSequenceMgr->UpdateOnDataChanged( NULL ); 
			m_pSequenceStrip->m_pActiveCurveStrip = this;
			m_pSequenceStrip->SetFocus( 2 );
			break;

		case WM_KILLFOCUS:
			m_pSequenceStrip->m_pActiveCurveStrip = NULL;
			m_pSequenceStrip->SetFocus( 0 );
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			hr = OnLButtonDown( wParam, lXPos, lYPos );
			break;

		case WM_LBUTTONUP:
			hr = OnLButtonUp( );
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			hr = OnRButtonDown( wParam, lXPos, lYPos );
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp( );
			break;

		case WM_MOUSEMOVE:
			hr = OnMouseMove( lXPos, lYPos );
			break;

		case WM_SETCURSOR:
			hr = OnSetCursor( lXPos, lYPos );
			break;

		case WM_COMMAND:
		{
			// We should only get this message in response to a selection in the right-click context menu.
			//WORD wNotifyCode = HIWORD( wParam );	// notification code 
			WORD wID		 = LOWORD( wParam );	// item, control, or accelerator identifier 

			switch( wID )
			{
				case ID_EDIT_CUT:
					hr = Cut( NULL );
					break;

				case ID_EDIT_COPY:
					hr = Copy( NULL );
					break;

				case ID_EDIT_PASTE_MERGE:
					m_pSequenceMgr->m_pTimeline->SetPasteType( TL_PASTE_MERGE );
					hr = Paste( NULL );
					break;

				case ID_EDIT_PASTE_OVERWRITE:
					m_pSequenceMgr->m_pTimeline->SetPasteType( TL_PASTE_OVERWRITE );
					hr = Paste( NULL );
					break;

				case ID_EDIT_SELECT_ALL:
					hr = SelectAll();
					break;

				case ID_NEW_CC_TRACK:
					m_pSequenceStrip->OnNewCurveStrip();
					break;

				case ID_DELETE_CC_TRACK:
					m_pSequenceStrip->OnDeleteCurveStrip( this );
					break;

				case ID_EDIT_INSERT:
					hr = Insert();
					break;

				case ID_EDIT_DELETE:
					hr = Delete();
					break;

				case ID_SINGLE_CURVE:
					hr = OnSingleCurve();
					break;

				case ID_VIEW_PROPERTIES:
					OnShowProperties();
					break;

				default:
					break;
			}
			break;
		}

		case WM_SIZE:
			if( wParam == SIZE_MAXIMIZED )
			{
				m_pSequenceStrip->OnChangeCurveStripView( SV_NORMAL );
			}
			else if( wParam == SIZE_MINIMIZED )
			{
				m_pSequenceStrip->OnChangeCurveStripView( SV_MINIMIZED );
			}
			break;

		case WM_CREATE:
			ASSERT( m_pSequenceMgr );
			ASSERT( m_pSequenceMgr->m_pTimeline );

			// Sync timeline's gutter select flag
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL( &var ) = (short)m_pSequenceStrip->m_bGutterSelected;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_GUTTER_SELECTED, var );
			break;

		default:
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::FBDraw

HRESULT CCurveStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(hDC);
	UNREFERENCED_PARAMETER(sv);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::FBOnWMMessage

HRESULT CCurveStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	ASSERT( m_pSequenceStrip != NULL );

	// Process the window message
	HRESULT hr = S_OK;

	switch( nMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			StopInsertingCurve();
			break;

		case WM_LBUTTONUP:
			hr = OnLButtonUp( );
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp( );
			break;

		case WM_MOUSEMOVE:
			hr = OnMouseMove( lXPos, lYPos );
			break;

		case WM_SETCURSOR:
			m_hCursor = m_pSequenceStrip->GetArrowCursor();
			break;

		default:
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip CreateCopyStream

HRESULT CCurveStrip::CreateCopyStream( IStream** ppIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;

	if( m_pSequenceMgr->m_pDMProdFramework == NULL )
	{
		return E_FAIL;
	}

	IStream* pIStream;

	HRESULT hr = E_FAIL;
	if( SUCCEEDED ( m_pSequenceMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Save the selected Curves into stream
		if( SUCCEEDED ( Save( pIStream ) ) )
		{
			*ppIStream = pIStream;
			(*ppIStream)->AddRef();
			hr = S_OK;
		}

		pIStream->Release();
		pIStream = NULL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip MergeDataObjects

HRESULT CCurveStrip::MergeDataObjects( IDataObject* pIDataObject1,
									   IDataObject* pIDataObject2,
									   IDataObject** ppINewDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppINewDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppINewDataObject = NULL;

	// Create a CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IEnumFORMATETC* pIEnumFORMATETC;
	IStream* pIStream;
	FORMATETC formatEtc;
	ULONG ulElem;

	HRESULT hr = E_FAIL;

	// Place streams from pIDataObject1 into CDllJazzDataObject
	hr = pIDataObject1->EnumFormatEtc( DATADIR_GET, &pIEnumFORMATETC );
	if( SUCCEEDED ( hr ) )
	{
		pIEnumFORMATETC->Reset();

		while( pIEnumFORMATETC->Next( 1, &formatEtc, &ulElem ) == S_OK )
		{
			hr = pDataObject->AttemptRead( pIDataObject1, formatEtc.cfFormat, &pIStream);
			if( hr != S_OK )
			{
				break;
			}

			hr = pDataObject->AddClipFormat( formatEtc.cfFormat, pIStream );
			pIStream->Release();

			if( hr != S_OK )
			{
				break;
			}
		}
	
		pIEnumFORMATETC->Release();
	}

	// Place streams from pIDataObject2 into CDllJazzDataObject
	if( SUCCEEDED ( hr ) )
	{
		hr = pIDataObject2->EnumFormatEtc( DATADIR_GET, &pIEnumFORMATETC );
		if( SUCCEEDED ( hr ) )
		{
			pIEnumFORMATETC->Reset();

			while( pIEnumFORMATETC->Next( 1, &formatEtc, &ulElem ) == S_OK )
			{
				hr = pDataObject->AttemptRead( pIDataObject2, formatEtc.cfFormat, &pIStream);
				if( hr != S_OK )
				{
					break;
				}

				hr = pDataObject->AddClipFormat( formatEtc.cfFormat, pIStream );
				pIStream->Release();

				if( hr != S_OK )
				{
					break;
				}
			}
		
			pIEnumFORMATETC->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppINewDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Load

HRESULT CCurveStrip::Load( IStream* pIStream, TIMELINE_PASTE_TYPE tlPasteType )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = FOURCC_SEQ_CURVE_CLIPBOARD_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = LoadDataObjectCurves( pIRiffStream, &ckMain, tlPasteType );

			if( hr != E_ABORT )
			{
				// Redraw CurveStrips
				RefreshCurveStrips();
				RefreshCurvePropertyPage();

				// Let the object know about the changes
				m_pSequenceMgr->m_fDirty = TRUE;
				m_pSequenceStrip->m_nLastEdit = IDS_UNDO_PASTE_CURVE;
				m_pSequenceMgr->OnDataChanged(); 
			}
		}

		pIRiffStream->Release();
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Save

HRESULT CCurveStrip::Save( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
		pPersistInfo = NULL;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_SEQ_CURVE_CLIPBOARD_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveSelectedCurves( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				hr = S_OK;
			}
			pIRiffStream->Release();
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::LoadDataObjectCurves

HRESULT CCurveStrip::LoadDataObjectCurves( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, TIMELINE_PASTE_TYPE tlPasteType )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
    DWORD dwStructSize;
    DWORD dwExtra;

	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );
	ASSERT( m_pSequenceStrip != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	CTypedPtrList<CPtrList, CCurveItem*> lstCurves;

	MUSIC_TIME mtEarliestCurve = INT_MAX;
	MUSIC_TIME mtLatestCurve = INT_MIN;

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK:
			{
				CCurveItem* pDMCurve;
				DMUS_IO_CURVE_ITEM iDMStyleCurve;
				ioDMCurveClipInfo iDMCurveClipInfo;

				dwSize = ck.cksize;

				// Read size of the ioDMCurveClipInfo structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				if( dwStructSize > sizeof( ioDMCurveClipInfo ) )
				{
					dwExtra = dwStructSize - sizeof( ioDMCurveClipInfo );
					dwStructSize = sizeof( ioDMCurveClipInfo );
				}
				else
				{
					dwExtra = 0;
				}

				// Now read the ioDMCurveClipInfo structure
				hr = pIStream->Read( &iDMCurveClipInfo, dwStructSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwStructSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwStructSize;

				if( dwExtra > 0 )
				{
					StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
					dwSize -= dwExtra;
				}

				// Make sure Curves are same type of Curves in this strip
				if( m_bCCType != iDMCurveClipInfo.m_bCCType )
				{
					AfxMessageBox( IDS_ERROR_CCTYPE_MISMATCH );
					hr = E_ABORT;
					goto ON_ERROR;
				}

				// Read size of the DMUS_IO_CURVE_ITEM structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				if( dwStructSize > sizeof( DMUS_IO_CURVE_ITEM ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_CURVE_ITEM );
					dwStructSize = sizeof( DMUS_IO_CURVE_ITEM );
				}
				else
				{
					dwExtra = 0;
				}

				// Now read in the Curves
				while( dwSize > 0 )
				{
					// Initialize iDMStyleCurve to all zeros
					ZeroMemory( &iDMStyleCurve, sizeof( DMUS_IO_CURVE_ITEM ) );

					hr = pIStream->Read( &iDMStyleCurve, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music Curve
					pDMCurve = new CCurveItem;
					if( pDMCurve )
					{
						pDMCurve->CopyFrom( iDMStyleCurve );

						// Update the time of the earliest and latest curves
						mtLatestCurve = max( mtLatestCurve, pDMCurve->m_mtDuration + pDMCurve->AbsTime() );
						mtEarliestCurve = min( mtEarliestCurve, pDMCurve->AbsTime() );

						// Place curve in temporary Curve list
						lstCurves.AddTail( pDMCurve ); 
					}
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	// If there are no Curves, exit
	if( lstCurves.GetHead() == NULL )
	{
		hr = S_OK;
		goto ON_ERROR;
	}

	long lTime;

	// Get cursor time
	if( FAILED( m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Snap cursor time to nearest grid
	lTime = m_pSequenceStrip->FloorTimeToGrid( lTime, NULL );

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		// Delete between mtEarliestCurve and mtLatestCurve
		DeleteCurvesBetweenTimes( mtEarliestCurve, mtLatestCurve, lTime );
	}

	// turn off Curve select flags
	UnselectAllCurvesInPart();
	
	/// Paste Curves
	while( !lstCurves.IsEmpty() )
	{
		hr = PasteCurve( lstCurves.RemoveHead(), lTime );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::PasteCurve

HRESULT CCurveStrip::PasteCurve( CCurveItem* pDMCurve,
								 long lCursorGrid )
{
	// Set m_mtTime
	pDMCurve->m_mtTime += lCursorGrid;
	
	// Do not add Curves past end of Part
	if( pDMCurve->AbsTime() > m_pSequenceStrip->m_mtLength )
	{
		delete pDMCurve;
		return S_FALSE;
	}

	// Turn on select flag
	pDMCurve->m_fSelected = TRUE;

	// Place Curve in event list
	m_pSequenceMgr->InsertByAscendingTime( pDMCurve ); 

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SaveSelectedCurves

HRESULT CCurveStrip::SaveSelectedCurves( IDMUSProdRIFFStream* pIRiffStream )
{
	CCurveItem* pDMCurve;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_CURVE_ITEM oDMStyleCurve;
	ioDMCurveClipInfo oDMCurveClipInfo;
	long lStartTime;
	POSITION pos;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	ck.ckid = DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of ioDMCurveClipInfo structure
	dwStructSize = sizeof(ioDMCurveClipInfo);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMCurveClipInfo structure
	memset( &oDMCurveClipInfo, 0, sizeof(ioDMCurveClipInfo) );

	oDMCurveClipInfo.m_bCCType = m_bCCType;

	// Save ioDMCurveClipInfo structure
	hr = pIStream->Write( &oDMCurveClipInfo, sizeof(ioDMCurveClipInfo), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMCurveClipInfo) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_CURVE_ITEM structure
	dwStructSize = sizeof(DMUS_IO_CURVE_ITEM);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get the grid positin of the earliest selected curve
	pDMCurve = GetEarliestSelectedCurve();
	if( pDMCurve )
	{
		lStartTime = pDMCurve->AbsTime();
	}
	else
	{
		lStartTime = 0;
	}

	// Now save all of the curves
	pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( pDMCurve->m_fSelected )
		{
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				// Prepare DMUS_IO_CURVE_ITEM structure
				memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_CURVE_ITEM) );

				pDMCurve->CopyTo( oDMStyleCurve );
				oDMStyleCurve.mtStart -= lStartTime;

				// Write DMUS_IO_CURVE_ITEM structure
				hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Cut

HRESULT CCurveStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	if( CanCut() == S_OK )
	{
		hr = Copy( pITimelineDataObject );
		if( SUCCEEDED ( hr ) )
		{
			hr = Delete();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Copy

HRESULT CCurveStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	// If we are passed a TimelineDataObject while our gutter is selected and there
	// is something selected in the time strip, the PianoRollStrip will take care of Copy().
	if( pITimelineDataObject && m_pSequenceStrip->m_bGutterSelected && (m_pSequenceStrip->m_lBeginSelect >= 0) && (m_pSequenceStrip->m_lEndSelect > 0))
	{
		return S_OK;
	}

	if( CanCopy() == S_OK )
	{
		IStream* pIStream;

		// Put the selected curves into an IDataObject
		if( SUCCEEDED ( CreateCopyStream( &pIStream ) ) )
		{
			// Merge with other strips
			if(pITimelineDataObject != NULL)
			{
				// add the stream to the passed IDMUSProdTimelineDataObject
				hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfCurve, pIStream );
				pIStream->Release();
				ASSERT( hr == S_OK );
				if ( hr != S_OK )
				{
					return E_FAIL;
				}
			}
			// This is the only strip -  add it to the clipboard
			else
			{
				// There is no existing data object, so just create a new one
				CDllJazzDataObject*	pDataObject;
				pDataObject = new CDllJazzDataObject;

				// add the stream to the DataObject
				hr = pDataObject->AddClipFormat( theApp.m_cfCurve, pIStream );
				pIStream->Release();
				ASSERT( hr == S_OK );
				if ( hr != S_OK )
				{
					pDataObject->Release();
					return E_FAIL;
				}

				IDataObject* pIDataObject;
				// get the new IDataObject
				hr = pDataObject->QueryInterface(IID_IDataObject, (void**) &pIDataObject);
				pDataObject->Release();
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
					return E_FAIL;
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Paste

HRESULT CCurveStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	TIMELINE_PASTE_TYPE tlPasteType = TL_PASTE_MERGE;
	m_pSequenceMgr->m_pTimeline->GetPasteType( &tlPasteType );

	const bool fInitialialDataObjectIsNull = (pITimelineDataObject == NULL);

	// Get the ITimelineDataObject
	if(pITimelineDataObject == NULL)
	{
		// Get the IDataObject from the clipboard
		HRESULT hr;
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
		if( pIDataObject )
		{
			pIDataObject->Release();
			pIDataObject = NULL;
		}
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
	
	HRESULT hr = E_FAIL;

	// Read the Curve data
	if( pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfCurve ) == S_OK )
	{
		IStream* pIStream;

		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( theApp.m_cfCurve, &pIStream ) ) )
		{
			if( SUCCEEDED ( Load ( pIStream, tlPasteType ) ) )
			{
				hr = S_OK;
			}
	
			pIStream->Release();
		}
	}
	// If this is a right-click paste and our gutter is selected
	else if( fInitialialDataObjectIsNull && m_pSequenceStrip->m_bGutterSelected )
	{
		// Try and paste through the PianoRollStrip
		hr = m_pSequenceStrip->Paste( pITimelineDataObject );
	}

	pITimelineDataObject->Release();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Insert

HRESULT CCurveStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StartInsertingCurve();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::Delete

HRESULT CCurveStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( CanDelete() == S_OK )
	{
		DeleteSelectedCurves();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectAll

HRESULT CCurveStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	WORD wRefreshUI = FALSE;

	wRefreshUI = SelectAllCurves( TRUE );

	if( wRefreshUI )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanCut

HRESULT CCurveStrip::CanCut( void )
{
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanCopy

HRESULT CCurveStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		if( IsAnyCurveSelected() )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanPaste

HRESULT CCurveStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_FALSE;
	
	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfCurve );
	}
	// Otherwise, check the clipboard
	else
	{
		// Get the IDataObject
		IDataObject*		pIDataObject;
		hr = OleGetClipboard( &pIDataObject );
		if( FAILED( hr ))
		{
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		IDMUSProdTimelineDataObject *pIInternalTimelineDataObject;
		if( FAILED( m_pSequenceMgr->m_pTimeline->AllocTimelineDataObject( &pIInternalTimelineDataObject ) ) )
		{
			pIDataObject->Release();
			return E_OUTOFMEMORY;
		}

		// Insert the IDataObject into the TimelineDataObject
		if( SUCCEEDED( pIInternalTimelineDataObject->Import( pIDataObject ) ) )
		{
			hr = pIInternalTimelineDataObject->IsClipFormatAvailable(theApp.m_cfCurve);
		}
		else
		{
			hr = E_FAIL;
		}

		pIInternalTimelineDataObject->Release();
		pIDataObject->Release();
	}

	// If our gutter is selected, and we haven't found a valid clipboard format
	if( m_pSequenceStrip->m_bGutterSelected
	&&	(hr != S_OK) )
	{
		// Check if the PianoRollStrip can paste what's in the clipboard
		return m_pSequenceStrip->CanPaste( pITimelineDataObject );
	}
	else
	{
		// Otherwise, just return hr
		return hr;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanInsert

HRESULT CCurveStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		return S_FALSE;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanDelete

HRESULT CCurveStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		if( IsAnyCurveSelected() )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CanSelectAll

HRESULT CCurveStrip::CanSelectAll( void )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetData

HRESULT CCurveStrip::GetData( void** ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	int nNbrSelectedCurves = 0;

	CPropCurve* pPropCurve = (CPropCurve *)*ppData;

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();

	if( m_fInsertingCurve )
	{
		pos = NULL;
		CCurveItem tmpCurveItem;
		tmpCurveItem.m_mtTime = m_pSequenceStrip->FloorTimeToGrid( m_lInsertingStartClock, NULL );
		if( tmpCurveItem.m_mtTime < 0 )
		{
			tmpCurveItem.m_nOffset = max( SHRT_MIN, tmpCurveItem.m_mtTime );
			tmpCurveItem.m_mtTime = 0;
		}
		else
		{
			tmpCurveItem.m_nOffset = 0;
		}
		tmpCurveItem.m_mtDuration = max( 1, m_lInsertingEndClock - m_lInsertingStartClock );

		tmpCurveItem.m_bCurveShape = DMUS_CURVES_SINE;
		tmpCurveItem.m_nStartValue = m_nInsertingStartValue;
		tmpCurveItem.m_nEndValue = m_nInsertingEndValue;
		tmpCurveItem.m_bType = StripCCTypeToCurveType( m_bCCType );
		if( tmpCurveItem.m_bType == DMUS_CURVET_CCCURVE )
		{
			tmpCurveItem.m_bCCData = m_bCCType;
		}

		tmpCurveItem.m_wParamType = m_wRPNType;

		tmpCurveItem.SetDefaultResetValues( CalcPartClockLength() );

		pPropCurve->m_pSequenceMgr = m_pSequenceMgr;
		pPropCurve->Import( &tmpCurveItem );
		nNbrSelectedCurves = 1;
	}

	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext(pos);
		if( pDMCurve->m_fSelected )
		{
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				CCurveItem newCurve( pDMCurve );
				if( m_pCCurveTracker )
				{
					CRect newRect = GetUpdatedCurveRect(*m_pCCurveTracker, &newCurve, m_nTrackerAction );

					// figure out start and end values and positions
					long lStartClock, lEndClock;
					m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
					m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
					short nStartValue = (short)YPosToValue(newRect.top);
					short nEndValue = (short)YPosToValue(newRect.bottom);

					if( lStartClock < 0 )
					{
						newCurve.m_mtTime = 0;
						newCurve.m_nOffset = max( SHRT_MIN, lStartClock );
					}
					else
					{
						newCurve.m_mtTime = lStartClock;
						newCurve.m_nOffset = 0;
					}
					if (m_nTrackerAction != CTRK_DRAGGING)
					{
						newCurve.m_mtDuration = lEndClock - lStartClock;
					}

					newCurve.m_nStartValue = nStartValue;
					newCurve.m_nEndValue = nEndValue;
				}

				if( nNbrSelectedCurves == 0 )
				{
					pPropCurve->m_pSequenceMgr = m_pSequenceMgr;
					pPropCurve->Import( &newCurve );
				}
				else
				{
					CPropCurve propcurve( &newCurve, m_pSequenceMgr );

					*pPropCurve += propcurve;
				}

				nNbrSelectedCurves++;
			}
		}
	}

	if( nNbrSelectedCurves == 0 )
	{
		return S_FALSE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SetData

HRESULT CCurveStrip::SetData( void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// A list to temporarily store the sequences in if their start times are moved
	CTypedPtrList<CPtrList, CCurveItem*> lstMovedCurves;

	DWORD dwChanged = 0;
	CPropCurve* pPropCurve = (CPropCurve *)pData;

	POSITION pos2, pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext(pos);
		if( pDMCurve->m_fSelected )
		{
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				// If we should copy the curve
				if( !(pPropCurve->m_dwUndetermined & UD_COPY) )
				{
					// Create a new note
					CCurveItem *pNewCurve;
					pNewCurve = new CCurveItem;
					pNewCurve->Copy( pDMCurve );

					// Insert just after the existing note
					pos2 = m_pSequenceMgr->m_lstCurves.InsertAfter( pos2, pNewCurve );

					pDMCurve->m_fSelected = FALSE;
					pDMCurve = pNewCurve;
				}

				// Save the old start time
				long lOldValue = pDMCurve->AbsTime();

				if ( !(pPropCurve->m_dwChanged & (CHGD_CHORDBAR | CHGD_CHORDBEAT | CHGD_START_BARBEATGRIDTICK | CHGD_END_BARBEATGRIDTICK | CHGD_DUR_BARBEATGRIDTICK)) )
				{
					// We're not changing any start/end/dur bar/beat/grid/tick
					// Apply the changes
					pPropCurve->m_pSequenceMgr = m_pSequenceMgr;
					dwChanged |= pPropCurve->ApplyToCurve(pDMCurve);
				}
				else
				{
					// We're not doing an offset, and we're changing at least one
					// start/end/dur bar/beat/grid/tick
					CPropCurve propnote( pDMCurve, m_pSequenceMgr );
					propnote.ApplyPropCurve( pPropCurve );

					// Apply the changes
					dwChanged |= propnote.ApplyToCurve(pDMCurve);
				}

				// If the note's start position changed, remove and re-add it to the list
				if( lOldValue != pDMCurve->AbsTime() )
				{
					// If moved backwards, only add to lstMovedCurves if we're now before
					// the item before the one at pos2.
					if( pDMCurve->AbsTime() < lOldValue )
					{
						POSITION pos3 = pos2;
						m_pSequenceMgr->m_lstCurves.GetPrev( pos3 );
						if( pos3 )
						{
							//CSequenceItem* pItem = m_pSequenceMgr->m_lstCurves.GetAt( pos3 );
							if( pDMCurve->AbsTime() < m_pSequenceMgr->m_lstCurves.GetAt( pos3 )->AbsTime() )
							{
								// Need to move pDMCurve, so remove it from its current location
								m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );

								lstMovedCurves.AddTail( pDMCurve );

								// This should be fine
								//InsertByAscendingTimeBackwardsFrom( pDMCurve, pos3 );
							}
						}
					}
					// If moved forwards, only add to lstMovedCurves if we're now after
					// the item at pos.
					else if( pos )
					{
						//CSequenceItem* pItem = m_pSequenceMgr->m_lstCurves.GetAt( pos );
						if( pDMCurve->AbsTime() > m_pSequenceMgr->m_lstCurves.GetAt( pos )->AbsTime() )
						{
							// Need to move pDMCurve, so remove it from its current location
							m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );

							lstMovedCurves.AddTail( pDMCurve );

							// If we do this, the notes will be changed repeatedly...
							//InsertByAscendingTimeFrom( pDMCurve, pos );
						}
					}
				}
			}
		}
	}

	if( !lstMovedCurves.IsEmpty() )
	{
		m_pSequenceMgr->MergeCurve( lstMovedCurves );
	}

	// Always refresh property page. (If user tries to change duration to value less than 1,
	// we need to reset the property page to valid values)
	RefreshCurvePropertyPage();
	if( dwChanged )
	{
		// Redraw CurveStrips
		RefreshCurveStrips();

		// Let the object know about the changes
		m_pSequenceMgr->m_fDirty = TRUE;
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_CHANGE_CURVE;
		m_pSequenceMgr->OnDataChanged(); 
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnShowProperties

HRESULT CCurveStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStripMgr != NULL );

	if( m_pSequenceMgr->m_pDMProdFramework == NULL )
	{
		return E_FAIL;
	}

	// Get the Curve page manager
	CCurvePropPageMgr* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_CurvePropPageManager ) == S_OK )
	{
		pPageManager = (CCurvePropPageMgr *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CCurvePropPageMgr( GUID_CurvePropPageManager );
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus;
	hwndHadFocus = ::GetFocus();

	// Show the Curve properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pSequenceMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CCurvePropPageMgr::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			if( theApp.m_pIPageManager != pPageManager )
			{
				theApp.m_pIPageManager = pPageManager;
				pIPropSheet->SetActivePage( nActiveTab ); 
			}

			pPageManager->SetObject( this );
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	// Restore the focus if it has changed
	if( hwndHadFocus != ::GetFocus() )
	{
		::SetFocus( hwndHadFocus );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnRemoveFromPageManager

HRESULT CCurveStrip::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip Selection methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::CalcPartClockLength

long CCurveStrip::CalcPartClockLength( void )
{
	ASSERT( m_pSequenceStrip != NULL );

	return m_pSequenceStrip->m_mtLength;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetCurveFromPoint

CCurveItem*	CCurveStrip::GetCurveFromPoint( int nStripXPos, int nStripYPos, BOOL bFrameOnly )
{
	CCurveItem* pTheDMCurve = NULL;
	BOOL fOnTheCurveHandles = FALSE;

	BOOL fOnCurveHandles;
	CRect rect;

	long lPartMaxXPos;
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( CalcPartClockLength(), &lPartMaxXPos );

	CPoint point1( nStripXPos, nStripYPos );

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			CRect rectCurve = bFrameOnly ? pDMCurve->m_rectFrame : pDMCurve->m_rectSelect;
	
			if( rectCurve.PtInRect( point1 ) )  
			{
				// Need to set fOnCurveHandles
				fOnCurveHandles = FALSE;
				if( pDMCurve->m_fSelected )
				{
					// Convert the frame rect of the curve to window coordinates
					CRect selRect = pDMCurve->m_rectFrame;
					VERIFY( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->StripToWindowPos( this, &selRect.TopLeft() ) ) );
					VERIFY( SUCCEEDED ( m_pSequenceMgr->m_pTimeline->StripToWindowPos( this, &selRect.BottomRight() ) ) );

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

				if( pTheDMCurve == NULL )
				{
					pTheDMCurve = pDMCurve;
					fOnTheCurveHandles = fOnCurveHandles;
				}
				else
				{
					if( fOnTheCurveHandles == FALSE )
					{
						if( fOnCurveHandles == TRUE )
						{
							// Use the Curve whose handle is being hit
							pTheDMCurve = pDMCurve;
							fOnTheCurveHandles = TRUE;
						}
						else
						{
							// Use the Curve with the narrowest width
							if( (pDMCurve->m_rectSelect.right - pDMCurve->m_rectSelect.left) <
								(pTheDMCurve->m_rectSelect.right - pTheDMCurve->m_rectSelect.left) ) 
							{
								pTheDMCurve = pDMCurve;
							}
						}
					}
					else
					{
						if( fOnCurveHandles == TRUE )
						{
							// Use the Curve with the narrowest width
							if( (pDMCurve->m_rectSelect.right - pDMCurve->m_rectSelect.left) <
								(pTheDMCurve->m_rectSelect.right - pTheDMCurve->m_rectSelect.left) ) 
							{
								pTheDMCurve = pDMCurve;
							}
						}
					}
				}
			}
		}
	}

	return pTheDMCurve;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectCurve

WORD CCurveStrip::SelectCurve( CCurveItem* pTheDMCurve, BOOL fState )
{
	WORD wRefreshUI = FALSE;

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			if( pDMCurve == pTheDMCurve )
			{
				if( pTheDMCurve->m_fSelected != fState )
				{
					pTheDMCurve->m_fSelected = fState;

					wRefreshUI = TRUE;
				}
			}
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectAllCurves

WORD CCurveStrip::SelectAllCurves( BOOL fState )
{
	WORD wRefreshUI = FALSE;

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			if( pDMCurve->m_fSelected != fState )
			{
				pDMCurve->m_fSelected = fState;

				wRefreshUI = TRUE;
			}
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::UnselectAllCurvesInPart

WORD CCurveStrip::UnselectAllCurvesInPart( void )
{
	WORD wRefreshUI = FALSE;

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( pDMCurve->m_fSelected )
		{
			pDMCurve->m_fSelected = FALSE;

			wRefreshUI = TRUE;
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectCurvesInMusicTimeRange

WORD CCurveStrip::SelectCurvesInMusicTimeRange( long lBegin, long lEnd )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	if( lBegin == lEnd )
	{
		return FALSE;
	}

	CRect rectSelect1;
	CRect rectSelect2;
	WORD wRefreshUI = FALSE;

	long lPartClockLength = CalcPartClockLength();
	long lBeginClock = lBegin > lEnd ? lEnd : lBegin;
	long lEndClock = lBegin > lEnd ? lBegin : lEnd;

	if( (lEndClock - lBeginClock) >= lPartClockLength )
	{
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( 0, &rectSelect1.left );
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( lPartClockLength, &rectSelect1.right );
		rectSelect2 = rectSelect1;
	}
	else
	{
		if( lBegin <= lEnd )
		{
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( lBegin, &rectSelect1.left );
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( lEnd, &rectSelect1.right );
			rectSelect2 = rectSelect1;
		}
		else
		{
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( lBegin, &rectSelect1.left );
			rectSelect1.right = lPartClockLength;
			rectSelect2.left = 0;
			m_pSequenceMgr->m_pTimeline->ClocksToPosition( lEnd, &rectSelect2.right );
		}
	}

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			if( (pDMCurve->m_rectSelect.left >= rectSelect1.left  &&
				 pDMCurve->m_rectSelect.left <= rectSelect1.right)
			||  (pDMCurve->m_rectSelect.left >= rectSelect2.left  &&
				 pDMCurve->m_rectSelect.left <= rectSelect2.right) )
			{
				if( pDMCurve->m_fSelected != TRUE )
				{
					pDMCurve->m_fSelected = TRUE;

					wRefreshUI = TRUE;
				}
			}
			else
			{
				if( pDMCurve->m_fSelected != FALSE )
				{
					pDMCurve->m_fSelected = FALSE;

					wRefreshUI = TRUE;
				}
			}
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectCurvesInRect

WORD CCurveStrip::SelectCurvesInRect( CRect* pRect )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	CRect rect;
	CRect rectSelect1( pRect );
	WORD wRefreshUI = FALSE;

	long lPartMaxXPos;
	long lPartClockLength = CalcPartClockLength();
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( lPartClockLength, &lPartMaxXPos );

	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			if( rect.IntersectRect(&pDMCurve->m_rectSelect, rectSelect1) )
			{
				if( pDMCurve->m_rectSelect.left >= rectSelect1.left  &&
					pDMCurve->m_rectSelect.left <= rectSelect1.right )
				{
					if( pDMCurve->m_fSelected != TRUE )
					{
						pDMCurve->m_fSelected = TRUE;

						wRefreshUI = TRUE;
					}
				}
				else
				{
					if( pDMCurve->m_fSelected != FALSE )
					{
						pDMCurve->m_fSelected = FALSE;

						wRefreshUI = TRUE;
					}
				}
			}
			else
			{
				if( pDMCurve->m_fSelected != FALSE )
				{
					pDMCurve->m_fSelected = FALSE;

					wRefreshUI = TRUE;
				}
			}
		}
	}

	return wRefreshUI;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::UnselectGutterRange

void CCurveStrip::UnselectGutterRange( void )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_pSequenceStrip->m_bSelectionCC = m_bCCType;
	m_pSequenceStrip->m_wSelectionParamType = m_wRPNType;
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_pSequenceStrip->m_bSelectionCC = 0xFF;
	m_pSequenceStrip->m_wSelectionParamType = 0xFFFF;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip Additional methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::HitTest

UINT CCurveStrip::HitTest( CCurveTracker& tracker, long lStripXPos, long lStripYPos )
{
	CPoint point( lStripXPos, lStripYPos );
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &point)));

	// Try point
	UINT hit = tracker.HitTest( point );

	return hit;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnLButtonDown

HRESULT CCurveStrip::OnLButtonDown( WPARAM wParam, long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		return S_OK;
	}

	UnselectGutterRange();

	// Handle inserting a new Curve
	if( m_fInsertingCurve )
	{
		InsertCurve( lXPos, lYPos );
		StopInsertingCurve();
		return S_OK;
	}

	// Set timeline cursor
	MUSIC_TIME mtTime;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime ) ) )
	{
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, mtTime );
	}

	// Get a pointer to the Timeline
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	
	CCurveItem* pDMCurve = GetCurveFromPoint( lXPos, lYPos, TRUE );
	WORD wRefreshUI = FALSE;
	
	if (pDMCurve) {		// have we clicked on a curve?
		if (pDMCurve->m_fSelected) {	// curve selected?
			// convert the frame rect of the curve to window coordinates
			CRect selRect = pDMCurve->m_rectFrame;
			VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
			VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
			
			// create a tracker and find out if we are on a handle or somewhere within the curve
			CCurveTracker curveTracker(&selRect);
			UINT hit = HitTest( curveTracker, lXPos, lYPos );

			switch(hit) {
			case CRectTracker::hitMiddle:
				DragModify(pDMCurve, lXPos, lYPos, CTRK_DRAGGING, (wParam & MK_CONTROL) ? true : false);
				break;
			default:
				wRefreshUI |= SelectAllCurves(FALSE);
				wRefreshUI |= SelectCurve(pDMCurve, TRUE);
				RefreshUI(wRefreshUI);
				DragModify(pDMCurve, lXPos, lYPos, CTRK_DRAGGING_EDGE, false);
			}
		}
		else {
			
			if (!(wParam & MK_CONTROL)) {
				wRefreshUI |= SelectAllCurves(FALSE);
			}

			wRefreshUI |= SelectCurve(pDMCurve, TRUE);
			RefreshUI(wRefreshUI);
			DragModify(pDMCurve, lXPos, lYPos, CTRK_DRAGGING, false);
		}
		RefreshUI(wRefreshUI);
	}
	else {
		
		// user might have clicked on the curve outside of the frame
		// in which case we select the curve only
		CCurveItem* pDMCurve = GetCurveFromPoint( lXPos, lYPos, FALSE );
		if (pDMCurve) {
			if (!(wParam & MK_CONTROL)) {
				wRefreshUI |= SelectAllCurves(FALSE);
			}
			wRefreshUI |= SelectCurve(pDMCurve, !((wParam & MK_CONTROL) && pDMCurve->m_fSelected));
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

			if( pDMCurve )
			{
				wRefreshUI |= SelectCurve( pDMCurve, TRUE );
			}
			else
			{
				// Refresh UI
				RefreshUI(wRefreshUI);

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
							CCurveTracker curveTracker( &wnd, m_pSequenceMgr->m_pTimeline, this, CTRK_SELECTING );
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
// CCurveStrip::OnLButtonUp

HRESULT CCurveStrip::OnLButtonUp( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Finish inserting new Curve
	if( m_fInsertingCurve )
	{
		StopInsertingCurve();
		return S_OK;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnRButtonDown

HRESULT CCurveStrip::OnRButtonDown( WPARAM wParam, long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
	{
		return S_OK;
	}

	UnselectGutterRange();

	// Can't insert Curve with right mouse button
	StopInsertingCurve();

	// Set timeline cursor
	MUSIC_TIME mtTime;
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime ) ) )
	{
		m_pSequenceMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, mtTime );
	}

	/*
	// TODO: REMOVE THIS (its a test)
	long lTestX;
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( mtTime, &lTestX );
	if (lTestX != lXPos) {
		MessageBox(0, "Not Equal!", "Doh", MB_OK);
	}*/

	// Get a pointer to the Timeline
	if( m_pSequenceMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	// Handle Curve selection
	CCurveItem* pDMCurve = GetCurveFromPoint( lXPos, lYPos );

	WORD wRefreshUI = FALSE;

	if( !(wParam & MK_CONTROL)
	&&  !(wParam & MK_SHIFT) )
	{
		if( pDMCurve )
		{
			if( pDMCurve->m_fSelected == FALSE )
			{
				wRefreshUI  = SelectAllCurves( FALSE );
				wRefreshUI |= SelectCurve( pDMCurve, TRUE );
			}
		}
	}

	if( wRefreshUI )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnRButtonUp

HRESULT CCurveStrip::OnRButtonUp( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure "inserting curve" fields are initialized
	if( m_fInsertingCurve )
	{
		StopInsertingCurve();
		return S_OK;
	}

	HRESULT hr = OnContextMenu( );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnMouseMove

HRESULT CCurveStrip::OnMouseMove( long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Handle inserting a new Curve
	if( m_fInsertingCurve == INSERT_CURVE_START_VALUE )
	{
		CPoint ptLeft( lXPos, lYPos );
		CPoint ptRight( -1, -1 );

		OnUpdateInsertCurveValue( ptLeft, ptRight );
		return S_OK;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnSetCursor
//
// Sets the mouse cursor based on where the mouse is relative to the curves.
HRESULT CCurveStrip::OnSetCursor( long lXPos, long lYPos)
{
	if (m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP)
	{
		m_hCursor = m_pSequenceStrip->GetArrowCursor();
		return S_OK;
	}

	CCurveItem* pDMCurve = GetCurveFromPoint( lXPos, lYPos, TRUE );

	if (pDMCurve)
	{
		if (pDMCurve->m_fSelected)
		{
			// convert the frame rect of the curve to window coordinates
			CRect selRect = pDMCurve->m_rectFrame;
			VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
			VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));

			// create a tracker
			CCurveTracker curveTracker(&selRect);
			UINT hit = HitTest( curveTracker, lXPos, lYPos );
		
			// I had to do this because CRectTracker.LoadCursor doesn't return an HCURSOR
			switch(hit)
			{
			case CRectTracker::hitTopLeft:
			case CRectTracker::hitBottomRight:
				m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
				break;
			case CRectTracker::hitTopRight:
			case CRectTracker::hitBottomLeft:
				m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
				break;
			case CRectTracker::hitTop:
			case CRectTracker::hitBottom:
				m_hCursor = LoadCursor(NULL, IDC_SIZENS);
				break;
			case CRectTracker::hitRight:
			case CRectTracker::hitLeft:
				m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
				break;
			case CRectTracker::hitMiddle:
				m_hCursor = LoadCursor(NULL, IDC_SIZEALL);
				break;
			default:
				m_hCursor = LoadCursor(NULL, IDC_ARROW);
				break;
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

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnContextMenu

HRESULT CCurveStrip::OnContextMenu( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStripMgr != NULL );

	HRESULT hr = E_FAIL;

	if( m_pSequenceMgr->m_pTimeline )
	{
		HMENU hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_CURVE_RMENU));
		if( hMenu )
		{
			HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
			if( hMenuPopup )
			{
				if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP )
				{
					// Disable items
					EnableMenuItem( hMenuPopup, ID_EDIT_CUT,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_COPY,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, 2, MF_BYPOSITION | MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_DELETE_CC_TRACK,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_INSERT,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_DELETE,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_SINGLE_CURVE,  MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES,  MF_GRAYED );
				}
				else
				{
					// Enable/disable items as appropriate
					EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ?
									MF_ENABLED : MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ?
									MF_ENABLED : MF_GRAYED );
					EnableMenuItem( hMenuPopup, 2, ( CanPaste() == S_OK ) ?
									MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, MF_ENABLED );
					EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ?
									MF_ENABLED : MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ?
									MF_ENABLED : MF_GRAYED );
					EnableMenuItem( hMenuPopup, ID_SINGLE_CURVE, ( IsAnyCurveSelected() ) ?
									MF_ENABLED : MF_GRAYED );
				}

				POINT pt;
				if( GetCursorPos( &pt ) )
				{
					m_pSequenceMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
					hr = S_OK;
				}
			}

			DestroyMenu( hMenu ); // This will destroy the submenu as well.
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::YPosToValue

short CCurveStrip::YPosToValue( long lYPos )
{
	long lMaxValue;
	long lValue;

	// Store maximum value
	if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
	{
		lMaxValue = MAX_PB_VALUE;
	}
	else if( (m_bCCType == CCTYPE_RPN_CURVE_STRIP)
		 ||  (m_bCCType == CCTYPE_NRPN_CURVE_STRIP) )
	{
		lMaxValue = MAX_RPN_VALUE;
	}
	else
	{
		lMaxValue = MAX_CC_VALUE;
	}

	// Get Strip height
	VARIANT var;
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}

	lValue  = (long)nStripHeight - lYPos;
	lValue  = lValue * lMaxValue;
	lValue /= (long)nStripHeight;

	if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
	{
		// Don't allow "0" values for pitch bends
		if( lValue == 0 )
		{
			lValue = 1;
		}

		// Snap to center when pitch bend value is near center
		if( (lValue >= (PB_DISP_OFFSET - 100))
		&&  (lValue <= (PB_DISP_OFFSET + 100)) )
		{
			lValue = PB_DISP_OFFSET;
		}
	}
	
	return (short)lValue;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::ValueToYPos

short CCurveStrip::ValueToYPos( long lValue )
{
	DWORD dwYPos;

	// Get Strip height
	VARIANT var;
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}

	if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
	{
		// Needed to reverse YPosToValue code which changes "0" to "1"
		if( lValue == 1 )
		{
			lValue = 0;
		}
	}

	dwYPos = (DWORD)lValue;
	if( (m_bCCType != CCTYPE_PB_CURVE_STRIP)
	&&	(m_bCCType != CCTYPE_RPN_CURVE_STRIP)
	&&	(m_bCCType != CCTYPE_NRPN_CURVE_STRIP))
	{
		dwYPos = dwYPos << 7;
	}
	dwYPos = 16384 - dwYPos;
	dwYPos *= (DWORD)nStripHeight;
	dwYPos /= 16384;

	return (short)dwYPos;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnUpdateInsertCurveValue
//
// Called when tracker is resized during curve insertion.
// Updates the status bar and computes curve start and end values and
// start and end times.
void CCurveStrip::OnUpdateInsertCurveValue( POINT ptLeft, POINT ptRight )
{
	CString strStart;
	CString strEnd;
	CString strLeft;
	CString strMid;
	CString strRight;
	CString strStatus;

	ASSERT( m_pSequenceStrip != NULL );
	
	strStart.LoadString( IDS_START_TEXT );
	strEnd.LoadString( IDS_END_TEXT );

	if( m_bCCType == CCTYPE_PAN_CURVE_STRIP )
	{
		strLeft.LoadString( IDS_PAN_LEFT );
		strMid.LoadString( IDS_PAN_MID );
		strRight.LoadString( IDS_PAN_RIGHT );
	}

	// Get CurveStrip rect
	CRect rectStrip;
	if (!GetStripRect(rectStrip))
	{
		return;
	}

	// Convert points into Window coordinates
	CPoint ptWndLeft( ptLeft );
	CPoint ptWndRight( ptRight );
	m_pSequenceMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWndLeft );
	m_pSequenceMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWndRight );

	// Make sure ptLeft.y is in this CurveStrip
	if( ptWndLeft.y < rectStrip.top
	||  ptWndLeft.y > rectStrip.bottom )
	{
		// ptLeft.y is NOT in this CurveStrip
		strStatus.Format( "%s, %s", strStart, strEnd );
	}
	else
	{
		// Get clock length of this CurveStrip
		VARIANT var;
		m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lClockLength = V_I4(&var);

		// Make sure ptLeft.x is in this CurveStrip
		long lStartClock;

		m_pSequenceMgr->m_pTimeline->PositionToClocks( ptLeft.x, &lStartClock );
		if( lStartClock < 0
		||  lStartClock > lClockLength )
		{
			// ptLeft.x is NOT in this CurveStrip
			strStatus.Format( "%s, %s", strStart, strEnd );
		}
		else
		{
			long lPartClockLength = CalcPartClockLength();
			
			// start and end values that will be *displayed*
			short nDispStartValue;
			short nDispEndValue;

			// Set StartValue
			m_lInsertingStartClock = lStartClock % lPartClockLength;
			m_nInsertingStartValue = (short)YPosToValue( ptLeft.y );
			
			// offset start value for pitch bend curves
			if (m_bCCType == CCTYPE_PB_CURVE_STRIP)
			{
				nDispStartValue = short(m_nInsertingStartValue - PB_DISP_OFFSET);
			}
			else
			{
				nDispStartValue = m_nInsertingStartValue;
			}

			// See if ptRight.y is in this CurveStrip
			if( ptWndRight.y >= rectStrip.top
			&&  ptWndRight.y <= rectStrip.bottom )
			{
				long lEndClock;

				// ptRight.y is in this CurveStrip so set EndValue
				m_pSequenceMgr->m_pTimeline->PositionToClocks( ptRight.x, &lEndClock );
				m_lInsertingEndClock = m_lInsertingStartClock + (lEndClock - lStartClock);
				m_nInsertingEndValue = (short)YPosToValue( ptRight.y );
				
				// offset end value for pitch bend curves
				if (m_bCCType == CCTYPE_PB_CURVE_STRIP)
				{
					nDispEndValue = short(m_nInsertingEndValue - PB_DISP_OFFSET);
				}
				else
				{
					nDispEndValue = m_nInsertingEndValue;
				}

				if( m_bCCType == CCTYPE_PAN_CURVE_STRIP )
				{
					// Format Start string
					CString strPanStart;
					if( nDispStartValue < 63 )
					{
						strPanStart.Format( "%s%d", strLeft, (63 - nDispStartValue) );
					}
					else if( nDispStartValue > 63 )
					{
						strPanStart.Format( "%s%d", strRight, (nDispStartValue - 63) );
					}
					else
					{
						strPanStart = strMid;
					} 

					// Format End string
					CString strPanEnd;
					if( nDispEndValue < 63 )
					{
						strPanEnd.Format( "%s%d", strLeft, (63 - nDispEndValue) );
					}
					else if( nDispEndValue > 63 )
					{
						strPanEnd.Format( "%s%d", strRight, (nDispEndValue - 63) );
					}
					else
					{
						strPanEnd = strMid;
					} 
					
					strStatus.Format( "%s %s, %s %s", strStart, strPanStart, strEnd, strPanEnd );
				}
				else
				{
					strStatus.Format( "%s %d, %s %d", strStart, nDispStartValue, strEnd, nDispEndValue );
				}

//				TRACE( "START %i    END %i\n", m_lInsertingStartClock, m_lInsertingEndClock );
			}
			else
			{
				if( m_bCCType == CCTYPE_PAN_CURVE_STRIP )
				{
					// Format Start string
					CString strPanStart;
					if( nDispStartValue < 63 )
					{
						strPanStart.Format( "%s%d", strLeft, (63 - nDispStartValue) );
					}
					else if( nDispStartValue > 63 )
					{
						strPanStart.Format( "%s%d", strRight, (nDispStartValue - 63) );
					}
					else
					{
						strPanStart = strMid;
					} 
					
					strStatus.Format( "%s %s, %s", strStart, strPanStart, strEnd );
				}
				else
				{
					strStatus.Format( "%s %d, %s", strStart, nDispStartValue, strEnd );
				}
			}
		}
	}

	// Refresh status bar
	if( m_pSequenceMgr->m_pDMProdFramework )
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}

	// Refresh property page
	RefreshCurvePropertyPage();
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnUpdateDragCurveValue
//
// Called when the tracker is resized during curve dragging/resizing.
// Updates the status bar with start, end values.
//
void CCurveStrip::OnUpdateDragCurveValue(CCurveTracker& tracker, int nAction)
{
	CRect newRect = GetUpdatedCurveRect(tracker, tracker.m_pDMCurve, nAction);

	// figure out start and end values and positions
	long lStartClock, lEndClock;
	m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
	m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
	short nStartValue = (short)YPosToValue(newRect.top);
	short nEndValue = (short)YPosToValue(newRect.bottom);

	// offset values for pitch bend curves
	if (m_bCCType == CCTYPE_PB_CURVE_STRIP) {

		nStartValue -= PB_DISP_OFFSET;
		nEndValue -= PB_DISP_OFFSET;
	}

	// create the status bar string
	CString strStart;
	CString strEnd;
	CString strLeft;
	CString strMid;
	CString strRight;
	CString strStatus;

	strStart.LoadString( IDS_START_TEXT );
	strEnd.LoadString( IDS_END_TEXT );

	if( m_bCCType == CCTYPE_PAN_CURVE_STRIP )
	{
		strLeft.LoadString( IDS_PAN_LEFT );
		strMid.LoadString( IDS_PAN_MID );
		strRight.LoadString( IDS_PAN_RIGHT );
	}

	if( m_bCCType == CCTYPE_PAN_CURVE_STRIP )
	{
		// Format Start string
		CString strPanStart;
		if( nStartValue < 63 )
		{
			strPanStart.Format( "%s%d", strLeft, (63 - nStartValue) );
		}
		else if( nStartValue > 63 )
		{
			strPanStart.Format( "%s%d", strRight, (nStartValue - 63) );
		}
		else
		{
			strPanStart = strMid;
		} 

		// Format End string
		CString strPanEnd;
		if( nEndValue < 63 )
		{
			strPanEnd.Format( "%s%d", strLeft, (63 - nEndValue) );
		}
		else if( nEndValue > 63 )
		{
			strPanEnd.Format( "%s%d", strRight, (nEndValue - 63) );
		}
		else
		{
			strPanEnd = strMid;
		} 
		
		strStatus.Format( "%s %s, %s %s", strStart, strPanStart, strEnd, strPanEnd );
	}
	else
	{
		strStatus.Format("%s %d, %s %d", strStart, nStartValue, strEnd, nEndValue);
	}
	
	// Refresh status bar
	if( m_pSequenceMgr->m_pDMProdFramework )
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}

	// Refresh property page
	RefreshCurvePropertyPage();
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::StartInsertingCurve

void CCurveStrip::StartInsertingCurve( void )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	m_nInsertingStartValue = 0;
	m_nInsertingEndValue = 0;
	m_lInsertingStartClock = 0;
	m_lInsertingEndClock = 0;

	m_fInsertingCurve = INSERT_CURVE_START_VALUE;

	// Ensure the arrow cursor is displayed
	m_hCursor = LoadCursor(NULL, IDC_ARROW);
	::SetCursor( m_hCursor );

	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	if( m_pSequenceMgr->m_pDMProdFramework )
	{
		if( m_hKeyStatusBar )
		{
			m_pSequenceMgr->m_pDMProdFramework->RestoreStatusBar( m_hKeyStatusBar );				
			m_hKeyStatusBar = NULL;
		}

		m_pSequenceMgr->m_pDMProdFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar );
		if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
		{
			m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		}
		else
		{
			m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 18 );
		}
	}

	CPoint pt( -1, -1 );
	OnUpdateInsertCurveValue( pt, pt );
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::StopInsertingCurve

void CCurveStrip::StopInsertingCurve( void )
{
	ASSERT( m_pSequenceMgr != NULL );
	ASSERT( m_pSequenceMgr->m_pTimeline != NULL );

	if( m_fInsertingCurve )
	{
		if( m_fInsertingCurve == INSERT_CURVE_START_VALUE )
		{
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		}

		m_fInsertingCurve = INSERT_CURVE_FALSE;

		if( m_pSequenceMgr->m_pDMProdFramework )
		{
			m_pSequenceMgr->m_pDMProdFramework->RestoreStatusBar( m_hKeyStatusBar );				
			m_hKeyStatusBar = NULL;
		}

		// Refresh property page
		RefreshCurvePropertyPage();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::InsertCurve

void CCurveStrip::InsertCurve( long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pSequenceStrip != NULL );

	// Get CurveStrip rect
	CRect rectStrip;
	if (!GetStripRect(rectStrip))
	{
		return;
	}

	// Convert points into Window coordinates
	CPoint ptWnd( lXPos, lYPos );
	m_pSequenceMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWnd );

	// Make sure YPos is in this CurveStrip
	if( ptWnd.y < rectStrip.top
	||  ptWnd.y > rectStrip.bottom )
	{
		// YPos is NOT in this CurveStrip
		return;
	}

	// Make sure XPos is in this CurveStrip
	VARIANT var;
	m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lClockLength = V_I4(&var);
	long lXPosClock;

	m_pSequenceMgr->m_pTimeline->PositionToClocks( lXPos, &lXPosClock );
	if( lXPosClock < 0
	||  lXPosClock > lClockLength
	||  ptWnd.x < rectStrip.left 
	||  ptWnd.x > rectStrip.right )
	{
		// XPos is NOT in this CurveStrip
		return;
	}

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
				// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
				// We have to call ReleaseCapture(), or the CurveTracker won't work.
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
				::ReleaseCapture();

				m_fInsertingCurve = INSERT_CURVE_END_VALUE;

				wnd.Attach( hWnd );
				CCurveTracker curveTracker( &wnd, m_pSequenceMgr->m_pTimeline, this, CTRK_INSERTING );
				if( curveTracker.TrackRubberBand( &wnd, point, TRUE ) )
				{
					// Create Direct Music Curve
					CCurveItem* pCurve = new CCurveItem;
					if( pCurve )
					{
						// Fill in Curve values
						pCurve->m_mtTime = m_pSequenceStrip->FloorTimeToGrid( m_lInsertingStartClock, NULL );
						if( pCurve->m_mtTime < 0 )
						{
							pCurve->m_nOffset = max( SHRT_MIN, pCurve->m_mtTime );
							pCurve->m_mtTime = 0;
						}
						else
						{
							pCurve->m_nOffset = 0;
						}
						pCurve->m_mtDuration = m_lInsertingEndClock - m_lInsertingStartClock;

						pCurve->m_bCurveShape = DMUS_CURVES_SINE;
						pCurve->m_nStartValue = m_nInsertingStartValue;
						pCurve->m_nEndValue = m_nInsertingEndValue;
						pCurve->m_bType = StripCCTypeToCurveType( m_bCCType );
						if( pCurve->m_bType == DMUS_CURVET_CCCURVE )
						{
							pCurve->m_bCCData = m_bCCType;
						}

						pCurve->m_wParamType = m_wRPNType;

						pCurve->SetDefaultResetValues( CalcPartClockLength() );

						// Place Curve in Part's event list
						SelectAllCurves( FALSE );
						pCurve->m_fSelected = TRUE;

						m_pSequenceMgr->InsertByAscendingTime( pCurve );

						// Redraw CurveStrips
						RefreshCurveStrips();
						RefreshCurvePropertyPage();

						// Let the object know about the changes
						m_pSequenceMgr->m_fDirty = TRUE;
						m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT_CURVE;
						m_pSequenceMgr->OnDataChanged(); 
					}
				}

				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

				wnd.Detach();
			}
		}

		pIOleWindow->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetUpdatedCurveRect
//
// Returns an updated rect for the given curve based on the tracker's rect
// and offset.  The rect returned is relative to the curve strip.
// This method is used in the process dragging existing curves.
CRect CCurveStrip::GetUpdatedCurveRect(CCurveTracker& curveTracker, 
									   CCurveItem* pCurve,
									   int nAction)
{
	ASSERT(pCurve != NULL);

	CRect newRect;

	if (nAction == CTRK_DRAGGING) {
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
		m_pSequenceMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
		m_pSequenceMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

		// get the new curve bounds relative to the curve strip
		newRect = curveTracker.m_rect;
		newRect.OffsetRect(-rectStrip.TopLeft());
		newRect.OffsetRect(lLeftPosition, 0);
		newRect.OffsetRect(-curveTracker.m_nXOffset, 0);
	}

	// swap values of the rect if necessary
	if (pCurve->m_nEndValue > pCurve->m_nStartValue) {
		int temp = newRect.top;
		newRect.top = newRect.bottom;
		newRect.bottom = temp;
	}

	return newRect;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DragModify
//
// Handles move operation on the selected curves.
void CCurveStrip::DragModify(CCurveItem* pDMCurve, int nStripXPos, int nStripYPos, int nAction, bool fDuplicateCurves)
{
	// setup the status bar
	if( m_pSequenceMgr->m_pDMProdFramework )
	{
		m_pSequenceMgr->m_pDMProdFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar );
		if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
		{
			m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		}
		else
		{
			m_pSequenceMgr->m_pDMProdFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 18 );
		}
	}

	VARIANT var;
	m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	long lPartMaxXPos;
	m_pSequenceMgr->m_pTimeline->ClocksToPosition( CalcPartClockLength(), &lPartMaxXPos );

	long lXOffset = 0;
	CRect selRect = pDMCurve->m_rectFrame;
	CPoint point( nStripXPos, nStripYPos );

	while( selRect.PtInRect(point) == FALSE )
	{
		lXOffset += lPartMaxXPos;
		if( lXOffset > lTimelineClockLength )
		{
			ASSERT( 0 );	// Should not happen!
			break;
		}
		selRect.OffsetRect( lPartMaxXPos, 0 );
	}

	// convert point to window coordinates
	VERIFY( SUCCEEDED( m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &point) ) );
	
	// convert the frame rect of the curve to window coordinates
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
	VERIFY(SUCCEEDED(m_pSequenceMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
	
	// create a list of selected curves (not including the one clicked on)
	CTypedPtrList<CPtrList, CCurveItem*> curveList;
	CTypedPtrList<CPtrList, CCurveItem*> curveDuplicateList;

	if (nAction == CTRK_DRAGGING)
	{
		POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
		while( pos )
		{
			CCurveItem* pCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
			if(IsCurveForThisStrip(pCurve) && (pCurve != pDMCurve))
			{
				if (pCurve->m_fSelected)
				{
					curveList.AddTail(pCurve);

					if( fDuplicateCurves )
					{
						CCurveItem *pOldCurve = new CCurveItem;
						if( pOldCurve )
						{
							pOldCurve->Copy( pCurve );
							pOldCurve->m_fSelected = false;
							m_pSequenceMgr->InsertByAscendingTime( pOldCurve );
							curveDuplicateList.AddHead( pOldCurve );
						}
					}
				}
			}
		}
	}

	// Now, duplicate the curve that was clicked on (if necessary)
	if( fDuplicateCurves )
	{
		CCurveItem *pOldCurve = new CCurveItem;
		if( pOldCurve )
		{
			pOldCurve->Copy( pDMCurve );
			pOldCurve->m_fSelected = false;
			m_pSequenceMgr->InsertByAscendingTime( pOldCurve );
			curveDuplicateList.AddHead( pOldCurve );
		}
	}

	CWnd wnd;
	wnd.Attach(GetMyWindow());

	// create a tracker to handle the move
	CCurveTracker curveTracker(&wnd, m_pSequenceMgr->m_pTimeline, this, (short)nAction, &selRect);
	curveTracker.SetCurve(pDMCurve);
	curveTracker.SetCurveList(&curveList);
	curveTracker.SetXOffset( lXOffset );
	curveTracker.m_offset = 0;

	m_pCCurveTracker = &curveTracker;
	m_nTrackerAction = nAction;

	OnUpdateDragCurveValue(curveTracker, nAction);

	// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
	// We have to call ReleaseCapture(), or the CurveTracker won't work.
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
	::ReleaseCapture();

	int nResult = curveTracker.Track(&wnd, point, FALSE);

	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pSequenceMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	m_pCCurveTracker = NULL;
	m_nTrackerAction = 0;

	if( nResult )
	{
		CTypedPtrList<CPtrList, CCurveItem*> lstMovedCurves;

		curveList.AddTail(pDMCurve);

		while (!curveList.IsEmpty())
		{
			CCurveItem* pCurve = curveList.RemoveHead();
			
			CRect newRect = GetUpdatedCurveRect(curveTracker, pCurve, nAction);

			// figure out start and end values and positions
			long lStartClock, lEndClock;
			m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
			m_pSequenceMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
			short nStartValue = (short)YPosToValue(newRect.top);
			short nEndValue = (short)YPosToValue(newRect.bottom);

			// Save the old start time
			long lOldValue = pCurve->AbsTime();

			if( lStartClock < 0 )
			{
				pCurve->m_mtTime = 0;
				pCurve->m_nOffset = max( SHRT_MIN, lStartClock );
			}
			else
			{
				pCurve->m_mtTime = lStartClock;
				pCurve->m_nOffset = 0;
			}
			if (nAction != CTRK_DRAGGING)
			{
				pCurve->m_mtDuration = lEndClock - lStartClock;
			}

			pCurve->m_nStartValue = nStartValue;
			pCurve->m_nEndValue = nEndValue;

			// If the curve's start position changed, remove and re-add it to the list
			if( lOldValue != pCurve->AbsTime() )
			{
				// Get the curve's position in the curve list
				POSITION posCurve = m_pSequenceMgr->m_lstCurves.Find( pCurve );

				if( pCurve->AbsTime() < lOldValue )
				{
					// TODO: Optimize this, if possible
					// Move note backwards - remove it and re-add it later

					// Remove the event from the curve list
					if( posCurve )
					{
						m_pSequenceMgr->m_lstCurves.RemoveAt( posCurve );
					}

					// Add it to our private list
					lstMovedCurves.AddTail( pCurve );
				}

				// If moved forwards, only add to lstMovedSequences if we're now after
				// the item at pos.
				else
				{
					// Get the position of the next curve
					POSITION posNext = posCurve;
					if( posCurve )
					{
						m_pSequenceMgr->m_lstCurves.GetNext( posNext );
					}

					if( posNext
					&& (m_pSequenceMgr->m_lstCurves.GetAt( posNext )->AbsTime() < pCurve->AbsTime()) )
					{
						// Remove the event from the curve list
						m_pSequenceMgr->m_lstCurves.RemoveAt( posCurve );

						// Add it to our private list
						lstMovedCurves.AddTail( pCurve );
					}
				}
			}
		}

		// Merge back in any curves we moved
		while( !lstMovedCurves.IsEmpty() )
		{
			m_pSequenceMgr->InsertByAscendingTime( lstMovedCurves.RemoveHead() );
		}

		// Redraw CurveStrips
		RefreshCurveStrips();

		// Let the object know about the changes
		m_pSequenceMgr->m_fDirty = TRUE;
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_CHANGE_CURVE;
		m_pSequenceMgr->OnDataChanged(); 
	}
	else
	{
		// Remove the duplicate curves we inserted, since the user canceled with the 'Esc' key
		if( fDuplicateCurves )
		{
			while( !curveDuplicateList.IsEmpty() )
			{
				CCurveItem *pCurve = curveDuplicateList.RemoveHead();
				POSITION posCurve = m_pSequenceMgr->m_lstCurves.Find( pCurve );
				ASSERT( posCurve );
				m_pSequenceMgr->m_lstCurves.RemoveAt( posCurve );
				delete pCurve;
			}
		}
	}

	// Always update property page
	RefreshCurvePropertyPage();

	if( m_pSequenceMgr->m_pDMProdFramework )
	{
		m_pSequenceMgr->m_pDMProdFramework->RestoreStatusBar( m_hKeyStatusBar );				
		m_hKeyStatusBar = NULL;
	}
	
	wnd.Detach();
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnSingleCurve

HRESULT CCurveStrip::OnSingleCurve( void )
{
	ASSERT( m_pSequenceStrip != NULL );

	// Create Direct Music Curve
	CCurveItem* pDMSingleCurve = new CCurveItem;
	if( pDMSingleCurve == NULL )
	{
		return E_FAIL;
	}

	MUSIC_TIME mtSingleCurveEndTime;
	MUSIC_TIME mtCurveEndTime;
	
	WORD wRefreshUI = FALSE;

	POSITION pos2, pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );

		if( pDMCurve->m_fSelected )
		{
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				if( wRefreshUI == FALSE )
				{
					// First Curve
					pDMSingleCurve->Copy( pDMCurve );
					pDMSingleCurve->m_bCurveShape = DMUS_CURVES_SINE;
				}
				else
				{
					mtSingleCurveEndTime = pDMSingleCurve->AbsTime();
					if( pDMSingleCurve->m_bCurveShape != DMUS_CURVES_INSTANT )
					{
						mtSingleCurveEndTime += pDMSingleCurve->m_mtDuration;
					}
						
					
					mtCurveEndTime = pDMCurve->AbsTime();
					if( pDMCurve->m_bCurveShape != DMUS_CURVES_INSTANT )
					{
						mtCurveEndTime += pDMCurve->m_mtDuration;
					}
						
					// Make sure the pDMSingleCurve "start" fields represent the Curve
					// with the earliest start time
					if( pDMCurve->AbsTime() < pDMSingleCurve->AbsTime() )
					{
						pDMSingleCurve->m_mtTime = pDMCurve->m_mtTime;
						pDMSingleCurve->m_nOffset = pDMCurve->m_nOffset;
						pDMSingleCurve->m_nStartValue = pDMCurve->m_nStartValue;
						pDMSingleCurve->m_mtResetDuration = pDMCurve->m_mtResetDuration;
						pDMSingleCurve->m_nResetValue = pDMCurve->m_nResetValue;
						pDMSingleCurve->m_bFlags = pDMCurve->m_bFlags;
					}

					// Make sure the pDMSingleCurve "end" fields represent the Curve
					// with the latest end time
					if( mtCurveEndTime > mtSingleCurveEndTime )
					{
						pDMSingleCurve->m_mtDuration = mtCurveEndTime - pDMSingleCurve->AbsTime();
						if( pDMCurve->m_bCurveShape == DMUS_CURVES_INSTANT )
						{
							pDMSingleCurve->m_nEndValue = pDMCurve->m_nStartValue;
						}
						else
						{
							pDMSingleCurve->m_nEndValue = pDMCurve->m_nEndValue;
						}
					}
					else
					{
						pDMSingleCurve->m_mtDuration = mtSingleCurveEndTime - pDMSingleCurve->AbsTime();
					}
				}

				m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );
				delete pDMCurve;

				wRefreshUI = TRUE;
			}
		}
	}

	if( wRefreshUI == FALSE )
	{
		delete pDMSingleCurve;
		return E_FAIL;
	}

	// Reset Curve m_mtResetDuration to length of this Part
	if( pDMSingleCurve->m_bFlags & DMUS_CURVE_RESET )
	{
		pDMSingleCurve->m_mtResetDuration = CalcPartClockLength();
	}

	// Place Curve in Part's event list
	m_pSequenceMgr->InsertByAscendingTime( pDMSingleCurve );

	// Redraw CurveStrips
	RefreshCurveStrips();
	RefreshCurvePropertyPage();

	// Let the object know about the changes
	m_pSequenceStrip->m_nLastEdit = IDS_UNDO_SINGLE_CURVE;
	m_pSequenceMgr->OnDataChanged(); 

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteSelectedCurves

void CCurveStrip::DeleteSelectedCurves()
{
	WORD wRefreshUI = FALSE;

	POSITION pos2, pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );

		if( pDMCurve->m_fSelected )
		{
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );
				delete pDMCurve;

				wRefreshUI = TRUE;
			}
		}
	}

	if( wRefreshUI )
	{
		// Redraw CurveStrips
		RefreshCurveStrips();
		RefreshCurvePropertyPage();

		// Let the object know about the changes
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_DELETE_CURVE;
		m_pSequenceMgr->OnDataChanged(); 
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteAllCurves

WORD CCurveStrip::DeleteAllCurves()
{
	WORD wRefreshUI = FALSE;

	POSITION pos2, pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pDMCurve ) )
		{
			m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );
			delete pDMCurve;

			wRefreshUI = TRUE;
		}
	}

	return wRefreshUI;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnGutterSelectionChange

void CCurveStrip::OnGutterSelectionChange( BOOL fChanged )
{
	if( fChanged )
	{
		// Sync strip's gutter select flag in timeline
		if( m_pSequenceMgr->m_pTimeline )
		{
			VARIANT var;

			var.vt = VT_BOOL;
			V_BOOL(&var) = (short)m_pSequenceStrip->m_bGutterSelected;
			m_pSequenceMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_GUTTER_SELECTED, var );
		}
	}

	if( (m_pSequenceStrip->m_bSelectionCC != m_bCCType)
	||	(m_pSequenceStrip->m_wSelectionParamType != m_wRPNType) )
	{
		if( m_pSequenceStrip->m_lBeginSelect == m_pSequenceStrip->m_lEndSelect 
		||  m_pSequenceStrip->m_bGutterSelected == FALSE )
		{
			fChanged |= SelectAllCurves( FALSE );
		}
	}

	if( fChanged )
	{
		RefreshCurveStrips();
		RefreshCurvePropertyPage();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetEarliestSelectedCurve

CCurveItem* CCurveStrip::GetEarliestSelectedCurve( void )
{
	// Since curves are stored in order, we can just return the first one we find
	POSITION pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem* pDMCurve = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( pDMCurve->m_fSelected && IsCurveForThisStrip( pDMCurve ) )
		{
			return pDMCurve;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteCurvesBetweenTimes

void CCurveStrip::DeleteCurvesBetweenTimes( MUSIC_TIME mtEarliestCurve, MUSIC_TIME mtLatestCurve, long lTime )
{
	// Snap cursor time to earliest grid
	mtEarliestCurve = m_pSequenceStrip->FloorTimeToGrid( mtEarliestCurve + lTime, NULL );
	mtLatestCurve = m_pSequenceStrip->FloorTimeToGrid( mtLatestCurve + lTime, NULL );

	// Delete all curves that start int the last grid
	mtLatestCurve += m_pSequenceStrip->GetGridClocks( mtLatestCurve ) - 1;

	POSITION pos2, pos = m_pSequenceMgr->m_lstCurves.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CCurveItem *pItem = m_pSequenceMgr->m_lstCurves.GetNext( pos );
		if( IsCurveForThisStrip( pItem ) )
		{
			MUSIC_TIME mtCurveStart = pItem->AbsTime();
			if( (mtCurveStart >= mtEarliestCurve) &&
				(mtCurveStart <= mtLatestCurve) )
			{
				m_pSequenceMgr->m_lstCurves.RemoveAt( pos2 );
				delete pItem;
			}
		}
	}
}
