// SongDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"
#include "Song.h"
#include "SongCtl.h"
#include "SongDlg.h"
#include "DlgAddTracks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Bitmaps
long CSongDlg::sm_lBitmapRefCount = 0;
CBitmap	CSongDlg::sm_bmpTransition;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox

CSegmentListBox::CSegmentListBox()
{
	m_pSongDlg = NULL;

	m_ptLastXYPos.x = INT_MAX;
	m_ptLastXYPos.y = INT_MAX;
}

CSegmentListBox::~CSegmentListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::SelectVirtualSegment

void CSegmentListBox::SelectVirtualSegment( CVirtualSegment* pVirtualSegment )
{
	ASSERT( pVirtualSegment != NULL );

	int nCurSel = GetCurSel();
	int nPos = -1;
	int nNbrItems = GetCount();
	for( int i = 0 ;  i < nNbrItems ;  i++ )
	{
		CVirtualSegment* pVirtualSegmentList = (CVirtualSegment *)GetItemDataPtr( i );

		if( pVirtualSegmentList == pVirtualSegment )
		{
			nPos = i;
			break;
		}
	}

	if( nPos != LB_ERR )
	{
		SetSel( -1, FALSE );
		SetSel( nPos, TRUE ); 
		SetCaretIndex( nPos, 0 ); 
		OnSelChange();
		SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::SelectVirtualSegmentList

void CSegmentListBox::SelectVirtualSegmentList( CTypedPtrList<CPtrList, CVirtualSegment*>& list )
{
	SetSel( -1, FALSE );

	POSITION pos = list.GetHeadPosition();
	while( pos )
	{
		CVirtualSegment* pVirtualSegment = list.GetNext( pos );
	
		int nPos = FindStringExact( -1, (LPCTSTR)pVirtualSegment );
		if( nPos != LB_ERR )
		{
			SetSel( nPos, TRUE ); 
			SetCaretIndex( nPos, 0 ); 
		}
	}

	OnSelChange();
	SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::CreateDataObject

HRESULT CSegmentListBox::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}
	*ppIDataObject = NULL;

	HRESULT hr = E_FAIL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Mark selected VirtualSegments
	m_pSongDlg->MarkSelectedVirtualSegments( BF_SELECTED );

	// Save selected VirtualSegments
	IStream* pIStream;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AllocMemoryStream(FT_DESIGN, GUID_SaveSelectedVirtualSegments, &pIStream) ) )
	{
		if( SUCCEEDED ( m_pSongDlg->m_pSong->Save( pIStream, FALSE ) ) )
		{
			// Place CF_VIRTUAL_SEGMENT_LIST into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pSongComponent->m_cfVirtualSegmentList, pIStream ) ) )
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

	// Unmark selected VirtualSegments
	m_pSongDlg->UnMarkVirtualSegments( BF_SELECTED );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox message handlers

BEGIN_MESSAGE_MAP(CSegmentListBox, CListBox)
	//{{AFX_MSG_MAP(CSegmentListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_VKEYTOITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnSetFocus

void CSegmentListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_VIRTUAL_SEGMENTS;
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnKillFocus

void CSegmentListBox::OnKillFocus( CWnd* pNewWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnKillFocus( pNewWnd );
	
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::DrawItem

void CSegmentListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
	}

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
    ||  lpDrawItemStruct->itemAction & ODA_SELECT 
    ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
	{
		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)lpDrawItemStruct->itemData; 
		ASSERT( pVirtualSegment != NULL );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = m_pSongDlg->CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			// Get the VirtualSegment's name
			CString strName;
			pVirtualSegment->GetName( strName );

			// Set horizontal extent
			TEXTMETRIC tm;
			dc.GetTextMetrics( &tm );
			CSize sizeText = dc.GetTextExtent( strName );
			sizeText.cx += tm.tmMaxCharWidth;
			if( sizeText.cx > GetHorizontalExtent() )
			{
				SetHorizontalExtent( sizeText.cx );
			}

			// Fill the background color
			if( pVirtualSegment->m_fIsPlaying )
			{
				dc.FillSolidRect( &rect, RGB(255,0,0) );
				dc.SetTextColor( RGB(255,255,255) );
			}
			else if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == GetSafeHwnd() )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_INACTIVECAPTION) );
					dc.SetTextColor( ::GetSysColor(COLOR_INACTIVECAPTIONTEXT) );
				}
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Draw top separator (when applicable)
			int nPos = m_pSongDlg->m_pSong->VirtualSegmentToIndex( pVirtualSegment );
			if( nPos > 0 )
			{
				CVirtualSegment* pPrevVirtualSegment = m_pSongDlg->m_pSong->IndexToVirtualSegment( nPos - 1 );
				if( pPrevVirtualSegment 
				&&  pPrevVirtualSegment->m_pNextVirtualSegment != pVirtualSegment )
				{
					CRect rectSeparator( rect );
					rectSeparator.bottom = rectSeparator.top + 1;
					dc.FillSolidRect( &rectSeparator, ::GetSysColor(COLOR_WINDOW) );
				}
			}

			// Draw bottom separator (when applicable)
			if( pVirtualSegment->m_pNextVirtualSegment == NULL )
			{
				CRect rectSeparator( rect );
				rectSeparator.top = rectSeparator.bottom - 2;
				dc.FillSolidRect( &rectSeparator, ::GetSysColor(COLOR_WINDOW) );
		
				// Draw solid horizontal line (bottom)
				CPen pen;
				if( pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) )
				{
					CPen* pPenOld = dc.SelectObject( &pen );
					dc.MoveTo( rect.left, (rect.bottom - 1) );
					dc.LineTo( rect.right, (rect.bottom - 1) );
					dc.SelectObject( pPenOld );
				}
			}
			else
			{
				int nPos1 = m_pSongDlg->m_pSong->VirtualSegmentToIndex( pVirtualSegment );
				int nPos2 = m_pSongDlg->m_pSong->VirtualSegmentToIndex( pVirtualSegment->m_pNextVirtualSegment );

				if( nPos1 != (nPos2 - 1) )
				{
					CRect rectSeparator( rect );
					rectSeparator.top = rectSeparator.bottom - 2;
					dc.FillSolidRect( &rectSeparator, ::GetSysColor(COLOR_WINDOW) );

					// Draw broken horizontal line (bottom)
					CPen pen;
					if( pen.CreatePen(PS_DASH, 1, RGB(0, 0, 0)) )
					{
						CPen* pPenOld = dc.SelectObject( &pen );
						dc.MoveTo( rect.left, (rect.bottom - 1) );
						dc.LineTo( rect.right, (rect.bottom - 1) );
						dc.SelectObject( pPenOld );
					}
				}
			}

			// Draw the item
			rect.left  += 3;
			dc.DrawText( strName, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
			pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
						 &dc, 0, 0, SRCCOPY );

			// Clean up
			dc.SetBkMode( nBkModeOld );
			if( pFontOld )
			{
				dc.SelectObject( pFontOld );
				pFont->DeleteObject();
				delete pFont;
			}
			if( pBitmapOld )
			{
				dc.SelectObject( pBitmapOld );
			}
		}

		if( ::GetFocus() == GetSafeHwnd() )
		{
			if( (lpDrawItemStruct->itemAction & ODA_FOCUS)
			&&  (lpDrawItemStruct->itemState & ODS_FOCUS) )
			{
				InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
				lpDrawItemStruct->rcItem.bottom--;
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
				lpDrawItemStruct->rcItem.bottom++;
				InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::MeasureItem

void CSegmentListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pSongDlg->CreateFont();
		if( pFont )
		{
			pFontOld = pDC->SelectObject( pFont );
		}

		TEXTMETRIC tm;
		pDC->GetTextMetrics( &tm );
		lpMeasureItemStruct->itemHeight = tm.tmHeight + 2;
		
		if( pFontOld )
		{
			pDC->SelectObject( pFontOld );
			pFont->DeleteObject();
			delete pFont;
		}

		ReleaseDC( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnLButtonDown

void CSegmentListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	CListBox::OnLButtonDown( nFlags, point );

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );

	if( fOutside == TRUE
	||  nPos == LB_ERR )
	{
		SetSel( -1, FALSE );
		SetCaretIndex( -1, 0 ); 
		OnSelChange();
		SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnRButtonDown

void CSegmentListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );

	if( fOutside == FALSE
	&&  nPos != LB_ERR )
	{
		CRect rect;
		GetItemRect( nPos, &rect );
		if( rect.PtInRect( point ) )
		{
			// Cursor is in the nearest item
			if( GetSel( nPos ) == 0 )
			{
				// Cursor is in the nearest item
				SetSel( -1, FALSE ); 
				SetSel( nPos, TRUE ); 
			}
			SetCaretIndex( nPos, 0 ); 
			OnSelChange();
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnDblClk

void CSegmentListBox::OnDblClk() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pSongDlg->m_fOpenProperties = true;
	m_pSongDlg->OnViewProperties();
	m_pSongDlg->m_fOpenProperties = false;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::VKeyToItem

int CSegmentListBox::VKeyToItem( UINT nKey, UINT nIndex ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nKey )
	{
		case VK_RETURN:
			m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_VIRTUAL_SEGMENTS;
			m_pSongDlg->m_fOpenProperties = true;
			m_pSongDlg->OnViewProperties();
			m_pSongDlg->m_fOpenProperties = false;
			return -2;

		case VK_INSERT:
			m_pSongDlg->OnInsertVirtualSegment();
			return -2;

		case VK_DELETE:
			m_pSongDlg->OnDeleteVirtualSegments();
			return -2;
	}
	
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox::OnSelChange

void CSegmentListBox::OnSelChange( void ) 
{
	m_pSongDlg->RefreshVirtualSegmentControls();
	m_pSongDlg->RefreshProperties( IDC_LIST_VIRTUAL_SEGMENTS );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTrackListBox

CTrackListBox::CTrackListBox()
{
	m_pSongDlg = NULL;

	m_ptLastXYPos.x = INT_MAX;
	m_ptLastXYPos.y = INT_MAX;
}

CTrackListBox::~CTrackListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::SelectTrack

void CTrackListBox::SelectTrack( CTrack* pTrack, bool fSetFocus )
{
	ASSERT( pTrack != NULL );

	int nPos = -1;
	int nNbrItems = GetCount();
	for( int i = 0 ;  i < nNbrItems ;  i++ )
	{
		CTrack* pTrackList = (CTrack *)GetItemDataPtr( i );

		if( pTrackList == pTrack )
		{
			nPos = i;
			break;
		}
	}

	if( nPos != LB_ERR )
	{
		SetSel( -1, FALSE );
		SetSel( nPos, TRUE ); 
		SetCaretIndex( nPos, 0 ); 

		if( fSetFocus )
		{
			OnSelChange();
			SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::SelectTrackList

void CTrackListBox::SelectTrackList( CTypedPtrList<CPtrList, CTrack*>& list )
{
	SetSel( -1, FALSE );

	POSITION pos = list.GetHeadPosition();
	while( pos )
	{
		CTrack* pTrack = list.GetNext( pos );
	
		int nPos = FindStringExact( -1, (LPCTSTR)pTrack );
		if( nPos != LB_ERR )
		{
			SetSel( nPos, TRUE ); 
			SetCaretIndex( nPos, 0 ); 
		}
	}

	OnSelChange();
	SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::CreateDataObject

HRESULT CTrackListBox::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}
	*ppIDataObject = NULL;

	CVirtualSegment* pVirtualSegmentToEdit = m_pSongDlg->GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return E_UNEXPECTED;
	}

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	// Mark selected Tracks 
	m_pSongDlg->MarkSelectedTracks( pVirtualSegmentToEdit, BF_SELECTED );

	// Save selected Tracks
	IStream* pIStream;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AllocMemoryStream(FT_DESIGN, GUID_SaveSelectedTracks, &pIStream) ) )
	{
	    IDMUSProdRIFFStream* pIRiffStream;
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			if( SUCCEEDED ( pVirtualSegmentToEdit->Save( pIRiffStream ) ) )
			{
				// Place CF_TRACK_LIST into CDllJazzDataObject
				if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pSongComponent->m_cfTrackList, pIStream ) ) )
				{
					hr = S_OK;
				}
			}

			RELEASE( pIRiffStream );
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

	// Unmark selected Tracks
	m_pSongDlg->UnMarkTracks( pVirtualSegmentToEdit, BF_SELECTED );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox message handlers

BEGIN_MESSAGE_MAP(CTrackListBox, CListBox)
	//{{AFX_MSG_MAP(CTrackListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_VKEYTOITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnSetFocus

void CTrackListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_TRACKS;
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnKillFocus

void CTrackListBox::OnKillFocus( CWnd* pNewWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnKillFocus( pNewWnd );
	
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::DrawItem

void CTrackListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
	}

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
    ||  lpDrawItemStruct->itemAction & ODA_SELECT 
    ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
	{
		CTrack* pTrack = (CTrack *)lpDrawItemStruct->itemData; 
		ASSERT( pTrack != NULL );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = m_pSongDlg->CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == GetSafeHwnd() )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_INACTIVECAPTION) );
					dc.SetTextColor( ::GetSysColor(COLOR_INACTIVECAPTIONTEXT) );
				}
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Determine text
			CString strText;
			pTrack->FormatTextUI( strText );

			// Set horizontal extent
			TEXTMETRIC tm;
			dc.GetTextMetrics( &tm );
			CSize sizeText = dc.GetTextExtent( strText );
			sizeText.cx += tm.tmMaxCharWidth;
			if( sizeText.cx > GetHorizontalExtent() )
			{
				SetHorizontalExtent( sizeText.cx );
			}

			// Draw the item
			rect.left  += 3;
			dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
			pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
						 &dc, 0, 0, SRCCOPY );

			// Clean up
			dc.SetBkMode( nBkModeOld );
			if( pFontOld )
			{
				dc.SelectObject( pFontOld );
				pFont->DeleteObject();
				delete pFont;
			}
			if( pBitmapOld )
			{
				dc.SelectObject( pBitmapOld );
			}
		}

		if( ::GetFocus() == GetSafeHwnd() )
		{
			if( (lpDrawItemStruct->itemAction & ODA_FOCUS)
			&&  (lpDrawItemStruct->itemState & ODS_FOCUS) )
			{
				InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
				InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::MeasureItem

void CTrackListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pSongDlg->CreateFont();
		if( pFont )
		{
			pFontOld = pDC->SelectObject( pFont );
		}

		TEXTMETRIC tm;
		pDC->GetTextMetrics( &tm );
		lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;
		
		if( pFontOld )
		{
			pDC->SelectObject( pFontOld );
			pFont->DeleteObject();
			delete pFont;
		}

		ReleaseDC( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::CompareItem

int CTrackListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CTrack* pTrack1 = (CTrack *)lpCompareItemStruct->itemData1;
	CTrack* pTrack2 = (CTrack *)lpCompareItemStruct->itemData2;

	CString strName1;
	pTrack1->FormatFileNameText( strName1 );

	CString strName2;
	pTrack2->FormatFileNameText( strName2 );

	// If filenames are equal, sort by track index
	if( strName1.CompareNoCase( strName2 ) == 0 )
	{
		if( pTrack1->m_TrackUI.dwTrackIndex < pTrack2->m_TrackUI.dwTrackIndex )
		{
			return -1;
		}
		else if( pTrack1->m_TrackUI.dwTrackIndex > pTrack2->m_TrackUI.dwTrackIndex )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	// Sort by filename
	return strName1.CompareNoCase( strName2 );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnLButtonDown

void CTrackListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	CListBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnRButtonDown

void CTrackListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	SetFocus();

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );

	if( fOutside == FALSE
	&&  nPos != LB_ERR )
	{
		CRect rect;
		GetItemRect( nPos, &rect );
		if( rect.PtInRect( point ) )
		{
			// Cursor is in the nearest item
			if( GetSel( nPos ) == 0 )
			{
				// Cursor is in the nearest item
				SetSel( -1, FALSE ); 
				SetSel( nPos, TRUE ); 
			}
			SetCaretIndex( nPos, 0 ); 
			OnSelChange();
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnDblClk

void CTrackListBox::OnDblClk() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pSongDlg->m_fOpenProperties = true;
	m_pSongDlg->OnViewProperties();
	m_pSongDlg->m_fOpenProperties = false;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::VKeyToItem

int CTrackListBox::VKeyToItem( UINT nKey, UINT nIndex ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nKey )
	{
		case VK_RETURN:
			m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_TRACKS;
			m_pSongDlg->m_fOpenProperties = true;
			m_pSongDlg->OnViewProperties();
			m_pSongDlg->m_fOpenProperties = false;
			return -2;

		case VK_INSERT:
			m_pSongDlg->OnInsertTrack();
			return -2;

		case VK_DELETE:
			m_pSongDlg->OnDeleteTracks();
			return -2;
	}
	
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackListBox::OnSelChange

void CTrackListBox::OnSelChange( void ) 
{
	m_pSongDlg->RefreshProperties( IDC_LIST_TRACKS );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox

CTransitionListBox::CTransitionListBox()
{
	m_pSongDlg = NULL;

	m_ptLastXYPos.x = INT_MAX;
	m_ptLastXYPos.y = INT_MAX;
}

CTransitionListBox::~CTransitionListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::SelectTransition

void CTransitionListBox::SelectTransition( CTransition* pTransition, bool fSetFocus )
{
	ASSERT( pTransition != NULL );

	int nPos = -1;
	int nNbrItems = GetCount();
	for( int i = 0 ;  i < nNbrItems ;  i++ )
	{
		CTransition* pTransitionList = (CTransition *)GetItemDataPtr( i );

		if( pTransitionList == pTransition )
		{
			nPos = i;
			break;
		}
	}

	if( nPos != LB_ERR )
	{
		SetSel( -1, FALSE );
		SetSel( nPos, TRUE ); 
		SetCaretIndex( nPos, 0 ); 

		if( fSetFocus )
		{
			OnSelChange();
			SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::SelectTransitionList

void CTransitionListBox::SelectTransitionList( CTypedPtrList<CPtrList, CTransition*>& list )
{
	SetSel( -1, FALSE );

	POSITION pos = list.GetHeadPosition();
	while( pos )
	{
		CTransition* pTransition = list.GetNext( pos );
	
		int nPos = FindStringExact( -1, (LPCTSTR)pTransition );
		if( nPos != LB_ERR )
		{
			SetSel( nPos, TRUE ); 
			SetCaretIndex( nPos, 0 ); 
		}
	}

	OnSelChange();
	SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::CreateDataObject

HRESULT CTransitionListBox::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}
	*ppIDataObject = NULL;

	CVirtualSegment* pVirtualSegmentToEdit = m_pSongDlg->GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return E_UNEXPECTED;
	}

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	// Mark selected Transitions 
	m_pSongDlg->MarkSelectedTransitions( pVirtualSegmentToEdit, BF_SELECTED );

	// Save selected Transitions
	IStream* pIStream;
	if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->AllocMemoryStream(FT_DESIGN, GUID_SaveSelectedTransitions, &pIStream) ) )
	{
	    IDMUSProdRIFFStream* pIRiffStream;
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			if( SUCCEEDED ( pVirtualSegmentToEdit->Save( pIRiffStream ) ) )
			{
				// Place CF_TRANSITION_LIST into CDllJazzDataObject
				if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pSongComponent->m_cfTransitionList, pIStream ) ) )
				{
					hr = S_OK;
				}
			}

			RELEASE( pIRiffStream );
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

	// Unmark selected Transitions
	m_pSongDlg->UnMarkTransitions( pVirtualSegmentToEdit, BF_SELECTED );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox message handlers

BEGIN_MESSAGE_MAP(CTransitionListBox, CListBox)
	//{{AFX_MSG_MAP(CTransitionListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_VKEYTOITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnSetFocus

void CTransitionListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_TRANSITIONS;
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnKillFocus

void CTransitionListBox::OnKillFocus( CWnd* pNewWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnKillFocus( pNewWnd );
	
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::DrawItem

void CTransitionListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
	}

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
    ||  lpDrawItemStruct->itemAction & ODA_SELECT 
    ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
	{
		CTransition* pTransition = (CTransition *)lpDrawItemStruct->itemData; 
		ASSERT( pTransition != NULL );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = m_pSongDlg->CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == GetSafeHwnd() )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_INACTIVECAPTION) );
					dc.SetTextColor( ::GetSysColor(COLOR_INACTIVECAPTIONTEXT) );
				}
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Determine Text
			CString strText;
			pTransition->FormatTextUI( strText );

			// Set horizontal extent
			TEXTMETRIC tm;
			dc.GetTextMetrics( &tm );
			CSize sizeText = dc.GetTextExtent( strText );
			sizeText.cx += tm.tmMaxCharWidth;
			if( sizeText.cx > GetHorizontalExtent() )
			{
				SetHorizontalExtent( sizeText.cx );
			}

			// Draw Text
			rect.left += 3;
			dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
			pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
						 &dc, 0, 0, SRCCOPY );

			// Clean up
			dc.SetBkMode( nBkModeOld );
			if( pFontOld )
			{
				dc.SelectObject( pFontOld );
				pFont->DeleteObject();
				delete pFont;
			}
			if( pBitmapOld )
			{
				dc.SelectObject( pBitmapOld );
			}
		}

		if( ::GetFocus() == GetSafeHwnd() )
		{
			if( (lpDrawItemStruct->itemAction & ODA_FOCUS)
			&&  (lpDrawItemStruct->itemState & ODS_FOCUS) )
			{
				InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
				InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::MeasureItem

void CTransitionListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pSongDlg->CreateFont();
		if( pFont )
		{
			pFontOld = pDC->SelectObject( pFont );
		}

		TEXTMETRIC tm;
		pDC->GetTextMetrics( &tm );
		lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;
		
		if( pFontOld )
		{
			pDC->SelectObject( pFontOld );
			pFont->DeleteObject();
			delete pFont;
		}

		ReleaseDC( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnLButtonDown

void CTransitionListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	CListBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnRButtonDown

void CTransitionListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSongDlg != NULL );

	SetFocus();

	m_ptLastXYPos.x = point.x;
	m_ptLastXYPos.y = point.y;

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );

	if( fOutside == FALSE
	&&  nPos != LB_ERR )
	{
		CRect rect;
		GetItemRect( nPos, &rect );
		if( rect.PtInRect( point ) )
		{
			// Cursor is in the nearest item
			if( GetSel( nPos ) == 0 )
			{
				// Cursor is in the nearest item
				SetSel( -1, FALSE ); 
				SetSel( nPos, TRUE ); 
			}
			SetCaretIndex( nPos, 0 ); 
			OnSelChange();
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnDblClk

void CTransitionListBox::OnDblClk() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pSongDlg->m_fOpenProperties = true;
	m_pSongDlg->OnViewProperties();
	m_pSongDlg->m_fOpenProperties = false;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::VKeyToItem

int CTransitionListBox::VKeyToItem( UINT nKey, UINT nIndex ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nKey )
	{
		case VK_RETURN:
			m_pSongDlg->m_nEditMenuCtrlID = IDC_LIST_TRANSITIONS;
			m_pSongDlg->m_fOpenProperties = true;
			m_pSongDlg->OnViewProperties();
			m_pSongDlg->m_fOpenProperties = false;
			return -2;

		case VK_INSERT:
			m_pSongDlg->OnInsertTransition();
			return -2;

		case VK_DELETE:
			m_pSongDlg->OnDeleteTransitions();
			return -2;
	}
	
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox::OnSelChange

void CTransitionListBox::OnSelChange( void ) 
{
	m_pSongDlg->RefreshProperties( IDC_LIST_TRANSITIONS );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSongDlg

//IMPLEMENT_DYNCREATE(CSongDlg, CFormView)

CSongDlg::CSongDlg()
	: CFormView(CSongDlg::IDD)
{
	//{{AFX_DATA_INIT(CSongDlg)
	//}}AFX_DATA_INIT

	m_pSongCtrl = NULL;
	m_pSong = NULL;

	m_nEditMenuCtrlID = 0;

	m_pVirtualSegmentPropPageObject = NULL;
	m_pTrackPropPageObject = NULL;
	m_pTransitionPropPageObject = NULL;
	m_fOpenProperties = false;

	m_pIDataObject = NULL;
	m_nStartDragControlID = 0;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_pDragSong = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;

	m_pINodeRightMenu = NULL;
	m_pointRightMenu.x = -1;
	m_pointRightMenu.y = -1;

	m_dwMouseDownButton = 0;
	m_pointMouseDown.x = 0;
	m_pointMouseDown.y = 0;

	m_dwScrollTick = 0;
}

CSongDlg::~CSongDlg()
{
	RELEASE( m_pVirtualSegmentPropPageObject );
	RELEASE( m_pTrackPropPageObject );
	RELEASE( m_pTransitionPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::RefreshProperties

void CSongDlg::RefreshProperties( UINT nControlID )
{
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	IDMUSProdPropSheet* pIPropSheet;
	if( FAILED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return;
	}

	bool fOpenProperties = false;
	
	switch( nControlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( theApp.m_pIPageManager == NULL
			||  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_VirtualSegmentPropPageManager ) != S_OK )
			{
				fOpenProperties = true;
			}
			break;
		
		case IDC_LIST_TRACKS:
			if( theApp.m_pIPageManager == NULL
			||  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_TrackPropPageManager ) != S_OK )
			{
				fOpenProperties = true;
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( theApp.m_pIPageManager == NULL
			||  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_TransitionPropPageManager ) != S_OK )
			{
				fOpenProperties = true;
			}
			break;

		default:
			ASSERT( 0 );
			break;
	}

	if( fOpenProperties )
	{
		m_nEditMenuCtrlID = nControlID;
		OnViewProperties();
		CWnd* pWnd = GetDlgItem( nControlID );
		if( pWnd )
		{
			pWnd->SetFocus();
		}
	}
	else
	{
		pIPropSheet->RefreshTitle();
		theApp.m_pIPageManager->RefreshData();
	}

	RELEASE( pIPropSheet );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::SyncSongEditor

void CSongDlg::SyncSongEditor( DWORD dwFlags )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// SSE_ALL
	if( dwFlags == SSE_ALL )
	{
		FillVirtualSegmentListBox();
		FillVirtualSegmentComboBox();
		FillSourceSegmentComboBox();
		FillToolGraphComboBox();
		RefreshVirtualSegmentControls();
		return;
	}

	// SSE_SOURCE_SEGMENT_LIST
	if( dwFlags & SSE_SOURCE_SEGMENT_LIST )
	{
		FillSourceSegmentComboBox();
	}

	// SSE_VIRTUAL_SEGMENT_LIST
	if( dwFlags & SSE_VIRTUAL_SEGMENT_LIST )
	{
		FillVirtualSegmentComboBox();
		FillVirtualSegmentListBox();
		RefreshVirtualSegmentControls();
	}

	// SSE_REDRAW_VIRTUAL_SEGMENT_LIST
	if( dwFlags & SSE_REDRAW_VIRTUAL_SEGMENT_LIST )
	{
		CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
		if( pVirtualSegmentToEdit )
		{
			m_editName.SetWindowText( pVirtualSegmentToEdit->m_strName );
		}
		m_lstbxSegments.Invalidate();
		FillVirtualSegmentComboBox();
	}

	// SSE_INVALIDATE_VIRTUAL_SEGMENT_LIST
	if( dwFlags & SSE_INVALIDATE_VIRTUAL_SEGMENT_LIST )
	{
		m_lstbxSegments.Invalidate();
	}

	// SSE_TOOLGRAPH_LIST
	if( dwFlags & SSE_TOOLGRAPH_LIST )
	{
		FillToolGraphComboBox();
	}

	// SSE_TRANSITION_LIST
	if( dwFlags & SSE_TRANSITION_LIST )
	{
		CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
		if( pVirtualSegmentToEdit )
		{
			FillTransitionListBox( pVirtualSegmentToEdit );
		}
	}

	// SSE_REDRAW_TRANSITION_LIST
	if( dwFlags & SSE_REDRAW_TRANSITION_LIST )
	{
		m_lstbxTransitions.Invalidate();
	}

	// SSE_TRACK_LIST
	if( dwFlags & SSE_TRACK_LIST )
	{
		CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
		if( pVirtualSegmentToEdit )
		{
			FillTrackListBox( pVirtualSegmentToEdit );
		}
	}

	// SSE_REDRAW_TRACK_LIST
	if( dwFlags & SSE_REDRAW_TRACK_LIST )
	{
		m_lstbxTracks.Invalidate();
	}

	// SSE_SELECTED_VIRTUAL_SEGMENT 
	if( dwFlags & SSE_SELECTED_VIRTUAL_SEGMENT )
	{
		RefreshVirtualSegmentControls();
	}

	// SSE_EMPTY_ALL_LISTS
	if( dwFlags & SSE_EMPTY_ALL_LISTS )
	{
		m_lstbxSegments.ResetContent();
		m_lstbxTracks.ResetContent();
		m_lstbxTransitions.ResetContent();
		m_comboSegment.ResetContent();
		m_comboNextSegment.ResetContent();
		m_comboToolGraph.ResetContent();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::RefreshVirtualSegmentControls

void CSongDlg::RefreshVirtualSegmentControls( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_lstbxSegments.GetCount() > 0
	&&  m_lstbxSegments.GetSelCount() == 1 )
	{
		// One item selected in VirtualSegments list
		int nSelItem;
		m_lstbxSegments.GetSelItems( 1, &nSelItem );

		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( nSelItem );
	
		if( pVirtualSegment
		&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			EnableVirtualSegmentControls( TRUE );
		
			FillTrackListBox( pVirtualSegment );
			FillTransitionListBox( pVirtualSegment );

			m_editName.SetWindowText( pVirtualSegment->m_strName );
			SelectSourceSegmentComboBox( pVirtualSegment->m_pSourceSegment );
			SelectVirtualSegmentComboBox( pVirtualSegment->m_pNextVirtualSegment );
			SelectToolGraphComboBox( pVirtualSegment->m_pIToolGraphNode );
			return;
		}
	}

	// VirtualSegment list is empty OR
	// no VirtualSegments selected OR
	// multiple VirtualSegments selected
	EnableVirtualSegmentControls( FALSE );

	m_lstbxTracks.ResetContent();
	m_lstbxTransitions.ResetContent();
	m_comboSegment.SetCurSel( -1 );
	m_comboNextSegment.SetCurSel( -1 );
	m_comboToolGraph.SetCurSel( -1 );

	if( m_lstbxSegments.GetSelCount() > 1 )
	{
		CString strText;
		strText.LoadString( IDS_MULTIPLE_SEGMENTS_SELECTED );
		m_editName.SetWindowText( strText );
	}
	else
	{
		m_editName.SetWindowText( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::EnableVirtualSegmentControls

void CSongDlg::EnableVirtualSegmentControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_lstbxTracks.EnableWindow( fEnable );
	m_lstbxTransitions.EnableWindow( fEnable );

	m_editName.EnableWindow( fEnable );
	
	m_comboSegment.EnableWindow( fEnable );
	m_comboNextSegment.EnableWindow( fEnable );
	m_comboToolGraph.EnableWindow( fEnable );
}


void CSongDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSongDlg)
	DDX_Control(pDX, IDC_TRANSITION, m_btnTransition);
	DDX_Control(pDX, IDC_COMBO_TOOLGRAPH, m_comboToolGraph);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_COMBO_SEGMENT, m_comboSegment);
	DDX_Control(pDX, IDC_COMBO_NEXT_VIRTUAL_SEGMENT, m_comboNextSegment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSongDlg, CFormView)
	//{{AFX_MSG_MAP(CSongDlg)
	ON_COMMAND(IDM_DRAG_MOVE, OnDragRMenuMove)
	ON_COMMAND(IDM_DRAG_COPY, OnDragRMenuCopy)
	ON_COMMAND(IDM_DRAG_CANCEL, OnDragRMenuCancel)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_COMBO_NEXT_VIRTUAL_SEGMENT, OnSelChangeNextVirtualSegment)
	ON_CBN_SELCHANGE(IDC_COMBO_SEGMENT, OnSelChangeSourceSegment)
	ON_CBN_SELCHANGE(IDC_COMBO_TOOLGRAPH, OnSelChangeToolGraph)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillFocusEditName)
	ON_BN_CLICKED(IDC_TRANSITION, OnTransition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSongDlg diagnostics

#ifdef _DEBUG
void CSongDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CSongDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CSongDlg IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// IUknown CSongDlg::QueryInterface

HRESULT CSongDlg::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDropSource)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDropSource *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDropTarget) )
    {
        AddRef();
        *ppvObj = (IDropTarget *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CSongDlg::AddRef

ULONG CSongDlg::AddRef( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CSongDlg::Release

ULONG CSongDlg::Release( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    ASSERT( m_dwRef != 0 );	// m_dwRef should never get to zero.

//  if( m_dwRef == 0 )		   CSongDlg should be deleted when		
//  {						   control is destroyed. 						
//		delete this;		
//		return 0;
//  }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg IDropSource implementation

/////////////////////////////////////////////////////////////////////////////
// IDropSource CSongDlg::QueryContinueDrag

HRESULT CSongDlg::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton == MK_LBUTTON )
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

	if( m_dwStartDragButton == MK_RBUTTON )
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
// IDropSource CSongDlg::GiveFeedback

HRESULT CSongDlg::GiveFeedback( DWORD dwEffect )
{
	UNREFERENCED_PARAMETER( dwEffect );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CSongDlg IDropTarget implementation

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSongDlg::DragEnter

HRESULT CSongDlg::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pIDataObject == NULL );

	m_dwScrollTick = 0;

	// Store IDataObject associated with current drag-drop operation
	m_pIDataObject = pIDataObject;
	m_pIDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( GetDesktopWindow (), point );
	}

	// Make sure editor is on top
	if( m_pSong->m_hWndEditor )
	{
		::BringWindowToTop( m_pSong->m_hWndEditor );
	}

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSongDlg::DragOver

HRESULT CSongDlg::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );

	POINT point;
	point.x = pt.x;
	point.y = pt.y;

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	bool fMoveIsOK = false;
	if( CanPasteFromData( m_pIDataObject, true, point, &fMoveIsOK ) == S_OK )
	{
		if( fMoveIsOK == false )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else if( grfKeyState & MK_RBUTTON )
		{
			dwEffect = *pdwEffect;
		}
		else if( grfKeyState & MK_CONTROL )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else if( *pdwEffect & DROPEFFECT_COPY
			 &&  *pdwEffect & DROPEFFECT_MOVE )
		{
			dwEffect = DROPEFFECT_MOVE;
		}
		else
		{
			dwEffect = *pdwEffect;
		}
	}

	// Scroll
/* TODO
	{
		RECT rect;
		m_btnSongHeader.ScreenToClient( &point );
		m_btnSongHeader.GetClientRect( &rect );

		if( point.x < 0
		||  point.x > rect.right )
		{
			if( m_dwScrollTick == 0 )
			{
				m_dwScrollTick = GetTickCount() + 350;
			}
			else if( m_dwScrollTick < GetTickCount() )
			{
				if( point.x < 0 )
				{
					m_lstbxSong.SendMessage( WM_HSCROLL,
											  MAKEWPARAM( SB_THUMBPOSITION, max(0, m_lstbxSong.m_nHScrollPos - 30) ),
											  NULL );
				}
				else
				{
					int nExtent = m_lstbxSong.GetHorizontalExtent();
					m_lstbxSong.SendMessage( WM_HSCROLL,
											  MAKEWPARAM( SB_THUMBPOSITION, min(nExtent, m_lstbxSong.m_nHScrollPos + 30) ),
											  NULL );
				}

				m_dwScrollTick += 100;
			}
			
			dwEffect |= DROPEFFECT_SCROLL;
		}
		else
		{
			m_dwScrollTick = 0;
		}

		m_btnSongHeader.ClientToScreen( &point );
	}
*/

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
// IDropTarget CSongDlg::DragLeave

HRESULT CSongDlg::DragLeave( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_dwScrollTick = 0;

	// Release IDataObject
	RELEASE( m_pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CSongDlg::Drop

HRESULT CSongDlg::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER( grfKeyState );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );
	ASSERT( m_pIDataObject == pIDataObject );
	ASSERT( m_pSong != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	POINT point;
	point.x = pt.x;
	point.y = pt.y;
// TODO
/*	if( m_pDragSong == m_pSong )
	{
		if( m_nStartDragControlID == IDC_GRAPH_HEADER )
		{
			CTool* pToolStart = GetToolFromXPos( m_pointMouseDown.x );
			if( pToolStart )
			{
				m_btnSongHeader.ScreenToClient( &point );
				CTool* pToolEnd = GetToolFromXPos( point.x );
				m_btnSongHeader.ClientToScreen( &point );

				if( pToolStart == pToolEnd )
				{
					// Nothing to do
					DragLeave();
					return hr;
				}
			}
		}
	}
*/
	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		HMENU hMenu;
		HMENU hMenuPopup;
		
		// Display arrow cursor
		::LoadCursor( theApp.m_hInstance, IDC_ARROW );
	
		// Prepare context menu
		hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_DRAG_RMENU) );
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

			// Display and track menu
			CPoint point( pt.x, pt.y );
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			DestroyMenu( hMenu );

			// Need to process WM_COMMAND from TrackPopupMenu
			MSG msg;
			while( ::PeekMessage( &msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
			m_dwOverDragEffect = m_dwDragRMenuEffect;
			m_dwDragRMenuEffect = DROPEFFECT_NONE;
		}
	}

	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		// Paste data
		hr = PasteFromData( pIDataObject, true, point );
		if( SUCCEEDED ( hr ) )
		{
			*pdwEffect = m_dwOverDragEffect;
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CSongDlg::Create

BOOL CSongDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		return FALSE;
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnInitialUpdate

void CSongDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	CFormView::OnInitialUpdate();
	
	HRESULT hr = ::RegisterDragDrop( GetSafeHwnd(), (IDropTarget *)this );
	ASSERT(SUCCEEDED(hr));

	// Subclass controls
	m_lstbxSegments.SubclassDlgItem( IDC_LIST_VIRTUAL_SEGMENTS, this );
	m_lstbxSegments.m_pSongDlg = this;

	m_lstbxTracks.SubclassDlgItem( IDC_LIST_TRACKS, this );
	m_lstbxTracks.m_pSongDlg = this;

	m_lstbxTransitions.SubclassDlgItem( IDC_LIST_TRANSITIONS, this );
	m_lstbxTransitions.m_pSongDlg = this;

	// Set transition button bitmap

	// Load button bitmaps
	if( InterlockedIncrement( &sm_lBitmapRefCount ) == 1 )
	{
		if( sm_bmpTransition.GetSafeHandle() == NULL )
		{
			sm_bmpTransition.LoadBitmap( IDB_TRANSITION );
		}
	}
	if( sm_bmpTransition.GetSafeHandle() )
	{
		m_btnTransition.SetBitmap( (HBITMAP)sm_bmpTransition.GetSafeHandle() );
	}

	// Limit controls
	m_editName.SetLimitText( DMUS_MAX_NAME );

	// Populate all controls
	SyncSongEditor( SSE_ALL );

	// Set transport to this Song	
	m_pSong->SetActiveTransport();
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDestroy

void CSongDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = ::RevokeDragDrop( GetSafeHwnd() );
	ASSERT( SUCCEEDED( hr ) );

	if( InterlockedDecrement( &sm_lBitmapRefCount ) == 0 )
	{
		if( sm_bmpTransition.GetSafeHandle() != NULL )
		{
			sm_bmpTransition.DeleteObject();
		}
	}

	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDragRMenuMove

void CSongDlg::OnDragRMenuMove() 
{
	m_dwDragRMenuEffect = DROPEFFECT_MOVE;	
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDragRMenuCopy

void CSongDlg::OnDragRMenuCopy() 
{
	m_dwDragRMenuEffect = DROPEFFECT_COPY;	
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDragRMenuCancel

void CSongDlg::OnDragRMenuCancel() 
{
	m_dwDragRMenuEffect = DROPEFFECT_NONE;	
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnSize

void CSongDlg::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	CFormView::OnSize( nType, cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::DoDrag

void CSongDlg::DoDrag( UINT nID, DWORD dwStartDragButton )
{
	// If already dragging, just return
	if( m_pDragImage )
	{
		return;
	}

	IDataObject* pIDataObject = NULL;
	switch( nID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
// TODO		m_pSong->MarkSelectedTools( UD_DRAGSELECT );
			m_lstbxSegments.CreateDataObject( &pIDataObject );
			break;

		default:
			ASSERT( 0 );
			break;
	}

	if( pIDataObject )
	{
		DWORD dwEffect;
		HRESULT hr;

		// Create image used for drag-drop feedback
		if( m_pDragImage )
		{
			m_pDragImage->BeginDrag( 0, CPoint(8, 12) );
		}

		// Start drag-drop operation
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
// TODO	if( nID == IDC_GRAPH_HEADER )
//		{
//			dwOKDragEffects |= DROPEFFECT_MOVE;
//		}

		m_pDragSong = m_pSong;
		m_nStartDragControlID = nID;
		m_dwStartDragButton = dwStartDragButton;
		hr = ::DoDragDrop( pIDataObject, (IDropSource *)this, dwOKDragEffects, &dwEffect );
		m_pDragSong = NULL;
		m_nStartDragControlID = 0;
		m_dwStartDragButton = 0;
		m_pointMouseDown.x = 0;
		m_pointMouseDown.y = 0;

		// Delete image used for drag-drop feedback
		if( m_pDragImage )
		{
			m_pDragImage->EndDrag();

			delete m_pDragImage;
			m_pDragImage = NULL;
		}

// TODO
/*
		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					ASSERT( nID == IDC_GRAPH_HEADER );
					if( m_fDragToSameSong == false )
					{
						m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_TOOLS );
						m_pSong->DeleteMarkedTools( UD_DRAGSELECT );
						m_pSong->SetModified( TRUE );
					}
					m_pSong->Refresh();
				}
				else if( dwEffect & DROPEFFECT_COPY )
				{
					if( m_fDragToSameSong == false )
					{
						m_pSong->UnselectAllTools();
					}
					m_pSong->Refresh();
				}
				break;
		}
*/

// TODO		m_pSong->UnMarkTools( UD_DRAGSELECT ); 
			
		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CanPasteFromData

HRESULT CSongDlg::CanPasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt, bool* fMoveIsOK )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	*fMoveIsOK = true;

	if( fInDragDrop )
	{
		// Need to set m_nEditMenuCtrlID
		ScreenToClient( &pt );
		CWnd* pWnd = ChildWindowFromPoint( pt, CWP_ALL );
		if( pWnd )
		{
			m_nEditMenuCtrlID = pWnd->GetDlgCtrlID();
		}
		else
		{
			m_nEditMenuCtrlID = -1;
		}
		ClientToScreen( &pt );
	}

	HRESULT hr = E_FAIL;

	// Create a new CDllJazzDataObject and see if it contains a Song
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// See what control we are over
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSegment ) ) )
			{
				IDMUSProdNode* pIDocRootNode;
				if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
				{
					*fMoveIsOK = false;
					hr = S_OK;

					RELEASE( pIDocRootNode );
				}
			}
			else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfVirtualSegmentList ) ) )
			{
				if( IsDataObjectFromSameSong(pIDataObject) )
				{
					*fMoveIsOK = true;
					hr = S_OK;
				}
			}
			break;

		case IDC_LIST_TRACKS:
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfTrackList ) ) )
			{
				// We will paste into ALL selected virtual segments
				if( m_lstbxSegments.GetSelCount() > 0 )
				{
					if( IsDataObjectFromSameSong(pIDataObject) )
					{
						*fMoveIsOK = false;
						hr = S_OK;
					}
				}
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfTransitionList ) ) )
			{
				// We will paste into ALL selected virtual segments
				if( m_lstbxSegments.GetSelCount() > 0 )
				{
					if( IsDataObjectFromSameSong(pIDataObject) )
					{
						*fMoveIsOK = false;
						hr = S_OK;
					}
				}
			}
			break;

	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::PasteFromData

HRESULT CSongDlg::PasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	bool fMoveIsOK = false;
	if( CanPasteFromData( pIDataObject, fInDragDrop, pt, &fMoveIsOK ) != S_OK )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = E_FAIL;

	// See what control we are over
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
		{
			// Determine pt
			if( fInDragDrop )
			{
				// Drop
				m_lstbxSegments.ScreenToClient( &pt );
			}
			else
			{
				// Paste
				pt = m_lstbxSegments.m_ptLastXYPos;
			}

			// Handle CF_SEGMENT format
			if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSegment ) ) )
			{
				hr = CF_SEGMENT_PasteVirtualSegment( pDataObject, pIDataObject, fInDragDrop, pt );
			}

			// Handle CF_VIRTUAL_SEGMENT_LIST format
			else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfVirtualSegmentList ) ) )
			{
				hr = CF_VIRTUAL_SEGMENT_LIST_PasteVirtualSegment( pDataObject, pIDataObject, fInDragDrop, pt );
			}
			break;
		}

		case IDC_LIST_TRACKS:
		{
			// Determine pt
			if( fInDragDrop )
			{
				// Drop
				m_lstbxTracks.ScreenToClient( &pt );
			}
			else
			{
				// Paste
				pt = m_lstbxTracks.m_ptLastXYPos;
			}

			// Handle CF_TRACK_LIST format
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfTrackList ) ) )
			{
				hr = CF_TRACK_LIST_PasteTrack( pDataObject, pIDataObject, fInDragDrop, pt );
			}
			break;
		}

		case IDC_LIST_TRANSITIONS:
		{
			// Determine pt
			if( fInDragDrop )
			{
				// Drop
				m_lstbxTransitions.ScreenToClient( &pt );
			}
			else
			{
				// Paste
				pt = m_lstbxTransitions.m_ptLastXYPos;
			}

			// Handle CF_TRANSITION_LIST format
			if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfTransitionList ) ) )
			{
				hr = CF_TRANSITION_LIST_PasteTransition( pDataObject, pIDataObject, fInDragDrop, pt );
			}
			break;
		}

		default:
			// Should not happen!
			ASSERT( 0 );
			break;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CF_SEGMENT_PasteVirtualSegment

HRESULT CSongDlg::CF_SEGMENT_PasteVirtualSegment( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
	HRESULT hr = E_FAIL;

	// Create a VirtualSegment 
	CVirtualSegment* pVirtualSegment = new CVirtualSegment( m_pSong );
	if( pVirtualSegment )
	{
		IDMUSProdNode* pIDocRootNode;
		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
		{
			// Make sure the DocRoot is n the SourceSegment list
			if( m_pSong->IsSourceSegmentInSong( pIDocRootNode ) == FALSE )
			{
				m_pSong->m_FolderSegments.m_FolderReference.PasteFromData( pIDataObject );
			}

			// Get the stream containing the Segment
			IStream* pIStream;
			if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfSegment, &pIStream  ) ) )
			{
				LARGE_INTEGER liTemp;

				// Seek to beginning of stream
				liTemp.QuadPart = 0;
				pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

				// Initialize VirtualSegment
				if( SUCCEEDED ( pVirtualSegment->SetDefaultValues( pIStream ) ) )
				{
					// Set m_pSourceSegment
					pVirtualSegment->m_pSourceSegment = m_pSong->GetSourceSegmentFromDocRoot( pIDocRootNode );
					if( pVirtualSegment->m_pSourceSegment )
					{
						pVirtualSegment->m_pSourceSegment->AddRef();
					}

					// Find position in list
					int nPos;
					if( fInDragDrop )
					{
						// Drop
						BOOL fOutside;
						nPos = m_lstbxSegments.ItemFromPoint( pt, fOutside );
						if( fOutside == TRUE
						||  nPos == LB_ERR )
						{
							nPos = -1;
						}
					}
					else
					{
						// Paste
						BOOL fOutside;
						nPos = m_lstbxSegments.ItemFromPoint( m_lstbxSegments.m_ptLastXYPos, fOutside );
						if( fOutside == TRUE
						||  nPos == LB_ERR )
						{
							nPos = -1;
						}
					}

					// Save undo state
					if( fInDragDrop )
					{
						if( m_dwOverDragEffect == DROPEFFECT_MOVE 
						&&  m_pDragSong == m_pSong )
						{
							m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_MOVE_VIRTUAL_SEGMENTS );
// TODO						m_pSong->DeleteMarkedTools( UD_DRAGSELECT );
						}
						else
						{
							m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_VIRTUAL_SEGMENT );
						}
					}
					else
					{
						m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_VIRTUAL_SEGMENT );
					}

					// Insert the VirtualSegment into the Song
					m_pSong->InsertVirtualSegment( pVirtualSegment, nPos );

					// Determine "next" VirtualSegment
					if( nPos == -1 )
					{
						nPos = m_pSong->VirtualSegmentToIndex( pVirtualSegment );
					}
					CVirtualSegment* pPrevVirtualSegment = m_pSong->IndexToVirtualSegment( nPos - 1 );
					CVirtualSegment* pNextVirtualSegment = m_pSong->IndexToVirtualSegment( nPos + 1 );
					if( pPrevVirtualSegment )
					{
						if( pPrevVirtualSegment->m_pNextVirtualSegment
						&&	pPrevVirtualSegment->m_pNextVirtualSegment == pNextVirtualSegment )
						{
							RELEASE( pPrevVirtualSegment->m_pNextVirtualSegment );
							pPrevVirtualSegment->m_pNextVirtualSegment = pVirtualSegment;
							pPrevVirtualSegment->m_pNextVirtualSegment->AddRef();
						}
						else if( pPrevVirtualSegment->m_pNextVirtualSegment == NULL
							 &&  pNextVirtualSegment == NULL )
						{
							RELEASE( pPrevVirtualSegment->m_pNextVirtualSegment );
							pPrevVirtualSegment->m_pNextVirtualSegment = pVirtualSegment;
							pPrevVirtualSegment->m_pNextVirtualSegment->AddRef();
						}
					}
					if( pNextVirtualSegment )
					{
						RELEASE( pVirtualSegment->m_pNextVirtualSegment );
						pVirtualSegment->m_pNextVirtualSegment = pNextVirtualSegment;
						pVirtualSegment->m_pNextVirtualSegment->AddRef();
					}

					// Sync changes
					m_pSong->SetModified( TRUE );
					m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
										  SSE_VIRTUAL_SEGMENT_LIST,
										  NULL );

					// Select the VirtualSegment
					SelectVirtualSegment( pVirtualSegment );
					RefreshProperties( IDC_LIST_VIRTUAL_SEGMENTS );
					hr = S_OK;
				}

				RELEASE( pIStream );
			}

			RELEASE( pIDocRootNode );
		}
	}

	RELEASE( pVirtualSegment );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CF_VIRTUAL_SEGMENT_LIST_PasteVirtualSegment

HRESULT CSongDlg::CF_VIRTUAL_SEGMENT_LIST_PasteVirtualSegment( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
    IDMUSProdRIFFStream* pIRiffStream;
	IStream* pIStream;
    MMCKINFO ckMain;
	MMCKINFO ckList;
	MMCKINFO ck;
	int nThePos;

	// New VirtualSegments
	CTypedPtrList<CPtrList, CVirtualSegment*> list;

	// Find position in VirtualSegment list
	int nPos;
	BOOL fOutside;
	nPos = m_lstbxSegments.ItemFromPoint( pt, fOutside );
	if( fOutside == TRUE
	||  nPos == LB_ERR )
	{
		nPos = -1;
	}

	HRESULT hr = E_FAIL;

	// Get the stream containing the Segment
	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfVirtualSegmentList, &pIStream  ) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			// Seek to beginning of stream
			LARGE_INTEGER liTemp;
			liTemp.QuadPart = 0;
			pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

			ckMain.fccType = DMUS_FOURCC_SONG_FORM;
			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				ckList.fccType = DMUS_FOURCC_SEGREFS_LIST;
				if( pIRiffStream->Descend( &ckList, &ckMain, MMIO_FINDLIST ) == 0 )
				{
					while( pIRiffStream->Descend( &ck, &ckList, 0 ) == 0 )
					{
						switch( ck.ckid )
						{
							case FOURCC_LIST:
								switch( ck.fccType )
								{
									case DMUS_FOURCC_SEGREF_LIST:
									{
										CVirtualSegment* pVirtualSegment = new CVirtualSegment( m_pSong );
										if( pVirtualSegment == NULL )
										{
											hr = E_OUTOFMEMORY;
											break;
										}

										hr = pVirtualSegment->Load( pIRiffStream, &ck );
										if( FAILED ( hr ) )
										{
											delete pVirtualSegment;
											break;
										}

										// Keep track of the newly created Virtual Segment
										list.AddTail( pVirtualSegment );
									}
									break;
								}
						}
						
						if( FAILED ( hr ) )
						{
							break;
						}

					    pIRiffStream->Ascend( &ck, 0 );
					}
				}
			}

			RELEASE( pIRiffStream );
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) 
	&&  !list.IsEmpty() )
	{
		// Save undo state
		if( fInDragDrop )
		{
			if( m_dwOverDragEffect == DROPEFFECT_MOVE 
			&&  m_pDragSong == m_pSong )
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_MOVE_VIRTUAL_SEGMENTS );
// TODO			m_pSong->DeleteMarkedTools( UD_DRAGSELECT );
			}
			else
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_VIRTUAL_SEGMENT );
			}
		}
		else
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_VIRTUAL_SEGMENT );
		}

		POSITION pos = list.GetHeadPosition();
		while( pos )
		{
			CVirtualSegment* pVirtualSegment = list.GetNext( pos );

			// Insert the VirtualSegment into the Song
			m_pSong->InsertVirtualSegment( pVirtualSegment, nPos );

			// Determine "next" VirtualSegment
			nThePos = nPos;
			if( nThePos == -1 )
			{
				nThePos = m_pSong->VirtualSegmentToIndex( pVirtualSegment );
			}
			CVirtualSegment* pPrevVirtualSegment = m_pSong->IndexToVirtualSegment( nThePos - 1 );
			CVirtualSegment* pNextVirtualSegment = m_pSong->IndexToVirtualSegment( nThePos + 1 );
			if( pPrevVirtualSegment )
			{
				if( pPrevVirtualSegment->m_pNextVirtualSegment
				&&	pPrevVirtualSegment->m_pNextVirtualSegment == pNextVirtualSegment )
				{
					RELEASE( pPrevVirtualSegment->m_pNextVirtualSegment );
					pPrevVirtualSegment->m_pNextVirtualSegment = pVirtualSegment;
					pPrevVirtualSegment->m_pNextVirtualSegment->AddRef();
				}
				else if( pPrevVirtualSegment->m_pNextVirtualSegment == NULL
					 &&  pNextVirtualSegment == NULL )
				{
					RELEASE( pPrevVirtualSegment->m_pNextVirtualSegment );
					pPrevVirtualSegment->m_pNextVirtualSegment = pVirtualSegment;
					pPrevVirtualSegment->m_pNextVirtualSegment->AddRef();
				}
			}
			if( pNextVirtualSegment )
			{
				RELEASE( pVirtualSegment->m_pNextVirtualSegment );
				pVirtualSegment->m_pNextVirtualSegment = pNextVirtualSegment;
				pVirtualSegment->m_pNextVirtualSegment->AddRef();
			}

			// Increment nPos
			if( nPos >= 0 )
			{
				nPos++;
			}
		}

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_VIRTUAL_SEGMENT_LIST,
							  NULL );

		// Select the VirtualSegment(s)
		m_lstbxSegments.SelectVirtualSegmentList( list );

		RefreshProperties( IDC_LIST_VIRTUAL_SEGMENTS );
	}

	// Cleanup
	while( !list.IsEmpty() )
	{
		CVirtualSegment* pVirtualSegment = list.RemoveHead();

		RELEASE( pVirtualSegment );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CF_TRANSITION_LIST_PasteTransition

HRESULT CSongDlg::CF_TRANSITION_LIST_PasteTransition( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
    IDMUSProdRIFFStream* pIRiffStream;
	IStream* pIStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;

	// We will paste into ALL selected virtual segments
	int nNbrSelItems = m_lstbxSegments.GetSelCount();
	if( nNbrSelItems <= 0 )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// New Transitions
	CTypedPtrList<CPtrList, CTransition*> list;

	HRESULT hr = E_FAIL;

	// Get the stream containing the Transitions
	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfTransitionList, &pIStream  ) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			// Seek to beginning of stream
			LARGE_INTEGER liTemp;
			liTemp.QuadPart = 0;
			pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

			ckMain.fccType = DMUS_FOURCC_SEGREF_LIST;
			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = DMUS_FOURCC_TRANSITION_COPY_PASTE_UI_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ioTransitionCopyPasteUI iTransitionCopyPasteUI;
					CTransition* pTransition;

					dwSize = ck.cksize;
					while( dwSize > 0 )
					{
						hr = pIStream->Read( &iTransitionCopyPasteUI, sizeof(ioTransitionCopyPasteUI), &dwByteCount );
						if( FAILED( hr )
						||  dwByteCount != sizeof(ioTransitionCopyPasteUI) )
						{
							hr = E_FAIL;
							break;
						}

						pTransition = new CTransition( m_pSong );
						if( pTransition == NULL )
						{
							hr = E_OUTOFMEMORY;
							break;
						}

						pTransition->m_dwToSegmentFlag = iTransitionCopyPasteUI.dwToSegmentFlag;
						pTransition->m_pToSegment = m_pSong->PtrToVirtualSegment( iTransitionCopyPasteUI.pToSegment );
						if( pTransition->m_pToSegment )
						{
							ASSERT( pTransition->m_dwToSegmentFlag == 0 ); 
							pTransition->m_pToSegment->AddRef();
						}
						else
						{
							// No "To Segment" so we must have a value in m_dwToSegmentFlag
							if( pTransition->m_dwToSegmentFlag == 0 )
							{
								pTransition->m_dwToSegmentFlag = DMUS_SONG_NOSEG;
							}
						}
						pTransition->m_pTransitionSegment = m_pSong->PtrToVirtualSegment( iTransitionCopyPasteUI.pTransitionSegment );
						if( pTransition->m_pTransitionSegment )
						{
							pTransition->m_pTransitionSegment->AddRef();
						}
						pTransition->m_dwPlayFlagsDM = iTransitionCopyPasteUI.dwPlayFlagsDM;
						list.AddTail( pTransition );

						dwSize -= dwByteCount;
					}
				}
			}

			RELEASE( pIRiffStream );
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) 
	&&  !list.IsEmpty() )
	{
		// Save undo state
		if( fInDragDrop )
		{
			if( m_dwOverDragEffect == DROPEFFECT_MOVE 
			&&  m_pDragSong == m_pSong )
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_MOVE_TRANSITIONS );
// TODO			m_pSong->DeleteMarkedTools( UD_DRAGSELECT );
			}
			else
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRANSITION );
			}
		}
		else
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRANSITION );
		}

		// Insert Transitions into the selected VirtualSegments
		int* pnSelItems = new int[nNbrSelItems];
		if( pnSelItems )
		{
			m_lstbxSegments.GetSelItems( nNbrSelItems, pnSelItems );

			for( int i = 0;  i < nNbrSelItems ;  i++ )
			{
				CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( pnSelItems[i] );
			
				if( pVirtualSegment
				&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
				{
					POSITION pos = list.GetHeadPosition();
					while( pos )
					{
						CTransition* pTransition = list.GetNext( pos );

						if( i == 0 )
						{
							// Insert the Transition into the VirtualSegment
							pVirtualSegment->InsertTransition( pTransition );
						}
						else
						{
							// Insert a copy of the Transition into the VirtualSegment
							CTransition* pTransitionDupe = new CTransition( m_pSong );
							if( pTransitionDupe )
							{
								pTransitionDupe->Copy( pTransition );
								pVirtualSegment->InsertTransition( pTransitionDupe );
								RELEASE( pTransitionDupe );
							}
						}
					}
				}
			}

			delete [] pnSelItems;
		}

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_TRANSITION_LIST,
							  NULL );

		// Select the Transition(s)
		m_lstbxTransitions.SelectTransitionList( list );

		RefreshProperties( IDC_LIST_TRANSITIONS );
	}

	// Cleanup
	while( !list.IsEmpty() )
	{
		CTransition* pTransition = list.RemoveHead();

		RELEASE( pTransition );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CF_TRACK_LIST_PasteTrack

HRESULT CSongDlg::CF_TRACK_LIST_PasteTrack( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
    IDMUSProdRIFFStream* pIRiffStream;
	IStream* pIStream;
    MMCKINFO ckMain;
	MMCKINFO ckList;
	MMCKINFO ck;

	// We will paste into ALL selected virtual segments
	int nNbrSelItems = m_lstbxSegments.GetSelCount();
	if( nNbrSelItems <= 0 )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// New Tracks
	CTypedPtrList<CPtrList, CTrack*> list;

	HRESULT hr = E_FAIL;

	// Get the stream containing the Tracks
	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfTrackList, &pIStream  ) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			// Seek to beginning of stream
			LARGE_INTEGER liTemp;
			liTemp.QuadPart = 0;
			pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

			ckMain.fccType = DMUS_FOURCC_SEGREF_LIST;
			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
			{
				ckList.fccType = DMUS_FOURCC_TRACKREFS_LIST;
				if( pIRiffStream->Descend( &ckList, &ckMain, MMIO_FINDLIST ) == 0 )
				{
					while( pIRiffStream->Descend( &ck, &ckList, 0 ) == 0 )
					{
						switch( ck.ckid )
						{
							case FOURCC_LIST:
								switch( ck.fccType )
								{
									case DMUS_FOURCC_TRACKREF_LIST:
									{
										CTrack* pTrack = new CTrack( m_pSong );
										if( pTrack == NULL )
										{
											hr = E_OUTOFMEMORY;
											break;
										}

										hr = pTrack->Load( pIRiffStream, &ck );
										if( hr == S_OK )
										{
											// Keep track of the newly created Track
											list.AddTail( pTrack );
										}
										else if( hr == S_FALSE )
										{
											// Could not resolve SourceSegment so discard this track
											RELEASE( pTrack );
											hr = S_OK;
										}
										else
										{
											RELEASE( pTrack );
										}
									}
									break;
								}
						}
						
						if( FAILED ( hr ) )
						{
							break;
						}

					    pIRiffStream->Ascend( &ck, 0 );
					}
				}
			}

			RELEASE( pIRiffStream );
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) 
	&&  !list.IsEmpty() )
	{
		// Save undo state
		if( fInDragDrop )
		{
			if( m_dwOverDragEffect == DROPEFFECT_MOVE 
			&&  m_pDragSong == m_pSong )
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_MOVE_TRACKS );
// TODO			m_pSong->DeleteMarkedTools( UD_DRAGSELECT );
			}
			else
			{
				m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRACK );
			}
		}
		else
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRACK );
		}

		// Insert Tracks into the selected VirtualSegments
		int* pnSelItems = new int[nNbrSelItems];
		if( pnSelItems )
		{
			m_lstbxSegments.GetSelItems( nNbrSelItems, pnSelItems );

			for( int i = 0;  i < nNbrSelItems ;  i++ )
			{
				CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( pnSelItems[i] );
			
				if( pVirtualSegment
				&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
				{
					POSITION pos = list.GetHeadPosition();
					while( pos )
					{
						CTrack* pTrack = list.GetNext( pos );

						if( i == 0 )
						{
							// Insert the Track into the VirtualSegment
							pVirtualSegment->InsertTrack( pTrack );
						}
						else
						{
							// Insert a copy of the Track into the VirtualSegment
							CTrack* pTrackDupe = new CTrack( m_pSong );
							if( pTrackDupe )
							{
								pTrackDupe->Copy( pTrack );
								pVirtualSegment->InsertTrack( pTrackDupe );
								RELEASE( pTrackDupe );
							}
						}
					}
				}
			}

			delete [] pnSelItems;
		}

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_TRACK_LIST,
							  NULL );

		// Select the Track(s)
		m_lstbxTracks.SelectTrackList( list );

		RefreshProperties( IDC_LIST_TRACKS );
	}

	// Cleanup
	while( !list.IsEmpty() )
	{
		CTrack* pTrack = list.RemoveHead();

		RELEASE( pTrack );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetVirtualSegmentToEdit

CVirtualSegment* CSongDlg::GetVirtualSegmentToEdit( void ) 
{
	if( m_lstbxSegments.GetCount() > 0
	&&  m_lstbxSegments.GetSelCount() == 1 )
	{
		// One item selected in VirtualSegments list
		int nSelItem;
		m_lstbxSegments.GetSelItems( 1, &nSelItem );

		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( nSelItem );
	
		if( pVirtualSegment
		&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			return pVirtualSegment;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetTrackToEdit

CTrack* CSongDlg::GetTrackToEdit( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return NULL;
	}

	if( m_lstbxTracks.GetCount() > 0
	&&  m_lstbxTracks.GetSelCount() == 1 )
	{
		// One item selected in Tracks list
		int nSelItem;
		m_lstbxTracks.GetSelItems( 1, &nSelItem );

		CTrack* pTrack = (CTrack *)m_lstbxTracks.GetItemDataPtr( nSelItem );
	
		if( pTrack
		&&  pTrack != (CTrack *)0xFFFFFFFF )
		{
			return pTrack;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetTransitionToEdit

CTransition* CSongDlg::GetTransitionToEdit( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return NULL;
	}

	if( m_lstbxTransitions.GetCount() > 0
	&&  m_lstbxTransitions.GetSelCount() == 1 )
	{
		// One item selected in Transitions list
		int nSelItem;
		m_lstbxTransitions.GetSelItems( 1, &nSelItem );

		CTransition* pTransition = (CTransition *)m_lstbxTransitions.GetItemDataPtr( nSelItem );
	
		if( pTransition
		&&  pTransition != (CTransition *)0xFFFFFFFF )
		{
			return pTransition;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetVirtualSegmentSelCount

int CSongDlg::GetVirtualSegmentSelCount( void ) 
{
	if( m_lstbxSegments.GetCount() > 0 )
	{
		return m_lstbxSegments.GetSelCount();
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetTrackSelCount

int CSongDlg::GetTrackSelCount( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return 0;
	}

	if( m_lstbxTracks.GetCount() > 0 )
	{
		return m_lstbxTracks.GetSelCount();
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetTransitionSelCount

int CSongDlg::GetTransitionSelCount( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return 0;
	}

	if( m_lstbxTransitions.GetCount() > 0 )
	{
		return m_lstbxTransitions.GetSelCount();
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnKillFocusEditName

void CSongDlg::OnKillFocusEditName( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		return;
	}

	CString strName;
	m_editName.GetWindowText( strName );

	// Strip leading and trailing spaces
	strName.TrimRight();
	strName.TrimLeft();

	if( strName.Compare( pVirtualSegmentToEdit->m_strName ) != 0 )
	{
		CString strOrigName = pVirtualSegmentToEdit->m_strName;
		pVirtualSegmentToEdit->m_strName = strName;
		BOOL fUnique = m_pSong->IsVirtualSegmentNameUnique( pVirtualSegmentToEdit );
		pVirtualSegmentToEdit->m_strName = strOrigName;

		if( fUnique == FALSE )
		{
			CString strMsg;
			AfxFormatString1( strMsg, IDS_DUPE_VIRTUAL_SEGMENT_NAME, strName );
			AfxMessageBox( strMsg, MB_OK );

			m_editName.SetWindowText( pVirtualSegmentToEdit->m_strName );
			m_editName.SetSel( 0, -1 );
			m_editName.SetFocus();
		}
		else
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_CHANGE_VIRTUAL_SEGMENT_NAME );

			pVirtualSegmentToEdit->m_strName = strName;

			m_pSong->SetModified( TRUE );
			m_pSong->SyncChanges( SC_EDITOR | SC_PROPERTIES | SC_DIRECTMUSIC,
								  SSE_REDRAW_VIRTUAL_SEGMENT_LIST | SSE_REDRAW_TRANSITION_LIST,
								  NULL );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnSelChangeNextVirtualSegment

void CSongDlg::OnSelChangeNextVirtualSegment( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	int nCurSel = m_comboNextSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_comboNextSegment.GetItemDataPtr( nCurSel );

		if( pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_CHANGE_NEXT_SEGMENT );

			RELEASE( pVirtualSegmentToEdit->m_pNextVirtualSegment );
			pVirtualSegmentToEdit->m_pNextVirtualSegment = pVirtualSegment;

			// pVirtualSegmentToEdit->m_pNextVirtualSegment equals NULL when "<None>" selected
			if( pVirtualSegmentToEdit->m_pNextVirtualSegment )
			{
				pVirtualSegmentToEdit->m_pNextVirtualSegment->AddRef();
			}

			m_pSong->SetModified( TRUE );
			m_pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );
		}
	}

	// Need to draw solid/dash lines
	m_lstbxSegments.Invalidate();	
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnSelChangeSourceSegment

void CSongDlg::OnSelChangeSourceSegment( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	int nCurSel = m_comboSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		CSourceSegment* pSourceSegment = (CSourceSegment *)m_comboSegment.GetItemDataPtr( nCurSel );

		if( pSourceSegment != (CSourceSegment *)0xFFFFFFFF )
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_CHANGE_SOURCE_SEGMENT );

			RELEASE( pVirtualSegmentToEdit->m_pSourceSegment );
			pVirtualSegmentToEdit->m_pSourceSegment = pSourceSegment;

			// pVirtualSegmentToEdit->m_pSourceSegment equals NULL when "<None>" selected
			if( pVirtualSegmentToEdit->m_pSourceSegment )
			{
				pVirtualSegmentToEdit->m_pSourceSegment->AddRef();
			}

			m_pSong->SetModified( TRUE );
			m_pSong->SyncChanges( SC_PROPERTIES | SC_DIRECTMUSIC,
								  0,
								  NULL );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnSelChangeToolGraph

void CSongDlg::OnSelChangeToolGraph( void ) 
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	int nCurSel = m_comboToolGraph.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		IDMUSProdNode* pIToolGraphNode = (IDMUSProdNode *)m_comboToolGraph.GetItemDataPtr( nCurSel );

		if( pIToolGraphNode != (IDMUSProdNode *)0xFFFFFFFF )
		{
			m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_CHANGE_SOURCE_TOOLGRAPH );

			RELEASE( pVirtualSegmentToEdit->m_pIToolGraphNode );
			pVirtualSegmentToEdit->m_pIToolGraphNode = pIToolGraphNode;

			// pVirtualSegmentToEdit->m_pIToolGraphNode equals NULL when "<None>" selected
			if( pVirtualSegmentToEdit->m_pIToolGraphNode )
			{
				pVirtualSegmentToEdit->m_pIToolGraphNode->AddRef();
			}

			m_pSong->SetModified( TRUE );
			m_pSong->SyncChanges( SC_DIRECTMUSIC, 0, NULL );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::SelectSourceSegmentComboBox

void CSongDlg::SelectSourceSegmentComboBox( CSourceSegment* pSourceSegment )
{
	int nNbrItems = m_comboSegment.GetCount();

	for( int nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		CSourceSegment* pSourceSegmentList = (CSourceSegment *)m_comboSegment.GetItemDataPtr( nPos );

		if( pSourceSegmentList == pSourceSegment )
		{
			m_comboSegment.SetCurSel( nPos );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::SelectVirtualSegmentComboBox

void CSongDlg::SelectVirtualSegmentComboBox( CVirtualSegment* pVirtualSegment )
{
	int nNbrItems = m_comboNextSegment.GetCount();

	for( int nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		CVirtualSegment* pVirtualSegmentList = (CVirtualSegment *)m_comboNextSegment.GetItemDataPtr( nPos );

		if( pVirtualSegmentList == pVirtualSegment )
		{
			m_comboNextSegment.SetCurSel( nPos );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::SelectToolGraphComboBox

void CSongDlg::SelectToolGraphComboBox( IDMUSProdNode* pIToolGraphNode )
{
	int nNbrItems = m_comboToolGraph.GetCount();

	for( int nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		IDMUSProdNode* pIToolGraphNodeList = (IDMUSProdNode *)m_comboToolGraph.GetItemDataPtr( nPos );

		if( pIToolGraphNodeList == pIToolGraphNode )
		{
			m_comboToolGraph.SetCurSel( nPos );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillSourceSegmentComboBox

void CSongDlg::FillSourceSegmentComboBox( void )
{
	CSourceSegment* pCurSelSourceSegment = NULL;

	// Get selection
	int nCurSel = m_comboSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		pCurSelSourceSegment = (CSourceSegment *)m_comboSegment.GetItemDataPtr( nCurSel );
	}

	// Empty combo box
	m_comboSegment.ResetContent();

	// Add "<None>"
	CString strNone;
	strNone.LoadString( IDS_NONE_TEXT );
	int nPos = m_comboSegment.AddString( strNone  );
	m_comboSegment.SetItemDataPtr( nPos, NULL );

	// Add SourceSegments
	CSourceSegment* pSourceSegment;
	for( int i = 0 ;  pSourceSegment = m_pSong->IndexToSourceSegment(i) ;  i++ )
	{
		if( pSourceSegment == NULL )
		{
			break;
		}

		BSTR bstrName;
		pSourceSegment->GetNodeName( &bstrName );
		CString strName = bstrName;
		::SysFreeString( bstrName );

		nPos = m_comboSegment.AddString( strName );
		m_comboSegment.SetItemDataPtr( nPos, pSourceSegment );
	}

	// Restore selection
	if( pCurSelSourceSegment != (CSourceSegment *)0xFFFFFFFF )
	{
		SelectSourceSegmentComboBox( pCurSelSourceSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillVirtualSegmentComboBox

void CSongDlg::FillVirtualSegmentComboBox( void )
{
	CVirtualSegment* pCurSelVirtualSegment = NULL;

	// Get selection
	int nCurSel = m_comboNextSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		 pCurSelVirtualSegment = (CVirtualSegment *)m_comboNextSegment.GetItemDataPtr( nCurSel );
	}

	// Empty combo box
	m_comboNextSegment.ResetContent();

	// Add "<None>"
	CString strNone;
	strNone.LoadString( IDS_NONE_TEXT );
	int nPos = m_comboNextSegment.AddString( strNone  );
	m_comboNextSegment.SetItemDataPtr( nPos, NULL );

	// Add VirtualSegments
	POSITION pos = m_pSong->m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{ 
		CVirtualSegment* pVirtualSegment = m_pSong->m_lstVirtualSegments.GetNext( pos );

		nPos = m_comboNextSegment.AddString( pVirtualSegment->m_strName  );
		m_comboNextSegment.SetItemDataPtr( nPos, pVirtualSegment );
	}

	// Restore selection
	if( pCurSelVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
	{
		SelectVirtualSegmentComboBox( pCurSelVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillToolGraphComboBox

void CSongDlg::FillToolGraphComboBox( void )
{
	IDMUSProdNode* pICurSelToolGraphNode = NULL;

	// Get Current selection
	int nCurSel = m_comboToolGraph.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		pICurSelToolGraphNode = (IDMUSProdNode *)m_comboToolGraph.GetItemDataPtr( nCurSel );
	}

	// Empty combo box
	m_comboToolGraph.ResetContent();

	// Add "<None>"
	CString strNone;
	strNone.LoadString( IDS_NONE_TEXT );
	int nPos = m_comboToolGraph.AddString( strNone  );
	m_comboToolGraph.SetItemDataPtr( nPos, NULL );

	// Add ToolGraphs
	IDMUSProdNode* pIToolGraphNode;
	for( int i = 0 ;  pIToolGraphNode = m_pSong->IndexToToolGraph(i) ;  i++ )
	{
		if( pIToolGraphNode == NULL )
		{
			break;
		}

		BSTR bstrName;
		pIToolGraphNode->GetNodeName( &bstrName );
		CString strName = bstrName;
		::SysFreeString( bstrName );

		nPos = m_comboToolGraph.AddString( strName );
		m_comboToolGraph.SetItemDataPtr( nPos, pIToolGraphNode );
	}

	// Set selection
	if( pICurSelToolGraphNode != (IDMUSProdNode *)0xFFFFFFFF )
	{
		SelectToolGraphComboBox( pICurSelToolGraphNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillVirtualSegmentListBox

void CSongDlg::FillVirtualSegmentListBox( void )
{
	CVirtualSegment* pCurSelVirtualSegment = NULL;

	// Get selection
	int nCurSel = m_lstbxSegments.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		 pCurSelVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( nCurSel );
	}

	// Empty list box
	m_lstbxSegments.ResetContent();
	m_lstbxSegments.SetHorizontalExtent( 0 );

	// Add VirtualSegments
	POSITION pos = m_pSong->m_lstVirtualSegments.GetHeadPosition();
	while( pos )
	{ 
		CVirtualSegment* pVirtualSegment = m_pSong->m_lstVirtualSegments.GetNext( pos );

		int nPos = m_lstbxSegments.AddString( _T( "" )  );
		m_lstbxSegments.SetItemDataPtr( nPos, pVirtualSegment );
	}

	// Restore selection
	if( pCurSelVirtualSegment
	&&  pCurSelVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
	{
		m_lstbxSegments.SelectVirtualSegment( pCurSelVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillTrackListBox

void CSongDlg::FillTrackListBox( CVirtualSegment* pVirtualSegment )
{
	CTrack* pCurSelTrack = NULL;

	// Get selection
	int nCurSel = m_lstbxTracks.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		 pCurSelTrack = (CTrack *)m_lstbxTracks.GetItemDataPtr( nCurSel );
	}

	// Empty list box
	m_lstbxTracks.ResetContent();
	m_lstbxTracks.SetHorizontalExtent( 0 );

	// Add Tracks
	POSITION pos = pVirtualSegment->m_lstTracks.GetHeadPosition();
	while( pos )
	{ 
		CTrack* pTrack = pVirtualSegment->m_lstTracks.GetNext( pos );

		int nPos = m_lstbxTracks.AddString( (LPCTSTR)pTrack  );
	}

	// Restore selection
	if( pCurSelTrack
	&&  pCurSelTrack != (CTrack *)0xFFFFFFFF )
	{
		m_lstbxTracks.SelectTrack( pCurSelTrack, false );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::FillTransitionListBox

void CSongDlg::FillTransitionListBox( CVirtualSegment* pVirtualSegment )
{
	CTransition* pCurSelTransition = NULL;

	// Get selection
	int nCurSel = m_lstbxTransitions.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		 pCurSelTransition = (CTransition *)m_lstbxTransitions.GetItemDataPtr( nCurSel );
	}

	// Empty list box
	m_lstbxTransitions.ResetContent();
	m_lstbxTransitions.SetHorizontalExtent( 0 );

	// Add Transitions
	POSITION pos = pVirtualSegment->m_lstTransitions.GetHeadPosition();
	while( pos )
	{ 
		CTransition* pTransition = pVirtualSegment->m_lstTransitions.GetNext( pos );

		int nPos = m_lstbxTransitions.AddString( _T( "" )  );
		m_lstbxTransitions.SetItemDataPtr( nPos, pTransition );
	}

	// Restore selection
	if( pCurSelTransition
	&&  pCurSelTransition != (CTransition *)0xFFFFFFFF )
	{
		m_lstbxTransitions.SelectTransition( pCurSelTransition, false );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::CreateFont

CFont* CSongDlg::CreateFont( void )
{
	CFont* pFont = new CFont;

	if( pFont 
	&&  pFont->CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 						   DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
						   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
	{
		return pFont;
	}

	if( pFont )
	{
		delete pFont;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::SelectVirtualSegment

void CSongDlg::SelectVirtualSegment( CVirtualSegment* pVirtualSegment )
{
	// Select the VirtualSegment
	m_lstbxSegments.SelectVirtualSegment( pVirtualSegment );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::GetFirstSelectedVirtualSegment

CVirtualSegment* CSongDlg::GetFirstSelectedVirtualSegment( void )
{
	if( m_lstbxSegments.GetCount() > 0 )
	{
		// Get the first item selected in VirtualSegments list
		int nSelItem;
		m_lstbxSegments.GetSelItems( 1, &nSelItem );

		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( nSelItem );
	
		if( pVirtualSegment
		&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			return pVirtualSegment;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnUpdateEditCut

void CSongDlg::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	BOOL fEnable = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( m_lstbxSegments.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
		
		case IDC_LIST_TRACKS:
			if( m_lstbxTracks.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( m_lstbxTransitions.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditCut

void CSongDlg::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
		case IDC_LIST_TRACKS:
		case IDC_LIST_TRANSITIONS:
			OnEditCopy();
			OnEditDelete();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnUpdateEditCopy

void CSongDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	BOOL fEnable = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( m_lstbxSegments.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
		
		case IDC_LIST_TRACKS:
			if( m_lstbxTracks.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( m_lstbxTransitions.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditCopy

void CSongDlg::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( m_lstbxSegments.GetSelCount() > 0 )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( m_lstbxSegments.CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, m_pSong );

					RELEASE( pIDataObject );
				}
			}
			break;
		
		case IDC_LIST_TRACKS:
			if( m_lstbxTracks.GetSelCount() > 0 )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( m_lstbxTracks.CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, m_pSong );

					RELEASE( pIDataObject );
				}
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( m_lstbxTransitions.GetSelCount() > 0 )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( m_lstbxTransitions.CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, m_pSong );

					RELEASE( pIDataObject );
				}
			}
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnUpdateEditPaste

void CSongDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL fEnable = FALSE;

	// Get the IDataObject
	IDataObject* pIDataObject;
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		POINT pt = {0, 0};
		bool fMoveIsOK = false;
		HRESULT hr = CanPasteFromData( pIDataObject, false, pt, &fMoveIsOK );
		
		RELEASE( pIDataObject );
		
		if( hr == S_OK )
		{
			fEnable = TRUE;
		}
	}
	
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditPaste

void CSongDlg::OnEditPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the IDataObject
	IDataObject* pIDataObject;
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		POINT pt = {0, 0};
		PasteFromData( pIDataObject, false, pt );
		
		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnUpdateEditInsert

void CSongDlg::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	BOOL fEnable = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			fEnable = TRUE;
			break;

		case IDC_LIST_TRACKS:
		case IDC_LIST_TRANSITIONS:
			if( GetVirtualSegmentToEdit() )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditInsert

void CSongDlg::OnEditInsert() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			OnInsertVirtualSegment();
			break;

		case IDC_LIST_TRACKS:
			OnInsertTrack();
			break;
		
		case IDC_LIST_TRANSITIONS:
			OnInsertTransition();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnUpdateEditDelete

void CSongDlg::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	BOOL fEnable = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			if( m_lstbxSegments.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
		
		case IDC_LIST_TRACKS:
			if( m_lstbxTracks.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;

		case IDC_LIST_TRANSITIONS:
			if( m_lstbxTransitions.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditDelete

void CSongDlg::OnEditDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			OnDeleteVirtualSegments();
			break;

		case IDC_LIST_TRACKS:
			OnDeleteTracks();
			break;
		
		case IDC_LIST_TRANSITIONS:
			OnDeleteTransitions();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnEditSelectAll

void CSongDlg::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
			m_lstbxSegments.SetSel( -1, TRUE );
			m_lstbxSegments.SetCaretIndex( m_lstbxSegments.GetTopIndex(), 0 ); 
			break;
		
		case IDC_LIST_TRACKS:
			m_lstbxTracks.SetSel( -1, TRUE );
			m_lstbxTracks.SetCaretIndex( m_lstbxTracks.GetTopIndex(), 0 ); 
			break;

		case IDC_LIST_TRANSITIONS:
			m_lstbxTransitions.SetSel( -1, TRUE );
			m_lstbxTransitions.SetCaretIndex( m_lstbxTransitions.GetTopIndex(), 0 ); 
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnViewProperties

BOOL CSongDlg::OnViewProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( FAILED ( theApp.m_pSongComponent->m_pIFramework8->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( m_fOpenProperties == false
	&&  pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
		{
			if( m_pVirtualSegmentPropPageObject == NULL )
			{
				m_pVirtualSegmentPropPageObject = new CVirtualSegmentPropPageObject( this );
			}
			if( m_pVirtualSegmentPropPageObject )
			{
				if( SUCCEEDED ( m_pVirtualSegmentPropPageObject->OnShowProperties() ) )
				{
					fSuccess = TRUE;
				}
			}
			break;
		}
		
		case IDC_LIST_TRACKS:
		{
			if( m_pTrackPropPageObject == NULL )
			{
				m_pTrackPropPageObject = new CTrackPropPageObject( this );
			}
			if( m_pTrackPropPageObject )
			{
				if( SUCCEEDED ( m_pTrackPropPageObject->OnShowProperties() ) )
				{
					fSuccess = TRUE;
				}
			}
			break;
		}

		case IDC_LIST_TRANSITIONS:
		{
			if( m_pTransitionPropPageObject == NULL )
			{
				m_pTransitionPropPageObject = new CTransitionPropPageObject( this );
			}
			if( m_pTransitionPropPageObject )
			{
				if( SUCCEEDED ( m_pTransitionPropPageObject->OnShowProperties() ) )
				{
					fSuccess = TRUE;
				}
			}
			break;
		}

		default:
			if( SUCCEEDED ( m_pSong->OnShowProperties() ) )
			{
				fSuccess = TRUE;
			}
			break;
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnCommand

BOOL CSongDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( HIWORD(wParam) == 0 )	// menu command
	{
		if( m_pINodeRightMenu )	// can receive menu commands when m_pINodeRightMenu is NULL
		{
			m_pINodeRightMenu->OnRightClickMenuSelect( LOWORD(wParam) );
			m_pINodeRightMenu = NULL ;
			return TRUE;
		}

		switch( LOWORD(wParam) )
		{
			case IDM_INSERT_VIRTUAL_SEGMENT:
				OnInsertVirtualSegment();
				return TRUE;

			case IDM_DELETE_VIRTUAL_SEGMENT:
				OnDeleteVirtualSegments();
				return TRUE;

			case IDM_INSERT_TRACK:
				OnInsertTrack();
				return TRUE;

			case IDM_DELETE_TRACKS:
				OnDeleteTracks();
				return TRUE;

			case IDM_INSERT_TRANSITION:
				OnInsertTransition();
				return TRUE;

			case IDM_DELETE_TRANSITIONS:
				OnDeleteTransitions();
				return TRUE;

			case ID_EDIT_CUT:
				OnEditCut();
				return TRUE;

			case ID_EDIT_COPY:
				OnEditCopy();
				return TRUE;

			case ID_EDIT_PASTE:
				OnEditPaste();
				return TRUE;

			case ID_EDIT_SELECT_ALL:
				OnEditSelectAll();
				return TRUE;

			case ID_EDIT_INSERT:
				OnEditInsert();
				return TRUE;

			case ID_EDIT_DELETE:
				OnEditDelete();
				return TRUE;

			case IDM_VIRTUAL_SEGMENT_PROPERTIES:
				m_nEditMenuCtrlID = IDC_LIST_VIRTUAL_SEGMENTS;
				m_fOpenProperties = true;
				OnViewProperties();
				m_fOpenProperties = false;
				return TRUE;

			case IDM_TRACK_PROPERTIES:
				m_nEditMenuCtrlID = IDC_LIST_TRACKS;
				m_fOpenProperties = true;
				OnViewProperties();
				m_fOpenProperties = false;
				return TRUE;

			case IDM_TRANSITION_PROPERTIES:
				m_nEditMenuCtrlID = IDC_LIST_TRANSITIONS;
				m_fOpenProperties = true;
				OnViewProperties();
				m_fOpenProperties = false;
				return TRUE;
		}
	}
	
	return CFormView::OnCommand( wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnContextMenu

void CSongDlg::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	ScreenToClient( &point );
	CWnd* pWndControl= ChildWindowFromPoint( point, CWP_ALL );
	if( pWndControl )
	{
		pWnd = pWndControl;
	}
	ClientToScreen( &point );

	m_nEditMenuCtrlID = pWnd->GetDlgCtrlID();

	switch( m_nEditMenuCtrlID )
	{
		case IDC_LIST_VIRTUAL_SEGMENTS:
		{
			CVirtualSegment* pVirtualSegmentToEdit = NULL;

			m_lstbxSegments.SetFocus(); 

			// Get the item under the mouse
			BOOL fOutside;
			POINT ptClient = point;
			m_lstbxSegments.ScreenToClient( &ptClient );
			UINT nPos = m_lstbxSegments.ItemFromPoint( ptClient, fOutside );
			if( nPos != LB_ERR
			&&  fOutside == FALSE )
			{
				pVirtualSegmentToEdit = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( nPos );
				ASSERT( pVirtualSegmentToEdit != NULL );
				ASSERT( pVirtualSegmentToEdit != (CVirtualSegment *)0xFFFFFFFF );
			}

			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_VIRTUAL_SEGMENT_RMENU) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			int nSelCount = m_lstbxSegments.GetSelCount();
			int nCount = m_lstbxSegments.GetCount();
			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, (nSelCount > 0) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, (nSelCount > 0) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, (nCount > 0) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_DELETE_VIRTUAL_SEGMENT, (nSelCount > 0) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_VIRTUAL_SEGMENT_PROPERTIES, (pVirtualSegmentToEdit && (nSelCount == 1))
								? MF_ENABLED : MF_GRAYED );

			// Enable Paste
			BOOL fEnable = FALSE;
			IDataObject* pIDataObject;
			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				POINT pt = {0, 0};
				bool fMoveIsOK = false;
				HRESULT hr = CanPasteFromData( pIDataObject, false, pt, &fMoveIsOK );
				
				RELEASE( pIDataObject );
				
				if( hr == S_OK )
				{
					fEnable = TRUE;
				}
			}
			::EnableMenuItem( hMenuPopup, ID_EDIT_PASTE, fEnable ? MF_ENABLED : MF_GRAYED );

			m_pointRightMenu = point;
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}

		case IDC_LIST_TRACKS:
		{
			CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();

			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_TRACK_RMENU) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			int nSelCount = m_lstbxTracks.GetSelCount();
			int nCount = m_lstbxTracks.GetCount();
			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, (pVirtualSegmentToEdit && (nCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_INSERT_TRACK, (pVirtualSegmentToEdit) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_DELETE_TRACKS, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_TRACK_PROPERTIES, (pVirtualSegmentToEdit && (nSelCount == 1))
								? MF_ENABLED : MF_GRAYED );

			// Enable Paste
			BOOL fEnable = FALSE;
			IDataObject* pIDataObject;
			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				POINT pt = {0, 0};
				bool fMoveIsOK = false;
				HRESULT hr = CanPasteFromData( pIDataObject, false, pt, &fMoveIsOK );
				
				RELEASE( pIDataObject );
				
				if( hr == S_OK )
				{
					fEnable = TRUE;
				}
			}
			::EnableMenuItem( hMenuPopup, ID_EDIT_PASTE, fEnable ? MF_ENABLED : MF_GRAYED );

			m_pointRightMenu = point;
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}

		case IDC_LIST_TRANSITIONS:
		{
			CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();

			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_TRANSITION_RMENU) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			int nSelCount = m_lstbxTransitions.GetSelCount();
			int nCount = m_lstbxTransitions.GetCount();
			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, (pVirtualSegmentToEdit && (nCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_INSERT_TRANSITION, (pVirtualSegmentToEdit)
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_DELETE_TRANSITIONS, (pVirtualSegmentToEdit && (nSelCount > 0))
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_TRANSITION_PROPERTIES, (pVirtualSegmentToEdit && (nSelCount == 1))
								? MF_ENABLED : MF_GRAYED );

			// Enable Paste
			BOOL fEnable = FALSE;
			IDataObject* pIDataObject;
			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				POINT pt = {0, 0};
				bool fMoveIsOK = false;
				HRESULT hr = CanPasteFromData( pIDataObject, false, pt, &fMoveIsOK );
				
				RELEASE( pIDataObject );
				
				if( hr == S_OK )
				{
					fEnable = TRUE;
				}
			}
			::EnableMenuItem( hMenuPopup, ID_EDIT_PASTE, fEnable ? MF_ENABLED : MF_GRAYED );

			m_pointRightMenu = point;
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}

		default:
		{
			HINSTANCE hInstance;
			UINT nResourceId;
			if( SUCCEEDED ( m_pSong->GetRightClickMenuId(&hInstance, &nResourceId) ) )
			{
				HMENU hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
				if( hMenu )
				{
					HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
					m_pSong->OnRightClickMenuInit( hMenuPopup );

					m_pINodeRightMenu = m_pSong;
					m_pointRightMenu = point;
					::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
									  point.x, point.y, 0, GetSafeHwnd(), NULL );

					DestroyMenu( hMenu );
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnInsertVirtualSegment

void CSongDlg::OnInsertVirtualSegment( void )
{
	// Create a VirtualSegment 
	CVirtualSegment* pVirtualSegment = new CVirtualSegment( m_pSong );
	if( pVirtualSegment )
	{
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_VIRTUAL_SEGMENT );

		// Determine position in VirtualSegment list
		BOOL fOutside;
		int nPos = m_lstbxSegments.ItemFromPoint( m_lstbxSegments.m_ptLastXYPos, fOutside );
		if( fOutside == TRUE
		||  nPos == LB_ERR )
		{
			nPos = -1;
		}

		// Determine "next" VirtualSegment
		CVirtualSegment* pPrevVirtualSegment = m_pSong->IndexToVirtualSegment( nPos - 1 );
		CVirtualSegment* pNextVirtualSegment = m_pSong->IndexToVirtualSegment( nPos );
		if( pPrevVirtualSegment 
		&&  pPrevVirtualSegment->m_pNextVirtualSegment
		&&	pPrevVirtualSegment->m_pNextVirtualSegment == pNextVirtualSegment )
		{
			RELEASE( pPrevVirtualSegment->m_pNextVirtualSegment );
			pPrevVirtualSegment->m_pNextVirtualSegment = pVirtualSegment;
			pPrevVirtualSegment->m_pNextVirtualSegment->AddRef();
		}
		if( pNextVirtualSegment )
		{
			RELEASE( pVirtualSegment->m_pNextVirtualSegment );
			pVirtualSegment->m_pNextVirtualSegment = pNextVirtualSegment;
			pVirtualSegment->m_pNextVirtualSegment->AddRef();
		}

		// Insert the VirtualSegment into the Song
		m_pSong->InsertVirtualSegment( pVirtualSegment, nPos );

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_VIRTUAL_SEGMENT_LIST,
							  NULL );

		// Select the VirtualSegment
		SelectVirtualSegment( pVirtualSegment );

		// Open properties
		m_nEditMenuCtrlID = IDC_LIST_VIRTUAL_SEGMENTS;
		m_fOpenProperties = true;
		OnViewProperties();
		m_fOpenProperties = false;

		RELEASE( pVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDeleteVirtualSegments

void CSongDlg::OnDeleteVirtualSegments( void )
{
	int nNbrSelItems = m_lstbxSegments.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_VIRTUAL_SEGMENT );

		m_lstbxSegments.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( pnSelItems[i] );
		
			if( pVirtualSegment
			&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
			{
				// Remove pVirtualSegment from m_lstVirtualSegments
				m_pSong->RemoveVirtualSegment( pVirtualSegment );
			}
		}

		m_lstbxSegments.m_ptLastXYPos.x = INT_MAX;
		m_lstbxSegments.m_ptLastXYPos.y = INT_MAX;

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_PROPERTIES | SC_DIRECTMUSIC,
							  SSE_VIRTUAL_SEGMENT_LIST | SSE_TRANSITION_LIST,
							  NULL );

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::MarkSelectedVirtualSegments

void CSongDlg::MarkSelectedVirtualSegments( DWORD dwFlags )
{
	int nNbrSelItems = m_lstbxSegments.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxSegments.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_lstbxSegments.GetItemDataPtr( pnSelItems[i] );
		
			if( pVirtualSegment
			&&  pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
			{
				pVirtualSegment->m_dwBitsUI |= dwFlags;
			}
		}

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::UnMarkVirtualSegments

void CSongDlg::UnMarkVirtualSegments( DWORD dwFlags )
{
	CVirtualSegment* pVirtualSegment;
	for( int i = 0 ;  pVirtualSegment = m_pSong->IndexToVirtualSegment(i) ;  i++ )
	{
		if( pVirtualSegment == NULL )
		{
			break;
		}

		pVirtualSegment->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::DeleteMarkedVirtualSegments

void CSongDlg::DeleteMarkedVirtualSegments( DWORD dwFlags )
{
	CVirtualSegment* pVirtualSegment;
	for( int i = 0 ;  pVirtualSegment = m_pSong->IndexToVirtualSegment(i) ;  i++ )
	{
		if( pVirtualSegment == NULL )
		{
			break;
		}

		if( pVirtualSegment->m_dwBitsUI & dwFlags )
		{
			// Remove pVirtualSegment from m_lstVirtualSegments
			m_pSong->RemoveVirtualSegment( pVirtualSegment );

			m_lstbxSegments.m_ptLastXYPos.x = INT_MAX;
			m_lstbxSegments.m_ptLastXYPos.y = INT_MAX;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnInsertTrack

void CSongDlg::OnInsertTrack( void )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	CDlgAddTracks dlgAddTracks;

	// Prepare dialog
	dlgAddTracks.m_pSong = m_pSong;

	// Display dialog
	if( dlgAddTracks.DoModal() == IDOK 
	&&  dlgAddTracks.m_lstTracks.IsEmpty() == FALSE )
	{
		// Add tracks
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRACK );

		// Insert the Tracks into the VirtualSegment
		CTrack* pTrack;
		POSITION pos = dlgAddTracks.m_lstTracks.GetHeadPosition();
		while( pos )
		{
			pTrack = dlgAddTracks.m_lstTracks.GetNext( pos );

			pVirtualSegmentToEdit->InsertTrack( pTrack );
		}

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_TRACK_LIST,
							  NULL );

		// Select tracks
		m_lstbxTracks.SelectTrackList( dlgAddTracks.m_lstTracks );
		
		// Open properties
		m_nEditMenuCtrlID = IDC_LIST_TRACKS;
		m_fOpenProperties = true;
		OnViewProperties();
		m_fOpenProperties = false;
	}

	m_lstbxTracks.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDeleteTracks

void CSongDlg::OnDeleteTracks( void )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	int nNbrSelItems = m_lstbxTracks.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_TRACK );

		m_lstbxTracks.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CTrack* pTrack = (CTrack *)m_lstbxTracks.GetItemDataPtr( pnSelItems[i] );
		
			if( pTrack
			&&  pTrack != (CTrack *)0xFFFFFFFF )
			{
				// Remove pTrack from m_lstTracks
				pVirtualSegmentToEdit->RemoveTrack( pTrack );
			}
		}

		m_lstbxTracks.m_ptLastXYPos.x = INT_MAX;
		m_lstbxTracks.m_ptLastXYPos.y = INT_MAX;

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_PROPERTIES | SC_DIRECTMUSIC,
							  SSE_TRACK_LIST,
							  NULL );

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::MarkSelectedTracks

void CSongDlg::MarkSelectedTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit != pVirtualSegment )
	{
		ASSERT( 0 );
		return;
	}

	int nNbrSelItems = m_lstbxTracks.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxTracks.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CTrack* pTrack = (CTrack *)m_lstbxTracks.GetItemDataPtr( pnSelItems[i] );
		
			if( pTrack
			&&  pTrack != (CTrack *)0xFFFFFFFF )
			{
				pTrack->m_dwBitsUI |= dwFlags;
			}
		}

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::UnMarkTracks

void CSongDlg::UnMarkTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	POSITION pos = pVirtualSegment->m_lstTracks.GetHeadPosition();
	while( pos )
	{ 
		CTrack* pTrack = pVirtualSegment->m_lstTracks.GetNext( pos );

		pTrack->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::DeleteMarkedTracks

void CSongDlg::DeleteMarkedTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	POSITION pos = pVirtualSegment->m_lstTracks.GetHeadPosition();
	while( pos )
	{ 
		CTrack* pTrack = pVirtualSegment->m_lstTracks.GetNext( pos );

		if( pTrack->m_dwBitsUI & dwFlags )
		{
			// Remove pTrack from m_lstTracks
			pVirtualSegment->RemoveTrack( pTrack );

			m_lstbxTracks.m_ptLastXYPos.x = INT_MAX;
			m_lstbxTracks.m_ptLastXYPos.y = INT_MAX;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnInsertTransition

void CSongDlg::OnInsertTransition( void )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	CTransition* pTransition = new CTransition( m_pSong );
	if( pTransition )
	{
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_ADD_TRANSITION );

		// Insert the Transition into the VirtualSegment
		pVirtualSegmentToEdit->InsertTransition( pTransition );

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_TRANSITION_LIST,
							  NULL );

		// Select the Transition
		m_lstbxTransitions.SelectTransition( pTransition, true );

		// Open properties
		m_nEditMenuCtrlID = IDC_LIST_TRANSITIONS;
		m_fOpenProperties = true;
		OnViewProperties();
		m_fOpenProperties = false;

		RELEASE( pTransition );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnDeleteTransitions

void CSongDlg::OnDeleteTransitions( void )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit == NULL )
	{
		ASSERT( 0 );
		return;
	}

	int nNbrSelItems = m_lstbxTransitions.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_TRANSITION );

		m_lstbxTransitions.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CTransition* pTransition = (CTransition *)m_lstbxTransitions.GetItemDataPtr( pnSelItems[i] );
		
			if( pTransition
			&&  pTransition != (CTransition *)0xFFFFFFFF )
			{
				// Remove pTransition from m_lstTransitions
				pVirtualSegmentToEdit->RemoveTransition( pTransition );
			}
		}

		m_lstbxTransitions.m_ptLastXYPos.x = INT_MAX;
		m_lstbxTransitions.m_ptLastXYPos.y = INT_MAX;

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_PROPERTIES | SC_DIRECTMUSIC,
							  SSE_TRANSITION_LIST,
							  NULL );

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::MarkSelectedTransitions

void CSongDlg::MarkSelectedTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	CVirtualSegment* pVirtualSegmentToEdit = GetVirtualSegmentToEdit();
	if( pVirtualSegmentToEdit != pVirtualSegment )
	{
		ASSERT( 0 );
		return;
	}

	int nNbrSelItems = m_lstbxTransitions.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxTransitions.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CTransition* pTransition = (CTransition *)m_lstbxTransitions.GetItemDataPtr( pnSelItems[i] );
		
			if( pTransition
			&&  pTransition != (CTransition *)0xFFFFFFFF )
			{
				pTransition->m_dwBitsUI |= dwFlags;
			}
		}

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::UnMarkTransitions

void CSongDlg::UnMarkTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	POSITION pos = pVirtualSegment->m_lstTransitions.GetHeadPosition();
	while( pos )
	{ 
		CTransition* pTransition = pVirtualSegment->m_lstTransitions.GetNext( pos );

		pTransition->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::DeleteMarkedTransitions

void CSongDlg::DeleteMarkedTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags )
{
	POSITION pos = pVirtualSegment->m_lstTransitions.GetHeadPosition();
	while( pos )
	{ 
		CTransition* pTransition = pVirtualSegment->m_lstTransitions.GetNext( pos );

		if( pTransition->m_dwBitsUI & dwFlags )
		{
			// Remove pTransition from m_lstTransitions
			pVirtualSegment->RemoveTransition( pTransition );

			m_lstbxTransitions.m_ptLastXYPos.x = INT_MAX;
			m_lstbxTransitions.m_ptLastXYPos.y = INT_MAX;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::OnTransition

void CSongDlg::OnTransition() 
{
	if( m_pSong )
	{
		m_pSong->TransitionWithinSong();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSongDlg::IsDataObjectFromSameSong

bool CSongDlg::IsDataObjectFromSameSong( IDataObject* pIDataObject ) 
{
    IDMUSProdRIFFStream* pIRiffStream;
	IStream* pIStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;

	GUID guidSourceSong;
	memset( &guidSourceSong, 0, sizeof(GUID) );

	GUID guidTargetSong;
	memset( &guidTargetSong, 1, sizeof(GUID) );
	if( m_pSong )
	{
		m_pSong->GetGUID( &guidTargetSong );
	}

	// Create a new CDllJazzDataObject and see if it contains a Song
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject )
	{
		// CF_VIRTUAL_SEGMENT_LIST
		if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfVirtualSegmentList, &pIStream  ) ) )
		{
			if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				// Seek to beginning of stream
				LARGE_INTEGER liTemp;
				liTemp.QuadPart = 0;
				pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

				ckMain.fccType = DMUS_FOURCC_SONG_FORM;
				if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
				{
					// Get Source Song GUID
					ck.ckid = DMUS_FOURCC_GUID_CHUNK;
					if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
					{
						dwSize = min( ck.cksize, sizeof( GUID ) );
						if( FAILED ( pIStream->Read( &guidSourceSong, dwSize, &dwByteCount ) )
						||  dwByteCount != dwSize )
						{
							memset( &guidSourceSong, 0, sizeof(GUID) );
						}
					}
				}

				RELEASE( pIRiffStream );
			}

			RELEASE( pIStream );
		}		

		// CF_TRACK_LIST
		// CF_TRANSITION_LIST
		else if( SUCCEEDED ( pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfTrackList, &pIStream  ) ) 
			 ||  SUCCEEDED ( pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfTransitionList, &pIStream  ) ) )
		{
			if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				// Seek to beginning of stream
				LARGE_INTEGER liTemp;
				liTemp.QuadPart = 0;
				pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

				ckMain.fccType = DMUS_FOURCC_SEGREF_LIST;
				if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
				{
					// Get Source Song GUID
					ck.ckid = DMUS_FOURCC_SONG_GUID_UI_CHUNK;
					if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
					{
						dwSize = min( ck.cksize, sizeof( GUID ) );
						if( FAILED ( pIStream->Read( &guidSourceSong, dwSize, &dwByteCount ) )
						||  dwByteCount != dwSize )
						{
							memset( &guidSourceSong, 0, sizeof(GUID) );
						}
					}
				}

				RELEASE( pIRiffStream );
			}

			RELEASE( pIStream );
		}		

		RELEASE( pDataObject );
	}

	if( ::IsEqualGUID( guidSourceSong, guidTargetSong ) )
	{
		return true;
	}

	return false;
}
