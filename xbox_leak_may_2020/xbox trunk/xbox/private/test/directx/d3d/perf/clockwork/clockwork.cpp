/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    clockwork.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "cogs.h"
#include "clockwork.h"

//******************************************************************************
BOOL CreateSphereV(float fRadius, PVERTEX* pprVertices, LPDWORD pdwNumVertices,
                  LPWORD* ppwIndices, LPDWORD pdwNumIndices) {

    PVERTEX     prVertices;
    PVERTEX     pr;
    DWORD       dwNumVertices;
    LPWORD      pwIndices;
    DWORD       dwNumIndices;
    UINT        uIndex = 0;
    UINT        uStepsU = SPHERE_U, uStepsV = SPHERE_V;
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

    pwIndices = new WORD[dwNumIndices];
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

            pr->vNormal.x = -(float)(sin(fTX));// * fSinY;
            pr->vNormal.y = -fCosY;
            pr->vNormal.z = (float)(cos(fTX));// * fSinY;
            pr->vPosition.x = -pr->vNormal.x * fRadius;
            pr->vPosition.y = -pr->vNormal.y * fRadius;
            pr->vPosition.z = -pr->vNormal.z * fRadius;
            pr->u0 = fX * 4.0f;
            pr->v0 = fY * 4.0f;
        }
    }

    for (j = 0; j < uStepsU; j++) {

        for (i = 0; i < uStepsV - 1; i++) {

            pwIndices[uIndex++] = i * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j + 1;
            pwIndices[uIndex++] = i * (uStepsU + 1) + j + 1;
            pwIndices[uIndex++] = i * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j;
            pwIndices[uIndex++] = (i + 1) * (uStepsU + 1) + j + 1;
        }
    }

    *pprVertices = prVertices;
    *pdwNumVertices = dwNumVertices;
    *ppwIndices = pwIndices;
    *pdwNumIndices = dwNumIndices;

    return TRUE;
}

//******************************************************************************
void ReleaseSphereV(PVERTEX* ppr, LPWORD* ppw) {

    if (ppr && *ppr) {
        MemFree32(*ppr);
    }
    if (ppw && *ppw) {
        delete [] *ppw;
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
BOOL ExhibitScene(CDisplay* pDisplay, int* pnExitCode) {

    CStateChangePerf* pStateChangePerf;
    BOOL              bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pStateChangePerf = new CStateChangePerf();
    if (!pStateChangePerf) {
        return FALSE;
    }

    // Initialize the scene
    if (!pStateChangePerf->Create(pDisplay)) {
        pStateChangePerf->Release();
        return FALSE;
    }

    bRet = pStateChangePerf->Exhibit(pnExitCode);

    // Clean up the scene
    pStateChangePerf->Release();

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
BOOL ValidateDisplay(LPDIRECT3D8 pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CStateChangePerf
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CStateChangePerf
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
CStateChangePerf::CStateChangePerf() {

    m_pClockwork = NULL;
    m_fStartTime = 0.0f;
    m_uOrbitFrame = 0;
    m_bProgShader = FALSE;
    m_fOrbitAngle = 0.0f;
    m_dwVShader = INVALID_SHADER_HANDLE;
    m_dwPShader = INVALID_SHADER_HANDLE;
    m_pd3dtBackground = NULL;
    m_bShowFPS = TRUE;
}

//******************************************************************************
//
// Method:
//
//     ~CStateChangePerf
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
CStateChangePerf::~CStateChangePerf() {
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
BOOL CStateChangePerf::Create(CDisplay* pDisplay) {

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
int CStateChangePerf::Exhibit(int *pnExitCode) {

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
BOOL CStateChangePerf::Prepare() {

    LARGE_INTEGER   qwCounter;
    DWORD           dwSeed;

    dwSeed = GetTickCount();
    dwSeed = 2993;
//    dwSeed = 1011451159;
//    dwSeed = 1016517294;
//    dwSeed = 1021200919;

    srand(dwSeed);
    DebugString(TEXT("Seeded the pseudo-random number generator with %d"), dwSeed);

    m_mWorld = m_mIdentity;

    QueryPerformanceFrequency(&qwCounter);
    m_fTimeFreq = 1.0f / (float)qwCounter.QuadPart;

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
BOOL CStateChangePerf::Setup() {

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
BOOL CStateChangePerf::Initialize() {

    DWORD   dwSDecl[] = {
                D3DVSD_STREAM(0),
                D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                D3DVSD_END()
            };

    SetPerspectiveProjection(&m_mProj, m_cam.fNearPlane, m_cam.fFarPlane, 
            m_cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    SetDirectionalLight(&m_light, D3DXVECTOR3(0.0f, -1.0f, 1.0f));
    SetMaterial(&m_material, RGB_MAKE(255, 255, 255));

    m_pDevice->LightEnable(0, TRUE);

    // Initialize the vertices
    if (!CreateSphereV(BOUND_RADIUS + 8.0f, &m_prSphere, &m_dwSphereVertices, &m_pwSphere, &m_dwSphereIndices)) {
        return FALSE;
    }
    m_dwSpherePrimitives = m_dwSphereIndices / 3;

    m_pClockwork = new CClockwork();
    if (!m_pClockwork) {
        return FALSE;
    }

    if (!m_pClockwork->Create(m_pDevice, &m_cam, (float)m_pDisplay->GetHeight(), (float)m_pDisplay->GetWidth())) {
        return FALSE;
    }

    m_pd3dtBackground = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("backcogs.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, D3DPOOL_MANAGED, 256, 256);
    if (!m_pd3dtBackground) {
        return FALSE;
    }
#ifdef UNDER_XBOX
    m_pd3dtBackground->MoveResourceMemory(D3DMEM_VIDEO);
#endif // UNDER_XBOX

    m_pd3drSphere = CreateVertexBuffer(m_pDevice, m_prSphere, m_dwSphereVertices * sizeof(VERTEX), 0, FVF_VERTEX);
    if (!m_pd3drSphere) {
        return FALSE;
    }

    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }
/*
    m_dwVShader = CreateVertexShader(m_pDevice, dwSDecl, TEXT("ldir.vsh"));
    if (m_dwVShader == INVALID_SHADER_HANDLE) {
        return FALSE;
    }

    m_dwPShader = CreatePixelShader(m_pDevice, TEXT("texdfmod.psh"));
    if (m_dwPShader == INVALID_SHADER_HANDLE) {
        return FALSE;
    }
*/
    m_bFade = TRUE;

    memset(&m_fsStats, 0, sizeof(FRAMESTATS));

    // Add a change for the view transform
    m_fsStats.dwTransforms++;

    // Add a change for the sphere vertex shader
    m_fsStats.dwVShaders++;

    // Add a change for the sphere world transform
    m_fsStats.dwTransforms++;

    // Add a change for the background texture
    m_fsStats.dwTextures++;

    m_fsStats.dwMaterials++;

    m_fsStats.dwLights++;

    // Add a change for the sphere vertex buffer
    m_fsStats.dwVBuffers++;

    // Add a change for the sphere index buffer
    m_fsStats.dwIBuffers++;

    // Add mesh and polygon changes for the sphere
    m_fsStats.dwMeshes++;
    m_fsStats.dwPolygons += m_dwSpherePrimitives;

    // Get the frame stats from clockwork
    m_pClockwork->GetFrameStats(&m_fsStats);

    OutputString(TEXT("Frame statistics:"));
    OutputString(TEXT("    %d polygons"), m_fsStats.dwPolygons);
    OutputString(TEXT("    %d meshes"), m_fsStats.dwMeshes);
    OutputString(TEXT("    %d vertex buffer changes"), m_fsStats.dwVBuffers);
    OutputString(TEXT("    %d index buffer changes"), m_fsStats.dwIBuffers);
    OutputString(TEXT("    %d transform changes"), m_fsStats.dwTransforms);
    OutputString(TEXT("    %d texture changes"), m_fsStats.dwTextures);
    OutputString(TEXT("    %d material changes"), m_fsStats.dwMaterials);
    OutputString(TEXT("    %d light changes"), m_fsStats.dwLights);

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
void CStateChangePerf::Efface() {

    ReleaseVertexShader(m_pDevice, m_dwVShader);
    ReleasePixelShader(m_pDevice, m_dwPShader);
    ReleaseTexture(m_pd3dtBackground);
    m_pd3dtBackground = NULL;
    delete m_pClockwork;
    ReleaseSphereV(&m_prSphere, &m_pwSphere);

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
void CStateChangePerf::Update() {

    m_pClockwork->Update(m_fTimeDilation);

    m_fOrbitAngle += (M_2PI / (float)ORBIT_FRAMES * m_fTimeDilation);
    if (m_fOrbitAngle > M_2PI) {
        m_fOrbitAngle -= M_2PI;
    }

    m_cam.vPosition.x = (float)(sin(m_fOrbitAngle)) * VIEW_Z;
    m_cam.vPosition.z = (float)(cos(m_fOrbitAngle)) * VIEW_Z;
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
BOOL CStateChangePerf::Render() {

    D3DXMATRIX      mView;
    DWORD           dwClearFlags = D3DCLEAR_TARGET;
    HRESULT         hr;

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

    SetView(&mView, &m_cam.vPosition, &m_cam.vInterest, 
            &D3DXVECTOR3((float)sin(m_cam.fRoll), (float)cos(m_cam.fRoll), 
            0.0f));

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }

    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

m_bProgShader = FALSE;
    if (!m_bProgShader) {

        m_pDevice->SetTransform(D3DTS_VIEW, &mView);

        m_pDevice->SetVertexShader(FVF_VERTEX);
        m_pDevice->SetPixelShader(NULL);

        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

        m_pDevice->SetTexture(0, m_pd3dtBackground);

#ifndef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
#endif

        m_pDevice->SetRenderState(D3DRS_AMBIENT, (DWORD)RGBA_MAKE(255, 255, 255, 255));

        m_pDevice->SetMaterial(&m_material);
        m_pDevice->SetLight(0, &m_light);
        m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

        m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(VERTEX));
        m_pDevice->SetIndices(m_pd3diSphere, 0);

        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices,
                0, m_dwSpherePrimitives);

        m_pClockwork->Render();
    }
    else {

        D3DXVECTOR4 vPower, vDBase, vLightDir, vDOffset, vSBase;
        D3DXVECTOR3 vAmbient;
        D3DXMATRIX  mR1, mR2;

        m_pDevice->SetVertexShader(m_dwVShader);
        m_pDevice->SetPixelShader(m_dwPShader);

        m_pDevice->SetVertexShaderConstant(5, &m_cam.vPosition, 1);

        vPower = D3DXVECTOR4(m_material.Power, 0.0f, 1.0f, 0.0f);
        m_pDevice->SetVertexShaderConstant(6, &vPower, 1);

        vDBase = D3DXVECTOR4(m_light.Diffuse.r * m_material.Diffuse.r, m_light.Diffuse.g * m_material.Diffuse.g, m_light.Diffuse.b * m_material.Diffuse.b, m_material.Diffuse.a);
        m_pDevice->SetVertexShaderConstant(7, &vDBase, 1);

        D3DXVec3Normalize((D3DXVECTOR3*)&vLightDir, (D3DXVECTOR3*)&m_light.Direction);
        m_pDevice->SetVertexShaderConstant(4, &vLightDir, 1);

        vAmbient = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

        vDOffset = D3DXVECTOR4((vAmbient.x + m_light.Ambient.r) * m_material.Ambient.r + m_material.Emissive.r,
                               (vAmbient.y + m_light.Ambient.g) * m_material.Ambient.g + m_material.Emissive.g,
                               (vAmbient.z + m_light.Ambient.b) * m_material.Ambient.b + m_material.Emissive.b,
                               0.0f);
        m_pDevice->SetVertexShaderConstant(8, &vDOffset, 1);

        vSBase = D3DXVECTOR4(m_light.Specular.r * m_material.Specular.r, m_light.Specular.g * m_material.Specular.g, m_light.Specular.b * m_material.Specular.b, m_material.Specular.a);
        m_pDevice->SetVertexShaderConstant(9, &vSBase, 1);

        D3DXMatrixMultiply(&mR1, &m_mWorld, &mView);
        D3DXMatrixMultiply(&mR2, &mR1, &m_mProj);
        D3DXMatrixTranspose(&mR1, &mR2);

        m_pDevice->SetVertexShaderConstant(0, &mR1, 4);

        m_pDevice->SetTexture(0, m_pd3dtBackground);

#ifndef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
#endif

        m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(VERTEX));
        m_pDevice->SetIndices(m_pd3diSphere, 0);

        m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices,
                0, m_dwSpherePrimitives);

        m_pClockwork->Render(&mView);
    }

    // Display the adapter mode
//    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
//    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
//    if (m_pfnFade) {
//        (this->*m_pfnFade)();
//    }

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    if (++m_uOrbitFrame >= ORBIT_FRAMES) {

        LARGE_INTEGER qwCounter;
//        float         fEndTime;
        float         fAvgFrameTime;

        // Get the end time
//        QueryPerformanceCounter(&qwCounter);
//        fEndTime = (float)qwCounter.QuadPart * m_fTimeFreq;

        fAvgFrameTime = (m_fTime - m_fStartTime) / (float)ORBIT_FRAMES;
        m_fStartTime = m_fTime;
        OutputString(TEXT("%s vertex and pixel shaders:"), m_bProgShader ? TEXT("Programmable") : TEXT("Fixed-function"));
        OutputString(TEXT("    Clockwork took an average of %3.3f ms to render a frame"), fAvgFrameTime * 1000.0f);
        OutputString(TEXT("    (%d polygons per second)"), (UINT)(1.0f / fAvgFrameTime * (float)m_fsStats.dwPolygons));
        m_bProgShader = !m_bProgShader;
        m_uOrbitFrame = 0;

        // Get the start time
//        QueryPerformanceCounter(&qwCounter);
//        m_fStartTime = (float)qwCounter.QuadPart * m_fTimeFreq;
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
void CStateChangePerf::ProcessInput() {

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
BOOL CStateChangePerf::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, VIEW_Z);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);
    memcpy(&m_cam, &m_camInitial, sizeof(CAMERA));

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
BOOL CStateChangePerf::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
