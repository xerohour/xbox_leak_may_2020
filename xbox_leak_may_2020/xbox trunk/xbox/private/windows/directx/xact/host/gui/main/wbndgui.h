/***************************************************************************
 *
 *  Copyright (C) 2/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wbndgui.h
 *  Content:    Wave Bundler GUI wrapper objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/13/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WBNDGUI_H__
#define __WBNDGUI_H__

#ifdef __cplusplus

//
// Forward declarations
//

class CGuiWaveBank;
class CWaveBankWindow;
class CGuiWaveBankProject;

//
// Wave bank entry
//

class CGuiWaveBankEntry
    : public CWaveBankEntry, public CListViewItem
{
public:
    static const LVCOLUMNDATA   m_aColumnData[];                // List-view column data

protected:
    TCHAR                       m_szFileExtension[MAX_PATH];    // File extension (used to type-define the entry in the list)

public:
    CGuiWaveBankEntry(CGuiWaveBank *pWaveBank);
    virtual ~CGuiWaveBankEntry(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCTSTR pszEntryName, LPCTSTR pszFileName, DWORD dwFlags);
    virtual HRESULT Load(BOOL fForce = FALSE);

    // Entry properties
    virtual HRESULT SetName(LPCTSTR pszName);
    virtual DWORD SetFlags(DWORD dwMask, DWORD dwFlags);

    // UI
    virtual void MakeDirty(void);
    virtual void UpdateUI(void);

    // Command handlers
    virtual void OnCmdPlay(void);

    // Notification handlers
    virtual BOOL OnEndLabelEdit(LPCTSTR pszText);
    virtual void OnStateChanged(DWORD dwOldState, DWORD dwNewState);
    virtual void OnDoubleClick(void);
};

__inline void CGuiWaveBankEntry::MakeDirty(void)
{
    g_pApplication->m_Project.MakeDirty();
    UpdateUI();
}

__inline void CGuiWaveBankEntry::OnDoubleClick(void)
{
    OnCmdPlay();
}

//
// Wave bank list-view
//

class CWaveBankListView
    : public CListView
{
public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Message handlers
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
};

__inline ULONG CWaveBankListView::AddRef(void)
{
    return 1;
}

__inline ULONG CWaveBankListView::Release(void)
{
    return 1;
}

__inline BOOL CWaveBankListView::OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult)
{
    if(CListView::OnCommand(nCommandId, nControlId, hWndControl, plResult))
    {
        return TRUE;
    }

    if(m_pParent)
    {
        return m_pParent->OnCommand(nCommandId, nControlId, hWndControl, plResult);
    }

    return FALSE;
}

//
// Wave bank list window
//

class CWaveBankWindow
    : public CWorkspaceChild
{
    friend class CGuiWaveBank;
    friend class CGuiWaveBankEntry;

public:
    enum
    {
        WKSPCTYPE = CWorkspaceChild::WKSPCTYPE_WAVEBANK
    };

    CWaveBankListView    m_ListView;         // List view
    CGuiWaveBank *          m_pBank;            // Bank object

protected:
    BOOL                    m_fUpdateCommands;  // Allow command updates?

public:
    CWaveBankWindow(void);
    virtual ~CWaveBankWindow(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);
    
    // Initialization
    virtual void Initialize(CGuiWaveBank *pBank);
    virtual BOOL Create(LPBOOL pfCreated = NULL);

    // List contents
    virtual BOOL Refresh(void);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
    virtual BOOL OnDropFiles(HDROP hDrop, LRESULT *plResult);

    // Command handlers
    virtual void OnCmdAddEntry(void);
    virtual void OnCmdRename(void);
    virtual void OnCmdRemove(void);
    virtual void OnCmdSetFlags(DWORD dwFilter);
    virtual void OnCmdPlay(void);
    virtual void OnCmdStop(void);
    virtual void OnCmdSelectAll(void);

    // Event handlers
    virtual void OnActivate(void);
    virtual void OnDeactivate(void);

    // Command state
    virtual BOOL EnableCommandUpdate(BOOL fEnable);
    virtual void UpdateCommandState(void);
};

__inline ULONG CWaveBankWindow::AddRef(void)
{
    return 1;
}

__inline ULONG CWaveBankWindow::Release(void)
{
    return 1;
}

__inline void CWaveBankWindow::Initialize(CGuiWaveBank *pBank)
{
    m_pBank = pBank;
}

__inline void CWaveBankWindow::OnActivate(void)
{
    UpdateCommandState();
}

__inline void CWaveBankWindow::OnDeactivate(void)
{
    UpdateCommandState();
}

//
// Wave bank
//

class CGuiWaveBank
    : public CWaveBank, public CMainProjectTreeItem
{
    friend class CWaveBankWindow;
    friend class CGuiWaveBankEntry;

protected:
    CWaveBankWindow  m_ListWindow;           // Bank list window

public:
    CGuiWaveBank(CGuiWaveBankProject *pProject);
    virtual ~CGuiWaveBank(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCTSTR pszBankName, LPCTSTR pszBankFile, LPCTSTR pszHeaderFile);

    // Bank properties
    virtual HRESULT SetName(LPCTSTR pszName);
    virtual HRESULT SetBankPath(LPCTSTR pszBankPath);
    virtual HRESULT SetHeaderPath(LPCTSTR pszHeaderPath);
    virtual HRESULT SetFlags(DWORD dwFlags);
    virtual HRESULT SetAlignment(DWORD dwAlignment);

    // UI
    virtual void MakeDirty(void);
    virtual BOOL OpenList(void);
    virtual void CloseList(void);
    virtual void UpdateUI(void);

    // Message handlers
    virtual BOOL OnContextMenu(HWND hWndFocus, int x, int y, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);

    // Command handlers
    virtual void OnCmdRefresh(void);
    virtual void OnCmdRename(void);
    virtual void OnCmdRemove(void);
    virtual void OnCmdProperties(void);

    // Notification handlers
    virtual void OnDoubleClick(void);
    virtual void OnGetInfoTip(LPTSTR pszText, int cchTextMax);
    virtual BOOL OnEndLabelEdit(LPCTSTR pszText);

protected:
    // Entry creation
    virtual CWaveBankEntry *CreateEntry(void);
};

__inline CWaveBankEntry *CGuiWaveBank::CreateEntry(void)
{
    return NEW(CGuiWaveBankEntry(this));
}

__inline void CGuiWaveBank::MakeDirty(void)
{
    g_pApplication->m_Project.MakeDirty();
    UpdateUI();
}

__inline void CGuiWaveBank::OnDoubleClick(void)
{
    OpenList();
}

//
// Wave bank properties
//

class CWaveBankPropertiesDialog
    : public CDialog
{
public:
    enum
    {
        IDD = IDD_WAVEBANK_PROPERTIES
    };

protected:
    CGuiWaveBank *          m_pBank;        // Wave bank

public:
    CWaveBankPropertiesDialog(CGuiWaveBank *pBank);
    virtual ~CWaveBankPropertiesDialog(void);

protected:
    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hControlWnd, LRESULT *plResult);

    // Command handlers
    virtual void OnCmdOK(void);
    virtual void OnCmdBrowseBankPath(void);
    virtual void OnCmdBrowseHeaderPath(void);
};

//
// Wave bank project
//

class CGuiWaveBankProject
    : public CWaveBankProject
{
public:
    CGuiWaveBankProject(void);
    virtual ~CGuiWaveBankProject(void);

public:
    // File browsing
    static BOOL BrowseBankFile(HWND hWnd, LPTSTR pszFile, UINT nLength);
    static BOOL BrowseHeaderFile(HWND hWnd, LPTSTR pszFile, UINT nLength);
    static BOOL BrowseEntryFile(HWND hWnd, LPTSTR pszFile, UINT nLength);

protected:    
    // Bank creation
    virtual CWaveBank *CreateBank(void);

private:
    // Wave bank entry OFN hook procedure
    static UINT CALLBACK WaveBankEntryOFNHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__inline CWaveBank *CGuiWaveBankProject::CreateBank(void)
{
    return NEW(CGuiWaveBank(this));
}

#endif // __cplusplus

#endif // __WBNDGUI_H__
