// TimeSigStrip.cpp : Implementation of CTimeSigStrip
#include "stdafx.h"
#include "TimeSigIO.h"
#include "TimeSigStripMgr.h"
#include "TimeSigMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include <RiffStrm.h>
#include <StyleDesigner.h>
#include "musictimeconverter.h"
#include "SegmentIO.h"
#include "TrackFlagsPPG.h"
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
// CTimeSigStrip constructor/destructor

CTimeSigStrip::CTimeSigStrip( CTimeSigMgr* pTimeSigMgr )
{
	ASSERT( pTimeSigMgr );
	if ( pTimeSigMgr == NULL )
	{
		return;
	}

	m_pTimeSigMgr = pTimeSigMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pTimeSigMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;
	m_bGutterSelected = FALSE;

	m_cfTimeSigList = 0;
	m_cfStyle = 0;

	m_lXPos = -1;
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
	m_fShowTimeSigProps = FALSE;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;
	m_fInRightClickMenu = FALSE;
	m_fSingleSelect = FALSE;
	m_dwShiftSelectFromMeasure = 0;
}

CTimeSigStrip::~CTimeSigStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pTimeSigMgr = NULL;
	}
	RELEASE( m_pISourceDataObject );
	RELEASE( m_pITargetDataObject );
	RELEASE( m_pPropPageMgr );
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::QueryInterface

STDMETHODIMP CTimeSigStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
// CTimeSigStrip::AddRef

STDMETHODIMP_(ULONG) CTimeSigStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Release

STDMETHODIMP_(ULONG) CTimeSigStrip::Release(void)
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
// CTimeSigStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Draw

HRESULT	STDMETHODCALLTYPE CTimeSigStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	BOOL fUseGutterSelectRange = FALSE;
	if( m_bGutterSelected
	&&  m_lGutterBeginSelect != m_lGutterEndSelect )
	{
		fUseGutterSelectRange = TRUE;
	}

	// Validate pointer to the Timeline
	if( m_pTimeSigMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pTimeSigMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pTimeSigMgr->m_dwGroupBits, 0, lXOffset );
		
		// variables for ghosting
		CDC dc;
		IDMUSProdTimeline* pTimeline = m_pTimeSigMgr->m_pTimeline;
		CString	strGhost;
		RECT	rectGhost;
		long	lFirstVisibleMeasure, lPartialVisibleMeasure;
		long	lClocks;
		bool	bGhostTimeSigCovered = false;
		CTimeSigItem*	pGhostTimeSig = NULL;

		// Validate our device context
		if( hDC && dc.Attach(hDC))
		{
			RECT	rectClip, rectHighlight;
			long	lStartTime, lPosition;
			CTimeSigItem* pTimeSigItem;
			SIZE	sizeText;

			::GetClipBox( hDC, &rectClip );
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;
			rectGhost.top = rectGhost.left = rectGhost.right = 0;
			rectGhost.bottom = STRIP_HEIGHT;
			// find first visible measure for ghosting timesigs
			pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasure(lPartialVisibleMeasure, pTimeline, m_pTimeSigMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasureBeat(lPartialVisibleMeasure, 0, pTimeline, 
														m_pTimeSigMgr->m_dwGroupBits);
			if(cmtNearestMeasureBeat.Time() < cmtFirstVisible.Time())
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure + 1;
			}
			else
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure;
			}
	
			// find last timesig before first visible measure
			POSITION pos = m_pTimeSigMgr->m_lstTimeSigs.GetHeadPosition();
			while(pos)
			{
				pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetNext(pos);
				if(pTimeSigItem->m_dwMeasure < (unsigned)lFirstVisibleMeasure
					&& !(*pTimeSigItem->GetString() == 0))
				{
					pGhostTimeSig = pTimeSigItem;
				}
				else
				{
					break;
				}
			}
			if(pGhostTimeSig)
			{
				// get rect of style, will have to truncate if ghost style covers part of real style
				pTimeline->MeasureBeatToPosition(m_pTimeSigMgr->m_dwGroupBits, 0, lFirstVisibleMeasure,
												0, &lPosition);
				lPosition++;
				CSize csize;
				csize = dc.GetTextExtent(pGhostTimeSig->GetString());
				rectGhost.left = lPosition - lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
			}

			// we'll draw ghost timesig after checking whether real timesig covers it

			m_pTimeSigMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

			// Initialize the text color
			COLORREF crOldTextColor = ::SetTextColor( hDC, m_pTimeSigMgr->m_fHaveStyleRefStrip ? RGB(168, 168, 168) : RGB(0, 0, 0) );

			// Iterate through all time sigs
			pos = m_pTimeSigMgr->m_lstTimeSigs.GetHeadPosition();
			while( pos )
			{
				pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos );

				if(pTimeSigItem->m_dwMeasure == (unsigned)lFirstVisibleMeasure)
				{
					// real timesig covers ghost
					bGhostTimeSigCovered = true;
				}

				// Compute the position of the time signatures
				m_pTimeSigMgr->m_pTimeline->MeasureBeatToPosition( m_pTimeSigMgr->m_dwGroupBits, 0, pTimeSigItem->m_dwMeasure, 0, &lPosition );

				if(pGhostTimeSig)
				{
					//truncate ghost timesig's text so it doesn't cover up real timesig's text
					long lTruePos = lPosition - lXOffset;
					if(lTruePos > rectGhost.left && lTruePos < rectGhost.right)
					{
						rectGhost.right = lTruePos;
					}
				}

				// If it's not visible, break out of the loop
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				// Otherwise, draw it one pixel to the right (so it doesn't overwrite the measure line)
				::TextOut( hDC, lPosition - lXOffset + 1, 0, pTimeSigItem->GetString(), _tcslen(pTimeSigItem->GetString()) );
			}


			// make sure the first selected TimeSig is shown in its entirety
			pTimeSigItem = m_pTimeSigMgr->FirstSelectedTimeSig();
			if( pTimeSigItem )
			{
				pos = m_pTimeSigMgr->m_lstTimeSigs.Find( pTimeSigItem, NULL );
				while( pos )
				{
					if( pTimeSigItem->m_fSelected )
					{
						long lMeasureLength = 0;
						m_pTimeSigMgr->m_pTimeline->MeasureBeatToPosition( m_pTimeSigMgr->m_dwGroupBits, 0,
														  pTimeSigItem->m_dwMeasure, 0, &lPosition );
						m_pTimeSigMgr->m_pTimeline->MeasureBeatToPosition( m_pTimeSigMgr->m_dwGroupBits, 0,
														 (pTimeSigItem->m_dwMeasure + 1), 0, &lMeasureLength );
						lMeasureLength -= lPosition;

						// find extent of text
						::GetTextExtentPoint32( hDC, pTimeSigItem->GetString(), _tcslen(pTimeSigItem->GetString()), &sizeText );
						if( sizeText.cx < lMeasureLength )
						{
							sizeText.cx = lMeasureLength;
						}
						long lExtent = lPosition + sizeText.cx;

						// truncate if a selected TimeSig covers part of text
						POSITION pos2 = pos;
						CTimeSigItem* pTempTimeSig;
						bool fDone = false;
						while( pos2 && !fDone)
						{
							m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos2 );
							if (!pos2) break;
							pTempTimeSig = m_pTimeSigMgr->m_lstTimeSigs.GetAt( pos2 );
							if( pTempTimeSig->m_fSelected )
							{	
								long lRPos;
								m_pTimeSigMgr->m_pTimeline->MeasureBeatToPosition( m_pTimeSigMgr->m_dwGroupBits, 0, pTempTimeSig->m_dwMeasure, 0, &lRPos );
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
							::DrawText( hDC, pTimeSigItem->GetString(), _tcslen(pTimeSigItem->GetString()), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
							GrayOutRect( hDC, &rectHighlight );
						}
					}
					m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos );
					if (pos) pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetAt( pos );
				}
			}

			// now draw ghost TimeSig
			// but not if original TimeSig is still showing
			if(!bGhostTimeSigCovered && pGhostTimeSig)
			{
				pTimeline->MeasureBeatToPosition(m_pTimeSigMgr->m_dwGroupBits, 0,
													pGhostTimeSig->m_dwMeasure, 0, &lPosition);
						
				// lPosition -= lXOffset deliberately left off as left margin already has scrolling taking into accout
				CSize extent = dc.GetTextExtent(pGhostTimeSig->GetString());
				if(!((lPosition + extent.cx) > LeftMargin(pTimeline)))
				{
					COLORREF cr = dc.SetTextColor(RGB(168, 168, 168));
					dc.DrawText(pGhostTimeSig->GetString(), &rectGhost, (DT_LEFT | DT_NOPREFIX));
					dc.SetTextColor(cr);
				}
			}

			// Reset Text color
			::SetTextColor( hDC, crOldTextColor );

			// Highlight the selected range if there is one.
			if( fUseGutterSelectRange )
			{
				long lBeginSelect = m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterEndSelect : m_lGutterBeginSelect;
				long lEndSelect = m_lGutterBeginSelect > m_lGutterEndSelect ? m_lGutterBeginSelect : m_lGutterEndSelect;

				long lMeasure;
				long lBeat;
				long lClocks;

				m_pTimeSigMgr->m_pTimeline->ClocksToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, lBeginSelect, &lMeasure, &lBeat );
				m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, lMeasure, 0, &lClocks );
				m_pTimeSigMgr->m_pTimeline->ClocksToPosition( lClocks, &(rectHighlight.left));

				m_pTimeSigMgr->m_pTimeline->ClocksToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, lEndSelect, &lMeasure, &lBeat );
				m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, (lMeasure + 1), 0, &lClocks );
				m_pTimeSigMgr->m_pTimeline->ClocksToPosition( (lClocks - 1), &(rectHighlight.right));

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
// CTimeSigStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CTimeSigStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
			CString strBaseName;
			strBaseName.LoadString( IDS_FUNCTIONBAR_NAME );
			CString str = GetName(m_pTimeSigMgr->m_dwGroupBits, strBaseName);

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
		if( m_pTimeSigMgr )
		{
			m_pTimeSigMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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
// CTimeSigStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CTimeSigStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fRefresh = FALSE;

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
			if( m_pTimeSigMgr->UnselectAll() )
			{
				// Only redraw if something changed
				m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			break;
		}

		if( m_bGutterSelected )
		{
			//TODO: Optimize this
			m_pTimeSigMgr->UnselectAll();
			m_pTimeSigMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
			fRefresh = TRUE;
		}
		else
		{
			// Why does this need to happen?
			fRefresh |= m_pTimeSigMgr->UnselectAll();
		}

		if( fRefresh )
		{
			m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

			// Update the property page
			if( m_pTimeSigMgr->m_pPropPageMgr != NULL )
			{
				m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pTimeSigMgr->UnselectAll();
			m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pTimeSigMgr->UnselectAll();
			m_pTimeSigMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pTimeSigMgr->UnselectAll();
		}
		m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pTimeSigMgr->m_pPropPageMgr != NULL )
		{
			m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::GetTimeSigFromMeasureBeat

CTimeSigItem *CTimeSigStrip::GetTimeSigFromMeasureBeat( DWORD dwMeasure, BYTE bBeat )
{
	ASSERT( bBeat == 0 );	// Time Sigs must be on a measure boundary
	
	CTimeSigItem* pTimeSigItem = NULL;

	POSITION pos = m_pTimeSigMgr->m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos );
		ASSERT( pTimeSigItem != NULL );
		if ( pTimeSigItem != NULL )
		{
			if ( pTimeSigItem->m_dwMeasure == dwMeasure )
			{
				return pTimeSigItem;
			}
			else if ( pTimeSigItem->m_dwMeasure > dwMeasure )
			{
				break;
			}
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::GetTimeSigFromPoint

CTimeSigItem *CTimeSigStrip::GetTimeSigFromPoint( long lPos )
{
	CTimeSigItem*		pTimeSigReturn = NULL;

	if( m_pTimeSigMgr->m_pTimeline )
	{
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->PositionToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pTimeSigReturn = GetTimeSigFromMeasureBeat( lMeasure, 0 );
		}
	}
	return pTimeSigReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CTimeSigStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		m_fShowTimeSigProps = TRUE;
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

		// Save the position of the click so we know where to insert a TimeSig. if Insert is selected.
		m_lXPos = lXPos;
		m_fInRightClickMenu = TRUE;
		m_pTimeSigMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
		m_fInRightClickMenu = FALSE;

		hr = S_OK;
		break;

	case WM_MOUSEMOVE:
		if( m_dwStartDragButton )
		{
			m_nStripIsDragDropSource = 1;
	
			if( DoDragDrop() )
			{
				// Redraw the strip and refresh the TimeSig property page
				m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pTimeSigMgr->OnShowProperties();
				if( m_pTimeSigMgr->m_pPropPageMgr )
				{
					m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
				}
			}

			m_dwStartDragButton = 0;
			m_nStripIsDragDropSource = 0;
		}
		break;

	case WM_SETFOCUS:
		m_dwShiftSelectFromMeasure = 0;
		hr = S_OK;
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
			hr = DisplayPropertySheet(m_pTimeSigMgr->m_pTimeline);
			if (m_fShowTimeSigProps)
			{
				// Change to the Time sig reference property page
				m_pTimeSigMgr->OnShowProperties();
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
			CTimeSigItem* pTimeSig = GetTimeSigFromPoint( lXPos );
			if( pTimeSig
			&&  !(pTimeSig->m_dwBits & UD_FAKE) )
			{
				m_pTimeSigMgr->UnselectAll();
				pTimeSig->SetSelectFlag( TRUE );

				// Redraw the strip and refresh the Tempo property page
				m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pTimeSigMgr->OnShowProperties();
				if( m_pTimeSigMgr->m_pPropPageMgr )
				{
					m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
				}
			}
			m_fSingleSelect = FALSE;
		}
		m_lXPos = lXPos;
		hr = S_OK;
		break;

	case WM_CREATE:
		m_cfTimeSigList = RegisterClipboardFormat( CF_TIMESIGLIST );
		m_cfStyle = RegisterClipboardFormat( CF_STYLE);

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );

		// Unselect all
		m_pTimeSigMgr->UnselectAll();

		// Reset m_dwShiftSelectFromMeasure
		m_dwShiftSelectFromMeasure = 0;
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::FBDraw

HRESULT CTimeSigStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	UNREFERENCED_PARAMETER(hDC);
	UNREFERENCED_PARAMETER(sv);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::FBOnWMMessage

HRESULT CTimeSigStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
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
		m_fShowTimeSigProps = FALSE;
		OnShowProperties();
		break;

	case WM_RBUTTONUP:
		m_fShowTimeSigProps = FALSE;
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

		if( m_pTimeSigMgr->m_pTimeline )
		{
			m_fInRightClickMenu = TRUE;
			m_pTimeSigMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
			m_fInRightClickMenu = FALSE;
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Cut

HRESULT CTimeSigStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	hr = CanCut();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
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
// CTimeSigStrip::Copy

HRESULT CTimeSigStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfTimeSigList == 0 )
	{
		m_cfTimeSigList = RegisterClipboardFormat( CF_TIMESIGLIST );
		if( m_cfTimeSigList == 0 )
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

	m_pTimeSigMgr->MarkSelectedTimeSigs(UD_DRAGSELECT);

	MUSIC_TIME mtTime;
	if( pITimelineDataObject )
	{
		long lMeasure, lBeat;

		// Get measure boundary of pITimelineDataObject clock
		if( FAILED( pITimelineDataObject->GetBoundaries( &mtTime, NULL ) ) )
		{
			return E_UNEXPECTED;
		}
		m_pTimeSigMgr->m_pTimeline->ClocksToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
		m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, lMeasure, 0, &mtTime );
	}
	else
	{
		// Get clock of first selected Time Sig's measure
		CTimeSigItem* pTimeSigAtDragPoint = m_pTimeSigMgr->FirstSelectedTimeSig();
		ASSERT( pTimeSigAtDragPoint != NULL );
		if( pTimeSigAtDragPoint == NULL )
		{
			return E_UNEXPECTED;
		}
		m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, pTimeSigAtDragPoint->m_dwMeasure, 0, &mtTime );
	}

	// Save the Time Sigs into the stream.
	hr = m_pTimeSigMgr->SaveSelectedTimeSigs( pStreamCopy, mtTime );
	if( FAILED( hr ))
	{
		RELEASE( pStreamCopy );
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfTimeSigList, pStreamCopy );
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
		hr = m_pTimeSigMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pTimeSigMgr->GetBoundariesOfSelectedTimeSigs( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfTimeSigList, pStreamCopy );

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
		RELEASE( m_pTimeSigMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pTimeSigMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pTimeSigMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::PasteAt

HRESULT CTimeSigStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtPasteTime, BOOL fDropNotEditPaste, BOOL &fChanged)
{
	ASSERT( m_pTimeSigMgr != NULL );
	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );

	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Determine paste measure
	long lMeasure;
	long lBeat;
	m_pTimeSigMgr->m_pTimeline->ClocksToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits,
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
			if( SUCCEEDED ( m_pTimeSigMgr->m_pTimeline->PositionToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits,
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

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfTimeSigList ) == S_OK )
	{
		IStream* pIStream;
		
		if( m_nStripIsDragDropSource )
		{
			m_pTimeSigMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pTimeSigMgr->UnselectAll();
		}

		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfTimeSigList, &pIStream)))
		{
			STATSTG StatStg;
			ZeroMemory( &StatStg, sizeof(STATSTG) );
			pIStream->Stat( &StatStg, STATFLAG_NONAME );
			hr = m_pTimeSigMgr->LoadTimeSigChunk( pIStream, StatStg.cbSize.LowPart, TRUE, mtPasteTime );
			if( hr == S_OK )
			{
				fChanged = TRUE;
			}
		}
	}

	if( m_nStripIsDragDropSource )
	{
		// Drag/drop Target and Source are the same TimeSig strip
		m_nStripIsDragDropSource = 2;
	}

	RELEASE( pIRiffStream );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Paste

HRESULT CTimeSigStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfTimeSigList == 0 )
	{
		m_cfTimeSigList = RegisterClipboardFormat( CF_TIMESIGLIST );
		if( m_cfTimeSigList == 0 )
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
		hr = m_pTimeSigMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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
	long lMeasure, lBeat;
	if( m_bContextMenuPaste )
	{
		m_pTimeSigMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}
	}
	m_pTimeSigMgr->m_pTimeline->ClocksToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, lMeasure, 0, &mtTime );

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pTimeSigMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
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

			fChanged = m_pTimeSigMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}

	// Now, do the paste operation
	hr = PasteAt(pITimelineDataObject, mtTime, false, fChanged);
	RELEASE( pITimelineDataObject );

	// If S_OK, redraw our strip
	if( SUCCEEDED(hr)  &&  fChanged )
	{
		m_nLastEdit = IDS_PASTE;
		m_pTimeSigMgr->OnDataChanged();
		m_pTimeSigMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Update the property page
		if( m_pTimeSigMgr->m_pPropPageMgr != NULL )
		{
			m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
		}

		m_pTimeSigMgr->SyncWithDirectMusic();

		// Notify the other strips of possible TimeSig change
		m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Insert

HRESULT CTimeSigStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = CanInsert();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	// Unselect all items in this strip 
	m_pTimeSigMgr->UnselectAll();
	
	// Unselect items in other strips
	UnselectGutterRange();

	long	lMeasure, lBeat;
	CTimeSigItem* pTimeSig = NULL;
	BOOL fNewTimeSig = FALSE;
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	hr = m_pTimeSigMgr->m_pTimeline->PositionToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED( hr ) );

	pTimeSig = GetTimeSigFromPoint( m_lXPos );
	if( pTimeSig == NULL )
	{
		pTimeSig = new CTimeSigItem( m_pTimeSigMgr );
		fNewTimeSig = TRUE;
	}
	if( pTimeSig == NULL )
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pTimeSig->m_dwMeasure = lMeasure;
		pTimeSig->SetSelectFlag( TRUE );
		pTimeSig->m_dwBits &= ~UD_FAKE;
		pTimeSig->m_TimeSignature.mtTime = 0;
		pTimeSig->m_TimeSignature.bBeatsPerMeasure = 4;
		pTimeSig->m_TimeSignature.bBeat = 4;
		pTimeSig->m_TimeSignature.wGridsPerBeat = 4;
		if( fNewTimeSig )
		{
			m_pTimeSigMgr->InsertByAscendingTime(pTimeSig);
		}

		// Redraw our strip
		m_pTimeSigMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Display the property sheet
		DisplayPropertySheet(m_pTimeSigMgr->m_pTimeline);
		m_pTimeSigMgr->OnShowProperties();

		// If the property page manager exists, refresh it (it should exist)
		if( m_pTimeSigMgr->m_pPropPageMgr )
		{
			m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
		}

		// Update the Segment Designer
		m_nLastEdit = IDS_INSERT;
		m_pTimeSigMgr->OnDataChanged();

		// Sync with DirectMusic
		m_pTimeSigMgr->SyncWithDirectMusic();

		// Notify the other strips of possible TimeSig change
		m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::Delete

HRESULT CTimeSigStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/* Ignore this, since we can be in a Cut() operation.  We can cut blank data, but we can't delete it.
	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}
	*/

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pTimeSigMgr->DeleteSelectedTimeSigs();

	m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pTimeSigMgr->m_pPropPageMgr )
	{
		m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
	}

	// Update the Segment Designer
	m_nLastEdit = IDS_DELETE;
	m_pTimeSigMgr->OnDataChanged();

	// Sync with DirectMusic
	m_pTimeSigMgr->SyncWithDirectMusic();

	// Notify the other strips of possible TimeSig change
	m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::SelectAll

HRESULT CTimeSigStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pTimeSigMgr->SelectAll();

	m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::CanCut

HRESULT CTimeSigStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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
// CTimeSigStrip::CanCopy

HRESULT CTimeSigStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pTimeSigMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pTimeSigMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::CanPaste

HRESULT CTimeSigStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr = S_FALSE;

	ASSERT( m_pTimeSigMgr != NULL );
	if( m_pTimeSigMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfTimeSigList == 0 )
	{
		m_cfTimeSigList = RegisterClipboardFormat( CF_TIMESIGLIST );
		if( m_cfTimeSigList == 0 )
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
		if( (pITimelineDataObject->IsClipFormatAvailable( m_cfTimeSigList ) == S_OK)
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
			if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					if( (pITimelineDataObject->IsClipFormatAvailable( m_cfTimeSigList ) == S_OK)
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
// CTimeSigStrip::CanInsert

HRESULT CTimeSigStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeSigMgr != NULL );
	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	// Check for existing TimeSig
	if( m_lXPos >= 0 )
	{
		CPropTimeSig* pTimeSig = GetTimeSigFromPoint( m_lXPos );
		if( pTimeSig == NULL
		||  (pTimeSig->m_dwBits & UD_FAKE) )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::CanDelete

HRESULT CTimeSigStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		return S_FALSE;
	}

	return m_pTimeSigMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::CanSelectAll

HRESULT CTimeSigStrip::CanSelectAll( void )
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

	CTimeSigItem* pTimeSigItem;

	POSITION pos = m_pTimeSigMgr->m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos );

		if( !(pTimeSigItem->m_dwBits & UD_FAKE) )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::QueryContinueDrag

HRESULT CTimeSigStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// CTimeSigStrip::GiveFeedback

HRESULT CTimeSigStrip::GiveFeedback( DWORD dwEffect )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwEffect);

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::CreateDataObject

HRESULT	CTimeSigStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Convert lPosition from pixels to clocks
	long lMeasure, lBeat;
	if( FAILED( m_pTimeSigMgr->m_pTimeline->PositionToMeasureBeat( m_pTimeSigMgr->m_dwGroupBits, 0, lPosition, &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}
	long lClocks;
	if( FAILED( m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( m_pTimeSigMgr->m_dwGroupBits, 0, lMeasure, 0, &lClocks ) ) )
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

	// Save Selected TimeSigs into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pTimeSigMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// mark the time signatures as being dragged: this used later for deleting time signatures in drag move
		m_pTimeSigMgr->MarkSelectedTimeSigs(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pTimeSigMgr->SaveSelectedTimeSigs( pIStream, lClocks ) ) )
		{
			// Place CF_TIMESIGLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfTimeSigList, pIStream ) ) )
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
// CTimeSigStrip::DragEnter

HRESULT CTimeSigStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
// CTimeSigStrip::DragOver

HRESULT CTimeSigStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( m_pTimeSigMgr->m_fHaveStyleRefStrip )
	{
		*pdwEffect = dwEffect;
		return S_OK;
	}

	MUSIC_TIME mtTime;
	if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
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
// CTimeSigStrip::DragLeave

HRESULT CTimeSigStrip::DragLeave( void )
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
// CTimeSigStrip::Drop

HRESULT CTimeSigStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED( m_pTimeSigMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
				{
					BOOL fChanged = FALSE;
					hr = PasteAt( pITimelineDataObject, mtTime, true, fChanged );
					if( hr == S_OK )
					{
						*pdwEffect = m_dwOverDragEffect;

						if( fChanged )
						{
							if( m_nStripIsDragDropSource != 2 )
							{
								// Target strip is different from source strip
								m_nLastEdit = IDS_PASTE;
								m_pTimeSigMgr->OnDataChanged();
								m_pTimeSigMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

								// Update the property page
								if( m_pTimeSigMgr->m_pPropPageMgr != NULL )
								{
									m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
								}

								m_pTimeSigMgr->SyncWithDirectMusic();

								// Notify the other strips of possible TimeSig change
								m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
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
// CTimeSigStrip::GetTimelineCWnd

CWnd* CTimeSigStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pTimeSigMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pTimeSigMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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
// CTimeSigStrip::CanPasteFromData

HRESULT CTimeSigStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfTimeSigList ) )
	/*||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfStyle ) )*/ )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::GetData

HRESULT CTimeSigStrip::GetData( void **ppData )
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
		ioGroupBitsPPGData *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPGData *>(*ppData);
		pGroupBitsPPGData->dwGroupBits = m_pTimeSigMgr->m_dwGroupBits;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pTimeSigMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pTimeSigMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	default:
		ASSERT(FALSE);
		break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::SetData

HRESULT CTimeSigStrip::SetData( void *pData )
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

		if( pGroupBitsPPGData->dwGroupBits != m_pTimeSigMgr->m_dwGroupBits )
		{
			DWORD dwOrigGroupBits = m_pTimeSigMgr->m_dwGroupBits; 
			m_pTimeSigMgr->m_dwGroupBits = pGroupBitsPPGData->dwGroupBits;

			m_nLastEdit = IDS_TRACK_GROUP;
			m_pTimeSigMgr->m_pTimeline->OnDataChanged( (ITimeSigMgr*)m_pTimeSigMgr );

			// Time signature may have changed
			m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature,
														 (dwOrigGroupBits | m_pTimeSigMgr->m_dwGroupBits),
														  NULL );
		}
		return S_OK;
		break;
	}
	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pTimeSigMgr->m_dwTrackExtrasFlags )
		{
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pTimeSigMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;
			m_pTimeSigMgr->m_pTimeline->OnDataChanged( (ITimeSigMgr*)m_pTimeSigMgr );
		}
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pTimeSigMgr->m_dwProducerOnlyFlags )
		{
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pTimeSigMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;
			m_pTimeSigMgr->m_pTimeline->OnDataChanged( (ITimeSigMgr*)m_pTimeSigMgr );
		}
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
// CTimeSigStrip::OnShowProperties

HRESULT CTimeSigStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pTimeSigMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pTimeSigMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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
		if( NULL == pPPM ) return E_OUTOFMEMORY;
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
	m_pTimeSigMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	RELEASE( pIPropSheet );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::OnRemoveFromPageManager

HRESULT CTimeSigStrip::OnRemoveFromPageManager( void )
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
// CTimeSigStrip::DisplayPropertySheet

HRESULT CTimeSigStrip::DisplayPropertySheet(IDMUSProdTimeline* pTimeline)
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
// CTimeSigStrip::DoDragDrop

BOOL CTimeSigStrip::DoDragDrop( void )
{
	// Drag drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTimeSigMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Query ourself for our IDropSource interface
	IDropSource* pIDropSource;
	if( FAILED ( QueryInterface(IID_IDropSource, (void**)&pIDropSource) ) )
	{
		return FALSE;
	}

	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a data object from the selected time signatures
	HRESULT hr = CreateDataObject( &m_pISourceDataObject, m_lStartDragPosition );
	if( SUCCEEDED( hr ) )
	{
		// We can always copy time signatures
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
		if( CanCut() == S_OK )
		{
			// If we can Cut(), allow the user to move the time signatures as well.
			dwOKDragEffects |= DROPEFFECT_MOVE;
		}

		// Do the Drag/Drop.
		hr = ::DoDragDrop( m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect );

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					m_pTimeSigMgr->DeleteMarked( UD_DRAGSELECT );
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

	m_pTimeSigMgr->UnMarkTimeSigs(UD_DRAGSELECT);
	m_pTimeSigMgr->SyncWithDirectMusic();

	if( dwEffect != DROPEFFECT_NONE )
	{
		if( m_nStripIsDragDropSource == 2 )
		{
			// Drag/drop target and source are the same TimeSig strip
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_UNDO_MOVE;
				m_pTimeSigMgr->OnDataChanged();
				
				// Notify the other strips of possible TimeSig change
				m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_nLastEdit = IDS_PASTE;
				m_pTimeSigMgr->OnDataChanged();
				
				// Notify the other strips of possible TimeSig change
				m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
			}
		}
		else
		{
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_DELETE;
				m_pTimeSigMgr->OnDataChanged();
				
				// Notify the other strips of possible TimeSig change
				m_pTimeSigMgr->m_pTimeline->NotifyStripMgrs( GUID_TimeSignature, m_pTimeSigMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_pTimeSigMgr->UnselectAll();
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
// CTimeSigStrip::OnLButtonDown

HRESULT CTimeSigStrip::OnLButtonDown( WPARAM wParam, LONG lXPos)
{
	ASSERT( m_pTimeSigMgr != NULL );
	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );

	m_fSingleSelect = FALSE;

	// If we're already dragging, just return
	// TODO: Implement

	// Get Timeline length
	VARIANT var;
	m_pTimeSigMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pTimeSigMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
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
	m_pTimeSigMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// See if there is a time signature under the cursor.
	CTimeSigItem* pTimeSig = GetTimeSigFromPoint( lXPos );
	if( pTimeSig
	&&  !(pTimeSig->m_dwBits & UD_FAKE) )
	{
		// There is a TimeSig on this measure
		if( wParam & MK_CONTROL )
		{
			if( pTimeSig->m_fSelected )
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
					pTimeSig->SetSelectFlag( !pTimeSig->m_fSelected );
				}
			}
			else
			{
				pTimeSig->SetSelectFlag( TRUE );

				// Set fields to initiate drag/drop on next mousemove
				m_dwStartDragButton = (unsigned long)wParam;
				m_lStartDragPosition = lXPos;

				m_dwShiftSelectFromMeasure = pTimeSig->m_dwMeasure;
			}
		}
		else if( wParam & MK_SHIFT )
		{
			pTimeSig->SetSelectFlag( TRUE );
			SelectRange( pTimeSig );
		}
		else
		{
			if( pTimeSig->m_fSelected == FALSE )
			{
				m_pTimeSigMgr->UnselectAll();
				pTimeSig->SetSelectFlag( TRUE );
			}
			else
			{
				m_fSingleSelect = TRUE;
			}
			
			// Set fields to initiate drag/drop on next mousemove
			m_dwStartDragButton = (unsigned long)wParam;
			m_lStartDragPosition = lXPos;
			m_dwShiftSelectFromMeasure = pTimeSig->m_dwMeasure;
		}
	}
	else
	{
		// There is not a "real" TimeSig on this measure
		hr = S_OK;
		if( pTimeSig == NULL )
		{
			// Create a "fake" TimeSig
			MUSIC_TIME mtTime = 0;
			m_pTimeSigMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime );
			hr = m_pTimeSigMgr->CreateTimeSig( mtTime, pTimeSig );
		}

		if( SUCCEEDED ( hr ) )
		{
			if( wParam & MK_CONTROL )
			{
				pTimeSig->SetSelectFlag( !pTimeSig->m_fSelected );
				m_dwShiftSelectFromMeasure = pTimeSig->m_dwMeasure;
			}
			else if( wParam & MK_SHIFT )
			{
				pTimeSig->SetSelectFlag( TRUE );
				SelectRange( pTimeSig );
			}
			else
			{
				// Click on empty space deselects all
				m_pTimeSigMgr->UnselectAll();

				pTimeSig->SetSelectFlag( TRUE );
				m_dwShiftSelectFromMeasure = pTimeSig->m_dwMeasure;
			}
		}
	}

	// Redraw the strip and refresh the TimeSig property page
	m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	m_pTimeSigMgr->OnShowProperties();
	if( m_pTimeSigMgr->m_pPropPageMgr )
	{
		m_pTimeSigMgr->m_pPropPageMgr->RefreshData();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::OnRButtonDown

HRESULT CTimeSigStrip::OnRButtonDown( WPARAM wParam, LONG lXPos )
{
	ASSERT( m_pTimeSigMgr != NULL );
	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pTimeSigMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pTimeSigMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
	if( lClock >= lTimelineLength )
	{
		return S_OK;
	}

	UnselectGutterRange();

	// Get the item at the mouse click.
	CTimeSigItem* pTimeSig = GetTimeSigFromPoint( lXPos );
	
	if( pTimeSig == NULL )
	{
		// No TimeSig so create a "fake" one
		m_pTimeSigMgr->UnselectAll();
		MUSIC_TIME mtTime = 0;
		m_pTimeSigMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime );
		if( SUCCEEDED ( m_pTimeSigMgr->CreateTimeSig( mtTime, pTimeSig ) ) )
		{
			pTimeSig->SetSelectFlag( TRUE );
		}
	}
	else if( !(wParam & MK_CONTROL)
		 &&  !(wParam & MK_SHIFT) )
	{
		if( pTimeSig )
		{
			if( pTimeSig->m_fSelected == FALSE )
			{
				m_pTimeSigMgr->UnselectAll();
				pTimeSig->SetSelectFlag( TRUE );
			}
		}
	}

	m_pTimeSigMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	m_fShowTimeSigProps = TRUE;
	m_pTimeSigMgr->OnShowProperties(); 
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::UnselectGutterRange

void CTimeSigStrip::UnselectGutterRange( void )
{
	ASSERT( m_pTimeSigMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pTimeSigMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pTimeSigMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeSigStrip::SelectRange

HRESULT CTimeSigStrip::SelectRange( CTimeSigItem* pTimeSig )
{
	HRESULT hr = S_OK;

	// Find measure for TimeSig to start from and TimeSig to end at
	DWORD dwEarlyMeasure = m_dwShiftSelectFromMeasure;
	DWORD dwLateMeasure = pTimeSig->m_dwMeasure;

	// If the order is backwards, swap
	if( dwLateMeasure < dwEarlyMeasure )
	{
		DWORD dwTemp = dwEarlyMeasure;
		dwEarlyMeasure = dwLateMeasure;
		dwLateMeasure = dwTemp;
	}

	// Add empty empty TimeSigs to fill in gaps
	CTimeSigItem* pTimeSigItem = NULL;
	DWORD dwCurrentMeasure = dwEarlyMeasure;
	while( dwCurrentMeasure <= dwLateMeasure )
	{
		if( GetTimeSigFromMeasureBeat(dwCurrentMeasure, 0) == NULL )
		{
			MUSIC_TIME mtPosition = 0;
			m_pTimeSigMgr->m_pTimeline->MeasureBeatToClocks( 
				m_pTimeSigMgr->m_dwGroupBits, 0, dwCurrentMeasure, 0, &mtPosition );
			pTimeSigItem = NULL;
			hr = m_pTimeSigMgr->CreateTimeSig( mtPosition, pTimeSigItem );
		}
		dwCurrentMeasure++;
	}

	// Unselect all TimeSigs in the strip
	m_pTimeSigMgr->UnselectAll();

	// Select all TimeSigs in the range (can't simply select TimeSigs in the
	// above loop, because there may be multiple TimeSigs at the same measure)
	POSITION pos = m_pTimeSigMgr->m_lstTimeSigs.GetHeadPosition();
	while( pos )
	{
		pTimeSigItem = m_pTimeSigMgr->m_lstTimeSigs.GetNext( pos );

		if( pTimeSigItem->m_dwMeasure >= dwEarlyMeasure
		&&	pTimeSigItem->m_dwMeasure <= dwLateMeasure )
		{
			pTimeSigItem->SetSelectFlag( TRUE );
		}
	}

	return hr;
}
