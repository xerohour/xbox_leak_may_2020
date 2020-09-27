// VarSwitchStrip.cpp : Implementation of CVarSwitchStrip
#include "stdafx.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "VarSwitchStrip.h"
#include "ioDMStyle.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <riffstrm.h>
#include "PropSwitchPoint.h"
#include "SharedPianoRoll.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {4CD210CD-3ADB-4886-8050-AE8A519C3485}
static const GUID GUID_MarkerPropPageManager = 
{ 0x4cd210cd, 0x3adb, 0x4886, { 0x80, 0x50, 0xae, 0x8a, 0x51, 0x9c, 0x34, 0x85 } };

#define COLOR_ENTER			RGB( 0, 200, 0 )
#define COLOR_EXIT			RGB( 200, 0, 0 )
#define COLOR_MULT_ENTER	RGB( 0, 200, 0 )
#define COLOR_MULT_EXIT		RGB( 200, 0, 0 )
#define COLOR_SEL_ENTER			RGB( 0, 110, 0 )
#define COLOR_SEL_EXIT			RGB( 110, 0, 0 )
#define COLOR_SEL_MULT_ENTER	RGB( 0, 110, 0 )
#define COLOR_SEL_MULT_EXIT		RGB( 110, 0, 0 )

#define ENTER_TOP			0
#define ENTER_BOTTOM		(VARSWITCH_HEIGHT / 2)
#define EXIT_TOP			(VARSWITCH_HEIGHT / 2)
#define EXIT_BOTTOM			VARSWITCH_HEIGHT

#define CHORD_LINE_PENSTYLE	PS_SOLID
#define CHORD_LINE_WIDTH	3
#define CHORD_LINE_COLOR	RGB(0,0,0)

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip constructor/destructor

CVarSwitchStrip::CVarSwitchStrip( CMIDIMgr* pMIDIMgr, CPianoRollStrip* pPianoRollStrip )
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

	m_pIFramework = NULL;

	if( m_pMIDIMgr->m_pTimeline )
	{
		VARIANT var;

		if( SUCCEEDED ( m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var ) ) )
		{
			m_pIFramework = (IDMUSProdFramework *)V_UNKNOWN( &var );
		}
	}

	m_mtGridToShiftSelFrom = -1;

	m_lStartSelectInRange = 0;
	m_lEndSelectInRange = 0;
}

CVarSwitchStrip::~CVarSwitchStrip()
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
		m_pStripMgr = NULL;
	}

	m_pMIDIMgr = NULL;

	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::QueryInterface

STDMETHODIMP CVarSwitchStrip::QueryInterface( REFIID riid, LPVOID *ppv )
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
// CVarSwitchStrip::AddRef

STDMETHODIMP_(ULONG) CVarSwitchStrip::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Release

STDMETHODIMP_(ULONG) CVarSwitchStrip::Release(void)
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
// CVarSwitchStrip Helper methods

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetMyWindow
//
// Retrieves the handle of the window which this strip is in.
HWND CVarSwitchStrip::GetMyWindow()
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
// CVarSwitchStrip::GetStripRect
//
BOOL CVarSwitchStrip::GetStripRect(LPRECT pRectStrip)
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
	POINT ptTop;
	POINT ptBottom;
	
	ptTop.x = 0;
	ptTop.y = 0;
	ptBottom.x = 0;
	ptBottom.y = VARSWITCH_HEIGHT;
	
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &ptTop );
	m_pMIDIMgr->m_pTimeline->StripToWindowPos( this, &ptBottom );

	pRectStrip->top = ptTop.y;
	pRectStrip->bottom = ptBottom.y;
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip Drawing methods

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::InvertGutterRange

void CVarSwitchStrip::InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset )
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
// CVarSwitchStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Draw

HRESULT	STDMETHODCALLTYPE CVarSwitchStrip::Draw( HDC hDC, STRIPVIEW /*sv*/, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );
	ASSERT( m_pPianoRollStrip != NULL );
	
	if( hDC == NULL )
	{
		return E_INVALIDARG;
	}

	// Get Function Bar rectangle
	VARIANT var;
	RECT rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if( FAILED ( m_pMIDIMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
	{
		return E_FAIL;
	}

	// Get Strip rectangle
	RECT rectStrip;
	if (!GetStripRect(&rectStrip))
	{
		return E_FAIL;
	}

	// Get Strip left position
	long lLeftPosition;
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftPosition );
	m_pMIDIMgr->m_pTimeline->ClocksToPosition( lLeftPosition, &lLeftPosition );

	::OffsetRect( &rectStrip, -rectFBar.right, -rectStrip.top );
	::OffsetRect( &rectStrip, lLeftPosition, 0 );

	// Get clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// DRAW STRIP
	m_pPianoRollStrip->DrawVerticalines( hDC, lXOffset );
	InvertGutterRange( hDC, &rectClip, lXOffset );

	MUSIC_TIME mtLeftClock, mtRightClock;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &mtLeftClock );
	m_pMIDIMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &mtRightClock );

	// Create the brush to display the 'chord' markers with
	HBRUSH hbrushHatchChord = ::CreateHatchBrush( HS_BDIAGONAL, ::GetNearestColor(hDC, COLOR_HATCH_OVERLAPPING) ); 

	// Create the pen to draw the chord outlines with
	HPEN hpenOld = NULL;
	HPEN hpenChordLine = ::CreatePen( CHORD_LINE_PENSTYLE, CHORD_LINE_WIDTH, CHORD_LINE_COLOR );
	if( hpenChordLine )
	{
		// Save the old pen
		hpenOld = static_cast<HPEN>(::SelectObject( hDC, hpenChordLine ));
	}

	// Rect to write the marker in
	RECT rectMarker;

	// Save the background color
	COLORREF crOldBkColor = ::GetBkColor( hDC );

	// Calculate the length of the part and how many repeats of the part there is in the pattern
	MUSIC_TIME mtPartClockLength = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength();
	MUSIC_TIME mtPatternClockLength = m_pPianoRollStrip->m_pPartRef->m_pPattern->CalcLength();
	long lNumPartRepeats = (mtPartClockLength - 1 + mtPatternClockLength) / mtPartClockLength;

	for( long lPatternRepeat = mtLeftClock / mtPatternClockLength; lPatternRepeat <= mtRightClock / mtPatternClockLength; lPatternRepeat++ )
	{
		for( long lPartRepeat = 0; lPartRepeat < lNumPartRepeats; lPartRepeat++ )
		{
			// Calculate the clock offset to use
			MUSIC_TIME mtPatternMax = lPatternRepeat * mtPatternClockLength;
			MUSIC_TIME mtClockOffset = lPartRepeat * mtPartClockLength + mtPatternMax;
			mtPatternMax += mtPatternClockLength;

			// Get first marker
			CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations );
			while( pDMMarker )
			{
				// Only draw the markers that belong to the variations being displayed
				// and that are displays within the clipping region we were given
				if( MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
				{
					// Compute the start and end positions
					GetGridRect( pDMMarker->m_mtGridStart, &rectMarker, mtClockOffset, mtPatternMax );

					// Ensure there is a gap between markers
					if( rectMarker.right - rectMarker.left > 3 )
					{
						rectMarker.right--;
						rectMarker.left++;
					}

					// Offset them by lXOffset
					rectMarker.left -= lXOffset;
					rectMarker.right -= lXOffset;

					// Check if we need to draw an enter marker
					if( pDMMarker->m_dwEnterVariation & m_pPianoRollStrip->m_dwVariations )
					{
						// Check if we're displaying both start+chord and start only markers
						BOOL fOverlappingMarkers = (m_pPianoRollStrip->m_dwVariations & pDMMarker->m_dwEnterChordVariation)
							&& ((pDMMarker->m_dwEnterVariation ^ pDMMarker->m_dwEnterChordVariation) & m_pPianoRollStrip->m_dwVariations);

						// Enter
						::SetBkColor( hDC, fOverlappingMarkers ? (pDMMarker->m_fSelected ? COLOR_SEL_MULT_ENTER : COLOR_MULT_ENTER) : (pDMMarker->m_fSelected ? COLOR_SEL_ENTER : COLOR_ENTER) );
						rectMarker.top = ENTER_TOP;
						rectMarker.bottom = ENTER_BOTTOM;

						if( fOverlappingMarkers )
						{
							// Is overlapping
							::FillRect( hDC, &rectMarker, hbrushHatchChord );
						}
						else
						{
							::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectMarker, NULL, 0, NULL);
						}
					}

					// Check if we need to draw an exit marker
					if( pDMMarker->m_dwExitVariation & m_pPianoRollStrip->m_dwVariations )
					{
						// Check if we're displaying both exit and exit+chord markers
						BOOL fOverlappingMarkers = (m_pPianoRollStrip->m_dwVariations & pDMMarker->m_dwExitChordVariation)
							&& ((pDMMarker->m_dwExitVariation ^ pDMMarker->m_dwExitChordVariation) & m_pPianoRollStrip->m_dwVariations);

						// Exit
						::SetBkColor( hDC, fOverlappingMarkers ? (pDMMarker->m_fSelected ? COLOR_SEL_MULT_EXIT : COLOR_MULT_EXIT) : (pDMMarker->m_fSelected ? COLOR_SEL_EXIT : COLOR_EXIT) );
						rectMarker.top = EXIT_TOP;
						rectMarker.bottom = EXIT_BOTTOM;

						if( fOverlappingMarkers )
						{
							// Is overlapping
							::FillRect( hDC, &rectMarker, hbrushHatchChord );
						}
						else
						{
							::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectMarker, NULL, 0, NULL);
						}
					}

					if( pDMMarker->m_dwEnterChordVariation & m_pPianoRollStrip->m_dwVariations )
					{
						rectMarker.top = ENTER_TOP;
						if( pDMMarker->m_dwExitChordVariation & m_pPianoRollStrip->m_dwVariations )
						{
							rectMarker.bottom = EXIT_BOTTOM;
						}
						else
						{
							rectMarker.bottom = ENTER_BOTTOM;
						}

						::MoveToEx( hDC, rectMarker.right, rectMarker.top, NULL );
						::LineTo( hDC, rectMarker.left, rectMarker.top );
						::LineTo( hDC, rectMarker.left, rectMarker.bottom );
						::LineTo( hDC, rectMarker.right, rectMarker.bottom );
					}
					else if( pDMMarker->m_dwExitChordVariation & m_pPianoRollStrip->m_dwVariations )
					{
						rectMarker.top = EXIT_TOP;
						rectMarker.bottom = EXIT_BOTTOM;

						::MoveToEx( hDC, rectMarker.right, rectMarker.top, NULL );
						::LineTo( hDC, rectMarker.left, rectMarker.top );
						::LineTo( hDC, rectMarker.left, rectMarker.bottom );
						::LineTo( hDC, rectMarker.right, rectMarker.bottom );
					}
				}

				// Get next marker
				pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext());
			}
		}
	}

	if( lNumPartRepeats > 1 )
	{
		// If the right side of the clipping region is after the end of the part, draw hatch marks

		// Compute the rectangle to display the hatch marks in
		RECT rectRepeat;
		rectRepeat.top = rectClip.top;
		rectRepeat.bottom = rectClip.bottom;
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength(), &rectRepeat.left );
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

	// Reset the pen type
	if( hpenChordLine )
	{
		::SelectObject( hDC, hpenOld );
		::DeleteObject( hpenChordLine );
	}

	// Reset the old background color
	::SetBkColor( hDC, crOldBkColor );

	// Delete the brush
	if( hbrushHatchChord )
	{
		::DeleteObject( hbrushHatchChord );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CVarSwitchStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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
			   *pCLSID = CLSID_VarSwitchStrip;
			}
			else
			{
				return E_FAIL;
			}
			break;

		case SP_RESIZEABLE:
			// We are not resizable
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = FALSE;
			break;

		case SP_GUTTERSELECTABLE:
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = TRUE;
			break;

		case SP_MINMAXABLE:
			// We don't support Minimize/Maximize
			pvar->vt = VT_BOOL;
			V_BOOL(pvar) = FALSE ;
			break;

		case SP_DEFAULTHEIGHT:
		case SP_MAXHEIGHT:
		case SP_MINIMIZE_HEIGHT:
			pvar->vt = VT_INT;
			V_INT(pvar) = VARSWITCH_HEIGHT;
			break;

		case SP_NAME:
		{
			CComBSTR bstrName;
			bstrName.LoadString( IDS_VARIATION_SWITCH_TITLE );
			pvar->vt = VT_BSTR;
			V_BSTR(pvar) = bstrName.Detach();
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
			return m_pPianoRollStrip->GetStripProperty( sp, pvar );

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CVarSwitchStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CVarSwitchStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, LONG lXPos, LONG lYPos )
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
			RefreshPropertyPage();
			if( m_pMIDIMgr->m_pDMPattern->m_fInLoad == FALSE )
			{
				m_pMIDIMgr->UpdateOnDataChanged( NULL ); 
			}
			m_pMIDIMgr->SetFocus(m_pPianoRollStrip,3);
			break;

		case WM_KILLFOCUS:
			m_pMIDIMgr->KillFocus(m_pPianoRollStrip);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			hr = OnLButtonDown( wParam, lXPos, lYPos );
			break;

		case WM_KEYDOWN:
			if( wParam == VK_INSERT )
			{
				// Check if the control key is down
				if( (GetKeyState( VK_CONTROL ) & 0x8000) != 0 )
				{
					hr = InsertHelper( MARKERF_BOTH | DMUS_MARKERF_CHORD_ALIGN );
				}
				else
				{
					hr = InsertHelper( DMUS_MARKERF_CHORD_ALIGN );
				}
			}
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp( wParam, lXPos, lYPos );
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

				case ID_EDIT_MERGE_VARIATIONS:
					m_pMIDIMgr->MergeVariations( m_pPianoRollStrip->m_pPartRef, m_pPianoRollStrip->m_dwVariations );
					break;

				case ID_EDIT_INSERT:
					hr = InsertHelper( MARKERF_BOTH );
					break;

				case ID_EDIT_INSERTSP_ENTER:
					hr = InsertHelper( DMUS_MARKERF_START );
					break;

				case ID_EDIT_INSERTSP_EXIT:
					hr = InsertHelper( DMUS_MARKERF_STOP );
					break;

				case ID_EDIT_INSERTSP_ENTEREXITCHORD:
					hr = InsertHelper( MARKERF_BOTH | DMUS_MARKERF_CHORD_ALIGN );
					break;

				case ID_EDIT_INSERTSP_ENTERCHORD:
					hr = InsertHelper( DMUS_MARKERF_START | DMUS_MARKERF_CHORD_ALIGN );
					break;

				case ID_EDIT_INSERTSP_EXITCHORD:
					hr = InsertHelper( DMUS_MARKERF_STOP | DMUS_MARKERF_CHORD_ALIGN );
					break;

				case ID_EDIT_DELETE:
					hr = Delete();
					break;

				case ID_VIEW_PROPERTIES:
					OnShowProperties();
					break;

				case ID_EDIT_MARKALL_MEASURES:
					hr = MarkAllHelper( TRUE, DMUS_SEGF_MEASURE );
					break;

				case ID_EDIT_MARKALL_BEATS:
					hr = MarkAllHelper( TRUE, DMUS_SEGF_BEAT );
					break;

				case ID_EDIT_MARKALL_GRIDS:
					hr = MarkAllHelper( TRUE, DMUS_SEGF_GRID );
					break;

				case ID_EDIT_MARKRANGE_MEASURES:
					hr = MarkRangeHelper( TRUE, DMUS_SEGF_MEASURE );
					break;

				case ID_EDIT_MARKRANGE_BEATS:
					hr = MarkRangeHelper( TRUE, DMUS_SEGF_BEAT );
					break;

				case ID_EDIT_MARKRANGE_GRIDS:
					hr = MarkRangeHelper( TRUE, DMUS_SEGF_GRID );
					break;

				case ID_EDIT_UNMARKALL_MEASURES:
					hr = MarkAllHelper( FALSE, DMUS_SEGF_MEASURE );
					break;

				case ID_EDIT_UNMARKALL_BEATS:
					hr = MarkAllHelper( FALSE, DMUS_SEGF_BEAT );
					break;

				case ID_EDIT_UNMARKALL_GRIDS:
					hr = MarkAllHelper( FALSE, DMUS_SEGF_GRID );
					break;

				case ID_EDIT_UNMARKRANGE_MEASURES:
					hr = MarkRangeHelper( FALSE, DMUS_SEGF_MEASURE );
					break;

				case ID_EDIT_UNMARKRANGE_BEATS:
					hr = MarkRangeHelper( FALSE, DMUS_SEGF_BEAT );
					break;

				case ID_EDIT_UNMARKRANGE_GRIDS:
					hr = MarkRangeHelper( FALSE, DMUS_SEGF_GRID );
					break;

				default:
					break;
			}
			break;
		}

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
// CVarSwitchStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::FBDraw

HRESULT CVarSwitchStrip::FBDraw( HDC /*hDC*/, STRIPVIEW /*sv*/ )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::FBOnWMMessage

HRESULT CVarSwitchStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM /*lParam*/, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPianoRollStrip != NULL );

	// Process the window message
	HRESULT hr = S_OK;

	switch( nMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			break;

		case WM_RBUTTONUP:
			hr = OnRButtonUp( wParam, lXPos, lYPos );
			break;

		default:
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Load

HRESULT CVarSwitchStrip::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    HRESULT hr = E_FAIL;

	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		MMCKINFO ckMain;
		ckMain.ckid = FOURCC_STYLE_MARKER_CLIPBOARD_CHUNK;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDCHUNK ) == 0 )
		{
			// Assume that something will change
			m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

			hr = LoadMarkers( pIStream, ckMain.cksize );

			if( hr != E_ABORT )
			{
				// Redraw CurveStrips
				RefreshVarSwitchStrips();
				RefreshPropertyPage();

				// Let the object know about the changes
				m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PASTE_MARKER ); 
			}
		}

		pIRiffStream->Release();
	}

    return hr;
}
 

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::LoadMarkers

HRESULT CVarSwitchStrip::LoadMarkers( IStream* pIStream, long lChunkSize )
{
	// Get cursor time
	long lInsertGrid;
	if( FAILED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lInsertGrid ) ) )
	{
		return E_FAIL;
	}

	// Change clocks into a grid
	lInsertGrid = CLOCKS_TO_GRID( lInsertGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );

	// turn off select flags
	SelectAllVarSwitches( FALSE );

	// Compute length of part
	long lLastGrid;
	lLastGrid = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetGridLength() - 1;

	// Read size of the ioDMStyleMarkerClipInfo structure
	long lSize = lChunkSize;
	DWORD dwStructSize, dwByteCount;
	HRESULT hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
	if( FAILED( hr ) || (dwByteCount != sizeof( dwStructSize )) )
	{
		return E_FAIL;
	}
	lSize -= dwByteCount;

	DWORD dwExtra;
	if( dwStructSize > sizeof( ioDMStyleMarkerClipInfo ) )
	{
		dwExtra = dwStructSize - sizeof( ioDMStyleMarkerClipInfo );
		dwStructSize = sizeof( ioDMStyleMarkerClipInfo );
	}
	else
	{
		dwExtra = 0;
	}

	// Now read the ioDMStyleMarkerClipInfo structure
	ioDMStyleMarkerClipInfo iDMStyleMarkerClipInfo;
	hr = pIStream->Read( &iDMStyleMarkerClipInfo, dwStructSize, &dwByteCount );
	if( FAILED( hr ) || (dwByteCount != dwStructSize) )
	{
		return E_FAIL;
	}
	lSize -= dwStructSize;

	if( dwExtra > 0 )
	{
		StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
		lSize -= dwExtra;
	}

	// Read size of the DMUS_IO_STYLEMARKER structure
	hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
	if( FAILED( hr ) || (dwByteCount != sizeof( dwStructSize )) )
	{
		return E_FAIL;
	}
	lSize -= dwByteCount;

	if( dwStructSize != sizeof( DMUS_IO_STYLEMARKER ) )
	{
		return E_FAIL;
	}

	// Save the original size of the stream, and the current position
	long lOrigSize = lSize;
	DWORD dwStreamPos = StreamTell( pIStream );

	// Find the extent of the markers
	MUSIC_TIME mtEarliestMarker = INT_MAX;
	MUSIC_TIME mtLatestMarker = INT_MIN;
	DMUS_IO_STYLEMARKER iDMStyleMarker;
	while( lSize >= (signed)dwStructSize )
	{
		hr = pIStream->Read( &iDMStyleMarker, dwStructSize, &dwByteCount );
		if( FAILED( hr ) || (dwByteCount != dwStructSize) )
		{
			return E_FAIL;
		}
		lSize -= dwStructSize;

		mtEarliestMarker = min( mtEarliestMarker, iDMStyleMarker.mtGridStart );
		mtLatestMarker = max( mtLatestMarker, iDMStyleMarker.mtGridStart );
	}

	// Delete or offset markers
	if( m_pMIDIMgr->m_ptPasteType == TL_PASTE_OVERWRITE )
	{
		// Delete between grids mtEarliestMarker and mtLatestMarker
		DeleteMarkersBetweenTimes( lInsertGrid + mtEarliestMarker, lInsertGrid + mtLatestMarker );
	}

	// Reset lSize and seek pointer
	lSize = lOrigSize;
	StreamSeek( pIStream, dwStreamPos, STREAM_SEEK_SET );

	// Create a mapping of source variations to destination variations
	DWORD dwVariationMap[32];
	ZeroMemory( dwVariationMap, sizeof(DWORD) * 32 );
	int nCur = 0;
	for ( int nClip = 0; nClip < 32 && hr == S_OK; nClip++ )
	{
		if ( iDMStyleMarkerClipInfo.m_dwVariations & (1 << nClip) )
		{
			for ( int nTmp = nCur; nTmp < 32; nTmp++ )
			{
				if ( m_pPianoRollStrip->m_dwVariations & (1 << nTmp) )
				{
					dwVariationMap[nClip] = (1 << nTmp);
					nCur = nTmp + 1;
					break;
				}
			}
			if ( nTmp == 32 )
			{
				return E_ABORT;
			}
		}
	}

	// Now read in the Markers
	while( lSize >= (signed)dwStructSize )
	{
		CDirectMusicStyleMarker *pDMMarker = new CDirectMusicStyleMarker;
		if( pDMMarker )
		{
			hr = pDMMarker->Read( pIStream, sizeof( DMUS_IO_STYLEMARKER ), 0 );
			if( FAILED( hr ) )
			{
				delete pDMMarker;
				return E_FAIL;
			}

			// Select the marker
			pDMMarker->m_fSelected = TRUE;

			lSize -= sizeof( DMUS_IO_STYLEMARKER );

			// Offset the grid value
			pDMMarker->m_mtGridStart += lInsertGrid;

			// Do not add Markers past end of Part
			if( pDMMarker->m_mtGridStart > lLastGrid )
			{
				delete pDMMarker;
			}
			else
			{
				// Map the incoming variations to the displayed variations
				if( pDMMarker->m_dwEnterVariation )
				{
					pDMMarker->m_dwEnterVariation = MapVariations( pDMMarker->m_dwEnterVariation, dwVariationMap );

					if( pDMMarker->m_dwEnterChordVariation )
					{
						// Will always be the same as the main enter variations
						pDMMarker->m_dwEnterChordVariation = pDMMarker->m_dwEnterVariation;
					}
				}

				if( pDMMarker->m_dwExitVariation )
				{
					pDMMarker->m_dwExitVariation = MapVariations( pDMMarker->m_dwExitVariation, dwVariationMap );

					if( pDMMarker->m_dwExitChordVariation )
					{
						// Will always be the same as the main exit variations
						pDMMarker->m_dwExitChordVariation = pDMMarker->m_dwExitVariation;
					}
				}

				// Place Marker in Part's event list
				m_pPianoRollStrip->m_pPartRef->m_pDMPart->InsertMarkerInAscendingOrder( pDMMarker );
			}
		}
	}

	// Compact the marker list to ensure there is only one marker on each grid
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.CompactMarkerList();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Save

HRESULT CVarSwitchStrip::Save( IStream* pIStream )
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
			ckMain.ckid = FOURCC_STYLE_MARKER_CLIPBOARD_CHUNK;

			if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0
			&&  SUCCEEDED( SaveSelectedMarkers( pIStream ) )
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
// CVarSwitchStrip::SaveSelectedMarkers

HRESULT CVarSwitchStrip::SaveSelectedMarkers( IStream *pIStream )
{
	// Save size of ioDMStyleMarkerClipInfo structure
	DWORD dwStructSize = sizeof(ioDMStyleMarkerClipInfo);
	DWORD dwBytesWritten;
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		return E_FAIL;
	}

	// Prepare ioDMStyleMarkerClipInfo structure
	ioDMStyleMarkerClipInfo oDMMarkerClipInfo;
	memset( &oDMMarkerClipInfo, 0, sizeof(ioDMStyleMarkerClipInfo) );

	oDMMarkerClipInfo.m_dwVariations = m_pPianoRollStrip->m_dwVariations;

	// Save ioDMStyleMarkerClipInfo structure
	hr = pIStream->Write( &oDMMarkerClipInfo, sizeof(ioDMStyleMarkerClipInfo), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMStyleMarkerClipInfo) )
	{
		return E_FAIL;
	}

	// Save size of DMUS_IO_STYLEMARKER structure
	dwStructSize = sizeof(DMUS_IO_STYLEMARKER);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		return E_FAIL;
	}

	// Get the grid positin of the earliest selected marker
	long lStartGrid;
	CDirectMusicStyleMarker *pDMMarker = GetEarliestSelectedMarker();
	if( pDMMarker )
	{
		lStartGrid = pDMMarker->m_mtGridStart;
	}
	else
	{
		// Nothing to write
		return S_FALSE;
	}

	// Now save all the markers
	// already set pDMMarker to the first selected marker, so start from there
	for( ;  pDMMarker ;  pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext()) )
	{
		if( pDMMarker->m_fSelected
		&&	MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			CDirectMusicStyleMarker marker = *pDMMarker;
			marker.m_mtGridStart -= lStartGrid;
			marker.m_dwEnterVariation &= m_pPianoRollStrip->m_dwVariations;
			marker.m_dwEnterChordVariation &= m_pPianoRollStrip->m_dwVariations;
			marker.m_dwExitVariation &= m_pPianoRollStrip->m_dwVariations;
			marker.m_dwExitChordVariation &= m_pPianoRollStrip->m_dwVariations;
			if( FAILED( marker.Write( pIStream ) ) )
			{
				return E_FAIL;
			}
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Cut

HRESULT CVarSwitchStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
// CVarSwitchStrip::Copy

HRESULT CVarSwitchStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

		if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
		{
			// Put the selected curves into an IDataObject
			if( SUCCEEDED ( Save( pIStream ) ) )
			{
				// Merge with other strips
				if(pITimelineDataObject != NULL)
				{
					// add the stream to the passed ITimelineDataObject
					hr = pITimelineDataObject->AddInternalClipFormat( theApp.m_cfStyleMarker, pIStream );
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

					// Create a new TimelineDataObject
					IDMUSProdTimelineDataObject *pITimelineDataObject;
					if( FAILED( m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
					{
						return E_OUTOFMEMORY;
					}

					// add the stream to the DataObject
					hr = pITimelineDataObject->AddExternalClipFormat( theApp.m_cfStyleMarker, pIStream );
					pIStream->Release();
					ASSERT( hr == S_OK );
					if ( hr != S_OK )
					{
						pITimelineDataObject->Release();
						return E_FAIL;
					}

					// get the new IDataObject
					IDataObject* pIDataObject;
					hr = pITimelineDataObject->Export( &pIDataObject );
					pITimelineDataObject->Release();
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
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Paste

HRESULT CVarSwitchStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	// Read the Marker data
	if( pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfStyleMarker ) == S_OK )
	{
		IStream* pIStream;

		if( SUCCEEDED ( pITimelineDataObject->AttemptRead( theApp.m_cfStyleMarker, &pIStream ) ) )
		{
			if( SUCCEEDED ( Load ( pIStream ) ) )
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
// CVarSwitchStrip::Insert

HRESULT CVarSwitchStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return InsertHelper( MARKERF_BOTH );
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::InsertHelper

HRESULT CVarSwitchStrip::InsertHelper( WORD wMarkerType )
{
	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();

	ASSERT( m_pPianoRollStrip != NULL );
	if ( m_pPianoRollStrip == NULL )
	{
		return E_UNEXPECTED;
	}
	
	// Can't insert into 0 variations
	if( m_pPianoRollStrip->m_dwVariations == 0 )
	{
		return S_FALSE;
	}

	long lTime;
	if (FAILED(m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return E_FAIL;
	}

	// 27768: Disallow inserting in the repeats
	if( lTime >= m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength() )
	{
		return E_FAIL;
	}

	long lGrid = CLOCKS_TO_GRID( lTime, m_pPianoRollStrip->m_pPartRef->m_pDMPart );

	if (lGrid < 0)
	{
		lGrid = 0;
	}
	else
	{
		lGrid %= m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetGridLength();
	}

	// Unselect all markers
	SelectAllVarSwitches( FALSE );

	// Assume something will change
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	InsertMarkerAtGrid( wMarkerType, lGrid );

	// Let the object know about the changes
	m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_INSERT_MARKER ); 

	RefreshVarSwitchStrips();
	RefreshPropertyPage();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::InsertMarkerAtGrid

BOOL CVarSwitchStrip::InsertMarkerAtGrid( WORD wMarkerType, MUSIC_TIME mtGrid )
{
	// Flag whether anything changed
	BOOL fChanged = FALSE;

	// Check if there is an existing marker already on this grid
	CDirectMusicStyleMarker* pExistingMarker = GetMarkerFromGrid( mtGrid, ALL_VARIATIONS );

	// Check if we did not find a marker
	if( !pExistingMarker )
	{
		// Marker doesn't exist - create a new one
		pExistingMarker = CreateNewMarker( mtGrid );

		fChanged = TRUE;
	}

	// Add Start variations
	if( wMarkerType & DMUS_MARKERF_START )
	{
		// Check to see if this will change the marker
		if( (pExistingMarker->m_dwEnterVariation & m_pPianoRollStrip->m_dwVariations)
		!=	m_pPianoRollStrip->m_dwVariations )
		{
			fChanged = TRUE;
		}

		// Update the marker
		pExistingMarker->m_dwEnterVariation |= m_pPianoRollStrip->m_dwVariations;

		// Add Start+Chord variations
		if( wMarkerType & DMUS_MARKERF_CHORD_ALIGN )
		{
			// Check to see if this will change the marker
			if( (pExistingMarker->m_dwEnterChordVariation & m_pPianoRollStrip->m_dwVariations)
			!=	m_pPianoRollStrip->m_dwVariations )
			{
				fChanged = TRUE;
			}

			// Update the marker
			pExistingMarker->m_dwEnterChordVariation |= m_pPianoRollStrip->m_dwVariations;
		}
		else
		{
			// Ensure no Start+Chord flags are set in the displayed variations
			if( pExistingMarker->m_dwEnterChordVariation & m_pPianoRollStrip->m_dwVariations )
			{
				fChanged = TRUE;
			}

			// Update the marker
			pExistingMarker->m_dwEnterChordVariation &= ~m_pPianoRollStrip->m_dwVariations;
		}
	}

	// Add Stop variations
	if( wMarkerType & DMUS_MARKERF_STOP )
	{
		// Check to see if this will change the marker
		if( (pExistingMarker->m_dwExitVariation & m_pPianoRollStrip->m_dwVariations)
		!=	m_pPianoRollStrip->m_dwVariations )
		{
			fChanged = TRUE;
		}
		pExistingMarker->m_dwExitVariation |= m_pPianoRollStrip->m_dwVariations;

		// Add Exit+Chord variations
		if( wMarkerType & DMUS_MARKERF_CHORD_ALIGN )
		{
			// Check to see if this will change the marker
			if( (pExistingMarker->m_dwExitChordVariation & m_pPianoRollStrip->m_dwVariations)
			!=	m_pPianoRollStrip->m_dwVariations )
			{
				fChanged = TRUE;
			}

			// Update the marker
			pExistingMarker->m_dwExitChordVariation |= m_pPianoRollStrip->m_dwVariations;
		}
		else
		{
			// Ensure no Exit+Chord flags are set in the displayed variations
			if( pExistingMarker->m_dwExitChordVariation & m_pPianoRollStrip->m_dwVariations )
			{
				fChanged = TRUE;
			}

			// Update the marker
			pExistingMarker->m_dwExitChordVariation &= ~m_pPianoRollStrip->m_dwVariations;
		}
	}

	// By default, newly inserted markers are selected
	pExistingMarker->m_fSelected = TRUE;

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::Delete

HRESULT CVarSwitchStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( CanDelete() == S_OK )
	{
		// Set if something changes
		BOOL fChanged = FALSE;

		// Get first marker
		CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations );

		// Iterate through all markers
		while( !fChanged && pDMMarker )
		{
			// Return TRUE if the marker is selected and belongs to the displayed variations
			if( pDMMarker->m_fSelected && MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
			{
				// Flag that something changed
				m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );
				fChanged = TRUE;
				break;
			}

			// Get a pointer to the next marker
			pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext());
		}

		// Get first marker - m_pDMPart may have changed
		pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations );

		// Iterate through all markers
		while( pDMMarker )
		{
			// Return TRUE if the marker is selected and belongs to the displayed variations
			if( pDMMarker->m_fSelected && MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
			{
				// Remove the marker from the displayed variations
				// Update the current item to point to the next marker
				pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.RemoveMarkerFromVariations( pDMMarker, m_pPianoRollStrip->m_dwVariations );
			}
			else
			{
				// Get a pointer to the next marker
				pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext());
			}
		}

		// If necessary, refresh the display and property page
		if( fChanged )
		{
			RefreshVarSwitchStrips();
			RefreshPropertyPage();

			// Let the object know about the changes
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_DELETE_MARKER ); 
		}

		// Clear m_mtGridToShiftSelFrom, if the marker on that grid was removed
		if( m_mtGridToShiftSelFrom != -1 )
		{
			if( NULL == GetMarkerFromGrid( m_mtGridToShiftSelFrom, m_pPianoRollStrip->m_dwVariations ) )
			{
				m_mtGridToShiftSelFrom = -1;
			}
		}

		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::SelectAll

HRESULT CVarSwitchStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	WORD wRefreshUI = FALSE;

	wRefreshUI = SelectAllVarSwitches( TRUE );

	if( wRefreshUI )
	{
		RefreshVarSwitchStrips();
		RefreshPropertyPage();
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CanCut

HRESULT CVarSwitchStrip::CanCut( void )
{
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CanCopy

HRESULT CVarSwitchStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( IsAnyMarkerSelected() )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CanPaste

HRESULT CVarSwitchStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get cursor time
	long lPasteTime;
	if( FAILED( m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lPasteTime ) ) )
	{
		return E_FAIL;
	}

	// 27768: Disallow pasting in the repeats
	if( lPasteTime >= m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength() )
	{
		return E_FAIL;
	}

	HRESULT hr = S_FALSE;
	
	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( theApp.m_cfStyleMarker );
	}
	// Otherwise, check the clipboard
	else
	{
		// Get the IDataObject
		IDataObject* pIDataObject;
		hr = OleGetClipboard( &pIDataObject );
		if( FAILED( hr ))
		{
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( FAILED( m_pMIDIMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			pIDataObject->Release();
			return E_OUTOFMEMORY;
		}

		// Insert the IDataObject into the TimelineDataObject
		if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
		{
			hr = pITimelineDataObject->IsClipFormatAvailable(theApp.m_cfStyleMarker);
		}
		else
		{
			hr = E_FAIL;
		}

		pITimelineDataObject->Release();
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
// CVarSwitchStrip::CanInsert

HRESULT CVarSwitchStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPianoRollStrip->m_dwVariations == 0 )
	{
		return S_FALSE;
	}

	long lTime;
	if (FAILED(m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
	{
		return E_FAIL;
	}

	// 27768: Disallow inserting in the repeats
	if( lTime >= m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength() )
	{
		return S_FALSE;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CanDelete

HRESULT CVarSwitchStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( IsAnyMarkerSelected() )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CanSelectAll

HRESULT CVarSwitchStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPianoRollStrip->m_dwVariations )
	{
		if( m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations ) )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetData

HRESULT CVarSwitchStrip::GetData( void** ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate our pointers
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_POINTER;
	}

	// Cast ppData to pPropSwitchPoint
	CPropSwitchPoint* pPropSwitchPoint = (CPropSwitchPoint *)*ppData;

	// Flag if anything is selected
	BOOL fAnySelectedMarkers = FALSE;

	// Get a pointer to the first marker
	CDirectMusicStyleMarker* pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();

	// Iterate through all markers
	for( ;  pDMMarker ;  pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext()) )
	{
		// Check if the marker is selected and belongs to a displayed variation
		if( pDMMarker->m_fSelected && MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			// Check if there are no selected markers
			if( !fAnySelectedMarkers )
			{
				// Initialize pPropSwitchPoint with the data from pDMEvent
				pPropSwitchPoint->GetValuesFromDMMarker( pDMMarker, m_pPianoRollStrip->m_dwVariations );

				// Flag that at least one marker is selected
				fAnySelectedMarkers = TRUE;
			}
			else
			{
				// Create a CPropSwitchPoint with the data from pDMEvent
				CPropSwitchPoint propSwitchPoint( pDMMarker, m_pPianoRollStrip->m_dwVariations );

				// Add propSwitchPoint into pPropSwitchPoint (setting the undetermined flags)
				*pPropSwitchPoint += propSwitchPoint;
			}
		}
	}

	// If nothing is selected, return S_FALSE
	if( !fAnySelectedMarkers )
	{
		return S_FALSE;
	}

	// Something's selected, return S_OK
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::SetData

HRESULT CVarSwitchStrip::SetData( void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pData == NULL )
	{
		return E_POINTER;
	}

	CPropSwitchPoint* pPropSwitchPoint = (CPropSwitchPoint *)pData;

	// Assume that something will change
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	// Flags to see what changed
	DWORD dwChanged = 0;

	// Get a pointer to the first marker
	CDirectMusicStyleMarker* pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();

	// Don't allow the user to remove the marker from all types
	if( ((pPropSwitchPoint->m_dwChanged & (CHGD_ENTER | CHGD_EXIT)) && !(pPropSwitchPoint->m_wEnterMarkerFlags & DMUS_MARKERF_START) && !(pPropSwitchPoint->m_wExitMarkerFlags & DMUS_MARKERF_STOP)) )
	{
		pDMMarker = NULL;
	}

	// Iterate through all markers
	for( ;  pDMMarker ;  pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext()) )
	{
		// Check if the marker is selected and belongs to a displayed variation
		if( pDMMarker->m_fSelected && MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			// If not changing variations, make a copy of the marker
			/* Never need to make a copy of the marker, since there's only one on each grid
			if( !(pPropSwitchPoint->m_dwChanged & CHGD_VARIATIONS) )
			{
				m_pPianoRollStrip->m_pPartRef->m_pDMPart->CreateMarkerCopyIfNeeded( pDMMarker, m_pPianoRollStrip->m_dwVariations, FALSE );
			}
			*/

			// Update pDMMarker with the changes
			dwChanged |= pPropSwitchPoint->ApplyValuesToDMMarker( pDMMarker, m_pPianoRollStrip->m_dwVariations );

			// If the marker now belongs to no variations, delete it
			if( MARKER_AND_VARIATION(pDMMarker, ALL_VARIATIONS) == 0 )
			{
				// Save a pointer to the previous item
				CDirectMusicStyleMarker* pDMMarkerPrev = reinterpret_cast<CDirectMusicStyleMarker*>(m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetPrev( pDMMarker ));

				// Remove pDMMarker from the list of markers
				m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.Remove( pDMMarker );

				// Delete pDMMarker
				delete pDMMarker;

				// Set pDMMarker to point to the previous item.  This will make the for(..) loop
				// move on to the item after pDMMarkerPrev (which is the same as the item after pDMMarker).
				pDMMarker = pDMMarkerPrev;

				// If there are no more markers, exit
				if( pDMMarker == NULL )
				{
					break;
				}
			}
		}
	}

	// Always refresh property page.
	RefreshPropertyPage();

	if( dwChanged )
	{
		// Redraw CurveStrips
		RefreshVarSwitchStrips();

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_CHANGE_MARKER ); 
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::OnShowProperties

HRESULT CVarSwitchStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStripMgr != NULL );

	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

	// Get the Curve page manager
	CMarkerPropPageMgr* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_MarkerPropPageManager ) == S_OK )
	{
		pPageManager = (CMarkerPropPageMgr *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CMarkerPropPageMgr( GUID_MarkerPropPageManager );
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus;
	hwndHadFocus = ::GetFocus();

	// Show the Variation switch point properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CMarkerPropPageMgr::sm_nActiveTab;

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
// CVarSwitchStrip::OnRemoveFromPageManager

HRESULT CVarSwitchStrip::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip Selection methods

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::AdjustXPos

int CVarSwitchStrip::AdjustXPos( int nStripXPos )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	VARIANT var;
	m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	long lPartClockLength = CalcPartClockLength();
	long lPatternClockLength = m_pPianoRollStrip->m_pPartRef->m_pPattern->CalcLength();

	if( (lPatternClockLength < lTimelineClockLength)
	||	(lPartClockLength < lTimelineClockLength) )
	{	
		MUSIC_TIME mtTime;
		m_pMIDIMgr->m_pTimeline->PositionToClocks( nStripXPos, &mtTime );

		// Handle patterns shorter than the timeline (pattern tracks)
		mtTime %= m_pPianoRollStrip->m_pPartRef->m_pPattern->CalcLength();

		m_pMIDIMgr->m_pTimeline->ClocksToPosition( mtTime % lPartClockLength, &mtTime );

		return mtTime;
	}
	else
	{
		return nStripXPos;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::AdjustClock

long CVarSwitchStrip::AdjustClock( long lClock )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Handle patterns shorter than the timeline (pattern tracks)
	lClock %= m_pPianoRollStrip->m_pPartRef->m_pPattern->CalcLength();

	return lClock % CalcPartClockLength();
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CalcPartClockLength

long CVarSwitchStrip::CalcPartClockLength( void ) const
{
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef->m_pDMPart != NULL );

	return m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetClockLength();
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::UnselectGutterRange

void CVarSwitchStrip::UnselectGutterRange( void )
{
	ASSERT( m_pMIDIMgr != NULL );
	ASSERT( m_pMIDIMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_fSelecting = TRUE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC = 0xFE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_wSelectionParamType = 0xFFFF;
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pMIDIMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_fSelecting = FALSE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC = 0xFF;
}

/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip Additional methods


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::OnLButtonDown

HRESULT CVarSwitchStrip::OnLButtonDown( WPARAM wParam, long lXPos, long lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	UnselectGutterRange();

	// Find the first marker at the lXPos (doesn't check for vertical position)
	CDirectMusicStyleMarker* pDMMarker = GetMarkerFromPoint( lXPos, lYPos, m_pPianoRollStrip->m_dwVariations );
	
	WORD wRefreshUI = 0;

	if( pDMMarker )
	{
		if ( wParam & MK_SHIFT )
		{
			if( m_mtGridToShiftSelFrom != -1 )
			{
				// Double-check that m_mtGridToShiftSelFrom is valid
				if( NULL == GetMarkerFromGrid( m_mtGridToShiftSelFrom, m_pPianoRollStrip->m_dwVariations ) )
				{
					// Not valid - replace with pDMMarker->m_mtGridStart
					m_mtGridToShiftSelFrom = pDMMarker->m_mtGridStart;
				}

				// Now, select all markers between m_mtGridToShiftSelFrom and pDMMarker->m_mtGridStart, inclusive.
				wRefreshUI = SelectOnlyMarkersBetweenGrids( m_mtGridToShiftSelFrom, pDMMarker->m_mtGridStart );
			}
			else
			{
				m_mtGridToShiftSelFrom = pDMMarker->m_mtGridStart;
			}
		}
		else if ( wParam & MK_CONTROL )
		{
			m_mtGridToShiftSelFrom = pDMMarker->m_mtGridStart;
			wRefreshUI = TRUE;
			pDMMarker->m_fSelected = !pDMMarker->m_fSelected;
		}
		else
		{
			m_mtGridToShiftSelFrom = pDMMarker->m_mtGridStart;
			// If the marker is unselected, unselect all markers
			if( !pDMMarker->m_fSelected )
			{
				SelectAllVarSwitches( FALSE );
				// wRefreshUI will be set to TRUE below (since pDMMarker->m_fSelected is FALSE).
			}
		}

		// Select the marker, if it's not selected (and CTRL was not pressed)
		if( !pDMMarker->m_fSelected
		&&	!(wParam & MK_CONTROL) )
		{
			wRefreshUI = TRUE;
			pDMMarker->m_fSelected = TRUE;
		}
	}
	else
	{
		// No marker clicked on, unselect all
		wRefreshUI = SelectAllVarSwitches( FALSE );

		// Check if the CTRL key is down
		if ( wParam & MK_CONTROL )
		{
			// Yes - insert new marker (Insert() will handle the undo, UI updates, etc.)
			return Insert();
		}
	}

	if( wRefreshUI )
	{
		// Only update the display and property page when something changes
		RefreshVarSwitchStrips();
		RefreshPropertyPage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::OnRButtonUp

HRESULT CVarSwitchStrip::OnRButtonUp( WPARAM wParam, long lXPos, long lYPos )
{
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lStartSelectInRange );
	m_pMIDIMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelectInRange );

	//UnselectGutterRange();

	// Find the first marker at the lXPos (doesn't check for vertical position)
	CDirectMusicStyleMarker* pDMMarker = GetMarkerFromPoint( lXPos, lYPos, m_pPianoRollStrip->m_dwVariations );
	
	if( pDMMarker
	&&	!(wParam & MK_CONTROL)
	&&  !(wParam & MK_SHIFT) )
	{
		m_mtGridToShiftSelFrom = pDMMarker->m_mtGridStart;

		if( pDMMarker->m_fSelected == FALSE )
		{
			// Unselect all markers
			SelectAllVarSwitches( FALSE );

			// Select just this marker
			pDMMarker->m_fSelected = TRUE;

			// Only update the display and property page when something changes
			RefreshVarSwitchStrips();
			RefreshPropertyPage();
		}
	}

	return OnContextMenu();
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::OnContextMenu

HRESULT CVarSwitchStrip::OnContextMenu( void )
{
	ASSERT( m_pStripMgr != NULL );

	HRESULT hr = E_FAIL;

	if( m_pMIDIMgr->m_pTimeline )
	{
		HMENU hMenu = LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MARKER_RMENU));
		if( hMenu )
		{
			HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
			if( hMenuPopup )
			{
				// Enable/disable items as appropriate
				EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ?
								MF_ENABLED : MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ?
								MF_ENABLED : MF_GRAYED );
				// Paste
				EnableMenuItem( hMenuPopup, 2, ( CanPaste() == S_OK ) ?
								MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ?
								MF_ENABLED : MF_GRAYED );
				// Insert
				EnableMenuItem( hMenuPopup, 5, ( CanInsert() == S_OK ) ?
								MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
				EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ?
								MF_ENABLED : MF_GRAYED );

				// Check if there is a valid selection range
				BOOL fHaveValidRange = (m_lStartSelectInRange != m_lEndSelectInRange);
				// Mark Range
				EnableMenuItem( hMenuPopup, 9, fHaveValidRange ?
								MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );
				// Unmark Range
				EnableMenuItem( hMenuPopup, 11, fHaveValidRange ?
								MF_BYPOSITION | MF_ENABLED : MF_BYPOSITION | MF_GRAYED );

				// Mark All
				EnableMenuItem( hMenuPopup, 8, MF_BYPOSITION | MF_ENABLED );
				// Unmark All
				EnableMenuItem( hMenuPopup, 10, MF_BYPOSITION | MF_ENABLED );

				// Merge variations
				EnableMenuItem( hMenuPopup, ID_EDIT_MERGE_VARIATIONS, m_pPianoRollStrip->m_dwVariations ? MF_ENABLED : MF_GRAYED );

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
// CVarSwitchStrip::OnGutterSelectionChange

void CVarSwitchStrip::OnGutterSelectionChange( BOOL fChanged )
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
	||	(m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_bSelectionCC != 0xFE) )
	{
		if( m_pPianoRollStrip->m_lBeginTimelineSelection == m_pPianoRollStrip->m_lEndTimelineSelection 
		||  m_pPianoRollStrip->m_fGutterSelected == FALSE )
		{
			fChanged |= SelectAllVarSwitches( FALSE );
		}
	}

	if( fChanged )
	{
		RefreshVarSwitchStrips();
		RefreshPropertyPage();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::RefreshPropertyPage	

void CVarSwitchStrip::RefreshPropertyPage( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pMIDIMgr
	&&  m_pMIDIMgr->m_pTimeline
	&&  m_pPianoRollStrip->m_fRefreshVarSwitchProperties )
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
// CVarSwitchStrip::RefreshVarSwitchStrips
	
void CVarSwitchStrip::RefreshVarSwitchStrips( void )
{
	// Verify our pointers are valid
	ASSERT( m_pPianoRollStrip != NULL );
	ASSERT( m_pPianoRollStrip->m_pPartRef != NULL );

	// Verify our pointers are valid
	if( m_pMIDIMgr
	&&  m_pMIDIMgr->m_pTimeline )
	{
		// Get the start of the piano roll strip list
		POSITION pos = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each piano roll strip
			CPianoRollStrip* pPianoRollStrip = m_pMIDIMgr->m_pPRSList.GetNext( pos );

			ASSERT( pPianoRollStrip->m_pPartRef != NULL );

			// Check if this piano roll strip is pointing to the same part
			if( pPianoRollStrip->m_pPartRef->m_pDMPart == m_pPianoRollStrip->m_pPartRef->m_pDMPart )
			{
				// Yep - redraw its variation switch strip
				m_pMIDIMgr->m_pTimeline->StripInvalidateRect( pPianoRollStrip->m_pVarSwitchStrip, NULL, FALSE );
			}
		}
	}

	m_pMIDIMgr->UpdateStatusBarDisplay();
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::SelectAllVarSwitches

WORD CVarSwitchStrip::SelectAllVarSwitches( BOOL fState )
{
	WORD wRefreshUI = FALSE;

	CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();
	for( ;  pDMMarker ;  pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext()) )
	{
		if( MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			if( pDMMarker->m_fSelected != fState )
			{
				pDMMarker->m_fSelected = fState;

				wRefreshUI = TRUE;
			}
		}
	}

	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetMarkerFromGrid

CDirectMusicStyleMarker* CVarSwitchStrip::GetMarkerFromGrid( MUSIC_TIME mtGrid, DWORD dwVariations ) const
{
	CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();

	while( pDMMarker )
	{
		if( (pDMMarker->m_mtGridStart == mtGrid)
		&&	MARKER_AND_VARIATION(pDMMarker, dwVariations) )
		{
			return pDMMarker;
		}
		else if( pDMMarker->m_mtGridStart > mtGrid )
		{
			break;
		}
		pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext());
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::CreateNewMarker

CDirectMusicStyleMarker* CVarSwitchStrip::CreateNewMarker( MUSIC_TIME mtGrid )
{
	CDirectMusicStyleMarker *pDMMarker = new CDirectMusicStyleMarker();
	pDMMarker->m_mtGridStart = mtGrid;
	pDMMarker->m_fSelected = TRUE;
	m_pPianoRollStrip->m_pPartRef->m_pDMPart->InsertMarkerInAscendingOrder( pDMMarker );

	return pDMMarker;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetGridRect

void CVarSwitchStrip::GetGridRect( MUSIC_TIME mtGrid, RECT *pRectMarker, MUSIC_TIME mtClockOffset, MUSIC_TIME mtMax ) const
{
	ASSERT( pRectMarker );
	if( m_pMIDIMgr && m_pMIDIMgr->m_pTimeline )
	{
		// Convert from a grid to clocks
		long lTime = mtClockOffset + GRID_TO_CLOCKS( mtGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );

		// Convert from clocks to pixels
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( lTime, &pRectMarker->left );

		// Find the end of this grid (in clocks)
		lTime = min( mtMax, lTime + m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_mtClocksPerGrid);

		// Convert from clocks to pixels
		m_pMIDIMgr->m_pTimeline->ClocksToPosition( lTime, &pRectMarker->right );
	}
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetMarkerFromPoint

CDirectMusicStyleMarker* CVarSwitchStrip::GetMarkerFromPoint( long lXPos, long lYPos, DWORD dwVariations ) const
{
	UNREFERENCED_PARAMETER(lYPos);

	long lGrid;
	m_pMIDIMgr->m_pTimeline->PositionToClocks( lXPos, &lGrid );
	lGrid = CLOCKS_TO_GRID( lGrid, m_pPianoRollStrip->m_pPartRef->m_pDMPart );

	return GetMarkerFromGrid( lGrid, dwVariations );
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::IsAnyMarkerSelected

BOOL CVarSwitchStrip::IsAnyMarkerSelected( void ) const
{
	if( GetEarliestSelectedMarker() )
	{
		return TRUE;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::SelectOnlyMarkersBetweenGrids

WORD CVarSwitchStrip::SelectOnlyMarkersBetweenGrids( MUSIC_TIME mtGridStart, MUSIC_TIME mtGridEnd )
{
	// Ensure the times are in order
	if( mtGridEnd < mtGridStart )
	{
		MUSIC_TIME mtTemp = mtGridStart;
		mtGridStart = mtGridEnd;
		mtGridEnd = mtTemp;
	}

	// Get first marker
	CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations );

	// Whether anything changed
	WORD wRefreshUI = 0;

	// Iterate through all markers
	while( pDMMarker )
	{
		// Check if the marker belongs to one of the displayed variations
		if( MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			// Check if the marker is within the time range
			if( (pDMMarker->m_mtGridStart <= mtGridEnd)
			&&	(pDMMarker->m_mtGridStart >= mtGridStart) )
			{
				// Check if the marker is unselectd
				if( !pDMMarker->m_fSelected )
				{
					// Flag to update the UI and select the marker
					wRefreshUI = TRUE;
					pDMMarker->m_fSelected = TRUE;
				}
			}
			// Check if the marker is selected
			else if( pDMMarker->m_fSelected )
			{
				// Flag to update the UI and unselect the marker
				wRefreshUI = TRUE;
				pDMMarker->m_fSelected = FALSE;
			}
		}

		// Get a pointer to the next marker
		pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext());
	}

	// Return whether or not to update the UI
	return wRefreshUI;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::MarkAllHelper

HRESULT CVarSwitchStrip::MarkAllHelper( BOOL fInsert, DWORD dwFlags )
{
	// Calculate the length of the part
	MUSIC_TIME mtGridLength = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetGridLength();

	MarkTimeHelper( fInsert, dwFlags, 0, mtGridLength );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::MarkRangeHelper

HRESULT CVarSwitchStrip::MarkRangeHelper( BOOL fInsert, DWORD dwFlags )
{
	// Convert from clocks to grids
	MUSIC_TIME mtGridStart = CLOCKS_TO_GRID( m_lStartSelectInRange, m_pPianoRollStrip->m_pPartRef->m_pDMPart);
	MUSIC_TIME mtGridEnd = CLOCKS_TO_GRID( m_lEndSelectInRange, m_pPianoRollStrip->m_pPartRef->m_pDMPart);

	// Calculate the length of the part
	MUSIC_TIME mtGridLength = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetGridLength();

	// Calculate the span of the selection range
	MUSIC_TIME mtGridSpan = mtGridEnd - mtGridStart;

	// Ensure mtGridStart is within the part length
	mtGridStart %= mtGridLength;

	// Update mtGridEnd to the length of the part, or the end of the selection,
	// whichever is lesser
	mtGridEnd = min( mtGridStart + mtGridSpan + 1, mtGridLength );

	// Now, actually mark the time range
	return MarkTimeHelper( fInsert, dwFlags, mtGridStart, mtGridEnd );
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::MarkTimeHelper

HRESULT CVarSwitchStrip::MarkTimeHelper( BOOL fInsert, DWORD dwFlags, MUSIC_TIME mtStart, MUSIC_TIME mtEnd )
{
	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();

	ASSERT( m_pPianoRollStrip != NULL );
	if ( m_pPianoRollStrip == NULL )
	{
		return E_UNEXPECTED;
	}
	
	// Can't insert into 0 variations
	if( m_pPianoRollStrip->m_dwVariations == 0 )
	{
		return S_FALSE;
	}

	// Unselect all markers
	SelectAllVarSwitches( FALSE );

	// Assume something will change
	m_pMIDIMgr->PreChangePartRef( m_pPianoRollStrip->m_pPartRef );

	MUSIC_TIME mtIncrement = 1;
	switch( dwFlags )
	{
	case DMUS_SEGF_MEASURE:
		mtIncrement = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature.m_bBeatsPerMeasure * m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat;
		break;
	case DMUS_SEGF_BEAT:
		mtIncrement = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat;
		break;
	}

	// Ensure mtStart starts on the next even increment on or after mtStart
	mtStart = mtIncrement * ((mtStart + mtIncrement - 1) / mtIncrement);

	BOOL fChanged = FALSE;

	if( fInsert )
	{
		for( MUSIC_TIME mtGrid = mtStart; mtGrid < mtEnd; mtGrid += mtIncrement )
		{
			fChanged |= InsertMarkerAtGrid( DMUS_MARKERF_START | DMUS_MARKERF_STOP, mtGrid );
		}

		// Let the object know about the changes, if necessary
		if( fChanged )
		{
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_INSERT_MARKER ); 
		}
	}
	else
	{
		for( MUSIC_TIME mtGrid = mtStart; mtGrid < mtEnd; mtGrid += mtIncrement )
		{
			CDirectMusicStyleMarker *pDMMarker = GetMarkerFromGrid( mtGrid, m_pPianoRollStrip->m_dwVariations );
			if( pDMMarker )
			{
				// Since there is something to remove, we definitely will change
				fChanged = TRUE;

				m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.RemoveMarkerFromVariations( pDMMarker, m_pPianoRollStrip->m_dwVariations );
			}
		}

		// Let the object know about the changes, if necessary
		if( fChanged )
		{
			m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_DELETE_MARKER ); 
		}
	}

	RefreshVarSwitchStrips();
	RefreshPropertyPage();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::GetEarliestSelectedMarker

CDirectMusicStyleMarker* CVarSwitchStrip::GetEarliestSelectedMarker( void ) const
{
	CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->GetFirstMarker( m_pPianoRollStrip->m_dwVariations );

	while( pDMMarker )
	{
		if( pDMMarker->m_fSelected
		&&	MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			return pDMMarker;
		}

		pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext());
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// CVarSwitchStrip::DeleteMarkersBetweenTimes

void CVarSwitchStrip::DeleteMarkersBetweenTimes( MUSIC_TIME mtGridEarliestMarker, MUSIC_TIME mtGridLatestMarker )
{
	CDirectMusicStyleMarker *pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.GetHead();

	// Iterate through all markers
	while( pDMMarker )
	{
		// Check if the marker is within the correct time range and is being displayed
		if( (pDMMarker->m_mtGridStart >= mtGridEarliestMarker)
		&&	(pDMMarker->m_mtGridStart <= mtGridLatestMarker)
		&&	MARKER_AND_VARIATION(pDMMarker, m_pPianoRollStrip->m_dwVariations) )
		{
			// Yes - remove the marker from the displayed variations
			pDMMarker = m_pPianoRollStrip->m_pPartRef->m_pDMPart->m_lstMarkers.RemoveMarkerFromVariations( pDMMarker, m_pPianoRollStrip->m_dwVariations );
		}
		//  Check if we've passed mtGridLatestMarker
		else if( pDMMarker->m_mtGridStart > mtGridLatestMarker )
		{
			// Yes - return
			break;
		}
		else
		{
			// No - continue on
			pDMMarker = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext());
		}
	}
}
