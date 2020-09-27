/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    depthbuf.cpp

Author:

    Matt Bronder

Description:

    Depth buffer test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "depthbuf.h"

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CDepthBufferTest*   pDepthTest;
    BOOL                bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pDepthTest = new CDepthBufferTest();
    if (!pDepthTest) {
        return FALSE;
    }

    // Initialize the scene
    if (!pDepthTest->Create(pDisplay)) {
        pDepthTest->Release();
        return FALSE;
    }

    bRet = pDepthTest->Exhibit(pnExitCode);

    // Clean up the scene
    pDepthTest->Release();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CDepthBufferTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDepthBufferTest
//
// Description:
//
//     Initialize the test.
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
CDepthBufferTest::CDepthBufferTest() {

    m_fAngle = 0.0f;
    m_fmtd = D3DFMT_LIN_D24S8;
    m_bFloatDepth = FALSE;
    m_bD16 = FALSE;
    m_d3dzbt = D3DZB_TRUE;
    m_d3dpp.AutoDepthStencilFormat = m_fmtd;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CDepthBufferTest
//
// Description:
//
//     Clean up the test.
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
CDepthBufferTest::~CDepthBufferTest() {
}

//******************************************************************************
//
// Method:
//
//     Prepare
//
// Description:
//
//     Initialize all device-independent data to be used in the scene.  This
//     method is called only once at creation (as opposed to Setup and
//     Initialize, which get called each time the device is Reset).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared, FALSE if it was not.
//
//******************************************************************************
BOOL CDepthBufferTest::Prepare() {

    float u, x, z;
    UINT  i;

    // Set the duration
    if (GetStartupContext() & TSTART_HARNESS) {
        SetTimeDuration(8.0f * M_2PI);
    }

    // Set up the vertices for the blue plane
    m_prBluePlane[0] = LVERTEX(D3DXVECTOR3(-0.8f,-0.8f, 1.0f ), 0xFF0000FF, 0, 0.0f, 1.0f);
    m_prBluePlane[1] = LVERTEX(D3DXVECTOR3(-0.8f, 0.8f, 1.0f ), 0xFF0000FF, 0, 0.0f, 0.0f);
    m_prBluePlane[2] = LVERTEX(D3DXVECTOR3( 0.8f, 0.8f, 1.0f ), 0xFF0000FF, 0, 1.0f, 1.0f);
    m_prBluePlane[3] = LVERTEX(D3DXVECTOR3( 0.8f,-0.8f, 1.0f ), 0xFF0000FF, 0, 1.0f, 0.0f);

    // Set up the vertices for the waffle object
    for (i = 0; i < NUM_WAFFLE_SLICES; i++) {

        u = ((float)(i)) / (NUM_WAFFLE_SLICES - 1);
        x = 2 * u - 1.0f;
        z = 0.8995f;
        if (i % 4 == 0) {
            z = 0.8f;
        }
        if (i % 4 == 2) {
            z = 0.999f;
        }

        m_prGreenWaffle[2*i]   = LVERTEX(D3DXVECTOR3(x, -1.0f, z), 0xFF00FF00, 0x0, u, 1.0f);
        m_prGreenWaffle[2*i+1] = LVERTEX(D3DXVECTOR3(x,  1.0f, z), 0xFF00FF00, 0x0, u, 0.0f);
    }

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CDepthBufferTest::Initialize() {

    m_pDisplay->SetView(&m_camInitial);

    return CScene::Initialize();
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
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
void CDepthBufferTest::Efface() {

    CScene::Efface();
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
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
void CDepthBufferTest::Update() {

    D3DXMATRIX mWorld;
    TCHAR      szDType[64];
    TCHAR      szDFmt[64];

    m_fAngle += m_fTimeDelta;

    if (GetStartupContext() & TSTART_HARNESS) {
        if (m_fAngle > M_2PI) {
            if (m_d3dzbt == D3DZB_USEW) {
                if (m_fTime > 4.0f * M_2PI && m_fTime < 6.0f * M_2PI) {
                    UpdateBufferFormat(!m_bD16, !m_bFloatDepth);
                }
                else {
                    UpdateBufferFormat(m_bD16, !m_bFloatDepth);
                }
                m_d3dzbt = D3DZB_TRUE;
            }
            else {
                m_d3dzbt = D3DZB_USEW;
            }
            m_fAngle -= M_2PI;
        }
    }

    D3DXMatrixRotationZ(&mWorld, (M_PIDIV2) * ( 1.0f - (float)cos(m_fAngle)));

    mWorld._43 = 50.0f - 40.0f * (float)cos(m_fAngle);

    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    switch (m_d3dzbt) {
        case D3DZB_FALSE:
            _tcscpy(szDType, TEXT("None"));
            break;
        case D3DZB_TRUE:
            _tcscpy(szDType, TEXT("Z Buffer"));
            break;
        case D3DZB_USEW:
            _tcscpy(szDType, TEXT("W Buffer"));
            break;
    }

    switch (m_fmtd) {
        case D3DFMT_LIN_D24S8:
            _tcscpy(szDFmt, TEXT("D3DFMT_LIN_D24S8"));
            break;
        case D3DFMT_LIN_F24S8:
            _tcscpy(szDFmt, TEXT("D3DFMT_LIN_F24S8"));
            break;
        case D3DFMT_LIN_D16:
            _tcscpy(szDFmt, TEXT("D3DFMT_LIN_D16"));
            break;
        case D3DFMT_LIN_F16:
            _tcscpy(szDFmt, TEXT("D3DFMT_LIN_F16"));
            break;
    }

    _stprintf(m_szBufferDesc, TEXT("Buffer type:   %s\n")
                              TEXT("Buffer format: %s\n")
                              TEXT("Z coordinate:  %.2f\n"),
                              szDType,
                              szDFmt,
                              mWorld._43);
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
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
BOOL CDepthBufferTest::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

#ifndef UNDER_XBOX
    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }
#endif // !UNDER_XBOX

    // Clear the rendering target
    if (m_d3dzbt != D3DZB_FALSE) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, 0, 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

m_pDisplay->SetCamera(&m_camInitial);
m_pDisplay->SetView(&m_camInitial);

    if (!(m_d3dzbt == D3DZB_USEW && m_fmtd == D3DFMT_LIN_F24S8)) {
        // Use a fixed function shader
        m_pDevice->SetVertexShader(FVF_LVERTEX);

        m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pDevice->SetRenderState(D3DRS_ZENABLE, m_d3dzbt);
        m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, m_d3dzbt != D3DZB_FALSE);
        m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

        m_pDevice->DrawVerticesUP(D3DPT_TRIANGLESTRIP, NUM_WAFFLE_SLICES * 2, m_prGreenWaffle, sizeof(LVERTEX));
        m_pDevice->DrawVerticesUP(D3DPT_TRIANGLEFAN, 4, m_prBluePlane, sizeof(LVERTEX));
    }

    xTextOut(20.0f, 420.0f, 8.0f, 15.0f, RGBA_XCOLOR(255, 0, 0, 255), m_szBufferDesc, _tcslen(m_szBufferDesc));

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
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
void CDepthBufferTest::ProcessInput() {

    CScene::ProcessInput();

    if (GetStartupContext() & TSTART_HARNESS) {
        return;
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                if (m_d3dzbt == D3DZB_FALSE) {
                    m_d3dzbt = D3DZB_TRUE;
                }
                else {
                    m_d3dzbt = D3DZB_FALSE;
                }
            }
            else {
                if (m_d3dzbt == D3DZB_TRUE) {
                    m_d3dzbt = D3DZB_USEW;
                }
                else {
                    m_d3dzbt = D3DZB_TRUE;
                }
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_Y)) {

        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {

            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                UpdateBufferFormat(!m_bD16, m_bFloatDepth);
            }
            else {
                UpdateBufferFormat(m_bD16, !m_bFloatDepth);
            }
        }
    }
}

//******************************************************************************
BOOL CDepthBufferTest::UpdateBufferFormat(BOOL bD16, BOOL bFloat) {

    D3DFORMAT fmtd;
    float z, a, b, c;
    UINT i;

    if (bFloat) {
        if (bD16) {
            fmtd = D3DFMT_LIN_F16;
        }
        else {
            fmtd = D3DFMT_LIN_F24S8;
        }
    }
    else {
        if (bD16) {
            fmtd = D3DFMT_LIN_D16;
        }
        else {
            fmtd = D3DFMT_LIN_D24S8;
        }
    }

    m_d3dpp.AutoDepthStencilFormat = fmtd;
    if (!m_pDisplay->Reset(&m_d3dpp)) {
        m_d3dpp.AutoDepthStencilFormat = m_fmtd;
        return FALSE;
    }

    // The user may have moved the display camera by hitting the joysticks.  Restore the initial view.
    m_pDisplay->SetView(&m_camInitial);

#ifdef SCALE_Z_DELTA_WITH_BUFFER_PRECISION
    if (m_bD16) {
        a = -0.5f;
        b = 0.0f;
        c = 0.5f;
    }
    else {
        a = 0.8f;
        b = 0.8995f;
        c = 0.999f;
    }

    for (i = 0; i < NUM_WAFFLE_SLICES; i++) {

        z = b;
        if (i % 4 == 0) {
            z = a;
        }
        if (i % 4 == 2) {
            z = c;
        }

        m_prGreenWaffle[2*i].vPosition.z = z;
        m_prGreenWaffle[2*i+1].vPosition.z = z;
    }
#endif

    m_fmtd = fmtd;
    m_bFloatDepth = bFloat;
    m_bD16 = bD16;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
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
BOOL CDepthBufferTest::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = 10.0f * M_PI / 180.0f;
    m_camInitial.fNearPlane    = 0.01f;
    m_camInitial.fFarPlane     = 100.0f;
    m_pDisplay->SetCamera(&m_camInitial);

    return m_pDisplay->SetView(&m_camInitial);
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
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
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CDepthBufferTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
