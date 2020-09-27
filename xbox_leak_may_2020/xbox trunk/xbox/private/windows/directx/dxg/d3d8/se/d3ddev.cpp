/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3ddev.cpp
 *  Content:    Direct3D device implementation
 *
 ***************************************************************************/
 
#include "precomp.hpp"
#include "xboxverp.h"

// Push buffer global object.  This will always have the same value as
// g_pDevice but we want to expose it as a different type and the
// compiler is not at all happy with that.
//
XMETAL_PushBuffer *g_pPushBuffer;

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//----------------------------------------------------------------------------
// Global device object.

CDevice* g_pDevice;

CDevice g_Device;

//----------------------------------------------------------------------------
// Direct3D_CreateDevice

extern "C"
HRESULT WINAPI Direct3D_CreateDevice(
    UINT Adapter, 
    D3DDEVTYPE DeviceType, 
    void *pUnused, 
    DWORD Flags, 
    D3DPRESENT_PARAMETERS *pPresentationParams, 
    D3DDevice **ppNewInterface)
{
    COUNT_API(API_DIRECT3D_CREATEDEVICE);

    HRESULT ret;

    // DOC: No multithreading support (D3DCREATE_MULTI_THREADED)
    // DOC: No FPU preserve support (D3DCREATE_FPU_PRESERVE)
    // DOC: D3DSWAPEFFECT_DISCARD does no enforcement on debug builds

    if (DBG_CHECK(TRUE))
    {
        if (NULL == pPresentationParams)
        {
            DPF_ERR("Invalid D3DPRESENT_PARAMETERS pointer, CreateDevice fails");
        }
    
        if (pPresentationParams->BackBufferFormat == D3DFMT_UNKNOWN)
        {
            DPF_ERR("D3DFORMAT_UNKNOWN not allowed on Xbox");
        }
    
        if (Adapter > 1)
        {
            DPF_ERR("Invalid Adapter parameter passed to CreateDevice");
        }

        if (Flags & ~(0x800000FFL))
        {
            DPF_ERR("Invalid BehaviorFlags passed to CreateDevice");
        }
    
        if (pPresentationParams->Windowed != FALSE)
        {
            DPF_ERR("Windowed must be FALSE");
        }
    
        if ((Flags & (D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING)) != 0)
        {
            D3D_ERR("Only hardware vertex processing supported");
        }
    
        if (DeviceType != D3DDEVTYPE_HAL)
        {
            D3D_ERR("Only HAL devices supported");
        }

        if (!IsValidRenderTargetD3DFORMAT(pPresentationParams->BackBufferFormat))
        {
            D3D_ERR("Only D3DFMT_A8R8G8B8, _X8R8G8B8, _R5G6B5, _X1R5G5B5 (and _LIN versions) supported");
        }

        if (g_pDevice != NULL)
        {
            D3D_ERR("Can't create more than one D3D device");
        }
    }    

    // If the push buffer size was not set by the title then use our defaults.

    if (!CDevice::m_PushBufferSize)
    {
        CDevice::m_PushBufferSize = DEFAULT_PUSH_BUFFER_SIZE;
    }
    
    if (!CDevice::m_PushSegmentSize)
    {
        CDevice::m_PushSegmentSize = DEFAULT_PUSH_BUFFER_SEGMENT_SIZE;
    }

    CDevice* pDevice = &g_Device;

    g_pDevice = pDevice;
    g_pPushBuffer = &g_pDevice->m_Pusher;

    // Do not do this inside of Init because that method is also called
    // from Reset.

    pDevice->m_cRefs = 1;

    ASSERT(STATE_PUREDEVICE == D3DCREATE_PUREDEVICE);

    pDevice->m_StateFlags |= (Flags & STATE_PUREDEVICE);

    // Handle stuff common to both CreateDevice() and Reset():

    ret = pDevice->Init(pPresentationParams);

    if (FAILED(ret))
    {
        WARNING("FATAL: Failed to initialize device object");

        pDevice->UnInit();

        if (ppNewInterface)
        {
            *ppNewInterface = NULL;
        }

        // Zero out the global device and push buffer pointers and the device object

        g_pPushBuffer = NULL;
        g_pDevice = NULL;
        ZeroMemory(pDevice, sizeof(*pDevice));

        return ret;
    }

    // Don't sully our call statistics with what we've set here:

    CLEAR_STATISTICS();

    if (ppNewInterface)
    {
        *ppNewInterface = pDevice;
    }

    return S_OK;
}

//----------------------------------------------------------------------------
// Create the push buffer and set the push buffer segment count.  This must
// be called before calling CreateDevice.

extern "C"
void WINAPI Direct3D_SetPushBufferSize(
    DWORD PushBufferSize, 
    DWORD KickOffSize)
{
    COUNT_API(API_DIRECT3D_SETPUSHBUFFERSIZE);

    if (DBG_CHECK(TRUE))
    {
        if (g_pDevice)
        {
            DXGRIP("Direct3D_SetPushBufferSize - Called when there is an active device.");
        }

        if (KickOffSize & (KickOffSize - 1))
        {
            DXGRIP("Direct3D_SetPushBufferSize - The KickOffSize must be a power of 2.");
        }

        // This size restriction is somewhat arbitrary.  

        if (PushBufferSize < 64 * 1024)
        {
            DXGRIP("Direct3D_SetPushBufferSize - The PushBufferSize must not be less than 64 KB.");
        }

        // The KickOffSize must be more than PUSHER_THRESHOLD_SIZE_PLUS_OVERHEAD.
        // So we somewhat arbitrarily restrict it to being not less than 2048.

        if (KickOffSize < 2048)
        {
            DXGRIP("Direct3D_SetPushBufferSize - KickOffSize must be no less than 2048 bytes.");
        }

        if (PushBufferSize % KickOffSize)
        {
            DXGRIP("Direct3D_SetPushBufferSize - The PushBufferSize must be a multiple of the KickOffSize.");
        }

        if (PushBufferSize / KickOffSize < 4)
        {
            DXGRIP("Direct3D_SetPushBufferSize - A push buffer must have at least 4 segments.");
        }
    }

    CDevice::m_PushBufferSize = PushBufferSize;
    CDevice::m_PushSegmentSize = KickOffSize;
}

} // end namespace
