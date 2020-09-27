/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    snow.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dbase.h"
#include "snow.h"

#ifndef UNDER_XBOX
#define SPRITE_STAGE 0
#else
#define SPRITE_STAGE 3
#endif

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

    CSnow*   pSnow;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pSnow = new CSnow();
    if (!pSnow) {
        return FALSE;
    }

    // Initialize the scene
    if (!pSnow->Create(pDisplay)) {
        pSnow->Release();
        return FALSE;
    }

    bRet = pSnow->Exhibit(pnExitCode);

    // Clean up the scene
    pSnow->Release();

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
//     LPDIRECT3D8 pd3d                 - Pointer to the Direct3D object
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
// CSnow
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CSnow
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
CSnow::CSnow() {

    m_pd3dtBackground = NULL;
    m_pd3dtSnowFlake = NULL;
    m_bFade = TRUE;
    m_pprSnowMesh = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CSnow
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
CSnow::~CSnow() {
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
BOOL CSnow::Create(CDisplay* pDisplay) {

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
int CSnow::Exhibit(int *pnExitCode) {

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
BOOL CSnow::Setup() {

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
BOOL CSnow::Initialize() {

    float                       fTrigRatio,
                                fAspect;
    D3DCOLOR                    cSnowColorKey;
    UINT                        i, j, k;

    float                       fCellWidth, fCellHeight, fCellDepth;
    float                       fWidth, fHeight, fDepth;
    UINT                        uCellFlakes;
    UINT                        uCellsX, uCellsY, uCell;

    float                       fTU, fTV,
                                fUOffset, fVOffset,
                                fRefWidth,
                                fRefHeight;
    float                       fBackDepth;

    if (!CScene::Initialize()) {
        return FALSE;
    }

    m_uNumCells = 0;

    D3DXMatrixIdentity(&m_mWorld);

    // Calculate a snow cell's dimensions
    fAspect = (float)(m_pDisplay->GetHeight()) / (float)(m_pDisplay->GetWidth());
    fTrigRatio = (float)(tan(M_PI / (float)(8.0f)));
    fCellDepth = (DEPTH_BACK_PLANE - DEPTH_FRONT_PLANE) / (float)(CELLS_Z);
    fCellWidth = DEPTH_BACK_PLANE * fTrigRatio * 2.0f / (float)(CELLS_X);
    fCellHeight = DEPTH_BACK_PLANE * fTrigRatio * fAspect * 2.0f / (float)(CELLS_Y);

    // Calculate the number of cells needed
    for (i = 0, fDepth = DEPTH_FRONT_PLANE + CELLS_Z * fCellDepth; 
         i < CELLS_Z; 
         i++, fDepth -= fCellDepth) 
    {
        fWidth = fDepth * fTrigRatio * 2;
        fHeight = fWidth * fAspect;
        m_uNumCells += ((UINT)(fWidth / fCellWidth) + 2) * 
                     ((UINT)(fHeight / fCellHeight) + 2);
    }

    // Allocate memory
    m_pscList = (PSNOWCELL)MemAlloc(sizeof(SNOWCELL) * m_uNumCells);
    if (!m_pscList) {
        return FALSE;
    }
    uCellFlakes = NUM_FLAKES / m_uNumCells + 1;

    m_pprSnowMesh = (D3DXVECTOR3**)MemAlloc(sizeof(D3DXVECTOR3*) * m_uNumCells);
    if (!m_pprSnowMesh) {
        return FALSE;
    }
    memset(m_pprSnowMesh, 0, m_uNumCells * sizeof(D3DXVECTOR3*));
    m_uNumSnowVertices = uCellFlakes;
    for (i = 0; i < m_uNumCells; i++) {
        m_pprSnowMesh[i] = (D3DXVECTOR3*)MemAlloc32(m_uNumSnowVertices * sizeof(D3DXVECTOR3));
        if (!m_pprSnowMesh[i]) {
            return FALSE;
        }
    }

    // Initialize the snow meshes
    m_fFlakeDim = fCellHeight / 8.0f;
    for (i = 0; i < m_uNumCells; i++) {
        for (j = 0; j < m_uNumSnowVertices; j++) {
            m_pprSnowMesh[i][j] = D3DXVECTOR3(
                                            RND() * fCellWidth + m_fFlakeDim / 2.0f, 
                                            RND() * -fCellHeight - m_fFlakeDim / 2.0f, 
                                            RND() * -fCellDepth);
        }
    }

    m_ppd3drSnowMesh = (LPDIRECT3DVERTEXBUFFER8*)MemAlloc(sizeof(LPDIRECT3DVERTEXBUFFER8) * m_uNumCells);
    if (!m_ppd3drSnowMesh) {
        return FALSE;
    }
    memset(m_ppd3drSnowMesh, 0, m_uNumCells * sizeof(LPDIRECT3DVERTEXBUFFER8));
    for (i = 0; i < m_uNumCells; i++) {
        m_ppd3drSnowMesh[i] = CreateVertexBuffer(m_pDevice, m_pprSnowMesh[i], m_uNumSnowVertices * sizeof(D3DXVECTOR3), D3DUSAGE_POINTS, D3DFVF_XYZ);
        if (!m_ppd3drSnowMesh[i]) {
            return FALSE;
        }
#ifdef UNDER_XBOX
        m_ppd3drSnowMesh[i]->MoveResourceMemory(D3DMEM_VIDEO);
#endif
    }

    // Initialize the snow sections and cells
    uCell = 0;
    for (i = 0, fDepth = DEPTH_FRONT_PLANE + CELLS_Z * fCellDepth; 
         i < CELLS_Z; 
         i++, fDepth -= fCellDepth) 
    {
        m_pssList[i].z = fDepth;
        m_pssList[i].fMaxViewX = fDepth * fTrigRatio;
        m_pssList[i].fMaxViewY = m_pssList[i].fMaxViewX * fAspect;
        uCellsX = ((UINT)(m_pssList[i].fMaxViewX * 2 / fCellWidth) + 2);
        uCellsY = ((UINT)(m_pssList[i].fMaxViewY * 2 / fCellHeight) + 2);
        m_pssList[i].fLengthX = uCellsX * fCellWidth;
        m_pssList[i].fLengthY = uCellsY * fCellHeight;

        for (j = 0; j < uCellsY; j++) {

            for (k = 0; k < uCellsX; k++) {

                m_pscList[uCell + (j * uCellsX) + k].uz = i;
                m_pscList[uCell + (j * uCellsX) + k].uMesh = rand() % m_uNumCells;
                m_pscList[uCell + (j * uCellsX) + k].x = -m_pssList[i].fMaxViewX + k * fCellWidth;
                m_pscList[uCell + (j * uCellsX) + k].y = m_pssList[i].fMaxViewY - j * fCellHeight;
            }
        }

        m_pssList[i].fMaxViewX += fCellWidth;
        m_pssList[i].fMaxViewY += fCellHeight;

        uCell += uCellsX * uCellsY;
    }

    // Create a material for the snow
    SetMaterial(m_pDevice, RGBA_MAKE(255, 255, 255, 255));

    // Create a background texture
    m_pd3dtBackground = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("backgrnd.bmp"), D3DFMT_R5G6B5);

    // Create the snow field textures
    cSnowColorKey = RGB_MAKE(200, 200, 200);

    m_pd3dtSnowFlake = (LPDIRECT3DTEXTURE8)CreateTexture(m_pDevice, TEXT("snow.bmp"), 
                                   D3DFMT_A1R5G5B5, TTYPE_TEXTURE, NULL, 
                                   (float)(1.0f), cSnowColorKey);
#ifdef UNDER_XBOX
    m_pd3dtSnowFlake->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

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
void CSnow::Efface() {

    UINT i;

    ReleaseTexture(m_pd3dtSnowFlake);
    ReleaseTexture(m_pd3dtBackground);

    MemFree(m_pscList);
    for (i = 0; i < m_uNumCells; i++) {
        if (m_pprSnowMesh[i]) {
            MemFree32(m_pprSnowMesh[i]);
        }
    }
    MemFree(m_pprSnowMesh);

    for (i = 0; i < m_uNumCells; i++) {
        ReleaseVertexBuffer(m_ppd3drSnowMesh[i]);
    }
    MemFree(m_ppd3drSnowMesh);

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
void CSnow::Update() {

    UINT i, j;

    for (i = 0; i < m_uNumCells; i++) {

        // Update the cell position
        m_pscList[i].x -= OFFSET_X * m_fTimeDilation;
        m_pscList[i].y -= OFFSET_Y * m_fTimeDilation;
        if (m_pscList[i].x < -m_pssList[m_pscList[i].uz].fMaxViewX) {
            m_pscList[i].x += m_pssList[m_pscList[i].uz].fLengthX;
            m_pscList[i].uMesh = rand() % m_uNumCells;
        }
        if (m_pscList[i].y < -m_pssList[m_pscList[i].uz].fMaxViewY) {
            m_pscList[i].y += m_pssList[m_pscList[i].uz].fLengthY;
            m_pscList[i].uMesh = rand() % m_uNumCells;
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
BOOL CSnow::Render() {

    UINT     i;
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

#endif

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
    SetAlphaStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetTexture(0, m_pd3dtBackground);

    // Draw the background
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

    // Use a fixed function shader
    m_pDevice->SetVertexShader(D3DFVF_XYZ);
/*
    // Set the snow sheet transform and texture
    m_mWorld._41 = 0.0f;
    m_mWorld._42 = 0.0f;
    m_mWorld._43 = 0.0f;
    m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
    m_pDevice->SetTexture(0, m_pd3dtSnowSheet);

    // Render the snow sheets
    for (i = 0; i < NUM_PLANES; i++) {
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
                            m_prSnowSheet[i], sizeof(VERTEX));
    }
*/
    // Set the snow flake texture
    m_pDevice->SetTexture(SPRITE_STAGE, m_pd3dtSnowFlake);

    m_pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW((m_fFlakeDim * 0.6f)));
    m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, F2DW(1.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, F2DW(0.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, F2DW(0.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, F2DW(1.0f));
    m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
SetColorStage(m_pDevice, SPRITE_STAGE, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);
SetAlphaStage(m_pDevice, SPRITE_STAGE, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);

    for (i = 0; i < m_uNumCells; i++) {

        // Set the world transform
        m_mWorld._41 = m_pscList[i].x;
        m_mWorld._42 = m_pscList[i].y;
        m_mWorld._43 = m_pssList[m_pscList[i].uz].z;
        m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

        m_pDevice->SetStreamSource(0, m_ppd3drSnowMesh[m_pscList[i].uMesh], sizeof(D3DXVECTOR3));
        m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_uNumSnowVertices);

        // Render the snow cell mesh
//        m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 
//                            m_uNumSnowVertices,
//                            m_pprSnowMesh[m_pscList[i].uMesh], 
//                            sizeof(D3DXVECTOR3));
    }

    m_pDevice->SetTexture(SPRITE_STAGE, NULL);
    SetColorStage(m_pDevice, SPRITE_STAGE, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_DISABLE);
    SetAlphaStage(m_pDevice, SPRITE_STAGE, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_DISABLE);
    m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);

m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

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
void CSnow::ProcessInput() {

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
BOOL CSnow::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
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
BOOL CSnow::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
