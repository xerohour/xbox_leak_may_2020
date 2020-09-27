/*=============================================================================

    Copyright (C) 1997, NVidia Coporation

    File:       gncrtc.h

    Purpose:    This file holds structures and equates dealing with the
                GNCRTCCX.

=============================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

#define MAX_DISPDATA_SIZE                   96
#define MAX_EDID_BUFFER_SIZE                512
#define MAX_MODEINFO_SIZE                   48
#define MAX_MODEOUT_SIZE                    40
#define MIN_TICKS_BEFORE_REREADING_EDID     40

///////////////////////////////////////////////////////////////////////
#define XOR_DATA_SIZE   ((CURSOR_WIDTH * CURSOR_HEIGHT) * 4)

typedef struct  _CURSOREXTRA
{
    ULONG       dwCursorType;       // This is for H/W specific code
    ULONG       dwCursorHash;       // This is for generic code
}   CURSOREXTRA;

typedef struct  _CURSORDESCEX
{
    CURSOREXTRA ExtraData;
    USHORT      wHotX;
    USHORT      wHotY;
    USHORT      wWidth;
    USHORT      wHeight;
    USHORT      wWidthBytes;
    UCHAR       bPlanes;
    UCHAR       bBitsPerPixel;
    UCHAR       bCursorAndMask[AND_MASK_SIZE];
    UCHAR       bCursorXorData[XOR_DATA_SIZE];
}   CURSORDESCEX;

//  This structure is immediately followed by some number of
//  CURSORDESC structures which hold all the cached cursor desciptions.
typedef struct  _CURSORCACHE
{
    ULONG       dwCursorFlags;
    USHORT      wCursorX;
    USHORT      wCursorY;
    ULONG       dwNextCacheSlot;
    ULONG       dwSelectedCacheSlot;
}   CURSORCACHE;

// These flags are for the dwCursorFlags field in the CURSORCACHE structure
// The interrupt handler which changes cursor settings will only look at
// the low 3 bits if the CURSOR_IS_NULL bit is not set.

#define CURSOR_HAS_TRAILS       0x0001
#define CURSOR_MOVE             0x0002
#define CURSOR_SELECT           0x0004
#define CURSOR_IS_NULL          0x0008
#define CURSOR_SEMAPHORE        0x0010
#define CURSOR_IS_SOFTWARE      0x0020
#define CURSOR_SEMAPHORE_BIT    4

// Bits 8 through 31 are the 'SETTING' bits. They indicate that SetCursor
// was called with a cursor pattern, but the pattern has not yet been
// supplied to the hardware. Bit n set means that the cursor corresponding
// to Cache slot n-8 needs to be placed in the hardware's cache. This is
// done to allow for lazy setting of cursor shapes. The CURSOR_SET bit
// is a global SETTING bit to indicate at least one of bits 8 - 31 is set.
#define NUM_CURSOR_FLAGS        8
#define MAX_CACHEABLE_CURSORS   (32 - NUM_CURSOR_FLAGS)

#if (MAX_CACHEABLE_CURSORS > (32 - NUM_CURSOR_FLAGS))
    error
#endif
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;; The Generic CRTC Instance Specific portion of the DEVICE ;;;;;;;;
typedef struct  _GNCRTCCX
{
    // 1 means we are initting. Cleared to 0 on first mode set.
    ULONG           dwInitting;

    // This GNCRTCCX is tied to the following board.
    ULONG           pGNBoardCX;

    // The HW specific crtc context data ptr
    ULONG           pHWCrtcCX;

    // For VideoMirror modes, the Video module issues an Escape to
    // "LOCK" the hw crtccx. What this means is that the GNCRTCCX
    // no longer controls the HW CRTC. To implement this, we NULL
    // out the pHWCrtcCX ptr above. In order that we can still
    // make some hwcrtc calls (just to get state information and
    // maybe do macrovision), we keep a copy of what the HWCRTCCX
    // was in the field below.
    ULONG           pHWCrtcCXWhenLocked;

    // This gives the windows logical device number considering this
    /// board as 0 based. So in normal and clone mode, this will be 0.
    // In multi-mon mode, this number is 0 or 1 (for a 2 head board.)
    ULONG           dwLogicalDeviceInstance;

    // This is a number from 0 to the number of physical devices - 1
    // as an ordinal. It tells which physical device number this is
    // that is attached to the logical device in the prior variable.
    // So for normal and multi-mon mode this will be a 0. For clone
    // mode, you can have several physical devices attached to a
    // given logical device. This would tell which one it is.
    ULONG           dwPhysicalDeviceInstance;

    // This is true if this is the primary CRTC.
    ULONG           dwBootCrtc;

    // This is the registry path that this CRTC will be using as
    // the local monitor base path -- the place in the registry
    // where values specific to the monitor are stored. It may not,
    // however, always be valid since the user can plug new monitors
    // in on the fly. Basically it tells the last .inf that was
    // installed, or the EDID monitor that was seen at boot.
    ULONG           lpLocalMonitorBasePath;
    UCHAR           szLocalMonitorBasePath[128];

    // There is some data in the DISPDATA structure that is specific
    // to each CRTC.
    ULONG           lpBoardRegistryData;
    ULONG           lpNvmodeDispData;
    ULONG           sNvmodeDispData[(MAX_DISPDATA_SIZE + 3) / 4];
    ULONG           lpCrtcModeOut;
    ULONG           sCrtcModeOut[(MAX_MODEOUT_SIZE + 3) / 4];

    // Save the Edid buffer so we don't have to reread all the time.
    ULONG           lpEdidBuffer;
    UCHAR           sEdidBuffer[MAX_EDID_BUFFER_SIZE];
    ULONG           dwEdidSize;

    // Don't want to read the EDID too often because if a monitor
    // doesn't have an EDID, the timeouts take a long time.
    ULONG           dwTicksSinceLastEdidRead;

    // Tells how many bits per color gun the hardware supports for this CRTC
    ULONG           dwBitsPerColorChannel;

    // Tells how many cached cursor slots the hardware has allowed.
    ULONG           dwMaxCacheableCursors;

    // When changing the display device, we ask Windows to recompute
    // the .inf status by reenumerating the attached display devices.
    // Unfortunately, this is asyncronous, and so the modeset that
    // occurs when changing display devices will not give a valid
    // .inf status. This flag tells us to ignore the status
    ULONG           dwDisplayDeviceChange;

    // Our cursor cache for this CRTC
    ULONG           pCursorCache;

    // A flag to indicate whether pan and scan is enabled and also
    // the coordinates of the monitor viewport on the desktop.
    // Enough additional stuff to fully specify the mode.
    ULONG           dwPanAndScanEnabled;
    MYRECTL         rcPhysicalViewport;
    ULONG           dwBytesPerPixelInCurrentMode;
    ULONG           dwBytesPerLineInCurrentMode;
    UCHAR           sCrtcModeInfo[MAX_MODEINFO_SIZE];
    ULONG           lpCrtcModeInfo;

    // The LOGDEV needs one copy of these for each CRTC
    ULONG           dwPaletteTable[256];
    ULONG           dwGammaTable[256];
}   GNCRTCCX;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

