// TimeStripMgr.cpp : Implementation of CTimeStripMgr
#include "stdafx.h"
#include "DMusProd.h"
#include "DMPPrivate.h"
#include "TimeStripMgr.h"
#include <dmusicf.h>
#include <initguid.h>
#include <math.h>
#include "TimelineCtl.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MUSICTIME_VERT_POS 15
#define REALTIME_VERT_POS 0

#define FIRST_SCROLL_ID		1
#define SECOND_SCROLL_ID	2
#define SCROLL_HORIZ_AMOUNT 25 

/////////////////////////////////////////////////////////////////////////////
// CTimeStripMgr

/////////////////////////////////////////////////////////////////////////////
// CTimeStripMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CTimeStripMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	if ( pData == NULL )
	{
		return E_POINTER;
	}
	if ( InlineIsEqualGUID( guidType, GUID_TimeSignature ) )
	{
		memcpy(	pData, &m_TimeSig, sizeof( DMUS_TIMESIGNATURE ) );
		((DMUS_TIMESIGNATURE*)pData)->mtTime = mtTime;
		if ( pmtNext != NULL )
		{
			*pmtNext = 0;
		}
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_TempoParam ) )
	{
		DMUS_TEMPO_PARAM *pTempoParam = (DMUS_TEMPO_PARAM *)pData;
		pTempoParam->mtTime = 0;
		pTempoParam->dblTempo = m_dblTempo;
		if ( pmtNext != NULL )
		{
			*pmtNext = 0;
		}
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		CComBSTR bstrTmp;
		bstrTmp.LoadString( this->m_pTimeStrip->m_nLastEdit );
		*(BSTR*)pData = bstrTmp.Detach();
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CTimeStripMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	if ( pData == NULL )
	{
		return E_POINTER;
	}
	if ( mtTime != 0 )
	{
		return E_FAIL;
	}
	if ( InlineIsEqualGUID( guidType, GUID_TimeSignature ) )
	{
		DMUS_TIMESIGNATURE *pTimeSig = (DMUS_TIMESIGNATURE *)pData;
		BOOL fChange = FALSE;
		if ( m_TimeSig.bBeatsPerMeasure != pTimeSig->bBeatsPerMeasure )
		{
			fChange = TRUE;
		}
		else if ( m_TimeSig.bBeat != pTimeSig->bBeat )
		{
			fChange = TRUE;
		}
		else if ( m_TimeSig.wGridsPerBeat != pTimeSig->wGridsPerBeat )
		{
			fChange = TRUE;
		}
		if ( fChange )
		{
			memcpy(	&m_TimeSig, pData, sizeof( DMUS_TIMESIGNATURE ) );
			m_TimeSig.mtTime = 0;
			m_pTimeline->StripInvalidateRect( m_pTimeStrip, NULL, TRUE );
			// BUGBUG: Should we also invalidate all other strips that belong to our group?
			// Probably not, since we belong to all groups.. But then how will the strips
			// that depend on us for their TimeSig be refreshed?
		}
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_TempoParam ) )
	{
		DMUS_TEMPO_PARAM *pTempoParam = (DMUS_TEMPO_PARAM *)pData;
		if ( m_dblTempo != pTempoParam->dblTempo )
		{
			m_dblTempo = pTempoParam->dblTempo;
			m_pTimeline->StripInvalidateRect( m_pTimeStrip, NULL, TRUE );
			// BUGBUG: Should we also invalidate all other strips that belong to our group?
			// Probably not, since we belong to all groups.. But then how will the strips
			// that depend on us for their TimeSig be refreshed?
		}
		return S_OK;
	}
	else if( InlineIsEqualGUID( guidType, GUID_TimelineShowTimeSig ) )
	{
		BOOL *pfShowTimeSig = (BOOL *)pData;
		if( m_fShowTimeSig != *pfShowTimeSig )
		{
			m_fShowTimeSig = *pfShowTimeSig;
			m_pTimeline->StripInvalidateRect( m_pTimeStrip, NULL, TRUE );
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CTimeStripMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if ( InlineIsEqualGUID( guidType, GUID_TimeSignature ) )
	{
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_TempoParam ) )
	{
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_TimelineShowTimeSig ) )
	{
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, CLSID_TimeStripMgr ) )
	{
		return S_OK;
	}
	else if ( InlineIsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CTimeStripMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Redraw our strip
		if( m_pTimeline )
		{
			m_pTimeline->StripInvalidateRect( m_pTimeStrip, NULL, TRUE );
		}
		return S_OK;
	}
	else if( ::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		// Redraw our strip
		if( m_pTimeline && m_pTimeStrip->m_bDisplayRealTime )
		{
			m_pTimeline->StripInvalidateRect( m_pTimeStrip, NULL, TRUE );
		}
		return S_OK;
	}

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CTimeStripMgr::GetStripMgrProperty(
		/* [in] */  STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */	VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pVariant );
	if( !pVariant )
	{
		return E_POINTER;
	}

	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		pVariant->vt = VT_UNKNOWN;
		if( m_pTimeline )
		{
			V_UNKNOWN( pVariant ) = m_pTimeline;
			V_UNKNOWN( pVariant )->AddRef();
			return S_OK;
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;
		/* Unsupported
	case SMP_IDIRECTMUSICTRACK:
		break;
	case SMP_IDMUSPRODFRAMEWORK:
		break;
	case SMP_DMUSIOTRACKHEADER:
		break;
		*/
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTimeStripMgr::SetStripMgrProperty(
		/* [in] */  STRIPMGRPROPERTY stripMgrProperty,
		/* [in] */	VARIANT		variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		if( m_pTimeline )
		{
			m_pTimeline->RemoveStrip( m_pTimeStrip );
			delete m_pTimeStrip;
			m_pTimeStrip = NULL;

			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, 0xFFFFFFFF );
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}
		if( V_UNKNOWN( &variant ) && FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
		{
			m_pTimeline = NULL;
			return E_FAIL;
		}
		if( m_pTimeline )
		{
			m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, 0xFFFFFFFF );
			m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, 0xFFFFFFFF );

			CTimeStrip*	pTimeStrip = new CTimeStrip(this);
			if( !pTimeStrip )
			{
				return E_OUTOFMEMORY;
			}

			m_pTimeStrip = pTimeStrip;

			m_pTimeline->AddStrip( pTimeStrip );
		}
		break;
		/* Unsupported
	case SMP_IDIRECTMUSICTRACK:
		break;
	case SMP_IDMUSPRODFRAMEWORK:
		break;
	case SMP_DMUSIOTRACKHEADER:
		break;
		*/
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTimeStrip

/////////////////////////////////////////////////////////////////////////////
// CTimeStrip IUnknown
STDMETHODIMP CTimeStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( ppv );
    *ppv = NULL;
    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else
	{
		return E_NOTIMPL;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CTimeStrip::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CTimeStrip::Release(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
// CTimeStrip IDMUSProdStrip
HRESULT	STDMETHODCALLTYPE CTimeStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	UNREFERENCED_PARAMETER( sv );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTimeStripMgr->m_pTimeline )
	{
		m_pTimeStripMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, 0xFFFFFFFF, 0, lXOffset );
		{
			RECT	rectClip;
			::GetClipBox( hDC, &rectClip );

			if ( m_bDisplayMusicTime == TRUE )
			{
				DrawMusicTime( hDC, lXOffset );
			}

			if ( m_bDisplayRealTime == TRUE )
			{
				switch (m_tsdDisplayRealTime)
				{
				case TS_RT_SECOND:
					DrawRealOnSecond( hDC, lXOffset );
					break;
				case TS_RT_MEASURE:
					DrawRealOnMeasure( hDC, lXOffset );
					break;
				case TS_RT_BEAT:
					DrawRealOnBeat( hDC, lXOffset );
					break;
				default:
					break;
				}
			}
			// invertrect selected time
			if( m_lBeginSelect != m_lEndSelect )
			{
				long beginPos, endPos, cursorPos;

				m_pTimeStripMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &cursorPos );
				if( cursorPos > 0 )
				{
					m_pTimeStripMgr->m_pTimeline->ClocksToPosition( cursorPos, &cursorPos );
				}
				else
				{
					cursorPos = 0;
				}
				
				if (m_lBeginSelect > 0)
				{
					m_pTimeStripMgr->m_pTimeline->ClocksToPosition( m_lBeginSelect, &beginPos );
				}
				else
				{
					beginPos = 0;
				}
				if (m_lEndSelect > 0)
				{
					m_pTimeStripMgr->m_pTimeline->ClocksToPosition( m_lEndSelect, &endPos );
				}
				else
				{
					endPos = 0;
				}

				if( endPos && ( beginPos == endPos ) )
				{
					endPos += 1;
				}

				if ( cursorPos )
				{
					if ( beginPos == cursorPos )
					{
						beginPos++;
					}
					else if ( endPos == cursorPos )
					{
						endPos--;
					}
				}
				rectClip.left = beginPos - lXOffset;
				rectClip.right = endPos - lXOffset;

				GrayOutRect( hDC, &rectClip );
			}
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTimeStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
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

	case SP_MINHEIGHT:
	case SP_DEFAULTHEIGHT:
	case SP_MAXHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = STRIP_HEIGHT;
		break;

	case SP_NAME:
		{
			if ( !m_pTimeStripMgr )
			{
				return E_FAIL;
			}

			HRESULT hr = E_FAIL;

			// Only display the time sig if m_pTimeStripMgr->m_fShowTimeSig is set
			if( m_pTimeStripMgr->m_fShowTimeSig )
			{
				BSTR bstr;
				CString str;

				long lTimeSigBottom = m_pTimeStripMgr->m_TimeSig.bBeat;
				// Zero is really 256
				if ( lTimeSigBottom == 0 )
				{
					lTimeSigBottom = 256;
				}
				str.Format("%d / %d", m_pTimeStripMgr->m_TimeSig.bBeatsPerMeasure, lTimeSigBottom );

				// Set pvar
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

				// We've succeeded
				hr = S_OK;
			}

			if( FAILED( hr ) )
			{
				return hr;
			}
		}
		break;

	case SP_CURSOR_HANDLE:
		if (m_hCursor)
		{
			pvar->vt = VT_I4;
			V_I4(pvar) = (long) m_hCursor;
		}
		else return E_FAIL;
		break;

	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pTimeStripMgr )
		{
			m_pTimeStripMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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

void CTimeStrip::OnGutterSelect( BOOL fSelect )
{
	// Set selection start to 0
	m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );

	// now see if it was originally zero
	if( !fSelect )
	{
		// Set selection end to 0 (should clear selection)
		m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	}
	else
	{
		// Set selection end to entire timeline
		VARIANT varLength;
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength );

		m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, V_I4(&varLength) );
	}
}

/*
	VARIANT varLength;
	long lTemp;
	IDMUSProdTimeline* pTimeline = m_pTimeStripMgr->m_pTimeline;
	// if we've currently selected all time, deselect all time. Otherwise,
	// select all time
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength );
	pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTemp );
	// this gets set to 0 regardless
	pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	// now see if it was originally zero
	if( lTemp == 0 )
	{
		// and see if the endtime was the length
		pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTemp );
		if( lTemp == V_I4(&varLength) )
		{
			pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
			pTimeline->StripInvalidateRect( this, NULL, FALSE );
			return;
		}
	}
	pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, V_I4(&varLength) );
	pTimeline->StripInvalidateRect( this, NULL, FALSE );
*/

HRESULT STDMETHODCALLTYPE CTimeStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
	{
		if( var.vt != VT_I4 )
		{
			return E_INVALIDARG;
		}

		BOOL fRefresh = FALSE;
		if( sp == SP_BEGINSELECT )
		{
			if( m_lBeginSelect != V_I4( &var ) )
			{
				fRefresh = TRUE;
				m_lBeginSelect = V_I4( &var );
				if( m_lBeginSelect != 0 )
				{
					// If begin selection is not 0, clear our gutter selection
					var.vt = VT_BOOL;
					V_BOOL( &var ) = FALSE;
					m_pTimeStripMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_GUTTER_SELECTED, var );
				}
			}
		}
		else
		{
			if( m_lEndSelect != V_I4( &var ) )
			{
				fRefresh = TRUE;
				m_lEndSelect = V_I4( &var );

				// Get length of timeline
				m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
				if( m_lEndSelect != V_I4( &var ) )
				{
					// If end selection is not the end of the timeline, clear our gutter selection
					var.vt = VT_BOOL;
					V_BOOL( &var ) = FALSE;
					m_pTimeStripMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip*)this, STP_GUTTER_SELECTED, var );
				}
			}
		}

		if( fRefresh )
		{
			m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}
	}
		break;

	case SP_GUTTERSELECT:
		if( var.vt != VT_BOOL )
		{
			return E_INVALIDARG;
		}
		OnGutterSelect(V_BOOL( &var ));
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}

void CTimeStrip::OnLButtonDown( WPARAM wParam, LONG lXPos )
{
	int fwKeys = wParam;        // key flags 
	long time;
	VARIANT var;

	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTimeStripMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	m_pTimeStripMgr->m_pTimeline->PositionToClocks( lXPos, &time );
	SnapTime( &time );

	if (m_tsrResizing == TS_NOTRESIZING)
	{
		if( !( fwKeys & MK_SHIFT ))
		{
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, time );
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, time );
			m_fScrollWhenSettingTimeCursor = FALSE;
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, m_lBeginSelect );
			m_fScrollWhenSettingTimeCursor = TRUE;
			m_lBeginSelect = time;
			m_lEndSelect = time;

			m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
		}
		else if ( fwKeys & MK_SHIFT )
		{
			if( time != m_lBeginSelect )
			{
				if( time < m_lBeginSelect )
				{
					// End time is before begin time - swap them
					UpdateStartEnd( time, m_lBeginSelect );
				}
				else if( time > m_lBeginSelect )
				{
					// End time is after begin time - leave it
					UpdateStartEnd( m_lBeginSelect, time );
				}
			}
		}
	}
	else if (m_tsrResizing == TS_START)
	{
		if( m_lBeginSelect != time )
		{
			if( time < m_lEndSelect )
			{
				UpdateStartEnd( time, m_lEndSelect );
			}
			else
			{
				// Move start past end - switch to resizing end time
				m_tsrResizing = TS_END;
				UpdateStartEnd( m_lEndSelect, time );
			}
		}
	}
	else if (m_tsrResizing == TS_END)
	{
		if( m_lEndSelect != time )
		{
			if( time > m_lBeginSelect )
			{
				UpdateStartEnd( m_lBeginSelect, time );
			}
			else
			{
				// Move end before start - switch to resizing start time
				m_tsrResizing = TS_START;
				UpdateStartEnd( time, m_lBeginSelect );
			}
		}
	}

	m_fSelecting = TRUE;
}

void CTimeStrip::OnMouseMove( LONG lXPos )
{
	long time;

	if( m_fLButtonDown )
	{
		// Deal with timer used for scrolling
		VARIANT var;

		// Get rectangle defining strip position
		var.vt = VT_BYREF;
		RECT rectStrip;
		V_BYREF(&var) = &rectStrip;
		if( SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
		{
			// Get cursor position
			POINT pt;
			::GetCursorPos( &pt );

			// Convert cursor position to strip coordinates
			if( SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->ScreenToStripPosition( (IDMUSProdStrip *)this, &pt ) ) 
			&&  SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &pt ) ) )
			{
				if( pt.x < rectStrip.left 
				||  pt.x > rectStrip.right )
				{
					// May need to scroll, cursor is NOT in the visible portion of the strip
					if( m_nScrollTimerID == 0 )
					{
						StartScrollTimer( FIRST_SCROLL_ID );
					}
				}
				else
				{
					// No need to scroll, cursor IS in the visible portion of the strip
					if( m_nScrollTimerID )
					{
						StopScrollTimer();
					}
				}
			}
		}
	}

	m_pTimeStripMgr->m_pTimeline->PositionToClocks( lXPos, &time );
	SnapTime( &time );

	if ((m_tsrResizing == TS_NOTRESIZING) || (m_tsrResizing == TS_END))
	{
		if( m_lEndSelect != time )
		{
			if( time > m_lBeginSelect )
			{
				UpdateStartEnd( m_lBeginSelect, time );
			}
			else
			{
				// Move end before start - switch to resizing start time
				m_tsrResizing = TS_START;
				UpdateStartEnd( time, m_lBeginSelect );
			}
		}
	}
	else if (m_tsrResizing == TS_START)
	{
		if( m_lBeginSelect != time )
		{
			if( time < m_lEndSelect )
			{
				UpdateStartEnd( time, m_lEndSelect );
			}
			else
			{
				// Move start past end - switch to resizing end time
				m_tsrResizing = TS_END;
				UpdateStartEnd( m_lEndSelect, time );
			}
		}
	}
}

void CTimeStrip::OnButtonClicked( WPARAM wParam )
{
	if (wParam == BTN_REALTIME)
	{
		if (m_bDisplayRealTime == FALSE)
		{
			ASSERT(m_bDisplayMusicTime == TRUE);
			m_bDisplayRealTime = TRUE;
			m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
		}
		else // m_bDisplayRealTime == TRUE
		{
			if ( m_bDisplayMusicTime == TRUE )
			{
				m_bDisplayRealTime = FALSE;
				m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
			}
		}
	}
	if (wParam == BTN_MUSICTIME)
	{
		if (m_bDisplayMusicTime == FALSE)
		{
			ASSERT(m_bDisplayRealTime == TRUE);
			m_bDisplayMusicTime = TRUE;
			m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
		}
		else // m_bDisplayMusicTime == TRUE
		{
			if ( m_bDisplayRealTime == TRUE )
			{
				m_bDisplayMusicTime = FALSE;
				m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
			}
		}
	}
}

HRESULT STDMETHODCALLTYPE CTimeStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER( lYPos );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_CREATE:
		{
			CWnd *pWnd = GetTimelineCWnd();
			if (pWnd)
			{
				RECT rect;
				rect.top = 0;
				rect.bottom = 15;
				rect.left = 0;
				rect.right = 15;

				m_BtnRealTime.Create( "R", BS_OWNERDRAW | BS_PUSHBUTTON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, rect, pWnd, BTN_REALTIME );
				m_BtnRealTime.LoadBitmaps( IDB_REALTIME, IDB_REALTIMEDOWN );
				m_BtnRealTime.EnableToolTips(TRUE);

				m_BtnMusicTime.Create( "M", BS_OWNERDRAW | BS_PUSHBUTTON | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, rect, pWnd, BTN_MUSICTIME );
				m_BtnMusicTime.LoadBitmaps( IDB_MUSICTIME, IDB_MUSICTIMEDOWN );
				m_BtnMusicTime.EnableToolTips(TRUE);
			}
		}
		break;

	case WM_TIMER:
		OnTimer();
		break;
	case WM_LBUTTONDOWN:
		m_fLButtonDown = TRUE;
		OnLButtonDown( wParam, lXPos );
		break;
	case WM_LBUTTONUP:
		StopScrollTimer();
		m_fLButtonDown = FALSE;
		m_fSelecting = FALSE;
		break;
	case WM_DRAWITEM:
		OnDrawItem( wParam, lParam );
		break;
	case BN_CLICKED:
		OnButtonClicked( wParam );
		break;
	case WM_RBUTTONUP:
		{
			HMENU hMenu;
			HMENU hMenuPopup;

			hMenu = ::LoadMenu( AfxGetStaticModuleState()->m_pCurrentWinApp->m_hInstance, MAKEINTRESOURCE( IDR_REALTIME_MENU ));			//		hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE( IDR_EDIT_RMENU ));
			hMenuPopup = ::GetSubMenu( hMenu, 0 );

			::CheckMenuItem( hMenuPopup, ID_RT_SECONDS, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_RT_MEASURES, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_RT_BEATS, MF_UNCHECKED );

			switch( m_tsdDisplayRealTime )
			{
			case TS_RT_SECOND:
				::CheckMenuItem( hMenuPopup, ID_RT_SECONDS, MF_CHECKED );
				break;
			case TS_RT_MEASURE:
				::CheckMenuItem( hMenuPopup, ID_RT_MEASURES, MF_CHECKED );
				break;
			case TS_RT_BEAT:
				::CheckMenuItem( hMenuPopup, ID_RT_BEATS, MF_CHECKED );
				break;
			}

			::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_UNCHECKED );
			::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_UNCHECKED );

			switch( m_stSetting )
			{
			case DMUSPROD_TIMELINE_SNAP_NONE:
				::CheckMenuItem( hMenuPopup, ID_SNAP_NONE, MF_CHECKED );
				break;
			case DMUSPROD_TIMELINE_SNAP_GRID:
				::CheckMenuItem( hMenuPopup, ID_SNAP_GRID, MF_CHECKED );
				break;
			case DMUSPROD_TIMELINE_SNAP_BEAT:
				::CheckMenuItem( hMenuPopup, ID_SNAP_BEAT, MF_CHECKED );
				break;
			case DMUSPROD_TIMELINE_SNAP_BAR:
				::CheckMenuItem( hMenuPopup, ID_SNAP_BAR, MF_CHECKED );
				break;
			}

			if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetParam( GUID_Segment_DisplayContextMenu, 0xFFFFFFFF, 0, 0, NULL, &lParam ) ) )
			{
				EnableMenuItem( hMenuPopup, ID_EDIT_DELETE_TRACK, ((CTimelineCtl*)m_pTimeStripMgr->m_pTimeline)->ShouldEnableDeleteTrack() ? MF_ENABLED : MF_GRAYED );
			}
			else
			{
				::DeleteMenu( hMenuPopup, ID_EDIT_DELETE_TRACK, MF_BYCOMMAND );
				::DeleteMenu( hMenuPopup, ID_EDIT_ADD_TRACK, MF_BYCOMMAND );
				::DeleteMenu( hMenuPopup, 2, MF_BYPOSITION );
			}

			// Get the cursor position (To put the menu there)
			POINT pt;
			BOOL bResult = GetCursorPos( &pt );
			ASSERT( bResult );
			if( !bResult )
			{
				break;
			}
			
			ASSERT(m_pTimeStripMgr->m_pTimeline);
			m_pTimeStripMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);

			::DestroyMenu( hMenu );
		}
		break;

	case WM_SETFOCUS:
		OnShowProperties();
		break;

	case WM_SETCURSOR:
		if( !m_fSelecting )
		{
			if( m_lBeginSelect == m_lEndSelect )
			{
				HCURSOR hCursorArrow = GetArrowCursor();
				if( hCursorArrow )
					m_hCursor = hCursorArrow;
				m_tsrResizing = TS_NOTRESIZING;
			}
			else
			{
				long nPos;
				BOOL fResizing = FALSE;
				if (m_lEndSelect >= 0)
				{
					m_pTimeStripMgr->m_pTimeline->ClocksToPosition(m_lEndSelect,&nPos);
					if (abs(lXPos-nPos)<3)
					{
						fResizing = TRUE;
						m_tsrResizing = TS_END;
					}
				}
				if (m_lBeginSelect >= 0)
				{
					m_pTimeStripMgr->m_pTimeline->ClocksToPosition(m_lBeginSelect,&nPos);
					if (abs(lXPos-nPos)<3)
					{
						fResizing = TRUE;
						m_tsrResizing = TS_START;
					}
				}
				if (fResizing)
				{
					HCURSOR hCursorSizeWE = GetWECursor();
					if( hCursorSizeWE )
						m_hCursor = hCursorSizeWE;
				}
				else
				{
					HCURSOR hCursorArrow = GetArrowCursor();
					if( hCursorArrow )
						m_hCursor = hCursorArrow;
					m_tsrResizing = TS_NOTRESIZING;
				}
			}
		}
		break;
	case WM_MOUSEMOVE:
		if( m_fSelecting )
		{
			OnMouseMove( lXPos );
		}
		break;
	case WM_SIZE:
		hr = OnSize( wParam, lParam );
		break;
	case WM_COMMAND:
		{
			;
			ASSERT(m_pTimeStripMgr);
			if ( m_pTimeStripMgr == NULL )
			{
				break;
			}
			ASSERT(m_pTimeStripMgr->m_pTimeline);
			if ( m_pTimeStripMgr->m_pTimeline == NULL )
			{
				break;
			}
			
			BOOL bResult;
			WORD wID = LOWORD(wParam);	// item, control, or accelerator identifier 
			switch(wID)
			{
			case ID_RT_SECONDS:
				if ( m_tsdDisplayRealTime != TS_RT_SECOND )
				{
					m_tsdDisplayRealTime = TS_RT_SECOND;
					m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				}
				break;
			case ID_RT_MEASURES:
				if ( m_tsdDisplayRealTime != TS_RT_MEASURE )
				{
					m_tsdDisplayRealTime = TS_RT_MEASURE;
					m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				}
				break;
			case ID_RT_BEATS:
				if ( m_tsdDisplayRealTime != TS_RT_BEAT )
				{
					m_tsdDisplayRealTime = TS_RT_BEAT;
					m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				}
				break;
			case ID_SNAP_NONE:
				SetSnapTo( DMUSPROD_TIMELINE_SNAP_NONE );
				break;
			case ID_SNAP_GRID:
				SetSnapTo( DMUSPROD_TIMELINE_SNAP_GRID );
				break;
			case ID_SNAP_BEAT:
				SetSnapTo( DMUSPROD_TIMELINE_SNAP_BEAT );
				break;
			case ID_SNAP_BAR:
				SetSnapTo( DMUSPROD_TIMELINE_SNAP_BAR );
				break;
			case ID_EDIT_HORIZZOOMIN:
				((CTimelineCtl *)m_pTimeStripMgr->m_pTimeline)->OnZoomIn( 0, 0, 0, bResult );
				break;
			case ID_EDIT_HORIZZOOMOUT:
				((CTimelineCtl *)m_pTimeStripMgr->m_pTimeline)->OnZoomOut( 0, 0, 0, bResult );
				break;
			case ID_EDIT_DELETE_TRACK:
			case ID_EDIT_ADD_TRACK:
				::SendMessage( ((CTimelineCtl *)m_pTimeStripMgr->m_pTimeline)->GetParent(), WM_COMMAND, wParam, lParam );
				break;
			case ID_VIEW_PROPERTIES :
				return E_NOTIMPL;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return S_OK;
}

HCURSOR CTimeStrip::GetWECursor(void)
{
	static HCURSOR hCursorSizeWE;
	if (!hCursorSizeWE)
		hCursorSizeWE = LoadCursor( NULL, IDC_SIZEWE ); // West/east resize
	return hCursorSizeWE;
}

HCURSOR CTimeStrip::GetArrowCursor(void)
{
	static HCURSOR hCursorArrow;
	if (!hCursorArrow)
		hCursorArrow = LoadCursor( NULL, IDC_ARROW );
	return hCursorArrow;
}

void CTimeStrip::DrawMusicTime( HDC hDC, LONG lXOffset )
{
	{
		// get first number we need to draw
		RECT	rectClip;
		long	measure, position, endPosition;
		char	cstrMeasure[20];
		VARIANT var;

		::GetClipBox( hDC, &rectClip );
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		m_pTimeStripMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &endPosition );

		if ( SUCCEEDED(m_pTimeStripMgr->m_pTimeline->PositionToMeasureBeat( 0xFFFFFFFF, 0, rectClip.left + lXOffset, &measure, NULL )) )
		{
			do	{
				m_pTimeStripMgr->m_pTimeline->MeasureBeatToPosition( 0xFFFFFFFF, 0, measure, 0, &position );
				_itoa( (int)(++measure), cstrMeasure, 10 );
				position++;
				if( position < endPosition )
				{
					::TextOut( hDC, position - lXOffset, MUSICTIME_VERT_POS, cstrMeasure, strlen(cstrMeasure) );
				}
			} while( (position < rectClip.right + lXOffset) && (position < endPosition) );
		}
	}
}

void CTimeStrip::DrawRealOnSecond( HDC hDC, LONG lXOffset )
{
	{
		RECT	rectClip;
		long	position, endPosition;
		char	strTime[20];
		VARIANT var;

		::GetClipBox( hDC, &rectClip );
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		m_pTimeStripMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &endPosition );

		REFERENCE_TIME refTime,refSecond;
		m_pTimeStripMgr->m_pTimeline->PositionToRefTime( rectClip.left + lXOffset, &refTime);
		refSecond = refTime / REFCLOCKS_PER_SECOND;

		HPEN hPenTimeTick;
		hPenTimeTick = ::CreatePen( PS_SOLID, 1, m_colorTimeTick );
		if( hPenTimeTick )
		{
			do
			{
				m_pTimeStripMgr->m_pTimeline->RefTimeToPosition( refSecond * REFCLOCKS_PER_SECOND, &position );
				if( position < endPosition )
				{
					if( refSecond < 60 )
					{
						_snprintf( strTime, 19, "%02d", (int)refSecond );
					}
					else
					{
						_snprintf( strTime, 19, "%d:%02d", (int)refSecond/60, (int)refSecond%60 );
					}
					::TextOut( hDC, position - lXOffset, REALTIME_VERT_POS, strTime, strlen(strTime) );

					HPEN hPenOld;
					hPenOld = static_cast<HPEN> ( ::SelectObject( hDC, hPenTimeTick ) );
					::MoveToEx( hDC, position - lXOffset, 0, NULL );
					::LineTo( hDC, position - lXOffset, STRIP_HEIGHT / 4 );
					::SelectObject( hDC, hPenOld );
				}
				refSecond++;
			} while( position < rectClip.right + lXOffset );
			
			::DeleteObject( hPenTimeTick );
		}
	}
}

void CTimeStrip::DrawRealOnMeasure( HDC hDC, LONG lXOffset )
{
	{
		RECT	rectClip;
		long	measure, position, endPosition, endMeasure;
		char	strTime[20];
		VARIANT var;
		
		::GetClipBox( hDC, &rectClip );
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		m_pTimeStripMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &endPosition );
		m_pTimeStripMgr->m_pTimeline->PositionToMeasureBeat( 0xFFFFFFFF, 0, rectClip.right + lXOffset, &endMeasure, NULL );
		m_pTimeStripMgr->m_pTimeline->PositionToMeasureBeat( 0xFFFFFFFF, 0, rectClip.left + lXOffset, &measure, NULL );

		while( measure <= endMeasure )
		{
			m_pTimeStripMgr->m_pTimeline->MeasureBeatToPosition( 0xFFFFFFFF, 0, measure, 0, &position );
			position++;
			if( position < endPosition )
			{
				REFERENCE_TIME refTimeTmp;
				m_pTimeStripMgr->m_pTimeline->MeasureBeatToRefTime( 0xFFFFFFFF, 0, measure, 0, &refTimeTmp);
				float fTime = (float) refTimeTmp;
				fTime /= REFCLOCKS_PER_SECOND;
				if( fTime >= 60.0 )
				{
					_snprintf( strTime, 19, "%d:%#05.2f", int(fTime / 60.0), fmod(fTime, 60.0));
				}
				else
				{
					_snprintf( strTime, 19, "%#05.2f", fTime);
				}
				::TextOut( hDC, position - lXOffset, REALTIME_VERT_POS, strTime, strlen(strTime) );
			}
			measure++;
		};
	}
}

void CTimeStrip::DrawRealOnBeat( HDC hDC, LONG lXOffset )
{
	{
		RECT	rectClip;
		long	measure, beat, position, endPosition, endMeasure;
		char	strTime[20];
		VARIANT var;
		
		::GetClipBox( hDC, &rectClip );
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		m_pTimeStripMgr->m_pTimeline->ClocksToPosition( V_I4(&var), &endPosition );
		m_pTimeStripMgr->m_pTimeline->PositionToMeasureBeat( 0xFFFFFFFF, 0, rectClip.right + lXOffset, &endMeasure, NULL );
		m_pTimeStripMgr->m_pTimeline->PositionToMeasureBeat( 0xFFFFFFFF, 0, rectClip.left + lXOffset, &measure, NULL );

		DMUS_TIMESIGNATURE TimeSig;
		MUSIC_TIME mtTSCur;
		m_pTimeStripMgr->m_pTimeline->MeasureBeatToClocks( 0xFFFFFFFF, 0, measure, 0, &mtTSCur );

		HRESULT hr;
		while( measure <= endMeasure )
		{
			hr = m_pTimeStripMgr->m_pTimeline->GetParam( GUID_TimeSignature, 0xFFFFFFFF, 0, mtTSCur, NULL, &TimeSig );
			if ( FAILED( hr ) )
			{
				return;
			}

			for (beat = 0; beat < TimeSig.bBeatsPerMeasure; beat ++)
			{
				m_pTimeStripMgr->m_pTimeline->MeasureBeatToPosition( 0xFFFFFFFF, 0, measure, beat, &position );
				position++;
				if( position < endPosition )
				{
					REFERENCE_TIME refTimeTmp;
					m_pTimeStripMgr->m_pTimeline->MeasureBeatToRefTime( 0xFFFFFFFF, 0, measure, beat, &refTimeTmp);
					float fTime = (float) refTimeTmp;
					fTime /= REFCLOCKS_PER_SECOND;
					if( fTime >= 60.0 )
					{
						_snprintf( strTime, 19, "%d:%#05.2f", int(fTime / 60.0), fmod(fTime, 60.0));
					}
					else
					{
						_snprintf( strTime, 19, "%#05.2f", fTime);
					}
					::TextOut( hDC, position - lXOffset, REALTIME_VERT_POS, strTime, strlen(strTime) );
				}
			}
			measure++;
			mtTSCur += NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ) * TimeSig.bBeatsPerMeasure;
		};
	}
}

HRESULT CTimeStrip::FBDraw( HDC hDC, STRIPVIEW sv)
{
	UNREFERENCED_PARAMETER( hDC );
	UNREFERENCED_PARAMETER( sv );
	return E_NOTIMPL;
}

HRESULT CTimeStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( lXPos );
	UNREFERENCED_PARAMETER( lYPos );

	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fLButtonDown = TRUE;
		break;

	case WM_LBUTTONUP:
		StopScrollTimer();
		m_fLButtonDown = FALSE;
		m_fSelecting = FALSE;
		break;

	case WM_MOUSEMOVE:
		if( m_fSelecting )
		{
			// May need to scroll, cursor is NOT in the visible portion of the strip
			if( m_nScrollTimerID == 0 )
			{
				StartScrollTimer( FIRST_SCROLL_ID );
			}
		}
		break;

	case WM_SETCURSOR:
		if( !m_fSelecting )
		{
			HCURSOR hCursorArrow = GetArrowCursor();
			if( hCursorArrow )
				m_hCursor = hCursorArrow;
			m_tsrResizing = TS_NOTRESIZING;
		}
		break;

	default:
		break;
	};

	return S_OK;
}

void CTimeStrip::OnDrawItem( WPARAM wParam, LPARAM lParam )
{
	UINT	idCtl;
	idCtl = (UINT) wParam;             // control identifier 

	if( idCtl == BTN_REALTIME )
	{
		m_BtnRealTime.SendMessage( WM_DRAWITEM, wParam, lParam );
	}
	else if( idCtl == BTN_MUSICTIME )
	{
		m_BtnMusicTime.SendMessage( WM_DRAWITEM, wParam, lParam );
	}
}

HRESULT CTimeStrip::OnSize( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( wParam );
	int nWidth;
	VARIANT var;

	if( FAILED(m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var )))
	{
		return E_UNEXPECTED;
	}
	nWidth = V_I4(&var) - BORDER_VERT_WIDTH;
	// move the RealTime and MusicTime buttons to where they need to go
	RECT rectMusicTime, rectRealTime;
	m_BtnMusicTime.GetClientRect(&rectMusicTime);
	m_BtnRealTime.GetClientRect(&rectRealTime);
	rectMusicTime.left = nWidth - rectMusicTime.right;
	rectMusicTime.right = nWidth;
	rectMusicTime.top = BORDER_VERT_WIDTH + MUSICTIME_VERT_POS;
	rectMusicTime.bottom = rectMusicTime.top + 15;

	rectRealTime.left = nWidth - rectRealTime.right;
	rectRealTime.right = nWidth;
	rectRealTime.top = BORDER_VERT_WIDTH + REALTIME_VERT_POS;
	rectRealTime.bottom = rectRealTime.top + 15;

	m_BtnRealTime.MoveWindow( &rectRealTime, TRUE );
	m_BtnMusicTime.MoveWindow( &rectMusicTime, TRUE );
	return S_OK;
}

void CTimeStrip::SnapTime( long *plTime )
{
	ASSERT( plTime );

	// ensure plTime is not negative
	*plTime = max( *plTime, 0 );

	if( m_stSetting == DMUSPROD_TIMELINE_SNAP_NONE )
	{
		return;
	}

	long lMeasure, lBeat;
	DMUS_TIMESIGNATURE TimeSig;
	if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->ClocksToMeasureBeat( 0xFFFFFFFF, 0, *plTime, &lMeasure, &lBeat ) )
	 && SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetParam( GUID_TimeSignature, 0xFFFFFFFF, 0, *plTime, NULL, &TimeSig ) ) )
	{
		switch( m_stSetting )
		{
		case DMUSPROD_TIMELINE_SNAP_BAR:
		{
			long lMeasureTime;
			m_pTimeStripMgr->m_pTimeline->MeasureBeatToClocks( 0xFFFFFFFF, 0, lMeasure, 0, &lMeasureTime );

			// If difference is greater than half the measure length, set plTime to the time of the next bar
			if( *plTime - lMeasureTime > (DMUS_PPQ * 2 / TimeSig.bBeat) * TimeSig.bBeatsPerMeasure )
			{
				*plTime = lMeasureTime + (DMUS_PPQ * 4 / TimeSig.bBeat) * TimeSig.bBeatsPerMeasure;
			}
			// Otherwise, set plTime to the time of the first bar
			else
			{
				*plTime = lMeasureTime;
			}
			break;
		}

		case DMUSPROD_TIMELINE_SNAP_BEAT:
		{
			long lBeatTime;
			m_pTimeStripMgr->m_pTimeline->MeasureBeatToClocks( 0xFFFFFFFF, 0, lMeasure, lBeat, &lBeatTime );

			// If difference is greater than half the beat length, set plTime to the time of the next beat
			if( *plTime - lBeatTime > (DMUS_PPQ * 2 / TimeSig.bBeat) )
			{
				*plTime = lBeatTime + (DMUS_PPQ * 4 / TimeSig.bBeat);
			}
			// Otherwise, set plTime to the time of the first beat
			else
			{
				*plTime = lBeatTime;
			}
			break;
		}

		case DMUSPROD_TIMELINE_SNAP_GRID:
		{
			long lGridTime;
			m_pTimeStripMgr->m_pTimeline->MeasureBeatToClocks( 0xFFFFFFFF, 0, lMeasure, lBeat, &lGridTime );

			// Calculate whicih grid we're in
			long lGridDiff;
			lGridDiff = *plTime - lGridTime;
			lGridDiff /= (DMUS_PPQ * 4 / TimeSig.bBeat) / TimeSig.wGridsPerBeat;
			lGridDiff *= (DMUS_PPQ * 4 / TimeSig.bBeat) / TimeSig.wGridsPerBeat;
			lGridTime += lGridDiff;

			// If difference is greater than half the grid length, set plTime to the time of the next grid
			if( *plTime - lGridTime > ((DMUS_PPQ * 2 / TimeSig.bBeat) / TimeSig.wGridsPerBeat) )
			{
				*plTime = lGridTime + ((DMUS_PPQ * 4 / TimeSig.bBeat) / TimeSig.wGridsPerBeat);
			}
			// Otherwise, set plTime to the time of the first grid
			else
			{
				*plTime = lGridTime;
			}
			break;
		}

		default:
			break;
		}
	}
}

void CTimeStrip::UpdateStartEnd( long lNewBegin, long lNewEnd )
{
	SnapTime( &lNewBegin );
	SnapTime( &lNewEnd );

	if( (lNewBegin != m_lBeginSelect) || (lNewEnd != m_lEndSelect) )
	{
		if( lNewBegin > lNewEnd )
		{
			m_lEndSelect = lNewBegin;
			if( m_stSetting != DMUSPROD_TIMELINE_SNAP_NONE )
			{
				m_lEndSelect--;
			}
			lNewBegin = lNewEnd;
			lNewEnd = m_lEndSelect;
		}
		else if( lNewEnd > lNewBegin )
		{
			m_lEndSelect = lNewEnd;
			if( m_stSetting != DMUSPROD_TIMELINE_SNAP_NONE )
			{
				m_lEndSelect--;
			}
		}
		else
		{
			m_lEndSelect = lNewEnd;
		}

		m_lBeginSelect = lNewBegin;

		long lTmpTime;
		m_pTimeStripMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTmpTime );
		if( lTmpTime != m_lBeginSelect )
		{
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, m_lBeginSelect );
		}
		m_pTimeStripMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTmpTime );
		if( lTmpTime != m_lEndSelect )
		{
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, m_lEndSelect );
		}

		ASSERT( m_lBeginSelect <= m_lEndSelect );

		m_pTimeStripMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTmpTime );
		if( lTmpTime != m_lBeginSelect )
		{
			m_fScrollWhenSettingTimeCursor = FALSE;
			m_pTimeStripMgr->m_pTimeline->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, m_lBeginSelect );
			m_fScrollWhenSettingTimeCursor = TRUE;
		}

		m_pTimeStripMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
	}
}

long CTimeStrip::SnapAmount( long lTime )
{
	if( m_stSetting == DMUSPROD_TIMELINE_SNAP_NONE )
	{
		return 1;
	}

	DMUS_TIMESIGNATURE TimeSig;
	if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetParam( GUID_TimeSignature, 0xFFFFFFFF, 0, lTime, NULL, &TimeSig ) ) )
	{
		switch( m_stSetting )
		{
		case DMUSPROD_TIMELINE_SNAP_BAR:
			return ((DMUS_PPQ * 4) / TimeSig.bBeat) * TimeSig.bBeatsPerMeasure;

		case DMUSPROD_TIMELINE_SNAP_BEAT:
			return (DMUS_PPQ * 4) / TimeSig.bBeat;

		case DMUSPROD_TIMELINE_SNAP_GRID:
			return (DMUS_PPQ * 4) / (TimeSig.bBeat * TimeSig.wGridsPerBeat);
		}
	}
	return 1;
}

void CTimeStrip::StartScrollTimer( UINT nScrollTimerID )
{
	if( m_nScrollTimerID )
	{
		StopScrollTimer();
	}

	CWnd *pWnd = GetTimelineCWnd();
	if( pWnd )
	{
		UINT nInterval;

		switch( nScrollTimerID )
		{
			case FIRST_SCROLL_ID:
				nInterval = 350;
				break;
			case SECOND_SCROLL_ID:
				nInterval = 90;
				break;
			default:
				nInterval = 90;
				break;
		}

		if( ::SetTimer( pWnd->GetSafeHwnd(), nScrollTimerID, nInterval, NULL ) )
		{
			m_nScrollTimerID = nScrollTimerID;
		}
	}
}

void CTimeStrip::StopScrollTimer( void )
{
	if( m_nScrollTimerID )
	{
		CWnd *pWnd = GetTimelineCWnd();
		if( pWnd )
		{
			::KillTimer( pWnd->GetSafeHwnd(), m_nScrollTimerID );
		}

		m_nScrollTimerID = 0;
	}
}


CWnd* CTimeStrip::GetTimelineCWnd( void )
{
	CWnd* pWnd = NULL;

	// Get the DC of our Strip
	if( m_pTimeStripMgr->m_pTimeline )
	{
		IOleWindow *pIOleWindow;
		if( SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow ) ) )
		{
			HWND hwnd;
			if( SUCCEEDED( pIOleWindow->GetWindow( &hwnd ) ) )
			{
				pWnd = CWnd::FromHandlePermanent( hwnd );
			}
			pIOleWindow->Release();
		}
	}

	return pWnd;
}

void CTimeStrip::OnTimer( void )
{
	if( m_nScrollTimerID == FIRST_SCROLL_ID )
	{
		StartScrollTimer( SECOND_SCROLL_ID );
	}

	// Get cursor position
	POINT pt;
	::GetCursorPos( &pt );

	// Convert to strip coordinates
	if( FAILED ( m_pTimeStripMgr->m_pTimeline->ScreenToStripPosition( (IDMUSProdStrip *)this, &pt ) ) )
	{
		return;
	}

	// Get current scroll position
	VARIANT var;
	long lHScroll = 0;
	if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
	{
		lHScroll = V_I4(&var);
	}

	// Scroll left?
	if( pt.x < lHScroll 
	&&  lHScroll > 0 )
	{
		// Scroll left
		var.vt = VT_I4;
		V_I4(&var) = max( lHScroll - SCROLL_HORIZ_AMOUNT, 0 );
		m_pTimeStripMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

		OnMouseMove( max(lHScroll - SCROLL_HORIZ_AMOUNT, 0) );
		return;
	}

	// Get rectangle defining strip position
	var.vt = VT_BYREF;
	RECT rectStrip;
	V_BYREF(&var) = &rectStrip;
	if( SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
	{
		// Compute the right side of the display
		long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

		// Compute the maximum scroll position
		m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lTimelineClockLength = V_I4(&var);
		long lMaxXPos;
		if( SUCCEEDED ( m_pTimeStripMgr->m_pTimeline->ClocksToPosition( lTimelineClockLength, &lMaxXPos ) ) )
		{
			long lMaxHScroll = lMaxXPos - (rectStrip.right - rectStrip.left);

			// Scroll right?
			if( pt.x > lMaxScreenPos 
			&&  lHScroll < lMaxHScroll )
			{
				// Scroll right
				var.vt = VT_I4;
				V_I4(&var) = min( lHScroll + SCROLL_HORIZ_AMOUNT, lMaxHScroll);
				m_pTimeStripMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

				OnMouseMove( min(lMaxScreenPos + SCROLL_HORIZ_AMOUNT, lMaxXPos) );
				return;
			}
		}
	}
}

void CTimeStrip::SetSnapTo( DMUSPROD_TIMELINE_SNAP_TO stNew )
{
	if( m_stSetting == stNew )
	{
		return;
	}

	m_stSetting = stNew;

	switch( stNew )
	{
	case DMUSPROD_TIMELINE_SNAP_NONE:
	case DMUSPROD_TIMELINE_SNAP_GRID:
	case DMUSPROD_TIMELINE_SNAP_BEAT:
	case DMUSPROD_TIMELINE_SNAP_BAR:
		// Do nothing
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	UpdateStartEnd( m_lBeginSelect, m_lEndSelect );

	// Notify the strips that the snap-to setting changed
	m_pTimeStripMgr->m_pTimeline->NotifyStripMgrs( GUID_TimelineSetSnapTo, 0xFFFFFFFF, &stNew );
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CTimeStrip::GetData( /* [retval][out] */ void **ppData)
{
	UNREFERENCED_PARAMETER( ppData );
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CTimeStrip::SetData( /* [in] */ void *pData)
{
	UNREFERENCED_PARAMETER( pData );
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CTimeStrip::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	VARIANT varFramework;
	if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &varFramework ) )
	&&	(V_UNKNOWN(&varFramework) != NULL) )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED( V_UNKNOWN(&varFramework)->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pJPS )))
		{
			if( pJPS->IsShowing() == S_OK )
			{
				VARIANT varNode;
				if( SUCCEEDED( m_pTimeStripMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &varNode ) )
				&&	(V_UNKNOWN(&varNode) != NULL) )
				{
					IDMUSProdPropPageObject *pIDMUSProdPropPageObject;
					if( SUCCEEDED( V_UNKNOWN(&varNode)->QueryInterface( IID_IDMUSProdPropPageObject, (void**)&pIDMUSProdPropPageObject ) ) )
					{
						HWND hwnd = ::GetFocus();	// save window to setfocus to afterwards if necessary

						pIDMUSProdPropPageObject->OnShowProperties();

						if(hwnd != ::GetFocus())
						{
							::SetFocus(hwnd);
						}

						pIDMUSProdPropPageObject->Release();
					}
					V_UNKNOWN(&varNode)->Release();
				}
			}
			pJPS->Release();
		}
		V_UNKNOWN(&varFramework)->Release();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTimeStrip::OnRemoveFromPageManager( void)
{
	return E_NOTIMPL;
}
