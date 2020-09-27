#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock.h>
#include <d3dx8.h>
#include "transprt.h"
#include "util.h"
#include "typetran.h"
#include "server.h"
#include "resource.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define WM_TASKBAR          (WM_USER + 123)

#define ICON_DISCONNECTED   0
#define ICON_CONNECTED      1

//******************************************************************************
// Globals
//******************************************************************************

TCHAR                   g_szAppName[] = TEXT("D3DLocus");
HINSTANCE               g_hInstance = NULL;
HWND                    g_hWnd = NULL;
HWND                    g_hDlgAbout = NULL;
HICON                   g_hIcon[2] = {NULL};
CServer*                g_pServer = NULL;

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                    RefreshTaskbar();
static void             GetTaskbarData(NOTIFYICONDATA* pnid);
static BOOL             InitTaskbar();
static void             ReleaseTaskbar();
static void             DisplayMenu();
static BOOL CALLBACK    AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//***********************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message) {

        case WM_CREATE:
            g_hWnd = hWnd;
            if (!InitTaskbar()) {
                return -1;
            }
            return 0;

        case WM_TASKBAR:

            switch (lParam) {

                case WM_LBUTTONUP:
//                    ToggleState();
                    break;

                case WM_MBUTTONUP:
                case WM_RBUTTONUP:
                    DisplayMenu();
                    break;
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rect);
            EndPaint(hWnd, &ps);
            return 0;

        case WM_DESTROY:
            ReleaseTaskbar();
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;

        case WM_COMMAND:

            switch (LOWORD(wParam)) {

//                case IDM_ACTIVE: {
//                    ToggleState();
//                    return 0;
//                }
                
                case IDM_ABOUT:
                    if (g_hDlgAbout) {
                        SetForegroundWindow(g_hDlgAbout);
                    }
                    else {
                        DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), g_hWnd, (DLGPROC)AboutDlgProc);
                    }
                    return 0;

                case IDM_EXIT:
                    ReleaseTaskbar();
                    PostQuitMessage(0);
                    return 0;
            }
   }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//***********************************************************************************
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

        case WM_INITDIALOG: {

            RECT rect;

            g_hDlgAbout = hDlg;
            GetWindowRect(hDlg, &rect);
            MoveWindow(hDlg, (GetSystemMetrics(SM_CXSCREEN) - (rect.right  - rect.left)) / 2,
                    (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top )) / 2,
                    rect.right - rect.left, rect.bottom - rect.top, FALSE);
            return TRUE;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    g_hDlgAbout = NULL;
                    EndDialog(hDlg, LOWORD(wParam));
            }
            break;
    }

    return FALSE;
}

//***********************************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

    MSG      msg;
    WNDCLASS wc;
    BOOL     bRegistered = FALSE;

    g_hInstance = hInstance;

    if (!CreateHeap()) {
        return 0;
    }

    // Register the window class
    memset(&wc, 0, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;   // Window Procedure                       
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;          // Owner of this class
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.hCursor       = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = g_szAppName;

    RegisterClass(&wc);
    bRegistered = TRUE;

    // Create the main application window
    CreateWindow(g_szAppName, g_szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
                                    CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    // Create the transport
    g_pServer = new CServer();
    if (g_pServer) {

        if (g_pServer->Create()) {

        //    ShowWindow(g_hWnd, nCmdShow);
        //    UpdateWindow(g_hWnd);

            while (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        delete g_pServer;
    }

    DestroyWindow(g_hWnd);

    if (bRegistered) {
        UnregisterClass(g_szAppName, hInstance);
    }

    ReleaseHeap();

    return msg.wParam;
}

//***********************************************************************************
void GetTaskbarData(NOTIFYICONDATA* pnid) {

    // Initialize the taskbar status icon
    pnid->cbSize = sizeof(NOTIFYICONDATA);
    pnid->hWnd = g_hWnd;
    pnid->uID = 0;
    pnid->uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    pnid->uCallbackMessage = WM_TASKBAR;
    if (g_pServer && g_pServer->IsConnected()) {
        pnid->hIcon = g_hIcon[ICON_CONNECTED];
        _tcscpy(pnid->szTip, TEXT("D3DLocus [Connected]"));
    }
    else {
        pnid->hIcon = g_hIcon[ICON_DISCONNECTED];
        _tcscpy(pnid->szTip, TEXT("D3DLocus [Disconnected]"));
    }
}

//***********************************************************************************
BOOL InitTaskbar() {

    NOTIFYICONDATA nid;

    // Load the icons that will appear in the bar
    g_hIcon[ICON_DISCONNECTED] = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_DISCONNECTED),
                                        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    g_hIcon[ICON_CONNECTED] = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_CONNECTED),
                                        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

    // Set the taskbar icon
    GetTaskbarData(&nid);
    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        return FALSE;
    }

    return TRUE;
}

//***********************************************************************************
void ReleaseTaskbar() {

    NOTIFYICONDATA nid;
    UINT           i;

    // Remove the application from the taskbar
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hWnd;
    
    if (!Shell_NotifyIcon(NIM_DELETE, &nid)) {
        DebugString(TEXT("Shell_NotifyIcon() failed"));
    }

    // Release the taskbar icons
    for (i = 0; i < 2; i++) {
        if (g_hIcon[i]) {
            DestroyIcon(g_hIcon[i]);
            g_hIcon[i] = NULL;
        }
    }
}

//***********************************************************************************
void DisplayMenu() {

    HMENU hMenu, hMenuSub;
    POINT pt;

    // Load the menu
    hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU));
    if (!hMenu) {
        return;
    }
    hMenuSub = GetSubMenu(hMenu, 0);
    if (!hMenuSub) {
        return;
    }

    // Initialize the menu
//    CheckMenuItem(hMenu, IDM_ACTIVE, MF_BYCOMMAND | 
//                    (g_pSchedule->IsActive() ? MF_CHECKED : MF_UNCHECKED));

    // Determine the menu position
    GetCursorPos(&pt);
    pt.y = GetSystemMetrics(SM_CYSCREEN);

    // Display the menu
    TrackPopupMenu(hMenuSub, TPM_BOTTOMALIGN | TPM_RIGHTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, g_hWnd, NULL);

    // Delete the menu
    DestroyMenu(hMenuSub);
    DestroyMenu(hMenu);
}

//***********************************************************************************
BOOL RefreshTaskbar() {

    NOTIFYICONDATA nid;
    GetTaskbarData(&nid);
    return (Shell_NotifyIcon(NIM_MODIFY, &nid) == TRUE);
}
