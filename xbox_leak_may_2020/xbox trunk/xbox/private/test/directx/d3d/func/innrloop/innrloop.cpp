/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    innrloop.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include <profilep.h>
#endif // UNDER_XBOX
#include "innrloop.h"

//******************************************************************************
// Globals
//******************************************************************************

DWORD       g_dwSkipList[] = {
                    274496,
            };
UINT        g_uSkipSize = 0;//sizeof(g_dwSkipList) / sizeof(g_dwSkipList[0]);

//******************************************************************************
// Function prototypes
//******************************************************************************

HRESULT     DrawSpheres(CDevice8* pDevice, CSphereForms* psf, BOOL bPrelit, DWORD dwSphere);
void        OffsetLoops(int* pnParam, int nOffset);
void        OutputLoops(int* pnParam);
DWORD       CondenseState(int* pnParam);

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

    CInnerLoop*     pILoop;
    BOOL            bRet;
    TCHAR           szName[32];
    TCHAR           szCmdLine[128];
    DWORD           dwInitialState = 0;
    DWORD           dwSphere = 0;
    TCHAR           szBuffer[64];
    TCHAR*          sz;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    if (GetModuleName(GetTestInstance(), szName, 32)) {

        GetProfileString(szName, TEXT("CommandLine"), TEXT(""), szCmdLine, sizeof(szCmdLine));

        if (*szCmdLine) {

            for (sz = szCmdLine; *sz && *sz == TEXT(' '); sz++);
            for (i = 0; *sz && *sz != TEXT(' '); sz++, i++) {
                szBuffer[i] = *sz;
            }
            szBuffer[i] = TEXT('\0');
            for (; *sz && *sz == TEXT(' '); sz++);
            dwInitialState = _ttoi(szBuffer);
            if (*sz) {
                for (i = 0; *sz && *sz != TEXT(' '); sz++, i++) {
                    szBuffer[i] = *sz;
                }
                szBuffer[i] = TEXT('\0');
                dwSphere = _ttoi(szBuffer);
            }
        }
    }

    // Create the scene
    pILoop = new CInnerLoop();
    if (!pILoop) {
        return FALSE;
    }

    // Initialize the scene
    if (!pILoop->Create(pDisplay, dwInitialState, dwSphere)) {
        pILoop->Release();
        return FALSE;
    }

    bRet = pILoop->Exhibit(pnExitCode);

    // Clean up the scene
    pILoop->Release();

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
// CInnerLoop
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CInnerLoop
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
CInnerLoop::CInnerLoop() {

    m_nOffset = 0;
    m_bWait = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CInnerLoop
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
CInnerLoop::~CInnerLoop() {
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
BOOL CInnerLoop::Create(CDisplay* pDisplay, DWORD dwInitialState, DWORD dwSphere) {

    UINT i;

    m_dwInitialState = dwInitialState;
    m_dwSphere = dwSphere;

    for (i = 0; i <= MAX_PARAM; i++) {
        m_nInitial[i] = GETSTATE(dwInitialState, i);
    }

    m_nParam[FILLMODE] = m_nInitial[FILLMODE];
    m_nParam[CLIPPING] = m_nInitial[CLIPPING];
    m_nParam[TEXTURING] = m_nInitial[TEXTURING];
    m_nParam[WRAPPING] = m_nInitial[WRAPPING];
    m_nParam[LIGHTING] = m_nInitial[LIGHTING];
    m_nParam[VERTEXFOG] = m_nInitial[VERTEXFOG];
    m_nParam[SPECULAR] = m_nInitial[SPECULAR];

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
int CInnerLoop::Exhibit(int *pnExitCode) {

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
BOOL CInnerLoop::Prepare() {

    // Initialize the spheres
    if (!m_sf.Create(256)) {
        return FALSE;
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
BOOL CInnerLoop::Setup() {

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
BOOL CInnerLoop::Initialize() {

    UINT i;

    // Initialize the fog
    if (!SetPixelFog(m_pDevice, RGB_MAKE(0, 0, 255), 15.0f, 25.0f, 1.0f)) {
        return FALSE;
    }

    // Create textures
    m_pd3dt[0] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture0.bmp"));
    m_pd3dt[1] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture1.bmp"));
    m_pd3dt[2] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture2.bmp"));
    m_pd3dt[3] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture3.bmp"));
    for (i = 0; i < 4; i++) {
        if (!m_pd3dt[i]) {
            return FALSE;
        }
    }
    for (i = 0; i < 4; i++) {
        m_pDevice->SetTexture(i, m_pd3dt[i]);
	    m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
    }
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    SetDirectionalLight(m_pDevice, 0, LIGHT_DIR);
    SetPointLight(m_pDevice, 1, LIGHT_POS, RGB_MAKE(255, 0, 0));
    SetSpotLight(m_pDevice, 2, LIGHT_POS, LIGHT_DIR, RGB_MAKE(0, 255, 0), 10.0f, 0.5f);
    SetSpotLight(m_pDevice, 3, LIGHT_POS, LIGHT_DIR, RGB_MAKE(255, 0, 0));

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
void CInnerLoop::Efface() {

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
void CInnerLoop::Update() {

    if (m_bWait) {
        OffsetLoops(m_nParam, -1 + m_nOffset);
    }
    else {
        OffsetLoops(m_nParam, m_nOffset);
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
BOOL CInnerLoop::Render() {

    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    DWORD       dwState;
    UINT        i;
    D3DMATRIX   mProj;
    CAMERA      cam;
    D3DFILLMODE d3dfm[3] = {D3DFILL_SOLID, D3DFILL_WIREFRAME,
                                            D3DFILL_POINT};

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

    m_pDisplay->GetCamera(&cam);

    do {

        if (m_nParam[SPECULAR] >= 2) {
            m_nParam[SPECULAR] = m_nInitial[SPECULAR];
            m_nParam[VERTEXFOG]++;
        }

        if (m_nParam[VERTEXFOG] >= 2) {
            m_nParam[VERTEXFOG] = m_nInitial[VERTEXFOG];
            m_nParam[LIGHTING]++;
        }

        if (m_nParam[LIGHTING] >= 4) {
            m_nParam[LIGHTING] = m_nInitial[LIGHTING];
            m_nParam[WRAPPING]++;
        }

        if (m_nParam[WRAPPING] >= 2) {
            m_nParam[WRAPPING] = m_nInitial[WRAPPING];
            m_nParam[TEXTURING]++;
        }

        if (m_nParam[TEXTURING] >= 2) {
            m_nParam[TEXTURING] = m_nInitial[TEXTURING];
            m_nParam[CLIPPING]++;
        }

        if (m_nParam[CLIPPING] >= 2) {
            m_nParam[CLIPPING] = m_nInitial[CLIPPING];
            m_nParam[FILLMODE]++;
        }

        if (m_nParam[FILLMODE] >= 3) {
            if (GetStartupContext() & TSTART_HARNESS) {
                return FALSE;
            }
            m_nParam[FILLMODE] = m_nInitial[FILLMODE];
        }

        if (g_uSkipSize && !m_dwInitialState) {

            dwState = CondenseState(m_nParam);
            for (i = 0; i < (int)g_uSkipSize; i++) {
                if (dwState == g_dwSkipList[i]) {
                    break;
                }
            }
            if (i < (int)g_uSkipSize) {
                m_nParam[SPECULAR]++;
            }
        }

    } while (g_uSkipSize && !m_dwInitialState && i < (int)g_uSkipSize);


    for (i = 0; i <= MAX_PARAM; i++) {
        m_nInitial[i] = 0;
    }

    // Fill mode
    m_pDevice->SetRenderState(D3DRS_FILLMODE, d3dfm[m_nParam[FILLMODE]]);

    // Front clipping
    SetPerspectiveProjection(&mProj, m_nParam[CLIPPING] ? 19.25f : cam.fNearPlane, 
            cam.fFarPlane, cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    m_pDevice->SetTransform(D3DTS_PROJECTION, &mProj);

    // Set texturing
    switch (m_nParam[TEXTURING]) {
        case 0:
            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
            m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            break;
        case 1:
            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
            m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            break;
        case 2:
            SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
            SetColorStage(m_pDevice, 1, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
            SetColorStage(m_pDevice, 2, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
            SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
            m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            break;
    }

    // Set wrapping
    for (i = 0; i < 4; i++) {
        m_pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_WRAP0 + i), 
                                          (DWORD)(m_nParam[WRAPPING] ? (D3DWRAP_U | D3DWRAP_V) : 0));
    }

    // Enable/disable lights
    m_pDevice->LightEnable(0, m_nParam[LIGHTING] > 1);
    m_pDevice->LightEnable(1, m_nParam[LIGHTING] > 2);
    m_pDevice->LightEnable(2, m_nParam[LIGHTING] > 2);
    m_pDevice->LightEnable(3, m_nParam[LIGHTING] > 2);

    // Enable/disable fog
    m_pDevice->SetRenderState(D3DRS_FOGENABLE, m_nParam[VERTEXFOG] == 1);

    // Set the specular
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (BOOL)m_nParam[SPECULAR]);

//OutputLoops(nParam);

    m_nParam[SPECULAR]++;

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Draw the spheres
    DrawSpheres(m_pDevice, &m_sf, m_nParam[LIGHTING] == 0, m_dwSphere);

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
void CInnerLoop::ProcessInput() {

    KEYSDOWN    kdKeys;
    static UINT uFMDisplay = 0;

    if (GetStartupContext() & TSTART_HARNESS) {
        return;
    }

    kdKeys = m_pDisplay->GetKeyState();
    m_pDisplay->GetJoyState(&m_jsJoys, &m_jsLast);

    if (kdKeys & KEY_LEFT) {
        m_nOffset = -1;
        m_bWait = TRUE;
    }
    else if (kdKeys & KEY_RIGHT) {
        m_nOffset = 1;
        m_bWait = TRUE;
    }
    else {
        m_nOffset = 0;
    }

    if (kdKeys & KEY_UP) {
        m_bWait = !m_bWait;
    }
    else if (kdKeys & KEY_DOWN) {
        m_bWait = !m_bWait;
    }

#ifdef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            m_bQuit = TRUE;
        }
    }
#endif // UNDER_XBOX

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_bWait = !m_bWait;
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_Y)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {
            OutputLoops(m_nParam);
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            m_nOffset = 1;
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RSTICK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RSTICK) && !BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
            switch (uFMDisplay++) {
                case 0:
                    m_bShowFPS = TRUE;
                    m_bShowMode = TRUE;
                    break;
                case 1:
                    m_bShowFPS = TRUE;
                    m_bShowMode = FALSE;
                    break;
                case 2:
                    m_bShowFPS = FALSE;
                    m_bShowMode = TRUE;
                    break;
                case 3:
                    m_bShowFPS = FALSE;
                    m_bShowMode = FALSE;
                    break;
            }
            if (uFMDisplay > 3) {
                uFMDisplay = 0;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LSTICK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LSTICK)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                DWORD dwZFunc;
                if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_ZFUNC, (LPDWORD)&dwZFunc))) {
                    if (dwZFunc == D3DCMP_LESSEQUAL) {
                        dwZFunc = D3DCMP_ALWAYS;
                    }
                    else {
                        dwZFunc = D3DCMP_LESSEQUAL;
                    }
                    m_pDevice->SetRenderState(D3DRS_ZFUNC, dwZFunc);
                }
            }
            else {
                DWORD dwFillMode;
                if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_FILLMODE, (LPDWORD)&dwFillMode))) {
                    if (dwFillMode == D3DFILL_SOLID) {
                        dwFillMode = D3DFILL_WIREFRAME;
                    }
                    else {
                        dwFillMode = D3DFILL_SOLID;
                    }
                    m_pDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
                }
            }
        }
    }

    // Left keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_LEFT)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_LEFT)) {
            m_nOffset = -1;
        }
        else {
            m_nOffset = 0;
        }
        m_bWait = TRUE;
    }

    // Right keypad
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_RIGHT)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_RIGHT)) {
            m_nOffset = 1;
        }
        else {
            m_nOffset = 0;
        }
        m_bWait = TRUE;
    }

    if (m_jsJoys.f2X != m_jsLast.f2X) {
        m_nOffset = (int)(m_jsJoys.f2X * 20.0f);
        m_bWait = TRUE;
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
BOOL CInnerLoop::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -20.0f);
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
BOOL CInnerLoop::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
HRESULT DrawSpheres(CDevice8* pDevice, CSphereForms* psf, BOOL bPrelit, DWORD dwSphere) {

    D3DMATRIX        mWorld;
    D3DPRIMITIVETYPE d3dpt[6] = {D3DPT_POINTLIST, D3DPT_LINELIST, 
                                 D3DPT_LINESTRIP, D3DPT_TRIANGLELIST, 
                                 D3DPT_TRIANGLESTRIP, D3DPT_TRIANGLEFAN};
    DWORD            dwType = bPrelit ? FVF_VT_LVERTEX : FVF_VT_VERTEX;
    HRESULT          hr;
    UINT             i, j, k;

    InitMatrix(&mWorld,
        (float)( 1.0f), (float)( 0.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 1.0f), (float)( 0.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 1.0f), (float)( 0.0f),
        (float)( 0.0f), (float)( 0.0f), (float)( 0.0f), (float)( 1.0f)
    );

    if (!dwSphere) {
        for (i = 0, mWorld._42 = 5.0f; i < 6; i++, mWorld._42 -= 2.0f) {

            for (j = 0, mWorld._41 = -5.0f; j < 3; j++, mWorld._41 += 2.0f) {

                for (k = 0; k < 2; k++) {

                    mWorld._41 += (2.0f * k);
                    hr = pDevice->SetTransform(D3DTS_WORLD, &mWorld);
                    if (FAILED(hr)) return hr;

                    hr = psf->Render(pDevice, d3dpt[i], (j > 1) ? FVF_VT_TLVERTEX : dwType, 
                                     (k == 1), (j == 1), 0);
                }
            }
        }
    }
    else {
        mWorld._42 = 5.0f - 2.0f * ((dwSphere - 1) / 6);
        mWorld._41 = -5.0f + 2.0f * ((dwSphere - 1) % 6);

        hr = pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        if (FAILED(hr)) return hr;

        hr = psf->Render(pDevice, d3dpt[(dwSphere - 1) / 6], ((dwSphere - 1) % 6 > 3) ? FVF_VT_TLVERTEX : dwType, 
                         (((dwSphere - 1) % 6) % 2), ((dwSphere - 1) % 6 > 1) && ((dwSphere - 1) % 6 < 4), 0);
    }

    return hr;
}

//******************************************************************************
// CSphereForms methods
//******************************************************************************

//******************************************************************************
CSphereForms::CSphereForms() {

    m_prGeneral = NULL;
    m_prLineList = NULL;
    m_prTriList = NULL;
    m_prTriStrip = NULL;
    m_pwGeneral = NULL;
    m_pwLineList = NULL;
    m_pwTriList = NULL;
    m_pwTriStrip = NULL;
}

//******************************************************************************
CSphereForms::~CSphereForms() {

    if (m_prGeneral) delete [] m_prGeneral;
    if (m_prLineList) delete [] m_prLineList;
    if (m_prTriList) delete [] m_prTriList;
    if (m_prTriStrip) delete [] m_prTriStrip;
    if (m_pwGeneral) delete [] m_pwGeneral;
    if (m_pwLineList) delete [] m_pwLineList;
    if (m_pwTriList) delete [] m_pwTriList;
    if (m_pwTriStrip) delete [] m_pwTriStrip;
}    

//******************************************************************************
BOOL CSphereForms::Create(UINT uSphereVertices) {

    PVT_VERTEX  pr;
    UINT        uModelVertices = uSphereVertices;
    UINT        i, j, k;
    UINT        uOffset, uFactor, uStepsU, uStepsV;
    float       fX, fY, fTX, fSinY, fCosY;

    do {
        uFactor = 1;
        j = (UINT)sqrt((double)uModelVertices);
        for (i = 2; i <= j; i++) {
            if (!(uModelVertices % i)) {
                uFactor = i;
            }
        }
        if (uFactor < 2) {
            uModelVertices /= 2;
            uModelVertices *= 2;
            DebugString(TEXT("Rounding the number of vertices per render to %d"), uModelVertices);
        }
        else {
            uStepsU = uFactor;
            uStepsV = uModelVertices / uFactor;
        }
    } while (uFactor == 1);

    m_urGeneral = uModelVertices;
    m_urLineList = (uModelVertices - 1) * 2;
    m_urTriList = (uStepsV - 1) * (uStepsU - 1) * 6;
    m_urTriStrip = (uStepsV - 1) * (uStepsU + 1) * 2 - 2;

    m_prGeneral = new VT_VERTEX[m_urGeneral];
    m_prLineList = new VT_VERTEX[m_urLineList];
    m_prTriList = new VT_VERTEX[m_urTriList];
    m_prTriStrip = new VT_VERTEX[m_urTriStrip];

    m_uwGeneral = uModelVertices;
    m_uwLineList = (uModelVertices - 1) * 2;
    m_uwTriList = (uStepsV - 1) * (uStepsU - 1) * 6;
    m_uwTriStrip = (uStepsV - 1) * (uStepsU + 1) * 2 - 2;

    m_pwGeneral = new WORD[m_uwGeneral];
    m_pwLineList = new WORD[m_uwLineList];
    m_pwTriList = new WORD[m_uwTriList];
    m_pwTriStrip = new WORD[m_uwTriStrip];

    if (!m_prGeneral || !m_prLineList || !m_prTriList || !m_prTriStrip ||
        !m_uwGeneral || !m_uwLineList || !m_uwTriList || !m_uwTriStrip)
    {
        return FALSE;
    }

    // Create the sphere vertices
    for (j = 0; j < uStepsV; j++) {

        fY = (float)j / (float)(uStepsV - 1);
        fSinY = (float)(sin(fY * M_PI));
        fCosY = (float)(cos(fY * M_PI));

        for (i = 0; i < uStepsU; i++) {

            pr = &m_prGeneral[uStepsU * j + i];
            fX = (float)i / (float)(uStepsU - 1);
            fTX = fX * M_2PI;

            pr->vPosition.x  = (float)(sin(fTX)) * fSinY;
            pr->vPosition.y  = fCosY;
            pr->vPosition.z  = -(float)(cos(fTX)) * fSinY;
            D3DXVec3Normalize((D3DXVECTOR3*)&pr->vNormal, &D3DXVECTOR3(pr->vPosition.x, pr->vPosition.y, pr->vPosition.z));
            pr->dwDiffuse = RGBA_MAKE(255 - rand() % 128, 255 - rand() % 128, 255 - rand() % 128, 128);
            pr->dwSpecular = RGBA_MAKE(255 - rand() % 128, 255 - rand() % 128, 255 - rand() % 128, 128);
//            pr->dwDiffuse = RGBA_MAKE(223, 255, 255, 128);
//            pr->dwSpecular = RGBA_MAKE(255, 255, 223, 128);
//            pr->dwDiffuse = RGBA_MAKE(32, 0, 0, 128);
//            pr->dwSpecular = RGBA_MAKE(0, 0, 32, 128);
            for (k = 0; k < 4; k++) {
//                pr->fTexCoord[k][0] = fX;
//                pr->fTexCoord[k][1] = fY;
//                pr->fTexCoord[k][0] = 0.1f + fX * 0.8f;
//                pr->fTexCoord[k][1] = 0.1f + fY * 0.8f;
                pr->fTexCoord[k][0] = 0.5f + fX * 1.0f;
                if (pr->fTexCoord[k][0] > 1.0f) pr->fTexCoord[k][0] -= 1.0f;
                pr->fTexCoord[k][1] = 0.5f + fY * 1.0f;
                if (pr->fTexCoord[k][1] > 1.0f) pr->fTexCoord[k][1] -= 1.0f;
            }
        }
    }

    // General cases
    for (i = 0; i < m_urGeneral; i++) {
        m_pwGeneral[i] = (WORD)i;
    }

    // Line lists
    for (i = 0, uOffset = 0; i < m_urGeneral - 1; i++) {
        m_prLineList[uOffset] = m_prGeneral[i];
        m_pwLineList[uOffset++] = (WORD)i;
        m_prLineList[uOffset] = m_prGeneral[i + 1];
        m_pwLineList[uOffset++] = i + 1;
    }

    // Triangle lists
    for (j = 0, uOffset = 0; j < uStepsV - 1; j++) {
        for (i = 0; i < uStepsU - 1; i++) {
            m_prTriList[uOffset] = m_prGeneral[(j + 1) * uStepsU + i];
            m_pwTriList[uOffset++] = (j + 1) * uStepsU + i;
            m_prTriList[uOffset] = m_prGeneral[j * uStepsU + i];
            m_pwTriList[uOffset++] = j * uStepsU + i;
            m_prTriList[uOffset] = m_prGeneral[(j + 1) * uStepsU + i + 1];
            m_pwTriList[uOffset++] = (j + 1) * uStepsU + i + 1;
            m_prTriList[uOffset] = m_prGeneral[j * uStepsU + i];
            m_pwTriList[uOffset++] = j * uStepsU + i;
            m_prTriList[uOffset] = m_prGeneral[j * uStepsU + i + 1];
            m_pwTriList[uOffset++] = j * uStepsU + i + 1;
            m_prTriList[uOffset] = m_prGeneral[(j + 1) * uStepsU + i + 1];
            m_pwTriList[uOffset++] = (j + 1) * uStepsU + i + 1;
        }
    }

    // Triangle strips
    for (j = 0, uOffset = 0; j < uStepsV - 1; j++) {
        for (i = 0; i < uStepsU; i++) {
            m_prTriStrip[uOffset] = m_prGeneral[(j + 1) * uStepsU + i];
            m_pwTriStrip[uOffset++] = (j + 1) * uStepsU + i;
            m_prTriStrip[uOffset] = m_prGeneral[j * uStepsU + i];
            m_pwTriStrip[uOffset++] = j * uStepsU + i;
        }

        if (j < uStepsV - 2) {
            m_prTriStrip[uOffset] = m_prGeneral[(j + 2) * uStepsU - 1];
            m_pwTriStrip[uOffset++] = (j + 2) * uStepsU - 1;
            m_prTriStrip[uOffset] = m_prGeneral[(j + 2) * uStepsU - 1];
            m_pwTriStrip[uOffset++] = (j + 2) * uStepsU - 1;
        }
    }

    return TRUE;
}

//******************************************************************************
HRESULT CSphereForms::Render(CDevice8* pDevice,
                             D3DPRIMITIVETYPE d3dpt, DWORD dwVertexType, 
                             BOOL bIndexed, BOOL bStrided, DWORD dwFlags)
{
    LPVOID  pvVertices;
    LPWORD  pwIndices;
    UINT    uNumVertices;
    UINT    uNumIndices;
    UINT    uNumPrimitives;
    HRESULT hr;
    DWORD   dwStride = sizeof(VT_VERTEX);
    LPBYTE  pvert;

    if (dwVertexType == FVF_TLVERTEX) {
        bStrided = FALSE;
    }

    switch (d3dpt) {
        case D3DPT_POINTLIST:
            pvVertices = m_prGeneral;
            pwIndices = m_pwGeneral;
            uNumVertices = m_urGeneral;
            uNumIndices = m_uwGeneral;
            uNumPrimitives = bIndexed ? uNumIndices : uNumVertices;
            break;
        case D3DPT_LINESTRIP:
            pvVertices = m_prGeneral;
            pwIndices = m_pwGeneral;
            uNumVertices = m_urGeneral;
            uNumIndices = m_uwGeneral;
            uNumPrimitives = bIndexed ? uNumIndices - 1 : uNumVertices - 1;
            break;
        case D3DPT_TRIANGLEFAN:
            pvVertices = m_prGeneral;
            pwIndices = m_pwGeneral;
            uNumVertices = m_urGeneral;
            uNumIndices = m_uwGeneral;
            uNumPrimitives = bIndexed ? uNumIndices - 2 : uNumVertices - 2;
            break;
        case D3DPT_LINELIST:
            pvVertices = bIndexed ? m_prGeneral : m_prLineList;
            pwIndices = m_pwLineList;
            uNumVertices = bIndexed ? m_urGeneral : m_urLineList;
            uNumIndices = m_uwLineList;
            uNumPrimitives = bIndexed ? uNumIndices / 2 : uNumVertices / 2;
            break;
        case D3DPT_TRIANGLELIST:
            pvVertices = bIndexed ? m_prGeneral : m_prTriList;
            pwIndices = m_pwTriList;
            uNumVertices = bIndexed ? m_urGeneral : m_urTriList;
            uNumIndices = m_uwTriList;
            uNumPrimitives = bIndexed ? uNumIndices / 3 : uNumVertices / 3;
            break;
        case D3DPT_TRIANGLESTRIP:
            pvVertices = bIndexed ? m_prGeneral : m_prTriStrip;
            pwIndices = m_pwTriStrip;
            uNumVertices = bIndexed ? m_urGeneral : m_urTriStrip;
            uNumIndices = m_uwTriStrip;
            uNumPrimitives = bIndexed ? uNumIndices - 2 : uNumVertices - 2;
            break;
    }

    switch (dwVertexType) {

        case FVF_VT_VERTEX: {
            PVT_VERTEX pr = (PVT_VERTEX)pvVertices;
            PVT_VERTEX pvr;
            UINT i, j;

            if (!bStrided) {
                dwStride = sizeof(VT_VERTEX);
            }
            else {
                dwStride = sizeof(VT_VERTEX) + sizeof(WORD);
            }
            pvVertices = MemAlloc32(uNumVertices * dwStride);
            if (!pvVertices) {
                return E_OUTOFMEMORY;
            }

            for (i = 0, pvert = (LPBYTE)pvVertices; i < uNumVertices; i++, pvert += dwStride) {
                pvr = (PVT_VERTEX)pvert;
                pvr->vPosition = pr[i].vPosition;
                pvr->dwDiffuse = RGBA_MAKE(196, 196, 196, 128);
                pvr->dwSpecular = RGBA_MAKE(64, 64, 64, 128);
                for (j = 0; j < 4; j++) {
                    pvr->fTexCoord[j][0] = pr[i].fTexCoord[j][0];
                    pvr->fTexCoord[j][1] = pr[i].fTexCoord[j][1];
                }
            }
            break;
        }
        case FVF_VT_LVERTEX: {
            PVT_VERTEX pr = (PVT_VERTEX)pvVertices;
            PVT_LVERTEX plr;
            UINT i, j;
            
            if (!bStrided) {
                dwStride = sizeof(VT_LVERTEX);
            }
            else {
                dwStride = sizeof(VT_LVERTEX) + sizeof(WORD);
            }
            pvVertices = MemAlloc32(uNumVertices * dwStride);
            if (!pvVertices) {
                return E_OUTOFMEMORY;
            }

            for (i = 0, pvert = (LPBYTE)pvVertices; i < uNumVertices; i++, pvert += dwStride) {
                plr = (PVT_LVERTEX)pvert;
                plr->vPosition = pr[i].vPosition;
                plr->dwDiffuse = RGBA_MAKE(196, 196, 196, 128);
                plr->dwSpecular = RGBA_MAKE(64, 64, 64, 128);
                for (j = 0; j < 4; j++) {
                    plr->fTexCoord[j][0] = pr[i].fTexCoord[j][0];
                    plr->fTexCoord[j][1] = pr[i].fTexCoord[j][1];
                }
            }
            break;
        }
        case FVF_VT_TLVERTEX: {

            PVT_VERTEX          pr = (PVT_VERTEX)pvVertices;
            PVT_TLVERTEX        ptlrV;
            UINT                i, j;

            if (!bStrided) {
                dwStride = sizeof(VT_TLVERTEX);
            }
            else {
                dwStride = sizeof(VT_TLVERTEX) + sizeof(WORD);
            }
            pvVertices = MemAlloc32(uNumVertices * dwStride);
            if (!pvVertices) {
                return E_OUTOFMEMORY;
            }

            hr = TransformVertices(pDevice, pvVertices, &pr[0].vPosition,
                                    uNumVertices, dwStride, sizeof(VT_VERTEX));
            if (SUCCEEDED(hr)) {
                for (i = 0, pvert = (LPBYTE)pvVertices; i < uNumVertices; i++, pvert += dwStride) {
                    ptlrV = (PVT_TLVERTEX)pvert;
                    ptlrV->dwDiffuse = RGBA_MAKE(196, 196, 196, 128);
                    ptlrV->dwSpecular = RGBA_MAKE(64, 64, 64, 128);
                    for (j = 0; j < 4; j++) {
                        ptlrV->fTexCoord[j][0] = pr[i].fTexCoord[j][0];
                        ptlrV->fTexCoord[j][1] = pr[i].fTexCoord[j][1];
                    }
                }
            }

            if (FAILED(hr)) {
                if (pvVertices) MemFree32(pvVertices);
                return hr;
            }

            break;
        }
    }

    pDevice->SetVertexShader(dwVertexType);

    if (!bStrided || dwVertexType == FVF_VT_TLVERTEX) {
        if (!bIndexed) {
            hr = pDevice->DrawPrimitiveUP(d3dpt, uNumPrimitives, 
                                    pvVertices, dwStride);
        }
        else {
            hr = pDevice->DrawIndexedPrimitiveUP(d3dpt, 0, uNumVertices,
                                    uNumPrimitives, pwIndices, D3DFMT_INDEX16,
                                    pvVertices, dwStride);
        }
    }

    MemFree32(pvVertices);

    return hr;
}

#define OFFSET_LOOP(inner, outer, range) \
    if (!bOverflow) return; \
    if (pnParam[inner] < 0 || pnParam[inner] >= range) { \
        if (pnParam[inner] > 0) { \
            pnParam[outer] += (pnParam[inner] / range); \
            pnParam[inner] %= range; \
        } \
        else { \
            for (; pnParam[inner] < 0; pnParam[inner] += range) { \
                pnParam[outer] -= 1; \
            } \
        } \
    } \
    else { \
        bOverflow = FALSE; \
    }

void OffsetLoops(int* pnParam, int nOffset) {

    int  i;
    BOOL bOverflow = TRUE;
    pnParam[SPECULAR] += nOffset;
    OFFSET_LOOP(SPECULAR, VERTEXFOG, 2)
    OFFSET_LOOP(VERTEXFOG, LIGHTING, 2)
    OFFSET_LOOP(LIGHTING, WRAPPING, 4)
    OFFSET_LOOP(WRAPPING, TEXTURING, 2)
    OFFSET_LOOP(TEXTURING, CLIPPING, 2)
    OFFSET_LOOP(CLIPPING, FILLMODE, 2)
    if (bOverflow) {
        if (pnParam[FILLMODE] < 0) {
            for (i = 0; i < 7; i++) {
                pnParam[i] = 0;
            }
        }
/*
        else if (pnParam[FILLMODE] >= 3) {
            pnParam[SPECULAR] = 1;
            pnParam[VERTEXFOG] = 1;
            pnParam[LIGHTING] = 3;
            pnParam[WRAPPING] = 1;
            pnParam[TEXTURING] = 1;
            pnParam[CLIPPING] = 1;
            pnParam[FILLMODE] = 2;
        }
*/
    }
}

DWORD CondenseState(int* pnParam) {

    DWORD dwState = 0;
    int   i;

    for (i = 0; i <= MAX_PARAM; i++) {
        dwState |= pnParam[i] << ((MAX_PARAM - i) * 3);
    }

    return dwState;
}

void OutputLoops(int* pnParam) {

    LPTSTR pszFillMode[] = {TEXT("Solid"), TEXT("Wireframe"), TEXT("Point")};
    LPTSTR pszTexture[] = {TEXT("None"), TEXT("Single"), TEXT("Multi-stage"), TEXT("Bumpmap")};
    LPTSTR pszLight[] = {TEXT("None (Pre-lit)"), TEXT("Ambient"), TEXT("Directional"), TEXT("Complex")};

    OutputDebugString(TEXT("\r\n"));
    DebugString(TEXT("Fill mode:          %s"), pszFillMode[pnParam[FILLMODE]]);
    DebugString(TEXT("Clipping:           %s"), pnParam[CLIPPING] ? TEXT("Front") : TEXT("None"));
    DebugString(TEXT("Texturing:          %s"), pszTexture[pnParam[TEXTURING]]);
    DebugString(TEXT("Wrapping:           %s"), pnParam[WRAPPING] ? TEXT("U and V") : TEXT("None"));
    DebugString(TEXT("Lighting:           %s"), pszLight[pnParam[LIGHTING]]);
    DebugString(TEXT("Vertex fog:         %s"), pnParam[VERTEXFOG] ? TEXT("On") : TEXT("Off"));
    DebugString(TEXT("Specular:           %s"), pnParam[SPECULAR] ? TEXT("On") : TEXT("Off"));
    DebugString(TEXT("State value:        %d\n"), CondenseState(pnParam));
}

