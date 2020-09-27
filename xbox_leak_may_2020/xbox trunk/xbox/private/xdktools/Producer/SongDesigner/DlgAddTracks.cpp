// DlgAddTracks.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "DlgAddTracks.h"
#include "SegmentDesigner.h"
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks dialog

CDlgAddTracks::CDlgAddTracks(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddTracks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddTracks)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pSong = NULL;
}

CDlgAddTracks::~CDlgAddTracks()
{
	CTrack* pTrack;

	while( !m_lstTracks.IsEmpty() )
	{
		pTrack = m_lstTracks.RemoveHead();

		RELEASE( pTrack );
	}

	while( !m_lstSegmentTracks.IsEmpty() )
	{
		pTrack = m_lstSegmentTracks.RemoveHead();

		RELEASE( pTrack );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks:BuildSegmentTrackList

void CDlgAddTracks::BuildSegmentTrackList( void )
{
	// Initialize list
	while( !m_lstSegmentTracks.IsEmpty() )
	{
		CTrack* pTrack = m_lstSegmentTracks.RemoveHead();

		RELEASE( pTrack );
	}

	// Enumerate SourceSegments
	CSourceSegment* pSourceSegment;
	for( DWORD dwIndex = 0 ;  pSourceSegment = m_pSong->IndexToSourceSegment(dwIndex) ;  dwIndex++ )
	{
		if( pSourceSegment == NULL )
		{
			break;
		}

		IDMUSProdNode* pISegmentNode;
		if( SUCCEEDED ( pSourceSegment->GetSegmentDocRootNode( &pISegmentNode ) ) )
		{
			// Get an IDMUSProdSegmentEdit8 interface pointer
			IDMUSProdSegmentEdit8* pISegmentEdit8;
			if( SUCCEEDED ( pISegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void **)&pISegmentEdit8 ) ) )
			{
				DMUS_IO_TRACK_HEADER dmusTrackHeader;
				DMUS_IO_TRACK_EXTRAS_HEADER dmusTrackExtrasHeader;
				HRESULT hr = S_OK;

				DWORD dwSequenceTrackGroups = 0;

				// Enumerate Tracks
				for( DWORD dwIdx = 0 ;  (hr == S_OK) ;  dwIdx++ )
				{
					hr = pISegmentEdit8->EnumTrack( dwIdx,
							 						sizeof(DMUS_IO_TRACK_HEADER), &dmusTrackHeader,
													sizeof(DMUS_IO_TRACK_EXTRAS_HEADER), &dmusTrackExtrasHeader );
					if( hr == S_OK )
					{
						// If this is a sequence track
						if( CLSID_DirectMusicSeqTrack == dmusTrackHeader.guidClassID )
						{
							// If we've already found a sequence track in this track group
							if( dmusTrackHeader.dwGroup & dwSequenceTrackGroups )
							{
								// Continue enumerating
								continue;
							}

							// Mark that we've found a sequence track in this track group
							dwSequenceTrackGroups |= dmusTrackHeader.dwGroup;
						}

						// Add the track to our list to display
						CTrack* pTrack = new CTrack( m_pSong, pSourceSegment, dwIdx, &dmusTrackHeader, &dmusTrackExtrasHeader );

						m_lstSegmentTracks.AddTail( pTrack );
					}
				}

				RELEASE( pISegmentEdit8 );
			}

			RELEASE( pISegmentNode );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks:FillSegmentTrackListBox

void CDlgAddTracks::FillSegmentTrackListBox( void )
{
	m_lstbxSegmentTracks.SetRedraw( FALSE );
	m_lstbxSegmentTracks.ResetContent();
	m_lstbxSegmentTracks.SetHorizontalExtent( 0 );

	POSITION pos = m_lstSegmentTracks.GetHeadPosition();
	while( pos )
	{
		CTrack* pTrack = m_lstSegmentTracks.GetNext( pos );

		int nPos = m_lstbxSegmentTracks.AddString( (LPCTSTR)pTrack  );
	}

	m_lstbxSegmentTracks.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::CreateFont

CFont* CDlgAddTracks::CreateFont( void )
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


void CDlgAddTracks::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddTracks)
	DDX_Control(pDX, IDC_LIST_TRACKS, m_lstbxSegmentTracks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddTracks, CDialog)
	//{{AFX_MSG_MAP(CDlgAddTracks)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_COMPAREITEM()
	ON_LBN_DBLCLK(IDC_LIST_TRACKS, OnDblClkListTracks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks message handlers

/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::OnInitDialog

BOOL CDlgAddTracks::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	BuildSegmentTrackList();
	FillSegmentTrackListBox();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::OnOK

void CDlgAddTracks::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_lstTracks.IsEmpty() );

	int nNbrSelItems = m_lstbxSegmentTracks.GetSelCount();
	if( nNbrSelItems > 0 )
	{
		int* pnSelItems = new int[nNbrSelItems];
		if( pnSelItems )
		{
			m_lstbxSegmentTracks.GetSelItems( nNbrSelItems, pnSelItems );

			for( int i = 0;  i < nNbrSelItems ;  i++ )
			{
				CTrack* pTrack = (CTrack *)m_lstbxSegmentTracks.GetItemDataPtr( pnSelItems[i] );
			
				if( pTrack
				&&  pTrack != (CTrack *)0xFFFFFFFF )
				{
					pTrack->AddRef();
					m_lstTracks.AddTail( pTrack );
				}
			}

			delete [] pnSelItems;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::OnDrawItem

void CDlgAddTracks::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( nIDCtl != IDC_LIST_TRACKS )
	{
		CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
		return;
	}

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
			CFont* pFont = CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			// Fill the background color
			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
				dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Get the Track's name
			CString strName;
			pTrack->FormatTextUI( strName );

			// Set horizontal extent
			TEXTMETRIC tm;
			dc.GetTextMetrics( &tm );
			CSize sizeText = dc.GetTextExtent( strName );
			sizeText.cx += tm.tmMaxCharWidth;
			if( sizeText.cx > m_lstbxSegmentTracks.GetHorizontalExtent() )
			{
				m_lstbxSegmentTracks.SetHorizontalExtent( sizeText.cx );
			}

			// Draw the item
			rect.left  += 3;
			dc.DrawText( strName, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT) );
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


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::OnMeasureItem

void CDlgAddTracks::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nIDCtl )
	{
		case IDC_LIST_TRACKS:
		{
			CDC* pDC = GetDC();
			if( pDC )
			{
				CFont* pFontOld = NULL;
				CFont* pFont = CreateFont();
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
			return;
		}
	}

	CDialog::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddTracks::OnCompareItem

int CDlgAddTracks::OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( nIDCtl != IDC_LIST_TRACKS )
	{
		return CDialog::OnCompareItem( nIDCtl, lpCompareItemStruct );
	}

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
// CDlgAddTracks::OnDblClkListTracks

void CDlgAddTracks::OnDblClkListTracks( void ) 
{
	OnOK();
}
