// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      xbGameDisc.h
// Contents:  
// Revisions: 30-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum {TB_GLOBAL=0, TB_LAYER, TB_EXPLORER};

enum {VIEWMODE_EXPLORER, VIEWMODE_LAYER};

// Various window IDs.  Used for WM_NOTIFY handling.
enum {WNDID_TOOLBARMGR = 1000, WNDID_SPLITTER, WNDID_TREEVIEW, WNDID_TREEVIEW_STATIC,
      WNDID_EXPLISTVIEW, WNDID_LISTVIEW, WNDID_LISTVIEW_HEADER, WNDID_UNPLACED};

class CToolbarMgr;
class CStatusBar;

extern CToolbarMgr g_tbm;
extern HINSTANCE g_hInst;
extern CStatusBar g_statusbar;

enum {UM_TOOLBARRESIZED = WM_USER + 1};
typedef enum eEvent {EVENT_ADDED = 0, EVENT_REMOVED, EVENT_SIZECHANGED,
                     EVENT_TIMECHANGED, EVENT_ROOTDELETED};

#define CB_SIZE 13
extern HWND g_hwndPrevFocus;
extern HWND g_hwndMain;

extern HCURSOR g_hcurWait;
extern HCURSOR g_hcurArrow;
extern HCURSOR g_hcur;

extern int g_nWindowX, g_nWindowY, g_nWindowW, g_nWindowH;
extern int g_nUnplacedWindowX, g_nUnplacedWindowY, g_nUnplacedWindowW, g_nUnplacedWindowH;
extern int g_nCmdShow;

extern int g_nSplitterX, g_nSplitterX2;
extern bool g_fModified;
extern bool g_fDoStartupDlg;
extern HACCEL g_haccel;
extern BOOL CheckMessages();
extern CUpdateBox g_updatebox;
extern char g_szRootDir[MAX_PATH];
extern void SetModified();
extern void ClearModified();

class CFileHierarchy;
extern CFileHierarchy g_fh;

class CChangeQueue;
extern CChangeQueue g_cq;

class CViewMode;
extern CViewMode *g_pvmCur;

extern void SetViewMode(CViewMode *pvmNew);

extern bool g_fJustOpened;
extern char g_szName[MAX_PATH];

extern void SetMenuCmds(DWORD dw);
