/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1996,1997,1998,1999. All rights reserved.

    nvpecore.h

    Local definitions used by vpe and kernel mode video transport

    Dec 17, 1999

==============================================================================
*/

#ifdef _WIN32_WINNT

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"
#include "nv32.h"
#include "nvos.h"
#include "nvpeos.h"

#include "nvpekmvt.h"

#else // _WIN32_WINNT

// MS includes
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <vwin32.h>

// NV includes 
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>
#include <nvrmr0api.h>

#include "ddkmmini.h"

#include "nvpeos.h"

#endif // _WIN32_WINNT

//// VPE/kernel interface 
#include "nvpesrvc.h"


#define BUFSIZE	720*(576+4)*2	//size of sys mem buffer for dma xfer

//some debug stuff

extern int DebugLevel;

#ifndef _WIN32_WINNT
#define KdPrint(DBG_STR)	{	_Debug_Printf_Service("NVPECORE: ");	\
								_Debug_Printf_Service(DBG_STR);			\
								_Debug_Printf_Service("\n");}

#define KdPrintValue(DBG_STR, DBG_VALUE)	{	_Debug_Printf_Service("NVPECORE: ");	\
								_Debug_Printf_Service("%s %d", DBG_STR, DBG_VALUE);				\
								_Debug_Printf_Service("\n");}
#else
#define KdPrint(DBG_STR)	{	DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVPECORE: ");	\
								DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, DBG_STR);			\
								DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "\n");}

#define KdPrintValue(DBG_STR, DBG_VALUE)	{	DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVPECORE: ");	\
								DBG_PRINT_STRING_VALUE(DEBUGLEVEL_WARNINGS, DBG_STR, DBG_VALUE);			\
								DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "\n");}
#endif

#define DBGERROR	0
#define DBGWARN		1
#define DBGINFO		2
#define DBGTRACE	3
#define DBGPRINT(DbgLevel, DbgStr)	{if(DebugLevel >= DbgLevel) {KdPrint(DbgStr);}}
#define DBGPRINTVALUE(DbgLevel, DbgStr, DbgValue)	{if(DebugLevel >= DbgLevel) {KdPrintValue(DbgStr, DbgValue);}}
								
#define NVP_CLASS04D_MAX_NOTIFIERS			9
#define NVP_CLASS039_MAX_NOTIFIERS			2
#define NVP_CLASS07A_MAX_NOTIFIERS          3

#ifdef VPEFSMIRROR
// [XW:09/29/2000]
#define NVP_CLASS089_MAX_NOTIFIERS          1
#define NVP_CLASS07C_MAX_NOTIFIERS          9

#define NVP_VIDEO_LUT_DAC_SUBCHANNEL 3
#define NVP_SCALED_IMAGE_SUBCHANNEL  4
#define NVP_FREE_SUBCHANNEL          7

// [XW:09/26/2000] FS Mirror Functions
VP_STATUS NVPIsFsMirrorEnabled(PHW_DEVICE_EXTENSION pHwDevExt,NVP_CMD_OBJECT *pInCmdObj);
VP_STATUS NVPFsRegSurface(PHW_DEVICE_EXTENSION pHwDevExt,NVP_CMD_OBJECT *pCmdObj);
VP_STATUS NVPFsMirrorEnable(PHW_DEVICE_EXTENSION pHwDevExt,NVP_CMD_OBJECT *pCmdObj);
VP_STATUS NVPFsMirrorDisable(PHW_DEVICE_EXTENSION pHwDevExt,NVP_CMD_OBJECT *pCmdObj);
VP_STATUS NVPFsFlip(PNVP_CONTEXT pVPEContext,int nFieldId);
VP_STATUS NVPFsMirror(PNVP_CONTEXT pVPEContext,PNVP_OVERLAY pOverlayInfo,DWORD dwOffset,DWORD dwPitch,DWORD dwWidth,DWORD dwHeight,DWORD dwIndex,DWORD dwFlags);

#ifdef _WIN32_WINNT

void NVPFsCRTCField0Notify(PVOID pHwDeviceExtension);
void NVPFsCRTCField1Notify(PVOID pHwDeviceExtension);
#else

void NVPFsCRTCField0Notify();
void NVPFsCRTCField1Notify();
extern nvpeDevExtension;
#endif


#endif

#ifndef NV01_CONTEXT_DMA
#define NV01_CONTEXT_DMA   NV01_CONTEXT_DMA_FROM_MEMORY
#endif 

#define NVP_VPP_TIME_OUT    10000

///////
/// BUGBUG: taken from dvp.h !!! 
#define DDVP_AUTOFLIP                   0x00000001l
#define DDVP_INTERLEAVE 				0x00000008l
#define DDVP_SKIPEVENFIELDS				0x00000080l
#define DDVP_SKIPODDFIELDS				0x00000100l
#define DDVPCONNECT_INVERTPOLARITY		0x00000004l
#define DDVPCONNECT_INTERLACED			0x00000020l

#define DDFLIP_EVEN                     0x00000002L
#define DDFLIP_ODD                      0x00000004L

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


#ifdef	asmMergeCoords
#undef	asmMergeCoords
#endif
#define asmMergeCoords(LOWWORD, HIWORD) \
        (long)(((long)HIWORD << 16) | (unsigned short)LOWWORD)

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif // mmioFOURCC

///////////
// external services
//

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
#define NVP_JUMP(ADDR)		(0x20000000 | (ADDR))
#define NVP_DECODER_SUBCHANNEL	0
#define NVP_M2M_SUBCHANNEL		1
#define NVP_DMAPUSH_WRITE1(SUBCH, METHOD_OFFSET, METHOD_VALUE)              			\
{\
	pVPEContext->nvDmaFifo[nvDmaCount++] = ((1 <<18) | ((SUBCH)<<13) | (METHOD_OFFSET)); \
    pVPEContext->nvDmaFifo[nvDmaCount++] = METHOD_VALUE;                        			\
}
// assuming the FIFO is at least twice (2x + 1) the size of NEEDED_DWORDS
#define NVP_DMAPUSH_CHECK_FREE_COUNT(NEEDED_DWORDS)		\
{	\
	if ((nvDmaCount + (NEEDED_DWORDS)) >= (pVPEContext->DmaPushBufTotalSize >> 2))	\
	{	\
		pVPEContext->nvDmaFifo[nvDmaCount] = 0x20000000;	\
		nvDmaCount = 0;	\
		DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "nvpecore: DMA wrap around...\n");\
	}	\
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
VP_STATUS NVPProcessState(PHW_DEVICE_EXTENSION pHwDevExt, int nFieldId,	BOOL bVBIField);
VP_STATUS NVPScheduleOverlayFlip(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface, int  nImageId, U032 dwSrcPitch, U032 dwMoveDownOffset);
VP_STATUS DoProgressive(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoWeave(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoBob(PNVP_CONTEXT pVPEContext, int nFieldId);
VP_STATUS DoBobFromInterleaved(PNVP_CONTEXT pVPEContext, int nFieldId);
BOOL NVPGetSurface(	PNVP_CONTEXT pVPEContext,U032 dwSurfOffset, PNVP_SURFACE_INFO *pSurface);
void NVPTransferNotify(PVOID pHwDevExtension);
DWORD NVPCheckTransferStatus(PHW_DEVICE_EXTENSION pHwDevExt);
DWORD NVPCancelTransfer(PNVP_CONTEXT pVPEContext, DDTRANSFERININFO* pInInfo);
DWORD NVPRemoveTransfer(PNVP_CONTEXT pVPEContext, int nTransferIdx);
DWORD NVPAddTransfer(PNVP_CONTEXT pVPEContext, PNVP_DDTRANSFERININFO pInInfo, BOOL bIsOddField);
DWORD NVPGetTransferStatus(PVOID pContext, PVOID pNotUsed, PNVP_DDGETTRANSFEROUTINFO pOutInfo);
DWORD NVPCheckForLaunch(PNVP_CONTEXT pVPEContext, PNVP_SURFACE_INFO pSurface);
DWORD NVPRecreateContext(PNVP_CONTEXT pVPEContext, PMDL pDestMDL);
DWORD NVPTransfer(PVOID pContext, PNVP_DDTRANSFERININFO pInInfo, PNVP_DDTRANSFEROUTINFO pOutInfo);
PVOID GetHwInfo(PVOID HwDevExt);
BOOL NVPPrescaleAndFlip(PNVP_CONTEXT pVPEContext, PNVP_OVERLAY pOverlayInfo, DWORD dwOffset, DWORD dwPitch, DWORD dwIndex, DWORD dwFlags);
VP_STATUS NVPFlipVideoPort(PHW_DEVICE_EXTENSION pHwDevExt, NVP_CMD_OBJECT *pCmdObj);
DWORD kmvtNVPCallbackDxApi(PVOID *pContext, DWORD dwIRQSource);
VP_STATUS NVPInitialize(PHW_DEVICE_EXTENSION pHwDevExt,	NVP_CMD_OBJECT *pCmdObj);
VP_STATUS NVPIsVPEEnabled(PHW_DEVICE_EXTENSION pHwDevExt, NVP_CMD_OBJECT *pInCmdObj, NVP_CMD_OBJECT *pOutCmdObj);

#ifdef _WIN32_WINNT
VP_STATUS NVRegistryCallbackPnpId(PVOID HwDeviceExtension, PVOID Context, PWSTR ValueName, PVOID ValueData, ULONG ValueLength);
#endif

