/*
 * nvEscDef.h
 *
 * Provides definitions for nVidia ExtEscape calls into display driver.
 *
 * Copyright (c) 1998, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

#ifndef NV_ESC_DEF_INCLUDED
#define NV_ESC_DEF_INCLUDED


////////////////////////////////////////////////////////////////////////
// nVidia private escapes

#ifdef UNIX
#define NV_ESC_RM_BASE                  0x0000
#else
#define NV_ESC_RM_BASE                  0x7000
#endif

#define NV_ESC_RM_OPEN                  (NV_ESC_RM_BASE + 0x20)
#define NV_ESC_RM_CLOSE                 (NV_ESC_RM_BASE + 0x21)
#define NV_ESC_RM_ALLOC_ROOT            (NV_ESC_RM_BASE + 0x22)
#define NV_ESC_RM_ALLOC_DEVICE          (NV_ESC_RM_BASE + 0x23)
#define NV_ESC_RM_ALLOC_CONTEXT_DMA     (NV_ESC_RM_BASE + 0x24)
#define NV_ESC_RM_ALLOC_CHANNEL_PIO     (NV_ESC_RM_BASE + 0x25)
#define NV_ESC_RM_ALLOC_CHANNEL_DMA     (NV_ESC_RM_BASE + 0x26)
#define NV_ESC_RM_ALLOC_MEMORY          (NV_ESC_RM_BASE + 0x27)
#define NV_ESC_RM_ALLOC_OBJECT          (NV_ESC_RM_BASE + 0x28)
#define NV_ESC_RM_FREE                  (NV_ESC_RM_BASE + 0x29)
#define NV_ESC_RM_DMA_PUSH_INFO         (NV_ESC_RM_BASE + 0x2A)
#define NV_ESC_RM_ALLOC                 (NV_ESC_RM_BASE + 0x2B)
#define NV_ESC_RM_CONFIG_VERSION        (NV_ESC_RM_BASE + 0x31)
#define NV_ESC_RM_CONFIG_GET            (NV_ESC_RM_BASE + 0x32)
#define NV_ESC_RM_CONFIG_SET            (NV_ESC_RM_BASE + 0x33)
#define NV_ESC_RM_CONFIG_UPDATE         (NV_ESC_RM_BASE + 0x34)
#define NV_ESC_RM_ARCH_HEAP             (NV_ESC_RM_BASE + 0x35)
#define NV_ESC_RM_DEBUG_CONTROL         (NV_ESC_RM_BASE + 0x36)
#define NV_ESC_RM_CONFIG_GET_EX         (NV_ESC_RM_BASE + 0x37)
#define NV_ESC_RM_CONFIG_SET_EX         (NV_ESC_RM_BASE + 0x38)
#define NV_ESC_RM_I2C_ACCESS            (NV_ESC_RM_BASE + 0x39)
#define NV_ESC_RM_POWER_MANAGEMENT      (NV_ESC_RM_BASE + 0x3A)
#define NV_ESC_RM_INTERRUPT             (NV_ESC_RM_BASE + 0x3B)
#define NV_ESC_RM_OS_CONFIG_GET         (NV_ESC_RM_BASE + 0x3C)
#define NV_ESC_RM_OS_CONFIG_SET         (NV_ESC_RM_BASE + 0x3D)
#define NV_ESC_RM_OS_CONFIG_GET_EX      (NV_ESC_RM_BASE + 0x3E)
#define NV_ESC_RM_OS_CONFIG_SET_EX      (NV_ESC_RM_BASE + 0x3F)
#define NV_ESC_RM_NVWATCH               (NV_ESC_RM_BASE + 0x40)

#ifdef UNIX
#define NV_ESC_RM_ALLOC_EVENT           (NV_ESC_RM_BASE + 0x44)
#define NV_ESC_RM_AGP_INIT              (NV_ESC_RM_BASE + 0x45)
#define NV_ESC_RM_AGP_TEARDOWN          (NV_ESC_RM_BASE + 0x46)
#define NV_ESC_RM_IO_FLUSH              (NV_ESC_RM_BASE + 0x47)
#endif

////////////////////////////////////////////////////////////////////////
// Nvidia OpenGL client information.

//
// NOTE: WNDOBJ_SETUP and ESC_NV_OPENGL_CREATE_DRAWABLE perform
// the same function except that the I/O manager in NT responds
// to WNDOBJ_SETUP by calling our DrvClipChanged during the actual
// escape call.  This doesn't happen with ESC_NV_OPENGL_CREATE_DRAWABLE
// so the shared clip list area isn't initialized properly.
//
#define WNDOBJ_SETUP                            4354

#define ESC_NV_OPENGL_DMA_PUSH_GO               0x7001

#define ESC_NV_OPENGL_ESCAPE                    0x7101 // nVidia OpenGL escape
//#define ESC_NV_OPENGL_CREATE_DRAWABLE           0x0001 // create drawable client information
#define ESC_NV_OPENGL_DESTROY_DRAWABLE          0x0002 // destroy drawable client information
#define ESC_NV_OPENGL_CREATE_CONTEXT            0x0003 // create context client information
#define ESC_NV_OPENGL_DESTROY_CONTEXT           0x0004 // destroy context client information
#define ESC_NV_OPENGL_DMA_PUSH_GO_CMD           0x0005 // NV3 DMA push go command
#define ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY     0x0006 // memory to screen copy
#define ESC_NV_OPENGL_SCREEN_TO_SCREEN_SRCCOPY  0x0007 // screen to screen copy
#define ESC_NV_OPENGL_SUPPORT_ENABLED           0x0008 // OpenGL ICD enabled or NOT!
#define ESC_NV_OPENGL_ALLOC_SHARED_MEMORY       0x0009 // create shared memory area
#define ESC_NV_OPENGL_FREE_SHARED_MEMORY        0x000A // free shared memory area
#define ESC_NV_OPENGL_GET_DISPLAY_PITCH         0x000B // return pitch of display
#define ESC_NV4_OPENGL_SWAP_BUFFERS             0x000C // NV4 swap buffers...
#define ESC_NV4_OPENGL_FLUSH                    0x000D // NV4 flush...
#define ESC_NV_OPENGL_PURGE_DEVICE_BITMAP       0x000E // purge GDI device bitmaps...
#define ESC_NV_OPENGL_FLUSH                     0x000F // NV10 and up style flush...
#define ESC_NV_OPENGL_SWAP_BUFFERS              0x0010 // NV10 and up style swap buffers...
#define ESC_NV_OPENGL_CPUBLIT                   0x0011 // Generic CPU blit. Currently used for buffer region Ext.
#define ESC_NV_OPENGL_INIT_STATE                0x0012 // Initialize HW state from the display driver side
#define ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES    0x0021 // alloc single back/depth in GDI display driver
#define ESC_NV_OPENGL_FREE_UNIFIED_SURFACES     0x0022 // free single back/depth in GDI display driver
#define ESC_NV_OPENGL_GET_SURFACE_INFO          0x0023 // get offset for single back
#define ESC_NV_OPENGL_DID_STATUS_CHANGE         0x0029 // clip list status call
#define ESC_NV_OPENGL_GET_CLIP_LIST_COUNT       0x002A // return count of rectangles in clip list
#define ESC_NV_OPENGL_GET_CLIP_LIST             0x002B // return list of rectangles for window clip
#define ESC_NV_OPENGL_SUPPORT                   0x002C // return TRUE if OpenGL supported...
#define ESC_NV_OPENGL_REGISTER_RM_CLIENT        0x002D // register RM client with display driver
#define ESC_NV_OPENGL_UNREGISTER_RM_CLIENT      0x002E // unregister RM client with display driver
#define ESC_NV_OPENGL_SET_DAC_BASE              0x002F // set the display base offset
#define ESC_NV_OPENGL_SYNC_CHANNEL              0x0030 // wait for FIFO and graphics engine to be !busy
#define ESC_NV_OPENGL_SET_LAYER_PALETTE         0x0031 // Overlay specific: set palette entries in the DD
#define ESC_NV_OPENGL_GET_LAYER_PALETTE         0x0032 // Overlay specific: get palette entries from the DD
#define ESC_NV_OPENGL_OVERLAY_MERGEBLIT         0x0033 // Overlay specific: do the merge blit fm+fo->scratch->primary
#define ESC_NV_OPENGL_OVERLAY_ALLOWED           0x0034 // W2K: DrvDescribePixelformat needs to know is overlay is possible
#define ESC_NV_OPENGL_STEREO_ALLOWED            0x0035 // W2K: DrvDescribePixelformat needs to know is stereo is possible
#define ESC_NV_OPENGL_SHARED_LIBRARY_SIZES      0x0036 // check __GLNVstate and __GLdrawablePrivate sizes

#undef ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY // uses a separate escape...
#define ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY     0x7102


#define ESC_NV_QUERYSET_REGISTRY_KEY              0x7103
#define NV_QUERY_REGISTRY_KEY                     0x0001
#define NV_SET_REGISTRY_KEY                       0x0002
#define NV_QUERY_REGISTRY_BINARY_KEY              0x0004


#define ESC_NV_MAP_USER					0x9000
#define ESC_NV_UNMAP_USER				0x9001

#ifndef NV_SIGNATURE
// This value also is defined in driver.h of display driver
#define NV_SIGNATURE 0x11223344
#endif

#define ESC_NV_QUERY_PCI_SLOT                   0x7105
#define ESC_NV_QUERY_HEAD_REFRESH_RATE          0x7106

#ifndef NV_ESC_PRIMARY_INFO
#define NV_ESC_PRIMARY_INFO 0x7104
typedef struct _NV_PRIMARY_INFO
{
    ULONG  ulNVSignature;   // IN
    ULONG  ulNumDevices;    // IN
    ULONG  ulHeadNum;       // IN
    ULONG  ulReturnCode;    // IN
    ULONG  ulWidth;         // OUT from display driver
    ULONG  ulHeight;        // OUT from display driver
    ULONG  ulDepth;         // OUT from display driver
    ULONG  ulPitch;         // OUT from display driver
    ULONG  ulPrimaryOffset; // OUT from display driver
    ULONG  ulNumDACs;       // OUT from display driver (NT only)
    ULONG  ulNumActiveDACs; // OUT from display driver
    ULONG  ulActiveDACs;    // OUT from display driver (NT only)
    ULONG *ulPanningPtr;    // OUT from display driver (9x only)
    ULONG *ulFullscreenPtr; // OUT from display driver (9x only)
} NV_PRIMARY_INFO;
#endif

#ifdef _WIN32 // rest of this file only needed for win32 interfaces

typedef struct _NV_OPENGL_COMMAND
{
    ULONG       NVSignature;
    ULONG       command;             // Handle to resource manager client used in NvFree
    ULONG       numDevices;          // Number of devices in the system
    HWND        hWnd;                // Window handle
    HDC         hDC;                 // GDI device context handle
    ULONG       hClient;             // Handle to resource manager client
    ULONG       processHandle;       // Process handle from client associated with globalData
    PVOID       globalData;          // Pointer to global shared data for destroy
    PVOID       other;               // Pointer to whatever
} NV_OPENGL_COMMAND;

typedef struct _NV_OPENGL_CLIP_LIST_DATA
{
    int  clipChangedCount;
    int  rgnDataSize;
    void *rgnData;
    void *rect;
} NV_OPENGL_CLIP_LIST_DATA;

typedef struct _NV_OPENGL_GLOBAL_DATA
{
    ULONG oglMutex;
    ULONG oglModeSwitch;
    ULONG oglPFifoAddress;
    ULONG oglSystemMutex;
} NV_OPENGL_GLOBAL_DATA;

typedef struct _NV_WNDOBJ_SETUP
{
    ULONG       NVSignature;
    HWND        hWnd;       /* Handle to window */
    HDC         hDC;        /* handle to GDI device context */
    ULONG       numDevices; /* Number of devices in the system */
    int         overlayMember; // TRUE if part of an overlay
} NV_WNDOBJ_SETUP;

#define NV_CLIP_MUTEX_WAIT_IN_SECONDS       10 // 10 milliseconds
#define NV_WINDOW_CLIP_FULLYVISIBLE         0x0001
#define NV_WINDOW_CLIP_OCCLUDED             0x0002
#define NV_WINDOW_CLIP_SINGLERECTANGLE      0x0004
#define NV_WINDOW_CLIP_COMPLEX              0x0008
#define NV_WINDOW_CLIP_FULLSCREEN           0x0010
#define MAX_OGL_CLIP_RECTS                  250
#define MAX_OGL_CLIENTS                     512

#define NV_CLIP_LIST_FLUSH                  0x0001
#define NV_CLIP_LIST_SWAP                   0x0002
#define NV_CLIP_LIST_DIRTY                  0x0004

typedef struct _NV_OPENGL_DRAWABLE_INFO
{
    ULONG       oglMutexPtr;            // Handle to resource manager client used in NvFree
    PVOID       oglGlobalPagePtr;       // Pointer to shared memory
    ULONG       mutexLock;              // Lock on this data structure
    PVOID       pClientInfoMdl;         // MDL for lock down pages
    PVOID       userVirtualAddress;     // user virtual address for clientInfo
    ULONG       hClient;                // handle to resource manager
    HANDLE      hWnd;                   // Handle to window tracked in DrvClipChanged
    HANDLE      hDC;                    // Handle to GDI device context
    LONG        iPixelFormat;           // pixel format associated with this client
    RECT        rect;                   // rectangle of window on screen
    ULONG       clipChangedCount;       // Number of times clip has changed since client registered
    ULONG       clipFlags;              // Clip status for last clip
    ULONG       numClipRects;           // Number of clip rectangles
    RECTL       windowRect[MAX_OGL_CLIP_RECTS];
                                        // Clip region for client window
    PVOID       origClientDrawableInfoPtr; 
                                        // Original Ptr remembered to free memory

    RECTL       *cachedClipRectList;    // Cache the clip list so that we do not have to merge it
    ULONG       cachedNumClipRects;     // Number of rectangles.
    ULONG       cachedClipListSize;     // Currently allocated size of the clip list.
	ULONG       cachedClipListFlag;     // Flag to determine whether to recompute clip list.
                                        // Values: NV_CLIP_LIST_FLUSH, NV_CLIP_LIST_SWAP, NV_CLIP_LIST_DIRTY
    RECTL       *cachedExclusiveRectList;    // Cache the clip list so that we do not have to merge it
    ULONG       cachedNumExclusiveRects;     // Number of rectangles.
    RECTL       *copyWindowRect;           // Actual clip rectangle pointer used everywhere in NV4+ driver
    ULONG       sizeCopyWindowRectList;    // Size of clip rectangle pointer above.

    RECTL       *translateWindowRectList; // cache for translated clip list
    ULONG       translateNumWindowRects;  // cache count for translate clip list

    ULONG       cxScreen, cyScreen;     // width and height of display
    ULONG       cxcyChanged;

    //
    // The following flag is used when UBB is ON, window flipping is OFF and there are no OGL windows
    // occluding the window this drawable structure references.  This is an optimization for window
    // clipping created for 3D Studio Max.  When 3D Studio Max is in No redraw on expose mode, an expose
    // event will not cause a re-render of the scene.  Compaq workstation did not like this and complained
    // that the image is wrong in comparison to other CAD graphics products.  Since the 3D pixels are 
    // clipped to a GDI window that is occluding our OGL window, it is possible to render those pixels
    // with UBB on without causing scribble.  But for this to work, the occluding window must NOT be a
    // OGL window and window flipping must be off.
    //
    // This flag is computed as part of the DrvClipChanged function.
    //
    ULONG       ubbWindowClipFlag;      // TRUE means use window bounds for clip; otherwise use copyWindowRect list
    ULONG       refCount;               // mainplane+overlay track the same window!, so we need a refCount, to securely destroy the drawable
    // **attention**: - currently sizeof(NV_OPENGL_DRAWABLE_INFO)> 1 page
    //                - only the first page(4096) of NV_OPENGL_DRAWABLE_INFO is visible in the ICD!
    //                - do not place icd-visible members at the end! [and vice versa]
    int         overlayMember;
    } NV_OPENGL_DRAWABLE_INFO;

typedef struct _NV_OPENGL_CONTEXT_INFO
    {
    PVOID       oglGlobalPagePtr;       // Pointer to shared memory
    PVOID       clientPFifoAddress;     // Address of FIFO in client address space
    HWND        hWnd;                   // Window/drawable associated with context
    HDC         hDC;                    // GDI device context
    ULONG       hClient;                // Handle to resource manager client
    PVOID       pClientInfoMdl;         // MDL for lock down pages
    PVOID       userVirtualAddress;     // user virtual address for clientInfo
    PVOID       origClientContextInfoPtr; 

                                        // Original Ptr remembered to free memory
    } NV_OPENGL_CONTEXT_INFO;

// WARNING! This structure is shared with NT display driver in driver.h
#define OGL_ERROR_NO_ERROR             0x0000
#define OGL_ERROR_MODE_SWITCH          0x0001
#define OGL_ERROR_CLIENT_NOT_FOUND     0x0002
#define OGL_ERROR_CLIP_MUTEX_TIMEOUT   0x0003
#define OGL_ERROR_CLIP_LIST_BAD        0x0004
#define OGL_ERROR_LOAD_BUFFER_INFO     0x0005
#define OGL_ERROR_SIMPLE_TO_COMPLEX    0x0006
#define OGL_ERROR_COMPLEX_TO_SIMPLE    0x0007
#define OGL_ERROR_REALLOC_FAILURE      0x0008
#define OGL_ERROR_NO_SWAP              0x0009
#define OGL_ERROR_WINDOW_CHANGED_COUNT 0x000A
#define OGL_ERROR_SURFACE_INVALID      0x000B

typedef struct __GLNVClientSwapHintInfoRec {
    ULONG swapHintRectCount;
    // Swap Hint Rectangle List
    LPRECT swapHintRectList;
} __GLNVClientSwapHintInfo;

typedef struct __GLNVWindowInfoRec {
    // window info
    ULONG left;
    ULONG top;
    ULONG offsetX;
    ULONG offsetY;
    int   winLeft; // window left relative to desktop
    int   winTop;  // window top relative to desktop
    int   devLeft; // device left relative to desktop
    int   devTop;  // device top relative to desktop
    int   clipX;   // drawable clip left value
    int   clipY;   // drawable clip top value
    int   clipW;   // drawable clip width
    int   clipH;   // drawable clip height
} __GLNVWindowInfo;

typedef struct __GLNV4ScissorClipHWInfoRec {
    // scissor clip info
    ULONG set3DSurfaceClipControlWord;
    ULONG size3DSurfacesClip;
} __GLNV4ScissorClipHWInfo;

typedef struct __GLNV4BlitHWInfoRec {
    // BLIT info
    ULONG bltControlWord;
    ULONG bltSizeSet;
} __GLNV4BlitHWInfo;

typedef struct __GLNV4PushBufferHWInfoRec {

    // push buffer info
    ULONG *pClipStart;  // start of clip NOOPs
    ULONG *pBufCur;
    ULONG  dwCmdBufFreeCount;
    PVOID  dmaChannel;
    ULONG *pushBufferBase;
    ULONG *pushBufferEnd;
    ULONG  maskOtherChannels;
    ULONG  ourChannelId;
    ULONG  dwCachedGet;
    ULONG  dwCachedPut;
    ULONG *lastJump;
} __GLNV4PushBufferHWInfo;

typedef struct __GLNV4ClipBufferHWInfoRec {

    // clip buffer info
    void  *tagTable;
    ULONG  tagEnd;
    ULONG *pClipBufBase;
} __GLNV4ClipBufferHWInfo;


typedef struct __GLNV4swapInfoRec {
    ULONG hClient;
    HWND  hWnd;
    HDC   hDC;

    // window info
    __GLNVWindowInfo winInfo;

    // BLIT info
    __GLNV4BlitHWInfo blitInfo;

    // push buffer info
    __GLNV4PushBufferHWInfo pushBufferInfo;

    // clip buffer info
    __GLNV4ClipBufferHWInfo clipBufferInfo;

    // clip and mode switch info
    ULONG localModeSwitchCount;

    __GLNVClientSwapHintInfo clientSwapInfo;

    // swap interval
    ULONG swapInterval;
    ULONG swapCounter;

    // Window Changed Count 
    ULONG windowChangedCount;

    // return status
    ULONG retStatus;
} __GLNV4swapInfo;

#define NV_BLIT_MEM_TO_SCR 1
#define NV_BLIT_SCR_TO_MEM 2
typedef struct {
    HWND        hWnd;
    HDC         hDC;
    ULONG       blitType;
    ULONG       useClipList;
    PVOID       pSrcBase; 
    ULONG       dwSrcByteWidth; 
    ULONG       dwSrcElementSize;
    ULONG       dstOffset; // offset in frame buffer
    PVOID       pDstBase; 
    ULONG       dwDstByteWidth; 
    ULONG       dwDstElementSize;
    ULONG       dwSrcLeft; 
    ULONG       dwSrcTop;
    ULONG       dwDstLeft; 
    ULONG       dwDstTop;
    ULONG       dwWidth; 
    ULONG       dwHeight;
    ULONG       retStatus;
} __GLNvCpuBltData;

typedef struct {
    ULONG       type;
    PCHAR       keyName;
    PVOID       keyVal;
    ULONG       keyValSize;
    ULONG       keyValMaxSize;
    ULONG       retStatus;
} *__PNVRegKeyInfo, __NVRegKeyInfo;


// Desktop Manager private escapes
#define ESC_NV_DESKMGR_ZOOMBLIT         0x7107

#define NVDM_ESC_BLIT_VIDMEM            0x00000000
#define NVDM_ESC_BLIT_SYSMEM            0x00000001
#define NVDM_ESC_BLIT_SMOOTH            0x00000002

typedef struct {
    ULONG flags;
    int scale;
    union {
        RECTL srcRect;
        struct {
            ULONG format;
            int stride;
            PVOID bits;
        } srcMem;
    };
    RECTL dstRect;
    int clips;
    RECTL clipRects[1];
} __NVDMEscapeParams;

#endif // _WIN32

#endif // NV_ESC_DEF_INCLUDED

