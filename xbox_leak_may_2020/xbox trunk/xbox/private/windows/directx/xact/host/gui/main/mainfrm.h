/***************************************************************************
 *
 *  Copyright (C) 2/4/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mainfrm.h
 *  Content:    Main window frame.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/4/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#ifdef __cplusplus

//
// Forward declarations
//

class CMainFrame;
class CWorkspaceChild;
class CGuiWaveBankListWindow;
class CMainProjectTree;

//
// Main status bar
//

class CMainStatusBar
    : public CWindow
{
public:
    CMainStatusBar(void);
    virtual ~CMainStatusBar(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(void);
};

__inline ULONG CMainStatusBar::AddRef(void)
{
    return 1;
}

__inline ULONG CMainStatusBar::Release(void)
{
    return 1;
}

//
// Main toolbar
//

class CMainToolBar
    : public CToolBar
{
public:
    
    //
    // Toolbar bitmap images
    //

    enum
    {
        BTN_WAVEBANK = 0,
        BTN_WAVEBANK_ENTRY,
        BTN_BUILD,
        BTN_ADPCM,
        BTN_8BIT,
        BTN_PLAY,
        BTN_STOP,
        BTN_CONNECT,
        BTN_DISCONNECT,
        BTN_COUNT
    };

public:
    CMainToolBar(void);
    virtual ~CMainToolBar(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(void);
};

__inline ULONG CMainToolBar::AddRef(void)
{
    return 1;
}

__inline ULONG CMainToolBar::Release(void)
{
    return 1;
}

//
// Project tree item
//

class CMainProjectTreeItem
    : public CTreeViewItem
{
public:
    CMainProjectTreeItem(void);
    virtual ~CMainProjectTreeItem(void);

public:
    // Creation
    virtual BOOL Create(CMainProjectTree *pTree, CMainProjectTreeItem *pParentItem, LPCTSTR pszText, LPCTSTR pszType, LPVOID pvContext = NULL);

    // Message handlers
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);
};

__inline BOOL CMainProjectTreeItem::OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult)
{
    return FALSE;
}

//
// Project tree category item
//

class CMainProjectTreeCategory
    : public CMainProjectTreeItem
{
public:
    CMainProjectTreeCategory(void);
    virtual ~CMainProjectTreeCategory(void);

public:
    // Creation
    virtual BOOL Create(CMainProjectTree *pTree, UINT nStringId, UINT nIconId);
};

//
// Project tree object
//

class CMainProjectTree
    : public CTreeView
{
public:
    static const LPCTSTR        m_pszWaveBankType;      // Tree control registered type name
    CMainProjectTreeCategory    m_WaveBundlerCategory;  // Wave Bundler category item

public:
    CMainProjectTree(void);
    virtual ~CMainProjectTree(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(void);

    // Message handlers
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);
};

__inline ULONG CMainProjectTree::AddRef(void)
{
    return 1;
}

__inline ULONG CMainProjectTree::Release(void)
{
    return 1;
}

//
// Main window vertical splitter
//

class CMainSplitter
    : public CWindow
{
    friend class CMainFrame;

public:
    static const LPCTSTR    m_pszClassName;         // Class name
    static const UINT       m_nWidth;               // Splitter width

protected:
    int                     m_nPosition;            // X-coordinate
    BOOL                    m_fDragging;            // Drag operation in progress?

public:
    CMainSplitter(void);
    virtual ~CMainSplitter(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(void);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnMouseButtonDown(DWORD dwKeys, int x, int y, LRESULT *plResult);
    virtual BOOL OnMouseButtonUp(DWORD dwKeys, int x, int y, LRESULT *plResult);
    virtual BOOL OnMouseMove(DWORD dwKeys, int x, int y, LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
};

__inline ULONG CMainSplitter::AddRef(void)
{
    return 1;
}

__inline ULONG CMainSplitter::Release(void)
{
    return 1;
}

//
// Main window workspace
//

class CMainWorkspace
    : public CMDIClientWindow
{
public:
    CMainWorkspace(void);
    virtual ~CMainWorkspace(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(void);
};

__inline ULONG CMainWorkspace::AddRef(void)
{
    return 1;
}

__inline ULONG CMainWorkspace::Release(void)
{
    return 1;
}

//
// Workspace child window
//

class CWorkspaceChild
    : public CMDIChildWindow
{
public:
    enum
    {
        WKSPCTYPE_WAVEBANK = 0,
    };

    static const LPCTSTR    m_pszClassName;     // Class name
    const UINT              m_nWorkspaceType;   // Window type

public:
    CWorkspaceChild(UINT nWorkspaceType);
    virtual ~CWorkspaceChild(void);

public:
    // Creation
    virtual BOOL Create(LPCTSTR pszWindowTitle, UINT nIconId, LPBOOL pfOpened = NULL);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
};

//
// Main window frame
//

class CMainFrame
    : public CMDIFrameWindow
{
    friend class CMainSplitter;

public:
    
    //
    // Control identifiers
    //
    
    enum
    {
        IDC_MAIN_TOOLBAR = 1,
        IDC_MAIN_PROJECTTREE,
        IDC_MAIN_SPLITTER,
        IDC_MAIN_WORKSPACE,
        IDC_MAIN_STATUSBAR,
        IDC_MAIN_FIRSTMDICHILD,
    };

    //
    // Menu item indices
    //

    enum
    {
        MENUPOS_FILE = 0,
        MENUPOS_FILE_MRU = 7,
        MENUPOS_WINDOW = 4,
    };

    //
    // Secondary menu indices
    //

    enum
    {
        MENUIDX_WAVEBANK_POPUP = 0,
        MENUIDX_WAVEBANK_ENTRY_POPUP,
        MENUIDX_COUNT
    };

    //
    // Command forwarding flags
    //

    enum
    {
        FORWARD_FOCUS = 0,
        FORWARD_ACTIVE_WORKSPACE_CHILD,
    };

public:
    static const LPCTSTR    m_pszClassName;             // Window class
                                                        
public:                                                 
    CMainToolBar            m_ToolBar;                  // Toolbar
    CMainStatusBar          m_StatusBar;                // Status bar
    CMainProjectTree        m_ProjectTree;              // Project tree
    CMainSplitter           m_Splitter;                 // Vertical splitter
    CMainWorkspace          m_Workspace;                // Workspace
    HMENU                   m_ahMenus[MENUIDX_COUNT];   // Secondary menus

public:
    CMainFrame(void);
    virtual ~CMainFrame(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Initialization
    virtual int Create(int nShowCmd);

    // Control layout
    virtual void LayoutControls(BOOL fRepaint = TRUE);
    virtual void LayoutControls(UINT nWidth, UINT nHeight, BOOL fRepaint = TRUE);

    // Command helpers
    virtual void EnableCommand(UINT nCommandId, BOOL fEnable);
    virtual void CheckCommand(UINT nCommandId, BOOL fChecked);
    virtual void SetDefaultCommandState(void);

    // Message pump
    virtual int PumpMessages(void);

    // Message handlers
    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnMenuSelect(UINT nMenuItem, UINT nFlags, HMENU hMenu, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);

    // Command handlers
    virtual BOOL ForwardCommand(UINT nForward, UINT nCommandId, UINT nControlId, HWND hWndControl, LRESULT *plResult);
    virtual void OnCmdNewProject(void);
    virtual void OnCmdOpenProject(void);
    virtual void OnCmdReloadProject(void);
    virtual void OnCmdSaveProject(void);
    virtual void OnCmdSaveProjectAs(void);
    virtual void OnCmdExit(void);
    virtual void OnCmdAbout(void);
    virtual void OnCmdWindowNext(void);
    virtual void OnCmdWindowPrev(void);
    virtual void OnCmdWindowCascade(void);
    virtual void OnCmdWindowTileHorz(void);
    virtual void OnCmdWindowTileVert(void);
    virtual void OnCmdWindowClose(void);
    virtual void OnCmdWindowCloseAll(void);
    virtual void OnCmdAddWaveBank(void);
    virtual void OnCmdBuild(void);
    virtual void OnCmdConnect(void);
    virtual void OnCmdDisconnect(void);

    // Notification handlers
    virtual void OnToolTipDisplayInfo(LPNMTTDISPINFO pHdr);
};

__inline ULONG CMainFrame::AddRef(void)
{
    return 1;
}

__inline ULONG CMainFrame::Release(void)
{
    return 1;
}

//
// The main application object
//

class CApplication
{
public:
    CRegistry               m_Registry;         // Application registry
    CMainFrame              m_MainFrame;        // Main window frame
    CProject                m_Project;          // Project object
    CMRU                    m_MRU;              // Most-recently-used file list
    CRemoteAudition         m_RemoteAudition;   // Remote audition object

public:
    CApplication(void);
    virtual ~CApplication(void);
};

extern CApplication *g_pApplication;

#endif // __cplusplus

#endif // __MAINFRM_H__
