/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rearviewmirror.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "rearviewmirror.h"

static CTexture8* CreateAlphaTexture(CDevice8* pDevice, LPCTSTR szTexture, D3DFORMAT fmt);

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

    CRearViewMirror* pRVMirror;
    BOOL             bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pRVMirror = new CRearViewMirror();
    if (!pRVMirror) {
        return FALSE;
    }

    // Initialize the scene
    if (!pRVMirror->Create(pDisplay)) {
        pRVMirror->Release();
        return FALSE;
    }

    bRet = pRVMirror->Exhibit(pnExitCode);

    // Clean up the scene
    pRVMirror->Release();

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
// CRearViewMirror
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CRearViewMirror
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
CRearViewMirror::CRearViewMirror() {
}

//******************************************************************************
//
// Method:
//
//     ~CRearViewMirror
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
CRearViewMirror::~CRearViewMirror() {
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
BOOL CRearViewMirror::Create(CDisplay* pDisplay) {

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
int CRearViewMirror::Exhibit(int *pnExitCode) {

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
BOOL CRearViewMirror::Prepare() {

    // Initalize the vertices

    // Vertex 0
    m_rgvertCube[0].vPosition.x  = 0.5f;
    m_rgvertCube[0].vPosition.y  = 0.5f;
    m_rgvertCube[0].vPosition.z  = 0.5f;
    m_rgvertCube[0].vNormal.x = 0.5f;
    m_rgvertCube[0].vNormal.y = 0.5f;
    m_rgvertCube[0].vNormal.z = 0.5f;

    // Vertex 1
    m_rgvertCube[1].vPosition.x  = 0.5f;
    m_rgvertCube[1].vPosition.y  = 0.5f;
    m_rgvertCube[1].vPosition.z  = -0.5f;
    m_rgvertCube[1].vNormal.x = 0.5f;
    m_rgvertCube[1].vNormal.y = 0.5f;
    m_rgvertCube[1].vNormal.z = -0.5f;

    // Vertex 2
    m_rgvertCube[2].vPosition.x  = -0.5f;
    m_rgvertCube[2].vPosition.y  = 0.5f;
    m_rgvertCube[2].vPosition.z  = -0.5f;
    m_rgvertCube[2].vNormal.x = -0.5f;
    m_rgvertCube[2].vNormal.y = 0.5f;
    m_rgvertCube[2].vNormal.z = -0.5f;

    // Vertex 3
    m_rgvertCube[3].vPosition.x  = -0.5f;
    m_rgvertCube[3].vPosition.y  = 0.5f;
    m_rgvertCube[3].vPosition.z  = 0.5f;
    m_rgvertCube[3].vNormal.x = -0.5f;
    m_rgvertCube[3].vNormal.y = 0.5f;
    m_rgvertCube[3].vNormal.z = 0.5f;

    // Vertex 4
    m_rgvertCube[4].vPosition.x  = 0.5f;
    m_rgvertCube[4].vPosition.y  = -0.5f;
    m_rgvertCube[4].vPosition.z  = 0.5f;
    m_rgvertCube[4].vNormal.x = 0.5f;
    m_rgvertCube[4].vNormal.y = -0.5f;
    m_rgvertCube[4].vNormal.z = 0.5f;

    // Vertex 5
    m_rgvertCube[5].vPosition.x  = 0.5f;
    m_rgvertCube[5].vPosition.y  = -0.5f;
    m_rgvertCube[5].vPosition.z  = -0.5f;
    m_rgvertCube[5].vNormal.x = 0.5f;
    m_rgvertCube[5].vNormal.y = -0.5f;
    m_rgvertCube[5].vNormal.z = -0.5f;

    // Vertex 6
    m_rgvertCube[6].vPosition.x  = -0.5f;
    m_rgvertCube[6].vPosition.y  = -0.5f;
    m_rgvertCube[6].vPosition.z  = -0.5f;
    m_rgvertCube[6].vNormal.x = -0.5f;
    m_rgvertCube[6].vNormal.y = -0.5f;
    m_rgvertCube[6].vNormal.z = -0.5f;

    // Vertex 7
    m_rgvertCube[7].vPosition.x  = -0.5f;
    m_rgvertCube[7].vPosition.y  = -0.5f;
    m_rgvertCube[7].vPosition.z  = 0.5f;
    m_rgvertCube[7].vNormal.x = -0.5f;
    m_rgvertCube[7].vNormal.y = -0.5f;
    m_rgvertCube[7].vNormal.z = 0.5f;

    // Initialize the indices

    // top face
    m_rgwCubeIndices[0]  = 0;
    m_rgwCubeIndices[1]  = 1;
    m_rgwCubeIndices[2]  = 2;
    m_rgwCubeIndices[3]  = 0;
    m_rgwCubeIndices[4]  = 2;
    m_rgwCubeIndices[5]  = 3;

    //bottom face
    m_rgwCubeIndices[6]  = 5;
    m_rgwCubeIndices[7]  = 4;
    m_rgwCubeIndices[8]  = 7;
    m_rgwCubeIndices[9]  = 5;
    m_rgwCubeIndices[10] = 7;
    m_rgwCubeIndices[11] = 6;

    //front face
    m_rgwCubeIndices[12] = 1;
    m_rgwCubeIndices[13] = 5;
    m_rgwCubeIndices[14] = 6;
    m_rgwCubeIndices[15] = 1;
    m_rgwCubeIndices[16] = 6;
    m_rgwCubeIndices[17] = 2;

    //back face
    m_rgwCubeIndices[18] = 3;
    m_rgwCubeIndices[19] = 7;
    m_rgwCubeIndices[20] = 4;
    m_rgwCubeIndices[21] = 3;
    m_rgwCubeIndices[22] = 4;
    m_rgwCubeIndices[23] = 0;

    //left face
    m_rgwCubeIndices[24] = 2;
    m_rgwCubeIndices[25] = 6;
    m_rgwCubeIndices[26] = 7;
    m_rgwCubeIndices[27] = 2;
    m_rgwCubeIndices[28] = 7;
    m_rgwCubeIndices[29] = 3;

    //right face
    m_rgwCubeIndices[30] = 0;
    m_rgwCubeIndices[31] = 4;
    m_rgwCubeIndices[32] = 5;
    m_rgwCubeIndices[33] = 0;
    m_rgwCubeIndices[34] = 5;
    m_rgwCubeIndices[35] = 1;

    // Initialize the trench's vertices

    // Floor
    m_rgvertTrench[0] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f,-3000.0f), 
                                 D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 300.0f);
    m_rgvertTrench[1] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f, 3000.0f), 
                                 D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_rgvertTrench[2] = VERTEX(D3DXVECTOR3( 10.0f,-10.0f, 3000.0f), 
                                 D3DXVECTOR3(0.0f, 1.0f, 0.0f), 5.0f, 0.0f);
    m_rgvertTrench[3] = VERTEX(D3DXVECTOR3( 10.0f,-10.0f,-3000.0f), 
                                 D3DXVECTOR3(0.0f, 1.0f, 0.0f), 5.0f, 300.0f);
    // Left wall
    m_rgvertTrench[4] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f,-3000.0f), 
                                 D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 5.0f);
    m_rgvertTrench[5] = VERTEX(D3DXVECTOR3(-10.0f, 10.0f,-3000.0f), 
                                 D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
    m_rgvertTrench[6] = VERTEX(D3DXVECTOR3(-10.0f, 10.0f, 3000.0f), 
                                 D3DXVECTOR3(1.0f, 0.0f, 0.0f), 300.0f, 0.0f);
    m_rgvertTrench[7] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f, 3000.0f), 
                                 D3DXVECTOR3(1.0f, 0.0f, 0.0f), 300.0f, 5.0f);

    // Right wall
    m_rgvertTrench[8]  = VERTEX(D3DXVECTOR3(10.0f,-10.0f, 3000.0f), 
                                 D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 5.0f);
    m_rgvertTrench[9]  = VERTEX(D3DXVECTOR3(10.0f, 10.0f, 3000.0f), 
                                 D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
    m_rgvertTrench[10] = VERTEX(D3DXVECTOR3(10.0f, 10.0f,-3000.0f), 
                                 D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 300.0f, 0.0f);
    m_rgvertTrench[11] = VERTEX(D3DXVECTOR3(10.0f,-10.0f,-3000.0f), 
                                 D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 300.0f, 5.0f);

    // Initialize the trench's indices

    // Floor
    m_rgwTrenchIndices[0]  = 0;
    m_rgwTrenchIndices[1]  = 1;
    m_rgwTrenchIndices[2]  = 2;
    m_rgwTrenchIndices[3]  = 0;
    m_rgwTrenchIndices[4]  = 2;
    m_rgwTrenchIndices[5]  = 3;

    // Left wall
    m_rgwTrenchIndices[6]  = 4;
    m_rgwTrenchIndices[7]  = 5;
    m_rgwTrenchIndices[8]  = 6;
    m_rgwTrenchIndices[9]  = 4;
    m_rgwTrenchIndices[10] = 6;
    m_rgwTrenchIndices[11] = 7;

    // Right wall
    m_rgwTrenchIndices[12] = 8;
    m_rgwTrenchIndices[13] = 9;
    m_rgwTrenchIndices[14] = 10;
    m_rgwTrenchIndices[15] = 8;
    m_rgwTrenchIndices[16] = 10;
    m_rgwTrenchIndices[17] = 11;

    // Initialize the vertices for the mirror.  The vertices are lit
    // so the mirror isn't affected by the lighting in the scene
    // on the second rendering pass.  The two texture coordinate sets
    // are used for multistage texturing.
    m_rgvertMirror[0].vectPosition = D3DXVECTOR3(-1.5f, -4.5f, 5.0f);
    m_rgvertMirror[0].u0 = m_rgvertMirror[0].u1 = 0.0f;
    m_rgvertMirror[0].v0 = m_rgvertMirror[0].v1 = 1.0f;

    m_rgvertMirror[1].vectPosition = D3DXVECTOR3(-1.5f, -3.7f, 4.925f);
    m_rgvertMirror[1].u0 = m_rgvertMirror[1].u1 = 0.0f;
    m_rgvertMirror[1].v0 = m_rgvertMirror[1].v1 = 0.0f;

    m_rgvertMirror[2].vectPosition = D3DXVECTOR3( 1.5f, -3.7f, 4.925f);
    m_rgvertMirror[2].u0 = m_rgvertMirror[2].u1 = 1.0f;
    m_rgvertMirror[2].v0 = m_rgvertMirror[2].v1 = 0.0f;

    m_rgvertMirror[3].vectPosition = D3DXVECTOR3( 1.5f, -4.5f, 5.0f);
    m_rgvertMirror[3].u0 = m_rgvertMirror[3].u1 = 1.0f;
    m_rgvertMirror[3].v0 = m_rgvertMirror[3].v1 = 1.0f;

    m_mCube = m_mIdentity;

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
BOOL CRearViewMirror::Setup() {

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
BOOL CRearViewMirror::Initialize() {

    D3DSURFACE_DESC d3dsd;
    HRESULT         hr;

    // Load the trench texture
    m_pd3dtTrench = (CTexture8*)CreateTexture(m_pDevice, TEXT("tile.bmp"));
    if (!m_pd3dtTrench)
    {
        OutputDebugString(TEXT("Error: Failed to load texture\r\n"));
        return FALSE;
    }

    // Load the alpha texture to blend with the mirror
    m_pd3dtShade = CreateAlphaTexture(m_pDevice, TEXT("shade.bmp"), D3DFMT_A8R8G8B8);
    if (!m_pd3dtShade)
    {
        OutputDebugString(TEXT("Error: Failed to load texture\r\n"));
        return FALSE;
    }

    // Create the mirror texture as a rendering target
    m_pd3dtMirror = (CTexture8*)CreateTexture(m_pDevice, 512, 512, D3DFMT_X8R8G8B8, TTYPE_TARGET);
    if (!m_pd3dtMirror)
        return FALSE;

    hr = m_pd3dtMirror->GetSurfaceLevel(0, &m_pd3dsMirror);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
        return FALSE;
    }

    hr = m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pd3dsBack);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetBackBuffer"))) {
        return FALSE;
    }

    // Create the mirror depth buffer
    hr = m_pDevice->GetDepthStencilSurface(&m_pd3dsDepth);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilSurface"))) {
        return FALSE;
    }
    m_pd3dsDepth->GetDesc(&d3dsd);

    hr = m_pDevice->CreateDepthStencilSurface(512, 512, d3dsd.Format, D3DMULTISAMPLE_NONE, &m_pd3dsZMirror);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateDepthStencilSurface"))) {
        return FALSE;
    }

    // Initialize texture coordinates to their appropriate stages
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

    // Add a white light
    if (!SetDirectionalLight(m_pDevice, 0, D3DXVECTOR3(-0.5f, -1.0f, -1.0f), RGB_MAKE(255, 255, 255))) {
        return FALSE;
    }

    // Set a white material
    SetMaterial(&m_matWhite, RGB_MAKE(255, 255, 255));

    m_pDevice->SetMaterial(&m_matWhite);

    // Create materials to color the cubes
    SetMaterial(&m_material[0], D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.75f, 0.0f, 0.0f, 1.0f), 20.0f);
    SetMaterial(&m_material[1], D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.5f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.75f, 0.0f, 1.0f), 20.0f);
    SetMaterial(&m_material[2], D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.5f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.75f, 1.0f), 20.0f);
    SetMaterial(&m_material[3], D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.5f, 5.0f, 0.0f, 1.0f), D3DXVECTOR4(0.75f, 0.75f, 0.0f, 1.0f), 20.0f);
    SetMaterial(&m_material[4], D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.5f, 0.0f, 0.5f, 1.0f), D3DXVECTOR4(0.75f, 0.0f, 0.75f, 1.0f), 20.0f);

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
void CRearViewMirror::Efface() {

    if (m_pd3dsZMirror) m_pd3dsZMirror->Release();
    if (m_pd3dsMirror) m_pd3dsMirror->Release();
    if (m_pd3dsBack) m_pd3dsBack->Release();
    if (m_pd3dsDepth) m_pd3dsDepth->Release();
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
void CRearViewMirror::Update() {

    D3DXMATRIX   mRotate;
    static float fAngle = 0.0f; 

    // Translate the view along the z axis 
    m_cam.vPosition.z += 0.5f * m_fTimeDilation;
    m_cam.vInterest.z += 0.5f * m_fTimeDilation;
    if (m_cam.vPosition.z >= 1000.0f) 
    {
        m_cam.vPosition.z = -1000.0f;
        m_cam.vInterest.z = -950.0f;
    }

    // Set the mirror's world matrix to keep it positioned in front of the camera
    m_mMirror = m_mIdentity;
    m_mMirror._43 = m_cam.vPosition.z;

    // Rotate the cube about the y axis
    m_mCube._11 = (float)cos(fAngle);
    m_mCube._13 = (float)-sin(fAngle);
    m_mCube._31 = (float)sin(fAngle);
    m_mCube._33 = (float)cos(fAngle);
    m_mCube._42 = -5.0f;

    fAngle += 0.015f * m_fTimeDilation;
    if (fAngle > M_2PI) {
        fAngle -= M_2PI;
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
BOOL CRearViewMirror::Render() {

    CSurface8*  pd3dsBack;
    CSurface8*  pd3dsDepth;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    UINT        uPass, i;
    D3DRECT     d3drect;

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

    // Initialize the view matrix
    ::SetView(&m_mView, &m_cam.vPosition, &m_cam.vInterest, 
            &D3DXVECTOR3((float)sin(m_cam.fRoll), (float)cos(m_cam.fRoll), 
            0.0f));

    SetPerspectiveProjection(&m_mProj, m_cam.fNearPlane, m_cam.fFarPlane, 
            m_cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    m_pDevice->SetTransform(D3DTS_PROJECTION, &m_mProj);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    for (uPass = 0; uPass < 2; uPass++) {

        // Set up the rendering target and viewing matrices
        if (!uPass) {

            // Set the texture as the new rendering target for the device
            if (!ReplaceRenderTarget(m_pd3dsMirror, m_pd3dsZMirror, &pd3dsBack, &pd3dsDepth, &d3drect)) {
                return FALSE;
            }

            // Move the camera to the mirror's point of view
            if (!SetReflectedView(&m_cam, m_rgvertMirror, &m_mMirror)) {
                return FALSE;
            }
        }

        else {

            // Restore the back buffer as the rendering target for the scene
            if (!ReplaceRenderTarget(pd3dsBack, pd3dsDepth, &m_pd3dsMirror, &m_pd3dsZMirror, &d3drect)) {
                return FALSE;
            }

            // Restore the view matrix
            m_pDevice->SetTransform(D3DTS_VIEW, &m_mView);

            // Restore the projection matrix
            SetPerspectiveProjection(&m_mProj, m_cam.fNearPlane, m_cam.fFarPlane, 
                    m_cam.fFieldOfView, (float)d3drect.y2 / (float)d3drect.x2);

            m_pDevice->SetTransform(D3DTS_PROJECTION, &m_mProj);
        }

        // Clear the rendering target
        if (m_pDisplay->IsDepthBuffered()) {
            dwClearFlags |= D3DCLEAR_ZBUFFER;
        }
        m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        // Set the trench texture
        m_pDevice->SetTexture(0, m_pd3dtTrench);

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);

        // Draw the trench
        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 12, 6, m_rgwTrenchIndices,
                D3DFMT_INDEX16, m_rgvertTrench, sizeof(VERTEX));

        // Clear the trench texture
        m_pDevice->SetTexture(0, NULL);

        // Draw a series of cubes evenly spaced along the trench
        for (i = 0; i < 5; i++) {

            // Set a material for the cube
            m_pDevice->SetMaterial(&m_material[i]);

            for (m_mCube._43 = 2000.0f + 200.0f * (float)i; 
                 m_mCube._43 > -2000.0f; m_mCube._43 -= 1000.0f) 
            {
                // Set the world matrix for the cube
                m_pDevice->SetTransform(D3DTS_WORLD, &m_mCube);

                // Draw the cube
                m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, m_rgwCubeIndices,
                        D3DFMT_INDEX16, m_rgvertCube, sizeof(VERTEX));
            }
        }

        // Restore the default material
        m_pDevice->SetMaterial(&m_matWhite);

        // Set the world matrix for the mirror
        m_pDevice->SetTransform(D3DTS_WORLD, &m_mMirror);

        // Render the mirror
        if (!uPass) {

            // Set the texture coordinates of the mirror in such a way as to
            // clip the scene rendered in the first pass to the mirror's boundaries
            hr = SetReflectionTextureCoords(m_rgvertMirror, 4);
            if (FAILED(hr)) {
                return FALSE;
            }
        }

        else {

            // Disable perspective correct texture mapping since the perspective
            // of the mirror's reflection was already set on the first rendering pass
//            m_pDevice->SetRenderState(D3DRS_TEXTUREPERSPECTIVE, (DWORD)FALSE);

            // Set the mirror's textures
            m_pDevice->SetTexture(0, m_pd3dtMirror);
            m_pDevice->SetTexture(1, m_pd3dtShade);

            // Use color and alpha information from the texture for the first stage
            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
            SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);

            // Modulate the color and alpha information with the alpha texture for the second stage
            SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
            SetAlphaStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

            // Enable blending
            m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

            m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

            m_pDevice->SetVertexShader(FVF_MLVERTEX);

            // Draw the mirror
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
                    m_rgvertMirror, sizeof(MLVERTEX));

            m_pDevice->SetVertexShader(FVF_VERTEX);

            m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

            // Disable blending
            m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

            // Modulate color and select texture alpha for the first stage
            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
            SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);

            // Disable texture and blend operations for the second stage
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            m_pDevice->SetTexture(1, NULL);

            // Restore perspective correct texture mapping
//            m_pDevice->SetRenderState(D3DRS_TEXTUREPERSPECTIVE, (DWORD)TRUE);

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
        }

        // End the scene
        m_pDevice->EndScene();
    }

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
void CRearViewMirror::ProcessInput() {

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
BOOL CRearViewMirror::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_cam.vPosition     = D3DXVECTOR3(0.0f, -5.0f, -1000.0f);
    m_cam.vInterest     = D3DXVECTOR3(0.0f, -6.0f, -950.0f);
    m_cam.fRoll         = 0.0f;
    m_cam.fFieldOfView  = M_PI / 4.0f;
    m_cam.fNearPlane    = 0.1f;
    m_cam.fFarPlane     = 3000.0f;
    m_pDisplay->SetCamera(&m_cam);

    return m_pDisplay->SetView(&m_cam);
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
BOOL CRearViewMirror::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

//******************************************************************************
// 
// Function:
// 
//     ReplaceRenderTarget
// 
// Description:
// 
//     Replaces the current rendering target surface with the given 
//     surface and updates the aspect ratio of the viewport.
// 
// Arguments:
// 
//     LPDIRECTDRAWSURFACE4 pdds      - New rendering target surface
// 
//     LPDIRECTDRAWSURFACE4* ppddsOld - A pointer to a surface interface
//                                      pointer that, on return, will be 
//                                      set to the rendering target 
//                                      that was replaced
// 
//     LPD3DRECT pd3drect             - A D3DRECT pointer that will be
//                                      set to the dimensions of the 
//                                      new rendering target on return
// 
// Return Value:
// 
//    true on success, false on failure.
// 
//******************************************************************************
BOOL CRearViewMirror::ReplaceRenderTarget(CSurface8* pd3ds, CSurface8* pd3dsZ, CSurface8** ppd3dsOld, CSurface8** ppd3dsZOld, D3DRECT* pd3drect) {

    D3DSURFACE_DESC d3dsd;
    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    if (ppd3dsOld) {

        // Retrieve a pointer to the current rendering target
        hr = m_pDevice->GetRenderTarget(ppd3dsOld);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
            return FALSE;
        }

        // GetRenderTarget incremented the ref count on the target surface.
        // Release the reference.
        (*ppd3dsOld)->Release();
    }

    if (ppd3dsZOld) {

        // Retrieve a pointer to the current depth buffer
        hr = m_pDevice->GetDepthStencilSurface(ppd3dsZOld);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDepthStencilBuffer"))) {
            return FALSE;
        }

        // GetRenderTarget incremented the ref count on the target surface.
        // Release the reference.
        (*ppd3dsZOld)->Release();
    }

    // Replace the current rendering target with the given surface
    hr = m_pDevice->SetRenderTarget(pd3ds, pd3dsZ);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderTarget"))) {
        return FALSE;
    }

    // Obtain the dimensions of the new rendering target
    pd3ds->GetDesc(&d3dsd);

    if (pd3drect) {
        pd3drect->x1 = 0;
        pd3drect->y1 = 0;
        pd3drect->x2 = d3dsd.Width;
        pd3drect->y2 = d3dsd.Height;
    }

    // Update the viewport with the dimensions of the new rendering target
    memset(&viewport, 0, sizeof(D3DVIEWPORT8));
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = d3dsd.Width;
    viewport.Height = d3dsd.Height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    hr = m_pDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

// ##HACK for ATI card
m_pDevice->SetRenderState(D3DRS_SHADEMODE, (DWORD)D3DSHADE_GOURAUD);

    return TRUE;
}

//******************************************************************************
// 
// Function:
// 
//     SetReflectedView
// 
// Description:
// 
//     Sets the view and projection matrices in such a way as to 
//     render the scene from a reflective surface's point of view.
// 
// Arguments:
// 
//     PCAMERA pcam                   - The camera in the scene
// 
//     PMVERTEX prVertices            - Pointer to the first of at
//                                      least three vertices representing
//                                      the plane of the reflective
//                                      surface
// 
//     LPD3DMATRIX pmWorld            - The world matrix that will
//                                      be used to render the reflective
//                                      surface
// 
// Return Value:
// 
//    true on success, false on failure.
// 
//******************************************************************************
BOOL CRearViewMirror::SetReflectedView(PCAMERA pcam, PMLVERTEX prVertices, D3DXMATRIX* pmWorld) {

    D3DXVECTOR3 vFaceNormal,
                vIncidenceDir,
                vReflectDir,
                vOffset,
                v1, v2,
                pvPlane[3];
    CAMERA      camReflect;
    UINT i;

    // Transform the first three vertices of the reflective plane from model space to world space
    for (i = 0; i < 3; i++) {
        D3DXVec3TransformCoord(&pvPlane[i], &(prVertices[i].vectPosition), pmWorld);
    }

    // Calculate the face normal of the reflective plane
    v1 = pvPlane[1] - pvPlane[0];
    v2 = pvPlane[2] - pvPlane[0];
    D3DXVec3Normalize(&vFaceNormal, D3DXVec3Cross(&vOffset, &v1, &v2));

    // Calculate the position of the camera for the reflected point of view
    memcpy(&camReflect, pcam, sizeof(CAMERA));
    D3DXVec3Normalize(&vIncidenceDir, &(pcam->vInterest - pcam->vPosition));
    vOffset = (2.0f * D3DXVec3Dot(&vFaceNormal, &(*(D3DXVECTOR3*)&(pvPlane[0].x) - pcam->vPosition)) * vFaceNormal);
    camReflect.vPosition = pcam->vPosition + vOffset;

    // Calculate the direction of the camera for the reflected point of view
    vOffset = (2.0f * D3DXVec3Dot(&vFaceNormal, &vIncidenceDir) * vFaceNormal);
    vReflectDir = vIncidenceDir - vOffset;
    camReflect.vInterest = camReflect.vPosition + vReflectDir;

    // TODO: Move front clipping plane forward for texture render target

    // Update the view matrix to reposition the camera to the mirrored point of view
    if (!m_pDisplay->SetView(&camReflect)) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
// 
// Function:
// 
//     SetReflectionTextureCoords
// 
// Description:
// 
//     Set the texture coordinates of the reflective surface in such 
//     a way as to clip the scene to the surface's boundaries
// 
// Arguments:
// 
//     PMVERTEX prVertices            - Pointer to an array of vertices
//                                      comprising the reflective surface
// 
//     UINT uNumVertices              - Number of vertices in the array
// 
// Return Value:
// 
//     An HRESULT containing the error code of the failed method.
// 
//******************************************************************************
HRESULT CRearViewMirror::SetReflectionTextureCoords(PMLVERTEX prVertices, UINT uNumVertices) {

    D3DXMATRIX   mClip, r1, r2;
    D3DVECTOR*   pv;
    float        fRHW, fX, fY;
    UINT         i;
    HRESULT      hr;

    // Build a transformation matrix from the current world, view, and
    // projection matrices

    // Create a clipping matrix
    InitMatrix(&mClip,
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    );

    D3DXMatrixMultiply(&r1, &m_mProj, &mClip);
    D3DXMatrixMultiply(&r2, &m_mView, &r1);
    D3DXMatrixMultiply(&r1, &m_mMirror, &r2);

    // Transform the reflective surface vertices into screen coordinates.
    // Use the screen coordinates to set texture coordinates for the surface.
    for (i = 0; i < uNumVertices; i++) {
        pv = &prVertices[i].vectPosition;
        fRHW = 1.0f / (r1._14 * pv->x + r1._24 * pv->y + r1._34 * pv->z + r1._44);
        fX = (pv->x * r1._11 + pv->y * r1._21 + pv->z * r1._31 + r1._41) * fRHW;
        fY = (pv->x * r1._12 + pv->y * r1._22 + pv->z * r1._32 + r1._42) * fRHW;
		prVertices[i].u0 = fX;
        prVertices[i].v0 = -fY + 1.0f;
    }

    return D3D_OK;
}

//******************************************************************************
//
// Function:
// 
//     CreateAlphaTexture
// 
// Description:
// 
//     Create a texture map surface and texture object and initialize
//     it with alpha information based on the named bitmap file.  The bitmap
//     is first loaded into a temporary surface.  Next a system memory texture
//     is created and its alpha bits are initialized proportionally to the
//     color information interpolated from the bitmap surface.  Finally a
//     device memory texture is loaded from the system memory texture.
// 
// Arguments:
// 
//     LPDIRECT3DDEVICE2 pd3dDevice         - Direct3DDevice object
//
//     LPCTSTR szTexture                    - File or resource name of the 
//                                            bitmap to create the texture from
//
//     PIXELFORMAT pxf                      - Optional pixel format of the 
//                                            texture to create
// 
// Return Value:
// 
//     LPDIRECT3DTEXTURE2   - Pointer to the loaded Texture (or NULL if failure)
// 
//******************************************************************************
CTexture8* CreateAlphaTexture(CDevice8* pDevice, LPCTSTR szTexture, D3DFORMAT fmt)
{
    CTexture8 *pd3dtC, *pd3dtA;
    D3DLOCKED_RECT d3dlrSrc, d3dlrDst;
    D3DSURFACE_DESC d3dsd;
    DWORD           dwDepth;
    ARGBPIXELDESC   pixd;
    LPDWORD         pdwSrc;
    float           fAlpha;
    UINT            i, j;
    HRESULT hr;

    dwDepth = FormatToBitDepth(fmt);
    if (!(dwDepth == 16 || dwDepth == 32)) {
        return NULL;
    }

    GetARGBPixelDesc(fmt, &pixd);

    pd3dtC = (CTexture8*)CreateTexture(pDevice, szTexture, D3DFMT_X8R8G8B8);
    if (!pd3dtC) {
        return NULL;
    }

    pd3dtC->GetLevelDesc(0, &d3dsd);

    pd3dtA = (CTexture8*)CreateTexture(pDevice, d3dsd.Width, d3dsd.Height, fmt);
    if (!pd3dtA) {
        ReleaseTexture(pd3dtC);
        return NULL;
    }

    hr = pd3dtC->LockRect(0, &d3dlrSrc, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        ReleaseTexture(pd3dtA);
        ReleaseTexture(pd3dtC);
        return NULL;
    }

    pdwSrc = (LPDWORD)d3dlrSrc.pBits;

    hr = pd3dtA->LockRect(0, &d3dlrDst, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        ReleaseTexture(pd3dtA);
        ReleaseTexture(pd3dtC);
        return NULL;
    }

    if (dwDepth == 32) {

        LPDWORD pdwDst;

        pdwDst = (LPDWORD)d3dlrDst.pBits;

        for (i = 0; i < d3dsd.Height; i++) {

            for (j = 0; j < d3dsd.Width; j++) {

                // Interpolate the red, green, and blue components of the source bitmap
                // into an intensity value between 0.0 and 1.0
                fAlpha = ((float)(*(pdwSrc + j) & 0x00FF0000) / (float)0x00FF0000 +
                          (float)(*(pdwSrc + j) & 0x0000FF00) / (float)0x0000FF00 +
                          (float)(*(pdwSrc + j) & 0x000000FF) / (float)0x000000FF) / 3.0f;

                // Set the alpha bits according to the intensity value
                *(pdwDst + j) = (((UINT)((float)(pixd.dwAMask >> pixd.dwAShift) * fAlpha) << pixd.dwAShift) & pixd.dwAMask) | ~pixd.dwAMask;
            }

            pdwSrc += d3dlrSrc.Pitch / 4;
            pdwDst += d3dlrDst.Pitch / 4;
        }
    }

    else if (dwDepth == 16) {

        LPWORD pwDst;

        pwDst = (LPWORD)d3dlrDst.pBits;

        for (i = 0; i < d3dsd.Height; i++) {

            for (j = 0; j < d3dsd.Width; j++) {

                // Interpolate the red, green, and blue components of the source bitmap
                // into an intensity value between 0.0 and 1.0
                fAlpha = ((float)(*(pdwSrc + j) & 0x00FF0000) / (float)0x00FF0000 +
                          (float)(*(pdwSrc + j) & 0x0000FF00) / (float)0x0000FF00 +
                          (float)(*(pdwSrc + j) & 0x000000FF) / (float)0x000000FF) / 3.0f;

                // Set the alpha bits according to the intensity value
                *(pwDst + j) = ((UINT)((float)(pixd.dwAMask) * fAlpha) & pixd.dwAMask) | 
                                            (~(WORD)pixd.dwAMask & 0xFFFF);
            }

            pdwSrc += d3dlrSrc.Pitch / 4;
            pwDst += d3dlrDst.Pitch / 2;
        }
    }
    
    pd3dtA->UnlockRect(0);
    pd3dtC->UnlockRect(0);
    ReleaseTexture(pd3dtC);

    return pd3dtA;
}

