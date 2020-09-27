// ChordMapStrip.cpp : Implementation of CChordMapStrip
#include "stdafx.h"
#include <math.h>
#include "ChordMapStripMgr.h"
#include "chordio.h"
#include "ChordMapMgr.h"
#include "..\shared\RiffStrm.h"
#include "chorddatabase.h"

extern CChordMapStripMgrApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordMapStrip helpers


inline BOOL IsDeleteKey(int nVirtKey, unsigned long lKeyData)
{
	if(nVirtKey == VK_DELETE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

inline BOOL IsCutKey(int nVirtKey, unsigned long lKeyData)
{
	if(::GetKeyState(VK_CONTROL))
	{
		TRACE("Keycode = %x\n", nVirtKey);
	}
	return FALSE;
}

inline BOOL IsInsertKey(int nVirtKey, unsigned long lKeyData)
{
	DWORD dwInsert = 0x01000000;
	if((nVirtKey == 45) && (lKeyData & dwInsert))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CChordMapStrip IUnknown
STDMETHODIMP CChordMapStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
	else if (IsEqualIID(riid, IID_IDropTarget))
	{
		*ppv = (IDropTarget *) this;
	}
	else if (IsEqualIID(riid, IID_IDropSource))
	{
		*ppv = (IDropSource *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdTimelineEdit))
	{
		*ppv = (IDMUSProdTimelineEdit *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
		*ppv = (IDMUSProdStripFunctionBar *) this;
	}

	else
	{
		return E_NOTIMPL;
	}
    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CChordMapStrip::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CChordMapStrip::Release(void)
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
// CChordMapStrip IDMUSProdStrip
/*
static const char astrRootNote[12][3] =
{
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};
*/

HRESULT	STDMETHODCALLTYPE CChordMapStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDC	dc;

	if( m_pChordMapMgr->m_pTimeline )
	{
		m_pChordMapMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pChordMapMgr->m_dwGroupBits, 0, lXOffset );
		if( dc.Attach(hDC) )
		{
			ChordEntry*	pChord;
			RECT		rcChord;

			dc.GetClipBox( &m_rcScreenClip );
			
			dc.SetBkMode( TRANSPARENT );
			// Sync all chords in ChordMapPane
			if( (pChord = m_pChordMapMgr->m_pChordMapList->GetHead()) != NULL )
			{		
				while( pChord ) //( ( pChord ) && (lPosition - lXOffset < rcScreenClip.right) )
				{
					SyncRectToMeasureBeat(pChord );
					
					pChord = pChord->GetNext();
				}
			}

			// Draw Chords in ChordMap Pane
			if( (pChord = m_pChordMapMgr->m_pChordMapList->GetHead()) != NULL )
			{
				COLORREF	crPrevBkColor = dc.SetBkColor( CHORD_CONNECTION_3D_FILL ),
							crPrevForColor = dc.SetTextColor( CHORD_CONNECTION_TEXT_COLOR );
				
				while( pChord ) //( ( pChord ) && (lPosition < rcScreenClip.right) )
				{
					rcChord.left = pChord->m_rect.left;
					rcChord.right = pChord->m_rect.right;
					rcChord.top = pChord->m_rect.top;
					rcChord.bottom = pChord->m_rect.bottom;

					// Display Chord Box.
					DrawChord( dc, pChord, lXOffset );
					
					DrawConnectionBoxes( dc, pChord, lXOffset );
					
					pChord = pChord->GetNext();
				}
				dc.SetBkColor( crPrevBkColor );
				dc.SetTextColor( crPrevForColor );
			}

			// if dragging a connection, draw the line
			if(m_ConnectionDragState.bActive)
			{
				dc.MoveTo(m_ConnectionDragState.xOrg-lXOffset, m_ConnectionDragState.yOrg);
				dc.LineTo(m_ConnectionDragState.xTo, m_ConnectionDragState.yTo);
//				TRACE("\nline to: (%d,%d)", m_ConnectionDragState.xTo, m_ConnectionDragState.yTo);
			}

			// if dragging selection box, draw the line
			if(m_SelectionDragState.bActive)
			{
				/* swapped in ComputeSelectionBoxOutline
				RECT rect;
				if(m_rectBounding.left > m_rectBounding.right)
				{
					rect.left = m_rectBounding.right;
					rect.right = m_rectBounding.left;
				}
				else
				{
					rect.left = m_rectBounding.left;
					rect.right = m_rectBounding.right;
				}
				if(m_rectBounding.top > m_rectBounding.bottom)
				{
					rect.top = m_rectBounding.bottom;
					rect.bottom = m_rectBounding.top;
				}
				else
				{
					rect.top = m_rectBounding.top;
					rect.bottom = m_rectBounding.bottom;
				}
				*/
//				TRACE("top edge: %x bottom edge: %x\r\n", m_rectBounding.top, m_rectBounding.bottom);
				dc.DrawFocusRect(&m_rectBounding);
			}

			dc.Detach();
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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
		V_BOOL(pvar) = FALSE;
		break;
	case SP_MINMAXABLE:
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = FALSE;
		break;
	case SP_MAXHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = enumMaxHeight;
		break;
	case SP_DEFAULTHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = enumDefaultHeight;
		break;

case 666:	// whether strip uses variable mode or fixed time mode for chord placement
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = m_fVariableNotFixed;
		break;
	case WM_USER+1:	// change later to SP_USER+1, also change 666 to SP_USER
		pvar->vt = VT_INT;
		V_INT(pvar) = (long)m_pfnFocusCallback;
		break;
	case WM_USER+2:
		pvar->vt = VT_INT;
		V_INT(pvar) = m_lFocusHint;
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( sp )
	{
	case SP_BEGINSELECT:
		break;
	case SP_ENDSELECT:
		break;
	case SP_GUTTERSELECT:
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect( this, NULL, TRUE );
		m_fSelected = V_BOOL(&var);
		break;
	case 666:	// whether strip uses variable mode or fixed time mode for chord placement
		m_fVariableNotFixed = V_BOOL(&var);
		break;
	case WM_USER+1:	// change later to SP_USER+1, also change 666 to SP_USER
		m_pfnFocusCallback = (pfnCallback)V_INT(&var);
		break;
	case WM_USER+2:
		m_lFocusHint = V_INT(&var);
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::SyncRectToMeasureBeat( ChordEntry* pChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pChordMapMgr != NULL );
	ASSERT( m_pChordMapMgr->m_pTimeline != NULL );

	LONG lPosition;

	// Get LeftX of Chord Box and use existing Y of ChordBox.
	if( FAILED(m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMapMgr->m_dwGroupBits, 0,
														pChord->m_chordsel.Measure(),
														pChord->m_chordsel.Beat(),
														&lPosition )) )
	{
		return E_FAIL;
	}

	pChord->m_rect.left		= (short)lPosition;
	pChord->m_rect.right	= (short)lPosition + CHORD_DEFAULT_WIDTH;

	// When Chord box needs to be made larger to hold connection boxes then add the code here.

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::SetChordXY( ChordEntry* pChord, POINTS& Point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	LONG	lMeasureX, lMeasure, lBeat;

	ASSERT( m_pChordMapMgr != NULL );
	ASSERT( m_pChordMapMgr->m_pTimeline != NULL );

	// If this is an Ending or Beginning Chord then snap to measure boundary
	// Connecting Chords snap to beat boundaries.
	m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, Point.x, &lMeasure, &lBeat );

	if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END) )
	{
		lBeat = 0;
	}
	m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMapMgr->m_dwGroupBits, 0, lMeasure, lBeat, &lMeasureX );

	// Set default Chord widths here (for default).  <m_rect.right would become Position units away from m_rect.left>

	pChord->m_rect.left		= (short)lMeasureX;
	pChord->m_rect.right	= (short)lMeasureX + CHORD_DEFAULT_WIDTH;
	pChord->m_rect.top		= Point.y;
	pChord->m_rect.bottom	= Point.y + CHORD_DEFAULT_HEIGHT;

	pChord->m_lastrect.left		= 0;
	pChord->m_lastrect.right	= 0;
	pChord->m_lastrect.top		= 0;
	pChord->m_lastrect.bottom	= 0;

	pChord->m_chordsel.Measure() = (short)lMeasure;
	pChord->m_chordsel.Beat() = (char)lBeat;

	return hr;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::GetClickedChord( POINTS& Point, ChordEntry** ppChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(ppChord == NULL)
		return E_UNEXPECTED;
	ChordEntry*	pChord = NULL;
	*ppChord = NULL;
	
//	ASSERT( ppChord != NULL );	// Assert that we have a valid address to store the found Chord.


	// Find Last Selected Chord in ChordList...Last Selected chord is last drawn so it
	// should be on top of the Z-order
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			pChord != NULL;
			pChord = pChord->GetNext() )
	{
		if( (Point.x >= pChord->m_rect.left && Point.x <= pChord->m_rect.right) &&
			(Point.y >= pChord->m_rect.top && Point.y <= pChord->m_rect.bottom) )
		{
			*ppChord = pChord;
		}
	}


	return (*ppChord==NULL)?E_FAIL:S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ClearDragRes()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// The Drag Resources do not need to be cleared.
	if( m_pbmpBackground == NULL )
	{
		return S_FALSE;
	}

	m_pbmpBackground->DeleteObject();
	delete m_pbmpBackground;
	m_pbmpBackground = NULL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DeleteAllConnections( int nID )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// Delete Connection
	for(ChordEntry *pchord = m_pChordMapMgr->m_pChordMapList->GetHead();
		pchord != NULL;
		pchord = pchord->GetNext())
	{
		for(NextChord *pnext = pchord->m_nextchordlist.GetHead();
			pnext != NULL;
			)
		{
			// Delete all connections to the newly removed Chord.
			if( pnext->m_nid == nID )
			{
				NextChord* ptemp = pnext->GetNext();
				pchord->m_nextchordlist.Remove(pnext);
				delete pnext;
				pnext = ptemp;
			}
			else
			{
				pnext->m_nextchord = NULL;
				pnext = pnext->GetNext();
			}
		} 
	}
	m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
	DoPropertyPage(m_pChordMapMgr->m_selection);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DeleteConnection( NextChord* pNextChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	pNextChord->m_parent->m_nextchordlist.Remove(pNextChord);
	delete pNextChord;
	return S_OK;
}

void CChordMapStrip::RenumberIDs()
{
	m_pChordMapMgr->m_nIDCounter = 1;
	
	// Assign numbers to all chords
	ChordEntry* pchord = m_pChordMapMgr->m_pChordMapList->GetHead();  
	for( ;  pchord ;  pchord = pchord->GetNext() )
	{ 
		pchord->m_nid = m_pChordMapMgr->m_nIDCounter++;
	}
	
	// Sync connecting chords
	pchord = m_pChordMapMgr->m_pChordMapList->GetHead();  
	for( ;  pchord ;  pchord = pchord->GetNext() )
	{ 
		NextChord* pnext = pchord->m_nextchordlist.GetHead();
		for( ;  pnext ;  pnext = pnext->GetNext() )
		{
			if( pnext->m_nextchord )
			{
				pnext->m_nid = pnext->m_nextchord->m_nid;
			}
		} 
	}
}


HRESULT STDMETHODCALLTYPE CChordMapStrip::ResolveConnections()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	for(ChordEntry *pchord = m_pChordMapMgr->m_pChordMapList->GetHead();
		pchord != NULL;
		pchord = pchord->GetNext())
	{
		for(NextChord *pnext = pchord->m_nextchordlist.GetHead();
			pnext != NULL;
			pnext = pnext->GetNext())
		{
			pnext->m_nextchord = NULL;
		} 
	}

	pchord = m_pChordMapMgr->m_pChordMapList->GetHead();   
	for (;pchord;pchord = pchord->GetNext())
	{
		NextChord *pnext = pchord->m_nextchordlist.GetHead();
		for (;pnext;pnext = pnext->GetNext())
		{
			if (pnext->m_nid)
			{
				ChordEntry *pconnect = m_pChordMapMgr->m_pChordMapList->GetHead();   
				for (;pconnect;pconnect = pconnect->GetNext())
				{
                	if (pconnect->m_nid == pnext->m_nid) break;
				}
				pnext->m_nextchord = pconnect;
			}
		} 
	}
	
	// Renumber connection ids
	RenumberIDs();

	return S_OK;
}

HRESULT	STDMETHODCALLTYPE CChordMapStrip::SetSelectedConnection(NextChord* pNextChord)
{
	if(pNextChord)
	{
		m_pChordMapMgr->m_pSelectedConnection = pNextChord;
		m_pChordMapMgr->m_selection = CChordMapMgr::ConnectionSelected;
	}
	else
	{
		m_pChordMapMgr->m_pSelectedConnection = NULL;
		m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DelSelChords()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_FAIL;
	
	bool bCheckForOrphans = false;

	bool bDelete = GetNumSelections() == S_OK;
	if(bDelete)
	{
		// need to send notify before deleting chords
		m_pChordMapMgr->SendEditNotification(IDS_UNDO_DeleteChord);
	}

	for( ChordEntry *pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
		 pChord != NULL;
	   )
	{
		// This Chord is not selected to be deleted.  Get the next chord in the list and continue.
		if( !pChord->m_chordsel.KeyDown() )
		{
			pChord = pChord->GetNext();
			continue;
		}

		// Delete the Chord and get the next chord in the list.
		int nOldID = pChord->m_nid;
		m_pChordMapMgr->m_pChordMapList->Remove(pChord);
		if(pChord->IsSignPost())
		{
			bCheckForOrphans = true;
		}
		bDelete = true;
		delete pChord;
		pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
		
		// Remove all Connections to this Chord ID
		DeleteAllConnections( nOldID );

		// Resolve connections
		ResolveConnections();
	}

	// make sure all selections are nullified
	ClearSelectedChords();
	ClearSelectedConnections();
	m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
	m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
	DoPropertyPage(m_pChordMapMgr->m_selection);

	if(bCheckForOrphans)
	{
		m_pChordMapMgr->m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
	}


	return hr;

}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DelSelConnections()
// currently only connects THE selected connection
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;

	if(m_pChordMapMgr->m_pSelectedConnection && m_pChordMapMgr->m_pSelectedConnection->m_nextchord)
	{
		m_pChordMapMgr->SendEditNotification(IDS_UNDO_RemoveConnection);
		DeleteConnection(m_pChordMapMgr->m_pSelectedConnection);
		m_pChordMapMgr->m_pSelectedConnection = NULL;
		m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
		m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
		DoPropertyPage(m_pChordMapMgr->m_selection);
	}

	return hr;

}

BOOL CChordMapStrip::ProcessOnSize()
{
	TRACE("Process On Size\n");
	VARIANT			var;
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_MAXIMUM_HEIGHT, &var);
	int h = V_I4(&var);
	h = h > enumDefaultHeight ? h : enumDefaultHeight;
	V_I4(&var) = h;
	BOOL b = SUCCEEDED(m_pChordMapMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, var));
	ASSERT(b);
	return b;
}

#ifndef ONESTEP_DRAGDROP
#define ONESTEP_DRAGDROP
#endif

/*
static void SyncTrace(int num)
{
	TRACE("\r\nSyncTrace at point %d\r\n", num);
}
*/

HRESULT STDMETHODCALLTYPE CChordMapStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL		bRefresh = FALSE;
	BOOL		bSyncEngine = FALSE;
	ChordEntry*	pChordChange = NULL;
	ChordEntry	tempChord;
	bool bConnectionChange = false;
	m_bDrop = false;
	ChordEntry*	pChord;
	static		int cMouseCount;	// used to make sure chord is painted before dragging
	static		bool bChordWasSelected = false;	// used to remember state of control-clicked chords
//	static		bool bInSelectionNet = false; 

	HRESULT		hr = E_NOTIMPL;

	static	POINTS	ptMouseDown;	// Used on MouseDrag and MouseUp operations.  This was the initial MouseDown point.
	static	HCURSOR	hCursor = NULL;	// Used to show dragging cursor

	switch( nMsg )
	{
	case WM_TIMER:
		{
			// wParam is timer id
			switch(wParam)
			{
			case ConnectionDrag:
				UpdateAutoScroll(&m_ConnectionDragState);
				break;
			case SelectionDrag:
				{
					// counter makes sure that compute gets called 
					// only every other time so that window gets
					// opportunity to scroll
//					TRACE("Timer handler for SelectionDrag\r\n");
					UpdateAutoScroll(&m_SelectionDragState);
//					OnWMMessage(WM_MOUSEMOVE, MK_LBUTTON, 
//								m_ptLastMouse.x + (m_ptLastMouse.y << 16), 0,0);
					CWnd* pWnd = GetTimelineCWnd();
					HWND hwnd = pWnd->GetSafeHwnd();
					m_pChordMapMgr->HitTest(m_rectSelect);
					m_pChordMapMgr->m_pTimeline->Refresh();
					::InvalidateRect(hwnd, 0, TRUE);
					::UpdateWindow(hwnd);

					ComputeSelectionBoxOutline(m_ptAnchor, m_ptLastMouse, false);
					/*
					m_pChordMapMgr->HitTest(m_rectSelect);
//					ComputeSelectionBoxOutline(m_ptAnchor, m_ptLastMouse, false);
					CWnd* pWnd = GetTimelineCWnd();
					HWND hwnd = pWnd->GetSafeHwnd();
					m_pChordMapMgr->m_pTimeline->Refresh();
					::InvalidateRect(hwnd, 0, TRUE);
					::UpdateWindow(hwnd);
					*/
				}
				break;
			}
		}
		break;

		case WM_CREATE:
		{
			VARIANT var;
			var.vt = VT_R8;

			m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
			m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST);

			GetWindowOrg(m_xOrg, m_yOrg);	// not used, but keep just in case

			break;
		}
/*
		case WM_DESTROY:
		{
			// persist zoom factor to personality
			VARIANT var;
			var.vt = VT_R8;
			m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_ZOOM, &var);
			ZoomChangeNotification* pZoom = new ZoomChangeNotification;
			pZoom->m_dblZoom = V_R8(&var);
			m_pChordMapMgr->m_pTimeline->OnDataChanged( pZoom );
			break;
		}
*/
		case WM_SIZE:
		{
			ProcessOnSize();
			TRACE("Received Size Message\n");
			break;
		}
		case WM_KEYDOWN:
		{
			int nVirtKey = (int)wParam;
			unsigned long lKeyData = lParam;
			if(IsInsertKey(nVirtKey, lKeyData))
			{
				ChordEntry *pChord;
				POINTS		ptNewPoint = m_ptLastMouse;
				hr = InsertNewChord(ptNewPoint, pChord);
				if(hr == S_OK)
				{
					pChordChange = pChord;					// notify client of selected chord
					m_fSelected = TRUE;
					m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					bRefresh = TRUE;
					bSyncEngine = TRUE;
//					SyncTrace(1);
				}
			}
			else if((m_fSelected = ProcessKey(nVirtKey, lKeyData)))
			{
				bRefresh = TRUE;
				bSyncEngine = TRUE;
//				SyncTrace(2);
				hr = S_OK;
			}
			break;
		}
		case WM_COMMAND:
			hr =  OnCommand(nMsg, wParam, lParam, lXPos, lYPos, bRefresh, bSyncEngine, pChordChange);
//			SyncTrace(3);
			break;
		
		case WM_RBUTTONDOWN:
			m_SelectionDragState.bActive = false;
			m_bInSelectionNet = false;
			ptMouseDown = MAKEPOINTS( lParam );
			SetFocusToTimeline();
			if( SUCCEEDED(GetClickedChord( ptMouseDown, &pChord )) )
			{
				if(!pChord->m_chordsel.KeyDown())
				{
					ClearSelectedChords();
					ClearSelectedConnections();
				}
				pChord->m_chordsel.KeyDown() = TRUE;
				pChordChange = pChord;
				m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
				bRefresh = TRUE;
				bSyncEngine = TRUE;
//				SyncTrace(4);
			}
			else
			{
				if( SUCCEEDED(GetClickedConnection( ptMouseDown, &pChord, NULL )) )
				{

					ClearSelectedChords();
					m_fSelected = FALSE;
					m_fDragging = FALSE;
					m_pChordMapMgr->m_selection = CChordMapMgr::ConnectionSelected;
					bRefresh = TRUE;
				}
				else
				{
					ClearSelectedChords();
					ClearSelectedConnections();
					bRefresh = TRUE;
					DoPropertyPage(CChordMapMgr::NoSelection);				}
			}
			break;
		case WM_RBUTTONUP:
			hr = OnRButtonUp(nMsg, wParam, lParam, lXPos, lYPos);
			break;
		
		case WM_INSERT_CHORD:
			hr = OnInsert(nMsg, wParam, lParam, lXPos, lYPos, bRefresh, bSyncEngine, pChordChange);
//			SyncTrace(5);
			break;

// MouseMoving 
		case WM_MOUSEMOVE:
//			TRACE("\n YPOS = %x", lYPos);
			if( wParam & MK_RBUTTON ) 
			{
				break;
			}
			else if (m_bInSelectionNet)
			{
				CWnd* pWnd = GetTimelineCWnd();
				HWND hwnd = pWnd->GetSafeHwnd();
				bRefresh = FALSE;
				m_ptLastMouse = MAKEPOINTS(lParam);
				m_pChordMapMgr->HitTest(m_rectSelect);
				m_pChordMapMgr->m_pTimeline->Refresh();
				::InvalidateRect(hwnd, 0, TRUE);
				::UpdateWindow(hwnd);

				ComputeSelectionBoxOutline(m_ptAnchor, m_ptLastMouse);
			}

			// Timeline Bug does not pass mouse messages along to it's children
			// so we can't set the cursor back to it's original state when the
			// user drags a Chord to the timeline and releases the mouse button.
			// Also, we don't have a window handle so we can't call ::SetCapture().
			if( !(wParam & MK_LBUTTON) && (hCursor != NULL) )
			{
				SetCursor( hCursor );
				hCursor = NULL;
				m_fDragging = FALSE;
				ClipCursor( NULL );
				TRACE("\nSet Dragging False");
			}

// LMB is DOWN
			if( wParam & MK_LBUTTON )
			{
				// User is dragging a Chord.
				if(!m_fConnectionDrag && SUCCEEDED(GetClickedChord( ptMouseDown, &pChord )) )
				{
					if(cMouseCount == 0)
						DoDragDrop(wParam, lParam, pChord, pChordChange);
					else
						--cMouseCount;
					break;
				}

				// User is dragging a Connection.
				else if( m_fConnectionDrag == TRUE )
				{
					TRACE("\nDragConnection");
					DragConnection(lParam, ptMouseDown);
					bRefresh = TRUE;
				}
				break;
			}
// LMB is UP
			else
			{
//				TRACE("\nClean up the DCs");
				// Clean up the DCs if the user drags the connection outside of the window, releases the button,
				// and we don't get the message.
				if( m_pbmpBackground != NULL )
				{
					m_fConnectionDrag = FALSE;
					ClearDragRes();
				}
			}
			
			break;

		case WM_LBUTTONDOWN:
		// Store Mouse Position.
		ptMouseDown = MAKEPOINTS( lParam );
		m_ptLastMouse = ptMouseDown;
			
			SetFocusToTimeline();

			// User want to drag the Chord.
			if( SUCCEEDED(GetClickedChord( ptMouseDown, &pChord )) )
			{
				ClearSelectedConnections();
				if(wParam & MK_CONTROL)
				{
					if(!pChord->m_chordsel.KeyDown())
					{
						bChordWasSelected = false;
						pChord->m_chordsel.KeyDown() = !pChord->m_chordsel.KeyDown();
					}
					else
					{
						bChordWasSelected = true;
					}
				}
				else
				{
					if(!pChord->m_chordsel.KeyDown())
					{
						ClearSelectedChords();
						ClearSelectedConnections();
					}
					pChord->m_chordsel.KeyDown() = TRUE;
				}
				bRefresh = TRUE;
				bSyncEngine = TRUE;
//				SyncTrace(6);
				cMouseCount = 3;
				pChordChange = pChord;
				m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
			}
			else
			{
				/////////////////////
				// Drag Connection (Modify/Add)
				if( SUCCEEDED(GetClickedConnection( ptMouseDown, &pChord, NULL )) )
				{

					ClearSelectedChords();
					m_fSelected = FALSE;
					m_fDragging = FALSE;
					m_pChordMapMgr->m_selection = CChordMapMgr::ConnectionSelected;
					m_fConnectionDrag = TRUE;
					if(m_fCapture == false)
					{
						VARIANT var;
						var.vt = VT_BOOL;
						V_BOOL(&var) = TRUE;
						m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
						m_fCapture = true;
					}
					m_ConnectionDragState.bActive = true;
					break;	// Break Modify Connection
				}
				else
				{
					// whitespace click
					ClearSelectedChords();
					ClearSelectedConnections();
					VARIANT var;
					var.vt = VT_BOOL;
					V_BOOL(&var) = TRUE;
					m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
					m_fCapture = true;
					m_bInSelectionNet = true;
					m_ptAnchor = ptMouseDown;
					ComputeSelectionBoxOutline(m_ptAnchor, m_ptLastMouse);
					m_SelectionDragState.bActive = true;
					bRefresh = TRUE;
					DoPropertyPage(CChordMapMgr::NoSelection);
				}
			}
			break;

		case WM_LBUTTONUP:			
			POINTS ptMouseUp;
			if(m_fCapture)
			{
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
				m_fCapture = false;
				if(m_bInSelectionNet)
				{
					int nsel;
					GetNumSelections(&nsel);
					if(nsel == 1)
						m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					else if(nsel > 1)
						m_pChordMapMgr->m_selection = CChordMapMgr::MultipleSelections;
					if(m_SelectionDragState.bAutoscroll)
					{
						::KillTimer(GetTimelineCWnd()->m_hWnd, SelectionDrag);
						TRACE("Killing Timer for SelectionDrag\n");
					}
					m_SelectionDragState.bActive = false;
					m_SelectionDragState.dir = DragState::off;
					m_bInSelectionNet = false;
					bRefresh = TRUE;
				}
			}
			m_ConnectionDragState.bActive = false;
			m_ConnectionDragState.dir = DragState::off;
			if(m_ConnectionDragState.bAutoscroll)
			{
				::KillTimer(GetTimelineCWnd()->m_hWnd, ConnectionDrag);
			}
			ptMouseUp = MAKEPOINTS( lParam );

			// If user didn't select a Chord then they want to:
			// 1) Add a Connection between Chords
			// 2) Cancel a Chord-Add operation
			// 3) Drop a Dragged Chord
			// 4) Add a new Chord to the ChordMap
			// 5) Cancel a Connection drag operation
			// 6) Cancel Chord selection if m_fSelected flag is set
			//
			if( FAILED(GetClickedChord( ptMouseUp, &pChord )) )
			{
				if( SUCCEEDED(GetClickedConnection( ptMouseUp, &pChord, NULL )) )
				{
					bConnectionChange = TRUE;
				}

				/////////////////////////
				// 6) Cancel Chord Select Operation
				if( m_fSelected == TRUE )
				{
					m_fSelected = FALSE;
				}

				/////////////////////////////
				// 5) Cancel Connection drag operation
				if( m_fConnectionDrag == TRUE )
				{
					ClearDragRes(); // Release DCs and BackBuffer.
					m_fConnectionDrag = FALSE;
					bRefresh = TRUE;
					hr = S_OK;
					break;
				}
			}
// User Selected a Chord (pChord is valid).
			else
			{	
				// Since the method succeeded we should always have a valid ptr to the selected chord.
				ASSERT( pChord != NULL );
				ChordEntry* pPrevChord;

				// If this release is over the same Chord that was initially clicked, then select/deselect it.
				// At this point it is not a drag operation.
				if( SUCCEEDED(GetClickedChord( ptMouseDown, &pPrevChord )) )
				{

					if( pPrevChord == pChord )
					{
						int selcount;
						if(pChord->m_chordsel.KeyDown())
						{
							if(!(wParam & MK_CONTROL))
							{
								// deselect everything else, then reselect
								ClearSelectedChords();
								ClearSelectedConnections();
								pChord->m_chordsel.KeyDown() = TRUE;
							}
							else
							{
								if(bChordWasSelected)
									pChord->m_chordsel.KeyDown() = !pChord->m_chordsel.KeyDown();
								bChordWasSelected = false;
							}
							// make sure chord is on top
							m_pChordMapMgr->m_pChordMapList->Remove(pChord);
							pChord->SetNext(0);
							m_pChordMapMgr->m_pChordMapList->AddTail(pChord);
//							bSyncEngine = TRUE;
							bSyncEngine = FALSE;  // already did sync
//							SyncTrace(7);
							pChordChange = pChord;	// notify client of selected chord
							m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
						}
						
						m_fSelected = ( GetNumSelections(&selcount) == S_OK ) ? TRUE : FALSE;
						hr = S_OK;

						m_fDragging = FALSE;

						bRefresh = TRUE;
						
						// Show Chord(s) in PropertyPage if visible
						switch(selcount)
						{
						case 0:
							m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
							DoPropertyPage(CChordMapMgr::NoSelection);
							pChordChange = 0;
							break;
						case 1:
							pChordChange = pChord;
//							DoPropertyPage(CChordMapMgr::ChordSelected);
							break;
						default: // multiple select
							pChordChange = 0;
							m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
							DoPropertyPage(CChordMapMgr::MultipleSelections);
							break;
						}
						hr = S_OK;
					}
					// Restore the previous cursor
					if( hCursor != NULL )
					{
						SetCursor( hCursor );
						hCursor = NULL;
						ClipCursor( NULL );
					}
					break;	// User Selected a Chord
				}

				// Drop the Connection on a Chord
				if( m_fConnectionDrag == TRUE )
				{
					ChordEntry* pFromChord;
					ChordEntry* pNextChord;
					if( SUCCEEDED(GetClickedConnection( ptMouseDown, &pFromChord, &pNextChord )) )
					{
						ASSERT( pFromChord != NULL );

						// Don't connect chords to themselves
						if( pFromChord == pChord )
						{
							hr = S_FALSE;
						}
						// If the user is connecting a Chord that occurs before the first, then fail.
						// We will also prohibit the user from dragging Chords beyond any Chords it is connected to.
						else if( pFromChord->m_rect.left >= pChord->m_rect.left )
						{
							hr = E_FAIL;
						}
						else // Ok to Connect these two Chords.
						{
							// first make sure not multiple connecting
							bool bAllowConnection = true;
							for(NextChord* pnext = pFromChord->m_nextchordlist.GetHead();
									pnext;pnext = pnext->GetNext() )
							{
								// Prohibit more than one connection from pFromChord to pToChord
								if( pnext->m_nextchord == pChord )
								{
									CString msg;
									msg.LoadString(IDS_MULTIPLE_CONN);
									AfxMessageBox(msg,MB_ICONEXCLAMATION );
									bAllowConnection = false;
								}
							}

							if(bAllowConnection)
							// Add new Connection
							{
								if(pNextChord)
									m_pChordMapMgr->SendEditNotification(IDS_UNDO_MoveConnection);
								else
									m_pChordMapMgr->SendEditNotification(IDS_UNDO_AddConnection);
								// allow connection should always succeed
								if( SUCCEEDED(hr = AddConnection( pFromChord, pChord )) )
								{
									bSyncEngine = TRUE;
								}
								bConnectionChange = true;
							}
						}
					}


					bRefresh = TRUE;
					
					m_fConnectionDrag = FALSE;
					hr = S_OK;

					// Clean up DCs and Bitmaps
					ClearDragRes();
					break;

				}

			}
						
	}
	if( bSyncEngine == TRUE )
	{
		TRACE( "ChordMapStrip: Syncing w/ Engine\n" );
		ComputeChordMap();
		if(!m_bDrop && pChordChange)
		{
			ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
			// this is roundabout way to copy chord, but what the hey, we need something the client can delete
			ChordEntry* pc = new ChordEntry;
			CPropChord prop;
			m_pChordMapMgr->ChordToPropChord(&prop, pChordChange);
			m_pChordMapMgr->PropChordToChord(pc, &prop);
			pCallback->SetChordEntry(pc);
			m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );
			DoPropertyPage(m_pChordMapMgr->m_selection);
		}
		else if(bConnectionChange)
		{
			m_pChordMapMgr->m_pTimeline->OnDataChanged( NULL );
			DoPropertyPage(CChordMapMgr::ConnectionSelected);
		}
		else
		{
			m_pChordMapMgr->m_pTimeline->OnDataChanged( NULL );
		}
	}
	else if(bConnectionChange)
	{
		DoPropertyPage(CChordMapMgr::ConnectionSelected);
	}
	/*  This already covered in the if (bSyncEngine == TRUE) clause
	else if(!m_bDrop && pChordChange)
	{
		ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
		// this is roundabout way to copy chord, but what the hey, we need something the client can delete
		ChordEntry* pc = new ChordEntry;
		CPropChord prop;
		m_pChordMapMgr->ChordToPropChord(&prop, pChordChange);
		m_pChordMapMgr->PropChordToChord(pc, &prop);
		pCallback->SetChordEntry(pc);
		m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );
		DoPropertyPage(m_pChordMapMgr->m_selection);
	}
*/
	if( bRefresh == TRUE )
	{
		// Kludge: Timeline doesn't refresh properly.  Force refresh.
		m_pChordMapMgr->m_pTimeline->Refresh();
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DoPropertyPage( enum CChordMapMgr::SelectionType selection, BOOL fShow )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(CChordMapMgr::NoSelection <= selection && selection < CChordMapMgr::EndEnum);
	if(!(CChordMapMgr::NoSelection <= selection && selection < CChordMapMgr::EndEnum))
	{
		return E_UNEXPECTED;
	}
	m_pChordMapMgr->m_selection = static_cast<enum CChordMapMgr::SelectionType>(selection);
	if(m_pChordMapMgr->m_selection == CChordMapMgr::PaletteSelected)
	{
		ASSERT( m_pChordMapMgr->m_pJazzFramework != NULL );

		if( fShow == TRUE )
		{
			IDMUSProdPropSheet* pJPS;
			if( SUCCEEDED(m_pChordMapMgr->m_pJazzFramework->QueryInterface( IID_IDMUSProdPropSheet,
				(void**)&pJPS )))
			{
				pJPS->Show(TRUE);
				pJPS->Release();
			}
		}

		m_pChordMapMgr->OnShowProperties();
		if( m_pChordMapMgr->m_pChordPropPageMgr )
		{
			m_pChordMapMgr->m_pChordPropPageMgr->RefreshData();
		}
	}
	else if(m_pChordMapMgr->m_selection == CChordMapMgr::ChordSelected)
	{
		// Prepare the template SelectedChord buffer with
		// selected chords.
		m_pChordMapMgr->PreparePropChord();


		ASSERT( m_pChordMapMgr->m_pJazzFramework != NULL );

		if( fShow == TRUE )
		{
			IDMUSProdPropSheet* pJPS;
			if( SUCCEEDED(m_pChordMapMgr->m_pJazzFramework->QueryInterface( IID_IDMUSProdPropSheet,
				(void**)&pJPS )))
			{
				pJPS->Show(TRUE);
				pJPS->Release();
			}
		}

		// Get the window with the focus so we can set the focus back to it
		// to handle keyboard actions like DELETE
		CWnd* pWindowWithFocus = CWnd::GetFocus();

		m_pChordMapMgr->OnShowProperties();

		// Set the focus back to the previous window 
		if(pWindowWithFocus)
			pWindowWithFocus->SetFocus();

		if( m_pChordMapMgr->m_pChordPropPageMgr )
		{
			m_pChordMapMgr->m_pChordPropPageMgr->RefreshData();
		}
	}
	else if(m_pChordMapMgr->m_selection == CChordMapMgr::ConnectionSelected)
	{
		// Prepare the template SelectedConnection buffer with
		// selected connection.
//		m_pChordMapMgr->PrepareConnection();


		ASSERT( m_pChordMapMgr->m_pJazzFramework != NULL );

		if( fShow == TRUE )
		{
			IDMUSProdPropSheet* pJPS;
			if( SUCCEEDED(m_pChordMapMgr->m_pJazzFramework->QueryInterface( IID_IDMUSProdPropSheet,
				(void**)&pJPS )))
			{
				pJPS->Show(TRUE);
				pJPS->Release();
			}
		}

		// Get the window with the focus so we can set the focus back to it
		// to handle keyboard actions like DELETE
		CWnd* pWindowWithFocus = CWnd::GetFocus();
		
		m_pChordMapMgr->OnShowProperties();

		// Set the focus back to the previous window 
		if(pWindowWithFocus)
			pWindowWithFocus->SetFocus();

		if( m_pChordMapMgr->m_pConnectionPropPageMgr )
		{
			m_pChordMapMgr->m_pConnectionPropPageMgr->RefreshData();
		}
	
	}
	else
	{
		// this is the Invalid Prop Page
		if( m_pChordMapMgr->m_pChordPropPageMgr != NULL )
		{
			m_pChordMapMgr->m_pChordPropPageMgr->RefreshData();
		}
		m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
		m_pChordMapMgr->OnShowProperties();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::AddConnection( ChordEntry* pFromChord, ChordEntry* pToChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NextChord* pnext;
	if(m_pChordMapMgr->m_pSelectedConnection->m_nextchord != NULL)
	{

		// use this to link to chord, first delete its old connection
		m_pChordMapMgr->m_pSelectedConnection->m_nextchord = 0;
		m_pChordMapMgr->m_pSelectedConnection->m_nid = 0;
		pnext = m_pChordMapMgr->m_pSelectedConnection;
	}

	else
	{
		// find blank NextChord
		for(pnext = pFromChord->m_nextchordlist.GetHead();
			(pnext) && (pnext->m_nextchord) /*&& (pnext->m_nextchord != pToChord)*/;
			pnext = pnext->GetNext() );

		ASSERT( pnext != NULL );


		ASSERT( pnext->m_nextchord == NULL );

		NextChord* pConnection = new NextChord;
		if( pConnection == NULL )
		{
			return E_FAIL;
		}
		// Get the new last box ready (as far as we can)
		pConnection->m_parent = pFromChord;
		// Connect the Connection to the parent chord.
		pFromChord->m_nextchordlist.AddTail(pConnection);
	}

	// Link the new Connection
	pnext->m_nextchord = pToChord;
	pnext->m_nid = pToChord->m_nid;

	// Establish min/max beats
	long nC1Pos, nC2Pos;
	long nC1Clocks, nC2Clocks;
	m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
													  pFromChord->m_chordsel.Measure(), 
													  pFromChord->m_chordsel.Beat(), 
													  &nC1Clocks );
	m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
													  pToChord->m_chordsel.Measure(),
													  pToChord->m_chordsel.Beat(), 
													  &nC2Clocks );
	long nDeltaC1C2 = ( nC1Clocks > nC2Clocks ) ? nC1Clocks - nC2Clocks : nC2Clocks - nC1Clocks;

	m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, nDeltaC1C2, &nC1Pos, &nC2Pos );

	long bpm;
	m_pChordMapMgr->GetTimeSig(&bpm, 0);
	nC2Pos += bpm*nC1Pos;

	if( m_fVariableNotFixed )
	{
		pnext->m_nminbeats = 1;
		pnext->m_nmaxbeats = (short)nC2Pos;
	}
	else
	{
		pnext->m_nminbeats = (short)nC2Pos;
		pnext->m_nmaxbeats = (short)nC2Pos;
	}

	// update selected connection
	SetSelectedConnection(pnext);	

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::CountConnections( ChordEntry* pChord, int* pnCount )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pChord != NULL );

	if( pChord == NULL )
	{
		return E_INVALIDARG;
	}

	*pnCount = 0;

	NextChord *pNextChord = pChord->m_nextchordlist.GetHead();

	// New Chord
	if( !pNextChord )
	{
		return S_OK;
	}

	do{
		(*pnCount)++;
	}while( (pNextChord = pNextChord->GetNext()) != NULL );

	return ((*pnCount) == 0)?S_FALSE:S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ConnectionToChord( NextChord* pNextChord, ChordEntry** ppChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pNextChord != NULL );
	ASSERT( ppChord != NULL );
	
	if( (ppChord == NULL) || (pNextChord == NULL) )
	{
		return E_INVALIDARG;
	}

	// Reached the end of the list.  Return E_FAIL to indicate that we couldn't
	// return a valid ChordEntry pointer.
	if( (*ppChord = (((SmallNextChord*)(&*pNextChord)))->m_nextchord) == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ChordFromID( int nID, ChordEntry** ppChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	for(	ChordEntry* pceOldChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			pceOldChord != NULL;
			pceOldChord = pceOldChord->GetNext() )
	{
		if( ((SmallChordEntry*)pceOldChord)->m_nid == nID )
		{
			break;
		}
	}

	if( pceOldChord == NULL )
	{
		return E_FAIL;
	}

	(*ppChord) = pceOldChord;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::DrawConnectionBoxes( CDC& cDC, ChordEntry* pChord, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( pChord != NULL );

	RECT		boundrect;	// Bounding rectangle used to draw Boxes.
	int			nNumConnections;
	NextChord*	pConnection;

	// Draw Chord Connection boxes in front of Chords.  Including blank one for adding.
	if( CountConnections( pChord, &nNumConnections ) == S_FALSE )
	{
		return S_OK; // This is an ending Chord and should not have connection boxes.
	}

	// Increment the number of connections so we can account for a blank 'Add' connection box.

//	double zoom = ZoomFactor();
//	if(zoom < 1.0) zoom = 1.0;
	// for now zoom = 1
	long zoom = 1;

	boundrect.bottom	=	(pChord->m_rect.top + ( long(zoom*(pChord->m_rect.bottom - pChord->m_rect.top) )>>1)) -
					((( long(CONNECTION_HEIGHT*zoom)>>1)*nNumConnections)+nNumConnections);
	boundrect.left	= pChord->m_rect.left + (pChord->m_rect.right - pChord->m_rect.left)*zoom + 1;
	boundrect.right	= boundrect.left + CONNECTION_WIDTH*zoom;

	// Set next Connection ptr so we can set the text for connections 
	// right after we draw the boxes for them.  
	pConnection = pChord->m_nextchordlist.GetHead();

	// Set text color for Connection box text and restore before method ends
	int			nEndWeight;
	VARIANT		vt;
	vt.vt = VT_I4;

	COLORREF	colRef = cDC.GetTextColor();
	cDC.SetTextColor( CONNECTION_TEXT_COLOR );

	while( pConnection )
	{
////////////////
// Draw the Connection boxes
//
		CPen *pOldPen;
		CPen pen;

		if (m_pChordMapMgr->m_pSelectedConnection == pConnection) 
		{
			if (!pen.CreatePen(PS_SOLID,2,RGB(0xFF,0,0)))
			{
				return E_OUTOFMEMORY;
			}
			pOldPen = cDC.SelectObject(&pen);
		} 


		boundrect.top = boundrect.bottom;
		boundrect.bottom += CONNECTION_HEIGHT+1;

		// If the top connection box is heigher then the top of the screen, then make the
		// top connection box = 0 and bump the bottom connection box down the amount
		// of where we adjusted for the top.
		if( boundrect.top < 0 )
		{
			boundrect.bottom += -(boundrect.top);
			boundrect.top = 0;
		}


		// Save the Connection box coordinates for mouse-hit checking.
		pConnection->m_rect.left	= (short)(boundrect.left);
		pConnection->m_rect.top		= (short)(boundrect.top);
		pConnection->m_rect.right	= (short)(boundrect.right);
		pConnection->m_rect.bottom	= (short)(boundrect.bottom);

		RECT rect;
		rect.top = boundrect.top;
		rect.bottom = boundrect.bottom;
		rect.left = boundrect.left - lXOffset;
		rect.right = boundrect.right -lXOffset;

		if(m_pChordMapMgr->m_pSelectedConnection == pConnection)
		{
			cDC.FillSolidRect(	&rect, CONNECTION_SEL_FILL );
		}
		else
		{
			cDC.FillSolidRect(	&rect, CONNECTION_FILL_BACKGROUND );
		}
		cDC.Draw3dRect(	&rect, CHORD_CONNECTION_3D_HIGHLIGHT, CHORD_CONNECTION_3D_LOWLIGHT );

		// If this connection connects two chords then show the weight and min/max data
		// for the connection.  Otherwise it's a blank 'Add' Connection box.
		if( pConnection->m_nextchord != NULL )
		{
			// Show weight as a measurement of the box.
			nEndWeight = (int)((rect.right-rect.left)*(((SmallNextChord*)pConnection)->m_nweight/100.0));
			if(m_pChordMapMgr->m_pSelectedConnection == pConnection)
			{
				cDC.FillSolidRect(	rect.left+1,
									rect.top+1,
									nEndWeight,
									rect.bottom-rect.top-2,
									CONNECTION_SEL_FILL );
			}
/*
			else if(m_fVariableNotFixed)
			{
				// draw grey half rect used to show min/max
				cDC.FillSolidRect(	rect.left+1,
									rect.top+1,
									nEndWeight,
									rect.bottom-rect.top-2,
									CONNECTION_FILL );
									
			}

*/
////////////////
// Update min/max depending on the distance to the next chord.
//
			long nC1Pos, nC2Pos;
			long nC1Clocks, nC2Clocks;
			m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0, 
															  pChord->m_chordsel.Measure(), 
															  pChord->m_chordsel.Beat(), 
															  &nC1Clocks );
			m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
															  pConnection->m_nextchord->m_chordsel.Measure(),
															  pConnection->m_nextchord->m_chordsel.Beat(), 
															  &nC2Clocks );
			long nDeltaC1C2 = ( nC1Clocks > nC2Clocks ) ? nC1Clocks - nC2Clocks 
															: nC2Clocks - nC1Clocks;
			m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, nDeltaC1C2, &nC1Pos, &nC2Pos );
			
			long bpm = 4;
			m_pChordMapMgr->GetTimeSig(&bpm, 0);
			nC2Pos += (bpm*nC1Pos);
			if(!m_fVariableNotFixed)
			{
				pConnection->m_nminbeats = (short)nC2Pos;
				pConnection->m_nmaxbeats = (short)nC2Pos;
			}						

////////////////
// Draw the Min/Max numbers in the Connection box
//
/*
			if(m_fVariableNotFixed)
			{
				CString csBuf;

				csBuf.Format( "%d/%d",	((SmallNextChord*)pConnection)->m_nminbeats,
										((SmallNextChord*)pConnection)->m_nmaxbeats );
				cDC.DrawText( csBuf, &rect, DT_CENTER );
			}
*/
//////////////////
// Draw the Connecting line
//
			cDC.MoveTo( pConnection->m_rect.right - lXOffset, pConnection->m_rect.top + ((pConnection->m_rect.bottom-pConnection->m_rect.top)>>1) );
			cDC.LineTo( pConnection->m_nextchord->m_rect.left - lXOffset, pConnection->m_nextchord->m_rect.top + ((pConnection->m_nextchord->m_rect.bottom-pConnection->m_nextchord->m_rect.top)>>1) );
			
		}
		if(m_pChordMapMgr->m_pSelectedConnection == pConnection)
		{
			cDC.SelectObject(pOldPen);
		}

		pConnection = pConnection->GetNext();
	}

	
	// Restore previous text color
	cDC.SetTextColor( colRef );

	return S_OK;
}
					
HRESULT STDMETHODCALLTYPE CChordMapStrip::DrawChord( CDC& cDC, ChordEntry* pChord, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pChord != NULL );

	if( pChord->m_rect.left < 0 )
	{
		pChord->m_rect.left = 0;
		pChord->m_rect.right = CHORD_DEFAULT_WIDTH;
	}

	if( pChord->m_rect.right - pChord->m_rect.left > CHORD_DEFAULT_WIDTH)
	{
		pChord->m_rect.right = pChord->m_rect.left + CHORD_DEFAULT_WIDTH;
	}

	if( pChord->m_rect.top < 0 )
	{
//		pChord->m_rect.top = 0;
//		pChord->m_rect.bottom = CHORD_DEFAULT_HEIGHT;
		pChord->m_rect.top = -pChord->m_rect.top;
		pChord->m_rect.bottom += CHORD_DEFAULT_HEIGHT;
	}

	if( pChord->m_rect.bottom - pChord->m_rect.top > CHORD_DEFAULT_HEIGHT)
	{
		pChord->m_rect.bottom = pChord->m_rect.top + CHORD_DEFAULT_HEIGHT;
	}

	// Calculate Chord rect based on measure and beat.
	// called in draw: for zoom all chords need to be computed prior to draw
//	SyncRectToMeasureBeat( pChord );

//	double zoom = ZoomFactor();
//	if(zoom < 1.0) zoom = 1.0;
	// for now zoom = 1
	long zoom = 1;

	COLORREF crFill, crHighLight, crLowLight;
	// Draw selected Chord box
	if( pChord->m_chordsel.KeyDown() == TRUE )
	{
		crFill = CHORD_CONNECTION_3D_SEL_FILL;
		crHighLight = CHORD_CONNECTION_3D_SEL_HIGHLIGHT;
		crLowLight = CHORD_CONNECTION_3D_SEL_LOWLIGHT;
	}
	// Draw unselected Chord box
	else
	{
		crFill = CHORD_CONNECTION_3D_FILL;
		crHighLight = CHORD_CONNECTION_3D_HIGHLIGHT;
		crLowLight = CHORD_CONNECTION_3D_LOWLIGHT;
	}

	long d1 = pChord->m_rect.left - lXOffset * zoom;
	long d2 = pChord->m_rect.top;
	long d3 = (pChord->m_rect.right - pChord->m_rect.left) * zoom;
	long d4 = (pChord->m_rect.bottom - pChord->m_rect.top) * zoom;

	cDC.FillSolidRect(	d1, d2, d3, d4, crFill);

	cDC.Draw3dRect(	d1, d2, d3, d4, crHighLight, crLowLight );

	// Display Special characters for Beginning and Ending Chords.
	CBitmap	cBitmap;
	CPoint	cPoint( d1+1, d2+1 );
	CSize	cSize;
	if( pChord->m_dwflags & CE_START )	// Starting Chord.
	{
		if(pChord->m_chordsel.KeyDown())
		{
			cBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_GOSIGN_SEL ) );
		}
		else
		{
			cBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_GOSIGN ) );
		}
		cSize = cBitmap.GetBitmapDimension();
//		cPoint.y -= GO_SIGN_HEIGHT;
		cDC.DrawState( cPoint, cSize, &cBitmap, DST_BITMAP | DSS_NORMAL );
		cBitmap.DeleteObject();
	}
	if( pChord->m_dwflags & CE_END )		// Ending Chord
	{
		if( pChord->m_dwflags & CE_START )
		{
			cPoint.x = (pChord->m_rect.right - STOP_SIGN_WIDTH - lXOffset)*zoom;
		}

		if(pChord->m_chordsel.KeyDown())
		{
			cBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_STOPSIGN_SEL ) );
		}
		else
		{
			cBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_STOPSIGN ) );
		}
		cSize = cBitmap.GetBitmapDimension();
//		cPoint.y = (rect.bottom - STOP_SIGN_HEIGHT);
		cDC.DrawState( cPoint, cSize, &cBitmap, DST_BITMAP | DSS_NORMAL );
		cBitmap.DeleteObject();
	}
	
	if( !(pChord->m_dwflags & (CE_START + CE_END)) )
	{
		// need size of bitmap
		cBitmap.LoadBitmap( MAKEINTRESOURCE (IDB_GOSIGN) );
		cSize = cBitmap.GetBitmapDimension();
		cBitmap.DeleteObject();
	}
	
	
	// Display the Chord name.
	RECT rect;
	CString csChordName1, csChordName2;
	char	szRootName[ROOT_TO_NAME_MAX];

	pChord->m_chordsel.RootToString(szRootName);
	int nChars = strlen(szRootName) + 1 + strlen(pChord->m_chordsel.Name() );
	if(nChars <= 6)
	{
		csChordName1.Format( "%s %s", szRootName, pChord->m_chordsel.Name() );
	}
	else
	{
		csChordName1 = szRootName;
		csChordName2 = pChord->m_chordsel.Name();
	}

	LOGFONT lf;
	memset( &lf, 0 , sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(8, GetDeviceCaps(cDC, LOGPIXELSY), 72);
	lf.lfWeight = 400;
//	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	_tcscpy(lf.lfFaceName, _T("Arial"));

	
//	rect.left	= pChord->m_rect.left+1 - lXOffset*zoom;
//	rect.right	= pChord->m_rect.right-1 - lXOffset*zoom;
	rect.left = pChord->m_rect.left + cSize.cx + 1 - lXOffset*zoom;
	rect.right = pChord->m_rect.right - cSize.cx - 1 - lXOffset*zoom;
	rect.top		= pChord->m_rect.top + 1;
	rect.bottom	= rect.top - lf.lfHeight + 1;

	// indicate whether or not chord is orphan
	if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END))
	{
		if(pChord->m_dwflags & CE_MATCHED)
		{
			lf.lfWeight = 700;
		}
		else
		{
			lf.lfItalic = TRUE;
		}
	}

	CFont font;
	if (font.CreateFontIndirect( &lf ))
	{
		CFont *pOldFont;
		pOldFont = cDC.SelectObject( &font );
		if (pOldFont)
		{
			// 
			int nOldBkMode;
			nOldBkMode = cDC.SetBkMode( TRANSPARENT );
			::DrawText(cDC, csChordName1, csChordName1.GetLength(), &rect, 
						DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_CENTER | /*DT_WORDBREAK */ DT_SINGLELINE);
			if(!csChordName2.IsEmpty())
			{
				rect.top = rect.bottom + 1;
				rect.bottom = rect.top - lf.lfHeight + 1;
				::DrawText(cDC, csChordName2, csChordName2.GetLength(), &rect, 
						DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);
			}
			
			cDC.SetBkMode( nOldBkMode );
			cDC.SelectObject( pOldFont );
		}
	}



	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::SetNewChordXY( POINTS& DropPoint )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ChordEntry*		pChord;

	// Find Selected Chord in ChordList...
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			pChord != NULL;
			pChord = pChord->GetNext() )
	{
		if( pChord->m_chordsel.KeyDown() == TRUE )
		{
			break;
		}
	}

	if( pChord == NULL )
	{
		return E_FAIL;
	}

	// Update the XY of the Chord, measure and beat members too.
	SetChordXY( pChord, DropPoint );

	// Indicate that we are no longer dragging this Chord.
	pChord->m_chordsel.KeyDown() = FALSE;

	// Update min/max for Connections that exit from this Chord.


	return S_OK;
}

void CChordMapStrip::ClearSelectedConnections() 
{

	ASSERT( m_pChordMapMgr != NULL );
	ASSERT( m_pChordMapMgr->m_pChordMapList != NULL );

	ChordEntry*	pChord		= NULL;
	NextChord*	pConnection	= NULL;

	// Go through the Chords in the ChordList and check each connection for a hit.
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			(pChord != NULL) && (pConnection == NULL);
			pChord = pChord->GetNext() )
	{
		// Go through Connections looking for a hit
		for(	pConnection = pChord->m_nextchordlist.GetHead();
				pConnection != NULL;
				pConnection = pConnection->GetNext() )
		{
				SetSelectedConnection(NULL);
		}
	}

	
}


HRESULT STDMETHODCALLTYPE CChordMapStrip::GetClickedConnection( POINTS& Point, ChordEntry** ppChord, ChordEntry** ppNextChord )
// modified to return only if there is a connection if ppChord && ppNextChord are null pointers
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pChordMapMgr != NULL );
	ASSERT( m_pChordMapMgr->m_pChordMapList != NULL );
//	ASSERT( ppChord != NULL );

	ChordEntry*	pChord		= NULL;
	NextChord*	pConnection	= NULL;

//	*ppChord = NULL;

	// Go through the Chords in the ChordList and check each connection for a hit.
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			(pChord != NULL) && (pConnection == NULL);
			pChord = pChord->GetNext() )
	{
		// Go through Connections looking for a hit
		for(	pConnection = pChord->m_nextchordlist.GetHead();
				pConnection != NULL;
				pConnection = pConnection->GetNext() )
		{
			if( (Point.x >= pConnection->m_rect.left && Point.x <= pConnection->m_rect.right) &&
				(Point.y >= pConnection->m_rect.top && Point.y <= pConnection->m_rect.bottom) )
			{
				SetSelectedConnection(pConnection);
				break; // Found a hit.
			}
		}
	}

	// Didn't find a hit.
	if( (pChord == NULL) && (pConnection == NULL) )
	{
		return E_FAIL;
	}

	// Otherwise the hit was a regular 'ol Connection box.
	if(ppChord)
		(*ppChord) = pConnection->m_parent;
	if(ppNextChord)
		(*ppNextChord) = pConnection->m_nextchord;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ClearSelectedChords()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ChordEntry* pChord = NULL;

	// Go through the Chords in the ChordList and check each connection for a hit.
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			pChord != NULL;
			pChord = pChord->GetNext() )
	{
		if( pChord->m_chordsel.KeyDown() == TRUE )
		{
			pChord->m_chordsel.KeyDown() = FALSE;
		}
	}

	// now clear out chord palette
	for(int i = 0; i < 24; i++)
	{
		m_pChordMapMgr->m_pChordPalette->m_chords[i].KeyDown() = false;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::GetNumSelections( int* pnSelected )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	ChordEntry* pChord = NULL;
	int nSelected = 0;

	// Go through the Chords in the ChordList and check each connection for a hit.
	for(	pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
			pChord != NULL;
			pChord = pChord->GetNext() )
	{
		if( pChord->m_chordsel.KeyDown() == TRUE )
		{
			nSelected++;
		}
	}		

	// Default behavior of GetNumSelections is to just return S_FALSE if no Chords are selected.
	if( pnSelected != NULL )
	{
		(*pnSelected) = nSelected;
	}

	return (nSelected == 0) ? S_FALSE : S_OK;
}


// kludge because draw not happening before engine data sync
HRESULT STDMETHODCALLTYPE CChordMapStrip::ComputeChordMap()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pChordMapMgr->m_pTimeline )
	{
		ChordEntry*	pChord;
		RECT		rcChord;


		// Draw Chords in ChordMap Pane
		if( (pChord = m_pChordMapMgr->m_pChordMapList->GetHead()) != NULL )
		{
			while( pChord ) //( ( pChord ) && (lPosition < rcScreenClip.right) )
			{
				rcChord.left = pChord->m_rect.left;
				rcChord.right = pChord->m_rect.right;
				rcChord.top = pChord->m_rect.top;
				rcChord.bottom = pChord->m_rect.bottom;

				// Display Chord Box.
				ComputeChord(pChord );
					
				ComputeConnectionBoxes(pChord );
					
				pChord = pChord->GetNext();
			}
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ComputeChord(ChordEntry* pChord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pChord != NULL );

	if( pChord->m_rect.left < 0 )
	{
		pChord->m_rect.left = 0;
		pChord->m_rect.right = CHORD_DEFAULT_WIDTH;
	}
	if( pChord->m_rect.top < 0 )
	{
		pChord->m_rect.top = 0;
		pChord->m_rect.bottom = CHORD_DEFAULT_HEIGHT;
	}

	// Calculate Chord rect based on measure and beat.
	SyncRectToMeasureBeat( pChord );

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::ComputeConnectionBoxes(ChordEntry* pChord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pChord != NULL );

	RECT		rect;	// Bounding rectangle used to draw Boxes.
	int			nNumConnections;
	NextChord*	pConnection;

	// Draw Chord Connection boxes in front of Chords.  Including blank one for adding.
	if( CountConnections( pChord, &nNumConnections ) == S_FALSE )
	{
		return S_OK; // This is an ending Chord and should not have connection boxes.
	}

	// Increment the number of connections so we can account for a blank 'Add' connection box.

	rect.bottom	=	(pChord->m_rect.top + ((pChord->m_rect.bottom - pChord->m_rect.top)>>1)) -
					(((CONNECTION_HEIGHT>>1)*nNumConnections)+nNumConnections);
	rect.left	= pChord->m_rect.right + 1;
	rect.right	= rect.left + CONNECTION_WIDTH;

	// Set next Connection ptr so we can set the text for connections 
	// right after we draw the boxes for them.  
	pConnection = pChord->m_nextchordlist.GetHead();

	// Set text color for Connection box text and restore before method ends
	VARIANT		vt;
	vt.vt = VT_I4;


	while( pConnection )
	{
////////////////
// Draw the Connection boxes
//
		rect.top = rect.bottom;
		rect.bottom += CONNECTION_HEIGHT+1;

		// If the top connection box is heigher then the top of the screen, then make the
		// top connection box = 0 and bump the bottom connection box down the amount
		// of where we adjusted for the top.
		if( rect.top < 0 )
		{
			rect.bottom += -(rect.top);
			rect.top = 0;
		}

		// Save the Connection box coordinates for mouse-hit checking.
		pConnection->m_rect.left	= (short)(rect.left);
		pConnection->m_rect.top		= (short)(rect.top);
		pConnection->m_rect.right	= (short)(rect.right);
		pConnection->m_rect.bottom	= (short)(rect.bottom);


		// If this connection connects two chords then show the weight and min/max data
		// for the connection.  Otherwise it's a blank 'Add' Connection box.
		if( pConnection->m_nextchord != NULL )
		{

////////////////
// Update min/max depending on the distance to the next chord.
//
			long nC1Pos, nC2Pos;
			long nC1Clocks, nC2Clocks;
			m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
															  pChord->m_chordsel.Measure(), 
															  pChord->m_chordsel.Beat(), 
															  &nC1Clocks );
			m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
															  pConnection->m_nextchord->m_chordsel.Measure(),
															  pConnection->m_nextchord->m_chordsel.Beat(), 
															  &nC2Clocks );
			long nDeltaC1C2 = ( nC1Clocks > nC2Clocks ) ? nC1Clocks - nC2Clocks 
															: nC2Clocks - nC1Clocks;

			m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, nDeltaC1C2, &nC1Pos, &nC2Pos );

			long bpm;
/*
			DMUS_TIMESIGNATURE timesig;
			m_pChordMapMgr->m_pTimeline->GetParam(GUID_TimeSignature, 0xFFFFFFFF, 0, 0, 0, static_cast<void*>(&timesig));
			nC2Pos += timesig.bBeatsPerMeasure*nC1Pos;
*/
			m_pChordMapMgr->GetTimeSig(&bpm, 0);
			nC2Pos += bpm*nC1Pos;
			if(!m_fVariableNotFixed)
			{
				pConnection->m_nminbeats = (short)nC2Pos;
				pConnection->m_nmaxbeats = (short)nC2Pos;
			}
		
			
		}

		pConnection = pConnection->GetNext();
	}

	return S_OK;
}


BOOL CChordMapStrip::ProcessKey(int nVirtKey, unsigned long lKeyData)
{
	BOOL b = FALSE;
	if(IsDeleteKey(nVirtKey, lKeyData))
	{
		if(m_pChordMapMgr->m_pSelectedConnection && m_pChordMapMgr->m_pSelectedConnection->m_nextchord)
		{
			m_pChordMapMgr->SendEditNotification(IDS_UNDO_RemoveConnection);
			DelSelConnections();
//			DeleteConnection(m_pChordMapMgr->m_pSelectedConnection);
//			m_pChordMapMgr->m_pSelectedConnection = NULL;
//			m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
//			DoPropertyPage(m_pChordMapMgr->m_selection);
			b = TRUE;
		}
		else if(m_pChordMapMgr->m_selection == CChordMapMgr::ChordSelected 
				|| m_pChordMapMgr->m_selection == CChordMapMgr::MultipleSelections)
		{
			DelSelChords();
			m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
			b = TRUE;
		}
	}
	else if (IsCutKey(nVirtKey, lKeyData))
	{
	}
	return b;
}

///////////////////////////////////////////////// DropTarget helpers

CWnd* CChordMapStrip::GetTimelineCWnd()
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	CWnd* pWnd = 0;

	// Get the DC of our Strip
	if( m_pChordMapMgr->m_pTimeline )
	{
		if( SUCCEEDED(m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt )) )
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

HRESULT CChordMapStrip::CanPasteFromData(IDataObject* pIDataObject)
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordList ) )
		|| SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfChordMapList ) ) )
	{
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}

///////////////////////////////////////////////////////// IDropTarget methods
// IDropTarget CChordMapStrip::DragEnter

HRESULT CChordMapStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));


	TRACE("\nCChordMapStrip Drag Enter!");
	ASSERT( pIDataObject != NULL );
	if(pIDataObject == NULL)
		return E_UNEXPECTED;

	if(m_pITargetDataObject != NULL)
	{
		TRACE("releasing m_pITargetDataObject\r\n");
		m_pITargetDataObject->Release();
		m_pITargetDataObject = 0;
	}

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

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	// save boundary information about chord aggregate being dragged
	
	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pJazzDataObject = new CDllJazzDataObject();
	if( pJazzDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	IStream* pIStream;
	if(SUCCEEDED (pJazzDataObject->AttemptRead( pIDataObject, m_cfChordMapList, &pIStream)))
	{
			ChordEntryList list;
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			// declare a temp list and write chords into it
			DMSubChordSet::m_sbDisableMessages = true;
			hr = DMLoadChordData(list, pIStream);
			DMSubChordSet::m_sbDisableMessages = false;
			ChordEntry* pRefChord = list.GetHead();
			if(pRefChord)
			{
				GetBoundingRectRelativeToChord(list, pRefChord, m_rectAggregate);
			}
			// reset everything
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			pIStream->Release();
			list.ClearList();
	}
	else
	{
		memset(&m_rectAggregate, 0, sizeof(RECT));
	}

	pJazzDataObject->Release();

	
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
// IDropTarget CChordMapStrip::DragOver

HRESULT CChordMapStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	int height;

	HRESULT hr = S_OK;

//	TRACE("CChordMapStrip Drag to %x\n", pt.y);
	if(m_pITargetDataObject == NULL)
		ASSERT( m_pITargetDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}


	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	VARIANT var;
	hr = m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty(this, STP_HEIGHT, &var);
	if(hr != S_OK)
	{
		goto Leave;
	}
	height = V_I4(&var);
	// adjust for vertical scroll
//	height += GetVerticalScroll();

	if( pt.y > (height) )
	{
		// prevent chord from getting swallowed up by the abyss
		hr = E_FAIL;
		goto Leave;
	}
/*
	if(CheckBoundsForDrag(pt) != S_OK)	
	{
		hr = E_FAIL;
	}
*/
	if( CanPasteFromData( m_pITargetDataObject ) == S_OK && (CheckBoundsForDrag(pt) == S_OK))
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
Leave:
	*pdwEffect = dwEffect;
//	TRACE("pdwEffect = %d\r\n", dwEffect);
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CChordMapStrip::DragLeave

HRESULT CChordMapStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	TRACE("CChordMapStrip Drag Leave!");

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
// IDropTarget CChordMapStrip::Drop

HRESULT CChordMapStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
		// BUGBUG: Error messages?
		IDMUSProdTimelineDataObject *pITimelineDataObject;
		if( SUCCEEDED( m_pChordMapMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
		{
			if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
			{
				if(pt.x < 0)
				{
					hr = FBPasteAt( pITimelineDataObject, pt, true);
				}
				else
				{
					if( S_OK == pITimelineDataObject->IsClipFormatAvailable(m_cfChordMapList) )
					{
						hr = ChordEntryPasteAt(pITimelineDataObject, pt, true);
					}
					else if(S_OK == pITimelineDataObject->IsClipFormatAvailable(m_cfChordList))
					{
						hr = ChordItemPasteAt(pITimelineDataObject, pt, true);
					}
					else
					{
						ASSERT(FALSE);
						hr = E_UNEXPECTED;
					}
				}
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

HRESULT CChordMapStrip::InsertChords(CChordList& list, POINTL pt)
{
//	ChordEntry pChordChange = 0;
	HRESULT hr = S_OK;
	VARIANT			var;
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_MAXIMUM_HEIGHT, &var);
	int strip_height = V_I4(&var);
	short y = static_cast<short>(pt.y);
	for(CChordItem* pItem = list.GetHead(); pItem; pItem = pItem->GetNext())
	{
		ChordEntry* pChord = NULL;
		hr = m_pChordMapMgr->CreateBlankChord(&pChord);
		if(SUCCEEDED(hr))
		{
			pChord->LoadFromChordItem(*pItem);
			// we need to regenerate this because LoadFromChordItem does not preserve nid
			((SmallChordEntry*)pChord)->m_nid = m_pChordMapMgr->m_nIDCounter++;
			// Set new Chord (UI Box) where the user clicked the mouse.
			long position;
			// need to preserve measure (mapping to position distorts it)
			short measure = pChord->m_chordsel.Measure();
			BYTE beat = pChord->m_chordsel.Beat();
			m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMapMgr->m_dwGroupBits, 0,
																pChord->m_chordsel.Measure(),
																pChord->m_chordsel.Beat(),
																&position);

			POINTS ptNewPoint;
			ptNewPoint.x = static_cast<short>(position);
			ptNewPoint.y = y;
			y += 20;
			if(y > strip_height)
			{
				y = 0;
			}
			if( FAILED(SetChordXY( pChord, ptNewPoint )) ) // If user clicks end of timeline then method should fail (it currently does not).
			{
				delete pChord;
				break;
			}

			pChord->m_chordsel.Measure() = measure;
			pChord->m_chordsel.Beat() = beat;
			m_pChordMapMgr->m_pChordMapList->AddTail( pChord );
			ClearSelectedChords();
			ClearSelectedConnections();
			pChord->m_chordsel.KeyDown() = TRUE;	// new inserted chord is selected
//			pChordChange = pChord;					// notify client of selected chord
			m_fSelected = TRUE;
			m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
		}
		else
		{
			ASSERT(FALSE);
			break;
		}
	}
	return hr;
}


HRESULT CChordMapStrip::NormalizeChordList(CChordList& list, long beatOffset, 
									long* pFirstMeasure, long* pFirstBeat,
									long* pLastMeasure, long* pLastBeat)
//
// normalize using strip's timesig, also returns min/max measure/beat of list if user asks for it
//
{
	long bpm;
	HRESULT hr = E_FAIL;
	long minBeats = 0x7fffffff;
	long maxBeats = 0;
	long totalBeats;
	long beatLength;
	CChordItem* pChord = 0;
	if(SUCCEEDED(m_pChordMapMgr->GetTimeSig(&bpm, 0)))
	{
		hr = S_OK;
		for(pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
		{
			totalBeats = pChord->Measure()*bpm + pChord->Beat();
			if(totalBeats < minBeats)
				minBeats = totalBeats;
			if(totalBeats > maxBeats)
				maxBeats = totalBeats;
		}
		
		VARIANT var;
		m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
		long m,b;
		m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat(0xFFFFFFFF, 0, V_I4(&var), &m, &b);
		beatLength = m*bpm + b;
		if( (maxBeats + beatOffset) > beatLength)
		{
			CString msg;
			msg.LoadString(IDS_PASTE_TOOLONG);
			AfxMessageBox(msg);
			hr = E_FAIL;
		}
		else
		{
			for(pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
			{
				totalBeats = pChord->Measure()*bpm + pChord->Beat() - minBeats + beatOffset;
				pChord->Measure() = short(totalBeats / bpm);
				pChord->Beat() = char(totalBeats - (pChord->Measure() * bpm));
			}
			if(pFirstMeasure)
				*pFirstMeasure = minBeats/bpm;
			if(pFirstBeat)
				*pFirstBeat = minBeats - (minBeats/bpm)*bpm;
			if(pLastMeasure)
				*pLastMeasure = maxBeats/bpm;
			if(pLastBeat)
				*pLastBeat = maxBeats - (maxBeats/bpm)*bpm;
		}
	}
	return hr;
}


HRESULT CChordMapStrip::NormalizeChordList(ChordEntryList& list, long beatOffset,
									long* pFirstMeasure, long* pFirstBeat,
									long* pLastMeasure, long* pLastBeat)
//
// normalize using strip's timesig, also returns min/max measure/beat of list if user asks for it
//
{
	long bpm;
	HRESULT hr = E_FAIL;
	long minBeats = 0x7fffffff;
	long maxBeats = 0;
	long totalBeats;
	long beatLengthMap;
	long beatLengthList;
	ChordEntry* pChord;
	if(SUCCEEDED(m_pChordMapMgr->GetTimeSig(&bpm, 0)))
	{
		hr = S_OK;
		for(pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
		{
			totalBeats = pChord->m_chordsel.Measure()*bpm + pChord->m_chordsel.Beat();
			if(totalBeats < minBeats)
				minBeats = totalBeats;
			if(totalBeats > maxBeats)
				maxBeats = totalBeats;
		}

		VARIANT var;
		m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
		long m,b;
		m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat(0xFFFFFFFF, 0, V_I4(&var), &m, &b);
		beatLengthMap = m*bpm + b;
		beatLengthList = maxBeats - minBeats;

		// check not needed
		if(false && (beatLengthList + beatOffset) > beatLengthMap)
		{
			CString msg;
			msg.LoadString(IDS_PASTE_TOOLONG);
			AfxMessageBox(msg);
			hr = E_FAIL;
		}
		else
		{
			long excessBeats = 0;
			for(pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
			{
				totalBeats = pChord->m_chordsel.Measure()*bpm + pChord->m_chordsel.Beat() 
								 + beatOffset;
				if((totalBeats - beatLengthMap) >= 0)
				{
					excessBeats = (totalBeats-beatLengthMap+1);
				}
				pChord->m_chordsel.Measure() = short(totalBeats / bpm);
				pChord->m_chordsel.Beat() = char(totalBeats - (pChord->m_chordsel.Measure() * bpm));
			}

			if(excessBeats > 0)
			{
				for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
				{
					long beats = pChord->m_chordsel.Measure()*bpm + pChord->m_chordsel.Beat();
					beats -= excessBeats;
					pChord->m_chordsel.Measure() = (short)(beats / bpm);
					pChord->m_chordsel.Beat() = (BYTE)(beats  %  bpm);
				}
			}

			if(pFirstMeasure)
				*pFirstMeasure = minBeats/bpm;
			if(pFirstBeat)
				*pFirstBeat = minBeats - (minBeats/bpm)*bpm;
			if(pLastMeasure)
				*pLastMeasure = maxBeats/bpm;
			if(pLastBeat)
				*pLastBeat = maxBeats - (maxBeats/bpm)*bpm;
		}
	}
	return hr;
}

HRESULT CChordMapStrip::ChordEntryPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, 
										  POINTL pt, bool bDropNotEditPaste)
{
	ChordEntry* pRefChord = NULL;	// chord at drag point
	bool bOrphanCheck = false;
	if( pITimelineDataObject == NULL )
	{
		return E_NOTIMPL;
	}

	HRESULT hr = E_FAIL;

	if( pITimelineDataObject->IsClipFormatAvailable( m_cfChordMapList ) == S_OK )
	{
		IStream* pIStream;
		if(SUCCEEDED (pITimelineDataObject->AttemptRead( m_cfChordMapList, &pIStream)))
		{
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			// declare a temp list and write chords into it
			ChordEntryList list;
			hr = DMLoadChordData(list, pIStream);
			pIStream->Release();
			if(list.GetHead() == NULL)
			{
				// don't drop an empty list
				hr = E_FAIL;
				goto Leave;
			}
			else if(bDropNotEditPaste)
			{
				// this is normally set in drag over, we need to make sure its set for paste as well
				ChordEntry* pRefChord = list.GetHead();
				if(pRefChord)
				{
					GetBoundingRectRelativeToChord(list, pRefChord, m_rectAggregate);
				}
			}
			// update times from position of drop
			
			if( m_pChordMapMgr->m_pTimeline )
			{
				// get offset of first chord's drag position to normalize all chords to offset zero:
				long beatDragOffset ;
				// convert drop postition to beats
				long position = pt.x;
				long beatDropOffset;
				long firstBeat = 0, lastBeat = 0;
				long firstMeasure = 0, lastMeasure = 0;
				long m,b;
				long timesigBPM, offset;
				bool bFromPalette = m_startDragPosition.x < 0;
				if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, position, &m, &b)))
				{
					// convert measure/beat to beats in the timesig of the target
					m_pChordMapMgr->GetTimeSig(&timesigBPM, 0);
					if(bDropNotEditPaste)
					{
						beatDropOffset = m*timesigBPM + b;
						// check that we've actually "moved" somewhere.  If not cancel the drop
						if(m_pISourceDataObject == m_pITargetDataObject && !bFromPalette)
						{
							// we're in our strip, check if we've moved
							long deltapos = abs(pt.x - m_startDragPosition.x);
							long m,b;
							m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, deltapos, &m, &b);
							if(m == 0 && b < 1 && (abs(pt.y - m_startDragPosition.y) < 2))
							{
								hr = E_FAIL;
							}
							else if(m_dwOverDragEffect == DROPEFFECT_MOVE)
							{
								// save drop point -- need to let source thread handle rest of it
								// so we can do different things on a move or copy
								if(ContainsSignpost(list))
								{
									// adjust drop point so it is integral number of measures from
									// drag point
									long clocks, clocksPerMeasure;
									
									deltapos = pt.x - m_startDragPosition.x;
									PositionToClocks(deltapos, clocks);
									clocksPerMeasure = ClocksPerMeasure();
									if(clocks >= 0)
									{
										clocks = (clocks / clocksPerMeasure) * clocksPerMeasure;
									}
									else
									{
										clocks = ( (clocks/clocksPerMeasure) - 1 ) * clocksPerMeasure;
									}
									ClocksToPosition(clocks, deltapos);
									pt.x = m_startDragPosition.x + deltapos + 1;

									if(pt.x < 0)
									{
										// oops went too far, correct.
										ClocksToPosition(clocksPerMeasure, deltapos);
										pt.x += deltapos;
									}
									
								}
								m_ptDrop.x = static_cast<short>(pt.x);
								m_ptDrop.y = static_cast<short>(pt.y);
								m_bInternalDrop = true;
								hr = S_OK;
//								InternalDrop(ptDrop);
							}
							else
							{
								m_bInternalDrop = false;	
							}
							if(!(hr==S_OK) || m_bInternalDrop)
							{
								list.ClearList();
								goto Leave;
							}
						}
						// offset measure/beat in chords to be drop
						// first find selected chord
						ChordEntry* pChord = NULL;
						bool bContainsSignpost = false;
						long beatFirstChord = 0;
						pRefChord = 0;
						ChordEntry* pFirstChord = NULL;
						for(pChord = list.GetHead(); pChord; pChord = pChord->GetNext()) 
						{
							// find chord where mouse is
							if(pChord->m_chordsel.KeyDown())
							{
								pRefChord = pChord;
							}
							// got signposts?
							if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END) )
							{
								bContainsSignpost = true;
							}
							// find earliest chord
							if(pFirstChord == 0)
							{
								pFirstChord = pChord;
							}
							else if(pChord->m_chordsel.Measure() < pFirstChord->m_chordsel.Measure())
							{
								pFirstChord = pChord;
							}
							else if(pChord->m_chordsel.Measure() == pFirstChord->m_chordsel.Measure()
									&& pChord->m_chordsel.Beat() < pFirstChord->m_chordsel.Beat())
							{
								pFirstChord = pChord;
							}
						}
						if(pRefChord)
						{
							// this is selected chord: use it as the drag offset
							// convert its measure/beat to beats
							beatDragOffset = pRefChord->m_chordsel.Measure()*timesigBPM + pRefChord->m_chordsel.Beat();
						}
						else
						{
							// no chord selected, list came from somewhere else, dragoffset = 0
							beatDragOffset = 0;
						}
						offset = beatDropOffset - beatDragOffset;
						if(bContainsSignpost)
						{
							// adjust offset to an integral number of beats
							if( (offset < 0) && ( (-offset % timesigBPM) != 0))
							{
								offset = offset - (timesigBPM - (-offset)%timesigBPM);
							}
							else if((offset % timesigBPM) != 0)
							{
								offset = offset - offset%timesigBPM;
							}
							// make sure no negative chords, since pRefChord is first chord,
							// this means when(pRefChord) + offset <= 0 while maintaining
							// offset % timesigBPM
							beatFirstChord = pFirstChord->m_chordsel.Measure()*timesigBPM + pFirstChord->m_chordsel.Beat();
							while( (beatFirstChord + offset) < 0)
							{
								offset += timesigBPM;
							}
						}
					}
					else
					{
						bool bContainsSignpost = false;
						// paste, need refchord to be earliest chord
						pRefChord = 0;
						for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
						{
							if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END) )
							{
								bContainsSignpost = true;
							}
							if(pRefChord == 0)
							{
								pRefChord = pChord;
							}
							else if(pChord->m_chordsel.Measure() < pRefChord->m_chordsel.Measure())
							{
								pRefChord = pChord;
							}
							else if(pChord->m_chordsel.Measure() == pRefChord->m_chordsel.Measure()
									&& pChord->m_chordsel.Beat() < pRefChord->m_chordsel.Beat())
							{
								pRefChord = pChord;
							}
						}
						beatDropOffset = m*timesigBPM + b;
						beatDragOffset = pRefChord->m_chordsel.Measure()*timesigBPM + pRefChord->m_chordsel.Beat();
						offset = beatDropOffset - beatDragOffset;
						if(bContainsSignpost)
						{
							// adjust offset to an integral number of beats
							if( (offset < 0) && ( (-offset % timesigBPM) != 0))
							{
								offset = offset - (timesigBPM - (-offset)%timesigBPM);
							}
							else if((offset % timesigBPM) != 0)
							{
								offset = offset - offset%timesigBPM;
							}
							// make sure no negative chords, since pRefChord is first chord,
							// this means when(pRefChord) + offset <= 0 while maintaining
							// offset % timesigBPM
							while( (beatDragOffset + offset) < 0)
							{
								offset += timesigBPM;
							}
						}
					}
					hr = NormalizeChordList(list, offset, &firstMeasure, &firstBeat, &lastMeasure, &lastBeat);
					if(hr != S_OK)
					{
						list.ClearList();
						goto Leave;
					}
					m_pChordMapMgr->SendEditNotification(IDS_UNDO_PasteChord);
					UnMarkChords(list, CE_DRAGSELECT);
					m_pChordMapMgr->m_pTimeline->Refresh();
					hr = MergeChords(list, pt, pRefChord, bDropNotEditPaste);
					ResolveConnections();
					for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
					{
						if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END))
						{
							bOrphanCheck = true;
						}
					}
					list.ClearList();

					m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					if(bOrphanCheck)
					{
						m_pChordMapMgr->m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
					}
					DoPropertyPage(m_pChordMapMgr->m_selection);
/*
					if( m_pChordMapMgr->m_pPropPageMgr )
					{
						m_pChordMapMgr->m_pPropPageMgr->RefreshData();
					}
*/
					m_pChordMapMgr->m_pTimeline->OnDataChanged(0);
					m_pChordMapMgr->m_pTimeline->Refresh();

				}
				else
				{
					ASSERT(0);
				}
			}
			else
			{
				list.ClearList();
			}
		}
	}

Leave:	
	return hr;
	
}

HRESULT CChordMapStrip::ChordItemPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste)
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
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			// declare a temp list and write chords into it
			CChordList list;
			list.Load(pIStream);
			pIStream->Release();
			if(list.GetHead() == NULL)
			{
				// don't drop an empty list
				hr = E_FAIL;
				goto Leave;
			}
			// update times from position of drop
			
			if( m_pChordMapMgr->m_pTimeline )
			{
				// get offset of first chord's drag position to normalize all chords to offset zero:
				long beatDragOffset ;
				// convert drop postition to beats
				long position = pt.x;
				long beatDropOffset;
				long firstBeat = 0, lastBeat = 0;
				long firstMeasure = 0, lastMeasure = 0;
				long m,b;
				long timesigBPM, offset;
				bool bFromPalette = m_startDragPosition.x < 0;
				if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, position, &m, &b)))
				{
					// convert measure/beat to beats in the timesig of the target
					m_pChordMapMgr->GetTimeSig(&timesigBPM, 0);
					if(bDropNotEditPaste)
					{
						beatDropOffset = m*timesigBPM + b;
						// check that we've actually "moved" somewhere.  If not cancel the drop
						if(m_pISourceDataObject == m_pITargetDataObject && !bFromPalette)
						{
							// we're in our strip, check if we've moved
							long deltapos = abs(pt.x - m_startDragPosition.x);
							long m,b;
							m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, deltapos, &m, &b);
							if(m == 0 && b < 1 && (abs(pt.y - m_startDragPosition.y) < 2))
							{
								hr = E_FAIL;
							}
							else
							{
								// save drop point -- need to let source thread handle rest of it
								// so we can do different things on a move or copy
								m_ptDrop.x = static_cast<short>(pt.x);
								m_ptDrop.y = static_cast<short>(pt.y);
								m_bInternalDrop = true;
								hr = S_OK;
//								InternalDrop(ptDrop);
							}
							list.ClearList();
							goto Leave;
						}
						// offset measure/beat in chords to be drop
						// first find selected chord
						CChordItem* pChord = NULL;
						for(pChord = list.GetHead(); pChord && !pChord->IsSelected(); pChord = pChord->GetNext()) 
						{
							continue;
						}
						BOOL bNotFromPalette = true;
						if(pChord && bNotFromPalette)
						{
							// this is selected chord: use it as the drag offset
							// convert its measure/beat to beats
							beatDragOffset = pChord->Measure()*timesigBPM + pChord->Beat();
							// if this is from the signpost list, then no drop offset, chords go
							// at beginning and end if signpost, at end if cadence
//							if(pChord->IsSignPost())
//							{
//								beatDropOffset = 0;
//							}
						}
						else
						{
							// no chord selected, list came from somewhere else, dragoffset = 0
							beatDragOffset = 0;
						}
						offset = beatDropOffset - beatDragOffset;
						if(offset < 0)
						{
							offset = 0;
						}
					}
					else
					{
						offset = m*timesigBPM + b;
					}
					hr = NormalizeChordList(list, offset, &firstMeasure, &firstBeat, &lastMeasure, &lastBeat);
					if(hr != S_OK)
					{
						list.ClearList();
						goto Leave;
					}
					m_pChordMapMgr->SendEditNotification(IDS_UNDO_PasteChord);
					hr = InsertChords(list, pt);
					ResolveConnections();
					// if any of chords are signpost, link chords
					for(CChordItem* pChord = list.GetHead(); pChord ; pChord = pChord->GetNext())
					{
						if(pChord->IsSignPost())
						{
							m_pChordMapMgr->m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
							break;
						}
					}

					

					list.ClearList();



					m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					DoPropertyPage(m_pChordMapMgr->m_selection);
					
					/*
					if( m_pChordMapMgr->m_pPropPageMgr )
					{
						m_pChordMapMgr->m_pPropPageMgr->RefreshData();
					}
					*/

					m_pChordMapMgr->m_pTimeline->OnDataChanged(0);
					m_pChordMapMgr->m_pTimeline->Refresh();

				}
				else
				{
					ASSERT(0);
				}
			}
			else
			{
				list.ClearList();
			}
		}
	}

Leave:	
	return hr;
	
}



HRESULT CChordMapStrip::FBPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste)
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
			LARGE_INTEGER liTemp;
			// seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			// declare a temp list and write chords into it
			CChordList list;
			list.Load(pIStream);
			pIStream->Release();
			if(list.GetHead() == NULL)
			{
				// don't drop an empty list
				hr = E_FAIL;
				goto Leave;
			}
			// update times from position of drop	
			POINTS pts;
			pts.x = static_cast<short>(pt.x);
			pts.y = static_cast<short>(pt.y);

			long index = PaletteChordIndexFromCoords(pts);

			bool bFromPalette = m_startDragPosition.x < 0;

			if(bFromPalette && m_startDragPosition.x == pt.x)
			{
				// we're dropping on where we started, cancel paste at
				hr = E_FAIL;
				list.ClearList();
				goto Leave;
			}
			
			// put chord in palette
			m_pChordMapMgr->SendEditNotification(IDS_UNDO_PastePalette);
			ChordEntry ce;
			ce.LoadFromChordItem(*list.GetHead());
			ChordSelection& chordsel = m_pChordMapMgr->m_pChordPalette->Chord(index);
			dynamic_cast<DMPolyChord&>(chordsel) = ce.m_chordsel;
			list.ClearList();
			// convert root to match postion in palette
			chordsel.Base()->ChordRoot() = static_cast<unsigned char>(index);
			hr = S_OK;
			// select only dropped chord
			ClearPaletteSelections();
			chordsel.KeyDown() = true;
//			m_pChordMapMgr->m_pTimeline->OnDataChanged(0);
			m_pChordMapMgr->m_pTimeline->Refresh();

		}
	}
Leave:	
	return hr;
	
}

// IDropSource Methods
HRESULT CChordMapStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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

HRESULT CChordMapStrip::GiveFeedback( DWORD dwEffect )
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

CImageList* CChordMapStrip::CreateDragImage()
{
	CImageList* pimage = new CImageList;
	ASSERT(pimage);
	if(!pimage)
	{
		return 0;
	}
	pimage->Create(::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON),
					ILC_COLOR4 | ILC_MASK, 1,1);
	pimage->SetBkColor(CLR_NONE);
	HICON hIcon = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CHORDTRUCK));
	pimage->Add(hIcon);
	return pimage;
}

HRESULT	CChordMapStrip::CreateDataObject(IDataObject** ppIDataObject, POINTS where)
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

	if( SUCCEEDED ( m_pChordMapMgr->m_pJazzFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		ChordEntry* pChordAtDragPoint;
		hr = GetClickedChord(where, &pChordAtDragPoint);
		ASSERT(pChordAtDragPoint);
		m_pDragChord = pChordAtDragPoint;
		TRACE("drag chord created, addr = %p\n");
		if(!SUCCEEDED(hr))
		{
			ASSERT(FALSE);
			pIStream->Release();
			return hr;
		}
		// mark the chords as being dragged: this used later for deleting chords in drag move
		MarkSelectedChords(CE_DRAGSELECT);
//		pChordAtDragPoint = FirstSelectedChord();
		if( SUCCEEDED ( SaveSelectedChords( pIStream, pChordAtDragPoint, TRUE ) ) )
		{
			// Place CF_CHORDLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfChordMapList, pIStream ) ) )
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


// create data object from chord palette
HRESULT	CChordMapStrip::CreateDataObject(IDataObject** ppIDataObject, int index)
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

	if( SUCCEEDED ( m_pChordMapMgr->m_pJazzFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		ChordEntry* pChordAtDragPoint = new ChordEntry;
		ASSERT(pChordAtDragPoint);
		if(!pChordAtDragPoint)
		{
			hr = E_FAIL;
			pIStream->Release();
			return hr;
		}
		CChordItem* pChordItem = new CChordItem(m_pChordMapMgr->m_pChordPalette->Chord(index));
		ASSERT(pChordItem);
		if(!pChordItem)
		{
			hr = E_FAIL;
			pIStream->Release();
			return hr;
		}
		pChordItem->Measure() = 0;
		pChordItem->Beat() = 0;
		pChordAtDragPoint->LoadFromChordItem(*pChordItem);
		m_pDragChord = pChordAtDragPoint;
		CChordList list;
		list.AddTail(pChordItem);
		TRACE("drag chord created, addr = %p\n");
		hr = list.Save(pIStream, FALSE);
		list.ClearList();
		if( SUCCEEDED(hr))
		{
			// Place CF_CHORDLIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_cfChordList, pIStream ) ) )
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


ChordEntry* CChordMapStrip::FirstSelectedChord()
{
	ChordEntry* pFoundChord = NULL;
	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown())
		{
			pFoundChord = pChord;
			break;
		}
	}
	return pFoundChord;
}

void CChordMapStrip::MarkSelectedChords(DWORD flags)
{
	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown())
			pChord->m_dwflags |= flags;
	}
}

HRESULT CChordMapStrip::SaveSelectedChordItems(LPSTREAM pStream, ChordEntry* pChordAtDragPoint, BOOL bNormalize)
{
	ASSERT(pStream);
	ASSERT(pChordAtDragPoint);
	CChordList list;
	ChordEntry* pChord;
	CChordItem* pItem;
	long offset;
	// copy selected chords to list
	for(pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown())
		{
			// make sure that only the chord at the mouse drag point is selected
//			pChord->m_chordsel.KeyDown() = (pChord == pChordAtDragPoint) ? TRUE : FALSE;
			list.AddTail(new CChordItem(*pChord));
			// re-select chord
//			pChord->m_chordsel.KeyDown() = TRUE;
		}

	}
	// check that anything is selected
	if(!list.GetHead())
		return E_FAIL;
	// normalize chords so that first chord is at offset 0
	if(bNormalize)
	{
		pItem = list.GetHead();
		long bpm;
		m_pChordMapMgr->GetTimeSig(&bpm, 0);
		offset = pItem->Measure()*bpm + pItem->Beat();
		NormalizeChordList(list, -offset);

	}
	HRESULT hr = list.Save(pStream, FALSE, FT_DESIGN);
	list.ClearList();
	return hr;
}

HRESULT CChordMapStrip::SaveSelectedChords(LPSTREAM pStream, ChordEntry* pChordAtDragPoint, BOOL bNormalize)
{
	ASSERT(pStream);
	ASSERT(pChordAtDragPoint);
	ChordEntryList list;
	ChordEntry* pChord;
	long offset;
	// add drag chord to head of list
	pChord = new ChordEntry(*pChordAtDragPoint);
	pChord->CopyNextList(*pChordAtDragPoint, TRUE);
	pChord->m_chordsel.KeyDown() = TRUE;
	list.AddHead(pChord);
	// copy selected chords to list
	for(pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown() && pChord != pChordAtDragPoint)
		{
			// make new copy of chord
			ChordEntry* pNewChord = new ChordEntry(*pChord);
			pNewChord->CopyNextList(*pChord, TRUE);
			pNewChord->m_chordsel.KeyDown() = FALSE;
			list.AddTail(pNewChord);
		}
	}
	// check that anything is selected
	if(!list.GetHead())
		return E_FAIL;
	// normalize chords so that first chord is at offset 0
	if(bNormalize)
	{
		pChord = list.GetHead();
		long bpm;
		m_pChordMapMgr->GetTimeSig(&bpm, 0);
		offset = pChord->m_chordsel.Measure()*bpm + pChord->m_chordsel.Beat();
//		NormalizeChordList(list, -offset);

	}
	
	HRESULT hr = DMSaveChordData(list, pStream, true);
	list.ClearList();
	return hr;
}



void CChordMapStrip::DeleteMarkedChords(DWORD flags)
{
	for(ChordEntry *pChord = m_pChordMapMgr->m_pChordMapList->GetHead();pChord != NULL;	)
	{
		if( !(pChord->m_dwflags & flags) )
		{
			// chord not selected, continue
			pChord = pChord->GetNext();
			continue;
		}
		// Delete the Chord and get the next chord in the list.
		int nOldID = pChord->m_nid;
		m_pChordMapMgr->m_pChordMapList->Remove(pChord);
		delete pChord;
		pChord = m_pChordMapMgr->m_pChordMapList->GetHead();
		
		// Remove all Connections to this Chord ID
		DeleteAllConnections( nOldID );

		// Resolve connections
		ResolveConnections();
	}
}

void CChordMapStrip::UnMarkChords(DWORD flags)
{
	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		pChord->m_dwflags &= ~flags;
	}
}

void CChordMapStrip::UnMarkChords(ChordEntryList& list, DWORD flags)
{
	for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
	{
		pChord->m_dwflags &= ~flags;
	}
}


/* old single chord version
void CChordMapStrip::InternalMove(POINTS ptDrop)
{
	ASSERT(m_pDragChord);
	if( m_pDragChord)
	{
		// Move the Chord.
		ClearSelectedChords();
		ClearSelectedConnections();
		m_pDragChord->m_chordsel.KeyDown() = TRUE;
		m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
		SetNewChordXY( ptDrop );

		m_fSelected = FALSE;
		m_fDragging = FALSE;

		// need this since SetNewChordXY clears selection
		m_pDragChord->m_chordsel.KeyDown() = TRUE;


	}
}
*/

POINTS operator+(POINTS pt1, POINTS pt2)
{
	POINTS pt;
	pt.x = pt1.x + pt2.x;
	pt.y = pt1.y + pt2.y;
	return pt;
}
POINTS operator-(POINTS pt1, POINTS pt2)
{
	POINTS pt;
	pt.x = pt1.x - pt2.x;
	pt.y = pt1.y - pt2.y;
	return pt;
}

POINTS GetChordTopLeft(ChordEntry* pChord)
{
	POINTS pt;
	pt.x = pChord->m_rect.left;
	pt.y = pChord->m_rect.top;
	return pt;
}
void CChordMapStrip::InternalMove(POINTS ptDrop)
{
	ASSERT(m_pDragChord);
	if(!m_pDragChord)
		return;

	// get needed timeline props
	VARIANT			var;
	int strip_height = GetStripHeight();
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
	int clocks = V_I4(&var);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var );	
	short vpos = (short)V_I4(&var);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var );	
	short hpos = (short)V_I4(&var);
	long strip_width;
	m_pChordMapMgr->m_pTimeline->ClocksToPosition(clocks, &strip_width);
	

	// need later to check right boundary
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
	clocks = V_I4(&var);
	long lengthInMeasures, lengthInBeats, timesigBPM;
	m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat(m_pChordMapMgr->m_dwGroupBits, 0,
									clocks,
									&lengthInMeasures, &lengthInBeats);
	m_pChordMapMgr->GetTimeSig(&timesigBPM, 0);
	lengthInBeats = lengthInMeasures*timesigBPM + lengthInBeats;

	// get bounding rect and translate its top left courner to drop/past point
	RECT rect;
	GetBoundingRectOfSelectedChords(rect);

	short xtrans = (short)(ptDrop.x - m_startDragPosition.x);
	short ytrans = (short)(ptDrop.y - m_startDragPosition.y);

	// translate all selected (dropped) chords
	long excessBeats = 0;
	long underflow = 0;
	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown() == TRUE)
		{
			POINTS ptNewPoint;
			ptNewPoint.x = pChord->m_rect.left + xtrans;
			ptNewPoint.y = pChord->m_rect.top + ytrans;
			
			SetChordXY(pChord,	ptNewPoint);
			pChord->m_chordsel.KeyDown() = TRUE;

			long beats = pChord->m_chordsel.Measure()*timesigBPM + pChord->m_chordsel.Beat();
			if((beats-lengthInBeats) >= excessBeats)
			{
				excessBeats = (beats-lengthInBeats+1);
			}
			if(beats < 0 && beats < underflow)
			{
				underflow = beats;
			}
		}
	}

	if( /*!SelectedChordsContainsSignpost() && */(excessBeats > 0 || underflow < 0) )
	{
		if(SelectedChordsContainsSignpost())
		{
			long bpm;
			long beat;
			m_pChordMapMgr->GetTimeSig(&bpm, &beat);
			excessBeats = excessBeats > 0 ? excessBeats + bpm - (excessBeats % bpm) : 0;
			underflow = underflow < 0 ? underflow - (bpm - ((-underflow) % bpm)) : 0;
		}
		for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
		{
			if(pChord->m_chordsel.KeyDown() == TRUE)
			{
				long beats = pChord->m_chordsel.Measure()*timesigBPM + pChord->m_chordsel.Beat();
				beats -= excessBeats;
				beats -= underflow;
				pChord->m_chordsel.Measure() = (short)(beats / timesigBPM);
				pChord->m_chordsel.Beat() = (BYTE)(beats % timesigBPM);
			}
		}
	}

	m_fDragging = FALSE;
}


ChordEntry* CChordMapStrip::InternalCopy(POINTS ptDrop)
{
	ChordEntry *pChord = 0;

	pChord = new ChordEntry(*m_pDragChord);

	NextChord* pNextChord = new NextChord;
	if( pNextChord == NULL )
	{
		delete pChord;
		return 0;
	}

	pNextChord->m_parent = pChord;
	pChord->m_nextchordlist.AddTail(pNextChord);

	ZeroMemory( &(pChord->m_rect), sizeof(pChord->m_rect) );
	ZeroMemory( &(pChord->m_lastrect), sizeof(pChord->m_lastrect) );

	((SmallChordEntry*)pChord)->m_nid = m_pChordMapMgr->m_nIDCounter++;

	// Set new Chord (UI Box) where the user clicked the mouse.
	if( FAILED(SetChordXY( pChord, ptDrop )) ) // If user clicks end of timeline then method should fail (it currently does not).
	{
		delete pChord;
		pChord = 0;
	}

	m_pChordMapMgr->m_pChordMapList->AddTail( pChord );
	ClearSelectedChords();
	pChord->m_chordsel.KeyDown() = TRUE;	// new inserted chord is selected
	return pChord;
}



/////////////////////////////////////////////////////////////////////////////
// CChordMapStrip IDMUSProdTimelineEdit

HRESULT CChordMapStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
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

HRESULT CChordMapStrip::Copy( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamCopy;

	if(m_pChordMapMgr->m_selection == CChordMapMgr::PaletteSelected)
	{
		return Copy(pITimelineDataObject, m_pChordMapMgr->m_nSelectedIndex);
	}

	hr = CanCopy();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordMapList == 0 )
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfChordList == 0)
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0)
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected commands in.
//	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );
	hr = m_pChordMapMgr->m_pJazzFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pStreamCopy);
	
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// Save the commands into the stream.
	MarkSelectedChords(CE_DRAGSELECT);
	// make sure chord at cursor is first
	ChordEntry* pChordAtDragPoint = 0;
	hr = GetClickedChord(m_ptLastMouse, &pChordAtDragPoint);
	if(FAILED(hr) || pChordAtDragPoint==0)
	{
		pChordAtDragPoint = FirstSelectedChord();
	}
	hr = SaveSelectedChords( pStreamCopy, pChordAtDragPoint, TRUE );
	if( FAILED( hr ))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		// This only works for chords from chordmap, not for those from palette
		hr = pITimelineDataObject->AddExternalClipFormat( m_cfChordMapList, pStreamCopy );
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
		hr = m_pChordMapMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lStartTime, lEndTime;
		m_pChordMapMgr->GetBoundariesOfSelectedChords(lStartTime, lEndTime);
		hr = pITimelineDataObject->SetBoundaries(lStartTime, lEndTime);

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddExternalClipFormat( m_cfChordMapList, pStreamCopy );
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
		if(m_pChordMapMgr->m_pCopyDataObject)
		{
			m_pChordMapMgr->m_pCopyDataObject->Release();
		}

		// set m_pCopyDataObject to the object we just copied to the clipboard
		m_pChordMapMgr->m_pCopyDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pCopyDataObject->AddRef
	}

	return hr;
}

HRESULT CChordMapStrip::Copy( IDMUSProdTimelineDataObject *pITimelineDataObject, int index )
//
// Copy Palette Chord
//
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;
	IStream*			pStreamCopy;
	CChordItem*			pChordItem;
	CChordList			list;

	bool guard = ( index >=0 && index <= 23 );
	ASSERT(guard);
	if( !guard )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if( m_cfChordMapList == 0 )
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfChordList == 0)
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0)
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected commands in.
//	hr = CreateStreamOnHGlobal( NULL, TRUE, &pStreamCopy );
	hr = m_pChordMapMgr->m_pJazzFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pStreamCopy);
	
	if( FAILED( hr ))
	{
		return E_OUTOFMEMORY;
	}

	// get palette chord
	pChordItem = new CChordItem(m_pChordMapMgr->m_pChordPalette->Chord(index));
	ASSERT(pChordItem);
	if(!pChordItem)
	{
		return E_OUTOFMEMORY;
	}
	pChordItem->Measure() = 0;
	pChordItem->Beat() = 0;

	list.AddTail(pChordItem);
	hr = list.Save(pStreamCopy, FALSE);
	list.ClearList();

	if(hr != S_OK)
	{
		return E_FAIL;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		// This only works for chords from chordmap, not for those from palette
		hr = pITimelineDataObject->AddExternalClipFormat( m_cfChordMapList, pStreamCopy );
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
		hr = m_pChordMapMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long clocksPerBeat;
		m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks(m_pChordMapMgr->m_dwGroupBits, 0, 0, 1, &clocksPerBeat);
		hr = pITimelineDataObject->SetBoundaries(0, clocksPerBeat);

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddExternalClipFormat( m_cfChordList, pStreamCopy );
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
		if(m_pChordMapMgr->m_pCopyDataObject)
		{
			m_pChordMapMgr->m_pCopyDataObject->Release();
		}

		// set m_pCopyDataObject to the object we just copied to the clipboard
		m_pChordMapMgr->m_pCopyDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pCopyDataObject->AddRef
	}

	return hr;
}



HRESULT CChordMapStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT				hr;
	
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr->m_pTimeline != NULL );
	if( m_pChordMapMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Make sure everything on the timeline is deselected.
	// JHD 8/5/98: Why is this here?
	/*
	m_bSelecting = TRUE;
	m_pChordMapMgr->m_pTimeline->SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
	m_pChordMapMgr->m_pTimeline->SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
	m_bSelecting = FALSE;
	*/
	
	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			return E_FAIL;
		}
	}
	if( m_cfChordMapList == 0 )
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
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
		hr = m_pChordMapMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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
	pt.x = m_ptLastMouse.x;
	pt.y = m_ptLastMouse.y;

	// do we have enough room to paste?
	long lStart, lEnd, lTime, lLength, mm, bb;
	VARIANT vtInit;
	vtInit.vt = VT_I4;

	pITimelineDataObject->GetBoundaries(&lStart, &lEnd);
	m_pChordMapMgr->m_pTimeline->PositionToClocks(pt.x, &lTime);
	// round time down to nearest beat
	m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat(m_pChordMapMgr->m_dwGroupBits, 0, lTime, &mm, &bb);
	m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks(m_pChordMapMgr->m_dwGroupBits, 0, mm, bb, &lTime);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &vtInit );
	lLength = V_I4(&vtInit);

	if( (lTime + lEnd - lStart) > lLength) 
	{
		AfxMessageBox(IDS_MAPTOOSHORT);
		hr = E_FAIL;
	}
	else
	{
		if( S_OK == pITimelineDataObject->IsClipFormatAvailable(m_cfChordMapList) )
		{
			hr = ChordEntryPasteAt(pITimelineDataObject, pt, false);
		}
		else
		{
			hr = ChordItemPasteAt(pITimelineDataObject, pt, false);
		}
	}
	
	if(SUCCEEDED(hr))
	{
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	}
	pITimelineDataObject->Release();

	return hr;
}

HRESULT CChordMapStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	ChordEntry* pChord = NULL;
	hr = m_pChordMapMgr->CreateBlankChord(&pChord);
	if(SUCCEEDED(hr))
	{
		hr = SetChordXY( pChord, m_ptLastMouse);
		if(SUCCEEDED(hr))
		{
			ClearSelectedChords();
			ClearSelectedConnections();
			pChord->m_chordsel.KeyDown() = TRUE;
			m_pChordMapMgr->SendEditNotification(IDS_INSERT);
			m_pChordMapMgr->m_pChordMapList->AddTail(pChord);
			m_fSelected = TRUE;
			m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
			DoPropertyPage(CChordMapMgr::ChordSelected);
			TRACE( "ChordMapStrip: Syncing w/ Engine\n" );
			ComputeChordMap();
			ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
			// this is roundabout way to copy chord, but what the hey, we need something the client can delete
			ChordEntry* pc = new ChordEntry;
			CPropChord prop;
			m_pChordMapMgr->ChordToPropChord(&prop, pChord);
			m_pChordMapMgr->PropChordToChord(pc, &prop);
			pCallback->SetChordEntry(pc);
			m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );
			m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		}
		else
		{
			delete pChord;
		}
	}
	return hr;
}

HRESULT CChordMapStrip::Delete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr->m_pTimeline != NULL );
	if( m_pChordMapMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pChordMapMgr->m_selection == CChordMapMgr::ChordSelected 
	||  m_pChordMapMgr->m_selection == CChordMapMgr::MultipleSelections)
	{
		DelSelChords();
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pChordMapMgr->m_pTimeline->OnDataChanged( NULL );
		m_pChordMapMgr->m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
	}
	else if( m_pChordMapMgr->m_selection == CChordMapMgr::ConnectionSelected )
	{
		DelSelConnections();
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
		m_pChordMapMgr->m_pTimeline->OnDataChanged( NULL );
	}

	return S_OK;
}

HRESULT CChordMapStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pChordMapMgr->m_pTimeline != NULL );
	if( m_pChordMapMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		pChord->m_chordsel.KeyDown() = true;
	}


	

	m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
	m_pChordMapMgr->m_selection = CChordMapMgr::MultipleSelections;


	m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );
	if( m_pChordMapMgr->m_pChordPropPageMgr != NULL )
	{
		m_pChordMapMgr->m_pChordPropPageMgr->RefreshData();
	}
	m_pChordMapMgr->OnShowProperties();
	
	return S_OK;
}

HRESULT CChordMapStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
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

HRESULT CChordMapStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	if(m_pChordMapMgr->m_selection == CChordMapMgr::PaletteSelected)
	{
		return S_OK;
	}
	else
	{
		return GetNumSelections();
	}
}

HRESULT CChordMapStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT				hr;

	ASSERT( m_pChordMapMgr != NULL );
	if( m_pChordMapMgr == NULL )
	{
		return E_UNEXPECTED;
	}
/*
	// Something must be selected so paste knows where to put the data.
	if( !m_pCommandMgr->m_bSelected )
	{
		return S_FALSE;
	}
*/
	// If the format hasn't been registered yet, do it now.
	if( m_cfChordList == 0 )
	{
		m_cfChordList = RegisterClipboardFormat( CF_CHORDLIST );
		if( m_cfChordList == 0 )
		{
			return E_FAIL;
		}
	}

	if( m_cfChordMapList == 0 )
	{
		m_cfChordMapList = RegisterClipboardFormat( CF_CHORDMAP_LIST );
		if( m_cfChordMapList == 0 )
		{
			return E_FAIL;
		}
	}
	// If pITimelineDataObject != NULL, check it.
	if( pITimelineDataObject != NULL )
	{
		hr = pITimelineDataObject->IsClipFormatAvailable( m_cfChordList );
		if(hr != S_OK)
		{
			hr = pITimelineDataObject->IsClipFormatAvailable( m_cfChordMapList );
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
			if( SUCCEEDED( m_pChordMapMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable(m_cfChordList);
					if(hr != S_OK)
					{
						hr = pITimelineDataObject->IsClipFormatAvailable( m_cfChordMapList );
					}
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

HRESULT CChordMapStrip::CanInsert( void )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
//	HRESULT hr = GetClickedChord( m_ptLastMouse, NULL );
//	return hr == S_OK ? S_OK : S_FALSE;
	if(m_pChordMapMgr->m_selection != CChordMapMgr::PaletteSelected)
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CChordMapStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( GetNumSelections() == S_OK )
	{
		return S_OK;
	}

	if( m_pChordMapMgr->m_selection == CChordMapMgr::ConnectionSelected 
	&&  m_pChordMapMgr->m_pSelectedConnection->m_nextchord )
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CChordMapStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return m_pChordMapMgr->m_pChordMapList->IsEmpty() ? S_FALSE : S_OK;
}

void CChordMapStrip::SetFocus()
{
	IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
	m_pChordMapMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
	if (pIOleInPlaceObjectWindowless)
	{
		HWND hwnd;
		if (pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK)
		{
			::SetFocus(hwnd);
		}
		pIOleInPlaceObjectWindowless->Release();
	}
}



/////////////////////////////////// misc helpers
BOOL CChordMapStrip::ToggleSignPost(int BegOrEnd, POINTS where)
{
	BOOL bResult = TRUE;
	ChordEntry* pChord;
	if(SUCCEEDED(GetClickedChord(where, &pChord)))
	{
		m_pChordMapMgr->SendEditNotification(IDS_UNDO_ToggleSignPost);
		pChord->m_dwflags ^= (BegOrEnd == BegSignPost) ? CE_START : CE_END;
		// set beat to zero.  If chord turned into signpost this forces beat to zero as required
		// if chord was signpost and is now free, then beat was already zero so no harm done.
		pChord->m_chordsel.Beat() = 0;
		m_pChordMapMgr->m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}



/////////////////////////////////////////////////////////////////////////////
// CChordMapStrip IDMUSProdStripFunctionBar
HRESULT	STDMETHODCALLTYPE CChordMapStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	DrawFunctionBar( hDC, sv );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	ChordEntry* pChordChange = 0;
	ChordSelection* pChordsel = 0;
	POINTS ptMouse;
	int index;
	ptMouse = MAKEPOINTS( lParam );
	
	bool bRecallThisFunction = false;

	switch(nMsg)
	{
		case WM_MOUSEMOVE:
			if(m_bInSelectionNet)
			{
				// continuation of drag
				CWnd* pWnd = GetTimelineCWnd();
				HWND hwnd = pWnd->GetSafeHwnd();
				m_ptLastMouse = MAKEPOINTS(lParam);
				m_pChordMapMgr->HitTest(m_rectSelect);
				m_pChordMapMgr->m_pTimeline->Refresh();
				::InvalidateRect(hwnd, 0, TRUE);
				::UpdateWindow(hwnd);

				ComputeSelectionBoxOutline(m_ptAnchor, m_ptLastMouse);
			}
			break;
		case WM_LBUTTONUP:
			if(m_fCapture)
			{
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
				m_fCapture = false;
				if(m_bInSelectionNet)
				{
					int nsel;
					GetNumSelections(&nsel);
					if(nsel == 1)
						m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					else if(nsel > 1)
						m_pChordMapMgr->m_selection = CChordMapMgr::MultipleSelections;
					if(m_SelectionDragState.bAutoscroll)
					{
						::KillTimer(GetTimelineCWnd()->m_hWnd, SelectionDrag);
						TRACE("Killing Timer for SelectionDrag\n");
					}
					m_SelectionDragState.bActive = false;
					m_SelectionDragState.dir = DragState::off;
					m_bInSelectionNet = false;
					CWnd* pWnd = GetTimelineCWnd();
					HWND hwnd = pWnd->GetSafeHwnd();
					m_pChordMapMgr->m_pTimeline->Refresh();
					::InvalidateRect(hwnd, 0, TRUE);
					::UpdateWindow(hwnd);
				}
			}
			break;
		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
				case IDR_PALETTE_PROPS:
					{
						TRACE("Chordpalette properties selected\n");
						// have we selected a chord?
						int index = PaletteChordIndexFromCoords(m_ptLastMouse);
						if(index < 0)
							break;
						// yes, clear everything else and show this chord
						ClearSelectedChords();
						ClearSelectedConnections();
						pChordChange = new ChordEntry;
						pChordChange->LoadFromChordItem(CChordItem(m_pChordMapMgr->m_pChordPalette->Chord(index)));
						m_pChordMapMgr->m_pChordPalette->m_chords[index].KeyDown() = true;
						m_fSelected = TRUE;
						m_pChordMapMgr->m_selection = CChordMapMgr::PaletteSelected;
						pChordChange->SaveToPropChord(m_pChordMapMgr->m_SelectedChord);
						m_pChordMapMgr->m_SelectedChord.Undetermined(m_pChordMapMgr->m_SelectedChord.RootIndex()) |= UD_FROMCHORDPALETTE;
						m_pChordMapMgr->m_nSelectedIndex = index;
						DoPropertyPage(CChordMapMgr::PaletteSelected, TRUE ); // Make the PropertyPage display.
						ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
						pCallback->SetChordEntry(pChordChange);
						m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );
						m_pChordMapMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
						return S_OK;
					}
					break;
				case ID_PALETTE_COPY:
					{
						TRACE("Chordpalette copy selected\n");
						hr = Copy(0, m_pChordMapMgr->m_nSelectedIndex);
					}
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			// set focus to timeline
			// Set the focus to the timeline
			IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
			m_pChordMapMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
			if (pIOleInPlaceObjectWindowless)
			{
				HWND hwnd;
				if (pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK)
				{
					::SetFocus(hwnd);
				}
				pIOleInPlaceObjectWindowless->Release();
			}
			// have we selected a chord?
			index = PaletteChordIndexFromCoords(ptMouse);
			if(index < 0)
				break;

			pChordsel = &(m_pChordMapMgr->m_pChordPalette->Chord(index));
			if(pChordsel->KeyDown() == true)
			{
				// user is starting a drag
				m_fDragging = TRUE;
				m_bDrop = true;
				IDropSource* pIDropSource;
				DWORD dwEffect = DROPEFFECT_NONE;
				if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
				{
					hr = CreateDataObject(&m_pISourceDataObject, index);
					if(SUCCEEDED(hr))
					{
						m_pDragImage = CreateDragImage();
						if(m_pDragImage)
						{
							m_pDragImage->BeginDrag(0, CPoint(8,12));
						}
						// start drag drop
						DWORD dwOKDragEffects = DROPEFFECT_COPY;
						m_dwStartDragButton = (unsigned long)wParam;
						// store index and mark as being from chord palette
						m_startDragPosition.x = -(index+1);
						m_bInternalDrop = false;
						hr  = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);
						m_startDragPosition.x = 0;
						// we don't need this until we are a target so just delete it
						// remember to remove when we become a target
						delete m_pDragChord;
						m_pDragChord = 0;
						// drag drop completed, cleanup
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
							if(m_bInternalDrop)
							{
								m_bInternalDrop = false;
								ASSERT(FALSE);	// not supported yet
							}
							else if(dwEffect != DROPEFFECT_NONE)
							{
								// not an internal drop, don't need to do anything
								TRACE("Successful drag drop from chord palette\n");
							}
							else
							{
								m_bDrop = false;
							}
							break;
						default:
							m_bDrop=false;
							break;
						}
						hr = S_OK;
						m_pISourceDataObject->Release();
						m_pISourceDataObject = NULL;
					}
					pIDropSource->Release();
				}
				else
				{
					hr = E_FAIL;
					break;
				}
			}
			else
			{
				pChordChange = new ChordEntry();
				pChordChange->LoadFromChordItem(CChordItem(m_pChordMapMgr->m_pChordPalette->Chord(index)));
				bRecallThisFunction = true;
			}
			break;
		case WM_RBUTTONDOWN:
			// have we selected a chord?
			{
				index = PaletteChordIndexFromCoords(ptMouse);
				if(index < 0)
					break;
				pChordChange = new ChordEntry;
				pChordChange->LoadFromChordItem(CChordItem(m_pChordMapMgr->m_pChordPalette->Chord(index)));
				m_ptLastMouse = ptMouse;
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
				m_fCapture = true;
			}
			break;
		case WM_RBUTTONUP:
			{
				HMENU hMenu, hMenuPopup;
				POINT	 pt;
				m_ptLastMouse = ptMouse;

				if(m_fCapture)
				{
					VARIANT var;
					var.vt = VT_BOOL;
					V_BOOL(&var) = FALSE;
					m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
					m_fCapture = false;
				}
				// Load the SignPost RMB menu.
				hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_PALETTE_RMENU) );
				hMenuPopup = ::GetSubMenu(hMenu, 0);
				m_MenuHandler.m_pStrip = (IDMUSProdStrip *)this;
				GetCursorPos(&pt);
				m_pChordMapMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
			}
			break;
	}

	if(pChordChange)
	{
		ClearSelectedChords();
		ClearSelectedConnections();
		m_pChordMapMgr->m_pChordPalette->m_chords[index].KeyDown() = true;
		m_fSelected = TRUE;
//		m_pChordMapMgr->m_selection = CChordMapMgr::PaletteSelected;
		// next couple of lines assures that legacy palette chords will show accidentals correctly
		if(m_pChordMapMgr->m_dwKey & CChordMapMgr::UseFlats)
		{
			pChordChange->m_chordsel.SubChord(0)->UseFlat() = true;
			pChordChange->m_chordsel.PropagateUseFlat();
		}
		pChordChange->SaveToPropChord(m_pChordMapMgr->m_SelectedChord);
		m_pChordMapMgr->m_SelectedChord.Undetermined(m_pChordMapMgr->m_SelectedChord.RootIndex()) |= UD_FROMCHORDPALETTE;
		m_pChordMapMgr->m_nSelectedIndex = index;
		DoPropertyPage(CChordMapMgr::PaletteSelected ); // Make the PropertyPage display.
		ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
		pCallback->SetChordEntry(pChordChange);
		m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );
		m_pChordMapMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);
		if(bRecallThisFunction)
		{
			FBOnWMMessage(nMsg, wParam, lParam, lXPos, lYPos);
		}
	}
	return hr;
}

int CChordMapStrip::PaletteChordIndexFromCoords( POINTS pt)
{
	long lHeight;
	
	lHeight = enumDefaultHeight;
	lHeight -= FUNCTION_NAME_HEIGHT;
	

	if(pt.y < FUNCTION_NAME_HEIGHT)
		return -1;
	pt.y -= FUNCTION_NAME_HEIGHT;
	double keyheight = static_cast<double>(lHeight)/24.0;
	int index = static_cast<int>(floor(static_cast<double>(pt.y)/keyheight));
	index = 23 - index;
	if(index < 0) index = 0;
	return index;
}

void CChordMapStrip::DrawFunctionBar( HDC hDC, STRIPVIEW sv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( sv == SV_FUNCTIONBAR_NORMAL )
	{
		CRect	rectClip;
		RECT	rectNote;
		CDC		dc;
		short	value;
		long	lTop;
		long lWidth, lHeight;
//		static const char MidiValueToName[61] = "C    C#/DbD    D#/EbE    F    F#/GbG    G#/AbA    A#/BbB    ";
		static const char* NoteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G",
			"G#", "A", "A#", "B" };
		static const char* FlatNoteNames[12] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

		if( dc.Attach(hDC) )
		{
			lWidth = enumFBWidth;
			lHeight = enumDefaultHeight;
			CPoint point = dc.GetWindowOrg();
			point.y -= FUNCTION_NAME_HEIGHT;
			lHeight -= FUNCTION_NAME_HEIGHT;
			dc.SetWindowOrg( point );
			
			dc.GetClipBox( &rectClip );
			dc.FillSolidRect( rectClip, dc.GetNearestColor(GetSysColor(COLOR_WINDOW)));
			CPen	penNoteLine;
			if( !penNoteLine.CreatePen( PS_SOLID, 1, RGB(0,0,0) ))
			{
				dc.Detach();
				return;
			}
			short keyheight = lHeight / 24;
			for( value = 0; value < 24; value++ )
			{
				lTop = (long)(( 23 - value ) * ( lHeight ))/24;
				if( lTop > rectClip.bottom )
				{
					continue;
				}
				rectNote.top = lTop;
				rectNote.bottom = (long)(( 24 - value ) * ( lHeight ))/24;;
				if( rectNote.bottom < rectClip.top )
				{
					continue;
				}
				rectNote.left = rectClip.left;
				rectNote.right = rectClip.right;
				if(m_pChordMapMgr->m_pChordPalette->m_chords[value].KeyDown())
				{
					dc.FillSolidRect ( &rectNote, dc.GetNearestColor(PIANOROLL_SELKEY_COLOR));
				}
				else if( PianoKeyColor[ value % 12 ] == BLACK )
				{
					dc.FillSolidRect( &rectNote, dc.GetNearestColor(PIANOROLL_BLACKKEY_COLOR));
				}
				else
				{
					dc.FillSolidRect( &rectNote, dc.GetNearestColor(PIANOROLL_WHITEKEY_COLOR));
				}
				
				// draw chord names
				CPen*	ppenOld;
				ppenOld = dc.SelectObject( &penNoteLine );
				dc.MoveTo( rectNote.left, rectNote.bottom );
				dc.LineTo( rectNote.right, rectNote.bottom );
				dc.SelectObject( ppenOld );

				LOGFONT lf;
				memset( &lf, 0 , sizeof(LOGFONT));
//				lf.lfHeight = long(keyheight * 1.4 );
				lf.lfHeight = 100;
//				lfHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
				lf.lfWidth = 0;
				//lf.lfEscapement = 0;
				//lf.lfOrientation = 0;
				lf.lfWeight = 400;
				//lf.lfItalic = FALSE;
				//lf.lfUnderline = FALSE;
				//lf.lfStrikeOut = FALSE;
				//lf.lfCharSet = ANSI_CHARSET;
				//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
				//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				//lf.lfQuality = DEFAULT_QUALITY;
				lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
				strcpy(lf.lfFaceName,"Arial(Baltic)");
				
//				TEXTMETRIC tm;
				CFont font;


//				if (font.CreatePointFontIndirect( &lf, &dc))
				if (font.CreatePointFont( 100,"System", &dc))
				{
					CFont *pOldFont;
					pOldFont = dc.SelectObject( &font );
					if (pOldFont)
					{
						COLORREF oldColor;
						if(m_pChordMapMgr->m_pChordPalette->m_chords[value].KeyDown())
						{
							oldColor = dc.SetTextColor( RGB(255, 255, 255) );
						}
						else if( PianoKeyColor[ value % 12 ] == BLACK )
						{
							oldColor = dc.SetTextColor( RGB(255, 255, 255) );
						}
						else
						{
							oldColor = dc.SetTextColor( RGB(0, 0, 0) );
						}
//							oldColor = dc.SetTextColor( RGB(0, 0, 0) );
						
						int nOldBkMode;
						nOldBkMode = dc.SetBkMode( TRANSPARENT );
		
						CString cstrTxt;
						//cstrTxt.Format( "%s%d", CString(MidiValueToName).Mid((value % 12)*5, 5) , value/12 );
//							cstrTxt.Format( "C %d", value/12 );
/* don't need -- setting "use flats" on personality prop page sets flag in each chord
						if(m_pChordMapMgr->m_dwKey & m_pChordMapMgr->UseFlats)	
						{
							cstrTxt.Format("%s %s", 
											FlatNoteNames[value%12],
											m_pChordMapMgr->m_pChordPalette->Chord(value).Name());
						}
*/
						DMChord* pDMChord = m_pChordMapMgr->m_pChordPalette->Chord(value).Base();
						/* else */ if(m_pChordMapMgr->m_pChordPalette->Chord(value).Base()->UseFlat() == TRUE)
						{
							cstrTxt.Format("%s %s", 
											FlatNoteNames[value%12],
											m_pChordMapMgr->m_pChordPalette->Chord(value).Name());
						}
						else
						{
							cstrTxt.Format("%s %s", 
											NoteNames[value%12],
											m_pChordMapMgr->m_pChordPalette->Chord(value).Name());
						}
						rectNote.left = 0;
						dc.DrawText( cstrTxt, &rectNote,
							DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
						
						dc.SetBkMode( nOldBkMode );
						dc.SetTextColor( oldColor );
						dc.SelectObject( pOldFont );
					}
				}
			}
			penNoteLine.DeleteObject();
			dc.Detach();
		}
	}
}


void CChordMapStrip::ClearPaletteSelections()
{
	for(int idx = 0; idx < 24; idx++)
	{
		m_pChordMapMgr->m_pChordPalette->m_chords[idx].KeyDown() = false;
	}
}

double CChordMapStrip::ZoomFactor()
{
	double nominal = 24.0;	// 24 pixels per beat
	long m = 0;
	long b = 1;
	long pos;
	double factor = 1.0;
	if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMapMgr->m_dwGroupBits, 0, m,b, &pos)))
	{
		factor = double(pos)/nominal;
	}
	return factor;
}

long CChordMapStrip::LeftEdge()
//
// returns left edge of chordmap in strip coordinates (0 = 0 time)
//
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapMgr);
	ASSERT(m_pChordMapMgr->m_pTimeline);

	long pos;
	HRESULT hr;
	VARIANT var;
	if(SUCCEEDED(hr = m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var )))
	{
		pos = V_I4(&var);
	}
	else
	{
		pos = -1;
	}
	return pos;
}

long CChordMapStrip::RightEdge()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapMgr);
	ASSERT(m_pChordMapMgr->m_pTimeline);

	long pos;
	HRESULT hr = S_OK;
	VARIANT var;
	if(SUCCEEDED(hr = m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var )))
	{
		pos = V_I4(&var);
		if(SUCCEEDED(hr = m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty(this, STP_STRIP_RECT, &var )))
		{
			RECT* pRect = (RECT*)V_BYREF(&var);
			CRect rect(*pRect);
			pos += rect.Width();
			delete pRect;
		}
	}
	if(FAILED(hr))
	{
		pos = -1;
	}
	return pos;
}

HRESULT CChordMapStrip::ScrollHoriz(long pos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapMgr);
	ASSERT(m_pChordMapMgr->m_pTimeline);

	VARIANT var;
	var.vt = VT_I4;
	V_I4(&var) = pos;

	HRESULT hr = m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_HORIZONTAL_SCROLL, var);
	return hr;
}

HRESULT CChordMapStrip::ScrollVert(long pos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapMgr);
	ASSERT(m_pChordMapMgr->m_pTimeline);

	VARIANT var;
	var.vt = VT_I4;
	V_I4(&var) = pos;

	HRESULT hr = m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_VERTICAL_SCROLL, var);
	return hr;
}

HRESULT CChordMapStrip::InsertNewChord(POINTS ptWhere, ChordEntry*& pChord)
{
	HRESULT hr = S_OK;
	if( FAILED(m_pChordMapMgr->CreateBlankChord( &pChord )) )
	{
		hr = E_NOTIMPL;
		goto END;
	}

	// Set new Chord (UI Box) where the user clicked the mouse.
	if( FAILED(SetChordXY( pChord, ptWhere )) ) // If user clicks end of timeline then method should fail (it currently does not).
	{
		delete pChord;
		hr = E_FAIL;
		goto END;
	}

	pChord->SaveToPropChord(m_pChordMapMgr->m_SelectedChord);

	ClearSelectedChords();
	ClearSelectedConnections();
	pChord->m_chordsel.KeyDown() = TRUE;	// new inserted chord is selected
	m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
	m_pChordMapMgr->SendEditNotification(IDS_INSERT);
	m_pChordMapMgr->m_pChordMapList->AddTail( pChord );

	m_pChordMapMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );


END:
	return hr;
}


void CChordMapStrip::GetWindowOrg(long& xpos, long& ypos)
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;

	// Get the DC of our Strip					
	if( SUCCEEDED(m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_GET_HDC, &vt )) )
	{
		CWnd* pWnd = NULL;
		if( cDC.Attach( (HDC)(vt.lVal) ) != 0 )
		{
			pWnd = cDC.GetWindow();
			CPoint pt;
			pt = cDC.GetWindowOrg();
			xpos = pt.x;
			ypos = pt.y;
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

HRESULT CChordMapStrip::MergeChords(ChordEntryList &list, POINTL pt, ChordEntry* pRefChord, bool bDropNotPaste)
{
	HRESULT hr = S_OK;

	UNREFERENCED_PARAMETER(bDropNotPaste);

	// get needed timeline props
	VARIANT			var;
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_MAXIMUM_HEIGHT, &var);
	int strip_height = V_I4(&var);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
	int clocks = V_I4(&var);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var );	
	short vpos = (short)V_I4(&var);
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var );	
	short hpos = (short)V_I4(&var);
	long strip_width;
	m_pChordMapMgr->m_pTimeline->ClocksToPosition(clocks, &strip_width);
	
	// transform boundaries if scrolling
	strip_height += vpos;
	// strip_width is entire width of timeline, strip height is only the visible part (without scrolling)
//	strip_width += hpos;
	
	m_pChordMapMgr->MakeCompatibleNids(list);

	// get bounding rect and translate it's top left corner to drop/paste point
	RECT rect;
	GetBoundingRect(list, rect);
	short xtrans = pt.x - pRefChord->m_rect.left;
	short ytrans = pt.y - pRefChord->m_rect.top;

	short xExcess = 0, yExcess = 0;
	if((rect.top + ytrans) < 0)
		yExcess = -(rect.top + ytrans);	// yExcess >= 0
	if((rect.left + xtrans) < 0)
	{
		// should be impossible for paste: (pRectChord->m_rect.left == pt.x) > 0
		xExcess = -(rect.left + xtrans);  // xExcess >= 0
	}
	if((rect.bottom + ytrans)> strip_height)
	{
		yExcess = strip_height - (rect.bottom + ytrans); // yExcess <= 0
	}
	if((rect.right  + xtrans) > strip_width)
	{
		xExcess = strip_width - (rect.right + xtrans);  // xExcess <= 0;
	}

	for(ChordEntry* pItem = list.GetHead(); pItem; pItem = pItem->GetNext())
	{
		ChordEntry* pChord = new ChordEntry(*pItem);
		pChord->CopyNextList(*pItem, TRUE);
		POINTS ptNewPoint;
		ptNewPoint.x = pChord->m_rect.left + xtrans;
		ptNewPoint.y = pChord->m_rect.top  + ytrans;
		ptNewPoint.x += xExcess;
		ptNewPoint.y += yExcess;
		// preserve measure
		long lMeasure = pChord->m_chordsel.Measure();
		long lBeat = pChord->m_chordsel.Beat();
		if( FAILED(SetChordXY( pChord, ptNewPoint )) ) // If user clicks end of timeline then method should fail (it currently does not).
		{
			delete pChord;
			break;
		}
		pChord->m_chordsel.Measure() = (short)lMeasure;
		pChord->m_chordsel.Beat() = (BYTE)lBeat;
		m_pChordMapMgr->m_pChordMapList->AddTail( pChord );
		pChord->m_chordsel.KeyDown() = TRUE;	// new inserted chord is selected
//			pChordChange = pChord;					// notify client of selected chord
		m_fSelected = TRUE;
		m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
	}
	return hr;
}

BOOL CChordMapStrip::IntersectsChordInMap(CRect& rect)
{
	ChordEntry* pChord;
	CRect urect(0,0,0,0);
	for(pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		CRect chordrect(pChord->m_rect.left, pChord->m_rect.top, pChord->m_rect.right, pChord->m_rect.bottom);
		CRect irect;
		irect.IntersectRect(chordrect, rect);
		if(!irect.IsRectEmpty())
		{
			urect.UnionRect(urect, irect);
		}
	}
	return urect.IsRectEmpty();
}

void CChordMapStrip::FindYPositionForChord(short xpos, short& ypos, bool bUpdatePosition)
{

	// get candidate y pos
	//long ypos;
	/*
	VARIANT			var;
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_MAXIMUM_HEIGHT, &var);
	int strip_height = V_I4(&var);
	int nrow = strip_height/CHORD_DEFAULT_HEIGHT;
	*/
	int nrow = enumDefaultHeight/CHORD_DEFAULT_HEIGHT;
	BOOL fFoundPos = FALSE;
	int irow = m_irowSaved;	// previous position saved
	for(irow; !fFoundPos && irow < nrow; irow++)
	{
		ypos = irow * CHORD_DEFAULT_HEIGHT;
		CRect rect;
		rect.left			= (short)xpos;
		rect.right			= (short)xpos + CHORD_DEFAULT_WIDTH;
		rect.top			= (short)ypos;
		rect.bottom			= (short)ypos + CHORD_DEFAULT_HEIGHT;

		fFoundPos = IntersectsChordInMap(rect);
	}

	if(!fFoundPos)
	{
		ypos = ((m_irowSaved+1)%nrow) * CHORD_DEFAULT_HEIGHT;
	}

	if(bUpdatePosition)
	{
		m_irowSaved = (m_irowSaved + 1) % nrow;
	}
}

HRESULT CChordMapStrip::DragConnection(LPARAM lParam, POINTS ptMouseDown)
{
	CDC cDC;
	VARIANT vt;
	vt.vt = VT_I4;


	// Get the DC of our Strip					
	if( SUCCEEDED(m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_GET_HDC, &vt )) )
	{
		if( cDC.Attach( (HDC)(vt.lVal) ) != 0 )
		{
			// Figure out coordinates for connection line
			POINTS ptCurPos = MAKEPOINTS(lParam);
			short xOrg = ptMouseDown.x;
			short yOrg = ptMouseDown.y;
			short xTo = ptCurPos.x;
			short yTo = ptCurPos.y;
			long vpos, hpos;
			VARIANT var;
			if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var )))
			{
				hpos = V_I4(&var);
//				xOrg -= static_cast<short>(hpos);
				xTo -= static_cast<short>(hpos);
			}
			if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var )))
			{
				vpos = V_I4(&var);
//				yOrg -= static_cast<short>(vpos);
//				yTo -= static_cast<short>(vpos);
			}
			// end figure out coords

			// update ConnectionDragState
			m_ConnectionDragState.xOrg = xOrg;
			m_ConnectionDragState.yOrg = yOrg;
			m_ConnectionDragState.xTo = xTo;
			m_ConnectionDragState.yTo = yTo;

			RECT rtStrip;
			cDC.GetClipBox( &rtStrip );
			TRACE(" xTo: %d, yTo: %d, right edge: %d\r\n", xTo, yTo, rtStrip.right);

			// set up autoscroll
			if(xTo > rtStrip.right)
			{
//				TRACE("\nTurn on right Drag");
				m_ConnectionDragState.dir |= DragState::right;
			}
			else
			{
//				TRACE("\nTurn off right Drag");
				m_ConnectionDragState.dir &= ~DragState::right;
			}
			if(xTo < rtStrip.left + 3)
			{
//				TRACE("\nTurn on left Drag ");
//				TRACE("xTo=%d, rtStrip.left=%d, hpos=%d\n", xTo, rtStrip.left, hpos);
				m_ConnectionDragState.dir |= DragState::left;
			}
			else
			{
//				TRACE("\nTurn off left Drag");
				m_ConnectionDragState.dir &= ~DragState::left;
			}
			if(yTo < rtStrip.top)
			{
				m_ConnectionDragState.dir |= DragState::up;
			}
			else
			{
				m_ConnectionDragState.dir &= ~DragState::up;
			}
			if(yTo > rtStrip.bottom)
			{
				m_ConnectionDragState.dir |= DragState::down;
			}
			else
			{
				m_ConnectionDragState.dir &= ~DragState::down;
			}

			if(m_ConnectionDragState.dir != DragState::off
				&& !m_ConnectionDragState.bAutoscroll)
			{
				// turn autoscrolling on
				m_ConnectionDragState.bAutoscroll  = true;
				::SetTimer(GetTimelineCWnd()->m_hWnd, ConnectionDrag, 100, NULL);
			}
			else if(m_ConnectionDragState.dir == DragState::off
					&& m_ConnectionDragState.bAutoscroll)
			{
				// turn autoscrolling off
				m_ConnectionDragState.bAutoscroll = false;
				::KillTimer(GetTimelineCWnd()->m_hWnd, ConnectionDrag);
			}
					
			CWnd* pWnd = cDC.GetWindow();
			cDC.Detach();
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
	return S_OK;
}




void CChordMapStrip::UpdateAutoScroll(DragState* pDragState)
{
	long vpos, hpos;
	VARIANT var;
	if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var )))
	{
		hpos = V_I4(&var);
	}
	if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var )))
	{
		vpos = V_I4(&var);
	}

	if(pDragState->bAutoscroll)
	{
		if(pDragState->dir == DragState::left)
		{
			ScrollHoriz(hpos-12);
		}
		if(pDragState->dir == DragState::right)
		{
			ScrollHoriz(hpos+12);
		}
		if(pDragState->dir == DragState::up)
		{
			ScrollVert(vpos-12);
		}
		if(pDragState->dir == DragState::down)
		{
			ScrollVert(vpos+12);
		}
	}
}


HRESULT CChordMapStrip::OnCommand(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos,
								  BOOL bRefresh, BOOL bSyncEngine, ChordEntry*&	pChordChange)
{
	HRESULT hr = S_OK;
	ChordEntry	tempChord;
	switch( LOWORD(wParam) )
	{
		case ID_PALETTE_COPY:
		case IDR_PALETTE_PROPS:
			// reflect to FBOnWMMessage
			return FBOnWMMessage( nMsg, wParam, lParam, lXPos, lYPos );
			break;

		case IDM_BEGSIGNPOST:
			bRefresh = TRUE;
			bSyncEngine = TRUE;
			ToggleSignPost(BegSignPost, m_ptLastRightMouse);
			break;
		case IDM_ENDSIGNPOST:
			bRefresh = TRUE;
			bSyncEngine = TRUE;
			ToggleSignPost(EndSignPost, m_ptLastRightMouse);
			break;
		case ID_EDIT_CUT:
			bRefresh = TRUE;
			bSyncEngine = TRUE;
			m_ptLastMouse.x = m_ptLastRightMouse.x;
			m_ptLastMouse.y = m_ptLastRightMouse.y;
			m_pChordMapMgr->SendEditNotification(IDS_UNDO_CutChord);
			hr = Cut();
			break;
		case ID_EDIT_COPY:
			// make sure mouse pts are the same
			m_ptLastMouse.x = m_ptLastRightMouse.x;
			m_ptLastMouse.y = m_ptLastRightMouse.y;
			hr = Copy();
			break;
		case ID_EDIT_PASTE:
			bRefresh = TRUE;
			bSyncEngine = TRUE;
			hr = Paste();
			SetFocus();
	//					m_nLastEdit = IDS_PASTE; // deliberately commented out--PasteAt does this
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			bRefresh = TRUE;
			break;
		case IDM_CHORD_INSERT:
			{
				ChordEntry *pChord;
				POINTS		ptNewPoint = m_ptLastRightMouse;
				hr = InsertNewChord(ptNewPoint, pChord);
				if(hr == S_OK)
				{
					pChordChange = pChord;					// notify client of selected chord
					m_fSelected = TRUE;
					m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					DoPropertyPage(CChordMapMgr::ChordSelected);
					bRefresh = TRUE;
					bSyncEngine = TRUE;
				}
			}
			break;
		case IDM_CHORD_DELETE:
		case ID_TIMELINE_DELETE:
			{
				Delete();
			}
			break;
		case IDM_KILLPROPS:
			if( m_pChordMapMgr->m_pChordPropPageMgr && m_pChordMapMgr->IsPropPageShowing())
			{
				m_pChordMapMgr->ShowPropPage(FALSE);
			}
			break;
		case IDM_CLEARSELECT:
			m_fSelected = FALSE;
			m_pChordMapMgr->m_selection = CChordMapMgr::NoSelection;
			ClearPaletteSelections();
			ClearSelectedChords();
			ClearSelectedConnections();
			bRefresh = TRUE;
			m_pChordMapMgr->m_pTimeline->Refresh();
			m_pChordMapMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

			break;
		case IDM_PROPERTIES:
		case ID_TIMELINE_PROPERTIES:
			{
				// make sure that right clicked chord gets selected
				// so we do properties for the right chord
				ChordEntry* pChord;
				GetClickedChord(m_ptLastRightMouse, &pChord);
				if(!pChord)
				{
					if(m_pChordMapMgr->m_selection == CChordMapMgr::ChordSelected)
					{
						pChord = FirstSelectedChord();
						ASSERT(pChord);
						ClearSelectedChords();
						ClearSelectedConnections();
						pChord->m_chordsel.KeyDown() = TRUE;
						bRefresh = TRUE;
						bSyncEngine = TRUE;
						pChordChange = pChord;	// notify client of chord selected
						m_fSelected = TRUE;
						m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
						DoPropertyPage( CChordMapMgr::ChordSelected, TRUE ); // Make the PropertyPage display.
					}
					else if(m_pChordMapMgr->m_selection == CChordMapMgr::ConnectionSelected)
					{
						DoPropertyPage(CChordMapMgr::ConnectionSelected, TRUE);
					}
					else if(m_pChordMapMgr->m_selection == CChordMapMgr::PaletteSelected)
					{
						ClearSelectedChords();
						ClearSelectedConnections();
						m_pChordMapMgr->m_pChordPalette->m_chords[m_pChordMapMgr->m_nSelectedIndex].KeyDown() = true;

						tempChord.m_chordsel = m_pChordMapMgr->m_pChordPalette->Chord(m_pChordMapMgr->m_nSelectedIndex);
						bRefresh = TRUE;
						bSyncEngine = TRUE;
						m_fSelected = TRUE;
						tempChord.SaveToPropChord(m_pChordMapMgr->m_SelectedChord);
						pChordChange = &tempChord;
						m_pChordMapMgr->m_SelectedChord.Undetermined(m_pChordMapMgr->m_SelectedChord.RootIndex()) |= UD_FROMCHORDPALETTE;
						DoPropertyPage(CChordMapMgr::PaletteSelected, TRUE);
					}
					else if(m_pChordMapMgr->m_selection == CChordMapMgr::NoSelection
						|| m_pChordMapMgr->m_selection == CChordMapMgr::MultipleSelections)
					{
						m_pChordMapMgr->m_SelectedChord.RootIndex() = DMPolyChord::INVALID;
						DoPropertyPage(m_pChordMapMgr->m_selection);
					}
					else
					{
						ChordChangeCallback* pCallback = 
									new ChordChangeCallback(ChordChangeCallback::ParentShowProps);
						m_pChordMapMgr->m_pTimeline->OnDataChanged( pCallback );

						SetFocus();	// back to use
					}
				}
				else
				{
					ClearSelectedChords();
					ClearSelectedConnections();
					pChord->m_chordsel.KeyDown() = TRUE;
					bRefresh = TRUE;
					bSyncEngine = TRUE;
					pChordChange = pChord;	// notify client of chord selected
					m_fSelected = TRUE;
					m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
					DoPropertyPage( CChordMapMgr::ChordSelected, TRUE ); // Make the PropertyPage display.
				}
			}
			break;
	}
	return hr;
}

HRESULT CChordMapStrip::OnRButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	HRESULT hr = S_OK;
	if(m_fCapture)
	{
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
		m_fCapture = false;
	}
	HMENU hMenu, hMenuPopup;
	POINT	 pt;
	POINTS ptCur = MAKEPOINTS(lParam);
	ChordEntry* pChord;
	GetClickedChord(ptCur, &pChord);
	m_fSelected = (GetNumSelections() == S_OK);
	/* select logic down in RButtonDown
	if(m_fSelected)
	{
		ClearSelectedChords();
		ClearSelectedConnections();
		if(pChord)
			pChord->m_chordsel.KeyDown() = TRUE;
	}
	*/
	m_ptLastRightMouse = ptCur;	// save so we know where the chord is
	m_ptLastMouse = ptCur;

	// Load the SignPost RMB menu.
	hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_CHORD_RMENU) );
	hMenuPopup = ::GetSubMenu(hMenu, 0);
	BOOL fOldSelected = m_fSelected;
	if(GetNumSelections() == S_OK || (S_OK == GetClickedConnection(ptCur, 0, 0) ) )
	{
		// select chord to fool timeline menu enabler so that delete is an option
		m_fSelected = TRUE;
		::EnableMenuItem(hMenuPopup, IDM_CHORD_INSERT, MF_GRAYED);
	}				
	else
	{
//		::EnableMenuItem(hMenuPopup, IDM_CHORD_DELETE, MF_GRAYED);
		::EnableMenuItem(hMenuPopup, IDM_PROPERTIES, MF_GRAYED);
	}

	if(CanDelete() != S_OK)
	{
		::EnableMenuItem(hMenuPopup, IDM_CHORD_DELETE, MF_GRAYED);
	}

	if(CanCut() != S_OK)
	{
		::EnableMenuItem(hMenuPopup, ID_EDIT_CUT, MF_GRAYED);
	}
	if(CanCopy() != S_OK)
	{
		::EnableMenuItem(hMenuPopup, ID_EDIT_COPY, MF_GRAYED);
	}
	if(CanPaste() != S_OK)
	{
		::EnableMenuItem(hMenuPopup, ID_EDIT_PASTE, MF_GRAYED);
	}
	if(CanSelectAll() != S_OK)
	{
		::EnableMenuItem(hMenuPopup, ID_EDIT_SELECT_ALL, MF_GRAYED);
	}

	m_MenuHandler.m_pStrip = (IDMUSProdStrip *)this;
	GetCursorPos(&pt);
	m_pChordMapMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
	::DestroyMenu(hMenu);
	m_fSelected = fOldSelected;

	// make sure selection choices are updated
	m_pChordMapMgr->m_pTimeline->StripInvalidateRect(this, NULL, TRUE);

	return hr;
}


HRESULT CChordMapStrip::OnInsert(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos,
								  BOOL bRefresh, BOOL bSyncEngine, ChordEntry*&	pChordChange)
{
	ChordEntry *pChord;
	ChordSelection *pTemp;
	POINTS		ptNewPoint = {0, 0};
	CRect rect;
	long measure = 0;

	HRESULT hr = S_OK;

	if( FAILED(m_pChordMapMgr->CreateBlankChord( &pChord )) )
	{
		hr = E_NOTIMPL;
		goto Leave;
	}

	// Copy the allocated SignPost Chord into the new Beginning Chord.
	pTemp = (ChordSelection *)lParam;
	ASSERT( pTemp != NULL );  // means bug in SignPost list.

	memcpy( &(pChord->m_chordsel), pTemp, sizeof( pChord->m_chordsel ) );
	pChord->m_dwflags = (DWORD)(wParam);

	if(wParam & CE_END)
	{
		VARIANT vtInit;
		long clocks;
		long pos;
		vtInit.vt = VT_I4;
		m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &vtInit );
		clocks = V_I4(&vtInit);
		m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, clocks, &measure, 0);
		--measure;
		m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition( m_pChordMapMgr->m_dwGroupBits, 0, measure, 0, &pos);
		ptNewPoint.x = static_cast<short>(pos);
		FindYPositionForChord(ptNewPoint.x, ptNewPoint.y, true);
	}
	else
	{
		// only clear for beginning signpost (or cadence chords)
		// end signpost chords are usually inserted with beg signpost chords and we
		// want to highlight both
		ClearSelectedChords();
		ptNewPoint.x = 0;
		FindYPositionForChord(0, ptNewPoint.y, false);
	}

	// Set new Chord (UI Box) where the user clicked the mouse.
	if( FAILED(SetChordXY( pChord, ptNewPoint )) ) // If user clicks end of timeline then method should fail (it currently does not).
	{
		delete pChord;
		hr = E_FAIL;
		goto Leave;
	}
	// redo measure as timeline calcs sometimes screw it up
	pChord->m_chordsel.Measure() = (short)measure;
	m_pChordMapMgr->SendEditNotification(IDS_INSERT);
	m_pChordMapMgr->m_pChordMapList->AddTail( pChord );
	pChord->m_chordsel.KeyDown() = TRUE;
	bRefresh = TRUE;
	bSyncEngine = TRUE;
	pChordChange = pChord;	// notify client of selected chord
	hr = S_OK;

Leave:
	return hr;

}

void CChordMapStrip::SetFocusToTimeline()
{
	// set focus to timeline
			// Set the focus to the timeline
	IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
	m_pChordMapMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless );
	if (pIOleInPlaceObjectWindowless)
	{
		HWND hwnd;
		if (pIOleInPlaceObjectWindowless->GetWindow(&hwnd) == S_OK)
		{
			::SetFocus(hwnd);
		}
		pIOleInPlaceObjectWindowless->Release();
	}

	// notify parent dialog that we have focus
	if( m_pfnFocusCallback)
	{
		m_pfnFocusCallback(m_lFocusHint);
	}
}

HRESULT CChordMapStrip::DoDragDrop(WPARAM wParam, LPARAM lParam, ChordEntry* pChord, ChordEntry*& pChordChange)
{
	HRESULT hr = S_OK;
	// first cancel drag drop
	if(m_fCapture)
	{
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pChordMapMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
		m_fCapture = false;
	}
	///////////////////////////////
	// Drag Chord
	m_fDragging = TRUE;
	m_bDrop = false;
	pChord->m_chordsel.KeyDown() = TRUE; // select and drag at same time
	// make sure chord is on top
	m_pChordMapMgr->m_pChordMapList->Remove(pChord);
	pChord->SetNext(0);
	m_pChordMapMgr->m_pChordMapList->AddTail(pChord);
	m_pChordMapMgr->m_selection = CChordMapMgr::ChordSelected;
	m_fSelected = TRUE;
	pChordChange = pChord;	// notify client of chord selected
	if(pChord->m_chordsel.KeyDown())	// drag/drop ops
	{
		m_bDrop = true;
		IDropSource* pIDropSource;
		DWORD dwEffect = DROPEFFECT_NONE;
		if(SUCCEEDED(QueryInterface(IID_IDropSource, (void**)&pIDropSource)))
		{
			// create data object and image
			hr = CreateDataObject(&m_pISourceDataObject, m_ptLastMouse);
			if(SUCCEEDED(hr))
			{
				m_pDragImage = CreateDragImage();
				if(m_pDragImage)
				{
					m_pDragImage->BeginDrag(0, CPoint(8,12));
				}
				// start drag drop
				// when CanCut added, check to see if DROPEFFECT_MOVE allowed (see chordstrip.cpp)
				DWORD dwOKDragEffects = DROPEFFECT_COPY | DROPEFFECT_MOVE;
				m_dwStartDragButton = (unsigned long)wParam;
				/*
				m_startDragPosition.x = (long)(LOWORD(lParam));
				m_startDragPosition.y = (long)(HIWORD(lParam));
				*/
				// set to upper left of chord being dragged
				m_startDragPosition.x = pChord->m_rect.left;
				m_startDragPosition.y = pChord->m_rect.top;
				m_bInternalDrop = false;
				hr = ::DoDragDrop(m_pISourceDataObject, pIDropSource, dwOKDragEffects, &dwEffect);
				// drag drop completed, cleanup
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
					if(m_bInternalDrop)
					{
						m_bInternalDrop = false;
						if(dwEffect & DROPEFFECT_MOVE)
						{
							// we're doing an internal drag/drop, just move the chord
							pChordChange = m_pDragChord;
							ASSERT(pChordChange);
							m_pChordMapMgr->SendEditNotification(IDS_MOVE);
							InternalMove(m_ptDrop);
						}
						else // this path never entered: copies handled like foreign drops
						{
							pChordChange = m_pDragChord;
							ASSERT(pChordChange);
							m_pChordMapMgr->SendEditNotification(IDS_COPY);
							InternalCopy(m_ptDrop);
						}
						m_pChordMapMgr->DetectAndReverseConnections(*(m_pChordMapMgr->m_pChordMapList));
					}
					else if(dwEffect & DROPEFFECT_MOVE)
					{
						m_pChordMapMgr->SendEditNotification(IDS_DELETE);
						DeleteMarkedChords(CE_DRAGSELECT);
						pChordChange = 0;
					}
					if(dwEffect == DROPEFFECT_NONE)
					{
						m_bDrop = false;
					}
					break;
				default:
					m_bDrop = false;
					break;
				}
				hr = S_OK;
				m_pISourceDataObject->Release();
				m_pISourceDataObject = NULL;
				m_startDragPosition.x = 0;
				m_startDragPosition.y = 0;
			}
			pIDropSource->Release();
			UnMarkChords(CE_DRAGSELECT);
		}
		else
		{
			hr = E_FAIL;
		}

		bool drop = m_bDrop;
		m_bDrop = drop;

		if(!m_bDrop)
		{
			/* selected chords should remain selected
			ClearSelectedChords();
			*/
		}
		else
		{
			RecomputeConnections( false );
			m_pChordMapMgr->m_pTimeline->OnDataChanged(0);
		}
		m_pChordMapMgr->m_pTimeline->Refresh();


	}
	else
	{
		// select chord
		pChord->m_chordsel.KeyDown() = TRUE;
	}
	// forces Jazz/OLE drag/drop rather than old intra-chordmap drag/drop
	m_fDragging = FALSE;
	return hr;
}


void CChordMapStrip::GetBoundingRectRelativeToChord(ChordEntryList& list, ChordEntry* pRefChord, RECT& rect)
{
	GetBoundingRect(list, rect);


	// take max extents relative to chord's top left corner
	rect.left = rect.left - pRefChord->m_rect.left;
	rect.top = rect.top - pRefChord->m_rect.top;
	rect.right = rect.right - pRefChord->m_rect.left;
	rect.bottom = rect.bottom - pRefChord->m_rect.top;
/*
	if(m_pISourceDataObject == m_pITargetDataObject)
	{
		// intra-map drag -- correct for diff between drag-point and refchord
		long x = m_startDragPosition.x;
		long y = m_startDragPosition.y;

		long l = x - pRefChord->m_rect.left;
		long t = y - pRefChord->m_rect.top;
		long r = pRefChord->m_rect.right - x;
		long b = pRefChord->m_rect.bottom -y;

		rect.left -= l;
		rect.top -= t;
		rect.right -= l;
		rect.bottom -= t;
	}
*/
}

HRESULT CChordMapStrip::CheckBoundsForDrag(POINTL pt, RECT* pExcess)
{
	VARIANT var;
	int strip_height = GetStripHeight();
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);

	int clocks = V_I4(&var);
	long lengthInMeasures, lengthInBeats, timesigBPM;
	long strip_width;
	m_pChordMapMgr->m_pTimeline->ClocksToPosition(clocks, &strip_width);
	m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat(m_pChordMapMgr->m_dwGroupBits, 0,
									clocks,
									&lengthInMeasures, &lengthInBeats);
	m_pChordMapMgr->GetTimeSig(&timesigBPM, 0);
	lengthInBeats = lengthInMeasures*timesigBPM + lengthInBeats;


	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_VERTICAL_SCROLL, &var);
	int vscroll = V_I4(&var);
/*
	bool ok = (pt.y - m_rectAggregate.top/2) >= 0;
	ok &= ((pt.x - m_rectAggregate.left/2) >= 0);
	ok &= ((pt.y + m_rectAggregate.bottom/2) < strip_height);
	ok &= ((pt.x + m_rectAggregate.right/4) < strip_width);
*/
	
	bool ok = (pt.x + m_rectAggregate.left) >= 0;
	ok = ok && (pt.y + m_rectAggregate.top) >= 0;
	ok = ok && (pt.y + m_rectAggregate.bottom) < strip_height;
	// get position of rightmost chord and translate it to beats
	long rightmost = pt.x + m_rectAggregate.right;
	long m, rightmostBeats;
	rightmost -= CHORD_WIDTH;
	m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat(m_pChordMapMgr->m_dwGroupBits, 0,
														rightmost,
														&m, &rightmostBeats);
	rightmostBeats = m*timesigBPM + rightmostBeats;

	ok = ok && (rightmostBeats < (lengthInBeats-1));

/*
	static int x = 0;
	x += 1;
	x = x % 10;
	if(x == 0)
	{
		TRACE("\r\nl:%d, t:%d, b:%d, height:%d, rightmostBeats:%d, lengthInBeats:%d",
				pt.x + m_rectAggregate.left,
				pt.y + m_rectAggregate.top,
				pt.y + m_rectAggregate.bottom,
				strip_height, 
				rightmostBeats,
				lengthInBeats);
	}
*/
	CString s;
	s = ok ? "OK" : "OUT";
	if(!ok)
	{
		TRACE("DragPoint Outside = %d, %d\r\n", pt.x, pt.y);
	}
	if(pExcess)
	{
		pExcess->top = pt.y - m_rectAggregate.top/2;
		pExcess->left = pt.x - m_rectAggregate.left/2;
		pExcess->bottom = strip_height - pt.y - m_rectAggregate.bottom/2;
		pExcess->right = strip_width - pt.x - m_rectAggregate.right/4;
	}
	return ok ? S_OK : S_FALSE;
}




void CChordMapStrip::ComputeSelectionBoxOutline(POINTS ptBeg, POINTS ptEnd, bool bRecomputeDragState)
{
	VARIANT var;
	CDC dc;
	CRect rectFBar;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	ASSERT(m_pChordMapMgr);
	
	if(!m_pChordMapMgr)
		return;
	ASSERT(m_pChordMapMgr->m_pTimeline);
	if(!m_pChordMapMgr->m_pTimeline)
		return;
	if( FAILED ( m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_FBAR_RECT, &var ) ) )
	{
		return;
	}
	rectFBar.InflateRect(1, 1);

	m_SelectionDragState.bActive = true;


	// Get the DC of our Strip					
	if( SUCCEEDED(m_pChordMapMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_GET_HDC, &var )) )
	{
		RECT rtStrip;
		if( dc.Attach( (HDC)(var.lVal) ) != 0 )
		{                  

			dc.GetClipBox(&rtStrip);
			long vpos, hpos;
			if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var )))
			{
				hpos = V_I4(&var);
			}
			if(SUCCEEDED(m_pChordMapMgr->m_pTimeline->GetTimelineProperty( TP_VERTICAL_SCROLL, &var )))
			{
				vpos = V_I4(&var);
			}

			if(bRecomputeDragState)
			{
				// setup or update autoscroll
				short xOrg = ptBeg.x - static_cast<short>(hpos);
				short xTo = ptEnd.x  - static_cast<short>(hpos);
				short yOrg = ptBeg.y - static_cast<short>(vpos);
				short yTo = ptEnd.y - static_cast<short>(vpos);
				TRACE("ptBeg: (%d, %d) ptEnd: (%d, %d)\r\n", ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
				/*
				short xOrg = ptBeg.x - static_cast<short>(hpos);
				short xTo = ptEnd.x  - static_cast<short>(hpos);
				short yOrg = ptBeg.y - static_cast<short>(vpos);
				short yTo = ptEnd.y - static_cast<short>(vpos);
				*/
				m_SelectionDragState.xOrg = xOrg;
				m_SelectionDragState.yOrg = yOrg;
				m_SelectionDragState.xTo = xTo;
				m_SelectionDragState.yTo = yTo;


//				TRACE("xTo: %d, right edge: %d\r\n", xTo, rtStrip.right);
				if(xTo > rtStrip.right)
				{
					m_SelectionDragState.dir |= DragState::right;
				}
				else
				{
					m_SelectionDragState.dir &= ~DragState::right;
				}
				if(xTo < rtStrip.left+3)
				{
					m_SelectionDragState.dir |= DragState::left;
				}
				else
				{
					m_SelectionDragState.dir &= ~DragState::left;
				}
				if(yTo < rtStrip.top)
				{
					m_SelectionDragState.dir |= DragState::up;
				}
				else
				{
					m_SelectionDragState.dir &= ~DragState::up;
				}
				if(yTo > rtStrip.bottom)
				{
					m_SelectionDragState.dir |= DragState::down;
				}
				else
				{
					m_SelectionDragState.dir &= ~DragState::down;
				}
				if(m_SelectionDragState.dir != DragState::off && !m_SelectionDragState.bAutoscroll)
				{
					// turn autoscrolling on
					m_SelectionDragState.bAutoscroll = true;
					::SetTimer(GetTimelineCWnd()->m_hWnd, SelectionDrag, 100, NULL);
//					TRACE("Setting Timer for SelectionDrag\r\n");
				}
				else if(m_SelectionDragState.dir == DragState::off && m_SelectionDragState.bAutoscroll)
				{
					// turn autoscrolling off
					m_SelectionDragState.bAutoscroll = false;
					::KillTimer(GetTimelineCWnd()->m_hWnd, SelectionDrag);
					TRACE("Killing Timer for SelectionDrag\r\n");
				}
			}

		  // set select rectangle
			m_rectSelect.top = ptBeg.y <= ptEnd.y ? ptBeg.y : ptEnd.y;
			m_rectSelect.bottom = ptBeg.y <= ptEnd.y ? ptEnd.y : ptBeg.y;
			m_rectSelect.right = ptBeg.x <= ptEnd.x ? ptEnd.x : ptBeg.x;
			m_rectSelect.left = ptBeg.x <= ptEnd.x ? ptBeg.x : ptEnd.x;

			if(ptBeg.x <= ptEnd.x)
			{
				// normal rectangle
				if(m_SelectionDragState.dir == DragState::left)
				{
					// against left edge
					m_rectBounding.left = rtStrip.left;
				}
				else
				{
					m_rectBounding.left = ptBeg.x - hpos;
				}
				if(m_SelectionDragState.dir == DragState::right)
				{
					// against right edge
					m_rectBounding.right = rtStrip.right;
				}
				else
				{
					m_rectBounding.right = ptEnd.x - hpos;
				}
			}
			else
			{
				// left side > right side, swap sides
				if(m_SelectionDragState.dir == DragState::left)
				{
					// against left edge
					m_rectBounding.right = rtStrip.left;
				}
				else
				{
					m_rectBounding.right = ptBeg.x - hpos;
				}
				if(m_SelectionDragState.dir == DragState::right)
				{
					// against right edge
					m_rectBounding.left = rtStrip.right;
				}
				else
				{
					m_rectBounding.left = ptEnd.x - hpos;
				}
			}

			if(ptBeg.y <= ptEnd.y)
			{
				// normal rectangle
				if(m_SelectionDragState.dir == DragState::up)
				{
					// against top edge
					m_rectBounding.top = rtStrip.top;
				}
				else
				{
					m_rectBounding.top = ptBeg.y/* - vpos*/;
				}
				if(m_SelectionDragState.dir == DragState::down)
				{
					// against bottom edge
					m_rectBounding.bottom = rtStrip.bottom;
				}
				else
				{
					m_rectBounding.bottom = ptEnd.y/* - vpos*/;
				}
			}
			else
			{
				// top side > bottom side, swap sides
				if(m_SelectionDragState.dir == DragState::up)
				{
					// against top edge
					m_rectBounding.bottom = rtStrip.top;
				}
				else
				{
					m_rectBounding.bottom = ptBeg.y /*- vpos*/;
				}
				if(m_SelectionDragState.dir == DragState::down)
				{
					// against bottom edge
					m_rectBounding.top = rtStrip.bottom;
				}
				else
				{
					m_rectBounding.top = ptEnd.y /*- vpos*/;
				}
			}

		}
		TRACE("top edge: %x bottom edge: %x\r\n", rtStrip.top, rtStrip.bottom);
		dc.Detach();
	}
}

void CChordMapStrip::GetBoundingRect(ChordEntryList& list, RECT& rect)
{
	rect.left = 0x7fffffff;
	rect.right = 0;
	rect.top = 0x7fffffff;
	rect.bottom = 0;

	for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_rect.left < rect.left)
		{
			rect.left = pChord->m_rect.left;
		}
		if(pChord->m_rect.right > rect.right)
		{
			rect.right = pChord->m_rect.right;
		}
		if(pChord->m_rect.top < rect.top)
		{
			rect.top = pChord->m_rect.top;
		}
		if(pChord->m_rect.bottom > rect.bottom)
		{
			rect.bottom = pChord->m_rect.bottom;
		}
	}
	if(rect.bottom < rect.top)
	{
		rect.bottom = rect.top;
	}
}

void CChordMapStrip::GetBoundingRectOfSelectedChords(RECT& rect)
{
	rect.left = 0x7fffffff;
	rect.right = 0;
	rect.top = 0x7fffffff;
	rect.bottom = 0;

	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); 
			pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown())
		{
			if(pChord->m_rect.left < rect.left)
			{
				rect.left = pChord->m_rect.left;
			}
			if(pChord->m_rect.right > rect.right)
			{
				rect.right = pChord->m_rect.right;
			}
			if(pChord->m_rect.top < rect.top)
			{
				rect.top = pChord->m_rect.top;
			}
			if(pChord->m_rect.bottom > rect.bottom)
			{
				rect.bottom = pChord->m_rect.bottom;
			}
		}
	}
}

long CChordMapStrip::GetVerticalScroll()
{
	VARIANT var;
	m_pChordMapMgr->m_pTimeline->GetTimelineProperty(TP_VERTICAL_SCROLL, &var);
	long vscroll = V_I4(&var);
	return vscroll;
}



HRESULT CChordMapStrip::RecomputeConnections( bool fChangingToVariableMode )
{
	ChordEntry* pChord;
	for(pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		NextChord*	pConnection;
		pConnection = pChord->m_nextchordlist.GetHead();

		while( pConnection )
		{
			if( pConnection->m_nextchord != NULL )
			{

				long nC1Pos, nC2Pos;
				long nC1Clocks, nC2Clocks;
				m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
																  pChord->m_chordsel.Measure(), 
																  pChord->m_chordsel.Beat(), 
																  &nC1Clocks );
				m_pChordMapMgr->m_pTimeline->MeasureBeatToClocks( m_pChordMapMgr->m_dwGroupBits, 0,
																  pConnection->m_nextchord->m_chordsel.Measure(),
																  pConnection->m_nextchord->m_chordsel.Beat(), 
																  &nC2Clocks );
				long nDeltaC1C2 = ( nC1Clocks > nC2Clocks ) ? nC1Clocks - nC2Clocks 
																: nC2Clocks - nC1Clocks;

				m_pChordMapMgr->m_pTimeline->ClocksToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 0, nDeltaC1C2, &nC1Pos, &nC2Pos );

				long bpm;
				m_pChordMapMgr->GetTimeSig(&bpm, 0);
				nC2Pos += bpm*nC1Pos;

				if( m_fVariableNotFixed )
				{
					if( fChangingToVariableMode
					|| ((pChord->m_chordsel.KeyDown() == TRUE) && (pConnection->m_nextchord->m_chordsel.KeyDown() == FALSE))
					|| ((pChord->m_chordsel.KeyDown() == FALSE) && (pConnection->m_nextchord->m_chordsel.KeyDown() == TRUE)) )
					{
						pConnection->m_nminbeats = 1;
						pConnection->m_nmaxbeats = (short)nC2Pos;
					}
				}
				else
				{
					pConnection->m_nminbeats = (short)nC2Pos;
					pConnection->m_nmaxbeats = (short)nC2Pos;
				}
			}

			pConnection = pConnection->GetNext();
		}
	}
	return S_OK;
}

bool CChordMapStrip::ContainsSignpost(ChordEntryList& list)
{
	bool bContainsSignpost = false;
	for(ChordEntry* pChord = list.GetHead(); pChord && !bContainsSignpost; pChord = pChord->GetNext())
	{
		if( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END) )
		{
			bContainsSignpost = true;
		}
	}
	return bContainsSignpost;
}

bool CChordMapStrip::SelectedChordsContainsSignpost()
{
	bool bContainsSignpost = false;
	for(ChordEntry* pChord = m_pChordMapMgr->m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if( (pChord->m_chordsel.KeyDown()) && ( (pChord->m_dwflags & CE_START) || (pChord->m_dwflags & CE_END)) )
		{
			bContainsSignpost = true;
		}
	}
	return bContainsSignpost;
}

void CChordMapStrip::PositionToMeasureBeat(long pos, long& measure, long& beat)
{
	m_pChordMapMgr->m_pTimeline->PositionToMeasureBeat( m_pChordMapMgr->m_dwGroupBits, 
														0, 
														pos, 
														&measure, &beat);
}

void CChordMapStrip::MeasureBeatToPosition(long measure, long beat, long& pos)
{
	m_pChordMapMgr->m_pTimeline->MeasureBeatToPosition(m_pChordMapMgr->m_dwGroupBits,
														0,
														measure, beat,
														&pos);
}

void CChordMapStrip::PositionToClocks(long pos, long& clocks)
{
	m_pChordMapMgr->m_pTimeline->PositionToClocks(pos, &clocks);
}

void CChordMapStrip::ClocksToPosition(long clocks, long& pos)
{
	m_pChordMapMgr->m_pTimeline->ClocksToPosition(clocks, &pos);
}

long CChordMapStrip::ClocksPerMeasure()
{
	long bpm;
	long beat;
	m_pChordMapMgr->GetTimeSig(&bpm, &beat);

	long clocksPerMeasure = ( (DMUS_PPQ * 4) / beat ) * bpm;
	return clocksPerMeasure;
}