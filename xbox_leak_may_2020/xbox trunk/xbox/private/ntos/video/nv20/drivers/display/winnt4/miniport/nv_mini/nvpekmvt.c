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

//***************************************************************************
//  Module Name:
//
//      nvpekmvt.c
//
//  Abstract:
//
//      This module contains the callback to Kernel mode video transport services
//      Win2k
//
//  Environment:
//
//      Kernel mode
//
//***************************************************************************

#ifdef NVPE

#define INITGUID     // Instantiate GUID_DxApi
#include "dderror.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"
#include "nv.h"
#include "nvpekmvt.h"


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

DWORD kmvtGetIrqInfo(
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

BOOLEAN kmvtInterrupt(PVOID HwDeviceExtension)
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

DWORD kmvtEnableIrq(
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

DWORD kmvtFlipVideoPort(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPVIDEOPORTINFO pFlipVideoPort,
    PVOID lpOutput)
{
    return FlipVideoPort(HwDeviceExtension, pFlipVideoPort, lpOutput);
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
DWORD kmvtFlipOverlay(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPOVERLAYINFO pFlipOverlayInfo,
    PVOID lpOutput)
{
    return FlipOverlay(HwDeviceExtension, pFlipOverlayInfo, lpOutput);
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

DWORD kmvtBobNextField(
    PVOID HwDeviceExtension,
    PNVP_DDBOBNEXTFIELDINFO pBobNextFieldInfo,
    PVOID lpOutput)
{
    return BobNextField(HwDeviceExtension, pBobNextFieldInfo, lpOutput);
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
DWORD kmvtSetState(
    PVOID HwDeviceExtension,
    PNVP_DDSETSTATEININFO pSetStateInInfo,
    PNVP_DDSETSTATEOUTINFO pSetStateOutInfo)
{
    return SetState(HwDeviceExtension, pSetStateInInfo, pSetStateOutInfo);
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
DWORD kmvtSkipNextField(
    PVOID HwDeviceExtension,
    PNVP_DDSKIPNEXTFIELDINFO pSkipNextFieldInfo,
    PVOID lpOutput)
{
    return SkipNextField(HwDeviceExtension, pSkipNextFieldInfo, lpOutput);
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

DWORD kmvtGetPolarity(
    PVOID HwDeviceExtension,
    PDDGETPOLARITYININFO pGetPolarityInInfo,
    PDDGETPOLARITYOUTINFO pGetPolarityOutInfo)
{
    return GetPolarity(HwDeviceExtension, pGetPolarityInInfo, pGetPolarityOutInfo);
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
DWORD kmvtGetCurrentAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETCURRENTAUTOFLIPININFO pGetCurrentAutoflipInInfo,
    PNVP_DDGETCURRENTAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{

    return GetCurrentAutoflip(HwDeviceExtension, pGetCurrentAutoflipInInfo, pGetAutoFlipInfoOut);
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
DWORD kmvtGetPreviousAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETPREVIOUSAUTOFLIPININFO pGetAutoflipInInfo,
    PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{
    return GetPreviousAutoflip(HwDeviceExtension, pGetAutoflipInInfo, pGetAutoFlipInfoOut);
}


DWORD kmvtTransfer(
	PVOID pContext, 
	PNVP_DDTRANSFERININFO pInInfo, 
	PNVP_DDTRANSFEROUTINFO pOutInfo)
{
	return NVPTransfer(pContext, pInInfo, pOutInfo);
}

DWORD kmvtGetTransferStatus(
	PVOID pContext, 
	PVOID pNotUsed, 
	PNVP_DDGETTRANSFEROUTINFO pOutInfo)
{
	return NVPGetTransferStatus(pContext, NULL, pOutInfo);
}

DWORD kmvtLockSurface(
    PVOID HwDeviceExtension,
    PNVP_DDLOCKININFO pLockInfoIn, 
    PNVP_DDLOCKOUTINFO pLockInfoOut)
{
    return LockSurface(HwDeviceExtension, pLockInfoIn, pLockInfoOut);
}

DWORD kmvtNVPCallbackDxApi(PVOID *pContext, DWORD dwIRQSource)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION) pContext;
    PDX_IRQDATA pIRQData;
	
	// can we pass on the interrupt !
	if (pHwDevExt->IRQCallback)
	{
		pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
		pIRQData->dwIrqFlags |= dwIRQSource;
		((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
	}

    return 0;
}

VOID
NVInterfaceReference(
    IN PVOID pContext)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = (PHW_DEVICE_EXTENSION) pContext;

    //
    // Increment the I2C interface reference count
    //
    hwDeviceExtension->I2C_Reference_Count++;

    return;
}

VOID
NVInterfaceDereference(
    IN PVOID pContext)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = (PHW_DEVICE_EXTENSION) pContext;

    //
    // Decrement the I2C interface reference count
    //
    hwDeviceExtension->I2C_Reference_Count--;

    return;
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
    VP_STATUS status = NO_ERROR;
    ULONG HwID;

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

        DxApiInterface->DxGetIrqInfo          = kmvtGetIrqInfo;
        DxApiInterface->DxEnableIrq           = kmvtEnableIrq;
        DxApiInterface->DxSkipNextField       = kmvtSkipNextField;
        DxApiInterface->DxBobNextField        = kmvtBobNextField;
        DxApiInterface->DxSetState            = kmvtSetState;
        DxApiInterface->DxFlipOverlay         = kmvtFlipOverlay;
        DxApiInterface->DxFlipVideoPort       = kmvtFlipVideoPort;
        DxApiInterface->DxGetPolarity         = kmvtGetPolarity;
        DxApiInterface->DxGetCurrentAutoflip  = kmvtGetCurrentAutoflip;
        DxApiInterface->DxGetPreviousAutoflip = kmvtGetPreviousAutoflip;
        DxApiInterface->DxTransfer            = kmvtTransfer;
        DxApiInterface->DxGetTransferStatus   = kmvtGetTransferStatus;
        DxApiInterface->DxLock                = kmvtLockSurface;
    }
    // I2c Interface
    else if (IsEqualGUID(QueryInterface->InterfaceType, &GUID_I2C_INTERFACE))
    {
        I2CINTERFACE *pI2cInterface;
        pI2cInterface = (I2CINTERFACE *) QueryInterface->Interface;

        if (QueryInterface->Size == sizeof(I2CINTERFACE))
        {    
            //
            // Check to see if we can convert PDO to hardware ID
            //
            if (hwDeviceExtension->pfnVideoPortGetAssociatedDeviceID)
            {
                //
                // Force hardware ID to use dynamic I2C port (0)
                // This allows dual heads to work properly
                //
                HwID = 0;
            }
            else    // Cannot get hardware ID value
            {
                //
                // Switch on the I2C Interface Version
                //
                switch(QueryInterface->Version)
                {
                    case 1:     /* Original I2C interface version */

                        //
                        // Force hardware ID to first external device (NVCAP_I2C_DEVICE_ID)
                        // i.e., assume I2C access is to external device (Secondary)
                        //
                        HwID = NVCAP_I2C_DEVICE_ID;
                        break;

                    case 2:     /* New I2C interface version */

                        //
                        // Check for special PDO value (0 or -1 is primary monitor)
                        //
                        if (((ULONG_PTR) QueryInterface->InterfaceSpecificData == 0) ||
                            ((ULONG_PTR) QueryInterface->InterfaceSpecificData == -1))
                        {
                            //
                            // Force hardware ID to first head DDC monitor
                            //
                            HwID = QUERY_MONITOR_ID;
                        }
                        else    // Regular PDO value
                        {
                            //
                            // Force hardware ID to first external device (NVCAP_I2C_DEVICE_ID)
                            // i.e., assume I2C access is to external device (Secondary)
                            //
                            HwID = NVCAP_I2C_DEVICE_ID;
                        }
                        break;

                    default:    /* Unknown I2C interface version */

                        VideoDebugPrint((1, "QueryInterface: I2C interface size incorrect!\n"));
                        status = ERROR_INVALID_PARAMETER;
                        break;
                }
            }
            //
            // Check for valid I2C interface
            //
            if (status == NO_ERROR)
            {
                //
                // Initialize the interface
                //
                pI2cInterface->_vddInterface.Size = sizeof(I2CINTERFACE);
                pI2cInterface->_vddInterface.Version = QueryInterface->Version;
                pI2cInterface->_vddInterface.Context = HwDeviceExtension;
                pI2cInterface->_vddInterface.InterfaceReference = NVInterfaceReference;
                pI2cInterface->_vddInterface.InterfaceDereference = NVInterfaceDereference;

                //
                // Switch on the I2C port number to use (0 = Dynamic)
                //
                switch(NVGetI2CPort(HwID))
                {
                    case I2C_ACCESS_DYNAMIC_PORT:

                        //
                        // Setup dynamic I2C port routines
                        //
                        pI2cInterface->i2cOpen = (I2COPEN) dynI2COpen;
                        pI2cInterface->i2cAccess = (I2CACCESS) dynI2CAccess;

                        break;

                    case I2C_ACCESS_PRIMARY_PORT:

                        //
                        // Setup primary I2C port routines
                        //
                        pI2cInterface->i2cOpen = (I2COPEN) priI2COpen;
                        pI2cInterface->i2cAccess = (I2CACCESS) priI2CAccess;

                        break;

                    case I2C_ACCESS_SECONDARY_PORT:

                        //
                        // Setup secondary I2C port routines
                        //
                        pI2cInterface->i2cOpen = (I2COPEN) secI2COpen;
                        pI2cInterface->i2cAccess = (I2CACCESS) secI2CAccess;

                        break;
                }
                //
                // Reference the interface before handing it out
                //
                pI2cInterface->_vddInterface.InterfaceReference(pI2cInterface->_vddInterface.Context);
            }
        }
        else    // Invalid I2C Interface Size
        {
            VideoDebugPrint((1, "QueryInterface: I2C interface size incorrect!\n"));
            status = ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        VideoDebugPrint((1, "QueryInteface: Unsupported Interface\n"));
        status = ERROR_INVALID_PARAMETER;
    }
    return status;
}

#endif // NVPE