/***************************************************************************
 *
 *  Copyright (C) 2/4/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mainfrm.cpp
 *  Content:    Main window frame.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/4/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactapp.h"

const LPCTSTR CMainFrame::m_pszClassName = XACTGUI_MAKE_WNDCLASS_NAME("MainFrame");

const LPCTSTR CMainSplitter::m_pszClassName = XACTGUI_MAKE_WNDCLASS_NAME("MainSplitter");
const UINT CMainSplitter::m_nWidth = 4;

const LPCTSTR CMainProjectTree::m_pszWaveBankType = TEXT("Wave Bank");

const LPCTSTR CWorkspaceChild::m_pszClassName = XACTGUI_MAKE_WNDCLASS_NAME("WorkspaceChild");

CApplication *g_pApplication = NULL;


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
 *      int [in]: application show command.
 *
 *  Returns:  
 *      int: application return code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WinMain"

int WINAPI
WinMain
(
    HINSTANCE               hInstance,
    HINSTANCE               hPrevInstance,
    LPSTR                   pszCmdLine,
    int                     nShowCmd
)
{
    INITCOMMONCONTROLSEX    icc;
    int                     nResult;

    //
    // Initialize globals
    //

    CWaveBankEntry::m_fAutoLoad = FALSE;

    //
    // Initialize the common controls library
    //

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_UPDOWN_CLASS;
    
    if(!InitCommonControlsEx(&icc))
    {
        DPF_ERROR("InitCommonControlsEx failed");
        return -1;
    }

    //
    // Load the application title
    //
    
    if(!LoadString(hInstance, IDS_APPTITLE, g_szAppTitle, NUMELMS(g_szAppTitle)))
    {
        DPF_ERROR("Failed to load application title");
        return -1;
    }

#ifdef _DEBUG

    strcat(g_szAppTitle, " (debug)");

#endif // _DEBUG

    //
    // Create the application object
    //
    
    if(!(g_pApplication = NEW(CApplication)))
    {
        return -1;
    }
    
    nResult = g_pApplication->m_MainFrame.Create(nShowCmd);

    DELETE(g_pApplication);

    //
    // Check for leaks
    //

    CMemoryManager::DumpMemoryUsage(TRUE);

    return nResult;
}


/****************************************************************************
 *
 *  CApplication
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
#define DPF_FNAME "CApplication::CApplication"

CApplication::CApplication
(
    void
)
:   m_Registry(HKEY_XACT_APPLICATION_ROOT, REGKEY_XACT_APPLICATION_ROOT),
    m_MRU(m_Registry, REGKEY_PROJECT_MRU)
{
}


/****************************************************************************
 *
 *  ~CApplication
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
#define DPF_FNAME "CApplication::~CApplication"

CApplication::~CApplication
(
    void
)
{
}


/****************************************************************************
 *
 *  CMainFrame
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
#define DPF_FNAME "CMainFrame::CMainFrame"

CMainFrame::CMainFrame
(
    void
)
:   CMDIFrameWindow(IDM_MAIN, IDA_MAIN)
{
}


/****************************************************************************
 *
 *  ~CMainFrame
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
#define DPF_FNAME "CMainFrame::~CMainFrame"

CMainFrame::~CMainFrame
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
 *      Creates the window.
 *
 *  Arguments:
 *      int [in]: window show command.
 *
 *  Returns:  
 *      int: application exit code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::Create"

int
CMainFrame::Create
(
    int                     nShowCmd
)
{
    static const DWORD      dwExStyle   = 0;
    DWORD                   dwStyle     = WS_OVERLAPPEDWINDOW;
    WNDCLASSEX              wc          = { 0 };
    LPTSTR                  pszCmdLine  = NULL;
    LPTSTR                  pszProject  = NULL;
    WINDOWPLACEMENT         wp;
    HRESULT                 hr;

    //
    // Register the window class
    //

    wc.cbSize = sizeof(wc);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpfnWndProc = WindowProc;
    wc.hIcon = LoadResourceIconLarge(IDI_XACT);
    wc.hIconSm = LoadResourceIconSmall(IDI_XACT);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = g_hInstance;
    wc.lpszClassName = m_pszClassName;

    RegisterClassEx(&wc);

    //
    // Load window placement data
    //
    
    if(g_pApplication->m_Registry.GetBinaryValue(REGKEY_MAINFRAME_PLACEMENT, &wp, sizeof(wp)))
    {
        wp.rcNormalPosition.right -= wp.rcNormalPosition.left;
        wp.rcNormalPosition.bottom -= wp.rcNormalPosition.top;

        if((SW_SHOWNORMAL == nShowCmd) && (SW_MAXIMIZE == wp.showCmd))
        {
            nShowCmd = SW_SHOWMAXIMIZED;
        }
    }
    else
    {
        wp.rcNormalPosition.left = CW_USEDEFAULT;
        wp.rcNormalPosition.top = CW_USEDEFAULT;
        wp.rcNormalPosition.right = CW_USEDEFAULT;
        wp.rcNormalPosition.bottom = CW_USEDEFAULT;
    }

    //
    // Create the window
    //

    if(!CMDIFrameWindow::Create(m_pszClassName, g_szAppTitle, dwExStyle, dwStyle, wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom))
    {
        return -1;
    }

    ShowWindow(m_hWnd, nShowCmd);

    //
    // Load project data
    //

    if(ParseCommandLine(GetCommandLine(), &pszCmdLine, NULL))
    {
        pszProject = pszCmdLine + _tcslen(pszCmdLine) + 1;

        if(!*pszProject)
        {
            pszProject = NULL;
        }
    }

    if(pszProject)
    {
        g_pApplication->m_Project.Open(pszProject);
    }
    else
    {
        g_pApplication->m_Project.New();
    }
        
    MEMFREE(pszCmdLine);

    //
    // Enter the message pump
    //

    return PumpMessages();
}


/****************************************************************************
 *
 *  PumpMessages
 *
 *  Description:
 *      Window message pump.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      int: application exit code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::PumpMessages"

int
CMainFrame::PumpMessages
(
    void
)
{
    MSG                     msg;

    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if(!DispatchMessage(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    ASSERT(WM_QUIT == msg.message);

    return msg.wParam;
}


/****************************************************************************
 *
 *  HandleMessage
 *
 *  Description:
 *      Dispatches a window message to the appropriate handler.
 *
 *  Arguments:
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::HandleMessage"

BOOL 
CMainFrame::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_MENUSELECT:
            fHandled = OnMenuSelect(LOWORD(wParam), HIWORD(wParam), (HMENU)lParam, plResult);
            break;

        default:
            fHandled = CMDIFrameWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
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
#define DPF_FNAME "CMainFrame::OnCreate"

BOOL
CMainFrame::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    HMENU                   hFileMenu;
    HMENU                   hMRUMenu;
    BOOL                    fSuccess;

    if(CMDIFrameWindow::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Create main window controls
    //
    
    fSuccess = m_ToolBar.Create();
    
    if(fSuccess)
    {
        fSuccess = m_StatusBar.Create();
    }
    
    if(fSuccess)
    {
        fSuccess = m_ProjectTree.Create();
    }
    
    if(fSuccess)
    {
        fSuccess = m_Splitter.Create();
    }
    
    if(fSuccess)
    {
        fSuccess = m_Workspace.Create();
    }

    //
    // Attach the MRU object
    //

    if(fSuccess)
    {
        hFileMenu = GetSubMenu(m_hMenu, MENUPOS_FILE);
        hMRUMenu = GetSubMenu(hFileMenu, MENUPOS_FILE_MRU);
        
        g_pApplication->m_MRU.AttachMenu(hMRUMenu);
    }

    //
    // Load secondary menus
    //

    if(fSuccess)
    {
        m_ahMenus[MENUIDX_WAVEBANK_POPUP] = LoadPopupMenu(IDM_WAVEBANK_POPUP);
        SetMenuDefaultItem(m_ahMenus[MENUIDX_WAVEBANK_POPUP], 0, MF_BYPOSITION);

        m_ahMenus[MENUIDX_WAVEBANK_ENTRY_POPUP] = LoadPopupMenu(IDM_WAVEBANK_ENTRY_POPUP);
        SetMenuDefaultItem(m_ahMenus[MENUIDX_WAVEBANK_ENTRY_POPUP], 0, MF_BYPOSITION);
    }

    //
    // Set default command states
    //
    
    if(fSuccess)
    {
        SetDefaultCommandState();
    }

    if(!fSuccess)
    {
        *plResult = -1;
        return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles WM_SIZE messages.
 *
 *  Arguments:
 *      UINT [in]: resize type.
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnSize"

BOOL
CMainFrame::OnSize
(
    UINT                    nType,
    UINT                    nWidth,
    UINT                    nHeight,
    LRESULT *               plResult
)
{
    if(CMDIFrameWindow::OnSize(nType, nWidth, nHeight, plResult))
    {
        return TRUE;
    }

    if(SIZE_MINIMIZED == nType)
    {
        return FALSE;
    }
    
    LayoutControls(nWidth, nHeight);

    return FALSE;
}


/****************************************************************************
 *
 *  OnClose
 *
 *  Description:
 *      Handles WM_CLOSE messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnClose"

BOOL
CMainFrame::OnClose
(
    LRESULT *               plResult
)
{
    if(!g_pApplication->m_Project.CheckDirty())
    {
        *plResult = 0;
        return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles WM_DESTROY messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnDestroy"

BOOL
CMainFrame::OnDestroy
(
    LRESULT *               plResult
)
{
    WINDOWPLACEMENT         wp;
    
    if(CMDIFrameWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    //
    // Close all MDI child windows
    //

    OnCmdWindowCloseAll();

    //
    // Save the window placement data
    //

    wp.length = sizeof(wp);
    
    GetWindowPlacement(m_hWnd, &wp);

    g_pApplication->m_Registry.SetBinaryValue(REGKEY_MAINFRAME_PLACEMENT, &wp, sizeof(wp));

    //
    // Terminate the main thread
    //
    
    PostQuitMessage(0);

    return FALSE;
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
#define DPF_FNAME "CMainFrame::OnMenuSelect"

BOOL
CMainFrame::OnMenuSelect
(
    UINT                    nMenuItem, 
    UINT                    nFlags, 
    HMENU                   hMenu, 
    LRESULT *               plResult
)
{
    TCHAR                   szText[0x100];

    szText[0] = 0;

    if(g_pApplication->m_MRU.TranslateCommand(nMenuItem))
    {
        nMenuItem = ID_RECENT_FILE_0;
    }
    
    if(!(nFlags & (MF_SYSMENU | MF_POPUP)))
    {
        LoadString(g_hInstance, nMenuItem, szText, NUMELMS(szText));
    }

    m_StatusBar.SetWindowText(szText);

    return FALSE;
}


/****************************************************************************
 *
 *  OnNotify
 *
 *  Description:
 *      Handles WM_NOTIFY messages.
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
#define DPF_FNAME "CMainFrame::OnNotify"

BOOL
CMainFrame::OnNotify
(
    LPNMHDR                 pHdr,
    LRESULT *               plResult
)
{
    if(CMDIFrameWindow::OnNotify(pHdr, plResult))
    {
        return TRUE;
    }
    
    switch(pHdr->code)
    {
        case TTN_GETDISPINFO:
            OnToolTipDisplayInfo((LPNMTTDISPINFO)pHdr);
            break;
    }

    return FALSE;
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
#define DPF_FNAME "CMainFrame::OnToolTipDisplayInfo"

void
CMainFrame::OnToolTipDisplayInfo
(
    LPNMTTDISPINFO          pInfo
)
{
    pInfo->hinst = g_hInstance;
    pInfo->lpszText = MAKEINTRESOURCE(pInfo->hdr.idFrom);
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
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
#define DPF_FNAME "CMainFrame::OnCommand"

BOOL
CMainFrame::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    LPCTSTR                 pszPath;
    
    if(CMDIFrameWindow::OnCommand(nCommandId, nControlId, hWndControl, plResult))
    {
        return TRUE;
    }

    if(pszPath = g_pApplication->m_MRU.TranslateCommand(nControlId))
    {
        g_pApplication->m_Project.Open(pszPath);
        return FALSE;
    }
    
    switch(nControlId)
    {
        case ID_NEW_PROJECT:
            OnCmdNewProject();
            break;
        
        case ID_OPEN_PROJECT:
            OnCmdOpenProject();
            break;

        case ID_RELOAD_PROJECT:
            OnCmdReloadProject();
            break;

        case ID_SAVE_PROJECT:
            OnCmdSaveProject();
            break;

        case ID_SAVE_PROJECT_AS:
            OnCmdSaveProjectAs();
            break;
        
        case ID_EXIT:
            OnCmdExit();
            break;

        case ID_ABOUT:
            OnCmdAbout();
            break;

        case ID_WINDOW_NEXT:
            OnCmdWindowNext();
            break;

        case ID_WINDOW_PREV:
            OnCmdWindowPrev();
            break;

        case ID_WINDOW_CASCADE:
            OnCmdWindowCascade();
            break;

        case ID_WINDOW_TILE_HORZ:
            OnCmdWindowTileHorz();
            break;
        
        case ID_WINDOW_TILE_VERT:
            OnCmdWindowTileVert();
            break;
        
        case ID_WINDOW_CLOSE:
            OnCmdWindowClose();
            break;

        case ID_WINDOW_CLOSE_ALL:
            OnCmdWindowCloseAll();
            break;

        case ID_ADD_WAVEBANK:
            OnCmdAddWaveBank();
            break;

        case ID_BUILD:
            OnCmdBuild();
            break;

        case ID_CONNECT:
            OnCmdConnect();
            break;

        case ID_DISCONNECT:
            OnCmdDisconnect();
            break;

        case ID_ADD_WAVEBANK_ENTRY:
            return ForwardCommand(FORWARD_ACTIVE_WORKSPACE_CHILD, nCommandId, nControlId, hWndControl, plResult);

        default:
            return ForwardCommand(FORWARD_FOCUS, nCommandId, nControlId, hWndControl, plResult);
    }

    return FALSE;
}


/****************************************************************************
 *
 *  ForwardCommand
 *
 *  Description:
 *      Forwards a command on to the window with the keyboard focus.
 *
 *  Arguments:
 *      UINT [in]: forward type.
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
#define DPF_FNAME "CMainFrame::ForwardCommand"

BOOL
CMainFrame::ForwardCommand
(
    UINT                    nForward,
    UINT                    nCommandId, 
    UINT                    nControlId, 
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    CWindow *               pWindow;
    CMDIChildWindow *       pMDIChild;
    HWND                    hWnd;

    if(FORWARD_FOCUS == nForward)
    {
        //
        // We really want to send this to the first window in the chain that has
        // a registered handler we recognize (i.e. derived from CWindow)
        //
        // WARNING: if we forward this to a window who turns around and forwards
        // it back to us, we're screwed.
        //

        hWnd = GetFocus();
        
        while(TRUE)
        {
            if(pWindow = GetWindowContext(hWnd))
            {
                break;
            }

            hWnd = GetParent(hWnd);
        }

        if(this == pWindow)
        {
            return FALSE;
        }
    }
    else if(FORWARD_ACTIVE_WORKSPACE_CHILD == nForward)
    {
        if(m_Workspace.GetActiveChild(&pMDIChild))
        {
            pWindow = pMDIChild;
        }
    }
    else
    {
        ASSERTMSG("Invalid forward type");
        return FALSE;
    }

    return pWindow->OnCommand(nCommandId, nControlId, hWndControl, plResult);
}


/****************************************************************************
 *
 *  OnCmdNewProject
 *
 *  Description:
 *      Creates a new project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdNewProject"

void
CMainFrame::OnCmdNewProject
(
    void
)
{
    g_pApplication->m_Project.New();
}


/****************************************************************************
 *
 *  OnCmdOpenProject
 *
 *  Description:
 *      Opens a new project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdOpenProject"

void
CMainFrame::OnCmdOpenProject
(
    void
)
{
    g_pApplication->m_Project.Open();
}


/****************************************************************************
 *
 *  OnCmdReloadProject
 *
 *  Description:
 *      Reopens the current project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdReloadProject"

void
CMainFrame::OnCmdReloadProject
(
    void
)
{
    g_pApplication->m_Project.Reopen();
}


/****************************************************************************
 *
 *  OnCmdSaveProject
 *
 *  Description:
 *      Saves the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdSaveProject"

void
CMainFrame::OnCmdSaveProject
(
    void
)
{
    g_pApplication->m_Project.Save();
}


/****************************************************************************
 *
 *  OnCmdSaveProjectAs
 *
 *  Description:
 *      Saves the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdSaveProjectAs"

void
CMainFrame::OnCmdSaveProjectAs
(
    void
)
{
    g_pApplication->m_Project.SaveAs();
}


/****************************************************************************
 *
 *  OnCmdAddWaveBank
 *
 *  Description:
 *      Adds a wave bank to the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdAddWaveBank"

void
CMainFrame::OnCmdAddWaveBank
(
    void
)
{
    g_pApplication->m_Project.AddWaveBank();
}


/****************************************************************************
 *
 *  OnCmdBuild
 *
 *  Description:
 *      Builds the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdBuild"

void
CMainFrame::OnCmdBuild
(
    void
)
{
    g_pApplication->m_Project.Build();
}


/****************************************************************************
 *
 *  OnCmdConnect
 *
 *  Description:
 *      Connects to a remote instance of Audio Console.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdConnect"

void
CMainFrame::OnCmdConnect
(
    void
)
{
    g_pApplication->m_RemoteAudition.Connect();
}


/****************************************************************************
 *
 *  OnCmdDisconnect
 *
 *  Description:
 *      Disconnects from a remote instance of Audio Console.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdDisconnect"

void
CMainFrame::OnCmdDisconnect
(
    void
)
{
    g_pApplication->m_RemoteAudition.Disconnect();
}


/****************************************************************************
 *
 *  OnCmdExit
 *
 *  Description:
 *      Handles the Exit command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdExit"

void
CMainFrame::OnCmdExit
(
    void
)
{
    PostMessage(WM_DESTROY);
}


/****************************************************************************
 *
 *  OnCmdAbout
 *
 *  Description:
 *      Handles the About command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdAbout"

void
CMainFrame::OnCmdAbout
(
    void
)
{
    TCHAR                   szVersion[0x100];
    LPCTSTR                 pszString;
    HICON                   hIcon;
    HWND                    hWndFocus;

    GetXDKVersion(szVersion);

    pszString = FormatStringResourceStatic(IDS_XDKVERSION, szVersion);
    hIcon = LoadResourceIconLarge(IDI_XACT);

    hWndFocus = GetFocus();
    
    ShellAbout(m_hWnd, g_szAppTitle, pszString, hIcon);

    SetFocus(hWndFocus);
}


/****************************************************************************
 *
 *  OnCmdWindowPrev
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowPrev"

void
CMainFrame::OnCmdWindowPrev
(
    void
)
{
    m_Workspace.SendMessage(WM_MDINEXT, NULL, TRUE);
}


/****************************************************************************
 *
 *  OnCmdWindowNext
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowNext"

void
CMainFrame::OnCmdWindowNext
(
    void
)
{
    m_Workspace.SendMessage(WM_MDINEXT, NULL, FALSE);
}


/****************************************************************************
 *
 *  OnCmdWindowCascade
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowCascade"

void
CMainFrame::OnCmdWindowCascade
(
    void
)
{
    m_Workspace.SendMessage(WM_MDICASCADE);
}


/****************************************************************************
 *
 *  OnCmdWindowTileHorz
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowTileHorz"

void
CMainFrame::OnCmdWindowTileHorz
(
    void
)
{
    m_Workspace.SendMessage(WM_MDITILE, MDITILE_HORIZONTAL);
}


/****************************************************************************
 *
 *  OnCmdWindowTileVert
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowTileVert"

void
CMainFrame::OnCmdWindowTileVert
(
    void
)
{
    m_Workspace.SendMessage(WM_MDITILE, MDITILE_VERTICAL);
}


/****************************************************************************
 *
 *  OnCmdWindowClose
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowClose"

void
CMainFrame::OnCmdWindowClose
(
    void
)
{
    HWND                    hWnd;

    hWnd = (HWND)m_Workspace.SendMessage(WM_MDIGETACTIVE);
    
    m_Workspace.SendMessage(WM_MDIDESTROY, (WPARAM)hWnd);
}


/****************************************************************************
 *
 *  OnCmdWindowCloseAll
 *
 *  Description:
 *      Handles window commands.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::OnCmdWindowCloseAll"

void
CMainFrame::OnCmdWindowCloseAll
(
    void
)
{
    HWND                    hWnd;

    while(TRUE)
    {
        if(!(hWnd = (HWND)m_Workspace.SendMessage(WM_MDIGETACTIVE)))
        {
            break;
        }
    
        m_Workspace.SendMessage(WM_MDIDESTROY, (WPARAM)hWnd);
    }
}


/****************************************************************************
 *
 *  LayoutControls
 *
 *  Description:
 *      Moves the controls around.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::LayoutControls"

void
CMainFrame::LayoutControls
(
    BOOL                    fRepaint
)
{
    RECT                    rcClient;

    GetClientRect(&rcClient);

    LayoutControls(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, fRepaint);
}


/****************************************************************************
 *
 *  LayoutControls
 *
 *  Description:
 *      Moves the controls around.
 *
 *  Arguments:
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::LayoutControls"

void
CMainFrame::LayoutControls
(
    UINT                    nWidth,
    UINT                    nHeight,
    BOOL                    fRepaint
)
{
    RECT                    rcClient        = { 0, 0, nWidth, nHeight };
    RECT                    rcToolBar;
    RECT                    rcStatusBar;
    RECT                    rcProjectTree;
    RECT                    rcSplitter;
    RECT                    rcWorkspace;
    
    //
    // The basic window layout is as follows:
    //
    //   +-----------------------------+
    //   | Title bar                   |
    //   +-----------------------------+
    //   | Menu bar                    |
    //   +-----------------------------+
    //   | Tool bar                    |
    //   +-----------------------------+
    //   | T   |S|  MDI Workspace      |
    //   | r   |p|                     |
    //   | e   |l|                     |
    //   | e   |i|                     |
    //   |     |t|                     |
    //   | v   |t|                     |
    //   | i   |e|                     |
    //   | e   |r|                     |
    //   | w   | |                     |
    //   |     | |                     |
    //   +-----------------------------+
    //   | Status bar                  |
    //   +-----------------------------+
    //

    //
    // Trim the width and height down to account for the toolbar and status
    // bar sizes
    //

    m_ToolBar.SendMessage(WM_SIZE, SIZE_RESTORED, MAKELONG(nWidth, nHeight));
    m_StatusBar.SendMessage(WM_SIZE, SIZE_RESTORED, MAKELONG(nWidth, nHeight));
    
    m_ToolBar.GetWindowRect(&rcToolBar);
    m_StatusBar.GetWindowRect(&rcStatusBar);

    rcClient.top += rcToolBar.bottom - rcToolBar.top;
    rcClient.bottom -= rcStatusBar.bottom - rcStatusBar.top;

    //
    // The splitter defines the size of the other two windows
    // 

    if(m_Splitter.m_nPosition < 0)
    {
        m_Splitter.m_nPosition = 0;
    }
    else if(m_Splitter.m_nPosition + m_Splitter.m_nWidth > nWidth)
    {
        m_Splitter.m_nPosition = nWidth - m_Splitter.m_nWidth;
    }
    
    rcSplitter.left = rcClient.left + m_Splitter.m_nPosition;
    rcSplitter.top = rcClient.top;
    rcSplitter.right = rcSplitter.left + m_Splitter.m_nWidth;
    rcSplitter.bottom = rcClient.bottom;
    
    // 
    // Resize the tree-view up to the splitter
    //

    rcProjectTree.left = rcClient.left;
    rcProjectTree.top = rcClient.top;
    rcProjectTree.right = rcSplitter.left;
    rcProjectTree.bottom = rcClient.bottom;

    //
    // Resize the workspace to take the remaining area
    //

    rcWorkspace.left = rcSplitter.right;
    rcWorkspace.top = rcClient.top;
    rcWorkspace.right = rcClient.right;
    rcWorkspace.bottom = rcClient.bottom;

    m_ProjectTree.MoveWindow(&rcProjectTree, fRepaint);
    m_Splitter.MoveWindow(&rcSplitter, fRepaint);
    m_Workspace.MoveWindow(&rcWorkspace, fRepaint);
}


/****************************************************************************
 *
 *  EnableCommand
 *
 *  Description:
 *      Enables or disables a command.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      BOOL [in]: TRUE to enable.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::EnableCommand"

void
CMainFrame::EnableCommand
(
    UINT                    nCommandId,
    BOOL                    fEnable
)
{
    const DWORD             dwMenuFlags = MF_BYCOMMAND | (fEnable ? MF_ENABLED : MF_GRAYED);
    UINT                    i;
    
    //
    // Update the toolbar
    //

    m_ToolBar.EnableButton(nCommandId, fEnable);

    //
    // Update the main menu
    //

    EnableMenuItem(m_hMenu, nCommandId, dwMenuFlags);

    //
    // Update the secondary menus
    //

    for(i = 0; i < NUMELMS(m_ahMenus); i++)
    {
        EnableMenuItem(m_ahMenus[i], nCommandId, dwMenuFlags);
    }

    //
    // Update check state
    //

    if(!fEnable)
    {
        CheckCommand(nCommandId, FALSE);
    }
}


/****************************************************************************
 *
 *  CheckCommand
 *
 *  Description:
 *      Checks or unchecks a command.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      BOOL [in]: TRUE to check.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::CheckCommand"

void
CMainFrame::CheckCommand
(
    UINT                    nCommandId,
    BOOL                    fCheck
)
{
    const DWORD             dwMenuFlags = MF_BYCOMMAND | (fCheck ? MF_CHECKED : MF_UNCHECKED);
    UINT                    i;
    
    //
    // Update the toolbar
    //

    m_ToolBar.SetButtonState(nCommandId, TBSTATE_CHECKED, fCheck ? TBSTATE_CHECKED : 0);

    //
    // Update the main menu
    //

    CheckMenuItem(m_hMenu, nCommandId, dwMenuFlags);

    //
    // Update the secondary menus
    //

    for(i = 0; i < NUMELMS(m_ahMenus); i++)
    {
        CheckMenuItem(m_ahMenus[i], nCommandId, dwMenuFlags);
    }
}


/****************************************************************************
 *
 *  SetDefaultCommandState
 *
 *  Description:
 *      Sets the default state for all commands.  This method is really only
 *      meant to be called during init, so it only disables commands.  None
 *      are enabled, and the check state is never changed.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainFrame::SetDefaultCommandState"

void
CMainFrame::SetDefaultCommandState
(
    void
)
{
    static const UINT       anDisabled[] =
    {
        ID_RELOAD_PROJECT,
        ID_ADD_WAVEBANK_ENTRY,
        ID_WAVEBANK_ENTRY_ADPCM,
        ID_WAVEBANK_ENTRY_8BIT,
        ID_PLAY,
        ID_STOP,
        ID_RECENT_FILE_0,
        ID_DISCONNECT,
    };

    UINT                    i;

    for(i = 0; i < NUMELMS(anDisabled); i++)
    {
        EnableCommand(anDisabled[i], FALSE);
    }
}


/****************************************************************************
 *
 *  CMainStatusBar
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
#define DPF_FNAME "CMainStatusBar::CMainStatusBar"

CMainStatusBar::CMainStatusBar
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainStatusBar
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
#define DPF_FNAME "CMainStatusBar::~CMainStatusBar"

CMainStatusBar::~CMainStatusBar
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
#define DPF_FNAME "CMainStatusBar::Create"

BOOL
CMainStatusBar::Create
(
    void
)
{
    static const DWORD      dwExStyle   = 0;
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP;
    
    //
    // Create the window
    //

    return CWindow::Create(&g_pApplication->m_MainFrame, STATUSCLASSNAME, NULL, dwExStyle, dwStyle, 0, 0, 0, 0, CMainFrame::IDC_MAIN_STATUSBAR);
}


/****************************************************************************
 *
 *  CMainToolBar
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
#define DPF_FNAME "CMainToolBar::CMainToolBar"

CMainToolBar::CMainToolBar
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainToolBar
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
#define DPF_FNAME "CMainToolBar::~CMainToolBar"

CMainToolBar::~CMainToolBar
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
#define DPF_FNAME "CMainToolBar::Create"

BOOL
CMainToolBar::Create
(
    void
)
{
    static const DWORD      dwExStyle       = 0;
    static const DWORD      dwStyle         = WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
    UINT                    nBitmapIndex;
    
    //
    // Create the window
    //
    
    if(!CToolBar::Create(&g_pApplication->m_MainFrame, dwExStyle, dwStyle, CMainFrame::IDC_MAIN_TOOLBAR))
    {
        return FALSE;
    }

    //
    // Add the main toolbar bitmap
    //

    nBitmapIndex = LoadImages(IDB_MAIN_TOOLBAR, BTN_COUNT);

    //
    // Add buttons
    //

    AddButton(IML_STD, STD_FILENEW, ID_NEW_PROJECT);

    AddSeparator();

    AddButton(IML_STD, STD_FILEOPEN, ID_OPEN_PROJECT);
    AddButton(IML_STD, STD_FILESAVE, ID_SAVE_PROJECT);

    AddSeparator();

    AddButton(nBitmapIndex, BTN_WAVEBANK, ID_ADD_WAVEBANK);
    AddButton(nBitmapIndex, BTN_WAVEBANK_ENTRY, ID_ADD_WAVEBANK_ENTRY);

    AddSeparator();

    AddButton(nBitmapIndex, BTN_ADPCM, ID_WAVEBANK_ENTRY_ADPCM);
    AddButton(nBitmapIndex, BTN_8BIT, ID_WAVEBANK_ENTRY_8BIT);

    AddSeparator();

    AddButton(nBitmapIndex, BTN_CONNECT, ID_CONNECT);
    AddButton(nBitmapIndex, BTN_DISCONNECT, ID_DISCONNECT);

    AddSeparator();

    AddButton(nBitmapIndex, BTN_PLAY, ID_PLAY);
    AddButton(nBitmapIndex, BTN_STOP, ID_STOP);

    AddSeparator();

    AddButton(nBitmapIndex, BTN_BUILD, ID_BUILD);

    return TRUE;
}


/****************************************************************************
 *
 *  CMainProjectTree
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
#define DPF_FNAME "CMainProjectTree::CMainProjectTree"

CMainProjectTree::CMainProjectTree
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainProjectTree
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
#define DPF_FNAME "CMainProjectTree::~CMainProjectTree"

CMainProjectTree::~CMainProjectTree
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
#define DPF_FNAME "CMainProjectTree::Create"

BOOL
CMainProjectTree::Create
(
    void
)
{
    static const DWORD      dwExStyle   = WS_EX_CLIENTEDGE;
    static const DWORD      dwStyle     = WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS;

    //
    // Create the window
    //

    if(!CTreeView::Create(&g_pApplication->m_MainFrame, dwExStyle, dwStyle, 0, 0, 0, 0, CMainFrame::IDC_MAIN_PROJECTTREE))
    {
        return FALSE;
    }

    //
    // Create category items
    //

    m_WaveBundlerCategory.Create(this, IDS_CATEGORY_WAVEBUNDLER, IDI_CATEGORY_WAVEBUNDLER);

    //
    // Register standard types
    //

    RegisterType(m_pszWaveBankType, LoadResourceIconSmall(IDI_WAVEBANK));

    return TRUE;
}


/****************************************************************************
 *
 *  OnCommand
 *
 *  Description:
 *      Handles WM_COMMAND messages.
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
#define DPF_FNAME "CMainProjectTree::OnCommand"

BOOL
CMainProjectTree::OnCommand
(
    UINT                    nCommandId,
    UINT                    nControlId,
    HWND                    hWndControl,
    LRESULT *               plResult
)
{
    CMainProjectTreeItem *  pItem;
    
    if(CTreeView::OnCommand(nCommandId, nControlId, hWndControl, plResult))
    {
        return TRUE;
    }

    if(!(pItem = (CMainProjectTreeItem *)GetSelection()))
    {
        return FALSE;
    }

    return pItem->OnCommand(nCommandId, nControlId, hWndControl, plResult);
}


/****************************************************************************
 *
 *  CMainProjectTreeItem
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
#define DPF_FNAME "CMainProjectTreeItem::CMainProjectTreeItem"

CMainProjectTreeItem::CMainProjectTreeItem
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainProjectTreeItem
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
#define DPF_FNAME "CMainProjectTreeItem::~CMainProjectTreeItem"

CMainProjectTreeItem::~CMainProjectTreeItem
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
 *      Creates the item.
 *
 *  Arguments:
 *      CMainProjectTree * [in]: tree control.
 *      CTreeViewItem * [in]: parent item.
 *      LPCTSTR [in]: item name.
 *      LPCTSTR [in]: item type.
 *      LPVOID [in]: item context.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainProjectTreeItem::Create"

BOOL
CMainProjectTreeItem::Create
(
    CMainProjectTree *      pTree,
    CMainProjectTreeItem *  pParentItem,
    LPCTSTR                 pszText,
    LPCTSTR                 pszType,
    LPVOID                  pvContext
)
{
    return CTreeViewItem::Create(pTree, pParentItem, pszText, pszType, pvContext);
}


/****************************************************************************
 *
 *  CMainProjectTreeCategory
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
#define DPF_FNAME "CMainProjectTreeCategory::CMainProjectTreeCategory"

CMainProjectTreeCategory::CMainProjectTreeCategory
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainProjectTreeCategory
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
#define DPF_FNAME "CMainProjectTreeCategory::~CMainProjectTreeCategory"

CMainProjectTreeCategory::~CMainProjectTreeCategory
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
 *      Creates the item.
 *
 *  Arguments:
 *      CMainProjectTree * [in]: tree.
 *      UINT [in]: category name string identifier.
 *      UINT [in]: icon identifier.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainProjectTreeCategory::Create"

BOOL
CMainProjectTreeCategory::Create
(
    CMainProjectTree *      pTree,
    UINT                    nStringId,
    UINT                    nIconId
)
{
    TCHAR                   szName[0x100];
    HICON                   hIcon;

    FormatStringResource(szName, NUMELMS(szName), nStringId);

    hIcon = LoadResourceIconSmall(nIconId);

    pTree->RegisterType(szName, hIcon);
    
    return CMainProjectTreeItem::Create(pTree, NULL, szName, szName);
}


/****************************************************************************
 *
 *  CMainSplitter
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
#define DPF_FNAME "CMainSplitter::CMainSplitter"

CMainSplitter::CMainSplitter
(
    void
)
{
    m_nPosition = 250;
    m_fDragging = FALSE;
}


/****************************************************************************
 *
 *  ~CMainSplitter
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
#define DPF_FNAME "CMainSplitter::~CMainSplitter"

CMainSplitter::~CMainSplitter
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
#define DPF_FNAME "CMainSplitter::Create"

BOOL
CMainSplitter::Create
(
    void
)
{
    static const DWORD      dwExStyle   = 0;
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE;
    WNDCLASSEX              wc          = { 0 };

    //
    // Load the splitter position from the registry
    //
    
    g_pApplication->m_Registry.GetNumericValue(REGKEY_SPLITTER_POSITION, &m_nPosition);

    //
    // Register the window class
    //

    wc.cbSize = sizeof(wc);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
    wc.hInstance = g_hInstance;
    wc.lpszClassName = m_pszClassName;

    RegisterClassEx(&wc);

    //
    // Create the window
    //

    return CWindow::Create(&g_pApplication->m_MainFrame, m_pszClassName, NULL, dwExStyle, dwStyle, 0, 0, 0, 0, CMainFrame::IDC_MAIN_SPLITTER);
}


/****************************************************************************
 *
 *  HandleMessage
 *
 *  Description:
 *      Dispatches a window message to the appropriate handler.
 *
 *  Arguments:
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

BOOL 
CMainSplitter::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            fHandled = OnMouseButtonDown(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam), plResult);
            break;
        
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            fHandled = OnMouseButtonUp(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam), plResult);
            break;
        
        case WM_MOUSEMOVE:
            fHandled = OnMouseMove(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam), plResult);
            break;
        
        default:
            fHandled = CWindow::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnMouseButtonDown
 *
 *  Description:
 *      Handles mouse button events.
 *
 *  Arguments:
 *      DWORD [in]: pressed keys.
 *      UINT [in]: x-coordinate of the mouse.
 *      UINT [in]: y-coordinate of the mouse.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

BOOL 
CMainSplitter::OnMouseButtonDown
(
    DWORD                   dwKeys,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    //
    // Capture the mouse so the user can drag the splitter around
    //

    SetCapture(m_hWnd);

    m_fDragging = TRUE;

    return FALSE;
}


/****************************************************************************
 *
 *  OnMouseButtonUp
 *
 *  Description:
 *      Handles mouse button events.
 *
 *  Arguments:
 *      DWORD [in]: pressed keys.
 *      UINT [in]: x-coordinate of the mouse.
 *      UINT [in]: y-coordinate of the mouse.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

BOOL 
CMainSplitter::OnMouseButtonUp
(
    DWORD                   dwKeys,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    //
    // Release the mouse
    //

    ReleaseCapture();

    m_fDragging = FALSE;

    return FALSE;
}


/****************************************************************************
 *
 *  OnMouseMove
 *
 *  Description:
 *      Handles mouse events.
 *
 *  Arguments:
 *      DWORD [in]: pressed keys.
 *      UINT [in]: x-coordinate of the mouse.
 *      UINT [in]: y-coordinate of the mouse.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

BOOL 
CMainSplitter::OnMouseMove
(
    DWORD                   dwKeys,
    int                     x,
    int                     y,
    LRESULT *               plResult
)
{
    if(!m_fDragging)
    {
        return FALSE;
    }

    //
    // Update our position
    //

    m_nPosition += x;
    
    //
    // Tell the parent to shuffle controls around
    //

    ((CMainFrame *)m_pParent)->LayoutControls();
    
    return FALSE;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles WM_DESTROY messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMainSplitter::OnDestroy"

BOOL
CMainSplitter::OnDestroy
(
    LRESULT *               plResult
)
{
    WINDOWPLACEMENT         wp;
    
    if(CWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    g_pApplication->m_Registry.SetNumericValue(REGKEY_SPLITTER_POSITION, m_nPosition);

    return FALSE;
}


/****************************************************************************
 *
 *  CMainWorkspace
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
#define DPF_FNAME "CMainWorkspace::CMainWorkspace"

CMainWorkspace::CMainWorkspace
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CMainWorkspace
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
#define DPF_FNAME "CMainWorkspace::~CMainWorkspace"

CMainWorkspace::~CMainWorkspace
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
#define DPF_FNAME "CMainWorkspace::Create"

BOOL
CMainWorkspace::Create
(
    void
)
{
    static const DWORD      dwExStyle   = WS_EX_CLIENTEDGE;
    static const DWORD      dwStyle     = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL;
    CLIENTCREATESTRUCT      ccs;

    //
    // Create the window
    //

    ccs.hWindowMenu = GetSubMenu(g_pApplication->m_MainFrame.m_hMenu, g_pApplication->m_MainFrame.MENUPOS_WINDOW);
    ccs.idFirstChild = g_pApplication->m_MainFrame.IDC_MAIN_FIRSTMDICHILD;

    return CMDIClientWindow::Create(&g_pApplication->m_MainFrame, dwExStyle, dwStyle, 0, 0, 0, 0, g_pApplication->m_MainFrame.IDC_MAIN_WORKSPACE, &ccs);
}


/****************************************************************************
 *
 *  CWorkspaceChild
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      UINT [in]: window type.
 *      UINT [in]: window menu resource identifier.
 *      UINT [in]: window accelerator table resource identifier.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWorkspaceChild::CWorkspaceChild"

CWorkspaceChild::CWorkspaceChild
(
    UINT                    nWorkspaceType
)
:   m_nWorkspaceType(nWorkspaceType)
{
}


/****************************************************************************
 *
 *  ~CWorkspaceChild
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
#define DPF_FNAME "CWorkspaceChild::~CWorkspaceChild"

CWorkspaceChild::~CWorkspaceChild
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
 *      Creates the window.
 *
 *  Arguments:
 *      LPCTSTR [in]: window title.
 *      UINT [in]: window icon resource identifier.
 *      LPBOOL [out]: TRUE if the window was opened; FALSE if it was just
 *                    activated.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWorkspaceChild::Create"

BOOL
CWorkspaceChild::Create
(
    LPCTSTR                 pszWindowTitle,
    UINT                    nIconId,
    LPBOOL                  pfOpened
)
{
    static const DWORD      dwExStyle   = 0;
    DWORD                   dwStyle     = WS_CHILD | WS_VISIBLE;
    WNDCLASSEX              wc          = { 0 };
    WINDOWPLACEMENT         wp;

    //
    // If the window already exists, just activate it
    //

    if(m_hWnd)
    {
        if(pfOpened)
        {
            *pfOpened = FALSE;
        }
        
        m_pParent->SendMessage(WM_MDIACTIVATE, (WPARAM)m_hWnd);

        return TRUE;
    }

    //
    // Register the window class
    //

    wc.cbSize = sizeof(wc);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = g_hInstance;
    wc.lpszClassName = m_pszClassName;

    RegisterClassEx(&wc);

    //
    // Load window placement data
    //
    
    if(g_pApplication->m_Registry.GetBinaryValue(REGKEY_WORKSPACE_CHILD_PLACEMENT, &wp, sizeof(wp)))
    {
        if(SW_MAXIMIZE == wp.showCmd)
        {
            dwStyle |= WS_MAXIMIZE;
        }
    }

    //
    // Create the window
    //

    if(!CMDIChildWindow::Create(&g_pApplication->m_MainFrame.m_Workspace, m_pszClassName, pszWindowTitle, dwExStyle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, (LPVOID)nIconId))
    {
        return FALSE;
    }

    //
    // Return that the window was opened
    //

    if(pfOpened)
    {
        *pfOpened = TRUE;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWorkspaceChild::OnCreate"

BOOL
CWorkspaceChild::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    const int               nIconId = (int)pvContext;

    if(CMDIChildWindow::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Set the window's icon
    //

    SendMessage(WM_SETICON, ICON_BIG, (LPARAM)LoadResourceIconLarge(nIconId));
    SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)LoadResourceIconSmall(nIconId));

    return FALSE;
}


/****************************************************************************
 *
 *  OnDestroy
 *
 *  Description:
 *      Handles WM_DESTROY messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWorkspaceChild::OnDestroy"

BOOL
CWorkspaceChild::OnDestroy
(
    LRESULT *               plResult
)
{
    WINDOWPLACEMENT         wp;
    
    if(CMDIChildWindow::OnDestroy(plResult))
    {
        return TRUE;
    }

    //
    // Save the window placement data
    //

    wp.length = sizeof(wp);
    
    GetWindowPlacement(m_hWnd, &wp);

    g_pApplication->m_Registry.SetBinaryValue(REGKEY_WORKSPACE_CHILD_PLACEMENT, &wp, sizeof(wp));

    return FALSE;
}


