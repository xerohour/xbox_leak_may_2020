//***************************************************************************
//  Module Name:
//
//      vpemini.c
//
//  Abstract:
//
//      This module contains the miniport parts of the DXAPI "kernel-mode
//      VPE" interface for the .
//
//      The DirectDraw Video Port Extensions (VPE) are the services that
//      control hardware videoports, where video data is streamed into
//      off-screen memory from an external device such as a TV tuner or
//      MPEG decoder, and then displayed on the screen by a video overlay.
//
//      To support VPE, you'll need the following:
//
//          1. A 2-D driver with DirectDraw overlay support;
//          2. A video input device that interfaces with the videoport;
//          3. A bunch of VPE code in the display driver;
//          4. A bunch of VPE code here in the miniport.
//
//      The miniport code here comprises part of the DXAPI "kernel-mode
//      VPE" interface that can be called by other WDM drivers, such as
//      an MPEG driver.
//
// IMPORTANT NOTE ABOUT HARDWARE REQUIREMENTS:
//
//      Almost all of these miniport "kernel-mode VPE" routines may be
//      called asynchronously to what the display driver is currently
//      doing.  For example, a thread may be in the display driver in the
//      middle of programming an accelerated Blt at the exact moment that
//      the miniport gets called to do a videoport flip call.  Consequently,
//      your hardware MUST be able to handle these commands at any time
//      regardless of the state of the display driver.  So this means:
//
//          1. Your hardware has to be able to allow the programming of any
//             "kernel-mode VPE" command even while Blts or direct frame
//             buffer accesses or DMAs or whatever are occuring, without
//             hanging or incorrect drawing.
//
//          2. The registers used to program any of the "kernel-mode VPE"
//             services must be disjoint and separate from those used by
//             the display driver.  If you must program an indexed register
//             that is also used by the display driver (such as 0x3d4/0x3d5),
//             *YOU ARE PROBABLY TOAST*.  Both a thread in the display driver
//             and a thread calling these miniport services are preemptible,
//             meaning that even if you save and restore the index register,
//             there's no guarantee that the other thread won't wake up in
//             the middle and try to use the same (now incorrectly set)
//             register.
//
//      If your hardware does not guarantee both of the above, you CANNOT
//      support the "kernel-mode VPE" interface!  Go yell at your hardware
//      designers and get them to fix the hardware.  If both of the above
//      are not guaranteed, it's inevitable that your customers will randomly
//      hit system hangs and crashes while using VPE.  Your customers will
//      not be happy!
//
//      Also note that the likelihood of hitting a problem is significantly
//      increased when running on a multiple-processor machine.  Consequently,
//      you should try to test VPE on an MP machine while applications (such
//      as Winstone) are running in the background.
//
//      (Okay, okay, so I exaggerated a bit about pointer number 2.  You can
//      in fact use shared indexed registers in these VPE routines if you do
//      one of the following:
//
//          1. Disable VPE on multiple-processor machines.  (As of the time
//             of this writing, we haven't yet implemented a VideoPort or
//             Eng function that you can call to determine whether you're
//             running on a UP or MP machine, otherwise I would tell you
//             the name of the function to call.  But by the time you read
//             this, it will probably exist -- so have a look through the
//             VideoPort and Eng calls...)
//
//             You'll also have to save and restore the index register when
//             you're done.  Because a thread that calls a miniport function
//             is at raised IRQL, it may interrupt a thread in the display
//             driver at any point, but on a UP machine it's guaranteed that
//             the display driver thread will not interrupt the miniport
//             thread.
//
//          2. Use the CRTC register only in the DxEnableIrq routine --
//             DirectDraw automatically synchronizes with the display driver
//             before calling DxEnableIrq (this is not true for any other
//             Dx miniport calls).
//
// NOTE:
//
//      Most "kernel-mode VPE" routines can be called at raised IRQL, meaning
//      that they MUST be non-paged, and cannot call any pageable functions.
//
//  Environment:
//
//      Kernel mode
//
//  Copyright (c) 1997 Microsoft Corporation
//
//***************************************************************************

#ifdef NVPE

// Temporary fix for Compile error...ask!
#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free

#include "vpelocal.h"

//#define _NVP_DTRACE

#ifdef _NVP_DTRACE
#define DEFINE_GLOBALS
#include "dtrace.h"
#endif


////////// globals
BOOL g_bFlip = FALSE;

extern void KeClearEvent(void *hEvent);


//
// Mark those routines that we can as pageable.  Remember that most
// "kernel-mode VPE" routines can be called at raised IRQL, which means
// that they must be kept non-paged.
//
// Note that these are the only two VPE miniport routines which may be
// pageable:
//

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE,NVQueryInterface)
#pragma alloc_text(PAGE,GetIrqInfo)
#endif

// !!! Not hooking Lock is success, but not hooking SetPattern is failure?
// !!! Should dwFlags be ANDed or EQUALed?  (SkipNextField)
// !!! Should we mark the interrupt as unsharable?


/*++

Routine Description:

    On NT, the miniport always has to "manage the IRQ" (to borrow from the
    Win95 terminology).  Consequently, this routine simply tells DirectDraw
    what interrupts we support.

    It's actually pretty easy to enable interrupts, in part because you
    don't have to tell the system what interrupt vector you're using, as
    PNP already knows that.  However, you *do* have to check whether PNP
    successfully assigned you an interrupt vector, before you tell DirectDraw
    that you support interrupts.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetIrqInfo - Function parameters.

Return Value:

    DX_OK if successful.

--*/

DWORD GetIrqInfo(
    PVOID HwDeviceExtension,
    PVOID lpInput,
    PDDGETIRQINFO GetIrqInfo)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DWORD status = DXERR_UNSUPPORTED;

    //
    // Sometimes, the motherboard BIOS or PNP may conspire to refuse to
    // allocate an interrupt to us.  So before we tell DirectDraw that
    // we support interrupts, we must first check if we've actually been
    // assigned one!
    //

//     if ((hwDeviceExtension->BusInterruptLevel != 0) ||
//        (hwDeviceExtension->BusInterruptVector != 0)) 
	{

        GetIrqInfo->dwFlags = IRQINFO_HANDLED;

        status = DX_OK;
	}

    return status;
}

/*++

Routine Description:

    This routine is the interrupt service routine for the kernel video
    driver.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's adapter information.

Return Value:

    TRUE if the interrupt is serviced; FALSE if it wasn't ours (this is done
    to support chained interrupts).

--*/

BOOLEAN Interrupt(PVOID HwDeviceExtension)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    // add your IRQ handling code here
	return FALSE;
}

/*++

Routine Description:

    This function notifies the miniport when it should enable or disable
    interrupts for VPE.  If an IRQ is not specified in this call, it
    should be disabled.

    On NT, since miniports always "manage the IRQ", we must call DirectDraw's
    IRQCallback routine from our HwInterrupt routine to notify DirectDraw
    whenever an interrupt occurs.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    EnableIrqInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD EnableIrq(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PDDENABLEIRQINFO EnableIrqInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    //
    // First make sure we stash IRQCallback in our device extension before
    // enabling interrupts for the first time, otherwise we may call a NULL
    // pointer from our HwInterrupt routine!
    //
    // (Note that DirectDraw will give us a valid IRQCallback and lpIRQData
    // even when disabling interrupts, which means we won't crash if an
    // interrupt is taken between this point and the actual disabling of the
    // interrupt.)
    //

    hwDeviceExtension->IRQCallback = EnableIrqInfo->IRQCallback;
    hwDeviceExtension->pIRQData = EnableIrqInfo->lpIRQData;
	hwDeviceExtension->dwIRQSources = EnableIrqInfo->dwIRQSources;

    //
    // Clear any existing interrupts, turn off all interrupts by default,
    // and enable the new requested interrupts.
    //
    // NOTE: Be very careful if your driver uses interrupts for purposes
    //       other than VPE.  For example, if your display driver uses DMA,
    //       you probably won't want to clear the DMA interrupt flag here!
    //

    // Now enable the master interrupt switch.  Again, be careful if your
    // display driver uses interrupt for purposes other than VPE -- you
    // probably won't want to disable all video interrupts here like we
    // do!
    //
    // Note: DxEnableIrq is the only routine that may access the CRTC
    //       registers!  For more details, read the large note at the
    //       top of this file.
    //

    // If the vsync interrupt has been disabled, but a deferred overlay
    // offset is pending, flush it now.
    //

    return DX_OK;
}

/*++

Routine Description:

    This function flips the video port to the target surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    FlipVideoPortInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD FlipVideoPort(
    PVOID HwDeviceExtension,
    PDDFLIPVIDEOPORTINFO FlipVideoPortInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    ULONG offset;

	VideoDebugPrint((0,"VPEMINI: FlipVideoPort()\n"));

    //
    // Get offset (from framebuffer) and add offset from surface:
    //

    offset = FlipVideoPortInfo->lpTargetSurface->dwSurfaceOffset
           + FlipVideoPortInfo->lpVideoPortData->dwOriginOffset;

    // flip the actual hw surface here

    return DX_OK;
}

/*++

Routine Description:

    This function flips the overlay to the target surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    FlipOverlayInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD FlipOverlay(
    PVOID HwDeviceExtension,
    PDDFLIPOVERLAYINFO pFlipOverlayInfo,
    PVOID lpOutput)
{
#if 0
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
	DDSURFACEDATA* pCurSurf =	(DDSURFACEDATA*) pFlipOverlayInfo->lpCurrentSurface;
	DDSURFACEDATA* pTargSurf =	(DDSURFACEDATA*) pFlipOverlayInfo->lpTargetSurface;
	PNVP_CONTEXT pVPEContext;
	U032 dwFlags;
	NVP_OVERLAY pOvInfo;

	VideoDebugPrint((1,"VPEMINI: FlipOverlay()\n"));

	// get a pointer to the appropriate VPE context (assume VPE 0)
	pVPEContext = &(pHwDevExt->avpContexts[0]);

	pOvInfo = &(pVPEContext->nvpOverlay);

    // VPP flags
	dwFlags = VPP_PRESCALE;

	if (pTargSurf->dwOverlayFlags & DDOVER_INTERLEAVED)
	   dwFlags |= VPP_INTERLEAVED;

	if (pTargSurf->dwOverlayFlags & DDOVER_BOB)
	   dwFlags |= VPP_BOB;

    if (pFlipOverlayInfo->dwFlags & DDFLIP_ODD)
	   dwFlags |= VPP_ODD;
	else if (pFlipOverlayInfo->dwFlags & DDFLIP_EVEN)
	   dwFlags |= VPP_EVEN;


    // if not initialized 
/*
    NVP_CMD_OBJECT cmdObj;
	cmdObj.dwPortId = 0;
	cmdObj.dwChipId = pHwDevExt->ulChipID;
	cmdObj.dwDeviceId = ppdev->hDriver;
	cmdObj.hClient = ppdev->hClient;

    NVPInitialize(pHwDevExt, &cmdObj);
*/
    // initialize index 
    if (pVPEContext->nvpOverlay.dwOverlayBufferIndex == NVP_UNDEF)
        pVPEContext->nvpOverlay.dwOverlayBufferIndex = 0;

    // flip overlay here !
	if (NVPPrescaleAndFlip(pVPEContext, pOvInfo, pTargSurf->dwOverlayOffset, pTargSurf->lPitch, 0, dwFlags))
	    return DX_OK;

    return DXERR_GENERIC;
#else
	return DX_OK;
#endif
}


/*++

Routine Description:

    This function is called when "bob" is used and a VPORT VSYNC occurs
    that does not cause a flip to occur (e.g., bobbing while interleaved).
    When bobbing, the overlay must adjust itself on every VSYNC, so this
    function notifies it of the VSYNCs that it doesn't already know about
    (e.g., VSYNCs that trigger a flip to occur).

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    BobNextFieldInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD BobNextField(
    PVOID HwDeviceExtension,
    PDDBOBNEXTFIELDINFO BobNextFieldInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

	VideoDebugPrint((2,"VPEMINI: BobNextField()\n"));

    return DX_OK;
}

/*++

Routine Description:

    This function is called when the client wants to switch from bob to
    weave.  The overlay flags indicate which state to use.  This is only
    called for interleaved surfaces.

    Note that when this is called, the specified surface may not be
    displaying the overlay (due to a flip).  Instead of failing the call,
    change the bob/weave state for the overlay that would be used if the
    overlay was flipped again to the specified surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    SetStateInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD SetState(
    PVOID HwDeviceExtension,
    PDDSETSTATEININFO SetStateInInfo,
    PDDSETSTATEOUTINFO SetStateOutInfo)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

	VideoDebugPrint((0,"VPEMINI: SetState()\n"));

    return DX_OK;
}


/*++

Routine Description:

    This function is called when the client wants to skip the next field,
    usually to undo a 3:2 pulldown but also for decreasing the frame
    rate.  The driver should not lose the VBI lines if dwVBIHeight contains
    a valid value.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    SkipNextFieldInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD SkipNextField(
    PVOID HwDeviceExtension,
    PDDSKIPNEXTFIELDINFO SkipNextFieldInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

	VideoDebugPrint((0,"VPEMINI: SkipNextField()\n"));

    return DX_OK;
}


/*++

Routine Description:

    This function returns the polarity of the current field being written
    to the specified video port.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetPolarityInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD GetPolarity(
    PVOID HwDeviceExtension,
    PDDGETPOLARITYININFO pGetPolarityInInfo,
    PDDGETPOLARITYOUTINFO pGetPolarityOutInfo)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION) HwDeviceExtension;
	PNVP_CONTEXT pVPEContext;

	VideoDebugPrint((3,"VPEMINI: GetPolarity()\n"));

	// get a handle to current VPE context (BUGBUG: assume index 0 for now !)
	pVPEContext = &(pHwDevExt->avpContexts[0]);

	// check video first !
	if (pVPEContext->nNumVidSurf)
	{
		// TRUE for Even, FALSE for Odd
		if (pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType == NVP_FIELD_ODD)
			pGetPolarityOutInfo->bPolarity = FALSE;
		else
			pGetPolarityOutInfo->bPolarity = TRUE;
	}
	// vbi polarity 
	else if (pVPEContext->nNumVBISurf)
	{
		if (pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType == NVP_FIELD_ODD)
			pGetPolarityOutInfo->bPolarity = FALSE;
		else
			pGetPolarityOutInfo->bPolarity = TRUE;
	}
	else
		return DXERR_GENERIC;

    return DX_OK;
}

/*++

Routine Description:

    This function returns the current surface receiving data from the
    video port while autoflipping is taking place.  Only called when
    hardware autoflipping.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetCurrentAutoflipInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD GetCurrentAutoflip(
    PVOID HwDeviceExtension,
    PDDGETCURRENTAUTOFLIPININFO pGetCurrentAutoflipInInfo,
    PDDGETCURRENTAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
	int nNotifyIdx;

	VideoDebugPrint((3,"VPEMINI: GetCurrentAutoflip()\n"));

	// get a handle to current VPE context (BUGBUG: assume index 0 for now !)
	pVPEContext = &(pHwDevExt->avpContexts[0]);
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

	pGetAutoFlipInfoOut->dwSurfaceIndex = NVP_UNDEF;
	// check out video 
	if (pVPEContext->nNumVidSurf)
	{
		nNotifyIdx = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType);
		// check progress status of current surface
		if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
			pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->nVidSurfIdx);
		else
			// assume it is the next surface then !
			pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].nNextSurfaceIdx);
	}

	pGetAutoFlipInfoOut->dwVBISurfaceIndex = NVP_UNDEF;
	// check out vbi
	if (pVPEContext->nNumVBISurf)
	{
		nNotifyIdx = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType);
		// check progress status of current surface
		if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
			pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->nVBISurfIdx);
		else
			// assume it is the next surface then !
			pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].nNextSurfaceIdx);
	}

    return DX_OK;
}


/*++

Routine Description:

    This function returns the current surface receiving data from the
    video port while autoflipping is taking place.  Only called when
    hardware autoflipping.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetPreviousAutoflipInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD GetPreviousAutoflip(
    PVOID HwDeviceExtension,
    PDDGETPREVIOUSAUTOFLIPININFO pGetAutoflipInInfo,
    PDDGETPREVIOUSAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
	int nNotifyIdx;

	VideoDebugPrint((3,"VPEMINI: GetPreviousAutoflip()\n"));

	// get a handle to current VPE context (BUGBUG: assume index 0 for now !)
	pVPEContext = &(pHwDevExt->avpContexts[0]);
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

	pGetAutoFlipInfoOut->dwSurfaceIndex = NVP_UNDEF;
	// check out video 
	if (pVPEContext->nNumVidSurf)
	{
		nNotifyIdx = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType);
		// check progress status of current surface
		if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
			// if in progress, return index of previous surface
			pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)((pVPEContext->nVidSurfIdx == 0) ? (pVPEContext->nNumVidSurf - 1): (pVPEContext->nVidSurfIdx - 1));
		else
			// assume this is the "previous"ly autoflipped surface then !
			pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->nVidSurfIdx);
	}

	pGetAutoFlipInfoOut->dwVBISurfaceIndex = NVP_UNDEF;
	// check out vbi
	if (pVPEContext->nNumVBISurf)
	{
		nNotifyIdx = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType);
		// check progress status of current surface
		if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
			// if in progress, return index of previous surface
			pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)((pVPEContext->nVBISurfIdx == 0) ? (pVPEContext->nNumVBISurf - 1): (pVPEContext->nVBISurfIdx - 1));
		else
			// assume this is the "previous"ly autoflipped surface then !
			pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->nVBISurfIdx);
	}

    return DX_OK;
}


/*++

Routine Description:

    Generic method for obtaining interfaces from the miniport.

    Among other things, it's used for exporting the DXAPI "kernel-mode
    VPE" interface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    QueryInterface - Pointer to structure that describes the requested
        interface.

Return Value:

    DX_OK if successful.

--*/
VP_STATUS NVQueryInterface(
    PVOID HwDeviceExtension,
    PQUERY_INTERFACE QueryInterface)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    VP_STATUS status;

    status = DXERR_UNSUPPORTED;

    if (IsEqualGUID(QueryInterface->InterfaceType, &GUID_DxApi)) 
    {
	    PDXAPI_INTERFACE DxApiInterface;
        DxApiInterface = (PDXAPI_INTERFACE) QueryInterface->Interface;

        //
        // We are queried here for our "kernel-mode VPE" support only after
        // the display driver has already enabled VPE.  Consequently, we let
        // the display driver control what chips VPE is enabled on -- we
        // don't have to do that here.
        //

        DxApiInterface->Size    = sizeof(DXAPI_INTERFACE);
        DxApiInterface->Version = DXAPI_HALVERSION;
        DxApiInterface->Context = hwDeviceExtension;

        DxApiInterface->DxGetIrqInfo          = GetIrqInfo;
        DxApiInterface->DxEnableIrq           = EnableIrq;
        DxApiInterface->DxSkipNextField       = SkipNextField;
        DxApiInterface->DxBobNextField        = BobNextField;
        DxApiInterface->DxSetState            = SetState;
        DxApiInterface->DxFlipOverlay         = FlipOverlay;
        DxApiInterface->DxFlipVideoPort       = FlipVideoPort;
        DxApiInterface->DxGetPolarity         = GetPolarity;
        DxApiInterface->DxGetCurrentAutoflip  = GetCurrentAutoflip;
        DxApiInterface->DxGetPreviousAutoflip = GetPreviousAutoflip;
		DxApiInterface->DxTransfer			  = NVPTransfer;
		DxApiInterface->DxGetTransferStatus	  = NVPGetTransferStatus;

        status = DX_OK;
    }
	// I2c Interface
	else if (IsEqualGUID(QueryInterface->InterfaceType, &GUID_I2C_INTERFACE))
	{
		I2CINTERFACE *pI2cInterface;
        pI2cInterface = (I2CINTERFACE *) QueryInterface->Interface;


		//pI2cInterface->_vddInterface;
    	pI2cInterface->i2cOpen = (I2COPEN) miniI2COpen;
    	pI2cInterface->i2cAccess = (I2CACCESS) miniI2CAccess;

        status = DX_OK;
	}

    return status;
}

/*
==============================================================================
	
	NVP3Initialize

	Description:	Initializes the NV video port. Creates channels and DMA
					contexts. Create Video and VBI objects.
					NV3 version uses PIO channel

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVP3Initialize(
	PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
	NVOS10_PARAMETERS evParam;
	int nFreeCount;
	U032 status;
	NVOS09_PARAMETERS ntfParam;
	Nv03ChannelPio *pNVPChannel = (Nv03ChannelPio *) (pCmdObj->pChannel);

//////////// HACK
	g_bFlip = FALSE;
///////////

	VideoDebugPrint((3,"VPEMINI: NVP3Initialize()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
	// initialize context
	pVPEContext->pNVPChannel = (ULONG) pNVPChannel;	// keep a copy of the pio channel pointer
	pVPEContext->nvDmaFifo = NULL;
	pVPEContext->dwDeviceId = pCmdObj->dwDeviceId;
	pVPEContext->hClient = pCmdObj->hClient;
	pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;
	pVPEContext->dwOvrSubChannel = pCmdObj->dwOvrSubChannel;
	pVPEContext->dwOverlayObjectID = pCmdObj->dwOverlayObjectID;
	pVPEContext->dwScalerObjectID = pCmdObj->dwScalerObjectID;
	pVPEContext->dwDVDSubPicObjectID = pCmdObj->dwDVDObjectID;
    pVPEContext->dwDVDObjectCtxDma = pCmdObj->dwDVDObjectCtxDma;
	pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = pCmdObj->pNvPioFlipOverlayNotifierFlat;
	pVPEContext->nNumVidSurf = 0;
	pVPEContext->nNumVBISurf = 0;
	pVPEContext->dwVPState = NVP_STATE_STOPPED;
	pVPEContext->dwVPConFlags = pCmdObj->dwVPTypeFlags;
	pVPEContext->dwVPFlags = 0; //pCmdObj->dwVPInfoFlags;
	pVPEContext->nCurTransferIdx = NVP_EOQ;
	pVPEContext->nLastTransferIdx = NVP_EOQ;
	pVPEContext->nRecentTransferIdx = NVP_EOQ;
	pVPEContext->nNumTransfer = 0;
	pVPEContext->bxfContextCreated = FALSE;
	pVPEContext->bxfContextFlushed = FALSE;
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

	// alloc external decoder object
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_EXTERNAL_DECODER_OBJECT,
				NV03_EXTERNAL_VIDEO_DECODER) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate External Decoder Object 0x%X\n", NVP_EXTERNAL_DECODER_OBJECT));
		return FALSE;
	}

	// allocate transfer object (used for vbi and video capture)
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_M2M_OBJECT,
				NV03_MEMORY_TO_MEMORY_FORMAT) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate Mem to mem transfer Object 0x%X\n", NVP_M2M_OBJECT));
		return FALSE;
	}

    // allocate the transfer FROM context
    if (RmAllocContextDma(
						pCmdObj->hClient,
                        NVP_M2M_FROM_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
						0, // selector
                        (U032)pCmdObj->pFrameBuffer,
                        pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for M2M transfer Object\n"));
        return FALSE;
	}

    // allocate a notify context for transfer object
    if (RmAllocContextDma(
    					pCmdObj->hClient,
                        NVP_M2M_NOTIFIER,
                        NV01_CONTEXT_DMA_FROM_MEMORY,
                        0, 	//(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032)(pVPEContext->axfNotifiers),
                        (sizeof( NvNotification)*NVP_CLASS039_MAX_NOTIFIERS  - 1)) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for M2M transfer Object\n"));
        return FALSE;
    }
	
	// allocate a notify context for external decoder
	if (RmAllocContextDma(
					pCmdObj->hClient,
	         		NVP_EXTERNAL_DECODER_NOTIFIER,
	         		NV01_CONTEXT_DMA_FROM_MEMORY,
	         		0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
					0, // selector
	         		(U032)(pVPEContext->avpNotifiers),
	         		(sizeof(NvNotification)*NVP_CLASS04D_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for external decoder object\n"));
		return FALSE;
	}

	// context DMA for video field0
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VIDEO_FIELD0_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
				pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for video field0\n"));
		return FALSE;
	}

	// context DMA for video field1	
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VIDEO_FIELD1_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
	            pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for video field1\n"));
		return FALSE;
	}
	
	// context DMA for video field0
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VBI_FIELD0_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
				pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for VBI field0\n"));
		return FALSE;
	}

	// context DMA for video field1
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VBI_FIELD1_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
	            pCmdObj->dwFBLen - 1) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for VBI field1\n"));
		return FALSE;
	}

	//CHECK_FREE_COUNT(pNVPChannel,10*5);
	nFreeCount = NvGetFreeCount(pNVPChannel, 0);
	while (nFreeCount < 13*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pNVPChannel, 0);
	}			
	pVPEContext->nFreeCount = nFreeCount - (13*4);

	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	// transfer object setup
    pNVPChannel->subchannel[5].SetObject  = NVP_M2M_OBJECT;
    pNVPChannel->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaNotifies = NVP_M2M_NOTIFIER;
    pNVPChannel->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferIn = NVP_M2M_FROM_CONTEXT;
	
	// make sure decoder object is in the channel....
	pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaNotifies = NVP_EXTERNAL_DECODER_NOTIFIER;

	// plug in our buffer contexts
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[0] = NVP_VIDEO_FIELD0_BUFFER_CONTEXT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[1] = NVP_VIDEO_FIELD1_BUFFER_CONTEXT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[0]   = NVP_VBI_FIELD0_BUFFER_CONTEXT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[1]   = NVP_VBI_FIELD1_BUFFER_CONTEXT;
	
	// NB: we don't want to launch any captures at this point

	// setup the transfer callback
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_M2M_OBJECT;
  	evParam.hObjectNew = NVP_M2M_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV039_NOTIFIERS_BUFFER_NOTIFY;
  	evParam.hEvent.low = (ULONG) NVPTransferNotify;
	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for m2m transfer 0\n"));
		return FALSE;
	}

	// setup fields callbacks
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VIDEO_FIELD0_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
  	evParam.hEvent.low = (ULONG) VideoField0Notify;

	// video field0
	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for video field 0\n"));
		return FALSE;
	}

	// video field1
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VIDEO_FIELD1_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
  	evParam.hEvent.low = (ULONG) VideoField1Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for video field 1\n"));
		return FALSE;
	}

	// vbi field0
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VBI_FIELD0_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0);
  	evParam.hEvent.low = (ULONG) VBIField0Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for VBI field 0\n"));
		return FALSE;
	}
			
	// vbi field1
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VBI_FIELD1_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1);
  	evParam.hEvent.low = (ULONG) VBIField1Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for VBI field 1\n"));
		return FALSE;
	}

	return NO_ERROR;
}


/*
==============================================================================
	
	NVPInitialize

	Description:	Initializes the NV video port. Creates channels and DMA
					contexts. Create Video and VBI objects.
					DMA push buffer version (NV ver >= 0x04)

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPInitialize(
	PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
    NvNotification *aovNotifiers;
	U032 dwOvObj;
	NVOS10_PARAMETERS evParam;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;
	U032 status;

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pCmdObj->dwChipId) <= NVP_CHIP_VER_03)
		return NVP3Initialize(pHwDevExt, pCmdObj);

//////////// HACK
	g_bFlip = FALSE;
///////////

	VideoDebugPrint((3,"VPEMINI: NVPInitialize()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
	// initialize context
	//pVPEContext->pNVPChannel = (ULONG) 0;
	pVPEContext->dwChipId = pCmdObj->dwChipId;
	pVPEContext->dwDeviceId = pCmdObj->dwDeviceId;
	pVPEContext->hClient = pCmdObj->hClient;
	pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;
	pVPEContext->dwOvrSubChannel = pCmdObj->dwOvrSubChannel;
	pVPEContext->dwOverlayObjectID = pCmdObj->dwOverlayObjectID;
	pVPEContext->dwScalerObjectID = pCmdObj->dwScalerObjectID;
	pVPEContext->dwDVDSubPicObjectID = pCmdObj->dwDVDObjectID;
    pVPEContext->dwDVDObjectCtxDma = pCmdObj->dwDVDObjectCtxDma;
	pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = pCmdObj->pNvPioFlipOverlayNotifierFlat;
	pVPEContext->nNumVidSurf = 0;
	pVPEContext->nNumVBISurf = 0;
	pVPEContext->dwVPState = NVP_STATE_STOPPED;
	pVPEContext->dwVPConFlags = pCmdObj->dwVPTypeFlags;
	pVPEContext->dwVPFlags = 0; //pCmdObj->dwVPInfoFlags;
	pVPEContext->nCurTransferIdx = NVP_EOQ;
	pVPEContext->nLastTransferIdx = NVP_EOQ;
	pVPEContext->nRecentTransferIdx = NVP_EOQ;
	pVPEContext->nNumTransfer = 0;
	pVPEContext->bxfContextCreated = FALSE;
	pVPEContext->bxfContextFlushed = FALSE;
	pVPEContext->hVPSyncEvent = pCmdObj->hVPSyncEvent;

	pVPEContext->nvDmaFifo = NULL;
	pVPEContext->DmaPushBufTotalSize = 32*1024*4;	// 32k DWORDs
	if (VideoPortAllocateBuffer((PVOID)pHwDevExt, pVPEContext->DmaPushBufTotalSize, (PVOID *)&(pVPEContext->nvDmaFifo)) != NO_ERROR)
		return FALSE;

    if (RmAllocContextDma(  pCmdObj->hClient,
                            NVP_DMA_FROM_MEMORY_PUSHBUF,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_READ_ONLY,
							0, // selector
                            (U032)(pVPEContext->nvDmaFifo),
                            pVPEContext->DmaPushBufTotalSize - 1) != 0)
    {
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate Context Dma for PushBuffer Object\n"));
		return FALSE;
    }

    // Allocate the DMA push buffer ERROR notifier
    if (RmAllocContextDma(  pCmdObj->hClient,
                            NVP_DMA_PUSHBUF_ERROR_NOTIFIER,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
							0, // selector
                            (U032)(&(pVPEContext->DmaPushBufErrNotifier[0])),
                            sizeof(NvNotification) - 1) != 0)
    {
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate PushBuffer notifier\n"));
		return FALSE;
    }

	// allocate DMA channel here !
    if (RmAllocChannelDma(  pCmdObj->hClient,
                            pCmdObj->dwDeviceId,
                            NVP_VPE_CHANNEL,
                            NV4_CHANNEL_DMA,
                            NVP_DMA_PUSHBUF_ERROR_NOTIFIER,
                            NVP_DMA_FROM_MEMORY_PUSHBUF,
                            0,
                            (PVOID)&(pVPEContext->pDmaChannel)) != 0)
    {
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate Dma Channel 0x%X\n", NVP_VPE_CHANNEL));
		return FALSE;
    }

	// alloc external decoder object
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_EXTERNAL_DECODER_OBJECT,
				NV03_EXTERNAL_VIDEO_DECODER) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate External Decoder Object 0x%X\n", NVP_EXTERNAL_DECODER_OBJECT));
		return FALSE;
	}

	// allocate transfer object (used for vbi and video capture)
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_M2M_OBJECT,
				NV03_MEMORY_TO_MEMORY_FORMAT) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate Mem to mem transfer Object 0x%X\n", NVP_M2M_OBJECT));
		return FALSE;
	}

    dwOvObj = (pCmdObj->dwChipId < NV_DEVICE_VERSION_10) ? NV04_VIDEO_OVERLAY : NV10_VIDEO_OVERLAY;

	// alloc overlay object 
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_OVERLAY_OBJECT,
				dwOvObj) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate Overlay Object 0x%X\n", NVP_OVERLAY_OBJECT));
		return FALSE;
	}


	// alloc DVD subpicture object 
	if (RmAllocObject(
				pCmdObj->hClient,
				NVP_VPE_CHANNEL,
	            NVP_DVD_SUBPICTURE_OBJECT,
				NV4_DVD_SUBPICTURE) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate DVD subpicture Object 0x%X\n", NVP_DVD_SUBPICTURE_OBJECT));
		return FALSE;
	}

    // allocate the transfer FROM context
    if (RmAllocContextDma(
						pCmdObj->hClient,
                        NVP_M2M_FROM_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
						0, // selector
                        (U032)pCmdObj->pFrameBuffer,
                        pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for M2M transfer Object\n"));
        return FALSE;
	}

    // allocate a notify context for transfer object
    if (RmAllocContextDma(
    					pCmdObj->hClient,
                        NVP_M2M_NOTIFIER,
                        NV01_CONTEXT_DMA_FROM_MEMORY,
                        0, 	//(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032)(pVPEContext->axfNotifiers),
                        (sizeof( NvNotification)*NVP_CLASS039_MAX_NOTIFIERS  - 1)) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for M2M transfer Object\n"));
        return FALSE;
    }
	
	// allocate a notify context for external decoder
	if (RmAllocContextDma(
					pCmdObj->hClient,
	         		NVP_EXTERNAL_DECODER_NOTIFIER,
	         		NV01_CONTEXT_DMA_FROM_MEMORY,
	         		0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
					0, // selector
	         		(U032)(pVPEContext->avpNotifiers),
	         		(sizeof(NvNotification)*NVP_CLASS04D_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for external decoder object\n"));
		return FALSE;
	}

	// context DMA for video field0
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VIDEO_FIELD0_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
				pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for video field0\n"));
		return FALSE;
	}

	// context DMA for video field1	
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VIDEO_FIELD1_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
	            pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for video field1\n"));
		return FALSE;
	}
	
	// context DMA for video field0
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VBI_FIELD0_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
				pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for VBI field0\n"));
		return FALSE;
	}

	// context DMA for video field1
	if (RmAllocContextDma(
				pCmdObj->hClient,
				NVP_VBI_FIELD1_BUFFER_CONTEXT,
				NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
				0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
				0, // selector
				(U032) (pCmdObj->pFrameBuffer),
	            pCmdObj->dwFBLen - 1) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate context DMA for VBI field1\n"));
		return FALSE;
	}

    // allocate a context for overlay object
    if (RmAllocContextDma(
    					pCmdObj->hClient,
                        NVP_OVERLAY_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA,
                        0, 	//(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032) (pCmdObj->pFrameBuffer),
                        pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate dma context for overlay buffer0\n"));
        return FALSE;
    }

    // allocate a context for DVD subpicture object
    if (RmAllocContextDma(
    					pCmdObj->hClient,
                        NVP_DVD_SUBPICTURE_CONTEXT,
                        NV01_CONTEXT_DMA,
                        0, 	//(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032) (pCmdObj->pFrameBuffer),
                        pCmdObj->dwFBLen - 1) != 0)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate dma context for DVD subpicture Object\n"));
        return FALSE;
    }

	// allocate a notify context for overlay notifiers
	if (RmAllocContextDma(
					pCmdObj->hClient,
	         		NVP_OVERLAY_NOTIFIER,
	         		NV01_CONTEXT_DMA_FROM_MEMORY,
	         		0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
					0, // selector
	         		(U032)(pVPEContext->aovNotifiers),
	         		(sizeof(NvNotification)*NVP_CLASS07A_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate notifiers for external decoder object\n"));
		return FALSE;
	}


    // reset media port notifiers
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    // reset overlay notifiers
    aovNotifiers = (NvNotification *) (pVPEContext->aovNotifiers);
    aovNotifiers[NV07A_NOTIFIERS_SET_OVERLAY(0)].status = 0;
    aovNotifiers[NV07A_NOTIFIERS_SET_OVERLAY(1)].status = 0;

	pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = (U032) (pVPEContext->aovNotifiers);

	// init Dma channel pointers
	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    pDmaChannel->Put = 0;
    nvDmaCount = 0;

	// set m2m object context
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT, NVP_M2M_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_NOTIFIES, NVP_M2M_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_BUFFER_IN, NVP_M2M_FROM_CONTEXT);

	// make sure decoder object is in the channel....
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_NOTIFIES, NVP_EXTERNAL_DECODER_NOTIFIER);
	// plug in our buffer contexts
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_IMAGE(0), NVP_VIDEO_FIELD0_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_IMAGE(1), NVP_VIDEO_FIELD1_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_VBI(0), NVP_VBI_FIELD0_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_VBI(1), NVP_VBI_FIELD1_BUFFER_CONTEXT);

    // set overlay contexts
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_NOTIFIES, NVP_OVERLAY_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(0), NVP_OVERLAY_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(1), NVP_OVERLAY_BUFFER_CONTEXT);

    // same for DVD subpicture context
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_OBJECT, NVP_DVD_SUBPICTURE_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_OVERLAY, NVP_DVD_SUBPICTURE_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_IMAGE_IN, NVP_DVD_SUBPICTURE_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_IMAGE_OUT, NVP_DVD_SUBPICTURE_CONTEXT);

	pVPEContext->nvDmaCount = nvDmaCount;
    pDmaChannel->Put = nvDmaCount << 2;

	// NB: we don't want to launch any captures at this point

	// setup the transfer callback
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_M2M_OBJECT;
  	evParam.hObjectNew = NVP_M2M_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV039_NOTIFIERS_BUFFER_NOTIFY;
  	evParam.hEvent.low = (ULONG) NVPTransferNotify;
	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for m2m transfer 0\n"));
		return FALSE;
	}

	// setup fields callbacks
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VIDEO_FIELD0_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
  	evParam.hEvent.low = (ULONG) VideoField0Notify;

	// video field0
	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for video field 0\n"));
		return FALSE;
	}

	// video field1
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VIDEO_FIELD1_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
  	evParam.hEvent.low = (ULONG) VideoField1Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for video field 1\n"));
		return FALSE;
	}

	// vbi field0
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VBI_FIELD0_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0);
  	evParam.hEvent.low = (ULONG) VBIField0Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for VBI field 0\n"));
		return FALSE;
	}
			
	// vbi field1
	evParam.hRoot = pCmdObj->hClient;
  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
  	evParam.hObjectNew = NVP_VBI_FIELD1_BUFFER_EVENT;
  	evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
  	evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1);
  	evParam.hEvent.low = (ULONG) VBIField1Notify;

	status = Nv01AllocEvent(&evParam);
	if (status)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot create callback for VBI field 1\n"));
		return FALSE;
	}

	// vsync event
	if (pCmdObj->hVPSyncEvent)
	{
	    // field 0
		evParam.hRoot = pCmdObj->hClient;
	  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
	  	evParam.hObjectNew = NVP_VSYNC_EVENT0;
	  	evParam.hClass = NV01_EVENT_WIN32_EVENT;
	  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
	  	evParam.hEvent.low = (ULONG) pCmdObj->hVPSyncEvent;

		status = Nv01AllocEvent(&evParam);
		if (status)
		{
	    	VideoDebugPrint((0,"VPEMINI: Cannot set event for video field 0\n"));
			return FALSE;
		}

        // field 1 (use same event for both fields)
		evParam.hRoot = pCmdObj->hClient;
	  	evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
	  	evParam.hObjectNew = NVP_VSYNC_EVENT1;
	  	evParam.hClass = NV01_EVENT_WIN32_EVENT;
	  	evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
	  	evParam.hEvent.low = (ULONG) pCmdObj->hVPSyncEvent;

		status = Nv01AllocEvent(&evParam);
		if (status)
		{
	    	VideoDebugPrint((0,"VPEMINI: Cannot set event for video field 0\n"));
			return FALSE;
		}

	}

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPUninitialize

	Description:	Free channels, DMA contexts and objects created by a 
					previous call to NVPInitialize

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPUninitialize(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;

	VideoDebugPrint((3,"VPEMINI: NVPUninitialize()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

	// free allocated resources
	////////// shouldn't we wait for completion (either grab of transfer ?!)
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD0_BUFFER_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD1_BUFFER_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD0_BUFFER_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD1_BUFFER_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD0_BUFFER_EVENT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD1_BUFFER_EVENT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD0_BUFFER_EVENT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD1_BUFFER_EVENT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_EXTERNAL_DECODER_NOTIFIER);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_EXTERNAL_DECODER_OBJECT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_OVERLAY_BUFFER_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DVD_SUBPICTURE_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_OVERLAY_NOTIFIER);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_OVERLAY_OBJECT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DVD_SUBPICTURE_OBJECT);

	// free transfer object resources
	if (pVPEContext->bxfContextCreated)
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_CONTEXT);

	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_NOTIFIER);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_FROM_CONTEXT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_EVENT);
	RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_OBJECT);

	if (pVPEContext->hVPSyncEvent)
	{
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VSYNC_EVENT0);
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VSYNC_EVENT1);
		pVPEContext->hVPSyncEvent = 0;
	}

	if (pVPEContext->nvDmaFifo != NULL)
	{
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VPE_CHANNEL);
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DMA_FROM_MEMORY_PUSHBUF);
		RmFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DMA_PUSHBUF_ERROR_NOTIFIER);
		VideoPortReleaseBuffer((PVOID)(pHwDevExt), (PVOID)(pVPEContext->nvDmaFifo));
	}

    pVPEContext->pOverlayChannel = 0;
    pVPEContext->nvDmaFifo = NULL;

    pVPEContext->nvpOverlay.dwOverlayBufferIndex = NVP_UNDEF;

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPUpdate

	Description:	Updates the NV video port data.

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPUpdate(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_OVERLAY pnvOverlay;
    int i;

	VideoDebugPrint((3,"VPEMINI: NVPUpdate()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

	pVPEContext->dwVPFlags = pCmdObj->dwVPInfoFlags;
	pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;

    if (pCmdObj->dwExtra422NumSurfaces > NVP_MAX_EXTRA422)
	{
		VideoDebugPrint((0,"VPEMINI: Error in 'NVPUpdate' >>> exceeded number of extra 422 surfaces \n"));
		return ERROR_INVALID_PARAMETER;
	}

	// get a pointer to the overlay object in VPE context
	pnvOverlay = &(pVPEContext->nvpOverlay);

	// update overlay info: extra 422 surfaces used for prescaling
	pnvOverlay->dwExtra422NumSurfaces = pCmdObj->dwExtra422NumSurfaces;
	pnvOverlay->dwExtra422Index = pCmdObj->dwExtra422Index;
	pnvOverlay->dwExtra422Pitch = pCmdObj->dwExtra422Pitch;            // extra surface pitch

    for (i = 0; i < (int)pCmdObj->dwExtra422NumSurfaces; i++)
	{
        pnvOverlay->aExtra422OverlayOffset[i] = pCmdObj->aExtra422OverlayOffset[i];	// extra memory space used for prescaling
    }

	// start video grabbing here if not already done in NVPStart
	if ((pVPEContext->nNumVidSurf > 0) && !(pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO))
	{
		// start VBI at the same time as video if vbi is not already enabled !
		if (pVPEContext->nNumVBISurf > 0)
		{
		    // is it already started, then no need to do it again...
		    if (!(pVPEContext->dwVPState & NVP_STATE_STARTED_VBI))
			{
				(pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
				pVPEContext->nNextVBISurfId = 1;
			}
			pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VIDEO | NVP_STATE_STARTED_VBI);
		}
		else
	    	pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VIDEO);
		 
		(pVPEContext->aVideoSurfaceInfo[pVPEContext->nNumVidSurf - 1]).nNextSurfaceIdx = 0;
		// kick off VP 
		pVPEContext->nNextVideoSurfId = 1;
        pnvOverlay->dwOverlayBufferIndex = NVP_UNDEF;
		// we OR 0x80 to tell function this is not a notifier calling !
		NVPProcessState(pHwDevExt, NVP_FIELD_EVEN | 0x80, FALSE);
		NVPProcessState(pHwDevExt, NVP_FIELD_ODD | 0x80, FALSE);
	}
	// start VBI grabbing here if not already done in NVPStart
    else if ((pVPEContext->nNumVBISurf > 0) && !(pVPEContext->dwVPState & NVP_STATE_STARTED_VBI))
	{
	    // if video already started, then only enable VBI state flag
        if (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO)
		{
		    pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
		}
		else
		{
			pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VBI);
			(pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
			pVPEContext->nNextVBISurfId = 1;
			// kick off VP 
			NVPProcessState(pHwDevExt, NVP_FIELD_EVEN, TRUE);
			NVPProcessState(pHwDevExt, NVP_FIELD_ODD, TRUE);
		}
	}

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPStart

	Description:	Tells the NV video port to start grabbing video

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPStart(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_OVERLAY pnvOverlay;
    int i;

	VideoDebugPrint((3,"VPEMINI: NVPStart()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

	// aleady started ?! it must be an UpdateVideo then...
	if (pVPEContext->dwVPState & NVP_STATE_STARTED)
		return NVPUpdate(pHwDevExt,	pCmdObj);

	// current
	pVPEContext->nVidSurfIdx = 0;
	pVPEContext->nVBISurfIdx = 0;
	// next in the list
	pVPEContext->nNextVideoSurfId = 0;
	pVPEContext->nNextVBISurfId = 0;

	// update overlay channel pointer ! 
	/////// NVPUpdate() must be the place for this !?
	pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;

	pVPEContext->dwVPState = NVP_STATE_STARTED;
	pVPEContext->dwVPFlags = pCmdObj->dwVPInfoFlags;
//	pVPEContext->aExtra422OverlayOffset[0] = pCmdObj->aExtra422OverlayOffset[0];
//	pVPEContext->aExtra422OverlayOffset[1] = pCmdObj->aExtra422OverlayOffset[1];

    if (pCmdObj->dwExtra422NumSurfaces > NVP_MAX_EXTRA422)
	{
		VideoDebugPrint((0,"VPEMINI: Error in 'NVPStart' >>> exceeded number of extra 422 surfaces \n"));
		return ERROR_INVALID_PARAMETER;
	}

	// get a pointer to the overlay object in VPE context
	pnvOverlay = &(pVPEContext->nvpOverlay);

	// update overlay info: extra 422 surfaces used for prescaling
	pnvOverlay->dwExtra422NumSurfaces = pCmdObj->dwExtra422NumSurfaces;
	pnvOverlay->dwExtra422Index = pCmdObj->dwExtra422Index;
	pnvOverlay->dwExtra422Pitch = pCmdObj->dwExtra422Pitch;            // extra surface pitch

    for (i = 0; i < (int)pCmdObj->dwExtra422NumSurfaces; i++)
	{
        pnvOverlay->aExtra422OverlayOffset[i] = pCmdObj->aExtra422OverlayOffset[i];	// extra memory space used for prescaling
    }

	// make a cyclic chain with surfaces 
	if (pVPEContext->nNumVidSurf > 0)
	{
		pVPEContext->dwVPState |= NVP_STATE_STARTED_VIDEO;
		(pVPEContext->aVideoSurfaceInfo[pVPEContext->nNumVidSurf - 1]).nNextSurfaceIdx = 0;
		// kick off VP 
		pVPEContext->nNextVideoSurfId = 1;
        pnvOverlay->dwOverlayBufferIndex = NVP_UNDEF;
		// before kicking off the media port, check out VBI setting
		if (pVPEContext->nNumVBISurf > 0)
		{
			pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
			(pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
			pVPEContext->nNextVBISurfId = 1;
		}
		// we OR 0x80 to tell function this is not a notifier calling !
		NVPProcessState(pHwDevExt, NVP_FIELD_EVEN | 0x80, FALSE);
		NVPProcessState(pHwDevExt, NVP_FIELD_ODD | 0x80, FALSE);
	}
	else if (pVPEContext->nNumVBISurf > 0)
	{
		pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
		(pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
		pVPEContext->nNextVBISurfId = 1;
		// kick off VP 
		NVPProcessState(pHwDevExt, NVP_FIELD_EVEN, TRUE);
		NVPProcessState(pHwDevExt, NVP_FIELD_ODD, TRUE);
	}

	return NO_ERROR;
}

/*
==============================================================================
	
	NVP3Stop

	Description:	Tells the NV video port to stop grabbing. PIO channel 
					version

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVP3Stop(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	Nv03ChannelPio *pNVPChannel = (Nv03ChannelPio *) (pCmdObj->pChannel);
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
	NVOS09_PARAMETERS ntfParam;
	U032 i = 0;

	VideoDebugPrint((3,"VPEMINI: NVP3Stop()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
	// aleady stopped ?!
	if (pVPEContext->dwVPState & NVP_STATE_STOPPED)
		return NO_ERROR;
	else
		pVPEContext->dwVPState = NVP_STATE_STOPPED;

	g_bFlip = FALSE;

	pVPEContext->pNVPChannel = (ULONG) pNVPChannel;	// get a copy of the pio channel pointer
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

	// tell VP to stop grabbing !
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.StopTransferImage = 0;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.StopTransferVbi = 0;


// NO LONGER EXISTS IN OFFICIAL NVOS.H....ASK!!!

//	ntfParam.hObjectParent = pCmdObj->dwDeviceId;


#define NVP_NOTIFY_TIMEOUT 100000
	// wait for the last few notifies to show up...
	while ((((avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
	         (avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
		   ((avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) || 
		   (avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
		   (i < NVP_NOTIFY_TIMEOUT)))
	{
		i++;
		//RmInterrupt(&ntfParam);
	}

	// reset surfaces counters
	pVPEContext->nNumVBISurf = 0;
	pVPEContext->nNumVidSurf = 0;

	return NO_ERROR;
}


/*
==============================================================================
	
	NVPStop

	Description:	Tells the NV video port to stop grabbing

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPStop(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	NvNotification *avpNotifiers;
    Nv03ChannelPio *pOvChannel;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;
    NvNotification *pPioFlipOverlayNotifier;
	U032 i;

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    // overlay channel
    pOvChannel = (Nv03ChannelPio *) (pVPEContext->pOverlayChannel);	// get a pointer to the overlay channel

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
		return NVP3Stop(pHwDevExt, pCmdObj);

	VideoDebugPrint((3,"VPEMINI: NVPStop()\n"));

	// aleady stopped ?!
	if (pVPEContext->dwVPState & NVP_STATE_STOPPED)
		return NO_ERROR;
	else
		pVPEContext->dwVPState = NVP_STATE_STOPPED;

    // channel
	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
	nvDmaCount = pVPEContext->nvDmaCount;

	// stop overlay engine
	if (pOvChannel && g_bFlip)
	{
	    g_bFlip = FALSE;

	    NVP_DMAPUSH_CHECK_FREE_COUNT(6);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(0), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(1), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
		pDmaChannel->Put = nvDmaCount << 2;
	    pVPEContext->nvDmaCount = nvDmaCount;

		// reset notifiers
        pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

        i = 0;
        while (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) || (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) && (i < NVP_VPP_TIME_OUT))
            i++;

        // timed-out... 
        if (i >= NVP_VPP_TIME_OUT)
	        VideoDebugPrint((0,"VPEMINI: waiting for overlay flip notifiers timed-out....\n"));

        pPioFlipOverlayNotifier[1].status = 0;
        pPioFlipOverlayNotifier[2].status = 0;
    }

	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

	// tell VP to stop grabbing !
	nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(6);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
	//if (pCmdObj->dwStopStrm & NVP_STOP_VIDEO)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_IMAGE , 0);
	//if (pCmdObj->dwStopStrm & NVP_STOP_VBI)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_VBI , 0);
    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;

#define NVP_NOTIFY_TIMEOUT 100000
    i = 0;
	// wait for the last few notifies to show up...
	while ((((avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
	         (avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
		   ((avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) || 
		   (avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
		   (i < NVP_NOTIFY_TIMEOUT)))
	{
		i++;
		//RmInterrupt(&ntfParam);
	}

    // timed-out... 
    if (i >= NVP_NOTIFY_TIMEOUT)
        VideoDebugPrint((0,"VPEMINI: waiting for MediaPort notifiers timed-out....\n"));

	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	// reset surfaces counters
	pVPEContext->nNumVBISurf = 0;
	pVPEContext->nNumVidSurf = 0;

    // clear/reset sync event 
	KeClearEvent((PVOID)pVPEContext->hVPSyncEvent);

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPRegSurface

	Description:	register a DDraw surface. Insert in list and update 
					attributes

	Date:			Feb 18, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPRegSurface(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_SURFACE_INFO pSurface;
	BOOL bVideoStarted, bVBIStarted;

	VideoDebugPrint((3,"VPEMINI: NVPRegSurface()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    bVideoStarted = (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO);
    bVBIStarted = (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI);

	// update attributes of already existing surfaces ?!
	if (pCmdObj->bSurfUpdate)
	{
		// vbi surfaces ?!
		if (pCmdObj->bSurfVBI)
		{
			if (pCmdObj->dwSurfaceId < (ULONG)pVPEContext->nNumVBISurf)
			{
				pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
				// check for mem offset in framebuffer !
				if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
				{
				    pSurface->dwHeight = pCmdObj->dwVBIHeight;
				    pSurface->dwStartLine = pCmdObj->dwVBIStartLine; 		// startline for vbi

				}
				else
				{
					VideoDebugPrint((0,"VPEMINI: can't update VBI surface data !\n"));
					return ERROR_INVALID_PARAMETER;
				}

			}
			else
			{
				// new vbi surfaces
				pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
				(pVPEContext->nNumVBISurf)++;
			    pSurface->pVidMem = pCmdObj->pSurfVidMem;				// Flat pointer to the surface
			    pSurface->dwOffset = pCmdObj->dwSurfOffset;         	// Offset from the base of the frame buffer
			    pSurface->dwPitch = pCmdObj->dwSurfPitch;
			    pSurface->dwHeight = pCmdObj->dwVBIHeight;
			    pSurface->dwStartLine = pCmdObj->dwVBIStartLine; 		// Programs Y-crop for image, and startline for vbi
			    pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;	// the next surface idx in the chain
				pSurface->bIsFirstFieldCaptured = FALSE;				// reset value

			}
		}
		else	// video surface
		{
			if (pCmdObj->dwSurfaceId < (ULONG)pVPEContext->nNumVidSurf)
			{
				//////////
				///// I assume something changing in the following values only !
				pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
				if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
				{
				    pSurface->dwHeight = pCmdObj->dwInHeight;
				    pSurface->dwWidth = pCmdObj->dwInWidth;            		// Ignored for VBI
				    pSurface->dwStartLine = pCmdObj->dwImageStartLine; 		// Programs Y-crop for image, and startline for vbi
				    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;    	// Ignored for VBI.. prescale size width
				    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;   	// Ignored for VBI.. prescale size height
				}
				else
				{
					VideoDebugPrint((0,"VPEMINI: can't update video surface data !\n"));
					return ERROR_INVALID_PARAMETER;
				}
			}
			else
			{
				// new video surfaces
				pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
				(pVPEContext->nNumVidSurf)++;
			    pSurface->pVidMem = pCmdObj->pSurfVidMem;				// Flat pointer to the surface
			    pSurface->dwOffset = pCmdObj->dwSurfOffset;         	// Offset from the base of the frame buffer
			    pSurface->dwPitch = pCmdObj->dwSurfPitch;
			    pSurface->dwHeight = pCmdObj->dwInHeight;
			    pSurface->dwWidth = pCmdObj->dwInWidth;            		// Ignored for VBI
			    pSurface->dwStartLine = pCmdObj->dwImageStartLine; 		// Programs Y-crop for image, and startline for vbi
			    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;    	// Ignored for VBI.. prescale size width
			    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;   	// Ignored for VBI.. prescale size height
			    pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;	// the next surface idx in the chain
				pSurface->bIsFirstFieldCaptured = FALSE;				// reset value

			}
		}
	}
	else	// new surfaces !
	{
		if (pVPEContext->dwVPState & NVP_STATE_STARTED)
		{
			VideoDebugPrint((0,"VPEMINI: can't register new DD surfaces while Video Port is running !\n"));
			return ERROR_INVALID_PARAMETER;
		}

		// vbi surfaces ?!
		if (pCmdObj->bSurfVBI)
		{
			pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
			(pVPEContext->nNumVBISurf)++;
		    pSurface->dwStartLine = pCmdObj->dwVBIStartLine; 		// startline for vbi
		}
		else	// video surface
		{
			pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
			(pVPEContext->nNumVidSurf)++;
		    pSurface->dwStartLine = pCmdObj->dwImageStartLine; 		// startline for video
		}

	    pSurface->pVidMem = pCmdObj->pSurfVidMem;				// Flat pointer to the surface
	    pSurface->dwOffset = pCmdObj->dwSurfOffset;         	// Offset from the base of the frame buffer
	    pSurface->dwPitch = pCmdObj->dwSurfPitch;
	    pSurface->dwHeight = pCmdObj->dwInHeight;
	    pSurface->dwWidth = pCmdObj->dwInWidth;            		// Ignored for VBI
	    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;    	// Ignored for VBI.. prescale size width
	    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;   	// Ignored for VBI.. prescale size height
	    pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;	// the next surface idx in the chain
		pSurface->bIsFirstFieldCaptured = FALSE;				// reset value
	}

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPUpdateOverlay

	Description:	update the overlay info structure

	Date:			Mar 01, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPUpdateOverlay(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
    Nv03ChannelPio *pChannel = (Nv03ChannelPio *) (pVPEContext->pOverlayChannel);	// get a pointer to the overlay channel;
	PNVP_OVERLAY pnvOverlay =  &(pVPEContext->nvpOverlay);
    NvNotification *pPioFlipOverlayNotifier;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;
	int i = 0;

	VideoDebugPrint((3,"VPEMINI: NVPUpdateOverlay()\n"));

	if ((pCmdObj->dwOverlayDstWidth != 0) && (pCmdObj->dwOverlayDstHeight != 0))
	{
		// I must rather have a structure memcpy !
		pnvOverlay->dwOverlayFormat = pCmdObj->dwOverlayFormat;
		pnvOverlay->dwOverlaySrcX = pCmdObj->dwOverlaySrcX;
		pnvOverlay->dwOverlaySrcY = pCmdObj->dwOverlaySrcY;
		pnvOverlay->dwOverlaySrcPitch = pCmdObj->dwOverlaySrcPitch;
		pnvOverlay->dwOverlaySrcSize = pCmdObj->dwOverlaySrcSize;
		pnvOverlay->dwOverlaySrcWidth = pCmdObj->dwOverlaySrcWidth;
		pnvOverlay->dwOverlaySrcHeight = pCmdObj->dwOverlaySrcHeight;
		pnvOverlay->dwOverlayDstWidth = pCmdObj->dwOverlayDstWidth;
		pnvOverlay->dwOverlayDstHeight = pCmdObj->dwOverlayDstHeight;
		pnvOverlay->dwOverlayDstX = pCmdObj->dwOverlayDstX;
	    pnvOverlay->dwOverlayDstY = pCmdObj->dwOverlayDstY;
		pnvOverlay->dwOverlayDeltaX = pCmdObj->dwOverlayDeltaX;
		pnvOverlay->dwOverlayDeltaY = pCmdObj->dwOverlayDeltaY;
		pnvOverlay->dwOverlayColorKey = pCmdObj->dwOverlayColorKey;
		pnvOverlay->dwOverlayMode = pCmdObj->dwOverlayMode;
	    pnvOverlay->dwOverlayMaxDownScale = pCmdObj->dwOverlayMaxDownScale;

		VideoDebugPrint((3,"VPEMINI: overlay flipping enabled....\n"));
		g_bFlip = TRUE;
	}
	else
	{
		VideoDebugPrint((3,"VPEMINI: overlay flipping disabled....\n"));
		g_bFlip = FALSE;	// no autoflip !
		// stop overlay engine
		if (pChannel)
		{
		    // channel
			pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
			nvDmaCount = pVPEContext->nvDmaCount;

		    NVP_DMAPUSH_CHECK_FREE_COUNT(6);
	        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
	        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(0), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
	        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(1), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
			pDmaChannel->Put = nvDmaCount << 2;
		    pVPEContext->nvDmaCount = nvDmaCount;

			// reset notifiers
            pPioFlipOverlayNotifier = (NvNotification *)pnvOverlay->pNvPioFlipOverlayNotifierFlat;

            while (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) || (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) && (i < NVP_VPP_TIME_OUT))
                i++;
            // timed-out... 
            if (i >= NVP_VPP_TIME_OUT)
		        VideoDebugPrint((0,"VPEMINI: waiting for overlay flip notifiers timed-out....\n"));

	        pPioFlipOverlayNotifier[1].status = 0;
            pPioFlipOverlayNotifier[2].status = 0;
		}
	}

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPFlipVideoPort

	Description:	start capturing in specified surface 

	Date:			Mar 01, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPFlipVideoPort(
    PHW_DEVICE_EXTENSION pHwDevExt,
	NVP_CMD_OBJECT *pCmdObj)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_SURFACE_INFO pSurface;
	NvNotification *avpNotifiers;
	int i;

	VideoDebugPrint((3,"VPEMINI: NVPFlipVideoPort()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

	// NVP_UNDEF tells us to lookup the surface in list
	if (pCmdObj->dwSurfaceId == NVP_UNDEF)
	{
		for (i = 0; i < pVPEContext->nNumVidSurf; i++)
		{
				//////////
				///// I assume nothing changing in the surfaces values 
				pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
				if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
				{
				    // clear/reset sync event 
					KeClearEvent((PVOID)pVPEContext->hVPSyncEvent);
					// found target surface, so update index of video surface we're currently capturing into
					pVPEContext->nVidSurfIdx = i;
					return NO_ERROR;
				}
		}
	}
	else
	{
		// check for weird values !
		if ((pCmdObj->dwSurfaceId >= 0) && 
			(pCmdObj->dwSurfaceId < pVPEContext->nNumVidSurf) && 
			(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId].pVidMem == pCmdObj->pSurfVidMem))
		{
			    // clear/reset sync event 
				KeClearEvent((PVOID)pVPEContext->hVPSyncEvent);
				// update index of video surface we're currently capturing into
				pVPEContext->nVidSurfIdx = (int)(pCmdObj->dwSurfaceId);
				return NO_ERROR;
		}
	}

	////// if we got here, it means the target surface wasn't found in list... 
	// so add it at the end of the list  
	i = pVPEContext->nNumVidSurf;
	pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
	(pVPEContext->nNumVidSurf)++;
	pSurface->dwStartLine = pCmdObj->dwImageStartLine; 		// startline for video
    pSurface->pVidMem = pCmdObj->pSurfVidMem;				// Flat pointer to the surface
    pSurface->dwOffset = pCmdObj->dwSurfOffset;         	// Offset from the base of the frame buffer
    pSurface->dwPitch = pCmdObj->dwSurfPitch;
    pSurface->dwHeight = pCmdObj->dwInHeight;
    pSurface->dwWidth = pCmdObj->dwInWidth;            		// Ignored for VBI
    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;    	// Ignored for VBI.. prescale size width
    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;   	// Ignored for VBI.. prescale size height
    pSurface->nNextSurfaceIdx = 0;							// the next surface idx in the chain
	pSurface->bIsFirstFieldCaptured = FALSE;				// reset value
    // clear/reset sync event 
	KeClearEvent((PVOID)pVPEContext->hVPSyncEvent);
	// flip to surface !
	pVPEContext->nVidSurfIdx = i;							

	return NO_ERROR;
}

/*
==============================================================================
	
	NVP3ProgVideoField

	Description:	programs the vp to capture the specified video field

	Date:			Feb 19, 1999
	
	Author:			H. Azar

==============================================================================
*/
void NVP3ProgVideoField(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface,
	int nFieldId,
	U032 dwSurfPitch,
	U032 dwInterleaveOffset,
	BOOL bGrab)
{
	Nv03ChannelPio *pNVPChannel;
	NvNotification *avpNotifiers;
	int nFreeCount;
	NVOS09_PARAMETERS ntfParam;

	pSurface->dwFieldType = (ULONG)nFieldId;

	// get a pointer to the appropriate VPE context
	pNVPChannel = (Nv03ChannelPio *)(pVPEContext->pNVPChannel) ;	// get a copy of the pio channel pointer

#ifdef _NV_DPC
	//CHECK_FREE_COUNT(pNVPChannel,6*4);
	ntfParam.hObjectParent = pVPEContext->dwDeviceId;
	nFreeCount = pVPEContext->nFreeCount;
	while (nFreeCount < 7*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pNVPChannel, 0);
	}			
	pVPEContext->nFreeCount = nFreeCount - (7*4);
#endif

	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
	// program the VP to grab in the next surface
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].sizeIn  = (pSurface->dwWidth) | (pSurface->dwHeight << 16);
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].sizeOut = (pSurface->dwPreScaleSizeX) | (pSurface->dwPreScaleSizeY << 16);

	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImageStartLine = pSurface->dwStartLine;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].offset  = pSurface->dwOffset + dwInterleaveOffset; 

	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].format =
			(dwSurfPitch & 0xFFFF)  |
			((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
			((NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000);
}

/*
==============================================================================
	
	NVP3ProgVBIField

	Description:	programs the vp to capture the specified VBI field.
					PIO channel version.

	Date:			Feb 19, 1999
	
	Author:			H. Azar

==============================================================================
*/
void NVP3ProgVBIField(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface,
	int nFieldId)
{
	Nv03ChannelPio *pNVPChannel;
	int nFreeCount;
	NVOS09_PARAMETERS ntfParam;
	NvNotification *avpNotifiers;

	pSurface->dwFieldType = (ULONG)nFieldId;

	// get a pointer to the appropriate VPE context
	pNVPChannel = (Nv03ChannelPio *)(pVPEContext->pNVPChannel) ;	// get a copy of the pio channel pointer

#ifdef _NV_DPC
	//CHECK_FREE_COUNT(pNVPChannel,5*4);
	ntfParam.hObjectParent = pVPEContext->dwDeviceId;
	nFreeCount = pVPEContext->nFreeCount;
	while (nFreeCount < 5*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pNVPChannel, 0);
	}
	pVPEContext->nFreeCount = nFreeCount - (5*4);
#endif
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].size  = (pSurface->dwStartLine) | (pSurface->dwHeight << 16);
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].offset  = pSurface->dwOffset;
	pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].format =
			(pSurface->dwPitch & 0xFFFF)  |
			((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_VBI_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
			((NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24) & 0xFF000000);
}

/*
==============================================================================
	
	NVPProgVideoField

	Description:	programs the vp to capture the specified video field

	Date:			Apr 19, 1999
	
	Author:			H. Azar

==============================================================================
*/
void NVPProgVideoField(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface,
	int nFieldId,
	U032 dwSurfPitch,
	U032 dwInterleaveOffset,
	BOOL bGrab)
{
	Nv4ControlDma *pDmaChannel;
	NvNotification *avpNotifiers;
	U032 dwFormat;
	U032 nvDmaCount;

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
	{
		NVP3ProgVideoField(pVPEContext, pSurface, nFieldId, dwSurfPitch, dwInterleaveOffset, bGrab);
		return;
	}

	// update type of field we're capturing
	pSurface->dwFieldType = (ULONG)nFieldId;

	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(12);

	// reset notifier
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
	avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	// program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_SIZE_IN(nFieldId), ((pSurface->dwWidth) | (pSurface->dwHeight << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_SIZE_OUT(nFieldId), ((pSurface->dwPreScaleSizeX) | (pSurface->dwPreScaleSizeY << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_START_LINE, pSurface->dwStartLine);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_OFFSET(nFieldId), (pSurface->dwOffset + dwInterleaveOffset));
	dwFormat = ((dwSurfPitch & 0xFFFF) |
				((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
				((NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000));

	NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_FORMAT(nFieldId), dwFormat);

    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;
}

/*
==============================================================================
	
	NVPProgVBIField

	Description:	programs the vp to capture the specified VBI field

	Date:			Apr 22, 1999
	
	Author:			H. Azar

==============================================================================
*/
void NVPProgVBIField(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface,
	int nFieldId)
{
	Nv4ControlDma *pDmaChannel;
	NvNotification *avpNotifiers;
	U032 dwFormat;
	U032 nvDmaCount;

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
	{
		NVP3ProgVBIField(pVPEContext, pSurface, nFieldId);
		return;
	}

	// update type of field we're capturing
	pSurface->dwFieldType = (ULONG)nFieldId;

	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(8);

	// reset notifier
	avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
	avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

	// program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_SIZE(nFieldId), ((pSurface->dwStartLine) | (pSurface->dwHeight << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_OFFSET(nFieldId), pSurface->dwOffset);
	dwFormat = (pSurface->dwPitch & 0xFFFF)  |
			((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_VBI_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
			((NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24) & 0xFF000000);
	NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_FORMAT(nFieldId), dwFormat);

    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;
}


/*
==============================================================================
	
	NVPProcessState

	Description:	programs the video port grab in a surface and determines
					which surface is next.

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPProcessState(
	PHW_DEVICE_EXTENSION pHwDevExt,
	int nIndex,
	BOOL bVBIField)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_SURFACE_INFO pSurface;
	int nFieldId = nIndex & 1;
	U032 status;
	BOOL bIRQCallback;

	VideoDebugPrint((3,"VPEMINI: NVPProcessState()\n"));

	// get a pointer to the appropriate VPE context
	pVPEContext = &(pHwDevExt->avpContexts[0]);

    // setting nIndex to greater than 1 (nField | any_value) means that the caller is NVPStart or NVPUpdate
	// so first time initialize the dwOverlayIndex to match up with the nFieldId (notifier)
    if ((pVPEContext->nvpOverlay.dwOverlayBufferIndex == NVP_UNDEF) && (nIndex <= 1))
       pVPEContext->nvpOverlay.dwOverlayBufferIndex = nFieldId;

	// calling IRQCallback must be done only at the end of video field or at end of vbi when
	// capturing vbi only 
	bIRQCallback = (!bVBIField) || (pVPEContext->nNumVidSurf == 0);
	// any transfer request of already captured vbi/video data ?!
	if (bIRQCallback)
		NVPCheckForLaunch(pVPEContext, NULL);


	if (pVPEContext->dwVPState != NVP_STATE_STOPPED)
    {
        if (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO)
	    {
		    // program VBI here. This way VBI is synchronized with the video thus we eliminate the need 
		    // for a seperate VBI notifications routine unless we're streaming VBI only
		    if (!bVBIField)
			{
			    if (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI)
				{
					pSurface = &(pVPEContext->aVBISurfaceInfo[pVPEContext->nNextVBISurfId]);
					// program the VP to grab in the next surface
					NVPProgVBIField(pVPEContext, pSurface, nFieldId);

					// next surface in the chain !
					pVPEContext->nVBISurfIdx = pVPEContext->nNextVBISurfId;
					pVPEContext->nNextVBISurfId = pSurface->nNextSurfaceIdx;
				}

				if (pVPEContext->dwVPConFlags & DDVPCONNECT_INTERLACED)
				{
					if (pVPEContext->dwVPFlags & DDVP_INTERLEAVE)
					{
						if (pVPEContext->nvpOverlay.dwOverlayMode & NV_VFM_FORMAT_BOBFROMINTERLEAVE)
						{
							// Bob from interleaved surfaces (skip even/odd included here too)
							DoBobFromInterleaved(pVPEContext, nFieldId);
						}
						else
						{
							// Weave
							DoWeave(pVPEContext, nFieldId);
						}
					}
					else
					{
						// Bob from non-interleaved (separate) surfaces (skip even/odd included here too)
						DoBob(pVPEContext, nFieldId);
					}
				}
				else
				{
					// Progressive 
					DoProgressive(pVPEContext, nFieldId);
				}
			}
		}
        else if (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI)
		{
			pSurface = &(pVPEContext->aVBISurfaceInfo[pVPEContext->nNextVBISurfId]);
			// program the VP to grab in the next surface
			NVPProgVBIField(pVPEContext, pSurface, nFieldId);

			// next surface in the chain !
			pVPEContext->nVBISurfIdx = pVPEContext->nNextVBISurfId;
			pVPEContext->nNextVBISurfId = pSurface->nNextSurfaceIdx;
		}
    }

	//
	// calling IRQCallback must be done only at the end of video field or at end of vbi when
	// capturing vbi only 
	if (bIRQCallback)
	{
		// can we pass on the interrupt !
		if ((pHwDevExt->IRQCallback) && (pHwDevExt->dwIRQSources & DDIRQ_VPORT0_VSYNC))
		{
			PDX_IRQDATA pIRQData;
			pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
			pIRQData->dwIrqFlags |= DDIRQ_VPORT0_VSYNC;
			((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
		}
	}

	return NO_ERROR;
}


/*
==============================================================================
	
	NVPEProcessCommand

	Description:	dispatches commands sent via IOCTL to the appropriate 
					NVP function

	Date:			Feb 03, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPEProcessCommand(
    PVOID pHwDeviceExtension,
    PVIDEO_REQUEST_PACKET pRequestPacket)
{
	U032 status = 0;
	NVP_CMD_OBJECT *pnvCmdObj;
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

	VideoDebugPrint((3,"VPEMINI: NVPEProcessCommand()\n"));

	pnvCmdObj = (NVP_CMD_OBJECT *)(pRequestPacket->InputBuffer);
	// some arguments validation
	if ((pnvCmdObj->dwPortId < 0) || (pnvCmdObj->dwPortId >= NVP_MAX_VPORTS)) 
		return ERROR_INVALID_PARAMETER;

	// dispatch call
	switch (pnvCmdObj->dwSrvId)
	{
		case NVP_SRV_INITIALIZE:
			status = NVPInitialize(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_UNINITIALIZE:
			status = NVPUninitialize(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_START_VIDEO:
			status = NVPStart(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_STOP_VIDEO:
			status = NVPStop(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_UPDATE_VIDEO:
			status = NVPUpdate(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_REG_SURFACE:
			status = NVPRegSurface(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_UPDATE_OVERLAY:
			status = NVPUpdateOverlay(pHwDevExt, pnvCmdObj);
			break;
		case NVP_SRV_FLIP_VIDEOPORT:
			status = NVPFlipVideoPort(pHwDevExt, pnvCmdObj);
			break;
		default:
			VideoDebugPrint((0,"VPEMINI: invalid VPE command !\n"));
			return ERROR_INVALID_PARAMETER;
	}

	return status;
}

/*
==============================================================================
	
	VideoField0Notify

	Description:	callback function. Called after a video buffer gets filled

	Date:			Feb 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
void VideoField0Notify(PVOID pHwDeviceExtension)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

	NVPProcessState(pHwDevExt, 0, FALSE);
}

/*
==============================================================================
	
	VideoField1Notify

	Description:	callback function. Called after a vbi buffer gets filled

	Date:			Feb 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
void VideoField1Notify(PVOID pHwDeviceExtension)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

	NVPProcessState(pHwDevExt, 1, FALSE);
}


/*
==============================================================================
	
	VBIField0Notify

	Description:	callback function. Called after a vbi buffer gets filled

	Date:			Feb 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
void VBIField0Notify(PVOID pHwDeviceExtension)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

	NVPProcessState(pHwDevExt, 0, TRUE);
}

/*
==============================================================================
	
	VBIField1Notify

	Description:	callback function. Called after a vbi buffer gets filled

	Date:			Feb 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
void VBIField1Notify(PVOID pHwDeviceExtension)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

	NVPProcessState(pHwDevExt, 1, TRUE);
}


/*
==============================================================================
	
	NVPScheduleOverlayFlip

	Description:	program overlay object to flip surface

	Date:			Feb 25, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS NVPScheduleOverlayFlip(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface,
	int  nImageId,
	U032 dwSrcPitch,
	U032 dwMoveDownOffset)
{
	NVP_OVERLAY ovOverlayInfo;
	Nv03ChannelPio *pOvrChannel;
	U032 dwFlags = 0;
	U032 dwSubChn;
	U032 dwSrcDeltaY;
	U032 dwSrcOffset;

	if (!g_bFlip)
		return NO_ERROR;

	VideoDebugPrint((3,"VPEMINI: NVPScheduleOverlayFlip()\n"));


	dwFlags = VPP_PRESCALE;

	if (pVPEContext->dwVPFlags & DDVP_INTERLEAVE)
	   dwFlags |= VPP_INTERLEAVED;
	else 
	   dwFlags |= VPP_BOB;

	if (pVPEContext->nvpOverlay.dwOverlayMode & NV_VFM_FORMAT_BOBFROMINTERLEAVE)
	   dwFlags |= VPP_BOB;

    if (nImageId == NVP_FIELD_ODD)
	   dwFlags |= VPP_ODD;
	else if (nImageId == NVP_FIELD_EVEN)
	   dwFlags |= VPP_EVEN;
    
    NVPPrescaleAndFlip(
	    pVPEContext, 
	    &(pVPEContext->nvpOverlay), 
	    pSurface->dwOffset, 
	    pSurface->dwPitch, // source pitch
        nImageId,
        dwFlags);          // VPP flags

	return NO_ERROR;
}


/*
==============================================================================
	
	DoProgressive

	Description:	displays progressive video

	Date:			Mar 08, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS DoProgressive(
	PNVP_CONTEXT pVPEContext,
	int nFieldId)
{
	PNVP_SURFACE_INFO pSurface;
	int nTargetSurfId = pVPEContext->nVidSurfIdx;
	U032 dwSrcPitch;
	int nImage = nFieldId;

	pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
		// if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
	if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
		nTargetSurfId = pSurface->nNextSurfaceIdx;

	dwSrcPitch = pSurface->dwPitch;

	// Overlay Image n from CS
	NVPScheduleOverlayFlip(pVPEContext,	pSurface, nImage, dwSrcPitch, 0);

	pVPEContext->nVidSurfIdx = nTargetSurfId;

	// Grab Image n in TS
	pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
	NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);

	return NO_ERROR;
}

/*
==============================================================================
	
	DoWeave

	Description:	use weave method to display video

	Date:			Mar 08, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS DoWeave(
	PNVP_CONTEXT pVPEContext,
	int nFieldId)
{
	PNVP_SURFACE_INFO pSurface;
	int nTargetSurfId = pVPEContext->nVidSurfIdx;
	U032 dwSrcPitch, dwOffset = 0;
	int nImage = nFieldId;

	pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
	// if autoflip, TS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
	if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
		nTargetSurfId = pSurface->nNextSurfaceIdx;
	// double pitch (interleaved data)
	dwSrcPitch = 2*(pSurface->dwPitch);

	if (nFieldId == NVP_FIELD_ODD)
	{
		// Invert Polarity ?!
		if (pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY)
		{
			// change vp grab offset one line down
			dwOffset = pSurface->dwPitch;
			// Shift Down Image destination by 1 line !
			/////// the following code uses an "undocumented feature" in the RM to achieve the 1 line shift down effect !
			//dwSrcPitch = (dwSrcPitch & 0xFFFFFFFC) | 0x2;
		}
	}
	else
	{
		// not Invert Polarity ?!
		if (!(pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY))
		{
			// offset vp grab one line down
			dwOffset = pSurface->dwPitch;

			// Shift Down Image destination by 1 line !
			/////// the following code uses an "undocumented feature" in the RM to achieve the 1 line shift down effect !
			//dwSrcPitch = (dwSrcPitch & 0xFFFFFFFC) | 0x2;
		}
	}

	// whole frame captured ?!
	if (pSurface->bIsFirstFieldCaptured)
	{
		// Overlay Image n from CS
		NVPScheduleOverlayFlip(pVPEContext,	pSurface, NVP_UNDEF, pSurface->dwPitch, 0);
		// NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage ^ 1, dwSrcPitch, dwOffset);

		pSurface->bIsFirstFieldCaptured = FALSE;	// reset flag (for the next time)

		// if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
		if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
			pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;
	}
	else
		pSurface->bIsFirstFieldCaptured = TRUE;	// set flag (for the next time)

	// Grab Image n in TS
	pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
	NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, dwOffset, TRUE);

	return NO_ERROR;
}

/*
==============================================================================
	
	DoBobFromInterleaved

	Description:	use bob from interleaved input to display video

	Date:			Mar 08, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS DoBobFromInterleaved(
	PNVP_CONTEXT pVPEContext,
	int nFieldId)
{
	PNVP_SURFACE_INFO pSurface;
	int nTargetSurfId = pVPEContext->nVidSurfIdx;
	U032 dwSrcPitch, dwOffset = 0;
	int nImage = nFieldId;

	// Current Surface (CS)
	pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
	// NewPitch = OldPitch
	dwSrcPitch = pSurface->dwPitch;	// interleaved
	// if autoflip, TS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
	if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
		nTargetSurfId = pSurface->nNextSurfaceIdx;

#if 0
	if (pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS)
	{
		if (nFieldId == NVP_FIELD_EVEN)
		{
			// VP minimum programming to issue an interrupt at end of field !
			NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, FALSE);
			return NO_ERROR;
		}
	}
	else if (pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS)
	{
		if (nFieldId == NVP_FIELD_ODD)
		{
			// VP minimum programming to issue an interrupt at end of field !
			NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, FALSE);
			return NO_ERROR;
		}
	}
	else
#endif
	{
		// double pitch (interleaved data)
		dwSrcPitch = 2*dwSrcPitch;
		if (nFieldId == NVP_FIELD_ODD)
		{
			// Invert Polarity ?!
			if (pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY)
			{
				// change vp grab offset one line down
				dwOffset = pSurface->dwPitch;
			}
		}
		else
		{
			// Invert Polarity ?!
			// for the even field (bottom) shift down overlay destination if invert polarity flag is not set !
			if (!(pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY))
			{
				dwOffset = pSurface->dwPitch;

			}
		}

	}

	if (!((pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS) && (nFieldId == NVP_FIELD_EVEN)) &&
		!((pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS) && (nFieldId == NVP_FIELD_ODD)))
	{
		// Overlay Image n from CS
		NVPScheduleOverlayFlip(pVPEContext,	pSurface, nImage, dwSrcPitch, dwOffset);
		// NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage ^ 1, dwSrcPitch, dwOffset);
	}

	// whole frame captured ?!
	if (pSurface->bIsFirstFieldCaptured)
	{
		pSurface->bIsFirstFieldCaptured = FALSE;	// reset (for the next time)
		// if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
		if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
			pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;
	}
	else
		pSurface->bIsFirstFieldCaptured = TRUE;	// for the next time

	// Grab Image n in TS
	pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
	NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, dwOffset, TRUE);

	return NO_ERROR;
}

/*
==============================================================================
	
	DoBob

	Description:	use bob from non-interleaved input to display video

	Date:			Mar 08, 1999
	
	Author:			H. Azar

==============================================================================
*/
VP_STATUS DoBob(
	PNVP_CONTEXT pVPEContext,
	int nFieldId)
{
	PNVP_SURFACE_INFO pSurface;
	int nTargetSurfId = pVPEContext->nVidSurfIdx;
	U032 dwSrcPitch;
	int nImage = nFieldId;

	pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
	dwSrcPitch = pSurface->dwPitch;
#if 0
	if (pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS)
	{
		if (nFieldId == NVP_FIELD_EVEN)
		{
			// VP minimum programming to issue an interrupt at end of field !
			// TS = next2(CS)
			nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
			pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfaceId]);
			NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);
			return NO_ERROR;
		}
		else
			// TS = next(CS)
			nTargetSurfId = pSurface->nNextSurfaceIdx;
	}
	else if (pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS)
	{
		if (nFieldId == NVP_FIELD_ODD)
		{
			// VP minimum programming to issue an interrupt at end of field !
			nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
			pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfaceId]);
			NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);
			return NO_ERROR;
		}
		else
			// TS = next(CS)
			nTargetSurfId = pSurface->nNextSurfaceIdx;
	}
	else
#endif
	{
		// if autoflip, TS = next2(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
		if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
			nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
	}

	if (!((pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS) && (nFieldId == NVP_FIELD_EVEN)) &&
		!((pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS) && (nFieldId == NVP_FIELD_ODD)))
	{
		// Overlay Image n from CS
		NVPScheduleOverlayFlip(pVPEContext,	pSurface, nImage, dwSrcPitch, 0);
	}

	// if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
	if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
		pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;

	// Grab Image n in TS
	pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
	NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);

	return NO_ERROR;
}

/*
==============================================================================
	
	NVPTransfer

	Description:	transfers content of video or vbi field to specified
					buffer (usually system memory buffer)

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPTransfer(
	PVOID pContext, 
	PDDTRANSFERININFO pInInfo, 
	PDDTRANSFEROUTINFO pOutInfo)
{
	PNVP_CONTEXT pVPEContext;
	PNVP_TRANSFER_BUFFER pTransfer;
	PNVP_SURFACE_INFO pSurface = NULL;
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
	BOOL bVal;
	int i;

	VideoDebugPrint((3,"VPEMINI: NVPTransfer()\n"));

	// get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
	pVPEContext = &(pHwDevExt->avpContexts[0]);

	if (pInInfo->dwTransferFlags & DDTRANSFER_NONLOCALVIDMEM)
		return DXERR_UNSUPPORTED;
	
	// we DO handle cancels for transfer buffers not "in progress" !
	if (pInInfo->dwTransferFlags & DDTRANSFER_CANCEL)
		return NVPCancelTransfer(pVPEContext, pInInfo);

	// determine polarity, TRUE for Even, FALSE for Odd !
	NVPGetSurface(pVPEContext, ((DDSURFACEDATA*) pInInfo->lpSurfaceData)->dwSurfaceOffset, &pSurface);
	if ((pSurface != NULL) && (pSurface->dwFieldType == NVP_FIELD_ODD))
		pOutInfo->dwBufferPolarity = FALSE;
	else
		pOutInfo->dwBufferPolarity = TRUE;
	
	// queue request
	if (NVPAddTransfer(pVPEContext, pInInfo, !(pOutInfo->dwBufferPolarity)))
		return DXERR_GENERIC;

	if (pVPEContext->nCurTransferIdx != -1)
	{
		pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
		// Recreate context dma for MDL
		NVPRecreateContext(pVPEContext, (PMDL)(pTransfer->pDestMDL));
	}

	return DX_OK;
}

/*
==============================================================================
	
	NVPGetSurface

	Description:	looks for surface that has the specified offset. pSurface
					is set to NULL if no surface is found.


	Date:			Mar 18, 1999
	
	Author:			H. Azar

==============================================================================
*/
BOOL NVPGetSurface(
	PNVP_CONTEXT pVPEContext,
	U032 dwSurfOffset,
	PNVP_SURFACE_INFO *pSurface)
{
	int i;

	VideoDebugPrint((3,"VPEMINI: NVPGetSurface()\n"));

	*pSurface = NULL;

	// why VBI first ! well, this is most probably what we're transfering... kinda optimization !
	for (i = 0; i < pVPEContext->nNumVBISurf; i++)
	{
		if (pVPEContext->aVBISurfaceInfo[i].dwOffset == dwSurfOffset)
		{
			*pSurface = &(pVPEContext->aVBISurfaceInfo[i]);
			return FALSE;	// FALSE means VBI
		}
	}

	for (i = 0; i < pVPEContext->nNumVidSurf; i++)
	{
		if (pVPEContext->aVideoSurfaceInfo[i].dwOffset == dwSurfOffset)
		{
			*pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
			return TRUE;	// TRUE means VIDEO
		}
	}

	return FALSE;	// return something ! (pSurface is NULL anyway)
}

/*
==============================================================================
	
	NVP3RecreateContext

	Description:	if necessary, create context DMA for mem to mem object
					PIO channel version.

	Date:			Mar 18, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVP3RecreateContext(
	PNVP_CONTEXT pVPEContext,
	PMDL pDestMDL) 
{
	Nv03ChannelPio *pChan;
	NVOS09_PARAMETERS ntfParam;
	U032 dwStart;
	U032 dwLimit;
	int nFreeCount;

	pChan = (Nv03ChannelPio *)(pVPEContext->pNVPChannel);

	// first check to see if we can reuse the OLD context... since context creation is expensive.
	dwStart = (U032)(pDestMDL->lpMappedSystemVa);
	dwLimit = (pDestMDL->ByteCount) - 1;
	
	if (pVPEContext->bxfContextCreated)
	{
		// do not recreate if same context
		if ((dwStart == pVPEContext->dwTransferCtxStart) && (dwLimit == pVPEContext->dwTransferCtxSize))
			return DX_OK;
	}

#ifdef _NV_DPC
	// FIFO free count check up
	ntfParam.hObjectParent = pVPEContext->dwDeviceId;
	nFreeCount = pVPEContext->nFreeCount;
	while (nFreeCount < 4*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pChan, 5);
	}
	pVPEContext->nFreeCount = nFreeCount - (4*4);
#endif

	// free the old context first
	if (pVPEContext->bxfContextCreated)
	{
		pChan->subchannel[5].SetObject = NVP_M2M_OBJECT;
		pChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = 0;
#ifdef _NV_DPC
		// Do a software method flush here
		ntfParam.hObjectParent = pVPEContext->dwDeviceId;
		nFreeCount = pVPEContext->nFreeCount;
		while (nFreeCount < NV06A_FIFO_GUARANTEED_SIZE) 
		{
			//RmInterrupt(&ntfParam);
			nFreeCount = NvGetFreeCount(pChan, 5);
		}
		pVPEContext->nFreeCount = nFreeCount - NV06A_FIFO_GUARANTEED_SIZE;
#endif
		RmFree(pVPEContext->hClient, pVPEContext->dwDeviceId, NVP_M2M_CONTEXT);

		pVPEContext->bxfContextCreated = FALSE;
	}
	
	// setup Notify Context
	if (RmAllocContextDma(
					pVPEContext->hClient,
	         		NVP_M2M_CONTEXT,
	         		NV01_CONTEXT_DMA_FROM_MEMORY,
	         		0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
					0, // selector
	         		dwStart,
	         		dwLimit) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate mem 2 mem context\n"));
		return DXERR_GENERIC;
	}

	// set context of M2M object
	pChan->subchannel[5].SetObject  = NVP_M2M_OBJECT;
	pChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = NVP_M2M_CONTEXT;

	pVPEContext->dwTransferCtxStart = dwStart;
	pVPEContext->dwTransferCtxSize = dwLimit;
	pVPEContext->bxfContextCreated = TRUE;

	return DX_OK;
}

/*
==============================================================================
	
	NVP3CheckForLaunch

	Description:	try to launch another mem transfer if engine not busy
					(PIO version)

	Date:			Mar 18, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVP3CheckForLaunch(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface)
{
	Nv03ChannelPio *pChan;
	PNVP_TRANSFER_BUFFER pTransfer;
	LPDDSURFACEDATA pSurfaceData;
	NVOS09_PARAMETERS ntfParam;
	NvNotification *aM2MNotifiers;
	PMDL pDestMDL;
	U032 dwOffsetStartByte;
	LONG lLength, lPitch, lCount;
	int nFreeCount;
	PNVP_SURFACE_INFO pSurf = NULL;

	VideoDebugPrint((3,"VPEMINI: NVP3CheckForLaunch()\n"));

	aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);

	// is transfer in progress ?
	if ((aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS) ||
		// or no transfer request !?
		(pVPEContext->nCurTransferIdx == -1))
		return DXERR_GENERIC;

	// get a pointer to the VP pio channel
	pChan = (Nv03ChannelPio *)(pVPEContext->pNVPChannel);
	// which surface are we transferring from ?
	pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
	pSurfaceData = (LPDDSURFACEDATA) (pTransfer->pSurfaceData);
	if (pSurface == NULL)
	{
		NVPGetSurface(pVPEContext, pSurfaceData->dwSurfaceOffset, &pSurf);
		if (pSurf == NULL)
		{
			VideoDebugPrint((0,"VPEMINI: NVPCheckForLaunch() can't get pointer to surface\n"));
			return DXERR_GENERIC;
		}
	}
	else
		pSurf = pSurface;

	// Recreate context dma for MDL
	NVPRecreateContext(pVPEContext, (PMDL)(pTransfer->pDestMDL));
	
	// reset notifier status
	aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

#ifdef _NV_DPC
	//CHECKFREECOUNT(pMyNvChan,9*4);
	ntfParam.hObjectParent = pVPEContext->dwDeviceId;
	nFreeCount = pVPEContext->nFreeCount;
	while (nFreeCount < 9*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pChan, 5);
	}
	pVPEContext->nFreeCount = nFreeCount - (9*4);
#endif

	// start at given line
	dwOffsetStartByte = pSurfaceData->dwSurfaceOffset + (pSurfaceData->lPitch * pTransfer->lStartLine);

	// lLength and lPitch might not be the same (case of interleaved fields !!!)
	lLength = (pSurfaceData->dwWidth)*(pSurfaceData->dwFormatBitCount >> 3);
	lPitch = pSurfaceData->lPitch;
			
	// SMAC@microsoft.com  :  Yes, you should transfer the end line.  
	// Therefore, if we only wanted you to transfer a single line, start line would equal end line.	
	lCount = (pTransfer->lEndLine - pTransfer->lStartLine + 1);
	if (pTransfer->dwTransferFlags & DDTRANSFER_INVERT)
	{
		// if we want to invert things, then we should 
		// negate the pitch and put the start offset at the end.
		
		dwOffsetStartByte = dwOffsetStartByte + lPitch * (lCount - 1);
		lPitch = -lPitch;
	}

	pDestMDL = (PMDL)(pTransfer->pDestMDL);
	if (pDestMDL->ByteCount < (ULONG)(lLength * lCount))
	{ 
		VideoDebugPrint((0,"VPEMINI: NVPCheckForLaunch() - Destination memory is not large enough to hold this image, truncating line count\n"));
	//	lCount = (pTransfer->pDestMDL->ByteCount + pTransfer->pDestMDL->ByteOffset) / lLength;
		lCount = pDestMDL->ByteCount / lLength;
	}

#ifdef _NV_DPC
	ntfParam.hObjectParent = pVPEContext->dwDeviceId;
	nFreeCount = pVPEContext->nFreeCount;
	while (nFreeCount < 8*4) 
	{
		//RmInterrupt(&ntfParam);
		nFreeCount = NvGetFreeCount(pChan, 5);
	}
	pVPEContext->nFreeCount = nFreeCount - (8*4);
#endif

	// set mem to mem transfer object !
	pChan->subchannel[NVP_M2M_SUBCHANNEL].SetObject = NVP_M2M_OBJECT;
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetIn	= dwOffsetStartByte;
	//pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetOut = (unsigned long)pTheTransfer->DestMDL->ByteOffset;
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetOut = 0;
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.PitchIn	= lPitch;	// use the pitch in so that we can do interleaved
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.PitchOut	= lLength;  // use line length on pitch out so that it squishes together
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.LineLengthIn = lLength; 
	
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.LineCount 	= lCount;
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.Format 		=  ( 1 | (1 << 8));
	pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.BufferNotify = NV039_NOTIFY_WRITE_THEN_AWAKEN_1;

	pTransfer->dwTransferStatus = NVP_TRANSFER_IN_PROGRESS;

	return DX_OK;
}

/*
==============================================================================
	
	NVPRecreateContext

	Description:	if necessary, create context DMA for mem to mem object
					(DMA version)

	Date:			Apr 22, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPRecreateContext(
	PNVP_CONTEXT pVPEContext,
	PMDL pDestMDL) 
{
	U032 dwStart;
	U032 dwLimit;

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
		return NVP3RecreateContext(pVPEContext, pDestMDL);

	// first check to see if we can reuse the OLD context... since context creation is expensive.
	dwStart = (U032)(pDestMDL->lpMappedSystemVa);
	dwLimit = (pDestMDL->ByteCount) - 1;
	
	if (pVPEContext->bxfContextCreated)
	{
		// do not recreate if same context
		if ((dwStart == pVPEContext->dwTransferCtxStart) && (dwLimit == pVPEContext->dwTransferCtxSize))
			return DX_OK;
	}

	// free the old context first
	if (pVPEContext->bxfContextCreated)
	{
		// free m2m dma context
		RmFree(pVPEContext->hClient, pVPEContext->dwDeviceId, NVP_M2M_CONTEXT);

		pVPEContext->bxfContextCreated = FALSE;
	}
	
	// setup Notify Context
	if (RmAllocContextDma(
					pVPEContext->hClient,
	         		NVP_M2M_CONTEXT,
	         		NV01_CONTEXT_DMA_FROM_MEMORY,
	         		0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
					0, // selector
	         		dwStart,
	         		dwLimit) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
	{
    	VideoDebugPrint((0,"VPEMINI: Cannot allocate mem 2 mem context\n"));
		return DXERR_GENERIC;
	}

	pVPEContext->dwTransferCtxStart = dwStart;
	pVPEContext->dwTransferCtxSize = dwLimit;
	pVPEContext->bxfContextFlushed = FALSE;
	pVPEContext->bxfContextCreated = TRUE;

	return DX_OK;
}

/*
==============================================================================
	
	NVPCheckForLaunch

	Description:	try to launch another mem transfer if engine not busy
					(DMA version)

	Date:			Apr 22, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPCheckForLaunch(
	PNVP_CONTEXT pVPEContext,
	PNVP_SURFACE_INFO pSurface)
{
	PNVP_TRANSFER_BUFFER pTransfer;
	LPDDSURFACEDATA pSurfaceData;
	NvNotification *aM2MNotifiers;
	PMDL pDestMDL;
	U032 dwOffsetStartByte;
	LONG lLength, lPitch, lCount;
	PNVP_SURFACE_INFO pSurf = NULL;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;

	// NV3 specific code
	if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
		return NVP3CheckForLaunch(pVPEContext, pSurface);

//	VideoDebugPrint((3,"VPEMINI: NVPCheckForLaunch()\n"));

	aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);

	// is transfer in progress ?
	if (aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS)
		return DXERR_GENERIC;

	// or no transfer request !?
	if ((pVPEContext->nCurTransferIdx == -1) || !(pVPEContext->bxfContextCreated))
	{
		return DXERR_GENERIC;
	}

	// which surface are we transferring from ?
	pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
	pSurfaceData = (LPDDSURFACEDATA) (pTransfer->pSurfaceData);
	if (pSurface == NULL)
	{
		NVPGetSurface(pVPEContext, pSurfaceData->dwSurfaceOffset, &pSurf);
		if (pSurf == NULL)
		{
			VideoDebugPrint((0,"VPEMINI: NVPCheckForLaunch() can't get pointer to surface\n"));
			return DXERR_GENERIC;
		}
	}
	else
		pSurf = pSurface;

	// reset notifier status
	aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

	// start at given line
	dwOffsetStartByte = pSurfaceData->dwSurfaceOffset + (pSurfaceData->lPitch * pTransfer->lStartLine);

	// lLength and lPitch might not be the same (case of interleaved fields !!!)
	lLength = (pSurfaceData->dwWidth)*(pSurfaceData->dwFormatBitCount >> 3);
	lPitch = pSurfaceData->lPitch;
			
	// SMAC@microsoft.com  :  Yes, you should transfer the end line.  
	// Therefore, if we only wanted you to transfer a single line, start line would equal end line.	
	lCount = (pTransfer->lEndLine - pTransfer->lStartLine + 1);
	if (pTransfer->dwTransferFlags & DDTRANSFER_INVERT)
	{
		// if we want to invert things, then we should 
		// negate the pitch and put the start offset at the end.
		
		dwOffsetStartByte = dwOffsetStartByte + lPitch * (lCount - 1);
		lPitch = -lPitch;
	}

	pDestMDL = (PMDL)(pTransfer->pDestMDL);
	if (pDestMDL->ByteCount < (ULONG)(lLength * lCount))
	{ 
		VideoDebugPrint((0,"VPEMINI: NVPCheckForLaunch() - Destination memory is not large enough to hold this image, truncating line count\n"));
	//	lCount = (pTransfer->pDestMDL->ByteCount + pTransfer->pDestMDL->ByteOffset) / lLength;
		lCount = pDestMDL->ByteCount / lLength;
	}

	// get a pointer to the VPE dma channel
	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(20);

	NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT, NVP_M2M_OBJECT);
	NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_BUFFER_OUT, NVP_M2M_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_OFFSET_IN, dwOffsetStartByte);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_OFFSET_OUT, 0);	//(unsigned long)pTransfer->DestMDL->ByteOffset
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_PITCH_IN, lPitch);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_PITCH_OUT, lLength);

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_LINE_LENGTH_IN, lLength);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_LINE_COUNT, lCount);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_FORMAT, ( 1 | (1 << 8)));
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_BUFFER_NOTIFY, NV039_NOTIFY_WRITE_THEN_AWAKEN_1);

	pTransfer->dwTransferStatus = NVP_TRANSFER_IN_PROGRESS;
	// go !
    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;

	return DX_OK;
}


/*
==============================================================================
	
	NVPGetTransferStatus

	Description:	returns status of buffer transfer (from video mem to 
					system mem)

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPGetTransferStatus(
	PVOID pContext, 
	PVOID pNotUsed, 
	PDDGETTRANSFEROUTINFO pOutInfo)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
	PNVP_TRANSFER_BUFFER pTransfer;
	PNVP_CONTEXT pVPEContext;

//	VideoDebugPrint((3,"VPEMINI: NVPGetTransferStatus()\n"));

	// get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
	pVPEContext = &(pHwDevExt->avpContexts[0]);

	if (pVPEContext->nRecentTransferIdx == NVP_EOQ)
		return DXERR_GENERIC;

	pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nRecentTransferIdx]);

	if (pTransfer->dwTransferStatus == NVP_TRANSFER_DONE)
	{
		// return MS transfer ID
		pOutInfo->dwTransferID = pTransfer->dwTransferId;
		NVPRemoveTransfer(pVPEContext, pVPEContext->nRecentTransferIdx);

	}
	else 
		return DXERR_GENERIC;

	return DX_OK;
}

/*
==============================================================================
	
	NVPAddTransfer

	Description:	adds transfer buffer to queue

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPAddTransfer(
	PNVP_CONTEXT pVPEContext,
	PDDTRANSFERININFO pInInfo,
	BOOL bIsOddField)
{
	PNVP_TRANSFER_BUFFER pNewTransfer;
	int i = pVPEContext->nCurTransferIdx;

	VideoDebugPrint((3,"VPEMINI: NVPAddTransfer()\n"));

	if (pVPEContext->nNumTransfer >= NVP_MAX_TRANSFER_BUFFERS)
		return DXERR_GENERIC;
	
	while (i != NVP_EOQ)
	{
		if (pVPEContext->aTransferBuffers[i].dwTransferId == pInInfo->dwTransferID)
			// a transfer request with same ID is already queued !!!
			return DXERR_GENERIC;
		else 
			i = pVPEContext->aTransferBuffers[i].nNextTransferIdx;
	}

		
	// queue new transfer request !
	i = pVPEContext->nLastTransferIdx;
	pVPEContext->nLastTransferIdx = ++(pVPEContext->nLastTransferIdx) % NVP_MAX_TRANSFER_BUFFERS;
	pNewTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nLastTransferIdx]);
	if (i != NVP_EOQ)
		pVPEContext->aTransferBuffers[i].nNextTransferIdx = pVPEContext->nLastTransferIdx;
	pNewTransfer->nNextTransferIdx = NVP_EOQ;
	pNewTransfer->nPrevTransferIdx = i;
	(pVPEContext->nNumTransfer)++;
	if (pVPEContext->nCurTransferIdx == NVP_EOQ)
		pVPEContext->nCurTransferIdx = pVPEContext->nLastTransferIdx;

    // DDTRANSFER_HALFLINES indicates that, due to half line issues, the odd field contains an 
    // extra line of useless data at the top that the driver must account for
	if ((pInInfo->dwTransferFlags & DDTRANSFER_HALFLINES) && (bIsOddField))
	{
		pNewTransfer->lStartLine = (LONG)(pInInfo->dwStartLine) + 1;
		pNewTransfer->lEndLine = (LONG)(pInInfo->dwEndLine) + 1;
	} 
	else 
	{
		pNewTransfer->lStartLine = (LONG)(pInInfo->dwStartLine);
		pNewTransfer->lEndLine = (LONG)(pInInfo->dwEndLine);
	}

	// copy input data...
	pNewTransfer->pSurfaceData = (ULONG) (pInInfo->lpSurfaceData);
	pNewTransfer->dwTransferId = pInInfo->dwTransferID;
	pNewTransfer->dwTransferFlags = pInInfo->dwTransferFlags;
	pNewTransfer->pDestMDL = (ULONG) (pInInfo->lpDestMDL);
	pNewTransfer->dwTransferStatus = NVP_TRANSFER_QUEUED;

	return DX_OK;
}


/*
==============================================================================
	
	NVPCancelTransfer

	Description:	cancels transfer request before it gets processed 

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPCancelTransfer(
	PNVP_CONTEXT pVPEContext,
	PDDTRANSFERININFO pInInfo)
{
	PNVP_TRANSFER_BUFFER pTransfer = NULL;
	int i = pVPEContext->nCurTransferIdx;

	VideoDebugPrint((3,"VPEMINI: NVPCancelTransfer()\n"));

	// look for transfer in queue
	while (i != NVP_EOQ)
	{
		if (pVPEContext->aTransferBuffers[i].dwTransferId == pInInfo->dwTransferID)
		{
			pTransfer = &(pVPEContext->aTransferBuffers[i]);
			break;
		}
		i = pVPEContext->aTransferBuffers[i].nNextTransferIdx;
	}

	if ((pTransfer) && (pTransfer->dwTransferStatus == NVP_TRANSFER_QUEUED))
		return NVPRemoveTransfer(pVPEContext, i);
	else
		return DXERR_GENERIC;
}

/*
==============================================================================
	
	NVPRemoveTransfer

	Description:	removes transfer request from queue

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPRemoveTransfer(
	PNVP_CONTEXT pVPEContext,
	int nTransferIdx)
{
	PNVP_TRANSFER_BUFFER pTransfer = &(pVPEContext->aTransferBuffers[nTransferIdx]);

	VideoDebugPrint((3,"VPEMINI: NVPRemoveTransfer()\n"));

	// check limits
	if (nTransferIdx == pVPEContext->nCurTransferIdx)
		pVPEContext->nCurTransferIdx = pTransfer->nNextTransferIdx;
	else if (nTransferIdx == pVPEContext->nLastTransferIdx)
		pVPEContext->nLastTransferIdx = pTransfer->nPrevTransferIdx;
	// reset values
	pTransfer->dwTransferId = NVP_UNDEF;
	pTransfer->dwTransferStatus = NVP_TRANSFER_FREE;
	// re-map indices
	if (pTransfer->nPrevTransferIdx != NVP_EOQ)
	{
		pVPEContext->aTransferBuffers[pTransfer->nPrevTransferIdx].nNextTransferIdx = pTransfer->nNextTransferIdx;
		pTransfer->nPrevTransferIdx = NVP_EOQ;
	}
	if (pTransfer->nNextTransferIdx != NVP_EOQ)
	{
		pVPEContext->aTransferBuffers[pTransfer->nNextTransferIdx].nPrevTransferIdx = pTransfer->nPrevTransferIdx;
		pTransfer->nNextTransferIdx = NVP_EOQ;
	}

	// one down !
	(pVPEContext->nNumTransfer)--;

	return DX_OK;
}
	

/*
==============================================================================
	
	NVPCheckTransferStatus

	Description:	checks for transfer status. Calls MS callback. 

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
DWORD NVPCheckTransferStatus(
	PHW_DEVICE_EXTENSION pHwDevExt)
{
	NvNotification *aM2MNotifiers;
	PNVP_CONTEXT pVPEContext;
	PDX_IRQDATA pIRQData;
	PNVP_TRANSFER_BUFFER pTransfer;
	U032 status;

	VideoDebugPrint((3,"VPEMINI: NVPCheckTransferStatus()\n"));

	// get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
	pVPEContext = &(pHwDevExt->avpContexts[0]);

	aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);

	status = aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status;
	
	if (status == NV039_NOTIFICATION_STATUS_IN_PROGRESS)
		// still in progress
		return FALSE;
	else if (status == NV039_NOTIFICATION_STATUS_DONE_SUCCESS)
	{
		pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

		// finished ok!
		if (pTransfer->dwTransferStatus == NVP_TRANSFER_IN_PROGRESS)
			pTransfer->dwTransferStatus = NVP_TRANSFER_DONE;

		pVPEContext->nRecentTransferIdx = pVPEContext->nCurTransferIdx;
	} 
	else
	{
		VideoDebugPrint((0,"VPEMINI: NVPCheckTransferStatus() - memory transfer failed error %d\n", status));

		pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

		// finished with an error !
		if (pTransfer->dwTransferStatus == NVP_TRANSFER_IN_PROGRESS)
			pTransfer->dwTransferStatus = NVP_TRANSFER_FAILURE;

	}

	// move to next transfer in queue (*** not sure whether I should do this even when an error had occured ?!?!?!?)
	pVPEContext->nCurTransferIdx = pTransfer->nNextTransferIdx;

	// we pass on the interrupt
	if (pHwDevExt->IRQCallback) //&& (pHwDevExt->dwIRQSources & DDIRQ_BUSMASTER))
	{
		pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
		pIRQData->dwIrqFlags |= DDIRQ_BUSMASTER;
		((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
	}
	
	return DX_OK;
}

/*
==============================================================================
	
	NVPTransferNotify

	Description:	checks for transfer status. Calls MS callback. 

	Date:			Mar 16, 1999
	
	Author:			H. Azar

==============================================================================
*/
void NVPTransferNotify(
	PVOID pHwDevExtension)
{
	PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION) pHwDevExtension;
	PNVP_CONTEXT pVPEContext;

	// call MS callback if done with transfer
	if (NVPCheckTransferStatus(pHwDevExt) == DX_OK)
	{
		// get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
		pVPEContext = &(pHwDevExt->avpContexts[0]);
		//  transfer next in queue !
//		NVPCheckForLaunch(pVPEContext, NULL);
	}
}




//---------------------------------------------------------------------------
// vddPreScale
//      Prescales YUV422 surface.  The new width and height are returned.  If
//      this routine does the field prescaling, it flips off the field flags
//      to indicate that the flip should not bob.
//

BOOL NVPPreScale(
    PNVP_CONTEXT  pVPEContext, 
	PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwSrcOffset, 
    DWORD         dwSrcPitch,
    DWORD         dwDstOffset, 
    DWORD         dwDstPitch,
    DWORD         *pWidth, 
    DWORD         *pHeight,
    DWORD         *pField)
{
    DWORD preScaleDeltaX;
    DWORD preScaleDeltaY;
    DWORD preScaleOverlay;
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwOutSizeX, dwOutSizeY, dwOutFormat;
    DWORD dwInPitch, dwInOffset, dwInPoint, dwInSize;
    BOOL isField;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;

    if (pOverlayInfo->dwOverlayDstWidth == 0 ||
        pOverlayInfo->dwOverlayDstHeight == 0)
        return FALSE;

    isField = IS_FIELD(*pField);

    if (pOverlayInfo->dwOverlayDstWidth <= 1) 
    {
        dwDeltaX = 0x100000;
	}
	else
	{
	    dwDeltaX = (pOverlayInfo->dwOverlaySrcWidth - 1) << 16;
	    dwDeltaX /= pOverlayInfo->dwOverlayDstWidth - 1;
	    dwDeltaX <<= 4;
	}

    if (pOverlayInfo->dwOverlayDstHeight <= 1) 
    {
        dwDeltaY = 0x100000;
	}
	else
	{
	    dwDeltaY = (pOverlayInfo->dwOverlaySrcHeight - 1) << 16; //(*pHeight - 1) << 16; //
	    // err on the side of caution, need to do this because of the 1/4 pixel offset
	    // ????? why is NV4 different from NV5?  NV4 HW must be a little wonky
	    if (pVPEContext->dwChipId <= NV_DEVICE_VERSION_4) {
	        dwDeltaY /= pOverlayInfo->dwOverlayDstHeight + 2;
	    } else {
	        dwDeltaY /= pOverlayInfo->dwOverlayDstHeight + 1;
	    }
	    dwDeltaY <<= 4;
    }

    preScaleOverlay = 0;

    if (pOverlayInfo->dwOverlaySrcWidth > (pOverlayInfo->dwOverlayDstWidth * pOverlayInfo->dwOverlayMaxDownScale)) 
    {
        // use X prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
        dwOutSizeX = (pOverlayInfo->dwOverlayDstWidth + 2) & ~1;
        preScaleDeltaX = dwDeltaX;
        pOverlayInfo->dwOverlayDeltaX = 0x100000;
    } else {
        // use X overlay scaler
        dwOutSizeX = (*pWidth + 2) & ~1;
        preScaleDeltaX = 0x100000;
        pOverlayInfo->dwOverlayDeltaX = dwDeltaX;
    }

    if (pOverlayInfo->dwOverlaySrcHeight > (pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayMaxDownScale))
	{
        // use Y prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
        dwOutSizeY = pOverlayInfo->dwOverlayDstHeight;
        preScaleDeltaY = dwDeltaY;
        pOverlayInfo->dwOverlayDeltaY = 0x100000;
    } 
    else 
    {
        // use Y overlay scaler
        dwOutSizeY = *pHeight;
        preScaleDeltaY = 0x100000;
        pOverlayInfo->dwOverlayDeltaY = dwDeltaY;
    }

    if ((pVPEContext->dwChipId < NV_DEVICE_VERSION_10) || (preScaleOverlay & NV_PRESCALE_OVERLAY_Y)) 
    {
        if (isField) 
        {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_FIELD;
            if (*pField & VPP_INTERLEAVED)
                preScaleDeltaY >>= 1;
        }
    }

    if (preScaleOverlay == 0)
        return FALSE;

    dwInSize = asmMergeCoords(*pWidth, *pHeight);
    dwInPoint = 0;
    dwInPitch = dwSrcPitch;
    dwInOffset = dwSrcOffset;
    if (isField && (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        if (*pField & VPP_INTERLEAVED) {
            dwInPitch <<= 1;
            *pHeight >>= 1;
            // if (*pField & VPP_ODD) {
            if (*pField & VPP_EVEN) {
                dwInOffset += dwSrcPitch;
            }
        }
        if (*pField & VPP_BOB) {
            // use -1/4, +1/4 biasing for bob fields
            // if (*pField & VPP_ODD) {
            if (*pField & VPP_EVEN) {
                dwInPoint = 0xFFFC0000;
            } else {
                dwInPoint = 0x00040000;
            }
        }
    }

//    if (dwFourCC == FOURCC_UYVY) {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8;
//    } else {
//        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8;
//    }

	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(38);

	// program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_OBJECT , NVP_DVD_SUBPICTURE_OBJECT); // pVPEContext->dwDVDSubPicObjectID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_POINT, 0);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_SIZE, (asmMergeCoords(dwOutSizeX, dwOutSizeY)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_FORMAT, ((dwOutFormat << 16) | dwDstPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_OFFSET, dwDstOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_DELTA_DU_DX, preScaleDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_DELTA_DV_DY, preScaleDeltaY);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_SIZE, dwInSize);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_FORMAT, ((dwOutFormat << 16) | dwInPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_OFFSET, dwInOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_POINT, dwInPoint);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_DELTA_DU_DX, preScaleDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_DELTA_DV_DY, preScaleDeltaY);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_SIZE, dwInSize);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_FORMAT, ((NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwInPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_OFFSET, dwInOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_POINT, 0);

    //NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_NOTIFY, NV038_NOTIFY_WRITE_THEN_AWAKEN);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_NO_OPERATION, 0);

    // this forces the H/W to serialize
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_OVERLAY, NVP_DVD_SUBPICTURE_CONTEXT);

//    NVP_DMAPUSH_START();
    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;

    // return changes in state
    if (//(pDriverData->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) ||
        (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        // with HQVUp, we no longer use the overlay to bob
        *pField &= ~(VPP_ODD | VPP_EVEN | VPP_BOB | VPP_INTERLEAVED);
    }

    *pHeight = dwOutSizeY;
    *pWidth  = dwOutSizeX;

    return TRUE;
}

//---------------------------------------------------------------------------
// vddFlip
//      Flip to surface.
//

BOOL NVPFlip(
    PNVP_CONTEXT  pVPEContext, 
	PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwOffset,        // source surface offset 
    DWORD         dwPitch,         // source pitch
	DWORD         dwWidth,         // source width
	DWORD         dwHeight,        // source height
    DWORD         dwIndex,         // overlay buffer index
    DWORD         dwFlags)         // VPP flags
{
    NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pOverlayInfo->pNvPioFlipOverlayNotifierFlat;
    DWORD dwPointIn, dwOverlayFormat, dwSizeIn;
    DWORD dwDeltaX, dwDeltaY;
	Nv4ControlDma *pDmaChannel;
	U032 nvDmaCount;

    dwPointIn = (dwOffset & NV_OVERLAY_BYTE_ALIGNMENT_PAD) << 3;  // << 3 insteed of << 4 because YUYV is 2 bytes wide
    dwSizeIn = asmMergeCoords((dwWidth & ~1), (dwHeight & ~1));
    dwSizeIn += dwPointIn;
    dwOffset &= ~NV_OVERLAY_BYTE_ALIGNMENT_PAD;

    // Remember this in case we get a SetOverlayPosition call
    //pOverlayInfo->dwOverlaySrcOffset = dwOffset;
    //pOverlayInfo->dwOverlaySrcPitch = dwPitch;

    dwDeltaX = pOverlayInfo->dwOverlayDeltaX;
    dwDeltaY = pOverlayInfo->dwOverlayDeltaY;

    if (IS_FIELD(dwFlags)) {
        if (dwFlags & VPP_INTERLEAVED) {
            // if (dwFlags & VPP_ODD) { 
            if (dwFlags & VPP_EVEN) {
                dwOffset += dwPitch;
            }
            dwPitch <<= 1;
            dwHeight >>= 1;
            dwDeltaY >>= 1;
        }
        if (dwFlags & VPP_BOB) {
            // if (dwFlags & VPP_ODD) {
            if (dwFlags & VPP_EVEN) {
                dwPointIn += 0xFFFC0000;
            } else {
                dwPointIn += 0x00040000;
            }
        }
    }

    // replace old surface pitch value with current pitch value
    dwOverlayFormat = pOverlayInfo->dwOverlayFormat;
    dwOverlayFormat &= 0xFFFF0000;
    dwOverlayFormat |= dwPitch;
    dwOverlayFormat |= (1 << 31);

/*

    if (pVPEContext->dwVPConnectFlags & NVP_CONNECT_HDTV) // progressive (non-interlaced) and 16bit VIP
        dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_MATRIX_ITURBT709 << 24);
*/
    if (dwIndex & 0x1) {
        pPioFlipOverlayNotifier[2].status = NV_IN_PROGRESS;
    } else {
        pPioFlipOverlayNotifier[1].status = NV_IN_PROGRESS;
    }

	// program overlay to flip
	pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
	NVP_DMAPUSH_CHECK_FREE_COUNT(20);

	// program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_SIZE_IN(dwIndex), dwSizeIn);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_SIZE_OUT(dwIndex), (asmMergeCoords(pOverlayInfo->dwOverlayDstWidth, pOverlayInfo->dwOverlayDstHeight)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_POINT_IN(dwIndex), dwPointIn);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_POINT_OUT(dwIndex), (asmMergeCoords(pOverlayInfo->dwOverlayDstX, pOverlayInfo->dwOverlayDstY)));

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_OFFSET(dwIndex), dwOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_COLORKEY(dwIndex), pOverlayInfo->dwOverlayColorKey);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_DU_DX(dwIndex), dwDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_DV_DY(dwIndex), dwDeltaY);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_FORMAT(dwIndex), dwOverlayFormat);

//    NVP_DMAPUSH_START();
    pDmaChannel->Put = nvDmaCount << 2;
	pVPEContext->nvDmaCount = nvDmaCount;

    return TRUE;
}

//---------------------------------------------------------------------------
// vddPrescaleAndFlip
//
//      VPE implementation of selected parts of DDRAW's VPP pipe.  Only 
//      prescaling and flip are performed.
//

#define NEXT_SURFACE { \
            dwSrcOffset = dwDstOffset;  \
            dwSrcPitch = dwDstPitch;    \
            dwDstOffset = dwNextOffset; \
            dwDstPitch = dwNextPitch;   \
            dwNextOffset = dwSrcOffset; \
            dwNextPitch = dwSrcPitch;   \
            dwOpCount++;                \
        }

#define NV_WIN_DEVICE    0x00002002

BOOL NVPPrescaleAndFlip(
    PNVP_CONTEXT  pVPEContext, 
	PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwOffset,        // source surface offset 
    DWORD         dwPitch,         // source pitch
    DWORD         dwOvIndex,         // overlay buffer index
    DWORD         dwFlags)         // VPP flags
{
	DWORD dwHeight, dwWidth, i = 0;
    DWORD dwSrcOffset, dwSrcPitch;
    DWORD dwDstOffset, dwDstPitch;
    DWORD dwNextOffset, dwNextPitch;
    DWORD dwOpCount;
    DWORD dwWorkSurfaces;
    BOOL  doXPreScale, doYPreScale, doPreScale;
    BOOL  isDownScale;
    DWORD dwRoundX, dwRoundY;
    DWORD vppExec;
    BOOL  isField;
    NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pOverlayInfo->pNvPioFlipOverlayNotifierFlat;


#if 0 //#ifdef DEBUG
    char debugstr[256];

    debugstr[0] = '\0';
    if (dwFlags & VPP_ODD)          strcat(debugstr, "odd  ");
    if (dwFlags & VPP_EVEN)         strcat(debugstr, "even ");
    if (dwFlags & VPP_BOB)          strcat(debugstr, "bob ");
    if (dwFlags & VPP_INTERLEAVED)  strcat(debugstr, "int ");
    if (dwFlags & VPP_VIDEOPORT)    strcat(debugstr, "vp ");
    if (dwFlags & VPP_WAIT)         strcat(debugstr, "wait ");
    if (dwFlags & VPP_OPTIMIZEFLIP) strcat(debugstr, "opt ");
    strcat(debugstr, "REQ: ");
    if (dwFlags & VPP_CONVERT)      strcat(debugstr, "cvt ");
    if (dwFlags & VPP_SUBPICTURE)   strcat(debugstr, "sp ");
    if (dwFlags & VPP_PRESCALE)     strcat(debugstr, "ps ");
    if (dwFlags & VPP_COLOURCONTROL) strcat(debugstr, "cc ");
    if (dwFlags & VPP_TEMPORAL)     strcat(debugstr, "tf ");
    if (dwFlags & VPP_DEINTERLACE)  strcat(debugstr, "df ");
#endif

    //assert(pDriverData);

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // find out if overlay hardware is busy, if so return "overlay busy" error code.
    // Perhaps check a "number of overlay buffers programmed" count.  When this count is less 
    // than 2, the overlay hardare is not busy and when it's equal to 2, the hardwar is busy. 
    // If it's not busy, then program it up here and increment the count.  Then use a kernel 
    // callback to decrement this count upon buffer complete.
    //
    
    DWORD dwIndex = pOverlayInfo->dwOverlayBufferIndex;

#ifdef _NVP_DTRACE
    DTRACE(0xdead0001);
    DTRACE_CYCLE_DELTA();
    DTRACE(dwOffset);
    DTRACE(dwIndex);

//    return TRUE;
#endif

/*
        while ((pPioFlipOverlayNotifier[dwIndex + 1].status == NV_IN_PROGRESS) && (i < VDD_VPP_TIME_OUT))
            i++;
        // timed-out... stop the appropriate buffer and return error
        if (i >= VDD_VPP_TIME_OUT)
*/
    // drop field if previous one is still being drawn (same overlay buffer) !
    if (pPioFlipOverlayNotifier[dwIndex + 1].status == NV_IN_PROGRESS)
        return 1;

    dwWorkSurfaces = pOverlayInfo->dwExtra422NumSurfaces;
    vppExec = 0;
    dwOpCount = 0;
    dwSrcOffset = dwOffset;
    dwSrcPitch = dwPitch;
    dwDstOffset = pOverlayInfo->aExtra422OverlayOffset[pOverlayInfo->dwExtra422Index];
    dwDstPitch = pOverlayInfo->dwExtra422Pitch;
    dwNextOffset = pOverlayInfo->aExtra422OverlayOffset[pOverlayInfo->dwExtra422Index + 1];
    dwNextPitch = pOverlayInfo->dwExtra422Pitch;

    isField = IS_FIELD(dwFlags);

    doXPreScale = (pOverlayInfo->dwOverlaySrcWidth > (pOverlayInfo->dwOverlayDstWidth * pOverlayInfo->dwOverlayMaxDownScale));
    doYPreScale = (pOverlayInfo->dwOverlaySrcHeight > (pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayMaxDownScale));
  
    if (pVPEContext->dwChipId < NV_DEVICE_VERSION_10) {
        // NV4/5 also prescales:
        //  - all BOBed fields
        //  - vertical upscales when HQVUp is enabled
        doYPreScale = TRUE; //doYPreScale || isField; //|| (pOverlayInfo->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)
    }

    doPreScale = (dwFlags & VPP_PRESCALE) &&
                 (doXPreScale || doYPreScale);

    isDownScale = FALSE;
    if (doPreScale) 
    {
        DWORD dwSrcArea, dwDstArea;

        dwSrcArea = pOverlayInfo->dwOverlaySrcHeight * pOverlayInfo->dwOverlaySrcWidth;
        if ((dwFlags & (VPP_ODD | VPP_EVEN)) && (dwFlags & VPP_INTERLEAVED)) 
        {
            // bob mode
			dwSrcArea >>= 1;
        }
        dwDstArea = pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayDstWidth;
        if (dwSrcArea > dwDstArea) 
        {
            isDownScale = TRUE;
        }
    }

    if (dwWorkSurfaces < 2) {
        // no work surfaces, can't do anything
        doPreScale = FALSE;
    }

    // adjust for the subrectangle
    dwRoundX = pOverlayInfo->dwOverlaySrcX & 0x1;
    dwRoundY = pOverlayInfo->dwOverlaySrcY & 0x1;
    dwSrcOffset += (pOverlayInfo->dwOverlaySrcY & ~0x1) * dwSrcPitch;
    dwSrcOffset += (pOverlayInfo->dwOverlaySrcX & ~0x1) << 1;   // always YUV422
    dwWidth = pOverlayInfo->dwOverlaySrcSize & 0xFFFF;
    dwHeight = pOverlayInfo->dwOverlaySrcSize >> 16;
    if (dwWidth & 1)
        dwWidth += 1;
    if (dwRoundY)
        dwHeight += 1;

    // do the prescaling now
    if (doPreScale) 
    {
        if (NVPPreScale(pVPEContext, pOverlayInfo, 
                        dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch, &dwWidth, &dwHeight, 
                        &dwFlags)) {
            NEXT_SURFACE;
            vppExec |= VPP_PRESCALE;
        }
    }

    if (dwRoundY) 
    {
        dwSrcOffset += dwSrcPitch;
        dwHeight -= 1;
    }

//    if (dwRoundX)
//        dwWidth -= 1;

    // always do the flip, even if we are not flipping
    if (NVPFlip(pVPEContext, pOverlayInfo, dwSrcOffset,
                dwSrcPitch, dwWidth, dwHeight, dwIndex, dwFlags)) 
    {
        pOverlayInfo->dwOverlayBufferIndex ^= 1;
        if (dwWorkSurfaces == 0) 
        {
            pOverlayInfo->dwExtra422Index = 0;
        }
        else if (dwWorkSurfaces <= 3)
        {
            pOverlayInfo->dwExtra422Index = (pOverlayInfo->dwExtra422Index + 1) % dwWorkSurfaces;
        } 
        else 
        {
            pOverlayInfo->dwExtra422Index = (pOverlayInfo->dwExtra422Index + 2) % dwWorkSurfaces;
        }
    } 
    else 
    {
        return FALSE;
    }

    return TRUE;
}


#endif // #ifdef NVPE