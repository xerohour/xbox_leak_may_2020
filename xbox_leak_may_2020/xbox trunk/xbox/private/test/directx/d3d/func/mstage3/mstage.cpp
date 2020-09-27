/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mstage.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "mstage.h"

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

    CMStage3* pMStage;
    BOOL      bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMStage = new CMStage3();
    if (!pMStage) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMStage->Create(pDisplay)) {
        pMStage->Release();
        return FALSE;
    }

    bRet = pMStage->Exhibit(pnExitCode);

    // Clean up the scene
    pMStage->Release();

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
_MVERTEX3::_MVERTEX3(const D3DXVECTOR3& v, const D3DXVECTOR3& n, float _u0, 
                   float _v0, float _u1, float _v1, 
                   float _u2, float _v2, float _u3, float _v3)
{
    vPosition = v;
    vNormal = n;
    u0 = _u0;
    v0 = _v0;
    u1 = _u1;
    v1 = _v1;
    u2 = _u2;
    v2 = _v2;
    u3 = _u3;
    v3 = _v3;
}

//******************************************************************************
// CMStage3
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMStage3
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
CMStage3::CMStage3() {

    UINT i;

    // Initialize the vertices
    m_prVertices[0] = MVERTEX3(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    m_prVertices[1] = MVERTEX3(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_prVertices[2] = MVERTEX3(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    m_prVertices[3] = MVERTEX3(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    for (i = 0; i < 8; i++) {
        m_mTexture[i] = m_mIdentity;
    }

    m_pd3dtBackgrnd = NULL;
    memset(m_pd3dt, 0, 8 * sizeof(CTexture8*));
}

//******************************************************************************
//
// Method:
//
//     ~CMStage3
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
CMStage3::~CMStage3() {
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
BOOL CMStage3::Create(CDisplay* pDisplay) {

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
int CMStage3::Exhibit(int *pnExitCode) {

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
BOOL CMStage3::Prepare() {

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
BOOL CMStage3::Setup() {

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
BOOL CMStage3::Initialize() {

    UINT i;

    // Create textures
    m_pd3dtBackgrnd = (CTexture8*)CreateTexture(m_pDevice, TEXT("backgrnd.bmp"), D3DFMT_R5G6B5);
    if (!m_pd3dtBackgrnd) {
        return FALSE;
    }

    m_pd3dt[0] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture1.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[1] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture2.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[2] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture3.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[3] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture4.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[4] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture5.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[5] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture6.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[6] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture7.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.95f);
    m_pd3dt[7] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture8.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.95f);

    for (i = 0; i < 8; i++) {
        if (!m_pd3dt[i]) {
            return FALSE;
        }
    }

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    for (i = 1; i < 8; i++) {
        SetColorStage(m_pDevice, i, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
        SetAlphaStage(m_pDevice, i, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    }

    for (i = 0; i < 8; i++) {
        m_pDevice->SetTexture(i, m_pd3dt[i]);
    }

    for (i = 0; i < 4; i++) {
    	m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
    	m_pDevice->SetTextureStageState(i+4, D3DTSS_TEXCOORDINDEX, i);
    }

    for (i = 0; i < 8; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    }

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
void CMStage3::Efface() {

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
void CMStage3::Update() {

    float fOffset = 0.005f * m_fTimeDilation;

    m_mTexture[0]._32 += fOffset;
    m_mTexture[1]._31 += fOffset;
    m_mTexture[2]._32 -= fOffset;
    m_mTexture[3]._31 -= fOffset;

    if (m_mTexture[0]._32 > 1.0f) {
        m_mTexture[0]._32 -= 1.0f;
    }
    if (m_mTexture[1]._31 > 1.0f) {
        m_mTexture[1]._31 -= 1.0f;
    }
    if (m_mTexture[2]._32 < 0.0f) {
        m_mTexture[2]._32 += 1.0f;
    }
    if (m_mTexture[3]._31 < 0.0f) {
        m_mTexture[3]._31 += 1.0f;
    }

    m_mTexture[4]._32 = m_mTexture[0]._32;
    m_mTexture[5]._31 = m_mTexture[1]._31;
    m_mTexture[6]._32 = m_mTexture[2]._32;
    m_mTexture[7]._31 = m_mTexture[3]._31;
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
BOOL CMStage3::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;
    UINT     i;

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
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dtBackgrnd);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    m_pDevice->SetVertexShader(FVF_MVERTEX3);

    for (i = 0; i < 8; i++) {
        m_pDevice->SetTransform((D3DTRANSFORMSTATETYPE)((UINT)D3DTS_TEXTURE0 + i), &m_mTexture[i]);
    }

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetTexture(0, m_pd3dt[0]);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    // Render
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(MVERTEX3));

/*
    // Update the texture coordinates
    for (i = 0; i < 4; i++) {
        prVertices[i].v0 += 0.005f;
        prVertices[i].u1 += 0.005f;
        prVertices[i].v2 -= 0.005f;
        prVertices[i].u3 -= 0.005f;
    }
    if (prVertices[0].v0 > 1.0f) {
        for (i = 0; i < 4; i++) {
            prVertices[i].v0 -= 1.0f;
        }
    }
    if (prVertices[0].u1 > 1.0f) {
        for (i = 0; i < 4; i++) {
            prVertices[i].u1 -= 1.0f;
        }
    }
    if (prVertices[0].v2 < 1.0f) {
        for (i = 0; i < 4; i++) {
            prVertices[i].v2 += 1.0f;
        }
    }
    if (prVertices[0].u3 < 1.0f) {
        for (i = 0; i < 4; i++) {
            prVertices[i].u3 += 1.0f;
        }
    }
*/

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
void CMStage3::ProcessInput() {

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
BOOL CMStage3::InitView() {

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
BOOL CMStage3::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
