#ifndef _NVDD_
#define _NVDD_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/*==========================================================================;
 *
 *  Copyright (C) 1995, 1999 NVidia Corporation. All Rights Reserved.
 *
 *  File:       nvdd.h
 *  Content:    header for NVidia Windows 95 Direct Draw driver
 *
 ***************************************************************************/

#ifndef NVARCH
ERROR! NVARCH must be defined. Definition of NV3, NV4, NV10, etc. is obsolete.
#endif

#if(NVARCH < 0x4)
#define CDriverContext GLOBALDATA
#pragma warning(disable:4005) //macro redefinition warning/error
#endif

// force write combine and put
#ifdef NVSPEAK
// obsolete - use nvPusherStart
#else //!NVSPEAK
#define NVDD_FLUSH_AND_PUT(putAddress) \
    { pDriverData->NvDmaPusherBufferEnd[0] += putAddress[0]; \
      _outp (0x3d0,0); \
      npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2; }
#endif //!NV_SPEAK

#define NVFAR FAR

#define FAST register

#define asmMergeCoords(LOWWORD, HIWORD) \
        (long)(((long)HIWORD << 16) | (unsigned short)LOWWORD)


typedef struct {
    unsigned long offset;
    unsigned short selector;
} NVFP;

#define NVLP2FP(NVFAR_POINTER, USHORT_POINTER) \
    {   \
        NVFP *fptemp = (NVFP *)&(NVFAR_POINTER); \
         fptemp->selector = (USHORT_POINTER)[1]; \
        fptemp->offset = (USHORT_POINTER)[0]; \
    }


#define CLOSE_HANDLE(h) CloseHandle(h)
//#define CLOSE_HANDLE(h) (1)

#ifdef NVD3D

typedef struct {
    short   x;
    short   y;
} NVXY;

typedef struct {
    unsigned short w;
    unsigned short h;
} NVWH;


typedef union {
    long xy;
    unsigned long wh;
    NVXY xy16;
    NVWH wh16;
} NVCOORD;

#endif

#ifdef WINNT
    extern PDEV *ppdev;
    #define nvSetDriverDataPtrFromDDGbl(pDDGbl) {                                                                   \
        NvAcquireSemaphore((CDriverContext *)(((PDEV *)(((PDD_DIRECTDRAW_GLOBAL)(pDDGbl))->dhpdev))->pDriverData))  \
        ppdev       = ((PDEV *)(((PDD_DIRECTDRAW_GLOBAL)(pDDGbl))->dhpdev));                                        \
        pDriverData = (CDriverContext *)(ppdev->pDriverData);                                                       \
        pDriverData->dwDDGlobal = (DWORD)(pDDGbl);                                                                  \
        NvSetStereoDataPtrFromDriverData;                                                                           \
    }
    #define nvSetDriverDataPtrFromContext(pContext) {                                                               \
        NvAcquireSemaphore((pContext)->pDriverData)                                                                 \
        ppdev       = (pContext)->ppdev;                                                                            \
        pDriverData = (pContext)->pDriverData;                                                                      \
        NvSetStereoDataPtrFromDriverData;                                                                           \
    }
#else // !WINNT
    #include "dxshare.h"
    extern DISPDRVDIRECTXCOMMON *pDXShare;
    #define nvSetDriverDataPtrFromDDGbl(pDDGbl) {                                                                   \
        pDXShare         = (DISPDRVDIRECTXCOMMON *)((pDDGbl)->dwReserved3);                                         \
        pDriverData      = (CDriverContext *)pDXShare->pDirectXData;                                                \
        pDriverData->dwDDGlobal = (DWORD)(pDDGbl);                                                                  \
        NvSetStereoDataPtrFromDriverData;                                                                           \
    }
    #define nvSetDriverDataPtrFromContext(pContext) {                                                               \
        pDXShare         = (DISPDRVDIRECTXCOMMON *)((pContext)->pDXShare);                                          \
        pDriverData      = (pContext)->pDriverData;                                                                 \
        NvSetStereoDataPtrFromDriverData;                                                                           \
    }
#endif // !WINNT

//
// DDHAL_EXIT(retcode) should always be invoked when returning from a HAL function
// or any function which loads the global variable pDriverData. This macro will
// return the parameter retVal to its caller.
//
#ifndef WINNT
    #define DDHAL_EXIT(retVal) return(retVal)
#else
    #define DDHAL_EXIT(retVal)              \
        NvReleaseSemaphore(pDriverData);    \
        return(retVal)
#endif

#if !defined(WINNT) || (DIRECT3D_VERSION >= 0x0800)
    #define NvTexSurfHandle(ptcd) ((LPDIRECTDRAWSURFACE) ((ptcd)->lpDDS))
#else   // WINNT
    #define NvTexSurfHandle(ptcd) ((LPDIRECTDRAWSURFACE) ((ptcd)->hDDS))
#endif  // WINNT

#if !defined(WINNT) || (DIRECT3D_VERSION >= 0x0800)
    #define NvLockTextureSurface(ptr)   ((LPDDRAWI_DDRAWSURFACE_INT)NvTexSurfHandle(ptr))->lpLcl
    #define NvUnlockTextureSurface(ptr)
#else   // WINNT
    #define NvLockTextureSurface(ptr)   EngLockDirectDrawSurface(NvTexSurfHandle(ptr))
    #define NvUnlockTextureSurface(ptr) EngUnlockDirectDrawSurface((ptr))
#endif  // WINNT

#if !defined(WINNT) || (DIRECT3D_VERSION >= 0x0800)
#define NvSaveTexSurfHandle(dst, src)   (dst)->lpDDS = (src)->lpDDS
#else   // WINNT
#define NvSaveTexSurfHandle(dst, src)   (dst)->hDDS = (src)->lpDDS
#endif  // WINNT

/*
 * Macros used to obtain video memory offset/addresses from fpVidMem field
 * in surface struct. Our Win9x driver requires an address in the fpVidMem
 * field while NT requires an offset. These macros are used to hide these
 * differences.
 */

#if defined(WINNT) && (DIRECT3D_VERSION < 0x0800)
    #define SETLIGHT_DATATYPE(lpDp2SetLight)          ((lpDp2SetLight)->lightData)
    #define SETPRIORITY_DSTSURF(lpDp2SetPriority)     ((lpDp2SetPriority)->dwDDDestSurface)
    #define SETRENDERTARG_DDSLCL(lpDp2SetRenderTarg)  ((lpDp2SetRenderTarg)->lpDDS)
    #define SETRENDERTARG_DDSLCLZ(lpDp2SetRenderTarg) ((lpDp2SetRenderTarg)->lpDDSZ)
#else // !WINNT
    #define SETLIGHT_DATATYPE(lpDp2SetLight)          ((lpDp2SetLight)->dwDataType)
    #define SETPRIORITY_DSTSURF(lpDp2SetPriority)     ((lpDp2SetPriority)->dwDDSurface)
    #define SETRENDERTARG_DDSLCL(lpDp2SetRenderTarg)  ((lpDp2SetRenderTarg)->lpDDSLcl)
    #define SETRENDERTARG_DDSLCLZ(lpDp2SetRenderTarg) ((lpDp2SetRenderTarg)->lpDDSLclZ)
#endif // !WINNT

#ifdef WINNT
#ifndef VIDMEM_ADDR
    #define VIDMEM_ADDR(fpVidMem)    ((fpVidMem) + (ULONG) ppdev->pjFrameBufbase)
#endif
    #define VIDMEM_OFFSET(fpVidMem)  (fpVidMem)
#ifndef AGPMEM_ADDR
    #define AGPMEM_ADDR(fpVidMem)    ((ULONG) fpVidMem + (ULONG) pDriverData->GARTLinearBase)
#endif
    #define VIDMEM_OFFSET(fpVidMem)  (fpVidMem)
    #define AGPMEM_OFFSET(fpVidMem)  ((ULONG) fpVidMem - (ULONG) pDriverData->GARTLinearBase)
    #define GET_PRIMARY_ADDR()       (ppdev->ulPrimarySurfaceOffset)
    #define GET_MODE_WIDTH()         (ppdev->cxScreen)
    #define GET_MODE_HEIGHT()        (ppdev->cyScreen)
    #define GET_MODE_PITCH()         (ppdev->lDelta)
    #define GET_MODE_PHYSICAL_HEIGHT(dwHead) (pDriverData->flipPrimaryRecord.nvTimingInfo[dwHead].VDisplayEnd)
    #define GET_MODE_BPP()           (ppdev->cBitsPerPel)
    #define GET_HALINFO()            (&(pDriverData->HALInfo))
    #define GET_CURRENT_HEAD()       (ppdev->ulDeviceDisplay[0])
    #define GET_SPAN_OFFSET(dwHead)  (ppdev->ulHeadDisplayOffset[dwHead])
    #define PRIMARY_BUFFER_INDEX     (ppdev->ulPrimaryBufferIndex)
#else // !WINNT
    #define VIDMEM_ADDR(fpVidMem)    (fpVidMem)
    #define VIDMEM_OFFSET(fpVidMem)  ((fpVidMem) - pDriverData->BaseAddress)
    #define AGPMEM_ADDR(fpVidMem)    (fpVidMem)
    #define AGPMEM_OFFSET(fpVidMem)  ((fpVidMem) - pDriverData->GARTLinearBase)
    #define GET_PRIMARY_ADDR()       (pDXShare->dwScreenLinearBase + (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deBitsOffset))
    #define GET_MODE_WIDTH()         (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deWidth)
    #define GET_MODE_HEIGHT()        (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deHeight)
    #define GET_MODE_PITCH()         (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deDeltaScan)
    #define GET_MODE_PHYSICAL_HEIGHT(dwHead) (pDriverData->flipPrimaryRecord.nvTimingInfo[dwHead].VDisplayEnd)
    #define GET_MODE_BPP()           (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deBitsPixel)
    #define GET_HALINFO()            ((LPDDHALINFO)pDXShare->pHalInfo)
    #define GET_CURRENT_HEAD()       (pDXShare->dwHeadNumber)
    #define GET_SPAN_OFFSET(dwHead)  (0)
    #define PRIMARY_BUFFER_INDEX     (pDXShare->primaryBufferIndex)
#endif // !WINNT
    #define CURRENT_FLIP             (pDriverData->dwCurrentFlip[GET_CURRENT_HEAD()])
    #define COMPLETED_FLIP           (pDriverData->dwCompletedFlip[GET_CURRENT_HEAD()])

#ifdef WINNT
    #define TEXTURE_LOCK_SWIZZLED(pTex)  ((pTex)->getSwizzled()->getOffset())
    #define TEXTURE_LOCK_LINEAR(pTex)    ((pTex)->getLinear()->getAddress())
#else // !WINNT
    #define TEXTURE_LOCK_SWIZZLED(pTex)  ((pTex)->getSwizzled()->getAddress())
    #define TEXTURE_LOCK_LINEAR(pTex)    ((pTex)->getLinear()->getAddress())
#endif // !WINNT


// this now exists for win2k, prototype it
void getDisplayDuration(void);


#if (NVARCH >= 0x04)
#define NvGetFreeCount(NN, CH)\
         (NN)->subchannel[CH].control.Free
#else   // !(NVARCH >= 0x04)
#define NvGetFreeCount(NN, CH)\
         (NN)->subchannel[CH].control.free
#endif  // !(NVARCH >= 0x04)

#define NV_ALPHA_1_032          ((U032)0xFF000000)
#if (NVARCH >= 0x04)
#define NV_ALPHA_1_016          ((U032)0xFFFF0000)
#else
#define NV_ALPHA_1_016          ((U032)0x00008000)
#endif
#define NV_ALPHA_1_008          ((U032)0x0000FF00)
#define NV_PATTERN_EVEN         ((U032)0x55555555)
#define NV_PATTERN_ODD          ((U032)0xAAAAAAAA)
#define NV_PRESCALE_OVERLAY_X           1
#define NV_PRESCALE_OVERLAY_Y           2
#define NV_PRESCALE_OVERLAY_FIELD       4
#define NV_PRESCALE_OVERLAY_QUADRANT    8

#ifndef WINNT
#define NV_DD_NOTIFIER_BLOCK_SIZE       (sizeof(NvNotification) << 5)
#else
// BUGBUG we need to change this for NT when we update to latest
// Win9x driver source base.
#define NV_DD_NOTIFIER_BLOCK_SIZE       (sizeof(NvNotification) << 4)
#endif

#if (NVARCH >= 0x010)

#define NV_MAX_X_CLIP                   0x00000FFF
#define NV_MAX_X_WIDTH                  0x00001000

#define NV_MAX_Y_CLIP                   0x00000FFF
#define NV_MAX_Y_HEIGHT                 0x00001000

#else   // !(NVARCH >= 0x010)

#define NV_MAX_X_CLIP                   0x000007FF
#define NV_MAX_X_WIDTH                  0x00000800

#define NV_MAX_Y_CLIP                   0x00003FFF
#define NV_MAX_Y_HEIGHT                 0x00004000

#endif  // !(NVARCH >= 0x010)


// DMA Pusher related stuff

#if (NVARCH >= 0x04)

#define NV_PFIFO_DMA                                     0x00002508
#define NV_PFIFO_CACHE1_PUSH1                            0x00003204
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT               0x00000100
#define NV_PFIFO_CACHE1_DMA_PUSH                         0x00003220
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY_BIT        0x00000100

#ifndef NVSPEAK
#define NV_DD_DMA_PUSHER_SYNC() {                                           \
    volatile DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;            \
    DWORD *pDW = (DWORD *) (pPDevice->HWPDevice.pCurrentChannelID);         \
                                                                            \
  if  (*pDW != pDriverData->dwDDDmaPusherChannelIndex)                      \
  {                                                                         \
    *pDW = pDriverData->dwDDDmaPusherChannelIndex;                          \
    while ((npNvBase[(NV_PFIFO_DMA >> 2)] &                                 \
            (~pDriverData->dwDDDmaPusherChannelMask)) != 0);                \
    if ((npNvBase[NV_PFIFO_CACHE1_PUSH1 >> 2] & NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT) && \
        ((npNvBase[NV_PFIFO_CACHE1_PUSH1 >> 2] & 0x1F) !=                   \
            (WORD)pDriverData->dwDDDmaPusherChannelIndex)) {                \
        while ((npNvBase[NV_PFIFO_CACHE1_DMA_PUSH >> 2] &                   \
                 NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY_BIT) == 0);          \
    }                                                                       \
  }                                                                         \
}
#endif

#define NV_DD_GET_DMA_CHANNEL_INDEX(I) {                                    \
    volatile DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;            \
    (I) = (npNvBase[NV_PFIFO_CACHE1_PUSH1 >> 2] & 0x1F);                    \
}

#ifndef NVSPEAK
#define NvGetDmaBufferFreeCount(dmaChannel, freeCount, requiredCount, currentBufferAddress) { \
    long getOffset = (long)(volatile)(dmaChannel)->Get;                     \
    long putOffset = (long)((currentBufferAddress) - (DWORD *)pDriverData->NvDmaPusherBufferBase) << 2; \
    if (putOffset >= getOffset) {                                           \
        (freeCount) = (long)(pDriverData->NvDmaPusherBufferSize - 64 - putOffset) >> 2; \
        if ((long)(freeCount) < (long)(requiredCount)) {                    \
            (currentBufferAddress)[0] = NV_DMA_PUSHER_JUMP;                 \
            pDriverData->NvDmaPusherBufferEnd[0] += (DWORD)currentBufferAddress; \
            _outp (0x3d0,0);                                                \
            (dmaChannel)->Put = 0;                                          \
            (currentBufferAddress) =                                        \
                (DWORD *)pDriverData->NvDmaPusherBufferBase;                \
        }                                                                   \
    } else {                                                                \
        (freeCount) = ((getOffset - putOffset) >> 2) - 1;                   \
    }                                                                       \
    if ((freeCount) < requiredCount) {                                      \
        putOffset = 350 << 3;                                               \
        while (--putOffset > 0)                                             \
            (volatile)getOffset = putOffset;                                \
    }                                                                       \
}

#ifdef  CACHE_FREECOUNT
#ifdef  DEBUG
    extern void __stdcall NvCachedFreeCountError(long, long, DWORD *);

#define NvCheckCachedFreeCount(freeCount, currentBufferAddress) { \
    long putOffset = (long)(((DWORD *)currentBufferAddress) - (DWORD *)pDriverData->NvDmaPusherBufferBase) << 2; \
    long correctFreeCount = (long)(pDriverData->NvDmaPusherBufferSize - 64 - putOffset) >> 2; \
    if ((long)freeCount > (long)correctFreeCount)                                       \
        NvCachedFreeCountError((long)freeCount, correctFreeCount, (DWORD *)currentBufferAddress); \
}
#endif  /* DEBUG */
#endif  /* CACHE_FREECOUNT */

#endif //!NVSPEAK

#else   // (NVARCH >= 0x04)

#define NV3_3D_PIPE_STATE_RESET ((U032)0x00000008)

#ifdef  NV3T
#define NV_DD_DMA_PUSHER_SYNC() {                                           \
    if (pDriverData->fDmaPusherActive) {                                    \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvNotification *pDDPusherSyncNotifier =                             \
            (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;     \
        NvNotification *pWinDmaToMemNotifier =                              \
            (NvNotification *)pDriverData->NvWinDmaPusherNotifierFlat;      \
        if ((pDriverData->fDmaPusherActive & NV_DD_DMA_PUSHER_IS_ACTIVE) && \
            (pDriverData->NvDevFlatPio != 0))                               \
            while ((volatile)pDDPusherSyncNotifier->status == NV_IN_PROGRESS);\
        pDriverData->fDmaPusherActive = 0;                                  \
        npNvBase[(0x3200 >> 2)] = 1;                                        \
    }                                                                       \
    if (pDriverData->ThreeDRenderingOccurred) {                             \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvChannel *npDev = (NvChannel *)pDriverData->NvDevFlatPio;          \
        if (npDev != NULL) {                                                \
            long freeCount = NvGetFreeCount(npDev, NV_DD_ROP);              \
            while (freeCount < 4)                                           \
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);               \
            npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;                         \
            while ((freeCount = NvGetFreeCount(npDev, NV_DD_ROP)) < 124);   \
        }                                                                   \
        npNvBase[0x100020] = npNvBase[0x100020] | NV3_3D_PIPE_STATE_RESET;  \
        pDriverData->ThreeDRenderingOccurred = 0;                           \
    }                                                                       \
}
#else   // ~NV3T
#define NV_DD_DMA_PUSHER_SYNC() {                                           \
    if (pDriverData->fDmaPusherActive) {                                    \
        NvNotification *pDDPusherSyncNotifier =                             \
            (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;     \
        NvNotification *pWinDmaToMemNotifier =                              \
            (NvNotification *)pDriverData->NvWinDmaPusherNotifierFlat;      \
        if ((pDriverData->fDmaPusherActive & NV_DD_DMA_PUSHER_IS_ACTIVE) && \
            (pDriverData->NvDevFlatPio != 0))                               \
            while ((volatile)pDDPusherSyncNotifier->status == NV_IN_PROGRESS);\
        pDriverData->fDmaPusherActive = 0;                                  \
    }                                                                       \
    if (pDriverData->ThreeDRenderingOccurred) {                             \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvChannel *npDev = (NvChannel *)pDriverData->NvDevFlatPio;          \
        if (npDev != NULL) {                                                \
            long freeCount = NvGetFreeCount(npDev, NV_DD_ROP);              \
            while (freeCount < 4)                                           \
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);               \
            npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;                         \
            while ((freeCount = NvGetFreeCount(npDev, NV_DD_ROP)) < 124);   \
        }                                                                   \
        npNvBase[0x100020] = npNvBase[0x100020] | NV3_3D_PIPE_STATE_RESET;  \
        pDriverData->ThreeDRenderingOccurred = 0;                           \
    }                                                                       \
}
#endif  // ~NV3T

#ifdef  NEW_DMAPUSH
#define NV_DD_DMA_PUSHER_FLUSH() {                                          \
    if (pDriverData->fDmaPusherActive) {                                    \
        NvNotification *pDDPusherSyncNotifier =                             \
            (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;     \
        NvNotification *pWinDmaToMemNotifier =                              \
            (NvNotification *)pDriverData->NvWinDmaPusherNotifierFlat;      \
        if ((pDriverData->fDmaPusherActive & NV_DD_DMA_PUSHER_IS_ACTIVE) && \
            (pDriverData->NvDevFlatPio != 0))                               \
            while ((volatile)pDDPusherSyncNotifier->status == NV_IN_PROGRESS);\
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
    }                                                                       \
    else if ((pDriverData->dwDmaCurrentCount > sizeSetHeaderDmaPush) || (dwDmaPendingCount != 0)) \
    {                                                                       \
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
    }                                                                       \
    if (pDriverData->ThreeDRenderingOccurred) {                             \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvChannel *npDev = (NvChannel *)pDriverData->NvDevFlatPio;          \
        if (npDev != NULL) {                                                \
            long freeCount = NvGetFreeCount(npDev, NV_DD_ROP);              \
            while (freeCount < 4)                                           \
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);               \
            npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;                         \
            while ((freeCount = NvGetFreeCount(npDev, NV_DD_ROP)) < 124);   \
        }                                                                   \
        npNvBase[0x100020] = npNvBase[0x100020] | NV3_3D_PIPE_STATE_RESET;  \
        pDriverData->ThreeDRenderingOccurred = 0;                           \
    }                                                                       \
}
#else   // OLD_DMAPUSH
#ifdef  NV3T
#define NV_DD_DMA_PUSHER_FLUSH() {                                          \
    if (pDriverData->fDmaPusherActive) {                                    \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvNotification *pDDPusherSyncNotifier =                             \
            (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;     \
        NvNotification *pWinDmaToMemNotifier =                              \
            (NvNotification *)pDriverData->NvWinDmaPusherNotifierFlat;      \
        if ((pDriverData->fDmaPusherActive & NV_DD_DMA_PUSHER_IS_ACTIVE) && \
            (pDriverData->NvDevFlatPio != 0))                               \
            while ((volatile)pDDPusherSyncNotifier->status == NV_IN_PROGRESS);\
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
        npNvBase[(0x3200 >> 2)] = 1;                                        \
    }                                                                       \
    else if (pDriverData->dwDmaCount > sizeSetHeaderDmaPush)                \
    {                                                                       \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
        npNvBase[(0x3200 >> 2)] = 1;                                        \
    }                                                                       \
    if (pDriverData->ThreeDRenderingOccurred) {                             \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvChannel *npDev = (NvChannel *)pDriverData->NvDevFlatPio;          \
        if (npDev != NULL) {                                                \
            long freeCount = NvGetFreeCount(npDev, NV_DD_ROP);              \
            npNvBase[(0x3200 >> 2)] = 1;                                    \
            while (freeCount < 4)                                           \
             freeCount = NvGetFreeCount(npDev, NV_DD_ROP);                  \
            npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;                         \
            while ((freeCount = NvGetFreeCount(npDev, NV_DD_ROP)) < 124);   \
        }                                                                   \
        npNvBase[0x100020] = npNvBase[0x100020] | NV3_3D_PIPE_STATE_RESET;  \
        pDriverData->ThreeDRenderingOccurred = 0;                           \
    }                                                                       \
}
#else   // ~NV3T
#define NV_DD_DMA_PUSHER_FLUSH() {                                          \
    if (pDriverData->fDmaPusherActive) {                                    \
        NvNotification *pDDPusherSyncNotifier =                             \
            (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;     \
        NvNotification *pWinDmaToMemNotifier =                              \
            (NvNotification *)pDriverData->NvWinDmaPusherNotifierFlat;      \
        if ((pDriverData->fDmaPusherActive & NV_DD_DMA_PUSHER_IS_ACTIVE) && \
            (pDriverData->NvDevFlatPio != 0))                               \
            while ((volatile)pDDPusherSyncNotifier->status == NV_IN_PROGRESS);\
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
    }                                                                       \
    else if (pDriverData->dwDmaCount > sizeSetHeaderDmaPush)                \
    {                                                                       \
        nvFlushDmaBuffers();                                                \
        pDriverData->fDmaPusherActive = 0;                                  \
    }                                                                       \
    if (pDriverData->ThreeDRenderingOccurred) {                             \
        DWORD *npNvBase = (DWORD *)pDriverData->NvBaseFlat;                 \
        NvChannel *npDev = (NvChannel *)pDriverData->NvDevFlatPio;          \
        if (npDev != NULL) {                                                \
            long freeCount = NvGetFreeCount(npDev, NV_DD_ROP);              \
            while (freeCount < 4)                                           \
             freeCount = NvGetFreeCount(npDev, NV_DD_ROP);                  \
            npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;                         \
            while ((freeCount = NvGetFreeCount(npDev, NV_DD_ROP)) < 124);   \
        }                                                                   \
        npNvBase[0x100020] = npNvBase[0x100020] | NV3_3D_PIPE_STATE_RESET;  \
        pDriverData->ThreeDRenderingOccurred = 0;                           \
    }                                                                       \
}
#endif  // NV3T
#endif  // OLD_DMAPUSH

#endif  // !(NVARCH >= 0x04)

#ifdef NVD3D
/*
 * D3D Hal Fcts
 */
#if (NVARCH <0x04)
extern BOOL __stdcall D3DHALCreateDriver(LPD3DHAL_GLOBALDRIVERDATA *lplpGlobal,
                                         LPD3DHAL_CALLBACKS* lplpHALCallbacks
                                         );
#endif
extern BOOL __stdcall D3DGetDriverInfo(LPDDHAL_GETDRIVERINFODATA lpData);

extern BOOL DestroyItemArrays(void);
#endif // NVD3D
#ifdef __cplusplus
}
#endif  // __cplusplus
#endif // _NVDD_

