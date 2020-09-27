/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    persist.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "persist.h"

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

    CPersistDisplay*    pPersist;
    BOOL                bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPersist = new CPersistDisplay();
    if (!pPersist) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPersist->Create(pDisplay)) {
        pPersist->Release();
        return FALSE;
    }

    bRet = pPersist->Exhibit(pnExitCode);

    // Clean up the scene
    pPersist->Release();

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
// CPersistDisplay
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CPersistDisplay
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
CPersistDisplay::CPersistDisplay() {

    m_pd3dt = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CPersistDisplay
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
CPersistDisplay::~CPersistDisplay() {

    if (!(GetStartupContext() & TSTART_HARNESS)) {
        m_pDevice->PersistDisplay();
        XLaunchNewImage("d:\\" MODULE_STRING ".xbe", NULL);
    }
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
BOOL CPersistDisplay::Prepare() {

    SetFrameDuration(300.0f);
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
BOOL CPersistDisplay::Initialize() {

    // Create a texture
    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("testpat.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, RGB_MAKE(0, 0, 0));
    if (!m_pd3dt) {
        return FALSE;
    }

    m_pDisplay->GetPresentParameters(&m_d3dppReset);

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
void CPersistDisplay::Efface() {

    ReleaseTexture(m_pd3dt);
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
void CPersistDisplay::Update() {

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
BOOL CPersistDisplay::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    BYTE     Intensity;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

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

    if (!((UINT)m_fFrame % 60)) {
        m_pDevice->PersistDisplay();
        switch ((UINT)m_fFrame) {
            case 60:
                m_d3dppReset.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
                m_pDisplay->Reset(&m_d3dppReset);
                break;
            case 120:
                m_d3dppReset.MultiSampleType = D3DMULTISAMPLE_NONE;
                m_d3dppReset.BackBufferCount = 2;
                m_pDisplay->ReleaseDevice();
                m_pDisplay->CreateDevice(&m_d3dppReset);
                break;
            case 180:
                m_d3dppReset.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR;
                m_d3dppReset.BackBufferCount = 1;
                m_pDisplay->Reset(&m_d3dppReset);
                break;
            case 240:
                m_d3dppReset.MultiSampleType = D3DMULTISAMPLE_NONE;
                m_d3dppReset.BackBufferFormat = D3DFMT_LIN_X1R5G5B5;
                m_pDisplay->ReleaseDevice();
                m_pDisplay->CreateDevice(&m_d3dppReset);
                break;
        }
        m_fFrame += 1.0f;
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

    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dt);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    Intensity = (BYTE)(128.0f + 127.0f * (float)cos(m_fFrame / 300.0f * M_2PI));
    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(Intensity, Intensity, Intensity, 255));

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

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
void CPersistDisplay::ProcessInput() {

    CScene::ProcessInput();
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
BOOL CPersistDisplay::InitView() {

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
BOOL CPersistDisplay::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
