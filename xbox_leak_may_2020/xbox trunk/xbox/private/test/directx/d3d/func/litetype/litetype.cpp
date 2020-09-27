/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    litetype.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include <profilep.h>
#endif
#include "litetype.h"

#define RAND_COLOR  (RGBA_MAKE(rand()%255, rand()%255, rand()%255, rand()%255))

typedef UINT* LPUINT;

static bool InitLights(LPTSTR szCmdLine, PLIGHTCLUSTER* pplc, LPUINT puNumClusters);
static void SetLightType(D3DLIGHT8* plight, D3DLIGHTTYPE d3dlt, UINT uCluster);
static void UpdateCluster(PLIGHTCLUSTER plc);

//******************************************************************************
bool InitLights(LPTSTR szCmdLine, PLIGHTCLUSTER* pplc, LPUINT puNumClusters) {

    PLIGHTCLUSTER plcRet;
    PLIGHTCLUSTER plc;
    D3DLIGHT8*    plight;
    UINT          uNumClusters = 0, uNumLights, i;
    bool          bAmbient;
    TCHAR         szDefault[] = TEXT("-d -p -s -a");
    TCHAR*        sz;

    if (!pplc || !puNumClusters) {
        return false;
    }

    plc = (PLIGHTCLUSTER)MemAlloc(50 * sizeof(LIGHTCLUSTER));
    if (!plc) {
        return false;
    }

    plight = (D3DLIGHT8*)MemAlloc(50 * sizeof(D3DLIGHT8));
    if (!plight) {
        MemFree(plc);
        return false;
    }

    for (sz = szCmdLine; *sz; sz++) {
        if (*sz == TEXT('/') || *sz == TEXT('-')) {
            break;
        }
    }
    if (*sz) {
        sz = szCmdLine;
    }
    else {
        sz = szDefault;
    }

    while (true) {

        for (; *sz && !(*sz == TEXT('/') || *sz == TEXT('-')); sz++);
        if (!*sz) {
            break;
        }

        uNumLights = 0;
        bAmbient = false;

        for (; *sz && !(*sz == TEXT(' ') || *sz == TEXT('\t')); sz++) {
        
            switch (*sz) {

                case TEXT('a'):
                    bAmbient = true;
                    break;

                case TEXT('d'):
                case TEXT('D'):
                    SetLightType(&plight[uNumLights], D3DLIGHT_DIRECTIONAL, uNumClusters);
                    uNumLights++;
                    break;

                case TEXT('p'):
                case TEXT('P'):
                    SetLightType(&plight[uNumLights], D3DLIGHT_POINT, uNumClusters);
                    uNumLights++;
                    break;

                case TEXT('s'):
                case TEXT('S'):
                    SetLightType(&plight[uNumLights], D3DLIGHT_SPOT, uNumClusters);
                    uNumLights++;
                    break;
            }
        }

        if (uNumLights) {

            plc[uNumClusters].plight = new D3DLIGHT8[uNumLights];
            if (!plc[uNumClusters].plight) {
                // TODO: Cleanup allocated clusters
                MemFree(plc);
                MemFree(plight);
                return false;
            }

            for (i = 0; i < uNumLights; i++) {
                memcpy(&plc[uNumClusters].plight[i], &plight[i], sizeof(D3DLIGHT8));
            }
        }
        else {

            plc[uNumClusters].plight = NULL;
        }

        plc[uNumClusters].uNumLights = uNumLights;
        plc[uNumClusters].bAmbient = bAmbient;
        uNumClusters++;
    }

    if (!uNumClusters) {
        MemFree(plc);
        MemFree(plight);
        return false;
    }

    plcRet = new LIGHTCLUSTER[uNumClusters];
    if (!plcRet) {
        MemFree(plc);
        MemFree(plight);
        return false;
    }

    for (i = 0; i < uNumClusters; i++) {
        memcpy(&plcRet[i], &plc[i], sizeof(LIGHTCLUSTER));
    }
    *pplc = plcRet;
    *puNumClusters = uNumClusters;

    MemFree(plc);
    MemFree(plight);

    return true;
}

//******************************************************************************
void SetLightType(D3DLIGHT8* plight, D3DLIGHTTYPE d3dlt, UINT uCluster) {

    memset(plight, 0, sizeof(D3DLIGHT8));
    plight->Type = d3dlt;
    plight->Range = LIGHT_RANGE;

    switch (d3dlt) {

        case D3DLIGHT_DIRECTIONAL:
            plight->Diffuse.r = 1.0f;
            plight->Diffuse.g = 1.0f;
            plight->Diffuse.b = 1.0f;
            plight->Diffuse.a = 1.0f;
            plight->Specular.r = 1.0f;
            plight->Specular.g = 1.0f;
            plight->Specular.b = 1.0f;
            plight->Specular.a = 1.0f;
            plight->Ambient.r = 1.0f;
            plight->Ambient.g = 1.0f;
            plight->Ambient.b = 1.0f;
            plight->Ambient.a = 1.0f;
            plight->Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
            break;

        case D3DLIGHT_POINT:
            plight->Diffuse.r = 1.0f;
            plight->Diffuse.g = 1.0f;
            plight->Diffuse.b = 1.0f;
            plight->Diffuse.a = 1.0f;
            plight->Specular.r = 1.0f;
            plight->Specular.g = 1.0f;
            plight->Specular.b = 1.0f;
            plight->Specular.a = 1.0f;
            plight->Ambient.r = 1.0f;
            plight->Ambient.g = 1.0f;
            plight->Ambient.b = 1.0f;
            plight->Ambient.a = 1.0f;
            plight->Position = D3DXVECTOR3(LIGHT_RANGE + uCluster * LIGHT_RANGE * 2.5f, 0.0f, 0.0f);
            plight->Attenuation0 = 1.0f;
            plight->Attenuation1 = 0.0f;
            plight->Attenuation2 = 0.0f;
            break;

        case D3DLIGHT_SPOT:
            plight->Diffuse.r = 1.0f;
            plight->Diffuse.g = 1.0f;
            plight->Diffuse.b = 1.0f;
            plight->Diffuse.a = 1.0f;
            plight->Specular.r = 1.0f;
            plight->Specular.g = 1.0f;
            plight->Specular.b = 1.0f;
            plight->Specular.a = 1.0f;
            plight->Ambient.r = 1.0f;
            plight->Ambient.g = 1.0f;
            plight->Ambient.b = 1.0f;
            plight->Ambient.a = 1.0f;
            plight->Position = D3DXVECTOR3(LIGHT_RANGE + uCluster * LIGHT_RANGE * 2.5f, 0.0f, 0.0f);
            plight->Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
            plight->Falloff = 1.0f;
            plight->Attenuation0 = 1.0f;
            plight->Attenuation1 = 0.0f;
            plight->Attenuation2 = 0.0f;
            plight->Theta = M_PI / 6.0f;
            plight->Phi = M_PIDIV2;
            break;
    }
}

//******************************************************************************
void UpdateCluster(PLIGHTCLUSTER plc) {

    HRESULT         hr;
    UINT            i, uNumLights;
    D3DLIGHTTYPE    d3dlt[] = {D3DLIGHT_DIRECTIONAL, D3DLIGHT_POINT, 
                               D3DLIGHT_SPOT};


    uNumLights = rand() % (STRESS_MAX_LIGHTS_PER_CLUSTER + 1);

    // Initialize new lights
    for (i = 0; i < uNumLights; i++) {
        SetLightType(&(plc->plight[i]), d3dlt[rand() % 3], i);
        plc->plight[i].Diffuse.r = (plc->plight[i].Type == D3DLIGHT_DIRECTIONAL || plc->plight[i].Type == D3DLIGHT_SPOT) ? 0.0f : 1.0f;
        plc->plight[i].Diffuse.g = (plc->plight[i].Type == D3DLIGHT_DIRECTIONAL) ? 1.0f : 0.0f;
        plc->plight[i].Diffuse.b = (plc->plight[i].Type == D3DLIGHT_SPOT) ? 1.0f : 0.0f;
    }

    plc->bSpecular = (BOOL)(rand() % 2);
    plc->uNumLights = uNumLights;
    plc->bAmbient = (rand() % 2 == 1);
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

    CLightType* pLightType;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pLightType = new CLightType();
    if (!pLightType) {
        return FALSE;
    }

    // Initialize the scene
    if (!pLightType->Create(pDisplay)) {
        pLightType->Release();
        return FALSE;
    }

    bRet = pLightType->Exhibit(pnExitCode);

    // Clean up the scene
    pLightType->Release();

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
// CLightType
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CLightType
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
CLightType::CLightType() {

    m_fScreenDepth = 3.0f / 5.0f * LIGHT_RANGE;
    m_fDepthOffset = 0.0f;
    m_plc = NULL;
    m_uNumClusters = 0;
    m_cAmbient = RGB_MAKE(64, 64, 64);
    m_uAttenuation = 0;
    m_bTexture = false;
    m_bColored = false;
    m_bSpecular = TRUE;
    m_bColorVertex = FALSE;
    m_bLighting = TRUE;

    m_pd3dt = NULL;
    m_prScreenXY = NULL;
    m_prScreenYZ = NULL;
    m_pwScreenXY = NULL;
    m_pwScreenYZ = NULL;

RegisterStartupContext(GetStartupContext() | TSTART_HARNESS);
}

//******************************************************************************
//
// Method:
//
//     ~CLightType
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
CLightType::~CLightType() {

    UINT i;

    if (!m_bStress) {
        for (i = 0; i < m_uNumClusters; i++) {
            if (m_plc[i].plight) {
                delete [] m_plc[i].plight;
            }
        }
        delete [] m_plc;
    }

    MemFree32(m_prScreenXY);
    MemFree32(m_prScreenYZ);
    delete [] m_pwScreenXY;
    delete [] m_pwScreenYZ;
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
BOOL CLightType::Create(CDisplay* pDisplay) {

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
int CLightType::Exhibit(int *pnExitCode) {

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
BOOL CLightType::Prepare() {

    TCHAR       szName[32];
    D3DXVECTOR3 vN;
    float       x, y, z, fCell, fDepth;
    int         i, j, k;

#ifndef UNDER_XBOX

    LPTSTR      szCmdLine;

    for (szCmdLine = GetCommandLine(); *szCmdLine && *szCmdLine != TEXT('-'); szCmdLine++);

#else

    TCHAR       szCmdLine[128] = {0};

    if (GetModuleName(GetTestInstance(), szName, 32)) {

        GetProfileString(szName, TEXT("CommandLine"), TEXT(""), szCmdLine, sizeof(szCmdLine));

#endif // UNDER_XBOX

        if (*szCmdLine || !(GetStartupContext() & TSTART_HARNESS)) {

            if (!_tcscmp(szCmdLine, TEXT("-stress"))) {

                // Stress mode
                m_plc = NULL;
                m_uNumClusters = STRESS_CLUSTERS;
                m_bStress = TRUE;
            }
            else {

                // Initialize the lights
                if (!InitLights(szCmdLine, &m_plc, &m_uNumClusters)) {
                    return FALSE;
                }
                m_bStress = FALSE;
            }
        }
        else {
            if (rand() % 2) {
                m_plc = NULL;
                m_uNumClusters = STRESS_CLUSTERS;
                m_bStress = TRUE;
            }
            else {
                if (!InitLights(szCmdLine, &m_plc, &m_uNumClusters)) {
                    return FALSE;
                }
                m_bStress = FALSE;
            }
            SetFrameDuration(1440.0f);
        }

#ifdef UNDER_XBOX
    }
    else {
        m_plc = NULL;
        m_uNumClusters = STRESS_CLUSTERS;
        m_bStress = TRUE;
    }
#endif

    fCell = 2.0f * LIGHT_RANGE / SCREEN_DIM;
    fDepth = 0.0f;
    m_uNumVerticesXY = (SCREEN_DIM+1)*(SCREEN_DIM+1);
    m_uNumVerticesYZ = (SCREEN_DIM+1)*(SCREEN_DIM/2+1);
    m_uNumIndicesXY = SCREEN_DIM * SCREEN_DIM * 6;
    m_uNumIndicesYZ = SCREEN_DIM * SCREEN_DIM / 2 * 6;

    // Initialize the screen vertices
    m_prScreenXY = (PCLVERTEX)MemAlloc32(m_uNumVerticesXY * sizeof(CLVERTEX));
    if (!m_prScreenXY) {
        return FALSE;
    }
    m_prScreenYZ = (PCLVERTEX)MemAlloc32(m_uNumVerticesYZ * sizeof(CLVERTEX));
    if (!m_prScreenYZ) {
        return FALSE;
    }

    for (j = 0, y = LIGHT_RANGE; j <= SCREEN_DIM; j++, y -= fCell) {

        // XY plane
        for (k = 0, x = 0.0f; k <= SCREEN_DIM; k++, x += fCell) {

            m_prScreenXY[j*(SCREEN_DIM+1)+k] = CLVERTEX(D3DXVECTOR3(x, y, fDepth), D3DXVECTOR3(0.0f, 0.0f, -1.0f), RAND_COLOR, RAND_COLOR, (float)k / (float)SCREEN_DIM, (float)j / (float)SCREEN_DIM);
        }

        // YZ plane
        for (k = 0, z = 0.0f; k <= SCREEN_DIM / 2; k++, z += fCell) {

            m_prScreenYZ[j*(SCREEN_DIM/2+1)+k] = CLVERTEX(D3DXVECTOR3(LIGHT_RANGE, y, z), *D3DXVec3Normalize(&vN, &D3DXVECTOR3(0.0f, -y, -z)), RAND_COLOR, RAND_COLOR, 0.0f, 0.0f);
        }
    }

    // Initialize the screen indices
    m_pwScreenXY = new WORD[m_uNumIndicesXY];
    if (!m_pwScreenXY) {
        return FALSE;
    }
    m_pwScreenYZ = new WORD[m_uNumIndicesYZ];
    if (!m_pwScreenYZ) {
        return FALSE;
    }

    for (j = 0; j < SCREEN_DIM; j++) {

        for (k = 0; k < SCREEN_DIM; k++) {

            m_pwScreenXY[j*SCREEN_DIM*6+k*6+0] = (j + 1) * (SCREEN_DIM + 1) + k;
            m_pwScreenXY[j*SCREEN_DIM*6+k*6+1] = (j    ) * (SCREEN_DIM + 1) + k;
            m_pwScreenXY[j*SCREEN_DIM*6+k*6+2] = (j    ) * (SCREEN_DIM + 1) + k + 1;
            m_pwScreenXY[j*SCREEN_DIM*6+k*6+3] = (j + 1) * (SCREEN_DIM + 1) + k;
            m_pwScreenXY[j*SCREEN_DIM*6+k*6+4] = (j    ) * (SCREEN_DIM + 1) + k + 1;
            m_pwScreenXY[j*SCREEN_DIM*6+k*6+5] = (j + 1) * (SCREEN_DIM + 1) + k + 1;
        }

        for (k = 0; k < SCREEN_DIM / 2; k++) {

            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+0] = (j + 1) * (SCREEN_DIM / 2 + 1) + k;
            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+1] = (j    ) * (SCREEN_DIM / 2 + 1) + k;
            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+2] = (j    ) * (SCREEN_DIM / 2 + 1) + k + 1;
            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+3] = (j + 1) * (SCREEN_DIM / 2 + 1) + k;
            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+4] = (j    ) * (SCREEN_DIM / 2 + 1) + k + 1;
            m_pwScreenYZ[j*SCREEN_DIM/2*6+k*6+5] = (j + 1) * (SCREEN_DIM / 2 + 1) + k + 1;
        }
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
BOOL CLightType::Setup() {

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
BOOL CLightType::Initialize() {

    HRESULT hr;
    UINT    i, j;

    if (!SetMaterial(m_pDevice, RGB_MAKE(255, 255, 255))) {
        return FALSE;
    }

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"));
    if (!m_pd3dt) { 
        return FALSE;
    }

    if (m_bStress) {

        m_plc = new LIGHTCLUSTER[STRESS_CLUSTERS];
        if (!m_plc) {
            return FALSE;
        }
        memset(m_plc, 0, STRESS_CLUSTERS * sizeof(LIGHTCLUSTER));

        for (i = 0; i < STRESS_CLUSTERS; i++) {

            m_plc[i].plight = new D3DLIGHT8[STRESS_MAX_LIGHTS_PER_CLUSTER];
            if (!m_plc[i].plight) {
                return FALSE;
            }
            memset(m_plc[i].plight, 0, STRESS_MAX_LIGHTS_PER_CLUSTER * sizeof(D3DLIGHT8));

            m_plc[i].uNumLights = 0;
            m_plc[i].uDecay = 0;
        }
    }
    else {

        for (i = 0; i < m_uNumClusters; i++) {
            for (j = 0; j < m_plc[i].uNumLights; j++) {
                hr = m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
            }
        }
    }

    m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
    m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

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
void CLightType::Efface() {

    UINT i;

    if (m_bStress) {
        if (m_plc) {
            for (i = 0; i < STRESS_CLUSTERS; i++) {
                if (m_plc[i].plight) {
                    delete [] m_plc[i].plight;
                }
            }
            delete [] m_plc;
        }
    }

    ReleaseTexture(m_pd3dt);

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
void CLightType::Update() {

    if ((GetStartupContext() & TSTART_HARNESS) && !m_bStress) {
        UINT uFrame = (UINT)m_fFrame - 1;
        if (!(uFrame % 240)) {
            switch (uFrame / 240) {
                case 0:
                case 1:
                case 2:
                    UpdateAttenuation(uFrame / 240);
                    break;
                case 3:
                    UpdateAttenuation(0);
                    UpdateLightColor(TRUE);
                    break;
                case 4:
                    UpdateLightColor(FALSE);
                    m_bTexture = !m_bTexture;
                    break;
                case 5:
                    m_bTexture = !m_bTexture;
                    m_bColorVertex = !m_bColorVertex;
                    break;
            }
        }
        m_fScreenDepth = (1.0f - (float)sin((m_fFrame - 1.0f) / 240.0f * M_2PI)) * (LIGHT_RANGE / 2.0f);
    }
    else {
        m_fScreenDepth += m_fDepthOffset;
        if (m_fScreenDepth > LIGHT_RANGE) {
            m_fScreenDepth = LIGHT_RANGE;
        }
        else if (m_fScreenDepth < 0.0f) {
            m_fScreenDepth = 0.0f;
        }
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
BOOL CLightType::Render() {

    D3DXMATRIX  mWorld;
    D3DCOLOR    cAmbient;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    UINT        i, j;

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
    m_pDevice->SetVertexShader(FVF_CLVERTEX);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, m_bLighting);
    m_pDevice->SetRenderState(D3DRS_COLORVERTEX, m_bColorVertex);

    for (i = 0; i < m_uNumClusters; i++) {

        if (m_bStress) {

            // Update any decayed clusters mid-scene
            if (m_plc[i].uDecay == 0) {
                UpdateCluster(&m_plc[i]);
                m_plc[i].uDecay = MIN_DECAY + rand() % (MAX_DECAY - MIN_DECAY) + 1;
                for (j = 0; j < m_plc[i].uNumLights; j++) {
                    m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                }
            }
            else {
                m_plc[i].uDecay--;
            }
        }

        mWorld = m_mIdentity;
        mWorld._41 = i * (LIGHT_RANGE * 2.5f);
        mWorld._43 = m_fScreenDepth;

        // Enable the lights in the current cluster
        for (j = 0; j < m_plc[i].uNumLights; j++) {
            hr = m_pDevice->LightEnable(i * MAX_LIGHTS_PER_CLUSTER + j, TRUE);
            ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
        }

        m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

        if (m_plc[i].bAmbient) {
            cAmbient = m_cAmbient;
        }
        else {
            cAmbient = RGB_MAKE(0, 0, 0);
        }

        // Set ambient lighting
        hr = m_pDevice->SetRenderState(D3DRS_AMBIENT, (DWORD)cAmbient);
        ResultFailed(hr, TEXT("IDirect3DDevice8::SetLightState"));

        if (m_bStress) {
            // Set specular
            m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)m_plc[i].bSpecular);
        }
        else {
            m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, m_bSpecular);
        }

        if (m_bTexture) {
            m_pDevice->SetTexture(0, m_pd3dt);
        }

        // Render
        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_uNumVerticesXY, m_uNumIndicesXY / 3,
                m_pwScreenXY, D3DFMT_INDEX16, m_prScreenXY, sizeof(CLVERTEX));

        mWorld._43 = 0.0f;

        m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);

        if (m_bTexture) {
            m_pDevice->SetTexture(0, NULL);
        }

        // Render
        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_uNumVerticesYZ, m_uNumIndicesYZ / 3,
                m_pwScreenYZ, D3DFMT_INDEX16, m_prScreenYZ, sizeof(CLVERTEX));

        // Disable the lights in the current cluster
        for (j = 0; j < m_plc[i].uNumLights; j++) {
            hr = m_pDevice->LightEnable(i * MAX_LIGHTS_PER_CLUSTER + j, FALSE);
            ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
        }
    }

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
void CLightType::ProcessInput() {

    static UINT uFMDisplay = 0;

    if ((GetStartupContext() & TSTART_HARNESS) && !m_bStress) {
        return;
    }

    m_pDisplay->GetJoyState(&m_jsJoys, &m_jsLast);

#ifdef UNDER_XBOX
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_BACK)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
            m_bQuit = TRUE;
        }
    }
#endif // UNDER_XBOX

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_X)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_X)) {
            m_fDepthOffset = LIGHT_RANGE / 40.0f;
        }
        else {
            m_fDepthOffset = 0.0f;
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_Y)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_Y)) {
            m_fDepthOffset = -LIGHT_RANGE / 40.0f;
        }
        else {
            m_fDepthOffset = 0.0f;
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_WHITE)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_WHITE)) {
            if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                UpdateAttenuation(m_uAttenuation + 1);
            }
            else {
                m_pDisplay->SetCamera(&m_camInitial);
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_A)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_A)) {
            if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                UpdateLightColor(!m_bColored);
            }
            else {
                m_bColorVertex = !m_bColorVertex;
            }
        }
    }

    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_B)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_B)) {
            if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_START)) {
                m_bTexture = !m_bTexture;
            }
        }
    }

/*
    if (BUTTON_CHANGED(m_jsJoys, m_jsLast, JOYBUTTON_E)) {
        if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_E)) {
            if (!BUTTON_PRESSED(m_jsJoys, JOYBUTTON_SELECT)) {
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
    }
*/
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
void CLightType::UpdateAttenuation(UINT uAttenuation) {

    UINT    i, j;
    HRESULT hr;

    if (uAttenuation > 2) {
        uAttenuation = 0;
    }

    for (i = 0; i < m_uNumClusters; i++) {

        for (j = 0; j < m_plc[i].uNumLights; j++) {

            m_plc[i].plight[j].Attenuation0 = (uAttenuation == 0) ? 1.0f : 0.0f;
            m_plc[i].plight[j].Attenuation1 = (uAttenuation == 1) ? 1.0f : 0.0f;
            m_plc[i].plight[j].Attenuation2 = (uAttenuation == 2) ? 1.0f : 0.0f;

            // Set the light
            if (m_pDevice) {
                hr = m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
            }
        }
    }

    m_uAttenuation = uAttenuation;
}

//******************************************************************************
void CLightType::UpdateLightColor(bool bColored) {

    UINT    i, j;
    HRESULT hr;

    m_cAmbient = bColored ? RGB_MAKE(64, 64, 0) : RGB_MAKE(64, 64, 64);

    for (i = 0; i < m_uNumClusters; i++) {

        for (j = 0; j < m_plc[i].uNumLights; j++) {

            if (bColored) {
                m_plc[i].plight[j].Diffuse.r = (m_plc[i].plight[j].Type == D3DLIGHT_DIRECTIONAL || m_plc[i].plight[j].Type == D3DLIGHT_SPOT) ? 0.0f : 1.0f;
                m_plc[i].plight[j].Diffuse.g = (m_plc[i].plight[j].Type == D3DLIGHT_DIRECTIONAL) ? 1.0f : 0.0f;
                m_plc[i].plight[j].Diffuse.b = (m_plc[i].plight[j].Type == D3DLIGHT_SPOT) ? 1.0f : 0.0f;
                m_plc[i].plight[j].Specular.r = m_plc[i].plight[j].Diffuse.r;
                m_plc[i].plight[j].Specular.g = m_plc[i].plight[j].Diffuse.g;
                m_plc[i].plight[j].Specular.b = m_plc[i].plight[j].Diffuse.b;
                m_plc[i].plight[j].Ambient.r = m_plc[i].plight[j].Diffuse.r;
                m_plc[i].plight[j].Ambient.g = m_plc[i].plight[j].Diffuse.g;
                m_plc[i].plight[j].Ambient.b = m_plc[i].plight[j].Diffuse.b;
            }
            else {
                m_plc[i].plight[j].Diffuse.r = 1.0f;
                m_plc[i].plight[j].Diffuse.g = 1.0f;
                m_plc[i].plight[j].Diffuse.b = 1.0f;
                m_plc[i].plight[j].Specular.r = 1.0f;
                m_plc[i].plight[j].Specular.g = 1.0f;
                m_plc[i].plight[j].Specular.b = 1.0f;
                m_plc[i].plight[j].Ambient.r = 1.0f;
                m_plc[i].plight[j].Ambient.g = 1.0f;
                m_plc[i].plight[j].Ambient.b = 1.0f;
            }

            // Set the light
            if (m_pDevice) {
                hr = m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
            }
        }
    }

    m_bColored = bColored;
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
BOOL CLightType::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    if (GetStartupContext() & TSTART_HARNESS) {
        m_camInitial.vPosition = D3DXVECTOR3((1.25f * LIGHT_RANGE * (float)m_uNumClusters - 0.25f * LIGHT_RANGE) * 4.0f, 0.0f, -35.0f * (float)m_uNumClusters);
    }
    else {
        m_camInitial.vPosition = D3DXVECTOR3(1.25f * LIGHT_RANGE * (float)m_uNumClusters - 0.25f * LIGHT_RANGE, 0.0f, -35.0f * (float)m_uNumClusters);
    }
    m_camInitial.vInterest     = D3DXVECTOR3(1.25f * LIGHT_RANGE * (float)m_uNumClusters - 0.25f * LIGHT_RANGE, 0.0f, 0.0f);
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
BOOL CLightType::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case 0x5A:
                    m_fDepthOffset = LIGHT_RANGE / 40.0f;
                    return 0;

                case 0x58:
                    m_fDepthOffset = -LIGHT_RANGE / 40.0f;
                    return 0;

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;

        case WM_KEYUP:
            
            switch (wParam) {

                case 0x5A:
                case 0x58:
                    m_fDepthOffset = 0.0f;
                    return 0;

                case VK_F7:   // Toggle lighting
                    m_bSpecular = !m_bSpecular;
                    return 0;

                case VK_F8:   // Toggle lighting
                    m_bLighting = !m_bLighting;
                    return 0;

                case VK_F9:     // Toggle texturing
                    m_bTexture = !m_bTexture;
                    return 0;
            }

            break;

        case WM_CHAR:

            switch (wParam) {

                case TEXT('C'):
                case TEXT('c'): // Toggle colored vertices
                    m_bColorVertex = !m_bColorVertex;
                    return 0;

                case TEXT('V'):
                case TEXT('v'): {   // Toggle between white and colored lights
                    UINT    i, j;
                    HRESULT hr;

                    m_bColored = !m_bColored;
                    m_cAmbient = m_bColored ? RGB_MAKE(64, 64, 0) : RGB_MAKE(64, 64, 64);

                    for (i = 0; i < m_uNumClusters; i++) {

                        for (j = 0; j < m_plc[i].uNumLights; j++) {

                            if (m_bColored) {
                                m_plc[i].plight[j].Diffuse.r = (m_plc[i].plight[j].Type == D3DLIGHT_DIRECTIONAL || m_plc[i].plight[j].Type == D3DLIGHT_SPOT) ? 0.0f : 1.0f;
                                m_plc[i].plight[j].Diffuse.g = (m_plc[i].plight[j].Type == D3DLIGHT_DIRECTIONAL) ? 1.0f : 0.0f;
                                m_plc[i].plight[j].Diffuse.b = (m_plc[i].plight[j].Type == D3DLIGHT_SPOT) ? 1.0f : 0.0f;
                                m_plc[i].plight[j].Specular.r = m_plc[i].plight[j].Diffuse.r;
                                m_plc[i].plight[j].Specular.g = m_plc[i].plight[j].Diffuse.g;
                                m_plc[i].plight[j].Specular.b = m_plc[i].plight[j].Diffuse.b;
                                m_plc[i].plight[j].Ambient.r = m_plc[i].plight[j].Diffuse.r;
                                m_plc[i].plight[j].Ambient.g = m_plc[i].plight[j].Diffuse.g;
                                m_plc[i].plight[j].Ambient.b = m_plc[i].plight[j].Diffuse.b;
                            }
                            else {
                                m_plc[i].plight[j].Diffuse.r = 1.0f;
                                m_plc[i].plight[j].Diffuse.g = 1.0f;
                                m_plc[i].plight[j].Diffuse.b = 1.0f;
                                m_plc[i].plight[j].Specular.r = 1.0f;
                                m_plc[i].plight[j].Specular.g = 1.0f;
                                m_plc[i].plight[j].Specular.b = 1.0f;
                                m_plc[i].plight[j].Ambient.r = 1.0f;
                                m_plc[i].plight[j].Ambient.g = 1.0f;
                                m_plc[i].plight[j].Ambient.b = 1.0f;
                            }

                            // Set the light
                            if (m_pDevice) {
                                hr = m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                                ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
                            }
                        }
                    }
                    return 0;
                }

                case TEXT('A'):
                case TEXT('a'): {   // Toggle between contant, linear, and quadratic attenuation
                    UINT    i, j;
                    HRESULT hr;

                    if (++m_uAttenuation > 2) {
                        m_uAttenuation = 0;
                    }

                    for (i = 0; i < m_uNumClusters; i++) {

                        for (j = 0; j < m_plc[i].uNumLights; j++) {

                            m_plc[i].plight[j].Attenuation0 = (m_uAttenuation == 0) ? 1.0f : 0.0f;
                            m_plc[i].plight[j].Attenuation1 = (m_uAttenuation == 1) ? 1.0f : 0.0f;
                            m_plc[i].plight[j].Attenuation2 = (m_uAttenuation == 2) ? 1.0f : 0.0f;

                            // Set the light
                            if (m_pDevice) {
                                hr = m_pDevice->SetLight(i * MAX_LIGHTS_PER_CLUSTER + j, &m_plc[i].plight[j]);
                                ResultFailed(hr, TEXT("IDirect3DDevice8::LightEnable"));
                            }
                        }
                    }
                    return 0;
                }
            }

            break;
    }

#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}
