/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "test.h"

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

    CTest*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CTest();
    if (!pTest) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTest->Create(pDisplay)) {
        pTest->Release();
        return FALSE;
    }

    bRet = pTest->Exhibit(pnExitCode);

    // Clean up the scene
    pTest->Release();

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
BOOL ValidateDisplay(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
// CTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTest
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
CTest::CTest() {

    m_pd3dr = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CTest
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
CTest::~CTest() {
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
BOOL CTest::Initialize() {

    D3DLOCKED_RECT d3dlr;
    LPWORD pwPixel;
    UINT i, j;
    HRESULT hr;

    float fWidth = (float)m_pDisplay->GetWidth();
    float fHeight = (float)m_pDisplay->GetHeight();

    m_prTriangle[0] = TLVERTEX(D3DXVECTOR3(fWidth / 2.0f, 0.0f, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 0.0f, 1.0f);
    m_prTriangle[1] = TLVERTEX(D3DXVECTOR3(fWidth, fHeight, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 0.0f, 0.0f);
    m_prTriangle[2] = TLVERTEX(D3DXVECTOR3(0.0f, fHeight, 0.9999f), 0.01f, RGBA_MAKE(255,255,255,255), RGB_MAKE(0,0,0), 1.0f, 0.0f);

    m_pd3dr = CreateVertexBuffer(m_pDevice, m_prTriangle, 3 * sizeof(TLVERTEX), 0, FVF_TLVERTEX);
    if (!m_pd3dr) {
        return FALSE;
    }

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, 256, 256, D3DFMT_R5G6B5);
    if (!m_pd3dt) {
        return FALSE;
    }

    hr = m_pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return FALSE;
    }

    pwPixel = (LPWORD)d3dlr.pBits;

    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            pwPixel[j] = 0x03E0;
        }
        pwPixel += d3dlr.Pitch / 2;
    }

    hr = m_pd3dt->UnlockRect(0);
    if (FAILED(hr)) {
        return FALSE;
    }

    return TRUE;
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
BOOL CTest::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

    if (!m_bCreated) {
        return FALSE;
    }

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

    m_pDevice->SetTexture(0, m_pd3dt);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(TLVERTEX));

    m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

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
