/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// BaseStrip.cpp : Implementation of CBaseStrip
#include "stdafx.h"
#include "BaseMgr.h"
#include "selectedregion.h"
#include "resource.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_STRIP_HEIGHT 20

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


IDMUSProdNode*	CBaseStrip::m_pIDocRootOfDragDropSource = NULL;
BOOL			CBaseStrip::m_fDragDropIntoSameDocRoot = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip constructor/destructor

CBaseStrip::CBaseStrip( CBaseMgr* pBaseMgr )
{
	ASSERT( pBaseMgr );
	if ( pBaseMgr == NULL )
	{
		return;
	}

	m_pBaseMgr = pBaseMgr;

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_bGutterSelected = FALSE;
	m_lGutterBeginSelect = 0;
	m_lGutterEndSelect = 0;

	m_fSelecting = FALSE;
	m_nStripIsDragDropSource = 0;

	m_lXPos = 0;
	m_pSelectedRegions = NULL;

	m_pISourceDataObject = NULL;
	m_pITargetDataObject = NULL;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_lStartDragPosition = 0;
	m_fPropPageActive = FALSE;
	m_dwDropEffect = DROPEFFECT_NONE;
	m_fInRightClickMenu = false;
	m_pPropPageMgr = NULL;
	m_fInFunctionBarMenu = false;
}

CBaseStrip::~CBaseStrip()
{
	ASSERT( m_pBaseMgr );
	if ( m_pBaseMgr )
	{
		//m_pBaseMgr->Release();
		m_pBaseMgr = NULL;
	}
	if ( m_pISourceDataObject )
	{
		m_pISourceDataObject->Release();
		m_pISourceDataObject = NULL;
	}
	if ( m_pITargetDataObject )
	{
		m_pITargetDataObject->Release();
		m_pITargetDataObject = NULL;
	}
	if ( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}

	if(m_pSelectedRegions)
	{
		delete m_pSelectedRegions;
		m_pSelectedRegions = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::QueryInterface

STDMETHODIMP CBaseStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
// CBaseStrip::AddRef

STDMETHODIMP_(ULONG) CBaseStrip::AddRef(void)
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Release

STDMETHODIMP_(ULONG) CBaseStrip::Release(void)
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
// CBaseStrip IDMUSProdStrip implementation

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Draw

HRESULT	STDMETHODCALLTYPE CBaseStrip::Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(sv);

	// Validate our pointer to the Timeline
	if( m_pBaseMgr->m_pTimeline )
	{
		// Draw Measure and Beat lines in our strip
		m_pBaseMgr->m_pTimeline->DrawMusicLines( hDC, ML_DRAW_MEASURE_BEAT, m_pBaseMgr->m_dwGroupBits, 0, lXOffset );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CBaseStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
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
		V_INT(pvar) = DEFAULT_STRIP_HEIGHT;
		break;

	case SP_NAME:
		{
			BSTR bstr;
			CString strStripName;
			strStripName.LoadString( IDS_STRIP_NAME );

			pvar->vt = VT_BSTR; 
			try
			{
				bstr = GetName(m_pBaseMgr->m_dwGroupBits, strStripName).AllocSysString();
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
		if( m_pBaseMgr )
		{
			m_pBaseMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
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
// CBaseStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CBaseStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	bool fRefresh = false;

	switch( sp )
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		// Verify that we're passed an integer
		if( var.vt != VT_I4)
		{
			return E_FAIL;
		}

		// Update the appropriate member variables with the new information
		if( sp == SP_BEGINSELECT )
		{
			m_lGutterBeginSelect = V_I4( &var );
		}
		else
		{
			m_lGutterEndSelect = V_I4( &var );
		}

		// This flag will be true iff we're trying to clear the selections in all other strips
		if( m_fSelecting )
		{
			break;
		}

		// Initialize anchor for future shift-select operations
		m_pSelectedRegions->SetShiftSelectAnchor( 0 );

		// If the start time and the end time are identical, unselect everything
		if( m_lGutterBeginSelect == m_lGutterEndSelect )
		{	
			// Clear all selections
			if( m_pSelectedRegions )
			{
				fRefresh = m_pSelectedRegions->Clear();
			}

			// Deselect all items
			if( SelectItemsInSelectedRegions() || fRefresh )
			{
				// If any items become unselected, redraw the strip
				m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
			}
			break;
		}

		// If our gutter is selected
		if( m_bGutterSelected )
		{
			// Clear all current selections
			if( m_pSelectedRegions )
			{
				m_pSelectedRegions->Clear();
			}

			// Always need to refresh the display
			fRefresh = true;

			// Select all items between the new time range, and deselect all items
			// outside the new time range
			SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
		}
		else
		{
			// Our gutter is not selected
			// Clear all current selections
			if( m_pSelectedRegions )
			{
				fRefresh = m_pSelectedRegions->Clear();
			}

			// If any items become unselected, set fRefresh to true
			fRefresh |= SelectItemsInSelectedRegions();
		}

		// If the selection state of any item was changed
		if( fRefresh )
		{
			// Redraw the strip
			m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

			// Update the item property page
			if( m_pBaseMgr->m_pPropPageMgr != NULL )
			{
				m_pBaseMgr->m_pPropPageMgr->RefreshData();
			}
		}
		break;

	case SP_GUTTERSELECT:
		// Check if the gutter selection state changed
		if( m_bGutterSelected != V_BOOL(&var) )
		{
			// Save the new state of the gutter selection
			m_bGutterSelected = V_BOOL(&var);

			// If the time range is empty
			if( m_lGutterBeginSelect == m_lGutterEndSelect )
			{	
				// Clear all current selections
				if( m_pSelectedRegions )
				{
					m_pSelectedRegions->Clear();
				}

				// If any items become unselected, redraw the strip
				if( SelectItemsInSelectedRegions() )
				{
					m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
				}
				break;
			}

			// Check if the gutter became selected
			if( m_bGutterSelected )
			{
				// Clear the list of selected regions
				if( m_pSelectedRegions )
				{
					m_pSelectedRegions->Clear();
				}

				// Select the segment of the timeline
				// This will return true if the selection state of any item changed
				fRefresh = SelectSegment( m_lGutterBeginSelect, m_lGutterEndSelect );
			}
			else
			{
				// Clear the list of selected regions
				if( m_pSelectedRegions )
				{
					m_pSelectedRegions->Clear();
				}

				// Clear the selection state of all items
				// This will return true if the selection state of any item changed
				fRefresh = SelectItemsInSelectedRegions();
			}

			// Since the gutter state changed, we need to redraw the strip
			m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

			// However, only update the property page if the selection state of any items
			// changed
			if( fRefresh )
			{
				// Update the property page
				if( m_pBaseMgr->m_pPropPageMgr != NULL )
				{
					m_pBaseMgr->m_pPropPageMgr->RefreshData();
				}
			}
		}
		break;

	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::OnWMMessage

HRESULT STDMETHODCALLTYPE CBaseStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);

	// Process the window message
	HRESULT hr = S_OK;
	if( m_pBaseMgr->m_pTimeline == NULL )
	{
		return E_FAIL;
	}
	switch( nMsg )
	{
	case WM_RBUTTONUP:
		// Display a right-click context menu.

		// Get the cursor position (To put the menu there)
		POINT pt;
		if( !GetCursorPos( &pt ) )
		{
			ASSERT(FALSE);
			hr = E_UNEXPECTED;
			break;
		}

		// Save the position of the click so we know where to insert a Lyric. if Insert is selected.
		m_lXPos = lXPos;

		// Display the menu
		PostRightClickMenu( pt );
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
			hr = ShowPropertySheet();
			if (m_fShowItemProps)
			{
				// Change to the lyric property page
				m_pBaseMgr->OnShowProperties();

				// Update the property page, if it exists
				if( m_pBaseMgr->m_pPropPageMgr )
				{
					m_pBaseMgr->m_pPropPageMgr->RefreshData();
				}
			}
			else
			{
				// Change to our property page
				OnShowProperties();

				// Update the property page, if it exists
				if( m_pPropPageMgr )
				{
					m_pPropPageMgr->RefreshData();
				}
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
		case ID_EDIT_PASTE_OVERWRITE:
		case ID_EDIT_PASTE:
			// TODO: Paste->Merge vs. Paste->Overwrite distinction?
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

	case WM_CREATE:
		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_lGutterBeginSelect );
		m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_lGutterEndSelect );

		if(!m_pSelectedRegions)
		{
			m_pSelectedRegions = new CListSelectedRegion(m_pBaseMgr->m_pTimeline, m_pBaseMgr->m_dwGroupBits);
		}
		else
		{
			m_pSelectedRegions->Timeline() = m_pBaseMgr->m_pTimeline;
			m_pSelectedRegions->GroupBits() = m_pBaseMgr->m_dwGroupBits;
		}
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::FBDraw

HRESULT CBaseStrip::FBDraw( HDC hDC, STRIPVIEW sv )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(hDC);
	UNREFERENCED_PARAMETER(sv);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::FBOnWMMessage

HRESULT CBaseStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(lXPos);
	UNREFERENCED_PARAMETER(lYPos);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);

	// Process the window message
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONDOWN:
		// Change the property page to the strip (group bits) property page
		m_fShowItemProps = FALSE;
		OnShowProperties();
		break;

	case WM_RBUTTONUP:
		// Change the property page to the strip (group bits) property page
		m_fShowItemProps = FALSE;
		OnShowProperties();

		// Set the cursor position for the insert operation to -1
		m_lXPos = -1;

		// Get the cursor position (To put the menu there)
		POINT pt;
		if( !GetCursorPos( &pt ) )
		{
			hr = E_UNEXPECTED;
			break;
		}

		// Display a right-click context menu.
		m_fInFunctionBarMenu = true;
		hr = PostRightClickMenu( pt );
		m_fInFunctionBarMenu = false;
		break;

	default:
		break;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Cut

HRESULT CBaseStrip::Cut( IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Double-check that we can actually cut
	if( CanCut() != S_OK )
	{
		ASSERT( FALSE );
		return E_UNEXPECTED;
	}

	// Cut is simply a Copy followed by a Delete.
	HRESULT hr = Copy(pITimelineDataObject);

	// If the copy succeeded, do the delete
	if( SUCCEEDED( hr ))
	{
		hr = Delete();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Copy

// Pure virtual
// HRESULT CBaseStrip::Copy( IDMUSProdTimelineDataObject* pITimelineDataObject ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Paste

// Pure virtual
// HRESULT CBaseStrip::Paste( IDMUSProdTimelineDataObject* pITimelineDataObject ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Insert

// Pure virtual
// HRESULT CBaseStrip::Insert( void ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::Delete

// Pure virtual
// HRESULT CBaseStrip::Delete( void ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::SelectAll

HRESULT CBaseStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pBaseMgr != NULL );
	if( m_pBaseMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pBaseMgr->m_pTimeline != NULL );
	if( m_pBaseMgr->m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Select everything
	m_pSelectedRegions->SelectAll();

	// Update the selection state of the items
	if( SelectItemsInSelectedRegions() )
	{
		// If an an item became selected, redraw the strip
		m_pBaseMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip *)this, NULL, TRUE );

		// Update the item property page
		if( m_pBaseMgr->m_pPropPageMgr != NULL )
		{
			m_pBaseMgr->m_pPropPageMgr->RefreshData();
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::CanCut

HRESULT CBaseStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pBaseMgr != NULL );
	if( m_pBaseMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pBaseMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	// Otherwise, Cut is simply a Copy followed by a Delete
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
// CBaseStrip::CanCopy

HRESULT CBaseStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pBaseMgr != NULL );
	if( m_pBaseMgr == NULL )
	{
		return E_UNEXPECTED;
	}

	// If our gutter is selected, and the user selected a range of time in the time strip,
	// we can cut even if nothing is selected.
	VARIANT variant;
	long lTimeStart, lTimeEnd;
	if( SUCCEEDED( m_pBaseMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GUTTER_SELECTED, &variant ) )
	&&	(V_BOOL( &variant ) == TRUE)
	&&	SUCCEEDED( m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &lTimeStart ) )
	&&	(lTimeStart >= 0)
	&&	SUCCEEDED( m_pBaseMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &lTimeEnd ) )
	&&	(lTimeEnd > lTimeStart) )
	{
		return S_OK;
	}

	// Otherwise, check if anything is selected
	return IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::CanPaste

// Pure virtual
// HRESULT CBaseStrip::CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::CanInsert

// Pure virtual
// HRESULT CBaseStrip::CanInsert( void ) = 0;


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::CanDelete

HRESULT CBaseStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if anything is selected
	return IsSelected() ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::CanSelectAll

HRESULT CBaseStrip::CanSelectAll( void )
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

	if( IsEmpty() )
	{
		return S_FALSE;
	}

	return S_OK;
}


// IDropSource Methods

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::QueryContinueDrag

HRESULT CBaseStrip::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
 
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
// CBaseStrip::GiveFeedback

HRESULT CBaseStrip::GiveFeedback( DWORD dwEffect )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwEffect);

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


// IDropTarget Methods

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::DragEnter

HRESULT CBaseStrip::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIDataObject == NULL )
	{
		ASSERT(FALSE); // This shouldn't happen
		return E_POINTER;
	}

	if( m_pITargetDataObject )
	{
		ASSERT(FALSE); // This shouldn't happen

		m_pITargetDataObject->Release();
		// No need - it's overwritten just below
		// m_pITargetDataObject = NULL;
	}

	// Store IDataObject associated with current drag-drop operation
	m_pITargetDataObject = pIDataObject;
	m_pITargetDataObject->AddRef();

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::DragOver

HRESULT CBaseStrip::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pt);

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if(m_pITargetDataObject == NULL)
	{
		ASSERT( FALSE ); // Shouldn't happen - CanPasteFromData will return E_POINTER.
	}

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
// CBaseStrip::DragLeave

HRESULT CBaseStrip::DragLeave( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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
// CBaseStrip::Drop

// Pure virtual
//HRESULT CBaseStrip::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) = 0;


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::GetData

HRESULT CBaseStrip::GetData( void **ppData )
{
	// Not needed
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
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
		pPPGTrackParams->dwGroupBits = m_pBaseMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pBaseMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = g_dwTrackExtrasMask;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pBaseMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = g_dwProducerOnlyMask;
		break;
	}
	default:
		ASSERT(FALSE);
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::SetData

HRESULT CBaseStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}

	// Check which property page is setting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
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
			m_pBaseMgr->m_dwOldGroupBits = pPPGTrackParams->dwGroupBits;
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pBaseMgr->m_dwTrackExtrasFlags )
		{
			m_pBaseMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_pBaseMgr->m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pBaseMgr->OnDataChanged();
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pBaseMgr->m_dwProducerOnlyFlags )
		{
			m_pBaseMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_pBaseMgr->m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pBaseMgr->OnDataChanged();
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
// CBaseStrip::OnShowProperties

HRESULT CBaseStrip::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate our Timeline pointer
	if( m_pBaseMgr->m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Validate our Framework pointer
	if( m_pBaseMgr->m_pDMProdFramework == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	m_pBaseMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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

	// Check if our property page manager exists yet
	if( m_pPropPageMgr == NULL )
	{
		// Nope, need to create it
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr;

		// Check if the creation succeeded.
		if( pPPM == NULL )
		{
			pIPropSheet->Release();
			return E_OUTOFMEMORY;
		}

		// Set m_pPropPageMgr with a reference to the IDMUSProdPropPageManager interface
		HRESULT hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );

		// Release our original reference on the property page manager (added when it
		// was created).
		m_pPropPageMgr->Release();

		// If the QueryInterface failed, return.
		if( FAILED(hr) )
		{
			pIPropSheet->Release();
			// Nothing to release for pPPM, since no reference was added by the call to QueryInterface.
			return hr;
		}
	}

	// Store the current active tab
	short nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;

	// Set the property page to refer to the group bits property page.
	m_pBaseMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Flag that the group bits property page is active
	m_fPropPageActive = TRUE;

	// Reset the active tab
	pIPropSheet->SetActivePage( nActiveTab ); 

	// release our reference to the property sheet
	pIPropSheet->Release();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::OnRemoveFromPageManager

HRESULT CBaseStrip::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Clear our property page manager
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->SetObject(NULL);
	}

	// Flag thar the group bits property page is not active
	m_fPropPageActive = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::ShowPropertySheet

HRESULT CBaseStrip::ShowPropertySheet( void )
{
	HRESULT hr = E_UNEXPECTED;

	// Get a pointer to the property sheet and show it
	IDMUSProdPropSheet*	pIPropSheet;
	if( m_pBaseMgr->m_pDMProdFramework )
	{
		hr = m_pBaseMgr->m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
		ASSERT( SUCCEEDED( hr ));
		if( SUCCEEDED( hr ))
		{
			pIPropSheet->Show( TRUE );
			pIPropSheet->Release();
			hr = S_OK;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::PostRightClickMenu

HRESULT CBaseStrip::PostRightClickMenu( POINT pt )
{
	m_fInRightClickMenu = TRUE;
	HRESULT hr = m_pBaseMgr->m_pTimeline->TrackPopupMenu(NULL, pt.x, pt.y, (IDMUSProdStrip *)this, TRUE);
	m_fInRightClickMenu = FALSE;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseStrip::GetTimelineHWND

HWND CBaseStrip::GetTimelineHWND()
{
	HWND hwnd = NULL;
	IOleWindow *pOleWindow;
	if( m_pBaseMgr->m_pTimeline
	&&	SUCCEEDED( m_pBaseMgr->m_pTimeline->QueryInterface( IID_IOleWindow, (void**)&pOleWindow) ) )
	{
		pOleWindow->GetWindow( &hwnd );
		pOleWindow->Release();
	}

	return hwnd;
}


/////////////////////////////////////////////////////////////////////////////
// Helper methods

/////////////////////////////////////////////////////////////////////////////
// CopyDataToClipboard

HRESULT CopyDataToClipboard( IDMUSProdTimelineDataObject* pITimelineDataObject, IStream* pStreamCopy, UINT uiClipFormat, CBaseMgr *pBaseMgr, CBaseStrip *pBaseStrip )
{
	HRESULT hr;
	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed IDMUSProdTimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat( uiClipFormat, pStreamCopy );
		if ( hr != S_OK )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}
	else
	{
		// There is no existing data object, so just create a new one
		hr = pBaseMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		ASSERT(hr == S_OK);
		if(hr != S_OK)
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		long lRegionStart, lRegionEnd;
		pBaseStrip->m_pSelectedRegions->GetSpan(lRegionStart, lRegionEnd);
		hr = pITimelineDataObject->SetBoundaries(lRegionStart, (lRegionEnd - 1));

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( uiClipFormat, pStreamCopy );
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

		// If we already have a pBaseMgr->m_pCopyDataObject, release it
		if(pBaseMgr->m_pCopyDataObject)
		{
			pBaseMgr->m_pCopyDataObject->Release();
		}

		// set pBaseMgr->m_pCopyDataObject to the object we just copied to the clipboard
		pBaseMgr->m_pCopyDataObject = pIDataObject;

		// Not needed = Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// pBaseMgr->m_pCopyDataObject->AddRef
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// GetTimelineDataObject

HRESULT GetTimelineDataObject( IDMUSProdTimelineDataObject* &pITimelineDataObject, IDMUSProdTimeline* pTimeline, IDataObject *pIDataObject )
{
	if(pITimelineDataObject == NULL)
	{
		if( pIDataObject == NULL )
		{
			// Get the IDataObject from the clipboard
			HRESULT hr = OleGetClipboard(&pIDataObject);
			if(FAILED(hr) || (pIDataObject == NULL))
			{
				return E_FAIL;
			}
		}
		else
		{
			pIDataObject->AddRef();
		}

		// Create a new TimelineDataObject
		HRESULT hr = pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
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
			pITimelineDataObject = NULL;
			return E_FAIL;
		}
	}
	else
	{
		pITimelineDataObject->AddRef();
	}
	
	return S_OK;
}