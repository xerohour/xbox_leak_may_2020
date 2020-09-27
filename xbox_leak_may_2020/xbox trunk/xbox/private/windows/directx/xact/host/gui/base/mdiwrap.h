/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mdiwrap.h
 *  Content:    Windows wrapper classes (aka My Mini MFC)
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __MDIWRAP_H__
#define __MDIWRAP_H__

#ifdef __cplusplus

//
// Forward declarations
//

class CMDIFrameWindow;
class CMDIClientWindow;
class CMDIChildWindow;

//
// MDI frame window wrapper class
//

class CMDIFrameWindow
    : public CWindow
{
    friend class CMDIClientWindow;

public:
    enum
    {
        WNDTYPE = WNDTYPE_MDIFRAME
    };

protected:
    CMDIClientWindow *      m_pMDIClient;       // MDI client window

public:
    CMDIFrameWindow(UINT nMenuId = 0, UINT nAccelId = 0);
    virtual ~CMDIFrameWindow(void);

protected:
    // Creation (protected to force subclassing of the CWindow type)
    virtual BOOL Create(LPCTSTR pszClassName, LPCTSTR pszWindowName, DWORD dwExStyle, DWORD dwStyle, int x, int y, int nWidth, int nHeight, LPVOID pvParam = NULL);

    // Default handler
    virtual LRESULT DefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//
// MDI client window wrapper class
//

class CMDIClientWindow
    : public CWindow
{
    friend class CMDIFrameWindow;
    friend class CMDIChildWindow;

public:
    enum
    {
        WNDTYPE = WNDTYPE_MDICLIENT
    };

public:
    CMDIClientWindow(void);
    virtual ~CMDIClientWindow(void);

public:
    // Child windows
    virtual BOOL GetActiveChild(CMDIChildWindow **ppWindow = NULL, LPBOOL pfMaximized = NULL);
    virtual BOOL IsActiveChild(CMDIChildWindow *pWindow);

protected:
    // Creation (protected to force subclassing of the CWindow type)
    virtual BOOL Create(CMDIFrameWindow *pParent, DWORD dwExStyle, DWORD dwStyle, int x, int y, int nWidth, int nHeight, UINT nControlId, const CLIENTCREATESTRUCT *pccs);

    // Message dispatching
    virtual BOOL DispatchMessage(LPMSG pMsg);
};

//
// MDI child window wrapper class
//

class CMDIChildWindow
    : public CWindow
{
public:
    enum
    {
        WNDTYPE = WNDTYPE_MDICHILD
    };

public:
    CMDIChildWindow(void);
    virtual ~CMDIChildWindow(void);

protected:
    // Creation (protected to force subclassing of the CWindow type)
    virtual BOOL Create(CMDIClientWindow *pParent, LPCTSTR pszClassName, LPCTSTR pszWindowName, DWORD dwExStyle, DWORD dwStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, LPVOID pvParam = NULL);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnStyleChanged(UINT nType, LPSTYLESTRUCT pStyle, LRESULT *plResult);
    virtual BOOL OnForceResize(LRESULT *plResult);
    virtual BOOL OnMdiActivate(HWND hWndInactive, HWND hWndActive, LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);

    // Event handlers
    virtual void OnActivate(void);
    virtual void OnDeactivate(void);

    // Default handler
    virtual LRESULT DefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__inline void CMDIChildWindow::OnActivate(void)
{
}

__inline void CMDIChildWindow::OnDeactivate(void)
{
}

#endif // __cplusplus

#endif // __MDIWRAP_H__
