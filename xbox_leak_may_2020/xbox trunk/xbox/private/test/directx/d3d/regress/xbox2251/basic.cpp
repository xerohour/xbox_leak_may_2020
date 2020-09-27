/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    basic.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "..\..\inc\d3dlocus.h"
#include "basic.h"

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

    CBasicTest* pTest;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CBasicTest();
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
// CBasicTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CBasicTest
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
CBasicTest::CBasicTest() {

    m_pd3dt = NULL;
    m_pd3drVertices = NULL;
    m_uNumVertices = 4;
    m_mWorld = m_mIdentity;
}

//******************************************************************************
//
// Method:
//
//     ~CBasicTest
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
CBasicTest::~CBasicTest() {
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
BOOL CBasicTest::Create(CDisplay* pDisplay) {

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
int CBasicTest::Exhibit(int *pnExitCode) {

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
BOOL CBasicTest::Setup() {

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
BOOL CBasicTest::Initialize() {

    if (!CScene::Initialize()) {
        return FALSE;
    }

    xSetBackgroundColor(255, 255, 0, 191);

    // Create a texture
    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("dxlogo.bmp"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP, NULL, 1.0f, RGB_MAKE(0, 0, 0));
    if (!m_pd3dt) {
        return FALSE;
    }
#ifdef UNDER_XBOX
    m_pd3dt->GetIDirect3DTexture8()->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

    // Initialize the vertices
    m_prVertices[0] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prVertices[1] = VERTEX(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prVertices[2] = VERTEX(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prVertices[3] = VERTEX(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);

    m_pd3drVertices = CreateVertexBuffer(m_pDevice, m_prVertices, 4 * sizeof(VERTEX), 
                                                            0, FVF_VERTEX);
    if (!m_pd3drVertices) { 
        return FALSE;
    }

    m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

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
void CBasicTest::Efface() {

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
void CBasicTest::Update() {

    D3DXMATRIX mWorld;
    float      fTheta = M_PI / 100.0f;

    if (!m_bCreated) {
        return;
    }

    // Update the rotation
    fTheta *= m_fFrameDelta;
    InitMatrix(&mWorld,
        (float)cos(fTheta), 0.0f, (float)-sin(fTheta), 0.0f,
        0.0f,               1.0f, 0.0f,                0.0f,
        (float)sin(fTheta), 0.0f, (float)cos(fTheta),  0.0f,
        0.0f,               0.0f, 0.0f,                1.0f
    );

    D3DXMatrixMultiply(&m_mWorld, &mWorld, &m_mWorld);
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
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
BOOL CBasicTest::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(255, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Set a texture
    m_pDevice->SetTexture(0, m_pd3dt);
    m_pDevice->SetTexture(1, m_pd3dt);

    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_PREMODULATE);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_PREMODULATE);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drVertices, sizeof(VERTEX));

    // Draw the quad
//    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
//            m_prVertices, sizeof(VERTEX));
    m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    // Release the texture
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);

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

// ##DELETE
// ##HACK to make diffuse modulated blending work on Inspiron 5000
#ifndef UNDER_XBOX
    TLVERTEX rDummy[3];
    rDummy[0] = TLVERTEX(D3DXVECTOR3(0.0f, (float)0.0f, 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 1.0f);
    rDummy[1] = TLVERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 0.0f);
    rDummy[2] = TLVERTEX(D3DXVECTOR3((float)0.0f, 0.0f, 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 0.0f);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, rDummy, sizeof(TLVERTEX));
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
#endif
// ##END HACK
// ##END DELETE

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
void CBasicTest::ProcessInput() {

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
BOOL CBasicTest::InitView() {

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
BOOL CBasicTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
