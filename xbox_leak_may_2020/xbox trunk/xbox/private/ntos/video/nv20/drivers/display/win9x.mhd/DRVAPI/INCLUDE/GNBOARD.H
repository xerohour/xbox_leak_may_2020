/*=============================================================================

    Copyright (C) 1997, NVidia Coporation

    File:       gnboard.h

    Purpose:    This file holds structures and equates dealing with the
                GNBOARDCX.

=============================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// The following data structure holds information about a logical device
// to board context.

typedef struct  _LOGDEVTOBOARD
{
    ULONG   dwBoardDevnodeHandle;
    ULONG   dwGNBoardCXOffset;
}   LOGDEVTOBOARD;

// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// The following data structure holds information about a particular
// offscreen bitmap. There is an array of these structures.

typedef struct  _OFFBMPINFO
{
    ULONG   dwBitmapSel;
    ULONG   dwBitmapOffset;
    ULONG   dwBitmapLength;
    ULONG   dwBitmapVRAMOffset;
}   OFFBMPINFO;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// The following is for the shared memory allocation escapes.
typedef struct  _ID_AND_DATA
{
    ULONG   dwID;
    ULONG   dwData;
}   ID_AND_DATA;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;  The Generic Board Context  ;;;;;;;;;;;;;;;;;;;;;;;;

#define MAX_OFFSCREEN_BITMAPS       0x0300
#define MAX_SHARED_MEM_BLOCKS       0x04

typedef struct  _GNBOARDCX
{
    // Set to 1 when initting. Cleared on first modeset.
    ULONG       dwInitting;

    // How many CRTCs are available on this board?
    ULONG       dwNumCrtcsOnBoard;

    // How many GNLOGDEV structs are attached to this board.
    ULONG       dwLogicalDeviceInstancesOnThisBoard;

    // Here are all the logical devices attached to the board
    ULONG       pGNLogdevCX[MAX_ATTACHED_CRTCS];

    // This is the flat data selector that this board is using.
    ULONG       dwFlatDataSelector;

    // This field is TRUE if DX is active, and FALSE if not.
    ULONG       dwDirectXActive;

    // Keep track of this.
    ULONG       pHWBoardCX;

    // This variable holds the number of bytes of video memory needed
    // to validate mode.
    ULONG       dwMemorySizeForValidateMode;

    // This is the registry path that this board will be using as
    // the local base path -- the place in the registry where values
    // specific to this board are stored.
    ULONG       dwDeviceNodeHandle;
    ULONG       lpLocalDisplayBasePath;
    UCHAR       szLocalDisplayBasePath[128];

    // The starting offset and length in video memory of the space
    // available for offscreen allocations.
    ULONG       dwOffscreenBaseAddress;
    ULONG       dwOffscreenLength;

    // Offscreen Memory And Bitmap Management
    ULONG       dwOffscreenHeap;
    ULONG       dwOffscreenFlags;
    ULONG       dwOffscreenLockCount;

    ULONG       dwOffscreenBitmapsAllocatedSinceLastFlush;
    ULONG       dwOffscreenBitmapsCurrentlyAllocated;
    ULONG       dwBytesOfOffscreenMemoryCurrentlyAllocatedForBitmaps;
    ULONG       pNextOffbmpinfo;
    OFFBMPINFO  OffscreenBitmapCache[MAX_OFFSCREEN_BITMAPS];

    // Used for some crazy display device management stuff.
    ULONG       dwCheckCursorTicksSinceSecondDriverLoaded;
    ULONG       dwOldTwinviewState;
    ULONG       dwOldPrimaryDevData;
    ULONG       dwOldCloneDevData;

    // This array is for shared memory allocated by clients.
    ID_AND_DATA sSharedMemBlocks[MAX_SHARED_MEM_BLOCKS];
}   GNBOARDCX;

// These flags can be used in the dwOffscreenFlags field.
#define OFFS_ALLOCS_BLOCKED         0x0001
#define OFFS_HEAP_UNAVAILABLE       0x0002
#define NO_OFFS_BMPS                0x0003
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

