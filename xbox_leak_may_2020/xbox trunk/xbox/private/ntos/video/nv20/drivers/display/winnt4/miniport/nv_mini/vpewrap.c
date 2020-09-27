//***************************************************************************
//  Module Name:
//
//      vpewrap.c
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

#ifdef VPE_NT

#include "dderror.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"

typedef unsigned long   DWORD;
// Commented out benign redefinition// typedef int      BOOL;

#define INITGUID     // Instantiate GUID_DxApi
#include "dxmini.h"

#include "nv.h"

DWORD
GetIrqInfo(
    PVOID HwDeviceExtension,
    PVOID lpInput,
    PDDGETIRQINFO GetIrqInfo
    );

VP_STATUS
nvVPEQueryInterface(
    PVOID HwDeviceExtension,
    PQUERY_INTERFACE QueryInterface
    );

//
// Mark those routines that we can as pageable.  Remember that most
// "kernel-mode VPE" routines can be called at raised IRQL, which means
// that they must be kept non-paged.
//
// Note that these are the only two VPE miniport routines which may be
// pageable:
//

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE,nvVPEQueryInterface)
#pragma alloc_text(PAGE,GetIrqInfo)
#endif

// !!! Not hooking Lock is success, but not hooking SetPattern is failure?
// !!! Should dwFlags be ANDed or EQUALed?  (SkipNextField)
// !!! Should we mark the interrupt as unsharable?

DWORD
GetIrqInfo(
    PVOID HwDeviceExtension,
    PVOID lpInput,
    PDDGETIRQINFO GetIrqInfo
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DWORD status = DXERR_UNSUPPORTED;

    //
    // Sometimes, the motherboard BIOS or PNP may conspire to refuse to
    // allocate an interrupt to us.  So before we tell DirectDraw that
    // we support interrupts, we must first check if we've actually been
    // assigned one!
    //

    if ((hwDeviceExtension->BusInterruptLevel != 0) ||
        (hwDeviceExtension->BusInterruptVector != 0)) {

        GetIrqInfo->dwFlags = IRQINFO_HANDLED;

        status = DX_OK;
    }

    return status;
}

BOOLEAN
Interrupt(
    PVOID HwDeviceExtension
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    // add your IRQ handling code here
}

DWORD
EnableIrq(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PDDENABLEIRQINFO EnableIrqInfo,
    PVOID lpOutput
    )

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


DWORD
FlipVideoPort(
    PVOID HwDeviceExtension,
    PDDFLIPVIDEOPORTINFO FlipVideoPortInfo,
    PVOID lpOutput
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    ULONG offset;

    //
    // Get offset (from framebuffer) and add offset from surface:
    //

    offset = FlipVideoPortInfo->lpTargetSurface->dwSurfaceOffset
           + FlipVideoPortInfo->lpVideoPortData->dwOriginOffset;

    // flip the actual hw surface here

    return DX_OK;
}


DWORD
FlipOverlay(
    PVOID HwDeviceExtension,
    PDDFLIPOVERLAYINFO FlipOverlayInfo,
    PVOID lpOutput
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


DWORD
BobNextField(
    PVOID HwDeviceExtension,
    PDDBOBNEXTFIELDINFO BobNextFieldInfo,
    PVOID lpOutput
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


DWORD
SetState(
    PVOID HwDeviceExtension,
    PDDSETSTATEININFO SetStateInInfo,
    PDDSETSTATEOUTINFO SetStateOutInfo
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


DWORD
SkipNextField(
    PVOID HwDeviceExtension,
    PDDSKIPNEXTFIELDINFO SkipNextFieldInfo,
    PVOID lpOutput
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


DWORD
GetPolarity(
    PVOID HwDeviceExtension,
    PDDGETPOLARITYININFO GetPolarityInInfo,
    PDDGETPOLARITYOUTINFO GetPolarityOutInfo
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}

DWORD
GetCurrentAutoflip(
    PVOID HwDeviceExtension,
    PDDGETCURRENTAUTOFLIPININFO GetCurrentAutoflipInInfo,
    PDDGETCURRENTAUTOFLIPOUTINFO GetCurrentAutoflipOutInfo
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


DWORD
GetPreviousAutoflip(
    PVOID HwDeviceExtension,
    PDDGETPREVIOUSAUTOFLIPININFO GetPreviousAutoflipInInfo,
    PDDGETPREVIOUSAUTOFLIPOUTINFO GetPreviousAutoflipOutInfo
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    return DX_OK;
}


VP_STATUS
nvVPEQueryInterface(
    PVOID HwDeviceExtension,
    PQUERY_INTERFACE QueryInterface
    )

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

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    PDXAPI_INTERFACE DxApiInterface;
    VP_STATUS status;

    status = DXERR_UNSUPPORTED;

    if (IsEqualGUID(QueryInterface->InterfaceType, &GUID_DxApi)) {

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

        status = DX_OK;
    }

    return status;
}

#endif /* VPE_NT */

