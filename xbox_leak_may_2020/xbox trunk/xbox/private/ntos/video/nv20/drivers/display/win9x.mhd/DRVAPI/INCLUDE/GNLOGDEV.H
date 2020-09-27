/*=============================================================================

    Copyright (C) 1997, NVidia Coporation

    File:       gnlogdev.h

    Purpose:    This file holds structures and equates dealing with the
                pdevice.

=============================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

#define MAX_DISP_CONFIG 80

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;  A DEVICE BITMAP HEADER  ;;;;;;;;;;;;;;;;;;;;;;;;;;;
// This structure must have a DIBENGINEHDR as the first field. After that,
// anything is fine.
typedef struct  _DEVBMP
{
    DIBENGINEHDR    Dibeng;
    USHORT          wLeft;
    USHORT          wTop;
    USHORT          wRight;
    USHORT          wBottom;
}   DEVBMP;

#define MAX_SSBS    3
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;  The PHYSICAL DEVICE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

typedef struct  _GNLOGDEVCX
{
    // This must go first
    DIBENGINEHDR    Dibeng;

    // Tells us if we are initting or not. This is cleared after
    // the second enable call (the one that inits the PPDEVICE.
    ULONG       dwInitting;

    // The GNBOARDCX that is attached to this GNLOGDEV
    ULONG       pGNBoardCX;

    // Which GNLOGDEV instance this is on the board (0, 1, 2, ...)
    ULONG       dwLogicalDeviceInstanceOnThisBoard;

    // How many CRTCs are attached to this logical device
    ULONG       dwNumCRTCsAttachedToThisLogicalDevice;

    // The number of clone devices that this logical device is
    // waiting for. Generally a logical device is waiting for a
    // clone if a clone device got stolen from it at one point
    // in time. As the stolen CRTCCXs get freed, they will be
    // restored when they become available again.
    ULONG       dwNumClonesAwaited;

    // How many CRTCs are on this board
    ULONG       dwTotalCRTCsOnBoard;

    // Do we allow desktops larger than a CRTC will set?
    ULONG       dwVirtualDesktopEnabled;

    // Is this an Nvidia Control Panel SetMode?
    ULONG       lpBoardRegistryData;

    // This variable holds the number of bytes of video memory needed
    // by the primary surface. For safety sake, because we can set
    // low res DX modes, this will never be less than the amount of
    // memory required for a 640x480x8 screen even if the actual
    // display is smaller.
    ULONG       dwMemorySizeForPrimarySurface;

    // OK, this is a bit ugly. We keep the two code segments we are
    // using in the driver in here.
    ULONG       dw16BitCodeSegment;
    ULONG       dw32BitCodeSegment;

    // There can be a number of GNCRTCCXs attached to this GNLOGDEVCX.
    ULONG       pGNCrtcCX[MAX_ATTACHED_CRTCS];

    // How about the hw logdevice data
    ULONG       pHWLogdevCX;

    // Keep a ptr to the GDI info for this PDEVICE.
    ULONG       lpGdiInfo;

    // If the hardware uses a 5-6-5 format for 16bpp, then set this
    // field to FIVE6FIVE. If the hardware uses 1-5-5-5 format, then
    // set this field to 0.
    ULONG       dw16BppFormat;      // FIVE6FIVE or 0 (which means 5-5-5)
    ULONG       dwFontFormat;        // C1_BIT_PACKED or C1_BYTE_PACKED

    // Here is where we crate pseudo bitmap headers for
    // save-screen offscreen bitmaps.
    DEVBMP      SaveScreenBitmapCache[MAX_SSBS];

    // These are for handling a Windows bug having to do with
    // Screen repaints coming back from a DOS box.
    ULONG       dwCheckCursorCount;
    ULONG       dwGraphicsTimeStamp;
    ULONG       lpfnUserRepaint;

    // This flag indiactes that device(s) has been remapped.
    ULONG       dwDeviceHasBeenRemapped;

    //--------------- Begin byte alignment!!! ------------
    UCHAR       bRepaintDisable;
    UCHAR       bRepaintPending;
    UCHAR       bUserRepaintCallCount;
    UCHAR       bRepaintIssued;

    // Scanline and Polyscanline stuff
    UCHAR       bInScanSequence;
    UCHAR       bHollowBrushForScan;
    UCHAR       bPuntingScan;

    UCHAR       bDirectXActive;

    UCHAR       bInterlacedStereo;
    UCHAR       bSetModeBypass;
    UCHAR       bReserved1;
    UCHAR       bReserved2;
    //---------------   End byte alignment!!!  ------------

    // Macrovision Support
    ULONG       dwMacrovisionNavigatorID;
    ULONG       dwCurrentMacrovisionKey;
    ULONG       dwNextMacrovisionKey;
    ULONG       dwMacrovisionMode;

    // This flag indiactes that the hardware should not be touched.
    ULONG       dwHardwareBusy;

    // fInt2FScreenSwitchOut is used to indicate we have received
    // a ScreenSwitchOut but not a screenSwitchIn.
    ULONG       dwInt2FScreenSwitchOut;

    // This is used to store some black-box control panel data
    ULONG       dwCplData;

    // dwCurrentLine is used as the last address for GN_RollerToolEscape
    ULONG       dwCurrentLine;

    // This memory block will be used for the hotkey display switch
    // escape. We save off the state information for the state that
    // the hotkey handler wants us to switch to on the next Enable.
    // We don't know the exact size in here, but there is a sizecheck
    // in the file that references this data structure, so that the
    // build will break if it is not big enough.
    ULONG       sDispConfig[MAX_DISP_CONFIG];
    ULONG       dwDisplayConfigPending;
    ULONG       dwNvDisplayChangePending;
    ULONG       dwBeginDisplayChangeIssued;

    // This will temporarily hold the xres,yres that Windows wants
    // us to change to. In multimon, Windows will truncate our
    // mode heights so that they are a multiple of 8. Hence, we
    // have to play a few games with the mode they ask us to
    // set and the mode that we validate against our table.
    USHORT      wWindowsXRes;
    USHORT      wWindowsYRes;

    DIBHEADER   bmi;
    ULONG       dwPalette[256];
    UCHAR       bPaletteXlatTable[256];
    UCHAR       bPaletteInverseXlatTable[256];
}   GNLOGDEVCX;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

