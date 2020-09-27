// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      xbGameDisc.cpp
// Contents:  
// Revisions: 29-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"   -- Precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_szAppName -- Name of the application (used for class registration)
static char gs_szAppName[] = "xbGameDisc";

static bool g_fInited;

#define UM_DEBUG WM_USER + 1000

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES +++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_tbm        -- Creates and handles the various toolbars.
CToolbarMgr g_tbm;

// g_hInst      -- Handle to the 'current' instance of the application.
HINSTANCE g_hInst;

// g_pvmCur     -- The current view mode
CViewMode *g_pvmCur = NULL;

// g_statusbar  -- The status bar that appears at the bottom of the window
CStatusBar g_statusbar;

CChangeQueue g_cq;

// g_hwndPrevFocus  -- Used to restore focus to the correct control when
//                     the user tabs back to this app
HWND g_hwndPrevFocus;

HWND g_hwndMain;

void HandleDebugMsg(WPARAM wparam, LPARAM lparam);
void HandleEvents();

HACCEL g_haccel;

HCURSOR g_hcurWait;
HCURSOR g_hcurArrow;
HCURSOR g_hcur;

bool g_fModified;

char g_szName[MAX_PATH];

int g_nWindowX, g_nWindowY, g_nWindowW, g_nWindowH;
int g_nUnplacedWindowX, g_nUnplacedWindowY, g_nUnplacedWindowW, g_nUnplacedWindowH;
int g_nCmdShow;

int g_nSplitterX, g_nSplitterX2;
bool g_fDoStartupDlg;
char g_szRootDir[MAX_PATH];
bool g_fJustOpened = false;
CFileHierarchy g_fh;
void StartupDlg();

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetMenuCmds(DWORD dw)
{
    HMENU hmenu = GetMenu(g_hwndMain);
    EnableMenuItem(hmenu, IDM_FILE_SAVE,        dw);
    EnableMenuItem(hmenu, IDM_FILE_SAVEAS,      dw);
    EnableMenuItem(hmenu, IDM_EDIT_FIND,        dw);
    EnableMenuItem(hmenu, IDM_EDIT_SELECTALL,   dw);
    EnableMenuItem(hmenu, IDM_EDIT_FINDNEXT,    dw);
    EnableMenuItem(hmenu, IDM_EDIT_GROUP,       dw);
    EnableMenuItem(hmenu, IDM_EDIT_UNGROUP,     dw);
    EnableMenuItem(hmenu, IDM_EDIT_RESCAN,      dw);
    EnableMenuItem(hmenu, IDM_EDIT_REGENLAYOUT, dw);
    EnableMenuItem(hmenu, IDM_VIEW_FILE,        dw);
    EnableMenuItem(hmenu, IDM_VIEW_LAYOUT,      dw);
    EnableMenuItem(hmenu, IDM_TOOLS_EMULATE,    dw);
    EnableMenuItem(hmenu, IDM_TOOLS_PERF,       dw);
    EnableMenuItem(hmenu, IDM_TOOLS_PREMASTER,  dw);

    g_tbm.EnableButton(TB_GLOBAL, IDM_FILE_SAVE,       dw==MF_ENABLED);
    g_tbm.EnableButton(TB_GLOBAL, IDM_VIEW_FILE,       dw==MF_ENABLED);
    g_tbm.EnableButton(TB_GLOBAL, IDM_VIEW_LAYOUT,     dw==MF_ENABLED);
    g_tbm.EnableButton(TB_GLOBAL, IDM_TOOLS_EMULATE,   dw==MF_ENABLED);
    g_tbm.EnableButton(TB_GLOBAL, IDM_TOOLS_PREMASTER, dw==MF_ENABLED);
    g_tbm.EnableButton(TB_GLOBAL, IDM_TOOLS_PERF,      dw==MF_ENABLED);

    if (dw == MF_GRAYED)
    {
        g_tbm.EnableButton(TB_LAYER, IDM_EDIT_GROUP,   FALSE);
        g_tbm.EnableButton(TB_LAYER, IDM_EDIT_UNGROUP, FALSE);
        g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER0,  FALSE);
        g_tbm.EnableButton(TB_LAYER, IDM_VIEW_LAYER1,  FALSE);
        g_tbm.EnableButton(TB_LAYER, IDM_VIEW_BOTHLAYERS, FALSE);
        g_tbm.EnableButton(TB_LAYER, IDM_VIEW_UNPLACED,   dw==MF_ENABLED);
    }
    g_tbm.EnableButton(TB_LAYER, IDM_EDIT_RESCAN, dw==MF_ENABLED);
}

void SetViewMode(CViewMode *pvmNew)
{
    // Validate parameters
    assert(pvmNew);

    // Nothing to do if we're already in the specified mode
    if (pvmNew == g_pvmCur)
        return;

    // We're switching modes -- hide the current one and show the new one.
    if (g_pvmCur)
        g_pvmCur->Hide();
    g_pvmCur = pvmNew;
    g_pvmCur->Show();
}

void UpdateWindowCaption()
{
    char sz[MAX_PATH + 100];
    if (g_szName[0] == '\0')
        sprintf(sz, "xbGameDisc %s", g_fModified ? "*":"");
    else
        sprintf(sz, "xbGameDisc - [%s%s]", g_szName, g_fModified ? " *":"");
    SetWindowText(g_hwndMain, sz);
}

void ClearModified()
{
    g_fModified = false;

    UpdateWindowCaption();
}

void SetModified()
{
    g_fModified = true;

    UpdateWindowCaption();    
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WndProc
// Purpose:   This is the application's Windows message notification
//            function.  When Windows wants to tell the app that something
//            has happened (ie mouse button pressed), it does so by calling
//            into WndProc with the message and it's related information.
//            If this function does not handle the specified message, then
//            it is passed on to the Win32 function 'DefWindowProc', which
//            allows Windows to do any handling on it that it wants.
// Arguments: hwnd          -- Handle to the window
//            uMsg          -- The message sent to the window
//            wparam        -- A message parameter.
//            lparam        -- Another message parameter
// Return:    Result of the message processing - depends on the message sent.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    RECT rc;
    HDC hdc;
    PAINTSTRUCT ps;
    int nY;
    LPNMHDR pnmhdr;
    
//    TrackMessage(uMsg, wparam, lparam, "xbGameDisc");

    switch (uMsg) 
    {
    case WM_CREATE:
        g_hwndMain = hwnd;

        ClearModified();

        // Initialize the toolbar manager
		g_tbm.Init(hwnd);
        
        // Add the toolbars to the toolbarmgr
        InitToolbars();

        // Make the global toolbar visible
        g_tbm.Show(TB_GLOBAL);

        // Initialize the status bar
        g_statusbar.Init(hwnd);

        // Initialize the explorer and layer viewmodes (they will initialize
        // their own toolbars and menus).
        g_vmExplorer.Init(hwnd);
        g_vmLayer.Init(hwnd);

        // Initialize the file hierarchy 
         g_fh.Init();

        // By default, start in the Explorer mode
        g_vmLayer.Hide();
        SetViewMode(&g_vmExplorer);

        SetMenuCmds(MF_GRAYED);

        // Return 0 to tell windows to continue creation of the window
        return 0;

    case WM_SIZE:
        // Resize the different view modes to match the window's new size
        g_statusbar.Resize(LOWORD(lparam), HIWORD(lparam));        
        g_vmExplorer.Resize(LOWORD(lparam), HIWORD(lparam));
        g_vmLayer.Resize(LOWORD(lparam), HIWORD(lparam));

        // Resize the toolbar manager.
        g_tbm.Resize(wparam, lparam);
        break;

    case WM_NOTIFY:
        switch (wparam)
        {
        case WNDID_TOOLBARMGR:
            g_tbm.HandleNotify((LPNMHDR)lparam);
            break;
        case WNDID_TREEVIEW:
            pnmhdr = (LPNMHDR) lparam;
            if (pnmhdr->code == TVN_SELCHANGED)
                g_vmExplorer.m_treeview.HandleSelection(((LPNMTREEVIEW)pnmhdr));
            break;
        }
        break;

    case WM_SETFOCUS:
        if (g_hwndPrevFocus)
            SetFocus(g_hwndPrevFocus);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        nY = g_tbm.GetRowsHeight();
        rc.left = ps.rcPaint.left;
        rc.right = ps.rcPaint.right;
        rc.top = nY-1;
        rc.bottom = rc.top + 1;
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        rc.top = nY-2;
        rc.bottom = rc.top + 1;
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        EndPaint(hwnd, &ps);
        break;
        
    case WM_ERASEBKGND:
        return 1;

    case WM_CLOSE:
        // Close out current workspace if modified.
        if (g_fModified)
        {
            if (!VerifyCloseWorkspace())
                return 0;
        }
        CloseWorkspace();        
        StoreGlobalVars();
        PostQuitMessage(0);
        return 0;     

    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDM_EDIT_SELECTALL:
        case IDM_VIEW_LAYER0:
        case IDM_VIEW_LAYER1:
        case IDM_VIEW_BOTHLAYERS:
        case IDM_VIEW_UNPLACED:
            g_pvmCur->HandleMenuCommand(LOWORD(wparam));
            break;
        case IDM_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_VIEW_FILE:
            SetViewMode(&g_vmExplorer);
            break;
        case IDM_VIEW_LAYOUT:
            SetViewMode(&g_vmLayer);
            break;       
        case IDM_FILE_NEW:
            HandleNew();
            break;
        case IDM_FILE_SAVE:
            HandleSave();
            break;

        case IDM_FILE_SAVEAS:
            SaveAs();
            break;

        case IDM_FILE_OPEN:
            HandleOpen();
            break;

        case IDM_HELP_ABOUT:
            DoAbout(hwnd);
            break;
        }
        break;
        
    case WM_MOUSEMOVE:
        SetCursor(g_hcur);
        break;
        
    // USER DEFINED MESSAGES:

    case UM_TOOLBARRESIZED:
        // Toolbar has been resized; need to resize the current view to
        // take that into account.
        if (g_fInited)
        {
            GetClientRect(hwnd, &rc);
            g_vmExplorer.Resize(rc.right - rc.left, rc.bottom - rc.top);
            g_vmLayer.Resize(rc.right - rc.left, rc.bottom - rc.top);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;

    case UM_DEBUG:
        // A test application has sent us a debug message
        HandleDebugMsg(wparam, lparam);
        break;
    }

    // Let Windows handle all unhandled messages
    return DefWindowProc(hwnd, uMsg, wparam, lparam);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InitInstance
// Purpose:   Register our class and then create and show our window.
// Arguments: hinst         -- The handle to this instance of the app
//            nCmdShow      -- Default visibility of the application
// Return:    Handle to the newly created window (NULL if error).
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static HWND InitInstance(HINSTANCE hinst, int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND       hwnd;

    g_fInited = false;

    // Register the class for the app's window.
    memset (&wcex, 0, sizeof wcex);
    wcex.cbSize         = sizeof(WNDCLASSEX); 
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.hInstance      = hinst;
    wcex.lpszClassName  = gs_szAppName;
    wcex.lpszMenuName   = TEXT("IDR_MENU1");
    if (!RegisterClassEx(&wcex))
        return NULL;

    // Create the app's window.
    hwnd = CreateWindow(gs_szAppName, gs_szAppName, WS_OVERLAPPEDWINDOW,
                        g_nWindowX, g_nWindowY, g_nWindowW, g_nWindowH, NULL, NULL, hinst, NULL);

    if (hwnd)
    {
        // Make our window visible and refresh it.
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    g_hcurWait = LoadCursor(NULL, IDC_WAIT);
    g_hcurArrow = LoadCursor(NULL, IDC_ARROW);
    g_hcur = g_hcurArrow;

    g_fInited = true;

    // Drop into the startup dialog
    if (g_fDoStartupDlg)
        StartupDlg();

    return hwnd;
}

BOOL g_fQuit = false;
BOOL CheckMessages()
{
    MSG msg;
    if (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
    {
        if (!TranslateAccelerator(g_hwndMain, g_haccel, &msg))
        {
            if (msg.message == WM_QUIT)
            {
                g_fQuit = true;
                return FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return TRUE;
    }
    return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WinMain
// Purpose:   This is the default entry point into a Windows application.  
//            When the application is started, Windows calls into this
//            function - when this function exits, the application ends.
// Arguments: hinst         -- The handle to this instance of the app
//            hinstPrev     -- Obsolete; throwback to Win16 -- ignore
//            szCmdLine     -- Contains the parameter list passed to the app
//            nCmdShow      -- Default visibility of the application
// Return:    Result value application returns to the operating system
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR szCmdLine,
                     int nCmdShow)
{
    g_hInst = hinst;

    // Initialize COM (necessary for SHGetFileInfo)
    CoInitialize(NULL);

    // Initialize global vars (which may have been saved in the registry
    // during a previous run)
    InitGlobalVars();
    
    // Initialize our class and create our window
    g_hwndMain = InitInstance (hinst, g_nCmdShow);
    if (g_hwndMain == NULL)
    {
        // A thoroughly unrecoverable (and unexpected) error occurred.  The
        // user's machine is fubar.
        MessageBox(NULL, "xbGameDisc failed to initialize.  Please "
                         "restart your computer and try again.",
                         "Failed to start", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    g_haccel = LoadAccelerators(hinst, "MYACCEL");
    
    // Main message loop:
    while (true)
    {
        if (!CheckMessages())
        {
            // Anything sitting in the change queue that we should handle?
            while (g_cq.m_cChangeQueueReady > 0)
            {
                HandleEvents();
                g_cq.m_cChangeQueueReady--;
            }
            if (g_pvmCur)
                g_pvmCur->UpdateIfDirty();
        }
        if (g_fQuit)
            break;
    }

    return 0;
}

void HandleDebugMsg(WPARAM wparam, LPARAM lparam)
{
    // A debugging application has sent us a debugging message.  Handle it here

    // For now, only message is "verify state"
    Debug_VerifyState();
}

void DumpEvent(eEvent event, CFileObject *pfo)
{
    return;
    
    if (event == EVENT_ADDED)
        DebugOutput("Added: %s", pfo->m_szPath);
    else if (event == EVENT_REMOVED)
        DebugOutput("Removed: %s", pfo->m_szPath);

    if (pfo->m_pfoParent) DebugOutput(" (parent=%s)", pfo->m_pfoParent->m_szName);
    DebugOutput("\n");
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleRootDeleted
// Purpose:   User deleted the root tree.  Clean up everything and warn the user
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleRootDeleted()
{
    CloseWorkspace();

    // Inform the user
    MessageBox(NULL, "The root directory has been removed.",
               "Root directory not found",
               MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function
// Purpose:   
// Arguments: 
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleEvents()
{
    eEvent event;
    CFileObject *pfo;
    bool fMod = false;
    
    while (g_cq.HasEvent())
    {
        g_cq.Dequeue(&event, &pfo);

        if (event == EVENT_ROOTDELETED)
        {
            // User deleted the root tree!
            HandleRootDeleted();
            return;
        }

//        DumpEvent(event, pfo);
        if (event == EVENT_ADDED)
            g_fh.Add(pfo);

        g_pvmCur->HandleEvent(event, pfo->m_pfoMainThread);
        
        if (event == EVENT_REMOVED)
        {
            g_fh.Remove(pfo);
            delete pfo;
        }

        // UNDONE: If *any* file system event occurs, the workspace is considered to
        // have been modified.  This may lead to some user confusion ("what?
        // I didn't modify anything!").  Need to document appropriately
        fMod = true;
    }
    if (fMod)
        SetModified();

    if (g_fJustOpened)
    {
        MessageBox(g_hwndMain, "Files have changed in the layout since the XLO file was last saved.",
                   "Layout has changed", MB_OK | MB_ICONWARNING | MB_APPLMODAL);
    }
    g_fJustOpened = false;
    g_pvmCur->UpdateIfDirty();
}