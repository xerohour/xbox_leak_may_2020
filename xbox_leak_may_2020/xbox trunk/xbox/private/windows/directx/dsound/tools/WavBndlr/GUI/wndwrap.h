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

#define WINDOW_SIGNATURE 'wdnw'

// 
// Globals
//

EXTERN_C LPCSTR g_pszAppTitle;
EXTERN_C HINSTANCE g_hInstance;

//
// Debugging
//

#define WM_DPF WM_APP
#define WM_DPF_SIGNATURE 0xDF

//
// Window creation properties
//

BEGIN_DEFINE_STRUCT()
    DWORD       dwClassStyle;       // Window class style
    DWORD       dwClassExtra;       // Extra amount of memory allocated for the class
    DWORD       dwWndExtra;         // Extra amount of memory allocated for the window
    HICON       hIcon;              // Icon
    HICON       hIconSm;            // 16x16 icon
    HCURSOR     hCursor;            // Class cursor
    HBRUSH      hbrBackground;      // Class background color
    LPCTSTR     pszClassName;       // Class name
    LPCTSTR     pszWindowName;      // window name
    DWORD       dwStyle;            // window style
    DWORD       dwExStyle;          // extended window style
    int         x;                  // horizontal position of window
    int         y;                  // vertical position of window
    int         nWidth;             // window width
    int         nHeight;            // window height
    LPCSTR      pszMenu;            // menu resource identifier
    LPCSTR      pszAccelerator;     // accelerator table resource identifier
END_DEFINE_STRUCT(WNDCREATE);

//
// Generic helper functions
//

EXTERN_C BOOL FormatStringResource(LPTSTR pszText, HINSTANCE hInstance, UINT nResourceId, ...);
EXTERN_C void CenterWindow(HWND hwnd, HWND hwndParent);

#ifdef __cplusplus

//
// Forward declarations
//

class CWindow;

//
// Window base class
//

class CWindow
{
    friend class CDialog;

public:
    const DWORD             m_dwSignature;  // Validity signature
    CWindow *               m_pParent;      // Parent window handle
    LIST_ENTRY              m_lstChildren;  // Child window list
    LIST_ENTRY              m_leChild;      // Child window list entry
    HWND                    m_hwnd;         // Window handle
    HMENU                   m_hmenu;        // Menu handle
    HACCEL                  m_haccel;       // Accelerator table handle
    BOOL                    m_fEnableAccel; // Enable accelerators?

public:
    CWindow(void);
    virtual ~CWindow(void);

public:
    // Creation
    virtual BOOL Create(LPCWNDCREATE pCreate, LPVOID pvContext = NULL, CWindow *pParent = NULL);

    // Window heirarchy
    virtual void SetParent(CWindow *pParent);

    // Window size
    virtual void MoveWindow(const RECT &rc);
    virtual void MoveWindow(HWND hwnd, const RECT &rc);

    // User prompt
    virtual int MessageBox(LPCTSTR pszText, UINT uType);
    virtual int MessageBox(UINT nResourceId, UINT uType);

    // Message pump
    virtual void PumpMessages(void);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnSize(DWORD dwResize, int nWidth, int nHeight, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);
    virtual BOOL OnDebug(UINT nSignature, UINT nLevel, LPCSTR pszMessage, LRESULT *plResult);
    virtual BOOL OnContextMenu(HWND hwnd, int x, int y, LRESULT *plResult);
    virtual BOOL OnMenuSelect(UINT nMenuItem, UINT nFlags, HMENU hMenu, LRESULT *plResult);

    // Accelerator state
    virtual void EnableAccelerators(BOOL fEnable);

    // Debug helpers
    static void CALLBACK DebugCallback(DWORD dwLevel, LPCSTR pszString);

public:
    virtual operator HWND(void);

protected:
    // Message dispatcher
    virtual BOOL DispatchMessage(LPMSG pmsg);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT DefaultMessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__inline CWindow::operator HWND(void)
{
    return m_hwnd;
}

__inline LRESULT CWindow::DefaultMessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

__inline BOOL CWindow::OnClose(LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnDestroy(LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnSize(DWORD dwResize, int nWidth, int nHeight, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnNotify(LPNMHDR pHdr, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnContextMenu(HWND hwnd, int x, int y, LRESULT *plResult)
{
    return FALSE;
}

__inline BOOL CWindow::OnMenuSelect(UINT nMenuItem, UINT nFlags, HMENU hMenu, LRESULT *plResult)
{
    return FALSE;
}

__inline void CWindow::MoveWindow(const RECT &rc)
{
    MoveWindow(m_hwnd, rc);
}

__inline void CWindow::MoveWindow(HWND hwnd, const RECT &rc)
{
    ::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

__inline int CWindow::MessageBox(LPCTSTR pszText, UINT uType)
{
    return ::MessageBox(m_hwnd, pszText, g_pszAppTitle, uType);
}

__inline int CWindow::MessageBox(UINT nResourceId, UINT uType)
{
    TCHAR                   szText[0x100];

    LoadString(g_hInstance, nResourceId, szText, NUMELMS(szText));
    
    return MessageBox(szText, uType);
}

__inline void CWindow::EnableAccelerators(BOOL fEnable)
{
    m_fEnableAccel = fEnable;
}

__inline void CWindow::DebugCallback(DWORD dwLevel, LPCSTR pszString)
{
    SendMessage(g_hwndMain, WM_DPF, MAKELONG(dwLevel, WM_DPF_SIGNATURE), (LPARAM)pszString);
}

//
// Dialog base class
//

class CDialog
    : public CWindow
{
public:
    CDialog(void);
    virtual ~CDialog(void);

public:
    // Message handlers
    virtual BOOL OnInitDialog(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);

    // Command handlers
    virtual void OnOk(void);
    virtual void OnCancel(void);

protected:
    // Message dispatcher
    virtual BOOL DispatchMessage(LPMSG pmsg);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT DefaultMessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__inline LRESULT CDialog::DefaultMessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

__inline BOOL CDialog::OnInitDialog(LPVOID pvContext, LRESULT *plResult)
{
    return TRUE;
}

__inline BOOL CDialog::OnClose(LRESULT *plResult)
{
    OnCancel();
    return FALSE;
}

__inline void CDialog::OnOk(void)
{
    EndDialog(m_hwnd, TRUE);
}

__inline void CDialog::OnCancel(void)
{
    EndDialog(m_hwnd, FALSE);
}

//
// Modal dialog base class
//

class CModalDialog
    : public CDialog
{
public:
    CModalDialog(void);
    virtual ~CModalDialog(void);

public:
    // Creation
    virtual BOOL Create(UINT nResourceId, LPVOID pvContext = NULL, CWindow *pParent = NULL);
};

//
// Modeless dialog base class
//

class CModelessDialog
    : public CDialog
{
public:
    CModelessDialog(void);
    virtual ~CModelessDialog(void);

public:
    // Creation
    virtual HWND Create(UINT nResourceId, LPVOID pvContext = NULL, CWindow *pParent = NULL);
};

#endif // __cplusplus

#endif // __WNDWRAP_H__

