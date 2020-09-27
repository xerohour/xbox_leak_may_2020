/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    stencil.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "stencil.h"

#define SHADOW_STENCIL_FLAG 0x20

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

    CStencil0*  pStencil;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pStencil = new CStencil0();
    if (!pStencil) {
        return FALSE;
    }

    // Initialize the scene
    if (!pStencil->Create(pDisplay)) {
        pStencil->Release();
        return FALSE;
    }

    bRet = pStencil->Exhibit(pnExitCode);

    // Clean up the scene
    pStencil->Release();

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
void MatrixShadow(D3DXMATRIX* pm, D3DXVECTOR4* pvPlane, D3DXVECTOR4* pvLight) {

    float fDot = D3DXVec4Dot(pvPlane, pvLight);

    InitMatrix(pm,
        fDot - pvPlane->x * pvLight->x,
             - pvPlane->x * pvLight->y,
             - pvPlane->x * pvLight->z,
             - pvPlane->x * pvLight->w,
             - pvPlane->y * pvLight->x,
        fDot - pvPlane->y * pvLight->y,
             - pvPlane->y * pvLight->z,
             - pvPlane->y * pvLight->w,
             - pvPlane->z * pvLight->x,
             - pvPlane->z * pvLight->y,
        fDot - pvPlane->z * pvLight->z,
             - pvPlane->z * pvLight->w,
             - pvPlane->w * pvLight->x,
             - pvPlane->w * pvLight->y,
             - pvPlane->w * pvLight->z,
        fDot - pvPlane->w * pvLight->w
    );
}

//******************************************************************************
// CStencil0
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CStencil0
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
CStencil0::CStencil0() {

    m_pd3dt = NULL;
    m_pd3dr = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CStencil0
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
CStencil0::~CStencil0() {
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
BOOL CStencil0::Create(CDisplay* pDisplay) {

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
int CStencil0::Exhibit(int *pnExitCode) {

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
BOOL CStencil0::Prepare() {

    float fTheta;

    fTheta = -0.0104847f;//-0.0139796f;
    InitMatrix(&m_mRotation,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    fTheta = 0.036063525f;//0.0480847f;
    InitMatrix(&m_mWorld,
        (float)( cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( sin(fTheta)), (float)( 0.0f), (float)( cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );
    D3DXMatrixMultiply(&m_mRotation, &m_mWorld, &m_mRotation);

    fTheta = -0.0162912f;//-0.0217216f;
    InitMatrix(&m_mWorld,
        (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f), (float)( 0.0f),
        (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );
    D3DXMatrixMultiply(&m_mRotation, &m_mWorld, &m_mRotation);

    InitMatrix(&m_mWorld,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    m_vLightPos = D3DXVECTOR4(7.5f, 15.0f, -7.5f, 1.0f);

    MatrixShadow(&m_mShadow, &D3DXVECTOR4(0.0f, 1.0f, 0.0f, 21.0f), &m_vLightPos);

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
BOOL CStencil0::Setup() {

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
BOOL CStencil0::Initialize() {

    PVERTEX prVertices;
    D3DXMATRIX m;
    UINT i;
    D3DLIGHT8 light;
    HRESULT   hr;

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
    light.Direction = D3DXVECTOR3(0.0f, -11.0f, 0.0f) - *(D3DXVECTOR3*)&m_vLightPos;

    hr = m_pDevice->SetLight(0, &light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    SetMaterial(&m_matObject, RGBA_MAKE(196, 196, 196, 255), RGBA_MAKE(32, 32, 32, 255), RGBA_MAKE(255, 255, 255, 255));
    SetMaterial(&m_matShadow, RGBA_MAKE(32, 32, 32, 196), RGBA_MAKE(32, 32, 32, 196), 0);

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"));
    if (!m_pd3dt) {
        return FALSE;
    }

    m_pd3dr = CreateVertexBuffer(m_pDevice, NULL, 3 * 4 * 6 * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!m_pd3dr) {
        return FALSE;
    }

    m_pd3dr->Lock(0, 0, (LPBYTE*)&prVertices, 0);

    prVertices[0].vPosition = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
    prVertices[1].vPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
    prVertices[2].vPosition = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
    prVertices[3].vPosition = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
    prVertices[4].vPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
    prVertices[5].vPosition = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    prVertices[6].vPosition = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    prVertices[7].vPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
    prVertices[8].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
    prVertices[9].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
    prVertices[10].vPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
    prVertices[11].vPosition = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);

    for (i = 0; i < 12; i += 3) {
        D3DXVec3Cross(&prVertices[i].vNormal, &(prVertices[i+2].vPosition - prVertices[i+1].vPosition), &(prVertices[i].vPosition - prVertices[i+1].vPosition));
        D3DXVec3Normalize(&prVertices[i].vNormal, &prVertices[i].vNormal);
        prVertices[i+1].vNormal = prVertices[i].vNormal;
        prVertices[i+2].vNormal = prVertices[i].vNormal;
    }

    D3DXMatrixIdentity(&m);
    m._11 = (float)cos(M_PIDIV2);
    m._12 = (float)sin(M_PIDIV2);
    m._21 = -m._12;
    m._22 = m._11;

    for (i = 36; i < 48; i++) {
        D3DXVec3TransformCoord(&prVertices[i-24].vPosition, &prVertices[i-36].vPosition, &m);
        D3DXVec3TransformCoord(&prVertices[i-24].vNormal, &prVertices[i-36].vNormal, &m);
        D3DXVec3TransformCoord(&prVertices[i-12].vPosition, &prVertices[i-24].vPosition, &m);
        D3DXVec3TransformCoord(&prVertices[i-12].vNormal, &prVertices[i-24].vNormal, &m);
        D3DXVec3TransformCoord(&prVertices[i].vPosition, &prVertices[i-12].vPosition, &m);
        D3DXVec3TransformCoord(&prVertices[i].vNormal, &prVertices[i-12].vNormal, &m);
    }

    D3DXMatrixIdentity(&m);
    m._22 = (float)cos(M_PIDIV2);
    m._23 = (float)sin(M_PIDIV2);
    m._32 = -m._23;
    m._33 = m._22;

    for (i = 48; i < 60; i++) {
        D3DXVec3TransformCoord(&prVertices[i].vPosition, &prVertices[i-48].vPosition, &m);
        D3DXVec3TransformCoord(&prVertices[i].vNormal, &prVertices[i-48].vNormal, &m);
    }

    m._22 = (float)cos(-M_PIDIV2);
    m._23 = (float)sin(-M_PIDIV2);
    m._32 = -m._23;
    m._33 = m._22;

    for (i = 60; i < 72; i++) {
        D3DXVec3TransformCoord(&prVertices[i].vPosition, &prVertices[i-60].vPosition, &m);
        D3DXVec3TransformCoord(&prVertices[i].vNormal, &prVertices[i-60].vNormal, &m);
    }

    m_pd3dr->Unlock();

    m_prQuad[0] = VERTEX(D3DXVECTOR3(-40.0f, -21.0f, -40.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f);
    m_prQuad[1] = VERTEX(D3DXVECTOR3(-40.0f, -21.0f,  40.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prQuad[2] = VERTEX(D3DXVECTOR3( 40.0f, -21.0f,  40.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f);
    m_prQuad[3] = VERTEX(D3DXVECTOR3( 40.0f, -21.0f, -40.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);

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
void CStencil0::Efface() {

    ReleaseTexture(m_pd3dt);
    ReleaseVertexBuffer(m_pd3dr);
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
void CStencil0::Update() {

    D3DXMatrixMultiply(&m_mWorld, &m_mRotation, &m_mWorld);
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
BOOL CStencil0::Render() {

    D3DXMATRIX mShadow;
    CAMERA cam;
    HRESULT hr;

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
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, RGB_MAKE(255, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->LightEnable(0, TRUE);
    m_pDevice->SetMaterial(&m_matObject);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(VERTEX));

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 24);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);

    m_pDevice->SetTexture(0, m_pd3dt);

    m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INVERT);
    m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, SHADOW_STENCIL_FLAG);
    m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prQuad, sizeof(VERTEX));

    m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

    m_pDevice->SetTexture(0, NULL);

    D3DXMatrixMultiply(&mShadow, &m_mWorld, &m_mShadow);
    m_pDevice->SetTransform(D3DTS_WORLD, &mShadow);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    m_pDevice->LightEnable(0, FALSE);
    m_pDevice->SetMaterial(&m_matShadow);

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(VERTEX));

    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

    m_pDevice->SetRenderState(D3DRS_STENCILREF, SHADOW_STENCIL_FLAG);
//    m_pDevice->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF); // Default
    m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
//    m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INVERT); // Already set
//    m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP); // Default
//    m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); // Default
//    m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, SHADOW_STENCIL_FLAG); // Already set
    m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 24);

    m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

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
void CStencil0::ProcessInput() {

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
BOOL CStencil0::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(11.09f, 21.58f, -49.24f);
    m_camInitial.vInterest     = D3DXVECTOR3(-1.65f, -3.14f, -7.68f);
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
BOOL CStencil0::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
