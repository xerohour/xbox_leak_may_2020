/***************************************************************************
 *
 *  Copyright (C) 11/8/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       winmain.cpp
 *  Content:    Application entry point and main window object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/8/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wbndgui.h"

CHAR g_szAppTitle[0x100];
HWND g_hwndMain = NULL;
LPCSTR g_pszAppTitle = g_szAppTitle;
HINSTANCE g_hInstance = NULL;


/****************************************************************************
 *
 *  WinMain
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      HINSTANCE [in]: application instance handle.
 *      HINSTANCE [in]: unused.
 *      LPSTR [in]: application command-line.
 *      INT [in]: show command.
 *
 *  Returns:  
 *      INT: application return code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WinMain"

INT WINAPI
WinMain
(
    HINSTANCE               hInstance,
    HINSTANCE               hUnused,
    LPSTR                   pszCmdLine,
    INT                     nShowCmd
)
{
    INITCOMMONCONTROLSEX    icc;
    CMainWindow             Window;
    BOOL                    fSuccess;
    CHAR                    szPath[MAX_PATH];
    UINT                    a, b;

    //
    // Initialize globals
    //
    
    g_hInstance = hInstance;
    CDebug::m_pfnDpfCallback = CWindow::DebugCallback;

    //
    // Initialize the common controls library
    //
    
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    
    fSuccess = InitCommonControlsEx(&icc);

    //
    // Load the application title
    //

    LoadString(hInstance, IDS_APPTITLE, g_szAppTitle, NUMELMS(g_szAppTitle));

#ifdef _DEBUG

    strcat(g_szAppTitle, " (debug)");

#endif // _DEBUG
    
    //
    // Create the main window
    //

    if(fSuccess)
    {
        fSuccess = Window.Create();
    }

    //
    // If a project file was specified in the command line, open it
    //

    if(fSuccess && pszCmdLine)
    {
        Window.OnOpenProject(pszCmdLine);
    }

    //
    // Enter the message pump
    //

    if(fSuccess)
    {
        Window.PumpMessages();
    }

    return !fSuccess;
}


/****************************************************************************
 *
 *  CMainWindow
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::CMainWindow"

CMainWindow::CMainWindow
(
    void
)
{
    ZeroMemory(&m_ahwndControls, sizeof(m_ahwndControls));
    ZeroMemory(&m_ahmenuPopup, sizeof(m_ahmenuPopup));

    m_pProject = NULL;
    m_fDirty = FALSE;
    m_pxbdm = NULL;

    m_aColumnData[IDC_MAIN_BANKLIST].paColumnData = CGuiWaveBank::m_aColumnData;
    m_aColumnData[IDC_MAIN_BANKLIST].nColumnCount = NUMELMS(CGuiWaveBank::m_aColumnData);

    m_aColumnData[IDC_MAIN_ENTRYLIST].paColumnData = CGuiWaveBankEntry::m_aColumnData;
    m_aColumnData[IDC_MAIN_ENTRYLIST].nColumnCount = NUMELMS(CGuiWaveBankEntry::m_aColumnData);
}


/****************************************************************************
 *
 *  ~CMainWindow
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::~CMainWindow"

CMainWindow::~CMainWindow
(
    void
)
{
    CHAR                    szCommand[0x100];
    UINT                    i;

    for(i = 0; i < NUMELMS(m_ahmenuPopup); i++)
    {
        if(m_ahmenuPopup[i])
        {
            DestroyMenu(m_ahmenuPopup[i]);
        }
    }

    if(m_pxbdm)
    {
        sprintf(szCommand, "XAUD!disconnect %s", g_pszAppTitle);

        DmSendCommand(m_pxbdm, szCommand, NULL, NULL);
        DmCloseConnection(m_pxbdm);
    }
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the window.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::Create"

BOOL
CMainWindow::Create
(
    void
)
{
    WNDCREATE               Create;
    LRESULT                 lrWndPos;
    HKEY                    hkey;
    WINDOWPLACEMENT         wp;
    DWORD                   cbSize;

    ZeroMemory(&Create, sizeof(Create));

    Create.dwClassStyle = CS_OWNDC;
    Create.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_PROJECT));
    Create.hCursor = LoadCursor(NULL, IDC_ARROW);
    Create.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    Create.pszClassName = "Wave Bundler GUI";
    Create.pszWindowName = NULL;
    Create.dwStyle = WS_OVERLAPPEDWINDOW;
    Create.x = CW_USEDEFAULT;
    Create.y = CW_USEDEFAULT;
    Create.nWidth = CW_USEDEFAULT;
    Create.nHeight = CW_USEDEFAULT;
    Create.pszMenu = MAKEINTRESOURCE(IDM_MAIN);
    Create.pszAccelerator = MAKEINTRESOURCE(IDA_MAIN);

    if(!CWindow::Create(&Create))
    {
        return FALSE;
    }

    //
    // Load window position and state.  If none exist, center the window
    // over the default monitor
    //

    if(ERROR_SUCCESS == (lrWndPos = RegOpenKey(HKEY_CURRENT_USER, WBND_REG_KEY, &hkey)))
    {
        cbSize = sizeof(wp);
        
        lrWndPos = RegQueryValueEx(hkey, WBND_REG_VAL_WNDPOS, NULL, NULL, (LPBYTE)&wp, &cbSize);

        RegCloseKey(hkey);
    }

    if(ERROR_SUCCESS == lrWndPos)
    {
        if(SW_SHOWMAXIMIZED != wp.showCmd)
        {
            wp.showCmd = SW_SHOWNORMAL;
        }
        
        MoveWindow(wp.rcNormalPosition);
        ShowWindow(m_hwnd, wp.showCmd);
    }
    else
    {            
        CenterWindow(m_hwnd, NULL);
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
    }

    //
    // Connect to the remote audio console
    //

    ConnectRemote();

    //
    // Initialize the project
    //

    OnNewProject(FALSE);

    return TRUE;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles creation of the window.
 *
 *  Arguments:
 *      LPVOID [in]: window creation context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnCreate"

BOOL
CMainWindow::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    HMENU                   hSubMenu;
    
    //
    // Save the main window handle
    //

    g_hwndMain = m_hwnd;

    //
    // Hand off to the base class
    //
    
    fHandled = CWindow::OnCreate(pvContext, plResult);

    //
    // Set the window title
    //

    UpdateAppTitle();

    //
    // Create the toolbar
    //

    if(!(m_ahwndControls[IDC_MAIN_TOOLBAR] = CreateToolBar()))
    {
        *plResult = -1;
        return TRUE;
    }

    //
    // Create the status bar
    //

    if(!(m_ahwndControls[IDC_MAIN_STATUSBAR] = CreateStatusBar()))
    {
        *plResult = -1;
        return TRUE;
    }

    //
    // Create the list views
    //

    if(!(m_ahwndControls[IDC_MAIN_BANKLIST] = CreateListView(IDC_MAIN_BANKLIST)))
    {
        *plResult = -1;
        return TRUE;
    }

    if(!(m_ahwndControls[IDC_MAIN_ENTRYLIST] = CreateListView(IDC_MAIN_ENTRYLIST)))
    {
        *plResult = -1;
        return TRUE;
    }

    //
    // Initialize the MRU list
    //

    hSubMenu = GetSubMenu(m_hmenu, 0);
    hSubMenu = GetSubMenu(hSubMenu, 6);
    
    m_MRU.Initialize(WBND_REG_KEY, hSubMenu, ID_FILE_MRU_FILE1);

    //
    // Load popup menus
    //

    m_ahmenuPopup[IDC_MAIN_BANKLIST] = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDM_BANK_POPUP));
    m_ahmenuPopup[IDC_MAIN_ENTRYLIST] = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDM_ENTRY_POPUP));

    return fHandled;
}


/****************************************************************************
 *
 *  CreateListView
 *
 *  Description:
 *      Creates a list view.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *
 *  Returns:  
 *      HWND: child window handle.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::CreateListView"

HWND
CMainWindow::CreateListView
(
    UINT                    nCommandId
)
{
    static const DWORD      dwStyle         = WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE | LVS_EDITLABELS | LVS_REPORT | LVS_SHOWSELALWAYS;
    static const DWORD      dwStyleEx       = WS_EX_CLIENTEDGE;
    static const DWORD      dwLvStyleEx     = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
    LPCLVCOLUMNDATAPTR      pColumnData     = &m_aColumnData[nCommandId];
    HWND                    hwnd;
    LVCOLUMN                lvc;

    ASSERT(nCommandId < NUMELMS(m_aColumnData));
    
    lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 0;

    if(hwnd = CreateWindowEx(dwStyleEx, WC_LISTVIEW, NULL, dwStyle, 0, 0, 0, 0, m_hwnd, NULL, g_hInstance, NULL))
    {
        SetWindowLong(hwnd, GWL_ID, nCommandId);
        
        ListView_SetExtendedListViewStyleEx(hwnd, dwLvStyleEx, dwLvStyleEx);

#ifdef USE_XBOX_COLORS

        ListView_SetBkColor(hwnd, XBOX_BLACK);
        ListView_SetTextBkColor(hwnd, XBOX_BLACK);
        ListView_SetTextColor(hwnd, XBOX_GREEN);

#endif // USE_XBOX_COLORS
        
        for(; lvc.iSubItem < (int)pColumnData->nColumnCount; lvc.iSubItem++)
        {
            lvc.pszText = (LPSTR)pColumnData->paColumnData[lvc.iSubItem].pszText;
    
            ListView_InsertColumn(hwnd, lvc.iSubItem, &lvc);
            ListView_AutoSizeColumn(hwnd, lvc.iSubItem);
        }
    }

    return hwnd;
}


/****************************************************************************
 *
 *  CreateToolBar
 *
 *  Description:
 *      Creates the main window's toolbar.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HWND: child window handle.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::CreateToolBar"

HWND
CMainWindow::CreateToolBar
(
    void
)
{
    #define MAKE_TOOLBAR_BUTTON(command, image) \
        { image, command, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, 0 }

    #define MAKE_TOOLBAR_CHECKBUTTON(command, image) \
        { image, command, TBSTATE_ENABLED, BTNS_CHECK, { 0, 0 }, 0, 0 }

    #define MAKE_TOOLBAR_SEPARATOR() \
        { 0, IDC_STATIC, TBSTATE_ENABLED, BTNS_SEP, { 0, 0 }, 0, 0 }
    
    static const TBBUTTON   aButtons[] =
    {
        MAKE_TOOLBAR_BUTTON(ID_FILE_NEW, BTN_LEAF),
        MAKE_TOOLBAR_SEPARATOR(),
        MAKE_TOOLBAR_BUTTON(ID_FILE_OPEN, BTN_OPEN),
        MAKE_TOOLBAR_BUTTON(ID_FILE_SAVE, BTN_SAVE),
        MAKE_TOOLBAR_SEPARATOR(),
        MAKE_TOOLBAR_BUTTON(ID_BANK_ADD, BTN_BANK),
        MAKE_TOOLBAR_BUTTON(ID_ENTRY_ADD, BTN_ENTRY),
        MAKE_TOOLBAR_SEPARATOR(),
        MAKE_TOOLBAR_CHECKBUTTON(ID_CONVERT_ADPCM, BTN_ADPCM),
        MAKE_TOOLBAR_CHECKBUTTON(ID_CONVERT_8BIT, BTN_8BIT),
        MAKE_TOOLBAR_SEPARATOR(),
        MAKE_TOOLBAR_BUTTON(ID_ENTRY_AUDITION, BTN_AUDITION),
        MAKE_TOOLBAR_BUTTON(ID_ENTRY_CANCEL_AUDITION, BTN_STOP),
        MAKE_TOOLBAR_SEPARATOR(),
        MAKE_TOOLBAR_BUTTON(ID_PROJECT_GENERATE, BTN_GENERATE),
    };
    
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
    UINT                    nBitmaps;
    HWND                    hwnd;
    UINT                    i;

    for(i = 0, nBitmaps = 0; i < NUMELMS(aButtons); i++)
    {
        if(BTNS_SEP != aButtons[i].fsStyle)
        {
            nBitmaps++;
        }
    }

    return CreateToolbarEx(m_hwnd, dwStyle, IDC_MAIN_TOOLBAR, nBitmaps, g_hInstance, IDB_TOOLBAR, aButtons, NUMELMS(aButtons), 16, 16, 16, 16, sizeof(aButtons[0]));
}


/****************************************************************************
 *
 *  CreateStatusBar
 *
 *  Description:
 *      Creates the main window's status bar.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HWND: child window handle.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::CreateStatusBar"

HWND
CMainWindow::CreateStatusBar
(
    void
)
{
    static const DWORD      dwStyle = WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP;
    HWND                    hwnd;
    
    if(hwnd = CreateWindowEx(0, STATUSCLASSNAME, NULL, dwStyle, 0, 0, 0, 0, m_hwnd, NULL, g_hInstance, NULL))
    {
        SetWindowLong(hwnd, GWL_ID, IDC_MAIN_STATUSBAR);
    }

    return hwnd;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles destruction of the window.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnDestroy"

BOOL
CMainWindow::OnDestroy
(
    LRESULT *               plResult
)
{
    HKEY                    hkey;
    WINDOWPLACEMENT         wp;
    BOOL                    fHandled;
    
    fHandled = CWindow::OnDestroy(plResult);

    //
    // Save window placement
    //
    
    if(ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, WBND_REG_KEY, &hkey))
    {
        wp.length = sizeof(wp);
        
        if(GetWindowPlacement(m_hwnd, &wp))
        {
            RegSetValueEx(hkey, WBND_REG_VAL_WNDPOS, 0, REG_BINARY, (const BYTE *)&wp, sizeof(wp));
        }

        RegCloseKey(hkey);
    }

    //
    // Bail
    //

    PostQuitMessage(0);

    return fHandled;
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles resizing of the window.
 *
 *  Arguments:
 *      DWORD [in]: resize flags.
 *      int [in]: window width.
 *      int [in]: window height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnSize"

BOOL
CMainWindow::OnSize
(
    DWORD                   dwResize, 
    int                     nWidth, 
    int                     nHeight,
    LRESULT *               plResult
)
{
    static const int        nMargin             = 7;
    static const int        nBankListPercent    = 35;
    BOOL                    fHandled;
    RECT                    rcClient;
    RECT                    rcToolBar;
    RECT                    rcStatusBar;
    RECT                    rcBankList;
    RECT                    rcEntryList;
    
    fHandled = CWindow::OnSize(dwResize, nWidth, nHeight, plResult);

    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = nWidth;
    rcClient.bottom = nHeight;

    //
    // Resize toolbar and status bar
    //
    
    SendMessage(m_ahwndControls[IDC_MAIN_TOOLBAR], WM_SIZE, dwResize, MAKELPARAM(nWidth, nHeight));
    SendMessage(m_ahwndControls[IDC_MAIN_STATUSBAR], WM_SIZE, dwResize, MAKELPARAM(nWidth, nHeight));

    GetWindowRect(m_ahwndControls[IDC_MAIN_TOOLBAR], &rcToolBar);
    GetWindowRect(m_ahwndControls[IDC_MAIN_STATUSBAR], &rcStatusBar);

    rcClient.top += rcToolBar.bottom - rcToolBar.top;
    rcClient.bottom -= rcStatusBar.bottom - rcStatusBar.top;

    //
    // Resize list views
    //

    rcClient.left += nMargin;
    rcClient.top += nMargin;
    rcClient.right -= nMargin;
    rcClient.bottom -= nMargin;

    rcBankList.left = rcClient.left;
    rcBankList.top = rcClient.top;
    rcBankList.right = rcBankList.left + ((rcClient.right - rcClient.left) * nBankListPercent / 100);
    rcBankList.bottom = rcClient.bottom;

    rcEntryList.left = rcBankList.right + nMargin;
    rcEntryList.top = rcClient.top;
    rcEntryList.right = rcClient.right;
    rcEntryList.bottom = rcClient.bottom;
    
    MoveWindow(m_ahwndControls[IDC_MAIN_BANKLIST], rcBankList);
    MoveWindow(m_ahwndControls[IDC_MAIN_ENTRYLIST], rcEntryList);

    return fHandled;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Command message dispatcher.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      UINT [in]: control identifier.
 *      HWND [in]: control window handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnCommand"

BOOL
CMainWindow::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hwndControl,
    LRESULT *               plResult
)
{
    BOOL                    fHandled;

    fHandled = CWindow::OnCommand(nCommandId, nControlId, hwndControl, plResult);

    if((nControlId >= ID_FILE_MRU_FILE1) && (nControlId < ID_FILE_MRU_FILE1 + MRU_MAX_COUNT))
    {
        OnOpenProject(m_MRU.m_aszPaths[nControlId - ID_FILE_MRU_FILE1]);
    }
    else
    {
        switch(nControlId)
        {
            case ID_FILE_NEW:
                OnNewProject();
                break;

            case ID_FILE_OPEN:
                OnOpenProject();
                break;

            case ID_FILE_SAVE:
                OnSaveProject(FALSE);
                break;

            case ID_FILE_SAVE_AS:
                OnSaveProject(TRUE);
                break;

            case ID_BANK_ADD:
                OnNewBank();
                break;
        
            case ID_BANK_RENAME:
                OnRename(m_ahwndControls[IDC_MAIN_BANKLIST]);
                break;
        
            case ID_BANK_REMOVE:
                OnRemoveBank();
                break;
        
            case ID_ENTRY_ADD:
                OnNewEntry();
                break;
        
            case ID_ENTRY_RENAME:
                OnRename(m_ahwndControls[IDC_MAIN_ENTRYLIST]);
                break;
        
            case ID_ENTRY_REMOVE:
                OnRemoveEntry();
                break;

            case ID_CONVERT_ADPCM:
                OnConvert(WBFILTER_ADPCM);
                break;

            case ID_CONVERT_8BIT:
                OnConvert(WBFILTER_8BIT);
                break;

            case ID_PROJECT_GENERATE:
                OnGenerate();
                break;
        
            case ID_RENAME:
                OnRename();
                break;
        
            case ID_DELETE:
                OnDelete();
                break;

            case ID_SELECT_ALL:
                OnSelectAll();
                break;
        
            case ID_FILE_EXIT:
                OnExit();
                break;

            case ID_HELP_ABOUT:
                OnAbout();
                break;

            case ID_REFRESH:
                OnRefresh();
                break;

            case ID_ENTRY_AUDITION:
                OnAudition();
                break;

            case ID_ENTRY_CANCEL_AUDITION:
                OnCancelAudition();
                break;
        }
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnExit
 *
 *  Description:
 *      Exits the application.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnExit"

void
CMainWindow::OnExit
(
    void
)
{
    if(!CheckDirty())
    {
        return;
    }

    DestroyWindow(m_hwnd);
}


/****************************************************************************
 *
 *  OnAbout
 *
 *  Description:
 *      Displays information about the application.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnAbout"

void
CMainWindow::OnAbout
(
    void
)
{   

#if 1

    HICON                   hicon;
    CHAR                    szText[0x100];
    CHAR                    szVersion[0x100];

    hicon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_PROJECT), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    GetWaveBundlerVersion(szVersion);

    sprintf(szText, "XDK version %s", szVersion);

    ShellAbout(m_hwnd, g_pszAppTitle, szText, hicon);

    DestroyIcon(hicon);

#else

    CAboutDialog            AboutDialog;

    AboutDialog.Create(this);

#endif

}


/****************************************************************************
 *
 *  OnNotify
 *
 *  Description:
 *      Dispatches notification messages.
 *
 *  Arguments:
 *      LPNMHDR [in]: notification header.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnNotify"

BOOL
CMainWindow::OnNotify
(
    LPNMHDR                 pHdr,
    LRESULT *               plResult
)
{
    BOOL                    fHandled;

    fHandled = CWindow::OnNotify(pHdr, plResult);

    switch(pHdr->code)
    {
        case TTN_GETDISPINFO:
            OnToolTipDisplayInfo((LPNMTTDISPINFO)pHdr);
            break;

        case LVN_ITEMCHANGED:
            OnListViewItemChanged((LPNMLISTVIEW)pHdr);
            break;

        case LVN_COLUMNCLICK:
            OnListViewColumnClick((LPNMLISTVIEW)pHdr);
            break;

        case LVN_BEGINLABELEDIT:
            OnListViewBeginLabelEdit((NMLVDISPINFO *)pHdr);
            break;

        case LVN_ENDLABELEDIT:
            OnListViewEndLabelEdit((NMLVDISPINFO *)pHdr);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnToolTipDisplayInfo
 *
 *  Description:
 *      Gets display information for a tool-tip.
 *
 *  Arguments:
 *      LPTOOLTIPTEXT [in/out]: tool tip data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnToolTipDisplayInfo"

void
CMainWindow::OnToolTipDisplayInfo
(
    LPNMTTDISPINFO          pInfo
)
{
    pInfo->hinst = g_hInstance;
    pInfo->lpszText = MAKEINTRESOURCE(pInfo->hdr.idFrom);
}


/****************************************************************************
 *
 *  OnListViewItemChanged
 *
 *  Description:
 *      Notifies that a list view item has changed in some way.
 *
 *  Arguments:
 *      LPNMLISTVIEW [in]: notification data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnListViewItemChanged"

void
CMainWindow::OnListViewItemChanged
(
    LPNMLISTVIEW            pInfo
)
{
    if(pInfo->uChanged & LVIF_STATE)
    {
        if((pInfo->uOldState & LVIS_SELECTED) || (pInfo->uNewState & LVIS_SELECTED))
        {
            switch(pInfo->hdr.idFrom)
            {
                case IDC_MAIN_BANKLIST:
                    OnBankListItemStateChange();
                    break;

                case IDC_MAIN_ENTRYLIST:
                    OnEntryListItemStateChange();
                    break;
            }
        }
    }
}


/****************************************************************************
 *
 *  OnBankListItemStateChange
 *
 *  Description:
 *      Notifies that a list view item has changed in some way.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnBankListItemStateChange"

void
CMainWindow::OnBankListItemStateChange
(
    void
)
{
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_BANKLIST];
    int                     nSelCount;
    int                     nSelItem;
    
    //
    // If only one bank is selected, activate it
    //

    if(m_pProject)
    {
        if(1 == (nSelCount = ListView_GetSelectedCount(hwnd)))
        {
            nSelItem = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
        }
        else
        {
            nSelItem = -1;
        }

        m_pProject->SelectBank(nSelItem);
    }

    //
    // Update command states
    //

    SetCommandStates();
}


/****************************************************************************
 *
 *  OnEntryListItemStateChange
 *
 *  Description:
 *      Notifies that a list view item has changed in some way.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnEntryListItemStateChange"

void
CMainWindow::OnEntryListItemStateChange
(
    void
)
{
    //
    // Update command states
    //
    
    SetCommandStates();
}


/****************************************************************************
 *
 *  OnListViewColumnClick
 *
 *  Description:
 *      Notifies that a list view column has been clicked.
 *
 *  Arguments:
 *      LPNMLISTVIEW [in]: notification data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnListViewColumnClick"

void
CMainWindow::OnListViewColumnClick
(
    LPNMLISTVIEW            pInfo
)
{
    if(!m_pProject)
    {
        return;
    }
    
    switch(pInfo->hdr.idFrom)
    {
        case IDC_MAIN_BANKLIST:
            m_pProject->SortBankList(pInfo->iSubItem);

            MakeDirty();

            break;

        case IDC_MAIN_ENTRYLIST:
            m_pProject->SortEntryList(pInfo->iSubItem);

            MakeDirty();

            break;
    }
}


/****************************************************************************
 *
 *  OnListViewBeginLabelEdit
 *
 *  Description:
 *      Handles the start of a list-view item edit operation.
 *
 *  Arguments:
 *      NMLVDISPINFO * [in]: notification data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnListViewBeginLabelEdit"

void
CMainWindow::OnListViewBeginLabelEdit
(
    NMLVDISPINFO *          pInfo
)
{
    EnableAccelerators(FALSE);
}


/****************************************************************************
 *
 *  OnListViewEndLabelEdit
 *
 *  Description:
 *      Handles the completion of a list-view item edit operation.
 *
 *  Arguments:
 *      NMLVDISPINFO * [in]: notification data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnListViewEndLabelEdit"

void
CMainWindow::OnListViewEndLabelEdit
(
    NMLVDISPINFO *          pInfo
)
{
    CGuiWaveBank *         pBank;
    CGuiWaveBankEntry *    pEntry;
    
    EnableAccelerators(TRUE);

    if(!m_pProject)
    {
        return;
    }

    if(!pInfo->item.pszText)
    {
        return;
    }

    if(!*pInfo->item.pszText)
    {
        return;
    }

    if(IDC_MAIN_BANKLIST == pInfo->hdr.idFrom)
    {
        if(pBank = (CGuiWaveBank *)ListView_GetItemParam(m_ahwndControls[IDC_MAIN_BANKLIST], pInfo->item.iItem))
        {
            pBank->SetName(pInfo->item.pszText);

            MakeDirty();
        }
        else
        {
            ASSERTMSG("Bad item data");
        }
    }
    else if(IDC_MAIN_ENTRYLIST == pInfo->hdr.idFrom)
    {
        if(pEntry = (CGuiWaveBankEntry *)ListView_GetItemParam(m_ahwndControls[IDC_MAIN_ENTRYLIST], pInfo->item.iItem))
        {
            if(SUCCEEDED(pEntry->SetName(pInfo->item.pszText)))
            {
                MakeDirty();
            }
        }
        else
        {
            ASSERTMSG("Bad item data");
        }
    }
}


/****************************************************************************
 *
 *  OnContextMenu
 *
 *  Description:
 *      Handles right-click notifications for all windows.
 *
 *  Arguments:
 *      HWND [in]: window that received the message.
 *      int [in]: x-coordinate.
 *      int [in]: y-coordinate.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnContextMenu"

BOOL
CMainWindow::OnContextMenu
(
    HWND                    hwnd, 
    int                     x, 
    int                     y, 
    LRESULT *               plResult
)
{
    HMENU                   hmenu;
    BOOL                    fHandled;

    fHandled = CWindow::OnContextMenu(hwnd, x, y, plResult);
    
    if(hwnd == m_ahwndControls[IDC_MAIN_BANKLIST])
    {
        hmenu = GetSubMenu(m_ahmenuPopup[IDC_MAIN_BANKLIST], 0);
    }
    else if(hwnd == m_ahwndControls[IDC_MAIN_ENTRYLIST])
    {
        hmenu = GetSubMenu(m_ahmenuPopup[IDC_MAIN_ENTRYLIST], 0);
    }
    else
    {
        hmenu = NULL;
    }
    
    if(hmenu)
    {
        TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, x, y, 0, m_hwnd, NULL);
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnMenuSelect
 *
 *  Description:
 *      Handles selection of a menu item.
 *
 *  Arguments:
 *      UINT [in]: menu item identifier.
 *      UINT [in]: flags.
 *      HMENU [in]: menu handle.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnMenuSelect"

BOOL
CMainWindow::OnMenuSelect
(
    UINT                    nMenuItem, 
    UINT                    nFlags, 
    HMENU                   hMenu, 
    LRESULT *               plResult
)
{
    CHAR                    szText[0x100];
    BOOL                    fHandled;

    fHandled = CWindow::OnMenuSelect(nMenuItem, nFlags, hMenu, plResult);

    szText[0] = 0;

    if(!(nFlags & (MF_SYSMENU | MF_POPUP)))
    {
        LoadString(g_hInstance, nMenuItem, szText, NUMELMS(szText));
    }

    SetWindowText(m_ahwndControls[IDC_MAIN_STATUSBAR], szText);

    return fHandled;
}


/****************************************************************************
 *
 *  OnNewProject
 *
 *  Description:
 *      Creates a new project.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to prompt for a bank to be added to the project.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnNewProject"

void
CMainWindow::OnNewProject
(
    BOOL                    fAddBank
)
{
    HRESULT                 hr;
    
    if(!CheckDirty())
    {
        return;
    }

    //
    // Delete the old project
    //
    
    DELETE(m_pProject);
    
    //
    // Update UI
    //

    UpdateAppTitle();
    SetCommandStates();

    //
    // Create a new project
    //
    
    hr = HRFROMP(m_pProject = NEW(CGuiWaveBankProject(this)));

    if(SUCCEEDED(hr))
    {
        m_pProject->Attach(m_ahwndControls[IDC_MAIN_BANKLIST], m_ahwndControls[IDC_MAIN_ENTRYLIST]);
    }

    if(SUCCEEDED(hr))
    {
        UpdateAppTitle();
    }

    //
    // Add a bank to the new project
    //

    if(SUCCEEDED(hr) && fAddBank)
    {
        OnNewBank();
    }
}


/****************************************************************************
 *
 *  OnNewBank
 *
 *  Description:
 *      Creates a new bank.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to prompt for entries to be added to the bank.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnNewBank"

void
CMainWindow::OnNewBank
(
    BOOL                    fAddEntries
)
{
    HRESULT                 hr;
    
    if(!m_pProject)
    {
        return;
    }

    hr = m_pProject->AddBank();

    if(SUCCEEDED(hr))
    {
        MakeDirty();
    }

    if(SUCCEEDED(hr) && fAddEntries)
    {
        OnNewEntry();
    }
}


/****************************************************************************
 *
 *  OnRemoveBank
 *
 *  Description:
 *      Removes the selected bank(s).
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnRemoveBank"

void
CMainWindow::OnRemoveBank
(
    void
)
{
    static const LPCSTR     pszSingle   = "Are you sure you want to remove the bank from the project?";
    static const LPCSTR     pszMultiple = "Are you sure you want to remove the selected banks from the project?";
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_BANKLIST];
    int                     nItemCount;
    int                     i;
    
    if(!m_pProject)
    {
        return;
    }

    if((nItemCount = ListView_GetSelectedCount(hwnd)) > 0)
    {
        if(IDYES == MessageBox(nItemCount > 1 ? pszMultiple : pszSingle, MB_ICONQUESTION | MB_YESNO))
        {
            while(TRUE)
            {
                if(-1 == (i = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED)))
                {
                    break;
                }

                m_pProject->RemoveBank(i);
            }

            MakeDirty();
        }
    }
}


/****************************************************************************
 *
 *  OnNewEntry
 *
 *  Description:
 *      Creates a new entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnNewEntry"

void
CMainWindow::OnNewEntry
(
    void
)
{
    if(!m_pProject)
    {
        return;
    }

    if(SUCCEEDED(m_pProject->AddEntry()))
    {
        MakeDirty();
    }
}


/****************************************************************************
 *
 *  OnRemoveEntry
 *
 *  Description:
 *      Removes the selected entry or entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnRemoveEntry"

void
CMainWindow::OnRemoveEntry
(
    void
)
{
    static const LPCSTR     pszSingle   = "Are you sure you want to remove the entry from the bank?";
    static const LPCSTR     pszMultiple = "Are you sure you want to remove the selected entries from the bank?";
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_ENTRYLIST];
    int                     nItemCount;
    int                     i;
    
    if(!m_pProject)
    {
        return;
    }

    if((nItemCount = ListView_GetSelectedCount(hwnd)) > 0)
    {
        if(IDYES == MessageBox(nItemCount > 1 ? pszMultiple : pszSingle, MB_ICONQUESTION | MB_YESNO))
        {
            while(TRUE)
            {
                if(-1 == (i = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED)))
                {
                    break;
                }

                m_pProject->RemoveEntry(i);
            }

            MakeDirty();
        }
    }
}


/****************************************************************************
 *
 *  OnConvert
 *
 *  Description:
 *      Toggles the conversion flags for each selected entry.
 *
 *  Arguments:
 *      DWORD [in]: flags mask.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnConvert"

void
CMainWindow::OnConvert
(
    DWORD                   dwMask
)
{
    static const LPCSTR     pszConfusion    = "Not all of the selected entries are able to be converted";
    HWND                    hwnd            = m_ahwndControls[IDC_MAIN_ENTRYLIST];
    DWORD                   dwFlags         = 0;
    int                     nChanged        = 0;
    int                     nItemCount;
    CGuiWaveBankEntry *     pEntry;
    int                     i;
    
    if(!m_pProject)
    {
        return;
    }

    //
    // If any selected items are compressed, we'll uncompress and vice-versa
    //

    if((nItemCount = ListView_GetSelectedCount(hwnd)) > 0)
    {
        i = -1;
        
        while(TRUE)
        {
            if(-1 == (i = ListView_GetNextItem(hwnd, i, LVNI_SELECTED)))
            {
                break;
            }

            pEntry = (CGuiWaveBankEntry *)ListView_GetItemParam(hwnd, i);
            ASSERT(pEntry);

            dwFlags |= pEntry->m_dwFlags;
        }

        dwFlags = ~dwFlags & dwMask;

        i = -1;
        
        while(TRUE)
        {
            if(-1 == (i = ListView_GetNextItem(hwnd, i, LVNI_SELECTED)))
            {
                break;
            }

            pEntry = (CGuiWaveBankEntry *)ListView_GetItemParam(hwnd, i);
            ASSERT(pEntry);

            if(dwFlags == pEntry->SetFlags(WBFILTER_MASK, dwFlags))
            {
                nChanged++;
            }

            pEntry->UpdateListView();
        }

        MakeDirty();

        OnEntryListItemStateChange();

        if(nChanged < nItemCount)
        {
            MessageBox(pszConfusion, MB_OK | MB_ICONINFORMATION);
        }
    }
}


/****************************************************************************
 *
 *  OnGenerate
 *
 *  Description:
 *      Generates wave bank files.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnGenerate"

void
CMainWindow::OnGenerate
(
    void
)
{

#if 0

    static const LPCSTR     pszMessage  = "All entries were successfully written";
    HRESULT                 hr;
    
    if(!m_pProject)
    {
        return;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));
    
    hr = m_pProject->Generate(this);

    SendMessage(m_ahwndControls[IDC_MAIN_STATUSBAR], SB_SETTEXT, 0, NULL);

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    
    if(SUCCEEDED(hr))
    {
        MessageBox(pszMessage, MB_OK | MB_ICONINFORMATION);
    }

#else

    m_pProject->Generate();

#endif
    
}


/****************************************************************************
 *
 *  BeginEntry
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: entry file name.
 *      DWORD [in]: entry flags.
 *      UINT [in]: entry index.
 *      UINT [in]: entry count.
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::BeginEntry"

BOOL
CMainWindow::BeginEntry
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags
)
{
    CHAR                    szText[MAX_PATH + 0x100];

    strcpy(szText, "Processing ");

    strcat(szText, pszFile);
    // _splitpath(pszFile, NULL, NULL, &szText[strlen(szText)], NULL);

    SendMessage(m_ahwndControls[IDC_MAIN_STATUSBAR], SB_SETTEXT, 0, (LPARAM)szText);

    return TRUE;
}


/****************************************************************************
 *
 *  OnSaveProject
 *
 *  Description:
 *      Saves the current project.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to prompt for a file name.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnSaveProject"

void
CMainWindow::OnSaveProject
(
    BOOL                    fPrompt
)
{
    if(!m_pProject)
    {
        return;
    }

    if(SUCCEEDED(m_pProject->Save(fPrompt)))
    {
        m_MRU.AddItem(m_pProject->m_szProjectFile);

        MakeDirty(FALSE, TRUE);
    }
}


/****************************************************************************
 *
 *  OnOpenProject
 *
 *  Description:
 *      Opens a project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnOpenProject"

void
CMainWindow::OnOpenProject
(
    LPCSTR                  pszProjectFile
)
{
    CHAR                    szPath[MAX_PATH];
    LPSTR                   psz;
    HRESULT                 hr;

    //
    // Strip any quotes from the path
    //

    if(pszProjectFile)
    {
        psz = szPath;
        
        while(TRUE)
        {
            if('\"' == *pszProjectFile)
            {
                pszProjectFile++;
                continue;
            }

            if(!(*psz++ = *pszProjectFile++))
            {
                break;
            }
        }

        pszProjectFile = szPath;
    }

    //
    // Open the project
    //

    if(pszProjectFile && m_pProject)
    {
        if(!_strcmpi(pszProjectFile, m_pProject->m_szProjectFile))
        {
            return;
        }
    }

    if(!CheckDirty())
    {
        return;
    }

    MakeDirty(FALSE);

    DELETE(m_pProject);

    hr = HRFROMP(m_pProject = NEW(CGuiWaveBankProject(this)));

    if(SUCCEEDED(hr))
    {
        m_pProject->Attach(m_ahwndControls[IDC_MAIN_BANKLIST], m_ahwndControls[IDC_MAIN_ENTRYLIST]);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pProject->Open(pszProjectFile);
    }

    if(SUCCEEDED(hr))
    {
        m_MRU.AddItem(m_pProject->m_szProjectFile);
    }

    if(SUCCEEDED(hr))
    {
        UpdateAppTitle();
    }

    //
    // If we failed to open the project, create an empty one
    //

    if(FAILED(hr))
    {
        OnNewProject(FALSE);
    }
}


/****************************************************************************
 *
 *  OnDelete
 *
 *  Description:
 *      Handles the generic delete command.
 *
 *  Arguments:
 *      HWND [in]: window handle that received the message.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnDelete"

void
CMainWindow::OnDelete
(
    HWND                    hwnd
)
{
    if(!hwnd)
    {
        hwnd = GetFocus();
    }

    if(hwnd == m_ahwndControls[IDC_MAIN_BANKLIST])
    {
        OnRemoveBank();
    }
    else if(hwnd == m_ahwndControls[IDC_MAIN_ENTRYLIST])
    {
        OnRemoveEntry();
    }
}


/****************************************************************************
 *
 *  OnRename
 *
 *  Description:
 *      Handles the generic rename command.
 *
 *  Arguments:
 *      HWND [in]: window handle that received the message.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnRename"

void
CMainWindow::OnRename
(
    HWND                    hwnd
)
{
    int                     i;

    if(!hwnd)
    {
        hwnd = GetFocus();
    }

    if((hwnd == m_ahwndControls[IDC_MAIN_BANKLIST]) || (hwnd == m_ahwndControls[IDC_MAIN_ENTRYLIST]))
    {
        if(1 == ListView_GetSelectedCount(hwnd))
        {
            if(-1 != (i = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED)))
            {
                ListView_EditLabel(hwnd, i);
            }
        }
    }
}


/****************************************************************************
 *
 *  OnSelectAll
 *
 *  Description:
 *      Selects all entries in a list.
 *
 *  Arguments:
 *      HWND [in]: window handle that received the message.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnSelectAll"

void
CMainWindow::OnSelectAll
(
    HWND                    hwnd
)
{
    int                     nItemCount;
    int                     i;

    if(!hwnd)
    {
        hwnd = GetFocus();
    }
    
    if((hwnd == m_ahwndControls[IDC_MAIN_BANKLIST]) || (hwnd == m_ahwndControls[IDC_MAIN_ENTRYLIST]))
    {
        nItemCount = ListView_GetItemCount(hwnd);

        for(i = 0; i < nItemCount; i++)
        {
            ListView_SetItemState(hwnd, i, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}


/****************************************************************************
 *
 *  OnClose
 *
 *  Description:
 *      Handles closing the window.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnClose"

BOOL
CMainWindow::OnClose
(
    LRESULT *               plResult
)
{
    OnExit();
    return FALSE;
}


/****************************************************************************
 *
 *  CheckDirty
 *
 *  Description:
 *      Prompts the user to save if the project has changed.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing; FALSE to cancel.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::CheckDirty"

BOOL
CMainWindow::CheckDirty
(
    void
)
{
    static const LPCSTR     pszMessage                  = "Save changes to %s?";
    static const LPCSTR     pszNoName                   = "New Project";
    CHAR                    szMessage[MAX_PATH + 0x100];
    UINT                    nResult;
    
    if(!m_pProject || !m_fDirty)
    {
        return TRUE;
    }

    sprintf(szMessage, pszMessage, m_pProject->m_szProjectFile[0] ? m_pProject->m_szProjectFile : pszNoName);

    nResult = MessageBox(szMessage, MB_ICONQUESTION | MB_YESNOCANCEL);

    if(IDYES == nResult)
    {
        OnSaveProject(FALSE);

        return !m_fDirty;
    }
    else if(IDNO == nResult)
    {
        return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  MakeDirty
 *
 *  Description:
 *      Updates the dirty flag.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to flag the project as dirty.
 *      BOOL [in]: TRUE to force an update.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::MakeDirty"

void
CMainWindow::MakeDirty
(
    BOOL                    fDirty,
    BOOL                    fForce
)
{
    if((fDirty != m_fDirty) || fForce)
    {
        m_fDirty = fDirty;

        UpdateAppTitle();
    }
}


/****************************************************************************
 *
 *  UpdateAppTitle
 *
 *  Description:
 *      Updates the application title.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::UpdateAppTitle"

void
CMainWindow::UpdateAppTitle
(
    void
)
{
    CHAR                    szText[MAX_PATH + 0x100];

    if(m_pProject)
    {
        if(m_pProject->m_szProjectFile[0])
        {
            _splitpath(m_pProject->m_szProjectFile, NULL, NULL, szText, NULL);
        }
        else
        {
            strcpy(szText, "New Project");
        }
        
        if(m_fDirty)
        {
            strcat(szText, " *");
        }
        
        strcat(szText, " - ");
        strcat(szText, g_pszAppTitle);

        SetWindowText(m_hwnd, szText);
    }
    else
    {
        SetWindowText(m_hwnd, g_pszAppTitle);
    }
}


/****************************************************************************
 *
 *  SetCommandState
 *
 *  Description:
 *      En/disables menu, toolbar and accelerator commands based on current
 *      application state.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      BOOL [in]: TRUE to enable; FALSE to disable; -1 to skip.
 *      BOOL [in]: TRUE to check; FALSE to uncheck; -1 to skip.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::SetCommandState"

void
CMainWindow::SetCommandState
(
    UINT                    nCommandId,
    BOOL                    fEnable,
    BOOL                    fCheck
)
{
    static const DWORD      adwMenuSelected[] = 
    { 
        MF_BYCOMMAND | MF_GRAYED,
        MF_BYCOMMAND | MF_ENABLED 
    };

    static const DWORD      adwCheckState[] =
    {
        MF_BYCOMMAND | MF_UNCHECKED,
        MF_BYCOMMAND | MF_CHECKED
    };

    UINT                    i;

    if((TRUE == fEnable) || (FALSE == fEnable))
    {
        EnableMenuItem(m_hmenu, nCommandId, adwMenuSelected[fEnable]);

        for(i = 0; i < NUMELMS(m_ahmenuPopup); i++)
        {
            EnableMenuItem(m_ahmenuPopup[i], nCommandId, adwMenuSelected[fEnable]);
        }

        SendMessage(m_ahwndControls[IDC_MAIN_TOOLBAR], TB_ENABLEBUTTON, nCommandId, MAKELONG(fEnable, 0));
    }

    if((TRUE == fCheck) || (FALSE == fCheck))
    {
        CheckMenuItem(m_hmenu, nCommandId, adwCheckState[fCheck]);

        for(i = 0; i < NUMELMS(m_ahmenuPopup); i++)
        {
            CheckMenuItem(m_ahmenuPopup[i], nCommandId, adwCheckState[fCheck]);
        }

        SendMessage(m_ahwndControls[IDC_MAIN_TOOLBAR], TB_CHECKBUTTON, nCommandId, MAKELONG(fCheck, 0));
    }
}


/****************************************************************************
 *
 *  SetBankCommandStates
 *
 *  Description:
 *      En/disables menu, toolbar and accelerator commands based on current
 *      application state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::SetBankCommandStates"

void
CMainWindow::SetBankCommandStates
(
    void
)
{
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_BANKLIST];
    int                     nSelCount;

    //
    // Update command state based on the number of items currently selected
    // in the bank list
    //
    
    nSelCount = ListView_GetSelectedCount(hwnd);

    SetCommandState(ID_BANK_RENAME, (1 == nSelCount), -1);
    SetCommandState(ID_BANK_REMOVE, (nSelCount > 0), -1);
    SetCommandState(ID_ENTRY_ADD, (1 == nSelCount), -1);
}


/****************************************************************************
 *
 *  SetEntryCommandStates
 *
 *  Description:
 *      En/disables menu, toolbar and accelerator commands based on current
 *      application state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::SetEntryCommandStates"

void
CMainWindow::SetEntryCommandStates
(
    void
)
{
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_ENTRYLIST];
    int                     nAdpcmValid = 0;
    int                     nAdpcm      = 0;
    int                     n8BitValid  = 0;
    int                     n8Bit       = 0;
    int                     nSelCount   = 0;
    int                     i           = -1;
    DWORD                   dwValid;
    CGuiWaveBankEntry *    pEntry;

    //
    // Determine the number of selected items and the availability of 
    // conversion for each item.
    //

    while(TRUE)
    {
        if(-1 == (i = ListView_GetNextItem(hwnd, i, LVNI_SELECTED)))
        {
            break;
        }

        nSelCount++;    

        pEntry = (CGuiWaveBankEntry *)ListView_GetItemParam(hwnd, i);
        ASSERT(pEntry);

        dwValid = pEntry->GetValidFlags();

        if(dwValid & WBFILTER_ADPCM)
        {
            nAdpcmValid++;
            
            if(pEntry->m_dwFlags & WBFILTER_ADPCM)
            {
                nAdpcm++;
            }
        }
        else
        {
            ASSERT(!(pEntry->m_dwFlags & WBFILTER_ADPCM));
        }
        
        if(dwValid & WBFILTER_8BIT)
        {
            n8BitValid++;
            
            if(pEntry->m_dwFlags & WBFILTER_8BIT)
            {
                n8Bit++;
            }
        }
        else
        {
            ASSERT(!(pEntry->m_dwFlags & WBFILTER_8BIT));
        }
    }

    //
    // Update command state based on the number of items currently selected
    // in the entry list
    //
    
    SetCommandState(ID_ENTRY_RENAME, (1 == nSelCount), -1);
    SetCommandState(ID_ENTRY_REMOVE, (nSelCount > 0), -1);
    SetCommandState(ID_CONVERT_ADPCM, (nAdpcmValid > 0), (nAdpcm > 0));
    SetCommandState(ID_CONVERT_8BIT, (n8BitValid > 0), (n8Bit > 0));
    SetCommandState(ID_ENTRY_AUDITION, (nSelCount > 0), -1);
}


/****************************************************************************
 *
 *  SetCommandStates
 *
 *  Description:
 *      En/disables menu, toolbar and accelerator commands based on current
 *      application state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::SetCommandStates"

void
CMainWindow::SetCommandStates
(
    void
)
{
    SetBankCommandStates();
    SetEntryCommandStates();
}


/****************************************************************************
 *
 *  OnRefresh
 *
 *  Description:
 *      Refreshes project data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnRefresh"

void
CMainWindow::OnRefresh
(
    void
)
{
    if(!m_pProject)
    {
        return;
    }

    SetCursor(LoadCursor(NULL, IDC_WAIT));
    
    m_pProject->Refresh();

    SetCursor(LoadCursor(NULL, IDC_ARROW));
}


/****************************************************************************
 *
 *  OnAudition
 *
 *  Description:
 *      Auditions an entry.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnAudition"

void
CMainWindow::OnAudition
(
    void
)
{
    HWND                    hwnd        = m_ahwndControls[IDC_MAIN_ENTRYLIST];
    int                     i           = -1;
    CGuiWaveBankEntry *    pEntry;
    int                     nItemCount;
    
    if(!m_pProject)
    {
        return;
    }

    if(!ConnectRemote())
    {
        DPF_ERROR("Can't connect to audio console");
        return;
    }

    if((nItemCount = ListView_GetSelectedCount(hwnd)) > 0)
    {
        SetCursor(LoadCursor(NULL, IDC_WAIT));
    
        while(TRUE)
        {
            if(-1 == (i = ListView_GetNextItem(hwnd, i, LVNI_SELECTED)))
            {
                break;
            }

            pEntry = CGuiWaveBankEntry::GetEntry(hwnd, i);
            ASSERT(pEntry);

            pEntry->Audition(m_pxbdm);
        }

        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
}


/****************************************************************************
 *
 *  OnCancelAudition
 *
 *  Description:
 *      Cancels any autitioning entries.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::OnCancelAudition"

void
CMainWindow::OnCancelAudition
(
    void
)
{
    HWND                    hwnd;
    
    if(!ConnectRemote())
    {
        DPF_ERROR("Can't connect to audio console");
        return;
    }

    DmSendCommand(m_pxbdm, "XAUD!WaveBank.StopAll", NULL, NULL);
}


/****************************************************************************
 *
 *  ConnectRemote
 *
 *  Description:
 *      Connects to a remote instance of Audio Console.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainWindow::ConnectRemote"

BOOL
CMainWindow::ConnectRemote
(
    void
)
{
    HRESULT                 hr                  = S_OK;
    CHAR                    szCommand[0x100];
    
    if(!m_pxbdm)
    {
        if(XBDM_NOERR == (hr = DmOpenConnection(&m_pxbdm)))
        {
            hr = S_OK;
        }

        if(SUCCEEDED(hr))
        {
            sprintf(szCommand, "XAUD!connect %s", g_pszAppTitle);

            if(XBDM_NOERR == (hr = DmSendCommand(m_pxbdm, szCommand, NULL, NULL)))
            {
                hr = S_OK;
            }
        }

        if(FAILED(hr) && m_pxbdm)
        {
            DmCloseConnection(m_pxbdm);
            m_pxbdm = NULL;
        }
    }

    return SUCCEEDED(hr);
}


/****************************************************************************
 *
 *  CAboutDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAboutDialog::CAboutDialog"

CAboutDialog::CAboutDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CAboutDialog
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAboutDialog::~CAboutDialog"

CAboutDialog::~CAboutDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the dialog.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAboutDialog::Create"

BOOL
CAboutDialog::Create
(
    CWindow *               pParent
)
{
    return CModalDialog::Create(IDD, NULL, pParent);
}


/****************************************************************************
 *
 *  OnInitDialog
 *
 *  Description:
 *      Handles dialog initialization.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAboutDialog::OnInitDialog"

BOOL
CAboutDialog::OnInitDialog
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    CHAR                    szTemplate[0x100];
    CHAR                    szVersion[0x100];
    CHAR                    szText[0x100];
    SYSTEMTIME              time;
    BOOL                    fHandled;
    
    fHandled = CModalDialog::OnInitDialog(pvContext, plResult);
    
    //
    // Set text items and window title
    //

    GetWindowText(m_hwnd, szTemplate, NUMELMS(szTemplate));
    sprintf(szText, szTemplate, g_pszAppTitle);
    SetWindowText(m_hwnd, szText);

    GetDlgItemText(m_hwnd, IDC_APPNAME, szTemplate, NUMELMS(szTemplate));
    sprintf(szText, szTemplate, g_pszAppTitle);
    SetDlgItemText(m_hwnd, IDC_APPNAME, szText);

    GetDlgItemText(m_hwnd, IDC_VERSION, szTemplate, NUMELMS(szTemplate));
    GetWaveBundlerVersion(szVersion);
    sprintf(szText, szTemplate, szVersion);
    SetDlgItemText(m_hwnd, IDC_VERSION, szText);

    GetDlgItemText(m_hwnd, IDC_COPYRIGHT, szTemplate, NUMELMS(szTemplate));
    GetLocalTime(&time);
    sprintf(szText, szTemplate, time.wYear);
    SetDlgItemText(m_hwnd, IDC_COPYRIGHT, szText);

    //
    // Center the window
    //

    CenterWindow(m_hwnd, m_pParent ? m_pParent->m_hwnd : NULL);

    return fHandled;
}


