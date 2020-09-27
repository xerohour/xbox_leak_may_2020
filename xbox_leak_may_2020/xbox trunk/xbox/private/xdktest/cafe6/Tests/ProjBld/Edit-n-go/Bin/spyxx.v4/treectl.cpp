// treectl.cpp : implementation file
//

//[byrond] Change to Spy++ headers.
#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "imgwell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int CTreeCtl::c_cyBorder = GetSystemMetrics(SM_CYBORDER);
int CTreeCtl::c_cyBorderX2 = c_cyBorder * 2;

// This image well contains: +box, -box, closed-folder, open-folder
CImageWell CTreeCtl::c_imageWell;
#define nPlusImage		0
#define nMinusImage		1
#define nClosedImage	2
#define nOpenImage		3

// width&height of the boxed + and - images
#define cxExpandBox 11
#define cyExpandBox 11

// size of the folder images
#define cxFolder	16
#define cyFolder	15


/////////////////////////////////////////////////////////////////////////////
// CTreeCtl

CTreeCtl::CTreeCtl(BOOL bIsMultiSelection /*= FALSE*/, BOOL bKeepSelectionRect /*=FALSE*/)
{
	if (!c_imageWell.IsOpen())
	{
		VERIFY(c_imageWell.Load(IDB_TREECTL, CSize(16, 16)));
		VERIFY(c_imageWell.Open());
		VERIFY(c_imageWell.CalculateMask());
	}
	
	m_bIsMultiSelection = bIsMultiSelection;
	m_bKeepSelectionRect= bKeepSelectionRect;
	m_EnableUpdates = 0;
	m_bEnableDragDrop = FALSE;
	m_bShowContextPopup = FALSE;
	m_bShowProperties = FALSE;
	m_timeLastKey = 0;
	m_bSorted = TRUE;		   //[byrond] needed for unsorted tree support
}

CTreeCtl::~CTreeCtl()
{
	if (m_hWnd != NULL)
		DestroyWindow();
}

BOOL CTreeCtl::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(0);
			
	return TRUE;
}

IMPLEMENT_DYNAMIC(CTreeListBox, CWnd)

BEGIN_MESSAGE_MAP(CTreeListBox, CWnd)
	//{{AFX_MSG_MAP(CTreeListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CTreeCtl, CWnd)

BEGIN_MESSAGE_MAP(CTreeCtl, CWnd)
	//{{AFX_MSG_MAP(CTreeCtl)
	ON_WM_CHARTOITEM()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_VKEYTOITEM()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_LBN_DBLCLK(IDCW_TREELISTBOX, OnLbDblClk)
	ON_LBN_SELCHANGE(IDCW_TREELISTBOX, OnLbSelChange)
	ON_LBN_SETFOCUS(IDCW_TREELISTBOX, InvalSelection)
	ON_LBN_KILLFOCUS(IDCW_TREELISTBOX, InvalSelection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl message handlers

int CTreeCtl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CClientDC dc(this);
	ResetSizes(&dc);
	
	FlushTheCache();	//[byrond]

	m_listBox.m_pTree = this;
	if (!m_listBox.Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER |
		LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT |
		LBS_WANTKEYBOARDINPUT | ( (m_bIsMultiSelection) ? LBS_EXTENDEDSEL : 0),
		CRect(0, 0, 0, 0), this, IDCW_TREELISTBOX))
	{
		return -1;
	}
	
	SetFont(GetStdFont(font_Normal));
	
	return 0;
}

void CTreeCtl::OnDestroy()
{
	FreeAllTreeData();
}

void CTreeCtl::OnPaint()
{
	// we want to search through the tree control
	// for any dirty nodes, if we find one of these
	// we want to update it's contents

	// [patbr] don't need to do this in Spy++ since nodes never get dirty...
	// RefreshAllRoots();		// refresh from our roots down

	CWnd::OnPaint();	// pass on to our base instance
}

void CTreeCtl::ResetSizes(CDC* pDC)
{
	// REVIEW: Should maybe be using TEXTMETRIC's
	m_sizeM = pDC->GetTextExtent("M", 1);
	// Make sure this is even so our brush lines up when we scroll:
	m_cyNode = (max(m_sizeM.cy, cyFolder) + 2) & ~1;
		
	m_cxExtent = 0; // not calculated yet...
}

void CTreeCtl::RecalcHorizontalExtent ()
{
	m_cxExtent = 0;
	int nIndex = m_listBox.GetCount();
	while (--nIndex >= 0)
	{
		CNode * pNode = (CNode *)m_listBox.GetItemDataPtr(nIndex);
		if (pNode->m_nWidth > m_cxExtent) m_cxExtent = pNode->m_nWidth;
	}

	// If the listbox fits, set the scroll position to 0 to make sure
	// the scroll bar goes away:
	CRect rcWindow;
	m_listBox.GetClientRect(rcWindow);
	if (m_cxExtent <= rcWindow.right && m_listBox.GetScrollPos(SB_HORZ))
		m_listBox.SendMessage(WM_HSCROLL, SB_TOP);

	m_listBox.SetHorizontalExtent(m_cxExtent);

}
	
LPARAM CTreeCtl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	CFont* pFont = CFont::FromHandle((HFONT)wParam);

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(pFont);
	ResetSizes(&dc);
	RecalcHorizontalExtent();
	dc.SelectObject(pOldFont);

	m_listBox.SetFont(pFont);
	m_listBox.SetItemHeight(0, m_cyNode);
	
	return 0;
}

LPARAM CTreeCtl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return m_listBox.SendMessage(WM_GETFONT, wParam, lParam);
}

void CTreeCtl::OnSetFocus(CWnd* pOldWnd)
{
	m_listBox.SetFocus();
}

void CTreeCtl::InvalItem(int nItem)
{
	CRect rect;
	m_listBox.GetItemRect(nItem, rect);
	m_listBox.InvalidateRect(rect);
}

void CTreeCtl::InvalSelection()
{
	if (m_bIsMultiSelection)
	{
		CRect client;
		m_listBox.GetClientRect(client);
		for (int nItem = m_listBox.GetTopIndex(); nItem != -1; nItem += 1)
		{
			CRect rect;
			if (m_listBox.GetItemRect(nItem, &rect) == LB_ERR || rect.top >= client.bottom)
				break;
			
			if (m_listBox.GetSel(nItem))
				m_listBox.InvalidateRect(rect);
		}
	}
	else
	{
		InvalItem(GetCurSel());
	}
}

void CTreeCtl::OnSize(UINT nType, int cx, int cy)
{
	if (GetParent()->IsIconic())
		return;

	// Put the client area of the listbox in the client area of the tree.
	CRect rectNew, rectCurrent;
	GetClientRect(rectNew);
	::AdjustWindowRect(rectNew, m_listBox.GetStyle(), FALSE);

	m_listBox.GetWindowRect(rectCurrent);
	ScreenToClient(rectCurrent);

	if (rectNew == rectCurrent)
		return;
		
	m_listBox.MoveWindow(rectNew.left, rectNew.top,
		rectNew.Width(), rectNew.Height());
		
	int iMax = GetCurSel();
	if (iMax >= 0)
	{
		int top, bottom;
			
		m_listBox.GetClientRect(rectCurrent);
		top = m_listBox.GetTopIndex();
		bottom = top + rectCurrent.bottom / m_cyNode;
		if (iMax < top || iMax > bottom)
			m_listBox.SetTopIndex(iMax - (bottom - top) / 2);
	}
	RecalcHorizontalExtent();
}


// REVIEW: This must be made to work with or without DBCS at all times!  (NO IFDEFS)
int CTreeCtl::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
#ifdef DBCS
	char szB [3], szW [3];
#else
	char szB [2];
#endif

	int nItems = m_listBox.GetCount();
	int i = GetCurSel();
	
	if (i < 0 || nChar <= ' ')
		return -2;
	
#ifdef DBCS
	if (nChar > 255)
		nChar = (nChar << 8) | ((nChar >> 8) & 0xFF);
	szW[0] = (unsigned char)nChar;
	szW[1] = (unsigned char)(nChar >> 8);
	szW[2] = '\0';
	szB[2] = '\0';
#else
	szB[1] = '\0';
	nChar &= 0xFF;
#endif

	for (int j = 1; j < nItems; j += 1)
	{
		CNode* pNode = GetNode((i + j) % nItems);
		szB[0] = pNode->m_szText[0];
		
#ifdef DBCS
	if (IsDBCSLeadByte(szB[0]))
		szB[1] = pNode->m_szText[1];
	else
		szB[1] = '\0';
	
	if (lstrcmpi(szW, szB) == 0)
		break;
#else
	if (lstrcmpi((char*)&nChar, szB) == 0)
		break;
#endif

	}
	
	if (j == nItems)
		return -2;
	
	m_listBox.SetTopIndex((i + j) % nItems);
	
	return (i + j) % nItems;
}

BOOL CTreeCtl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message != WM_KEYDOWN || GetParentFrame()->IsIconic())
		return FALSE;
		
	switch (pMsg->wParam)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_BACK:
	case VK_RETURN:
	case VK_ADD:
	case VK_SUBTRACT:
	case VK_MULTIPLY:
		// Make sure we get the keys we handle in OnKeyDown!
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}
	
	return FALSE;
}

// OnKeyDown and OnChar
//
// These two functions are called as the listbox receives keyboard input,
// and give you a chance to add specific keyboard processing for your
// tree.
//
// Note that you will receive both an OnKeyDown AND an OnChar for character
// input.
//
// Return TRUE if you handle this message, FALSE if you want default listbox
// handling to occur.
//
BOOL CTreeCtl::OnKeyDown(UINT nChar)
{
	int i = GetCurSel();
	if (i < 0)
		return TRUE;

	int j = 1;
	CNode* pNode = GetNode(i);
	CNode* pNodeT;

	switch (nChar)
	{
	case VK_LEFT:
		while ((pNodeT = GetNode(--i)) != NULL)
		{
			if (pNodeT == pNode->m_pParent)
			{
				KeySetSel(i);
				return TRUE;
			}
		}
		goto LSameSelection;
		
	case VK_RIGHT:
		if ((pNodeT = GetNode(i + 1)) == NULL || pNodeT->m_pParent != pNode)
		{
			goto LSameSelection;
		}
		KeySetSel(i + 1);
		return TRUE;
		
	case VK_UP:
		j = -1;
		// FALL THROUGH
		
	case VK_DOWN:
		if (GetKeyState(VK_CONTROL) >= 0)
		{
			ClearSearchBuffer();
			return FALSE;
		}
		
		while ((i += j) >= 0 && (pNodeT = GetNode(i)) != NULL)
		{
			if (pNodeT->m_pParent == pNode->m_pParent)
			{
				KeySetSel(i);
				return TRUE;
			}
		}
LSameSelection:
		MessageBeep(0);
		return TRUE;
		
	case VK_BACK:
		{
			if (i <= 0)
				return TRUE;
			
			int nStartLevel = pNode->m_nLevels;
			do
			{
				pNodeT = GetNode(--i);
			}
			while (i > 0 && pNodeT->m_nLevels >= nStartLevel);
			
			KeySetSel(i);
			return TRUE;
		}
		
	case VK_RETURN:
		OnPrimary();
		return TRUE;

	case VK_ADD:
		BeginWaitCursor();
		OnExpand();
		EndWaitCursor();
		return TRUE;
		
	case VK_SUBTRACT:
		BeginWaitCursor();
		OnCollapse();
		EndWaitCursor();
		return TRUE;
		
	case VK_MULTIPLY:
		BeginWaitCursor();
		if (GetKeyState(VK_CONTROL) < 0)
			OnExpandAll();
		else
			OnExpandBranch();
		EndWaitCursor();
		return TRUE;
	}

	return FALSE;
}

BOOL CTreeCtl::OnChar(UINT nChar)
{
	switch (nChar)
	{
	case '+':
		if (GetKeyState(VK_ADD) >= 0)
		{
			BeginWaitCursor();
			OnExpand();
			EndWaitCursor();
		}
		return TRUE;
		
	case '-':
		if (GetKeyState(VK_SUBTRACT) >= 0)
		{
			BeginWaitCursor();
			OnCollapse();
			EndWaitCursor();
		}
		return TRUE;
		
	case '*':
		if (GetKeyState(VK_MULTIPLY) >= 0)
		{
			BeginWaitCursor();
			if (GetKeyState(VK_CONTROL) < 0)
				OnExpandAll();
			else
				OnExpandBranch();
			EndWaitCursor();
		}
		return TRUE;
	}
	
	// if enough time has elapsed, or the user hits a control key (e.g.
	// backspace) zero the string.
	time_t timeNow = time(NULL);
	
	// need a place to store the lead byte of a DBC
	static TCHAR cLead;

	if (iscntrl(nChar) || (timeNow - m_timeLastKey > LIST_SEARCH_DELAY))
		ClearSearchBuffer();

	// process the key
	if ((m_listBox.GetCount() == 0) || iscntrl(nChar))
		return FALSE;

	if (cLead)
	{
		TCHAR szTmp[3];
		szTmp[0] = cLead;
		szTmp[1] = (TCHAR) nChar;
		szTmp[2] = _T('\0');
		
		cLead = 0;
		m_strSearch += CString( szTmp );
	}
	else
	{
		if (IsDBCSLeadByte( (char) nChar ))
		{
			cLead = (TCHAR) nChar;
			return TRUE;
		}

		m_strSearch += CString((char)nChar);
	}
	
	m_timeLastKey = timeNow;

	// look for a match
	CString str;
	int index, indexStart;
	index = indexStart = max(GetCurSel(), 0);

	do
	{
		GetNodeTitle(GetNode(index), str);
		if (m_strSearch.CompareNoCase(str.Left(m_strSearch.GetLength())) == 0)
		{
			// found a match
			KeySetSel(index, FALSE);
			return TRUE;
		}

		if (++index >= m_listBox.GetCount())
			index = 0;
	} while (index != indexStart);

	// no match found, but we did handle the key
	return TRUE;
}

void CTreeCtl::KeySetSel(int nIndex, BOOL bClearSearchBuffer)
{
	// called when user selects a specific item with the keyboard
	if (bClearSearchBuffer)
		ClearSearchBuffer();

	if (m_bIsMultiSelection)
		SetSel(-1, FALSE);

	SetSel(nIndex, TRUE);
	OnLbSelChange();
}

void CTreeCtl::ClearSearchBuffer()
{
	m_strSearch.Empty();
}



////////////////////////////////////////////////////////////////////////////
// Outline "command" handlers...

void CTreeCtl::OnPrimary()
{
	if (m_listBox.GetCaretIndex() != -1)
		OnOpen(m_listBox.GetCaretIndex());
	ClearSearchBuffer();
}

void CTreeCtl::OnExpand()
{
	CNode* pNode = GetNode();
	if (pNode != NULL && (pNode->m_nFlags & TF_EXPANDABLE) != 0 && !IsExpanded())
		Expand();
	ClearSearchBuffer();
}

void CTreeCtl::OnCollapse()
{
	CNode* pNode = GetNode();
	int nItem = m_listBox.GetCurSel();
	
	if ((pNode->m_nFlags & TF_EXPANDABLE) == 0)
	{
		if (nItem <= 0)
			return;
		
		int nStartLevel = pNode->m_nLevels;
		do
		{
			pNode = GetNode(--nItem);
		}
		while (nItem > 0 && pNode->m_nLevels >= nStartLevel);
	}
	
	if (pNode != NULL && IsExpanded(nItem))
		Collapse(nItem);
		
	ClearSearchBuffer();
}

void CTreeCtl::OnExpandBranch()
{
	int nIndex = GetCurSel();
	if (nIndex != LB_ERR)
		Expand(nIndex, TRUE);
	ClearSearchBuffer();
}

void CTreeCtl::OnExpandAll()
{
	// UNDONE: these cases could be consolidated
	SetCurSel(0);	// work-around for #2909
	ExpandAll();
	ClearSearchBuffer();
}

////////////////////////////////////////////////////////////////////////////


void CTreeCtl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_cyNode;
}

void CTreeCtl::DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL bHighlight, CPoint pt)
{
	// Override this function to provide custom glyphs for folders or other nodes...
	
	c_imageWell.DrawImage(pDC, pt, (pNode->m_nFlags & TF_EXPANDED) != 0 ? 
		nOpenImage : nClosedImage);
}

void CTreeCtl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// dolphin 13143 [patbr] tree control faults with empty list box
	if (lpDrawItemStruct->itemData == NULL)
	{
		// list box is empty (only when Daytona not allowing access 
		// to process/thread info), so indicate that list box has focus.
		DrawFocusRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem));
		return;								 
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CNode* pNode = (CNode*)lpDrawItemStruct->itemData;
//	CFont* pOldFont = pDC->SelectObject(pNode->GetFont());
	int cchText = _tcslen(pNode->m_szText);
	
	// our drawing starting point for this node from left and top
	CPoint	pt(c_cyBorderX2 + pNode->m_nLevels * m_sizeM.cx * 2, lpDrawItemStruct->rcItem.top);

	// our focus/highlight rectangle for the text
	CRect rect;

	rect.top = lpDrawItemStruct->rcItem.top;
	rect.left = pt.x + 2 * m_sizeM.cx + pNode->GetGlyphWidth() + c_cyBorderX2;
	rect.bottom = lpDrawItemStruct->rcItem.bottom;
	rect.right = rect.left + pDC->GetTextExtent(pNode->m_szText, cchText).cx + 4 * c_cyBorderX2;
	
	if (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		int x = pt.x + m_sizeM.cx;	// initial drawing starting position
		x &= ~1;	// Make even;
		int y = pt.y + m_cyNode / 2; 
	//	int yp = y | 1; 

		BOOL bDrawSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 ||
			(pNode->m_nFlags & TF_DROPTARGET) != 0;
		BOOL bHasFocus = ::GetFocus() == lpDrawItemStruct->hwndItem ||
			(pNode->m_nFlags & TF_DROPTARGET) != 0;
		COLORREF rgbText, rgbBackground;

		// First see if we have tested for expandability yet
		if (!((pNode->m_nFlags & TF_EXPANDABLE) || (pNode->m_nFlags & TF_NOT_EXPANDABLE)))
		{
			pNode->m_nFlags |= NodeIsExpandable(pNode) ? TF_EXPANDABLE : TF_NOT_EXPANDABLE;
			if (pNode->m_nFlags	& TF_NOT_EXPANDABLE)
			{
				pNode->m_nFlags &= ~TF_EXPANDED;
			}
		}
		
		
		// Draw the dotted lines...
		// use system color (button shadow) for tree control dotted lines (as in Windows95)
		COLORREF rgbForeground = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		CBrush* pOldBrush = pDC->SelectObject(CDC::GetHalftoneBrush());

		if (pNode->m_pParent != NULL || (pNode->m_nFlags & TF_EXPANDABLE) != 0)
		{
			// Draw the horizontal line over the (possible) folder
			// Draw on an odd lines so our hatch brush looks nice.
			pDC->PatBlt (x, y, m_sizeM.cx, 1, PATCOPY);
		}
		
		if (pNode->m_pParent != NULL)
		{
			// Draw the top part of the vertical line
			pDC->PatBlt (x, rect.top, 1, m_cyNode / 2, PATCOPY); 
			
			// If not the end of a node, draw the bottom part
			if ((pNode->m_nFlags & TF_LASTLEVELENTRY) == 0)
			{
				pDC->PatBlt (x, y + c_cyBorder, 1, m_cyNode / 2, PATCOPY); 
			}

			// Draw the verticals on the left connecting other nodes
			int nLevel = pNode->m_nLevels;
			for (CNode* pNodeT = pNode->m_pParent; pNodeT != NULL; pNodeT = pNodeT->m_pParent)
			{
				nLevel -= 1;

				if (!(pNodeT->m_nFlags & TF_LASTLEVELENTRY) && (pNodeT->m_pParent != NULL))
				{
					int	x = (nLevel + 1) * m_sizeM.cx * 2 + c_cyBorderX2 - m_sizeM.cx;
					x &= ~1;	// Make even;
					pDC->PatBlt(x, rect.top, 1, m_cyNode, PATCOPY); 
				}
			}
		}
		
		pDC->SetTextColor(rgbForeground);
		pDC->SelectObject(pOldBrush);
		
		
		// Draw the boxed plus/minus sign if appropriate...

		//[byrond] Use our own brush
		pDC->SelectObject ( &theApp.m_BrushWindow );
		
		if ((pNode->m_nFlags & TF_EXPANDABLE) != 0)
		{
			// Draw the box	[+] or [-]
			c_imageWell.DrawImage(pDC, 
				CPoint(x - cxExpandBox / 2, y - cyExpandBox / 2), 
				(pNode->m_nFlags & TF_EXPANDED) ? nMinusImage : nPlusImage);

			// use system color (button shadow) for plus/minus sign frames (as in Windows95)
			COLORREF rgbForeground = GetSysColor(COLOR_BTNSHADOW);
			CBrush br(rgbForeground);
			pOldBrush = pDC->SelectObject(&br);

			pDC->PatBlt(x - cxExpandBox / 2 + 1, y - cyExpandBox / 2 + 1, cxExpandBox - 2, 1, PATCOPY); 
			pDC->PatBlt(x - cxExpandBox / 2 + 1, y + cyExpandBox / 2 - 1, cxExpandBox - 2, 1, PATCOPY); 
			pDC->PatBlt(x - cxExpandBox / 2 + 1, y - cyExpandBox / 2 + 1, 1, cyExpandBox - 2, PATCOPY); 
			pDC->PatBlt(x + cxExpandBox / 2 - 1, y - cyExpandBox / 2 + 1, 1, cyExpandBox - 2, PATCOPY); 
	
			pDC->SelectObject(pOldBrush);
		}
		
		
		// Draw the glyph...
		
		DrawNodeGlyph(pDC, pNode, bDrawSelected && bHasFocus,
			CPoint(x + m_sizeM.cx + c_cyBorder, y - (cyFolder / 2)));
		
		
		// Draw the text...
		
		if ((bHasFocus || m_bKeepSelectionRect) && bDrawSelected)
		{
			rgbText = pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			rgbBackground = pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}
		
		pDC->ExtTextOut(rect.left + c_cyBorderX2 * 2,
			y - (m_sizeM.cy / 2), ETO_OPAQUE, &rect, pNode->m_szText, cchText, NULL);
		
		if (bDrawSelected)
		{
			if (bHasFocus || m_bKeepSelectionRect)
			{
				pDC->SetTextColor(rgbText);
				pDC->SetBkColor(rgbBackground);
			}
			else
			{
				// Highlight is a frame when we don't have the focus...
				CBrush brush;
				if (brush.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)))
					pDC->FrameRect(&rect, &brush);
			}
		}
	}
	
	if (lpDrawItemStruct->itemAction == ODA_FOCUS)
		pDC->DrawFocusRect(&rect);

//	pDC->SelectObject(pOldFont);
}


void CTreeCtl::Collapse(CNode* pNode, int nIndex)
{
	CNode* pParentNode = pNode;
	int nIndexT = nIndex;
	int cDeleted = 0;
	int iCurSel = m_listBox.GetCurSel();
	
	m_listBox.SetRedraw(FALSE);
	
	nIndexT += 1;
	
	for (;;)
	{
		if ((pNode = GetNode(nIndexT)) == NULL)
			break;
		
		if (pNode->m_nLevels <= pParentNode->m_nLevels)
			break;

		DeleteNode(pNode);
		cDeleted++;
		
		m_listBox.DeleteString(nIndexT);
	}
	
	FlushTheCache();	//[byrond]
	pParentNode->m_nFlags &= ~TF_EXPANDED;

	if (iCurSel > nIndex)
	{
		// selection/focus needs was after the collapsed item,
		// need to adjust...

		if (iCurSel <= nIndex + cDeleted)
		{
			// selection/focus was inside the block of deleted
			// items, make the top level node the selected node
			iCurSel = nIndex;

		}
		else
		{
			// selection/focus was after the block of deleted
			// items, move the selection up by the number of
			// deleted items

			iCurSel -= cDeleted;
		}

		m_listBox.SetCurSel(iCurSel);

		if (m_bIsMultiSelection)
		{
			m_listBox.SetCaretIndex(iCurSel);
			m_listBox.SetSel(iCurSel, TRUE);
		}

	#ifdef _WIN32
		SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetListBox()->GetSafeHwnd());
	#else
		SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
			MAKELPARAM(GetListBox()->GetSafeHwnd(), LBN_SELCHANGE));
	#endif
	}
	
	if (m_EnableUpdates == 0) 
	{
		m_listBox.SetRedraw (TRUE);
		m_listBox.Invalidate();
	}
}

void CTreeCtl::Refresh(CNode *pParentNode, int nIndex)
{
	CNode * pNextNode;
	int nIndexT;

	// is this node dirty?
	if (pParentNode->m_nFlags & TF_DIRTY_CONTENT)
	{
		// reset this node dirty bit
		pParentNode->m_nFlags &= ~TF_DIRTY_CONTENT;

		// if is is expanded? then collapse it
		if (pParentNode->m_nFlags & TF_EXPANDED)
			Collapse(nIndex);

		// reset the expandablity of this node
		pParentNode->m_nFlags &= ~(TF_EXPANDABLE | TF_NOT_EXPANDABLE);
		pParentNode->m_nFlags |= NodeIsExpandable(pParentNode) ? TF_EXPANDABLE : TF_NOT_EXPANDABLE;
		// is it potentially expandable?
		if (pParentNode->m_nFlags & TF_EXPANDABLE)
		{
			Expand(nIndex, FALSE);
		}
	}
	// if we have children and we are expanded
	// then perform this function on each of the children first
	else if (pParentNode->m_nFlags & (TF_HASCHILDREN | TF_EXPANDED))
	{
		nIndexT = nIndex + 1;
		for (;;)
		{
			if ((pNextNode = GetNode(nIndexT)) == NULL)
				break;	// do we have a child? ie. not end of list
		
			if (pNextNode->m_nLevels <= pParentNode->m_nLevels)
				break;	// is this one of our siblings?

			if (pNextNode->m_nLevels == (pParentNode->m_nLevels + 1))
				Refresh(pNextNode, nIndexT);	// refresh this child

			nIndexT++;
		}
	}


	if (pParentNode->m_nFlags & TF_DIRTY_ITEM)
	{
		// reset this node dirty bit
		pParentNode->m_nFlags &= ~TF_DIRTY_ITEM;

		// get the (possibly new) text for this node
		CString	strTitle;
		GetNodeTitle(pParentNode, strTitle);

		ASSERT(pParentNode->m_szText != NULL);
		delete [] pParentNode->m_szText;
		pParentNode->m_szText = new char[strTitle.GetLength() + 1];
		_tcscpy(pParentNode->m_szText, strTitle);
		// [byrond] Don't need to flush the cache
		// because the indices didn't change
	}
}

void CTreeCtl::FreeAllTreeData()
{
	int nIndex = m_listBox.GetCount();
	while (--nIndex >= 0)
	{
		CNode * pNode = (CNode *)m_listBox.GetItemDataPtr(nIndex);

		DeleteNode(pNode);
	}
	m_listBox.ResetContent();
	FlushTheCache();	//[byrond]
}

int CTreeCtl::CompareData(const CNode* pNode1, const CNode* pNode2)
{
	if (pNode1 == pNode2)
		return 0;
	
	int ret = CompareData(pNode1->m_pParent, pNode2->m_pParent);
	
	if (ret == 0)
	{
		ret = _tcsicmp (pNode1->m_szText, pNode2->m_szText);

		// if we have a match then compare the data
		if (ret == 0 && (pNode1->m_dwData != 0 || pNode2->m_dwData != 0))
		{
			if (pNode1->m_dwData == pNode2->m_dwData)
				ret = 0;
			else
				ret = (int) (pNode1->m_dwData - pNode2->m_dwData);
		}
	}
	
	return ret;
}

int CTreeCtl::CompareNodes(const CNode* pNode1, const CNode* pNode2)
{
	const CNode* pSav1;
	const CNode* pSav2;
	
	pSav1 = pNode1;
	pSav2 = pNode2;
	
	while (pNode1->m_nLevels > pNode2->m_nLevels)
		pNode1 = pNode1->m_pParent;
		
	while (pNode2->m_nLevels > pNode1->m_nLevels)
		pNode2 = pNode2->m_pParent;

	int ret = CompareData(pNode1, pNode2);

	if (ret == 0)
		ret = pSav1->m_nLevels - pSav2->m_nLevels;
	
	return ret;
}

int CTreeCtl::FindNode(DWORD dwData)
{
	int nCount = m_listBox.GetCount();
	for (int i = 0; i < nCount; i += 1)
	{
		if (GetNode(i)->m_dwData == dwData)
			return i;
	}
	
	return LB_ERR;
}

int CTreeCtl::FindNode(const TCHAR * szText, DWORD & dwData, BOOL fMatchCase)
{
	int nCount = m_listBox.GetCount();
	for (int i = 0; i < nCount; i += 1)
	{
		CNode * pNode = GetNode(i); ASSERT(pNode != (CNode *)NULL);

		if ((fMatchCase && !_tcsicmp(pNode->m_szText, szText) || !_tcscmp(pNode->m_szText, szText)))
		{
			dwData = pNode->m_dwData;
			return i;	// success, return index of node and assoc. data
		}
	}

	return LB_ERR;	// not found
}

int CTreeCtl::InsertNode(int iParentNode, const char* szName, DWORD dwData)
{
	CNode * pNode;
	int iNode;
	
	if (!InsertNode(GetNode(iParentNode), iParentNode, szName, dwData, pNode, iNode))
		return LB_ERR;
	
	return iNode;
}

void CTreeCtl::DirtyNode(int iNode, WORD tf)
{
	CRect rect;
	CNode * pNode = GetNode(iNode);
	ASSERT(pNode != NULL);
	pNode->m_nFlags |= tf;

	Invalidate();
}

void CTreeCtl::RemoveNode(int iNode, BOOL bUpdateSel /* = TRUE*/)
{
	CRect rect,rect1;
	CNode* pNode = GetNode(iNode), *pNode1;
	int i;
	ASSERT(pNode != NULL);
	if ((pNode->m_nFlags & TF_EXPANDED) != 0)
		Collapse(pNode, iNode);

	// Don't know anymore if parent is expandable or not:
	if (pNode->m_pParent != NULL)
		pNode->m_pParent->m_nFlags &= ~(TF_EXPANDABLE | TF_NOT_EXPANDABLE);

	if (pNode->m_nFlags & TF_LASTLEVELENTRY)
	{
		m_listBox.GetItemRect(iNode, &rect);
		i = iNode - 1;
		do
		{
			ASSERT(i >= 0);
			pNode1 = GetNode(i);
			m_listBox.GetItemRect(i, &rect1);
			rect.UnionRect (rect, rect1);
			i--;
		}
		while (i >= 0 && pNode1->m_nLevels != pNode->m_nLevels);

		if (pNode1->m_nLevels == pNode->m_nLevels)
			pNode1->m_nFlags |= TF_LASTLEVELENTRY;
		m_listBox.InvalidateRect(&rect);
	}

	int iCurSel = m_listBox.GetCurSel();
	m_listBox.DeleteString(iNode);
	DeleteNode(pNode);
	if (iCurSel == iNode && bUpdateSel)
	{
		int nCount = m_listBox.GetCount();
		if (iCurSel > nCount - 1)
			iCurSel = nCount - 1;

		m_listBox.SetCurSel(iCurSel);

		if (m_bIsMultiSelection)
		{
			m_listBox.SetCaretIndex(iCurSel);
			m_listBox.SetSel(iCurSel, TRUE);
		}

#ifdef _WIN32
	    SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetListBox()->GetSafeHwnd());
#else
	    SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	        MAKELPARAM(GetListBox()->GetSafeHwnd(), LBN_SELCHANGE));
#endif
	}
	FlushTheCache();	//[byrond]
}

BOOL CTreeCtl::InsertNode(CNode* pParentNode, int iParentNode, 
	const char* szName, DWORD dwData, CNode*& rpNodeRet, int& riNodeRet)
{
	CClientDC dc(this);
//	[patbr] don't reset the font--horz. scroll bars will screw up.
	dc.SelectObject((CFont *)CFont::FromHandle((HFONT)SendMessage(WM_GETFONT, 0, 0)));
	
	int len = _tcslen(szName);
	CNode* pNode = CreateNode(dwData);
	
	pNode->m_pParent = pParentNode;

#ifdef _DEBUG
	// make sure that the parent links are valid CNode address
	// (we'd use ASSERT_VALID, but CNode is not a CObject)
	CNode * pTemp = pNode;
	while ((pTemp = pTemp->m_pParent) != NULL)
	{	
		ASSERT(AfxIsValidAddress(pTemp, sizeof(CNode)));
	}
#endif

	pNode->m_nLevels = pParentNode != NULL ? (pParentNode->m_nLevels + 1) : 0;
	pNode->m_dwData = dwData;
	pNode->m_nFlags = 0;
	pNode->m_szText = new char[len + 1];
	_tcscpy(pNode->m_szText, szName);
	
	if (pParentNode != NULL)
	{
		pParentNode->m_nFlags |= TF_HASCHILDREN | TF_EXPANDED | TF_EXPANDABLE;
	 	pParentNode->m_nFlags &= ~TF_NOT_EXPANDABLE;
	}
	// Get text extent from a compatible DC that we know is already
	// setup with our font:
	pNode->m_nWidth =  dc.GetTextExtent (pNode->m_szText, len).cx
						+ (2 * (pNode->m_nLevels + 1)) * m_sizeM.cx +
						cxFolder + c_cyBorderX2 * 6;

	// Find the insert point with a binary search	
	int iMax = m_listBox.GetCount();
	if (iMax > 0)
	{
		int iMin = iParentNode + 1;
		iMax -= 1;
		
		//[byrond] We allow unsorted trees in Spy++ (for Windows, for instance)
		if (m_bSorted)
		{
			// Find the insert point with a binary search
			do
			{
				int iMid = (iMax + iMin) / 2;

				CNode* pMid = GetNode(iMid);
#ifdef _DEBUG
				CNode * pTemp = pMid;
				while ((pTemp = pTemp->m_pParent) != NULL)
				{	
					ASSERT(AfxIsValidAddress(pTemp, sizeof(CNode)));
				}
#endif
				if (CompareNodes(pNode, pMid) > 0)
					iMin = iMid + 1;
				else
					iMax = iMid - 1;
			}
			while (iMax > iMin);

			// can't compare to our parent? (we are at the root if iMax < 0)
			if (iMax >= 0)
			{
				CNode* pNodeT = GetNode(iMax);
				if (CompareNodes(pNode, pNodeT) > 0)
					iMax += 1;
			}
			else
			{
				iMax = 0;   // insert at front
			}
		}
		//[byrond] Handle unsorted lines quickly, using the cache
		else
		{
			if (m_pParentNodeCache == pParentNode)
			{
				iMax = m_iNextCache;
			}
			else
			{
				CNode* pNodeT;

				//
				// Look for the last item on this level
				//
				do
				{
					pNodeT = GetNode(iMin);

					if (!pNodeT || pNodeT->m_nLevels < pNode->m_nLevels)
					{
						break;
					}

					iMin++;
				}
				while (iMax >= iMin);

				iMax = iMin;
			}

			//
			// Cache this parent node and the next insert point.
			// Note that this is only done if the tree is not sorted.
			//
			m_pParentNodeCache = pParentNode;
			m_iNextCache = iMax + 1;
		}
	}
	
	// look for the first node on this level above us and turn off its
	// TFD_LASTLEVELENTRY flag so it gets a line down to this node
	
	int i = iMax - 1;
	while (i >= 0)
	{
		CNode* pNodeT = GetNode(i--);
		if (pNodeT->m_nLevels == pNode->m_nLevels)
		{
			pNodeT->m_nFlags &= ~TF_LASTLEVELENTRY;
			break;
		}
		
		if (pNodeT->m_nLevels < pNode->m_nLevels)
			break;
	}
	
	CNode* pNodeT;
	if ((pNodeT = GetNode(iMax)) == NULL ||
		pNodeT->m_nLevels < pNode->m_nLevels)
	{
		pNode->m_nFlags |= TF_LASTLEVELENTRY;
	}
	
    rpNodeRet = pNode;
	VERIFY((riNodeRet = m_listBox.InsertString(iMax, (LPSTR)pNode)) != LB_ERR);
	VERIFY((m_listBox.SetItemDataPtr(riNodeRet, (void *)pNode)) != LB_ERR);
	// [byrond] The cache for the next insert point was set above if needed...
		
	return TRUE;
}

BOOL CTreeCtl::InsertLevel(CNode* pParentNode, int nParentIndex, BOOL bExpandAll)
{
	// NOTE: This function must be overridden for a CTreeCtl to be useful,
	// this is a template of what a real one should look like.
	/*
	for (each item in pParentNode)
	{
		CNode* pNode;
		int iNode;
		if (!InsertNode(pParentNode, nParentIndex, "string", 0, pNode, iNode))
		{
			TRACE("CTreeCtl::InsertLevel: call to InsertNode failed!\n");
			return FALSE;
		}
		
		if (pNode has children && bExpandAll && !InsertLevel(pNode, iNode, TRUE))
			return FALSE;
	}
	*/

	ASSERT(FALSE);
	
	return TRUE;
}

CNode* CTreeCtl::GetNode(int nIndex)
{
	if (nIndex == -1 && (nIndex = GetCurSel()) == LB_ERR)
		return NULL;
	
	CNode* pNode = (CNode*)m_listBox.GetItemDataPtr(nIndex);
	if (pNode == (CNode*)LB_ERR)
		return NULL;
	
	return pNode;
}

BOOL CTreeCtl::NodeIsExpandable(CNode *)
{
	// NOTE: This function must be overridden for a CTreeCtl to be useful,
	ASSERT (FALSE);

	return FALSE;
}

void CTreeCtl::Expand(int nIndex, BOOL bWholeBranch)
{
	if (nIndex == -1)
	{
		nIndex = GetCurSel();
		if (nIndex == LB_ERR)	// ignore if no current selection
			return;
	}
	else if (bWholeBranch && GetCurSel() != LB_ERR && nIndex != GetCurSel())
	{
		SetCurSel(nIndex);
	}
	
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::Expand: %d invalid index!\n", nIndex);
		return;
	}

	// Update the nodes expandable/unexpandable bit.  Then see if we can
	// really expand this node:
	if (!((pNode->m_nFlags & TF_EXPANDABLE) || (pNode->m_nFlags & TF_NOT_EXPANDABLE)))
	{
		pNode->m_nFlags |= NodeIsExpandable(pNode) ? TF_EXPANDABLE : TF_NOT_EXPANDABLE;
		if (pNode->m_nFlags	& TF_NOT_EXPANDABLE)
		{
			pNode->m_nFlags &= ~TF_EXPANDED;
			return;
		}
	}
	
	if ((pNode->m_nFlags & TF_EXPANDED) && (!bWholeBranch))  // [KRS:2020]
		return;			// nothing to do unless recursive
	
	m_listBox.SetRedraw(FALSE);
	
	int iCurrentIndex = GetCurSel();
	int iNumExpanded = m_listBox.GetCount();
	int iTopIndex = m_listBox.GetTopIndex();
	CRect rc; m_listBox.GetClientRect(&rc);
	int iBottomIndex = iTopIndex + (rc.bottom + 1) / m_cyNode;
	
	// this is a really bad hack and it complicates the logic
	// for figuring out how to adjust the selection as a result
	// of doing an expand.  The problem is that this might be
	// an expand-all operation in which case we want the whole
	// tree to appear but we don't have code to do that if
	// the tree is already partly expanded so we do this collapse
	// and then expand thing... swell... [rm]
	
	if (pNode->m_nFlags & TF_EXPANDED) // [KRS:2020]
	{
		ASSERT(bWholeBranch);
		// PDC hack: collapse tree before expanding to avoid having
		// to deal with partially expanded branches
		Collapse(pNode,nIndex);
		m_listBox.SetRedraw(FALSE);
	}

	// must get the state after the possible delete above because
	// the selection and count may have changed...  I know this
	// is redundant but until the above code can be deleted
	// we can't use the variables that are already available [rm]

	int iCurSel   = m_listBox.GetCurSel();	// sel after possible collapse above
	int cItemsNew = m_listBox.GetCount();  // kinda redundant with iNumExpanded

	InsertLevel(pNode, nIndex, bWholeBranch);
	
	int iExpandInView = iBottomIndex - iCurrentIndex;
	iNumExpanded = m_listBox.GetCount() - iNumExpanded;
	
	cItemsNew = m_listBox.GetCount() - cItemsNew;

	if (iCurSel > nIndex)
	{
		// selection was after the expand point, must adjust...

		iCurSel += cItemsNew;

		m_listBox.SetCurSel(iCurSel);

		if (m_bIsMultiSelection)
		{
			m_listBox.SetCaretIndex(iCurSel);
			m_listBox.SetSel(iCurSel, TRUE);
		}

#ifdef _WIN32
	    SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetListBox()->GetSafeHwnd());
#else
	    SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	        MAKELPARAM(GetListBox()->GetSafeHwnd(), LBN_SELCHANGE));
#endif
	}
	
	if (iNumExpanded >= iExpandInView)
	{
		m_listBox.SetTopIndex(min(iCurrentIndex, 
			iTopIndex + iNumExpanded - iExpandInView + 1));
	}
	
	RecalcHorizontalExtent ();
	if (m_EnableUpdates == 0) 
	{
		m_listBox.SetRedraw (TRUE);
		if (iNumExpanded != 0)
			m_listBox.Invalidate();
	}
	
}

void CTreeCtl::ExpandAll()
{
	int nIndex = 0;
	CNode * pNode;

	HoldUpdates();

	while ((pNode = GetNode(nIndex)) != (CNode *)NULL)
	{
		// root node?
		if (pNode->m_nLevels == 0)
			Expand(nIndex, TRUE);
		nIndex++;
	}
	SetCurSel(0);

	EnableUpdates();
}

void CTreeCtl::Collapse(int nIndex)
{
	if (nIndex == -1)
		nIndex = GetCurSel();
	
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::Collapse: %d invalid index!\n", nIndex);
		return;
	}
	
	if ((pNode->m_nFlags & TF_EXPANDED) == 0)
		return;
	
	Collapse(pNode, nIndex);
	RecalcHorizontalExtent ();
}

void CTreeCtl::RefreshAllRoots()
{
	if (m_EnableUpdates > 0) return;

	int nIndex = 0;
	CNode * pNode;
	while ((pNode = GetNode(nIndex)) != (CNode *)NULL)
	{
		// root node?
		if (pNode->m_nLevels == 0)
			Refresh(pNode, nIndex);
		nIndex++;
	}
}

void CTreeCtl::Refresh(int nIndex)
{
	if (m_EnableUpdates > 0) return;

	if (nIndex == -1)
		nIndex = GetCurSel();
	
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::Refresh: %d invalid index!\n", nIndex);
		return;
	}
	
	Refresh(pNode, nIndex);
}

BOOL CTreeCtl::IsExpanded(int nIndex)
{
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::IsExpanded: %d invalid index!\n", nIndex);
		return FALSE;                             
	}
	
	return (pNode->m_nFlags & TF_EXPANDED) != 0;
}

BOOL CTreeCtl::HasChildren(int nIndex /*= -1*/ )
{
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::HasChildren: %d invalid index!\n", nIndex);
		return FALSE;                             
	}
	
	return (pNode->m_nFlags & TF_HASCHILDREN) != 0;
}

DWORD CTreeCtl::GetData(int nIndex)
{
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
		return NULL;
	return pNode->m_dwData;
}

void CTreeCtl::SetData(int nIndex, DWORD dwData)
{
	CNode* pNode = GetNode(nIndex);
	if (pNode == NULL)
	{
		TRACE("CTreeCtl::SetData %d invalid index\n", nIndex);
		return;
	}
	
	pNode->m_dwData = dwData;
}


CTreeCtl::HIT_ZONE CTreeCtl::HitTest(int nItem, int x)
{
	CNode* pNode = GetNode(nItem);
	ASSERT(pNode != NULL);
	
	CRect client;
	GetClientRect(client);
	
	if (m_cxExtent > client.right)
		x += m_listBox.GetScrollPos(SB_HORZ) * (m_cxExtent - client.right) / 100;
	
	int xLeft = c_cyBorderX2 + pNode->m_nLevels * m_sizeM.cx * 2;
	if ((pNode->m_nFlags & TF_EXPANDABLE) != 0)
	{
		if (x < xLeft)
			return left;
		if (x < xLeft + 2 * m_sizeM.cx)
			return expand;
	}
	
	xLeft += 2 * m_sizeM.cx;
	if (x < xLeft)
		return left;
	
	xLeft += cxFolder + c_cyBorderX2;
	if (x < xLeft)
		return glyph;
	
	CClientDC dc(this);
	dc.SelectObject(pNode->GetFont());

	// Extra area between primary glyph and text is in the text zone...
	xLeft += pNode->GetGlyphWidth() - cxFolder;
	
	xLeft += dc.GetTextExtent(pNode->m_szText, _tcslen(pNode->m_szText)).cx +
		4 * c_cyBorderX2;
	if (x < xLeft)
		return text;
	
	return right;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListBox -- the actual list box inside a tree control

int CTreeListBox::iSlobPixels = 5;
CPoint CTreeListBox::mouseDownPt;

CTreeListBox::CTreeListBox()
{
	m_pTree = NULL;
    m_bGotMouseDown = FALSE;
    m_bDeferedDeselection = FALSE;
	m_bStdMouse = TRUE;
}

int CTreeListBox::ItemFromPoint(const CPoint& point)
{
    CRect client;
    GetClientRect(&client);

    if (!client.PtInRect(point))
        return LB_ERR;

    CRect itemRect;
    GetItemRect(0, &itemRect);

	int nItem = (GetTopIndex() + (point.y / itemRect.Height()));
	return nItem < GetCount() ? nItem : LB_ERR;
}

void CTreeListBox::OnLButtonDown(UINT nFlags, CPoint pt)
{
	BOOL bPassOn = TRUE;

	SetFocus();
	SetCapture();

	m_bStdMouse = TRUE;
	
	int nItem = ItemFromPoint(pt);
	if (nItem != LB_ERR)
	{
		int nCurSel = GetCurSel();
		int nCount = GetCount();
	//	SetCurSel(nItem);
		CTreeCtl::HIT_ZONE hitZone = m_pTree->HitTest(nItem, pt.x);
		
		if (hitZone == CTreeCtl::expand)
		{
			// Inside the expand/collapse box...
		
			m_pTree->OnOpen(nItem);

			if (nItem < nCurSel)
			{
				if (GetCount() > nCount)
				{
					// did expand, change selection accordingly
					SetCurSel(nCurSel + GetCount() - nCount);
				}
				else if (GetCount() < nCount)
				{
					// did collapse, change selection accordingly
					if (nCurSel > (nItem + (nCount - GetCount())))
					{
						// previous selection not in collapsed area, reset selection
						SetCurSel(nCurSel + GetCount() - nCount);
					}
					else
					{
						// previous selection in collapsed area, set selection to collapsed node
						SetCurSel(nItem);
					}
				}
			}

			// don't change selection unless we just cleared in
			// by collapsing the tree...
			if (GetSelCount() != 0)
				return;
		}
	
		if (m_pTree->m_bEnableDragDrop && 
			(hitZone == CTreeCtl::glyph || hitZone == CTreeCtl::text))
		{
			// Inside glyph or text and D&D is enabled...
		
			m_bStdMouse = FALSE;
		    m_bGotMouseDown = TRUE;
		    mouseDownPt = pt;

		    if ((nFlags & MK_SHIFT) == 0 && GetSel(nItem))
			{
		        // don't deselect yet, wait for either a mouse up or a drag and drop
		        m_bDeferedDeselection = TRUE;
		        m_nDeferedItem = nItem;
				return;
			}
			else
			{
				if ((nFlags & MK_SHIFT) != 0)
				{
					// Shift held down - so select a range, but do not pass on.
					int nCurrentSel;

					SetSel(-1, FALSE);
					nCurrentSel = GetCaretIndex();
					if (nCurrentSel > nItem)
						SelItemRange(TRUE, nItem, nCurrentSel);
					else
						SelItemRange(TRUE, nCurrentSel, nItem);
 				}
				else
				{
 					// Only clear selection if CONTROL key not pressed
					if ((nFlags & MK_CONTROL) == 0)
						SetSel(-1, FALSE);
			    	SetSel(nItem, TRUE);
				}
				bPassOn = FALSE;
			}
		}
	}

	if (bPassOn)
		CListBox::OnLButtonDown(nFlags, pt);	// pass on

	if (m_pTree->m_bEnableDragDrop)
	{
#ifdef _WIN32
	    GetParent()->SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
	    GetParent()->SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	        MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
	}
}

void CTreeListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nItem = ItemFromPoint(point);
	if (nItem != LB_ERR)
	{
		switch (m_pTree->HitTest(nItem, point.x))
		{
		case CTreeCtl::expand:
			// Treat as single click...
			m_pTree->OnOpen(nItem);
			return;
			
		case CTreeCtl::left:
		case CTreeCtl::right:
			// Ignore the double click...
			return;
		
		case CTreeCtl::glyph:
		case CTreeCtl::text:
			break;
		}
	}

	CListBox::OnLButtonDblClk(nFlags, point);
}

void CTreeListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bStdMouse || !m_pTree->m_bEnableDragDrop)
	{
		CListBox::OnMouseMove(nFlags, point);
		return;
	}

    // Nasty hack here because Windows sends a continuous stream of
    // WM_MOUSEMOVE messages when a button is held down...
    static CPoint NEAR lastPoint;
    if ((point == lastPoint) || (GetCount() == 0))
        return;

    lastPoint = point;

    CPoint offsetPoint (abs(mouseDownPt.x - point.x),
                        abs(mouseDownPt.y - point.y));

    if (m_bGotMouseDown && offsetPoint.x + offsetPoint.y > iSlobPixels && GetSelCount() > 0)
    {
		// Begin a drag...
		m_pTree->BeginDragDrop();

        m_bGotMouseDown = FALSE;
        m_bDeferedDeselection = FALSE;
    }
}

void CTreeListBox::OnLButtonUp(UINT nFlags, CPoint pt)
{
	if (!m_bStdMouse && m_pTree->m_bEnableDragDrop)
	{
		m_bGotMouseDown = FALSE;

		if (m_bDeferedDeselection)
		{
		    if (nFlags & MK_CONTROL)
		    {
		        SetSel(m_nDeferedItem, FALSE);
		    }
		    else
		    {
		        SetSel(-1, FALSE);
		        SetSel(m_nDeferedItem, TRUE);
		    }
		    m_bDeferedDeselection = FALSE;
#ifdef _WIN32
	        GetParent()->SendMessage(WM_COMMAND, 
				MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
	        GetParent()->SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	            MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
		}
    }

    // need to call Windows so notifications get sent
    CListBox::OnLButtonUp(nFlags, pt);
	ReleaseCapture();
}

void CTreeListBox::OnRButtonUp(UINT nFlags, CPoint pt)
{
	CListBox::OnRButtonUp(nFlags, pt);	// pass on

	CWnd *	pwnd = GetParent();

	ASSERT(pwnd->IsKindOf(RUNTIME_CLASS(CTreeCtl)));

	// inform the tree control of this
	((CTreeCtl *)pwnd)->OnRightButtonUp(nFlags, pt);
}

void CTreeListBox::OnRButtonDown(UINT nFlags, CPoint pt)
{
	//[byrond] Don't update this routine because we don't have CSlobWnds,
	// and our handler for OnRightButtonDown will handle it!
	CListBox::OnRButtonDown(nFlags, pt);	// pass on

	CWnd *  pwnd = GetParent();

	ASSERT(pwnd->IsKindOf(RUNTIME_CLASS(CTreeCtl)));

	// inform the tree control of this
	((CTreeCtl *)pwnd)->OnRightButtonDown(nFlags, pt);
}

void CTreeListBox::OnKeyDown(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (m_pTree->OnKeyDown(nChar))
		return;

	CListBox::OnKeyDown(nChar, nRepCount, nFlags);
}

void CTreeListBox::OnChar(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (m_pTree->OnChar(nChar))
		return;

	CListBox::OnChar(nChar, nRepCount, nFlags);
}

void CTreeCtl::OnRightButtonUp(UINT, CPoint)
{
	// do nothing .. derived class implementation is more useful
}

void CTreeCtl::OnRightButtonDown(UINT, CPoint)
{
	// do nothing .. derived class implementation is more useful
}

void CTreeCtl::OnLbDblClk()
{	
	// CHICAGO: Move property activation to rbutton.
	//if (m_bShowProperties && (GetKeyState(VK_MENU) & ~1) != 0)
	//{
	//	ActivatePropertyBrowser();
	//	return;
	//}

	OnPrimary();
}

void CTreeCtl::OnLbSelChange()
{
	OnSelect(GetCurSel());
}

void CTreeCtl::OnOpen(int nIndex)
{
	CNode* pNode = GetNode(nIndex);
	if (pNode != NULL)
	{
		if (IsExpanded(nIndex))
		{
			BeginWaitCursor();
			Collapse(nIndex);
			EndWaitCursor();
		}
		else if (pNode->m_nFlags & TF_EXPANDABLE)
		{
			BeginWaitCursor();
			Expand(nIndex);
			EndWaitCursor();
		}
	}
}

void CTreeCtl::OnSelect(int nIndex)
{
}

#if 0 //[byrond] Unused code follows:
struct TreeData
{
	WORD	wSize;
	int 	nCurSel;
	int		nLastSel;
	int		nTopIndex;
	int		nExpandedNodes[1];
};

BYTE* CTreeCtl::GetInitData()
{
	// Create a block of intit data for workspace saving.  This should enable the
	// reconstruct its state in ReadInitData.

	int nIndex = m_listBox.GetCount(), nExpanded = 0, i, nDataSize;
	int *pint = new int[nIndex];
	TreeData *pdata;
	ASSERT (nIndex);
	for (i=0; i < nIndex; i++)
	{
		CNode * pNode = (CNode *) m_listBox.GetItemDataPtr(i);
		if (pNode->m_nFlags & TF_EXPANDED) pint[nExpanded++] = i;
	}

	nDataSize = sizeof (TreeData) + max (nExpanded -1,0) * sizeof (int);

	pdata = (TreeData *) new BYTE[ nDataSize ];
	pdata->wSize = nDataSize - sizeof ( WORD );

	pdata->nTopIndex = m_listBox.GetTopIndex ();

	if (nExpanded > 0)
		memcpy ( pdata->nExpandedNodes, pint, nExpanded * sizeof (int));
	else pdata->nExpandedNodes[0] = -1;

	if (m_bIsMultiSelection)
	{
		pdata->nLastSel = m_listBox.GetSelItems (m_listBox.GetCount(), pint);
		pdata->nCurSel = *pint;
		pdata->nLastSel += *pint;
	}
	else pdata->nCurSel = m_listBox.GetCurSel ();

	delete [] (pint);
	return (BYTE *) pdata;
}						 	

void CTreeCtl::ReadInitData (CPartFrame *pFrame)
{
	int i,j, nItems, nItemsBox;
	CNode *pNode;
	ASSERT (pFrame->IsKindOf ( RUNTIME_CLASS ( CPartFrame )));
	TreeData  *pdata = (TreeData  *) pFrame->GetCurData ();

	if (pdata == NULL) return;

	RefreshAllRoots();

	if (pdata->nExpandedNodes[0] != -1)
	{
		nItems = (pdata->wSize + sizeof (WORD) - sizeof ( TreeData ))
					 / sizeof (int) + 1;
		nItemsBox = m_listBox.GetCount();
		Expand (0);
		i=0; j=0;
		while ( i < nItems && j < nItemsBox)
		{
			while (j < pdata->nExpandedNodes[i] )
			{
				if ((pNode = (CNode *) m_listBox.GetItemDataPtr(j))
									== (CNode *) -1 )
				{
					TRACE ("Tree control state read failed.\n");
					return;
				}
				if (pNode->m_nFlags & TF_EXPANDED)
				{
					Collapse (j);
					nItemsBox = m_listBox.GetCount();
				}
				j++;
			}
			if ((pNode = (CNode *) m_listBox.GetItemDataPtr(j))
									== (CNode *) -1 )
			{
				TRACE ("Tree control state read failed.\n");
				return;
			}
			if ( !(pNode->m_nFlags & TF_EXPANDED) ) 
			{
				Expand (j);
				nItemsBox = m_listBox.GetCount();
			}
			j++;
			i++;	
		}
		// Collaps any remaining nodes: 
		while (j < nItemsBox) 
		{
			VERIFY ((pNode =  (CNode *) m_listBox.GetItemDataPtr(j))
									!= (CNode *) -1 );
			if (pNode->m_nFlags & TF_EXPANDED)
			{
				Collapse (j);
				nItemsBox = m_listBox.GetCount();
			}
			j++;
		}

	}
	else Collapse (0);

	if (m_bIsMultiSelection)
	{
		for (i = pdata->nCurSel; i < pdata->nLastSel; i++)
			m_listBox.SetSel (i, TRUE);
	}
	else m_listBox.SetCurSel (pdata->nCurSel);

	m_listBox.SetTopIndex (pdata->nTopIndex);	
}

void CTreeCtl::SizeViewToTree ( CPartFrame *pFrame )
{
	CRect rcView, rcListBox, rcParent;
	int dl, curWidth, cxm, cxe;
	BOOL bIsMax;

	RefreshAllRoots();

	// If the MDI child is maximized, then don't do anything:
	pFrame->GetMDIFrame()->MDIGetActive ( &bIsMax );
	if (bIsMax) return;

	m_listBox.GetWindowRect (rcListBox);	 
	pFrame->GetWindowRect   (rcView);
	curWidth = rcView.Width (); 
	cxe = m_cxExtent + GetSystemMetrics (SM_CXHSCROLL) * 2;
	cxm = m_sizeM.cx << 5;
	dl = curWidth - rcListBox.Width () + max (cxe, cxm);
	// If we're make the window bigger, don't make it too big:
	if (dl > rcView.Width () )
	{
		pFrame->GetParent()->GetWindowRect   (rcParent);
		cxe = rcParent.Width() * 2 / 3;
		cxm = max (curWidth, cxe); 
		dl = min (dl, cxm);
	}
	pFrame->SetWindowPos (
						NULL, 
						0,
						0,
						dl,
						rcView.Height (),
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE	
						);

	if (m_bIsMultiSelection) m_listBox.SetSel (0, TRUE);
	else m_listBox.SetCurSel (0);
	m_listBox.SetTopIndex (0);	
}
#endif //[byrond] ...end of the unused code

void  CTreeCtl::HoldUpdates ()
{
	if (m_EnableUpdates == 0) m_listBox.SetRedraw (FALSE);
	m_EnableUpdates++;
}
void  CTreeCtl::EnableUpdates ()
{
	m_EnableUpdates--;
	if (m_EnableUpdates == 0)
	{
		m_listBox.SetRedraw (TRUE);
		m_listBox.Invalidate();
	}

	ASSERT (m_EnableUpdates >= 0);
}

void CTreeCtl::GetNodeTitle(CNode* pNode, CString& strTitle)
{
	strTitle = pNode->m_szText;
}

CNode* CTreeCtl::CreateNode(DWORD dwData)
{
	return new CNode(dwData);
}

void CTreeCtl::DeleteNode(CNode* pNode)
{
	delete pNode;
}

void CTreeCtl::BeginDragDrop()
{
	TRACE("Must override CTreeCtl::BeginDragDrop for drag/drop functionality!\n");
}

CNode::CNode(DWORD dwData)
{
	m_szText = NULL;
	m_dwData = dwData;
}

CNode::~CNode()
{
	delete [] m_szText;
}

CFont* CNode::GetFont() const
{
	return GetStdFont(font_Normal);
}

int CNode::GetGlyphWidth()
{
	return cxFolder;
}
