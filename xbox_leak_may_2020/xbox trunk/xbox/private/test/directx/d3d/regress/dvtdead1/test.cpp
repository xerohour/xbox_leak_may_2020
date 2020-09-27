/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    test.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "util.h"
#include "dtypes.h"
#include "test.h"

//******************************************************************************
// WinMain
//******************************************************************************

#define RETURN(x)   return

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
void __cdecl main()
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
    if (!pRTest->Create(NULL)) {
        DebugString(TEXT("CDisplay::Create failed"));
        delete pRTest;
        ReleaseHeap();
        RETURN(0);
    }

    // Render the scene
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
    m_dwVShader = 0xFFFFFFFF;
}

//******************************************************************************
CReproTest::~CReproTest() {

    if (m_dwVShader != 0xFFFFFFFF) {
        m_pDevice->SetVertexShader(D3DFVF_XYZ);
        m_pDevice->DeleteVertexShader(m_dwVShader);
    }

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
}

//******************************************************************************
BOOL CReproTest::Create(HINSTANCE hInstance) {

    D3DPRESENT_PARAMETERS   d3dpp;
    LVERTEX                 plrVertices[3];
    LPBYTE                  pData;
    HRESULT                 hr;
    UINT                    i;
    LPXGBUFFER              pxgbufShader;
    DWORD                   dwSDecl[] = {
                                D3DVSD_STREAM(0),
                                D3DVSD_REG(0, D3DVSDT_FLOAT3),
                                D3DVSD_REG(1, D3DVSDT_D3DCOLOR),
                                D3DVSD_REG(2, D3DVSDT_D3DCOLOR),
                                D3DVSD_REG(3, D3DVSDT_FLOAT2),
                                D3DVSD_END()
                            };
    char                    szVShader[] = {
                                "vs.1.0\n"
                                "dp4 oPos.x, v0, c0\n"
                                "dp4 oPos.y, v0, c1\n"
                                "dp4 oPos.z, v0, c2\n"
                                "dp4 oPos.w, v0, c3\n"
                                "mov oD0, v1\n"
                            };

    m_hInstance = hInstance;

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
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = m_hWnd;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = FALSE; // Set this to TRUE and the triangle is rendered correctly

    // Create the device
    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice failed"))) {
        return FALSE;
    }

    plrVertices[0] = LVERTEX(D3DXVECTOR3(-1.0f, -1.0f, 0.0f), RGBA_MAKE(255, 0, 0, 255), 0, 0.0f, 0.0f);
    plrVertices[1] = LVERTEX(D3DXVECTOR3( 0.0f,  1.0f, 0.0f), RGBA_MAKE(0, 0, 255, 255), 0, 0.0f, 0.0f);
    plrVertices[2] = LVERTEX(D3DXVECTOR3( 1.0f, -1.0f, 0.0f), RGBA_MAKE(0, 255, 0, 255), 0, 0.0f, 0.0f);

    hr = m_pDevice->CreateVertexBuffer(3 * sizeof(LVERTEX), 0, FVF_LVERTEX, D3DPOOL_DEFAULT, &m_pd3dr);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"))) {
        return FALSE;
    }

    hr = m_pd3dr->Lock(0, 0, &pData, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"))) {
        return FALSE;
    }

    memcpy(pData, plrVertices, 3 * sizeof(LVERTEX));

    hr = m_pd3dr->Unlock();
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"))) {
        return FALSE;
    }

    AssembleShader("", szVShader, strlen(szVShader), 0, NULL, 
                        &pxgbufShader, NULL, NULL, NULL, NULL, NULL);

    m_pDevice->CreateVertexShader(dwSDecl, (LPDWORD)pxgbufShader->GetBufferPointer(), &m_dwVShader, 0);

    pxgbufShader->Release();

    return InitDeviceState();
}

//******************************************************************************
BOOL CReproTest::Run() {

    D3DXMATRIX  mView, mProj, mTransform;
    HRESULT     hr;
    UINT        i;
    static UINT uFrame = 0;

    do {

        DebugString(TEXT("Rendering frame %d"), ++uFrame);

        // Clear the rendering target
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(0, 0, 0), 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        // Disable lighting
        m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

        SetView(&mView, &D3DXVECTOR3(0.0f, 0.0f, -50.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

        SetPerspectiveProjection(&mProj, 0.1f, 1000.0f, 
                M_PI / 4.0f, 480.0f / 640.0f);

        D3DXMatrixMultiply(&mTransform, &mView, &mProj);
        D3DXMatrixTranspose(&mTransform, &mTransform);

        m_pDevice->SetVertexShaderConstant(0, &mTransform, 4);

        // Use a fixed function shader
        m_pDevice->SetVertexShader(m_dwVShader);

        m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(LVERTEX));

        // Draw a triangle
        m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

        // End the scene
        m_pDevice->EndScene();

        // Update the screen
        m_pDevice->Present(NULL, NULL, NULL, NULL);

    } while (TRUE);

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

