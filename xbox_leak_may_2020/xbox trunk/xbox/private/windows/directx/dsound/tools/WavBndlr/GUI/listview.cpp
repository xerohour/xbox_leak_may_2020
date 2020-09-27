/***************************************************************************
 *
 *  Copyright (C) 11/12/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       listview.cpp
 *  Content:    Misc. list-view helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/12/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wbndgui.h"

BEGIN_DEFINE_STRUCT()
    HWND            hwnd;
    UINT            nSubItem;
    BOOL            fString;
    BOOL            fAscending;
    LPFNLVSORTITEMS pfnCallback;
    LPVOID          pvContext;
END_DEFINE_STRUCT(LVSORTITEMDATA);


/****************************************************************************
 *
 *  ListView_SelectItem
 *
 *  Description:
 *      Sets the selection state of a list view item.
 *
 *  Arguments:
 *      HWND [in]: list view window handle.
 *      UINT [in]: item index.
 *      BOOL [in]: TRUE to select the item; FALSE to unselect it.
 *      BOOL [in]: TRUE to unselect all other items.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_SelectItem"

void
ListView_SelectItem
(
    HWND                    hwnd,
    UINT                    nItem,
    BOOL                    fSelected,
    BOOL                    fSingleSelect
)
{
    static const DWORD      dwStateMask = LVIS_SELECTED | LVIS_FOCUSED;
    int                     nItemCount;
    int                     i;
    
    if(fSingleSelect)
    {
        nItemCount = ListView_GetItemCount(hwnd);

        for(i = 0; i < (int)nItemCount; i++)
        {
            if(i != (int)nItem)
            {
                ListView_SetItemState(hwnd, i, 0, dwStateMask);
            }
        }
    }

    ListView_SetItemState(hwnd, nItem, fSelected ? dwStateMask : 0, dwStateMask);
}


/****************************************************************************
 *
 *  ListView_ResizeColumns
 *
 *  Description:
 *      Resizes all columns in a list view.
 *
 *  Arguments:
 *      HWND [in]: list view window handle.
 *      int [in]: column width.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_ResizeColumns"

void
ListView_ResizeColumns
(
    HWND                    hwnd,
    int                     cx
)
{
    HWND                    hwndHeader;
    int                     nColumns;
    int                     i;

    hwndHeader = ListView_GetHeader(hwnd);
    nColumns = Header_GetItemCount(hwndHeader);

    for(i = 0; i < nColumns; i++)
    {
        ListView_SetColumnWidth(hwnd, i, cx);
    }
}


/****************************************************************************
 *
 *  ListView_AutoSizeColumn
 *
 *  Description:
 *      Resizes a columns in a list view.
 *
 *  Arguments:
 *      HWND [in]: list view window handle.
 *      UINT [in]: column index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_AutoSizeColumn"

void
ListView_AutoSizeColumn
(
    HWND                    hwnd,
    UINT                    nColumn
)
{
    static const int        nMarginWidth    = 7;
    CHAR                    szText[0x400];
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
    
    ListView_GetColumn(hwnd, nColumn, &lvc);

    nLargestWidth = ListView_GetStringWidth(hwnd, szText);

    //
    // Get the width of each item in the column
    //

    nCount = ListView_GetItemCount(hwnd);
    
    for(i = 0; i < nCount; i++)
    {
        ListView_GetItemText(hwnd, i, nColumn, szText, NUMELMS(szText));

        if((nWidth = ListView_GetStringWidth(hwnd, szText)) > nLargestWidth)
        {
            nLargestWidth = nWidth;
        }
    }

    //
    // Factor in the margins and set the column width
    //

    ListView_SetColumnWidth(hwnd, nColumn, nLargestWidth + (nMarginWidth * 2));
}


/****************************************************************************
 *
 *  ListView_AutoSizeColumns
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
#define DPF_FNAME "ListView_AutoSizeColumns"

void
ListView_AutoSizeColumns
(
    HWND                    hwnd
)
{
    HWND                    hwndHeader;
    int                     nCount;
    int                     i;
    
    hwndHeader = ListView_GetHeader(hwnd);
    nCount = Header_GetItemCount(hwndHeader);

    for(i = 0; i < nCount; i++)
    {
        ListView_AutoSizeColumn(hwnd, i);
    }
}


/****************************************************************************
 *
 *  ListView_SetItemText
 *
 *  Description:
 *      Sets the text for a list-view item.
 *
 *  Arguments:
 *      HWND [in]: list view window handle.
 *      UINT [in]: item index.
 *      UINT [in]: sub-item index.
 *      LPCSTR [in]: item format text.
 *      ...
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_SetItemText"

BOOL
ListView_SetItemText
(
    HWND                    hwnd,
    UINT                    nItem,
    UINT                    nSubItem,
    LPCSTR                  pszFormat,
    ...
)
{
    CHAR                    szText[0x100];
    LVITEM                  lvi;
    va_list                 va;

    va_start(va, pszFormat);
    vsprintf(szText, pszFormat, va);
    va_end(va);

    lvi.mask = LVIF_TEXT;
    lvi.iItem = nItem;
    lvi.iSubItem = nSubItem;
    lvi.pszText = szText;
    
    return ListView_SetItem(hwnd, &lvi);
}


/****************************************************************************
 *
 *  ListView_SortCallback
 *
 *  Description:
 *      ListView_SortItemsEx callback.
 *
 *  Arguments:
 *      LPARAM [in]: item index.
 *      LPARAM [in]: item index.
 *      LPARAM [in]: sort parameter.
 *
 *  Returns:  
 *      int: negative value if the first item should precede the second, a 
 *           positive value if the first item should follow the second, or 
 *           zero if the two items are equivalent. 
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_SortCallback"

int CALLBACK
ListView_SortCallback
(
    LPARAM                  nItem0,
    LPARAM                  nItem1,
    LPARAM                  lContext
)
{
    LPCLVSORTITEMDATA       pContext            = (LPCLVSORTITEMDATA)lContext;
    CHAR                    szText[2][0x400];
    INT                     nValue[2];
    int                     nResult;
    
    ListView_GetItemText(pContext->hwnd, nItem0, pContext->nSubItem, szText[0], NUMELMS(szText[0]));
    ListView_GetItemText(pContext->hwnd, nItem1, pContext->nSubItem, szText[1], NUMELMS(szText[1]));

    if(pContext->fString)
    {
        nResult = strcmp(szText[0], szText[1]);
    }
    else
    {
        nValue[0] = atoi(szText[0]);
        nValue[1] = atoi(szText[1]);

        nResult = nValue[0] - nValue[1];
    }        

    if(!pContext->fAscending)
    {
        nResult = -nResult;
    }

    if((nResult > 0) && pContext->pfnCallback)
    {
        pContext->pfnCallback(pContext->hwnd, nItem0, nItem1, pContext->pvContext);
    }

    return nResult;
}


/****************************************************************************
 *
 *  ListView_SortItems
 *
 *  Description:
 *      Sorts a list of string items.
 *
 *  Arguments:
 *      HWND [in]: list-view window handle.
 *      UINT [in]: sub-item to sort by.
 *      BOOL [in]: TRUE to sort as a string; FALSE to sort as a number.
 *      BOOL [in]: TRUE to sort items in ascending order; FALSE for 
 *                 descending.
 *      LPFNLVSORTITEMS [in]: callback function called when two items are
 *                            swapped.
 *      LPVOID [in]: callback context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_SortItems"

BOOL
ListView_SortItems
(
    HWND                    hwnd,
    UINT                    nSubItem,
    BOOL                    fString,
    BOOL                    fAscending,
    LPFNLVSORTITEMS         pfnCallback,
    LPVOID                  pvContext
)
{
    LVSORTITEMDATA          data;

    data.hwnd = hwnd;
    data.nSubItem = nSubItem;
    data.fString = fString;
    data.fAscending = fAscending;
    data.pfnCallback = pfnCallback;
    data.pvContext = pvContext;
    
    return ListView_SortItemsEx(hwnd, ListView_SortCallback, (LPARAM)&data);
}


/****************************************************************************
 *
 *  ListView_GetItemParam
 *
 *  Description:
 *      Gets the 32-bit data associated with a list-view item.
 *
 *  Arguments:
 *      HWND [in]: list-view window handle.
 *      UINT [in]: item index.
 *
 *  Returns:  
 *      LPARAM: item data or 0 on error.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "ListView_GetItemParam"

LPARAM
ListView_GetItemParam
(
    HWND                    hwnd, 
    UINT                    nItem
)
{
    LVITEM                  lvi;

    lvi.mask = LVIF_PARAM;
    lvi.iItem = nItem;
    lvi.iSubItem = 0;

    if(!ListView_GetItem(hwnd, &lvi))
    {
        return 0;
    }

    return lvi.lParam;
}


