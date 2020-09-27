#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4MINI.H                                                         *
* Hardware specific driver setup routines.                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman               01/31/98 - created.                     *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "d3dinc.h"
#include "nv4dreg.h"
#include "nvd3dcap.h"
#include "nv3ddbg.h"
#include "nvtexfmt.h"

#ifdef  NVD3D_DX6
/*
 * Define the Z-Buffer/Stencil Buffer Formats supported.
 */
#define NV_NUM_ZBUFFER_FORMATS      3
typedef struct _def_zpixel_formats
{
    DWORD           dwNumZFormats;
    DDPIXELFORMAT   NvZBufferFormats[NV_NUM_ZBUFFER_FORMATS];
} ZPIXELFORMATS;
static  ZPIXELFORMATS   NvZPixelFormats =
{
    NV_NUM_ZBUFFER_FORMATS,
    {
        /*
         * 16 bit z buffer
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER,                           // dwFlags
            0,                                      // dwFourCC (Not used)
            16,                                     // dwZBufferBitDepth
            0,                                      // dwStencilBitDepth
            0x0000FFFF,                             // dwZBitMask
            0x00000000,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        },
        /*
         * 24 bit z-buffer, no stencil
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER,                           // dwFlags
            0,                                      // dwFourCC (Not used)
            24,                                     // dwZBufferBitDepth
            0,                                      // dwStencilBitDepth
            0xFFFFFF00,                             // dwZBitMask
            0x00000000,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        },
        /*
         * 24 bit z-buffer, 8 bit stencil
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER | DDPF_STENCILBUFFER,      // dwFlags
            0,                                      // dwFourCC (Not used)
            32,                                     // dwZBufferBitDepth - This is so brain dead.  MS needs to learn consistency.
            8,                                      // dwStencilBitDepth
            0xFFFFFF00,                             // dwZBitMask
            0x000000FF,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        }
    }
};
#endif  // NVD3D_DX6

/*
 * Read the current driver settings from the registry.
 */
void D3DReadRegistry
(
    void
)
{
    DWORD   dwSize;
    DWORD   dwType;
    DWORD   dwValue;
    HKEY    hKey;

    /*
     * Set the defaults first.
     */
    pDriverData->regD3DEnableBits1       = DEFAULT_FOG_TABLE
                                         | DEFAULT_ANTI_ALIAS_ENABLE
                                         | DEFAULT_USER_MIPMAPS
                                         | DEFAULT_VIDEO_TEXTURE
                                         | DEFAULT_NO_WAIT_4_VSYNC
                                         | DEFAULT_MIPMAP_DITHER_ENABLE
                                         | DEFAULT_CKCOMPATABILITY_ENABLE
                                         | DEFAULT_CONTROLTRAFFIC;
    pDriverData->regMipMapLevels         = DEFAULT_MIPMAP_LEVELS;
    pDriverData->regTexHeap              = DEFAULT_TEX_HEAP;
    pDriverData->regMinVideoTextureSize  = DEFAULT_MIN_VIDEO_TEX_SIZE;
    pDriverData->regD3DContextMax        = DEFAULT_D3D_CONTEXT_MAX;
    pDriverData->regD3DTextureMax        = DEFAULT_D3D_TEXTURE_MAX;
    pDriverData->regDmaPushBufferSizeMax = DEFAULT_PUSH_BUFFER_SIZE_MAX_PCI;
    pDriverData->regDmaMinPushCount      = DEFAULT_DMA_MIN_PUSH_COUNT_PCI;
    pDriverData->regTexelAlignment       = DEFAULT_TEXEL_ALIGNMENT;
    pDriverData->regLODBiasAdjust        = DEFAULT_LOD_BIAS_ADJUST;
    pDriverData->regAutoMipMapMethod     = DEFAULT_AUTO_MIPMAP_METHOD;
    pDriverData->regPal8TextureConvert   = DEFAULT_PAL8_TEXTURE_CONVERT;

    pDriverData->regD3DDx6Enable         = DEFAULT_DX6_ENABLE;

    pDriverData->regPreRenderLimit       = DEFAULT_PRERENDER_LIMIT;

#if 1
    pDriverData->regZPerspectiveEnable = NV054_CONTROL_Z_PERSPECTIVE_ENABLE_FALSE;
    pDriverData->regZFormat            = NV054_CONTROL_Z_FORMAT_FIXED;
#else
    pDriverData->regZPerspectiveEnable = NV054_CONTROL_Z_PERSPECTIVE_ENABLE_TRUE;
    pDriverData->regZFormat            = NV054_CONTROL_Z_FORMAT_FLOAT;
#endif

    pDriverData->regAAMethod           = DEFAULT_AAMETHOD;
    pDriverData->regValidateZMethod    = DEFAULT_VALIDATE_Z_METHOD;

    pDriverData->regColorkeyRef        = DEFAULT_CKREF;

    pDriverData->regAGPTexCutOff       = DEFAULT_AGPTEX_CUTOFF;


#ifndef WINNT // BUGBUG need to seperate Registry routines into seperate lib

    /*
     * Next try reading the overrides from the registry.
     */
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;

    /*
     * Try and open the Direct3D Registry Key.
     */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, NV4D3D_APP_STRING, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        hKey = 0;

    if (hKey)
    {

        if (RegQueryValueEx(hKey,
                            NVD3D_AGPTEXTURE_CUTOFF,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regAGPTexCutOff = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_ANTI_ALIAS_ENABLE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_ANTI_ALIAS_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_ANTI_ALIAS_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_AAMETHOD_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue < REG_AAMETHOD_MIN) dwValue = REG_AAMETHOD_MIN;
            if (dwValue > REG_AAMETHOD_MAX) dwValue = REG_AAMETHOD_MAX;
            pDriverData->regAAMethod = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_MIPMAP_LEVELS_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regMipMapLevels = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_USER_MIPMAPS_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_USER_MIPMAPS_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_USER_MIPMAPS_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_TEX_HEAP_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if ((dwValue) && (dwValue >= NV_MIN_TEXTURE_HEAP_SIZE))
                pDriverData->regTexHeap = dwValue;
            else
                pDriverData->regTexHeap = NV_MIN_TEXTURE_HEAP_SIZE;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_VIDEO_TEXTURE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_VIDEO_TEXTURE_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_VIDEO_TEXTURE_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_NO_WAIT_4_VSYNC_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_NO_WAIT_4_VSYNC;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_NO_WAIT_4_VSYNC : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_MIN_VIDEO_TEX_SIZE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regMinVideoTextureSize = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_DMA_PUSH_BUFFER_SIZE_MAX_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue)
            {
                pDriverData->regDmaPushBufferSizeMax  = dwValue;
                pDriverData->regD3DEnableBits1       |= REG_BIT1_DMA_PUSH_SIZE_OVERRIDE;

            }
            else
            {
                pDriverData->regDmaPushBufferSizeMax  = DEFAULT_PUSH_BUFFER_SIZE_MAX_PCI;
                pDriverData->regD3DEnableBits1       &= ~REG_BIT1_DMA_PUSH_SIZE_OVERRIDE;
            }
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_DMA_MIN_PUSH_COUNT_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue)
            {
                pDriverData->regDmaMinPushCount  = dwValue;
                pDriverData->regD3DEnableBits1  |= REG_BIT1_DMA_PUSH_COUNT_OVERRIDE;
            }
            else
            {
                pDriverData->regDmaMinPushCount  = DEFAULT_DMA_MIN_PUSH_COUNT_PCI;
                pDriverData->regD3DEnableBits1  &= ~REG_BIT1_DMA_PUSH_COUNT_OVERRIDE;
            }

        }

        if (RegQueryValueEx(hKey,
                            NVD3D_FOG_TABLE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_FOG_TABLE_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_FOG_TABLE_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_CONTEXT_MAX_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DContextMax = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_TEXTURE_MAX_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DTextureMax = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_TEXEL_ALIGNMENT_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_TA_MAX)
                dwValue = DEFAULT_TEXEL_ALIGNMENT;
            pDriverData->regTexelAlignment = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_LOD_BIAS_ADJUST_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue & 0xFFFFFF00)
                dwValue = DEFAULT_LOD_BIAS_ADJUST;
            pDriverData->regLODBiasAdjust = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_AUTO_MIPMAP_METHOD_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_AMMM_MAX)
                dwValue = DEFAULT_AUTO_MIPMAP_METHOD;
            pDriverData->regAutoMipMapMethod = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_MIPMAP_DITHER_ENABLE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_MIPMAP_DITHER_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_MIPMAP_DITHER_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_PAL8_TEXTURE_CONVERT_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_P8TC_MAX)
                dwValue = DEFAULT_PAL8_TEXTURE_CONVERT;
            pDriverData->regPal8TextureConvert = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_DX6_ENABLE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_DX6E_MAX)
                dwValue = DEFAULT_DX6_ENABLE;
            pDriverData->regD3DDx6Enable = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_CKCOMPATABILITY_ENABLE_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_CKCE_MAX)
                dwValue = DEFAULT_CKCOMPATABILITY_ENABLE;
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_CKCOMPATABILITY_ENABLE;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_CKCOMPATABILITY_ENABLE : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_CONTROLTRAFFIC_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            pDriverData->regD3DEnableBits1 &= ~REG_BIT1_CONTROLTRAFFIC;
            pDriverData->regD3DEnableBits1 |= (dwValue) ? REG_BIT1_CONTROLTRAFFIC : 0;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_CKREF_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_CKREF_MAX)
                dwValue = DEFAULT_CKREF;
            pDriverData->regColorkeyRef = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_PRERENDER_LIMIT_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_PRERENDER_MAX)
                dwValue = REG_PRERENDER_MAX;
            if (dwValue < REG_PRERENDER_MIN)
                dwValue = REG_PRERENDER_MIN;
            pDriverData->regPreRenderLimit = dwValue;
        }

        if (RegQueryValueEx(hKey,
                            NVD3D_VALIDATE_Z_METHOD_STRING,
                            NULL,
                            &dwType,
                            (LPBYTE)&dwValue,
                            &dwSize) == ERROR_SUCCESS)
        {
            if (dwValue > REG_VZM_MAX)
                dwValue = DEFAULT_VALIDATE_Z_METHOD;
            pDriverData->regValidateZMethod = dwValue;
        }

        RegCloseKey(hKey);
    }
#endif // #ifndef WINNT
    if (pDriverData->regMipMapLevels == REG_MML_RESTRICTED)
    {
        pDriverData->regMipMapLevels = REG_MML_ENABLE;
        pDriverData->regD3DEnableBits1 |= REG_BIT1_RESTRICT_AUTO_MIPMAPS;
    }

    return;
}
/*
 * Read the current driver settings from the registry.
 */
void D3DModifyCapabilities
(
    D3DHAL_GLOBALDRIVERDATA  *pNvGlobal
)
{
    /*
     * Set the Fog Table Caps bit based on the registry setting.
     */
    pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps &= ~D3DPRASTERCAPS_FOGTABLE;
    pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  &= ~D3DPRASTERCAPS_FOGTABLE;
    if (pDriverData->regD3DEnableBits1 & REG_BIT1_FOG_TABLE_ENABLE)
    {
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= D3DPRASTERCAPS_FOGTABLE;
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= D3DPRASTERCAPS_FOGTABLE;
    }

    /*
     * Set anti-aliasing capabilities based on the registry setting.
     */
    pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps &= ~(D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
    pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  &= ~(D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
    if (pDriverData->regD3DEnableBits1 & REG_BIT1_ANTI_ALIAS_ENABLE)
    {
        /* for now we only support scene aa
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
        */
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
    }

    /*
     * Remove 8bit texture format if the registry says so.
     */
    if (pDriverData->regPal8TextureConvert == REG_P8TC_NONE)

        pNvGlobal->dwNumTextureFormats -= NV_NUM_8BIT_TEXTURE_FORMATS;
    else if (pDriverData->regPal8TextureConvert == REG_P8TC_Y8ONLY)
        pNvGlobal->dwNumTextureFormats -= NV_NUM_8BIT_Y8ONLY_TEXTURE_FORMATS;

    /*
     * Set the DRAWPRIMITVES2 capability bit when driver is exporting DX6 capabilities.
     */
#ifdef  NVD3D_DX6
    pNvGlobal->hwCaps.dwDevCaps &= ~D3DDEVCAPS_DRAWPRIMITIVES2;
    if (pDriverData->regD3DDx6Enable)
        pNvGlobal->hwCaps.dwDevCaps |= D3DDEVCAPS_DRAWPRIMITIVES2;
#endif  // NVD3D_DX6

    return;
}
/*
 * Return D3D Driver Information.
 */
BOOL __stdcall D3DGetDriverInfo
(
    LPDDHAL_GETDRIVERINFODATA lpData
)
{
    DWORD               dwSize;

    /*
     * Is this an extended D3D Callback GUID?
     */
#ifdef  NVD3D_DX6
    /*
     * Is this an extended D3D Callback3 GUID?
     */
    if ((IsEqualIID(&lpData->guidInfo, &GUID_D3DCallbacks3))
     && (pDriverData->regD3DDx6Enable != REG_DX6E_DISABLE))
    {
        D3DHAL_CALLBACKS3   D3DCallbacks3;

        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DCallbacks3");
        memset(&D3DCallbacks3, 0, sizeof(D3DHAL_CALLBACKS3));
        dwSize               = min(lpData->dwExpectedSize, sizeof(D3DHAL_CALLBACKS3));
        lpData->dwActualSize = sizeof(D3DHAL_CALLBACKS3);
        D3DCallbacks3.dwSize = dwSize;

        /*
         * Export DX6 DrawPrimitives2 DDI callback.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_DRAWPRIMITIVES2;
        D3DCallbacks3.DrawPrimitives2 = DrawPrimitives2;

        /*
         * Export DX6 Mult-Texture state validation callback.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_VALIDATETEXTURESTAGESTATE;
        D3DCallbacks3.ValidateTextureStageState = nvValidateTextureStageState;

        /*
         * Export DX6 callback for clear Render target, Z-Buffer and Stencil Buffer.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_CLEAR2;
        D3DCallbacks3.Clear2 = Clear2;

        /*
         * Copy as much of the data as possible up to dwExpectedSize.
         */
        memcpy(lpData->lpvData, &D3DCallbacks3, dwSize);

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }
    if ((IsEqualIID(&lpData->guidInfo, &GUID_D3DParseUnknownCommandCallback))
     && (pDriverData->regD3DDx6Enable != REG_DX6E_DISABLE))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DParseUnknownCommandCallback");

        /*
         * Get address of callback function.
         */
        fnD3DParseUnknownCommandCallback = lpData->lpvData;

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }
    if ((IsEqualIID(&lpData->guidInfo, &GUID_ZPixelFormats))
     && (pDriverData->regD3DDx6Enable != REG_DX6E_DISABLE))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_ZPixelFormats");

        /*
         * Return a pointer to the z-buffer format data.
         */
        dwSize = min(lpData->dwExpectedSize, sizeof(NvZPixelFormats));
        memcpy(lpData->lpvData, &NvZPixelFormats, dwSize);
        lpData->dwActualSize = dwSize;

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }
#endif  // NVD3D_DX6

    if (IsEqualIID(&lpData->guidInfo, &GUID_D3DCallbacks2))
    {
        D3DHAL_CALLBACKS2   D3DCallbacks2;

        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DCallbacks2");
        memset(&D3DCallbacks2, 0, sizeof(D3DHAL_CALLBACKS2));
        dwSize               = min(lpData->dwExpectedSize, sizeof(D3DHAL_CALLBACKS2));
        lpData->dwActualSize = sizeof(D3DHAL_CALLBACKS2);
        D3DCallbacks2.dwSize = dwSize;

        /*
         * Always export the SetRenderTarget callback.
         */
        D3DCallbacks2.dwFlags = D3DHAL2_CB32_SETRENDERTARGET;
        D3DCallbacks2.SetRenderTarget = nvSetRenderTarget;

        /*
         * Always export the D3D Clear callback.
         */
#ifndef WINNT
        D3DCallbacks2.dwFlags |= D3DHAL2_CB32_CLEAR;
        D3DCallbacks2.Clear    = Clear32;

        /*
         * Always export the DrawPrimitive callbacks.
         */
        D3DCallbacks2.dwFlags |= D3DHAL2_CB32_DRAWONEPRIMITIVE
                              |  D3DHAL2_CB32_DRAWONEINDEXEDPRIMITIVE
                              |  D3DHAL2_CB32_DRAWPRIMITIVES;
        D3DCallbacks2.DrawOnePrimitive        = DrawOnePrimitive32;
        D3DCallbacks2.DrawOneIndexedPrimitive = DrawOneIndexedPrimitive32;
        D3DCallbacks2.DrawPrimitives          = DrawPrimitives32;
#endif // #ifdef WINNT

        /*
         * Copy as much of the data as possible up to dwExpectedSize.
         */
        memcpy(lpData->lpvData, &D3DCallbacks2, dwSize);

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID(&lpData->guidInfo, &GUID_D3DExtendedCaps))
    {
        D3DHAL_D3DEXTENDEDCAPS  D3DExtendedCaps;

        memset(&D3DExtendedCaps, 0, sizeof(D3DHAL_D3DEXTENDEDCAPS));
        dwSize                 = min(lpData->dwExpectedSize, sizeof(D3DHAL_D3DEXTENDEDCAPS));
        lpData->dwActualSize   = dwSize;
        D3DExtendedCaps.dwSize = dwSize;

        /*
         * Fill in the extended capabilities.
         *
         * Set supported texture min/max dimensions.
         */
        D3DExtendedCaps.dwMinTextureWidth  = 1;
        D3DExtendedCaps.dwMaxTextureWidth  = 2048;
        D3DExtendedCaps.dwMinTextureHeight = 1;
        D3DExtendedCaps.dwMaxTextureHeight = 2048;

        /*
         * Set supported stipple min/max dimensions.
         */
//        D3DExtendedCaps.dwMinStippleWidth  = 0;
//        D3DExtendedCaps.dwMaxStippleWidth  = 0;
//        D3DExtendedCaps.dwMinStippleHeight = 0;
//        D3DExtendedCaps.dwMaxStippleHeight = 0;

#ifdef  NVD3D_DX6
        if (pDriverData->regD3DDx6Enable != REG_DX6E_DISABLE)
        {
            D3DExtendedCaps.dwMaxTextureRepeat       = NV_CAPS_MAX_TEXTURE_REPEAT;
            D3DExtendedCaps.dwMaxTextureAspectRatio  = NV_CAPS_MAX_TEXTURE_ASPECT_RATIO;
            D3DExtendedCaps.dwMaxAnisotropy          = NV_CAPS_MAX_ANISOTROPY;
            D3DExtendedCaps.dvGuardBandLeft          = NV_CAPS_GUARD_BAND_LEFT;
            D3DExtendedCaps.dvGuardBandTop           = NV_CAPS_GUARD_BAND_TOP;
            D3DExtendedCaps.dvGuardBandRight         = NV_CAPS_GUARD_BAND_RIGHT;
            D3DExtendedCaps.dvGuardBandBottom        = NV_CAPS_GUARD_BAND_BOTTOM;
            D3DExtendedCaps.dvExtentsAdjust          = NV_CAPS_EXTENTS_ADJUST;
            D3DExtendedCaps.dwStencilCaps            = D3DSTENCILCAPS_KEEP
                                                     | D3DSTENCILCAPS_ZERO
                                                     | D3DSTENCILCAPS_REPLACE
                                                     | D3DSTENCILCAPS_INCRSAT
                                                     | D3DSTENCILCAPS_DECRSAT
                                                     | D3DSTENCILCAPS_INVERT
                                                     | D3DSTENCILCAPS_INCR
                                                     | D3DSTENCILCAPS_DECR;
            D3DExtendedCaps.dwFVFCaps                = NV_CAPS_FVF_CAPS
                                                     | D3DFVFCAPS_DONOTSTRIPELEMENTS;
            D3DExtendedCaps.dwTextureOpCaps          = D3DTEXOPCAPS_DISABLE
                                                     | D3DTEXOPCAPS_SELECTARG1
                                                     | D3DTEXOPCAPS_SELECTARG2
                                                     | D3DTEXOPCAPS_MODULATE
                                                     | D3DTEXOPCAPS_MODULATE2X
                                                     | D3DTEXOPCAPS_MODULATE4X
                                                     | D3DTEXOPCAPS_ADD
                                                     | D3DTEXOPCAPS_ADDSIGNED
                                                     | D3DTEXOPCAPS_ADDSIGNED2X
                                                     | D3DTEXOPCAPS_ADDSMOOTH
                                                     | D3DTEXOPCAPS_SUBTRACT
                                                     | D3DTEXOPCAPS_BLENDDIFFUSEALPHA
                                                     | D3DTEXOPCAPS_BLENDTEXTUREALPHA
                                                     | D3DTEXOPCAPS_BLENDFACTORALPHA
                                                     | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM
                                                     | D3DTEXOPCAPS_BLENDCURRENTALPHA
                                                     | D3DTEXOPCAPS_PREMODULATE
                                                     | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR
                                                     | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA
                                                     | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR
                                                     | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA;
            D3DExtendedCaps.wMaxTextureBlendStages   = NV_CAPS_MAX_TEXTURE_BLEND_STATES;
            D3DExtendedCaps.wMaxSimultaneousTextures = NV_CAPS_MAX_SIMULTANEOUS_TEXTURES;
        }
#endif  // NVD3D_DX6

        /*
         * Copy as much of the data as possible up to dwExpectedSize.
         */
        memcpy(lpData->lpvData, &D3DExtendedCaps, dwSize);

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }

    /*
     * Return successfully.
     */
    return (TRUE);
}
#endif  // NV4
