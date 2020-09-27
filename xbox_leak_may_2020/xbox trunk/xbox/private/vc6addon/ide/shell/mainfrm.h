// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#ifndef __SMENU_H__
#include "shlmenu.h"
#endif

#ifndef __SBAR_H__
#include "sbar.h"
#endif

#ifndef __SHLDOCS_H__
#include "shldocs.h"
#endif

#ifndef _OLEREF_H_
#include <oleref.h>
#endif

#define MANAGER_STATES 4

class CVshellPackage;
class CDockManager;
class CPartFrame;
class CStateSaver;
class CIPCompFrameHook;
class CIPCompMainFrame;
interface IMsoComponentManager;
class CShellRecorder;
extern CShellRecorder g_theShellRecorder;

class CAutoDocuments ;

class CFullScreenState : public CObject
{
   static _TCHAR szKeyName[];

public:
   CFullScreenState ();

   void LoadFromReg (LPCTSTR szSection);
   void SaveToReg (LPCTSTR szSection);

   const CFullScreenState& operator<< (BYTE *);
   const CFullScreenState& operator>> (BYTE *);

   BYTE m_bFullScreen:1;
   BYTE m_bFSWasZoomed:1;
   BYTE m_bFSWasIconic:1;
   BYTE m_bStatusBar:1;
   BYTE m_bFSStatusBar:1;
   BYTE m_bFSVertScroll:1;
   BYTE m_bFSHorzScroll:1;
   BYTE m_bFirstTime:1;
   CRect m_FullScreenWindowRect;
   WINDOWPLACEMENT m_wpFSPrevWinState;

   struct Notify {
      BOOL  bUnused;
      BOOL  bFirstTime;
      RECT  rectFullScreen;
      BOOL  bVert;
      BOOL  bHorz;
   };
};

class CMDIClientValidator : public CWnd
{
public:
   CMDIClientValidator();

protected:
   virtual WNDPROC* GetSuperWndProcAddr();
   virtual void PostNcDestroy();

   afx_msg void OnParentNotify(UINT message, LPARAM lParam);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnRButtonUp(UINT nFlags, CPoint pt);
   int m_cyLast;

   DECLARE_MESSAGE_MAP();
};

interface IMacroRecorder;
interface IRecorderForShell;

class CMainFrame : public CMDIFrameWnd
{
        friend class CAutoDocuments;
        friend class CAutoWindows;

        DECLARE_DYNAMIC(CMainFrame)
public:
        CMainFrame();

// Attributes
public:
        BOOL m_bInMenuMode:1;
        BOOL m_bLockLayout:1;
        BOOL m_bLockManager:1;
    BOOL m_fInOnEnterState:1;
        int m_nManagerState;
        int m_nManagerStateSave;
        CDockManager* m_pManager;
        CMDIClientValidator* m_pWndMDIClient;
        CRect m_lastWindowRect; // last non-min/max position of app window
        CSushiBar m_statusBar;
        CKeymap m_keymap;
        CUIntArray m_aDockCmds;

        CString m_strModeName;
        CString m_strAppTitle;
        CString m_strSubTitle;
        CFullScreenState m_FS;

        CPtrList m_plIPCompFrameHook;   // the active frame hook is always at the head
        CIPCompMainFrame * m_pIPCmpMainFrm;     

protected:
   HGLOBAL m_ahManagerStates[MANAGER_STATES];
   HWND m_hwndAppWiz;

   // For recording default text...
   IMacroRecorder* m_pMacroRecorder;
   IRecorderForShell* m_pRecorderForShell;
   void RecordDefaultTextIfNecessary(WORD nID); 
        
        static CShellRecorder& GetShellRec(void) { return g_theShellRecorder; }

// Implementation
public:
   virtual ~CMainFrame();
   virtual BOOL DestroyWindow();

   // frame window based modality
   virtual void BeginModalState();
   virtual void EndModalState();

   BOOL  InitAfterRegistration();
   // If bAllEditors is TRUE, then all editor specific commands will be shown, independent of the current editor. Otherwise,
   // only those for the current editor will be shown.
   // If bAllProjects is TRUE, all project dependent commands will be shown.
   // If bEmptyMenus is TRUE, then any menu which has no items (visible or invisible) will be shown
   void  RebuildMenus(BOOL bAllEditors=FALSE, BOOL bAllProjects=FALSE, BOOL bEmptyMenus=FALSE);
   // resets a single menu to its default state
   void  ResetMenu(UINT nID);
   // resets a single menu to its default state
   void  ResetAllMenus();
   // fills a list box with all commands that are missing from the menus
   void  FillDeletedMenuItemList(CListBox *pList);
   void  MapDockingCmd(UINT nID, UINT nCmd);
   void  MapDockingCmds();
   void  InitializeToolsMenu(CVshellPackage* pPackage);
   void  InitializeHelpMenu (CVshellPackage* pPackage);
   void  InitKeymap();

   void  SetDockState(int nState, int nMask = 1);
   void  LockDockState(BOOL bLock);
   void  LockLayout(BOOL bLock);

   inline   BOOL IsFullScreen () const { return m_FS.m_bFullScreen; }

   // Located in workspc.cpp.
   void LoadLayout();
   void SaveLayout();
   BOOL LoadManager();
   BOOL SaveManager();
   BOOL SerializeWorkspaceLayout(CStateSaver& stateSave);
   BOOL LoadWorkspace(CArchive& ar);
   BOOL SaveWorkspace(CArchive& ar);
   CPartFrame* LoadWorkspaceWindow(CArchive& ar,
      BOOL bPlace = TRUE, CDocument** hDoc = NULL,
        UINT* pnCmdShow = NULL);
   void SaveWorkspaceWindow(CArchive& ar, CPartFrame* pFrame,
      BOOL bRelative = FALSE);

   void AddIPCompFrameHook(CIPCompFrameHook * pHook);
   void RemoveIPCompFrameHook(CIPCompFrameHook * pHook);
        
   void EnsureCustomizerActive();
   void EnsurePopupActive();
   void EnsureViewActive();

   virtual void GetMessageString(UINT nID, CString& rMessage) const;

	void UpdateProjectCommands(void);

#ifdef _DEBUG
   virtual  void AssertValid() const;
   virtual  void Dump(CDumpContext& dc) const;
#endif

   void OnDirChange();
   virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
   virtual void RecalcLayout(BOOL bNotify = TRUE);
   virtual BOOL NegotiateBorderSpace(UINT nBorderCmd, LPRECT lpRectBorder);

   void CloseWindows(UINT nFilter = FILTER_NONE);

// Slob-based Drag&Drop
   void BeginDragDrop(class CDragNDrop* pInfo);
   void EndDragDrop ();
   class CDragNDrop* m_dragDrop; // controls the drag and drop operations

// Super-Deluxe ESC key handler
   afx_msg void OnCancel();

// Generic Edit Go To dialog
   afx_msg void CmdGoto();

   void SetAppWizHwnd(HWND hWnd) { m_hwndAppWiz = hWnd; }

   BOOL SetComponentManager(IMsoComponentManager * pCmpMgr);
   IMsoComponentManager * GetComponentMgr();    
   DWORD GetComponentID();

// Generated message map functions
protected:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   // The following is copied from MFC's CMDIFrameWnd.
   BOOL MDIFrameWnd_PreTranslateMessage(MSG* pMsg) ;

   virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo);
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpcs);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int cx, int cy);
   afx_msg void OnClose();
   afx_msg void OnDestroy();
   afx_msg void OnInitMenu(CMenu* pMenu);
   afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
   afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
   afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
   afx_msg void OnEnable(BOOL bEnable);
   afx_msg void OnWindowCloseAll();
   afx_msg void OnWindowClose();
   afx_msg void OnWindowNext();
   afx_msg void OnWindowPrevious();
   afx_msg void OnWindowFullScreen();
   afx_msg void OnWindowListDocs();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnDragMove();
   afx_msg void OnDragCopy();
   afx_msg void OnDragCancel();
   afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnWinIniChange(LPCSTR lpszSection);
   afx_msg void OnFontChange ();
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
   afx_msg BOOL OnNcActivate(BOOL bActive);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnActivate(UINT nState, CWnd* pInactiveWnd, BOOL bMinimized);
   afx_msg void OnTimer(UINT nTimerID);
   afx_msg void OnSysColorChange();
   afx_msg void OnEndSession(BOOL bEnding);
   afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
   afx_msg BOOL OnQueryNewPalette();
   afx_msg void OnUpdateNewWindow(CCmdUI* pCmdUI);
   afx_msg void OnUpdateWindowCommand(CCmdUI* pCmdUI);
   afx_msg void OnNewWindow();
   afx_msg void OnUpdateFullScreen(CCmdUI* pCmdUI);
   afx_msg void OnUpdateIndicator(CCmdUI* pCmdUI);
   afx_msg void OnUpdateMenuCommand(CCmdUI* pCmdUI);
   afx_msg void OnUpdateCustomMenuCommand(CCmdUI* pCmdUI);
   afx_msg void OnUpdateWindows(CCmdUI* pCmdUI);
   afx_msg void OnMenuCommand(UINT nID);
   afx_msg void OnWindows(UINT nID);
   afx_msg void OnHelp();
   afx_msg void OnFastHelp();
   afx_msg void OnHelpContents();
#ifdef __DEVHELP98__ 
   afx_msg void OnHelpKeywords() ;
#endif
   afx_msg void OnHelpSearch();
   afx_msg void OnHelpKeyboard();
   afx_msg void OnDropFiles(HDROP hDropInfo);
   afx_msg void OnGetMinMaxInfo (MINMAXINFO * pGetMinMaxInfo);
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        //}}AFX_MSG

   afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg BOOL OnDockWindowShow(UINT nID);
   afx_msg BOOL OnDockViewShow(UINT nID);
#ifdef _DEBUG
   afx_msg void OnOpenLayout();
#endif

   afx_msg LRESULT OnSetAppValidate(WPARAM wParam, LPARAM lParam);
#ifdef _DEBUG
   afx_msg LRESULT OnFatalExit(WPARAM wParam, LPARAM lParam);
#endif
#ifdef _TEST
   afx_msg LRESULT OnSetProp(WPARAM wParam, LONG lParam);
   afx_msg LRESULT OnGetProp(WPARAM wParam, LONG lParam);
   afx_msg LRESULT OnHeapUse(WPARAM wParam, LONG lParam);
   afx_msg LRESULT OnTestDockable(WPARAM wParam, LONG lParam);

   //Project window
   afx_msg LRESULT OnGetItemProp(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnSetItemProp(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnGetProjPath(WPARAM wParam, LPARAM lParam);

   //Build state
   afx_msg LRESULT OnIsBuildPossible(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnGetErrorCount(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnGetWarningCount(WPARAM wParam, LPARAM lParam);
#endif
   afx_msg LRESULT OnPostEnableIME(WPARAM wParam, LPARAM lParam);

   afx_msg LRESULT OnFileChange(WPARAM wPackage, LPARAM lpFCItem);
   afx_msg LRESULT OnBarSizeChanging(WPARAM bLarge, LPARAM);
   afx_msg LRESULT OnTestMenu(WPARAM wParam, LPARAM lParam);

   afx_msg LRESULT OnRegMouseWheel(WPARAM wParam, LPARAM lParam);

   CPtrList m_plFileChange;

   DECLARE_MESSAGE_MAP()

public: //NoMenu
        virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
        protected:
        virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

};

inline void CMainFrame::LockLayout(BOOL bLock)
{  m_bLockLayout = bLock; }

/////////////////////////////////////////////////////////////////////////////

#define IDT_FILE_CHANGE    102

#endif // __MAINFRM_H__

