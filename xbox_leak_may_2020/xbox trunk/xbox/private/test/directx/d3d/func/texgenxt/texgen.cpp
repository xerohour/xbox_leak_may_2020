/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    texgen.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "texgen.h"

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

    CTexGenExt* pTexGen;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTexGen = new CTexGenExt();
    if (!pTexGen) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTexGen->Create(pDisplay)) {
        pTexGen->Release();
        return FALSE;
    }

    bRet = pTexGen->Exhibit(pnExitCode);

    // Clean up the scene
    pTexGen->Release();

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
// CTexGenExt
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTexGenExt
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
CTexGenExt::CTexGenExt() {

    m_dwPShader = INVALID_SHADER_HANDLE;
    m_pd3dtc = NULL;
    m_bFade = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CTexGenExt
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
CTexGenExt::~CTexGenExt() {
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
BOOL CTexGenExt::Prepare() {

    // TODO: Reset the device to ensure it is always using a 32 bit back buffer
    return TRUE;
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
BOOL CTexGenExt::Initialize() {

    char szShader[] = {
#ifndef UNDER_XBOX
        "ps.1.0\n"
#else
        "xps.1.1\n"
#endif
        "tex t0\n"
        "mov r0, t0\n"
    };

    m_pDevice->CreatePixelShaderAsm(szShader, strlen(szShader), &m_dwPShader);

    m_pd3dtc = CreateCubeTexture();
    if (!m_pd3dtc) {
        return FALSE;
    }

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
void CTexGenExt::Efface() {

    if (m_dwPShader != INVALID_SHADER_HANDLE) {
        ReleasePixelShader(m_pDevice, m_dwPShader);
        m_dwPShader = INVALID_SHADER_HANDLE;
    }
    ReleaseTexture(m_pd3dtc);
    m_pd3dtc = NULL;

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
void CTexGenExt::Update() {

    m_rPoint.vPosition = D3DXVECTOR3(SFRND(100.0f), SFRND(100.0f), SFRND(100.0f));
    m_cam.vPosition     = m_rPoint.vPosition + D3DXVECTOR3(SFRND(10.0f), SFRND(10.0f), SFRND(10.0f));
    m_cam.vInterest     = m_rPoint.vPosition;
    D3DXVec3Normalize(&m_rPoint.vNormal, &D3DXVECTOR3(NRND(), NRND(), NRND()));
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
BOOL CTexGenExt::Render() {

    CSurface8*      pd3ds;
    D3DLOCKED_RECT  d3dlr;
    D3DSURFACE_DESC d3dsd;
    D3DXVECTOR3     vTCoord, vColor, vE, vN;
    DWORD           dwColor;
    DWORD           dwClearFlags = D3DCLEAR_TARGET;
    D3DXMATRIX      mView;
    HRESULT         hr;
    UINT            uGen;
    DWORD           dwTexGen[] = {
                        D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR,
                        D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR,
                        D3DTSS_TCI_CAMERASPACENORMAL,
                        D3DTSS_TCI_CAMERASPACEPOSITION
    };
    LPTSTR          szTexGen[] = {
                        TEXT("D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR"),
                        TEXT("D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR"),
                        TEXT("D3DTSS_TCI_CAMERASPACENORMAL"),
                        TEXT("D3DTSS_TCI_CAMERASPACEPOSITION")
    };
    LPTSTR          szLViewer[] = {
                        TEXT("(Non-local viewer)"),
                        TEXT("(Local viewer)"),
                        TEXT(""),
                        TEXT("")
    };

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
    m_pDisplay->SetView(&m_cam);

    SetView((LPD3DXMATRIX)&mView, &m_cam.vPosition, &m_cam.vInterest, 
            &D3DXVECTOR3((float)sin(m_cam.fRoll), (float)cos(m_cam.fRoll), 
            0.0f));

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_CVERTEX);

    m_pDevice->SetPixelShader(m_dwPShader);

    uGen = (UINT)m_fFrame % 4;

    hr = m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, dwTexGen[uGen]);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetTextureStageState"))) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_LOCALVIEWER, uGen ? TRUE : FALSE);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->SetTexture(0, m_pd3dtc);

    m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &m_rPoint, sizeof(CVERTEX));

    m_pDevice->SetTexture(0, NULL);

    m_pDevice->SetPixelShader(NULL);

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

    m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);

    pd3ds->GetDesc(&d3dsd);

    pd3ds->LockRect(&d3dlr, NULL, 0);

    if (FormatToBitDepth(d3dsd.Format) != 32) {
        return FALSE;
    }

    dwColor = *((LPDWORD)((LPBYTE)d3dlr.pBits + d3dlr.Pitch * (d3dsd.Height / 2)) + (d3dsd.Width / 2));
    vColor = D3DXVECTOR3(((float)RGBA_GETRED(dwColor) - 128.0f) / 127.0f, ((float)RGBA_GETGREEN(dwColor) - 128.0f) / 127.0f, ((float)RGBA_GETBLUE(dwColor) - 128.0f) / 127.0f);

    switch (dwTexGen[uGen]) {
        case D3DTSS_TCI_CAMERASPACENORMAL:
            D3DXVec3TransformNormal(&vTCoord, &m_rPoint.vNormal, &mView);
            break;
        case D3DTSS_TCI_CAMERASPACEPOSITION:
            vTCoord = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
            break;
        case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR:
            if (!uGen) {
                D3DXVec3TransformNormal(&vN, &m_rPoint.vNormal, &mView);
                vTCoord = 2.0f * m_rPoint.vNormal.z * vN - D3DXVECTOR3(0.0f, 0.0f, 1.0f);
                D3DXVec3Normalize(&vTCoord, &vTCoord);
            }
            else {
                D3DXVec3Normalize(&vE, &(m_cam.vPosition - m_rPoint.vPosition));
                D3DXVec3TransformNormal(&vN, &m_rPoint.vNormal, &mView);
                D3DXVec3TransformNormal(&vE, &vE, &mView);
                vTCoord = 2.0f * D3DXVec3Dot(&vE, &vN) * vN - vE;
                D3DXVec3Normalize(&vTCoord, &vTCoord);
            }
            break;
    }

    if (FABS(vColor.x - vTCoord.x) > 0.1f || FABS(vColor.y - vTCoord.y) > 0.1f || FABS(vColor.z - vTCoord.z) > 0.1f) {
        Log(LOG_FAIL, TEXT("%s %s generated incorrect texture coordinates - generated: %3.3f, %3.3f, %3.3f, expected: %3.3f, %3.3f, %3.3f"), szTexGen[uGen], szLViewer[uGen], vColor.x, vColor.y, vColor.z, vTCoord.x, vTCoord.y, vTCoord.z);
    }

    pd3ds->UnlockRect();

    pd3ds->Release();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    return TRUE;
}

//******************************************************************************
CCubeTexture8* CTexGenExt::CreateCubeTexture() {

    CCubeTexture8*  pd3dtc;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    D3DXVECTOR3     vDir;
    float           fU, fV;
    UINT            j, u, v;
    HRESULT         hr;
#ifdef UNDER_XBOX
    Swizzler        swz(512, 512, 1);
#endif

    // Currently only support 32 bpp formats
    pd3dtc = (CCubeTexture8*)CreateTexture(m_pDevice, 512, 512, D3DFMT_X8R8G8B8, TTYPE_CUBEMAP);
    if (!pd3dtc) {
        return NULL;
    }

    // Generate the normals
    for (j = 0; j < 6; j++) {

        hr = pd3dtc->LockRect((D3DCUBEMAP_FACES)j, 0, &d3dlr, NULL, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
            ReleaseTexture(pd3dtc);
            return NULL;
        }

        pdwPixel = (LPDWORD)d3dlr.pBits;

#ifdef UNDER_XBOX
        Swizzler swz(512, 512, 1);
        swz.SetU(0);
        swz.SetV(0);
#endif

        for (v = 0; v < 512; v++) {

            for (u = 0; u < 512; u++) {

                fU = (float)u / 511.0f;
                fV = (float)v / 511.0f;

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
				D3DXVec3Normalize(&vDir, &vDir);

#ifndef UNDER_XBOX
                pdwPixel[u] = VectorToColor(&vDir);
#else
                pdwPixel[swz.Get2D()] = VectorToColor(&vDir);
                swz.IncU();
#endif
            }

#ifndef UNDER_XBOX
            pdwPixel += (d3dlr.Pitch >> 2);
#else
            swz.IncV();
#endif
        }

        hr = pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
            ReleaseTexture(pd3dtc);
            return NULL;
        }
    }

    return pd3dtc;
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
void CTexGenExt::ProcessInput() {

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
BOOL CTexGenExt::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_cam.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
    m_cam.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_cam.fRoll         = 0.0f;
    m_cam.fFieldOfView  = M_PI / 4.0f;
    m_cam.fNearPlane    = 0.1f;
    m_cam.fFarPlane     = 1000.0f;
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
BOOL CTexGenExt::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
