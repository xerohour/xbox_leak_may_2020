// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cwindow.cpp
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_pwindowTemp        -- Temporary pointer used to circumvent CreateWindow annoyances (see below).
CWindow *g_pwindowTemp = NULL;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL VARIABLES +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam) ;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::InitInstance
// Purpose:   Register our class and then create and show our window.
// Arguments: hinst         -- The handle to this instance of the application
//            nCmdShow      -- Default visibility of the application (maximized, minimized, etc)
// Return:    true if successful.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow::InitInstance(CWindow *pwindowParent)
{
    WNDCLASSEX wcex;
    // Register the class for the app's window.
    wcex.cbSize         = sizeof(WNDCLASSEX); 
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)::WndProc;
    wcex.hInstance      = m_hinst;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = NULL;
    wcex.lpszClassName  = m_szName;
    wcex.lpszMenuName   = m_szMenuName;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hIcon          = LoadIcon(m_hinst, "IDC_MAIN");
    wcex.hIconSm        = 0;
    if (!RegisterClassEx(&wcex))
        return NULL;

    // Create the app's window.
    m_hwnd = CreateWindow(m_szName, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                          CW_USEDEFAULT, 0,
                          pwindowParent ? pwindowParent->m_hwnd : NULL, NULL,
                          m_hinst, NULL);
    if (m_hwnd == NULL)
        return false;

    return true;
}

bool CWindow::GetRegValue(HKEY hkey, char *szValue, long *pn)
{
    char szFull[1000];
    DWORD dwSize = 4;

    // prepend the window's name to make it unique
    sprintf(szFull, "%s-%s",m_szName, szValue);
    return RegQueryValueEx(hkey, szFull, NULL, NULL, (BYTE*)pn, &dwSize) == ERROR_SUCCESS;
}

bool CWindow::SetRegValue(HKEY hkey, char *szValue, long n)
{
    char szFull[1000];
    BYTE *pby = (BYTE*)&n;

    // prepend the window's name to make it unique
    sprintf(szFull, "%s-%s",m_szName, szValue);

    return RegSetValueEx(hkey, szFull, 0, REG_DWORD, pby, 4) == ERROR_SUCCESS;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::SetLastKnownPos
// Purpose:   Read the last known position out of the registry and set it
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow::SetLastKnownPos()
{
    HKEY hkey;
    RECT rc;
    int nScreenWidth, nScreenHeight;

    // UNDONE: Verify that I should be creating my own key
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbLayout"), 0,
                     KEY_READ, &hkey) != ERROR_SUCCESS)
        return;
    
    if (!GetRegValue(hkey, "LastKnownX1", &rc.left))   return;
    if (!GetRegValue(hkey, "LastKnownY1", &rc.top))    return;
    if (!GetRegValue(hkey, "LastKnownX2", &rc.right))  return;
    if (!GetRegValue(hkey, "LastKnownY2", &rc.bottom)) return;
    RegCloseKey(hkey);

    // Verify the values place the window in a visible location
    HDC hdc = GetDC(m_hwnd);
    nScreenWidth  = GetDeviceCaps(hdc, HORZRES);
    nScreenHeight = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(m_hwnd, hdc);

    if (rc.left > nScreenWidth - 4 || rc.right < 4 || rc.bottom < 4 || rc.top > nScreenHeight + 4)
        return;

    MoveWindow(m_hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::StorePosition
// Purpose:   Stores the current position of the window in the registry
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow::StorePosition()
{
    HKEY hkey;
    RECT rc;

    GetWindowRect(m_hwnd, &rc);

    if (rc.left == 0xcccccccc)
        DebugBreak();

    // Open/Create the key
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbLayout"), 0,
                 NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
        return;

    if (!SetRegValue(hkey, "LastKnownX1", rc.left))   return;
    if (!SetRegValue(hkey, "LastKnownY1", rc.top))    return;
    if (!SetRegValue(hkey, "LastKnownX2", rc.right))  return;
    if (!SetRegValue(hkey, "LastKnownY2", rc.bottom)) return;
    RegCloseKey(hkey);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::CWindow
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CWindow::CWindow(char *szName, DWORD dwWindowCreationFlags, HINSTANCE hinst)
{
    // Track this instance of the application
    m_hinst = hinst;

    // Store our name
    strcpy(m_szName, szName);
    m_dwWindowCreationFlags = dwWindowCreationFlags;
}

bool CWindow::Init(CWindow *pwindowParent)
{
    // No other window can be in the process of being created when this window is created, due to
    // the lack of reentrancy imposed by g_pwindowTemp.
    assert(g_pwindowTemp == NULL);

    // Set the global CWindow pointer to point at us so that ::WndProc can determine which CWindow
    // the current message belongs to.  This use of a global variable is necessary due to the
    // messages that the OS sends to ::WndProc before we can stuff a pointer to ourselves into the
    // window (see above).
    g_pwindowTemp = this;

    // Initialize our class and create our window
    if (!InitInstance(pwindowParent))
    {
        // A thoroughly unrecoverable (and unexpected) error occurred.  The user's machine is fubar
        MessageBox(NULL, "This application failed to initialize.  Please restart your computer and "\
                   "try again.", "Failed to start", MB_ICONEXCLAMATION | MB_APPLMODAL | MB_OK);
        return false;
    }

    // Tell the newly created Win32 window that it "belongs" to us.  This is necessary so that 
    // the static WndProc that Windows calls (::WndProc above) can determine that it came from us,
    // and call the WndProc of this instance of CWindow
    SetWindowLong(m_hwnd, GWL_USERDATA, (long)this);

    // Now that our pointer has been stuffed into the window, we can stop using the global var.
    g_pwindowTemp = NULL;

    // Initialize keyboard accelerators
    m_haccel = LoadAccelerators(m_hinst, (LPCTSTR)IDC_ACCEL);

    // Mark that we successfully initialized
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::~CWindow
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CWindow::~CWindow()
{
    // At this point, m_hwnd has *already* been destroyed, so don't do anything that uses it!
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::Run
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CWindow::Run()
{
    MSG msg;

    // Just drop straight into a Win32 message loop.
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(m_hwnd, m_haccel, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // When here, we have exited the application.
    return msg.wParam;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WndProc
// Purpose:   This is the static callback for the CWindow class.  It receives all messages from the
//            OS (ie user input, callbacks, etc).  We simply take the message, determine which
//            window it actually belongs to, and pass it on to the appropriate CWindow object.
// Arguments: hwnd              -- The handle to the window that the message is intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    CWindow *pwindow;

    // When a CWindow calls CreateWindow to create the Win32 windo, the OS sends a number of
    // messages to WndProc before the CreateWindow returns (WM_CREATE, WM_SIZE, etc).
    // Because we haven't returned from the CreateWindow call, we can't set which CWindow hwnd is
    // actually referring to.  Therefore, we need to use a temporary global variable to point at
    // the correct CWindow.  If that variable is non-NULL, then we know that we haven't returned from
    // the CreateWindow call, and that we should use it instead.  If the variable is NULL, then the
    // CWindow has had the opportunity to cram a pointer to itself into the window. This would
    // create issues if two windows are created at the exact same time, but we never do that here.
    if (g_pwindowTemp != NULL)
    {
        pwindow = g_pwindowTemp;

        // We haven't returned from CreateWindow yet, so we haven't set our hwnd.  Get around this
        // by placing the passed-in hwnd into the window's hwnd.
        pwindow->m_hwnd = hwnd;
    }
    else
    {
        // Determine which CWindow this message should be sent to.  We stored a pointer to the
        // window in the hwnd's 'user' area.  Extract it now.
        pwindow = (CWindow*)GetWindowLong(hwnd, GWL_USERDATA);
        if (!pwindow)
            return false;
    }    

    // Pass the message on to the appropriate CWindow and let it handle it.
    return pwindow->WndProc(uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::WndProc
// Purpose:   This is the instance-specific window message handler.  There are some message that
//            we handle here (in the base class), while others are ignored (and handled by the OS).
// Arguments: uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CWindow::WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    switch (uMsg) 
    {
    case WM_CREATE:
        return 0;

  //  case WM_ERASEBKGND:
//        // Don't erase the background
    //    return 1;

    case WM_SIZE:
        OnSize();
        return 1;
        
    case WM_COMMAND:
        if (ButtonPressed(LOWORD(wparam)))
            return 0;
        break;

    case WM_NOTIFY:
        return OnNotify(wparam, lparam);
    }

    // Let derived class handle all unhandled messages
    return DerivedWndProc(uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::GetCaption
// Purpose:   Generate and return a pointer to the current caption for the xbLayout window
// Arguments: None
// Return:    Pointer to the current caption.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow::GetCaption(char *sz)
{
    GetWindowText(m_hwnd, sz, 1024);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::SetCaption
// Purpose:   
// Arguments: None
// Return:    Pointer to the current caption.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow::SetCaption(char *sz)
{
    SetWindowText(m_hwnd, sz);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow::GetChildControlRect
// Purpose:   Gets the rectangle for the specified control
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow::GetChildControlRect(HWND hwndControl, RECT *prc)
{
    GetClientRect(hwndControl, prc);
    MapWindowPoints(hwndControl, m_hwnd, (LPPOINT)prc, 2);
}
