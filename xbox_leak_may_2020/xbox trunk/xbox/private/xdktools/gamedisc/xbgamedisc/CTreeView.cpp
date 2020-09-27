// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      TreeView.cpp
// Contents:  
// Revisions: 15-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

// Hack: Done to allow TreeViewProc to call into the actual CTreeView object.
//       I know there's only one in this app, so I don't bother with storing
//       the pointer in GWL_USERDATA (etc).
static CTreeView *g_ptreeview = NULL;
static WNDPROC gs_treeviewproc;
static WNDPROC gs_staticproc;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK TreeViewProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
//    TrackMessage(uMsg, wparam, lparam, "TreeView");
    switch(uMsg)
    {
    case WM_SETFOCUS:
         g_hwndPrevFocus = hwnd;
         break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        // We don't like the default treeview checkbox handling, so we
        // explicitly handle it ourselves.  If the user presses the mouse
        // over a checkbox, we toggle its state.
        if (g_ptreeview->HandleMouseDown(LOWORD(lparam), HIWORD(lparam)))
        {
            // Return zero to indicate that we handled the message
            return 0;
        }

        break;
    }

    // Pass all other messages to the base window procedure.
    return CallWindowProc(gs_treeviewproc, hwnd, uMsg, wparam, lparam);
}
HFONT m_hfont;
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK StaticProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rc;
    
    switch(uMsg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SelectObject(hdc, m_hfont);
        ExtTextOut(hdc, 2, 2, ETO_CLIPPED, &rc, "Folders", 7, NULL);
        rc.top = rc.bottom-1;
        FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNSHADOW));
        EndPaint(hwnd, &ps);
        break;
    }

    // Pass all other messages to the base window procedure.
    return CallWindowProc(gs_staticproc, hwnd, uMsg, wparam, lparam);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::CTreeView
// Purpose:   
// Arguments: 
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CTreeView::CTreeView()
{
    m_himl = NULL;
    m_hwnd = NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::Resize
// Purpose:   
// Arguments: 
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CTreeView::Resize(RECT *prc)
{
    int nH = 20;
    MoveWindow(m_hwndStatic, prc->left, prc->top, prc->right - prc->left,
               nH, TRUE);
    MoveWindow(m_hwnd, prc->left, prc->top + nH, prc->right - prc->left,
               prc->bottom - prc->top-nH, TRUE);
    UpdateWindow(m_hwnd);
    UpdateWindow(m_hwndStatic);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::Init
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CTreeView::Init(HWND hwndParent)
{
    // Create the Win32 treeview control.
    m_hwndStatic = CreateWindow("static", "Folders", WS_VISIBLE | WS_CHILD, 
                            0, 0, 0, 0, hwndParent, (HMENU) WNDID_TREEVIEW_STATIC, g_hInst, NULL); 
    assert(gs_staticproc == NULL); // Only allow one instance for now.
    gs_staticproc = (WNDPROC) SetWindowLong(m_hwndStatic, GWL_WNDPROC,
                                             (LONG)StaticProc);
    // Create the GDI font object
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -11;
    lf.lfWeight = 0;
    strcpy(lf.lfFaceName, "Tahoma");
    m_hfont = CreateFontIndirect(&lf);
    
    m_hwnd = CreateWindowEx(0, WC_TREEVIEW, "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
                            TVS_HASLINES | TVS_HASBUTTONS |
                            TVS_LINESATROOT |TVS_SHOWSELALWAYS, 
                            0, 0, 0, 0, hwndParent, (HMENU) WNDID_TREEVIEW, g_hInst, NULL); 

    // Overload the default window procedure so that we can apply our own
    // mouse button handling.
    assert(gs_treeviewproc == NULL); // Only allow one instance for now.
    gs_treeviewproc = (WNDPROC) SetWindowLong(m_hwnd, GWL_WNDPROC,
                                             (LONG)TreeViewProc);
    g_ptreeview = this;
    
    // Initialize the image list for the treeview item icons.
    if (!InitImageList())
        return FALSE; 

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::Uninit
// Purpose:   
// Arguments: 
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CTreeView::Uninit()
{
    if (m_himl)
        ImageList_Destroy(m_himl);
    if (m_hwnd)
        DestroyWindow(m_hwnd);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::InitImageList
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CTreeView::InitImageList() 
{ 
    HBITMAP hbmp;

    // Create the image list. 
    m_himl = ImageList_Create(16, 16, ILC_COLOR8, 1, 0);
    if (m_himl == NULL) 
        return FALSE; 

    hbmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TREEVIEW)); 
    if (hbmp == NULL)
        return FALSE;

    int nImageIndex = ImageList_Add(m_himl, hbmp, (HBITMAP) NULL);
    DeleteObject(hbmp); 

    if (nImageIndex == -1)
        return FALSE;

    // Associate the image list with the tree-view control. 
    TreeView_SetImageList(m_hwnd, m_himl, TVSIL_NORMAL); 
    TreeView_SetImageList(m_hwnd, m_himl, TVSIL_STATE); 

    return TRUE; 
} 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::AddItem
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HTREEITEM CTreeView::AddItem(CFileObject *pfo, int nLevel, HTREEITEM htiParent)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
 
    tvi.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE;
 
    if (nLevel == 1)
    {
        tvi.iImage = IMAGE_XROOT; 
        tvi.iSelectedImage = IMAGE_XROOT; 
        tvi.pszText = "Game Disc Root"; 
    }
    else
    {
        tvi.iImage = IMAGE_FOLDERCLOSED; 
        tvi.iSelectedImage = IMAGE_FOLDEROPEN; 
        tvi.pszText = pfo->m_szName; 
    }
    tvi.cchTextMax = lstrlen(tvi.pszText); 
 
    // Save the heading level in the item's application-defined 
    // data area. 
    tvi.lParam = (LPARAM) pfo; 
 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 
 
    // Set the parent item based on the specified level. 
    tvins.hParent = htiParent; 
 
    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM) SendMessage(m_hwnd, TVM_INSERTITEM, 0, 
                                    (LPARAM) (LPTVINSERTSTRUCT) &tvins); 

    return hPrev; 
} 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::SetCheckState
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CTreeView::SetCheckState(HTREEITEM hItem, eCheckState checkstate)
{
    TVITEM tvItem;

    ((CFileObject*)GetTVItemLPARAM(hItem))->m_checkstate = checkstate;
    
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;

    switch (checkstate) {
    case CHECKSTATE_CHECKED:
        tvItem.state = INDEXTOSTATEIMAGEMASK(IMAGE_CHECK);
        break;
    case CHECKSTATE_UNCHECKED:
        tvItem.state = INDEXTOSTATEIMAGEMASK(IMAGE_UNCHECK);
        break;
    case CHECKSTATE_TRISTATE:
        tvItem.state = INDEXTOSTATEIMAGEMASK(IMAGE_TRISTATE);
        break;
    }

    return TreeView_SetItem(m_hwnd, &tvItem);
}

BOOL CTreeView::RecurseSetCheckState(HTREEITEM hItem, eCheckState checkstate)
{
    HTREEITEM htiParent = TreeView_GetParent(m_hwnd, hItem);

    if (htiParent)
        if (!SetCheckState(hItem, checkstate))
            return FALSE;

    // Now, for every child of htiCur, set its state to match htiCur
    if (checkstate != CHECKSTATE_TRISTATE)
        RecurseDownSetState(hItem, checkstate);
        
    // now, for every parent of htiCur, enumerate its children and set state appropriately
    // This is necessary since the state may be the greyed out 'tristate'.
    if (htiParent)
        RecurseUpSetState(htiParent);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::GetCheckState
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
eCheckState CTreeView::GetCheckState(HTREEITEM hItem)
{
    TVITEM tvItem;

    // Prepare to receive the desired information.
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;

    // Request the information.
    TreeView_GetItem(m_hwnd, &tvItem);

    // Return zero if it's not checked, or nonzero otherwise.
    switch (tvItem.state >> 12) {
    case IMAGE_CHECK:    return CHECKSTATE_CHECKED;
    case IMAGE_UNCHECK:  return CHECKSTATE_UNCHECKED;
    case IMAGE_TRISTATE: return CHECKSTATE_TRISTATE;
    }
    return CHECKSTATE_UNDEFINED;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::ToggleCheckState
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CTreeView::ToggleCheckState(HTREEITEM htiCur)
{
    eCheckState checkstate = GetCheckState(htiCur);
    if (checkstate == CHECKSTATE_UNCHECKED)
        checkstate = CHECKSTATE_CHECKED;
    else
        checkstate = CHECKSTATE_UNCHECKED;
    
    if (!SetCheckState(htiCur, checkstate))
        return;

    // Now, for every child of htiCur, set its state to match htiCur
    RecurseDownSetState(htiCur, checkstate);
        
    // now, for every parent of htiCur, enumerate its children and set state appropriately
    // This is necessary since the state may be the greyed out 'tristate'.
    RecurseUpSetState(TreeView_GetParent(m_hwnd, htiCur));
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::GetTVItemLPARAM
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
INT CTreeView::GetTVItemLPARAM(HTREEITEM hti)
{
    TVITEM tvi;
    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hti;
    TreeView_GetItem(m_hwnd, &tvi);

    return tvi.lParam;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::RecurseDownSetState
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CTreeView::RecurseDownSetState(HTREEITEM htiCur, eCheckState checkstate)
{
    HTREEITEM htiChild = TreeView_GetChild(m_hwnd, htiCur);
    while (htiChild)
    {
        SetCheckState(htiChild, checkstate);
        RecurseDownSetState(htiChild, checkstate);

        htiChild = TreeView_GetNextSibling(m_hwnd, htiChild);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::RecurseUpSetState
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CTreeView::RecurseUpSetState(HTREEITEM htiCur)
{
    // Determine the state of the current item based on the state of all
    // its child items.
    // if ALL == unchecked, this == unchecked
    // if ALL == checked, this == checked
    // else this == tristate

    // Stop if this is the root
    CFileObject *pfo = (CFileObject*)GetTVItemLPARAM(htiCur);
    assert(pfo);
    if (pfo->m_nDepth == 1)
        return;

    // Get the check state of the first child.
    CFileObject *pfoChild = pfo->m_lpfoChildren.GetFirst();
    assert(pfoChild);
    eCheckState csAggregate = pfoChild->m_checkstate;
    while ((pfoChild = pfo->m_lpfoChildren.GetNext()) != NULL)
    {
        eCheckState csChild = pfoChild->m_checkstate;
        if (csChild != csAggregate)
        {
            // Our children have different states; we're thus tristate
            csAggregate = CHECKSTATE_TRISTATE;
            break;
        } 
    }

    SetCheckState(htiCur, csAggregate);
    RecurseUpSetState(TreeView_GetParent(m_hwnd, htiCur));
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::HandleMouseDown
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CTreeView::HandleMouseDown(int nX, int nY)
{
    // Find the item (if any) the cursor is over
    RECT rcItem;
    HTREEITEM htiCur = TreeView_GetFirstVisible(m_hwnd);
    POINT pt = {nX, nY};
    int nIndent = TreeView_GetIndent(m_hwnd);

    while (htiCur)
    {
        // Is the mouse over the line containing the current item?
        TreeView_GetItemRect(m_hwnd, htiCur, &rcItem, FALSE);
        if (PtInRect(&rcItem, pt) && rcItem.top != 0)
        {
            // Mouse is over the line.  Is it over the checkbox?  Get the
            // indentation level of the item.
            rcItem.left = nIndent * ((CFileObject*)GetTVItemLPARAM(htiCur))->m_nDepth;
            rcItem.right = rcItem.left + 13;

            if (PtInRect(&rcItem, pt))
            {
                // Yep, it's over the checkbox -- Toggle it
                ToggleCheckState(htiCur);

                g_vmExplorer.m_listview.SetChildrenCheckState((CFileObject*)GetTVItemLPARAM(htiCur));
                InvalidateRect(g_vmExplorer.m_listview.m_hwnd, NULL, FALSE);
                
                SetModified();

                // return that we handled the message
                return TRUE;
            }

            // Mouse isn't over the checkbox -- let the OS handle the msg
            return FALSE;
        }
        htiCur = TreeView_GetNextVisible(m_hwnd, htiCur);
    }

    // Mouse wasn't over any item
    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CTreeView::Populate
// Purpose:   Adds pfiCur and (recursively) each of its subfolders.
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CTreeView::Populate(CFileObject *pfoCur)
{
    HTREEITEM htiParent;

    assert (pfoCur != NULL);
    assert (pfoCur->m_fIsDir);

    // If pfiCur doesn't have a parent, then it's the root node.
    if (pfoCur->m_pfoParent == NULL)
        htiParent = TVI_ROOT;
    else
        htiParent = pfoCur->m_pfoParent->m_hti;

    // Add pfoCur to the treeview and set its default check state.
    pfoCur->m_hti = AddItem(pfoCur, pfoCur->m_nDepth, htiParent);
    if (pfoCur->m_nDepth > 1)
        SetCheckState(pfoCur->m_hti, pfoCur->m_checkstate);

    // Then recurse into each of its child folders and add them.
    CFileObject *pfoChild = pfoCur->m_lpfoChildren.GetFirst();
    while (pfoChild)
    {
        if (pfoChild->m_fIsDir)
            if (!Populate(pfoChild))
                return FALSE;
        pfoChild = pfoCur->m_lpfoChildren.GetNext();
    }    

    return TRUE;
}

void CTreeView::HandleSelection(LPNMTREEVIEW pnmhdr)
{
    TVITEM item;
    item.hItem = pnmhdr->itemNew.hItem;
    item.mask = TVIF_PARAM;
    TreeView_GetItem(m_hwnd, &item);

    g_vmExplorer.m_listview.SetDir((CFileObject*)item.lParam);
}

void CTreeView::SetCurFolder(CFileObject *pfo)
{
    TreeView_SelectItem(m_hwnd, pfo->m_hti);
    TreeView_Expand(m_hwnd, pfo->m_hti, TVE_EXPAND);
}

void CTreeView::AddObject(CFileObject *pfo)
{
    pfo->m_hti = AddItem(pfo, pfo->m_pfoParent->m_nDepth + 1, pfo->m_pfoParent->m_hti);
    pfo->m_nDepth = pfo->m_pfoParent->m_nDepth + 1;
    SetCheckState(pfo->m_hti, pfo->m_checkstate);
}

void CTreeView::RemoveObject(CFileObject *pfo)
{
    TreeView_DeleteItem(m_hwnd, pfo->m_hti);
}

void CTreeView::Clear()
{
    TreeView_DeleteAllItems(m_hwnd);
    InvalidateRect(m_hwnd, NULL, TRUE);
    UpdateWindow(m_hwnd);
}