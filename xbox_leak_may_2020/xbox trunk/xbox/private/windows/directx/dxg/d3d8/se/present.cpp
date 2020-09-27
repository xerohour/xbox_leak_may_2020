/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       present.cpp
 *  Content:    Support for handling the Present and Swap APIs
 *
 ***************************************************************************/

#include "precomp.hpp"
#include "dm.h"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// g_AntiAliasedRenderStates, g_AntiAliasedTextureStates
//
// Default states needed for our Kelvin copy.

D3DCONST DWORD g_AntiAliasedRenderStates[] =
{
    D3DRS_FILLMODE,                     D3DFILL_SOLID,
    D3DRS_BACKFILLMODE,                 D3DFILL_SOLID,
    D3DRS_CULLMODE,                     D3DCULL_NONE,
    D3DRS_DITHERENABLE,                 TRUE,
    D3DRS_ALPHATESTENABLE,              FALSE,
    D3DRS_ALPHABLENDENABLE,             FALSE,
    D3DRS_FOGENABLE,                    FALSE,
    D3DRS_EDGEANTIALIAS,                FALSE,
    D3DRS_STENCILENABLE,                FALSE,
    D3DRS_LIGHTING,                     FALSE,
    D3DRS_MULTISAMPLEMASK,              0xffffffff,
    D3DRS_LOGICOP,                      D3DLOGICOP_NONE,
    D3DRS_COLORWRITEENABLE,             D3DCOLORWRITEENABLE_ALL,
    D3DRS_YUVENABLE,                    FALSE,
    D3DRS_SPECULARENABLE,               FALSE,
    D3DRS_ZBIAS,                        0,
    D3DRS_MULTISAMPLERENDERTARGETMODE,  D3DMULTISAMPLEMODE_1X,

    // Note that both the 'immediate' and one-back-buffer cases require
    // the swath to be off, otherwise tearing is visible, since swathing
    // causes the GPU to do the fill in vertical columns.

    D3DRS_SWATHWIDTH,                   D3DSWATH_OFF,
};

CONST g_AntiAliasedRenderStatesCount = sizeof(g_AntiAliasedRenderStates) / 8;

D3DCONST DWORD g_AntiAliasedTextureStates[] =
{
    D3DTSS_COLOROP,                     D3DTOP_SELECTARG1,
    D3DTSS_COLORARG1,                   D3DTA_TEXTURE,
    D3DTSS_ALPHAOP,                     D3DTOP_DISABLE,
    D3DTSS_TEXCOORDINDEX,               0,
    D3DTSS_ADDRESSU,                    D3DTADDRESS_CLAMP,
    D3DTSS_ADDRESSV,                    D3DTADDRESS_CLAMP,
    D3DTSS_COLORKEYOP,                  D3DTCOLORKEYOP_DISABLE,
    D3DTSS_COLORSIGN,                   0,
    D3DTSS_ALPHAKILL,                   D3DTALPHAKILL_DISABLE,
    D3DTSS_MINFILTER,                   D3DTEXF_LINEAR, // reset later
    D3DTSS_MAGFILTER,                   D3DTEXF_LINEAR, // reset later
};

CONST g_AntiAliasedTextureStatesCount = sizeof(g_AntiAliasedTextureStates) / 8;

//------------------------------------------------------------------------------
// SwapSavedState

struct SwapSavedState
{
    PixelShader *pPixelShader;
    DWORD VertexShaderHandle;
    DWORD Stage1ColorOp;
    DWORD PSTextureModes;
    DWORD RenderState[g_AntiAliasedRenderStatesCount];
    DWORD TextureState[g_AntiAliasedTextureStatesCount];
    DWORD PixelShaderState[D3DRS_PS_MAX - D3DRS_PS_MIN];
    DWORD VertexShaders[4 * D3DVS_XBOX_RESERVEDXYZRHWSLOTS];
};

//------------------------------------------------------------------------------
// SwapSetRenderState

static VOID SwapSetRenderState(
    D3DRENDERSTATETYPE State,
    DWORD Value,
    BOOL Restore)   // TRUE if in restoring pass
{
    ASSERT(State >= D3DRS_PS_MAX);

    // D3D doesn't save the state if we're a 'pure' device and the state
    // is 'simple':

    BOOL saved = (State >= D3DRS_SIMPLE_MAX) ||
                !(g_pDevice->m_StateFlags & STATE_PUREDEVICE);

    if ((Restore) && (!saved))
    {
        // Can't restore simple states when running on a pure device
        // because we don't know what values to restore them to:

        return;
    }

    if ((saved) && (Value == D3D__RenderState[State]))
    {
        // Don't bother setting the render state if it's already correct:

        return;
    }

    D3DDevice_SetRenderState(State, Value);
}

//------------------------------------------------------------------------------
// SwapSetTextureState

static VOID SwapSetTextureState(
    DWORD Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD Value)
{
    if (Value == D3D__TextureState[Stage][Type])
    {
        // Don't bother setting the render state if it's already correct:

        return;
    }

    D3DDevice_SetTextureStageState(Stage, Type, Value);
}

//------------------------------------------------------------------------------
// SwapSetState
//
// Sets a bunch of Kelvin state to what's needed by 'SwapCopyBlt'.
//
// NOTE: Any state that gets touched here should have corresponding save and
//       restore functionality put into SwapSave/RestoreState!

static VOID SwapSetState(
    SwapSavedState *pSave)
{
    DWORD i;

    CDevice* pDevice = g_pDevice;

    for (i = 0; i < g_AntiAliasedRenderStatesCount; i++)
    {
        SwapSetRenderState(
            (D3DRENDERSTATETYPE) g_AntiAliasedRenderStates[2*i],
            g_AntiAliasedRenderStates[2*i + 1],
            FALSE);
    }

    for (i = 0; i < g_AntiAliasedTextureStatesCount; i++)
    {
        SwapSetTextureState(
            0,
            (D3DTEXTURESTAGESTATETYPE) g_AntiAliasedTextureStates[2*i],
            g_AntiAliasedTextureStates[2*i + 1]);
    }

    D3DDevice_SetPixelShader(NULL);
    D3DDevice_SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

    DWORD filter = D3D__RenderState[D3DRS_SWAPFILTER];

    SwapSetTextureState(0, D3DTSS_MINFILTER, filter);
    SwapSetTextureState(0, D3DTSS_MAGFILTER, filter);
    SwapSetTextureState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

    // Whack the hardware with the new state so that we're all set to go
    // when we enter SwapCopyBlt:

    SetState();
}

//------------------------------------------------------------------------------
// SwapSaveState

static VOID SwapSaveState(
    SwapSavedState *pSave)
{
    DWORD i;

    CDevice* pDevice = g_pDevice;

    pSave->pPixelShader = pDevice->m_pPixelShader;

    pSave->VertexShaderHandle = pDevice->m_VertexShaderHandle;

    pSave->Stage1ColorOp = D3D__TextureState[1][D3DTSS_COLOROP];

    for (i = 0; i < g_AntiAliasedRenderStatesCount; i++)
    {
        pSave->RenderState[i] = D3D__RenderState[g_AntiAliasedRenderStates[2*i]];
    }

    for (i = 0; i < g_AntiAliasedTextureStatesCount; i++)
    {
        pSave->TextureState[i] = D3D__TextureState[0]
            [g_AntiAliasedTextureStates[2*i]];
    }

    if (pSave->pPixelShader)
    {
        for (i = D3DRS_PS_MIN; i < D3DRS_PS_MAX; i++)
        {
            pSave->PixelShaderState[i - D3DRS_PS_MIN] = D3D__RenderState[i];
        }

        pSave->PSTextureModes = D3D__RenderState[D3DRS_PSTEXTUREMODES];
    }

    memcpy(pSave->VertexShaders,
           &pDevice->m_VertexShaderProgramSlots[0][0],
           sizeof(pSave->VertexShaders));
}

//------------------------------------------------------------------------------
// SwapRestoreState

static VOID SwapRestoreState(
    SwapSavedState* pSave)
{
    DWORD i;

    CDevice* pDevice = g_pDevice;

    D3DDevice_SetPixelShader((DWORD) pSave->pPixelShader);

    D3DDevice_SetVertexShader(pSave->VertexShaderHandle);

    BOOL pure = pDevice->m_StateFlags & STATE_PUREDEVICE;

    SwapSetTextureState(1, D3DTSS_COLOROP, pSave->Stage1ColorOp);

    for (i = 0; i < g_AntiAliasedRenderStatesCount; i++)
    {
        SwapSetRenderState(
                (D3DRENDERSTATETYPE) g_AntiAliasedRenderStates[2*i],
                pSave->RenderState[i],
                TRUE);
    }

    for (i = 0; i < g_AntiAliasedTextureStatesCount; i++)
    {
        SwapSetTextureState(
            0, 
            (D3DTEXTURESTAGESTATETYPE) g_AntiAliasedTextureStates[2*i],
            pSave->TextureState[i]);
    }

    if (!pure)
    {
        if (pSave->pPixelShader)
        {
            for (i = D3DRS_PS_MIN; i < D3DRS_PS_MAX; i++)
            {
                D3DDevice_SetRenderState((D3DRENDERSTATETYPE) i,
                    pSave->PixelShaderState[i - D3DRS_PS_MIN]);
            }

            D3DDevice_SetRenderState(D3DRS_PSTEXTUREMODES, pSave->PSTextureModes);
        }

        RestoreVertexShaders(pSave->VertexShaders,
                             sizeof(pSave->VertexShaders) / sizeof(DWORD));
    }
}

//------------------------------------------------------------------------------
// SwapSetSurfaces

static VOID SwapSetSurfaces(
    CDevice* pDevice)
{
    D3DDevice_SetRenderTarget(pDevice->m_pFrameBuffer[1], NULL);
    D3DDevice_SetTexture(0, (D3DTexture*) pDevice->m_pFrameBuffer[0]);
}

//------------------------------------------------------------------------------
// SwapSaveSurfaces

static VOID SwapSaveSurfaces(
    CDevice* pDevice)
{
    pDevice->m_pSaveRenderTarget = pDevice->m_pRenderTarget;
    pDevice->m_pSaveRenderTarget->AddRef();

    pDevice->m_pSaveZBuffer = pDevice->m_pZBuffer;
    if (pDevice->m_pSaveZBuffer)
        pDevice->m_pSaveZBuffer->AddRef();

    pDevice->m_pSaveTexture = pDevice->m_Textures[0];
    if (pDevice->m_pSaveTexture)
        pDevice->m_pSaveTexture->AddRef();

    // SetRenderTarget nukes the current viewport, so we have to save that
    // as well:

    pDevice->m_SaveViewport = pDevice->m_Viewport;
}

//------------------------------------------------------------------------------
// SwapRestoreSurfaces

static VOID SwapRestoreSurfaces(
    CDevice* pDevice)
{
    D3DDevice_SetRenderTarget(pDevice->m_pSaveRenderTarget, pDevice->m_pSaveZBuffer);
    pDevice->m_pSaveRenderTarget->Release();
    if (pDevice->m_pSaveZBuffer)
        pDevice->m_pSaveZBuffer->Release();

    D3DDevice_SetTexture(0, pDevice->m_pSaveTexture);
    if (pDevice->m_pSaveTexture)
        pDevice->m_pSaveTexture->Release();

    D3DDevice_SetViewport(&pDevice->m_SaveViewport);
}

//------------------------------------------------------------------------------
// SwapCopyBlt
//
// Does a minification filtered blt, using the current state.

static VOID SwapCopyBlt(
    CDevice* pDevice)
{
    PPUSH pPush = pDevice->StartPush();

    FLOAT xScale = pDevice->m_AntiAliasScaleX;
    FLOAT yScale = pDevice->m_AntiAliasScaleY;

    DWORD right = PixelJar::GetWidth(pDevice->m_pRenderTarget);
    DWORD bottom = PixelJar::GetHeight(pDevice->m_pRenderTarget);

    // I could have used DrawPrimitiveUP here instead of programming the
    // registers directly, but I didn't want to pull in all that UP code.

    PushCount(pPush++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16);

    for (DWORD i = 0; i < 16; i++)
    {
        *pPush++ = SIZEANDTYPE_DISABLED;
    }

    Push1(pPush,
          NV097_SET_VERTEX_DATA_ARRAY_FORMAT(SLOT_POSITION),
          DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F)
        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2));

    Push1(pPush + 2,
          NV097_SET_VERTEX_DATA_ARRAY_FORMAT(SLOT_TEXTURE0),
          DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F)
        | DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, _2));

    Push1(pPush + 4, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_TRIANGLES);

    PushCount(pPush + 6, PUSHER_NOINC(NV097_INLINE_ARRAY), 12);

    DWORD filter = D3D__TextureState[0][D3DTSS_MINFILTER];

    if (DBG_CHECK(TRUE))
    {
        if ((filter == D3DTEXF_QUINCUNX) && 
            ((xScale != 2.0f) || (yScale != 1.0f)))
        {
            DPF_ERR("Quincunx filter is nonsensical unless x scale is 2, "
                    "y scale is 1. \n(Use linear filter instead?)");
        }
        if ((filter == D3DTEXF_GAUSSIANCUBIC) &&
            ((Round(xScale) != xScale) || (Round(yScale) != yScale)))
        {
            DPF_ERR("Gaussian filter is nonsensical unless x and y scale "
                    "are integral. \n(Use linear filter instead?)");
        }
    }

    // Set up the texture coordinate jiggle assuming a linear filter:

    FLOAT uAdjust = 0.0f;
    FLOAT vAdjust = 0.0f;

    // For the convolution kernels, when the scaling is 2x we always
    // contrive to center the kernel around every second pixel.
    // (Quincunx and the 2x multisample format require this, otherwise
    // the result is nonsensical.)  That is, we always want 0.5 in
    // the destination to map to 1.5 in the source:

    if ((filter == D3DTEXF_QUINCUNX) || (filter == D3DTEXF_GAUSSIANCUBIC))
    {
        if (xScale == 2.0f)
            uAdjust = 0.5f;
        if (yScale == 2.0f)
            vAdjust = 0.5f;
    }

    // If you subtract KELVIN_BORDER off of every screen space
    // coordinate, as I'm doing here, you can think of things
    // according to OGL's pixel center convention, which is a whole
    // heck of a lot easier to think with than D3D's.

    FLOAT xAdjust = -pDevice->m_ScreenSpaceOffsetX;
    FLOAT yAdjust = -pDevice->m_ScreenSpaceOffsetY;

    FLOAT* pVertices = (float*) (pPush + 7);

    pVertices[0] = xAdjust;
    pVertices[1] = yAdjust;
    pVertices[2] = uAdjust;
    pVertices[3] = vAdjust;

    pVertices[4] = xAdjust + (4.0f * right);
    pVertices[5] = yAdjust;
    pVertices[6] = uAdjust + (4.0f * (right * xScale)); // !!! Round
    pVertices[7] = vAdjust;

    pVertices[8] = xAdjust;
    pVertices[9] = yAdjust + (4.0f * bottom);
    pVertices[10] = uAdjust;
    pVertices[11] = vAdjust + (4.0f * (bottom * yScale));

    Push1(pPush + 19, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);

    pPush += 21;

    // Account for the fact that we just blew away the vertex format
    // by accessing the registers directly:

    D3D__DirtyFlags |= D3DDIRTYFLAG_VERTEXFORMAT;

    // !!! This also blows away SetVertexShaderInput settings

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// SwapCopy

static VOID SwapCopy(
    CDevice* pDevice,
    DWORD Flags)
{
    PPUSH pPush;
    SwapSavedState savedState;

    ////////////////////////////////////////////////////////////////////////////
    // 1.  Begin

    if (Flags & (D3DSWAP_COPY | D3DSWAP_BYPASSCOPY))
    {
        // Rotate the back-buffers before doing SwapSetState if more
        // than one and we're synchronized to the vertical blank:

        if (pDevice->m_FrameBufferCount == 3)
        {
            // Swap the buffers so that index '1' points to the visible
            // surface:

            D3DSurface* pBuffer1 = pDevice->m_pFrameBuffer[1];
            D3DSurface* pBuffer2 = pDevice->m_pFrameBuffer[2];

            DWORD OldData = pBuffer2->Data;
            DWORD OldLock = pBuffer2->Lock;

            // Swap Data and Lock members.

            pBuffer2->Data = pBuffer1->Data;
            pBuffer2->Lock = pBuffer1->Lock;

            // Complete the swap by copying the old primary into the last
            // slot:

            pBuffer1->Data = OldData;
            pBuffer1->Lock = OldLock;
        }

        SwapSaveSurfaces(pDevice);

        SwapSetSurfaces(pDevice);

        if (Flags & D3DSWAP_COPY)
        {
            // Save away any rendering state we're about to clobber, if we can:

            SwapSaveState(&savedState);                     

            // Set any state in preparation for our final sampling blt.
            //
            // NOTE: We want to do this before the FLIP_STALL for the one-buffer
            //       case, so that the Blt starts as soon as possible at Vblank!

            SwapSetState(&savedState);
        }

        pPush = pDevice->StartPush();

        if (pDevice->m_PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE)
        {
            // 1.  Immediate flip

        }
        else if (pDevice->m_FrameBufferCount < 3)
        {
            // 2.  Synchronized flips with one back-buffer
            //
            // Handle the one-back-buffer case, where we do a Blt that is 
            // started when the next Vblank happens.

            // When doing synchronized blts with one buffer, we release the
            // Blt command at the next Vblank, and when it beats the DAC
            // raster (as it should), then there's no tearing as a result.
            // To ensure that the GPU is ready to go as soon as the Vblank
            // comes where we commit to the blt, we tell the GPU to wait-for-
            // idle *before* processing the NVX_FLIP_SYNCHRONIZED command
            // (which is what will release the GPU to start processing the
            // blt).
            //
            // Note that the FLIP_STALL command automatically synchronizes
            // with the back-buffer, but that is not sufficient in this case.
            // If we did not do the following WAIT_FOR_IDLE, then
            // NVX_FLIP_SYNCHRONIZED could release the Blt while the back-end
            // is still digesting the synchronize in FLIP_STALL, and that
            // could delay the Blt long enough to causing a tear.

            Push1(pPush, NV097_WAIT_FOR_IDLE, 0);

            // Technically, we don't have to set the DAC address on every 
            // Swap for this case.  But NVX_FLIP_SYNCHRONIZED has the 
            // additional nice side effects of handling the gamma ramp 
            // change, and doing the INCREMENT_READ_3D at the next Vblank.

            // NVX_FLIP_* Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE

            DWORD flipAddress = pDevice->m_pFrameBuffer[1]->Data;

            Push1(pPush + 2, NV097_SET_ZSTENCIL_CLEAR_VALUE, flipAddress);

            Push1(pPush + 4, NV097_NO_OPERATION, NVX_FLIP_SYNCHRONIZED);

            Push1(pPush + 6, NV097_FLIP_INCREMENT_WRITE, 0);

            // Due to a bug, the hardware requires a NOP here, otherwise a stall 
            // may go through before the write:

            Push1(pPush + 8, NV097_NO_OPERATION, 0);

            // This command will stall the GPU until the INCREMENT_READ_3D is
            // done at the next Vblank, at which time the GPU is released to
            // immediately start the filter Blt.

            Push1(pPush + 10, NV097_FLIP_STALL, 0);

            pPush += 12;
        }
        else
        {
            // 3.  Synchronized flips with two back-buffers:
        
            Push1(pPush, NV097_FLIP_INCREMENT_WRITE, 0);

            // Due to a bug, the hardware requires a NOP here, otherwise a stall 
            // may go through before the write:

            Push1(pPush + 2, NV097_NO_OPERATION, 0);

            // This command will stall the GPU until one of the two buffers
            // becomes available:

            Push1(pPush + 4, NV097_FLIP_STALL, 0);

            pPush += 6;
        }

        pDevice->EndPush(pPush);
    }

    ////////////////////////////////////////////////////////////////////////////
    // 2.  Blt

    if (Flags & D3DSWAP_COPY)
    {
        SwapCopyBlt(pDevice);
        
        SwapRestoreState(&savedState);
    }

    ////////////////////////////////////////////////////////////////////////////
    // 3.  End

    if (Flags & D3DSWAP_FINISH)
    {
        SwapRestoreSurfaces(pDevice);

        pPush = pDevice->StartPush();

        DWORD flipAddress = pDevice->m_pFrameBuffer[1]->Data;

        if (pDevice->m_PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE)
        {
            // 1.  Immediate flip

            // For the very first 'Swap' call, wait until the Blt completes
            // before letting the flip happen.  Otherwise, the DAC could start
            // showing memory before the Blt gets there, resulting in crap on 
            // the screen:
        
            if (pDevice->m_SwapCount == 1)
            {
                Push1(pPush, NV097_WAIT_FOR_IDLE, 0);
                pPush += 2;
            }
        
            // Technically, we don't have to set the DAC address on every 
            // Swap for this case.  But NVX_FLIP_IMMEDIATE has the 
            // beneficial side-effect of handling any posted gamma ramp 
            // changes.
        
            // NVX_FLIP_IMMEDIATE Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE
        
            Push1(pPush, NV097_SET_ZSTENCIL_CLEAR_VALUE, flipAddress);
        
            Push1(pPush + 2, NV097_NO_OPERATION, NVX_FLIP_IMMEDIATE);
        
            pPush += 4;
        }
        else if (pDevice->m_FrameBufferCount < 3)
        {
            // 2.  Synchronized flips with one back-buffer
        }
        else
        {
            // 3.  Synchronized flips with two back-buffers
        
            // Make sure the back-end isn't still doing work after the next
            // Flip call is started:
        
            Push1(pPush, NV097_WAIT_FOR_IDLE, 0);
        
            // Tell the DAC to scan out of the new buffer.  
            //
            // NVX_FLIP_SYCHRONIZED has the additional nice side effects of 
            // handling the gamma ramp change, and doing the INCREMENT_READ_3D 
            // at the next Vblank.
        
            // NVX_FLIP_* Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE
        
            Push1(pPush + 2, NV097_SET_ZSTENCIL_CLEAR_VALUE, flipAddress);
        
            Push1(pPush + 4, NV097_NO_OPERATION, NVX_FLIP_SYNCHRONIZED);
        
            pPush += 6;
        }

        pDevice->EndPush(pPush);
    }
}


//------------------------------------------------------------------------------
// SwapFlip

static VOID SwapFlip(
    CDevice* pDevice)
{
    // Remember the current primary header information:

    DWORD i = pDevice->m_FrameBufferCount - 1;

    D3DSurface* pBufferThis = pDevice->m_pFrameBuffer[i];

    DWORD OldData = pBufferThis->Data;
    DWORD OldLock = pBufferThis->Lock;

    do {
        // Swap Data and Lock members.

        D3DSurface* pBufferNext = pDevice->m_pFrameBuffer[i - 1];

        pBufferThis->Data = pBufferNext->Data;
        pBufferThis->Lock = pBufferNext->Lock;

        pBufferThis = pBufferNext;

    } while (--i != 0);

    // Complete the swap by copying the old primary into the last slot:

    pBufferThis->Data = OldData;
    pBufferThis->Lock = OldLock;

    // On debug builds, modify the context DMA to point to the
    // new render target address:

    DWORD colorBase = pDevice->m_Miniport.SetDmaRange(
                            D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY,
                            pDevice->m_pFrameBuffer[0]);

    PPUSH pPush = pDevice->StartPush();

    // Send the flip software method.  Buffer one is always the one that
    // the DAC will be scanning from:

    Push1(pPush, NV097_WAIT_FOR_IDLE, 0);

    // NVX_FLIP_* Data stored in NV097_SET_ZSTENCIL_CLEAR_VALUE

    Push1(pPush + 2, 
          NV097_SET_ZSTENCIL_CLEAR_VALUE, 
          pDevice->m_pFrameBuffer[1]->Data);

    if (pDevice->m_PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE)
    {
        Push1(pPush + 4, NV097_NO_OPERATION, NVX_FLIP_IMMEDIATE);

        pPush += 6;
    }
    else
    {
        Push1(pPush + 4, NV097_NO_OPERATION, NVX_FLIP_SYNCHRONIZED);

        Push1(pPush + 6, NV097_FLIP_INCREMENT_WRITE, 0);

        // Due to a bug, the hardware requires a NOP here, otherwise a stall 
        // may go through before the write:

        Push1(pPush + 8, NV097_NO_OPERATION, 0);

        pPush += 10;

        // If the current render target is not a texture, then tell the
        // hardware the location of the new backbuffer:

        if (pDevice->m_pRenderTarget == pDevice->m_pFrameBuffer[0])
        {
            // Stall the graphics pipe until we've flipped (flip_read == 
            // flip_write)

            Push1(pPush, NV097_FLIP_STALL, 0);

            pPush += 2;
        }
        else
        {
            // We don't have to stall the GPU while rendering to texture render
            // targets.  But the first time a SetRenderTarget is done back to
            // the back-buffer, we'll have to synchronize at that point:

            pDevice->m_StateFlags |= STATE_FLIPSTALLPENDING;
        }
    }

    // Set the new render target:

    Push1(pPush,
          NV097_SET_SURFACE_COLOR_OFFSET,
          pDevice->m_pFrameBuffer[0]->Data - colorBase);

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// SwapFirstFlip
//
// Helper function to handle the very first flip

extern BOOL g_FlickerFilterSet;
extern BOOL g_SoftDisplayFilterSet;

static VOID SwapFirstFlip(
    CDevice* pDevice)
{
    DWORD DisplayMode = pDevice->m_Miniport.m_DisplayMode;
    DWORD Format = pDevice->m_Miniport.m_Format;
    DWORD Pitch = pDevice->m_Miniport.m_SurfacePitch;

    void *RegisterBase = pDevice->m_Miniport.m_RegisterBase;
    void *SavedDataAddress;

    ULONG Step;
    ULONG Field;

    KickOffAndWaitForIdle();

    // Set the pitch for a field-rendered mode to half of its true
    // value.
    //
    if (pDevice->m_Miniport.m_CurrentAvInfo & AV_FLAGS_FIELD)
    {
        Pitch /= 2;
    }

    DWORD flipAddress = pDevice->m_pFrameBuffer[1]->Data;

#ifdef STARTUPANIMATION

    REG_WR32(RegisterBase, NV_PCRTC_START, flipAddress);

#else

    Step = 0;

    do
    {
        D3DDevice_BlockUntilVerticalBlank();

        Step = AvSetDisplayMode(RegisterBase, Step, DisplayMode, Format, Pitch, flipAddress);
    }
    while (Step);

    // Field-rendered modes require that we turn off the flicker filter.

    if (pDevice->m_Miniport.m_CurrentAvInfo & AV_FLAGS_FIELD)
    {
        AvSendTVEncoderOption(pDevice->m_Miniport.m_RegisterBase, 
                              AV_OPTION_FLICKER_FILTER,
                              0,
                              NULL);
    }

    // Guess as to which field is currently being displayed and synchronize
    // the vblank count with it.

    AvSendTVEncoderOption(pDevice->m_Miniport.m_RegisterBase, AV_OPTION_GUESS_FIELD, 0, &Field);

    if ((pDevice->m_Miniport.m_VBlankCount & 1) == (Field & 1))
    {
        pDevice->m_Miniport.m_VBlankCount++;
    }

    // Make sure that the next call to these APIs don't get short-circuited.

    g_FlickerFilterSet = FALSE;
    g_SoftDisplayFilterSet = FALSE;

#endif

    // Clean up any buffers that were saved across reboots.

    SavedDataAddress = AvGetSavedDataAddress();

    if (SavedDataAddress != NULL)
    {
        SIZE_T AllocSize = MmQueryAllocationSize(SavedDataAddress);
        MmPersistContiguousMemory(SavedDataAddress, AllocSize, FALSE);
        MmFreeContiguousMemory(SavedDataAddress);
        AvSetSavedDataAddress(NULL);
    }

    // Unblank the screen that was blanked when the mode change first started.

    AvSendTVEncoderOption(RegisterBase, AV_OPTION_BLANK_SCREEN, FALSE, NULL);        

    pDevice->m_Miniport.m_FirstFlip = FALSE;
}

//------------------------------------------------------------------------------
// SwapStart

static VOID SwapStart(
    CDevice* pDevice)
{
    // Let any objects that are no longer being used be unlocked before
    // the Vblank occurs:

    SetFence(SETFENCE_NOKICKOFF);

    // Flip throttling.
    //
    // We never let the app have more than 3 frames queued up in the
    // push buffer at any time (3 is somewhat arbitrary, but should be
    // more than enough).  It's unlikely that games will ever get more
    // than 3 frames ahead, so a busy-loop here is just fine.

    DWORD blockTime = pDevice->m_SwapTime[
                (pDevice->m_SwapCount + 1) & (SWAP_THROTTLE_QUEUE_SIZE - 1)];

    if (blockTime != 0)
    {
        BlockOnTime(blockTime, FALSE);
    }

    // We need to do this increment after the block to reduce the amount of time
    // that the screen capture code in the debugger can be hosed.  We were
    // seeing a case where we would increment this value then block on the
    // above BlockOnTime at which point the user did an "xbcapture".  The 
    // screenshot logic would then stop all threads and wait for the swap
    // count to match the vblank flip count...something that would never happen
    // because we never actually pushed out the instructions to the GPU to
    // do the swap.  There still exists a possibility of this happening
    // if this thread loses its quantum in between now and when the swap
    // gets pushed but this thread is a lot more likely to get interrupted
    // in the above block then now and if it does get interrupted then the
    // capture will fail this one time.
    //
    // I don't think we can ever get this perfect without adding a bunch of
    // thread locking code to make this part of a swap atomic and that's 
    // something that we definately do not want to do just for the screenshot
    // utility.  If it fails, they can try it again.  But this change makes it
    // work 99% of the time instead of the 1% it was working before if we
    // entered the above BlockOnTime.

    pDevice->m_SwapCount++;
}

//------------------------------------------------------------------------------
// SwapFinish

static VOID SwapFinish(
    CDevice* pDevice)
{
    // Update the time-stamp of the framebuffer which we've just made
    // visible, in order to make sure that a LockRect() can't be done
    // on it until the flip is actually processed by the DAC (usually at 
    // the next VBlank), or that the copy-blt is completely done.
    //
    // Note that we do NOT have to do this for the framebuffer to which 
    // we're now rendering, since that is handled by the normal
    // IsResourceSetInDevice logic.

    pDevice->RecordSurfaceWritePush(pDevice->m_pFrameBuffer[1]);

    // Mark the time.  
    //
    // Calling SetFence() has the added benefit of doing a KickOff, 
    // which ensures that all the push-buffer instructions to handle 
    // our Swap don't get stuck in the queue forever:

    pDevice->m_SwapTime[pDevice->m_SwapCount 
                        & (SWAP_THROTTLE_QUEUE_SIZE - 1)] = SetFence(0);

    // Mode changes are deferred until after the first flip.  Wait around
    // until that flip happens and set the new mode.

    if (pDevice->m_Miniport.m_FirstFlip)
    {
        SwapFirstFlip(pDevice);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_Swap

extern "C"
DWORD WINAPI D3DDevice_Swap(
    DWORD Flags)
{
    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        static BOOL InBegin;
        if (Flags & (D3DSWAP_COPY | D3DSWAP_BYPASSCOPY))
        {
            if (InBegin)
            {
                DPF_ERR("D3DSWAP_COPY/BYPASSCOPY before a previous one was FINISHed");
            }
            InBegin = TRUE;
        }
        if (Flags & D3DSWAP_FINISH)
        {
            if (!InBegin)
            {
                DPF_ERR("D3DSWAP_FINISH without a D3DSWAP_COPY/BYPASSCOPY first");
            }
            InBegin = FALSE;
        }
        if (Flags & ~(D3DSWAP_COPY | D3DSWAP_BYPASSCOPY | D3DSWAP_FINISH))
        {
            DPF_ERR("Invalid flag");
        }
        if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        {
            DPF_ERR("Can't call Swap while recording a push-buffer");
        }
        // only do end-of-field processing if this is the end of field
        if ((pDevice->m_dwOpcode) &&
            ((Flags == 0) || (Flags & D3DSWAP_FINISH)))
        {
            HandleShaderSnapshotOpcode();
        }
    }

    // Reset debug marker to 0 on each call to swap
    // (compiled away on non-debug build)
    pDevice->SetDebugMarker(0);

    if (Flags == 0)
    {
        Flags = D3DSWAP_COPY | D3DSWAP_FINISH;
    }

#if PROFILE

    // This var needs to persist between a beginning call to Swap and a Finish
    // call to swap.
    static D3DPERFEvent *pD3DPerfEvent;
    
    // Count this as the beginning of Present if (copy || bypasscopy) is specified
    if (Flags & (D3DSWAP_COPY | D3DSWAP_BYPASSCOPY))
    {
        COUNT_API(API_D3DDEVICE_PRESENT);
        pD3DPerfEvent = D3DPERF_PerfEventStart(D3DPERFEvent_Present, TRUE);
    }

#endif

    if (pDevice->m_StateFlags & STATE_COPYSWAP)
    {
        if (Flags & (D3DSWAP_COPY | D3DSWAP_BYPASSCOPY))
            SwapStart(pDevice);

        SwapCopy(pDevice, Flags);

        if (Flags & D3DSWAP_FINISH)
            SwapFinish(pDevice);
    }
    else if (Flags & D3DSWAP_FINISH)
    {
        SwapStart(pDevice);

        SwapFlip(pDevice);

        SwapFinish(pDevice);
    }
    
#if PROFILE

    // Count this as an end of Present if Finish is specified
    if(Flags & D3DSWAP_FINISH)
    {
        D3DPERF_PerfEventEnd(pD3DPerfEvent, TRUE);
        D3DPERF_HandlePresent();
    }

#endif

    return pDevice->m_SwapCount;
}

//------------------------------------------------------------------------------
// D3DDevice_SetBackBufferScale

extern "C"
void WINAPI D3DDevice_SetBackBufferScale(
    FLOAT xScale,
    FLOAT yScale)
{
    CDevice* pDevice = g_pDevice;

    COUNT_API(API_D3DDEVICE_SETBACKBUFFERSCALE);

    // The scale is always relative to the original antialiasing
    // scale factor:

    xScale *= ANTIALIAS_XSCALE(pDevice->m_MultiSampleType);
    yScale *= ANTIALIAS_YSCALE(pDevice->m_MultiSampleType);

    if (DBG_CHECK(TRUE))
    {
        if ((xScale <= 0.0f) || (yScale <= 0.0f))
        {
            DPF_ERR("Invalid scale value");
        }

        // Calculate the effective post- and pre-transform dimensions:

        DWORD drawWidth 
            = Round(PixelJar::GetWidth(pDevice->m_pFrameBuffer[1]) * xScale);
        DWORD drawHeight 
            = Round(PixelJar::GetHeight(pDevice->m_pFrameBuffer[1]) * yScale);

        DWORD surfaceWidth = PixelJar::GetWidth(pDevice->m_pFrameBuffer[0]);
        DWORD surfaceHeight = PixelJar::GetHeight(pDevice->m_pFrameBuffer[0]);

        if ((drawWidth > surfaceWidth) || (drawHeight > surfaceHeight))
        {
            DPF_ERR("Scale causes rendering to exceed pre-filter surface dimensions");
        }
    }

    // We handle SetBackBufferScale even for D3DSWAPEFFECT_FLIP by converting
    // on-the-fly to a 'copy' effect.  We also switch back to a 'flip' effect
    // when possible:

    if (pDevice->m_SwapEffect != D3DSWAPEFFECT_COPY)
    {
        if ((xScale == 1.0f) && (yScale == 1.0f))
        {
            // We can only convert to flipping when the format, dimensions, and 
            // stride are the same for each of the back-buffer surfaces.  It's 
            // sufficient just to check the back-buffer and one of the front-
            // buffers:

            if ((pDevice->m_pFrameBuffer[0]->Size 
                            == pDevice->m_pFrameBuffer[1]->Size) &&
                (pDevice->m_pFrameBuffer[0]->Format 
                            == pDevice->m_pFrameBuffer[1]->Format))
            {
                pDevice->m_StateFlags &= ~STATE_COPYSWAP;
            }
        }
        else
        {
            pDevice->m_StateFlags |= STATE_COPYSWAP;

            if (pDevice->m_StateFlags & STATE_FLIPSTALLPENDING)
            {
                pDevice->m_StateFlags &= ~STATE_FLIPSTALLPENDING;

                PPUSH pPush = pDevice->StartPush();

                Push1(pPush, NV097_FLIP_STALL, 0);

                pDevice->EndPush(pPush + 2);
            }
        }
    }

    pDevice->m_AntiAliasScaleX = xScale;
    pDevice->m_AntiAliasScaleY = yScale;

    D3DDevice_SetRenderTarget(pDevice->m_pRenderTarget, pDevice->m_pZBuffer);
}

//------------------------------------------------------------------------------
// D3DDevice_GetBackBufferScale

extern "C"
void WINAPI D3DDevice_GetBackBufferScale(
    FLOAT* pxScale,
    FLOAT* pyScale)
{
    CDevice* pDevice = g_pDevice;

    COUNT_API(API_D3DDEVICE_GETBACKBUFFERSCALE);

    *pxScale = pDevice->m_AntiAliasScaleX;
    *pyScale = pDevice->m_AntiAliasScaleY;
}

} // end namespace
