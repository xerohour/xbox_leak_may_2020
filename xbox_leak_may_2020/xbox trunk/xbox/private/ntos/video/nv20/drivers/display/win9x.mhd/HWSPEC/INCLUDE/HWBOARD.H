/*=======================================================================

    Copyright (C), 1998 Nvidia Coporation

    File:       hwboard.h

    Purpose:    This file holds the HWBOARDCX structure and other
                board related data.

=========================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

// Other NV specific defines
#define NV_DEVICE_VERSION_4     0x0004
#define NV_DEVICE_VERSION_5     0x0005
#define NV_DEVICE_VERSION_10    0x0010
#define NV_DEVICE_VERSION_20    0x0020

// NV Architecture IDs -- NV05 is NV_04_ARCHITECTURE, REVISION 1
#define NV_03_ARCHITECTURE      0x03
#define NV_04_ARCHITECTURE      0x04
#define NV_10_ARCHITECTURE      0x10
#define NV_20_ARCHITECTURE      0x20

// This wasn't defined in the nvidia\sdk header files
#define NV1_MEMORY_SYSTEM       0x3E

// These are for the dwBusType field in the HWBOARDCX
#define NV_BUS_TYPE_PCI         0x01
#define NV_BUS_TYPE_AGP         0x04

// Our push buffer size
#define DMA_PUSH_BUFFER_SIZE    0x400000

// Hey, hope this is correct ...
#define WRITE_COMBINE_PAD_SIZE  0x20

// Normal Heap allocations that are purged on modesets or if we
// need more memory use this ID.
#define GDI2D_HEAP_OWNER        'NV2D'

// Allocations of the primary surface use this ID
#define GDI2D_DESKTOP_OWNER     'NVPS'

// Allocations for the board for things that must never disappear
// use this ID.
#define GDI2D_BOARD_OWNER       'NVBD'

// DDRAW heap ID that are purged on modesets if we need more memory.
#define DX_HEAP_ID      'NVDX'


// The first 100H bytes of each channel is NvControl stuff
typedef volatile struct
{
    V032    object;             // 0000-0003    current object register
    V032    reserved01[0x003];  // 0004-000C
    U016    free;               // 0010-0011    free count
    U016    reserved02[0x001];
    U032    reserved03[0x003];
    struct  tagNvPassword
    {
        V032    token[4];       // 0020-002F    Any write unlocks the chip
    }
    password;
    V032 dwPush;                // 0030-0033    Any write saves channel state
    V032 dwPop;                 // 0034-0037    Any write restores channel state
    V032 SetMethodCtxDma;       // 0038-003B    NV_CONTEXT_DMA_FROM_MEMORY
    V032 SetMethodNotifyCtxDma; // 003C-003F    NV_CONTEXT_DMA_TO_MEMORY
    V032 SetMethodStart;        // 0040-0043    offset within ctxdma in bytes
    V032 SetMethodLength;       // 0044-0047    Context DMA length in bytes
    V032 SetMethodNotify;       // 0048-004B    notification style
    V032 reserved04[0x02d];
}   NvControl;


// This structure is the layout of the GDI display driver's channel.
typedef volatile struct tagGDICHAN
{
    union
    {
        NvControl                   StartOfChannelHdr;
        NvControl                   ContextRopHdr;
    };

    Nv03ContextRop                  ContextRop;
    NvControl                       ContextSurfaces2dHdr;
    Nv04ContextSurfaces2d           ContextSurfaces2d;
    NvControl                       ContextPatternHdr;
    Nv04ImagePattern                ContextPattern;
    NvControl                       ContextClipRectHdr;
    Nv01ImageBlackRectangle         ContextClipRect;
    NvControl                       RectAndTextHdr;
    Nv04GdiRectangleText            RectAndText;
    NvControl                       ImageBlitHdr;
    Nv04ImageBlit                   ImageBlit;
    union
    {
        NvControl                   ImageFromCpuHdr;
        NvControl                   IndexedImageFromCpuHdr;
        NvControl                   StretchedImageFromCpuHdr;
        NvControl                   MemoryToMemoryFormat0Hdr;
        NvControl                   RenderSolidLinHdr;
        NvControl                   VideoSinkHdr;
        NvControl                   VideoFromMemoryHdr;
        NvControl                   VideoColorMapHdr;
        NvControl                   VideoLutCursorDacHdr;
    };
    union
    {
        Nv04ImageFromCpu            ImageFromCpu;
        Nv04IndexedImageFromCpu     IndexedImageFromCpu;
        Nv04StretchedImageFromCpu   StretchedImageFromCpu;
        Nv03MemoryToMemoryFormat    MemoryToMemoryFormat0;
        Nv04RenderSolidLin          RenderSolidLin;
        NvVideoSink                 VideoSink;
        NvVideoFromMemory           VideoFromMemory;
        NvVideoColormap             VideoColorMap;
        Nv04VideoLutCursorDac       VideoLutCursorDac;
    };
    union
    {
        NvControl                   MemoryToMemoryFormat1Hdr;
    };
    union
    {
        Nv03MemoryToMemoryFormat    MemoryToMemoryFormat1;
    };
}   GDICHAN;

typedef GDICHAN *LPGDICHAN;


// These are the objects which we will be using
#define OBJ_DMA_PUSH_BUFFER                     0x00009000
#define OBJ_DMA_PUSH_BUFFER_NOTIFIER            0x00009001
#define OBJ_VRAM_TO_SYSRAM_NOTIFIER0            0x00009002
#define OBJ_VRAM_TO_SYSRAM_NOTIFIER1            0x00009003
#define OBJ_SYSRAM_TO_VIDEO_NOTIFIER            0x00009004
#define OBJ_DMA_TO_SYSRAM                       0x00009006
#define OBJ_DMA_FROM_SYSRAM                     0x00009007
#define OBJ_DMA_FROM_VRAM                       0x00009008
#define OBJ_DMA_IN_VRAM                         0x00009009

#define OBJ_CONTEXT_SURFACES_2D                 0x0000900A
#define OBJ_DMA_FROM_VRAM_TO_SYSRAM_0           0x0000900B
#define OBJ_DMA_FROM_VRAM_TO_SYSRAM_1           0x0000900C
#define OBJ_CONTEXT_CLIP_RECT                   0x0000900D
#define OBJ_CONTEXT_PATTERN                     0x0000900E
#define OBJ_CONTEXT_ROP                         0x0000900F
#define OBJ_IMAGE_BLIT                          0x00009010
#define OBJ_RECT_AND_TEXT                       0x00009011
#define OBJ_IMAGE_FROM_CPU                      0x00009012
#define OBJ_SOLID_LINE                          0x00009013
#define OBJ_INDEXED_IMAGE_FROM_CPU              0x00009014
#define OBJ_STRETCHED_IMAGE_FROM_CPU		0x00009015

#define OBJ_DMA_PUSH_MEMORY                     0x00009999

// Note that there might be multiple VideoLutCursorDac objects
// This ID is for the first one. There is one for each additional
// CRTC on the board and the object IDs are sequential numbers
// starting with this first one.
#define OBJ_VIDEO_LUT_CURSOR_DAC                0x00009020

// Similar to above. Seems like 0x10 CRTCs is enough ...
#define OBJ_VIDEO_LUT_CURSOR_DAC_NOTIFIER       0x00009030

// Similar to above but for the Palette
#define OBJ_DMA_IN_SYSRAM_FOR_PALETTE           0x00009040

// Similar to above but for the Gamma
#define OBJ_DMA_IN_SYSRAM_FOR_GAMMA             0x00009050

#define	INTERLACED_STEREO						0x00000001

typedef struct  _HWBOARDCX
{
    ////////////////////////////////////////////////////////////////
    // The pages which contain the first part of this structure
    // should be marked as CACHEABLE.

    // These "constants" describe the DMA push buffer
    ULONG    pPushBufferStartOffset;                        // 0000-0003
    ULONG    pPushBufferContextStartOffset;                 // 0004-0007
    ULONG    dwPushBufferLength;                            // 0008-000B

    // Use IO to flush processor and chipset cache if true
    ULONG    dwFlushCacheWithIO;                            // 000C-000F

    // These "variables" are used to manage the push buffer
    ULONG    pCurrentPushBufferOffset;                      // 0010-0013
    ULONG    dwBytesFree;                                   // 0014-0017

    // Keep these variables accessible
    ULONG    dwContextSurface2dDstSrcPitch;                 // 0018-001B
    ULONG    dwContextSurface2dSrcOffset;                   // 001C-001F
    ULONG    dwContextSurface2dDstOffset;                   // 0020-0023
    ULONG    dwMinContextSurface2dPitch;                    // 0024-0027
    ULONG    dwMaxContextSurface2dPitch;                    // 0028-002B

    // I need a variable that reminds me to restore the alpha when
    // I have temporarily set a TRANSPARENT hatch pattern in output.
    ULONG    dwGetOffsetAtLastBeginAccess;                  // 002C-002F

    ULONG    dwTemp[4];                                     // 0030-003F

    // The start of this context will be the PUSHER notifier
    NvNotification  sPushBufferNotifier;                    // 0040-004F

    // I need some notifiers for the MemoryToMemoryFormat objects.
    // The standard MemoryToMemoryFormat object has an array of
    // two notifiers. They are the following:
    // MethodNotifer    -- When certain methods of the object are written
    // BufferNotifier   -- When the MemoryToMemory transfer is completed
    NvNotification  sVramToSysramMethodNotifier0;           // 0050-005F
    NvNotification  sVramToSysramBufferNotifier0;           // 0060-006F
    NvNotification  sVramToSysramMethodNotifier1;           // 0070-007F
    NvNotification  sVramToSysramBufferNotifier1;           // 0080-008F

    // I need some notifiers for the VideoFromMemory object.
    // The standard VideoFromMemory object has an array of
    // two notifiers. They are the following:
    // BufferNotifier   -- When the VideoFromMemory transfer is completed
    // Flip0Notifer     -- Indicates page 0 has been flipped to
    // Flip1Notifier    -- Indicates page 1 has been flipped to
    // Buffer 0 Scan    -- Data field of notifier contains current scanline
    //                     being displayed in buffer 0
    // Buffer 1 Scan    -- Data field of notifier contains current scanline
    //                     being displayed in buffer 1
    NvNotification  sSysramToVideoBufferNotifier;           // 0090-009F
    NvNotification  sSysramToVideoFlip0Notifier;            // 00A0-00AF
    NvNotification  sSysramToVideoFlip1Notifier;            // 00B0-00BF
    NvNotification  sSysramToVideoScan0Notifier;            // 00C0-00CF
    NvNotification  sSysramToVideoScan1Notifier;            // 00D0-00DF

    // These are offscreen video memory addresses of where the cursor
    // cache is located for each CRTC.
    ULONG   dwCursorCacheSize;                              // 00E0-00E4
    ULONG   adwCursorCacheVramOffset[MAX_ATTACHED_CRTCS];   // 00E4-???
    ULONG   adwReservedPrimarySurfaceOffsets[MAX_ATTACHED_CRTCS]; // ????

    // A little slack space ...
    ULONG   tmp[35 - 2 * MAX_ATTACHED_CRTCS];               // ????-016F

    /////////////////////////////////////////////////////////////////
    // Set to non-0 if we are initting. Cleared on first modeset
    ULONG   dwInitting;                                     // 0170-0173

    // The specific Nvidia board number from the RM  (1 based.)
    ULONG   dwDeviceNum;                                    // 0174-0177

    // Root handle
    ULONG   dwRootHandle;                                   // 0178-017B

    // The ResMan device handle returned from allocating the device
    ULONG   dwhDevice;                                      // 017C-017F

    // Information about the frame buffer
    ULONG   dwRamFlags;                                     // 0180-0183
    ULONG   dwVideoMemorySize;                              // 0184-0187
    ULONG   pVideoMemory;                                   // 0188-018B
    ULONG   dwVideoMemorySel;                               // 018C-018F

    // Information about the device
    ULONG   pChipRegs;                                      // 0190-0193

    // Deevice version like NV_DEVICE_VERSION_5 (or _4 or _10, etc.)
    ULONG   dwDeviceVersion;                                // 0194-0197

    // PCI or AGP?
    ULONG   dwBusType;                                      // 0198-019B

    // This field tells how many CRTCs are on the board. The next one
    // field has a one bit set for each CRTC that has been allocated.
    ULONG   dwNumCRTCsOnBoard;                              // 019C-019F
    ULONG   dwCRTCsAllocatedFlags;                          // 01A0-01A3

    // This is information about the push buffer and channel
    // Freed by FreeRoot if Allocated via NV_AllocMemory.
    // If allocated with SysAllocPages, we must free it by
    // calling lpfnNvSysFreePages with dwRealPushBufferPageHandle.
    ULONG   dwRealPushBufferPageHandle;                     // 01A4-01A7
    ULONG   pRealPushBuffer;                                // 01A8-01AB

    // Freed by FreeRoot
    ULONG   pDmaChannel;                                    // 01AC-01AF

    // This is the real ptr that was allocated for the HWBOARDCX.
    // It is not what you return to the generic code as the
    // HWBOARDCX ptr though, so we must save this so that we can
    // free it later. NOTE: We return conctext sekector relative
    // ptr version of pAliasedHWBoardCX. This is different
    // linear address space that maps to the same memory as
    // pHWBoardCXReal.                                      
    ULONG   pHWBoardCXReal;                                 // 01B0-01B3

    // We have aliased the push buffer so we can remap some of the pages
    // to point to other things. As a result, we can reference everything
    // off of the context selector that is passed around.
    ULONG   pAliasedHWBoardCX;                              // 01B4-01B7

    // Non-0 when DirectX is active
    ULONG   dwDirectXActive;                                // 01B8-01BB

    // This is where the table of LUT values is stored
    ULONG   dwIndexedLUTOffset;                             // 01BC-01BF

    // This is used by OpenGL to track the mode switches. The variable
    // is incremented by one when a mode switch begins and by another
    // one when the mode switch is finished. An even number indicates the
    // mode is stable. An odd number indicates a mode set is in progress.
    ULONG   dwModeSwitchCount;                              // 01C0-01C3

    // This is the channel number used by the display driver.
    ULONG   dwGdiDriverChannelID;                           // 01C4-01C7

    // I haven't yet decided if I want the current channel ID variable
    // to be in the PDEVICE or in the HARDWARESEGMENT. It depends on
    // which I have a ptr to more often when I need to access the
    // current channel ID variable. If I decide that I want it in the
    // PDEVICE, then this will be it.
    ULONG   dwCurrentChannelID;                             // 01C8-01CB

    // The pCurrentChannelID is a ptr to a dword variable thats holds
    // the channel ID (0-15) of the most recent channel to trigger
    // their DMA pusher. Every channel should check this variable to
    // see if it matches the channel ID of their channel and, if it
    // does not, the channel should sync to the other channels before
    // triggering its own DMA. Each channel should write their channel
    // ID into the dword pointed to by pCurrentChannelID when triggering
    // their DMA. This variable is used by all the different drivers
    // that can have channels to the hardware (e.g. DirectX, OpenGL).
    ULONG   pCurrentChannelID;                              // 01CC-01CF

    // To sync between the different DMA channels, these are needed
    ULONG   dwGdiPushBufferIndex;                           // 01D0-01D3
    ULONG   dwGdiPushBufferMask;                            // 01D4-01D7


    // Used for debugging
    ULONG    dwTempEnd;                                     // 01D8-01DB
    ULONG    dwLastGet;                                     // 01DC-01DF
    ULONG    dwLastWaitPath;                                // 01E0-01E3
    ULONG    dwPushBufferOffsetOnWaitEnter;                 // 01E4-01E7
    ULONG    dwLastBytesFree;                               // 01E8-01EB
    ULONG    dwLastByteRequest;                             // 01EC-01EF

    // This holds the chip revision
    ULONG   dwDeviceRevision;                               // 01F0-01F3

    ULONG   dwDisplayChangeCount;                           // 01F4-01F7

	// Misc stereo related properties
    ULONG   dwStereoConnectionType;                         // 01F8-01FB
    ULONG   dwReserved;										// 01FC-01FF

    // Next we need a cacheable buffer that MemoryToMemoryFormat can use
    // to transfer chunks of VRAM back into. This should be the remainder
    // of this 4K page plus some whole number of additional 4K pages.
#define CACHEABLE_BUFFER_SIZE   (0x8000 - 0x0200)
    UCHAR   bCacheableDmaBuffer[CACHEABLE_BUFFER_SIZE];

    // End of the CACHEABLE area.
    ////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // The page that maps the following should be marked as
    // NON_CACHEABLE and NON-WRITECOMBINE -- it is mapping hardware.
    UCHAR   bHWPageStart[0x0040];
    ULONG   dwDmaPutPtr;
    ULONG   dwDmaGetPtr;
    UCHAR   bHWPageEnd[0x1000 - 0x0048];
    // End NON_CACHEABLE, NON WRITE-COMBINE page
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // The rest of the structure is the Dma Pusher Buffer. It can be
    // marked NON-CACHEABLE, WRITE-COMBINE but may also run as CACHED
    // memory. We cannot actually put
    // the entire DMA PUSH BUFFER in here as an array if it is larger
    // than 64K because in 16bit land, structures are limited to 64K.
    // But this file will be converted to an assembly include file,
    // and as long as we have a label for the start of the buffer,
    // the assembly code will be able to access the whole push buffer
    // correctly.
    //
    // To force a flush of the CPU's internal Write-Combine (WC) buffer,
    // a write is required to a region of WC memory which is not in the
    // same 32byte window as the last write to a WC memory area. We want
    // the first dword of the first page to provide this mechanism.
    //
    // One more thing. Make the push buffer 0x20 bytes less than a
    // multiple of 4K. Why? Because then the buffer will end just on
    // a page boundary and there won't be any wasted bytes on the end
    // of a 4K page (the OS does page allocation).

    // The start of this context will be the PUSHER notifier
    UCHAR   bScrapSpace[WRITE_COMBINE_PAD_SIZE];

    // I'd like to do what the next line does, but this is 16bit land
    // so I'll just use the line after to provide a label.
//  UCHAR   bDmaPushBuffer[DMA_PUSH_BUFFER_SIZE];
    UCHAR   bDmaPushBuffer;
    ///////////////////////////////////////////////////////////////////

}   HWBOARDCX;


