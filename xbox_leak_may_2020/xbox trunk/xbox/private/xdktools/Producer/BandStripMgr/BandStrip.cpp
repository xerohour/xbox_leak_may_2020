// BandStrip.cpp : Implementation of CBandStrip
#include "stdafx.h"
#include "BandIO.h"
#include "BandStripMgr.h"
#include "BandMgr.h"
#include "PropPageMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include <SegmentGuids.h>
#include <RiffStrm.h>
#include <dmusicf.h>
#include "musictimeconverter.h"
#include "SegmentIO.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STRIP_HEIGHT 20

//BEGIN_MESSAGE_MAP(CBandStrip)
//	//{{AFX_MSG_MAP(CBandStrip)
//	ON_WM_SIZE()
//	ON_WM_DESTROY()
//	ON_COMMAND(IDM_DRAG_MOVE, OnDragRMenuMove)
//	ON_COMMAND(IDM_DRAG_COPY, OnDragRMenuCopy)
//	ON_COMMAND(IDM_DRAG_CANCEL, OnDragRMenuCancel)
//	ON_WM_DROPFILES()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

void SendAudiopathToNode( IDMUSProdNode* pIBandNode, IDirectMusicSegmentState8 *pSegmentState8 )
{
	if( pIBandNode == NULL )
	{
		return;
	}

	IUnknown *punkAudiopath = NULL;
	if( pSegmentState8 )
	{
		pSegmentState8->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_AUDIOPATH, 0, GUID_NULL, 0, IID_IUnknown, (void **)&punkAudiopath );
	}

	IDMUSProdBandEdit8a *pIDMUSProdBandEdit8a;
	if( SUCCEEDED( pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIDMUSProdBandEdit8a ) ) )
	{
		pIDMUSProdBandEdit8a->SetAudiopath( punkAudiopath );
		pIDMUSProdBandEdit8a->Release();
	}

	if( punkAudiopath )
	{
		punkAudiopath->Release();
	}
}

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

void CBandStrip::OnDragRMenuMove(  )
{
	m_dwDragRMenuEffect = DROPEFFECT_MOVE;
}

void CBandStrip::OnDragRMenuCopy(  )
{
	m_dwDragRMenuEffect = DROPEFFECT_COPY;
}

void CBandStrip::OnDragRMenuCancel(  )
{
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip constructor/destructor

CBandStrip::CBandStrip( CBandMgr* pBandMgr )
{
	ASSERT( pBandMgr );
	if ( pBandMgr == NULL )
	{
		return;
	}

	m_pBandMgr = pBandMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pBandMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_lXPos = -1;
	m_bGutterSelected = FALSE;
	m_bSelecting = FALSE;
	m_bContextMenuPaste = FALSE;
	m_lBeginSelect = 0;
	m_lEndSelect = 0;
	m_pISourceDataObject = NULL;
	m_pITargetDataObject = NULL;
	m_nStripIsDragDropSource = 0;
	m_dwStartDragButton = 0;
	m_lStartDragPosition = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_nLastEdit = 0;
	m_fShowBandProps = FALSE;
	m_fPropPageActive = FALSE;
	m_fSingleSelect = FALSE;
	m_pPropPageMgr = NULL;
	m_pStripNode = NULL;
}

CBandStrip::~CBandStrip()
{
	ASSERT( m_pStripMgr );
	if ( m_pStripMgr )
	{
		//m_pStripMgr->Release();
		m_pStripMgr = NULL;
		m_pBandMgr = NULL;
	}

	RELEASE( m_pISourceDataObject );
	RELEASE( m_pITargetDataObject );
	RELEASE( m_pPropPageMgr );
	RELEASE( m_pStripNode );
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::QueryInterface

STDMETHODIMP CBandStrip::QueryInterface( REFIID riid, LPVOID *ppv )
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
// CBandStrip::AddRef

STDMETHODIMP_(ULONG) CBandStrip::AddRef(void)
{
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Release

STDMETHODIMP_(ULONG) CBandStrip::Release(void)
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
// CBandStrip::GetBeatWidth

long CBandStrip::GetBeatWidth( DWORD dwMeasure )
{
	ASSERT( m_pBandMgr->m_pTimeline != NULL );

	// Determine width of a beat
	long lBeatBeginPosition = 0;
	m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0,
												    dwMeasure, 0, &lBeatBeginPosition );

	long lBeatEndPosition = 0;
	m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0,
													dwMeasure, 1, &lBeatEndPosition );
	
	return (lBeatEndPosition - lBeatBeginPosition);
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Draw

bool IsRealBand(CBandItem* pBand)
{
	ASSERT(pBand);
	if(!pBand)
		return false;
	return (pBand->m_strText.IsEmpty() == FALSE) ? true : false;
}

HRESULT	STDMETHODCALLTYPE CBandStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDC	dc;

	bool fUseGutterSelectRange = m_bGutterSelected && m_lBeginSelect != m_lEndSelect;

	// Get a pointer to the Timeline
	if( m_pBandMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pBandMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pBandMgr->m_dwGroupBits, 0, lXOffset );

		// Attach to our device context
		if( dc.Attach(hDC) )
		{
			CRect	rectClip, rectHighlight;
			long	lPosition;
			CBandItem* pBandItem;
			CBandItem* pTopBandItem;
			CBandItem* pNextBandItem;
			DWORD dwCurrentMeasure = 0xFFFFFFFF;
			BYTE bCurrentBeat = 0xFF;
			CSize	csize;
			POSITION posNext;
			POSITION pos;

			// ghost band vars
			CBandItem*	pGhostBand = NULL;
			long lFirstVisibleMeasure, lPartialVisibleMeasure;
			long lFirstVisibleBeat, lPartialVisibleBeat;
			long lClocks;
			RECT rectGhost;
			CString strGhost;
			bool bUseItalicsForGhost = false;
			bool bGhostBandCovered = false;	// indicates real band covering ghost band

			// Get clip box
			dc.GetClipBox( &rectClip );
			rectHighlight.top = 0;
			rectHighlight.bottom = STRIP_HEIGHT;
			rectGhost.top = 0;
			rectGhost.bottom = STRIP_HEIGHT;

			// Create Italics font
			BOOL fHaveItalics = FALSE;
			CFont fontItalics;
			CFont* pFont;
			LOGFONT logfont;
			pFont = dc.GetCurrentFont();
			if( pFont )
			{
				pFont->GetLogFont( &logfont );
				logfont.lfItalic = TRUE;
				if( fontItalics.CreateFontIndirect( &logfont ) )
				{
					fHaveItalics = TRUE;
				}
			}

			// find first visible measure/beat for ghosting bands
			m_pBandMgr->m_pTimeline->GetMarkerTime(MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lClocks);
			CMusicTimeConverter cmtFirstVisible(lClocks);
			cmtFirstVisible.GetMeasureBeat(lPartialVisibleMeasure, lPartialVisibleBeat,
											m_pBandMgr->m_pTimeline, m_pBandMgr->m_dwGroupBits);
			CMusicTimeConverter cmtNearestMeasureBeat(lPartialVisibleMeasure, lPartialVisibleBeat, 
											m_pBandMgr->m_pTimeline, m_pBandMgr->m_dwGroupBits);
			if(cmtNearestMeasureBeat.Time() < cmtFirstVisible.Time())
			{
				// round up
				cmtNearestMeasureBeat.AddOffset(0, 1, m_pBandMgr->m_pTimeline, m_pBandMgr->m_dwGroupBits);
				cmtNearestMeasureBeat.GetMeasureBeat(lFirstVisibleMeasure, lFirstVisibleBeat, 
													m_pBandMgr->m_pTimeline, m_pBandMgr->m_dwGroupBits);
			}
			else
			{
				lFirstVisibleMeasure = lPartialVisibleMeasure;
				lFirstVisibleBeat = lPartialVisibleBeat;
			}

			// find nearest band before first visible measure/beat
			pos = m_pBandMgr->m_lstBands.GetHeadPosition();
			while( pos )
			{
				pBandItem = m_pBandMgr->m_lstBands.GetNext(pos);
				if(pBandItem->m_dwMeasure > (DWORD)lFirstVisibleMeasure
					|| ((pBandItem->m_dwMeasure == (DWORD)lFirstVisibleMeasure) && (pBandItem->m_bBeat >= (DWORD)lFirstVisibleBeat)))
				{
					break;
				}
//				else
//				{
					// we may override this later
//					bUseItalicsForGhost = false;
//				}
				m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, 
									pBandItem->m_dwMeasure, pBandItem->m_bBeat, &lPosition );
				lPosition++;
				
				if( pBandItem->m_dwMeasure != dwCurrentMeasure
				||  pBandItem->m_bBeat != bCurrentBeat )
				{
					dwCurrentMeasure = pBandItem->m_dwMeasure;
					bCurrentBeat = pBandItem->m_bBeat;

					pTopBandItem = GetTopBandFromMeasureBeat( dwCurrentMeasure, bCurrentBeat );
					if(pTopBandItem)
					{
						if(IsRealBand(pTopBandItem))
						{
							bUseItalicsForGhost = false;	// may be overridden later
							pGhostBand = pTopBandItem;
							if(pTopBandItem != pBandItem && IsRealBand(pBandItem))
							{
								bUseItalicsForGhost = true;
							}
							else
							{
								// either pBandItem is the top band of many or it is a singleton
								// in either case we must get to the end of list of measure/beat
								// coincident bands
								posNext = pos;
								while(posNext)
								{
									pNextBandItem = m_pBandMgr->m_lstBands.GetNext(posNext);
									if(pNextBandItem->m_dwMeasure != dwCurrentMeasure
										|| pNextBandItem->m_bBeat != bCurrentBeat)
									{
										//end of measure/beat coincident bands
										posNext = NULL;
									}
									else
									{
										if(pNextBandItem != pTopBandItem
											&& IsRealBand(pNextBandItem))
										{
											bUseItalicsForGhost = true;
										}
									}
								}
							}
						}
					}
				}
			}

			if(pGhostBand)
			{
				// get rect of ghost band's text, we may have to truncate it if it covers real band
				m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, 
									lFirstVisibleMeasure, lFirstVisibleBeat, &lPosition );
				lPosition++;
				CSize csize;
				csize = dc.GetTextExtent(pGhostBand->m_strText);
				rectGhost.left = lPosition-lXOffset;
				rectGhost.right = rectGhost.left + csize.cx;
			}

			// we'll draw the ghost after checking whether or not band covers it

			// Draw the names of all Bands in the strip
			dwCurrentMeasure = 0xFFFFFFFF;
			bCurrentBeat = 0xFF;
			pos = m_pBandMgr->m_lstBands.GetHeadPosition();
			while( pos )
			{
				pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );

				m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, 
									pBandItem->m_dwMeasure, pBandItem->m_bBeat, &lPosition );
				lPosition++;
				
				if( pBandItem->m_dwMeasure != dwCurrentMeasure
				||  pBandItem->m_bBeat != bCurrentBeat )
				{
					dwCurrentMeasure = pBandItem->m_dwMeasure;
					bCurrentBeat = pBandItem->m_bBeat;

					pTopBandItem = GetTopBandFromMeasureBeat( dwCurrentMeasure, bCurrentBeat );
					ASSERT( pTopBandItem != NULL );		// Should never be NULL!
					if( pTopBandItem )
					{
						CFont* pFontOld = NULL;
						if( fHaveItalics )
						{
							if( pTopBandItem->m_strText.IsEmpty() == FALSE )
							{
								if( pTopBandItem != pBandItem
								&&  pBandItem->m_strText.IsEmpty() == FALSE )
								{
									pFontOld = dc.SelectObject( &fontItalics );
								}
								else
								{
									posNext = pos;
									while( posNext )
									{
										pNextBandItem = m_pBandMgr->m_lstBands.GetNext( posNext );

										if( pNextBandItem->m_dwMeasure != dwCurrentMeasure
										||  pNextBandItem->m_bBeat != bCurrentBeat )
										{
											posNext = NULL;
										}
										else
										{
											if( pNextBandItem != pTopBandItem
											&&  pNextBandItem->m_strText.IsEmpty() == FALSE )
											{
												pFontOld = dc.SelectObject( &fontItalics );
												posNext = NULL;
											}
										}
									}
								}
							}
						}

						// Determine width of a beat
						long lBeatLength = GetBeatWidth( pTopBandItem->m_dwMeasure );

						// Find extent of text
						csize = dc.GetTextExtent( pTopBandItem->m_strText );
						if( csize.cx < lBeatLength )
						{
							csize.cx = lBeatLength;
						}
						long lExtent = lPosition + csize.cx;

						// Truncate if next Band covers part of text
						POSITION pos2 = pos;
						CBandItem* pTempBand;
						bool fDone = false;
						while( pos2 && !fDone )
						{
							pTempBand = m_pBandMgr->m_lstBands.GetNext( pos2 );

							if( pTempBand->m_strText.IsEmpty() == FALSE )
							{
								if( pTempBand->m_dwMeasure != dwCurrentMeasure
								||  pTempBand->m_bBeat != bCurrentBeat )
								{
									long lRPos;
									m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, 
										pTempBand->m_dwMeasure, pTempBand->m_bBeat, &lRPos );
									if( lRPos <= lExtent )
									{
										lExtent = lRPos;
									}
									fDone = true;
								}
							}
						}


						rectHighlight.left = lPosition - lXOffset;
						rectHighlight.right = lExtent - lXOffset;

						// truncate ghost band if necessary
						if(pGhostBand)
						{
							bool b1 = pTopBandItem->m_dwMeasure == (DWORD)lFirstVisibleMeasure
								 && pTopBandItem->m_bBeat == lFirstVisibleBeat;
							bool b2 = pTopBandItem->m_dwMeasure == (DWORD)lPartialVisibleMeasure
									&& rectHighlight.right > LeftMargin(m_pBandMgr->m_pTimeline) - lXOffset;
							if(b1 || b2)
							{
								bGhostBandCovered = true;
							}
							else if(rectHighlight.left > rectGhost.left
										&& rectHighlight.left < rectGhost.right)
							{
								rectGhost.right = rectHighlight.left;
							}

						}


						dc.DrawText( pTopBandItem->m_strText, rectHighlight, (DT_LEFT | DT_NOPREFIX) );
						
						if( pFontOld )
						{
							dc.SelectObject( pFontOld );
						}
					}
				}
				
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}
			}

			// finally draw ghost band
			if(pGhostBand && !bGhostBandCovered)
			{
				CFont* pOldFont = 0;
				if(bUseItalicsForGhost && fHaveItalics)
				{
					pOldFont = dc.SelectObject(&fontItalics);
				}
				COLORREF cr = dc.SetTextColor(RGB(168,168,168));
				dc.DrawText( pGhostBand->m_strText, &rectGhost, (DT_LEFT | DT_NOPREFIX) );

				if(pOldFont)
				{
					dc.SelectObject (pOldFont);
				}
				dc.SetTextColor(cr);
			}

			// Make sure the names of selected Bands are shown in their entirety
			DWORD dwLastMeasure = 0xFFFFFFFF;
			BYTE bLastBeat = 0xFF;
			dwCurrentMeasure = 0xFFFFFFFF;
			bCurrentBeat = 0xFF;
			pos = m_pBandMgr->m_lstBands.GetHeadPosition();
			while( pos )
			{
				pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );

				m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, pBandItem->m_dwMeasure, pBandItem->m_bBeat, &lPosition );

				if( pBandItem->m_fSelected )
				{
					if( pBandItem->m_dwMeasure != dwCurrentMeasure
					||  pBandItem->m_bBeat != bCurrentBeat )
					{
						dwCurrentMeasure = pBandItem->m_dwMeasure;
						bCurrentBeat = pBandItem->m_bBeat;

						pTopBandItem = GetTopBandFromMeasureBeat( dwCurrentMeasure, bCurrentBeat );
						ASSERT( pTopBandItem != NULL );		// Should never be NULL!
						if( pTopBandItem )
						{
							CFont* pFontOld = NULL;
							if( fHaveItalics )
							{
								if( pTopBandItem->m_strText.IsEmpty() == FALSE )
								{
									if( pTopBandItem != pBandItem
									&&  pBandItem->m_strText.IsEmpty() == FALSE )
									{
										pFontOld = dc.SelectObject( &fontItalics );
									}
									else if( dwLastMeasure == pBandItem->m_dwMeasure
										 &&  bLastBeat == pBandItem->m_bBeat )
									{
										pFontOld = dc.SelectObject( &fontItalics );
									}
									else
									{
										posNext = pos;
										while( posNext )
										{
											pNextBandItem = m_pBandMgr->m_lstBands.GetNext( posNext );

											if( pNextBandItem->m_dwMeasure != dwCurrentMeasure
											||  pNextBandItem->m_bBeat != bCurrentBeat )
											{
												posNext = NULL;
											}
											else
											{
												if( pNextBandItem != pTopBandItem
												&&  pNextBandItem->m_strText.IsEmpty() == FALSE )
												{
													pFontOld = dc.SelectObject( &fontItalics );
													posNext = NULL;
												}
											}
										}
									}
								}
							}

							if( pTopBandItem != pBandItem )
							{
								if( pTopBandItem->m_fSelected == FALSE )
								{
									pTopBandItem = pBandItem;
								}

								pBandItem->m_wFlags &= ~BF_TOPBAND;
								pTopBandItem->m_wFlags |= BF_TOPBAND;
							}

							// Determine width of a beat
							long lBeatLength = GetBeatWidth( pTopBandItem->m_dwMeasure );

							// find extent of text
							csize = dc.GetTextExtent( pTopBandItem->m_strText );
							if( csize.cx < lBeatLength )
							{
								csize.cx = lBeatLength;
							}
							long lExtent = lPosition + csize.cx;

							// truncate if a selected Band covers part of text
							POSITION pos2 = pos;
							CBandItem* pTempBand;
							bool fDone = false;
							while( pos2 && !fDone )
							{
								pTempBand = m_pBandMgr->m_lstBands.GetNext( pos2 );

								if( pTempBand->m_dwMeasure != dwCurrentMeasure
								||  pTempBand->m_bBeat != bCurrentBeat )
								{
									if( pTempBand->m_fSelected)
									{	
										long lRPos;
										m_pBandMgr->m_pTimeline->MeasureBeatToPosition( m_pBandMgr->m_dwGroupBits, 0, pTempBand->m_dwMeasure, pTempBand->m_bBeat, &lRPos );
										if( lRPos <= lExtent )
										{
											lExtent = lRPos;
										}
										fDone = true;
									}
								}
							}

							if( !fUseGutterSelectRange )
							{
								rectHighlight.left = lPosition - lXOffset;
								rectHighlight.right = lExtent - lXOffset;
								dc.DrawText( pTopBandItem->m_strText, rectHighlight, (DT_LEFT | DT_NOPREFIX) );
								GrayOutRect( dc.m_hDC, rectHighlight );
							}

							if( pFontOld )
							{
								dc.SelectObject( pFontOld );
							}
						}
					}
				}
			
				if( lPosition - lXOffset > rectClip.right )
				{
					break;
				}

				if( pBandItem->m_strText.IsEmpty() == FALSE )
				{
					dwLastMeasure = pBandItem->m_dwMeasure;
					bLastBeat = pBandItem->m_bBeat;
				}
			}

			// Highlight the selected range if there is one.
			if( fUseGutterSelectRange )
			{
				long lBeginSelect = m_lBeginSelect > m_lEndSelect ? m_lEndSelect : m_lBeginSelect;
				long lEndSelect = m_lBeginSelect > m_lEndSelect ? m_lBeginSelect : m_lEndSelect;

				long lMeasure;
				long lBeat;
				long lClocks;

				m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, lBeginSelect, &lMeasure, &lBeat );
				m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
				m_pBandMgr->m_pTimeline->ClocksToPosition( lClocks, &(rectHighlight.left));

				m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, lEndSelect, &lMeasure, &lBeat );
				m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, (lBeat + 1), &lClocks );
				m_pBandMgr->m_pTimeline->ClocksToPosition( (lClocks - 1), &(rectHighlight.right));

				rectHighlight.left -= lXOffset;
				rectHighlight.right -= lXOffset;

				// Invert it
				GrayOutRect( dc.m_hDC, rectHighlight );
			}

			if( fHaveItalics )
			{
				fontItalics.DeleteObject();
			}

			dc.Detach();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CBandStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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
			// Get "Bands" text
			CString strBands;
			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( _Module.GetModuleInstance() );
			strBands.LoadString( IDS_BANDS_TEXT );
			AfxSetResourceHandle( hInstance );

			BSTR bstr;
			CString str = GetName(m_pBandMgr->m_dwGroupBits, strBands);

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
		if( m_pBandMgr )
		{
			m_pBandMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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
// CBandStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CBandStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
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
			m_lBeginSelect = V_I4( &var );
		}
		else
		{
			m_lEndSelect = V_I4( &var );
		}

		if( m_bSelecting )
		{
			break;
		}

		if( m_lBeginSelect == m_lEndSelect )
		{	
			m_pBandMgr->UnselectAll();
			m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}
		if( m_bGutterSelected )
		{
			m_pBandMgr->UnselectAll();
			m_pBandMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
		}
		else
		{
			m_pBandMgr->UnselectAll();
		}
		m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pBandMgr->m_pPropPageMgr != NULL )
		{
			m_pBandMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);

		if( m_lBeginSelect == m_lEndSelect )
		{	
			m_pBandMgr->UnselectAll();
			m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}
		if( m_bGutterSelected )
		{
			m_pBandMgr->UnselectAll();
			m_pBandMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
		}
		else
		{
			m_pBandMgr->UnselectAll();
		}
		m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pBandMgr->m_pPropPageMgr != NULL )
		{
			m_pBandMgr->m_pPropPageMgr->RefreshData();
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::GetTopBandFromMeasureBeat

CBandItem *CBandStrip::GetTopBandFromMeasureBeat( DWORD dwMeasure, BYTE bBeat )
{
	CBandItem* pTheBandItem = NULL;
	CBandItem* pFirstBandItem = NULL;
	CBandItem* pBandItem;

	POSITION pos = m_pBandMgr->m_lstBands.GetHeadPosition();
	while( pos )
	{
		pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );

		if( pBandItem->m_dwMeasure == dwMeasure
		&&  pBandItem->m_bBeat == bBeat )
		{
			if( pFirstBandItem == NULL )
			{
				pFirstBandItem = pBandItem;
			}
			else
			{
				if( pFirstBandItem->m_strText.IsEmpty() )
				{
					pFirstBandItem = pBandItem;
				}
			}

			if( pBandItem->m_wFlags & BF_TOPBAND )
			{
				pTheBandItem = pBandItem;
				break;
			}
		}

		if( pBandItem->m_dwMeasure > dwMeasure )
		{
			break;
		}
	}

	if( pTheBandItem == NULL )
	{
		pTheBandItem = pFirstBandItem;
	}

	return pTheBandItem;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::GetBandFromPoint

CBandItem *CBandStrip::GetBandFromPoint( long lPos )
{
	CBandItem*		pBandReturn = NULL;

	if( m_pBandMgr->m_pTimeline != NULL )
	{
		CBandItem* pBandItem = NULL;
		long lMeasure, lBeat;
		if( SUCCEEDED( m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, lPos, &lMeasure, &lBeat ) ) )
		{
			 //BUGBUG: validate range of lBeat
			pBandReturn = GetTopBandFromMeasureBeat( lMeasure, (BYTE)lBeat );
		}
	}
	return pBandReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CBandStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	CBandItem* pBand = NULL;
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		// Save the position of the click
		m_lXPos = lXPos;
		m_fShowBandProps = TRUE;
		hr = OnLButtonDown( wParam, lParam, lXPos, lYPos );
		break;

	case WM_LBUTTONDBLCLK:
		{
			long lMeasure;
			long lBeat;
			hr = m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
			// use the measure and beat to get a band item
			CBandItem* pTopBandItem = GetTopBandFromMeasureBeat( (DWORD)lMeasure, (BYTE)lBeat );
			if( pTopBandItem
			&&  pTopBandItem->m_pIBandNode )
			{
				hr = pTopBandItem->m_pIBandNode->SetDocRootNode( m_pStripNode );
				hr = m_pBandMgr->m_pDMProdFramework->OpenEditor( pTopBandItem->m_pIBandNode );
			}
		}
		break;

	case WM_MOUSEMOVE:
		if( m_dwStartDragButton )
		{
			m_nStripIsDragDropSource = 1;
	
			if( DoDragDrop() )
			{
				// Redraw the strip and refresh the Band property page
				m_pBandMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				m_pBandMgr->OnShowProperties();
				if( m_pBandMgr->m_pPropPageMgr )
				{
					m_pBandMgr->m_pPropPageMgr->RefreshData();
				}
			}

			m_dwStartDragButton = 0;
			m_nStripIsDragDropSource = 0;
		}
		break;

	case WM_RBUTTONDOWN:
	{
		ASSERT( m_pBandMgr != NULL );
		ASSERT( m_pBandMgr->m_pTimeline != NULL );

		// Save the position of the click so we know where to insert a Band. if Insert is selected.
		m_lXPos = lXPos;

		// Get Timeline length
		VARIANT var;
		m_pBandMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lTimelineLength = V_I4( &var );

		// Exit if user clicked past end of Strip
		long lClock;
		m_pBandMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
		if( lClock >= lTimelineLength )
		{
			break;
		}

		// Make sure everything on the timeline is deselected first.
		UnselectGutterRange();

		pBand = GetBandFromPoint( lXPos );
		if (!pBand)
		{
			// no band, so create one with an empty band node.
			m_pBandMgr->UnselectAll();
			// now, create the band node and insert it into the band manager's list
			hr = m_pBandMgr->CreateBand( lXPos, this, pBand);
			if (SUCCEEDED(hr))
			{
				pBand->SetSelectFlag( TRUE );
			}

			m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
		}
		else if( m_pBandMgr->IsSelected() == FALSE )
		{
			m_pBandMgr->UnselectAll();
			// Select the item at the mouse click (if there is one).
			// Different from commands, which select empty measures.
			pBand->SetSelectFlag( TRUE );
			m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
		}
		else if( !( wParam & ( MK_SHIFT | MK_CONTROL )))
		{
			// Make sure this band is selected.  If it isn't, make it the only one selected
			if( !pBand->m_fSelected)
			{
				m_pBandMgr->UnselectAll();
				pBand->SetSelectFlag( TRUE );
			}
			m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
		}

		m_fShowBandProps = TRUE;
		m_pBandMgr->OnShowProperties(); 
		break;
	}

	case WM_RBUTTONUP:
	{
		// Get the item at the mouse click.
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

		// Save the position of the click so we know where to insert a Band. if Insert is selected.
		m_lXPos = lXPos;

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
		::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( RightClickMenuCanSelectAll() == S_OK ) ? MF_ENABLED :
				MF_GRAYED );
		::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
		::EnableMenuItem( hMenuPopup, IDM_CYCLE_BANDS, ( m_pBandMgr->CanCycle(m_lXPos) == TRUE ) ? MF_ENABLED :
				MF_GRAYED );

		m_pBandMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
		::DestroyMenu( hMenu );

		hr = S_OK;
		break;
	}

	case WM_COMMAND:
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			hr = ShowPropertySheet(m_pBandMgr->m_pTimeline);
			if( m_fShowBandProps )
			{
				// Change to the Band property page
				m_pBandMgr->OnShowProperties();
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
			if( m_lXPos >= 0 )
			{
				m_bContextMenuPaste = TRUE;
			}
			hr = Paste( NULL );
			m_bContextMenuPaste = FALSE;
			break;
		case ID_EDIT_INSERT:
			hr = Insert();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		case IDM_CYCLE_BANDS:
			hr = m_pBandMgr->CycleBands( m_lXPos );
			break;
		case IDM_DRAG_MOVE:
			OnDragRMenuMove();
			break;
		case IDM_DRAG_COPY:
			OnDragRMenuCopy();
			break;
		case IDM_DRAG_CANCEL:
			OnDragRMenuCancel();
			break;
		case ID_OPEN_BANDEDITOR:
			if( m_pBandMgr->m_pBandItemForEditBandButton
			&&  m_pBandMgr->m_pBandItemForEditBandButton->m_pIBandNode )
			{
				// Select just this band
				m_pBandMgr->UnselectAll();
				m_pBandMgr->m_pBandItemForEditBandButton->m_fSelected = true;

				hr = m_pBandMgr->m_pBandItemForEditBandButton->m_pIBandNode->SetDocRootNode( m_pStripNode );
				hr = m_pBandMgr->m_pDMProdFramework->OpenEditor( m_pBandMgr->m_pBandItemForEditBandButton->m_pIBandNode );
			}
			break;
		case ID_LOCATE_BAND:
			if( m_pBandMgr->m_pBandItemForEditBandButton
			&&  m_pBandMgr->m_pBandItemForEditBandButton->m_pIBandNode )
			{
				// Select just this band
				m_pBandMgr->UnselectAll();
				m_pBandMgr->m_pBandItemForEditBandButton->SetSelectFlag( TRUE );
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				m_pBandMgr->m_pBandItemForEditBandButton->m_dwBits |= UD_CURRENTSELECTION;
				m_fSingleSelect = FALSE;

				// Center the display horizontally on this band
				long lLeftDisplay, lRightDisplay;
				m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftDisplay );
				m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_RIGHTDISPLAY, TIMETYPE_CLOCKS, &lRightDisplay );

				lLeftDisplay = m_pBandMgr->m_pBandItemForEditBandButton->m_mtTimeLogical - ((lRightDisplay - lLeftDisplay) / 2);
				m_pBandMgr->m_pTimeline->ClocksToPosition( lLeftDisplay, &lLeftDisplay );

				VARIANT var;
				var.vt = VT_I4;
				V_I4(&var) = lLeftDisplay;
				m_pBandMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

				// Activate this strip
				var.vt = VT_UNKNOWN;
				V_UNKNOWN(&var) = static_cast<IDMUSProdStrip*>(this);
				V_UNKNOWN(&var)->AddRef();
				m_pBandMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );

				// Display this strip at the top of the display
				var.vt = VT_UNKNOWN;
				V_UNKNOWN(&var) = static_cast<IDMUSProdStrip*>(this);
				V_UNKNOWN(&var)->AddRef();
				m_pBandMgr->m_pTimeline->SetTimelineProperty( TP_TOP_STRIP, var );

				// Redraw the strip and refresh the Band property page
				m_pBandMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				m_pBandMgr->OnShowProperties();
				if( m_pBandMgr->m_pPropPageMgr )
				{
					m_pBandMgr->m_pPropPageMgr->RefreshData();
				}
			}
			break;
		case ID_INSERT_NEWINSTRUMENT:
			{
				// Get 'now'
				MUSIC_TIME mtNow;
				if( FAILED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtNow ) ) )
				{
					return E_UNEXPECTED;
				}

				// Look for a band at exactly tick -1 of the current beat
				CBandItem *pBand = NULL;
				long lMeasure, lBeat, lTmpTime = mtNow;
				if( SUCCEEDED( m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, mtNow, &lMeasure, &lBeat ) )
				&&	SUCCEEDED( m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lTmpTime ) ) )
				{
					lTmpTime--;

					// Try and find a band exactly at lTimeTime
					pBand = m_pBandMgr->FindBandAtTime( lTmpTime );
				}

				// If we found a real band
				if( pBand
				&&	pBand->m_pIBandNode )
				{
					// Just insert the instrument
					IDMUSProdBandEdit8a *pIDMUSProdBandEdit8a;
					if( SUCCEEDED( pBand->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIDMUSProdBandEdit8a ) ) )
					{
						m_pBandMgr->m_fNoUpdateSegment = true;
						pIDMUSProdBandEdit8a->InsertPChannel( m_pBandMgr->m_dwPChannelForEditBandButton, FALSE );
						m_pBandMgr->m_fNoUpdateSegment = true;
						pIDMUSProdBandEdit8a->Release();

						m_nLastEdit = IDS_INSERT_INSTRUMENT;
						m_pBandMgr->OnDataChanged();
						// Sequence strip will need to repaint FBar to catch latest DLS region wave names
						m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
					}
				}
				else
				{
					// Otherwise, create a new band node
					bool fNewBand = false;
					if (!pBand)
					{
						pBand = new CBandItem(this);
						fNewBand = true;
					}
					if(pBand != NULL)
					{
						HINSTANCE hInstance = AfxGetResourceHandle();
						AfxSetResourceHandle( _Module.GetModuleInstance() );
						pBand->m_strText.LoadString( IDS_EMPTY_TEXT );
						AfxSetResourceHandle( hInstance );

						pBand->SetTimePhysical( lTmpTime, STP_LOGICAL_SET_DEFAULT );
						// Don't select the band
						//pBand->SetSelectFlag( TRUE );
						m_pBandMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

						IDMUSProdDocType* pIDocType;
						ASSERT( pBand->m_pIBandNode == NULL );
						hr = m_pBandMgr->m_pDMProdFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
						if( SUCCEEDED ( hr ) )
						{
							hr = pIDocType->AllocNode( GUID_BandNode, &pBand->m_pIBandNode );
							pIDocType->Release();
							pIDocType = NULL;
						}
							
						if( SUCCEEDED ( hr ) )
						{
							BSTR bstrBandName = m_pBandMgr->CreateUniqueName();
							pBand->m_strText = bstrBandName;
							pBand->m_pIBandNode->SetNodeName( bstrBandName );
							hr = pBand->m_pIBandNode->SetDocRootNode( m_pStripNode );
							m_pBandMgr->m_pDMProdFramework->AddToNotifyList( pBand->m_pIBandNode, m_pStripNode );
							SendAudiopathToNode( pBand->m_pIBandNode, m_pBandMgr->m_pSegmentState );
						}

						// Open property page
						m_fShowBandProps = TRUE;
						//ShowPropertySheet( m_pBandMgr->m_pTimeline );
						// Change to the Band property page
						//m_pBandMgr->OnShowProperties();
						
						if( fNewBand )
						{
							m_pBandMgr->AddBand( pBand );
						}

						if( pBand->m_pIBandNode )
						{
							BSTR bstrName;
							pBand->m_pIBandNode->GetNodeName( &bstrName );
							pBand->m_strText = bstrName;
							SysFreeString(bstrName);

							// Insert the instrument
							IDMUSProdBandEdit8a *pIDMUSProdBandEdit8a;
							if( SUCCEEDED( pBand->m_pIBandNode->QueryInterface( IID_IDMUSProdBandEdit8a, (void **)&pIDMUSProdBandEdit8a ) ) )
							{
								m_pBandMgr->m_fNoUpdateSegment = true;
								pIDMUSProdBandEdit8a->InsertPChannel( m_pBandMgr->m_dwPChannelForEditBandButton, TRUE );
								m_pBandMgr->m_fNoUpdateSegment = false;

								pIDMUSProdBandEdit8a->Release();
							}
						}

						m_nLastEdit = IDS_INSERT;
						m_pBandMgr->OnDataChanged();
						// Sequence strip will need to repaint FBar to catch latest DLS region wave names
						m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
					}
				}
			}
			break;
		default:
			break;
		}
		break;

	case WM_LBUTTONUP:
		if( m_fSingleSelect )
		{
			CBandItem* pBand = GetBandFromPoint( lXPos );
			if( pBand
			&&  pBand->m_pIBandNode)
			{
				m_pBandMgr->UnselectAll();
				pBand->SetSelectFlag( TRUE );
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				pBand->m_dwBits |= UD_CURRENTSELECTION;

				// Redraw the strip and refresh the Band property page
				m_pBandMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
				m_pBandMgr->OnShowProperties();
				if( m_pBandMgr->m_pPropPageMgr )
				{
					m_pBandMgr->m_pPropPageMgr->RefreshData();
				}
			}
			m_fSingleSelect = FALSE;
		}
		m_lXPos = lXPos;
		hr = S_OK;
		break;

	case WM_CREATE:
		m_bGutterSelected = FALSE;
		
		m_cfBand = RegisterClipboardFormat( CF_BAND );
		m_cfBandTrack = RegisterClipboardFormat( CF_BANDTRACK );
		if( m_pBandMgr->m_pTimeline )
		{
			IUnknown* punk = NULL;
			VARIANT var;
			HRESULT hr = m_pBandMgr->m_pTimeline->GetTimelineProperty(TP_TIMELINECALLBACK, &var);
			punk = V_UNKNOWN(&var);
			hr = punk->QueryInterface(IID_IDMUSProdNode, (void**)&m_pStripNode);
			RELEASE( punk );

			// Update all bands to use this DocRootNode
			m_pBandMgr->SetDocRootForAllBands();

			// Get Left and right selection boundaries
			m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect );
			m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect );

			m_pBandMgr->SyncWithDirectMusic();
		}
		break;

	default:
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::FBDraw

HRESULT CBandStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::FBOnWMMessage

HRESULT CBandStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fShowBandProps = FALSE;
		OnShowProperties();
		break;
	case WM_RBUTTONUP:
	{
		m_fShowBandProps = FALSE;
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

		if( m_pBandMgr->m_pTimeline )
		{
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
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( RightClickMenuCanSelectAll() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
			::EnableMenuItem( hMenuPopup, IDM_CYCLE_BANDS, MF_GRAYED );

			m_pBandMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
			::DestroyMenu( hMenu );
		}
		break;
	}

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Cut

HRESULT CBandStrip::Cut( IDMUSProdTimelineDataObject *pIDataObject )
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
	hr = Copy(pIDataObject);
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Copy

HRESULT CBandStrip::Copy( IDMUSProdTimelineDataObject *pITimelineDataObject )
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

	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfBandTrack == 0 )
	{
		m_cfBandTrack = RegisterClipboardFormat( CF_BANDTRACK );
		if( m_cfBandTrack == 0 )
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected commands in.
	if( FAILED( m_pBandMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pStreamCopy) ) )
	{
		return E_OUTOFMEMORY;
	}

	m_pBandMgr->MarkSelectedBands(UD_COPYSELECT);

	MUSIC_TIME mtTime;
	if( pITimelineDataObject )
	{
		// Get clock from pITimelineDataObject
		if( FAILED( pITimelineDataObject->GetBoundaries( &mtTime, NULL ) ) )
		{
			return E_UNEXPECTED;
		}

		// Adjust to measure/beat boundary
		long lMeasure, lBeat;
		m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
		m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
	}
	else
	{
		// Get clock of first selected Band's measure/beat
		CBandItem* pBandAtDragPoint = m_pBandMgr->FirstSelectedBand();
		ASSERT( pBandAtDragPoint != NULL );
		if( pBandAtDragPoint == NULL )
		{
			return E_UNEXPECTED;
		}
		m_pBandMgr->MeasureBeatTickToClocks( pBandAtDragPoint->m_dwMeasure, pBandAtDragPoint->m_bBeat, 0, &mtTime );  
	}

	// Save the Bands into the stream.
	hr = m_pBandMgr->SaveSelectedBands( pStreamCopy, mtTime, SSB_COPY );
	if( FAILED( hr ))
	{
		RELEASE( pStreamCopy );
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed IDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfBandTrack, pStreamCopy );
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
		hr = m_pBandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pBandMgr->GetBoundariesOfSelectedBands( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfBandTrack, pStreamCopy );

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
		RELEASE( m_pBandMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pBandMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pBandMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::PasteAt

HRESULT CBandStrip::PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtPasteTime, BOOL fDropNotEditPaste, BOOL &fChanged)
{
	if( pITimelineDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure everything in other strip is deselected first.
	if( fDropNotEditPaste == FALSE )
	{
		UnselectGutterRange();
	}

	// Keep paste on a measure/beat boundary
	long lMeasure;
	long lBeat;
	long lTick;

	m_pBandMgr->ClocksToMeasureBeatTick( mtPasteTime, &lMeasure, &lBeat, &lTick );
	if( lTick < 0 )
	{
		// Use previous beat
		lBeat--;
		if( lBeat < 0 )
		{
			MUSIC_TIME mtTime1;
			MUSIC_TIME mtTime2;

			lMeasure--;
			if( lMeasure < 0 )
			{
				m_pBandMgr->MeasureBeatTickToClocks( 0, 0, 0, &mtTime1 );
				m_pBandMgr->MeasureBeatTickToClocks( 0, 1, 0, &mtTime2 );
			}
			else
			{
				m_pBandMgr->MeasureBeatTickToClocks( lMeasure, 0, 0, &mtTime1 );
				m_pBandMgr->MeasureBeatTickToClocks( lMeasure, 1, 0, &mtTime2 );
			}
			long lClocksPerBeat = mtTime2 - mtTime1;
			m_pBandMgr->ClocksToMeasureBeatTick( mtPasteTime - lClocksPerBeat, &lMeasure, &lBeat, &lTick );
		}
	}
	m_pBandMgr->MeasureBeatTickToClocks( lMeasure, lBeat, 0, &mtPasteTime );

	// Don't bother to do anything if user simply moved cursor within same beat in same strip
	if( fDropNotEditPaste )
	{
		if( m_nStripIsDragDropSource )
		{
			long lSourceMeasure;
			long lSourceBeat;
			if( SUCCEEDED ( m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits,
																			0,
																			m_lStartDragPosition,
																			&lSourceMeasure,
																			&lSourceBeat ) ) )
			{
				if( lSourceMeasure == lMeasure
				&&  lSourceBeat == lBeat )
				{
					return S_FALSE;
				}
			}
		}
	}

	IPersistStream *pIPS = NULL;
	IDMUSProdComponent* pIComponent = NULL;
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfBandTrack ) == S_OK )
	{
		IStream* pIStream;
		
		if( m_nStripIsDragDropSource )
		{
			m_pBandMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pBandMgr->UnselectAll();
		}

		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfBandTrack, &pIStream)))
		{
			// Check for RIFF format
			if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				goto Leave;
			}

			MMCKINFO ck;
			ck.fccType = DMUS_FOURCC_BANDS_LIST;
			if ( pIRiffStream->Descend( &ck, NULL, MMIO_FINDLIST ) == 0 )
			{
				if (SUCCEEDED(m_pBandMgr->BuildDirectMusicBandList(pIStream, pIRiffStream, ck, fChanged, true, mtPasteTime) ) )
				{
					m_pBandMgr->UnMarkBands(UD_COPYSELECT);
					hr = S_OK;
				}
				pIRiffStream->Ascend( &ck, 0 );
			}
		}
	}
	else if( pITimelineDataObject->IsClipFormatAvailable( m_cfBand ) == S_OK )
	{
		IStream* pIStream;
		
		if( m_nStripIsDragDropSource )
		{
			m_pBandMgr->UnselectAllKeepBits();
		}
		else
		{
			m_pBandMgr->UnselectAll();
		}

		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfBand, &pIStream)))
		{
			CBandItem *pItem = new CBandItem(m_pBandMgr->m_pBandStrip);
			if(pItem == NULL)
			{
				return E_OUTOFMEMORY;
			}
			pItem->m_strText.Empty();

			m_pBandMgr->ClocksToMeasureBeatTick( mtPasteTime, &lMeasure, &lBeat, &lTick );
			m_pBandMgr->MeasureBeatTickToClocks( lMeasure, lBeat, -1, &mtPasteTime );  
			pItem->SetTimePhysical( mtPasteTime, STP_LOGICAL_SET_DEFAULT );
			
			// load stream into current band
			hr = m_pBandMgr->m_pDMProdFramework->FindComponent(CLSID_BandComponent, &pIComponent);
			if(SUCCEEDED(hr))
			{
				IDMUSProdRIFFExt* pRiff = NULL;
				hr = pIComponent->QueryInterface(IID_IDMUSProdRIFFExt, (void**)&pRiff);
				if( SUCCEEDED( hr ) )
				{
					IDMUSProdNode* pNode = NULL;
					hr = pRiff->LoadRIFFChunk(pIStream, &pNode);
					RELEASE( pRiff );
					if (SUCCEEDED(hr))
					{
						pItem->m_pIBandNode = pNode;
						BSTR bstrName;
						pItem->m_pIBandNode->GetNodeName( &bstrName );
						pItem->m_strText = bstrName;
						if (m_pBandMgr->m_pBandStrip->m_pStripNode)
						{
							m_pBandMgr->m_pDMProdFramework->AddToNotifyList(
								pItem->m_pIBandNode, m_pBandMgr->m_pBandStrip->m_pStripNode);
						}
						m_pBandMgr->InsertByAscendingTime( pItem, TRUE );
						fChanged = TRUE;
						pItem->SetSelectFlag( TRUE );
						SysFreeString(bstrName);
						hr = S_OK;
					}
				}
				RELEASE( pIComponent );
			}
		}
	}

Leave:
	RELEASE( pIPS );
	RELEASE( pIRiffStream );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Paste

HRESULT CBandStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pBandMgr->m_pTimeline != NULL );
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfBandTrack == 0 )
	{
		m_cfBandTrack = RegisterClipboardFormat( CF_BANDTRACK );
		if( m_cfBandTrack == 0 )
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
		hr = m_pBandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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

	// Determine paste measure/beat
	MUSIC_TIME mtTime;
	long lMeasure, lBeat;
	if( m_bContextMenuPaste )
	{
		m_pBandMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}
	}
	m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pBandMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
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

			fChanged = m_pBandMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}

	// Now, do the paste operation
	hr = PasteAt(pITimelineDataObject, mtTime, false, fChanged);
	RELEASE( pITimelineDataObject );

	// If successful and something changed, redraw our strip
	if( SUCCEEDED(hr) && fChanged )
	{
		// Set the last edit type
		m_nLastEdit = IDS_PASTE;

		// Update our hosting editor.
		m_pBandMgr->OnDataChanged();

		// Redraw our strip
		m_pBandMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		// Update the property page
		if( m_pBandMgr->m_pPropPageMgr != NULL )
		{
			m_pBandMgr->m_pPropPageMgr->RefreshData();
		}

		m_pBandMgr->SyncWithDirectMusic();

		// Sequence strip will need to repaint FBar to catch latest DLS region wave names
		m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Insert

HRESULT CBandStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure everything on the timeline is deselected first.
	UnselectGutterRange();
	m_pBandMgr->UnselectAll();

	// BUGBUG: Need to fix this.  Should use m_lXPos if it's valid, otherwise use the time cursor.
	HRESULT hr = S_OK;
	long	lMeasure, lBeat;
	CBandItem* pBand = NULL;
	bool fNewBand = false;
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	hr = m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, m_lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED( hr ));

	pBand = GetBandFromPoint( m_lXPos );
	if (!pBand || pBand->m_pIBandNode)
	{
		pBand = new CBandItem(this);
		fNewBand = true;
	}
	if(pBand == NULL)
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		HINSTANCE hInstance = AfxGetResourceHandle();
		AfxSetResourceHandle( _Module.GetModuleInstance() );
		pBand->m_strText.LoadString( IDS_EMPTY_TEXT );
		AfxSetResourceHandle( hInstance );

		MUSIC_TIME mtTime;
		m_pBandMgr->MeasureBeatTickToClocks( lMeasure, lBeat, -1, &mtTime );
		pBand->SetTimePhysical( mtTime, STP_LOGICAL_SET_DEFAULT );
		pBand->SetSelectFlag( TRUE );
		m_pBandMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

		IDMUSProdDocType* pIDocType;
		ASSERT( pBand->m_pIBandNode == NULL );
		hr = m_pBandMgr->m_pDMProdFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
		if( SUCCEEDED ( hr ) )
		{
			hr = pIDocType->AllocNode( GUID_BandNode, &pBand->m_pIBandNode );
			pIDocType->Release();
			pIDocType = NULL;
		}
			
		if( SUCCEEDED ( hr ) )
		{
			BSTR bstrBandName = m_pBandMgr->CreateUniqueName();
			pBand->m_strText = bstrBandName;
			pBand->m_pIBandNode->SetNodeName( bstrBandName );
			hr = pBand->m_pIBandNode->SetDocRootNode( m_pStripNode );
			m_pBandMgr->m_pDMProdFramework->AddToNotifyList( pBand->m_pIBandNode, m_pStripNode );
			SendAudiopathToNode( pBand->m_pIBandNode, m_pBandMgr->m_pSegmentState );
		}

		// Open property page
		m_fShowBandProps = TRUE;
		ShowPropertySheet( m_pBandMgr->m_pTimeline );
		// Change to the Band property page
		m_pBandMgr->OnShowProperties();
		
		if( fNewBand )
		{
			m_pBandMgr->AddBand( pBand );
		}

		if( pBand->m_pIBandNode )
		{
			BSTR bstrName;
			pBand->m_pIBandNode->GetNodeName( &bstrName );
			pBand->m_strText = bstrName;
			SysFreeString(bstrName);
		}
		
		m_nLastEdit = IDS_INSERT;
		m_pBandMgr->OnDataChanged();
		// Sequence strip will need to repaint FBar to catch latest DLS region wave names
		m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::Delete

HRESULT CBandStrip::Delete( void )
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

	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pBandMgr->m_pTimeline != NULL );
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pBandMgr->DeleteSelectedBands();
	m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	if( m_pBandMgr->m_pPropPageMgr )
	{
		m_pBandMgr->m_pPropPageMgr->RefreshData();
	}

	m_pBandMgr->SyncWithDirectMusic();
	m_nLastEdit = IDS_DELETE;
	m_pBandMgr->OnDataChanged();

	// Sequence strip will need to repaint FBar to catch latest DLS region wave names
	m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::SelectAll

HRESULT CBandStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pBandMgr->m_pTimeline != NULL );
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pBandMgr->SelectAll();

	m_pBandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::CanCut

HRESULT CBandStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pBandMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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
// CBandStrip::CanCopy

HRESULT CBandStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can copy even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pBandMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pBandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	return m_pBandMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::CanPaste

HRESULT CBandStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;

	ASSERT( m_pBandMgr != NULL );
	if( m_pBandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfBandTrack == 0 )
	{
		m_cfBandTrack = RegisterClipboardFormat( CF_BANDTRACK );
		if( m_cfBandTrack == 0 )
		{
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfBandTrack );
		if( hr != S_OK )
		{
			hr = pITimelineDataObject->IsClipFormatAvailable( m_cfBand );
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
			if( SUCCEEDED( m_pBandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfBandTrack);
					if( hr != S_OK )
					{
						hr = pITimelineDataObject->IsClipFormatAvailable(m_cfBand);
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
// CBandStrip::CanInsert

HRESULT CBandStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pBandMgr != NULL );
	ASSERT( m_pBandMgr->m_pTimeline != NULL );

	// Get Timeline length
	VARIANT var;
	m_pBandMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Get clock at XPos
	long lClock;
	m_pBandMgr->m_pTimeline->PositionToClocks( m_lXPos, &lClock );

	// Make sure XPos is within strip
	if( lClock > 0 
	&&  lClock < lTimelineLength )
	{
		// User clicked within boundaries of strip
		long lMeasure;
		long lBeat;

		if( SUCCEEDED ( m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits,
																		0,
																		m_lXPos,
																		&lMeasure,
																		&lBeat ) ) )
		{
			CPropBand* pBand = GetBandFromPoint( m_lXPos );
			if( pBand == NULL
			||  pBand->m_pIBandNode == NULL )
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::CanDelete

HRESULT CBandStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//CPropBand* pBand;
	//pBand = GetBandFromPoint( m_lXPos );
	//return (pBand != NULL && pBand->m_pIBandNode != NULL) ? S_OK : S_FALSE;
	return m_pBandMgr->IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::RightClickMenuCanSelectAll

HRESULT CBandStrip::RightClickMenuCanSelectAll( void )
{
	CBandItem* pBandItem;

	POSITION pos = m_pBandMgr->m_lstBands.GetHeadPosition();
	while( pos )
	{
		pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );

		if( pBandItem->m_pIBandNode )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::CanSelectAll

HRESULT CBandStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Always enable when gutter is selected
	if( m_bGutterSelected )
	{
		return S_OK;
	}

	CBandItem* pBandItem;

	POSITION pos = m_pBandMgr->m_lstBands.GetHeadPosition();
	while( pos )
	{
		pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );

		if( pBandItem->m_pIBandNode )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::QueryContinueDrag

HRESULT CBandStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// CBandStrip::GiveFeedback

HRESULT CBandStrip::GiveFeedback( DWORD dwEffect )
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
// CBandStrip::CreateDragImage

CImageList* CBandStrip::CreateDragImage()
{
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::CreateDataObject

HRESULT	CBandStrip::CreateDataObject(IDataObject** ppIDataObject, long lPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Convert lPosition from pixels to clocks
	long lMeasure, lBeat;
	if( FAILED( m_pBandMgr->m_pTimeline->PositionToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, lPosition, &lMeasure, &lBeat ) ) )
	{
		return E_UNEXPECTED;
	}
	long lClocks;
	if( FAILED( m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lClocks ) ) )
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

	// Save Selected Bands into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pBandMgr->m_pDMProdFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// mark the bands as being dragged: this used later for deleting bands in drag move
		m_pBandMgr->MarkSelectedBands(UD_DRAGSELECT);
		if( SUCCEEDED ( m_pBandMgr->SaveSelectedBands( pIStream, lClocks, SSB_DRAG ) ) )
		{
			// Place CF_BANDTRACK into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfBandTrack, pIStream ) ) )
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
// CBandStrip::DragEnter

HRESULT CBandStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
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
// CBandStrip::DragOver

HRESULT CBandStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		BOOL fCF_BAND = FALSE;

		// Does m_pITargetDataObject contain format CF_BAND?
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject )
		{
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( m_pITargetDataObject, m_cfBand ) ) )
			{
				fCF_BAND = TRUE;
			}
			RELEASE( pDataObject );
		}

		// Can only copy CF_BAND data!
		if( fCF_BAND )
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
// CBandStrip::DragLeave

HRESULT CBandStrip::DragLeave( void )
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
// CBandStrip::Drop

HRESULT CBandStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pBandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED( m_pBandMgr->m_pTimeline->PositionToClocks( pt.x, &mtTime ) ) )
				{
					BOOL fChanged = FALSE;
					hr = PasteAt( pITimelineDataObject, mtTime, true, fChanged );
					if( hr == S_OK )
					{
						*pdwEffect = m_dwOverDragEffect;

						// If we pasted anything
						if( fChanged )
						{
							if( m_nStripIsDragDropSource )
							{
								// Drag/drop Target and Source are the same Band strip
								m_nStripIsDragDropSource = 2;
							}
							else
							{
								// Set the last edit type
								m_nLastEdit = IDS_PASTE;

								// Update our hosting editor.
								m_pBandMgr->OnDataChanged();

								// Redraw our strip
								m_pBandMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

								// Update the property page
								if( m_pBandMgr->m_pPropPageMgr != NULL )
								{
									m_pBandMgr->m_pPropPageMgr->RefreshData();
								}

								m_pBandMgr->SyncWithDirectMusic();

								// Sequence strip will need to repaint FBar to catch latest DLS region wave names
								m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
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
// CBandStrip::GetTimelineCWnd

CWnd* CBandStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pBandMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pBandMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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
// CBandStrip::CanPasteFromData

HRESULT CBandStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfBandTrack ) ) ||
		SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfBand ) ))
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::GetData

HRESULT CBandStrip::GetData( void **ppData )
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
		pPPGTrackParams->dwGroupBits = m_pBandMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pBandMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pBandMgr->m_dwProducerOnlyFlags;
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
// CBandStrip::SetData

HRESULT CBandStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

		// Update our group bits setting, if necessary
		if( pPPGTrackParams->dwGroupBits != m_pBandMgr->m_dwGroupBits )
		{
			DWORD dwOrigGroupBits = m_pBandMgr->m_dwGroupBits; 
			m_pBandMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;

			// Time signature may have changed
			m_pBandMgr->OnUpdate( GUID_TimeSignature, m_pBandMgr->m_dwGroupBits, NULL );

			m_nLastEdit = IDS_UNDO_TRACKGROUP;
			m_pBandMgr->m_pTimeline->OnDataChanged( (IBandMgr*)m_pBandMgr );
			m_pBandMgr->m_dwOldGroupBits = pPPGTrackParams->dwGroupBits;

			// Sequence strip will need to repaint FBar to catch latest DLS region wave names
			m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, 
													 (dwOrigGroupBits | m_pBandMgr->m_dwGroupBits),
													  NULL );
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pBandMgr->m_dwTrackExtrasFlags )
		{
			m_pBandMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pBandMgr->m_pTimeline->OnDataChanged( (IBandMgr*)m_pBandMgr );
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pBandMgr->m_dwProducerOnlyFlags )
		{
			m_pBandMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pBandMgr->m_pTimeline->OnDataChanged( (IBandMgr*)m_pBandMgr );
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
// CBandStrip::OnShowProperties

HRESULT CBandStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( m_pBandMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pBandMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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
	m_pBandMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::OnRemoveFromPageManager

HRESULT CBandStrip::OnRemoveFromPageManager( void )
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
// CBandStrip::ShowPropertySheet

HRESULT CBandStrip::ShowPropertySheet(IDMUSProdTimeline* pTimeline)
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
// CBandStrip::DoDragDrop

BOOL CBandStrip::DoDragDrop( void )
{
	// Drag drop will capture mouse, so release it from timeline
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pBandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

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
		// Create an image to use when dragging style references
		m_pDragImage = CreateDragImage();
		if( m_pDragImage )
		{
			m_pDragImage->BeginDrag( 0, CPoint(8,12) );
		}

		// We can always copy style references.
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
		if( CanCut() == S_OK )
		{
			// If we can Cut(), allow the user to move the style references as well.
			dwOKDragEffects |= DROPEFFECT_MOVE;
		}

		// Do the Drag/Drop.
		hr = ::DoDragDrop( m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect );

		if( m_pDragImage )
		{
			m_pDragImage->EndDrag();
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					m_pBandMgr->DeleteMarked( UD_DRAGSELECT );
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

	m_pBandMgr->UnMarkBands(UD_DRAGSELECT);
	m_pBandMgr->SyncWithDirectMusic();

	if( dwEffect != DROPEFFECT_NONE )
	{
		if( m_nStripIsDragDropSource == 2 )
		{
			// Drag/drop target and source are the same Band strip
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_UNDO_MOVE;
				m_pBandMgr->OnDataChanged();
				// Sequence strip will need to repaint FBar to catch latest DLS region wave names
				m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_nLastEdit = IDS_PASTE;
				m_pBandMgr->OnDataChanged();
				// Sequence strip will need to repaint FBar to catch latest DLS region wave names
				m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
			}
		}
		else
		{
			if( dwEffect == DROPEFFECT_MOVE )
			{
				m_nLastEdit = IDS_DELETE;
				m_pBandMgr->OnDataChanged();
				// Sequence strip will need to repaint FBar to catch latest DLS region wave names
				m_pBandMgr->m_pTimeline->NotifyStripMgrs( GUID_Segment_BandTrackChange, m_pBandMgr->m_dwGroupBits, NULL );
			}
			else
			{
				m_pBandMgr->UnselectAll();
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
// CBandStrip::OnLButtonDown

HRESULT CBandStrip::OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	ASSERT( m_pBandMgr != NULL );
	ASSERT( m_pBandMgr->m_pTimeline != NULL );

	m_fSingleSelect = FALSE;

	// If we're already dragging, just return
	if( m_pDragImage )
	{
		return S_OK;
	}

	// Get Timeline length
	VARIANT var;
	m_pBandMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineLength = V_I4( &var );

	// Exit if user clicked past end of Strip
	long lClock;
	m_pBandMgr->m_pTimeline->PositionToClocks( lXPos, &lClock );
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
	m_pBandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	// See if there is a band under the cursor.
	CBandItem* pBand = GetBandFromPoint( lXPos );
	if( pBand
	&&  pBand->m_pIBandNode)
	{
		// There is a "real" Band on this beat
		if( wParam & MK_CONTROL )
		{
			if( pBand->m_fSelected )
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
					pBand->SetSelectFlag( !pBand->m_fSelected );
					m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
					pBand->m_dwBits |= UD_CURRENTSELECTION;
				}
			}
			else
			{
				pBand->SetSelectFlag( !pBand->m_fSelected );
				if( pBand->m_fSelected )
				{
					// Set fields to initiate drag/drop on next mousemove
					m_dwStartDragButton = (unsigned long)wParam;
					m_lStartDragPosition = lXPos;
				}
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				pBand->m_dwBits |= UD_CURRENTSELECTION;
			}
		}
		else if( wParam & MK_SHIFT )
		{
			pBand->SetSelectFlag( TRUE );
			SelectRange( pBand );
		}
		else
		{
			if( pBand->m_fSelected == FALSE )
			{
				m_pBandMgr->UnselectAll();
				pBand->SetSelectFlag( TRUE );
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				pBand->m_dwBits |= UD_CURRENTSELECTION;
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
		// There is not a "real" Band on this measure
		hr = S_OK;
		if( pBand == NULL )
		{
			// Create a "fake" Band
			hr = m_pBandMgr->CreateBand( lXPos, this, pBand );
		}

		if( SUCCEEDED ( hr ) )
		{
			if( wParam & MK_CONTROL )
			{
				pBand->SetSelectFlag( !pBand->m_fSelected );
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				pBand->m_dwBits |= UD_CURRENTSELECTION;
			}
			else if( wParam & MK_SHIFT )
			{
				pBand->SetSelectFlag( TRUE );
				SelectRange( pBand );
			}
			else
			{
				// Click on empty space deselects all
				m_pBandMgr->UnselectAll();

				pBand->SetSelectFlag( TRUE );
				m_pBandMgr->UnMarkBands( UD_CURRENTSELECTION );
				pBand->m_dwBits |= UD_CURRENTSELECTION;
			}
		}
	}

	// Redraw the strip and refresh the Band property page
	m_pBandMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
	m_pBandMgr->OnShowProperties();
	if( m_pBandMgr->m_pPropPageMgr )
	{
		m_pBandMgr->m_pPropPageMgr->RefreshData();
	}

	return hr;
}

// CBandStrip::SelectRange()

HRESULT CBandStrip::SelectRange(CBandItem* pBand)
{
	HRESULT hr = S_OK;
	// select inclusive, find band to start from
	CBandItem* pCurrent = m_pBandMgr->CurrentlySelectedBand();

	// find measure and beat for band to start from and band to end at
	BYTE bEarlyBeat = (pCurrent) ? pCurrent->m_bBeat : 0;
	DWORD dwEarlyMeasure = (pCurrent) ? pCurrent->m_dwMeasure : 0;
	BYTE bLateBeat = pBand->m_bBeat;
	DWORD dwLateMeasure = pBand->m_dwMeasure;

	// if the order is backwards, swap
	if ( dwLateMeasure < dwEarlyMeasure ||
		 (dwLateMeasure == dwEarlyMeasure && bLateBeat < bEarlyBeat) )
	{
		BYTE bTemp = bEarlyBeat;
		bEarlyBeat = bLateBeat;
		bLateBeat = bTemp;
		DWORD dwTemp = dwEarlyMeasure;
		dwEarlyMeasure = dwLateMeasure;
		dwLateMeasure = dwTemp;
	}

	// add empty empty bands to fill in gaps
	BYTE bCurrentBeat = bEarlyBeat;
	DWORD dwCurrentMeasure = dwEarlyMeasure;
	CBandItem* pBandItem = NULL;
	while( dwCurrentMeasure <= dwLateMeasure &&
		   ( bCurrentBeat <= bLateBeat || dwCurrentMeasure < dwLateMeasure ) )
	{
		long lCheckClocks, lCheckMeasure, lCheckBeat;
		m_pBandMgr->m_pTimeline->MeasureBeatToClocks( m_pBandMgr->m_dwGroupBits, 0, dwCurrentMeasure, bCurrentBeat, &lCheckClocks );
		m_pBandMgr->m_pTimeline->ClocksToMeasureBeat( m_pBandMgr->m_dwGroupBits, 0, lCheckClocks, &lCheckMeasure, &lCheckBeat );
		if( lCheckMeasure == (long) dwCurrentMeasure
		||  lCheckBeat == (long) bCurrentBeat )
		{
			// We don't want to create bands on invalidate beats
			// when range crosses time sig changes
			if ( !m_pBandMgr->FindBand(dwCurrentMeasure, (long) bCurrentBeat) )
			{
				pBandItem = NULL;
				hr = m_pBandMgr->CreateBand(dwCurrentMeasure, bCurrentBeat, this, pBandItem);
			}
		}

		bCurrentBeat++;
		DMUS_TIMESIGNATURE TS;
		long lClocks = 0;
		m_pBandMgr->m_pTimeline->MeasureBeatToClocks( 
				m_pBandMgr->m_dwGroupBits, 
				0, 
				dwCurrentMeasure, 
				(long) bCurrentBeat, 
				&lClocks );
		hr = m_pBandMgr->m_pTimeline->GetParam(
			GUID_TimeSignature, 
			m_pBandMgr->m_dwGroupBits, 
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
		if (bCurrentBeat >= bBeats) // BUGBUG need timesig here!
		{
			bCurrentBeat = 0;
			dwCurrentMeasure++;
		}
	}
	m_pBandMgr->UnselectAllKeepBits();

	// select all bands in the range (can't simply select bands in the
	// above loop, because there may be multiple bands at the same
	// measure/beat)
	POSITION pos = m_pBandMgr->m_lstBands.GetHeadPosition();
	pBandItem = NULL;
	while( pos )
	{
		pBandItem = m_pBandMgr->m_lstBands.GetNext( pos );
		ASSERT( pBandItem );
		if( pBandItem->m_dwMeasure >= dwEarlyMeasure &&
			pBandItem->m_dwMeasure <= dwLateMeasure &&
			( pBandItem->m_dwMeasure != dwEarlyMeasure || 
			  pBandItem->m_bBeat >= bEarlyBeat) &&
			( pBandItem->m_dwMeasure != dwLateMeasure || 
			  pBandItem->m_bBeat <= bLateBeat) )
		{
			pBandItem->m_fSelected = TRUE;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBandStrip::UnselectGutterRange

void CBandStrip::UnselectGutterRange( void )
{
	ASSERT( m_pBandMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pBandMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pBandMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}
	

/////////////////////////////////////////////////////////////////////////////
// CBandStrip::AddBandNode

HRESULT CBandStrip::AddBandNode( IDMUSProdNode* pIBandNode, MUSIC_TIME mtTime )
{
	// Only called in response to the GUID_Segment_NewStyleSelected notification.
	// Note that due to bug 28541, we now append a "1" to the end of the band name.

	CBandItem* pBand = new CBandItem( this );
	if( pBand == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pBand->SetTimePhysical( mtTime, STP_LOGICAL_SET_DEFAULT );

	HRESULT hr = E_FAIL;

	IDMUSProdDocType* pIDocType;
	IPersistStream* pIBandNodePS;
	IPersistStream* pIPS;
	IStream* pIMemStream;
	BSTR bstrName;

	if( SUCCEEDED ( m_pBandMgr->m_pDMProdFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType ) ) )
	{
		if( SUCCEEDED ( pIDocType->AllocNode( GUID_BandNode, &pBand->m_pIBandNode ) ) )
		{
			if( SUCCEEDED ( pIBandNode->QueryInterface( IID_IPersistStream, (void**)&pIBandNodePS ) ) )
			{
				if( SUCCEEDED ( m_pBandMgr->m_pDMProdFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream ) ) )
				{
					if( SUCCEEDED ( pIBandNodePS->Save( pIMemStream, FALSE ) ) )
					{
						StreamSeek( pIMemStream, 0, STREAM_SEEK_SET ); //seek to beginning

						if( SUCCEEDED ( pBand->m_pIBandNode->QueryInterface( IID_IPersistStream, (void**)&pIPS ) ) )
						{
							if( SUCCEEDED ( pIPS->Load( pIMemStream ) ) )
							{
								// Set  name of Band
								pBand->m_pIBandNode->GetNodeName( &bstrName );
								pBand->m_strText = bstrName;
								pBand->m_strText += _T("1");
								SysFreeString( bstrName );

								// Set Band's DocRoot node
								hr = pBand->m_pIBandNode->SetDocRootNode( m_pStripNode );

								// Set notifications
								m_pBandMgr->m_pDMProdFramework->AddToNotifyList( pBand->m_pIBandNode, m_pStripNode );

								// Hook it up with our audiopath
								SendAudiopathToNode( pBand->m_pIBandNode, m_pBandMgr->m_pSegmentState );

								// Everything is ok
								hr = S_OK;
							}

							RELEASE( pIPS );
						}
					}
				
					RELEASE( pIMemStream );
				}

				RELEASE( pIBandNodePS );
			}
		}

		RELEASE( pIDocType );
	}
		
	if( SUCCEEDED ( hr ) )
	{
		m_pBandMgr->InsertByAscendingTime( pBand, TRUE );
		m_pBandMgr->SyncWithDirectMusic();
	}
	else
	{
		delete pBand;
	}

	return hr;
}
