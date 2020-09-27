/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dragnfly.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "dragnfly.h"

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

    CDragonfly* pDragonfly;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pDragonfly = new CDragonfly();
    if (!pDragonfly) {
        return FALSE;
    }

    // Initialize the scene
    if (!pDragonfly->Create(pDisplay)) {
        pDragonfly->Release();
        return FALSE;
    }

    bRet = pDragonfly->Exhibit(pnExitCode);

    // Clean up the scene
    pDragonfly->Release();

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
// CDragonfly
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDragonfly
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
CDragonfly::CDragonfly() {

    m_prVertices = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CDragonfly
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
CDragonfly::~CDragonfly() {

    if (m_prVertices) {
        MemFree32(m_prVertices);
    }
    ReleaseMesh(&m_mesh);
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
BOOL CDragonfly::Create(CDisplay* pDisplay) {

    CPerlinNoise3D pNoise(0.5f, 5, GetTickCount(), INTERPOLATE_COSINE, FALSE, FALSE);
    float fIntensity;
    UINT i;
#ifndef UNDER_XBOX
    if (!CreateMesh(TEXT("dragnfly.xdg"), &m_mesh)) {
#else
    if (!CreateMesh(TEXT("dragnfly.xdg"), &m_mesh)) {
#endif // UNDER_XBOX
        return FALSE;
    }
    m_prVertices = (PDVERTEX)MemAlloc32(m_mesh.uNumVertices * sizeof(DVERTEX));
    if (!m_prVertices) {
        return FALSE;
    }
    pNoise.SetRange(0.0f, 106.0f);
    for (i = 0; i < m_mesh.uNumVertices; i++) {
        m_prVertices[i].vPosition = m_mesh.prVertices[i].vPosition;
        m_prVertices[i].vNormal = m_mesh.prVertices[i].vNormal;
        m_prVertices[i].u0 = m_mesh.prVertices[i].u0;
        m_prVertices[i].v0 = m_mesh.prVertices[i].v0;
        fIntensity = pNoise.GetIntensity(m_prVertices[i].vPosition.x, m_prVertices[i].vPosition.y, m_prVertices[i].vPosition.z);
        m_prVertices[i].cDiffuse = RGB_MAKE(23, (BYTE)fIntensity, (BYTE)fIntensity + 30);
    }
    pDisplay->SetCameraOffsets(1.0f, 0.011415f * 2.0f);
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
int CDragonfly::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
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
BOOL CDragonfly::Setup() {

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
BOOL CDragonfly::Initialize() {

    if (!CScene::Initialize()) {
        return FALSE;
    }

    if (!SetMaterial(m_pDevice, RGBA_MAKE(255, 255, 255, 255), 0, RGBA_MAKE(64, 64, 64, 64))) {
        return FALSE;
    }

    m_pd3dr = CreateVertexBuffer(m_pDevice, m_prVertices, m_mesh.uNumVertices * sizeof(DVERTEX), 0, FVF_DVERTEX);
    if (!m_pd3dr) {
        return FALSE;
    }

    m_pd3di = CreateIndexBuffer(m_pDevice, m_mesh.pwIndices, m_mesh.uNumIndices * sizeof(WORD));
    if (!m_pd3di) {
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
void CDragonfly::Efface() {

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
void CDragonfly::Update() {

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
BOOL CDragonfly::Render() {

    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    D3DXMATRIX  mWorld;
    HRESULT     hr;

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

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_DVERTEX);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_AMBIENT, RGB_MAKE(32, 32, 32));

    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

    D3DXMatrixIdentity(&mWorld);
    mWorld._11 = 0.75f;
    mWorld._22 = 0.75f;
    mWorld._33 = 0.75f;
    mWorld._41 = -5.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

//    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, m_prVertices, sizeof(DVERTEX));

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(DVERTEX));
    m_pDevice->SetIndices(m_pd3di, 0);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, 0, m_mesh.uNumIndices / 3);
//    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, 0, m_mesh.uNumIndices / 6);
//    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, m_mesh.uNumIndices / 2, m_mesh.uNumIndices / 6);

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
void CDragonfly::ProcessInput() {

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
BOOL CDragonfly::InitView() {

    return CScene::InitView();
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
BOOL CDragonfly::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
