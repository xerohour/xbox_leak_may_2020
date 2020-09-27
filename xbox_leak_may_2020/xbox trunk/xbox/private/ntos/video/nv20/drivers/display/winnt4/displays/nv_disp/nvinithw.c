//******************************************************************************
//
// Module Name:
//
//     NVINITHW.C
//
// Abstract:
//
//     Initialization of HW routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996-2000  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"
#include "driver.h"
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
//    #include "nvProcMan.h"
    #include "ddmini.h"
#endif

#include "excpt.h"
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#include "ddminint.h"
#include "dmamacro.h"
#include "nvcm.h"
#include "oglDD.h"
#include "oglOverlay.h"
#include "oglutils.h"

// To get the NV11 twinview definitions
#include "nvMultiMon.h"
extern BOOL InitMultiMon(PDEV *ppdev);
extern void InduceAndUpdateDeviceScanning (PPDEV ppdev);
extern VOID NV_WaitForOneVerticalRefresh(PDEV*);


VOID NV4_DmaPushSend(PDEV *);
VOID NV_DmaPush_Wrap(PDEV *, ULONG );
VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG);

BOOL NVInitDmaPushHardware(PDEV *);
VOID NVReleaseDmaPushHardware(PDEV *ppdev,BOOL InitDmaPushHardwareFailFlag);
void NvValidateCaps(PDEV* ppdev);
BOOL bClassSupported(PDEV *ppdev, NvU32 classID);

#ifdef NV3
BOOL bNV3CreateStdPatches(PDEV *ppdev);
VOID vNV3DestroyStdPatches(PDEV *ppdev, BOOL CreateStdPatchFailFlag);
#endif // NV3

#define OFFSET_NV_PFIFO_BASE                        (0x2000)
#define OFFSET_NV_PFIFO_CACHE1_DMA0_REG             (0x3224-OFFSET_NV_PFIFO_BASE)

#define NO_MAPPING                                  ((PVOID)1)

//******************************************************************************
//
//  Function:   bAssertModeHardware
//
//  Routine Description:
//
//      Sets the appropriate hardware state for graphics mode or full-screen.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************


BOOL bAssertModeHardware(
PDEV* ppdev,
BOOL  bEnable)

    {
    BYTE*                   pjIoBase;
    BYTE*                   pjMmBase;
    DWORD                   ReturnedDataLength;
    ULONG                   ulReturn;
    BYTE                    jExtendedMemoryControl;
    VIDEO_MODE_INFORMATION  VideoModeInfo;
    LONG                    cjEndOfFrameBuffer;
    LONG                    cjPointerOffset;
    LONG                    lDelta;
    ULONG                   ulMiscState;
    Nv3ChannelPio           *nv;
    ULONG                   i;
    PVOID                   frameBuffer;
    ULONG                   status;
    TV_CURSOR_ADJUST_INFO   CursorAdjustInfo;
    NV_DESKTOP_INFO         DeskTopInfo;
    NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS nvPitch;
    BOOLEAN                 bVirtualDesktopEnabled;
    ULONG                   ulData;
    DWORD                   dwStatus;

    //**************************************************************************
    // bEnable == TRUE  --> Set the graphics mode
    // bEnable == FALSE --> Reset the adapter into a known state, NTVDM will take over
    //**************************************************************************
    if (bEnable)
        {

        //**********************************************************************
        // Call the miniport via an IOCTL to set the graphics mode.
        //
        // ppdev->ulMode is the REQUESTED mode which is initialized
        // in bInitializeModeFields
        //
        //**********************************************************************

        ppdev->lDelta = 0;

        // Initialize the multi-mon info.
        if(!InitMultiMon(ppdev))
            goto bAssertModeHardware_ReturnFalse;

        ppdev->ulDesktopMode |=  NV_ENABLE_CLASS_SET_MODE;
        DeskTopInfo.ulDesktopModeIndex = ppdev->ulMode;
        DeskTopInfo.ulDesktopMode = ppdev->ulDesktopMode;
        DeskTopInfo.ulTwinView_State = ppdev->TwinView_State;

        DeskTopInfo.ulDesktopWidth = ppdev->cxScreen;
        DeskTopInfo.ulDesktopHeight = ppdev->cyScreen;

        DeskTopInfo.ulNumberDacsOnBoard = ppdev->ulNumberDacsOnBoard;
        DeskTopInfo.ulNumberDacsConnected = ppdev->ulNumberDacsConnected;
        DeskTopInfo.ulNumberDacsActive = ppdev->ulNumberDacsActive;

        DeskTopInfo.ulConnectedDeviceMask = ppdev->ulConnectedDeviceMask;
        DeskTopInfo.ulAllDeviceMask = ppdev->ulAllDeviceMask;
        

        for(i = 0; i < NV_NO_DACS; i++)
        {
            DeskTopInfo.ulDisplayWidth[i] = ppdev->rclCurrentDisplay[i].right - ppdev->rclCurrentDisplay[i].left;
            DeskTopInfo.ulDisplayHeight[i] = ppdev->rclCurrentDisplay[i].bottom - ppdev->rclCurrentDisplay[i].top;
            DeskTopInfo.ulDisplayRefresh[i] = ppdev->ulRefreshRate[i];
            DeskTopInfo.ulDisplayPixelDepth[i] = ppdev->cBitsPerPel;
            DeskTopInfo.ulDeviceDisplay[i] = ppdev->ulDeviceDisplay[i];
            DeskTopInfo.ulDeviceMask[i] = ppdev->ulDeviceMask[i];
            DeskTopInfo.ulDeviceType[i] = ppdev->ulDeviceType[i];
            DeskTopInfo.ulTVFormat[i] = ppdev->ulTVFormat[i];
        }

        if (EngDeviceIoControl(ppdev->hDriver,
                       IOCTL_VIDEO_SET_DESKTOP_INFO_MODE,
                       &DeskTopInfo,  // input buffer
                       sizeof(NV_DESKTOP_INFO),
                       &bVirtualDesktopEnabled,
                       sizeof(BOOLEAN),
                       &ReturnedDataLength))
        {
            DISPDBG((0, "bAssertModeHardware - Failed IOCTL_VIDEO_SET_DESKTOP_INFO_MODE"));
        }
        else if (ReturnedDataLength == sizeof(BOOLEAN))
        {
            if(bVirtualDesktopEnabled)
            {
                DISPDBG((5, "bAssertModeHardware - Support DualView"));
            }
        }

        nvPitch.Width = ppdev->cxScreen;
        nvPitch.Height = ppdev->cyScreen;
        nvPitch.Depth = ppdev->cjPelSize * 8;

        if (NvConfigGetEx(ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                 NV_CFGEX_GET_SURFACE_DIMENSIONS, &nvPitch, sizeof(NV_CFGEX_GET_SURFACE_DIMENSIONS_PARAMS))) 
        {
            DISPDBG((0,"bAssertModeHardware - Cannot get pitch value from RM"));  // ERROR ???
            // IF RM cannot calculate the pitch, it will be in trouble to allocate the primary surface late on.
            goto bAssertModeHardware_ReturnFalse;
        }

        ppdev->lDelta = nvPitch.Pitch;

        // clean up the heap first.
        if(!ppdev->ulEnableDualView)
            NVHEAP_DESTROY();

        NVHEAP_ALLOC_TILED(dwStatus,ppdev->ulPrimarySurfaceOffset, (DWORD)nvPitch.Pitch, (DWORD)ppdev->cyScreen, MEM_TYPE_PRIMARY);
        //
        // Under Dualview, it is likely to fail allocation of frame buffer.
        // Suppose both viws are on.  And 2nd view holds big chunk of DDraw surface, which leave no room for the 1st view to getting bigger.
        // Under such a case, 1st can only shrink.  If it tries to expand, OS DrvChangeDisplaySetting will try again on a fall back res at 
        // 640x480.
        //
        if (dwStatus != 0)
        {
            DISPDBG((0, "bAssertModeHardware - Failed to allocate video memory for frame buffer.  Pitch=0x%08x, cyScreen=0x%08x", nvPitch.Pitch, ppdev->cyScreen));
            if (ppdev->ulEnableDualView)
                return FALSE;
            else
                goto bAssertModeHardware_ReturnFalse;
        }

        ppdev->pjScreen = ppdev->pjFrameBufbase + ppdev->ulPrimarySurfaceOffset;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SET_CURRENT_MODE,
                               &ppdev->ulMode,  // input buffer
                               sizeof(DWORD),
                               &ppdev->bHwVidOvl,
                               sizeof(BOOLEAN),
                               &ReturnedDataLength))
            {
            DISPDBG((1, "bAssertModeHardware - Failed VIDEO_SET_CURRENT_MODE"));
            goto bAssertModeHardware_ReturnFalse;
            }
        else if (ReturnedDataLength != sizeof(BOOLEAN))
            {
            //****************************************************************
            // Miniport didn't know if we can do hw video overlay. Assume we can!
            //****************************************************************
                ppdev->bHwVidOvl = TRUE;

            }

#ifndef NV3
        // Using VIDEO_OVERLAY class, the HW should be able to handle it
        ppdev->bHwVidOvl = TRUE;
#endif

#if _WIN32_WINNT < 0x0500
        if(ppdev->ulNumberDacsActive > 1)
            ppdev->bHwVidOvl = FALSE;
#endif
        //**********************************************************************
        // After we've set the mode, get the information and
        // put it in the VideoModeInfo structure
        //**********************************************************************

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_QUERY_CURRENT_MODE,
                               NULL,
                               0,
                               &VideoModeInfo,
                               sizeof(VideoModeInfo),
                               &ReturnedDataLength))
            {
            DISPDBG((1, "bAssertModeHardware - failed VIDEO_QUERY_CURRENT_MODE"));
            goto bAssertModeHardware_ReturnFalse;
            }

        #if DEBUG_HEAP
            VideoModeInfo.VideoMemoryBitmapWidth  = VideoModeInfo.VisScreenWidth;
            VideoModeInfo.VideoMemoryBitmapHeight = VideoModeInfo.VisScreenHeight;
        #endif


        //**********************************************************************
        // Also get the necessary cursor adjustments
        // (when we're running on a TV)
        //**********************************************************************

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_GET_TV_CURSOR_ADJUST_INFO,
                               NULL,
                               0,
                               &CursorAdjustInfo,
                               sizeof(CursorAdjustInfo),
                               &ReturnedDataLength))
            {
            DISPDBG((1, "bAssertModeHardware - failed VIDEO_QUERY_TV_CURSOR_ADJUST"));
            goto bAssertModeHardware_ReturnFalse;
            }

        //**********************************************************************
        // Store these values in the pdev structure for all to see
        //**********************************************************************

        ppdev->MonitorType  = CursorAdjustInfo.MonitorType;
        ppdev->Underscan_x  = CursorAdjustInfo.Underscan_x;
        ppdev->Underscan_y  = CursorAdjustInfo.Underscan_y;
        ppdev->Scale_x      = CursorAdjustInfo.Scale_x;
        ppdev->Scale_y      = CursorAdjustInfo.Scale_y;
        ppdev->FilterEnable = CursorAdjustInfo.FilterEnable;
        ppdev->TVCursorMin  = CursorAdjustInfo.TVCursorMin;
        ppdev->TVCursorMax  = CursorAdjustInfo.TVCursorMax;

        //**********************************************************************
        // The following variables are determined only after the initial modeset:
        //
        // NV1 / NV3 / NV4:
        //      flCaps is returned to the display driver and denotes whether
        //      NV1 / NV3 / NV4 was found by the miniport
        //
        //**********************************************************************

        ppdev->flCaps   = VideoModeInfo.DriverSpecificAttributeFlags;
        ppdev->cxMemory = VideoModeInfo.VideoMemoryBitmapWidth;
#ifndef RM_HEAPMGR
        ppdev->lDelta   = VideoModeInfo.ScreenStride;
#else
        //************************************************************
        // If we are using the RM heap manager, ask the RM for the
        // pitch. Fallback to using the pitch in the mode table if
        // the RM call fails. For NV3, the RM will return success,
        // but will always return a pitch value of 0, so we have
        // to check for this and fallback to the default pitch.
        //************************************************************
        if(!ppdev->lDelta)
        {
            if (NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                         NV_CFG_PRIMARY_SURFACE_PITCH, &ppdev->lDelta)) 
            {
                ppdev->lDelta = VideoModeInfo.ScreenStride;
            }
            if (!ppdev->lDelta)
                ppdev->lDelta = VideoModeInfo.ScreenStride;
            else
                VideoModeInfo.ScreenStride = ppdev->lDelta;
        }
#endif

        //**********************************************************************
        // Get the frame buffer length from the RM
        //**************************************************************************
        //frameBuffer = ppdev->pjScreen;
        frameBuffer = NO_MAPPING;

        ppdev->hFrameBuffer = DD_FRAME_BUFFER_OBJECT_HANDLE;
        status = NvAllocMemory(ppdev->hDriver,
                                     ppdev->hClient,
                                     ppdev->hDevice,
                                     ppdev->hFrameBuffer,
                                     NV01_MEMORY_LOCAL_USER,
                                     0,
                                     &frameBuffer,
                                     &ppdev->cbFrameBuf);

        if (status != NVOS02_STATUS_SUCCESS)
            {
            DISPDBG((1, "Failed bAssertModeHardware"));
            goto bAssertModeHardware_ReturnFalse;
            }

        ASSERTDD(status == NVOS02_STATUS_SUCCESS,
                     "bAssertModeHardware: Failed to allocate memory");

        //**********************************************************************
        // Save the frame buffer size in bytes rounded to the next lowest 4K boundary.
        // Compute the Frame Buffer size (in scanlines) based on the rounded size in
        // bytes.
        //**********************************************************************
        ppdev->cbFrameBuf += 1;
        ppdev->cyMemory = ppdev->cbFrameBuf / ppdev->lDelta;

        if (ppdev->cyMemory > MAX_CY_MEMORY_VALUE)
            ppdev->cyMemory = MAX_CY_MEMORY_VALUE;

        // Max. Clip value:((ppdev->cyMemory)<<16) | (0x7fff)
        ppdev->dwMaxClip = ((ppdev->cyMemory)<<16) | (0x7fff);

        DISPDBG((1, "NVDD: Get channel pointer from RM"));

        //**********************************************************************
        // NV3 uses the old style patchcord initialization
        // (and it must use the old style Resource Manager too)
        // NV4 no longer uses patchcord initialization
        // (and must use the new NV4 Resource Manager)
        //**********************************************************************
#ifdef NV3
        if (!bNV3CreateStdPatches(ppdev))
            goto bAssertModeHardware_ReturnFalse;
#else
        //******************************************************************
        // New NV4 architecture no longer uses patchcords
        //******************************************************************

        ppdev->DmaPushEnabled2D = TRUE;        // Always enabled for now
        ppdev->fontCacheEnabled = FALSE;       // Always disabled for now
                                               //   since not as fast
        //**************************************************************
        // Use DMA Pusher functionality for 2d
        //**************************************************************

        if (!NVInitDmaPushHardware(ppdev)) {
            goto bAssertModeHardware_ReturnFalse;
        }
#endif // NV3

        //**************************************************************
        // Validate capabilities based on classes we allocated.
        //**************************************************************
        NvValidateCaps(ppdev);
        
        //**************************************************************
        // Initialize heap size info.
        //**************************************************************
        NVHEAP_INFO();

        //**********************************************************************
        // Make sure that Clipping rectangle gets reset by the first hardware function
        // By default, the clipping rectangle should include ALL of video memory
        // (including offscreen). Whenever we change it, we need to set this flag.
        // So, by setting it here, the first function we get to will reset
        // the clipping rectangle.  Also, make sure that pattern gets reset.
        //**********************************************************************

        ppdev->NVClipResetFlag=1;
        ppdev->NVPatternResetFlag=1;

        //**********************************************************************
        // Make sure to reset pitches and offsets across modesets!
        //**********************************************************************

        ppdev->CurrentSourceOffset = 0;
        ppdev->CurrentSourcePitch  = 0;
        ppdev->CurrentDestOffset   = 0;
        ppdev->CurrentDestPitch    = 0;

        //**********************************************************************
        // Display the current mode for our convenience on the debugger
        //**********************************************************************

        DISPDBG((1, "cxMemory: %lx  cyMemory: %lx", ppdev->cxMemory, ppdev->cyMemory));
        DISPDBG((1, "cxScreen: %lx  cyScreen: %lx", ppdev->cxMemory, ppdev->cyMemory));
        DISPDBG((1, "ldelta: %lx ", ppdev->lDelta));

        //**********************************************************************
        // Set the CAPS_SW_POINTER to specify a software cursor if necessary,
        // and make the necessary modifications to pointer.c
        //
        // ppdev->flCaps |= CAPS_SW_POINTER;
        //
        //**********************************************************************

        //**********************************************************************
        // Turn off hardware cursor when running in 960x720 modes
        // (Because we're limited to 46k of PRAMIN instance memory,
        // and cursor caching takes up a lot of it)
        //**********************************************************************

        if ((ppdev->cxScreen == 960) && (ppdev->cyScreen == 720))
            ppdev->flCaps |= CAPS_SW_POINTER;


        //**********************************************************************
        // Do some parameter checking on the values that the miniport
        // returned to us:
        //**********************************************************************

        ASSERTDD(ppdev->cxMemory >= ppdev->cxScreen, "Invalid cxMemory");
        ASSERTDD(ppdev->cyMemory >= ppdev->cyScreen, "Invalid cyMemory");

        OglEnableModeSwitchUpdate(ppdev);

        // Now induce the device scan by the miniport to rebuild the possible device options for each head.
        // InduceAndUpdateDeviceScanning(ppdev);

        NvHwSpecific(ppdev);

        ppdev->bEnabled = TRUE;

        }

    else

        {
        OglDisableModeSwitchUpdate(ppdev);


#ifdef NV3

        //******************************************************************
        // Normally, we would wait on the videocolormap buffer notifier
        // to make sure palette writes are done before deleting the patch/contexts.
        // (ie...palette writes occur during vblank, so we must not delete the
        // associated colormap context until the palette writes are completed)
        // However notifiers for the video colormap are not yet implemented.
        // So for now, we have to wait for at least one vertical refresh to occur ,
        // to make sure all the palette writes have completed.
        // Without this sync, the rm colormap notify routine in the vblank handler
        // could potentially crash.
        //******************************************************************

        NV_WaitForOneVerticalRefresh(ppdev);
        NV_WaitForOneVerticalRefresh(ppdev);
        NV_WaitForOneVerticalRefresh(ppdev);

        vNV3DestroyStdPatches(ppdev, FALSE);

#else
        ASSERTDD(ppdev->DmaPushEnabled2D,"NV4 and up need DmaPushEnabled");
        //**************************************************************
        // Normally, we would wait on the videocolormap buffer notifier
        // to make sure palette writes are done before deleting the patch/contexts.
        // (ie...palette writes occur during vblank, so we must not delete the
        // associated colormap context until the palette writes are completed)
        // However notifiers for the video colormap are not yet implemented.
        // So for now, we have to wait for at least one vertical refresh to occur ,
        // to make sure all the palette writes have completed.
        // Without this sync, the rm colormap notify routine in the vblank handler
        // could potentially crash.
        //**************************************************************

        NV_WaitForOneVerticalRefresh(ppdev);
        NV_WaitForOneVerticalRefresh(ppdev);
        NV_WaitForOneVerticalRefresh(ppdev);

        if (ppdev->iBitmapFormat != BMF_8BPP) 
        {
            if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_SAVE_GAMMA_VALUES,
                           ppdev->ajClutData,  
                           sizeof(NV_VIDEO_CLUTDATA) * NV_NO_DACS * 256,
                           NULL,
                           0,
                           &ReturnedDataLength))
            {
                DISPDBG((1, "bAssertModeHardware - IOCTL_VIDEO_POSTMODE_SET failed"));
            }
        }

        //**********************************************************************
        // We've been asked to go full-screen DOS, but if we're not the VGA 
        // controller- which is possible in multimon- then another card will go 
        // into full-screen DOS. Painting black on our framebuffer avoids screen 
        // corruption. Its just cosmetic.
        //**********************************************************************
        EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_CLEAR_FRAMEBUFFER,
                               NULL,
                               0,
                               NULL,
                               0,
                               &ulReturn);

    
        NVReleaseDmaPushHardware(ppdev, TRUE);
        //**************************************************************
        // Release DMA Pusher functionality
        //**************************************************************

#endif // NV3

        NVHEAP_FREE(ppdev->ulPrimarySurfaceOffset);
        ppdev->ulDesktopMode = 0;

        //**********************************************************************
        // Invalidate cursor cache.
        //**********************************************************************
        for (i=0; i<NUM_CACHED_CURSORS; i++)
            ppdev->SavedCursorCheckSums[i] = 0;


        //**********************************************************************
        // For NV4, when using DMA Push, palette writes
        // (using video colormap) get queued up and don't
        // actually get written until the next vertical blank.
        // Normally, we would wait on the video colormap buffer notifier
        // to make sure the palette writes are done, before resetting
        // the device (calling int 10h).  However notifiers for the video
        // colormap are currrently not functional.  So for now, we'll
        // wait for at least one vertical refresh to occur , to make sure
        // all the palette writes have completed.
        //**********************************************************************

        if (ppdev->iBitmapFormat == BMF_8BPP) {
            NV_WaitForOneVerticalRefresh(ppdev);
        }

        //**********************************************************************
        // Call the kernel driver to reset the device to a known state.
        // NTVDM will take things from there:
        //**********************************************************************

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_RESET_DEVICE,
                               NULL,
                               0,
                               NULL,
                               0,
                               &ulReturn))
            {
            DISPDBG((1, "bAssertModeHardware - Failed reset IOCTL"));
            goto bAssertModeHardware_ReturnFalse;
            }
    }

    //**************************************************************************
    // AssertModeHardware call succeeded.
    //**************************************************************************

    DISPDBG((5, "Passed bAssertModeHardware"));

    return(TRUE);

    //**************************************************************************
    // AssertModeHardware call failed.
    //**************************************************************************

bAssertModeHardware_ReturnFalse:

    DISPDBG((0, "Failed bAssertModeHardware"));

    return(FALSE);
    }

//******************************************************************************
//
//  Function:   bEnableHardware
//
//  Routine Description:
//
//      Puts the hardware in the requested mode and initializes it.
//
//      Note: Should be called before any access is done to the hardware from
//            the display driver.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************

BOOL bEnableHardware(
PDEV*   ppdev)

    {
    BYTE*                       pjIoBase;
    VIDEO_PUBLIC_ACCESS_RANGES  VideoAccessRange[16];
    VIDEO_MEMORY                VideoMemory;
    VIDEO_MEMORY_INFORMATION    VideoMemoryInfo;
    VIDEO_MEMORY                AliVidMem;
    VIDEO_MEMORY_INFORMATION    AliVidMemInfo;
    DWORD                       ReturnedDataLength;
    UCHAR*                      pj;
    USHORT*                     pw;
    ULONG*                      pd;
    ULONG                       i, j;
    ULONG                       ulOffset;
#ifdef PERFTEST
    ULONG* nvptr;
#endif

    //**************************************************************************
    // Create semaphore.  This is mainly used by the
    // ACQUIRE_CRTC_CRITICAL_SECTION macros for an asynchronous cursor.
    //**************************************************************************

    ppdev->csCrtc = EngCreateSemaphore();
    if (ppdev->csCrtc == 0)
        {
        DISPDBG((1, "bEnableHardware - Error creating CRTC semaphore"));
        goto bEnableHardware_ReturnFalse;
        }

    // OpenGL code: Allocate NV3 FIFO semaphore
    ppdev->csFifo = EngCreateSemaphore();
    if (ppdev->csFifo == 0)
        {
        DISPDBG((1, "bEnableHardware - Error creating FIFO semaphore"));
        goto bEnableHardware_ReturnFalse;
        }

    //**************************************************************************
    // Get virtual addresses for the following memory ranges
    //
    // Currently, we'll be giving the display driver
    // public access to 5 ranges (in addition to the frame buffer)
    //
    //     1) NV User Channel 0  (64k)
    //     2) Graphics Status register (to check for Engine Busy)
    //     3) Vertical Blank (PFB_CONFIG_0 register)
    //     4) Frame Buffer Start Address (for page flipping)
    //     5) DAC cursor registers
    //
    // The Dumb Frame buffer ptr is mapped separately in VIDEO_MAP_VIDEO_MEMORY
    //
    // UPDATE: Looks like we'll be needing access to more and more privileged
    //         registers (at least until we get a full resource manager working)
    //         It's becoming a little ugly...but can't be helped right now..
    //         We're programming straight to NV registers, instead of using
    //         objects the way NV was meant to be programmed.
    //
    //     - PRMCIO Registers
    //     - PRMVIO Registers
    //     - PRAMDAC Registers
    //     - PRAMIN Registers
    //     - TEXTURE BUFFER AREA
    //     - PGRAPH Registers
    //     - DMA BUFFER AREA
    //
    //**************************************************************************

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES,
                           NULL,                      // input buffer
                           0,
                           VideoAccessRange,         // output buffer
                           sizeof(VideoAccessRange),
                           &ReturnedDataLength))
        {
        DISPDBG((1, "bEnableHardware - Initialization error mapping IO port base"));
        goto bEnableHardware_ReturnFalse;
        }

    //  Initialize function ptrs and mmio here as soon as we got all necessary info.

    //**************************************************************************
    // Initialize the memory ranges. They're DIFFERENT depending
    // on which chip we're running on, because some ranges that exist
    // on one chip, may not exist on the other
    //**************************************************************************

    ppdev->NvBaseAddr = (volatile DWORD *) VideoAccessRange[0].VirtualAddress;
    ppdev->GrStatusReg = (volatile DWORD *) VideoAccessRange[1].VirtualAddress;
    ppdev->FbConfig0Reg = (volatile DWORD *) VideoAccessRange[2].VirtualAddress;
    ppdev->FbStartAddr = (volatile DWORD *) 0x0;
    ppdev->DACRegs = (volatile DWORD *) 0x0;
    ppdev->PRMCIORegs = (volatile UCHAR *) VideoAccessRange[5].VirtualAddress;
    ppdev->PRMVIORegs = (volatile UCHAR *) VideoAccessRange[6].VirtualAddress;
    ppdev->PRAMDACRegs = (volatile DWORD *) VideoAccessRange[7].VirtualAddress;

    ppdev->PGRAPHRegs = (volatile DWORD *) VideoAccessRange[10].VirtualAddress;
    ppdev->PFIFORegs = (volatile DWORD *) VideoAccessRange[11].VirtualAddress;

#ifdef NV3
    //**********************************************************************
    // Init DMA push length register to 0
    //**********************************************************************
    if (ppdev->PFIFORegs) {
        volatile ULONG *DmaPushLengthReg;
        
        DmaPushLengthReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA0_REG);
        *DmaPushLengthReg = 0;
    }
#endif // NV3

    //**********************************************************************
    // DMA buffer is NO LONGER allocated in the miniport.
    // It's now allocated in EnablePDEV, so do NOT initialize it here.
    // (VideoAccessRange[11] is ZERO)
    //**********************************************************************

    DISPDBG((1, "pjMmBase: 0x%p  ", ppdev->pjMmBase));
    DISPDBG((1, "GrStatusReg: 0x%p  ", ppdev->GrStatusReg));
    DISPDBG((1, "FbConfig0Reg: 0x%p  ", ppdev->FbConfig0Reg));
    DISPDBG((1, "FbStartAddr: 0x%p  ", ppdev->FbStartAddr));
    DISPDBG((1, "DACRegs: 0x%p  ", ppdev->DACRegs));
    DISPDBG((1, "PRMCIORegs: 0x%p  ", ppdev->PRMCIORegs));
    DISPDBG((1, "PRMVIORegs: 0x%p  ", ppdev->PRMVIORegs));
    DISPDBG((1, "PRAMDACRegs: 0x%p  ", ppdev->PRAMDACRegs));
    DISPDBG((1, "PRAMINRegs: 0x%p  ", ppdev->PRAMINRegs));
    DISPDBG((1, "PGRAPHRegs: 0x%p  ", ppdev->PGRAPHRegs));
    DISPDBG((1, "PFIFORegs: 0x%p  ", ppdev->PFIFORegs));


    //**************************************************************************
    // Now set the pointers to the hardware specific functions
    //**************************************************************************
#ifdef NV3
    ppdev->pfnSetDestBase           = NV3_SetDestBase;
    ppdev->pfnSetSourceBase         = NV3_SetSourceBase;

    ppdev->pfnFillSolid             = NV3FillSolid;
    ppdev->pfnFillPat               = NV3FillPatFast;
    ppdev->pfnXfer1bpp              = NV3Xfer1bpp;
    ppdev->pfnXfer4bpp              = NV3Xfer4bpp;
    ppdev->pfnXferNative            = NV3XferNative;
    ppdev->pfnCopyBlt               = NV3CopyBlt;

    ppdev->pfnXfer4to16bpp          = NULL;
    ppdev->pfnXfer8to16bpp          = NULL;
    ppdev->pfnXfer4to32bpp          = NULL;
    ppdev->pfnXfer8to32bpp          = NULL;
    ppdev->pfnIndexedImage          = NULL;   

    ppdev->pfnMemToScreenBlt        = NV3MemToScreenBlt;
    ppdev->pfnMemToScreenPatternBlt = NV3MemToScreenWithPatternBlt;
    ppdev->pfnTextOut               = NV3TextOut;
    ppdev->pfnSetPalette            = NV3SetPalette;

    ppdev->pfnScreenTo1BppMemBlt    = NULL;
    ppdev->pfnGetScreenBits         = NULL;
    ppdev->pfnScreenToMemBlt        = NULL;
    ppdev->pfnScreenToMem16to4bppBlt= NULL;  
    ppdev->pfnScreenToMem32to4bppBlt= NULL; 
    ppdev->pfnScreenToMem16to8bppBlt= NULL; 
    ppdev->pfnScreenToMem32to8bppBlt= NULL; 
    ppdev->pfnScreenToScreenWithPatBlt= NULL;


    ppdev->pfnWaitEngineBusy        = NV3_WaitWhileGraphicsEngineBusy;
    ppdev->pfnWaitForChannelSwitch  = NV3_WaitForChannelSwitch;
    ppdev->pfnDmaPushGo             = NV3_DmaPushGo;
    ppdev->pfnStrokePath            = NV1StrokePath;

    ppdev->pfnAcquireOglMutex       = NV3_AcquireOglMutex;
    ppdev->pfnReleaseOglMutex       = NV3_ReleaseOglMutex;

    ppdev->pfnLineTo                = NV1LineTo;
    ppdev->pfnStretchCopy           = NULL;
#else // NV3

    //**************************************************************
    // Use DMA pusher
    //**************************************************************

    ppdev->pfnSetDestBase           = NV4_DmaPushSetDestBase;  // Needed for newer 565 functions
    ppdev->pfnSetSourceBase         = NV4_DmaPushSetSourceBase;

    ppdev->pfnFillSolid             = NV4DmaPushFillSolid;
    ppdev->pfnFillPat               = NV4DmaPushFillPatFast;
    ppdev->pfnXfer1bpp              = NV4DmaPushXfer1bpp;
    ppdev->pfnXfer4bpp              = NV4DmaPushXfer4bpp;
    ppdev->pfnXferNative            = NV4DmaPushXferNative;
    ppdev->pfnCopyBlt               = NV4DmaPushCopyBlt;

    ppdev->pfnXfer4to16bpp          = NV4DmaPushXfer4to16bpp;
    ppdev->pfnXfer8to16bpp          = NV4DmaPushXfer8to16bpp;
    ppdev->pfnXfer4to32bpp          = NV4DmaPushXfer4to32bpp;
    ppdev->pfnXfer8to32bpp          = NV4DmaPushXfer8to32bpp;
    ppdev->pfnFastXfer8to32         = NV4DmaPushFastXfer8to32;
    ppdev->pfnFastXfer8to16         = NV4DmaPushFastXfer8to16;
    
    ppdev->pfnIndexedImage          = NV4DmaPushIndexedImage;

    ppdev->pfnMemToScreenBlt        = NV4DmaPushMemToScreenBlt;
    ppdev->pfnMemToScreenPatternBlt = NV4DmaPushMemToScreenWithPatternBlt;
    ppdev->pfnTextOut               = NV4DmaPushTextOut;
    ppdev->pfnSetPalette            = NV4DmaPushSetPalette;

    ppdev->pfnScreenTo1BppMemBlt    = NV4ScreenTo1bppMemBlt;
    ppdev->pfnGetScreenBits         = NV4DmaPushDMAGetScreenBits;
    ppdev->pfnScreenToMemBlt        = NV4ScreenToMemBlt;
    ppdev->pfnScreenToMem16to4bppBlt= NV4ScreenToMem16to4bppBlt;     
    ppdev->pfnScreenToMem32to4bppBlt= NV4ScreenToMem32to4bppBlt;     
    ppdev->pfnScreenToMem16to8bppBlt= NV4ScreenToMem16to8bppBlt;     
    ppdev->pfnScreenToMem32to8bppBlt= NV4ScreenToMem32to8bppBlt;     
    ppdev->pfnScreenToScreenWithPatBlt= NV4ScreenToScreenWithPatBlt; 

    ppdev->pfnWaitEngineBusy        = NV4_DmaPushWaitWhileGraphicsEngineBusy;
    ppdev->pfnWaitForChannelSwitch  = NV4_DmaPushWaitForChannelSwitch;
    ppdev->pfnDmaPushGo             = NULL;
    ppdev->pfnStrokePath            = NV4DmaPushStrokePath;
    ppdev->pfnLineTo                = NV4DmaPushLineTo;
    ppdev->pfnStretchCopy           = NV4DmaPushStretchCopy;

    ppdev->pfnAcquireOglMutex = NV4_AcquireOglMutex;
    ppdev->pfnReleaseOglMutex = NV4_ReleaseOglMutex;
#endif // NV3

    //**************************************************************************
    // Memory ranges For NV1 vs NV3 will be initialized AFTER
    // AssertModeHardware so we'll know which chip we're currently running on.
    // These values are stored in VideoAccessRange[] array.
    // So don't overwrite them until we extract them later in this function !
    //**************************************************************************


    //**************************************************************************
    // Allocate 64k for a 'Dummy' user channel so that we can profile
    // performance.  We'll use this instead of the NV user channel
    // when we don't want to send data to NV (i.e. so we can measure
    // performance of the software code by itself)
    //**************************************************************************
#ifdef PERFTEST
    ppdev->TestChannelFlag=0;
    ppdev->TestChannelPtr = EngAllocMem(FL_ZERO_MEMORY, 0x10000, ALLOC_TAG);
    if (ppdev->TestChannelPtr == NULL)
        {
        DISPDBG((1, "DrvEnablePDEV - Failed EngAllocMem"));
        goto bEnableHardware_ReturnFalse;
        }

    //**************************************************************************
    // For each of the 8 subchannels, we set the 'FreeCount' register to 0xffff.
    // This effectively tells the software code that there's always plenty
    // of space in the FIFO (i.e...measure performance for hardware which
    // never has it's FIFO busy)
    //**************************************************************************

    nvptr = ppdev->TestChannelPtr;

    //**************************************************************************
    // Each sub-channel is 8k (nvptr is a pointer to ULONG values)
    // 'FreeCount' register exists 16 bytes after beginning of subchannel
    //**************************************************************************

    *(nvptr+(2048*0)+4) = 0xffffffff;
    *(nvptr+(2048*1)+4) = 0xffffffff;
    *(nvptr+(2048*2)+4) = 0xffffffff;
    *(nvptr+(2048*3)+4) = 0xffffffff;
    *(nvptr+(2048*4)+4) = 0xffffffff;
    *(nvptr+(2048*5)+4) = 0xffffffff;
    *(nvptr+(2048*6)+4) = 0xffffffff;
    *(nvptr+(2048*7)+4) = 0xffffffff;
#endif // ifdef PERFTEST

    //**************************************************************************
    // Get the linear memory address range for the FRAME buffer
    //**************************************************************************

    VideoMemory.RequestedVirtualAddress = NULL;

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_MAP_VIDEO_MEMORY,
                           &VideoMemory,      // input buffer
                           sizeof(VIDEO_MEMORY),
                           &VideoMemoryInfo,  // output buffer
                           sizeof(VideoMemoryInfo),
                           &ReturnedDataLength))
        {
        DISPDBG((1, "bEnableHardware - Error mapping buffer address"));
        goto bEnableHardware_ReturnFalse;
        }

    //**************************************************************************
    // Record the Frame Buffer Linear Address in our PDEV structure.
    //**************************************************************************

    ppdev->pjFrameBufbase  = (BYTE*) VideoMemoryInfo.FrameBufferBase;

    //**************************************************************************
    // If necessary , map an IO port to workaround an ALI chipset cache issue
    //**************************************************************************

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_MAP_IO_PORT_FOR_ALI,
                           &AliVidMem,      // input buffer
                           sizeof(VIDEO_MEMORY),
                           &AliVidMemInfo,  // output buffer
                           sizeof(AliVidMemInfo),
                           &ReturnedDataLength))
        {
        DISPDBG((1, "bEnableHardware - Error mapping IO port for ALI"));
        goto bEnableHardware_ReturnFalse;
        }

    //**************************************************************************
    // We use the FrameBufferBase field  to return the IO base.
    // We use the FrameBufferlength field to signal if an ALI chipset is present of not.
    //**************************************************************************

    // No need to worry about ALI on IA64
#ifndef _WIN64
    ppdev->AliFixupIoBase   = (ULONG)(AliVidMemInfo.FrameBufferBase);
    ppdev->AliFixupIoNeeded = AliVidMemInfo.FrameBufferLength;
#else
    ppdev->AliFixupIoNeeded = FALSE;
#endif

    //**************************************************************************
    // Identifying
    // NV1 vs NV3:  We won't know what chip version of NV we're using until
    //              AFTER the first modeset occurs (in AssertModeHardware).
    //              That is, the miniport will store the CHIP ID information
    //              in the ppdev->flCaps field during QUERY_CURRENT_VIDEO_MODE
    //              (See DriverSpecificAttributeFlags)
    //**************************************************************************
    {
    union
    {
        ULONG osName;
        char devName[NV_DEVICE_NAME_LENGTH_MAX+1];
    } nameBuffer;

    // set the device handle
    ppdev->hDevice = DD_DEVICE_OBJECT_HANDLE;

    // escape the device class to the device reference
    ppdev->hDevClass = NV03_DEVICE_XX;
    nameBuffer.osName = ppdev->ulDeviceReference;

    // register the client and allocate a device
    ppdev->hClient = 0;
    if (NvAllocRoot(ppdev->hDriver, NV01_ROOT, &ppdev->hClient) != NVOS01_STATUS_SUCCESS)
        {
        DISPDBG((2, "NVDD: Cannot register as client to resource manager"));
        goto bEnableHardware_ReturnFalse;
        }


    if (NvAllocDevice(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, ppdev->hDevClass, nameBuffer.devName) != NVOS06_STATUS_SUCCESS)
        {
        DISPDBG((2, "NVDD: Cannot allocate a device"));
        NvFree(ppdev->hDriver, ppdev->hClient, NV01_NULL_OBJECT, ppdev->hClient);
        goto bEnableHardware_ReturnFalse;
        }
    }

    if (!AllocDmaPushBuf(ppdev))
        goto bEnableHardware_ReturnFalse;

    if (!AllocMemOncePerPdev(ppdev))
        goto bEnableHardware_ReturnFalse;

    // initialize the Gamma default values
    for(i = 0; i < NV_NO_DACS; i++)
    {
        ulOffset = i*256;
   
        for(j = 0 ; j < 256; j++)
        {
            ppdev->ajClutData[j + ulOffset].Red   = (UCHAR)j;   
            ppdev->ajClutData[j + ulOffset].Green = (UCHAR)j;
            ppdev->ajClutData[j + ulOffset].Blue  = (UCHAR)j; 
            ppdev->ajClutData[j + ulOffset].Unused = 0;
        }
    }

    //**************************************************************************
    // Now we can set the mode, and fill in the videomode information structure
    // as well as important PPDEV values
    //**************************************************************************

    if (!bAssertModeHardware(ppdev, TRUE))
        goto bEnableHardware_ReturnFalse;

#ifndef NV3
    //**********************************************************
    // Determine the NV Architecture revision.
    //**********************************************************
    NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                 NV_CFG_ARCHITECTURE, (ULONG *)(&(ppdev->dwDeviceVersion)));

    if (ppdev->dwDeviceVersion == NV_DEVICE_VERSION_4)
    {
        ULONG dwRevision;
        NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                     NV_CFG_REVISION, &dwRevision);
        if (dwRevision >= 1) ppdev->dwDeviceVersion = NV_DEVICE_VERSION_5;

       // The intermittent bug, 20001023-154559, hang the Graphic engine while performing INDEXED_IMAGE_FROM_CPU on NV5 only.
       // Disabling (NV4 and NV5 only) the Screen to memory blt would make HW happy for INDEXED_IMAGE_FROM_CPU.
       ppdev->pfnScreenToMemBlt        = NULL;
    }

#endif // !NV3

    
    //**************************************************************************
    // All done, return successful
    //**************************************************************************

    DISPDBG((5, "Passed bEnableHardware"));

    return(TRUE);

    //**************************************************************************
    // Function failed
    //**************************************************************************

bEnableHardware_ReturnFalse:
    ASSERTDD(FALSE, "Failed bEnableHardware");

    return(FALSE);
    }

//******************************************************************************
//
//  Function:   vDisableHardware
//
//  Routine Description:
//
//      Undoes anything done in bEnableHardware.
//      Note: In an error case, we may call this before bEnableHardware is
//            completely done.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************

VOID vDisableHardware(
PDEV*   ppdev)

    {
    DWORD        ReturnedDataLength;
    VIDEO_MEMORY VideoMemory[15];

    FreeMemOncePerPdev(ppdev);
    FreeDmaPushBuf(ppdev);

    NvFree(ppdev->hDriver, ppdev->hClient, NV01_NULL_OBJECT, ppdev->hClient);
    ppdev->hClient = 0;

    //**************************************************************************
    //
    // OpenGL code:
    //
    // Unlock the OpenGL mutex and free it up.
    //
    //**************************************************************************
    DestroyOglGlobalMemory(ppdev);

    //**************************************************************************
    // Free up pointer to frame buffer memory
    //**************************************************************************

    if (ppdev->pjFrameBufbase)
    {
        VideoMemory[0].RequestedVirtualAddress = ppdev->pjFrameBufbase;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNMAP_VIDEO_MEMORY,
                               VideoMemory,
                               sizeof(VIDEO_MEMORY),
                               NULL,
                               0,
                               &ReturnedDataLength))
        {
            DISPDBG((1, "vDisableHardware failed IOCTL_VIDEO_UNMAP_VIDEO"));
        }
        ppdev->pjFrameBufbase = ppdev->pjScreen = NULL;
    }


    //**************************************************************************
    // Free up pointer to IO base for the ALI fixup
    //**************************************************************************

    if (ppdev->AliFixupIoBase)
    {
        VideoMemory[0].RequestedVirtualAddress = (PVOID)(ULONG_PTR)ppdev->AliFixupIoBase;
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNMAP_IO_PORT_FOR_ALI,
                               VideoMemory,
                               sizeof(VIDEO_MEMORY),
                               NULL,
                               0,
                               &ReturnedDataLength))
        {
            DISPDBG((1, "vDisableHardware failed IOCTL_VIDEO_UNMAP_IO_PORT_FOR_ALI"));
        }

        ppdev->AliFixupIoBase   = 0;
        ppdev->AliFixupIoNeeded = FALSE;
    }

    //**************************************************************************
    // Free up the memory ranges. They're DIFFERENT depending
    // on which chip we're running on, because some ranges that exist
    // on one chip, may not exist on the other.
    //**************************************************************************

    //**************************************************************************
    // Free up pointer to NV device memory range
    //**************************************************************************

    if (ppdev->NvBaseAddr)
    {
        VideoMemory[0].RequestedVirtualAddress = (PVOID)(ppdev->NvBaseAddr);
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES,
                               VideoMemory,
                               sizeof(VideoMemory),
                               NULL,
                               0,
                               &ReturnedDataLength))
        {
            DISPDBG((1, "vDisableHardware failed IOCTL_VIDEO_FREE_PUBLIC_ACCESS"));
        }
        ppdev->NvBaseAddr = NULL;
    }

#ifdef PERFTEST
    if (ppdev->TestChannelPtr)
        EngFreeMem(ppdev->TestChannelPtr);
    ppdev->TestChannelPtr = NULL;
#endif  // ifdef PERFTEST
    if (ppdev->csCrtc)
        EngDeleteSemaphore(ppdev->csCrtc);
    if (ppdev->csFifo)
        EngDeleteSemaphore(ppdev->csFifo); // OpenGL code: free NV3 FIFO semaphore
    ppdev->csCrtc = ppdev->csFifo = NULL;
    }

//******************************************************************************
//
//  Function:   NvGetSupportedClasses
//
//  Routine Description:
//      Queries the resman for a list of supported classes on current hardware.
//      Fills in the ppdev->nvClassList and ppdev->nvNumClasses with this info
//
//  Arguments:
//      PDEV *
//
//  Return Value:
//
//      TRUE on success
//
//******************************************************************************

BOOL NvGetSupportedClasses(PDEV *ppdev) {
    
    NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS nvClassListParams;
    
    //**************************************************************************
    // Query the Resman for a list of supported classes
    //
    // First call RM to get the number of supported classes
    // Then call RM again to get fill a preallocated buffer with the class list
    //**************************************************************************
    ppdev->nvNumClasses = 0;
    
    if (ppdev->nvClassList != NULL) {
        EngFreeMem(ppdev->nvClassList);
        ppdev->nvClassList = NULL;
    }
    
    nvClassListParams.numClasses = 0;
    nvClassListParams.classBuffer = NULL;

    if (NVOS_CGE_STATUS_SUCCESS != 
        NvConfigGetEx (ppdev->hDriver,
                       ppdev->hClient,
                       ppdev->hDevice,
                       NV_CFGEX_GET_SUPPORTED_CLASSES, 
                       &(nvClassListParams),
                       sizeof(NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS)
                       )) {
        ASSERTDD(0,"Failed to get list of supported classes");
        return (FALSE);
    }
    ASSERTDD(nvClassListParams.numClasses > 0,"RM reported 0 supported classes");
    
    nvClassListParams.classBuffer = 
        EngAllocMem(0, nvClassListParams.numClasses * sizeof(NvU32), ALLOC_TAG);

    if (NULL == nvClassListParams.classBuffer) {
        return (FALSE);
    }
    
    if (NVOS_CGE_STATUS_SUCCESS != 
        NvConfigGetEx (ppdev->hDriver,
                       ppdev->hClient,
                       ppdev->hDevice,
                       NV_CFGEX_GET_SUPPORTED_CLASSES, 
                       &(nvClassListParams),
                       sizeof(NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS)
                       )) {
        ASSERTDD(0,"Failed to get list of supported classes");
        return (FALSE);
    }

    ppdev->nvClassList  = nvClassListParams.classBuffer;
    ppdev->nvNumClasses = nvClassListParams.numClasses;

    return TRUE;
}

//******************************************************************************
//
//  Function:   NvAllocDmaMemory
//
//  Routine Description:
//      Machine independent wrapper to allocate memory that will be accessed
//      by the adapter.  On 64 bit systems, the RM allocates the memory to
//      ensure that the memory will be addressible by the adapter.  On systems
//      that do not support physical addresses > 32 bits, a EngAllocMem()
//      works fine.
//
//  Return Value:
//
//       NVOS02_STATUS_SUCCESS on success
//
//******************************************************************************

ULONG NvAllocDmaMemory(PDEV *ppdev, PVOID *pAddress, ULONG hMemory, ULONG numBytes)
{
#if defined(_WIN64)
    ULONG limit = ROUND_TO_PAGES(numBytes) - 1;
    ULONG status;

    *pAddress = NULL;
    return (NvAllocMemory(ppdev->hDriver,
                         ppdev->hClient,
                         ppdev->hDevice,
                         hMemory,
                         NV01_MEMORY_SYSTEM,
                         DRF_DEF(OS02, _FLAGS, _LOCATION, _PCI) |
                         DRF_DEF(OS02, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS),
                         pAddress,
                         &limit));
#else
    *pAddress = EngAllocMem(FL_ZERO_MEMORY, numBytes, ALLOC_TAG);
    if (*pAddress == NULL) {
        return NVOS02_STATUS_ERROR_INSUFFICIENT_RESOURCES;
    } else {
        return NVOS02_STATUS_SUCCESS;
    }
#endif
}

//******************************************************************************
//
//  Function:   NvFreeDmaMemory
//
//  Routine Description:
//      Machine independent wrapper to free memory allocated with the
//      routine above.
//
//  Return Value:
//
//       NVOS00_STATUS_SUCCESS on success
//
//******************************************************************************
ULONG NvFreeDmaMemory(PDEV *ppdev, ULONG hMemory, PVOID pAddress)
{
#if defined(_WIN64)
    return (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, hMemory));
#else
    EngFreeMem(pAddress);
    return NVOS00_STATUS_SUCCESS;
#endif
}

//******************************************************************************
//
//  Function: NVInitDmaPushHardware
//
//  Routine Description:
//      Initializes DMA Push objects for NV4 and greater.
//
//  Arguments:
//
//  Return Value:
//      TRUE if successful
//      FALSE if unseccessful
//
//
//******************************************************************************

BOOL NVInitDmaPushHardware(
PDEV *ppdev)
    {
    Nv04ControlDma* nvDmaChannel;
    ULONG           i;
    ULONG           ReturnedDataLength;
    ULONG           status;

    DISPDBG((1, "NV_DISP:  Setting up NV4 and greater hardware for DMA Pusher..."));

    ppdev->bNVInitDmaCompleted = FALSE;

    if(!NvSetupContextDma(ppdev))
        goto NVInitDmaPushHardware_ReturnFalse;

    //**************************************************************************
    // Init local DMA push variables
    //**************************************************************************

    ppdev->nvDmaCount       = 0;
    ppdev->nvDmaCachedGet   = 0;
    ppdev->nvDmaCachedPut   = 0;
    ppdev->nvDmaPreviousPut = 0;
    ppdev->nvDmaWrapFlag    = FALSE;
    ppdev->nvDmaCachedFreeCount = (ppdev->DmaPushBufTotalSize>>2) - DMAPUSH_PADDING;
    ppdev->DmaPushBufCanUseSize = ppdev->nvDmaCachedFreeCount;

    //**************************************************************************
    // Allocate the DMA push channel
    //
    // DMA channel alloc parameters (NVOS07_PARAMETERS):
    //  IN  HANDLE hDriver  Driver Handle
    //  IN  V032 hObject;   unique, user defined handle
    //  IN  V032 hDevice;   currently unused -- use 0
    //  IN  V032 hClass;    currently unused -- use 0
    //  IN  V032 hError;    handle used to allocate the error notifier DMA context
    //  IN  V032 hBuffer;   handle used to allocate the data buffer DMA context
    //  IN  U032 offset;    initial offset into the buffer
    //  OUT P064 pChannel;  ptr to DMA push channel
    //
    //**************************************************************************

    ppdev->hDmaChannel = 0x11111111;  // Unique Handle

    //**************************************************************************
    // After this call, nvDmaChannel will contain the pointer to the DMA push channel
    //**************************************************************************

    ppdev->hDmaChannel = DD_DMA_CHANNEL_OBJECT_HANDLE;
    
    //**************************************************************************
    // Create Pushbuffer Channel DMA
    //**************************************************************************
    {
        NvU32 classes[] = {NV10_CHANNEL_DMA,NV04_CHANNEL_DMA, 0};
        ULONG fail = TRUE;

        for (i=0; classes[i] != 0; i++) {
            if (bClassSupported(ppdev,classes[i])) {
                if (NvAllocChannelDma(ppdev->hDriver,
                                      ppdev->hClient,
                                      ppdev->hDevice,
                                      ppdev->hDmaChannel,
                                      classes[i],
                                      NV_CONTEXT_DMA_PUSHBUF_ERROR_NOTIFIER,
                                      NV_CONTEXT_DMA_FROM_MEMORY_PUSHBUF,
                                      0,
                                      (PVOID)&nvDmaChannel) == NVOS07_STATUS_SUCCESS) {
                    fail = FALSE;
                    break;
                }
            }
        }
        if (fail) {
            DISPDBG((0, "NVDD: Cannot allocate DMA push channel"));
            goto NVInitDmaPushHardware_ReturnFalse;
        } else {
            ppdev->CurrentClass.ChannelDMA = classes[i];
        }
    }

    ppdev->nvDmaChannel = (ULONG *)nvDmaChannel;
    
    if(!NvSetupHwObjects(ppdev))
    {
        DISPDBG((0, "NVInitalDmaPushHardware: Cannot setup HW Objects"));
        goto NVInitDmaPushHardware_ReturnFalse;
    }


    //**************************************************************************
    // Create VIDEO_LUT_CURSOR_DAC object and setup DAC
    //**************************************************************************
    
    if (!NvInitialDac(ppdev)) 
    {
        DISPDBG((0, "NVInitalDmaPushHardware: Cannot initialize DAC"));
        goto NVInitDmaPushHardware_ReturnFalse;
    }

    if (!NvInitHwObjects(ppdev)) 
    {
        DISPDBG((0, "NVInitalDmaPushHardware: Cannot initialize HW Objects"));
        goto NVInitDmaPushHardware_ReturnFalse;
    }

    ppdev->bNVInitDmaCompleted = TRUE;

    return TRUE;



NVInitDmaPushHardware_ReturnFalse:

    //**************************************************************************
    // Remove ALL DMA contexts that we previously allocated!
    //**************************************************************************

    NVReleaseDmaPushHardware(ppdev,TRUE);
    return(FALSE);

} // end of NVInitDmaPushHardware()



//******************************************************************************
//
//  Function: NVReleaseDmaPushHardware
//
//  Routine Description:
//
//  Arguments:
//      A pointer to the physical device
//
//  Return Value:
//      TRUE if successful
//      FALSE if unseccessful
//
//
//******************************************************************************

VOID NVReleaseDmaPushHardware(
    PDEV *ppdev,
    BOOL InitDmaPushHardwareFailedFlag
)
{
    if(ppdev->bNVInitDmaCompleted)
        ppdev->pfnWaitEngineBusy(ppdev);

    //**************************************************************************
    // Ensure hw has completed processing prior to destroying objects.
    //
    // MAKE SURE that the RM is done updating the palette registers
    // before we start freeing the contexts associated with the colormap object
    // (Make sure the 2nd notifier array entry (index 1) is zero)
    //**************************************************************************

    //**************************************************************************
    // If we're releasing DMA contexts because InitDmaPushHardware failed,
    // then the pfnWaitEngineBusy ptr may *NOT* have been initialized.
    // So in this case, don't do a WaitForEnginebusy.
    //**************************************************************************

    if (InitDmaPushHardwareFailedFlag == FALSE)
        if(ppdev->bNVInitDmaCompleted)
            ppdev->pfnWaitEngineBusy(ppdev);

    NvFreeContextDma(ppdev);

    //**************************************************************************
    // Free up the DMA push channel
    //**************************************************************************
    if (ppdev->nvDmaChannel !=NULL)
        {

        NvFree(ppdev->hDriver,
               ppdev->hClient,
               ppdev->hDevice,
               ppdev->hDmaChannel);
        ppdev->nvDmaChannel = NULL;
        }

    return;
}

//******************************************************************************
//
//  Function: AllocDmaPushBuf(PDEV *)
//
//  Routine Description: Attempts to allocate push buf from AGP memory, if
//  there is no AGP memory it falls back to allocating the push buf from
//  system memory.
//  Allocate the DMA push BUFFER (at the very minimum, it should be 8k or larger)
//  Then allocate the DMA Context for the DMA push buffer
//
//  PushBufTotalSize should at the very minimum be large enough to hold
//  the largest possible scanline (width = 2k at 32bpp = 2048*4 = 8k)
//
//  Arguments:
//
//  Return Value:
//            TRUE or FALSE
//
//
//******************************************************************************
BOOL AllocDmaPushBuf(PDEV *ppdev)
{

ULONG   cbPushBuf = ppdev->DmaPushBufTotalSize - 1;
int i;

#if (defined(GDI_AGP_PUSHBUF) && (_WIN32_WINNT >= 0x0500) && (defined(NV4)))

// Attempt to alloc AGP mem to determine if AGP is enabled. Currently we don't
// use AGP mem for the GDI pushbuf. To enable the AGP GDI pushbuf,
// #define GDI_AGP_PUSHBUF.

    if (NvAllocMemory(ppdev->hDriver,
                        ppdev->hClient,
                        ppdev->hDevice,
                        NV_AGP_PUSHBUF_OBJECT_HANDLE,
                        NV01_MEMORY_SYSTEM,
                        (DRF_DEF(OS02, _FLAGS, _LOCATION, _AGP) | DRF_DEF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE)),
                        (PVOID *)&(ppdev->AgpHeapBase),
                        &cbPushBuf))
    {
        ppdev->bAgp = FALSE;
        ppdev->AgpHeapBase = NULL;
        // AGP allocation failed;  will allocate it as NON-AGP device
    }
    else
    {
        ppdev->bAgp = TRUE;
        ppdev->nvDmaFifo = ppdev->AgpPushBuffer = ppdev->AgpHeapBase;

        goto DoneDmaPushAllocation;
    }
#endif

    //**********************************************************************
    // This is not an AGP device.
    //**********************************************************************
    ppdev->DmaPushBufMinSize   = 0x0;                 // Unused
    for(i = 0; i < 4; i++)
    {
        ppdev->DmaPushBufTotalSize = (DMAPUSH_BUFFER_SIZE >> i);
        if (NvAllocDmaMemory(ppdev,
                         &ppdev->nvDmaFifo,
                         NV_SYS_PUSHBUF_OBJECT_HANDLE,
                         ppdev->DmaPushBufTotalSize) != NVOS02_STATUS_SUCCESS)
        {
            DISPDBG((0, "AllocDmaPushBuf: Failed to allocate a %d byte DMA Push buffer", ppdev->DmaPushBufTotalSize));
        }
        else
        {
            goto DoneDmaPushAllocation;
        }
    }

    DISPDBG((0, "AllocDmaPushBuf: Failed to allocate DMA Push buffer"));
    ppdev->nvDmaFifo = NULL;
    return(FALSE);

DoneDmaPushAllocation:

    //**************************************************************************
    // Get location of a DWORD near the very end of the DMA Push buffer.
    // This is needed to workaround a VIA chipset problem.
    // The goal is to flush the chipset's locality/cache.
    // (This is part of the 'Flush Write-Combine Fix')
    // Back off 16 bytes from the end just to be safe.
    //**************************************************************************

    ppdev->nvDmaFlushFixDwordPtr = (ULONG *) ((BYTE *)(ppdev->nvDmaFifo) + ppdev->DmaPushBufTotalSize - 16);
    return(TRUE);


}

//******************************************************************************
//
//  Function: FreeDmaPushBuf(PDEV *)
//
//  Routine Description: 
//
//  Arguments:
//
//  Return Value:
//            None
//
//******************************************************************************
VOID FreeDmaPushBuf(PDEV * ppdev)
{
    if (ppdev->nvDmaFifo != NULL)
    {
// Free DMA push buffer.

#if (_WIN32_WINNT >= 0x0500) && (defined(NV4))
        if (ppdev->AgpPushBuffer)
            {
            ULONG   ReturnedDataLength;
            NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_AGP_PUSHBUF_OBJECT_HANDLE);
            ppdev->AgpPushBuffer = NULL;
            }
        else
#endif  // #if _WIN32_WINNT >= 0x0500
        if (NvFreeDmaMemory(ppdev, NV_SYS_PUSHBUF_OBJECT_HANDLE, ppdev->nvDmaFifo) != NVOS00_STATUS_SUCCESS)
        {
            DISPDBG((1, "NVDD: Cannot free DMA push buffer"));
        }
        ppdev->nvDmaFifo = NULL;
     }


}

//******************************************************************************
//
//  Function: AllocMemOncePerPdev(PDEV *)
//
//  Routine Description: Allocating necessary memory once for members in a PDEV
//
//  Arguments:
//
//  Return Value:
//      TRUE or FALSE
//
//******************************************************************************
BOOL AllocMemOncePerPdev(PDEV * ppdev)
{
    //**************************************************************************
    // Allocate the indexed image LUT buffer and context (system memory)
    //**************************************************************************

    if (NvAllocDmaMemory(ppdev,
                         &ppdev->NvDmaIndexedImageLut,
                         NV_INDEXED_IMAGE_LUT_OBJECT_HANDLE,
                         INDEXED_IMAGE_LUT_BUFFER_SIZE) != NVOS02_STATUS_SUCCESS)

    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot allocate indexed image LUT buffer"));
        return(FALSE);
    }

    //**************************************************************************
    // Create our generic temporary buffer, which may be used by any
    // component. (mainly used by memory to memory format object)
    //
    // NOTE: Add 4K to buffer since there appears to be some hardware problem
    // with the Mem to Mem class that requires some additional space
    //**************************************************************************

    ppdev->MemToMemBufferSize = MEM_TO_MEM_BUFFER_SIZE;

    if (NvAllocDmaMemory(ppdev,
                         &ppdev->pMemToMemBuffer,
                         NV_MEM_TO_MEM_BUFFER_OBJECT_HANDLE,
                         MEM_TO_MEM_BUFFER_SIZE + (4*1024)) != NVOS02_STATUS_SUCCESS)
    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot allocate pMemToMemBuffer"));
        return(FALSE);
    }

    //**************************************************************************
    // Allocate our local copy of palette data
    //**************************************************************************

    if (NvAllocDmaMemory(ppdev,
                         &ppdev->ajClutData,
                         NV_CLUT_DATA_OBJECT_HANDLE,
                         sizeof(NV_VIDEO_CLUTDATA) * NV_NO_DACS * 256) != NVOS02_STATUS_SUCCESS)
    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot allocate ajClutData"));
        return(FALSE);
    }

    if (!NvGetSupportedClasses(ppdev)) 
    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot Get Support Nv Classes"));
        return FALSE;
    }

    //*************************************************************************
    // Allocate memory used for notifiers
    //*************************************************************************

    if (NvAllocDmaMemory(  ppdev,
                           &ppdev->Notifiers,
                           NV_NOTIFIERS_OBJECT_HANDLE,
                           sizeof(NV_NOTIFIERS)))
    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot allocate notifier memory"));
        return(FALSE);
    }

#if defined(_WIN64)
    //*************************************************************************
    // Allocate memory used to save floating point state prior to OpenGL escape
    //*************************************************************************

    ppdev->fpStateSize = EngSaveFloatingPointState(NULL, 0);
    ppdev->fpState = EngAllocMem(0, ppdev->fpStateSize, ALLOC_TAG);
    if (ppdev->fpState == NULL)
    {
        DISPDBG((0, "AllocMemOncePerPdev: Cannot allocate memory for floating point state"));
        return(FALSE);
    }
#endif
    
    return(TRUE);


}


//******************************************************************************
//
//  Function: FreeMemOncePerPdev(PDEV *)
//
//  Routine Description: free memory for members in a PDEV
//
//  Arguments:
//
//  Return Value:
//      TRUE or FALSE
//
//******************************************************************************
VOID FreeMemOncePerPdev(PDEV * ppdev)
{
    //**************************************************************************
    // De-allocate memory for Indexed Image LUT buffer
    //**************************************************************************

    if (ppdev->NvDmaIndexedImageLut != NULL)
    {
        if (NvFreeDmaMemory(ppdev, NV_INDEXED_IMAGE_LUT_OBJECT_HANDLE, ppdev->NvDmaIndexedImageLut) != NVOS00_STATUS_SUCCESS)
        {
            DISPDBG((1, "FreeMemOncePerPdev: Cannot free indexed image LUT buffer"));
        }
        ppdev->NvDmaIndexedImageLut = NULL;
    }

    //**************************************************************************
    // Free our generic temporary buffer, which may be used by any
    // component. (mainly used by memory to memory format object)
    //**************************************************************************

    if (ppdev->pMemToMemBuffer != NULL)
    {
        if (NvFreeDmaMemory(ppdev, NV_MEM_TO_MEM_BUFFER_OBJECT_HANDLE, ppdev->pMemToMemBuffer) != NVOS00_STATUS_SUCCESS)
        {
            DISPDBG((1, "FreeMemOncePerPdev: Cannot free pMemToMemBuffer"));
        }
        ppdev->pMemToMemBuffer = NULL;
    }

    //**************************************************************************
    // Free our local copy of palette data
    //**************************************************************************

    if(ppdev->ajClutData != NULL)
    {
        if (NvFreeDmaMemory(ppdev, NV_CLUT_DATA_OBJECT_HANDLE, ppdev->ajClutData) != NVOS00_STATUS_SUCCESS)
        {
            DISPDBG((1, "FreeMemOncePerPdev: Cannot free ajClutData"));
        }
        ppdev->ajClutData = NULL;
    }

    //*************************************************************************
    // Free notifier memory
    //*************************************************************************

    if(ppdev->Notifiers != NULL)
    {
        if (NvFreeDmaMemory(ppdev, NV_NOTIFIERS_OBJECT_HANDLE, ppdev->Notifiers) != NVOS00_STATUS_SUCCESS)
        {
            DISPDBG((1, "FreeMemOncePerPdev: Cannot free notifier memory"));
        }
        ppdev->Notifiers = NULL;
    }

    //**************************************************************************
    // Free nvidia class list structure
    //**************************************************************************
    if(ppdev->nvClassList)
    {
        EngFreeMem(ppdev->nvClassList);
        ppdev->nvClassList = NULL;
    }

#if defined(_WIN64)
    //*************************************************************************
    // Free memory used to save floating point state
    //*************************************************************************

    if (ppdev->fpState != NULL)
    {
        EngFreeMem(ppdev->fpState);
        ppdev->fpState = NULL;
    }
#endif

}

//******************************************************************************
//
//  Function: NvValidateCaps
//
//  Routine Description: Validates all capabilities based on what classes were
//                       created in NVInitHardwareDmaPush()
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************

void NvValidateCaps(PDEV* ppdev) 
{
        // Query the graphics caps from the RM
    if (NvConfigGet(ppdev->hDriver,
                    ppdev->hClient,
                    ppdev->hDevice,
                    NV_CFG_GRAPHICS_CAPS,
                    &ppdev->hwGfxCaps)
        == NVOS13_STATUS_SUCCESS) {
        // Coolio!
    } else {
        ppdev->hwGfxCaps = 0;
    }
    
#ifdef NV3
    ppdev->ulSurfaceAlign = NV3_BYTE_ALIGNMENT_PAD;
    ppdev->ulSurfaceBitAlign = NV3_BIT_ALIGNMENT;
    ppdev->cxSurfaceMax = (8192 - NV3_BIT_ALIGNMENT/8) / ppdev->cjPelSize; //max_pitch / bytesPP
#else
    
    OglReadRegistry(ppdev);

    //******************************************************************
    // Set surface alignment
    //*************************************************************
    
    if (ppdev->CurrentClass.ContextSurfaces2D == NV10_CONTEXT_SURFACES_2D) {
        ppdev->ulSurfaceAlign = NV10_BYTE_ALIGNMENT_PAD;
        ppdev->ulSurfaceBitAlign = NV10_BIT_ALIGNMENT;
        ppdev->cxSurfaceMax = (65536 - NV10_BIT_ALIGNMENT/8) / ppdev->cjPelSize; //max_pitch / bytesPP
    } else if (ppdev->CurrentClass.ContextSurfaces2D == NV04_CONTEXT_SURFACES_2D) {
        ppdev->ulSurfaceAlign = NV4_BYTE_ALIGNMENT_PAD;
        ppdev->ulSurfaceBitAlign = NV4_BIT_ALIGNMENT;
        ppdev->cxSurfaceMax = (8192 - NV4_BIT_ALIGNMENT/8) / ppdev->cjPelSize; //max_pitch / bytesPP
    } else {
        ASSERTDD(0,"NvValidateCaps: Unsupported CONTEXT_SURFACES_2D");
    }
    
    //**************************************************************************
    // A little bit of validation based on what type of VIDEO_LUT_CURSOR_DAC
    // object we allocated.
    //**************************************************************************
    if (ppdev->CurrentClass.VideoLutCursorDac == NV10_VIDEO_LUT_CURSOR_DAC ||
        ppdev->CurrentClass.VideoLutCursorDac == NV15_VIDEO_LUT_CURSOR_DAC) {
        ppdev->ulMaxHwCursorDepth = 32;
        ppdev->ulMaxHwCursorSize = 64;
        if (ppdev->CurrentClass.VideoLutCursorDac == NV15_VIDEO_LUT_CURSOR_DAC) {
            ppdev->bSupportAlphaCursor = TRUE;
        } else {
            ppdev->bSupportAlphaCursor = FALSE;
        }
    } else {
        ppdev->ulMaxHwCursorDepth = 16;
        ppdev->ulMaxHwCursorSize = 32;
    }
    {
        int i;
        NvU32 classes11[] = { NV11_CELSIUS_PRIMITIVE };
        NvU32 classes20[] = { NV20_KELVIN_PRIMITIVE };
        
        ppdev->oglColor32Depth16 = FALSE;
        ppdev->oglColor16Depth32 = FALSE;
        ppdev->oglMultisample = FALSE;
        for (i=0; i < (sizeof(classes11)/sizeof(NvU32)); i++) {
            if (bClassSupported(ppdev,classes11[i])) {
                ppdev->oglColor32Depth16 = TRUE;
            }
        }
        for (i=0; i < (sizeof(classes20)/sizeof(NvU32)); i++) {
            if (bClassSupported(ppdev,classes20[i])) {
                ppdev->oglColor32Depth16 = TRUE;
                ppdev->oglColor16Depth32 = TRUE;
                ppdev->oglMultisample = TRUE;
            }
        }
    }
#endif    
}

//******************************************************************************
//
//  Function: NvSetupContextDma(PDEV *)
//
//  Routine Description: Allocating Context Dma
//
//  Arguments:
//
//  Return Value:
//      TRUE or FALSE
//
//******************************************************************************
#define SETUP_CTX_DMA(ppdev, ID, pAddr, size)                                                       \
{                                                                                                   \
    if (NvAllocContextDma(ppdev->hDriver,                                                           \
                          ppdev->hClient,                                                           \
                          ID,                                                                       \
                          NV01_CONTEXT_DMA,                                                         \
                          NVOS03_FLAGS_ACCESS_READ_WRITE,                                           \
                          (PVOID)(pAddr),                                                           \
                          size                                                                      \
                          ))                                                                        \
    {                                                                                               \
        DISPDBG((0, "NvSetupContextDma: Cannot allocate context dma for handle ID %08xh", ID));     \
        return(FALSE);                                                                              \
    }                                                                                               \
}                                                                                                   

BOOL NvSetupContextDma(PDEV * ppdev)
{
    ULONG i;

    SETUP_CTX_DMA(ppdev,DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM,              ppdev->pjFrameBufbase,             ppdev->cbFrameBuf);
    SETUP_CTX_DMA(ppdev,NV_DD_PRIMARY_2D_NOTIFIER_CONTEXT_DMA_TO_MEMORY, ppdev->Notifiers->Primary2d,       sizeof(NvNotification)*5 - 1);
    SETUP_CTX_DMA(ppdev,NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,   ppdev->Notifiers->DmaToMem,        sizeof(NvNotification)*2 - 1);
    SETUP_CTX_DMA(ppdev,NV_WIN_INDEXED_IMAGE_CONTEXT,                    ppdev->NvDmaIndexedImageLut,       INDEXED_IMAGE_LUT_BUFFER_SIZE - 1);
    SETUP_CTX_DMA(ppdev,DD_MEMORY_TO_MEMORY_CONTEXT_DMA_NOTIFIER,        ppdev->Notifiers->MemToMem,        sizeof(NvNotification)*2 - 1);
    SETUP_CTX_DMA(ppdev,DD_TEMP_BUFFER_CONTEXT_DMA,                      ppdev->pMemToMemBuffer,            ppdev->MemToMemBufferSize+4*1024-1);
    SETUP_CTX_DMA(ppdev,NV_CONTEXT_DMA_FROM_MEMORY_PUSHBUF,              ppdev->nvDmaFifo,                  ppdev->DmaPushBufTotalSize - 1);
    SETUP_CTX_DMA(ppdev,NV_CONTEXT_DMA_PUSHBUF_ERROR_NOTIFIER,           ppdev->Notifiers->DmaPushBufErr,   sizeof(NvNotification) - 1);
    SETUP_CTX_DMA(ppdev,NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER,    ppdev->Notifiers->VideoLutCursorDac,sizeof(NvNotification)*9-1);

    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        SETUP_CTX_DMA(ppdev,NV_WIN_COLORMAP_CONTEXT + ppdev->ulDeviceDisplay[i],
                            &ppdev->ajClutData[ppdev->ulDeviceDisplay[i]*256],
                            (256 * sizeof(VIDEO_CLUTDATA)) - 1);
    }

    for(i = 0; i < MAX_INDEXED_IMAGE_NOTIFIERS; i++)
    {
        SETUP_CTX_DMA(ppdev, NV_DD_FROM_MEM_INDEXED_IMAGE1_NOTIFIER_CONTEXT_DMA_TO_MEMORY+i,
                             &(ppdev->Notifiers->DmaToMemIndexedImage[i*sizeof(NvNotification)]),
                             sizeof(NvNotification) - 1);
    }

    return(TRUE);
}

//******************************************************************************
//
//  Function: NvFreeContextDma(PDEV *)
//
//  Routine Description: Free Allocated Context Dma
//
//  Arguments:
//
//  Return Value:
//      None
//
//******************************************************************************
#define FREE_CTX_DMA(ppdev, ID)                                                                 \
{                                                                                               \
    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, ID) != NVOS00_STATUS_SUCCESS)    \
    {                                                                                           \
        DISPDBG((1, "NvFreeContextDma: Cannot free DMA context of handle %d", ID));             \
    }                                                                                           \
}

VOID NvFreeContextDma(PDEV * ppdev)
{
    ULONG i;

    FREE_CTX_DMA(ppdev, DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    FREE_CTX_DMA(ppdev, NV_DD_PRIMARY_2D_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    FREE_CTX_DMA(ppdev, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    FREE_CTX_DMA(ppdev, NV_WIN_INDEXED_IMAGE_CONTEXT);                   
    FREE_CTX_DMA(ppdev, DD_MEMORY_TO_MEMORY_CONTEXT_DMA_NOTIFIER);
    FREE_CTX_DMA(ppdev, DD_TEMP_BUFFER_CONTEXT_DMA);                     
    FREE_CTX_DMA(ppdev, NV_CONTEXT_DMA_FROM_MEMORY_PUSHBUF);             
    FREE_CTX_DMA(ppdev, NV_CONTEXT_DMA_PUSHBUF_ERROR_NOTIFIER);          
    FREE_CTX_DMA(ppdev, NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER);

    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        FREE_CTX_DMA(ppdev,NV_WIN_COLORMAP_CONTEXT + ppdev->ulDeviceDisplay[i]);
    }

    for(i = 0; i < MAX_INDEXED_IMAGE_NOTIFIERS; i++)
    {
        FREE_CTX_DMA(ppdev, NV_DD_FROM_MEM_INDEXED_IMAGE1_NOTIFIER_CONTEXT_DMA_TO_MEMORY+i);
    }
}


//******************************************************************************
//
//  Function: NvInitHwObject(PDEV *)
//
//  Routine Description: Initializing hw Objects
//
//  Arguments:
//
//  Return Value:
//      None
//
//******************************************************************************
BOOL NvInitHwObjects(PDEV * ppdev)
{
    ULONG i;
    ULONG color0, color1;
    ULONG videoFmt;

    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Determine black and white colors
    //**************************************************************************

    switch (ppdev->iBitmapFormat)
    {
        case BMF_32BPP:
            color0 =    NV_ALPHA_1_32 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_32 | 0x00ffffff;         // WHITE
            videoFmt =  NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
            ppdev->AlphaEnableValue = NV_ALPHA_1_32;
            break;
        case BMF_16BPP:

            if (ppdev->flGreen == 0x03e0)
                {
                //**************************************************************
                // 5:5:5 format
                //**************************************************************

                color0 =    NV_ALPHA_1_16 | 0x00000000;     // BLACK
                color1 =    NV_ALPHA_1_16 | 0x00007fff;     // WHITE
                videoFmt =  NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2;
                ppdev->AlphaEnableValue = NV_ALPHA_1_16;        // 5:5:5 format
                }
            else
                {
                //**************************************************************
                // 5:6:5 format
                //**************************************************************

                color0 =   (NV_ALPHA_1_565 | 0x00000000);       // BLACK
                color1 =   (NV_ALPHA_1_565 | 0x0000ffff);       // WHITE
                videoFmt = NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
                ppdev->AlphaEnableValue = NV_ALPHA_1_565;       // 5:6:5 format
                }

            break;
        case BMF_8BPP:
            color0 =    NV_ALPHA_1_08 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_08 | 0x000000ff;         // WHITE
            videoFmt =  NV_VFM_FORMAT_COLOR_LE_Y8_P4;
            ppdev->AlphaEnableValue = NV_ALPHA_1_08;
            break;

        default:
            return(FALSE);
    }

    //**************************************************************************
    // Setup ROP5 SOLID
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(10)));

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NVFFF_SET_OBJECT(0),               DD_ROP5_SOLID);

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_CONTEXT_DMA_NOTIFIES, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5,                 0xcccc);

    //**************************************************************************
    // Setup IMAGE BLACK RECTANGLE
    //**************************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(12)));

    NV_DMAPUSH_WRITE1(CLIP_SUBCHANNEL, NVFFF_SET_OBJECT(0),                    DD_IMAGE_BLACK_RECTANGLE);

    NV_DMAPUSH_WRITE1(CLIP_SUBCHANNEL, NV019_SET_CONTEXT_DMA_NOTIFIES,      NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(CLIP_SUBCHANNEL, NV019_SET_POINT,                     0);
    NV_DMAPUSH_WRITE1(CLIP_SUBCHANNEL, NV019_SET_SIZE,                      ((0x7fff<<16) | 0x7fff));

    //*************************************************************************
    // Create the memory to memory format object.
    //*************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(12)));

    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),           MY_MEMORY_TO_MEMORY);

    NV_DMAPUSH_WRITE1(DD_SPARE, NV039_SET_CONTEXT_DMA_NOTIFIES,     DD_MEMORY_TO_MEMORY_CONTEXT_DMA_NOTIFIER );
    NV_DMAPUSH_WRITE1(DD_SPARE, NV039_SET_CONTEXT_DMA_BUFFER_IN,    DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV039_SET_CONTEXT_DMA_BUFFER_OUT,   DD_TEMP_BUFFER_CONTEXT_DMA);

    ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;

    //**********************************************************************
    // Setup DST surface for device bitmaps
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(20)));
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),                    DD_PRIMARY_IMAGE_IN_MEMORY);

    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_CONTEXT_DMA_NOTIFIES,      NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_CONTEXT_DMA_IMAGE_SOURCE,  DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_CONTEXT_DMA_IMAGE_DESTIN,  DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);

    // Determine the color format
    switch (ppdev->iBitmapFormat) {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_R5G6B5);
            break;
        case BMF_8BPP:
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_Y8);
            break;
        default:
            return(FALSE);
    }

    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_PITCH,                 ( ((ppdev->lDelta) << 16) |  (ppdev->lDelta)) ) ;
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_OFFSET_SOURCE,         ppdev->ulPrimarySurfaceOffset);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_OFFSET_DESTIN,         ppdev->ulPrimarySurfaceOffset);

    //**********************************************************************
    // Setup CONTEXT PATTERN
    //**********************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(24)));
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NVFFF_SET_OBJECT(0),       DD_IMAGE_PATTERN);

    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_CONTEXT_DMA_NOTIFIES, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
            break;
        case BMF_8BPP:
            // Must be set to a legal value but hardware ignores it otherwise
            NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
            break;
        default:
            return(FALSE);
    }

    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT,        NV044_SET_MONOCHROME_FORMAT_CGA6_M1);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_SHAPE,         NV044_SET_MONOCHROME_SHAPE_64X_1Y);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_SELECT,           NV044_SET_PATTERN_SELECT_MONOCHROME);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_COLOR0,        color0);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_COLOR1,        color1);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_PATTERN0,      0xffffffff);
    NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_PATTERN1,      0x50505050);

    //**********************************************************************
    // Y8 default pattern values
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(17)));
    NV_DMAPUSH_START(16, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_Y8(0));
    for (i=0;i<16;i++)
        NV_DMA_FIFO = 0xffffffff;

    //**********************************************************************
    // R5G6B5 default pattern values
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(33)));
    NV_DMAPUSH_START(32, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_R5G6B5(0));
    for (i=0;i<32;i++)
        NV_DMA_FIFO = 0xffffffff;

    //**********************************************************************
    // X1R5G5B5 default pattern values
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(33)));
    NV_DMAPUSH_START(32, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_X1R5G5B5(0));
    for (i=0;i<32;i++)
        NV_DMA_FIFO = 0xffffffff;

    //**********************************************************************
    // X8R8G8B8 default pattern values
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(65)));
    NV_DMAPUSH_START(64, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_X8R8G8B8(0));
    for (i=0;i<64;i++)
        NV_DMA_FIFO = 0xffffffff;

    //**********************************************************************
    // Setup CONTEXT COLOR KEY
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(12)));
    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),                DD_IMAGE_SOLID);

    NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_CONTEXT_DMA_NOTIFIES,  NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    // Determine the color format
    switch (ppdev->iBitmapFormat) {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR_FORMAT, NV057_SET_COLOR_FORMAT_LE_A8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR_FORMAT, NV057_SET_COLOR_FORMAT_LE_A16R5G6B5);
            break;
        case BMF_8BPP:
            // Does hardware ignore this value???
            NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR_FORMAT, NV057_SET_COLOR_FORMAT_LE_A8R8G8B8);
            break;
        default:
            return(FALSE);
    }

    NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR, 0);  // Disabled

    ppdev->dDrawSpareSubchannelObject = DD_IMAGE_SOLID;

    //******************************************************************
    // Setup multiple Indexed Image From CPU objects (565 format)
    //
    // Each object has an associated LUT context DMA and a notifier.
    // This is necessary since a LUT can't be updated until the operation
    // is completed. To improve performance, many objects were created so
    // that we didn't have to wait for the previous indexed image operation
    // to be completed.
    //
    // NOTE:  We CAN NOT use the COLOR_KEY (IMAGE_SOLID)
    //        if ROP_OPERATION = ROP_AND
    //******************************************************************                                                 
    for(i=0; i < MAX_INDEXED_IMAGE_NOTIFIERS; i++)
    {
        NV_DMAPUSH_CHECKFREE(((ULONG)(22)));
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NVFFF_SET_OBJECT(0),                  DD_INDEXED_IMAGE1_FROM_CPU + i);

        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_DMA_LUT,         NV_WIN_INDEXED_IMAGE_CONTEXT);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_DMA_NOTIFIES,
                          NV_DD_FROM_MEM_INDEXED_IMAGE1_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_CLIP_RECTANGLE,  DD_IMAGE_BLACK_RECTANGLE);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_PATTERN,         DD_IMAGE_PATTERN);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_ROP,             DD_ROP5_SOLID);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_CONTEXT_SURFACE,         DD_PRIMARY_IMAGE_IN_MEMORY);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_COLOR_CONVERSION,        NV060_SET_COLOR_CONVERSION_TRUNCATE);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_OPERATION,               NV060_SET_OPERATION_ROP_AND);

        // Determine the color format
        switch (ppdev->iBitmapFormat) 
        {
            case BMF_32BPP:
                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_COLOR_FORMAT, NV060_SET_COLOR_FORMAT_LE_X8R8G8B8);
                break;
            case BMF_16BPP:
                // In 16bpp, hardware will dither when using R5G6B5, so we must use X8R8G8B8 to get the exact desired results
                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_COLOR_FORMAT, NV060_SET_COLOR_FORMAT_LE_X8R8G8B8);
                break;
            case BMF_8BPP:
                // Does hardware ignore this value???
                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV060_SET_COLOR_FORMAT, NV060_SET_COLOR_FORMAT_LE_X8R8G8B8);
                break;
            default:
                return(FALSE);
        }
    }

    //**********************************************************************
    // Setup Image From CPU (565 format)
    //
    // NOTE:  We CAN NOT use the COLOR_KEY (IMAGE_SOLID)
    //        if ROP_OPERATION = ROP_AND
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(20)));
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NVFFF_SET_OBJECT(0),                  DD_IMAGE_FROM_CPU);

    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_DMA_NOTIFIES,    NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_CLIP_RECTANGLE,  DD_IMAGE_BLACK_RECTANGLE);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_PATTERN,         DD_IMAGE_PATTERN);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_ROP,             DD_ROP5_SOLID);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_BETA4,           NV_DD_CONTEXT_BETA4);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_SURFACE,         DD_PRIMARY_IMAGE_IN_MEMORY);
    NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OPERATION,               NV061_SET_OPERATION_ROP_AND);

    // Determine the color format
    switch (ppdev->iBitmapFormat) {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_R5G6B5);
            break;
        case BMF_8BPP:
            // Does hardware ignore this value???
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        default:
            return(FALSE);
    }

    //**********************************************************************
    // Setup Image BLIT (565 format)
    //
    // NOTE:  We CAN NOT use the COLOR_KEY (IMAGE_SOLID)
    //        if ROP_OPERATION = ROP_AND
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(18)));
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),                    DD_IMAGE_BLIT);

    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_DMA_NOTIFIES,      NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_CLIP_RECTANGLE,    DD_IMAGE_BLACK_RECTANGLE);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_PATTERN,           DD_IMAGE_PATTERN);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_ROP,               DD_ROP5_SOLID);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_BETA4,             NV_DD_CONTEXT_BETA4);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_SURFACES,          DD_PRIMARY_IMAGE_IN_MEMORY);
    NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_OPERATION,                 NV05F_SET_OPERATION_ROP_AND);

    //**********************************************************************
    // Setup RECT and TEXT  (565 format)
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(22)));
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NVFFF_SET_OBJECT(0),               DD_RENDER_RECT_AND_TEXT);

    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_CONTEXT_DMA_NOTIFIES, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_CONTEXT_DMA_FONTS,    DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_CONTEXT_PATTERN,      DD_IMAGE_PATTERN);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_CONTEXT_ROP,          DD_ROP5_SOLID);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_CONTEXT_SURFACE,      DD_PRIMARY_IMAGE_IN_MEMORY);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_OPERATION,            NV04A_SET_OPERATION_ROP_AND);

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_COLOR_FORMAT, NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_COLOR_FORMAT, NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5);
            break;
        case BMF_8BPP:
            NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_COLOR_FORMAT, NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        default:
            return(FALSE);
    }

    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_SET_MONOCHROME_FORMAT,    NV04A_SET_MONOCHROME_FORMAT_CGA6_M1);

    NV_DMAPUSH_CHECKFREE(((ULONG)(20)));

    //**********************************************************************
    // Setup SOLID_LINE
    //**********************************************************************
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NVFFF_SET_OBJECT(0),                   DD_IMAGE_ROP_AND_LIN);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_CONTEXT_DMA_NOTIFIES,     NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_CONTEXT_CLIP_RECTANGLE,   DD_IMAGE_BLACK_RECTANGLE);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_CONTEXT_PATTERN,          DD_IMAGE_PATTERN);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_CONTEXT_ROP,              DD_ROP5_SOLID);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_CONTEXT_SURFACE,          DD_PRIMARY_IMAGE_IN_MEMORY);
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_OPERATION,                NV05C_SET_OPERATION_ROP_AND);
    ppdev->dDrawSpareSubchannelObject     = DD_IMAGE_ROP_AND_LIN;

    // Determine the color format
    switch (ppdev->iBitmapFormat) {
        case BMF_32BPP:
            NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_COLOR_FORMAT, NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        case BMF_16BPP:
            NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_COLOR_FORMAT, NV05C_SET_COLOR_FORMAT_LE_X16R5G6B5);
            break;
        case BMF_8BPP:
            NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_SET_COLOR_FORMAT, NV05C_SET_COLOR_FORMAT_LE_X8R8G8B8);
            break;
        default:
            return(FALSE);
    }

    if (ppdev->cjPelSize>1)
    {
        ASSERT((2==ppdev->cjPelSize) || (4==ppdev->cjPelSize));

        NV_DMAPUSH_CHECKFREE(((ULONG)(10)));

        /* Set color key blit object into subchannel. */
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),            DD_IMAGE_BLIT_COLORKEY);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV05F_SET_CONTEXT_DMA_NOTIFIES, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV05F_SET_CONTEXT_SURFACES,     DD_PRIMARY_IMAGE_IN_MEMORY);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV05F_SET_CONTEXT_COLOR_KEY,    DD_IMAGE_SOLID);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV05F_SET_OPERATION,            NV05F_SET_OPERATION_SRCCOPY_AND); // SRCCOPY doesn't support color key

        ppdev->dDrawSpareSubchannelObject = DD_IMAGE_BLIT_COLORKEY;
    }    

    //**********************************************************************
    // Setup ALPHA IMAGE FROM MEMORY
    //**********************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(8*2)));
    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),            DD_ALPHA_IMAGE_FROM_MEMORY);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_DMA_NOTIFIES, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_DMA_IMAGE,    DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_PATTERN,      DD_IMAGE_PATTERN);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_ROP,          DD_ROP5_SOLID);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_BETA4,        NV_DD_CONTEXT_BETA4);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_COLOR_FORMAT,         NV077_SET_COLOR_FORMAT_LE_A8R8G8B8);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_OPERATION,            NV077_SET_OPERATION_BLEND_PREMULT);

    ppdev->dDrawSpareSubchannelObject = DD_ALPHA_IMAGE_FROM_MEMORY;

    //**************************************************************************
    // Make sure to update the DMA count before we kickoff!
    //**************************************************************************
    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);

    ((NvNotification *) (ppdev->Notifiers->Primary2d))->status = 0;

    ((NvNotification *) (ppdev->Notifiers->DmaToMem))->status = 0;

    ((NvNotification *) (&(ppdev->Notifiers->DmaToMem[NVFF9_NOTIFIERS_COLORMAP_DIRTY_NOTIFY*sizeof(NvNotification)])))->status = NVFF9_NOTIFICATION_STATUS_DONE_SUCCESS;

    ppdev->NvLastIndexedImageNotifier = 0;

    for(i = 0; i < MAX_INDEXED_IMAGE_NOTIFIERS; i++)
        ((NvNotification *) (&(ppdev->Notifiers->DmaToMemIndexedImage[i*sizeof(NvNotification)])))->status = NV060_NOTIFICATION_STATUS_DONE_SUCCESS;

    return(TRUE);
}

BOOL NvSetupHwObjects(PDEV * ppdev)
{

    ULONG i;

    NV_CREATE_OBJECT_SETUP();

    //**************************************************************************
    // Create misc objects that we need
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_DMA1(DD_ROP5_SOLID,                NV03_CONTEXT_ROP);
    NV_CREATE_OBJECT_DMA1(NV_DD_CONTEXT_BETA4,          NV04_CONTEXT_BETA);
    NV_CREATE_OBJECT_DMA1(MY_MEMORY_TO_MEMORY,          NV03_MEMORY_TO_MEMORY_FORMAT);
    NV_CREATE_OBJECT_DMA1(DD_IMAGE_BLACK_RECTANGLE,     NV01_IMAGE_BLACK_RECTANGLE);
    NV_CREATE_OBJECT_DMA1(DD_IMAGE_PATTERN,             NV04_CONTEXT_PATTERN);
    NV_CREATE_OBJECT_DMA1(DD_IMAGE_SOLID,               NV04_CONTEXT_COLOR_KEY);
    if (NV_CREATE_OBJECT_FAIL()) {
        DISPDBG((0, "NVDD: Cannot allocate misc objects"));
        return(FALSE);
    }

    //**************************************************************************
    // Create INDEXED_IMAGE_FROM_CPU objects
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    for(i = 0; i < MAX_INDEXED_IMAGE_NOTIFIERS; i++)
    {
        NV_CREATE_OBJECT_DMA2(DD_INDEXED_IMAGE1_FROM_CPU + i,
                          NV05_INDEXED_IMAGE_FROM_CPU,NV04_INDEXED_IMAGE_FROM_CPU)
    }
    if (NV_CREATE_OBJECT_FAIL()) {
        DISPDBG((0, "NVDD: Cannot allocate INDEXED_IMAGE_FROM_CPU"));
        return(FALSE);
    } else {
        ppdev->CurrentClass.IndexedImageFromCPU = NV_CREATE_OBJECT_CLASS();
    }

    //**************************************************************************
    // Create CONTEXT_SURFACES_2D object
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_DMA2(DD_PRIMARY_IMAGE_IN_MEMORY,
                          NV10_CONTEXT_SURFACES_2D,NV04_CONTEXT_SURFACES_2D);
    if (NV_CREATE_OBJECT_FAIL()) {
        DISPDBG((0, "NVDD: Cannot allocate CONTEXT_SURFACES"));
        return(FALSE);
    } else {
        ppdev->CurrentClass.ContextSurfaces2D = NV_CREATE_OBJECT_CLASS();
    }

    //**************************************************************************
    // Create IMAGE objects
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_DMA2(DD_IMAGE_BLIT,           NV15_IMAGE_BLIT,NV04_IMAGE_BLIT);
    NV_CREATE_OBJECT_DMA1(DD_RENDER_RECT_AND_TEXT, NV04_GDI_RECTANGLE_TEXT);
    NV_CREATE_OBJECT_DMA1(DD_IMAGE_ROP_AND_LIN,    NV04_RENDER_SOLID_LIN);
    NV_CREATE_OBJECT_DMA2(DD_IMAGE_FROM_CPU,
                          NV05_IMAGE_FROM_CPU,NV04_IMAGE_FROM_CPU);

    if (NV_CREATE_OBJECT_FAIL()) {
        DISPDBG((0, "NVDD: Cannot allocate IMAGE objects"));
        return(FALSE);
    }

    //**************************************************************************
    // Create SCALED_IMAGE object used by ESC_NV_DESKMGR_ZOOMBLIT
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_DMA1(DD_SCALED_IMAGE_FROM_MEMORY,
                          NV10_SCALED_IMAGE_FROM_MEMORY);

    if (NV_CREATE_OBJECT_FAIL()) {
        ppdev->CurrentClass.ScaledImageFromMemory = 0;
    } else {
        ppdev->CurrentClass.ScaledImageFromMemory = NV_CREATE_OBJECT_CLASS();
    }


    //**************************************************************************
    // Create objects needed to to a color key transparent blit 
    // for the overly merge blit. Only supported in 16 or 32 bpp
    //**************************************************************************
    if (ppdev->cjPelSize>1)
    {
        ASSERT((2==ppdev->cjPelSize) || (4==ppdev->cjPelSize));

        NV_CREATE_OBJECT_INIT();
        NV_CREATE_OBJECT_DMA1(DD_IMAGE_BLIT_COLORKEY, NV04_IMAGE_BLIT);

        if (NV_CREATE_OBJECT_FAIL()) 
        {
            DISPDBG((0, "Cannot allocate DD_IMAGE_BLIT_COLORKEY overlay merge blit objects"));            
            return(FALSE);
        }
    }

    //**************************************************************************
    // Create ALPHA IMAGE FROM MEMORY object used for AlphaBlended CopyBlt
    //**************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_DMA2(DD_ALPHA_IMAGE_FROM_MEMORY,
                          NV10_SCALED_IMAGE_FROM_MEMORY, NV04_SCALED_IMAGE_FROM_MEMORY);

    if (NV_CREATE_OBJECT_FAIL()) 
    {
        DISPDBG((0, "Cannot allocate DD_ALPHA_IMAGE_FROM_MEMORY object"));            
        return(FALSE);
    } else {
        ppdev->CurrentClass.AlphaImageFromMemory = NV_CREATE_OBJECT_CLASS();
    }

    return(TRUE);
}


BOOL bClassSupported(PDEV *ppdev, NvU32 classID)
{
    unsigned int i=0;
    while (i < ppdev->nvNumClasses) {
        if (ppdev->nvClassList[i] == classID) {
            return (TRUE);
        }
        i++;
    }
    return (FALSE);
}



//******************************************************************************
//
//  Function:   NvCreateObject
//
//  Routine Description:
//      Creates a DMA or PIO object given an array of classes.  Walks down array and
//      determines which class is supported by the current hardware.  Once it
//      finds a supported class, it allocates the object.
//      We use this to create the highest known class of a particular type. 
//
//      ie.  NV_CONTEXT_SURFACES_2D which try first with NV10_CONTEXT_SURFACES_2D 
//      and then NV04_CONTEXT_SURFACES_2D
//
//  Arguments:
//      A bunch
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NvCreateObject(PDEV *ppdev,
                    BOOL bCreateDMA,
                    PVOID parms, 
                    NvU32 obj, 
                    NvU32 *createObjClass,
                    NvU32 *createObjFail,
                    int numclasses, 
                    NvU32 *classes) {
    
    int i;                                                                    
    ULONG channel;

    if (bCreateDMA) {
        channel = ppdev->hDmaChannel;
    } else {
        channel = ppdev->hPioChannel;
    }
    *createObjFail |= 1;  // Fail by default
    *createObjClass = 0; 
    for (i=0; i<numclasses; i++) {                      
        if (bClassSupported(ppdev,classes[i])) {
            *createObjClass = classes[i];                                          
                if (NvAlloc(ppdev->hDriver,                                  
                            ppdev->hClient,                                  
                            channel,
                            (obj),                                           
                            (*createObjClass),
                            (parms)) == NVOS05_STATUS_SUCCESS){   
                    *createObjFail &= ~(0x1);                                            
                }                                                                     
            break;
        }
    }                                                                         
    *createObjFail <<= 1;                                                      
}





//******************************************************************************
//
//  Function:   vCalibrate_XferSpeeds
//
//
//  Routine Description:
//
//      This routine calibrates the xfer speeds of the CPU transfers,
//      the DMA xfers, the DMA hardware overhead, the CPU overhead to
//      setup a xfer.  The routine then combine these values into a 
//      single factor in pdev (globalXferOverheadFactor) that will be
//      used in the NVScreenToMem routine to decide which size to take
//      for the xfer chunks. ( The details are in the math. model )
//
//           - Stephane
//
//  Arguments:
//
//      Ptr to PDEV describing the physical device.
//
//  Return Value:
//
//      None.
//
//  Note:
//
//      **IMPORTANT**  This routine include (copy pasted) the actual code of
//                     the function call NV4_DmaPushSend.  This means that
//                     ANY changes to NV4_DmaPushSend must be reflected in
//                     here.  (This is not very good, but this function needs
//                     this to be more accurate)
//
//******************************************************************************


VOID vCalibrate_XferSpeeds(PDEV *ppdev)
{ 
    //***[ For calibration routine ]***[CALIBLOCALS]*****
    
    #define NV_MTMF_FORMAT_INPUT_INC_1     0x00000001
    #define NV_MTMF_FORMAT_OUTPUT_INC_1    0x00000100
    
    ULONG               j;
    
    LONGLONG            llCntrFreq;
    LONGLONG            llTimeStamp1;
    LONGLONG            llTimeStamp2;
    
    BYTE*               SysMemBuffer;
    BYTE*               limit;
    BYTE*               pjFinalDstWrite;
    BYTE*               pjDmaRead;
    BYTE*               pjDmaRead_start;
    
    ULONG               bytesPerLine_max;
    ULONG               bytesPerLine;
    ULONG volatile      volatileDummy;
    BYTE * volatile     volatileDummyPtr;
    ULONG               linesPerXfer;
    ULONG               SrcDMAOffset;
    ULONG               DstDMAOffset;
    ULONG               DstDMA2ndPartOffset;
    ULONG               SrcDMAincPerXfer;
    ULONG               linesToGo;
    
    LONG                lSrcPitch;
    LONG                lDstPitch;
    LONG                FinalDstJump2NextLineBeg;
    
    double              dfX;
    double              dfY;
    double              dfOH;
    double              dfK;
    double              dfTime;
    double              dfFactor;
    
    Nv04ControlDma      *nvDmaChannel;
    NvNotification      *pNotifier;
    
    DECLARE_DMA_FIFO;               // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)
        
    //***[ End of locals for calibration routine ]*******
    //*********************************[CALIBLOCALSEND]**

       
    
    
    
   
    //**************************************************************************
    //*[ Profiling Stuff BEGIN ]***[CALIBSTART]*********************************
    //**************************************************************************   
    //************************************************************************** 
    //
    //               SPEED CALIBRATION FOR CONCURENT DMA/CPU XFERS
    //                              
    //                                  - Stephane
    //
    //************************************************************************** 
    // Setup calibration data and calculate and store a "special" num
    // in ppdev->globalXferOverheadFactor.  (There is mathematical/algorithmic
    // details that are not explained in this function, ask me for details)
    // 
    //
    // We have to calculate 
    //
    // X     ( DMAXfer speed )
    // Y     ( DMAXfer speed )
    // OH    ( overhead between issuing command, and DMAXfer actually start and
    //         get into high (well, constant plateau speed ))
    // k     ( overhead of setting up a xfer )
    //
    //
    //  To approximate OH we kick a very small Xfer, and to approximate X,
    //  we set a long transfer, time it, substract OH from time, and compute 
    //  bytes/time.  To approximate Xaj, we use data from long xfer without
    //  substracting OH.
    //
    
    
    
    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    
    //**************************************************************************
    //                                                                         *
    // Get push buffer global information (from pdev) into local variables
    //
    //************************************************************************* 
    
    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
    
    //*                                                                        *
    //**************************************************************************
    
    
    if ( ppdev->pfnScreenToMemBlt != NULL ) // if this function can be called!
    {                                       // or else profiling data is useless
                                            // and moreover, if we are on NV3,
                                            // there is no DMA push buffer
        
        // Make sure our Mem2Mem object is in spare subchannel
        
        NV_DMAPUSH_CHECKFREE(((ULONG)(2)));
    
        if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY)
        {
            NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
            ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
            
            UPDATE_PDEV_DMA_COUNT;   //
            NV4_DmaPushSend(ppdev);  // Flush now, to avoid overhead interfering with calibration
        }
    
    
        //**********************************************************************
        //  Good. We are on NV4 or above, and this function is present. 
        //  We are going to setup variables needed to execute a Vram to SysMem 
        //  Xfer via an intermediate buffer (the DMA locked Mem2Mem buffer)
        //  We need to allocate system memory to simulate a destination bitmap
        //  of a destination region. But how big do we create this region? See:
        //**********************************************************************
    
        //  NOTE to get more "accurate" calibration results.
        //  ----------------------------------------------------------
        //
        //        Either prevent optimization of compiler with a special pragma,
        //        OR prevent optimization by setting data type qualifier to VOLATILE
        //        OR make a dummy function accepting all these parameters by adress,
        //        that do not modify them, so that the compiler flag these vars as
        //        being "unoptimizable" due to use (in the call to the dummy()) 
        //        between initialisation and the use of their value here (below)
        //        
        //        The reason why we want to prevent this is to be more precise in the
        //        profiling: if NV_DMA_FIFO == 1, this might be translated as something
        //        like
        //                      -> adress of dma fifo + dmacount (see dmamacro.h)
        //                     |
        //              mov [ebx],1              ; value appended as immediate operhand in
        //                                       ; actual opcode.
        //
        //        instead of
        //
        //              mov eax, [ebp + ??]      ; put local value into reg
        //              mov [ebx],eax            ; put local value into fifo
        //
        //        The following variables are affected by this remark:
        //        (see Nv4ScreenToMemBlt in nv4bltc.c )
        //        
        //        SrcDMAOffset          : Because it is based on the pjSrcBits which
        //                                is based on the src surface we normally 
        //                                receive in Nv4ScreenToMemBlt
        //
        //        lSrcPitch             : (same thing)
        //        bytesPerLine          : (based on the format of the region we get...)
        //        linesPerXfer          : (based on the optimum byte per xfer runtime
        //                                 algo - this calibration is for that algo!)
        //        linesToGo             : (used in the real loop of the xfer per chunk,
        //                                 even tough here it may never be used, and
        //                                 could be pulled out of this code by the 
        //                                 compiler) BUT SEE NOTE BELOW
        //        
        //        SrcDmaincPerXfer      : This don't change in the loops, but still, the
        //                                values must NOT be included in the ADD opcode
        //                                as immediates by the compiler, because in reality,
        //                                (in NV4ScreenToMemBlt), this would really be
        //                                computed runtime.
        //
        // 
        //
        //        FinalDstJump2NextLineBeg :  ( same thing here)
        //
        //        By the way, pjDmaRead is NOT affected, because we use it as in the "real"
        //        case, that is, it gets = to pdev->MemToMemBuffer
        //
        //  NOTE: There might be some unintended side effects by using Volatile, in that
        //        if we use directly the volatile variables in the loop, those might not
        //        be "cached" into registers.  One way to avoid this is to declare dummy
        //        "mirror" volatile variables, assign the static value to them, and then,
        //        init the normal variables with the content of these "mirror" variables.
        //        That way, the compiler has no choice but to take into account that
        //        these variables MUST be used and the compiler cannot assume any value
        //        at one point in the program after their assignation, since the assignation
        //        values were provided by volatiles, which can, by definition, have any 
        //        values any time.
        //        For linesToGo, it is tricky: we don't want to put THIS variable volatile
        //        to prevent unwanted side effects, but we HAVE to use it or else compiler
        //        will remove it.  So one way to prevent this is to assign a volatile to
        //        the value of linesToGo.
        //
        //  The best solution seems to be to use the "mirror-volatile-init-method" because
        //  disabling optimization could impact the calibration because our "real" code in
        //  ScreenToMem blt IS optimized by the compiler.
        //
    
        
        
        EngQueryPerformanceFrequency(&llCntrFreq);  // get the counter frequency
    
        ppdev->pfnWaitEngineBusy( ppdev );          // because we want to time the overhead 
                                                    // of processing the stuff comming next,
                                                    // not including previous commands that
                                                    // might be still pending or running...
    
        UPDATE_LOCAL_DMA_COUNT;                     // !!!RESYNC the local fifo count because
                                                    // wait busy modifies them!!!!
        
        
        // Find an arbitrary bitmap size (height*width) fitting in the
        // mem2mem buffer by taking the squareroot of its size.
        // we will still limit the transfer to 3.5 MB because we don't
        // know how much vram the card has (well we know but where
        // in the driver this info is stored ? ) Anyway, no card has
        // a framebuffer less than 4 MB, (but to be safe, we're going
        // to put about 3.5 meg, just in case (instance memory ? ) )
        //
        // We take the squareroot for bytesperline because we are
        // going to use that same number as our number of lines
        //
    
        if ( ppdev->MemToMemBufferSize > 3655744 )
        {
            bytesPerLine_max = 1912; // clamp to sqrt(3.486 Meg)                                                                
        }
        else // else use mem2mem buffer size since it is < 3.5 meg
             // (we dont want to use 1912, because mem2mem is smaller
             // than 3.5 meg so we would corrupt mem past the buffer)
             // in other words, take Minimum( 1912, sqrt(memtomembuffersize) )
        {
            bytesPerLine_max = (ULONG) sqrt( ppdev->MemToMemBufferSize ) ;
        }
                         
        //
        // From here on, we will use bytesPerLine as the width also, so that
        // the number (max) of lines we can copy is bytesPerLine also. (considering
        // the "surface" being 8bpp)
        //
    
    
        // Allocate the system memory buffer that will simulate the bits of
        // a destination region in unlocked system ram.
    
        
        SysMemBuffer = (BYTE *) EngAllocMem(0, bytesPerLine_max*bytesPerLine_max, ALLOC_TAG);
    
        if( SysMemBuffer == NULL )
        {
            DISPDBG((1, "DrvEnableSurface - Failed EngAllocMem for mem buffer in calibration section"));
            goto CalibXfer_Error;
        }
        
    
        //**********************************************************************
        //
        //                          OH CALIBRATION
        //                **************************************
        //
        // OH is the overhead time of the hardware: the time the hardware takes
        //    to pull from the DMA push buffer and execute the command ( and
        //    finally start the xfer)
        //
        //
        // Setup variables for a very small DMA Xfer (1 byte) from VRAM start
        // to DMA Mem2Mem buffer
        // (remember the variables are all dummies, we do this for 
        // calibration; all we have to make sure is that there is no access
        // violation of course...)
        //
        // Note that for this first transfer we don't calibrate "K" so it doesnt
        // mather if we use the volatiles to init our variables.
        //
        //**********************************************************************
     
        //---[Set the number of bytes per line of the region to Xfer]-----------
    
        volatileDummy    = 1;                   // set the volatile
        bytesPerLine     = volatileDummy;       // assign by volatile to prevent
                                                // optimization
        
        //---[Set the number of lines per Xfer ]--------------------------------
        //
        //                                   (normally based on OptBytesPerXfer)
    
        volatileDummy    = 1;
        linesPerXfer     = volatileDummy;
        
        
        //---[Set Source/FinalDest Pitch and Address of bits (and inc for 
        //                                                    FinalDest Ptr)]---  
    
        // Don't need this for this for calibration
        //
        //pjSrcBits    = ppdev->pjScreen;      // Assume VRAM Base == pjScreen
                                             // (== base of primary surface)
    
        
        volatileDummy = bytesPerLine;
        lSrcPitch     = volatileDummy;       // set source pitch == bytesPerLines 
                                             // (no gap in mem btw ending of lines and beg.
                                             // of next lines)
                
        //   Don't need these for this first calibration xfer
        //
        //volatileDummy = 1;                
        //lDstPitch     = volatileDummy;
    
        
        //FinalDstJump2NextLineBeg = lDstPitch - bytesPerLine;
    
    
    
        //---[Mem2Mem DMA Buffer ReadPtr (to FinalDest) (it's inc == 0)]--------  
                
                
        //  Don't need this for this first calibration xfer
        //
        //pjDmaRead = ppdev->pMemToMemBuffer;  // Init the reading ptr for the second
                                             // transfer "pipe" (the CPU-Xfers)
        
    
        //---[Convert the SRC offset in DMA Offset (and calculate inc. 
        //                                                  after each Xfer)]---
        
        
        volatileDummy    = 0;           // Assume ppdev->pjScreen == Vram DmaContext Base
        SrcDMAOffset     = volatileDummy;
    
        DstDMAOffset = 0;           // Xfer the bytes to start of mem2mem buffer
        
        //  Don't need this for this first calibration xfer
        //
        //                                                      // dont need to "volatile
        //SrcDMAincPerXfer   = linesPerXfer * lSrcPitch;        // mirror" these, because
                                                                // linesPerXfer has already
                                                                // been volatile-assigned.
    
        //---[Set the total number of lines to Xfer (Xfer_siz/bytesPerLine)]----
        
        
        //  Don't need this for this first calibration xfer
        //
        //linesToGo = 1;                       // Init the number of lines left to transfer
    
          
        //***[ Push commands in the DMA Fifo for the 1 byte Xfer ]**************
        
    
        NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  
    
        NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
        NV_DMA_FIFO = SrcDMAOffset;                    // Offset In
        NV_DMA_FIFO = DstDMAOffset;                    // Offset Out
        NV_DMA_FIFO = lSrcPitch;                       // Pitch In
        NV_DMA_FIFO = bytesPerLine;                      // Pitch Out == bytesPerLine because
                                                         // we dont want any memory "hole" between
                                                         // two contiguous lines in the mem2mem buffer
        NV_DMA_FIFO = bytesPerLine;                    // Line Length in bytes
        NV_DMA_FIFO = linesPerXfer;                    // Line Count (num to copy)
        NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1; // Format
        NV_DMA_FIFO = 0;                               // Set Buffer Notify and kickoff
                
        // Set notifier to 'BUSY' value (use second notification structure)
        
        pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
        
        UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
                        
        
        // Don't need that, we don't calibrate that yet
        //
        //
        //linesToGo    -= linesPerXfer;          // Susbtract the lines that have been done.
        //SrcDMAOffset += SrcDMAincPerXfer;      // Advance in the source DMA region
        //DstDMAOffset  = DstDMA2ndPartOffset;   // Use the other part of dst DMA region (mem2mem buffer)
                        
                
        // Intstead of that:
        //
        //NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer
        //
        // We will copy paste the function in here so that we can get the time
        // stamp just before the hardware starts processing the FIFO
    
        //  THIS IS COPY PASTED  nv4hw.c BECAUSE WE WANT TO START PROFILING
        //  JUST BEFORE THE LAST STATEMENT OF THIS FUNCTION
    
        nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

        //**********************************************************************
        // Check if the ALI 1541 chipset is present.
        // If so, then we need to workaround a cache issue by doing an out to port
        //**********************************************************************
        
        if (ppdev->AliFixupIoNeeded)
            {
        // Assembly is not feasible for IA64
        #ifndef _WIN64
            outp(ppdev->AliFixupIoBase, 0);      // This will flush the cache
        #endif
            }
        
        
        #ifdef NVD3D    // DX6
        //**********************************************************************
        // If there is another DMA push channel active, wait til it's finished.
        //
        // It hurts performance to check this with every kickoff, so we only do
        // this check when:
        // 1) This is a DX6 build. We can only successfully sync with other DMA
        //    Push Channels and not PIO channels (DX3 is PIO only).
        // 2) DirectDraw is enabled and ddraw objects have been created
        //   (determined by checking if surfaces have been created).
        //
        //**********************************************************************
        
        if ((ppdev->DDrawEnabledFlag) &&
            (ppdev->pDriverData->DDrawVideoSurfaceCount))
            {
            if (ppdev->pfnWaitForChannelSwitch)
                ppdev->pfnWaitForChannelSwitch(ppdev);
            }
        #else
        //**********************************************************************
        // It's possible the DX3 PIO channel is still active. We need to wait
        // til it's finished. It hurts performance to check this with every
        // kickoff, so we only do this check when DirectDraw is enabled.
        //**********************************************************************
        if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
            {
            NV4_DdPioSync(ppdev);
            ppdev->bDDChannelActive = FALSE;
            }
        #endif // #ifndef NVD3D
        
        //**************************************************************************
        // Convert the PUT ptr to a byte address
        //**************************************************************************
        
        ppdev->nvDmaCachedPut = ppdev->nvDmaCount << 2;
        
        //**************************************************************************
        // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
        // Then 'OR' it near the end of the push buffer
        //**************************************************************************
        
        if (ppdev->nvDmaCount >0)
            ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
        else
            ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];
        
        *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;
        
        //**************************************************************************
        // Tell the hardware to start processing the push buffer
        //**************************************************************************
        
        //**********************************************************************
        //  Here, just before kicking this, we snoop the Eng counter 
        //**********************************************************************
        
        EngQueryPerformanceCounter(&llTimeStamp1);
    
        nvDmaChannel->Put = ppdev->nvDmaCachedPut;
        
        
        // Wait for the DMA-Xfer to complete (use second notification structure)
    
        while ( pNotifier->status != 0 );
    
    
        // The 1 byte Transfer is completed, get the timestamp
    
        EngQueryPerformanceCounter(&llTimeStamp2);
        
        dfOH = (double)(llTimeStamp2 - llTimeStamp1)/(double)(llCntrFreq) ; // get time in seconds;
        
        if ( dfOH == 0 ) 
            goto CalibXfer_Error;
        
        
        
        //**********************************************************************
        //
        //                      K,X, and Y  CALIBRATION
        //                **************************************
        //
        // Setup variables for the largest DMA xfer we can have.
        //
        //**********************************************************************
        
    
    
        //---[Set the number of bytes per line of the region to Xfer]-----------
    
        // bytesPerLine_max must be run time calculated so don't need to use
        // the volatile trick here ? Need to use it so we have a different
        // local variable created, otherwise compiler will never really create
        // "bytesPerLine" and only bytesPerLine_max
        
        volatileDummy    = bytesPerLine_max;        // see init of bytesPerLine_max
        bytesPerLine     = volatileDummy;
                                            
        
        //---[Set the number of lines per Xfer ]--------------------------------
        //
        //                                   (normally based on OptBytesPerXfer)
    
        volatileDummy    = bytesPerLine_max;        // see init of bytesPerLine_max
        linesPerXfer     = volatileDummy;
        
        
        //---[Set Source/FinalDest Pitch and Address of bits (and inc for 
        //                                                    FinalDest Ptr)]---  
    
        
        volatileDummy = bytesPerLine;
        lSrcPitch     = volatileDummy;       // set source pitch == bytesPerLine 
                                             // (no gap in mem btw ending of lines and beg.
                                             // of next lines)
                
        volatileDummy = bytesPerLine;        // Final destination pitch (our sys MEM        
        lDstPitch     = volatileDummy;       // cached surface (unlocked))
    
        
        FinalDstJump2NextLineBeg = lDstPitch - bytesPerLine;  // inc to jump a beg of next line
                                                              // when at the end off the preceding
                                                              // one
    
        //---[Mem2Mem DMA Buffer ReadPtr (to FinalDest) (it's inc == 0)]--------  
                
                
        pjDmaRead       = ppdev->pMemToMemBuffer;  // Init the reading ptr for the second
                                                   // transfer "pipe" (the CPU-Xfers)
        
        volatileDummyPtr= (BYTE *)(ppdev->pMemToMemBuffer);
        
        pjDmaRead_start = volatileDummyPtr;

        pjFinalDstWrite = SysMemBuffer;      // Where to write our CPU Xfers
    
    
        //---[Convert the SRC offset in DMA Offset (and calculate inc. 
        //                                                  after each Xfer)]---
        
        
        volatileDummy    = 0;           // Assume ppdev->pjScreen == Vram DmaContext Base
        SrcDMAOffset     = volatileDummy;
    
        DstDMAOffset        = 0;               // Xfer the bytes to start of mem2mem buffer
        
        DstDMA2ndPartOffset = volatileDummy;   // 2nd part of mem2mem buffer (dummy, we don't care)
        
                                                                // dont need to "volatile
        SrcDMAincPerXfer   = linesPerXfer * lSrcPitch;          // mirror" these, because
                                                                // linesPerXfer has already
                                                                // been volatile-assigned.
    
        //---[Set the total number of lines to Xfer (Xfer_siz/bytesPerLine)]----
                
        volatileDummy = bytesPerLine_max;    // see init of bytesPerLine_max
        linesToGo     = volatileDummy;       // Init the number of lines left to transfer
    
          
        //***[ Push commands in the DMA Fifo for the big Xfer ]*****************
        
        EngQueryPerformanceCounter(&llTimeStamp1);  // Get time stamp first (for K)
    
    
        NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  
    
        NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
        NV_DMA_FIFO = SrcDMAOffset;                    // Offset In
        NV_DMA_FIFO = DstDMAOffset;                    // Offset Out
        NV_DMA_FIFO = lSrcPitch;                       // Pitch In
        NV_DMA_FIFO = bytesPerLine;                      // Pitch Out == bytesPerLine because
                                                         // we dont want any memory "hole" between
                                                         // two contiguous lines in the mem2mem buffer
        NV_DMA_FIFO = bytesPerLine;                    // Line Length in bytes
        NV_DMA_FIFO = linesPerXfer;                    // Line Count (num to copy)
        NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1; // Format
        NV_DMA_FIFO = 0;                               // Set Buffer Notify and kickoff
                
        // Set notifier to 'BUSY' value (use second notification structure)
        
        pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
        
        UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
                        
                
        NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer
        
                
        // We now need the following, we calibrate K
        //
        //
        linesToGo    -= linesPerXfer;          // Susbtract the lines that have been done.
        SrcDMAOffset += SrcDMAincPerXfer;      // Advance in the source DMA region
        DstDMAOffset  = DstDMA2ndPartOffset;   
        
        // we also include the following, even if in the actual code, it's not in the same place,
        // but because it still has to be executed for each cpu xfers, we include this in K

        pjDmaRead = pjDmaRead_start;

    
        EngQueryPerformanceCounter(&llTimeStamp2);  // Get time stamp (for K)
                
        
        dfK = (double)( llTimeStamp2 - llTimeStamp1 ) / (double)(llCntrFreq) ; // get K in seconds
        
        if ( dfK == 0 ) 
            goto CalibXfer_Error;
        
        
        // Note that here, the EngQuery have overhead, and if the Xfer is too
        // fast, we might wrongly calibrate X if we do the while(!notified) and 
        // get a third timestamp right here. Instead, we are going to kick the xfer
        // another time
    
        // Make sure the variables are used in our program so compiler don't
        // warn us or remove the above (useless) statements
    
        volatileDummy = linesToGo;        // dummy...
        volatileDummy = SrcDMAOffset;
        volatileDummy = DstDMAOffset;
        
        
        // Wait for the DMA-Xfer to complete (use second notification structure)
    
        while ( pNotifier->status != 0 );
    
    
    
        //***[ Setup another Xfer to calibrate X ]**********************************
        
        
        volatileDummy    = 0;           // Assume ppdev->pjScreen == Vram DmaContext Base
        SrcDMAOffset     = volatileDummy;
    
        DstDMAOffset = 0;               // Xfer the bytes to start of mem2mem buffer
        
        volatileDummy = bytesPerLine_max;  // see init of bytesPerLine_max
        linesToGo     = volatileDummy;     // Init the number of lines left to transfer
    
    
        //***[ Push commands in the DMA Fifo for the 2nd big Xfer ]*************
        
        NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  
    
        NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
        NV_DMA_FIFO = SrcDMAOffset;                    // Offset In
        NV_DMA_FIFO = DstDMAOffset;                    // Offset Out
        NV_DMA_FIFO = lSrcPitch;                       // Pitch In
        NV_DMA_FIFO = bytesPerLine;                      // Pitch Out == bytesPerLine because
                                                         // we dont want any memory "hole" between
                                                         // two contiguous lines in the mem2mem buffer
        NV_DMA_FIFO = bytesPerLine;                    // Line Length in bytes
        NV_DMA_FIFO = linesPerXfer;                    // Line Count (num to copy)
        NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1; // Format
        NV_DMA_FIFO = 0;                               // Set Buffer Notify and kickoff
                
        // Set notifier to 'BUSY' value (use second notification structure)
        
        pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
        
        
        UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count                                        
        // PushSend:
        //
        //  THIS IS COPY PASTED  nv4hw.c BECAUSE WE WANT TO START PROFILING
        //  JUST BEFORE THE LAST STATEMENT OF THIS FUNCTION
    
        nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

        //**********************************************************************
        // Check if the ALI 1541 chipset is present.
        // If so, then we need to workaround a cache issue by doing an out to port
        //**********************************************************************

        if (ppdev->AliFixupIoNeeded)
            {
        // Assembly is not feasible for IA64
        #ifndef _WIN64
                outp(ppdev->AliFixupIoBase, 0);      // This will flush the cache
        #endif
            }
    
    
        #ifdef NVD3D    // DX6
        //**********************************************************************
        // If there is another DMA push channel active, wait til it's finished.
        //
        // It hurts performance to check this with every kickoff, so we only do
        // this check when:
        // 1) This is a DX6 build. We can only successfully sync with other DMA
        //    Push Channels and not PIO channels (DX3 is PIO only).
        // 2) DirectDraw is enabled and ddraw objects have been created
        //   (determined by checking if surfaces have been created).
        //
        //**********************************************************************

        if ((ppdev->DDrawEnabledFlag) &&
            (ppdev->pDriverData->DDrawVideoSurfaceCount))
            {
            if (ppdev->pfnWaitForChannelSwitch)
                ppdev->pfnWaitForChannelSwitch(ppdev);
            }
        #else
        //**********************************************************************
        // It's possible the DX3 PIO channel is still active. We need to wait
        // til it's finished. It hurts performance to check this with every
        // kickoff, so we only do this check when DirectDraw is enabled.
        //**********************************************************************
        if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
            {
            NV4_DdPioSync(ppdev);
            ppdev->bDDChannelActive = FALSE;
            }
        #endif // #ifndef NVD3D

        //**************************************************************************
        // Convert the PUT ptr to a byte address
        //**************************************************************************

        ppdev->nvDmaCachedPut = ppdev->nvDmaCount << 2;

        //**************************************************************************
        // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
        // Then 'OR' it near the end of the push buffer
        //**************************************************************************

        if (ppdev->nvDmaCount >0)
            ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
        else
            ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

        *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;
        
        //**************************************************************************
        // Tell the hardware to start processing the push buffer
        //**************************************************************************
        
        //**********************************************************************
        //  Here, just before kicking this, we snoop the Eng counter 
        //**********************************************************************
        
        EngQueryPerformanceCounter(&llTimeStamp1);
    
        nvDmaChannel->Put = ppdev->nvDmaCachedPut;
        
        
        // Wait for the DMA-Xfer to complete (use second notification structure)
    
        while ( pNotifier->status != 0 );
    
    
        // The big transfer is completed, get the timestamp
    
        EngQueryPerformanceCounter(&llTimeStamp2);
        
        // Problem: We don't know how big dfTime can be: this is dependant of the counter
        //          frequency. If it is too big, we will loose all precision in the totalbytes/dfTime
        //          division.  So, the division must not, at first come lower than the smallest positive,
        //          that can be represented by a double, 4.94065645841247E-324 .  Even then, we must check
        //          the operations AFTER that concern X. We will calculate, at the end, sqrt(X*OH) or
        //          sqrt(Y*K).  K and OH are at least one, so sqrt(X) and sqrt(Y) must not be lower than
        //          4.94065645841247E-324, so X and Y should be >= (4.94065645841247E-324 )^2 (but this is
        //          even lower than 4.94065645841247E-324 so the latter is our boundary. 
        //          
        //          (We could also convert the ticks in seconds, with counter frequency, and then divide 
        //          the size of the transfer by the number of seconds it took.  We know that this can't be 
        //          lower than 1 byte (even 1k, even 1 MB ! )   )
      
        dfTime = ( (double)( llTimeStamp2 - llTimeStamp1) - dfOH ) / (double)(llCntrFreq) ; // get time in seconds
    
        if( dfTime == 0 ) // never supposed to happen but...
        {
            // OH is of the order of the time of the xfer, bailout, disable overlapping
            // of Xfers
            goto CalibXfer_Error;
        }
        
        
        dfX = (double)( bytesPerLine * bytesPerLine ) / dfTime ;
        
        if ( dfX <= 4.94065645841247E-324 ) 
            goto CalibXfer_Error;  // practically, dfX would be >>> 1 ! 
                        
        
        //**********************************************************************
        //
        //                           Y  CALIBRATION
        //                **************************************
        //
        // CPU Xfer the big Xfer we just DMA-Xfered.
        //
        //**********************************************************************
    
        
        EngQueryPerformanceCounter(&llTimeStamp1);
    
        for(j=0; j < linesPerXfer; j++)
        {            
                    
            // Xfer a whole line
            //
            for(limit = pjDmaRead + bytesPerLine; 
                pjDmaRead < limit; 
                pjDmaRead++, pjFinalDstWrite++ )
            {
                *pjFinalDstWrite = *pjDmaRead ;
            }
            pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                         // next line in the dest surface
    
        } // ...for all bytes in chunk to cpu-xfer
    
        EngQueryPerformanceCounter(&llTimeStamp2);
    
                
        
        dfTime = (double)( llTimeStamp2 - llTimeStamp1 ) / (double)(llCntrFreq) ; // get time in seconds
    
        if( dfTime == 0 ) 
            goto CalibXfer_Error;
                
        dfY = (double)( bytesPerLine * bytesPerLine ) / dfTime ;
        
        if ( dfY <= 4.94065645841247E-324 ) 
            goto CalibXfer_Error;  // practically, dfY would be >>> 1 ! 
    
        // One remark here: we could combine this last if with the if( dfTime == 0)
        // same thing goes for calculation of dfX (see before)...
    
    
        //**********************************************************************
        //
        //               globalXferOverheadFactor CALCULATION
        //             ****************************************
        //
        //  Now that we have X, Y, K, OH, all is left to do is to combine 
        //  (choose which data to use) the calibration data into one value,
        //  the OverheadFactor.  This is were the real model I have designed
        //  and the actual implementation diverge:  this has been done for 
        //  speed purpose of course, and anyway, with experimentation in MatLab,
        //  I have showed that this approximation of the dependant variable
        //  that give the "maxima" of the piecewise optimization function, gives
        //  pretty good results.  Otherwise, if we wanted to have the REAL
        //  optimal block size 100% of the time (I say 100% because the appro-
        //  ximation computation == the optimal anyway), we would be forced to
        //  find the maximum of a piecewise function (made of 2 analytical 
        //  functions and 1 junction).  This implies calculating 2 analytical
        //  maxima, the maxima of the junction (boundarie of the two pieces)
        //  and finally take the max of all those max. (For each Xfer of bitmaps
        //  region) This is unacceptable !
        //  Even now, with this approximation, I end up calculating one maxima
        //  by plugging values in the derivative of an analytical function. This
        //  implies calculating 1 MUL, and 1 SQRT (but the SQRT is optimized, it
        //  is not a CPU-SQRT). Thus...
        //
        //  TODO (would be good to try): instead of calc. the SQRT of Transfer 
        //  size * globalXferOverheadFactor (see nv4bltc.c NV4ScreenToMemBlt),
        //  fill a lookup table with common xfer size VS the opt xfer size, using
        //  the REAL model.  The "common xfer sizes" could be the middle of each
        //  power of 2. (Pretty much like I did the LUT to be able to compute a
        //  SQRT with Newton's method with only 2 iterations).
        //
        //  Of course, before trying this, we would have to see what is the
        //  maximum error we can commit for S (block size of xfer) for each
        //  value in the lookup (each interval) and see if it's worth it. If it's
        //  not, do a linear interpolation ?  (but this bring us back to the sqrt
        //  because Newton's method implies iteration similar to the interpolation)
        //
        //**********************************************************************
    
        //
        // Decide what to use: sqrt(Y*OH) or sqrt(X*K)
        //
                
        if(  ( (dfX/dfY) / (dfOH/dfK) )    >= 1  )
        {
            // Use analytic equation (OgA)' (derivative)
            //  
            // OgA' = sqrt( T*X*K ) = sqrt(T) * sqrt( X*K )
    
            dfFactor = sqrt( dfX*dfK );            
        }
        else
        {
            // Use analytic equation (OgB)' (derivative)
            //  
            // OgB' = sqrt( T*Y*OH ) = sqrt(T) * sqrt( Y*OH )
    
            dfFactor = sqrt( dfY*dfOH );
        }
    
        //
        // Convert the dfFactor in int but check boundaries before
        //
    
        if( dfFactor < 1 )                        // clamp to 1 at least
            ppdev->globalXferOverheadFactor = 1;
        
        else 
        {
            if( dfFactor > 65535 )
                ppdev->globalXferOverheadFactor = 65535;  // 16bit max
            else
                ppdev->globalXferOverheadFactor = (int)(dfFactor);
        }
    
        goto CalibXfer_Done;
    
        
        //---[ An error has most likely occured ]-------------------------------
        
        CalibXfer_Error:
    
            // We diseable implicitly the overlapping
    
            ppdev->globalXferOverheadFactor = 65535;  // 16bit max
    
    
        //---[ All Done... ]----------------------------------------------------
    
        CalibXfer_Done:
    
            EngFreeMem(SysMemBuffer);
    
            
    } // ...if ( ppdev->pfnScreenToMemBlt != NULL ), profile stuff
                                                            
    //**************************************************************************
    //**************************************************************************
    //*[ Profiling Stuff END ]*****[CALIBEND]***********************************
    //**************************************************************************

}

// ...End of:  vCalibrate_XferSpeeds


