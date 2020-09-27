// checklis.cpp : implementation file
//

#include "stdafx.h"
#include "utilctrl.h"
#include "resource.h"

#define OBM_CHECKBOXES      32759 // stolen from windows.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckList
                              
CBitmap CCheckList::c_checkBitmap;
CSize CCheckList::c_checkSize;


CCheckList::CCheckList()
{
	m_cyItem = 0;
	m_cyText = 0;
}

BOOL CCheckList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	 return CListBox::Create(dwStyle | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS, 
	 	rect, pParentWnd, nID);
}

BEGIN_MESSAGE_MAP(CCheckList, CListBox)
	//{{AFX_MSG_MAP(CCheckList)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckList message handlers

int CCheckList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	ResetItemHeight();
	return 0;
}

LRESULT CCheckList::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	Default();
	ResetItemHeight();
	return 0;
}

void CCheckList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(m_cyItem > 0); // Must be set at create or subclass time
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect;
	rect.left = lpDrawItemStruct->rcItem.left + c_checkSize.cx + 2;
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
		
		CDC bitmapDC;
		if (bitmapDC.CreateCompatibleDC(pDC))
		{
			int nCheck = GetCheck(lpDrawItemStruct->itemID);
			CBitmap* pOldBitmap = bitmapDC.SelectObject(&c_checkBitmap);
			
			int xSrc, ySrc, xDest, yDest;
			if (theApp.m_bWin4)
			{
				xSrc = c_checkSize.cx * nCheck;
				ySrc = 0;
				xDest = 1;
				yDest = 0;
			}
			else
			{
				xSrc = c_checkSize.cx * (nCheck == 2 ? 1 : nCheck);
				ySrc = c_checkSize.cy * (nCheck == 2 ? 2 : 0);
				xDest = 1;
				yDest = 1;
			}
			
			pDC->BitBlt(lpDrawItemStruct->rcItem.left + xDest, 
				rect.top + yDest + max(0, (m_cyItem - c_checkSize.cy) / 2), 
				c_checkSize.cx, c_checkSize.cy, 
				&bitmapDC, xSrc, ySrc, SRCCOPY);
			bitmapDC.SelectObject(pOldBitmap);
		}
		
		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		CString strText; 
//		GetText(lpDrawItemStruct->itemID, strText);
		GetDisplayText(lpDrawItemStruct->itemID, pDC, rect, strText);
		ItemStringOut (pDC, strText, rect);
		
		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}
	
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(rect);
}

//virtual
void CCheckList::GetDisplayText(int index, CDC *pDC, const CRect&, CString& strOut)
{
	GetText(index, strOut);
}

void CCheckList::ItemStringOut (CDC * pDC, const CString& strOut, const CRect&  rect)
{
	pDC->ExtTextOut(rect.left, rect.top + max(0, (m_cyItem - m_cyText) / 2), 
			ETO_OPAQUE, rect, strOut, strOut.GetLength(), NULL);
}


void CCheckList::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	
	if (point.x < c_checkSize.cx && point.y < m_cyItem * (GetCount() - GetTopIndex()))
	{
		// Cheap button functionality: we toggle on the button down.  This is
		// less cool than tracking the mouse like a real checkbox, but if
		// WinWord can get away with this, so can we!

		int nItem = GetTopIndex() + point.y / m_cyItem;
	
		if ((GetStyle() & LBS_EXTENDEDSEL) != 0 && GetSel(nItem))
		{
			int nSelCount = GetSelCount();
			ASSERT(nSelCount > 0);

			INT* pSelItems = new INT [nSelCount];
			GetSelItems(nSelCount, pSelItems);

			int nCheck = GetCheck(nItem);
			nCheck = (nCheck + 1) % 2; // can't go back to tri-state!

			for (int i = 0; i < nSelCount; i += 1)
			{
				SetCheck(pSelItems[i], nCheck);
				InvalidateCheck(pSelItems[i]);
			}

			delete [] pSelItems;
		}
		else 
		{

			SetCheck(nItem, (GetCheck(nItem) + 1) % (IsTriState(nItem) ? 3 : 2));
			InvalidateCheck(nItem);
		}

		// Inform of check
		CWnd*	pParent = GetParent();
		ASSERT_VALID(pParent);
		pParent->PostMessage(WM_COMMAND,
			MAKEWPARAM(GetDlgCtrlID(), CLN_CHKCHANGE),
			(LPARAM)m_hWnd);

		return;
	}
	
	CListBox::OnLButtonDown(nFlags, point);
}

void CCheckList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (point.x < c_checkSize.cx)
	{
		// Double and single clicks act the same on the check box!
		OnLButtonDown(nFlags, point);
		return;
	}
	
	CListBox::OnLButtonDblClk(nFlags, point);
}

void CCheckList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE)
	{
		int		nCurSel = GetCurSel();
		CWnd*	pParent = GetParent();
		ASSERT_VALID(pParent);

		if ((GetStyle() & LBS_EXTENDEDSEL) != 0)
		{
			int nSelCount = GetSelCount();
			if (nSelCount == 0)
				return;
			
			INT* pSelItems = new INT [nSelCount];
			GetSelItems(nSelCount, pSelItems);
			
			int nCheck = GetCheck(nCurSel);
			nCheck = (nCheck + 1) % 2; // can't go back to tri-state!
			
			for (int i = 0; i < nSelCount; i += 1)
			{
				SetCheck(pSelItems[i], nCheck);
				InvalidateCheck(pSelItems[i]);
			}
			
			delete [] pSelItems;
			return;
		}
		else if ((GetStyle() & LBS_MULTIPLESEL) != 0)
		{
			// For multi-sel listbox's, we cycle through the four possible
			// state combinations on each space key.
			
			int w = ((GetSel(nCurSel) ? 1 : 0) |		// toggle selection
					 ((GetCheck(nCurSel) + 1) % (IsTriState(nCurSel) ? 3 : 2) << 1))
					 + 1;
			SetSel(nCurSel, (w & 1) != 0);
			SetCheck(nCurSel, (w & 6) >> 1);
			InvalidateCheck(nCurSel);

			// Inform of check
			pParent->PostMessage(WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(), CLN_CHKCHANGE),
				(LPARAM)m_hWnd);
			return;
		}
		else if (nCurSel != LB_ERR)
		{
			// If there is a selection, the space bar toggles that check,
			// all other keys are the same as a standard listbox.
			
			SetCheck(nCurSel, (GetCheck(nCurSel) + 1) % (IsTriState(nCurSel) ? 3 : 2));
			InvalidateCheck(nCurSel);

			// Inform of check
			pParent->PostMessage(WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(), CLN_CHKCHANGE),
				(LPARAM)m_hWnd);
			return;
		}
	}
	
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCheckList::InvalidateCheck(int nItem)
{
	CRect rect;
	GetItemRect(nItem, rect);
	rect.right = rect.left + c_checkSize.cx;
	InvalidateRect(rect, FALSE);
}

BOOL CCheckList::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	if (!CWnd::SubclassDlgItem(nID, pParent))
		return FALSE;
	
	ASSERT((GetStyle() & (LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS)) == 
		(LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS) ||
	(GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) == 
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));
	
	ResetItemHeight();
	
	return TRUE;
}

void CCheckList::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
	ResetItemHeight();
	lpmis->itemHeight = m_cyItem;
	lpmis->itemWidth  = (UINT)-1;
}

void CCheckList::ResetItemHeight()
{
	if (c_checkBitmap.m_hObject == NULL)
	{
		if (theApp.m_bWin4)
		{
			VERIFY(c_checkBitmap.LoadBitmap(IDB_CHECKLISTBOXES));
			BITMAP bitmap;
			c_checkBitmap.GetObject(sizeof (BITMAP), &bitmap);
			c_checkSize.cx = bitmap.bmWidth / 3;
			c_checkSize.cy = bitmap.bmHeight;
		}
		else
		{
			VERIFY(c_checkBitmap.LoadOEMBitmap(OBM_CHECKBOXES));
			BITMAP bitmap;
			c_checkBitmap.GetObject(sizeof (BITMAP), &bitmap);
			c_checkSize.cx = bitmap.bmWidth / 4;
			c_checkSize.cy = bitmap.bmHeight / 3;
		}
	}
	
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	TEXTMETRIC tm;
	VERIFY (dc.GetTextMetrics ( &tm ));
	m_cyText = tm.tmHeight;
	m_cyItem = max(c_checkSize.cy + 1, m_cyText);
	dc.SelectObject(pOldFont);
	SetItemHeight(0, m_cyItem);
}

void CCheckList::SetCheck(int nItem, int nCheck)
{
	VERIFY(SetItemData(nItem, nCheck) != LB_ERR);
	InvalidateCheck(nItem);
}

int CCheckList::GetCheck(int nItem)
{
	return GetItemData(nItem) != 0 ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////
// CPropCheckList -- a check list that can store property values

void CPropCheckList::OnPropChanged(UINT nIDProp)
{
	int iitem = 0, icount = GetCount();
	while (iitem < icount)
	{
		if (nIDProp == (GetItemData(iitem) >> 1))
			InvalidateCheck(iitem);

		iitem++;
	}
}

BOOL CPropCheckList ::IsTriState(int iitem)
{
	return FALSE;
	// Tri-state not supported until we have a better UI
	// to show undeterminate state for other dialog controls
	// eg. editboxes and listboxes
	// return (GetItemData(iitem) & 1) != 0;
}

void CPropCheckList::SetCheck(int iitem, int nval)
{
	UINT nProp = (UINT) (GetItemData(iitem) >> 1);

	ASSERT (nval != 2);
	// check or un-check
	m_pSlob->SetIntProp(nProp, nval);
}

int CPropCheckList::GetCheck(int iitem)
{
	UINT nProp = (UINT) (GetItemData(iitem) >> 1);

	int val = 0;
	if (m_pSlob->GetIntProp(nProp, val) == ambiguous)
	{
		SetItemData(iitem, (nProp << 1) + 1);	// set to a tri-state check
		return 2;	// indeterminate state
	}

	// checked or unchecked
	return val ? 1 : 0;
}

int CPropCheckList::AddItem(const char * szId, UINT nIDProp)
{
	int iitem = AddString((LPCSTR)szId);

	DWORD dwData = nIDProp << 1;
	// make sure we didn't drop bits!
	ASSERT(dwData >> 1 == (DWORD)nIDProp);

	if (iitem != CB_ERR) (void) SetItemData(iitem, dwData);
	return iitem;
}
