// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      clistview.cpp
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <zmouse.h>

// Default column widths
int gs_rgnDefColWidth[NUM_COLUMNS] = {125, 175, 80, 125};


extern DWORD BlockSize(ULONGLONG uliSize);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ListViewWndProc
// Purpose:   
// Arguments: hwnd              -- The handle to the window that the message is intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CALLBACK ListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    // Determine which CListView this message is intended for, and call it instead
    CListView *plv = (CListView*)GetWindowLong(hwnd, GWL_USERDATA);
    return plv->WndProc(uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::WndProc
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CListView::WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    NMLISTVIEW   *pnmlv = (NMLISTVIEW*)lparam;

    switch(uMsg)
    {
    case WM_LBUTTONUP:
        // This message ONLY gets sent to us if we're in a dragging state or
        // the user double clicks
        if (m_fDragging)
        {
            POINT pt = {(short)(lparam & 0xffff), (short)(lparam >> 16)};
            EndDrag(pt);
        }
        return 1;

    case WM_LBUTTONDOWN:
        ::SetFocus(m_hwnd);
        break;

    case WM_TIMER:
        if (m_fDragging)
        {
            // User is in the dragging state -- check if they want to scroll
            CheckDragScroll();
        }
        return 1;
        
//    case WM_MOUSEWHEEL:
//    case WM_VSCROLL:
//        return 0;
        
    case WM_MOUSEMOVE:
        m_dwMouseX = LOWORD(lparam); 
        m_dwMouseY = HIWORD(lparam);
        if (!m_fDragging)
            SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        else
        {
            POINT pt = {(short)(lparam & 0xffff), (short)(lparam >> 16)};
            UpdateDrag(pt);
        }
        break;
    }
    return CallWindowProc(m_wndprocOld, m_hwnd, uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::SetFocus
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::SetFocus(bool fHaveFocus)
{
    m_fHaveFocus = fHaveFocus;
    if (!m_fHaveFocus)
    {
        // Remove the input focus rect
        int iItem = ListView_GetNextItem(m_hwnd, -1, LVNI_FOCUSED);
        if (iItem != -1)
            ListView_SetItemState(m_hwnd, iItem, 0, LVIS_FOCUSED);
    }

    InvalidateRect(m_hwnd, NULL, TRUE);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::Init
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::Init(CWindow *pwindow, bool fInsideOut, bool fDisplayLSNs)
{
	LV_COLUMN lvcol;
    int rgnColSizes[NUM_COLUMNS];

    m_fModified = false;
    
    // Track which window we're connected to.
    m_pwindow = pwindow;

    // Create our Win32 listview control
	m_hwnd = CreateWindow(WC_LISTVIEW, "", WS_BORDER | WS_VISIBLE | WS_CHILD | LVS_REPORT |
                          LVS_SHOWSELALWAYS | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED,
                          0, 0, 0, 0, pwindow->m_hwnd, (HMENU)this, pwindow->m_hinst, NULL);
    if (!m_hwnd)
        return false;

    m_fInsideOut = fInsideOut;
    m_fSelecting = false;
    // Disable grid lines (we draw our own)
    ListView_SetExtendedListViewStyleEx(m_hwnd, LVS_EX_GRIDLINES, 0);

    m_iPrevBarEntry = -1;

    // Clear out the cursor since we'll be setting it ourselves (check MSDN for
    // info on why this is necessary).
    SetClassLong(m_hwnd, GCL_HCURSOR, NULL);

    // Set the cursor to the default 'arrow' cursor
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    // Add the columns to the listview

    // Load the column name strings
    m_nColumns = 3;
    LoadString(pwindow->m_hinst, IDS_COLUMN_NAME,   m_rgszColumnNames[0], MAX_PATH);
    LoadString(pwindow->m_hinst, IDS_COLUMN_FOLDER, m_rgszColumnNames[1], MAX_PATH);
    LoadString(pwindow->m_hinst, IDS_COLUMN_SIZE,   m_rgszColumnNames[2], MAX_PATH);
    if (fDisplayLSNs)
    {
        LoadString(pwindow->m_hinst, IDS_COLUMN_LSN, m_rgszColumnNames[3], MAX_PATH);
        m_nColumns++;
    }

    // Get the column sizes
    GetColumnSizes(rgnColSizes);

    m_fSelectedObjList = false;
    m_fSelectingAll = false;

    // Set up the mask so that we set the width, text, and subitem
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add each column
    int nColumns = fDisplayLSNs ? NUM_COLUMNS : NUM_COLUMNS - 1;
	for (int i = 0; i < nColumns; i++)
	{
		lvcol.cx       = rgnColSizes[i];
		lvcol.iSubItem = i;
        lvcol.fmt      = LVCFMT_LEFT;
	    lvcol.pszText  = m_rgszColumnNames[i];

        // Finally, insert the column at the 'ith' position
		if (ListView_InsertColumn(m_hwnd, i, &lvcol) == -1)
            return false;
	}

    m_hbmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_FOLDER));
    m_hbmpSel = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_FOLDERSEL));

    m_fDragging = false;
    SetInited(S_OK);

    // Override the listview's window procedure so that we can capture mouse events.
    m_wndprocOld = (WNDPROC)SetWindowLong(m_hwnd, GWL_WNDPROC, (long)ListViewWndProc);

    m_fPrevTop = false;
    m_fPrevBot = false;

    // Point our listview control's hwnd at 'this' object
    SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::~CListView
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CListView::~CListView()
{
    if (m_hbmp)
        DeleteObject(m_hbmp);
    if (m_hbmpSel)
        DeleteObject(m_hbmpSel);

    if (m_hwnd)
        DestroyWindow(m_hwnd);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::SetVisible
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::SetVisible(bool fVisible)
{
    ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetColumnSizes
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::GetColumnSizes(int rgnColSize[NUM_COLUMNS])
{
    // UNDONE: Read from registry if set during previous run

    // for now, just copy the default values
    memcpy(rgnColSize, gs_rgnDefColWidth, NUM_COLUMNS * 4);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::MoveTo
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::MoveTo(int nX, int nY, int nW, int nH)
{
    m_rc.left = nX;
    m_rc.right = nX + nW;
    m_rc.top = nY;
    m_rc.bottom = nY + nH;

    MoveWindow(m_hwnd, nX, nY, nW, nH, FALSE);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::Clear
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::Clear()
{
    ListView_DeleteAllItems(m_hwnd);
    m_fModified = true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::AddObject
// Purpose:   
// Arguments: pobj          -- The object to add to the listview
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::AddObject(CObject *pobj)
{
    AddObjectAtIndex(pobj, 0);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::AddObject
// Purpose:   
// Arguments: pobj          -- The object to add to the listview
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::AddObjectAtIndex(CObject *pobj, int iIndex)
{
    char szTemp[MAX_PATH];
    LVITEM item;

    if (pobj->GetType() == OBJ_GROUP)
        return;

    item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iSubItem = 0;
    item.iItem = iIndex;
    item.lParam = (long)pobj;
    if (pobj->GetType() == OBJ_FOLDER)
    {
        if (!lstrcmpi(pobj->m_szFullFileName, g_szRootFolderName))
            sprintf(szTemp, "<.>");
        else
            sprintf(szTemp, "<%s>", pobj->m_szName);
        item.pszText = szTemp;
    }
    else
        item.pszText = pobj->m_szName;
	int index = ListView_InsertItem(m_hwnd, &item);

    // Relative path column
    if (!lstrcmpi(pobj->m_szFullFileName, g_szRootFolderName) || !lstrcmpi(pobj->m_szFolder, ""))
    {
        ListView_SetItemText(m_hwnd, index, 1, "");
    }
    else
    {
        char szFolder[MAX_PATH];
        if (!lstrcmpi(pobj->m_szFolder, g_szRootFolderName))
            strcpy(szFolder, ".");
        else if (lstrcmpi(pobj->m_szFolder, ""))
            sprintf(szFolder, ".%s", pobj->m_szFolder + strlen(g_szRootFolderName));
        ListView_SetItemText(m_hwnd, index, 1, szFolder);
    }

    // File size
    FormatByteString(BlockSize(pobj->m_uliSize.LowPart) * 2048, szTemp);
    ListView_SetItemText(m_hwnd, index, 2, szTemp);

    // LSN Range
    sprintf(szTemp, "%d - %d", pobj->m_dwLSN, pobj->m_dwLSN + pobj->m_dwBlockSize - 1);
    ListView_SetItemText(m_hwnd, index, 3, szTemp);

    // Check if the item was selected when previously removed and should now
    // be reselected.
    if (m_fSelectedObjList && pobj->m_fWasSelected)
        ListView_SetItemState(m_hwnd, iIndex, LVNI_SELECTED, LVNI_SELECTED);
    pobj->m_fWasSelected = false;

    m_fModified = true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::SelectAll
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::SelectAll()
{
    LVITEM lvi;

    lvi.stateMask = LVIS_SELECTED;
    lvi.state     = LVIS_SELECTED;
    m_fSelectingAll = true;
    SendMessage(m_hwnd, LVM_SETITEMSTATE, -1, (LPARAM)&lvi);
    m_fSelectingAll = false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::BeginDrag
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::BeginDrag()
{
    if (ListView_GetSelectedCount(m_hwnd) == 0)
        return;
    
    m_fDragging = true;

    // Change the cursor the 'holding'
    HCURSOR hcur = LoadCursor(m_pwindow->m_hinst, MAKEINTRESOURCE(IDC_MOVECURSOR));
    SetCapture(m_hwnd);
    SetCursor(hcur);
    ::SetFocus(m_hwnd);

    // Start the dragscroll timer.
    SetTimer(m_hwnd, 1, 100, NULL);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::EndDrag
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::EndDrag(POINT pt)
{
    ReleaseCapture();
    m_fDragging = false;

    // Change the cursor back to 'arrow'
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    MapWindowPoints(m_hwnd, m_pwindow->m_hwnd, &pt, 1);

    // Notify our window that a drag event (that started with us) has ended.
    m_pwindow->FinishedDrag(this, pt);

    KillTimer(m_hwnd, 1);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::CheckDragScroll
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::CheckDragScroll()
{
    if (m_iPrevBarEntry == 0)
        return;

    int nTop = 40;
    if (m_iLayer == 2) nTop -= 20;

    POINT pt = {m_dwMouseX, m_dwMouseY+20}; 
    if ((signed short)m_dwMouseY < nTop)
    {
        // Have they been here since the last tick?
        if (m_fPrevTop)
        {
            ListView_Scroll(m_hwnd, 0, -GetColumnHeight());
            if (m_iPrevBarEntry != -1)
                DrawInsertionBar(pt);
        }
    
        m_fPrevTop = true;
    }
    else
        m_fPrevTop = false;

    DWORD dwBottom = (DWORD)m_rc.bottom - 40;
    if (m_rc.right - m_rc.left < GetLineLength())
        dwBottom -= 16; // account for the horz scroll bar

    if (m_dwMouseY > dwBottom && m_dwMouseY < dwBottom + 20)
    {
        if (m_fPrevBot)
        {
            ListView_Scroll(m_hwnd, 0, GetColumnHeight());
            if (m_iPrevBarEntry != -1)
                DrawInsertionBar(pt);
        }
        m_fPrevBot = true;
    }
    else
        m_fPrevBot = false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::ObjectDropped
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::ObjectDropped()
{
    m_iDropPoint = m_iPrevBarEntry;

    // Also track which object we dropped at.

    // Did user drop at end of list?
    if (m_iDropPoint >= GetTotalNumItems())
    {
        m_pobjDropAt = (CObject*)INSERT_OUTSIDEEDGE;
    }
    else
    {
        int iDropPoint = m_iDropPoint;
        do
        {
            m_pobjDropAt = GetObjectFromIndex(iDropPoint++);
        } while (m_pobjDropAt && m_pobjDropAt->GetType() == OBJ_EMPTY);
        
        if (!m_pobjDropAt)
            m_pobjDropAt = (CObject*)INSERT_INSIDEEDGE;
        else if (m_fInsideOut)
        {
            m_pobjDropAt = m_pobjDropAt->m_poOuter;
            if (m_pobjDropAt->GetType() == OBJ_GROUP)
                m_pobjDropAt = m_pobjDropAt->m_poOuter;
        }
    }
    m_pwindow->EraseAllSelectionBars();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::UpdateDrag
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::UpdateDrag(POINT pt)
{
    // Convert the point from relative to us, to relative to our parent window
    MapWindowPoints(m_hwnd, m_pwindow->m_hwnd, &pt, 1);

    // Inform our window that a drag move has occurred.  Our presence in this function
    // does not mean that the move occurred *over* us, merely that the mouse has moved
    // during a drag that *started in* us.  Our window needs to determine what listview
    // (if any) the drag is currently actually over.
    m_pwindow->DragMove(pt);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetHeaderHeight
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetHeaderHeight()
{
    RECT rcHeader;
    HWND hwndHeader = ListView_GetHeader(m_hwnd);
    GetClientRect(hwndHeader, &rcHeader);
    return rcHeader.bottom - rcHeader.top;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetColumnHeight
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetColumnHeight()
{
    POINT pt0, pt1;
    ListView_GetItemPosition(m_hwnd, 0, &pt0);
    ListView_GetItemPosition(m_hwnd, 1, &pt1);
    return pt1.y - pt0.y;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetTotalNumItems
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetTotalNumItems()
{
    return ListView_GetItemCount(m_hwnd);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetNumVisibleRows
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetNumVisibleRows()
{
    return ListView_GetCountPerPage(m_hwnd);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetTopRow
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetTopRow()
{
    return ListView_GetTopIndex(m_hwnd);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetListOfSelectedObjects
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// The list will be in top-to-bottom visibility order, regardless of the source
// list's sort insideout-ness
#define ITEM_BATCH 2
bool CListView::GetListOfSelectedObjects(CObjList *polToMove)
{
    // This function generates a list of all of the selected objects in the
    // listview, and removes them from their concordant layers (so that the only
    // place they exist is in polToMove).
    int *rgn = (int*)malloc(ITEM_BATCH*sizeof(int));
    int cItem = 0, cMaxItem = ITEM_BATCH;
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
    {
        // Get the item's object pointer and add to the list
        CObject *pobj = GetObjectFromIndex(iItem);
        assert(pobj);

        // Keep track of the selected item indices -- we have to remove them
        // in reverse order as a post-processing step due to a bug in the Win32
        // listview class.
        if (cItem == cMaxItem) {
            cMaxItem += ITEM_BATCH;
            rgn = (int*)realloc(rgn, cMaxItem*sizeof(int));
        }
        rgn[cItem++] = iItem;

        // Remove the object from it's layer list.
        pobj->RemoveFromList();
        
        if (pobj->m_pog && !m_fInsideOut)
        {
            // If this is the top-most member of the group, then add the group
            // so that it appears after all of its objects
            if (pobj->m_pog->m_gol.IsFirstMember(pobj))
            {
                pobj->m_pog->RemoveFromList();
                polToMove->AddToHead(pobj->m_pog);
            }
        }

        // Add the object to the passed-in list.
        if (m_fInsideOut)
            polToMove->AddToHead(pobj);
        else
            polToMove->AddToTail(pobj);

        if (pobj->m_pog && m_fInsideOut)
        {
            if (pobj->m_pog->m_gol.IsFirstMember(pobj))
            {
                pobj->m_pog->RemoveFromList();
                polToMove->AddToHead(pobj->m_pog);
            }
        }
    }

    // Now delete the objects from the listview.  We have to do this as a post-
    // processing step due to an apparent bug in the Win32 listview class, in 
    // which certain circumstances caused a call to ListView_DeleteItem to
    // incorrectly remove selection state from a different item.
    for (iItem = cItem - 1; iItem >= 0; iItem--)
        ListView_DeleteItem(m_hwnd, rgn[iItem]);
    free(rgn);
    
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetSelectedFileCountAndSize
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::GetSelectedFileCountAndSize(int *pcFiles, int *pnbyFileSize)
{
    int iItem = -1;
    *pcFiles = 0;
    *pnbyFileSize = 0;

    while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
    {
        // Get the item's object pointer and add to the list
        CObject *pobj = GetObjectFromIndex(iItem);
        assert(pobj);

        (*pcFiles)++;
        *pnbyFileSize += pobj->m_dwBlockSize * 2048;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::RemoveObjectList
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::RemoveObjectList(CObject *rgpol[])
{
    for (int i = 0; rgpol[i] != NULL; i++)
    {
        LVFINDINFO lvfi;
        lvfi.flags = LVFI_PARAM;
        lvfi.lParam = (long)rgpol[i];
        // Remove object rgpol[i] from the list.
        int iItem = ListView_FindItem(m_hwnd, -1, &lvfi);
        if (iItem == -1)
        {
            // Object doesn't exist in this list.  Should never get here
            return false;
        }
        if (!ListView_DeleteItem(m_hwnd, iItem))
            return false;
    }

    m_fModified = true;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::RemoveObject
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::RemoveObject(CObject *po)
{
    LVFINDINFO lvfi;
    lvfi.flags = LVFI_PARAM;
    lvfi.lParam = (long)po;
    // Remove object rgpol[i] from the list.
    int iItem = ListView_FindItem(m_hwnd, -1, &lvfi);
    if (iItem == -1)
    {
        // Object doesn't exist in this list.  Should never get here
        return false;
    }
    if (!ListView_DeleteItem(m_hwnd, iItem))
        return false;

    m_fModified = true;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::CanItemChange
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::CanItemChange(int iItem, DWORD dwOldState, DWORD dwNewState)
{
    // If the item is nonmoveable, then don't allow selection or focus
    CObject *pobj = GetObjectFromIndex(iItem);
    if (!pobj)
        return false;
    if (pobj->m_fIgnoreNextUnselect && ((dwOldState & LVIS_SELECTED) && !(dwNewState & LVIS_SELECTED)))
    {
        pobj->m_fIgnoreNextUnselect = false;
        return false;
    }

    if ((pobj->GetType() == OBJ_SEC || pobj->GetType() == OBJ_VOLDESC || pobj->GetType() == OBJ_EMPTY))
        return false;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::ItemSelected
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::ItemSelected(int iSelectedItem, bool fSelected)
{
    // note: must be at least two files selected to enable group
    bool fGroupable = true, fUngroupable = true;
    int iItem, cFiles, iPrevItem;

    if (m_fSelecting)
        return;
    m_fSelecting = true;

    CObject *pobj = GetObjectFromIndex(iSelectedItem);

    if (pobj->m_pog)
    {
        iItem = -1;
        while ((iItem = ListView_GetNextItem(m_hwnd, iItem, 0)) != -1)
        {
            CObject *pobj2 = GetObjectFromIndex(iItem);
            assert(pobj2);
            if (pobj2->m_pog == pobj->m_pog && pobj2 != pobj)
            {
                ListView_SetItemState(m_hwnd, iItem, (fSelected) ? LVNI_SELECTED : 0, LVNI_SELECTED);
                if (fSelected && !m_fSelectingAll)
                    pobj2->m_fIgnoreNextUnselect = true;
            }
        }
    }
    m_fSelecting = false;
    
    // Get the first selected object
    iItem = 0;
    cFiles = 0;
    iPrevItem = ListView_GetNextItem(m_hwnd, -1, LVNI_SELECTED);
    if (iPrevItem != -1 && IsGroupable(-1, iPrevItem))
    {
        while (iItem != -1)
        {
            cFiles++;
            iItem = ListView_GetNextItem(m_hwnd, iPrevItem, LVNI_SELECTED);
            if (iItem == -1)
                break; // done

            // verify it's groupable
            if (!IsGroupable(iPrevItem, iItem))
            {
                fGroupable = false;
                break;
            }

            iPrevItem = iItem;
        }
    }
    if (cFiles < 2)
        fGroupable = false;

    // If all selected items are grouped, then the user can ungroup them
    if (ListView_GetSelectedCount(m_hwnd) == 0)
        fUngroupable = false;
    else
    {
        iItem = -1;
        while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
        {
            if (!IsUngroupable(iItem))
            {
                fUngroupable = false;
                break;
            }
        }
    }

    m_pwindow->SetGroupable(fGroupable);
    m_pwindow->SetUngroupable(fUngroupable);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::IsGroupable
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::IsGroupable(int iPrevItem, int iItem)
{
    CObject *pobj = GetObjectFromIndex(iItem);
    if (pobj->GetType() == OBJ_GROUP || pobj->GetType() == OBJ_VOLDESC || pobj->m_pog != NULL)
        return false;

    // Verify it's contiguous
    if (iPrevItem != -1 && iItem != iPrevItem + 1)
    {
        if (iItem == iPrevItem + 2)
        {
            // We'll allow it if the intervening object is a placeholder
            if ((GetObjectFromIndex(iPrevItem + 1)->GetType() != OBJ_SEC))
                return false;
        }
        else
            return false;
    }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::IsUngroupable
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::IsUngroupable(int iItem)
{
    CObject *pobj = GetObjectFromIndex(iItem);
    return (pobj->m_pog != NULL);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::ClearSelected
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::ClearSelected()
{
    ListView_SetItemState(m_hwnd, -1, 0, LVIS_SELECTED);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::CheckDroppedOnSelf
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::CheckDroppedOnSelf()
{
    CObject *pobj = GetObjectFromIndex(m_iPrevBarEntry);

    // Check if 'm_iPrevBarEntry' cooresponds to any of the currently selected
    // items.  If so, return true
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
    {
        if (pobj && pobj->m_pog)
        {
            CObject *pobj2 = GetObjectFromIndex(iItem);
            if (pobj2->GetType() == OBJ_GROUP)
                return true;
        }
        if (iItem == m_iPrevBarEntry - (m_fInsideOut) ? 1 : 0)
            return true;
    }
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetObjectFromIndex
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CObject *CListView::GetObjectFromIndex(int iItem)
{
    LVITEM lvi;
    lvi.mask = LVIF_PARAM;
    lvi.iItem = iItem;
    if (!ListView_GetItem(m_hwnd, &lvi))
        return NULL;
        
    return (CObject*)lvi.lParam;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetIndexFromObject
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::GetIndexFromObject(CObject *pobj)
{
    LVFINDINFO lvfi;
    lvfi.flags = LVFI_PARAM;
    lvfi.lParam = (long)pobj;
    return ListView_FindItem(m_hwnd, -1, &lvfi);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GroupSelectedItems
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CObject *CListView::GroupSelectedItems(CObj_Group *pogNew)
{
    CObject *pobjToRet;

    // Get first selected item
    int iFirstSelectedItem = ListView_GetNextItem(m_hwnd, -1, LVNI_SELECTED);
    assert(iFirstSelectedItem != -1);

    // Move all of the selected objects into the new group object
    int iSelectedItem = -1, iFile = 0;
    while ((iSelectedItem = ListView_GetNextItem(m_hwnd, iSelectedItem, LVNI_SELECTED)) != -1)
    {
        // Get the object pointer
        CObject *pobj = GetObjectFromIndex(iSelectedItem);

        // Tell the object that it belongs in the group
        pobj->SetGroup(pogNew);

        // Set the group's LSN to be immediately before the files
        if (iFile == 0)
        {
            pogNew->m_dwLSN = pobj->m_dwLSN;
            pobjToRet = pobj;
        }

        iFile++;
    }
    InvalidateRect(m_hwnd, NULL, false);
    // We know we can't group now
    m_pwindow->SetGroupable(false);
    m_pwindow->SetUngroupable(true);

    m_fModified = true;

    return pobjToRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::UngroupSelectedItems
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::UngroupSelectedItems(CWorkspace *pws, int nLayer)
{
    int iSelectedItem = -1;
    
    while ((iSelectedItem = ListView_GetNextItem(m_hwnd, iSelectedItem, LVNI_SELECTED)) != -1)
    {
        // If the item is in a group then remove it from the group.
        CObject *poCur = GetObjectFromIndex(iSelectedItem);
        CObj_Group *pog = poCur->m_pog;
        if (pog)
        {
            // Tell the group to remove all it's grouped objects
            pog->RemoveAllGroupedObjects();

            // Tell the workspace to remove the group
            pws->RemoveObjectFromLayer(pog, nLayer);
        }
    }
    
    m_fModified = true;

    // We know we can't ungroup now
    m_pwindow->SetUngroupable(false);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetFirstSelectedObject
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CObject *CListView::GetFirstSelectedObject()
{
    return GetObjectFromIndex(ListView_GetNextItem(m_hwnd, -1, LVNI_SELECTED));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::GetLastSelectedObject
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CObject *CListView::GetLastSelectedObject()
{
    int iItem = -1;
    CObject *pobj = NULL;
    while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
    {
        pobj = GetObjectFromIndex(iItem);
    }
    return pobj;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::StoreSelectedObjList
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::StoreSelectedObjList()
{
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_hwnd, iItem, LVNI_SELECTED)) != -1)
    {
        GetObjectFromIndex(iItem)->m_fWasSelected = true;
    }

    // Also store scroll position
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
    GetScrollInfo(m_hwnd, SB_VERT, &si);
    m_iScrollPos = si.nPos;

    m_fSelectedObjList = true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::ClearSelectedObjList
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::ClearSelectedObjList()
{
    int n = m_iScrollPos;

    ListView_Scroll(m_hwnd, 0, n * GetColumnHeight());

    m_fSelectedObjList = false;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ListViewCompareProc
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    return ((CListView*)lParamSort)->CompareProc(lParam1, lParam2);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::SortByColumn
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CListView::Sort()
{
    // a little bit of hackery here to get around the fact that sorting works
    // through a callback function...
    ListView_SortItems(m_hwnd, ListViewCompareProc, this);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::CompareProc
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CListView::CompareProc(LPARAM lParam1, LPARAM lParam2)
{
    CObject *pobj1 = (CObject*)lParam1;
    CObject *pobj2 = (CObject*)lParam2;

    // Sort by LSN
    if (pobj1->m_dwLSN == pobj2->m_dwLSN)
        return 0;
    else
        return  (pobj1->m_dwLSN > pobj2->m_dwLSN) ? 1 : -1;
}	

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CListView::MoveSelection
// Purpose:   
// Arguments: 
// Return:    'true' if we handled the move; 'false' if the os should
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CListView::MoveSelection(WORD wVKey)
{
    // Get the currently focused item
    int iItem = ListView_GetNextItem(m_hwnd, -1, LVNI_FOCUSED);
    if (iItem == -1)
        return false;

    // Determine which entry the user is trying to move towards
    int iTargetItem = iItem;
    switch (wVKey)
    {
    case VK_UP:    iTargetItem--; break;
    case VK_LEFT:  iTargetItem--; break;
    case VK_DOWN:  iTargetItem++; break;
    case VK_RIGHT: iTargetItem++; break;
    case VK_HOME:   // FUTURE: Move to topmost valid item
    case VK_END:    // FUTURE: move to bottommost valid item
    case VK_PRIOR:  // FUTURE:
    case VK_NEXT:   // FUTURE:
                   return false;
    default:       return true;
    }

    // Is the target item one that the user can move onto?
    if (!CanItemChange(iTargetItem, 0, 0))
    {
        // Item can't change, so pretend we already handled it.
        // FUTURE: Step over target item to next 'valid' one
        return true;
    }
    else
    {
        // Item can change - let os handle it
        return false;
    }   
}

