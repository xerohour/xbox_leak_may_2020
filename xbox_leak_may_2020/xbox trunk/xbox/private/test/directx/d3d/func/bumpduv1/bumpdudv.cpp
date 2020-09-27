/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bumpdudv.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "bumpdudv.h"

//******************************************************************************
BOOL CreateSphere(float fRadius, PBVERTEX2* pprVertices, LPDWORD pdwNumVertices,
                  LPWORD* ppwIndices, LPDWORD pdwNumIndices) {

    PBVERTEX2   prVertices;
    PBVERTEX2   pr;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    UINT        uIndex = 0;
    UINT        uStepsU = 16, uStepsV = 16;
    UINT        i, j;
    float    fX, fY, fTX, fSinY, fCosY;

    if (!pprVertices || !pdwNumVertices || !ppwIndices || !pdwNumIndices) {
        return FALSE;
    }

    *pprVertices = NULL;
    *pdwNumVertices = 0;
    *ppwIndices = NULL;
    *pdwNumIndices = 0;

    dwNumVertices = (uStepsU + 1) * uStepsV;

    // Allocate memory for the vertices
    prVertices = (PBVERTEX2)MemAlloc32(dwNumVertices * sizeof(BVERTEX2));
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

        fY = (float)(j) / (float)(uStepsV - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i <= uStepsU; i++) {

            pr = &prVertices[(uStepsU + 1) * j + i];
            fX = (float)(i) / (float)(uStepsU);
            fTX = fX * M_2PI;

            pr->vPosition.x = (float)(cos(fTX)) * fSinY * fRadius;
            pr->vPosition.y = fCosY * fRadius;
            pr->vPosition.z = (float)(sin(fTX)) * fSinY * fRadius;
            pr->u0 = fX;
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
void ReleaseSphere(PBVERTEX2* ppr, LPWORD* ppw) {

    if (ppr && *ppr) {
        MemFree32(*ppr);
        *ppr = NULL;
    }
    if (ppw && *ppw) {
        MemFree32(*ppw);
        *ppw = NULL;
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

    CBumpDuDv1* pBump;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pBump = new CBumpDuDv1();
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

    HRESULT hr;

    if (!(pd3dcaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP)) {
        return FALSE;
    }

    hr = pd3d->CheckDeviceFormat(pd3dcaps->AdapterOrdinal, pd3dcaps->DeviceType, pd3ddm->Format, 0, D3DRTYPE_TEXTURE, D3DFMT_V8U8);
    if (FAILED(hr)) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
// CBumpDuDv1
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CBumpDuDv1
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
CBumpDuDv1::CBumpDuDv1() {

    m_pd3dtBase = NULL;
    m_pd3dtBump = NULL;
    m_pd3dtEnv = NULL;
    m_prSphere = NULL;
    m_pwSphere = NULL;
    m_pd3drSphere = NULL;
    m_pd3diSphere = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CBumpDuDv1
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
CBumpDuDv1::~CBumpDuDv1() {
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
BOOL CBumpDuDv1::Create(CDisplay* pDisplay) {

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
int CBumpDuDv1::Exhibit(int *pnExitCode) {

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
BOOL CBumpDuDv1::Setup() {

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
BOOL CBumpDuDv1::Initialize() {

    LPBYTE  pData;
    HRESULT hr;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    // Create a texture
//    m_pd3dt = CreateTexture(TEXT("Texture"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP, NULL, 1.0f, RGB_MAKE(0, 0, 0));
    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("earth.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE);
//m_pd3dt = CreateDiffuseMap(256, 256, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
    if (!m_pd3dtBase) {
        return FALSE;
    }

    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("earthbump.bmp"), D3DFMT_V8U8, TTYPE_BUMPMAP);
    if (!m_pd3dtBump) {
        return FALSE;
    }

    m_pd3dtEnv = (CTexture8*)CreateDiffuseMap(m_pDevice, 256, 256, RGBA_MAKE(255, 255, 255, 255), D3DFMT_A8R8G8B8, FALSE);
    if (!m_pd3dtEnv) {
        return FALSE;
    }

    CreateSphere(10.0f, &m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices);
//    m_prSphere = (PBVERTEX2)MemAlloc32(m_dwSphereVertices * sizeof(BVERTEX2));
    if (!m_prSphere) {
        return FALSE;
    }

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }

    m_pd3drSphere = CreateVertexBuffer(m_pDevice, m_prSphere, m_dwSphereVertices * sizeof(BVERTEX2), 0, FVF_BVERTEX2);
    if (!m_pd3drSphere) {
        return FALSE;
    }
/*
    hr = m_pDevice->CreateVertexBuffer(m_dwSphereVertices * sizeof(BVERTEX2), 0, FVF_BVERTEX2, D3DPOOL_DEFAULT, &m_pd3drSphere);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));
        return FALSE;
    }

    hr = m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(BVERTEX2), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    memcpy(pData, m_prSphere, m_dwSphereVertices * sizeof(BVERTEX2));

    hr = m_pd3drSphere->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }
*/
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
void CBumpDuDv1::Efface() {



    ReleaseIndexBuffer(m_pd3diSphere);
    ReleaseVertexBuffer(m_pd3drSphere);
    ReleaseSphere(&m_prSphere, &m_pwSphere);
    ReleaseTexture(m_pd3dtBase);
    ReleaseTexture(m_pd3dtBump);
    ReleaseTexture(m_pd3dtEnv);
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
void CBumpDuDv1::Update() {

    D3DXMATRIX mWorld;
    float      fTheta;
//    float  fTheta = M_PI / 25.0f;

//    fTheta = M_PI / 75.0f * m_fFrameDelta;
    fTheta = M_PI / 600.0f;

    // Update the rotation
    InitMatrix(&mWorld,
        (float)(cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)(sin(fTheta)), (float)( 0.0f), (float)(cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    D3DXMatrixMultiply(&m_mWorld, &mWorld, &m_mWorld);
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
BOOL CBumpDuDv1::Render() {

    DWORD        dwClearFlags = D3DCLEAR_TARGET;
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

    SetDiffuseMapCoords((PTCOORD2)&m_prSphere[0].u1, sizeof(BVERTEX2), 
                        &m_prSphere[0].vPosition, sizeof(BVERTEX2),
                        (PTCOORD2)&m_prSphere[0].u0, sizeof(BVERTEX2),
                        m_pwSphere, m_dwSphereIndices / 3,
                        D3DPT_TRIANGLELIST, &m_mWorld, &m_vLightDir,
                        TRUE);

    hr = m_pd3drSphere->Lock(0, m_dwSphereVertices * sizeof(BVERTEX2), &pData, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
        return FALSE;
    }

    memcpy(pData, m_prSphere, m_dwSphereVertices * sizeof(BVERTEX2));

    hr = m_pd3drSphere->Unlock();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
        return FALSE;
    }

m_pDevice->SetRenderState(D3DRS_WRAP1, D3DWRAP_U | D3DWRAP_V);
m_pDevice->SetRenderState(D3DRS_WRAP2, D3DWRAP_U | D3DWRAP_V);
    // Set textures
    m_pDevice->SetTexture(0, m_pd3dtBase);
    m_pDevice->SetTexture(1, m_pd3dtBump);
    m_pDevice->SetTexture(2, m_pd3dtEnv);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 1);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_BUMPENVMAP);
//    SetAlphaStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
//    SetAlphaStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_SELECTARG1);

    m_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
    m_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
    m_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
    m_pDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(1.0f));

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_BVERTEX2);

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(BVERTEX2));

    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);

    // Release the texture
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    m_pDevice->SetTexture(2, NULL);

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
void CBumpDuDv1::ProcessInput() {

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
BOOL CBumpDuDv1::InitView() {

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
BOOL CBumpDuDv1::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
