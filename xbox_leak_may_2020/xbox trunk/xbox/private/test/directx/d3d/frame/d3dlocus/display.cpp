/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    display.cpp

Author:

    Matt Bronder

Description:

    Direct3D initialization routines.

*******************************************************************************/

#include "d3dlocus.h"
#include "conio.h"

#define DEMO_HACK

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

// View and projection transform data
#define VIEW_POS                    D3DXVECTOR3(0.0f, 0.0f, 0.0f)
#define VIEW_AT                     D3DXVECTOR3(0.0f, 0.0f, 1.0f)
#define VIEW_UP                     D3DXVECTOR3(0.0f, 1.0f, 0.0f)
#define PROJ_NEAR                   0.1f
#define PROJ_FAR                    1000.0f
#define PROJ_FOV                    (M_PI / 4.0f)

// Resource identifiers
#define IDM_DEFAULT_FILE_DISPLAY    1
#define IDM_DEFAULT_FILE_EXIT       2

#ifndef IDC_STATIC
#define IDC_STATIC                  (-1)
#endif
#define IDC_DISPLAY_ADAPTER         101
#define IDC_DISPLAY_DEVICE          102
#define IDC_DISPLAY_MODE            103
#define IDC_DISPLAY_FULLSCREEN      104
#define IDC_DISPLAY_DEPTHBUFFER     105
#define IDC_DISPLAY_ANTIALIAS       106

//******************************************************************************
// Function prototypes
//******************************************************************************

#ifndef UNDER_XBOX

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
                                        LPARAM lParam);

static BOOL CALLBACK    DisplayDlgProc(HWND hDlg, UINT message, WPARAM wParam, 
                                        LPARAM lParam);

#endif

//******************************************************************************
// Window procedures
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Window procedure to process messages for the main application window.
//
// Arguments:
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     0 if the message was handled, the return of DefWindowProc otherwise.
//
//******************************************************************************
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    CDisplay* pDisplay;

    if (uMsg == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        SetWindowLong(hWnd, GWL_USERDATA, (LONG)(pcs->lpCreateParams));
    }

    pDisplay = (CDisplay*)GetWindowLong(hWnd, GWL_USERDATA);
    if (pDisplay) {
        return pDisplay->WndProc(hWnd, uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

//******************************************************************************
//
// Function:
//
//     DisplayDlgProc
//
// Description:
//
//     Window procedure to process messages for the display dialog.
//
// Arguments:
//
//     HWND hDlg                - Dialog window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL CALLBACK DisplayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    CDisplay* pDisplay;

    if (uMsg == WM_INITDIALOG) {
        SetWindowLong(hDlg, GWL_USERDATA, lParam);
    }

    pDisplay = (CDisplay*)GetWindowLong(hDlg, GWL_USERDATA);
    if (pDisplay) {
        return pDisplay->DisplayDlgProc(hDlg, uMsg, wParam, lParam);
    }
    else {
        return FALSE;
    }
}

#endif // !UNDER_XBOX

//******************************************************************************
// CDisplay
//******************************************************************************

UINT CDisplay::m_uCreated = 0;

//******************************************************************************
// Initialization methods
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDisplay
//
// Description:
//
//     Initialize a display object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CDisplay::CDisplay() {

    m_pDirect3D = NULL;
    m_pDevice = NULL;
#ifdef UNDER_XBOX
    memset(m_pvColorBuffer, 0, 3 * sizeof(LPVOID));
    m_pvDepthBuffer = NULL;
#endif
    m_padpdList = NULL;
    memset(&m_disdCurrent, 0, sizeof(DISPLAYDESC));
    m_hWnd = NULL;
    m_kdKeys = 0;
    memset(&m_jsJoys, 0, sizeof(JOYSTATE));
    memset(&m_jsLast, 0, sizeof(JOYSTATE));
    m_bUseInput = TRUE;
    m_bSwapSticks = TRUE;
    m_bActive = TRUE;
    m_bTimeSync = FALSE;
    m_bShowConsole = FALSE;
    m_cam.vPosition     = VIEW_POS;
    m_cam.vInterest     = VIEW_AT;
    m_cam.fRoll         = 0.0f;
    m_cam.fFieldOfView  = PROJ_FOV;
    m_cam.fNearPlane    = PROJ_NEAR;
    m_cam.fFarPlane     = PROJ_FAR;
    m_fCameraTDelta = 0.5f;
    m_fCameraRDelta = 0.011415f;
    m_pfnSetDevice = NULL;
    m_pfnClearDevice = NULL;
    m_pfnSubWndProc = NULL;
    m_pvSetDeviceParam = NULL;
    m_pvClearDeviceParam = NULL;
    m_pvSubWndProcParam = NULL;
    m_pClient = NULL;
    m_uCreated++;
    m_bCreated = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CDisplay
//
// Description:
//
//     Release all display objects.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CDisplay::~CDisplay() {

    PADAPTERDESC padpd;
    PDEVICEDESC  pdevd;
    PDISPLAYMODE pdm;

    // Release the device
    ReleaseDevice();

    // Release the Direct3D object
    if (m_pDirect3D) {
        m_pDirect3D->Release();
        m_pDirect3D = NULL;
    }

    if (m_pClient) {
        delete m_pClient;
        m_pClient = NULL;
    }

    // Release input
    ReleaseInput();

#ifndef UNDER_XBOX

    MoveWindow(m_hWnd, m_rectWnd.left, m_rectWnd.top, 
               m_rectWnd.right - m_rectWnd.left, 
               m_rectWnd.bottom - m_rectWnd.top, TRUE);

#endif // !UNDER_XBOX

    // Free the display device list
    for (padpd = m_padpdList; m_padpdList; padpd = m_padpdList) {
        m_padpdList = m_padpdList->padpdNext;
        for (pdevd = padpd->pdevdList; padpd->pdevdList; pdevd = padpd->pdevdList) {
            padpd->pdevdList = padpd->pdevdList->pdevdNext;
            for (pdm = pdevd->pdmList; pdevd->pdmList; pdm = pdevd->pdmList) {
                pdevd->pdmList = pdevd->pdmList->pdmNext;
                MemFree(pdm);
            }
            MemFree(pdevd);
        }
        MemFree(padpd);
    }

    // Reset any application supplied window procedure
    m_pfnSubWndProc = NULL;

#ifndef UNDER_XBOX

    // Unregister the window class
    UnregisterClass(TEXT("Direct3D Application"), GetTestInstance());

#endif // !UNDER_XBOX

#ifndef UNDER_XBOX
    // Free the console
    UnloadConsole();
#endif // !UNDER_XBOX

    if (--m_uCreated == 0) {
        RegisterDisplay(NULL);
    }
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Initialize DX objects needed for 3D rendering.
//
// Arguments:
//
//     PD3DCREATIONDESC pd3dcd          - Pointer to creation parameters
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::Create(PD3DCREATIONDESC pd3dcd) {

    TCHAR           szPath[MAX_PATH];
    LPTSTR          szFile, sz;
    BOOL            bWindowedDefault = TRUE;
    BOOL            bDepthBufferDefault = TRUE;
    BOOL            bAntialiasDefault = TRUE;
    UINT            uNumBackBuffers = 1;
    RECT            rect, rectWndDefault = {30, 30, 550, 460};
    POINT           point;
    PADAPTERDESC    padpd;
    PDEVICEDESC     pdevd;
    PDISPLAYMODE    pdm;
#ifndef UNDER_XBOX
	WNDCLASS        wc;
#endif

    if (m_bCreated || m_uCreated > 1) {
        return FALSE;
    }

#ifndef UNDER_XBOX
    // Load the console to buffer text output
    if (LoadConsole()) {
        return FALSE;
    }
#endif // !UNDER_XBOX

    // Set the creation parameters
    if (pd3dcd) {
#ifdef UNDER_XBOX
        if (pd3dcd->d3ddm.Format == D3DFMT_X8R8G8B8) pd3dcd->d3ddm.Format = D3DFMT_LIN_X8R8G8B8;
        if (pd3dcd->d3ddm.Format == D3DFMT_A8R8G8B8) pd3dcd->d3ddm.Format = D3DFMT_LIN_A8R8G8B8;
        if (pd3dcd->d3ddm.Format == D3DFMT_X1R5G5B5) pd3dcd->d3ddm.Format = D3DFMT_LIN_X1R5G5B5;
        if (pd3dcd->d3ddm.Format == D3DFMT_R5G6B5) pd3dcd->d3ddm.Format = D3DFMT_LIN_R5G6B5;
        if (pd3dcd->fmtd == D3DFMT_D16) pd3dcd->fmtd = D3DFMT_LIN_D16;
        if (pd3dcd->fmtd == D3DFMT_D24S8) pd3dcd->fmtd = D3DFMT_LIN_D24S8;
#endif // UNDER_XBOX
        if (pd3dcd->d3ddt == D3DDEVTYPE_HAL ||
            pd3dcd->d3ddt == D3DDEVTYPE_SW ||
            pd3dcd->d3ddt == D3DDEVTYPE_REF)
        {
            g_devdDefault.d3ddt = pd3dcd->d3ddt;
        }
        if (pd3dcd->d3ddm.Width != 0 && pd3dcd->d3ddm.Height != 0) {
            memcpy(&g_dmDefault.d3ddm, &pd3dcd->d3ddm, sizeof(D3DDISPLAYMODE));
        }
        g_fmtdDefault = pd3dcd->fmtd;
        if (pd3dcd->mst != D3DMULTISAMPLE_NONE) {
            g_mstDefault = pd3dcd->mst;
        }
        g_devdDefault.uPresentInterval = pd3dcd->uPresentInterval;
        g_devdDefault.dwBehavior = pd3dcd->dwBehavior;

        if (pd3dcd->rectWnd.right > 0 && pd3dcd->rectWnd.bottom > 0) {
            memcpy(&rectWndDefault, &pd3dcd->rectWnd, sizeof(RECT));
        }
        bWindowedDefault = pd3dcd->bWindowed;
        bDepthBufferDefault = pd3dcd->bDepthBuffer;
        bAntialiasDefault = pd3dcd->bAntialias;
        uNumBackBuffers = pd3dcd->uNumBackBuffers;
    }

#ifndef UNDER_XBOX

    // Register the window class
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style            = 0;
    wc.lpfnWndProc      = ::WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = GetTestInstance();
    wc.hIcon            = NULL;
    wc.hbrBackground    = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = TEXT("Direct3D Application");

    if (!RegisterClass(&wc)) {
        DebugString(TEXT("RegisterClass failed"));
        return FALSE;
    }

    // Get the name of the executable
    _tcscpy(szPath, GetCommandLine());
    szFile = _tcstok(szPath, TEXT(" \t"));
    sz = szFile + _tcslen(szFile) - 1;
    if (*sz == TEXT('"')) {
        *sz = TEXT('\0');
    }
    sz = szFile + _tcslen(szFile) - 4;
    if (!_tcsncmp(sz, TEXT(".exe"), 4)) {
        *sz = TEXT('\0');
    }
    for (sz = szFile + _tcslen(szFile); sz != szFile && *(sz - 1) != TEXT('\\'); sz--);

    // Create the main application window
    m_hWnd = CreateWindowEx(0, TEXT("Direct3D Application"), 
                            sz, 
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            rectWndDefault.left, rectWndDefault.top,
                            rectWndDefault.right - rectWndDefault.left, 
                            rectWndDefault.bottom - rectWndDefault.top,
                            NULL, CreateMainMenu(), GetTestInstance(), this);

    if (!m_hWnd) {
        DebugString(TEXT("CreateWindowEx failed"));
		return FALSE;
    }

    UpdateWindow(m_hWnd);

    // Get the client area of the window
    if (!GetClientRect(m_hWnd, &rect)) {
        return FALSE;
    }

    // Convert the client window coordinates to screen coordinates
    point.x = rect.left;
    point.y = rect.top;
    if (!ClientToScreen(m_hWnd, &point)) {
        return FALSE;
    }

    // Save the screen coordinates of the client window
    SetRect(&m_rectWnd, point.x, point.y, 
            point.x + rect.right, point.y + rect.bottom);

    // Get the size offsets of the window border
    GetWindowRect(m_hWnd, &m_rectBorder);
    SetRect(&m_rectBorder, m_rectWnd.left - m_rectBorder.left,
            m_rectWnd.top - m_rectBorder.top,
            m_rectBorder.right - m_rectWnd.right,
            m_rectBorder.bottom - m_rectWnd.bottom);

#else

    m_hWnd = NULL;

#endif // UNDER_XBOX

    // Initialize input
    if (m_bUseInput && !(GetStartupContext() & TSTART_PARALLEL)) {
        CreateInput(m_hWnd);
    }

    // Create the client object and connect to the server (if present)
    m_pClient = new CClient();
    if (m_pClient) {
        if (!m_pClient->Create()) {
            delete m_pClient;
            m_pClient = NULL;
        }
    }

    // Create the Direct3D object
    if (m_pClient) {
        m_pDirect3D = new CCDirect3D8();
        if (!m_pDirect3D) {
            DebugString(TEXT("CCDirect3D8::CCDirect3D8 failed"));
            return FALSE;
        }

        if (!((CCDirect3D8*)m_pDirect3D)->Create(m_pClient)) {
            DebugString(TEXT("CCDirect3D8::Create failed"));
            return FALSE;
        }
    }
    else {
        m_pDirect3D = new CDirect3D8();
        if (!m_pDirect3D) {
            DebugString(TEXT("CDirect3D8::CDirect3D8 failed"));
            return FALSE;
        }

        if (!m_pDirect3D->Create()) {
            DebugString(TEXT("CDirect3D8::Create failed"));
            return FALSE;
        }
    }

    // Enumerate all display devices in the system and store the information in
    // a driver list
    m_padpdList = EnumDisplays(m_pDirect3D);
    if (!m_padpdList) {
        return FALSE;
    }

    // Select a default driver
    padpd = SelectAdapter(m_padpdList);
    if (!padpd) {
        return FALSE;
    }

    // Select a default device
    pdevd = SelectDevice(padpd);
    if (!pdevd) {
        return FALSE;
    }

    // Select a default display mode (in case of full screen)
    pdm = SelectDisplayMode(pdevd);
    if (!pdm) {
        return FALSE;
    }

    m_bCreated = TRUE;

    // Create the device
    if (!CreateDevice(padpd, pdevd, pdm, uNumBackBuffers, bWindowedDefault, bDepthBufferDefault, bAntialiasDefault)) {
        m_bCreated = FALSE;
        return FALSE;
    }

    // Create a console to display text output
    if (!xCreateConsole(m_pDevice)) {
        m_bCreated = FALSE;
        return FALSE;
    }

    RegisterDisplay(this);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     CreateDevice
//
// Description:
//
//     Create the device object using the given adapter, device, display
//     mode, and other information.
//
// Arguments:
//
//     PADAPTERDESC padpd       - The adapter to use in creating the device
//                                object
//
//     PDEVICEDESC pdevd        - The device to use in creating the device
//                                object
//
//     PDISPLAYMODE pdm         - The display mode to use in creating the
//                                device object (if full screen)
//
//     UINT uNumBackBuffers     - The number of back buffers (including
//                                prefilter buffer for FSAA) to create
//
//     BOOL bWindowed           - The windowed mode to use with the device
//                                (windowed or full screen)
//
//     BOOL bDepthBuffer        - Indicates whether or not to use an automatic
//                                depth/stencil buffer with the device
//
//     BOOL bAntialias          - Indicates whether or not to create a
//                                multi-sample swap chain for use in FSAA
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::CreateDevice(PADAPTERDESC padpd, PDEVICEDESC pdevd, 
                            PDISPLAYMODE pdm, UINT uNumBackBuffers, 
                            BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias)
{
    CSurface8*  pd3ds;
    BOOL        bRMC;
    HRESULT     hr;

    if (!m_bCreated || !padpd || !pdevd || !pdm) {
        return FALSE;
    }

    // Check if windowed mode is available
    if (!pdevd->bCanRenderWindowed) {
        bWindowed = FALSE;
    }

    // Check if antialiasing is available
    if (!ANTIALIAS_SUPPORTED(pdevd, pdm, bWindowed)) {
        bAntialias = FALSE;
    }

    // Initialize the presentation parameters
    memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
        m_d3dpp.BackBufferFormat                  = padpd->d3ddmWindowed.Format;
        m_d3dpp.MultiSampleType                   = pdevd->d3dmstWindowed;
        m_d3dpp.AutoDepthStencilFormat            = pdevd->fmtdWindowed;
    }
    else {
        m_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        m_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        m_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        m_d3dpp.MultiSampleType                   = pdm->d3dmst;
        m_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        m_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        m_d3dpp.FullScreen_PresentationInterval   = pdevd->uPresentInterval;
    }
    m_d3dpp.BackBufferCount                       = uNumBackBuffers;
    m_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.hDeviceWindow                         = m_hWnd;
    m_d3dpp.Windowed                              = bWindowed;
    m_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    m_d3dpp.Flags                                 = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    if (!bAntialias) {
        m_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

#ifdef DEMO_HACK
    m_pDevice = NULL;
#endif

    // Create the device
DebugString(TEXT("D3D - Startup context: 0x%X"), GetStartupContext());

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {
        hr = CreateDeviceAB(padpd->uAdapter, pdevd->d3ddt, m_hWnd, pdevd->dwBehavior, &m_d3dpp, &m_pDevice);
    }
    else {
#endif
        hr = m_pDirect3D->CreateDevice(padpd->uAdapter, pdevd->d3ddt, m_hWnd, pdevd->dwBehavior, &m_d3dpp, &m_pDevice);
#ifdef UNDER_XBOX
    }
#endif

    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice"))) {
        return FALSE;
    }

#ifdef DEMO_HACK
    if (m_pDevice == NULL) {
        OutputDebugString(TEXT("D3D: Error - CreateDevice succeeded but returned a NULL device pointer"));
        __asm int 3;
    }
#endif

    // Get the surface description of the back buffer
    bRMC = RMCEnabled();
    EnableRMC(FALSE);

#ifndef UNDER_XBOX
    hr = m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#else
    hr = m_pDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    hr = pd3ds->GetDesc(&m_d3dsdBack);
    pd3ds->Release();
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        return FALSE;
    }

    EnableRMC(bRMC);

    // Initialize the viewport
    SetViewport(m_d3dsdBack.Width, m_d3dsdBack.Height);

    // Initialize the default states of the device
    if (!InitDeviceState()) {
        return FALSE;
    }

    // Initialize the camera
    if (!SetView(&m_cam)) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

    m_disdCurrent.padpd = padpd;
    m_disdCurrent.pdevd = pdevd;
    m_disdCurrent.pdm = pdm;
    m_disdCurrent.uNumBackBuffers = uNumBackBuffers;
    m_disdCurrent.bWindowed = bWindowed;
    m_disdCurrent.bDepthBuffer = bDepthBuffer;
    m_disdCurrent.bAntialias = bAntialias;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     ResetDevice
//
// Description:
//
//     Release all device objects in a scene, reset the device with new
//     presentation parameters, and recreate the device objects in the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::ResetDevice() {

    CSurface8*  pd3ds;
    BOOL        bRMC;
    HRESULT     hr;

    if (!m_bCreated) {
        return FALSE;
    }

    if (m_pfnClearDevice) {
        m_pfnClearDevice(m_pvClearDeviceParam);
    }

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {

        m_pDevice->BlockUntilIdle();

        if (m_pvColorBuffer[0]) {
            m_pDevice->SetTile(0, NULL);
        }
        if (m_pvDepthBuffer) {
            m_pDevice->SetTile(1, NULL);
        }
        for (UINT i = 0; i < 3; i++) {
            if (m_pvColorBuffer[i]) {
                D3D_FreeContiguousMemory(m_pvColorBuffer[i]);
                m_pvColorBuffer[i] = NULL;
            }
        }
        if (m_pvDepthBuffer) {
            D3D_FreeContiguousMemory(m_pvDepthBuffer);
            m_pvDepthBuffer = NULL;
        }

        hr = ResetAB(&m_d3dpp);
    }
    else {
#endif
        hr = m_pDevice->Reset(&m_d3dpp);
#ifdef UNDER_XBOX
    }
#endif

    if (ResultFailed(hr, TEXT("IDirect3DDevice8::Reset"))) {
        return FALSE;
    }

    // Get the surface description of the back buffer
    bRMC = RMCEnabled();
    EnableRMC(FALSE);

#ifndef UNDER_XBOX
    hr = m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#else
    hr = m_pDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#endif // UNDER_XBOX
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }
    hr = pd3ds->GetDesc(&m_d3dsdBack);
    pd3ds->Release();
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        return FALSE;
    }

    EnableRMC(bRMC);

    // Initialize the viewport
    SetViewport(m_d3dsdBack.Width, m_d3dsdBack.Height);

#ifndef UNDER_XBOX  // Only reset the initial device state on WinX
    InitDeviceState();
#endif // !UNDER_XBOX

    // Initialize the camera
    if (!SetView(&m_cam)) {
        return FALSE;
    }

    if (!xResetConsole()) {
        return FALSE;
    }

    if (m_pfnSetDevice) {
        if (!m_pfnSetDevice(m_pvSetDeviceParam)) {
            return FALSE;
        }
    }

    return TRUE;
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDisplay::CreateDeviceAB(UINT uAdapter, D3DDEVTYPE d3ddt, void*, 
                                    DWORD dwFlags, D3DPRESENT_PARAMETERS* pd3dpp, 
                                    CDevice8** ppDevice)
{
    D3DTILE tile[2];
    HRESULT hr;

    hr = AllocateDeviceSurfaces(pd3dpp, &tile[0], &tile[1]);

    if (SUCCEEDED(hr)) {

        hr = m_pDirect3D->CreateDevice(uAdapter, d3ddt, NULL, dwFlags, pd3dpp, ppDevice);

        if (SUCCEEDED(hr)) {
            (*ppDevice)->SetTile(0, &tile[0]);
            if (pd3dpp->EnableAutoDepthStencil) {
                (*ppDevice)->SetTile(1, &tile[1]);
            }
        }
    }

    return hr;
}

//******************************************************************************
HRESULT CDisplay::ResetAB(D3DPRESENT_PARAMETERS* pd3dpp) {

    D3DTILE tile[2];
    HRESULT hr;

    hr = AllocateDeviceSurfaces(pd3dpp, &tile[0], &tile[1]);

    if (SUCCEEDED(hr)) {

        hr = m_pDevice->Reset(pd3dpp);

        if (SUCCEEDED(hr)) {
            m_pDevice->SetTile(0, &tile[0]);
            if (pd3dpp->EnableAutoDepthStencil) {
                m_pDevice->SetTile(1, &tile[1]);
            }
        }
    }

    return hr;
}

//******************************************************************************
UINT AlignTilePitch(UINT uPitch) {

    UINT i;
    UINT uPitchAlignment[26] = {
        D3DTILE_PITCH_0200,
        D3DTILE_PITCH_0300,
        D3DTILE_PITCH_0400,
        D3DTILE_PITCH_0500,
        D3DTILE_PITCH_0600,
        D3DTILE_PITCH_0700,
        D3DTILE_PITCH_0800,
        D3DTILE_PITCH_0A00,
        D3DTILE_PITCH_0C00,
        D3DTILE_PITCH_0E00,
        D3DTILE_PITCH_1000,
        D3DTILE_PITCH_1400,
        D3DTILE_PITCH_1800,
        D3DTILE_PITCH_1C00,
        D3DTILE_PITCH_2000,
        D3DTILE_PITCH_2800,
        D3DTILE_PITCH_3000,
        D3DTILE_PITCH_3800,
        D3DTILE_PITCH_4000,
        D3DTILE_PITCH_5000,
        D3DTILE_PITCH_6000,
        D3DTILE_PITCH_7000,
        D3DTILE_PITCH_8000,
        D3DTILE_PITCH_A000,
        D3DTILE_PITCH_C000,
        D3DTILE_PITCH_E000,
    };

    for (i = 0; i < 26; i++) {
        if (uPitch <= uPitchAlignment[i]) {
            return uPitchAlignment[i];
        }
    }

    return 0;
}

//******************************************************************************
HRESULT CDisplay::AllocateDeviceSurfaces(D3DPRESENT_PARAMETERS* pd3dpp, D3DTILE* ptileColor, D3DTILE* ptileDepth) {

    D3DFORMAT fmt;
    UINT uWidth, uHeight, uBuffers;
    UINT uSize, uPitch;
    UINT i, j;
    HRESULT hr;

    if (!pd3dpp || !ptileColor) {
        return FALSE;
    }

    fmt = pd3dpp->BackBufferFormat;
    
    switch (pd3dpp->MultiSampleType & 0xF0000) {
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5:
            fmt = D3DFMT_LIN_X1R5G5B5;
            break;
        case D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5:
            fmt = D3DFMT_LIN_R5G6B5;
            break;
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8:
            fmt = D3DFMT_LIN_X8R8G8B8;
            break;
        case D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8:
            fmt = D3DFMT_LIN_A8R8G8B8;
            break;
    }

    uWidth = pd3dpp->BackBufferWidth;
    uHeight = pd3dpp->BackBufferHeight;

    switch (pd3dpp->MultiSampleType & 0xFFFF) {
        case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR:
        case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX:
        case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR:
            uWidth *= 2;
            break;
        case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
            uHeight *= 2;
            break;
        case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR:
        case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN:
        case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR:
        case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
            uWidth *= 2;
            uHeight *= 2;
            break;
        case D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN:
        case D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
            uWidth *= 3;
            uHeight *= 3;
            break;
    }

    if ((pd3dpp->MultiSampleType & 0xFFFF) == D3DMULTISAMPLE_NONE) {
        uBuffers = pd3dpp->BackBufferCount + 1;
        if (uBuffers == 1) {
            uBuffers = 2;
        }
    }
    else {
        uBuffers = 1;
    }

    uPitch = AlignTilePitch(uWidth * XGBytesPerPixelFromFormat(fmt));
    uSize = uPitch * uHeight * uBuffers;
    uSize = (uSize + D3DTILE_ALIGNMENT - 1) & ~(D3DTILE_ALIGNMENT - 1);

    m_pvColorBuffer[0] = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
    if (!m_pvColorBuffer[0]) {
        OutputDebugString(TEXT("Insufficient contiguous memory available to create color buffer surface(s)\r\n"));
        return E_OUTOFMEMORY;
    }

    memset(ptileColor, 0, sizeof(D3DTILE));
    ptileColor->Size = uSize;
    ptileColor->Pitch = uPitch;
    ptileColor->pMemory = m_pvColorBuffer[0];

    if ((pd3dpp->MultiSampleType & 0xFFFF) == D3DMULTISAMPLE_NONE) {

        for (i = 0; i < uBuffers; i++) {
            XGSetSurfaceHeader(pd3dpp->BackBufferWidth, pd3dpp->BackBufferHeight, pd3dpp->BackBufferFormat, &m_d3dsColorBuffer[i], 0, uPitch);
            m_d3dsColorBuffer[i].Register((LPBYTE)m_pvColorBuffer[0] + i * (uPitch * uHeight));
            pd3dpp->BufferSurfaces[i] = &m_d3dsColorBuffer[i];
        }
    }
    else {

        XGSetSurfaceHeader(uWidth, uHeight, fmt, &m_d3dsColorBuffer[0], 0, uPitch);
        m_d3dsColorBuffer[0].Register((LPBYTE)m_pvColorBuffer[0]);
        pd3dpp->BufferSurfaces[0] = &m_d3dsColorBuffer[0];

        uPitch = AlignTilePitch(pd3dpp->BackBufferWidth * XGBytesPerPixelFromFormat(pd3dpp->BackBufferFormat));
        uSize = uPitch * pd3dpp->BackBufferHeight;
        uBuffers = pd3dpp->BackBufferCount ? pd3dpp->BackBufferCount : 1;

        for (i = 1; i <= uBuffers; i++) {
            m_pvColorBuffer[i] = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
            if (!m_pvColorBuffer[i]) {
                OutputDebugString(TEXT("Insufficient contiguous memory available to create postfilter buffer surface\r\n"));
                for (j = 0; j < i; j++) {
                    if (m_pvColorBuffer[j]) {
                        D3D_FreeContiguousMemory(m_pvColorBuffer[j]);
                        m_pvColorBuffer[j] = NULL;
                    }
                }
                return E_OUTOFMEMORY;
            }

            XGSetSurfaceHeader(pd3dpp->BackBufferWidth, pd3dpp->BackBufferHeight, pd3dpp->BackBufferFormat, &m_d3dsColorBuffer[i], 0, uPitch);
            m_d3dsColorBuffer[i].Register((LPBYTE)m_pvColorBuffer[i]);
            pd3dpp->BufferSurfaces[i] = &m_d3dsColorBuffer[i];
        }
    }

    if (pd3dpp->EnableAutoDepthStencil) {

        uPitch = AlignTilePitch(uWidth * XGBytesPerPixelFromFormat(pd3dpp->AutoDepthStencilFormat));
        uSize = uPitch * uHeight;
        uSize = (uSize + D3DTILE_ALIGNMENT - 1) & ~(D3DTILE_ALIGNMENT - 1);

        m_pvDepthBuffer = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
        if (!m_pvDepthBuffer) {
            OutputDebugString(TEXT("Insufficient contiguous memory available to create depth buffer surface\r\n"));
            for (i = 0; i < 3; i++) {
                if (m_pvColorBuffer[i]) {
                    D3D_FreeContiguousMemory(m_pvColorBuffer[i]);
                    m_pvColorBuffer[i] = NULL;
                }
            }
            return E_OUTOFMEMORY;
        }

        if (ptileDepth) {
            memset(ptileDepth, 0, sizeof(D3DTILE));
            ptileDepth->Size = uSize;
            ptileDepth->Pitch = uPitch;
            ptileDepth->pMemory = m_pvDepthBuffer;
            ptileDepth->Flags = D3DTILE_FLAGS_ZBUFFER | D3DTILE_FLAGS_ZCOMPRESS;
            ptileDepth->ZStartTag = 0;
            ptileDepth->ZOffset = 0;
            if (XGBytesPerPixelFromFormat(pd3dpp->AutoDepthStencilFormat) == 4) {
                ptileDepth->Flags |= D3DTILE_FLAGS_Z32BITS;
            }
        }

        XGSetSurfaceHeader(uWidth, uHeight, pd3dpp->AutoDepthStencilFormat, &m_d3dsDepthBuffer, 0, uPitch);
        m_d3dsDepthBuffer.Register(m_pvDepthBuffer);

        pd3dpp->DepthStencilSurface = &m_d3dsDepthBuffer;
    }
    else {
        pd3dpp->DepthStencilSurface = NULL;
    }

    return D3D_OK;
}

#endif

//******************************************************************************
//
// Method:
//
//     InitDeviceState
//
// Description:
//
//     Initialize the default device states.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::InitDeviceState() {

    D3DMATRIX           mTransform;
    BOOL                bRet = TRUE;
    UINT                i;
    HRESULT             hr;

    if (!m_bCreated) {
        return FALSE;
    }

    // Begin the scene
    hr = m_pDevice->BeginScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::BeginScene"))) {
        return FALSE;
    }

    // Set ambient lighting
    hr = m_pDevice->SetRenderState(D3DRS_AMBIENT, 
                                      (DWORD)RGBA_MAKE(32, 32, 32, 255));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the texture filters
    for (i = 0; i < 4; i++) {
#if 0 // Non-KELVIN code
    for (i = 0; i < 2; i++) {
#endif // 0
        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }

        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }
    }

    // Set blend modes
    hr = m_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Modulate color and alpha texture stages
    hr = m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    // Set the alpha comparison function
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the alpha reference value to opaque
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Turn on specular highlights
    hr = m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Enable dithering
    hr = m_pDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable per-vertex color
    hr = m_pDevice->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

/*  // ##DELETE: The following block does redundant render state calls
    // in that it sets the device to states that should already be the
    // default state.  When the default render states are incorrect
    // on a system the following block replaces the above block as a quick
    // test

    // Set ambient lighting
    hr = m_pDevice->SetRenderState(D3DRS_AMBIENT, 
                                      (DWORD)RGBA_MAKE(128, 128, 128, 128));
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Enable z-buffering
    hr = m_pDevice->SetRenderState(D3DRS_ZENABLE, (DWORD)D3DZB_TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetRenderState(D3DRS_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the texture filters
    for (i = 0; i < 4; i++) {
        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }

        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }

        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, (DWORD)D3DTEXF_NONE);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
            bRet = FALSE;
        }
    }

    // Use gouraud shading
    hr = m_pDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_GOURAUD);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Enable perspective correction
    hr = m_pDevice->SetRenderState(D3DRS_TEXTUREPERSPECTIVE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set blend modes
    hr = m_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }
    
    // Modulate color and alpha texture stages
    hr = SetColorStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = SetAlphaStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    hr = m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        bRet = FALSE;
    }

    // Set the alpha comparison function
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Set the alpha reference value to opaque
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Use a solid fill mode
    hr = m_pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }
    
    // Turn on specular highlights
    hr = m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable texture wrapping
    for (i = 0; i < 4; i++) {
        hr = m_pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_WRAP0 + i), (DWORD)0);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
            bRet = FALSE;
        }
    }

    // Enable dithering
    hr = m_pDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }

    // Disable per-vertex color
    hr = m_pDevice->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState"))) {
        bRet = FALSE;
    }
*/
    // End the scene
    hr = m_pDevice->EndScene();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::EndScene"))) {
        bRet = FALSE;
    }

    // Initialize the world matrix to the identity matrix
    InitMatrix(&mTransform,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    hr = m_pDevice->SetTransform(D3DTS_WORLD, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    return bRet;
}

//******************************************************************************
//
// Method:
//
//     SetViewport
//
// Description:
//
//     Initialize the viewport using the given dimensions for the frustum.
//
// Arguments:
//
//     DWORD dwWidth                    - Width of the viewport
//
//     DWORD dwHeight                   - Height of the viewport
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::SetViewport(DWORD dwWidth, DWORD dwHeight) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    if (!m_bCreated) {
        return FALSE;
    }

    // Set a viewport for the device
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = dwWidth;
    viewport.Height = dwHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    hr = m_pDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     WndProc
//
// Description:
//
//     Window procedure to process messages for the main application window.
//
// Arguments:
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     0 if the message was handled, the return of DefWindowProc otherwise.
//
//******************************************************************************
LRESULT CDisplay::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    // If the application supplied a window procedure, give it a chance to
    // handle the message first
    if (m_pfnSubWndProc) {
        LRESULT lr;
        if (m_pfnSubWndProc(m_pvSubWndProcParam, &lr, hWnd, uMsg, wParam, lParam)) {
            return lr;  // The message was handled, return the given LRESULT
        }
    }

#ifdef UNDER_XBOX
    return 0;
#else
    switch (uMsg) {

        case WM_ACTIVATEAPP:

            // Suspend the application if it is running in full screen mode
            if (!m_disdCurrent.bWindowed) {
                m_bActive = (BOOL)wParam;
            }

            break;

        case WM_MOVE:

            if (!MoveWindowTarget((short)LOWORD(lParam), (short)HIWORD(lParam))) {
                DestroyWindow(hWnd);
            }

            break;

        case WM_SIZE:

            if (wParam != SIZE_MINIMIZED) {
                if (!SizeWindowTarget(LOWORD(lParam), HIWORD(lParam))) {
                    DestroyWindow(hWnd);
                }
            }
            break;

        case WM_SETCURSOR:

            if (m_bActive && !m_disdCurrent.bWindowed) {
                SetCursor(NULL);
                return TRUE;
            }
            break;

        case WM_MOUSEMOVE:

            if (m_bActive) {
                POINT point;
                GetCursorPos(&point);
                ScreenToClient(hWnd, &point);
                m_pDevice->SetCursorPosition(point.x, point.y, D3DCURSOR_IMMEDIATE_UPDATE);
            }
            break;

        case WM_ENTERMENULOOP:

            if (!m_disdCurrent.bWindowed) {
// ##REVIEW: Is FlipToGDISurface gone or has it been changed to something else?  Can we simply remove the call and still have things work?
//                m_pDevice->FlipToGDISurface();
                DrawMenuBar(hWnd);
                RedrawWindow(hWnd, NULL, NULL, RDW_FRAME);
            }
            break;

        case WM_EXITMENULOOP:

            // Do not include the amount of time the menu was up in game time calculations
            m_bTimeSync = TRUE;
            break;

        case WM_POWERBROADCAST:

            switch (wParam) {

                case PBT_APMQUERYSUSPEND:
                    if (!m_disdCurrent.bWindowed) {
//                        m_pDevice->FlipToGDISurface();
                        DrawMenuBar(hWnd);
                        RedrawWindow(hWnd, NULL, NULL, RDW_FRAME);
                    }
                    break;

                case PBT_APMRESUMESUSPEND:
                    // Do not include the amount of time power management was up in game time calculations
                    m_bTimeSync = TRUE;
                    break;
            }
            break;

        case WM_SYSCOMMAND:

            switch (wParam) {

                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    if (!m_disdCurrent.bWindowed) {
                        return 1;
                    }
                    break;
            }
            break;

        case WM_COMMAND:

            switch (LOWORD(wParam)) {

                case IDM_DEFAULT_FILE_DISPLAY:
                    Select();
                    return 0;

                case IDM_DEFAULT_FILE_EXIT:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_LEFT:
                    m_kdKeys |= KEY_LEFT;
                    return 0;

                case VK_RIGHT:
                    m_kdKeys |= KEY_RIGHT;
                    return 0;

                case VK_UP:
                    m_kdKeys |= KEY_UP;
                    return 0;

                case VK_DOWN:
                    m_kdKeys |= KEY_DOWN;
                    return 0;

                case VK_ADD:
                    m_kdKeys |= KEY_ADD;
                    return 0;

                case VK_SUBTRACT:
                    m_kdKeys |= KEY_SUBTRACT;
                    return 0;

                case VK_INSERT:
                    m_kdKeys |= KEY_INSERT;
                    return 0;

                case VK_DELETE:
                    m_kdKeys |= KEY_DELETE;
                    return 0;

                case VK_HOME:
                    m_kdKeys |= KEY_HOME;
                    return 0;

                case VK_END:
                    m_kdKeys |= KEY_END;
                    return 0;

                case VK_PRIOR:
                    m_kdKeys |= KEY_PAGEUP;
                    return 0;

                case VK_NEXT:
                    m_kdKeys |= KEY_PAGEDOWN;
                    return 0;

                case VK_SHIFT:
                    m_kdKeys |= KEY_SHIFT;
                    return 0;

                case VK_ESCAPE: // Exit
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;

        case WM_KEYUP:

            switch (wParam) {

                case VK_LEFT:
                    m_kdKeys &= ~KEY_LEFT;
                    return 0;

                case VK_RIGHT:
                    m_kdKeys &= ~KEY_RIGHT;
                    return 0;

                case VK_UP:
                    m_kdKeys &= ~KEY_UP;
                    return 0;

                case VK_DOWN:
                    m_kdKeys &= ~KEY_DOWN;
                    return 0;

                case VK_ADD:
                    m_kdKeys &= ~KEY_ADD;
                    return 0;

                case VK_SUBTRACT:
                    m_kdKeys &= ~KEY_SUBTRACT;
                    return 0;

                case VK_INSERT:
                    m_kdKeys &= ~KEY_INSERT;
                    return 0;

                case VK_DELETE:
                    m_kdKeys &= ~KEY_DELETE;
                    return 0;

                case VK_HOME:
                    m_kdKeys &= ~KEY_HOME;
                    return 0;

                case VK_END:
                    m_kdKeys &= ~KEY_END;
                    return 0;

                case VK_PRIOR:
                    m_kdKeys &= ~KEY_PAGEUP;
                    return 0;

                case VK_NEXT:
                    m_kdKeys &= ~KEY_PAGEDOWN;
                    return 0;

                case VK_SHIFT:
                    m_kdKeys &= ~KEY_SHIFT;
                    return 0;

                case VK_F2:     // Open display dialog
                    Select();
                    return 0;
            }

            break;

        case WM_SYSKEYUP:

            switch (wParam) {

                case VK_RETURN:
                    Update(m_disdCurrent.pdm, !m_disdCurrent.bWindowed,
                                  m_disdCurrent.bDepthBuffer, 
                                  m_disdCurrent.bAntialias);
                    return 0;
            }

            break;

        case WM_CHAR:

            switch (wParam) {

                case TEXT('D'):
                case TEXT('d'):
                    Update(m_disdCurrent.pdm, m_disdCurrent.bWindowed, 
                        !m_disdCurrent.bDepthBuffer, m_disdCurrent.bAntialias);
                    return 0;

                case TEXT('A'):
                case TEXT('a'):
                    Update(m_disdCurrent.pdm, m_disdCurrent.bWindowed, 
                        m_disdCurrent.bDepthBuffer, !m_disdCurrent.bAntialias);
                    return 0;

                case TEXT('~'):
                case TEXT('`'):
                    m_bShowConsole = !m_bShowConsole;
                    return 0;

                case TEXT('>'):
                case TEXT('.'):
                    if (m_pClient) {
                        ((CCDevice8*)m_pDevice)->ToggleVerificationDisplay();
                    }
                    return 0;
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);

#endif // !UNDER_XBOX
}

//******************************************************************************
// Member access functions
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     GetDirect3D8
//
// Description:
//
//     Return a pointer to the Direct3D8 object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     IDirect3D8 interface pointer.
//
//******************************************************************************
CDirect3D8* CDisplay::GetDirect3D8() {

    return m_pDirect3D;
}

//******************************************************************************
//
// Method:
//
//     GetDevice8
//
// Description:
//
//     Return a pointer to the Direct3DDevice8 object.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     IDirect3DDevice8 interface pointer.
//
//******************************************************************************
CDevice8* CDisplay::GetDevice8() {

    return m_pDevice;
}

//******************************************************************************
//
// Method:
//
//     GetCurrentDisplay
//
// Description:
//
//     Return a description of the current display.
//
// Arguments:
//
//     PDISPLAYDESC pdisd       - Pointer to a structure to be filled with
//                                a description of the current display
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::GetCurrentDisplay(PDISPLAYDESC pdisd) {

    if (pdisd) {
        memcpy(pdisd, &m_disdCurrent, sizeof(DISPLAYDESC));
    }
}

//******************************************************************************
//
// Method:
//
//     GetDisplayList
//
// Description:
//
//     Return a pointer to the head of the display list.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     A pointer to the head of the display adapter list.
//
//******************************************************************************
PADAPTERDESC CDisplay::GetDisplayList() {

    return m_padpdList;
}

//******************************************************************************
//
// Method:
//
//     IsActive
//
// Description:
//
//     Indicate whether or not the display is currently active.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the display is active, FALSE if not.
//
//******************************************************************************
BOOL CDisplay::IsActive() {

    return m_bActive;
}

//******************************************************************************
//
// Method:
//
//     GetWidth
//
// Description:
//
//     Return the width of the first back buffer in the device's swap chain.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Width of the display.
//
//******************************************************************************
UINT CDisplay::GetWidth() {

    return m_d3dsdBack.Width;
}

//******************************************************************************
//
// Method:
//
//     GetHeight
//
// Description:
//
//     Return the height of the first back buffer in the device's swap chain.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Height of the display.
//
//******************************************************************************
UINT CDisplay::GetHeight() {

    return m_d3dsdBack.Height;
}

//******************************************************************************
//
// Method:
//
//     GetWindow
//
// Description:
//
//     Return the focus window for the device.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Handle of the focus window.
//
//******************************************************************************
HWND CDisplay::GetWindow() {

    return m_hWnd;
}

//******************************************************************************
//
// Method:
//
//     GetPresentParameters
//
// Description:
//
//     Return the presentation parameters used to Create (or last Reset) the 
//     current device.
//
// Arguments:
//
//     D3DPRESENT_PARAMETERS* pd3dpp    - Pointer to a structure that will contain
//                                        the presentation parameters on return
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::GetPresentParameters(D3DPRESENT_PARAMETERS* pd3dpp) {

    if (pd3dpp) {
        memcpy(pd3dpp, &m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        memset(pd3dpp->BufferSurfaces, 0, 3 * sizeof(D3DSurface*));
        pd3dpp->DepthStencilSurface = NULL;
    }
}

//******************************************************************************
//
// Method:
//
//     GetKeyState
//
// Description:
//
//     Return the current state of the keyboard.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     A combination of key flags OR'd together indicating what keys are down.
//
//******************************************************************************
KEYSDOWN CDisplay::GetKeyState() {

    return m_kdKeys;
}

//******************************************************************************
//
// Method:
//
//     GetJoyState
//
// Description:
//
//     Return the current and last states of the joystick.
//
// Arguments:
//
//     PJOYSTATE pjsCurrent     - Pointer to a structure to be filled with the
//                                current state of the joystick
//
//     PJOYSTATE pjsLast        - Pointer to a structure to be filled with the
//                                state of the joystick on the previous poll
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::GetJoyState(PJOYSTATE pjsCurrent, PJOYSTATE pjsLast) {

    if (pjsCurrent) {
        memcpy(pjsCurrent, &m_jsJoys, sizeof(JOYSTATE));
    }
    if (pjsLast) {
        memcpy(pjsLast, &m_jsLast, sizeof(JOYSTATE));
    }
}

//******************************************************************************
//
// Method:
//
//     GetCamera
//
// Description:
//
//     Return a description of the display camera.
//
// Arguments:
//
//     PCAMERA pcam             - Pointer to a structure to be filled with
//                                the camera description
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::GetCamera(PCAMERA pcam) {

    if (pcam) {
        memcpy(pcam, &m_cam, sizeof(CAMERA));
    }
}

//******************************************************************************
//
// Method:
//
//     SetCamera
//
// Description:
//
//     Initialize the display camera to the given camera description.
//
// Arguments:
//
//     PCAMERA pcam             - Pointer to a camera description to use for
//                                the display camera
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::SetCamera(PCAMERA pcam) {

    if (pcam) {
        memcpy(&m_cam, pcam, sizeof(CAMERA));
    }
}

//******************************************************************************
//
// Method:
//
//     SetCameraOffsets
//
// Description:
//
//     Set the delta values for the translation and rotation of the camera
//     when it is moved in response to user input.
//
// Arguments:
//
//     float fTranslation       - Camera translation delta value
//
//     float fRotation          - Camera rotation delta value
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::SetCameraOffsets(float fTranslation, float fRotation) {

    m_fCameraTDelta = fTranslation;
    m_fCameraRDelta = fRotation;
}

//******************************************************************************
//
// Method:
//
//     ConsoleVisible
//
// Description:
//
//     Return the state of console visibility.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the console is currently being displayed, FALSE otherwise.
//
//******************************************************************************
BOOL CDisplay::ConsoleVisible() {

    return m_bShowConsole;
}

//******************************************************************************
//
// Method:
//
//     IsWindowed
//
// Description:
//
//     Indicate if the display is windowed or full screen.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the display is windowed, FALSE if it is full screen.
//
//******************************************************************************
BOOL CDisplay::IsWindowed() {

    return m_disdCurrent.bWindowed;
}

//******************************************************************************
//
// Method:
//
//     IsDepthBuffered
//
// Description:
//
//     Indicate whether or not the display is using a depth buffer.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the display is using a depth buffer, FALSE if it is not.
//
//******************************************************************************
BOOL CDisplay::IsDepthBuffered() {

    return m_disdCurrent.bDepthBuffer;
}

//******************************************************************************
// Utilitiy functions
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CreateDevice
//
// Description:
//
//     Create a new device using the current display settings.  If present
//     parameters are given, verify the parameters, update the state of CDisplay 
//     to be in sync with the parameters, and Reset the device using the 
//     parameters.
//
// Arguments:
//
//     D3DPRESENT_PARAMETERS pd3dpp     - Present parameters to use
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::CreateDevice(D3DPRESENT_PARAMETERS* pd3dpp) {

    if (m_pDevice) {
        return FALSE;
    }

    m_bCreated = TRUE;

    if (!CreateDevice(m_disdCurrent.padpd, m_disdCurrent.pdevd, m_disdCurrent.pdm, m_disdCurrent.uNumBackBuffers, m_disdCurrent.bWindowed, m_disdCurrent.bDepthBuffer, m_disdCurrent.bAntialias) ||
        !xCreateConsole(m_pDevice) ||
        (m_pfnSetDevice && !m_pfnSetDevice(m_pvSetDeviceParam))) 
    {
        return FALSE;
    }

    if (pd3dpp) {
        return Reset(pd3dpp);
    }
    else {
        return TRUE;
    }
}

//******************************************************************************
//
// Method:
//
//     ReleaseDevice
//
// Description:
//
//     Release the device and all device objects in the scene and console.  The
//     only valid CDisplay method calls after ReleaseDevice are CreateDevice and
//     Release.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::ReleaseDevice() {

    xReleaseConsole();

    if (m_pDevice) {

        if (m_pfnClearDevice) {
            m_pfnClearDevice(m_pvClearDeviceParam);
        }

#ifdef UNDER_XBOX
        if (m_pvColorBuffer[0]) {
            m_pDevice->SetTile(0, NULL);
        }
        if (m_pvDepthBuffer) {
            m_pDevice->SetTile(1, NULL);
        }
#endif

        m_pDevice->Release();
        m_pDevice = NULL;
    }

#ifdef UNDER_XBOX
    for (UINT i = 0; i < 3; i++) {
        if (m_pvColorBuffer[i]) {
            D3D_FreeContiguousMemory(m_pvColorBuffer[i]);
            m_pvColorBuffer[i] = NULL;
        }
    }
    if (m_pvDepthBuffer) {
        D3D_FreeContiguousMemory(m_pvDepthBuffer);
        m_pvDepthBuffer = NULL;
    }
#endif

    m_bCreated = FALSE;
}

//******************************************************************************
//
// Method:
//
//     Reset
//
// Description:
//
//     Verify the given present parameters, update the state of CDisplay to be
//     in sync with the parameters, and Reset the device.
//
// Arguments:
//
//     D3DPRESENT_PARAMETERS pd3dpp     - Present parameters to use in the Reset
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::Reset(D3DPRESENT_PARAMETERS* pd3dpp) {

    D3DPRESENT_PARAMETERS   d3dpp;
    DISPLAYDESC             disdLast;
    HWND                    hWndLast;
    PDISPLAYMODE            pdm;
    D3DFORMAT               fmtr, fmtd;
    D3DMULTISAMPLE_TYPE     mst;
    BOOL                    bRestorePos;
    HRESULT                 hr;

    if (!m_bCreated || !pd3dpp) {
        return FALSE;
    }

    // Check if windowed mode is available
    if (pd3dpp->Windowed && !m_disdCurrent.pdevd->bCanRenderWindowed) {
        return FALSE;
    }

    // If the application is going from full screen to windowed, the window
    // position must be restored
    bRestorePos = (pd3dpp->Windowed && !m_disdCurrent.bWindowed);

    if (pd3dpp->Windowed) {
        // Verify the format of the back buffer matches with the display mode of the adapter
        if (m_disdCurrent.padpd->d3ddmWindowed.Format != pd3dpp->BackBufferFormat) {
            DebugString(TEXT("The given back buffer format does not match the display mode of the adapter"));
            return FALSE;
        }
        fmtr = m_disdCurrent.padpd->d3ddmWindowed.Format;
        fmtd = m_disdCurrent.pdevd->fmtdWindowed;
        mst = m_disdCurrent.pdevd->d3dmstWindowed;
    }
    else {
        // Find the given display mode in the display list
        for (pdm = m_disdCurrent.pdevd->pdmList; pdm; pdm = pdm->pdmNext) {
            if (pdm->d3ddm.Width == pd3dpp->BackBufferWidth &&
                pdm->d3ddm.Height == pd3dpp->BackBufferHeight &&
                pdm->d3ddm.Format == pd3dpp->BackBufferFormat &&
                (pdm->d3ddm.RefreshRate == pd3dpp->FullScreen_RefreshRateInHz ||
                pd3dpp->FullScreen_RefreshRateInHz == 0))
            {
                break;
            }
        }

        if (!pdm) {
            DebugString(TEXT("The given display mode is not present in the display list"));
            return FALSE;
        }

        if (pd3dpp->FullScreen_PresentationInterval != m_disdCurrent.pdevd->uPresentInterval) {
            D3DCAPS8 d3dcaps;
            hr = m_pDirect3D->GetDeviceCaps(m_disdCurrent.padpd->uAdapter, m_disdCurrent.pdevd->d3ddt, &d3dcaps);
            if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceCaps"))) {
                return FALSE;
            }
            if (!d3dcaps.PresentationIntervals & pd3dpp->FullScreen_PresentationInterval) {
                DebugString(TEXT("The current device does not support the given presentation interval"));
                return FALSE;
            }
        }

        fmtr = pdm->d3ddm.Format;
        fmtd = pdm->fmtd;
        mst = pdm->d3dmst;
    }

    if (pd3dpp->EnableAutoDepthStencil && pd3dpp->AutoDepthStencilFormat != fmtd) {
        hr = m_pDirect3D->CheckDeviceFormat(m_disdCurrent.padpd->uAdapter, m_disdCurrent.pdevd->d3ddt, fmtr, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, pd3dpp->AutoDepthStencilFormat);
        if (FAILED(hr)) {
            DebugString(TEXT("AutoDepthStencilFormat %d is not available"), pd3dpp->AutoDepthStencilFormat);
            return FALSE;
        }
        hr = m_pDirect3D->CheckDepthStencilMatch(m_disdCurrent.padpd->uAdapter, m_disdCurrent.pdevd->d3ddt, fmtr, fmtr, pd3dpp->AutoDepthStencilFormat);
        if (FAILED(hr)) {
            DebugString(TEXT("AutoDepthStencilFormat %d is not compatible with render target format %d"), pd3dpp->AutoDepthStencilFormat, fmtr);
            return FALSE;
        }
    }

    if (pd3dpp->MultiSampleType != mst) {
        hr = m_pDirect3D->CheckDeviceMultiSampleType(m_disdCurrent.padpd->uAdapter, m_disdCurrent.pdevd->d3ddt, fmtr, pd3dpp->Windowed, pd3dpp->MultiSampleType);
        if (FAILED(hr)) {
            DebugString(TEXT("Multisample type %d is not available"), pd3dpp->MultiSampleType);
            return FALSE;
        }
    }

    memcpy(&disdLast, &m_disdCurrent, sizeof(DISPLAYDESC));

    // Update the display list
    if (pd3dpp->Windowed) {
        if (pd3dpp->EnableAutoDepthStencil) {
            m_disdCurrent.pdevd->fmtdWindowed = pd3dpp->AutoDepthStencilFormat;
        }
        m_disdCurrent.pdevd->d3dmstWindowed = pd3dpp->MultiSampleType;
    }
    else {
        if (pd3dpp->EnableAutoDepthStencil) {
            pdm->fmtd = pd3dpp->AutoDepthStencilFormat;
        }
        pdm->d3dmst = pd3dpp->MultiSampleType;
        m_disdCurrent.pdevd->uPresentInterval = pd3dpp->FullScreen_PresentationInterval;
        m_disdCurrent.pdm = pdm;
    }

    m_disdCurrent.bWindowed = pd3dpp->Windowed;
    m_disdCurrent.bDepthBuffer = pd3dpp->EnableAutoDepthStencil;
    m_disdCurrent.uNumBackBuffers = pd3dpp->BackBufferCount;
    m_disdCurrent.bAntialias = (pd3dpp->MultiSampleType != D3DMULTISAMPLE_NONE);

    hWndLast = m_hWnd;
    m_hWnd = pd3dpp->hDeviceWindow;
    memcpy(&d3dpp, &m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    memcpy(&m_d3dpp, pd3dpp, sizeof(D3DPRESENT_PARAMETERS));

    if (!ResetDevice()) {
        // Restore the display description
        memcpy(&m_disdCurrent, &disdLast, sizeof(DISPLAYDESC));
        memcpy(&m_d3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        m_hWnd = hWndLast;
        return FALSE;
    }

#ifndef UNDER_XBOX
    if (bRestorePos) {

        // Restore the window position
        MoveWindow(m_hWnd, m_rectWnd.left - m_rectBorder.left, 
                   m_rectWnd.top - m_rectBorder.top, 
                   m_rectWnd.right - (m_rectWnd.left - m_rectBorder.left) + m_rectBorder.right, 
                   m_rectWnd.bottom - (m_rectWnd.top - m_rectBorder.top) + m_rectBorder.bottom, TRUE);
    }
#endif // !UNDER_XBOX

    if (!m_disdCurrent.bAntialias) {
        m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
    }

    // Do not include the amount of time the display took to update in game time calculations
    m_bTimeSync = TRUE;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Reset the device with the given display mode, windowed mode, auto
//     depth/stencil buffer, and multi-sample swap chain.
//
// Arguments:
//
//     PDISPLAYMODE pdm         - The display mode to use in resetting the
//                                device object (if full screen)
//
//     BOOL bWindowed           - The windowed mode to use with the device
//                                (windowed or full screen)
//
//     BOOL bDepthBuffer        - Indicates whether or not to use an automatic
//                                depth/stencil buffer with the device
//
//     BOOL bAntialias          - Indicates whether or not to create a
//                                multi-sample swap chain for use in FSAA
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::Update(PDISPLAYMODE pdm, BOOL bWindowed, BOOL bDepthBuffer, BOOL bAntialias) {

    D3DPRESENT_PARAMETERS   d3dpp;
    DISPLAYDESC             disdLast;
    BOOL                    bRestorePos;

    if (!m_bCreated) {
        return TRUE;
    }

#ifdef UNDER_XBOX
    if (bWindowed) {
        return FALSE;
    }
#endif // UNDER_XBOX

    // Check if windowed mode is available
    if (bWindowed && !m_disdCurrent.pdevd->bCanRenderWindowed) {
        return FALSE;
    }

    // If no display mode was given, use the current
    if (!pdm) {
        pdm = m_disdCurrent.pdm;
    }

    // Check if antialiasing is available
    if (bAntialias && !ANTIALIAS_SUPPORTED(m_disdCurrent.pdevd, pdm, bWindowed)) {
        return FALSE;
    }

    // If the application is going from full screen to windowed, the window
    // position must be restored
    bRestorePos = (bWindowed && !m_disdCurrent.bWindowed);

    memcpy(&disdLast, &m_disdCurrent, sizeof(DISPLAYDESC));
    m_disdCurrent.pdm = pdm;
    m_disdCurrent.bWindowed = bWindowed;
    m_disdCurrent.bDepthBuffer = bDepthBuffer;
    m_disdCurrent.bAntialias = bAntialias;

    // Initialize the presentation parameters
    memcpy(&d3dpp, &m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    if (bWindowed) {
        m_d3dpp.BackBufferFormat                  = m_disdCurrent.padpd->d3ddmWindowed.Format;
        m_d3dpp.MultiSampleType                   = m_disdCurrent.pdevd->d3dmstWindowed;
        m_d3dpp.AutoDepthStencilFormat            = m_disdCurrent.pdevd->fmtdWindowed;
    }
    else {
        m_d3dpp.BackBufferWidth                   = pdm->d3ddm.Width;
        m_d3dpp.BackBufferHeight                  = pdm->d3ddm.Height;
        m_d3dpp.BackBufferFormat                  = pdm->d3ddm.Format;
        m_d3dpp.MultiSampleType                   = pdm->d3dmst;
        m_d3dpp.AutoDepthStencilFormat            = pdm->fmtd;
        m_d3dpp.FullScreen_RefreshRateInHz        = pdm->d3ddm.RefreshRate;
        m_d3dpp.FullScreen_PresentationInterval   = m_disdCurrent.pdevd->uPresentInterval;
    }
    m_d3dpp.BackBufferCount                       = 1;
    m_d3dpp.SwapEffect                            = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.hDeviceWindow                         = m_hWnd;
    m_d3dpp.Windowed                              = bWindowed;
    m_d3dpp.EnableAutoDepthStencil                = bDepthBuffer;
    m_d3dpp.Flags                                 = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    if (!bAntialias) {
        m_d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    }

    if (!ResetDevice()) {
        // Restore the display description
        memcpy(&m_disdCurrent, &disdLast, sizeof(DISPLAYDESC));
        memcpy(&m_d3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        return FALSE;
    }

#ifndef UNDER_XBOX
    if (bRestorePos) {

        // Restore the window position
        MoveWindow(m_hWnd, m_rectWnd.left - m_rectBorder.left, 
                   m_rectWnd.top - m_rectBorder.top, 
                   m_rectWnd.right - (m_rectWnd.left - m_rectBorder.left) + m_rectBorder.right, 
                   m_rectWnd.bottom - (m_rectWnd.top - m_rectBorder.top) + m_rectBorder.bottom, TRUE);
    }
#endif // !UNDER_XBOX

    m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bAntialias);

    // Do not include the amount of time the display took to update in game time calculations
    m_bTimeSync = TRUE;

    return TRUE;
}

/*
//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Reset the device with the given display mode, windowed mode, auto
//     depth/stencil buffer, and multi-sample swap chain.
//
// Arguments:
//
//     PDISPLAYMODE pdm         - The display mode to use in resetting the
//                                device object (if full screen)
//
//     BOOL bWindowed           - The windowed mode to use with the device
//                                (windowed or full screen)
//
//     BOOL bDepthBuffer        - Indicates whether or not to use an automatic
//                                depth/stencil buffer with the device
//
//     BOOL bAntialias          - Indicates whether or not to create a
//                                multi-sample swap chain for use in FSAA
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::Update(D3DDISPLAYMODE* pd3ddm, BOOL bWindowed, BOOL bDepthBuffer, 
                                                        D3DMULTISAMPLE_TYPE mst)
{
    PDISPLAYMODE pdm;

    if (!pd3ddm) {
        return FALSE;
    }

    // Find the given display mode in the display list
    for (pdm = m_disdCurrent.pdevd->pdmList; pdm; pdm = pdm->pdmNext) {
        if (pdm->d3ddm.Width == pd3ddm->Width &&
            pdm->d3ddm.Height == pd3ddm->Height &&
            pdm->d3ddm.Format == pd3ddm->Format &&
            pdm->d3ddm.RefreshRate == pd3ddm->RefreshRate)
        {
            break;
        }
    }

    if (!pdm) {
        return FALSE;
    }

    if (mst != D3DMULTISAMPLE_NONE && mst != pdm->d3dmst) {

        HRESULT hr = m_pDirect3D->CheckDeviceMultiSampleType(m_disdCurrent.padpd->uAdapter, m_disdCurrent.pdevd->d3ddt, bWindowed ? m_disdCurrent.padpd->d3ddmWindowed.Format : pdm->d3ddm.Format, bWindowed, mst);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceMultiSampleTypes"));
            }
            return FALSE;
        }
        else {
            if (bWindowed) {
                m_disdCurrent.pdevd->d3dmstWindowed = mst;
            }
            else {
                pdm->d3dmst = mst;
            }
        }
    }

    return Update(pdm, bWindowed, bDepthBuffer, mst != D3DMULTISAMPLE_NONE);
}
*/

//******************************************************************************
//
// Method:
//
//     EnableConsoleVisibility
//
// Description:
//
//     Enable the overlay of the console in the scene.
//
// Arguments:
//
//     BOOL bEnable             - TRUE to display the console, FALSE to hide it.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::EnableConsoleVisibility(BOOL bEnable) {

    m_bShowConsole = bEnable;
}

//******************************************************************************
//
// Method:
//
//     ShowConsole
//
// Description:
//
//     Overlay the console in the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::ShowConsole() {

    if (!m_bCreated) {
        return;
    }

    if (m_bShowConsole) {
        xDisplayConsole();
    }
}

//******************************************************************************
void CDisplay::EnableFrameVerification(BOOL bEnable) {

    if (m_pClient) {
        ((CCDevice8*)m_pDevice)->EnableFrameVerification(bEnable);
    }
}

//******************************************************************************
void CDisplay::SetVerificationThreshold(float fThreshold) {

    if (m_pClient) {
        ((CCDevice8*)m_pDevice)->SetVerificationThreshold(fThreshold);
    }
}

//******************************************************************************
//
// Method:
//
//     InitSetDeviceProc
//
// Description:
//
//     Set the function used to perform all device initialization and object 
//     creation in a scene.  The function will be used in restoring
//     a scene after resetting a lost or changed device.
//
// Arguments:
//
//     SETDEVICEPROC pfnSetDevice       - Pointer to device initialization
//                                        function in the scene
//
//     LPVOID pvParam                   - Application data to supply as an
//                                        argument to the procedure callback
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::InitSetDeviceProc(SETDEVICEPROC pfnSetDevice, LPVOID pvParam) {

    m_pfnSetDevice = pfnSetDevice;
    m_pvSetDeviceParam = pvParam;
}

//******************************************************************************
//
// Method:
//
//     InitClearDeviceProc
//
// Description:
//
//     Set the function used to perform all device object clean up in a scene.
//     The function will be used in releasing all device objects in a scene
//     in preparation for resetting a lost or changed device.
//
// Arguments:
//
//     CLEARDEVICEPROC pfnClearDevice   - Pointer to device clean up function
//                                        in the scene
//
//     LPVOID pvParam                   - Application data to supply as an
//                                        argument to the procedure callback
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void CDisplay::InitClearDeviceProc(CLEARDEVICEPROC pfnClearDevice, LPVOID pvParam) {

    m_pfnClearDevice = pfnClearDevice;
    m_pvClearDeviceParam = pvParam;
}

//******************************************************************************
//
// Method:
//
//     InitSubWindowProc
//
// Description:
//
//     Set the function a scene will use to process window messages.  The
//     function will be called by the main window procedure, giving the
//     scene the first crack at handling messages.
//
// Arguments:
//
//     SUBWNDPROC pfnSubWndProc         - Pointer to the scene window procedure
//
//     LPVOID pvParam                   - Application data to supply as an
//                                        argument to the procedure callback
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::InitSubWindowProc(SUBWNDPROC pfnSubWndProc, LPVOID pvParam) {

    m_pfnSubWndProc = pfnSubWndProc;
    m_pvSubWndProcParam = pvParam;
}

//******************************************************************************
//
// Method:
//
//     MoveWindowTarget
//
// Description:
//
//     Update the stored window position so the window may be correctly 
//     repositioned after a switch to full screen mode and back.
//
// Arguments:
//
//     int x                            - x coordinate of the upper left corner
//                                        of the window
//
//     int y                            - y coordinate of the upper left corner
//                                        of the window
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::MoveWindowTarget(int x, int y) {

#ifndef UNDER_XBOX
    if (m_disdCurrent.bWindowed && m_bCreated && m_pDevice) {
        SetRect(&m_rectWnd, x, y, x + m_d3dsdBack.Width, y + m_d3dsdBack.Height);
    }

    return TRUE;
#else
    return FALSE;
#endif // UNDER_XBOX
}

//******************************************************************************
//
// Method:
//
//     SizeWindowTarget
//
// Description:
//
//     Update the display dimensions corresponding to a change in window size.
//
// Arguments:
//
//     int width                        - Width of the window
//
//     int height                       - Height of the window
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::SizeWindowTarget(int width, int height) {

    if (m_disdCurrent.bWindowed && m_bCreated && m_pDevice) {
        m_rectWnd.right = m_rectWnd.left + width;
        m_rectWnd.bottom = m_rectWnd.top + height;
        return Update(m_disdCurrent.pdm, TRUE, m_disdCurrent.bDepthBuffer, 
                             m_disdCurrent.bAntialias);
    }

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     EnableInput
//
// Description:
//
//     Enable or disable the use of joystick input.
//
// Arguments:
//
//     BOOL bEnable            - TRUE to enable input, FALSE to disable it
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::EnableInput(BOOL bEnable) {

    m_bUseInput = bEnable;
    if (!bEnable) {
        memset(&m_jsJoys, 0, sizeof(JOYSTATE));
    }
}

//******************************************************************************
//
// Method:
//
//     EnableRMC
//
// Description:
//
//     Enable or disable remote Direct3D method calls.
//
// Arguments:
//
//     BOOL bEnable            - TRUE to enable RMCs, FALSE to disable them
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::EnableRMC(BOOL bEnable) {

    if (m_pClient) {
        m_pClient->EnableMethodCalls(bEnable);
    }
}

//******************************************************************************
//
// Method:
//
//     RMCEnabled
//
// Description:
//
//     Return the enabled state of remote Direct3D method calls.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if remote method calls are enabled, FALSE if they are disabled.
//
//******************************************************************************
BOOL CDisplay::RMCEnabled() {

    if (m_pClient) {
        return m_pClient->MethodCallsEnabled();
    }

    return FALSE;
}

//******************************************************************************
//
// Method:
//
//     GetConnectionStatus
//
// Description:
//
//     Return the connection status of the verification server.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Connection status of the server.
//
//******************************************************************************
SERVERSTATUS CDisplay::GetConnectionStatus() {

    if (m_pClient) {
        if (m_pClient->IsConnected()) {
            return SERVER_CONNECTED;
        }
        else {
            return SERVER_DISCONNECTED;
        }
    }
    else {
        return SERVER_NOTCONNECTED;
    }
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input from the keyboard or joystick.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::ProcessInput() {

    if (!m_bCreated) {
        return;
    }

    memcpy(&m_jsLast, &m_jsJoys, sizeof(JOYSTATE));

    // Poll the joystick
    if (m_bUseInput) {
        GetJoystickState(&m_jsJoys);
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BLACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BLACK)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                if (m_pClient) {
                    ((CCDevice8*)m_pDevice)->ToggleVerificationDisplay();
                }
            }
            else {
                if (!(GetStartupContext() & TSTART_STRESS)) {
                    m_bShowConsole = !m_bShowConsole;
                }
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_WHITE)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_WHITE) && BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
            m_bSwapSticks = !m_bSwapSticks;
        }
    }

#ifndef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            SendMessage(m_hWnd, WM_CLOSE, 0, 0);
        }
    }
#endif // !UNDER_XBOX

    if (!m_bShowConsole) {
        // Update the default camera
        UpdateCamera(&m_cam);
    }

    else {

        DWORD dwScroll = 0;
        float fHorz, fVert;

        // Update the console
        if (m_kdKeys & KEY_LEFT) {
            dwScroll |= XSCROLL_LINELEFT;
        }
        if (m_kdKeys & KEY_RIGHT) {
            dwScroll |= XSCROLL_LINERIGHT;
        }
        if (m_kdKeys & KEY_UP) {
            dwScroll |= XSCROLL_LINEUP;
        }
        if (m_kdKeys & KEY_DOWN) {
            dwScroll |= XSCROLL_LINEDOWN;
        }
        if (m_kdKeys & KEY_PAGEDOWN || BUTTON_PRESSED(m_jsJoys, JOYBUTTON_DOWN)) {
            dwScroll |= XSCROLL_PAGEDOWN;
        }
        if (m_kdKeys & KEY_PAGEUP || BUTTON_PRESSED(m_jsJoys, JOYBUTTON_UP)) {
            dwScroll |= XSCROLL_PAGEUP;
        }
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LEFT)) {
            dwScroll |= XSCROLL_PAGELEFT;
        }
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RIGHT)) {
            dwScroll |= XSCROLL_PAGERIGHT;
        }
        if (m_kdKeys & KEY_HOME) {
            dwScroll |= XSCROLL_LEFT;
        }
        if (m_kdKeys & KEY_END) {
            dwScroll |= XSCROLL_RIGHT;
        }
        if (m_kdKeys & KEY_INSERT || BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {
            dwScroll |= XSCROLL_TOP;
        }
        if (m_kdKeys & KEY_DELETE || BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            dwScroll |= XSCROLL_BOTTOM;
        }
        fHorz = m_jsJoys.f1X;
        fVert = m_jsJoys.f1Y;
        if (fHorz != 0.0f) {
            dwScroll |= XSCROLL_HORZ;
        }
        if (fVert != 0.0f) {
            dwScroll |= XSCROLL_VERT;
        }
        if (dwScroll) {
            xScrollConsole(dwScroll, &fHorz, &fVert);
        }
    }
}

//******************************************************************************
//
// Method:
//
//     SetView
//
// Description:
//
//     Update the view and projection matrices based on the given camera data.
//
// Arguments:
//
//     PCAMERA pcam                 - Pointer to the camera information
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::SetView(PCAMERA pcam) {

    D3DXMATRIX      mTransform;
    HRESULT         hr;

    if (!m_bCreated) {
        return FALSE;
    }

    if (!pcam) {
        pcam = &m_cam;
    }

    // Initialize the view matrix
    ::SetView(&mTransform, &pcam->vPosition, &pcam->vInterest, 
            &D3DXVECTOR3((float)sin(pcam->fRoll), (float)cos(pcam->fRoll), 
            0.0f));

    if (GetStartupContext() & TSTART_STRESS) {
        float* fElement = &mTransform._11;
        UINT i;
        for (i = 0; i < 16; i++) {
            if (_isnan(fElement[i])) {
                TCHAR szOut[256];
                wsprintf(szOut, TEXT("Element %d of view transform is not a number (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                OutputDebugString(szOut);
                for (i = 0; i < 16; i++) {
                    wsprintf(szOut, TEXT("View transform element %d - (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                    OutputDebugString(szOut);
                }
                __asm int 3;
                break;
            }
        }
    }

    hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    // Initialize the projection matrix
    SetPerspectiveProjection(&mTransform, pcam->fNearPlane, pcam->fFarPlane, 
            pcam->fFieldOfView, (float)m_d3dsdBack.Height / (float)m_d3dsdBack.Width);

    hr = m_pDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTransform"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     UpdateCamera
//
// Description:
//
//     Update the camera state by polling user input from the keyboard and 
//     and joystick.
//
// Arguments:
//
//     PCAMERA pcam                 - Pointer to the camera information
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void CDisplay::UpdateCamera(PCAMERA pcam) {

    static KEYSDOWN     kdLastKeys = 0;
    static D3DXVECTOR3  vViewTranslation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    static D3DXVECTOR3  vViewRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    static D3DXMATRIX   mViewRotationX = D3DXMATRIX(
                            1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f
                        );
    static D3DXMATRIX   mViewRotationY = mViewRotationX;
    static BOOL         bOrbitStick = FALSE;

    if (!m_bCreated) {
        return;
    }

    // Poll the keyboard
    if (m_kdKeys != kdLastKeys) {

        if (m_kdKeys & KEY_PAGEDOWN) {
            vViewTranslation.x = -m_fCameraTDelta;
        }
        else if (m_kdKeys & KEY_DELETE) {
            vViewTranslation.x = m_fCameraTDelta;
        }
        else {
            vViewTranslation.x = 0.0f;
        }

        if (m_kdKeys & KEY_HOME) {
            vViewTranslation.y = -m_fCameraTDelta;
        }
        else if (m_kdKeys & KEY_END) {
            vViewTranslation.y = m_fCameraTDelta;
        }
        else {
            vViewTranslation.y = 0.0f;
        }

        if (m_kdKeys & KEY_ADD || m_kdKeys & KEY_PAGEUP) {
            vViewTranslation.z = -m_fCameraTDelta;
        }
        else if (m_kdKeys & KEY_SUBTRACT || m_kdKeys & KEY_INSERT) {
            vViewTranslation.z = m_fCameraTDelta;
        }
        else {
            vViewTranslation.z = 0.0f;
        }

        if (m_kdKeys & KEY_LEFT) {
            vViewRotation.y = -m_fCameraRDelta;
        }
        else if (m_kdKeys & KEY_RIGHT) {
            vViewRotation.y = m_fCameraRDelta;
        }
        else {
            vViewRotation.y = 0.0f;
        }

        if (m_kdKeys & KEY_UP) {
            vViewRotation.x = -m_fCameraRDelta;
        }
        else if (m_kdKeys & KEY_DOWN) {
            vViewRotation.x = m_fCameraRDelta;
        }
        else {
            vViewRotation.x = 0.0f;
        }
    }

    kdLastKeys = m_kdKeys;

    // Only update the camera when the start button is not pressed
    if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {

        // Left keypad
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LEFT)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LEFT)) {
                vViewTranslation.x = m_fCameraTDelta;
            }
            else {
                vViewTranslation.x = 0.0f;
            }
        }

        // Right keypad
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RIGHT)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RIGHT)) {
                vViewTranslation.x = -m_fCameraTDelta;
            }
            else {
                vViewTranslation.x = 0.0f;
            }
        }

        // Up keypad
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_UP)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_UP)) {
                vViewTranslation.y = -m_fCameraTDelta;
            }
            else {
                vViewTranslation.y = 0.0f;
            }
        }

        // Down keypad
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_DOWN)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_DOWN)) {
                vViewTranslation.y = m_fCameraTDelta;
            }
            else {
                vViewTranslation.y = 0.0f;
            }
        }

        // Left trigger
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LTRIG)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LTRIG)) {
                vViewTranslation.z = m_fCameraTDelta;
            }
            else {
                vViewTranslation.z = 0.0f;
            }
        }

        // Right trigger
        if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RTRIG)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RTRIG)) {
                vViewTranslation.z = -m_fCameraTDelta;
            }
            else {
                vViewTranslation.z = 0.0f;
            }
        }

        // Joysticks
        if (m_jsJoys.f1X != m_jsLast.f1X ||
            m_jsJoys.f1Y != m_jsLast.f1Y ||
            m_jsJoys.f2X != m_jsLast.f2X ||
            m_jsJoys.f2Y != m_jsLast.f2Y)
        {
            float f1X, f1Y, f2X, f2Y;

            if (m_bSwapSticks) {
                f1X = m_jsJoys.f2X;
                f1Y = m_jsJoys.f2Y;
                f2X = m_jsJoys.f1X;
                f2Y = m_jsJoys.f1Y;
            }
            else {
                f1X = m_jsJoys.f1X;
                f1Y = m_jsJoys.f1Y;
                f2X = m_jsJoys.f2X;
                f2Y = m_jsJoys.f2Y;
            }
            if (f2X == 0.0f && f2Y == 0.0f) {
                vViewRotation.y = f1X * m_fCameraRDelta;
                vViewRotation.x = f1Y * m_fCameraRDelta;
                bOrbitStick = FALSE;
            }
            else {
                vViewRotation.y = f2X * m_fCameraRDelta;
                vViewRotation.x = f2Y * m_fCameraRDelta;
                bOrbitStick = TRUE;
            }
        }
    }

    if (pcam) {

        D3DXMATRIX  mView, mViewTransform;
        D3DXVECTOR3 vTranslation, vDirection, vUp, vProj, vCross;
        D3DXVECTOR3 vViewTrans = vViewTranslation;
        float       fMagnitude, fDot, fScaleX, fScaleY, fTheta;
        BOOL        bOrbit = (BOOL)((m_kdKeys & KEY_SHIFT) || bOrbitStick);


        if (GetStartupContext() & TSTART_STRESS) {
            float* fElement = (float*)pcam;
            UINT i;
            for (i = 0; i < 7; i++) {
                if (_isnan(fElement[i])) {
                    TCHAR szOut[256];
                    wsprintf(szOut, TEXT("Element %d of camera is not a number before update (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                    OutputDebugString(szOut);
                    for (i = 0; i < 7; i++) {
                        wsprintf(szOut, TEXT("Camera element %d - (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                        OutputDebugString(szOut);
                    }
                    __asm int 3;
                    break;
                }
            }
        }


        if (!bOrbit) {
            vDirection = pcam->vInterest - pcam->vPosition;
        }
        else {
            vDirection = pcam->vPosition - pcam->vInterest;
        }
        fMagnitude = D3DXVec3Length(&vDirection);
        D3DXVec3Normalize(&vDirection, &vDirection);
        vUp = D3DXVECTOR3((float)sin(pcam->fRoll), (float)cos(pcam->fRoll), 0.0f);
        // Project the direction vector into a plane orthogonal to the up vector.
        // The resulting vector will originate at the point the projected direction
        // vector intersects with the plane and terminate at the up vector/
        // The DotProduct gives the ratio between the magnitude of the direction
        // vector and the scalar projection of the direction vector onto the up
        // vector.  Multiplying by this scales the direction vector out to the
        // point that it will intersect with the up vector's orthogonal plane
        fDot = D3DXVec3Dot(&vUp, &vDirection);
        D3DXVec3Normalize(&vProj, &(vUp - (vDirection * fDot)));
        D3DXVec3Cross(&vCross, &vProj, &vDirection);

        fScaleY = (fDot < 0.0f) ? -fDot : fDot;
        fTheta = (float)acos(fScaleY);
        fScaleY = fTheta / M_PIDIV2;

        if (bOrbit) {
            fScaleY *= -3.0f;
            fScaleX = 3.0f;
        }
        else {
            fScaleX = 1.0f;
        }

        mViewRotationY._11 = (float)cos(vViewRotation.y * fScaleY);
        mViewRotationY._13 = (float)-sin(vViewRotation.y * fScaleY);
        mViewRotationY._31 = -mViewRotationY._13;
        mViewRotationY._33 = mViewRotationY._11;

        mViewRotationX._22 = (float)cos(vViewRotation.x * fScaleX);
        mViewRotationX._23 = (float)sin(vViewRotation.x * fScaleX);
        mViewRotationX._32 = -mViewRotationX._23;
        mViewRotationX._33 = mViewRotationX._22;

        InitMatrix(&mViewTransform,
            vCross.x,       vCross.y,       vCross.z,       0.0f,
            vProj.x,        vProj.y,        vProj.z,        0.0f,
            vDirection.x,   vDirection.y,   vDirection.z,   0.0f,
            0.0f,           0.0f,           0.0f,           1.0f
        );

        D3DXMatrixMultiply(&mView, &mViewRotationX, &mViewTransform);
        D3DXMatrixMultiply(&mViewTransform, &mViewRotationY, &mView);

        if (!bOrbit) {
            D3DXVec3TransformCoord(&pcam->vInterest, 
                                &D3DXVECTOR3(0.0f, 0.0f, fMagnitude), &mViewTransform);
            pcam->vInterest += pcam->vPosition;
        }
        else {
            D3DXVec3TransformCoord(&pcam->vPosition, 
                                &D3DXVECTOR3(0.0f, 0.0f, fMagnitude), &mViewTransform);
            pcam->vPosition += pcam->vInterest;
            vViewTrans.x = -vViewTrans.x;
            vViewTrans.z = -vViewTrans.z;
        }
        D3DXVec3TransformCoord(&vTranslation, &vViewTrans, &mViewTransform);
        pcam->vPosition -= vTranslation;
        pcam->vInterest -= vTranslation;

        if (fDot != 0.0f) {
            float fPol = (fDot < 0.0f) ? 1.0f : -1.0f;
            if ((fPol * vViewRotation.x * fScaleX) > fTheta) {
                pcam->fRoll += M_PI;
                if (pcam->fRoll > M_PI) {
                    pcam->fRoll -= M_2PI;
                }
            }
        }


        if (GetStartupContext() & TSTART_STRESS) {
            float* fElement = (float*)pcam;
            UINT i;
            for (i = 0; i < 7; i++) {
                if (_isnan(fElement[i])) {
                    TCHAR szOut[256];
                    wsprintf(szOut, TEXT("Element %d of camera is not a number after update (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                    OutputDebugString(szOut);
                    for (i = 0; i < 7; i++) {
                        wsprintf(szOut, TEXT("Camera element %d - (0x%X : %f)\r\n"), i, *(LPDWORD)&fElement[i], fElement[i]);
                        OutputDebugString(szOut);
                    }
                    __asm int 3;
                    break;
                }
            }
        }

    }
}

//******************************************************************************
// Menu functions
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
//
// Method:
//
//     CreateMainMenu
//
// Description:
//
//     Create the main menu for the application window
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Handle to the created menu.
//
//******************************************************************************
HMENU CDisplay::CreateMainMenu() {

    HMENU hMenu, hMenuFile;

    // If the application has supplied a menu, load and return it
    hMenu = LoadMenu(GetTestInstance(), TEXT("IDR_MENU"));
    if (hMenu) {
        return hMenu;
    }

    // Otherwise create a default menu
    hMenu = CreateMenu();

    hMenuFile = CreateMenu();

    AppendMenu(hMenuFile, MF_STRING, IDM_DEFAULT_FILE_DISPLAY, TEXT("&Display...\tF2"));
    AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenuFile, MF_STRING, IDM_DEFAULT_FILE_EXIT, TEXT("E&xit\tEsc"));

    AppendMenu(hMenu, MF_POPUP, (UINT)hMenuFile, TEXT("&File"));

    return hMenu;
}

//******************************************************************************
//
// Method:
//
//     DisplayDlgProc
//
// Description:
//
//     Window procedure to process messages for the display dialog.
//
// Arguments:
//
//     HWND hDlg                - Dialog window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CDisplay::DisplayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    PADAPTERDESC        padpd;
    PDEVICEDESC         pdevd;
    PDISPLAYMODE        pdm;
    TCHAR               szMode[512];
    int                 sel;
#ifdef UNICODE
    WCHAR               wszBuffer[512];
#endif // UNICODE
    TCHAR*              pszDevice[] = {
                            TEXT(""),
                            TEXT("NULL"),
                            TEXT("Abstraction (HAL)"),
                            TEXT("Reference (REF)"),
                            TEXT("Emulation (SW)"),
                        };

    switch (uMsg) {

        case WM_INITDIALOG:

            // Fill the adapter list
            for (padpd = m_padpdList; padpd; padpd = padpd->padpdNext) {
#ifndef UNICODE
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_ADDSTRING, 0, (LPARAM)padpd->d3dai.Description);
#else
                mbstowcs(wszBuffer, padpd->d3dai.Description, 512);
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_ADDSTRING, 0, (LPARAM)wszBuffer);
#endif // !UNICODE
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)padpd);
                if (m_disdSelect.padpd == padpd) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            // Fill the device list
            for (pdevd = m_disdSelect.padpd->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_ADDSTRING, 0, (LPARAM)pszDevice[pdevd->d3ddt]);
                SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdevd);
                if (m_disdSelect.pdevd == pdevd) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            // Fill the display mode
            for (pdm = m_disdSelect.pdevd->pdmList; pdm; pdm = pdm->pdmNext) {
                if (pdm->d3ddm.RefreshRate) {
                    wsprintf(szMode, TEXT("%d x %d x %d  %d Hz"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format), pdm->d3ddm.RefreshRate);
                }
                else {
                    wsprintf(szMode, TEXT("%d x %d x %d"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format));
                }
                sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_ADDSTRING, 0, (LPARAM)szMode);
                SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdm);
                if (pdm == m_disdSelect.pdm) {
                    SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)sel, 0);
                }
            }

            SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_SETCHECK, (BOOL)!m_disdSelect.bWindowed, 0);
            EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_FULLSCREEN), m_disdSelect.pdevd->bCanRenderWindowed);
            EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), !m_disdSelect.bWindowed);

            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEPTHBUFFER, BM_SETCHECK, m_disdSelect.bDepthBuffer, 0);
            if (ANTIALIAS_SUPPORTED(m_disdSelect.pdevd, m_disdSelect.pdm, m_disdSelect.bWindowed)) {
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, m_disdSelect.bAntialias, 0);
            }
            else {
                SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), FALSE);
            }

            return TRUE;

        case WM_COMMAND:

            switch (LOWORD(wParam)) {

                case IDC_DISPLAY_FULLSCREEN:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        PDEVICEDESC pdevd;
                        PDISPLAYMODE pdm;
                        BOOL bAntialias;
                        BOOL bFullscreen = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), bFullscreen);

                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        bAntialias = ANTIALIAS_SUPPORTED(pdevd, pdm, !bFullscreen);
                        if (!bAntialias) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), bAntialias);
                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_ADAPTER:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {

                        PADAPTERDESC padpdCurrent;
                        PDEVICEDESC  pdevdDefault, pdevdLast;
                        int          def;

                        // Get the current device selection
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevdLast = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Clear the device list
                        SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_RESETCONTENT, 0, 0);

                        // Get the new adapter
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETCURSEL, 0, 0);
                        padpdCurrent = (PADAPTERDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Select a default device for the adapter
                        pdevdDefault = SelectDevice(padpdCurrent);

                        // Add the devices for the new adapter
                        for (pdevd = padpdCurrent->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {

                            sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_ADDSTRING, 0, (LPARAM)pszDevice[pdevd->d3ddt]);
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdevd);

                            // If the new adapter supports the same device that was selected for the old
                            // adapter, select the device again
                            if (pdevd->d3ddt == pdevdLast->d3ddt) {
                                SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)sel, 0);
                            }

                            // If the device matches the default device, save the index
                            if (pdevd == pdevdDefault) {
                                def = sel;
                            }
                        }

                        // If no device has been selected (the new adapter does not support the old selected device)
                        // select the default as current
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        if (sel == CB_ERR) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_SETCURSEL, (WPARAM)def, 0);
                        }

                        // Update the display mode list for the new device
                        SendMessage(hDlg, WM_COMMAND, MAKELPARAM(IDC_DISPLAY_DEVICE, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_DISPLAY_DEVICE));

                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_DEVICE:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {

                        PDEVICEDESC  pdevdCurrent;
                        PDISPLAYMODE pdmDefault, pdmLast;
                        int          def;
                        BOOL         bEnable;

                        // Get the current display mode selection
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdmLast = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Clear the display mode list
                        SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_RESETCONTENT, 0, 0);

                        // Get the new device
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevdCurrent = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);

                        // Select a default display mode for the device
                        pdmDefault = SelectDisplayMode(pdevdCurrent);

                        // Add the display modes for the new device
                        for (pdm = pdevdCurrent->pdmList; pdm; pdm = pdm->pdmNext) {

                            if (pdm->d3ddm.RefreshRate) {
                                wsprintf(szMode, TEXT("%d x %d x %d  %d Hz"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format), pdm->d3ddm.RefreshRate);
                            }
                            else {
                                wsprintf(szMode, TEXT("%d x %d x %d"), pdm->d3ddm.Width, pdm->d3ddm.Height, FormatToBitDepth(pdm->d3ddm.Format));
                            }
                            sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_ADDSTRING, 0, (LPARAM)szMode);
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETITEMDATA, (WPARAM)sel, (LPARAM)pdm);

                            // If the new device supports the same display mode that was selected for the old
                            // device, select the mode again
                            if (pdm->d3ddm.Width == pdmLast->d3ddm.Width &&
                                pdm->d3ddm.Height == pdmLast->d3ddm.Height &&
                                pdm->d3ddm.Format == pdmLast->d3ddm.Format &&
                                pdm->d3ddm.RefreshRate == pdmLast->d3ddm.RefreshRate) 
                            {
                                SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)sel, 0);
                            }

                            // If the display mode matches the default, save the index
                            if (pdm == pdmDefault) {
                                def = sel;
                            }
                        }

                        // If no mode has been selected (the new device does not support the old selected mode)
                        // select the default as current
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        if (sel == CB_ERR) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_SETCURSEL, (WPARAM)def, 0);
                        }

                        // If the device does not supported rendering in a windowed state, select and
                        // disable the full screen check box
                        if (!pdevdCurrent->bCanRenderWindowed) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_SETCHECK, (WPARAM)TRUE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_FULLSCREEN), pdevdCurrent->bCanRenderWindowed);
                        bEnable = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_MODE), bEnable);

                        // Update the antialias check box
                        SendMessage(hDlg, WM_COMMAND, MAKELPARAM(IDC_DISPLAY_MODE, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_DISPLAY_MODE));

                        return TRUE;
                    }
                    break;

                case IDC_DISPLAY_MODE:

                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        PDEVICEDESC pdevd;
                        PDISPLAYMODE pdm;
                        BOOL bAntialias;
                        BOOL bFullscreen = SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                        pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                        pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                        bAntialias = ANTIALIAS_SUPPORTED(pdevd, pdm, !bFullscreen);
                        if (!bAntialias) {
                            SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_SETCHECK, FALSE, 0);
                        }
                        EnableWindow(GetDlgItem(hDlg, IDC_DISPLAY_ANTIALIAS), bAntialias);
                        return TRUE;
                    }
                    break;

                case IDOK:
                    // Update the current driver, device, window state, and display mode
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETCURSEL, 0, 0);
                    m_disdSelect.padpd = (PADAPTERDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_ADAPTER, CB_GETITEMDATA, (WPARAM)sel, 0);
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETCURSEL, 0, 0);
                    m_disdSelect.pdevd = (PDEVICEDESC)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEVICE, CB_GETITEMDATA, (WPARAM)sel, 0);
                    sel = SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETCURSEL, 0, 0);
                    m_disdSelect.pdm = (PDISPLAYMODE)SendDlgItemMessage(hDlg, IDC_DISPLAY_MODE, CB_GETITEMDATA, (WPARAM)sel, 0);
                    m_disdSelect.bWindowed = !((BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_FULLSCREEN, BM_GETCHECK, 0, 0));
                    m_disdSelect.bDepthBuffer = (BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_DEPTHBUFFER, BM_GETCHECK, 0, 0);
                    m_disdSelect.bAntialias = (BOOL)SendDlgItemMessage(hDlg, IDC_DISPLAY_ANTIALIAS, BM_GETCHECK, 0, 0);

                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    return TRUE;
            }

            break;
    }

    return FALSE;
}

//******************************************************************************
//
// Method:
//
//     AddDlgItemTemplate
//
// Description:
//
//     Reset the device with the given display mode, windowed mode, auto
//     depth/stencil buffer, and multi-sample swap chain.
//
// Arguments:
//
//     LPWORD* ppw                      - Pointer to an item in the dialog
//                                        template
//
//     WORD wClass                      - Class ordinal
//
//     LPWSTR wszText                   - Text of the dialog item
//
//     WORD wID                         - Dialog item identifier
//
//     short x                          - x coordinate of the dialog item
//
//     short y                          - y coordinate of the dialog item
//
//     short cx                         - Width of the dialog item
//
//     short cy                         - Height of the dialog item
//
//     DWORD dwStyle                    - Style of the dialog item
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::AddDlgItemTemplate(LPWORD* ppw, WORD wClass, LPWSTR wszText, 
            WORD wId, short x, short y, short cx, short cy, DWORD dwStyle) 
{
    LPDLGITEMTEMPLATE pdit;
    LPWSTR            wsz;
    LPWORD            pw = *ppw;

    pdit = (LPDLGITEMTEMPLATE)((((DWORD)pw) + 3) & ~3);    // Align the data
    pdit->style           = WS_VISIBLE | WS_CHILD | dwStyle;
    pdit->dwExtendedStyle = 0;
    pdit->x               = x;
    pdit->y               = y;
    pdit->cx              = cx;
    pdit->cy              = cy;
    pdit->id              = wId;

    pw = (LPWORD)(pdit+1);
    *pw++ = 0xFFFF;                         // System class
    *pw++ = wClass;                         // Class ordinal
    wsz = (LPWSTR)pw;
    wcscpy(wsz, wszText);                   // Item text
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);
    *pw++ = 0;                              // No creation data
    *ppw = pw;
}

#endif // !UNDER_XBOX

//******************************************************************************
//
// Method:
//
//     Select
//
// Description:
//
//     Build and create a display dialog to allow a selection of adapter, 
//     device, display mode, windowed mode, auto depth buffer, and multi-sample
//     type.  Reset or recreate the device based on the display configuration
//     selected.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CDisplay::Select() {

#ifdef UNDER_XBOX
    return;
#else

    LPDLGTEMPLATE     pTemplate;
    LPWORD            pw;
    LPWSTR            wsz;
    int               ret;

    if (!m_bCreated) {
        return;
    }

    // Save the current display state
    memcpy(&m_disdSelect, &m_disdCurrent, sizeof(DISPLAYDESC));

    pTemplate = (LPDLGTEMPLATE)MemAlloc(sizeof(DLGTEMPLATE) + 3 * sizeof(WORD) + 
                                        512 * sizeof(WCHAR) + 
                                        11 * (sizeof(DLGITEMTEMPLATE) +
                                        (5 * sizeof(WORD) + 256 * sizeof(WCHAR))));
    if (!pTemplate) {
        return;
    }

    // Initialize the dialog template
    pTemplate->style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER | DS_SETFONT;
    pTemplate->cdit  = 11;
    pTemplate->x     = 0;
    pTemplate->y     = 0;
    pTemplate->cx    = 149;
    pTemplate->cy    = 138;
    pTemplate->dwExtendedStyle = 0;

    pw = (LPWORD)(pTemplate+1);
    *pw++ = 0;                                  // No menu
    *pw++ = 0;                                  // Default dialog class
    
    // Specify the caption for the dialog
    wsz = (LPWSTR)pw;
    wcscpy(wsz, L"Display");                    // Caption
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);
    *pw++ = 8;                                  // 8 point font
    wsz = (LPWSTR)pw;
    wcscpy(wsz, L"MS Sans Serif");              // MS Sans Serif typeface
    pw = (LPWORD)(wsz + wcslen(wsz) + 1);

    // Specify the controls of the dialog
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_ADAPTER, 13, 16, 123, 50, CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Adapter", IDC_STATIC, 7, 7, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_DEVICE, 13, 48, 123, 50, CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Device", IDC_STATIC, 7, 38, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0080, L"", IDC_STATIC, 7, 70, 135, 27, WS_GROUP | BS_GROUPBOX);
    AddDlgItemTemplate(&pw, 0x0080, L"F&ull screen", IDC_DISPLAY_FULLSCREEN, 13, 68, 50, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0085, L"", IDC_DISPLAY_MODE, 13, 79, 123, 100, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"&Depth Buffer", IDC_DISPLAY_DEPTHBUFFER, 13, 102, 56, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"&Antialias", IDC_DISPLAY_ANTIALIAS, 83, 102, 42, 10, BS_AUTOCHECKBOX | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"OK", IDOK, 38, 117, 50, 14, BS_DEFPUSHBUTTON | WS_TABSTOP);
    AddDlgItemTemplate(&pw, 0x0080, L"Cancel", IDCANCEL, 92, 117, 50, 14, WS_TABSTOP);

    // Display the dialog in a full screen state
    if (m_pDevice && !m_disdCurrent.bWindowed) {
//        m_pDevice->FlipToGDISurface();
        DrawMenuBar(m_hWnd);
        RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME);
    }

    // Create the display dialog
    ret = DialogBoxIndirectParam(GetTestInstance(), pTemplate, m_hWnd, ::DisplayDlgProc, (LPARAM)this);

    MemFree(pTemplate);

    // Do not include the amount of time the dialog was open in game time calculations
    m_bTimeSync = TRUE;

    if (ret != IDOK) {
        return;
    }

    // If the display driver or device has been changed, recreate everything
    if (m_disdCurrent.padpd != m_disdSelect.padpd || m_disdCurrent.pdevd != m_disdSelect.pdevd) {
        if (m_pfnClearDevice) {
            m_pfnClearDevice(m_pvClearDeviceParam);
        }
        xReleaseConsole();
        m_pDevice->Release();
        m_pDevice = NULL;
        if (!CreateDevice(m_disdSelect.padpd, m_disdSelect.pdevd, m_disdSelect.pdm, m_disdCurrent.uNumBackBuffers, m_disdSelect.bWindowed, m_disdSelect.bDepthBuffer, m_disdSelect.bAntialias) ||
            !xCreateConsole(m_pDevice) ||
            (m_pfnSetDevice && !m_pfnSetDevice(m_pvSetDeviceParam))) 
        {
            DestroyWindow(m_hWnd);
        }
    }

    // If the state of the display has changed, update the display
    else if (m_disdCurrent.pdm != m_disdSelect.pdm ||
             m_disdCurrent.bWindowed != m_disdSelect.bWindowed ||
             m_disdCurrent.bDepthBuffer != m_disdSelect.bDepthBuffer ||
             m_disdCurrent.bAntialias != m_disdSelect.bAntialias) 
    {
        Update(m_disdSelect.pdm, m_disdSelect.bWindowed, m_disdSelect.bDepthBuffer, m_disdSelect.bAntialias);
    }

#endif // !UNDER_XBOX
}

