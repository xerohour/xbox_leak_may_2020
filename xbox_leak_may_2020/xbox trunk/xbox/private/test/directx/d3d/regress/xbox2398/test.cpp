/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "test.h"

//******************************************************************************
CCubeTexture8*  CreateNormalTexture(CDevice8* pDevice, UINT uLength = 256, 
                                float fBumpDelta = 0.0f,
                                D3DFORMAT fmt = D3DFMT_A8R8G8B8, 
                                BOOL bMipmap = TRUE, 
                                D3DPOOL pool = POOL_DEFAULT);

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

    CTest*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CTest();
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

    return TRUE;
}

//******************************************************************************
// CTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTest
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
CTest::CTest() {
}

//******************************************************************************
//
// Method:
//
//     ~CTest
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
CTest::~CTest() {
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
BOOL CTest::Create(CDisplay* pDisplay) {

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
int CTest::Exhibit(int *pnExitCode) {

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
BOOL CTest::Prepare() {

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
BOOL CTest::Setup() {

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
BOOL CTest::Initialize() {

    D3DPIXELSHADERDEF d3dpsd;
    UINT i;
    char szShader[] = {
            "ps.1.0\n"
            "tex t0\n"
            "texm3x3pad t1, t0_sgn\n"
            "texm3x3pad t2, t0_sgn\n"
            "texm3x3tex t3, t0_sgn\n"
            "mov r0, t3\n"
         };

    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));

    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_CUBEMAP, PS_TEXTUREMODES_DOTPRODUCT, PS_TEXTUREMODES_DOTPRODUCT, PS_TEXTUREMODES_DOT_STR_3D);
    d3dpsd.PSDotMapping = PS_DOTMAPPING(PS_DOTMAPPING_MINUS1_TO_1_D3D, PS_DOTMAPPING_MINUS1_TO_1_D3D, PS_DOTMAPPING_MINUS1_TO_1_D3D, PS_DOTMAPPING_MINUS1_TO_1_D3D);
    d3dpsd.PSInputTexture = PS_INPUTTEXTURE(0, 0, 0, 0);
    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(1, PS_COMBINERCOUNT_SAME_C0 | PS_COMBINERCOUNT_MUX_LSB | PS_COMBINERCOUNT_UNIQUE_C1);
    d3dpsd.PSRGBInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_T3, PS_REGISTER_ONE, PS_REGISTER_ZERO, PS_REGISTER_ZERO);
    d3dpsd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, PS_COMBINEROUTPUT_IDENTITY);
    d3dpsd.PSAlphaInputs[0] = PS_COMBINERINPUTS(PS_REGISTER_T3 | PS_CHANNEL_ALPHA, PS_REGISTER_ONE, PS_REGISTER_ZERO, PS_REGISTER_ZERO);
    d3dpsd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, PS_COMBINEROUTPUT_IDENTITY);
    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(PS_REGISTER_ZERO, PS_REGISTER_ZERO, PS_REGISTER_ZERO, PS_REGISTER_R0);
    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(PS_REGISTER_ZERO, PS_REGISTER_ZERO, PS_REGISTER_T3, 0);

    m_prQuad[0] = VERTEXDP3(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), -1.0f, -1.0f, -0.5f);
    m_prQuad[1] = VERTEXDP3(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), -1.0f, 1.0f, -0.5f);
    m_prQuad[2] = VERTEXDP3(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f, -0.5f);
    m_prQuad[3] = VERTEXDP3(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, -1.0f, -0.5f);

    m_prQuad[0].mTransform._11 = (float)cos(M_3PIDIV2);
    m_prQuad[0].mTransform._21 = 0.0f;
    m_prQuad[0].mTransform._31 = -(float)sin(M_3PIDIV2);
    m_prQuad[0].mTransform._12 = 0.0f;
    m_prQuad[0].mTransform._22 = 1.0f;
    m_prQuad[0].mTransform._32 = 0.0f;
    m_prQuad[0].mTransform._13 = (float)sin(M_3PIDIV2);
    m_prQuad[0].mTransform._23 = 0.0f;
    m_prQuad[0].mTransform._33 = (float)cos(M_3PIDIV2);
    memcpy(&m_prQuad[1].mTransform, &m_prQuad[0].mTransform, sizeof(MATRIX3X3));
    memcpy(&m_prQuad[2].mTransform, &m_prQuad[0].mTransform, sizeof(MATRIX3X3));
    memcpy(&m_prQuad[3].mTransform, &m_prQuad[0].mTransform, sizeof(MATRIX3X3));

    m_pd3dt1 = CreateNormalTexture(m_pDevice, 256);
    m_pd3dt2 = (CCubeTexture8*)CreateTexture(m_pDevice, TEXT("face0.bmp"), D3DFMT_A8R8G8B8, TTYPE_CUBEMAP);

//    m_pDevice->CreatePixelShaderAsm(szShader, strlen(szShader), &m_dwPShader);
    m_pDevice->CreatePixelShader(&d3dpsd, &m_dwPShader);

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
void CTest::Efface() {

    ReleaseTexture(m_pd3dt1);
    ReleaseTexture(m_pd3dt2);
    ReleaseVertexShader(m_pDevice, m_dwPShader);
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
void CTest::Update() {

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
BOOL CTest::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

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

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEXDP3);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    m_pDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
    m_pDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
    m_pDevice->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);

    m_pDevice->SetTexture(0, m_pd3dt1);
    m_pDevice->SetTexture(3, m_pd3dt2);

    m_pDevice->SetPixelShader(m_dwPShader);

    m_pDevice->SetPixelShaderConstant(0, &D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), 1);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
            m_prQuad, sizeof(VERTEXDP3));

    m_pDevice->SetPixelShader(NULL);

    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);

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
void CTest::ProcessInput() {

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
BOOL CTest::InitView() {

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
BOOL CTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
CCubeTexture8* CreateNormalTexture(CDevice8* pDevice, UINT uLength, float fBumpDelta, 
                                    D3DFORMAT fmt, BOOL bMipmap, D3DPOOL pool)
{
    CCubeTexture8*  pd3dtc;
    CCubeTexture8*  pd3dtcBump = NULL;
    TEXTURETYPE     ttype;
    UINT            uLevels;
    D3DLOCKED_RECT  d3dlr, d3dlrB;
    LPDWORD         pdwPixel, pdwPixelB;
    D3DXVECTOR3     vDir;
    float           fU, fV;
    UINT            i, j, u, v;
    HRESULT         hr;

    // Currently only support 32 bpp formats
    if (!(fmt == D3DFMT_A8R8G8B8 || fmt == D3DFMT_X8R8G8B8)) {
        return NULL;
    }

    ttype = TTYPE_CUBEMAP;
    if (bMipmap) {
        ttype |= TTYPE_MIPMAP;
    }

    pd3dtc = (CCubeTexture8*)CreateTexture(pDevice, uLength, uLength, fmt, ttype, pool);
    if (!pd3dtc) {
        return NULL;
    }

    // If the normal map has multiple levels and a delta is to be applied to the normals,
    // generate the random offset values to be applied and store them in the highest
    // level of a second cube map.  Filter the offsets to the lower levels for consistent
    // transitions.
    if (bMipmap && fBumpDelta != 0.0f) {
        pd3dtcBump = (CCubeTexture8*)CreateTexture(pDevice, uLength, uLength, D3DFMT_A8R8G8B8, ttype);
        if (!pd3dtcBump) {
            ReleaseTexture(pd3dtc);
            return NULL;
        }

        // Populate the cube texture with random offsets to be applied to the normal map
        for (i = 0; i < 6; i++) {
            hr = pd3dtcBump->LockRect((D3DCUBEMAP_FACES)i, 0, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

				    vDir = D3DXVECTOR3(SFRND(fBumpDelta), SFRND(fBumpDelta), SFRND(fBumpDelta));
                    pdwPixel[u] = VectorToColor(&vDir);
                }

                pdwPixel += (d3dlr.Pitch >> 2);
            }

            hr = pd3dtcBump->UnlockRect((D3DCUBEMAP_FACES)i, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }
        }

        // Filter the offsets to the lower levels
        hr = D3DXFilterCubeTexture(pd3dtcBump->GetIDirect3DCubeTexture8(), NULL, 0, D3DX_FILTER_BOX);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("D3DXFilterCubeTexture"));
            ReleaseTexture(pd3dtcBump);
            ReleaseTexture(pd3dtc);
            return NULL;
        }
    }

    // Generate the normals
    uLevels = pd3dtc->GetLevelCount();

    for (i = 0; i < uLevels; i++) {

        for (j = 0; j < 6; j++) {

            hr = pd3dtc->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }

            if (pd3dtcBump) {
                hr = pd3dtcBump->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlrB, NULL, 0);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                    ReleaseTexture(pd3dtcBump);
                    ReleaseTexture(pd3dtc);
                    return NULL;
                }
                pdwPixelB = (LPDWORD)d3dlrB.pBits;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            if (uLevels > 1) {
                uLength = 0x1 << ((uLevels - 1) - i);
            }

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

                    if (uLength > 1) {
                        fU = (float)u / (float)(uLength - 1);
                        fV = (float)v / (float)(uLength - 1);
                    }
                    else {
                        fU = 0.5f;
                        fV = 0.5f;
                    }

                    switch ((D3DCUBEMAP_FACES)j) {
                        case D3DCUBEMAP_FACE_POSITIVE_X:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(1.0f, 1.0f - (fV * 2.0f), 1.0f - (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_X:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f, 1.0f - (fV * 2.0f), -1.0f + (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Y:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f, -1.0f + (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Y:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), -1.0f, 1.0f - (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Z:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f - (fV * 2.0f), 1.0f));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Z:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(1.0f - (fU * 2.0f), 1.0f - (fV * 2.0f), -1.0f));
                            break;
                    }
//                    vDir = -vDir;
                    if (!pd3dtcBump) {
    				    vDir += D3DXVECTOR3(SFRND(fBumpDelta), SFRND(fBumpDelta), SFRND(fBumpDelta));
                    }
                    else {
                        vDir += ColorToVector(pdwPixelB[u]);
                    }
				    D3DXVec3Normalize(&vDir, &vDir);

                    pdwPixel[u] = VectorToColor(&vDir);
                }

                pdwPixel += (d3dlr.Pitch >> 2);
                if (pd3dtcBump) {
                    pdwPixelB += (d3dlrB.Pitch >> 2);
                }
            }

            if (pd3dtcBump) {
                hr = pd3dtcBump->UnlockRect((D3DCUBEMAP_FACES)j, i);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                    if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                    ReleaseTexture(pd3dtc);
                    return NULL;
                }
            }

            hr = pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }
        }
    }

    if (pd3dtcBump) {
        ReleaseTexture(pd3dtcBump);
    }

    return pd3dtc;
}
