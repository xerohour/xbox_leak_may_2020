/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wndwrap.h
 *  Content:    Windows wrapper classes (aka My Mini MFC)
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WNDWRAP_H__
#define __WNDWRAP_H__

#define WNDWRAP_USE_SET_WINDOW_LONG

//
// Window class names
//

#define XACTGUI_MAKE_WNDCLASS_NAME(name) \
    TEXT("XACT:GUI:" name)

//
// Common dialog helpers
//

#define MAKE_COMMDLG_FILTER(desc, ext) \
    desc TEXT(" Files (") ext TEXT(")\0") ext TEXT("\0")


//
// Private window messages
//

enum
{
    //
    // WM_USER_FORCERESIZE
    //
    // Private message used by MDI children.
    //  wParam == (void)
    //  lParam == (void)
    //  lResult == (void)
    //
    
    WM_USER_FORCERESIZE = WM_USER + 0xBFFF,

    //
    // WM_PROJECT_BUILDDONE
    //
    // Private message used by the project object to notify itself that
    // the build has completed.
    //  wParam == (void)
    //  lParam == build result code.
    //  lResult == (void)
    //

    WM_PROJECT_BUILDDONE,

    //
    // LVM_FIXITEMINDICES
    //
    // Private message used by ListView controls to notify themselves that
    // the item indices have changed and need to be updated.
    //  wParam == (void)
    //  lParam == (void)
    //  lResult == (void)
    //

    LVM_FIXITEMINDICES,
};

//
// Window types
//

enum
{
    WNDTYPE_WINDOW = 0,
    WNDTYPE_MDIFRAME,
    WNDTYPE_MDICLIENT,
    WNDTYPE_MDICHILD,
    WNDTYPE_DIALOG,
};

//
// Globals
//

EXTERN_C const HINSTANCE g_hInstance;
EXTERN_C TCHAR g_szAppTitle[0x100];

#ifdef __cplusplus

//
// Forward declarations
//

class CWindow;

//
// OnCreate context
//

BEGIN_DEFINE_STRUCT()
    DWORD       dwSignature;
    CWindow *   pWindow;
    LPVOID      pvContext;
END_DEFINE_STRUCT(WINDOWCREATECTX);

//
// Window wrapper class
//

class CWindow
{
public:
    const DWORD             m_dwSignature;          // Window signature
    const UINT              m_nWindowType;          // Window type
    CWindow *               m_pParent;              // Parent window
    LIST_ENTRY              m_lstChildren;          // Child window list
    HWND                    m_hWnd;                 // Window handle
    HMENU                   m_hMenu;                // Window menu
    HACCEL                  m_hAccel;               // Window accelerator table

protected:
    static const DWORD      m_dwValidSignature;     // Window signature
    BOOL                    m_fRedraw;              // Allowing redraws?

private:

#ifndef WNDWRAP_USE_SET_WINDOW_LONG

    static LIST_ENTRY       m_lstWindows;           // List of all windows
    LIST_ENTRY              m_leWindows;            // Window list entry

#endif // WNDWRAP_USE_SET_WINDOW_LONG

    LIST_ENTRY              m_leChildren;           // Child window list entry

public:
    CWindow(UINT nMenuId = -1, UINT nAccelId = -1, UINT nWindowType = WNDTYPE_WINDOW);
    virtual ~CWindow(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

    // Validity
    virtual BOOL IsWindow(void);

    // Window destruction
    virtual void Destroy(void);

    // Redraw
    virtual BOOL SetRedraw(BOOL fRedraw);

    // Messaging
    virtual LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
    virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
    virtual LRESULT SendDlgItemMessage(int nItemId, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
    virtual HWND GetDlgItem(int nItemId);

    // Window text
    virtual BOOL SetWindowText(LPCTSTR pszText);
    virtual BOOL SetDlgItemText(int nItemId, LPCTSTR pszString);
    virtual BOOL SetDlgItemInt(int nItemId, UINT nValue, BOOL fSigned);
    virtual BOOL GetDlgItemText(int nItemId, LPTSTR pszString, int nLength);
    virtual UINT GetDlgItemInt(int nItemId, LPBOOL pfTranslated, BOOL fSigned);
    
    // Window size
    virtual BOOL MoveWindow(int x, int y, int nWidth, int nHeight, BOOL fRepaint = TRUE);
    virtual BOOL MoveWindow(LPCRECT prcWindow, BOOL fRepaint = TRUE);
    virtual BOOL GetWindowRect(LPRECT prcWindow);
    virtual BOOL GetClientRect(LPRECT prcClient);

    // Screen coordinates
    virtual BOOL ScreenToClient(LPPOINT ppt);
    virtual BOOL ScreenToClient(LPRECT prc);

    // Drag-and-drop
    virtual void DragAcceptFiles(BOOL fAccept = TRUE);
    
    // Operators
    virtual operator HWND(void);
    static HWND GetHWND(CWindow *pWindow);

    // Message dispatching
    virtual BOOL DispatchMessage(LPMSG pMsg);

    // Basic message handlers (no modification of data)
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
    virtual BOOL OnSysCommand(UINT nCommandId, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);
    virtual BOOL OnEraseBackground(HDC hdc, LRESULT *plResult);
    virtual BOOL OnPaint(LRESULT *plResult);
    virtual BOOL OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult);
    virtual BOOL OnDropFiles(HDROP hDrop, LRESULT *plResult);

    // Advanced message handlers (handle stupid Windows stuff before handler)
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnScroll(UINT nScrollBarType, HWND hWndScrollBar, INT nPosition, LRESULT *plResult);
    virtual BOOL OnScroll(UINT nScrollBarType, UINT nRequest, HWND hWndScrollBar, LRESULT *plResult);

    // Default handler
    virtual LRESULT DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Main window procedure
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    // Creation (protected to force subclassing of the CWindow type)
    virtual BOOL Create(CWindow *pParent, LPCTSTR pszClassName, LPCTSTR pszWindowName, DWORD dwExStyle, DWORD dwStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, UINT nControlId = 0, LPVOID pvParam = NULL);

    // Window context
    static CWindow *GetWindowContext(HWND hWnd);

    // The first and final message handlers
    static CWindow *OnInitialCreate(HWND hWnd, LPARAM *plParam);
    
    // Window context
    static void SetWindowContext(HWND hWnd, CWindow *pWindow);
    void FreeWindowContext(void);
};

__inline HWND CWindow::GetHWND(CWindow *pWindow)
{
    return pWindow ? pWindow->m_hWnd : NULL;
}

__inline BOOL CWindow::OnCreate(LPVOID pvContext, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnClose(LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnDestroy(LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnSysCommand(UINT nCommandId, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnNotify(LPNMHDR pHdr, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnEraseBackground(HDC hdc, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnPaint(LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnScroll(UINT nScrollBarType, HWND hWndScrollBar, INT nPosition, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult)
{
    return FALSE;
}

__inline LRESULT CWindow::DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

__inline CWindow::operator HWND(void)
{
    ASSERT(this);
    return m_hWnd;
}

__inline BOOL CWindow::IsWindow(void)
{
    if(!m_hWnd)
    {
        return FALSE;
    }

    ASSERT(::IsWindow(m_hWnd));

    return TRUE;
}

__inline LRESULT CWindow::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

__inline LRESULT CWindow::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

__inline BOOL CWindow::SetWindowText(LPCTSTR pszText)
{
    return ::SetWindowText(m_hWnd, pszText);
}

__inline BOOL CWindow::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL fRepaint)
{
    return ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, fRepaint);
}

__inline BOOL CWindow::MoveWindow(LPCRECT prcWindow, BOOL fRepaint)
{
    return ::MoveWindow(m_hWnd, prcWindow->left, prcWindow->top, prcWindow->right - prcWindow->left, prcWindow->bottom - prcWindow->top, fRepaint);
}

__inline BOOL CWindow::GetWindowRect(LPRECT prcWindow)
{
    return ::GetWindowRect(m_hWnd, prcWindow);
}

__inline BOOL CWindow::GetClientRect(LPRECT prcClient)
{
    return ::GetClientRect(m_hWnd, prcClient);
}

__inline BOOL CWindow::ScreenToClient(LPPOINT ppt)
{
    return ::ScreenToClient(m_hWnd, ppt);
}

__inline BOOL CWindow::ScreenToClient(LPRECT prc)
{
    if(!::ScreenToClient(m_hWnd, (LPPOINT)&prc->left))
    {
        return FALSE;
    }

    return ::ScreenToClient(m_hWnd, (LPPOINT)&prc->right);
}

__inline LRESULT CWindow::SendDlgItemMessage(int nItemId, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendDlgItemMessage(m_hWnd, nItemId, uMsg, wParam, lParam);
}

__inline HWND CWindow::GetDlgItem(int nItemId)
{
    return ::GetDlgItem(m_hWnd, nItemId);
}

__inline BOOL CWindow::SetDlgItemText(int nItemId, LPCTSTR pszString)
{
    return ::SetDlgItemText(m_hWnd, nItemId, pszString);
}

__inline BOOL CWindow::SetDlgItemInt(int nItemId, UINT nValue, BOOL fSigned)
{
    return ::SetDlgItemInt(m_hWnd, nItemId, nValue, fSigned);
}

__inline BOOL CWindow::GetDlgItemText(int nItemId, LPTSTR pszString, int nLength)
{
    return ::GetDlgItemText(m_hWnd, nItemId, pszString, nLength);
}

__inline UINT CWindow::GetDlgItemInt(int nItemId, LPBOOL pfTranslated, BOOL fSigned)
{
    return ::GetDlgItemInt(m_hWnd, nItemId, pfTranslated, fSigned);
}

__inline void CWindow::DragAcceptFiles(BOOL fAccept)
{
    ::DragAcceptFiles(m_hWnd, fAccept);
}

__inline BOOL CWindow::OnDropFiles(HDROP hDrop, LRESULT *plResult)
{
    return FALSE;
}

//
// Function templates don't work
//

template <class type> class CWindowCast
{
public:
    static type *Cast(CWindow *pWindow);
};

template <class type> type *CWindowCast<type>::Cast(CWindow *pWindow)
{
    if(!pWindow)
    {
        return NULL;
    }

    ASSERT(type::WNDTYPE == pWindow->m_nWindowType);

    return (type *)pWindow;
}

#endif // __cplusplus

//
// Generic window helpers
//

__inline void BeginWaitCursor(void)
{
    SetCursor(LoadCursor(NULL, IDC_WAIT));
}

__inline void EndWaitCursor(void)
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

__inline HICON LoadResourceIconLarge(UINT nResourceId)
{
    return (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(nResourceId), IMAGE_ICON, 32, 32, LR_SHARED);
}

__inline HICON LoadResourceIconSmall(UINT nResourceId)
{
    return (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(nResourceId), IMAGE_ICON, 16, 16, LR_SHARED);
}

__inline HMENU LoadPopupMenu(UINT nResourceId)
{
    HMENU                   hMenu;

    hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(nResourceId));

    return GetSubMenu(hMenu, 0);
}

#endif // __WNDWRAP_H__
