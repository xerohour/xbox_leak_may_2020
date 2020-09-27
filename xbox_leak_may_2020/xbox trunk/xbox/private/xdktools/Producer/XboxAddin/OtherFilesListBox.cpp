// OtherFilesListBox.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "OtherFilesListBox.h"
#include "OtherFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PLAY_ICON_HEIGHT (16)
#define PLAY_ICON_WIDTH (16)
#define PLAY_BUTTON_WIDTH (16);

/////////////////////////////////////////////////////////////////////////////
// COtherFilesListBox

COtherFilesListBox::COtherFilesListBox()
{
}

COtherFilesListBox::~COtherFilesListBox()
{
}


BEGIN_MESSAGE_MAP(COtherFilesListBox, CFileListBox)
	//{{AFX_MSG_MAP(COtherFilesListBox)
	ON_WM_DRAWITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherFilesListBox message handlers

void COtherFilesListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Get the OtherFile
	COtherFile *pOtherFile = reinterpret_cast<COtherFile *>(lpDrawItemStruct->itemData);
	if( !pOtherFile )
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

        //DFCS_PUSHED
        rect.right = PLAY_BUTTON_WIDTH;
        //::DrawFrameControl( lpDrawItemStruct->hDC, &rect, DFC_BUTTON, DFCS_BUTTONPUSH );

        int nYPos = ((rect.bottom + rect.top) / 2) - (PLAY_ICON_HEIGHT / 2);
        ::DrawIconEx( lpDrawItemStruct->hDC, rect.left, nYPos, pOtherFile->GetIcon(), PLAY_ICON_WIDTH, PLAY_ICON_HEIGHT, 0, NULL, DI_NORMAL );

        // Set up the text rectangle
        rect.left = PLAY_BUTTON_WIDTH;
        rect.right = lpDrawItemStruct->rcItem.right;

		// File Name
		::DrawText( lpDrawItemStruct->hDC, pOtherFile->GetName(), -1, &rect, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

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
