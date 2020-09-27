// AudiopathListBox.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "AudiopathListBox.h"
#include "Audiopath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudiopathListBox

CAudiopathListBox::CAudiopathListBox()
{
}

CAudiopathListBox::~CAudiopathListBox()
{
}


BEGIN_MESSAGE_MAP(CAudiopathListBox, CFileListBox)
	//{{AFX_MSG_MAP(CAudiopathListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudiopathListBox message handlers

void CAudiopathListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Get the Audiopath
	CAudiopath *pAudiopath = reinterpret_cast<CAudiopath *>(lpDrawItemStruct->itemData);
	if( !pAudiopath )
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

		// File Name
		::DrawText( lpDrawItemStruct->hDC, pAudiopath->GetName(), -1, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

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

void CAudiopathListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	BOOL bOutside = TRUE;
	int nIndex = ItemFromPoint( point, bOutside );

	CAudiopath *pAudiopath = NULL;
	if( !bOutside )
	{
		pAudiopath = reinterpret_cast<CAudiopath *>(GetItemDataPtr( nIndex ));
	}

	HMENU hMenuMain = ::LoadMenu( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_AUDIOPATH_MENU) );
	if( hMenuMain )
	{
		HMENU hSubMenu = ::GetSubMenu( hMenuMain, 0 );
		if( hSubMenu )
		{
			if( !pAudiopath )
			{
				::EnableMenuItem( hSubMenu, ID_SET_DEFAULT, MF_GRAYED | MF_BYCOMMAND );
				::EnableMenuItem( hSubMenu, ID_DELETE_AUDIOPATH, MF_GRAYED | MF_BYCOMMAND );
			}
			else if( pAudiopath->IsDefault() )
			{
				// Already default item
				::EnableMenuItem( hSubMenu, ID_SET_DEFAULT, MF_GRAYED | MF_BYCOMMAND );
			}


			POINT pt;
			::GetCursorPos( &pt );
			int nChosenCommand = ::TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, 0, GetSafeHwnd(), NULL );

			if( nChosenCommand )
			{
				::SendMessage( ::GetParent( GetSafeHwnd() ), WM_APP, (WPARAM)nChosenCommand, (LPARAM)pAudiopath );
			}
		}
	}
	else
	{
		CFileListBox::OnRButtonUp(nFlags, point);
	}
}

void CAudiopathListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( VK_DELETE == nChar )
	{
		// Delete the currently selected item
		int nIndex = GetCurSel();
		if( nIndex != LB_ERR )
		{
			CAudiopath *pAudiopath = reinterpret_cast<CAudiopath *>(GetItemDataPtr( nIndex ));
			if( pAudiopath )
			{
				::SendMessage( ::GetParent( GetSafeHwnd() ), WM_APP, ID_DELETE_AUDIOPATH, (LPARAM)pAudiopath );
			}
		}
	}
	
	CFileListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
