/***************************************************************************
 *
 *  Copyright (C) 2/20/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dlgwrap.h
 *  Content:    Dialog wrapper classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/20/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __DLGWRAP_H__
#define __DLGWRAP_H__

#ifdef __cplusplus

//
// Dialog base class
//

class CDialog
    : public CWindow
{
public:
    enum
    {
        WNDTYPE = WNDTYPE_DIALOG
    };

    const UINT              m_nDialogResourceId;

public:
    CDialog(UINT nDialogResourceId);
    virtual ~CDialog(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(CWindow *pParent, LPVOID pvParam = NULL);
    virtual BOOL DoModal(CWindow *pParent, LPVOID pvParam = NULL);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);

    // Command handlers
    virtual void OnCmdOK(void);
    virtual void OnCmdCancel(void);

    // Default handler
    virtual LRESULT DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    // Main window procedure
    static int CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // The first and final message handlers
    static CWindow *OnInitialCreate(HWND hWnd, LPARAM *plParam);
};

__inline ULONG CDialog::AddRef(void)
{
    return 1;
}

__inline ULONG CDialog::Release(void)
{
    return 1;
}

__inline LRESULT CDialog::DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

#endif // __cplusplus

#endif // __DLGWRAP_H__
