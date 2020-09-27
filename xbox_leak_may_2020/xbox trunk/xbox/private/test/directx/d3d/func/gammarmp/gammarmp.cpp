    /*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    gammarmp.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "gammarmp.h"

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

    CGammaRamp* pGammaRamp;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pGammaRamp = new CGammaRamp();
    if (!pGammaRamp) {
        return FALSE;
    }

    // Initialize the scene
    if (!pGammaRamp->Create(pDisplay)) {
        pGammaRamp->Release();
        return FALSE;
    }

    bRet = pGammaRamp->Exhibit(pnExitCode);

    // Clean up the scene
    pGammaRamp->Release();

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
// CGammaRamp
//******************************************************************************

CPerlinNoise2D CGammaRamp::m_NoiseR(0.25f, 5, 0xF334C125, INTERPOLATE_COSINE, TRUE, FALSE);
CPerlinNoise2D CGammaRamp::m_NoiseG(0.25f, 5, 0x234A65C3, INTERPOLATE_COSINE, TRUE, FALSE);
CPerlinNoise2D CGammaRamp::m_NoiseB(0.25f, 5, 0x45B98D03, INTERPOLATE_COSINE, TRUE, FALSE);

//******************************************************************************
//
// Method:
//
//     CGammaRamp
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
CGammaRamp::CGammaRamp() {

    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    m_pd3dt = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CGammaRamp
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
CGammaRamp::~CGammaRamp() {
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
BOOL CGammaRamp::Prepare() {

    if (GetStartupContext() & TSTART_HARNESS) {
//        SetFrameDuration(3365.0f);
        SetFrameDuration(4565.0f);
    }

    m_NoiseR.SetRange(0.0f, 255.0f);
    m_NoiseG.SetRange(0.0f, 255.0f);
    m_NoiseB.SetRange(0.0f, 255.0f);

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
BOOL CGammaRamp::Initialize() {

    UINT i;

    m_pDevice->GetGammaRamp(&m_d3dgr);

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"), D3DFMT_A8R8G8B8);
    if (!m_pd3dt) {
        return FALSE;
    }

    return TRUE;
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
void CGammaRamp::Efface() {
    
    UINT i;

    for (i = 0; i < 256; i++) {
        m_d3dgr.red[i] = (BYTE)i;
        m_d3dgr.green[i] = (BYTE)i;
        m_d3dgr.blue[i] = (BYTE)i;
    }

    m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &m_d3dgr);

m_pd3dt->AddRef();
m_pd3dt->Release();
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
void CGammaRamp::Update() {

    UINT    i;

    // Frame    X: X indicates the frame currently being composed, not the frame currently being displayed on the screen
    // Frame    1: Default gamma: Present image with default gamma
    // Frame    2; Default gamma: Change gamma to half intensity with an update on present.  Delay the present of frame 2 (by increasing scene complexity) to verify the gamma ramp does not change until after the Present).
    // Frame    3: Half gamma: The gamma set at frame 2 should take effect during composition of frame 3.  A device Reset and render target change are performed at the beginning of this frame, but should not effect the gamma settings.
    // Frame  240: Red gamma: The intensity of red moves from full intensity to zero.  Green and blue ramps are set to zero.
    // Frame  861: Green gamma: The intensity of green moves from full intensity to zero.  Red and blue ramps are set to zero.
    // Frame 1482: Blue gamma: The intensity of blue moves from full intensity to zero.  Red and green ramps are set to zero.
    // Frame 2104: White gamma: The intensity of all ramps move from zero to full intensity.
    // Frame 2360: Inverted red gamma: The red gamma curve inverts itself.  Green and blue ramps remain unchanged.
    // Frame 2615: Inverted green gamma: The green gamma curve inverts itself.  Red and blue ramps remain unchanged.
    // Frame 2870: Inverted blue gamma: The blue gamma curve inverts itself.  Red and green ramps remain unchanged.
    // Frame 3125: Noisy gamma: Gamma ramps change according to three noise functions.  They are set with an IMMEDIATE flag (they take effect before the present).  No further rendering calls are made to verify the gamma update is taking place.

    if (m_fFrame < 240.0f) {

        if (m_fFrame == 2.0f) {
            for (i = 0; i < 256; i++) {
                m_d3dgr.red[i] = i / 2;
                m_d3dgr.green[i] = i / 2;
                m_d3dgr.blue[i] = i / 2;
            }

            m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &m_d3dgr);
        }
        else if (m_fFrame == 239.0f) {
            for (i = 0; i < 256; i++) {
                m_d3dgr.red[i] = (BYTE)i;
                m_d3dgr.green[i] = 0;
                m_d3dgr.blue[i] = 0;
                m_fgr.red[i] = (float)i;
                m_fgr.green[i] = 0.0f;
                m_fgr.blue[i] = 0.0f;
            }
        }
    }
    else if (m_fFrame < 2104.0f) {
        if (m_fgr.red[255] > 0.5f) {
            for (i = 0; i < 256; i++) {
                m_fgr.red[i] *= 0.99f;
                m_d3dgr.red[i] = (BYTE)(m_fgr.red[i] + 0.5f);
            }
            if (m_d3dgr.red[255] == 0) {
                for (i = 0; i < 256; i++) {
                    m_fgr.green[i] = (float)i;
                }
            }
        }
        else if (m_fgr.green[255] > 0.5f) {
            for (i = 0; i < 256; i++) {
                m_fgr.green[i] *= 0.99f;
                m_d3dgr.green[i] = (BYTE)(m_fgr.green[i] + 0.5f);
            }
            if (m_d3dgr.green[255] == 0) {
                for (i = 0; i < 256; i++) {
                    m_fgr.blue[i] = (float)i;
                }
            }
        }
        else if (m_fgr.blue[255] > 0.5f) {
            for (i = 0; i < 256; i++) {
                m_fgr.blue[i] *= 0.99f;
                m_d3dgr.blue[i] = (BYTE)(m_fgr.blue[i] + 0.5f);
            }
        }
    }
    else if (m_fFrame < 2360.0f) {

        for (i = 0; i < 256; i++) {
            m_fgr.red[i] = (float)i * ((m_fFrame - 2104.0f) / 255.0f);
            m_fgr.green[i] = (float)i * ((m_fFrame - 2104.0f) / 255.0f);
            m_fgr.blue[i] = (float)i * ((m_fFrame - 2104.0f) / 255.0f);
            m_d3dgr.red[i] = (BYTE)(m_fgr.red[i] + 0.5f);
            m_d3dgr.green[i] = (BYTE)(m_fgr.green[i] + 0.5f);
            m_d3dgr.blue[i] = (BYTE)(m_fgr.blue[i] + 0.5f);
        }
    }
    else if (m_fFrame < 3125.0f) {
        if (m_fgr.red[255] > 0.5f) {
            for (i = 0; i < 128; i++) {
                m_fgr.red[i] += 1.0f - ((float)i / 127.0f);
                m_d3dgr.red[i] = (BYTE)(m_fgr.red[i] + 0.5f);
            }
            for (i = 128; i < 256; i++) {
                m_fgr.red[i] -= (((float)i - 128.0f) / 127.0f);
                m_d3dgr.red[i] = (BYTE)(m_fgr.red[i] + 0.5f);
            }
        }
        else if (m_fgr.green[255] > 0.5f) {
            for (i = 0; i < 128; i++) {
                m_fgr.green[i] += 1.0f - ((float)i / 127.0f);
                m_d3dgr.green[i] = (BYTE)(m_fgr.green[i] + 0.5f);
            }
            for (i = 128; i < 256; i++) {
                m_fgr.green[i] -= (((float)i - 128.0f) / 127.0f);
                m_d3dgr.green[i] = (BYTE)(m_fgr.green[i] + 0.5f);
            }
        }
        else if (m_fgr.blue[255] > 0.5f) {
            for (i = 0; i < 128; i++) {
                m_fgr.blue[i] += 1.0f - ((float)i / 127.0f);
                m_d3dgr.blue[i] = (BYTE)(m_fgr.blue[i] + 0.5f);
            }
            for (i = 128; i < 256; i++) {
                m_fgr.blue[i] -= (((float)i - 128.0f) / 127.0f);
                m_d3dgr.blue[i] = (BYTE)(m_fgr.blue[i] + 0.5f);
            }
        }
    }
    else {

        for (i = 0; i < 256; i++) {
            m_d3dgr.red[i] = (BYTE)(m_NoiseR.GetIntensity(m_fFrame / 800.0f, (float)i / 50.0f));
            m_d3dgr.green[i] = (BYTE)(m_NoiseG.GetIntensity(m_fFrame / 800.0f, (float)i / 50.0f));
            m_d3dgr.blue[i] = (BYTE)(m_NoiseB.GetIntensity(m_fFrame / 800.0f, (float)i / 50.0f));
        }
    }
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
BOOL CGammaRamp::Render() {

    D3DGAMMARAMP    d3dgr;
    DWORD           dwClearFlags = D3DCLEAR_TARGET;
    UINT            i;
    HRESULT         hr;

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

    if (m_fFrame >= 3125.0f) {
        m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION | D3DSGR_IMMEDIATE, &m_d3dgr);

        m_pDevice->GetGammaRamp(&d3dgr);
        if (memcmp(&m_d3dgr, &d3dgr, sizeof(D3DGAMMARAMP))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetGammaRamp returned an incorrect gamma ramp preceding the Present update"));
        }

        if (!m_pfnFade) {
            return TRUE;
        }
    }

    if (m_fFrame >= 239.0f) {
        m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &m_d3dgr);

        m_pDevice->GetGammaRamp(&d3dgr);
        if (memcmp(&m_d3dgr, &d3dgr, sizeof(D3DGAMMARAMP))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetGammaRamp returned an incorrect gamma ramp preceding the Present update"));
        }
    }

    if (m_fFrame == 3.0f) {

        D3DPRESENT_PARAMETERS d3dpp, d3dppOriginal;
        CSurface8* pd3dsTarget, *pd3dsDepth, *pd3dsTemp;
        CTexture8* pd3dtTemp;

        memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
        d3dpp.BackBufferWidth                   = 640;
        d3dpp.BackBufferHeight                  = 480;
        d3dpp.BackBufferFormat                  = D3DFMT_LIN_R5G6B5;
        d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
        d3dpp.AutoDepthStencilFormat            = D3DFMT_LIN_D16;
        d3dpp.FullScreen_RefreshRateInHz        = 0;
        d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
        d3dpp.BackBufferCount                   = 1;
        d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow                     = NULL;
        d3dpp.Windowed                          = FALSE;
        d3dpp.EnableAutoDepthStencil            = FALSE;
        m_pDisplay->GetPresentParameters(&d3dppOriginal);
        m_pDevice->PersistDisplay();
        if (!m_pDisplay->Reset(&d3dpp)) {
            return FALSE;
        }
        if (!m_pDisplay->Reset(&d3dppOriginal)) {
            return FALSE;
        }
        m_pDisplay->m_bTimeSync = FALSE;

        pd3dtTemp = (CTexture8*)CreateTexture(m_pDevice, 256, 256, D3DFMT_LIN_R5G6B5, TTYPE_TARGET);
        if (!pd3dtTemp) {
            return FALSE;
        }
        pd3dtTemp->GetSurfaceLevel(0, &pd3dsTemp);
        ReleaseTexture(pd3dtTemp);

        m_pDevice->GetRenderTarget(&pd3dsTarget);
        m_pDevice->GetDepthStencilSurface(&pd3dsDepth);

        m_pDevice->SetRenderTarget(pd3dsTemp, NULL);
        pd3dsTemp->Release();

        m_pDevice->SetRenderTarget(pd3dsTarget, pd3dsDepth);

        pd3dsTarget->Release();
        pd3dsDepth->Release();
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

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dt);

    if (m_fFrame == 2.0f) {
        for (i = 0; i < 5000; i++) {
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));
        }
    }
    else {
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));
    }

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

    m_pDevice->GetGammaRamp(&d3dgr);

    if (memcmp(&m_d3dgr, &d3dgr, sizeof(D3DGAMMARAMP))) {
        Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetGammaRamp returned an incorrect gamma ramp following the Present update"));
    }

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
void CGammaRamp::ProcessInput() {

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
BOOL CGammaRamp::InitView() {

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
BOOL CGammaRamp::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
