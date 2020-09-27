/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    clockwork.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "cogs.h"
#include "clockwork.h"

//******************************************************************************
BOOL CreateSphereV(float fRadius, PVERTEX* pprVertices, LPDWORD pdwNumVertices,
                  LPWORD* ppwIndices, LPDWORD pdwNumIndices) {

    PVERTEX     prVertices;
    PVERTEX     pr;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    UINT        uIndex = 0;
    UINT        uStepsU = SPHERE_U, uStepsV = SPHERE_V;
    UINT        i, j;
    float       fX, fY, fTX, fSinY, fCosY;

    if (!pprVertices || !pdwNumVertices || !ppwIndices || !pdwNumIndices) {
        return FALSE;
    }

    *pprVertices = NULL;
    *pdwNumVertices = 0;
    *ppwIndices = NULL;
    *pdwNumIndices = 0;

    dwNumVertices = (uStepsU + 1) * uStepsV;

    // Allocate memory for the vertices
    prVertices = (PVERTEX)MemAlloc32(dwNumVertices * sizeof(VERTEX));
    if (!prVertices) {
        return FALSE;
    }

    // Allocate memory for the indices
    dwNumIndices = uStepsU * (uStepsV - 1) * 6;

    pwIndices = new WORD[dwNumIndices];
    if (!pwIndices) {
        MemFree32(prVertices);
        return FALSE;
    }

    // Create the sphere
    for (j = 0; j < uStepsV; j++) {

        fY = (float)(j) / (float)(uStepsV - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i <= uStepsU; i++) {

            pr = &prVertices[(uStepsU + 1) * j + i];
            fX = (float)(i) / (float)(uStepsU);
            fTX = fX * M_2PI;

            pr->vNormal.x = -(float)(sin(fTX));// * fSinY;
            pr->vNormal.y = -fCosY;
            pr->vNormal.z = (float)(cos(fTX));// * fSinY;
            pr->vPosition.x = -pr->vNormal.x * fRadius;
            pr->vPosition.y = -pr->vNormal.y * fRadius;
            pr->vPosition.z = -pr->vNormal.z * fRadius;
            pr->u0 = fX * 4.0f;
            pr->v0 = fY * 4.0f;
        }
    }

    for (j = 0; j < uStepsU; j++) {

        for (i = 0; i < uStepsV - 1; i++) {

            pwIndices[uIndex++] = i * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j + 1;
            pwIndices[uIndex++] = i * (uStepsU + 1) + j + 1;
            pwIndices[uIndex++] = i * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j + 1;
        }
    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void ReleaseSphereV(PVERTEX* ppr, LPWORD* ppw) {

    if (ppr && *ppr) {
        MemFree32(*ppr);
    }
    if (ppw && *ppw) {
        delete [] *ppw;
    }
}

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
BOOL ExhibitScene(CDisplay* pDisplay, int* pnExitCode) {

    CClockwork*   pClockwork;
    BOOL          bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pClockwork = new CClockwork();
    if (!pClockwork) {
        return FALSE;
    }

    // Initialize the scene
    if (!pClockwork->Create(pDisplay)) {
        pClockwork->Release();
        return FALSE;
    }

    bRet = pClockwork->Exhibit(pnExitCode);

    // Clean up the scene
    pClockwork->Release();

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
//     LPDIRECT3D8 pd3d                 - Pointer to the Direct3D object
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
BOOL ValidateDisplay(LPDIRECT3D8 pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CClockwork
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CClockwork
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
CClockwork::CClockwork() {

    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;
    m_pd3dtBackground = NULL;
    m_prSphere = NULL;
    m_pwSphere = NULL;
    m_fOrbitAngle = 0.0f;
    memset(&m_cw, 0, sizeof(CLOCKWORK));
}

//******************************************************************************
//
// Method:
//
//     ~CClockwork
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
CClockwork::~CClockwork() {
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
BOOL CClockwork::Create(CDisplay* pDisplay) {

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
int CClockwork::Exhibit(int *pnExitCode) {

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
BOOL CClockwork::Prepare() {

    DWORD dwSeed;

    dwSeed = GetTickCount();
#ifdef LIMITED
    dwSeed = 441428380;
#else
    dwSeed = 2993;
//DebugString(TEXT("seeding pseudo-random number generator with 0x%X"), dwSeed);
#endif // !LIMITED
    srand(dwSeed);

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
BOOL CClockwork::Setup() {

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
BOOL CClockwork::Initialize() {

    SetDirectionalLight(m_pDevice, 0, D3DXVECTOR3(0.0f, -1.0f, 1.0f));
    SetMaterial(m_pDevice, RGB_MAKE(255, 255, 255));

    // Initialize the vertices
    if (!CreateSphereV(BOUND_RADIUS + 8.0f, &m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices)) {
        return FALSE;
    }

    CreateClockwork(m_pDevice, &m_cw);

    m_pd3dtBackground = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("backcogs.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 256, 256);
    if (!m_pd3dtBackground) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3dtBackground->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    m_pd3drSphere = CreateVertexBuffer(m_pDevice, m_prSphere, m_dwSphereVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!m_pd3drSphere) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3drSphere->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }

    m_bFade = TRUE;

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
void CClockwork::Efface() {

    ReleaseVertexBuffer(m_pd3drSphere);
    ReleaseIndexBuffer(m_pd3diSphere);
    ReleaseTexture(m_pd3dtBackground);
    m_pd3dtBackground = NULL;
    ReleaseClockwork(&m_cw);
    ReleaseSphereV(&m_prSphere, &m_pwSphere);

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
void CClockwork::Update() {

    UpdateClockwork(&m_cw, m_fTimeDilation);

    m_fOrbitAngle += (M_2PI / (float)ORBIT_FRAMES * m_fTimeDilation);
    if (m_fOrbitAngle > M_2PI) {
        m_fOrbitAngle -= M_2PI;
    }

    m_cam.vPosition.x = (float)(sin(m_fOrbitAngle)) * VIEW_Z;
    m_cam.vPosition.z = (float)(cos(m_fOrbitAngle)) * VIEW_Z;
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
BOOL CClockwork::Render() {

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

#endif

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView(&m_cam);

    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    m_pDevice->SetTexture(0, m_pd3dtBackground);

#ifndef UNDER_XBOX
    m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
#endif // !UNDER_XBOX

    m_pDevice->SetRenderState(D3DRS_AMBIENT, (DWORD)RGBA_MAKE(255, 255, 255, 255));

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(VERTEX));
    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices,
            0, m_dwSphereIndices / 3);

    m_pDevice->SetRenderState(D3DRS_AMBIENT, (DWORD)RGBA_MAKE(128, 128, 128, 128));

    RenderClockwork(m_pDevice, &m_cw);

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
void CClockwork::ProcessInput() {

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
BOOL CClockwork::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, VIEW_Z);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);
    memcpy(&m_cam, &m_camInitial, sizeof(CAMERA));

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
BOOL CClockwork::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
