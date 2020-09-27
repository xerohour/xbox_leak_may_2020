#include "stdafx.h"
#include "utilctrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CIconList class

void CIconList::SetIcon(int nItem, HICON hIcon)
{
	SetItemData(nItem, (DWORD)hIcon);
}

HICON CIconList::GetIcon(int nItem)
{
	return (HICON)GetItemData(nItem);
}

BOOL CIconList::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	if (!CWnd::SubclassDlgItem(nID, pParent))
		return FALSE;
	
	ASSERT((GetStyle() & (LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS)) == 
		(LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS) ||
	(GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) == 
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));
	
	return TRUE;
}

void CIconList::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
	lpmis->itemHeight = 1 + 32 + 1;
	lpmis->itemWidth  = (UINT)-1;
}

void CIconList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect;
	rect.left = lpDrawItemStruct->rcItem.left;
	rect.top = lpDrawItemStruct->rcItem.top;
	rect.right = lpDrawItemStruct->rcItem.right;
	rect.bottom = lpDrawItemStruct->rcItem.bottom;
	
	if (lpDrawItemStruct->itemID >= 0 &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0)
	{
		BOOL fDisabled = !IsWindowEnabled();

		COLORREF newTextColor = fDisabled ? RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);	// light gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);	// dark gray
		
		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		CString strText; 
		GetText(lpDrawItemStruct->itemID, strText);
		CSize size = pDC->GetTextExtent(strText);
		pDC->ExtTextOut(rect.left + 1 + 32 + 4, rect.top + max(0, 1 + (32 - size.cy) / 2), 
				ETO_OPAQUE, rect, strText, strText.GetLength(), NULL);
		
		HICON hIcon = GetIcon(lpDrawItemStruct->itemID);
		if (hIcon != NULL)
			pDC->DrawIcon(rect.left + 1, rect.top + 1, hIcon);

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}
	
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(rect);
}
