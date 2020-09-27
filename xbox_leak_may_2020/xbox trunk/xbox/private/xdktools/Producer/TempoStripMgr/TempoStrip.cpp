// TempoStrip.cpp : Implementation of CTempoStrip
#include "stdafx.h"
#include "TempoIO.h"
#include "TempoStripMgr.h"
#include "TempoMgr.h"
#include "PropPageMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include <RiffStrm.h>
#include <dmusicf.h>
#include "MusicTimeConverter.h"
#include "SegmentIO.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STRIP_HEIGHT 20

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip constructor/destructor

CTempoStrip::CTempoStrip( CTempoMgr* pTempoMgr )
{
	ASSERT( pTempoMgr );
	if ( pTempoMgr == NULL )
	{
		return;
	}

	m_pTempoMgr = pTempoMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pTempoMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lXPos = -1;
	m_bGutterSelected = FALSE;
	m_bSelecting = FALSE;
	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;
	m_pISourceDataObject = NULL;
	m_pITargetDataObject = NULL;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_nLastEdit = 0;
	m_fShowTempoProps = FALSE;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;
	m_fInRightClickMenu = FALSE;
	m_fSingleSelect = FALSE;
	m_fLButtonDown = FALSE;
	m_fInDragDrop = FALSE;
	m_pCurrentlySelectedTempoItem = NULL;

	UpdateName();
}

CTempoStrip::~CTempoStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pTempoMgr = NULL;
	}
	if ( m_pISourceDataObject )
	{
		m_pISourceDataObject->Release();
	}
	if ( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
	}
	if ( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::QueryInterface

STDMETHODIMP CTempoStrip::QueryInterface( REFIID riid, LPVOID *ppv )
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
// CTempoStrip::AddRef

STDMETHODIMP_(ULONG) CTempoStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Release

STDMETHODIMP_(ULONG) CTempoStrip::Release(void)
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
// CTempoStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Draw

bool IsRealTempo(CTempoItem* pTempo)
{
	ASSERT(pTempo);
	if(!pTempo)
		return false;
	return (pTempo->m_strText.IsEmpty() == FALSE) ? true : false;
}

HRESULT	STDMETHODCALLTYPE CTempoStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( sv );

	bool fUseGutterSelectRange = m_bGutterSelected && m_lGutterBeginSelect != m_lGutterEndSelect;

	// Get a pointer to the Timeline
	if( m_pTempoMgr->m_pTimeline )
	{
		IDMUSProdTimeline* pTimeline = m_pTempoMgr->m_pTimeline;

		// Draw Measure and Beat lines in our strip
		pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pTempoMgr->m_dwGroupBits, 0, lXOffset );

			// Get clip box
			RECT rectClip;
			::GetClipBox( hDC, &rectClip );
			RECT rectHighlight;
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;
			RECT rectGhost;
			rectGhost.top = rectGhost.left = rectGhost.right = 0;
			rectGhost.bottom = STRIP_HEIGHT;

			// Create Italics font
			HFONT hFontItalics = NULL;
			HFONT hFont = static_cast<HFONT>(::GetCurrentObject( hDC, OBJ_FONT ));
			if( hFont )
			{
				LOGFONT logfont;
				if( ::GetObject( hFont, sizeof(logfont), &logfont) > 0 )
				{
					logfont.lfItalic = TRUE;
					hFontItalics = ::CreateFontIndirect( &logfont );
				}
			}

			//
			// find first visible measure/beat for ghosting tempos
			//
			
			// Get the time of the first pixel
			long lClocks;
			pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);

			// Get the measure/beat of the first pixel
			long lFirstPartialVisibleMeasure, lFirstPartialVisibleBeat;
			m_pTempoMgr->ClocksToMeasureBeat( lClocks, &lFirstPartialVisibleMeasure, &lFirstPartialVisibleBeat );

			// Get the start time of the first visible measure/beat
			CMusicTimeConverter cmtNearestMeasureBeat(lFirstPartialVisibleMeasure, lFirstPartialVisibleBeat, pTimeline, m_pTempoMgr->m_dwGroupBits);

			// If the first visible measure beat starts earlier than the display
			long lFirstFullVisibleMeasure, lFirstFullVisibleBeat;
			if(cmtNearestMeasureBeat.Time() < lClocks)
			{
				// round up to the next beat
				cmtNearestMeasureBeat.AddOffset(0, 1, pTimeline, m_pTempoMgr->m_dwGroupBits);
				cmtNearestMeasureBeat.GetMeasureBeat(lFirstFullVisibleMeasure, lFirstFullVisibleBeat, pTimeline, m_pTempoMgr->m_dwGroupBits);
			}
			else
			{
				// Otherwise the first full visible beat is the current one
				lFirstFullVisibleMeasure = lFirstPartialVisibleMeasure;
				lFirstFullVisibleBeat = lFirstPartialVisibleBeat;
			}

			// Find nearest Tempo in or after first visible measure/beat
			POSITION pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();
			POSITION posCurrent = pos;
			while( pos )
			{
				CTempoItem* pTempoItem = m_pTempoMgr->m_lstTempos.GetNext(pos);
				
				if(pTempoItem->m_lMeasure > lFirstFullVisibleMeasure || (pTempoItem->m_lMeasure == lFirstFullVisibleMeasure && 
				  pTempoItem->m_lBeat >= lFirstFullVisibleBeat))
				{
					break;
				}

				posCurrent = pos;
			}

			//
			// Find the first previous real tempo (the ghost tempo)
			bool bUseItalicsForGhost = false;
			bool bGhostTempoCovered = false;	// indicates real tempo covering ghost tempo
			CTempoItem* pGhostTempo = NULL;

			// Back up one in the tempo list
			if( posCurrent )
			{
				m_pTempoMgr->m_lstTempos.GetPrev( posCurrent );
			}
			else
			{
				posCurrent = m_pTempoMgr->m_lstTempos.GetTailPosition();
			}

			// While there is a tempo before the one we found
			long lCurrentMeasure = -1;
			long lCurrentBeat = -1;
			while( posCurrent )
			{
				// Get a pointer to it
				const POSITION posCurCurrent = posCurrent;
				CTempoItem *pTempoItem = m_pTempoMgr->m_lstTempos.GetPrev( posCurrent );
				if( pTempoItem->m_lMeasure != lCurrentMeasure || pTempoItem->m_lBeat != lCurrentBeat )
				{
					// Set the current measure and beat
					lCurrentMeasure = pTempoItem->m_lMeasure;
					lCurrentBeat = pTempoItem->m_lBeat;

					// Get the top tempo from that measure and beat
					CTempoItem* pTopTempoItem = GetTopTempoFromMeasureBeatAndPositionReverse( lCurrentMeasure, lCurrentBeat, posCurCurrent );
					if(pTopTempoItem)
					{
						// Check that it's a real tempo
						if(IsRealTempo(pTopTempoItem))
						{
							bUseItalicsForGhost = false;	// may be overridden later
							pGhostTempo = pTopTempoItem;
							if(	pTopTempoItem != pTempoItem
							&&	IsRealTempo(pTempoItem) )
							{
								bUseItalicsForGhost = true;
							}
							else
							{
								// either pTempoItem is the top Tempo of many or it is a singleton
								// in either case we must get to the end of list of measure/beat
								// coincident Tempos
								POSITION posPrev = posCurrent;
								while(posPrev)
								{
									// Get a pointer to the previous tempo
									CTempoItem* pPrevTempoItem = m_pTempoMgr->m_lstTempos.GetPrev(posPrev);

									// Check if its still in the same measure/beat
									if( (pPrevTempoItem->m_lMeasure != lCurrentMeasure)
									||	(pPrevTempoItem->m_lBeat != lCurrentBeat) )
									{
										//end of measure/beat coincident Tempos
										posPrev = NULL;
									}
									else
									{
										// Check if we found another real tempo on the same measure/beat
										if( (pPrevTempoItem != pTopTempoItem)
										&&	IsRealTempo(pPrevTempoItem))
										{
											// Yes - use italics
											bUseItalicsForGhost = true;
										}
									}
								}
							}

							// Break out of the loop, since we found the ghost tempo
							break;
						}
					}
				}
			}

			long lPosition;
			SIZE size;
			if(pGhostTempo)
			{
				// get rect of ghost Tempo's text, we may have to truncate it if it covers a real Tempo
				pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, 
									lFirstFullVisibleMeasure, lFirstFullVisibleBeat, &lPosition );
				lPosition++;
				::GetTextExtentPoint32( hDC, pGhostTempo->m_strText, pGhostTempo->m_strText.GetLength(), &size );
				rectGhost.left = lPosition-lXOffset;
				rectGhost.right = rectGhost.left + size.cx;
			}

			// we'll draw the ghost after checking whether or not Tempo covers it

			// Flag if any of the tempos are selected
			bool fAnyTemposSelected = false;

			// Draw all Tempos in the strip
			long lMeasure;
			long lBeat;
			lCurrentMeasure = -1;
			lCurrentBeat = -1;
			pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();
			while( pos )
			{
				const POSITION posCurrent = pos;
				CTempoItem* pTempoItem = m_pTempoMgr->m_lstTempos.GetNext( pos );
				
				// Get measure/beat of this tempo
				const long lTempoMeasure = pTempoItem->m_lMeasure;
				const long lTempoBeat = pTempoItem->m_lBeat;

				// Get position to draw at
				pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, lTempoMeasure, lTempoBeat, &lPosition );
				
				lPosition++;

				// If the tempo starts after the visible region
				if( lPosition - lXOffset > rectClip.right )
				{
					// break out of the loop
					break;
				}

				// Draw the tempo
				if( lCurrentMeasure != lTempoMeasure || lCurrentBeat != lTempoBeat )
				{
					lCurrentMeasure = lTempoMeasure;
					lCurrentBeat = lTempoBeat;

					CTempoItem* pTopTempoItem = GetTopTempoFromMeasureBeatAndPosition( lTempoMeasure, lTempoBeat, posCurrent );
					ASSERT( pTopTempoItem != NULL );		// Should never be NULL!
					if( pTopTempoItem )
					{
						HFONT hFontOld = NULL;
						if( hFontItalics )
						{
							if( pTopTempoItem->m_strText.IsEmpty() == FALSE )
							{
								if( pTopTempoItem != pTempoItem	&& pTempoItem->m_strText.IsEmpty() == FALSE )
								{
									hFontOld = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
								}
								else
								{
									POSITION posNext = pos;
									while( posNext )
									{
										CTempoItem* pNextTempoItem = m_pTempoMgr->m_lstTempos.GetNext( posNext );
										m_pTempoMgr->ClocksToMeasureBeat( pNextTempoItem->m_mtTime, &lMeasure, &lBeat );
										if( lCurrentMeasure != lMeasure	|| lCurrentBeat != lBeat )
										{
											posNext = NULL;
										}
										else
										{
											if( pNextTempoItem != pTopTempoItem	&& pNextTempoItem->m_strText.IsEmpty() == FALSE )
											{
												hFontOld = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
												posNext = NULL;

												// If this item is selected and the one on top is not then do the needful...
												if(pTopTempoItem->m_fSelected == FALSE && pNextTempoItem->m_fSelected == TRUE)
												{
													pTopTempoItem->m_wFlags &= ~BF_TOPTEMPO;
													pTopTempoItem = pNextTempoItem;
													pTopTempoItem->m_wFlags |= BF_TOPTEMPO;
												}
											}
										}
									}
								}
							}
						}

						// Find extent of text
						::GetTextExtentPoint32( hDC, pTopTempoItem->m_strText, pTopTempoItem->m_strText.GetLength(), &size );
						long lBeatLength = GetBeatWidth(lTempoMeasure);
						if( size.cx < lBeatLength )
						{
							size.cx = lBeatLength;
						}
						long lExtent = lPosition + size.cx;

						// Check if this tempo is visible
						if( lExtent - lXOffset >= rectClip.left )
						{
							// Truncate if next Tempo covers part of text
							POSITION pos2 = pos;
							while( pos2 )
							{
								CTempoItem* pTempTempo = m_pTempoMgr->m_lstTempos.GetNext( pos2 );

								if( pTempTempo->m_strText.IsEmpty() == FALSE )
								{
									lMeasure = pTempTempo->m_lMeasure;
									lBeat = pTempTempo->m_lBeat;
									if( lCurrentMeasure != lMeasure
									||  lCurrentBeat != lBeat )
									{
										long lNextTempoStartPos;
										pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lNextTempoStartPos );
										if( lNextTempoStartPos <= lExtent )
										{
											lExtent = lNextTempoStartPos;
										}
										break;
									}
								}
							}

							rectHighlight.left = lPosition - lXOffset;
							rectHighlight.right = lExtent - lXOffset;

							// truncate ghost tempo if necessary
							if( pGhostTempo
							&&	IsRealTempo(pTopTempoItem) )
							{
								bool b1 = pTopTempoItem->m_lMeasure == lFirstFullVisibleMeasure
									&& pTopTempoItem->m_lBeat == lFirstFullVisibleBeat;
								bool b2 = pTopTempoItem->m_lMeasure == lFirstPartialVisibleMeasure
										&& rectHighlight.right > (LeftMargin(pTimeline) - lXOffset);
								if(b1 || b2)
								{
									bGhostTempoCovered = true;
								}
								else if(rectHighlight.left > rectGhost.left
											&& rectHighlight.left < rectGhost.right)
								{
									rectGhost.right = rectHighlight.left;
								}
							}

							if( !fUseGutterSelectRange
							&&	pTopTempoItem->m_fSelected )
							{
								// Don't need to draw the selected tempo item, since it's draw below
								fAnyTemposSelected = true;
							}
							else
							{
								::DrawText( hDC, pTopTempoItem->m_strText, pTopTempoItem->m_strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
							}
						}

						if( hFontOld )
						{
							::SelectObject( hDC, hFontOld );
						}
					}
				}
			}

			// finally draw ghost tempo
			if(pGhostTempo && !bGhostTempoCovered)
			{
				HFONT hOldFont = 0;
				if(bUseItalicsForGhost && hFontItalics)
				{
					hOldFont = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
				}
				COLORREF cr = ::SetTextColor( hDC, RGB(168,168,168));
				::DrawText( hDC, pGhostTempo->m_strText, pGhostTempo->m_strText.GetLength(), &rectGhost, (DT_LEFT | DT_NOPREFIX) );

				if(hOldFont)
				{
					::SelectObject( hDC, hOldFont );
				}
				::SetTextColor( hDC, cr );
			}

			// Make sure the names of selected tempos are shown in their entirety
			long lLastMeasure = -1;
			long lLastBeat = -1;
			lCurrentMeasure = -1;
			lCurrentBeat = -1;
			pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();

			// Skip this if we're using the gutter select range or if there are no selected tempos
			if( fUseGutterSelectRange
			||	!fAnyTemposSelected )
			{
				pos = NULL;
			}
			while( pos )
			{
				POSITION posCurrentTemp = pos;
				CTempoItem *pTempoItem = m_pTempoMgr->m_lstTempos.GetNext( pos );

				const long lTempoMeasure = pTempoItem->m_lMeasure;
				const long lTempoBeat = pTempoItem->m_lBeat;
				pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, lTempoMeasure, lTempoBeat, &lPosition );

				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				if( pTempoItem->m_fSelected )
				{
					if( lCurrentMeasure != lTempoMeasure || lCurrentBeat != lTempoBeat )
					{
						lCurrentMeasure = lTempoMeasure;
						lCurrentBeat = lTempoBeat;

						// Back up and find the first tempo on this beat
						POSITION posFirst = posCurrentTemp;
						while( posCurrentTemp )
						{
							posFirst = posCurrentTemp;
							CTempoItem *pTmpTempoItem = m_pTempoMgr->m_lstTempos.GetPrev( posCurrentTemp );
							if( lCurrentMeasure != pTmpTempoItem->m_lMeasure
							||	lCurrentBeat != pTmpTempoItem->m_lBeat )
							{
								break;
							}
						}

						CTempoItem* pTopTempoItem = GetTopTempoFromMeasureBeatAndPosition( lTempoMeasure, lTempoBeat, posFirst );
						ASSERT( pTopTempoItem != NULL );		// Should never be NULL!
						if( pTopTempoItem )
						{
							HFONT hFontOld = NULL;
							if( hFontItalics )
							{
								if( pTopTempoItem->m_strText.IsEmpty() == FALSE )
								{
									if( pTopTempoItem != pTempoItem	&&  pTempoItem->m_strText.IsEmpty() == FALSE )
									{
										hFontOld = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
									}
									else if( lLastMeasure == lTempoMeasure &&  lLastBeat == lTempoBeat )
									{
										hFontOld = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
									}
									else
									{
										POSITION posNext = pos;
										while( posNext )
										{
											CTempoItem* pNextTempoItem = m_pTempoMgr->m_lstTempos.GetNext( posNext );
											m_pTempoMgr->ClocksToMeasureBeat( pNextTempoItem->m_mtTime, &lMeasure, &lBeat );
											if( lCurrentMeasure != lMeasure	||  lCurrentBeat != lBeat )
											{
												posNext = NULL;
											}
											else
											{
												if( pNextTempoItem != pTopTempoItem	&&  pNextTempoItem->m_strText.IsEmpty() == FALSE )
												{
													hFontOld = static_cast<HFONT>(::SelectObject( hDC, hFontItalics ));
													posNext = NULL;
												}
											}
										}
									}
								}
							}

							// If this item is selected and the one on top is not then do the needful...
							if( pTopTempoItem != pTempoItem )
							{
								if( pTopTempoItem->m_fSelected == FALSE )
								{
									pTopTempoItem = pTempoItem;
								}

								pTempoItem->m_wFlags &= ~BF_TOPTEMPO;
								pTopTempoItem->m_wFlags |= BF_TOPTEMPO;
							}

							// find extent of text
							long lBeatLength = GetBeatWidth(lTempoMeasure);
							::GetTextExtentPoint32( hDC, pTopTempoItem->m_strText, pTopTempoItem->m_strText.GetLength(), &size );
							if( size.cx < lBeatLength )
							{
								size.cx = lBeatLength;
							}
							long lExtent = lPosition + size.cx;

							// Check if this tempo is visible
							if( lExtent - lXOffset >= rectClip.left )
							{
								// Truncate if a selected Tempo covers part of text
								POSITION pos2 = pos;
								while( pos2 )
								{
									CTempoItem* pTempTempo = m_pTempoMgr->m_lstTempos.GetNext( pos2 );

									if( pTempTempo->m_fSelected
									&&	pTempTempo->m_strText.IsEmpty() == FALSE )
									{
										lMeasure = pTempTempo->m_lMeasure;
										lBeat = pTempTempo->m_lBeat;
										if( lCurrentMeasure != lMeasure
										||	lCurrentBeat != lBeat )
										{
											long lNextTempoStartPos;
											pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lNextTempoStartPos );
											if( lNextTempoStartPos <= lExtent )
											{
												lExtent = lNextTempoStartPos;
											}
											break;
										}
									}
								}

								rectHighlight.left = lPosition - lXOffset;
								rectHighlight.right = lExtent - lXOffset;
								::DrawText( hDC, pTopTempoItem->m_strText, pTopTempoItem->m_strText.GetLength(), &rectHighlight, (DT_LEFT | DT_NOPREFIX) );
								GrayOutRect( hDC, &rectHighlight );
							}

							if( hFontOld )
							{
								::SelectObject( hDC, hFontOld );
							}
						}
					}
				}
			
				if( pTempoItem->m_strText.IsEmpty() == FALSE )
				{
					lLastMeasure = lTempoMeasure;
					lLastBeat = lTempoBeat;
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

				pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, 0, lBeginSelect, &lMeasure, &lBeat );
				pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
				pTimeline->ClocksToPosition( lClocks, &(rectHighlight.left));

				pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, 0, lEndSelect, &lMeasure, &lBeat );
				pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, 0, lMeasure, (lBeat + 1), &lClocks );
				pTimeline->ClocksToPosition( (lClocks - 1), &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;

				// Invert it.
				GrayOutRect( hDC, &rectHighlight );
			}

			if( hFontItalics )
			{
				::DeleteObject( hFontItalics );
			}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CTempoStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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

			pvar->vt = VT_BSTR; 
			try
			{
				bstr = m_strName.AllocSysString();
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
		if( m_pTempoMgr )
		{
			m_pTempoMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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
// CTempoStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CTempoStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
			if( m_pTempoMgr->UnselectAll() )
			{
				m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			break;
		}
		if( m_bGutterSelected )
		{
			m_pTempoMgr->UnselectAll();
			m_pTempoMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
			m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}
		else
		{
			if( m_pTempoMgr->UnselectAll() )
			{
				m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
		}

		// Update the property page
		if( m_pTempoMgr->m_pPropPageMgr != NULL )
		{
			m_pTempoMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			m_pTempoMgr->UnselectAll();
			m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}
		if( m_bGutterSelected )
		{
			m_pTempoMgr->UnselectAll();
			m_pTempoMgr->SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			m_pTempoMgr->UnselectAll();
		}
		m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pTempoMgr->m_pPropPageMgr != NULL )
		{
			m_pTempoMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::GetTopTempoFromMeasureBeatAndPositionReverse

CTempoItem *CTempoStrip::GetTopTempoFromMeasureBeatAndPositionReverse( long lMeasure, long lBeat, POSITION pos )
{
	// Validate our Timeline pointer
	if( m_pTempoMgr->m_pTimeline == NULL )
	{
		return NULL;
	}

	// Get the time of the start of the beat
	MUSIC_TIME mtTimeBeatStart;
	if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits,
					m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &mtTimeBeatStart ) ) )
	{
		return NULL;
	}

	// Get the timesig of the beat
	DMUS_TIMESIGNATURE dmTimeSig;
	if( FAILED( m_pTempoMgr->m_pTimeline->GetParam( GUID_TimeSignature, 
					m_pTempoMgr->m_dwGroupBits, 0, mtTimeBeatStart, NULL, (void*) &dmTimeSig ) ) ) 
	{
		return NULL;
	}

	// Get the time of the next beat
	const MUSIC_TIME mtTimeBeatEnd = mtTimeBeatStart + ((DMUS_PPQ * 4) / dmTimeSig.bBeat);

	// If pos is NULL, set it to the first item in the list
	if( NULL == pos )
	{
		pos = m_pTempoMgr->m_lstTempos.GetTailPosition();
	}

	CTempoItem* pTheTempoItem = NULL;
	CTempoItem* pFirstTempoItem = NULL;

	// Iterate through the tempos
	while( pos )
	{
		CTempoItem* pTempoItem = m_pTempoMgr->m_lstTempos.GetPrev( pos );

		// If this tempo is earlier than the beat we're looking for
		if( pTempoItem->m_mtTime < mtTimeBeatStart )
		{
			// Break out of the loop
			break;
		}
		// If this tempo is in the beat we're looking for
		else if( pTempoItem->m_mtTime < mtTimeBeatEnd )
		{
			if( pTempoItem->m_wFlags & BF_TOPTEMPO )
			{
				return pTempoItem;
			}

			if( pFirstTempoItem == NULL )
			{
				pFirstTempoItem = pTempoItem;
			}
			else
			{
				if( pFirstTempoItem->m_strText.IsEmpty() )
				{
					pFirstTempoItem = pTempoItem;
				}
			}
		}
	}

	// If we didn't find a tempo in the beat with the BF_TOPTEMPO flag
	if( pTheTempoItem == NULL )
	{
		// Just use the first tempo we found
		pTheTempoItem = pFirstTempoItem;
	}

	return pTheTempoItem;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::GetTopTempoFromMeasureBeatAndPosition

CTempoItem *CTempoStrip::GetTopTempoFromMeasureBeatAndPosition( long lMeasure, long lBeat, POSITION pos )
{
	// Validate our Timeline pointer
	if( m_pTempoMgr->m_pTimeline == NULL )
	{
		return NULL;
	}

	// Get the time of the start of the beat
	MUSIC_TIME mtTimeBeatStart;
	if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits,
					m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &mtTimeBeatStart ) ) )
	{
		return NULL;
	}

	// Get the timesig of the beat
	DMUS_TIMESIGNATURE dmTimeSig;
	if( FAILED( m_pTempoMgr->m_pTimeline->GetParam( GUID_TimeSignature, 
					m_pTempoMgr->m_dwGroupBits, 0, mtTimeBeatStart, NULL, (void*) &dmTimeSig ) ) ) 
	{
		return NULL;
	}

	// Get the time of the next beat
	const MUSIC_TIME mtTimeBeatEnd = mtTimeBeatStart + ((DMUS_PPQ * 4) / dmTimeSig.bBeat);

	// If pos is NULL, set it to the first item in the list
	if( NULL == pos )
	{
		pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();
	}

	CTempoItem* pTheTempoItem = NULL;
	CTempoItem* pFirstTempoItem = NULL;

	// Iterate through the tempos
	while( pos )
	{
		CTempoItem* pTempoItem = m_pTempoMgr->m_lstTempos.GetNext( pos );

		// If this tempo is after the beat we're looking for
		if( pTempoItem->m_mtTime >= mtTimeBeatEnd )
		{
			// Break out of the loop
			break;
		}
		// If this tempo is in the beat we're looking for
		else if( pTempoItem->m_mtTime >= mtTimeBeatStart )
		{
			if( pTempoItem->m_wFlags & BF_TOPTEMPO )
			{
				return pTempoItem;
			}

			if( pFirstTempoItem == NULL )
			{
				pFirstTempoItem = pTempoItem;
			}
			else
			{
				if( pFirstTempoItem->m_strText.IsEmpty() )
				{
					pFirstTempoItem = pTempoItem;
				}
			}
		}
	}

	// If we didn't find a tempo in the beat with the BF_TOPTEMPO flag
	if( pTheTempoItem == NULL )
	{
		// Just use the first tempo we found
		pTheTempoItem = pFirstTempoItem;
	}

	return pTheTempoItem;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::GetTopTempoFromPoint

CTempoItem *CTempoStrip::GetTopTempoFromPoint( long lPos )
{
	CTempoItem* pTheTempoItem = NULL;
	
	if( m_pTempoMgr->m_pTimeline )
	{
		long lMeasure, lBeat;

		if( SUCCEEDED( m_pTempoMgr->m_pTimeline->PositionToMeasureBeat( m_pTempoMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			pTheTempoItem = GetTopTempoFromMeasureBeatAndPosition( lMeasure, lBeat, NULL );
		}
	}

	return pTheTempoItem;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CTempoStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	if( !m_pTempoMgr->m_pTimeline )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_lXPos = lXPos;
		hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_LBUTTONUP:
		m_lXPos = lXPos;
		if( m_fSingleSelect )
		{
			CTempoItem* pTempo = GetTopTempoFromPoint( lXPos );
			if( pTempo
			&&  pTempo->m_dblTempo != 0.0 )
			{
				m_pTempoMgr->UnselectAll();
				pTempo->m_fSelected = TRUE;
				m_pCurrentlySelectedTempoItem = pTempo;

				// Redraw the strip and refresh the Tempo property page
				m_pTempoMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
				m_pTempoMgr->OnShowProperties();
				if( m_pTempoMgr->m_pPropPageMgr )
				{
					m_pTempoMgr->m_pPropPageMgr->RefreshData();
				}
			}
			m_fSingleSelect = FALSE;
		}
		m_fLButtonDown = FALSE;
		UnselectGutterRange();
		hr = S_OK;
		break;

	case WM_RBUTTONDOWN:
		m_lXPos = lXPos;
		hr = OnRButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_RBUTTONUP:
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

			if( m_pTempoMgr->m_pTimeline )
			{
				m_fInRightClickMenu = TRUE;
				HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
				HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

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
				::EnableMenuItem( hMenuPopup, IDM_CYCLE_TEMPOS, ( m_pTempoMgr->CanCycle(m_lXPos) == TRUE ) ? MF_ENABLED :
						MF_GRAYED );

				m_pTempoMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
				m_fInRightClickMenu = FALSE;
				::DestroyMenu( hMenu );
			}
			hr = S_OK;
		}
		break;

	case WM_MOUSEMOVE:
		if( m_fLButtonDown )
		{
			// Do Drag'n'drop
			DoDragDrop(m_dwStartDragButton, lXPos);

			// Clear button down flag so we don't do drag'n'drop again.
			m_fLButtonDown= FALSE;
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
			hr = ShowPropertySheet(m_pTempoMgr->m_pTimeline);
			if (m_fShowTempoProps)
			{
				// Change to the tempo property page
				m_pTempoMgr->OnShowProperties();
			}
			else
			{
				// Change to our property page
				OnShowProperties();
			}
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
			hr = Paste( NULL );
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		case IDM_CYCLE_TEMPOS:
			hr = m_pTempoMgr->CycleTempos( m_lXPos );
			break;
		default:
			break;
		}
		break;

	case WM_CREATE:
		m_cfTempoList = RegisterClipboardFormat( CF_TEMPOLIST );
		UpdateName();

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::FBDraw

HRESULT CTempoStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( sv );
	UNREFERENCED_PARAMETER( hDC );
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::FBOnWMMessage

HRESULT CTempoStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lXPos );
	UNREFERENCED_PARAMETER( lYPos );

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_lXPos = -1;
		m_fShowTempoProps = FALSE;
		OnShowProperties();
		break;
	case WM_LBUTTONUP:
		m_lXPos = -1;
		break;
	case WM_RBUTTONDOWN:
		m_lXPos = -1;
		break;
	case WM_RBUTTONUP:
		m_fShowTempoProps = FALSE;
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

		if( m_pTempoMgr->m_pTimeline )
		{
			m_fInRightClickMenu = TRUE;
			HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
					MF_GRAYED | MF_BYPOSITION );
			::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, MF_GRAYED ); // 26970: Shouldn't enable Insert
			::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
			::EnableMenuItem( hMenuPopup, IDM_CYCLE_TEMPOS, ( m_pTempoMgr->CanCycle(m_lXPos) == TRUE ) ? MF_ENABLED :
						MF_GRAYED );

			m_pTempoMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
			m_fInRightClickMenu = FALSE;
			::DestroyMenu( hMenu );
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Cut

HRESULT CTempoStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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
	hr = Copy( pITimelineDataObject );
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Copy

HRESULT CTempoStrip::Copy( IDMUSProdTimelineDataObject *pITimelineDataObject )
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

	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfTempoList == 0 )
	{
		m_cfTempoList = RegisterClipboardFormat( CF_TEMPOLIST );
		if( m_cfTempoList == 0 )
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
//	m_pTempoMgr->MarkSelectedTempos(UD_DRAGSELECT);

	MUSIC_TIME mtTime;
	if( !pITimelineDataObject )
	{
		// Get clock of first selected tempo's measure/beat
		CTempoItem* pTempoAtDragPoint = m_pTempoMgr->FirstSelectedTempo();
		long lMeasure, lBeat;
		if( FAILED( m_pTempoMgr->ClocksToMeasureBeat( pTempoAtDragPoint->m_mtTime, &lMeasure, &lBeat ) ) )
		{
			return E_UNEXPECTED;
		}
		if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &mtTime ) ) )
		{
			return E_UNEXPECTED;
		}
	}
	else
	{
		// Get clock from pITimelineDataObject
		if( FAILED( pITimelineDataObject->GetBoundaries( &mtTime, NULL ) ) )
		{
			return E_UNEXPECTED;
		}
		
		// Adjust to measure/beat boundary
		long lMeasure, lBeat;
		m_pTempoMgr->m_pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
		m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
	}



	// Save the commands into the stream.
	hr = m_pTempoMgr->SaveSelectedTempos( pStreamCopy, mtTime );
	if( FAILED( hr ))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfTempoList, pStreamCopy );
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
		hr = m_pTempoMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pTempoMgr->GetBoundariesOfSelectedTempos( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfTempoList, pStreamCopy );

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
		if(	m_pTempoMgr->m_pCopyDataObject )
		{
			m_pTempoMgr->m_pCopyDataObject->Release();
		}

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pTempoMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the ITimelienDataObject
		//m_pTempoMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::PasteAt

HRESULT CTempoStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtTime, BOOL &fChanged)
{
	if( pITimelineDataObject == NULL )
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfTempoList ) == S_OK )
	{
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfTempoList, &pIStream)))
		{
			// Check for RIFF format
			if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				goto Leave;
			}

			MMCKINFO ck;
			ck.ckid = DMUS_FOURCC_TEMPO_TRACK;
			if ( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) == 0 )
			{
				// get offset of first tempo's drag position to normalize all tempos to offset zero:
				PROD_IO_TEMPO_ITEM iTempo;
				DWORD dwByteCount;
				DWORD dwTempoSize;
				long lChunkSize;

				hr = pIStream->Read( &dwTempoSize, sizeof(dwTempoSize), &dwByteCount );
				if( FAILED( hr ) || dwByteCount != sizeof(dwTempoSize) )
				{
					hr = E_FAIL;
					goto Leave;
				}

				ASSERT( dwTempoSize == sizeof(PROD_IO_TEMPO_ITEM) );
				if( dwTempoSize != sizeof(PROD_IO_TEMPO_ITEM) )
				{
					hr = E_FAIL;
					goto Leave;
				}

				lChunkSize = ck.cksize - sizeof(dwTempoSize);

				// Save the original stream position
				DWORD dwOrigPos = StreamTell( pIStream );

				// Scan through all tempos to find the last one
				long lOrigSize = lChunkSize;
				long lLastTime = LONG_MIN;
				long lFirstTime = LONG_MAX;
				long lFirstOffset = 0;
				while( lChunkSize >= (signed) dwTempoSize )
				{
					hr = pIStream->Read( &iTempo, sizeof(PROD_IO_TEMPO_ITEM), &dwByteCount );
					if( FAILED( hr ) || dwByteCount != sizeof(PROD_IO_TEMPO_ITEM) )
					{
						hr = E_FAIL;
						goto Leave;
					}

					lChunkSize -= dwByteCount;

					if( lLastTime < iTempo.lTime )
					{
						lLastTime = iTempo.lTime;
					}
					if( lFirstTime > iTempo.lTime )
					{
						lFirstTime = iTempo.lTime;
						lFirstOffset = iTempo.lOffset;
					}
				}

				// Ensure the last tempo won't display past the end of the segment
				VARIANT var;
				if( SUCCEEDED( m_pTempoMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var ) ) )
				{
					if( lLastTime + mtTime >= V_INT(&var) )
					{
						mtTime = V_INT(&var) - lLastTime - 1;

						// Snap it to the last possible beat
						long lMeasure, lBeat;
						if( SUCCEEDED( m_pTempoMgr->m_pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, mtTime, &lMeasure, &lBeat ) ) )
						{
							m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &mtTime );
						}
					}
				}

				// Ensure the first tempo won't display before the start of the segment
				if( mtTime + lFirstTime < 0 )
				{
					// Make the first tempo display at time lFirstOffset
					mtTime = lFirstOffset - lFirstTime;
				}

				// Reset the stream pointer to the start position
				StreamSeek( pIStream, dwOrigPos, STREAM_SEEK_SET );

				lChunkSize = lOrigSize;

				while( lChunkSize > 0 )
				{
					hr = pIStream->Read( &iTempo, sizeof(PROD_IO_TEMPO_ITEM), &dwByteCount );
					if( FAILED( hr ) || dwByteCount != sizeof(PROD_IO_TEMPO_ITEM) )
					{
						hr = E_FAIL;
						goto Leave;
					}

					lChunkSize -= dwByteCount;

					CTempoItem* pItem = new CTempoItem;
					if ( pItem == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto Leave;
					}

					pItem->m_mtTime = iTempo.lTime + mtTime;
					pItem->m_dblTempo = iTempo.dblTempo;
					pItem->m_strText.Format("%.2f", pItem->m_dblTempo);

					pItem->m_fSelected = TRUE;

					// Compute the Measure, Beat, Grid, and Offset
					m_pTempoMgr->SetPropTempoBarBeat( (CPropTempo*) pItem );

					// If there is an empty tempo at this position, remove it
					CTempoItem* pTopItem = GetTopTempoFromMeasureBeatAndPosition( pItem->m_lMeasure, pItem->m_lBeat, NULL );
					if( pTopItem && pTopItem->m_strText.IsEmpty() )
					{
						m_pTempoMgr->RemoveItem( pTopItem );
						delete pTopItem;
					}

					fChanged = TRUE;
					m_pTempoMgr->InsertByAscendingTime( pItem, TRUE );
				}

				// Ascend out of our chunk
				pIRiffStream->Ascend( &ck, 0 );

				hr = S_OK;
			}
			else
			{
				// Nothing to paste
				hr = S_FALSE;
			}
		}
	}

Leave:
	RELEASE( pIRiffStream );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Paste

HRESULT CTempoStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	// Make sure we can paste
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	// Get a Timeline pointer
	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTempoMgr->m_pTimeline != NULL );
	if( m_pTempoMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfTempoList == 0 )
	{
		m_cfTempoList = RegisterClipboardFormat( CF_TEMPOLIST );
		if( m_cfTempoList == 0 )
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
		hr = m_pTempoMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pTempoMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	// Get the time to paste at
	MUSIC_TIME mtTime;
	if( FAILED( m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}
	long lMeasure, lBeat;
	if( FAILED( m_pTempoMgr->m_pTimeline->ClocksToMeasureBeat( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, mtTime, &lMeasure, &lBeat ) ) )
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}
	long lClocks;
	if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &lClocks) ) )
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	BOOL fChanged = FALSE;

	if( tlPasteType == TL_PASTE_OVERWRITE )
	{
		long lStart, lEnd;
		if( SUCCEEDED( pITimelineDataObject->GetBoundaries( &lStart, &lEnd ) ) )
		{
			fChanged = m_pTempoMgr->DeleteBetweenTimes( mtTime, mtTime + lEnd - lStart );
		}
	}

	// Unselect the previous tempos
	m_pTempoMgr->UnselectAll();

	// Now, do the paste operation
	hr = PasteAt(pITimelineDataObject, lClocks, fChanged);
	RELEASE( pITimelineDataObject );

	// If successful and something changed, redraw our strip
	if(SUCCEEDED(hr) && fChanged)
	{
		// Set the last edit type
		m_nLastEdit = IDS_PASTE;

		// Update our hosting editor.
		m_pTempoMgr->OnDataChanged();

		// Redraw our strip
		m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		m_pTempoMgr->OnShowProperties();
		if( m_pTempoMgr->m_pPropPageMgr )
		{
			m_pTempoMgr->m_pPropPageMgr->RefreshData();
		}

	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Insert

HRESULT CTempoStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_lXPos < 0 )
	{
		return E_FAIL;
	}

	// Unselect all items in this strip 
	m_pTempoMgr->UnselectAll();
	
	// Unselect items in other strips
	UnselectGutterRange();

	if( !m_pTempoMgr->m_pTimeline )
	{
		return E_FAIL;
	}

	// BUGBUG: Need to fix this.  Should check if m_lXPos is valid, if not, use the time cursor.
	long lMeasure, lBeat;
	if( FAILED( m_pTempoMgr->m_pTimeline->PositionToMeasureBeat( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, m_lXPos, &lMeasure, &lBeat ) ) )
	{
		return E_FAIL;
	}
	long lClocks;
	if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &lClocks ) ) )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	CTempoItem* pTempo;
	bool fNewTempo = false;
	pTempo = GetTopTempoFromPoint( m_lXPos );
	if (!pTempo || (pTempo->m_dblTempo != 0.0) )
	{
		pTempo = new CTempoItem;
		fNewTempo = true;
	}

	if(pTempo == NULL)
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pTempo->m_dblTempo = 120;
		pTempo->m_strText.Format("%.2f", pTempo->m_dblTempo);
		pTempo->m_mtTime = lClocks;
		m_pTempoMgr->SetPropTempoBarBeat( (CPropTempo*)pTempo );
		pTempo->m_fSelected = TRUE;
		if( fNewTempo )
		{
			m_pTempoMgr->InsertByAscendingTime(pTempo, FALSE);
		}

		// TODO: Replace NULL by actual area invalidated (in strip coordinates)
		m_pTempoMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
		ShowPropertySheet(m_pTempoMgr->m_pTimeline);
		m_pTempoMgr->OnShowProperties();
			
		if( m_pTempoMgr->m_pPropPageMgr )
		{
			m_pTempoMgr->m_pPropPageMgr->RefreshData();
		}
		m_nLastEdit = IDS_INSERT;
		m_pTempoMgr->OnDataChanged();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::Delete

HRESULT CTempoStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/* Ignore this, since we can be in a Cut() operation.  We can cut blank data, but we can't delete it.
	if( CanDelete() != S_OK )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}
	*/

	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTempoMgr->m_pTimeline != NULL );
	if( m_pTempoMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pTempoMgr->DeleteSelectedTempos();

	m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pTempoMgr->m_pPropPageMgr )
	{
		m_pTempoMgr->m_pPropPageMgr->RefreshData();
	}

	m_nLastEdit = IDS_DELETE;
	m_pTempoMgr->OnDataChanged();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::SelectAll

HRESULT CTempoStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTempoMgr->m_pTimeline != NULL );
	if( m_pTempoMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pTempoMgr->SelectAll();

	m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CanCut

HRESULT CTempoStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pTempoMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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
// CTempoStrip::CanCopy

HRESULT CTempoStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pTempoMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pTempoMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pTempoMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CanPaste

HRESULT CTempoStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	ASSERT( m_pTempoMgr != NULL );
	if( m_pTempoMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfTempoList == 0 )
	{
		m_cfTempoList = RegisterClipboardFormat( CF_TEMPOLIST );
		if( m_cfTempoList == 0 )
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfTempoList );
	}
	// Otherwise, check the clipboard
	else
	{
		hr = E_FAIL;

		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		if( SUCCEEDED( OleGetClipboard(&pIDataObject) ) )
		{
			// Create a new TimelineDataObject
			IDMUSProdTimelineDataObject *pITimelineDataObject;
			if( SUCCEEDED( m_pTempoMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfTempoList);
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


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CanInsert

HRESULT CTempoStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_lXPos >= 0 )
	{
		//BUGBUG: This doesn't seem right.
		CPropTempo* pTempo;
		pTempo = GetTopTempoFromPoint( m_lXPos );
		if( !pTempo || pTempo->m_dblTempo == 0.0 )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CanDelete

HRESULT CTempoStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pTempoMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CanSelectAll

HRESULT CTempoStrip::CanSelectAll( void )
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

	CTempoItem* pTempoItem;

	POSITION pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();
	while( pos )
	{
		pTempoItem = m_pTempoMgr->m_lstTempos.GetNext( pos );

		if( pTempoItem->m_dblTempo != 0.0 )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::QueryContinueDrag

HRESULT CTempoStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// CTempoStrip::GiveFeedback

HRESULT CTempoStrip::GiveFeedback( DWORD dwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( dwEffect );

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
// CTempoStrip::CreateDragImage

CImageList* CTempoStrip::CreateDragImage()
{
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::CreateDataObject

HRESULT	CTempoStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
{
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Convert lPosition from pixels to clocks
	long lMeasure, lBeat;
	if( FAILED( m_pTempoMgr->m_pTimeline->PositionToMeasureBeat( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lPosition, &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}
	long lClocks;
	if( FAILED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &lClocks ) ) )
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

	// Save Selected Tempos into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pTempoMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// mark the tempos as being dragged: this used later for deleting tempos in drag move
		m_pTempoMgr->MarkSelectedTempos(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pTempoMgr->SaveSelectedTempos( pIStream, lClocks ) ) )
		{
			// Place CF_TEMPOLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfTempoList, pIStream ) ) )
			{
				hr = S_OK;
			}
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


// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::DragEnter

HRESULT CTempoStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
// CTempoStrip::DragOver

HRESULT CTempoStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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

	if( (pt.x >= 0) && (CanPasteFromData( m_pITargetDataObject ) == S_OK) )
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
// CTempoStrip::DragLeave

HRESULT CTempoStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

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
// CTempoStrip::Drop

HRESULT CTempoStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER( grfKeyState );

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
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}

				// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
				m_dwOverDragEffect = m_dwDragRMenuEffect;
				m_dwDragRMenuEffect = DROPEFFECT_NONE;
			}
		}
	}

	// Paste data
	// If the drop effect is not 'None' and (we are dragging from another strip,
	// or the drop point is not the start point) try and do the drop
	if( (m_dwOverDragEffect != DROPEFFECT_NONE) && (!m_fInDragDrop || (pt.x != m_lStartDragPosition) ) )
	{
		// BUGBUG: Error messages?
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pTempoMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				long lMeasure;
				long lBeat;

				if( SUCCEEDED( m_pTempoMgr->m_pTimeline->PositionToMeasureBeat( m_pTempoMgr->m_dwGroupBits, 0, pt.x, &lMeasure, &lBeat ) ) )
				{
					if( SUCCEEDED( m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( m_pTempoMgr->m_dwGroupBits, m_pTempoMgr->m_dwIndex, lMeasure, lBeat, &mtTime ) ) )
					{
						BOOL fChanged = FALSE;
						hr = PasteAt( pITimelineDataObject, mtTime, fChanged);
						if( SUCCEEDED ( hr ) )
						{
							*pdwEffect = m_dwOverDragEffect;

							// If we pasted anything
							if( fChanged )
							{
								// Set the last edit type
								m_nLastEdit = IDS_PASTE;

								// If we are pasting data that originated in another strip,
								// or if we just did a copy,
								// update our hosting editor.
								if( !m_fInDragDrop
								||	(m_dwOverDragEffect & DROPEFFECT_COPY) )
								{
									m_pTempoMgr->OnDataChanged();
								}

								// Redraw our strip
								m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

								m_fInDragDrop = FALSE;

								// BUGBUG: This is wrong! - We should use the time of the
								// tempo that the user just dropped at this time (but we don't
								// know which tempo they were dragging..)
								//m_pCurrentlySelectedTempoItem = pTempo;
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
// CTempoStrip::GetTimelineCWnd

CWnd* CTempoStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pTempoMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pTempoMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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
// CTempoStrip::CanPasteFromData

HRESULT CTempoStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfTempoList ) ))
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::GetData

HRESULT CTempoStrip::GetData( void **ppData )
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
		pPPGTrackParams->dwGroupBits = m_pTempoMgr->m_dwGroupBits;
		pPPGTrackParams->dwPropPageFlags = GROUPBITSPPG_GROUPBITS_RO;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pTempoMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pTempoMgr->m_dwProducerOnlyFlags;
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
// CTempoStrip::SetData

HRESULT CTempoStrip::SetData( void *pData )
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
		ASSERT(FALSE);
		/* Can't change group bits settings
		PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

		// Update our group bits setting, if necessary
		if( pPPGTrackParams->dwGroupBits != m_pBaseMgr->m_dwGroupBits )
		{
			m_pBaseMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;

			// Time signature may have changed
			m_pBaseMgr->OnUpdate( GUID_TimeSignature, m_pBaseMgr->m_dwGroupBits, NULL );

			// Notify our editor that we've changed
			m_pBaseMgr->m_nLastEdit = IDS_UNDO_TRACK_GROUP;
			m_pBaseMgr->OnDataChanged();

			// Update m_dwOldGroupBits after the call to OnDataChanged, because it is needed
			// to ensure the StripMgre removes itself correctly from the Timeline's notification
			// list.
			m_pBaseMgr->m_dwOldGroupBits = *(DWORD*)pData;
		}
		*/
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pTempoMgr->m_dwTrackExtrasFlags )
		{
			m_pTempoMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pTempoMgr->m_pTimeline->OnDataChanged( (ITempoMgr*)m_pTempoMgr );
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pTempoMgr->m_dwProducerOnlyFlags )
		{
			m_pTempoMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pTempoMgr->m_pTimeline->OnDataChanged( (ITempoMgr*)m_pTempoMgr );
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
// CTempoStrip::OnShowProperties

HRESULT CTempoStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( !m_pTempoMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pTempoMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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
	m_pTempoMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::OnRemoveFromPageManager

HRESULT CTempoStrip::OnRemoveFromPageManager( void )
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
// CTempoStrip::AddTempo

HRESULT	CTempoStrip::AddTempo( double dblTempo, MUSIC_TIME mtTime )
{
	if( !m_pTempoMgr->m_pTimeline )
	{
		return E_FAIL;
	}

	CTempoItem* pTempo = new CTempoItem;
	if( pTempo == NULL )
	{
		return E_OUTOFMEMORY;
	}

	long lMeasure, lBeat;
	m_pTempoMgr->ClocksToMeasureBeat( mtTime, &lMeasure, &lBeat );

	pTempo->m_mtTime = mtTime; 
	pTempo->m_lMeasure = lMeasure;
	pTempo->m_lBeat = lBeat;
	pTempo->m_lOffset = 0;

	pTempo->m_dblTempo = dblTempo;
	pTempo->m_strText.Format("%.2f", pTempo->m_dblTempo);

	m_pTempoMgr->InsertByAscendingTime( pTempo, TRUE );
	m_pTempoMgr->SyncWithDirectMusic();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::ShowPropertySheet

HRESULT CTempoStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
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
				pIPropSheet->Release();
			}
			punk->Release();
		}
	}
	m_pTempoMgr->OnShowProperties();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::OnLButtonDown

HRESULT CTempoStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( lYPos );

	m_fSingleSelect = FALSE;

	// If we're already dragging, just return
	if( m_pDragImage )
	{
		return S_OK;
	}

	// Validate reference to the timeline
	if( !m_pTempoMgr->m_pTimeline )
	{
		return E_FAIL;
	}

	// Unselect items in other strips
	UnselectGutterRange();

	HRESULT hr = S_OK;

	// Capture mouse so we get the WM_LBUTTONUP message as well.
	// The timeline will release the capture when it receives the
	// WM_LBUTTONUP message.
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pTempoMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// Show Tempo properties
	m_fShowTempoProps = TRUE;

	// See if there is a tempo under the cursor.
	CTempoItem* pTempo = GetTopTempoFromPoint( lXPos );
	if( pTempo && (pTempo->m_dblTempo != 0.0) )
	{
		// There is a "Real' tempo on this beat
		if( wParam & MK_CONTROL )
		{
			if( pTempo->m_fSelected )
			{
				// Do drag/drop
				hr = DoDragDrop( wParam, lXPos );

				// If we cancelled (or didn't do) a drag'n'drop operation.
				if( FAILED(hr) )
				{
					pTempo->m_fSelected = !pTempo->m_fSelected;
					m_pCurrentlySelectedTempoItem = pTempo;
				}
			}
			else
			{
				pTempo->m_fSelected = !pTempo->m_fSelected;
				if( pTempo->m_fSelected )
				{
					// Set fields to initiate drag/drop on next mousemove
					m_dwStartDragButton = (unsigned long)wParam;
					m_lStartDragPosition = lXPos;
					m_fLButtonDown = TRUE;
				}
				m_pCurrentlySelectedTempoItem = pTempo;
			}
		}
		else if( wParam & MK_SHIFT )
		{
			pTempo->m_fSelected = TRUE;
			SelectRange( pTempo );
		}
		else
		{
			if( pTempo->m_fSelected == FALSE )
			{
				m_pTempoMgr->UnselectAll();
				pTempo->m_fSelected = TRUE;
				m_pCurrentlySelectedTempoItem = pTempo;
			}
			else
			{
				m_fSingleSelect = TRUE;
			}
			
			// Set fields to initiate drag/drop on next mousemove
			m_dwStartDragButton = (unsigned long)wParam;
			m_lStartDragPosition = lXPos;
			m_fLButtonDown = TRUE;
		}
	}
	else
	{
		// There is not a "real" tempo on this measure
		hr = S_OK;
		if( pTempo == NULL )
		{
			// Create a "fake" Tempo
			MUSIC_TIME mtTime = 0;
			m_pTempoMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime );
			hr = m_pTempoMgr->CreateTempo( mtTime, pTempo );
		}

		if( SUCCEEDED( hr ) )
		{
			if(wParam & MK_CONTROL)
			{
				pTempo->m_fSelected = !pTempo->m_fSelected;
				m_pCurrentlySelectedTempoItem = pTempo;
			}
			else if( wParam & MK_SHIFT )
			{
				pTempo->m_fSelected = TRUE;
				SelectRange( pTempo );
			}
			else
			{
				// click on empty space deselects all
				m_pTempoMgr->UnselectAll();

				pTempo->m_fSelected = TRUE;
				m_pCurrentlySelectedTempoItem = pTempo;
			}
		}
	}

	// Redraw the strip and refresh the Tempo property page
	m_pTempoMgr->m_pTimeline->StripInvalidateRect( this, NULL, FALSE );
	m_pTempoMgr->OnShowProperties();
	if( m_pTempoMgr->m_pPropPageMgr )
	{
		m_pTempoMgr->m_pPropPageMgr->RefreshData();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::UpdateName

void CTempoStrip::UpdateName()
{
	CString strText, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_pTempoMgr->m_dwGroupBits & (1 << i) )
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

	strTmp.LoadString( IDS_STRIP_NAME );

	m_strName = strText + CString(": ") + strTmp;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::OnRButtonDown

HRESULT CTempoStrip::OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( lYPos );
	ASSERT( m_pTempoMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();

	// Get the item at the mouse click.
	CTempoItem* pTempo = GetTopTempoFromPoint( lXPos );

	if (!pTempo)
	{
		// no Tempo, so create a 'fake' one
		m_pTempoMgr->UnselectAll();

		// now, create the tempo item and insert it into the tempo manager's list
		HRESULT hr;
		MUSIC_TIME mtTime = 0;
		m_pTempoMgr->m_pTimeline->PositionToClocks( lXPos, &mtTime );
		hr = m_pTempoMgr->CreateTempo( mtTime, pTempo);
		if (SUCCEEDED(hr))
		{
			pTempo->m_fSelected = TRUE;
		}

		m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
	}
	else if( m_pTempoMgr->IsSelected() == FALSE )
	{
		m_pTempoMgr->UnselectAll();
		// Select the item at the mouse click (if there is one).
		// Different from commands, which select empty measures.
		pTempo->m_fSelected = TRUE;
		m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
	}
	else if( !(wParam & (MK_CONTROL | MK_SHIFT)) )
	{
		if( pTempo->m_fSelected == FALSE )
		{
			m_pTempoMgr->UnselectAll();
			pTempo->m_fSelected = TRUE;
		}
		m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
	}

	m_fShowTempoProps = TRUE;
	m_pTempoMgr->OnShowProperties();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::UnselectGutterRange

void CTempoStrip::UnselectGutterRange( void )
{
	ASSERT( m_pTempoMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pTempoMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pTempoMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTempoStrip::DoDragDrop

HRESULT CTempoStrip::DoDragDrop(WPARAM mousekeybutton, LONG lXPos)
{
	HRESULT hr = E_FAIL;
	BOOL fDrop = TRUE;

	// drag drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pTempoMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	IDropSource*	pIDropSource;
	DWORD			dwEffect = DROPEFFECT_NONE;

	// Query ourself for our IDropSource interface
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// Create a data object from the selected tempos
		hr = CreateDataObject( &m_pISourceDataObject, lXPos );
		if(SUCCEEDED(hr))
		{
			m_fInDragDrop = TRUE;

			// Create an image to use when dragging tempos
			m_pDragImage = CreateDragImage();
			if(m_pDragImage)
			{
				m_pDragImage->BeginDrag(0, CPoint(8,12));
			}

			// We can always copy tempos.
			DWORD dwOKDragEffects = DROPEFFECT_COPY;
			if(CanCut() == S_OK)
			{
				// If we can Cut(), allow the user to move the tempos as well.
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			// Do the Drag/Drop.
			m_dwStartDragButton = (unsigned long)mousekeybutton;
			m_lStartDragPosition = lXPos;
			hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);

			// Drag/Drop completed, clean up
			m_dwStartDragButton = 0;

			// delete thr drag image
			if(m_pDragImage)
			{
				m_pDragImage->EndDrag();
				delete m_pDragImage;
				m_pDragImage = NULL;
			}

			switch(hr)
			{
			case DRAGDROP_S_DROP:
				// If we did a move, delete the tempos that were selected (since they were
				// dropped somewhere else.
				if(dwEffect & DROPEFFECT_MOVE)
				{
					m_pTempoMgr->DeleteMarked(UD_DRAGSELECT);
					// TODO: replace NULL by actual area invalidated (in strip coordinates)
					m_pTempoMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
					m_pTempoMgr->OnShowProperties();
					if( m_pTempoMgr->m_pPropPageMgr )
					{
						m_pTempoMgr->m_pPropPageMgr->RefreshData();
					}
					if( m_fInDragDrop )
					{
						m_nLastEdit = IDS_DELETE;
					}
					else
					{
						m_nLastEdit = IDS_MOVE;
					}

					// We just deleted some data from this strip, update our hosting editor.
					m_pTempoMgr->OnDataChanged();
					m_pTempoMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
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
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;

			m_fInDragDrop = FALSE;
		}
		else
		{
			pIDropSource->Release();
			return E_FAIL;
		}
		pIDropSource->Release();
		m_pTempoMgr->UnMarkTempos(UD_DRAGSELECT);
	}
	else
	{
		return E_FAIL;
	}

	// The Drag and Drop methods should handle redrawing the strip and updating the
	// property pages.
	if( fDrop )
	{
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

// CTempoStrip::SelectRange()

HRESULT CTempoStrip::SelectRange(CTempoItem* pTempo)
{
	HRESULT hr = S_OK;
	// select inclusive, find tempo to start from
	CTempoItem* pCurrent = m_pCurrentlySelectedTempoItem;

	// find measure and beat for tempo to start from and tempo to end at
	long lEarlyBeat = (pCurrent) ? pCurrent->m_lBeat : 0;
	long lEarlyMeasure = (pCurrent) ? pCurrent->m_lMeasure : 0;
	long lLateBeat = pTempo->m_lBeat;
	long lLateMeasure = pTempo->m_lMeasure;

	// if the order is backwards, swap
	if ( lLateMeasure < lEarlyMeasure ||
		 (lLateMeasure == lEarlyMeasure && lLateBeat < lEarlyBeat) )
	{
		long lTemp = lEarlyBeat;
		lEarlyBeat = lLateBeat;
		lLateBeat = lTemp;

		lTemp = lEarlyMeasure;
		lEarlyMeasure = lLateMeasure;
		lLateMeasure = lTemp;
	}

	// add empty empty tempos to fill in gaps
	long lCurrentBeat = lEarlyBeat;
	long lCurrentMeasure = lEarlyMeasure;
	CTempoItem* pTempoItem = NULL;
	while( lCurrentMeasure <= lLateMeasure &&
		   ( lCurrentBeat <= lLateBeat || lCurrentMeasure < lLateMeasure ) )
	{
		if ( !m_pTempoMgr->FindTempo(lCurrentMeasure, lCurrentBeat) )
		{
			MUSIC_TIME mtPosition = 0;
			m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( 
				m_pTempoMgr->m_dwGroupBits, 
				0, 
				lCurrentMeasure, 
				lCurrentBeat, 
				&mtPosition );
			pTempoItem = NULL;
			hr = m_pTempoMgr->CreateTempo(mtPosition, pTempoItem);
		}
		lCurrentBeat++;
		DMUS_TIMESIGNATURE TS;
		long lClocks = 0;
		m_pTempoMgr->m_pTimeline->MeasureBeatToClocks( 
				m_pTempoMgr->m_dwGroupBits, 
				0, 
				lCurrentMeasure, 
				lCurrentBeat, 
				&lClocks );
		hr = m_pTempoMgr->m_pTimeline->GetParam(
			GUID_TimeSignature, 
			m_pTempoMgr->m_dwGroupBits, 
			0, 
			lClocks, 
			NULL, 
			(void*) &TS);
		ASSERT(SUCCEEDED(hr));
		BYTE bBeats = 4;  // assume 4/4 if the above call fails
		if (SUCCEEDED(hr))
		{
			bBeats = TS.bBeatsPerMeasure;
		}
		if (lCurrentBeat >= bBeats)
		{
			lCurrentBeat = 0;
			lCurrentMeasure++;
		}
	}
	m_pTempoMgr->UnselectAll();
	m_pCurrentlySelectedTempoItem = pCurrent;

	// select all tempos in the range (can't simply select tempos in the
	// above loop, because there may be multiple tempos at the same
	// measure/beat)
	POSITION pos = m_pTempoMgr->m_lstTempos.GetHeadPosition();
	pTempoItem = NULL;
	while( pos )
	{
		pTempoItem = m_pTempoMgr->m_lstTempos.GetNext( pos );
		ASSERT( pTempoItem );
		if( pTempoItem->m_lMeasure >= lEarlyMeasure &&
			pTempoItem->m_lMeasure <= lLateMeasure &&
			( pTempoItem->m_lMeasure != lEarlyMeasure || 
			  pTempoItem->m_lBeat >= lEarlyBeat) &&
			( pTempoItem->m_lMeasure != lLateMeasure || 
			  pTempoItem->m_lBeat <= lLateBeat) )
		{
			pTempoItem->m_fSelected = TRUE;
		}
	}
	return hr;
}


long CTempoStrip::GetBeatWidth(long measure)
// get positional width of beat
{
	// Determine width of a beat
	long lBeatBeginPosition = 0;
	long lBeatEndPosition = 0;
	m_pTempoMgr->m_pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, measure, 0, &lBeatBeginPosition );
	m_pTempoMgr->m_pTimeline->MeasureBeatToPosition( m_pTempoMgr->m_dwGroupBits, 0, measure, 1, &lBeatEndPosition );
	long lBeatLength = lBeatEndPosition - lBeatBeginPosition;
	return lBeatLength;
}