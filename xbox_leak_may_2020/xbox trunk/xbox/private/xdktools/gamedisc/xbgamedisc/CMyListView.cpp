// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CMyListView.cpp
// Contents:  
// Revisions: 6-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"
#include <zmouse.h>

bool gs_fInitedListViewClasses = false;
CMyListView *g_pmlvTemp = NULL;

#define LVF_FOCUSITEM    4 // internal use only

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LRESULT CALLBACK MyListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wparam,
                                   LPARAM lparam)
{
    // Thunk over to the actual ListView object.
    CMyListView *pmlv = (CMyListView*)GetWindowLong(hwnd, GWL_USERDATA);
    if (!pmlv)
    {
        assert(g_pmlvTemp);
        pmlv = g_pmlvTemp;
        pmlv->m_hwnd = hwnd;
        g_pmlvTemp = NULL;
        SetWindowLong(hwnd, GWL_USERDATA, (long)pmlv);
    }
    return pmlv->WndProc(uMsg, wparam, lparam);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CMyListView::Init(HWND hwndParent)
{
    WNDCLASSEX wcex;

    if (!gs_fInitedListViewClasses)
    {
        // Register the class for the app's window.
        memset(&wcex, 0, sizeof wcex);
        wcex.cbSize         = sizeof(WNDCLASSEX); 
        wcex.style          = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
        wcex.lpfnWndProc    = (WNDPROC)MyListViewWndProc;
        wcex.hInstance      = g_hInst;
        wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszClassName  = "MyListView";
        if (!RegisterClassEx(&wcex))
            return FALSE;
        
        gs_fInitedListViewClasses = true;
    }

    m_iFocusItem = -1;

    // Create the GDI font object
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -11;
    lf.lfWeight = 0;
    strcpy(lf.lfFaceName, "Tahoma");
    m_hfont = CreateFontIndirect(&lf);

    m_nCurHScrollLoc = 0;
    m_nMaxHScroll = 0;
    m_nCurVScrollLoc = 0;
    m_nMaxVScroll = 0;

    m_fHaveFocus = false;
    m_iShiftItem = -1;

    m_fInvertSort = false;
    m_cItems = 0;
    m_nItemHeight = 17;

    m_cMaxItems = 0;
    m_cAddItemDelta = 100;
    m_rgli = NULL;

    m_fDirty = FALSE;

    // Warning: this is not reentrant!
    assert(g_pmlvTemp == NULL);
    g_pmlvTemp = this;
    CreateWindow("MyListView", "", WS_CHILD | WS_HSCROLL | WS_VSCROLL |
                 WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 0, 0,
                 hwndParent, (HMENU)WNDID_LISTVIEW, g_hInst, NULL);
    if (m_hwnd == NULL)
        return FALSE;

    // Add our column (header) control
    if (!InitHeader())
        return FALSE;

    SendMessage(m_hwndHeader, WM_SETFONT, (WPARAM)m_hfont, FALSE);

    // Initialize the scroll bars
    ShowScrollBar(m_hwnd, SB_VERT, false);
    ShowScrollBar(m_hwnd, SB_HORZ, false);

    m_iSortColumn = -1;

    return TRUE;
}

bool m_fMouseDown = false;
LRESULT CMyListView::WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    LPNMHEADER pnmhdr;
    LPHDITEM   phdi;
    SCROLLINFO si;
    int        nDelta;
    
//    TrackMessage(uMsg, wparam, lparam, "ListView");

    switch(uMsg)
    {
    case WM_PAINT:
        UpdateListDisplay();
        break;

    case WM_ERASEBKGND:
        return 1;
        
    case WM_NOTIFY:
        pnmhdr = (LPNMHEADER)lparam;
        switch(pnmhdr->hdr.code)
        {
        case HDN_ITEMCHANGED:
            // User is moving (resizing) a divider.  Update the columns.
            phdi = pnmhdr->pitem;
            m_rgnColWidths[pnmhdr->iItem] = phdi->cxy;
            UpdateColumnWidths();
            UpdateScrollBars();
            InvalidateRect(m_hwnd, NULL, TRUE);
            // Update the position of the header control in case it's been scrolled.
            // Note that this creates slightly strange behaviour, but the behaviour
            // exactly mimics what WinXP does, so, when in rome...
            // (example: shrink window so that horz scroll bar appears; scroll
            //  window all the way to the right; shrink one of the column headers;
            //  RESULT: The whole column quickly shrinks).
            SetWindowPos(m_hwndHeader, NULL, -m_nCurHScrollLoc, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            break;

        case HDN_ITEMCLICK:
            // User clicked a column header. need to sort by that column
            SortColumn(pnmhdr->iItem);
            break;
            
        case HDN_DIVIDERDBLCLICK:
            // User double clicked a header divider.  Resize the column to
            // the maximum length of the contents of that column in any row.
            HandleHeaderDoubleClick(pnmhdr);
            break;
        }
        break;

    case IDM_EDIT_SELECTALL:
        SelectAllItems();
        break;

    case WM_DRAWITEM:
        if (wparam == WNDID_LISTVIEW_HEADER)
        {
            DrawHeader((LPDRAWITEMSTRUCT)lparam);
            return TRUE;
        }
        break;

    case WM_HSCROLL: 
        si.cbSize = sizeof(si); 
        si.fMask  = SIF_ALL; 
        GetScrollInfo(m_hwnd, SB_HORZ, &si);

        switch (LOWORD(wparam)) 
        { 
        case SB_PAGEUP:     nDelta = -100; break; 
        case SB_PAGEDOWN:   nDelta = 100;  break; 
        case SB_LINEUP:     nDelta = -5;   break; 
        case SB_LINEDOWN:   nDelta = 5;    break; 
        case SB_THUMBTRACK: nDelta = HIWORD(wparam) - si.nPos; break;
        default:            nDelta = 0;    break;
        }

        nDelta = max(-si.nPos, min(nDelta, (int)(si.nMax -
                                                 (si.nPos + si.nPage))));
        if (nDelta)
        {
            si.nPos += nDelta;
            m_nCurHScrollLoc = si.nPos;
            SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
            ScrollWindowEx(m_hwnd, -nDelta, 0, NULL, NULL,
                           NULL, NULL, 0);
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            MoveWindow(m_hwndHeader, -m_nCurHScrollLoc, 0, rc.right - rc.left +
                       m_nCurHScrollLoc, m_nHeaderHeight, TRUE);

            InvalidateRect(m_hwnd, NULL, TRUE);
            UpdateWindow(m_hwndHeader);
        }
        break;

    case WM_LBUTTONDOWN: HandleButtonDown(LOWORD(lparam), HIWORD(lparam), wparam); break;
    case WM_LBUTTONDBLCLK: HandleButtonDblClick(LOWORD(lparam), HIWORD(lparam), wparam); break;
    case WM_LBUTTONUP:   HandleButtonUp(LOWORD(lparam), HIWORD(lparam), wparam);   break;
    case WM_MOUSEMOVE:   HandleMouseMove(LOWORD(lparam), HIWORD(lparam), wparam);  break;

    case WM_SETFOCUS:
         g_hwndPrevFocus = m_hwnd;
         m_fHaveFocus = true;
         m_iShiftItem = m_iFocusItem;
         UpdateAllItems();
         break;

    case WM_KILLFOCUS:
         m_fHaveFocus = false;
         UpdateAllItems();
         break;

    case WM_KEYDOWN:
        si.cbSize = sizeof(si); 
        si.fMask  = SIF_ALL; 
        GetScrollInfo(m_hwnd, SB_VERT, &si);

        switch (wparam)
        {
        case VK_SHIFT:
            // only update if shift wasn't previously pressed (ie ignore autorepeat)
            if ((lparam & 0x40000000) == 0)
                m_iShiftItem = m_iFocusItem;
            break;

        default:
            HandleKeyDown(wparam);
            break;
        }
        break;

    case WM_MOUSEWHEEL:
    case WM_VSCROLL: 
        si.cbSize = sizeof(si); 
        si.fMask  = SIF_ALL; 
        GetScrollInfo(m_hwnd, SB_VERT, &si);

        if (uMsg == WM_MOUSEWHEEL)
        {
            if ((short)HIWORD(wparam) > 0)
                nDelta = -m_nItemHeight;
            else
                nDelta = m_nItemHeight;
        }
        else
        {
            switch (LOWORD(wparam)) 
            { 
            case SB_PAGEUP:     nDelta = -((int)si.nPage - m_nItemHeight); break; 
            case SB_PAGEDOWN:   nDelta = ((int)si.nPage - m_nItemHeight);  break; 
            case SB_LINEUP:     nDelta = -m_nItemHeight;          break; 
            case SB_LINEDOWN:   nDelta = m_nItemHeight;           break; 
            case SB_THUMBTRACK: nDelta = si.nTrackPos - si.nPos;  break;
            default:            nDelta = 0;                       break;
            }
        }

        nDelta = max(-si.nPos, min(nDelta, (int)(si.nMax -
                                                 (si.nPos + si.nPage))));
        nDelta = (nDelta / m_nItemHeight) * m_nItemHeight;
        if (nDelta)
        {
            si.nPos += nDelta;
            si.nPos = (si.nPos / m_nItemHeight) * m_nItemHeight;
            m_nCurVScrollLoc = si.nPos;
            SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
            ScrollWindowEx(m_hwnd, 0, -nDelta, NULL, NULL,
                           NULL, NULL, 0);
            RECT rc;
            GetClientRect(m_hwnd, &rc);

            if (nDelta < 0)
                rc.bottom = rc.top - nDelta + m_nHeaderHeight;
            else
                rc.top = rc.bottom - nDelta;

            InvalidateRect(m_hwnd, &rc, TRUE);
            UpdateWindow(m_hwnd);
        }
        break;
    }

    return DefWindowProc(m_hwnd, uMsg, wparam, lparam);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SortObjects
// Purpose:   Comparison function for qsort.
// Arguments: pvElem1       -- The first element to compare.
//            pvElem2       -- The second element to compare.
// Return:    <0 if (elem1<elem2), 0 if (elem1==elem2), >0 if (elem1>elem2)
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CMyListView *g_plvTemp = NULL;
int _cdecl SortObjects(const void *pvElem1, const void *pvElem2)
{
    // pvElem1 and pvElem2 are passed in as pointers to array elements.  Do
    // some wacky casting to get the actual object pointers.
    void *pv1 = (void*)*(DWORD*)pvElem1;
    void *pv2 = (void*)*(DWORD*)pvElem2;
    int nRet = g_plvTemp->CompareObjects(pv1, pv2);
    if (g_plvTemp->m_fInvertSort)
        nRet *= -1;
    return nRet;
}

DWORD GetKeyFlags()
{
    DWORD dwKeys = 0;

    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        dwKeys |= MK_SHIFT;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        dwKeys |= MK_CONTROL;

    return dwKeys;
}

int CMyListView::GetTopItem()
{
    SCROLLINFO si;
    si.cbSize = sizeof(si); 
    si.fMask  = SIF_ALL; 
    GetScrollInfo(m_hwnd, SB_VERT, &si);

    return si.nPos / m_nItemHeight;
}

int CMyListView::GetBottomItem()
{
    SCROLLINFO si;
    si.cbSize = sizeof(si); 
    si.fMask  = SIF_ALL; 
    GetScrollInfo(m_hwnd, SB_VERT, &si);

    return (si.nPos + si.nPage-m_nItemHeight)/ m_nItemHeight-1;
}
void CMyListView::SetFocusItem(int iFocusItem)
{
    if (m_iFocusItem != -1)
        m_rgli[m_iFocusItem].dwFlags &= ~LVF_FOCUSITEM;
    m_iFocusItem = iFocusItem;
    if (m_iFocusItem != -1)
        m_rgli[m_iFocusItem].dwFlags |= LVF_FOCUSITEM;
}

void CMyListView::HandleKeyDown(DWORD dwVKey)
{
    int iTopItem, iBottomItem;
    SCROLLINFO si;
    si.cbSize = sizeof(si); 
    si.fMask  = SIF_ALL; 
    if (m_cItems == 0)
        return;
    
    int iOrigFocusItem = m_iFocusItem;
    switch(dwVKey)
    {
    case VK_UP:
        iTopItem = GetTopItem();
        if (m_iFocusItem == iTopItem)
        {
            if (m_iFocusItem == 0)
                break;

            SendMessage(m_hwnd, WM_VSCROLL, SB_LINEUP, 0L);
            iTopItem = GetTopItem();
        }
        SetFocusItem(m_iFocusItem-1);

        break;
        
    case VK_DOWN:
        iBottomItem = GetBottomItem();
        if (m_iFocusItem == iBottomItem)
        {
            if (m_iFocusItem == m_cItems - 1)
                break;

            SendMessage(m_hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
            iBottomItem = GetBottomItem();
        }
        SetFocusItem(m_iFocusItem+1);
        break;

    case VK_PRIOR:
        iTopItem = GetTopItem();
        if (m_iFocusItem == iTopItem)
        {
            if (m_iFocusItem == 0)
                break;

            SendMessage(m_hwnd, WM_VSCROLL, SB_PAGEUP, 0L);
            iTopItem = GetTopItem();
        }
        SetFocusItem(iTopItem);
        break;
    
    case VK_NEXT:
        iBottomItem = GetBottomItem();
        if (m_iFocusItem == iBottomItem)
        {
            if (m_iFocusItem == m_cItems - 1)
                break;

            SendMessage(m_hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
            iBottomItem = GetBottomItem();
        }
        SetFocusItem(iBottomItem);
        break;
        
    case VK_HOME:
        GetScrollInfo(m_hwnd, SB_VERT, &si);
        si.nPos = 0;
        m_nCurVScrollLoc = si.nPos;
        SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
        InvalidateRect(m_hwnd, NULL, FALSE);
        SetFocusItem(0);
        break;

    case VK_END:
        GetScrollInfo(m_hwnd, SB_VERT, &si);
        si.nPos = ((si.nMax - si.nPage) / m_nItemHeight) * m_nItemHeight;
        m_nCurVScrollLoc = max(0,si.nPos);
        SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
        InvalidateRect(m_hwnd, NULL, FALSE);
        SetFocusItem(m_cItems-1);
        break;

    default:
        return;
    }
    if (m_iFocusItem != iOrigFocusItem && iOrigFocusItem != -1)
        RefreshItem(iOrigFocusItem);
    if (GetKeyFlags() == MK_CONTROL)
        RefreshItem(m_iFocusItem);
    else
        SelectItem(m_iFocusItem, GetKeyFlags());
}


// Return true if we drew it.
BOOL CMyListView::DrawHeader(LPDRAWITEMSTRUCT pdis)
{
    HDC hdcHeader = pdis->hDC; 
    RECT rc = pdis->rcItem; 
    int iColumn = pdis->itemID; 
    int nState = pdis->itemState; 

//#define OFFSCREEN
#ifdef OFFSCREEN
    static HBITMAP s_hbmpOff;
    static int s_nLastWidth = -1, s_nLastHeight = -1;
    RECT rcOrig = rc;
    rc.bottom -= rc.top;
    rc.top = 0;
    rc.right -= rc.left;
    rc.left = 0;
    
    HDC hdcOffscreen = CreateCompatibleDC(hdcHeader);
    if (rc.right - rc.left > s_nLastWidth  || rc.bottom - rc.top > s_nLastHeight)
    {
        s_hbmpOff = CreateCompatibleBitmap(hdcHeader, rc.right - rc.left,
                                           rc.bottom - rc.top);
        s_nLastWidth = rc.right - rc.left;
        s_nLastHeight = rc.bottom - rc.top;
        DebugOutput("create header bmp (%d, %d)\n", s_nLastWidth, s_nLastHeight);
    }
    HBITMAP hbmpPrevOff = (HBITMAP)SelectObject(hdcOffscreen, s_hbmpOff);
#else
    HDC hdcOffscreen = pdis->hDC;
#endif

    // Draw the sort icon if we're sorting on this column
    SetTextColor(hdcOffscreen, RGB(0, 0, 0)); 
    SetBkMode(hdcOffscreen, TRANSPARENT); 
    HFONT hfontPrev = (HFONT)SelectObject(hdcOffscreen, m_hfont);

    FillRect(hdcOffscreen, &rc, GetSysColorBrush(COLOR_BTNFACE));

    rc.left += 7;
    rc.top  += 3;
    if (nState & ODS_SELECTED) 
    {
        rc.left++;
        rc.top++;
    }

    HDITEM hdi;
    char sz[256];
    hdi.mask = HDI_TEXT;
    hdi.pszText = sz;
    hdi.cchTextMax = 256;
    Header_GetItem(m_hwndHeader, iColumn, &hdi);
    DrawText(hdcOffscreen, sz, strlen(sz), &rc, DT_LEFT); 

    if (iColumn == m_iSortColumn) 
    {
        SIZE size;
        GetTextExtentPoint32(hdcOffscreen, sz, strlen(sz), &size);
        SetTextJustification(hdcOffscreen, 0, 0);
    
        int nX = min(rc.left + size.cx + 2, rc.right - 16);
        ImageList_Draw(m_himlSortArrows, m_fInvertSort ? 0 : 1,
                       hdcOffscreen, nX, rc.top-1, ILD_TRANSPARENT | ILD_NORMAL);
    }

    SelectObject(hdcOffscreen, hfontPrev);
#ifdef OFFSCREEN
    BitBlt(hdcHeader, rcOrig.left, rcOrig.top,
           rcOrig.right - rcOrig.left, rcOrig.bottom - rcOrig.top,
           hdcOffscreen, 0, 0, SRCCOPY);
    SelectObject(hdcOffscreen, hbmpPrevOff);
    DeleteDC(hdcOffscreen);
#endif
    return TRUE;
}

void CMyListView::SortColumn(int iColumn)
{
    m_fInvertSort = (m_iSortColumn == iColumn) & !m_fInvertSort;

    RECT rc;
    rc.top = 0;
    rc.bottom =20;
    rc.left = 0;
    for (int i = 0; i < m_iSortColumn; i++)
        rc.left += m_rgnColWidths[i];
    rc.left += 5;
    rc.right = rc.left + m_rgnColWidths[i] - 5;
    InvalidateRect(m_hwndHeader, &rc, TRUE);
    
    m_iSortColumn = iColumn;
    // warning: not reentrant
    assert(g_plvTemp == NULL);
    g_plvTemp = this;
    qsort((void*)m_rgli, m_cItems, sizeof(sListItem), &SortObjects);
    g_plvTemp = NULL;

    // Update m_iFocusItem.  Find the item with 'LVF_FOCUSITEM' and set m_iFocusItem to it.
    for (int i2 = 0; i2 < m_cItems; i2++)
        if (m_rgli[i2].dwFlags & LVF_FOCUSITEM)
        {
            SetFocusItem(i2);
            break;
        }

    UpdateAllItems();
}

void CMyListView::GetMouseInfoAt(int nX, int nY, sMouseInfo *pmi)
{
    int iRow, iColumn;
        
    pmi->nX = nX;
    pmi->nY = nY - m_nHeaderHeight+1;
    
    if (GetItemAtPoint(nX, nY, &iRow, &iColumn))
    {
        pmi->iRow = iRow;
        pmi->iColumn = iColumn;
        pmi->pvItem = m_rgli[iRow].pvData;
    
        pmi->rcItem.left  = -m_nCurHScrollLoc;
        pmi->rcItem.right = pmi->rcItem.left + m_nHeaderHeight;
        pmi->rcItem.top = (iRow - GetTopItem()) * m_nItemHeight;
        pmi->rcItem.bottom = pmi->rcItem.top + m_nItemHeight;

        pmi->rcSubItem = pmi->rcItem;
        int nCols = Header_GetItemCount(m_hwndHeader);
        for (int c = 0; c < iColumn; c++)
            pmi->rcSubItem.left += m_rgnColWidths[c];
        pmi->rcSubItem.right = pmi->rcSubItem.left + m_rgnColWidths[c];
    }
    else
    {
        pmi->iRow = -1;
        pmi->iColumn = -1;
        pmi->pvItem = NULL;
    }
}

BOOL CMyListView::GetItemAtPoint(int nX, int nY, int *piItem, int *piColumn)
{
    if (nX > GetHeaderWidth() + m_nCurHScrollLoc)
        return FALSE;
    
    *piItem = (m_nCurVScrollLoc + nY - m_nHeaderHeight)/ m_nItemHeight;

    if (*piItem >= m_cItems)
        return FALSE;

    if (piColumn == NULL)
        return TRUE;
    int nC = -m_nCurHScrollLoc;
    int nCols = Header_GetItemCount(m_hwndHeader);
    for (int c = 0; c < nCols; c++)
    {
        if (nX < nC + m_rgnColWidths[c])
        {
            *piColumn = c;
            break;
        }
        nC += m_rgnColWidths[c];
    }

    if (c == nCols)
        return FALSE;
    return TRUE;
}

void CMyListView::UnselectAllItems()
{
    g_statusbar.DeferUpdate();
    SetFocusItem(-1);
    for (int i = 0; i < m_cItems; i++)
    {
        if (m_rgli[i].dwFlags & LVF_ITEMSELECTED)
        {
            UnselectItem(i);
        }
    }
    g_statusbar.Update();
}

void CMyListView::SelectAllItems()
{
    g_statusbar.DeferUpdate();
    Notify_SelectChange(NULL, false);

    for (int i = 0; i < m_cItems; i++)
    {
        if (~(m_rgli[i].dwFlags & LVF_ITEMSELECTED))
        {
            SelectSingleItem(i);
        }
    }
    g_statusbar.Update();
}

void CMyListView::UpdateAllItems()
{
    g_statusbar.DeferUpdate();
    for (int i = 0; i < m_cItems; i++)
        RefreshItem(i);
    g_statusbar.Update();
}

BOOL CMyListView::IsSelected(int iItem)
{
    // undone: allow subclass to reject selection
    return (m_rgli[iItem].dwFlags & LVF_ITEMSELECTED) ? TRUE : FALSE;
}

void CMyListView::SelectSingleItem(int iItem)
{
    // undone: allow subclass to reject selection
    m_rgli[iItem].dwFlags |= LVF_ITEMSELECTED;
    Notify_SelectChange(m_rgli[iItem].pvData, true);
    RefreshItem(iItem);
}

void CMyListView::UnselectItem(int iItem)
{
    // undone: allow subclass to reject selection
    m_rgli[iItem].dwFlags &= ~LVF_ITEMSELECTED;
    Notify_SelectChange(m_rgli[iItem].pvData, false);
    RefreshItem(iItem);
}

void CMyListView::RefreshItem(int iItem)
{
    int nCols = Header_GetItemCount(m_hwndHeader);
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    // Is the item onscreen?
    RECT rcItem = rcClient, rcClip;
    rcItem.left -= m_nCurHScrollLoc;
    rcItem.right = rcItem.left + GetHeaderWidth();
    rcItem.top =  iItem * m_nItemHeight - m_nCurVScrollLoc + m_nHeaderHeight;
    rcItem.bottom = rcItem.top + m_nItemHeight;
    RECT rcFocus = rcItem;
    BOOL fOnScreen = IntersectRect(&rcClip, &rcItem, &rcClient);

    rcItem.right = rcItem.left + m_rgnColWidths[0]; 
    HDC hdc = GetDC(m_hwnd);
    SelectObject(hdc, m_hfont);
    int nC = -m_nCurHScrollLoc;
    DWORD dwFocusFlag = m_fHaveFocus ? LVF_ITEMFOCUSED : 0;
    for (int c = 0; c < nCols; c++)
    {
        // Is the subitem onscreen?
        BOOL fSubOnScreen = fOnScreen;
        if (fOnScreen)
        {
            rcItem.left  = nC;
            rcItem.right = rcItem.left + m_rgnColWidths[c];
            fSubOnScreen = IntersectRect(&rcClip, &rcItem, &rcClient);
        }

        RECT *prcClip;
        RECT rc = rcItem;
        if (fSubOnScreen)
            prcClip = &rc;
        else
            prcClip = NULL;
        UpdateItem(m_rgli[iItem].pvData, m_rgli[iItem].dwFlags | dwFocusFlag, c, hdc, prcClip);

        nC += m_rgnColWidths[c];
    }

    if (m_iFocusItem == iItem)
        if (m_fHaveFocus)
            DrawFocusRect(hdc, &rcFocus);
    
    ReleaseDC(m_hwnd, hdc);
}

void CMyListView::HandleButtonDown(int nX, int nY, DWORD dwFlags)
{
    sMouseInfo mi;

    GetMouseInfoAt(nX, nY, &mi);

    m_fHaveFocus = true;

    // Give the listview keyboard input focus
    SetFocus(m_hwnd);

    // let the subclass completely override us if so desired
    if (!Override_LButtonDown(&mi, dwFlags))
        return;

    // Over an item?
    if (mi.pvItem == NULL) {
        // Didn't click on an item.  Since we don't support dragrects, there's
        // nothing to do here.
        UnselectAllItems();
    }
    else
        SelectItem(mi.iRow, dwFlags);
}

void CMyListView::HandleButtonDblClick(int nX, int nY, DWORD dwFlags)
{
    sMouseInfo mi;

    GetMouseInfoAt(nX, nY, &mi);

    m_fHaveFocus = true;

    // Give the listview keyboard input focus
    SetFocus(m_hwnd);

    // let the subclass completely override us if so desired
    Override_LButtonDblClick(&mi, dwFlags);

    // We don't do anything for double clicks ourselves.
}

void CMyListView::SelectItem(int iItem, DWORD dwFlags)
{
    int i1, i2;

    if (iItem < m_iShiftItem) {
        i1 = iItem;
        i2 = m_iShiftItem;
    } else {
        i1 = m_iShiftItem;
        i2 = iItem;
    }

    g_statusbar.DeferUpdate();

    int iPrevFocus = m_iFocusItem;
    SetFocusItem(-1);
    if (iPrevFocus != -1)
        RefreshItem(iPrevFocus);
    // User clicked on item 'iItem'.  if 'ctrl' isn't pressed, then
    // unselect all other previously selected items
    if (!(dwFlags & MK_CONTROL)) {
        // If shift is pressed then unselect those items NOT in the new selection range
        if (!(dwFlags & MK_SHIFT))
            UnselectAllItems();
        else {
            for (int i = 0; i < m_cItems; i++) {
                if (IsSelected(i) && (i < i1 || i > i2)) {
                    UnselectItem(i);
                    RefreshItem(i);
                }
            }
        }
    }

    // If 'shift' is pressed, then select every item between the current-focus
    // item and the item the user clicked on.
    if ((dwFlags & MK_SHIFT) && m_iShiftItem != -1) {
        for (int i = i1; i <= i2; i++)
            if (!IsSelected(i))
                SelectSingleItem(i);
    } else {
        // Finally, select the item
        if (IsSelected(iItem) && !(dwFlags & MK_FORCESEL))
            UnselectItem(iItem);
        else
            SelectSingleItem(iItem);
    }
    SetFocusItem(iItem);
    RefreshItem(m_iFocusItem);
    g_statusbar.Update();
}

void CMyListView::HandleButtonUp(int nX, int nY, DWORD dwFlags)
{
    sMouseInfo mi;
    GetMouseInfoAt(nX, nY, &mi);
    // No dragrect, so no default handling
    Override_LButtonUp(&mi, dwFlags);
}

void CMyListView::HandleMouseMove(int nX, int nY, DWORD dwFlags)
{
    sMouseInfo mi;
    GetMouseInfoAt(nX, nY, &mi);

    // No dragrect, so no default handling.
    Override_MouseMove(&mi, dwFlags);
}

void CMyListView::HandleHeaderDoubleClick(LPNMHEADER pnmhdr)
{
    // Find the length of the contents of all rows in the specified column;
    // resize the column so that the longest one fits.
    int nMaxWidth = 0;
    HDC hdc = GetDC(m_hwnd);
    SelectObject(hdc, m_hfont);
    for (int i = 0; i < m_cItems; i++)
        nMaxWidth = max(nMaxWidth, (int)GetItemWidth(hdc, m_rgli[i].pvData, pnmhdr->iItem));

    ReleaseDC(m_hwnd, hdc);

    HDITEM hdi;
    hdi.mask = HDI_WIDTH;
    hdi.cxy = nMaxWidth;
    Header_SetItem(m_hwndHeader, pnmhdr->iItem, &hdi);


    UpdateColumnWidths();
    UpdateScrollBars();
    InvalidateRect(m_hwnd, NULL, TRUE);

    // Update the position of the header control in case it's been scrolled.
    // Note that this creates slightly strange behaviour, but the behaviour
    // exactly mimics what WinXP does, so, when in rome...
    // (example: shrink window so that horz scroll bar appears; scroll
    //  window all the way to the right; shrink one of the column headers;
    //  RESULT: The whole column quickly shrinks).
    SetWindowPos(m_hwndHeader, NULL, -m_nCurHScrollLoc, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

}

BOOL CMyListView::InitHeader()
{
    RECT rcParent;
    HDLAYOUT hdl;
    WINDOWPOS wp;

    m_hwndHeader = CreateWindow(WC_HEADER, "", WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ | HDS_FULLDRAG, 0, 0, 0, 0,
                                m_hwnd, (HMENU)WNDID_LISTVIEW_HEADER, g_hInst, NULL);
    if (m_hwndHeader == NULL)
        return FALSE;

    // Retrieve the bounding rectangle of the listview's 
    // client area, and then request size and position values 
    // from the header control. 
    GetClientRect(m_hwnd, &rcParent); 

    hdl.prc = &rcParent; 
    hdl.pwpos = &wp; 
    if (!Header_Layout(m_hwndHeader, &hdl)) 
        return FALSE;
    m_nHeaderHeight = wp.cy;

    m_himlSortArrows = ImageList_Create(16, 16, ILC_COLOR8, 2, 1);
    if (m_himlSortArrows == NULL) 
        return FALSE; 

    HBITMAP hbmp;
    if ((hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_SORTARROWS))) == NULL)
        return FALSE;
    int nImageIndex = ImageList_Add(m_himlSortArrows, hbmp, (HBITMAP) NULL);
    DeleteObject(hbmp);
    if (nImageIndex == -1)
        return FALSE;

    Header_SetImageList(m_hwndHeader, m_himlSortArrows);

    return TRUE;
}

void CMyListView::SetVisible(bool fVisible)
{
    ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE);
}

int CMyListView::GetHeaderWidth()
{
    return m_nHeaderWidth;
}

int CMyListView::GetTotalItemHeight()
{
    return m_cItems * m_nItemHeight;
}
        
void CMyListView::UpdateIfDirty()
{
    if (m_fDirty)
    {
        InvalidateRect(m_hwnd, NULL, FALSE);
        SetWindowPos(m_hwndHeader, NULL, -m_nCurHScrollLoc, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        InvalidateRect(m_hwndHeader, NULL, TRUE);
        UpdateWindow(m_hwndHeader);

        UpdateScrollBars();
        
        m_fInvertSort = !m_fInvertSort;
        SortColumn(m_iSortColumn);
        
        UpdateWindow(m_hwnd);
        m_fDirty = FALSE;
    }
}

void CMyListView::Resize(RECT *prc)
{
    m_nMaxHScroll = max(GetHeaderWidth() - (prc->right - prc->left), 0); 
    m_nMaxVScroll = max(GetTotalItemHeight() - (prc->bottom - prc->top-m_nHeaderHeight - 30), 0); 
    m_nCurHScrollLoc = min(m_nCurHScrollLoc, m_nMaxHScroll); 
    m_nCurVScrollLoc = min(m_nCurVScrollLoc, m_nMaxVScroll); 

    MoveWindow(m_hwnd, prc->left, prc->top, prc->right - prc->left,
               prc->bottom - prc->top, TRUE);
    MoveWindow(m_hwndHeader, -m_nCurHScrollLoc, 0, prc->right - prc->left+m_nCurHScrollLoc, m_nHeaderHeight, FALSE);
    UpdateWindow(m_hwndHeader);
    
    UpdateScrollBars();

    UpdateWindow(m_hwnd);
}

void CMyListView::UpdateScrollBars()
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    m_nMaxHScroll = max(GetHeaderWidth() - (rc.right - rc.left), 0); 
    m_nMaxVScroll = max(GetTotalItemHeight() - (rc.bottom - rc.top-m_nHeaderHeight - m_nItemHeight), 0); 
    m_nCurHScrollLoc = min(m_nCurHScrollLoc, m_nMaxHScroll); 
    m_nCurVScrollLoc = min(m_nCurVScrollLoc, m_nMaxVScroll); 

    // Update the horz/vert scrollbars so that the dragbar is proportional to the
    // size of the listview in that dimension
    SCROLLINFO si;
    si.cbSize = sizeof si;
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = GetHeaderWidth();
    si.nPage = rc.right - rc.left;
    SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
    
    si.nMin = 0;
    if (m_nMaxVScroll == 0)
        si.nMax = GetTotalItemHeight()+m_nItemHeight;
    else
        si.nMax = ((GetTotalItemHeight() + m_nHeaderHeight + m_nItemHeight) / m_nItemHeight) * m_nItemHeight;
    si.nPage = rc.bottom - rc.top;
    SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);

    // Do we need to show/enable the scroll bars?
    BOOL fShowHScroll = rc.right - rc.left < GetHeaderWidth();
    BOOL fShowVScroll = rc.bottom - rc.top > m_nHeaderHeight && rc.bottom - rc.top < GetTotalItemHeight() + m_nHeaderHeight;
    ShowScrollBar(m_hwnd, SB_HORZ, fShowHScroll);
    ShowScrollBar(m_hwnd, SB_VERT, fShowVScroll);
    EnableScrollBar(m_hwnd, SB_HORZ, fShowHScroll ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
    EnableScrollBar(m_hwnd, SB_VERT, fShowVScroll ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
}

BOOL CMyListView::AddItemAtIndex(void *pvItem, int iLoc)
{
    if (m_cItems + 1 > m_cMaxItems)
    {
        // Realloc the internal storage container
        m_cMaxItems += m_cAddItemDelta;
        m_rgli = (sListItem*)realloc(m_rgli, m_cMaxItems * sizeof(sListItem));
    }
    m_rgli[m_cItems].pvData = pvItem;
    m_rgli[m_cItems].dwFlags = 0;
    m_cItems++;
    m_fDirty = TRUE;
    return TRUE;
}

BOOL CMyListView::DeleteItem(void *pvItem)
{
    // Remove the specified item from the listview
    // UNDONE: very unoptimized
    for (int i = 0; i < m_cItems; i++)
    {
        if (m_rgli[i].pvData == pvItem)
        {
            // found it.  Remove it.
            if (i < m_cItems - 1)
                memcpy(&m_rgli[i], &m_rgli[i+1], (m_cItems - i - 1) * sizeof (sListItem));
            m_cItems--;
            m_fDirty = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

void CMyListView::Clear()
{
    m_iFocusItem = -1;
    m_fHaveFocus = false;
    m_iShiftItem = -1;
    m_nCurHScrollLoc = 0;
    m_nCurVScrollLoc = 0;
    SetScrollPos(m_hwnd, SB_HORZ, 0, FALSE);
    SetScrollPos(m_hwnd, SB_VERT, 0, FALSE);
    free(m_rgli);
    // Note: leave m_cMaxItems at it's highwater point?
    m_cMaxItems = 0;
    m_cItems    = 0; 
    m_rgli      = NULL;
    m_fDirty    = true;

    InvalidateRect(m_hwnd, NULL, TRUE);
    UpdateWindow(m_hwnd);
}

void CMyListView::AddColumn(char *szName, UINT nWidth)
{
    HDITEM hdi; 

    hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
    hdi.fmt = HDF_OWNERDRAW; 

    hdi.pszText = szName; 
    hdi.cxy = nWidth; 
    hdi.cchTextMax = lstrlen(hdi.pszText); 
 
    Header_InsertItem(m_hwndHeader, 1000, (LPARAM)&hdi);
    UpdateColumnWidths();
}

void CMyListView::UpdateColumnWidths()
{
    m_nHeaderWidth = 0;
    int nCols = Header_GetItemCount(m_hwndHeader);
    for (int i = 0; i < nCols; i++)
    {
        HDITEM hdi;
        hdi.mask = HDI_WIDTH;
        Header_GetItem(m_hwndHeader, i, &hdi);
        m_nHeaderWidth += hdi.cxy;
        m_rgnColWidths[i] = hdi.cxy;
    }
}

void CMyListView::UpdateListDisplay()
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint(m_hwnd, &ps);
    RECT rcPaint = ps.rcPaint;

    // For now, draw the items in their entirety
    int nY = m_nHeaderHeight;
    int nFirstItem = m_nCurVScrollLoc / m_nItemHeight;
    int nX;

    SelectObject(hdc, m_hfont);
    DWORD dwFocusFlag = m_fHaveFocus ? LVF_ITEMFOCUSED : 0;
    int nLastItem = min(m_cItems, nFirstItem + (ps.rcPaint.bottom + m_nItemHeight) / m_nItemHeight);
    for (int i = nFirstItem; i < nLastItem; i++)
    {
        nX = -m_nCurHScrollLoc;
        for (int c = 0; c < NUM_COLUMNS; c++)
        {
            RECT rcDummy, rc = {nX, nY, nX + m_rgnColWidths[c], nY + m_nItemHeight};
            if (IntersectRect(&rcDummy, &rc, &rcPaint))
                UpdateItem(m_rgli[i].pvData, m_rgli[i].dwFlags | dwFocusFlag, c, hdc, &rc);

            nX += m_rgnColWidths[c];
        }
        if (m_fHaveFocus && m_iFocusItem == i)
        {
            RECT rc = {-m_nCurHScrollLoc, nY, -m_nCurHScrollLoc + m_nHeaderWidth, nY + m_nItemHeight};
            DrawFocusRect(hdc, &rc);
        }
        nY += m_nItemHeight;
    }

    // Fill unused area if necessary
    if (nX < rcPaint.right)
    {
        RECT rc = rcPaint;
        rc.left = nX;
        FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
    }

    if (nY < rcPaint.bottom)
    {
        RECT rc = rcPaint;
        rc.top = nY;
        FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
    }

    EndPaint(m_hwnd, &ps);
}
