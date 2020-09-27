// CommandStrip.cpp : Implementation of CCommandStrip
#include "stdafx.h"
#include "CommandStripMgr.h"
#include "CommandMgr.h"
#include "FileIO.h"
#include "DllJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include <dmusici.h>
#include "SegmentIO.h"
#include "GrayOutRect.h"

// Light grey
#define GREY_COMMAND_COLOR		RGB( 168, 168, 168 )
#define STRIP_HEIGHT			20

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

void GetGrooveText( BYTE bGrooveLevel, BYTE bGrooveRange, BYTE bRepeatMode, CString &strGroove )
{
	// Erase strGroove
	strGroove.Empty();

	// Determine what text (if any) to display for the groove
	if (bGrooveLevel >= 1 && bGrooveLevel <= 100)
	{
		// A groove range of 1 acts like a groove range of 0, so ignore it as well as 0
		if( bGrooveRange > 1 )
		{
			/* TODO: Is this faster?
			TCHAR tcstrTmp1[4], tcstrTmp2[4];
			_itot(pCommand->bGrooveLevel - pCommand->bGrooveRange / 2, tcstrTmp1, 10);
			_itot(pCommand->bGrooveLevel + pCommand->bGrooveRange / 2, tcstrTmp2, 10);
			TCHAR tcstrTmp3[10];
			_tcscpy( tcstrTmp3, _T("(") );
			strGroove = _tcsncat( _tcsncat( _tcsncat( _tcsncat( tcstrTmp3, tcstrTmp1, 4 ), _T("-"), 2 ), tcstrTmp2, 4 ), _T(")"), 2 );
			*/
			strGroove.Format("(%d-%d)", max( 1, bGrooveLevel - bGrooveRange / 2 ),
										min( 100, bGrooveLevel + bGrooveRange / 2 ) );
		}
		else
		{
			TCHAR tcstrTmp[4];
			_itot(bGrooveLevel, tcstrTmp, 10);

			TCHAR tcstrTmp2[6];
			_tcscpy( tcstrTmp2, _T("(") );
			strGroove = _tcsncat( _tcsncat( tcstrTmp2, tcstrTmp, 4 ), _T(")"), 2 );
		}
	}

	// Determine what text (if any) to display for the repeat mode
	CString strRepeatMode;
	if( (bRepeatMode > 0) && (bRepeatMode <= 5)
	&&	strRepeatMode.LoadString( IDS_RANDOM + bRepeatMode ) )
	{
		if( !strGroove.IsEmpty() )
		{
			strGroove += " ";
		}
		strGroove += strRepeatMode;
	}
}

void GetCommandText( CommandExt* pCommand, CString &strCommand )
{
	// Erase strCommand
	strCommand.Empty();

	if( pCommand == NULL )
	{
		return;
	}

	// Determine what text (if any) to display for the embellishment
	switch( pCommand->bCommand )
	{
	case DMUS_COMMANDT_FILL:
		strCommand.LoadString( IDS_FILL_TEXT );
		break;
	case DMUS_COMMANDT_BREAK:
		strCommand.LoadString( IDS_BREAK_TEXT );
		break;
	case DMUS_COMMANDT_INTRO:
		strCommand.LoadString( IDS_INTRO_TEXT );
		break;
	case DMUS_COMMANDT_END:
		strCommand.LoadString( IDS_END_TEXT );
		break;
	case DMUS_COMMANDT_GROOVE:
		//strCommand += "Groove";
		break;
	default:
		if( (pCommand->bCommand >= MIN_EMB_CUSTOM_ID)
		&&	(pCommand->bCommand <= MAX_EMB_CUSTOM_ID) )
		{
			TCHAR tcstrTmp[4];
			_itot( pCommand->bCommand, tcstrTmp, 10 );
			strCommand = CString(tcstrTmp);
		}
		break;
	}

	// Determine what text (if any) to display for the groove
	CString strGroove;
	GetGrooveText( pCommand->bGrooveLevel, pCommand->bGrooveRange, pCommand->bRepeatMode, strGroove );

	if( !strGroove.IsEmpty() )
	{
		if( !strCommand.IsEmpty() )
		{
			strCommand += " ";
		}
		strCommand += strGroove;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCommandStrip

/////////////////////////////////////////////////////////////////////////////
// CCommandStrip IUnknown
STDMETHODIMP CCommandStrip::QueryInterface( REFIID riid, LPVOID* ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( ppv );
    *ppv = NULL;
    if( IsEqualIID( riid, IID_IUnknown ))
	{
        *ppv = (IUnknown*) (IDMUSProdStrip*) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdStrip ))
	{
        *ppv = (IDMUSProdStrip*) this;
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
		return E_NOINTERFACE;
	}
    ((IUnknown*) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CCommandStrip::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CCommandStrip::Release(void)
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
// CCommandStrip IDMUSProdStrip

HRESULT	STDMETHODCALLTYPE CCommandStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(sv);

	BYTE			bGreyGroove = 0;
	BYTE			bGreyGrooveRange = 0;
	BYTE			bGreyRepeatMode = 0;
	//CRect			rectInvalid;
	CRect			rectHighlight;
	long			lPositionFirst;
	long			lMeasureFirst;// = m_lLastFirstMeasure;

	ASSERT( m_pCommandMgr != NULL && m_pStripMgr != NULL );
	if( m_pCommandMgr == NULL || m_pStripMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pCommandMgr->m_pTimeline )
	{
		m_pCommandMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pCommandMgr->m_dwGroupBits, 0, lXOffset );
		if( hDC )
		{
			// draw the commands
			CRect		rectClip;
			CString		strCommand;
			CommandExt*	pCommand = m_pCommandMgr->m_pCommandList;
			long		position;
			long		lMeasure;
			SIZE		sizeText, sizeGreyText;
			bool		fDrawGreyText = true;
			RECT		rectToDrawIn;

			::GetClipBox( hDC, &rectClip );
			rectHighlight = rectClip;

			rectToDrawIn.top = 0;
			rectToDrawIn.bottom = STRIP_HEIGHT;

			// Find the measure in which to draw the greyed command.
			m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lPositionFirst );

			// The following few lines are because of a bug in the timeline which sometimes
			// sends a null clipping region.
			// Make sure we should even bother drawing.
			m_pCommandMgr->m_pTimeline->ClocksToPosition(lPositionFirst, &position);
			if( position - lXOffset > rectClip.right )
			{
				return S_OK;
			}

			m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lPositionFirst, &lMeasureFirst, NULL );
			long lClocks;
			m_pCommandMgr->m_pTimeline->MeasureBeatToClocks( m_pCommandMgr->m_dwGroupBits, 0, lMeasureFirst, 0, &lClocks );
			if( lClocks < lPositionFirst )
			{
				lMeasureFirst += 1;
			}
			m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits, 0, lMeasureFirst, 0, &lPositionFirst );

			while( pCommand )
			{
				m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, pCommand->lTime, &lMeasure, NULL );
				if( lMeasure < lMeasureFirst )
				{
					// Only update the groove level and range if it is set
					if( (pCommand->bGrooveLevel >= 1) && (pCommand->bGrooveLevel <= 100) )
					{
						bGreyGroove = pCommand->bGrooveLevel;
						bGreyGrooveRange = pCommand->bGrooveRange;
					}

					// Always update the repeat mode
					bGreyRepeatMode = pCommand->bRepeatMode;
				}

				m_pCommandMgr->m_pTimeline->ClocksToPosition( pCommand->lTime, &position );
				position++;

				rectToDrawIn.left = position - lXOffset;

				if( pCommand->pNext )
				{
					m_pCommandMgr->m_pTimeline->ClocksToPosition( pCommand->pNext->lTime, &rectToDrawIn.right );
					rectToDrawIn.right -= lXOffset - 1;
				}
				else
				{
					rectToDrawIn.right = SHRT_MAX;
				}

				GetCommandText( pCommand, strCommand );

				if( lMeasure <= lMeasureFirst )
				{
					// Get the extent of the groove text
					if( ::GetTextExtentPoint( hDC, strCommand, strCommand.GetLength(), &sizeText ) )
					{
						// Check if we should display a grey groove level
						CString	strGreyText;
						sizeGreyText.cx = 0;
						if( fDrawGreyText
						&&	pCommand->bGrooveLevel == 0 )
						{
							// Add the space after the embellishment text
							strCommand += _T(" ");

							// Recalculate the size of the text
							::GetTextExtentPoint( hDC, strCommand, strCommand.GetLength(), &sizeText );

							// Convert the grey groove level and range to a string
							GetGrooveText( bGreyGroove, bGreyGrooveRange, 0xFF, strGreyText );

							// Get the extent of the groove text
							::GetTextExtentPoint( hDC, strGreyText, strGreyText.GetLength(), &sizeGreyText );
						}

						// If the text extends past the left edge
						if( position - lXOffset + sizeText.cx + sizeGreyText.cx > rectClip.left )
						{
							// Write out the groove text
							::DrawText( hDC, strCommand, -1, &rectToDrawIn, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_TOP );

							// Write out the grey text, if necessary
							if( !strGreyText.IsEmpty() )
							{
								// Set rectInvalid so we invalidate the old grey text
								//rectInvalid = m_rectInvalid;

								// Calculate the rectangle to draw in
								rectToDrawIn.left = position - lXOffset + sizeText.cx;

								// Check if there is any space to draw the grey text
								if( rectToDrawIn.left < rectToDrawIn.right )
								{
									// Set m_rectInvalid so that the next time the strip is drawn, the grey command will be erased.
									//m_rectInvalid = rectToDrawIn;
									//m_rectInvalid.left += lXOffset;
									//m_rectInvalid.right = min( m_rectInvalid.right + lXOffset, m_rectInvalid.left + sizeGreyText.cx );

									// Draw the greyed command.
									COLORREF crOld = ::SetTextColor( hDC, GREY_COMMAND_COLOR);
									::DrawText( hDC, strGreyText, -1, &rectToDrawIn, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_TOP );
									::SetTextColor( hDC, crOld );
								}
							}

							// Flag that we don't need to draw the grey text
							fDrawGreyText = false;
						}
					}
				}
				else // if( lMeasure > lMeasureFirst )
				{
					// Check if we need to draw the grey text in lMeasureFirst
					if( fDrawGreyText )
					{
						// Yes - Draw the greyed command
						long lGreyPosition = lPositionFirst + 1;

						// Convert the groove level and range to a string
						CString	strGreyText;
						GetGrooveText( bGreyGroove, bGreyGrooveRange, bGreyRepeatMode, strGreyText );

						// Write out the grey text, if necessary
						if( !strGreyText.IsEmpty() )
						{
							// Set rectInvalid so that we invalidate the old grey text
							//rectInvalid = m_rectInvalid;

							// Calculate the rectangle to draw in
							RECT rectGreyText = rectToDrawIn;
							rectGreyText.left = lGreyPosition - lXOffset;
							rectGreyText.right = position - lXOffset;

							if( rectGreyText.left < rectGreyText.right )
							{
								// Set m_rectInvalid so that the next time the strip is drawn, the grey command will be erased.
								//::GetTextExtentPoint( hDC, strGreyText, strGreyText.GetLength(), &sizeGreyText );

								// Set m_rectInvalid so that the next time the strip is drawn, the grey command will be erased.
								//m_rectInvalid = rectGreyText;
								//m_rectInvalid.left += lXOffset;
								//m_rectInvalid.right = min( position, m_rectInvalid.left + sizeGreyText.cx );

								// Draw the greyed command.
								COLORREF crOld = ::SetTextColor( hDC, GREY_COMMAND_COLOR);
								::DrawText( hDC, strGreyText, -1, &rectGreyText, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_TOP );
								::SetTextColor( hDC, crOld );
							}
						}

						// Flag that we don't need to draw the grey text
						fDrawGreyText = false;
					}

					// Now, write out the groove text
					::DrawText( hDC, strCommand, -1, &rectToDrawIn, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_TOP );
				}

				pCommand = pCommand->pNext;
				if( position - lXOffset > rectClip.right )
				{
					break;
				}
			} // while( pCommand )

			// Need to draw the grey text after all commands have been drawn
			if( fDrawGreyText )
			{
				// Yes - Draw the greyed command
				long lGreyPosition = lPositionFirst + 1;

				// Convert the groove level and range to a string
				CString	strGreyText;
				GetGrooveText( bGreyGroove, bGreyGrooveRange, bGreyRepeatMode, strGreyText );

				// Write out the grey text, if necessary
				if( !strGreyText.IsEmpty() )
				{
					// Set rectInvalid so that we invalidate the old grey text
					//rectInvalid = m_rectInvalid;

					// Get the size of the grey text, for m_rectInvalid
					//::GetTextExtentPoint( hDC, strGreyText, strGreyText.GetLength(), &sizeGreyText );

					// Set m_rectInvalid so that the next time the strip is drawn, the grey command will be erased.
					//m_rectInvalid = rectToDrawIn;
					//m_rectInvalid.left = lGreyPosition;
					//m_rectInvalid.right = lGreyPosition + sizeGreyText.cx;

					// Draw the greyed command.
					COLORREF crOld = ::SetTextColor( hDC, GREY_COMMAND_COLOR);
					::TextOut( hDC, lGreyPosition - lXOffset, 0, strGreyText, strGreyText.GetLength() );
					::SetTextColor( hDC, crOld );
				}
			}

			// Highlight the selected measures.
			if( m_pCommandMgr->m_bSelected )
			{
				ASSERT( m_pCommandMgr->m_pMeasureArray != NULL );
				if( m_pCommandMgr->m_pMeasureArray == NULL )
				{
					return E_UNEXPECTED;
				}

				long lBeginSelMeasure;
				long lEndSelMeasure;
				long lMeasureBeginPosition = 0;
				long lMeasureEndPosition = 0;
				short n;

				lBeginSelMeasure = -1;
				lEndSelMeasure = -1;
				for( n = 0; n < m_pCommandMgr->m_nMeasures; n++ )
				{
					if( m_pCommandMgr->m_pMeasureArray[n].dwFlags & CMI_SELECTED )
					{
						if( lBeginSelMeasure == -1 )
						{
							lBeginSelMeasure = (long) n;
						}
						lEndSelMeasure = (long) n + 1;
					}
					else if( lBeginSelMeasure != -1 )
					{
						ASSERT( lEndSelMeasure > lBeginSelMeasure );

						// Highlight the range which just ended.
						m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits, 0, lBeginSelMeasure, 0, &lMeasureBeginPosition );
						m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits, 0, lEndSelMeasure, 0, &lMeasureEndPosition );
						rectHighlight.left = lMeasureBeginPosition - lXOffset;
						rectHighlight.right = lMeasureEndPosition - lXOffset;
						GrayOutRect( hDC, &rectHighlight );
						lBeginSelMeasure = -1;
						lEndSelMeasure = -1;
					}
				}

				if( lBeginSelMeasure != -1 )
				{
					// The last measure was highlighted.
					m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits, 0, lBeginSelMeasure, 0, &lMeasureBeginPosition );
					m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits, 0, lEndSelMeasure, 0, &lMeasureEndPosition );
					rectHighlight.left = lMeasureBeginPosition - lXOffset;
					rectHighlight.right = lMeasureEndPosition - lXOffset;
					GrayOutRect( hDC, &rectHighlight );
				}
			}
		}
		/*
		if( m_lLastFirstMeasure > -1 && ( m_lLastFirstMeasure != lMeasureFirst ))
		{
			m_lLastFirstMeasure = -1;
			
			// If the position of the grey text changed, and the rect to invalidate
			// was not redrawn
			if( (rectInvalid != m_rectInvalid)
			&&	( !rectClip.PtInRect( CPoint(rectInvalid.left, rectInvalid.top) ||
				  !rectClip.PtInRect( CPoint(rectInvalid.right, rectInvalid.bottom) ) )
			{
				rectInvalid.left -= lXOffset;
				rectInvalid.right -= lXOffset;
				m_pCommandMgr->m_pTimeline->StripInvalidateRect( this, rectInvalid, TRUE );
			}
		}
		*/
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CCommandStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		pvar->vt = VT_BOOL;
		V_BOOL( pvar ) = FALSE;
		break;
	case SP_GUTTERSELECTABLE:
		pvar->vt = VT_BOOL;
		V_BOOL( pvar ) = TRUE;
		break;
	case SP_MINMAXABLE:
		pvar->vt = VT_BOOL;
		V_BOOL( pvar ) = FALSE;
		break;
	case SP_DEFAULTHEIGHT:
		pvar->vt = VT_INT;
		V_INT( pvar ) = STRIP_HEIGHT;
		break;
	case SP_NAME:
		{
			BSTR bstr;
			CString strName;
			strName.LoadString( IDS_FUNCTIONBAR_TEXT );

			CString str = GetName(m_pCommandMgr->m_dwGroupBits, strName);
			pvar->vt = VT_BSTR;
			try
			{
				bstr = str.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR( pvar ) = bstr;
		}
		break;
	case SP_BEGINSELECT:
		pvar->vt = VT_I4;
		V_I4( pvar ) = m_lBeginSelect;
		break;
	case SP_ENDSELECT:
		pvar->vt = VT_I4;
		V_I4( pvar ) = m_lEndSelect;
		break;
	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pCommandMgr )
		{
			m_pCommandMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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

HRESULT STDMETHODCALLTYPE CCommandStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}
	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		if( var.vt != VT_I4 )
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
			if( m_pCommandMgr->m_bSelected )
			{
				m_pCommandMgr->ClearSelected();
			}
			m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			break;
		}
		if( m_fSelected )
		{
			m_pCommandMgr->ClearSelected();
			m_pCommandMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
		}
		else if( m_pCommandMgr->m_bSelected )
		{
			m_pCommandMgr->ClearSelected();
		}
		else
		{
			break;
		}
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pCommandMgr->m_pIPageManager != NULL )
		{
			m_pCommandMgr->m_pIPageManager->RefreshData();
		}
		break;
	case SP_GUTTERSELECT:
		m_fSelected = V_BOOL( &var );
		if( m_lBeginSelect == m_lEndSelect )
		{
			// Selection doesn't change.
			break;
		}
		if( m_fSelected )
		{
			ASSERT( !m_pCommandMgr->m_bSelected );
			m_pCommandMgr->SelectSegment( m_lBeginSelect, m_lEndSelect );
		}
		else
		{
			m_pCommandMgr->ClearSelected();
		}
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

// Make the timeline regain Focus
HRESULT CCommandStrip::GiveTimelineFocus( void )
{
	HRESULT hr = E_POINTER;
	ASSERT(m_pCommandMgr != NULL);
	if(m_pCommandMgr != NULL)
	{
		ASSERT(m_pCommandMgr->m_pTimeline != NULL);
		if(m_pCommandMgr->m_pTimeline != NULL)
		{
			IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
			if(SUCCEEDED(m_pCommandMgr->m_pTimeline->QueryInterface(IID_IOleInPlaceObjectWindowless, (void **) &pIOleInPlaceObjectWindowless)))
			{
				HWND hWnd;
				pIOleInPlaceObjectWindowless->GetWindow(&hWnd);
				::SetFocus(hWnd);
				pIOleInPlaceObjectWindowless->Release();
				hr = S_OK;
			}
		}
	}
	return hr;
}


HRESULT CCommandStrip::DoDragDrop(WPARAM mousekeybutton, LONG lXPos, long lmeasure)
{
	HRESULT hr;
	BOOL bDrop = TRUE;
	IDropSource*	pIDropSource;
	DWORD			dwEffect = DROPEFFECT_NONE;
	if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
	{
		// create data object and image
		hr = CreateDataObject(&m_pISourceDataObject, lmeasure);
		if(SUCCEEDED(hr))
		{
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
			m_nDragDropStatus = source;
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
					switch(m_nDragDropStatus)
					{
					case source:
						// target was different strip
						m_nLastEdit = IDS_DELETE;
						break;
					case target:
						m_nLastEdit = IDS_UNDO_MOVE;
						break;
					}
					m_pCommandMgr->DeleteMarkedCommands(DRAG_SELECT);
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
			m_nDragDropStatus = off;
			hr = bDrop ? S_OK : S_FALSE;
			m_pISourceDataObject->Release();
			m_pISourceDataObject = NULL;
		}
		pIDropSource->Release();
		m_pCommandMgr->UnMarkCommands(DRAG_SELECT);
		m_pCommandMgr->SyncWithDirectMusic();
	}
	else
	{
		hr = E_FAIL;
	}

	if(bDrop)
	{
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		m_pCommandMgr->OnShowProperties();
		m_pCommandMgr->UpdateSegment();
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CCommandStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);

	HRESULT			hr = S_FALSE;
	CString			strError;

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}
	
	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	switch( nMsg )
	{

	case WM_LBUTTONDOWN:
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
		m_pCommandMgr->m_lShiftFromMeasure = 0;
		break;

	case WM_COMMAND:
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			//GiveTimelineFocus();

			// Get a pointer to the property sheet and show it

			VARIANT			var;
			LPUNKNOWN		punk;
			IDMUSProdPropSheet*	pIPropSheet;
			m_pCommandMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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
						break;
					}
					pIPropSheet->Show( TRUE );
					pIPropSheet->Release();
					punk->Release();
				}
			}
			hr = OnShowProperties(m_pCommandMgr->m_pTimeline);
			if (!m_fShowGroupProps)
			{
				// Change to the groove property page
				m_pCommandMgr->OnShowProperties();
			}
			else
			{
				// Change to our property page
				OnShowProperties();
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
			m_pCommandMgr->OnShowProperties();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			break;
		default:
			break;
		}
		break;

	case WM_CREATE:
		m_cfCommandList = RegisterClipboardFormat( CF_COMMANDLIST );

		// Get Left and right selection boundaries
		m_fSelected = FALSE;
		m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lBeginSelect );
		m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lEndSelect );
		m_pCommandMgr->ClearSelected();
		m_pCommandMgr->m_lShiftFromMeasure = 0;
		break;

	default:
		break;
	}
	return hr;
}

void CCommandStrip::SetEnabledItems( HMENU hMenu )
{
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return;
	}

	EnableMenuItem( hMenu, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_PASTE, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
	EnableMenuItem( hMenu, ID_VIEW_PROPERTIES, m_pCommandMgr->m_bSelected ? MF_ENABLED :
					MF_GRAYED | MF_BYCOMMAND );
}


void CCommandStrip::UnselectGutterRange( void )
{
	ASSERT( m_pCommandMgr->m_pTimeline != NULL );

	// Make sure everything on the timeline is deselected.
	m_bSelecting = TRUE;
	m_pCommandMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pCommandMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
}

// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CCommandStrip::GetData

HRESULT CCommandStrip::GetData( void **ppData )
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
		pPPGTrackParams->dwGroupBits = m_pCommandMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pCommandMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pCommandMgr->m_dwProducerOnlyFlags;
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
// CCommandStrip::SetData

HRESULT CCommandStrip::SetData( void *pData )
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
		if( pPPGTrackParams->dwGroupBits != m_pCommandMgr->m_dwGroupBits )
		{
			m_pCommandMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKGROUP;
			m_pCommandMgr->UpdateSegment();
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pCommandMgr->m_dwTrackExtrasFlags )
		{
			m_pCommandMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pCommandMgr->UpdateSegment();
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pCommandMgr->m_dwProducerOnlyFlags )
		{
			m_pCommandMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pCommandMgr->UpdateSegment();
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
// CCommandStrip::OnShowProperties

HRESULT CCommandStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the Timeline
	if( !m_pCommandMgr->m_pTimeline )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the Framework from the timeline
	IDMUSProdFramework* pIFramework = NULL;
	VARIANT var;
	m_pCommandMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
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

	// Set the property page to refer to the Group property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pCommandMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	// release our reference to the property sheet
	pIPropSheet->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandStrip::OnRemoveFromPageManager

HRESULT CCommandStrip::OnRemoveFromPageManager( void )
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
// CCommandStrip::OnShowProperties

HRESULT CCommandStrip::OnShowProperties(IDMUSProdTimeline* pTimeline)
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
	m_pCommandMgr->OnShowProperties();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CCommandStrip::FBDraw

HRESULT CCommandStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);
	UNREFERENCED_PARAMETER(hDC);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandStrip::FBOnWMMessage

HRESULT CCommandStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		m_fShowGroupProps = TRUE;
		OnShowProperties();
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

		if( m_pCommandMgr->m_pTimeline )
		{
			m_pCommandMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandStrip IDMUSProdTimelineEdit

HRESULT CCommandStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	hr = CanCut();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	hr = Copy( pITimelineDataObject );
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}

HRESULT CCommandStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfFormat == 0 )
	{
		m_cfFormat = RegisterClipboardFormat( CF_COMMANDLIST );
		if( m_cfFormat == 0 )
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
	hr = m_pCommandMgr->SaveCommandList( pStreamCopy, TRUE );
	if( FAILED( hr ))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
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
		hr = m_pCommandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pCommandMgr->GetBoundariesOfSelectedCommands( &lStartTime, &lEndTime );
		hr = pITimelineDataObject->SetBoundaries( lStartTime, lEndTime );

		// add the stream to the ITimelineDataObject
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

		// Release the ITimelineDataObject
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
		if(	m_pCommandMgr->m_pCopyDataObject )
		{
			m_pCommandMgr->m_pCopyDataObject->Release();
		}

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pCommandMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the ITimelienDataObject
		//m_pCommandMgr->m_pCopyDataObject->AddRef();
	}

	return hr;
}

HRESULT CCommandStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamPaste;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Make sure everything on the timeline is deselected.
	UnselectGutterRange();
	
	// If the format hasn't been registered yet, do it now.
	if( m_cfFormat == 0 )
	{
		m_cfFormat = RegisterClipboardFormat( CF_COMMANDLIST );
		if( m_cfFormat == 0 )
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
		hr = m_pCommandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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
		m_pCommandMgr->m_pTimeline->PositionToClocks( m_lXPos, &mtTime );
	}
	else
	{
		if( FAILED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}
	}
	m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	m_pCommandMgr->m_pTimeline->MeasureBeatToClocks( m_pCommandMgr->m_dwGroupBits, 0, lMeasure, 0, &mtTime );

	hr = pITimelineDataObject->IsClipFormatAvailable(m_cfFormat);
	if(hr != S_OK)
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}
	
	hr = pITimelineDataObject->AttemptRead( m_cfFormat, &pStreamPaste);
	if(hr != S_OK)
	{
		pITimelineDataObject->Release();
		return E_FAIL;
	}

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( FAILED( m_pCommandMgr->m_pTimeline->GetPasteType( &tlPasteType ) ) )
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

			fChanged = m_pCommandMgr->DeleteBetweenTimes( lStart, lEnd );
		}
	}
	pITimelineDataObject->Release();
	pITimelineDataObject = NULL;

	// Now, do the paste operation
	hr = m_pCommandMgr->LoadDroppedCommandList( pStreamPaste, 0, FALSE, fChanged );	// This also updates the property page.

	pStreamPaste->Release(); 
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}

	if(SUCCEEDED(hr) && fChanged)
	{
		m_nLastEdit = IDS_PASTE;
		m_pCommandMgr->UpdateSegment();
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	}

	return S_OK;
}

HRESULT CCommandStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT			hr;

	hr = CanInsert();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Make sure everything on the timeline is deselected.
	UnselectGutterRange();

	m_nLastEdit = IDS_INSERT;
	hr = m_pCommandMgr->InsertCommand( -1 );	// Insert at first selected.
	m_pCommandMgr->UpdateSegment();
	m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return hr;
}

HRESULT CCommandStrip::Delete( void )
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

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_nLastEdit = IDS_DELETE;
	m_pCommandMgr->DeleteSelected();
	m_pCommandMgr->UpdateSegment();
	m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

	return S_OK;
}

HRESULT CCommandStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	m_pCommandMgr->SelectAll();

	m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	if( m_pCommandMgr->m_pIPageManager != NULL )
	{
		m_pCommandMgr->m_pIPageManager->RefreshData();
	}
	
	return S_OK;
}

HRESULT CCommandStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pCommandMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
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

HRESULT CCommandStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pCommandMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	//return m_pCommandMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pCommandMgr->m_bSelected)
	{
		CommandExt* pCommand = m_pCommandMgr->GetFirstSelectedCommand();
		return pCommand == NULL ? S_FALSE : S_OK;
	}
	else return S_FALSE;
}

HRESULT CCommandStrip::CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_FALSE;

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfFormat == 0 )
	{
		m_cfFormat = RegisterClipboardFormat( CF_COMMANDLIST );
		if( m_cfFormat == 0 )
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
			if( SUCCEEDED( m_pCommandMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
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

HRESULT CCommandStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	//return m_pCommandMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pCommandMgr->m_bSelected)
	{
		CommandExt* pCommand = m_pCommandMgr->GetFirstSelectedCommand();
		return pCommand == NULL ? S_OK : S_FALSE;
	}
	else return S_FALSE;
}

HRESULT CCommandStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	//return m_pCommandMgr->m_bSelected ? S_OK : S_FALSE;
	if (m_pCommandMgr->m_bSelected)
	{
		CommandExt* pCommand = m_pCommandMgr->GetFirstSelectedCommand();
		return pCommand == NULL ? S_FALSE : S_OK;
	}
	else return S_FALSE;
}

HRESULT CCommandStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Look through all measures for a command
	for( int n = 0; n < m_pCommandMgr->m_nMeasures; n++ )
	{
		if( m_pCommandMgr->m_pMeasureArray[n].pCommand )
		{
			// Found a command - return S_OK (yes)
			return S_OK;
		}
	}

	// No commands found - return S_FALSE (no)
	return S_FALSE;
}


// IDropSource Methods
HRESULT CCommandStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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

HRESULT CCommandStrip::GiveFeedback( DWORD dwEffect )
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

CImageList* CCommandStrip::CreateDragImage()
{
	/*
	CImageList* pimage = new CImageList;
	ASSERT(pimage);
	if(!pimage)
	{
		return 0;
	}
	pimage->Create(::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON),
					ILC_COLOR4 | ILC_MASK, 1,1);
	pimage->SetBkColor(CLR_NONE);
	HICON hIcon = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
	if(hIcon == NULL)
	{
		//DWORD err = ::GetLastError();
	}
	pimage->Add(hIcon);
	return pimage;
	*/
	return NULL;
}

HRESULT	CCommandStrip::CreateDataObject(IDataObject** ppIDataObject, long measure)
{
	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_pCommandMgr != NULL );
	if( m_pCommandMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pCommandMgr->m_pTimeline != NULL );
	if( m_pCommandMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// get pJazzFramework
	VARIANT var;
	LPUNKNOWN punk;
	IDMUSProdFramework* pJazzFramework = NULL;
	m_pCommandMgr->m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		punk = V_UNKNOWN(&var);
		if( punk )
		{
			punk->QueryInterface( IID_IDMUSProdFramework, (void**)&pJazzFramework );
			punk->Release();
		}
	}
	ASSERT(pJazzFramework != NULL);
	if(pJazzFramework == NULL)
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

	// Save Selected Commands into stream
	HRESULT hr = E_FAIL;


	if( SUCCEEDED ( pJazzFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{

		// mark the chords as being dragged: this used later for deleting chords in drag move
		// make sure that if this strip is drop target that they get deselected!
		short nZeroOffset = m_pCommandMgr->MarkSelectedCommands(DRAG_SELECT);
		measure -= nZeroOffset;
		// write measure at which mouse grabbed drag selection into the stream
		DWORD cb;
		hr = pIStream->Write( &measure, sizeof( long ), &cb );
	    if( FAILED( hr ) || cb != sizeof( long ))
		{
			hr = E_FAIL;
			goto Leave;
		}


		if( SUCCEEDED ( m_pCommandMgr->SaveDroppedCommandList( pIStream, TRUE, nZeroOffset ) ) )
		{
			// Place CF_COMMANDLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfCommandList, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

Leave:
	if(pJazzFramework)
		pJazzFramework->Release();
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
// IDropTarget CCommandStrip::DragEnter

HRESULT CCommandStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//TRACE("CCommandStrip Drag Enter!\n");
	ASSERT( pIDataObject != NULL );
	ASSERT( m_pITargetDataObject == NULL );

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( CWnd::FromHandle( ::GetDesktopWindow() ), point );
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
// IDropTarget CCommandStrip::DragOver

HRESULT CCommandStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//TRACE("CCommandStrip Drag OVER!\n");
	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	LONG lLeftSide, lRightSide;
	if( (pt.x >= 0) &&
		SUCCEEDED( m_pCommandMgr->m_pTimeline->GetMarkerTime( MARKER_LEFTDISPLAY, TIMETYPE_CLOCKS, &lLeftSide ) ) &&
		SUCCEEDED( m_pCommandMgr->m_pTimeline->ClocksToPosition( lLeftSide, &lLeftSide ) ) &&
		(pt.x >= lLeftSide) && SUCCEEDED( m_pCommandMgr->m_pTimeline->MeasureBeatToPosition( m_pCommandMgr->m_dwGroupBits,0, m_pCommandMgr->m_nMeasures, 0, &lRightSide ) ) &&
		(pt.x < lRightSide ) && (CanPasteFromData( m_pITargetDataObject ) == S_OK) )
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
// IDropTarget CCommandStrip::DragLeave

HRESULT CCommandStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//TRACE("CCommandStrip Drag Leave!\n");

	// Release IDataObject
	if( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( CWnd::FromHandle( ::GetDesktopWindow() ) );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CCommandStrip::Drop

HRESULT CCommandStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(grfKeyState);

	ASSERT( m_pITargetDataObject != NULL );
	ASSERT( m_pITargetDataObject == pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( CWnd::FromHandle( ::GetDesktopWindow() ) );
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
			HWND hwnd = GetTimelineHWND();
			if(hwnd)
			{
				// Display and track menu
				::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  pt.x, pt.y, 0, hwnd, NULL );
				DestroyMenu( hMenu );

				// Need to process WM_COMMAND from TrackPopupMenu
				MSG msg;
				while( ::PeekMessage( &msg, hwnd, NULL, NULL, PM_REMOVE) )
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
		if(m_nDragDropStatus == source)
		{
		//	don't drop at same place

			long lSource, lTarget;
			if( FAILED( m_pCommandMgr->m_pTimeline->PositionToMeasureBeat( 
								m_pCommandMgr->m_dwGroupBits, 0, m_startDragPosition, &lSource, NULL ) ))
			{
				ASSERT( FALSE );
				return E_UNEXPECTED;
			}
			if( FAILED( m_pCommandMgr->m_pTimeline->PositionToMeasureBeat( 
								m_pCommandMgr->m_dwGroupBits, 0, pt.x, &lTarget, NULL ) ))
			{
				ASSERT( FALSE );
				return E_UNEXPECTED;
			}

			if(lSource == lTarget)
			{
				m_pITargetDataObject->Release();
				m_pITargetDataObject = NULL;
				*pdwEffect = DROPEFFECT_NONE;
				if(m_wParam & MK_LBUTTON)
					OnLButtonUp(m_wParam, pt.x, pt.y);
				else if(m_wParam & MK_RBUTTON)
					OnRButtonUp(m_wParam, pt.x, pt.y);
				return S_OK;
			}
		}
		
		BOOL fChanged = FALSE;
		hr = PasteAt( pIDataObject, pt.x, fChanged );

		if(m_nDragDropStatus == source)
		{
			// in our strip, give the handshake
			m_nDragDropStatus = target;
		}
		else
		{
			if( fChanged )
			{
				m_nLastEdit = IDS_INSERT;
				m_pCommandMgr->UpdateSegment();
				m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
				// Update the property page and the object we represent.
				if( m_pCommandMgr->m_pIPageManager != NULL )
				{
					m_pCommandMgr->m_pIPageManager->RefreshData();
				}

				if( m_nDragDropStatus != source )
				{
					UnselectGutterRange();
				}

			}
		}
		if( SUCCEEDED ( hr ) )
		{
			*pdwEffect = m_dwOverDragEffect;
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}



HWND CCommandStrip::GetTimelineHWND()
{
	if( m_pCommandMgr->m_pTimeline )
	{
		IOleWindow *pIOleWindow;
		if( SUCCEEDED(m_pCommandMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleWindow ) ) )
		{
			HWND hwnd = NULL;
			pIOleWindow->GetWindow( &hwnd );
			pIOleWindow->Release();
			return hwnd;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////// Drop (and paste) helpers
//
HRESULT CCommandStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfCommandList ) ))
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}



HRESULT CCommandStrip::PasteAt(IDataObject* pIDataObject, long lPastePos, BOOL &fChanged )
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

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfCommandList ) ) )
	{
		IStream* pIStream;
		if(SUCCEEDED (pDataObject->AttemptRead( pIDataObject, m_cfCommandList, &pIStream)))
		{
			hr = m_pCommandMgr->LoadDroppedCommandList( pIStream, lPastePos, true, fChanged );
		}
	}

	
	pDataObject->Release();
	
	return hr;
}

HRESULT CCommandStrip::OnRButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	m_fShowGroupProps = FALSE;

	// Get the measure number
	long lMeasure;
	if( FAILED( m_pCommandMgr->m_pTimeline->PositionToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lXPos, &lMeasure, NULL ) ))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if( !m_pCommandMgr->m_bSelected )
	{
		// Make sure everything on the timeline is deselected.
		UnselectGutterRange();

		// If nothing is selected, select this measure.
		m_pCommandMgr->ToggleSelect( lMeasure );
		m_pCommandMgr->m_lShiftFromMeasure = lMeasure;

		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
	}
	else if( !( wParam & ( MK_SHIFT | MK_CONTROL )))
	{
		// Make sure this measure is in the selected list.  If it isn't, make it the only one selected
		if( !m_pCommandMgr->IsSelected( lMeasure ))
		{
			// Make sure everything on the timeline is deselected first.
			UnselectGutterRange();

			m_pCommandMgr->ClearSelected();
			m_pCommandMgr->ToggleSelect( lMeasure );
			m_pCommandMgr->m_lShiftFromMeasure = lMeasure;
		}
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*) this, NULL, TRUE );
	}

	if (!m_fShowGroupProps)
	{
		// Change to the groove property page
		m_pCommandMgr->OnShowProperties();
	}
	else
	{
		// Change to our property page
		OnShowProperties();
	}
	//if( m_pCommandMgr->m_pIPageManager != NULL )
	//{
	//	m_pCommandMgr->m_pIPageManager->RefreshData();
	//}

	// Get the cursor position (To put the menu there)
	POINT pt;
	if( !GetCursorPos( &pt ) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	m_pCommandMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);

	return S_OK;
}

HRESULT CCommandStrip::OnLButtonUp( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lYPos);
	m_fShowGroupProps = FALSE;

	// Release mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = FALSE;
	m_pCommandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

	m_fLButtonDown = FALSE;
	m_dwStartDragButton = 0;

	// Get the clock position
	long lClocks;
	if( FAILED(m_pCommandMgr->m_pTimeline->PositionToClocks( lXPos, &lClocks)))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}
	// Get the measure number 
	long lMeasure;
	if(FAILED(m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lClocks, &lMeasure, NULL)))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if(!(wParam & MK_CONTROL) && !(wParam & MK_SHIFT))
	{
		m_pCommandMgr->ClearSelected();
		// Toggle the selection state of this measure
		m_pCommandMgr->ToggleSelect(lMeasure);
		// Redraw our strip
		m_pCommandMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	}
	else if((wParam & MK_CONTROL) && m_bWasSelected)
	{
		m_bWasSelected = false;
		m_pCommandMgr->ToggleSelect(lMeasure);
		// Redraw our strip
		m_pCommandMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
	}

	return S_OK;
}

HRESULT CCommandStrip::OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	// Get the measure number
	long lMeasure;
	if( FAILED( m_pCommandMgr->m_pTimeline->PositionToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lXPos, &lMeasure, NULL ) ))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if( m_fLButtonDown )
	{
		// Release mouse capture
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pCommandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

		// Do Drag'n'drop
		m_wParam = wParam;
		DoDragDrop(m_dwStartDragButton, lXPos, lMeasure);

		// Clear button down flag so we don't do drag'n'drop again.
		m_fLButtonDown= FALSE;

		// need to clear this else control select gets messed up
		m_bWasSelected = false;
	}

	return S_OK;
}

HRESULT CCommandStrip::OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos )
{
	UNREFERENCED_PARAMETER(lYPos);
	long lMeasure;

	m_fShowGroupProps = FALSE;

	m_wParam = wParam;

	// Make sure everything on the timeline is deselected.
	UnselectGutterRange();

	// Get the time position of the mouse click.
	long lClocks;
	if( FAILED( m_pCommandMgr->m_pTimeline->PositionToClocks( lXPos, &lClocks ) ))
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Get the measure number
	if( FAILED( m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lClocks, &lMeasure, NULL ) ) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	if( wParam & MK_SHIFT )
	{
		if( !( wParam & MK_CONTROL ))
		{
			m_pCommandMgr->ClearSelected();
		}
		m_pCommandMgr->SelectSegment( -1, lClocks );
		m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pCommandMgr->OnShowProperties();
		GiveTimelineFocus();
		return S_OK;
	}

	// Get the measure number
	if( FAILED( m_pCommandMgr->m_pTimeline->ClocksToMeasureBeat( m_pCommandMgr->m_dwGroupBits, 0, lClocks, &lMeasure, NULL ) ) )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	m_pCommandMgr->m_lShiftFromMeasure = lMeasure;

	// Get the info for the selected measure
	CommandMeasureInfo*	pMeasureInfo = NULL;
	if ( lMeasure >=0 && lMeasure < m_pCommandMgr->m_nMeasures )
	{
		pMeasureInfo = &(m_pCommandMgr->m_pMeasureArray[lMeasure]);
	}

	if( pMeasureInfo )
	{

		if(pMeasureInfo->pCommand && pMeasureInfo->dwFlags & CMI_SELECTED )
		{
			m_pCommandMgr->OnShowProperties();

			// If not already dragging, start drag'n'drop
			if( !m_dwStartDragButton )
			{
//				DoDragDrop(wParam, lXPos, lMeasure);
				m_dwStartDragButton = wParam;
				m_fLButtonDown = TRUE;
			}
			// capture mouse so we get the LBUTTONUP message as well
			// the timeline will release the capture when it receives the
			// LBUTTONUP message
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pCommandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );
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
			m_pCommandMgr->m_pTimeline->SetTimelineProperty( TP_STRIPMOUSECAPTURE, var );

			// If we clicked on a command, set m_fLButtonDown to TRUE
			if( pMeasureInfo->pCommand )
			{
				m_dwStartDragButton = wParam;
				m_fLButtonDown = TRUE;
			}

			// If CTRL is not pressed, clear all selections in our strip
			if( !( wParam & MK_CONTROL ))
			{
				m_pCommandMgr->ClearSelected();
				m_pCommandMgr->ToggleSelect(lMeasure);
			}
			else if( wParam & MK_CONTROL && pMeasureInfo->dwFlags & CMI_SELECTED)
			{
				// indicate toggle select to mouse up
				m_bWasSelected = true;
			}
			else
			{
				// Toggle the selection state of this measure
				m_pCommandMgr->ToggleSelect( lMeasure );
			
			}
			// Set the properties to the ones for the current selection.
			m_pCommandMgr->OnShowProperties();
			GiveTimelineFocus();
			// Redraw our strip
			m_pCommandMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		}
	}
	return S_OK;
}
