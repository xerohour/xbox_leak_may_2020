/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    pslights.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "pslights.h"

D3DCOLOR ModulateColors(D3DCOLORVALUE* pdcv1, D3DCOLORVALUE* pdcv2);

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

    CPSLights*  pPSLights;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPSLights = new CPSLights();
    if (!pPSLights) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPSLights->Create(pDisplay)) {
        pPSLights->Release();
        return FALSE;
    }

    bRet = pPSLights->Exhibit(pnExitCode);

    // Clean up the scene
    pPSLights->Release();

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
//     D3DDISPLAYMODE* pd3ddm           - Display mode into which the device
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
// CPSLights
//******************************************************************************

CPerlinNoise1D CPSLights::PNSphereRotX(0.25f, 6, 0x53E6712A, INTERPOLATE_COSINE, TRUE, TRUE);
CPerlinNoise1D CPSLights::PNSphereRotY(0.25f, 6, 0x7423CC4D, INTERPOLATE_COSINE, TRUE, TRUE);
CPerlinNoise1D CPSLights::PNSphereRotZ(0.25f, 6, 0x0157B329, INTERPOLATE_COSINE, TRUE, TRUE);

//******************************************************************************
//
// Method:
//
//     CPSLights
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
CPSLights::CPSLights() {

    m_pd3dtBase = NULL;
    m_pd3dtBump = NULL;
    m_pd3dtcNormal = NULL;
    m_pd3dtLight = NULL;

    m_pd3drSphere = NULL;
    m_pwSphere = NULL;
    m_vSphereRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_uNumLights = 0;

    m_dwVShader1 = INVALID_SHADER_HANDLE;
    m_dwVShader2 = INVALID_SHADER_HANDLE;
    m_dwPShader1 = 0;
    m_dwPShader2 = 0;

    m_bCompleteLight = TRUE;
    m_bInitialShowFPS = m_bShowFPS;
    m_bShowFPS = TRUE;

    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

//******************************************************************************
//
// Method:
//
//     ~CPSLights
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
CPSLights::~CPSLights() {

    m_bShowFPS = m_bInitialShowFPS;
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
BOOL CPSLights::Prepare() {

    D3DPRESENT_PARAMETERS d3dpp;
    UINT                  i, j;

    D3DXMatrixIdentity(&m_mWorld);

    // Global scene ambient lighting
    m_dcvAmbient.r = 0.25f;
    m_dcvAmbient.g = 0.25f;
    m_dcvAmbient.b = 0.25f;
    m_dcvAmbient.a = 1.0f;

    // Set the sphere's material
    SetMaterial(&m_material, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 255.0f), 16.0f);

    // Initialize the lights
    for (i = 0; i < MAX_LIGHTS; i++) {
        for (j = 0; j < 3; j++) {
            m_Lights[i].PNDirection[j] = CPerlinNoise1D(0.25f, 6, rand(), INTERPOLATE_COSINE, FALSE, TRUE);
            m_Lights[i].PNDirection[j].SetRange(-1.0f, 1.0f);
        }
        m_Lights[i].PNDistance = CPerlinNoise1D(0.25f, 6, rand(), INTERPOLATE_COSINE, FALSE, TRUE);
        m_Lights[i].PNDistance.SetRange(11.0f, 20.0f);
    }

    // Add a light
    if (GetStartupContext() & TSTART_HARNESS) {
        for (i = 0; i < 15; i++) {
            AddLight();
        }
    }
    else {
        AddLight();
    }

    PNSphereRotX.SetRange(0.0f, M_PI / 1000.0f);
    PNSphereRotY.SetRange(0.0f, M_PI / 1000.0f);
    PNSphereRotZ.SetRange(0.0f, M_PI / 1000.0f);

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
BOOL CPSLights::Initialize() {

    UINT i;

    m_pd3dtBase = (CTexture8*)CreateTexture(m_pDevice, TEXT("base.bmp"), D3DFMT_A8R8G8B8, TTYPE_MIPMAP);
    if (!m_pd3dtBase) {
        return FALSE;
    }

    m_pd3dtBump = (CTexture8*)CreateTexture(m_pDevice, TEXT("bump.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP | TTYPE_MIPMAP);
    if (!m_pd3dtBump) {
        return FALSE;
    }

    m_pd3dtcNormal = (CCubeTexture8*)CreateNormalMap(m_pDevice/*, TTYPE_MIPMAP*/);
    if (!m_pd3dtcNormal) {
        return FALSE;
    }

    m_pd3dtLight = CreateLightTexture(64);
    if (!m_pd3dtLight) {
        return FALSE;
    }

    if (!CreateSphere(10.0f, 16, 12)) {
        return FALSE;
    }

    if (!CreateShaders()) {
        return FALSE;
    }

    for (i = 0; i < 2; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    }
    for (i = 2; i < 4; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT);
        m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT);
        m_pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    }

    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    m_pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(2.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, F2DW(1.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, F2DW(0.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, F2DW(0.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, F2DW(1.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    return TRUE;
}

//******************************************************************************
void CPSLights::AddLight() {

    D3DLIGHT8* plight = &m_Lights[m_uNumLights].light;
    D3DCOLORVALUE dcvAmbientEmissive;
    D3DXVECTOR3   vColor, vSpecular;

    if (m_uNumLights == MAX_LIGHTS) {
        return;
    }

    D3DXVec3Normalize(&vColor, &D3DXVECTOR3(FRND(1.0f), FRND(1.0f), FRND(1.0f)));
    D3DXVec3Normalize(&vSpecular, &D3DXVECTOR3(vColor.x + 0.2f, vColor.y + 0.2f, vColor.z + 0.2f));

    memset(plight, 0, sizeof(D3DLIGHT8));
    plight->Type = D3DLIGHT_POINT;
    plight->Diffuse.r = vColor.x;
    plight->Diffuse.g = vColor.y;
    plight->Diffuse.b = vColor.z;
    plight->Diffuse.a = 1.0f;//FRND(1.0f);
    plight->Specular.r = vSpecular.x;
    plight->Specular.g = vSpecular.y;
    plight->Specular.b = vSpecular.z;
    plight->Specular.a = 1.0f;
    plight->Ambient.r = 0.0f;
    plight->Ambient.g = 0.0f;
    plight->Ambient.b = 0.0f;
    plight->Ambient.a = 0.0f;
    plight->Range = 1000.0f;
//    plight->Attenuation0 = 0.5f;
//    plight->Attenuation1 = 0.0f;
//    plight->Attenuation2 = 0.0f;
//    plight->Attenuation0 = 0.0f;
//    plight->Attenuation1 = 0.0125f + FRND(0.0125f);
//    plight->Attenuation2 = 0.0f;
    plight->Attenuation0 = 0.0f;
    plight->Attenuation1 = 0.125f + FRND(0.125f);
    plight->Attenuation2 = 0.0f;

    m_Lights[m_uNumLights].cDiffuse = ModulateColors(&plight->Diffuse, &m_material.Diffuse);
    m_Lights[m_uNumLights].cAmbient = ModulateColors(&plight->Ambient, &m_material.Ambient);
    m_Lights[m_uNumLights].cSpecular = ModulateColors(&plight->Specular, &m_material.Specular);
    if (m_uNumLights) {
        m_Lights[m_uNumLights].cEmissive = 0;
    }
    else {
        dcvAmbientEmissive.r = m_material.Emissive.r + m_material.Ambient.r * m_dcvAmbient.r;
        dcvAmbientEmissive.g = m_material.Emissive.g + m_material.Ambient.g * m_dcvAmbient.g;
        dcvAmbientEmissive.b = m_material.Emissive.b + m_material.Ambient.b * m_dcvAmbient.b;
        dcvAmbientEmissive.a = m_material.Emissive.a + m_material.Ambient.a * m_dcvAmbient.a;
        if (dcvAmbientEmissive.r > 1.0f) dcvAmbientEmissive.r = 1.0f;
        if (dcvAmbientEmissive.g > 1.0f) dcvAmbientEmissive.g = 1.0f;
        if (dcvAmbientEmissive.b > 1.0f) dcvAmbientEmissive.b = 1.0f;
        if (dcvAmbientEmissive.a > 1.0f) dcvAmbientEmissive.a = 1.0f;

        m_Lights[m_uNumLights].cEmissive = (BYTE)(dcvAmbientEmissive.a * 255.0f) << 24 | 
                                           (BYTE)(dcvAmbientEmissive.r * 255.0f) << 16 |
                                           (BYTE)(dcvAmbientEmissive.g * 255.0f) << 8 |
                                           (BYTE)(dcvAmbientEmissive.b * 255.0f);
    }

    m_uNumLights++;
}

//******************************************************************************
BOOL CPSLights::CreateShaders() {

    D3DPIXELSHADERDEF   d3dpsd;
    HRESULT             hr;

    DWORD dwSDecl[] = {
            D3DVSD_STREAM(0),
            D3DVSD_REG(0, D3DVSDT_FLOAT3),
            D3DVSD_REG(1, D3DVSDT_FLOAT2),
            D3DVSD_REG(2, D3DVSDT_FLOAT3),
            D3DVSD_REG(3, D3DVSDT_FLOAT3),
            D3DVSD_REG(4, D3DVSDT_FLOAT3),
            D3DVSD_END()
    };

    m_dwVShader1 = CreateVertexShader(m_pDevice, dwSDecl, TEXT("light1.vsh"));
    if (m_dwVShader1 == INVALID_SHADER_HANDLE) {
        return FALSE;
    }

    m_dwVShader2 = CreateVertexShader(m_pDevice, dwSDecl, TEXT("light2.vsh"));
    if (m_dwVShader2 == INVALID_SHADER_HANDLE) {
        return FALSE;
    }

    // White light with specular power of 8
    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));
    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(2, PS_COMBINERCOUNT_SAME_C0);//PS_COMBINERCOUNT_UNIQUE_C0);
    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_CUBEMAP, 
                                            PS_TEXTUREMODES_CUBEMAP);

    d3dpsd.PSRGBInputs[0] = PS_COMBINERINPUTS(
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T2 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T3 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_R1, PS_REGISTER_DISCARD,
                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA);

    d3dpsd.PSRGBInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    // Ambient light
    d3dpsd.PSConstant0[0] = (BYTE)(m_dcvAmbient.a * 255.0f) << 24 | 
                            (BYTE)(m_dcvAmbient.r * 255.0f) << 16 |
                            (BYTE)(m_dcvAmbient.g * 255.0f) << 8 |
                            (BYTE)(m_dcvAmbient.b * 255.0f);

    d3dpsd.PSAlphaInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB, 
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB,
                                PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_T0 | PS_CHANNEL_ALPHA,
                                0);

    hr = m_pDevice->CreatePixelShader(&d3dpsd, &m_dwPShader1);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
        return FALSE;
    }

    // Complete light with specular power of 16
    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));
    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(4, PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_CUBEMAP, 
                                            PS_TEXTUREMODES_CUBEMAP);

    d3dpsd.PSRGBInputs[0] = PS_COMBINERINPUTS(
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T2 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T3 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_R1, PS_REGISTER_DISCARD,
                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA);

    d3dpsd.PSRGBInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C1 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_T1, PS_REGISTER_T2, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSAlphaInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSRGBInputs[2] = PS_COMBINERINPUTS(
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[2] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    d3dpsd.PSAlphaInputs[2] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[2] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSRGBInputs[3] = PS_COMBINERINPUTS(
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[3] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    d3dpsd.PSAlphaInputs[3] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[3] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
                                PS_REGISTER_T2 | PS_CHANNEL_RGB, 
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB,
                                PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_T0 | PS_CHANNEL_ALPHA,
                                0);

    hr = m_pDevice->CreatePixelShader(&d3dpsd, &m_dwPShader2);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePixelShader"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
void CPSLights::ReleaseShaders() {

    if (m_dwVShader1 != INVALID_SHADER_HANDLE) {
        m_pDevice->DeleteVertexShader(m_dwVShader1);
    }
    if (m_dwVShader2 != INVALID_SHADER_HANDLE) {
        m_pDevice->DeleteVertexShader(m_dwVShader2);
    }
    if (m_dwPShader1) {
        m_pDevice->DeletePixelShader(m_dwPShader1);
    }
    if (m_dwPShader2) {
        m_pDevice->DeletePixelShader(m_dwPShader2);
    }
}

//******************************************************************************
BOOL CPSLights::CreateSphere(float fRadius, UINT uStepsU, UINT uStepsV) {

    BUMPVERTEX* prVertices;
    BUMPVERTEX* pr;
    UINT        uNumVertices;
    UINT        uIndex = 0;
    UINT        i, j;
    float       fX, fY, fTX, fSinY, fCosY;
    HRESULT     hr;

    uNumVertices = (uStepsU + 1) * uStepsV;

    // Create the sphere vertex buffer
    hr = m_pDevice->CreateVertexBuffer(uNumVertices * sizeof(BUMPVERTEX), 0, 0, 0, &m_pd3drSphere);
    if (ResultFailed(hr, TEXT("CreateVertexBuffer"))) {
        return FALSE;
    }

    // Allocate memory for the indices
    m_uNumSphereIndices = uStepsU * (uStepsV - 1) * 6;

    m_pd3drSphere->Lock(0, 0, (LPBYTE*)&prVertices, 0);

    m_pwSphere = (LPWORD)MemAlloc32(m_uNumSphereIndices * sizeof(WORD));
    if (!m_pwSphere) {
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
            pr->u = fX * (float)uStepsU / 4.0f;
            pr->v = fY * (float)uStepsV / 4.0f;
        }
    }

    for (j = 0; j < uStepsV - 1; j++) {

        for (i = 0; i < uStepsU; i++) {

            m_pwSphere[uIndex++] = (j + 1) * (uStepsU + 1) + i;
            m_pwSphere[uIndex++] = j * (uStepsU + 1) + i;
            m_pwSphere[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
            m_pwSphere[uIndex++] = j * (uStepsU + 1) + i;
            m_pwSphere[uIndex++] = j * (uStepsU + 1) + i + 1;
            m_pwSphere[uIndex++] = (j + 1) * (uStepsU + 1) + i + 1;
        }
    }

    if (!ComputeTangentTransforms(prVertices, m_pwSphere, m_uNumSphereIndices, TRUE)) {
        return FALSE;
    }

    m_pd3drSphere->Unlock();

    return TRUE;
}

//******************************************************************************
void CPSLights::ReleaseSphere() {

    if (m_pd3drSphere) {
        m_pd3drSphere->Release();
    }
    if (m_pwSphere) {
        MemFree32(m_pwSphere);
    }
}

//******************************************************************************
CTexture8* CPSLights::CreateLightTexture(UINT uLength) {

    CTexture8* pd3dt;
    D3DLOCKED_RECT d3dlr;
    LPDWORD pdwTexel;
    D3DXVECTOR3 vNormal, vCenter, vUp;
    float fRadius, fRadiusSq;
    BYTE Intensity;
    UINT i, j;
    Swizzler swz(uLength, uLength, 1);

    pd3dt = (CTexture8*)CreateTexture(m_pDevice, uLength, uLength, D3DFMT_A8R8G8B8);
    if (!pd3dt) {
        return NULL;
    }

    pd3dt->LockRect(0, &d3dlr, NULL, 0);
    pdwTexel = (LPDWORD)d3dlr.pBits;
    fRadius = (float)(uLength / 2);
    fRadiusSq = fRadius * fRadius;
    vCenter = D3DXVECTOR3(fRadius + 0.5f, fRadius + 0.5f, 0.0f);
    vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    swz.SetU(0);
    swz.SetV(0);

    for (i = 0; i < uLength; i++) {

        for (j = 0; j < uLength; j++) {

            vNormal = D3DXVECTOR3((float)j, (float)i, 0.0f) - vCenter;
            if (D3DXVec3Length(&vNormal) < fRadius) {
                vNormal.z = (float)sqrt(fRadiusSq - vNormal.x * vNormal.x - vNormal.y * vNormal.y);
                D3DXVec3Normalize(&vNormal, &vNormal);
                Intensity = (BYTE)(D3DXVec3Dot(&vNormal, &vUp) * 255.0f);
                pdwTexel[swz.Get2D()] = RGBA_MAKE(Intensity, Intensity, Intensity, Intensity);
            }
            else {
                pdwTexel[swz.Get2D()] = 0;
            }

            swz.IncU();
        }

        swz.IncV();
    }

    pd3dt->UnlockRect(0);

    return pd3dt;
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
void CPSLights::Efface() {

    ReleaseTexture(m_pd3dtBase);
    ReleaseTexture(m_pd3dtBump);
    ReleaseTexture(m_pd3dtcNormal);
    ReleaseTexture(m_pd3dtLight);
    ReleaseSphere();
    ReleaseShaders();
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
void CPSLights::Update() {

    D3DXMATRIX mTransform, mRotation;
    UINT       i;

    m_vSphereRot += D3DXVECTOR3(PNSphereRotX.GetIntensity(m_fTime), PNSphereRotY.GetIntensity(m_fTime), PNSphereRotZ.GetIntensity(m_fTime));
    D3DXMatrixRotationX(&mTransform, m_vSphereRot.x);
    D3DXMatrixRotationY(&mRotation, m_vSphereRot.y);
    D3DXMatrixMultiply(&mTransform, &mTransform, &mRotation);
    D3DXMatrixRotationZ(&mRotation, m_vSphereRot.z);
    D3DXMatrixMultiply(&m_mWorld, &mTransform, &mRotation);

    for (i = 0; i < m_uNumLights; i++) {
        D3DXVec3Normalize(&m_Lights[i].vDirection, &D3DXVECTOR3(m_Lights[i].PNDirection[0].GetIntensity(m_fTime), m_Lights[i].PNDirection[1].GetIntensity(m_fTime), m_Lights[i].PNDirection[2].GetIntensity(m_fTime)));
        m_Lights[i].fDistance = m_Lights[i].PNDistance.GetIntensity(m_fTime);
        m_Lights[i].light.Position = m_Lights[i].fDistance * m_Lights[i].vDirection;
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
BOOL CPSLights::Render() {

    DWORD        dwClearFlags = D3DCLEAR_TARGET;
    D3DXMATRIX   mTransform, mView, mProj, mBackWorld;
    D3DXVECTOR3  vEyePos, vLightPos;
    CAMERA       cam;
    TCHAR        szNumLights[64];
    UINT         uLen;
    float        fHeight, fWidth;
    UINT         i;
    HRESULT      hr;

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

    m_pDisplay->SetView();
    m_pDisplay->GetCamera(&cam);

    SetView((LPD3DXMATRIX)&mView, &cam.vPosition, &cam.vInterest, 
            &D3DXVECTOR3((float)sin(cam.fRoll), (float)cos(cam.fRoll), 
            0.0f));
    SetPerspectiveProjection(&mProj, cam.fNearPlane, cam.fFarPlane, 
            cam.fFieldOfView, (float)m_pDisplay->GetHeight() / (float)m_pDisplay->GetWidth());

    D3DXMatrixMultiply(&mTransform, &m_mWorld, &mView);
    D3DXMatrixMultiply(&mTransform, &mTransform, &mProj);
    D3DXMatrixTranspose(&mTransform, &mTransform);

    m_pDevice->SetVertexShaderConstant(0, &mTransform, 4);

    D3DXMatrixInverse(&mBackWorld, NULL, &m_mWorld);

    // Transform the eye position into model space
    D3DXVec3TransformCoord(&vEyePos, &cam.vPosition, &mBackWorld);
    m_pDevice->SetVertexShaderConstant(5, &vEyePos, 1);

    if (!m_bCompleteLight) {
        m_pDevice->SetVertexShader(m_dwVShader1);
        m_pDevice->SetPixelShader(m_dwPShader1);
    }
    else {
        m_pDevice->SetVertexShader(m_dwVShader2);
        m_pDevice->SetPixelShader(m_dwPShader2);
    }

    m_pDevice->SetStreamSource(0, m_pd3drSphere, sizeof(BUMPVERTEX));

    m_pDevice->SetTexture(0, m_pd3dtBase);
    m_pDevice->SetTexture(1, m_pd3dtBump);
    m_pDevice->SetTexture(2, m_pd3dtcNormal);
    m_pDevice->SetTexture(3, m_pd3dtcNormal);

    for (i = 0; i < m_uNumLights; i++) {

        // Transform the light position into model space
        vLightPos.x = m_Lights[i].light.Position.x;
        vLightPos.y = m_Lights[i].light.Position.y;
        vLightPos.z = m_Lights[i].light.Position.z;
        D3DXVec3TransformCoord(&vLightPos, &vLightPos, &mBackWorld);
        m_pDevice->SetVertexShaderConstant(4, &vLightPos, 1);

        // Set the light parameters
        m_pDevice->SetVertexShaderConstant(7, &m_Lights[i].light.Attenuation0, 1);
        m_pDevice->SetVertexShaderConstant(8, &D3DXVECTOR4(0.0f, 0.0f, 0.0f, m_Lights[i].light.Range), 1);
        m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_1, m_Lights[i].cDiffuse);
        m_pDevice->SetRenderState(D3DRS_PSCONSTANT1_1, m_Lights[i].cSpecular);
        m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_2, m_Lights[i].cAmbient);
        m_pDevice->SetRenderState(D3DRS_PSCONSTANT0_3, m_Lights[i].cEmissive);

        m_pDevice->DrawIndexedVertices(D3DPT_TRIANGLELIST, m_uNumSphereIndices, m_pwSphere);

        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    }

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    m_pDevice->SetTexture(2, NULL);

    // Draw the lights themselves
    m_pDevice->SetTexture(3, m_pd3dtLight);
    m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_TFACTOR, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    m_pDevice->SetVertexShader(D3DFVF_XYZ);
    m_pDevice->SetPixelShader(0);

    if (!m_bCompleteLight) {
        m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
        for (i = 0; i < m_uNumLights; i++) {
            m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &m_Lights[i].light.Position, sizeof(D3DXVECTOR3));
        }
    }
    else {
        for (i = 0; i < m_uNumLights; i++) {
            m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, RGBA_MAKE((BYTE)(255.0f * m_Lights[i].light.Diffuse.r), (BYTE)(255.0f * m_Lights[i].light.Diffuse.g), (BYTE)(255.0f * m_Lights[i].light.Diffuse.b), (BYTE)(255.0f * m_Lights[i].light.Diffuse.a)));
            m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &m_Lights[i].light.Position, sizeof(D3DXVECTOR3));
        }
    }

    m_pDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    SetColorStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_DISABLE);
    SetAlphaStage(m_pDevice, 3, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_DISABLE);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    m_pDevice->SetTexture(3, NULL);

    // Display the adapter mode
//    ShowDisplayMode();

    if (!(GetStartupContext() & TSTART_STRESS)) {

        wsprintf(szNumLights, TEXT("%2d light%s"), m_uNumLights, m_uNumLights != 1 ? TEXT("s") : TEXT(" "));
        fHeight = (float)m_pDisplay->GetHeight() / 20.0f;
        fWidth = fHeight / CHAR_ASPECT;
        uLen = _tcslen(szNumLights);
        xTextOut((float)(m_pDisplay->GetWidth() / 2) - (float)(uLen / 2) * fWidth, 
                    fHeight, fWidth, fHeight, RGBA_XCOLOR(255, 255, 0, 255), szNumLights, uLen);

        // Display the frame rate
        ShowFrameRate();

        // Display the console
        m_pDisplay->ShowConsole();
    }

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
void CPSLights::ProcessInput() {

    CScene::ProcessInput();

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                AddLight();
            }
            else {
                m_bCompleteLight = !m_bCompleteLight;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_Y)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {
            if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                if (m_uNumLights) {
                    m_uNumLights--;
                }
            }
            else {
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
BOOL CPSLights::InitView() {

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
BOOL CPSLights::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
// Calculate non-normalized tangent and binormal vector terms for use in
// constructing an othonormal basis transform to rotate a vector from model
// space to tangent space.
//******************************************************************************
BOOL CPSLights::CalculateTangentTerms(D3DXVECTOR3* pvTangent, D3DXVECTOR3* pvBinormal, 
                                            BUMPVERTEX* prVertices, LPWORD pwIndices, 
                                            UINT uNumIndices, BOOL bInterpolate)
{
    D3DXVECTOR3     vEdge0, vEdge1, vTemp;
    D3DXVECTOR3     vPlane[3];
    WORD            w0, w1, w2;
    float           fTemp;
    UINT            i, j, k;

    memset(pvTangent, 0, uNumIndices * sizeof(D3DXVECTOR3));
    memset(pvBinormal, 0, uNumIndices * sizeof(D3DXVECTOR3));

    // Calculate the tangent and binormal vectors for each vertex.  If the vertex is
    // indexed by more than one triangle, add the vectors for each triangle together
    // to obtain an average of the vectors for all triangles formed by the vertex.
    for (i = 0; i < uNumIndices; i+=3) {

        w0 = pwIndices[i];
        w1 = pwIndices[i+1];
        w2 = pwIndices[i+2];

        vEdge0 = D3DXVECTOR3(prVertices[w1].vPosition.x - prVertices[w0].vPosition.x, prVertices[w1].u - prVertices[w0].u, prVertices[w1].v - prVertices[w0].v);
        vEdge1 = D3DXVECTOR3(prVertices[w2].vPosition.x - prVertices[w0].vPosition.x, prVertices[w2].u - prVertices[w0].u, prVertices[w2].v - prVertices[w0].v);
        D3DXVec3Cross(&vPlane[0], &vEdge0, &vEdge1);
        vEdge0.x = prVertices[w1].vPosition.y - prVertices[w0].vPosition.y;
        vEdge1.x = prVertices[w2].vPosition.y - prVertices[w0].vPosition.y;
        D3DXVec3Cross(&vPlane[1], &vEdge0, &vEdge1);
        vEdge0.x = prVertices[w1].vPosition.z - prVertices[w0].vPosition.z;
        vEdge1.x = prVertices[w2].vPosition.z - prVertices[w0].vPosition.z;
        D3DXVec3Cross(&vPlane[2], &vEdge0, &vEdge1);

        if (FABS(vPlane[0].x) < 0.000000001f || FABS(vPlane[1].x) < 0.000000001f || FABS(vPlane[2].x) < 0.000000001f) {
            return FALSE;
        }

        vTemp = D3DXVECTOR3(-vPlane[0].y / vPlane[0].x, -vPlane[1].y / vPlane[1].x, -vPlane[2].y / vPlane[1].x);
        pvTangent[w0] += vTemp;
        pvTangent[w1] += vTemp;
        pvTangent[w2] += vTemp;

        vTemp = D3DXVECTOR3(-vPlane[0].z / vPlane[0].x, -vPlane[1].z / vPlane[1].x, -vPlane[2].z / vPlane[1].x);
        pvBinormal[w0] += vTemp;
        pvBinormal[w1] += vTemp;
        pvBinormal[w2] += vTemp;
    }

    if (bInterpolate) {

        LPWORD          pwMerge, pwProcessed;
        UINT            uNumMerges, uNumProcessed = 0;

        pwMerge = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwMerge) {
            return FALSE;
        }

        pwProcessed = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwProcessed) {
            MemFree32(pwMerge);
            return FALSE;
        }

        for (i = 0; i < uNumIndices; i++) {

            // Verify pwIndices[i] has to already been processed...if it has continue
            for (j = 0; j < uNumProcessed; j++) {
                if (pwIndices[i] == pwProcessed[j]) {
                    break;
                }
            }
            if (j < uNumProcessed) {
                continue;
            }

            pwMerge[0] = pwIndices[i];
            pwProcessed[uNumProcessed++] = pwIndices[i];
            uNumMerges = 1;

            // Traverse the vertex list, identifying all vertices whose positions are
            // equal to the current vertex position
            for (j = i + 1; j < uNumIndices; j++) {

                fTemp = D3DXVec3LengthSq(&(prVertices[pwIndices[i]].vPosition - prVertices[pwIndices[j]].vPosition));
                if (fTemp < 0.0000001f) {

                    // See whether the matching vertex has already been added to the merge list
                    for (k = 0; k < uNumMerges; k++) {
                        if (pwIndices[j] == pwMerge[k]) {
                            break;
                        }
                    }
                    if (k == uNumMerges) {
                        pwMerge[uNumMerges++] = pwIndices[j];
                        pwProcessed[uNumProcessed++] = pwIndices[j];
                    }
                }
            }

            if (uNumMerges > 1 && uNumMerges < 5) {
                w0 = pwMerge[0];
                D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
                D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);
                for (j = 1; j < uNumMerges; j++) {
                    w1 = pwMerge[j];
                    D3DXVec3Normalize(&pvTangent[w1], &pvTangent[w1]);
                    D3DXVec3Normalize(&pvBinormal[w1], &pvBinormal[w1]);
                    pvTangent[w0] += pvTangent[w1];
                    pvBinormal[w0] += pvBinormal[w1];
                }
                for (j = 1; j < uNumMerges; j++) {
                    pvTangent[pwMerge[j]] = pvTangent[w0];
                    pvBinormal[pwMerge[j]] = pvBinormal[w0];
                }
            }
        }

        MemFree32(pwProcessed);
        MemFree32(pwMerge);
    }

    return TRUE;
}

//******************************************************************************
BOOL CPSLights::ComputeTangentTransforms(BUMPVERTEX* prVertices, LPWORD pwIndices, UINT uNumIndices, BOOL bInterpolate) {

    D3DXVECTOR3     vNormal, vTemp;
    D3DXVECTOR3     *pvTangent, *pvBinormal;
    WORD            w0;
    PMATRIX3X3      pmT;
    MATRIX3X3       mZero;
	UINT            i;

    memset(&mZero, 0, sizeof(MATRIX3X3));

    for (i = 0; i < uNumIndices; i++) {
        memcpy((LPBYTE)&prVertices[pwIndices[i]].vTangent.x, &mZero, sizeof(MATRIX3X3));
    }

    pvTangent = (D3DXVECTOR3*)MemAlloc32(2 * uNumIndices * sizeof(D3DXVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + uNumIndices;

    if (!CalculateTangentTerms(pvTangent, pvBinormal, prVertices, pwIndices, uNumIndices, bInterpolate)) {
        MemFree32(pvTangent);
        return FALSE;
    }

    for (i = 0; i < uNumIndices; i++) {

        w0 = pwIndices[i];
        pmT = (PMATRIX3X3)((LPBYTE)&prVertices[w0].vTangent.x);
        if (!memcmp(pmT, &mZero, sizeof(MATRIX3X3))) {

            D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            D3DXVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

            pmT->_11 = -pvTangent[w0].x;
            pmT->_12 = -pvTangent[w0].y;
            pmT->_13 = -pvTangent[w0].z;

            pmT->_21 = -pvBinormal[w0].x;
            pmT->_22 = -pvBinormal[w0].y;
            pmT->_23 = -pvBinormal[w0].z;

            pmT->_31 = vNormal.x;
            pmT->_32 = vNormal.y;
            pmT->_33 = vNormal.z;
        }
    }

    MemFree32(pvTangent);

    return TRUE;
}

//******************************************************************************
D3DCOLOR ModulateColors(D3DCOLORVALUE* pdcv1, D3DCOLORVALUE* pdcv2) {

    D3DCOLORVALUE dcv;
    dcv.r = pdcv1->r * pdcv2->r;
    dcv.g = pdcv1->g * pdcv2->g;
    dcv.b = pdcv1->b * pdcv2->b;
    dcv.a = pdcv1->a * pdcv2->a;
//    if (dcv.r > 1.0f) dcv.r = 1.0f;
//    if (dcv.g > 1.0f) dcv.g = 1.0f;
//    if (dcv.b > 1.0f) dcv.b = 1.0f;
//    if (dcv.a > 1.0f) dcv.a = 1.0f;

    return (BYTE)(dcv.a * 255.0f) << 24 | (BYTE)(dcv.r * 255.0f) << 16 |
           (BYTE)(dcv.g * 255.0f) << 8 | (BYTE)(dcv.b * 255.0f);
}
