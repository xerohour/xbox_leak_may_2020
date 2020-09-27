/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mirror.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "mirror.h"

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

    CMirror* pMirror;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMirror = new CMirror();
    if (!pMirror) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMirror->Create(pDisplay)) {
        pMirror->Release();
        return FALSE;
    }

    bRet = pMirror->Exhibit(pnExitCode);

    // Clean up the scene
    pMirror->Release();

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
// CMirror
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMirror
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
CMirror::CMirror() {

    m_pd3dsZMirror = NULL;
    m_pd3dsMirror = NULL;
    m_pd3dsBack = NULL;
    m_pd3dsDepth = NULL;
    m_bShade = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CMirror
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
CMirror::~CMirror() {
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
BOOL CMirror::Create(CDisplay* pDisplay) {

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
int CMirror::Exhibit(int *pnExitCode) {

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
BOOL CMirror::Prepare() {

    // Initialize the vertices
    m_prFloor[0] = VERTEX(D3DXVECTOR3(-5.0f,-10.0f,-5.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f);
    m_prFloor[1] = VERTEX(D3DXVECTOR3(-5.0f,-10.0f, 5.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
    m_prFloor[2] = VERTEX(D3DXVECTOR3( 5.0f,-10.0f, 5.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f);
    m_prFloor[3] = VERTEX(D3DXVECTOR3( 5.0f,-10.0f,-5.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);

    m_prMirror[0].vPosition = D3DXVECTOR3(-2.0f, -10.0f, 0.0f);
    m_prMirror[0].u0 = m_prMirror[0].u1 = 0.0f;
    m_prMirror[0].v0 = m_prMirror[0].v1 = 1.0f;

    m_prMirror[1].vPosition = D3DXVECTOR3(-2.0f, -6.0f, 0.0f);
    m_prMirror[1].u0 = m_prMirror[1].u1 = 0.0f;
    m_prMirror[1].v0 = m_prMirror[1].v1 = 0.0f;

    m_prMirror[2].vPosition = D3DXVECTOR3( 2.0f, -6.0f, 0.0f);
    m_prMirror[2].u0 = m_prMirror[2].u1 = 1.0f;
    m_prMirror[2].v0 = m_prMirror[2].v1 = 0.0f;

    m_prMirror[3].vPosition = D3DXVECTOR3( 2.0f, -10.0f, 0.0f);
    m_prMirror[3].u0 = m_prMirror[3].u1 = 1.0f;
    m_prMirror[3].v0 = m_prMirror[3].v1 = 1.0f;

    m_mWorld = m_mIdentity;

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
BOOL CMirror::Setup() {

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
BOOL CMirror::Initialize() {

    D3DSURFACE_DESC d3dsd;
    HRESULT         hr;

    // Create the floor texture
    m_pd3dtFloor = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"));
    if (!m_pd3dtFloor) {
        return FALSE;
    }

    // Create the shade texture
    m_pd3dtShade = CreateAlphaTexture(m_pDevice, TEXT("shade.bmp"), D3DFMT_A8R8G8B8);
    if (!m_pd3dtShade) {
        return FALSE;
    }

    // Create the mirror texture
    m_pd3dtMirror = (CTexture8*)CreateTexture(m_pDevice, 512, 512, D3DFMT_X8R8G8B8, TTYPE_TARGET);
    if (!m_pd3dtMirror) {
        return FALSE;
    }
#ifdef UNDER_XBOX
    m_pd3dtMirror->GetIDirect3DTexture8()->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

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

    hr = m_pDevice->CreateDepthStencilSurface(512, 512, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &m_pd3dsZMirror);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateDepthStencilSurface"))) {
        return FALSE;
    }
#ifdef UNDER_XBOX
    m_pd3dsMirror->GetIDirect3DSurface8()->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

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
void CMirror::Efface() {

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
void CMirror::Update() {

    D3DXMATRIX mRotate;
    float      fTheta;

    fTheta = 0.02f * m_fTimeDilation;

    InitMatrix(&mRotate,
        (float)(cos(fTheta)), (float)( 0.0f), (float)(-sin(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)(sin(fTheta)), (float)( 0.0f), (float)(cos(fTheta)), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    D3DXMatrixMultiply(&m_mWorld, &mRotate, &m_mWorld);
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
BOOL CMirror::Render() {

    CSurface8*  pd3dsBack;
    CSurface8*  pd3dsDepth;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    CAMERA      cam;
    UINT        uPass;
    D3DRECT     d3drect;

    if (!m_bCreated) {
        return FALSE;
    }

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
    m_pDisplay->GetCamera(&cam);
    ::SetView(&m_mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));

    SetPerspectiveProjection(&m_mProj, cam.fNearPlane, cam.fFarPlane, 
            cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

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
            if (!SetReflectedView(&cam, m_prMirror, &m_mWorld)) {
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
//            SetPerspectiveProjection(&mProj, g_cam.fNearPlane, g_cam.fFarPlane, 
//                    g_cam.fFieldOfView, D3DVAL(d3drect.y2) / D3DVAL(d3drect.x2));

            m_pDevice->SetTransform(D3DTS_PROJECTION, &m_mProj);
        }

        // Clear the rendering target
        if (m_pDisplay->IsDepthBuffered()) {
            dwClearFlags |= D3DCLEAR_ZBUFFER;
        }
        m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        // Set the floor texture
        m_pDevice->SetTexture(0, m_pd3dtFloor);

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);

        // Draw the floor
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
                    m_prFloor, sizeof(VERTEX));

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

        // Render the mirror
        if (!uPass) {

            // Set the texture coordinates of the mirror in such a way as to
            // clip the scene rendered in the first pass to the mirror's boundaries
            hr = SetReflectionTextureCoords(m_prMirror, 4);
            if (FAILED(hr)) {
                return FALSE;
            }
        }

        else {

            // Disable perspective correct texture mapping since the perspective
            // of the mirror's reflection was already set on the first rendering pass
//            m_pDevice->SetRenderState(D3DRS_TEXTUREPERSPECTIVE, (DWORD)FALSE);

            // Set the mirror's textures
            m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

            m_pDevice->SetTexture(0, m_pd3dtMirror);
            m_pDevice->SetTexture(1, m_pd3dtShade);

            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
//SetAlphaStage(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
            SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
            if (m_bShade) {
                SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
                SetAlphaStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
            }

            m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

            m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);

            m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

            m_pDevice->SetVertexShader(FVF_MVERTEX);

            // Draw the mirror
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
                    m_prMirror, sizeof(MVERTEX));

            m_pDevice->SetVertexShader(FVF_VERTEX);

            m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

            m_pDevice->SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_CCW);

            m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
            SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            m_pDevice->SetTexture(1, NULL);

            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

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
void CMirror::ProcessInput() {

    CScene::ProcessInput();

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            m_bShade = !m_bShade;
        }
    }
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
BOOL CMirror::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -15.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, -6.0f, -5.0f);
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
BOOL CMirror::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

        case WM_CHAR:

            switch (wParam) {

                case TEXT('Z'):
                case TEXT('z'):
                    m_bShade = !m_bShade;
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
BOOL CMirror::ReplaceRenderTarget(CSurface8* pd3ds, CSurface8* pd3dsZ, CSurface8** ppd3dsOld, CSurface8** ppd3dsZOld, D3DRECT* pd3drect) {

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
BOOL CMirror::SetReflectedView(PCAMERA pcam, PMVERTEX prVertices, D3DXMATRIX* pmWorld) {

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
        D3DXVec3TransformCoord(&pvPlane[i], &(prVertices[i].vPosition), pmWorld);
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
HRESULT CMirror::SetReflectionTextureCoords(PMVERTEX prVertices, UINT uNumVertices) {

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
    D3DXMatrixMultiply(&r1, &m_mWorld, &r2);

    // Transform the reflective surface vertices into screen coordinates.
    // Use the screen coordinates to set texture coordinates for the surface.
    for (i = 0; i < uNumVertices; i++) {
        pv = &prVertices[i].vPosition;
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

