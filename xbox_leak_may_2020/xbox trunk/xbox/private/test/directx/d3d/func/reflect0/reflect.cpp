/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    reflect.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "reflect.h"

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

    CReflect0*  pReflect;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pReflect = new CReflect0();
    if (!pReflect) {
        return FALSE;
    }

    // Initialize the scene
    if (!pReflect->Create(pDisplay)) {
        pReflect->Release();
        return FALSE;
    }

    bRet = pReflect->Exhibit(pnExitCode);

    // Clean up the scene
    pReflect->Release();

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
// CReflect0
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CReflect0
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
CReflect0::CReflect0() {

    m_pd3drCube = NULL;
    m_pd3diCube = NULL;
    m_pd3dtCube = NULL;
    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CReflect0
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
CReflect0::~CReflect0() {
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
BOOL CReflect0::Create(CDisplay* pDisplay) {

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
int CReflect0::Exhibit(int *pnExitCode) {

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
BOOL CReflect0::Prepare() {

    if (GetStartupContext() & TSTART_HARNESS) {
        SetTimeDuration(13.0f);
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
BOOL CReflect0::Setup() {

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
BOOL CReflect0::Initialize() {

    PCVERTEX    pr;
    LPBYTE      pData;
    PVERTEX     prSphere;
    LPWORD      pwSphere;
    UINT        i;
    HRESULT     hr;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    if (!InitCube(TEXT("face0.bmp"), 80.0f)) {
        return FALSE;
    }

    if (!CreateSphere(&prSphere, &m_dwSphereVertices, &pwSphere, &m_dwSphereIndices, 10.0f, 32, 32)) {
        return FALSE;
    }

    hr = m_pDevice->CreateVertexBuffer(m_dwSphereVertices * sizeof(CVERTEX), 0, FVF_CVERTEX, POOL_DEFAULT, &m_pd3drSphere);
    if (FAILED(hr)) {
        ReleaseSphere(&prSphere, &pwSphere);
        return FALSE;
    }

    hr = m_pDevice->CreateIndexBuffer(m_dwSphereIndices * sizeof(WORD), 0, D3DFMT_INDEX16, POOL_DEFAULT, &m_pd3diSphere);
    if (FAILED(hr)) {
        ReleaseSphere(&prSphere, &pwSphere);
        return FALSE;
    }

    m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(CVERTEX), (LPBYTE*)&pr, 0);

    for (i = 0; i < m_dwSphereVertices; i++) {
        pr[i].vPosition = prSphere[i].vPosition;
        pr[i].vNormal = prSphere[i].vNormal;
    }

    m_pd3drSphere->Unlock();

    m_pd3diSphere->Lock(0, m_dwSphereIndices * sizeof(WORD), &pData, 0);
    memcpy(pData, pwSphere, m_dwSphereIndices * sizeof(WORD));
    m_pd3diSphere->Unlock();

    ReleaseSphere(&prSphere, &pwSphere);

    // Set a material
//    if (!SetMaterial(m_pDevice, RGBA_MAKE(196, 196, 196, 196), RGBA_MAKE(32, 32, 32, 32), RGBA_MAKE(255, 255, 255, 255), 0, 80.0f)) {
//        return FALSE;
//    }

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
void CReflect0::Efface() {

    if (m_pd3drCube) {
        m_pd3drCube->Release();
        m_pd3drCube = NULL;
    }
    if (m_pd3diCube) {
        m_pd3diCube->Release();
        m_pd3diCube = NULL;
    }
    if (m_pd3dtCube) {
        ReleaseTexture(m_pd3dtCube);
        m_pd3dtCube = NULL;
    }
    if (m_pd3drSphere) {
        m_pd3drSphere->Release();
        m_pd3drSphere = NULL;
    }
    if (m_pd3diSphere) {
        m_pd3diSphere->Release();
        m_pd3diSphere = NULL;
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
void CReflect0::Update() {

    if (GetStartupContext() & TSTART_HARNESS) {
        if (m_fTime < 4.0f) {
            m_cam.vPosition.y = (float)sin(m_fTime / 4.0f * M_2PI) * 50.0f;
            m_cam.vPosition.z = (float)cos(m_fTime / 4.0f * M_2PI) * 50.0f;
            if (m_fTime > 3.0f) {
                m_cam.fRoll = 0.0f;
            }
            else if (m_fTime > 1.0f) {
                m_cam.fRoll = M_PI;
            }
        }
        else if (m_fTime < 9.0f) {
            m_cam.vPosition.x = (float)sin(m_fTime / 4.0f * M_2PI) * 50.0f;
            m_cam.vPosition.z = (float)cos(m_fTime / 4.0f * M_2PI) * 50.0f;
        }
        else {
            m_cam.vPosition.x = (float)cos((m_fTime - 9.0f) / 4.0f * M_2PI) * 50.0f;
            m_cam.vPosition.y = (float)sin((m_fTime - 9.0f) / 4.0f * M_2PI) * 50.0f;
            if (m_fTime > 12.0f) {
                m_cam.fRoll = 0.0f;
            }
            else if (m_fTime > 10.0f) {
                m_cam.fRoll = M_PI;
            }
        }
    }
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
BOOL CReflect0::Render() {

    D3DMATRIX   mView, mBackView;
    CAMERA      cam;
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

    if (GetStartupContext() & TSTART_HARNESS) {
        m_pDisplay->SetCamera(&m_cam);
    }

    m_pDisplay->SetView();

    m_pDisplay->GetCamera(&cam);
    SetView((LPD3DXMATRIX)&mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));

    InitMatrix(&mBackView,
        mView._11, mView._21, mView._31, 0.0f,
        mView._12, mView._22, mView._32, 0.0f,
        mView._13, mView._23, mView._33, 0.0f,
        0.0f,      0.0f,      0.0f,      1.0f
    );

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_CUBEVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drCube, sizeof(CUBEVERTEX));
    m_pDevice->SetIndices(m_pd3diCube, 0);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

    m_pDevice->SetTexture(0, m_pd3dtCube);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 8, 0, 12);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetVertexShader(FVF_CVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(CVERTEX));
    m_pDevice->SetIndices(m_pd3diSphere, 0);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTransform(D3DTS_TEXTURE0, &mBackView);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    m_pDevice->SetTransform(D3DTS_TEXTURE0, &m_mIdentity);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

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
void CReflect0::ProcessInput() {

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
BOOL CReflect0::InitView() {

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

    memcpy(&m_cam, &m_camInitial, sizeof(CAMERA));

    return m_pDisplay->SetView(&m_camInitial);
}

//******************************************************************************
BOOL CReflect0::InitCube(LPCTSTR szTexture, float fRadius) {

    PCUBEVERTEX prCube;
    LPWORD      pwCube;
    HRESULT     hr;

    hr = m_pDevice->CreateVertexBuffer(8 * sizeof(CUBEVERTEX), 0, FVF_CUBEVERTEX, POOL_DEFAULT, &m_pd3drCube);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        return FALSE;
    }

    hr = m_pd3drCube->Lock(0, 8 * sizeof(CUBEVERTEX), (LPBYTE*)&prCube, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer::Lock"))) {
        return FALSE;
    }

    // Vertex 0
    prCube[0].vPosition.x  = fRadius;
    prCube[0].vPosition.y  = fRadius;
    prCube[0].vPosition.z  = fRadius;
    prCube[0].u0 = 1.0f;
    prCube[0].v0 = 1.0f;
    prCube[0].t0 = 1.0f;

    // Vertex 1
    prCube[1].vPosition.x  = fRadius;
    prCube[1].vPosition.y  = fRadius;
    prCube[1].vPosition.z  = -fRadius;
    prCube[1].u0 = 1.0f;
    prCube[1].v0 = 1.0f;
    prCube[1].t0 = -1.0f;

    // Vertex 2
    prCube[2].vPosition.x  = -fRadius;
    prCube[2].vPosition.y  = fRadius;
    prCube[2].vPosition.z  = -fRadius;
    prCube[2].u0 = -1.0f;
    prCube[2].v0 = 1.0f;
    prCube[2].t0 = -1.0f;

    // Vertex 3
    prCube[3].vPosition.x  = -fRadius;
    prCube[3].vPosition.y  = fRadius;
    prCube[3].vPosition.z  = fRadius;
    prCube[3].u0 = -1.0f;
    prCube[3].v0 = 1.0f;
    prCube[3].t0 = 1.0f;

    // Vertex 4
    prCube[4].vPosition.x  = fRadius;
    prCube[4].vPosition.y  = -fRadius;
    prCube[4].vPosition.z  = fRadius;
    prCube[4].u0 = 1.0f;
    prCube[4].v0 = -1.0f;
    prCube[4].t0 = 1.0f;

    // Vertex 5
    prCube[5].vPosition.x  = fRadius;
    prCube[5].vPosition.y  = -fRadius;
    prCube[5].vPosition.z  = -fRadius;
    prCube[5].u0 = 1.0f;
    prCube[5].v0 = -1.0f;
    prCube[5].t0 = -1.0f;

    // Vertex 6
    prCube[6].vPosition.x  = -fRadius;
    prCube[6].vPosition.y  = -fRadius;
    prCube[6].vPosition.z  = -fRadius;
    prCube[6].u0 = -1.0f;
    prCube[6].v0 = -1.0f;
    prCube[6].t0 = -1.0f;

    // Vertex 7
    prCube[7].vPosition.x  = -fRadius;
    prCube[7].vPosition.y  = -fRadius;
    prCube[7].vPosition.z  = fRadius;
    prCube[7].u0 = -1.0f;
    prCube[7].v0 = -1.0f;
    prCube[7].t0 = 1.0f;

    hr = m_pd3drCube->Unlock();
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"))) {
        return FALSE;
    }

    // Initialize the indices

    hr = m_pDevice->CreateIndexBuffer(36 * sizeof(WORD), 0, D3DFMT_INDEX16, POOL_DEFAULT, &m_pd3diCube);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateIndexBuffer"))) {
        return FALSE;
    }

    hr = m_pd3diCube->Lock(0, 36 * sizeof(WORD), (LPBYTE*)&pwCube, 0);
    if (ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Lock"))) {
        return FALSE;
    }

    // top face
    pwCube[0]  = 0;
    pwCube[1]  = 2;
    pwCube[2]  = 1;
    pwCube[3]  = 0;
    pwCube[4]  = 3;
    pwCube[5]  = 2;

    //bottom face
    pwCube[6]  = 5;
    pwCube[7]  = 7;
    pwCube[8]  = 4;
    pwCube[9]  = 5;
    pwCube[10] = 6;
    pwCube[11] = 7;

    //front face
    pwCube[12] = 1;
    pwCube[13] = 6;
    pwCube[14] = 5;
    pwCube[15] = 1;
    pwCube[16] = 2;
    pwCube[17] = 6;

    //back face
    pwCube[18] = 3;
    pwCube[19] = 4;
    pwCube[20] = 7;
    pwCube[21] = 3;
    pwCube[22] = 0;
    pwCube[23] = 4;

    //left face
    pwCube[24] = 2;
    pwCube[25] = 7;
    pwCube[26] = 6;
    pwCube[27] = 2;
    pwCube[28] = 3;
    pwCube[29] = 7;

    //right face
    pwCube[30] = 0;
    pwCube[31] = 5;
    pwCube[32] = 4;
    pwCube[33] = 0;
    pwCube[34] = 1;
    pwCube[35] = 5;

    hr = m_pd3diCube->Unlock();
    if (ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Unlock"))) {
        return FALSE;
    }

    m_pd3dtCube = (CTexture8*)CreateTexture(m_pDevice, szTexture, D3DFMT_A8R8G8B8, TTYPE_CUBEMAP);
    if (!m_pd3dtCube) {
        return FALSE;
    }

    return TRUE;
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
BOOL CReflect0::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
