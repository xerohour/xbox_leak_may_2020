/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rteapot.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "rteapot.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define MAX_THETA                  (M_PI / 64.0f)

//******************************************************************************
D3DVECTOR VectorMatrixMultiply(D3DVECTOR* v, D3DXMATRIX* m) {

    D3DVECTOR r;
    r.x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + m->_41;
    r.y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + m->_42;
    r.z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + m->_43;
    return r;
}

//******************************************************************************
void EnvironmentMap(LPDIRECT3DDEVICE8 pDevice, PVERTEX prVertices, UINT uNumVertices) {

    D3DXMATRIX  mWorld;
    D3DXMATRIX  mView;
    D3DXMATRIX  mMap;
    D3DXMATRIX  mWorld2;
    D3DXMATRIX  mView2;
    D3DXVECTOR3 vPoint;
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 vReflect;
    D3DXVECTOR3 vDot;
    D3DXVECTOR3 vT;
    float       fDivisor;
    UINT        i;

    pDevice->GetTransform(D3DTS_WORLD, &mWorld);
    pDevice->GetTransform(D3DTS_VIEW, &mView);

    D3DXMatrixMultiply(&mMap, &mWorld, &mView);

    mWorld2 = mWorld;
    mWorld2._41 = 0.0f;
    mWorld2._42 = 0.0f;
    mWorld2._43 = 0.0f;
    mView2 = mView;
    mView2._41 = 0.0f;
    mView2._42 = 0.0f;
    mView2._43 = 0.0f;

    for (i = 0; i < uNumVertices; i++) {

//        vNormal = D3DVECTOR(prVertices[i].nx, prVertices[i].ny, prVertices[i].nz);
        memcpy(&vNormal, &prVertices[i].vNormal, sizeof(D3DVECTOR));
//        vPoint  = D3DVECTOR(prVertices[i].x, prVertices[i].y, prVertices[i].z);
        memcpy(&vPoint, &prVertices[i].vPosition, sizeof(D3DVECTOR));

//        vPoint = VectorMatrixMultiply(vPoint, mWorld);
        vT = VectorMatrixMultiply(&vPoint, &mMap);
        D3DXVec3Normalize(&vPoint, &vT);
        vNormal = VectorMatrixMultiply(&vNormal, &mWorld2);
        vNormal = VectorMatrixMultiply(&vNormal, &mView2);
        vReflect = 2.0f * D3DXVec3Dot(&vNormal, &vPoint) * vNormal - vPoint;
        fDivisor = 2.0f * (float)(sqrt(vReflect.x * vReflect.x + vReflect.y * vReflect.y + (vReflect.z + 1.0f) * (vReflect.z + 1.0f)));
        prVertices[i].u0 = vReflect.x / fDivisor + 0.5f;
        prVertices[i].v0 = vReflect.y / fDivisor + 0.5f;
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

    CTeapot* pTeapot;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTeapot = new CTeapot();
    if (!pTeapot) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTeapot->Create(pDisplay)) {
        pTeapot->Release();
        return FALSE;
    }

    bRet = pTeapot->Exhibit(pnExitCode);

    // Clean up the scene
    pTeapot->Release();

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
// CTeapot
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTeapot
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
CTeapot::CTeapot() {

    m_mesh.prVertices = NULL;
    m_mesh.pwIndices = NULL;
    m_pd3dt = NULL;
    m_pd3dr = NULL;
    m_pd3di = NULL;
    m_pd3drBack = NULL;
    m_bFade = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CTeapot
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
CTeapot::~CTeapot() {

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
BOOL CTeapot::Create(CDisplay* pDisplay) {

    float   fTheta;
    UINT    i;

    if (!CreateMesh(TEXT("hteapot.xdg"), &m_mesh)) {
        return FALSE;
    }

    if (!CScene::Create(pDisplay)) {
        return FALSE;
    }

    if (NUM_TEAPOTS > 1) {

        for (i = 0; i < NUM_TEAPOTS; i++) {

            fTheta = 2 * MAX_THETA * RND() - MAX_THETA;
            InitMatrix(&m_pmRotation[i],
                (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
                (float)( 0.0f), (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f),
                (float)( 0.0f), (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f),
                (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
            );

            fTheta = 2 * MAX_THETA * RND() - MAX_THETA;
            InitMatrix(&m_pmWorld[i],
                (float)( cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
                (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
                (float)( sin(fTheta)), (float)( 0.0f), (float)( cos(fTheta)), (float)( 0.0f),
                (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
            );
            D3DXMatrixMultiply(&m_pmRotation[i], &m_pmWorld[i], &m_pmRotation[i]);

            fTheta = 2 * MAX_THETA * RND() - MAX_THETA;
            InitMatrix(&m_pmWorld[i],
                (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f), (float)( 0.0f),
                (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f), (float)( 0.0f),
                (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
                (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
            );
            D3DXMatrixMultiply(&m_pmRotation[i], &m_pmWorld[i], &m_pmRotation[i]);

            InitMatrix(&m_pmWorld[i],
                (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
                (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
                (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
                (float)( -10.0f + 20.0f * RND()), (float)( -6.0f + 12.0f * RND()), 
                (float)( 25.0f + 10.0f * RND()), (float)( 1.0f)
            );
        }
    }
    else {

        fTheta = -0.0104847f;//-0.0139796f;
        InitMatrix(&m_pmRotation[0],
            (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f),
            (float)( 0.0f), (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );

        fTheta = 0.036063525f;//0.0480847f;
        InitMatrix(&m_pmWorld[0],
            (float)( cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
            (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( sin(fTheta)), (float)( 0.0f), (float)( cos(fTheta)), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
        D3DXMatrixMultiply(&m_pmRotation[0], &m_pmWorld[0], &m_pmRotation[0]);

        fTheta = -0.0162912f;//-0.0217216f;
        InitMatrix(&m_pmWorld[0],
            (float)( cos(fTheta)), (float)( sin(fTheta)), (float)( 0.0f), (float)( 0.0f),
            (float)(-sin(fTheta)), (float)( cos(fTheta)), (float)( 0.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
        D3DXMatrixMultiply(&m_pmRotation[0], &m_pmWorld[0], &m_pmRotation[0]);

        InitMatrix(&m_pmWorld[0],
            (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
            (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
        );
    }

    return TRUE;
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
int CTeapot::Exhibit(int *pnExitCode) {

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
BOOL CTeapot::Setup() {

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
BOOL CTeapot::Initialize() {

    LPBYTE pData;
    HRESULT hr;
    VERTEX prVertices[4];
    float  x, y, z = 30.0f;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    // Create a material
//    pd3dm = CreateMaterial(g_pd3dDevice, &hMaterial, RGB_MAKE(196, 196, 196), RGB_MAKE(128, 128, 128), RGB_MAKE(255, 255, 255), 0, 20.0f);
    if (!SetMaterial(m_pDevice, RGB_MAKE(196, 196, 196), RGB_MAKE(128, 128, 128), RGB_MAKE(255, 255, 255), 0, 40.0f)) {
        return FALSE;
    }

    // Create an environment texture
    m_pd3dt = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("backgrnd.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 256, 256);
    if (!m_pd3dt) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3dt->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    hr = m_pDevice->CreateVertexBuffer(m_mesh.uNumVertices * sizeof(VERTEX), 0, FVF_VERTEX, D3DPOOL_DEFAULT, &m_pd3dr);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));
        return FALSE;
    }

    hr = m_pd3dr->Lock(0, m_mesh.uNumVertices * sizeof(VERTEX), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    memcpy(pData, m_mesh.prVertices, m_mesh.uNumVertices * sizeof(VERTEX));

    hr = m_pd3dr->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

#ifdef UNDER_XBOX
    m_pd3dr->MoveResourceMemory(D3DMEM_AGP);
#endif

    m_pd3di = CreateIndexBuffer(m_pDevice, m_mesh.pwIndices, m_mesh.uNumIndices * sizeof(WORD));
    if (!m_pd3di) {
        return FALSE;
    }

    x = 19.75f;
    y = x * (480.0f / 640.0f);

    prVertices[0] = VERTEX(D3DXVECTOR3(-x,-y, z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    prVertices[1] = VERTEX(D3DXVECTOR3(-x, y, z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    prVertices[2] = VERTEX(D3DXVECTOR3( x, y, z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    prVertices[3] = VERTEX(D3DXVECTOR3( x,-y, z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);

    m_pd3drBack = CreateVertexBuffer(m_pDevice, m_prBackground, 4 * sizeof(TLVERTEX), 0, FVF_TLVERTEX);
    if (!m_pd3drBack) {
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
void CTeapot::Efface() {

    if (m_pd3dr) {
        m_pd3dr->Release();
    }
    ReleaseIndexBuffer(m_pd3di);
    ReleaseVertexBuffer(m_pd3drBack);
    ReleaseTexture(m_pd3dt);
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
void CTeapot::Update() {

    UINT i; 

    for (i = 0; i < NUM_TEAPOTS; i++) {
        D3DXMatrixMultiply(&m_pmWorld[i], &m_pmRotation[i], &m_pmWorld[i]);
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
BOOL CTeapot::Render() {

    LPBYTE pData;
    UINT     i;
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
    m_pDisplay->SetView();

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dt);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);

    // Draw the background
    m_pDevice->SetStreamSource(0, m_pd3drBack, sizeof(TLVERTEX));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

//    m_pDevice->EndScene();
//    m_pDevice->BeginScene();

//    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(VERTEX));

    m_pDevice->SetIndices(m_pd3di, 0);

    // Draw the teapot(s)
    for (i = 0; i < NUM_TEAPOTS; i++) {

        m_pDevice->SetTransform(D3DTS_WORLD, &m_pmWorld[i]);

        m_pd3dr->Lock(0, m_mesh.uNumVertices * sizeof(VERTEX), &pData, 0);
        EnvironmentMap(m_pDevice, (PVERTEX)pData, m_mesh.uNumVertices);
        m_pd3dr->Unlock();

//        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, 0, m_mesh.uNumIndices / 6);
//        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, m_mesh.uNumIndices / 2, m_mesh.uNumIndices / 6);
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, 0, m_mesh.uNumIndices / 3);
//        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices, m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, m_mesh.prVertices, sizeof(VERTEX));
    }

    m_pDevice->SetTexture(0, NULL);

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
void CTeapot::ProcessInput() {

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
BOOL CTeapot::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -15.0f);
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
BOOL CTeapot::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
