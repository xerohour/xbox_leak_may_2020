// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvFormats.cpp
//      routines for generating lists of surface formats supported
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        05Mar2001         creation
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//---------------------------------------------------------------------------

#ifdef TEXFORMAT_CRD

// surface descriptors for DX7 and earlier

static DDSURFACEDESC nvSurfaceDescriptorsBasic[] = {
    NV_TFMT_X1R5G5B5,
    NV_TFMT_A1R5G5B5,
    NV_TFMT_A4R4G4B4,
    NV_TFMT_R5G6B5,
    NV_TFMT_X8R8G8B8,
    NV_TFMT_A8R8G8B8
};

static DDSURFACEDESC nvSurfaceDescriptorsBumpmap[] = {
    NV_TFMT_BUMPMAP_DV8_DU8,
    NV_TFMT_BUMPMAP_L6_DV5_DU5,
    NV_TFMT_BUMPMAP_L8_DV8_DU8
};

static DDSURFACEDESC nvSurfaceDescriptorsDXT[] = {
    NV_TFMT_DXT1,
    NV_TFMT_DXT2,
    NV_TFMT_DXT3,
    NV_TFMT_DXT4,
    NV_TFMT_DXT5
};

static DDSURFACEDESC nvSurfaceDescriptorsPalettized[] = {
    NV_TFMT_RGB8
};

static DDSURFACEDESC nvSurfaceDescriptorsNVxn[] = {
    NV_TFMT_FOURCC(FOURCC_NVT0),
    NV_TFMT_FOURCC(FOURCC_NVT1),
    NV_TFMT_FOURCC(FOURCC_NVT2),
    NV_TFMT_FOURCC(FOURCC_NVT3),
    NV_TFMT_FOURCC(FOURCC_NVT4),
    NV_TFMT_FOURCC(FOURCC_NVT5),
    NV_TFMT_FOURCC(FOURCC_NVS0),
    NV_TFMT_FOURCC(FOURCC_NVS1),
    NV_TFMT_FOURCC(FOURCC_NVS2),
    NV_TFMT_FOURCC(FOURCC_NVS3),
    NV_TFMT_FOURCC(FOURCC_NVS4),
    NV_TFMT_FOURCC(FOURCC_NVS5)
};

static DDSURFACEDESC nvSurfaceDescriptorsHILO[] = {
    NV_TFMT_NVHU,
    NV_TFMT_NVHS
};

// pixel formats for DX8 and later

static DDPIXELFORMAT nvPixelFormatsBasicCelsius[] = {
    NV_TFMT_X1R5G5B5_DX8,
    NV_TFMT_A1R5G5B5_DX8,
    NV_TFMT_A4R4G4B4_DX8,
    NV_TFMT_R5G6B5_DX8_CELSIUS_MULTISAMPLE,
    NV_TFMT_X8R8G8B8_DX8_CELSIUS_MULTISAMPLE,
    NV_TFMT_A8R8G8B8_DX8_CELSIUS_MULTISAMPLE
};

static DDPIXELFORMAT nvPixelFormatsBasicKelvin[] = {
    NV_TFMT_X1R5G5B5_DX8,
    NV_TFMT_A1R5G5B5_DX8,
    NV_TFMT_A4R4G4B4_DX8,
    NV_TFMT_R5G6B5_DX8_KELVIN_MULTISAMPLE,
    NV_TFMT_X8R8G8B8_DX8_KELVIN_MULTISAMPLE,
    NV_TFMT_A8R8G8B8_DX8_KELVIN_MULTISAMPLE
};

static DDPIXELFORMAT nvPixelFormatsBumpmap[] = {
    NV_TFMT_BUMPMAP_DV8_DU8_DX8,
    NV_TFMT_BUMPMAP_L6_DV5_DU5_DX8,
    NV_TFMT_BUMPMAP_L8_DV8_DU8_DX8
};

static DDPIXELFORMAT nvPixelFormatsDXT[] = {
    NV_TFMT_DXT1_DX8,
    NV_TFMT_DXT2_DX8,
    NV_TFMT_DXT3_DX8,
    NV_TFMT_DXT4_DX8,
    NV_TFMT_DXT5_DX8
};

static DDPIXELFORMAT nvPixelFormatsPalettized[] = {
    NV_TFMT_RGB8_DX8
};

static DDPIXELFORMAT nvPixelFormatsNVxn[] = {
    NV_TFMT_DX8(FOURCC_NVT0, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVT1, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVT2, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVT3, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVT4, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVT5, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS0, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS1, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS2, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS3, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS4, D3D_TEXTURE),
    NV_TFMT_DX8(FOURCC_NVS5, D3D_TEXTURE),
};

static DDPIXELFORMAT nvPixelFormatsHILO[] = {
    NV_TFMT_NVHU_DX8,
    NV_TFMT_NVHS_DX8,
};

static DDPIXELFORMAT nvPixelFormatsZetaCelsius[] = {
    NV_TFMT_Z16_CELSIUS_MULTISAMPLE,
    NV_TFMT_Z24_CELSIUS_MULTISAMPLE,
    NV_TFMT_Z24_S8_CELSIUS_MULTISAMPLE
};

static DDPIXELFORMAT nvPixelFormatsZetaKelvin[] = {
    NV_TFMT_Z16_KELVIN_MULTISAMPLE,
    NV_TFMT_Z24_KELVIN_MULTISAMPLE,
    NV_TFMT_Z24_S8_KELVIN_MULTISAMPLE
};

static DDPIXELFORMAT nvPixelFormatsMisc[] = {
    NV_TFMT_Q8W8V8U8_DX8
};

//---------------------------------------------------------------------------

BOOL nvEnumerateFourCCs (void)
{
    DWORD dwNum = 0;

    // A few other random things that never change
    pDriverData->fourCC[dwNum++] = FOURCC_UYVY;
    pDriverData->fourCC[dwNum++] = FOURCC_UYNV;
    pDriverData->fourCC[dwNum++] = FOURCC_YUY2;
    pDriverData->fourCC[dwNum++] = FOURCC_YUNV;
    pDriverData->fourCC[dwNum++] = FOURCC_YV12;
    pDriverData->fourCC[dwNum++] = FOURCC_YVU9;
    pDriverData->fourCC[dwNum++] = FOURCC_IF09;
    pDriverData->fourCC[dwNum++] = FOURCC_IV32;
    pDriverData->fourCC[dwNum++] = FOURCC_IV31;
    pDriverData->fourCC[dwNum++] = FOURCC_RAW8;

    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))
        &&
        ((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN) ||
         (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN))
       )
    {
        pDriverData->fourCC[dwNum++] = FOURCC_NVDS;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT0;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT1;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT2;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT3;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT4;
        pDriverData->fourCC[dwNum++] = FOURCC_NVT5;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS0;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS1;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS2;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS3;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS4;
        pDriverData->fourCC[dwNum++] = FOURCC_NVS5;
    }

    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))
        &&
        ((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT) ||
         (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT))
       )
    {
        pDriverData->fourCC[dwNum++] = FOURCC_DXT1;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT2;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT3;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT4;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT5;
    }

    if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
        &&
        ((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_HILO) ||
         (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_HILO))
       )
    {
        pDriverData->fourCC[dwNum++] = FOURCC_NVHS;
        pDriverData->fourCC[dwNum++] = FOURCC_NVHU;
    }

    return (TRUE);
}

//---------------------------------------------------------------------------

#define COPY_AND_INCREMENT(a,b)     \
{                                   \
    memcpy (a, &(b), sizeof(b));    \
    a += sizeof(b);                 \
}

//---------------------------------------------------------------------------

BOOL nvEnumerateSurfaceFormats (void)
{
    DWORD  dwSize;
    BYTE  *pCopyAddr;

    dbgTracePush ("nvEnumerateSurfaceFormats");

    // free any memory we may already have allocated
    if (getDC()->pSurfaceDescriptors) {
        FreeIPM(getDC()->pSurfaceDescriptors);
        getDC()->pSurfaceDescriptors = NULL;
    }
    if (getDC()->pPixelFormats) {
        FreeIPM(getDC()->pPixelFormats);
        getDC()->pPixelFormats = NULL;
    }

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {

        // surface descriptors
        dwSize = 0;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      dwSize += sizeof(nvSurfaceDescriptorsBasic);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BUMPMAP)    dwSize += sizeof(nvSurfaceDescriptorsBumpmap);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT)        dwSize += sizeof(nvSurfaceDescriptorsDXT);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_PALETTIZED) dwSize += sizeof(nvSurfaceDescriptorsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN)       dwSize += sizeof(nvSurfaceDescriptorsNVxn);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_HILO)       dwSize += sizeof(nvSurfaceDescriptorsHILO);

        getDC()->dwSurfaceDescriptorCount = dwSize / sizeof(DDSURFACEDESC);
        getDC()->pSurfaceDescriptors = (DDSURFACEDESC *) AllocIPM (dwSize);
        if (!getDC()->pSurfaceDescriptors) {
            dbgTracePop();
            return FALSE;
        }

        pCopyAddr = (BYTE *) getDC()->pSurfaceDescriptors;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsBasic     );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BUMPMAP)    COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsBumpmap   );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT)        COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsDXT       );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_PALETTIZED) COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN)       COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsNVxn      );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_HILO)       COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsHILO      );

        // pixel formats
        dwSize = 0;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BASIC)      dwSize += sizeof(nvPixelFormatsBasicKelvin);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BUMPMAP)    dwSize += sizeof(nvPixelFormatsBumpmap);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT)        dwSize += sizeof(nvPixelFormatsDXT);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED) dwSize += sizeof(nvPixelFormatsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN)       dwSize += sizeof(nvPixelFormatsNVxn);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_HILO)       dwSize += sizeof(nvPixelFormatsHILO);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_ZETA)       dwSize += sizeof(nvPixelFormatsZetaKelvin);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_MISC)       dwSize += sizeof(nvPixelFormatsMisc);

        getDC()->dwPixelFormatCount = dwSize / sizeof(DDPIXELFORMAT);
        getDC()->pPixelFormats = (DDPIXELFORMAT *) AllocIPM (dwSize);
        if (!getDC()->pPixelFormats) {
            FreeIPM(getDC()->pSurfaceDescriptors);
            getDC()->pSurfaceDescriptors = NULL;
            dbgTracePop();
            return FALSE;
        }

        pCopyAddr = (BYTE *) getDC()->pPixelFormats;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BASIC)      COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsBasicKelvin);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BUMPMAP)    COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsBumpmap    );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT)        COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsDXT        );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED) COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsPalettized );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsNVxn       );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_HILO)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsHILO       );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_ZETA)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsZetaKelvin );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_MISC)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsMisc       );

    }

    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {

        // surface descriptors
        dwSize = 0;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      dwSize += sizeof(nvSurfaceDescriptorsBasic);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT)        dwSize += sizeof(nvSurfaceDescriptorsDXT);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_PALETTIZED) dwSize += sizeof(nvSurfaceDescriptorsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN)       dwSize += sizeof(nvSurfaceDescriptorsNVxn);

        getDC()->dwSurfaceDescriptorCount = dwSize / sizeof(DDSURFACEDESC);
        getDC()->pSurfaceDescriptors = (DDSURFACEDESC *) AllocIPM (dwSize);
        if (!getDC()->pSurfaceDescriptors) {
            dbgTracePop();
            return FALSE;
        }

        pCopyAddr = (BYTE *) getDC()->pSurfaceDescriptors;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsBasic     );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT)        COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsDXT       );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_PALETTIZED) COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN)       COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsNVxn      );

        // pixel formats
        dwSize = 0;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BASIC)      dwSize += sizeof(nvPixelFormatsBasicCelsius);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT)        dwSize += sizeof(nvPixelFormatsDXT);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED) dwSize += sizeof(nvPixelFormatsPalettized);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN)       dwSize += sizeof(nvPixelFormatsNVxn);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_ZETA)       dwSize += sizeof(nvPixelFormatsZetaCelsius);

        getDC()->dwPixelFormatCount = dwSize / sizeof(DDPIXELFORMAT);
        getDC()->pPixelFormats = (DDPIXELFORMAT *) AllocIPM (dwSize);
        if (!getDC()->pPixelFormats) {
            FreeIPM(getDC()->pSurfaceDescriptors);
            getDC()->pSurfaceDescriptors = NULL;
            dbgTracePop();
            return FALSE;
        }

        pCopyAddr = (BYTE *) getDC()->pPixelFormats;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_BASIC)      COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsBasicCelsius);
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT)        COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsDXT         );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED) COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsPalettized  );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsNVxn        );
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_ZETA)       COPY_AND_INCREMENT (pCopyAddr, nvPixelFormatsZetaCelsius );

    }

    else {

        // surface descriptors
        nvAssert (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_DXTRI);

        dwSize = 0;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      dwSize += sizeof(nvSurfaceDescriptorsBasic);

        getDC()->dwSurfaceDescriptorCount = dwSize / sizeof(DDSURFACEDESC);
        getDC()->pSurfaceDescriptors = (DDSURFACEDESC *) AllocIPM (dwSize);
        if (!getDC()->pSurfaceDescriptors) {
            dbgTracePop();
            return FALSE;
        }

        pCopyAddr = (BYTE *) getDC()->pSurfaceDescriptors;
        if (getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_BASIC)      COPY_AND_INCREMENT (pCopyAddr, nvSurfaceDescriptorsBasic);

        // pixel formats
        getDC()->dwPixelFormatCount = 0;
        getDC()->pPixelFormats = NULL;

    }

    dbgTracePop();
    return TRUE;
}

#endif  // TEXFORMAT_CRD

#endif // (NVARCH >= 0x04)

