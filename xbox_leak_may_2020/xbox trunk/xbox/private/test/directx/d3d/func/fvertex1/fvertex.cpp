/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    fvertex.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "fvertex.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#ifdef UNDER_CE
#define TEST_INVALID_FLAGS(x) \
    __try { \
        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, (x), mesh.prVertices, mesh.uNumVertices, mesh.pwIndices, mesh.uNumIndices, 0); \
        if (hr != DDERR_INVALIDPARAMS) { \
            DebugString(TEXT("IDirect3DDevice3::DrawIndexedPrimitive returned 0x%X instead of DDERR_INVALIDPARAMS"), hr); \
        } \
    } \
    __except(EXCEPTION_EXECUTE_HANDLER) { \
        DebugString(TEXT("IDirect3DDevice3::DrawIndexedPrimitive threw an exception for invalid FVF ") TEXT(#x)); \
    }
#else
#define TEST_INVALID_FLAGS(x) \
    hr = m_pDevice->SetVertexShader(x); \
    if (hr != DDERR_INVALIDPARAMS) { \
        DebugString(TEXT("IDirect3DDevice8::SetVertexShader returned 0x%X instead of DDERR_INVALIDPARAMS"), hr); \
    }
#endif // UNDER_CE

#define RAND_COLOR                 (RGB_MAKE(rand() % 256, rand() % 256, rand() % 256))

//******************************************************************************
//
// Function:
//
//     TexProjectXY
//
// Description:
//
//     Initialize the texture coordinates of the given mesh for a parallel
//     projection onto the XY plane.
//
// Arguments:
//
//     LPD3DVERTEX pr           - Array of vertices on which to project the 
//                                texture
//
//     UINT uNumVertices        - Number of vertices in the array
//
//     float fX1             - Reference value of x at which tu will be zero
//
//     float fX2             - Reference value of x at which tu will be one
//
//     float fY1             - Reference value of y at which tv will be zero
//
//     float fY2             - Reference value of y at which tv will be one
//
// Return Value:
//
//     None.
//
//******************************************************************************
void TexProjectXY(PVERTEX pr, UINT uNumVertices,
                      float fX1, float fX2, float fY1, float fY2) 
{
    float       fWidth, fHeight;
    UINT        i;

    if (!pr) {
        return;
    }

    fWidth = fX2 - fX1;
    fHeight = fY2 - fY1;

    for (i = 0; i < uNumVertices; i++) {

        pr[i].u0 = (pr[i].vPosition.x - fX1) / fWidth;
        pr[i].v0 = (pr[i].vPosition.y - fY1) / fHeight;
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
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CFVertex*   pFVertex;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pFVertex = new CFVertex();
    if (!pFVertex) {
        return FALSE;
    }

    // Initialize the scene
    if (!pFVertex->Create(pDisplay)) {
        pFVertex->Release();
        return FALSE;
    }

    bRet = pFVertex->Exhibit(pnExitCode);

    // Clean up the scene
    pFVertex->Release();

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
// CFVertex
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CFVertex
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
CFVertex::CFVertex() {

    m_pr1 = NULL;
    m_pr2 = NULL;
    m_pr3 = NULL;
    m_pr4 = NULL;

    m_plr1 = NULL;
    m_plr2 = NULL;
    m_plr3 = NULL;
    m_plr4 = NULL;
    m_plr5 = NULL;

    m_ptlr1 = NULL;
    m_ptlr2 = NULL;
    m_ptlr3 = NULL;
    m_ptlr4 = NULL;

    memset(&m_mesh, 0, sizeof(MESH));
}

//******************************************************************************
//
// Method:
//
//     ~CFVertex
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
CFVertex::~CFVertex() {

    if (m_pr1) delete [] m_pr1;
    if (m_pr2) delete [] m_pr2;
    if (m_pr3) delete [] m_pr3;
    if (m_pr4) delete [] m_pr4;
    if (m_plr1) delete [] m_plr1;
    if (m_plr2) delete [] m_plr2;
    if (m_plr3) delete [] m_plr3;
    if (m_plr4) delete [] m_plr4;
    if (m_plr5) delete [] m_plr5;
    if (m_ptlr1) delete [] m_ptlr1;
    if (m_ptlr2) delete [] m_ptlr2;
    if (m_ptlr3) delete [] m_ptlr3;
    if (m_ptlr4) delete [] m_ptlr4;
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
BOOL CFVertex::Create(CDisplay* pDisplay) {

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
int CFVertex::Exhibit(int *pnExitCode) {

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
BOOL CFVertex::Prepare() {

    UINT i;

    if (!CreateMesh(TEXT("lteapot.xdg"), &m_mesh)) {
        return FALSE;
    }

    TexProjectXY(m_mesh.prVertices, m_mesh.uNumVertices, -6.0f, 6.0f, -6.0f, 6.0f);

    // Allocate memory for the various vertex types
    m_pr1 = new VERTEX_XYZ_NORMAL[m_mesh.uNumVertices];
    if (!m_pr1) {
        return FALSE;
    }
    m_pr2 = new VERTEX_XYZ_NORMAL_TEX1[m_mesh.uNumVertices];
    if (!m_pr2) {
        return FALSE;
    }
    m_pr3 = new VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR[m_mesh.uNumVertices];
    if (!m_pr3) {
        return FALSE;
    }
    m_pr4 = new VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2[m_mesh.uNumVertices];
    if (!m_pr4) {
        return FALSE;
    }
    m_plr1 = new VERTEX_XYZ_TEX1[m_mesh.uNumVertices];
    if (!m_plr1) {
        return FALSE;
    }
    m_plr2 = new VERTEX_XYZ_DIFFUSE[m_mesh.uNumVertices];
    if (!m_plr2) {
        return FALSE;
    }
    m_plr3 = new VERTEX_XYZ_SPECULAR[m_mesh.uNumVertices];
    if (!m_plr3) {
        return FALSE;
    }
    m_plr4 = new VERTEX_XYZ_DIFFUSE_SPECULAR[m_mesh.uNumVertices];
    if (!m_plr4) {
        return FALSE;
    }
    m_plr5 = new VERTEX_XYZ_RESERVED1_DIFFUSE_TEX3[m_mesh.uNumVertices];
    if (!m_plr5) {
        return FALSE;
    }
    m_ptlr1 = new VERTEX_XYZRHW_SPECULAR[m_mesh.uNumVertices];
    if (!m_ptlr1) {
        return FALSE;
    }
    m_ptlr2 = new VERTEX_XYZRHW_TEX4[m_mesh.uNumVertices];
    if (!m_ptlr2) {
        return FALSE;
    }
    m_ptlr3 = new VERTEX_XYZRHW_DIFFUSE_SPECULAR[m_mesh.uNumVertices];
    if (!m_ptlr3) {
        return FALSE;
    }
    m_ptlr4 = new VERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4[m_mesh.uNumVertices];
    if (!m_ptlr4) {
        return FALSE;
    }

    for (i = 0; i < m_mesh.uNumVertices; i++) {

        m_pr1[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_pr1[i].vNormal = *((D3DVECTOR*)(&m_mesh.prVertices[i]) + 1);

        m_pr2[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_pr2[i].vNormal = *((D3DVECTOR*)(&m_mesh.prVertices[i]) + 1);
        m_pr2[i].u0 = m_mesh.prVertices[i].u0;
        m_pr2[i].v0 = m_mesh.prVertices[i].v0;

        m_pr3[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_pr3[i].vNormal = *((D3DVECTOR*)(&m_mesh.prVertices[i]) + 1);
        m_pr3[i].cDiffuse = RGB_MAKE(255, 0, 0);
        m_pr3[i].cSpecular = RAND_COLOR;

        m_pr4[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_pr4[i].vNormal = *((D3DVECTOR*)(&m_mesh.prVertices[i]) + 1);
        m_pr4[i].cDiffuse = RGB_MAKE(0, 0, 255);
        m_pr4[i].cSpecular = RAND_COLOR;
        m_pr4[i].u0 = m_mesh.prVertices[i].u0;
        m_pr4[i].v0 = m_mesh.prVertices[i].v0;
        m_pr4[i].u1 = m_mesh.prVertices[i].u0;
        m_pr4[i].v1 = m_mesh.prVertices[i].v0;

        m_plr1[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_plr1[i].u0 = m_mesh.prVertices[i].u0;
        m_plr1[i].v0 = m_mesh.prVertices[i].v0;

        m_plr2[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_plr2[i].cDiffuse = RGB_MAKE(255, 0, 0);

        m_plr3[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_plr3[i].cSpecular = RGB_MAKE(0, 0, 255);

        m_plr4[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_plr4[i].cDiffuse = RGB_MAKE(255, 0, 0);
        m_plr4[i].cSpecular = RGB_MAKE(0, 0, 255);

        m_plr5[i].vPosition = *(D3DVECTOR*)(&m_mesh.prVertices[i]);
        m_plr5[i].cDiffuse = RGB_MAKE(255, 0, 0);
        m_plr5[i].u0 = m_mesh.prVertices[i].u0;
        m_plr5[i].v0 = m_mesh.prVertices[i].v0;
        m_plr5[i].u1 = m_mesh.prVertices[i].u0;
        m_plr5[i].v1 = m_mesh.prVertices[i].v0;
        m_plr5[i].u2 = m_mesh.prVertices[i].u0;
        m_plr5[i].v2 = m_mesh.prVertices[i].v0;

        m_ptlr1[i].cSpecular = RGB_MAKE(0, 0, 255);

        m_ptlr2[i].u0 = m_mesh.prVertices[i].u0;
        m_ptlr2[i].v0 = m_mesh.prVertices[i].v0;
        m_ptlr2[i].u1 = m_mesh.prVertices[i].u0;
        m_ptlr2[i].v1 = m_mesh.prVertices[i].v0;
        m_ptlr2[i].u2 = m_mesh.prVertices[i].u0;
        m_ptlr2[i].v2 = m_mesh.prVertices[i].v0;
        m_ptlr2[i].u3 = m_mesh.prVertices[i].u0;
        m_ptlr2[i].v3 = m_mesh.prVertices[i].v0;

        m_ptlr3[i].cDiffuse = RGB_MAKE(255, 0, 0);
        m_ptlr3[i].cSpecular = RGB_MAKE(0, 0, 255);

        m_ptlr4[i].cDiffuse = RGB_MAKE(255, 0, 0);
        m_ptlr4[i].cSpecular = RGB_MAKE(0, 0, 255);
        m_ptlr4[i].u0 = m_mesh.prVertices[i].u0;
        m_ptlr4[i].v0 = m_mesh.prVertices[i].v0;
        m_ptlr4[i].u1 = m_mesh.prVertices[i].u0;
        m_ptlr4[i].v1 = m_mesh.prVertices[i].v0;
        m_ptlr4[i].u2 = m_mesh.prVertices[i].u0;
        m_ptlr4[i].v2 = m_mesh.prVertices[i].v0;
        m_ptlr4[i].u3 = m_mesh.prVertices[i].u0;
        m_ptlr4[i].v3 = m_mesh.prVertices[i].v0;
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
BOOL CFVertex::Setup() {

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
BOOL CFVertex::Initialize() {

    UINT i;

    // Create a texture
    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"));
    if (!m_pd3dt) {
        return FALSE;
    }

    for (i = 0; i < 4; i++) {
        m_pDevice->SetTexture(i, m_pd3dt);
        m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
    }

    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

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
void CFVertex::Efface() {

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
void CFVertex::Update() {

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
BOOL CFVertex::Render() {

    D3DXMATRIX mWorld;
    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    mWorld = m_mIdentity;

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
/*
    // Invalid vertex format combinations
#ifdef DEBUG
    TEST_INVALID_FLAGS(0);
    TEST_INVALID_FLAGS(D3DFVF_XYZ | D3DFVF_XYZRHW);
    TEST_INVALID_FLAGS(D3DFVF_XYZRHW | D3DFVF_NORMAL);
    TEST_INVALID_FLAGS(D3DFVF_TEX1);
    TEST_INVALID_FLAGS(D3DFVF_DIFFUSE);
    TEST_INVALID_FLAGS(D3DFVF_SPECULAR);
    TEST_INVALID_FLAGS(D3DFVF_RESERVED1);
    TEST_INVALID_FLAGS(D3DFVF_TEX8 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
    TEST_INVALID_FLAGS(D3DFVF_XYZ | D3DFVF_RESERVED1);
    TEST_INVALID_FLAGS(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_RESERVED1);
    TEST_INVALID_FLAGS(D3DFVF_XYZRHW | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE);
#endif // DEBUG
*/

    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

    // Render the untransformed, unlit vertices
    mWorld._41 = -20.0f;
    mWorld._42 = 10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_NORMAL);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_pr1, sizeof(VERTEX_XYZ_NORMAL));

    mWorld._41 = -10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_NORMAL_TEX1);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_pr2, sizeof(VERTEX_XYZ_NORMAL_TEX1));

    mWorld._41 = 0.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_NORMAL_DIFFUSE_SPECULAR);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_pr3, sizeof(VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR));

    mWorld._41 = 10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_pr4, sizeof(VERTEX_XYZ_NORMAL_DIFFUSE_SPECULAR_TEX2));

    // Render the untransformed, lit vertices
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    mWorld._41 = -20.0f;
    mWorld._42 = 0.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_TEX1);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_plr1, sizeof(VERTEX_XYZ_TEX1));

    mWorld._41 = -10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_DIFFUSE);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_plr2, sizeof(VERTEX_XYZ_DIFFUSE));

    mWorld._41 = 0.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

//        SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
//        m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_SPECULAR);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_plr3, sizeof(VERTEX_XYZ_SPECULAR));

    mWorld._41 = 10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

//        SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
//        m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_DIFFUSE_SPECULAR);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_plr4, sizeof(VERTEX_XYZ_DIFFUSE_SPECULAR));

    mWorld._41 = 20.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);

    m_pDevice->SetVertexShader(D3DFVF_XYZ_RESERVED_DIFFUSE_TEX3);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_plr5, sizeof(VERTEX_XYZ_RESERVED1_DIFFUSE_TEX3));

    // Render the transformed, lit vertices
    mWorld._41 = -20.0f;
    mWorld._42 = -10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    TransformVertices(m_pDevice, &m_ptlr1[0].vPosition, &m_mesh.prVertices[0],
                        m_mesh.uNumVertices, sizeof(VERTEX_XYZRHW_SPECULAR),
                        sizeof(VERTEX));

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetVertexShader(D3DFVF_XYZRHW_SPECULAR);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_ptlr1, sizeof(VERTEX_XYZRHW_SPECULAR));

    mWorld._41 = -10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    TransformVertices(m_pDevice, &m_ptlr2[0].vPosition, &m_mesh.prVertices[0],
                        m_mesh.uNumVertices, sizeof(VERTEX_XYZRHW_TEX4),
                        sizeof(VERTEX));

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->SetVertexShader(D3DFVF_XYZRHW_TEX4);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_ptlr2, sizeof(VERTEX_XYZRHW_TEX4));

    mWorld._41 = 0.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    TransformVertices(m_pDevice, &m_ptlr3[0].vPosition, &m_mesh.prVertices[0],
                        m_mesh.uNumVertices, sizeof(VERTEX_XYZRHW_DIFFUSE_SPECULAR),
                        sizeof(VERTEX));

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    m_pDevice->SetVertexShader(D3DFVF_XYZRHW_DIFFUSE_SPECULAR);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_ptlr3, sizeof(VERTEX_XYZRHW_DIFFUSE_SPECULAR));

    mWorld._41 = 10.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

    TransformVertices(m_pDevice, &m_ptlr4[0].vPosition, &m_mesh.prVertices[0],
                        m_mesh.uNumVertices, sizeof(VERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4),
                        sizeof(VERTEX));

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
    SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);

    m_pDevice->SetVertexShader(D3DFVF_XYZRHW_DIFFUSE_SPECULAR_TEX4);

    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_mesh.uNumVertices,
                m_mesh.uNumIndices / 3, m_mesh.pwIndices, D3DFMT_INDEX16, 
                m_ptlr4, sizeof(VERTEX_XYZRHW_DIFFUSE_SPECULAR_TEX4));

    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);

//        RenderMesh(m_pDevice, &mesh);

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
void CFVertex::ProcessInput() {

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
BOOL CFVertex::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -62.0f);
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
BOOL CFVertex::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
