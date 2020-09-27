// ChordStrip.cpp : Implementation of CChordStrip
#include "stdafx.h"
#include "ChordIO.h"
#include "ChordStripMgr.h"
#include "ChordMgr.h"
#include "..\shared\MusicTimeConverter.h"
#include "ChordStripPPG.h"
#include "SegmentGuids.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STRIP_HEIGHT 20

static const cbKeySize = 2;

extern const TCHAR SharpKey[12*cbKeySize+1];
extern const TCHAR FlatKey[12*cbKeySize +1];

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

CString GetKeyText( int nKeyRoot, int nNumAccidentals, BOOL fDisplayingFlats )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strText2;
	TCHAR tcstrText1[3];
	TCHAR tcstrTmp[10];
	if( fDisplayingFlats )
	{
		_tcsncpy( tcstrText1, &(FlatKey[nKeyRoot * cbKeySize]), cbKeySize );
		tcstrText1[2] = 0;
		if( tcstrText1[1] == _T(' ') )
		{
			tcstrText1[1] = 0;
		}
		strText2.LoadString( IDS_FLAT_PROPPAGE_TEXT );
	}
	else
	{
		_tcsncpy( tcstrText1, &(SharpKey[nKeyRoot * cbKeySize]), cbKeySize );
		tcstrText1[2] = 0;
		if( tcstrText1[1] == _T(' ') )
		{
			tcstrText1[1] = 0;
		}
		strText2.LoadString( IDS_SHARP_PROPPAGE_TEXT );
	}

	// Add the # of accidentals
	_itot( nNumAccidentals, tcstrTmp, 10 );
	strText2 = tcstrTmp + strText2;

	// Format the string to be "(Key=C#/7#'s)"
	CString strReturn;
	AfxFormatString2( strReturn, IDS_KEY_NAME_FORMAT, tcstrText1, strText2 );
	return strReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CChordStrip

/////////////////////////////////////////////////////////////////////////////
// CChordStrip IUnknown
STDMETHODIMP CChordStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	ASSERT( ppv );
    *ppv = NULL;
    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip*) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdStripFunctionBar ))
	{
		*ppv = (IDMUSProdStripFunctionBar*) this;
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
	else if( IsEqualIID( riid, IID_IDMUSProdPropPageObject ))
	{
		*ppv = (IDMUSProdPropPageObject*) this;
	}
	else
	{
		return E_NOTIMPL;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CChordStrip::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CChordStrip::Release(void)
{
	if( 0L == --m_cRef )
	{
//		if(m_pSelectedRegions)
//		{
//			delete m_pSelectedRegions;
//			m_pSelectedRegions = 0;
//		}
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


void CChordStrip::FormatNameForDrawRoutine( CString& strName, CChordItem* pChord )
{
	char strTemp[24];

	strName.Empty();

	if( pChord->Flags() & DMUS_CHORDKEYF_SILENT )
	{
		strName = _T( "(" );
	}

	pChord->RootToString( strTemp, pChord->RootIndex() );
	strcat( strTemp, pChord->Name() );

	strName += strTemp;

	if( pChord->Flags() & DMUS_CHORDKEYF_SILENT )
	{
		strName += _T( ")" );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip IDMUSProdStrip

static bool IntersectsRect(CList<CRect, CRect&>& list, CRect& target)
{
	POSITION pos = list.GetHeadPosition();
	bool rc = false;
	target.NormalizeRect();
	while(pos && !rc)
	{
		CRect regionRect = list.GetNext(pos);
		regionRect.NormalizeRect();
		regionRect &= target;
		rc = !regionRect.IsRectNull();
	}
	return rc;
}

HRESULT	STDMETHODCALLTYPE CChordStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDC	dc;
	CList<CRect, CRect&> chordRectList;
	CList<CRect, CRect&> regionRectList;

	if( m_pChordMgr->m_pTimeline)
	{
		m_pChordMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pChordMgr->m_dwGroupBits, 0, lXOffset );
		if( dc.Attach(hDC) )
		{
			// draw numbers
			// get first number we need to draw
			CRect	rectClip;
			CRect	rectHighlight;
			long	lFirstVisibleMeasure, lFirstVisibleBeat, lPosition;
			long	lPartialVisibleMeasure, lPartialVisibleBeat;
			long	lClocks;

			CString	str;
			CString	strGhost;
			RECT	rectGhost;
			CChordItem* pGhostChord = NULL;

			CChordItem* pChord = m_pChordMgr->m_ChordList.GetHead();

			dc.GetClipBox( &rectClip );
			//rectHighlight = rectClip;
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;

			// find first visible measure for ghosting chords
			bool bGhostChordCovered = false;	// whether or not a real chord exists where ghostchord would be
			m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter	cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasureBeat(lPartialVisibleMeasure, lPartialVisibleBeat, m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasureBeat(lPartialVisibleMeasure, lPartialVisibleBeat, m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
			if(cmtNearestMeasureBeat.Time() < cmtFirstVisible.Time())
			{
				// round up
				DMUS_TIMESIGNATURE	dmTimeSig;
				cmtFirstVisible.GetTimeSig(dmTimeSig, m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
				lFirstVisibleBeat = lPartialVisibleBeat + 1;
				lFirstVisibleMeasure = lPartialVisibleMeasure;
				if(lFirstVisibleBeat > (static_cast<long>(dmTimeSig.bBeatsPerMeasure)-1))
				{
					lFirstVisibleBeat = 0;
					lFirstVisibleMeasure = lPartialVisibleMeasure + 1;
				}
			}
			else
			{
				lFirstVisibleBeat = lPartialVisibleBeat;
				lFirstVisibleMeasure = lPartialVisibleMeasure;
			}
			// find nearest chord before first visible measure/beat
			for(CChordItem* pTestChord = pChord; pTestChord; pTestChord = pTestChord->GetNext())
			{
				if( (pTestChord->Measure() < lFirstVisibleMeasure) 
					|| (pTestChord->Measure() == lFirstVisibleMeasure && pTestChord->Beat() < lFirstVisibleBeat) )
				{
					pGhostChord = pTestChord;
				}
				else
				{
					break;
				}
			}
			if(pGhostChord)
			{
				FormatNameForDrawRoutine( strGhost, pGhostChord );

				// get rect of chord, we may have to truncate it if ghostchord covers part of real chord
				m_pChordMgr->m_pTimeline->MeasureBeatToPosition(m_pChordMgr->m_dwGroupBits, 0, 
												lFirstVisibleMeasure, lFirstVisibleBeat, &lPosition);
				lPosition++;
				CSize csize;
				csize = dc.GetTextExtent(strGhost);
				rectGhost.left = lPosition - lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
				rectGhost.top = 0;
				rectGhost.bottom = STRIP_HEIGHT;
			}


			// we'll draw chord after checking whether or not chord covers it

//			m_pChordMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMgr->m_dwGroupBits, 0, rectClip.left + lXOffset, &lMeasure, NULL );

			while( pChord )
			{
				if( (pChord->Measure() == lFirstVisibleMeasure && pChord->Beat() == lFirstVisibleBeat)
					|| (pChord->Measure() == lPartialVisibleMeasure && pChord->Beat() == lPartialVisibleBeat))
				{
					bGhostChordCovered = true;
				}
				m_pChordMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMgr->m_dwGroupBits, 0, pChord->Measure(), pChord->Beat(), &lPosition );
				lPosition++;

				if(pGhostChord)
				{
					long lTruePos = lPosition - lXOffset;
					// truncate ghost chord's text so that it doesn't cover up a real chord
					if(lTruePos > rectGhost.left && lTruePos < rectGhost.right)
					{
						rectGhost.right = lTruePos;
					}
				}


				// If this chord is not visible (beyond right side of display), break out of the loop
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				FormatNameForDrawRoutine( str, pChord );

				CChordItem *pChordTemp;

				// Get the bounding rect for the beat.
				rectHighlight.left = lPosition - lXOffset;

				// If the next chord covers part of the previous chord, then truncate text
				pChordTemp = pChord->GetNext();
				if( pChordTemp != NULL)
				{
					m_pChordMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMgr->m_dwGroupBits, 0, pChordTemp->Measure(), 
													 pChordTemp->Beat(), &(rectHighlight.right));
					CSize csize;
					csize = dc.GetTextExtent(str);
					if( (rectHighlight.left + csize.cx) > rectHighlight.right)
					{
						dc.DrawText(str, rectHighlight, (DT_LEFT | DT_NOPREFIX));
					}
					else
					{
						dc.TextOut( lPosition - lXOffset, 0, str, strlen(str) );
					}
					
				}
				else
				{
					dc.TextOut( lPosition - lXOffset, 0, str, strlen(str) );
				}

				pChord->m_lDrawPosition = lPosition;
				pChord = pChord->GetNext();
			}

			// make sure the selected chord is shown in its entirety
			pChord = m_pChordMgr->m_ChordList.GetHead();
//			m_pChordMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMgr->m_dwGroupBits, 0, rectClip.left + lXOffset, &lMeasure, NULL );
			bool bSelected = false;
			// Leave rectHighlight alone
			//rectHighlight = rectClip;
			while(  pChord )	
			{
				if(pChord->IsSelected() && m_lBeginSelect == m_lEndSelect )
				{
					long lBeatEndPos;
					m_pChordMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMgr->m_dwGroupBits, 0, 
															pChord->Measure(), pChord->Beat(), &lPosition );
					m_pChordMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMgr->m_dwGroupBits, 0, 
															pChord->Measure(), pChord->Beat()+1, &lBeatEndPos );
					// find extent of text
					CSize csize;
					FormatNameForDrawRoutine( str, pChord );
					csize = dc.GetTextExtent(str);
					long lExtent = lPosition + csize.cx;
					lExtent = lBeatEndPos > lExtent ? lBeatEndPos : lExtent;
					// truncate if a selected chord covers part of text
					CChordItem* pTempChord = pChord->GetNext();
					bool bTruncated = false;
					while(pTempChord && !bTruncated)
					{
						if(pTempChord->IsSelected())
						{
							
							long lrpos;
							m_pChordMgr->m_pTimeline->MeasureBeatToPosition(m_pChordMgr->m_dwGroupBits, 0,
															 pTempChord->Measure(),
															 pTempChord->Beat(),
															 &lrpos);
							lExtent = (lExtent <= lrpos) ? lExtent : lrpos;
							bTruncated = true;
						}
						pTempChord = pTempChord->GetNext();
					}
					rectHighlight.left = lPosition - lXOffset;
					rectHighlight.right = lExtent - lXOffset;
					dc.DrawText(str, rectHighlight,	 (DT_LEFT | DT_NOPREFIX));
					//TRACE("Highlight Rect: %d %d", rectHighlight.left, rectHighlight.right);
//					GrayOutRect( dc.m_hDC, rectHighlight );
					chordRectList.AddTail(CRect(rectHighlight));	// add so that selectedRegions draw won't double invert
				}

				pChord = pChord->GetNext();
			}
			

			// now draw ghost chord
			if(!bGhostChordCovered && pGhostChord)
			{
				COLORREF cr = dc.SetTextColor(RGB(168,168,168));	// light grey
				dc.DrawText(strGhost, &rectGhost, (DT_LEFT | DT_NOPREFIX));
				dc.SetTextColor(cr);
			}

			
			// Highlight the selected range if there is one.
			if( m_bGutterSelected && m_lBeginSelect != m_lEndSelect )
			{
				m_pChordMgr->m_pTimeline->ClocksToPosition( m_lBeginSelect > m_lEndSelect ? m_lEndSelect : m_lBeginSelect,
											 &(rectHighlight.left));
				m_pChordMgr->m_pTimeline->ClocksToPosition( m_lBeginSelect > m_lEndSelect ? m_lBeginSelect : m_lEndSelect,
											 &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;
				// Invert it.
				GrayOutRect( dc.m_hDC, rectHighlight );
			}
			else if(m_pSelectedRegions)
			{
				// Combine region rects with chord rects
				POSITION pos = m_pSelectedRegions->GetHeadPosition();
				while(pos)
				{
					CSelectedRegion* psr = m_pSelectedRegions->GetNext(pos);
					rectHighlight.left = psr->BeginPos(m_pChordMgr->m_pTimeline) - lXOffset;
					rectHighlight.right = psr->EndPos(m_pChordMgr->m_pTimeline) - lXOffset;
					POSITION pos1, pos2;
					pos1 = chordRectList.GetHeadPosition();
					while(pos1)
					{
						pos2 = pos1;
						CRect chordRect = chordRectList.GetNext(pos1);
						CRect tmp;
						tmp.IntersectRect(chordRect, rectHighlight);
						if(!tmp.IsRectEmpty())
						{
							rectHighlight |= chordRect;
							chordRectList.RemoveAt(pos2);
						}
					}
					regionRectList.AddTail(rectHighlight);
				}
			}
			
			// last step may have some overlapping region rects -- combine them
			POSITION pos = regionRectList.GetHeadPosition();
			while(pos)
			{
				POSITION tmp = pos;
				CRect rect = regionRectList.GetNext(pos);
				bool canCombine = false;
				POSITION tmp1 = pos;
				while(tmp1 && !canCombine)
				{
					CRect& rectNext = regionRectList.GetNext(tmp1);
					if(!(rect & rectNext).IsRectEmpty())
					{
						canCombine = true;
						rectNext |= rect;
						regionRectList.RemoveAt(tmp);
					}
				}
			}
			// draw inverted regions
			pos = regionRectList.GetHeadPosition();
			while(pos)
			{
				CRect rect = regionRectList.GetNext(pos);
				GrayOutRect( dc.m_hDC, rect );
			}
/*
			pos = chordRectList.GetHeadPosition();
			while(pos)
			{
				CRect rect = chordRectList.GetNext(pos);
				GrayOutRect( dc.m_hDC, rect );
			}
*/
			dc.Detach();
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		pvar->vt = VT_BOOL;
//		V_BOOL(pvar) = TRUE; // this is really FALSE, but we'll make it true for now for testing
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
	case SP_MINHEIGHT:
	case SP_MAXHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = STRIP_HEIGHT;
		break;
	case SP_NAME:
		{
			BSTR bstr;
			CString str;
			CString strTemp;
			TCHAR buf[256];

			HINSTANCE hinst = _Module.GetModuleInstance();

			// Set strTemp to the correct string, according to m_bPatternEditorMode
			if( m_pChordMgr->m_bPatternEditorMode == 1 )
			{
				::LoadString( hinst, IDS_MIDI_MODE_TEXT, buf, 256 );
				strTemp = buf;
			}
			else if( m_pChordMgr->m_bPatternEditorMode == 2 )
			{
				::LoadString( hinst, IDS_FUNCTION_MODE_TEXT, buf, 256 );
				strTemp = buf;
			}

			// Check if we're in the segment designer
			if( m_pChordMgr->m_pIDMTrack )
			{
				// Yes - load the 'Chord' text
				::LoadString( hinst, IDS_CHORDS_TEXT, buf, 256 );

				// If the m_bPatternEditorMode text is empty
				if( strTemp.IsEmpty() )
				{
					// Just use the 'Chord' text
					strTemp = buf;
				}
				else
				{
					// Use the m_bPatternEditorMode text + the 'Chord' text
					strTemp += CString(_T(" ")) + CString(buf);
				}

				// Format the name to include the group bits at the front
				str = GetName( m_pChordMgr->m_dwGroupBits, strTemp );
			}
			else
			{
				// No, we're in the pattern editor
				::LoadString( hinst, IDS_PATTERN_EDITOR_TEXT, buf, 256 );

				// Use the m_bPatternEditorMode text + the 'Chord for Composition' text
				str = strTemp + _T(" ") + buf;
			}

			// Add the Key to the name
			str += _T(" ") + GetKeyText(m_pChordMgr->m_nKeyRoot, m_pChordMgr->m_nNumAccidentals, m_pChordMgr->m_fDisplayingFlats);
			
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
		if( m_pChordMgr )
		{
			m_pChordMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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

HRESULT STDMETHODCALLTYPE CChordStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	long measure, beat;
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
			m_lBeginSelect = V_I4( &var );
			// round down to nearest beat
			m_pChordMgr->m_pTimeline->ClocksToMeasureBeat(m_pChordMgr->m_dwGroupBits, 0, m_lBeginSelect,
															&measure, &beat);
			m_pChordMgr->m_pTimeline->MeasureBeatToClocks(m_pChordMgr->m_dwGroupBits, 0,
															measure, beat, &m_lBeginSelect);
		}
		else
		{
			long clocks;
			clocks = m_lEndSelect = V_I4( &var );
			// clip to nearest beat
			m_pChordMgr->m_pTimeline->ClocksToMeasureBeat(m_pChordMgr->m_dwGroupBits, 0, m_lEndSelect,
															&measure, &beat);
			m_pChordMgr->m_pTimeline->MeasureBeatToClocks(m_pChordMgr->m_dwGroupBits, 0,
															measure, beat, &m_lEndSelect);
			if(clocks > m_lEndSelect)
			{
				beat++;
				m_pChordMgr->m_pTimeline->MeasureBeatToClocks(m_pChordMgr->m_dwGroupBits, 0,
															measure, beat, &m_lEndSelect);
			}
		}

		if( m_bSelecting )
		{
			break;
		}

		// Initialize anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( 0 );

		if( m_lBeginSelect == m_lEndSelect )
		{	
			m_pSelectedRegions->Clear();
			SelectChordsInSelectedRegions();

			m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}
		if( m_bGutterSelected )
		{
			m_pSelectedRegions->Clear();
			SelectChordsInSelectedRegions();
			m_pChordMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
			// save point of begin select for pasting from timeline
			m_pChordMgr->m_pTimeline->ClocksToPosition(m_lBeginSelect, &m_lXPos);
		}
		else
		{
			m_pChordMgr->UnselectChords();
		}
		m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pChordMgr->m_pPropPageMgr != NULL )
		{
			m_pChordMgr->m_pPropPageMgr->RefreshData();
		}
		break;
	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);
		if( m_lBeginSelect == m_lEndSelect )
		{	
			m_pSelectedRegions->Clear();
			SelectChordsInSelectedRegions();
			m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			if(m_bGutterSelected)
			{
				long lTime;
				// save point of begin select for pasting from timeline
				m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime);
				m_pChordMgr->m_pTimeline->ClocksToPosition(lTime, &m_lXPos);
			}
			break;
		}
		if( m_bGutterSelected )
		{
			long lTime;
			m_pSelectedRegions->Clear();
			SelectChordsInSelectedRegions();
			m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect);
			m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect);
			m_pChordMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
			// save point of begin select for pasting from timeline
			m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime);
			m_pChordMgr->m_pTimeline->ClocksToPosition(lTime, &m_lXPos);
		}
		else
		{
			m_pSelectedRegions->Clear();
			SelectChordsInSelectedRegions();
		}
		m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pChordMgr->m_pPropPageMgr != NULL )
		{
			m_pChordMgr->m_pPropPageMgr->RefreshData();
		}
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

CChordItem *CChordStrip::GetChordFromPoint( long lPos )
{
	long lMeasure, lBeat;


	if( m_pChordMgr->m_pTimeline )
	{
		CChordItem *pChord = m_pChordMgr->m_ChordList.GetHead();
		if ( SUCCEEDED(	m_pChordMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat )))
		{
			for( ; pChord != NULL; pChord = pChord->GetNext())
			{
				if ( pChord->Measure() == lMeasure)
				{
					if (pChord->Beat() == lBeat)
					{
						break;
					}
				}
				else if ( pChord->Measure() > lMeasure )
				{
					pChord = NULL;
					break;
				}
			}
		}
		return pChord;
	}
	return NULL;
}


HRESULT STDMETHODCALLTYPE CChordStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	CChordItem* pChord = NULL;
	VARIANT var;
	static slXPos;

	if( m_pChordMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_KEYDOWN:
		{
			int nVirtKey = (int)wParam;
			unsigned long lKeyData = lParam;
			if(ProcessKey(nVirtKey, lKeyData))
			{
				m_nLastEdit = IDS_UNDO_DELETE;
				m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
				m_pChordMgr->UpdateDirectMusic( TRUE );
			}
			hr = S_OK;
			break;
		}
	case WM_LBUTTONDOWN:
		// capture mouse so we get the LBUTTONUP message as well
		// the timeline will release the capture when it receives the
		// LBUTTONUP message
		GetTimelineCWnd()->SetFocus();
		slXPos = lXPos;
		var.vt = VT_BOOL;
		V_BOOL(&var) = TRUE;
		m_pChordMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
		UnselectGutterRange();
		m_fShowChordProps = TRUE;

		pChord = GetChordFromPoint( lXPos );

		if(wParam & MK_SHIFT)
		{
			m_pSelectedRegions->ShiftAddRegion(lXPos);
			SelectChordsInSelectedRegions();
		}
		else
		{
			if( pChord )					// drag/drop ops
			{
				if(wParam & MK_CONTROL)
				{
					// only want to do this once--when a chord is selected it goes through twice, once
					// so that selection shows up, then a second time to do drag drop.  We only want to toggle
					// if (a) no chord was selected or (b) no drag/drop done.
					m_pSelectedRegions->DelayToggle(lXPos);
					SelectChordsInSelectedRegions();
	//				return S_OK;
				}
				else if(!pChord->IsSelected())
				{
					m_pSelectedRegions->Clear();
					m_pSelectedRegions->AddRegion(lXPos);
					SelectChordsInSelectedRegions();
					m_pChordMgr->m_SelectedChord = *pChord;
				}
				m_bLeftMouseButtonDown = true;
				m_dwStartDragButton = (unsigned long)wParam;
				m_startDragPosition = lXPos;
			}
			else
			{
				if(wParam & MK_CONTROL)
				{
					m_pSelectedRegions->ToggleRegion(lXPos);
					SelectChordsInSelectedRegions();
				}
				else
				{
					m_pSelectedRegions->Clear();
					m_pSelectedRegions->AddRegion(lXPos);
					SelectChordsInSelectedRegions();
				}
			}

			// Set anchor for future shift-select operations
			m_pSelectedRegions->SetShiftSelectAnchor( lXPos );
		}

		if(pChord && pChord->IsSelected())
		{
			m_pChordMgr->m_SelectedChord = *pChord;
			m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
		}
		else if(m_pChordMgr->IsSelected())
		{
			CChordItem* pItem = m_pChordMgr->FirstSelectedChord();
			m_pChordMgr->m_SelectedChord = *pItem;
			m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
		}
		else
		{
			m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
		}

		// Todo, replace NULL by actual area invalidated (in strip coordinates)
		m_pChordMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		if( m_pChordMgr->m_pPropPageMgr )
		{
			m_pChordMgr->m_pPropPageMgr->RefreshData();
		}
		m_pChordMgr->OnShowProperties();
		break;
	case WM_RBUTTONDOWN:
		UnselectGutterRange();
		// Get the item at the mouse click.
		// TODO: modify code to work with CListSelectedRegion
	    pChord = GetChordFromPoint( lXPos );
		slXPos = lXPos;
		if( !(wParam & MK_CONTROL)
		&&  !(wParam & MK_SHIFT) )
		{
			if( pChord )
			{
				if( !pChord->IsSelected() )
				{
					m_pChordMgr->UnselectChords();
					pChord->SetSelected( TRUE );
					m_pChordMgr->m_SelectedChord = *pChord;
					m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
					m_pSelectedRegions->Clear();
					m_pSelectedRegions->AddRegion(lXPos);
				}
				else
				{
					// chord is selected already, do nothing
				}
			}
			else
			{
				// if region not selected, select it, otherwise do nothing
				long m, b;
				m_pChordMgr->m_pTimeline->PositionToMeasureBeat(m_pChordMgr->m_dwGroupBits, 0, lXPos, &m, &b);
				if(!m_pSelectedRegions->Contains(m,b))
				{
					m_pChordMgr->UnselectChords();
					m_pSelectedRegions->Clear();
					m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
					m_pSelectedRegions->AddRegion(lXPos);
				}
				else
				{
					// region is selected already, do nothing
				}
			}
		}
		else
		{
			// control or shift
		}


		m_pChordMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
		m_fShowChordProps = TRUE;
		if( m_pChordMgr->m_pPropPageMgr )
		{
			m_pChordMgr->m_pPropPageMgr->RefreshData();
		}
		m_pChordMgr->OnShowProperties(); 
		break;
	case WM_RBUTTONUP:
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

		m_lXPos = lXPos;
		hr = DoRightClickMenu( m_pChordMgr->m_pTimeline, pt );
		break;
	case WM_COMMAND:
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			hr = ShowPropertySheet(m_pChordMgr->m_pTimeline);
			if (m_fShowChordProps )
			{
				// Change to the chord property page
				m_pChordMgr->OnShowProperties();
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
		case ID_HIDE_FROM_RHYTHM:
		{
			short nState = m_pChordMgr->GetSelectedHideState();
			for( CChordItem* pChord = m_pChordMgr->m_ChordList.GetHead();  pChord;  pChord = pChord->GetNext() )
			{
				if( pChord->IsSelected() )
				{
					if( nState == HIDE_ALL_ON )
					{
						pChord->Flags() &= ~DMUS_CHORDKEYF_SILENT;
					}
					else
					{
						pChord->Flags() |= DMUS_CHORDKEYF_SILENT;
					}
				}
			}
			m_nLastEdit = IDS_UNDO_HIDE_FROM_RHYTHM;
			m_pChordMgr->UpdateDirectMusic( TRUE );
			m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			hr = S_OK;
			break;
		}
		case IDM_CONSTANT_MIDI:
			m_pChordMgr->m_bPatternEditorMode = 1;
			m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordParam, m_pChordMgr->m_dwGroupBits, NULL );
			hr = S_OK;
			break;
		case IDM_CONSTANT_FUNCTION:
			m_pChordMgr->m_bPatternEditorMode = 2;
			m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordParam, m_pChordMgr->m_dwGroupBits, NULL );
			hr = S_OK;
			break;
		default:
			break;
		}
		break;

	case WM_LBUTTONUP:
		{
			pChord = GetChordFromPoint( lXPos );
			if(pChord)
			{
				if( !(wParam & MK_CONTROL)
				&&  !(wParam & MK_SHIFT) )
				{
					m_pSelectedRegions->Clear();
					m_pSelectedRegions->AddRegion(lXPos);
				}
				else
				{
					m_pSelectedRegions->CompleteToggle();
				}
//				m_pSelectedRegions->CompleteDeselect();
				SelectChordsInSelectedRegions();
				// make sure if no chords are selected that prop page shows invalid
				if(!m_pChordMgr->IsSelected())
				{
					m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
					if( m_pChordMgr->m_pPropPageMgr )
					{
						m_pChordMgr->m_pPropPageMgr->RefreshData();
					}
//					m_pChordMgr->OnShowProperties();
				}
				else
				{
					// adjust property page to show first selected chord
					m_pChordMgr->m_SelectedChord = *(m_pChordMgr->FirstSelectedChord());
					m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
					
				}
				m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			m_bLeftMouseButtonDown = false;
			m_lXPos = lXPos;
//		UnselectGutterRange();
			m_bMouseSelect = false;
			hr = S_OK;
			// if nothing is selected, invalidate prop page
			if(m_pSelectedRegions->IsEmpty())
			{
				m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
			}
			m_fShowChordProps = TRUE;
			if( m_pChordMgr->m_pPropPageMgr )
			{
				m_pChordMgr->m_pPropPageMgr->RefreshData();
			}
			m_pChordMgr->OnShowProperties(); 
		}
		break;
	case WM_MOUSEMOVE:
		if(m_bLeftMouseButtonDown)
		{
			if(abs(slXPos - lXPos) <= 3)
				return S_OK;	// delay till user actually moves
			hr = StartDragDrop(m_pChordMgr->m_pTimeline, m_startDragPosition);
			if(hr != S_OK)
			{
				m_lXPos = lXPos;	// because drag drop eats mouse up
				m_pSelectedRegions->CompleteToggle();
//				m_pSelectedRegions->CompleteDeselect();
				SelectChordsInSelectedRegions();
				// make sure if no chords are selected that prop page shows invalid
				if(!m_pChordMgr->IsSelected())
				{
					m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
					if( m_pChordMgr->m_pPropPageMgr )
					{
						m_pChordMgr->m_pPropPageMgr->RefreshData();
					}
//					m_pChordMgr->OnShowProperties();
				}		
				m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
		}
		m_bLeftMouseButtonDown = false;
		break;
	case WM_CREATE:
		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect );
		m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect );

		if( !m_cfChordList )
		{
			m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		}

		if(!m_pSelectedRegions)
			m_pSelectedRegions = new CListSelectedRegion(m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
		else
		{
			m_pSelectedRegions->m_pTimeline = m_pChordMgr->m_pTimeline;
			m_pSelectedRegions->m_dwGroupbits = m_pChordMgr->m_dwGroupBits;
		}
		break;
	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip IDMUSProdTimelineEdit

HRESULT CChordStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	hr = CanCut();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	hr = Copy(pITimelineDataObject);
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}

HRESULT CChordStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
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
	long offset, endRegion;
	m_pSelectedRegions->GetSpan(offset, endRegion);
	hr = m_pChordMgr->SaveSelectedChords( pStreamCopy, offset );
	/* not necessary, unless you want to save regions in clipboard
	if(hr == S_OK)
	{
		hr = SaveSelectedRegions(pStreamCopy, offset, TRUE );
	}
	*/
	if( FAILED( hr ))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfChordList, pStreamCopy );
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
		hr = m_pChordMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lRegionStart, lRegionEnd;
		m_pSelectedRegions->GetSpan(lRegionStart, lRegionEnd);
		hr = pITimelineDataObject->SetBoundaries(lRegionStart, (lRegionEnd - 1));

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfChordList, pStreamCopy );
		pStreamCopy->Release();
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}

		// get the new IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export(&pIDataObject);

		// Release the ITimelineDataObject
		pITimelineDataObject->Release();

		// Exit if the Export failed
		if(FAILED(hr))
		{
			return E_UNEXPECTED;
		}

		// Send the IDataObject to the clipboard
		hr = OleSetClipboard( pIDataObject );
		if( hr != S_OK )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// If we already have a CopyDataObject, release it
		if(m_pChordMgr->m_pCopyDataObject)
		{
			m_pChordMgr->m_pCopyDataObject->Release();
		}

		// set m_pCopyDataObject to the object we just copied to the clipboard
		m_pChordMgr->m_pCopyDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pCopyDataObject->AddRef
	}

	return hr;
}


HRESULT CChordStrip::PasteReplace(CChordList& list, CMusicTimeConverter& cmtStart, CMusicTimeConverter& cmtEnd, BOOL& fChanged)
//
// this algorithm is slow -- we make several passes, but it is clear and lists should be relatively short
//
{
//	ASSERT(cmtStart <= cmtEnd);
	// add regions of chords to selected regions to be used as a mask to delete overwritten chords
	CChordItem* pChord;
	m_pChordMgr->m_ChordList.ClearSelections();
	// select chords in same time range
	pChord = m_pChordMgr->m_ChordList.GetHead();
	for(; pChord; pChord = pChord->GetNext())
	{
		CMusicTimeConverter cmtChord(pChord->Measure(), pChord->Beat(), 
										m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
		if(cmtChord > cmtEnd)
			break;		// done, past selection range
		else if(cmtChord < cmtStart)
			continue;	// not to selection range yet
		else
		{
			pChord->SetSelected(TRUE);
			fChanged = TRUE; // This chord will be deleted below
		}
	}
	// delete selected chords
	m_pChordMgr->DeleteSelectedChords();
	// now insert list
	for(pChord = list.RemoveHead(); pChord; pChord = list.RemoveHead())
	{
		pChord->SetNext(0);
		pChord->SetSelected(TRUE);		// make sure chord is selected
//		pChord->m_dwUndeterminined |= UD_DROPSELECT; // prevent property page logic from mucking with dropped chords
		m_pChordMgr->m_ChordList.InsertByAscendingTime(pChord);
		fChanged = TRUE;
	}
	return S_OK;
}


HRESULT CChordStrip::PasteMerge(CChordList& list, CMusicTimeConverter& cmtStart, CMusicTimeConverter& cmtEnd, BOOL& fChanged)
//
// this algorithm is slow -- we make several passes, but it is clear and lists should be relatively short
//
{
//	ASSERT(cmtStart < cmtEnd);
	m_pChordMgr->m_ChordList.ClearSelections();
	CChordItem* pChord;
	// now insert list
	for(pChord = list.RemoveHead(); pChord; pChord = list.RemoveHead())
	{
		pChord->SetNext(0);
		pChord->SetSelected(TRUE);		// make sure chord is selected
//		pChord->m_dwUndeterminined |= UD_DROPSELECT; // prevent property page logic from mucking with dropped chords
		m_pChordMgr->m_ChordList.InsertByAscendingTime(pChord, true);
		fChanged = TRUE;
	}
	return S_OK;
}

HRESULT CChordStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste)
{
	if( pITimelineDataObject == NULL )
	{
		return E_NOTIMPL;
	}

	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfChordList ) == S_OK )
	{
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfChordList, &pIStream)))
		{
			// declare a temp list and write chords into it
			CChordList list;
			// Set PatternEditorMode so DMUS_CHORDKEYF_SILENT flag turned off when chords pasted into Pattern editor
			list.SetPatternEditorMode( m_pChordMgr->m_bPatternEditorMode );	
			list.Load(pIStream);
			pIStream->Release();
			if(bDropNotEditPaste && (list.GetHead() == NULL))
			{
				// don't drop an empty list, note don't check listRegions since can't have paste from regions only
				// However, do paste an empty list
				hr = E_FAIL;
				goto Leave;
			}
			// update times from position of drop
			
			if( m_pChordMgr->m_pTimeline )
			{
				// get offset of first chord's drag position to normalize all chords to offset zero:
				long clocksDragOffset ;
				// convert drop postition to beats
				long position = pt.x;
				long clocksDropOffset;
				long firstClock = 0, lastClock = 0;
				long m,b;
				long offset;
				if(SUCCEEDED(m_pChordMgr->m_pTimeline->PositionToMeasureBeat(m_pChordMgr->m_dwGroupBits, 0, position, &m, &b)))
				{
						// convert measure/beat to clcks
					CMusicTimeConverter cmt(m,b, m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
					if(bDropNotEditPaste)
					{
						clocksDropOffset = cmt;
						// check that we've actually "moved" somewhere.  If not cancel the drop
						if(m_pISourceDataObject == m_pITargetDataObject)
						{
							// we're in our strip
							long deltapos = abs(pt.x - m_startDragPosition);
							/*
							long m,b;
							m_pChordMgr->m_pTimeline->PositionToMeasureBeat(m_pChordMgr->m_dwGroupBits, 0, deltapos, &m, &b);
							if(m == 0 && b < 1)
							*/
							if(deltapos < 1)
							{
								// cancel drop
								hr = E_FAIL;
								list.ClearList();
								goto Leave;
							}
						}
						// offset measure/beat in chords to be drop
						// first find selected chord
						CChordItem* pChord = NULL;
						for(pChord = list.GetHead(); pChord && !pChord->IsSelected(); pChord = pChord->GetNext()) 
						{
							continue;
						}
						if(pChord)
						{
							// this is selected chord: use it as the drag offset
							// convert its measure/beat to beats
							CMusicTimeConverter cmt1(pChord->Measure(), pChord->Beat(), 
														m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
							clocksDragOffset = cmt1;
						}
						else
						{
							// no chord selected, list came from somewhere else, dragoffset = 0
							clocksDragOffset = 0;
						}
						offset = clocksDropOffset - clocksDragOffset;
						if(offset < 0)
						{
							offset = 0;
						}
					}
					else
					{
						offset = cmt;
					}
					TIMELINE_PASTE_TYPE pasteType;
					long lStartTime, lEndTime;
					// get timespan of pasted area
					if(bDropNotEditPaste)
					{
						// need to set boundaries and paste type from internal data
						m_pChordMgr->GetSpanOfChordList(lStartTime, lEndTime, &list);
						pasteType = TL_PASTE_MERGE;
						hr = S_OK;
					}
					else
					{
						hr = pITimelineDataObject->GetBoundaries(&lStartTime, &lEndTime);
						hr = m_pChordMgr->m_pTimeline->GetPasteType(&pasteType);
						lEndTime -= lStartTime;
						lStartTime = 0;
					}
					ASSERT(hr==S_OK);
					m_pChordMgr->NormalizeChordList(list, offset);
					// normalized time converters
					CMusicTimeConverter cmtStart(lStartTime + offset);
					CMusicTimeConverter cmtEnd( lEndTime + offset );
					BOOL fChanged = FALSE;
					if(hr == S_OK)
					{
						// Just did this.
						//if(SUCCEEDED(hr))
						{
							switch (pasteType)
							{
							case TL_PASTE_OVERWRITE:
								hr = PasteReplace(list, cmtStart, cmtEnd, fChanged);
								break;
							case TL_PASTE_MERGE:
								// no regions, since regions are transparent in a merge
								hr = PasteMerge(list, cmtStart, cmtEnd, fChanged);
								break;
							default:
								hr = E_UNEXPECTED;
								break;
							}
							if(SUCCEEDED(hr))
							{
								SelectRegionsFromSelectedChords();
							}
						}
//						m_pSelectedRegions->Clear();	// no selected "space" after paste
					}

					if( m_nStripIsDragDropSource )
					{
						// Drag/drop Target and Source are the same StyleRef strip
						m_nStripIsDragDropSource = 2;
					}
					else if ( fChanged )
					{
						if(!bDropNotEditPaste)
						{
							// make sure chord is in prop page
							CChordItem* pChord = GetChordFromPoint( pt.x );
							if(!pChord)
							{
								// first chord will do
								pChord = m_pChordMgr->m_ChordList.GetHead();
							}
							if(pChord)
							{
								m_pChordMgr->m_SelectedChord = *pChord;
								m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
								m_pChordMgr->OnShowProperties();
								if( m_pChordMgr->m_pPropPageMgr )
								{
									m_pChordMgr->m_pPropPageMgr->RefreshData();
								}
							}

						}
						m_nLastEdit = IDS_UNDO_PASTE;
						m_pChordMgr->UpdateDirectMusic( TRUE );
						m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
					}

				}
				else
				{
					ASSERT(0);
				}
			}

		}
	}
Leave:	
	return hr;
	
}




HRESULT CChordStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMgr->m_pTimeline != NULL );
	if( m_pChordMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
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
		hr = m_pChordMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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

	// Paste
	POINTL pt;
	pt.x = m_lXPos;
	pt.y = 0;
	hr = PasteAt(pITimelineDataObject, pt, false);
	if(SUCCEEDED(hr))
	{
		m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	}
	pITimelineDataObject->Release();

	return hr;
}

HRESULT CChordStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	long	lClocks;
	long	lBeat;
	long	lMeasure;
	CChordItem* pChord = NULL;

	// Unselect all items in this strip 
	m_pChordMgr->UnselectChords();
	
	// Unselect items in other strips
	UnselectGutterRange();

	if( m_pChordMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	hr = m_pChordMgr->m_pTimeline->PositionToClocks( m_lXPos, &lClocks );
	ASSERT( SUCCEEDED( hr ));
	hr = m_pChordMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMgr->m_dwGroupBits, 0, lClocks, &lMeasure, &lBeat );
	if(SUCCEEDED(hr))
	{
		CChordItem* pChord = new CChordItem;
		if(pChord == NULL)
		{
			hr = E_OUTOFMEMORY;
		}
		else
		{
			DWORD dwPos = 1;
			DWORD dwChordPattern = 1;
			for( int i = 1 ;  i < 24 ;  i++ )
			{
				if( m_pChordMgr->m_dwScale & (1 << i) )
				{
					dwPos++;

					if( dwPos == 3
					||  dwPos == 5 )
					{
						dwChordPattern |= (1 << i);
					}

					if( dwPos >= 5 )
					{
						break;
					}
				}
			}

			for( i = 0;  i < DMPolyChord::MAX_POLY;  i++ )
			{
				pChord->SubChord(i)->ChordRoot() = m_pChordMgr->m_nKeyRoot + 12;
				while( pChord->SubChord(i)->ChordRoot() > 24 )
				{
					pChord->SubChord(i)->ChordRoot() -= 12;
				}
				pChord->SubChord(i)->ChordPattern() = dwChordPattern;
				pChord->SubChord(i)->ScaleRoot() = m_pChordMgr->m_nKeyRoot;
				pChord->SubChord(i)->ScalePattern() = m_pChordMgr->m_dwScale;
				pChord->SubChord(i)->UseFlat() = m_pChordMgr->m_fDisplayingFlats;
			}
			strcpy(pChord->Name(), "M");
			pChord->m_dwIndex = 0;	// probably not needed anymore
			pChord->SetSelected(TRUE);
			m_pChordMgr->m_SelectedChord = *pChord;
			pChord->Measure() = static_cast<short>(lMeasure);
			pChord->Beat() = static_cast<BYTE>(lBeat);
			m_pChordMgr->m_ChordList.InsertByAscendingTime(pChord);
			m_pChordMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

			SelectRegionsFromSelectedChords();

			ShowPropertySheet(m_pChordMgr->m_pTimeline);
			m_pChordMgr->OnShowProperties();
			if( m_pChordMgr->m_pPropPageMgr )
			{
				m_pChordMgr->m_pPropPageMgr->RefreshData();
			}
			m_nLastEdit = IDS_UNDO_PASTE;
			m_pChordMgr->UpdateDirectMusic( TRUE );
		}
	}

	return hr;
}

HRESULT CChordStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	HRESULT			hr = E_NOTIMPL;

	/* This may be part of a cut operation.  You can cut empty space, but you can't delete it.
	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}
	*/

	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMgr->m_pTimeline != NULL );
	if( m_pChordMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pChordMgr->DeleteSelectedChords();
	m_pSelectedRegions->Clear();
	m_pChordMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	m_pChordMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;


	if( m_pChordMgr->m_pPropPageMgr )
	{
		m_pChordMgr->m_pPropPageMgr->RefreshData();
	}

	m_nLastEdit = IDS_UNDO_DELETE;
	m_pChordMgr->UpdateDirectMusic( TRUE );
	return S_OK;
}

HRESULT CChordStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pSelectedRegions->SelectAll();
	SelectChordsInSelectedRegions();


	CChordItem* pChord = m_pChordMgr->FirstSelectedChord();
	if(pChord)
	{
		m_pChordMgr->m_SelectedChord = *pChord;
		m_pChordMgr->m_ChordList.PreparePropChord( &m_pChordMgr->m_SelectedChord );
	}
	if( pChord )
	{
		m_fShowChordProps = TRUE;
		m_pChordMgr->OnShowProperties();
	}

	m_pChordMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	
	return S_OK;
}

HRESULT CChordStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pChordMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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

HRESULT CChordStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pChordMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pChordMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pChordMgr->IsSelected() ? S_OK : S_FALSE;
}

HRESULT CChordStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;

	ASSERT( m_pChordMgr != NULL );
	if( m_pChordMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfChordList );
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
			if( SUCCEEDED( m_pChordMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfChordList);
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

HRESULT CChordStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_lXPos < 0 )
	{
		return S_FALSE;
	}

	CPropChord* pChord = GetChordFromPoint( m_lXPos );
	VARIANT var;
	
	if( m_pChordMgr == NULL
	||  m_pChordMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pChordMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
	long len = V_I4(&var);
	long clocks;
	m_pChordMgr->m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &clocks);

	if( clocks >= len
	||  pChord != NULL )
	{
		return S_FALSE;
	}
	else
	{
		return S_OK;
	}
}

HRESULT CChordStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pChordMgr->IsSelected() ? S_OK : S_FALSE;
}

HRESULT CChordStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pChordMgr->m_ChordList.IsEmpty() && !m_bGutterSelected ? S_FALSE : S_OK;
}

// IDropSource Methods
HRESULT CChordStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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

HRESULT CChordStrip::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

HRESULT	CChordStrip::CreateDataObject(IDataObject** ppIDataObject, long position)
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

	// Save Selected Chords into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pChordMgr->m_pIDMUSProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		CChordItem* pChordAtDragPoint = GetChordFromPoint( position );

		// mark the chords as being dragged: this used later for deleting chords in drag move
		m_pChordMgr->MarkSelectedChords(UD_DRAGSELECT);
		long offset;
		if( SUCCEEDED ( m_pChordMgr->SaveSelectedChords( pIStream, pChordAtDragPoint, TRUE, &offset ) ) )
		{
			// uncomment to save regions in clipboard
//			if( SUCCEEDED (SaveSelectedRegions(pIStream, offset, TRUE) ) )
//			{
				// Place CF_CHORDLIST into CDllJazzDataObject
				if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfChordList, pIStream ) ) )
				{
					hr = S_OK;
				}
//			}
		}

		pIStream->Release();
	}


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
// IDropTarget CChordStrip::DragEnter

HRESULT CChordStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//TRACE("CChordStrip Drag Enter!");
	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

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
// IDropTarget CChordStrip::DragOver

HRESULT CChordStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//TRACE("CChordStrip Drag OVER!");
	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

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
// IDropTarget CChordStrip::DragLeave

HRESULT CChordStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//TRACE("CChordStrip Drag Leave!");

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CChordStrip::Drop

HRESULT CChordStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		// BUGBUG: Error messages?
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pChordMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				hr = PasteAt( pITimelineDataObject, pt, true );
				if( SUCCEEDED ( hr ) )
				{
					*pdwEffect = m_dwOverDragEffect;
				}
			}
			pITimelineDataObject->Release();
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}



CWnd* CChordStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pChordMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pChordMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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


HRESULT CChordStrip::CanPasteFromData(IDataObject* pIDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList ) ))
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}


inline BOOL IsDeleteKey(int nVirtKey, unsigned long lKeyData)
{
	DWORD dwDelete = 0x01000000;
	if((nVirtKey == 53  || nVirtKey == 46) && (lKeyData & dwDelete))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CChordStrip::ProcessKey(int nVirtKey, unsigned long lKeyData)
{
	BOOL b = FALSE;
	if(IsDeleteKey(nVirtKey, lKeyData))
	{
		m_pChordMgr->DeleteSelectedChords();
		b = TRUE;
	}
	return b;
}


HRESULT CChordStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
{
	//GiveTimelineFocus();
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
				pIPropSheet->Release();
			}
			punk->Release();
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CChordStrip::FBDraw

HRESULT CChordStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip::FBOnWMMessage

HRESULT CChordStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_lXPos = -1;
		m_fShowChordProps = FALSE;
		OnShowProperties();
		break;
	case WM_RBUTTONUP:
		// Display a right-click context menu.
		POINT pt;
		// Get the cursor position (To put the menu there)
		if( !GetCursorPos( &pt ) )
		{
			hr = E_UNEXPECTED;
			break;
		}

		if( m_pChordMgr->m_pTimeline )
		{
			hr = DoRightClickMenu( m_pChordMgr->m_pTimeline, pt );
		}
		break;

	default:
		break;
	}
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CChordStrip::GetData

HRESULT CChordStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (ppData == NULL) || (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);

	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		// Copy our groupbits to the location pointed to by ppData
		PPGTrackParams *pPPGTrackParams = static_cast<PPGTrackParams *>(*ppData);
		pPPGTrackParams->dwGroupBits = m_pChordMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pChordMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pChordMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	case 2:
		{
			ioChordStripPPG *poChordStripPPG = (ioChordStripPPG *)*ppData;
			poChordStripPPG->fUseFlats = m_pChordMgr->m_fDisplayingFlats;
			poChordStripPPG->nKeyRoot = m_pChordMgr->m_nKeyRoot;
			poChordStripPPG->nNumAccidentals = m_pChordMgr->m_nNumAccidentals;
			break;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip::SetData

HRESULT CChordStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_POINTER;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
		// Only allow changing the group bits and track settings if m_fShowChordProps is set
		if( m_pChordMgr->m_fUseChordStripPPG )
		{
			PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

			// Update our group bits setting, if necessary
			if( pPPGTrackParams->dwGroupBits != m_pChordMgr->m_dwGroupBits )
			{
				m_nLastEdit = IDS_UNDO_TRACK_GROUP;
				DWORD dwOldBits = m_pChordMgr->m_dwGroupBits;
				m_pChordMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;
				m_pChordMgr->m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)m_pChordMgr );
				m_pChordMgr->m_dwOldGroupBits = m_pChordMgr->m_dwGroupBits;
				m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordKey, dwOldBits | m_pChordMgr->m_dwGroupBits, NULL );
				m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordSharpsFlats, dwOldBits | m_pChordMgr->m_dwGroupBits, NULL );

				// Check if there are any MIDIStripMgrs in m_dwGroupBits
				IDMUSProdStripMgr *pStripMgr;
				if( SUCCEEDED( m_pChordMgr->m_pTimeline->GetStripMgr( CLSID_DirectMusicPatternTrack, m_pChordMgr->m_dwGroupBits, 0, &pStripMgr ) ) )
				{
					pStripMgr->Release();
				}
				else
				{
					// No MIDIStripMgrs, reset the CHORDSTRIP_MODE to nothing
					m_pChordMgr->m_bPatternEditorMode = 0xFF;
				}
			}
			return S_OK;
		}
		break;

	case TRACKFLAGSPPG_INDEX:
		// Only allow changing the group bits and track settings if m_fShowChordProps is set
		if( m_pChordMgr->m_fUseChordStripPPG )
		{
			PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

			// Update our track extras flags, if necessary
			if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pChordMgr->m_dwTrackExtrasFlags )
			{
				m_pChordMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

				// Notify our editor that we've changed
				m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
				m_pChordMgr->m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)m_pChordMgr );
			}
			// Update our Producer-specific flags, if necessary
			else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pChordMgr->m_dwProducerOnlyFlags )
			{
				m_pChordMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

				// Notify our editor that we've changed
				m_nLastEdit = IDS_UNDO_PRODUCERONLY;
				m_pChordMgr->m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)m_pChordMgr );
			}
			return S_OK;
		}
		break;

	case 2:
		{
			ioChordStripPPG *piChordStripPPG = (ioChordStripPPG *)pData;

			if( m_pChordMgr->m_fDisplayingFlats != piChordStripPPG->fUseFlats ||
					 m_pChordMgr->m_nKeyRoot != piChordStripPPG->nKeyRoot ||
					 m_pChordMgr->m_nNumAccidentals != piChordStripPPG->nNumAccidentals )
			{
				if( m_pChordMgr->m_fDisplayingFlats != piChordStripPPG->fUseFlats )
				{
					m_pChordMgr->m_fDisplayingFlats = piChordStripPPG->fUseFlats;
					m_pChordMgr->m_ChordList.UseFlats(piChordStripPPG->fUseFlats == TRUE);
					// Must be done before updating DirectMusic and the other StripMgrs
					m_pChordMgr->UpdateMasterScaleAndKey();
					m_pChordMgr->UpdateDirectMusic( TRUE );
					m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordSharpsFlats, m_pChordMgr->m_dwGroupBits, NULL );
				}

				if( m_pChordMgr->m_nKeyRoot != piChordStripPPG->nKeyRoot ||
					m_pChordMgr->m_nNumAccidentals != piChordStripPPG->nNumAccidentals )
				{
					m_pChordMgr->m_nKeyRoot = piChordStripPPG->nKeyRoot;
					m_pChordMgr->m_nNumAccidentals = piChordStripPPG->nNumAccidentals;
					// Must be done before updating the other StripMgrs
					m_pChordMgr->UpdateMasterScaleAndKey();
					m_pChordMgr->UpdateDirectMusic( TRUE );
					m_pChordMgr->m_pTimeline->NotifyStripMgrs( GUID_ChordKey, m_pChordMgr->m_dwGroupBits, NULL );
				}

				m_pChordMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_nLastEdit = IDS_UNDO_KEYCHANGE;
				m_pChordMgr->m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)m_pChordMgr );
			}
			return S_OK;
			break;
		}
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip::OnShowProperties

HRESULT CChordStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pChordMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pChordMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	if (pIFramework == NULL)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}


	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	pIFramework->Release();
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
		CChordStripPropPageMgr* pPPM = new CChordStripPropPageMgr;
		if( NULL == pPPM )
		{
			hr = E_OUTOFMEMORY;
			goto EXIT;
		}
		pPPM->m_fShowGroupBits = m_pChordMgr->m_fUseChordStripPPG;
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
		{
			goto EXIT;
		}
	}

	// Set the property page to refer to the groupbits and/or key property page.
	short nActiveTab;
	nActiveTab = CChordStripPropPageMgr::sm_nActiveTab;
	m_pChordMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CChordStrip::OnRemoveFromPageManager

HRESULT CChordStrip::OnRemoveFromPageManager( void )
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
// CChordStrip::UnselectGutterRange

void CChordStrip::UnselectGutterRange( void )
{
	ASSERT( m_pChordMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pChordMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pChordMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}

HRESULT CChordStrip::StartDragDrop(IDMUSProdTimeline* pTimeline, LONG lXPos)
{
	HRESULT hr = S_OK;
	VARIANT var;
	BOOL bDrop = FALSE;

	m_nStripIsDragDropSource = 1;	

	bDrop = TRUE;
	// drag drop will capture mouse, so release it from timeline
	V_BOOL(&var) = FALSE;
	pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
	IDropSource*	pIDropSource;
//				IDataObject*	pIDataObject;
	DWORD			dwEffect = DROPEFFECT_NONE;
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// create data object and image
		hr = CreateDataObject(&m_pISourceDataObject, lXPos);
		if(SUCCEEDED(hr))
		{
			// start drag drop
			DWORD dwOKDragEffects = DROPEFFECT_COPY;
			if(CanCut() == S_OK)
			{
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}
			m_startDragPosition = lXPos;
			hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

			// drag drop completed, clean up
			m_dwStartDragButton = 0;
			switch(hr)
			{
			case DRAGDROP_S_DROP:
				if(dwEffect & DROPEFFECT_MOVE)
				{
					m_pChordMgr->DeleteMarkedChords(UD_DRAGSELECT);
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
		else
		{
			m_nStripIsDragDropSource = 0;	
		}

		pIDropSource->Release();
		m_pChordMgr->UnMarkChords(UD_DRAGSELECT);

	}
	else
	{
		m_nStripIsDragDropSource = 0;	
		hr = E_FAIL;
		goto END;
	}

	if(!bDrop)
	{
		hr = S_FALSE;
	}
	else if( dwEffect != DROPEFFECT_NONE )
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
			m_pChordMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
			m_fShowChordProps = TRUE;
			if( m_pChordMgr->m_pPropPageMgr )
			{
				m_pChordMgr->m_pPropPageMgr->RefreshData();
			}
			m_pChordMgr->OnShowProperties(); 
			m_pChordMgr->UpdateDirectMusic( TRUE );
		}
		else
		{
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_UNDO_DELETE;
				m_pChordMgr->UpdateDirectMusic( TRUE );
			}
		}
	}

	// Done with drag/drop
	m_nStripIsDragDropSource = 0;


END:
	return hr;
}


HRESULT CChordStrip :: DoRightClickMenu(IDMUSProdTimeline* pTimeline, POINT pt)
{
	HMENU hMenu;
	HMENU hMenuPopup;

	HRESULT hr = S_OK;

	// Check if we're in the segment designer
	if( m_pChordMgr->m_pIDMTrack )
	{
		// Does the Segment contain a Pattern track?
		if( m_pChordMgr->m_bPatternEditorMode == 0xFF )
		{
			// No Pattern Track - Use Segment editor right-click menu
			hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_SEGMENT_EDITOR_RMENU) );
		}
		else
		{
			// Has Pattern Track - Use Segment editor (with Pattern Track) right-click menu
			hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_SEGMENT_EDITOR_WITHPTRN_RMENU) );
		}
		if( hMenu )
		{
			// Initialize menu items specific to Segment editor
			hMenuPopup = ::GetSubMenu(hMenu, 0);
			if( hMenuPopup )
			{
				switch( m_pChordMgr->GetSelectedHideState() )
				{
					case HIDE_NONE_SELECTED:
						::CheckMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_UNCHECKED | MF_BYCOMMAND) );
						::EnableMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_GRAYED | MF_BYCOMMAND) );
						break;

					case HIDE_ALL_ON:
					{
						::CheckMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_CHECKED | MF_BYCOMMAND) );
						::EnableMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_ENABLED | MF_BYCOMMAND) );
						break;
					}

					case HIDE_ALL_OFF:
					case HIDE_MIXED:
					{
						::CheckMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_UNCHECKED | MF_BYCOMMAND) );
						::EnableMenuItem( hMenuPopup, ID_HIDE_FROM_RHYTHM, (MF_ENABLED | MF_BYCOMMAND) );
						break;
					}
				}
				// Does the Segment contain a Pattern track?
				if( m_pChordMgr->m_bPatternEditorMode != 0xFF )
				{
					// Has Pattern Track
					if( m_pChordMgr->m_bPatternEditorMode == 1 )
					{
						::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_CHECKED | MF_BYCOMMAND) );
						::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_UNCHECKED | MF_BYCOMMAND) );
					}				
					else
					{
						::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_UNCHECKED | MF_BYCOMMAND) );
						::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_CHECKED | MF_BYCOMMAND) );
					}
				}
			}
		}
	}
	else
	{
		// Use Pattern editor right-click menu
		hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_PATTERN_EDITOR_RMENU) );
		if( hMenu )
		{
			// Initialize menu items specific to Pattern editor
			hMenuPopup = ::GetSubMenu(hMenu, 0);
			if( hMenuPopup )
			{
				if( m_pChordMgr->m_bPatternEditorMode == 1 )
				{
					::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_CHECKED | MF_BYCOMMAND) );
					::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_UNCHECKED | MF_BYCOMMAND) );
				}				
				else
				{
					::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_UNCHECKED | MF_BYCOMMAND) );
					::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_CHECKED | MF_BYCOMMAND) );
				}
			}
		}
	}

	if( hMenu
	&&  hMenuPopup )
	{
		::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
				MF_GRAYED | MF_BYPOSITION );
		::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );

		pTimeline->TrackPopupMenu( hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE );
	}

	if( hMenu )
	{
		::DestroyMenu( hMenu );
	}

	return hr;
}


HRESULT CChordStrip :: PostRightClickMenu(IDMUSProdTimeline* pTimeline, POINT pt)
{
	HRESULT hr = S_OK;
	// Use Chordstrip editor right-click menu
	HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_PATTERN_EDITOR_RMENU) );
	HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);
	if( m_pChordMgr->m_bPatternEditorMode == 1 )
	{
		::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_CHECKED | MF_BYCOMMAND) );
		::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_UNCHECKED | MF_BYCOMMAND) );
	}				
	else
	{
		::CheckMenuItem( hMenuPopup, IDM_CONSTANT_MIDI, (MF_UNCHECKED | MF_BYCOMMAND) );
		::CheckMenuItem( hMenuPopup, IDM_CONSTANT_FUNCTION, (MF_CHECKED | MF_BYCOMMAND) );
	}

	::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
			MF_GRAYED | MF_BYPOSITION );
	::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED :
			MF_GRAYED );
	::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );

	pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
	::DestroyMenu( hMenu );

	return hr;
}

void CChordStrip::SelectChordsInSelectedRegions()
{
	for(CChordItem* pChord = m_pChordMgr->m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(m_pSelectedRegions->Contains(pChord->Measure(), pChord->Beat()))		
		{
			pChord->SetSelected(TRUE);
		}
		else
		{
			pChord->SetSelected(FALSE);
		}
	}
}

void CChordStrip::SelectRegionsFromSelectedChords()
{
	m_pSelectedRegions->Clear();
	for(CChordItem* pChord = m_pChordMgr->m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			CListSelectedRegion_AddRegion(*m_pSelectedRegions, *pChord);
		}
	}
}


HRESULT CChordStrip::NormalizeSelectedRegions(CListSelectedRegion& list, long clockoffset, long* pStartClocks, long* pEndClocks)
{
	HRESULT hr = S_OK;
	CMusicTimeConverter cmtMin(0x7fffffff);
	CMusicTimeConverter cmtMax(0);

	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		CSelectedRegion* psr = list.GetNext(pos);
		if(psr->Beg() < cmtMin)
		{
			cmtMin = psr->Beg();
		}
		if(cmtMax < psr->End())
		{
			cmtMax = psr->End();
		}
		// offset selected Regions
		psr->Beg().Time() += clockoffset;
		psr->End().Time() += clockoffset;
	}
	if(pStartClocks)
		*pStartClocks = cmtMin;
	if(pEndClocks)
		*pEndClocks = cmtMax;
	return hr;
}

HRESULT CChordStrip::SaveSelectedRegions(IStream* pIStream, long offset, bool bNormalize)
{
	ASSERT(pIStream);

	CListSelectedRegion list(m_pChordMgr->m_pTimeline, m_pChordMgr->m_dwGroupBits);
	
	list.Copy(*m_pSelectedRegions);

	if(!list.IsEmpty() && bNormalize)
	{
		NormalizeSelectedRegions(list, TRUE);
	}
	
	HRESULT hr = list.Save(pIStream);
	list.Clear();
	return hr;
}