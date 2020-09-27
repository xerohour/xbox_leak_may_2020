/*=======================================================================

    Copyright (C), 1998 Nvidia Coporation

    File:       hwcrtc.h

    Purpose:    This file holds the HWCRTCCX structure and other
                CRTC related data.

=========================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

// This structure holds dynamic data that is unique for each
// given instance of a CRTC on a given board.
typedef struct  _HWCRTCCX
{
    // Set to non-0 if we are initting. Cleared on first modeset
    ULONG   dwInitting;

    // The HWBOARDCX is needed sometimes so store it here.
    ULONG   pHWBoardCX;

    // This is the HWLOGDEVCX that this HWCRTCCX is a child of
    ULONG   pHWLogdevCX;

    // 0 based number telling us which CRTC we are using with
    // respect to the board device.
    ULONG   dwPhysicalCRTCInstance;

    // This is the offset in vram where the cursor patterns are stored
    // Also the cursor alpha to be applied to color cursors. Also we
    // keep around how many cursors we may have to cache.
    ULONG   dwMaxCachedCursors;
    ULONG   dwCursorCacheVramOffset;
    ULONG   dwCursorAlpha;

    // The TV ID and Addr are sometimes needed so store them both here.
    ULONG   dwTVOutDeviceID;
    ULONG   dwTVOutDeviceAddr;

    // This holds the current CRTC pitch.
    ULONG   dwPanPitch;

    // Each one is set to TRUE if the object is created.
    UCHAR   bNotifierCreated;
    UCHAR   bPaletteContextDmaCreated;
    UCHAR   bGammaContextDmaCreated;
    UCHAR   bVidLutCursorDacObjectCreated;

    NvNotification  sVideoLutCursorDacNotifier0;
    NvNotification  sVideoLutCursorDacNotifier1;
    NvNotification  sVideoLutCursorDacNotifier2;
    NvNotification  sVideoLutCursorDacNotifier3;
    NvNotification  sVideoLutCursorDacNotifier4;
    NvNotification  sVideoLutCursorDacNotifier5;
    NvNotification  sVideoLutCursorDacNotifier6;
    NvNotification  sVideoLutCursorDacNotifier7;
    NvNotification  sVideoLutCursorDacNotifier8;

    // The palette and gamma table are placed here for updating and
    // then a VideoFromMemory transfer is initiated.
    ULONG   dwPaletteTable[256];
    ULONG   dwGammaTable[256];
}   HWCRTCCX;


