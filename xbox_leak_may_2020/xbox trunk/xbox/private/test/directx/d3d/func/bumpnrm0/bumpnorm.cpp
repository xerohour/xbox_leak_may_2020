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
BOOL CalculateTangentTerms(D3DXVECTOR3* pvTangent, 
                                D3DXVECTOR3* pvBinormal, D3DVECTOR* pvPosition, 
                                DWORD dwStridePosition, PTCOORD2 ptcTCoord, 
                                DWORD dwStrideTCoord, LPWORD pwIndices, 
                                DWORD dwNumPrimitives, D3DPRIMITIVETYPE d3dpt, 
                                BOOL bInterpolate);

//******************************************************************************
BOOL ComputeTangentTransforms(PMATRIX3X3 pmTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld,
                                BOOL bInterpolate);

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
    m_pd3dtcNormal = NULL;

    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;
    m_prSphere = NULL;
    m_pwSphere = NULL;
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

    PBUMPVERTEX prVertices;
    UINT        i;
    HRESULT     hr;
    DWORD       dwSDecl[] = {
                    D3DVSD_STREAM(0),
                    D3DVSD_REG(0, D3DVSDT_FLOAT3),
                    D3DVSD_REG(1, D3DVSDT_FLOAT2),
                    D3DVSD_REG(2, D3DVSDT_FLOAT3),
                    D3DVSD_REG(3, D3DVSDT_FLOAT3),
                    D3DVSD_REG(4, D3DVSDT_FLOAT3),
                    D3DVSD_END()
                };
#ifdef UNDER_XBOX
    D3DPIXELSHADERDEF d3dpsd;
#endif

    m_mWorld = m_mIdentity;

    m_vLightDir = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

    // Create a texture
//    m_pd3dt = CreateTexture(TEXT("Texture"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP, NULL, 1.0f, RGB_MAKE(0, 0, 0));
//    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("fscaleb.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("base.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
//m_pd3dt = CreateDiffuseMap(256, 256, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
    if (!m_pd3dtBase) {
        return FALSE;
    }

//    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("bump4.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP);
    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("bump.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP);
    if (!m_pd3dtBump) {
        return FALSE;
    }

    m_pd3dtcNormal = (CCubeTexture8*)CreateNormalMap(m_pDevice);
    if (!m_pd3dtcNormal) {
        return FALSE;
    }

    CreateSphere(&m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices, 10.0f, 16, 16, 6.0f, 4.0f);
    if (!m_prSphere) {
        return FALSE;
    }

    hr = m_pDevice->CreateVertexBuffer(m_dwSphereVertices * sizeof(BUMPVERTEX), 0, FVF_BUMPVERTEX, D3DPOOL_DEFAULT, &m_pd3drSphere);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));
        return FALSE;
    }

    hr = m_pd3drSphere->Lock(0, 0, (LPBYTE*)&prVertices, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    for (i = 0; i < m_dwSphereVertices; i++) {
        prVertices[i].vPosition = m_prSphere[i].vPosition;
        prVertices[i].u0 = m_prSphere[i].u0;
        prVertices[i].v0 = m_prSphere[i].v0;
    }

    ComputeTangentTransforms((PMATRIX3X3)&prVertices[0].vTangent.x, sizeof(BUMPVERTEX), &prVertices[0].vPosition, sizeof(BUMPVERTEX), (PTCOORD2)&prVertices[0].u0, sizeof(BUMPVERTEX), m_pwSphere, m_dwSphereIndices / 3, D3DPT_TRIANGLELIST, &m_mWorld, TRUE);

    hr = m_pd3drSphere->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }

    m_dwVShader = CreateVertexShader(m_pDevice, dwSDecl, TEXT("speculr8.vsh"));
    if (m_dwVShader == INVALID_SHADER_HANDLE) {
        return FALSE;
    }

    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));
    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(2, PS_COMBINERCOUNT_SAME_C0);
    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_CUBEMAP, 
                                            PS_TEXTUREMODES_CUBEMAP);
    d3dpsd.PSRGBInputs[0] = PS_COMBINERINPUTS(
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T2 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T3 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_R1, PS_REGISTER_DISCARD,
                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA);

    d3dpsd.PSRGBInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    // Ambient light
    d3dpsd.PSConstant0[0] = 0xFF404040;

    d3dpsd.PSAlphaInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_V0, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB, 
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB,
                                PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_T0 | PS_CHANNEL_ALPHA,
                                0);

    hr = m_pDevice->CreatePixelShader(&d3dpsd, &m_dwPShader);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

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

    ReleaseVertexShader(m_pDevice, m_dwVShader);
    ReleasePixelShader(m_pDevice, m_dwPShader);
    ReleaseTexture(m_pd3dtBase);
    ReleaseTexture(m_pd3dtBump);
    ReleaseTexture(m_pd3dtcNormal);
    ReleaseVertexBuffer(m_pd3drSphere);
    ReleaseIndexBuffer(m_pd3diSphere);
    ReleaseSphere(&m_prSphere, &m_pwSphere);
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

    D3DMATRIX mWorld;
    float     fTheta;
//    float  fTheta = M_PI / 25.0f;

    fTheta = M_PI / 75.0f * m_fFrameDelta;

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
    D3DXMATRIX   mTransform, mView, mProj, mBackWorld;
    D3DXVECTOR4  vLightDir, vEyePos;
    CAMERA       cam;
    LPBYTE       pData;
    UINT         i;
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
//    m_pDisplay->SetView();

    m_pDisplay->GetCamera(&cam);

    SetView((LPD3DXMATRIX)&mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));
    SetPerspectiveProjection(&mProj, cam.fNearPlane, cam.fFarPlane, 
            cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    D3DXMatrixMultiply(&mTransform, &mView, &mProj);
    D3DXMatrixTranspose(&mTransform, &mTransform);

    m_pDevice->SetVertexShaderConstant(0, &mTransform, 4);

    InitMatrix(&mBackWorld,
        m_mWorld._11, m_mWorld._21, m_mWorld._31, 0.0f,
        m_mWorld._12, m_mWorld._22, m_mWorld._32, 0.0f,
        m_mWorld._13, m_mWorld._23, m_mWorld._33, 0.0f,
        0.0f,         0.0f,         0.0f,         1.0f
    );

{
    static float fAngle = 0.0f;

    m_vLightDir.x = (float)(cos(fAngle));
    m_vLightDir.z = (float)(sin(fAngle));
    m_vLightDir.y = -0.75f;

//m_vLightDir = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
//m_vLightDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
//m_vLightDir = D3DXVECTOR3(0.0f, 1.0f, 1.0f);
//    fAngle += M_PI / 200.0f;
    fAngle += M_PI / 200.0f;
    if (fAngle > M_2PI) {
        fAngle -= M_2PI;
    }
}

    D3DXVec3Normalize((D3DXVECTOR3*)&vLightDir, &(-m_vLightDir));
    D3DXVec3TransformCoord((D3DXVECTOR3*)&vLightDir, (D3DXVECTOR3*)&vLightDir, &mBackWorld);
    m_pDevice->SetVertexShaderConstant(4, &vLightDir, 1);

    // Back transform the eye position into model space (difficult since it requires both rotation and translation)
// TODO: Add full back transformation of eye position into model space
    D3DXVec3TransformCoord((D3DXVECTOR3*)&vEyePos, &cam.vPosition, &mBackWorld);
    m_pDevice->SetVertexShaderConstant(5, &vEyePos, 1);

//    m_pDevice->SetVertexShaderConstant(6, &D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f), 1);

    // Set textures
    m_pDevice->SetTexture(0, m_pd3dtBase);
    m_pDevice->SetTexture(1, m_pd3dtBump);
    m_pDevice->SetTexture(2, m_pd3dtcNormal);
    m_pDevice->SetTexture(3, m_pd3dtcNormal);

    for (i = 1; i < 4; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT);
        m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    }

    // REVIEW: Things might be slightly sped up if instead of replicating the base/bump
    // texture coordinates in the vertex shader to oT0 and oT1, the texcoordindex
    // is instead set here to 0 for both stage 0 and 1.  Then the coordinates in the
    // vertex shader only need to be propagated to oT0.
    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    m_pDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
    m_pDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);

    m_pDevice->SetVertexShader(m_dwVShader);
    m_pDevice->SetPixelShader(m_dwPShader);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(BUMPVERTEX));

    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    m_pDevice->SetVertexShader(NULL);
    m_pDevice->SetPixelShader(NULL);

    // Release the texture
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    m_pDevice->SetTexture(2, NULL);
    m_pDevice->SetTexture(3, NULL);

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

//******************************************************************************
// Calculate non-normalized tangent and binormal vector terms for use in
// constructing an othonormal basis transform to rotate a vector from model
// space to tangent space.  This function assumes two vector arrays have been
// allocated, each with at least as many elements as three times the given 
// number of primitives.  These arrays will hold the calculated vector terms.
//******************************************************************************
BOOL CalculateTangentTerms(D3DXVECTOR3* pvTangent, D3DXVECTOR3* pvBinormal, 
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, BOOL bInterpolate)
{
    D3DXVECTOR3     vEdge0, vEdge1, vTemp;
    D3DXVECTOR3     vPlane[3];
    D3DXVECTOR3     *pvP[3];
    PTCOORD2        ptc2[3];
    WORD            w0, w1, w2;
    float           fTemp;
    UINT            uNumIndices;
    UINT            i, j, k;

    uNumIndices = 3 * dwNumPrimitives;

    memset(pvTangent, 0, uNumIndices * sizeof(D3DXVECTOR3));
    memset(pvBinormal, 0, uNumIndices * sizeof(D3DXVECTOR3));

    // Calculate the tangent and binormal vectors for each vertex.  If the vertex is
    // indexed by more than one triangle, add the vectors for each triangle together
    // to obtain an average of the vectors for all triangles formed by the vertex.
    for (i = 0; i < uNumIndices; i+=3) {

        w0 = pwIndices[i];
        w1 = pwIndices[i+1];
        w2 = pwIndices[i+2];

        pvP[0] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w0 * dwStridePosition);
        pvP[1] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w1 * dwStridePosition);
        pvP[2] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w2 * dwStridePosition);
        ptc2[0] = (PTCOORD2)((LPBYTE)ptcTCoord + w0 * dwStrideTCoord);
        ptc2[1] = (PTCOORD2)((LPBYTE)ptcTCoord + w1 * dwStrideTCoord);
        ptc2[2] = (PTCOORD2)((LPBYTE)ptcTCoord + w2 * dwStrideTCoord);

        vEdge0 = D3DXVECTOR3(pvP[1]->x - pvP[0]->x, ptc2[1]->u - ptc2[0]->u, ptc2[1]->v - ptc2[0]->v);
        vEdge1 = D3DXVECTOR3(pvP[2]->x - pvP[0]->x, ptc2[2]->u - ptc2[0]->u, ptc2[2]->v - ptc2[0]->v);
        D3DXVec3Cross(&vPlane[0], &vEdge0, &vEdge1);
        vEdge0.x = pvP[1]->y - pvP[0]->y;
        vEdge1.x = pvP[2]->y - pvP[0]->y;
        D3DXVec3Cross(&vPlane[1], &vEdge0, &vEdge1);
        vEdge0.x = pvP[1]->z - pvP[0]->z;
        vEdge1.x = pvP[2]->z - pvP[0]->z;
        D3DXVec3Cross(&vPlane[2], &vEdge0, &vEdge1);

        if (FABS(vPlane[0].x) < 0.000000001f || FABS(vPlane[1].x) < 0.000000001f || FABS(vPlane[2].x) < 0.000000001f) {
            return FALSE;
        }

        vTemp = D3DXVECTOR3(-vPlane[0].y / vPlane[0].x, -vPlane[1].y / vPlane[1].x, -vPlane[2].y / vPlane[1].x);
        pvTangent[w0] += vTemp;
        pvTangent[w1] += vTemp;
        pvTangent[w2] += vTemp;

        vTemp = D3DXVECTOR3(-vPlane[0].z / vPlane[0].x, -vPlane[1].z / vPlane[1].x, -vPlane[2].z / vPlane[1].x);
        pvBinormal[w0] += vTemp;
        pvBinormal[w1] += vTemp;
        pvBinormal[w2] += vTemp;
    }

    if (bInterpolate) {

        D3DXVECTOR3     *pv0, *pv1;
        LPWORD          pwMerge, pwProcessed;
        UINT            uNumMerges, uNumProcessed = 0;

        pwMerge = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwMerge) {
            return FALSE;
        }

        pwProcessed = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwProcessed) {
            MemFree32(pwMerge);
            return FALSE;
        }

        for (i = 0; i < uNumIndices; i++) {

            // Verify pwIndices[i] has to already been processed...if it has continue
            for (j = 0; j < uNumProcessed; j++) {
                if (pwIndices[i] == pwProcessed[j]) {
                    break;
                }
            }
            if (j < uNumProcessed) {
                continue;
            }

            pv0 = (D3DXVECTOR3*)((LPBYTE)pvPosition + pwIndices[i] * dwStridePosition);
            pwMerge[0] = pwIndices[i];
            pwProcessed[uNumProcessed++] = pwIndices[i];
            uNumMerges = 1;

            // Traverse the vertex list, identifying all vertices whose positions are
            // equal to the current vertex position
            for (j = i + 1; j < uNumIndices; j++) {

                pv1 = (D3DXVECTOR3*)((LPBYTE)pvPosition + pwIndices[j] * dwStridePosition);
                fTemp = D3DXVec3LengthSq(&(*pv0 - *pv1));
                if (fTemp < 0.0000001f) {

                    // See whether the matching vertex has already been added to the merge list
                    for (k = 0; k < uNumMerges; k++) {
                        if (pwIndices[j] == pwMerge[k]) {
                            break;
                        }
                    }
                    if (k == uNumMerges) {
                        pwMerge[uNumMerges++] = pwIndices[j];
                        pwProcessed[uNumProcessed++] = pwIndices[j];
                    }
                }
            }

            if (uNumMerges > 1 && uNumMerges < 5) {
                w0 = pwMerge[0];
                D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
                D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);
                for (j = 1; j < uNumMerges; j++) {
                    w1 = pwMerge[j];
                    D3DXVec3Normalize(&pvTangent[w1], &pvTangent[w1]);
                    D3DXVec3Normalize(&pvBinormal[w1], &pvBinormal[w1]);
                    pvTangent[w0] += pvTangent[w1];
                    pvBinormal[w0] += pvBinormal[w1];
                }
                for (j = 1; j < uNumMerges; j++) {
                    pvTangent[pwMerge[j]] = pvTangent[w0];
                    pvBinormal[pwMerge[j]] = pvBinormal[w0];
                }
            }
        }

        MemFree32(pwProcessed);
        MemFree32(pwMerge);
    }

    return TRUE;
}

//******************************************************************************
BOOL ComputeTangentTransforms(PMATRIX3X3 pmTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld,
                                BOOL bInterpolate)
{
    D3DXVECTOR3     vNormal, vTemp;
    D3DXVECTOR3     *pvTangent, *pvBinormal;
    DWORD           dwNumIndices;
    WORD            w0;
    PMATRIX3X3      pmT;
//    float           fDot;
    MATRIX3X3       mZero;
	UINT            i;

    dwNumIndices = dwNumPrimitives * 3;

    memset(&mZero, 0, sizeof(MATRIX3X3));

    for (i = 0; i < dwNumIndices; i++) {
        memcpy((LPBYTE)pmTangent + pwIndices[i] * dwStrideTangent, &mZero, sizeof(MATRIX3X3));
    }

    pvTangent = (D3DXVECTOR3*)MemAlloc32(2 * dwNumIndices * sizeof(D3DXVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + dwNumIndices;

    if (!CalculateTangentTerms(pvTangent, pvBinormal, pvPosition, dwStridePosition, ptcTCoord, dwStrideTCoord, pwIndices, dwNumPrimitives, d3dpt, bInterpolate)) {
        MemFree32(pvTangent);
        return FALSE;
    }

    for (i = 0; i < dwNumIndices; i++) {

        w0 = pwIndices[i];
        pmT = (PMATRIX3X3)((LPBYTE)pmTangent + w0 * dwStrideTangent);
        if (!memcmp(pmT, &mZero, sizeof(MATRIX3X3))) {

//            pvN = (D3DXVECTOR3*)((LPBYTE)pvNormal + w0 * dwStrideNormal);

            D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            D3DXVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

//            fDot = D3DXVec3Dot(&vNormal, pvN);
//            if (fDot < 0.0f) {
//                // Either negate vNormal or pvBinormal...
//                vNormal = -vNormal;
//            }

            pmT->_11 = -pvTangent[w0].x;
            pmT->_12 = -pvTangent[w0].y;
            pmT->_13 = -pvTangent[w0].z;

            pmT->_21 = -pvBinormal[w0].x;
            pmT->_22 = -pvBinormal[w0].y;
            pmT->_23 = -pvBinormal[w0].z;

            pmT->_31 = vNormal.x;
            pmT->_32 = vNormal.y;
            pmT->_33 = vNormal.z;
        }
    }

    MemFree32(pvTangent);

    return TRUE;
}
