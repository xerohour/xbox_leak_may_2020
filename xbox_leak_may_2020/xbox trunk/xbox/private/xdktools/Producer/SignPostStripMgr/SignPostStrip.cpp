// SignPostStrip.cpp : Implementation of CSignPostStrip
#include "stdafx.h"
#include "SignPostStripMgr.h"
#include "SignPostMgr.h"
#include "SignPostIO.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "SignPostStripPPG.h"
#include <SegmentGuids.h>
#include "MusicTimeConverter.h"
#include "SegmentIO.h"
#include <dmusicp.h>
#include "GrayOutRect.h"

#define STRIP_HEIGHT 20

interface IPrivateSegment : public IUnknown
{
    public:
        virtual HRESULT STDMETHODCALLTYPE Compose( DWORD dwFlags, DWORD dwActivity ) = 0;
};

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
// CSignPostStrip

/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip IUnknown
STDMETHODIMP CSignPostStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(ppv);
    *ppv = NULL;
    if(IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdPropPageObject))
	{
        *ppv = (IUnknown *) (IDMUSProdPropPageObject *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdTimelineEdit))
	{
		*ppv = (IDMUSProdTimelineEdit *) this;
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
		return E_NOINTERFACE;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CSignPostStrip::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CSignPostStrip::Release(void)
{
	if(0L == --m_cRef)
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
// CSignPostStrip IDMUSProdStrip

CString FormatSignPostString(SignPostExt* pSignPost)
{
	CString strSignPost = "";
	if(pSignPost->dwSignPost & SP_CADENCE)
	{
		strSignPost += "^";
	}
	switch(pSignPost->dwSignPost & (SP_ROOT | SP_LETTER))
	{
		case SP_1 : strSignPost += "1"; break; 
		case SP_2 : strSignPost += "2"; break; 
		case SP_3 : strSignPost += "3"; break; 
		case SP_4 : strSignPost += "4"; break; 
		case SP_5 : strSignPost += "5"; break; 
		case SP_6 : strSignPost += "6"; break; 
		case SP_7 : strSignPost += "7"; break;
		case SP_A : strSignPost += "A"; break;
		case SP_B : strSignPost += "B"; break;
		case SP_C : strSignPost += "C"; break;
		case SP_D : strSignPost += "D"; break;
		case SP_E : strSignPost += "E"; break;
		case SP_F : strSignPost += "F"; break;
		default:	strSignPost  = "" ; break;
	}
	return strSignPost;
}

HRESULT	STDMETHODCALLTYPE CSignPostStrip::Draw(HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(sv);

	CRect			rectHighlight;
	long			lMeasureBeginPosition = 0;
	long			lMeasureEndPosition = 0;
	long			lBeginSelMeasure;
	long			lEndSelMeasure;

	ASSERT(m_pSignPostMgr != NULL && m_pStripMgr != NULL);
	if(m_pSignPostMgr == NULL || m_pStripMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	if( m_pSignPostMgr->m_pTimeline )
	{
		m_pSignPostMgr->m_pTimeline->DrawMusicLines(hDC, ML_DRAW_MEASURE_BEAT, m_pSignPostMgr->m_dwGroupBits, 0, lXOffset);

		CDC			dc;
		if(dc.Attach(hDC))
		{
			// draw the signposts

			CRect		rectClip;
			CString		strSignPost;
			SignPostExt	*pSignPost = m_pSignPostMgr->m_pSignPostList;
			SignPostExt*	pGhostSignPost = NULL;
			long		position;
			long		lMeasure;
			long		lFirstVisibleMeasure, lPartialVisibleMeasure;
			long		lClocks;
			RECT		rectGhost;
			CString		strGhost;

			rectGhost.top = 0;
			rectGhost.bottom = STRIP_HEIGHT;
			rectGhost.left = rectGhost.right = 0;

			::GetClipBox( hDC, &rectClip );
			rectHighlight = rectClip;

			// find first visible measure for ghosting chords
			bool bGhostSignPostCovered = false;	// indicates a real signpost exists where ghost would be
			IDMUSProdTimeline* pTimeline = m_pSignPostMgr->m_pTimeline;
			pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasure(lPartialVisibleMeasure, pTimeline, m_pSignPostMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasureBeat(lPartialVisibleMeasure, 0, pTimeline, 
														m_pSignPostMgr->m_dwGroupBits);
			if(cmtNearestMeasureBeat.Time() < cmtFirstVisible.Time())
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure + 1;
			}
			else
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure;
			}

			// find latest signpost before first visible measure
			ASSERT(m_pSignPostMgr->m_pMeasureArray != NULL);
			if(m_pSignPostMgr->m_pMeasureArray == NULL)
			{
				return E_UNEXPECTED;
			}
			for(int idx = lFirstVisibleMeasure - 1; idx >= 0; idx--)
			{
				if(idx >= m_pSignPostMgr->m_nMeasures)
					continue;
				if(m_pSignPostMgr->m_pMeasureArray[idx].pSignPost != NULL)
				{
					pGhostSignPost = m_pSignPostMgr->m_pMeasureArray[idx].pSignPost;
					pGhostSignPost->nMeasure = static_cast<short>(idx & 0xFFFF);
					break;
				}
			}

			if(pGhostSignPost)
			{
				// get rect of signpost, will have to truncate if ghost signpost covers part of 
				// real signpost
				m_pSignPostMgr->m_pTimeline->MeasureBeatToPosition(m_pSignPostMgr->m_dwGroupBits, 0,
																	lFirstVisibleMeasure, 0, &position);
				position++;
				strGhost = FormatSignPostString(pGhostSignPost);
				CSize csize;
				csize = dc.GetTextExtent(strGhost);
				rectGhost.left = position - lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
			}
			// we'll draw ghost signpost after checking whether real signpost covers it

			while(pSignPost)
			{
				m_pSignPostMgr->m_pTimeline->ClocksToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, pSignPost->lTime, &lMeasure, NULL);

				if(lMeasure == lFirstVisibleMeasure )
				{
					// real signpost covers ghost
					bGhostSignPostCovered = true;
				}

				m_pSignPostMgr->m_pTimeline->ClocksToPosition(pSignPost->lTime, &position);
				position++;

				if(pGhostSignPost)
				{
					// truncate ghost signpost's text so it doesn't cover up real signpost's text
					long lTruePos = position - lXOffset;
					if(lTruePos > rectGhost.left && lTruePos < rectGhost.right)
					{
						rectGhost.right = lTruePos;
					}
				}
				strSignPost = FormatSignPostString(pSignPost);
				::TextOut( hDC, position - lXOffset, 0, LPCSTR(strSignPost), strSignPost.GetLength() );

				pSignPost = pSignPost->pNext;
				if(position - lXOffset > rectClip.right)
				{
					break;
				}
			} // while(pSignPost)

			// now draw ghost signpost
			// but not if original signpost is still showing
			if(!bGhostSignPostCovered && pGhostSignPost)
			{
				long leftMargin = LeftMargin(pTimeline);
				pTimeline->MeasureBeatToPosition(m_pSignPostMgr->m_dwGroupBits, 0,
													pGhostSignPost->nMeasure, 0, &position);
						
				CSize extent = dc.GetTextExtent(strGhost);
				if(!((position + extent.cx) > leftMargin))
				{
					COLORREF cr = dc.SetTextColor(RGB(168, 168, 168));
					dc.DrawText(strGhost, &rectGhost, (DT_LEFT | DT_NOPREFIX));
					dc.SetTextColor(cr);
				}
			}

			// Highlight the selected measures.
			if(m_pSignPostMgr->m_bSelected)
			{
				ASSERT(m_pSignPostMgr->m_pMeasureArray != NULL);
				if(m_pSignPostMgr->m_pMeasureArray == NULL)
				{
					return E_UNEXPECTED;
				}

				short n;

				lBeginSelMeasure = -1;
				lEndSelMeasure = -1;
				for(n = 0; n < m_pSignPostMgr->m_nMeasures; n++)
				{
					if(m_pSignPostMgr->m_pMeasureArray[n].dwFlags & SPMI_SELECTED)
					{
						if(lBeginSelMeasure == -1)
						{
							lBeginSelMeasure = (long) n;
						}
						lEndSelMeasure = (long) n + 1;
					}
					else if(lBeginSelMeasure != -1)
					{
						ASSERT(lEndSelMeasure > lBeginSelMeasure);

						// Highlight the range which just ended.
						m_pSignPostMgr->m_pTimeline->MeasureBeatToPosition( m_pSignPostMgr->m_dwGroupBits, 0, lBeginSelMeasure, 0, &lMeasureBeginPosition);
						m_pSignPostMgr->m_pTimeline->MeasureBeatToPosition( m_pSignPostMgr->m_dwGroupBits, 0, lEndSelMeasure, 0, &lMeasureEndPosition);
						rectHighlight.left = lMeasureBeginPosition - lXOffset;
						rectHighlight.right = lMeasureEndPosition - lXOffset;
						GrayOutRect( hDC, &rectHighlight );
						lBeginSelMeasure = -1;
						lEndSelMeasure = -1;
					}
				}

				if(lBeginSelMeasure != -1)
				{
					// The last measure was highlighted.
					m_pSignPostMgr->m_pTimeline->MeasureBeatToPosition( m_pSignPostMgr->m_dwGroupBits, 0, lBeginSelMeasure, 0, &lMeasureBeginPosition);
					m_pSignPostMgr->m_pTimeline->MeasureBeatToPosition( m_pSignPostMgr->m_dwGroupBits, 0, lEndSelMeasure, 0, &lMeasureEndPosition);
					rectHighlight.left = lMeasureBeginPosition - lXOffset;
					rectHighlight.right = lMeasureEndPosition - lXOffset;
					GrayOutRect( hDC, &rectHighlight );
				}
			}
			dc.Detach();
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSignPostStrip::GetStripProperty(STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(NULL == pvar)
	{
		return E_POINTER;
	}

	switch(sp)
	{
	case SP_RESIZEABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_GUTTERSELECTABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_MINMAXABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_DEFAULTHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = STRIP_HEIGHT;
		break;
	case SP_NAME:
		{
			BSTR bstr;
			CString strName;
			strName.LoadString(IDS_FUNCTIONBAR_TEXT);

			CString str = GetName(m_pSignPostMgr->m_dwGroupBits, strName);

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
	case SP_BEGINSELECT:
		pvar->vt = VT_I4;
		V_I4(pvar) = m_lBeginSelect;
		break;
	case SP_ENDSELECT:
		pvar->vt = VT_I4;
		V_I4(pvar) = m_lEndSelect;
		break;

	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pSignPostMgr )
		{
			m_pSignPostMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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

HRESULT STDMETHODCALLTYPE CSignPostStrip::SetStripProperty(STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}
	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	switch(sp)
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		if(var.vt != VT_I4)
		{
			return E_FAIL;
		}
		if(sp == SP_BEGINSELECT)
		{
			m_lBeginSelect = V_I4(&var);
		}
		else
		{
			m_lEndSelect = V_I4(&var);
		}

		if(m_bSelecting)
		{
			break;
		}

		if(m_lBeginSelect == m_lEndSelect)
		{
			if(m_pSignPostMgr->m_bSelected)
			{
				m_pSignPostMgr->ClearSelected();
			}
			m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
			break;
		}
		if(m_fSelected)
		{
			m_pSignPostMgr->ClearSelected();
			m_pSignPostMgr->SelectSegment(m_lBeginSelect, m_lEndSelect);
		}
		else if(m_pSignPostMgr->m_bSelected)
		{
			m_pSignPostMgr->ClearSelected();
		}
		else
		{
			break;
		}
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

		// Update the property page
		if(m_pSignPostMgr->m_pIPageManager != NULL)
		{
			m_pSignPostMgr->m_pIPageManager->RefreshData();
		}
		break;
	case SP_GUTTERSELECT:
		m_fSelected = V_BOOL(&var);
		if(m_lBeginSelect == m_lEndSelect)
		{
			// Selection doesn't change.
			break;
		}
		if(m_fSelected)
		{
			ASSERT(!m_pSignPostMgr->m_bSelected);
			m_pSignPostMgr->SelectSegment(m_lBeginSelect, m_lEndSelect);
		}
		else
		{
			m_pSignPostMgr->ClearSelected();
		}
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSignPostStrip::OnWMMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT			hr = S_FALSE;

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	switch(nMsg)
	{
	case WM_LBUTTONDOWN:
		m_lMouseXPos = lXPos;
		m_lMouseYPos = lYPos;
		hr = OnLButtonDown( wParam, lXPos, lYPos );
		break;

	case WM_LBUTTONUP:
		m_lXPos = lXPos;
		hr = OnLButtonUp( wParam, lXPos, lYPos );
		break;

	case WM_RBUTTONDOWN:
		// Make sure everything on the timeline is deselected.
		UnselectGutterRange();
		break;

	case WM_RBUTTONUP:
		m_lXPos = lXPos;
		hr = OnRButtonUp( wParam, lXPos, lYPos );
		break;

	case WM_MOUSEMOVE:
		hr = OnMouseMove( wParam, lParam, lXPos, lYPos );
		break;

	case WM_SETFOCUS:
		m_pSignPostMgr->m_lShiftFromMeasure = 0;
		break;

	case WM_COMMAND:
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD(wParam);	// notification code 
		wID			= LOWORD(wParam);	// item, control, or accelerator identifier 
		switch(wID)
		{
		case ID_VIEW_PROPERTIES:
			// Get a pointer to the property sheet and show it
			IDMUSProdPropSheet*	pIPropSheet;
			if( SUCCEEDED( m_pSignPostMgr->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				pIPropSheet->Show( TRUE );
				pIPropSheet->Release();

				if (!m_fShowGroupProps)
				{
					// Change to the signpost property page
					m_pSignPostMgr->OnShowProperties();
				}
				else
				{
					// Change to our property page
					OnShowProperties();
				}
			}
			hr = S_OK;
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
		case ID_EDIT_PASTE:
			if( m_lXPos >= 0 )
			{
				m_bContextMenuPaste = TRUE;
			}
			hr = Paste( NULL );
			m_bContextMenuPaste = FALSE;
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			m_pSignPostMgr->OnShowProperties();
			if(m_pSignPostMgr->m_pIPageManager != NULL)
			{
				m_pSignPostMgr->m_pIPageManager->RefreshData();
			}
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		default:
			break;
		}
		break;

	case WM_CREATE:
		// Get Left and right selection boundaries
		m_fSelected = FALSE;
		m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect );
		m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect );
		m_pSignPostMgr->ClearSelected();
		m_pSignPostMgr->m_lShiftFromMeasure = 0;
		break;

	default:
		break;
	}

	return hr;
}

// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip::GetData

HRESULT CSignPostStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);

	switch( *pdwIndex )
	{
	
	case 0:
	{
		// Copy our groupbits to the location pointed to by ppData
		ioGroupBitsPPGData *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPGData *>(*ppData);
		pGroupBitsPPGData->dwGroupBits = m_pSignPostMgr->m_dwGroupBits;
		break;
	}

	case 1:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pSignPostMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pSignPostMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}

	case 2:
		{
			ioSignPostStripPPG *poSignPostStripPPG = (ioSignPostStripPPG *)*ppData;
			poSignPostStripPPG->dwActivityLevel = m_pSignPostMgr->m_dwActivityLevel;
			break;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip::SetData

HRESULT CSignPostStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{

	case 0:
	{
		ioGroupBitsPPGData *pGroupBitsPPGData = reinterpret_cast<ioGroupBitsPPGData *>(pData);

		// Update our group bits setting, if necessary
		if( pGroupBitsPPGData->dwGroupBits != m_pSignPostMgr->m_dwGroupBits )
		{
			m_pSignPostMgr->m_dwGroupBits = pGroupBitsPPGData->dwGroupBits;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACK_GROUP;
			m_pSignPostMgr->UpdateSegment();
		}
		break;
	}

	case 1:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pSignPostMgr->m_dwTrackExtrasFlags )
		{
			m_pSignPostMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pSignPostMgr->UpdateSegment();
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pSignPostMgr->m_dwProducerOnlyFlags )
		{
			m_pSignPostMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pSignPostMgr->UpdateSegment();
		}
		break;
	}

	case 2:
		{
			ioSignPostStripPPG *piSignPostStripPPG = (ioSignPostStripPPG *)pData;
			m_nLastEdit = IDS_UNDO_ACTIVITYLEVEL;
			m_pSignPostMgr->m_dwActivityLevel = piSignPostStripPPG->dwActivityLevel;
			m_pSignPostMgr->UpdateSegment();
			break;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip::OnShowProperties

HRESULT CSignPostStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( !m_pSignPostMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Validate m_pSignPostMgr->m_pIFramework
	ASSERT( m_pSignPostMgr->m_pIFramework );

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	m_pSignPostMgr->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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
		ASSERT( m_pPropPageMgr != NULL );
		pIPropSheet->Release();
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		CSignPostStripPropPageMgr* pPPM = new CSignPostStripPropPageMgr;
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

	// Set the property page to refer to the Group property page.
	short nActiveTab;
	nActiveTab = CSignPostStripPropPageMgr::sm_nActiveTab;
	m_pSignPostMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip::OnRemoveFromPageManager

HRESULT CSignPostStrip::OnRemoveFromPageManager( void )
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
// CLyricStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CLyricStrip::FBDraw

HRESULT CSignPostStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(sv);

	// get function bar rect
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if (FAILED(m_pSignPostMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
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
// CLyricStrip::FBOnWMMessage

HRESULT CSignPostStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		{
			BOOL fShowProps = TRUE;
			if( lYPos <= m_bmCompose.GetBitmapDimension().cy )
			{
				VARIANT varXS;
				if( SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &varXS ) ) )
				{
					if( lXPos >= V_I4( &varXS ) - m_bmCompose.GetBitmapDimension().cx - 3 )
					{
						if( SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &varXS ) ) )
						{
							IPrivateSegment *pISegment;
							if( SUCCEEDED( V_UNKNOWN( &varXS )->QueryInterface( IID_IPrivateSegment, (void**)&pISegment ) ) )
							{
								fShowProps = FALSE;
								m_fComposeButtonDown = TRUE;

								// capture mouse so we get the LBUTTONUP message as well
								// the timeline will release the capture when it receives the
								// LBUTTONUP message
								VARIANT var;
								var.vt = VT_BOOL;
								V_BOOL(&var) = TRUE;
								m_pSignPostMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

								m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

								// Compose Chords
								hr = pISegment->Compose( m_pSignPostMgr->m_dwActivityLevel < 4 ? DMUS_COMPOSE_TEMPLATEF_ACTIVITY : 0, m_pSignPostMgr->m_dwActivityLevel );
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
										// No chords were generated
										strWarning.LoadString(IDS_ERR_COMPOSE_NOCHORDS);
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
									else if( hr == E_NOINTERFACE )
									{
										// Running on DX7 bits
										strWarning.LoadString(IDS_ERR_COMPOSE_DX8_REQUIRED);
									}
									else
									{
										// Shouldn't happen
										TRACE("SignPost Strip: Received an unknown error code %x\n", hr );
										ASSERT( FALSE );
										strWarning.LoadString(IDS_ERR_COMPOSE_UNEXPECTED);
									}
									AfxMessageBox(strWarning);
//									::MessageBox(NULL, LPCSTR(strWarning), NULL, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
									::SetFocus(hwndOld);
									hr = S_OK;

									m_fComposeButtonDown = FALSE;
									m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
								}
								pISegment->Release();
								V_UNKNOWN( &varXS )->Release();
							}
						}
					}
				}
			}
			if( fShowProps )
			{
				m_fShowGroupProps = TRUE;
				OnShowProperties();
			}
		}
		break;

	case WM_RBUTTONUP:
		m_fShowGroupProps = TRUE;
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

		if( m_pSignPostMgr->m_pTimeline )
		{
			m_pSignPostMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		if( m_fComposeButtonDown )
		{
			m_fComposeButtonDown = FALSE;
			m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		}
		break;

	default:
		break;
	}
	return hr;
}

void CSignPostStrip::SetEnabledItems(HMENU hMenu)
{
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return;
	}

	EnableMenuItem( hMenu, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_PASTE, ( CanPaste() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_VIEW_PROPERTIES, m_pSignPostMgr->m_bSelected ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
}


void CSignPostStrip::UnselectGutterRange( void )
{
	ASSERT( m_pSignPostMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pSignPostMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pSignPostMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostStrip IDMUSProdTimelineEdit

HRESULT CSignPostStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr;

	hr = CanCut();
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	hr = Copy(pITimelineDataObject);
	if(SUCCEEDED(hr))
	{
		hr = Delete();
	}

	return hr;
}

HRESULT CSignPostStrip::Copy(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT				hr;
	IStream				*pStreamCopy;

	hr = CanCopy();
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if(m_cfFormat == 0)
	{
		m_cfFormat = RegisterClipboardFormat(CF_SIGNPOST);
		if(m_cfFormat == 0)
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected signposts in.
	hr = CreateStreamOnHGlobal(NULL, TRUE, &pStreamCopy);
	if(FAILED(hr))
	{
		return E_OUTOFMEMORY;
	}

	// Save the signposts into the stream.
	hr = m_pSignPostMgr->SaveSignPostList(pStreamCopy, TRUE, true);
	if(FAILED(hr))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed IDMUSProdTimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfFormat, pStreamCopy );
		pStreamCopy->Release();
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
		hr = m_pSignPostMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pSignPostMgr->GetBoundariesOfSelectedCommands( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the IDMUSProdTimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfFormat, pStreamCopy );

		// Release the IStream we copied into
		pStreamCopy->Release();

		// Exit with an error if we failed to add the stream
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// get the IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export( &pIDataObject );

		// Release the IDMUSProdTimelineDataObject
		pITimelineDataObject->Release();

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
			pIDataObject->Release();
			return E_FAIL;
		}

		// If we already have a CopyDataObject, release it
		if(	m_pSignPostMgr->m_pCopyDataObject )
		{
			m_pSignPostMgr->m_pCopyDataObject->Release();
		}

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pSignPostMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the ITimelienDataObject
		//m_pSignPostMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}

HRESULT CSignPostStrip::Paste(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT				hr;
	IStream*			pStreamPaste;

	hr = CanPaste( pITimelineDataObject );
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if(m_cfFormat == 0)
	{
		m_cfFormat = RegisterClipboardFormat(CF_SIGNPOST);
		if(m_cfFormat == 0)
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
		hr = m_pSignPostMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		pIDataObject->Release();
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

	// Determine paste measure
	MUSIC_TIME mtTime;
	long lMeasure, lBeat;
	if( m_bContextMenuPaste )
	{
		m_pSignPostMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}
	}
	m_pSignPostMgr->m_pTimeline->ClocksToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	m_pSignPostMgr->m_pTimeline->MeasureBeatToClocks( m_pSignPostMgr->m_dwGroupBits, 0, lMeasure, 0, &mtTime );

	hr = pITimelineDataObject->IsClipFormatAvailable(m_cfFormat);
	if(hr != S_OK)
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}
	
	hr = pITimelineDataObject->AttemptRead(m_cfFormat, &pStreamPaste);
	if(hr != S_OK)
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pSignPostMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		pITimelineDataObject->Release();
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

			fChanged = m_pSignPostMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}
	pITimelineDataObject->Release();
	pITimelineDataObject = NULL;

	// Clear all selections
	m_pSignPostMgr->ClearSelected();

	// Now, do the paste operation
	hr = m_pSignPostMgr->LoadSignPostList( pStreamPaste, 0, FALSE, fChanged );

	pStreamPaste->Release(); 
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}


	if(SUCCEEDED(hr) && fChanged)
	{
		m_nLastEdit = IDS_UNDO_PASTE;
		m_pSignPostMgr->UpdateSegment();
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	}

	return S_OK;
}

HRESULT CSignPostStrip::Insert(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT			hr;

	hr = CanInsert();
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Make sure everything on the timeline is deselected.
	UnselectGutterRange();

	m_nLastEdit = IDS_UNDO_INSERT;
	hr = m_pSignPostMgr->InsertSignPost(-1);	// Insert at first selected.
	m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

	return hr;
}

HRESULT CSignPostStrip::Delete(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/* Ignore this, since we can be in a Cut() operation.  We can cut blank data, but we can't delete it.
	HRESULT hr = CanDelete();
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		return E_UNEXPECTED;
	}
	*/

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	m_nLastEdit = IDS_UNDO_DELETE;
	m_pSignPostMgr->DeleteSelected();
	m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

	return S_OK;
}

HRESULT CSignPostStrip::SelectAll(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pSignPostMgr->m_pTimeline != NULL);
	if(m_pSignPostMgr->m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	m_pSignPostMgr->SelectAll();

	m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	if(m_pSignPostMgr->m_pIPageManager != NULL)
	{
		m_pSignPostMgr->m_pIPageManager->RefreshData();
	}
	
	return S_OK;
}

HRESULT CSignPostStrip::CanCut(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pSignPostMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	if(CanCopy() == S_OK && CanDelete() == S_OK)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT CSignPostStrip::CanCopy(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pSignPostMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	//return m_pSignPostMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pSignPostMgr->m_bSelected)
	{
		SignPostExt* pSignPost = m_pSignPostMgr->GetFirstSelectedSignPost();
		return pSignPost == NULL ? S_FALSE : S_OK;
	}
	else return S_FALSE;
}

HRESULT CSignPostStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT				hr = S_OK;

	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if(m_cfFormat == 0)
	{
		m_cfFormat = RegisterClipboardFormat(CF_SIGNPOST);
		if(m_cfFormat == 0)
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfFormat );
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
			if( SUCCEEDED( m_pSignPostMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfFormat);
				}
				pITimelineDataObject->Release();
			}
			pIDataObject->Release();
		}
	}

	if (hr == S_OK)
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CSignPostStrip::CanInsert(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	//return m_pSignPostMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pSignPostMgr->m_bSelected)
	{
		SignPostExt* pSignPost = m_pSignPostMgr->GetFirstSelectedSignPost();
		return pSignPost == NULL ? S_OK : S_FALSE;
	}
	else return S_FALSE;
}

HRESULT CSignPostStrip::CanDelete(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pSignPostMgr != NULL);
	if(m_pSignPostMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	//return m_pSignPostMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pSignPostMgr->m_bSelected)
	{
		SignPostExt* pSignPost = m_pSignPostMgr->GetFirstSelectedSignPost();
		return pSignPost == NULL ? S_FALSE : S_OK;
	}
	else return S_FALSE;
}

HRESULT CSignPostStrip::CanSelectAll(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for(int n = 0; n < m_pSignPostMgr->m_nMeasures; n++)
	{
		if( m_pSignPostMgr->m_pMeasureArray[n].pSignPost )
		{
			return S_OK;
		}
	}
	return S_FALSE;
}


//////////////////////////////////////////////////////////////////////////////// drag drop methods
////////
////////

HRESULT CSignPostStrip::DoDragDrop(WPARAM mousekeybutton, LONG lXPos, long lmeasure)
{
	HRESULT hr;
	BOOL bDrop = FALSE;
	IDropSource*	pIDropSource;
	DWORD			dwEffect = DROPEFFECT_NONE;
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// create data object and image
		hr = CreateDataObject(&m_pISourceDataObject, lmeasure);
		if(SUCCEEDED(hr))
		{
			bDrop = TRUE;
			m_nStripIsDragDropSource = 1;	

			m_pDragImage = CreateDragImage();
			if(m_pDragImage)
			{
				m_pDragImage->BeginDrag(0, CPoint(8,12));
			}
			// start drag drop
			DWORD dwOKDragEffects = DROPEFFECT_COPY;
			if(CanCut() == S_OK)
			{
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}
			m_dwStartDragButton = (unsigned long)mousekeybutton;
			m_startDragPosition = lXPos;
			hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

			// drag drop completed, clean up
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
					m_pSignPostMgr->DeleteMarkedSignPosts(DRAG_SELECT);
				}
				if(dwEffect == DROPEFFECT_NONE)
				{
					bDrop = FALSE;
				}
				break;
			default:
				bDrop = FALSE;
				break;
			}
			hr = S_OK;
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;
		}
		pIDropSource->Release();
		m_pSignPostMgr->UnMarkSignPosts(DRAG_SELECT);
	}
	else
	{
		m_nStripIsDragDropSource = 0;	
		hr = E_FAIL;
	}

	if(bDrop)
	{
		if( dwEffect != DROPEFFECT_NONE )
		{
			if( m_nStripIsDragDropSource == 2 )
			{
				// Drag/drop target and source are the same PersRef strip
				if( dwEffect == DROPEFFECT_MOVE )
				{
					m_nLastEdit = IDS_UNDO_MOVE;
				}
				else
				{
					m_nLastEdit = IDS_UNDO_PASTE;
				}
				m_pSignPostMgr->UpdateSegment();
			}
			else
			{
				if( dwEffect == DROPEFFECT_MOVE )
				{
					m_nLastEdit = IDS_UNDO_DELETE;
					m_pSignPostMgr->UpdateSegment();
				}
			}

		}
		
		// Done with drag/drop
		m_nStripIsDragDropSource = 0;

		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		m_pSignPostMgr->OnShowProperties();
		// BUGBUG: I don't think this is necessary
		
	}
	return hr;
}

// IDropSource Methods
HRESULT CSignPostStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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

HRESULT CSignPostStrip::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwEffect);

	if( m_pDragImage )
	{
		CPoint pt;

		GetCursorPos( &pt );

		// Move the drag image
		m_pDragImage->DragMove( pt );
	}

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

CImageList* CSignPostStrip::CreateDragImage()
{
	return NULL;
}

HRESULT	CSignPostStrip::CreateDataObject(IDataObject** ppIDataObject, long measure)
{
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_pSignPostMgr != NULL );
	if( m_pSignPostMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pSignPostMgr->m_pTimeline != NULL );
	if( m_pSignPostMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Validate m_pSignPostMgr->m_pIFramework
	ASSERT( m_pSignPostMgr->m_pIFramework );

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Selected SignPosts into stream
	HRESULT hr = E_FAIL;


	if( SUCCEEDED ( m_pSignPostMgr->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{

		// mark the chords as being dragged: this used later for deleting chords in drag move
		// make sure that if this strip is drop target that they get deselected!
		short nZeroOffset = m_pSignPostMgr->MarkSelectedSignPosts(DRAG_SELECT);
		measure -= nZeroOffset;
		// write measure at which mouse grabbed drag selection into the stream
		DWORD cb;
		hr = pIStream->Write( &measure, sizeof( long ), &cb );
	    if( FAILED( hr ) || cb != sizeof( long ))
		{
			hr = E_FAIL;
			goto Leave;
		}


		if( SUCCEEDED ( m_pSignPostMgr->SaveDroppedSignPostList( pIStream, TRUE, nZeroOffset ) ) )
		{
			// Place CF_SignPostLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfFormat, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

Leave:
	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;

}

/////////////////////////////////////////////////// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostStrip::DragEnter

HRESULT CSignPostStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//TRACE("CSignPostStrip Drag Enter!\n");
	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( GetTimelineCWnd()->GetDesktopWindow (), point );
	}
/*
	// Make sure editor is on top
	if( m_pStyle->m_hWndEditor )
	{
		::BringWindowToTop( m_pStyle->m_hWndEditor );
	}
*/
	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostStrip::DragOver

HRESULT CSignPostStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pt);

	//TRACE("CSignPostStrip Drag OVER!\n");
	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	LONG lLeftSide;
	if( (pt.x >= 0) &&
		SUCCEEDED( m_pSignPostMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftSide ) ) &&
		SUCCEEDED( m_pSignPostMgr->m_pTimeline->ClocksToPosition( lLeftSide, &lLeftSide ) ) &&
		(pt.x >= lLeftSide) && (CanPasteFromData( m_pITargetDataObject ) == S_OK) )
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
// IDropTarget CSignPostStrip::DragLeave

HRESULT CSignPostStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//TRACE("CSignPostStrip Drag Leave!\n");

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetTimelineCWnd()->GetDesktopWindow () );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSignPostStrip::Drop

HRESULT CSignPostStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(grfKeyState);

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetTimelineCWnd()->GetDesktopWindow () );
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
			if(pWnd)
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
		hr = PasteAt( pIDataObject, pt );
		if( SUCCEEDED ( hr ) )
		{
			*pdwEffect = m_dwOverDragEffect;
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}



CWnd* CSignPostStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pSignPostMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pSignPostMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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


/////////////////////////////////////////////////////////// Drop (and paste) helpers
//
HRESULT CSignPostStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfFormat ) ))
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}



HRESULT CSignPostStrip::PasteAt(IDataObject* pIDataObject, POINTL pt)
{
	if( pIDataObject == NULL )
	{
		return E_NOTIMPL;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfFormat ) ) )
	{
		IStream* pIStream;
		if(SUCCEEDED (pDataObject->AttemptRead( pIDataObject, m_cfFormat, &pIStream)))
		{
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);

			BOOL fChanged = FALSE;
			
			hr = m_pSignPostMgr->LoadSignPostList( pIStream, pt.x, TRUE, fChanged );
			
			if( m_nStripIsDragDropSource )
			{
				// Drag/drop Target and Source are the same StyleRef strip
				m_nStripIsDragDropSource = 2;
			}
			else
			{
				if( fChanged )
				{
					m_nLastEdit = IDS_UNDO_PASTE;
					m_pSignPostMgr->UpdateSegment();
					UnselectGutterRange();
				}
			}
			
			m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		}
	}

	
	pDataObject->Release();
	
	return hr;
	
}

HRESULT CSignPostStrip::OnRButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	POINT pt;
	BOOL  bResult;

	m_fShowGroupProps = FALSE;

	// Get the measure number 
	long lMeasure;
	if(FAILED(m_pSignPostMgr->m_pTimeline->PositionToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, lXPos, &lMeasure, NULL)))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if(!m_pSignPostMgr->m_bSelected)
	{
		// Make sure everything on the timeline is deselected.
		UnselectGutterRange();

		// If nothing is selected, select this measure.
		m_pSignPostMgr->ToggleSelect(lMeasure);
		m_pSignPostMgr->m_lShiftFromMeasure = lMeasure;

		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *) this, NULL, TRUE);
	}
	else if(!(wParam & (MK_SHIFT | MK_CONTROL)))
	{
		// Make sure this measure is in the selected list.  If it isn't, make it the only one selected
		if(!m_pSignPostMgr->IsSelected(lMeasure))
		{
			// Make sure everything on the timeline is deselected first.
			UnselectGutterRange();

			m_pSignPostMgr->ClearSelected();
			m_pSignPostMgr->ToggleSelect(lMeasure);
			m_pSignPostMgr->m_lShiftFromMeasure = lMeasure;
		}
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *) this, NULL, TRUE);
	}

	if (!m_fShowGroupProps)
	{
		// Change to the signpost property page
		m_pSignPostMgr->OnShowProperties();
	}
	else
	{
		// Change to our property page
		OnShowProperties();
	}
	//if(m_pSignPostMgr->m_pIPageManager != NULL)
	//{
	//	m_pSignPostMgr->m_pIPageManager->RefreshData();
	//}

	// Get the cursor position (To put the menu there)
	bResult = GetCursorPos(&pt);
	ASSERT(bResult);
	if(!bResult)
	{
		return E_UNEXPECTED;
	}

	// Display the popup menu
	return m_pSignPostMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
}

HRESULT CSignPostStrip::OnLButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lYPos);
	m_fShowGroupProps = FALSE;

	// Release mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pSignPostMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	m_fLButtonDown = FALSE;
	m_dwStartDragButton = 0;

	// Get the clock position
	long lClocks;
	if( FAILED( m_pSignPostMgr->m_pTimeline->PositionToClocks( lXPos, &lClocks) ) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the measure number 
	long lMeasure;
	if(FAILED(m_pSignPostMgr->m_pTimeline->ClocksToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, lClocks, &lMeasure, NULL)))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if(!(wParam & MK_CONTROL) && !(wParam & MK_SHIFT))
	{
		m_pSignPostMgr->ClearSelected();
		// Toggle the selection state of this measure
		m_pSignPostMgr->ToggleSelect(lMeasure);
		// Redraw our strip
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	}
	else if((wParam & MK_CONTROL) && m_bWasSelected)
	{
		m_bWasSelected = false;
		m_pSignPostMgr->ToggleSelect(lMeasure);
		// redraw our strip
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	}

	m_bWasSelected = false;	// so toggle select is in sync
	return S_OK;
}

HRESULT CSignPostStrip::OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	m_fShowGroupProps = FALSE;
	m_fLButtonDown = FALSE;

	// Make sure everything on the timeline is deselected.
	UnselectGutterRange();

	// Get the clock position
	long lClocks;
	if( FAILED( m_pSignPostMgr->m_pTimeline->PositionToClocks( lXPos, &lClocks) ) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if(wParam & MK_SHIFT)
	{
		if(!(wParam & MK_CONTROL))
		{
			m_pSignPostMgr->ClearSelected();
		}
		m_pSignPostMgr->SelectSegment(-1, lClocks);
		m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		m_pSignPostMgr->OnShowProperties();
		//GiveTimelineFocus();
		return S_OK;
	}

	// Get the measure number 
	long lMeasure;
	if(FAILED(m_pSignPostMgr->m_pTimeline->ClocksToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, lClocks, &lMeasure, NULL)))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	m_pSignPostMgr->m_lShiftFromMeasure = lMeasure;

	// Get the info for the selected measure
	SignPostMeasureInfo* pMeasureInfo = NULL;
	if( lMeasure >=0 && lMeasure < m_pSignPostMgr->m_nMeasures)
	{
		pMeasureInfo = &(m_pSignPostMgr->m_pMeasureArray[lMeasure]);
	}

	if( pMeasureInfo )
	{
		if(pMeasureInfo->pSignPost && pMeasureInfo->dwFlags & SPMI_SELECTED )
		{
			m_pSignPostMgr->OnShowProperties();
			// start drag
//			DoDragDrop(wParam, lXPos, lMeasure);
			if(pMeasureInfo->pSignPost)
			{
				m_dwStartDragButton = wParam;
				m_fLButtonDown = TRUE;
			}
			// capture mouse so we get the LBUTTONUP message as well
			// the timeline will release the capture when it receives the
			// LBUTTONUP message
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSignPostMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
			// indicate toggle select to mouse up
			if(wParam & MK_CONTROL)
			{
				m_bWasSelected = true;
			}
		}
		else
		{
			// capture mouse so we get the LBUTTONUP message as well
			// the timeline will release the capture when it receives the
			// LBUTTONUP message
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pSignPostMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

			// If we clicked on a signpost, set m_fLButtonDown to TRUE
			if( pMeasureInfo->pSignPost )
			{
				m_dwStartDragButton = wParam;
				m_fLButtonDown = TRUE;
			}

			// If CTRL is not pressed, clear all selections in our strip
			if(!(wParam & MK_CONTROL))
			{
				m_pSignPostMgr->ClearSelected();
				m_pSignPostMgr->ToggleSelect(lMeasure);
				// Redraw our strip
				m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

				// Set the properties to the ones for the current selection.
				m_pSignPostMgr->OnShowProperties();
			}
			else if(wParam & MK_CONTROL && pMeasureInfo->dwFlags & SPMI_SELECTED)
			{
				// indicate toggle select to mouse up
				m_bWasSelected = true;
			}
			else
			{
				// Toggle the selection state of this measure
				m_pSignPostMgr->ToggleSelect(lMeasure);

				// Redraw our strip
				m_pSignPostMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);

				// Set the properties to the ones for the current selection.
				m_pSignPostMgr->OnShowProperties();
				//GiveTimelineFocus();
			}
		}
	}

	return S_OK;
}

HRESULT CSignPostStrip::OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	// Get the measure number
	long lMeasure;
	if( FAILED( m_pSignPostMgr->m_pTimeline->PositionToMeasureBeat( m_pSignPostMgr->m_dwGroupBits, 0, lXPos, &lMeasure, NULL ) ))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if( m_fLButtonDown )
	{
		if( abs(m_lMouseXPos - lXPos) <= 3 
		&&  abs(m_lMouseYPos - lYPos) <= 3 )
			return S_OK;
		// Release mouse capture
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pSignPostMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

		// Do Drag'n'drop
		DoDragDrop(m_dwStartDragButton, lXPos, lMeasure);

		// Clear button down flag so we don't do drag'n'drop again.
		m_fLButtonDown= FALSE;
	}

	return S_OK;
}
