/*==========================================================================;
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       leaks.cpp
 *  Content:    Tests for leaks
 *
 ***************************************************************************/
 
#include "precomp.hpp" 

D3DFORMAT g_BackBufferFormats[] =
{
    D3DFMT_X8R8G8B8,
    D3DFMT_A8R8G8B8,
    D3DFMT_R5G6B5,
    D3DFMT_X1R5G5B5,
};

D3DFORMAT g_DepthFormats[] =
{
    D3DFMT_D24S8,
    D3DFMT_F24S8,
    D3DFMT_D16,
    D3DFMT_F16,
};

DWORD g_MultisampleTypes[] =
{
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,   
    D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,   
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,   
    D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,   
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,   
    D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,   
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,   
    D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,   
    D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,   
    D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,   
};

DWORD g_BackCounts[] =
{
    0, 1, 2,
};

DWORD g_Intervals[] =
{
    D3DPRESENT_INTERVAL_IMMEDIATE,
    D3DPRESENT_INTERVAL_ONE,
    D3DPRESENT_INTERVAL_TWO,
};

//------------------------------------------------------------------------------
void TestLeaks()
{
    DWORD count;
    IDirect3D8* pD3D;
    IDirect3DDevice8* pDev;
    D3DTexture* pTexture;
    D3DSurface* pTextureSurface;
    D3DSurface* pDepthSurface;
    D3DPalette* pPalette;
    D3DVertexBuffer* pVertexBuffer;
    D3DIndexBuffer* pIndexBuffer;
    DWORD alphaTestEnable;
    DWORD minFilter;
    DWORD texCoordIndex;
    D3DPRESENT_PARAMETERS d3dpp;

    pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    ASSERT(pD3D != NULL);

#if DBG
    ASSERT((D3D__AllocsContiguous == 0) && (D3D__AllocsNoncontiguous == 0));
#endif

    count = 0;

    for (DWORD iReset = FALSE;
         iReset <= TRUE;
         iReset++)
    {
        if (iReset)
        {
            // Create a device using the last presentation parameters used
            // by the non-reset case:

            CheckHR(pD3D->CreateDevice(
                            D3DADAPTER_DEFAULT,
                            D3DDEVTYPE_HAL,
                            NULL,
                            D3DCREATE_HARDWARE_VERTEXPROCESSING,
                            &d3dpp,
                            &pDev));
        }

        for (DWORD iBackBufferFormat = 0; 
             iBackBufferFormat < ARRAYSIZE(g_BackBufferFormats); 
             iBackBufferFormat++)
        {
            for (DWORD iDepthFormat = 0; 
                 iDepthFormat < ARRAYSIZE(g_DepthFormats); 
                 iDepthFormat++)
            {
                for (DWORD iMultisampleType = 0; 
                     iMultisampleType < ARRAYSIZE(g_MultisampleTypes); 
                     iMultisampleType++)
                {
                    for (DWORD iBackCount = 0; 
                         iBackCount < ARRAYSIZE(g_BackCounts); 
                         iBackCount++)
                    {
                        for (DWORD iInterval = 0; 
                             iInterval < ARRAYSIZE(g_Intervals); 
                             iInterval++)
                        {
                            ZeroMemory(&d3dpp, sizeof(d3dpp));
                        
                            d3dpp.BackBufferWidth        = 640;
                            d3dpp.BackBufferHeight       = 480;
                            d3dpp.EnableAutoDepthStencil = true;
                            d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
                            d3dpp.BackBufferFormat       = g_BackBufferFormats[iBackBufferFormat];
                            d3dpp.BackBufferCount        = g_BackCounts[iBackCount];
                            d3dpp.AutoDepthStencilFormat = g_DepthFormats[iDepthFormat];
                            d3dpp.MultiSampleType        = g_MultisampleTypes[iMultisampleType];
                        
                            count++;
                            if (iReset)
                            {
                                if ((count % 100) == 0)
                                    DbgPrint("Reset: %li\n", count);

                                CheckHR(pDev->Reset(&d3dpp));
                            }
                            else
                            {
                                if ((count % 100) == 0)
                                    DbgPrint("CreateDevice: %li\n", count);

                                CheckHR(pD3D->CreateDevice(
                                                D3DADAPTER_DEFAULT,
                                                D3DDEVTYPE_HAL,
                                                NULL,
                                                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                                &d3dpp,
                                                &pDev));
                            }
    
                            // Now create and select all the resource types:

                            CheckHR(pDev->CreateImageSurface(1024, 1024, D3DFMT_LIN_F24S8, &pDepthSurface));
                            CheckHR(pDev->CreateTexture(1024, 1024, 1, 0, D3DFMT_A8R8G8B8, 0, &pTexture));
                            CheckHR(pTexture->GetSurfaceLevel(0, &pTextureSurface));
    
                            CheckHR(pDev->SetTexture(0, pTexture));
                            CheckHR(pDev->SetTexture(1, pTexture));
                            CheckHR(pDev->SetTexture(2, pTexture));
                            CheckHR(pDev->SetTexture(3, pTexture));
                            CheckHR(pDev->SetRenderTarget(pTextureSurface, pDepthSurface));
    
                            CheckHR(pDev->CreatePalette(D3DPALETTE_256, &pPalette));
                            CheckHR(pDev->SetPalette(0, pPalette));
                            CheckHR(pDev->SetPalette(1, pPalette));
                            CheckHR(pDev->SetPalette(2, pPalette));
                            CheckHR(pDev->SetPalette(3, pPalette));
    
                            CheckHR(pDev->CreateVertexBuffer(32768, 0, 0, 0, &pVertexBuffer));
                            for (DWORD i = 0; i < 15; i++) // Can't be 16, we'd overflow
                            {
                                CheckHR(pDev->SetStreamSource(i, pVertexBuffer, 32));
                            }
    
                            CheckHR(pDev->CreateIndexBuffer(32768, 0, D3DFMT_INDEX16, 0, &pIndexBuffer));
                            CheckHR(pDev->SetIndices(pIndexBuffer, 0));
    
                            // Do a Present, verifying that some states are preserved across
                            // an FSAA Present call:
    
                            CheckHR(pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));
                            CheckHR(pDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT));
                            CheckHR(pDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 3));
                            CheckHR(pDev->Clear(0, NULL, D3DCLEAR_TARGET, 0, 0, 0));
                            CheckHR(pDev->Present(NULL, NULL, NULL, NULL));
                            CheckHR(pDev->GetRenderState(D3DRS_ALPHATESTENABLE, &alphaTestEnable));
                            CheckHR(pDev->GetTextureStageState(0, D3DTSS_MINFILTER, &minFilter));
                            CheckHR(pDev->GetTextureStageState(0, D3DTSS_TEXCOORDINDEX, &texCoordIndex));
    
                            ASSERT((alphaTestEnable == TRUE) && (minFilter == D3DTEXF_POINT) && (texCoordIndex == 3));

                            if (!iReset)
                            {
                                pDev->Release();
                                pDev = NULL;
                            }
    
                            // Now release any created objects:
    
                            pTextureSurface->Release();
                            pTexture->Release();
                            pDepthSurface->Release();
                            pPalette->Release();
                            pVertexBuffer->Release();
                            pIndexBuffer->Release();
    
                            // Finally, assert that everything is back to zero.  
                            // (We can't do this now for the Reset case because
                            // it can have a bunch of frame buffer objects and
                            // stuff active.)
    
                        #if DBG
                            if (!iReset)
                            {
                                ASSERT((D3D__AllocsContiguous == 0) && (D3D__AllocsNoncontiguous == 0));
                            }
                        #endif
                        }
                    }
                }
            }
        }

        if (iReset)
        {
            pDev->Release();
        #if DBG
            ASSERT((D3D__AllocsContiguous == 0) && (D3D__AllocsNoncontiguous == 0));
        #endif
        }
    }

    pD3D->Release();
}

