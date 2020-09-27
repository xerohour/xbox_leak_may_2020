/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    dp3lite.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "..\..\inc\d3dlocus.h"
#include "dp3lite.h"

//******************************************************************************
// Function prototypes
//******************************************************************************
/*
BOOL                            CreateSphere(float fRadius, PVERTEX* pprVertices, 
                                    LPDWORD pdwNumVertices, LPWORD* ppwIndices, 
                                    LPDWORD pdwNumIndices);
void                            ReleaseSphere(PVERTEX* ppr, LPWORD* ppw);
*/
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

    CDiffuseMap* pTest;
    BOOL         bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CDiffuseMap();
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

    if (!(pd3dcaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP)) {
        return FALSE;
    }
    return TRUE;
}

//******************************************************************************
// CScene
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDiffuseMap
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
CDiffuseMap::CDiffuseMap() {

    m_pd3dtcNorm = NULL;
    m_pd3dtcHVec = NULL;
    m_pd3drSphere2 = NULL;
    m_pd3drSphere0 = NULL;
    m_pd3diSphere = NULL;
    m_prSphere2 = NULL;
    m_prSphere0 = NULL;
    m_dwSphereVertices;
    m_pwSphere = NULL;
    m_dwSphereIndices;
    m_vLightDir = D3DXVECTOR3(0.5f, -0.0f, 1.0f);
    m_fAngle = 0.0f;
}

//******************************************************************************
//
// Method:
//
//     ~CDiffuseMap
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
CDiffuseMap::~CDiffuseMap() {

    ReleaseSphere(&m_prSphere2, &m_pwSphere);
    MemFree32(&m_prSphere0);
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
BOOL CDiffuseMap::Create(CDisplay* pDisplay) {

    UINT i;

    m_mWorld = m_mIdentity;

    m_vSpherePos[0] = D3DXVECTOR3(-12.0f, 7.0f, 0.0f);
    m_vSpherePos[1] = D3DXVECTOR3(0.0f, 7.0f, 0.0f);
    m_vSpherePos[2] = D3DXVECTOR3(12.0f, 7.0f, 0.0f);
    m_vSpherePos[3] = D3DXVECTOR3(-12.0f, -7.0f, 0.0f);
    m_vSpherePos[4] = D3DXVECTOR3(0.0f, -7.0f, 0.0f);
    m_vSpherePos[5] = D3DXVECTOR3(12.0f, -7.0f, 0.0f);

    // Initialize the vertices
    CreateSphere(&m_prSphere2, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices, 5.0f);
    m_prSphere0 = (PCVERTEX)MemAlloc32(m_dwSphereVertices * sizeof(CVERTEX));
    if (!m_prSphere0) {
        return FALSE;
    }
    for (i = 0; i < m_dwSphereVertices; i++) {
        m_prSphere0[i].vPosition = m_prSphere2[i].vPosition;
        m_prSphere0[i].vNormal = m_prSphere2[i].vNormal;
    }

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
int CDiffuseMap::Exhibit(int *pnExitCode) {

    return CScene::Exhibit(pnExitCode);
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
BOOL CDiffuseMap::Initialize() {

    HRESULT hr;

    // Create a directional light
    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
    m_light.Diffuse.r = 1.0f;
    m_light.Diffuse.g = 1.0f;
    m_light.Diffuse.b = 1.0f;
    m_light.Diffuse.a = 1.0f;
    m_light.Specular.r = 1.0f;
    m_light.Specular.g = 1.0f;
    m_light.Specular.b = 1.0f;
    m_light.Specular.a = 1.0f;
    m_light.Ambient.r = 0.0f;
    m_light.Ambient.g = 0.0f;
    m_light.Ambient.b = 0.0f;
    m_light.Ambient.a = 0.0f;
    m_light.Direction = D3DXVECTOR3(0.0f, -1.0f, 1.0f);

    hr = m_pDevice->SetLight(0, &m_light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    // Set a material
    if (!SetMaterial(m_pDevice, RGBA_MAKE(200, 200, 200, 255), RGBA_MAKE(0, 0, 0, 255), RGBA_MAKE(255, 255, 255, 255), 0, 40.0f)) {
        return FALSE;
    }

    m_pd3dtcNorm = CreateNormalMap(m_pDevice, 256, 0.05f, D3DFMT_A8R8G8B8, TRUE);
    if (!m_pd3dtcNorm) {
        return FALSE;
    }

    m_pd3dtcHVec = CreateHalfVectorMap(m_pDevice, &m_vLightDir, 32, D3DFMT_A8R8G8B8, TRUE);
    if (!m_pd3dtcHVec) {
        return FALSE;
    }

    // Initialize the vertices
    m_pd3drSphere2 = CreateVertexBuffer(m_pDevice, m_prSphere2, m_dwSphereVertices * sizeof(VERTEX), 0, FVF_VERTEX, D3DPOOL_MANAGED);
    if (!m_pd3drSphere2) {
        return FALSE;
    }

    m_pd3drSphere0 = CreateVertexBuffer(m_pDevice, m_prSphere0, m_dwSphereVertices * sizeof(CVERTEX), 0, FVF_CVERTEX, D3DPOOL_MANAGED);
    if (!m_pd3drSphere0) {
        return FALSE;
    }

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
void CDiffuseMap::Efface() {

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
void CDiffuseMap::Update() {

    CAMERA    cam;
    D3DMATRIX mWorld;
    float     fTheta = M_PI / 100.0f;

    CScene::Update();

    m_pDisplay->GetCamera(&cam);
    cam.vPosition.x = 50.0f * (float)sin(m_fAngle);
    cam.vPosition.y = 0.0f;
    cam.vPosition.z = -50.0f * (float)cos(m_fAngle);
    m_pDisplay->SetCamera(&cam);
    m_fAngle += (M_PI / 50.0f) * m_fFrameDelta;
    if (m_fAngle > M_2PI) {
        m_fAngle -= M_2PI;
    }

    // Update the rotation
    fTheta *= m_fFrameDelta;
    InitMatrix(&mWorld,
        (float)(cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)(sin(fTheta)), (float)( 0.0f), (float)(cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

//    m_pDevice->MultiplyTransform(D3DTS_WORLD, &mWorld);
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
BOOL CDiffuseMap::Render() {
    
    D3DMATRIX   mView, mBackView;
    D3DVECTOR   vLightDir;
    DWORD       dwLightDir;
    D3DXVECTOR3 vHalf, vTHalf, vEyeModel, vModelPos;
    DWORD       dwHVec;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
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

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_POINT);

    m_vLightDir.x = (float)cos(m_fAngle);
    m_vLightDir.y = (float)sin(m_fAngle);
    m_vLightDir.z = 1.0f;
    m_vLightDir = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
    D3DXVec3Normalize(&m_vLightDir, &m_vLightDir);

    // Transform the light into camera space
//    m_pDevice->GetTransform(D3DTS_VIEW, &mView);
    CAMERA cam;
    m_pDisplay->GetCamera(&cam);
    SetView((LPD3DXMATRIX)&mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));

    vLightDir.x = m_vLightDir.x * mView._11 + m_vLightDir.y * mView._21 + m_vLightDir.z * mView._31;
    vLightDir.y = m_vLightDir.x * mView._12 + m_vLightDir.y * mView._22 + m_vLightDir.z * mView._32;
    vLightDir.z = m_vLightDir.x * mView._13 + m_vLightDir.y * mView._23 + m_vLightDir.z * mView._33;

    dwLightDir = VectorToColor((D3DXVECTOR3*)&vLightDir);

    //
    // Gouraud shaded lighting with Direct3D lighting engine
    //
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_light.Direction = m_vLightDir;
    m_pDevice->SetLight(0, &m_light);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drSphere2, sizeof(VERTEX));

    m_mWorld._41 = m_vSpherePos[0].x;
    m_mWorld._42 = m_vSpherePos[0].y;
    m_mWorld._43 = m_vSpherePos[0].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    //
    // Diffuse lighting using a surface normal cube map dotproduct3 lighting
    //
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    // Set a texture
    m_pDevice->SetTexture(0, m_pd3dtcNorm);

    m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_CVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3drSphere0, sizeof(CVERTEX));

    m_mWorld._41 = m_vSpherePos[1].x;
    m_mWorld._42 = m_vSpherePos[1].y;
    m_mWorld._43 = m_vSpherePos[1].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);

    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwLightDir);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_TFACTOR, D3DTOP_DOTPRODUCT3);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    // 
    // Specular lighting using a surface normal cube map, a half vector cube map,
    // and dotproduct3 lighting.  The half vector cube map must be updated in response
    // to changes in view direction since the dotproduct3 lighting takes place in
    // camera space and the lighting information in the half vector cube map is encoded
    // in camera space.  When the view changes the light direction relative to the camera
    // also changes and the map needs to be updated.
    //
    m_mWorld._41 = m_vSpherePos[2].x;
    m_mWorld._42 = m_vSpherePos[2].y;
    m_mWorld._43 = m_vSpherePos[2].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    // Update the cubemap with a light direction in camera space
    UpdateHalfVectorMap(m_pd3dtcHVec, &vLightDir);

    m_pDevice->SetTexture(1, m_pd3dtcHVec);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_DOTPRODUCT3);
    SetColorStage(m_pDevice, 2, D3DTA_CURRENT, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    // 
    // Specular lighting using a surface normal cube map, a half vector cube map,
    // and dotproduct3 lighting.  The half vector cube map only needs to be updated in
    // response to changes in light direction.  In this case the dotproduct3 lighting takes
    // place in world space and the lighting information in the half vector cube map is also
    // encoded in world space.  This has the benefit of not having to regenerate the half vector
    // cube map with each change in view direction, but the drawback is that the automatically
    // generated texture coordinates for both the cameraspaceposition and cameraspacenormal
    // must be back-transformed into world space to correctly index into the surface normal and
    // half vector cube maps.  This method is better when the size of the geometry being lit is
    // smaller relative to the dimensions of the half vector cube map.  If the size of the
    // geometry is relatively larger it is better to use the above case, taking the up front 
    // performance hit in recalculating half-vectors in the cube map but saving time in generating
    // texture coordinates for the two texture stages by not having to transform them.
    //
    m_mWorld._41 = m_vSpherePos[3].x;
    m_mWorld._42 = m_vSpherePos[3].y;
    m_mWorld._43 = m_vSpherePos[3].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    // Update the light direction in the half-vector map, but leave the light direction in world space
    UpdateHalfVectorMap(m_pd3dtcHVec, &m_vLightDir);

    InitMatrix(&mBackView,
        mView._11, mView._21, mView._31, 0.0f,
        mView._12, mView._22, mView._32, 0.0f,
        mView._13, mView._23, mView._33, 0.0f,
        0.0f,      0.0f,      0.0f,      1.0f
    );
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTransform(D3DTS_TEXTURE0, &mBackView);
    m_pDevice->SetTransform(D3DTS_TEXTURE1, &mBackView);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    m_pDevice->SetTransform(D3DTS_TEXTURE0, &m_mIdentity);
    m_pDevice->SetTransform(D3DTS_TEXTURE1, &m_mIdentity);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    //
    // Specular lighting using an infinite viewer.  With an infinite viewer the half vectors
    // for all vertices in the model are identical so only a surface normal map and two texture
    // stages are required to perform the lighting (with the second stage used to sharpen the
    // highlight).  The constant half-vector is calculated using an object to eye vector
    // taken from a point at the center of the model and set as the tfactor in the blend.
    //
    m_mWorld._41 = m_vSpherePos[4].x;
    m_mWorld._42 = m_vSpherePos[4].y;
    m_mWorld._43 = m_vSpherePos[4].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    D3DXVec3TransformCoord(&vModelPos, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &m_mWorld);
    D3DXVec3Normalize(&vEyeModel, &(vModelPos - cam.vPosition));
    D3DXVec3Normalize(&vHalf, &(vEyeModel + m_vLightDir));

    vTHalf.x = vHalf.x * mView._11 + vHalf.y * mView._21 + vHalf.z * mView._31;
    vTHalf.y = vHalf.x * mView._12 + vHalf.y * mView._22 + vHalf.z * mView._32;
    vTHalf.z = vHalf.x * mView._13 + vHalf.y * mView._23 + vHalf.z * mView._33;

    dwHVec = VectorToColor((D3DXVECTOR3*)&vTHalf);

    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwHVec);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_TFACTOR, D3DTOP_DOTPRODUCT3);
    SetColorStage(m_pDevice, 1, D3DTA_CURRENT, D3DTA_CURRENT, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    // 
    // Two pass lighting for diffuse and specular.  The first pass generates the diffuse lighting
    // using a surface normal cube map and dotproduct3 lighting.  The second pass adds specular
    // using a surface normal cube map, a half vector cube map, and dotproduct3 lighting.  The
    // two passes are blended together to add the specular to the diffuse light.
    //
    m_mWorld._41 = m_vSpherePos[5].x;
    m_mWorld._42 = m_vSpherePos[5].y;
    m_mWorld._43 = m_vSpherePos[5].z;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwLightDir);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_TFACTOR, D3DTOP_DOTPRODUCT3);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    // Update the cubemap with a light direction in camera space
    UpdateHalfVectorMap(m_pd3dtcHVec, &vLightDir);

    m_pDevice->SetTexture(1, m_pd3dtcHVec);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_DOTPRODUCT3);
    SetColorStage(m_pDevice, 2, D3DTA_CURRENT, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 3, D3DTA_CURRENT, D3DTA_CURRENT, D3DTOP_MODULATE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, RGBA_MAKE(128, 128, 128, 128));

    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_TFACTOR, D3DTOP_SELECTARG2);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);


    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

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

/*

TODO: Draw the following spheres:

a sphere lit by gouraud diffuse and specular light
a sphere lit by per-pixel diffuse and gouraud specular light
a sphere lit by per-pixel diffuse/specular light updating the halfvector map in response to view changes
a sphere lit by per-pixel diffuse/specular light backtransforming the texcoords in both stages to put things in view's coord system
a sphere lit by per-pixel diffuse/specular light assuming an infinite viewer (i.e. without relying on the halfvector map)
s sphere lit by per-pixel diffuse/specular light using a pixel shader instead of halfvector map
spheres with higher specular powers using a custom pixel shader applied to diffusely lit spheres
make them all translucent..use dotproduct3 in the alpha channel to alter the level of translucency
make them all have textures?

vertextoeye = (eye - vertex)


H = Normalize(L + Normalize(V)) or H = Normalize(V) + L


tfactor = lightdir

1: texture = normal map
1: texture dot tfactor

2: texture = halfvector map
2: texture dot current

OR

1: texture = normal map
1: texture dot tfactor

2: texture = halfvector map
2: texture dot current

PROBABLY:

1: texture = normal map
1: coord = cameraspacenormal
1: select texture
2: texture = halfvector map
2: coord = cameraspaceposition?
2: texture dot current
3: current modulate current (for specular power of 2, things like modulate2x or modulate4x could also be used)

OR for infinite viewer

tfactor1 = backtransform(normalize(normalize(eye - vertex at center of model) + lightdir))
1: texture = normal map
1: coord = cameraspacenormal
1: texture dot tfactor
2: current modulate current

saves one stage at the expense of accuracy (acts like parallel point light)

*/

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
void CDiffuseMap::ProcessInput() {

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
BOOL CDiffuseMap::InitView() {

//    return CScene::InitView();

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
BOOL CDiffuseMap::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
/*
//******************************************************************************
BOOL CreateSphere(float fRadius, PVERTEX* pprVertices, LPDWORD pdwNumVertices,
                  LPWORD* ppwIndices, LPDWORD pdwNumIndices) {

    PVERTEX     prVertices;
    PVERTEX     pr;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    UINT        uIndex = 0;
    UINT        uStepsU = 16, uStepsV = 16;
    UINT        i, j;
    float       fX, fY, fTX, fSinY, fCosY;

    if (!pprVertices || !pdwNumVertices || !ppwIndices || !pdwNumIndices) {
        return FALSE;
    }

    *pprVertices = NULL;
    *pdwNumVertices = 0;
    *ppwIndices = NULL;
    *pdwNumIndices = 0;

    dwNumVertices = (uStepsU + 1) * uStepsV;

    // Allocate memory for the vertices
    prVertices = (PVERTEX)MemAlloc32(dwNumVertices * sizeof(VERTEX));
    if (!prVertices) {
        return FALSE;
    }

    // Allocate memory for the indices
    dwNumIndices = uStepsU * (uStepsV - 1) * 6;

    pwIndices = (LPWORD)MemAlloc32(dwNumIndices * sizeof(WORD));
    if (!pwIndices) {
        MemFree32(prVertices);
        return FALSE;
    }

    // Create the sphere
    for (j = 0; j < uStepsV; j++) {

        fY = (float)j / (float)(uStepsV - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i <= uStepsU; i++) {

            pr = &prVertices[(uStepsU + 1) * j + i];
            fX = (float)i / (float)uStepsU;
            fTX = fX * M_2PI;

            pr->vNormal = D3DXVECTOR3((float)cos(fTX) * fSinY, fCosY, (float)sin(fTX) * fSinY);
            pr->vPosition = pr->vNormal * fRadius;
            pr->u0 = fX * 2.0f;
            pr->v0 = fY;
        }
    }

    for (j = 0; j < uStepsV - 1; j++) {

        for (i = 0; i < uStepsU; i++) {

            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i;
            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i;
            pwIndices[uIndex++] = j * (uStepsU + 1) + i + 1;
            pwIndices[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
        }
    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void ReleaseSphere(PVERTEX* ppr, LPWORD* ppw) {

    if (ppr && *ppr) {
        MemFree32(*ppr);
        *ppr = NULL;
    }
    if (ppw && *ppw) {
        MemFree32(*ppw);
        *ppw = NULL;
    }
}
*/
