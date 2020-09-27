/***************************************************************************
 *
 *  Copyright (C) 2/7/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       listview.h
 *  Content:    List-view control wrapper class.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/7/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __LISTVIEW_H__
#define __LISTVIEW_H__

DEFINELPTYPE(NMLVDISPINFO);

//
// List-view column types
//

BEGIN_DEFINE_ENUM()
    LVCTYPE_STRING = 0,
    LVCTYPE_NUMBER,
END_DEFINE_ENUM(LVCTYPE);

//
// List-view column data
//

BEGIN_DEFINE_STRUCT()
    UINT    nHeaderStringId;
    LVCTYPE nDataType;
    BOOL    fSortAscending;
END_DEFINE_STRUCT(LVCOLUMNDATA);

//
// List-view sorting flags
//

#define LVSORT_TOGGLE_DIRECTION 0x00000001
#define LVSORT_FORCE_ASCENDING  0x00000002
#define LVSORT_FORCE_DECENDING  0x00000004

#ifdef __cplusplus

//
// Forward declarations
//

class CListView;
class CListViewType;
class CListViewItem;

//
// List view child window
//

class CListViewCtrl
    : public CWindow
{
public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(CListView *pParent, DWORD dwExStyle, DWORD dwStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT);
};

__inline ULONG CListViewCtrl::AddRef(void)
{
    return 1;
}

__inline ULONG CListViewCtrl::Release(void)
{
    return 1;
}

//
// List view wrapper
//

class CListView
    : public CWindow
{
    friend class CListViewType;
    friend class CListViewItem;

public:
    static const LPCTSTR    m_pszClassName;         // Window class name
    CListViewCtrl           m_ListCtrl;             // List view control

protected:                                          
    HIMAGELIST              m_hLargeImageList;      // Large icon image list
    HIMAGELIST              m_hSmallImageList;      // Small icon image list
    LIST_ENTRY              m_lstTypes;             // Registered types
    LVCOLUMNDATA            m_aColumnData[32];      // Column data
    UINT                    m_nColumnCount;         // Column count
    UINT                    m_nLastSortColumn;      // Last column the list was sorted by
   
public:
    CListView(void);
    virtual ~CListView(void);

public:
    // Creation
    virtual BOOL Create(CWindow *pParent, DWORD dwExStyle, DWORD dwStyle, DWORD dwListExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, UINT nControlId = 0);

    // Columns
    virtual BOOL InsertColumn(LPCTSTR pszHeader, LVCTYPE nType = LVCTYPE_STRING, BOOL fSortAscending = TRUE);
    virtual BOOL InsertColumns(LPCLVCOLUMNDATA paColumnData, UINT nCount);
    virtual void ResizeColumn(UINT nColumnIndex);
    virtual void ResizeColumns(void);

    // Items
    virtual int GetSelectedCount(void);
    virtual CListViewItem *GetNextItem(CListViewItem *pItem, DWORD dwFlags);
    virtual CListViewItem *PtrFromListIndex(int nItemIndex);
    virtual void SelectAll(BOOL fSelected);

    // Sorting
    virtual void Sort(UINT nColumnIndex, DWORD dwFlags);

    // Item types
    virtual BOOL RegisterType(LPCTSTR pszText, HICON hIconLarge, HICON hIconSmall);
    virtual CListViewType *FindRegisteredType(LPCTSTR pszText);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);

    // Notification handlers
    virtual void OnColumnClick(LPNMLISTVIEW pHdr);
    virtual void OnDeleteItem(LPNMLISTVIEW pHdr);
    virtual BOOL OnEndLabelEdit(LPNMLVDISPINFO pHdr);
    virtual void OnItemChanged(LPNMLISTVIEW pHdr);
    virtual void OnDoubleClick(LPNMHDR pHdr);

    // List view stupidity
    virtual void FixItemIndices(void);

private:
    // Sort callbacks
    static int CALLBACK StringSortCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    static int CALLBACK NumericSortCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

__inline int CListView::GetSelectedCount(void)
{
    return ListView_GetSelectedCount(m_ListCtrl.m_hWnd);
}

//
// List-view item type
//

class CListViewType
    : public CRefCount
{
public:
    LIST_ENTRY              m_leTypes;          // Registered type list entry
    TCHAR                   m_szName[0x100];    // Type name
    int                     m_nImageIndex;      // Image list index

public:
    CListViewType(CListView *pList, LPCTSTR pszText, int nImageIndex);
    virtual ~CListViewType(void);
};

//
// List-view item
//

class CListViewItem
{
    friend class CListView;

public:
    CListView *             m_pList;            // Parent list object
    int                     m_nItemIndex;       // List item index
    LPVOID                  m_pvContext;        // Item context

public:
    CListViewItem(void);
    virtual ~CListViewItem(void);

public:
    // Creation/destruction
    virtual BOOL Create(CListView *pList, LPCTSTR pszText, LPCTSTR pszType, LPVOID pvContext = NULL);
    virtual void Remove(void);

    // Item properties
    virtual void SetText(LPCTSTR pszText);
    virtual void SetColumnText(int nColumnIndex, LPCTSTR pszText);
    virtual void SetColumnValue(int nColumnIndex, int nValue, BOOL fSigned = TRUE, BOOL fFormat = FALSE);
    virtual HWND EditLabel(void);
    virtual void SetSelection(BOOL fSelected);

    // Operators
    static CListViewItem *PtrFromListCtx(LPARAM lParam);
    static CListViewItem *PtrFromListIndex(CListView *pList, int nItemIndex);

    // Event handlers
    virtual void OnDelete(void);
    virtual BOOL OnEndLabelEdit(LPCTSTR pszText);
    virtual void OnStateChanged(DWORD dwOldState, DWORD dwNewState);
    virtual void OnDoubleClick(void);

    // List helpers
    virtual int GetListIndex(void);
};

__inline void CListViewItem::SetText(LPCTSTR pszText)
{
    SetColumnText(0, pszText);
}

__inline HWND CListViewItem::EditLabel(void)
{
    return ListView_EditLabel(m_pList->m_ListCtrl.m_hWnd, m_nItemIndex);
}

__inline CListViewItem *CListViewItem::PtrFromListCtx(LPARAM lParam)
{
    return (CListViewItem *)lParam;
}

__inline CListViewItem *CListView::PtrFromListIndex(int nItemIndex)
{
    return CListViewItem::PtrFromListIndex(this, nItemIndex);
}

__inline BOOL CListViewItem::OnEndLabelEdit(LPCTSTR pszText)
{
    return FALSE;
}

__inline void CListViewItem::OnStateChanged(DWORD dwOldState, DWORD dwNewState)
{
}

__inline void CListViewItem::OnDoubleClick(void)
{
}

#endif // __cplusplus

#endif // __LISTVIEW_H__
