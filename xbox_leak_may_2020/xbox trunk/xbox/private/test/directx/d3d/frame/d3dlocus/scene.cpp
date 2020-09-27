/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    scene.cpp

Author:

    Matt Bronder

Description:

    Scene functions.

*******************************************************************************/

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include "profilep.h"
#endif // UNDER_XBOX
//#include <mmsystem.h>

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

// Function table for x_ValidateScene and x_ExhibitScene
#ifdef UNDER_XBOX

#pragma data_seg(".D3D$A")
D3DTESTFUNCTIONDESC D3D_BeginTestFunctionTable = {0};
#pragma data_seg()

#pragma data_seg(".D3D$ZZZZZZZZ")
D3DTESTFUNCTIONDESC D3D_EndTestFunctionTable = {0};
#pragma data_seg()

#pragma comment(linker, "/merge:.D3D=.rdata")

#endif // UNDER_XBOX


//******************************************************************************
// Function prototypes
//******************************************************************************

static BOOL                     InitScene(LPVOID pvParam);
static void                     EffaceScene(LPVOID pvParam);

static BOOL                     SceneWndProc(LPVOID pvParam, LRESULT* plr, 
                                             HWND hWnd, UINT uMsg, 
                                             WPARAM wParam, LPARAM lParam);

//******************************************************************************
// CScene
//******************************************************************************

float CScene::m_fTimeDilation = 1.0f;
BOOL  CScene::m_bShowFPS = FALSE;
BOOL  CScene::m_bShowMode = FALSE;
UINT  CScene::m_uCreated = 0;

//******************************************************************************
//
// Method:
//
//     CScene
//
// Description:
//
//     Initialize a scene object.
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
CScene::CScene() {

    m_pDisplay = NULL;
    m_pDevice = NULL;
    m_bDisplayOK = TRUE;
    m_bPaused = FALSE;
    m_bAdvance = FALSE;
    m_fFPS = 0.0f;
    m_fFrameDuration = FLT_INFINITE;
    m_fTimeDuration = FLT_INFINITE;
    m_fTime = 0.0f;
    m_fFrame = 0.0f;
    m_fLastTime = 0.0f;
    m_fLastFrame = 0.0f;
#ifdef UNDER_XBOX
    m_bQuit = FALSE;
#endif // UNDER_XBOX
    InitMatrix(&m_mIdentity,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    m_dwInitialState = 0xFFFFFFFF;
    memset(&m_d3dpp, 0xCCCCCCCC, sizeof(D3DPRESENT_PARAMETERS));
    memset(&m_d3dppInitial, 0xCCCCCCCC, sizeof(D3DPRESENT_PARAMETERS));
    m_bFade = !(GetStartupContext() & TSTART_SEQUENTIAL);
    m_pfnFade = NULL;
    m_uCreated++;
    m_bCreated = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CScene
//
// Description:
//
//     Clean up the scene.
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
CScene::~CScene() {

    if (m_pDisplay) {

        m_pDisplay->InitSetDeviceProc(NULL, NULL);
        m_pDisplay->InitClearDeviceProc(NULL, NULL);
        m_pDisplay->InitSubWindowProc(NULL, NULL);

        if (m_bRestoreMode) {
            m_pDisplay->Reset(&m_d3dppInitial);
        }
    }
    m_uCreated--;
}

//******************************************************************************
//
// Method:
//
//     Release
//
// Description:
//
//     Efface the scene and release it.
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
ULONG CScene::Release() {

    if (RefCount() == 1) {
VerifyTextureListIntegrity();
        Efface();
    }
    return CObject::Release();
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the scene for rendering.
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
BOOL CScene::Create(CDisplay* pDisplay) {

    TCHAR szName[MAX_PATH] = {0};
    DWORD dwParam = 0xCCCCCCCC;

    if (m_bCreated || m_uCreated > 1) {
        return FALSE;
    }

    if (!pDisplay) {
        return FALSE;
    }

    m_pDisplay = pDisplay;

    m_pDevice = m_pDisplay->GetDevice8();
    if (!m_pDevice) {
        return FALSE;
    }

    // Reset the device if any changes were made to the present parameters
    if (memcmp(&m_d3dpp, &m_d3dppInitial, sizeof(D3DPRESENT_PARAMETERS))) {
        D3DPRESENT_PARAMETERS d3dpp;
        m_pDisplay->GetPresentParameters(&m_d3dppInitial);
        m_pDisplay->GetPresentParameters(&d3dpp);
        if (m_d3dpp.BackBufferWidth != dwParam) {
            d3dpp.BackBufferWidth = m_d3dpp.BackBufferWidth;
        }
        if (m_d3dpp.BackBufferHeight != dwParam) {
            d3dpp.BackBufferHeight = m_d3dpp.BackBufferHeight;
        }
        if (m_d3dpp.BackBufferFormat != (D3DFORMAT)dwParam) {
            d3dpp.BackBufferFormat = m_d3dpp.BackBufferFormat;
        }
        if (m_d3dpp.BackBufferCount != dwParam) {
            d3dpp.BackBufferCount = m_d3dpp.BackBufferCount;
        }
        if (m_d3dpp.MultiSampleType != (D3DMULTISAMPLE_TYPE)dwParam) {
            d3dpp.MultiSampleType = m_d3dpp.MultiSampleType;
        }
        if (m_d3dpp.SwapEffect != (D3DSWAPEFFECT)dwParam) {
            d3dpp.SwapEffect = m_d3dpp.SwapEffect;
        }
        if (m_d3dpp.hDeviceWindow != (HWND)dwParam) {
            d3dpp.hDeviceWindow = m_d3dpp.hDeviceWindow;
        }
        if (m_d3dpp.Windowed != (BOOL)dwParam) {
            d3dpp.Windowed = m_d3dpp.Windowed;
        }
        if (m_d3dpp.EnableAutoDepthStencil != (BOOL)dwParam) {
            d3dpp.EnableAutoDepthStencil = m_d3dpp.EnableAutoDepthStencil;
        }
        if (m_d3dpp.AutoDepthStencilFormat != (D3DFORMAT)dwParam) {
            d3dpp.AutoDepthStencilFormat = m_d3dpp.AutoDepthStencilFormat;
        }
        if (m_d3dpp.Flags != dwParam) {
            d3dpp.Flags = m_d3dpp.Flags;
        }
        if (m_d3dpp.FullScreen_RefreshRateInHz != dwParam) {
            d3dpp.FullScreen_RefreshRateInHz = m_d3dpp.FullScreen_RefreshRateInHz;
        }
        if (m_d3dpp.FullScreen_PresentationInterval != dwParam) {
            d3dpp.FullScreen_PresentationInterval = m_d3dpp.FullScreen_PresentationInterval;
        }
        memcpy(&m_d3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        if (!m_pDisplay->Reset(&m_d3dpp)) {
__asm int 3;
            DebugString(TEXT("Failed to Reset the display to the requested mode"));
            return FALSE;
        }
        m_bRestoreMode = TRUE;
    }

    // Set device initialization and wndproc callback functions
    pDisplay->InitSetDeviceProc(InitScene, this);
    pDisplay->InitClearDeviceProc(EffaceScene, this);
    pDisplay->InitSubWindowProc(SceneWndProc, this);

    // Get the name of the module
    GetModuleName(GetTestInstance(), szName, MAX_PATH);

    // Log the component and subcomponent
    LogComponent(TEXT("Direct3D"), szName);

    // Set the test duration
    if (GetStartupContext() & TSTART_SEQUENTIAL) {

        DWORD dwDuration;

        dwDuration = GetProfileInt(szName, TEXT("FrameDuration"), 0xFFFFFFFE);
        if (dwDuration != 0xFFFFFFFE) {
            if (dwDuration == 0) {
                m_fFrameDuration = FLT_INFINITE;
            }
            else {
                m_fFrameDuration = (float)dwDuration;
            }
        }
        dwDuration = GetProfileInt(szName, TEXT("TimeDuration"), 0xFFFFFFFE);
        if (dwDuration != 0xFFFFFFFE) {
            if (dwDuration == 0) {
                m_fTimeDuration = FLT_INFINITE;
            }
            else {
                m_fTimeDuration = (float)dwDuration;
            }
        }
    }
    else {
        m_fFrameDuration = FLT_INFINITE;
        m_fTimeDuration = FLT_INFINITE;
    }

    if (!Prepare()) {
        return FALSE;
    }

    if (!Setup()) {
        return FALSE;
    }

    if (!InitView()) {
        return FALSE;
    }

    if (!Initialize()) {
        return FALSE;
    }

    m_bCreated = TRUE;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Display the scene.
//
// Arguments:
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
BOOL CScene::Exhibit(int* pnExitCode) {

    BOOL            bMsgReady;
    float           fTime, fLastTime, fPrevTime, fTimeFreq;
    LARGE_INTEGER   qwCounter;
    DWORD           dwNumFrames = 0;
#ifndef UNDER_XBOX
    MSG             msg;
#endif // !UNDER_XBOX

    if (!m_bCreated) {
        return TRUE;
    }

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
    fPrevTime = fLastTime;

    do {

#ifndef UNDER_XBOX
        // Pump messages
        if (m_pDisplay->IsActive()) {

            // When the application is active, check for new
            // messages without blocking to wait on them
            bMsgReady = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        }
        else {

            // The application has lost the focus and is suspended
            // so we can afford to block here until a new message
            // has arrived (and conserve CPU usage in the process)
            bMsgReady = GetMessage(&msg, NULL, 0, 0);
        }

        if (bMsgReady) {

            // If a message is ready, process it and proceed to
            // check for another message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (msg.message != WM_QUIT) {
#endif // !UNDER_XBOX

            // Otherwise process user input, update the next frame,
            // and draw it
            QueryPerformanceCounter(&qwCounter);
            fTime = (float)qwCounter.QuadPart * fTimeFreq;

            // Process user input
            m_pDisplay->ProcessInput();
            if (!m_pDisplay->ConsoleVisible()) {
                ProcessInput();
            }

            if (m_pDisplay->m_bTimeSync) {
                m_pDisplay->m_bTimeSync = FALSE;
            }
            else if (!m_bPaused || m_bAdvance || (GetStartupContext() & TSTART_STRESS)) {
                m_fTime += (fTime - fLastTime) * m_fTimeDilation;
                m_fFrame += 1.0f;//(1.0f * m_fTimeDilation); // ##REVIEW
                if (m_fFrameDuration != FLT_INFINITE && m_fFrame > m_fFrameDuration) {
                    break;
                }
                if (m_fTimeDuration != FLT_INFINITE && m_fTime > m_fTimeDuration) {
                    break;
                }
                m_fTimeDelta = m_fTime - m_fLastTime;
                m_fFrameDelta = m_fFrame - m_fLastFrame;
                Update();
                VerifyTextureListIntegrity();
                m_fLastTime = m_fTime;
                m_fLastFrame = m_fFrame;
                m_bAdvance = FALSE;
            }

            fLastTime = fTime;

            if (!Render()) {
                break;
            }

            VerifyTextureListIntegrity();

            dwNumFrames++;

            if (fTime - fPrevTime > 1.0f) {
                m_fFPS = (float)dwNumFrames / (fTime - fPrevTime);
                fPrevTime = fTime;
                dwNumFrames = 0;
            }
#ifndef UNDER_XBOX
        }

    } while (msg.message != WM_QUIT);
#else
    } while (!m_bQuit);
#endif // UNDER_XBOX

VerifyTextureListIntegrity();

    if (pnExitCode) {
#ifndef UNDER_XBOX
        *pnExitCode = msg.wParam;
#else
        *pnExitCode = 0;
#endif // UNDER_XBOX
    }

    return m_bDisplayOK;
}

//******************************************************************************
//
// Method:
//
//     SetFrameDuration
//
// Description:
//
//     Set the scene duration in frames.
//
// Arguments:
//
//     float fNumFrames             - Duration of the scene measured in frames.
//                                    This number is checked against the current
//                                    frame set during UpdateScene and is 
//                                    subject to time dilation.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CScene::SetFrameDuration(float fNumFrames) {

    m_fFrameDuration = fNumFrames;
}

//******************************************************************************
//
// Method:
//
//     SetTimeDuration
//
// Description:
//
//     Set the scene duration in seconds.
//
// Arguments:
//
//     float fSeconds               - Duration of the scene measured in seconds.
//                                    This number is checked against the current
//                                    scene time given to UpdateScene and is 
//                                    subject to time dilation.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CScene::SetTimeDuration(float fSeconds) {

    m_fTimeDuration = fSeconds;
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
BOOL CScene::Prepare() {

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
//     TRUE if the scene was successfully set up for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CScene::Setup() {

    // Obtain a pointer to the device from the display (if the scene
    // is being re-initialized the device may have changed in response to
    // the selection of a new adapter)
    m_pDevice = m_pDisplay->GetDevice8();

    if (!m_pDevice) {
        return FALSE;
    }

    // Capture the state of the device
#ifndef DEMO_HACK
    if (FAILED(m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_dwInitialState))) {
        return FALSE;
    }
#endif

    // Initialize background vertices
    m_prBackground[0] = TLVERTEX(D3DXVECTOR3(-0.5f, (float)(m_pDisplay->GetHeight()) - 0.5f, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 0.0f, 1.0f);
    m_prBackground[1] = TLVERTEX(D3DXVECTOR3(-0.5f, -0.5f, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 0.0f, 0.0f);
    m_prBackground[2] = TLVERTEX(D3DXVECTOR3((float)(m_pDisplay->GetWidth()) - 0.5f, -0.5f, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 1.0f, 0.0f);
    m_prBackground[3] = TLVERTEX(D3DXVECTOR3((float)(m_pDisplay->GetWidth()) - 0.5f, (float)(m_pDisplay->GetHeight()) - 0.5f, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 1.0f, 1.0f);

    // Initialize the display mode string
    InitDisplayModeString();

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
//     Initialize the device and all device objects to be used in the scene (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CScene::Initialize() {

    D3DLIGHT8 light;
    HRESULT   hr;

    // Create a directional light
    memset(&light, 0, sizeof(D3DLIGHT8));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Diffuse.a = 1.0f;
    light.Specular.r = 1.0f;
    light.Specular.g = 1.0f;
    light.Specular.b = 1.0f;
    light.Specular.a = 1.0f;
    light.Ambient.r = 1.0f;
    light.Ambient.g = 1.0f;
    light.Ambient.b = 1.0f;
    light.Ambient.a = 1.0f;
    light.Direction = D3DXVECTOR3(0.0f, -1.0f, 1.0f);

    hr = m_pDevice->SetLight(0, &light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    // Set a material
    if (!SetMaterial(m_pDevice, RGBA_MAKE(255, 255, 255, 255))) {
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
void CScene::Efface() {

VerifyTextureListIntegrity();
    ReleaseVertexBuffers();
VerifyTextureListIntegrity();
    ReleaseIndexBuffers();
VerifyTextureListIntegrity();
    ReleaseTextures();
#ifdef UNDER_XBOX
    ReleasePalettes();
#endif

    // Revert to the initial device state
    if (m_bDisplayOK && m_pDevice && m_dwInitialState != 0xFFFFFFFF) {
#ifndef DEMO_HACK
        m_pDevice->ApplyStateBlock(m_dwInitialState);
        m_pDevice->DeleteStateBlock(m_dwInitialState);
#endif
    }
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
void CScene::Update() {

    // Update the scene
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render a scene.
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
BOOL CScene::Render() {

    LVERTEX  plrVertices[3];
    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

    plrVertices[0] = LVERTEX(D3DXVECTOR3(-10.0f, -10.0f, 0.0f), RGBA_MAKE(255, 0, 0, 255), 0, 0.0f, 0.0f);
    plrVertices[1] = LVERTEX(D3DXVECTOR3( 0.0f,  10.0f, 0.0f), RGBA_MAKE(0, 0, 255, 255), 0, 0.0f, 0.0f);
    plrVertices[2] = LVERTEX(D3DXVECTOR3( 10.0f, -10.0f, 0.0f), RGBA_MAKE(0, 255, 0, 255), 0, 0.0f, 0.0f);

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

    // Disable lighting
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_LVERTEX);

    // Draw a triangle
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, plrVertices, sizeof(LVERTEX));

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
// Function:
//
//     Method
//
// Description:
//
//     Reset the scene in response to the restoration of a lost device.
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
BOOL CScene::Reset() {

    if (m_pDisplay) {
        if (!m_pDisplay->ResetDevice()) {
            return FALSE;
        }
    }

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
void CScene::ProcessInput() {

    static UINT uFMDisplay = 0;

    m_pDisplay->GetJoyState(&m_jsJoys, &m_jsLast);

#ifdef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            FadeOut();
            m_bQuit = TRUE;
        }
    }
#endif // UNDER_XBOX

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                m_bPaused = !m_bPaused;
            }
            else {
                m_bPaused = TRUE;
                m_bAdvance = TRUE;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_B)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_B)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                // ##TODO: Reset the device and toggle FSAA
            }
            else {
                if (m_fTimeDilation < 1.5f && m_fTimeDilation > 0.26f) {
                    m_fTimeDilation /= 2.0f;
                }
                else if (m_fTimeDilation < 0.26f) {
                    m_fTimeDilation = 2.0f;
                }
                else if (m_fTimeDilation >= 1.5f) {
                    m_fTimeDilation = 1.0f;
                }
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_WHITE)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_WHITE) && !BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
            m_pDisplay->SetCamera(&m_camInitial);
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
BOOL CScene::InitView() {

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
//
// Function:
//
//     InitScene
//
// Description:
//
//     Initialize the device and all device objects to be used in the scene (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     LPVOID pvParam           - Context data
//
// Return Value:
//
//     TRUE if the scene was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
static BOOL InitScene(LPVOID pvParam) {

    if (pvParam) {
        if (!((CScene*)pvParam)->Setup()) {
            return FALSE;
        }
        return ((CScene*)pvParam)->Initialize();
    }
    else {
        return FALSE;
    }
}

//******************************************************************************
//
// Function:
//
//     EffaceScene
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
//
// Arguments:
//
//     LPVOID pvParam           - Context data
//
// Return Value:
//
//     None.
//
//******************************************************************************
static void EffaceScene(LPVOID pvParam) {

    if (pvParam) {
        ((CScene*)pvParam)->Efface();
    }
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SceneWndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LPVOID pvParam           - Context data
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
static BOOL SceneWndProc(LPVOID pvParam, LRESULT* plr, HWND hWnd, UINT uMsg, 
                         WPARAM wParam, LPARAM lParam)
{
    if (pvParam) {
        return ((CScene*)pvParam)->WndProc(plr, hWnd, uMsg, wParam, lParam);
    }
    else {
        *plr = 0;
        return FALSE;
    }
}

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
BOOL CScene::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_RETURN: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;

        case WM_KEYUP:

            switch (wParam) {

                case VK_SPACE:
                    m_bPaused = TRUE;
                    m_bAdvance = TRUE;
                    return TRUE;
            }

            break;

        case WM_CHAR:

            switch (wParam) {

                case TEXT('O'):
                case TEXT('o'):
                    // Reset the camera to its initial position
                    m_pDisplay->SetCamera(&m_camInitial);
                    return TRUE;

                case TEXT('F'):
                case TEXT('f'):
                    m_bShowFPS = !m_bShowFPS;
                    return TRUE;

                case TEXT('M'):
                case TEXT('m'):
                    m_bShowMode = !m_bShowMode;
                    return TRUE;

                case TEXT('T'):
                case TEXT('t'):
                    if (m_fTimeDilation < 1.5f && m_fTimeDilation > 0.26f) {
                        m_fTimeDilation /= 2.0f;
                    }
                    else if (m_fTimeDilation < 0.26f) {
                        m_fTimeDilation = 2.0f;
                    }
                    else if (m_fTimeDilation >= 1.5f) {
                        m_fTimeDilation = 1.0f;
                    }
                    return 0;

                case TEXT('P'):      // Point fill
                case TEXT('p'):
                    m_pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_POINT);
                    return TRUE;

                case TEXT('W'):      // Wireframe fill
                case TEXT('w'):
                    m_pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_WIREFRAME);
                    return TRUE;

                case TEXT('S'):      // Solid fill
                case TEXT('s'):
                    m_pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_SOLID);
                    return TRUE;

                case TEXT('I'):      // Toggle dithering
                case TEXT('i'): {
                    BOOL bDitheringOn;
                    if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_DITHERENABLE, (LPDWORD)&bDitheringOn))) {
                        bDitheringOn = !bDitheringOn;
                        m_pDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)bDitheringOn);
                    }
                    return TRUE;
                }

                case TEXT('N'):      // Toggle anti-aliasing
                case TEXT('n'): {
                    BOOL bAntialias;
                    if (SUCCEEDED(m_pDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (LPDWORD)&bAntialias))) {
                        bAntialias = !bAntialias;
                        m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (DWORD)bAntialias);
                    }
                    return TRUE;
                }

                case TEXT('E'):
                case TEXT('e'): {
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
                    return TRUE;
                }
            }
            break;

        case WM_CLOSE:
            FadeOut();
            break;
    }

#endif // !UNDER_XBOX

    return FALSE;
}

//******************************************************************************
// Frame rate
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     ShowFrameRate
//
// Description:
//
//     Overlay the frame rate in the scene.
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
void CScene::ShowFrameRate() {

    if (m_bShowFPS && !(GetStartupContext() & TSTART_STRESS)) {

        TCHAR sz[256];
        float fHeight = (float)m_pDisplay->GetHeight() / 20.0f;
        float fWidth = fHeight / CHAR_ASPECT;
        UINT  uLen;
        _stprintf(sz, TEXT("%.02f fps"), m_fFPS);
        uLen = _tcslen(sz);
        xTextOut((float)(m_pDisplay->GetWidth() / 2) - (float)(uLen / 2) * fWidth, 
                    (float)m_pDisplay->GetHeight() - (fHeight * 2.0f), fWidth, fHeight, 
                    RGBA_XCOLOR(255, 255, 0, 255), sz, uLen);
    }
}

//******************************************************************************
// Display mode
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     ShowDisplayMode
//
// Description:
//
//     Overlay the current display mode in the scene.
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
void CScene::ShowDisplayMode() {

    if (m_bShowMode && !(GetStartupContext() & TSTART_STRESS)) {

        UINT uLen;
        float fHeight = (float)m_pDisplay->GetHeight() / 20.0f;
        float fWidth = fHeight / CHAR_ASPECT;
        uLen = _tcslen(m_szDisplayMode);
        xTextOut((float)(m_pDisplay->GetWidth() / 2) - (float)(uLen / 2) * fWidth, 
                    fHeight, fWidth, fHeight, RGBA_XCOLOR(255, 255, 0, 255), 
                    m_szDisplayMode, uLen);
    }
}

//******************************************************************************
//
// Function:
//
//     InitDisplayModeString
//
// Description:
//
//     Initialize the display mode string.
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
void CScene::InitDisplayModeString() {

    DISPLAYDESC disd;
    TCHAR szFormat[16];
    D3DFORMAT fmtd;
    D3DDISPLAYMODE dm;
    D3DMULTISAMPLE_TYPE mst;

    // Initialize the display mode string
    m_pDisplay->GetCurrentDisplay(&disd);
    if (disd.bWindowed) {
        dm.Width = m_pDisplay->GetWidth();
        dm.Height = m_pDisplay->GetHeight();
        dm.RefreshRate = disd.padpd->d3ddmWindowed.RefreshRate;
        dm.Format = disd.padpd->d3ddmWindowed.Format;
        fmtd = disd.pdevd->fmtdWindowed;
        mst = disd.pdevd->d3dmstWindowed;
    }
    else {
        dm.Width = disd.pdm->d3ddm.Width;
        dm.Height = disd.pdm->d3ddm.Height;
        dm.RefreshRate = disd.pdm->d3ddm.RefreshRate;
        dm.Format = disd.pdm->d3ddm.Format;
        fmtd = disd.pdm->fmtd;
        mst = disd.pdm->d3dmst;
    }

    _stprintf(m_szDisplayMode, TEXT("%dx%d"), dm.Width, dm.Height);
    if (dm.RefreshRate) {
        _stprintf(m_szDisplayMode + _tcslen(m_szDisplayMode), TEXT(" %dHz"), dm.RefreshRate);
    }
    switch (dm.Format) {
#ifndef UNDER_XBOX
        case D3DFMT_X8R8G8B8:
#else
        case D3DFMT_LIN_X8R8G8B8:
#endif // UNDER_XBOX
            _tcscpy(szFormat, TEXT(" X8R8G8B8"));
            break;
#ifndef UNDER_XBOX
        case D3DFMT_A8R8G8B8:
#else
        case D3DFMT_LIN_A8R8G8B8:
#endif // UNDER_XBOX
            _tcscpy(szFormat, TEXT(" A8R8G8B8"));
            break;
#ifndef UNDER_XBOX
        case D3DFMT_X1R5G5B5:
#else
        case D3DFMT_LIN_X1R5G5B5:
#endif // UNDER_XBOX
            _tcscpy(szFormat, TEXT(" X1R5G5B5"));
            break;
#ifndef UNDER_XBOX
        case D3DFMT_R5G6B5:
#else
        case D3DFMT_LIN_R5G6B5:
#endif // UNDER_XBOX
            _tcscpy(szFormat, TEXT(" R5G6B5"));
            break;
    }
    _tcscat(m_szDisplayMode, szFormat);
    if (disd.bDepthBuffer) {
        switch (fmtd) {
#ifndef UNDER_XBOX
            case D3DFMT_D16:
#else
            case D3DFMT_LIN_D16:
#endif // UNDER_XBOX
                _tcscat(m_szDisplayMode, TEXT(" D16"));
                break;
#ifndef UNDER_XBOX
            case D3DFMT_D24S8:
#else
            case D3DFMT_LIN_D24S8:
#endif // UNDER_XBOX
                _tcscat(m_szDisplayMode, TEXT(" D24S8"));
                break;
#ifdef UNDER_XBOX
            case D3DFMT_LIN_F16:
                _tcscat(m_szDisplayMode, TEXT(" F16"));
                break;
            case D3DFMT_LIN_F24S8:
                _tcscat(m_szDisplayMode, TEXT(" F24S8"));
                break;
#endif // UNDER_XBOX
        }
    }
    if (disd.bAntialias && mst != D3DMULTISAMPLE_NONE) {
#ifndef UNDER_XBOX
        _stprintf(m_szDisplayMode + _tcslen(m_szDisplayMode), TEXT(" %dx"), (UINT)mst);
#else
        switch (mst & 0xFFFF) {
            case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR:
                _tcscat(m_szDisplayMode, TEXT(" 2xML"));
                break;
            case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX:
                _tcscat(m_szDisplayMode, TEXT(" 2xMQ"));
                break;
            case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR:
                _tcscat(m_szDisplayMode, TEXT(" 2xMH"));
                break;
            case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
                _tcscat(m_szDisplayMode, TEXT(" 2xMV"));
                break;
            case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR:
                _tcscat(m_szDisplayMode, TEXT(" 4xML"));
                break;
            case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN:
                _tcscat(m_szDisplayMode, TEXT(" 4xMG"));
                break;
            case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR:
                _tcscat(m_szDisplayMode, TEXT(" 4xSL"));
                break;
            case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
                _tcscat(m_szDisplayMode, TEXT(" 4xSG"));
                break;
            case D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN:
                _tcscat(m_szDisplayMode, TEXT(" 9xMG"));
                break;
            case D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
                _tcscat(m_szDisplayMode, TEXT(" 9xSG"));
                break;
        }
        switch (mst & 0xF0000) {
            case D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT:
                _tcscat(m_szDisplayMode, szFormat);
                break;
            case D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5:
                _tcscat(m_szDisplayMode, TEXT(" X1R5G5B5"));
                break;
            case D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5:
                _tcscat(m_szDisplayMode, TEXT(" R5G6B5"));
                break;
            case D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8:
                _tcscat(m_szDisplayMode, TEXT(" X8R8G8B8"));
                break;
            case D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8:
                _tcscat(m_szDisplayMode, TEXT(" X8R8G8B8"));
                break;
        }
#endif // UNDER_XBOX
    }
}

//******************************************************************************
// Fade functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     FadeOut
//
// Description:
//
//     Fade out the scene.
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
void CScene::FadeOut() {

    int i, fps;

    if (!m_bFade || (GetStartupContext() & TSTART_STRESS)) {
        return;
    }

    m_prFade[0] = TLVERTEX(D3DXVECTOR3(0.0f, (float)m_pDisplay->GetHeight(), 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 1.0f);
    m_prFade[1] = TLVERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 0.0f, 0.0f);
    m_prFade[2] = TLVERTEX(D3DXVECTOR3((float)m_pDisplay->GetWidth(), 0.0f, 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 0.0f);
    m_prFade[3] = TLVERTEX(D3DXVECTOR3((float)m_pDisplay->GetWidth(), (float)m_pDisplay->GetHeight(), 0.000009f), 110000.0f, RGBA_MAKE(0,0,0,255), RGBA_MAKE(0,0,0,0), 1.0f, 1.0f);

    m_pfnFade = ApplyFade;

    fps = (int)(m_fFPS + 0.5f);

    for (i = fps - 1; i >= 0; i--) {
        m_fadeAlpha = 255 - (BYTE)((float)i / (float)fps * 255.0f);
        if (!m_bPaused) {
            Update();
        }
        Render();
    }
}

//******************************************************************************
//
// Function:
//
//     ApplyFade
//
// Description:
//
//     Fade the scene to black.
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
void CScene::ApplyFade() {

//    CBaseTexture8*          pd3dtCurrent;
//    CVertexBuffer8*         pd3drCurrent;
//    UINT                    uStride;
//    DWORD                   dwSrcBlend, dwDstBlend, dwBlend, dwColorOp0, 
//                            dwColorOp1, dwAlphaOp0, dwColorArg1, dwColorArg2, 
//                            dwAlphaArg1, dwAlphaArg2, dwAlphaTest, dwShader;
    DWORD dwBlock;
    HRESULT hr;

    // Save the device state
    hr = m_pDevice->CreateStateBlock(D3DSBT_ALL, &dwBlock);
    if (FAILED(hr)) {
        return;
    }

    // Fade out the scene
    m_prFade[0].cDiffuse = m_prFade[1].cDiffuse = m_prFade[2].cDiffuse 
                         = m_prFade[3].cDiffuse = m_fadeAlpha << 24;

//    m_pDevice->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
//    m_pDevice->GetRenderState(D3DRS_DESTBLEND, &dwDstBlend);
//    m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwBlend);
//    m_pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
//    m_pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
//    m_pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
 //   m_pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
 //   m_pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
//    m_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
//    m_pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
//    m_pDevice->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
//    m_pDevice->GetStreamSource(0, &pd3drCurrent, &uStride);
//    m_pDevice->GetTexture(0, &pd3dtCurrent);
//    m_pDevice->GetVertexShader(&dwShader);

    m_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetVertexShader(FVF_TLVERTEX);
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prFade, sizeof(TLVERTEX));

//    m_pDevice->SetVertexShader(dwShader);
//    m_pDevice->SetTexture(0, pd3dtCurrent);
//    if (pd3drCurrent) {
//        m_pDevice->SetStreamSource(0, pd3drCurrent, uStride);
//    }
//    SetColorStage(m_pDevice, 0, dwColorArg1, dwColorArg2, (D3DTEXTUREOP)dwColorOp0);
//    SetAlphaStage(m_pDevice, 0, dwAlphaArg1, dwAlphaArg2, (D3DTEXTUREOP)dwAlphaOp0);
//    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
//    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, dwBlend);
//    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
//    m_pDevice->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
//    m_pDevice->SetRenderState(D3DRS_DESTBLEND, dwDstBlend);

    m_pDevice->ApplyStateBlock(dwBlock);
    m_pDevice->DeleteStateBlock(dwBlock);
}
