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
extern ULONG MaskToDeviceType( ULONG ulDeviceMask);
extern ULONG MaskToFindModeEntryDeviceType( ULONG ulDeviceMask);
extern ULONG MaskToTVFormat( ULONG ulDeviceMask);

extern VOID InitializeFPCode(VOID **ppSave, VOID **pMutex);
extern VOID EnterFPCode(VOID *pSave, VOID *pMutex);
extern VOID ExitFPCode(VOID *pSave, VOID *pMutex);



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

    VideoDebugPrint((1,"NumAvailableModes = %d\n", HwDeviceExtension->NumAvailableModes));
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
    // For CRUSH11, set the NV4 bit to signify NV4 compatibility !!
    //**************************************************************************

    else if (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)
        ModeInformation->DriverSpecificAttributeFlags = ( CAPS_CRUSH11_IS_PRESENT | CAPS_NV4_IS_PRESENT) ;

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
        (ModeInformation->DriverSpecificAttributeFlags & CAPS_CRUSH11_IS_PRESENT) ||   
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

    ULONG i, AvailableAdapterMemorySize;
    PHWINFO NVInfo;
    PMODE_ENTRY ModeEntry;
    ULONG NumAvailableModesTmp = 0;
    ULONG ulMonitorType;
    NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS nvPitch;

    HwDeviceExtension->NumAvailableModes = HwDeviceExtension->NumRegistryModes;
    HwDeviceExtension->NumAvailableModes1 = HwDeviceExtension->NumRegistryModes;
    
    //**********************************************************************
    // Get the amount of video memory on this card
    //**********************************************************************
    NVInfo = (PHWINFO)&(HwDeviceExtension->NvInfo);
    HwDeviceExtension->AdapterMemorySize = NVInfo->Framebuffer.RamSize - HwDeviceExtension->TotalInstanceMemory;
    AvailableAdapterMemorySize = HwDeviceExtension->AdapterMemorySize;
#if (_WIN32_WINNT >= 0x0500)
    if (HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_ENABLE_DUALVIEW)
        AvailableAdapterMemorySize /= 2;
#endif

    ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);

    for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)
    {
        nvPitch.Width = ModeEntry[i].Width;
        nvPitch.Height = ModeEntry[i].Height;
        nvPitch.Depth = ModeEntry[i].Depth;

        RmConfigGetExKernel(HwDeviceExtension->DeviceReference, NV_CFGEX_GET_SURFACE_DIMENSIONS, &nvPitch, sizeof(NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS));

        //******************************************************************
        // Make the modes that fit in video memory valid. 
        //******************************************************************
    
        if (AvailableAdapterMemorySize >= nvPitch.Pitch * ModeEntry[i].Height)
        {
            //**************************************************************
            // Set the mode as available if it fits in memory
            //**************************************************************
                if(nvPitch.Pitch > IMAGE_PITCH_LIMIT)
                {
                    ModeEntry[i].ValidMode = FALSE;
                    VideoDebugPrint((0,"%dx%d mode[%d] disabled\n",
                        ModeEntry[i].Width,
                        ModeEntry[i].Height,
                        i));
                }
                else
                {
    
                    ModeEntry[i].ValidMode = TRUE;
                    NumAvailableModesTmp++;
    
                    VideoDebugPrint((2,"mode[%d] valid\n",i));
                    VideoDebugPrint((2,"         width(%d)\n",ModeEntry[i].Width));
                    VideoDebugPrint((2,"         height(%d)\n",ModeEntry[i].Height));
                    VideoDebugPrint((2,"         depth(%d)\n",ModeEntry[i].Depth));
                    VideoDebugPrint((2,"         refresh(%d)\n",ModeEntry[i].RefreshRate));
                    VideoDebugPrint((2,"         bytes needed(%d)\n",nvPitch.Pitch * ModeEntry[i].Height));
                    VideoDebugPrint((2,"         memory pres(%d)\n",HwDeviceExtension->AdapterMemorySize));
                }
        }
        else
            //**************************************************************
            // Mode not available on this adapter
            //**************************************************************
        {
            ModeEntry[i].ValidMode = FALSE;
        }

        switch(HwDeviceExtension->ulChipID)
        {
            case NV4_DEVICE_NV4_ID:               
            case NV5_DEVICE_NV5_ID:               
            case NV5ULTRA_DEVICE_NV5ULTRA_ID:
            case NV5VANTA_DEVICE_NV5VANTA_ID:     
            case NV5MODEL64_DEVICE_NV5MODEL64_ID: 
            case NV0A_DEVICE_NV0A_ID:             
            case NV10_DEVICE_NV10_ID:             
            case NV10DDR_DEVICE_NV10DDR_ID:       
            case NV10GL_DEVICE_NV10GL_ID:         
            case NV15_DEVICE_NV15_ID:
            case NV15DDR_DEVICE_NV15DDR_ID:
            case NV15BR_DEVICE_NV15BR_ID:
            case NV15GL_DEVICE_NV15GL_ID:
                // NV15 and older have HW look table to perform scaling for DFP
                // 1152x864 is not in the table.  Cannot support the mode
                // NV11 and later is not using table anymore and fully programmable
                RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_VIDEO_MONITOR_TYPE, &ulMonitorType);
                if (ulMonitorType == MONITOR_TYPE_FLAT_PANEL && ModeEntry[i].Width == 1152 &&  ModeEntry[i].Height == 864)
                {
                    ModeEntry[i].ValidMode = FALSE;
                }
                break;
            default:
            break;
        }

    }

    HwDeviceExtension->NumAvailableModes = NumAvailableModesTmp;

    } // nvValidateModes()


// NOTE: This function is no longer needed. It will be deleted once Sandy removes this call from ddVPP.c
// Determine if a mode is valid for a particular device
BOOL NVValidateDeviceMode(PHW_DEVICE_EXTENSION HwDeviceExtension, VALMODEXTR *vmx)
{

   PMODE_ENTRY SrcModes, DstModes;
   PMODE_ENTRY ModeEntry;
   PHWINFO     NVInfo;
   ULONG       AvailableAdapterMemorySize;
   ULONG       BytesPerPel;
   U032        memstatus;
   ULONG       i,index;
   BOOL        ModeValid;
   ULONG       refresh;
   BOOL        FoundDell1501FP = FALSE;
   ULONG       ulHead = 0;
   UCHAR       *pEDIDBuffer = HwDeviceExtension->EdidCollection->HeadMaps[ulHead]->EDIDBuffer;

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
   AvailableAdapterMemorySize = NVInfo->Framebuffer.RamSize - HwDeviceExtension->TotalInstanceMemory;
#if (_WIN32_WINNT >= 0x0500)
   if (HwDeviceExtension->ulDualViewStatus & DUALVIEW_STATUS_ENABLE_DUALVIEW)
       AvailableAdapterMemorySize /= 2;
#endif
   
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
    
       if (AvailableAdapterMemorySize >= ModeEntry[i].Width * ModeEntry[i].Height * BytesPerPel )
                        
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
           VideoDebugPrint((2,"NVVDM:   memory pres(%d)\n",AvailableAdapterMemorySize));
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

            // Look at EDID Detailed block 3 (starts at offset 0x36)
            // See if can find DELL 1501FP flat panel monitor
            if (VideoPortCompareMemory(&pEDIDBuffer[0x36+ (18*2) + 5], "DELL 1501FP", 11)  == 11)
            {
                FoundDell1501FP = TRUE;
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

    // Initilize the floating point save/restore code.
    InitializeFPCode(&HwDeviceExtension->pFPStateSave, &HwDeviceExtension->pFPMutex);

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

        VideoDebugPrint((1,"Head: %d, Max8bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_8bpp[ulHead]));
        VideoDebugPrint((1,"Head: %d, Max16bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_16bpp[ulHead]));
        VideoDebugPrint((1,"Head: %d, Max32bppFreq (HZ): 0x%x\n",ulHead, HwDeviceExtension->maxPixelClockFrequency_32bpp[ulHead]));
    }
}

//
// uses the commond mode set code FindModeEntry() to get the monitor timings for the reqested resolution and head.
// If the ulOption flag is set to HEAD_RESOLUTION_OPTION_DONT_BACKOFF, then if the requested mode is not 
// supported in the EDID, ignores the EDID and returns the timing without backing off.
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
    LPEDID_UNIT pEdidUnit;

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

    
    ulHeadEDID = ConvertDeviceMaskToIndex(pResolution->ulDeviceMask);
    pEdidUnit  = HwDeviceExtension->EdidCollection->HeadMaps[ulHeadEDID];
    pResolution->ulDeviceType = MaskToDeviceType(pResolution->ulDeviceMask);
    
    lpDispData->lpBoardRegistryData = NULL;
    lpDispData->lpMonitorRegistryData = NULL;
    if (pEdidUnit->Valid)
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
    lpDispData->dwMaxTVOutResolution = HwDeviceExtension->ulMaxTVOutResolution;

    lpModeDesc->dwXRes = pResolution->ulDisplayWidth;
    lpModeDesc->dwYRes = pResolution->ulDisplayHeight;

    if((pResolution->ulDeviceType == MONITOR_TYPE_FLAT_PANEL) && (HwDeviceExtension->ulDisableDfpModesGreaterThan))
    {
        ULONG ulWidth, ulHeight;

        // Registry key force to limit the resolution
        ulWidth = HwDeviceExtension->ulDisableDfpModesGreaterThan >> 16;
        ulHeight = HwDeviceExtension->ulDisableDfpModesGreaterThan & 0xFFFF;
        if ((pResolution->ulDisplayWidth > ulWidth) && (pResolution->ulDisplayHeight > ulHeight))
        { 
            lpModeDesc->dwXRes = ulWidth;
            lpModeDesc->dwYRes = ulHeight;
        }
    }

    lpModeDesc->dwBpp = pResolution->ulDisplayPixelDepth;
    lpModeDesc->dwRefreshRate = pResolution->ulDisplayRefresh;
    lpModeDesc->dwDevType = MaskToFindModeEntryDeviceType(pResolution->ulDeviceMask);
    lpModeDesc->dwTVFormat = pResolution->ulTVFormat;
    lpModeDesc->dwOriginalRefreshRate = pResolution->ulDisplayRefresh; 

    lpModeDesc->MIDevData.cType     = (CHAR)MaskToFindModeEntryDeviceType(pResolution->ulDeviceMask);
    lpModeDesc->MIDevData.cNumber   = (CHAR)MaskToDeviceType(pResolution->ulDeviceMask);
    lpModeDesc->MIDevData.cFormat   = (CHAR)pResolution->ulTVFormat;
    lpModeDesc->MIDevData.cReserved = 0x0;

    // Call the modeset DLL to see if this mode is valid.
    VideoDebugPrint((3,"Before: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
       lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));

    EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

    ulRet = FindModeEntry(lpDispData, pEdidUnit->EDIDBuffer, pEdidUnit->Size,
                          lpModeDesc, lpModeOut, lpModeNew);
    
    ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

    VideoDebugPrint((3,"ulRet: 0x%x\n",ulRet));
    VideoDebugPrint((3,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
       lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
    VideoDebugPrint((3,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
       lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
       lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));
    if (ulRet != 0)
    {
        VideoDebugPrint((3, "EDID does not support this requested mode: ulRet: 0x%x\n", ulRet));
        //
        // This should not happen on laptop systems. We should never ask the timings for an unsupported
        // mode as pan-scan is handled in the display driver.
        //
        if (HwDeviceExtension->ACPISystem)
        {
            VideoDebugPrint((1,"Laptop: Something wrong. Asking for unsupported physical mode timings"));
            VideoDebugPrint((1,"lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
               lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
               lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
            VideoDebugPrint((1,"lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
               lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
               lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));
        }
        else
        {
            //
            // This is a desktop system. We now try without the EDID. We do this only if the
            // HEAD_RESOLUTION_OPTION_DONT_BACKOFF is set.
            //
            if (pResolution->ulOption & HEAD_RESOLUTION_OPTION_DONT_BACKOFF)
            {
                VideoDebugPrint((1,"Desktop: Getting timing without the EDID\n"));
                lpDispData->dwMonitorInfType = UNKNOWN_DEVICE;
                EnterFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

                ulRet = FindModeEntry(lpDispData, NULL, 0,
                          lpModeDesc, lpModeOut, lpModeNew);
    
                ExitFPCode(HwDeviceExtension->pFPStateSave, HwDeviceExtension->pFPMutex);

                VideoDebugPrint((1,"ulRet: 0x%x\n",ulRet));
                VideoDebugPrint((1,"After: lpModeDescr: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                   lpModeDesc->dwXRes, lpModeDesc->dwYRes, lpModeDesc->dwBpp, lpModeDesc->dwRefreshRate, 
                   lpModeDesc->dwDevType, lpModeDesc->dwOriginalRefreshRate));
                VideoDebugPrint((1,"After: lpModeNew: %d, %d, %d bpp, %d HZ, 0x%x devType, %d HZ ori\n",
                   lpModeNew->dwXRes, lpModeNew->dwYRes, lpModeNew->dwBpp, lpModeNew->dwRefreshRate, 
                   lpModeNew->dwDevType, lpModeNew->dwOriginalRefreshRate));
            }
        }
    }
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


