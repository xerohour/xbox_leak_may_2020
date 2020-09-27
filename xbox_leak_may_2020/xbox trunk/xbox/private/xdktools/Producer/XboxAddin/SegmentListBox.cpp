// SegmentListBox.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "SegmentListBox.h"
#include "Segment.h"
#include "XboxAddinComponent.h"
#include "XboxAddinDlg.h"
#include "LeftPaneDlg.h"
#include "Audiopath.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PLAY_ICON_HEIGHT (16)
#define PLAY_ICON_WIDTH (16)
#define PLAY_BUTTON_WIDTH (16);

/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox

CSegmentListBox::CSegmentListBox()
{
    m_hPlayIcon = AfxGetApp()->LoadIcon(IDI_PLAY);
	m_hStopIcon = AfxGetApp()->LoadIcon(IDI_STOP);
	m_hTransitionIcon = AfxGetApp()->LoadIcon(IDI_TRANSITION);
	m_pClickedSegment = NULL;
	ZeroMemory( &m_rectClicked, sizeof(m_rectClicked) );
}

CSegmentListBox::~CSegmentListBox()
{
}


BEGIN_MESSAGE_MAP(CSegmentListBox, CFileListBox)
	//{{AFX_MSG_MAP(CSegmentListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox message handlers

void CSegmentListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Get the Segment
	CSegment *pSegment = reinterpret_cast<CSegment *>(lpDrawItemStruct->itemData);
	if( !pSegment )
	{
		return;
	}

	// if we're drawing a real item and the item is to be completely
	// draw or drawn as if it is selected, then ...

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		// set up the text color and the background color
		COLORREF oldTextColor, oldBkColor;
		if ( (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 )
		{
			oldTextColor = ::SetTextColor( lpDrawItemStruct->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
			oldBkColor = ::SetBkColor( lpDrawItemStruct->hDC, ::GetFocus() == m_hWnd ? ::GetSysColor(COLOR_HIGHLIGHT) : ::GetSysColor(COLOR_INACTIVECAPTION) );
		}
		else
		{
			oldTextColor = ::SetTextColor( lpDrawItemStruct->hDC, ::GetSysColor(COLOR_WINDOWTEXT) );
			oldBkColor = ::SetBkColor( lpDrawItemStruct->hDC, ::GetSysColor(COLOR_WINDOW) );
		}

		// Draw the (un)selected background
		::ExtTextOut( lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &lpDrawItemStruct->rcItem, NULL, 0, NULL);

        // Play button
        RECT rect;
        rect = lpDrawItemStruct->rcItem;

		// Set the correct button style (pushed/unpushed)
		const DWORD dwFrameStyle = pSegment->m_fPressed ? DFCS_BUTTONPUSH | DFCS_CHECKED : DFCS_BUTTONPUSH;
        rect.right = PLAY_BUTTON_WIDTH;
        ::DrawFrameControl( lpDrawItemStruct->hDC, &rect, DFC_BUTTON, dwFrameStyle );

		// Choose the correct icon to display
		HICON hIconToUse;
		if( pSegment->m_fPlaying )
		{
			hIconToUse = m_hStopIcon;
		}
		else
		{
			hIconToUse = m_hPlayIcon;
		}

        int nYPos = ((rect.bottom + rect.top) / 2) - (PLAY_ICON_HEIGHT / 2);
        ::DrawIconEx( lpDrawItemStruct->hDC, rect.left, nYPos, hIconToUse, PLAY_ICON_WIDTH, PLAY_ICON_HEIGHT, 0, NULL, DI_NORMAL );

        // Set up the text rectangle
        rect.left = PLAY_BUTTON_WIDTH;
        rect.right = lpDrawItemStruct->rcItem.right;

		// Segment Name
		//if( LB_ERR != m_listSegment.GetItemRect( lpDrawItemStruct->itemID, &rect ) )
		{
			::DrawText( lpDrawItemStruct->hDC, pSegment->GetName(), -1, &rect, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );
		}

		// restore the old objects in the DC
		::SetTextColor( lpDrawItemStruct->hDC, oldTextColor );
		::SetBkColor( lpDrawItemStruct->hDC, oldBkColor);
	}

	// if the item is focused, draw the focus rectangle
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
	{
		::DrawFocusRect( lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem) );
	}
}

void InsertAudiopathItem( HMENU hAudiopathMenu, int nMenuID, int nAudiopathIndex, const CSegment *pSegment )
{
	MENUITEMINFO menuItemInfo;
	ZeroMemory( &menuItemInfo, sizeof(MENUITEMINFO) );
	menuItemInfo.cbSize = sizeof(MENUITEMINFO);

	menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
	menuItemInfo.fType = MFT_STRING;

	const CAudiopath *pAudiopath = reinterpret_cast<CAudiopath *>(theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->m_pLeftPaneDlg->m_listAudiopath.GetItemDataPtr( nAudiopathIndex ));

	// Add an extra & to any &s in the string, to ensure they display properly
	CString strName = pAudiopath->GetName();
	int nIndex = 0;
	while( nIndex != -1 )
	{
		nIndex = strName.Find( '&', nIndex );
		if( nIndex != -1 )
		{
			strName.Insert( nIndex, '&' );
			nIndex+=2;
		}
	}

	menuItemInfo.dwTypeData = strName.GetBuffer( strName.GetLength() );
	if( (pSegment->m_pAudiopath == NULL)
	&&	(pSegment->m_dwStandardAudiopath == 0)
	&&	pAudiopath->IsDefault() )
	{
		menuItemInfo.fState = MFS_CHECKED;
	}
	else
	{
		menuItemInfo.fState = (pSegment->m_pAudiopath == pAudiopath) ? MFS_CHECKED : 0;
	}
	menuItemInfo.wID = nMenuID;
	::InsertMenuItem( hAudiopathMenu, 0, true, &menuItemInfo );
}

void CSegmentListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BOOL bOutside = TRUE;
	int nIndex = ItemFromPoint( point, bOutside );

	if( !bOutside )
	{
		CSegment *pSegment = reinterpret_cast<CSegment *>(GetItemDataPtr( nIndex ));

		HMENU hMenuMain = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_SEGMENT_MENU) );
		if( hMenuMain )
		{
			HMENU hSubMenu = ::GetSubMenu( hMenuMain, 0 );
			if( hSubMenu )
			{
				HMENU hAudiopathMenu = ::GetSubMenu( hSubMenu, 1 );
				if( hAudiopathMenu )
				{
					if( theApp.m_pXboxAddinComponent->m_lstAudiopaths.GetCount() > 2 )
					{
						InsertAudiopathItem( hAudiopathMenu, ID__USEAUDIOPATH_AUDIOPATH3, 2, pSegment );
					}

					if( theApp.m_pXboxAddinComponent->m_lstAudiopaths.GetCount() > 1 )
					{
						InsertAudiopathItem( hAudiopathMenu, ID__USEAUDIOPATH_AUDIOPATH2, 1, pSegment );
					}

					if( theApp.m_pXboxAddinComponent->m_lstAudiopaths.GetCount() > 0 )
					{
						InsertAudiopathItem( hAudiopathMenu, ID__USEAUDIOPATH_AUDIOPATH1, 0, pSegment );
					}

					// If set to use a standard audiopath
					int nItemID = 0;
					switch( pSegment->m_dwStandardAudiopath )
					{
					case DMUS_APATH_SHARED_STEREOPLUSREVERB:
						nItemID = ID__USEAUDIOPATH_STANDARDSTEREOREVERB;
						break;
					case DMUS_APATH_DYNAMIC_3D:
						nItemID = ID__USEAUDIOPATH_STANDARD3DDRY;
						break;
					case DMUS_APATH_DYNAMIC_MONO:
						nItemID = ID__USEAUDIOPATH_STANDARDMONO;
						break;
                    case DMUS_APATH_MIXBIN_QUAD_MUSIC:
                        nItemID = ID__USEAUDIOPATH_STANDARDQUADMUSIC;
                        break;
                    case DMUS_APATH_MIXBIN_QUAD_ENV:
                        nItemID = ID__USEAUDIOPATH_STANDARDQUADENV;
                        break;
                    case DMUS_APATH_MIXBIN_5DOT1_MUSIC:
                        nItemID = ID__USEAUDIOPATH_STANDARD51MUSIC;
                        break;
                    case DMUS_APATH_MIXBIN_5DOT1_ENV:
                        nItemID = ID__USEAUDIOPATH_STANDARD51ENV;
                        break;
                    case DMUS_APATH_MIXBIN_STEREO_EFFECTS:
                        nItemID = ID__USEAUDIOPATH_STANDARDSTEREOEFFECT;
                        break;
					case DMUS_APATH_SHARED_STEREO:
						nItemID = ID__USEAUDIOPATH_STANDARDSHAREDSTEREO;
						break;
					}
					if( nItemID)
					{
						::CheckMenuItem( hAudiopathMenu, nItemID, MF_BYCOMMAND | MF_CHECKED );
					}

					if( pSegment->m_fPlaying )
					{
						// Disable the audiopath items if the segment is playing
						for( int i=::GetMenuItemCount(hAudiopathMenu)-1; i >= 0 ; i-- )
						{
							::EnableMenuItem( hAudiopathMenu, i, MF_BYPOSITION | MF_GRAYED );
						}
					}

					theApp.m_pXboxAddinComponent->m_lstAudiopaths;
				}

				POINT point;
				::GetCursorPos( &point );
				int nChosenCommand = ::TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, point.x, point.y, 0, GetSafeHwnd(), NULL );

				if( nChosenCommand )
				{
					::SendMessage( ::GetParent( GetSafeHwnd() ), WM_APP, (WPARAM)nChosenCommand, (LPARAM)pSegment );
				}
			}
		}
	}
	else
	{
		CFileListBox::OnRButtonUp(nFlags, point);
	}
}

void CSegmentListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( point.x < 16 )
	{
		BOOL bOutside = TRUE;
		int nIndex = ItemFromPoint( point, bOutside );

		if( !bOutside )
		{
			m_pClickedSegment = reinterpret_cast<CSegment *>(GetItemDataPtr( nIndex ));
			m_pClickedSegment->m_fPressed = true;

			// Redraw the button
			if( LB_ERR != GetItemRect( nIndex, &m_rectClicked ) )
			{
				m_rectClicked.right = 16;
				InvalidateRect( &m_rectClicked, FALSE );
			}
		}
	}
	
	CFileListBox::OnLButtonDown(nFlags, point);
}

void CSegmentListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( point.x < 16 )
	{
		BOOL bOutside = TRUE;
		int nIndex = ItemFromPoint( point, bOutside );

		if( !bOutside )
		{
			CSegment *pSegment = reinterpret_cast<CSegment *>(GetItemDataPtr( nIndex ));
			if( pSegment == m_pClickedSegment )
			{
				pSegment->ButtonClicked();
			}
		}
	}

	if( m_pClickedSegment )
	{
		m_pClickedSegment->m_fPressed = false;
		m_pClickedSegment = NULL;
	}

	// Redraw the button
	if( m_rectClicked.right > 0 )
	{
		InvalidateRect( &m_rectClicked, FALSE );
		m_rectClicked.right = 0;
	}
	
	CFileListBox::OnLButtonUp(nFlags, point);
}

void CSegmentListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( VK_DELETE == nChar )
	{
		// Delete the currently selected item
		int nIndex = GetCurSel();
		if( nIndex != LB_ERR )
		{
			CSegment *pSegment = reinterpret_cast<CSegment *>(GetItemDataPtr( nIndex ));
			if( pSegment )
			{
				::SendMessage( ::GetParent( GetSafeHwnd() ), WM_APP, ID_DELETE_SEGMENT, (LPARAM)pSegment );
			}
		}
	}
	
	CFileListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
