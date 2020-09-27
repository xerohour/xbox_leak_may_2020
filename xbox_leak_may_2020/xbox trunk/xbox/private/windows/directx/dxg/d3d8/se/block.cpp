/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       block.cpp
 *  Content:    State block implementation.
 *
 * The whole goal of the Xbox state block implementation is to add as little
 * overhead to non-state-block functionality as is possible.  Specifically, 
 * we have gone out of our way to avoid adding any run-time checks for 
 * 'record' mode to any of our state-setting routines such as SetRenderState.
 *
 * DOC: Need note about how Registered objects are not referenced properly
 *      by state blocks
 *
 ****************************************************************************/

#include "precomp.hpp"
 
// Optimize this module for size:
 
#pragma optimize("s", on)

// !!! Fix state blocks to do render states in reverse order (for ZBIAS)
// !!! Remove random API RECORDSTATE restrictions
// !!! DOC: State programs can modify constant registers and we'll never know
// !!! Gotta fix for 'direct' APIs like SetVertexInputs 
// !!! Add SetScissors support (and to Present!)

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// Don't use the d3d8.h versions of these macros because we don't compile 
// with D3DCOMPILE_BEGINSTATEBLOCK defined.

#define DIRTY_RENDERSTATE(state)                                     \
    { D3D__StateBlockDirty[D3DSBD_RENDERSTATES + (state)] = TRUE; }

#define DIRTY_TEXTURESTATE(stage, state)                             \
    { D3D__StateBlockDirty[D3DSBD_TEXTURESTATES +                    \
                      ((state) * D3DTSS_MAXSTAGES) + (stage)] = TRUE; } 

//------------------------------------------------------------------------------
// D3D__StateBlockDirty[]
//
// An array of bytes that correspond to changed state between BeginStateBlock 
// and EndStateBlock calls.
//
// NOTE: If the game calls no APIs in this module (as will be the case
//       most of the time), the linker will nicely remove this BIG
//       global allocation:
//
// PERF: Make sure things stay that way!

extern "C" 
{
    BYTE D3D__StateBlockDirty[(D3DSBD_MAX + 3) & ~3];
}

//------------------------------------------------------------------------------
// StateBlock
//
// Structure that we allocate to replay state-blocks when called by
// ApplyStateBlock.

struct StateBlock
{
#if DBG

    DWORD Signature;            // Always 'Sblk'
    DWORD* pDataEnd;            // Points one dword past the last Data[] value

#endif

    // NOTE: Each of the following fields represent the count of their
    //       corresponding entries in the variably-formatted 'Data'
    //       array.
    //
    // NOTE: The order of the variably-formatted 'Data' array also
    //       matches the order in which these fields are listed.
    //
    // NOTE: Pixel shaders REQUIRE that the restore order be:
    //          1. Pixel shader
    //          2. Pixel shader constants
    //          3. Pixel shader render states
    //
    // NOTE: Vertex shaders REQUIRE that the restore order be:
    //          1. Vertex shader
    //          2. Vertex shader constants

    DWORD D0_Textures;             
    BOOL D1_PixelShader;
    BOOL D2_VertexShader;
    BOOL D3_Index;
    DWORD D4_Streams;
    DWORD D6_PixelShaderConstants;
    DWORD D7_VertexShaderConstants;
    DWORD D8_RenderStates;         
    DWORD D9_TextureStates;   
    DWORD D10_Transforms;           
    BOOL D11_Viewport;             
    BOOL D12_Material;             
    BOOL D13_BackMaterial;             
    DWORD D14_Lights;               
    DWORD D15_LightEnables;         

    // Variable length, and Variably formatted data, based on the above:

    DWORD Data[1];
};

//------------------------------------------------------------------------------
// CreateStateBlock functionality groupings

D3DCONST BYTE g_PixelRenderStates[] =
{
    D3DRS_ZENABLE,
    D3DRS_FILLMODE,
    D3DRS_SHADEMODE,
    D3DRS_ZWRITEENABLE,
    D3DRS_ALPHATESTENABLE,
    D3DRS_SRCBLEND,
    D3DRS_DESTBLEND,
    D3DRS_ZFUNC,
    D3DRS_ALPHAREF,
    D3DRS_ALPHAFUNC,
    D3DRS_DITHERENABLE,
    D3DRS_FOGSTART,
    D3DRS_FOGEND,
    D3DRS_FOGDENSITY,
    D3DRS_EDGEANTIALIAS,
    D3DRS_ALPHABLENDENABLE,
    D3DRS_ZBIAS,
    D3DRS_STENCILENABLE,
    D3DRS_STENCILFAIL,
    D3DRS_STENCILZFAIL,
    D3DRS_STENCILPASS,
    D3DRS_STENCILFUNC,
    D3DRS_STENCILREF,
    D3DRS_STENCILMASK,
    D3DRS_STENCILWRITEMASK,
    D3DRS_TEXTUREFACTOR,
    D3DRS_WRAP0,
    D3DRS_WRAP1,
    D3DRS_WRAP2,
    D3DRS_WRAP3,
    D3DRS_COLORWRITEENABLE,
    D3DRS_BLENDOP,
};

CONST DWORD g_PixelRenderStatesCount = sizeof(g_PixelRenderStates) / 
                                       sizeof(g_PixelRenderStates[0]);

D3DCONST BYTE g_PixelTextureStates[] =
{
    D3DTSS_COLOROP,
    D3DTSS_COLORARG1,
    D3DTSS_COLORARG2,
    D3DTSS_ALPHAOP,
    D3DTSS_ALPHAARG1,
    D3DTSS_ALPHAARG2,
    D3DTSS_BUMPENVMAT00,
    D3DTSS_BUMPENVMAT01,
    D3DTSS_BUMPENVMAT10,
    D3DTSS_BUMPENVMAT11,
    D3DTSS_TEXCOORDINDEX,
    D3DTSS_ADDRESSU,
    D3DTSS_ADDRESSV,
    D3DTSS_BORDERCOLOR,
    D3DTSS_MAGFILTER,
    D3DTSS_MINFILTER,
    D3DTSS_MIPFILTER,
    D3DTSS_MIPMAPLODBIAS,
    D3DTSS_MAXMIPLEVEL,
    D3DTSS_MAXANISOTROPY,
    D3DTSS_BUMPENVLSCALE,
    D3DTSS_BUMPENVLOFFSET,
    D3DTSS_TEXTURETRANSFORMFLAGS,
    D3DTSS_ADDRESSW,
    D3DTSS_COLORARG0,
    D3DTSS_ALPHAARG0,
    D3DTSS_RESULTARG,
};

CONST DWORD g_PixelTextureStatesCount = sizeof(g_PixelTextureStates) / 
                                        sizeof(g_PixelTextureStates[0]);

D3DCONST BYTE g_VertexRenderStates[] =
{
    D3DRS_SHADEMODE,
    D3DRS_SPECULARENABLE,
    D3DRS_CULLMODE,
    D3DRS_FOGENABLE,
    D3DRS_FOGCOLOR,
    D3DRS_FOGTABLEMODE,
    D3DRS_FOGSTART,
    D3DRS_FOGEND,
    D3DRS_FOGDENSITY,
    D3DRS_RANGEFOGENABLE,
    D3DRS_AMBIENT,
    D3DRS_COLORVERTEX,
    D3DRS_LIGHTING,
    D3DRS_NORMALIZENORMALS,
    D3DRS_LOCALVIEWER,
    D3DRS_EMISSIVEMATERIALSOURCE,
    D3DRS_AMBIENTMATERIALSOURCE,
    D3DRS_DIFFUSEMATERIALSOURCE,
    D3DRS_SPECULARMATERIALSOURCE,
    D3DRS_VERTEXBLEND,
    D3DRS_POINTSIZE,
    D3DRS_POINTSIZE_MIN,
    D3DRS_POINTSPRITEENABLE,
    D3DRS_POINTSCALEENABLE,
    D3DRS_POINTSCALE_A,
    D3DRS_POINTSCALE_B,
    D3DRS_POINTSCALE_C,
    D3DRS_MULTISAMPLEANTIALIAS,
    D3DRS_MULTISAMPLEMASK,
    D3DRS_PATCHEDGESTYLE,
    D3DRS_PATCHSEGMENTS,
    D3DRS_POINTSIZE_MAX,
};

CONST DWORD g_VertexRenderStatesCount = sizeof(g_VertexRenderStates) / 
                                        sizeof(g_VertexRenderStates[0]);

D3DCONST BYTE g_VertexTextureStates[] =
{
    D3DTSS_TEXCOORDINDEX,
    D3DTSS_TEXTURETRANSFORMFLAGS
};

CONST DWORD g_VertexTextureStatesCount = sizeof(g_VertexTextureStates) / 
                                         sizeof(g_VertexTextureStates[0]);

//------------------------------------------------------------------------------
// ClearStateBlockFlags
//
// Clears all the 'dirty' flags we use to tell when a particular state
// gets modified.

VOID ClearStateBlockFlags()
{
    DWORD i;

    CDevice *pDevice = g_pDevice;

    ZeroMemory(&D3D__StateBlockDirty[0], sizeof(D3D__StateBlockDirty));

    // D14_Lights are special because there can be an arbitrary number of them,
    // so we keep their 'modified' status directly in their structure:

    Light* pLight = pDevice->m_pLights;
    for (i = 0; i < pDevice->m_LightCount; i++, pLight++)
    {
        pLight->Flags &= ~(LIGHT_STATEBLOCK_SET | LIGHT_STATEBLOCK_ENABLED);
    }
}

//------------------------------------------------------------------------------
// RecordStateBlock
//
// Here we run through our 'dirty' list and record any changed states into
// a variable-length, variably formatted allocation.

HRESULT RecordStateBlock(
    DWORD* pToken) 
{ 
    DWORD i;
    DWORD j;
    BYTE* pDirty;
    DWORD numStates; // Total number of states we check (used for an assert)
    StateBlock* pBlock = NULL;
    DWORD* pData = &pBlock->Data[0]; // We never actually dereference NULL

    CDevice* pDevice = g_pDevice;
    Light* pLights = pDevice->m_pLights;

    *pToken = NULL;

    // We always do two passes.  The first pass determines the size of
    // the state block allocation we'll need, and the second pass fills
    // in the state block:

    while (TRUE)
    {
        pDirty = &D3D__StateBlockDirty[0];

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_TEXTURES]);
        for (i = 0; i < D3DTSS_MAXSTAGES; i++)
        {
            if (*pDirty++)
            {
                if (pBlock != NULL)
                {
                    D3DBaseTexture* pTexture = pDevice->m_Textures[i];

                    pBlock->D0_Textures++;
                    pData[0] = i;
                    pData[1] = (DWORD) pTexture;

                    if (pTexture)
                    {
                        pTexture->AddRef();
                    }
                }
                pData += 2;
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_PIXELSHADER]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                PixelShader* pPixelShader = pDevice->m_pPixelShader;

                pBlock->D1_PixelShader = TRUE;
                pData[0] = (DWORD) pPixelShader;
                if (pPixelShader)
                {
                    pPixelShader->RefCount++;
                }
            }
            pData++;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_VERTEXSHADER]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                DWORD handle = pDevice->m_VertexShaderHandle;

                pBlock->D2_VertexShader = TRUE;
                pData[0] = handle;
                if (IsVertexProgram(handle))
                {
                    pDevice->m_pVertexShader->RefCount++;
                }
            }
            pData++;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_INDICES]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                D3DIndexBuffer* pIndexBuffer = pDevice->m_pIndexBuffer;

                pBlock->D3_Index = TRUE;
                pData[0] = pDevice->m_IndexBase;
                pData[1] = (DWORD) pIndexBuffer;
                if (pIndexBuffer)
                {
                    pIndexBuffer->AddRef();
                }
            }
            pData += 2;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_STREAMS]);
        for (i = 0; i < D3DVS_STREAMS_MAX_V1_0; i++)
        {
            if (*pDirty++)
            {
                if (pBlock != NULL)
                {
                    D3DVertexBuffer* pVertexBuffer = g_Stream[i].pVertexBuffer;
    
                    pBlock->D4_Streams++;
                    pData[0] = i;
                    pData[1] = g_Stream[i].Stride;
                    pData[2] = (DWORD) pVertexBuffer;
                    if (pVertexBuffer)
                    {
                        pVertexBuffer->AddRef();
                    }
                }
                pData += 3;
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_PIXELSHADERCONSTANTS]);
        for (i = 0; i < D3DPS_CONSTREG_MAX_DX8; i++)
        {
            if (*pDirty++)
            {
                // We can only call SetPixelShaderConstant if a pixel shader is
                // active, so a corollary is that there's no point in saving the
                // pixel shader constants if no pixel shader is active:

                if (pDevice->m_pPixelShader != NULL)
                {
                    if (pBlock != NULL)
                    {
                        pBlock->D6_PixelShaderConstants++;
                        pData[0] = i;
                        memcpy(pData + 1, &pDevice->m_PixelShaderConstants[i][0],
                               4*sizeof(FLOAT));
                    }
                    pData += 5;
                }
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_VERTEXSHADERCONSTANTS]);
        for (i = 0; i < D3DVS_CONSTREG_COUNT_XBOX; i++)
        {
            if (*pDirty++)
            {
                // We never save the vertex shader constants that map
                // from -96 to -1, although we leave space for them
                // in the 'dirty' array for future expansion:

                if (i >= 96)
                {
                    if (pBlock != NULL)
                    {
                        pBlock->D7_VertexShaderConstants++;
                        pData[0] = i;
                        memcpy(pData + 1, &pDevice->m_VertexShaderConstants[i][0],
                               4*sizeof(FLOAT));
                    }
                    pData += 5;
                }
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_RENDERSTATES]);
        for (i = 0; i < D3DRS_MAX; i++)
        {
            if (*pDirty++)
            {
                // If the fixed function pipeline is active, the shadowed
                // render states for the pixel shader registers are garbage,
                // so don't save those:

                if ((pDevice->m_pPixelShader != NULL) ||
                    (i < D3DRS_PSALPHAINPUTS0) ||
                    ((i > D3DRS_PSINPUTTEXTURE) && (i != D3DRS_PSTEXTUREMODES)))
                {
                    if (pBlock != NULL)
                    {
                        pBlock->D8_RenderStates++;
                        pData[0] = i;
                        pData[1] = D3D__RenderState[i];
                    }
                    pData += 2;
                }
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_TEXTURESTATES]);
        for (j = 0; j < D3DTSS_MAX; j++) // j = 0 .. 29
        {
            for (i = 0; i < D3DTSS_MAXSTAGES; i++) // i = 0 .. 3
            {
                // Note that in the 'dirty' buffer, we always have all the
                // stages for a particular state be consecutive

                if (*pDirty++)
                {
                    if (pBlock != NULL)
                    {
                        pBlock->D9_TextureStates++;
                        pData[0] = i;   // Stage
                        pData[1] = j;   // State
                        pData[2] = D3D__TextureState[i][j];
                    }
                    pData += 3;
                }
            }
        }
        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_TRANSFORMS]);
        for (i = 0; i < D3DTS_MAX; i++)
        {
            if (*pDirty++)
            {
                if (pBlock != NULL)
                {
                    pBlock->D10_Transforms++;
                    pData[0] = i;
                    *((D3DMATRIX*) &pData[1]) = pDevice->m_Transform[i];
                }
                pData += 1 + sizeof(D3DMATRIX) / 4;
            }
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_VIEWPORT]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                pBlock->D11_Viewport = TRUE;
                *((D3DVIEWPORT8*) &pData[0]) = pDevice->m_Viewport;
            }
            pData += sizeof(D3DVIEWPORT8) / 4;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_MATERIAL]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                pBlock->D12_Material = TRUE;
                *((D3DMATERIAL8*) &pData[0]) = pDevice->m_Material;
            }
            pData += sizeof(D3DMATERIAL8) / 4;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_BACKMATERIAL]);
        if (*pDirty++)
        {
            if (pBlock != NULL)
            {
                pBlock->D13_BackMaterial = TRUE;
                *((D3DMATERIAL8*) &pData[0]) = pDevice->m_BackMaterial;
            }
            pData += sizeof(D3DMATERIAL8) / 4;
        }

        ASSERT(pDirty == &D3D__StateBlockDirty[D3DSBD_MAX]);
        for (i = 0; i < pDevice->m_LightCount; i++)
        {
            if (pLights[i].Flags & LIGHT_STATEBLOCK_SET)
            {
                if (pBlock != NULL)
                {
                    pBlock->D14_Lights++;
                    pData[0] = i;
                    *((D3DLIGHT8*) &pData[1]) = pLights->Light8;
                }
                pData += 1 + sizeof(D3DLIGHT8) / 4;
            }
        }
        for (i = 0; i < pDevice->m_LightCount; i++)
        {
            if (pLights[i].Flags & LIGHT_STATEBLOCK_ENABLED)
            {
                if (pBlock != NULL)
                {
                    pBlock->D15_LightEnables++;
                    pData[0] = i;
                    pDevice->GetLightEnable(i, (BOOL*) &pData[1]);
                }
                pData += 2;
            }
        }

        if (pBlock != NULL)
        {
            ASSERT(pData == pBlock->pDataEnd);

            // We're done!

            *pToken = (DWORD) pBlock;
            return S_OK;                    // =====>
        }

        // Okay, now we know the exact length of the allocation we'll need.
        // So allocate it now and do a second pass where we actually fill
        // in all the data:

        pBlock = (StateBlock*) MemAllocNoZero((DWORD) pData);
        if (pBlock == NULL)
        {
            return E_OUTOFMEMORY;
        }

        // Zero just the header part of the block:

        ZeroMemory(pBlock, offsetof(StateBlock, Data));

        // Now initialize some stuff:

    #if DBG
        pBlock->Signature = 'Sblk';
        pBlock->pDataEnd = (DWORD*) ((BYTE*) pBlock + (DWORD) pData);
    #endif

        pData = &pBlock->Data[0];
    }
}

//------------------------------------------------------------------------------
// D3DDevice_ApplyStateBlock
//
// Plays back a state block.

extern "C"
void WINAPI D3DDevice_ApplyStateBlock(
    DWORD Token) 
{ 
    DWORD i;

    COUNT_API(API_D3DDEVICE_APPLYSTATEBLOCK);

    CDevice* pDevice = g_pDevice;

    StateBlock* pBlock = (StateBlock*) Token;

    if (DBG_CHECK(TRUE))
    {
        if (pBlock == NULL)
        {
            DPF_ERR("NULL parameter");
        }
    #if DBG
        if (pBlock->Signature != 'Sblk')
        {
            DPF_ERR("Invalid state block object (already deleted?)");
        }
    #endif
    }

    DWORD* pData = &pBlock->Data[0];

    for (i = 0; i < pBlock->D0_Textures; i++)
    {
        DWORD stage = pData[0];
        D3DBaseTexture* pTexture = (D3DBaseTexture*) pData[1];

        pDevice->SetTexture(stage, pTexture);

        pData += 2;
    }
    if (pBlock->D1_PixelShader)
    {
        DWORD handle = pData[0];

        pDevice->SetPixelShader(handle);

        pData++;
    }
    if (pBlock->D2_VertexShader)
    {
        DWORD handle = pData[0];

        pDevice->SetVertexShader(handle);

        pData++;
    }
    if (pBlock->D3_Index)
    {
        DWORD indexBase = pData[0];
        D3DIndexBuffer* pIndexBuffer = (D3DIndexBuffer*) pData[1];

        pDevice->SetIndices(pIndexBuffer, indexBase);

        pData += 2;
    }
    for (i = 0; i < pBlock->D4_Streams; i++)
    {
        DWORD index = pData[0];
        DWORD stride = pData[1];
        D3DVertexBuffer* pVertexBuffer = (D3DVertexBuffer*) pData[2];

        pDevice->SetStreamSource(index, pVertexBuffer, stride);

        pData += 3;
    }
    for (i = 0; i < pBlock->D6_PixelShaderConstants; i++)
    {
        DWORD index = pData[0];
        VOID* pConstantData = &pData[1];

        pDevice->SetPixelShaderConstant(index, pConstantData, 1);

        pData += 5;
    }
    for (i = 0; i < pBlock->D7_VertexShaderConstants; i++)
    {
        DWORD index = pData[0];
        VOID* pConstantData = &pData[1];

        // In our state block code here we refer to them as constants 0 to 191,
        // but SetVertexShaderConstant knows of them as -96 to 95:

        pDevice->SetVertexShaderConstant(index - 96, pConstantData, 1);

        pData += 5;
    }
    for (i = 0; i < pBlock->D8_RenderStates; i++)
    {
        DWORD state = pData[0];
        DWORD value = pData[1];

        // On free builds, we do a small optimization where we don't bother
        // to call SetRenderState if the state is already set to the proper
        // value.  We don't do this on checked builds as a small little
        // sanity check to ensure that all of our default values would 
        // actually get through the SetRenderState parameter checking.

    #if !DBG
        if (D3D__RenderState[state] != value)
    #endif
        {
            pDevice->SetRenderStateNotInline((D3DRENDERSTATETYPE) state, value);
        }

        pData += 2;
    }
    for (i = 0; i < pBlock->D9_TextureStates; i++)
    {
        DWORD stage = pData[0];
        DWORD state = pData[1];
        DWORD value = pData[2];

    #if !DBG
        if (D3D__TextureState[stage][state] != value)
    #endif
        {
            pDevice->SetTextureStageStateNotInline(stage, 
                (D3DTEXTURESTAGESTATETYPE) state, value);
        }

        pData += 3;
    }
    for (i = 0; i < pBlock->D10_Transforms; i++)
    {
        DWORD state = pData[0];
        D3DMATRIX* pMatrix = (D3DMATRIX*) &pData[1];

        pDevice->SetTransform((D3DTRANSFORMSTATETYPE) state, pMatrix);

        pData += 1 + sizeof(D3DMATRIX) / 4;
    }
    if (pBlock->D11_Viewport)
    {
        D3DVIEWPORT8* pViewport = (D3DVIEWPORT8*) &pData[0];

        pDevice->SetViewport(pViewport);

        pData += sizeof(D3DVIEWPORT8) / 4;
    }
    if (pBlock->D12_Material)
    {
        D3DMATERIAL8* pMaterial = (D3DMATERIAL8*) &pData[0];

        pDevice->SetMaterial(pMaterial);

        pData += sizeof(D3DMATERIAL8) / 4;
    }
    if (pBlock->D13_BackMaterial)
    {
        D3DMATERIAL8* pMaterial = (D3DMATERIAL8*) &pData[0];

        pDevice->SetBackMaterial(pMaterial);

        pData += sizeof(D3DMATERIAL8) / 4;
    }
    for (i = 0; i < pBlock->D14_Lights; i++)
    {
        DWORD index = pData[0];
        D3DLIGHT8* pLight = (D3DLIGHT8*) &pData[1];

        pDevice->SetLight(index, pLight);

        pData += 1 + sizeof(D3DLIGHT8) / 4;
    }
    for (i = 0; i < pBlock->D15_LightEnables; i++)
    {
        DWORD index = pData[0];
        BOOL enable = pData[1];

        pDevice->LightEnable(index, enable);

        pData += 2;
    }

    ASSERT(pData == pBlock->pDataEnd);
}

//------------------------------------------------------------------------------
// D3DDevice_CaptureStateBlock
//
// Update the values in the block (weird lights being the exception)

extern "C"
void WINAPI D3DDevice_CaptureStateBlock(
    DWORD Token) 
{ 
    DWORD i;

    COUNT_API(API_D3DDEVICE_CAPTURESTATEBLOCK);

    CDevice* pDevice = g_pDevice;

    StateBlock* pBlock = (StateBlock*) Token;

    if (DBG_CHECK(TRUE))
    {
        if (pBlock == NULL)
        {
            DPF_ERR("NULL parameter");
        }
    #if DBG
        if (pBlock->Signature != 'Sblk')
        {
            DPF_ERR("Invalid state block object (already deleted?)");
        }
    #endif
    }

    DWORD* pData = &pBlock->Data[0];

    for (i = 0; i < pBlock->D0_Textures; i++)
    {
        DWORD stage = pData[0];
        D3DBaseTexture* pTexture = (D3DBaseTexture*) pData[1];

        if (pTexture)
        {
            pTexture->Release();
        }
        pTexture = pDevice->m_Textures[stage];
        if (pTexture)
        {
            pTexture->AddRef();
        }
        pData[1] = (DWORD) pTexture;

        pData += 2;
    }
    if (pBlock->D1_PixelShader)
    {
        DWORD handle = pData[0];

        if (handle)
        {
            pDevice->DeletePixelShader(handle);
        }
        handle = (DWORD) pDevice->m_pPixelShader;
        if (handle)
        {
            pDevice->m_pPixelShader->RefCount++;
        }
        pData[0] = handle;

        pData++;
    }
    if (pBlock->D2_VertexShader)
    {
        DWORD handle = pData[0];

        if (IsVertexProgram(handle))
        {
            pDevice->DeleteVertexShader(handle);
        }
        handle = pDevice->m_VertexShaderHandle;
        if (IsVertexProgram(handle))
        {
            pDevice->m_pVertexShader->RefCount++;
        }
        pData[0] = handle;

        pData++;
    }
    if (pBlock->D3_Index)
    {
        D3DIndexBuffer* pIndexBuffer = (D3DIndexBuffer*) pData[1];

        if (pIndexBuffer)
        {
            pIndexBuffer->Release();
        }
        pIndexBuffer = pDevice->m_pIndexBuffer;
        if (pIndexBuffer)
        {
            pIndexBuffer->AddRef();
        }
        pData[0] = pDevice->m_IndexBase;
        pData[1] = (DWORD) pIndexBuffer;

        pData += 2;
    }
    for (i = 0; i < pBlock->D4_Streams; i++)
    {
        DWORD index = pData[0];
        D3DVertexBuffer* pVertexBuffer = (D3DVertexBuffer*) pData[2];

        if (pVertexBuffer)
        {
            pVertexBuffer->Release();
        }
        pVertexBuffer = g_Stream[index].pVertexBuffer;
        if (pVertexBuffer)
        {
            pVertexBuffer->AddRef();
        }
        pData[1] = g_Stream[index].Stride;
        pData[2] = (DWORD) pVertexBuffer;

        pData += 3;
    }
    for (i = 0; i < pBlock->D6_PixelShaderConstants; i++)
    {
        DWORD index = pData[0];
        VOID* pConstantData = &pData[1];

        memcpy(pConstantData, &pDevice->m_PixelShaderConstants[index][0],
               4*sizeof(FLOAT));

        pData += 5;
    }
    for (i = 0; i < pBlock->D7_VertexShaderConstants; i++)
    {
        DWORD index = pData[0];
        VOID* pConstantData = &pData[1];

        memcpy(pConstantData, &pDevice->m_VertexShaderConstants[index][0],
               4*sizeof(FLOAT));

        pData += 5;
    }
    for (i = 0; i < pBlock->D8_RenderStates; i++)
    {
        DWORD state = pData[0];

        pData[1] = D3D__RenderState[state];

        pData += 2;
    }
    for (i = 0; i < pBlock->D9_TextureStates; i++)
    {
        DWORD stage = pData[0];
        DWORD state = pData[1];

        pData[2] = D3D__TextureState[stage][state];

        pData += 3;
    }
    for (i = 0; i < pBlock->D10_Transforms; i++)
    {
        DWORD state = pData[0];

        *((D3DMATRIX*) &pData[1]) = pDevice->m_Transform[state];

        pData += 1 + sizeof(D3DMATRIX) / 4;
    }
    if (pBlock->D11_Viewport)
    {
        *((D3DVIEWPORT8*) &pData[0]) = pDevice->m_Viewport;

        pData += sizeof(D3DVIEWPORT8) / 4;
    }
    if (pBlock->D12_Material)
    {
        *((D3DMATERIAL8*) &pData[0]) = pDevice->m_Material;

        pData += sizeof(D3DMATERIAL8) / 4;
    }
    if (pBlock->D13_BackMaterial)
    {
        *((D3DMATERIAL8*) &pData[0]) = pDevice->m_BackMaterial;

        pData += sizeof(D3DMATERIAL8) / 4;
    }
    for (i = 0; i < pBlock->D14_Lights; i++)
    {
        DWORD index = pData[0];

        *((D3DLIGHT8*) &pData[1]) = pDevice->m_pLights[index].Light8;

        pData += 1 + sizeof(D3DLIGHT8) / 4;
    }
    for (i = 0; i < pBlock->D15_LightEnables; i++)
    {
        DWORD index = pData[0];

        pDevice->GetLightEnable(index, (BOOL*) &pData[1]);

        pData += 2;
    }

    ASSERT(pData == pBlock->pDataEnd);
}

//------------------------------------------------------------------------------
// D3DDevice_DeleteStateBlock

extern "C"
void WINAPI D3DDevice_DeleteStateBlock(
    DWORD Token) 
{ 
    DWORD i;
    DWORD stage;
    DWORD handle;
    D3DBaseTexture* pTexture;
    D3DIndexBuffer* pIndexBuffer;
    D3DVertexBuffer* pVertexBuffer;

    COUNT_API(API_D3DDEVICE_DELETESTATEBLOCK);

    CDevice* pDevice = g_pDevice;
    StateBlock* pBlock = (StateBlock*) Token;

    if (DBG_CHECK(TRUE))
    {
        if (pBlock == NULL)
        {
            DPF_ERR("NULL parameter");
        }

        #if DBG
        
            if (pBlock->Signature != 'Sblk')
            {
                DPF_ERR("Invalid state block object (already deleted?)");
            }
    
            pBlock->Signature = 'xxxx';
    
        #endif
    }

    // Release any referenced objects:

    DWORD* pData = &pBlock->Data[0];

    for (i = 0; i < pBlock->D0_Textures; i++)
    {
        stage = pData[0];
        pTexture = (D3DBaseTexture*) pData[1];
        pData += 2;

        if (pTexture)
        {
            pTexture->Release();
        }
    }
    if (pBlock->D1_PixelShader)
    {
        handle = pData[0];
        pData++;

        // Note this is a reference-counted delete:

        if (handle)
        {
            pDevice->DeletePixelShader(handle);
        }
    }
    if (pBlock->D2_VertexShader)
    {
        handle = pData[0];
        pData++;

        // Don't free the vertex shader if it's actually an FVF:

        if (IsVertexProgram(handle))
        {
            // Note this is a reference-counted delete:
    
            pDevice->DeleteVertexShader(handle);
        }
    }
    if (pBlock->D3_Index)
    {
        pIndexBuffer = (D3DIndexBuffer*) pData[1];
        pData += 2;

        if (pIndexBuffer)
        {
            pIndexBuffer->Release();
        }
    }
    for (i = 0; i < pBlock->D4_Streams; i++)
    {
        pVertexBuffer = (D3DVertexBuffer*) pData[2];
        pData += 3;

        if (pVertexBuffer)
        {
            pVertexBuffer->Release();
        }
    }

    // Free our state block memory:

    MemFree(pBlock);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateStateBlock
//
// Looks at the current state and captures a bunch.
//
// Note that the PC version of DX8 doesn't record the contents of the palette, 
// so neither do we.

extern "C"
HRESULT WINAPI D3DDevice_CreateStateBlock(
    D3DSTATEBLOCKTYPE Type,
    DWORD* pToken) 
{ 
    DWORD i;
    DWORD j;

    COUNT_API(API_D3DDEVICE_CREATESTATEBLOCK);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pDevice->m_StateFlags & STATE_PUREDEVICE)
        {
            DPF_ERR("Can't use state blocks with a pure device");
            return D3DERR_INVALIDCALL;
        }
        if (pDevice->m_StateFlags & STATE_RECORDBLOCK)
        {
            D3D_ERR("Cannot create state block when in the state record mode. "
                    "CreateStateBlock failed.");
            return D3DERR_INVALIDCALL;
        }
        if (pToken == NULL)
        {
            DPF_ERR("NULL parameter");
            return D3DERR_INVALIDCALL;
        }
        if ((Type < D3DSBT_ALL) || (Type > D3DSBT_VERTEXSTATE))
        {
            DPF_ERR("Invalid Type parameter");
            return D3DERR_INVALIDCALL;
        }
    }

    if (Type == D3DSBT_ALL)
    {
        // Dirty all the state-block dirty flags, so that everything
        // is recorded:

        FillMemory(&D3D__StateBlockDirty[0], 
                   sizeof(D3D__StateBlockDirty), 
                   0xffffffff);

        Light* pLights = pDevice->m_pLights;

        for (i = 0; i < pDevice->m_LightCount; i++)
        {
             if (pLights[i].Flags & LIGHT_SET)
             {
                 pLights[i].Flags |= (LIGHT_STATEBLOCK_SET | 
                                      LIGHT_STATEBLOCK_ENABLED);
             }
        }
    }
    else 
    {
        // First reset all flags, so that no extraneous stuff gets into
        // our state block:

        ClearStateBlockFlags();

        // Now dirty stuff appropriately:

        if (Type == D3DSBT_PIXELSTATE)
        {
            // We have to record the current pixel shader program, all
            // pixel shader constants, and pixel shader related render
            // states and texture stage states.

            D3D__StateBlockDirty[D3DSBD_PIXELSHADER] = TRUE;

            FillMemory(&D3D__StateBlockDirty[D3DSBD_PIXELSHADERCONSTANTS],
                       D3DPS_CONSTREG_MAX_DX8,
                       0xffffffff);

            for (i = 0; i < g_PixelRenderStatesCount; i++)
            {
                DIRTY_RENDERSTATE(g_PixelRenderStates[i]);
            }

            for (i = 0; i < D3DTSS_MAXSTAGES; i++)
            {
                for (j = 0; j < g_PixelTextureStatesCount; j++)
                {
                    DIRTY_TEXTURESTATE(i, g_PixelTextureStates[j]);
                }
            }
        }
        else
        {
            ASSERT(Type == D3DSBT_VERTEXSTATE);

            // We have to record the current vertex shader program, all
            // vertex shader constants, vertex shader related render
            // states and texture stage states, and lights:

            D3D__StateBlockDirty[D3DSBD_VERTEXSHADER] = TRUE;

            FillMemory(&D3D__StateBlockDirty[D3DSBD_VERTEXSHADERCONSTANTS],
                       D3DVS_CONSTREG_COUNT_XBOX,
                       0xffffffff);

            for (i = 0; i < g_VertexRenderStatesCount; i++)
            {
                DIRTY_RENDERSTATE(g_VertexRenderStates[i]);
            }

            for (i = 0; i < D3DTSS_MAXSTAGES; i++)
            {
                for (j = 0; j < g_VertexTextureStatesCount; j++)
                {
                    DIRTY_TEXTURESTATE(i, g_VertexTextureStates[j]);
                }
            }

            Light* pLights = pDevice->m_pLights;

            for (i = 0; i < pDevice->m_LightCount; i++)
            {
                 if (pLights[i].Flags & LIGHT_SET)
                 {
                     pLights[i].Flags |= (LIGHT_STATEBLOCK_SET | 
                                          LIGHT_STATEBLOCK_ENABLED);
                 }
            }
        }
    }

    // Finally, have RecordStateBlock do all the hard work of actually 
    // recording the state:

    return RecordStateBlock(pToken);
}

//------------------------------------------------------------------------------
// D3DDevice_BeginStateBlock
//
// Gets ready to start recording a state block.

extern "C"
void WINAPI D3DDevice_BeginStateBlock() 
{ 
    COUNT_API(API_D3DDEVICE_BEGINSTATEBLOCK);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pDevice->m_StateFlags & STATE_PUREDEVICE)
        {
            DPF_ERR("Can't use state blocks with a pure device");
        }
        if (pDevice->m_StateFlags & STATE_RECORDBLOCK)
        {
            D3D_ERR("Already in the state record mode. BeginStateBlock failed.");
        }
    }

    pDevice->m_StateFlags |= STATE_RECORDBLOCK;

    ClearStateBlockFlags();
}

//------------------------------------------------------------------------------
// D3DDevice_EndStateBlock
//
// Here we run through our 'dirty' list and record any changed states into
// a variable-length, variably formatted allocation.

extern "C"
HRESULT WINAPI D3DDevice_EndStateBlock(
    DWORD* pToken) 
{
    COUNT_API(API_D3DDEVICE_ENDSTATEBLOCK);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (!(pDevice->m_StateFlags & STATE_RECORDBLOCK))
        {
            D3D_ERR("Not in state record mode. EndStateBlock failed.");
        }
    }

    pDevice->m_StateFlags &= ~STATE_RECORDBLOCK;

    return RecordStateBlock(pToken);
}

//------------------------------------------------------------------------------
// D3DDevice_Suspend

extern "C"
VOID WINAPI D3DDevice_Suspend()
{
    COUNT_API(API_D3DDEVICE_SUSPEND);

    CDevice* pDevice = g_pDevice;

    // Flush any pending lazy state before they take control:

    pDevice->SetStateVB(0);
}

//------------------------------------------------------------------------------
// D3DDevice_Resume

extern "C"
VOID WINAPI D3DDevice_Resume(
    BOOL Reset)
{
    COUNT_API(API_D3DDEVICE_RESUME);

    DWORD token;
    HRESULT result;

    if (DBG_CHECK(TRUE))
    {
        if (Reset > TRUE)
        {
            DPF_ERR("Only TRUE or FALSE are valid arguments");
        }
    }

    if (Reset)
    {
        // We could of course write a bunch of code to handle this more
        // efficiently.  But perf-wise, this is such a painful operation
        // anyways, because we have to hit *all* the hardware registers,
        // I don't think it's worth much development time.  The ugliest part, 
        // though, is that anyone linking to this routine pulls in the whole 
        // D3D__StateBlockDirty global (about 528 bytes).

        result = D3DDevice_CreateStateBlock(D3DSBT_ALL, &token);
        if (DBG_CHECK(result != S_OK))
        {
            DPF_ERR("Resume failed, not enough memory");
        }

        D3DDevice_ApplyStateBlock(token);
        D3DDevice_DeleteStateBlock(token);
    }
}

} // end namespace
