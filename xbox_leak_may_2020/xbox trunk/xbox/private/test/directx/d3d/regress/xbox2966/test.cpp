/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include "util.h"
#include "dtypes.h"
#include "test.h"

//******************************************************************************
// WinMain
//******************************************************************************

#ifndef UNDER_XBOX
#define RETURN(x)   return x
#else
#define RETURN(x)   return
#endif // UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     WinMain
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     HINSTANCE hInstance      - Application instance handle
//
//     HINSTANCE hPrevInstance  - Previous instance of the application (always
//                                NULL for Win32 applications)
//
//     LPSTR szCmdLine          - Pointer to a string specifying the command
//                                line used in launching the application
//                                (excluding the program name)
//
//     int nCmdShow             - State specifying how the window is to be 
//                                displayed on creation
//
// Return Value:
//
//     0 on success, -1 on failure.
//
//******************************************************************************
#ifndef UNDER_XBOX
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow) 
#else
void __cdecl main()
#endif // UNDER_XBOX
{
    CReproTest*     pRTest;
    int             ret = 0;

    // Seed the pseudo-random number generator
    srand(GetTickCount());

    // Create a heap for the application
    if (!CreateHeap()) {
        RETURN(0);
    }

    // Create the display
    pRTest = new CReproTest();
    if (!pRTest) {
        ReleaseHeap();
        RETURN(0);
    }

    // Initialize the display
#ifndef UNDER_XBOX
    if (!pRTest->Create(hInstance)) {
#else
    if (!pRTest->Create(NULL)) {
#endif
        DebugString(TEXT("CDisplay::Create failed"));
        delete pRTest;
        ReleaseHeap();
        RETURN(0);
    }

    // Render the 3D scene
    pRTest->Run();

    // Clean up
    delete pRTest;
    ReleaseHeap();

    RETURN(ret);
}

//******************************************************************************
CReproTest::CReproTest() {

    m_hWnd = NULL;
    m_pd3d = NULL;
    m_pDevice = NULL;
    m_pd3dr = NULL;
}

//******************************************************************************
CReproTest::~CReproTest() {

    if (m_pd3dr) {
        m_pd3dr->Release();
        m_pd3dr = NULL;
    }

    if (m_pDevice) {
        m_pDevice->Release();
        m_pDevice = NULL;
    }

    if (m_pd3d) {
        m_pd3d->Release();
        m_pd3d = NULL;
    }

#ifndef UNDER_XBOX
    // Unregister the window class
    UnregisterClass(TEXT("Direct3D Repro Test"), m_hInstance);
#endif // !UNDER_XBOX
}

//******************************************************************************
BOOL CReproTest::Create(HINSTANCE hInstance) {

    BOOL bEnabled;
    D3DPRESENT_PARAMETERS   d3dpp;
    LVERTEX                 plrVertices[3];
    LPBYTE                  pData;
    HRESULT                 hr;
#ifndef UNDER_XBOX
    WNDCLASS                wc;
    MSG                     msg;
#endif // !UNDER_XBOX

    m_hInstance = hInstance;

#ifndef UNDER_XBOX

    // Register the window class
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style            = 0;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = m_hInstance;
    wc.hIcon            = NULL;
    wc.hbrBackground    = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = TEXT("Direct3D Repro Test");

    if (!RegisterClass(&wc)) {
        DebugString(TEXT("RegisterClass failed"));
		return FALSE;
    }

    // Create the main application window
    m_hWnd = CreateWindowEx(0, TEXT("Direct3D Repro Test"), 
                            TEXT("Direct3D Repro Test"), 
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,
                            GetSystemMetrics(SM_CXSCREEN), 
                            GetSystemMetrics(SM_CYSCREEN),
                            NULL, NULL, m_hInstance, NULL);

    if (!m_hWnd) {
        DebugString(TEXT("CreateWindowEx failed"));
		return FALSE;
    }

    UpdateWindow(m_hWnd);

    // Pump any messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

#endif

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_pd3d) {
        DebugString(TEXT("Direct3DCreate8 failed"));
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_A8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = m_hWnd;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    // Create the device
    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice failed"))) {
        return FALSE;
    }

//    hr = m_pDevice->GetLightEnable(100, &bEnabled);
//    DebugString(TEXT("GetLightEnable - hr = %X"), hr);

    plrVertices[0] = LVERTEX(D3DXVECTOR3(-10.0f, -10.0f, 0.0f), RGBA_MAKE(255, 0, 0, 255), 0, 0.0f, 0.0f);
    plrVertices[1] = LVERTEX(D3DXVECTOR3( 0.0f,  10.0f, 0.0f), RGBA_MAKE(0, 0, 255, 255), 0, 0.0f, 0.0f);
    plrVertices[2] = LVERTEX(D3DXVECTOR3( 10.0f, -10.0f, 0.0f), RGBA_MAKE(0, 255, 0, 255), 0, 0.0f, 0.0f);

    hr = m_pDevice->CreateVertexBuffer(3 * sizeof(LVERTEX), 0, FVF_LVERTEX, D3DPOOL_DEFAULT, &m_pd3dr);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        return FALSE;
    }

    hr = m_pd3dr->Lock(0, 3 * sizeof(LVERTEX), &pData, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"))) {
        return FALSE;
    }

    memcpy(pData, plrVertices, 3 * sizeof(LVERTEX));

    hr = m_pd3dr->Unlock();
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"))) {
        return FALSE;
    }

    return InitDeviceState();
}

//******************************************************************************
BOOL CReproTest::Run() {

#ifndef UNDER_XBOX
    MSG             msg;
#endif // !UNDER_XBOX
    HRESULT         hr;
    D3DGAMMARAMP    d3dgr;
    UINT            uFrame = 0;
    UINT            i;

    do {

    if (uFrame++ > 240) {

        D3DPRESENT_PARAMETERS d3dpp;

        memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
        d3dpp.BackBufferWidth                   = 640;
        d3dpp.BackBufferHeight                  = 480;
        d3dpp.BackBufferFormat                  = D3DFMT_A8R8G8B8;
        d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
        d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
        d3dpp.FullScreen_RefreshRateInHz        = 0;
        d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
        d3dpp.BackBufferCount                   = 1;
        d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow                     = m_hWnd;
        d3dpp.Windowed                          = FALSE;
        d3dpp.EnableAutoDepthStencil            = TRUE;

        m_pDevice->Reset(&d3dpp);
/*
        for (i = 0; i < 256; i++) {
            d3dgr.red[i] = (BYTE)i;
            d3dgr.green[i] = (BYTE)i;
            d3dgr.blue[i] = (BYTE)i;
        }

        m_pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION | D3DSGR_IMMEDIATE, &d3dgr);
*/
        uFrame = 0;
    }

#ifndef UNDER_XBOX
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (msg.message != WM_QUIT) {

            hr = m_pDevice->TestCooperativeLevel();
            if (FAILED(hr)) {
                return FALSE;
            }

#endif // !UNDER_XBOX

            // Clear the rendering target
            m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB_MAKE(0, 0, 0), 1.0f, 0);

            // Begin the scene
            m_pDevice->BeginScene();

            // Disable lighting
            m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

            // Use a fixed function shader
            m_pDevice->SetVertexShader(FVF_LVERTEX);

            m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(LVERTEX));

            // Draw a triangle
            m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, plrVertices, sizeof(LVERTEX));

            // End the scene
            m_pDevice->EndScene();

            // Update the screen
            m_pDevice->Present(NULL, NULL, NULL, NULL);

#ifndef UNDER_XBOX
        }
    } while (msg.message != WM_QUIT);
#else
    } while (TRUE);
#endif

    return TRUE;
}

//******************************************************************************
BOOL CReproTest::InitDeviceState() {

    D3DXMATRIX mTransform;
    UINT       i;
    HRESULT    hr;

    // Initialize the viewport
    if (!SetViewport(640, 480)) {
        DebugString(TEXT("SetViewport failed"));
    }

    // Initialize the view matrix
    SetView(&mTransform, &D3DXVECTOR3(0.0f, 0.0f, -50.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
            &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

    hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTransform);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Initialize the projection matrix
    SetPerspectiveProjection(&mTransform, 0.1f, 1000.0f, 
            M_PI / 4.0f, 480.0f / 640.0f);

    hr = m_pDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Set ambient lighting
    hr = m_pDevice->SetRenderState(D3DRS_AMBIENT, 0x20202020);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Set the texture filters
    for (i = 0; i < 2; i++) {
        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
        if (FAILED(hr)) {
            DebugString(TEXT("IDirect3DDevice8::SetTextureStageState"));
        }

        hr = m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
        if (FAILED(hr)) {
            DebugString(TEXT("IDirect3DDevice8::SetTextureStageState"));
        }
    }

    // Set blend modes
    hr = m_pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    hr = m_pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Modulate color and alpha texture stages
    hr = m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    hr = m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    hr = m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetTextureStageState"));
    }

    // Set the alpha comparison function
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATEREQUAL);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Set the alpha reference value to opaque
    hr = m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Turn on specular highlights
    hr = m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, (DWORD)TRUE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Enable dithering
    hr = m_pDevice->SetRenderState(D3DRS_DITHERENABLE, (DWORD)TRUE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    // Disable per-vertex color
    hr = m_pDevice->SetRenderState(D3DRS_COLORVERTEX, (DWORD)FALSE);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3DDevice8::SetRenderState failed"));
    }

    return TRUE;
}

//******************************************************************************
BOOL CReproTest::SetViewport(DWORD dwWidth, DWORD dwHeight) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    // Set a viewport for the device
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = dwWidth;
    viewport.Height = dwHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    hr = m_pDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    return TRUE;
}

#ifndef UNDER_XBOX

//******************************************************************************
// WndProc
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Window procedure to process messages for the main application window
//
// Arguments:
//
//     HWND hWnd                - Application window
//
//     UINT message             - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     0 if the message was handled, the return of DefWindowProc otherwise.
//
//******************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

        case WM_SETCURSOR:
            SetCursor(NULL);
            return TRUE;

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif // UNDER_XBOX

//******************************************************************************
//
// Function:
//
//     InitMatrix
//
// Description:
//
//     Initialize the given matrix with the given values.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Matrix to initialize
//
//     float _11 to _44         - Initialization values
//
// Return Value:
//
//     None.
//
//******************************************************************************
void InitMatrix(D3DMATRIX* pd3dm,
                float _11, float _12, float _13, float _14,
                float _21, float _22, float _23, float _24,
                float _31, float _32, float _33, float _34,
                float _41, float _42, float _43, float _44) 
{
    if (pd3dm) {

        pd3dm->_11 = _11;
        pd3dm->_12 = _12;
        pd3dm->_13 = _13;
        pd3dm->_14 = _14;
        pd3dm->_21 = _21;
        pd3dm->_22 = _22;
        pd3dm->_23 = _23;
        pd3dm->_24 = _24;
        pd3dm->_31 = _31;
        pd3dm->_32 = _32;
        pd3dm->_33 = _33;
        pd3dm->_34 = _34;
        pd3dm->_41 = _41;
        pd3dm->_42 = _42;
        pd3dm->_43 = _43;
        pd3dm->_44 = _44;
    }
}

//******************************************************************************
//
// Function:
//
//     SetView
//
// Description:
//
//     Align the given view matrix along the given direction and up vectors with
//     the position vector as the origin.
//
// Arguments:
//
//     LPD3DXMATRIX pd3dm       - View matrix to initialize
//
//     LPD3DXVECTOR3 pvPos      - View origin
//
//     LPD3DXVECTOR3 pvAt       - View interest vector
//
//     LPD3DXVECTOR3 pvUp       - View up vector
//
// Return Value:
//
//     Initialized view matrix.
//
//******************************************************************************
void SetView(LPD3DXMATRIX pd3dm, LPD3DXVECTOR3 pvPos, LPD3DXVECTOR3 pvAt, 
                                                      LPD3DXVECTOR3 pvUp) 
{
    D3DXVECTOR3  d, u, r;

    if (pd3dm && pvPos && pvAt && pvUp && !(*pvPos == *pvAt) 
                                 && !(*pvUp == D3DXVECTOR3(0.0f, 0.0f, 0.0f))) {

        D3DXVec3Normalize(&d, &(*pvAt - *pvPos));
        u = *pvUp;

        // Project the up vector into the plane of the direction vector
        D3DXVec3Normalize(&u, &(u - (d * D3DXVec3Dot(&u, &d))));

        // Get the cross product
        D3DXVec3Cross(&r, &u, &d);

        // Initialize the view transform
        InitMatrix(pd3dm,
            r.x, u.x, d.x, 0.0f,
            r.y, u.y, d.y, 0.0f,
            r.z, u.z, d.z, 0.0f,
            -(pvPos->x * r.x + pvPos->y * r.y + pvPos->z * r.z), 
            -(pvPos->x * u.x + pvPos->y * u.y + pvPos->z * u.z), 
            -(pvPos->x * d.x + pvPos->y * d.y + pvPos->z * d.z), 
            1.0f
        );
    }
}

//******************************************************************************
//
// Function:
//
//     SetPerspectiveProjection
//
// Description:
//
//     Initialize the given projection matrix using the given front and back
//     clipping planes, field of view, and aspect ratio.
//
// Arguments:
//
//     LPD3DMATRIX pd3dm        - Projection matrix to initialize
//
//     float fFront             - Front clipping plane
//
//     float fBack              - Back clipping plane
//
//     float fFieldOfView       - Angle, in radians, of the field of view
//
//     float fAspect            - Aspect ratio (y / x) of the view plane
//
// Return Value:
//
//     Initialized projection matrix.
//
//******************************************************************************
void SetPerspectiveProjection(D3DMATRIX* pd3dm, float fFront, float fBack, 
                                    float fFieldOfView, float fAspect) {

    float fTanHalfFOV = (float)tan((double)fFieldOfView / 2.0);
    float fFar = fBack / (fBack - fFront);

    InitMatrix(pd3dm,
        1.0f, 0.0f,           0.0f,                         0.0f,
        0.0f, 1.0f / fAspect, 0.0f,                         0.0f,
        0.0f, 0.0f,           fTanHalfFOV * fFar,           fTanHalfFOV,
        0.0f, 0.0f,           -fFront * fTanHalfFOV * fFar, 0.0f
    );
}

