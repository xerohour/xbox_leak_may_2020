/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    misorbit.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "revolve.h"
#include "misorbit.h"
#ifdef UNDER_XBOX
#include "xtestlib.h"
#endif // UNDER_XBOX

#ifdef UNDER_XBOX
//******************************************************************************
BYTE
CalculateCpuUsage(
    VOID
    )
{
    static LARGE_INTEGER PrevTotalTime = { 0, 0 };
    static LARGE_INTEGER PrevIdleTime = { 0, 0 };
    static LARGE_INTEGER PrevKernelTime = { 0, 0 };

    LARGE_INTEGER KernelTime;
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER TotalTime;

    LARGE_INTEGER DeltaTotalTime;
    LARGE_INTEGER DeltaIdleTime;
    LARGE_INTEGER DeltaKernelTime;

    BYTE CpuUsage;

    // Ignore Interrupt, DPC and User time
    QueryCpuTimesInformation( &KernelTime, &IdleTime, NULL, NULL, NULL );

    TotalTime.QuadPart = KernelTime.QuadPart + IdleTime.QuadPart;

    DeltaIdleTime.QuadPart = IdleTime.QuadPart - PrevIdleTime.QuadPart;
    DeltaKernelTime.QuadPart = KernelTime.QuadPart - PrevKernelTime.QuadPart;
    DeltaTotalTime.QuadPart = TotalTime.QuadPart - PrevTotalTime.QuadPart;

    PrevTotalTime.QuadPart = TotalTime.QuadPart;
    PrevIdleTime.QuadPart = IdleTime.QuadPart;
    PrevKernelTime.QuadPart = KernelTime.QuadPart;

    if ( DeltaTotalTime.QuadPart == 0 ) {
        CpuUsage = 0;
    } else {
        CpuUsage = (BYTE)( 100 - ( (DeltaIdleTime.QuadPart * 100)/DeltaTotalTime.QuadPart) );
    }

    return CpuUsage;
}
#endif

//******************************************************************************
//
// Function:
//
//     SphereRadius
//
// Description:
//
//     Describe the function profile of a sphere revolution.  This function will
//     return the radius of the sphere at a given height.
//
// Arguments:
//
//     float y                  - Height of the sphere at which to evaluate the 
//                                radius
//
//     float fLength            - Maximum height of the sphere
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius of the sphere at the given height.
//
//******************************************************************************
float SphereRadius(float y, float fLength, LPVOID pvContext) {

    float f = (float)((pow((double)(fLength/(float)(2.0)), 2.0) - 
                        pow((double)y - (double)(fLength / (float)(2.0)), 2.0)));
    if (f < (float)(0.00001) && f > (float)(-0.00001)) {
        f = (float)(0.0);
    }
    return (float)(sqrt((double)f));
}

//******************************************************************************
//
// Function:
//
//     CylinderRadius
//
// Description:
//
//     Describe the function profile of a cylinder revolution.  This function 
//     will return the radius of the cylinder at a given height.
//
// Arguments:
//
//     float y                  - Height of the cylinder at which to evaluate  
//                                the radius
//
//     float fLength            - Maximum height of the cylinder
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius of the cylinder at the given height.
//
//******************************************************************************
float CylinderRadius(float y, float fLength, LPVOID pvContext) {

    return 1.0f;
}

//******************************************************************************
//
// Function:
//
//     ConeRadius
//
// Description:
//
//     Describe the function profile of a cone revolution.  This function 
//     will return the radius of the cone at a given height.
//
// Arguments:
//
//     float y                  - Height of the cone at which to evaluate  
//                                the radius
//
//     float fLength            - Maximum height of the cone
//
//     LPVOID pvContext         - Variable supplied by the calling function.
//
// Return Value:
//
//     The radius of the cone at the given height.
//
//******************************************************************************
float ConeRadius(float y, float fLength, LPVOID pvContext) {

    return (fLength - y) / fLength * 1.0f;
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

    CMisorbit*   pMisorbit;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMisorbit = new CMisorbit();
    if (!pMisorbit) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMisorbit->Create(pDisplay)) {
        pMisorbit->Release();
        return FALSE;
    }

    bRet = pMisorbit->Exhibit(pnExitCode);

    // Clean up the scene
    pMisorbit->Release();

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
// CMisorbit
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMisorbit
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
CMisorbit::CMisorbit() {

    memset(&m_shSphere, 0, sizeof(SHAPE));
    memset(&m_shCylinder, 0, sizeof(SHAPE));
    memset(&m_shCone, 0, sizeof(SHAPE));
    m_fAngleOfRotation = 0;
    m_bSwitch = FALSE;
    m_bFade = TRUE;

    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

#ifdef UNDER_XBOX
    _stprintf(m_szCPUUsage, TEXT("CPU usage = %u%%"), 0);
    m_fInvCPUUsage = 1.0f;

    m_prUsageBar[0] = TLVERTEX(D3DXVECTOR3(0.0f, 480.0f, 0.00005f), 99000.0f, RGBA_MAKE(128, 128, 128, 255), 0, 0.28f, 0.75f);
    m_prUsageBar[1] = TLVERTEX(D3DXVECTOR3(0.0f, 440.0f, 0.00005f), 99000.0f, RGBA_MAKE(128, 128, 128, 255), 0, 0.28f, 0.27f);
    m_prUsageBar[2] = TLVERTEX(D3DXVECTOR3(640.0f, 440.0f, 0.00005f), 99000.0f, RGBA_MAKE(128, 128, 128, 255), 0, 0.72f, 0.27f);
    m_prUsageBar[3] = TLVERTEX(D3DXVECTOR3(640.0f, 480.0f, 0.00005f), 99000.0f, RGBA_MAKE(128, 128, 128, 255), 0, 0.72f, 0.75f);
#endif
}

//******************************************************************************
//
// Method:
//
//     ~CMisorbit
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
CMisorbit::~CMisorbit() {

    // Release all created objects
    ReleaseRevolution(&(m_shSphere.rv));
    ReleaseRevolution(&(m_shCylinder.rv));
    ReleaseRevolution(&(m_shCone.rv));
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
BOOL CMisorbit::Create(CDisplay* pDisplay) {

    // Create the shapes
    if (!CreateRevolution(&(m_shSphere.rv), 3.0f, 32, 32, SphereRadius, NULL, 0.001f, TRUE, 2.0f, 1.0f)) {
        return FALSE;
    }

    if (!CreateRevolution(&(m_shCylinder.rv), 3.0f, 2, 64, CylinderRadius, NULL, 0.001f, TRUE, 5.0f, 3.0f)) {
        return FALSE;
    }

    if (!CreateRevolution(&(m_shCone.rv), 3.0f, 2, 64, ConeRadius, NULL, 0.001f, TRUE, 10.0f, 9.0f)) {
        return FALSE;
    }

    // For the first half rotation the objects should appear correctly
    m_psh[0] = &m_shCone;
    m_psh[1] = &m_shSphere;
    m_psh[2] = &m_shCylinder;

    D3DXMatrixIdentity(&m_mRotation);
    D3DXMatrixIdentity(&m_shSphere.mWorld);

    InitMatrix(&(m_shCylinder.mWorld),
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
        (float)( 3.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

    InitMatrix(&(m_shCone.mWorld),
        (float)( 1.0), (float)( 0.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 1.0), (float)( 0.0),
        (float)(-3.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

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
int CMisorbit::Exhibit(int *pnExitCode) {

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
BOOL CMisorbit::Setup() {

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
BOOL CMisorbit::Initialize() {

    if (!CScene::Initialize()) {
        return FALSE;
    }

    SetMaterial(m_pDevice, RGB_MAKE(128, 128, 128), 
                                RGB_MAKE(128, 128, 128), RGB_MAKE(196, 196, 196), 
                                RGB_MAKE(0, 0, 0), 40.0f);

    // Create the textures
    m_shSphere.pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("tex1.bmp"));
    m_shCylinder.pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("tex1.bmp"));
    m_shCone.pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("tex1.bmp"));

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

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
void CMisorbit::Efface() {

    ReleaseTexture(m_shSphere.pd3dt);
    ReleaseTexture(m_shCylinder.pd3dt);
    ReleaseTexture(m_shCone.pd3dt);
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
void CMisorbit::Update() {

    D3DXMATRIX mDelta;
    float      fTheta;
#ifdef UNDER_XBOX
    BYTE       CPUUsage;
    
    if (!(((UINT)m_fFrame - 1) % 480)) {
        CPUUsage = CalculateCpuUsage();
        m_fInvCPUUsage = 1.0f - (float)CPUUsage / 100.0f;
        m_prUsageBar[2].vPosition.x = 640.0f * (1.0f - m_fInvCPUUsage);
        m_prUsageBar[3].vPosition.x = m_prUsageBar[2].vPosition.x;
        _stprintf(m_szCPUUsage, TEXT("CPU usage = %u%%"), CPUUsage);
    }

    fTheta = (M_PI / 40.0f) * m_fFrameDelta * m_fTimeDilation * (0.25f + m_fInvCPUUsage * 0.75f);

#else

    fTheta = (M_PI / 40.0f) * m_fFrameDelta * m_fTimeDilation;

#endif // UNDER_XBOX

    if ((m_bSwitch && m_fAngleOfRotation > M_PI) || m_fAngleOfRotation > M_2PI) {

        switch (rand()%3) {
            case 0:
                m_psh[0] = &m_shCylinder;
                m_psh[1] = &m_shSphere;
                m_psh[2] = &m_shCone;
                break;
            case 1:
                m_psh[0] = &m_shSphere;
                m_psh[1] = &m_shCylinder;
                m_psh[2] = &m_shCone;
                break;
            case 2:
                m_psh[0] = &m_shCone;
                m_psh[1] = &m_shSphere;
                m_psh[2] = &m_shCylinder;
                break;
        }

        if (m_fAngleOfRotation > M_2PI) {
            m_fAngleOfRotation -= M_2PI;
            D3DXMatrixIdentity(&m_mRotation);
            m_bSwitch = TRUE;
        }
        else {
            m_bSwitch = FALSE;
        }
    }

    InitMatrix(&mDelta,
        (float)( cos(fTheta)), (float)( 0.0), (float)(-sin(fTheta)), (float)( 0.0),
        (float)( 0.0), (float)( 1.0), (float)( 0.0), (float)( 0.0),
        (float)( sin(fTheta)), (float)( 0.0), (float)( cos(fTheta)), (float)( 0.0),
        (float)( 0.0), (float)( 0.0), (float)( 0.0), (float)( 1.0)
    );

    D3DXMatrixMultiply(&m_mRotation, &mDelta, &m_mRotation);
    m_fAngleOfRotation += fTheta;

    if ((UINT)m_fFrame % 2) {
        m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }
    else {
        m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
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
BOOL CMisorbit::Render() {

    D3DXMATRIX  mWorld;
    UINT        i;
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

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    for (i = 0; i < 3; i++) {

        D3DXMatrixMultiply(&mWorld, &(m_psh[i]->mWorld), &m_mRotation);

        m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

        m_pDevice->SetTexture(0, m_psh[i]->pd3dt);

        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
                0, m_psh[i]->rv.uCenterVertices, m_psh[i]->rv.uCenterIndices / 3,
                m_psh[i]->rv.pwCenter, D3DFMT_INDEX16, m_psh[i]->rv.prCenter, 
                sizeof(VERTEX));
    }

    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prUsageBar, sizeof(TLVERTEX));

    xTextOut(225.0f, 450.0f, 12.0f, 12.0f * CHAR_ASPECT, RGBA_MAKE(255, 0, 255, 255), m_szCPUUsage, _tcslen(m_szCPUUsage));

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
void CMisorbit::ProcessInput() {

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
BOOL CMisorbit::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 1.5f, -11.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 1.5f, 0.0f);
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
BOOL CMisorbit::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
