// CurveStrip.cpp : Implementation of CCurveStrip
#include "stdafx.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "CurveStrip.h"
#include "PropCurve.h"
#include "Tracker.h"
#include "ioDMStyle.h"
#include "DLLJazzDataObject.h"
#include <riffstrm.h>
#include "PropPageCurve.h"
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
// CCurveRectItem constructor/destructor

CCurveRectItem::CCurveRectItem( RECT* pRect )
{
	ASSERT( pRect != NULL );

	m_rect = *pRect;
}

CCurveRectItem::~CCurveRectItem()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip constructor/destructor

CCurveStrip::CCurveStrip( CMIDIMgr* pMIDIMgr, CPianoRollStrip* pPianoRollStrip, BYTE bCCType, WORD wRPNType )
{
	ASSERT( pMIDIMgr != NULL );
	ASSERT( pPianoRollStrip != NULL );

	// Initialize our reference count
	m_dwRef = 0;
	AddRef();

	m_pMIDIMgr = pMIDIMgr;
	m_pStripMgr = (IDMUSProdStripMgr *)pMIDIMgr;
	m_pStripMgr->AddRef();

	m_pPianoRollStrip = pPianoRollStrip;
	// m_pPianoRollStrip->AddRef();		intentionally missing

	m_bCCType = bCCType;
	m_wRPNType = wRPNType;

	m_hCursor = m_pPianoRollStrip->GetArrowCursor();
	m_hKeyStatusBar = NULL;

	m_fInsertingCurve = INSERT_CURVE_FALSE;
	m_nInsertingStartValue = 0;
	m_nInsertingEndValue = 0;
	m_lInsertingStartClock = 0;
	m_lInsertingEndClock = 0;

	m_pCCurveTracker = NULL;
	m_nTrackerAction = 0;

	m_pIFramework = NULL;

	if( m_pMIDIMgr->m_pTimeline )
	{
		VARIANT var;

		if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var ) ) )
		{
			m_pIFramework = (IDMUSProdFramework *)V_UNKNOWN( &var );
		}
	}
}

CCurveStrip::~CCurveStrip()
{
	// Clean up Property Sheet
	if( m_pIFramework )
	{
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
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

	if( m_pIFramework )
	{
		m_pIFramework->Release();
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
	m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
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
	if( FAILED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var ) ) )
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

		if( FAILED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
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
	
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &ptTop );
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &ptBottom );

	pRectStrip->top = ptTop.y;
	pRectStrip->bottom = ptBottom.y;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::IsCurveForThisStrip

BOOL CCurveStrip::IsCurveForThisStrip( CDirectMusicStyleCurve* pDMCurve )
{
	ASSERT( m_pPianoRollStrip != NULL );

	// Make sure Curve is for variations that should be displayed in this strip
	if( pDMCurve->m_dwVariation & m_pPianoRollStrip->m_dwVariations )
	{
		// Make sure Curve should be displayed in this strip
		if( m_pPianoRollStrip->CurveTypeToStripCCType(pDMCurve) == m_bCCType )
		{
			// If we are an RPN or NRPN, check that the wParamType matches
			if( ((m_bCCType == CCTYPE_RPN_CURVE_STRIP)
				 || (m_bCCType == CCTYPE_NRPN_CURVE_STRIP))
			&&	(pDMCurve->m_wParamType != m_wRPNType) )
			{
				return FALSE;
			}

			int nGridLength = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetGridLength();

			// Make sure curve is within Part
			if( pDMCurve->m_mtGridStart < nGridLength )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::IsAnyCurveSelected

BOOL CCurveStrip::IsAnyCurveSelected( void )
{
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( pDMEvent->m_fSelected )
		{
			if( IsCurveForThisStrip( (CDirectMusicStyleCurve *)pDMEvent ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::RefreshCurvePropertyPage	

void CCurveStrip::RefreshCurvePropertyPage( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pMIDIMgr
	&&  m_pMIDIMgr->m_pTimeline
	&&  m_pPianoRollStrip->m_fRefreshCurveProperties )
	{
		IDMUSProdStrip* pIActiveStrip;
		VARIANT var;

		// Get the active Strip
		if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
		{
			pIActiveStrip = (IDMUSProdStrip *)V_UNKNOWN( &var );
		}
		else
		{
			pIActiveStrip = NULL;
		}

		if( m_pIFramework )
		{
			IDMUSProdPropSheet* pIPropSheet;

			// Get IDMUSProdPropSheet interface
			if( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
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
		}

		if( pIActiveStrip )
		{
			pIActiveStrip->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::RefreshCurveStrips
	
void CCurveStrip::RefreshCurveStrips( void )
{
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );

	if( m_pMIDIMgr
	&&  m_pMIDIMgr->m_pTimeline )
	{
		CPianoRollStrip* pPianoRollStrip;
		CCurveStrip* pCurveStrip;

		POSITION pos = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
		while( pos )
		{
			pPianoRollStrip = m_pMIDIMgr->m_pPRSList.GetNext( pos );

			ASSERT( pPianoRollStrip->m_pPartRef != NULL );

			if( pPianoRollStrip->m_pPartRef->m_pDMPart == m_pPianoRollStrip->m_pPartRef->m_pDMPart )
			{
				POSITION pos2 = pPianoRollStrip->m_lstCurveStrips.GetHeadPosition();
				while( pos2 )
				{
					pCurveStrip = pPianoRollStrip->m_lstCurveStrips.GetNext( pos2 );

					if( pCurveStrip->m_bCCType == m_bCCType )
					{
						m_pMIDIMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, TRUE );
					}
				}
			}
		}
	}

	m_pMIDIMgr->UpdateStatusBarDisplay();
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

void CCurveStrip::DrawCurve( CDirectMusicStyleCurve* pDMCurve, HDC hDC,
							 RECT* pRectStrip, LONG lXOffset, MUSIC_TIME mtOffset )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	COLORREF crNoteColor = ::GetNearestColor( hDC, m_pPianoRollStrip->m_crUnselectedNoteColor );
	COLORREF crSelectedNoteColor = ::GetNearestColor( hDC, m_pPianoRollStrip->m_crSelectedNoteColor );

	if( pDMCurve->m_fSelected )
	{
		::SetBkColor( hDC, crSelectedNoteColor );
	}
	else
	{
		::SetBkColor( hDC, crNoteColor );
	}

	DWORD dwLastValue = 0xFFFFFFFF;
	DWORD dwValue;
	long lCurveStartClock;
	VARIANT var;
	CRect rect;

	lCurveStartClock = m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pDMCurve ) + mtOffset;

	// Get Strip height and center
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}
	int nStripCenter = pRectStrip->top + (nStripHeight >> 1);

	// Get MUSIC_TIME increment
	MUSIC_TIME mtIncrement;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( 1, &mtIncrement );

	// Ensure mtIncrement is at least one
	mtIncrement = max( 1, mtIncrement );

	// Initialize rectFrame and rectSelect
	pDMCurve->m_rectFrame.SetRectEmpty();
	pDMCurve->m_rectSelect.SetRectEmpty();

	pDMCurve->m_mtCurrent = 0;
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lCurveStartClock + pDMCurve->m_mtCurrent, &rect.right );

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
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lCurveStartClock + pDMCurve->m_mtCurrent, &rect.right );

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
			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			
			rect.right = nOrigRectRight;

			rect.left += lXOffset;
			rect.right += lXOffset;

			// Adjust rectSelect right, top, and bottom
			if( fFirstTime )
			{
				fFirstTime = FALSE;
				pDMCurve->m_rectSelect.SetRect( rect.left, rect.top, rect.right, rect.bottom );
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
	if( pDMCurve->m_rectSelect.Width() < CURVE_MIN_SIZE )
	{
		pDMCurve->m_rectSelect.right = pDMCurve->m_rectSelect.left + CURVE_MIN_SIZE;
	}
	if( pDMCurve->m_rectSelect.Height() < CURVE_MIN_SIZE )
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
	if( pDMCurve->m_rectFrame.Height() < CURVE_MIN_SIZE )
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
		VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
		VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
		
		CCurveTracker curveTracker(&selRect);
		curveTracker.Draw( CDC::FromHandle(hDC) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DrawMaximizedStrip_Callback

struct DrawMaximizedStripStruct
{
	BOOL	fHaveSelected;
	BOOL	fDrawSelected;
	long	lStripStartClock;
	long	lStripEndClock;
	HDC		hDC;
	RECT	*pRectStrip;
	long	lXOffset;
	CCurveRectList lstRects;
};

void CCurveStrip::DrawMaximizedStrip_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData )
{
	CCurveStrip *pCurveStrip = (CCurveStrip *)pThis;
	DrawMaximizedStripStruct *pDrawMaximizedStripStruct = (DrawMaximizedStripStruct *)pData;

	// Convert mtPartOffset to a pixel offset
	long lPixelOffset;
	pCurveStrip->m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtPartOffset, &lPixelOffset );

	// Set up work fields
	CRect rect;

	// DRAW UNSELECTED CURVES
	CDirectMusicEventItem* pDMEvent = pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		// Check if this curve is playable
		if( pDMEvent->m_mtGridStart >= lGridLength )
		{
			// no - go on to the next curve
			continue;
		}

		if( pDMEvent->m_fSelected != pDrawMaximizedStripStruct->fDrawSelected )
		{
			if( !pDrawMaximizedStripStruct->fDrawSelected
			&&	pDrawMaximizedStripStruct->fHaveSelected == FALSE )
			{
				if( pCurveStrip->IsCurveForThisStrip( (CDirectMusicStyleCurve *)pDMEvent ) )
				{
					pDrawMaximizedStripStruct->fHaveSelected = TRUE;
				}
			}
		}
		else
		{
			CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( pCurveStrip->IsCurveForThisStrip( pDMCurve ) )
			{
				// Make sure Curve is showing
				long lCurveStartClock = pPartRef->m_pDMPart->AbsTime( pDMCurve ) + mtPartOffset;
				long lCurveEndClock = lCurveStartClock + pDMCurve->m_mtDuration;

				if( lCurveEndClock >= pDrawMaximizedStripStruct->lStripStartClock
				&&	lCurveStartClock <= pDrawMaximizedStripStruct->lStripEndClock )
				{
					// Draw this Curve
					pCurveStrip->DrawCurve( pDMCurve, pDrawMaximizedStripStruct->hDC, pDrawMaximizedStripStruct->pRectStrip, pDrawMaximizedStripStruct->lXOffset, mtPartOffset );

					RECT rectSelect = pDMCurve->m_rectSelect;

					// If we're not in the very first copy of the part
					if( mtPartOffset > 0 )
					{
						// We need to offset the rectangles so they define where the curve is
						// in the first part offset.
						int nWidth;

						nWidth = pDMCurve->m_rectFrame.Width(); 
						pDMCurve->m_rectFrame.left -= lPixelOffset;
						pDMCurve->m_rectFrame.right = pDMCurve->m_rectFrame.left + nWidth;

						nWidth = pDMCurve->m_rectSelect.Width(); 
						pDMCurve->m_rectSelect.left -= lPixelOffset;
						pDMCurve->m_rectSelect.right = pDMCurve->m_rectSelect.left + nWidth;
					}

					// Frame intersecting rectangles
					CCurveRectItem* pCurveRect = pDrawMaximizedStripStruct->lstRects.GetHead(); 
					while( pCurveRect )
					{
						// Compute intersection with other Curves
						rect.IntersectRect( &pCurveRect->m_rect, &rectSelect );
										
						// Draw it
						if( rect.left != rect.right
						||  rect.top != rect.bottom )
						{
							rect.left -= pDrawMaximizedStripStruct->lXOffset;
							rect.right -= pDrawMaximizedStripStruct->lXOffset;
							::MoveToEx( pDrawMaximizedStripStruct->hDC, rect.left, rect.bottom, NULL );
							::LineTo( pDrawMaximizedStripStruct->hDC, rect.left, rect.top );
							::LineTo( pDrawMaximizedStripStruct->hDC, rect.right, rect.top );
							::LineTo( pDrawMaximizedStripStruct->hDC, rect.right, rect.bottom );
							::LineTo( pDrawMaximizedStripStruct->hDC, rect.left, rect.bottom );
						}
						pCurveRect = pCurveRect->GetNext();
					}

					// Store this Curve's rectangle
					pCurveRect = new CCurveRectItem( &rectSelect );
					if( pCurveRect )
					{
						pDrawMaximizedStripStruct->lstRects.AddHead( pCurveRect );
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DrawMaximizedStrip

HRESULT CCurveStrip::DrawMaximizedStrip( HDC hDC, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	//CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

	VARIANT var;

	// Store strip height
	int nStripHeight = CRV_DEFAULT_HEIGHT; 
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
	{
		nStripHeight = V_I4(&var);
	}
	ioCurveStripState* pCurveStripState = m_pPianoRollStrip->GetCurveStripState( m_bCCType, m_wRPNType );
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

	DrawMaximizedStripStruct drawMaximizedStripStruct;
	drawMaximizedStripStruct.fHaveSelected = FALSE;
	drawMaximizedStripStruct.fDrawSelected = FALSE;
	drawMaximizedStripStruct.hDC = hDC;
	//drawMaximizedStripStruct.lstRects = ;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( pRectClip->left + lXOffset, &drawMaximizedStripStruct.lStripStartClock );
	m_pMIDIMgr->m_pTimeline->PositionToClocks( pRectClip->right + lXOffset, &drawMaximizedStripStruct.lStripEndClock );
	drawMaximizedStripStruct.lXOffset = lXOffset;
	drawMaximizedStripStruct.pRectStrip = pRectStrip;

	CallFnForEachPart( m_pPianoRollStrip->m_pPartRef, DrawMaximizedStrip_Callback, this, &drawMaximizedStripStruct );
	drawMaximizedStripStruct.fDrawSelected = TRUE;
	CallFnForEachPart( m_pPianoRollStrip->m_pPartRef, DrawMaximizedStrip_Callback, this, &drawMaximizedStripStruct );

	if( penOld )
	{
		::SelectObject( hDC, penOld );
		::DeleteObject( penOverlappingLine );
	
	}

	while( drawMaximizedStripStruct.lstRects.GetHead() )
	{
		CCurveRectItem* pCurveRect = (CCurveRectItem *)drawMaximizedStripStruct.lstRects.RemoveHead();
		delete pCurveRect;
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
	if( m_pPianoRollStrip->m_lstCurveStripStates.IsEmpty() )
	{
		strText.LoadString( IDS_NO_CC_TRACKS_TEXT );
	}
	else
	{
		POSITION pos = m_pPianoRollStrip->m_lstCurveStripStates.GetHeadPosition();
		while( pos )
		{
			pCurveStripState = m_pPianoRollStrip->m_lstCurveStripStates.GetNext( pos );

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

	if( theApp.m_pCurveStripFont == NULL )
	{
		theApp.m_pCurveStripFont = new CFont;

		if( theApp.m_pCurveStripFont )
		{
			CString strFontName;
			if( !strFontName.LoadString( IDS_CURVE_STRIP_FONTNAME ) )
			{
				strFontName = _T("Arial");
			}
			if( theApp.m_pCurveStripFont->CreatePointFont( 80, strFontName, CDC::FromHandle(hDC) ) == FALSE )
			{
				delete theApp.m_pCurveStripFont;
				theApp.m_pCurveStripFont = NULL;
			}
		}
	}
	if( theApp.m_pCurveStripFont )
	{
		pFontOld = (HFONT)::SelectObject( hDC, theApp.m_pCurveStripFont->GetSafeHandle() );
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
// CCurveStrip::InvertGutterRange

void CCurveStrip::InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset )
{
	// Highlight the selected range (if there is one)
	if( m_pPianoRollStrip->m_fGutterSelected
	&&  m_pPianoRollStrip->m_lBeginTimelineSelection != m_pPianoRollStrip->m_lEndTimelineSelection )
	{
		if( m_pMIDIMgr->m_pTimeline )
		{
			RECT rectHighlight;

			rectHighlight.top = pRectClip->top;
			rectHighlight.bottom = pRectClip->bottom;

			m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_pPianoRollStrip->m_lBeginTimelineSelection > m_pPianoRollStrip->m_lEndTimelineSelection ?
													   m_pPianoRollStrip->m_lEndTimelineSelection : m_pPianoRollStrip->m_lBeginTimelineSelection,
													   &rectHighlight.left );

			m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_pPianoRollStrip->m_lBeginTimelineSelection > m_pPianoRollStrip->m_lEndTimelineSelection ?
													   m_pPianoRollStrip->m_lBeginTimelineSelection : m_pPianoRollStrip->m_lEndTimelineSelection,
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

HRESULT	STDMETHODCALLTYPE CCurveStrip::Draw( HDC hDC, STRIPVIEW /*sv*/, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	
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
	if( FAILED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
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
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

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
		m_pPianoRollStrip->DrawVerticalines( hDC, lXOffset );
		InvertGutterRange( hDC, &rectClip, lXOffset );
		DrawMaximizedStrip( hDC, &rectStrip, &rectClip, lXOffset );
	}

	// Compute the last time displayed
	long lStripEndClock;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &lStripEndClock );

	// If we are asked to draw a time that is beyond the end of our part, then we should loop
	if( m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength() < lStripEndClock )
	{
		// DRAW HATCH MARKS OVER REPEATING MEASURES
		long lRepeatClockLength = min( m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength(), m_pPianoRollStrip->m_pPartRef->m_pPattern->CalcLength() );
		long lTimelineClockLength;

		m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		lTimelineClockLength = V_I4(&var);
		lStripEndClock = min( lStripEndClock, lTimelineClockLength );

		if( (lRepeatClockLength < lTimelineClockLength)
		&&	(lRepeatClockLength < lStripEndClock) )
		{
			RECT rectRepeat;

			// Determine rectangle
			rectRepeat.top = rectClip.top;
			rectRepeat.bottom = rectClip.bottom;
			rectRepeat.right = rectClip.right;
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lRepeatClockLength, &rectRepeat.left );
			rectRepeat.left = max( rectRepeat.left - lXOffset, rectClip.left );
			
			// Draw it
			HBRUSH brushRepeatHatch = ::CreateHatchBrush( HS_DIAGCROSS, ::GetNearestColor(hDC, RGB(50,50,50)) );
			if( brushRepeatHatch )
			{
				int nOldBackgroundMode = ::SetBkMode( hDC, TRANSPARENT );
				HBRUSH brushOld = (HBRUSH)::SelectObject( hDC, brushRepeatHatch );
				::SetBkColor( hDC, 0 );
				::PatBlt( hDC, rectRepeat.left, rectRepeat.top, rectRepeat.right - rectRepeat.left, rectRepeat.bottom - rectRepeat.top, PATINVERT );
				::SetBkMode( hDC, nOldBackgroundMode );
				::SelectObject( hDC, brushOld );
				::DeleteObject( brushRepeatHatch );
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CCurveStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );

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
			   *pCLSID = CLSID_CurveStrip;
			}
			else
			{
				return E_FAIL;
			}
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
			if( m_pMIDIMgr )
			{
				m_pMIDIMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
			}
			else
			{
				V_UNKNOWN(pvar) = NULL;
			}
			break;

		case MSP_PIANOROLL_VARIATIONS:
		case MSP_PIANOROLL_TRACK:
		case MSP_PIANOROLL_GUID:
		case SP_EARLY_TIME:
		case SP_LATE_TIME:
			return m_pPianoRollStrip->GetStripProperty( sp, pvar );

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CCurveStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	WORD wRefreshUI = FALSE;

	switch( sp )
	{
		case SP_BEGINSELECT:
		case SP_ENDSELECT:
		case SP_GUTTERSELECT:
			m_pPianoRollStrip->SetStripProperty( sp, var );
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

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pPianoRollStrip != NULL );

	// Process the window message
	HRESULT hr = S_OK;

	if( m_pMIDIMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	switch( nMsg )
	{
		case WM_SETFOCUS:
			ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
			ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );
			if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP
			&&	(m_pMIDIMgr->m_pDMPattern->m_fInLoad == FALSE
				 || !m_pMIDIMgr->ParentNotePPGShowing()) )
			{
				RefreshCurvePropertyPage();
			}
			if( m_pMIDIMgr->m_pDMPattern->m_fInLoad == FALSE )
			{
				m_pMIDIMgr->UpdateOnDataChanged( NULL ); 
			}
			m_pPianoRollStrip->m_pActiveCurveStrip = this;
			m_pMIDIMgr->SetFocus(m_pPianoRollStrip,2);
			break;

		case WM_KILLFOCUS:
			m_pPianoRollStrip->m_pActiveCurveStrip = NULL;
			m_pMIDIMgr->KillFocus(m_pPianoRollStrip);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			hr = OnLButtonDown( wParam, lXPos, lYPos );
			break;

		case WM_LBUTTONUP:
			hr = OnLButtonUp();
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			hr = OnRButtonDown( wParam, lXPos, lYPos );
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp();
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

				case ID_EDIT_SELECT_ALL:
					hr = SelectAll();
					break;

				case ID_NEW_CC_TRACK:
					m_pPianoRollStrip->OnNewCurveStrip();
					break;

				case ID_DELETE_CC_TRACK:
					m_pPianoRollStrip->OnDeleteCurveStrip( this );
					break;

				case ID_EDIT_MERGE_VARIATIONS:
					m_pMIDIMgr->MergeVariations( m_pPianoRollStrip->m_pPartRef, m_pPianoRollStrip->m_dwVariations );
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
				m_pPianoRollStrip->OnChangeCurveStripView( SV_NORMAL );
			}
			else if( wParam == SIZE_MINIMIZED )
			{
				m_pPianoRollStrip->OnChangeCurveStripView( SV_MINIMIZED );
			}
			break;

		case WM_CREATE:
		{
			ASSERT( m_pMIDIMgr );
			ASSERT( m_pMIDIMgr->m_pTimeline );

			// Sync timeline's gutter select flag
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL( &var ) = (short)m_pPianoRollStrip->m_fGutterSelected;
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_GUTTER_SELECTED, var );

			// Set strip's positioning information
			DMUSPROD_TIMELINE_STRIP_INFO dmpTimelineStripInfo;
			dmpTimelineStripInfo.clsidType = CLSID_DirectMusicPatternTrack;
			dmpTimelineStripInfo.dwGroupBits = m_pMIDIMgr->m_dwGroupBits;
			dmpTimelineStripInfo.dwIndex = PChannelToStripIndex( m_pPianoRollStrip->m_pPartRef->m_dwPChannel );
			var.vt = VT_BYREF;
			V_BYREF(&var) = &dmpTimelineStripInfo;
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_STRIP_INFO, var );
			break;
		}

		default:
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::FBDraw

HRESULT CCurveStrip::FBDraw( HDC /*hDC*/, STRIPVIEW /*sv*/ )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::FBOnWMMessage

HRESULT CCurveStrip::FBOnWMMessage( UINT nMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPianoRollStrip != NULL );

	// Process the window message
	HRESULT hr = S_OK;

	switch( nMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			StopInsertingCurve();
			break;

		case WM_LBUTTONUP:
			hr = OnLButtonUp();
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp();
			break;

		case WM_MOUSEMOVE:
			hr = OnMouseMove( lXPos, lYPos );
			break;

		case WM_SETCURSOR:
			m_hCursor = m_pPianoRollStrip->GetArrowCursor();
			break;

		default:
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip CreateCopyStream

HRESULT CCurveStrip::CreateCopyStream( IStream** ppIStream, IStream** ppISeqStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIStream == NULL || ppISeqStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;
	*ppISeqStream = NULL;

	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

	IStream* pIStream;

	HRESULT hr = E_FAIL;
	if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Save the selected Curves into stream
		if( SUCCEEDED ( Save( pIStream ) ) )
		{
			*ppIStream = pIStream;
			(*ppIStream)->AddRef();
			IStream* pISeqStream;
			if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pISeqStream) ) )
			{
				// Save the selected Curves for seq track into stream
				if( SUCCEEDED ( SaveForSeqTrack( pISeqStream ) ) )
				{
					*ppISeqStream = pISeqStream;
					(*ppISeqStream)->AddRef();
					hr = S_OK;
				}
				pISeqStream->Release();
				//pISeqStream = NULL; // Not needed
			}
		}

		pIStream->Release();
		//pIStream = NULL; // Not needed
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

HRESULT CCurveStrip::Load( IStream* pIStream )
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
		ckMain.fccType = DMUS_FOURCC_CURVE_CLIPBOARD_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			// Assume that something will change
			m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

			hr = LoadDataObjectCurves( pIRiffStream, &ckMain );

			if( hr != E_ABORT )
			{
				// Redraw CurveStrips
				RefreshCurveStrips();
				RefreshCurvePropertyPage();

				// Let the object know about the changes
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PASTE_CURVE ); 
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
			ckMain.fccType = DMUS_FOURCC_CURVE_CLIPBOARD_FORM;

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
// CCurveStrip::Save

HRESULT CCurveStrip::SaveForSeqTrack( IStream* pIStream )
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

    HRESULT hr = E_FAIL;

	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			MMCKINFO ckMain;
			ckMain.fccType = FOURCC_SEQ_CURVE_CLIPBOARD_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveSelectedCurvesForSeqTrack( pIRiffStream ) )
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
// CCurveStrip::LoadFromSeqTrack

HRESULT CCurveStrip::LoadFromSeqTrack( IStream* pIStream )
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
			// Assume that something will change
			m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

			hr = LoadDataObjectCurvesFromSeqTrack( pIRiffStream, &ckMain );

			if( SUCCEEDED(hr) )
			{
				// Redraw CurveStrips
				RefreshCurveStrips();
				RefreshCurvePropertyPage();

				// Let the object know about the changes
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PASTE_CURVE ); 
			}
		}

		pIRiffStream->Release();
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::LoadDataObjectCurvesFromSeqTrack

HRESULT CCurveStrip::LoadDataObjectCurvesFromSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
    DWORD dwStructSize;
    DWORD dwExtra;

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	long lTime;
	long lCursorGrid;

	// Get cursor time
	if( FAILED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Change cursor time into a cursor grid
	lTime = m_pPianoRollStrip->ConvertAbsTimeToPartOffset( lTime );
	lCursorGrid = CLOCKS_TO_GRID( lTime, pDMPart );

	// turn off Curve select flags
	UnselectAllCurvesInPart();

	// Compute length of part
	long lLastGrid;
	lLastGrid = pDMPart->GetGridLength();

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK:
			{
				CDirectMusicStyleCurve* pDMCurve;
				DMUS_IO_CURVE_ITEM iDMCurveItem;
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

				DWORD dwOrigSize = dwSize;
				DWORD dwStreamPos = StreamTell( pIStream );

				// Find the extent of the curves
				MUSIC_TIME mtEarliestCurve = INT_MAX;
				MUSIC_TIME mtLatestCurve = INT_MIN;
				while( dwSize > 0 )
				{
					hr = pIStream->Read( &iDMCurveItem, dwStructSize, &dwByteCount );
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

					mtEarliestCurve = min( mtEarliestCurve, iDMCurveItem.mtStart + iDMCurveItem.nOffset );
					mtLatestCurve = max( mtLatestCurve, iDMCurveItem.mtStart + iDMCurveItem.nOffset + iDMCurveItem.mtDuration );
				}

				// Delete curves, if necessary
				if( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
				{
					// Delete between mtEarliestCurve and mtLatestCurve
					DeleteCurvesBetweenTimes( mtEarliestCurve, mtLatestCurve, lCursorGrid );
				}

				// Reset dwSize and seek pointer
				dwSize = dwOrigSize;
				StreamSeek( pIStream, dwStreamPos, STREAM_SEEK_SET );

				// Now read in the Curves
				while( dwSize > 0 )
				{
					hr = pIStream->Read( &iDMCurveItem, dwStructSize, &dwByteCount );
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
					pDMCurve = new CDirectMusicStyleCurve;
					if( pDMCurve )
					{
						//pDMCurve->m_mtGridStart = iDMCurveItem.mtGridStart;
						pDMCurve->m_dwVariation = m_pPianoRollStrip->m_dwVariations;
						pDMCurve->m_mtDuration = iDMCurveItem.mtDuration;
						pDMCurve->m_mtResetDuration = iDMCurveItem.mtResetDuration;
						pDMCurve->m_nTimeOffset = iDMCurveItem.nOffset;

						pDMCurve->m_nStartValue = iDMCurveItem.nStartValue;
						pDMCurve->m_nEndValue = iDMCurveItem.nEndValue;
						pDMCurve->m_nResetValue = iDMCurveItem.nResetValue;
						pDMCurve->m_bEventType = iDMCurveItem.bType;
						pDMCurve->m_bCurveShape = iDMCurveItem.bCurveShape;
						pDMCurve->m_bCCData = iDMCurveItem.bCCData;
						pDMCurve->m_bFlags = iDMCurveItem.bFlags;
						pDMCurve->m_wParamType = FILE_TO_MEMORY_WPARAMTYPE( iDMCurveItem.wParamType );
						pDMCurve->m_wMergeIndex = iDMCurveItem.wMergeIndex;

						// Convert from clocks to a grid value
						MUSIC_TIME mtTmpGrid = CLOCKS_TO_GRID( iDMCurveItem.mtStart, m_pPianoRollStrip->m_pPartRef->m_pDMPart );
						// Find out how much time is left over
						pDMCurve->m_nTimeOffset += iDMCurveItem.mtStart - GRID_TO_CLOCKS( mtTmpGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );
						// Store the grid value
						pDMCurve->m_mtGridStart = mtTmpGrid + lCursorGrid;

						pDMCurve->SetNext( NULL );

						// Do not add Curves past end of Part
						if( pDMCurve->m_mtGridStart > lLastGrid )
						{
							pDMCurve->SetNext( NULL );
							delete pDMCurve;
						}
						else
						{
							// Turn on select flag
							pDMCurve->m_fSelected = TRUE;

							// Place Curve in Part's event list
							m_pPianoRollStrip->m_pPartRef->m_pDMPart->InsertCurveInAscendingOrder( pDMCurve );
						}
					}
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::LoadDataObjectCurves

HRESULT CCurveStrip::LoadDataObjectCurves( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
    DWORD dwStructSize;
    DWORD dwExtra;

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	CDirectMusicCurveList lstCurves;
	DWORD dwVariationsOrigPianoRoll = 0;

	MUSIC_TIME mtEarliestCurve = INT_MAX;
	MUSIC_TIME mtLatestCurve = INT_MIN;

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK:
			{
				CDirectMusicStyleCurve* pDMCurve;
				DMUS_IO_STYLECURVE iDMStyleCurve;
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

				dwVariationsOrigPianoRoll = iDMCurveClipInfo.m_dwVariations;

				// Read size of the DMUS_IO_STYLECURVE structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				if( dwStructSize > sizeof( DMUS_IO_STYLECURVE ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLECURVE );
					dwStructSize = sizeof( DMUS_IO_STYLECURVE );
				}
				else
				{
					dwExtra = 0;
				}

				// Now read in the Curves
				while( dwSize > 0 )
				{
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
					pDMCurve = new CDirectMusicStyleCurve;
					if( pDMCurve )
					{
						//pDMCurve->m_mtGridStart = iDMStyleCurve.mtGridStart;
						pDMCurve->m_dwVariation = iDMStyleCurve.dwVariation;
						pDMCurve->m_mtDuration = iDMStyleCurve.mtDuration;
						pDMCurve->m_mtResetDuration = iDMStyleCurve.mtResetDuration;
						pDMCurve->m_nTimeOffset = iDMStyleCurve.nTimeOffset;

						pDMCurve->m_nStartValue = iDMStyleCurve.nStartValue;
						pDMCurve->m_nEndValue = iDMStyleCurve.nEndValue;
						pDMCurve->m_nResetValue = iDMStyleCurve.nResetValue;
						pDMCurve->m_bEventType = iDMStyleCurve.bEventType;
						pDMCurve->m_bCurveShape = iDMStyleCurve.bCurveShape;
						pDMCurve->m_bCCData = iDMStyleCurve.bCCData;
						pDMCurve->m_bFlags = iDMStyleCurve.bFlags;
						pDMCurve->m_wParamType = FILE_TO_MEMORY_WPARAMTYPE( iDMStyleCurve.wParamType );
						pDMCurve->m_wMergeIndex = iDMStyleCurve.wMergeIndex;

						// Convert from clocks to a grid value
						MUSIC_TIME mtTmpGrid = CLOCKS_TO_GRID( TS_GRID_TO_CLOCKS( iDMStyleCurve.mtGridStart, iDMCurveClipInfo.m_ts ), m_pPianoRollStrip->m_pPartRef->m_pDMPart );
						// Find out how much time is left over
						pDMCurve->m_nTimeOffset += TS_GRID_TO_CLOCKS( iDMStyleCurve.mtGridStart, iDMCurveClipInfo.m_ts ) - GRID_TO_CLOCKS( mtTmpGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );
						// Store the grid value
						pDMCurve->m_mtGridStart = mtTmpGrid;

						// Update the time of the earliest and latest curves
						mtLatestCurve = max( mtLatestCurve, pDMCurve->m_mtDuration + m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pDMCurve ) );
						mtEarliestCurve = min( mtEarliestCurve, m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pDMCurve ) );

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

	// Make sure copy and paste areas are for the same number of variations
	DWORD dwVariationMap[32];
	short nOrigVar, nCurVar, i;

	nCurVar = 0;
	memset( dwVariationMap, 0, sizeof(dwVariationMap) );

	for( nOrigVar = 0 ;  nOrigVar < 32 ;  nOrigVar++ )
	{
		if( dwVariationsOrigPianoRoll & (1 << nOrigVar) )
		{
			for( i = nCurVar ;  i < 32 ;  i++ )
			{
				if( m_pPianoRollStrip->m_dwVariations & (1 << i) )
				{
					dwVariationMap[nOrigVar] = (1 << i);
					nCurVar = (short)(i + 1);
					break;
				}
			}
			if( i == 32 )
			{
				AfxMessageBox( IDS_ERROR_VAR_MISMATCH );
				hr = E_ABORT;
				goto ON_ERROR;
			}
		}
	}

	CDirectMusicStyleCurve* pDMCurve;
	long lTime;
	long lCursorGrid;

	// Get cursor time
	if( FAILED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Change cursor time into a cursor grid
	lCursorGrid = (lTime / pDMPart->m_mtClocksPerBeat) * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
	lCursorGrid += (lTime % pDMPart->m_mtClocksPerBeat) / pDMPart->m_mtClocksPerGrid;

	if( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
	{
		// Delete between mtEarliestCurve and mtLatestCurve
		DeleteCurvesBetweenTimes( mtEarliestCurve, mtLatestCurve, lCursorGrid );
	}

	// turn off Curve select flags
	UnselectAllCurvesInPart();
	
	/// Paste Curves
	while( lstCurves.GetHead() )
	{
		pDMCurve = (CDirectMusicStyleCurve *)lstCurves.RemoveHead();

		hr = PasteCurve( pDMPart, pDMCurve, lCursorGrid, dwVariationMap );
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

HRESULT CCurveStrip::PasteCurve( CDirectMusicPart* pDMPart,
								 CDirectMusicStyleCurve* pDMCurve,
								 long lCursorGrid,
								 DWORD dwVariationMap[] )
{
	long lLastGrid = pDMPart->GetGridLength();
	
	// Make sure next pointer is set to NULL
	pDMCurve->SetNext( NULL );

	// Set GridStart
	pDMCurve->m_mtGridStart = pDMCurve->m_mtGridStart + lCursorGrid;
	
	// Do not add Curves past end of Part
	if( pDMCurve->m_mtGridStart > lLastGrid )
	{
		pDMCurve->SetNext( NULL );
		delete pDMCurve;
		return S_FALSE;
	}

	// Turn on select flag
	pDMCurve->m_fSelected = TRUE;

	// Set variations
	DWORD dwOrigVar = pDMCurve->m_dwVariation;
	pDMCurve->m_dwVariation = 0;
	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( dwOrigVar & (1 << i) )
		{
			pDMCurve->m_dwVariation |= dwVariationMap[i];
		}
	}

	// Place Curve in Part's event list
	pDMPart->InsertCurveInAscendingOrder( pDMCurve );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SaveSelectedCurves

HRESULT CCurveStrip::SaveSelectedCurves( IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicStyleCurve* pDMCurve;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLECURVE oDMStyleCurve;
	ioDMCurveClipInfo oDMCurveClipInfo;
	long lStartGrid;

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

	oDMCurveClipInfo.m_dwVariations = m_pPianoRollStrip->m_dwVariations;
	oDMCurveClipInfo.m_bCCType = m_bCCType;
	oDMCurveClipInfo.m_ts = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature;

	// Save ioDMCurveClipInfo structure
	hr = pIStream->Write( &oDMCurveClipInfo, sizeof(ioDMCurveClipInfo), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMCurveClipInfo) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_STYLECURVE structure
	dwStructSize = sizeof(DMUS_IO_STYLECURVE);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get the grid positin of the earliest selected curve
	CDirectMusicStyleCurve *pCurve;
	pCurve = GetEarliestSelectedCurve();
	if( pCurve )
	{
		lStartGrid = pCurve->m_mtGridStart;
	}
	else
	{
		lStartGrid = 0;
	}

	// Now save all of the curves
	pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( IsCurveForThisStrip( pDMCurve ) )
			{
				// Prepare DMUS_IO_STYLECURVE structure
				memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_STYLECURVE) );

				// Normalize the grid value of each curve to the earliest selected curve
				oDMStyleCurve.mtGridStart = pDMCurve->m_mtGridStart - lStartGrid;
				oDMStyleCurve.dwVariation = pDMCurve->m_dwVariation;
				oDMStyleCurve.nTimeOffset = pDMCurve->m_nTimeOffset;	

				oDMStyleCurve.mtDuration = pDMCurve->m_mtDuration;
				oDMStyleCurve.mtResetDuration = pDMCurve->m_mtResetDuration;
				oDMStyleCurve.nStartValue = pDMCurve->m_nStartValue;	
				oDMStyleCurve.nEndValue = pDMCurve->m_nEndValue;	
				oDMStyleCurve.nResetValue = pDMCurve->m_nResetValue;	
				oDMStyleCurve.bEventType = pDMCurve->m_bEventType;	
				oDMStyleCurve.bCurveShape = pDMCurve->m_bCurveShape;	
				oDMStyleCurve.bCCData = pDMCurve->m_bCCData;	
				oDMStyleCurve.bFlags = pDMCurve->m_bFlags;	
				oDMStyleCurve.wParamType = MEMORY_TO_FILE_WPARAMTYPE( pDMCurve->m_wParamType );
				oDMStyleCurve.wMergeIndex = pDMCurve->m_wMergeIndex;

				// Write DMUS_IO_STYLECURVE structure
				hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_STYLECURVE), &dwBytesWritten );
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(DMUS_IO_STYLECURVE) )
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
// CCurveStrip::SaveSelectedCurvesForSeqTrack

HRESULT CCurveStrip::SaveSelectedCurvesForSeqTrack( IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicStyleCurve* pDMCurve;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	ioDMCurveClipInfo oDMCurveClipInfo;
	long lStartGrid;

	IStream* pIStream;
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

	oDMCurveClipInfo.m_dwVariations = m_pPianoRollStrip->m_dwVariations;
	oDMCurveClipInfo.m_bCCType = m_bCCType;
	oDMCurveClipInfo.m_ts = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature;

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
	||	dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get the grid positin of the earliest selected curve
	CDirectMusicStyleCurve *pCurve;
	pCurve = GetEarliestSelectedCurve();
	if( pCurve )
	{
		lStartGrid = pCurve->m_mtGridStart;
	}
	else
	{
		lStartGrid = 0;
	}

	// Now save all of the curves
	pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;	pDMEvent ;	pDMEvent = pDMEvent->GetNext() )
	{
		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( IsCurveForThisStrip( pDMCurve ) )
			{
				// Prepare DMUS_IO_CURVE_ITEM structure
				DMUS_IO_CURVE_ITEM oDMStyleCurve;
				memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_CURVE_ITEM) );

				// Normalize the grid value of each curve to the earliest selected curve
				oDMStyleCurve.mtStart = GRID_TO_CLOCKS(pDMCurve->m_mtGridStart - lStartGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );
				oDMStyleCurve.mtDuration = pDMCurve->m_mtDuration;
				oDMStyleCurve.mtResetDuration = pDMCurve->m_mtResetDuration;
				oDMStyleCurve.dwPChannel = m_pPianoRollStrip->m_pPartRef->m_dwPChannel;
				oDMStyleCurve.nOffset = pDMCurve->m_nTimeOffset;	

				oDMStyleCurve.nStartValue = pDMCurve->m_nStartValue;	
				oDMStyleCurve.nEndValue = pDMCurve->m_nEndValue;	
				oDMStyleCurve.nResetValue = pDMCurve->m_nResetValue;	
				oDMStyleCurve.bType = pDMCurve->m_bEventType;	
				oDMStyleCurve.bCurveShape = pDMCurve->m_bCurveShape;	
				oDMStyleCurve.bCCData = pDMCurve->m_bCCData;	
				oDMStyleCurve.bFlags = pDMCurve->m_bFlags;	
				oDMStyleCurve.wParamType = MEMORY_TO_FILE_WPARAMTYPE( pDMCurve->m_wParamType );
				oDMStyleCurve.wMergeIndex = pDMCurve->m_wMergeIndex;	

				// Write DMUS_IO_CURVE_ITEM structure
				hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
				if( FAILED( hr )
				||	dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
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
	if( pITimelineDataObject && m_pPianoRollStrip->m_fGutterSelected && (m_pPianoRollStrip->m_lBeginTimelineSelection >= 0) && (m_pPianoRollStrip->m_lEndTimelineSelection > 0))
	{
		return S_OK;
	}

	if( CanCopy() == S_OK )
	{
		IStream* pIStream;
		IStream* pIStreamSeq;

		// Put the selected curves into an IDataObject
		if( SUCCEEDED ( CreateCopyStream( &pIStream, &pIStreamSeq ) ) )
		{
			// Merge with other strips
			if(pITimelineDataObject != NULL)
			{
				// add the stream to the passed ITimelineDataObject
				hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfCurve, pIStream );
				pIStream->Release();
				ASSERT( hr == S_OK );
				if ( hr != S_OK )
				{
					pIStreamSeq->Release();
					return E_FAIL;
				}

				hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfSeqCurves, pIStreamSeq );
				pIStreamSeq->Release();
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
					pIStreamSeq->Release();
					pDataObject->Release();
					return E_FAIL;
				}

				hr = pDataObject->AddClipFormat( theApp.m_cfSeqCurves, pIStreamSeq );
				pIStreamSeq->Release();
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

	m_pMIDIMgr->m_pTimeline->GetPasteType( &m_pMIDIMgr->m_ptPasteType );

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

	HRESULT hr = E_FAIL;

	// Read the Curve data
	if( pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfCurve ) == S_OK )
	{
		IStream* pIStream;

		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( theApp.m_cfCurve, &pIStream ) ) )
		{
			if( SUCCEEDED ( Load ( pIStream ) ) )
			{
				hr = S_OK;
			}
	
			pIStream->Release();
		}
	}
	else if( pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfSeqCurves ) == S_OK )
	{
		IStream* pIStream;

		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( theApp.m_cfSeqCurves, &pIStream ) ) )
		{
			if( SUCCEEDED ( LoadFromSeqTrack ( pIStream ) ) )
			{
				hr = S_OK;
			}
	
			pIStream->Release();
		}
	}
	// If this is a right-click paste and our gutter is selected
	else if( fInitialialDataObjectIsNull && m_pPianoRollStrip->m_fGutterSelected )
	{
		// Try and paste through the PianoRollStrip
		hr = m_pPianoRollStrip->Paste( pITimelineDataObject );
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
		if( hr != S_OK )
		{
			hr = pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfSeqCurves);
		}
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
		if( FAILED( m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject( &pIInternalTimelineDataObject ) ) )
		{
			pIDataObject->Release();
			return E_OUTOFMEMORY;
		}

		// Insert the IDataObject into the TimelineDataObject
		if( SUCCEEDED( pIInternalTimelineDataObject->Import( pIDataObject ) ) )
		{
			hr = pIInternalTimelineDataObject->IsClipFormatAvailable(theApp.m_cfCurve);
			if( hr != S_OK )
			{
				hr = pIInternalTimelineDataObject->IsClipFormatAvailable(theApp.m_cfSeqCurves);
			}
		}
		else
		{
			hr = E_FAIL;
		}

		pIInternalTimelineDataObject->Release();
		pIDataObject->Release();
	}

	// If our gutter is selected, and we haven't found a valid clipboard format
	if( m_pPianoRollStrip->m_fGutterSelected
	&&	(hr != S_OK) )
	{
		// Check if the PianoRollStrip can paste what's in the clipboard
		return m_pPianoRollStrip->CanPaste( pITimelineDataObject );
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

	if( m_bCCType == CCTYPE_MINIMIZED_CURVE_STRIP
	||  m_pPianoRollStrip->m_dwVariations == 0 )
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
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPianoRollStrip->m_dwVariations )
	{
		if( m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
		{
			if( m_pPianoRollStrip->GetFirstCurve() )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
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

	CPropCurve* pPropCurve = (CPropCurve *)*ppData;

	CDirectMusicStyleCurve* pDMCurve;
	int nNbrSelectedCurves = 0;

	CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

	long lClocksPerMeasure = pDMPart->m_mtClocksPerBeat * (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure;
	long lGridsPerMeasure = (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;

	CDirectMusicEventItem* pDMEvent = pDMPart->m_lstCurves.GetHead();

	if( m_fInsertingCurve )
	{
		pDMEvent = NULL;
		// Fill in Curve values
		CDirectMusicStyleCurve tmpCurve;
		long lClocksPerMeasure = pDMPart->m_mtClocksPerBeat * (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure;
		long lGridsPerMeasure = (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;

		long lGridStart = (m_lInsertingStartClock / lClocksPerMeasure) * lGridsPerMeasure;
		long lRemainder = m_lInsertingStartClock % lClocksPerMeasure;
		lGridStart += (lRemainder / pDMPart->m_mtClocksPerBeat) * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
		lRemainder  = lRemainder % pDMPart->m_mtClocksPerBeat;
		lGridStart += lRemainder / pDMPart->m_mtClocksPerGrid;
		lRemainder  = lRemainder % pDMPart->m_mtClocksPerGrid;

		tmpCurve.m_mtGridStart = lGridStart;
		tmpCurve.m_nTimeOffset = (short)lRemainder;
		tmpCurve.m_dwVariation = m_pPianoRollStrip->m_dwVariations;
		tmpCurve.m_mtDuration = max( 1, m_lInsertingEndClock - m_lInsertingStartClock );

		tmpCurve.m_nStartValue = m_nInsertingStartValue;
		tmpCurve.m_nEndValue = m_nInsertingEndValue;
		tmpCurve.m_bEventType = StripCCTypeToCurveType( m_bCCType );
		if( tmpCurve.m_bEventType == DMUS_CURVET_CCCURVE )
		{
			tmpCurve.m_bCCData = m_bCCType;
		}

		tmpCurve.m_wParamType = m_wRPNType;

		tmpCurve.SetDefaultResetValues( CalcPartClockLength() );

		pPropCurve->GetValuesFromDMCurve( pDMPart, &tmpCurve );
		nNbrSelectedCurves = 1;
	}

	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( IsCurveForThisStrip( pDMCurve ) )
			{
				CDirectMusicStyleCurve newCurve( pDMCurve );
				if( m_pCCurveTracker )
				{
					CRect newRect = GetUpdatedCurveRect( *m_pCCurveTracker, &newCurve, m_nTrackerAction );

					// figure out start and end values and positions
					long lStartClock, lEndClock;
					m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
					m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
					short nStartValue = (short)YPosToValue(newRect.top);
					short nEndValue = (short)YPosToValue(newRect.bottom);

					// update the curves values
					long lGridStart = (lStartClock / lClocksPerMeasure) * lGridsPerMeasure;
					long lRemainder = lStartClock % lClocksPerMeasure;
					lGridStart += (lRemainder / pDMPart->m_mtClocksPerBeat) * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					lRemainder  = lRemainder % pDMPart->m_mtClocksPerBeat;
					lGridStart += lRemainder / pDMPart->m_mtClocksPerGrid;
					lRemainder  = lRemainder % pDMPart->m_mtClocksPerGrid;

					ASSERT( lRemainder <= SHRT_MAX );
					newCurve.m_mtGridStart = lGridStart;
					newCurve.m_nTimeOffset = (short)lRemainder;

					if (m_nTrackerAction != CTRK_DRAGGING)
					{
						newCurve.m_mtDuration = lEndClock - lStartClock;
					}

					newCurve.m_nStartValue = nStartValue;
					newCurve.m_nEndValue = nEndValue;
				}

				if( nNbrSelectedCurves == 0 )
				{
					pPropCurve->GetValuesFromDMCurve( pDMPart, &newCurve );
				}
				else
				{
					CPropCurve propcurve;

					propcurve.GetValuesFromDMCurve( pDMPart, &newCurve );

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

	CPropCurve* pPropCurve = (CPropCurve *)pData;

	// Assume that something will change
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	CDirectMusicStyleCurve* pDMCurve;
	DWORD dwChanged = 0;

	CDirectMusicPart *pPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

	CDirectMusicCurveList lstMovedEvents;

	CDirectMusicEventItem* pDMEvent = pPart->m_lstCurves.GetHead();
	while( pDMEvent )
	{
		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( IsCurveForThisStrip( pDMCurve ) )
			{
				// If not changing variations, make a copy of the note
				if( !(pPropCurve->m_dwChanged & CHGD_VARIATIONS) )
				{
					pPart->CreateCurveCopyIfNeeded( pDMCurve, m_pPianoRollStrip->m_dwVariations, FALSE );
				}

				// Save the old start time
				long lOldValue = pPart->AbsTime( pDMCurve );

				dwChanged |= pPropCurve->ApplyValuesToDMCurve( pPart,
															   pDMCurve );

				// If the curve's start position changed, remove and re-add it to the list
				if( lOldValue != pPart->AbsTime( pDMEvent ) )
				{
					// Save a pointer to the next event
					CDirectMusicEventItem *pDMEvent2 = pDMEvent->GetNext();

					if( pPart->AbsTime( pDMEvent ) < lOldValue )
					{
						// TODO: Optimize this, if possible
						// Move note backwards - remove it and re-add it later

						// Remove the event from the part's list
						pPart->m_lstCurves.Remove( pDMEvent );

						// Add it to our private list
						lstMovedEvents.AddTail( pDMEvent );
					}

					// If moved forwards, only add to lstMovedSequences if we're now after
					// the item at pos.
					else
					{
						if( pDMEvent2
						&& (pPart->AbsTime( pDMEvent2 ) < pPart->AbsTime(pDMEvent)) )
						{
							// Remove the event from the part's list
							pPart->m_lstCurves.Remove( pDMEvent );

							// Add it to our private list
							lstMovedEvents.AddTail( pDMEvent );
						}
					}
					// Set the next pDMEvent
					pDMEvent = pDMEvent2;
				}
				else
				{
					pDMEvent = pDMEvent->GetNext();
				}
			}
			else
			{
				pDMEvent = pDMEvent->GetNext();
			}
		}
		else
		{
			pDMEvent = pDMEvent->GetNext();
		}
	}

	// Merge back in any curves we moved
	if( lstMovedEvents.GetHead() )
	{
		pPart->MergeCurveList( &lstMovedEvents );
	}

	// Always refresh property page. (If user tries to change duration to value less than 1,
	// we need to reset the property page to valid values)
	RefreshCurvePropertyPage();

	if( dwChanged )
	{
		// Redraw CurveStrips
		RefreshCurveStrips();

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_CHANGE_CURVE ); 
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnShowProperties

HRESULT CCurveStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStripMgr != NULL );

	if( m_pIFramework == NULL )
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

	if( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
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
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	return m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength();
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetCurveFromPoint_Callback

struct GetCurveStruct
{
	BOOL	bFrameOnly;
	BOOL	fOnTheCurveHandles;
	POINT	point;
	CDirectMusicStyleCurve* pTheDMCurve;
	RECT	*pTheDMCurveRect;
};

void CCurveStrip::GetCurveFromPoint_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData )
{
	GetCurveStruct *pGetCurveStruct = (GetCurveStruct *)pData;

	CCurveStrip *pCurveStrip = (CCurveStrip *)pThis;

	// Offset the point we're looking for (rather than offsetting each curve)
	long lStartPointOffset;
	VERIFY( SUCCEEDED ( pCurveStrip->m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtPartOffset, &lStartPointOffset ) ) );
	POINT pointToFind;
	pointToFind.y = pGetCurveStruct->point.y;
	pointToFind.x = pGetCurveStruct->point.x - lStartPointOffset;

	CDirectMusicEventItem* pDMEvent = pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		CDirectMusicStyleCurve *pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;
		
		if( (pDMCurve->m_mtGridStart < lGridLength)
		&&	pCurveStrip->IsCurveForThisStrip( pDMCurve ) )
		{
			CRect rectCurve = pGetCurveStruct->bFrameOnly ? pDMCurve->m_rectFrame : pDMCurve->m_rectSelect;

			if( rectCurve.PtInRect( pointToFind ) )
			{
				// Need to set fOnCurveHandles
				BOOL fOnCurveHandles = FALSE;
				if( pDMCurve->m_fSelected )
				{
					UINT nHit = pCurveStrip->HitTest( pDMCurve, pointToFind.x, pointToFind.y );
				
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

				if( pGetCurveStruct->pTheDMCurve == NULL )
				{
					pGetCurveStruct->pTheDMCurve = pDMCurve;
					pGetCurveStruct->fOnTheCurveHandles = fOnCurveHandles;
					if( pGetCurveStruct->pTheDMCurveRect )
					{
						*(pGetCurveStruct->pTheDMCurveRect) = pDMCurve->m_rectFrame;
						pGetCurveStruct->pTheDMCurveRect->left += lStartPointOffset;
						pGetCurveStruct->pTheDMCurveRect->right += lStartPointOffset;
					}
				}
				else
				{
					if( pGetCurveStruct->fOnTheCurveHandles == FALSE )
					{
						if( fOnCurveHandles == TRUE )
						{
							// Use the Curve whose handle is being hit
							pGetCurveStruct->pTheDMCurve = pDMCurve;
							pGetCurveStruct->fOnTheCurveHandles = TRUE;
							if( pGetCurveStruct->pTheDMCurveRect )
							{
								*(pGetCurveStruct->pTheDMCurveRect) = pDMCurve->m_rectFrame;
								pGetCurveStruct->pTheDMCurveRect->left += lStartPointOffset;
								pGetCurveStruct->pTheDMCurveRect->right += lStartPointOffset;
							}
						}
						else
						{
							// Use the Curve with the narrowest width
							if( pDMCurve->m_rectSelect.Width() < pGetCurveStruct->pTheDMCurve->m_rectSelect.Width() ) 
							{
								pGetCurveStruct->pTheDMCurve = pDMCurve;
								if( pGetCurveStruct->pTheDMCurveRect )
								{
									*(pGetCurveStruct->pTheDMCurveRect) = pDMCurve->m_rectFrame;
									pGetCurveStruct->pTheDMCurveRect->left += lStartPointOffset;
									pGetCurveStruct->pTheDMCurveRect->right += lStartPointOffset;
								}
							}
						}
					}
					else
					{
						if( fOnCurveHandles == TRUE )
						{
							// Use the Curve with the narrowest width
							if( pDMCurve->m_rectSelect.Width() < pGetCurveStruct->pTheDMCurve->m_rectSelect.Width() ) 
							{
								pGetCurveStruct->pTheDMCurve = pDMCurve;
								if( pGetCurveStruct->pTheDMCurveRect )
								{
									*(pGetCurveStruct->pTheDMCurveRect) = pDMCurve->m_rectFrame;
									pGetCurveStruct->pTheDMCurveRect->left += lStartPointOffset;
									pGetCurveStruct->pTheDMCurveRect->right += lStartPointOffset;
								}
							}
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::GetCurveFromPoint

CDirectMusicStyleCurve*	CCurveStrip::GetCurveFromPoint( int nStripXPos, int nStripYPos, BOOL bFrameOnly, RECT *pRectCurve )
{
	GetCurveStruct getCurveStruct;
	getCurveStruct.bFrameOnly = bFrameOnly;
	getCurveStruct.point.x = nStripXPos;
	getCurveStruct.point.y = nStripYPos;
	getCurveStruct.fOnTheCurveHandles = FALSE;
	getCurveStruct.pTheDMCurve = NULL;
	getCurveStruct.pTheDMCurveRect = pRectCurve;

	CallFnForEachPart( m_pPianoRollStrip->m_pPartRef, GetCurveFromPoint_Callback, this, &getCurveStruct );

	return getCurveStruct.pTheDMCurve;
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectCurve

WORD CCurveStrip::SelectCurve( CDirectMusicStyleCurve* pTheDMCurve, BOOL fState )
{
	CDirectMusicStyleCurve* pDMCurve;
	WORD wRefreshUI = FALSE;

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;
		
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

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( IsCurveForThisStrip( (CDirectMusicStyleCurve *)pDMEvent ) )
		{
			if( pDMEvent->m_fSelected != fState )
			{
				pDMEvent->m_fSelected = fState;

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
	CDirectMusicStyleCurve* pDMCurve;
	WORD wRefreshUI = FALSE;

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;
	
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
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	if( lBegin == lEnd )
	{
		return FALSE;
	}

	CDirectMusicStyleCurve* pDMCurve;
	CRect rectSelect1;
	CRect rectSelect2;
	WORD wRefreshUI = FALSE;

	long lPartClockLength = CalcPartClockLength();
	long lBeginClock = lBegin > lEnd ? lEnd : lBegin;
	long lEndClock = lBegin > lEnd ? lBegin : lEnd;

	if( (lEndClock - lBeginClock) >= lPartClockLength )
	{
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( 0, &rectSelect1.left );
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( lPartClockLength, &rectSelect1.right );
		rectSelect2 = rectSelect1;
	}
	else
	{
		lBegin = /*AdjustClock*/( lBeginClock );
		lEnd = /*AdjustClock*/( lEndClock );

		if( lBegin <= lEnd )
		{
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lBegin, &rectSelect1.left );
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lEnd, &rectSelect1.right );
			rectSelect2 = rectSelect1;
		}
		else
		{
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lBegin, &rectSelect1.left );
			rectSelect1.right = lPartClockLength;
			rectSelect2.left = 0;
			m_pMIDIMgr->m_pTimeline->ClocksToPosition( lEnd, &rectSelect2.right );
		}
	}

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;
		
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
// CCurveStrip::SelectCurvesInRect_Callback

struct SelectCurvesInRectStruct
{
	RECT *pRect;
	CDirectMusicStyleCurve* pDMCurve;
};

void CCurveStrip::SelectCurvesInRect_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData )
{
	UNREFERENCED_PARAMETER( pPartRef );

	SelectCurvesInRectStruct *pSelectCurvesInRectStruct = (SelectCurvesInRectStruct *)pData;

	if( pSelectCurvesInRectStruct->pDMCurve->m_mtGridStart >= lGridLength )
	{
		return;
	}

	// Offset the rect we're looking for (rather than offsetting the curve)
	long lStartPointOffset;
	CCurveStrip *pCurveStrip = (CCurveStrip *)pThis;
	VERIFY( SUCCEEDED ( pCurveStrip->m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtPartOffset, &lStartPointOffset ) ) );
	RECT rectSelect = *pSelectCurvesInRectStruct->pRect;
	rectSelect.left -= lStartPointOffset;
	rectSelect.right -= lStartPointOffset;

	RECT rectIntersect;
	if( ::IntersectRect( &rectIntersect, &pSelectCurvesInRectStruct->pDMCurve->m_rectSelect, &rectSelect ) )
	{
		if( pSelectCurvesInRectStruct->pDMCurve->m_rectSelect.left >= rectSelect.left
		&&	pSelectCurvesInRectStruct->pDMCurve->m_rectSelect.left <= rectSelect.right )
		{
			pSelectCurvesInRectStruct->pDMCurve->m_fSelected = TRUE;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// CCurveStrip::SelectCurvesInRect

WORD CCurveStrip::SelectCurvesInRect( CRect* pRect )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	WORD wRefreshUI = FALSE;

	SelectCurvesInRectStruct selectCurvesInRectStruct;
	selectCurvesInRectStruct.pRect = pRect;

	CDirectMusicEventItem* pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

		if( IsCurveForThisStrip( pDMCurve ) )
		{
			selectCurvesInRectStruct.pDMCurve = pDMCurve;

			BOOL fOrigState = pDMCurve->m_fSelected;
			pDMCurve->m_fSelected = FALSE;
			CallFnForEachPart( m_pPianoRollStrip->m_pPartRef, SelectCurvesInRect_Callback, this, &selectCurvesInRectStruct );
			if( pDMCurve->m_fSelected != fOrigState )
			{
				wRefreshUI = TRUE;
			}
		}
	}
	return wRefreshUI;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::UnselectGutterRange

void CCurveStrip::UnselectGutterRange( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_fSelecting = TRUE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC = m_bCCType;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_wSelectionParamType = m_wRPNType;
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_fSelecting = FALSE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC = 0xFF;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_wSelectionParamType = 0xFFFF;
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip Additional methods

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::HitTest_Callback

struct HitTestStruct
{
	CCurveTracker *pCurveTracker;
	POINT	point;
	long	lGridStart;
	UINT	uiHit;
};

void CCurveStrip::HitTest_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData )
{
	UNREFERENCED_PARAMETER( lGridLength );
	UNREFERENCED_PARAMETER( pPartRef );

	HitTestStruct *pHitTestStruct = (HitTestStruct *)pData;

	if( (pHitTestStruct->uiHit == 0xFFFFFFFF)
	&&	(pHitTestStruct->lGridStart < lGridLength) )
	{
		// Offset the point we're looking for (rather than offsetting each curve)
		long lStartPointOffset;
		CCurveStrip *pCurveStrip = (CCurveStrip *)pThis;
		VERIFY( SUCCEEDED ( pCurveStrip->m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtPartOffset, &lStartPointOffset ) ) );
		POINT pointToFind;
		pointToFind.y = pHitTestStruct->point.y;
		pointToFind.x = pHitTestStruct->point.x - lStartPointOffset;

		// Check to see what we hit
		pHitTestStruct->uiHit = pHitTestStruct->pCurveTracker->HitTest( pointToFind );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::HitTest

UINT CCurveStrip::HitTest( CDirectMusicStyleCurve* pDMCurve, long lStripXPos, long lStripYPos )
{
	// convert the frame rect of the curve to window coordinates
	CRect selRect = pDMCurve->m_rectFrame;
	//VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
	//VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
	
	// create a tracker and find out if we are on a handle or somewhere within the curve
	CCurveTracker curveTracker(&selRect);

	POINT point;
	point.x = lStripXPos;
	point.y = lStripYPos;
	//VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &point)));

	HitTestStruct hitTestStruct;
	hitTestStruct.pCurveTracker = &curveTracker;
	hitTestStruct.point = point;
	hitTestStruct.uiHit = 0xFFFFFFFF;
	hitTestStruct.lGridStart = pDMCurve->m_mtGridStart;

	CallFnForEachPart( m_pPianoRollStrip->m_pPartRef, HitTest_Callback, this, &hitTestStruct );

	return hitTestStruct.uiHit;
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

	// Get a pointer to the Timeline
	if( m_pMIDIMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	
	RECT rectCurve;
	CDirectMusicStyleCurve* pDMCurve = GetCurveFromPoint( lXPos, lYPos, TRUE, &rectCurve );
	WORD wRefreshUI = FALSE;
	
	if (pDMCurve) {		// have we clicked on a curve?
		if (pDMCurve->m_fSelected) {	// curve selected?
			UINT hit = HitTest( pDMCurve, lXPos, lYPos );

			switch(hit) {
			case CRectTracker::hitMiddle:
				DragModify(pDMCurve, lXPos, lYPos, &rectCurve, CTRK_DRAGGING, (wParam & MK_CONTROL) ? true : false);
				break;
			default:
				wRefreshUI |= SelectAllCurves(FALSE);
				wRefreshUI |= SelectCurve(pDMCurve, TRUE);
				RefreshUI(wRefreshUI);
				DragModify(pDMCurve, lXPos, lYPos, &rectCurve, CTRK_DRAGGING_EDGE, false);
			}
		}
		else {
			
			if (!(wParam & MK_CONTROL)) {
				wRefreshUI |= SelectAllCurves(FALSE);
			}

			wRefreshUI |= SelectCurve(pDMCurve, TRUE);
			RefreshUI(wRefreshUI);
			DragModify(pDMCurve, lXPos, lYPos, &rectCurve, CTRK_DRAGGING, false);
		}
		RefreshUI(wRefreshUI);
	}
	else {
		
		// user might have clicked on the curve outside of the frame
		// in which case we select the curve only
		CDirectMusicStyleCurve* pDMCurve = GetCurveFromPoint( lXPos, lYPos, FALSE );
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

				// Switch to curve strip property page
				RefreshCurvePropertyPage();

				// Create bounding box
				IOleWindow* pIOleWindow;
				m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
				if( pIOleWindow )
				{
					CWnd wnd;
					HWND hWnd;

					if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
					{
						CPoint point( lXPos, lYPos );

						if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &point ) ) )
						{
							wnd.Attach( hWnd );
							CCurveTracker curveTracker( &wnd, m_pMIDIMgr->m_pTimeline, this, CTRK_SELECTING );
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

	// Get a pointer to the Timeline
	if( m_pMIDIMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	// Handle Curve selection
	CDirectMusicStyleCurve* pDMCurve = GetCurveFromPoint( lXPos, lYPos );

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

	HRESULT hr = OnContextMenu();

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
		m_hCursor = m_pPianoRollStrip->GetArrowCursor();
		return S_OK;
	}

	CDirectMusicStyleCurve* pDMCurve = GetCurveFromPoint( lXPos, lYPos, TRUE );

	if (pDMCurve)
	{
		if (pDMCurve->m_fSelected)
		{
			UINT hit = HitTest( pDMCurve, lXPos, lYPos );
		
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

	if( m_pMIDIMgr->m_pTimeline )
	{
		HMENU hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_CURVE_RMENU));
		if( hMenu )
		{
			HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
			if( hMenuPopup )
			{
				EnableMenuItem( hMenuPopup, ID_EDIT_MERGE_VARIATIONS,  m_pPianoRollStrip->m_dwVariations ? MF_ENABLED : MF_GRAYED );

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
					EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ?
									MF_ENABLED : MF_GRAYED );
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
					m_pMIDIMgr->m_pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
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
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
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
	if( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_HEIGHT, &var ) ) )
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

	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );
	
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
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWndLeft );
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWndRight );

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
		m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lClockLength = V_I4(&var);

		// Make sure ptLeft.x is in this CurveStrip
		long lStartClock;

		m_pMIDIMgr->m_pTimeline->PositionToClocks( ptLeft.x, &lStartClock );
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
				m_pMIDIMgr->m_pTimeline->PositionToClocks( ptRight.x, &lEndClock );
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
	if( m_pIFramework )
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		m_pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
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
	m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
	m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
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
	if( m_pIFramework )
	{
		BSTR bstrStatus = strStatus.AllocSysString();
		m_pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstrStatus, TRUE );
	}

	// Refresh property page
	RefreshCurvePropertyPage();
}

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::StartInsertingCurve

void CCurveStrip::StartInsertingCurve( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

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
	m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	if( m_pIFramework )
	{
		if( m_hKeyStatusBar )
		{
			m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );				
			m_hKeyStatusBar = NULL;
		}

		m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar  );
		if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
		{
			m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		}
		else
		{
			m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 18 );
		}
	}

	CPoint pt( -1, -1 );
	OnUpdateInsertCurveValue( pt, pt );
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::StopInsertingCurve

void CCurveStrip::StopInsertingCurve( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	if( m_fInsertingCurve )
	{
		if( m_fInsertingCurve == INSERT_CURVE_START_VALUE )
		{
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		}

		m_fInsertingCurve = INSERT_CURVE_FALSE;

		if( m_pIFramework )
		{
			m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );	
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

	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	// Get CurveStrip rect
	CRect rectStrip;
	if (!GetStripRect(rectStrip))
	{
		return;
	}

	// Convert points into Window coordinates
	CPoint ptWnd( lXPos, lYPos );
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &ptWnd );

	// Make sure YPos is in this CurveStrip
	if( ptWnd.y < rectStrip.top
	||  ptWnd.y > rectStrip.bottom )
	{
		// YPos is NOT in this CurveStrip
		return;
	}

	// Make sure XPos is in this CurveStrip
	VARIANT var;
	m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lClockLength = V_I4(&var);
	long lXPosClock;

	m_pMIDIMgr->m_pTimeline->PositionToClocks( lXPos, &lXPosClock );
	if( lXPosClock < 0
	||  lXPosClock > lClockLength
	||  ptWnd.x < rectStrip.left 
	||  ptWnd.x > rectStrip.right )
	{
		// XPos is NOT in this CurveStrip
		return;
	}

	IOleWindow* pIOleWindow;

	m_pMIDIMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow );
	if( pIOleWindow )
	{
		CWnd wnd;
		HWND hWnd;

		if( pIOleWindow->GetWindow( &hWnd ) == S_OK )
		{
			CPoint point( lXPos, lYPos );

			if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &point ) ) )
			{
				// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
				// We have to call ReleaseCapture(), or the CurveTracker won't work.
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
				::ReleaseCapture();

				m_fInsertingCurve = INSERT_CURVE_END_VALUE;

				wnd.Attach( hWnd );
				CCurveTracker curveTracker( &wnd, m_pMIDIMgr->m_pTimeline, this, CTRK_INSERTING );
				if( curveTracker.TrackRubberBand( &wnd, point, TRUE ) )
				{
					// Create Direct Music Curve
					CDirectMusicStyleCurve* pCurve = new CDirectMusicStyleCurve;
					if( pCurve )
					{
						// Update the Part's GUID (and create a new Part if we need to)
						m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

						CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

						// Fill in Curve values
						long lClocksPerMeasure = pDMPart->m_mtClocksPerBeat * (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure;
						long lGridsPerMeasure = (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;

						long lGridStart = (m_lInsertingStartClock / lClocksPerMeasure) * lGridsPerMeasure;
						long lRemainder = m_lInsertingStartClock % lClocksPerMeasure;
						lGridStart += (lRemainder / pDMPart->m_mtClocksPerBeat) * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
						lRemainder  = lRemainder % pDMPart->m_mtClocksPerBeat;
						lGridStart += lRemainder / pDMPart->m_mtClocksPerGrid;
						lRemainder  = lRemainder % pDMPart->m_mtClocksPerGrid;

						if( lGridStart < 0 )
						{
							pCurve->m_mtGridStart = 0;
							pCurve->m_nTimeOffset = (short)max( SHRT_MIN, lRemainder + GRID_TO_CLOCKS( lGridStart, m_pPianoRollStrip->m_pPartRef->m_pDMPart ) );
						}
						else
						{
							pCurve->m_mtGridStart = lGridStart;
							pCurve->m_nTimeOffset = (short)lRemainder;
						}
						pCurve->m_dwVariation = m_pPianoRollStrip->m_dwVariations;
						pCurve->m_mtDuration = m_lInsertingEndClock - m_lInsertingStartClock;

						pCurve->m_nStartValue = m_nInsertingStartValue;
						pCurve->m_nEndValue = m_nInsertingEndValue;
						pCurve->m_bEventType = StripCCTypeToCurveType( m_bCCType );
						if( pCurve->m_bEventType == DMUS_CURVET_CCCURVE )
						{
							pCurve->m_bCCData = m_bCCType;
						}

						pCurve->m_wParamType = m_wRPNType;

						pCurve->SetDefaultResetValues( CalcPartClockLength() );

						// Place Curve in Part's event list
						SelectAllCurves( FALSE );
						pCurve->m_fSelected = TRUE;
						pDMPart->InsertCurveInAscendingOrder( pCurve );

						// Redraw CurveStrips
						RefreshCurveStrips();
						RefreshCurvePropertyPage();

						// Let the object know about the changes
						m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_INSERT_CURVE ); 
					}
				}

				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
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
									   CDirectMusicStyleCurve* pCurve,
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
		m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

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
void CCurveStrip::DragModify(CDirectMusicStyleCurve* pDMCurve, int nStripXPos, int nStripYPos, RECT *pRectCurve, int nAction, bool fDuplicateCurves)
{
	// setup the status bar
	if( m_pIFramework )
	{
		m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_MOUSEDOWNUP, &m_hKeyStatusBar );
		if( m_bCCType == CCTYPE_PB_CURVE_STRIP )
		{
			m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 20 );
		}
		else
		{
			m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 18 );
		}
	}

	long lXOffset = pRectCurve->left - pDMCurve->m_rectFrame.left;
	CRect selRect = *pRectCurve;
	CPoint point( nStripXPos, nStripYPos );

	// convert point to window coordinates
	VERIFY( SUCCEEDED( m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &point) ) );
	
	// convert the frame rect of the curve to window coordinates
	VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.TopLeft())));
	VERIFY(SUCCEEDED(m_pMIDIMgr->m_pTimeline->StripToWindowPos(this, &selRect.BottomRight())));
	
	// create a list of selected curves (not including the one clicked on)
	CCurveList curveList;

	if (nAction == CTRK_DRAGGING)
	{
		CDirectMusicEventItem* pEvent;
		pEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
		while(pEvent)
		{
			CDirectMusicStyleCurve* pCurve = (CDirectMusicStyleCurve*)pEvent;

			if(IsCurveForThisStrip(pCurve) && (pCurve != pDMCurve))
			{
				if (pCurve->m_fSelected)
				{
					curveList.AddTail(pCurve);
				}
			}
			pEvent = pEvent->GetNext();
		}
	}

	CWnd wnd;
	wnd.Attach(GetMyWindow());

	// create a tracker to handle the move
	CCurveTracker curveTracker(&wnd, m_pMIDIMgr->m_pTimeline, this, (short)nAction, &selRect);
	curveTracker.SetCurve(pDMCurve);
	curveTracker.SetCurveList(&curveList);
	curveTracker.SetXOffset( lXOffset );
	curveTracker.m_offset = 0;

	m_pCCurveTracker = &curveTracker;
	m_nTrackerAction = nAction;

	OnUpdateDragCurveValue(curveTracker, nAction);

	// This sets a flag in the Timeline so that it doesn't autoscroll during playback.
	// We have to call ReleaseCapture(), or the CurveTracker won't work.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
	::ReleaseCapture();

	int nResult = curveTracker.Track(&wnd, point, FALSE);

	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	m_pCCurveTracker = NULL;
	m_nTrackerAction = 0;

	if( nResult )
	{
		// Update the Part's GUID (and create a new Part if we need to)
		m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

		// Update the curves
		CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;
	
		long lClocksPerMeasure = pDMPart->m_mtClocksPerBeat * (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure;
		long lGridsPerMeasure = (long)pDMPart->m_TimeSignature.m_bBeatsPerMeasure * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;

		curveList.AddTail(pDMCurve);

		CDirectMusicCurveList lstMovedEvents;

		POSITION listPos;
		listPos = curveList.GetHeadPosition();
		while (listPos != NULL) {
			CDirectMusicStyleCurve* pCurve = curveList.GetNext(listPos);
			
			CRect newRect = GetUpdatedCurveRect(curveTracker, pCurve, nAction);

			// Assume the curve will change, so make a copy of it if necessary
			pDMPart->CreateCurveCopyIfNeeded( pCurve, m_pPianoRollStrip->m_dwVariations, fDuplicateCurves );

			// figure out start and end values and positions
			long lStartClock, lEndClock;
			m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.left, &lStartClock);
			m_pMIDIMgr->m_pTimeline->PositionToClocks(newRect.right, &lEndClock);
			short nStartValue = (short)YPosToValue(newRect.top);
			short nEndValue = (short)YPosToValue(newRect.bottom);

			// update the curves values
			long lGridStart = (lStartClock / lClocksPerMeasure) * lGridsPerMeasure;
			long lRemainder = lStartClock % lClocksPerMeasure;
			lGridStart += (lRemainder / pDMPart->m_mtClocksPerBeat) * (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
			lRemainder  = lRemainder % pDMPart->m_mtClocksPerBeat;
			lGridStart += lRemainder / pDMPart->m_mtClocksPerGrid;
			lRemainder  = lRemainder % pDMPart->m_mtClocksPerGrid;

			// Save the old start time
			long lOldValue = m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pCurve );

			ASSERT( lRemainder <= SHRT_MAX );
			if( lGridStart < 0 )
			{
				pCurve->m_mtGridStart = 0;
				pCurve->m_nTimeOffset = (short)max( SHRT_MIN, lRemainder + GRID_TO_CLOCKS( lGridStart, m_pPianoRollStrip->m_pPartRef->m_pDMPart ) );
			}
			else
			{
				pCurve->m_mtGridStart = lGridStart;
				pCurve->m_nTimeOffset = (short)lRemainder;
			}

			if (nAction != CTRK_DRAGGING) {
				pCurve->m_mtDuration = lEndClock - lStartClock;
			}

			pCurve->m_nStartValue = nStartValue;
			pCurve->m_nEndValue = nEndValue;

			// If the curve's start position changed, remove and re-add it to the list
			if( lOldValue != m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pCurve ) )
			{
				if( m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pCurve ) < lOldValue )
				{
					// TODO: Optimize this, if possible
					// Move note backwards - remove it and re-add it later

					// Remove the event from the part's list
					m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.Remove( pCurve );

					// Add it to our private list
					lstMovedEvents.AddTail( pCurve );
				}

				// If moved forwards, only add to lstMovedSequences if we're now after
				// the item at pos.
				else
				{
					// Save a pointer to the next event
					CDirectMusicEventItem *pDMEvent2 = pCurve->GetNext();

					if( pDMEvent2
					&& (m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pDMEvent2 ) < m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime(pCurve)) )
					{
						// Remove the event from the part's list
						m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.Remove( pCurve );

						// Add it to our private list
						lstMovedEvents.AddTail( pCurve );
					}
				}
			}
		}

		// Merge back in any curves we moved
		if( lstMovedEvents.GetHead() )
		{
			m_pPianoRollStrip->m_pPartRef->m_pDMPart->MergeCurveList( &lstMovedEvents );
		}

		// Redraw CurveStrips
		RefreshCurveStrips();

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_CHANGE_CURVE ); 
	}

	// Always refresh the property page
	RefreshCurvePropertyPage();

	if( m_pIFramework )
	{
		m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );	
		m_hKeyStatusBar = NULL;
	}
	
	wnd.Detach();
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::OnSingleCurve

HRESULT CCurveStrip::OnSingleCurve( void )
{
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicEventItem* pDMEventNext;
	CDirectMusicStyleCurve* pDMCurve;

	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	// Create Direct Music Curve
	CDirectMusicStyleCurve* pDMSingleCurve = new CDirectMusicStyleCurve;
	if( pDMSingleCurve == NULL )
	{
		return E_FAIL;
	}

	// Update the Part's GUID (and create a new Part if we need to)
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

	MUSIC_TIME mtSingleCurveEndTime;
	MUSIC_TIME mtCurveEndTime;
	long lNbrBeats;
	long lNbrGrids;
	
	WORD wRefreshUI = FALSE;

	pDMEventNext = pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEventNext ;  )
	{
		pDMEvent = pDMEventNext;
		pDMEventNext = pDMEvent->GetNext();

		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;
		
			if( IsCurveForThisStrip( pDMCurve ) )
			{
				if( wRefreshUI == FALSE )
				{
					// First Curve
					*pDMSingleCurve = *pDMCurve;
					pDMSingleCurve->m_dwVariation = m_pPianoRollStrip->m_dwVariations;
					pDMSingleCurve->m_bCurveShape = DMUS_CURVES_SINE;

					// Clear the m_pNext we copied
					pDMSingleCurve->SetNext( NULL );
				}
				else
				{
					lNbrBeats = (long)pDMSingleCurve->m_mtGridStart / (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					lNbrGrids = (long)pDMSingleCurve->m_mtGridStart % (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					mtSingleCurveEndTime = (lNbrBeats * pDMPart->m_mtClocksPerBeat) +
										   (lNbrGrids * pDMPart->m_mtClocksPerGrid) +
										   pDMSingleCurve->m_nTimeOffset;
					if( pDMSingleCurve->m_bCurveShape != DMUS_CURVES_INSTANT )
					{
						mtSingleCurveEndTime += pDMSingleCurve->m_mtDuration;
					}
						
					
					lNbrBeats = (long)pDMCurve->m_mtGridStart / (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					lNbrGrids = (long)pDMCurve->m_mtGridStart % (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					mtCurveEndTime = (lNbrBeats * pDMPart->m_mtClocksPerBeat) +
									 (lNbrGrids * pDMPart->m_mtClocksPerGrid) +
									 pDMCurve->m_nTimeOffset;
					if( pDMCurve->m_bCurveShape != DMUS_CURVES_INSTANT )
					{
						mtCurveEndTime += pDMCurve->m_mtDuration;
					}
						
					// Make sure the pDMSingleCurve "start" fields represent the Curve
					// with the earliest start time
					if( (pDMCurve->m_mtGridStart < pDMSingleCurve->m_mtGridStart)
					||  (pDMCurve->m_mtGridStart == pDMSingleCurve->m_mtGridStart  &&
						 pDMCurve->m_nTimeOffset < pDMSingleCurve->m_nTimeOffset) )
					{
						pDMSingleCurve->m_mtGridStart = pDMCurve->m_mtGridStart;
						pDMSingleCurve->m_nTimeOffset = pDMCurve->m_nTimeOffset;
						pDMSingleCurve->m_nStartValue = pDMCurve->m_nStartValue;
						pDMSingleCurve->m_mtResetDuration = pDMCurve->m_mtResetDuration;
						pDMSingleCurve->m_nResetValue = pDMCurve->m_nResetValue;
						pDMSingleCurve->m_bFlags = pDMCurve->m_bFlags;
					}

					// Make sure the pDMSingleCurve "end" fields represent the Curve
					// with the latest end time
					lNbrBeats = (long)pDMSingleCurve->m_mtGridStart / (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					lNbrGrids = (long)pDMSingleCurve->m_mtGridStart % (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
					if( mtCurveEndTime > mtSingleCurveEndTime )
					{
						pDMSingleCurve->m_mtDuration = mtCurveEndTime -
													  (lNbrBeats * pDMPart->m_mtClocksPerBeat) -
													  (lNbrGrids * pDMPart->m_mtClocksPerGrid) -
													  pDMSingleCurve->m_nTimeOffset;
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
						pDMSingleCurve->m_mtDuration = mtSingleCurveEndTime -
													  (lNbrBeats * pDMPart->m_mtClocksPerBeat) -
													  (lNbrGrids * pDMPart->m_mtClocksPerGrid) -
													  pDMSingleCurve->m_nTimeOffset;
					}
				}

				// Delete curve from displayed variations
				pDMCurve->m_dwVariation &= ~m_pPianoRollStrip->m_dwVariations;
				
				if( pDMCurve->m_dwVariation == 0 )
				{
					// Curve is not in any other variations, so delete it
					pDMPart->m_lstCurves.Remove( pDMCurve );
					//pDMCurve->SetNext( NULL );
					delete pDMCurve;
				}
				else
				{
					// Curve is in other variations, so turn off selected flag
					pDMCurve->m_fSelected = FALSE;
				}

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
	pDMPart->InsertCurveInAscendingOrder( pDMSingleCurve );

	// Redraw CurveStrips
	RefreshCurveStrips();
	RefreshCurvePropertyPage();

	// Let the object know about the changes
	m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_SINGLE_CURVE ); 

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteSelectedCurves

void CCurveStrip::DeleteSelectedCurves()
{
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicEventItem* pDMEventNext;

	WORD wRefreshUI = FALSE;

	// Update the Part's GUID (and create a new Part if we need to)
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	pDMEventNext = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEventNext ;  )
	{
		pDMEvent = pDMEventNext;
		pDMEventNext = pDMEvent->GetNext();

		if( pDMEvent->m_fSelected )
		{
			if( IsCurveForThisStrip( (CDirectMusicStyleCurve *)pDMEvent ) )
			{
				// Delete curve from displayed variations
				pDMEvent->m_dwVariation &= ~m_pPianoRollStrip->m_dwVariations;
				
				if( pDMEvent->m_dwVariation == 0 )
				{
					// Curve is not in any other variations, so delete it
					m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.Remove( pDMEvent );
					pDMEvent->SetNext( NULL );
					delete pDMEvent;
				}
				else
				{
					// Curve is in other variations, so turn off selected flag
					pDMEvent->m_fSelected = FALSE;
				}

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
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_DELETE_CURVE ); 
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteAllCurves

WORD CCurveStrip::DeleteAllCurves()
{
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicEventItem* pDMEventNext;

	WORD wRefreshUI = FALSE;

	pDMEventNext = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEventNext ;  )
	{
		pDMEvent = pDMEventNext;
		pDMEventNext = pDMEvent->GetNext();

		// Make sure Curve should be displayed in this strip
		if( (m_pPianoRollStrip->CurveTypeToStripCCType((CDirectMusicStyleCurve *)pDMEvent) == m_bCCType)
		&&	(((CDirectMusicStyleCurve *)pDMEvent)->m_wParamType == m_wRPNType) )
		{
			m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.Remove( pDMEvent );
			pDMEvent->SetNext(NULL);
			delete pDMEvent;

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
		if( m_pMIDIMgr->m_pTimeline )
		{
			VARIANT var;

			var.vt = VT_BOOL;
			V_BOOL(&var) = (short)m_pPianoRollStrip->m_fGutterSelected;
			m_pMIDIMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)this, STP_GUTTER_SELECTED, var );
		}
	}

	if( !m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_fSelecting
	||  (m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC != m_bCCType)
	||	(m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_wSelectionParamType != m_wRPNType) )
	{
		if( m_pPianoRollStrip->m_lBeginTimelineSelection == m_pPianoRollStrip->m_lEndTimelineSelection 
		||  m_pPianoRollStrip->m_fGutterSelected == FALSE )
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

CDirectMusicStyleCurve* CCurveStrip::GetEarliestSelectedCurve( void )
{
	CDirectMusicEventItem *pDMEvent;
	CDirectMusicStyleCurve *pDMCurve, *pDMEarlyCurve = NULL;
	ioDMCurveClipInfo oDMCurveClipInfo;

	pDMEvent = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( pDMEvent->m_fSelected )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			if( IsCurveForThisStrip( pDMCurve ) )
			{
				if( !pDMEarlyCurve )
				{
					pDMEarlyCurve = pDMCurve;
				}
				else if( pDMEarlyCurve->m_mtGridStart > pDMCurve->m_mtGridStart )
				{
					pDMCurve = pDMEarlyCurve;
				}
			}
		}
	}

	return pDMEarlyCurve;
}


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip::DeleteCurvesBetweenTimes

void CCurveStrip::DeleteCurvesBetweenTimes( MUSIC_TIME mtEarliestCurve, MUSIC_TIME mtLatestCurve, long lCursorGrid )
{
	CDirectMusicPart* pDMPart = m_pPianoRollStrip->m_pPartRef->m_pDMPart;

	mtEarliestCurve /= pDMPart->m_mtClocksPerGrid;
	mtLatestCurve /= pDMPart->m_mtClocksPerGrid;
	mtEarliestCurve += lCursorGrid;
	mtLatestCurve += lCursorGrid;
	mtEarliestCurve *= pDMPart->m_mtClocksPerGrid;
	mtLatestCurve *= pDMPart->m_mtClocksPerGrid;
	mtLatestCurve += pDMPart->m_mtClocksPerGrid - 1;

	CDirectMusicEventItem *pDMEvent;
	CDirectMusicEventItem *pDMEventNext = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.GetHead();
	for( ;  pDMEventNext ;  )
	{
		pDMEvent = pDMEventNext;
		pDMEventNext = pDMEvent->GetNext();

		if( IsCurveForThisStrip( (CDirectMusicStyleCurve *)pDMEvent ) )
		{
			MUSIC_TIME mtCurveStart = m_pPianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pDMEvent );
			if( (mtCurveStart >= mtEarliestCurve) &&
				(mtCurveStart <= mtLatestCurve) )
			{
				// Delete curve from displayed variations
				pDMEvent->m_dwVariation &= ~m_pPianoRollStrip->m_dwVariations;
				
				if( pDMEvent->m_dwVariation == 0 )
				{
					// Curve is not in any other variations, so delete it
					m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstCurves.Remove( pDMEvent );
					pDMEvent->SetNext( NULL );
					delete pDMEvent;
				}
			}
		}
	}
}


void CallFnForEachPart( CDirectMusicPartRef *pPartRef, EACHPART_CALLBACK eachPartFn, void *pThis, void *pData )
{
	// Get the length of the Timeline, Pattern, and Part
	VARIANT var;
	pPartRef->m_pDMPart->m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	const long lTimelineClockLength = V_I4(&var);
	const long lPartClockLength = pPartRef->m_pDMPart->GetClockLength();
	const long lPatternLength = pPartRef->m_pPattern->CalcLength();

	// Compute the number of times the pattern repeats within the Timeline
	const long lPatternRepeats = (lTimelineClockLength - 1) / lPatternLength;

	// Compute the number of times the part repeats within the pattern
	long lPartRepeats = (lPatternLength - 1) / lPartClockLength;

	// Compute the starting pattern and part indexes
	/*
	long lPatternIndex = mtStart / lPatternLength;
	long lPartIndex = (mtStart - lPatternIndex * lPatternLength) / lPartClockLength;
	*/
	long lPatternIndex = 0;
	long lPartIndex = 0;

	// Continue through all repeats of the pattern
	while( lPatternIndex <= lPatternRepeats )
	{
		// If we're on the last pattern repeat, compute how many parts fit in this last repeat
		if( lPatternIndex == lPatternRepeats )
		{
			lPartRepeats = (lTimelineClockLength - lPatternIndex * lPatternLength - 1) / lPartClockLength;;
		}

		// Continue through all repeats of the part
		while( lPartIndex <= lPartRepeats )
		{
			// Compute the nubmber of grid that are played in this part repeat
			long lPartGridLength;

			// If we're in the last part repeat
			if( lPartIndex == lPartRepeats )
			{
				// Compute the amount of time left to play
				MUSIC_TIME mtTimeLeft;
				if( lPatternIndex == lPatternRepeats )
				{
					mtTimeLeft = lTimelineClockLength - lPatternLength * lPatternIndex - lPartClockLength * lPartIndex;
				}
				else
				{
					mtTimeLeft = lPatternLength % lPartClockLength;
					if( mtTimeLeft == 0 )
					{
						mtTimeLeft = lPartClockLength;
					}
				}

				// Convert from clocks to a number of grids
				lPartGridLength = CLOCKS_TO_GRID( mtTimeLeft + pPartRef->m_pDMPart->m_mtClocksPerGrid - 1, pPartRef->m_pDMPart );
			}
			else
			{
				// Use the full length of the part
				lPartGridLength = pPartRef->m_pDMPart->GetGridLength();
			}

			// Compute the start time offset
			const MUSIC_TIME mtOffset = lPatternIndex * lPatternLength + lPartIndex * lPartClockLength;

			//eachPartFn( mtOffset, mtOffset > mtStart ? 0 : mtStart - mtOffset, mtOffset < mtEnd ?
			eachPartFn( pPartRef, mtOffset, lPartGridLength, pThis, pData );

			// Increment the part index
			lPartIndex++;
		}

		// Reset the part index
		lPartIndex = 0;

		// Increment the pattern index
		lPatternIndex++;
	}
}
