/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    mstage.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "mstage.h"

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

    CMStage4* pMStage;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pMStage = new CMStage4();
    if (!pMStage) {
        return FALSE;
    }

    // Initialize the scene
    if (!pMStage->Create(pDisplay)) {
        pMStage->Release();
        return FALSE;
    }

    bRet = pMStage->Exhibit(pnExitCode);

    // Clean up the scene
    pMStage->Release();

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
// CMStage4
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CMStage4
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
CMStage4::CMStage4() {

    UINT i;

    // Initialize the vertices
    m_prVertices[0] = VERTEX(D3DXVECTOR3(-10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prVertices[1] = VERTEX(D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prVertices[2] = VERTEX(D3DXVECTOR3( 10.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prVertices[3] = VERTEX(D3DXVECTOR3( 10.0f,-10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);

    m_pd3dtBackgrnd = NULL;
    memset(m_pd3dt, 0, 8 * sizeof(CTexture8*));
}

//******************************************************************************
//
// Method:
//
//     ~CMStage4
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
CMStage4::~CMStage4() {
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
BOOL CMStage4::Create(CDisplay* pDisplay) {

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
int CMStage4::Exhibit(int *pnExitCode) {

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
BOOL CMStage4::Prepare() {

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
BOOL CMStage4::Setup() {

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
BOOL CMStage4::Initialize() {

    D3DMATERIAL8 material;
    UINT         i;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    SetMaterial(&material, RGB_MAKE(255, 255, 255));
    SetMaterialTranslucency(&material, 0.75f);
    m_pDevice->SetMaterial(&material);

    // Create textures
    m_pd3dtBackgrnd = (CTexture8*)CreateTexture(m_pDevice, TEXT("backgrnd.bmp"), D3DFMT_R5G6B5);
    if (!m_pd3dtBackgrnd) {
        return FALSE;
    }

    m_pd3dt[0] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture1.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[1] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture2.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[2] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture3.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[3] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture4.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[4] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture5.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[5] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture6.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[6] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture7.bmp"), D3DFMT_A4R4G4B4, TTYPE_TEXTURE, NULL, 0.75f);
    m_pd3dt[7] = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture8.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 0.75f);

    for (i = 0; i < 4; i++) {
        if (!m_pd3dt[i]) {
            return FALSE;
        }
    }

    for (i = 0; i < 4; i++) {
    	m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, 0);
    }

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
void CMStage4::Efface() {

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
void CMStage4::Update() {
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
BOOL CMStage4::Render() {

    DWORD                       dwClearFlags = D3DCLEAR_TARGET;
    HRESULT                     hr;
    UINT                        i, uTextures;
    DWORD                       dwTArg1, dwTArg2, dwTMod1, dwTMod2, dwTOp, dwAOp;
    DWORD                       dwTexArg[3] = {
                                    D3DTA_CURRENT,
                                    D3DTA_DIFFUSE,
                                    D3DTA_TEXTURE,
                                };
#ifndef UNDER_CE
    DWORD                       dwTexMod[2] = {
                                    0,
                                    D3DTA_COMPLEMENT,
                                };
    D3DTEXTUREOP                d3dtopColor[22] = {
                                    D3DTOP_DISABLE,
                                    D3DTOP_SELECTARG1,
                                    D3DTOP_SELECTARG2,
                                    D3DTOP_MODULATE,
                                    D3DTOP_MODULATE2X,
                                    D3DTOP_MODULATE4X,
                                    D3DTOP_ADD,
                                    D3DTOP_ADDSIGNED,
                                    D3DTOP_ADDSIGNED2X,
                                    D3DTOP_SUBTRACT,
                                    D3DTOP_ADDSMOOTH,
                                    D3DTOP_BLENDDIFFUSEALPHA,
                                    D3DTOP_BLENDTEXTUREALPHA,
                                    D3DTOP_BLENDFACTORALPHA,
                                    D3DTOP_BLENDTEXTUREALPHAPM,
                                    D3DTOP_BLENDCURRENTALPHA,
                                    D3DTOP_PREMODULATE,
                                    D3DTOP_DOTPRODUCT3,
                                    D3DTOP_MODULATEALPHA_ADDCOLOR,
                                    D3DTOP_MODULATECOLOR_ADDALPHA,
                                    D3DTOP_MODULATEINVALPHA_ADDCOLOR,
                                    D3DTOP_MODULATEINVCOLOR_ADDALPHA,
                                };
    D3DTEXTUREOP                d3dtopAlpha[22] = {
                                    D3DTOP_DISABLE,
                                    D3DTOP_SELECTARG1,
                                    D3DTOP_SELECTARG2,
                                    D3DTOP_MODULATE,
                                    D3DTOP_MODULATE2X,
                                    D3DTOP_MODULATE4X,
                                    D3DTOP_ADD,
                                    D3DTOP_ADDSIGNED,
                                    D3DTOP_ADDSIGNED2X,
                                    D3DTOP_SUBTRACT,
                                    D3DTOP_ADDSMOOTH,
                                    D3DTOP_BLENDDIFFUSEALPHA,
                                    D3DTOP_BLENDTEXTUREALPHA,
                                    D3DTOP_BLENDFACTORALPHA,
                                    D3DTOP_BLENDTEXTUREALPHAPM,
                                    D3DTOP_BLENDCURRENTALPHA,
                                    D3DTOP_PREMODULATE,
                                    D3DTOP_DOTPRODUCT3,
                                    D3DTOP_DISABLE,
                                    D3DTOP_DISABLE,
                                    D3DTOP_DISABLE,
                                    D3DTOP_DISABLE,
                                };
    LPTSTR                      pszTopColor[22] = {
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_SELECTARG1")},
                                    {TEXT("D3DTOP_SELECTARG2")},
                                    {TEXT("D3DTOP_MODULATE")},
                                    {TEXT("D3DTOP_MODULATE2X")},
                                    {TEXT("D3DTOP_MODULATE4X")},
                                    {TEXT("D3DTOP_ADD")},
                                    {TEXT("D3DTOP_ADDSIGNED")},
                                    {TEXT("D3DTOP_ADDSIGNED2X")},
                                    {TEXT("D3DTOP_SUBTRACT")},
                                    {TEXT("D3DTOP_ADDSMOOTH")},
                                    {TEXT("D3DTOP_BLENDDIFFUSEALPHA")},
                                    {TEXT("D3DTOP_BLENDTEXTUREALPHA")},
                                    {TEXT("D3DTOP_BLENDFACTORALPHA")},
                                    {TEXT("D3DTOP_BLENDTEXTUREALPHAPM")},
                                    {TEXT("D3DTOP_BLENDCURRENTALPHA")},
                                    {TEXT("D3DTOP_PREMODULATE")},
                                    {TEXT("D3DTOP_DOTPRODUCT3")},
                                    {TEXT("D3DTOP_MODULATEALPHA_ADDCOLOR")},
                                    {TEXT("D3DTOP_MODULATECOLOR_ADDALPHA")},
                                    {TEXT("D3DTOP_MODULATEINVALPHA_ADDCOLOR")},
                                    {TEXT("D3DTOP_MODULATEINVCOLOR_ADDALPHA")},
                                };
    LPTSTR                      pszTopAlpha[22] = {
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_SELECTARG1")},
                                    {TEXT("D3DTOP_SELECTARG2")},
                                    {TEXT("D3DTOP_MODULATE")},
                                    {TEXT("D3DTOP_MODULATE2X")},
                                    {TEXT("D3DTOP_MODULATE4X")},
                                    {TEXT("D3DTOP_ADD")},
                                    {TEXT("D3DTOP_ADDSIGNED")},
                                    {TEXT("D3DTOP_ADDSIGNED2X")},
                                    {TEXT("D3DTOP_SUBTRACT")},
                                    {TEXT("D3DTOP_ADDSMOOTH")},
                                    {TEXT("D3DTOP_BLENDDIFFUSEALPHA")},
                                    {TEXT("D3DTOP_BLENDTEXTUREALPHA")},
                                    {TEXT("D3DTOP_BLENDFACTORALPHA")},
                                    {TEXT("D3DTOP_BLENDTEXTUREALPHAPM")},
                                    {TEXT("D3DTOP_BLENDCURRENTALPHA")},
                                    {TEXT("D3DTOP_PREMODULATE")},
                                    {TEXT("D3DTOP_DOTPRODUCT3")},
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_DISABLE")},
                                };
    LPTSTR                      pszTexArg[4] = {
                                    {TEXT("D3DTA_CURRENT")},
                                    {TEXT("D3DTA_DIFFUSE")},
                                    {TEXT("D3DTA_TEXTURE")},
                                };
    LPTSTR                      pszTexMod[2] = {
                                    {TEXT("no modifier")},
                                    {TEXT("D3DTA_COMPLEMENT")},
                                };
#else

    DWORD                       dwTexMod[1] = {
                                    0,
                                };
    D3DTEXTUREOP                d3dtopColor[8] = {
                                    D3DTOP_DISABLE,
                                    D3DTOP_SELECTARG1,
                                    D3DTOP_SELECTARG2,
                                    D3DTOP_MODULATE,
                                    D3DTOP_MODULATE2X,
                                    D3DTOP_MODULATE4X,
                                    D3DTOP_ADD,
                                    D3DTOP_SUBTRACT,
                                };
    D3DTEXTUREOP                d3dtopAlpha[8] = {
                                    D3DTOP_DISABLE,
                                    D3DTOP_SELECTARG1,
                                    D3DTOP_SELECTARG2,
                                    D3DTOP_MODULATE,
                                    D3DTOP_MODULATE2X,
                                    D3DTOP_MODULATE4X,
                                    D3DTOP_ADD,
                                    D3DTOP_SUBTRACT,
                                };
    LPTSTR                      pszTopColor[8] = {
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_SELECTARG1")},
                                    {TEXT("D3DTOP_SELECTARG2")},
                                    {TEXT("D3DTOP_MODULATE")},
                                    {TEXT("D3DTOP_MODULATE2X")},
                                    {TEXT("D3DTOP_MODULATE4X")},
                                    {TEXT("D3DTOP_ADD")},
                                    {TEXT("D3DTOP_SUBTRACT")},
                                };
    LPTSTR                      pszTopAlpha[8] = {
                                    {TEXT("D3DTOP_DISABLE")},
                                    {TEXT("D3DTOP_SELECTARG1")},
                                    {TEXT("D3DTOP_SELECTARG2")},
                                    {TEXT("D3DTOP_MODULATE")},
                                    {TEXT("D3DTOP_MODULATE2X")},
                                    {TEXT("D3DTOP_MODULATE4X")},
                                    {TEXT("D3DTOP_ADD")},
                                    {TEXT("D3DTOP_SUBTRACT")},
                                };
    LPTSTR                      pszTexArg[4] = {
                                    {TEXT("D3DTA_CURRENT")},
                                    {TEXT("D3DTA_DIFFUSE")},
                                    {TEXT("D3DTA_TEXTURE")},
                                };
    LPTSTR                      pszTexMod[1] = {
                                    {TEXT("no modifier")},
                                };

#endif // !UNDER_CE

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
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dtBackgrnd);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);

    uTextures = rand() % 4 + 1;

    for (i = 0; i < uTextures; i++) {
        // Set the texture
        m_pDevice->SetTexture(i, m_pd3dt[i]);

        // Set the blend mode
        dwTArg1 = rand() % 3;
//            do {
            dwTArg2 = rand() % 2;
//            } while (!dwTArg1 && !dwTArg2);
        if (dwTArg2 == dwTArg1) {
            dwTArg2 = !dwTArg2;
        }
        if (!i && dwTArg1 != 2) {
            dwTArg1 = 2;
        }
#ifndef UNDER_CE
        dwTMod1 = rand() % 2;
        dwTMod2 = rand() % 2;
        dwTOp = rand() % 18;//uFrame % 22;
        dwAOp = dwTOp;//uFrame % 22;
#else
        dwTMod1 = 0;
        dwTMod2 = 0;
        if (!i) {
            dwTOp = rand() % 4;//uFrame % 4;
            dwAOp = rand() % 4;
            if (dwAOp) {
                dwAOp = dwTOp;
            }
        }
        else {
            dwTOp = rand() % 8;//uFrame % 4;
            dwAOp = rand() % 4;
            if (dwAOp) {
                dwAOp = dwTOp;
            }
        }
#endif // UNDER_CE
        if (!dwTOp && !i) {
            dwTOp = 3;
        }

/*
        dwTArg1 = 2;
        dwTArg2 = 1;
        dwTMod2 = dwTMod1 = 0;
        dwTOp = uFrame % 22;
        dwAOp = 0;
        if (!dwTOp && !i) {
            dwTOp = 3;
        }
dwTOp = 4;
dwAOp = dwTOp;
//            if (dwTOp == 4 || dwTOp == 5 || dwTOp == 6 || dwTOp == 7 || dwTOp == 8 || dwTOp == 9 || dwTOp == 10 || dwTOp == 11 || dwTOp == 14 || dwTOp == 15 || dwTOp == 17 || dwTOp == 20 || dwTOp == 21) {
//                dwTOp = 3;
//            }
DebugBreak();
*/
        SetColorStage(m_pDevice, i, dwTexArg[dwTArg1] | dwTexMod[dwTMod1], dwTexArg[dwTArg2] | dwTexMod[dwTMod2], d3dtopColor[dwTOp]);
        SetAlphaStage(m_pDevice, i, dwTexArg[dwTArg1] | dwTexMod[dwTMod1], dwTexArg[dwTArg2] | dwTexMod[dwTMod2], d3dtopAlpha[dwAOp]);

        DebugString(TEXT("Stage %d - Color: %s | %s, %s | %s, %s"), i, pszTexArg[dwTArg1], pszTexMod[dwTMod1], pszTexArg[dwTArg2], pszTexMod[dwTMod2], pszTopColor[dwTOp]);
        DebugString(TEXT("Stage %d - Alpha: %s | %s, %s | %s, %s"), i, pszTexArg[dwTArg1], pszTexMod[dwTMod1], pszTexArg[dwTArg2], pszTexMod[dwTMod2], pszTopAlpha[dwAOp]);
    }

    // Render
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(VERTEX));

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);

    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);
    m_pDevice->SetTexture(2, NULL);
    m_pDevice->SetTexture(3, NULL);
//    m_pDevice->SetTexture(4, NULL);
//    m_pDevice->SetTexture(5, NULL);
//    m_pDevice->SetTexture(6, NULL);
//    m_pDevice->SetTexture(7, NULL);

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
void CMStage4::ProcessInput() {

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
BOOL CMStage4::InitView() {

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
BOOL CMStage4::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
