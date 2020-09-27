//depot/xbox/private/windows/directx/dxg/d3d8/se/d3dbase.cpp#125 - edit change 24474 (text)
/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3dbase.cpp
 *  Content:    Direct3D base device implementation
 *
 ***************************************************************************/

#include "precomp.hpp"

// Tell the linker to merge constant data and data sections into code section.
#pragma comment(linker, "/merge:D3D_RD=D3D")
#pragma comment(linker, "/merge:D3D_RW=D3D")
#pragma comment(linker, "/merge:D3D_URW=D3D")

// Tell the linker that the code section contains read/write data.
#pragma comment(linker, "/section:D3D,ERW")

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// g_DeviceCaps

D3DCONST D3DCAPS8 g_DeviceCaps = 
{
   D3DDEVTYPE_HAL                           , // DeviceType               
   0x0                                      , // AdapterOrdinal           
   KELVIN_CAPS_CAPS                         , // Caps                     
   0                                        , // Caps2                    
   0x0                                      , // Caps3                   
   D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_TWO | D3DPRESENT_INTERVAL_IMMEDIATE                                
                                            , // PresentationIntervals    
   0x0                                      , // CursorCaps               
   KELVIN_DEVCAPS_FLAGS                     , // DevCaps                  
   KELVIN_TRICAPS_MISC                      , // PrimitiveMiscCaps        
   KELVIN_TRICAPS_RASTER                    , // RasterCaps               
   KELVIN_TRICAPS_ZCMP                      , // ZCmpCaps                 
   KELVIN_TRICAPS_SRCBLEND                  , // SrcBlendCaps             
   KELVIN_TRICAPS_DESTBLEND                 , // DestBlendCaps            
   KELVIN_TRICAPS_ALPHACMP                  , // AlphaCmpCaps             
   KELVIN_TRICAPS_SHADE                     , // ShadeCaps                
   KELVIN_TRICAPS_TEXTURE                   , // TextureCaps              
   KELVIN_TRICAPS_TEXTUREFILTER             , // TextureFilterCaps        
   KELVIN_TRICAPS_TEXTUREFILTER             , // CubeTextureFilterCaps    
   0x0                                      , // VolumeTextureFilterCaps  
   KELVIN_TRICAPS_TEXTUREADDRESS            , // TextureAddressCaps       
   0x0                                      , // VolumeTextureAddressCaps 
   KELVIN_CAPS_LINECAPS                     , // LineCaps                 
   0x1000                                   , // MaxTextureWidth          
   0x1000                                   , // MaxTextureHeight         
   0x0                                      , // MaxVolumeExtent          
   KELVIN_CAPS_MAX_TEXTURE_REPEAT           , // MaxTextureRepeat         
   KELVIN_CAPS_MAX_TEXTURE_ASPECT_RATIO     , // MaxTextureAspectRatio    
   KELVIN_CAPS_MAX_ANISOTROPY               , // MaxAnisotropy            
   1.0e10                                   , // MaxVertexW               
   KELVIN_CAPS_GUARD_BAND_LEFT              , // GuardBandLeft            
   KELVIN_CAPS_GUARD_BAND_TOP               , // GuardBandTop             
   KELVIN_CAPS_GUARD_BAND_RIGHT             , // GuardBandRight           
   KELVIN_CAPS_GUARD_BAND_BOTTOM            , // GuardBandBottom          
   KELVIN_CAPS_EXTENTS_ADJUST               , // ExtentsAdjust            
   KELVIN_CAPS_STENCILOPS                   , // StencilCaps              
   KELVIN_CAPS_FVF_CAPS                     , // FVFCaps                  
   KELVIN_CAPS_TEXTUREOPS                   , // TextureOpCaps            
   KELVIN_CAPS_MAX_TEXTURE_BLEND_STAGES     , // MaxTextureBlendStages    
   KELVIN_CAPS_MAX_SIMULTANEOUS_TEXTURES    , // MaxSimultaneousTextures  
   KELVIN_CAPS_VTXPCAPS                     , // VertexProcessingCaps     
   KELVIN_CAPS_MAX_ACTIVE_LIGHTS            , // MaxActiveLights          
   0                                        , // MaxUserClipPlanes [NOTE: Xbox doesn't support user clip planes]
   KELVIN_CAPS_MAX_VERTEX_BLEND_MATRICES    , // MaxVertexBlendMatrices   
   0                                        , // MaxVertexBlendMatrixIndex
   64.0f                                    , // MaxPointSize             
   0xffff                                   , // MaxPrimitiveCount        
   0xffff                                   , // MaxVertexIndex           
   KELVIN_CAPS_MAX_STREAMS                  , // MaxStreams               
   0xff                                     , // MaxStreamStride          
   D3DPS_VERSION(1,1)                       , // VertexShaderVersion      
   KELVIN_CAPS_MAX_VSHADER_CONSTS           , // MaxVertexShaderConst     
   D3DPS_VERSION(1,1)                       , // PixelShaderVersion       
   1                                        , // MaxPixelShaderValue      
};

//------------------------------------------------------------------------------
// D3DDevice_GetDirect3D

extern "C"
void WINAPI D3DDevice_GetDirect3D(
    Direct3D** ppD3D8) 
{ 
    COUNT_API(API_D3DDEVICE_GETDIRECT3D);

    *ppD3D8 = (Direct3D *) 1;
}

//------------------------------------------------------------------------------
// D3DDevice_GetDeviceCaps

extern "C"
void WINAPI D3DDevice_GetDeviceCaps(
    D3DCAPS8* pCaps) 
{ 
    COUNT_API(API_D3DDEVICE_GETDEVICECAPS);
  
    *pCaps = g_DeviceCaps;
}

//------------------------------------------------------------------------------
// D3DDevice_GetDisplayMode

extern "C"
void WINAPI D3DDevice_GetDisplayMode(
    D3DDISPLAYMODE* pMode) 
{ 
    COUNT_API(API_D3DDEVICE_GETDISPLAYMODE);
    
    // Note that initially this will be all zeroes before the first mode
    // set:

    CDevice* pDevice = g_pDevice;

    pMode->Width = PixelJar::GetWidth(pDevice->m_pFrameBuffer[1]);
    pMode->Height = PixelJar::GetHeight(pDevice->m_pFrameBuffer[1]);
    pMode->RefreshRate = pDevice->m_Miniport.GetRefreshRate();
    pMode->Flags = CMiniport::GetPresentFlagsFromAvInfo(pDevice->m_Miniport.m_CurrentAvInfo);
    
    // Always return the format of the post-filter buffer:

    pMode->Format = DecodeD3DFORMAT(pDevice->m_pFrameBuffer[1]->Format);
}

//------------------------------------------------------------------------------
// D3DDevice_GetDisplayMode
//
// DOC: Note restricted Xbox returns

extern "C"
void WINAPI D3DDevice_GetCreationParameters(
    D3DDEVICE_CREATION_PARAMETERS *pParameters) 
{ 
    COUNT_API(API_D3DDEVICE_GETCREATIONPARAMETERS);

    CDevice* pDevice = g_pDevice;

    ZeroMemory(pParameters, sizeof(*pParameters));

    pParameters->DeviceType = D3DDEVTYPE_HAL;

    pParameters->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;

    if (pDevice->m_StateFlags & STATE_PUREDEVICE)
    {
        pParameters->BehaviorFlags |= D3DCREATE_PUREDEVICE;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_Reset

extern "C"
HRESULT WINAPI D3DDevice_Reset(
    D3DPRESENT_PARAMETERS* pPresentationParams) 
{ 
    COUNT_API(API_D3DDEVICE_RESET);
    HRESULT hr;

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        hr = PresentationParametersParameterCheck(pPresentationParams);
        if (FAILED(hr))
            return hr;
    }

    // Flush in case there is still rendering pending in the push buffer,
    // so that the reset is synchronous.

    KickOffAndWaitForIdle();

    // Wait for any last synchronized flip to get processed, in order to
    // make sure that the final frame is displayed, and so that the flip
    // synchronization code doesn't get messed up by the INCREMENT_READ_3D
    // done at the Vblank interrupt.

    while (pDevice->m_Miniport.IsFlipPending())
        ;

    // Recreate the frame and back buffers.

    pDevice->FreeFrameBuffers();
    hr = pDevice->InitializeFrameBuffers(pPresentationParams);

    if (FAILED(hr)) 
    {
        /// This is very bad, an application can't present now, and we're
        //  in the wrong mode:
        
        DXGRIP("Failed to recreate frame and back buffers on Reset!");
        pDevice->FreeFrameBuffers();
        return hr;
    }

    pDevice->m_Miniport.SetVideoMode(pPresentationParams->BackBufferWidth, 
                                     pPresentationParams->BackBufferHeight, 
                                     pPresentationParams->FullScreen_RefreshRateInHz,
                                     pPresentationParams->Flags,
                                     pPresentationParams->BackBufferFormat,
                                     pPresentationParams->FullScreen_PresentationInterval,
                                     PixelJar::GetPitch(pDevice->m_pFrameBuffer[1]));

    // Set the render target with the new buffers.

    D3DDevice_SetRenderTarget(pDevice->m_pFrameBuffer[0], pDevice->m_pAutoDepthBuffer);

    // Clear the Z and stencil buffers.  We do this mainly to clear stencil
    // to ensure that there's no random bits left in there which may muck
    // up the Z compression.
    //
    // Note that we don't bother with clearing the color buffer.

    D3DDevice_Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

    return S_OK;
}

//------------------------------------------------------------------------------
// g_LODBias
//
// A simple log2 table...

D3DCONST FLOAT g_LODBias2x[] = 
{ 
    0.000f, // 1.0
    0.585f, // 1.5
    1.000f, // 2.0
    1.322f, // 2.5
    1.585f, // 3.0
};

//------------------------------------------------------------------------------
// D3DDevice_SetRenderTarget

extern "C"
void WINAPI D3DDevice_SetRenderTarget(
    D3DSurface* pRenderTarget,
    D3DSurface* pZBuffer) 
{
    DWORD multisampleType;
    FLOAT xScale;
    FLOAT yScale;
    DWORD width;
    DWORD height;

    COUNT_API(API_D3DDEVICE_SETRENDERTARGET);

    CDevice *pDevice = g_pDevice;

    // If the render target is NULL, just reuse the existing target.

    if (pRenderTarget == NULL)
    {
        pRenderTarget = pDevice->m_pRenderTarget;
    }

    DWORD pitch = PixelJar::GetPitch(pRenderTarget);
    DWORD Zpitch = pZBuffer ? PixelJar::GetPitch(pZBuffer) : pitch;

    if (pRenderTarget == pDevice->m_pFrameBuffer[0]) 
    {
        // We're rendering to the back-buffer.  Compute the effective 
        // dimensions from the post-filter buffer size multiplied by
        // the antialiasing scale factor:

        multisampleType = D3D__RenderState[D3DRS_MULTISAMPLEMODE];
        xScale = pDevice->m_AntiAliasScaleX;
        yScale = pDevice->m_AntiAliasScaleY;
        width = Round(xScale * 
                      PixelJar::GetWidth(pDevice->m_pFrameBuffer[1]));
        height = Round(yScale * 
                       PixelJar::GetHeight(pDevice->m_pFrameBuffer[1]));

        // Make sure the resulting dimensions don't exceed our target
        // surface size:

        width = min(width, PixelJar::GetWidth(pDevice->m_pFrameBuffer[0]));
        height = min(height, PixelJar::GetHeight(pDevice->m_pFrameBuffer[0]));
    }
    else
    {
        // We're rendering to a non-back-buffer render target.

        multisampleType = D3D__RenderState[D3DRS_MULTISAMPLERENDERTARGETMODE];
        xScale = 1.0f;
        yScale = 1.0f;
        width = PixelJar::GetWidth(pRenderTarget);
        height = PixelJar::GetHeight(pRenderTarget);
    }

    if (DBG_CHECK(TRUE))
    {
        if (!PixelJar::IsValidRenderTarget(pRenderTarget))
        {
            DPF_ERR("Invalid render target format.");
        }

        if (!PixelJar::IsSurface(pRenderTarget))
        {
            DPF_ERR("Render target not a true surface (D3DCOMMON_TYPE_SURFACE)");
        }
        
        if ((pZBuffer != NULL) && (!PixelJar::IsSurface(pZBuffer)))
        {
            DPF_ERR("ZStencil not a true surface (D3DCOMMON_TYPE_SURFACE)");
        }

        if (pRenderTarget->Data % D3DSURFACE_ALIGNMENT)
        {
            DPF_ERR("The data for a render target must be 64-byte aligned.");
        }

        if (width > 4096 || height > 4096)
        {
            DPF_ERR("The render target can be at most 4096x4096.");
        }

        if (!PixelJar::IsSwizzled(pRenderTarget))
        {
            if (pitch < 64 || pitch > 65535 || (pitch & 0x3F) != 0)
            {
                DPF_ERR("An unswizzled render target requires a pitch that is nonzero, a multiple of 64 and is less than 64K.");
            }
        }
        else if (pitch < 64)
        {
            DPF_ERR("A swizzled render target needs a pitch >= 64 bytes (width * bytes per pixel, no padding).");
        }

        if (pZBuffer)
        {
            if (Zpitch < 64 || Zpitch > 65535 || (Zpitch & 0x3F) != 0)
            {
                DPF_ERR("An ZBuffer requires a pitch that is nonzero, a multiple of 64 and is less than 64K.");
            }
        }

        if (width == 1 && height != 1)
        {
            DPF_ERR("A render target that is only 1 pixel wide must also be 1 pixel high.");
        }

        if (pZBuffer)
        {
            if (PixelJar::IsSwizzled(pZBuffer))
            {
                DPF_ERR("A swizzled depth buffer can't be used in rendering.");
            }

            if (PixelJar::IsSwizzled(pRenderTarget)
                && PixelJar::GetBitsPerPixel(pRenderTarget) != PixelJar::GetBitsPerPixel(pZBuffer))
            {
                DPF_ERR("A swizzled render target requires a depth-buffer format with the same number of bits-per-pixel.");
            }

            if (!PixelJar::IsValidDepthBuffer(pZBuffer))
            {
                DPF_ERR("Invalid depth buffer format.");
            }

            if (pRenderTarget->Data % D3DSURFACE_ALIGNMENT)
            {
                DPF_ERR("The data for a ZBuffer must be 64-byte aligned.");
            }

            if ((width > PixelJar::GetWidth(pZBuffer)) 
                || (height > PixelJar::GetHeight(pZBuffer)))
            {
                DPF_ERR("The depth buffer must be at least as big as the "
                        "render target");
            }
        }
    }

    // Do what we can using hardware multisampling.
    //
    // 'width' and 'height' as given to SET_SURFACE_CLIP should not be
    // expanded values when multisampling.  

    DWORD setSurfaceFormatAntiAliasing = 0;
    pDevice->m_StateFlags &= ~STATE_MULTISAMPLING;
    if (multisampleType != D3DMULTISAMPLEMODE_1X)
    {
        pDevice->m_StateFlags |= STATE_MULTISAMPLING;

        // When multisampling, the hardware automatically scales
        // everything for us...

        xScale /= 2.0f;
        width = (width + 1) >> 1;
        setSurfaceFormatAntiAliasing = DRF_DEF(097, 
            _SET_SURFACE_FORMAT, _ANTI_ALIASING, _CENTER_CORNER_2);

        if (multisampleType == D3DMULTISAMPLEMODE_4X)
        {
            yScale /= 2.0f;
            height = (height + 1) >> 1;
            setSurfaceFormatAntiAliasing = DRF_DEF(097, 
                _SET_SURFACE_FORMAT, _ANTI_ALIASING, _SQUARE_OFFSET_4);
        }
    }

    // Set the render target surface.  'm_pRenderTarget' can be NULL during
    // initialization.

    InternalAddRefSurface(pRenderTarget);

    if (pDevice->m_pRenderTarget)
    {
        pDevice->RecordSurfaceWritePush(pDevice->m_pRenderTarget);

        InternalReleaseSurface(pDevice->m_pRenderTarget);
    }

    pDevice->m_pRenderTarget = pRenderTarget;

    // Set the z-buffer surface.

    if (pDevice->m_pZBuffer)
    {
        pDevice->RecordSurfaceWritePush(pDevice->m_pZBuffer);

        InternalReleaseSurface(pDevice->m_pZBuffer);
    }

    pDevice->m_pZBuffer = pZBuffer;

    if (pZBuffer)
    {
        InternalAddRefSurface(pDevice->m_pZBuffer);

        PixelJar::GetDepthBufferScale(pZBuffer, &pDevice->m_ZScale);
    }

    // If range-checking is enabled, we modify the context DMAs to have the 
    // same range as the destination surface.  This is useful to try to catch
    // errant rendering:

    DWORD colorBase = pDevice->m_Miniport.SetDmaRange(
                            D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY, 
                            pRenderTarget);

    DWORD zetaBase = pDevice->m_Miniport.SetDmaRange(
                            D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                            pZBuffer); 

    ////////////////////////////////////////////////////////////////////////////
    // Tell the hardware about the new buffers.

    PPUSH pPush = pDevice->StartPush();

    // Work around a hardware problem we hit with numerous titles.  The 
    // problem was very timing dependent - typically after a day or so
    // of running, the hardware wouldn't properly register the new stride
    // or offset value, and would proceed to draw with the old values,
    // causing considerable memory corruption.  Nvidia still hasn't been
    // able to tell us why yet, but hitting the registers twice seems to 
    // fix the problem.  I'm not sure if the NOPs and WAIT_FOR_IDLEs are
    // necessary, but they won't hurt performance (since these commands 
    // have to do implicit WAIT_FOR_IDLEs anyways).

    DWORD colorOffset = pRenderTarget->Data - colorBase;
    DWORD Zoffset = (pZBuffer != NULL) ? (pZBuffer->Data - zetaBase) : 0;

    for (DWORD iterations = 0; iterations < 2; iterations++)
    {
        Push1(pPush, NV097_NO_OPERATION, 0);
        Push1(pPush + 2, NV097_SET_SURFACE_PITCH,
                     DRF_NUM(097, _SET_SURFACE_PITCH, _COLOR, pitch)
                   | DRF_NUM(097, _SET_SURFACE_PITCH, _ZETA, Zpitch));
        Push1(pPush + 4, NV097_NO_OPERATION, 0);
        Push1(pPush + 6, NV097_WAIT_FOR_IDLE, 0);
        pPush += 8;

        Push1(pPush, NV097_NO_OPERATION, 0);
        Push1(pPush + 2, NV097_SET_SURFACE_COLOR_OFFSET, colorOffset);
        Push1(pPush + 4, NV097_NO_OPERATION, 0);
        Push1(pPush + 6, NV097_WAIT_FOR_IDLE, 0);
        pPush += 8;

        Push1(pPush, NV097_NO_OPERATION, 0);
        Push1(pPush + 2, NV097_SET_SURFACE_ZETA_OFFSET, Zoffset);
        Push1(pPush + 4, NV097_NO_OPERATION, 0);
        Push1(pPush + 6, NV097_WAIT_FOR_IDLE, 0);
        pPush += 8;
    }

    Push2(pPush, NV097_SET_SURFACE_CLIP_HORIZONTAL, 

              // NV097_SET_SURFACE_CLIP_HORIZONTAL:

                 DRF_NUM(097, _SET_SURFACE_CLIP_HORIZONTAL, _X, 0)
               | DRF_NUM(097, _SET_SURFACE_CLIP_HORIZONTAL, _WIDTH, width),

              // NV097_SET_SURFACE_CLIP_VERTICAL:

                 DRF_NUM(097, _SET_SURFACE_CLIP_VERTICAL, _Y, 0)
               | DRF_NUM(097, _SET_SURFACE_CLIP_VERTICAL, _HEIGHT, height));

    // Set the special Z formats.

    pPush = CommonSetControl0(pDevice, pPush + 3);

    // Enable Z (or not).

    Push1(pPush, 
          NV097_SET_DEPTH_TEST_ENABLE, 
          ((D3D__RenderState[D3DRS_ZENABLE] != D3DZB_FALSE) && 
           (pDevice->m_pZBuffer != NULL)));

    // Enable stencil (or not).

    Push1(pPush + 2,
          NV097_SET_STENCIL_TEST_ENABLE,
          (D3D__RenderState[D3DRS_STENCILENABLE] && 
           (pDevice->m_pZBuffer != NULL)));

    pPush += 4;

    // Handle any pending flip synchronization:

    if ((pDevice->m_StateFlags & STATE_FLIPSTALLPENDING) &&
        (pRenderTarget == pDevice->m_pFrameBuffer[0]))
    {
        pDevice->m_StateFlags &= ~STATE_FLIPSTALLPENDING;

        Push1(pPush, NV097_FLIP_STALL, 0);

        pPush += 2;
    }

    // Set the surface type and multisampling mode.

    Push1(pPush, NV097_SET_SURFACE_FORMAT,
          setSurfaceFormatAntiAliasing
          | PixelJar::GetSurfaceFormat(pRenderTarget, pZBuffer));

    pDevice->EndPush(pPush + 2);

    ////////////////////////////////////////////////////////////////////////////

    pDevice->m_SuperSampleScaleX = xScale;
    pDevice->m_SuperSampleScaleY = yScale;

    FLOAT minScale = min(xScale, yScale);

    if (minScale != pDevice->m_SuperSampleScale)
    {
        pDevice->m_SuperSampleScale = minScale;
        pDevice->m_SuperSampleLODBias = g_LODBias2x[Round(2.0f * minScale) - 2];

        // Point sprites and line widths are both affected by the super-
        // sampling factor:

        D3D__DirtyFlags |= D3DDIRTYFLAG_POINTPARAMS;

        D3DDevice_SetRenderState(D3DRS_LINEWIDTH, 
                                 D3D__RenderState[D3DRS_LINEWIDTH]);
    }

    // Changing the antialiasing scale factors will affect the XYZRHW program
    // if loaded.

    CommonSetPassthroughProgram(pDevice);

    // Update the viewport, to reflect the new dimensions, possible new
    // ZScale, and new supersampling scaling.  

    D3DVIEWPORT8 viewport = { 0, 0, 0x7fffffff, 0x7fffffff, 0.0f, 1.0f };

    D3DDevice_SetViewport(&viewport);

    // Update the DXT noise-enable status if necessary:

    D3DDevice_SetRenderState_Dxt1NoiseEnable(
                                D3D__RenderState[D3DRS_DXT1NOISEENABLE]);
}

//------------------------------------------------------------------------------
// D3DDevice_GetBackBuffer

extern "C"
void WINAPI D3DDevice_GetBackBuffer(
    INT BackBuffer,
    D3DBACKBUFFER_TYPE Type,
    D3DSurface** ppBackBuffer) 
{ 
    DWORD index;

    COUNT_API(API_D3DDEVICE_GETBACKBUFFER);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (BackBuffer >= (INT) (pDevice->m_FrameBufferCount - 1))
        {
            DPF_ERR("BackBuffer index is too big.");
        }
        if (BackBuffer < -1)
        {
            DPF_ERR("Invalid index");
        }
        if (Type != D3DBACKBUFFER_TYPE_MONO)
        {
            DPF_ERR("Xbox supports only D3DBACKBUFFER_TYPE_MONO.");
        }
    }

    // -1 is the current frame buffer, 0 is the currently rendered back buffer:

    if (BackBuffer == -1)
        index = 1;
    else if (BackBuffer == 0)
        index = 0;
    else
        index = 2;

    *ppBackBuffer = pDevice->m_pFrameBuffer[index];
    (*ppBackBuffer)->AddRef();
}

//------------------------------------------------------------------------------
// D3DDevice_GetRasterStatus

extern "C"
void WINAPI D3DDevice_GetRasterStatus(
    D3DRASTER_STATUS* pRasterStatus) 
{ 
    COUNT_API(API_D3DDEVICE_GETRASTERSTATUS);
     
    ULONG value;
    CDevice* pDevice = g_pDevice;
    HWREG* NvBase = pDevice->m_NvBase;

    value = REG_RD_DRF(NvBase, _PCRTC, _RASTER, _POSITION);

    if (value == 0 || value >= PixelJar::GetHeight(pDevice->m_pFrameBuffer[1]))
    {
        pRasterStatus->InVBlank = TRUE;
        pRasterStatus->ScanLine = 0;
    }
    else
    {
        pRasterStatus->InVBlank = FALSE;
        pRasterStatus->ScanLine = value;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetGammaRamp

extern "C"
void WINAPI D3DDevice_SetGammaRamp(
    DWORD Flags,
    CONST D3DGAMMARAMP* pRamp) 
{  
    COUNT_API(API_D3DDEVICE_SETGAMMARAMP);

    CDevice* pDevice = g_pDevice;
    CMiniport* pMiniport = &(pDevice->m_Miniport);

    // We can only ever have a maximum of two Presents queued up in the push
    // buffer, since we throttle 

    // Choose between the two gamma ramps based on the LSB of
    // current present count

    ULONG RampNo = ((pDevice->m_SwapCount) & 1);

    memcpy(&(pMiniport->m_GammaRamp[RampNo]), pRamp, sizeof(D3DGAMMARAMP));
    
    pMiniport->m_GammaCurrentIndex = RampNo;

    if (Flags & D3DSGR_IMMEDIATE)
    {
        // Immediately set the gamma ramp values
        
        pMiniport->DacProgramGammaRamp(&(pMiniport->m_GammaRamp[RampNo]));
    }
    else
    {
        // Synchronize it with the Vblank flip or immediate flips 
        
        pMiniport->m_GammaUpdated[RampNo] = TRUE;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetGammaRamp

extern "C"
void WINAPI D3DDevice_GetGammaRamp(
    D3DGAMMARAMP* pRamp)
{
    COUNT_API(API_D3DDEVICE_GETGAMMARAMP);

    CDevice* pDevice = g_pDevice;

    ULONG RampNo = pDevice->m_Miniport.m_GammaCurrentIndex;
    memcpy(pRamp, &(pDevice->m_Miniport.m_GammaRamp[RampNo]), sizeof(D3DGAMMARAMP));
}

//------------------------------------------------------------------------------
// D3DDevice_CreateTexture
//
// 'CreateTexture', 'CMipMap::Create' and 'CMipMap::CMipMap' all rolled into
// one.

extern "C"
HRESULT WINAPI D3DDevice_CreateTexture(
    UINT Width,
    UINT Height,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    D3DTexture** ppTexture) 
{ 
    COUNT_API(API_D3DDEVICE_CREATETEXTURE);

    return CreateTexture(Width,
                         Height,
                         1,         // Depth
                         Levels, 
                         Usage,
                         Format,
                         false,     // isCubeMap
                         false,     // isVolumeTexture
                         (D3DBaseTexture **)ppTexture);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateVolumeTexture

extern "C"
HRESULT WINAPI D3DDevice_CreateVolumeTexture(
    UINT Width,
    UINT Height,
    UINT Depth,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    D3DVolumeTexture** ppVolumeTexture) 
{ 
    COUNT_API(API_D3DDEVICE_CREATEVOLUMETEXTURE);

    return CreateTexture(Width,
                         Height,
                         Depth,
                         Levels, 
                         Usage,
                         Format,
                         false,
                         true,
                         (D3DBaseTexture **)ppVolumeTexture);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateCubeTexture

extern "C"
HRESULT WINAPI D3DDevice_CreateCubeTexture(
    UINT EdgeLength,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    D3DCubeTexture** ppCubeTexture) 
{ 
    COUNT_API(API_D3DDEVICE_CREATECUBETEXTURE);

    return CreateTexture(EdgeLength,
                         EdgeLength,
                         1,
                         Levels, 
                         Usage,
                         Format,
                         true,
                         false,
                         (D3DBaseTexture **)ppCubeTexture);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateRenderTarget

extern "C"
HRESULT WINAPI D3DDevice_CreateRenderTarget(
    UINT Width, 
    UINT Height, 
    D3DFORMAT D3DFormat, 
    D3DMULTISAMPLE_TYPE MultiSample, 
    BOOL Lockable, 
    D3DSurface **ppSurface)
{ 
    COUNT_API(API_D3DDEVICE_CREATERENDERTARGET);

#if DBG
    if (!IsValidRenderTargetD3DFORMAT(D3DFormat))
    {
        DXGRIP("Invalid render target format");
    }
#endif

    return CreateStandAloneSurface(Width,
                                   Height,
                                   D3DFormat,
                                   ppSurface);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateDepthStencilSurface

extern "C"
HRESULT WINAPI D3DDevice_CreateDepthStencilSurface(
    UINT Width, 
    UINT Height, 
    D3DFORMAT D3DFormat, 
    D3DMULTISAMPLE_TYPE MultiSample, 
    D3DSurface **ppSurface)
{ 
    COUNT_API(API_D3DDEVICE_CREATEDEPTHSTENCILSURFACE);

#if DBG
    if (!IsValidDepthBufferD3DFORMAT(D3DFormat))
    {
        DXGRIP("Invalid render target format");
    }
#endif

    return CreateStandAloneSurface(Width,
                                   Height,
                                   D3DFormat,
                                   ppSurface);
}

//------------------------------------------------------------------------------
// D3DDevice_CreateImageSurface

extern "C"
HRESULT WINAPI D3DDevice_CreateImageSurface(
    UINT Width,
    UINT Height,
    D3DFORMAT D3DFormat,
    D3DSurface** ppSurface) 
{ 
    COUNT_API(API_D3DDEVICE_CREATEIMAGESURFACE);

    return CreateStandAloneSurface(Width,
                                   Height,
                                   D3DFormat,
                                   ppSurface);
}

//------------------------------------------------------------------------------
// NV057FormatFromD3DRectColorFormat (also works with NV044...)

inline DWORD NV057FormatFromD3DRectColorFormat(
    D3DCOPYRECTCOLORFORMAT ColorFormat)
{
    ASSERT(ColorFormat > D3DCOPYRECT_COLOR_FORMAT_Y8);

    if (ColorFormat >= D3DCOPYRECT_COLOR_FORMAT_X8R8G8B8_Z8R8G8B8)
        return NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;
    else if (ColorFormat >= D3DCOPYRECT_COLOR_FORMAT_R5G6B5)
        return NV057_SET_COLOR_FORMAT_LE_A16R5G6B5;

    return NV057_SET_COLOR_FORMAT_LE_X16A1R5G5B5;
}

#if DBG

//------------------------------------------------------------------------------
// ValidateCopyRectState

void ValidateCopyRectState(
    D3DCOPYRECTCOLORFORMAT ColorFormat,
    D3DCOPYRECTOPERATION Operation)
{
    switch(ColorFormat)
    {
    case D3DCOPYRECT_COLOR_FORMAT_DEFAULT:
        if (Operation != D3DCOPYRECT_SRCCOPY)
        {
            DXGRIP("D3DDevice_SetCopyRectsState: COLOR_FORMAT_DEFAULT only supports D3DCOPYRECT_SRCCOPY.");
        }
        break;

    case D3DCOPYRECT_COLOR_FORMAT_Y8:
    case D3DCOPYRECT_COLOR_FORMAT_Y16:
    case D3DCOPYRECT_COLOR_FORMAT_Y32:
        if (Operation != D3DCOPYRECT_SRCCOPY_AND &&
            Operation != D3DCOPYRECT_ROP_AND &&
            Operation != D3DCOPYRECT_SRCCOPY)
        {
            DXGRIP("D3DDevice_SetCopyRectsState: Invalid CopyRectOperation with COLOR_FORMAT_Y*.");
        }
        break;

    case D3DCOPYRECT_COLOR_FORMAT_X1A7R8G8B8_Z1A7R8G8B8:
    case D3DCOPYRECT_COLOR_FORMAT_X1A7R8G8B8_O1A7R8G8B8:
    case D3DCOPYRECT_COLOR_FORMAT_A8R8G8B8:
        if (Operation != D3DCOPYRECT_SRCCOPY &&
            Operation != D3DCOPYRECT_SRCCOPY_PREMULT)
        {
            DXGRIP("D3DDevice_SetCopyRectsState: Invalid CopyRectOperation with COLOR_FORMAT_A* or COLOR_FORMAT_X1A*.");
        }
        break;
    }
}

#endif

//------------------------------------------------------------------------------
// SetupCopyRectRopState

PPUSH
SetupCopyRectRopState(
    PPUSH pPush,
    D3DCOPYRECTCOLORFORMAT ColorFormat,
    CONST D3DCOPYRECTROPSTATE *pCopyRectRopState)
{
    CDevice* pDevice = g_pDevice;

    // Set ROP5

    Push1(pPush,
        SUBCH_RECTCOPYOPTIONS,
        NV043_SET_OBJECT,
        D3D_RECTANGLE_COPY_ROP);

    Push1(pPush + 2,
          SUBCH_RECTCOPY,
          NV05F_SET_CONTEXT_ROP,
          D3D_RECTANGLE_COPY_ROP);

    Push1(pPush + 4,
        SUBCH_RECTCOPYOPTIONS,
        NV043_SET_ROP5,
        pCopyRectRopState->Rop);

    pPush += 6;

    // Set Context Pattern

    Push1(pPush,
        SUBCH_RECTCOPYOPTIONS,
        NV044_SET_OBJECT,
        D3D_RECTANGLE_COPY_PATTERN);

    Push1(pPush + 2,
          SUBCH_RECTCOPY,
          NV05F_SET_CONTEXT_PATTERN,
          D3D_RECTANGLE_COPY_PATTERN);

    pPush += 4;

    //
    // Set shape, color format, earth's orbital spin, etc.
    //

    DWORD NV057ColorFormat = NV057FormatFromD3DRectColorFormat(ColorFormat);

    PushCount(pPush, SUBCH_RECTCOPYOPTIONS, NV044_SET_COLOR_FORMAT, 8);

    pPush[1] = NV057ColorFormat;                    // NV044_SET_COLOR_FORMAT
    pPush[2] = NV044_SET_MONOCHROME_FORMAT_CGA6_M1; // NV044_SET_MONOCHROME_FORMAT
    pPush[3] = pCopyRectRopState->Shape;            // NV044_SET_MONOCHROME_SHAPE
    pPush[4] = pCopyRectRopState->PatternSelect;    // NV044_SET_PATTERN_SELECT
    pPush[5] = pCopyRectRopState->MonoColor0;       // NV044_SET_MONOCHROME_COLOR0
    pPush[6] = pCopyRectRopState->MonoColor1;       // NV044_SET_MONOCHROME_COLOR
    pPush[7] = pCopyRectRopState->MonoPattern0;     // NV044_SET_MONOCHROME_PATTERN0
    pPush[8] = pCopyRectRopState->MonoPattern1;     // NV044_SET_MONOCHROME_PATTERN1

    pPush += 9;

    //
    // Set Color Pattern if needed
    //

    CONST DWORD *ColorPattern = pCopyRectRopState->ColorPattern;

    if (ColorPattern &&
        pCopyRectRopState->PatternSelect == NV044_SET_PATTERN_SELECT_COLOR)
    {
        DWORD Method;
        DWORD Count = 4 * 8;    // Assume 16-bit 4x8 DWORDS

        switch(NV057ColorFormat)
        {
        default:
            NODEFAULT("SetupCopyRectRopState - Invalid ColorFormat\n");

        case NV044_SET_COLOR_FORMAT_LE_A16R5G6B5:
            Method = NV044_SET_PATTERN_R5G6B5(0);
            break;

        case NV044_SET_COLOR_FORMAT_LE_X16A1R5G5B5:
            Method = NV044_SET_PATTERN_X1R5G5B5(0);
            break;

        case NV044_SET_COLOR_FORMAT_LE_A8R8G8B8:
            Method = NV044_SET_PATTERN_X8R8G8B8(0);
            Count = 8 * 8;
            break;
        }

        PushCount(pPush++, SUBCH_RECTCOPYOPTIONS, Method, Count);

        for(DWORD iPattern = 0; iPattern < Count; iPattern++)
        {
            *pPush++ = *ColorPattern++;
        }
    }

    return pPush;
}

//------------------------------------------------------------------------------
// D3DDevice_GetCopyRectsState

extern "C"
void WINAPI D3DDevice_GetCopyRectsState(
    D3DCOPYRECTSTATE *pCopyRectState,
    D3DCOPYRECTROPSTATE *pCopyRectRopState)
{
    CDevice* pDevice = g_pDevice;

    if (pCopyRectState)
    {
        *pCopyRectState = pDevice->m_CopyRectState;
    }

    if (pCopyRectRopState)
    {
        *pCopyRectRopState = pDevice->m_CopyRectRopState;

        // We are not keeping up to 256 bytes of crap around just
        // so some lazy programmer can call GetCopyRectsState and
        // get what they previously set. If you really want it
        // idle the chip and go look at NV_PGRAPH_PATTERN. :)
        pCopyRectRopState->ColorPattern = NULL;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetCopyRectsState

extern "C"
void WINAPI D3DDevice_SetCopyRectsState(
    CONST D3DCOPYRECTSTATE *pCopyRectState,
    CONST D3DCOPYRECTROPSTATE *pCopyRectRopState)
{
    CDevice* pDevice = g_pDevice;

    D3DCOPYRECTOPERATION Operation = pCopyRectState->Operation;
    D3DCOPYRECTCOLORFORMAT ColorFormat = pCopyRectState->ColorFormat;

#if DBG

    ValidateCopyRectState(ColorFormat, Operation);

#endif

    PPUSH pPush = pDevice->StartPush();

    // Save our state for those turkeys who call GetCopyRectsState.

    pDevice->m_CopyRectState = *pCopyRectState;

    Push1(pPush,
          SUBCH_RECTCOPY,
          NV05F_SET_OPERATION,
          Operation);
    pPush += 2;

    // Set up various state depending on the operation

    switch(Operation)
    {
    default:
        NODEFAULT("D3DDevice_SetCopyRectsState - Bad CopyRectOperation\n");

    case D3DCOPYRECT_SRCCOPY_AND:
    case D3DCOPYRECT_SRCCOPY:

        // Nothing to do. These aren't the droids you're looking for. Move along.

        break;

    case D3DCOPYRECT_ROP_AND:

        // Setup rop state.

        pPush = SetupCopyRectRopState(pPush, ColorFormat, pCopyRectRopState);
        pDevice->m_CopyRectRopState = *pCopyRectRopState;

        break;

    case D3DCOPYRECT_BLEND_AND:

        // Set beta1

        Push1(pPush,
            SUBCH_RECTCOPYOPTIONS,
            NV012_SET_OBJECT,
            D3D_RECTANGLE_COPY_BETA1);

        Push1(pPush + 2,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_BETA1,
              D3D_RECTANGLE_COPY_BETA1);

        Push1(pPush + 4,
            SUBCH_RECTCOPYOPTIONS,
            NV012_SET_BETA_1D31,
            pCopyRectState->BlendAlpha);

        pPush += 6;
        break;

    case D3DCOPYRECT_SRCCOPY_PREMULT:
    case D3DCOPYRECT_BLEND_PREMULT:

        // Set beta4

        Push1(pPush,
            SUBCH_RECTCOPYOPTIONS,
            NV072_SET_OBJECT,
            D3D_RECTANGLE_COPY_BETA4);

        Push1(pPush + 2,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_BETA4,
              D3D_RECTANGLE_COPY_BETA4);

        Push1(pPush + 4,
            SUBCH_RECTCOPYOPTIONS,
            NV072_SET_BETA_FACTOR,
            pCopyRectState->PremultAlpha);

        pPush += 6;
        break;
    }

    // Handle ColorKey.

    if (pCopyRectState->ColorKeyEnable)
    {
        if (DBG_CHECK(Operation == D3DCOPYRECT_BLEND_PREMULT))
        {
            DXGRIP("D3DDevice_SetCopyRectsState - ColorKey does not work with D3DCOPYRECT_BLEND_PREMULT.");
        }

        if (DBG_CHECK(ColorFormat <= D3DCOPYRECT_COLOR_FORMAT_Y8))
        {
            DXGRIP("D3DDevice_SetCopyRectsState - Invalid ColorKey ColorFormat.");
        }

        Push1(pPush,
            SUBCH_RECTCOPYOPTIONS,
            NV057_SET_OBJECT,
            D3D_RECTANGLE_COPY_COLOR_KEY);

        Push1(pPush + 2,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_COLOR_KEY,
              D3D_RECTANGLE_COPY_COLOR_KEY);

        Push2(pPush + 4,
            SUBCH_RECTCOPYOPTIONS,
            NV057_SET_COLOR_FORMAT,

            // NV057_SET_COLOR_FORMAT:
            NV057FormatFromD3DRectColorFormat(ColorFormat),

            // NV057_SET_COLOR:
            pCopyRectState->ColorKeyValue);

        pPush += 7;
    }
    else
    {
        Push1(pPush,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_COLOR_KEY,
              D3D_NULL_OBJECT);

        pPush += 2;
    }

    if (pCopyRectState->ClippingSize)
    {
        Push1(pPush,
            SUBCH_RECTCOPYOPTIONS,
            NV019_SET_OBJECT,
            D3D_RECTANGLE_COPY_CLIP_RECTANGLE);

        Push1(pPush + 2,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_CLIP_RECTANGLE,
              D3D_RECTANGLE_COPY_CLIP_RECTANGLE);

        Push2(pPush + 4,
            SUBCH_RECTCOPYOPTIONS,
            NV019_SET_POINT,

            // NV019_SET_POINT:
            pCopyRectState->ClippingPoint,

            // NV019_SET_SIZE:
            pCopyRectState->ClippingSize);

        pPush += 7;
    }
    else
    {
        Push1(pPush,
              SUBCH_RECTCOPY,
              NV05F_SET_CONTEXT_CLIP_RECTANGLE,
              D3D_NULL_OBJECT);

        pPush += 2;
    }

    pDevice->EndPush(pPush);
}

//----------------------------------------------------------------------------
// Helper function to copy rectangles around in video memory.

VOID GPUCopyVideoRectangle(
    DWORD Format,
    DWORD SourceOffset,
    DWORD SourcePitch,
    DWORD DestOffset,                         
    DWORD DestPitch,
    DWORD Count,
    RECT *pRect,
    POINT *pPoint)
{
    CDevice *pDevice = g_pDevice;

    ASSERT(!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER));
    ASSERT(Count <= 16);

    ASSERT(SourceOffset % 32 == 0);
    ASSERT(DestOffset % 32 == 0);
    ASSERT(SourcePitch % 64 == 0 && SourcePitch != 0 && SourcePitch <= 8128);
    ASSERT(DestPitch % 64 == 0 && DestPitch != 0 /*&& DestPitch <= 8128*/);

    PPUSH pPush;

    // Set up the surfaces.

    pPush = pDevice->StartPush();

    DWORD Pitch = DRF_NUM(062, _SET_PITCH, _SOURCE, SourcePitch)
                  | DRF_NUM(062, _SET_PITCH, _DESTIN, DestPitch);

    // NV062_SET_COLOR_FORMAT,
    // NV062_SET_PITCH,
    // NV062_SET_OFFSET_SOURCE and
    // NV062_SET_OFFSET_DESTIN

    Push4(pPush,
          SUBCH_RECTCOPYSURFACES,
          NV062_SET_COLOR_FORMAT,
          Format,
          Pitch,
          SourceOffset,
          DestOffset);

    pDevice->EndPush(pPush + 5);

    // Run the copies.

    while (Count--)
    {
        pPush = pDevice->StartPush();

        DWORD SourcePoint = DRF_NUM(05F, _CONTROL_POINT_IN, _X, pRect->left)
                            | DRF_NUM(05F, _CONTROL_POINT_IN, _Y, pRect->top);

        DWORD DestPoint = DRF_NUM(05F, _CONTROL_POINT_OUT, _X, pPoint->x)
                          | DRF_NUM(05F, _CONTROL_POINT_OUT, _Y, pPoint->y);

        DWORD Size = DRF_NUM(05F, _SIZE, _WIDTH, pRect->right - pRect->left)
                     | DRF_NUM(05F, _SIZE, _HEIGHT, pRect->bottom - pRect->top);

        // NV05F_CONTROL_POINT_IN,
        // NV05F_CONTROL_POINT_OUT,
        // NV05F_SIZE

        Push3(pPush,
              SUBCH_RECTCOPY,
              NV05F_CONTROL_POINT_IN,
              SourcePoint,
              DestPoint,
              Size);

        pDevice->EndPush(pPush + 4);

        pRect++;
        pPoint++;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_CopyRects

extern "C"
void WINAPI D3DDevice_CopyRects(
    D3DSurface* pSourceSurface,
    CONST RECT* pSourceRectsArray,
    UINT cRects,
    D3DSurface* pDestinationSurface,
    CONST POINT* pDestPointsArray) 
{ 
    COUNT_API(API_D3DDEVICE_COPYRECTS);

    const DWORD MaxRectangles = 16;

    RECT Rectangles[MaxRectangles];
    POINT Points[MaxRectangles];
    DWORD SwizzledColumns;
    DWORD SwizzledRows;

    DWORD BytesPerPixel = PixelJar::GetBitsPerPixel(pSourceSurface) / 8;
    DWORD SourcePitch = PixelJar::GetPitch(pSourceSurface);
    DWORD DestPitch = PixelJar::GetPitch(pDestinationSurface);

    BOOL IsSwizzledFormat      = PixelJar::IsSwizzled(pSourceSurface) 
                              || PixelJar::IsCompressed(pSourceSurface);

    CDevice* pDevice = g_pDevice;

    D3DCOPYRECTCOLORFORMAT CopyRectColorFormat = pDevice->m_CopyRectState.ColorFormat;

#if DBG

    DWORD SWidth, SHeight, SDepth, SPitch, SSlice;
    DWORD DWidth, DHeight, DDepth, DPitch, DSlice;

    PixelJar::GetSize(pSourceSurface, 0, &SWidth, &SHeight, &SDepth, &SPitch, &SSlice);
    PixelJar::GetSize(pDestinationSurface, 0, &DWidth, &DHeight, &DDepth, &DPitch, &DSlice);

    if (!IsSwizzledFormat && (SPitch < D3DTEXTURE_PITCH_MIN || DPitch < D3DTEXTURE_PITCH_MIN))
    {
        DXGRIP("D3DDevice_CopyRects - The source and destination surfaces must have a pitch of at least 64 bytes");
    }

#endif DBG

    if (DBG_CHECK(TRUE))
    {
        if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        {
            DPF_ERR("Can't call CopyRects while recording a push-buffer");
        }

        if (PixelJar::GetFormat(pSourceSurface) != PixelJar::GetFormat(pDestinationSurface))
        {
            DXGRIP("D3DDevice_CopyRects - The source and destination surfaces must have the same format.");
        }

        // The dest pitch doesn't seem have the same restrictions as the source pitch...
        if (!IsSwizzledFormat && (SourcePitch > 8128 /*|| DestPitch > 8128*/))
        {
            DXGRIP("D3DDevice_CopyRects - CopyRects does not work on surfaces who pitch is greater than 8128 bytes.");
        }

        if (PixelJar::IsCompressed(pSourceSurface) &&
            CopyRectColorFormat != D3DCOPYRECT_COLOR_FORMAT_DEFAULT)
        {
            DXGRIP("D3DDevice_CopyRects - Must use D3DCOPYRECT_COLOR_FORMAT_DEFAULT with compressed surfaces.");
        }
    }

    // Special case, passing cRects == 0 will copy the entire first
    // surface to the second.
    //
    if (cRects == 0)
    {
        cRects = 1;
    }

    // Swizzled and compressed textures need to be jiggled to get them to 
    // copy using these routines, especially because this class has a 
    // minimum pitch of 64 bytes.
    //
    if (IsSwizzledFormat)
    {

#if DBG

        // Note that all this stuff below is going to mess up ClippingRects
        // and Patterns for swizzled textures. Technically we could check
        // if the width of the swizzled texture > 64, set the pitch
        // appropriately and run with it. However I'm not sure supporting
        // all that with swizzled textures is even worth it. Clipping
        // Rects are just plain messed up cause swizzled bits are all twiddled
        // and possibly someone can take their 8x8 pattern brush, pre-swizzle
        // format it and they can do full surface-->surface copies with
        // strange rops but I don't see the win. So warn the user that things
        // may not work as expected.

        if ((pDevice->m_CopyRectState.ClippingSize ||
            (pDevice->m_CopyRectState.Operation == D3DCOPYRECT_ROP_AND)) &&
            !g_WarnAboutCopyRectStateSwizzle)
        {
            WARNING("D3DDevice_CopyRects - Swizzled surfaces don't support "
                "ClippingRects and/or D3DCOPYRECT_ROP_AND.");
            g_WarnAboutCopyRectStateSwizzle = TRUE;
        }

#endif

        DWORD Slice = PixelJar::GetSlice(pSourceSurface);
        
        // The max pitch is 8128.  Just do one row if it fits.
        if (Slice <= 8128)
        {
            SourcePitch = (Slice + 63) & ~63;
            DestPitch   = SourcePitch;

            SwizzledColumns = Slice;
            SwizzledRows = 1;
        }

        // Otherwise we need to break this up into chunks.  Fortunately
        // we do know that the memory is always a multiple of two 
        // power-of-two numbers and is greater than 8128.
        //
        else
        {
            ASSERT(Slice % 4096 == 0);

            SourcePitch = 4096;
            DestPitch   = 4096;

            SwizzledColumns = 4096;
            SwizzledRows = Slice / 4096;
        }

        if (CopyRectColorFormat == D3DCOPYRECT_COLOR_FORMAT_DEFAULT)
        {
            // If we are just SRCCOPY'ing things then take the bytesperpixel
            // and run with it. Technically I'm 99.9% sure you can do the
            // BytesPerPixel divide below but since I really didn't want to
            // introduce any regressions whilst adding the CopyRectState
            // stuff I'm going this route.

            BytesPerPixel = 1;
        }
        else
        {
            // Ok - we're doing some type of blend operation so we can't
            // twiddle their pixel size. Take the new byte width and
            // divide it by BytesPerPixel.

            // Make sure we're not lopping any data off here.
            ASSERT((SwizzledColumns / BytesPerPixel) * BytesPerPixel == SwizzledColumns);

            SwizzledColumns /= BytesPerPixel;
        }
    }

    // Set up our ColorFormat.

    DWORD ColorFormat;

    if (CopyRectColorFormat == D3DCOPYRECT_COLOR_FORMAT_DEFAULT)
    {
        ASSERT(pDevice->m_CopyRectState.Operation == D3DCOPYRECT_SRCCOPY);

        switch (BytesPerPixel)
        {
        default:
            NODEFAULT(("GPUCopyVideoRectangle - Bad pixel size\n"));

        case 4:
            ColorFormat = NV062_SET_COLOR_FORMAT_LE_A8R8G8B8;
            break;

        case 2:
            ColorFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;
            break;

        case 1:
            ColorFormat = NV062_SET_COLOR_FORMAT_LE_Y8;
            break;
        }
    }
    else
    {
        ColorFormat = CopyRectColorFormat;

#if DBG

        DWORD ColorFormatBytes = 1;

        if (ColorFormat >= D3DCOPYRECT_COLOR_FORMAT_X8R8G8B8_Z8R8G8B8)
            ColorFormatBytes = 4;
        else if (ColorFormat >= D3DCOPYRECT_COLOR_FORMAT_X1R5G5B5_Z1R5G5B5)
            ColorFormatBytes = 2;

        if (BytesPerPixel != ColorFormatBytes)
            DXGRIP("D3DDevice_CopyRects - pixel size doesn't match ColorFormat CopyRectState size.");

#endif

    }

    // For debug purposes, modify the context DMA to ensure we stay within 
    // bounds.
    DWORD destinationBase = pDevice->m_Miniport.SetDmaRange(
                                D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY,
                                pDestinationSurface);

    // Copy 16 rectangles at a time.
    do
    {
        RECT *pRectangles;
        POINT *pPoints;
        
        DWORD RectangleCount = min(MaxRectangles, cRects);
        cRects -= RectangleCount;

        // Figure out the arrays of rectangles to copy.
        if (pSourceRectsArray)
        {
            pRectangles = (RECT *)pSourceRectsArray;

            pSourceRectsArray += MaxRectangles;
        }
        else
        {
            pRectangles = Rectangles;

            for (DWORD i = 0; i < RectangleCount; i++)
            {
                Rectangles[i].top = 0;
                Rectangles[i].bottom = PixelJar::GetHeight(pSourceSurface);
                Rectangles[i].left = 0;
                Rectangles[i].right = PixelJar::GetWidth(pSourceSurface);
            }
        }

        if (pDestPointsArray)
        {
            pPoints = (POINT *)pDestPointsArray;

            pDestPointsArray += MaxRectangles;
        }
        else
        {
            pPoints = Points;

            for (DWORD i = 0; i < RectangleCount; i++)
            {
                Points[i].x = pRectangles[i].left;
                Points[i].y = pRectangles[i].top;
            }
        }

#if DBG
        // Make sure those rectangles are legal.
        if (DBG_CHECK(TRUE))
        {
            for (DWORD i = 0; i < RectangleCount; i++)
            {
                if (PixelJar::IsSwizzled(pSourceSurface) || PixelJar::IsCompressed(pSourceSurface))
                {
                    if (   pPoints[i].x != 0 
                        || pPoints[i].y != 0
                        || pRectangles[i].top != 0
                        || pRectangles[i].left != 0
                        || pRectangles[i].right != (long)SWidth
                        || pRectangles[i].bottom != (long)SHeight)
                    {
                        DXGRIP("D3DDevice_CopyRects - CopyRects can only copy all of a swizzled surface.");
                    }

                    if (SWidth != DWidth || SHeight != DHeight)
                    {
                        DXGRIP("D3DDevice_CopyRects - The source and destination surfaces must be the same size when copying a swizzled surface.");
                    }
                }
                else
                {
                    if (pRectangles[i].top >= pRectangles[i].bottom
                        || pRectangles[i].left >= pRectangles[i].right)
                    {
                        DXGRIP("D3DDevice_CopyRects - Malformed source rectangle, one dimension is either zero size or is inverted.");
                    }

                    if (pRectangles[i].top < 0 || pRectangles[i].bottom > (long)SHeight
                        || pRectangles[i].left < 0 || pRectangles[i].right > (long)SWidth)
                    {
                        DXGRIP("D3DDevice_CopyRects - The source rectangle extends beyond the bounds of the source surface.");
                    }

                    if (pPoints[i].x < 0 
                        || pPoints[i].x + pRectangles[i].right - pRectangles[i].left > (long)DWidth
                        || pPoints[i].y < 0 
                        || pPoints[i].y + pRectangles[i].bottom - pRectangles[i].top > (long)DHeight)
                    {
                        // If they've set up a Clipping Rect assume they aren't going out of bounds and if they
                        // are that AndrewGo's mondo cool Dma Range checking will catch it. And if it doesn't go
                        // yell at him...
                        if(!pDevice->m_CopyRectState.ClippingSize)
                        {
                            DXGRIP("D3DDevice_CopyRects - The destination rectangle extends beyond the bounds of the destination surface.");
                        }
                    }
                }           
            }
        }
#endif DBG

        // Adjust the rectangles to handle swizzled and compressed
        // textures.
        //
        if (IsSwizzledFormat)
        {
            for (DWORD i = 0; i < RectangleCount; i++)
            {
                pRectangles[i].left  = 0;
                pRectangles[i].top   = 0;
                pRectangles[i].right = SwizzledColumns;
                pRectangles[i].bottom = SwizzledRows;
            }
        }

        GPUCopyVideoRectangle(
            ColorFormat,
            pSourceSurface->Data,  
            SourcePitch,
            pDestinationSurface->Data - destinationBase,
            DestPitch,
            RectangleCount,
            pRectangles,
            pPoints);
    }
    while (cRects);

    pDevice->RecordSurfaceWritePush(pSourceSurface);
    pDevice->RecordSurfaceWritePush(pDestinationSurface);
}

//------------------------------------------------------------------------------
// D3DDevice_GetRenderTarget

extern "C"
HRESULT WINAPI D3DDevice_GetRenderTarget(
    D3DSurface** ppRenderTarget) 
{ 
    COUNT_API(API_D3DDEVICE_GETRENDERTARGET);

    CDevice *pDevice = g_pDevice;  

    *ppRenderTarget = pDevice->m_pRenderTarget;

    if (*ppRenderTarget)
    {
        (*ppRenderTarget)->AddRef();
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_GetDepthStencilSurface

extern "C"
HRESULT WINAPI D3DDevice_GetDepthStencilSurface(
    D3DSurface** ppZStencilSurface) 
{ 
    COUNT_API(API_D3DDEVICE_GETDEPTHSTENCILSURFACE);

    CDevice *pDevice = g_pDevice;  

    *ppZStencilSurface = pDevice->m_pZBuffer;

    if (*ppZStencilSurface)
    {
        (*ppZStencilSurface)->AddRef();
        return S_OK;
    }
    else
    {
        return D3DERR_NOTFOUND;
    }
}

//------------------------------------------------------------------------------
// g_TransformDirtyTable

D3DCONST DWORD g_TransformDirtyTable[] =
{
    D3DDIRTYFLAG_TRANSFORM | D3DDIRTYFLAG_LIGHTS,       // D3DTS_VIEW
    D3DDIRTYFLAG_TRANSFORM,                             // D3DTS_PROJECTION
    D3DDIRTYFLAG_TEXTURE_TRANSFORM,                     // D3DTS_TEXTURE0
    D3DDIRTYFLAG_TEXTURE_TRANSFORM,                     // D3DTS_TEXTURE1
    D3DDIRTYFLAG_TEXTURE_TRANSFORM,                     // D3DTS_TEXTURE2
    D3DDIRTYFLAG_TEXTURE_TRANSFORM,                     // D3DTS_TEXTURE3
    D3DDIRTYFLAG_TRANSFORM,                             // D3DTS_WORLD
    D3DDIRTYFLAG_TRANSFORM,                             // D3DTS_WORLD1
    D3DDIRTYFLAG_TRANSFORM,                             // D3DTS_WORLD2
    D3DDIRTYFLAG_TRANSFORM,                             // D3DTS_WORLD3    
};

//------------------------------------------------------------------------------
// D3DDevice_SetTransform

extern "C"
void WINAPI D3DDevice_SetTransform(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX* pMatrix) 
{ 
    COUNT_API(API_D3DDEVICE_SETTRANSFORM);

    DWORD state = (DWORD) State;

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (state >= D3DTS_MAX)
        {
            D3D_ERR("Invalid state value passed to SetTransform");
        }
        if (pMatrix == NULL)
        {
            D3D_ERR("Invalid pointer");
        }
        if (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_MODELVIEW)
        {
            WARNING("SetTransform called while Set[VertexBlend]ModelView in effect,\n"
                    "will be deferred");
        }
    }

#if DBG

    if ((state >= D3DTS_WORLD) && (state <= D3DTS_WORLD3))
    {
        COUNT_PERF(PERF_SETTRANSFORM_WORLD);
    }
    else if (state == D3DTS_VIEW)
    {
        COUNT_PERF(PERF_SETTRANSFORM_VIEW);
    }
    else if (state == D3DTS_PROJECTION)
    {
        COUNT_PERF(PERF_SETTRANSFORM_PROJECTION);
    }
    else if ((state >= D3DTS_TEXTURE0) && (state <= D3DTS_TEXTURE3))
    {
        COUNT_PERF(PERF_SETTRANSFORM_TEXTURE);
    }

    if (memcmp(pMatrix, &pDevice->m_Transform[state], sizeof(*pMatrix)) == 0)
    {
        COUNT_PERF(PERF_REDUNDANT_SETTRANSFORM);
    }

#endif

    pDevice->m_Transform[state] = *pMatrix;
    D3D__DirtyFlags |= g_TransformDirtyTable[state];

    // We do a bunch more work here for the infrequently changed projection
    // matrix:

    if (State == D3DTS_PROJECTION)
    {
        // Logic taken from UpdateWInfo:

        if ((pMatrix->_33 == pMatrix->_34) || (pMatrix->_33 == 0.0f))
        {
            WARNING("Cannot compute WNear and WFar from the supplied "
                     "projection matrix");
            WARNING("Setting wNear to 0.0 and wFar to 1.0");

            pDevice->m_WNear = 0.0f;
            pDevice->m_WFar = 1.0f;
        }
        else
        {
            pDevice->m_WNear = pMatrix->_44 - pMatrix->_43 
                / pMatrix->_33 * pMatrix->_34;

            pDevice->m_WFar = (pMatrix->_44 - pMatrix->_43) 
                / (pMatrix->_33 - pMatrix->_34)*pMatrix->_34 + pMatrix->_44;
        }

        pDevice->m_InverseWFar = 1.0f / pDevice->m_WFar;

        // Determine if the fog source is Z or W.  Logic taken from 
        // nvDP2SetTransform:

        pDevice->m_StateFlags &= ~STATE_FOGSOURCEZ;
        if ((pMatrix->_14 == 0.0f) &&
            (pMatrix->_24 == 0.0f) &&
            (pMatrix->_34 == 0.0f) &&
            (pMatrix->_44 == 1.0f))
        {
            pDevice->m_StateFlags |= STATE_FOGSOURCEZ;
        }

        // Update our cached projection-viewport transform:

        UpdateProjectionViewportTransform();

        // If the fog source has changed and passthrough is active, we need
        // to update the current passthrough program here:

        CommonSetPassthroughProgram(pDevice);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetTransform

extern "C"
void WINAPI D3DDevice_GetTransform(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX* pMatrix) 
{ 
    COUNT_API(API_D3DDEVICE_GETTRANSFORM);

    DWORD state = (DWORD) State;

    if (DBG_CHECK(TRUE))
    {
        if (state >= D3DTS_MAX)
        {
            D3D_ERR("Invalid state value passed to GetTransform");
        }

        if (pMatrix == NULL)
        {
            D3D_ERR("Invalid pointer");
        }
    }

    CDevice *pDevice = g_pDevice;

    *pMatrix = pDevice->m_Transform[state];
}

//------------------------------------------------------------------------------
// D3DDevice_MultiplyTransform

extern "C"
void WINAPI D3DDevice_MultiplyTransform(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX* pMatrix) 
{ 
    COUNT_API(API_D3DDEVICE_MULTIPLYTRANSFORM);

    D3DALIGN D3DMATRIX result;
    D3DALIGN D3DMATRIX temp = *pMatrix;

    DWORD state = (DWORD) State;

    if (DBG_CHECK(TRUE))
    {
        if (state >= D3DTS_MAX)
        {
            D3D_ERR("Invalid state value passed to MultiplyTransform");
        }

        if (pMatrix == NULL)
        {
            D3D_ERR("Invalid pointer");
        }
    }

    CDevice* pDevice = g_pDevice;

    MatrixProduct4x4(&result, &temp, &pDevice->m_Transform[state]);

    D3DDevice_SetTransform(State, &result);
}

//------------------------------------------------------------------------------
// D3DDevice_SetViewport

extern "C"
void WINAPI D3DDevice_SetViewport(
    CONST D3DVIEWPORT8* pViewport) 
{ 
    D3DSurface* pSurface;

    COUNT_API(API_D3DDEVICE_SETVIEWPORT);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pViewport == NULL)
        {
            D3D_ERR( "Invalid viewport pointer" );
        }
    }

    // Clip to the current render target.  For antialiasing, we always
    // use the size of the post-filter buffer:

    if (pDevice->m_pRenderTarget == pDevice->m_pFrameBuffer[0])
        pSurface = pDevice->m_pFrameBuffer[1];
    else
        pSurface = pDevice->m_pRenderTarget;

    DWORD left = max(pViewport->X, 0);
    DWORD top = max(pViewport->Y, 0);
    DWORD right = min(pViewport->X + pViewport->Width, 
                      (INT) PixelJar::GetWidth(pSurface));
    DWORD bottom = min(pViewport->Y + pViewport->Height,
                       (INT) PixelJar::GetHeight(pSurface));

    if (DBG_CHECK(TRUE))
    {
        if ((left >= right) || (top >= bottom) ||
            (right > 0xfff) || (bottom > 0xfff))
        {
            D3D_ERR("Invalid effective clip dimensions");
        }
    }

    // Stash away the result:

    pDevice->m_Viewport.X = left;
    pDevice->m_Viewport.Y = top;
    pDevice->m_Viewport.Width = right - left;
    pDevice->m_Viewport.Height = bottom - top;
    pDevice->m_Viewport.MinZ = pViewport->MinZ;
    pDevice->m_Viewport.MaxZ = pViewport->MaxZ;

    // Update the cached projection-viewport transform, and dirty the
    // transforms:

    UpdateProjectionViewportTransform();

    // Set the window clipping.

    D3DDevice_SetScissors(0, 0, NULL);

    // Set the viewport scaling.

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetViewport(pDevice, pPush);

    pDevice->EndPush(pPush);

    // Point parameters are dependent upon the viewport height:

    D3D__DirtyFlags |= D3DDIRTYFLAG_POINTPARAMS;
}

//------------------------------------------------------------------------------
// D3DDevice_GetViewport

extern "C"
void WINAPI D3DDevice_GetViewport(
    D3DVIEWPORT8* pViewport) 
{ 
    COUNT_API(API_D3DDEVICE_GETVIEWPORT);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pViewport == NULL)
        {
            D3D_ERR( "Invalid viewport pointer" );
        }
    }

    *pViewport = pDevice->m_Viewport;
}

//------------------------------------------------------------------------------
// D3DDevice_SetMaterial

extern "C"
void WINAPI D3DDevice_SetMaterial(
    CONST D3DMATERIAL8* pMaterial) 
{ 
    COUNT_API(API_D3DDEVICE_SETMATERIAL);

    if (DBG_CHECK(TRUE))
    {
        if (pMaterial == NULL)
        {
            D3D_ERR("Null material pointer");
        }
    }

    CDevice* pDevice = g_pDevice;

    pDevice->m_Material = *pMaterial;

    D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;
}

//------------------------------------------------------------------------------
// D3DDevice_GetMaterial

extern "C"
void WINAPI D3DDevice_GetMaterial(
    D3DMATERIAL8* pMaterial) 
{ 
    COUNT_API(API_D3DDEVICE_GETMATERIAL);

    if (DBG_CHECK(TRUE))
    {
        if (pMaterial == NULL)
        {
            D3D_ERR("Null material pointer");
        }
    }

    CDevice* pDevice = g_pDevice;

    *pMaterial = pDevice->m_Material;
}

//------------------------------------------------------------------------------
// D3DDevice_SetBackMaterial

extern "C"
void WINAPI D3DDevice_SetBackMaterial(
    CONST D3DMATERIAL8* pBackMaterial) 
{ 
    COUNT_API(API_D3DDEVICE_SETBACKMATERIAL);

    if (DBG_CHECK(TRUE))
    {
        if (pBackMaterial == NULL)
        {
            D3D_ERR("Null material pointer");
        }
    }

    CDevice* pDevice = g_pDevice;

    pDevice->m_BackMaterial = *pBackMaterial;

    D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;
}

//------------------------------------------------------------------------------
// D3DDevice_GetBackMaterial

extern "C"
void WINAPI D3DDevice_GetBackMaterial(
    D3DMATERIAL8* pBackMaterial) 
{ 
    COUNT_API(API_D3DDEVICE_GETBACKMATERIAL);

    if (DBG_CHECK(TRUE))
    {
        if (pBackMaterial == NULL)
        {
            D3D_ERR("Null material pointer");
        }
    }

    CDevice* pDevice = g_pDevice;

    *pBackMaterial = pDevice->m_BackMaterial;
}

//------------------------------------------------------------------------------
// CheckLightParameters

void CheckLightParameters(
    CONST D3DLIGHT8* lpData)
{
    if (lpData->Type != D3DLIGHT_POINT &&
        lpData->Type != D3DLIGHT_SPOT &&
        lpData->Type != D3DLIGHT_DIRECTIONAL)
    {
        D3D_ERR( "Invalid D3DLIGHT type" );
    }

    if (lpData->Type != D3DLIGHT_DIRECTIONAL)
    {
        if (lpData->Range < 0.0f)
        {
            D3D_ERR( "Invalid D3DLIGHT range" );
        }
        if (lpData->Attenuation0 == 0 &&
            lpData->Attenuation1 == 0 &&
            lpData->Attenuation2 == 0)
        {
            D3D_ERR( "Attenuation factors can not all be zero" );
        }
    }

    if (lpData->Type == D3DLIGHT_SPOT || lpData->Type == D3DLIGHT_DIRECTIONAL)
    {
        float   magnitude;
        magnitude = lpData->Direction.x * lpData->Direction.x +
            lpData->Direction.y * lpData->Direction.y +
            lpData->Direction.z * lpData->Direction.z;
        if (magnitude < 0.00001f)
        {
            D3D_ERR( "Invalid D3DLIGHT direction" );
        }
        if (lpData->Type == D3DLIGHT_SPOT)
        {
            if (lpData->Phi < 0.0f)
            {
                D3D_ERR( "Invalid D3DLIGHT Phi angle, must be >= 0" );
            }
            if (lpData->Phi > 3.1415927f)
            {
                D3D_ERR( "Invalid D3DLIGHT Phi angle, must be <= pi" );
            }
            if (lpData->Theta < 0.0f)
            {
                D3D_ERR( "Invalid D3DLIGHT Theta angle, must be >= 0" );
            }
            if (lpData->Theta > lpData->Phi)
            {
                D3D_ERR( "Invalid D3DLIGHT Theta angle, must be <= Phi" );
            }
            if (lpData->Attenuation0 < 0 ||
                lpData->Attenuation1 < 0 ||
                lpData->Attenuation2 < 0)
            {
                D3D_ERR( "Attenuation factor can not be negative" );
            }
        }
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetLight

extern "C"
HRESULT WINAPI D3DDevice_SetLight(
    DWORD Index,
    CONST D3DLIGHT8* pLight8) 
{ 
    COUNT_API(API_D3DDEVICE_SETLIGHT);

    if (DBG_CHECK(TRUE))
    {
        CheckLightParameters(pLight8);

        if (pLight8 == NULL)
        {
            D3D_ERR("Invalid light pointer");
        }

        // '4096' is a completely arbitrary limit:

        if (Index > 4095)
        {
            D3D_ERR("Light index is more than 4095");
        }
    }

    CDevice* pDevice = g_pDevice;

    // Grow the lights array if necessary:

    DWORD count = pDevice->m_LightCount;
    if (Index >= count)
    {
        // Always round up to a multiple of 16:

        DWORD newCount = (Index + 16) & ~0xf;
        Light* pLights = (Light*) MemAllocNoZero(newCount * sizeof(Light));

        if (pLights == NULL)
        {
            return E_OUTOFMEMORY;
        }

        // Before the copy, run through the active lights linked-list
        // and add the offset to every pointer, thereby updating all
        // the 'Next' pointers:

        DWORD delta = (DWORD) pLights - (DWORD) pDevice->m_pLights;
        Light** ppNext = &pDevice->m_pActiveLights;
        Light** ppNextNext;

        while (*ppNext != NULL)
        {
            ppNextNext = &((*ppNext)->pNext);
            *((DWORD*) ppNext) += delta;
            ppNext = ppNextNext;
        }

        // Copy:

        memcpy(pLights, pDevice->m_pLights, count * sizeof(Light));
        ZeroMemory(pLights + count, (newCount - count) * sizeof(Light));

        // Free the old array and remember the new values:

        MemFree(pDevice->m_pLights);

        pDevice->m_LightCount = newCount;
        pDevice->m_pLights = pLights;
    }

    Light* pLight = &pDevice->m_pLights[Index];

    pLight->Light8 = *pLight8;
    pLight->Flags |= (LIGHT_SET | LIGHT_STATEBLOCK_SET);

    if (pLight->Light8.Type != D3DLIGHT_POINT)
    {
        // Negate direction vector to align with OpenGL:

        ReverseVector3(&pLight->Direction, &pLight->Light8.Direction);
        NormalizeVector3(&pLight->Direction);  // PERF: Necessary??

        if (pLight->Light8.Type == D3DLIGHT_SPOT)
        {
            // Calculate falloff and direction:
    
            Explut(pLight->Light8.Falloff, &pLight->Falloff_L, &pLight->Falloff_M);
    
            pLight->Falloff_N = 1.0f + pLight->Falloff_L - pLight->Falloff_M;
                
            // Attenuate the spot direction to get falloff to work:
    
            FLOAT theta2 = Cos(0.5f * pLight->Light8.Theta);
            FLOAT phi2 = Cos(0.5f * pLight->Light8.Phi);
    
            // Handle case in which theta gets close to or overtakes phi, since 
            // hardware can't:
    
            if (phi2 >= theta2)     // Outer angle <= inner angle, oops
            {        
                // Make outer angle cosine slightly smaller:
    
                phi2 = 0.999f * theta2;  
            }
    
            pLight->Scale = nvInv(theta2 - phi2);
            pLight->W = -phi2 * pLight->Scale;
        }
    }

    D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_GetLight

extern "C"
void WINAPI D3DDevice_GetLight(
    DWORD Index,
    D3DLIGHT8* pLight8) 
{ 
    COUNT_API(API_D3DDEVICE_GETLIGHT);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pLight8 == NULL)
        {
            D3D_ERR("Invalid light pointer");
        }

        // It seems a bit inconsistent that 'LightEnable' can automatically 
        // create a new light, but GetLight can't.  Oh well.

        if ((Index >= pDevice->m_LightCount) ||
            !(pDevice->m_pLights[Index].Flags & LIGHT_SET))
        {
            D3D_ERR("Invalid light index");
        }
    }

    *pLight8 = pDevice->m_pLights[Index].Light8;
}

//------------------------------------------------------------------------------
// D3DDevice_LightEnable
//
// DOC: Behavior when more than 8 lights enabled

extern "C"
HRESULT WINAPI D3DDevice_LightEnable(
    DWORD Index,
    BOOL Enable) 
{ 
    COUNT_API(API_D3DDEVICE_LIGHTENABLE);

    CDevice* pDevice = g_pDevice;

    // If the light hasn't already been created, we create a default light:

    if ((Index >= pDevice->m_LightCount) ||
        !(pDevice->m_pLights[Index].Flags & LIGHT_SET))
    {
        // Set the light's default value:

        D3DLIGHT8 light;
        ZeroMemory(&light, sizeof(light));

        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Direction.z = 1.0f;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;

        // Ugh, even on retail we do a run-time check for failure here, to
        // account for allocation failure:

        HRESULT res = D3DDevice_SetLight(Index, &light);
        if (res != S_OK)
            return res;
    }

    Light* pLight = &pDevice->m_pLights[Index];

    // Make a note that this light has been enabled or disabled, for the
    // purposes of our state-block support:

    pLight->Flags |= LIGHT_STATEBLOCK_ENABLED;

    // First remove the light from the active list, if it's there.
    // Initially, &pPrevious->pNext == &pDevice->m_pActiveLights:

    Light* pPrevious = (Light*) ((BYTE*) &pDevice->m_pActiveLights 
                                            - offsetof(Light, pNext));
    Light* pThis = pDevice->m_pActiveLights;

    while (pThis != NULL)
    {
        if (pThis == pLight)
        {
            pPrevious->pNext = pThis->pNext;
            break;          // ======>
        }

        // Advance:

        pPrevious = pThis;
        pThis = pThis->pNext;
    }

    // If enabling, insert the light at the head of the list:

    if (Enable)
    {
        pLight->pNext = pDevice->m_pActiveLights;
        pDevice->m_pActiveLights = pLight;
    }

    D3D__DirtyFlags |= D3DDIRTYFLAG_LIGHTS;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_GetLightEnable

extern "C"
void WINAPI D3DDevice_GetLightEnable(
    DWORD Index,
    BOOL* pEnable) 
{ 
    COUNT_API(API_D3DDEVICE_GETLIGHTENABLE);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (pEnable == NULL)
        {
            D3D_ERR("Invalid enable pointer");
        }

        // It seems a bit inconsistent that 'LightEnable' can automatically 
        // create a new light, but GetLight can't.  Oh well.

        if ((Index >= pDevice->m_LightCount) ||
            !(pDevice->m_pLights[Index].Flags & LIGHT_SET))
        {
            D3D_ERR("Invalid light index");
        }
    }

    // Since we *have* to keep an active list, I can't be bothered to
    // keep an 'active' flag in the light structure.  GetLightEnable will 
    // be rare, so simply loop through the active list to determine if
    // the light is enabled:

    Light* pLight = &pDevice->m_pLights[Index];
    Light* pActive = pDevice->m_pActiveLights;

    while (pActive != NULL)
    {
        if (pActive == pLight)
        {
            *pEnable = TRUE;
            return;
        }

        pActive = pActive->pNext;
    }

    *pEnable = FALSE;
}

//------------------------------------------------------------------------------
// D3DDevice_GetRenderState

extern "C"
void WINAPI D3DDevice_GetRenderState(
    D3DRENDERSTATETYPE State,
    DWORD* pValue) 
{ 
    COUNT_API(API_D3DDEVICE_GETRENDERSTATE);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (State >= D3DRS_MAX || State == 0)
        {
            D3D_ERR("Invalid render state type");
        }
    }

    *pValue = D3D__RenderState[State];
}

//------------------------------------------------------------------------------
// D3DDevice_GetTexture

extern "C"
void WINAPI D3DDevice_GetTexture(
    DWORD Stage,
    D3DBaseTexture** ppTexture) 
{ 
    COUNT_API(API_D3DDEVICE_GETTEXTURE);

    CDevice *pDevice = g_pDevice;
    D3DBaseTexture **Textures = pDevice->m_Textures;

    if (DBG_CHECK(Stage >= D3DTSS_MAXSTAGES))
    {
        D3D_ERR("D3DDevice_GetTexture - Bad texture stage.");
    }

    if (Textures[Stage])
    {
        Textures[Stage]->AddRef();
    }

    *ppTexture = Textures[Stage];
}

//------------------------------------------------------------------------------
// D3DDevice_SetTexture

extern "C"
void WINAPI D3DDevice_SetTexture(
    DWORD Stage,
    D3DBaseTexture* pTexture) 
{ 
    PPUSH pPush;

    COUNT_API(API_D3DDEVICE_SETTEXTURE);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(Stage >= D3DTSS_MAXSTAGES))
    {
        D3D_ERR("D3DDevice_SetTexture - Bad texture stage.");
    }

    if (DBG_CHECK(pTexture && IsChildSurface(pTexture)))
    {
        D3D_ERR("D3DDevice_SetTexture - a non-top-level surface was cast to a texture");
    }

    if (pTexture == pDevice->m_Textures[Stage])
    {
        COUNT_PERF(PERF_REDUNDANT_SETTEXTURE);
    }

    D3DBaseTexture* pPrevious = pDevice->m_Textures[Stage];

    if (pPrevious)
    {
        pDevice->RecordResourceReadPush(pPrevious);

        InternalRelease(pPrevious);
    }

    pDevice->m_Textures[Stage] = pTexture;

    if (!pTexture)
    {
        pPush = pDevice->StartPush();

        // Disable the texture unit:

        Push1(pPush, NV097_SET_TEXTURE_CONTROL0(Stage), 0);

        pDevice->EndPush(pPush + 2);

        // Set it up so that the new SetTexture will re-enable the shader
        // program again:

        pDevice->m_TextureCubemapAndDimension[Stage] = ~0;

        // The combiner logic needs to know whether a texture is present or
        // not.  We also need to update the shader program:

        D3D__DirtyFlags |= (D3DDIRTYFLAG_COMBINERS | 
                            D3DDIRTYFLAG_SHADER_STAGE_PROGRAM);
    }
    else
    {
        // Reference the new texture:

        InternalAddRef(pTexture);
    
        pPush = pDevice->StartPush();

        // Tell the hardware where and what format:
    
        Push2(pPush, 
              NV097_SET_TEXTURE_OFFSET(Stage), 

              // NV097_SET_TEXTURE_OFFSET:

              pTexture->Data,

              // NV097_SET_TEXTURE_FORMAT:

              pTexture->Format);

        pPush += 3;

        if (pTexture->Size != 0)
        {
            DWORD width = PixelJar::GetLinearWidth(pTexture);
            DWORD height = PixelJar::GetLinearHeight(pTexture);
            DWORD pitch = PixelJar::GetLinearPitch(pTexture);

            // It's an image format, so we have to set the dimension and
            // stride:

            Push1(pPush, 
                  NV097_SET_TEXTURE_CONTROL1(Stage), 
                  DRF_NUMFAST(097, _SET_TEXTURE_CONTROL1, _IMAGE_PITCH, pitch));

            Push1(pPush + 2, 
                  NV097_SET_TEXTURE_IMAGE_RECT(Stage), 
                  DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, width)
                  | DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, height));

            pPush += 4;
        }

        // If the cubemap-ness or dimensionality of the new texture is
        // different from that of the old, we have to reprogram the
        // shader stage program:

        DWORD cubenessAndDimensionality = pTexture->Format 
            & (DRF_NUM(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, ~0) | 
               DRF_NUM(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, ~0));

        if (pDevice->m_TextureCubemapAndDimension[Stage] 
                != cubenessAndDimensionality)
        {
            pDevice->m_TextureCubemapAndDimension[Stage] 
                = cubenessAndDimensionality;

            // Recompute the shader program because of the new texture type:

            D3D__DirtyFlags |= D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;

            if (pPrevious == NULL)
            {
                // Enable the texture unit, since we got here because the 
                // previously set texture was NULL:

                Push1(pPush, 
                      NV097_SET_TEXTURE_CONTROL0(Stage), 
                      pDevice->m_TextureControl0Enabled[Stage]);

                pPush += 2;

                // The combiner logic needs to know whether a texture is 
                // present or not.

                D3D__DirtyFlags |= D3DDIRTYFLAG_COMBINERS;
            }
        }
        else
        {
            // This is the most common fall-through case, where this texture's
            // cubemap-ness and dimensionality is exactly the same as the old.
            // We should never hit this fall-through if the texture unit was
            // previously disabled, though:

            ASSERT(pPrevious != NULL);
        }

        pDevice->EndPush(pPush);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SwitchTexture[_Fast]

extern "C"
#if DBG
_declspec(naked) void D3DFASTCALL D3DDevice_SwitchTexture_Fast(
#else
_declspec(naked) void D3DFASTCALL D3DDevice_SwitchTexture(
#endif
    DWORD Method, // Already encoded with two-dword count
    DWORD Data,
    DWORD Format)
{ 
    COUNT_API(API_D3DDEVICE_SWITCHTEXTURE);

    _asm
    {
        ; ecx = Method 
        ; edx = Data 
        ; [esp+4] = Format 

    Switch_Start:
        mov     eax, g_Device.m_Pusher.m_pPut
        add     eax, 12
        cmp     eax, g_Device.m_Pusher.m_pThreshold
        jae     Switch_MakeSpace
        mov     [g_Device.m_Pusher.m_pPut], eax
        mov     [eax-12], ecx
        mov     ecx, [esp+4]
        mov     [eax-8], edx
        mov     [eax-4], ecx
        ret     4

    Switch_MakeSpace:
        push    edx
        push    ecx
        call    MakeSpace
        pop     ecx
        pop     edx
        jmp     Switch_Start
    }
}

#if DBG

//------------------------------------------------------------------------------
// D3DDevice_SwitchTexture (Debug only)

extern "C"
void D3DFASTCALL D3DDevice_SwitchTexture(
    DWORD Method,
    DWORD Data,
    DWORD Format)
{
    COUNT_API(API_D3DDEVICE_SWITCHTEXTURE);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        DWORD Stage = (Method - 
                PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(0), 2)) / 0x40;

        if ((Stage >= 3) ||
            (Method != 
             PUSHER_METHOD(SUBCH_3D, NV097_SET_TEXTURE_OFFSET(Stage), 2)))
        {
            DPF_ERR("Invalid texture unit encoding");
        }
        if (pDevice->m_Textures[Stage] == NULL)
        {
            DPF_ERR("Can't call SwitchTexture when previous SetTexture "
                    "for this stage was NULL");
        }
        if (IsLinearD3DFORMAT(DecodeD3DFORMAT(Format)))
        {
            DPF_ERR("Can't call SwitchTexture with a linear texture");
        }
        DWORD cubenessAndDimensionality = Format 
            & (DRF_NUM(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, ~0) | 
               DRF_NUM(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, ~0));

        if (pDevice->m_TextureCubemapAndDimension[Stage] 
                != cubenessAndDimensionality)
        {
            DPF_ERR("Cubemap-ness and dimensionality must be same as previous "
                    "texture");
        }
    }

    D3DDevice_SwitchTexture_Fast(Method, Data, Format);
}

#endif DBG

//------------------------------------------------------------------------------
// D3DDevice_GetPalette

extern "C"
void WINAPI D3DDevice_GetPalette(
    DWORD Stage,
    D3DPalette** ppPalette) 
{ 
    COUNT_API(API_D3DDEVICE_GETPALETTE);

    CDevice *pDevice = g_pDevice;
    D3DPalette **Palettes = pDevice->m_Palettes;

    if (DBG_CHECK(Stage >= D3DTSS_MAXSTAGES))
    {
        D3D_ERR("D3DDevice_GetPalette - Bad texture stage.");
    }

    if (Palettes[Stage])
    {
        Palettes[Stage]->AddRef();
    }

    *ppPalette = Palettes[Stage];
}

//------------------------------------------------------------------------------
// D3DDevice_SetPalette

extern "C"
void WINAPI D3DDevice_SetPalette(
    DWORD Stage, 
    D3DPalette *pPalette)
{
    COUNT_API(API_D3DDEVICE_SETPALETTE);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(Stage >= D3DTSS_MAXSTAGES))
    {
        D3D_ERR("Bad texture stage.");
    }

    if (pPalette == pDevice->m_Palettes[Stage])
    {
        COUNT_PERF(PERF_REDUNDANT_SETPALETTE);
    }

    if (pDevice->m_Palettes[Stage])
    {
        pDevice->RecordResourceReadPush(pDevice->m_Palettes[Stage]);

        InternalRelease(pDevice->m_Palettes[Stage]);
    }

    pDevice->m_Palettes[Stage] = pPalette;

    if (pPalette)
    {
        InternalAddRef(pPalette);

        DWORD contextAndLength 
            = (pPalette->Common >> D3DPALETTE_COMMON_PALETTESET_SHIFT) 
            & D3DPALETTE_COMMON_PALETTESET_MASK;

        ASSERT((contextAndLength & 2) == 0);
        ASSERT(contextAndLength <= 
               (DRF_DEF(097, _SET_TEXTURE_PALETTE, _LENGTH, _32)
              | DRF_DEF(097, _SET_TEXTURE_PALETTE, _CONTEXT_DMA, _B)));
        ASSERT((pPalette->Data & 63) == 0);

        PPUSH pPush = pDevice->StartPush();

        Push1(pPush, 
              NV097_SET_TEXTURE_PALETTE(Stage), 
              pPalette->Data | contextAndLength);

        pDevice->EndPush(pPush + 2);
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetTextureStageState

extern "C"
void WINAPI D3DDevice_GetTextureStageState(
    DWORD Stage,
    D3DTEXTURESTAGESTATETYPE Type,
    DWORD* pValue) 
{ 
    COUNT_API(API_D3DDEVICE_GETTEXTURESTAGESTATE);

    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (Stage >= D3DTSS_MAXSTAGES 
            || Type == 0 
            || Type >= D3DTSS_MAX)
        {
            D3D_ERR( "Invalid texture stage or state index" );
        }
    }

    *pValue = D3D__TextureState[Stage][Type];
}

//------------------------------------------------------------------------------
// D3DDevice_SetIndices

extern "C"
void WINAPI D3DDevice_SetIndices(
    D3DIndexBuffer* pIndexBuffer,
    UINT BaseVertexIndex) 
{ 
    COUNT_API(API_D3DDEVICE_SETINDICES);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK((pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER) &&
                  (BaseVertexIndex != 0)))
    {
        D3D_ERR("Can't set non-zero BaseVertexIndex while recording a "
                "push-buffer");
    }
    
    // Reference the new, release the old, remembering that either may be
    // NULL:

    if (pIndexBuffer)
    {
        InternalAddRef(pIndexBuffer);

        D3D__IndexData = (WORD*) pIndexBuffer->Data;
    }
    else
    {
        D3D__IndexData = NULL;
    }

    if (pDevice->m_pIndexBuffer)
    {
        InternalRelease(pDevice->m_pIndexBuffer);
    }

    pDevice->m_pIndexBuffer = pIndexBuffer;
    pDevice->m_IndexBase = BaseVertexIndex;
}

//------------------------------------------------------------------------------
// D3DDevice_GetIndices

extern "C"
void WINAPI D3DDevice_GetIndices(
    D3DIndexBuffer** ppIndexData,
    UINT* pBaseVertexIndex) 
{ 
    COUNT_API(API_D3DDEVICE_GETINDICES);

    CDevice* pDevice = g_pDevice;

    D3DIndexBuffer* pIndexBuffer = pDevice->m_pIndexBuffer;
    if (pIndexBuffer)
    {
        pIndexBuffer->AddRef();

        *ppIndexData = pIndexBuffer;
        *pBaseVertexIndex = pDevice->m_IndexBase;
    }
    else
    {
        *ppIndexData = 0;
        *pBaseVertexIndex = 0;
    }
}

//------------------------------------------------------------------------------
// D3DDevice_AddRef

extern "C"
ULONG WINAPI D3DDevice_AddRef() 
{ 
    CDevice *pDevice = g_pDevice;

    return ++pDevice->m_cRefs;
}

//------------------------------------------------------------------------------
// D3DDevice_Release

extern "C"
ULONG WINAPI D3DDevice_Release() 
{ 
    CDevice *pDevice = g_pDevice;

    if (pDevice->m_cRefs == 1)
    {
        pDevice->UnInit();
        g_pPushBuffer = NULL;
        g_pDevice = NULL;
        ZeroMemory(pDevice, sizeof(*pDevice));

        return 0;
    }

    return --pDevice->m_cRefs;
}
 
//------------------------------------------------------------------------------
// D3DDevice_IsBusy

extern "C"
BOOL WINAPI D3DDevice_IsBusy()
{
    COUNT_API(API_D3DDEVICE_ISBUSY);

    CDevice *pDevice = g_pDevice;

    HWREG *reg = pDevice->m_NvBase;

    // See if the chip is idle.  

    if (GetPhysicalOffset((void*) pDevice->HwGet()) != 
        GetPhysicalOffset((void*) pDevice->m_pKickOff))
    {
        return TRUE;
    }

    // Note that the status check has to come AFTER the push-buffer
    // 'get' check:
    
    return (REG_RD32(reg, NV_PGRAPH_STATUS) != 0);
}

//------------------------------------------------------------------------------
// D3DDevice_BlockUntilIdle

extern "C"
void WINAPI D3DDevice_BlockUntilIdle()
{
    COUNT_API(API_D3DDEVICE_BLOCKUNTILIDLE);
    COUNT_PERFCYCLES(PERF_D3DDEVICE_BLOCKUNTILIDLE_WAITS);
    COUNT_PERFEVENT(D3DPERFEvent_BlockUntilIdle, FALSE);

    KickOffAndWaitForIdle();
}

//------------------------------------------------------------------------------
// D3DDevice_BlockOnFence
//
// Blocks the thread until a specific GPU time has passed.

extern "C"
void WINAPI D3DDevice_BlockOnFence(
    DWORD Time)
{
    COUNT_API(API_D3DDEVICE_BLOCKONFENCE);
    COUNT_PERFCYCLES(PERF_D3DDEVICE_BLOCKONFENCE_WAITS);
    COUNT_PERFEVENT(D3DPERFEvent_BlockOnFence, FALSE);

    BlockOnTime(Time, FALSE);
}

//------------------------------------------------------------------------------
// D3DDevice_KickPushBuffer

extern "C"
void WINAPI D3DDevice_KickPushBuffer()
{
    COUNT_API(API_D3DDEVICE_KICKPUSHBUFFER);

    g_pDevice->KickOff();
}

//------------------------------------------------------------------------------
// D3DDevice_BeginVisibilityTest

extern "C"
void WINAPI D3DDevice_BeginVisibilityTest()
{
    COUNT_API(API_D3DDEVICE_BEGINVISIBILITYTEST);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push2(pPush, NV097_CLEAR_REPORT_VALUE, 

          // NV097_CLEAR_REPORT_VALUE:

          NV097_CLEAR_REPORT_VALUE_TYPE_ZPASS_PIXEL_CNT,

          // NV097_SET_ZPASS_PIXEL_COUNT_ENABLE:

          TRUE);

    pDevice->EndPush(pPush + 3);
}

//------------------------------------------------------------------------------
// GetVisibilityAddress

BYTE* GetVisibilityAddress(
    DWORD Index)
{
    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (Index >= D3DVISIBILITY_TEST_MAX)
        {
            DPF_ERR("Index must be less than D3DVISIBILITY_TEST_MAX");
        }
    }

    // Assert that the divide and modulus we do here will really be shifts
    // and ANDs:

    ASSERT((REPORTS_PER_ALLOCATION & (REPORTS_PER_ALLOCATION - 1)) == 0);

    DWORD allocationIndex = Index / REPORTS_PER_ALLOCATION;
    if (pDevice->m_ReportAllocations[allocationIndex] == NULL)
    {
        // Technically, our allocations don't have to be contiguous, but
        // they do have to be page-locked, uncached, and in the lower 16MB
        // of physical memory:

        pDevice->m_ReportAllocations[allocationIndex]
            = MmAllocateContiguousMemoryEx(REPORTS_ALLOCATION_SIZE,
                                           0,
                                           REPORTS_MAXIMUM_ADDRESS - 1,
                                           0,
                                           PAGE_READWRITE);

        // DOC: If this fails, they can't call GetVisibilityTestResults
        //      for this index!

        if (pDevice->m_ReportAllocations[allocationIndex] == NULL)
        {
            return NULL;
        }
        if (DBG_CHECK(TRUE))
        {
            RtlFillMemoryUlong(pDevice->m_ReportAllocations[allocationIndex],
                               REPORTS_ALLOCATION_SIZE,
                               REPORTS_UNINITIALIZED_MARKER);
        }
    }

    BYTE *virtualAddress = (BYTE *)pDevice->m_ReportAllocations[allocationIndex]
        + (Index % REPORTS_PER_ALLOCATION) * REPORTS_SIZE;

    // When recording a state-block, the write-back never gets kicked off,
    // so don't actually mark the slot as used.  When they do the fix-up is
    // when this should happen.
    // !!! Actually, this should be a passed-in flag.

    if (!(pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER))
    {
        // The hardware always clears the fourth dword to zero, so we use that
        // for our marker:
    
        volatile DWORD* pMarker = (DWORD*) virtualAddress + 3;
    
        if (DBG_CHECK(*pMarker == REPORTS_INCOMPLETE_MARKER))
        {
            DPF_ERR("The previous visibility test for this index hasn't "
                    "completed yet");
        }
    
        *pMarker = REPORTS_INCOMPLETE_MARKER;
    }

    return virtualAddress;
}

//------------------------------------------------------------------------------
// D3DDevice_EndVisibilityTest

extern "C"
HRESULT WINAPI D3DDevice_EndVisibilityTest(
    DWORD Index)
{
    COUNT_API(API_D3DDEVICE_ENDVISIBILITYTEST);

    BYTE* virtualAddress = GetVisibilityAddress(Index);
    if (virtualAddress == NULL)
        return E_OUTOFMEMORY;

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push2(pPush, NV097_SET_ZPASS_PIXEL_COUNT_ENABLE,

          // NV097_SET_ZPASS_PIXEL_COUNT_ENABLE:

          FALSE,

          // NV097_GET_REPORT:
          
          DRF_DEF(097, _GET_REPORT, _TYPE, _ZPASS_PIXEL_CNT)
        | DRF_NUMFAST(097, _GET_REPORT, _OFFSET, GetGPUAddress(virtualAddress)));

    pDevice->EndPush(pPush + 3);

    // Make sure the GPU gets to the 'report' command as soon as possible.
    // This also helps retarded apps that immediately sit and spin on
    // GetVisibilityTestResult after calling EndVisibilityTest:

    pDevice->KickOff();

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DDevice_GetVisibilityTestResult
//
// DOC: EndVisibilityTest doesn't flush the push-buffer, so if someone 
//      immediately spins after doing EndVisibilityTest, they may spin 
//      forever!  Need to expose API to start a push-buffer kickoff...
//
// DOC: Multisampling affects the results!

extern "C"
HRESULT WINAPI D3DDevice_GetVisibilityTestResult(
    DWORD Index,
    UINT* pResult,
    ULONGLONG* pTimeStamp) 
{
    COUNT_API(API_D3DDEVICE_GETVISIBILITYTESTRESULT);

    CDevice* pDevice = g_pDevice;

    DWORD allocationIndex = Index / REPORTS_PER_ALLOCATION;

    if (DBG_CHECK(TRUE))
    {
        if (Index >= D3DVISIBILITY_TEST_MAX)
        {
            DPF_ERR("Index must be less than D3DVISIBILITY_TEST_MAX");
        }
        if (pDevice->m_ReportAllocations[allocationIndex] == NULL)
        {
            DPF_ERR("EndVisibilityTest wasn't called on this index!  "
                    "(Or it failed.)");
        }
    }

    DWORD virtualAddress = (DWORD) pDevice->m_ReportAllocations[allocationIndex]
        + (Index % REPORTS_PER_ALLOCATION) * REPORTS_SIZE;

    // We use the fourth dword as the marker, since the hardware always sets
    // that to zero when done:

    DWORD* pMarker = (DWORD*) virtualAddress + 3;

    if (DBG_CHECK(*pMarker == REPORTS_UNINITIALIZED_MARKER))
    {
        DPF_ERR("EndVisibilityTest wasn't called on this index");
    }

    // These reads are coming from uncached memory, so will be a bit 
    // expensive:

    if (*pMarker == REPORTS_INCOMPLETE_MARKER)
        return D3DERR_TESTINCOMPLETE;

    // The count is in the third dword:

    *pResult = *((DWORD*) virtualAddress + 2);

    // The time-stamp is in the first two dwords:

    if (pTimeStamp)
    {
        *pTimeStamp = *((ULONGLONG*) virtualAddress);
    }

    return S_OK;
}


//------------------------------------------------------------------------------
// D3DDevice_GetDisplayFieldStatus

extern "C"
void WINAPI D3DDevice_GetDisplayFieldStatus(
    D3DFIELD_STATUS *pFieldStatus)
{
    COUNT_API(API_D3DDEVICE_GETDISPLAYFIELDSTATUS);

    CDevice *pDevice = g_pDevice;

    pFieldStatus->VBlankCount = pDevice->m_Miniport.m_VBlankCount;

    if (pDevice->m_Miniport.m_CurrentAvInfo & (AV_FLAGS_INTERLACED | AV_FLAGS_FIELD))
    {
        // Determine even/odd field if it's TV

        if (pDevice->m_Miniport.IsOddField())
        {
            pFieldStatus->Field = D3DFIELD_ODD;
        }
        else
        {
            pFieldStatus->Field = D3DFIELD_EVEN;
        }
    }
    else
    {
        pFieldStatus->Field = D3DFIELD_PROGRESSIVE;  
    }
}

//------------------------------------------------------------------------------
// D3DDevice_SetVerticalBlankCallback
//
// Sets the function that will be called on each vertical blank.
// This method will be called at DPC-time.  Set this to NULL to disable
// the callback.

extern "C"
void WINAPI D3DDevice_SetVerticalBlankCallback(
    D3DVBLANKCALLBACK pCallback)
{
    COUNT_API(API_D3DDEVICE_SETVERTICALBLANKCALLBACK);

    g_pDevice->m_Miniport.m_pVerticalBlankCallback = pCallback;
}

//------------------------------------------------------------------------------
// D3DDevice_SetSwapCallback
//
// Sets the function that will be called on each Swap call.
// This method will be called at DPC-time.  Set this to NULL to disable
// the callback.

extern "C"
void WINAPI D3DDevice_SetSwapCallback(
    D3DSWAPCALLBACK pCallback)
{
    COUNT_API(API_D3DDEVICE_SETSWAPCALLBACK);

    g_pDevice->m_Miniport.m_pSwapCallback = pCallback;
}

//------------------------------------------------------------------------------
// D3DDevice_BlockUntilVerticalBlank
//
// Block until the next vertical blank interrupt.  

extern "C"
void WINAPI D3DDevice_BlockUntilVerticalBlank()
{
    COUNT_API(API_D3DDEVICE_BLOCKUNTILVERTICALBLANK);
    COUNT_PERFCYCLES(PERF_D3DDEVICE_BLOCKUNTILVERTICALBLANK_WAITS);

    CDevice *pDevice = g_pDevice;

    // Reset the event.

    KeClearEvent(&pDevice->m_Miniport.m_VerticalBlankEvent);

    // Block on it.

#if DBG

    DWORD Status;
    LARGE_INTEGER TimeOut;

    TimeOut.QuadPart = (LONGLONG)D3D__DeadlockTimeOutVal * (LONGLONG)-10000;

    for (;;)
    {
        Status = KeWaitForSingleObject(&pDevice->m_Miniport.m_VerticalBlankEvent,
                                       UserRequest,
                                       UserMode,
                                       FALSE,
                                       &TimeOut
                                       );

        if (Status != STATUS_TIMEOUT)
        {
            break;
        }

        DXGRIP("Possible deadlock, blocked for more than %d ms.", D3D__DeadlockTimeOutVal);
    }

#else !DBG

    KeWaitForSingleObject(&pDevice->m_Miniport.m_VerticalBlankEvent,
                          UserRequest,
                          UserMode,
                          FALSE,
                          NULL);

#endif !DBG
}

//----------------------------------------------------------------------------
// Flush the vertex cache.

extern "C"
void WINAPI D3DDevice_FlushVertexCache()
{
    COUNT_API(API_D3DDEVICE_FLUSHVERTEXCACHE);

    CDevice *pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_INVALIDATE_VERTEX_CACHE_FILE, 0);

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// D3DDevice_GetProjectionViewportMatrix

extern "C"
void WINAPI D3DDevice_GetProjectionViewportMatrix(
    D3DMATRIX* pProjectionViewport)
{
    COUNT_API(API_D3DDEVICE_GETPROJECTIONVIEWPORTMATRIX);

    *pProjectionViewport = g_pDevice->m_ProjectionViewportTransform;
}

//------------------------------------------------------------------------------
// D3DDevice_SetModelView

extern "C"
void WINAPI D3DDevice_SetModelView(
    CONST D3DMATRIX* pModelView,
    CONST D3DMATRIX* pInverseModelView,
    CONST D3DMATRIX* pComposite)
{
    COUNT_API(API_D3DDEVICE_SETMODELVIEW);

    CDevice *pDevice = g_pDevice;

    if (pComposite == NULL)
    {
        if (DBG_CHECK(TRUE))
        {
            if ((pInverseModelView != NULL) || (pModelView != NULL))
            {
                DPF_ERR("All pointers must be NULL to disable");
            }
        }

        D3D__DirtyFlags &= ~D3DDIRTYFLAG_DIRECT_MODELVIEW;
        D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;

        return;
    }

    if (DBG_CHECK(TRUE))
    {                         
        if (pDevice->m_ConstantMode == D3DSCM_192CONSTANTS)
        {
            DPF_ERR("SetModelView will nuke negative constant registers");
        }
        if (D3D__RenderState[D3DRS_VERTEXBLEND])
        {
            DPF_ERR("Can call SetVertexBlendModelView only when "
                    "D3DRS_VERTEXBLEND is FALSE");
        }
    }

    D3D__DirtyFlags &= ~D3DDIRTYFLAG_TRANSFORM;
    D3D__DirtyFlags |= D3DDIRTYFLAG_DIRECT_MODELVIEW;

    PPUSH pPush = pDevice->StartPush();

    PushMatrixTransposed(pPush, NV097_SET_MODEL_VIEW_MATRIX0(0), pModelView);

    PushMatrixTransposed(pPush + 17, NV097_SET_COMPOSITE_MATRIX(0), pComposite);

    pPush += 34;

    if (pInverseModelView)
    {
        PushInverseModelViewMatrix(pPush, 
                             NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0), 
                             pInverseModelView);
        pPush += 13;
    }

    pDevice->EndPush(pPush);

    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        memcpy(&pDevice->m_ModelViewTransform[0], pModelView, sizeof(D3DMATRIX));
    }
}

//------------------------------------------------------------------------------
// D3DDevice_GetModelView

extern "C"
HRESULT WINAPI D3DDevice_GetModelView(
    D3DMATRIX* pModelView)
{
    COUNT_API(API_D3DDEVICE_GETMODELVIEW);

    CDevice* pDevice = g_pDevice;

    if (pModelView)
    {
        memcpy(pModelView, &pDevice->m_ModelViewTransform[0], sizeof(D3DMATRIX));
    }

    return (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_MODELVIEW) ? S_OK : S_FALSE;
}

//------------------------------------------------------------------------------
// D3DDevice_SetVertexBlendModelView

extern "C"
void WINAPI D3DDevice_SetVertexBlendModelView(
    UINT Count,
    CONST D3DMATRIX* pModelViews,
    CONST D3DMATRIX* pInverseModelViews,
    CONST D3DMATRIX* pProjectionViewport)
{
    DWORD i;

    COUNT_API(API_D3DDEVICE_SETVERTEXBLENDMODELVIEW);

    CDevice *pDevice = g_pDevice;

    if (pModelViews == NULL)
    {
        if (DBG_CHECK(TRUE))
        {
            if (pInverseModelViews != NULL)
            {
                DPF_ERR("All pointers must be NULL to disable");
            }
        }

        D3D__DirtyFlags &= ~D3DDIRTYFLAG_DIRECT_MODELVIEW;
        D3D__DirtyFlags |= D3DDIRTYFLAG_TRANSFORM;

        return;
    }

    if (DBG_CHECK(TRUE))
    {
        if (pDevice->m_ConstantMode == D3DSCM_192CONSTANTS)
        {
            DPF_ERR("SetVertexBlendModelViews will nuke negative constant "
                    "registers");
        }
        if ((Count == 0) || (Count > 4))
        {
            DPF_ERR("Count out of range");
        }
        if (!D3D__RenderState[D3DRS_VERTEXBLEND])
        {
            DPF_ERR("Can call SetVertexBlendModelView only when "
                    "D3DRS_VERTEXBLEND is TRUE");
        }
    }

    D3D__DirtyFlags &= ~D3DDIRTYFLAG_TRANSFORM;
    D3D__DirtyFlags |= D3DDIRTYFLAG_DIRECT_MODELVIEW;

    PPUSH pPush = pDevice->StartPush(17 * 2 * Count + 17);

    for (i = 0; i < Count; i++)
    {
        DWORD stride = NV097_SET_MODEL_VIEW_MATRIX1(0) 
                     - NV097_SET_MODEL_VIEW_MATRIX0(0);

        PushMatrixTransposed(pPush, 
                             NV097_SET_MODEL_VIEW_MATRIX0(0) + (i * stride),
                             &pModelViews[i]);

        pPush += 17;
    }

    if (pInverseModelViews)
    {
        for (i = 0; i < Count; i++)
        {
            DWORD stride = NV097_SET_INVERSE_MODEL_VIEW_MATRIX1(0) 
                         - NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0);

            PushInverseModelViewMatrix(
                pPush, 
                NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) + (i * stride),
                &pInverseModelViews[i]);

            pPush += 13;
        }
    }

    PushMatrixTransposed(pPush,
                         NV097_SET_COMPOSITE_MATRIX(0),
                         pProjectionViewport);

    pPush += 17;

    pDevice->EndPush(pPush);

    if (!(pDevice->m_StateFlags & STATE_PUREDEVICE))
    {
        memcpy(&pDevice->m_ModelViewTransform[0], 
               pModelViews, 
               Count * sizeof(D3DMATRIX));

        memcpy(&pDevice->m_ProjectionViewport, 
               pProjectionViewport, 
               sizeof(D3DMATRIX));
    }
}

//------------------------------------------------------------------------------
// GetVertexBlendModelView

extern "C"
HRESULT WINAPI D3DDevice_GetVertexBlendModelView(
    UINT Count,
    D3DMATRIX* pModelViews,
    D3DMATRIX* pProjectionViewport)
{
    COUNT_API(API_D3DDEVICE_GETVERTEXBLENDMODELVIEW);

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(Count > 4))
    {
        DPF_ERR("Can't request more than 4 matrices");
    }

    if (pModelViews)
    {
        memcpy(pModelViews, 
               &pDevice->m_ModelViewTransform[0], 
               Count * sizeof(D3DMATRIX));
    }

    if (pProjectionViewport)
    {
        memcpy(pProjectionViewport,
               &pDevice->m_ProjectionViewport,
               sizeof(D3DMATRIX));
    }

    return (D3D__DirtyFlags & D3DDIRTYFLAG_DIRECT_MODELVIEW) ? S_OK : S_FALSE;
}

//------------------------------------------------------------------------------
// Nop
//
// Used to reserve space for the push-buffer Jump method.

extern "C"
void WINAPI D3DDevice_Nop()
{
    COUNT_API(API_D3DDEVICE_NOP);

    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_NO_OPERATION, 0);

    pDevice->EndPush(pPush + 2);
}

//------------------------------------------------------------------------------
// SetTile

extern "C"
void WINAPI D3DDevice_SetTile(
    DWORD Index,
    CONST D3DTILE* pTile)
{
    COUNT_API(API_D3DDEVICE_SETTILE);

    DWORD i;

    CDevice* pDevice = g_pDevice;

    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice == NULL)
        {
            DPF_ERR("Device hasn't been initialized yet");
        }
        if (Index >= D3DTILE_MAXTILES)
        {
            DPF_ERR("Can't set any tile index of D3DTILE_MAXTILES (8) or more");
        }
    }

    if ((pTile == NULL) || (pTile->pMemory == NULL))
    {
        // Note that we let the same tile be cleared multiple times.

        pDevice->m_Miniport.DestroyTile(Index, pDevice->m_Tile[Index].ZOffset);

        ZeroMemory(&pDevice->m_Tile[Index], sizeof(D3DTILE));

        return;
    }

    D3DTILE tile = *pTile;

    if (!(tile.Flags & D3DTILE_FLAGS_ZCOMPRESS))
    {
        tile.ZOffset = 0;
        tile.ZStartTag = 0;
    }

    if (DBG_CHECK(TRUE))
    {
        BOOL goodPitch = FALSE;

        AssertContiguous(tile.pMemory);

        if (pDevice->m_Tile[Index].Size != 0)
        {
            DPF_ERR("Tile already in use - set to NULL first if you want to reset");
        }
        if (tile.Flags & ~(D3DTILE_FLAGS_Z32BITS | D3DTILE_FLAGS_ZCOMPRESS | D3DTILE_FLAGS_ZBUFFER))
        {
            DPF_ERR("Invalid Flags");
        }
        if (tile.Size == 0)
        {
            DPF_ERR("Size can't be zero if pMemory is non-zero");
        }
        if ((DWORD) tile.pMemory & 0x3fff)
        {
            DPF_ERR("pMemory must be a multiple of D3DTILE_ALIGNMENT (16k)");
        }
        if (tile.Size & 0x3fff)
        {
            DPF_ERR("Size must be a multiple of D3DTILE_ALIGNMENT (16k)");
        }

#if 0
        // Not true.  The memory allocations can be padded in such a way that
        // the area to be tiled is not a multiple of the interesting pitch.
        // Try 620x240 field rendering with 2 back buffers, for example.
        //
        if ((tile.Size % tile.Pitch) != 0)
        {
            DPF_ERR("Size should be a multiple of Pitch");
        }
#endif

        for (i = 0; i < ARRAYSIZE(g_TilePitches); i++)
        {
            if (g_TilePitches[i] == tile.Pitch)
            {
                goodPitch = TRUE;
            }
        }
        if (!goodPitch)
        {
            DPF_ERR("Pitch must be a valid D3DTILE_PITCH_* value");
        }
        if (tile.Flags & D3DTILE_FLAGS_ZCOMPRESS)
        {
            if (tile.ZStartTag & 0xff)
            {
                DPF_ERR("ZStartTag must be a multiple of 256");
            }
            if (tile.ZStartTag >= D3DTILE_MAXTAGS)
            {
                DPF_ERR("ZStart can't be D3DTILE_MAXTAGS (76800) or more");
            }
            if (tile.ZOffset & 0x3fff)
            {
                DPF_ERR("ZOffset must be a multiple of D3DTILE_ALIGNMENT (16k)");
            }
            if ((tile.ZOffset != 0) && (D3DTILE_ZENDTAG(&tile) <= D3DTILE_MAXTAGS))
            {
                DPF_ERR("ZOffset can only be used for the last buffer - the one "
                        "in which \nthe tags run out (D3DTILE_ZENDTAG(pTile) > D3DTILE_MAXTAGS)");
            }
            for (i = 0; i < D3DTILE_MAXTILES; i++)
            {
                if (pDevice->m_Tile[i].Flags & D3DTILE_FLAGS_ZCOMPRESS)
                {
                    if ((tile.ZStartTag < D3DTILE_ZENDTAG(&pDevice->m_Tile[i])) &&
                        (D3DTILE_ZENDTAG(&tile) > pDevice->m_Tile[i].ZStartTag))
                    {
                        DPF_ERR("Z tag range overlaps with a tag range already in use");
                    }
                }
            }
        }
    }

    pDevice->m_Tile[Index] = tile;

    pDevice->m_Miniport.CreateTile(Index, 
                                   GetGPUAddress(tile.pMemory),
                                   tile.Size,
                                   tile.Pitch,
                                   tile.Flags,
                                   tile.ZStartTag,
                                   tile.ZOffset);
}

//------------------------------------------------------------------------------
// GetTile

extern "C"
void WINAPI D3DDevice_GetTile(
    DWORD Index,
    D3DTILE* pTile)
{
    COUNT_API(API_D3DDEVICE_GETTILE);

    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice == NULL)
        {
            DPF_ERR("Device hasn't been initialized yet");
        }
        if (Index >= D3DTILE_MAXTILES)
        {
            DPF_ERR("Can't set any tile index of D3DTILE_MAXTILES (8) or more");
        }
    }

    *pTile = g_pDevice->m_Tile[Index];
}

//------------------------------------------------------------------------------
// SetTileCompressionTagBits

extern "C"
void WINAPI D3DDevice_SetTileCompressionTagBits(
    DWORD Partition,
    DWORD Address,
    CONST DWORD *pData,
    DWORD Count)
{
    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice == NULL)
        {
            DPF_ERR("Device hasn't been initialized yet");
        }
        if (Partition >= 4)
        {
            DPF_ERR("Partition must be 0 - 3");
        }
        if (Address + Count > 600)
        {
            DPF_ERR("Address + Count must be <= 600");
        }
        if (REG_RD32(g_pDevice->m_NvBase, NV_PGRAPH_STATUS))
        {
            DPF_ERR("GPU should be idle for SetTileCompressionTagBits");
        }
    }

    HWREG *RegBase = g_pDevice->m_NvBase;
    DWORD DataAddress = Address % 16;
    Address /= 16;

    while (Count)
    {
        REG_WR32(RegBase, NV_PFB_RDI_INDEX,
             DRF_NUM(_PFB, _RDI_INDEX, _SELECT, NV_PFB_RDI_INDEX_SELECT_MC_ZTAG_P0 + Partition) |
             DRF_NUM(_PFB, _RDI_INDEX, _ADDRESS, Address++));

        DWORD DwordCount = min(Count, 16 - DataAddress);
        Count -= DwordCount;

        for (DWORD i = DataAddress; i < DwordCount; i++)
        {
            REG_WR32(RegBase, NV_PFB_RDI_DATA(i), *pData++);
        }

        DataAddress = 0;
    }
}

//------------------------------------------------------------------------------
// GetTileCompressionTagBits

extern "C"
void WINAPI D3DDevice_GetTileCompressionTagBits(
    DWORD Partition,
    DWORD Address,
    DWORD *pData,
    DWORD Count)
{
    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice == NULL)
        {
            DPF_ERR("Device hasn't been initialized yet");
        }
        if (Partition >= 4)
        {
            DPF_ERR("Partition must be 0 - 3");
        }
        if (Address + Count > 600)
        {
            DPF_ERR("Address + Count must be <= 600");
        }
        if (REG_RD32(g_pDevice->m_NvBase, NV_PGRAPH_STATUS))
        {
            DPF_ERR("GPU should be idle for GetTileCompressionTagBits");
        }
    }

    HWREG *RegBase = g_pDevice->m_NvBase;
    DWORD DataAddress = Address % 16;
    Address /= 16;

    while (Count)
    {
        REG_WR32(RegBase, NV_PFB_RDI_INDEX,
             DRF_NUM(_PFB, _RDI_INDEX, _SELECT, NV_PFB_RDI_INDEX_SELECT_MC_ZTAG_P0 + Partition) |
             DRF_NUM(_PFB, _RDI_INDEX, _ADDRESS, Address++));

        DWORD DwordCount = min(Count, 16 - DataAddress);
        Count -= DwordCount;

        for (DWORD i = DataAddress; i < DwordCount; i++)
        {
            *pData++ = REG_RD32(RegBase, NV_PFB_RDI_DATA(i));
        }

        DataAddress = 0;
    }
}

//------------------------------------------------------------------------------
// GetTileCompressionTags

extern "C"
DWORD WINAPI D3DDevice_GetTileCompressionTags(
    DWORD ZStartTag,
    DWORD ZEndTag)
{
    COUNT_API(API_D3DDEVICE_GETTILECOMPRESSIONTAGS);

    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice == NULL)
        {
            DPF_ERR("Device hasn't been initialized yet");
        }
        if (ZStartTag & 0xff)
        {
            DPF_ERR("ZStartTag must be a multiple of 256");
        }
        if (ZStartTag >= D3DTILE_MAXTAGS)
        {
            DPF_ERR("ZStart can't be D3DTILE_MAXTAGS (76800) or more");
        }
        if (ZEndTag & 0xff)
        {
            DPF_ERR("ZEndTag must be a multiple of 256");
        }
        if (ZEndTag > D3DTILE_MAXTAGS)
        {
            DPF_ERR("ZEndTag can't be greater than D3DTILE_MAXTAGS (76800)");
        }
        if (ZEndTag < ZStartTag)
        {
            DPF_ERR("ZEndTag must be greater than ZStartTag");
        }
    }

    // Block until the chip is idle
    KickOffAndWaitForIdle();

    /*
     * From: Stefan Gottschalk [SGottschalk@nvidia.com]
     *
     * As mentioned previously, there are 600 32-bit words in each RAM.
     *
     * We have RDI/RDO access to the tag rams.  The NV_PFB_RDI_INDEX has
     * two fields, NV_PFB_RDI_INDEX_SELECT and NV_PFB_RDI_INDEX_ADDRESS.  Setting
     * the select field to the value NV_PFB_RDI_INDEX_SELECT_MC_ZTAG_P0 (0x130)
     * will give access to the tag ram on partition 0.  Setting the address to 0x0
     * will select the first 16 32-bit words in this ram, accessible through the 16
     * registers at NV_PFB_RDI_DATA(0) through NV_PFB_RDI_DATA(15).  Setting the
     * address to 0x1 will select the second 16 words in the ram.  I hear that it
     * is an unusual system for RDI/RDO, but it permits faster access for
     * setting/reading large chunks of tag data.
     */

    HWREG *RegBase = g_pDevice->m_NvBase;

    // count of compressed blocks
    DWORD compressedtags = 0;

    // There are 4 partitions and 32 bits per dword
    DWORD DwordsPerPartition = (ZEndTag - ZStartTag) / (4 * 32);
    DWORD PartitionAddressStart = ZStartTag / (4 * 32);

    // 4 partitions
    for (DWORD partition = 0; partition < 4; partition++)
    {
        DWORD DwordsToRead = DwordsPerPartition;
        DWORD RDIDataStart = PartitionAddressStart % 16;

        // 600 tag DWORDs per partition read in blocks of 16
        for (DWORD AddressOffset = 0; DwordsToRead; AddressOffset++)
        {
            REG_WR32(RegBase, NV_PFB_RDI_INDEX,
                 DRF_NUM(_PFB, _RDI_INDEX, _SELECT, NV_PFB_RDI_INDEX_SELECT_MC_ZTAG_P0 + partition) |
                 DRF_NUM(_PFB, _RDI_INDEX, _ADDRESS, (PartitionAddressStart / 16) + AddressOffset));

            DWORD DwordCount = min(DwordsToRead, 16 - RDIDataStart);
            DwordsToRead -= DwordCount;

            for (DWORD i = RDIDataStart; i < DwordCount; i++)
            {
                DWORD data = REG_RD32(RegBase, NV_PFB_RDI_DATA(i));

                while (data)
                {
                    compressedtags++;
                    data &= (data - 1);
                }
            }

            RDIDataStart = 0;
        }
    }

    return compressedtags;
}


//------------------------------------------------------------------------------
// SetScissors

extern "C"
void WINAPI D3DDevice_SetScissors(
    DWORD Count, 
    BOOL Exclusive, 
    CONST D3DRECT *pRects)
{
    COUNT_API(API_D3DDEVICE_SETSCISSORS);

    CONST D3DRECT* pRect;
    D3DRECT rect;
    DWORD i;

    CDevice* pDevice = g_pDevice;    
    
    if (DBG_CHECK(TRUE))
    {
        if (Count >= D3DSCISSORS_MAX)
        {
            DPF_ERR("Count can't be more than D3DSCISSORS_MAX (8)");
        }
        if (Exclusive > TRUE)
        {
            DPF_ERR("Invalid Exclusive value");
        }
        if ((pRects == NULL) && (Count > 0))
        {
            DPF_ERR("pRects can be NULL only if Count is zero");
        }
        for (pRect = pRects, i = 0; i < Count; i++, pRect++)
        {
            if ((pRect->x1 >= pRect->x2) ||
                (pRect->y1 >= pRect->y2) ||
                (pRect->x1 < 0) ||
                (pRect->y1 < 0) ||
                (pRect->x2 > 4095) ||
                (pRect->y2 > 4095))
            {
                DPF_ERR("Rectangle dimension invalid or out of range");
            }
        }
    }

    if (Count == 0)
    {
        rect.x1 = pDevice->m_Viewport.X;
        rect.y1 = pDevice->m_Viewport.Y;
        rect.x2 = rect.x1 + pDevice->m_Viewport.Width;
        rect.y2 = rect.y1 + pDevice->m_Viewport.Height;

        Exclusive = FALSE;
        pRects = &rect;
        Count = 1;
    }

    // Let the hardware know:

    PPUSH pPush = pDevice->StartPush();

    Push1(pPush, NV097_SET_WINDOW_CLIP_TYPE, Exclusive);

    pPush += 2;

    FLOAT xScale = pDevice->m_SuperSampleScaleX;
    FLOAT yScale = pDevice->m_SuperSampleScaleY;

    for (pRect = pRects, i = 0; i < Count; i++, pRect++)
    {
        Push1(pPush,
              NV097_SET_WINDOW_CLIP_HORIZONTAL(i),
              DRF_NUMFAST(097,
                          _SET_WINDOW_CLIP_HORIZONTAL,
                          _XMIN,
                          Round(xScale * pRect->x1)) |
              DRF_NUMFAST(097,
                          _SET_WINDOW_CLIP_HORIZONTAL,
                          _XMAX,
                          Round(xScale * pRect->x2) - 1));
    
        Push1(pPush + 2,
              NV097_SET_WINDOW_CLIP_VERTICAL(i),
              DRF_NUMFAST(097,
                          _SET_WINDOW_CLIP_VERTICAL,
                          _YMIN,
                          Round(yScale * pRect->y1)) |
              DRF_NUMFAST(097,
                          _SET_WINDOW_CLIP_VERTICAL,
                          _YMAX,
                          Round(yScale * pRect->y2) - 1));

        pPush += 4;
    }

    pDevice->EndPush(pPush);

    // Remember the new settings:

    memcpy(&pDevice->m_ScissorsRects[0], pRects, Count * sizeof(D3DRECT));

    pDevice->m_ScissorsCount = Count;

    pDevice->m_ScissorsExclusive = Exclusive;
}

//------------------------------------------------------------------------------
// GetScissors

extern "C"
void WINAPI D3DDevice_GetScissors(
    DWORD* pCount, 
    BOOL* pExclusive, 
    D3DRECT* pRects)
{
    COUNT_API(API_D3DDEVICE_GETSCISSORS);

    CDevice* pDevice = g_pDevice;

    if (pCount != NULL)
    {
        *pCount = pDevice->m_ScissorsCount;
    }
    if (pExclusive != NULL)
    {
        *pExclusive = pDevice->m_ScissorsExclusive;
    }
    if (pRects != NULL)
    {
        memcpy(pRects, 
               &pDevice->m_ScissorsRects[0], 
               pDevice->m_ScissorsCount * sizeof(D3DRECT));
    }
}

//------------------------------------------------------------------------------
// PersistDisplay

extern BOOL g_FlickerFilterSet;
extern BOOL g_SoftDisplayFilterSet;

extern "C"
HRESULT WINAPI D3DDevice_PersistDisplay()
{
    COUNT_API(API_D3DDEVICE_PERSISTDISPLAY);

    DWORD Width, Height;
    D3DFORMAT Format;
    SIZE_T AllocSize;
    D3DSurface *pPrimary, *pCopy;
    CDevice *pDevice = g_pDevice;
    HRESULT hr;

    pCopy = (D3DSurface*)AvGetSavedDataAddress();
     
    if (pCopy != NULL)
    {
        MmFreeContiguousMemory(pCopy);
        AvSetSavedDataAddress(NULL);
    }

    // Can't make a copy if the device has already been uninitialized.

    if (pDevice->m_cRefs == 0)
    {
        return E_FAIL;
    }

    // Wait for the present to be processed.

    while (pDevice->m_Miniport.IsFlipPending()) {};

    // Make a copy of the active frame.

    pPrimary = pDevice->m_pFrameBuffer[1];
    Format = PixelJar::GetFormat(pPrimary);
    Width = PixelJar::GetWidth(pPrimary);
    Height = PixelJar::GetHeight(pPrimary);

    hr = CreateSurfaceWithContiguousHeader(Width, Height, Format, &pCopy);
    if (FAILED(hr))
    {
        return hr;
    }

    // Make a copy of the current buffer using the GPU and wait for
    // it to finish, so we don't reboot prematurely.

    D3DDevice_CopyRects(pPrimary, NULL, 0, pCopy, NULL);
    KickOffAndWaitForIdle();

    void *RegisterBase = pDevice->m_Miniport.m_RegisterBase;
    DWORD DisplayMode = pDevice->m_Miniport.m_DisplayMode;
    ULONG Step = 0;

    do
    {
        Step = AvSetDisplayMode(
                RegisterBase,
                Step,
                DisplayMode,
                MapToLinearD3DFORMAT(Format),
                PixelJar::GetPitch(pCopy),
                pCopy->Data);
    }
    while (Step);

    // Make sure the next call to these APIs don't get short-circuited.
    g_FlickerFilterSet = FALSE;
    g_SoftDisplayFilterSet = FALSE;

    // Save the data address and persist this memory across reboots.

    AllocSize = MmQueryAllocationSize(pCopy);
    MmPersistContiguousMemory(pCopy, AllocSize, TRUE);
    AvSetSavedDataAddress(pCopy);

    return S_OK;
}

//------------------------------------------------------------------------------
// GetPersistedSurface

extern "C"
void WINAPI D3DDevice_GetPersistedSurface(
    IDirect3DSurface8 **ppSurface)
{
    *ppSurface = (D3DSurface*)AvGetSavedDataAddress();
}

//------------------------------------------------------------------------------
// Adjusts the aggressiveness of the flicker filter.

BOOL g_FlickerFilterSet;
DWORD g_FlickerFilterValue;

extern "C"
void WINAPI D3DDevice_SetFlickerFilter(
    DWORD Filter)
{
    CDevice *pDevice = g_pDevice;

    if (DBG_CHECK(Filter > 5))
    {
        RIP(("D3DDevice_SetFlickerFilter - Bad value, must be < 6."));
    }

    if (!g_FlickerFilterSet || g_FlickerFilterValue != Filter)
    {
        AvSendTVEncoderOption(pDevice->m_Miniport.m_RegisterBase, 
                              AV_OPTION_FLICKER_FILTER,
                              Filter,
                              NULL);

        g_FlickerFilterSet = TRUE;
        g_FlickerFilterValue = Filter;
    }
}

//------------------------------------------------------------------------------
// Enable the luma 

BOOL g_SoftDisplayFilterSet;
BOOL g_SoftDisplayFilterValue;

extern "C"
void WINAPI D3DDevice_SetSoftDisplayFilter(
    BOOL Enable)
{
    CDevice *pDevice = g_pDevice;

    if (!g_SoftDisplayFilterSet || g_SoftDisplayFilterValue != !!Enable)
    {
        AvSendTVEncoderOption(pDevice->m_Miniport.m_RegisterBase, 
                              AV_OPTION_ENABLE_LUMA_FILTER,
                              Enable,
                              NULL);

        g_SoftDisplayFilterSet = TRUE;
        g_SoftDisplayFilterValue = !!Enable;
    }
}

//
// Private helpers for hardware testing.
//

_declspec(selectany) DWORD g_DoChecksum; 
_declspec(selectany) DWORD g_Channel; 
_declspec(selectany) DWORD g_Value[3]; 

//------------------------------------------------------------------------------ 
// Callback used to calculate the checksum for the current display.  This must
// be passed to D3DDevice_SetVerticalBlankCallback once before
// D3DTest_ScreenChecksum is called.

extern "C"
void __cdecl D3DTest_ChecksumCallback(
    D3DVBLANKDATA *pData)
{ 
    if (g_DoChecksum) 
    { 
        if (g_Channel > 0) 
        { 
            g_Value[g_Channel - 1] = *(volatile DWORD *)0xFD680840 & 0x00FFFFFF; 
        } 

        if (g_Channel < 3) 
        { 
            *(volatile DWORD *)0xFD680844 = 0x411 | (g_Channel << 8); 
            *(volatile DWORD *)0xFD680844 = 0x410 | (g_Channel << 8); 

            g_Channel++; 
        } 
        else 
        { 
            g_Channel = 0; 
            g_DoChecksum = 0; 
        } 
    } 
} 

//------------------------------------------------------------------------------ 
// Calculates the frame's checksum. 

extern "C"
void D3DTest_GetScreenChecksum( 
    DWORD *pRed, 
    DWORD *pGreen, 
    DWORD *pBlue) 
{ 
    g_DoChecksum = 1; 

    while (g_DoChecksum) 
    { 
        D3DDevice_BlockUntilVerticalBlank(); 
    } 

    *pRed   = g_Value[0]; 
    *pGreen = g_Value[1]; 
    *pBlue  = g_Value[2]; 
} 

//------------------------------------------------------------------------------
// Push a fence into the push buffer.

extern "C"
DWORD WINAPI D3DDevice_InsertFence()
{
    COUNT_API(API_D3DDEVICE_INSERTFENCE);

    return SetFence(0);
}

//------------------------------------------------------------------------------
// SetScreenSpaceOffset

extern "C"
void WINAPI D3DDevice_SetScreenSpaceOffset(
    float x,
    float y)
{
    COUNT_API(API_D3DDEVICE_SETSCREENSPACEOFFSET);

    CDevice* pDevice = g_pDevice;

    // We always add KELVIN_BORDER to the offset to match D3D's pixel
    // conventions.  

    pDevice->m_ScreenSpaceOffsetX = x + KELVIN_BORDER;
    pDevice->m_ScreenSpaceOffsetY = y + KELVIN_BORDER;

    CommonSetPassthroughProgram(pDevice);

    PPUSH pPush = pDevice->StartPush();

    pPush = CommonSetViewport(pDevice, pPush);

    pDevice->EndPush(pPush);
}

//------------------------------------------------------------------------------
// GetScreenSpaceOffset

extern "C"
void WINAPI D3DDevice_GetScreenSpaceOffset(
    float *pX,
    float *pY)
{
    COUNT_API(API_D3DDEVICE_GETSCREENSPACEOFFSET);

    CDevice* pDevice = g_pDevice;

    *pX = pDevice->m_ScreenSpaceOffsetX - KELVIN_BORDER;
    *pY = pDevice->m_ScreenSpaceOffsetY - KELVIN_BORDER;
}

//------------------------------------------------------------------------------
// SetOverscanColor

DWORD g_OverscanColor;

extern "C"
void WINAPI D3DDevice_SetOverscanColor(
    D3DCOLOR Color)
{
    double R, G, B;

    DWORD Y, Cr, Cb;
    DWORD RegColor;

    void *RegisterBase = g_pDevice->m_Miniport.m_RegisterBase;
    DWORD AvInfo = g_pDevice->m_Miniport.GetDisplayCapabilities();

    if ((AvInfo & AV_PACK_MASK) == AV_PACK_SCART)
    {
        RegColor = Color & 0x00FFFFFF;
    }
    else
    {
        R = ((Color >> 16) & 0xFF);
        G = ((Color >>  8) & 0xFF);
        B = ((Color >>  0) & 0xFF);

        Y  = (DWORD)(0.2989 * R + 0.5866 * G + 0.1145 * B);
        Cr = (DWORD)(128.0 + 0.5000 * R - 0.4183 * G - 0.0816 * B);
        Cb = (DWORD)(128.0 - 0.1687 * R - 0.3312 * G + 0.5000 * B);

        RegColor = ((Cr & 0xFF) << 16) | ((Y & 0xFF) << 8) | ((Cb & 0xFF) << 0);
    }

    REG_WR32(RegisterBase, NV_PRAMDAC_FP_INACTIVE_PXL_COLOR, RegColor);

    g_OverscanColor = Color;
}

//------------------------------------------------------------------------------
// GetOverscanColor

extern "C"
D3DCOLOR WINAPI D3DDevice_GetOverscanColor()
{
    return g_OverscanColor;
}

} // end namespace
