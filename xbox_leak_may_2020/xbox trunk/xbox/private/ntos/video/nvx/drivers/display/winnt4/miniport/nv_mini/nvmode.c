//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NVMODE.C
//
// Abstract:
//
//     This is the modeset code for the NV miniport driver.
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

// Bring this in to get the flat panel structs
#include "nvcm.h"
#include "cmdcnst.h"
#include "monitor.h"
#include "vesadata.h"
#include "nvMultiMon.h"

// modeset DLL definitions                    
#include "modeext.h"
void InitializeCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension);
ULONG GetTimingDacCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo);
extern int FindModeEntry (LPDISPDATA lpDispData,
    LPCHAR      lpEdidBuffer,
    ULONG       dwEdidSize,
    LPMODEINFO  lpModeDesc,
    LPMODEOUT   lpModeOut,
    LPMODEINFO  lpModeNew);

extern ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask);



#define NV3_BYTE_ALIGNMENT_PAD   15L
#define NV4_BYTE_ALIGNMENT_PAD   31L
#define NV10_BYTE_ALIGNMENT_PAD  63L

#define IMAGE_PITCH_LIMIT       16320


//******************************************************************************
//
// Function Declarations
//
//******************************************************************************

VP_STATUS
NVQueryAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    );

VP_STATUS
NVQueryNumberOfAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_NUM_MODES NumModes,
    ULONG NumModesSize,
    ULONG_PTR* OutputSize
    );

VP_STATUS
NVQueryCurrentMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    );

VOID
NVValidateModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

BOOL NVValidateDeviceMode(PHW_DEVICE_EXTENSION HwDeviceExtension, VALMODEXTR *vmx);

//******************************************************************************
//
// External Function Declarations
//
//******************************************************************************

extern BOOL RmConfigGetExKernel
(
    U032 deviceReference,
    U032 index,
    VOID* pParms,
    U032 parmSize
);

extern BOOL RmConfigGetKernel (U032, U032, U032*);

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE,NVQueryAvailableModes)
#pragma alloc_text(PAGE,NVQueryNumberOfAvailableModes)
#pragma alloc_text(PAGE,NVQueryCurrentMode)
#pragma alloc_text(PAGE,NVValidateModes)
#endif

MODE_ENTRY FakeModes[] =  { {1,640,480,8 ,60},
                            {1,640,480,16,60},
                            {1,640,480,32,60},
                            {1,640,480,8 ,70},
                            {1,640,480,16,70},
                            {1,640,480,32,70},
                            {1,640,480,8 ,75},
                            {1,640,480,16,75},
                            {1,640,480,32,75},

                            {1,800,600,8 ,60},
                            {1,800,600,16,60},
                            {1,800,600,32,60},
                            {1,800,600,8 ,70},
                            {1,800,600,16,70},
                            {1,800,600,32,70},
                            {1,800,600,8 ,75},
                            {1,800,600,16,75},
                            {1,800,600,32,75},

                            {1,1024,768,8 ,60},
                            {1,1024,768,16,60},
                            {1,1024,768,32,60},
                            {1,1024,768,8 ,70},
                            {1,1024,768,16,70},
                            {1,1024,768,32,70},
                            {1,1024,768,8 ,75},
                            {1,1024,768,16,75},
                            {1,1024,768,32,75} };


//******************************************************************************
//
//  Function:  NVQueryAvailableModes()
//
//  Routine Description:
//
//      This routine returns the list of all available available modes on the
//      card.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      ModeInformation - Pointer to the output buffer supplied by the user.
//          This is where the list of all valid modes is stored.
//
//      ModeInformationSize - Length of the output buffer supplied by the user.
//
//      OutputSize - Pointer to a buffer in which to return the actual size of
//          the data in the buffer. If the buffer was not large enough, this
//          contains the minimum required buffer size.
//
//  Return Value:
//
//      ERROR_INSUFFICIENT_BUFFER if the output buffer was not large enough
//          for the data being returned.
//
//      NO_ERROR if the operation completed successfully.
//
//******************************************************************************


VP_STATUS
NVQueryAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    )

{
    PVIDEO_MODE_INFORMATION videoModes = ModeInformation;
    ULONG i;
    PMODE_ENTRY ModeEntry;
    ULONG BytesPerPel;
    ULONG NumberOfModesBeforeMemRestrictions;
    
    
    //**************************************************************************
    // Find out the size of the data to be put in the buffer and return
    // that in the status information (whether or not the information is
    // there). If the buffer passed in is not large enough return an
    // appropriate error code.
    //**************************************************************************

    if (ModeInformationSize < (*OutputSize =
            HwDeviceExtension->NumAvailableModes *
            sizeof(VIDEO_MODE_INFORMATION)) ) {

        VideoDebugPrint((0,"NVQueryAvailableModes: ERROR_INSUFFICIENT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;

    }


    //**************************************************************************
    // During unattended install with an Intel 810 present,
    // we're going to return a 'fake' set of available modes.
    // We'll specify just enough modes to 'fake out' unattended install
    // to let it succeed.  We do this because we completely skipped
    // the ValidateModes() function.  We'll specify 9 modes, 3 refreshes each.
    // (640x480, 800x600, 1024x768 at 60hz,70hz,75hz)
    //**************************************************************************

    if (HwDeviceExtension->DeviceMappingFailed==TRUE)
        {

        if (ModeInformationSize < (*OutputSize = 9 * 3 * sizeof(VIDEO_MODE_INFORMATION)) ) 
            {
            VideoDebugPrint((0,"NVQueryAvailableModes: ERROR_INSUFFICIENT_BUFFER\n"));
            return ERROR_INSUFFICIENT_BUFFER;
            }


        //**************************************************************************
        // Point to fake mode table
        //**************************************************************************

        ModeEntry = (PMODE_ENTRY)&(FakeModes[0]);

        //**************************************************************************
        // Parse the fake mode table (27 modes), and return it to Win2k
        //**************************************************************************

        for (i = 0; i < 27 ; i++)

            {
            if ((ModeEntry[i].Depth == 15) || (ModeEntry[i].Depth == 16))
                BytesPerPel = 16/8;
            else
                BytesPerPel = (ModeEntry[i].Depth/8);

            videoModes->Length              = sizeof(VIDEO_MODE_INFORMATION);
            videoModes->ModeIndex           = i;
            videoModes->VisScreenWidth      = ModeEntry[i].Width;
            videoModes->ScreenStride        = ModeEntry[i].Width * BytesPerPel;
            videoModes->VisScreenHeight     = ModeEntry[i].Height;
            videoModes->NumberOfPlanes      = 1;

            if (ModeEntry[i].Depth == 15)
                videoModes->BitsPerPlane = 16;
            else
                videoModes->BitsPerPlane = ModeEntry[i].Depth;

            videoModes->Frequency           = ModeEntry[i].RefreshRate;
            videoModes->XMillimeter         = 320;  // temporary hardcoded constant
            videoModes->YMillimeter         = 240;  // temporary hardcoded constant
            videoModes->NumberRedBits       = 8;    // Do we have an 8 or 6 bit DAC?
            videoModes->NumberGreenBits     = 8;    // Do we have an 8 or 6 bit DAC?
            videoModes->NumberBlueBits      = 8;    // Do we have an 8 or 6 bit DAC?
            videoModes->AttributeFlags      = VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS;
            videoModes->AttributeFlags      |=  0;  // No interlacing today.

            videoModes->VideoMemoryBitmapWidth = ModeEntry[i].Width;
            videoModes->VideoMemoryBitmapHeight =
                   HwDeviceExtension->AdapterMemorySize / videoModes->ScreenStride;

            if (ModeEntry[i].Depth == 15)
                {
                videoModes->RedMask = 0x7c00;           // 5:5:5 format
                videoModes->GreenMask = 0x03e0;
                videoModes->BlueMask = 0x001f;
                }

            else if (ModeEntry[i].Depth == 16)
                {
                videoModes->RedMask = 0xf800;           // 5:6:5 format
                videoModes->GreenMask = 0x07e0;
                videoModes->BlueMask = 0x001f;
                }

            else if (ModeEntry[i].Depth == 32)

                {
                videoModes->RedMask =   0xff0000;
                videoModes->GreenMask = 0x00ff00;
                videoModes->BlueMask =  0x0000ff;
                }

            else

                {
                videoModes->RedMask = 0;
                videoModes->GreenMask = 0;
                videoModes->BlueMask = 0;
                videoModes->AttributeFlags |= VIDEO_MODE_PALETTE_DRIVEN |
                         VIDEO_MODE_MANAGED_PALETTE;
                }

            videoModes++;

            }

        return NO_ERROR;
        }


    //**************************************************************************
    // Get pointer to our mode data table
    //
    // NOTE: NV3 still uses the OLD style modeset code meaning:
    //
    //          1) The 'validated' modes are located in HwDeviceExtension->fbTimingTable
    //          2) The old modeset code (in modeset.c) is still being used.
    //             This code is no longer being maintained/updated.
    //             Available modes are 'fixed' , it's not as easy to
    //             to add/modify new modes using this old style modeset code.
    //
    //       NV4 (and better) now used the NEW style modeset code:
    //
    //          1) The 'validated' modes are located in HwDeviceExtension->ValidModeTable
    //          2) The mode gets set the the RM (via RmSetMode).  The RM also
    //             handles modeset code arbitration.  This is the same modeset
    //             code used by the Win9x driver, and is 'shared'.  It is much
    //             easier to add/modify new modes using this new style modeset code.
    //
    //**************************************************************************

    if (HwDeviceExtension->ulChipID  == NV3_DEVICE_NV3_ID)
        {        
        //**********************************************************************
        // NV3  (Old style modeset code)
        //**********************************************************************

        ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->fbTimingTable[0]);
        }
        
    else
        {
        //**********************************************************************
        // NV4 or better (New style modeset code)
        //**********************************************************************

        ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);
        }
        


    if (HwDeviceExtension->ulChipID  == NV3_DEVICE_NV3_ID)

        {
        //**********************************************************************
        // NV3 still uses OLD style modeset code
        //
        // 'NumVideoModes' is a fixed value, and represents the number
        // of modes that were available before eliminating modes due
        // to the amount of memory on the card.
        //**********************************************************************

        NumberOfModesBeforeMemRestrictions = NumVideoModes;
        }
        
    else

        {
        //**********************************************************************
        // NV4 uses the NEW style modeset code.
        //
        // 'NumRegistryModes' represents the number of modes available
        // before eliminating modes due to the amount of memory on the card.
        // The DIFFERENCE is that this value depends on 
        //      1) What modes are listed in VESADATA.C (in GTFTimingTable) and
        //      2) What modes were specified in the REGISTRY
        //**********************************************************************

        NumberOfModesBeforeMemRestrictions = HwDeviceExtension->NumRegistryModes;
        }
        

    //**************************************************************************
    // For each mode supported by the card, store the mode characteristics
    // in the output buffer.
    //**************************************************************************


      for (i = 0; i < NumberOfModesBeforeMemRestrictions; i++)

        {
        //**********************************************************************
        // In ValidateModes, we check to see which ones are currently
        // available, depending on the amount of RAM currently present
        // on the adapter, etc....
        //**********************************************************************

        if (ModeEntry[i].ValidMode)
            {
            //******************************************************************
            // Handle 15bpp modes same as 16bpp modes
            //******************************************************************

            if ((ModeEntry[i].Depth == 15) || (ModeEntry[i].Depth == 16))
                BytesPerPel = 16/8;
            else
                BytesPerPel = (ModeEntry[i].Depth/8);


            videoModes->Length = sizeof(VIDEO_MODE_INFORMATION);
            videoModes->ModeIndex  = i;
            videoModes->VisScreenWidth = ModeEntry[i].Width;
            videoModes->ScreenStride = ModeEntry[i].Width * BytesPerPel;
            videoModes->VisScreenHeight = ModeEntry[i].Height;
            videoModes->NumberOfPlanes = 1;

            //******************************************************************
            // NV3: Only supports 555 format for 16bpp modes.  However, we return
            // 16bits of color even though we are really running in 15bit (555 mode)
            // (Display applet thinks we have 64k colors) Microsoft said this was ok
            // due to WHQL complications related to 15bpp modes.
            //
            // NV4:  Can support 555 and 565 modes for 16bpp
            // We'll eventually just support 565 modes (64k colors).
            //******************************************************************

            if (ModeEntry[i].Depth == 15)
                videoModes->BitsPerPlane = 16;
            else
                videoModes->BitsPerPlane = ModeEntry[i].Depth;

            videoModes->Frequency = ModeEntry[i].RefreshRate;
            videoModes->XMillimeter = 320;      // temporary hardcoded constant
            videoModes->YMillimeter = 240;      // temporary hardcoded constant
            videoModes->NumberRedBits = 8;      // Do we have an 8 or 6 bit DAC?
            videoModes->NumberGreenBits = 8;    // Do we have an 8 or 6 bit DAC?
            videoModes->NumberBlueBits = 8;     // Do we have an 8 or 6 bit DAC?
            videoModes->AttributeFlags = VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS;
            videoModes->AttributeFlags |=  0;   // No interlacing today.

            //******************************************************************
            // Calculate the VideoMemoryBitmapWidth
            // Since our hardware handles variable pitch, we don't ever
            // have offscreen memory on the 'right' side.
            // BitmapWidth will always equal the VisScreenWidth.
            //******************************************************************

            videoModes->VideoMemoryBitmapWidth = ModeEntry[i].Width;

            //******************************************************************
            // NOTE: AdapterMemorySize was adjusted in NVValidateModes
            //       to compensate for INSTANCE memory in offscreen memory.
            //       That is, Hash Table, Fifo Context, and Run Out information
            //       is stored in PRAMIN memory, which also exists in offscreen memory.
            //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
            //       To make a long story short, AdapterMemorySize was adjusted by
            //       64k in NVValidateModes (The last 64k of VRAM contains Instance memory).
            //       See NVValidateModes for more comments.
            //
            //       In the display driver, ppdev->cyMemory = VideoMemoryBitmapHeight
            //       Direct Draw uses cyMemory to determine how much memory to 'Share'.
            //       Since these values are obtained from AdapterMemorySize,
            //       no additional adjustments are necessary here.
            //******************************************************************

            //******************************************************************
            // TOTAL number of vertical scanlines in video memory
            //******************************************************************

            videoModes->VideoMemoryBitmapHeight =
                   HwDeviceExtension->AdapterMemorySize / videoModes->ScreenStride;

            //******************************************************************
            // Color masks (specify 5:5:5 format for 16bpp on NV3 only)
            // NV4 supports 5:5:5 as well as 5:6:5
            //******************************************************************

            if (ModeEntry[i].Depth == 15)
                {
                videoModes->RedMask = 0x7c00;           // 5:5:5 format
                videoModes->GreenMask = 0x03e0;
                videoModes->BlueMask = 0x001f;
                }

            else if (ModeEntry[i].Depth == 16)
                {
                videoModes->RedMask = 0xf800;           // 5:6:5 format
                videoModes->GreenMask = 0x07e0;
                videoModes->BlueMask = 0x001f;
                }

            else if (ModeEntry[i].Depth == 24)

                {
                videoModes->RedMask =   0xff0000;
                videoModes->GreenMask = 0x00ff00;
                videoModes->BlueMask =  0x0000ff;
                }

            else if (ModeEntry[i].Depth == 32)

                {
                videoModes->RedMask =   0xff0000;
                videoModes->GreenMask = 0x00ff00;
                videoModes->BlueMask =  0x0000ff;
                }

            else

                {
                videoModes->RedMask = 0;
                videoModes->GreenMask = 0;
                videoModes->BlueMask = 0;
                videoModes->AttributeFlags |= VIDEO_MODE_PALETTE_DRIVEN |
                         VIDEO_MODE_MANAGED_PALETTE;
                }

            videoModes++;

            }

        }

        return NO_ERROR;

    } // end NVQueryAvailableModes()


//******************************************************************************
//
//  Function:   NVQueryNumberOfAvailableModes()
//
//  Routine Description:
//
//      This routine returns the number of available modes for this particular
//      video card.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      NumModes - Pointer to the output buffer supplied by the user. This is
//          where the number of modes is stored.
//
//      NumModesSize - Length of the output buffer supplied by the user.
//
//      OutputSize - Pointer to a buffer in which to return the actual size of
//          the data in the buffer.
//
//  Return Value:
//
//      ERROR_INSUFFICIENT_BUFFER if the output buffer was not large enough
//          for the data being returned.
//
//      NO_ERROR if the operation completed successfully.
//
//******************************************************************************

VP_STATUS NVQueryNumberOfAvailableModes(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_NUM_MODES NumModes,
    ULONG NumModesSize,
    ULONG_PTR* OutputSize
    )

    {
        
    

    //**************************************************************************
    // Find out the size of the data to be put in the the buffer and return
    // that in the status information (whether or not the information is
    // there). If the buffer passed in is not large enough return an
    // appropriate error code.
    //**************************************************************************

    if (NumModesSize < (*OutputSize = sizeof(VIDEO_NUM_MODES)) ) {

        return ERROR_INSUFFICIENT_BUFFER;

    }

    //**************************************************************************
    // During unattended install with an Intel 810 present,
    // we're going to return a 'fake' set of available modes.
    // We'll specify just enough modes to 'fake out' unattended install
    // to let it succeed.  We do this because we completely skipped
    // the ValidateModes() function.  We'll specify 9 modes, 3 refreshes each.
    // (640x480, 800x600, 1024x768 at 60hz,70hz,75hz) for a total of 27 modes
    //**************************************************************************


    if (HwDeviceExtension->DeviceMappingFailed==TRUE)
        {
        NumModes->NumModes = 27;
        NumModes->ModeInformationLength = sizeof(VIDEO_MODE_INFORMATION);

        VideoDebugPrint((0,"Fake NumAvailableModes = %d\n", 27));
        return NO_ERROR;
        }


    //**************************************************************************
    // Store the number of modes into the buffer.
    // This number is determined in NVValidateModes, which gets called
    // by the function NV1IsPresent (which gets called by NVFindAdapter)
    //**************************************************************************

    NumModes->NumModes = HwDeviceExtension->NumAvailableModes;
    NumModes->ModeInformationLength = sizeof(VIDEO_MODE_INFORMATION);

    VideoDebugPrint((0,"NumAvailableModes = %d\n", HwDeviceExtension->NumAvailableModes));
    return NO_ERROR;

} // end VgaGetNumberOfAvailableModes()


//******************************************************************************
//
// Function: NVQueryCurrentMode()
//
// Routine Description:
//
//     This routine returns a description of the current video mode.
//
// Arguments:
//
//     HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//     ModeInformation - Pointer to the output buffer supplied by the user.
//         This is where the current mode information is stored.
//
//     ModeInformationSize - Length of the output buffer supplied by the user.
//
//     OutputSize - Pointer to a buffer in which to return the actual size of
//         the data in the buffer. If the buffer was not large enough, this
//         contains the minimum required buffer size.
//
// Return Value:
//
//     ERROR_INSUFFICIENT_BUFFER if the output buffer was not large enough
//         for the data being returned.
//
//     NO_ERROR if the operation completed successfully.
//
//******************************************************************************

VP_STATUS
NVQueryCurrentMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE_INFORMATION ModeInformation,
    ULONG ModeInformationSize,
    ULONG_PTR* OutputSize
    )


    {
    ULONG BytesPerPel;


    //**************************************************************************
    // Find out the size of the data to be put in the the buffer and return
    // that in the status information (whether or not the information is
    // there). If the buffer passed in is not large enough return an
    // appropriate error code.
    //**************************************************************************

    if (ModeInformationSize < (*OutputSize = sizeof(VIDEO_MODE_INFORMATION)))
        {
        return ERROR_INSUFFICIENT_BUFFER;
        }

    //**************************************************************************
    // Return which chip is present (NV1 or NV3) in the
    // DriverSpecificAttributeFlags field. ulChipID gets determined
    // during NVFindAdapter()
    //
    // For NV5,NV5ULTRA, NV5VANTA, NV0A,NV10 we'll set the NV4 bit to also signify that
    // these chips are also NV4 COMPATIBLE'
    //**************************************************************************

    if (HwDeviceExtension->ulChipID == NV1_DEVICE_NV1_ID)
        ModeInformation->DriverSpecificAttributeFlags = CAPS_NV1_IS_PRESENT;
    else if (HwDeviceExtension->ulChipID == NV3_DEVICE_NV3_ID)
        ModeInformation->DriverSpecificAttributeFlags = CAPS_NV3_IS_PRESENT;
    else if (HwDeviceExtension->ulChipID == NV4_DEVICE_NV4_ID)
        ModeInformation->DriverSpecificAttributeFlags = CAPS_NV4_IS_PRESENT;

    //**************************************************************************
    // For NV5, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV5_DEVICE_NV5_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV5_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV5ULTRA, set the NV5ULTRA bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV5ULTRA_DEVICE_NV5ULTRA_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV5ULTRA_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV5VANTA, set the NV5VANTA bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV5VANTA_DEVICE_NV5VANTA_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV5VANTA_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV5MODEL64, set the NV5MODEL64 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV5MODEL64_DEVICE_NV5MODEL64_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV5MODEL64_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV0A, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV0A_DEVICE_NV0A_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV0A_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV10, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV10_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV10DDR, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV10DDR_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV10GL, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV10GL_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV11, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV11_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV11DDR, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV11DDR_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV11M, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV11M_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV11GL, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV11GL_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV15, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV15_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV15DDR, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV15DDR_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV15BR, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV15BR_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV15GL, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV15GL_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;


    //**************************************************************************
    // For NV20, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV20_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV20_1, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV20_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV20_2, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV20_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    //**************************************************************************
    // For NV20_3, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_NV20_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

    else
        ModeInformation->DriverSpecificAttributeFlags = 0;

    //**************************************************************************
    // Handle 15bpp modes same as 16bpp modes
    //**************************************************************************

    if (HwDeviceExtension->CurrentMode->Depth == 15)
        BytesPerPel = 16/8;
    else
        BytesPerPel = (HwDeviceExtension->CurrentMode->Depth/8);


    //**************************************************************************
    // Store the characteristics of the current mode into the buffer.
    //**************************************************************************

    ModeInformation->Length = sizeof(VIDEO_MODE_INFORMATION);

    ModeInformation->ModeIndex = HwDeviceExtension->ModeIndex;
    if(HwDeviceExtension->DeskTopInfo.ulDesktopMode & NV_ENABLE_VIRTUAL_DESKTOP)
    {
        ModeInformation->VisScreenWidth =  HwDeviceExtension->DeskTopInfo.ulDesktopWidth; 
        ModeInformation->VisScreenHeight = HwDeviceExtension->DeskTopInfo.ulDesktopHeight;
    }
    else
    {
        ModeInformation->VisScreenWidth = HwDeviceExtension->CurrentMode->Width;
        ModeInformation->VisScreenHeight = HwDeviceExtension->CurrentMode->Height;
    }

    ModeInformation->ScreenStride = ModeInformation->VisScreenWidth * BytesPerPel;

    //**************************************************************************
    // Add alignment for all chips
    //**************************************************************************
    if (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV3_IS_PRESENT)
    {
        ModeInformation->ScreenStride += NV3_BYTE_ALIGNMENT_PAD;
        ModeInformation->ScreenStride &= ~NV3_BYTE_ALIGNMENT_PAD;
    }
    else if ((ModeInformation->DriverSpecificAttributeFlags & CAPS_NV10_IS_PRESENT) ||      
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV10DDR_IS_PRESENT) || 
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV10GL_IS_PRESENT)  ||
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV11_IS_PRESENT)    ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV11DDR_IS_PRESENT) ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV11M_IS_PRESENT)   ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV11GL_IS_PRESENT)  ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV15_IS_PRESENT)    ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV15DDR_IS_PRESENT) ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV15BR_IS_PRESENT)  ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV15GL_IS_PRESENT)  ||   
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_NV20_IS_PRESENT) )
    {
        ModeInformation->ScreenStride += NV10_BYTE_ALIGNMENT_PAD;
        ModeInformation->ScreenStride &= ~NV10_BYTE_ALIGNMENT_PAD;
    }
    else        
    {
        ModeInformation->ScreenStride += NV4_BYTE_ALIGNMENT_PAD;
        ModeInformation->ScreenStride &= ~NV4_BYTE_ALIGNMENT_PAD;
    }

    ModeInformation->NumberOfPlanes = 1;

    //**************************************************************************
    // NV3: Only supports 555 format for 16bpp modes.  However, we return
    // 16bits of color even though we are really running in 15bit (555 mode)
    // (Display applet thinks we have 64k colors) Microsoft said this was ok
    // due to WHQL complications related to 15bpp modes.
    //
    // NV4:  Can support 555 and 565 modes for 16bpp
    // We'll eventually just support 565 modes (64k colors).
    //**************************************************************************

    if (HwDeviceExtension->CurrentMode->Depth == 15)
        ModeInformation->BitsPerPlane  = 16;
    else
        ModeInformation->BitsPerPlane = HwDeviceExtension->CurrentMode->Depth;


    ModeInformation->Frequency = HwDeviceExtension->CurrentMode->RefreshRate;

    ModeInformation->XMillimeter = 320;        // temporary hardcoded constant
    ModeInformation->YMillimeter = 240;        // temporary hardcoded constant
    ModeInformation->NumberRedBits = 8;
    ModeInformation->NumberGreenBits = 8;
    ModeInformation->NumberBlueBits = 8;
    ModeInformation->AttributeFlags = VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS;

   if (HwDeviceExtension->CurrentMode->Depth == 15)
       {
       ModeInformation->RedMask = 0x7c00;
       ModeInformation->GreenMask = 0x03e0;
       ModeInformation->BlueMask = 0x001f;
       }

   else if (HwDeviceExtension->CurrentMode->Depth == 16)
       {
       ModeInformation->RedMask = 0xf800;
       ModeInformation->GreenMask = 0x07e0;
       ModeInformation->BlueMask = 0x001f;
       }

   else if (HwDeviceExtension->CurrentMode->Depth == 24)

       {
       ModeInformation->RedMask =   0xff0000;
       ModeInformation->GreenMask = 0x00ff00;
       ModeInformation->BlueMask =  0x0000ff;
       }

   else if (HwDeviceExtension->CurrentMode->Depth == 32)

       {
       ModeInformation->RedMask =   0xff0000;
       ModeInformation->GreenMask = 0x00ff00;
       ModeInformation->BlueMask =  0x0000ff;
       }

   else        // 8bpp palette driven
       {
       ModeInformation->RedMask = 0;
       ModeInformation->GreenMask = 0;
       ModeInformation->BlueMask = 0;
       ModeInformation->AttributeFlags |= VIDEO_MODE_PALETTE_DRIVEN |
                VIDEO_MODE_MANAGED_PALETTE;
       }

   //**************************************************************************
   // Calculate the VideoMemoryBitmapWidth
   // Since our hardware handles variable pitch, we don't ever
   // have offscreen memory on the 'right' side.
   // BitmapWidth will always equal the VisScreenWidth.
   //**************************************************************************

    ModeInformation->VideoMemoryBitmapWidth = ModeInformation->VisScreenWidth;

   //**************************************************************************
   // NOTE: AdapterMemorySize was adjusted in NVValidateModes
   //       to compensate for INSTANCE memory in offscreen memory.
   //       That is, Hash Table, Fifo Context, and Run Out information
   //       is stored in PRAMIN memory, which also exists in offscreen memory.
   //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
   //       To make a long story short, AdapterMemorySize was adjusted by
   //       64k in NVValidateModes (The last 64k of VRAM contains Instance memory).
   //       See NVValidateModes for more comments.
   //
   //       In the display driver (in Enable), ppdev->cyMemory = VideoMemoryBitmapHeight
   //       Direct Draw uses cyMemory to determine how much memory to 'Share'.
   //       Since these values are obtained from AdapterMemorySize,
   //       no additional adjustments are necessary here.
   //**************************************************************************

    ModeInformation->VideoMemoryBitmapHeight =
             HwDeviceExtension->AdapterMemorySize / ModeInformation->ScreenStride;

    if(HwDeviceExtension->ulForceSwCursor)
        ModeInformation->DriverSpecificAttributeFlags |= CAPS_SW_POINTER;

    return NO_ERROR;

    } // end NVQueryCurrentMode()


//******************************************************************************
//
//  Function:   NVValidateModes()
//
//  Routine Description:
//
//      Determines which modes are valid and which are not.
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

VOID NVValidateModes(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {

#ifdef ENABLE_COMMON_MODESET_CODE
    PHWINFO NVInfo;
    HwDeviceExtension->NumAvailableModes = HwDeviceExtension->NumRegistryModes;
    HwDeviceExtension->NumAvailableModes1 = HwDeviceExtension->NumRegistryModes;
    
    //**********************************************************************
    // Get the amount of video memory on this card
    //**********************************************************************
    NVInfo = (PHWINFO)&(HwDeviceExtension->NvInfo);
    HwDeviceExtension->AdapterMemorySize = NVInfo->Framebuffer.RamSize - 0x10000;
#else
    // Delete the rest of this function later.

    ULONG i;
    PMODE_ENTRY SrcModeEntry;
    PMODE_ENTRY DstModeEntry,ModeEntry;
    PHWINFO NVInfo;
    ULONG BytesPerPel;
    ULONG PreviousWidth;
    ULONG PreviousHeight;
    ULONG PreviousDepth;
    ULONG PreviousRefresh;
    ULONG mode_count;
    ULONG tablesize;
    BOOL FoundDell1501FP = FALSE;
    BOOL FoundSGIFlatPanel = FALSE;
    ULONG maxPixelClockFrequency_8bpp;
    ULONG maxPixelClockFrequency_16bpp;
    ULONG maxPixelClockFrequency_32bpp;
    ULONG ThisModeFrequency;
    MODE_TIMING_VALUES timingInfo;
    ULONG vesaStatus;

    
    

    
    //**************************************************************************
    // NV3 still uses the OLD style modeset code.
    // Leave it alone and intact.
    //**************************************************************************

    if (HwDeviceExtension->ulChipID == NV3_DEVICE_NV3_ID)

        {

        //**********************************************************************
        // Initialize the instance of the fbTimingtable in the HW DEVICE EXTENSION !
        // We'll copy the 'global' version to the version in the hw device extension.
        // (The global version will remain untouched, the hw dev version gets modified)
        //**********************************************************************

        SrcModeEntry = (PMODE_ENTRY)&(fbTimingTable[0][0][0]);
        DstModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->fbTimingTable[0]);

        for (mode_count = 0 ; mode_count < NumVideoModes; mode_count++)
            {
            DstModeEntry->ValidMode   = SrcModeEntry->ValidMode;
            DstModeEntry->Width       = SrcModeEntry->Width;
            DstModeEntry->Height      = SrcModeEntry->Height;
            DstModeEntry->Depth       = SrcModeEntry->Depth;
            DstModeEntry->RefreshRate = SrcModeEntry->RefreshRate;
    
            SrcModeEntry++;
            DstModeEntry++;
                
        }

                    
        //**********************************************************************
        // Default to zero modes available and init pointers to
        // mode tables and NVInfo structure
        //**********************************************************************

        HwDeviceExtension->NumAvailableModes = 0;
        ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->fbTimingTable[0]);
        NVInfo = (PHWINFO)&(HwDeviceExtension->NvInfo);

        VideoDebugPrint((2, "NumVideoModes(%d)\n",NumVideoModes));

        //**********************************************************************
        // Init the Adapter Memory Size
        //
        // TO DO: This code is NV1 specific !!! Move to NV1.C...
        //
        // IMPORTANT:  We need to compensate for PRAMIN INSTANCE memory which exists
        //       in OFFSCREEN video memory.  PRAMIN INSTANCE memory consists of Hash Table,
        //       Fifo Context, and Run Out information, among other things.
        //
        //       We must make sure NO ONE (but the miniport) touches this offscreen memory.
        //       The layout of PRAMIN memory depends on whether PFB_CONFIG_0_SECOND_BUFFER bit
        //       is enabled or not. ENABLING the second buffer will cause PRAMIN memory to
        //       be mapped in 2 places as follows.  (See the NV technical refrence manuals for more info).
        //
        //                    Example of a 2Mb card (2nd Buffer enabled)
        //                    ------------------------------------------
        //
        //                  (Physical Addresses)
        //                  Base of NV Adapter (0 Mb) -> -------
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                  Dumb Frame Buffer (16Mb) --> --DFB--  <------\
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |--- 2Mbytes
        //                                               -------         |
        //                                              |       | <------|--- Instance Memory
        //                                    (17Mb) --> -------         |
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |
        //                                               -------         |
        //                                              |       | <------|--- Instance Memory
        //                                    (18Mb) --> -------  <------/
        //
        //
        //       For API's like Direct X, offscreen management would be complicated
        //       So,to make offscreen management easier, we will DISABLE the second buffer.
        //       This will cause memory layout to look like the following:
        //
        //                    Example of a 2Mb card (2nd Buffer enabled)
        //                    ------------------------------------------
        //
        //                  (Physical Addresses)
        //                  Base of NV Adapter (0 Mb) -> -------
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                                              |       |
        //                  Dumb Frame Buffer (16Mb) --> --DFB--  <------\
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |
        //                                    (17Mb) --> -------         |-- 2 Mbytes
        //                                              |       |        |
        //                                              |       |        |
        //                                              |       |        |
        //                                               -------         |
        //                                              |       | <------|--- Instance Memory
        //                                    (18Mb) --> -------  <------/
        //
        //
        //       As a result, Instance memory will always be located at the end of VRAM.
        //
        //       Enabling the 2nd buffer bit, is used more by a RESOURCE MANAGER
        //       and the Video Switch class.  For a normal NV app which uses an RM,
        //       using the VideoSwitch Class will cause buffers to flip between
        //       address = 0 and address = Half of VRAM size.  (The 2nd buffer
        //       is essentially at a fixed location).  Since we are NOT using
        //       a Resource manager, and since we are NOT using the VideoSwitch class,
        //       we can disable this bit, and make the NV1 card behave more like
        //       a normal graphics adapter.  We will flip buffers MANUALLY by modifying
        //       the PFB_START_ADDRESS register ourselves.  See DDRAW.C for more info.
        //
        //       In our miniport, we set PRAM_CONFIG_0_SIZE = 1 (20k).
        //       8k for HASH Table, 4k for RUNOUT, 4k for FIFO CONTEXT,
        //       3k for AUDIO (not used), and 1k for PASSWORD (not used).
        //       To make life easier, we'll always skip the last 64k of VRAM memory.
        //
        // NV3:  Additional instance memory is used (such as DMA instances to point
        //       to texture memory for D3DZ0 objects).  But still, we never use
        //       more than 64k.  Refer to NV3GetNVInfo for more information.
        //
        //       End result:
        //
        //              AdapterMemory Size -= 64k;
        //
        //**********************************************************************
    
        HwDeviceExtension->AdapterMemorySize = NVInfo->Framebuffer.RamSize - 0x10000;
    
        //**********************************************************************
        // Variables used to check for Redundant modes
        //**********************************************************************
    
        PreviousWidth = 0;
        PreviousHeight = 0;
        PreviousDepth = 0;
        PreviousRefresh = 0;
    
        //**********************************************************************
        // Now parse the modes to see which ones we can use
        //**********************************************************************
    
        for (i = 0; i < NumVideoModes; i++)
    
            {
            //******************************************************************
            // Handle 15bpp modes same as 16bpp modes (sizewise they're the same)
            //******************************************************************
    
            if ((ModeEntry[i].Depth == 15) || (ModeEntry[i].Depth == 16))
                BytesPerPel = 16/8;
            else
                BytesPerPel = (ModeEntry[i].Depth/8);
    
            //******************************************************************
            // Make the modes that fit in video memory valid. This code assumes that
            // any NV3 device with more than 4M of memory is an NV3T. We need to know
            // if this is an NV3 since 8bpp modes with xres > 1600 will fit in 4M
            // of VRAM, but the NV3 BIOS does not support these modes. Thus we must
            // invalidate these modes for NV3.
            //******************************************************************
            if ((HwDeviceExtension->AdapterMemorySize >=
                        ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel ) &&
                        !(ModeEntry[i].Width > 1600 && NVInfo->Framebuffer.RamSizeMb < 8)
#ifndef SUPPORT_320X400
                        && !(ModeEntry[i].Width == 320 && ModeEntry[i].Height == 400)
#endif //#ifdef SUPPORT_320X400
#if _WIN32_WINNT < 0x0500
    //**************************************************************************
    // This conditional was added for Dell. They want to exclude all modes
    // >= 85Hz as well as all 72Hz modes. If the registry switch
    // "DisableExtRefresh" == 1, we don't report support for
    // any of these modes.
    //
    // NOTE: If the DellSwitch1 flag is enabled, then we DO want to enable
    //       the 1920x1200 modes!  (this is getting confusing).  If so,
    //       then the >85Hz modes, 70, and 72 modes will be removed further below, NOT here.
    //**************************************************************************
                        && (!HwDeviceExtension->bExtRefreshDisable || HwDeviceExtension->bDellSwitch1 ||
                            (ModeEntry[i].RefreshRate <= 85 &&
                                ModeEntry[i].RefreshRate != 72 &&
                                ModeEntry[i].RefreshRate != 70 &&
                                ModeEntry[i].Width <= 1600))
#endif // #if _WIN32_WINNT < 0x0500
                        )
                {
                //**************************************************************
                // Check for Redundant mode entries in fbTimingTable
                //**************************************************************

                if (! ((ModeEntry[i].Width       == PreviousWidth) &&
                       (ModeEntry[i].Height      == PreviousHeight) &&
                       (ModeEntry[i].Depth       == PreviousDepth) &&
                       (ModeEntry[i].RefreshRate == PreviousRefresh)))
                    {
    
                    ModeEntry[i].ValidMode = TRUE;
                    HwDeviceExtension->NumAvailableModes++;
                    VideoDebugPrint((2,"mode[%d] valid\n",i));
                    VideoDebugPrint((2,"         hres(%d)\n",ModeEntry[i].Width));
                    VideoDebugPrint((2,"         bitsPerPlane(%d)\n",ModeEntry[i].Depth));
                    VideoDebugPrint((2,"         freq(%d)\n",ModeEntry[i].RefreshRate));
                    VideoDebugPrint((2,"         bytes needed(%d)\n",ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel));
                    VideoDebugPrint((2,"         memory pres(%d)\n",HwDeviceExtension->AdapterMemorySize));
    
                    //**********************************************************
                    // Save the last mode which was valid
                    //**********************************************************
    
                    PreviousWidth   = ModeEntry[i].Width;
                    PreviousHeight  = ModeEntry[i].Height;
                    PreviousDepth   = ModeEntry[i].Depth;
                    PreviousRefresh = ModeEntry[i].RefreshRate;
    
                    }
                else
    
                    //**********************************************************
                    // Remove Redundant mode entry
                    //**********************************************************
                    {
                    ModeEntry[i].ValidMode = FALSE;
                    }
    
    
                }
            else
    
                //**************************************************************
                // Mode not available on this adapter
                //**************************************************************
                {
                ModeEntry[i].ValidMode = FALSE;
                }
    
            }
    
        //**********************************************************************
        // Invalidate all modes except 640 X 480 for TV
        //**********************************************************************
    
        if (NVInfo->Framebuffer.MonitorType != NV_MONITOR_VGA)
            {
            for (i = 0; i < NumVideoModes; i++)
                {
                if (ModeEntry[i].ValidMode)
                    {
                    if ( (ModeEntry[i].Height != 480) )
                        {
                            ModeEntry[i].ValidMode = FALSE;
                            HwDeviceExtension->NumAvailableModes--;
                            VideoDebugPrint((0,"mode[%d] not valid for TV\n",i));
                        }
                    else
                        {
                            VideoDebugPrint((0,"mode[%d] valid for TV\n",i));
                        }
                    }
                }
            }
    
        //**********************************************************************
        // For now, we are disabling 960x720 modes due to the reduced amount of PRAMIN.
        // Having the reduced PRAMIN causes difficulty with OGL/DDRAW, so we
        // won't support it in this current release.
        //**********************************************************************
    
        for (i = 0; i < NumVideoModes; i++)
    
            {
            if (ModeEntry[i].ValidMode)
    
                {
    
                if(HwDeviceExtension->bMax16x12 &&
                    (ModeEntry[i].Width * ModeEntry[i].Height)  > (1600 * 1200) )
                    {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        VideoDebugPrint((0,"%dx%d mode[%d] disabled\n",
                            ModeEntry[i].Width,
                            ModeEntry[i].Height,
                            i));
                    }
    
    
                if ( (ModeEntry[i].Width == 960)  &&  (ModeEntry[i].Height == 720) )
    
                    {
                    ModeEntry[i].ValidMode = FALSE;
                    HwDeviceExtension->NumAvailableModes--;
                    VideoDebugPrint((0,"960x720 mode[%d] disabled\n",i));
                    }
    
                }
    
            }
    
    
    
            //******************************************************************
            // Disable 565 modes and 24bpp modes because NV3 doesn't support it.
            // NV4 supports both 555 and 565, but we'll just support 565 for NV4
            //******************************************************************
    
            for (i = 0; i < NumVideoModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    if ( ( ModeEntry[i].Depth == 16 ) || ( ModeEntry[i].Depth == 24 ))
    
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        }
    
                    }
    
                }
    
    
            //******************************************************************
            // Disable 1600x1024 resolutions for NV3 (because they're only implemented in NV4)
            //******************************************************************
    
            for (i = 0; i < NumVideoModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    if ( (ModeEntry[i].Width == 1600 ) && (ModeEntry[i].Height == 1024)  )
    
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        }
    
                    }
    
                }
    
    
    
    
    
            //******************************************************************
            // Disable 1920x1200 modes at 32bpp, due to bandwidth issues
            //******************************************************************
    
            for (i = 0; i < NumVideoModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    if ( (ModeEntry[i].Width == 1920)  &&  (ModeEntry[i].Height == 1200) &&
                         (ModeEntry[i].Depth == 32) )
    
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        }
    
                    }
    
                }
    
            //******************************************************************
            // Disable 1920x1080 modes at 32bpp, due to bandwidth issues
            //******************************************************************
    
            for (i = 0; i < NumVideoModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    if ( (ModeEntry[i].Width == 1920)  &&  (ModeEntry[i].Height == 1080) &&
                         (ModeEntry[i].Depth == 32) )
    
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        }
    
                    }
    
                }
    
    
            //******************************************************************
            // Disable 1800x1440 modes at 8 and 32bpp
            //******************************************************************
    
            for (i = 0; i < NumVideoModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    if ( (ModeEntry[i].Width == 1800)  &&  (ModeEntry[i].Height == 1440) &&
                         ( (ModeEntry[i].Depth == 32) || (ModeEntry[i].Depth == 8)))
    
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        HwDeviceExtension->NumAvailableModes--;
                        }
    
                    }
    
                }
    
    
            //******************************************************************
            // Disable 1600x1200 modes at 32bpp for SDRAM cards only
            //******************************************************************
    
    
            if (NVInfo->Framebuffer.RamType == NV_BUFFER_SDRAM)
    
                {
                for (i = 0; i < NumVideoModes; i++)
    
                    {
                    if (ModeEntry[i].ValidMode)
    
                        {
                        if ( (ModeEntry[i].Width == 1600)  &&  (ModeEntry[i].Height == 1200) && (ModeEntry[i].Depth == 32) )
    
                            {
                            ModeEntry[i].ValidMode = FALSE;
                            HwDeviceExtension->NumAvailableModes--;
                            }
    
                        }
    
                    }
                }
    
    
        //**********************************************************************
        // Disable refresh rates > 75 Hz for scan doubled modes
        //**********************************************************************
    
        for (i = 0; i < NumVideoModes; i++)
    
            {
            if (ModeEntry[i].ValidMode)
    
                {
                if ((ModeEntry[i].Height == 384 || ModeEntry[i].Height  == 360 ||
                     ModeEntry[i].Height == 300 || ModeEntry[i].Height == 240 ||
                     ModeEntry[i].Height == 200) &&
                     ModeEntry[i].RefreshRate > 75)
                    {
                    ModeEntry[i].ValidMode = FALSE;
                    HwDeviceExtension->NumAvailableModes--;
                    }
    
                }
    
            }
    
    
        }
    
    
    //**********************************************************************
    // NV4 or better uses the NEW style modeset code
    //
    //**********************************************************************

    else
        {
        

        ULONG NumAvailableModesTmp;
        ULONG ulHead;

        //
        // Copy the modetable to the second head's mode table also.
        // This table at this point is just the list of modes read from the registry.
        // It has not been trimmed yet.
        //
        if (HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected > 1)
        {
            PMODE_ENTRY SrcPtr, DestPtr;
            SrcPtr = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);
            DestPtr = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable1[0]);
            for (i=0; i < HwDeviceExtension->NumRegistryModes; i++)
            {
                *DestPtr++ = *SrcPtr++;
            }
        }
    

        for (ulHead=0; ulHead < HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard; ulHead++) 
        {
        FoundDell1501FP = FALSE;
        FoundSGIFlatPanel = FALSE;

        //**********************************************************************
        // Limit the modes according to how much video memory is available
        //**********************************************************************
        if (ulHead == 0) 
        {
            ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);
        }
        else
        {
            ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable1[0]);
        }

        NVInfo = (PHWINFO)&(HwDeviceExtension->NvInfo);

        //**********************************************************************
        // NumRegistryModes = Number of modes that were specified from registry/vesadata
        // NumAvailableModes = Number of modes that are allowed according to
        //                     how much video memory is on this card
        //**********************************************************************
    
        NumAvailableModesTmp = 0;

        //**********************************************************************
        // Get the amount of video memory on this card
        //**********************************************************************
    
        HwDeviceExtension->AdapterMemorySize = NVInfo->Framebuffer.RamSize - 0x10000;
    
        //**********************************************************************
        // Now parse the modes to see which ones we can use
        //**********************************************************************
    
        for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
            {

            //******************************************************************
            // Handle 15bpp modes same as 16bpp modes (sizewise they're the same)
            //******************************************************************
    
            if ((ModeEntry[i].Depth == 15) || (ModeEntry[i].Depth == 16))
                BytesPerPel = 16/8;
            else
                BytesPerPel = (ModeEntry[i].Depth/8);
    

            //******************************************************************
            // Make the modes that fit in video memory valid. 
            //******************************************************************
    
            if (HwDeviceExtension->AdapterMemorySize >=
                        ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel )
                        
                {
                //**************************************************************
                // Set the mode as available if it fits in memory
                //**************************************************************
    
                ModeEntry[i].ValidMode = TRUE;
                NumAvailableModesTmp++;
    
                VideoDebugPrint((2,"mode[%d] valid\n",i));
                VideoDebugPrint((2,"         width(%d)\n",ModeEntry[i].Width));
                VideoDebugPrint((2,"         height(%d)\n",ModeEntry[i].Height));
                VideoDebugPrint((2,"         depth(%d)\n",ModeEntry[i].Depth));
                VideoDebugPrint((2,"         refresh(%d)\n",ModeEntry[i].RefreshRate));
                VideoDebugPrint((2,"         bytes needed(%d)\n",ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel));
                VideoDebugPrint((2,"         memory pres(%d)\n",HwDeviceExtension->AdapterMemorySize));
    
                }
            else
    
                //**************************************************************
                // Mode not available on this adapter
                //**************************************************************
                {
                ModeEntry[i].ValidMode = FALSE;
                }
            }
    
        for (i = 0; i < HwDeviceExtension->NumRegistryModes ; i++)
            {
                NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS nvPitch;

                nvPitch.Width = ModeEntry[i].Width;
                nvPitch.Height = ModeEntry[i].Height;
                nvPitch.Depth = ModeEntry[i].Depth;

                RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_SURFACE_DIMENSIONS, &nvPitch, sizeof(NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS));
            
                if(nvPitch.Pitch > IMAGE_PITCH_LIMIT)
                {
                    ModeEntry[i].ValidMode = FALSE;
                    NumAvailableModesTmp--;
                    VideoDebugPrint((0,"%dx%d mode[%d] disabled\n",
                        ModeEntry[i].Width,
                        ModeEntry[i].Height,
                        i));
                }
            }

        //**********************************************************************
        // Disable all 1920 true color modes when running on an 8Mb card.
        // There IS enough video memory to support 1920x1080 true color with 94k free.
        // However, the Resource Manager needs at least 128k to function.
        // Adjusting HwDeviceExtension->AdapterMemorySize up above with more than 64k
        // is riskier. So a safer fix for now is to just remove these modes on an 8Mb card.
        //**********************************************************************
  
  
        if (NVInfo->Framebuffer.RamSize == 0x800000)

            {
            for (i = 0; i < HwDeviceExtension->NumRegistryModes ; i++)
  
                {
                if (ModeEntry[i].ValidMode)
  
                    {
                    if ( (ModeEntry[i].Width == 1920) && ( ModeEntry[i].Depth == 32 ))
  
                        {
                        ModeEntry[i].ValidMode = FALSE;
                        NumAvailableModesTmp--;
                        }
  
                    }
  
                }

            }


        {
        ULONG MonitorNameOffset;

        // Look at EDID Detailed block 3 (starts at offset 0x36)
        // See if can find DELL 1501FP flat panel monitor
        MonitorNameOffset = 0x36+ (18*2) + 5;

        if ( (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset  ]  == 'D') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+1]  == 'E') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+2]  == 'L') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+3]  == 'L') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+4]  == ' ') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+5]  == '1') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+6]  == '5') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+7]  == '0') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+8]  == '1') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+9]  == 'F') &&
             (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+10] == 'P'))
            {
            FoundDell1501FP = TRUE;
            }

        }

        //**********************************************************************
        // Check if this is the SGI Flat panel (1600x1024) (Examine vendor ID)
        //  EDID_V1_VERSION_NUMBER_INDEX    =   0x12
        //  EDID_V1_VENDOR_ID_INDEX         =   0x08
        //**********************************************************************

        if ( (HwDeviceExtension->EDIDBuffer[ulHead][0x12]==1) &&
             (HwDeviceExtension->EDIDBuffer[ulHead][0x08]==0x4c) &&
             (HwDeviceExtension->EDIDBuffer[ulHead][0x08+0x1]==0xf8)   )
            {
            FoundSGIFlatPanel = TRUE;
            }



        //**********************************************************************
        // Limit resolutions on flat panel to 1280x1024
        //**********************************************************************

        if ((HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected > 1 && HwDeviceExtension->DeskTopInfo.ulDeviceType[ulHead] == MONITOR_TYPE_FLAT_PANEL) ||
            (HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected == 1 && HwDeviceExtension->FlatPanelIsPresent == TRUE))


            {
            //******************************************************************
            // Very special case -  the SGI flat panel.
            // Just allow 6x4, 8x6, 10x7, 12x9, 12x10, and 16x10 at 60hz
            // regardless of what the INF entries are set to.
            // (Because for now we don't want to allow 1600x1024 modes with other displays)
            //******************************************************************

            if (FoundSGIFlatPanel==TRUE)
                {

                //**************************************************************
                // Disallow all modes...
                //**************************************************************

                NumAvailableModesTmp=0;

                for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
                    {
                    ModeEntry[i].ValidMode      = FALSE;
                    ModeEntry[i].Width          = 0;
                    ModeEntry[i].Height         = 0;
                    ModeEntry[i].Depth          = 0;
                    ModeEntry[i].RefreshRate    = 0;
                    }

                ModeEntry[0].ValidMode      = TRUE;
                ModeEntry[0].Width          = 640;
                ModeEntry[0].Height         = 480;
                ModeEntry[0].Depth          = 8;
                ModeEntry[0].RefreshRate    = 60;

                ModeEntry[1].ValidMode      = TRUE;
                ModeEntry[1].Width          = 640;
                ModeEntry[1].Height         = 480;
                ModeEntry[1].Depth          = 16;
                ModeEntry[1].RefreshRate    = 60;

                ModeEntry[2].ValidMode      = TRUE;
                ModeEntry[2].Width          = 640;
                ModeEntry[2].Height         = 480;
                ModeEntry[2].Depth          = 32;
                ModeEntry[2].RefreshRate    = 60;

                ModeEntry[3].ValidMode      = TRUE;
                ModeEntry[3].Width          = 800;
                ModeEntry[3].Height         = 600;
                ModeEntry[3].Depth          = 8;
                ModeEntry[3].RefreshRate    = 60;

                ModeEntry[4].ValidMode      = TRUE;
                ModeEntry[4].Width          = 800;
                ModeEntry[4].Height         = 600;
                ModeEntry[4].Depth          = 16;
                ModeEntry[4].RefreshRate    = 60;
                
                ModeEntry[5].ValidMode      = TRUE;
                ModeEntry[5].Width          = 800;
                ModeEntry[5].Height         = 600;
                ModeEntry[5].Depth          = 32;
                ModeEntry[5].RefreshRate    = 60;
                
                ModeEntry[6].ValidMode      = TRUE;
                ModeEntry[6].Width          = 1024;
                ModeEntry[6].Height         = 768;
                ModeEntry[6].Depth          = 8;
                ModeEntry[6].RefreshRate    = 60;

                ModeEntry[7].ValidMode      = TRUE;
                ModeEntry[7].Width          = 1024;
                ModeEntry[7].Height         = 768;
                ModeEntry[7].Depth          = 16;
                ModeEntry[7].RefreshRate    = 60;

                ModeEntry[8].ValidMode      = TRUE;
                ModeEntry[8].Width          = 1024;
                ModeEntry[8].Height         = 768;
                ModeEntry[8].Depth          = 32;
                ModeEntry[8].RefreshRate    = 60;
                
                ModeEntry[9].ValidMode      = TRUE;
                ModeEntry[9].Width          = 1280;
                ModeEntry[9].Height         = 960;
                ModeEntry[9].Depth          = 8;
                ModeEntry[9].RefreshRate    = 60;

                ModeEntry[10].ValidMode      = TRUE;
                ModeEntry[10].Width          = 1280;
                ModeEntry[10].Height         = 960;
                ModeEntry[10].Depth          = 16;
                ModeEntry[10].RefreshRate    = 60;

                ModeEntry[11].ValidMode      = TRUE;
                ModeEntry[11].Width          = 1280;
                ModeEntry[11].Height         = 960;
                ModeEntry[11].Depth          = 32;
                ModeEntry[11].RefreshRate    = 60;
                
                ModeEntry[12].ValidMode      = TRUE;
                ModeEntry[12].Width          = 1280;
                ModeEntry[12].Height         = 1024;
                ModeEntry[12].Depth          = 8;
                ModeEntry[12].RefreshRate    = 60;

                ModeEntry[13].ValidMode      = TRUE;
                ModeEntry[13].Width          = 1280;
                ModeEntry[13].Height         = 1024;
                ModeEntry[13].Depth          = 16;
                ModeEntry[13].RefreshRate    = 60;

                ModeEntry[14].ValidMode      = TRUE;
                ModeEntry[14].Width          = 1280;
                ModeEntry[14].Height         = 1024;
                ModeEntry[14].Depth          = 32;
                ModeEntry[14].RefreshRate    = 60;
                
                ModeEntry[15].ValidMode      = TRUE;
                ModeEntry[15].Width          = 1600;
                ModeEntry[15].Height         = 1024;
                ModeEntry[15].Depth          = 8;
                ModeEntry[15].RefreshRate    = 60;

                ModeEntry[16].ValidMode      = TRUE;
                ModeEntry[16].Width          = 1600;
                ModeEntry[16].Height         = 1024;
                ModeEntry[16].Depth          = 16;
                ModeEntry[16].RefreshRate    = 60;

                ModeEntry[17].ValidMode      = TRUE;
                ModeEntry[17].Width          = 1600;
                ModeEntry[17].Height         = 1024;
                ModeEntry[17].Depth          = 32;
                ModeEntry[17].RefreshRate    = 60;

                NumAvailableModesTmp  = 18;

                    
                }
                
            else
            
                {
            

                for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
                    {
                    if (ModeEntry[i].ValidMode)
        
                        {
                        ULONG widthDivide = 1, heightDivide = 1; // For NV11 MultiHead support.

                        if (MULTIMON_MODE(ModeEntry[i].Width, ModeEntry[i].Height)) 
                        {
                            if (HORIZONTAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                            {
                                widthDivide = 2;
                                heightDivide = 1;
                            }
                            else
                            {
                                if (VERTICAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                                {
                                    widthDivide = 1;
                                    heightDivide = 2;
                                }
                            }
                        }
                        


                        //**********************************************************
                        // Remove any mode that doesn't have a refresh rate of 60hz.
                        // - EXCEPT for 320x200, and 640x400 which have refresh
                        //   rates at 70hz...these are required by whql)
                        // - EXCEPT DELL 1501FP multisync flat panel monitor
                        //   Limit DELL refresh rates to 75 hz
                        //**********************************************************
    
                        if (FoundDell1501FP == TRUE)
    
                            {
                                if ( (ModeEntry[i].Width / widthDivide  > HwDeviceExtension->MaxFlatPanelWidth)  ||
                                     (ModeEntry[i].Height / heightDivide > HwDeviceExtension->MaxFlatPanelHeight) || 
                                     (ModeEntry[i].RefreshRate > 75)                               ||
                                     ((ModeEntry[i].Width / widthDivide == 480) && ( ModeEntry[i].Height / heightDivide == 360))   ||
                                     ((ModeEntry[i].Width / widthDivide == 1280) && ( ModeEntry[i].Height / heightDivide == 960))  ||
                                     ((ModeEntry[i].Width / widthDivide == 960) && ( ModeEntry[i].Height / heightDivide == 720))   ||
                                     ((ModeEntry[i].Width / widthDivide == 864) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                     ((ModeEntry[i].Width / widthDivide == 856) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                     ((ModeEntry[i].Width / widthDivide == 852) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                     ((ModeEntry[i].Width / widthDivide == 848) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                     ((ModeEntry[i].Width / widthDivide == 720) && ( ModeEntry[i].Height / heightDivide == 480))   )
                                    {
                                    ModeEntry[i].ValidMode = FALSE;
                                    NumAvailableModesTmp--;
                                    }
                            }
    
                        else
                        
                            {
                                                    
                            if ( (ModeEntry[i].Width / widthDivide > HwDeviceExtension->MaxFlatPanelWidth)  ||
                                 (ModeEntry[i].Height / heightDivide > HwDeviceExtension->MaxFlatPanelHeight) || 
                                 // ((ModeEntry[i].RefreshRate !=60) && 
                                       // (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width / widthDivide == 640) && (ModeEntry[i].Height / heightDivide == 400))) &&
                                       // (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width / widthDivide == 320) && (ModeEntry[i].Height / heightDivide == 200))))     ||
                                 ((ModeEntry[i].Width / widthDivide == 480) && ( ModeEntry[i].Height / heightDivide == 360))   ||
                                 ((ModeEntry[i].Width / widthDivide == 1280) && ( ModeEntry[i].Height / heightDivide == 960))  ||
                                 ((ModeEntry[i].Width / widthDivide == 960) && ( ModeEntry[i].Height / heightDivide == 720))   ||
                                 ((ModeEntry[i].Width / widthDivide == 864) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                 ((ModeEntry[i].Width / widthDivide == 856) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                 ((ModeEntry[i].Width / widthDivide == 852) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                 ((ModeEntry[i].Width / widthDivide == 848) && ( ModeEntry[i].Height / heightDivide == 480))   ||
                                 ((ModeEntry[i].Width / widthDivide == 720) && ( ModeEntry[i].Height / heightDivide == 480))   )
    
    
                                {
                                ModeEntry[i].ValidMode = FALSE;
                                NumAvailableModesTmp--;
                                }

                            }
        
                        }
        
                    }

                }

                // Remove 1152x864 modes from all flat panels. This is because our chips (except NV11
                // NV20) have a scaling buffer of 1K. Note that NV 15 has the same problem.
                // So unless the flatpanel can support 1152x864
                // as a native mode, we can not support it. We don't have the logic in our NT drivers
                // to check if a flatpanel supports 1151x864 as a native mode. Win9X drivers have this 
                // logic. Later we should pull in that logic into NT drivers. But for now, we simply
                // disable the mode for all flat panels.
                if (((HwDeviceExtension->ulChipID & 0xfffc) != NV11_DEVICE_NV11_ID) &&
                     (HwDeviceExtension->ulChipID & 0xfffc) != NV20_DEVICE_NV20_ID)
                    {
                    for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
                        {
                        if (ModeEntry[i].ValidMode)
                            {
                            ULONG widthDivide = 1, heightDivide = 1; // For NV11 MultiHead support.

                            if (MULTIMON_MODE(ModeEntry[i].Width, ModeEntry[i].Height)) 
                            {
                                if (HORIZONTAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                                {
                                    widthDivide = 2;
                                    heightDivide = 1;
                                }
                                else
                                {
                                    if (VERTICAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                                    {
                                        widthDivide = 1;
                                        heightDivide = 2;
                                    }
                                }
                            }      

                            if ((ModeEntry[i].Width / widthDivide == 1152) && (ModeEntry[i].Height / heightDivide == 864))
                                {
                                ModeEntry[i].ValidMode = FALSE;
                                NumAvailableModesTmp--;
                                }
                            }
                        }
                    }
            }



        //**********************************************************************
        // Limit resolutions on TV to 640x480 or less
        //**********************************************************************

        if (    (HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected == 1 && 
                    HwDeviceExtension->TvIsPresent == TRUE) ||
                (HwDeviceExtension->DeskTopInfo.ulNumberDacsConnected > 1 &&
                    (HwDeviceExtension->DeskTopInfo.ulDeviceType[ulHead] == MONITOR_TYPE_NTSC ||
                    HwDeviceExtension->DeskTopInfo.ulDeviceType[ulHead] == MONITOR_TYPE_PAL))) 

            {
            for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                        ULONG widthDivide = 1, heightDivide = 1; // For NV11 MultiHead support.

                        if (MULTIMON_MODE(ModeEntry[i].Width, ModeEntry[i].Height)) 
                        {
                            if (HORIZONTAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                            {
                                widthDivide = 2;
                                heightDivide = 1;
                            }
                            else
                            {
                                if (VERTICAL_MODE(ModeEntry[i].Width, ModeEntry[i].Height))
                                {
                                    widthDivide = 1;
                                    heightDivide = 2;
                                }
                            }
                        }

                    //**********************************************************
                    // Remove all modes greater than width == 640, AND
                    // any mode that doesn't have a refresh rate of 60hz.
                    // (EXCEPT for 320x200, and 640x400 which have refresh
                    // rates at 70hz...these are required by whql)
                    //**********************************************************

                    if ( (ModeEntry[i].Width / widthDivide > 800)  || 
                         ((ModeEntry[i].RefreshRate !=60) && 
                               (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width / widthDivide == 640) && (ModeEntry[i].Height / heightDivide == 400))) &&
                               (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width / widthDivide == 320) && (ModeEntry[i].Height / heightDivide == 200))))     ||
                         ((ModeEntry[i].Width / widthDivide == 480) && ( ModeEntry[i].Height == 360)) )


                        {
                        ModeEntry[i].ValidMode = FALSE;
                        NumAvailableModesTmp--;
                        }
    
                    }
    
                }

            }


        //**********************************************************************
        // pixelFreq validation code.
        //
        // For all NV4 or later devices, we validate the modes by checking the max
        // pixel clock frequency supported by the device/board. RM gives un this
        // info as max pixel clock freq for 8, 16 and 32bpp modes.
        //
        //**********************************************************************

#ifdef NT_BUILD_NV4
#ifndef ENABLE_COMMON_MODESET_CODE

        {
               NV_CFGEX_DAC_PCLK_LIMIT_PARAMS ClkParams;
            
               ClkParams.Head = ulHead;
               ClkParams.pclkLimit = 0;
               //
               // Check if CRT is connected for second head. Only then should we
               // use the pixel clock for the second head. Otherwise, we should use the pixel clock of
               // DAC 0 for both heads.
               //
               if (ulHead == 1 && HwDeviceExtension->DeskTopInfo.ulDeviceType[ulHead] != MONITOR_TYPE_VGA) {
                ClkParams.Head = 0;
               }
            

               if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_8BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
                   VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_8BPP for Head: 0x%x\n",ulHead));
                    return;
               } else {
                   maxPixelClockFrequency_8bpp = ClkParams.pclkLimit;
               }
               if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_16BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
                   VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_16BPP for Head: 0x%x\n",ulHead));
                    return;
               } else {
                   maxPixelClockFrequency_16bpp = ClkParams.pclkLimit;
               }
               if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_32BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
                   VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_32BPP for Head: 0x%x\n",ulHead));
                    return;
               } else {
                   maxPixelClockFrequency_32bpp = ClkParams.pclkLimit;
               }

               // sanity check
               if (maxPixelClockFrequency_8bpp == 0 || maxPixelClockFrequency_16bpp == 0 || maxPixelClockFrequency_32bpp == 0) {
                    VideoDebugPrint((0,"oops! MaxPixelClockFreq is 0!!!. Returning\n"));
                    return;
               }

               VideoDebugPrint((3,"Max8bppFreq (HZ): 0x%x\n",maxPixelClockFrequency_8bpp));
               VideoDebugPrint((3,"Max16bppFreq (HZ): 0x%x\n",maxPixelClockFrequency_16bpp));
               VideoDebugPrint((3,"Max32bppFreq (HZ): 0x%x\n",maxPixelClockFrequency_32bpp));

               for (i = 0; i < HwDeviceExtension->NumRegistryModes ; i++) {
                    if (ModeEntry[i].ValidMode) {
                        // Now get the vesa timings info. First try the DMT timings. If that fails, try the GTF timings.
                        vesaStatus = vesaGetDMTTimings(ModeEntry[i].Width ,
                                       ModeEntry[i].Height,
                                       ModeEntry[i].RefreshRate,
                                       &timingInfo);
                        if (vesaStatus) {
                            vesaStatus = vesaGetGTFTimings(ModeEntry[i].Width ,
                                               ModeEntry[i].Height,    
                                               ModeEntry[i].RefreshRate,
                                               &timingInfo);
                            if (vesaStatus) {
                                // Something wrong. There is no corresponding entry in our vesat tables. We should never
                                // hit this case. Lets mark this mode as invalid to be safe.
                                ModeEntry[i].ValidMode = FALSE;
                                NumAvailableModesTmp--;
                                // go on to validate the next mode
                                continue;
                            }
                        }

                        ThisModeFrequency = timingInfo.PixelClock;
                        // This unit is in (MHZ * 100). Convert to HZ.
                        ThisModeFrequency *= 10000;

                        // Now validate the pixel clock frequency
                        switch (ModeEntry[i].Depth) {
                            case 8:
                                if (ThisModeFrequency > maxPixelClockFrequency_8bpp) {
                                    // The device/board can not support this required pixel clock frequency
                                    ModeEntry[i].ValidMode = FALSE;
                                    NumAvailableModesTmp--;
                                }
                                break;
                            case 16:
                                if (ThisModeFrequency > maxPixelClockFrequency_16bpp) {
                                    // The device/board can not support this required pixel clock frequency
                                    ModeEntry[i].ValidMode = FALSE;
                                    NumAvailableModesTmp--;
                                }
                                break;
                            case 32:
                                if (ThisModeFrequency > maxPixelClockFrequency_32bpp) {
                                    // The device/board can not support this required pixel clock frequency
                                    ModeEntry[i].ValidMode = FALSE;
                                    NumAvailableModesTmp--;
                                }
                                break;
                            default:
                                VideoDebugPrint((0,"oops! Invalid color depth: 0x%x, widht: %d, height: %d, refresh: %d, line: %d\n",
                                    ModeEntry[i].Depth,ModeEntry[i].Width,ModeEntry[i].Height,ModeEntry[i].RefreshRate, __LINE__));
                                break;
                        } // Switch (depth)
                    } // if (ValidMode)
                }   // for (NumRegistryModes)
            } // pixelFreq validation code.
#endif ENABLE_COMMON_MODESET_CODE
#endif NT_BUILD_NV4


        if (ulHead == 0) 
        {
            HwDeviceExtension->NumAvailableModes = NumAvailableModesTmp;
        }
        else
        {
            HwDeviceExtension->NumAvailableModes1 = NumAvailableModesTmp;
        }
        } // For each head
        } // NV4 or later devices 
#endif ENABLE_COMMON_MODESET_CODE
    } // nvValidateModes()


// NOTE: This function is no longer needed. It will be deleted once Sandy removes this call from ddVPP.c
// Determine if a mode is valid for a particular device
BOOL NVValidateDeviceMode(PHW_DEVICE_EXTENSION HwDeviceExtension, VALMODEXTR *vmx)
{

   PMODE_ENTRY SrcModes, DstModes;
   PMODE_ENTRY ModeEntry;
   PHWINFO     NVInfo;
   ULONG       AdapterMemorySize;
   ULONG       BytesPerPel;
   U032        memstatus;
   ULONG       i,index;
   BOOL        ModeValid;
   ULONG       refresh;
   BOOL FoundDell1501FP = FALSE;
   ULONG ulHead;

   ulHead = 0;

   // @ Most of this code is sourced from NvValidateModes above
   // - Copy the modes in the ValidModeTable to our ValidDeviceModeTable
   DstModes = (PMODE_ENTRY)&(HwDeviceExtension->ValidDeviceModeTable[0]);
   SrcModes = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);
   VideoPortMoveMemory(DstModes, SrcModes, sizeof(MODE_ENTRY) * HwDeviceExtension->NumRegistryModes);
   
   //**********************************************************************
   // Limit the modes according to how much video memory is available
   //**********************************************************************
   ModeEntry = DstModes;
   NVInfo = (PHWINFO)&(HwDeviceExtension->NvInfo);

   //**********************************************************************
   // Get the amount of video memory on this card
   //**********************************************************************
   AdapterMemorySize = NVInfo->Framebuffer.RamSize - 0x10000;
    
   //**********************************************************************
   // Now parse the modes to see which ones we can use ( Limit by memory first )
   //**********************************************************************
   for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
      {
       //******************************************************************
       // Handle 15bpp modes same as 16bpp modes (sizewise they're the same)
       //******************************************************************
    
       if ((ModeEntry[i].Depth == 15) || (ModeEntry[i].Depth == 16))
            BytesPerPel = 16/8;
       else
            BytesPerPel = (ModeEntry[i].Depth/8);
    
       //******************************************************************
       // Make the modes that fit in video memory valid. 
       //******************************************************************
    
       if (AdapterMemorySize >= ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel )
                        
           {
           //**************************************************************
           // Set the mode as available if it fits in memory
           //**************************************************************
    
           ModeEntry[i].ValidMode = TRUE;
    
           VideoDebugPrint((2,"NVVDM: mode[%d] valid\n",i));
           VideoDebugPrint((2,"NVVDM:   width(%d)\n",ModeEntry[i].Width));
           VideoDebugPrint((2,"NVVDM:   height(%d)\n",ModeEntry[i].Height));
           VideoDebugPrint((2,"NVVDM:   depth(%d)\n",ModeEntry[i].Depth));
           VideoDebugPrint((2,"NVVDM:   refresh(%d)\n",ModeEntry[i].RefreshRate));
           VideoDebugPrint((2,"NVVDM:   bytes needed(%d)\n",ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel));
           VideoDebugPrint((2,"NVVDM:   memory pres(%d)\n",HwDeviceExtension->AdapterMemorySize));
           }

       else
    
           //**************************************************************
           // Mode not available on this adapter
           //**************************************************************
           {
           ModeEntry[i].ValidMode = FALSE;
           }
      }



   // @ Now that we know what is possible on the "adapter", lets find out whats possible on the requested display device

   // - Assume the requested mode is invalid on the requested device
   ModeValid = FALSE;
   
   // - Filter against requested device   
   switch( vmx->dwDeviceType ) {

      case  MONITOR_TYPE_VGA:
         // - Under NT4, we assume the monitor can do what the adapter can do for now
         //   Under W2K, we would filter again by the monitors caps here
         break;

      case  MONITOR_TYPE_FLAT_PANEL:
         {
            NV_CFGEX_GET_FLATPANEL_INFO_PARAMS fpp;
            unsigned long FPSizeX;
            unsigned long FPSizeY;
            BOOL  RmStatus;

            RmStatus = RmConfigGetExKernel(HwDeviceExtension->DeviceReference, 
                                           NV_CFGEX_GET_FLAT_PANEL_INFO, 
                                           (VOID *)&fpp,
                                           sizeof(NV_CFGEX_GET_FLATPANEL_INFO_PARAMS));

            // - Dynamically assign the width and height maximums  
            if(RmStatus) {
               // - We got something from the Rm, so use its width
               FPSizeX = fpp.FlatPanelSizeX;
               FPSizeY = fpp.FlatPanelSizeY;
            }
            else {
               // changed to match following mods by others 
               FPSizeX = HwDeviceExtension->MaxFlatPanelWidth;
               FPSizeY = HwDeviceExtension->MaxFlatPanelHeight;
            }


            {
            ULONG MonitorNameOffset;
    
            // Look at EDID Detailed block 3 (starts at offset 0x36)
            // See if can find DELL 1501FP flat panel monitor
            MonitorNameOffset = 0x36+ (18*2) + 5;
    
            if ( (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset  ]  == 'D') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+1]  == 'E') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+2]  == 'L') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+3]  == 'L') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+4]  == ' ') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+5]  == '1') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+6]  == '5') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+7]  == '0') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+8]  == '1') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+9]  == 'F') &&
                 (HwDeviceExtension->EDIDBuffer[ulHead][MonitorNameOffset+10] == 'P'))
                {
                FoundDell1501FP = TRUE;
                }
    
            }


            for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    
                {
                if (ModeEntry[i].ValidMode)
    
                    {
                    //**********************************************************
                    // Remove any mode that doesn't have a refresh rate of 60hz.
                    // - EXCEPT for 320x200, and 640x400 which have refresh
                    //   rates at 70hz...these are required by whql)
                    // - EXCEPT DELL 1501FP multisync flat panel monitor
                    //   Limit DELL refresh rates to 75 hz
                    //**********************************************************

                    if (FoundDell1501FP == TRUE)

                        {
                        if ( (ModeEntry[i].Width  > FPSizeX)  ||
                             (ModeEntry[i].Height > FPSizeY)  || 
                             (ModeEntry[i].RefreshRate > 75)                               ||
                             ((ModeEntry[i].Width == 480) && ( ModeEntry[i].Height == 360))   ||
                             ((ModeEntry[i].Width == 1280) && ( ModeEntry[i].Height == 960))  ||
                             ((ModeEntry[i].Width == 960) && ( ModeEntry[i].Height == 720))   ||
                             ((ModeEntry[i].Width == 864) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 856) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 852) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 848) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 720) && ( ModeEntry[i].Height == 480))   )
    
    
                            {
                            ModeEntry[i].ValidMode = FALSE;
                            }

                        }
                        
                        
                    else
                    
                        {                        
                        if ( (ModeEntry[i].Width  > FPSizeX)  ||
                             (ModeEntry[i].Height > FPSizeY)  || 
                             ((ModeEntry[i].RefreshRate !=60) && 
                                   (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width == 640) && (ModeEntry[i].Height == 400))) &&
                                   (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width == 320) && (ModeEntry[i].Height == 200))))     ||
                             ((ModeEntry[i].Width == 480) && ( ModeEntry[i].Height == 360))   ||
                             ((ModeEntry[i].Width == 1280) && ( ModeEntry[i].Height == 960))  ||
                             ((ModeEntry[i].Width == 960) && ( ModeEntry[i].Height == 720))   ||
                             ((ModeEntry[i].Width == 864) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 856) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 852) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 848) && ( ModeEntry[i].Height == 480))   ||
                             ((ModeEntry[i].Width == 720) && ( ModeEntry[i].Height == 480))   )
    
    
                            {
                            ModeEntry[i].ValidMode = FALSE;
                            }

                        }
    
                    }
    
                }
         }
         break;

      case  MONITOR_TYPE_NTSC:
      case  MONITOR_TYPE_PAL:
         // - For now, we use the TV modes filter from VnValidateModes above
         for(i = 0; i < HwDeviceExtension->NumRegistryModes; i++) {

             if(ModeEntry[i].ValidMode) {

                //**********************************************************
                // Remove all modes greater than width == 800, AND
                // any mode that doesn't have a refresh rate of 60hz.
                // (EXCEPT for 320x200, and 640x400 which have refresh
                // rates at 70hz...these are required by whql)
                //**********************************************************

                if ( (ModeEntry[i].Width > 800)  || 
                    ((ModeEntry[i].RefreshRate !=60) && 
                        (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width == 640) && (ModeEntry[i].Height == 400))) &&
                        (!( (ModeEntry[i].RefreshRate == 70) && (ModeEntry[i].Width == 320) && (ModeEntry[i].Height == 200))))     ||
                    ((ModeEntry[i].Width == 480) && ( ModeEntry[i].Height == 360)) ) {

                    ModeEntry[i].ValidMode = FALSE;
                    }
    
                }
    
         }
         break;

      default:
         // - We dont know this device, so mark all modes invalid
         for(i = 0; i < HwDeviceExtension->NumRegistryModes; i++) {

           ModeEntry[i].ValidMode = FALSE;
         }
         break;

   }


   // - Adjust for the refresh type, if dwRefresh is 0, use 60Hz
   if(vmx->dwRefresh == 0) {
      refresh = 60;
   }
   else {
      refresh = vmx->dwRefresh;
   }


   // - Finally, see if requested mode is in the trimmed modes table
   for(i = 0; (ModeValid == FALSE) && (i < HwDeviceExtension->NumRegistryModes) ; i++) {

      if(ModeEntry[i].ValidMode) {

         if( ModeEntry[i].Width == vmx->dwHRes &&
             ModeEntry[i].Height == vmx->dwVRes &&
             ModeEntry[i].Depth == vmx->dwBpp &&
             ModeEntry[i].RefreshRate == refresh ) {

            // - We located the mode, so indicate valid and break out...
            ModeValid = TRUE;
            index = i;
         }
      }
   }


   if(ModeValid) {
      VideoDebugPrint((1,"NVVDM:   Requested mode is VALID...\n"));
      VideoDebugPrint((1,"NVVDM:    width(%d)\n",ModeEntry[index].Width));
      VideoDebugPrint((1,"NVVDM:    height(%d)\n",ModeEntry[index].Height));
      VideoDebugPrint((1,"NVVDM:    depth(%d)\n",ModeEntry[index].Depth));
      VideoDebugPrint((1,"NVVDM:    refresh(%d)\n",ModeEntry[index].RefreshRate));
   }
   else {
      VideoDebugPrint((1,"NVVDM:   Requested mode is INVALID...\n"));
   }


   // - Return the validity code derived above
   return(ModeValid);
}


//
// Does a one-time initialization of various fields required by the new common mode set code.
// Initilizes the pixelclockfrequency limits for 8/16/32 bpp for each head.
//
void InitializeCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension)
{

    NV_CFGEX_DAC_PCLK_LIMIT_PARAMS ClkParams;
    ULONG ulHead;

    for (ulHead = 0; ulHead < HwDeviceExtension->DeskTopInfo.ulNumberDacsOnBoard; ulHead++)
    {
        ClkParams.Head = ulHead;
        ClkParams.pclkLimit = 0;

        //
        // Check if CRT is connected for second head. Only then should we
        // use the pixel clock for the second head. Otherwise, we should use the pixel clock of
        // DAC 0 for both heads.
        //
        // if (ulHead == 1 && HwDeviceExtension->DeskTopInfo.ulDeviceType[ulHead] != MONITOR_TYPE_VGA) {
        // ClkParams.Head = 0;
        // }
            

        if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_8BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
            VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_8BPP for Head: 0x%x\n",ulHead));
            return;
        } else {
            HwDeviceExtension->maxPixelClockFrequency_8bpp[ulHead] = ClkParams.pclkLimit;
        }
        if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_16BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
            VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_16BPP for Head: 0x%x\n",ulHead));
            return;
        } else {
            HwDeviceExtension->maxPixelClockFrequency_16bpp[ulHead] = ClkParams.pclkLimit;
        }
        if (!RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_DAC_PCLK_LIMIT_32BPP, (VOID *)&ClkParams, sizeof(NV_CFGEX_DAC_PCLK_LIMIT_PARAMS))) {
            VideoDebugPrint((0,"Oops!  RmConfigGetExKernel() failed NV_CFGEX_DAC_PCLK_LIMIT_32BPP for Head: 0x%x\n",ulHead));
            return;
        } else {
            HwDeviceExtension->maxPixelClockFrequency_32bpp[ulHead] = ClkParams.pclkLimit;
        }

        // sanity check
        if (HwDeviceExtension->maxPixelClockFrequency_8bpp[ulHead] == 0 || 
            HwDeviceExtension->maxPixelClockFrequency_16bpp[ulHead] == 0 || 
            HwDeviceExtension->maxPixelClockFrequency_32bpp[ulHead] == 0) {
             VideoDebugPrint((0,"oops! MaxPixelClockFreq is 0!!!. Returning\n"));
             return;
        }

        VideoDebugPrint((0,"Head: %d, Max8bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_8bpp[ulHead]));
        VideoDebugPrint((0,"Head: %d, Max16bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_16bpp[ulHead]));
        VideoDebugPrint((0,"Head: %d, Max32bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_32bpp[ulHead]));
    }
}

//
// uses the commond mode set code FindModeEntry() to get the monitor timings for the reqested resolution and head.
//
ULONG GetTimingDacCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo)
{
    ULONG ulFrequencyHead;
    DISPDATA DispData;
    MODEINFO ModeDesc;
    MODEOUT  ModeOut;
    MODEINFO ModeNew;
    ULONG ulRet;
    ULONG ulHeadEDID;

    LPDISPDATA lpDispData;
    LPMODEINFO lpModeDesc;
    LPMODEOUT  lpModeOut;
    LPMODEINFO lpModeNew;

    lpDispData = &DispData;
    lpModeDesc = &ModeDesc;
    lpModeOut = &ModeOut;
    lpModeNew = &ModeNew;

    VideoDebugPrint((3,"Enter GetTimingDacCommonModesetCode()\n"));

    //
    // Check if CRT is connected for second head. Only then should we
    // use the pixel clock for the second head. Otherwise, we should use the pixel clock of
    // DAC 0 for both heads.
    //
    ulFrequencyHead = pResolution->ulHead;
    if (pResolution->ulHead == 1 && pResolution->ulDeviceType != MONITOR_TYPE_VGA) {
         ulFrequencyHead = 0;
    }

    #ifdef ENABLE_HEAD_API
    ulHeadEDID =  ConvertDeviceMaskToIndex(pResolution->ulDeviceMask);
    #else
    ulHeadEDID = pResolution->ulHead;
    #endif




    lpDispData->lpBoardRegistryData = NULL;
    lpDispData->lpMonitorRegistryData = NULL;
    if (HwDeviceExtension->EDIDBufferValid[pResolution->ulHead])
    {
        lpDispData->dwMonitorInfType = PLUG_AND_PLAY_DEVICE;
    }
    else
    {
        lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
    }
    
    lpDispData->dwDeviceID = HwDeviceExtension->ulChipID;
    lpDispData->dwCRTCIndex = pResolution->ulHead;
    lpDispData->dwVideoMemoryInBytes = HwDeviceExtension->AdapterMemorySize;
    lpDispData->dwMaxDacSpeedInHertz8bpp= HwDeviceExtension->maxPixelClockFrequency_8bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz16bpp = HwDeviceExtension->maxPixelClockFrequency_16bpp[ulFrequencyHead];
    lpDispData->dwMaxDacSpeedInHertz32bpp = HwDeviceExtension->maxPixelClockFrequency_32bpp[ulFrequencyHead];
    lpDispData->dwMaxDfpScaledXResInPixels = HwDeviceExtension->MaxFlatPanelWidth;
    lpDispData->lpfnGetModeSize = 0;  // GK: WHat is this?
    lpDispData->dwContext1 = (PVOID)HwDeviceExtension;
    lpDispData->dwContext2 = (PVOID)0;



    lpModeDesc->dwXRes = pResolution->ulDisplayWidth;
    lpModeDesc->dwYRes = pResolution->ulDisplayHeight;
    lpModeDesc->dwBpp = pResolution->ulDisplayPixelDepth;
    lpModeDesc->dwRefreshRate = pResolution->ulDisplayRefresh;
    lpModeDesc->dwDevType = pResolution->ulDeviceType;
    lpModeDesc->dwTVFormat = pResolution->ulTVFormat;
    lpModeDesc->dwOriginalRefreshRate = pResolution->ulDisplayRefresh; 


    // Call the modeset DLL to see if this mode is valid.
    VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
       lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

    ulRet = FindModeEntry(lpDispData, 
                          &(HwDeviceExtension->EDIDBuffer[ulHeadEDID][0]),
                          HwDeviceExtension->EDIDBufferSize[ulHeadEDID], 
                          lpModeDesc, lpModeOut, lpModeNew);
    VideoDebugPrint((3,"ulRet: 0x%x\n",ulRet));
    VideoDebugPrint((3,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
       lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
    VideoDebugPrint((3,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
       lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));
    VideoDebugPrint((3,"lpModeOut: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x,0x%x, 0x%x, 0x%x, 0x%x,0x%x, 0x%x, 0x%x, 0x%x\n",
          lpModeOut->wXRes,
          lpModeOut->wYRes,
          lpModeOut->wHBlankStart,
          lpModeOut->wHBlankWidth,
          lpModeOut->wHSyncStart,
          lpModeOut->wHSyncWidth,
          lpModeOut->wVBlankStart,
          lpModeOut->wVBlankWidth,
          lpModeOut->wVSyncStart,
          lpModeOut->wVSyncWidth,
          lpModeOut->wHTotal,
          lpModeOut->wVTotal,
          lpModeOut->dwPixelClock,
          lpModeOut->dwFormat,
          lpModeOut->wRefreshRate,
          lpModeOut->dwPitchInBytes));

    // Copy the timing info into DAC_TIMING_VALUES
    pTimingInfo->HorizontalVisible = lpModeOut->wXRes;
    pTimingInfo->VerticalVisible = lpModeOut->wYRes;
    pTimingInfo->Refresh = lpModeOut->wRefreshRate;
    pTimingInfo->HorizontalTotal = lpModeOut->wHTotal;
    pTimingInfo->VerticalTotal = lpModeOut->wVTotal;
    pTimingInfo->HorizontalBlankStart = lpModeOut->wHBlankStart;
    pTimingInfo->VerticalBlankStart = lpModeOut->wVBlankStart;
    pTimingInfo->HorizontalRetraceStart = lpModeOut->wHSyncStart;
    pTimingInfo->VerticalRetraceStart = lpModeOut->wVSyncStart;
    pTimingInfo->HorizontalRetraceEnd = lpModeOut->wHSyncStart + lpModeOut->wHSyncWidth;
    pTimingInfo->VerticalRetraceEnd = lpModeOut->wVSyncStart + lpModeOut->wVSyncWidth;
    pTimingInfo->HorizontalBlankEnd = lpModeOut->wHBlankStart + lpModeOut->wHBlankWidth;
    pTimingInfo->VerticalBlankEnd = lpModeOut->wVBlankStart + lpModeOut->wVBlankWidth;
    pTimingInfo->PixelClock = lpModeOut->dwPixelClock;
    pTimingInfo->HSyncpolarity = (lpModeOut->dwFormat & 0x4);
    pTimingInfo->VSyncpolarity = (lpModeOut->dwFormat & 0x8);

    VideoDebugPrint((3,"Exit GetTimingDacCommonModesetCode()\n"));
    return(TRUE);
}


