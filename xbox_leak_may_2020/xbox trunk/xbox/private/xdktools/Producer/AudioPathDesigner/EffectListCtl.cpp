// EffectListCtl.cpp : implementation file
//

#include "stdafx.h"
#include "EffectListCtl.h"
#include "EffectInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectListCtl

CEffectListCtl::CEffectListCtl()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

}

CEffectListCtl::~CEffectListCtl()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

}


BEGIN_MESSAGE_MAP(CEffectListCtl, CListCtrl)
	//{{AFX_MSG_MAP(CEffectListCtl)
	ON_WM_DRAWITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectListCtl message handlers

void CEffectListCtl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the Effect
	EffectInfo *pEffectInfo = reinterpret_cast<EffectInfo *>(lpDrawItemStruct->itemData);
	if( !pEffectInfo )
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

		LVCOLUMN lvColumn;
		long lXPos = 0;

		long lScrollPos = -GetScrollPos( SB_HORZ );

		// Effect Name
		lvColumn.mask = LVCF_WIDTH;
		if( GetColumn( 0, &lvColumn ) )
		{
			RECT rectText = lpDrawItemStruct->rcItem;
			rectText.left = lScrollPos;
			rectText.right = lvColumn.cx + lScrollPos;
			::DrawText( lpDrawItemStruct->hDC, pEffectInfo->m_strName, -1, &rectText, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS );

			lXPos += lvColumn.cx;
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
