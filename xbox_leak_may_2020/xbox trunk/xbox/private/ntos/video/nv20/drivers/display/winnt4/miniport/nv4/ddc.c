//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     DDC.C
//
// Abstract:
//
//     This is code specific to NV4
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"

#include "nv4_ref.h"

#include "nv32.h"
#include "nvsubch.h"
#include "nvalpha.h"

#include "cmdcnst.h"
#include <arb_fifo.h>
#include <nvntioctl.h>

#include "modes.h"
#include "tv.h"

#include "monitor.h"
#include "modedefs.h"

// MP/RM merge
#include "nvos.h"
#include "rm.h"

extern VP_STATUS
NVReadRegistryTwinViewInfoCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

extern ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask);


//******************************************************************************
// DDC support is only available for NT5 or later
//******************************************************************************


#if (_WIN32_WINNT >= 0x0500)


//******************************************************************************
// Forward Function Declarations
//******************************************************************************

VOID    WriteClockLine(PHW_DEVICE_EXTENSION HwDeviceExtension, UCHAR ucData);
VOID    WriteDataLine(PHW_DEVICE_EXTENSION HwDeviceExtension, UCHAR ucData);
BOOLEAN ReadClockLine(PHW_DEVICE_EXTENSION HwDeviceExtension);
BOOLEAN ReadDataLine(PHW_DEVICE_EXTENSION HwDeviceExtension);
VOID    WaitForVsyncActive(PHW_DEVICE_EXTENSION HwDeviceExtension);
ULONG GetCurrentScanLine(PHW_DEVICE_EXTENSION);

BOOLEAN
GetDdcInformation (
    PHW_DEVICE_EXTENSION phwDeviceExtension,
    ULONG  ulDisplayIndex,
    PUCHAR QueryBuffer,
    ULONG  BufferSize
    );

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE,GetDdcInformation)
#endif

//******************************************************************************
//
//  Register Description:
//
//
//        6.24 CR3E DDC Status Register                 0x3X5 Index 3E (R-)
//      
//      #define NV_CIO_CRE_DDC_STATUS__INDEX                     0x0000003E /*       */
//      
//         Bit 3    SDA pin.  This is the current value of the SDA pin.
//         Bit 2    SCL pin.  This is the current value of the SCL pin.
//      
//      
//        6.23 CR3F DDC Write Register                   0x3X5 Index 3F (RW)
//      #define NV_CIO_CRE_DDC_WR__INDEX                         0x0000003F /*       */
//      
//         Bits 7-6 Reserved
//      
//         Bits 5   Write SCL pin. If 1, this will tri-state the SCL bus.  This will allow
//      	    the value of the SCL pin to reach a 1 since the SCL pin is connected
//      	    to a external pull-up.  A 0 will enable the SCL pin and force
//      	    a 0 onto the SCL signal.
//      
//         Bits 4   Write SDA pin. If 1, this will tri-state the SDA bus.  This will allow
//      	    the value of the SDA pin to reach a 1 since the SDA pin is connected
//      	    to a external pull-up.  A 0 will enable the SDA pin and force
//      	    a 0 onto the SDA signal.
//      
//         Bits 3-2   Reserved. 
//      
//         Bit  1   DDC Write release.  When a 0 is written, it will release the SCL pin
//      	    and allow it to float high.  When a 1 is written, it will latch the 
//      	    SDA value at the next falling edge of SCL and hold the SCL pin low
//      	    until a 0 is written.
//      
//         Bit  0   DDC enable pin.  Enables writes onto the DDC pins.  
//      	    1 indicates writes are enabled. 
//      
//         This register defaults to 0x30 after reset.
//         
//         The DDC is really a I2C interface.  This interface is designed to allow easy
//         software controllability.  
//      
//      None.
//
//******************************************************************************


//******************************************************************************
//
//  Function:   WriteClockLine
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID WriteClockLine(PHW_DEVICE_EXTENSION HwDeviceExtension, UCHAR ucData)

    {
    UCHAR ucPortData;

    //**************************************************************************
    // Read current value then update
    //**************************************************************************
        
    CRTC_RD(NV_CIO_CRE_DDC_WR__INDEX, ucPortData);

    if (ucData)
        ucPortData |= 0x20;
    else        
        ucPortData &= (~0x20);  

    //**************************************************************************
    // Set new value
    //**************************************************************************

    CRTC_WR(NV_CIO_CRE_DDC_WR__INDEX, ucPortData);   

    }

//******************************************************************************
//
//  Function:   WriteDataLine
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID WriteDataLine(PHW_DEVICE_EXTENSION HwDeviceExtension, UCHAR ucData)

    {
    UCHAR ucPortData;

    //**************************************************************************
    // Read current value then update
    //**************************************************************************
        
    CRTC_RD(NV_CIO_CRE_DDC_WR__INDEX, ucPortData);

    if (ucData)
        ucPortData |= 0x10;
    else        
        ucPortData &= (~0x10);  

    //**************************************************************************
    // Set new value
    //**************************************************************************

    CRTC_WR(NV_CIO_CRE_DDC_WR__INDEX, ucPortData);   

    }

//******************************************************************************
//
//  Function:   ReadClockLine
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOLEAN ReadClockLine(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    UCHAR ucPortData;
    
    //**************************************************************************
    //  Bit 2    SCL pin (current value of the SCL pin)
    //**************************************************************************

    CRTC_RD(NV_CIO_CRE_DDC_STATUS__INDEX , ucPortData);

    return((BOOLEAN)(ucPortData&4) >> 2);
    }


//******************************************************************************
//
//  Function:   ReadDataLine
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOLEAN ReadDataLine(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    UCHAR ucPortData;
    
    //**************************************************************************
    //  Bit 3    SDA pin (current value of the SCL pin)
    //**************************************************************************

    CRTC_RD(NV_CIO_CRE_DDC_STATUS__INDEX , ucPortData);

    return((BOOLEAN)(ucPortData&8) >> 3);
    }

//******************************************************************************
//
//  Function:   WaitForVsyncActive
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID WaitForVsyncActive(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    //**************************************************************************
    // Wait while in vblank
    //**************************************************************************

    while (GetCurrentScanLine(HwDeviceExtension) ==0);

    //**************************************************************************
    // Wait while display is active
    //**************************************************************************

    while (GetCurrentScanLine(HwDeviceExtension) !=0);

    //**************************************************************************
    // Now we're in vblank
    //**************************************************************************

    }


//******************************************************************************
//
//  Function: GetCurrentScanLine
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

ULONG GetCurrentScanLine(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    ULONG   LoByte, HiByte;
    ULONG   Value;
    
    //**************************************************************************
    // Get current scanline by reading both registers
    //**************************************************************************

    CRTC_RD(NV_CIO_CRE_RL1__INDEX , HiByte);
    HiByte <<=8;
    
    CRTC_RD(NV_CIO_CRE_RL0__INDEX , LoByte);
    LoByte &=0xff;
        
    Value = ( (HiByte << 8) | (LoByte&0xff) );
    Value &= 0x7FF;
        
    if (Value == 0)
        {
        //**********************************************************************
        // Double check to fix hw bug. (for NV3)
        //**********************************************************************

        CRTC_RD(NV_CIO_CRE_RL1__INDEX , HiByte);
        HiByte <<=8;
    
        CRTC_RD(NV_CIO_CRE_RL0__INDEX , LoByte);
        LoByte &=0xff;
        
        Value = ( (HiByte << 8) | (LoByte&0xff) );
        Value &= 0x7FF;
        }


    return(Value);
    }


//******************************************************************************
//
//  Function:   GetDdcInformation
//
//  Routine Description:
//
//      Reads the basic EDID structure from the monitor using DDC2.
//
//  Arguments:
//
//      HwDeviceExtension - Points to per-adapter device extension.
//  
//      ulDisplayIndex    - Logical Display index (GDI level)
//
//      QueryBuffer       - Buffer where information will be stored.
//  
//      BufferSize        - Size of the buffer to fill.
//
//  Return Value:
//
//      Whether the call succeeded or not.
//
//******************************************************************************


BOOLEAN
GetDdcInformation(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG  ulDisplayIndex,
    PUCHAR QueryBuffer,
    ULONG  BufferSize)

    {
    ULONG ulHead, i;
    ULONG ulPrimaryHead;
    NV_DESKTOP_INFO * pDeskTop;
    ULONG ulDeviceMask = 0;
    LPEDID_UNIT pEdidUnit;
    PUCHAR      pEDIDBuffer;

    pDeskTop = &HwDeviceExtension->DeskTopInfo;

    // BUGBUG
    // We take a different code path for Dualview. We want expose two monitors, no matter what.
    // The problem of single view is that it depends on HwDeviceExtension->DeskTopInfo to determine
    // which monitor to expose.  But at this moment, HwDeviceExtension->DeskTopInfo is not initialized yet.
    // And even worse, it's subject to change on switching to different heads.
    // In future, this code should be revisited.  We should always expose all monitors to GDI.
    // Non-Dualview code path shall be removed
    if (HwDeviceExtension->pDualViewAssociatedDE)
    {
        ULONG ulConnectedDeviceMask = pDeskTop->ulConnectedDeviceMask;
        // If on LCD/CRT/TV all exist, pick CLD and CRT
        if ((ulConnectedDeviceMask & BITMASK_ALL_CRT) &&
            (ulConnectedDeviceMask & BITMASK_ALL_TV) &&
            (ulConnectedDeviceMask & BITMASK_ALL_DFP))
            ulConnectedDeviceMask &= ~BITMASK_ALL_TV;

        for (i = 0; i <= ulDisplayIndex; i++)
        {
            ulDeviceMask = LOWESTBIT(ulConnectedDeviceMask);
            ulConnectedDeviceMask &= ~ulDeviceMask;
        }
    }
    else
    {
        // This function will be called only for the case of a board with a single head.
        // So we reference the EDID info of the Head 0.
        ulHead = 0;

        //
        // Enable this code once Terry finishes the control panel work of automoatically disabling the system EDID for
        // spanning modes.
        //
        // Extract the primary head. For standard/clone/span modes, we will let the OS manage the properties
        // (refresh rate, color depth, EDID flag) of the primary head.
        // Note: Dependency: The control panel needs to automatically disable the OS EDID if the user wants to switch
        // to spanning mode, otherwise the 'faked' doubled modes will be disallowed by the OS.

        // NOTE to keep in mind: This routine gets called before any mode set has happened. So encure that the right
        // device mask is being used.

        ulPrimaryHead = pDeskTop->ulDeviceDisplay[ulDisplayIndex];
        ulDeviceMask = pDeskTop->ulDeviceMask[ulPrimaryHead];
    }

    // We are overloading the term ulHead here, so clean up later by removing the variable ulHead
    // and use only the term ulEDIDIndex;
    ulHead = ConvertDeviceMaskToIndex(ulDeviceMask); 

    pEdidUnit = HwDeviceExtension->EdidCollection->HeadMaps[ulHead];
    pEDIDBuffer = pEdidUnit->EDIDBuffer;

    //**************************************************************************
    // If a flat panel was connected, then just return the EDID buffer
    // (which was obtained in the RM). Do the same for a CRT also.
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent == TRUE)
    {

        //**********************************************************************
        // Win2k (as of build 2195) does NOT seem to understand version 2.0 EDIDs.
        // Returning a version 2.0 EDID will cause Win2k to come up in VGA.
        // So for now, if we detect version 2.0, then just return FAILURE, and
        // don't bother returning the EDID.  (Fixes Gateway FPD1500 flat panel)
        //**********************************************************************

    	if (! ((pEDIDBuffer[0] == 0)    && 
               (pEDIDBuffer[1] == 0xFF) && 
               (pEDIDBuffer[7] == 0)) )
            return(FALSE);
    }

    //
    // Return failure if this is a non-DDC device. The whole EDID structure would have been zeroed out in this case.
    // Let us check a few random fields to verify.
    // (18)    EDID Version Number
    // (19)    Revision number
    //
    if (pEDIDBuffer[0] == 0    && 
        pEDIDBuffer[1] == 0    && 
        pEDIDBuffer[7] == 0    && 
        pEDIDBuffer[18] == 0   && 
        pEDIDBuffer[19] == 0)
    {
        VideoDebugPrint((0,"This is a non-DDC monitor. So not returning any EDID\n"));     
        return(FALSE);
    }

    if (!pEdidUnit->Valid)
    {
        VideoDebugPrint((0,"This is a non-DDC monitor. So not returning any EDID\n"));     
        return(FALSE);
    }
    

    //**********************************************************************
    // Go ahead and return the EDID
    //**********************************************************************

    VideoPortMoveMemory(QueryBuffer, pEDIDBuffer, BufferSize);

    //**********************************************************************
    // Return Success.
    //**********************************************************************

    return(TRUE);

    }



#endif // _WIN32_WINNT >= 0x0500
