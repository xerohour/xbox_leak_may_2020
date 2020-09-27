/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dxgcreate.cpp
 *  Content     Creates the DirectX graphics device object
 *
 ***************************************************************************/
 
#include "precomp.hpp"
#include "dm.h"
#include <xbdm.h>

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

DWORD CDevice::m_PushBufferSize;
DWORD CDevice::m_PushSegmentSize;

//------------------------------------------------------------------------------
// InitializeKelvin
//
// One-time initialization for the kelvin class

VOID InitializeKelvin()
{
    CDevice* pDevice = g_pDevice;

    PPUSH pPush = pDevice->StartPush();

    PushCount(pPush, NV097_SET_CONTEXT_DMA_NOTIFIES, 3);

    // NV097_SET_CONTEXT_DMA_NOTIFIES:

    *(pPush + 1) = NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY; 

    // NV097_SET_CONTEXT_DMA_A:

    *(pPush + 2) = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;

    // NV097_SET_CONTEXT_DMA_B:

    *(pPush + 3) = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;                

    PushCount(pPush + 4, NV097_SET_CONTEXT_DMA_STATE, 6);

    // NV097_SET_CONTEXT_DMA_STATE:

    *(pPush + 5) = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;                

    // NV097_SET_CONTEXT_DMA_COLOR:

    *(pPush + 6) = D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY;

    // NV097_SET_CONTEXT_DMA_ZETA:

    *(pPush + 7) = D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY;

    // NV097_SET_CONTEXT_DMA_VERTEX_A:

    *(pPush + 8) = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;

    // NV097_SET_CONTEXT_DMA_VERTEX_B:

    *(pPush + 9) = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY; 

    // NV097_SET_CONTEXT_DMA_SEMAPHORE

    *(pPush + 10 ) = D3D_SEMAPHORE_CONTEXT_DMA_IN_MEMORY;

    PushCount(pPush + 11, NV097_SET_CONTEXT_DMA_REPORT, 1);

    // NV097_SET_CONTEXT_DMA_REPORT:

    *(pPush + 12) = D3D_CONTEXT_IN_CACHED_MEMORY;

    Push1(pPush + 13, NV097_SET_SEMAPHORE_OFFSET, 0);

    pDevice->EndPush(pPush + 15);

    // Set some state that the driver never changes:

    pPush = pDevice->StartPush();

    // D3D always says to take the first vertex color for flat shading:

    Push1(pPush, NV097_SET_FLAT_SHADE_OP, NV097_SET_FLAT_SHADE_OP_V_FIRST_VTX);

    // EXT: ?

    Push4f(pPush + 2, NV097_SET_EYE_POSITION(0), 0.0f, 0.0f, 0.0f, 1.0f);

    // Edge flags are pretty much useless for games, we don't bother exposing
    // this as an extension:

    Push1(pPush + 7, NV097_SET_EDGE_FLAG, NV097_SET_EDGE_FLAG_V_TRUE); 

    // The fixed function pipeline always expects dependent texture 
    // lookups to run between stages i and i+1, so set that now.

    Push1(pPush + 9, 
          NV097_SET_SHADER_OTHER_STAGE_INPUT,
          (DRF_DEF(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE1, _INSTAGE_0) |
           DRF_DEF(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE2, _INSTAGE_1) |
           DRF_DEF(097, _SET_SHADER_OTHER_STAGE_INPUT, _STAGE3, _INSTAGE_2)));

    // We never have to disable Z compression:

    Push1(pPush + 11, NV097_SET_COMPRESS_ZBUFFER_EN, TRUE);

    // According to Shaun Ho, "offset clamp has a functional bug. It is 
    // fine to use polygon_offset, but once a polygon is offset, it should 
    // not be clamped."  It should be initialized to positive infinity.

    Push1(pPush + 13, NV097_SET_SHADOW_ZSLOPE_THRESHOLD, F_POS_INFINITY);

    pDevice->EndPush(pPush + 15);

    // Load the fixed function pipeline constant registers and set the
    // texgen planes:

    pDevice->SetShaderConstantMode(D3DSCM_96CONSTANTS);

#if 0

    // Re-enable this snippet to verify that push buffer errors are being
    // reported by the miniport:

    pPush = pDevice->StartPush();
    Push1(pPush,  NV097_SET_WINDOW_CLIP_HORIZONTAL(0), 0xffffffff);

    pDevice->EndPush(pPush + 2);

#endif
}

//------------------------------------------------------------------------------
// InitializeHardware
//
// One-time hardware initialization.  
//
// Initializes the sub-channels to the static set of objects that the driver
// uses, and initializes those classes to the default state.

VOID InitializeHardware()
{
    PPUSH pPush;

    CDevice* pDevice = g_pDevice;

    // Initialize our stock subchannels:

    pPush = pDevice->StartPush();

    Push1(pPush,     SUBCH_MEMCOPY, NV039_SET_OBJECT, D3D_MEMORY_TO_MEMORY_COPY);
    Push1(pPush + 2, SUBCH_RECTCOPY, NV05F_SET_OBJECT, D3D_RECTANGLE_COPY);
    Push1(pPush + 4, SUBCH_RECTCOPYSURFACES, NV062_SET_OBJECT, D3D_RECTANGLE_COPY_SURFACES);
    Push1(pPush + 6, SUBCH_3D, NV097_SET_OBJECT, D3D_KELVIN_PRIMITIVE);
    pPush += 8;

    // Memcpy class.

    Push1(pPush, 
          SUBCH_MEMCOPY, 
          NV039_SET_CONTEXT_DMA_NOTIFIES, 
          D3D_MEMCOPY_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    // Rectcopy class
    
    Push1(pPush + 2,
          SUBCH_RECTCOPY,
          NV05F_SET_OPERATION,
          NV05F_SET_OPERATION_SRCCOPY);

    // We just initalized our CopyRect operation to SRCCOPY.

    ZeroMemory(&pDevice->m_CopyRectRopState, sizeof(pDevice->m_CopyRectRopState));

    ZeroMemory(&pDevice->m_CopyRectState, sizeof(pDevice->m_CopyRectState));

    pDevice->m_CopyRectState.Operation = D3DCOPYRECT_SRCCOPY;
    pDevice->m_CopyRectState.ColorFormat = D3DCOPYRECT_COLOR_FORMAT_DEFAULT;

    Push2(pPush + 4, 
          SUBCH_RECTCOPYSURFACES, 
          NV062_SET_CONTEXT_DMA_IMAGE_SOURCE, 

          // NV062_SET_CONTEXT_DMA_IMAGE_SOURCE:

          NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,

          // NV062_SET_CONTEXT_DMA_IMAGE_DESTIN:

          D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY);

    pPush += 7;

    PushCount(pPush, SUBCH_RECTCOPY, NV05F_SET_CONTEXT_COLOR_KEY, 7);

    pPush[1] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_COLOR_KEY
    pPush[2] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_CLIP_RECTANGLE
    pPush[3] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_PATTERN
    pPush[4] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_ROP
    pPush[5] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_BETA1
    pPush[6] = D3D_NULL_OBJECT;             // NV05F_SET_CONTEXT_BETA4
    pPush[7] = D3D_RECTANGLE_COPY_SURFACES; // NV05F_SET_CONTEXT_SURFACES

    pDevice->EndPush(pPush + 8);

    // The Kelvin class.

    InitializeKelvin();
}   

//------------------------------------------------------------------------------
// g_InitialRenderStates

D3DCONST DWORD g_InitialRenderStates[] =
{
    D3DCMP_LESSEQUAL,           // D3DRS_ZFUNC                     
    D3DCMP_ALWAYS,              // D3DRS_ALPHAFUNC                 
    FALSE,                      // D3DRS_ALPHABLENDENABLE          
    FALSE,                      // D3DRS_ALPHATESTENABLE           
    0,                          // D3DRS_ALPHAREF                  
    D3DBLEND_ONE,               // D3DRS_SRCBLEND                  
    D3DBLEND_ZERO,              // D3DRS_DESTBLEND                 
    TRUE,                       // D3DRS_ZWRITEENABLE              
    FALSE,                      // D3DRS_DITHERENABLE              
    D3DSHADE_GOURAUD,           // D3DRS_SHADEMODE                 
    D3DCOLORWRITEENABLE_ALL,    // D3DRS_COLORWRITEENABLE          
    D3DSTENCILOP_KEEP,          // D3DRS_STENCILZFAIL              
    D3DSTENCILOP_KEEP,          // D3DRS_STENCILPASS               
    D3DCMP_ALWAYS,              // D3DRS_STENCILFUNC               
    0,                          // D3DRS_STENCILREF                
    0xffffffff,                 // D3DRS_STENCILMASK               
    0xffffffff,                 // D3DRS_STENCILWRITEMASK          
    D3DBLENDOP_ADD,             // D3DRS_BLENDOP                   
    0,                          // D3DRS_BLENDCOLOR                
    D3DSWATH_128,               // D3DRS_SWATHWIDTH                
    0,                          // D3DRS_POLYGONOFFSETZSLOPESCALE  
    0,                          // D3DRS_POLYGONOFFSETZOFFSET      
    FALSE,                      // D3DRS_POINTOFFSETENABLE         
    FALSE,                      // D3DRS_WIREFRAMEOFFSETENABLE     
    FALSE,                      // D3DRS_SOLIDOFFSETENABLE         
    FALSE,                      // D3DRS_FOGENABLE                 
    D3DFOG_NONE,                // D3DRS_FOGTABLEMODE              
    0,                          // D3DRS_FOGSTART                  
    F_ONE,                      // D3DRS_FOGEND                    
    F_ONE,                      // D3DRS_FOGDENSITY                
    FALSE,                      // D3DRS_RANGEFOGENABLE            
    0,                          // D3DRS_WRAP0                     
    0,                          // D3DRS_WRAP1                     
    0,                          // D3DRS_WRAP2                     
    0,                          // D3DRS_WRAP3                     
    TRUE,                       // D3DRS_LIGHTING                  
    FALSE,                      // D3DRS_SPECULARENABLE            
    TRUE,                       // D3DRS_LOCALVIEWER               
    TRUE,                       // D3DRS_COLORVERTEX               
    D3DMCS_COLOR2,              // D3DRS_BACKSPECULARMATERIALSOURCE
    D3DMCS_COLOR1,              // D3DRS_BACKDIFFUSEMATERIALSOURCE 
    D3DMCS_MATERIAL,            // D3DRS_BACKAMBIENTMATERIALSOURCE 
    D3DMCS_MATERIAL,            // D3DRS_BACKEMISSIVEMATERIALSOURCE
    D3DMCS_COLOR2,              // D3DRS_SPECULARMATERIALSOURCE    
    D3DMCS_COLOR1,              // D3DRS_DIFFUSEMATERIALSOURCE     
    D3DMCS_MATERIAL,            // D3DRS_AMBIENTMATERIALSOURCE     
    D3DMCS_MATERIAL,            // D3DRS_EMISSIVEMATERIALSOURCE    
    0,                          // D3DRS_BACKAMBIENT               
    0,                          // D3DRS_AMBIENT                   
    F_ONE,                      // D3DRS_POINTSIZE                 
    0,                          // D3DRS_POINTSIZE_MIN             
    0,                          // D3DRS_POINTSPRITEENABLE         
    0,                          // D3DRS_POINTSCALEENABLE          
    F_ONE,                      // D3DRS_POINTSCALE_A              
    0,                          // D3DRS_POINTSCALE_B              
    0,                          // D3DRS_POINTSCALE_C              
    F_64,                       // D3DRS_POINTSIZE_MAX             
    D3DPATCHEDGE_DISCRETE,      // D3DRS_PATCHEDGESTYLE            
    F_ONE,                      // D3DRS_PATCHSEGMENTS             
    0xdeadbeef,                 // D3DRS_SWAPFILTER (already set)
    0,                          // D3DRS_PSTEXTUREMODES            
    D3DVBF_DISABLE,             // D3DRS_VERTEXBLEND               
    0,                          // D3DRS_FOGCOLOR                  
    D3DFILL_SOLID,              // D3DRS_FILLMODE                  
    D3DFILL_SOLID,              // D3DRS_BACKFILLMODE              
    FALSE,                      // D3DRS_TWOSIDEDLIGHTING          
    FALSE,                      // D3DRS_NORMALIZENORMALS          
    D3DZB_USEW,                 // D3DRS_ZENABLE (gets overwritten later)
    FALSE,                      // D3DRS_STENCILENABLE             
    D3DSTENCILOP_KEEP,          // D3DRS_STENCILFAIL               
    D3DFRONT_CW,                // D3DRS_FRONTFACE                 
    D3DCULL_CCW,                // D3DRS_CULLMODE                  
    0xffffffff,                 // D3DRS_TEXTUREFACTOR             
    0,                          // D3DRS_ZBIAS                     
    D3DLOGICOP_NONE,            // D3DRS_LOGICOP                   
    FALSE,                      // D3DRS_EDGEANTIALIAS             
    TRUE,                       // D3DRS_MULTISAMPLEANTIALIAS      
    0xffffffff,                 // D3DRS_MULTISAMPLEMASK           
    0xdeadbeef,                 // D3DRS_MULTISAMPLEMODE (already set)
    D3DMULTISAMPLEMODE_1X,      // D3DRS_MULTISAMPLEMODERENDERTARGETS
    D3DCMP_NEVER,               // D3DRS_SHADOWFUNC                
    F_ONE,                      // D3DRS_LINEWIDTH                 
    TRUE,                       // D3DRS_DXT1NOISEENABLE
    FALSE,                      // D3DRS_YUVENABLE
    TRUE,                       // D3DRS_OCCLUSIONCULLENABLE
    TRUE,                       // D3DRS_STENCILCULLENABLE
    FALSE,                      // D3DRS_ROPZCMPALWAYSREAD
    FALSE,                      // D3DRS_ROPZREAD
    FALSE,                      // D3DRS_DONOTCULLUNCOMPRESSED
};

//------------------------------------------------------------------------------
// g_InitialTextureStates

D3DCONST BYTE g_InitialTextureStates[] =
{
    D3DTADDRESS_WRAP,           // D3DTSS_ADDRESSU              
    D3DTADDRESS_WRAP,           // D3DTSS_ADDRESSV              
    D3DTADDRESS_WRAP,           // D3DTSS_ADDRESSW              
    D3DTEXF_POINT,              // D3DTSS_MAGFILTER             
    D3DTEXF_POINT,              // D3DTSS_MINFILTER             
    D3DTEXF_NONE,               // D3DTSS_MIPFILTER             
    0,                          // D3DTSS_MIPMAPLODBIAS         
    0,                          // D3DTSS_MAXMIPLEVEL           
    1,                          // D3DTSS_MAXANISOTROPY         
    D3DTCOLORKEYOP_DISABLE,     // D3DTSS_COLORKEYOP            
    0,                          // D3DTSS_COLORSIGN             
    D3DTALPHAKILL_DISABLE,      // D3DTSS_ALPHAKILL             
    D3DTOP_DISABLE,             // D3DTSS_COLOROP (gets overwritten later)
    D3DTA_CURRENT,              // D3DTSS_COLORARG0 
    D3DTA_TEXTURE,              // D3DTSS_COLORARG1             
    D3DTA_CURRENT,              // D3DTSS_COLORARG2             
    D3DTOP_DISABLE,             // D3DTSS_ALPHAOP (gets overwritten later)
    D3DTA_CURRENT,              // D3DTSS_ALPHAARG0             
    D3DTA_TEXTURE,              // D3DTSS_ALPHAARG1             
    D3DTA_CURRENT,              // D3DTSS_ALPHAARG2             
    D3DTA_CURRENT,              // D3DTSS_RESULTARG             
    D3DTTFF_DISABLE,            // D3DTSS_TEXTURETRANSFORMFLAGS 
    0,                          // D3DTSS_BUMPENVMAT00          
    0,                          // D3DTSS_BUMPENVMAT01          
    0,                          // D3DTSS_BUMPENVMAT11          
    0,                          // D3DTSS_BUMPENVMAT10          
    0,                          // D3DTSS_BUMPENVLSCALE         
    0,                          // D3DTSS_BUMPENVLOFFSET        
    0,                          // D3DTSS_TEXCOORDINDEX (gets overwritten later)
    0,                          // D3DTSS_BORDERCOLOR           
    0,                          // D3DTSS_COLORKEYCOLOR         
    0,                          // Unused
};

//------------------------------------------------------------------------------
// InitializeD3dState
//
// Initialize the D3D render, texture, and transform state to the defaults.

VOID InitializeD3dState()
{
    DWORD i;
    DWORD stage;

    CDevice* pDevice = g_pDevice;

    DBG_CHECK(VerifyHeaderFileEncodings());

    // Initialize the transforms.
    //
    // Note that the viewport is handled by the first SetRenderTarget call.

    for (i = 0; i < D3DTS_MAX; i++)
    {
        pDevice->SetTransform((D3DTRANSFORMSTATETYPE) i, &g_IdentityMatrix);
    }

    // Initialize the render states.
    //
    // The pixel shader state does not need to be initialized here.  The 
    // device state is initialized to 0 already and the pixel shader
    // registers don't get loaded until a pixel shader is set as active.

    ASSERT(ARRAYSIZE(g_InitialRenderStates) == (D3DRS_MAX - D3DRS_PS_MAX));

    for (i = D3DRS_PS_MAX; i < D3DRS_MAX; i++)
    {
        // D3DRS_MULTISAMPLEMODE and D3DRS_SWAPFILTER are already 
        // initialized at this point:

        if ((i != D3DRS_MULTISAMPLEMODE) && (i != D3DRS_SWAPFILTER))
        {
            pDevice->SetRenderState((D3DRENDERSTATETYPE) i, 
                                    g_InitialRenderStates[i - D3DRS_PS_MAX]);
        }
    }

    pDevice->SetRenderState(D3DRS_ZENABLE, pDevice->m_pAutoDepthBuffer != NULL); 

    // Initialize the texture stage states.

    ASSERT(ARRAYSIZE(g_InitialTextureStates) == D3DTSS_MAX);

    for (stage = 0; stage < D3DTSS_MAXSTAGES; stage++)
    {
        for (i = 0; i < D3DTSS_MAX; i++)
        {
            pDevice->SetTextureStageState(stage, 
                                          (D3DTEXTURESTAGESTATETYPE) i, 
                                          g_InitialTextureStates[i]);
        }

        pDevice->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, stage);
    }

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetScreenSpaceOffset(0, 0);
}

//------------------------------------------------------------------------------
// CDevice::Init
//
// This is called ONLY by CreateDevice().  It is NOT called by Reset().

extern DWORD g_AvInfo;

#if defined(PROFILE) && !defined(STARTUPANIMATION)

struct PERFCOUNTER
{
    CHAR* Name;
    DWORD Flags;
    VOID* Data;
};

D3DCONST PERFCOUNTER g_PerfCounter[] =
{
    { "(SwPut-HwGet)/PushBufferSize", DMCOUNT_VALUE | DMCOUNT_SYNC, PerfGetPushBufferDistance },
    { "Direct3D contiguous memory allocated", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &D3D__AllocsContiguous },
    { "Direct3D non-contiguous memory allocated", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &D3D__AllocsNoncontiguous }
};

#endif // PROFILE && !STARTUPANIMATION

HRESULT CDevice::Init(
    D3DPRESENT_PARAMETERS *pPresentationParams)
{
    HRESULT hr;
    DWORD i;

    if (DBG_CHECK(true))
    {
        hr = PresentationParametersParameterCheck(pPresentationParams);
        if (FAILED(hr))
            return hr;
    }

    // Calculate the amount of cached contiguous memory we need and allocate
    // it.

    DWORD SemaphoreSize = 32;
    DWORD NotifierSize = sizeof(NvNotification) * 4;  // 2 kelvin, 2 memcpy

    DWORD CachedSize = SemaphoreSize + NotifierSize;

    m_pCachedContiguousMemoryBase = 
        (BYTE *)MmAllocateContiguousMemoryEx(CachedSize,
                                             0,
                                             AGP_APERTURE_BYTES - 1,
                                             0,
                                             PAGE_READWRITE);

    if (!m_pCachedContiguousMemoryBase)
    {
        WARNING("CDevice::Init - unable to allocate notifier memory, Init failed.");
        return E_OUTOFMEMORY;
    }

    // Remember the semaphore location.

    m_pGpuTime = (volatile DWORD *)(m_pCachedContiguousMemoryBase);

    // Set up the notification buffers

    m_pKelvinNotifiers = (volatile NvNotification *)
        ((BYTE *)m_pGpuTime + SemaphoreSize);

    m_pMemCopyNotifiers = m_pKelvinNotifiers + 2;

    memset((void *)m_pKelvinNotifiers, 0, NotifierSize);

    // Initialize the push buffer

    hr = InitializePushBuffer();

    if (FAILED(hr)) 
    {
        return hr;    
    }

    // Make sure the very first DrawPrim initializes all the lazy state:

    D3D__DirtyFlags |= D3DDIRTYFLAG_REALLY_FILTHY;

    // Initialize miniport

    CMiniport* pMiniport = &m_Miniport;
    Nv20ControlDma *pControlDma;

    ULONG HostMemoryBase = 0;
    ULONG HostMemoryLimit = AGP_APERTURE_BYTES - INSTANCE_MEM_MAXSIZE - 1;

    CMiniport::OBJECTINFO InMemoryCtxDma;
    CMiniport::OBJECTINFO FromMemoryCtxDma;
    CMiniport::OBJECTINFO ToMemoryCtxDma;
    CMiniport::OBJECTINFO ColorCtxDma;
    CMiniport::OBJECTINFO ZetaCtxDma;
    CMiniport::OBJECTINFO CopyCtxDma;
    CMiniport::OBJECTINFO NotifierCtxDma;
    CMiniport::OBJECTINFO MemCopyNotifierCtxDma;
    CMiniport::OBJECTINFO CachedMemoryCtxDma;
    CMiniport::OBJECTINFO PusherCtxDma;
    CMiniport::OBJECTINFO SemaphoreCtxDma;
    CMiniport::OBJECTINFO KelvinObj;
    CMiniport::OBJECTINFO MemCopyObj;
    CMiniport::OBJECTINFO RectCopyObj;
    CMiniport::OBJECTINFO RectCopySurfacesObj;
    CMiniport::OBJECTINFO RectCopyPatternObj;
    CMiniport::OBJECTINFO RectCopyColorKeyObj;
    CMiniport::OBJECTINFO RectCopyRopObj;
    CMiniport::OBJECTINFO RectCopyBeta1Obj;
    CMiniport::OBJECTINFO RectCopyBeta4Obj;
    CMiniport::OBJECTINFO RectCopyClipRectangleObj;
    CMiniport::OBJECTINFO RectCopyNullObj;

    pMiniport->InitHardware();

    // Create DMA contexts for video memory

    pMiniport->CreateCtxDmaObject(NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &InMemoryCtxDma);

    pMiniport->CreateCtxDmaObject(NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY, 
                                  NV01_CONTEXT_DMA_FROM_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &FromMemoryCtxDma);

    pMiniport->CreateCtxDmaObject(NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY, 
                                  NV01_CONTEXT_DMA_TO_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &ToMemoryCtxDma);

    // Create DMA context for renderable buffers (these will be modified
    // on-the-fly on debug builds):

    pMiniport->CreateCtxDmaObject(D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &ColorCtxDma);

    m_ColorContextDmaInstance = ColorCtxDma.Instance;

    pMiniport->CreateCtxDmaObject(D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &ZetaCtxDma);

    m_ZetaContextDmaInstance = ZetaCtxDma.Instance;

    pMiniport->CreateCtxDmaObject(D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &CopyCtxDma);

    m_CopyContextDmaInstance = CopyCtxDma.Instance;

    // Create DMA context for notifications

    pMiniport->CreateCtxDmaObject(NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY, 
                                  NV01_CONTEXT_DMA_TO_MEMORY,
                                  (void *)m_pKelvinNotifiers,
                                  (2 * sizeof(NvNotification) - 1),
                                  &NotifierCtxDma);

    // Create DMA context for notifications

    pMiniport->CreateCtxDmaObject(D3D_MEMCOPY_NOTIFIER_CONTEXT_DMA_TO_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (void *)m_pMemCopyNotifiers,
                                  (2 * sizeof(NvNotification) - 1),
                                  &MemCopyNotifierCtxDma);

    // Create DMA context for cached memory.

    pMiniport->CreateCtxDmaObject(D3D_CONTEXT_IN_CACHED_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (void *)XMETAL_MEMORY_TYPE_CONTIGUOUS,
                                  MM_BYTES_IN_PHYSICAL_MAP,
                                  &CachedMemoryCtxDma);

    // Allocate context DMA for the semaphore

    pMiniport->CreateCtxDmaObject(D3D_SEMAPHORE_CONTEXT_DMA_IN_MEMORY, 
                                  NV01_CONTEXT_DMA_IN_MEMORY,
                                  (PVOID)((void *)m_pGpuTime),
                                  SemaphoreSize,
                                  &SemaphoreCtxDma);

    // Allocate context DMA for the push buffer

    pMiniport->CreateCtxDmaObject(NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 
                                  NV01_CONTEXT_DMA_FROM_MEMORY,
                                  (PVOID)HostMemoryBase, 
                                  HostMemoryLimit,
                                  &PusherCtxDma);

    // Initialize the DMA channel

    pMiniport->InitDMAChannel(NV20_CHANNEL_DMA, 
                              NULL, 
                              &PusherCtxDma,
                              0,
                              (PVOID*)&(pControlDma));

    // Bind context DMAs to the channel

    pMiniport->BindToChannel(&PusherCtxDma);
    pMiniport->BindToChannel(&CachedMemoryCtxDma);
    pMiniport->BindToChannel(&NotifierCtxDma);
    pMiniport->BindToChannel(&MemCopyNotifierCtxDma);
    pMiniport->BindToChannel(&ToMemoryCtxDma);
    pMiniport->BindToChannel(&FromMemoryCtxDma);
    pMiniport->BindToChannel(&InMemoryCtxDma);
    pMiniport->BindToChannel(&ColorCtxDma);
    pMiniport->BindToChannel(&ZetaCtxDma);
    pMiniport->BindToChannel(&CopyCtxDma);
    pMiniport->BindToChannel(&SemaphoreCtxDma);

    // Create our various objects

    pMiniport->CreateGrObject(D3D_KELVIN_PRIMITIVE,              NV20_KELVIN_PRIMITIVE,        &KelvinObj);
    pMiniport->CreateGrObject(D3D_MEMORY_TO_MEMORY_COPY,         NV03_MEMORY_TO_MEMORY_FORMAT, &MemCopyObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY,                NV15_IMAGE_BLIT,              &RectCopyObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_SURFACES,       NV10_CONTEXT_SURFACES_2D,     &RectCopySurfacesObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_PATTERN,        NV04_CONTEXT_PATTERN,         &RectCopyPatternObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_COLOR_KEY,      NV04_CONTEXT_COLOR_KEY,       &RectCopyColorKeyObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_ROP,            NV03_CONTEXT_ROP,             &RectCopyRopObj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_BETA1,          NV01_CONTEXT_BETA,            &RectCopyBeta1Obj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_BETA4,          NV04_CONTEXT_BETA,            &RectCopyBeta4Obj);
    pMiniport->CreateGrObject(D3D_RECTANGLE_COPY_CLIP_RECTANGLE, NV01_CONTEXT_CLIP_RECTANGLE,  &RectCopyClipRectangleObj);
    pMiniport->CreateGrObject(D3D_NULL_OBJECT,                   NV01_NULL,                    &RectCopyNullObj);

    // Remember the base address.

    m_NvBase = (HWREG *)pMiniport->m_RegisterBase;

    // Use the returned control DMA

    m_pControlDma = pControlDma;

    // This is exported to the DVD player:

    D3D__GpuReg = (volatile DWORD *)m_NvBase;

    // We need to get the control DMA to point to the beginning of
    // our push buffer instead of offset zero in its context DMA
    // (i.e. the beginning of memory).  Slip a jump into the first
    // 4 bytes of memory and run the pusher past it.  This memory
    // is mapped as MmCached, so we have to flush the processor's
    // cache to make this change visible to the GPU.

    PageZero()->m_PushBufferJump = 
        PUSHER_JUMP(GetGPUAddressFromWC((void*) m_pPushBase));

    __asm wbinvd;

    KickOff();

    INITDEADLOCKCHECK();

    while (GetPhysicalOffset((void*) HwGet()) 
        != GetPhysicalOffset((void*) m_Pusher.m_pPut))
    {
        DODEADLOCKCHECK();

        BusyLoop();
    }

    // Of course, the above will hide errors where we inadvertently cause
    // the pusher to jump back to zero.  Whack the memory back to an
    // invalid value (DEADBEEF works great).

    PageZero()->m_PushBufferJump = 0xDEADBEEF;

    // Initialize the hardware, load the classes and set up the push buffer.

    InitializeHardware();

    // Set up the frame buffers

    hr = InitializeFrameBuffers(pPresentationParams);

    if (FAILED(hr))
    {
        return hr;
    }

    pMiniport->SetVideoMode(pPresentationParams->BackBufferWidth, 
                            pPresentationParams->BackBufferHeight,
                            pPresentationParams->FullScreen_RefreshRateInHz,
                            pPresentationParams->Flags,
                            pPresentationParams->BackBufferFormat,
                            pPresentationParams->FullScreen_PresentationInterval,
                            PixelJar::GetPitch(m_pFrameBuffer[1]));

#ifdef STARTUPANIMATION

    DWORD DisplayMode = m_Miniport.m_DisplayMode;
    DWORD Format = m_Miniport.m_Format;
    DWORD Pitch = m_Miniport.m_SurfacePitch;
    void *RegisterBase = m_Miniport.m_RegisterBase;

    ULONG Step = 0;

    do
    {
        Step = AvSetDisplayMode(RegisterBase, Step, DisplayMode, Format, Pitch, NULL);
    }
    while (Step);

#endif

    // Initialize the vertex shader:

    m_pVertexShader = &g_FvfVertexShader;

    SetVertexShader(D3DFVF_XYZ);

    // Initialize the Direct vertex shader object

    ZeroMemory(&g_DirectVertexShader, sizeof(g_DirectVertexShader));
#if DBG
    g_DirectVertexShader.Signature = 'Vshd';
#endif
    g_DirectVertexShader.Flags = VERTEXSHADER_PROGRAM;


    // Set the render target.  

    SetRenderTarget(m_pFrameBuffer[0], m_pAutoDepthBuffer);

    // Initialize the rest of the D3D state to the default.

    InitializeD3dState();

    // Clear the Z and stencil buffers.  We do this mainly to clear stencil
    // to ensure that there's no random bits left in there which may muck
    // up the Z compression.
    //
    // Note that we don't bother with clearing the color buffer.

    Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

#ifndef STARTUPANIMATION

    // Tell the perf framework about our frame counter.  We use the 
    // VBlankFlipCount because it is incremented only when we do the
    // actual flip, even if the flip happens outside of the 
    // VBlank.

    PDMGD pdmgd = DmGetCurrentDmgd();
    if (pdmgd)
    {
        pdmgd->FrameCounter = &m_Miniport.m_VBlankFlipCount;
        pdmgd->FlipCounter  = &m_SwapCount;
        pdmgd->Surface      = m_pFrameBuffer[1];
        pdmgd->RegisterBase = &m_Miniport.m_RegisterBase;
        pdmgd->pdwOpcode    = &m_dwOpcode;
        pdmgd->ppSnapshotBuffer = (BYTE **)&m_pShaderCaptureBuffer;
    }

#if PROFILE

    for (i = 0; i < ARRAYSIZE(g_PerfCounter); i++)
    {
        DmTell_RegisterPerformanceCounter(g_PerfCounter[i].Name,
                                          g_PerfCounter[i].Flags,
                                          g_PerfCounter[i].Data);
    }

#endif // PROFILE

#endif // !STARTUPANIMATION

    // Reset the flicker filter and soft display filter.

    D3DDevice_SetFlickerFilter(5);
    D3DDevice_SetSoftDisplayFilter(FALSE);

    return S_OK;
} 

//------------------------------------------------------------------------------
// CDevice::UnInit

VOID CDevice::UnInit()
{
    DWORD i;

    ASSERT(!(m_StateFlags & (STATE_RECORDPUSHBUFFER)));

    if (m_Pusher.m_pPut)
    {
        // Make sure that the GPU has processed everything except for
        // this final little bit.  Otherwise the KickOffAndWaitForIdle
        // code will get confused by the jump back to the beginning.
    
        KickOffAndWaitForIdle();
    
        // Point GPU to the start of the push buffer so we can reset the push
        // buffer member variables in the next Init.
    
        *m_Pusher.m_pPut = PUSHER_JUMP(GetGPUAddressFromWC((void*) m_pPushBase));
    
        m_Pusher.m_pPut = m_pPushBase;
    
        KickOff();
    
        // We need to wait until the hardware is idle before we can destroy the
        // device.
    
        INITDEADLOCKCHECK();

        while (GetPhysicalOffset((void*) HwGet()) 
            != GetPhysicalOffset((void*) m_pPushBase))
        {
            BusyLoop();
        
            DODEADLOCKCHECK();
        }
    }

    // Wait for any last synchronized flip to get processed, in order to
    // make sure that the final frame is displayed, and so that the flip
    // synchronization code doesn't get messed up by the INCREMENT_READ_3D
    // done at the Vblank interrupt.

    while (m_Miniport.IsFlipPending())
        ;

#if PROFILE
    // Tell the perf framework that our frame counter is gone.

    PDMGD pdmgd = DmGetCurrentDmgd();
    if (pdmgd)
    {
        pdmgd->FrameCounter = NULL;
        pdmgd->FlipCounter  = NULL;
        pdmgd->Surface      = NULL;
    }

#ifndef STARTUPANIMATION

    for (i = 0; i < ARRAYSIZE(g_PerfCounter); i++)
    {
        DmTell_UnregisterPerformanceCounter(g_PerfCounter[i].Name);
    }

#endif // !STARTUPANIMATION

#endif // PROFILE

    // First, free all stuff that would also be freed if we did a Reset()
    // call:

    FreeFrameBuffers();

    // FreeFrameBuffers() should take care of the following:

    ASSERT((m_pRenderTarget == NULL) && (m_pZBuffer == NULL));

    // Free all the other referenced objects:

    UninitializePushBuffer();

    if (m_pIndexBuffer)
    {
        InternalRelease(m_pIndexBuffer);
    }

    for (i = 0; i < D3DTSS_MAXSTAGES; i++)
    {
        if (m_Textures[i])
        {
            InternalRelease(m_Textures[i]);
        }
        if (m_Palettes[i])
        {
            InternalRelease(m_Palettes[i]);
        }
    }

    for (i = 0; i < 16; i++)
    {
        if (g_Stream[i].pVertexBuffer)
        {
            InternalRelease(g_Stream[i].pVertexBuffer);
        }
    }

    for (i = 0; i < REPORTS_ALLOCATIONS_MAX; i++)
    {
        if (m_ReportAllocations[i])
        {
            MmFreeContiguousMemory(m_ReportAllocations[i]);
        }
    }

    if (m_pCachedContiguousMemoryBase)
    {
        MmFreeContiguousMemory(m_pCachedContiguousMemoryBase);
    }

    if (m_pLights)
    {
        MemFree(m_pLights);
    }

    // Disable interrupts and shutdown engines

    if (m_Miniport.m_RegisterBase)
    {
        m_Miniport.DisableInterrupts(m_Miniport.m_RegisterBase);
        m_Miniport.ShutdownEngines();
    }

    // Zero-initialize g_Stream for next time.  

    ZeroMemory(&g_Stream, sizeof(g_Stream));
}

//------------------------------------------------------------------------------
// Frees the frame and back buffers
//
// This can get called at either 'Init' or 'Reset' time. 

VOID CDevice::FreeFrameBuffers()
{
    DWORD i;

    if (!AvGetSavedDataAddress())
    {
        AvSendTVEncoderOption(m_Miniport.m_RegisterBase, 
                              AV_OPTION_BLANK_SCREEN, 
                              TRUE, 
                              NULL);
    }

    if (m_pRenderTarget)
    {
        RecordSurfaceWritePush(m_pRenderTarget);
        InternalReleaseSurface(m_pRenderTarget);
        m_pRenderTarget = NULL;
    }

    if (m_pZBuffer)
    {
        RecordSurfaceWritePush(m_pZBuffer);
        InternalReleaseSurface(m_pZBuffer);
        m_pZBuffer = NULL;
    }

    for (i = 0; i < m_FrameBufferCount; i++)
    {
        m_pFrameBuffer[i]->Release();
        m_pFrameBuffer[i] = NULL;
    }

    if (m_pAutoDepthBuffer != NULL)
    {
        m_pAutoDepthBuffer->Release();
        m_pAutoDepthBuffer = NULL;
    }

    if (m_pAutoDepthBufferBase)
    {
        SetTile(TILE_ZBUFFER, NULL);

        FreeContiguousMemory(m_pAutoDepthBufferBase);
        m_pAutoDepthBufferBase = NULL;
    }

    if (m_pFrameBufferBase)
    {
        SetTile(TILE_FRAMEBUFFER, NULL);

        FreeContiguousMemory(m_pFrameBufferBase);
        m_pFrameBufferBase = NULL;
    }

    if (m_pAntiAliasBufferBase)
    {
        FreeContiguousMemory(m_pAntiAliasBufferBase);
        m_pAntiAliasBufferBase = NULL;
    }
}

//------------------------------------------------------------------------------
// PresentationParametersParameterCheck

HRESULT PresentationParametersParameterCheck(
    D3DPRESENT_PARAMETERS* pPresentationParams)
{
    DWORD i;

    DWORD sampleType = pPresentationParams->MultiSampleType;
    if ((sampleType != D3DMULTISAMPLE_NONE) && (sampleType != 0))
    {
        if ((sampleType & ANTIALIAS_FORMAT_MASK) > 
                            D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8)
        {
            DPF_ERR("Invalid D3DMULTISAMPLE_PREFILTER_FORMAT type");
            return D3DERR_INVALIDCALL;
        }

        if (!(sampleType & ANTIALIAS_ACTIVE) &&
            (sampleType & ANTIALIAS_FORMAT_MASK))
        {
            DPF_ERR("Can't specifcy D3DMULTISAMPLE_PREFILTER_FORMAT if not "
                    "antialiasing");
            return D3DERR_INVALIDCALL;
        }

        if ((ANTIALIAS_XSCALE(sampleType) < 1) ||
            (ANTIALIAS_YSCALE(sampleType) < 1) ||
            (ANTIALIAS_XSCALE(sampleType) > 3) ||
            (ANTIALIAS_YSCALE(sampleType) > 3))
        {
            DPF_ERR("Improper D3DMULTISAMPLE scale encoding");
            return D3DERR_INVALIDCALL;
        }
    }

    if ((pPresentationParams->FullScreen_PresentationInterval &
         ~D3DPRESENT_INTERVAL_IMMEDIATE) > D3DPRESENT_INTERVAL_TWO)
    {
        DPF_ERR("Invalid FullScreen_PresentationInterval value");
        return D3DERR_INVALIDCALL;
    }

    if (pPresentationParams->FullScreen_RefreshRateInHz != 0
        && pPresentationParams->FullScreen_RefreshRateInHz != 50
        && pPresentationParams->FullScreen_RefreshRateInHz != 60)
    {
        DPF_ERR("FullScreen_RefreshRateInHz must be zero, 50 or 60");
    }

    if (pPresentationParams->SwapEffect != D3DSWAPEFFECT_DISCARD &&
        pPresentationParams->SwapEffect != D3DSWAPEFFECT_FLIP &&
        pPresentationParams->SwapEffect != D3DSWAPEFFECT_COPY &&
        pPresentationParams->SwapEffect != 0)
    {
        DPF_ERR("Invalid SwapEffect value");
        return D3DERR_INVALIDCALL;
    }

    if (pPresentationParams->BackBufferCount > 2)
    {
        DPF_ERR("No point in more than 2 back buffers, due to throttling");
        return D3DERR_INVALIDCALL;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Creates the frame and back buffers.
//
// Note: This expects Kelvin to have been initialized.

HRESULT CDevice::InitializeFrameBuffers(
    D3DPRESENT_PARAMETERS* pPresentationParams)
{
    DWORD format;
    DWORD size;
    D3DTILE tile;
    DWORD i;

    // A 'BackBufferCount' of zero is to be treated as one:

    DWORD backBufferCount = max(1, pPresentationParams->BackBufferCount);

    // Map any swizzled frame buffer formats to their linear equivalent.  
    // This is a concession to how people use D3D today even though it is a
    // lie in this driver.

    D3DFORMAT postfilterFormat 
        = MapToLinearD3DFORMAT(pPresentationParams->BackBufferFormat);

    D3DFORMAT autoDepthStencilFormat
        = MapToLinearD3DFORMAT(pPresentationParams->AutoDepthStencilFormat);

    DWORD backBufferWidth = pPresentationParams->BackBufferWidth;

    DWORD backBufferHeight = pPresentationParams->BackBufferHeight;

    // Save some other state:

    m_PresentationInterval = pPresentationParams->FullScreen_PresentationInterval;

    m_SwapEffect = pPresentationParams->SwapEffect;

    m_FrameBufferCount = backBufferCount + 1;

    // This routine can get called on Reset(), so clear here any flags that
    // we might set in this routine:

    m_StateFlags &= ~STATE_COPYSWAP;

    // We treat 'D3DSWAPEFFECT_COPY' as 1x antialiasing:

    BOOL doAntiAlias = FALSE;

    if ((pPresentationParams->MultiSampleType & ANTIALIAS_ACTIVE) ||
        (pPresentationParams->SwapEffect == D3DSWAPEFFECT_COPY))
    {
        doAntiAlias = TRUE;
        m_StateFlags |= STATE_COPYSWAP;
    }

    DWORD presentSampleType = pPresentationParams->MultiSampleType;
    if (presentSampleType == 0)
        presentSampleType = ANTIALIAS_NONE;

    m_MultiSampleType = presentSampleType;

    DWORD xScale = ANTIALIAS_XSCALE(presentSampleType);
    DWORD yScale = ANTIALIAS_YSCALE(presentSampleType);

    m_AntiAliasScaleX = (FLOAT) xScale;
    m_AntiAliasScaleY = (FLOAT) yScale;

    // Set some render states that are dependent on the presentation
    // parameters:

    DWORD typeRenderState = D3DMULTISAMPLEMODE_1X;
    if (presentSampleType & ANTIALIAS_MULTISAMPLE)
    {
        typeRenderState = D3DMULTISAMPLEMODE_4X;
        if ((presentSampleType & ANTIALIAS_SCALE_MASK) == 0x21)
        {
            typeRenderState = D3DMULTISAMPLEMODE_2X;
        }
    }

    DWORD filterRenderState = D3DTEXF_LINEAR;
    if (presentSampleType & ANTIALIAS_QUINCUNX)
        filterRenderState = D3DTEXF_QUINCUNX;
    else if (presentSampleType & ANTIALIAS_GAUSSIAN)
        filterRenderState = D3DTEXF_GAUSSIANCUBIC;

    D3D__RenderState[D3DRS_MULTISAMPLEMODE] = typeRenderState;
    D3D__RenderState[D3DRS_SWAPFILTER] = filterRenderState;

    if (pPresentationParams->BufferSurfaces[0] != NULL)
    {
        // The caller has chosen to pre-create their own color- and Z-buffers.
        //
        // Note that we'll eventually call SetRenderTarget, which does more
        // validation than what we do here.

        if (DBG_CHECK(TRUE))
        {
            if ((pPresentationParams->BufferSurfaces[1] == NULL) ||
                (pPresentationParams->BufferSurfaces[backBufferCount] == NULL))
            {
                DPF_ERR("Unexpected NULL BufferSurfaces surface");
            }
            for (i = doAntiAlias; i < backBufferCount; i++)
            {
                if (pPresentationParams->BufferSurfaces[i]->Size !=
                    pPresentationParams->BufferSurfaces[i+1]->Size)
                {
                    DPF_ERR("Not all BufferSurfaces have same dimension or "
                            "stride");
                }
                if (pPresentationParams->BufferSurfaces[i]->Format !=
                    pPresentationParams->BufferSurfaces[i+1]->Format)
                {
                    DPF_ERR("Not all BufferSurfaces have same format");
                }
            }
            if (PixelJar::GetFormat(pPresentationParams->BufferSurfaces[1])
                 != pPresentationParams->BackBufferFormat)
            {
                DPF_ERR("Unexpected mistmatch between BufferSurfaces and "
                        "BackBufferFormat (make sure BackBufferFormat\n"
                        "is correct)");
            }
            if ((PixelJar::GetWidth(pPresentationParams->BufferSurfaces[1])
                 < pPresentationParams->BackBufferWidth) ||
                (PixelJar::GetHeight(pPresentationParams->BufferSurfaces[1])
                 < pPresentationParams->BackBufferHeight))
            {
                DPF_ERR("Front-buffer surfaces must be at least as large "
                        "as BackBufferWidth/Height");
            }
            if (pPresentationParams->DepthStencilSurface)
            {
                if ((PixelJar::GetWidth(pPresentationParams->BufferSurfaces[0]) >
                     PixelJar::GetWidth(pPresentationParams->DepthStencilSurface)) ||
                    (PixelJar::GetHeight(pPresentationParams->BufferSurfaces[0]) >
                     PixelJar::GetHeight(pPresentationParams->DepthStencilSurface)))
                {
                    DPF_ERR("DepthStencilSurface must be at least as big as "
                            "BufferSurfaces");
                }
            }
        }

        // Initialize our pointers to the specified surfaces.  We'll add
        // reference counts in a bit...

        for (i = 0; i < backBufferCount + 1; i++)
        {
            m_BufferSurfaces[i] = *pPresentationParams->BufferSurfaces[i];
            m_pFrameBuffer[i] = &m_BufferSurfaces[i];
        }
        if (pPresentationParams->DepthStencilSurface != NULL)
        {
            m_DepthStencilSurface = *pPresentationParams->DepthStencilSurface;
            m_pAutoDepthBuffer = &m_DepthStencilSurface;
        }
    }
    else
    {
        // We have to allocate the buffers ourselves.

        // The caller can override the format of the pre-filter buffer, which
        // defaults to the same format as the post-filter buffer:
    
        D3DFORMAT prefilterFormat = postfilterFormat;
    
        switch (pPresentationParams->MultiSampleType & ANTIALIAS_FORMAT_MASK)
        {
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5:
            prefilterFormat = D3DFMT_LIN_X1R5G5B5;
            break;
    
        case D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5:
            prefilterFormat = D3DFMT_LIN_R5G6B5;
            break;
    
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8:
            prefilterFormat = D3DFMT_LIN_X8R8G8B8;
            break;
    
        case D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8:
            prefilterFormat = D3DFMT_LIN_A8R8G8B8;
            break;
        }
    
        DWORD width = xScale * backBufferWidth;
        DWORD height = yScale * backBufferHeight;
        DWORD pitch = CalcTilePitch(width, prefilterFormat);
    
        // Allocate the memory for our frame buffer and set up the frame-/back-
        // buffer surfaces.
    
        DWORD frameSize = PixelJar::EncodeFormat(width,
                                                 height,
                                                 1,          // Depth
                                                 1,          // Levels
                                                 prefilterFormat,
                                                 pitch,
                                                 false,      // isCubeMap
                                                 false,      // isVolume
                                                 &format,
                                                 &size);
    
        // Create only one buffer at the expanded, pre-filter size:
    
        DWORD frameCount = (doAntiAlias) ? 1 : (backBufferCount + 1);
            
        // The large 0x200 pitch alignment we gave to EncodeFormat should 
        // ensure that all surfaces are aligned to a 128 byte boundary:
    
        ASSERT((frameSize & (D3D_RENDER_MEMORY_ALIGNMENT - 1)) == 0);
    
        DWORD allocationSize = frameSize * frameCount;
    
        // Size must aligned on D3D_TILED_SURFACE_ALIGNMENT boundary as well.
    
        allocationSize = (allocationSize + D3D_TILED_SURFACE_ALIGNMENT - 1) 
                       & ~(D3D_TILED_SURFACE_ALIGNMENT - 1);
    
        BYTE* pFrame = (BYTE*) AllocateContiguousMemory(allocationSize,
                                                        D3D_TILED_SURFACE_ALIGNMENT);
        if (!pFrame)
        {
            WARNING("InitializeFrameBuffers - unable to allocate pre-filter buffer");
            return E_OUTOFMEMORY;
        }
    
        m_pFrameBufferBase = pFrame;
    
        for (i = 0; i < frameCount; i++)
        {
            m_pFrameBuffer[i] = &m_BufferSurfaces[i];

            InitializeSurface(m_pFrameBuffer[i], format, size, pFrame);
    
            pFrame += frameSize;
        }
    
        if (width != 1920)
        {
            tile.Flags = 0;
            tile.pMemory = m_pFrameBufferBase;
            tile.Size = allocationSize;
            tile.Pitch = PixelJar::GetPitch(m_pFrameBuffer[0]);
        
            SetTile(TILE_FRAMEBUFFER, &tile);
        }

        // Okay, now create the smaller, post-filter buffers.  Note that we
        // don't waste tiles on these, since they're only used once per frame
        // and they have different strides from the pre-filter buffer (although
        // they could be packed together...)

        if (doAntiAlias)
        {
            frameSize = PixelJar::EncodeFormat(backBufferWidth,
                                               backBufferHeight,
                                               1,          // Depth
                                               1,          // Levels
                                               postfilterFormat,
                                               0,
                                               false,      // isCubeMap
                                               false,      // isVolume
                                               &format,
                                               &size);

            // The large 0x200 pitch alignment we gave to EncodeFormat should 
            // ensure that all surfaces are aligned to a 128 byte boundary:

            ASSERT((frameSize & (D3D_RENDER_MEMORY_ALIGNMENT - 1)) == 0);

            DWORD allocationSize = frameSize * backBufferCount;

            // Note that we're not locating the antialiasing back buffers in 
            // a tile, so it doesn't need the more austere tile alignment 
            // restriction:

            BYTE *pFrame = (BYTE *)AllocateContiguousMemory(allocationSize,
                                                            D3D_TILED_SURFACE_ALIGNMENT);
            if (!pFrame)
            {
                WARNING("InitializeFrameBuffers - unable to allocate pre-filter buffer");
                return E_OUTOFMEMORY;
            }

            m_pAntiAliasBufferBase = pFrame;

            for (i = 1; i <= backBufferCount; i++)
            {
                m_pFrameBuffer[i] = &m_BufferSurfaces[i];

                InitializeSurface(m_pFrameBuffer[i], format, size, pFrame);

                pFrame += frameSize;
            }
        }

        // Set up the memory/surface for the auto-stencil buffer.
    
        if (pPresentationParams->EnableAutoDepthStencil)
        {
            pitch = CalcTilePitch(width, autoDepthStencilFormat);
    
            DWORD depthSize = PixelJar::EncodeFormat(width,
                                                     height,
                                                     1,
                                                     1,
                                                     autoDepthStencilFormat,
                                                     pitch,
                                                     false, 
                                                     false,
                                                     &format,
                                                     &size);
    
            depthSize = (depthSize + D3D_TILED_SURFACE_ALIGNMENT - 1) 
                      & ~(D3D_TILED_SURFACE_ALIGNMENT - 1);
    
            BYTE *pDepthBuffer 
                = (BYTE *)AllocateContiguousMemory(depthSize, 
                                                   D3D_TILED_SURFACE_ALIGNMENT);
    
            if (!pDepthBuffer)
            {
                WARNING("InitializeFrameBuffers - unable to allocate Z buffer");
                return E_OUTOFMEMORY;
            }
    
            m_pAutoDepthBufferBase = pDepthBuffer;

            m_pAutoDepthBuffer = &m_DepthStencilSurface;
    
            InitializeSurface(m_pAutoDepthBuffer, format, size, pDepthBuffer);
    
            // For the default auto-created depth buffer, we always automatically
            // create a compressed Z-buffer tile.  We also always start it at
            // tag 'zero'.  It's the caller's responsibility, if they're using
            // SetTile, to ensure that they've left enough tag bits starting at
            // zero if they Reset into a new mode.
    
            if (width != 1920)
            {
                tile.Flags = D3DTILE_FLAGS_ZBUFFER | D3DTILE_FLAGS_ZCOMPRESS;
                tile.pMemory = pDepthBuffer;
                tile.Size = depthSize;
                tile.Pitch = PixelJar::GetPitch(m_pAutoDepthBuffer);
                tile.ZOffset = 0;
                tile.ZStartTag = 0;
    
                if (BitsPerPixelOfD3DFORMAT(autoDepthStencilFormat) == 32)
                {
                    tile.Flags |= D3DTILE_FLAGS_Z32BITS;
                }
    
                SetTile(TILE_ZBUFFER, &tile);
            }
        }
    }

    // For surfaces supplied by the title, make sure we have a reference count 
    // on each:

    for (i = 0; i < m_FrameBufferCount; i++)
    {
        m_pFrameBuffer[i]->AddRef();
    }

    if (m_pAutoDepthBuffer != NULL)
    {
        m_pAutoDepthBuffer->AddRef();
    }

    PPUSH pPush = StartPush();

    // Inialize flip read/write values.  
    //
    // Set NV097_SET_FLIP_READ, NV097_SET_FLIP_WRITE, and NV097_SET_FLIP_MODULO
    // all in one call:

    Push3(pPush, NV097_SET_FLIP_READ, 0, 1, m_FrameBufferCount);

    EndPush(pPush + 4);

    return S_OK;
}

} // end of namespace

