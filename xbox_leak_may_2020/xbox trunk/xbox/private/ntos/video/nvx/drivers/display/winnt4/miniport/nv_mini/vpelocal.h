/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1996,1997,1998,1999. All rights reserved.

    vpelocal.h

    Local definitions used by vpe and kernel mode video transport

    Jun 08, 1999

==============================================================================
*/

#define INITGUID     // Instantiate GUID_DxApi

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"

typedef unsigned long   DWORD;
// Commented out benign redefinition// typedef int              BOOL;

#include "nv.h"
#include "nv32.h"
#include "nvos.h"

#include "dxmini.h"

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif
#define PDEVICE_OBJECT  PVOID
#include "i2cgpio.h"

//// VPE/miniport interface 
#include "vpemini.h"

/////// VPP interface
//#include "nvreg.h"
//#include "ddvpp.h"
///////

#ifdef _TIMER_TEST
typedef struct _NVP_INIT_OBJECT
{
    Nv03ChannelPio *pChannel;
    U032 uTimerLo;
    U032 uTimerHi;
} NVP_INIT_OBJECT;
#endif

#define NVP_CLASS04D_MAX_NOTIFIERS          9
#define NVP_CLASS039_MAX_NOTIFIERS          2
#define NVP_CLASS07A_MAX_NOTIFIERS          3

#define NVP_VPP_TIME_OUT    10000

///////
/// BUGBUG: taken from dvp.h !!! 
#define DDVP_AUTOFLIP                   0x00000001l
#define DDVP_INTERLEAVE                 0x00000008l
#define DDVP_SKIPEVENFIELDS             0x00000080l
#define DDVP_SKIPODDFIELDS              0x00000100l
#define DDVPCONNECT_INVERTPOLARITY      0x00000004l
#define DDVPCONNECT_INTERLACED          0x00000020l

#define NV_VFM_FORMAT_BOBFROMINTERLEAVE 0x08000000l

/// equivalent in nvReg.h
#define VPP_ODD            0x001
#define VPP_EVEN           0x002
#define VPP_BOB            0x004
#define VPP_INTERLEAVED    0x008
#define VPP_PRESCALE       0x400

#define NV_PRESCALE_OVERLAY_X           1
#define NV_PRESCALE_OVERLAY_Y           2
#define NV_PRESCALE_OVERLAY_FIELD       4

#define IS_FIELD(f) ((f & (VPP_INTERLEAVED | VPP_BOB)) && (f & (VPP_ODD | VPP_EVEN)))

#ifndef NV_IN_PROGRESS
#define NV_IN_PROGRESS (0x8000)
#endif

#define NV_OVERLAY_BYTE_ALIGNMENT_PAD   63L
#define NV_OVERLAY_BYTE_ALIGNMENT       64L


#ifdef  asmMergeCoords
#undef  asmMergeCoords
#endif
#define asmMergeCoords(LOWWORD, HIWORD) \
        (long)(((long)HIWORD << 16) | (unsigned short)LOWWORD)

///////////
// external services
//
extern void KeClearEvent(void *hEvent);
extern U032 RmAllocChannelDma(U032, U032, U032, U032, U032, U032, U032, U032 *);
extern U032 RmAllocObject(U032, U032, U032, U032);
extern U032 RmFree(U032, U032, U032);
extern U032 RmAllocContextDma(U032, U032, U032, U032, U032, U032, U032);
//extern U032 RmAllocEvent(NVOS10_PARAMETERS *);
extern U032 Nv01AllocEvent(NVOS10_PARAMETERS *);
extern U032 RmInterrupt(NVOS09_PARAMETERS *);
extern NTSTATUS miniI2COpen (PDEVICE_OBJECT pDev, ULONG acquire, PI2CControl pI2CCtrl);
extern NTSTATUS miniI2CAccess (PDEVICE_OBJECT pDev, PI2CControl pI2CCtrl);
//#define _MEASURE_TIMING
#ifdef _MEASURE_TIMING
extern int osGetCurrentTime(U032 *year,U032 *month,U032 *day,U032 *hour,U032 *min,U032 *sec,U032 *msec);
#endif

#ifndef NvGetFreeCount
#if defined NV10 || defined NV4
#define NvGetFreeCount(NN, CH)\
         (NN)->subchannel[CH].control.Free
#else   /* !(NV10 || NV4) */
#define NvGetFreeCount(NN, CH)\
         (NN)->subchannel[CH].control.free
#endif  /* !(NV10 || NV4) */
#endif

//////////
// DMA channel macros
//
#define NVP_JUMP(ADDR)      (0x20000000 | (ADDR))
#define NVP_DECODER_SUBCHANNEL  0
#define NVP_M2M_SUBCHANNEL      1
#define NVP_DMAPUSH_WRITE1(SUBCH, METHOD_OFFSET, METHOD_VALUE)                          \
{\
    pVPEContext->nvDmaFifo[nvDmaCount++] = ((1 <<18) | ((SUBCH)<<13) | (METHOD_OFFSET)); \
    pVPEContext->nvDmaFifo[nvDmaCount++] = METHOD_VALUE;                                    \
}
// assuming the FIFO is at least twice (2x + 1) the size of NEEDED_DWORDS
#define NVP_DMAPUSH_CHECK_FREE_COUNT(NEEDED_DWORDS)     \
{   \
    if ((nvDmaCount + (NEEDED_DWORDS)) >= (pVPEContext->DmaPushBufTotalSize >> 2))  \
    {   \
        pVPEContext->nvDmaFifo[nvDmaCount] = 0x20000000;    \
        nvDmaCount = 0; \
        VideoDebugPrint((0,"VPEMINI: DMA wrap around...\n"));   \
    }   \
}

////////////
// local functions prototypes
//
void VideoField0Notify(PVOID pHwDeviceExtension);
void VideoField1Notify(PVOID pHwDeviceExtension);
void VBIField0Notify(PVOID pHwDeviceExtension);
void VBIField1Notify(PVOID pHwDeviceExtension);
void NVPProgVideoField(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface, int nFieldId, U032 dwSurfPitch, U032 dwInterleaveOffset, BOOL bGrab);
void NVPProgVBIField(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface, int nFieldId);
VP_STATUS NVPProcessState(PHW_DEVICE_EXTENSION pHwDevExt, int nFieldId, BOOL bVBIField);
VP_STATUS NVPScheduleOverlayFlip(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface, int  nImageId, U032 dwSrcPitch, U032 dwMoveDownOffset);
U032 NVPScaleDown(PNVP_CONTEXT pVPEContext, PNVP_OVERLAY pOvr, int nFieldId);
VP_STATUS DoProgressive(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoWeave(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoBob(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoBobFromInterleaved(PNVP_CONTEXT pVPEContext, int nFieldId);
BOOL NVPGetSurface( PNVP_CONTEXT pVPEContext,U032 dwSurfOffset, PNVP_SURFACE_INFO *pSurface);
void NVPTransferNotify(PVOID pHwDevExtension);
DWORD NVPCheckTransferStatus(PHW_DEVICE_EXTENSION pHwDevExt);
DWORD NVPCancelTransfer(PNVP_CONTEXT pVPEContext, PDDTRANSFERININFO pInInfo);
DWORD NVPRemoveTransfer(PNVP_CONTEXT pVPEContext, int nTransferIdx);
DWORD NVPAddTransfer(PNVP_CONTEXT pVPEContext, PDDTRANSFERININFO pInInfo, BOOL bIsOddField);
DWORD NVPGetTransferStatus(PVOID pContext, PVOID pNotUsed, PDDGETTRANSFEROUTINFO pOutInfo);
DWORD NVPCheckForLaunch(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface);
DWORD NVPRecreateContext(PNVP_CONTEXT pVPEContext, PMDL pDestMDL);
DWORD NVPTransfer(PVOID pContext, PDDTRANSFERININFO pInInfo, PDDTRANSFEROUTINFO pOutInfo);
PVOID GetHwInfo(PVOID HwDevExt);
BOOL NVPPrescaleAndFlip(PNVP_CONTEXT pVPEContext, PNVP_OVERLAY pOverlayInfo, DWORD dwOffset, DWORD dwPitch, DWORD dwIndex, DWORD dwFlags);
VP_STATUS NVPFlipVideoPort(PHW_DEVICE_EXTENSION pHwDevExt, NVP_CMD_OBJECT *pCmdObj);

/////////////////
// prototypes
//
DWORD GetIrqInfo(
    PVOID HwDeviceExtension,
    PVOID lpInput,
    PDDGETIRQINFO GetIrqInfo);

VP_STATUS NVQueryInterface(
    PVOID HwDeviceExtension,
    PQUERY_INTERFACE QueryInterface);
