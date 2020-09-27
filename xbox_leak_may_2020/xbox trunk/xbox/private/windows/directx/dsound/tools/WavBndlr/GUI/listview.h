/***************************************************************************
 *
 *  Copyright (C) 11/12/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       listview.h
 *  Content:    Misc. list-view helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/12/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __LISTVIEW_H__
#define __LISTVIEW_H__

#undef ListView_SetItemText
#undef ListView_SortItems

//
// ListView_SortItems callback
//

typedef void (CALLBACK *LPFNLVSORTITEMS)(HWND hwnd, UINT nItem1, UINT nItem2, LPVOID pvContext);

//
// ListView helper functions
//

EXTERN_C LPARAM ListView_GetItemParam(HWND hwnd, UINT nItem);
EXTERN_C void ListView_SelectItem(HWND hwnd, UINT nItem, BOOL fSelected, BOOL fSingleSelect);
EXTERN_C void ListView_ResizeColumns(HWND hwndLV, int cx);
EXTERN_C void ListView_AutoSizeColumn(HWND hwnd, UINT nColumn);
EXTERN_C void ListView_AutoSizeColumns(HWND hwnd);
EXTERN_C BOOL ListView_SetItemText(HWND hwnd, UINT nItem, UINT nSubItem, LPCSTR pszFormat, ...);
EXTERN_C BOOL ListView_SortItems(HWND hwnd, UINT nSubItem, BOOL fString, BOOL fAscending, LPFNLVSORTITEMS pfnCallback, LPVOID pvContext);

#endif // __LISTVIEW_H__
