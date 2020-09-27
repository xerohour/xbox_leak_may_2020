/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       enum.cpp
 *  Content     Handles all of the enum functions for determing what device
 *              you want before you go there.
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

D3DCONST D3DADAPTER_IDENTIFIER8 g_AdapterIdentifier = 
{
    "Xbox driver",
    "Xbox NV2A",
    D3D_SDK_VERSION
};

//------------------------------------------------------------------------------
// CheckDisplayFormat
//

inline BOOL CheckDisplayFormat(D3DFORMAT format)
{
    return IsValidRenderTargetD3DFORMAT(format);
}

//------------------------------------------------------------------------------
// Direct3DCreate8
//
// DLL exposed Creation function

extern "C"
Direct3D * WINAPI Direct3DCreate8(
    UINT SDKVersion)
{
    if (DBG_CHECK(SDKVersion != D3D_SDK_VERSION))
    {
        DXGRIP("\nD3D ERROR: This application compiled against improper D3D headers.\n"
             "D3D_SDK_VERSION is (%d) but should be (%d).\n"
             "Please recompile with an up-to-date SDK.\n\n",
           SDKVersion, D3D_SDK_VERSION);

        return NULL;
    }

    // This pointer is never used for anything, but we can't return 
    // NULL because that means this API failed.  Just return something
    // simple.
    //
    return (Direct3D *)(1);
} 

//------------------------------------------------------------------------------
// Direct3D_GetAdapterIdentifier

extern "C"
HRESULT WINAPI Direct3D_GetAdapterIdentifier(
    UINT Adapter,
    DWORD Flags,
    D3DADAPTER_IDENTIFIER8* pIdentifier) 
{ 
    COUNT_API(API_DIRECT3D_GETADAPTERIDENTIFIER);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    *pIdentifier = g_AdapterIdentifier;

    return S_OK;
}

//------------------------------------------------------------------------------
// Direct3D_GetAdapterModeCount

extern "C"
UINT WINAPI Direct3D_GetAdapterModeCount(
    UINT Adapter) 
{ 
    COUNT_API(API_DIRECT3D_GETADAPTERMODECOUNT);

    UINT Count = 0;

    const DISPLAYMODE *pMode = GetTableForCurrentAvInfo();

    DWORD SystemAvInfo = CMiniport::GetDisplayCapabilities();

    ULONG ModeAvPack    = pMode->AvInfo & AV_PACK_MASK;
    ULONG AvPack        = SystemAvInfo & AV_PACK_MASK;
    ULONG AvStandard    = SystemAvInfo & AV_STANDARD_MASK;
    ULONG AvRefresh     = SystemAvInfo & AV_REFRESH_MASK;
    ULONG AvHDTVModes   = SystemAvInfo & AV_HDTV_MODE_MASK;

    // Count the modes that meet our requirements.
    for (;; pMode++)
    {
        // Did we leave this AV pack?
        if ((pMode->AvInfo & AV_PACK_MASK) != ModeAvPack)
        {
            break;
        }

        // Throw away widescreen if we don't support it unless this is an 
        // HDTV mode that has implied widescreen support.
        //
        if ((AvPack != AV_PACK_HDTV || !(pMode->AvInfo & (AV_FLAGS_HDTV_720p | AV_FLAGS_HDTV_1080i)))
            && (pMode->AvInfo & AV_FLAGS_WIDESCREEN)
            && !(SystemAvInfo & AV_FLAGS_WIDESCREEN))
        {
            continue;
        }

        // Throw away any mode that uses a refresh rate the display doesn't
        // support.
        //
        if (!(pMode->AvInfo & AvRefresh))
        {
            continue;
        }

        // Throw away any HDTV mode that this display does not
        // support.
        //
        if (AvPack == AV_PACK_HDTV
            && !(pMode->AvInfo & AvHDTVModes)
            && (pMode->AvInfo & AV_HDTV_MODE_MASK) != 0)
        {
            continue;
        }

        Count++;        
    }

    // There are 4 valid formats for each mode.
    return Count * 4;
}

//------------------------------------------------------------------------------
// Direct3D_EnumAdapterModes

extern "C"
HRESULT WINAPI Direct3D_EnumAdapterModes(
    UINT Adapter,
    UINT Mode,
    D3DDISPLAYMODE* pModeRet) 
{ 
    COUNT_API(API_DIRECT3D_ENUMADAPTERMODES);

    DWORD Format = Mode % 4;
    Mode /= 4;

    const DISPLAYMODE *pMode = GetTableForCurrentAvInfo();

    DWORD SystemAvInfo = CMiniport::GetDisplayCapabilities();

    ULONG ModeAvPack    = pMode->AvInfo & AV_PACK_MASK;
    ULONG AvPack        = SystemAvInfo & AV_PACK_MASK;
    ULONG AvStandard    = SystemAvInfo & AV_STANDARD_MASK;
    ULONG AvRefresh     = SystemAvInfo & AV_REFRESH_MASK;
    ULONG AvHDTVModes   = SystemAvInfo & AV_HDTV_MODE_MASK;

    // Find the requested mode.
    for (;; pMode++)
    {
        // Did we leave this AV pack?
        if ((pMode->AvInfo & AV_PACK_MASK) != ModeAvPack)
        {
            DXGRIP(("Direct3D_EnumAdapterModes - Bad mode index."));
            return D3DERR_INVALIDCALL;
        }

        // Throw away widescreen if we don't support it unless this is an 
        // HDTV mode that has implied widescreen support.
        //
        if ((AvPack != AV_PACK_HDTV || !(pMode->AvInfo & (AV_FLAGS_HDTV_720p | AV_FLAGS_HDTV_1080i)))
            && (pMode->AvInfo & AV_FLAGS_WIDESCREEN)
            && !(SystemAvInfo & AV_FLAGS_WIDESCREEN))
        {
            continue;
        }

        // Throw away any mode that uses a refresh rate the display doesn't
        // support.
        //
        if (!(pMode->AvInfo & AvRefresh))
        {
            continue;
        }

        // Throw away any HDTV mode that this display does not
        // support.
        //
        if (AvPack == AV_PACK_HDTV
            && !(pMode->AvInfo & AvHDTVModes)
            && (pMode->AvInfo & AV_HDTV_MODE_MASK) != 0)
        {
            continue;
        }

        if (!Mode--)
        {
            pModeRet->Width = pMode->Width;
            pModeRet->Height = pMode->Height;
            pModeRet->RefreshRate = (pMode->AvInfo & AV_FLAGS_60Hz) ? 60 : 50;
            pModeRet->Flags = CMiniport::GetPresentFlagsFromAvInfo(pMode->AvInfo);
            
            switch(Format)
            {
            case 0:
                pModeRet->Format = D3DFMT_LIN_X8R8G8B8;
                break;

            case 1:
                pModeRet->Format = D3DFMT_LIN_R5G6B5;
                break;

            case 2:
                pModeRet->Format = D3DFMT_LIN_X1R5G5B5;
                break;

            case 3:
                pModeRet->Format = D3DFMT_LIN_A8R8G8B8;
                break;
            }

            return S_OK;
        }
    }
}

//------------------------------------------------------------------------------
// Direct3D_GetAdapterDisplayMode

extern "C"
HRESULT WINAPI Direct3D_GetAdapterDisplayMode(
    UINT Adapter,
    D3DDISPLAYMODE* pMode) 
{ 
    COUNT_API(API_DIRECT3D_GETADAPTERDISPLAYMODE);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    CDevice *pDevice = g_pDevice;

    if (pDevice == NULL)
    {
        ZeroMemory(pMode, sizeof(*pMode));
    }
    else
    {
        pMode->Width = PixelJar::GetWidth(pDevice->m_pFrameBuffer[1]);
        pMode->Height = PixelJar::GetHeight(pDevice->m_pFrameBuffer[1]);
        pMode->RefreshRate = pDevice->m_Miniport.GetRefreshRate();
        pMode->Format = DecodeD3DFORMAT(pDevice->m_pFrameBuffer[1]->Format);
        pMode->Flags = CMiniport::GetPresentFlagsFromAvInfo(pDevice->m_Miniport.m_CurrentAvInfo);
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Direct3D_CheckDeviceType

extern "C"
HRESULT WINAPI Direct3D_CheckDeviceType(
    UINT Adapter, 
    D3DDEVTYPE CheckType, 
    D3DFORMAT DisplayFormat, 
    D3DFORMAT BackBufferFormat, 
    BOOL Windowed)
{ 
    COUNT_API(API_DIRECT3D_CHECKDEVICETYPE);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    if (CheckType != D3DDEVTYPE_HAL)
    {
        return D3DERR_INVALIDDEVICE;
    }

    if (Windowed == TRUE)
    {
        return D3DERR_NOTAVAILABLE;
    }

    if (!CheckDisplayFormat(DisplayFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    if (!CheckDisplayFormat(BackBufferFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Direct3D_CheckDeviceFormat

extern "C"
HRESULT WINAPI Direct3D_CheckDeviceFormat(
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    D3DFORMAT AdapterFormat,
    DWORD Usage,
    D3DRESOURCETYPE RType,
    D3DFORMAT CheckFormat) 
{ 
    COUNT_API(API_DIRECT3D_CHECKDEVICEFORMAT);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    if (DeviceType != D3DDEVTYPE_HAL)
    {
        return D3DERR_INVALIDDEVICE;
    }

    if (!CheckDisplayFormat(AdapterFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    if (Usage & D3DUSAGE_DEPTHSTENCIL)
    {
        if (!IsValidDepthBufferD3DFORMAT(CheckFormat))
        {
            return D3DERR_NOTAVAILABLE;
        }
    }
    else if (Usage & D3DUSAGE_RENDERTARGET)
    {
        if (!CheckDisplayFormat(CheckFormat))
        {
            return D3DERR_NOTAVAILABLE;
        }
    }
    else
    {
        if (BitsPerPixelOfD3DFORMAT(CheckFormat) == 0)
        {
            return D3DERR_NOTAVAILABLE;
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Direct3D_CheckDeviceMultiSampleType

extern "C"
HRESULT WINAPI Direct3D_CheckDeviceMultiSampleType(
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    D3DFORMAT RenderTargetFormat,
    BOOL Windowed,
    D3DMULTISAMPLE_TYPE MultiSampleType) 
{ 
    COUNT_API(API_DIRECT3D_CHECKDEVICEMULTISAMPLETYPE);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    if (DeviceType != D3DDEVTYPE_HAL)
    {
        return D3DERR_INVALIDDEVICE;
    }

    if (Windowed == TRUE)
    {
        return D3DERR_NOTAVAILABLE;
    }

    if (!CheckDisplayFormat(RenderTargetFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    if ((MultiSampleType != D3DMULTISAMPLE_NONE) && (MultiSampleType != 0))
    {
        if ((MultiSampleType & ANTIALIAS_FORMAT_MASK) >
                            D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8)
        {
            return D3DERR_INVALIDCALL;
        }

        if ((ANTIALIAS_XSCALE(MultiSampleType) < 1) ||
            (ANTIALIAS_YSCALE(MultiSampleType) < 1) ||
            (ANTIALIAS_XSCALE(MultiSampleType) > 3) ||
            (ANTIALIAS_YSCALE(MultiSampleType) > 3))
        {
            return D3DERR_NOTAVAILABLE;
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Direct3D_CheckDepthStencilMatch

extern "C"
HRESULT WINAPI Direct3D_CheckDepthStencilMatch(
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    D3DFORMAT AdapterFormat,
    D3DFORMAT RenderTargetFormat,
    D3DFORMAT DepthStencilFormat)
{
    COUNT_API(API_DIRECT3D_CHECKDEPTHSTENCILMATCH);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    if (DeviceType != D3DDEVTYPE_HAL)
    {
        return D3DERR_INVALIDDEVICE;
    }

    if (!CheckDisplayFormat(RenderTargetFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    if (!IsValidDepthBufferD3DFORMAT(DepthStencilFormat))
    {
        return D3DERR_NOTAVAILABLE;
    }

    return S_OK; 
}

//------------------------------------------------------------------------------
// Direct3D_GetDeviceCaps

extern "C"
HRESULT WINAPI Direct3D_GetDeviceCaps(
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    D3DCAPS8* pCaps) 
{ 
    COUNT_API(API_DIRECT3D_GETDEVICECAPS);

    if (Adapter != 0)
    {
        return D3DERR_INVALIDCALL;
    }

    if (DeviceType != D3DDEVTYPE_HAL)
    {
        return D3DERR_INVALIDDEVICE;
    }

    D3DDevice_GetDeviceCaps(pCaps);

    return S_OK;
}

} // end of namespace
