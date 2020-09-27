/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bumpnorm.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "bumpnorm.h"

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

    CBumpNormal1*   pBump;
    BOOL            bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pBump = new CBumpNormal1();
    if (!pBump) {
        return FALSE;
    }

    // Initialize the scene
    if (!pBump->Create(pDisplay)) {
        pBump->Release();
        return FALSE;
    }

    bRet = pBump->Exhibit(pnExitCode);

    // Clean up the scene
    pBump->Release();

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

    if (!(pd3dcaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP)) {
        return FALSE;
    }
    return TRUE;
}

//******************************************************************************
// CBumpNormal1
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CBumpNormal1
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
CBumpNormal1::CBumpNormal1() {

    m_pd3dtBase = NULL;
    m_pd3dtBump = NULL;
    m_pd3dtEnv = NULL;
    m_prSphere2 = NULL;
    m_pwSphere = NULL;
    m_pmTangent = NULL;

    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CBumpNormal1
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
CBumpNormal1::~CBumpNormal1() {
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
BOOL CBumpNormal1::Create(CDisplay* pDisplay) {

    D3DXMatrixIdentity(&m_mWorld);

    m_vLightDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

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
int CBumpNormal1::Exhibit(int *pnExitCode) {

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
BOOL CBumpNormal1::Setup() {

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
BOOL CBumpNormal1::Initialize() {

    LPBYTE  pData;
    UINT    i;
    HRESULT hr;

    // Create a texture
//    m_pd3dt = CreateTexture(TEXT("Texture"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP, NULL, 1.0f, RGB_MAKE(0, 0, 0));
//    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("fscaleb.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("earth.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
//m_pd3dt = CreateDiffuseMap(256, 256, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
    if (!m_pd3dtBase) {
        return FALSE;
    }

//    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("bump4.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP);
    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("earthbump.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP);
    if (!m_pd3dtBump) {
        return FALSE;
    }

    m_pd3dtEnv = (CTexture8*)CreateDiffuseMap(m_pDevice, 256, 256, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
    if (!m_pd3dtEnv) {
        return FALSE;
    }

    m_pd3dtcNormal = (CCubeTexture8*)CreateNormalMap(m_pDevice);
    if (!m_pd3dtcNormal) {
        return FALSE;
    }

    CreateSphere(&m_prSphere2, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices, 10.0f, 16, 16, 1.0f, 1.0f);
//    m_prSphere = (PBVERTEX)MemAlloc32(m_dwSphereVertices * sizeof(BVERTEX));
    if (!m_prSphere2) {
        return FALSE;
    }

    m_prSphere = (PNVERTEX)MemAlloc32(m_dwSphereVertices * sizeof(NVERTEX));
    if (!m_prSphere) {
        return FALSE;
    }

    for (i = 0; i < m_dwSphereVertices; i++) {
        m_prSphere[i].vPosition = m_prSphere2[i].vPosition;
        m_prSphere[i].u0 = m_prSphere[i].u1 = m_prSphere2[i].u0;
        m_prSphere[i].v0 = m_prSphere[i].v1 = m_prSphere2[i].v0;
    }

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }

    hr = m_pDevice->CreateVertexBuffer(m_dwSphereVertices * sizeof(BVERTEX), 0, FVF_BVERTEX, D3DPOOL_DEFAULT, &m_pd3drSphere);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));
        return FALSE;
    }

    hr = m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(BVERTEX), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    memcpy(pData, m_prSphere, m_dwSphereVertices * sizeof(BVERTEX));

    hr = m_pd3drSphere->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

//    m_prQuad[0] = NVERTEX(D3DXVECTOR3(-10.0f, 0.0f, -10.0f), 0.0f, 2.0f, 0.0f, 1.0f);
//    m_prQuad[1] = NVERTEX(D3DXVECTOR3(-10.0f, 0.0f,  10.0f), 0.0f, 0.0f, 0.0f, 0.0f);
//    m_prQuad[2] = NVERTEX(D3DXVECTOR3( 10.0f, 0.0f,  10.0f), 2.0f, 0.0f, 1.0f, 0.0f);
//    m_prQuad[3] = NVERTEX(D3DXVECTOR3( 10.0f, 0.0f, -10.0f), 2.0f, 2.0f, 1.0f, 1.0f);

    m_prQuad[0] = NVERTEX(D3DXVECTOR3(-10.0f, -10.0f, 0.0f), 0.0f, 1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f);
    m_prQuad[1] = NVERTEX(D3DXVECTOR3(-10.0f,  10.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_prQuad[2] = NVERTEX(D3DXVECTOR3( 10.0f,  10.0f, 0.0f), 1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    m_prQuad[3] = NVERTEX(D3DXVECTOR3( 10.0f, -10.0f, 0.0f), 1.0f, 1.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f);

    m_pmTangent = (PMATRIX3X3)MemAlloc32(m_dwSphereVertices * sizeof(MATRIX3X3));
    if (!m_pmTangent) {
        return FALSE;
    }

D3DXMATRIX mWorld;
D3DXMatrixIdentity(&mWorld);
    SetTangentTransforms(m_pmTangent, sizeof(MATRIX3X3), &m_prSphere[0].vPosition, sizeof(NVERTEX), (PTCOORD2)&m_prSphere[0].u1, sizeof(NVERTEX), m_pwSphere, m_dwSphereIndices / 3, D3DPT_TRIANGLELIST, &mWorld, TRUE);

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
void CBumpNormal1::Efface() {

    if (m_pmTangent) {
        MemFree32(m_pmTangent);
    }
    if (m_prSphere2 && m_pwSphere) {
        ReleaseSphere(&m_prSphere2, &m_pwSphere);
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
void CBumpNormal1::Update() {

    D3DXMATRIX mWorld;
    float      fTheta;
//    float  fTheta = M_PI / 25.0f;

    fTheta = M_PI / 600.0f;

    // Update the rotation
    InitMatrix(&mWorld,
        (float)(cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)(sin(fTheta)), (float)( 0.0f), (float)(cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

//    D3DXMatrixMultiply(&m_mWorld, (LPD3DXMATRIX)&mWorld, &m_mWorld);
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
BOOL CBumpNormal1::Render() {

    DWORD        dwClearFlags = D3DCLEAR_TARGET;
    DWORD        dwLightDir;
    D3DXVECTOR3  vLightDir;
    D3DXMATRIX   mWorld;
    LPBYTE       pData;
    static float fAngle = 0.0f;
    HRESULT      hr;

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

    // Set the light
    // TODO: Change this to transpose/inverse the world matrix to backtransform
    // the light from world space to model space
    // Have a function take a light pointer, world matrix pointer, vertex list,
    // and size and calculate the bump u and v from the light dir and vertex normal dir
    m_vLightDir.x = (float)(cos(fAngle));
    m_vLightDir.z = (float)(sin(fAngle));
    m_vLightDir.y = 0.0f;

//m_vLightDir = D3DXVECTOR3(0.0f, 1.0f, 1.0f);
    fAngle -= M_PI / 400.0f;
//    fAngle -= M_PI / 8.0f;
    if (fAngle < -M_2PI) {
        fAngle += M_2PI;
    }

/*
Vertex format:
position
vertex normal
tcoord1: light direction in texture space (indexes into a cube normal map to renormalize the interpolated normals at the pixel level
tcoord2: half-vector in texture space (and indexes into the cube normal map)
tcoord3: base texture coordinates

to calculate tcoord1:
backtransform the lightdir from world space into model space using the inverse world matrix at the start of the function
for each face in the model:
backtransform the lightdir from model space into texture space using the tangent/orthonormal basis transform
for each vertex interpolate between the face ldirs for the faces sharing the vertex (and renormalize)
store the ldir in tcoord1
*/

    D3DXMatrixTranspose(&mWorld, &m_mWorld);
    D3DXVec3TransformCoord(&m_vLightDir, &m_vLightDir, &mWorld);

    D3DXVec3Normalize(&vLightDir, &(-m_vLightDir));
    dwLightDir = VectorToColor(&vLightDir);

//    SetDiffuseMapCoords(&m_prSphere[0].vNormal, sizeof(BVERTEX), 
//                       (PTCOORD2)&m_prSphere[0].u1, sizeof(BVERTEX), 
//                       m_dwSphereVertices, &m_mWorld, &m_vLightDir, fAngle);

    hr = m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(BVERTEX), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    memcpy(pData, m_prSphere, m_dwSphereVertices * sizeof(BVERTEX));

    hr = m_pd3drSphere->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

//D3DXMATRIX mWorld;
D3DXMatrixIdentity(&mWorld);
//    WorldVectorToTangentSpace((PTCOORD3)&m_prSphere[0].u2, sizeof(NVERTEX), &m_prSphere[0].vPosition, sizeof(NVERTEX), (PTCOORD2)&m_prSphere[0].u1, sizeof(NVERTEX), m_pwSphere, m_dwSphereIndices / 3, D3DPT_TRIANGLELIST, &mWorld, &m_vLightDir, TRUE);
D3DXVec3Normalize(&m_vLightDir, &m_vLightDir);
    for (UINT i = 0; i < m_dwSphereVertices; i++) {
        m_prSphere[i].u2 = vLightDir.x * m_pmTangent[i]._11 + vLightDir.y * m_pmTangent[i]._21 + vLightDir.z * m_pmTangent[i]._31;
        m_prSphere[i].v2 = vLightDir.x * m_pmTangent[i]._12 + vLightDir.y * m_pmTangent[i]._22 + vLightDir.z * m_pmTangent[i]._32;
        m_prSphere[i].t2 = vLightDir.x * m_pmTangent[i]._13 + vLightDir.y * m_pmTangent[i]._23 + vLightDir.z * m_pmTangent[i]._33;
    }

    // Set textures
    m_pDevice->SetTexture(0, m_pd3dtBump);
    m_pDevice->SetTexture(1, m_pd3dtcNormal);
    m_pDevice->SetTexture(2, m_pd3dtBase);

    m_pDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);

    m_pDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 2);

//    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwLightDir);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_DOTPRODUCT3);
//    SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);


    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_NVERTEX);

//    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(BVERTEX));

//    m_pDevice->SetIndices(m_pd3diSphere, 0);

//    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);
{
WORD pwIndices[6] = {0, 1, 2, 0, 2, 3};
//WorldVectorToTangentSpace((PTCOORD3)&m_prQuad[0].u2, sizeof(NVERTEX), &m_prQuad[0].vPosition, sizeof(NVERTEX), &vNormal, 0, (PTCOORD2)&m_prQuad[0].u1, sizeof(NVERTEX), pwIndices, 2, D3DPT_TRIANGLELIST, &mWorld, &m_vLightDir);
}
//m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prQuad, sizeof(NVERTEX));

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, m_dwSphereIndices / 3, m_pwSphere, D3DFMT_INDEX16, m_prSphere, sizeof(NVERTEX));

    // Release the texture
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    m_pDevice->SetTexture(2, NULL);

/*
{
D3DXVECTOR3 vNorm;
float fDot;
UINT i;
LVERTEX plr[2];
plr[0].cDiffuse = RGB_MAKE(0, 255, 0);
plr[0].cSpecular = 0;
plr[1].cDiffuse = RGB_MAKE(0, 255, 0);
plr[1].cSpecular = 0;

m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
m_pDevice->SetVertexShader(FVF_LVERTEX);
SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_DISABLE);

for (i = 0; i < m_dwSphereVertices; i++) {
plr[0].vPosition = m_prSphere[i].vPosition;
D3DXVec3Normalize(&vNorm, (D3DXVECTOR3*)&m_prSphere[i].u2);
fDot = D3DXVec3Dot(&vNorm, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
if (fDot < 0.1f) fDot = 0.1f;
vNorm *= (5.0f * fDot);
plr[1].vPosition = m_prSphere[i].vPosition + vNorm;
m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, plr, sizeof(LVERTEX));
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
void CBumpNormal1::ProcessInput() {

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
BOOL CBumpNormal1::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -40.0f);
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
BOOL CBumpNormal1::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
