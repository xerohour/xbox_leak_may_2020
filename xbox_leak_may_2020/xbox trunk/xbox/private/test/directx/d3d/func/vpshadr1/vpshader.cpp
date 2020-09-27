/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vpshader.cpp

Author:

    Matt Bronder

Description:

    Basic vertex and pixel shader test.

*******************************************************************************/

#include "d3dlocus.h"
#include "vpshader.h"

#define WINX_D3DVSDE_POSITION        0
#define WINX_D3DVSDE_BLENDWEIGHT     1
#define WINX_D3DVSDE_BLENDINDICES    2
#define WINX_D3DVSDE_NORMAL          3
#define WINX_D3DVSDE_PSIZE           4
#define WINX_D3DVSDE_DIFFUSE         5
#define WINX_D3DVSDE_SPECULAR        6
#define WINX_D3DVSDE_TEXCOORD0       7
#define WINX_D3DVSDE_TEXCOORD1       8
#define WINX_D3DVSDE_TEXCOORD2       9
#define WINX_D3DVSDE_TEXCOORD3       10
#define WINX_D3DVSDE_TEXCOORD4       11
#define WINX_D3DVSDE_TEXCOORD5       12
#define WINX_D3DVSDE_TEXCOORD6       13
#define WINX_D3DVSDE_TEXCOORD7       14
#define WINX_D3DVSDE_POSITION2       15
#define WINX_D3DVSDE_NORMAL2         16

//******************************************************************************
CVertexBuffer8* CreateComponentVB(CDevice8* pDevice, LPVOID pvComponent, 
                                  UINT uNumComponents, UINT uCompSize, 
                                  UINT uCompStride);

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

    CVPShader*  pVPShader;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pVPShader = new CVPShader();
    if (!pVPShader) {
        return FALSE;
    }

    // Initialize the scene
    if (!pVPShader->Create(pDisplay)) {
        pVPShader->Release();
        return FALSE;
    }

    bRet = pVPShader->Exhibit(pnExitCode);

    // Clean up the scene
    pVPShader->Release();

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

    if (pd3dcaps->VertexShaderVersion < 0x100) {
        return FALSE;
    }

    if (pd3dcaps->PixelShaderVersion < 0x100) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
// CVPShader
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CVPShader
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
CVPShader::CVPShader() {

    m_pd3drSphere = NULL;
    m_pd3drSPosX = NULL;
    m_pd3drSPosY = NULL;
    m_pd3drSPosZ = NULL;
    m_pd3drSNorX = NULL;
    m_pd3drSNorY = NULL;
    m_pd3drSNorZ = NULL;
    m_pd3drSTxcU = NULL;
    m_pd3drSTxcV = NULL;
    m_pd3diSphere = NULL;
    m_prSphere = NULL;
    m_pwSphere = NULL;
    m_dwPShader1 = INVALID_SHADER_HANDLE;
    m_dwVShader1 = INVALID_SHADER_HANDLE;
}

//******************************************************************************
//
// Method:
//
//     ~CVPShader
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
CVPShader::~CVPShader() {

    ReleaseSphere(&m_prSphere, &m_pwSphere);
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
BOOL CVPShader::Create(CDisplay* pDisplay) {

    if (!CreateSphere(&m_prSphere, &m_dwSphereVertices, &m_pwSphere,
        &m_dwSphereIndices, 10.0f, 32, 32))
    {
        return FALSE;
    }

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
int CVPShader::Exhibit(int *pnExitCode) {

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
BOOL CVPShader::Setup() {

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
BOOL CVPShader::Initialize() {

    D3DXVECTOR4     vPower, vDBase, vDOffset, vSBase;
    D3DXVECTOR3     vLightDir;
    D3DCOLORVALUE   dcvAmbient;
    HRESULT         hr;
    DWORD           dwSDecl1[] = {
                        D3DVSD_STREAM(1),
                        D3DVSD_REG(WINX_D3DVSDE_POSITION, D3DVSDT_FLOAT3),
                        D3DVSD_REG(WINX_D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
                        D3DVSD_REG(WINX_D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
                        D3DVSD_END()
                    };
    DWORD           dwSDecl2[] = {
                        D3DVSD_STREAM(0),
                        D3DVSD_REG(1,  D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(1),
                        D3DVSD_REG(3,  D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(2),
                        D3DVSD_REG(5,  D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(3),
                        D3DVSD_REG(7,  D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(4),
                        D3DVSD_REG(9,  D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(5),
                        D3DVSD_REG(11, D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(6),
                        D3DVSD_REG(13, D3DVSDT_FLOAT1),
                        D3DVSD_STREAM(7),
                        D3DVSD_REG(15, D3DVSDT_FLOAT1),
                    };

    if (!CScene::Initialize()) {
        return FALSE;
    }

    // Create a directional light
    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
    m_light.Diffuse.r = 1.0f;
    m_light.Diffuse.g = 1.0f;
    m_light.Diffuse.b = 1.0f;
    m_light.Diffuse.a = 1.0f;
    m_light.Specular.r = 1.0f;
    m_light.Specular.g = 1.0f;
    m_light.Specular.b = 1.0f;
    m_light.Specular.a = 1.0f;
    m_light.Ambient.r = 1.0f;
    m_light.Ambient.g = 1.0f;
    m_light.Ambient.b = 1.0f;
    m_light.Ambient.a = 1.0f;
    m_light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

    hr = m_pDevice->SetLight(0, &m_light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

//    hr = m_pDevice->GetMaterial(&m_material);
//    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetMaterial"))) {
//        return FALSE;
//    }

    SetMaterial(&m_material, RGBA_MAKE(255, 255, 255, 230), RGBA_MAKE(0, 0, 0, 230), RGBA_MAKE(255, 255, 255, 255), 0, 40.0f);
//    m_pDevice->SetMaterial(&m_material);

    // Create the textures
    m_pd3dtSphere = (CTexture8*)CreateTexture(m_pDevice, TEXT("edge.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.9f);
    if (!m_pd3dtSphere) {
        return FALSE;
    }

    m_pd3dtBack = (CTexture8*)CreateTexture(m_pDevice, TEXT("brick.bmp"));
    if (!m_pd3dtBack) {
        return FALSE;
    }

    // Create the vertex buffers
    m_pd3drSphere = CreateVertexBuffer(m_pDevice, (LPVOID)m_prSphere, m_dwSphereVertices * sizeof(VERTEX), 0);
    if (!m_pd3drSphere) {
        return FALSE;
    }
    m_pd3drSPosX = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vPosition.x, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSPosX) {
        return FALSE;
    }
    m_pd3drSPosY = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vPosition.y, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSPosY) {
        return FALSE;
    }
    m_pd3drSPosZ = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vPosition.z, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSPosZ) {
        return FALSE;
    }
    m_pd3drSNorX = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vNormal.x, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSNorX) {
        return FALSE;
    }
    m_pd3drSNorY = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vNormal.y, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSNorY) {
        return FALSE;
    }
    m_pd3drSNorZ = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].vNormal.z, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSNorZ) {
        return FALSE;
    }
    m_pd3drSTxcU = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].u0, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSTxcU) {
        return FALSE;
    }
    m_pd3drSTxcV = CreateComponentVB(m_pDevice, (LPVOID)&m_prSphere[0].v0, m_dwSphereVertices, sizeof(float), sizeof(VERTEX));
    if (!m_pd3drSTxcV) {
        return FALSE;
    }

    // Create the index buffer
    m_pd3diSphere = CreateIndexBuffer(m_pDevice, m_pwSphere, m_dwSphereIndices * sizeof(WORD));
    if (!m_pd3diSphere) {
        return FALSE;
    }

    // Create the vertex shaders
    m_dwVShader1 = CreateVertexShader(m_pDevice, dwSDecl1, TEXT("vshader1.vsh"));
    if (m_dwVShader1 == INVALID_SHADER_HANDLE) {
        return FALSE;
    }

    // Set the vertex shader constant registers
    D3DXVec3Normalize(&vLightDir, (D3DXVECTOR3*)&m_light.Direction);
    m_pDevice->SetVertexShaderConstant(4, &vLightDir, 1);

    vPower = D3DXVECTOR4(40.0f, 0.0f, 1.0f, 0.0f);
    m_pDevice->SetVertexShaderConstant(6, &vPower, 1);

    vDBase = D3DXVECTOR4(m_light.Diffuse.r * m_material.Diffuse.r, m_light.Diffuse.g * m_material.Diffuse.g, m_light.Diffuse.b * m_material.Diffuse.b, m_material.Diffuse.a);
    m_pDevice->SetVertexShaderConstant(7, &vDBase, 1);

    // Set the room ambient color
    dcvAmbient.r = 0.125f;
    dcvAmbient.g = 0.125f;
    dcvAmbient.b = 0.125f;
    dcvAmbient.a = 1.0f;
    vDOffset = D3DXVECTOR4((dcvAmbient.r + m_light.Ambient.r) * m_material.Ambient.r + m_material.Emissive.r,
                           (dcvAmbient.g + m_light.Ambient.g) * m_material.Ambient.g + m_material.Emissive.g,
                           (dcvAmbient.b + m_light.Ambient.b) * m_material.Ambient.b + m_material.Emissive.b,
                           0.0f);
    m_pDevice->SetVertexShaderConstant(8, &vDOffset, 1);

    vSBase = D3DXVECTOR4(m_light.Specular.r * m_material.Specular.r, m_light.Specular.g * m_material.Specular.g, m_light.Specular.b * m_material.Specular.b, m_material.Specular.a);
    m_pDevice->SetVertexShaderConstant(9, &vSBase, 1);

    // Create the pixel shader
    m_dwPShader1 = CreatePixelShader(m_pDevice, TEXT("pshader1.psh"));
    if (m_dwPShader1 == INVALID_SHADER_HANDLE) {
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
void CVPShader::Efface() {

    m_pDevice->SetVertexShader(D3DFVF_XYZ);
    m_pDevice->SetPixelShader(NULL);
    if (m_dwPShader1 != INVALID_SHADER_HANDLE) {
        ReleasePixelShader(m_pDevice, m_dwPShader1);
    }
    if (m_dwVShader1 != INVALID_SHADER_HANDLE) {
        ReleaseVertexShader(m_pDevice, m_dwVShader1);
    }
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
void CVPShader::Update() {

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
BOOL CVPShader::Render() {

    D3DXMATRIX  mTransform, mView, mProj;
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
        m_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, RGB_MAKE(0, 0, 0), 1.0f, 0);
    }

    // Begin the scene
    m_pDevice->BeginScene();

    m_pDevice->SetTexture(0, m_pd3dtBack);
    m_pDevice->SetVertexShader(FVF_TLVERTEX);
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    // Position the view using the default camera
//    m_pDisplay->SetView();

    m_pDevice->SetTexture(0, m_pd3dtSphere);

    CAMERA cam;
    m_pDisplay->GetCamera(&cam);
    SetView((LPD3DXMATRIX)&mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));
    SetPerspectiveProjection(&mProj, cam.fNearPlane, cam.fFarPlane, 
            cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    D3DXMatrixMultiply(&mTransform, &mView, &mProj);
    D3DXMatrixTranspose(&mTransform, &mTransform);

    m_pDevice->SetVertexShaderConstant(0, &mTransform, 4);
    m_pDevice->SetVertexShaderConstant(5, &cam.vPosition, 1);

    m_pDevice->SetVertexShader(m_dwVShader1);
    m_pDevice->SetPixelShader(m_dwPShader1);

    m_pDevice->SetStreamSource(1, m_pd3drSphere, sizeof(VERTEX));

    m_pDevice->SetIndices(m_pd3diSphere, 0);

    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, 0, m_dwSphereIndices / 3);
//    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_dwSphereVertices, m_dwSphereIndices / 3, m_pwSphere, D3DFMT_INDEX16, m_prSphere, sizeof(VERTEX));

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

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
void CVPShader::ProcessInput() {

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
BOOL CVPShader::InitView() {

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
BOOL CVPShader::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
CVertexBuffer8* CreateComponentVB(CDevice8* pDevice, LPVOID pvComponent, 
                                  UINT uNumComponents, UINT uCompSize, 
                                  UINT uCompStride)
{
    CVertexBuffer8* pd3dr;
    LPBYTE          pData;
    UINT            i;
    HRESULT         hr;

    pd3dr = CreateVertexBuffer(pDevice, NULL, uNumComponents * uCompSize, 0);
    if (pd3dr) {

        hr = pd3dr->Lock(0, uNumComponents * uCompSize, &pData, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
            pd3dr->Release();
            return NULL;
        }

        for (i = 0; i < uNumComponents; i++) {
            memcpy(pData + i * uCompSize, (LPBYTE)pvComponent + i * uCompStride, uCompSize);
        }

        hr = pd3dr->Unlock();
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
            pd3dr->Release();
            return NULL;
        }
    }

    return pd3dr;
}
