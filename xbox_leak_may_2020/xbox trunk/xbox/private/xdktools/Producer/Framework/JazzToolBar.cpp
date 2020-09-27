// JazzToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "JazzToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar

CBookmarkToolBar::CBookmarkToolBar()
{
}

CBookmarkToolBar::~CBookmarkToolBar()
{
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::AddBookmark

BOOL CBookmarkToolBar::AddBookmark( CBookmark* pBookmark, BOOL fSelect ) 
{
	ASSERT( pBookmark != NULL );
	ASSERT( !pBookmark->m_strName.IsEmpty() );

	// Combine bookmarks that have the same GUID.
	// Each piece is part of a multi-Project bookmark.
	CBookmark* pBookmarkCombo;
	int nPos;
	int nCount = m_ctlBookmarkComboBox.GetCount();

	for( nPos = 0 ;  nPos < nCount ;  nPos++ )
	{
		pBookmarkCombo = (CBookmark *)m_ctlBookmarkComboBox.GetItemDataPtr( nPos );
		if( pBookmarkCombo )
		{
			if( pBookmarkCombo != (CBookmark *)-1 )
			{
				// See if GUIDs are equal
				if( ::IsEqualGUID( pBookmarkCombo->m_guid, pBookmark->m_guid ) )
				{
					// GUIDs are equal so merge the bookmarks
					pBookmark->MergeBookmark( pBookmarkCombo );

					// Now remove pBookmarkCombo from the combo box
					// It will be replaced by pBookmark
					m_ctlBookmarkComboBox.SetCurSel( nPos );
					RemoveSelectedBookmark();
				}
			}
		}
	}

	// Add to combo box
	nPos = m_ctlBookmarkComboBox.AddString( pBookmark->m_strName );
	if( nPos >= 0 )
	{
		m_ctlBookmarkComboBox.SetItemDataPtr( nPos, pBookmark );
		
		if( fSelect == FALSE )
		{
			nPos = -1;
		}
		m_ctlBookmarkComboBox.SetCurSel( nPos );

		// Add to application list
		theApp.m_lstBookmarks.AddTail( pBookmark );

		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::RemoveAllBookmarks

void CBookmarkToolBar::RemoveAllBookmarks( void ) 
{
	if( AfxMessageBox( IDS_DELETE_ALL_BOOKMARKS, MB_OKCANCEL ) == IDOK )
	{
		// Clear combo box
		m_ctlBookmarkComboBox.ResetContent();

		// Delete all Bookmarks
		CBookmark* pBookmark;

		while( !theApp.m_lstBookmarks.IsEmpty() )
		{
			pBookmark = static_cast<CBookmark*>( theApp.m_lstBookmarks.RemoveHead() );
			delete pBookmark;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::RemoveSelectedBookmark

void CBookmarkToolBar::RemoveSelectedBookmark( void ) 
{
	// Get the current Bookmark
	int nPos = m_ctlBookmarkComboBox.GetCurSel();
	if( nPos == CB_ERR )
	{
		return;
	}

	// Get a pointer to the Bookmark
	CBookmark* pBookmark = (CBookmark *)m_ctlBookmarkComboBox.GetItemDataPtr( nPos );
	if( pBookmark )
	{
		if( pBookmark != (CBookmark *)-1 )
		{
			// Remove from application list
			POSITION pos = theApp.m_lstBookmarks.Find( pBookmark );
			if( pos )
			{
				theApp.m_lstBookmarks.RemoveAt( pos );
			}
			
			// Delete the Bookmark
			delete pBookmark;
		}
	}
	
	// Remove from combo box
	m_ctlBookmarkComboBox.DeleteString( nPos );
	
	// Select the next item
	if( m_ctlBookmarkComboBox.SetCurSel( nPos ) == CB_ERR )
	{
		m_ctlBookmarkComboBox.SetCurSel( 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::OnBookmarkRemove

void CBookmarkToolBar::OnBookmarkRemove() 
{
	// Get the current Bookmark
	int nPos = m_ctlBookmarkComboBox.GetCurSel();
	if( nPos == CB_ERR )
	{
		return;
	}

	CString strName;
	m_ctlBookmarkComboBox.GetLBText( nPos, strName );

	CString strPrompt;
	AfxFormatString1( strPrompt, IDS_DELETE_ONE_BOOKMARK, strName );

	if( AfxMessageBox( strPrompt, MB_OKCANCEL ) == IDOK )
	{
		RemoveSelectedBookmark();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::RemoveBookmark

void CBookmarkToolBar::RemoveBookmark( CBookmark* pBookmark ) 
{
	CBookmark* pComboBookmark;
	int nLastPos;

	// Remove existing bookmark (if one exists)
	int nPos = -1;
	for( ; ; )
	{
		nLastPos = nPos;
		nPos = m_ctlBookmarkComboBox.FindStringExact( nPos, pBookmark->m_strName );

		if( nPos == CB_ERR
		||	nPos < nLastPos )
		{
			break;
		}

		if( nPos >= 0 )
		{
			pComboBookmark = (CBookmark *)m_ctlBookmarkComboBox.GetItemDataPtr( nPos );
			
			if( pComboBookmark
			&&  pComboBookmark == pBookmark )
			{
				m_ctlBookmarkComboBox.SetCurSel( nPos );
				RemoveSelectedBookmark();
				m_ctlBookmarkComboBox.SetCurSel( -1 );
				break;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::ApplySelectedBookmark

void CBookmarkToolBar::ApplySelectedBookmark( void ) 
{
	// Get the current Bookmark
	int nPos = m_ctlBookmarkComboBox.GetCurSel();
	if( nPos == CB_ERR )
	{
		return;
	}

	// Get a pointer to the Bookmark
	CBookmark* pBookmark = (CBookmark *)m_ctlBookmarkComboBox.GetItemDataPtr( nPos );
	if( pBookmark )
	{
		if( pBookmark != (CBookmark *)-1 )
		{
			// Apply the bookmark
			pBookmark->Apply( TRUE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::IsBookmarkSelected

BOOL CBookmarkToolBar::IsBookmarkSelected( void ) 
{
	// Get the current Bookmark
	int nPos = m_ctlBookmarkComboBox.GetCurSel();
	if( nPos == CB_ERR )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar::BookmarkNameExists

BOOL CBookmarkToolBar::BookmarkNameExists( LPCTSTR szName )
{
	ASSERT( szName != NULL );

	int nPos = m_ctlBookmarkComboBox.FindStringExact( -1, szName );
	if( nPos >= 0 )
	{
		return TRUE;
	}

	return FALSE;
}


BEGIN_MESSAGE_MAP(CBookmarkToolBar, CToolBar)
	//{{AFX_MSG_MAP(CBookmarkToolBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar message handlers

/////////////////////////////////////////////////////////////////////////////
// CBookmarkToolBar OnCreate

int CBookmarkToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CToolBar::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	if( !LoadToolBar(ID_BOOKMARK_TOOLBAR) )
	{
		return -1;
	}

// Create font for toolbar combo box
	CClientDC dc( this );
	int nHeight = -( (dc.GetDeviceCaps(LOGPIXELSY) * 8) / 72 );

	m_font.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" );
	
	CFont* pOldFont = dc.SelectObject( &m_font );
	
	TEXTMETRIC tm;
	dc.GetTextMetrics( &tm );
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;

	dc.SelectObject( pOldFont );

// Create combo box
	SetButtonInfo( 0, IDC_BOOKMARK_COMBO, TBBS_SEPARATOR, (cxChar * 22) );
	
	CRect rect;
	GetItemRect( 0, &rect );
	rect.bottom = rect.top + (cyChar * 10);

	if( !m_ctlBookmarkComboBox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_SORT, rect, this, IDC_BOOKMARK_COMBO) )
	{
		return -1;
	}

// Set combo box font which controls height of combo box
	m_ctlBookmarkComboBox.SetFont( &m_font );

	return 0;
}
