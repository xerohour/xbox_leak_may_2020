 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _nvContext_h
#define _nvContext_h

#ifndef __cplusplus
#error This file requires a C++ compiler
#endif

extern "C++"
{

//---------------------------------------------------------------------------
// helpers

__forceinline DWORD swapRGB (DWORD dw) { return (dw & 0xff00ff00) | ((dw & 0x00ff0000) >> 16) | ((dw & 0x000000ff) << 16); }

//---------------------------------------------------------------------------
// nvidia mem copy

void nvMemCopyReset (void);
void nvMemCopy      (DWORD dwDest,DWORD dwSrc,DWORD dwCount);
__forceinline void nvMemCopy (void *pDest,void *pSrc,DWORD dwCount) { nvMemCopy(DWORD(pDest),DWORD(pSrc),dwCount); }

//---------------------------------------------------------------------------
// flip history

#define FLIP_HISTORY_SIZE 16  // should be power of 2
#define FLIP_HISTORY_MASK (FLIP_HISTORY_SIZE - 1)

typedef struct {
    DWORD surfacesFrom[FLIP_HISTORY_SIZE];  // circular buffer of surfaces we've flipped FROM
#ifdef CHECK_FLIP_TO
    DWORD surfacesTo[FLIP_HISTORY_SIZE];    // circular buffer of surfaces we've flipped TO
#endif
    DWORD dwIndex;                          // current buffer element
} FLIPHISTORY;

//---------------------------------------------------------------------------
// registry data

typedef struct _D3D_REGISTRYDATA
{
    // bank
    DWORD regD3DEnableBits1;
    DWORD regAntiAliasQuality;          // rel5 and later
    DWORD regColorkeyRef;
    DWORD regMipMapDitherMode;
    DWORD regTexelAlignment;
    DWORD regValidateZMethod;
    DWORD regVSyncMode;
    DWORD regW16Format;
    // bank
    DWORD regW32Format;
    DWORD regAGPTexCutOff;
    DWORD regD3DContextMax;
    DWORD regD3DTextureMax;
    DWORD regLODBiasAdjust;
    DWORD regMinVideoTextureSize;
    DWORD regPCITexHeapSize;
    DWORD regPreRenderLimit;
    // bank
    DWORD regPushBufferSizeMax;
    float regfWScale16;
    float regfWScale24;
    DWORD regAntiAliasDynamicMSpF;
    float regfZBias;
    DWORD regTextureManageStrategy;
    DWORD regAAReadCompatibilityFile;
    DWORD regDebugLevel;
    // bank
    DWORD regCaptureConfig;
    DWORD regCaptureEnable;
    DWORD regCapturePlayFilenum;
    DWORD regCaptureRecordFilenum;
    DWORD regAnisotropicLevel;
    DWORD regAACompatibilityBits;
#ifdef TEXFORMAT_CRD
    DWORD regSurfaceFormatsDX7;
    DWORD regSurfaceFormatsDX8;
#else   // !TEXFORMAT_CRD
    DWORD dwPad01[2];
#endif  // !TEXFORMAT_CRD
    // bank
    char  regszCapturePlayFilename[D3D_REG_STRING_LENGTH];
    char  regszCapturePlayPath[D3D_REG_STRING_LENGTH];
    char  regszCaptureRecordFilename[D3D_REG_STRING_LENGTH];
    char  regszCaptureRecordPath[D3D_REG_STRING_LENGTH];
} NVD3D_REGISTRYDATA;

//---------------------------------------------------------------------------
// device caps

typedef struct _NVD3D_DEVCAPS
{
#ifdef RM20
    // this padding is because the portion of VPE that lives in the RM is not quite
    // ready to switch direct7 yet
    DWORD                   paddingForVPE[272];
#else
    D3DDEVICEDESC_V1        dd1Caps;
    D3DHAL_D3DEXTENDEDCAPS  d3dExtCaps;
#endif
    D3DCAPS8                dwD3DCap8;
} NVD3D_DEVCAPS;

//---------------------------------------------------------------------------
// texture heap stuff

typedef struct _NVD3D_TEXTUREHEAPDATA
{
    DWORD dwHandle;
    DWORD dwBaseRaw;        // unaligned base address
    DWORD dwBase;           // aligned base address
    DWORD dwAllocHead;
    DWORD dwAllocTail;
    DWORD dwFreeHead;
    DWORD dwFreeTail;
    DWORD dwSize;
    // bank
    DWORD dwSizeMax;
    DWORD dwLimit;
    DWORD dwLimitMax;
    DWORD dwAllocSize;
    DWORD dwFreeSize;
    DWORD _pad[3];
} NVD3D_TEXTUREHEAPDATA;

//---------------------------------------------------------------------------
// forward declarations of things defined elsewhere

class CNvObject;
class CTexture;
class CPushBuffer;
class CVertexShader;
class CVertexBuffer;
class CReferenceCount;

#ifdef KPFS
#if (NVARCH >= 0x020)
class CKelvinProgramMan;
#endif
#endif

#ifdef NV_PROFILE_DP2OPS
class CBenchmark;
#endif

template <int cdwLogMax> class CReferenceCountMap;

//---------------------------------------------------------------------------
//
// driver context declaration
//  every instance here must preserve 32-byte alignment
//

class CDriverContext : public _GLOBALDATA
{
public:
    const enum {
        DC_ENABLESTATUS_DISABLED       = 0,   // push buffer, channels, and objects not allocated
        DC_ENABLESTATUS_ENABLED        = 1,   // push buffer, channels, and objects are allocated
        DC_ENABLESTATUS_PENDINGDISABLE = 2    // disable required at next opportunity
    };

public:
// bank
    BYTE                     pbScratchBuffer8192[8192]; // DO NOT MOVE OR RESIZE
// bank
    DWORD                    dwEnableStatus;
    DWORD                    dwEnablePixelDepth;
    DWORD                    dwAGPHeapList;
    DWORD                    dwAGPHeapListMax;
    DWORD                    dwAGPHashTableHead;
    DWORD                    dwAGPHashTable;
    DWORD                    dwAGPHashTableMax;
    BOOL                     bAGPHeapValid;
// bank
    DWORD                    dwFVFCachedType;
    DWORD                    dwCurrentBlit;             // used to count present blits to limit the number of frames we get ahead
    DWORD                    dwCurrentFlip[4];          // array of flip counts, one element per head
    DWORD                    dwCompletedFlip[4];
    BOOL                     bD3DViewVector;

#if (NVARCH >= 0x020)

#ifdef KELVIN_SEMAPHORES
    CPushBuffer              nvPusherDAC;
    CSemaphoreManager        nvSemaphoreManager;
    CFlipper                 nvFlipper;
#endif

#ifdef KPFS
    CKelvinProgramMan        nvKelvinPM;
#endif

#ifdef LPC_OPT
    DWORD dwLastDP2;
    DWORD dwThisDP2;
#endif    

#endif
    CDefaultVB               defaultVB;
    CDefaultVB               defaultSysVB;              // used to hold legacy user mode vertex data (can't this just be a CVertexBuffer?)
    CReferenceCountMap<5>   *pBlitTracker;
    FLIPHISTORY              flipHistory;

    NVD3D_REGISTRYDATA       nvD3DRegistryData;
    NVD3D_DEVCAPS            nvD3DDevCaps;

#if (NVARCH >= 0x04)
    NVD3D_TEXTUREHEAPDATA    nvD3DTexHeapData;

    D3DHAL_GLOBALDRIVERDATA  D3DGlobalDriverData;
    DWORD                    dwD3DContextCount;
    DWORD                    dwD3DTextureCount;
    DWORD                    dwAAContextCount;
    PNVD3DCONTEXT            dwContextListHead;
    PNVD3DCONTEXT            dwPreapprovedContextListHead;
    DWORD                    dwLastHWContext;           // the last d3d context for which celsius/kelvin was programmed
    DWORD                    dwOutOfContextCelsiusDirtyFlags;
#endif  // (NVARCH >= 0x04)

#ifdef NV_PROFILE_DP2OPS
    CBenchmark *pBenchmark;
#endif

    // nV logo stuff
    CNvObject               *pLogoObject;
    CTexture                *pLogoTexture;
    CVertexBuffer           *pLogoVertexBuffer;
    CVertexShader           *pLogoVertexShader;

    CTexture                *pDefaultTexture;           // Default texture rendering object.
    CTexture                *pClipPlaneTexture;         // Texture used for user clip planes

    // memory for unaligned texture access spoof
    DWORD                    dwSpareTextureAddr[2];
    DWORD                    dwSpareTextureOffset[2];

#ifdef TEXFORMAT_CRD
    // surface formats for < DX8
    DWORD                    dwSurfaceDescriptorCount;
    DDSURFACEDESC           *pSurfaceDescriptors;

    // surface formats for >= DX8
    DWORD                    dwPixelFormatCount;
    DDPIXELFORMAT           *pPixelFormats;
#endif  // TEXFORMAT_CRD

    // methods
public:
    __forceinline LPDDRAWI_DIRECTDRAW_GBL getDDGlobal (void) const { return (LPDDRAWI_DIRECTDRAW_GBL)pDriverData->dwDDGlobal; }

public:
    void create  (void);
    void destroy (void);

    // redefine 'new' for this class so that it zeroes out memory before returning it.
    // this is required because the memset can't be included in CDriverContext::CDriverContext;
    // at that point, the constructors for classes included in CDriverContext have already
    // been automatically called and we don't want to stomp on them.
    void * operator new (size_t sz)     {   void *p = new char[sz];
                                            if(p){memset(p,0,sz);}
                                            return p;   }

    // contruction
    CDriverContext  (void) {
        memset(&dwSpareTextureAddr[0],0xff,sizeof(dwSpareTextureAddr)); // Init to all 0xff (invalid addresses)
    };
    ~CDriverContext (void) {};
};

// this must be a macro so that we access the local pDriverData, if one exists, rather than the global
#define getDC() ((CDriverContext*)pDriverData)

}   // extern c++

#endif // _nvContext_h

