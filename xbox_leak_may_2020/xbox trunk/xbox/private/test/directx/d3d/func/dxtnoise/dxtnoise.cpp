/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dxtnoise.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "dxtnoise.h"

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

    CDXTNoise*  pTest;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CDXTNoise();
    if (!pTest) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTest->Create(pDisplay)) {
        pTest->Release();
        return FALSE;
    }

    bRet = pTest->Exhibit(pnExitCode);

    // Clean up the scene
    pTest->Release();

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
// CDXTNoise
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDXTNoise
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
CDXTNoise::CDXTNoise() {
}

//******************************************************************************
//
// Method:
//
//     ~CDXTNoise
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
CDXTNoise::~CDXTNoise() {
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
BOOL CDXTNoise::Prepare() {

    D3DPRESENT_PARAMETERS d3dpp;
    m_pDisplay->GetPresentParameters(&d3dpp);
    d3dpp.BackBufferFormat = D3DFMT_LIN_X8R8G8B8;
    d3dpp.AutoDepthStencilFormat = D3DFMT_LIN_D24S8;
    m_pDisplay->Reset(&d3dpp);

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
BOOL CDXTNoise::Initialize() {

    CTexture8* pd3dt;
    D3DSURFACE_DESC d3dsdSrc, d3dsdDst;
    D3DLOCKED_RECT d3dlrSrc, d3dlrDst;
    HRESULT hr;

    pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("gradient.bmp"), D3DFMT_LIN_X8R8G8B8);
    if (!pd3dt) {
        return FALSE;
    }

    pd3dt->GetLevelDesc(0, &d3dsdSrc);

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, d3dsdSrc.Width, d3dsdSrc.Height, D3DFMT_DXT1);
    if (!m_pd3dt) {
        ReleaseTexture(pd3dt);
        return FALSE;
    }

    m_pd3dt->GetLevelDesc(0, &d3dsdDst);

    pd3dt->LockRect(0, &d3dlrSrc, NULL, 0);
    m_pd3dt->LockRect(0, &d3dlrDst, NULL, 0);

    hr = XGCompressRect(d3dlrDst.pBits, D3DFMT_DXT1, d3dlrDst.Pitch, d3dsdDst.Width, d3dsdDst.Height, d3dlrSrc.pBits, D3DFMT_LIN_X8R8G8B8, d3dlrSrc.Pitch, 1.0f, 0);
    if (ResultFailed(hr, TEXT("XGCompressRect"))) {
        ReleaseTexture(pd3dt);
        return FALSE;
    }

    pd3dt->UnlockRect(0);
    m_pd3dt->UnlockRect(0);

    ReleaseTexture(pd3dt);

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
void CDXTNoise::Efface() {

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
void CDXTNoise::Update() {

    CScene::Update();
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
BOOL CDXTNoise::Render() {

    TLVERTEX prVertices[4];
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

    // Initialize the vertices
    prVertices[0] = TLVERTEX(D3DXVECTOR3(0.0f, 400.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 0.0f, 1.0f);
    prVertices[1] = TLVERTEX(D3DXVECTOR3(0.0f, 80.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 0.0f, 0.0f);
    prVertices[2] = TLVERTEX(D3DXVECTOR3(320.0f, 80.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 1.0f, 0.0f);
    prVertices[3] = TLVERTEX(D3DXVECTOR3(320.0f, 400.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 1.0f, 1.0f);

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dt);

    m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    m_pDevice->SetRenderState(D3DRS_DITHERENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_DXT1NOISEENABLE, FALSE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, prVertices, sizeof(VERTEX));

    prVertices[0] = TLVERTEX(D3DXVECTOR3(320.0f, 400.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 0.0f, 1.0f);
    prVertices[1] = TLVERTEX(D3DXVECTOR3(320.0f, 80.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 0.0f, 0.0f);
    prVertices[2] = TLVERTEX(D3DXVECTOR3(640.0f, 80.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 1.0f, 0.0f);
    prVertices[3] = TLVERTEX(D3DXVECTOR3(640.0f, 400.0f, 0.5f), 0.5f, 0xFFFFFFFF, 0, 1.0f, 1.0f);

    m_pDevice->SetRenderState(D3DRS_DXT1NOISEENABLE, TRUE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, prVertices, sizeof(VERTEX));

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
void CDXTNoise::ProcessInput() {

    CScene::ProcessInput();

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            D3DPRESENT_PARAMETERS d3dpp;
            m_pDisplay->GetPresentParameters(&d3dpp);
            switch (d3dpp.BackBufferFormat) {
                case D3DFMT_LIN_X8R8G8B8:
                    d3dpp.BackBufferFormat = D3DFMT_LIN_A8R8G8B8;
                    d3dpp.AutoDepthStencilFormat = D3DFMT_LIN_D24S8;
                    break;
                case D3DFMT_LIN_A8R8G8B8:
                    d3dpp.BackBufferFormat = D3DFMT_LIN_R5G6B5;
                    d3dpp.AutoDepthStencilFormat = D3DFMT_LIN_D16;
                    break;
                case D3DFMT_LIN_R5G6B5:
                    d3dpp.BackBufferFormat = D3DFMT_LIN_X1R5G5B5;
                    d3dpp.AutoDepthStencilFormat = D3DFMT_LIN_D16;
                    break;
                case D3DFMT_LIN_X1R5G5B5:
                    d3dpp.BackBufferFormat = D3DFMT_LIN_X8R8G8B8;
                    d3dpp.AutoDepthStencilFormat = D3DFMT_LIN_D24S8;
                    break;
            }
            m_pDisplay->Reset(&d3dpp);
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
BOOL CDXTNoise::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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
BOOL CDXTNoise::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
