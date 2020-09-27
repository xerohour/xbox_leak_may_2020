/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bumpdudv.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "bumpdudv.h"

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

    CBumpDuDv2*   pBump;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pBump = new CBumpDuDv2();
    if (!pBump) {
        return FALSE;
    }

    // Initialize the scene
    if (!pBump->Create(pDisplay)) {
        pBump->Release();
        return FALSE;
    }

    bRet = pBump->Exhibit(pnExitCode);

    // Clean up the scene
    pBump->Release();

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
// CBumpDuDv2
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CBumpDuDv2
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
CBumpDuDv2::CBumpDuDv2() {

    m_pd3drGrid = NULL;
    m_pwGrid = NULL;
    m_pd3dtBase = NULL;
    m_pd3dtBump = NULL;
    m_bFade = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CBumpDuDv2
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
CBumpDuDv2::~CBumpDuDv2() {
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
BOOL CBumpDuDv2::Prepare() {

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
BOOL CBumpDuDv2::Initialize() {

    PBUMPVERTEX pr;
    D3DLOCKED_RECT d3dlr;
    Swizzler swz(4, 4, 1);
    UINT i, j;
    UINT uIndex = 0;
    HRESULT hr;

    hr = m_pDevice->CreateVertexBuffer(65 * 49 * sizeof(BUMPVERTEX), 0, FVF_BUMPVERTEX, 0, &m_pd3drGrid);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = m_pd3drGrid->Lock(0, 0, (LPBYTE*)&pr, 0);
    if (FAILED(hr)) {
        return FALSE;
    }

    for (i = 0; i <= 48; i++) {
        for (j = 0; j <= 64; j++) {
            *pr++ = BUMPVERTEX(D3DXVECTOR3((float)j * 10.0f, (float)i * 10.0f, 0.5f), 2.0f, 0.375f, 0.625f);
        }
    }

    m_pd3drGrid->Unlock();

    m_uGridIndices = 64 * 48 * 6;
    m_pwGrid = (LPWORD)MemAlloc32(m_uGridIndices * sizeof(WORD));
    if (!m_pwGrid) {
        return FALSE;
    }

    for (i = 0; i < 48; i++) {
        for (j = 0; j < 64; j++) {
            m_pwGrid[uIndex++] = (i + 1) * 65 + j;
            m_pwGrid[uIndex++] = (i) * 65 + j;
            m_pwGrid[uIndex++] = (i) * 65 + j + 1;
            m_pwGrid[uIndex++] = (i + 1) * 65 + j;
            m_pwGrid[uIndex++] = (i) * 65 + j + 1;
            m_pwGrid[uIndex++] = (i + 1) * 65 + j + 1;
        }
    }

    hr = m_pDevice->CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, 0, &m_pd3dtBase);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = m_pd3dtBase->LockRect(0, &d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return FALSE;
    }

    for (i = 0; i < 16; i++) {
        *((LPDWORD)d3dlr.pBits + i) = D3DCOLOR_RGBA(0, 0, 0, 255);
    }
    *((LPDWORD)d3dlr.pBits + (swz.SwizzleU(2) | swz.SwizzleV(3))) = D3DCOLOR_RGBA(0, 0, 255, 255);

    m_pd3dtBase->UnlockRect(0);

    hr = m_pDevice->CreateTexture(4, 4, 1, 0, D3DFMT_V8U8, 0, &m_pd3dtBump);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = m_pd3dtBump->LockRect(0, &d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return FALSE;
    }

    for (i = 0; i < 16; i++) {
        *((LPWORD)d3dlr.pBits + i) = 0;
    }
    *((LPWORD)d3dlr.pBits + (swz.SwizzleU(1) | swz.SwizzleV(2))) = (int)(0.25f * 64.0f) << 8 | (int)(0.25f * 64.0f);

    m_pd3dtBump->UnlockRect(0);

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
void CBumpDuDv2::Efface() {

    if (m_pd3dtBase) {
        m_pd3dtBase->Release();
        m_pd3dtBase = NULL;
    }
    if (m_pd3dtBump) {
        m_pd3dtBump->Release();
        m_pd3dtBump = NULL;
    }
    if (m_pd3drGrid) {
        m_pd3drGrid->Release();
        m_pd3drGrid = NULL;
    }
    if (m_pwGrid) {
        MemFree32(m_pwGrid);
        m_pwGrid = NULL;
    }
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
void CBumpDuDv2::Update() {

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
BOOL CBumpDuDv2::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
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

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGBA_MAKE(255, 0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_BUMPVERTEX);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->SetStreamSource(0, m_pd3drGrid, sizeof(BUMPVERTEX));
    m_pDevice->SetTexture(0, m_pd3dtBump);
    m_pDevice->SetTexture(1, m_pd3dtBase);

    m_pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
    m_pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
    m_pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
    m_pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, F2DW(1.0f));

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);

    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

    m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);

    m_pDevice->DrawIndexedVertices(D3DPT_TRIANGLELIST, m_uGridIndices, m_pwGrid);

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

    VerifyFrameBuffer();

    return TRUE;
}

//******************************************************************************
BOOL CBumpDuDv2::VerifyFrameBuffer() {

    CSurface8* pd3ds;
    D3DLOCKED_RECT d3dlr;
    LPDWORD pdwTexel;
    UINT i, j;
    HRESULT hr;

    hr = m_pDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = pd3ds->LockRect(&d3dlr, NULL, D3DLOCK_TILED);
    if (FAILED(hr)) {
        pd3ds->Release();
        return FALSE;
    }

    pdwTexel = (LPDWORD)d3dlr.pBits;

    for (i = 0; i < 480; i++) {

        for (j = 0; j < 640; j++) {

            if (pdwTexel[j] != 0xFF) {
                TCHAR sz[160];
                wsprintf(sz, TEXT("BUMPENVMAP: Frame buffer contents (%d, %d) differ from expected [0x%08X : 0x%08X]\r\n"), j, i, pdwTexel[j], 0xFF);
                OutputDebugString(sz);
                __asm int 3;
                pd3ds->Release();
                return FALSE;
            }
        }

        pdwTexel += d3dlr.Pitch >> 2;
    }

    pd3ds->Release();

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
void CBumpDuDv2::ProcessInput() {

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
BOOL CBumpDuDv2::InitView() {

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
BOOL CBumpDuDv2::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
