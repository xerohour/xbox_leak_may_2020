/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vigil.cpp

Description:

    Memory surveillance.

*******************************************************************************/

#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <xgraphics.h>
#include <xgmath.h>
#include "vigil.h"

//******************************************************************************
// main
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     main
//
// Description:
//
//     Entry point for the application.
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
void __cdecl main()
{
    CVigil* pVigil;

    // Seed the pseudo-random number generator
    srand(0x5C7B82F4);

    pVigil = new CVigil();
    if (!pVigil) {
        return;
    }

    if (!pVigil->Create()) {
        OutputDebugString(TEXT("CVigil::Create failed"));
        delete pVigil;
        return;
    }

    pVigil->Run();

    delete pVigil;
}

//******************************************************************************
CVigil::CVigil() {

    DWORD dwCRC;
    UINT  i, j;

    m_pd3d = NULL;
    m_pDevice = NULL;
    m_ppd3dt = NULL;
    m_ppd3dr = NULL;
    m_ppwTile = NULL;
    m_pdwTextureCRC = NULL;
    m_pdwVertexBufferCRC = NULL;
    m_pdwIndexListCRC = NULL;
    m_uNumTileIndexLists = 0;
    m_uNumTiles = 0;
    memset(m_ppvTexture, 0, 6 * sizeof(LPVOID));
    memset(m_pd3dt, 0, 6 * sizeof(D3DTexture));

    // Initialize the CRC table
    for (i = 0; i < 256; i++) {
        dwCRC = i;
        for (j = 0; j < 8; j++) {
            dwCRC = (dwCRC & 1) ? ((dwCRC >> 1) ^ 0xEDB88320L) : (dwCRC >> 1);
        }
        m_dwCRCTable[i] = dwCRC;
    }

    // Initialize the blt quads
    m_prSwizzledQuad[0] = TVERTEX(XGVECTOR3(-0.5f, 255.5f, 0.5f), 2.0f, 0.0f, 1.0f);
    m_prSwizzledQuad[1] = TVERTEX(XGVECTOR3(-0.5f, -0.5f, 0.5f), 2.0f, 0.0f, 0.0f);
    m_prSwizzledQuad[2] = TVERTEX(XGVECTOR3(255.5f, -0.5f, 0.5f), 2.0f, 1.0f, 0.0f);
    m_prSwizzledQuad[3] = TVERTEX(XGVECTOR3(255.5f, 255.5f, 0.5f), 2.0f, 1.0f, 1.0f);
    memcpy(m_prLinearQuad, m_prSwizzledQuad, 4 * sizeof(TVERTEX));
    for (i = 0; i < 4; i++) {
        m_prLinearQuad[i].u0 *= 256.0f;
        m_prLinearQuad[i].v0 *= 256.0f;
    }

    // Initialize quad index list order configurations
    m_wQuadOrder[0][0][0].x = 0;
    m_wQuadOrder[0][0][0].y = 1;
    m_wQuadOrder[0][0][1].x = 0;
    m_wQuadOrder[0][0][1].y = 0;
    m_wQuadOrder[0][0][2].x = 1;
    m_wQuadOrder[0][0][2].y = 1;
    m_wQuadOrder[0][0][3].x = 0;
    m_wQuadOrder[0][0][3].y = 1;
    m_wQuadOrder[0][0][4].x = 0;
    m_wQuadOrder[0][0][4].y = 0;

    m_wQuadOrder[0][1][0].x = 0;
    m_wQuadOrder[0][1][0].y = 0;
    m_wQuadOrder[0][1][1].x = 1;
    m_wQuadOrder[0][1][1].y = 0;
    m_wQuadOrder[0][1][2].x = 1;
    m_wQuadOrder[0][1][2].y = 1;
    m_wQuadOrder[0][1][3].x = 0;
    m_wQuadOrder[0][1][3].y = 0;
    m_wQuadOrder[0][1][4].x = 1;
    m_wQuadOrder[0][1][4].y = 0;

    m_wQuadOrder[1][0][0].x = 0;
    m_wQuadOrder[1][0][0].y = 1;
    m_wQuadOrder[1][0][1].x = 0;
    m_wQuadOrder[1][0][1].y = 0;
    m_wQuadOrder[1][0][2].x = 1;
    m_wQuadOrder[1][0][2].y = 0;
    m_wQuadOrder[1][0][3].x = 0;
    m_wQuadOrder[1][0][3].y = 1;
    m_wQuadOrder[1][0][4].x = 0;
    m_wQuadOrder[1][0][4].y = 0;

    m_wQuadOrder[1][1][0].x = 0;
    m_wQuadOrder[1][1][0].y = 1;
    m_wQuadOrder[1][1][1].x = 1;
    m_wQuadOrder[1][1][1].y = 0;
    m_wQuadOrder[1][1][2].x = 1;
    m_wQuadOrder[1][1][2].y = 1;
    m_wQuadOrder[1][1][3].x = 0;
    m_wQuadOrder[1][1][3].y = 1;
    m_wQuadOrder[1][1][4].x = 1;
    m_wQuadOrder[1][1][4].y = 0;
}

//******************************************************************************
CVigil::~CVigil() {

    UINT i;

    for (i = 0; i < 6; i++) {
        if (m_ppvTexture[i]) {
            D3D_FreeContiguousMemory(m_ppvTexture[i]);
            m_ppd3dt[m_uTiledTexIndex[i]] = NULL;
        }
    }
    for (i = 0; m_ppd3dt && i < m_uNumTiles; i++) {
        if (m_ppd3dt[i]) {
            m_ppd3dt[i]->Release();
        }
    }
    if (m_ppd3dt) {
        HeapFree(GetProcessHeap(), 0, m_ppd3dt);
    }
#if defined(VIGILGENERATECRCS)
    if (m_pdwTextureCRC) {
        HeapFree(GetProcessHeap(), 0, m_pdwTextureCRC);
    }
#endif

    for (i = 0; m_ppwTile && i < m_uNumTileIndexLists; i++) {
        if (m_ppwTile[i]) {
            HeapFree(GetProcessHeap(), 0, m_ppwTile[i]);
        }
    }
    if (m_ppwTile) {
        HeapFree(GetProcessHeap(), 0, m_ppwTile);
    }
#if defined(VIGILGENERATECRCS)
    if (m_pdwIndexListCRC) {
        HeapFree(GetProcessHeap(), 0, m_pdwIndexListCRC);
    }
#endif

    for (i = 0; m_ppd3dr && i < m_uNumTiles; i++) {
        if (m_ppd3dr[i]) {
            m_ppd3dr[i]->Release();
        }
    }
    if (m_ppd3dr) {
        HeapFree(GetProcessHeap(), 0, m_ppd3dr);
    }
#if defined(VIGILGENERATECRCS)
    if (m_pdwVertexBufferCRC) {
        HeapFree(GetProcessHeap(), 0, m_pdwVertexBufferCRC);
    }
#endif

#if !defined(VIGILGENERATECRCS)
    XFreeSection("crc.log");
#endif

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
BOOL CVigil::Create() {

    D3DPRESENT_PARAMETERS   d3dpp;
    D3DVIEWPORT8            viewport = {0, 0, 640, 480, 0.0f, 1.0f};
    LPDIRECT3DSURFACE8      pd3ds;
    D3DLOCKED_RECT          d3dlr;
    XGMATRIX                mTransform;
    PTVERTEX                prVertices;
    MEMORYSTATUS            ms;
    UINT                    i, j;
    int                     n;
    HRESULT                 hr;

    GlobalMemoryStatus(&ms);
    m_b128M = (ms.dwTotalPhys > 67108864);

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_pd3d) {
        OutputString(TEXT("Direct3DCreate8 failed"));
        return FALSE;
    }

    // Initialize the presentation parameters
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth                   = 640;
    d3dpp.BackBufferHeight                  = 480;
    d3dpp.BackBufferFormat                  = D3DFMT_LIN_A8R8G8B8;
    d3dpp.MultiSampleType                   = D3DMULTISAMPLE_NONE;
    d3dpp.AutoDepthStencilFormat            = D3DFMT_LIN_D24S8;
    d3dpp.FullScreen_RefreshRateInHz        = 0;
    d3dpp.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferCount                   = 1;
    d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                     = NULL;
    d3dpp.Windowed                          = FALSE;
    d3dpp.EnableAutoDepthStencil            = TRUE;

    // Create the device
    hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);
    if (ResultFailed(hr, TEXT("IDirect3D8::CreateDevice failed"))) {
        return FALSE;
    }

    // Set a viewport
    hr = m_pDevice->SetViewport(&viewport);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetViewport"))) {
        return FALSE;
    }

    // Initialize the view matrix
    XGMatrixLookAtLH(&mTransform, &XGVECTOR3(0.0f, 0.0f, -50.0f), &XGVECTOR3(0.0f, 0.0f, 0.0f), &XGVECTOR3(0.0f, 1.0f, 0.0f));
    hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTransform);
    if (FAILED(hr)) {
        OutputString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    // Initialize the projection matrix
    XGMatrixPerspectiveFovLH(&mTransform, 3.14159f / 4.0f, 640.0f / 480.0f, 0.1f, 1000.0f);
    hr = m_pDevice->SetTransform(D3DTS_PROJECTION, &mTransform);
    if (FAILED(hr)) {
        OutputString(TEXT("IDirect3DDevice8::SetTransform failed"));
    }

    m_uNumTiles = 192;
    m_uTileDim = 40;

    for (i = 0; i < 6;) {
        m_uTiledTexIndex[i] = rand() % m_uNumTiles;
        for (j = 0; j < i; j++) {
            if (m_uTiledTexIndex[i] == m_uTiledTexIndex[j]) {
                break;
            }
        }
        if (j == i) {
            i++;
        }
    }

#if defined(VIGILGENERATECRCS)
#if defined(DBG)
    m_hCRCFile = CreateFile("t:\\crc_debug.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    m_hCRCFile = CreateFile("t:\\crc_retail.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
    if (m_hCRCFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
#else
#if defined(DBG)
    m_pdwCRCs = (LPDWORD)XLoadSection("crc_debug.log");
#else
    m_pdwCRCs = (LPDWORD)XLoadSection("crc_retail.log");
#endif
#endif

    if (!CreateTextures()) {
        return FALSE;
    }

    // The vertex buffers must be created after the textures since the texture coordinate format is dependent on the layout of each texture
    if (!CreateVertexBuffers()) {
        return FALSE;
    }

    // Note: The index lists must be created last since the number of them is determined by remaining memory available
    if (!CreateIndexLists()) {
        return FALSE;
    }

    GlobalMemoryStatus(&ms);
//    OutputString(TEXT("Available memory: %d / %d bytes"), ms.dwAvailPhys, ms.dwTotalPhys);
    OutputString(TEXT("%d bytes of memory were left unallocated"), ms.dwAvailPhys);

    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

    m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
    m_pDevice->SetRenderState(D3DRS_STENCILREF, 0xFF);
    m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

    // Create texture objects that point to color buffer memory
    for (n = 0; n < 2; n++) {
        m_pDevice->GetBackBuffer(n-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
        pd3ds->LockRect(&d3dlr, NULL, D3DLOCK_TILED);
        XGSetTextureHeader(256, 256, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_d3dtColorBuffer[n], 0, 0xA00);
//        m_d3dtColorBuffer[n].Register((LPVOID)((DWORD)d3dlr.pBits & 0x0FFFFFFF));
        m_d3dtColorBuffer[n].Data = ((DWORD)d3dlr.pBits & 0x0FFFFFFF);
        pd3ds->UnlockRect();
        pd3ds->Release();
    }

    return TRUE;
}

//******************************************************************************
BOOL CVigil::CreateVertexBuffers() {

    PTVERTEX                prVertices;
    float                   fX, fY, fLen, fScale;
    D3DSURFACE_DESC         d3dsd;
    UINT                    i, j, k;
    HRESULT                 hr;

    OutputStringN(TEXT("Creating %d vertex buffers."), m_uNumTiles);

#if defined(VIGILGENERATECRCS)
    DWORD dwWritten;
    WriteFile(m_hCRCFile, &m_uNumTiles, sizeof(DWORD), &dwWritten, NULL);

    m_pdwVertexBufferCRC = (LPDWORD)HeapAlloc(GetProcessHeap(), 0, m_uNumTiles * sizeof(DWORD));
    if (!m_pdwVertexBufferCRC) {
        return FALSE;
    }
#else
    if (*m_pdwCRCs++ != m_uNumTiles) {
        OutputString(TEXT("the number of vertex buffer CRCs required does not match the number stored in the CRC file"));
        __asm int 3;
    }
    m_pdwVertexBufferCRC = m_pdwCRCs;
    m_pdwCRCs += m_uNumTiles;
#endif

    m_ppd3dr = (LPDIRECT3DVERTEXBUFFER8*)HeapAlloc(GetProcessHeap(), 0, m_uNumTiles * sizeof(LPDIRECT3DVERTEXBUFFER8));
    if (!m_ppd3dr) {
        return FALSE;
    }
    memset(m_ppd3dr, 0, m_uNumTiles * sizeof(LPDIRECT3DVERTEXBUFFER8));

    m_uVertexBufferSize = (m_uTileDim + 1) * (m_uTileDim + 1) * sizeof(TVERTEX);

    for (i = 0; i < m_uNumTiles; i++) {

        m_ppd3dt[i]->GetLevelDesc(0, &d3dsd);
        if (XGIsSwizzledFormat(d3dsd.Format)) {
            fScale = 1.0f;
        }
        else {
            fScale = 256.0f;
        }

        OutputStringV(TEXT("Creating vertex buffer %d..."), i+1);
        hr = m_pDevice->CreateVertexBuffer(m_uVertexBufferSize, 0, FVF_TVERTEX, 0, &m_ppd3dr[i]);
        if (FAILED(hr)) {
            OutputString(TEXT("Failed to create vertex buffer %d [0x%X]"), i+1, hr);
            return FALSE;
        }
        else {
            OutputStringV(TEXT("succeeded.\r\n"));
        }

        m_ppd3dr[i]->Lock(0, 0, (LPBYTE*)&prVertices, 0);

        OutputStringV(TEXT("Populating vertex buffer %d..."), i+1);

        fX = (float)((i % (640 / m_uTileDim)) * m_uTileDim);
        fY = (float)((i / (640 / m_uTileDim)) * m_uTileDim);
        fLen = (float)(256 / m_uTileDim) * (float)m_uTileDim / 256.0f * fScale;

        for (j = 0; j <= m_uTileDim; j++) {
            for (k = 0; k <= m_uTileDim; k++) {
                prVertices[j * (m_uTileDim + 1) + k] = TVERTEX(XGVECTOR3(fX + (float)k, fY + (float)j, 0.5f), 2.0f, (float)k / (float)m_uTileDim * fLen, (float)j / (float)m_uTileDim * fLen);
            }
        }

        OutputStringV(TEXT("succeeded.\r\n"));

#if defined(VIGILGENERATECRCS)
        OutputStringV(TEXT("Generating CRC for vertex buffer %d..."), i+1);
        m_pdwVertexBufferCRC[i] = ComputeCRC32(prVertices, m_uVertexBufferSize);
        WriteFile(m_hCRCFile, &m_pdwVertexBufferCRC[i], sizeof(DWORD), &dwWritten, NULL);
        OutputStringV(TEXT("succeeded with 0x%X.\r\n"), m_pdwVertexBufferCRC[i]);
#endif

        m_ppd3dr[i]->Unlock();

        if (i % 10 == 0) {
            OutputStringN(TEXT("."));
        }
    }

    OutputStringN(TEXT("completed.\r\n"));

    return TRUE;
}

//******************************************************************************
BOOL CVigil::CreateTextures() {

    PTVERTEX                prVertices;
    TEXLAYOUT               layout;
    D3DLOCKED_RECT          d3dlr;
    LPDWORD                 pdwTexel;
    Swizzler                swz(256, 256, 1);
    D3DTILE                 tile;
    UINT                    uNumTiledTextures = 0;
    UINT                    x, y;
    UINT                    uInterval;
    UINT                    i, j, k;
    UINT                    u, v;
    HRESULT                 hr;

    OutputStringN(TEXT("Creating %d textures."), m_uNumTiles);

#if defined(VIGILGENERATECRCS)
    DWORD dwWritten;
    WriteFile(m_hCRCFile, &m_uNumTiles, sizeof(DWORD), &dwWritten, NULL);

    m_pdwTextureCRC = (LPDWORD)HeapAlloc(GetProcessHeap(), 0, m_uNumTiles * sizeof(DWORD));
    if (!m_pdwTextureCRC) {
        return FALSE;
    }
#else
    if (*m_pdwCRCs++ != m_uNumTiles) {
        OutputString(TEXT("the number of texture CRCs required does not match the number stored in the CRC file"));
        __asm int 3;
    }
    m_pdwTextureCRC = m_pdwCRCs;
    m_pdwCRCs += m_uNumTiles;
#endif

    m_ppd3dt = (LPDIRECT3DTEXTURE8*)HeapAlloc(GetProcessHeap(), 0, m_uNumTiles * sizeof(LPDIRECT3DTEXTURE8));
    if (!m_ppd3dt) {
        return FALSE;
    }
    memset(m_ppd3dt, 0, m_uNumTiles * sizeof(LPDIRECT3DTEXTURE8));

    m_uTextureSize = 256 * 256 * 4;

    uInterval = (256 / m_uTileDim);

    memset(&tile, 0, sizeof(D3DTILE));
    tile.Size = 256 * 256 * 4;
    tile.Pitch = D3DTILE_PITCH_0400;

    for (i = 0; i < m_uNumTiles; i++) {

        OutputStringV(TEXT("Creating texture %d..."), i+1);

        layout = (TEXLAYOUT)(rand() % 2);
        for (j = 0; j < 6; j++) {
            if (i == m_uTiledTexIndex[j]) {
                layout = TL_TILED;
                break;
            }
        }

        if (layout == TL_TILED) {

            m_ppvTexture[uNumTiledTextures] = D3D_AllocContiguousMemory(256 * 256 * 4, D3DTILE_ALIGNMENT);
            if (!m_ppvTexture[uNumTiledTextures]) {
                OutputString(TEXT("Failed to allocate contiguous memory for texture %d [0x%X]"), i+1, hr);
                return FALSE;
            }
            tile.pMemory = m_ppvTexture[uNumTiledTextures];

            hr = m_pDevice->SetTile(2+uNumTiledTextures, &tile);
            if (FAILED(hr)) {
                OutputString(TEXT("Failed to set tile region for texture %d [0x%X]"), i+1, hr);
                return FALSE;
            }

            XGSetTextureHeader(256, 256, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pd3dt[uNumTiledTextures], 0, 0x400);
            m_pd3dt[uNumTiledTextures].Register(m_ppvTexture[uNumTiledTextures]);

            m_ppd3dt[i] = &m_pd3dt[uNumTiledTextures];

            uNumTiledTextures++;
        }
        else {
            hr = m_pDevice->CreateTexture(256, 256, 1, 0, (layout == TL_SWIZZLED) ? D3DFMT_A8R8G8B8 : D3DFMT_LIN_A8R8G8B8, 0, &m_ppd3dt[i]);
            if (FAILED(hr)) {
                OutputString(TEXT("Failed to create texture %d [0x%X]"), i+1, hr);
                return FALSE;
            }
        }

        OutputStringV(TEXT("succeeded.\r\n"));

        OutputStringV(TEXT("Populating texture %d..."), i+1);

        m_ppd3dt[i]->LockRect(0, &d3dlr, NULL, (layout == TL_TILED) ? D3DLOCK_TILED : 0);

        x = i % (640 / m_uTileDim) * m_uTileDim;
        y = i / (640 / m_uTileDim) * m_uTileDim;

        pdwTexel = (LPDWORD)d3dlr.pBits;

        for (j = 0; j < 256; j++, swz.IncV()) {
            for (k = 0; k < 256; k++, swz.IncU()) {
                if (!(j % uInterval) && !(k % uInterval)) {
                    u = x + k / uInterval;
                    v = y + j / uInterval;
                    if (u >= 512) {
                        u -= 512;
                    }
                    else if (u >= 256) {
                        u = 255 - (u - 256);
                    }
                    if (v >= 256) {
                        v = 255 - (v - 256);
                    }
                    if (layout == TL_SWIZZLED) {
                        pdwTexel[swz.Get2D()] = D3DCOLOR_RGBA((BYTE)v, (BYTE)u, 255 - (BYTE)u, rand() % 256);
                    }
                    else {
                        pdwTexel[k] = D3DCOLOR_RGBA((BYTE)v, (BYTE)u, 255 - (BYTE)u, rand() % 256);
                    }
                }
                else {
                    if (layout == TL_SWIZZLED) {
                        pdwTexel[swz.Get2D()] = D3DCOLOR_RGBA(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
                    }
                    else {
                        pdwTexel[k] = D3DCOLOR_RGBA(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
                    }
                }
            }
            if (layout != TL_SWIZZLED) {
                pdwTexel += d3dlr.Pitch >> 2;
            }
        }

        m_ppd3dt[i]->UnlockRect(0);

        OutputStringV(TEXT("succeeded.\r\n"));

#if 0
        if (layout != TL_SWIZZLED) {
            m_ppd3dr[i]->Lock(0, 0, (LPBYTE*)&prVertices, 0);
            for (j = 0; j < (m_uTileDim + 1) * (m_uTileDim + 1); j++) {
                prVertices[j].u0 *= 256.0f;
                prVertices[j].v0 *= 256.0f;
            }
            m_ppd3dr[i]->Unlock();
        }
#endif

#if defined(VIGILGENERATECRCS)
        OutputStringV(TEXT("Generating CRC for texture %d..."), i+1);
        m_pdwTextureCRC[i] = ComputeTextureCRC32(m_ppd3dt[i]);
        WriteFile(m_hCRCFile, &m_pdwTextureCRC[i], sizeof(DWORD), &dwWritten, NULL);
        OutputStringV(TEXT("succeeded with 0x%X.\r\n"), m_pdwTextureCRC[i]);
#endif

        if (i % 10 == 0) {
            OutputStringN(TEXT("."));
        }
    }

    OutputStringN(TEXT("completed.\r\n"));

    return TRUE;
}

//******************************************************************************
BOOL CVigil::CreateIndexLists() {

    MEMORYSTATUS            ms;
    UINT                    uIndex;
    UINT                    i, j, k;
    BYTE                    def, ord;
    HRESULT                 hr;

    GlobalMemoryStatus(&ms);
    if (ms.dwAvailPhys > 67108864) {
        ms.dwAvailPhys -= 67108864;
    }

    OutputStringN(TEXT("Creating index lists."));

    m_uNumTileIndices = m_uTileDim * m_uTileDim * 6;
    m_uIndexListSize = m_uNumTileIndices * sizeof(WORD);
#if 0
    m_uNumTileIndexLists = (ms.dwAvailPhys - (ms.dwAvailPhys / m_uIndexListSize) * sizeof(DWORD)) / m_uIndexListSize;
//    m_uNumTileIndexLists = ms.dwAvailPhys / m_uIndexListSize;
    if (m_uNumTileIndexLists == 0) {
        return FALSE;
    }
#else
#if defined(DBG)
    m_uNumTileIndexLists = 113;
#else
    m_uNumTileIndexLists = 139;
#endif
#endif

#if defined(VIGILGENERATECRCS)
    DWORD dwWritten;
    WriteFile(m_hCRCFile, &m_uNumTileIndexLists, sizeof(DWORD), &dwWritten, NULL);

    m_pdwIndexListCRC = (LPDWORD)HeapAlloc(GetProcessHeap(), 0, m_uNumTileIndexLists * sizeof(DWORD));
    if (!m_pdwIndexListCRC) {
        return FALSE;
    }
#else
    if (*m_pdwCRCs++ != m_uNumTileIndexLists) {
        OutputString(TEXT("the number of index list CRCs required does not match the number stored in the CRC file"));
        __asm int 3;
    }
    m_pdwIndexListCRC = m_pdwCRCs;
    m_pdwCRCs += m_uNumTileIndexLists;
#endif

    m_ppwTile = (LPWORD*)HeapAlloc(GetProcessHeap(), 0, m_uNumTileIndexLists * sizeof(LPWORD));
    if (!m_ppwTile) {
        return FALSE;
    }
    memset(m_ppwTile, 0, m_uNumTileIndexLists * sizeof(LPWORD));

    for (i = 0; i < m_uNumTileIndexLists; i++) {
        OutputStringV(TEXT("Creating index list %d..."), i+1);
        m_ppwTile[i] = (LPWORD)HeapAlloc(GetProcessHeap(), 0, m_uIndexListSize);
        if (!m_ppwTile[i]) {
#if 0
            if (i == 0) {
                OutputString(TEXT("Failed to create index list %d"), i+1);
                return FALSE;
            }
            else {
                m_uNumTileIndexLists = i;
                break;
            }
#else
            OutputString(TEXT("Failed to create index list %d"), i+1);
            return FALSE;
#endif
        }
        else {
            OutputStringV(TEXT("succeeded.\r\n"));
        }

        OutputStringV(TEXT("Populating index list %d..."), i+1);

        uIndex = 0;

        for (j = 0; j < m_uTileDim; j++) {
            for (k = 0; k < m_uTileDim; k++) {
                def = rand() % 2;
                ord = rand() % 3;
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][0][ord  ].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][0][ord  ].x));
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][0][ord+1].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][0][ord+1].x));
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][0][ord+2].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][0][ord+2].x));
                ord = rand() % 3;
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][1][ord  ].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][1][ord  ].x));
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][1][ord+1].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][1][ord+1].x));
                m_ppwTile[i][uIndex++] = (WORD)((j + m_wQuadOrder[def][1][ord+2].y) * (m_uTileDim + 1) + (k + m_wQuadOrder[def][1][ord+2].x));
            }
        }

        OutputStringV(TEXT("succeeded.\r\n"));

#if defined(VIGILGENERATECRCS)
        OutputStringV(TEXT("Generating CRC for index list %d..."), i+1);
        m_pdwIndexListCRC[i] = ComputeCRC32(m_ppwTile[i], m_uIndexListSize);
        WriteFile(m_hCRCFile, &m_pdwIndexListCRC[i], sizeof(DWORD), &dwWritten, NULL);
        OutputStringV(TEXT("succeeded with 0x%X.\r\n"), m_pdwIndexListCRC[i]);
#endif

        if (i % 10 == 0) {
            OutputStringN(TEXT("."));
        }
    }

    OutputStringN(TEXT("completed. (%d created)\r\n"), m_uNumTileIndexLists);

    return TRUE;
}

//******************************************************************************
BOOL CVigil::Run() {

    float fTime, fLastTime, fTimeFreq, fFPS, fPPS;
    LARGE_INTEGER qwCounter;
    LPDIRECT3DSURFACE8 pd3ds, pd3dsColor, pd3dsDepth;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlr;
    LPBYTE pData;
    DWORD dwCRC;
    RECT rect = {0, 0, 256, 256};
    UINT uFrame = 1;
    UINT uFrameCycle = 0;
    BOOL bOutputFPS = FALSE;
    BOOL bCheckResources = FALSE;
    UINT i, j;

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;

    do {

        // Clear the rendering target
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, (float)rand() / (float)RAND_MAX, 0);

#if !defined (VIGILNORESOURCECHECK)
        bCheckResources = TRUE;
#endif

        if (bCheckResources && uFrame > 1) {

            // Verify index lists
            for (i = 0; i < m_uNumTileIndexLists; i++) {
                OutputStringV(TEXT("Checking memory integrity of index list %d..."), i);
                dwCRC = ComputeCRC32(m_ppwTile[i], m_uIndexListSize);
                if (dwCRC != m_pdwIndexListCRC[i]) {
                    OutputString(TEXT("Index list %d memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), i, uFrame, m_pdwIndexListCRC[i], dwCRC);
                }
                else {
                    OutputStringV(TEXT("passed.\r\n"));
                }
            }

            // Verify vertex buffers and textures
            for (i = 0; i < m_uNumTiles; i++) {

                OutputStringV(TEXT("Checking memory integrity of vertex buffer %d..."), i);
                m_ppd3dr[i]->Lock(0, 0, &pData, 0);
                dwCRC = ComputeCRC32(pData, m_uVertexBufferSize);
                m_ppd3dr[i]->Unlock();
                if (dwCRC != m_pdwVertexBufferCRC[i]) {
                    OutputString(TEXT("Vertex buffer %d memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), i, uFrame, m_pdwVertexBufferCRC[i], dwCRC);
                }
                else {
                    OutputStringV(TEXT("passed.\r\n"));
                }

                OutputStringV(TEXT("Checking memory integrity of texture %d..."), i);
                dwCRC = ComputeTextureCRC32(m_ppd3dt[i]);
                if (dwCRC != m_pdwTextureCRC[i]) {
                    OutputString(TEXT("Texture %d memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), i, uFrame, m_pdwTextureCRC[i], dwCRC);
                }
                else {
                    OutputStringV(TEXT("passed.\r\n"));
                }
            }
        }

        bCheckResources = FALSE;

        // Begin the scene
        m_pDevice->BeginScene();

        // Disable lighting
        m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        m_pDevice->SetVertexShader(FVF_TVERTEX);

#if !defined (VIGILNOTEXTUREWRITES)

        m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3dsColor);
        m_pDevice->GetDepthStencilSurface(&pd3dsDepth);

        for (i = 0; i < m_uNumTiles; i++) {

            m_ppd3dt[i]->GetSurfaceLevel(0, &pd3ds);
            pd3ds->GetDesc(&d3dsd);

            m_pDevice->SetTexture(0, m_ppd3dt[i]);
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, (d3dsd.Format == D3DFMT_A8R8G8B8) ? m_prSwizzledQuad : m_prLinearQuad, sizeof(TVERTEX));

            m_pDevice->SetRenderTarget(pd3ds, NULL);
            if (uFrame % 2) {
                pd3ds->LockRect(&d3dlr, NULL, 0);
                pData = (LPBYTE)d3dlr.pBits;
                for (j = 0; j < d3dsd.Height; j++) {
                    memset(pData, 0, d3dsd.Width << 2);
                    pData += d3dlr.Pitch;
                }
                pd3ds->UnlockRect();
            }
            else {
                m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
            }

            m_pDevice->SetTexture(0, &m_d3dtColorBuffer[uFrame%2]);
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prLinearQuad, sizeof(TVERTEX));

            m_pDevice->SetRenderTarget(pd3dsColor, pd3dsDepth);

//            m_pDevice->CopyRects(pd3ds, NULL, 0, pd3dsColor, NULL);
//            m_pDevice->CopyRects(pd3dsColor, &rect, 1, pd3ds, NULL);
            pd3ds->Release();
        }

        pd3dsDepth->Release();
        pd3dsColor->Release();

#endif

        for (i = 0; i < m_uNumTiles; i++) {
            m_pDevice->SetStreamSource(0, m_ppd3dr[i], sizeof(TVERTEX));
            m_pDevice->SetTexture(0, m_ppd3dt[i]);
            m_pDevice->DrawIndexedVertices(D3DPT_TRIANGLELIST, m_uNumTileIndices, m_ppwTile[i % m_uNumTileIndexLists]);
        }

        // End the scene
        m_pDevice->EndScene();

        if (uFrame > 1) {
            // Verify the front buffer
            OutputStringV(TEXT("Checking memory integrity of the front buffer..."));
            m_pDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
            dwCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            if (dwCRC != m_dwFrontBufferCRC) {
                OutputString(TEXT("Front buffer memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), uFrame, m_dwFrontBufferCRC, dwCRC);
            }
            else {
                OutputStringV(TEXT("passed.\r\n"));
            }

            // Verify the back buffer
            OutputStringV(TEXT("Checking memory integrity of the back buffer..."));
            m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
            dwCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            if (dwCRC != m_dwBackBufferCRC) {
                OutputString(TEXT("Back buffer memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), uFrame, m_dwBackBufferCRC, dwCRC);
                bCheckResources = TRUE;
            }
            else {
                OutputStringV(TEXT("passed.\r\n"));
            }

            // Verify the depth buffer
            OutputStringV(TEXT("Checking memory integrity of the depth buffer..."));
            m_pDevice->GetDepthStencilSurface(&pd3ds);
            dwCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            if (dwCRC != m_dwDepthBufferCRC) {
                OutputString(TEXT("Depth buffer memory corrupt on frame %d (Expected CRC: 0x%X, Actual CRC: 0x%X)"), uFrame, m_dwDepthBufferCRC, dwCRC);
            }
            else {
                OutputStringV(TEXT("passed.\r\n"));
            }

            m_pDevice->Present(NULL, NULL, NULL, NULL);
        }
        else {
#if defined(VIGILGENERATECRCS)
            DWORD dwWritten;
            m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
            m_dwBackBufferCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            m_pDevice->GetDepthStencilSurface(&pd3ds);
            m_dwDepthBufferCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            m_pDevice->Present(NULL, NULL, NULL, NULL);
            m_pDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
            m_dwFrontBufferCRC = ComputeSurfaceCRC32(pd3ds);
            pd3ds->Release();
            WriteFile(m_hCRCFile, &m_dwFrontBufferCRC, sizeof(DWORD), &dwWritten, NULL);
            WriteFile(m_hCRCFile, &m_dwBackBufferCRC, sizeof(DWORD), &dwWritten, NULL);
            WriteFile(m_hCRCFile, &m_dwDepthBufferCRC, sizeof(DWORD), &dwWritten, NULL);
            CloseHandle(m_hCRCFile);
            OutputString(TEXT("CRC file generation complete."));
#else
            m_dwFrontBufferCRC = *m_pdwCRCs++;
            m_dwBackBufferCRC = *m_pdwCRCs++;
            m_dwDepthBufferCRC = *m_pdwCRCs++;
            m_pDevice->Present(NULL, NULL, NULL, NULL);
#endif
            OutputString(TEXT("Initialization complete."));
        }

        uFrame++;
        uFrameCycle++;

        QueryPerformanceCounter(&qwCounter);
        fTime = (float)qwCounter.QuadPart * fTimeFreq;

        if (fTime - fLastTime > 1.0f) {
            fFPS = (float)uFrameCycle / (fTime - fLastTime);
            fPPS = (float)(uFrameCycle * m_uTileDim * m_uTileDim * 2 * m_uNumTiles) / (fTime - fLastTime);
            if (bOutputFPS) {
                OutputStringV(TEXT("\r\n"));
                OutputString(TEXT("%3.3f frames per second (%3.3f triangles per second)"), fFPS, fPPS);
                OutputStringV(TEXT("\r\n"));
            }
            QueryPerformanceCounter(&qwCounter);
            fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
            uFrameCycle = 0;
            bOutputFPS = TRUE;
        }

    } while (TRUE);

    return TRUE;
}

//******************************************************************************
DWORD CVigil::ComputeCRC32(LPVOID pvData, UINT uNumBytes) {

    LPBYTE pData = (LPBYTE)pvData;
    DWORD  dwCRC = 0;
    UINT   i;

    for (i = 0; i < uNumBytes; i++) {
        dwCRC = m_dwCRCTable[(dwCRC ^ pData[i]) & 0xFF] ^ (dwCRC >> 8);
    }

    return dwCRC;
}

//******************************************************************************
DWORD CVigil::ComputeTextureCRC32(LPDIRECT3DTEXTURE8 pd3dt) {
    
    LPDIRECT3DSURFACE8 pd3ds;
    DWORD              dwCRC;

    pd3dt->GetSurfaceLevel(0, &pd3ds);
    dwCRC = ComputeSurfaceCRC32(pd3ds);
    pd3ds->Release();
    return dwCRC;
}

//******************************************************************************
DWORD CVigil::ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds) {

    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    DWORD           dwCRC = 0;
    LPBYTE          pbData;
    UINT            i, j, k;
    HRESULT         hr;

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
        return 0;
    }

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return 0;
    }

    switch (d3dsd.Format) {

        case D3DFMT_LIN_R5G6B5:
        case D3DFMT_R5G6B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0xF800) << 8 |
                              (pwPixel[j] & 0x07E0) << 5 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_X1R5G5B5:
        case D3DFMT_X1R5G5B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0x7C00) << 9 |
                              (pwPixel[j] & 0x03E0) << 6 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_A1R5G5B5:
        case D3DFMT_A1R5G5B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0x8000) << 16 |
                              (pwPixel[j] & 0x7C00) << 9 |
                              (pwPixel[j] & 0x03E0) << 6 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 4; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_X8R8G8B8: {
            LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    pbData = (LPBYTE)&pdwPixel[j];
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pdwPixel += d3dlr.Pitch >> 2;
            }
            break;
        }

        case D3DFMT_LIN_D24S8:
        case D3DFMT_LIN_A8R8G8B8:
        case D3DFMT_D24S8:
        case D3DFMT_A8R8G8B8: {
            LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    pbData = (LPBYTE)&pdwPixel[j];
                    for (k = 0; k < 4; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pdwPixel += d3dlr.Pitch >> 2;
            }
            break;
        }

        default: // Unsupported format
            __asm int 3;
    }

    pd3ds->UnlockRect();

    return dwCRC;
}

//******************************************************************************
//
// Function:
//
//     OutputString
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CVigil::OutputString(LPCTSTR szFormat, ...) {

    va_list vl;

    va_start(vl, szFormat);
    _vstprintf(m_szOutput, szFormat, vl);
    va_end(vl);

    _tcscat(m_szOutput, TEXT("\r\n"));

    OutputDebugString(m_szOutput);
}

#if VIGILVERBOSE

//******************************************************************************
//
// Function:
//
//     OutputStringV
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CVigil::OutputStringV(LPCTSTR szFormat, ...) {

    va_list vl;

    va_start(vl, szFormat);
    _vstprintf(m_szOutput, szFormat, vl);
    va_end(vl);

    OutputDebugString(m_szOutput);
}

#else

//******************************************************************************
//
// Function:
//
//     OutputStringN
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CVigil::OutputStringN(LPCTSTR szFormat, ...) {

    va_list vl;

    va_start(vl, szFormat);
    _vstprintf(m_szOutput, szFormat, vl);
    va_end(vl);

    OutputDebugString(m_szOutput);
}

#endif

//******************************************************************************
//
// Function:
//
//     ResultFailed
//
// Description:
//
//     Test a given return code: if the code is an error, output a debug 
//     message with the error value.  If the code is a warning, output
//     a debug message with the warning value.
//
// Arguments:
//
//     HRESULT hr               - Return code to test for an error
//
//     LPCTSTR sz               - String describing the method that produced 
//                                the return code
//
// Return Value:
//
//     TRUE if the given return code is an error, FALSE otherwise.
//
//******************************************************************************
BOOL CVigil::ResultFailed(HRESULT hr, LPCTSTR sz) {

//    TCHAR szError[256];

    if (SUCCEEDED(hr)) {
        return FALSE;
    }

//    XGGetErrorString(hr, szError, 256);

    if (FAILED(hr)) {
//        OutputString(TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        OutputString(TEXT("%s failed with [0x%X]"), sz, hr);
        return TRUE;
    }
    else {
//        OutputString(TEXT("%s returned %s [0x%X]"), sz, szError, hr);
        OutputString(TEXT("%s returned [0x%X]"), sz, hr);
    }

    return FALSE;
}
