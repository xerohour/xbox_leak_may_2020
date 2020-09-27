/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "textr.h"

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

    CTextr2*  pTextr;
    BOOL      bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTextr = new CTextr2();
    if (!pTextr) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTextr->Create(pDisplay)) {
        pTextr->Release();
        return FALSE;
    }

    bRet = pTextr->Exhibit(pnExitCode);

    // Clean up the scene
    pTextr->Release();

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
_TVERTEX2::_TVERTEX2(const D3DXVECTOR3& v, float _tu, float _tv)
{
    vPosition = v;
    fRHW = 0.1f;
    tu = _tu;
    tv = _tv;
}

//******************************************************************************
// CTextr2
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTextr2
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
CTextr2::CTextr2() {

    m_ptq = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CTextr2
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
CTextr2::~CTextr2() {

    if (m_ptq) {
        MemFree32(m_ptq);
    }
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
BOOL CTextr2::Create(CDisplay* pDisplay) {

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
int CTextr2::Exhibit(int *pnExitCode) {

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
BOOL CTextr2::Prepare() {

    float fTileWidth, fTileHeight, fX, fY, fXOffset;
    UINT  i, j;

    m_ptq = (PTEXQUAD)MemAlloc32(TEX_DIM_Y * TEX_DIM_X * sizeof(TEXQUAD));
    if (!m_ptq) {
        return FALSE;
    }

    fTileWidth = (float)m_pDisplay->GetHeight() / (float)TEX_DIM_X;
    fTileHeight = (float)m_pDisplay->GetHeight() / (float)TEX_DIM_Y;
    fXOffset = (float)m_pDisplay->GetWidth() / 2.0f - (float)m_pDisplay->GetHeight() / 2.0f;

    for (i = 0, fY = 0.0f; i < TEX_DIM_Y; i++, fY += fTileHeight) {
        for (j = 0, fX = 0.0f; j < TEX_DIM_X; j++, fX += fTileWidth) {
            m_ptq[i * TEX_DIM_X + j].pr[0] = TVERTEX2(D3DXVECTOR3(fXOffset + fX, fY + fTileHeight, 0.5f), 0.0f, 1.0f);
            m_ptq[i * TEX_DIM_X + j].pr[1] = TVERTEX2(D3DXVECTOR3(fXOffset + fX, fY, 0.5f), 0.0f, 0.0f);
            m_ptq[i * TEX_DIM_X + j].pr[2] = TVERTEX2(D3DXVECTOR3(fXOffset + fX + fTileWidth, fY, 0.5f), 1.0f, 0.0f);
            m_ptq[i * TEX_DIM_X + j].pr[3] = TVERTEX2(D3DXVECTOR3(fXOffset + fX + fTileWidth, fY + fTileHeight, 0.5f), 1.0f, 1.0f);
            m_ptq[i * TEX_DIM_X + j].pd3dt = NULL;
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
BOOL CTextr2::Setup() {

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
BOOL CTextr2::Initialize() {

    HGLOBAL           hg;
    HRSRC             hrsrc;
    BITMAPINFOHEADER* pbih;
    LPBYTE            fp;
    LPBYTE            psrc, pData;
    DWORD             dwSize;
    CTexture8*        pd3dt;
    D3DLOCKED_RECT    d3dlr;
    LPWORD            pwDst;
    UINT              i, j, k, l;
    WORD              wPixel;
    HRESULT           hr;

    if (!LoadResourceData(TEXT("texture.bmp"), (LPVOID*)&fp, &dwSize)) {
        DebugString(TEXT("FindResourceData failed"));
        return FALSE;
    }

    pbih = (BITMAPINFOHEADER*)(fp + sizeof(BITMAPFILEHEADER));

    if (pbih->biBitCount != 24) {
        UnloadResourceData(TEXT("texture.bmp"));
        return FALSE;
    }

    pData = fp + ((BITMAPFILEHEADER*)fp)->bfOffBits;

    for (i = 0; i < TEX_DIM_Y; i++) {

        for (j = 0; j < TEX_DIM_X; j++) {

            hr = m_pDevice->CreateTexture(1 << j, 1 << i, 1, 0, D3DFMT_R5G6B5, POOL_DEFAULT, &pd3dt);
            if (FAILED(hr)) {
                UnloadResourceData(TEXT("texture.bmp"));
                return FALSE;
            }

            hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
                pd3dt->Release();
                UnloadResourceData(TEXT("texture.bmp"));
                return FALSE;
            }

            psrc = pData + (pbih->biHeight - 1 - i) * pbih->biWidth * 3 + j * 3;
            pwDst = (LPWORD)d3dlr.pBits;

            wPixel  = (*psrc++ & 0xF8) >> 3;
            wPixel |= (*psrc++ & 0xFC) << 3;
            wPixel |= (*psrc++ & 0xF8) << 8;

            for (k = 0; k < (UINT)(1 << i); k++) {

                for (l = 0; l < (UINT)(1 << j); l++) {

                    pwDst[l] = wPixel;
                }

                pwDst += d3dlr.Pitch / 2;
            }

            hr = pd3dt->UnlockRect(0);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
                pd3dt->Release();
                UnloadResourceData(TEXT("texture.bmp"));
                return FALSE;
            }

            m_ptq[i * TEX_DIM_X + j].pd3dt = pd3dt;
        }
    }

    UnloadResourceData(TEXT("texture.bmp"));

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_SELECTARG1);

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
void CTextr2::Efface() {

    UINT i, j;

    if (m_ptq) {

        for (i = 0; i < TEX_DIM_Y; i++) {

            for (j = 0; j < TEX_DIM_X; j++) {

                if (m_ptq[i * TEX_DIM_X + j].pd3dt) {
                    m_ptq[i * TEX_DIM_X + j].pd3dt->Release();
                    m_ptq[i * TEX_DIM_X + j].pd3dt = NULL;
                }
            }
        }
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
void CTextr2::Update() {

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
BOOL CTextr2::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;
    UINT     i, j;

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
    m_pDevice->SetVertexShader(FVF_TVERTEX2);

    for (i = 0; i < TEX_DIM_Y; i++) {

        for (j = 0; j < TEX_DIM_X; j++) {

            // Set the texture
            m_pDevice->SetTexture(0, m_ptq[i * TEX_DIM_X + j].pd3dt);

            // Render
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,
                    m_ptq[i * TEX_DIM_X + j].pr, sizeof(TVERTEX2));
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
void CTextr2::ProcessInput() {

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
BOOL CTextr2::InitView() {

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
BOOL CTextr2::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
