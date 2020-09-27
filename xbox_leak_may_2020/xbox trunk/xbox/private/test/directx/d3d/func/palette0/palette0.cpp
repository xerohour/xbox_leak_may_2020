/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    palette0.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "palette0.h"
#include "resource.h"

static CTexture8* CreateMipmap(CDevice8* pDevice, UINT uTexBase, PALETTECOLOR* ppc, float fAlpha);
static BOOL SwapTexturePalettes(CTexture8* pd3dt, PALETTECOLOR* ppcDst, D3DPALETTESIZE palsDst,
                                  PALETTECOLOR* ppcSrc);
static void RandomizePalette(PALETTECOLOR* ppc, D3DPALETTESIZE pals);
static void InitPalette(PALETTECOLOR* ppc, D3DPALETTESIZE pals, float fAlpha);

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

    CMPalette* pMPalette;
    BOOL       bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMPalette = new CMPalette();
    if (!pMPalette) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMPalette->Create(pDisplay)) {
        pMPalette->Release();
        return FALSE;
    }

    bRet = pMPalette->Exhibit(pnExitCode);

    // Clean up the scene
    pMPalette->Release();

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
// CMPalette
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMPalette
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
CMPalette::CMPalette() {

    m_fOffset = -0.002f;
    m_mScale = m_mIdentity;

    // Initialize the vertices
    m_prVertices[0] = VERTEX(D3DXVECTOR3(-2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prVertices[1] = VERTEX(D3DXVECTOR3(-2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prVertices[2] = VERTEX(D3DXVECTOR3( 2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prVertices[3] = VERTEX(D3DXVECTOR3( 2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);
}

//******************************************************************************
//
// Method:
//
//     ~CMPalette
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
CMPalette::~CMPalette() {
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
BOOL CMPalette::Create(CDisplay* pDisplay) {

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
int CMPalette::Exhibit(int *pnExitCode) {

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
BOOL CMPalette::Prepare() {

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
BOOL CMPalette::Setup() {

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
BOOL CMPalette::Initialize() {

    D3DMATERIAL8    material;
    PALETTECOLOR    palSrc[256], palDst[256];
    UINT            i, j;
    D3DPALETTESIZE  pals[4] = {
                        D3DPALETTE_256,
                        D3DPALETTE_128,
                        D3DPALETTE_64,
                        D3DPALETTE_32
                    };

    if (!CScene::Initialize()) {
        return FALSE;
    }

    SetMaterial(&material, RGB_MAKE(255, 255, 255));
    SetMaterialTranslucency(&material, 0.85f);
    m_pDevice->SetMaterial(&material);

    for (i = 0; i < 4; i++) {

        for (j = 0; j < 4; j++) {

            m_pd3dt[i][j] = CreateMipmap(m_pDevice, i, palSrc, 0.85f);
            if (!m_pd3dt[i][j]) {
                return FALSE;
            }

            if (j) {
                InitPalette(palDst, pals[j], 0.85f);
            }
            else {
                memcpy(palDst, palSrc, 256 * sizeof(D3DCOLOR));
            }

            RandomizePalette(palDst, pals[j]);

            if (!SwapTexturePalettes(m_pd3dt[i][j], palDst, pals[j], palSrc)) {
                return FALSE;
            }

            m_pd3dp[i][j] = CreatePalette(m_pDevice, palDst, pals[j]);
            if (!m_pd3dp[i][j]) {
                return FALSE;
            }
        }
    }

    for (i = 0; i < 4; i++) {
	    m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, 0);
    }

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    for (i = 1; i < 4; i++) {
        SetColorStage(m_pDevice, i, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
        SetAlphaStage(m_pDevice, i, D3DTA_TEXTURE, D3DTA_CURRENT, D3DTOP_MODULATE);
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
void CMPalette::Efface() {

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
void CMPalette::Update() {

    UINT i;

    for (i = 0; i < 3; i++) {
        m_mScale(i, i) += m_fOffset * m_fTimeDilation;
    }
    if (m_mScale(0, 0) > 1.0f) {
        m_fOffset = -0.002f;
    }
    else if (m_mScale(0, 0) < 0.05) {
        m_fOffset = 0.002f;
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
BOOL CMPalette::Render() {

    D3DXMATRIX  mWorld, mProduct;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT     hr;
    UINT        i, j, k;

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

    mWorld = m_mIdentity;

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    for (i = 0; i < 4; i++) {
        m_pDevice->SetPalette(i, m_pd3dp[i][0]);
        m_pDevice->SetTexture(i, m_pd3dt[i][0]);
    }

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    for (j = 0; j < 2; j++) {

        mWorld._42 = 2.75f - j * 5.5f;
        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, j);

        for (i = 0; i < 4; i++) {

            mWorld._41 = -8.5f + i * 5.5f;

            D3DXMatrixMultiply(&mProduct, &m_mScale, &mWorld);
            m_pDevice->SetTransform(D3DTS_WORLD, &mProduct);

            for (k = 0; k < 4; k++) {
                m_pDevice->SetPalette(k, m_pd3dp[k][(k+i)%4]);
                m_pDevice->SetTexture(k, m_pd3dt[k][(k+i)%4]);
            }

            // Render
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(VERTEX));
        }
    }

    for (i = 0; i < 4; i++) {
        m_pDevice->SetPalette(i, NULL);
        m_pDevice->SetTexture(i, NULL);
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
void CMPalette::ProcessInput() {

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
BOOL CMPalette::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -30.0f);
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
BOOL CMPalette::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
CTexture8* CreateMipmap(CDevice8* pDevice, UINT uTexBase, PALETTECOLOR* ppc, float fAlpha) {

    CTexture8   *pd3dt, *pd3dtTemp;
    CSurface8   *pd3ds, *pd3dsTemp;
    UINT        i, uLevels;
    PALETTEENTRY ppeMain[256], ppeSub[256]; 
    PALETTECOLOR ppcSub[256];
    HRESULT     hr;
    LPTSTR      szTexture[4][6] = {
                    TEXT("brick0_1.bmp"),
                    TEXT("brick0_2.bmp"),
                    TEXT("brick0_3.bmp"),
                    TEXT("brick0_4.bmp"),
                    TEXT("brick0_5.bmp"),
                    TEXT("brick0_6.bmp"),
                    TEXT("brick1_1.bmp"),
                    TEXT("brick1_2.bmp"),
                    TEXT("brick1_3.bmp"),
                    TEXT("brick1_4.bmp"),
                    TEXT("brick1_5.bmp"),
                    TEXT("brick1_6.bmp"),
                    TEXT("brick2_1.bmp"),
                    TEXT("brick2_2.bmp"),
                    TEXT("brick2_3.bmp"),
                    TEXT("brick2_4.bmp"),
                    TEXT("brick2_5.bmp"),
                    TEXT("brick2_6.bmp"),
                    TEXT("brick3_1.bmp"),
                    TEXT("brick3_2.bmp"),
                    TEXT("brick3_3.bmp"),
                    TEXT("brick3_4.bmp"),
                    TEXT("brick3_5.bmp"),
                    TEXT("brick3_6.bmp"),
                };

    pd3dt = (CTexture8*)CreateTexture(pDevice, szTexture[uTexBase][0], D3DFMT_P8, TTYPE_MIPMAP, ppc, fAlpha);
    if (!pd3dt) {
        return NULL;
    }

    uLevels = pd3dt->GetLevelCount();

    for (i = 1; i < uLevels && i < 6; i++) {

        hr = pd3dt->GetSurfaceLevel(i, &pd3ds);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        pd3dtTemp = (CTexture8*)CreateTexture(pDevice, szTexture[uTexBase][i], D3DFMT_P8, TTYPE_TEXTURE, ppcSub);
        if (!pd3dtTemp) {
            pd3ds->Release();
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = pd3dtTemp->GetSurfaceLevel(0, &pd3dsTemp);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dtTemp);
            pd3ds->Release();
            ReleaseTexture(pd3dt);
            return NULL;
        }

        memcpy(ppeMain, ppc, 256 * sizeof(PALETTECOLOR));
        ConvertD3DCOLORToPALETTEENTRY(ppeMain, 256);
        memcpy(ppeSub, ppcSub, 256 * sizeof(PALETTECOLOR));
        ConvertD3DCOLORToPALETTEENTRY(ppeSub, 256);

        hr = D3DXLoadSurfaceFromSurface(pd3ds->GetIDirect3DSurface8(), ppeMain, NULL, pd3dsTemp->GetIDirect3DSurface8(), ppeSub, NULL, D3DX_FILTER_POINT, 0);

        pd3dsTemp->Release();
        ReleaseTexture(pd3dtTemp);
        pd3ds->Release();

        if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
            ReleaseTexture(pd3dt);
            return NULL;
        }
    }

    return pd3dt;
}

//******************************************************************************
void RandomizePalette(PALETTECOLOR* ppc, D3DPALETTESIZE pals) {

    D3DCOLOR c;
    UINT i;
    UINT uIndex, uEntries;

    switch (pals) {
        case D3DPALETTE_32:
            uEntries = 32;
            break;
        case D3DPALETTE_64:
            uEntries = 64;
            break;
        case D3DPALETTE_128:
            uEntries = 128;
            break;
        case D3DPALETTE_256:
        default:
            uEntries = 256;
            break;
    }

    // Randomize the palette entries
    for (i = 0; i < uEntries; i++) {
        uIndex = rand() % uEntries;
        c = ppc[uIndex];
        ppc[uIndex] = ppc[i];
        ppc[i] = c;
    }
}

//******************************************************************************
//
// Function:
//
//     InitPalette
//
// Description:
//
//     Initialize a given number of palette entries with evenly spaced
//     color values.
//
// Arguments:
//
//     LPPALETTEENTRY ppe              - Pointer to an array of palette entries
//                                       to intialize
//
//     UINT uNumEntries                - Number of palette entries to initialize
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
void InitPalette(PALETTECOLOR* ppc, D3DPALETTESIZE pals, float fAlpha) {

    UINT    a, r, g, b, mr, mg, mb, uEntries, i = 0;
    float   f;

    switch (pals) {
        case D3DPALETTE_32:
            uEntries = 32;
            break;
        case D3DPALETTE_64:
            uEntries = 64;
            break;
        case D3DPALETTE_128:
            uEntries = 128;
            break;
        case D3DPALETTE_256:
        default:
            uEntries = 256;
            break;
    }

    a = (BYTE)(fAlpha * 255.0f);

    for (mb = 0; (mb + 1) * (mb + 1) * (mb + 1) <= uEntries; mb++);
//    mg = (UINT)((float)uEntries / (float)mb);
//    for (mr = 0; (mr + 1) * (mr + 1) <= mg; mr++);
//    mg = (UINT)((float)mg / (float)mr);
    mr = mg = mb;
    f = 255.0f / (float)(mr - 1);

    for (r = 0; r < mr; r++) {
        for (g = 0; g < mg; g++) {
            for (b = 0; b < mb; b++) {
                ppc[i++] = D3DCOLOR_ARGB(a, (BYTE)(r * f), (BYTE)(g * f), (BYTE)(b * f));
            }
        }
    }
}

//******************************************************************************
BOOL SwapTexturePalettes(CTexture8* pd3dt, PALETTECOLOR* ppcDst, D3DPALETTESIZE palsDst, PALETTECOLOR* ppcSrc) {

    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;
    LPBYTE pTexel;
    UINT i, j, k, l, uEntries, uLevels;
    int dr, dg, db, d, dLast;
    BYTE  r, g, b, a, e;
    D3DCOLOR c;

    switch (palsDst) {
        case D3DPALETTE_32:
            uEntries = 32;
            break;
        case D3DPALETTE_64:
            uEntries = 64;
            break;
        case D3DPALETTE_128:
            uEntries = 128;
            break;
        case D3DPALETTE_256:
        default:
            uEntries = 256;
            break;
    }

    uLevels = pd3dt->GetLevelCount();

    for (l = 0; l < uLevels; l++) {

        pd3dt->GetLevelDesc(l, &d3dsd);

        pd3dt->LockRect(l, &d3dlr, NULL, 0);

        // Translate the surface bits
        pTexel = (LPBYTE)d3dlr.pBits;

        for (k = 0; k < d3dsd.Height; k++) {

            for (j = 0; j < d3dsd.Width; j++) {

                c = ppcSrc[pTexel[j]];

                r = (BYTE)RGBA_GETRED(c);
                g = (BYTE)RGBA_GETGREEN(c);
                b = (BYTE)RGBA_GETBLUE(c);
                a = (BYTE)RGBA_GETALPHA(c);

                dLast = INT_MAX;

                // Find the entry in the destination palette that most closely matches the
                // given pixel color
                for (i = 0; i < uEntries; i++) {

                    dr = (int)RGBA_GETRED(ppcDst[i]) - r;
                    dg = (int)RGBA_GETGREEN(ppcDst[i]) - g;
                    db = (int)RGBA_GETBLUE(ppcDst[i]) - b;

                    d = dr * dr + dg * dg + db * db;

                    if (d < dLast) {
                        e = (BYTE)i;
                        dLast = d;
                        if (d == 0) {
                            break;
                        }
                    }
                }

                pTexel[j] = e;
            }

            pTexel += d3dlr.Pitch;
        }

        pd3dt->UnlockRect(l);
    }

    return TRUE;
}
