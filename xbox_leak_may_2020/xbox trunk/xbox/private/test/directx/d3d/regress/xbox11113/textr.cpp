/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    textr.cpp

Author:

    Matt Bronder

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <xgraphics.h>
#include <xgmath.h>
#include "util.h"

using namespace XBOX11113;

#include "textr.h"

//******************************************************************************
// CSample
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CSample
//
// Description:
//
//     Initialize the sample.
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
CSample::CSample() {

    m_pd3d = NULL;
    memset(m_pvColorBuffer, 0, 3 * sizeof(LPVOID));
    m_pvDepthBuffer = NULL;
    m_pDevice = NULL;

    m_pd3dt = NULL;

    m_prVertices[0] = VERTEX(D3DXVECTOR3(-2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f);
    m_prVertices[1] = VERTEX(D3DXVECTOR3(-2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f);
    m_prVertices[2] = VERTEX(D3DXVECTOR3( 2.5f, 2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f);
    m_prVertices[3] = VERTEX(D3DXVECTOR3( 2.5f,-2.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f);

    m_prBackground[0] = TLVERTEX(D3DXVECTOR3(-0.5f, 479.5f, 0.9999f), 0.01f, 0xFFFFFFFF, 0, 0.0f, 1.0f);
    m_prBackground[1] = TLVERTEX(D3DXVECTOR3(-0.5f, -0.5f, 0.9999f), 0.01f, 0xFFFFFFFF, 0, 0.0f, 0.0f);
    m_prBackground[2] = TLVERTEX(D3DXVECTOR3(639.5f, -0.5f, 0.9999f), 0.01f, 0xFFFFFFFF, 0, 1.0f, 0.0f);
    m_prBackground[3] = TLVERTEX(D3DXVECTOR3(639.5f, 479.5f, 0.9999f), 0.01f, 0xFFFFFFFF, 0, 1.0f, 1.0f);
}

//******************************************************************************
//
// Method:
//
//     ~CSample
//
// Description:
//
//     Clean up the sample.
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
CSample::~CSample() {

    UINT i;

    if (m_pd3dt) {
        m_pd3dt->Release();
    }

    if (m_pDevice) {

        if (m_pvColorBuffer[0]) {
            m_pDevice->SetTile(0, NULL);
        }
        if (m_pvDepthBuffer) {
            m_pDevice->SetTile(1, NULL);
        }

        m_pDevice->Release();
    }

    for (i = 0; i < 3; i++) {
        if (m_pvColorBuffer[i]) {
            D3D_FreeContiguousMemory(m_pvColorBuffer[i]);
            m_pvColorBuffer[i] = NULL;
        }
    }
    if (m_pvDepthBuffer) {
        D3D_FreeContiguousMemory(m_pvDepthBuffer);
        m_pvDepthBuffer = NULL;
    }

    if (m_pd3d) {
        m_pd3d->Release();
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
//     Prepare the sample for rendering.
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
BOOL CSample::Create() {

    if (!StartGraphics()) {
        return FALSE;
    }

    // Create a mipmap
    if (!LoadTexture(m_pDevice, "grid.bmp", &m_pd3dt)) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
    m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x80FFFFFF);

    m_pDevice->SetTexture(0, m_pd3dt);

    return TRUE;
}

//******************************************************************************
BOOL CSample::StartGraphics() {

    D3DPRESENT_PARAMETERS   d3dpp;
    UINT                    uSize, uPitch;
    D3DTILE                 tile[2];
    D3DVIEWPORT8            viewport = {0, 0, 640, 480, 0.0f, 1.0f};
    XGMATRIX                mView, mProj, mTransform;
    XGVECTOR3               vViewPos, vViewAt, vViewUp;
    UINT                    i;
    HRESULT                 hr;

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_LIN_X8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_LIN_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = NULL;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;
    
    if (XGetVideoFlags() & XC_VIDEO_FLAGS_WIDESCREEN) {
        d3dpp.Flags = D3DPRESENTFLAG_WIDESCREEN;
        m_fAspect = 640.0f / 480.0f * 1.25f;
    }
    else {
        m_fAspect = 640.0f / 480.0f;
    }

    m_fFieldOfView = XG_PI / 3.0f;

    // Allocate the prefilter buffer
    uPitch = D3DTILE_PITCH_1400; // 1280 * 4
    uSize = uPitch * 480;
    uSize = (uSize + D3DTILE_ALIGNMENT - 1) & ~(D3DTILE_ALIGNMENT - 1);

    m_pvColorBuffer[0] = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
    if (!m_pvColorBuffer[0]) {
        OutputDebugString(TEXT("Insufficient contiguous memory available to create color buffer surface(s)\r\n"));
        return FALSE;
    }

    memset(&tile[0], 0, sizeof(D3DTILE));
    tile[0].Size = uSize;
    tile[0].Pitch = uPitch;
    tile[0].pMemory = m_pvColorBuffer[0];

    XGSetSurfaceHeader(1280, 480, D3DFMT_LIN_X8R8G8B8, &m_d3dsColorBuffer[0], 0, uPitch);
    m_d3dsColorBuffer[0].Register((LPBYTE)m_pvColorBuffer[0]);
    d3dpp.BufferSurfaces[0] = &m_d3dsColorBuffer[0];

    // Allocate the postfilter buffer
    uPitch = D3DTILE_PITCH_0A00; // 640 * 4
    uSize = uPitch * 480;

    m_pvColorBuffer[1] = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
    if (!m_pvColorBuffer[1]) {
        OutputDebugString(TEXT("Insufficient contiguous memory available to create postfilter buffer surface\r\n"));
        return FALSE;
    }

    XGSetSurfaceHeader(640, 480, D3DFMT_LIN_X8R8G8B8, &m_d3dsColorBuffer[1], 0, uPitch);
    m_d3dsColorBuffer[1].Register((LPBYTE)m_pvColorBuffer[1]);
    d3dpp.BufferSurfaces[1] = &m_d3dsColorBuffer[1];

    // Allocate the depth buffer
    uPitch = D3DTILE_PITCH_1400; // 1280 * 4
    uSize = uPitch * 480;
    uSize = (uSize + D3DTILE_ALIGNMENT - 1) & ~(D3DTILE_ALIGNMENT - 1);

    m_pvDepthBuffer = D3D_AllocContiguousMemory(uSize, D3DTILE_ALIGNMENT);
    if (!m_pvDepthBuffer) {
        OutputDebugString(TEXT("Insufficient contiguous memory available to create depth buffer surface\r\n"));
        return FALSE;
    }

    memset(&tile[1], 0, sizeof(D3DTILE));
    tile[1].Size = uSize;
    tile[1].Pitch = uPitch;
    tile[1].pMemory = m_pvDepthBuffer;
    tile[1].Flags = D3DTILE_FLAGS_ZBUFFER | D3DTILE_FLAGS_ZCOMPRESS;
    tile[1].ZStartTag = 0;
    tile[1].ZOffset = 0;
    tile[1].Flags |= D3DTILE_FLAGS_Z32BITS;

    XGSetSurfaceHeader(1280, 480, D3DFMT_LIN_D24S8, &m_d3dsDepthBuffer, 0, uPitch);
    m_d3dsDepthBuffer.Register(m_pvDepthBuffer);
    d3dpp.DepthStencilSurface = &m_d3dsDepthBuffer;

    // Create the device
    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);
    if (FAILED(hr)) {
        DebugString(TEXT("IDirect3D8::CreateDevice failed"));
        return FALSE;
    }

    m_pDevice->SetTile(0, &tile[0]);
    if (d3dpp.EnableAutoDepthStencil) {
        m_pDevice->SetTile(1, &tile[1]);
    }

    // Set a viewport
    m_pDevice->SetViewport(&viewport);

    vViewPos = XGVECTOR3(0.0f, 0.0f, -20.0f);
    vViewAt = XGVECTOR3(0.0f, 0.0f, 0.0f);
    vViewUp = XGVECTOR3(0.0f, 1.0f, 0.0f);
    XGMatrixLookAtLH(&mView, &vViewPos, &vViewAt, &vViewUp);
    m_pDevice->SetTransform(D3DTS_VIEW, &mView);

    XGMatrixPerspectiveFovLH(&mProj, m_fFieldOfView, m_fAspect, 0.1f, 100.0f);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &mProj);

    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    for (i = 0; i < 4; i++) {
        m_pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, 0);
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
//     Render the sample scene.
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
BOOL CSample::Render() {

    XGMATRIX mWorld;
    UINT     uFrame = 0;

    for (uFrame = 0; uFrame < 11; uFrame++) {

        XGMatrixIdentity(&mWorld);

        // Clear the rendering target
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

        // Begin the scene
        m_pDevice->BeginScene();

        // Use a fixed function shader
        m_pDevice->SetVertexShader(FVF_TLVERTEX);

        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

        m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));

        m_pDevice->SetVertexShader(FVF_VERTEX);

        mWorld._42 = 2.75f;
        m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(VERTEX));

        mWorld._42 = -2.75f;
        m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prVertices, sizeof(VERTEX));

        // End the scene
        m_pDevice->EndScene();

        // Update the screen
        m_pDevice->Present(NULL, NULL, NULL, NULL);
    }

    return TRUE;
}
