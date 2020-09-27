/*----------------------------------------------------------------------------*/
/*
 * nvpekmvt.c
 *
 *      NVPE Kernel Mode Video Transport code for the miniVDD.  These functions 
 *      are called from kmvtdisp.asm.
 *
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

// MS includes
#include "windows.h"
#include "ddraw.h"
#include "ddrawi.h"
// #include "nvPusher.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
#include "minivdd.h"
#include "ddkmmini.h"

// NV includes 
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>
#include "nvrmr0api.h"
#include "nvpeos.h"

//
// prototypes
//
DWORD   SkipNextField   (PVOID, PNVP_DDSKIPNEXTFIELDINFO, PVOID);
DWORD   BobNextField    (PVOID, PNVP_DDBOBNEXTFIELDINFO, PVOID);
DWORD   SetState        (PVOID, PNVP_DDSETSTATEININFO, PNVP_DDSETSTATEOUTINFO);
DWORD   LockSurface     (PVOID, PNVP_DDLOCKININFO, PNVP_DDLOCKOUTINFO);
DWORD   FlipOverlay     (PVOID, PNVP_DDFLIPOVERLAYINFO, PVOID);
DWORD   FlipVideoPort   (PVOID, PNVP_DDFLIPVIDEOPORTINFO, PVOID);
DWORD   GetPolarity     (PVOID, PNVP_DDGETPOLARITYININFO, PNVP_DDGETPOLARITYOUTINFO);
DWORD   GetCurrentAutoflip(PVOID, PNVP_DDGETCURRENTAUTOFLIPININFO, PNVP_DDGETCURRENTAUTOFLIPOUTINFO);
DWORD   GetPreviousAutoflip(PVOID, PNVP_DDGETPREVIOUSAUTOFLIPININFO, PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO);
DWORD   NVPTransfer     (PVOID, PNVP_DDTRANSFERININFO, PNVP_DDTRANSFEROUTINFO);
DWORD   NVPGetTransferStatus(PVOID, PVOID, PNVP_DDGETTRANSFEROUTINFO);

extern struct _HW_DEVICE_EXTENSION nvpeDevExtension;

/*
;   DESCRIPTION: If the Mini VDD is already managing the IRQ, this
;          function returns that information; otherwise, it returns the
;          IRQ number assigned to the device so DDraw can manage the IRQ.
;
;          The returning the IRQ number, it is important that it get the
;          value assigned by the Config Manager rather than simply get
;          the value from the hardware (since it can be remapped by PCI).
*/
U032 vddGetIRQInfo(
    U032 dwMiniVDDContext, 
    U032 dummy, 
    DDGETIRQINFO* pGetIrqInfo) 
{
    pGetIrqInfo->dwFlags = IRQINFO_HANDLED;
    return 0;
}

/*
;   DESCRIPTION: Called when the VDD's IRQ handled is triggered.  This
;          determines if the IRQ was caused by our VGA and if so, it
;          clears the IRQ and returns which event(s) generated the IRQ.
*/
U032 vddIsOurIRQ(
    U032 dwMiniVDDContext, 
    U032 dummy) 
{
    return 1;
}

U032 vddEnableIRQ(
    U032 dwMiniVDDContext, 
    DDENABLEIRQINFO* pEnableIrqInfo, 
    U032 dummy) 
{

    nvpeDevExtension.dwIRQSources = pEnableIrqInfo->dwIRQSources;
    nvpeDevExtension.IRQCallback = (PVOID) (pEnableIrqInfo->IRQCallback);
    nvpeDevExtension.dwIRQContext = pEnableIrqInfo->dwContext;

    return 0;
}

U032 vddSkipNextField(
    U032 dwMiniVDDContext, 
    DDSKIPINFO* pSkipInfo, 
    U032 dummy) 
{
    return SkipNextField(&nvpeDevExtension, pSkipInfo, NULL);
}


U032 vddBobNextField(
    U032 dwMiniVDDContext, 
    DDBOBINFO* pBobInfo, 
    U032 dummy) 
{
    return BobNextField(&nvpeDevExtension, pBobInfo, NULL);
}

U032 vddSetState(
    U032 dwMiniVDDContext, 
    DDSTATEININFO* pSetStateInfoIn, 
    DDSTATEOUTINFO* pSetStateInfoOut) 
{
    return SetState(&nvpeDevExtension, pSetStateInfoIn, pSetStateInfoOut);
}

U032 vddLock(
    U032 dwMiniVDDContext, 
    DDLOCKININFO* pLockInfoIn, 
    DDLOCKOUTINFO* pLockInfoOut) 
{
    return LockSurface(&nvpeDevExtension, pLockInfoIn, pLockInfoOut);
}

U032 vddFlipOverlay(
    U032 dwMiniVDDContext, 
    DDFLIPOVERLAYINFO* pFlipOverlayInfo, 
    U032 dummy) 
{
    return FlipOverlay(&nvpeDevExtension, pFlipOverlayInfo, NULL);
}

U032 vddFlipVideoPort(
    U032 dwMiniVDDContext, 
    U032* pFlipVideoPort, 
    U032 dummy) 
{
    return FlipVideoPort(&nvpeDevExtension, (PNVP_DDFLIPVIDEOPORTINFO) pFlipVideoPort, NULL);
}

U032 vddGetFieldPolarity(
    U032 dwMiniVDDContext, 
    DDPOLARITYININFO* pGetPolarityInInfo, 
    DDPOLARITYOUTINFO* pGetPolarityOutInfo) 
{
    return GetPolarity(&nvpeDevExtension, pGetPolarityInInfo, pGetPolarityOutInfo);
}

U032 vddSetSkipPattern(
    U032 dwMiniVDDContext, 
    DDSETSKIPINFO* pSetSkipInfo, 
    U032 dummy) 
{
	return 1;
}

U032 vddGetCurrentAutoFlip(
    U032 dwMiniVDDContext, 
    DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, 
    DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut) 
{
    return GetCurrentAutoflip(&nvpeDevExtension, pGetAutoFlipInfoIn, pGetAutoFlipInfoOut);
}

U032 vddGetPreviousAutoFlip(
    U032 dwMiniVDDContext, 
    DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, 
    DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut) 
{
    return GetPreviousAutoflip(&nvpeDevExtension, pGetAutoFlipInfoIn, pGetAutoFlipInfoOut);
}

U032 vddTransfer(
    U032 dwMiniVddContext, 
    DDTRANSFERININFO* pTransferInInfo, 
    DDTRANSFEROUTINFO* pTransferOutInfo ) 
{
    return NVPTransfer(&nvpeDevExtension, pTransferInInfo, pTransferOutInfo);
}

U032 vddGetTransferStatus(
    U032 dwMiniVddContext, 
    U032 dummy, 
    DDGETTRANSFERSTATUSOUTINFO* pGetTransStatusInfo ) 
{
    return NVPGetTransferStatus(&nvpeDevExtension, NULL, pGetTransStatusInfo);
} 


U032 kmvtNVPCallbackDxApi(void *pContext, U032 dwIRQSource)
{
    HW_DEVICE_EXTENSION *pHwDevCtx = (HW_DEVICE_EXTENSION *) pContext;
    U032 dwIRQContext;

    // can we pass on the interrupt !
    if (pHwDevCtx->IRQCallback)
    {
        dwIRQContext = (U032)(pHwDevCtx->dwIRQContext);
        _asm mov eax, dwIRQSource
        _asm mov ebx, dwIRQContext
        ((PNVP_DX_IRQCALLBACK)(pHwDevCtx->IRQCallback))();
    }
    
    return 0;
}

// A mechanism to populate the WDM prescale values
// This is a back door mechanism and should not really be in this module.
U032 vddWriteVPEPrescale(U032 ulX, U032 ulY, U032 ulCap, U032 ulVPNotUseOverlay, U032 ulFlag)
{
	PNVP_CONTEXT pVPEContext;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "nvpekmvt: vddWriteVPEPrescale()\n");

    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(nvpeDevExtension.avpContexts[0]);
	
	if(ulFlag & 0x8){
		pVPEContext->ulVPECapturePrescaleXFactor=ulX;
	}

	if(ulFlag & 0x4){
		pVPEContext->ulVPECapturePrescaleYFactor=ulY;
	}

	if(ulFlag & 0x2){
		pVPEContext->ulVPECaptureFlag=ulCap;
	}

	if(ulFlag & 0x1){
		pVPEContext->ulVPNotUseOverSurf=ulVPNotUseOverlay;
	}

	return 0;
}
