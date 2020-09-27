/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       winmain.h
 *  Content:    Application entry point and main window object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WINMAIN_H__
#define __WINMAIN_H__

//
// Toolbar button indices
//

BEGIN_DEFINE_ENUM()
    BTN_LEAF = 0,
    BTN_OPEN,
    BTN_SAVE,
    BTN_BANK,
    BTN_ENTRY,
    BTN_GENERATE,
    BTN_ADPCM,
    BTN_8BIT,
    BTN_AUDITION,
    BTN_STOP,
    BTN_COUNT,
END_DEFINE_ENUM_();

//
// List-view column data
//

BEGIN_DEFINE_STRUCT()
    LPLVCOLUMNDATA  paColumnData;
    UINT            nColumnCount;
END_DEFINE_STRUCT(LVCOLUMNDATAPTR);

#ifdef __cplusplus

//
// Main window class
//

class CMainWindow
    : public CWindow, public CWaveBankCallback
{
public:
    enum                                                                // Control identifiers
    {
        IDC_MAIN_BANKLIST = 0,
        IDC_MAIN_ENTRYLIST,
        IDC_MAIN_TOOLBAR,
        IDC_MAIN_STATUSBAR,
        IDC_MAIN_COUNT
    };

    LVCOLUMNDATAPTR             m_aColumnData[IDC_MAIN_ENTRYLIST + 1];  // Column data
    HWND                        m_ahwndControls[IDC_MAIN_COUNT];        // Control window handles
    HMENU                       m_ahmenuPopup[IDC_MAIN_ENTRYLIST + 1];  // Popup menus
    CGuiWaveBankProject *       m_pProject;                             // Project object
    CMRU                        m_MRU;                                  // MRU list
    BOOL                        m_fDirty;                               // Does the project need to be saved?
    PDM_CONNECTION              m_pxbdm;                                // Xbox debugger connection

public:
    CMainWindow(void);
    virtual ~CMainWindow(void);

public:
    // Creation
    virtual BOOL Create(void);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnClose(LRESULT *plResult);
    virtual BOOL OnDestroy(LRESULT *plResult);
    virtual BOOL OnSize(DWORD dwResize, int nWidth, int nHeight, LRESULT *plResult);
    virtual BOOL OnCommand(UINT nCommandId, UINT nControlId, HWND hwndControl, LRESULT *plResult);
    virtual BOOL OnNotify(LPNMHDR pHdr, LRESULT *plResult);
    virtual BOOL OnContextMenu(HWND hwnd, int x, int y, LRESULT *plResult);
    virtual BOOL OnMenuSelect(UINT nMenuItem, UINT nFlags, HMENU hMenu, LRESULT *plResult);

    // Command handlers
    virtual void OnNewProject(BOOL fAddBank = TRUE);
    virtual void OnOpenProject(LPCSTR pszProjectFile = NULL);
    virtual void OnSaveProject(BOOL fPrompt);
    virtual void OnNewBank(BOOL fAddEntries = TRUE);
    virtual void OnRemoveBank(void);
    virtual void OnNewEntry(void);
    virtual void OnRemoveEntry(void);
    virtual void OnConvert(DWORD dwMask);
    virtual void OnGenerate(void);
    virtual void OnDelete(HWND hwnd = NULL);
    virtual void OnRename(HWND hwnd = NULL);
    virtual void OnSelectAll(HWND hwnd = NULL);
    virtual void OnExit(void);
    virtual void OnAbout(void);
    virtual void OnRefresh(void);
    virtual void OnAudition(void);
    virtual void OnCancelAudition(void);

    // Notification handlers
    virtual void OnToolTipDisplayInfo(LPNMTTDISPINFO pInfo);
    virtual void OnListViewItemChanged(LPNMLISTVIEW pInfo);
    virtual void OnBankListItemStateChange(void);
    virtual void OnEntryListItemStateChange(void);
    virtual void OnListViewColumnClick(LPNMLISTVIEW pInfo);
    virtual void OnListViewBeginLabelEdit(NMLVDISPINFO *pInfo);
    virtual void OnListViewEndLabelEdit(NMLVDISPINFO *pInfo);
    virtual BOOL BeginEntry(LPCSTR pszFile, DWORD dwFlags);

protected:
    // UI helpers
    virtual HWND CreateListView(UINT nCommandId);
    virtual HWND CreateToolBar(void);
    virtual HWND CreateStatusBar(void);
    virtual void MakeDirty(BOOL fDirty = TRUE, BOOL fForce = FALSE);
    virtual BOOL CheckDirty(void);
    virtual void UpdateAppTitle(void);
    
    virtual void SetCommandState(UINT nCommandId, BOOL fEnable, BOOL fCheck);
    virtual void SetBankCommandStates(void);
    virtual void SetEntryCommandStates(void);
    virtual void SetCommandStates(void);

    // Remote helpers
    virtual BOOL ConnectRemote(void);
};

//
// About box
//

class CAboutDialog
    : public CModalDialog
{
public:
    enum
    {
        IDD = IDD_ABOUT,
    };

public:
    CAboutDialog(void);
    virtual ~CAboutDialog(void);

public:
    // Creation
    virtual BOOL Create(CWindow *pParent = NULL);

    // Message handlers
    virtual BOOL OnInitDialog(LPVOID pvContext, LRESULT *plResult);
};    

#endif // __cplusplus

#endif // __WINMAIN_H__
