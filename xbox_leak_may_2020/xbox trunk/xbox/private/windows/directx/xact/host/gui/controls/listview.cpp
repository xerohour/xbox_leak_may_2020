/***************************************************************************
 *
 *  Copyright (C) 2/7/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       listview.cpp
 *  Content:    List-view control wrapper class.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/7/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactctl.h"

const LPCTSTR CListView::m_pszClassName = XACTGUI_MAKE_WNDCLASS_NAME("ListView");

BEGIN_DEFINE_STRUCT()
    HWND            hWnd;
    UINT            nSubItem;
    BOOL            fAscending;
END_DEFINE_STRUCT(LVSORTITEMDATA);


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the window.
 *
 *  Arguments:
 *      CListView * [in]: parent window.
 *      DWORD [in]: extended window style.
 *      DWORD [in]: window style.
 *      int [in]: x-coordinate.
 *      int [in]: y-coordinate.
 *      int [in]: width.
 *      int [in]: height.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewCtrl::Create"

BOOL
CListViewCtrl::Create
(
    CListView *             pParent, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight
)
{
    return CWindow::Create(pParent, WC_LISTVIEW, NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight);
}


/****************************************************************************
 *
 *  CListView
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::CListView"

CListView::CListView
(
    void
)
{
    //
    // Initialize the type list
    //
    
    InitializeListHead(&m_lstTypes);

    //
    // Create the image lists
    //

    m_hLargeImageList = ImageList_Create(32, 32, ILC_COLOR16 | ILC_MASK, 0, 0x100);
    m_hSmallImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 0x100);
}


/****************************************************************************
 *
 *  ~CListView
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::~CListView"

CListView::~CListView
(
    void
)
{
    CListViewType *         pType;
    PLIST_ENTRY             pleEntry;
    
    //
    // Free registered types
    //

    while((pleEntry = RemoveEntryList(m_lstTypes.Flink)) != &m_lstTypes)
    {
        pType = CONTAINING_RECORD(pleEntry, CListViewType, m_leTypes);

        RELEASE(pType);
    }

    //
    // Free the image lists
    //

    ImageList_Destroy(m_hLargeImageList);
    ImageList_Destroy(m_hSmallImageList);
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the window.
 *
 *  Arguments:
 *      HWND [in]: parent window.
 *      UINT [in]: control identifier.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::Create"

BOOL
CListView::Create
(
    CWindow *               pParent, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    DWORD                   dwListExStyle,
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight, 
    UINT                    nControlId
)
{
    WNDCLASSEX              wc              = { 0 };
    DWORD                   adwStyles[2];

    //
    // Mask off and save the list-view specific style bits
    //

    adwStyles[0] = LOWORD(dwStyle);
    dwStyle &= ~adwStyles[0];

    adwStyles[1] = dwListExStyle;

    //
    // Register the window class
    //

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = g_hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszClassName = m_pszClassName;

    RegisterClassEx(&wc);

    //
    // Create the window
    //

    return CWindow::Create(pParent, m_pszClassName, NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight, nControlId, (LPVOID)&adwStyles);
}


/****************************************************************************
 *
 *  HandleMessage
 *
 *  Description:
 *      Dispatches a window message to the appropriate handler.
 *
 *  Arguments:
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::HandleMessage"

BOOL 
CListView::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case LVM_FIXITEMINDICES:
            FixItemIndices();
            break;
        
        default:
            fHandled = CWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnCreate"

BOOL
CListView::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    static const DWORD      dwExStyle   = 0;
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE | LVS_SHAREIMAGELISTS;
    const DWORD *           adwStyles   = (const DWORD *)pvContext;
    BOOL                    fSuccess;

    if(CWindow::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Create the list-view control
    //

    fSuccess = m_ListCtrl.Create(this, dwExStyle, dwStyle | adwStyles[0]);

    //
    // Set extended list-view style
    //

    if(fSuccess)
    {
        ListView_SetExtendedListViewStyle(m_ListCtrl, adwStyles[1]);
    }
    
    //
    // Set the image lists
    //

    if(fSuccess)
    {
        ASSERT(m_hLargeImageList);
        ASSERT(m_hSmallImageList);
        
        ListView_SetImageList(m_ListCtrl, m_hLargeImageList, LVSIL_NORMAL);
        ListView_SetImageList(m_ListCtrl, m_hSmallImageList, LVSIL_SMALL);

        ImageList_SetBkColor(m_hLargeImageList, ListView_GetBkColor(m_ListCtrl));
        ImageList_SetBkColor(m_hSmallImageList, ListView_GetBkColor(m_ListCtrl));
    }

    //
    // Success
    //

    if(!fSuccess)
    {
        *plResult = 0;
        return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles WM_DESTROY messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnDestroy"

BOOL
CListView::OnDestroy
(
    LRESULT *               plResult
)
{
    if(CWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    ListView_DeleteAllItems(m_ListCtrl);

    return FALSE;
}


/****************************************************************************
 *
 *  OnDeleteItem
 *
 *  Description:
 *      Handles LVN_DELETEITEM notifications.
 *
 *  Arguments:
 *      LPNMLISTVIEW [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnDeleteItem"

void
CListView::OnDeleteItem
(
    LPNMLISTVIEW            pHdr
)
{
    CListViewItem *         pItem   = CListViewItem::PtrFromListIndex(this, pHdr->iItem);

    //
    // Notify the item
    //

    pItem->OnDelete();

    //
    // Post a message to ourselves to update the item indices *after* the
    // item has been deleted.
    //

    PostMessage(LVM_FIXITEMINDICES);
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles WM_SIZE messages.
 *
 *  Arguments:
 *      UINT [in]: resize type.
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnSize"

BOOL
CListView::OnSize
(
    UINT                    nType,
    UINT                    nWidth,
    UINT                    nHeight,
    LRESULT *               plResult
)
{
    if(CWindow::OnSize(nType, nWidth, nHeight, plResult))
    {
        return TRUE;
    }
    
    m_ListCtrl.MoveWindow(0, 0, nWidth, nHeight);

    return FALSE;
}


/****************************************************************************
 *
 *  OnNotify
 *
 *  Description:
 *      Handles WM_NOTIFY messages.
 *
 *  Arguments:
 *      LPNMHDR [in]: notification header.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnNotify"

BOOL
CListView::OnNotify
(
    LPNMHDR                 pHdr,
    LRESULT *               plResult
)
{
    if(CWindow::OnNotify(pHdr, plResult))
    {
        return TRUE;
    }
    
    //
    // We're intercepting certain messages and passing the rest along to
    // the parent to be processed
    //
    
    switch(pHdr->code)
    {
        case LVN_DELETEITEM:
            OnDeleteItem((LPNMLISTVIEW)pHdr);
            break;

        case LVN_COLUMNCLICK:
            OnColumnClick((LPNMLISTVIEW)pHdr);
            break;

        case LVN_ENDLABELEDIT:
            *plResult = OnEndLabelEdit((LPNMLVDISPINFO)pHdr);
            return TRUE;

        case LVN_ITEMCHANGED:
            OnItemChanged((LPNMLISTVIEW)pHdr);
            break;

        case NM_DBLCLK:
        case NM_RETURN:
            OnDoubleClick(pHdr);
            break;
    };

    return FALSE;
}


/****************************************************************************
 *
 *  OnColumnClick
 *
 *  Description:
 *      Handles column-click notifications.
 *
 *  Arguments:
 *      LPNMLISTVIEW [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnColumnClick"

void
CListView::OnColumnClick
(
    LPNMLISTVIEW            pHdr
)
{
    //
    // Resort the list
    //

    Sort(pHdr->iSubItem, LVSORT_TOGGLE_DIRECTION);
}


/****************************************************************************
 *
 *  OnEndLabelEdit
 *
 *  Description:
 *      Handles item text editing.
 *
 *  Arguments:
 *      LPNMLVDISPINFO [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnEndLabelEdit"

BOOL
CListView::OnEndLabelEdit
(
    LPNMLVDISPINFO          pHdr
)
{
    CListViewItem *         pItem   = pItem->PtrFromListCtx(pHdr->item.lParam);

    if(!pHdr->item.pszText)
    {
        return FALSE;
    }

    if(!*pHdr->item.pszText)
    {
        return FALSE;
    }

    return pItem->OnEndLabelEdit(pHdr->item.pszText);
}


/****************************************************************************
 *
 *  OnItemChanged
 *
 *  Description:
 *      Handles notifications that an item has changed in some way.
 *
 *  Arguments:
 *      LPNMLISTVIEW [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnItemChanged"

void
CListView::OnItemChanged
(
    LPNMLISTVIEW            pHdr
)
{
    CListViewItem *         pItem   = pItem->PtrFromListCtx(pHdr->lParam);

    if(pHdr->uChanged & LVIF_STATE)
    {
        pItem->OnStateChanged(pHdr->uOldState, pHdr->uNewState);
    }
}


/****************************************************************************
 *
 *  OnDoubleClick
 *
 *  Description:
 *      Handles NM_DBLCLK notifications.
 *
 *  Arguments:
 *      LPNMHDR [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::OnDoubleClick"

void
CListView::OnDoubleClick
(
    LPNMHDR                 pHdr
)
{
    CListViewItem *         pItem   = NULL;

    //
    // Find the selected item or items and call the activation callback
    //

    while(pItem = GetNextItem(pItem, LVNI_SELECTED))
    {
        pItem->OnDoubleClick();
    }
}


/****************************************************************************
 *
 *  RegisterType
 *
 *  Description:
 *      Registers an item type.
 *
 *  Arguments:
 *      LPCTSTR [in]: type name.
 *      HICON [in]: type icon.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::RegisterType"

BOOL
CListView::RegisterType
(
    LPCTSTR                 pszText,
    HICON                   hLargeIcon,
    HICON                   hSmallIcon
)
{
    CListViewType *         pType;
    int                     nLargeImageIndex;
    int                     nSmallImageIndex;

    ASSERT(pszText);
    ASSERT(hLargeIcon);
    ASSERT(hSmallIcon);

    //
    // Make sure the type isn't already registered
    //

    if(FindRegisteredType(pszText))
    {
        return TRUE;
    }

    //
    // Add the icon to the image list
    //

    if(-1 == (nLargeImageIndex = ImageList_AddIcon(m_hLargeImageList, hLargeIcon)))
    {
        return FALSE;
    }

    if(-1 == (nSmallImageIndex = ImageList_AddIcon(m_hSmallImageList, hSmallIcon)))
    {
        return FALSE;
    }

    ASSERT(nLargeImageIndex == nSmallImageIndex);

    //
    // Add the type to the list
    //
    
    if(!(pType = NEW(CListViewType(this, pszText, nLargeImageIndex))))
    {
        return FALSE;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  InsertColumn
 *
 *  Description:
 *      Inserts a column into the list.
 *
 *  Arguments:
 *      LPCTSTR [in]: column header text.
 *      LVCTYPE [in]: column data type.
 *      BOOL [in]: sort flag.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::InsertColumn"

BOOL
CListView::InsertColumn
(
    LPCTSTR                 pszText,
    LVCTYPE                 nDataType,
    BOOL                    fSortAscending
)
{
    LVCOLUMN                lvc;
    
    //
    // Insert the column
    //

    lvc.mask = LVCF_SUBITEM | LVCF_TEXT;
    lvc.pszText = (LPTSTR)pszText;
    lvc.iSubItem = m_nColumnCount;

    if(-1 == ListView_InsertColumn(m_ListCtrl, m_nColumnCount, &lvc))
    {
        return FALSE;
    }

    //
    // Save the column's data
    //

    m_aColumnData[m_nColumnCount].nHeaderStringId = -1;
    m_aColumnData[m_nColumnCount].nDataType = nDataType;
    m_aColumnData[m_nColumnCount].fSortAscending = fSortAscending;

    //
    // Resize the column
    //

    ResizeColumn(m_nColumnCount);

    //
    // Increment the column count
    //

    m_nColumnCount++;

    return TRUE;
}


/****************************************************************************
 *
 *  InsertColumns
 *
 *  Description:
 *      Inserts columns into the list.
 *
 *  Arguments:
 *      LPCLVCOLUMNDATA [in]: column data.
 *      UINT [in]: column count.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::InsertColumns"

BOOL
CListView::InsertColumns
(
    LPCLVCOLUMNDATA         paColumnData,
    UINT                    nColumnCount
)
{
    BOOL                    fSuccess    = TRUE;
    BOOL                    fRedraw;

    fRedraw = SetRedraw(FALSE);

    while(fSuccess && nColumnCount--)
    {
        fSuccess = InsertColumn(FormatStringResourceStatic(paColumnData->nHeaderStringId), paColumnData->nDataType, paColumnData->fSortAscending);

        paColumnData++;
    }

    SetRedraw(fRedraw);

    return fSuccess;
}


/****************************************************************************
 *
 *  Sort
 *
 *  Description:
 *      Sorts item data.
 *
 *  Arguments:
 *      UINT [in]: column index, or -1 to use the most recent sort column.
 *      DWORD [in]: sort flags.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::Sort"

void
CListView::Sort
(
    UINT                    nColumnIndex,
    DWORD                   dwFlags
)
{
    LVSORTITEMDATA          SortData;
    
    //
    // If the column index is -1, use the last column someone sorted by
    //
    
    if(-1 == nColumnIndex)
    {
        nColumnIndex = m_nLastSortColumn;
    }
    
    //
    // Set up sort data
    //
    
    ASSERT(nColumnIndex < m_nColumnCount);

    SortData.hWnd = m_ListCtrl;
    SortData.nSubItem = nColumnIndex;

    if(dwFlags & LVSORT_FORCE_ASCENDING)
    {
        ASSERT(!(dwFlags & (LVSORT_FORCE_DECENDING | LVSORT_TOGGLE_DIRECTION)));
        SortData.fAscending = TRUE;
    }
    else if(dwFlags & LVSORT_FORCE_DECENDING)
    {
        ASSERT(!(dwFlags & (LVSORT_FORCE_ASCENDING | LVSORT_TOGGLE_DIRECTION)));
        SortData.fAscending = FALSE;
    }
    else
    {
        SortData.fAscending = m_aColumnData[nColumnIndex].fSortAscending;

        if(dwFlags & LVSORT_TOGGLE_DIRECTION)
        {
            m_aColumnData[nColumnIndex].fSortAscending = !m_aColumnData[nColumnIndex].fSortAscending;
        }
    }

    //
    // Sort
    //

    if(LVCTYPE_NUMBER == m_aColumnData[nColumnIndex].nDataType)
    {
        ListView_SortItemsEx(m_ListCtrl, NumericSortCallback, &SortData);
    }
    else
    {
        ListView_SortItemsEx(m_ListCtrl, StringSortCallback, &SortData);
    }

    //
    // Save the column index
    //
    
    m_nLastSortColumn = nColumnIndex;

    //
    // Any time we sort, all the child items lose their item indices because
    // the stupid list-view stores everything as an index, not a handle.
    //

    FixItemIndices();
}


/****************************************************************************
 *
 *  StringSortCallback
 *
 *  Description:
 *      Sorting callback function for string columns.
 *
 *  Arguments:
 *      LPARAM [in]: item index 1.
 *      LPARAM [in]: item index 2.
 *      LPARAM [in]: sort context.
 *
 *  Returns:  
 *      int: negative value if the first item should precede the second, a 
 *           positive value if the first item should follow the second, or 
 *           zero if the two items are equivalent.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::StringSortCallback"

int
CListView::StringSortCallback
(
    LPARAM                  nItem0,
    LPARAM                  nItem1,
    LPARAM                  lContext
)
{
    LPCLVSORTITEMDATA       pContext            = (LPCLVSORTITEMDATA)lContext;
    CHAR                    szText[2][0x100];
    int                     nResult;
    
    ListView_GetItemText(pContext->hWnd, nItem0, pContext->nSubItem, szText[0], NUMELMS(szText[0]));
    ListView_GetItemText(pContext->hWnd, nItem1, pContext->nSubItem, szText[1], NUMELMS(szText[1]));

    nResult = strcmp(szText[0], szText[1]);

    if(!pContext->fAscending)
    {
        nResult = -nResult;
    }

    return nResult;
}


/****************************************************************************
 *
 *  NumericSortCallback
 *
 *  Description:
 *      Sorting callback function for numeric columns.
 *
 *  Arguments:
 *      LPARAM [in]: item index 1.
 *      LPARAM [in]: item index 2.
 *      LPARAM [in]: sort context.
 *
 *  Returns:  
 *      int: negative value if the first item should precede the second, a 
 *           positive value if the first item should follow the second, or 
 *           zero if the two items are equivalent.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::NumericSortCallback"

int
CListView::NumericSortCallback
(
    LPARAM                  nItem0,
    LPARAM                  nItem1,
    LPARAM                  lContext
)
{
    LPCLVSORTITEMDATA       pContext            = (LPCLVSORTITEMDATA)lContext;
    CHAR                    szText[2][0x100];
    int                     nValue[2];
    int                     nResult;
    
    ListView_GetItemText(pContext->hWnd, nItem0, pContext->nSubItem, szText[0], NUMELMS(szText[0]));
    ListView_GetItemText(pContext->hWnd, nItem1, pContext->nSubItem, szText[1], NUMELMS(szText[1]));

    nValue[0] = atoi(szText[0]);
    nValue[1] = atoi(szText[1]);

    nResult = nValue[0] - nValue[1];

    if(!pContext->fAscending)
    {
        nResult = -nResult;
    }

    return nResult;
}


/****************************************************************************
 *
 *  ResizeColumn
 *
 *  Description:
 *      Auto-resizes a list-view column.
 *
 *  Arguments:
 *      UINT [in]: column index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::ResizeColumn"

void
CListView::ResizeColumn
(
    UINT                    nColumn
)
{

#if 0

    static const int        nMarginWidth    = 7;
    CHAR                    szText[0x100];
    int                     nLargestWidth;
    int                     nWidth;
    int                     nCount;
    LVCOLUMN                lvc;
    int                     i;
    
    //
    // Get the width of the column header text
    //

    lvc.mask = LVCF_TEXT;
    lvc.pszText = szText;
    lvc.cchTextMax = NUMELMS(szText);
    
    ListView_GetColumn(m_ListCtrl, nColumn, &lvc);

    nLargestWidth = ListView_GetStringWidth(m_ListCtrl, szText);

    //
    // Get the width of each item in the column
    //

    nCount = ListView_GetItemCount(m_ListCtrl);
    
    for(i = 0; i < nCount; i++)
    {
        ListView_GetItemText(m_ListCtrl, i, nColumn, szText, NUMELMS(szText));

        if((nWidth = ListView_GetStringWidth(m_ListCtrl, szText)) > nLargestWidth)
        {
            nLargestWidth = nWidth;
        }
    }

    //
    // Factor in the margins and set the column width
    //

    ListView_SetColumnWidth(m_ListCtrl, nColumn, nLargestWidth + (nMarginWidth * 2));

#else

    int                     nColumnWidth;
    int                     nItemWidth;
    BOOL                    fRedraw;

    fRedraw = SetRedraw(FALSE);

    //
    // Resize based on the column header
    //

    ListView_SetColumnWidth(m_ListCtrl, nColumn, LVSCW_AUTOSIZE_USEHEADER);

    nColumnWidth = ListView_GetColumnWidth(m_ListCtrl, nColumn);

    //
    // Resize based on the item text
    //

    ListView_SetColumnWidth(m_ListCtrl, nColumn, LVSCW_AUTOSIZE);

    nItemWidth = ListView_GetColumnWidth(m_ListCtrl, nColumn);

    //
    // Use the larger of the two
    //

    ListView_SetColumnWidth(m_ListCtrl, nColumn, max(nColumnWidth, nItemWidth));

    SetRedraw(fRedraw);

#endif

}


/****************************************************************************
 *
 *  ResizeColumns
 *
 *  Description:
 *      Resizes all columns in a list view.
 *
 *  Arguments:
 *      HWND [in]: list view window handle.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::ResizeColumns"

void
CListView::ResizeColumns
(
    void
)
{
    BOOL                    fRedraw;
    UINT                    i;
    
    fRedraw = SetRedraw(FALSE);
    
    for(i = 0; i < m_nColumnCount; i++)
    {
        ResizeColumn(i);
    }

    SetRedraw(fRedraw);
}


/****************************************************************************
 *
 *  FindRegisteredType
 *
 *  Description:
 *      Finds a registered item type.
 *
 *  Arguments:
 *      LPCTSTR [in]: type name.
 *
 *  Returns:  
 *      CListViewType *: registered type object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::FindRegisteredType"

CListViewType *
CListView::FindRegisteredType
(
    LPCTSTR                 pszText
)
{
    PLIST_ENTRY             pleEntry;
    CListViewType *         pType;

    ASSERT(pszText);
    
    for(pleEntry = m_lstTypes.Flink; pleEntry != &m_lstTypes; pleEntry = pleEntry->Flink)
    {
        pType = CONTAINING_RECORD(pleEntry, CListViewType, m_leTypes);

        if(!_tcsicmp(pszText, pType->m_szName))
        {
            return pType;
        }
    }

    return NULL;
}


/****************************************************************************
 *
 *  FixItemIndices
 *
 *  Description:
 *      Resets the item indices for all items in the list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::FixItemIndices"

void
CListView::FixItemIndices
(
    void
)
{
    int                     nItemCount;
    int                     nItem;
    CListViewItem *         pItem;

    nItemCount = ListView_GetItemCount(m_ListCtrl);

    for(nItem = 0; nItem < nItemCount; nItem++)
    {
        pItem = PtrFromListIndex(nItem);

        pItem->m_nItemIndex = nItem;
    }
}


/****************************************************************************
 *
 *  GetNextItem
 *
 *  Description:
 *      Gets the next related item.
 *
 *  Arguments:
 *      CListViewItem * [in]: current item, or NULL.
 *      DWORD [in]: relationship flags.
 *
 *  Returns:  
 *      CListViewItem *: next item.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::GetNextItem"

CListViewItem *
CListView::GetNextItem
(
    CListViewItem *         pItem,
    DWORD                   dwFlags
)
{
    int                     nItem;

    nItem = ListView_GetNextItem(m_ListCtrl.m_hWnd, pItem ? pItem->m_nItemIndex : -1, dwFlags);

    if(-1 == nItem)
    {
        return NULL;
    }

    return PtrFromListIndex(nItem);
}


/****************************************************************************
 *
 *  SelectAll
 *
 *  Description:
 *      Selects or de-selects all items.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to select; FALSE to de-select.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListView::SelectAll"

void
CListView::SelectAll
(
    BOOL                    fSelected
)
{
    CListViewItem *         pItem   = NULL;

    while(pItem = GetNextItem(pItem, LVNI_ALL))
    {
        pItem->SetSelection(fSelected);
    }
}


/****************************************************************************
 *
 *  CListViewType
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CListView * [in]: parent tree.
 *      LPCTSTR [in]: type name.
 *      int [in]: image list index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewType::CListViewType"

CListViewType::CListViewType
(
    CListView *             pList,
    LPCTSTR                 pszText,
    int                     nImageIndex
)
{
    ASSERT(pList);
    ASSERT(pszText);
    
    InsertTailListUninit(&pList->m_lstTypes, &m_leTypes);

    _tcscpy(m_szName, pszText);

    m_nImageIndex = nImageIndex;
}


/****************************************************************************
 *
 *  ~CListViewType
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewType::~CListViewType"

CListViewType::~CListViewType
(
    void
)
{
    RemoveEntryList(&m_leTypes);
}


/****************************************************************************
 *
 *  CListViewItem
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::CListViewItem"

CListViewItem::CListViewItem
(
    void
)
{
    m_nItemIndex = -1;
}


/****************************************************************************
 *
 *  ~CListViewItem
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::~CListViewItem"

CListViewItem::~CListViewItem
(
    void
)
{
    Remove();
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the list-view item.
 *
 *  Arguments:
 *      CListView * [in]: parent list.
 *      LPCTSTR [in]: item text.
 *      LPCTSTR [in]: item type.
 *      LPVOID [in]: item context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::Create"

BOOL
CListViewItem::Create
(
    CListView *             pList,
    LPCTSTR                 pszText,
    LPCTSTR                 pszType,
    LPVOID                  pvContext
)
{
    LVITEM                  lvi    = { 0 };
    CListViewType *         pType;

    ASSERT(pList);

    m_pList = pList;
    m_pvContext = pvContext;

    //
    // Insert the item
    //

    lvi.mask = LVIF_PARAM;
    lvi.lParam = (LPARAM)this;

    if(pszText)
    {
        lvi.mask |= LVIF_TEXT;
        lvi.pszText = (LPTSTR)pszText;
    }

    if(pszType)
    {
        if(pType = pList->FindRegisteredType(pszType))
        {
            lvi.mask |= LVIF_IMAGE;
            lvi.iImage = pType->m_nImageIndex;
        }
        else
        {
            DPF_ERROR("Type not registered");
        }
    }

    if(-1 == (m_nItemIndex = ListView_InsertItem(pList->m_ListCtrl, &lvi)))
    {
        return -1;
    }

    //
    // Resort the list
    //

    pList->Sort(-1, 0);

    return m_nItemIndex;
}


/****************************************************************************
 *
 *  Remove
 *
 *  Description:
 *      Removes the item from the list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::Remove"

void
CListViewItem::Remove
(
    void
)
{
    CListView *             pList   = m_pList;
    
    if(pList && (-1 != m_nItemIndex))
    {
        ListView_DeleteItem(pList->m_ListCtrl, m_nItemIndex);
        pList->FixItemIndices();
    }
}


/****************************************************************************
 *
 *  SetColumnText
 *
 *  Description:
 *      Sets the item text.
 *
 *  Arguments:
 *      int [in]: column index.
 *      LPCTSTR [in]: text.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::SetColumnText"

void
CListViewItem::SetColumnText
(
    int                     nColumnIndex,
    LPCTSTR                 pszText
)
{
    LVITEM                  lvi;

    if(!m_pList)
    {
        return;
    }

    lvi.mask = TVIF_TEXT;
    lvi.pszText = (LPTSTR)pszText;
    lvi.iItem = m_nItemIndex;
    lvi.iSubItem = nColumnIndex;

    ListView_SetItem(m_pList->m_ListCtrl, &lvi);
}


/****************************************************************************
 *
 *  SetColumnValue
 *
 *  Description:
 *      Sets the item text.
 *
 *  Arguments:
 *      int [in]: column index.
 *      int [in]: value.
 *      BOOL [in]: TRUE for signed, FALSE for unsigned.
 *      BOOL [in]: TRUE to format (comma-separated thousands)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::SetColumnValue"

void
CListViewItem::SetColumnValue
(
    int                     nColumnIndex,
    int                     nValue,
    BOOL                    fSigned,
    BOOL                    fFormat
)
{
    TCHAR                   szText[0x100];

    FormatNumber(nValue, fSigned, fFormat, szText);
    
    SetColumnText(nColumnIndex, szText);
}


/****************************************************************************
 *
 *  PtrFromListIndex
 *
 *  Description:
 *      Converts a list item index to an object pointer.
 *
 *  Arguments:
 *      CListView * [in]: list view object.
 *      int [in]: list item index.
 *
 *  Returns:  
 *      CListViewItem *: object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::PtrFromListIndex"

CListViewItem *
CListViewItem::PtrFromListIndex
(
    CListView *             pList,
    int                     nItem
)
{
    LVITEM                  lvi;

    ASSERT(pList);

    lvi.mask = TVIF_PARAM;
    lvi.iItem = nItem;
    lvi.iSubItem = 0;

    if(!ListView_GetItem(pList->m_ListCtrl, &lvi))
    {
        return NULL;
    }

    return PtrFromListCtx(lvi.lParam);
}


/****************************************************************************
 *
 *  GetListIndex
 *
 *  Description:
 *      Gets the current list index for this object.  Because the list-view
 *      maintains it's own list that is (stupidly) referenced by index, but
 *      those indexes change due to sorting and user interaction, we never
 *      store the item index.  This means that every time we want to 
 *      reference the object in the list, we have to find it.  Suckity suck.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      int: item index.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::GetListIndex"

int
CListViewItem::GetListIndex
(
    void
)
{
    LVFINDINFO              lvfi;

    if(!m_pList)
    {
        return -1;
    }

    lvfi.flags = LVFI_PARAM;
    lvfi.lParam = (LPARAM)this;
    
    return m_pList->m_ListCtrl.SendMessage(LVM_FINDITEM, (WPARAM)-1, (LPARAM)&lvfi);
}


/****************************************************************************
 *
 *  OnDelete
 *
 *  Description:
 *      Handles TVN_DELETEITEM notifications.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::OnDelete"

void
CListViewItem::OnDelete
(
    void
)
{
    m_pList = NULL;
    m_nItemIndex = -1;
    m_pvContext = NULL;
}


/****************************************************************************
 *
 *  SetSelection
 *
 *  Description:
 *      Sets the selection state for the item.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to select; FALSE to de-select.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CListViewItem::SetSelection"

void
CListViewItem::SetSelection
(
    BOOL                    fSelected
)
{
    ListView_SetItemState(m_pList->m_ListCtrl, m_nItemIndex, LVIS_SELECTED, LVIS_SELECTED);
}


