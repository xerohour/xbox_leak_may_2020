// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CListView_OwnerDraw.cpp
// Contents:  
// Revisions: 8-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

extern int gs_rgnDefColWidth[NUM_COLUMNS];


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetLineLength
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetLineLength()
{
    int nWidth = 0;
    for (int i = 0; i < m_nColumns; i++)
        nWidth += ListView_GetColumnWidth(m_hwnd, i);

    return nWidth;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::DrawInsertionBar
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::DrawInsertionBar(POINT pt)
{
    // Convert coordinates to local to us
    MapWindowPoints(m_pwindow->m_hwnd, m_hwnd, &pt, 1);

    // Determine the entry that occurs immediately after the specified coordinate
    int iEntry = ((pt.y - GetColumnHeight()/2)/ GetColumnHeight()) + GetTopRow();

    // Don't let selection bar move past last entry
    if (iEntry >= GetTotalNumItems())
    {
        iEntry = GetTotalNumItems();
        if (m_fInsideOut)
        {
            // Find the farthest out object and use it.
            iEntry--;
            while (iEntry)
            {
                CObject *pobj = GetObjectFromIndex(iEntry);
                if (pobj && pobj->GetType() != OBJ_SEC && pobj->GetType() != OBJ_VOLDESC && pobj->GetType() != OBJ_EMPTY)
                    break;
                iEntry--;
            }
            iEntry++;
        }
    }

    // If the entry is at a nonmoveable object, then move outward until it's not
    CObject *pobj  = GetObjectFromIndex(iEntry);
    int iNextEntry = iEntry;
    while (pobj && (pobj->GetType() == OBJ_SEC   || pobj->GetType() == OBJ_VOLDESC ||
                    pobj->GetType() == OBJ_EMPTY))
    {
        iNextEntry = (m_fInsideOut) ? iEntry - 1 : iEntry + 1;
        if ((pobj = GetObjectFromIndex(iNextEntry)) == NULL)
            break;

        if (pobj && (pobj->GetType() == OBJ_FILE || pobj->GetType() == OBJ_FOLDER))
            break;
        iEntry = iNextEntry;
    }
    if (!m_fInsideOut)
        iEntry = iNextEntry;
    if (iEntry == -1)
        iEntry = 0;

    // Don't insert at a group - move "upward" until it's not.
    pobj = GetObjectFromIndex(iEntry);
    iNextEntry = iEntry;
    while (pobj && pobj->m_pog)
    {
        iNextEntry = iEntry - 1;
        if ((pobj = GetObjectFromIndex(iNextEntry)) == NULL || pobj->m_pog == NULL)
            break;
        iEntry = iNextEntry;
        pobj = GetObjectFromIndex(iEntry);
    }

    // Don't insert between two selected items - go to item "above" it.
    pobj = GetObjectFromIndex(iEntry);
    iNextEntry = iEntry;
    while (pobj && IsItemSelected(iNextEntry))
    {
        iNextEntry = iEntry - 1;
        if ((pobj = GetObjectFromIndex(iNextEntry)) == NULL || !IsItemSelected(iNextEntry))
            break;
        iEntry = iNextEntry;
        pobj = GetObjectFromIndex(iEntry);
    }

        
    // Don't redraw if not necessary
    if (m_iPrevBarEntry == iEntry)
        return;

    int nY = (iEntry - GetTopRow() )* GetColumnHeight() + GetHeaderHeight() + 1;

    // Draw the insertion line at the specified location
    HDC hdc = GetDC(m_hwnd);
    HPEN hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

    MoveToEx(hdc, 0, nY, NULL);
    LineTo(hdc, GetLineLength(), nY);
    SelectObject(hdc, hpenOld);
    DeleteObject(hpen);

    ErasePreviousSelectionBar();
    ReleaseDC(m_hwnd, hdc);

    // Track which entry is the 'previous' entry for erasing purposes
    m_iPrevBarEntry = iEntry;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::IsItemSelected(int iItem)
{
    LVITEM lvi;
    lvi.mask = LVIF_STATE;
    lvi.iItem = iItem;
    lvi.iSubItem = 0;
    lvi.stateMask = LVIS_SELECTED;
    if (!ListView_GetItem(m_hwnd, &lvi))
        return false;
        
    return (lvi.state & LVIS_SELECTED) ? true: false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::ErasePreviousSelectionBar
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::ErasePreviousSelectionBar()
{
    // Erase the previous entry
    if (m_iPrevBarEntry != -1)
    {
        HDC hdc = GetDC(m_hwnd);
        HPEN hpen = CreatePen(PS_SOLID, 1, RGB(212, 208, 200));
        HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
        int nY = (m_iPrevBarEntry - GetTopRow() )* GetColumnHeight() + GetHeaderHeight() + 1;

        MoveToEx(hdc, 0, nY, NULL);
        LineTo(hdc, GetLineLength(), nY);
        SelectObject(hdc, hpenOld);
        DeleteObject(hpen);

        CObject *pobj = GetObjectFromIndex(m_iPrevBarEntry-1);
        if (pobj && pobj->m_pog)
        {
            // member of a group, so draw blue bar
            HPEN hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
            HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
            MoveToEx(hdc, 2, nY, NULL);
            LineTo(hdc, min(6, GetLineLength()), nY);
            SelectObject(hdc, hpenOld);
            DeleteObject(hpen);
        }

        ReleaseDC(m_hwnd, hdc);
        m_iPrevBarEntry = -1;
    }
}

void CListView::DrawGridLines(LPDRAWITEMSTRUCT pdis)
{
    HPEN hpen = CreatePen(PS_SOLID, 0, RGB(212,208,200));
    HPEN hpenOld = (HPEN)SelectObject(pdis->hDC, hpen);
    HBRUSH hBrushOld = (HBRUSH)SelectObject(pdis->hDC, GetStockObject(NULL_BRUSH));
    Rectangle(pdis->hDC, pdis->rcItem.left-1, pdis->rcItem.top-1, pdis->rcItem.right, pdis->rcItem.bottom);

    int nWidth = 0;
    for (int i = 0; i < sizeof(gs_rgnDefColWidth)/sizeof(int)-1; i++)
    {
        nWidth += ListView_GetColumnWidth(m_hwnd, i);
        Rectangle(pdis->hDC, nWidth, pdis->rcItem.top-1, nWidth + 1, pdis->rcItem.bottom);
    }
    SelectObject(pdis->hDC, hpenOld);
    SelectObject(pdis->hDC, hBrushOld);
    DeleteObject(hpen);
}

void CListView::HandleSelectionState(LPDRAWITEMSTRUCT pdis, char *szBuf)
{
    HBRUSH hBrush;
    DWORD dwOldTextColor, dwOldBkColor;
    bool fGroupedItem = false, fLastGroupedItem = false, fFirstGroupedItem = false;

    // Set up the various rectangles we'll need for rendering
    int nColWidth = ListView_GetColumnWidth(m_hwnd, 0);
    
    // The main rectangle for the first subitem of the item
    RECT rcItem = pdis->rcItem;
    rcItem.left += 2;
    rcItem.right = rcItem.left + nColWidth - 2;

    RECT rcGroupVertLine = rcItem;
    rcGroupVertLine.right = rcGroupVertLine.left + 4;
    rcGroupVertLine.top--;
    rcGroupVertLine.bottom++;

    RECT rcGroupTopLine = rcItem;
    rcGroupTopLine.bottom = rcGroupTopLine.top + 2;
    rcGroupTopLine.right = rcGroupTopLine.left + 6;

    RECT rcGroupBotLine = rcItem;
    rcGroupBotLine.bottom--;
    rcGroupBotLine.top = rcGroupBotLine.bottom - 2;
    rcGroupBotLine.right = rcGroupBotLine.left + 6;

    // Get a pointer to the object in the current item
    CObject *pobj = GetObjectFromIndex(pdis->itemID);
    assert(pobj);
    
    // Is the object a member of a group?
    if (pobj->m_pog)
    {
        fGroupedItem = true;
        
        // Is the object the *first* member of the group?
        if (((!m_fInsideOut) && pobj->m_pog->m_gol.Head() == pobj) || ((m_fInsideOut) && pobj->m_pog->m_gol.Tail() == pobj))
        {
            fFirstGroupedItem = true;
            rcGroupVertLine.top++;
        }

        // Is the object the *last* member of the group?
        if (((!m_fInsideOut) && pobj->m_pog->m_gol.Tail() == pobj) || ((m_fInsideOut) && pobj->m_pog->m_gol.Head() == pobj))
        {
            fLastGroupedItem = true;
            rcGroupVertLine.bottom-=2;
        }
    }

    if (fGroupedItem)
    {
        rcItem.left += 8;
    }

    // if selected, draw selected item's text white, highlight rectangle blue
    if (pdis->itemState & ODS_SELECTED)
    {
        // draw background rectangle
        if (m_fHaveFocus)
            hBrush = CreateSolidBrush(RGB(0, 0, 128));
        else
            hBrush = CreateSolidBrush(RGB(192, 192, 192));
        RECT rcTemp = pdis->rcItem;
        rcTemp.bottom--;
        rcTemp.left--;
        FillRect(pdis->hDC, &rcTemp, hBrush);
        DeleteObject(hBrush);

        // Draw grid lines.  We don't use the default grid lines since we don't
        // want them to draw over the green 'grouped item' borders...
        DrawGridLines(pdis);

        // Draw group descriptor
        if (rcItem.left < rcItem.right)
            if (fGroupedItem)
            {
                hBrush = CreateSolidBrush(RGB(0, 0, 128));
                FillRect(pdis->hDC, &rcGroupVertLine, hBrush);
                if (fLastGroupedItem)
                    FillRect(pdis->hDC, &rcGroupBotLine, hBrush);
                if (fFirstGroupedItem)
                    FillRect(pdis->hDC, &rcGroupTopLine, hBrush);
                DeleteObject(hBrush);
            }
        if (rcItem.left < rcItem.right)
        {
            // change text and back ground color of list box's selection
            if (m_fHaveFocus)
            {
                dwOldTextColor = SetTextColor(pdis->hDC, RGB(255,255,255));
                dwOldBkColor = SetBkColor(pdis->hDC, RGB(0,0,255));
            }
            else
            {
                dwOldTextColor = SetTextColor(pdis->hDC, RGB(0, 0, 0));
                dwOldBkColor = SetBkColor(pdis->hDC, RGB(192,192,192));
            }
            if (pobj->GetType() == OBJ_FOLDER)
            {
                HDC hdcBmp = CreateCompatibleDC(pdis->hDC);
                SelectObject(hdcBmp, m_hbmpSel);
                BitBlt(pdis->hDC, rcItem.left, rcItem.top, 14, 12, hdcBmp, 0, 0, SRCCOPY);
                DeleteDC(hdcBmp);
                rcItem.left += 15;
            }

            ExtTextOut(pdis->hDC, (int)(rcItem.left), (int)(rcItem.top),
                       ETO_CLIPPED, &rcItem, (LPSTR)szBuf, lstrlen(szBuf), NULL);

            if (pobj->GetType() == OBJ_FOLDER)
                rcItem.left -= 15;

            // Draw the subitems
            if (fGroupedItem)
                rcItem.left -= 8;
            int nWidth = rcItem.left;
            for (int i = 1; i < NUM_COLUMNS; i++)
            {
                char sz[1024];
                rcItem.left += ListView_GetColumnWidth(m_hwnd, i-1);
                nWidth = ListView_GetColumnWidth(m_hwnd, i);

                ListView_GetItemText(m_hwnd, pdis->itemID, i, sz, 1024);
                rcItem.right = rcItem.left + nWidth - 2;
                ExtTextOut(pdis->hDC, (int)(rcItem.left), (int)(rcItem.top),
                           ETO_CLIPPED, &rcItem, (LPSTR)sz, lstrlen(sz), NULL);
            }
            // restore text and back ground color of list box's selection
            SetTextColor(pdis->hDC, dwOldTextColor);
            SetBkColor(pdis->hDC, dwOldBkColor);
        }
    }
    else // item not selected; draw black text on white background
    {
        // draw white back ground rectangle
        hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        RECT rcTemp = pdis->rcItem;
        rcTemp.bottom--;
        FillRect(pdis->hDC, &rcTemp, hBrush);

        // Draw grid lines.  We don't use the default grid lines since we don't
        // want them to draw over the green 'grouped item' borders...
        DrawGridLines(pdis);

        if (pobj->GetType() == OBJ_SEC || pobj->GetType() == OBJ_VOLDESC || pobj->GetType() == OBJ_EMPTY)
        {
            if (rcItem.left < rcItem.right)
            {
                if (pobj->GetType() == OBJ_EMPTY)
                    hBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 192));
                else
                    hBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(192, 0, 0));

                int nOldRight = rcItem.right;
                rcItem.right = min(rcItem.right, rcItem.left + 20);
                rcItem.bottom--;
                FillRect(pdis->hDC, &rcItem, hBrush);
                DeleteObject(hBrush);

                HPEN hpen;
                if (pobj->GetType() == OBJ_EMPTY)
                    hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 192));
                else
                    hpen = CreatePen(PS_SOLID, 0, RGB(192, 0, 0));
                HPEN hpenOld = (HPEN)SelectObject(pdis->hDC, hpen);
                hBrush = (HBRUSH)SelectObject(pdis->hDC, GetStockObject(NULL_BRUSH));
                Rectangle(pdis->hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
                SelectObject(pdis->hDC, hpenOld);
                SelectObject(pdis->hDC, hBrush);
                DeleteObject(hpen);

                rcItem.right = nOldRight - 1;
                ExtTextOut(pdis->hDC, (int)(rcItem.left) + 23, (int)(rcItem.top),
                           ETO_CLIPPED, &rcItem, (LPSTR)szBuf, lstrlen(szBuf), NULL);
            }
        }
        else
        {
            // Draw group descriptor
            if (rcItem.left < rcItem.right)
                if (fGroupedItem)
                {
                    hBrush = CreateSolidBrush(RGB(0, 0, 128));
                    FillRect(pdis->hDC, &rcGroupVertLine, hBrush);
                    if (fLastGroupedItem)
                        FillRect(pdis->hDC, &rcGroupBotLine, hBrush);
                    if (fFirstGroupedItem)
                        FillRect(pdis->hDC, &rcGroupTopLine, hBrush);
                    DeleteObject(hBrush);
                }

            if (pobj->GetType() == OBJ_FOLDER)
            {
                HDC hdcBmp = CreateCompatibleDC(pdis->hDC);
                SelectObject(hdcBmp, m_hbmp);
                BitBlt(pdis->hDC, rcItem.left, rcItem.top, 14, 12, hdcBmp, 0, 0, SRCCOPY);
                DeleteDC(hdcBmp);
                rcItem.left += 15;
            }
        
            // draw normal text
            if (rcItem.left < rcItem.right)
                ExtTextOut(pdis->hDC, (int)(rcItem.left), (int)(rcItem.top),
                           ETO_CLIPPED, &rcItem, (LPSTR)szBuf, lstrlen(szBuf), NULL);
            if (pobj->GetType() == OBJ_FOLDER)
                rcItem.left -= 15;
        }

        // Draw the subitems
        if (fGroupedItem)
            rcItem.left -= 8;
        int nWidth = rcItem.left;
        for (int i = 1; i < NUM_COLUMNS; i++)
        {
            char sz[1024];
            rcItem.left += ListView_GetColumnWidth(m_hwnd, i-1);
            nWidth = ListView_GetColumnWidth(m_hwnd, i);

            ListView_GetItemText(m_hwnd, pdis->itemID, i, sz, 1024);
            rcItem.right = rcItem.left + nWidth - 2;
            ExtTextOut(pdis->hDC, (int)(rcItem.left), (int)(rcItem.top),
                       ETO_CLIPPED, &rcItem, (LPSTR)sz, lstrlen(sz), NULL);
        }
    }
    return;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::HandleFocusState
// Purpose:   Handles indicating focus of control with black dashed
//            rectangular border.  
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::HandleFocusState(LPDRAWITEMSTRUCT pdis)
{
    pdis->rcItem.bottom--;
    DrawFocusRect(pdis->hDC, (LPRECT)&pdis->rcItem);
    return;
}

bool CListView::HandleDrawItem(LPDRAWITEMSTRUCT pdis)
{
    char szBuf[1024];

    // if there are no items in the control yet, indicate focus
    if (pdis->itemID == -1)
    {
        HandleFocusState(pdis);
        return (TRUE);
    }

    // normal/any item draw action -- checks for selection and focus
    if (pdis->itemAction & ODA_DRAWENTIRE)
    {
        ListView_GetItemText(m_hwnd, pdis->itemID, 0, szBuf, MAX_PATH);

        // Handle both selected and unselected states, so this
        //   itemAction's code could be combined with ODA_SELECT's.
        //   Typically, the selection state is an addition to drawing
        //   the normal state, but in this case the selection state
        //   is being draw differently, so don't draw normal state
        //   if selected.
        HandleSelectionState(pdis, szBuf);

        // Handle focus state if necessary
        if (pdis->itemState & ODS_FOCUS)
            HandleFocusState(pdis);
        return TRUE;
    }

    // selected/unselected item draw action -- white text on blue background
    if (pdis->itemAction & ODA_SELECT)
    {
        // Use the itemID to get to the text string
        SendMessage(m_hwnd, LB_GETTEXT, (WORD)pdis->itemID,
                    (LONG)(LPSTR)szBuf);
        HandleSelectionState(pdis, szBuf);
        
        // Handle focus state if necessary
        if (pdis->itemState & ODS_FOCUS)
            HandleFocusState(pdis);
        return TRUE;
    }

    // focus/unfocus item draw action -- dashed rectangular border
    if (pdis->itemAction & ODA_FOCUS)
    {
        HandleFocusState(pdis);
        return TRUE;
    }
    return TRUE;
}

