// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CExpListView.cpp
// Contents:  
// Revisions: 6-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ENUMERATIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// The columns visible in the Explorer View's listview control.
enum {COL_INCLUDED = 0, COL_NAME, COL_SIZE, COL_TYPE, COL_MODIFIED,
      COL_LOCATION};


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::SetDir
// Purpose:   Populate the list view with the contents of specified folder
// Arguments: pfoFolder     -- The folder with which to populate the list
//                             view.
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CExpListView::SetDir(CFileObject *pfoFolder)
{
    assert(pfoFolder);
    assert(this);

    // Erase contents from previous folder (if any)
    Clear();

    // Enumerate over the folder's child objects and add each of them.
    CFileObject *pfoChild = pfoFolder->m_lpfoChildren.GetFirst();
    while (pfoChild)
    {
        AddItemAtIndex(pfoChild, 0);
        pfoChild = pfoFolder->m_lpfoChildren.GetNext();
    }

    // Track which folder we're now viewing.
    m_pfoDisplay = pfoFolder;

    // Not over any mouse item at start
    m_iMouseOverItem = -1;

    // Mark that we're dirty to force refresh
    m_fDirty = TRUE;

    // Update status bar
    char sz[100];
    int nObjects = pfoFolder->m_lpfoChildren.GetCount();
    sprintf(sz, "%d objects", nObjects);
    g_statusbar.SetText(0, sz);

    FormatByteString(pfoFolder->m_lpfoChildren.GetSize(), sz);
    g_statusbar.SetText(1, sz);

    // Start out with none selected
    m_nSelected = 0;
    m_uliSelectedSize.QuadPart = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::Init
// Purpose:   Initialize the list view control; create columns and font.
// Arguments: hwndParent        -- The window we're attache to.
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CExpListView::Init(HWND hwndParent)
{
    SHFILEINFO sfi;
    LOGFONT    lf;

    assert(hwndParent);

    // Initialize our base 'MyListView' object.
    if (!CMyListView::Init(hwndParent))
        return FALSE;

    // Get the imagelist of icons for the file display
    ZeroMemory(&sfi, sizeof sfi);
    DWORD flg = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX;
    m_himlFileIcons = (HIMAGELIST)SHGetFileInfo("C:\\", 0, &sfi, sizeof(sfi),
                                                flg);

    // Preload bitmap images for the various checkbox states.
    m_hbmpUnchecked    = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_UNCHECK));
    m_hbmpChecked      = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHECK));
    m_hbmpUncheckedLit = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_UNCHECKLIT));
    m_hbmpCheckedLit   = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHECKLIT));

    // Init columns for listview
    // NOTE: Order must match order in COL_* enumeration
    AddColumn("Included",      60);
    AddColumn("Name",          150);
    AddColumn("Size",          75);
    AddColumn("Type",          75);
    AddColumn("Date Modified", 100);
    AddColumn("Location",      75);

    // Create the GDI font object that will be used to render the list view
    // elements.
    memset(&lf, 0, sizeof lf);
    lf.lfHeight = -11;
    lf.lfWeight = 0;
    strcpy(lf.lfFaceName, "Tahoma");
    m_hfont = CreateFontIndirect(&lf);

    // Default to Sorting by the 'Name' column
    m_iSortColumn = COL_NAME;

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::UpdateItem
// Purpose:   Update the display of the specified item.
// Arguments: pvItem      -- Data for the row to update
//            dwItemState -- Selected or focused
//            iColumn     -- The specific column to update
//            hdcDest     -- Device Context into which to render.
//            prcDest     -- Destination rect (clipped)
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CExpListView::UpdateItem(void *pvItem, DWORD dwItemState, int iColumn,
                              HDC hdcDest, RECT *prcDest)
{
    char *psz;
    HDC hdcBmp;
    HBITMAP hbmpPrev;
    int nX, nY, nW;
    CFileObject *pfo = (CFileObject*)pvItem;
    DWORD dwAlign = DT_LEFT;
    DWORD dwFlags;
    
    static HBRUSH s_hbrLightGray = CreateSolidBrush(RGB(247,247,247));

    assert(pvItem);
    assert(hdcDest);
    assert(iColumn < NUM_COLUMNS);

    // If prcDest is NULL, then the item is completely offscreen
    if (prcDest == NULL)
        return;

    // UNDONE: Can choose to use offscreenbuffer only when resizing if so
    //         desired (not really necessary for scroll/update).
#define OFFSCREENBUFFER
#ifdef OFFSCREENBUFFER
    static HBITMAP s_hbmpOff = NULL;
    static int s_nLastWidth = -1, s_nLastHeight = -1;
    RECT rcOrig = *prcDest;
    prcDest->bottom -= prcDest->top;
    prcDest->top = 0;
    prcDest->right -= prcDest->left;
    prcDest->left = 0;
    
    HDC hdcOffscreen = CreateCompatibleDC(hdcDest);

    // Resuse the offscreen bitmap from the previuos call unless it needs to
    // grow.
    if (prcDest->right - prcDest->left > s_nLastWidth ||
        prcDest->bottom - prcDest->top > s_nLastHeight)
    {
        if (s_hbmpOff)
            DeleteObject(s_hbmpOff);
        s_hbmpOff = CreateCompatibleBitmap(hdcDest,
                                           prcDest->right - prcDest->left,
                                           prcDest->bottom - prcDest->top);
        s_nLastWidth = prcDest->right - prcDest->left;
        s_nLastHeight = prcDest->bottom - prcDest->top;
    }

    HBITMAP hbmpPrevOff = (HBITMAP)SelectObject(hdcOffscreen, s_hbmpOff);
#else
    HDC hdcOffscreen = hdcDest;
#endif
    SelectObject(hdcOffscreen, m_hfont);

    if (dwItemState & LVF_ITEMSELECTED)
    {
        // Draw selected state background
        if (iColumn == 1)
        {
            if (dwItemState & LVF_ITEMFOCUSED)
            {
                FillRect(hdcOffscreen, prcDest,
                         GetSysColorBrush(COLOR_BACKGROUND));
                SetBkColor(hdcOffscreen, GetSysColor(COLOR_BACKGROUND));
                SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOW));
            }
            else
            {
                FillRect(hdcOffscreen, prcDest,
                         GetSysColorBrush(COLOR_BTNSHADOW));
                SetBkColor(hdcOffscreen, GetSysColor(COLOR_BTNSHADOW));
                SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOWTEXT));
            }
        }
        else
        {
            if (dwItemState & LVF_ITEMFOCUSED)
            {
                FillRect(hdcOffscreen, prcDest,
                         GetSysColorBrush(COLOR_HIGHLIGHT));
                SetBkColor(hdcOffscreen, GetSysColor(COLOR_HIGHLIGHT));
                SetTextColor(hdcOffscreen, GetSysColor(COLOR_HIGHLIGHTTEXT));
            }
            else
            {
                FillRect(hdcOffscreen, prcDest,
                         GetSysColorBrush(COLOR_INACTIVEBORDER));
                SetBkColor(hdcOffscreen, GetSysColor(COLOR_INACTIVEBORDER));
                SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOWTEXT));
            }
        }
    }
    else
    {
        // Draw unselected state background
        if (iColumn == 1)
        {
            FillRect(hdcOffscreen, prcDest, s_hbrLightGray);
            SetBkColor(hdcOffscreen, RGB(247,247,247));
        }
        else
        {
            FillRect(hdcOffscreen, prcDest, GetSysColorBrush(COLOR_WINDOW));
            SetBkColor(hdcOffscreen, GetSysColor(COLOR_WINDOW));
        }
        SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOWTEXT));
    }

    // Column 0 contains the checkbox and is centered - don't offset it.
    if (iColumn != 0)
    {
        prcDest->right -= 5;
        prcDest->top ++;
    }

    // Render the requested column.
    switch(iColumn)
    {
    case COL_INCLUDED:
        // Draw included state (centered in the first column)
        hdcBmp = CreateCompatibleDC(hdcOffscreen);
        hbmpPrev;
        if (pfo->m_checkstate == CHECKSTATE_CHECKED)
            if (pfo->m_fMouseOver)
                hbmpPrev = (HBITMAP)SelectObject(hdcBmp, m_hbmpCheckedLit);
            else
                hbmpPrev = (HBITMAP)SelectObject(hdcBmp, m_hbmpChecked);
        else
            if (pfo->m_fMouseOver)
                hbmpPrev = (HBITMAP)SelectObject(hdcBmp, m_hbmpUncheckedLit);
            else
                hbmpPrev = (HBITMAP)SelectObject(hdcBmp, m_hbmpUnchecked);

        nW = min(prcDest->right - prcDest->left, CB_SIZE);
        if (nW < CB_SIZE)
            nX = prcDest->left-2;
        else
            nX = prcDest->left + (prcDest->right - prcDest->left) / 2 - 7;
        nY = prcDest->top + (prcDest->bottom - prcDest->top) / 2 - 6;
        BitBlt(hdcOffscreen, nX, nY, nW, CB_SIZE, hdcBmp, CB_SIZE-nW, 0,
               SRCCOPY);
        SelectObject(hdcBmp, hbmpPrev);
        DeleteDC(hdcBmp);
        psz = NULL;
        break;

    case COL_NAME:
        // Render the icon
        nW = min(GetSystemMetrics(SM_CXSMICON),
                 prcDest->right - prcDest->left);
        dwFlags = ILD_TRANSPARENT | ILD_NORMAL;
        if ((dwItemState & LVF_ITEMSELECTED) &&
            (dwItemState & LVF_ITEMFOCUSED))
            dwFlags |= ILD_SELECTED;
        ImageList_Draw(pfo->m_himl, pfo->m_iIcon, hdcOffscreen,
                       prcDest->left, prcDest->top, dwFlags);

        prcDest->left += 20;
        psz = pfo->m_szName;
        break;

    case COL_SIZE:
        psz     = pfo->m_szSize;
        dwAlign = DT_RIGHT;
        break;

    case COL_TYPE:
        psz = pfo->m_szType;
        break;

    case COL_MODIFIED:
        psz = pfo->m_szModified;
        break;
    
    case COL_LOCATION:
        // UNDONE: change m_szLocation to an index into g_rgszLoc.
        psz = pfo->m_szLocation;
        break;

    default:
        assert(FALSE);
    }
    // Render the text string (if one).
    if (psz)
        DrawTextEx(hdcOffscreen, psz, strlen(psz), prcDest,
                   dwAlign | DT_END_ELLIPSIS, NULL);

#ifdef OFFSCREENBUFFER
    // Render offscreen buffer to the final destination DC.
    BitBlt(hdcDest, rcOrig.left, rcOrig.top,
           rcOrig.right - rcOrig.left, rcOrig.bottom - rcOrig.top,
           hdcOffscreen, 0, 0, SRCCOPY);
    SelectObject(hdcOffscreen, hbmpPrevOff);
    DeleteDC(hdcOffscreen);
#endif
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::GetItemWidth
// Purpose:   Obtain the Width (in pixels) of the specified column's text.
// Arguments: hdc       -- Device Context for the item
//            pvItem    -- Item data
//            iColumn   -- Column to query on.
// Return:    Width (in pixels) of the text in the specified column
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD CExpListView::GetItemWidth(HDC hdc, void *pvItem, int iColumn)
{
    CFileObject *pfo = (CFileObject*)pvItem;
    SIZE size;
    char *psz;
    int  nAdd = 5; // 5 for the padding we add during drawing

    assert(hdc);
    assert(pvItem);
    assert(iColumn < NUM_COLUMNS);

    switch(iColumn)
    {
    case COL_INCLUDED: return CB_SIZE+4;
    case COL_NAME:     psz = pfo->m_szName;     nAdd += 20; break;
    case COL_SIZE:     psz = pfo->m_szSize;     break;
    case COL_TYPE:     psz = pfo->m_szType;     break;
    case COL_MODIFIED: psz = pfo->m_szModified; break;
    case COL_LOCATION: psz = pfo->m_szLocation; break;
    }

    SelectObject(hdc, m_hfont);
    GetTextExtentPoint32(hdc, psz, strlen(psz), &size);
    SetTextJustification(hdc, 0, 0);
    return size.cx + nAdd;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::CompareObjects
// Purpose:   Compare to objects and return their 'sorting order'
// Arguments: pv1       -- First object to compare
//            pv2       -- Second object to compare
// Return:    return -1 if pv1 is "less than" pv2
//            return 1 if pv2 is "less than" pv1
//            return 0 if pv1 "equals" pv2
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CExpListView::CompareObjects(const void *pv1, const void *pv2)
{
    int nRet;

    assert(pv1);
    assert(pv2);
    
    CFileObject *po1 = (CFileObject*)pv1;
    CFileObject *po2 = (CFileObject*)pv2;

    // If one of the objects is a directory and the other isn't, then the
    // directory always comes first
    BOOL f1 = po1->m_dwa & FILE_ATTRIBUTE_DIRECTORY ? TRUE : FALSE;
    BOOL f2 = po2->m_dwa & FILE_ATTRIBUTE_DIRECTORY ? TRUE : FALSE;
    if (f1 && !f2)
        return -1;
    if (f2 && !f1)
        return 1;

    // Sort pv1 and pv2 based on the contents of the current sort column
    switch(m_iSortColumn)
    {
    case COL_INCLUDED:
        if (po1->m_checkstate == po2->m_checkstate)
            return _stricmp(po1->m_szName, po2->m_szName);
        return (po1->m_checkstate == CHECKSTATE_CHECKED) ? -1 : 1;

    case COL_NAME:
        return _stricmp(po1->m_szName, po2->m_szName);

    case COL_SIZE:
        if (po1->m_uliSize.QuadPart == po2->m_uliSize.QuadPart)
            return _stricmp(po1->m_szName, po2->m_szName);
        return (po1->m_uliSize.QuadPart < po2->m_uliSize.QuadPart) ? -1 : 1;

    case COL_TYPE:
        nRet = _stricmp(po1->m_szType, po2->m_szType);
        if (nRet == 0)
            nRet = _stricmp(po1->m_szName, po2->m_szName);
        return nRet;

    case COL_MODIFIED:
        nRet = CompareFileTime(&po1->m_ftModified, &po2->m_ftModified);
        if (nRet == 0)
            nRet = _stricmp(po1->m_szName, po2->m_szName);
        return nRet;

    case COL_LOCATION:
        nRet = _stricmp(po1->m_szLocation, po2->m_szLocation);
        if (nRet == 0)
            nRet = _stricmp(po1->m_szName, po2->m_szName);
        return nRet;

    default:
        assert(false);
    }

    // Should never get here.
    return 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::Override_LButtonDown
// Purpose:   Check if the user is over the 'include' checkbox when they
//            click the mouse.
// Arguments: pmi       -- Contains mouse state information.
//            dwFlags   -- Additional key state (Shift, Ctrl).
// Return:    'FALSE' if we've handled the message.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CExpListView::Override_LButtonDown(sMouseInfo *pmi, DWORD dwFlags)
{
    assert(pmi);

    RECT rcCheckBox;
    POINT pt = {pmi->nX, pmi->nY};

    // Only need to handle button down over include column
    if (pmi->iColumn != COL_INCLUDED)
        return TRUE;
    
    // User clicked on included column.  Are they in the checkbox?
    // Checkbox (12x12) is centered in the item
    rcCheckBox.left = pmi->rcSubItem.left + (pmi->rcSubItem.right -
                                         pmi->rcSubItem.left - CB_SIZE) / 2;
    rcCheckBox.right = rcCheckBox.left + CB_SIZE;
    rcCheckBox.top = pmi->rcSubItem.top + (pmi->rcSubItem.bottom -
                                        pmi->rcSubItem.top - CB_SIZE) / 2+1;
    rcCheckBox.bottom = rcCheckBox.top + CB_SIZE;
    if (!PtInRect(&rcCheckBox, pt))
        return TRUE;

    // If here, we're in the checkbox!
    CFileObject *po = (CFileObject*)pmi->pvItem;
    assert(po);
    
    // Toggle the object's included state.
    po->m_checkstate = (po->m_checkstate == CHECKSTATE_CHECKED) ?
                                  CHECKSTATE_UNCHECKED : CHECKSTATE_CHECKED;
    
    // If the object is a folder, then update its child objects to match it
    if (po->m_fIsDir)
    {
        // UNDONE: The recursive checkstate code works fine, but is not
        //         pretty.  Given time, remove it from treeview and place in
        //         ViewMode_Explorer of CFileHierarchy and clean it up
        //         considerably.

        // Treeview only sets check state of folders, not files, so force them
        // all here.
        SetChildrenCheckState(po);

        // Update the object itself
        g_vmExplorer.m_treeview.SetCheckState(po->m_hti, po->m_checkstate);

        // Update all of its child objects
        g_vmExplorer.m_treeview.RecurseDownSetState(po->m_hti,
                                                    po->m_checkstate);
    }

    // Finally, select the clicked item
    if (dwFlags & (MK_SHIFT | MK_CONTROL))
        SelectItem(pmi->iRow, dwFlags | MK_FORCESEL);
    else
    {
        if (!m_rgli[pmi->iRow].dwFlags & LVF_ITEMSELECTED)
            UnselectAllItems();
        SelectSingleItem(pmi->iRow);
    }
    SetFocusItem(pmi->iRow);
 
    // Update ALL selected items to match the same state
    for (int i = 0; i < m_cItems; i++)
        if (m_rgli[i].dwFlags & LVF_ITEMSELECTED)
        {
            CFileObject *po2 = (CFileObject*)(m_rgli[i].pvData);
            po2->m_checkstate = po->m_checkstate;
            if (po2->m_fIsDir)
            {
                SetChildrenCheckState(po2);
                g_vmExplorer.m_treeview.SetCheckState(po2->m_hti,
                                                      po2->m_checkstate);
                g_vmExplorer.m_treeview.RecurseDownSetState(po2->m_hti,
                                                         po2->m_checkstate);
            }
            RefreshItem(i);
        }

    // Propagate the check state information to the treeview.
    UpdateCheckState();

    // Mark that the layout has been modified.
    SetModified();

    // Return that we've handled the message.
    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::Override_LButtonDblClick
// Purpose:   If user double clicks on a folder, then set that as the current
//            displayed folder.
// Arguments: pmi       -- Contains mouse state information.
//            dwFlags   -- Additional key state (Shift, Ctrl).
// Return:    'FALSE' if we've handled the message.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CExpListView::Override_LButtonDblClick(sMouseInfo *pmi, DWORD dwFlags)
{
    assert(pmi);

    // We only care if the user double clicked on the name column
    if (pmi->iColumn != COL_NAME)
        return TRUE;

    // If the item is a folder, then set it as the current folder
    CFileObject *pfo = (CFileObject*)pmi->pvItem;
    if (pfo->m_fIsDir)
    {
        SetDir(pfo);
        g_vmExplorer.m_treeview.SetCurFolder(pfo);
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
    
    // Return that we've handled the message.
    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::Override_MouseMove
// Purpose:   If user moves the mouse over a checkbox, then hilight it.
// Arguments: pmi       -- Contains mouse state information.
//            dwFlags   -- Additional key state (Shift, Ctrl).
// Return:    'FALSE' if we've handled the message.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CExpListView::Override_MouseMove(sMouseInfo *pmi, DWORD dwFlags)
{
    assert(pmi);

    RECT rcCheckBox;
    POINT pt = {pmi->nX, pmi->nY};

    // We only care about the 'included' column
    if (pmi->iColumn != COL_INCLUDED)
        return TRUE;

    CFileObject *po = (CFileObject*)pmi->pvItem;
    assert(po);
 
    // Mouse is in column 0.  Are they in the checkbox?
    // Checkbox (12x12) is centered in the item
    rcCheckBox.left = pmi->rcSubItem.left + (pmi->rcSubItem.right -
                                         pmi->rcSubItem.left - CB_SIZE) / 2;
    rcCheckBox.right = rcCheckBox.left + CB_SIZE;
    rcCheckBox.top = pmi->rcSubItem.top + (pmi->rcSubItem.bottom -
                                        pmi->rcSubItem.top - CB_SIZE) / 2+1;
    rcCheckBox.bottom = rcCheckBox.top + CB_SIZE;
    if (!PtInRect(&rcCheckBox, pt))
    {
        // We're not in a checkbox
        if (m_iMouseOverItem != -1)
        {
            // We left a checkbox during the mouse move
            assert(m_poMouseOver);
            m_poMouseOver->m_fMouseOver = FALSE;
            RefreshItem(m_iMouseOverItem);
            m_iMouseOverItem = -1;
        }
        return TRUE;
    }
    
    if (m_iMouseOverItem != pmi->iRow && m_iMouseOverItem != -1)
    {
        // We scrolled out of a checkbox, and into a new one.  Unhighlight
        // the previously lit checkbox.
        assert(m_poMouseOver);
        m_poMouseOver->m_fMouseOver = FALSE;
        RefreshItem(m_iMouseOverItem);
    }

    // We're in a checkbox - highlight it.
    po->m_fMouseOver = TRUE;
    m_iMouseOverItem = pmi->iRow;
    m_poMouseOver = po;
    RefreshItem(m_iMouseOverItem);

    // Return that we handled the message
    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::AddObject
// Purpose:   Adds a new object to the start of the listview.
// Arguments: pvObj     -- The object to Add
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CExpListView::AddObject(void *pvObj)
{
    assert(pvObj);

    CFileObject *pfo = (CFileObject *)pvObj;

    // Only add the object if it's a child of the current folder we're
    // displaying.
    if (pfo->m_pfoParent == m_pfoDisplay)
        AddItemAtIndex(pvObj, 0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::RemoveObject
// Purpose:   Removes an object from the listview.
// Arguments: pvObj     -- The object to Remove
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CExpListView::RemoveObject(void *pvObj)
{
    assert(pvObj);

    CFileObject *pfo = (CFileObject*)pvObj;

    // If the user deleted the folder we were currently in, then move to the
    // parent object.
    if (pfo == m_pfoDisplay)
    {
        SetDir(pfo->m_pfoParent);
        return;
    }

    // If the user deleted a file in the listview, then remove it
    if (pfo->m_pfoParent == m_pfoDisplay)
        DeleteItem(pvObj);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CExpListView::RefreshObject
// Purpose:   Force an update of a particular object.  Only called when size
//            and/or time accessed of the object has changed.
// Arguments: pvObj     -- The objec to refresh
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CExpListView::RefreshObject(void *pvObj)
{
    // UNDONE: Unoptimized
    for (int i = 0; i < m_cItems; i++)
    {
        if (m_rgli[i].pvData == pvObj)
        {
            // Found it.  Update size and time
            ((CFileObject*)pvObj)->UpdateSizeTime();
            RefreshItem(i);
            m_fDirty = TRUE;
            break;
        }
    }
}

void CExpListView::UpdateCheckState()
{
    assert(m_pfoDisplay);

    CFileObject *pfoChild = m_pfoDisplay->m_lpfoChildren.GetFirst();
    
    // Shouldn't get here unless there's at least one child in the listview
    assert(pfoChild);

    // Determine the aggregate state of all objects in the current folder
    eCheckState csAggregate = pfoChild->m_checkstate;
    while ((pfoChild = m_pfoDisplay->m_lpfoChildren.GetNext()) != NULL)
    {
        if (pfoChild->m_checkstate != csAggregate)
        {
            csAggregate = CHECKSTATE_TRISTATE;
            break;
        }
    }
    
    // Has state of currently displayed object changed?
    if (csAggregate != m_pfoDisplay->m_checkstate)
    {
        g_vmExplorer.m_treeview.RecurseSetCheckState(m_pfoDisplay->m_hti,
                                                     csAggregate);
    }
}

void CExpListView::SetChildrenCheckState(CFileObject *pfo)
{
    assert(pfo);
    
    // set all child of 'pfo' to pfo's checkstate
    CFileObject *pfoChild = pfo->m_lpfoChildren.GetFirst();

    while (pfoChild)
    {
        pfoChild->m_checkstate = pfo->m_checkstate;
        if (pfoChild->m_fIsDir)
            SetChildrenCheckState(pfoChild);
        pfoChild = pfo->m_lpfoChildren.GetNext();
    }
}

void CExpListView::Notify_SelectChange(void *pvObj, BOOL fSelected)
{
    CFileObject *pfo = (CFileObject*)pvObj;

    if (pfo == NULL)
    {
        m_nSelected = 0;
        m_uliSelectedSize.QuadPart = 0;
        return;
    }

    // update 'space used'
    if (fSelected)
    {
        m_nSelected++;
        m_uliSelectedSize.QuadPart += pfo->m_uliSize.QuadPart;
    }
    else
    {
        m_nSelected--;
        m_uliSelectedSize.QuadPart -= pfo->m_uliSize.QuadPart;
        assert(m_nSelected >= 0);
    }

    // Update status bar
    char sz[100];
    int nObjects = m_pfoDisplay->m_lpfoChildren.GetCount();
    if (m_nSelected == 0)
    {
        sprintf(sz, "%d objects", nObjects);
        g_statusbar.SetText(0, sz);

        FormatByteString(m_pfoDisplay->m_lpfoChildren.GetSize(), sz);
        g_statusbar.SetText(1, sz);
    }
    else if (m_nSelected == 1)
    {
        // Which is selected object?
        CFileObject *pfoSelected;
        if (fSelected)
            pfoSelected = pfo;
        else
        {
            // find it
            for (int i = 0; i < m_cItems; i++)
                if (m_rgli[i].dwFlags & LVF_ITEMSELECTED)
                {
                    pfoSelected = (CFileObject*)m_rgli[i].pvData;
                    break;
                }
            assert(i < m_cItems);
        }
        sprintf(sz, "Type: %s  Date Modified: %s  Size: %s  Location: %s",
                pfoSelected->m_szType, pfoSelected->m_szModified,
                pfoSelected->m_szSize, pfoSelected->m_szLocation);
        
        g_statusbar.SetText(0, sz);
        g_statusbar.SetText(1, pfoSelected->m_szSize);
    }
    else
    {
        sprintf(sz, "%d objects selected", m_nSelected);        
        g_statusbar.SetText(0, sz);
        FormatByteString(m_uliSelectedSize, sz);
        g_statusbar.SetText(1, sz);
    }
}