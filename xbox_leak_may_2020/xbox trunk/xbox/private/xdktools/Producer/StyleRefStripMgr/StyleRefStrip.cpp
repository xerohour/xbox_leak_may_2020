// StyleRefStrip.cpp : Implementation of CStyleRefStrip
#include "stdafx.h"
#include "StyleRefIO.h"
#include "StyleRefStripMgr.h"
#include "StyleRefMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include <RiffStrm.h>
#include <StyleDesigner.h>
#include "MusicTimeConverter.h"
#include "PropPageVarSeed.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"
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
// CStyleRefStrip constructor/destructor

CStyleRefStrip::CStyleRefStrip( CStyleRefMgr* pStyleRefMgr )
{
	ASSERT( pStyleRefMgr );
	if ( pStyleRefMgr == NULL )
	{
		return;
	}

	m_pStyleRefMgr = pStyleRefMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pStyleRefMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;
	m_bGutterSelected = FALSE;

	m_cfStyleRefList = 0;
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
	m_fShowStyleRefProps = FALSE;
	m_fPropPageActive = FALSE;
	m_fSingleSelect = FALSE;
	m_pPropPageMgr = NULL;
	m_fInRightClickMenu = FALSE;
}

CStyleRefStrip::~CStyleRefStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pStyleRefMgr = NULL;
	}
	RELEASE( m_pISourceDataObject );
	RELEASE( m_pITargetDataObject );
	RELEASE( m_pPropPageMgr );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::QueryInterface

STDMETHODIMP CStyleRefStrip::QueryInterface( REFIID riid, LPVOID *ppv )
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
// CStyleRefStrip::AddRef

STDMETHODIMP_(ULONG) CStyleRefStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Release

STDMETHODIMP_(ULONG) CStyleRefStrip::Release(void)
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
// CStyleRefStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Draw

HRESULT	STDMETHODCALLTYPE CStyleRefStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	BOOL fUseGutterSelectRange = FALSE;
	if( m_bGutterSelected
	&&  m_lGutterBeginSelect != m_lGutterEndSelect )
	{
		fUseGutterSelectRange = TRUE;
	}

	// locals for Ghosting
	long lFirstVisibleMeasure, lPartialVisibleMeasure;
	long lClocks;
	RECT rectGhost;
	CStyleRefItem* pGhostStyle = 0;

	rectGhost.top = rectGhost.left = rectGhost.right = 0;
	rectGhost.bottom = STRIP_HEIGHT;
	bool bGhostStyleCovered = false;	// indicates a real style exists where ghost would be

	// Validate pointer to the Timeline
	if( m_pStyleRefMgr->m_pTimeline )
	{
		IDMUSProdTimeline* pTimeline = m_pStyleRefMgr->m_pTimeline; // added later for use in ghosting

		// Draw Measure and Beat lines in our strip
		m_pStyleRefMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pStyleRefMgr->m_dwGroupBits, 0, lXOffset );

		// Validate our device context
		if( hDC )
		{
			CDC dc;		// added for use in ghosting
			dc.Attach(hDC);
			RECT	rectClip, rectHighlight;
			long	lStartTime, lPosition;
			CStyleRefItem* pStyleRefItem;
			SIZE	sizeText;

			::GetClipBox( hDC, &rectClip );
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;


			// find first visible measure for ghosting styles
			pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasure(lPartialVisibleMeasure, pTimeline, m_pStyleRefMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasure(lPartialVisibleMeasure, 0, pTimeline, m_pStyleRefMgr->m_dwGroupBits);
			if(cmtNearestMeasure.Time() < cmtFirstVisible.Time())
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure + 1;
			}
			else
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure;
			}

			// find latest style before first visible measure
			POSITION pos = m_pStyleRefMgr->m_lstStyleRefs.GetHeadPosition();
			while(pos)
			{
				pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetNext(pos);
				if(pStyleRefItem->m_dwMeasure < (unsigned)lFirstVisibleMeasure
					&& !pStyleRefItem->m_StyleListInfo.strName.IsEmpty())
				{
					pGhostStyle = pStyleRefItem;
				}
				else
				{
					break;
				}
			}
			if(pGhostStyle)
			{
				// get rect of style, will have to truncate if ghost style covers part of real style
				pTimeline->MeasureBeatToPosition(m_pStyleRefMgr->m_dwGroupBits, 0, lFirstVisibleMeasure,
												0, &lPosition);
				lPosition++;
				CSize csize;
				csize = dc.GetTextExtent(pGhostStyle->m_StyleListInfo.strName);
				rectGhost.left = lPosition - lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
			}

			// well draw ghost style after checking whether real style covers it

			m_pStyleRefMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

			// Iterate through all styles
			pos = m_pStyleRefMgr->m_lstStyleRefs.GetHeadPosition();
			while( pos )
			{
				pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos );

				if(pStyleRefItem->m_dwMeasure == (unsigned)lFirstVisibleMeasure)
				{
					// real signpost covers ghost
					bGhostStyleCovered = true;
				}

				// Compute the position of the style
				m_pStyleRefMgr->m_pTimeline->MeasureBeatToPosition( m_pStyleRefMgr->m_dwGroupBits, 0, pStyleRefItem->m_dwMeasure, 0, &lPosition );

				// If it's not visible, break out of the loop
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				if(pGhostStyle)
				{
					// truncate ghost style's text so it doesn't cover up real style's text
					long lTruePos = lPosition - lXOffset;
					if(lTruePos > rectGhost.left && lTruePos < rectGhost.right)
					{
						rectGhost.right = lTruePos;
					}
				}

				// Otherwise, draw it one pixel to the right (so it doesn't overwrite the measure line)
				::TextOut( hDC, lPosition - lXOffset + 1, 0, pStyleRefItem->m_StyleListInfo.strName, _tcslen(pStyleRefItem->m_StyleListInfo.strName) );
			}

			// make sure the first selected StyleRef is shown in its entirety
			pStyleRefItem = m_pStyleRefMgr->FirstSelectedStyleRef();
			if( pStyleRefItem )
			{
				pos = m_pStyleRefMgr->m_lstStyleRefs.Find( pStyleRefItem, NULL );
				while( pos )
				{
					if( pStyleRefItem->m_fSelected )
					{
						long lMeasureLength = 0;
						m_pStyleRefMgr->m_pTimeline->MeasureBeatToPosition( m_pStyleRefMgr->m_dwGroupBits, 0,
														  pStyleRefItem->m_dwMeasure, 0, &lPosition );
						m_pStyleRefMgr->m_pTimeline->MeasureBeatToPosition( m_pStyleRefMgr->m_dwGroupBits, 0,
														 (pStyleRefItem->m_dwMeasure + 1), 0, &lMeasureLength );
						lMeasureLength -= lPosition;

						// find extent of text
						::GetTextExtentPoint32( hDC, pStyleRefItem->m_StyleListInfo.strName, _tcslen(pStyleRefItem->m_StyleListInfo.strName), &sizeText );
						if( sizeText.cx < lMeasureLength )
						{
							sizeText.cx = lMeasureLength;
						}
						long lExtent = lPosition + sizeText.cx;

						// truncate if a selected StyleRef covers part of text
						POSITION pos2 = pos;
						CStyleRefItem* pTempStyleRef;
						bool fDone = false;
						while( pos2 && !fDone)
						{
							m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos2 );
							if (!pos2) break;
							pTempStyleRef = m_pStyleRefMgr->m_lstStyleRefs.GetAt( pos2 );
							if( pTempStyleRef->m_fSelected )
							{	
								long lRPos;
								m_pStyleRefMgr->m_pTimeline->MeasureBeatToPosition( m_pStyleRefMgr->m_dwGroupBits, 0, pTempStyleRef->m_dwMeasure, 0, &lRPos );
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
							::DrawText( hDC, pStyleRefItem->m_StyleListInfo.strName, _tcslen(pStyleRefItem->m_StyleListInfo.strName), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
							GrayOutRect( hDC, &rectHighlight );
						}
					}
					m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos );
					if (pos) pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetAt( pos );
				}
			}

			// now draw ghost style
			// but not if original style is still showing
			if(!bGhostStyleCovered && pGhostStyle)
			{
				pTimeline->MeasureBeatToPosition(m_pStyleRefMgr->m_dwGroupBits, 0,
													pGhostStyle->m_dwMeasure, 0, &lPosition);
						
				// lPosition -= lXOffset deliberately left off as left margin already has scrolling taking into accout
				CSize extent = dc.GetTextExtent(pGhostStyle->m_StyleListInfo.strName);
				if(!((lPosition + extent.cx) > LeftMargin(pTimeline)))
				{
					COLORREF cr = dc.SetTextColor(RGB(168, 168, 168));
					dc.DrawText(pGhostStyle->m_StyleListInfo.strName, &rectGhost, (DT_LEFT | DT_NOPREFIX));
					dc.SetTextColor(cr);
				}
			}

			// Highlight the selected range if there is one.
			if( fUseGutterSelectRange )
			{
				long lBeginSelect = m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterEndSelect : m_lGutterBeginSelect;
				long lEndSelect = m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterBeginSelect : m_lGutterEndSelect;

				long lMeasure;
				long lBeat;
				long lClocks;

				m_pStyleRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits, 0, lBeginSelect, &lMeasure, &lBeat );
				m_pStyleRefMgr->m_pTimeline->MeasureBeatToClocks( m_pStyleRefMgr->m_dwGroupBits, 0, lMeasure, 0, &lClocks );
				m_pStyleRefMgr->m_pTimeline->ClocksToPosition( lClocks, &(rectHighlight.left));

				m_pStyleRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits, 0, lEndSelect, &lMeasure, &lBeat );
				m_pStyleRefMgr->m_pTimeline->MeasureBeatToClocks( m_pStyleRefMgr->m_dwGroupBits, 0, (lMeasure + 1), 0, &lClocks );
				m_pStyleRefMgr->m_pTimeline->ClocksToPosition( (lClocks - 1), &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;

				// Invert it.
				GrayOutRect( hDC, &rectHighlight );
			}
			dc.Detach();
		}
	}
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CStyleRefStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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
			CString strStyles;

			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( _Module.GetModuleInstance() );
			strStyles.LoadString( IDS_STYLES_TEXT );
			AfxSetResourceHandle( hInstance );

			CString str = GetName(m_pStyleRefMgr->m_dwGroupBits, strStyles);
			BSTR bstr;

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
		if( m_pStyleRefMgr )
		{
			m_pStyleRefMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		}
		else
		{
			V_UNKNOWN(pvar) = NULL;
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CStyleRefStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	switch( sp )
	{
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
			m_pStyleRefMgr->UnselectAll();
			m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pStyleRefMgr->UnselectAll();
			m_pStyleRefMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pStyleRefMgr->UnselectAll();
		}
		m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pStyleRefMgr->m_pPropPageMgr != NULL )
		{
			m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pStyleRefMgr->UnselectAll();
			m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pStyleRefMgr->UnselectAll();
			m_pStyleRefMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pStyleRefMgr->UnselectAll();
		}
		m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pStyleRefMgr->m_pPropPageMgr != NULL )
		{
			m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::GetStyleRefFromMeasureBeat

CStyleRefItem *CStyleRefStrip::GetStyleRefFromMeasureBeat( DWORD dwMeasure, BYTE bBeat )
{
	ASSERT( bBeat == 0 );	// Styles must be on a measure boundary
	
	CStyleRefItem* pStyleRefItem = NULL;

	POSITION pos = m_pStyleRefMgr->m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos );
		ASSERT( pStyleRefItem != NULL );
		if ( pStyleRefItem != NULL )
		{
			if ( pStyleRefItem->m_dwMeasure == dwMeasure )
			{
				return pStyleRefItem;
			}
			else if ( pStyleRefItem->m_dwMeasure > dwMeasure )
			{
				break;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::GetStyleRefFromPoint

CStyleRefItem *CStyleRefStrip::GetStyleRefFromPoint( long lPos )
{
	CStyleRefItem*		pStyleRefReturn = NULL;

	if( m_pStyleRefMgr->m_pTimeline )
	{
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->PositionToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pStyleRefReturn = GetStyleRefFromMeasureBeat( lMeasure, 0 );
		}
	}
	return pStyleRefReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CStyleRefStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		m_fShowStyleRefProps = TRUE;
		hr = OnLButtonDown( wParam, lXPos );
		break;

	case WM_RBUTTONDOWN:
		hr = OnRButtonDown( wParam, lXPos );
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

		// Save the position of the click so we know where to insert a StyleRef. if Insert is selected.
		m_lXPos = lXPos;
		m_fInRightClickMenu = TRUE;
		m_pStyleRefMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
		m_fInRightClickMenu = FALSE;

		hr = S_OK;
		break;

	case WM_MOUSEMOVE:
		if( m_dwStartDragButton )
		{
			m_nStripIsDragDropSource = 1;
	
			if( DoDragDrop() )
			{
				// Redraw the strip and refresh the StyleRef property page
				m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pStyleRefMgr->OnShowProperties();
				if( m_pStyleRefMgr->m_pPropPageMgr )
				{
					m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
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
			hr = DisplayPropertySheet(m_pStyleRefMgr->m_pTimeline);
			if (m_fShowStyleRefProps)
			{
				// Change to the style reference property page
				m_pStyleRefMgr->OnShowProperties();
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
		if( m_fSingleSelect )
		{
			CStyleRefItem* pStyleRef = GetStyleRefFromPoint( lXPos );
			if( pStyleRef
			&&  pStyleRef->m_StyleListInfo.strName.IsEmpty() == FALSE )
			{
				m_pStyleRefMgr->UnselectAll();
				pStyleRef->SetSelectFlag( TRUE );
				m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
				pStyleRef->m_dwBits |= UD_CURRENTSELECTION;

				// Redraw the strip and refresh the StyleRef property page
				m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pStyleRefMgr->OnShowProperties();
				if( m_pStyleRefMgr->m_pPropPageMgr )
				{
					m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
				}
			}
			m_fSingleSelect = FALSE;
		}
		m_lXPos = lXPos;
		hr = S_OK;
		break;

	case WM_CREATE:
		m_cfStyleRefList = RegisterClipboardFormat( CF_STYLEREFLIST );
		m_cfStyle = RegisterClipboardFormat( CF_STYLE);

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::FBDraw

HRESULT CStyleRefStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	UNREFERENCED_PARAMETER(sv);
	UNREFERENCED_PARAMETER(hDC);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::FBOnWMMessage

HRESULT CStyleRefStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fShowStyleRefProps = FALSE;
		OnShowProperties();
		break;

	case WM_RBUTTONUP:
		m_fShowStyleRefProps = FALSE;
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

		if( m_pStyleRefMgr->m_pTimeline )
		{
			m_fInRightClickMenu = TRUE;
			m_pStyleRefMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
			m_fInRightClickMenu = FALSE;
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Cut

HRESULT CStyleRefStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
// CStyleRefStrip::Copy

HRESULT CStyleRefStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfStyleRefList == 0 )
	{
		m_cfStyleRefList = RegisterClipboardFormat( CF_STYLEREFLIST );
		if( m_cfStyleRefList == 0 )
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

	m_pStyleRefMgr->MarkSelectedStyleRefs(UD_DRAGSELECT);

	MUSIC_TIME mtTime;
	if( pITimelineDataObject )
	{
		// Get clock from pITimelineDataObject
		if( FAILED( pITimelineDataObject->GetBoundaries( &mtTime, NULL ) ) )
		{
			return E_UNEXPECTED;
		}
	}
	else
	{
		// Get clock of first selected Style's measure
		CStyleRefItem* pStyleRefAtDragPoint = m_pStyleRefMgr->FirstSelectedStyleRef();
		ASSERT( pStyleRefAtDragPoint != NULL );
		if( pStyleRefAtDragPoint == NULL )
		{
			return E_UNEXPECTED;
		}
		mtTime = m_pStyleRefMgr->MeasureToClocks( pStyleRefAtDragPoint->m_dwMeasure );
	}

	// Save the Styles into the stream.
	hr = m_pStyleRefMgr->SaveSelectedStyleRefs( pStreamCopy, mtTime );
	if( FAILED( hr ))
	{
		RELEASE( pStreamCopy );
		return E_UNEXPECTED;
	}
		
	if( pITimelineDataObject )
	{
		// add the stream to the passed IDMUSProdTimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfStyleRefList, pStreamCopy );
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
		hr = m_pStyleRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pStyleRefMgr->GetBoundariesOfSelectedStyleRefs( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfStyleRefList, pStreamCopy );

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

		// Release the IDMUSProdTimelineDataObject
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
		RELEASE( m_pStyleRefMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pStyleRefMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pStyleRefMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::PasteAt

HRESULT CStyleRefStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtPasteTime, BOOL fDropNotEditPaste, BOOL &fChanged, CStyleRefItem** pSyncBandTempoItem )
{
	ASSERT( m_pStyleRefMgr != NULL );
	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );

	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure everything in other strip is deselected first.
	if( fDropNotEditPaste == FALSE )
	{
		UnselectGutterRange();
	}

	// Determine paste measure
	long lMeasure;
	long lBeat;
	m_pStyleRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits,
				 									  0,
													  mtPasteTime,
													  &lMeasure,
													  &lBeat );

	// Don't bother to do anything if user simply moved cursor within same measure in same strip
	if( fDropNotEditPaste )
	{
		if( m_nStripIsDragDropSource )
		{
			long lSourceMeasure;
			long lSourceBeat;
			if( SUCCEEDED ( m_pStyleRefMgr->m_pTimeline->PositionToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits,
																				0,
																				m_lStartDragPosition,
																				&lSourceMeasure,
																				&lSourceBeat ) ) )
			{
				if( lSourceMeasure == lMeasure )
				{
					return S_FALSE;
				}
			}
		}
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfStyleRefList ) == S_OK )
	{
		IStream* pIStream;
		
		if( m_nStripIsDragDropSource )
		{
			m_pStyleRefMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pStyleRefMgr->UnselectAll();
		}

		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfStyleRefList, &pIStream)))
		{
			// Check for RIFF format
			if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				goto Leave;
			}

			hr = m_pStyleRefMgr->LoadStyleRefList( pIRiffStream, NULL, TRUE, mtPasteTime, fChanged );
		}
	}
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK )
	{
		IDMUSProdNode* pIDocRootNode;
		IDataObject* pIDataObject;

		if( m_nStripIsDragDropSource )
		{
			m_pStyleRefMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pStyleRefMgr->UnselectAll();
		}

		hr = pITimelineDataObject->Export( &pIDataObject );
		if( SUCCEEDED ( hr ) )
		{
			hr = m_pStyleRefMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				CStyleRefItem* pItem = new CStyleRefItem( m_pStyleRefMgr );
				if( pItem )
				{
					m_pStyleRefMgr->ClocksToMeasure( mtPasteTime, (DWORD&)pItem->m_dwMeasure );

					hr = m_pStyleRefMgr->SetStyleReference( pIDocRootNode, pItem );
					if( SUCCEEDED ( hr ) )
					{
						m_pStyleRefMgr->InsertByAscendingTime( pItem );
						pItem->SetSelectFlag( TRUE );
						*pSyncBandTempoItem = pItem;
						fChanged = TRUE;
					}
					else
					{
						delete pItem;
					}
				}

				RELEASE( pIDocRootNode );
			}

			RELEASE( pIDataObject );
		}
	}

Leave:
	RELEASE( pIRiffStream );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Paste

HRESULT CStyleRefStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfStyleRefList == 0 )
	{
		m_cfStyleRefList = RegisterClipboardFormat( CF_STYLEREFLIST );
		if( m_cfStyleRefList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfStyle == 0 )
	{
		m_cfStyle = RegisterClipboardFormat( CF_STYLE );
		if( m_cfStyle == 0 )
		{
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
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		hr = m_pStyleRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			RELEASE( pIDataObject );
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		RELEASE( pIDataObject );
		if( FAILED(hr) )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}
	}
	else
	{
		pITimelineDataObject->AddRef();
	}

	// Determine paste measure
	MUSIC_TIME mtTime;
	DWORD dwMeasure;
	if( m_bContextMenuPaste )
	{
		m_pStyleRefMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}
	}
	m_pStyleRefMgr->ClocksToMeasure( mtTime, (DWORD&)dwMeasure );
	mtTime = m_pStyleRefMgr->MeasureToClocks( dwMeasure );

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pStyleRefMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		RELEASE( pITimelineDataObject );
		return E_FAIL;
	}

	BOOL fChanged = FALSE;

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		long lStart, lEnd, lDiff;
		if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
		{
			ASSERT( lStart < lEnd );

			lDiff = lEnd - lStart;
			lStart = mtTime;
			lEnd = lStart + lDiff;

			fChanged = m_pStyleRefMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}

	// Unselect the existing StyleRefs
	m_pStyleRefMgr->UnselectAll();

	// Now, do the paste operation
	CStyleRefItem* pNotUsed = NULL;
	hr = PasteAt(pITimelineDataObject, mtTime, false, fChanged, &pNotUsed);
	RELEASE( pITimelineDataObject );

	// If successful and something changed, redraw our strip
	if( SUCCEEDED(hr) && fChanged )
	{
		// Set the last edit type
		m_nLastEdit = IDS_PASTE;

		// Update our hosting editor.
		m_pStyleRefMgr->OnDataChanged();

		// Redraw our strip
		m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pStyleRefMgr->SyncWithDirectMusic();

		// Notify the other strips of possible TimeSig change
		m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );

		m_pStyleRefMgr->OnShowProperties();
		if( m_pStyleRefMgr->m_pPropPageMgr )
		{
			m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Insert

HRESULT CStyleRefStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Unselect all items in this strip 
	m_pStyleRefMgr->UnselectAll();
	
	// Unselect items in other strips
	UnselectGutterRange();

	HRESULT hr = S_OK;
	long	lMeasure, lBeat;
	CStyleRefItem* pStyleRef = NULL;
	BOOL fNewStyleRef = FALSE;
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	hr = m_pStyleRefMgr->m_pTimeline->PositionToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED( hr ) );

	pStyleRef = GetStyleRefFromPoint( m_lXPos );
	if( pStyleRef == NULL
	||  pStyleRef->m_pIStyleDocRootNode )
	{
		pStyleRef = new CStyleRefItem( m_pStyleRefMgr );
		fNewStyleRef = TRUE;
	}
	if( pStyleRef == NULL )
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pStyleRef->m_StyleListInfo.strProjectName.LoadString( IDS_EMPTY_TEXT );
		pStyleRef->m_StyleListInfo.strName.LoadString( IDS_EMPTY_TEXT );
		pStyleRef->m_StyleListInfo.strDescriptor.LoadString( IDS_EMPTY_TEXT );
		pStyleRef->m_dwMeasure = lMeasure;
		pStyleRef->SetSelectFlag( TRUE );
		pStyleRef->m_TimeSignature.mtTime = 0;
		pStyleRef->m_TimeSignature.bBeatsPerMeasure = 4;
		pStyleRef->m_TimeSignature.bBeat = 4;
		pStyleRef->m_TimeSignature.wGridsPerBeat = 4;
		if( fNewStyleRef )
		{
			m_pStyleRefMgr->InsertByAscendingTime(pStyleRef);
		}
		m_pStyleRefMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
		DisplayPropertySheet(m_pStyleRefMgr->m_pTimeline);
		m_pStyleRefMgr->OnShowProperties();
			
		if( m_pStyleRefMgr->m_pPropPageMgr )
		{
			m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
		}
		m_nLastEdit = IDS_INSERT;
		m_pStyleRefMgr->OnDataChanged();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::Delete

HRESULT CStyleRefStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/* This may be part of a cut operation.  You can cut empty space, but you can't delete it.
	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}
	*/

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pStyleRefMgr->DeleteSelectedStyleRefs();

	m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pStyleRefMgr->m_pPropPageMgr )
	{
		m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
	}
	m_pStyleRefMgr->SyncWithDirectMusic();

	m_nLastEdit = IDS_DELETE;
	m_pStyleRefMgr->OnDataChanged();

	// Notify the other strips of possible TimeSig change
	m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::SelectAll

HRESULT CStyleRefStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pStyleRefMgr->SelectAll();

	m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CanCut

HRESULT CStyleRefStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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
// CStyleRefStrip::CanCopy

HRESULT CStyleRefStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pStyleRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pStyleRefMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CanPaste

HRESULT CStyleRefStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr = S_FALSE;

	ASSERT( m_pStyleRefMgr != NULL );
	if( m_pStyleRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfStyleRefList == 0 )
	{
		m_cfStyleRefList = RegisterClipboardFormat( CF_STYLEREFLIST );
		if( m_cfStyleRefList == 0 )
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
		if( (pITimelineDataObject->IsClipFormatAvailable( m_cfStyleRefList ) == S_OK)
		||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK) )
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
			if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					if( (pITimelineDataObject->IsClipFormatAvailable( m_cfStyleRefList ) == S_OK)
					||  (pITimelineDataObject->IsClipFormatAvailable( m_cfStyle ) == S_OK) )
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
// CStyleRefStrip::CanInsert

HRESULT CStyleRefStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pStyleRefMgr != NULL );
	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pStyleRefMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Get clock at XPos
	long lClock;
	m_pStyleRefMgr->m_pTimeline->PositionToClocks( m_lXPos, &lClock );

	// Make sure XPos is within strip
	if( lClock > 0 
	&&  lClock < lTimelineLength )
	{
		// User clicked within boundaries of strip
		long lMeasure;
		long lBeat;

		if( SUCCEEDED ( m_pStyleRefMgr->m_pTimeline->PositionToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits,
																			0,
																			m_lXPos,
																			&lMeasure,
																			&lBeat ) ) )
		{
			CPropStyleRef* pStyleRef = GetStyleRefFromPoint( m_lXPos );
			if( pStyleRef == NULL
			||  pStyleRef->m_StyleListInfo.strName.IsEmpty() )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CanDelete

HRESULT CStyleRefStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pStyleRefMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CanSelectAll

HRESULT CStyleRefStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fInRightClickMenu == FALSE )
	{
		// Always enable when gutter is selected
		if( m_bGutterSelected )
		{
			return S_OK;
		}
	}

	CStyleRefItem* pStyleRefItem;

	POSITION pos = m_pStyleRefMgr->m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos );

		if( pStyleRefItem->m_StyleListInfo.strName.IsEmpty() == FALSE )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::QueryContinueDrag

HRESULT CStyleRefStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// CStyleRefStrip::GiveFeedback

HRESULT CStyleRefStrip::GiveFeedback( DWORD dwEffect )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwEffect);

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CreateDataObject

HRESULT	CStyleRefStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Convert lPosition from pixels to clocks
	long lMeasure, lBeat;
	if( FAILED( m_pStyleRefMgr->m_pTimeline->PositionToMeasureBeat( m_pStyleRefMgr->m_dwGroupBits, 0, lPosition, &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}
	long lClocks;
	if( FAILED( m_pStyleRefMgr->m_pTimeline->MeasureBeatToClocks( m_pStyleRefMgr->m_dwGroupBits, 0, lMeasure, 0, &lClocks ) ) )
	{
		return E_UNEXPECTED;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Selected StyleRefs into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pStyleRefMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// mark the style references as being dragged: this used later for deleting style references in drag move
		m_pStyleRefMgr->MarkSelectedStyleRefs(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pStyleRefMgr->SaveSelectedStyleRefs( pIStream, lClocks ) ) )
		{
			// Place CF_STYLE_REFLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfStyleRefList, pIStream ) ) )
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
// CStyleRefStrip::DragEnter

HRESULT CStyleRefStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
// CStyleRefStrip::DragOver

HRESULT CStyleRefStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	MUSIC_TIME mtTime;
	if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
	{
		if( (mtTime >= 0) && (CanPasteFromData( m_pITargetDataObject ) == S_OK) )
		{
			BOOL fCF_STYLE = FALSE;

			// Does m_pITargetDataObject contain format CF_STYLE?
			CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
			if( pDataObject )
			{
				if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfStyle ) ) )
				{
					fCF_STYLE = TRUE;
				}
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
// CStyleRefStrip::DragLeave

HRESULT CStyleRefStrip::DragLeave( void )
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
// CStyleRefStrip::Drop

HRESULT CStyleRefStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED( m_pStyleRefMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
				{
					BOOL fChanged = FALSE;
					CStyleRefItem* pSyncBandTempoItem = NULL;
					hr = PasteAt( pITimelineDataObject, mtTime, true, fChanged, &pSyncBandTempoItem );
					if( hr == S_OK )
					{
						*pdwEffect = m_dwOverDragEffect;

						// If we pasted anything
						if( fChanged )
						{
							if( m_nStripIsDragDropSource )
							{
								// Drag/drop Target and Source are the same StyleRef strip
								m_nStripIsDragDropSource = 2;
							}
							else
							{
								// Set the last edit type
								m_nLastEdit = IDS_PASTE;

								// Update our hosting editor.
								m_pStyleRefMgr->OnDataChanged();

								// Insert style's tempo and default band (only when style dragged from project tree)
								if( pSyncBandTempoItem )
								{
									m_pStyleRefMgr->SyncStyleBandAndTempo( pSyncBandTempoItem );
								}

								// Redraw our strip
								m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

								// Update the property page
								if( m_pStyleRefMgr->m_pPropPageMgr != NULL )
								{
									m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
								}

								m_pStyleRefMgr->SyncWithDirectMusic();

								// Notify the other strips of possible TimeSig change
								m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );
							}
						}
					}
					else if( hr == S_FALSE )
					{
						*pdwEffect = DROPEFFECT_NONE;
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
// CStyleRefStrip::GetTimelineCWnd

CWnd* CStyleRefStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pStyleRefMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pStyleRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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
	}
	return pWnd;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::CanPasteFromData

HRESULT CStyleRefStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfStyleRefList ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfStyle ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::GetData

HRESULT CStyleRefStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case 0:
	{
		ioGroupBitsPPGData *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPGData *>(*ppData);
		pGroupBitsPPGData->dwGroupBits = m_pStyleRefMgr->m_dwGroupBits;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pStyleRefMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pStyleRefMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	case 2:
	{
		ioVarSeedPPGData *pVarSeedPPGData = reinterpret_cast<ioVarSeedPPGData *>(*ppData);
		pVarSeedPPGData->fVariationSeedEnabled = m_pStyleRefMgr->m_fVariationSeedActive;
		pVarSeedPPGData->dwVariationSeed = m_pStyleRefMgr->m_dwVariationSeed;
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::SetData

HRESULT CStyleRefStrip::SetData( void *pData )
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
		ioGroupBitsPPGData *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPGData *>(pData);

		if( pGroupBitsPPGData->dwGroupBits != m_pStyleRefMgr->m_dwGroupBits )
		{
			DWORD dwOrigGroupBits = m_pStyleRefMgr->m_dwGroupBits; 
			m_pStyleRefMgr->m_dwGroupBits = pGroupBitsPPGData->dwGroupBits;

			m_nLastEdit = IDS_TRACK_GROUP;
			m_pStyleRefMgr->m_pTimeline->OnDataChanged( (IStyleRefMgr*)m_pStyleRefMgr );
			m_pStyleRefMgr->m_dwOldGroupBits = pGroupBitsPPGData->dwGroupBits;

			// Time signature may have changed
			m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature,
														 (dwOrigGroupBits | m_pStyleRefMgr->m_dwGroupBits),
														  NULL );
		}
		return S_OK;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pStyleRefMgr->m_dwTrackExtrasFlags )
		{
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pStyleRefMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			m_pStyleRefMgr->m_pTimeline->OnDataChanged( (IStyleRefMgr*)m_pStyleRefMgr );
		}
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pStyleRefMgr->m_dwProducerOnlyFlags )
		{
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pStyleRefMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
			m_pStyleRefMgr->m_pTimeline->OnDataChanged( (IStyleRefMgr*)m_pStyleRefMgr );
		}
		return S_OK;
		break;
	}
	case 2:
	{
		ioVarSeedPPGData *pVarSeedPPGData = reinterpret_cast<ioVarSeedPPGData *>(pData);

		if( pVarSeedPPGData->fVariationSeedEnabled != m_pStyleRefMgr->m_fVariationSeedActive )
		{
			m_pStyleRefMgr->m_fVariationSeedActive = pVarSeedPPGData->fVariationSeedEnabled;

			if( m_pStyleRefMgr->m_pIDMTrack )
			{
				DWORD dwSeed = m_pStyleRefMgr->m_fVariationSeedActive ? m_pStyleRefMgr->m_dwVariationSeed : 0;
				VERIFY( SUCCEEDED( m_pStyleRefMgr->m_pIDMTrack->SetParam( GUID_SeedVariations, 0, &dwSeed ) ) );
			}

			if( m_pStyleRefMgr->m_fVariationSeedActive )
			{
				m_nLastEdit = IDS_ENABLE_VARIATION_SEED;
			}
			else
			{
				m_nLastEdit = IDS_DISABLE_VARIATION_SEED;
			}
			m_pStyleRefMgr->m_pTimeline->OnDataChanged( (IStyleRefMgr*)m_pStyleRefMgr );

			return S_OK;
		}

		else if( m_pStyleRefMgr->m_fVariationSeedActive )
		{
			if( pVarSeedPPGData->dwVariationSeed != m_pStyleRefMgr->m_dwVariationSeed )
			{
				m_pStyleRefMgr->m_dwVariationSeed = pVarSeedPPGData->dwVariationSeed;

				if( m_pStyleRefMgr->m_pIDMTrack )
				{
					VERIFY( SUCCEEDED( m_pStyleRefMgr->m_pIDMTrack->SetParam( GUID_SeedVariations, 0, &m_pStyleRefMgr->m_dwVariationSeed ) ) );
				}

				m_nLastEdit = IDS_CHANGE_VARIATION_SEED;
				m_pStyleRefMgr->m_pTimeline->OnDataChanged( (IStyleRefMgr*)m_pStyleRefMgr );
			}

			return S_OK;
		}
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::OnShowProperties

HRESULT CStyleRefStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pStyleRefMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pStyleRefMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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

	// If our property page is already displayed, refresh its data and exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pPropPageMgr != NULL );

        if (m_pPropPageMgr)
        {
            m_pPropPageMgr->RefreshData();
        }

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

	// Set the property page to refer to the groupbits and/or variation seed property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pStyleRefMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	RELEASE( pIPropSheet );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::OnRemoveFromPageManager

HRESULT CStyleRefStrip::OnRemoveFromPageManager( void )
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
// CStyleRefStrip::DisplayPropertySheet

HRESULT CStyleRefStrip::DisplayPropertySheet(IDMUSProdTimeline* pTimeline)
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
// CStyleRefStrip::DoDragDrop

BOOL CStyleRefStrip::DoDragDrop( void )
{
	// Drag drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pStyleRefMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Query ourself for our IDropSource interface
	IDropSource* pIDropSource;
	if( FAILED ( QueryInterface(IID_IDropSource, (void**)&pIDropSource) ) )
	{
		return FALSE;
	}

	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a data object from the selected style references
	HRESULT hr = CreateDataObject( &m_pISourceDataObject, m_lStartDragPosition );
	if( SUCCEEDED( hr ) )
	{
		// We can always copy style references.
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
		if( CanCut() == S_OK )
		{
			// If we can Cut(), allow the user to move the style references as well.
			dwOKDragEffects |= DROPEFFECT_MOVE;
		}

		// Do the Drag/Drop.
		hr = ::DoDragDrop( m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect );

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					m_pStyleRefMgr->DeleteMarked( UD_DRAGSELECT );
				}
				break;

			default:
				dwEffect = DROPEFFECT_NONE;
				break;
		}
		hr = S_OK;
		RELEASE( m_pISourceDataObject );
	}

	RELEASE( pIDropSource );

	m_pStyleRefMgr->UnMarkStyleRefs(UD_DRAGSELECT);
	m_pStyleRefMgr->SyncWithDirectMusic();

	if( dwEffect != DROPEFFECT_NONE )
	{
		if( m_nStripIsDragDropSource == 2 )
		{
			// Drag/drop target and source are the same StyleRef strip
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_UNDO_MOVE;
				m_pStyleRefMgr->OnDataChanged();
				// Notify the other strips of possible TimeSig change
				m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_nLastEdit = IDS_PASTE;
				m_pStyleRefMgr->OnDataChanged();
				// Notify the other strips of possible TimeSig change
				m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );
			}
		}
		else
		{
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_DELETE;
				m_pStyleRefMgr->OnDataChanged();
				// Notify the other strips of possible TimeSig change
				m_pStyleRefMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pStyleRefMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_pStyleRefMgr->UnselectAll();
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
// CStyleRefStrip::OnLButtonDown

HRESULT CStyleRefStrip::OnLButtonDown( WPARAM wParam, LONG lXPos )
{
	ASSERT( m_pStyleRefMgr != NULL );
	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );

	m_fSingleSelect = FALSE;

	// If we're already dragging, just return
	// BUGBUG: Need to a better indicator
	/*
	if( m_pDragImage )
	{
		return S_OK;
	}
	*/

	// Get Timeline length
	VARIANT var;
	m_pStyleRefMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pStyleRefMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
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
	m_pStyleRefMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// See if there is a style reference under the cursor.
	CStyleRefItem* pStyleRef = GetStyleRefFromPoint( lXPos );
	if( pStyleRef
	&&  pStyleRef->m_StyleListInfo.strName.IsEmpty() == FALSE )
	{
		// There is a StyleRef on this measure
		if( wParam & MK_CONTROL )
		{
			if( pStyleRef->m_fSelected )
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
					pStyleRef->SetSelectFlag( !pStyleRef->m_fSelected );
					m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
					pStyleRef->m_dwBits |= UD_CURRENTSELECTION;
				}
			}
			else
			{
				pStyleRef->SetSelectFlag( !pStyleRef->m_fSelected );
				if( pStyleRef->m_fSelected )
				{
					// Set fields to initiate drag/drop on next mousemove
					m_dwStartDragButton = (unsigned long)wParam;
					m_lStartDragPosition = lXPos;
				}
				m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
				pStyleRef->m_dwBits |= UD_CURRENTSELECTION;
			}
		}
		else if( wParam & MK_SHIFT )
		{
			pStyleRef->SetSelectFlag( TRUE );
			SelectRange( pStyleRef );
		}
		else
		{
			if( pStyleRef->m_fSelected == FALSE )
			{
				m_pStyleRefMgr->UnselectAll();
				pStyleRef->SetSelectFlag( TRUE );
				m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
				pStyleRef->m_dwBits |= UD_CURRENTSELECTION;
			}
			else
			{
				m_fSingleSelect = TRUE;
			}
			
			// Set fields to initiate drag/drop on next mousemove
			m_dwStartDragButton = (unsigned long)wParam;
			m_lStartDragPosition = lXPos;
		}
	}
	else
	{
		// There is not a "real" StyleRef on this measure
		hr = S_OK;
		if( pStyleRef == NULL )
		{
			// Create a "fake" StyleRef
			hr = m_pStyleRefMgr->CreateStyleRef( lXPos, pStyleRef );
		}

		if( SUCCEEDED ( hr ) )
		{
			if( wParam & MK_CONTROL )
			{
				pStyleRef->SetSelectFlag( !pStyleRef->m_fSelected );
				m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
				pStyleRef->m_dwBits |= UD_CURRENTSELECTION;
			}
			else if( wParam & MK_SHIFT )
			{
				pStyleRef->SetSelectFlag( TRUE );
				SelectRange( pStyleRef );
			}
			else
			{
				// Click on empty space deselects all
				m_pStyleRefMgr->UnselectAll();

				pStyleRef->SetSelectFlag( TRUE );
				m_pStyleRefMgr->UnMarkStyleRefs( UD_CURRENTSELECTION );
				pStyleRef->m_dwBits |= UD_CURRENTSELECTION;
			}
		}
	}

	// Redraw the strip and refresh the StyleRef property page
	m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	m_pStyleRefMgr->OnShowProperties();
	if( m_pStyleRefMgr->m_pPropPageMgr )
	{
		m_pStyleRefMgr->m_pPropPageMgr->RefreshData();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::OnRButtonDown

HRESULT CStyleRefStrip::OnRButtonDown( WPARAM wParam, LONG lXPos )
{
	ASSERT( m_pStyleRefMgr != NULL );
	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pStyleRefMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pStyleRefMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
	if( lClock >= lTimelineLength )
	{
		return S_OK;
	}

	UnselectGutterRange();

	// Get the item at the mouse click.
	CStyleRefItem* pStyleRef = GetStyleRefFromPoint( lXPos );
	
	if( pStyleRef == NULL )
	{
		// No StyleRef so create a "fake" one
		m_pStyleRefMgr->UnselectAll();
		if( SUCCEEDED ( m_pStyleRefMgr->CreateStyleRef( lXPos, pStyleRef ) ) )
		{
			pStyleRef->SetSelectFlag( TRUE );
		}
	}
	else if( !(wParam & MK_CONTROL)
		 &&  !(wParam & MK_SHIFT) )
	{
		if( pStyleRef )
		{
			if( pStyleRef->m_fSelected == FALSE )
			{
				m_pStyleRefMgr->UnselectAll();
				pStyleRef->SetSelectFlag( TRUE );
			}
		}
	}

	m_pStyleRefMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	m_fShowStyleRefProps = TRUE;
	m_pStyleRefMgr->OnShowProperties(); 
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::UnselectGutterRange

void CStyleRefStrip::UnselectGutterRange( void )
{
	ASSERT( m_pStyleRefMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pStyleRefMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pStyleRefMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefStrip::SelectRange

HRESULT CStyleRefStrip::SelectRange( CStyleRefItem* pStyleRef )
{
	HRESULT hr = S_OK;

	// Select inclusive, find StyleRef to start from
	CStyleRefItem* pCurrent = m_pStyleRefMgr->CurrentlySelectedStyleRef();

	// Find measure for StyleRef to start from and StyleRef to end at
	DWORD dwEarlyMeasure = (pCurrent) ? pCurrent->m_dwMeasure : 0;
	DWORD dwLateMeasure = pStyleRef->m_dwMeasure;

	// If the order is backwards, swap
	if( dwLateMeasure < dwEarlyMeasure )
	{
		DWORD dwTemp = dwEarlyMeasure;
		dwEarlyMeasure = dwLateMeasure;
		dwLateMeasure = dwTemp;
	}

	// Add empty empty StyleRefs to fill in gaps
	CStyleRefItem* pStyleRefItem = NULL;
	DWORD dwCurrentMeasure = dwEarlyMeasure;
	while( dwCurrentMeasure <= dwLateMeasure )
	{
		if( GetStyleRefFromMeasureBeat(dwCurrentMeasure, 0) == NULL )
		{
			pStyleRefItem = NULL;
			hr = m_pStyleRefMgr->CreateStyleRef( dwCurrentMeasure, pStyleRefItem );
		}
		dwCurrentMeasure++;
	}

	// Unselect all StyleRefs in the strip
	m_pStyleRefMgr->UnselectAllKeepBits();

	// Select all StyleRefs in the range (can't simply select StyleRefs in the
	// above loop, because there may be multiple StyleRefs at the same measure)
	POSITION pos = m_pStyleRefMgr->m_lstStyleRefs.GetHeadPosition();
	while( pos )
	{
		pStyleRefItem = m_pStyleRefMgr->m_lstStyleRefs.GetNext( pos );

		if( pStyleRefItem->m_dwMeasure >= dwEarlyMeasure
		&&	pStyleRefItem->m_dwMeasure <= dwLateMeasure )
		{
			pStyleRefItem->m_fSelected = TRUE;
		}
	}

	return hr;
}
