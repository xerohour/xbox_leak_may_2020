// MelGenStrip.cpp : Implementation of CMelGenStrip
#include "stdafx.h"
#include "MelGenIO.h"
#include "MelGenStripMgr.h"
#include "MelGenMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "PropPageMelGenFlags.h"
#include <RiffStrm.h>
#include <StyleDesigner.h>
#include <SegmentGuids.h>
#include "SegmentIO.h"
#include <SegmentDesigner.h>
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STRIP_HEIGHT 20

CString GetName(DWORD dwGroupBits, CString strName)
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

	return strText + CString(": ") + strName;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip constructor/destructor

CMelGenStrip::CMelGenStrip( CMelGenMgr* pMelGenMgr )
{
	ASSERT( pMelGenMgr );
	if ( pMelGenMgr == NULL )
	{
		return;
	}

	m_pMelGenMgr = pMelGenMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pMelGenMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;
	m_bGutterSelected = FALSE;

	m_cfMelGenList = 0;
	m_cfStyle = 0;

	m_bSelecting = FALSE;
	m_bContextMenuPaste = FALSE;
	m_pISourceDataObject = NULL;
	m_pITargetDataObject = NULL;
	m_nStripIsDragDropSource = 0;
	m_dwStartDragButton = 0;
	m_lStartDragPosition = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_nLastEdit = 0;
	m_fShowMelGenProps = FALSE;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;
	m_fComposeButtonDown = FALSE;

	// Initialize bitmap
	m_bmCompose.LoadBitmap( IDB_MELODY );

	BITMAP bmParam;
	m_bmCompose.GetBitmap( &bmParam );
	m_bmCompose.SetBitmapDimension( bmParam.bmWidth, bmParam.bmHeight );
}

CMelGenStrip::~CMelGenStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pMelGenMgr = NULL;
	}
	RELEASE( m_pISourceDataObject );
	RELEASE( m_pITargetDataObject );
	RELEASE( m_pPropPageMgr );

	m_bmCompose.DeleteObject();
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::QueryInterface

STDMETHODIMP CMelGenStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	ASSERT( ppv );
	if ( ppv == NULL )
	{
		return E_INVALIDARG;
	}

    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip*) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdPropPageObject))
	{
        *ppv = (IUnknown *) (IDMUSProdPropPageObject *) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdTimelineEdit ))
	{
		*ppv = (IDMUSProdTimelineEdit*) this;
	}
	else if( IsEqualIID( riid, IID_IDropSource ))
	{
		*ppv = (IDropSource*) this;
	}
	else if( IsEqualIID( riid, IID_IDropTarget ))
	{
		*ppv = (IDropTarget*) this;
	}
	else
	{
		return E_NOTIMPL;
	}

    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::AddRef

STDMETHODIMP_(ULONG) CMelGenStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Release

STDMETHODIMP_(ULONG) CMelGenStrip::Release(void)
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
// CMelGenStrip IStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Draw

HRESULT	STDMETHODCALLTYPE CMelGenStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fUseGutterSelectRange = FALSE;
	if( m_bGutterSelected
	&&  m_lGutterBeginSelect != m_lGutterEndSelect )
	{
		fUseGutterSelectRange = TRUE;
	}

	CString strText;

	// Validate pointer to the Timeline
	if( m_pMelGenMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pMelGenMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pMelGenMgr->m_dwGroupBits, 0, lXOffset );

		// Validate our device context
		if( hDC )
		{
			RECT	rectClip, rectHighlight;
			long	lStartTime, lPosition;
			CMelGenItem* pMelGenItem;
			SIZE	sizeText;

			::GetClipBox( hDC, &rectClip );
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;

			m_pMelGenMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

			// Iterate through all melody fragments
			POSITION pos = m_pMelGenMgr->m_lstMelGens.GetHeadPosition();
			while( pos )
			{
				pMelGenItem = m_pMelGenMgr->m_lstMelGens.GetNext( pos );

				// Compute the position of the melody fragments
				m_pMelGenMgr->m_pTimeline->MeasureBeatToPosition( m_pMelGenMgr->m_dwGroupBits, 0, pMelGenItem->m_dwMeasure, pMelGenItem->m_bBeat, &lPosition );

				// If it's not visible, break out of the loop
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				// Otherwise, draw it one pixel to the right (so it doesn't overwrite the measure line)
				pMelGenItem->FormatTextForStrip( strText );
				::TextOut( hDC, lPosition - lXOffset + 1, 0, strText, strText.GetLength() );
			}

			// make sure the first selected MelGen is shown in its entirety
			pMelGenItem = m_pMelGenMgr->FirstSelectedMelGen();
			if( pMelGenItem )
			{
				pos = m_pMelGenMgr->m_lstMelGens.Find( pMelGenItem, NULL );
				while( pos )
				{
					if( pMelGenItem->m_fSelected )
					{
						long lBeatLength = 0;
						m_pMelGenMgr->m_pTimeline->MeasureBeatToPosition( m_pMelGenMgr->m_dwGroupBits, 0,
														  pMelGenItem->m_dwMeasure, pMelGenItem->m_bBeat, &lPosition );
						m_pMelGenMgr->m_pTimeline->MeasureBeatToPosition( m_pMelGenMgr->m_dwGroupBits, 0,
														 pMelGenItem->m_dwMeasure, (pMelGenItem->m_bBeat) + 1, &lBeatLength );
						lBeatLength -= lPosition;

						// find extent of text
						pMelGenItem->FormatTextForStrip( strText );
						::GetTextExtentPoint32( hDC, strText, strText.GetLength(), &sizeText );
						if( sizeText.cx < lBeatLength )
						{
							sizeText.cx = lBeatLength;
						}
						long lExtent = lPosition + sizeText.cx;

						// truncate if a selected MelGen covers part of text
						POSITION pos2 = pos;
						CMelGenItem* pTempMelGen;
						bool fDone = false;
						while( pos2 && !fDone)
						{
							m_pMelGenMgr->m_lstMelGens.GetNext( pos2 );
							if (!pos2) break;
							pTempMelGen = m_pMelGenMgr->m_lstMelGens.GetAt( pos2 );
							if( pTempMelGen->m_fSelected )
							{	
								long lRPos;
								m_pMelGenMgr->m_pTimeline->MeasureBeatToPosition( m_pMelGenMgr->m_dwGroupBits, 0, pTempMelGen->m_dwMeasure, pTempMelGen->m_bBeat, &lRPos );
								if( lRPos <= lExtent )
								{
									lExtent = lRPos;
								}
								fDone = true;
							}
						}

						if( fUseGutterSelectRange == FALSE )
						{
							rectHighlight.left = lPosition - lXOffset;
							rectHighlight.right = lExtent - lXOffset;
							pMelGenItem->FormatTextForStrip( strText );
							::DrawText( hDC, strText, strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
							GrayOutRect( hDC, &rectHighlight );
						}
					}
					m_pMelGenMgr->m_lstMelGens.GetNext( pos );
					if (pos) pMelGenItem = m_pMelGenMgr->m_lstMelGens.GetAt( pos );
				}
			}

			// Highlight the selected range if there is one.
			if( fUseGutterSelectRange )
			{
				m_pMelGenMgr->m_pTimeline->ClocksToPosition( m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterEndSelect : m_lGutterBeginSelect,
											 &(rectHighlight.left));
				m_pMelGenMgr->m_pTimeline->ClocksToPosition( m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterBeginSelect : m_lGutterEndSelect,
											 &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;

				// Invert it.
				GrayOutRect( hDC, &rectHighlight );
			}

		}
	}
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CMelGenStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		// We are not resizable
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;

	case SP_GUTTERSELECTABLE:
		// We support gutter selection
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;

	case SP_MINMAXABLE:
		// We don't support Minimize/maximize
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;

	case SP_DEFAULTHEIGHT:
	case SP_MAXHEIGHT:
	case SP_MINHEIGHT:
		// Our height is 20 pixels
		pvar->vt = VT_INT;
		V_INT(pvar) = STRIP_HEIGHT;
		break;

	case SP_NAME:
		{
			BSTR bstr;
			// TODO: Make into a String resource
			CString str = GetName(m_pMelGenMgr->m_dwGroupBits, CString("Melody Formulation"));

			pvar->vt = VT_BSTR; 
			try
			{
				bstr = str.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;

	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pMelGenMgr )
		{
			m_pMelGenMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		}
		else
		{
			V_UNKNOWN(pvar) = NULL;
		}
		break;

/*	case SP_LAST_EDIT_BSTR:
		{
			HINSTANCE hinst = _Module.GetModuleInstance();
			BSTR bstr;
			CString str;
			TCHAR buf[256];
			::LoadString(hinst, m_nLastEdit, buf, 256);
			str = buf;
//			str.LoadString(m_nLastEdit);
			pvar->vt = VT_BSTR;
			try
			{
				bstr = str.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;*/

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CMelGenStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	switch( sp )
	{
/*	case SP_CLEARSELECT:
		m_lGutterBeginSelect = -1;
		m_lGutterEndSelect = -1;
		m_pMelGenMgr->UnselectAll();
		break;*/

	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		if( var.vt != VT_I4)
		{
			return E_FAIL;
		}
		if( sp == SP_BEGINSELECT )
		{
			m_lGutterBeginSelect = V_I4( &var );
		}
		else
		{
			m_lGutterEndSelect = V_I4( &var );
		}

		if( m_bSelecting )
		{
			break;
		}

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pMelGenMgr->UnselectAll();
			m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pMelGenMgr->UnselectAll();
			m_pMelGenMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pMelGenMgr->UnselectAll();
		}
		m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pMelGenMgr->m_pPropPageMgr != NULL )
		{
			m_pMelGenMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pMelGenMgr->UnselectAll();
			m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pMelGenMgr->UnselectAll();
			m_pMelGenMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pMelGenMgr->UnselectAll();
		}
		m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pMelGenMgr->m_pPropPageMgr != NULL )
		{
			m_pMelGenMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetMelGenFromMeasureBeat

CMelGenItem *CMelGenStrip::GetMelGenFromMeasureBeat( DWORD dwMeasure, BYTE bBeat )
{
	
	CMelGenItem* pMelGenItem = NULL;

	POSITION pos = m_pMelGenMgr->m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pMelGenItem = m_pMelGenMgr->m_lstMelGens.GetNext( pos );
		ASSERT( pMelGenItem != NULL );
		if ( pMelGenItem != NULL )
		{
			if ( pMelGenItem->m_dwMeasure == dwMeasure &&
				 pMelGenItem->m_bBeat == bBeat )
			{
				return pMelGenItem;
			}
			else if ( pMelGenItem->m_dwMeasure > dwMeasure )
			{
				break;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetMelGenFromPoint

CMelGenItem *CMelGenStrip::GetMelGenFromPoint( long lPos )
{
	//IDMUSProdTimeline*	pTimeline;

	CMelGenItem*		pMelGenReturn = NULL;

	//if( SUCCEEDED( m_pStripMgr->GetClientTimeline( &pTimeline ) ))
	if (m_pMelGenMgr->m_pTimeline)
	{
		CMelGenItem* pMelGenItem = NULL;
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->PositionToMeasureBeat( m_pMelGenMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pMelGenReturn = GetMelGenFromMeasureBeat( lMeasure, (BYTE)lBeat );
		}
		//RELEASE( pTimeline );
	}
	return pMelGenReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CMelGenStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	CMelGenItem* pMelGen = NULL;
//	ITimelineCtl* pTimeline;
//	if( FAILED( m_pMelGenMgr->GetClientTimeline( &pTimeline )))
	if (!m_pMelGenMgr->m_pTimeline)
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		m_fShowMelGenProps = TRUE;
		hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_RBUTTONDOWN:
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_RBUTTONUP:
		// Display a right-click context menu.
		POINT pt;
		BOOL	bResult;
		// Get the cursor position (To put the menu there)
		bResult = GetCursorPos( &pt );
		ASSERT( bResult );
		if( !bResult )
		{
			hr = E_UNEXPECTED;
			break;
		}

		// Save the position of the click so we know where to insert a MelGen. if Insert is selected.
		m_lXPos = lXPos;
		m_pMelGenMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);

		hr = S_OK;
		break;

	case WM_MOUSEMOVE:
		if( m_dwStartDragButton )
		{
			m_nStripIsDragDropSource = 1;
	
			if( DoDragDrop() )
			{
				// Redraw the strip and refresh the MelGen property page
				m_pMelGenMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pMelGenMgr->OnShowProperties();
				if( m_pMelGenMgr->m_pPropPageMgr )
				{
					m_pMelGenMgr->m_pPropPageMgr->RefreshData();
				}
			}

			m_dwStartDragButton = 0;
			m_nStripIsDragDropSource = 0;
		}
		break;

	case WM_COMMAND:
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			hr = DisplayPropertySheet(m_pMelGenMgr->m_pTimeline);
			if (m_fShowMelGenProps)
			{
				// Change to the Melody formulation reference property page
				m_pMelGenMgr->OnShowProperties();
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
		case ID_EDIT_PASTE:
			if( m_lXPos >= 0 )
			{
				m_bContextMenuPaste = TRUE;
			}
			hr = Paste();
			m_bContextMenuPaste = FALSE;
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		default:
			break;
		}
		break;

	case WM_LBUTTONUP:
		m_lXPos = lXPos;
		hr = S_OK;
		break;

	case WM_CREATE:
		m_cfMelGenList = RegisterClipboardFormat( CF_MELGENLIST );
		m_cfStyle = RegisterClipboardFormat( CF_STYLE);

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pMelGenMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pMelGenMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );

		/*// Unselect all
		m_pTimeSigMgr->UnselectAll();

		// Reset m_dwShiftSelectFromMeasure
		m_dwShiftSelectFromMeasure = 0;*/
		break;

	default:
		break;
	}

	//RELEASE( pTimeline );
	return hr;
}

/*
/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnAddedToTimeline

HRESULT STDMETHODCALLTYPE CMelGenStrip::OnAddedToTimeline(void)
{
	m_cfMelGenList = RegisterClipboardFormat( CF_MELGENLIST );
	m_cfStyle = RegisterClipboardFormat( CF_STYLE);

	// Get Left and right selection boundaries
	m_bGutterSelected = FALSE;
	m_pMelGenMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
	m_pMelGenMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );

	return S_OK;
}
*/

/*
/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetStripMgr

HRESULT STDMETHODCALLTYPE CMelGenStrip::GetStripMgr( IStripMgr** ppIStripMgr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !ppIStripMgr )
	{
		return E_POINTER;
	}
	*ppIStripMgr = m_pStripMgr;
	if( m_pStripMgr )
	{
		(*ppIStripMgr)->AddRef();
	}
	return S_OK;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip IStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::FBDraw

HRESULT CMelGenStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(sv);

	// get function bar rect
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if (FAILED(m_pMelGenMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
	{
		return E_FAIL;
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
		return E_FAIL;
	}
	::SelectClipRgn( hDC, hRgn );
	::DeleteObject( hRgn );

	::DrawState( hDC, NULL, NULL, LPARAM (m_bmCompose.GetSafeHandle()), NULL,
		rectFBar.right - m_bmCompose.GetBitmapDimension().cx - 3 + m_fComposeButtonDown, m_fComposeButtonDown,
		m_bmCompose.GetBitmapDimension().cx, m_bmCompose.GetBitmapDimension().cy, DST_BITMAP | DSS_NORMAL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::FBOnWMMessage

HRESULT CMelGenStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
/*		m_fShowMelGenProps = FALSE;
		OnShowProperties();
		break;*/
		{
			BOOL fShowProps = TRUE;
			if( lYPos <= m_bmCompose.GetBitmapDimension().cy )
			{
				VARIANT varXS;
				if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
				{
					if( lXPos >= V_I4( &varXS ) - m_bmCompose.GetBitmapDimension().cx - 3 )
					{
						if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &varXS ) ) )
						{
							IDMUSProdNode *pSegmentNode = NULL;
							if( SUCCEEDED( V_UNKNOWN( &varXS )->QueryInterface( IID_IDMUSProdNode, (void**)&pSegmentNode ) ) )
							{
								fShowProps = FALSE;
								m_fComposeButtonDown = TRUE;

								// capture mouse so we get the LBUTTONUP message as well
								// the timeline will release the capture when it receives the
								// LBUTTONUP message
								VARIANT var;
								var.vt = VT_BOOL;
								V_BOOL(&var) = TRUE;
								m_pMelGenMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

								m_pMelGenMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

								// Compose Pattern track
								hr = ComposeMelody(pSegmentNode);
								if( FAILED( hr ) || (hr == S_FALSE) )
								{
									// An argument is wrong (no valid personality and/or style)
									HWND hwndOld;
									CString strWarning;

									// Store the window with the focus so it gets it back.
									hwndOld = ::GetFocus();

									if( hr == E_ABORT )
									{
										// Segment is playing
										strWarning.LoadString(IDS_ERR_COMPOSE_WHILE_PLAYING);
									}
									else if ( hr == S_FALSE )
									{
										// No melody was generated
										strWarning.LoadString(IDS_ERR_COMPOSE_NOMELODY);
									}
									else if( hr == E_UNEXPECTED )
									{
										// An unexpected error occurred
										strWarning.LoadString(IDS_ERR_COMPOSE_UNEXPECTED);
									}
									else if( hr == E_INVALIDARG )
									{
										// No style and/or no chordmap
										strWarning.LoadString(IDS_ERR_COMPOSE_NOSTYLE_CHORDMAP);
									}
									else if( hr == DMUS_E_NOT_FOUND )
									{
										// No style and/or no chordmap
										strWarning.LoadString(IDS_ERR_COMPOSE_NOSTYLE_CHORDMAP);
									}
									else
									{
										// Shouldn't happen
										// DMUS_E_NOT_FOUND
										TRACE("SignPost Strip: Received an unknown error code %x\n", hr );
										//ASSERT( FALSE );
										strWarning.LoadString(IDS_ERR_COMPOSE_UNEXPECTED);
									}
									::AfxMessageBox(strWarning);
									::SetFocus(hwndOld);
									hr = S_OK;

									m_fComposeButtonDown = FALSE;
									m_pMelGenMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
								}
								pSegmentNode->Release();
								V_UNKNOWN( &varXS )->Release();
							}
						}
					}
				}
			}
			if( fShowProps )
			{
				m_fShowMelGenProps = FALSE;
				OnShowProperties();
			}
		}
		break;

	case WM_RBUTTONUP:
		m_fShowMelGenProps = FALSE;
		OnShowProperties();
		m_lXPos = -1;

		// Display a right-click context menu.
		POINT pt;
		// Get the cursor position (To put the menu there)
		if( !GetCursorPos( &pt ) )
		{
			hr = E_UNEXPECTED;
			break;
		}

//		ITimelineCtl* pTimeline;
		//if( SUCCEEDED( m_pMelGenMgr->GetClientTimeline( &pTimeline )))
		if(  m_pMelGenMgr->m_pTimeline )
		{
			m_pMelGenMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
			//RELEASE( pTimeline );
		}
		break;

	case WM_LBUTTONUP:
		if( m_fComposeButtonDown )
		{
			m_fComposeButtonDown = FALSE;
			m_pMelGenMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::ComposeMelody

HRESULT CMelGenStrip::ComposeMelody( IDMUSProdNode* pSegmentNode )
{
    DWORD dw = 0;
    if (!m_pMelGenMgr->m_pTimeline)
    {
		return E_UNEXPECTED;
    }
	IDirectMusicSegment8* pSegment = NULL;
	HRESULT hr = pSegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, (void**)&pSegment);
	if (FAILED(hr)) // couldn't get a segment
	{
		return E_UNEXPECTED;
	}
	IDirectMusicStyle *pDMStyleForComposition = NULL;
	::CoCreateInstance( CLSID_DirectMusicStyle, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicStyle, (void**)&pDMStyleForComposition );
	if( !pDMStyleForComposition )
	{
		return E_UNEXPECTED;
	}
	IDirectMusicStyle8P *pDMStyleComposer = NULL;
	if (FAILED(pDMStyleForComposition->QueryInterface(IID_IDirectMusicStyle8P, (void**)&pDMStyleComposer)))
	{
		pDMStyleForComposition->Release();
		return E_UNEXPECTED;
	}
	pDMStyleForComposition->Release();

	IDirectMusicStyle *pIDMStyle = NULL;
	IDirectMusicSegment* pIDMComposedSegment = NULL;
	IDirectMusicTrack* pIDMTrack = NULL;
	IDirectMusicTrack* pIDMComposedTrack = NULL;
	IPersistStream* pIPersistStream = NULL;
	IStream* pIStream = NULL;
	IDMUSProdStripMgr* pIStripMgr = NULL;
	DWORD dwComposedTrackGroup = 0xffffffff;
	IDirectMusicTrack* pStyleTrack = NULL;
	IDMUSProdSegmentEdit8* pISegmentEdit = NULL;

        // Find the track group of the Melody Formulation track; composed track will have this group
	if( SUCCEEDED( pSegment->GetTrack( CLSID_DirectMusicMelodyFormulationTrack, dwComposedTrackGroup, 0, &pIDMTrack ) ) )
	{
		pSegment->GetTrackGroup( pIDMTrack, &dwComposedTrackGroup );
		pIDMTrack->Release();
		pIDMTrack = NULL;
	}
	// Remove the existing composed Track, if any
	if( SUCCEEDED( pSegment->GetTrack( CLSID_DirectMusicPatternTrack, dwComposedTrackGroup, 0, &pIDMTrack ) ) )
	{
		pSegment->RemoveTrack( pIDMTrack );
	}
	// Compose a segment with a new Track
	hr = pDMStyleComposer->ComposeMelodyFromTemplate( pIDMStyle, pSegment, &pIDMComposedSegment );
	if( FAILED( hr ) )
	{
		if( pIDMTrack )
		{
			// Add the track back.
			pSegment->InsertTrack( pIDMTrack, dwComposedTrackGroup );
		}
		goto ON_ERROR;
	}

	if( FAILED( hr = pSegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void**)&pISegmentEdit ) ) )
	{
        goto ON_ERROR;
    }

	if( pIDMTrack )
	{
		// Old composed track exists, update the strip with the new track

		// Find the new Track
		if( FAILED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicPatternTrack, dwComposedTrackGroup, 0, &pIDMComposedTrack) ) )
		{
			TRACE("Whoops, can't find a composed track in a composed segment.\n");
			// Add the track back.
			pSegment->InsertTrack( pIDMTrack, dwComposedTrackGroup );
			hr = S_FALSE;
			goto ON_ERROR;
		}

        // Get the strip manager for the existing track.
		m_pMelGenMgr->m_pTimeline->GetStripMgr(CLSID_DirectMusicPatternTrack, dwComposedTrackGroup, 0, &pIStripMgr );
		if( pIStripMgr == NULL )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}
	}
	else
	{
		// Created a new Track, add it to the display and our internal list of tracks.
		if( FAILED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicPatternTrack, dwComposedTrackGroup, 0, &pIDMComposedTrack) ) )
		{
			TRACE("Whoops, can't find a track in a composed segment.\n");
			hr = S_FALSE;
			goto ON_ERROR;
		}

        // Create a new strip manager for the track.
		IUnknown *punkStripMgr = NULL;

		// Create a Pattern track
		if( SUCCEEDED ( pISegmentEdit->AddStrip( CLSID_DirectMusicPatternTrack, dwComposedTrackGroup, &punkStripMgr ) ) )
		{
            punkStripMgr->QueryInterface(IID_IDMUSProdStripMgr, (void**) &pIStripMgr);
			punkStripMgr->Release();
		}
		if( pIStripMgr == NULL )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}
	}

    // Get the track from the Strip manager, and reload the newly composed track data.
    // The contents of the new track need to be filled in using Segment::Compose as
    // a model: write a patterntrack::Load(NULL) that copies data into the track.
	VARIANT varDMTrack;
	varDMTrack.vt = VT_UNKNOWN;
	V_UNKNOWN( &varDMTrack ) = pIDMComposedTrack;
	hr = pIStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack );
	if( FAILED( hr ) )
	{
		hr = E_UNEXPECTED;
		goto ON_ERROR;
	}

    if( FAILED( pIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
	{
		hr = E_UNEXPECTED;
		goto ON_ERROR;
	}

	// This causes the Pattern Strip to load the pattern from its attached IDirectMusicTrack.
	if( FAILED( pIPersistStream->Load( NULL ) ) )
	{
		hr = E_UNEXPECTED;
		goto ON_ERROR;
	}
    else
    {
        // Let the timeline display the strip
		VARIANT varTimeline;
		varTimeline.vt = VT_UNKNOWN;
		m_pMelGenMgr->m_pTimeline->QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
		pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
        m_pMelGenMgr->m_pTimeline->Release();
        // Let the pattern track give MIDI values to its note events, so they will display properly
        pIStripMgr->OnUpdate( GUID_Segment_AllTracksAdded, dwComposedTrackGroup, NULL );
        IUnknown* pUnk = NULL;
        if (SUCCEEDED(pIStripMgr->QueryInterface(IID_IUnknown, (void**)&pUnk)))
        {
            IUnknown* pUnkTrack = NULL;
            if (SUCCEEDED(pIDMComposedTrack->QueryInterface(IID_IUnknown, (void**)&pUnkTrack)))
            {
                // Replace the track in the editor
                pISegmentEdit->ReplaceTrackInStrip(pUnk, pUnkTrack);
                pUnkTrack->Release();
            }
	        // Let the timeline know about the changes
	        m_pMelGenMgr->m_pTimeline->OnDataChanged(pUnk);
            pUnk->Release();
        }
    }
    
    // Release the strip manager so it can be reused for style ref strips.
    if (pIStripMgr)
    {
        pIStripMgr->Release();
        pIStripMgr = NULL;
    }

    // Disable playback of any style tracks in the track group of the composed track.
    for (dw = 0; dw < 32; dw++)
    {
        DWORD dwStyleGroup = (1 << dw);
        if ( dwStyleGroup & dwComposedTrackGroup)
        {
            IDirectMusicTrack* pStyleTrack = NULL;
            DWORD dwIndex = 0;
        	while( SUCCEEDED( pSegment->GetTrack( CLSID_DirectMusicStyleTrack, dwStyleGroup, dwIndex, &pStyleTrack ) ) )
            {
				m_pMelGenMgr->m_pTimeline->GetStripMgr(CLSID_DirectMusicStyleTrack, dwStyleGroup, dwIndex, &pIStripMgr );
				if( pIStripMgr == NULL )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}
            	DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
	            ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
            	VARIANT varTrackHeader;
	            varTrackHeader.vt = VT_BYREF;
	            V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
			    if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader ) ) )
			    {
				    ioTrackExtrasHeader.dwFlags &= ~DMUS_TRACKCONFIG_PLAY_ENABLED;
                    pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, varTrackHeader );
			    }

                // Freeze undo, update the other strips that changed, and unfreeze undo

	            // If there are no previous edits, set TP_FREEZE_UNDO so an
	            // undo state is not added to the segment
	            BOOL fOrigFreeze = FALSE;
	            if( m_nLastEdit == 0 )
	            {
		            VARIANT var;
		            m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var );
		            fOrigFreeze = V_BOOL(&var);

		            if( !fOrigFreeze )
		            {
			            var.vt = VT_BOOL;
			            V_BOOL(&var) = TRUE;
			            m_pMelGenMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		            }
	            }

	            // Let the object know about the changes
                IUnknown* pUnk = NULL;
                if (SUCCEEDED(pIStripMgr->QueryInterface(IID_IUnknown, (void**)&pUnk)))
                {
	                m_pMelGenMgr->m_pTimeline->OnDataChanged(pUnk);
                    pUnk->Release();

                    // At this point, it would also be nice if the Style strip could redraw
                    // its property page, since we just modified it.  
					IDMUSProdPropSheet* pIPropSheet;
					if( SUCCEEDED ( m_pMelGenMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet ) ) )
					{
						pIPropSheet->RefreshActivePage();
						pIPropSheet->Release();
					}
                }

	            // If the original freeze state was FALSE, and there are no previous edits,
                // reset TP_FREEZE_UNDO to FALSE.
	            if( !fOrigFreeze
	            &&	(m_nLastEdit == 0) )
	            {
		            VARIANT var;
		            var.vt = VT_BOOL;
		            V_BOOL(&var) = FALSE;
		            m_pMelGenMgr->m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
	            }

                pStyleTrack->Release();
                pStyleTrack = NULL;
                pIStripMgr->Release();
                pIStripMgr = NULL;
	            if( FAILED( hr ) )
	            {
		            TRACE("Segment: Failed to set StripMgr's IDMTrack pointer\n");
		            hr = E_FAIL;
		            goto ON_ERROR;
	            }
                dwIndex++;
            }
        }
    }

ON_ERROR:
    if (pISegmentEdit)
    {
        pISegmentEdit->Release();
    }
	if( pIStripMgr )
	{
		pIStripMgr->Release();
	}
	if( pIDMComposedSegment )
	{
		pIDMComposedSegment->Release();
	}
	if( pIDMTrack )
	{
		pIDMTrack->Release();
	}
	if( pIDMComposedTrack )
	{
		pIDMComposedTrack->Release();
	}
	if( pIPersistStream )
	{
		pIPersistStream->Release();
	}
	if( pIStream )
	{
		pIStream->Release();
	}
	if( pIDMStyle )
	{
		pIDMStyle->Release();
	}
	if (pSegment)
	{
		pSegment->Release();
	}
	pDMStyleComposer->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip ITimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Cut

HRESULT CMelGenStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
// CMelGenStrip::Copy

HRESULT CMelGenStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamCopy;

	hr = CanCopy();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfMelGenList == 0 )
	{
		m_cfMelGenList = RegisterClipboardFormat( CF_MELGENLIST );
		if( m_cfMelGenList == 0 )
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected commands in.
	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Save the melody fragments into the stream.
	m_pMelGenMgr->MarkSelectedMelGens(UD_DRAGSELECT);
	CMelGenItem* pMelGenAtDragPoint = m_pMelGenMgr->FirstSelectedMelGen();
	hr = m_pMelGenMgr->SaveSelectedMelGens( pStreamCopy, pMelGenAtDragPoint, TRUE );
	if( FAILED( hr ))
	{
		RELEASE( pStreamCopy );
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfMelGenList, pStreamCopy );
		RELEASE( pStreamCopy );
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			return E_FAIL;
		}
	}
	// Otherwise, add it to the clipboard
	else
	{
		// There is no existing data object, so just create a new one
		hr = m_pMelGenMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pMelGenMgr->GetBoundariesOfSelectedMelGens( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfMelGenList, pStreamCopy );

		// Release the IStream we copied into
		RELEASE( pStreamCopy );

		// Exit with an error if we failed to add the stream
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}

		// get the IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export( &pIDataObject );

		// Release the ITimelineDataObject
		RELEASE( pITimelineDataObject );

		// Exit if the export failed
		if( FAILED(hr) )
		{
			return E_UNEXPECTED;
		}

		// Send the IDataObject to the clipboard
		hr = OleSetClipboard( pIDataObject );

		// Exit if we failed to set the clipboard with our data
		if( hr != S_OK )
		{
			// Release the IDataObject
			RELEASE( pIDataObject );
			return E_FAIL;
		}

		// If we already have a CopyDataObject, release it
		RELEASE( m_pMelGenMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pMelGenMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the ITimelineDataObject
		// m_pMelGenMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::PasteAt

HRESULT CMelGenStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtPasteTime, BOOL fDropNotEditPaste)
{
	ASSERT( m_pMelGenMgr != NULL );
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );

	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Determine paste measure/beat
	long lMeasure;
	long lBeat;
	m_pMelGenMgr->m_pTimeline->ClocksToMeasureBeat( m_pMelGenMgr->m_dwGroupBits,
				 									  0,
													  mtPasteTime,
													  &lMeasure,
													  &lBeat );
	m_pMelGenMgr->m_pTimeline->MeasureBeatToClocks( m_pMelGenMgr->m_dwGroupBits,
				 									  0,
													  lMeasure,
													  lBeat,
													  &mtPasteTime );

	// Don't bother to do anything if user simply moved cursor within same measure in same strip
	if( fDropNotEditPaste )
	{
		if( m_nStripIsDragDropSource )
		{
			long lSourceMeasure;
			long lSourceBeat;
			if( SUCCEEDED ( m_pMelGenMgr->m_pTimeline->PositionToMeasureBeat( m_pMelGenMgr->m_dwGroupBits,
																				0,
																				m_lStartDragPosition,
																				&lSourceMeasure,
																				&lSourceBeat ) ) )
			{
				if( lSourceMeasure == lMeasure && lSourceBeat == lBeat )
				{
					return S_FALSE;
				}
			}
		}
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfMelGenList ) == S_OK )
	{
		IStream* pIStream;
		
		if( m_nStripIsDragDropSource )
		{
			m_pMelGenMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pMelGenMgr->UnselectAll();
		}

		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfMelGenList, &pIStream)))
		{
			STATSTG StatStg;
			ZeroMemory( &StatStg, sizeof(STATSTG) );
			pIStream->Stat( &StatStg, STATFLAG_NONAME );
			hr = m_pMelGenMgr->LoadMelGenChunk( pIStream, StatStg.cbSize.LowPart, TRUE, mtPasteTime );
		}
	}

	// Do we need to enforce unique fragment IDs?
	bool fEnforceUniqueIDs = true;
	if( fDropNotEditPaste )
	{
		ASSERT( (m_dwOverDragEffect == DROPEFFECT_COPY) || (m_dwOverDragEffect == DROPEFFECT_MOVE) );
		if( m_nStripIsDragDropSource 
		&&  m_dwOverDragEffect == DROPEFFECT_MOVE )
		{
			// Moving items within the same strip
			fEnforceUniqueIDs = false;
		}
	}

	// Enforce unique IDs
	if( fEnforceUniqueIDs )
	{
		POSITION pos = m_pMelGenMgr->m_lstMelGens.GetHeadPosition();
		while( pos )
		{
			CMelGenItem* pMelGenItem = m_pMelGenMgr->m_lstMelGens.GetNext( pos );

			if( pMelGenItem->m_dwBits & UD_FROMPASTE )
			{
				if( m_pMelGenMgr->IsUniqueFragmentID(pMelGenItem) == false )
				{
					// Make the Fragment ID unique
					DWORD dwOrigID = pMelGenItem->m_MelGen.dwID;
					pMelGenItem->m_MelGen.dwID = m_pMelGenMgr->NewFragmentID();

					// Sync dwRepeatFragmentID of other pasted items
					POSITION posRepeat = m_pMelGenMgr->m_lstMelGens.GetHeadPosition();
					while( posRepeat )
					{
						CMelGenItem* pMG = m_pMelGenMgr->m_lstMelGens.GetNext( posRepeat );

						if( pMG->m_dwBits & UD_FROMPASTE )
						{
							if( pMG->m_MelGen.dwRepeatFragmentID == dwOrigID )
							{
								pMG->m_MelGen.dwRepeatFragmentID = pMelGenItem->m_MelGen.dwID;
							}
						}
					}
				}
			}
		}
	}

	// Turn off UD_FROMPASTE bit set in m_pMelGenMgr->LoadMelGenChunk()
	m_pMelGenMgr->UnMarkMelGens( UD_FROMPASTE );

	if( SUCCEEDED ( hr ) )
	{
		if( m_nStripIsDragDropSource )
		{
			// Drag/drop Target and Source are the same MelGen strip
			m_nStripIsDragDropSource = 2;
		}
		else
		{
			// Resolved repeat fragment IDs
			m_pMelGenMgr->HookUpRepeats();

			m_nLastEdit = IDS_PASTE;
			m_pMelGenMgr->OnDataChanged();
			m_pMelGenMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

			// Update the property page
			if( m_pMelGenMgr->m_pPropPageMgr != NULL )
			{
				m_pMelGenMgr->m_pPropPageMgr->RefreshData();
			}

			m_pMelGenMgr->SyncWithDirectMusic();

			// Notify the other strips of possible MelGen change
			m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );
		}
	}

	RELEASE( pIRiffStream );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Paste

HRESULT CMelGenStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
//	ITimelineCtl*		pTimeline;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	/*if( FAILED( m_pMelGenMgr->GetClientTimeline( &pTimeline )))
	if( !m_pMelGenMgr->m_pTimeline )))
	{
		return E_FAIL;
	}*/
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );
	if( m_pMelGenMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfMelGenList == 0 )
	{
		m_cfMelGenList = RegisterClipboardFormat( CF_MELGENLIST );
		if( m_cfMelGenList == 0 )
		{
			//RELEASE( pTimeline );
			return E_FAIL;
		}
	}
	if( m_cfStyle == 0 )
	{
		m_cfStyle = RegisterClipboardFormat( CF_STYLE );
		if( m_cfStyle == 0 )
		{
			//RELEASE( pTimeline );
			return E_FAIL;
		}
	}
	
	if(pITimelineDataObject == NULL)
	{
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		hr = OleGetClipboard(&pIDataObject);
		if(FAILED(hr) || (pIDataObject == NULL))
		{
			//RELEASE( pTimeline );
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		hr = m_pMelGenMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			RELEASE( pIDataObject );
			//RELEASE( pTimeline );
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		RELEASE( pIDataObject );
		if( FAILED(hr) )
		{
			RELEASE( pITimelineDataObject );
			//RELEASE( pTimeline );
			return E_FAIL;
		}
	}
	else
	{
		pITimelineDataObject->AddRef();
	}

	// Get the time to paste at
	MUSIC_TIME mtTime;
	if( m_bContextMenuPaste )
	{
		m_pMelGenMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pMelGenMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			RELEASE( pITimelineDataObject );
			//RELEASE( pTimeline );
			return E_FAIL;
		}
	}

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pMelGenMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		RELEASE( pITimelineDataObject );
		//RELEASE( pTimeline );
		return E_FAIL;
	}

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		long lStart, lEnd, lDiff;
		if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
		{
			ASSERT( lStart < lEnd );

			lDiff = lEnd - lStart;
			lStart = mtTime;
			lEnd = lStart + lDiff;

			m_pMelGenMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}

	// Now, do the paste operation
	hr = PasteAt(pITimelineDataObject, mtTime, false);
	RELEASE( pITimelineDataObject );

	// If S_OK, redraw our strip
	if( hr == S_OK )
	{
		m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pMelGenMgr->SyncWithDirectMusic();
	}

	//RELEASE( pTimeline );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Insert

HRESULT CMelGenStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Unselect all items in this strip 
	m_pMelGenMgr->UnselectAll();
	
	// Unselect items in other strips
	UnselectGutterRange();

	HRESULT hr = S_OK;
	long	lMeasure, lBeat;
	CMelGenItem* pMelGen = NULL;
	BOOL fNewMelGen = FALSE;
//	ITimelineCtl* pTimeline;
	//if( FAILED( m_pMelGenMgr->GetClientTimeline( &pTimeline )))
	if( !m_pMelGenMgr->m_pTimeline )
	{
		return E_FAIL;
	}
	hr = m_pMelGenMgr->m_pTimeline->PositionToMeasureBeat( m_pMelGenMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED( hr ) );

	pMelGen = GetMelGenFromPoint( m_lXPos );
	if( pMelGen == NULL )
	{
		pMelGen = new CMelGenItem( m_pMelGenMgr );
		fNewMelGen = TRUE;
	}
	if( pMelGen == NULL )
	{
		//RELEASE( pTimeline );
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pMelGen->m_dwMeasure = lMeasure;
		pMelGen->m_bBeat = (BYTE)lBeat;
		pMelGen->SetSelectFlag( TRUE );
		if( pMelGen->m_dwBits & UD_FAKE )
		{
			pMelGen->m_dwBits &= ~UD_FAKE;
			pMelGen->m_MelGen.dwID = m_pMelGenMgr->NewFragmentID();
		}
		if (fNewMelGen)
		{
			pMelGen->m_MelGen = m_pMelGenMgr->m_DefaultMelGen;
			pMelGen->m_MelGen.dwID = m_pMelGenMgr->NewFragmentID();
		}
		long lClocks;
		hr = m_pMelGenMgr->m_pTimeline->MeasureBeatToClocks(  m_pMelGenMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
		ASSERT( SUCCEEDED( hr ) );
		pMelGen->m_MelGen.mtTime = lClocks;
		if( fNewMelGen )
		{
			m_pMelGenMgr->InsertByAscendingTime(pMelGen);
		}

		// Redraw our strip
		m_pMelGenMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Notify the other strips of possible MelGen change
		m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );

		// Display the property sheet
		DisplayPropertySheet(m_pMelGenMgr->m_pTimeline);
		m_pMelGenMgr->OnShowProperties();

		// If the property page manager exists, refresh it (it should exist)
		if( m_pMelGenMgr->m_pPropPageMgr )
		{
			m_pMelGenMgr->m_pPropPageMgr->RefreshData();
		}

		// Update the Segment Designer
		m_nLastEdit = IDS_INSERT;
		m_pMelGenMgr->OnDataChanged();

		// Sync with DirectMusic
		m_pMelGenMgr->SyncWithDirectMusic();
	}

	//RELEASE( pTimeline );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Delete

HRESULT CMelGenStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	ITimelineCtl*	pTimeline;

	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	/*if( !m_pMelGenMgr->m_pTimeline )
	{
		return E_FAIL;
	}*/
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );
	if( m_pMelGenMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pMelGenMgr->DeleteSelectedMelGens();

	m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pMelGenMgr->m_pPropPageMgr )
	{
		m_pMelGenMgr->m_pPropPageMgr->RefreshData();
	}
	m_pMelGenMgr->SyncWithDirectMusic();

	m_nLastEdit = IDS_DELETE;
	m_pMelGenMgr->OnDataChanged();

	// Notify the other strips of possible MelGen change
	m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );

	//RELEASE( pTimeline );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::SelectAll

HRESULT CMelGenStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	ITimelineCtl* pTimeline;

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	/*if( FAILED( m_pMelGenMgr->GetClientTimeline( &pTimeline )))
	{
		return E_FAIL;
	}*/
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );
	if( m_pMelGenMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pMelGenMgr->SelectAll();

	m_pMelGenMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	//RELEASE( pTimeline );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanCut

HRESULT CMelGenStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
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
// CMelGenStrip::CanCopy

HRESULT CMelGenStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	return m_pMelGenMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanPaste

HRESULT CMelGenStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;

	ASSERT( m_pMelGenMgr != NULL );
	if( m_pMelGenMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfMelGenList == 0 )
	{
		m_cfMelGenList = RegisterClipboardFormat( CF_MELGENLIST );
		if( m_cfMelGenList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfStyle== 0 )
	{
		m_cfStyle = RegisterClipboardFormat( CF_STYLE );
		if( m_cfStyle == 0 )
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		if( (pITimelineDataObject->IsClipFormatAvailable( m_cfMelGenList ) == S_OK)
		/*||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK)*/ )
		{
			hr = S_OK;
		}
		else
		{
			hr = S_FALSE;
		}
	}
	// Otherwise, check the clipboard
	else
	{
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		if( SUCCEEDED( OleGetClipboard(&pIDataObject) ) )
		{
			// Create a new TimelineDataObject
			IDMUSProdTimelineDataObject *pITimelineDataObject;
			if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					if( (pITimelineDataObject->IsClipFormatAvailable( m_cfMelGenList ) == S_OK)
					/*||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK)*/ )
					{
						hr = S_OK;
					}
					else
					{
						hr = S_FALSE;
					}
				}
				RELEASE( pITimelineDataObject );
			}
			RELEASE( pIDataObject );
		}
	}

	if (hr == S_OK)
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanInsert

HRESULT CMelGenStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pMelGenMgr != NULL );
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Get clock at XPos
	long lClock;
	m_pMelGenMgr->m_pTimeline->PositionToClocks( m_lXPos, &lClock );

	// Make sure XPos is within strip
	if( lClock > 0 
	&&  lClock < lTimelineLength )
	{
		// User clicked within boundaries of strip
		long lMeasure;
		long lBeat;

		if( SUCCEEDED ( m_pMelGenMgr->m_pTimeline->PositionToMeasureBeat( m_pMelGenMgr->m_dwGroupBits,
																			0,
																			m_lXPos,
																			&lMeasure,
																			&lBeat ) ) )
		{
			CPropMelGen* pMelGen = GetMelGenFromPoint( m_lXPos );
			if( pMelGen == NULL
			||  (pMelGen->m_dwBits & UD_FAKE) )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanDelete

HRESULT CMelGenStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pMelGenMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanSelectAll

HRESULT CMelGenStrip::CanSelectAll( void )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_OK;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::QueryContinueDrag

HRESULT CMelGenStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton & MK_LBUTTON )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}

		if( !(grfKeyState & MK_LBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	if( m_dwStartDragButton & MK_RBUTTON )
	{
		if( grfKeyState & MK_LBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}
		
		if( !(grfKeyState & MK_RBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GiveFeedback

HRESULT CMelGenStrip::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CreateDataObject

HRESULT	CMelGenStrip::CreateDataObject(IDataObject** ppIDataObject, long position)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Selected MelGens into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pMelGenMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		CMelGenItem* pMelGenAtDragPoint = GetMelGenFromPoint( position );

		// mark the melody fragments as being dragged: this used later for deleting time signatures in drag move
		m_pMelGenMgr->MarkSelectedMelGens(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pMelGenMgr->SaveSelectedMelGens( pIStream, pMelGenAtDragPoint, TRUE ) ) )
		{
			// Place CF_MELGENLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfMelGenList, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}


	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;

}


// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::DragEnter

HRESULT CMelGenStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
// CMelGenStrip::DragOver

HRESULT CMelGenStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	MUSIC_TIME mtTime;
	if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
	{
		if( (mtTime >= 0) && (CanPasteFromData( m_pITargetDataObject ) == S_OK) )
		{
			BOOL fCF_STYLE = FALSE;

			// Does m_pITargetDataObject contain format CF_STYLE?
			CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
			if( pDataObject )
			{
				/*
				if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfStyle ) ) )
				{
					fCF_STYLE = TRUE;
				}
				*/
				pDataObject->Release();
			}

			// Can only copy CF_STYLE data!
			if( fCF_STYLE )
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
// CMelGenStrip::DragLeave

HRESULT CMelGenStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		RELEASE( m_pITargetDataObject );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::Drop

HRESULT CMelGenStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
			CWnd* pWnd = GetTimelineCWnd();
			if( pWnd )
			{
				// Display and track menu
				CPoint point( pt.x, pt.y );
				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, pWnd->GetSafeHwnd(), NULL );
				DestroyMenu( hMenu );

				// Need to process WM_COMMAND from TrackPopupMenu
				MSG msg;
				while( ::PeekMessage( &msg, pWnd->GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
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
				m_dwDragRMenuEffect = DROPEFFECT_NONE;
			}
		}
	}

	// Paste data
	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED( m_pMelGenMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
				{
					long lMeasure = 0, lBeat = 0;
					if (SUCCEEDED( hr = m_pMelGenMgr->m_pTimeline->ClocksToMeasureBeat( m_pMelGenMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat ) )  &&
						SUCCEEDED( hr = m_pMelGenMgr->m_pTimeline->MeasureBeatToClocks( m_pMelGenMgr->m_dwGroupBits, 0, lMeasure, lBeat, &mtTime ) ) ) 
					{
						hr = PasteAt( pITimelineDataObject, mtTime, true );
						if( hr == S_OK )
						{
							*pdwEffect = m_dwOverDragEffect;
						}
						else if( hr == S_FALSE )
						{
							*pdwEffect = DROPEFFECT_NONE;
						}
					}
				}
			}
			RELEASE( pITimelineDataObject );
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}


// IDropTarget helpers

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetTimelineCWnd

CWnd* CMelGenStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
//	ITimelineCtl* pTimeline;
//	if( SUCCEEDED(m_pMelGenMgr->GetClientTimeline(&pTimeline)))
	if( m_pMelGenMgr->m_pTimeline)
	{
		if( SUCCEEDED(m_pMelGenMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
		{
			if( cDC.Attach( (HDC)(vt.lVal) ) != 0 )
			{
				pWnd = cDC.GetWindow();
				cDC.Detach();
			}
			if( pWnd )
			{
				::ReleaseDC( pWnd->GetSafeHwnd(), (HDC)(vt.lVal) );
			}
			else
			{
				::ReleaseDC( NULL, (HDC)(vt.lVal) );
			}
		}
		//RELEASE( pTimeline );
	}
	return pWnd;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::CanPasteFromData

HRESULT CMelGenStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfMelGenList ) )
	/*||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfStyle ) )*/ )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::GetData

HRESULT CMelGenStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( ppData == NULL
	||	*ppData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case 0:
	{
		ioGroupBitsPPG *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPG *>(*ppData);
		pGroupBitsPPGData->dwGroupBits = m_pMelGenMgr->m_dwGroupBits;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pMelGenMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pMelGenMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	case 2:
	{
		ioMelGenFlagsPPG *pMelGenFlagsPPG = reinterpret_cast<ioMelGenFlagsPPG *>(*ppData);
		pMelGenFlagsPPG->dwPlayMode = m_pMelGenMgr->m_dwPlaymode;
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::SetData

HRESULT CMelGenStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case 0:
	{
		ioGroupBitsPPG *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPG *>(pData);

		if( pGroupBitsPPGData->dwGroupBits != m_pMelGenMgr->m_dwGroupBits )
		{
			DWORD dwOrigGroupBits = m_pMelGenMgr->m_dwGroupBits; 
			m_pMelGenMgr->m_dwGroupBits = pGroupBitsPPGData->dwGroupBits;

			m_nLastEdit = IDS_TRACK_GROUP;
			m_pMelGenMgr->m_pTimeline->OnDataChanged( (IMelGenMgr*)m_pMelGenMgr );
		}
		return S_OK;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pMelGenMgr->m_dwTrackExtrasFlags )
		{
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pMelGenMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			m_pMelGenMgr->m_pTimeline->OnDataChanged( (IMelGenMgr*)m_pMelGenMgr );
		}
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pMelGenMgr->m_dwProducerOnlyFlags )
		{
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pMelGenMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
			m_pMelGenMgr->m_pTimeline->OnDataChanged( (IMelGenMgr*)m_pMelGenMgr );
		}
		return S_OK;
		break;
	}
	case 2:
	{
		// Not supported in DX8
		ASSERT( 0 );	
//		ioMelGenFlagsPPG *pMelGenFlagsPPG = reinterpret_cast<ioMelGenFlagsPPG *>(pData);
//
//		if( pMelGenFlagsPPG->dwPlayMode != m_pMelGenMgr->m_dwPlaymode )
//		{
//			m_pMelGenMgr->m_dwPlaymode = pMelGenFlagsPPG->dwPlayMode;
//
//			m_pMelGenMgr->SyncWithDirectMusic();
//
//			m_nLastEdit = IDS_UNDO_PLAYMODE;
//			m_pMelGenMgr->m_pTimeline->OnDataChanged( (IMelGenMgr*)m_pMelGenMgr );
//
//		}
		return S_OK;
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnShowProperties

HRESULT CMelGenStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( !m_pMelGenMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	if (pIFramework == NULL)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}


	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	RELEASE( pIFramework );
	if( pIPropSheet == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//  If the property sheet is hidden, exit
	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return S_OK;
	}

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pPropPageMgr != NULL );

		// release our reference to the property sheet
		RELEASE( pIPropSheet );
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr;
		if( NULL == pPPM )
		{
			hr = E_OUTOFMEMORY;
			goto EXIT;
		}
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
		{
			goto EXIT;
		}
	}

	// Set the property page to refer to the Piano Roll property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pMelGenMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	RELEASE( pIPropSheet );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnRemoveFromPageManager

HRESULT CMelGenStrip::OnRemoveFromPageManager( void )
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
// CMelGenStrip::DisplayPropertySheet

HRESULT CMelGenStrip::DisplayPropertySheet(IDMUSProdTimeline* pTimeline)
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
				RELEASE( pIPropSheet );
			}
			RELEASE( punk );
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::DoDragDrop

BOOL CMelGenStrip::DoDragDrop( void )
{
	// Drag drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pMelGenMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Query ourself for our IDropSource interface
	IDropSource* pIDropSource;
	if( FAILED ( QueryInterface(IID_IDropSource, (void**)&pIDropSource) ) )
	{
		return FALSE;
	}

	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a data object from the selected melody fragments
	HRESULT hr = CreateDataObject( &m_pISourceDataObject, m_lStartDragPosition );
	if( SUCCEEDED( hr ) )
	{
		// We can always copy melody fragments
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
		if( CanCut() == S_OK )
		{
			// If we can Cut(), allow the user to move the melody fragments as well.
			dwOKDragEffects |= DROPEFFECT_MOVE;
		}

		// Do the Drag/Drop.
		hr = ::DoDragDrop( m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect );

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					m_pMelGenMgr->DeleteMarked( UD_DRAGSELECT );
				}
				break;

			default:
				dwEffect = DROPEFFECT_NONE;
				break;
		}
		hr = S_OK;
		RELEASE( m_pISourceDataObject );
	}

	m_pMelGenMgr->HookUpRepeats();

	RELEASE( pIDropSource );

	m_pMelGenMgr->UnMarkMelGens(UD_DRAGSELECT);
	m_pMelGenMgr->SyncWithDirectMusic();

	if( dwEffect != DROPEFFECT_NONE )
	{
		if( m_nStripIsDragDropSource == 2 )
		{
			// Drag/drop target and source are the same MelGen strip
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_UNDO_MOVE;
				m_pMelGenMgr->OnDataChanged();
				
				// Notify the other strips of possible MelGen change
				m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_nLastEdit = IDS_PASTE;
				m_pMelGenMgr->OnDataChanged();
				
				// Notify the other strips of possible MelGen change
				m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );
			}
		}
		else
		{
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_DELETE;
				m_pMelGenMgr->OnDataChanged();
				
				// Notify the other strips of possible MelGen change
				m_pMelGenMgr->m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_pMelGenMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_pMelGenMgr->UnselectAll();
			}
		}
	}

	if( dwEffect == DROPEFFECT_NONE )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnLButtonDown

HRESULT CMelGenStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	ASSERT( m_pMelGenMgr != NULL );
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );

	// If we're already dragging, just return
	// TODO: Implement

	// Get Timeline length
	VARIANT var;
	m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pMelGenMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
	if( lClock >= lTimelineLength )
	{
		return S_OK;
	}
	
	// Unselect items in other strips
	UnselectGutterRange();

	HRESULT hr = S_OK;

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pMelGenMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// See if there is a melody fragment under the cursor.
	CMelGenItem* pMelGen = GetMelGenFromPoint( lXPos );
	if( pMelGen
	&&  !(pMelGen->m_dwBits & UD_FAKE) )
	{
		// There is a MelGen on this measure
		if( wParam & MK_CONTROL )
		{
			if( pMelGen->m_fSelected )
			{
				// Do drag/drop
				m_dwStartDragButton = (unsigned long)wParam;
				m_lStartDragPosition = lXPos;
				m_nStripIsDragDropSource = 1;
		
				BOOL fDrop = DoDragDrop();

				m_dwStartDragButton = 0;
				m_nStripIsDragDropSource = 0;

				if( !fDrop )
				{
					pMelGen->SetSelectFlag( !pMelGen->m_fSelected );
				}
			}
			else
			{
				pMelGen->SetSelectFlag( !pMelGen->m_fSelected );
				if( pMelGen->m_fSelected )
				{
					// Set fields to initiate drag/drop on next mousemove
					m_dwStartDragButton = (unsigned long)wParam;
					m_lStartDragPosition = lXPos;
				}
			}
		}
		else if( wParam & MK_SHIFT )
		{
			pMelGen->SetSelectFlag( TRUE );
			SelectRange( pMelGen );
		}
		else
		{
			if( pMelGen->m_fSelected == FALSE )
			{
				m_pMelGenMgr->UnselectAll();
				pMelGen->SetSelectFlag( TRUE );
				m_pMelGenMgr->UnMarkMelGens( UD_CURRENTSELECTION );
				m_pMelGenMgr->MarkSelectedMelGens( UD_CURRENTSELECTION );
			}
			
			// Set fields to initiate drag/drop on next mousemove
			m_dwStartDragButton = (unsigned long)wParam;
			m_lStartDragPosition = lXPos;
		}
	}
	else
	{
		// There is not a "real" MelGen on this measure
		hr = S_OK;
		if( pMelGen == NULL )
		{
			// Create a "fake" MelGen
			hr = m_pMelGenMgr->CreateMelGen( lXPos, pMelGen );
		}

		if( SUCCEEDED ( hr ) )
		{
			if( wParam & MK_CONTROL )
			{
				pMelGen->SetSelectFlag( !pMelGen->m_fSelected );
			}
			else if( wParam & MK_SHIFT )
			{
				pMelGen->SetSelectFlag( TRUE );
				SelectRange( pMelGen );
			}
			else
			{
				// Click on empty space deselects all
				m_pMelGenMgr->UnselectAll();

				pMelGen->SetSelectFlag( TRUE );
				m_pMelGenMgr->UnMarkMelGens( UD_CURRENTSELECTION );
				m_pMelGenMgr->MarkSelectedMelGens( UD_CURRENTSELECTION );
			}
		}
	}

	// Redraw the strip and refresh the MelGen property page
	m_pMelGenMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	m_pMelGenMgr->OnShowProperties();
	if( m_pMelGenMgr->m_pPropPageMgr )
	{
		m_pMelGenMgr->m_pPropPageMgr->RefreshData();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::OnRButtonDown

HRESULT CMelGenStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	ASSERT( m_pMelGenMgr != NULL );
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pMelGenMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pMelGenMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
	if( lClock >= lTimelineLength )
	{
		return S_OK;
	}

	UnselectGutterRange();

	// Get the item at the mouse click.
	CMelGenItem* pMelGen = GetMelGenFromPoint( lXPos );
	
	if( pMelGen == NULL )
	{
		// No MelGen so create a "fake" one
		m_pMelGenMgr->UnselectAll();
		if( SUCCEEDED ( m_pMelGenMgr->CreateMelGen( lXPos, pMelGen ) ) )
		{
			pMelGen->SetSelectFlag( TRUE );
		}
	}
	else if( !(wParam & MK_CONTROL)
		 &&  !(wParam & MK_SHIFT) )
	{
		if( pMelGen )
		{
			if( pMelGen->m_fSelected == FALSE )
			{
				m_pMelGenMgr->UnselectAll();
				pMelGen->SetSelectFlag( TRUE );
			}
		}
	}

	m_pMelGenMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	m_fShowMelGenProps = TRUE;
	m_pMelGenMgr->OnShowProperties(); 
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::UnselectGutterRange

void CMelGenStrip::UnselectGutterRange( void )
{
	ASSERT( m_pMelGenMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pMelGenMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pMelGenMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenStrip::SelectRange

HRESULT CMelGenStrip::SelectRange( CMelGenItem* pMelGen )
{
	HRESULT hr = S_OK;

	// Select inclusive, find MelGen to start from
	CMelGenItem* pCurrent = m_pMelGenMgr->CurrentlySelectedMelGen();

	// Find measure/beat for MelGen to start from and MelGen to end at
	DWORD dwEarlyMeasure = (pCurrent) ? pCurrent->m_dwMeasure : 0;
	BYTE bEarlyBeat = (pCurrent) ? pCurrent->m_bBeat : 0;
	DWORD dwLateMeasure = pMelGen->m_dwMeasure;
	BYTE bLateBeat = pMelGen->m_bBeat;

	// If the order is backwards, swap
	if( (dwLateMeasure < dwEarlyMeasure)
	||  (dwLateMeasure == dwEarlyMeasure  &&  bLateBeat < bEarlyBeat) )
	{
		DWORD dwTemp = dwEarlyMeasure;
		dwEarlyMeasure = dwLateMeasure;
		dwLateMeasure = dwTemp;

		BYTE bTemp = bEarlyBeat;
		bEarlyBeat = bLateBeat;
		bLateBeat = bTemp;
	}

	// Add empty empty MelGens to fill in gaps
	CMelGenItem* pMelGenItem = NULL;
	DWORD dwCurrentMeasure = dwEarlyMeasure;
	BYTE bCurrentBeat = bEarlyBeat;
	long lClocks;
	long lBeat;
	while( dwCurrentMeasure <= dwLateMeasure )
	{
		if( GetMelGenFromMeasureBeat(dwCurrentMeasure, bCurrentBeat) == NULL )
		{
			long lPosition = 0;
			m_pMelGenMgr->m_pTimeline->MeasureBeatToPosition( 
				m_pMelGenMgr->m_dwGroupBits, 0, dwCurrentMeasure, bCurrentBeat, &lPosition );
			pMelGenItem = NULL;
			hr = m_pMelGenMgr->CreateMelGen( lPosition, pMelGenItem );
		}

		m_pMelGenMgr->m_pTimeline->MeasureBeatToClocks(
			m_pMelGenMgr->m_dwGroupBits, 0, dwCurrentMeasure, ++bCurrentBeat, &lClocks ); 
		m_pMelGenMgr->m_pTimeline->ClocksToMeasureBeat(
			m_pMelGenMgr->m_dwGroupBits, 0, lClocks, (long *)&dwCurrentMeasure, &lBeat ); 
		bCurrentBeat = (BYTE)lBeat;

		if( dwCurrentMeasure > dwLateMeasure 
		|| (dwCurrentMeasure == dwLateMeasure  &&  bCurrentBeat > bLateBeat) )
		{
			break;
		}
	}

	// Unselect all MelGens in the strip
	m_pMelGenMgr->UnselectAll();

	// Select all MelGens in the range (can't simply select MelGens in the
	// above loop, because there may be multiple MelGens at the same measure)
	POSITION pos = m_pMelGenMgr->m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pMelGenItem = m_pMelGenMgr->m_lstMelGens.GetNext( pos );

		if( pMelGenItem->m_dwMeasure > dwEarlyMeasure
		&&	pMelGenItem->m_dwMeasure < dwLateMeasure )
		{
			pMelGenItem->SetSelectFlag( TRUE );
		}

		else if( pMelGenItem->m_dwMeasure == dwEarlyMeasure 
			 &&  pMelGenItem->m_dwMeasure == dwLateMeasure )
		{
			if( pMelGenItem->m_bBeat >= bEarlyBeat 
			&&  pMelGenItem->m_bBeat <= bLateBeat )
			{
				pMelGenItem->SetSelectFlag( TRUE );
			}
		}

		else if( pMelGenItem->m_dwMeasure == dwEarlyMeasure )
		{
			if( pMelGenItem->m_bBeat >= bEarlyBeat )
			{
				pMelGenItem->SetSelectFlag( TRUE );
			}
		}
		
		else if( pMelGenItem->m_dwMeasure == dwLateMeasure )
		{
			if( pMelGenItem->m_bBeat <= bLateBeat )
			{
				pMelGenItem->SetSelectFlag( TRUE );
			}
		}
	}

	// Reset the current selection flag (it gets turned off in SetSelectFlag)
	if( pCurrent )
	{
		pCurrent->m_dwBits |= UD_CURRENTSELECTION;
	}

	return hr;
}
