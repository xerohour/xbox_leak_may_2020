/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    obelisk.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "obelisk.h"

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

    CObelisk*   pObelisk;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pObelisk = new CObelisk();
    if (!pObelisk) {
        return FALSE;
    }

    // Initialize the scene
    if (!pObelisk->Create(pDisplay)) {
        pObelisk->Release();
        return FALSE;
    }

    bRet = pObelisk->Exhibit(pnExitCode);

    // Clean up the scene
    pObelisk->Release();

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
// CObelisk
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CObelisk
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
CObelisk::CObelisk() {

    m_pd3dtGround = NULL;
    m_pd3dtClouds = NULL;
    m_pd3dtObelisk = NULL;
    m_fAlpha = 0.0f;
    m_fOffset = 0.0f;
    m_dAngleOfRotation = 0.0;
    m_bFade = TRUE;
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
}

//******************************************************************************
//
// Method:
//
//     ~CObelisk
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
CObelisk::~CObelisk() {
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
BOOL CObelisk::Create(CDisplay* pDisplay) {

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
int CObelisk::Exhibit(int *pnExitCode) {

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
BOOL CObelisk::Prepare() {

    D3DXVECTOR3 v1, v2, n1, n2, n3, n4;
    D3DXMATRIX  mWorld;
    UINT        i;

    // Initialize all vertices in the scene
    v1 = D3DXVECTOR3(0.06f, -0.15f, -0.06f);
    v2 = D3DXVECTOR3(-0.06f, -0.15f, -0.06f);
    D3DXVec3Cross(&n1, &v1, &v2);
    D3DXVec3Normalize(&n1, &n1);
    v1 = D3DXVECTOR3(0.06f, -0.15f, 0.06f);
    v2 = D3DXVECTOR3(0.06f, -0.15f, -0.06f);
    D3DXVec3Cross(&n2, &v1, &v2);
    D3DXVec3Normalize(&n2, &n2);
    v1 = D3DXVECTOR3(-0.06f, -0.15f, 0.06f);
    v2 = D3DXVECTOR3(0.06f, -0.15f, 0.06f);
    D3DXVec3Cross(&n3, &v1, &v2);
    D3DXVec3Normalize(&n3, &n3);
    v1 = D3DXVECTOR3(-0.06f, -0.15f, -0.06f);
    v2 = D3DXVECTOR3(-0.06f, -0.15f, 0.06f);
    D3DXVec3Cross(&n4, &v1, &v2);
    D3DXVec3Normalize(&n4, &n4);

    // Initialize the obelisk crown vertices
    m_prObeliskTip[0]  = VERTEX(D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
                                 D3DXVECTOR3(n1.x, n1.y, n1.z), 0.0f, 0.0f);
    m_prObeliskTip[1]  = VERTEX(D3DXVECTOR3(0.06f, 0.85f, -0.06f), 
                                 D3DXVECTOR3(n1.x, n1.y, n1.z), 0.0f, 0.0f);
    m_prObeliskTip[2]  = VERTEX(D3DXVECTOR3(-0.06f, 0.85f, -0.06f), 
                                 D3DXVECTOR3(n1.x, n1.y, n1.z), 0.0f, 0.0f);

    m_prObeliskTip[3]  = VERTEX(D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
                                 D3DXVECTOR3(n2.x, n2.y, n2.z), 0.0f, 0.0f);
    m_prObeliskTip[4]  = VERTEX(D3DXVECTOR3(0.06f, 0.85f,  0.06f), 
                                 D3DXVECTOR3(n2.x, n2.y, n2.z), 0.0f, 0.0f);
    m_prObeliskTip[5]  = VERTEX(D3DXVECTOR3(0.06f, 0.85f, -0.06f), 
                                 D3DXVECTOR3(n2.x, n2.y, n2.z), 0.0f, 0.0f);

    m_prObeliskTip[6]  = VERTEX(D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
                                 D3DXVECTOR3(n3.x, n3.y, n3.z), 0.0f, 0.0f);
    m_prObeliskTip[7]  = VERTEX(D3DXVECTOR3(-0.06f, 0.85f, 0.06f), 
                                 D3DXVECTOR3(n3.x, n3.y, n3.z), 0.0f, 0.0f);
    m_prObeliskTip[8]  = VERTEX(D3DXVECTOR3(0.06f, 0.85f, 0.06f), 
                                 D3DXVECTOR3(n3.x, n3.y, n3.z), 0.0f, 0.0f);

    m_prObeliskTip[9]  = VERTEX(D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
                                 D3DXVECTOR3(n4.x, n4.y, n4.z), 0.0f, 0.0f);
    m_prObeliskTip[10] = VERTEX(D3DXVECTOR3(-0.06f, 0.85f, -0.06f), 
                                 D3DXVECTOR3(n4.x, n4.y, n4.z), 0.0f, 0.0f);
    m_prObeliskTip[11] = VERTEX(D3DXVECTOR3(-0.06f, 0.85f, 0.06f), 
                                 D3DXVECTOR3(n4.x, n4.y, n4.z), 0.0f, 0.0f);

    // Initialize the obelisk trunk vertices
    m_prObeliskTrunk[0]  = VERTEX(D3DXVECTOR3(-0.060f,  0.85f, -0.060f), 
                                   D3DXVECTOR3( 0.0f, 0.0f, -1.0f), 0.05f, 0.0f);
    m_prObeliskTrunk[1]  = VERTEX(D3DXVECTOR3( 0.060f,  0.85f, -0.060f), 
                                   D3DXVECTOR3( 0.0f, 0.0f, -1.0f), 0.20f, 0.0f);
    m_prObeliskTrunk[2]  = VERTEX(D3DXVECTOR3( 0.135f, -0.95f, -0.135f), 
                                   D3DXVECTOR3( 0.0f, 0.0f, -1.0f), 0.25f, 1.0f);
    m_prObeliskTrunk[3]  = VERTEX(D3DXVECTOR3(-0.135f, -0.95f, -0.135f), 
                                   D3DXVECTOR3( 0.0f, 0.0f, -1.0f), 0.00f, 1.0f);

    m_prObeliskTrunk[4]  = VERTEX(D3DXVECTOR3( 0.060f,  0.85f, -0.060f), 
                                   D3DXVECTOR3( 1.0f, 0.0f,  0.0f), 0.30f, 0.0f);
    m_prObeliskTrunk[5]  = VERTEX(D3DXVECTOR3( 0.060f,  0.85f,  0.060f), 
                                   D3DXVECTOR3( 1.0f, 0.0f,  0.0f), 0.45f, 0.0f);
    m_prObeliskTrunk[6]  = VERTEX(D3DXVECTOR3( 0.135f, -0.95f,  0.135f), 
                                   D3DXVECTOR3( 1.0f, 0.0f,  0.0f), 0.50f, 1.0f);
    m_prObeliskTrunk[7]  = VERTEX(D3DXVECTOR3( 0.135f, -0.95f, -0.135f), 
                                   D3DXVECTOR3( 1.0f, 0.0f,  0.0f), 0.25f, 1.0f);

    m_prObeliskTrunk[8]  = VERTEX(D3DXVECTOR3( 0.060f,  0.85f,  0.060f), 
                                   D3DXVECTOR3( 0.0f, 0.0f,  1.0f), 0.55f, 0.0f);
    m_prObeliskTrunk[9]  = VERTEX(D3DXVECTOR3(-0.060f,  0.85f,  0.060f), 
                                   D3DXVECTOR3( 0.0f, 0.0f,  1.0f), 0.70f, 0.0f);
    m_prObeliskTrunk[10] = VERTEX(D3DXVECTOR3(-0.135f, -0.95f,  0.135f), 
                                   D3DXVECTOR3( 0.0f, 0.0f,  1.0f), 0.75f, 1.0f);
    m_prObeliskTrunk[11] = VERTEX(D3DXVECTOR3( 0.135f, -0.95f,  0.135f), 
                                   D3DXVECTOR3( 0.0f, 0.0f,  1.0f), 0.50f, 1.0f);

    m_prObeliskTrunk[12] = VERTEX(D3DXVECTOR3(-0.060f,  0.85f,  0.060f), 
                                   D3DXVECTOR3(-1.0f, 0.0f,  0.0f), 0.80f, 0.0f);
    m_prObeliskTrunk[13] = VERTEX(D3DXVECTOR3(-0.060f,  0.85f, -0.060f), 
                                   D3DXVECTOR3(-1.0f, 0.0f,  0.0f), 0.95f, 0.0f);
    m_prObeliskTrunk[14] = VERTEX(D3DXVECTOR3(-0.135f, -0.95f, -0.135f), 
                                   D3DXVECTOR3(-1.0f, 0.0f,  0.0f), 1.00f, 1.0f);
    m_prObeliskTrunk[15] = VERTEX(D3DXVECTOR3(-0.135f, -0.95f,  0.135f), 
                                   D3DXVECTOR3(-1.0f, 0.0f,  0.0f), 0.75f, 1.0f);

    // Initialize the ground vertices
    m_prGround[0] = VERTEX(D3DXVECTOR3(-100.0f, -0.95f, -100.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prGround[1] = VERTEX(D3DXVECTOR3(-100.0f, -0.95f,  100.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 50.0f, 0.0f);
    m_prGround[2] = VERTEX(D3DXVECTOR3( 100.0f, -0.95f,  100.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 50.0f, 50.0f);
    m_prGround[3] = VERTEX(D3DXVECTOR3( 100.0f, -0.95f, -100.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 50.0f);

    // Initialize the sky vertices
    m_prClouds[0] = VERTEX(D3DXVECTOR3(-1000.0f, 3.0f, -1000.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prClouds[1] = VERTEX(D3DXVECTOR3( 1000.0f, 3.0f, -1000.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 20.0f);
    m_prClouds[2] = VERTEX(D3DXVECTOR3( 1000.0f, 3.0f,  1000.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 20.0f, 20.0f);
    m_prClouds[3] = VERTEX(D3DXVECTOR3(-1000.0f, 3.0f,  1000.0f), 
                            D3DXVECTOR3(0.0f, 1.0f, 0.0f), 20.0f, 0.0f);

    InitMatrix(&mWorld,
        (float)( 1.01), (float)( 0.0), (float)( 0.0 ), (float)( 0.0),
        (float)( 0.0 ), (float)( 1.0), (float)( 0.0 ), (float)( 0.0),
        (float)( 0.0 ), (float)( 0.0), (float)( 1.01), (float)( 0.0),
        (float)( 0.0 ), (float)( 0.0), (float)( 0.0 ), (float)( 1.0)
    );

    // Initialize the cloud reflection vertices by slightly scaling out the
    // obelisk trunk.  This will prevent the polygons of the two meshes
    // from being coplanar and allow the reflection to correctly blend onto
    // the obelisk.
    for (i = 0; i < 16; i++) {
        m_prReflection[i] = m_prObeliskTrunk[i];
        D3DXVec3TransformCoord(&v1, (LPD3DXVECTOR3)&m_prReflection[i], &mWorld);
        memcpy(&m_prReflection[i], &v1, sizeof(D3DXVECTOR3));
    }

    // Initialize the materials
    SetMaterial(&m_matObeliskTip, RGB_MAKE(32, 32, 32), RGB_MAKE(13, 13, 13), 
                                     RGB_MAKE(96, 96, 96), RGB_MAKE(0, 0, 0), 
                                     40.0f);

    SetMaterial(&m_matObeliskTrunk, RGB_MAKE(242, 242, 242), 
                                     RGB_MAKE(230, 230, 230), 
                                     RGB_MAKE(255, 255, 255), 
                                     RGB_MAKE(0, 0, 0), 40.0f);

    for (i = 0; i < 4; i++) {

        SetMaterial(&m_pmatReflection[i], RGB_MAKE(242, 242, 242), 
                                     RGB_MAKE(230, 230, 230), 
                                     RGB_MAKE(255, 255, 255), 
                                     RGB_MAKE(0, 0, 0), 40.0f);

        SetMaterialTranslucency(&m_pmatReflection[i], 0.0f);
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
BOOL CObelisk::Setup() {

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
BOOL CObelisk::Initialize() {

    // Create the textures
    m_pd3dtObelisk = (CTexture8*)CreateTexture(m_pDevice, TEXT("obelisk.bmp"));
    if (!m_pd3dtObelisk) {
        return FALSE;
    }

    m_pd3dtGround = (CTexture8*)CreateTexture(m_pDevice, TEXT("ground.bmp"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP);
    if (!m_pd3dtGround) {
        return FALSE;
    }

    m_pd3dtClouds = (CTexture8*)CreateTexture(m_pDevice, TEXT("clouds.bmp"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP);
    if (!m_pd3dtClouds) {
        return FALSE;
    }

    // Initialize the directional light
    if (!SetDirectionalLight(m_pDevice, 0, D3DXVECTOR3(-2.0f, -2.0f, -1.0f), RGB_MAKE(230, 230, 230))) {
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
void CObelisk::Efface() {

    // Release all created objects
    if (m_pd3dtObelisk) {
        ReleaseTexture(m_pd3dtObelisk);
    }
    if (m_pd3dtClouds) {
        ReleaseTexture(m_pd3dtClouds);
    }
    if (m_pd3dtGround) {
        ReleaseTexture(m_pd3dtGround);
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
void CObelisk::Update() {

    PVERTEX prVertices;
    UINT    i;

    // Increment the rotation angle
    m_dAngleOfRotation += ROTATION_DELTA * m_fFrameDelta;
    if (m_dAngleOfRotation > M_2PI) {
        m_dAngleOfRotation -= M_2PI;
    }

    // Update the reflection alpha
    m_fAlpha = (float)cos(2.0 * m_dAngleOfRotation) * MAX_ALPHA;
    if (m_fAlpha < 0.0) {
        m_fAlpha = -m_fAlpha;
    }

    for (i = 0; i < 4; i++) {
        m_fReflectionAlpha[i] = 0.0f;
    }

    if (m_dAngleOfRotation > M_7PIDIV4 || m_dAngleOfRotation <= M_PIDIV4) {
        m_fReflectionAlpha[0] = m_fAlpha;
        prVertices = &m_prReflection[0];
    }
    else if (m_dAngleOfRotation > M_5PIDIV4) {
        m_fReflectionAlpha[3] = m_fAlpha;
        prVertices = &m_prReflection[12];
    }
    else if (m_dAngleOfRotation > M_3PIDIV4) {
        m_fReflectionAlpha[2] = m_fAlpha;
        prVertices = &m_prReflection[8];
    }
    else {
        m_fReflectionAlpha[1] = m_fAlpha;
        prVertices = &m_prReflection[4];
    }

    for (i = 0; i < 4; i++) {
        SetMaterialTranslucency(&m_pmatReflection[i], m_fReflectionAlpha[i]);
    }

    // Shift the cloud reflection
    m_fOffset += CLOUD_DRIFT_DELTA;
    if (m_fOffset > 1.0f) {
        m_fOffset -= 1.0f;
    }

    prVertices[0].u0 = m_fOffset;
    prVertices[1].u0 = m_fOffset + 0.25f;
    prVertices[2].u0 = m_fOffset + 0.25f;
    prVertices[3].u0 = m_fOffset;
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
BOOL CObelisk::Render() {

    D3DXMATRIX  mView;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    UINT        i;
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

    // Update the camera position
    memcpy(&mView, &m_mIdentity, sizeof(D3DXMATRIX));

    // Rotate the view by an angle of dAngleOfRotation
    mView._11 = (float)(cos(m_dAngleOfRotation));
    mView._31 = (float)(sin(m_dAngleOfRotation));
    mView._13 = -mView._31;
    mView._33 = mView._11;

    // Translate the view a distance back from the obelisk
    mView._43 = 4.0f;

    // Set the transform
    m_pDevice->SetTransform(D3DTS_VIEW, &mView);

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(66, 82, 107), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    // Enable mipmapping
    m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

    // Render the ground
    m_pDevice->SetMaterial(&m_matObeliskTrunk);
    m_pDevice->SetTexture(0, m_pd3dtGround);
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prGround, sizeof(VERTEX));

    // Render the clouds
    m_pDevice->SetTexture(0, m_pd3dtClouds);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prClouds, sizeof(VERTEX));

    m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);

    // Disable mipmapping
    m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

    // Render the obelisk trunk
    m_pDevice->SetTexture(0, m_pd3dtObelisk);

    for (i = 0; i < 4; i++) {
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &m_prObeliskTrunk[i * 4], sizeof(VERTEX));
    }

    // Render the obelisk tip
    m_pDevice->SetMaterial(&m_matObeliskTip);
    m_pDevice->SetTexture(0, NULL);

    for (i = 0; i < 4; i++) {
        hr = m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &m_prObeliskTip[i * 3], sizeof(VERTEX));
    }

    // Render the reflection
    m_pDevice->SetTexture(0, m_pd3dtClouds);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    for (i = 0; i < 4; i++) {

        m_pDevice->SetMaterial(&m_pmatReflection[i]);

        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &m_prReflection[i * 4], sizeof(VERTEX));
    }

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
void CObelisk::ProcessInput() {

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
BOOL CObelisk::InitView() {

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
BOOL CObelisk::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
