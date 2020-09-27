/***************************************************************************
 *
 *  Copyright (C) 2/7/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       treeview.cpp
 *  Content:    Tree-view control wrapper class.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/7/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactctl.h"

const LPCTSTR CTreeView::m_pszClassName = XACTGUI_MAKE_WNDCLASS_NAME("TreeView");


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the window.
 *
 *  Arguments:
 *      CTreeView * [in]: parent window.
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
#define DPF_FNAME "CTreeViewCtrl::Create"

BOOL
CTreeViewCtrl::Create
(
    CTreeView *             pParent, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight
)
{
    return CWindow::Create(pParent, WC_TREEVIEW, NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight);
}


/****************************************************************************
 *
 *  CTreeView
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
#define DPF_FNAME "CTreeView::CTreeView"

CTreeView::CTreeView
(
    void
)
{
    //
    // Initialize the type list
    //
    
    InitializeListHead(&m_lstTypes);

    //
    // Create the image list
    //

    m_hImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, 0x100);
}


/****************************************************************************
 *
 *  ~CTreeView
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
#define DPF_FNAME "CTreeView::~CTreeView"

CTreeView::~CTreeView
(
    void
)
{
    CTreeViewType *         pType;
    PLIST_ENTRY             pleEntry;
    
    //
    // Free registered types
    //

    while((pleEntry = RemoveEntryList(m_lstTypes.Flink)) != &m_lstTypes)
    {
        pType = CONTAINING_RECORD(pleEntry, CTreeViewType, m_leTypes);

        RELEASE(pType);
    }

    //
    // Free the image list
    //

    ImageList_Destroy(m_hImageList);
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
#define DPF_FNAME "CTreeView::Create"

BOOL
CTreeView::Create
(
    CWindow *               pParent, 
    DWORD                   dwExStyle, 
    DWORD                   dwStyle, 
    int                     x, 
    int                     y, 
    int                     nWidth, 
    int                     nHeight, 
    UINT                    nControlId
)
{
    WNDCLASSEX              wc              = { 0 };
    DWORD                   dwTreeStyle;

    //
    // Mask off and save the tree-view specific style bits
    //

    dwTreeStyle = LOWORD(dwStyle);
    dwStyle &= ~dwTreeStyle;

    //
    // Save the control identifier
    //

    m_nControlId = nControlId;

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

    return CWindow::Create(pParent, m_pszClassName, NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight, nControlId, (LPVOID)dwTreeStyle);
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
#define DPF_FNAME "CTreeView::OnCreate"

BOOL
CTreeView::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    static const DWORD      dwExStyle   = 0;
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE | TVS_INFOTIP;
    const DWORD             dwTreeStyle = (DWORD)pvContext;
    BOOL                    fSuccess;

    if(CWindow::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }
    
    //
    // Create the tree-view control
    //

    fSuccess = m_TreeCtrl.Create(this, dwExStyle, dwStyle | dwTreeStyle);
    
    //
    // Set the image list
    //

    if(fSuccess)
    {
        ASSERT(m_hImageList);
        
        TreeView_SetImageList(m_TreeCtrl, m_hImageList, TVSIL_NORMAL);
        
        ImageList_SetBkColor(m_hImageList, TreeView_GetBkColor(m_TreeCtrl));
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
#define DPF_FNAME "CTreeView::OnDestroy"

BOOL
CTreeView::OnDestroy
(
    LRESULT *               plResult
)
{
    if(CWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    TreeView_DeleteAllItems(m_TreeCtrl.m_hWnd);

    return FALSE;
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
#define DPF_FNAME "CTreeView::OnSize"

BOOL
CTreeView::OnSize
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
    
    m_TreeCtrl.MoveWindow(0, 0, nWidth, nHeight);

    return FALSE;
}


/****************************************************************************
 *
 *  OnContextMenu
 *
 *  Description:
 *      Handles WM_CONTEXTMENU messages.
 *
 *  Arguments:
 *      HWND [in]: window the user clicked in.
 *      int [in]: mouse x-coordinate.
 *      int [in]: mouse y-coordinate.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::OnContextMenu"

BOOL
CTreeView::OnContextMenu
(
    HWND                    hWndFocus,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    TVHITTESTINFO           htti    = { 0 };
    CTreeViewItem *         pItem;
    
    if(CWindow::OnContextMenu(hWndFocus, x, y, plResult))
    {
        return TRUE;
    }

    htti.pt.x = x;
    htti.pt.y = y;

    m_TreeCtrl.ScreenToClient(&htti.pt);

    TreeView_HitTest(m_TreeCtrl, &htti);

    if(!htti.hItem)
    {
        return FALSE;
    }

    TreeView_Select(m_TreeCtrl, htti.hItem, TVGN_CARET);
    
    pItem = pItem->PtrFromTreeItem(this, htti.hItem);

    return pItem->OnContextMenu(hWndFocus, x, y, plResult);
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
#define DPF_FNAME "CTreeView::OnNotify"

BOOL
CTreeView::OnNotify
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
        case TVN_DELETEITEM:
            OnDeleteItem((LPNMTREEVIEW)pHdr);
            break;

        case NM_DBLCLK:
        case NM_RETURN:
            OnDoubleClick(pHdr);
            break;

        case TVN_GETINFOTIP:
            OnGetInfoTip((LPNMTVGETINFOTIP)pHdr);
            break;

        case TVN_ENDLABELEDIT:
            *plResult = OnEndLabelEdit((LPNMTVDISPINFO)pHdr);
            return TRUE;
    };

    return FALSE;
}


/****************************************************************************
 *
 *  OnDeleteItem
 *
 *  Description:
 *      Handles TVN_DELETEITEM notifications.
 *
 *  Arguments:
 *      LPNMTREEVIEW [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::OnDeleteItem"

void
CTreeView::OnDeleteItem
(
    LPNMTREEVIEW            pHdr
)
{
    CTreeViewItem *         pItem   = CTreeViewItem::PtrFromTreeCtx(pHdr->itemOld.lParam);

    pItem->OnDelete();
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
#define DPF_FNAME "CTreeView::OnDoubleClick"

void
CTreeView::OnDoubleClick
(
    LPNMHDR                 pHdr
)
{
    CTreeViewItem *         pItem;

    //
    // Find the selected item or items and call the activation callback
    //

    if(!(pItem = GetSelection()))
    {
        return;
    }

    pItem->OnDoubleClick();
}


/****************************************************************************
 *
 *  OnGetInfoTip
 *
 *  Description:
 *      Handles TVN_GETINFOTIP notifications.
 *
 *  Arguments:
 *      LPNMTVGETINFOTIP [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::OnGetInfoTip"

void
CTreeView::OnGetInfoTip
(
    LPNMTVGETINFOTIP        pHdr
)
{
    CTreeViewItem *         pItem   = CTreeViewItem::PtrFromTreeCtx(pHdr->lParam);

    ASSERT(pItem);
    pItem->OnGetInfoTip(pHdr->pszText, pHdr->cchTextMax);
}


/****************************************************************************
 *
 *  OnEndLabelEdit
 *
 *  Description:
 *      Handles TVN_ENDLABELEDIT notifications.
 *
 *  Arguments:
 *      LPNMTVDISPINFO [in]: notification header.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::OnEndLabelEdit"

BOOL
CTreeView::OnEndLabelEdit
(
    LPNMTVDISPINFO          pHdr
)
{
    CTreeViewItem *         pItem   = CTreeViewItem::PtrFromTreeCtx(pHdr->item.lParam);

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
#define DPF_FNAME "CTreeView::RegisterType"

BOOL
CTreeView::RegisterType
(
    LPCTSTR                 pszText,
    HICON                   hIcon
)
{
    CTreeViewType *         pType;
    int                     nImageIndex;

    ASSERT(pszText);
    ASSERT(hIcon);

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

    if(-1 == (nImageIndex = ImageList_AddIcon(m_hImageList, hIcon)))
    {
        return FALSE;
    }

    //
    // Add the type to the list
    //
    
    if(!(pType = NEW(CTreeViewType(this, pszText, nImageIndex))))
    {
        return FALSE;
    }

    return TRUE;
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
 *      CTreeViewType *: registered type object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::FindRegisteredType"

CTreeViewType *
CTreeView::FindRegisteredType
(
    LPCTSTR                 pszText
)
{
    PLIST_ENTRY             pleEntry;
    CTreeViewType *         pType;

    ASSERT(pszText);
    
    for(pleEntry = m_lstTypes.Flink; pleEntry != &m_lstTypes; pleEntry = pleEntry->Flink)
    {
        pType = CONTAINING_RECORD(pleEntry, CTreeViewType, m_leTypes);

        if(!_tcsicmp(pszText, pType->m_szName))
        {
            return pType;
        }
    }

    return NULL;
}


/****************************************************************************
 *
 *  GetSelection
 *
 *  Description:
 *      Gets the currently selected tree item.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      CTreeViewItem *: selected item.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::GetSelection"

CTreeViewItem *
CTreeView::GetSelection
(
    void
)
{
    HTREEITEM               hTreeItem;
    
    if(!(hTreeItem = TreeView_GetSelection(m_TreeCtrl)))
    {
        return NULL;
    }

    return CTreeViewItem::PtrFromTreeItem(this, hTreeItem);
}


/****************************************************************************
 *
 *  GetNextItem
 *
 *  Description:
 *      Gets the next related item.
 *
 *  Arguments:
 *      CTreeViewItem * [in]: current item, or NULL.
 *      DWORD [in]: relationship flags.
 *
 *  Returns:  
 *      CTreeViewItem *: next item.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::GetNextItem"

CTreeViewItem *
CTreeView::GetNextItem
(
    CTreeViewItem *         pItem,
    DWORD                   dwFlags
)
{
    HTREEITEM               hItem;

    hItem = TreeView_GetNextItem(m_TreeCtrl.m_hWnd, pItem ? pItem->m_hTreeItem : NULL, dwFlags);

    if(!hItem)
    {
        return NULL;
    }

    return PtrFromTreeItem(hItem);
}


/****************************************************************************
 *
 *  PtrFromTreeItem
 *
 *  Description:
 *      Converts a tree-view item handle to a CTreeViewItem object pointer.
 *
 *  Arguments:
 *      HTREEITEM [in]: item handle.
 *
 *  Returns:  
 *      CTreeViewItem *: object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeView::PtrFromTreeItem"

CTreeViewItem *
CTreeView::PtrFromTreeItem
(
    HTREEITEM               hItem
)
{
    TVITEM                  tvi;

    tvi.mask = TVIF_PARAM;
    tvi.hItem = hItem;

    if(!TreeView_GetItem(m_TreeCtrl.m_hWnd, &tvi))
    {
        return NULL;
    }

    return CTreeViewItem::PtrFromTreeCtx(tvi.lParam);
}


/****************************************************************************
 *
 *  CTreeViewType
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CTreeView * [in]: parent tree.
 *      LPCTSTR [in]: type name.
 *      int [in]: image list index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeViewType::CTreeViewType"

CTreeViewType::CTreeViewType
(
    CTreeView *             pTree,
    LPCTSTR                 pszText,
    int                     nImageIndex
)
{
    ASSERT(pTree);
    ASSERT(pszText);
    
    InsertTailListUninit(&pTree->m_lstTypes, &m_leTypes);

    _tcscpy(m_szName, pszText);

    m_nImageIndex = nImageIndex;
}


/****************************************************************************
 *
 *  ~CTreeViewType
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
#define DPF_FNAME "CTreeViewType::~CTreeViewType"

CTreeViewType::~CTreeViewType
(
    void
)
{
    RemoveEntryList(&m_leTypes);
}


/****************************************************************************
 *
 *  CTreeViewItem
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
#define DPF_FNAME "CTreeViewItem::CTreeViewItem"

CTreeViewItem::CTreeViewItem
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CTreeViewItem
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
#define DPF_FNAME "CTreeViewItem::~CTreeViewItem"

CTreeViewItem::~CTreeViewItem
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
 *      Creates the item.
 *
 *  Arguments:
 *      CTreeView * [in]: tree control.
 *      CTreeViewItem * [in]: parent item.
 *      LPCTSTR [in]: item name.
 *      LPCTSTR [in]: item type.
 *      LPVOID [in]: item context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeViewItem::Create"

BOOL
CTreeViewItem::Create
(
    CTreeView *             pTree,
    CTreeViewItem *         pParentItem,
    LPCTSTR                 pszText,
    LPCTSTR                 pszType,
    LPVOID                  pvContext
)
{
    TVINSERTSTRUCT          tvi     = { 0 };
    CTreeViewType *         pType;

    ASSERT(pTree);
    ASSERT(!m_hTreeItem);

    m_pTree = pTree;
    m_pvContext = pvContext;

    tvi.hParent = pParentItem ? pParentItem->m_hTreeItem : NULL;
    tvi.hInsertAfter = TVI_SORT;
    
    tvi.item.mask = TVIF_PARAM;
    tvi.item.lParam = (LPARAM)this;

    if(pszText)
    {
        tvi.item.mask |= TVIF_TEXT;
        tvi.item.pszText = (LPTSTR)pszText;
    }

    if(pszType)
    {
        if(pType = pTree->FindRegisteredType(pszType))
        {
            tvi.item.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
            tvi.item.iImage = pType->m_nImageIndex;
            tvi.item.iSelectedImage = pType->m_nImageIndex;
        }
        else
        {
            DPF_ERROR("Type not registered");
        }
    }

    return MAKEBOOL(m_hTreeItem = TreeView_InsertItem(pTree->m_TreeCtrl, &tvi));
}


/****************************************************************************
 *
 *  Remove
 *
 *  Description:
 *      Removes the item from the tree.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeViewItem::Remove"

void
CTreeViewItem::Remove
(
    void
)
{
    if(m_hTreeItem && m_pTree)
    {
        TreeView_DeleteItem(m_pTree->m_TreeCtrl, m_hTreeItem);
    }
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
#define DPF_FNAME "CTreeViewItem::OnDelete"

void
CTreeViewItem::OnDelete
(
    void
)
{
    m_hTreeItem = NULL;
    m_pTree = NULL;
    m_pvContext = NULL;
}


/****************************************************************************
 *
 *  SetText
 *
 *  Description:
 *      Sets the item text.
 *
 *  Arguments:
 *      LPCTSTR [in]: text.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeViewItem::SetText"

void
CTreeViewItem::SetText
(
    LPCTSTR                 pszText
)
{
    TVITEM                  tvi;

    if(!m_pTree || !m_hTreeItem)
    {
        return;
    }

    tvi.mask = TVIF_TEXT;
    tvi.hItem = m_hTreeItem;
    tvi.pszText = (LPTSTR)pszText;

    TreeView_SetItem(m_pTree->m_TreeCtrl, &tvi);
}


/****************************************************************************
 *
 *  PtrFromTreeItem
 *
 *  Description:
 *      Converts a tree-view item handle to a CTreeViewItem object pointer.
 *
 *  Arguments:
 *      CTreeView * [in]: tree view.
 *      HTREEITEM [in]: item handle.
 *
 *  Returns:  
 *      CTreeViewItem *: object.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CTreeViewItem::PtrFromTreeItem"

CTreeViewItem *
CTreeViewItem::PtrFromTreeItem
(
    CTreeView *             pTree,
    HTREEITEM               hItem
)
{
    return pTree->PtrFromTreeItem(hItem);
}


