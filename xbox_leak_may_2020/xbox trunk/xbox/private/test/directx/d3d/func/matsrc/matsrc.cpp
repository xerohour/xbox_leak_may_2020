/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    matsrc.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "matsrc.h"

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

    CMaterialSource*    pMatSrc;
    BOOL                bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMatSrc = new CMaterialSource();
    if (!pMatSrc) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMatSrc->Create(pDisplay)) {
        pMatSrc->Release();
        return FALSE;
    }

    bRet = pMatSrc->Exhibit(pnExitCode);

    // Clean up the scene
    pMatSrc->Release();

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
// CMaterialSource
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMaterialSource
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
CMaterialSource::CMaterialSource() {

    m_vsh2SidedLighting = 0;
    m_bFade = FALSE;
}

//******************************************************************************
//
// Method:
//
//     ~CMaterialSource
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
CMaterialSource::~CMaterialSource() {
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
BOOL CMaterialSource::Prepare() {

    if (GetStartupContext() & TSTART_HARNESS) {
        SetFrameDuration(320.0f);
    }

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
BOOL CMaterialSource::Initialize() {

    HRESULT  hr;

    // Initialize the vertices
    m_prQuad[0] = MSVERTEX(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prQuad[1] = MSVERTEX(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prQuad[2] = MSVERTEX(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prQuad[3] = MSVERTEX(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);

    // Create a directional light
    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
/*
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
*/
    m_light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    hr = m_pDevice->SetLight(0, &m_light);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetLight"))) {
        return FALSE;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"))) {
        return FALSE;
    }

    // Create a white light
    memset(&m_lightWhite, 0, sizeof(D3DLIGHT8));
    m_lightWhite.Type = D3DLIGHT_DIRECTIONAL;
    m_lightWhite.Diffuse.r = 1.0f;
    m_lightWhite.Diffuse.g = 1.0f;
    m_lightWhite.Diffuse.b = 1.0f;
    m_lightWhite.Diffuse.a = 1.0f;
    m_lightWhite.Specular.r = 1.0f;
    m_lightWhite.Specular.g = 1.0f;
    m_lightWhite.Specular.b = 1.0f;
    m_lightWhite.Specular.a = 1.0f;
    m_lightWhite.Ambient.r = 1.0f;
    m_lightWhite.Ambient.g = 1.0f;
    m_lightWhite.Ambient.b = 1.0f;
    m_lightWhite.Ambient.a = 1.0f;
    m_lightWhite.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    // Set a material
    memset(&m_material, 0, sizeof(D3DMATERIAL8));
    m_material.Power = 1.0f;

    m_pDevice->SetMaterial(&m_material);

    // Set a white material
    SetMaterial(&m_materialWhite, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1.0f);

    // Set a black material
    SetMaterial(&m_materialBlack, 0, 0, 0, 0, 1.0f);

    m_pDevice->SetRenderState(D3DRS_AMBIENT, 0);
    m_pDevice->SetRenderState(D3DRS_BACKAMBIENT, 0);

    m_cWhite = RGBA_MAKE(255, 255, 255, 255);
    m_dcvWhite.r = 1.0f;
    m_dcvWhite.g = 1.0f;
    m_dcvWhite.b = 1.0f;
    m_dcvWhite.a = 1.0f;

    DWORD Declaration[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
        D3DVSD_REG( D3DVSDE_NORMAL, D3DVSDT_FLOAT3 ),
        D3DVSD_REG( D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR ),
        D3DVSD_REG( D3DVSDE_SPECULAR, D3DVSDT_D3DCOLOR ),
        D3DVSD_REG( D3DVSDE_BACKDIFFUSE, D3DVSDT_D3DCOLOR ),
        D3DVSD_REG( D3DVSDE_BACKSPECULAR, D3DVSDT_D3DCOLOR ),
        D3DVSD_REG( D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
        D3DVSD_END()
    };

    hr = m_pDevice->CreateVertexShader(Declaration, NULL, &m_vsh2SidedLighting, 0);
    if (FAILED(hr)) {
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
void CMaterialSource::Efface() {

    CScene::Efface();

    if (m_vsh2SidedLighting) {
        m_pDevice->SetVertexShader(D3DFVF_XYZ);
        m_pDevice->DeleteVertexShader(m_vsh2SidedLighting);
        m_vsh2SidedLighting = 0;
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
void CMaterialSource::Update() {

    D3DCOLOR                cAmbient = 0, cWhite = 0xFFFFFFFF;
    BOOL                    bColorVertex = TRUE;
    D3DMATERIALCOLORSOURCE  mcsDiffuse = D3DMCS_MATERIAL, 
                            mcsAmbient = D3DMCS_MATERIAL, 
                            mcsSpecular = D3DMCS_MATERIAL, 
                            mcsEmissive = D3DMCS_MATERIAL;
    D3DCOLOR                cDiffuse = 0, cSpecular = 0;
    UINT                    i, uFrame = (UINT)m_fFrame;
    BOOL                    bBack;
    float                   fDir = 1.0f;

    memset(&m_light, 0, sizeof(D3DLIGHT8));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
    m_light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    memset(&m_material, 0, sizeof(D3DMATERIAL8));
    m_material.Power = 1.0f;

    switch (((uFrame - 1) % 320) / 80) {

        case 0:
            m_cColor = RGBA_MAKE(255, 0, 0, 255);
            m_dcvColor.r = 1.0f;
            m_dcvColor.g = 0.0f;
            m_dcvColor.b = 0.0f;
            m_dcvColor.a = 0.0f;
            break;
        case 1:
            m_cColor = RGBA_MAKE(0, 255, 0, 255);
            m_dcvColor.r = 0.0f;
            m_dcvColor.g = 1.0f;
            m_dcvColor.b = 0.0f;
            m_dcvColor.a = 0.0f;
            break;
        case 2:
            m_cColor = RGBA_MAKE(0, 0, 255, 255);
            m_dcvColor.r = 0.0f;
            m_dcvColor.g = 0.0f;
            m_dcvColor.b = 1.0f;
            m_dcvColor.a = 0.0f;
            break;
        case 3:
            m_cColor = RGBA_MAKE(255, 255, 255, 255);
            m_dcvColor.r = 1.0f;
            m_dcvColor.g = 1.0f;
            m_dcvColor.b = 1.0f;
            m_dcvColor.a = 1.0f;
            break;
    }

    switch ((uFrame - 1) % 40) {

        case 0:
            m_light = m_lightWhite;
            m_material.Diffuse = m_dcvColor;
            break;

        case 1:
            m_light = m_lightWhite;
            cDiffuse = m_cColor;
            mcsDiffuse = D3DMCS_COLOR1;
            break;

        case 2:
            m_light = m_lightWhite;
            cSpecular = m_cColor;
            mcsDiffuse = D3DMCS_COLOR2;
            break;

        case 3:
            m_light = m_lightWhite;
            m_material.Diffuse = m_dcvColor;
            mcsDiffuse = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 4:
            m_light = m_lightWhite;
            m_material.Ambient = m_dcvColor;
            break;

        case 5:
            m_light = m_lightWhite;
            cDiffuse = m_cColor;
            mcsAmbient = D3DMCS_COLOR1;
            break;

        case 6:
            m_light = m_lightWhite;
            cSpecular = m_cColor;
            mcsAmbient = D3DMCS_COLOR2;
            break;

        case 7:
            m_light = m_lightWhite;
            m_material.Ambient = m_dcvColor;
            mcsAmbient = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 8:
            cAmbient = cWhite;
            m_material.Ambient = m_dcvColor;
            break;

        case 9:
            cAmbient = cWhite;
            cDiffuse = m_cColor;
            mcsAmbient = D3DMCS_COLOR1;
            break;

        case 10:
            cAmbient = cWhite;
            cSpecular = m_cColor;
            mcsAmbient = D3DMCS_COLOR2;
            break;

        case 11:
            cAmbient = cWhite;
            m_material.Ambient = m_dcvColor;
            mcsAmbient = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 12:
            m_light = m_lightWhite;
            m_material.Specular = m_dcvColor;
            break;

        case 13:
            m_light = m_lightWhite;
            cDiffuse = m_cColor;
            mcsSpecular = D3DMCS_COLOR1;
            break;

        case 14:
            m_light = m_lightWhite;
            cSpecular = m_cColor;
            mcsSpecular = D3DMCS_COLOR2;
            break;

        case 15:
            m_light = m_lightWhite;
            m_material.Specular = m_dcvColor;
            mcsSpecular = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 16:
            m_light = m_lightWhite;
            cAmbient = cWhite;
            m_material.Emissive = m_dcvColor;
            break;

        case 17:
            m_light = m_lightWhite;
            cAmbient = cWhite;
            cDiffuse = m_cColor;
            mcsEmissive = D3DMCS_COLOR1;
            break;

        case 18:
            m_light = m_lightWhite;
            cAmbient = cWhite;
            cSpecular = m_cColor;
            mcsEmissive = D3DMCS_COLOR2;
            break;

        case 19:
            m_light = m_lightWhite;
            cAmbient = cWhite;
            m_material.Emissive = m_dcvColor;
            mcsEmissive = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 20:
            m_light.Diffuse = m_dcvColor;
            m_material.Diffuse = m_dcvColor;
            break;

        case 21:
            m_light.Diffuse = m_dcvColor;
            cDiffuse = m_cColor;
            mcsDiffuse = D3DMCS_COLOR1;
            break;

        case 22:
            m_light.Diffuse = m_dcvColor;
            cSpecular = m_cColor;
            mcsDiffuse = D3DMCS_COLOR2;
            break;

        case 23:
            m_light.Diffuse = m_dcvColor;
            m_material.Diffuse = m_dcvColor;
            mcsDiffuse = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 24:
            m_light.Ambient = m_dcvColor;
            m_material.Ambient = m_dcvColor;
            break;

        case 25:
            m_light.Ambient = m_dcvColor;
            cDiffuse = m_cColor;
            mcsAmbient = D3DMCS_COLOR1;
            break;

        case 26:
            m_light.Ambient = m_dcvColor;
            cSpecular = m_cColor;
            mcsAmbient = D3DMCS_COLOR2;
            break;

        case 27:
            m_light.Ambient = m_dcvColor;
            m_material.Ambient = m_dcvColor;
            mcsAmbient = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 28:
            cAmbient = m_cColor;
            m_material.Ambient = m_dcvColor;
            break;

        case 29:
            cAmbient = m_cColor;
            cDiffuse = m_cColor;
            mcsAmbient = D3DMCS_COLOR1;
            break;

        case 30:
            cAmbient = m_cColor;
            cSpecular = m_cColor;
            mcsAmbient = D3DMCS_COLOR2;
            break;

        case 31:
            cAmbient = m_cColor;
            m_material.Ambient = m_dcvColor;
            mcsAmbient = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 32:
            m_light.Specular = m_dcvColor;
            m_material.Specular = m_dcvColor;
            break;

        case 33:
            m_light.Specular = m_dcvColor;
            cDiffuse = m_cColor;
            mcsSpecular = D3DMCS_COLOR1;
            break;

        case 34:
            m_light.Specular = m_dcvColor;
            cSpecular = m_cColor;
            mcsSpecular = D3DMCS_COLOR2;
            break;

        case 35:
            m_light.Specular = m_dcvColor;
            m_material.Specular = m_dcvColor;
            mcsSpecular = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;

        case 36:
            m_material.Emissive = m_dcvColor;
            break;

        case 37:
            cDiffuse = m_cColor;
            mcsEmissive = D3DMCS_COLOR1;
            break;

        case 38:
            cSpecular = m_cColor;
            mcsEmissive = D3DMCS_COLOR2;
            break;

        case 39:
            m_material.Emissive = m_dcvColor;
            mcsEmissive = D3DMCS_COLOR1;
            bColorVertex = FALSE;
            break;
    }

#ifdef UNDER_XBOX
    bBack = (((uFrame - 1) % 80) > 39);
#else
    bBack = FALSE;
#endif

    if (bBack) {
        fDir = -fDir;
    }

    for (i = 0; i < 4; i++) {
        m_prQuad[i].cDiffuse = cDiffuse;
        m_prQuad[i].cSpecular = cSpecular;
        m_prQuad[i].vNormal.z = -1.0f;//-fDir;
    }

    m_light.Direction.z = fDir;

    m_camInitial.vPosition.z = -50.0f * fDir;

    m_pDevice->SetLight(0, &m_light);

    if (!bBack) {
        m_pDevice->SetMaterial(&m_material);
#ifdef UNDER_XBOX
        m_pDevice->SetBackMaterial(&m_materialBlack);
        m_pDevice->SetRenderState(D3DRS_BACKAMBIENT, 0);
#endif // UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_AMBIENT, cAmbient);
        m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, mcsDiffuse);
        m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, mcsAmbient);
        m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, mcsSpecular);
        m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, mcsEmissive);
#ifdef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_BACKDIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_BACKSPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_BACKEMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
#endif // UNDER_XBOX
    }
    else {
        m_pDevice->SetMaterial(&m_materialBlack);
#ifdef UNDER_XBOX
        m_pDevice->SetBackMaterial(&m_material);
        m_pDevice->SetRenderState(D3DRS_BACKAMBIENT, cAmbient);
#endif // UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_AMBIENT, 0);
        m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
        m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
#ifdef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_BACKDIFFUSEMATERIALSOURCE, mcsDiffuse);
        m_pDevice->SetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE, mcsAmbient);
        m_pDevice->SetRenderState(D3DRS_BACKSPECULARMATERIALSOURCE, mcsSpecular);
        m_pDevice->SetRenderState(D3DRS_BACKEMISSIVEMATERIALSOURCE, mcsEmissive);
#endif // UNDER_XBOX
    }

    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, bColorVertex);
#ifdef UNDER_XBOX
    m_pDevice->SetRenderState(D3DRS_TWOSIDEDLIGHTING, TRUE);
#endif // UNDER_XBOX
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pDisplay->EnableFrameVerification(!bBack);
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
BOOL CMaterialSource::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

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
    m_pDisplay->SetView(&m_camInitial);

    m_pDevice->SetLight(0, &m_light);
    m_pDevice->SetMaterial(&m_material);

    m_pDevice->SetVertexShader(m_vsh2SidedLighting);
//    m_pDevice->SetVertexShader(FVF_MSVERTEX);

    // Draw the quad
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, 
                               m_prQuad, sizeof(MSVERTEX));

    Verify();

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
BOOL CMaterialSource::Verify() {

    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;
    D3DCOLOR cColor;
    DWORD dwColor;
    float r, g, b;

    m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);

    pd3ds->GetDesc(&d3dsd);

    pd3ds->LockRect(&d3dlr, NULL, D3DLOCK_TILED);

    if (FormatToBitDepth(d3dsd.Format) == 16) {
        dwColor = *((LPWORD)((LPBYTE)d3dlr.pBits + d3dlr.Pitch * (d3dsd.Height / 2)) + (d3dsd.Width / 2));
    }
    else {
        dwColor = *((LPDWORD)((LPBYTE)d3dlr.pBits + d3dlr.Pitch * (d3dsd.Height / 2)) + (d3dsd.Width / 2));
    }

    switch (d3dsd.Format) {
#ifdef UNDER_XBOX
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_A8R8G8B8:
#else
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
#endif // !UNDER_XBOX
            cColor = dwColor | 0xFF000000;
            break;
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A1R5G5B5:
        case D3DFMT_LIN_X1R5G5B5:
#else
        case D3DFMT_A1R5G5B5:
        case D3DFMT_X1R5G5B5:
#endif // !UNDER_XBOX
            cColor = D3DCOLOR_ARGB(255, (dwColor & 0x7C00) >> 7, (dwColor & 0x03E0) >> 2, (dwColor & 0x001F) << 3);
            break;
#ifdef UNDER_XBOX
        case D3DFMT_LIN_R5G6B5:
#else
        case D3DFMT_R5G6B5:
#endif // !UNDER_XBOX
            cColor = D3DCOLOR_ARGB(255, (dwColor & 0xF800) >> 8, (dwColor & 0x07E0) >> 3, (dwColor & 0x001F) << 3);
            break;
    }

    pd3ds->UnlockRect();

    pd3ds->Release();

    r = FABS((float)RGBA_GETRED(cColor) - (float)RGBA_GETRED(m_cColor));
    g = FABS((float)RGBA_GETGREEN(cColor) - (float)RGBA_GETGREEN(m_cColor));
    b = FABS((float)RGBA_GETBLUE(cColor) - (float)RGBA_GETBLUE(m_cColor));

    if (r > 16.0f || g > 16.0f || b > 16.0f) {
        Log(LOG_FAIL, TEXT("Material color incorrectly rendered for test case %d (%s lighting) - color: 0x%8X, expected: 0x%8X"), ((UINT)m_fFrame - 1) % 40, ((((UINT)m_fFrame - 1) % 80) > 39) ? TEXT("back") : TEXT("front"), cColor, m_cColor);
        return FALSE;
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
void CMaterialSource::ProcessInput() {

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
BOOL CMaterialSource::InitView() {

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
BOOL CMaterialSource::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
