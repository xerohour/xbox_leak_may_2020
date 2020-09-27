// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CUnplacedListView.cpp
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

enum {COL_NAME = 0, COL_FOLDER, COL_SIZEONDISK};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CUnplacedListView::Init
// Purpose:   
// Arguments: 
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CUnplacedListView::Init(HWND hwndParent)
{
    // initialize our 'MyListView' object.
    if (!CMyListView::Init(hwndParent))
        return FALSE;

    SHFILEINFO sfi;
    ZeroMemory(&sfi, sizeof(sfi));
    DWORD flg = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX;
    m_himlFileIcons = (HIMAGELIST)SHGetFileInfo("C:\\", 0, &sfi, sizeof(sfi), flg);

    // init columns for listview
    // NOTE: Order must match order in COL_* enumeration
    AddColumn("Name", 150);
    AddColumn("Folder", 250);
    AddColumn("Size on Disk", 75);

    // Create the GDI font object
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = -11;
    lf.lfWeight = 0;
    strcpy(lf.lfFaceName, "Tahoma");
    m_hfont = CreateFontIndirect(&lf);

    m_iSortColumn = COL_NAME;
    return TRUE;
}

#define LVF_ITEMSELECTED 1
#define LVF_ITEMFOCUSED  2
 
void CUnplacedListView::UpdateItem(void *pvItem, DWORD dwItemState, int iColumn, HDC hdcDest, RECT *prcDest)
{
    char *psz;
    int nW;
    CFileObject *pfo = (CFileObject*)pvItem;
    DWORD dwAlign = DT_LEFT;

    // if prcClip is NULL, then the item is completely offscreen
    if (prcDest == NULL)
        return;

    // undone: Can choose to use offscreenbuffer only when resizing if so desire (not really necessary for scroll/update)
#define OFFSCREENBUFFER
#ifdef OFFSCREENBUFFER
    static HBITMAP s_hbmpOff;
    static int s_nLastWidth = -1, s_nLastHeight = -1;
    RECT rcOrig = *prcDest;
    prcDest->bottom -= prcDest->top;
    prcDest->top = 0;
    prcDest->right -= prcDest->left;
    prcDest->left = 0;
    
    HDC hdcOffscreen = CreateCompatibleDC(hdcDest);
    if (prcDest->right - prcDest->left > s_nLastWidth  || prcDest->bottom - prcDest->top > s_nLastHeight)
    {
        s_hbmpOff = CreateCompatibleBitmap(hdcDest, prcDest->right - prcDest->left,
                                           prcDest->bottom - prcDest->top);
        s_nLastWidth = prcDest->right - prcDest->left;
        s_nLastHeight = prcDest->bottom - prcDest->top;
        DebugOutput("create (%d, %d)\n", s_nLastWidth, s_nLastHeight);
    }
    HBITMAP hbmpPrevOff = (HBITMAP)SelectObject(hdcOffscreen, s_hbmpOff);
#else
    HDC hdcOffscreen = hdcDest;
#endif
    SelectObject(hdcOffscreen, m_hfont);
    if (dwItemState & LVF_ITEMSELECTED)
    {
        if (dwItemState & LVF_ITEMFOCUSED)
        {
            FillRect(hdcOffscreen, prcDest, GetSysColorBrush(COLOR_HIGHLIGHT));
            SetBkColor(hdcOffscreen, GetSysColor(COLOR_HIGHLIGHT));
            SetTextColor(hdcOffscreen, GetSysColor(COLOR_HIGHLIGHTTEXT));
        }
        else
        {
            FillRect(hdcOffscreen, prcDest, GetSysColorBrush(COLOR_INACTIVEBORDER));
            SetBkColor(hdcOffscreen, GetSysColor(COLOR_INACTIVEBORDER));
            SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOWTEXT));
        }
    }
    else
    {
        FillRect(hdcOffscreen, prcDest, GetSysColorBrush(COLOR_WINDOW));
        SetBkColor(hdcOffscreen, GetSysColor(COLOR_WINDOW));
        SetTextColor(hdcOffscreen, GetSysColor(COLOR_WINDOWTEXT));
    }
    if (iColumn != 0)
    {
        prcDest->right -= 5;
        prcDest->top ++;
    }
//enum {COL_NAME = 0, COL_FOLDER, COL_SIZEONDISK, COL_SECTORRANGE};

    switch(iColumn)
    {
    case COL_NAME:
        nW = min(GetSystemMetrics(SM_CXSMICON), prcDest->right - prcDest->left);
        if (nW > 0 && pfo->m_fIsDir)
        {
            DWORD dwFlags = ILD_TRANSPARENT | ILD_NORMAL;
            if (dwItemState & LVF_ITEMSELECTED && dwItemState & LVF_ITEMFOCUSED)
                dwFlags |= ILD_SELECTED;
            ImageList_Draw(m_himlFileIcons, pfo->m_iIcon, hdcOffscreen, prcDest->left, prcDest->top, dwFlags);
            prcDest->left += 20;
        }

        psz = pfo->m_szName;
        break;

    case COL_FOLDER:      psz = pfo->m_szFolder;      break;
    case COL_SIZEONDISK:  psz = pfo->m_szSizeOnDisk;  break;
    }
    if (psz)
        DrawTextEx(hdcOffscreen, psz, strlen(psz), prcDest, dwAlign | DT_END_ELLIPSIS, NULL);
#ifdef OFFSCREENBUFFER
    BitBlt(hdcDest, rcOrig.left, rcOrig.top,
           rcOrig.right - rcOrig.left, rcOrig.bottom - rcOrig.top,
           hdcOffscreen, 0, 0, SRCCOPY);
    SelectObject(hdcOffscreen, hbmpPrevOff);
    DeleteDC(hdcOffscreen);
#endif
}

DWORD CUnplacedListView::GetItemWidth(HDC hdc, void *pvItem, int iColumn)
{
    CFileObject *pfo = (CFileObject*)pvItem;
    SIZE    size;
    char    *psz;
    int     nAdd = 0;

    switch(iColumn)
    {
    case COL_NAME:        psz = pfo->m_szName;     if (pfo->m_fIsDir) nAdd = 20; break;
    case COL_FOLDER:      psz = pfo->m_szFolder;      break;
    case COL_SIZEONDISK:  psz = pfo->m_szSizeOnDisk;  break;
    }

    GetTextExtentPoint32(hdc, psz, strlen(psz), &size);
    SetTextJustification(hdc, 0, 0);
    return size.cx + nAdd + 5; // +5 for the padding we add during drawing
}

// return -1 if pv1 is "less than" pv2
// return 1 if pv2 is "less than" pv1
// return 0 if pv1 "equals" pv2
int CUnplacedListView::CompareObjects(const void *pv1, const void *pv2)
{
    int nRet;

    // pv1 and pv2 are passed in as pointers to array elements.  Do
    // some wacky casting to get the actual object pointers.
    CFileObject *po1 = ((CFileObject*)*(DWORD*)pv1);
    CFileObject *po2 = ((CFileObject*)*(DWORD*)pv2);

    // If one of the objects is a directory and the other isn't, then the directory
    // always comes first
    bool f1 = po1->m_dwa & FILE_ATTRIBUTE_DIRECTORY ? true : false;
    bool f2 = po2->m_dwa & FILE_ATTRIBUTE_DIRECTORY ? true : false;
    if (f1 && !f2)
        return -1;
    if (f2 && !f1)
        return 1;

    switch(m_iSortColumn)
    {
    case COL_NAME:
        return _stricmp(po1->m_szName, po2->m_szName);

    case COL_FOLDER:
        nRet = _stricmp(po1->m_szFolder, po2->m_szFolder);
        if (nRet == 0)
            nRet = _stricmp(po1->m_szName, po2->m_szName);
        return nRet;

    case COL_SIZEONDISK:
        nRet = _stricmp(po1->m_szSizeOnDisk, po2->m_szSizeOnDisk);
        if (nRet == 0)
            nRet = _stricmp(po1->m_szName, po2->m_szName);
        return nRet;
    }
    assert(true);
    return 0;
}

// Over a checkbox? If so, uncheck it
BOOL CUnplacedListView::Override_LButtonDown(sMouseInfo *pmi, DWORD dwFlags)
{
    return TRUE;
}

BOOL CUnplacedListView::Override_LButtonDblClick(sMouseInfo *pmi, DWORD dwFlags)
{
    return TRUE;
}

BOOL CUnplacedListView::Override_MouseMove(sMouseInfo *pmi, DWORD dwFlags)
{
    return TRUE;
}

void CUnplacedListView::AddObject(void *pvObj)
{
    AddItemAtIndex(pvObj, 0);
}

void CUnplacedListView::RemoveObject(void *pvObj)
{
    DeleteItem(pvObj);
}

void CUnplacedListView::RefreshObject(void *pvObj)
{
    // UNDONE: very unoptimized
    for (int i = 0; i < m_cItems; i++)
    {
        if (m_rgli[i].pvData == pvObj)
        {
            // Found it.  Update size and time
            ((CFileObject*)pvObj)->UpdateSizeTime();
            RefreshItem(i);
            m_fDirty = true;
            break;
        }
    }
}
