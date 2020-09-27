// PersRefStrip.cpp : Implementation of CPersRefStrip
#include "stdafx.h"
#include "PersRefIO.h"
#include "ChordMapRefStripMgr.h"
#include "PersRefMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "ChordMapDesigner.h"
#include <RiffStrm.h>
#include "MusicTimeConverter.h"
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
// CPersRefStrip constructor/destructor

CPersRefStrip::CPersRefStrip( CPersRefMgr* pPersRefMgr )
{
	ASSERT( pPersRefMgr );
	if ( pPersRefMgr == NULL )
	{
		return;
	}

	m_pPersRefMgr = pPersRefMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pPersRefMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;
	m_bGutterSelected = FALSE;

	m_cfPersRefList = 0;
	m_cfPersonality = 0;

	m_bSelecting = FALSE;
	m_pISourceDataObject = NULL;
	m_pITargetDataObject = NULL;
	m_nStripIsDragDropSource = 0;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_nLastEdit = 0;
	m_fShowPersRefProps = FALSE;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;
	m_fInRightClickMenu = FALSE;
}

CPersRefStrip::~CPersRefStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pPersRefMgr = NULL;
	}

	RELEASE( m_pISourceDataObject );
	RELEASE( m_pITargetDataObject );
	RELEASE( m_pPropPageMgr );
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::QueryInterface

STDMETHODIMP CPersRefStrip::QueryInterface( REFIID riid, LPVOID *ppv )
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
// CPersRefStrip::AddRef

STDMETHODIMP_(ULONG) CPersRefStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Release

STDMETHODIMP_(ULONG) CPersRefStrip::Release(void)
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
// CPersRefStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Draw

HRESULT	STDMETHODCALLTYPE CPersRefStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDC	dc;

	BOOL fUseGutterSelectRange = FALSE;
	if( m_bGutterSelected
	&&  m_lGutterBeginSelect != m_lGutterEndSelect )
	{
		fUseGutterSelectRange = TRUE;
	}

	// Get a pointer to the Timeline
	if( m_pPersRefMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pPersRefMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pPersRefMgr->m_dwGroupBits, 0, lXOffset );
		// Attach to our device context
		if( dc.Attach(hDC) )
		{
			CRect	rectClip, rectHighlight;
			long	lStartTime, lPosition = 0;
			CSize	csize;

			// locals for ghosting
			long lFirstVisibleMeasure, lPartialVisibleMeasure;
			long lClocks;
			RECT rectGhost;
			CPersRefItem* pGhostPerson = 0;

			dc.GetClipBox( &rectClip );
			//rectHighlight = rectClip;
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;
			rectGhost.top = rectGhost.left = rectGhost.right = 0;
			rectGhost.bottom = STRIP_HEIGHT;
			bool bGhostPersonCovered = false;

			// find first visible measure for ghosting personalities
			m_pPersRefMgr->m_pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasure(lPartialVisibleMeasure, m_pPersRefMgr->m_pTimeline, m_pPersRefMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasure(lPartialVisibleMeasure, 0, m_pPersRefMgr->m_pTimeline, m_pPersRefMgr->m_dwGroupBits);
			if(cmtNearestMeasure.Time() < cmtFirstVisible.Time())
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure + 1;
			}
			else
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure;
			}

			// find latest personality before first visible measure
			POSITION pos = m_pPersRefMgr->m_lstPersRefs.GetHeadPosition();
			while(pos)
			{
				CPersRefItem* pPersRefItem = m_pPersRefMgr->m_lstPersRefs.GetNext( pos );
				if(pPersRefItem->m_dwMeasure < (unsigned)lFirstVisibleMeasure
					&& !pPersRefItem->m_PersListInfo.strName.IsEmpty())
				{
					pGhostPerson = pPersRefItem;
				}
				else
				{
					break;
				}
			}
			if(pGhostPerson)
			{
				// get rect of pers ref, will have to truncate if it covers part of real pers ref
				m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition(m_pPersRefMgr->m_dwGroupBits, 0, lFirstVisibleMeasure,
													0, &lPosition);
				lPosition++;
				CSize csize;
				csize = dc.GetTextExtent(pGhostPerson->m_PersListInfo.strName);
				rectGhost.left = lPosition - lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
			}

			// we'll draw ghost pers ref after checking whether real pers ref covers it


			m_pPersRefMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );

			pos = m_pPersRefMgr->m_lstPersRefs.GetHeadPosition();
			while( pos )
			{
				CPersRefItem* pPersRefItem = m_pPersRefMgr->m_lstPersRefs.GetNext( pos );

				if(pPersRefItem->m_dwMeasure == (unsigned)lFirstVisibleMeasure)
				{
					// real pers ref covers ghost
					bGhostPersonCovered = true;
				}

				m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0, pPersRefItem->m_dwMeasure, pPersRefItem->m_bBeat, &lPosition );
				csize = dc.GetTextExtent( pPersRefItem->m_PersListInfo.strName );

				rectHighlight.left = lPosition - lXOffset;

				if(pGhostPerson)
				{
					// truncate ghost person's text so it doesn't cover up real style's text
					long lTruePos = lPosition - lXOffset;
					if(lTruePos > rectGhost.left && lTruePos < rectGhost.right)
					{
						rectGhost.right = lTruePos;
					}
				}

				if (pos) 
				{
					CPersRefItem* pTempPersRef = m_pPersRefMgr->m_lstPersRefs.GetAt( pos );
					m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0, pTempPersRef->m_dwMeasure, pTempPersRef->m_bBeat, &(rectHighlight.right) );
					CSize csize;
					csize = dc.GetTextExtent(pPersRefItem->m_PersListInfo.strName);
					if( (rectHighlight.left + csize.cx) > rectHighlight.right)
					{
						dc.DrawText(pPersRefItem->m_PersListInfo.strName, rectHighlight, (DT_LEFT | DT_NOPREFIX));
					}
					else
					{
						dc.TextOut( lPosition - lXOffset + 1, 0, pPersRefItem->m_PersListInfo.strName, strlen(pPersRefItem->m_PersListInfo.strName) );
					}
				}
				else
				{
					dc.TextOut( lPosition - lXOffset + 1, 0, pPersRefItem->m_PersListInfo.strName, strlen(pPersRefItem->m_PersListInfo.strName) );
				}

				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}
			}

			// make sure the first selected PersRef is shown in its entirety
			CPersRefItem* pPersRefItem = m_pPersRefMgr->FirstSelectedPersRef();
			if( pPersRefItem )
			{
				pos = m_pPersRefMgr->m_lstPersRefs.Find( pPersRefItem, NULL );
				// Leave rectHighlight alone
				//rectHighlight = rectClip;
				while( pos )
				{
					if (pPersRefItem->m_fSelected)
					{
						long lMeasureBeginPosition = 0;
						long lMeasureEndPosition = 0;
						m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0,
														  pPersRefItem->m_dwMeasure, 0, &lMeasureBeginPosition );
						m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0,
														 (pPersRefItem->m_dwMeasure + 1), 0, &lMeasureEndPosition );
						long lMeasureLength = lMeasureEndPosition - lMeasureBeginPosition;
						lPosition = lMeasureBeginPosition;

						// find extent of text
						csize = dc.GetTextExtent( pPersRefItem->m_PersListInfo.strName );
						if( csize.cx < lMeasureLength )
						{
							csize.cx = lMeasureLength;
						}
						long lExtent = lPosition + csize.cx;

						// truncate if a selected PersRef covers part of text
						POSITION pos2 = pos;
						CPersRefItem* pTempPersRef;
						bool fDone = false;
						while( pos2 && !fDone)
						{
							m_pPersRefMgr->m_lstPersRefs.GetNext( pos2 );
							if (!pos2) break;
							pTempPersRef = m_pPersRefMgr->m_lstPersRefs.GetAt( pos2 );
							if( pTempPersRef->m_fSelected )
							{	
								long lRPos;
								m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0, pTempPersRef->m_dwMeasure, pTempPersRef->m_bBeat, &lRPos );
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
							dc.DrawText( pPersRefItem->m_PersListInfo.strName, rectHighlight, (DT_LEFT | DT_NOPREFIX) );
							GrayOutRect( dc.m_hDC, rectHighlight );
						}
					}
					m_pPersRefMgr->m_lstPersRefs.GetNext( pos );
					if (pos) pPersRefItem = m_pPersRefMgr->m_lstPersRefs.GetAt( pos );
				}
			}

			// DRAW HATCH MARKS OVER MEASURES 2 THRU END
			{
				CBrush brushHatch;
				RECT rectHatch;
				long lStartClock;

				// Determine rectangle
				rectHatch.top = rectClip.top;
				rectHatch.bottom = rectClip.bottom;
				rectHatch.right = rectClip.right;
				m_pPersRefMgr->m_pTimeline->MeasureBeatToClocks( m_pPersRefMgr->m_dwGroupBits, 0, 1, 0, &lStartClock );
				m_pPersRefMgr->m_pTimeline->ClocksToPosition( lStartClock, &rectHatch.left );
				rectHatch.left = max( rectHatch.left - lXOffset, rectClip.left );
/*
				// truncate hatch rect so that it doesn't cover ghosted pers ref
				if(!bGhostPersonCovered && pGhostPerson)
				{
					// get rect of pers ref, will have to truncate if it covers part of real pers ref
					m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition(m_pPersRefMgr->m_dwGroupBits, 0, lFirstVisibleMeasure,
														0, &lPosition);
					lPosition++;
					CSize csize;
					csize = dc.GetTextExtent(pGhostPerson->m_PersListInfo.strName);
					rectGhost.left = lPosition - lXOffset;
					rectGhost.right = rectGhost.left + csize.cx;

					if(rectHatch.left < rectGhost.right)
					{
						rectHatch.left = rectGhost.right;
					}
				}
*/				
				// Draw it
				if( brushHatch.CreateHatchBrush( HS_DIAGCROSS, dc.GetNearestColor(RGB(50,50,50)) ) )
				{
					int nOldBackgroundMode = dc.SetBkMode( TRANSPARENT );
					CBrush* pOldBrush = dc.SelectObject( &brushHatch );
					COLORREF cr = dc.SetBkColor( 0 );
					dc.PatBlt( rectHatch.left, rectHatch.top, rectHatch.right - rectHatch.left, rectHatch.bottom - rectHatch.top, PATINVERT );
					dc.SetBkMode( nOldBackgroundMode );
					dc.SelectObject( pOldBrush );
					brushHatch.DeleteObject();
					dc.SetBkColor(cr);
				}
			}

			// now draw ghost pers ref
			if(!bGhostPersonCovered && pGhostPerson)
			{
				// further truncate ghost rect by its real rect if close enough
				m_pPersRefMgr->m_pTimeline->MeasureBeatToPosition( m_pPersRefMgr->m_dwGroupBits, 0,
												  pGhostPerson->m_dwMeasure, 0, &lPosition );

				rectHighlight.left = lPosition-lXOffset;
				CSize csize = dc.GetTextExtent(pGhostPerson->m_PersListInfo.strName);
				rectHighlight.right = rectHighlight.left + csize.cx;
				m_pPersRefMgr->m_pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
				m_pPersRefMgr->m_pTimeline->ClocksToPosition(lClocks, &lPosition);
				if(!(rectHighlight.right > (lPosition-lXOffset)))
				{
					int nOldBackgroundMode = dc.SetBkMode(OPAQUE);	
					COLORREF cr = dc.SetTextColor(RGB(168,168,168));
					dc.DrawText(pGhostPerson->m_PersListInfo.strName, &rectGhost, (DT_LEFT | DT_NOPREFIX));
					dc.SetTextColor(cr);
					dc.SetBkMode( nOldBackgroundMode );
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

				m_pPersRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pPersRefMgr->m_dwGroupBits, 0, lBeginSelect, &lMeasure, &lBeat );
				m_pPersRefMgr->m_pTimeline->MeasureBeatToClocks( m_pPersRefMgr->m_dwGroupBits, 0, lMeasure, 0, &lClocks );
				m_pPersRefMgr->m_pTimeline->ClocksToPosition( lClocks, &(rectHighlight.left));

				m_pPersRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pPersRefMgr->m_dwGroupBits, 0, lEndSelect, &lMeasure, &lBeat );
				m_pPersRefMgr->m_pTimeline->MeasureBeatToClocks( m_pPersRefMgr->m_dwGroupBits, 0, (lMeasure + 1), 0, &lClocks );
				m_pPersRefMgr->m_pTimeline->ClocksToPosition( (lClocks - 1), &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;

				// Invert it.
				GrayOutRect( dc.m_hDC, rectHighlight );
			}

			dc.Detach();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CPersRefStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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

	case SP_CURSOR_HANDLE:
		pvar->vt = VT_I4;
		V_I4(pvar) = (int)m_hCursor;
		break;

	case SP_NAME:
		{
			BSTR bstr;

			CString strName;
			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( _Module.GetModuleInstance() );
			strName.LoadString( IDS_STRIP_FBAR_NAME );
			AfxSetResourceHandle( hInstance );

			CString str = GetName(m_pPersRefMgr->m_dwGroupBits, strName);

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
		if( m_pPersRefMgr )
		{
			m_pPersRefMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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
// CPersRefStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CPersRefStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
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
			if( m_pPersRefMgr->UnselectAll() )
			{
				m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			break;
		}
		if( m_bGutterSelected )
		{
			m_pPersRefMgr->UnselectAll();
			m_pPersRefMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
			m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}
		else
		{
			if( m_pPersRefMgr->UnselectAll() )
			{
				m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
		}

		// Update the property page
		if( m_pPersRefMgr->m_pPropPageMgr != NULL )
		{
			m_pPersRefMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pPersRefMgr->UnselectAll();
			m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}

		if( m_bGutterSelected )
		{
			m_pPersRefMgr->UnselectAll();
			m_pPersRefMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pPersRefMgr->UnselectAll();
		}
		m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pPersRefMgr->m_pPropPageMgr != NULL )
		{
			m_pPersRefMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::GetPersRefFromMeasure

CPersRefItem *CPersRefStrip::GetPersRefFromMeasure( DWORD dwMeasure )
{
	POSITION pos = m_pPersRefMgr->m_lstPersRefs.GetHeadPosition();
	while( pos )
	{
		CPersRefItem* pPersRefItem = m_pPersRefMgr->m_lstPersRefs.GetNext( pos );
		if ( pPersRefItem->m_dwMeasure == dwMeasure )
		{
			return pPersRefItem;
		}
		else if ( pPersRefItem->m_dwMeasure > dwMeasure )
		{
			break;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::GetPersRefFromPoint

CPersRefItem *CPersRefStrip::GetPersRefFromPoint( long lPos )
{
	CPersRefItem*		pPersRefReturn = NULL;

	if( m_pPersRefMgr->m_pTimeline )
	{
		CPersRefItem* pPersRefItem = NULL;
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->PositionToMeasureBeat( m_pPersRefMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pPersRefReturn = GetPersRefFromMeasure( lMeasure );
		}
	}
	return pPersRefReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CPersRefStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	CPersRefItem* pPersRef = NULL;

	// Process the window message
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	// Compute max position (end of measure 1)
	long lMaxClock;
	long lMaxPos;
	m_pPersRefMgr->m_pTimeline->MeasureBeatToClocks( m_pPersRefMgr->m_dwGroupBits, 0, 1, 0, &lMaxClock );
	m_pPersRefMgr->m_pTimeline->ClocksToPosition( lMaxClock, &lMaxPos );

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		if( lXPos < lMaxPos )
		{
			m_fShowPersRefProps = TRUE;
			hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		}
		break;

	case WM_RBUTTONDOWN:
		if( lXPos < lMaxPos )
		{
			m_fShowPersRefProps = TRUE;
			hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		}
		break;

	case WM_RBUTTONUP:
		if( lXPos < lMaxPos )
		{
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

			// Save the position of the click so we know where to insert a PersRef. if Insert is selected.
			m_lXPos = lXPos;
			m_fInRightClickMenu = TRUE;
			m_pPersRefMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
			m_fInRightClickMenu = FALSE;
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
			hr = ShowPropertySheet(m_pPersRefMgr->m_pTimeline);
			if (m_fShowPersRefProps)
			{
				// Change to the personality reference property page
				m_pPersRefMgr->OnShowProperties();
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
			hr = Paste();
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
		if( lXPos < lMaxPos )
		{
			m_lXPos = lXPos;
			UnselectGutterRange();
		}
		break;

	case WM_SETCURSOR:
		if( lXPos < lMaxPos )
		{
			m_hCursor = ::LoadCursor( NULL, IDC_ARROW );
		}
		else
		{
			m_hCursor = ::LoadCursor( NULL, IDC_NO );
		}
		break;

	case WM_CREATE:
		m_cfPersRefList = RegisterClipboardFormat( CF_PERSREFLIST );
		m_cfPersonality = RegisterClipboardFormat( CF_PERSONALITY );

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );

		// Fix measure/beat info for any personalities already in the strip
		m_pPersRefMgr->FixPersRefListMeasureBeat();
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::FBDraw

HRESULT CPersRefStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::FBOnWMMessage

HRESULT CPersRefStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fShowPersRefProps = FALSE;
		OnShowProperties();
		break;
	case WM_RBUTTONUP:
		m_fShowPersRefProps = FALSE;
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

		if( m_pPersRefMgr->m_pTimeline )
		{
			m_fInRightClickMenu = TRUE;
			m_pPersRefMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
			m_fInRightClickMenu = FALSE;
		}
		break;
	case WM_SETCURSOR:
		m_hCursor = ::LoadCursor( NULL, IDC_ARROW );
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Cut

HRESULT CPersRefStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
// CPersRefStrip::Copy

HRESULT CPersRefStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfPersRefList == 0 )
	{
		m_cfPersRefList = RegisterClipboardFormat( CF_PERSREFLIST );
		if( m_cfPersRefList == 0 )
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

	// Save the commands into the stream.
	m_pPersRefMgr->MarkSelectedPersRefs(UD_DRAGSELECT);
	CPersRefItem* pPersRefAtDragPoint = m_pPersRefMgr->FirstSelectedPersRef();
	hr = m_pPersRefMgr->SaveSelectedPersRefs( pStreamCopy, pPersRefAtDragPoint, TRUE );
	if( FAILED( hr ))
	{
		RELEASE( pStreamCopy );
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfPersRefList, pStreamCopy );
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
		hr = m_pPersRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pPersRefMgr->GetBoundariesOfSelectedPersRefs( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfPersRefList, pStreamCopy );

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
		RELEASE( m_pPersRefMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pPersRefMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		//m_pPersRefMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::PasteAt

HRESULT CPersRefStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtTime, BOOL fDropNotEditPaste, BOOL &fChanged)
{
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure everything in other strip is deselected first.
	UnselectGutterRange();

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfPersRefList ) == S_OK )
	{
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfPersRefList, &pIStream)))
		{
			// Check for RIFF format
			if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				goto Leave;
			}

			hr = m_pPersRefMgr->LoadPersRefList( pIRiffStream, NULL, true, mtTime, fChanged );
		}
	}
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfPersonality ) == S_OK )
	{
		IDMUSProdNode* pIDocRootNode;
		IDataObject* pIDataObject;

		hr = pITimelineDataObject->Export( &pIDataObject );
		if( SUCCEEDED ( hr ) )
		{
			hr = m_pPersRefMgr->m_pDMProdFramework->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				CPersRefItem* pItem = new CPersRefItem( m_pPersRefMgr );
				if( pItem )
				{
					hr = m_pPersRefMgr->SetPersReference( pIDocRootNode, pItem );
					if( SUCCEEDED ( hr ) )
					{
						m_pPersRefMgr->InsertByAscendingTime( pItem, TRUE );
						pItem->m_fSelected = TRUE;
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
// CPersRefStrip::Paste

HRESULT CPersRefStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste(pITimelineDataObject);
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfPersRefList == 0 )
	{
		m_cfPersRefList = RegisterClipboardFormat( CF_PERSREFLIST );
		if( m_cfPersRefList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfPersonality == 0 )
	{
		m_cfPersonality = RegisterClipboardFormat( CF_PERSONALITY );
		if( m_cfPersonality == 0 )
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
		hr = m_pPersRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pPersRefMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		RELEASE( pITimelineDataObject );
		return E_FAIL;
	}

	BOOL fChanged = FALSE;

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		long lStart, lEnd;
		if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
		{
			fChanged = m_pPersRefMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}

	// Get the time to paste at
	MUSIC_TIME mtTime;
	if( FAILED( m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
	{
		RELEASE( pITimelineDataObject );
		return E_FAIL;
	}

	// Now, do the paste operation
	hr = PasteAt(pITimelineDataObject, mtTime, FALSE, fChanged);
	RELEASE( pITimelineDataObject );

	// If successful and something changed, redraw our strip
	if( SUCCEEDED(hr) && fChanged )
	{
		// Set the last edit type
		m_nLastEdit = IDS_PASTE;

		// Update our hosting editor.
		m_pPersRefMgr->OnDataChanged();

		// Redraw our strip
		m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pPersRefMgr->SyncWithDirectMusic();

		m_pPersRefMgr->OnShowProperties();
		if( m_pPersRefMgr->m_pPropPageMgr )
		{
			m_pPersRefMgr->m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Insert

HRESULT CPersRefStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Unselect all items in this strip 
	m_pPersRefMgr->UnselectAll();
	
	// Unselect items in other strips
	UnselectGutterRange();

	// BUGBUG: Need to fix this.  Should use m_lXPos if it's valid, otherwise use the time cursor.
	HRESULT hr = S_OK;
	long	lMeasure, lBeat;
	CPersRefItem* pPersRef = NULL;
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	hr = m_pPersRefMgr->m_pTimeline->PositionToMeasureBeat( m_pPersRefMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED( hr ) );

	// Align on measure boundary
	lBeat = 0;

	pPersRef = new CPersRefItem( m_pPersRefMgr );
	if(pPersRef == NULL)
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pPersRef->m_PersListInfo.strProjectName = "Empty";
		pPersRef->m_PersListInfo.strName = "Empty";
		pPersRef->m_PersListInfo.strDescriptor = "Empty";
		pPersRef->m_dwMeasure = lMeasure;
		pPersRef->m_bBeat = (BYTE)lBeat;
		m_pPersRefMgr->m_pTimeline->MeasureBeatToClocks( m_pPersRefMgr->m_dwGroupBits, 0, lMeasure, lBeat, &pPersRef->m_mtTime );
		pPersRef->m_fSelected = TRUE;
		m_pPersRefMgr->InsertByAscendingTime(pPersRef, FALSE);
		m_pPersRefMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
		ShowPropertySheet(m_pPersRefMgr->m_pTimeline);
		m_pPersRefMgr->OnShowProperties();
			
		if( m_pPersRefMgr->m_pPropPageMgr )
		{
			m_pPersRefMgr->m_pPropPageMgr->RefreshData();
		}
		m_nLastEdit = IDS_INSERT;
		m_pPersRefMgr->OnDataChanged();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Delete

HRESULT CPersRefStrip::Delete( void )
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

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pPersRefMgr->DeleteSelectedPersRefs();

	m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pPersRefMgr->m_pPropPageMgr )
	{
		m_pPersRefMgr->m_pPropPageMgr->RefreshData();
	}
	m_pPersRefMgr->SyncWithDirectMusic();

	m_nLastEdit = IDS_DELETE;
	m_pPersRefMgr->OnDataChanged();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::SelectAll

HRESULT CPersRefStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pPersRefMgr->SelectAll();

	m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CanCut

HRESULT CPersRefStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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
// CPersRefStrip::CanCopy

HRESULT CPersRefStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pPersRefMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pPersRefMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CanPaste

HRESULT CPersRefStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPersRefMgr != NULL );
	if( m_pPersRefMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the formats haven't been registered yet, do it now.
	if( m_cfPersRefList == 0 )
	{
		m_cfPersRefList = RegisterClipboardFormat( CF_PERSREFLIST );
		if( m_cfPersRefList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfPersonality == 0 )
	{
		m_cfPersonality = RegisterClipboardFormat( CF_PERSONALITY );
		if( m_cfPersonality == 0 )
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	HRESULT	hr = E_FAIL;
	if( pITimelineDataObject != NULL )
	{
		if( (pITimelineDataObject->IsClipFormatAvailable( m_cfPersRefList ) == S_OK)
		||  (pITimelineDataObject->IsClipFormatAvailable( m_cfPersonality ) == S_OK) )
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
			if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					if( (pITimelineDataObject->IsClipFormatAvailable( m_cfPersRefList ) == S_OK)
					||  (pITimelineDataObject->IsClipFormatAvailable( m_cfPersonality ) == S_OK) )
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
// CPersRefStrip::CanInsert

HRESULT CPersRefStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPersRefMgr != NULL );
	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pPersRefMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Get clock at XPos
	long lClock;
	m_pPersRefMgr->m_pTimeline->PositionToClocks( m_lXPos, &lClock );


	// Make sure XPos is within strip
	if( lClock > 0 
	&&  lClock < lTimelineLength )
	{
		// User clicked within boundaries of strip
		long lMeasure;
		long lBeat;

		if( SUCCEEDED ( m_pPersRefMgr->m_pTimeline->PositionToMeasureBeat( m_pPersRefMgr->m_dwGroupBits,
																		   0,
																		   m_lXPos,
																		   &lMeasure,
																		   &lBeat ) ) )
		{
			CPropPersRef* pPersRef = GetPersRefFromPoint( m_lXPos );
			if( pPersRef == NULL )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CanDelete

HRESULT CPersRefStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pPersRefMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CanSelectAll

HRESULT CPersRefStrip::CanSelectAll( void )
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

	if( m_pPersRefMgr->m_lstPersRefs.IsEmpty() == FALSE )
	{
		return S_OK;
	}

	return S_FALSE;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::QueryContinueDrag

HRESULT CPersRefStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// CPersRefStrip::GiveFeedback

HRESULT CPersRefStrip::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pDragImage )
	{
		CPoint pt;

		GetCursorPos( &pt );

		// Move the drag image
		m_pDragImage->DragMove( pt );
	}

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CreateDragImage

CImageList* CPersRefStrip::CreateDragImage()
{
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::CreateDataObject

HRESULT	CPersRefStrip::CreateDataObject(IDataObject** ppIDataObject, long position)
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

	// Save Selected PersRefs into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pPersRefMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		CPersRefItem* pPersRefAtDragPoint = GetPersRefFromPoint( position );

		// mark the personality references as being dragged: this used later for deleting personality references in drag move
		m_pPersRefMgr->MarkSelectedPersRefs(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pPersRefMgr->SaveSelectedPersRefs( pIStream, pPersRefAtDragPoint, TRUE ) ) )
		{
			// Place CF_PERS_REFLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfPersRefList, pIStream ) ) )
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
// CPersRefStrip::DragEnter

HRESULT CPersRefStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		CWnd *pWnd = GetTimelineCWnd();
		if( pWnd )
		{
			// Show the feedback image
			m_pDragImage->DragEnter( pWnd->GetDesktopWindow(), point );
		}
	}

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::DragOver

HRESULT CPersRefStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( CanPasteFromData( m_pITargetDataObject ) == S_OK )
	{
		BOOL fCF_PERSONALITY = FALSE;

		// Does m_pITargetDataObject contain format CF_BAND?
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject )
		{
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfPersonality ) ) )
			{
				fCF_PERSONALITY = TRUE;
			}
			pDataObject->Release();
		}

		// Can only copy CF_PERSONALITY data!
		if( fCF_PERSONALITY )
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

	if( m_pDragImage )
	{
		// Show the feedback image
		m_pDragImage->DragShowNolock( TRUE );
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
// CPersRefStrip::DragLeave

HRESULT CPersRefStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Release IDataObject
	RELEASE( m_pITargetDataObject );

	if( m_pDragImage )
	{
		CWnd *pWnd = GetTimelineCWnd();
		if( pWnd )
		{
			// Hide the feedback image
			m_pDragImage->DragLeave( pWnd->GetDesktopWindow() );
		}
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::Drop

HRESULT CPersRefStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	if( m_pDragImage )
	{
		CWnd *pWnd = GetTimelineCWnd();
		if( pWnd )
		{
			// Hide the feedback image
			m_pDragImage->DragLeave( pWnd->GetDesktopWindow() );
		}
	}

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
		// BUGBUG: Error messages?
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED( m_pPersRefMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
				{
					BOOL fChanged = FALSE;
					hr = PasteAt( pITimelineDataObject, mtTime, true, fChanged );
					if( SUCCEEDED ( hr ) )
					{
						*pdwEffect = m_dwOverDragEffect;

						// If we pasted anything
						if( fChanged )
						{
							if( m_nStripIsDragDropSource )
							{
								// Drag/drop Target and Source are the same PersRef strip
								m_nStripIsDragDropSource = 2;
							}
							else
							{
								// Set the last edit type
								m_nLastEdit = IDS_PASTE;

								// Update our hosting editor.
								m_pPersRefMgr->OnDataChanged();

								// Redraw our strip
								m_pPersRefMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

								// Update the property page
								if( m_pPersRefMgr->m_pPropPageMgr != NULL )
								{
									m_pPersRefMgr->m_pPropPageMgr->RefreshData();
								}

								m_pPersRefMgr->SyncWithDirectMusic();
							}
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
// CPersRefStrip::GetTimelineCWnd

CWnd* CPersRefStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pPersRefMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pPersRefMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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
// CPersRefStrip::CanPasteFromData

HRESULT CPersRefStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfPersRefList ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfPersonality ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::GetData

HRESULT CPersRefStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	// Check which property page is requesting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		// Copy our groupbits to the location pointed to by ppData
		PPGTrackParams *pPPGTrackParams = static_cast<PPGTrackParams *>(*ppData);
		pPPGTrackParams->dwGroupBits = m_pPersRefMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pPersRefMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pPersRefMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	default:
		ASSERT(FALSE);
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::SetData

HRESULT CPersRefStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Check which property page is setting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

		// Update our group bits setting, if necessary
		if( pPPGTrackParams->dwGroupBits != m_pPersRefMgr->m_dwGroupBits )
		{
			m_pPersRefMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_TRACK_GROUP;
			m_pPersRefMgr->m_pTimeline->OnDataChanged( (IPersRefMgr*)m_pPersRefMgr );
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pPersRefMgr->m_dwTrackExtrasFlags )
		{
			m_pPersRefMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pPersRefMgr->m_pTimeline->OnDataChanged( (IPersRefMgr*)m_pPersRefMgr );
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pPersRefMgr->m_dwProducerOnlyFlags )
		{
			m_pPersRefMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pPersRefMgr->m_pTimeline->OnDataChanged( (IPersRefMgr*)m_pPersRefMgr );
		}
		break;
	}
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::OnShowProperties

HRESULT CPersRefStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pPersRefMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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
		pIPropSheet->Release();
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

	// Set the property page to refer to the Group bits property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pPersRefMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::OnRemoveFromPageManager

HRESULT CPersRefStrip::OnRemoveFromPageManager( void )
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
// CPersRefStrip::ShowPropertySheet

HRESULT CPersRefStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
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
// CPersRefStrip::OnLButtonDown

HRESULT CPersRefStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	// If we're already dragging, just return
	if( m_pDragImage )
	{
		return S_OK;
	}

	// Get a reference to the timeline
	if( m_pPersRefMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pPersRefMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// See if there is a personality reference under the cursor.
	CPersRefItem* pPersRef = GetPersRefFromPoint( lXPos );
	if( pPersRef )
	{
		BOOL fDrop = FALSE; // TRUE if we're doing a drop

		// If it's already selected, start a Drag/Drop operation
		if(pPersRef->m_fSelected)
		{
			// Start drag/drop
			m_nStripIsDragDropSource = 1;
			fDrop = TRUE;

			// drag drop will capture mouse, so release it from timeline
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pPersRefMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

			IDropSource*	pIDropSource;
			DWORD			dwEffect = DROPEFFECT_NONE;
			// Query ourself for our IDropSource interface
			if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
			{
				// Create a data object from the selected personality references
				hr = CreateDataObject( &m_pISourceDataObject, lXPos );
				if(SUCCEEDED(hr))
				{
					// Create an image to use when dragging personality references
					m_pDragImage = CreateDragImage();
					if(m_pDragImage)
					{
						m_pDragImage->BeginDrag(0, CPoint(8,12));
					}

					// We can always copy personality references.
					DWORD dwOKDragEffects = DROPEFFECT_COPY;
					if(CanCut() == S_OK)
					{
						// If we can Cut(), allow the user to move the personality references as well.
						dwOKDragEffects |= DROPEFFECT_MOVE;
					}

					// Do the Drag/Drop.
					m_dwStartDragButton = (unsigned long)wParam;
					m_lStartDragPosition = lXPos;
					hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

					// Drag/Drop completed, clean up
					m_dwStartDragButton = 0;

					if(m_pDragImage)
					{
						m_pDragImage->EndDrag();
						delete m_pDragImage;
						m_pDragImage = NULL;
					}

					switch(hr)
					{
					case DRAGDROP_S_DROP:
						if(dwEffect & DROPEFFECT_MOVE)
						{
							m_pPersRefMgr->DeleteMarked(UD_DRAGSELECT);
							m_nLastEdit = IDS_DELETE;
						}
						if(dwEffect == DROPEFFECT_NONE)
						{
							fDrop = FALSE;
						}
						break;
					default:
						fDrop = FALSE;
						break;
					}
					hr = S_OK;
					RELEASE( m_pISourceDataObject );
				}
				RELEASE( pIDropSource );
				m_pPersRefMgr->UnMarkPersRefs(UD_DRAGSELECT);
				m_pPersRefMgr->SyncWithDirectMusic();
			}
			else
			{
				hr = E_FAIL;
				goto ON_ERR;
			}

			if(!fDrop)
			{
//				m_pPersRefMgr->m_lstPersRefs.UnselectAll();
				if(wParam & MK_CONTROL)
				{
					// unselect the personality reference
					if(pPersRef->m_fSelected)
						pPersRef->m_fSelected = FALSE;
				}

			}
			if( dwEffect != DROPEFFECT_NONE )
			{
				if( m_nStripIsDragDropSource == 2 )
				{
					// Drag/drop target and source are the same PersRef strip
					if( dwEffect == DROPEFFECT_MOVE )
					{
						m_nLastEdit = IDS_UNDO_MOVE;
						m_pPersRefMgr->OnDataChanged();
					}
					else
					{
						m_nLastEdit = IDS_PASTE;
						m_pPersRefMgr->OnDataChanged();
					}
				}
				else
				{
					if( dwEffect == DROPEFFECT_MOVE )
					{
						m_nLastEdit = IDS_DELETE;
						m_pPersRefMgr->OnDataChanged();
					}
					else
					{
						m_pPersRefMgr->UnselectAll();
					}
				}
			}

			// Done with drag/drop
			m_nStripIsDragDropSource = 0;
		}
		else						// other ops
		{
			// Since there's only one item and it's unselected, select it no matter what
			// combination of SHIFT and CTRL the user has pressed
			pPersRef->m_fSelected = TRUE;

		}

	}
	else if(wParam & MK_CONTROL)
	{
		// user wants to insert a personality reference
	}
	else
	{
		// click on empty space deselects all
		// first clear selections
		m_pPersRefMgr->UnselectAll();
	}

	// TODO: replace NULL by actual area invalidated (in strip coordinates)
	m_pPersRefMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
	m_pPersRefMgr->OnShowProperties();
	if( m_pPersRefMgr->m_pPropPageMgr )
	{
		m_pPersRefMgr->m_pPropPageMgr->RefreshData();
	}

ON_ERR:
	return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::OnRButtonDown

HRESULT CPersRefStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );

	UnselectGutterRange();

	// Get the item at the mouse click.
	CPersRefItem* pPersRef = GetPersRefFromPoint( lXPos );

	if( !(wParam & MK_CONTROL)
	&&  !(wParam & MK_SHIFT) )
	{
		if( pPersRef )
		{
			if( pPersRef->m_fSelected == FALSE )
			{
				// Doesn't make sense, there can only be one item
				//m_pPersRefMgr->UnselectAll();
				pPersRef->m_fSelected = TRUE;
			}
		}
	}

	m_pPersRefMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );

	m_pPersRefMgr->OnShowProperties(); 
	if( pPersRef && m_pPersRefMgr->m_pPropPageMgr )
	{
		m_pPersRefMgr->m_pPropPageMgr->RefreshData();
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefStrip::UnselectGutterRange

void CPersRefStrip::UnselectGutterRange( void )
{
	ASSERT( m_pPersRefMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pPersRefMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pPersRefMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}
