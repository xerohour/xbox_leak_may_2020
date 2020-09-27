/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "textr.h"
#include "resource.h"

static CTexture8* CreateMipmap(CDevice8* pDevice, UINT uTexBase, D3DFORMAT fmt, TEXTURETYPE ttype);

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

    CTextr4* pTextr;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTextr = new CTextr4();
    if (!pTextr) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTextr->Create(pDisplay)) {
        pTextr->Release();
        return FALSE;
    }

    bRet = pTextr->Exhibit(pnExitCode);

    // Clean up the scene
    pTextr->Release();

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
// CTextr4
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTextr4
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
CTextr4::CTextr4() {

    m_fOffset = -0.001f;
    m_mScale = m_mIdentity;

    // Initialize the vertices
    m_prVertices[0] = VERTEX(D3DXVECTOR3(-2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prVertices[1] = VERTEX(D3DXVECTOR3(-2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prVertices[2] = VERTEX(D3DXVECTOR3( 2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prVertices[3] = VERTEX(D3DXVECTOR3( 2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);
}

//******************************************************************************
//
// Method:
//
//     ~CTextr4
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
CTextr4::~CTextr4() {
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
BOOL CTextr4::Create(CDisplay* pDisplay) {

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
int CTextr4::Exhibit(int *pnExitCode) {

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
BOOL CTextr4::Prepare() {

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
BOOL CTextr4::Setup() {

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
BOOL CTextr4::Initialize() {

    D3DMATERIAL8 material;
    UINT         i;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    SetMaterial(&material, RGB_MAKE(255, 255, 255));
    SetMaterialTranslucency(&material, 0.5f);
    m_pDevice->SetMaterial(&material);

    m_pd3dt = CreateMipmap(m_pDevice, BrickMip0, D3DFMT_R5G6B5, TTYPE_MIPMAP);
    if (!m_pd3dt) { 
        return FALSE;
    }

    m_pDevice->SetTexture(0, m_pd3dt);

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
void CTextr4::Efface() {

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
void CTextr4::Update() {

    UINT i;

    for (i = 0; i < 3; i++) {
        m_mScale(i, i) += m_fOffset * m_fTimeDilation;
    }
    if (m_mScale(0, 0) > 1.0f) {
        m_fOffset = -0.001f;
    }
    else if (m_mScale(0, 0) < 0.05) {
        m_fOffset = 0.001f;
        if (GetStartupContext() & TSTART_HARNESS) {
            SetFrameDuration(m_fFrame);
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
BOOL CTextr4::Render() {

    D3DXMATRIX              mWorld, mProduct;
    DWORD                   dwClearFlags = D3DCLEAR_TARGET;
    HRESULT                 hr;
    UINT                    i, j;
    D3DTEXTUREFILTERTYPE    d3dtft[] = {
                                D3DTEXF_NONE,
                                D3DTEXF_POINT, 
                                D3DTEXF_LINEAR
                            };

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

    mWorld = m_mIdentity;

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

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    m_pDevice->SetVertexShader(FVF_VERTEX);

    for (j = 0; j < 2; j++) {

        mWorld._42 = 2.75f - j * 5.5f;
        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, j);

        for (i = 0; i < 3; i++) {

            mWorld._41 = -5.5f + i * 5.5f;

            D3DXMatrixMultiply(&mProduct, &m_mScale, &mWorld);
            m_pDevice->SetTransform(D3DTS_WORLD, &mProduct);

            m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, d3dtft[i]);

            // Render
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(VERTEX));

            m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
        }
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
void CTextr4::ProcessInput() {

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
BOOL CTextr4::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -20.0f);
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
BOOL CTextr4::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

//******************************************************************************
CTexture8* CreateMipmap(CDevice8* pDevice, UINT uTexBase, D3DFORMAT fmt, TEXTURETYPE ttype) {

    CTexture8 *pd3dt, *pd3dtTemp;
    CSurface8 *pd3ds, *pd3dsTemp;
    UINT i, uLevels;
    HRESULT hr;
    LPTSTR  szTexture[] = {
                TEXT("brikmip0.bmp"),
                TEXT("brikmip1.bmp"),
                TEXT("brikmip2.bmp"),
                TEXT("brikmip3.bmp"),
                TEXT("brikmip4.bmp"),
                TEXT("brikmip5.bmp"),
                TEXT("brikmip6.bmp"),
                TEXT("brikmip7.bmp"),
                TEXT("brikmip8.bmp")
            };

    pd3dt = (CTexture8*)CreateTexture(pDevice, szTexture[0], fmt, ttype);
    if (!pd3dt) {
        return NULL;
    }

    uLevels = pd3dt->GetLevelCount();

    for (i = 1; i < uLevels && i < 9; i++) {

        hr = pd3dt->GetSurfaceLevel(i, &pd3ds);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        pd3dtTemp = (CTexture8*)CreateTexture(pDevice, szTexture[i], fmt);
        if (!pd3dtTemp) {
            pd3ds->Release();
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = pd3dtTemp->GetSurfaceLevel(0, &pd3dsTemp);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dtTemp);
            pd3ds->Release();
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = D3DXLoadSurfaceFromSurface(pd3ds->GetIDirect3DSurface8(), NULL, NULL, pd3dsTemp->GetIDirect3DSurface8(), NULL, NULL, D3DX_FILTER_POINT, 0);

        pd3dsTemp->Release();
        ReleaseTexture(pd3dtTemp);
        pd3ds->Release();

        if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
            ReleaseTexture(pd3dt);
            return NULL;
        }
    }

    if (!SyncTexture(pd3dt)) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    return pd3dt;
}
