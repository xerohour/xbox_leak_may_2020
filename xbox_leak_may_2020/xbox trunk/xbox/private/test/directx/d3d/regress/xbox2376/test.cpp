/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "test.h"

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

    CTest*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CTest();
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
// CTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTest
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
CTest::CTest() {

    m_prSphere = NULL;
    m_pwSphere = NULL;
    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;

}

//******************************************************************************
//
// Method:
//
//     ~CTest
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
CTest::~CTest() {

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
BOOL CTest::Create(CDisplay* pDisplay) {

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
int CTest::Exhibit(int *pnExitCode) {

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
BOOL CTest::Prepare() {

    if (!CreateSphere(&m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices, 10.0f, 32, 32)) {
        return FALSE;
    }

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
BOOL CTest::Setup() {

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
BOOL CTest::Initialize() {

    PCLVERTEX  prVertices;
    D3DLIGHT8 light;
    UINT      i;
    HRESULT   hr;

    if (!m_bCreated) {
        return FALSE;
    }

    // Create a directional light
    memset(&light, 0, sizeof(D3DLIGHT8));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Diffuse.a = 1.0f;
    light.Specular.r = 1.0f;
    light.Specular.g = 1.0f;
    light.Specular.b = 1.0f;
    light.Specular.a = 1.0f;
    light.Ambient.r = 1.0f;
    light.Ambient.g = 1.0f;
    light.Ambient.b = 1.0f;
    light.Ambient.a = 1.0f;
    light.Direction = D3DXVECTOR3(0.0f, -1.0f, 1.0f);

    hr = m_pDevice->SetLight(0, &light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    // Set a material
    if (!SetMaterial(m_pDevice, RGBA_MAKE(196, 196, 196, 196), 0, RGBA_MAKE(255, 255, 255, 255), 0, 40.0f)) {
        return FALSE;
    }

    m_pd3drSphere = CreateVertexBuffer(m_pDevice, NULL, m_dwSphereVertices * sizeof(CLVERTEX), 0, FVF_CLVERTEX);
    if (!m_pd3drSphere) {
        return FALSE;
    }

    m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(CLVERTEX), (LPBYTE*)&prVertices, 0);

    for (i = 0; i < m_dwSphereVertices; i++) {
        prVertices[i].vPosition = m_prSphere[i].vPosition;
        prVertices[i].vNormal = m_prSphere[i].vNormal;
        prVertices[i].cDiffuse = RGBA_MAKE(255, 0, 0, 255);
        prVertices[i].cSpecular = RGBA_MAKE(0, 0, 255, 0);
    }

    m_pd3drSphere->Unlock();

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
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
void CTest::Efface() {

    ReleaseVertexBuffer(m_pd3drSphere);
    ReleaseIndexBuffer(m_pd3diSphere);
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
void CTest::Update() {

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
BOOL CTest::Render() {

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
    m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_CLVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(CLVERTEX));
    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

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
void CTest::ProcessInput() {

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
BOOL CTest::InitView() {

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
BOOL CTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
