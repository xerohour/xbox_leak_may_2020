/***************************************************************************
 *
 *  Copyright (C) 2/7/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       treeview.h
 *  Content:    Tree-view control wrapper class.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/7/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __TREEVIEW_H__
#define __TREEVIEW_H__

#ifdef __cplusplus

//
// Forward declarations
//

class CTreeView;
class CTreeViewType;
class CTreeViewItem;

//
// Tree-view child window
//

class CTreeViewCtrl
    : public CWindow
{
public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(CTreeView *pParent, DWORD dwExStyle, DWORD dwStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT);
};

__inline ULONG CTreeViewCtrl::AddRef(void)
{
    return 1;
}

__inline ULONG CTreeViewCtrl::Release(void)
{
    return 1;
}

//
// Tree-view wrapper object
//

class CTreeView
    : public CWindow
{
    friend class CTreeViewType;
    friend class CTreeViewItem;

public:
    static const LPCTSTR    m_pszClassName;     // Window class name
    CTreeViewCtrl           m_TreeCtrl;         // Tree-view child window

protected:
    HIMAGELIST              m_hImageList;       // Image list
    LIST_ENTRY              m_lstTypes;         // Registered types
    UINT                    m_nControlId;       // Control identifier
   
public:
    CTreeView(void);
    virtual ~CTreeView(void);

public:
    // Creation
    virtual BOOL Create(CWindow *pParent, DWORD dwExStyle, DWORD dwStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, UINT nControlId = 0);

    // Item types
    virtual BOOL RegisterType(LPCTSTR pszText, HICON hIcon);
    virtual CTreeViewType *FindRegisteredType(LPCTSTR pszText);

    // Items
    virtual CTreeViewItem *GetSelection(void);
    virtual CTreeViewItem *GetNextItem(CTreeViewItem *pItem, DWORD dwFlags);
    virtual CTreeViewItem *PtrFromTreeItem(HTREEITEM hItem);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);

    // Notification handlers
    virtual void OnDeleteItem(LPNMTREEVIEW pHdr);
    virtual void OnDoubleClick(LPNMHDR pHdr);
    virtual void OnGetInfoTip(LPNMTVGETINFOTIP pHdr);
    virtual BOOL OnEndLabelEdit(LPNMTVDISPINFO pHdr);
};

//
// Tree-view item type
//

class CTreeViewType
    : public CRefCount
{
public:
    LIST_ENTRY              m_leTypes;          // Registered type list entry
    TCHAR                   m_szName[0x100];    // Type name
    int                     m_nImageIndex;      // Image list index

public:
    CTreeViewType(CTreeView *pTree, LPCTSTR pszText, int nImageIndex);
    virtual ~CTreeViewType(void);
};

//
// Tree-view item
//

class CTreeViewItem
{
    friend class CTreeView;

public:
    CTreeView *             m_pTree;            // Parent tree object
    HTREEITEM               m_hTreeItem;        // Tree item handle
    LPVOID                  m_pvContext;        // Item context

public:
    CTreeViewItem(void);
    virtual ~CTreeViewItem(void);

public:
    // Creation/destruction
    virtual BOOL Create(CTreeView *pTree, CTreeViewItem *pParentItem, LPCTSTR pszText, LPCTSTR pszType, LPVOID pvContext = NULL);
    virtual void Remove(void);

    // Item properties
    virtual void SetText(LPCTSTR pszText);
    virtual HWND EditLabel(void);

    // Operators
    static CTreeViewItem *PtrFromTreeCtx(LPARAM lParam);
    static CTreeViewItem *PtrFromTreeItem(CTreeView *pTree, HTREEITEM hItem);

    // Message handlers
    virtual BOOL OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult);

    // Notification handlers
    virtual void OnDoubleClick(void);
    virtual void OnDelete(void);    
    virtual void OnGetInfoTip(LPTSTR pszText, int cchTextMax);
    virtual BOOL OnEndLabelEdit(LPCTSTR pszText);
};

__inline HWND CTreeViewItem::EditLabel(void)
{
    return TreeView_EditLabel(m_pTree->m_TreeCtrl.m_hWnd, m_hTreeItem);
}

__inline CTreeViewItem *CTreeViewItem::PtrFromTreeCtx(LPARAM lParam)
{
    return (CTreeViewItem *)lParam;
}

__inline void CTreeViewItem::OnDoubleClick(void)
{
}

__inline BOOL CTreeViewItem::OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult)
{
    return FALSE;
}

__inline void CTreeViewItem::OnGetInfoTip(LPTSTR pszText, int cchTextMax)
{
    *pszText = 0;
}

__inline BOOL CTreeViewItem::OnEndLabelEdit(LPCTSTR pszText)
{
    return FALSE;
}

#endif // __cplusplus

#endif // __PRJTREE_H_
