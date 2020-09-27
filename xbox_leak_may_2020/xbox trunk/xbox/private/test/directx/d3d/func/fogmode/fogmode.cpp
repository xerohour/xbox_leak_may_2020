/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fogmode.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "fogmode.h"

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

    CFogMode*   pFogMode;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pFogMode = new CFogMode();
    if (!pFogMode) {
        return FALSE;
    }

    // Initialize the scene
    if (!pFogMode->Create(pDisplay)) {
        pFogMode->Release();
        return FALSE;
    }

    bRet = pFogMode->Exhibit(pnExitCode);

    // Clean up the scene
    pFogMode->Release();

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
// CFogMode
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CFogMode
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
CFogMode::CFogMode() {

    m_prPlane[0] = VERTEX(D3DXVECTOR3(-1000.0f, -5.0f, -1000.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 100.0f);
    m_prPlane[1] = VERTEX(D3DXVECTOR3(-1000.0f, -5.0f, 1000.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prPlane[2] = VERTEX(D3DXVECTOR3( 1000.0f, -5.0f, 1000.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 100.0f, 0.0f);
    m_prPlane[3] = VERTEX(D3DXVECTOR3( 1000.0f, -5.0f, -1000.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 100.0f, 100.0f);

    m_bFog = TRUE;
    m_uMode = 1;
    m_bTable = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CFogMode
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
CFogMode::~CFogMode() {

    ReleaseSphere(&m_prSphere, &m_pwSphere);
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the test for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CFogMode::Create(CDisplay* pDisplay) {

    return CScene::Create(pDisplay);
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Execute the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     The exit value contained in the wParam parameter of the WM_QUIT message.
//
//******************************************************************************
int CFogMode::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
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
BOOL CFogMode::Prepare() {

    UINT i;

    CreateSphere(&m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices, 50.0f, 16, 16, 10.0f, 10.0f);

    m_mWorld = m_mIdentity;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Setup
//
// Description:
//
//     Obtain the device interface pointer from the display, save the current
//     state of the device, and initialize the background vertices to the
//     dimensions of the render target.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CFogMode::Setup() {

    return CScene::Setup();
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
BOOL CFogMode::Initialize() {

    // Create a texture
    m_pd3dtSphere = (CTexture8*)CreateTexture(m_pDevice, TEXT("redvelv.bmp"));
    if (!m_pd3dtSphere) {
        return FALSE;
    }

    m_pd3dtPlane = (CTexture8*)CreateTexture(m_pDevice, TEXT("back.bmp"));
    if (!m_pd3dtPlane) {
        return FALSE;
    }


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
void CFogMode::Efface() {

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
void CFogMode::Update() {

    D3DXMATRIX mRotate;
    float fTheta = M_PI / 200.0f * m_fFrameDelta;

    InitMatrix(&mRotate,
        (float)(cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)(sin(fTheta)), (float)( 0.0f), (float)(cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    D3DXMatrixMultiply(&m_mWorld, &m_mWorld, &mRotate);
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
BOOL CFogMode::Render() {

    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    D3DFOGMODE  d3dfm[4] = {D3DFOG_NONE, D3DFOG_LINEAR, D3DFOG_EXP, D3DFOG_EXP2};

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

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

#ifndef UNDER_XBOX
    if (m_bTable) {
        SetPixelFog(m_pDevice, RGB_MAKE(0, 0, 255), 100.0f, 200.0f, 1.0f, d3dfm[m_uMode]);
    }
    else {
        SetVertexFog(m_pDevice, RGB_MAKE(0, 0, 255), 100.0f, 200.0f, 1.0f, d3dfm[m_uMode]);
    }
#else
    SetPixelFog(m_pDevice, RGB_MAKE(0, 0, 255), 100.0f, 200.0f, 1.0f, d3dfm[m_uMode]);
    m_pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, !m_bTable);
#endif
    m_pDevice->SetRenderState(D3DRS_FOGENABLE, m_bFog);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);
    m_pDevice->SetTexture(0, m_pd3dtPlane);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prPlane, sizeof(VERTEX));

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
    m_pDevice->SetTexture(0, m_pd3dtSphere);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices,
                                        m_dwSphereIndices / 3, m_pwSphere,
                                        D3DFMT_INDEX16, m_prSphere, sizeof(VERTEX));

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
void CFogMode::ProcessInput() {

    static UINT uFMDisplay = 0;

    m_pDisplay->GetJoyState(&m_jsJoys, &m_jsLast);

#ifdef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            FadeOut();
            m_bQuit = TRUE;
        }
    }
#endif // UNDER_XBOX

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_Y)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {
            m_bFog = !m_bFog;
        }
    }

#ifndef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_bTable = !m_bTable;
        }
    }
#else
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_bTable = !m_bTable;
//            if (++m_uMode > 3) {
//                m_uMode = 0;
//            }
        }
    }
#endif

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                m_bPaused = !m_bPaused;
            }
            else {
                m_bPaused = TRUE;
                m_bAdvance = TRUE;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_B)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_B)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                // ##TODO: Reset the device and toggle FSAA
            }
            else {
                if (m_fTimeDilation < 1.5f && m_fTimeDilation > 0.26f) {
                    m_fTimeDilation /= 2.0f;
                }
                else if (m_fTimeDilation < 0.26f) {
                    m_fTimeDilation = 2.0f;
                }
                else if (m_fTimeDilation >= 1.5f) {
                    m_fTimeDilation = 1.0f;
                }
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_WHITE)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_WHITE) && !BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
            m_pDisplay->SetCamera(&m_camInitial);
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RSTICK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RSTICK) && !BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
            switch (uFMDisplay++) {
                case 0:
                    m_bShowFPS = TRUE;
                    m_bShowMode = TRUE;
                    break;
                case 1:
                    m_bShowFPS = TRUE;
                    m_bShowMode = FALSE;
                    break;
                case 2:
                    m_bShowFPS = FALSE;
                    m_bShowMode = TRUE;
                    break;
                case 3:
                    m_bShowFPS = FALSE;
                    m_bShowMode = FALSE;
                    break;
            }
            if (uFMDisplay > 3) {
                uFMDisplay = 0;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LSTICK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LSTICK)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                DWORD dwZFunc;
                if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_ZFUNC, (LPDWORD)&dwZFunc))) {
                    if (dwZFunc == D3DCMP_LESSEQUAL) {
                        dwZFunc = D3DCMP_ALWAYS;
                    }
                    else {
                        dwZFunc = D3DCMP_LESSEQUAL;
                    }
                    m_pDevice->SetRenderState(D3DRS_ZFUNC, dwZFunc);
                }
            }
            else {
                DWORD dwFillMode;
                if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_FILLMODE, (LPDWORD)&dwFillMode))) {
                    if (dwFillMode == D3DFILL_SOLID) {
                        dwFillMode = D3DFILL_WIREFRAME;
                    }
                    else {
                        dwFillMode = D3DFILL_SOLID;
                    }
                    m_pDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
                }
            }
        }
    }
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
BOOL CFogMode::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -200.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 20.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
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
BOOL CFogMode::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

        case WM_CHAR:

            switch (wParam) {

                case TEXT('Z'):
                case TEXT('z'):
                    m_bFog = !m_bFog;
                    return 0;

                case TEXT('X'):
                case TEXT('x'):
                    m_bTable = !m_bTable;
                    return 0;

                case TEXT('C'):
                case TEXT('c'):
                    if (++m_uMode > 3) {
                        m_uMode = 0;
                    }
                    return 0;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
