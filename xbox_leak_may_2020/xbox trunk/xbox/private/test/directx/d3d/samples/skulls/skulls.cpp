/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    skulls.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "skulls.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define BRIDGE_LENGTH       1.75f
#define EYE_LENGTH          2.0f
#define POS_AMPLITUDE       (BRIDGE_LENGTH / 2 + EYE_LENGTH / 2)
#define SKULL_DISTANCE      15.0f
#define DISTANCE_PER_FRAME  0.25f//0.09375f//0.125f
#define MAX_THETA           ((float)atan2((double)POS_AMPLITUDE, \
                            (double)SKULL_DISTANCE))
#define FRONT_CLIP

#ifndef FRONT_CLIP
#define CLIP_DELTA          4
#else
#define CLIP_DELTA          0
#endif // FRONT_CLIP

#ifndef UNDER_XBOX
#ifdef FRONT_CLIP
#define SET_CLIPPING_STATE(x)   m_pDevice->SetRenderState(D3DRS_CLIPPING, (x))
#else
#define SET_CLIPPING_STATE(x)   m_pDevice->SetRenderState(D3DRS_CLIPPING, FALSE)
#endif // FRONT_CLIP
#else
#define SET_CLIPPING_STATE(x)
#endif // UNDER_XBOX


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

    CSkulls*    pSkulls;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pSkulls = new CSkulls();
    if (!pSkulls) {
        return FALSE;
    }

    // Initialize the scene
    if (!pSkulls->Create(pDisplay)) {
        pSkulls->Release();
        return FALSE;
    }

    bRet = pSkulls->Exhibit(pnExitCode);

    // Clean up the scene
    pSkulls->Release();

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
// CSkulls
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CSkulls
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
CSkulls::CSkulls() {

    m_pd3dr = NULL;
    m_pd3drScreen = NULL;
    m_pd3di = NULL;
    m_plr = NULL;
    m_mesh.prVertices = NULL;
    m_mesh.pwIndices = NULL;
    m_pmView = NULL;
    m_j = 0;
    m_bAdvanceView = FALSE;
    m_bFade = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CSkulls
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
CSkulls::~CSkulls() {

    MemFree32(m_plr);
    ReleaseMesh(&m_mesh);
    MemFree(m_pmView);
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
BOOL CSkulls::Create(CDisplay* pDisplay) {

    D3DMATERIAL8                material;
    D3DLIGHT8                   light;
    UINT                        i;
    D3DXMATRIX                  m;
    float                       fTheta,
                                fScale,
                                z;

    if (m_bCreated || m_uCreated > 1) {
        return FALSE;
    }

    if (!pDisplay) {
        return FALSE;
    }

    m_uSteps = 2 * (UINT)((SKULL_DISTANCE / DISTANCE_PER_FRAME) + 0.5f);

    // Precalculate camera motion
    m_pmView = (D3DXMATRIX*)MemAlloc(m_uSteps * sizeof(D3DXMATRIX));
    z = -CLIP_DELTA * DISTANCE_PER_FRAME;

    for (i = 0; i < m_uSteps; i++) {

        fScale = (float)sin((double)(M_2PI * (z / (2.0f * SKULL_DISTANCE))));

        // Orient the camera
        fTheta = fScale * MAX_THETA;

        InitMatrix(&m_pmView[i],
             (float)cos(fTheta),  0.0f, 
             (float)-sin(fTheta), 0.0f,
             0.0f,       1.0f, 
             0.0f,       0.0f,
             (float)sin(fTheta), 0.0f, 
             (float)cos(fTheta), 0.0f,
             0.0f,       0.0f, 
             0.0f,       1.0f
        );

        // Transpose the camera
        InitMatrix(&m,
            (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
            (float)( -fScale * POS_AMPLITUDE), (float)( 0.5), 
            (float)(  -z), (float)( 1.0)
        );

        D3DXMatrixMultiply(&m_pmView[i], &m, &m_pmView[i]);

        z -= DISTANCE_PER_FRAME;
    }

    for (i = 0; i < 3; i++) {

        InitMatrix(&m_mWorld[i],
            (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
            (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
            (float)( 0.0), (float)( 0.0), 
            (float)( SKULL_DISTANCE / 2.0f - i * SKULL_DISTANCE), 
            (float)( 1.0)
        );
    }

    // Initialize the skull vertices
    if (!CreateMesh(TEXT("skull.xdg"), &m_mesh)) {
        return FALSE;
    }

    // Initialize a bone material
    material.Diffuse.r =  0.78f;
    material.Diffuse.g =  0.78f;
    material.Diffuse.b =  0.78f;
    material.Ambient.r =  0.0f;
    material.Ambient.g =  0.0f;
    material.Ambient.b =  0.0f;
    material.Specular.r = 0.5f;
    material.Specular.g = 0.5f;
    material.Specular.b = 0.5f;
    material.Emissive.r = 0.0f;
    material.Emissive.g = 0.0f;
    material.Emissive.b = 0.0f;
    material.Power =      20.0f;

    SetDirectionalLight(&light, D3DXVECTOR3(0.0f, -1.0f, 1.0f), 0xFFFFFFFF);

    m_plr = (PLVERTEX)MemAlloc32(m_mesh.uNumVertices * sizeof(LVERTEX));
    if (!m_plr) {
        return FALSE;
    }

    for (i = 0; i < m_mesh.uNumVertices; i++) {
        m_plr[i].vPosition = m_mesh.prVertices[i].vPosition;
    }

    // Prelight the skull
    if (!LightVertices(m_mesh.prVertices, m_plr, m_mesh.uNumVertices, &material, &light, 0, &D3DXVECTOR3(0.0f, 0.0f, -10.0f))) {
        return FALSE;
    }

    // Initialize the screen vertices
    m_plrScreen[0][0] = LVERTEX(D3DXVECTOR3(-2.378f,-1.743f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][1] = LVERTEX(D3DXVECTOR3(-2.378f, 0.632f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][2] = LVERTEX(D3DXVECTOR3( 0.000f,-1.743f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[0][3] = LVERTEX(D3DXVECTOR3( 0.000f, 0.632f, 1.0f), 0, 0, 0.0f, 0.0f);

    m_plrScreen[1][0] = LVERTEX(D3DXVECTOR3( 0.000f,-1.743f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][1] = LVERTEX(D3DXVECTOR3( 0.000f, 0.632f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][2] = LVERTEX(D3DXVECTOR3( 2.378f,-1.743f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[1][3] = LVERTEX(D3DXVECTOR3( 2.378f, 0.632f, 1.0f), 0, 0, 0.0f, 0.0f);

    m_plrScreen[2][0] = LVERTEX(D3DXVECTOR3(-1.000f,-3.229f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][1] = LVERTEX(D3DXVECTOR3(-0.750f,-1.525f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][2] = LVERTEX(D3DXVECTOR3( 1.000f,-3.229f, 1.0f), 0, 0, 0.0f, 0.0f);
    m_plrScreen[2][3] = LVERTEX(D3DXVECTOR3( 0.750f,-1.525f, 1.0f), 0, 0, 0.0f, 0.0f);

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
int CSkulls::Exhibit(int *pnExitCode) {

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
BOOL CSkulls::Setup() {

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
BOOL CSkulls::Initialize() {

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

    m_pd3dr = CreateVertexBuffer(m_pDevice, m_plr, m_mesh.uNumVertices * sizeof(LVERTEX), 0, FVF_LVERTEX);
#ifdef UNDER_XBOX
    m_pd3dr->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    m_pd3di = CreateIndexBuffer(m_pDevice, m_mesh.pwIndices, m_mesh.uNumIndices * sizeof(WORD));
    m_pd3drScreen = CreateVertexBuffer(m_pDevice, m_plrScreen, 12 * sizeof(LVERTEX), 0, FVF_LVERTEX);
#ifdef UNDER_XBOX
    m_pd3drScreen->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX
    return (m_pd3dr && m_pd3di && m_pd3drScreen);
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
void CSkulls::Efface() {

    ReleaseVertexBuffer(m_pd3dr);
    ReleaseIndexBuffer(m_pd3di);
    ReleaseVertexBuffer(m_pd3drScreen);
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
void CSkulls::Update() {

    int j = (int)m_j;
    int delta;

    delta = (int)m_fFrameDelta;
    if (delta < 1) {
        delta = 1;
    }

    if (m_bAdvanceView) {
        j -= delta;
        if (j < 0) {
            j += 2 * (int)(SKULL_DISTANCE / DISTANCE_PER_FRAME);
        }
    }
    else {
        j += delta;
        if (j >= 2 * (int)(SKULL_DISTANCE / DISTANCE_PER_FRAME)) {
            j -= 2 * (int)(SKULL_DISTANCE / DISTANCE_PER_FRAME);
        }
    }

    m_j = (UINT)j;
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
BOOL CSkulls::Render() {

    UINT        uNumSkullPrim = m_mesh.uNumIndices / 3;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Disable lighting
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_LVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(LVERTEX));

    m_pDevice->SetIndices(m_pd3di, 0);

    m_pDevice->SetTransform(D3DTS_VIEW, &m_pmView[m_j]);

    // Draw the skulls
    if (m_j < m_uSteps / 2) {

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld[0]);

        SET_CLIPPING_STATE(m_j < m_uSteps / 4);

        // Render
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, 0, uNumSkullPrim / 2);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, m_mesh.uNumIndices / 2, uNumSkullPrim / 2);
    }

    if (!(m_j < m_uSteps / 4)) { 

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld[1]);

        SET_CLIPPING_STATE(m_j < m_uSteps / 2);

        // Render
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, 0, uNumSkullPrim / 2);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, m_mesh.uNumIndices / 2, uNumSkullPrim / 2);
    }


    if (!(m_j < 3 * m_uSteps / 4)) {

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld[2]);

        SET_CLIPPING_STATE(TRUE);

        // Render
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, 0, uNumSkullPrim / 2);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                0, m_mesh.uNumVertices, m_mesh.uNumIndices / 2, uNumSkullPrim / 2);
    }

    // Draw the screens
    m_pDevice->SetStreamSource(0, m_pd3drScreen, sizeof(LVERTEX));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
            8, 2);

    if (m_j > m_uSteps / 4 && m_j < 3 * m_uSteps / 4) {

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld[1]);

        m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
                4, 2);
    }
    else {

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld[(m_j < m_uSteps / 4)?0:2]);

        m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 
                0, 2);
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
void CSkulls::ProcessInput() {

    CScene::ProcessInput();

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_bAdvanceView = !m_bAdvanceView;
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
BOOL CSkulls::InitView() {

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
BOOL CSkulls::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

        case WM_KEYUP:

            switch (wParam) {

                case VK_SPACE:

                    if (!m_bPaused) {
                        m_bAdvanceView = !m_bAdvanceView;
                        return TRUE;
                    }
                    break;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
