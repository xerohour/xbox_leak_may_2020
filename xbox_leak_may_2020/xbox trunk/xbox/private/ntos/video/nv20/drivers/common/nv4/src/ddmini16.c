/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995,1997 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       ddmini16.c
 *  Content:    Windows95 Direct Draw driver
 *
 *  exported functions:
 *      ddEnable
 *      ddDisable
 *      Control
 *
 *  internal functions:
 *      FindMode
 *      GetRegInt
 *      GetFlatSel
 *
 *  public functions:
 *      none
 *
 ***************************************************************************/

#ifndef WINNT
#include "ddmini.h"
//#include "opengl.h"
#include "windows.h"
#else
#include "nv32.h"
#include "nvd3ddrv.h"
#endif // #ifdef WINNT
#include "nvcm.h"
#include "nvos.h"
#include "tvout.h"
#include  "escape.h"
#define  MSVC
#ifndef WINNT
#include "display.inc"
#endif // #ifndef WINNT
#ifdef  MULTI_MON
#include "nv432.h"
#include "nvrmarch.inc"
#include "nvddobj.h"
#endif  // MULTI_MON
#include "nvver.h"

#ifndef WINNT
#define NV_ERROR_NOTIFIER          0x0FF5
#define NV_DD_COMMON_DMA_BUFFER_SIZE 32768
#else // #ifdef WINNT
#define NV_ERROR_NOTIFIER           NV01_CONTEXT_DMA
#define SIZEOF_NVNOTIFICATION 16

//
// Redefine Obsolete class.
//
#ifdef NV01_CONTEXT_DMA_TO_MEMORY
#undef NV01_CONTEXT_DMA_TO_MEMORY
#endif // #ifdef NV01_CONTEXT_DMA_TO_MEMORY
#define NV01_CONTEXT_DMA_TO_MEMORY  NV01_CONTEXT_DMA
#endif // #ifdef WINNT


//
// Video RAM types
//
#define BUFFER_DRAM                     0
#define BUFFER_SGRAM                    1
#define BUFFER_SDRAM                    2
//
// Monitor types
//
#define NV_MONITOR_VGA                  0
#define NV_MONITOR_NTSC                 2
#define NV_MONITOR_PAL                  3
//
// TV Out Device
//
#define NV_TV_OUT_NO_DEVICE             0
#define NV_TV_OUT_CHRONTEL7003          1
#define NV_TV_OUT_BT868                 2
#define NV_TV_OUT_BT869                 3

typedef VIDEOPARAMETERS FAR *LPVIDEOPARAMETERS;

#ifndef  MULTI_MON  // Generate the old structure used by the old code

typedef struct {
    DWORD function;
    DWORD device;
    WORD  owner[2];
    DWORD type;
    DWORD depth;
    DWORD width;
    DWORD height;
    DWORD pitch;
    DWORD offset;
    DWORD size;
    DWORD address;
    DWORD limit;
    DWORD total;
    DWORD free;
    DWORD status;
} NVIOControl_ParamX;

#endif   // NOT MULTI_MON

#ifndef WINNT
extern void (FAR PASCAL *NVSYSCLOSE)(DWORD);
#ifdef  MULTI_MON
extern void (FAR PASCAL *NVALLOCBUFFER)(BYTE FAR *lpBufferFlat, BYTE FAR *lpBuffer, DWORD numPages, DWORD dwDeviceID);
#else   // MULTI_MON
extern void (FAR PASCAL *NVALLOCBUFFER)(BYTE FAR *lpBufferFlat, BYTE FAR *lpBuffer, DWORD numPages);
#endif  // MULTI_MON

#ifdef  MULTI_MON
extern DWORD (FAR PASCAL *NvGetConfig)(DWORD dwConfigID, DWORD dwDeviceID);
extern DWORD (FAR PASCAL *NvGetRAMInfo)(DWORD dwDeviceID);
extern DWORD (FAR PASCAL *NvGetScreenSize)(DWORD dwDeviceID);
extern DWORD (FAR PASCAL *NvGetFrameBufferInfo)(DWORD dwDeviceID);
extern DWORD (FAR PASCAL *NvGetDeviceBase)(DWORD, DWORD dwDeviceID);
extern DWORD (FAR PASCAL *NvGetDeviceLimit)(DWORD, DWORD dwDeviceID);
extern void (FAR PASCAL *Nv01AllocContextDma)(Alloc_Ctx_DMA_Param FAR *);
extern void (FAR PASCAL *Nv01Free)(NV01Free_Param FAR *);
#else   // MULTI_MON
extern DWORD (FAR PASCAL *NvGetConfig)(DWORD dwConfigID);
extern DWORD (FAR PASCAL *NvGetRAMInfo)(VOID);
extern DWORD (FAR PASCAL *NvGetScreenSize)(VOID);
extern DWORD (FAR PASCAL *NvGetFrameBufferInfo)(VOID);
extern DWORD (FAR PASCAL *NvGetDeviceBase)(DWORD);
extern DWORD (FAR PASCAL *NvGetDeviceLimit)(DWORD);
#endif  // MULTI_MON
extern DWORD (FAR PASCAL *NvIOControl)(DWORD Function, LPVOID pParameters);
extern WORD FAR PASCAL UnsDwordDiv(DWORD dividend, DWORD divisor);
extern DWORD FAR PASCAL UnsDwordMul(DWORD multiplicand, DWORD multiplier);
extern DWORD FAR PASCAL GetFlatAddress(DWORD);
extern DWORD FAR PASCAL SetMacrovisionMode(DWORD mode);
#else // #ifdef WINNT
#define NVALLOCBUFFER(pflatptr, nop, numpages, nop1) \
    (pflatptr) = EngAllocMem(FL_ZERO_MEMORY, (numpages) * 4096, ALLOC_TAG)

#define NV_ALLOC_CONTEXT_DMA(hPar, hObj, clas, fl, offs, sel, limlo, limhi) \
    NvAllocContextDma(lpDevice->hDriver, (hPar), (hObj), (clas), (fl), (offs), (limlo))
#define NvGetConfig(dwConfigID, dwDeviceID) \
    (NvConfigGet(lpDevice->hDriver, lpDevice->hClient, lpDevice->hDevice, dwConfigID, &lpDevice->DummyReturn) == NVOS13_STATUS_SUCCESS ? lpDevice->DummyReturn : 0)
#define NvGetRAMInfo(dwDeviceID)            \
    ((lpDevice->cbFrameBuf >> 19) << 20)
#define NvGetScreenSize(dwDeviceID) \
    ((lpDevice->cyScreen << 16) | (lpDevice->cxScreen & 0xffff))
#define NvGetFrameBufferInfo(dwDeviceID) \
    (lpDevice->cBitsPerPel)
#define NvGetDeviceBase(dev, dwDeviceID) \
    (lpDevice->NvBaseAddr)
#define NvGetDeviceLimit(dev, dwDeviceID) \
    (lpDevice->cbFrameBuf + 1)
#define NV01Free_Param  NVOS00_PARAMETERS
#define Nv01Free(param) \
    NvFree(lpDevice->hDriver, (ULONG)param.hRoot, (ULONG)param.hObjectParent, (ULONG)param.hObjectOld)
#define NVSYSCLOSE(channel) \
    NvFree(lpDevice->hDriver, lpDevice->hClient, lpDevice->hDevice, channel)
#define dmaD3DPusherNotifier \
    (lpDevice->dmaD3DPusherNotifier)
#define dmaGDIPusherNotifier \
    (lpDevice->NvSyncNotifierFlat)
#define UnsDwordDiv(a, b) \
    (a / b)
#define UnsDwordMul(a, b) \
    (a * b)
#endif // #ifdef WINNT



#ifndef WINNT
extern LONG FAR PASCAL Control32(DIBENGINE FAR *, UINT, LPVOID, LPVOID);
extern void (FAR PASCAL DepopulateOffscreenCache)(void);

extern WORD PASCAL wSurfaceBUSY;
extern DWORD PASCAL NV_ddDmaBuffer;
extern DWORD PASCAL dmaGDIPusherNotifier;
extern DWORD PASCAL dmaD3DPusherNotifier;
extern OFFSCREENINITPARAMS PASCAL offscreenData;
extern DDHALMODEINFO NEAR *ModeList;

extern DDHALMODEINFO NEAR *ModeList;
extern DDHALMODEINFO NEAR NV4VGAModeList[];
extern DDHALMODEINFO NEAR NV4NTSCModeList[];
extern DDHALMODEINFO NEAR NV4PALModeList[];
extern int numModes;
extern int numNV4VGAModes;
extern int numNV4PALModes;
extern int numNV4NTSCModes;
#endif // #ifndef WINNT

#ifndef WINNT
extern int PASCAL videoRamSize;
extern int PASCAL wMonitorType;
extern DWORD PASCAL dwTVOutDeviceID;
#else
extern int videoRamSize;
extern int wMonitorType;
extern DWORD dwTVOutDeviceID;
#endif

#ifdef  MULTI_MON
#ifndef WINNT
// Global variables for multimonitor support
extern  DWORD PASCAL dwRoot_Handle;         // From ENABLE.ASM - RM id for client
extern  DWORD PASCAL dwDevice_ID_Num;       // From ENABLE.ASM - RM id for display
extern  DWORD        dwDeviceHandle;        // From DRV2.C - VDD id for display
extern  WORD  PASCAL wFlatSelector;         // From ENABLE.ASM - Ring3 flat selector
#else
#define dwRoot_Handle (lpDevice->hClient)
#define dwDevice_ID_Num (lpDevice->ulDeviceReference)
#define wFlatSelector 0
#endif // #ifdef WINNT
#endif  // MULTI_MON

/***************************************************************************
 *
 * globals
 *
 ***************************************************************************/

int             firstTimeCall = 1;
#ifndef WINNT
GLOBALDATA      DriverData;
#else // #ifdef WINNT
#define DriverData (*(lpDevice->pDriverData))
#endif // #ifdef WINNT
DWORD           macrovisionKey = 0x9999;
DWORD           macrovisionMode = 0;

/***************************************************************************
 *
 * internal functions.
 *
 ***************************************************************************/

UINT  GetFlatSel(void);
BOOL  DDCreateDriverObject(BOOL bReset);

#ifndef WINNT
/***************************************************************************
 *
 * FindMode
 *
 *  search through the mode table and find a mode that matchs
 *  call the HWTestMode function in setmode.c to make sure the
 *  mode is valid.
 *
 *  returns mode number 0-N if we found a valid mode
 *  returns -1 if we cant find a valid mode.
 *
 ***************************************************************************/

int PASCAL FindMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
    int i;

    for (i=0; ModeList[i].dwWidth!=0; i++)
    {
        if (ModeList[i].dwWidth == dwWidth &&
            ModeList[i].dwHeight == dwHeight &&
            ModeList[i].dwBPP == dwBPP &&
            HWTestMode(i))
        {
            return i;
        }
    }

    return -1;
}
#endif // #ifndef WINNT

/***************************************************************************
 *
 * ddEnable   called by Enable to enable the device and set the video mode
 *
 ***************************************************************************/
#ifndef WINNT
UINT FAR PASCAL _loadds ddEnable(LPVOID lpDevice, UINT style, LPSTR lpDeviceType, LPSTR lpOutput, LPVOID lpStuff)
#else
ULONG ddEnable(PDEV *lpDevice)
#endif // #ifdef WINNT
{
    int   ModeNumber = 0;
    DWORD dwResolutionX;
    DWORD dwResolutionY;
    DWORD dwBPP;
    DWORD dwFrameBufferInfo;
    DWORD dwResolutionInfo;
    DWORD mclk;
    DWORD videoRamType;
#ifdef  MULTI_MON
    NVOS00_PARAMETERS freeParams;
#endif  // MULTI_MON

    //
    // If firstTimeCall connect to NV Resource Manager
    //
    if (firstTimeCall) {
        DriverData.NvFirstCall = 1;    // let 32 bit DDraw driver know to reset NV
        DriverData.dwGlobalStructSize = sizeof(GLOBALDATA); // let 32 bit DDraw driver know current struct size

#ifdef  MULTI_MON
        DriverData.TotalVRAM = NvGetRAMInfo( dwDevice_ID_Num ) >> 16;
        DriverData.TotalVRAM <<= 20; // convert MB to bytes
        DriverData.dwRootHandle = dwRoot_Handle;
  #ifndef WINNT
        DriverData.dwDeviceHandle = dwDeviceHandle;
        DriverData.dwDeviceIDNum = dwDevice_ID_Num;
        DriverData.dwDeviceClass = dwDevice_ID_Num + (NV01_DEVICE_0 - 1); // RM numbering starts at 1
        NVALLOCBUFFER((BYTE FAR *)&DriverData.NvCommonDmaBufferFlat,
            (BYTE FAR *)&DriverData.lpNvDmaBuffer, 8, DriverData.dwDeviceIDNum);
  #else
        DriverData.dwDeviceHandle = (DWORD)lpDevice->hDevice;
        DriverData.dwDeviceIDNum = dwDevice_ID_Num;
        DriverData.dwDeviceClass = dwDevice_ID_Num + (NV01_DEVICE_0 - 1); // RM numbering starts at 1
        NVALLOCBUFFER((PVOID)DriverData.NvCommonDmaBufferFlat, 0, 8, 0);
  #endif // #ifdef WINNT

#else   // MULTI_MON
        DriverData.TotalVRAM = NvGetRAMInfo() >> 16;
        DriverData.TotalVRAM <<= 20; // convert MB to bytes

        NVALLOCBUFFER((BYTE FAR *)&DriverData.NvCommonDmaBufferFlat,
            (BYTE FAR *)&DriverData.lpNvDmaBuffer, 8);
#endif  // MULTI_MON

        if (DriverData.NvCommonDmaBufferFlat == 0)
            return FALSE;

#ifdef  MULTI_MON

        /*
         * Allocate the Context Dmas that don't change until this board gets disabled
         *    They are never freed by us, we depend on the root closure in WEP to
         *    destroy them for us when the display driver gets unloaded.
         */

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_CONTEXT_ERROR_TO_MEMORY,
            NV_ERROR_NOTIFIER,
            ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED,
            (PVOID)DriverData.NvCommonDmaBufferFlat,
            (DWORD)wFlatSelector,
            SIZEOF_NVNOTIFICATION - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);

//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)DriverData.NvCommonDmaBufferFlat,
            (DWORD)wFlatSelector,
            SIZEOF_NVNOTIFICATION - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);

//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)DriverData.NvCommonDmaBufferFlat,
            (DWORD)wFlatSelector,
            (2 * SIZEOF_NVNOTIFICATION) - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);

//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)(DriverData.NvCommonDmaBufferFlat + (SIZEOF_NVNOTIFICATION << 1)),
            (DWORD)wFlatSelector,
            SIZEOF_NVNOTIFICATION - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)(DriverData.NvCommonDmaBufferFlat + (SIZEOF_NVNOTIFICATION << 1) +
            SIZEOF_NVNOTIFICATION),
            (DWORD)wFlatSelector,
            SIZEOF_NVNOTIFICATION - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)(DriverData.NvCommonDmaBufferFlat + (SIZEOF_NVNOTIFICATION << 2)),
            (DWORD)wFlatSelector,
            SIZEOF_NVNOTIFICATION - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)(DriverData.NvCommonDmaBufferFlat + (SIZEOF_NVNOTIFICATION << 2) +
            SIZEOF_NVNOTIFICATION),
            (DWORD)wFlatSelector,
            (5 * SIZEOF_NVNOTIFICATION) - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)(DriverData.NvCommonDmaBufferFlat + (SIZEOF_NVNOTIFICATION << 3) +
            (SIZEOF_NVNOTIFICATION << 1)),
            (DWORD)wFlatSelector,
            (5 * SIZEOF_NVNOTIFICATION) - 1, /* required size for notifier */
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
            (PVOID)DriverData.NvCommonDmaBufferFlat,
            (DWORD)wFlatSelector,
            NV_DD_COMMON_DMA_BUFFER_SIZE - 1,
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        NV_ALLOC_CONTEXT_DMA(DriverData.dwRootHandle,
            NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
            NV01_CONTEXT_DMA,
            ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY,
            (PVOID)DriverData.NvCommonDmaBufferFlat,
            (DWORD)wFlatSelector,
            NV_DD_COMMON_DMA_BUFFER_SIZE - 1,
            0);

//        Nv01AllocContextDma((Alloc_Ctx_DMA_Param FAR *)&allocContextDmaParams);
//
//        if (allocContextDmaParams.status != ALLOC_CTX_DMA_STATUS_SUCCESS)
//            return FALSE;

        /*
         * Let the 32 bit DDraw code know that the following Context Dmas have not yet
         * been allocated.
         */

        DriverData.dwVidMemCtxDmaSize = 0;
        DriverData.dwDmaPusherCtxDmaSize = 0;


#endif  /* MULTI_MON */

        DriverData.NvDevFlatDma = 0;
        DriverData.NvDmaPusherBufferHandle = 0;
        DriverData.NvDmaPusherBufferBase = 0;
        DriverData.NvDevVideoFlatPio = 0;
        DriverData.NvDevFlatVPE = 0;
        DriverData.DDrawVideoSurfaceCount = 0;
        DriverData.GARTLinearBase = 0;
        DriverData.GARTPhysicalBase = 0;
        DriverData.NvAGPDmaPusherBufferBase = 0;
        DriverData.NvDmaPusherBufferSize = 0;
        DriverData.NvDmaPusherBufferEnd = 0;

        // initialize mode switch counter
        DriverData.dwModeSwitchCount = 0;

        // initialize shared clip change counter
        DriverData.dwSharedClipChangeCount = 0;
        DriverData.dwDDMostRecentClipChangeCount = 0;

        DriverData.wDDrawActive = 0;

        DriverData.dwMacrovisionNavigatorID = 0;
#ifndef WINNT
        firstTimeCall = 0;
#endif
    } else {
#ifdef  MULTI_MON
        if (DriverData.NvDevFlatDma != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_DMA;

#ifndef WINNT
            Nv01Free((NV01Free_Param FAR *)&freeParams);
#else
            Nv01Free(&freeParams);
#endif

            DriverData.NvDevFlatDma = 0;
        }
        if (DriverData.NvDevVideoFlatPio != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_VIDEO;

#ifndef WINNT
            Nv01Free((NV01Free_Param FAR *)&freeParams);
#else
            Nv01Free(&freeParams);
#endif

            DriverData.NvDevVideoFlatPio = 0;
        }
        if (DriverData.NvDevFlatVPE != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_VPE;

#ifndef WINNT
            Nv01Free((NV01Free_Param FAR *)&freeParams);
#else
            Nv01Free(&freeParams);
#endif

            DriverData.NvDevFlatVPE = 0;
        }
#else   /* MULTI_MON */
        if (DriverData.NvDevFlatDma != 0) {
            NVSYSCLOSE(DriverData.NvDevFlatDma);
            DriverData.NvDevFlatDma = 0;
        }
        if (DriverData.NvDevVideoFlatPio != 0) {
            NVSYSCLOSE(DriverData.NvDevVideoFlatPio);
            DriverData.NvDevVideoFlatPio = 0;
        }
        if (DriverData.NvDevFlatVPE != 0) {
            NVSYSCLOSE(DriverData.NvDevFlatVPE);
            DriverData.NvDevFlatVPE = 0;
        }
#endif  /* MULTI_MON */
    }

#ifdef  MULTI_MON
    dwResolutionInfo  = NvGetScreenSize( DriverData.dwDeviceIDNum );
    dwFrameBufferInfo = NvGetFrameBufferInfo( DriverData.dwDeviceIDNum );
#else   // MULTI_MON
    dwResolutionInfo  = NvGetScreenSize();
    dwFrameBufferInfo = NvGetFrameBufferInfo();
#endif  // MULTI_MON
    dwResolutionX = LOWORD(dwResolutionInfo);
    dwResolutionY = HIWORD(dwResolutionInfo);
    dwBPP = dwFrameBufferInfo & 0xFFFF;

#ifdef  MULTI_MON
    mclk = NvGetConfig( NV_CFG_DAC_MEMORY_CLOCK, DriverData.dwDeviceIDNum );
    videoRamType = NvGetConfig( NV_CFG_RAM_TYPE, DriverData.dwDeviceIDNum );
#else   // MULTI_MON
    mclk = NvGetConfig( NV_CFG_DAC_MEMORY_CLOCK );
    videoRamType = NvGetConfig( NV_CFG_RAM_TYPE );
#endif  // MULTI_MON

#ifndef WINNT
    ModeList = &NV4VGAModeList[0];
    numModes = numNV4VGAModes;

    if (wMonitorType == NV_MONITOR_PAL) {
        ModeList = &NV4PALModeList[0];
        numModes = numNV4PALModes;
    }

    if (wMonitorType == NV_MONITOR_NTSC) {
        ModeList = &NV4NTSCModeList[0];
        numModes = numNV4NTSCModes;
    }

    //
    // see if the mode is valid.
    // if it is not a mode we can support we fail
    //
    ModeNumber = FindMode(dwResolutionX, dwResolutionY, dwBPP);

    //
    // InquireInfo means fill in a GDIINFO structure
    // that describes the mode and the capabilities of the device
    //
    // we call DIB_Enable() and modify the fields specific to our
    // driver.
    //
    // NOTE you should never set (ie assign to) the dpRasterCaps
    // or dpCaps1 fields.  you should set specific bits (|=val), or in
    // rare cases clear a bit (&=~val).
    //
    // return the size of the structure we filled in.
    //
    if (style == InquireInfo) {
        return TRUE;
    }
    //
    //  EnableDevice means actualy set the mode.
    //  return zero for fail, non zero for success.
    //
    if (style == EnableDevice)
#endif // #ifndef WINNT
    {
#ifndef WINNT
        DIBENGINE FAR *pde;
#endif // #ifndef WINNT

        //
        // call the code in setmode.c to set the mode
        //
        // if HWSetMode works we expect the following globals
        // to be set.
        //
        //      DriverData.TotalVRAM        ; total amount of VRAM
        //      DriverData.BaseAddress      ; *flat* address of frame buffer (if flat)
        //      DriverData.ScreenSel        ; selector to framebuffer (if vflatd)
        //

//        HWSetMode(ModeNumber);

#if defined(NVHP) && !defined(WINNT)
        if (ModeNumber >= 0)
        {
            //
            // Allocate the primary display surface from the heap.
            //

            //
            // First clear out anything lying around.
            //
#ifdef  MULTI_MON
            NVIOControl_Param   heapParams;


            heapParams.hRoot         = DriverData.dwRootHandle;
            heapParams.hObjectParent = NV_WIN_DEVICE;
            heapParams.owner         = (((DWORD)'NV') << 16) + 'DD';
#else   // MULTI_MON
            NVIOControl_ParamX  heapParams;


            heapParams.device   = 0;
            heapParams.owner[0] = 'DD';
            heapParams.owner[1] = 'NV';
#endif  // MULTI_MON
            heapParams.function = NVIOCONTROL_HEAP_PURGE;
            NvIOControl( NVIOCONTROL_ARCH_HEAP, (LPVOID)&heapParams );

            //
            // Allocate  primary surface.
            //
#ifdef  MULTI_MON
            heapParams.hRoot         = DriverData.dwRootHandle;
            heapParams.hObjectParent = NV_WIN_DEVICE;
            heapParams.owner         = (((DWORD)'NV') << 16) + 'DD';
#else   // MULTI_MON
            heapParams.device   = 0;
            heapParams.owner[0] = 'DD';
            heapParams.owner[1] = 'NV';
#endif  // MULTI_MON
            heapParams.function = NVIOCONTROL_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT;
            heapParams.type     = NVIOCONTROL_TYPE_PRIMARY;
            heapParams.depth    = dwBPP;
            heapParams.width    = ModeList[ModeNumber].dwWidth;
            heapParams.height   = ModeList[ModeNumber].dwHeight;
            NvIOControl( NVIOCONTROL_ARCH_HEAP, (LPVOID)&heapParams );

            DriverData.BaseAddress    = heapParams.address;  // *flat* address
            DriverData.VideoHeapBase  = heapParams.limit + 1;
            DriverData.VideoHeapTotal = heapParams.free;
            DriverData.VideoHeapFree  = heapParams.free;
        }
#else   // NVHP
        // fill in the framebuffer address
#ifdef  MULTI_MON
#ifndef WINNT
        DriverData.BaseAddress = NvGetDeviceBase( NV_DEV_BUFFER_0, DriverData.dwDeviceIDNum ); // *flat* address
#else // WINNT
        DriverData.BaseAddress = 0;
#endif // #ifdef WINNT
#else   // MULTI_MON
        DriverData.BaseAddress = NvGetDeviceBase( NV_DEV_BUFFER_0 );  // *flat* address
#endif  // MULTI_MON
        if (ModeNumber >= 0)
#ifndef WINNT
            DriverData.VideoHeapBase = DriverData.BaseAddress +
                UnsDwordMul(ModeList[ModeNumber].lPitch, ModeList[ModeNumber].dwHeight);
#else
            DriverData.VideoHeapBase = DriverData.BaseAddress +
                UnsDwordMul(lpDevice->lDelta, lpDevice->cyScreen);
 #endif // #ifdef WINNT

#endif  // NVHP

#ifdef  MULTI_MON
#ifndef WINNT
        DriverData.NvBaseFlat = NvGetDeviceBase( NV_DEV_BASE, DriverData.dwDeviceIDNum );  // *flat* address
#else
        DriverData.NvBaseFlat = (DWORD)lpDevice->NvBaseAddr;
#endif // #ifdef WINNT
#else   // MULTI_MON
        DriverData.NvBaseFlat = NvGetDeviceBase( NV_DEV_BASE );  // *flat* address
#endif  // MULTI_MON
        DriverData.ScreenSel     = GetFlatSel();
        DriverData.flatSelector = (USHORT)GetFlatSel();
#ifdef  MULTI_MON
        dwResolutionInfo = NvGetScreenSize( DriverData.dwDeviceIDNum );
        dwFrameBufferInfo = NvGetFrameBufferInfo( DriverData.dwDeviceIDNum );
#else   // MULTI_MON
        dwResolutionInfo = NvGetScreenSize();
        dwFrameBufferInfo = NvGetFrameBufferInfo();
#endif  // MULTI_MON

        dwFrameBufferInfo &= 0xFFFF;

        if (dwFrameBufferInfo == 8) { // Pixel depth
            DriverData.physicalColorMask = 0xFF;
        } else if (dwFrameBufferInfo == 16) {
            DriverData.physicalColorMask = 0xFFFF;
        } else if (dwFrameBufferInfo == 32) {
            DriverData.physicalColorMask = 0xFFFFFF;
        }

#ifdef  MULTI_MON
#ifndef WINNT
        DriverData.VideoHeapEnd = NvGetDeviceLimit( NV_DEV_BUFFER_0, DriverData.dwDeviceIDNum );
#else
        DriverData.VideoHeapEnd = lpDevice->cbFrameBuf - 1;
#endif // #ifdef WINNT
#else   // MULTI_MON
        DriverData.VideoHeapEnd = NvGetDeviceLimit( NV_DEV_BUFFER_0 );
#endif  // MULTI_MON
#ifndef WINNT
        /* Let display driver caching mechanism know it's usable range */
        offscreenData.left   = 0;
        offscreenData.top    = (unsigned short) dwResolutionY;

        if (ModeNumber >= 0)
            DriverData.maxOffScreenY =
                UnsDwordDiv(DriverData.VideoHeapEnd, ModeList[ModeNumber].lPitch);
#else

//
// Now initialized in ResetNV
//
DriverData.maxOffScreenY =
            UnsDwordDiv(DriverData.VideoHeapEnd, lpDevice->lDelta);
#endif // #ifdef WINNT

#ifndef WINNT
        offscreenData.right  = (unsigned short) dwResolutionX;
        offscreenData.bottom = (unsigned short) DriverData.maxOffScreenY;
#endif // #ifndef WINNT
        DriverData.VideoHeapEnd += DriverData.BaseAddress;

        //
        // now fill in a BITMAPINFO that describes our mode
        // and call the DIBENG function CreateDIBPDevice to
        // fill in our PDevice
        //
        DriverData.bi.biSize        = sizeof(BITMAPINFOHEADER);
        DriverData.bi.biPlanes      = 1;
#ifdef WINNT
        DriverData.bi.biWidth       = lpDevice->cxScreen;
        DriverData.bi.biHeight      = lpDevice->cyScreen;
        DriverData.bi.biBitCount    = (USHORT)lpDevice->cBitsPerPel;
        DriverData.bi.biCompression = 0;
        DriverData.color_table[0]   = lpDevice->flRed;
        DriverData.color_table[1]   = lpDevice->flGreen;
        DriverData.color_table[2]   = lpDevice->flBlue;
#else // #ifndef WINNT
        if (ModeNumber >= 0) {
            DriverData.bi.biWidth       = ModeList[ModeNumber].dwWidth;
            DriverData.bi.biHeight      = ModeList[ModeNumber].dwHeight;
            DriverData.bi.biBitCount    = (BYTE)ModeList[ModeNumber].dwBPP;
            DriverData.bi.biCompression = 0; // ModeList[ModeNumber].dwBPP <= 8 ? 0 : 3;
            DriverData.color_table[0]   = ModeList[ModeNumber].dwRBitMask;
            DriverData.color_table[1]   = ModeList[ModeNumber].dwGBitMask;
            DriverData.color_table[2]   = ModeList[ModeNumber].dwBBitMask;
        } else {
            DriverData.ModeNumber       = ModeNumber; // 32 bit code needs this
            DriverData.bi.biWidth       = dwResolutionInfo & 0xFFFF;
            DriverData.bi.biHeight      = dwResolutionInfo >> 16;
            DriverData.bi.biBitCount = (BYTE)dwBPP;
            DriverData.bi.biCompression = 0; // ModeList[ModeNumber].dwBPP <= 8 ? 0 : 3;
            DriverData.NvPrevDepth = 0;
            return FALSE;
        }

        //
        //  set a few things in the DIBENGINE structure that CreateDIBPDevice
        //  did not do.
        //
        pde = (DIBENGINE FAR *)lpDevice;

        pde->deBitsSelector = (UINT)DriverData.ScreenSel;
        pde->deBits = (void NVFAR *)DriverData.BaseAddress;

        if (ModeNumber >= 0)
            pde->deDeltaScan = ModeList[ModeNumber].lPitch;

        pde->deDriverReserved = 0x42;
        pde->deBeginAccess = (void FAR *)BeginAccess;
        pde->deEndAccess = (void FAR *)EndAccess;

        if (dwFrameBufferInfo == 8)
            pde->deFlags |= PALETTIZED;
        else
            pde->deFlags &= ~PALETTIZED;

        if (dwFrameBufferInfo == 16)
            pde->deFlags |= FIVE6FIVE;
        else
            pde->deFlags &= ~FIVE6FIVE;

        pde->deFlags &= ~BUSY;       // device is not BUSY
        pde->deFlags &= ~BANKEDVRAM;
        wSurfaceBUSY = 0;
#endif // #ifndef WINNT
        //
        // Everything worked remember the mode number and the driver
        // PDevice and return success
        //
        DriverData.dwVersion = DDMINI_VERSION;
        DriverData.ModeNumber = ModeNumber;
#ifndef WINNT
        DriverData.lpPDevice = (void FAR *)lpDevice;
#else
        DriverData.lpPDevice = (DWORD)lpDevice;
#endif
        DriverData.fReset = TRUE;

        DriverData.fVidMemLocked = FALSE;

        //
        // Let 32 bit Direct Draw know that there are currently no external users
        // (like NVLIB, NVDDX, etc)
        //
        DriverData.fActiveExternalUser = FALSE;

        //
        // Let Direct Draw know about Display Driver's notifier
        //
        DriverData.NvWinDmaPusherNotifierFlat = (DWORD)dmaGDIPusherNotifier;

        //
        // Let Display Driver know about Direct Draw's DMA pusher sync notifier
        //
        dmaD3DPusherNotifier = (DWORD *)DriverData.NvCommonDmaBufferFlat;
        dmaD3DPusherNotifier += (SIZEOF_NVNOTIFICATION << 1) + SIZEOF_NVNOTIFICATION;
#ifndef WINNT
        //
        // Now re-register with DirectDraw so it knows all about the
        // new display mode.
        //
        if (DriverData.HALCallbacks.lpSetInfo)
        {
            DDCreateDriverObject(TRUE);
        }
#endif // #ifndef WINNT
        return TRUE;
    }

    return FALSE;
}

/***************************************************************************
 *
 * Disable
 *
 ***************************************************************************/
#ifndef WINNT
UINT FAR PASCAL _loadds ddDisable(DIBENGINE FAR *pde)
#else
UINT FAR PASCAL _loadds ddDisable(PDEV *lpDevice)
#endif
{
#ifdef  MULTI_MON
    NV01Free_Param freeParams;
#endif  // MULTI_MON

#ifndef WINNT
    pde->deFlags |= BUSY;       // device is BUSY
    wSurfaceBUSY = 1;
#else

    /*
     * Free the Context Dmas.
     */

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_CONTEXT_ERROR_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY
        );

    NvFree(DriverData.ppdev->hDriver,
        DriverData.dwRootHandle,
        DriverData.dwRootHandle,
        NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY
        );

    /*
     * If this is WINNT we need to free per device common DMA buffer
     * which under Win9x is freed when the 16 bit driver instance (created
     * for each device) is destroyed.
     */
    if (DriverData.NvCommonDmaBufferFlat != 0) {
        EngFreeMem((PVOID)DriverData.NvCommonDmaBufferFlat);
        DriverData.NvCommonDmaBufferFlat = 0;
    }

    /*
     * Under WINNT we also need to free the DMA push buffer, which under
     * Win9x is freed by the 16 bit display driver's WEP exit point.
     */
    if (DriverData.NvDmaPusherBufferHandle != 0) {
        EngFreeMem((PVOID)DriverData.NvDmaPusherBufferBase);
        DriverData.NvDmaPusherBufferHandle =
        DriverData.NvDmaPusherBufferBase = 0;
    }

#endif // #ifdef WINNT

    DriverData.ModeNumber = 0xDEADDEAD;

#ifdef  MULTI_MON
        if (DriverData.NvDevFlatDma != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_DMA;

            Nv01Free((NV01Free_Param FAR *)&freeParams);

            DriverData.NvDevFlatDma = 0;
        }
        if (DriverData.NvDevVideoFlatPio != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_VIDEO;

            Nv01Free((NV01Free_Param FAR *)&freeParams);

            DriverData.NvDevVideoFlatPio = 0;
        }
        if (DriverData.NvDevFlatVPE != 0) {
            freeParams.hRoot = DriverData.dwRootHandle;
            freeParams.hObjectParent = NV_WIN_DEVICE;
            freeParams.hObjectOld = NV_DD_DEV_VPE;

            Nv01Free((NV01Free_Param FAR *)&freeParams);

            DriverData.NvDevFlatVPE = 0;
        }
#else   /* MULTI_MON */
        if (DriverData.NvDevFlatDma != 0) {
            NVSYSCLOSE(DriverData.NvDevFlatDma);
            DriverData.NvDevFlatDma = 0;
        }
        if (DriverData.NvDevVideoFlatPio != 0) {
            NVSYSCLOSE(DriverData.NvDevVideoFlatPio);
            DriverData.NvDevVideoFlatPio = 0;
        }
        if (DriverData.NvDevFlatVPE != 0) {
            NVSYSCLOSE(DriverData.NvDevFlatVPE);
            DriverData.NvDevFlatVPE = 0;
        }
#endif  /* MULTI_MON */

#if defined(NVHP) && !defined(WINNT)
    {

#ifdef  MULTI_MON
        NVIOControl_Param   heapParams;

        heapParams.hRoot         = DriverData.dwRootHandle;
        heapParams.hObjectParent = NV_WIN_DEVICE;
        heapParams.owner         = (((DWORD)'NV') << 16) + 'DD';
#else   // MULTI_MON
        NVIOControl_ParamX  heapParams;

        heapParams.device   = 0;
        heapParams.owner[0] = 'DD';
        heapParams.owner[1] = 'NV';
#endif  // MULTI_MON
        heapParams.function = NVIOCONTROL_HEAP_PURGE;
        NvIOControl( NVIOCONTROL_ARCH_HEAP, (LPVOID)&heapParams );
    }
#endif  // NVHP

    return 1;
}
#ifndef WINNT
/***************************************************************************
 *
 * BeginAccess
 *
 ***************************************************************************/

void FAR PASCAL _loadds BeginAccess(DIBENGINE FAR *pde, int left, int top, int right, int bottom, UINT flags)
{
    // call device specific code to access the frame buffer

    //***********************************************************************
    // Call nvPixelDev because it will wait for the NV Engine to complete
    // before continuing on.  We simply do a GetPixel (specify DrawMode=0)
    //
    // lpDestDev         -> DWORD            Need to specify '5250' and VRAM
    // X = 10 ; Y = 20   -> 2 bytes each     Dummy values, no significance
    // Color = 30        -> DWORD            Dummy value, no significance
    // Drawmode = 0      -> DWORD            Drawmode = 0 means GetPixel
    //
    //***********************************************************************
    if (!DriverData.fVidMemLocked)
        HWBeginAccess(DriverData.lpPDevice,10,20,(DWORD)30,(DWORD)0);
}

/***************************************************************************
 *
 * EndAccess
 *
 ***************************************************************************/

void FAR PASCAL _loadds EndAccess(DIBENGINE FAR *pde, UINT flags)
{
    // call device specific code to release the frame buffer
    HWEndAccess();
}

/***************************************************************************
 *
 * Control
 *
 * this is what GDI calls when a app calles Escape or ExtEscape
 * if you dont handle a escape make sure to pass it to the DIBENG
 *
 ***************************************************************************/

LONG FAR PASCAL _loadds Control(
    DIBENGINE FAR * lpDevice,
    UINT            function,
    LPVOID          lpInput,
    LPVOID          lpOutput)
{
    DCICMD        FAR  *pdci;
    unsigned long FAR  *lpResult;
    LPVIDEOPARAMETERS   lpTVparams;

    switch (function)
    {
        case QUERYESCSUPPORT:

            //
            // QUERYESCSUPPORT is sent to ask
            // "do you support this" lpInput points
            // the function code to query
            //
            switch (*(UINT FAR *)lpInput)
            {
                // QUERYDIBSUPPORT MUST BE PASSED ON TO DIB ENGINE

                case DCICOMMAND:
                    return DD_HAL_VERSION;
                    break;

                case OPENGL_GETINFO:
                    return TRUE;  // return non-zero if supported
                    break;

                case OPENGL_CMD:
                    return FALSE; // return non-zero if supported
                    break;

                case VIDEO_PARAMETERS:
                    return TRUE;  // return non-zero if supported
                    break;

                case DVD_MACROVISION_CMD:
                    return TRUE;  // return non-zero if supported
                    break;


            }
            break;

        case GETNVCHANNELPTR:
            lpResult = (unsigned long FAR *)lpOutput;
            *lpResult = (unsigned long)DriverData.NvDevFlatDma;
            DriverData.fActiveExternalUser = TRUE; // NVLIB, NVDDX, etc. is active
            return TRUE;

        case NVGETAGPBASE:
            lpResult = (unsigned long FAR *)lpOutput;
            *lpResult = (DWORD)DriverData.GARTLinearBase;
            return TRUE;

        case NVDMAPUSHMODESWITCHCOUNT:
            lpResult = (unsigned long FAR *)lpOutput;
            *lpResult = (DWORD)
                GetFlatAddress((DWORD)((void FAR *)&(DriverData.dwModeSwitchCount)));
            return TRUE;

        case NVDMAPUSH3DOCCUR:
            lpResult = (unsigned long FAR *)lpOutput;
            *lpResult = (DWORD)
                GetFlatAddress((DWORD)((void FAR *)&(DriverData.ThreeDRenderingOccurred)));
            return TRUE;

        case NVDMAPUSHSYNCNOTIFIER:
            lpResult = (unsigned long FAR *)lpOutput;
            *lpResult = (unsigned long)DriverData.NvDmaPusherSyncNotifierFlat;
            return TRUE;

        case NV_ESC_DVD_MACROVISION_CMD: /* No NV3 generation chip supports Macrovision */
            if ((DriverData.dwMacrovisionNavigatorID != 0) &&
                (DriverData.dwMacrovisionNavigatorID != ((MACROVISION_INFO FAR *)lpInput)->dwMacrovisionNavigatorID)) {
                ((MACROVISION_INFO FAR *)lpOutput)->dwMacrovisionData = FOURCC_UBAD; /* Invalid Navigator ID */
                return FALSE;
            }
            if ((wMonitorType == NV_MONITOR_NTSC) || (wMonitorType == NV_MONITOR_PAL)) {
                if (DriverData.dwMacrovisionNavigatorID == 0) {
                    DriverData.dwMacrovisionNavigatorID = ((MACROVISION_INFO FAR *)lpInput)->dwMacrovisionNavigatorID;
                    return FALSE;
                }
            }
            return TRUE;

        case RECONFIGNVOFFSCREEN:
            DepopulateOffscreenCache(); // in misc.asm

            return TRUE;

        case NVSETDDRAWMODE:
            HWSetMode();  // in ddmode.c

            return TRUE;

        case OPENGL_GETINFO:
            if (*((unsigned long *) lpInput) == OPENGL_GETINFO_DRVNAME) {
                (void) lstrcpy(((LP_OPENGL_INFO) lpOutput)->awch, OPENGL_KEY_NAME );
                ((LP_OPENGL_INFO) lpOutput)->ulVersion = OPENGL_ICD_VER;            // DDI version
                ((LP_OPENGL_INFO) lpOutput)->ulDriverVersion = OPENGL_DRIVER_VER;   // driver version
                return TRUE;
            } else {
                return FALSE;
            }

        case OPENGL_CMD:
            return FALSE;

        case DCICOMMAND:
            pdci = (DCICMD FAR *)lpInput;

            if (pdci == NULL || pdci->dwVersion != DD_VERSION)
                break;

            /*
             * this request gives us our direct draw routines to call
             */
            if (pdci->dwCommand == DDNEWCALLBACKFNS) {
                DriverData.HALCallbacks = *((LPDDHALDDRAWFNS)pdci->dwParam1);
                return TRUE;
            }
            /*
             * return information about our 32-bit DLL
             *
             * we pass a pointer to our shared global Data
             * to the 32-bit driver so we can talk to each
             * other by reading each others mind.
             */
            else if (pdci->dwCommand == DDGET32BITDRIVERNAME) {
                LPDD32BITDRIVERDATA p32dd = (LPDD32BITDRIVERDATA)lpOutput;

                lstrcpy(p32dd->szName, DIRECTDRAW_DLLNAME);
                lstrcpy(p32dd->szEntryPoint, "DriverInit");
                p32dd->dwContext = GetFlatAddress((DWORD)((void FAR *)&DriverData));

                return TRUE;
            }
            /*
             * handle the request to create a driver
             * NOTE we must return our HINSTANCE in *lpOutput
             */
            else if (pdci->dwCommand == DDCREATEDRIVEROBJECT) {
                DDCreateDriverObject(FALSE);
                *(DWORD FAR *)lpOutput = DriverData.HALInfo.hInstance;
                return TRUE;
            }

            break;

        // This is the new Windows98 tv out API extension.
        case VIDEO_PARAMETERS:
            lpTVparams = ((LPVIDEOPARAMETERS) lpInput);

            switch (lpTVparams->dwCommand) {

                case VP_COMMAND_GET:

                    if (lpTVparams->dwFlags & VP_FLAGS_TV_MODE)
                        lpTVparams->dwMode = VP_MODE_WIN_GRAPHICS;

                    if (lpTVparams->dwFlags & VP_FLAGS_TV_STANDARD) {
                        lpTVparams->dwTVStandard = 0;

                        if (wMonitorType == NV_MONITOR_VGA)
                            lpTVparams->dwTVStandard = VP_TV_STANDARD_WIN_VGA;
                        else if (wMonitorType == NV_MONITOR_NTSC)
                            lpTVparams->dwTVStandard = VP_TV_STANDARD_NTSC_M;
                        else if (wMonitorType == NV_MONITOR_PAL)
                            lpTVparams->dwTVStandard = VP_TV_STANDARD_PAL_B;
                    }

                    lpTVparams->dwAvailableModes = VP_MODE_WIN_GRAPHICS;

                    lpTVparams->dwAvailableTVStandard = 0;

                    /* Currently we support only 1 mode at a time controlled by physical connections */
                    if (wMonitorType == NV_MONITOR_VGA)
                        lpTVparams->dwAvailableTVStandard = VP_TV_STANDARD_WIN_VGA;
                    else if (wMonitorType == NV_MONITOR_NTSC)
                        lpTVparams->dwAvailableTVStandard = VP_TV_STANDARD_NTSC_M;
                    else if (wMonitorType == NV_MONITOR_PAL)
                        lpTVparams->dwAvailableTVStandard = VP_TV_STANDARD_PAL_B;

                    if (lpTVparams->dwFlags & VP_FLAGS_FLICKER)
                        lpTVparams->dwFlickerFilter = 0;

                    if (lpTVparams->dwFlags & VP_FLAGS_OVERSCAN) {
                        lpTVparams->dwOverScanX = 0;
                        lpTVparams->dwOverScanY = 0;
                    }

                    if (lpTVparams->dwFlags & VP_FLAGS_MAX_UNSCALED) {
                        lpTVparams->dwMaxUnscaledX = DriverData.bi.biWidth;
                        lpTVparams->dwMaxUnscaledY = DriverData.bi.biHeight;
                    }

                    if (lpTVparams->dwFlags & VP_FLAGS_POSITION) {
                        lpTVparams->dwPositionX = 0;
                        lpTVparams->dwPositionY = 0;
                    }

                    if (lpTVparams->dwFlags & VP_FLAGS_BRIGHTNESS)
                        lpTVparams->dwBrightness = 100;

                    if (lpTVparams->dwFlags & VP_FLAGS_CONTRAST)
                        lpTVparams->dwContrast = 100;

                    if (lpTVparams->dwFlags & VP_FLAGS_COPYPROTECT) {
                        lpTVparams->dwCPType = 0;
                        lpTVparams->dwCPStandard = 0;

                        if (dwTVOutDeviceID == NV_TV_OUT_BT869) {
//                              lpTVparams->dwCPType = VP_CP_TYPE_APS_TRIGGER |
//                                                      VP_CP_TYPE_MACROVISION;
                            lpTVparams->dwCPType = VP_CP_TYPE_APS_TRIGGER;
                            lpTVparams->bCP_APSTriggerBits = macrovisionMode;
                            if (wMonitorType == NV_MONITOR_NTSC)
                                lpTVparams->dwCPStandard = VP_TV_STANDARD_NTSC_M;
                            else if (wMonitorType == NV_MONITOR_PAL)
                                lpTVparams->dwCPStandard = VP_TV_STANDARD_PAL_B;
                        }
                    }

                    return TRUE;
                    break;

                case VP_COMMAND_SET:

                    if (lpTVparams->dwFlags & VP_FLAGS_COPYPROTECT) {
                        /* If not connected to a device that supports Macrovision copy protection */
                        if (dwTVOutDeviceID != NV_TV_OUT_BT869) {
                            return FALSE;
                        }
                        /* Currently we only support APS trigger bits */
                        if (lpTVparams->dwCPType != VP_CP_TYPE_APS_TRIGGER)
                            return FALSE;

                        if (lpTVparams->dwCPCommand == VP_CP_CMD_ACTIVATE) {
                            macrovisionMode = lpTVparams->bCP_APSTriggerBits & 3;
                            if (SetMacrovisionMode(macrovisionMode)) {
                                lpTVparams->dwCPKey = macrovisionKey;
                                return TRUE;
                            }
                        } else if (lpTVparams->dwCPCommand == VP_CP_CMD_DEACTIVATE) {
                            if (lpTVparams->dwCPKey != macrovisionKey)
                                return FALSE;
                            macrovisionMode = 0;
                            if (SetMacrovisionMode(macrovisionMode))
                                return TRUE;
                        } else if (lpTVparams->dwCPCommand == VP_CP_CMD_CHANGE) {
                            if (lpTVparams->dwCPKey != macrovisionKey)
                                return FALSE;
                            macrovisionMode = lpTVparams->bCP_APSTriggerBits & 3;
                            if (SetMacrovisionMode(macrovisionMode))
                                return TRUE;
                        } else return FALSE;
                    }

                    return FALSE;
                    break;

                default:
                    return FALSE;
                    break;
            }

            break;

#ifdef  NV_ESCAPE_ID
        case NV_ESCAPE_ID:
            lpResult = (unsigned long FAR *)lpOutput;
            if (lpResult != NULL) {
                *lpResult = (unsigned long) NV_ESCAPE_ID_RETURN;
                return TRUE;
            } else
                return FALSE;
#endif  // NV_ESCAPE_ID

    }

    if (lpDevice != 0)
        return DIB_Control(lpDevice,function,lpInput,lpOutput);
    else
        return FALSE;
}

/***************************************************************************
 * GetRegInt
 *
 *   read a integer from the HKEY_CURRENT_CONFIG\Display\Settings
 *   key in the registy.
 *
 *   will read a string value and return a integer, if the string is of
 *   the form X,Y will return X<<16+Y
 *
 * Entry:
 *       valname     name of value to read (ie "Resolution")
 *       default     default value to use if not found (ie 0)
 *
 * Returns:
 *       int from registry, or default if not there
 *
 ***************************************************************************/

/*
DWORD GetRegInt(LPSTR valname, DWORD def)
{
    HKEY    hkey;
    char    ach[20];
    DWORD   cb;
    int     i;

    if (RegOpenKey(HKEY_CURRENT_CONFIG, "Display\\Settings", &hkey) == 0)
    {
        ach[0] = 0;
        cb = sizeof(ach);

        if (RegQueryValueEx(hkey, valname, NULL, NULL, ach, &cb) == 0)
        {
            for (def=i=0; ach[i]; i++)
            {
                if (ach[i] >= '0' && ach[i] <= '9')
                    *((WORD*)&def) = LOWORD(def) * 10 + ach[i]-'0';

                if (ach[i] == ',')
                    def = def << 16;
            }
        }

        RegCloseKey(hkey);
    }

    return def;
}
*/
#endif // #ifndef WINNT
/***************************************************************************
 * GetFlatSel
 ***************************************************************************/

UINT GetFlatSel()
{
#ifndef WINNT
    extern WORD PASCAL NVVD_API_DATA;
    return NVVD_API_DATA;
#else
    return(0);
#endif // #ifdef WINNT
}

#ifdef MSDEBUG

#define START_STR "DDMINI: "

void __cdecl DPF(LPSTR szFormat, ...)
{
    static int (WINAPI *fpwvsprintf)(LPSTR lpszOut, LPCSTR lpszFmt, const void FAR* lpParams);
    char        str[256];

    if (fpwvsprintf == NULL)
    {
        fpwvsprintf = (LPVOID) GetProcAddress(GetModuleHandle("USER"),"wvsprintf");
        if (fpwvsprintf == NULL)
            return;
    }

    lstrcpy(str, START_STR);
    fpwvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
    lstrcat(str, "\r\n");
    OutputDebugString(str);
}

#endif
