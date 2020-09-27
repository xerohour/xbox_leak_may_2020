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

////////////////////////////////////////////////////////////////////////
// Bit to distinguish between 32 and 64 bit process
// (Make them the same for now until implementation is complete).

#define NV_ESC_64_BASE                  0x0000
#define NV_ESC_32_BASE                  0x0000

#if defined(_WIN64)
#define NV_ESC_BASE                     NV_ESC_64_BASE
#else
#define NV_ESC_BASE                     NV_ESC_32_BASE
#endif

////////////////////////////////////////////////////////////////////////
// Escapes destined for the Resource Manager

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
#define NV_ESC_RM_ALLOC_EVENT           (NV_ESC_RM_BASE + 0x44)

#ifdef UNIX
#define NV_ESC_RM_AGP_INIT              (NV_ESC_RM_BASE + 0x45)
#define NV_ESC_RM_AGP_TEARDOWN          (NV_ESC_RM_BASE + 0x46)
#define NV_ESC_RM_IO_FLUSH              (NV_ESC_RM_BASE + 0x47)
#endif
#define NV_ESC_RM_SWAP_EXT              (NV_ESC_RM_BASE + 0x49)

////////////////////////////////////////////////////////////////////////
// Nvidia OpenGL client information.

#define WNDOBJ_SETUP                            4354

#define ESC_NV_OPENGL_DMA_PUSH_GO               0x7001

#define ESC_NV_OPENGL_ESCAPE                      0x7101 // nVidia OpenGL escape
#define ESC_NV_OPENGL_DESTROY_DRAWABLE            0x0002 // destroy drawable client information
#define ESC_NV_OPENGL_START_STATE_CLIENT_TRACKING 0x0003  // start tracking of state->hClient
#define ESC_NV_OPENGL_STOP_STATE_CLIENT_TRACKING  0x0004 // stop tracking of state->hClient
#define ESC_NV_OPENGL_DMA_PUSH_GO_CMD             0x0005 // NV3 DMA push go command
#define ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY       0x0006 // memory to screen copy
#define ESC_NV_OPENGL_SCREEN_TO_SCREEN_SRCCOPY    0x0007 // screen to screen copy
#define ESC_NV_OPENGL_SUPPORT_ENABLED             0x0008 // OpenGL ICD enabled or NOT!
#define ESC_NV_OPENGL_ALLOC_SHARED_MEMORY         0x0009 // create shared memory area
#define ESC_NV_OPENGL_FREE_SHARED_MEMORY          0x000A // free shared memory area
#define ESC_NV_OPENGL_GET_DISPLAY_PITCH           0x000B // return pitch of display
#define ESC_NV4_OPENGL_SWAP_BUFFERS               0x000C // NV4 swap buffers...
#define ESC_NV4_OPENGL_FLUSH                      0x000D // NV4 flush...
#define ESC_NV_OPENGL_PURGE_DEVICE_BITMAP         0x000E // purge GDI device bitmaps...
#define ESC_NV_OPENGL_FLUSH                       0x000F // NV10 and up style flush...
#define ESC_NV_OPENGL_SWAP_BUFFERS                0x0010 // NV10 and up style swap buffers...
#define ESC_NV_OPENGL_CPUBLIT                     0x0011 // Generic CPU blit. Currently used for buffer region Ext.
#define ESC_NV_OPENGL_INIT_STATE                  0x0012 // Initialize HW state from the display driver side
#define ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES      0x0021 // alloc single back/depth in GDI display driver
#define ESC_NV_OPENGL_FREE_UNIFIED_SURFACES       0x0022 // free single back/depth in GDI display driver
#define ESC_NV_OPENGL_GET_SURFACE_INFO            0x0023 // get offset for single back
#define ESC_NV_OPENGL_DID_STATUS_CHANGE           0x0029 // clip list status call
#define ESC_NV_OPENGL_GET_CLIP_LIST_COUNT         0x002A // return count of rectangles in clip list
#define ESC_NV_OPENGL_GET_CLIP_LIST               0x002B // return list of rectangles for window clip
#define ESC_NV_OPENGL_SUPPORT                     0x002C // return TRUE if OpenGL supported...
#define ESC_NV_OPENGL_REGISTER_RM_CLIENT          0x002D // register RM client with display driver
#define ESC_NV_OPENGL_UNREGISTER_RM_CLIENT        0x002E // unregister RM client with display driver
#define ESC_NV_OPENGL_SET_DAC_BASE                0x002F // set the display base offset
#define ESC_NV_OPENGL_SYNC_CHANNEL                0x0030 // wait for FIFO and graphics engine to be !busy
#define ESC_NV_OPENGL_SET_LAYER_PALETTE           0x0031 // Overlay specific: set palette entries in the DD
#define ESC_NV_OPENGL_GET_LAYER_PALETTE           0x0032 // Overlay specific: get palette entries from the DD
#define ESC_NV_OPENGL_OVERLAY_MERGEBLIT           0x0033 // Overlay specific: do the merge blit fm+fo->scratch->primary
#define ESC_NV_OPENGL_PFD_CHECK                   0x0034 // Retrieve some caps from the driver
#define ESC_NV_OPENGL_COMMAND                     0x0035 // Command interface for e.g. locked clears
#define ESC_NV_OPENGL_SHARED_LIBRARY_SIZES        0x0036 // check __GLNVstate and __GLdrawablePrivate sizes
#define ESC_NV_OPENGL_NT4_GET_NUMBER_OF_MONITORS  0x0037 // multimon escape for NT4 
#define ESC_NV_OPENGL_NT4_GET_DEVICE_HANDLE       0x0038 // multimon escape for NT4
#define ESC_NV_OPENGL_NT4_GET_DEVICE_RECT         0x0039 // multimon escape for NT4
#define ESC_NV_OPENGL_NT4_GDI_LOCK                0x0040 // multimon escape for NT4

#define ESC_NV_CONVERT_RING3_TO_RING0             0x7200

#undef ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY // uses a separate escape...
#define ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY     0x7102


#define ESC_NV_QUERYSET_REGISTRY_KEY              0x7103
#define NV_QUERY_REGISTRY_KEY                     0x0001
#define NV_SET_REGISTRY_KEY                       0x0002
#define NV_QUERY_REGISTRY_BINARY_KEY              0x0004


#define ESC_NV_MAP_USER                 0x9000
#define ESC_NV_UNMAP_USER               0x9001

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

/* The defines for the escapes */
#define	OPENGL_CMD              4352
#define	OPENGL_GETINFO          4353

// Input structure for OPENGL_CMD ExtEscape.

typedef struct _OPENGLCMD
{
    ULONG    ulSubEsc;
    ULONG    fl;
    struct _WNDOBJ   *pwo;
    struct _XLATEOBJ *pxo;
} OPENGLCMD, *POPENGLCMD;

// Flags for OPENGL_CMD ExtEscape.

#define OGLCMD_NEEDWNDOBJ       0x01
#define OGLCMD_NEEDXLATEOBJ     0x02


#define NV_ESC_IS_64BIT_CLIENT   'SIX4'
#define NV_ESC_IS_32BIT_CLIENT   '3TWO'

#if defined(_WIN64)
#define NV_ESC_CLIENT_MODEL NV_ESC_IS_64BIT_CLIENT
#else
#define NV_ESC_CLIENT_MODEL NV_ESC_IS_32BIT_CLIENT
#endif

typedef struct _NV_OPENGL_COMMAND
{
    union                            // offset == 0 
    {
        OPENGLCMD openGLCmd;         // header needed for MS OpenGL escape call                                     
        ULONG     spaceUnifier[8];   // used to make sure the next element is aligned 
                                     // to 4x8bytes in 64 bit and 32 bit environment
    };
                                     // offset == 32
    ULONG       NVClientModel;       // can have only two values NV_ESC_IS_32BIT_CLIENT or NV_ESC_IS_64BIT_CLIENT 
                                     // to make sure it's used in a reliant way
                                     // Note: We can't or NV_ESC_64_BASE onto the escape cmd 
                                     // as long as we use the OPENGL_CMD escape 
                                     // To make life a bit easier the flag has an offset 
                                     // of 32bytes for both 64 and 32 bit Windows (guaranteed by spaceUnifier)
    ULONG       NVSignature;         // offset == 36
    ULONG       command;             // Handle to resource manager client used in NvFree
    ULONG       numDevices;          // Number of devices in the system
    union                            // offset == 48 (must be 8 byte aligned for 64bit Windows)
    { 
        HWND        hWnd;            // Window handle
        __int64     hWndSpaceUnifier;// used to make sure the next element has equal offsets on 
                                     // 64 and 32 bit Windows
    };                                     
    union                            // offset == 56 (must be 8 byte aligned for 64bit Windows)
    { 
        HDC         hDC;             // GDI device context handle
        __int64     hDCSpaceUnifier; // used to make sure the next element has equal offsets on 
                                     // 64 and 32 bit Windows
    };
    ULONG       hClient;             // Handle to resource manager client
    ULONG       processHandle;       // Process handle from client associated with globalData
    union                            // offset == 72 (must be 8 byte aligned for 64bit Windows)
    { 
        PVOID       globalData;      // Pointer to global shared data for destroy
        __int64     gDSpaceUnifier;  // used to make sure the next element has equal offsets on 
                                     // 64 and 32 bit Windows
    };
    union                            // offset == 80 (must be 8 byte aligned for 64bit Windows)
    { 
        PVOID       other;           // Pointer to whatever
        __int64     otherSpaceUnifier;// used to make sure the next element has equal offsets on 
                                     // 64 and 32 bit Windows
    };
    ULONG       devID;               // DevID of current device for NT4 multimon 
    ULONG       useOpenGLCmdEscape;  // use OPENGL_CMD escape if non zero (flush on front buffer 
                                     // rendering, clear, swap buffers)
    LONG        devOriginLeft;       // left origin of device: needed for multimon to identify device  (W2K)
    LONG        devOriginTop;        // top  origin of device: needed for multimon to identify device  (W2K)

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
    PIXELFORMATDESCRIPTOR pfd;
    ULONG       devID;      // DevID of current device for NT4 multimon 
} NV_WNDOBJ_SETUP;

#define NV_CLIP_MUTEX_WAIT_IN_SECONDS       10 // 10 milliseconds
#define NV_WINDOW_CLIP_FULLYVISIBLE         0x0001
#define NV_WINDOW_CLIP_OCCLUDED             0x0002
#define NV_WINDOW_CLIP_SINGLERECTANGLE      0x0004
#define NV_WINDOW_CLIP_COMPLEX              0x0008
#define NV_WINDOW_CLIP_FULLSCREEN           0x0010
#define MAX_OGL_CLIP_RECTS                  250
#define MAX_OGL_CLIENTS                     512

// The following structure lives in shared memory between the display driver
// and OpenGL.  Put shared items in the front to ensure that data items line
// up between a 64 bit display driver and a 32 bit OpenGL client.
// In addition, only the first page (4096 bytes on most systems) is shared.
                                        // 
typedef struct _NV_OPENGL_DRAWABLE_INFO
{
                                        // BEGIN SHARED AREA

    ULONG       clipChangedCount;       // Number of times clip has changed since client registered
    ULONG       clipFlags;              // Clip status for last clip
    ULONG       numClipRects;           // Number of clip rectangles

    ULONG       flipChangedCount;       // Number of times flip has changed 
    int         bFlipping;              // TRUE=client is flipping; FALSE=client is blitting

    ULONG       ulSwapDoneByClient;     // set if the ICD takes care of the swap
                                        // END SHARED AREA

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
    RECTL       windowRect[MAX_OGL_CLIP_RECTS];
                                        // Clip region for client window
    PVOID       origClientDrawableInfoPtr; 
                                        // Original Ptr remembered to free memory

    RECTL       *cachedClipRectList;    // Cache the clip list so that we do not have to merge it
    ULONG       cachedNumClipRects;     // Number of rectangles.
    ULONG       cachedClipListSize;     // Currently allocated size of the clip list.
    BOOL        cachedClipListDirty;    // Flag to determine whether to recompute clip list.
    RECTL       *cachedExclusiveRectList;    // Cache the clip list so that we do not have to merge it
    ULONG       cachedNumExclusiveRects;     // Number of rectangles.
    RECTL       *copyWindowRect;           // Actual clip rectangle pointer used everywhere in NV4+ driver
    ULONG       sizeCopyWindowRectList;    // Size of clip rectangle pointer above.

    ULONG       bOrigClipList;            // TRUE means the original clip list is cached in this structure
    RECTL       *origWindowRectList;      // cache of the original unclipped rectangle list for this window

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
    int         overlayMember;
    } NV_OPENGL_DRAWABLE_INFO;

typedef struct _NV_OPENGL_CONTEXT_INFO
    {
    HWND        hWnd;                   // Window/drawable associated with context
    HDC         hDC;                    // GDI device context
    ULONG       hClient;                // Handle to resource manager client
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

#define OGL_ERROR_CLIP_CHANGE_NOTIFY   0x000C


#define NV_BLIT_MEM_TO_SCR 1
#define NV_BLIT_SCR_TO_MEM 2
typedef struct {
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
    ULONG       devID;               // DevID of current device for NT4 multimon 
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


// ESC_NV_WKS_APP_SUPPORT - BEGIN
//
// Workstation application support escape
//
// The ESC_NV_WKS_APP_SUPPORT escape supports a couple of sub escapes through
// a common interface!
//
// WARNING:
//   Never change the escape number ESC_NV_WKS_APP_SUPPORT! 
//   This will break QuadroView, MaxTreme, PowerDraft and all other workstation apps!
//
// NOTES: 
// - all ExtEscape in-structures need to use NV_WKS_HEAD_IN at top of the structure
// - all out structures need to use NV_WKS_HEAD_OUT at top of the structure
// - all in and out structures must be initialized with 0 to clear unused members
// - changes to the interface structures must be downwards compatible
// - never change the size of a existing structure or the arrangement of the members
// - always check the exact strucure size and fail if not the same
// - if lResult returns an error only the NV_WKS_HEAD_OUT struct is valid and the 
//   members of the structure are used to specify the error (see error description)
#define ESC_NV_WKS_APP_SUPPORT          0x7108

//
// supported subescapes
//
                                          // 0x0000 unused
#define SUBESC_WKS_GET_CAPS                  0x0001 // uses NV_WKS_GET_CAPS_IN as in and NV_WKS_GET_CAPS_OUT as out structure 
#define SUBESC_WKS_WINDOW_STEREOMODE         0x0002 // uses NV_WKS_WINDOW_STEREOMODE_IN as in, and NULL as NV_WKS_WINDOW_STEREOMODE_OUT as out structure
//      add new subescapes here:             0x0003


//
// Common in and out header structures that must be used by all in and out structures
//

// must be used by all in structures
typedef struct _NV_WKS_HEAD_IN
{
    ULONG ulInSize;     // Length of entire in structure, e.g. sizeof(NV_WKS_GET_CAPS_IN)
    ULONG ulSubEsc;     // has to be (SUBESC_WKS_GET_CAPS, ...)
    ULONG ulOutSize;    // size of entire output data structure or 0 ()
    ULONG ulVersion;    // initialize with NV_WKS_VERSION; Version of ESC_NV_WKS_APP_SUPPORT interface
                        
    ULONG ulBoard;      // Calls that affect all boards use NV_WKS_ALLBOARDS to initialize dwBoard
                        // 0 -> board 0, 1 -> board 1, ET_ALL_BOARDS -> all boards (broadcast)  

    // reserved members are not allowed to be used!
    ULONG ulReserved0;
    ULONG ulReserved1;  // reserved for future extensions
    ULONG ulReserved2;
} NV_WKS_HEAD_IN;

// must be used by all out structures
typedef struct _NV_WKS_HEAD_OUT
{
    ULONG ulSize;       // Length of entire out structure, e.g. sizeof(NV_WKS_GET_CAPS_OUT)
                        // Should be same as NV_WKS_HEAD_IN.ulOutSize!
                        // NOTE: In error case used perhaps other meaning (see error description)!
    ULONG ulVersion;    // initialize with NV_WKS_VERSION; Version of ESC_NV_WKS_APP_SUPPORT interface
    LONG  lResult;      // If escapes succeeds ulResult contains more status information

    // reserved members are not allowed to be used!
    ULONG ulReserved0;  // reserved for future extensions
} NV_WKS_HEAD_OUT;

//
// defines for header variables
//
// NV_WKS_HEAD_IN.ulVersion, NV_WKS_HEAD_OUT.ulVersion
#define NV_WKS_VERSION                          1          // intial version

// NV_WKS_HEAD_IN.ulBoard
#define NV_WKS_ALLBOARDS                        0xFFFFFFFF // broadcast to all boards, 

// return values for NV_WKS_HEAD_OUT.lResult

// Success values lResult > 0
#define NV_WKS_RESULT_SUCCESS                      1 // wks escape succeeded

#define NV_WKS_RESULT_NOT_ALLOWED                  0 // unused valued, HEAD_OUT isn't valid at all!

// Error values lResult < 0
#define NV_WKS_RESULT_ERROR                       -1 // any unspecified error
#define NV_WKS_RESULT_ERROR_SUBESC_NOT_SUPPORTED  -2 // unsupported subescape
#define NV_WKS_RESULT_ERROR_INSIZE                -3 // input buffer has wrong size, expected size in NV_WKS_HEAD_OUT.ulSize
#define NV_WKS_RESULT_ERROR_OUTSIZE               -4 // output buffer has wrong size, expected size in NV_WKS_HEAD_OUT.ulSize
#define NV_WKS_RESULT_ERROR_INTERFACE             -5 // inconsisty in usage of interface, values in HEAD_IN do not match cjIn, cjOut, ...
#define NV_WKS_RESULT_ERROR_PARAMETER             -6 // interface correct, but a parameter is wrong and the escape couldn't be handled

// define that c
#define NV_WKS_INIT_HEAD_IN(pHdr, subEsc, sizeIn, sizeOut, board)   \
    ((NV_WKS_HEAD_IN *)(pHdr))->ulSubEsc  = subEsc;                 \
    ((NV_WKS_HEAD_IN *)(pHdr))->ulInSize  = sizeIn;                 \
    ((NV_WKS_HEAD_IN *)(pHdr))->ulOutSize = sizeOut;                \
    ((NV_WKS_HEAD_IN *)(pHdr))->ulVersion = NV_WKS_VERSION;         \
    ((NV_WKS_HEAD_IN *)(pHdr))->ulBoard   = board;


//
// SUBESC_WKS_GET_CAPS 
//
// Query workstation graphics capabilities.
//
// Example code can be found in .\drivers\display\winnt4\displays\nv_disp\wksappsupport.c
//
// input structure
typedef struct _NV_WKS_GET_CAPS_IN
{
    NV_WKS_HEAD_IN hdr;
} NV_WKS_GET_CAPS_IN;

// output structure
typedef struct _NV_WKS_GET_CAPS_OUT
{
    NV_WKS_HEAD_OUT hdr;

    ULONG flGraphicsCaps; // NV_WKS_GRAPHICS_CAPS_*, nearly same as NvRmConfigGet(NV_CFG_GRAPHICS_CAPS)
    ULONG ulMaxClipRects; // number of supported clip rects
    ULONG flWksCaps;      // NV_WKS_CAPS_*, workstation capabilities

    // reserved members are not allowed to be used!
    ULONG ulReserved0;    // reserved for future extensions
} NV_WKS_GET_CAPS_OUT;

// defines used in NV_WKS_GET_CAPS_OUT.flGraphicsCaps
#define NV_WKS_GRAPHICS_CAPS_AA_LINES           0x00000001 //NOTE: != NV_CFG_GRAPHICS_CAPS_AA_LINES
#define NV_WKS_GRAPHICS_CAPS_AA_POLYS           0x00000002
#define NV_WKS_GRAPHICS_CAPS_AKILL_USERCLIP     0x00000004
#define NV_WKS_GRAPHICS_CAPS_LOGIC_OPS          0x00000008
#define NV_WKS_GRAPHICS_CAPS_NV15_ALINES        0x00000010
#define NV_WKS_GRAPHICS_CAPS_2SIDED_LIGHTING    0x00000020
#define NV_WKS_GRAPHICS_CAPS_QUADRO_GENERIC     0x00000040
#define NV_WKS_GRAPHICS_CAPS_UBB                0x00000080

// defines used in NV_WKS_GET_CAPS_OUT.flWksCaps
#define NV_WKS_CAPS_WORKSTATION_BOARD           0x00000001
#define NV_WKS_CAPS_STERO_CAPABLE               0x00000010
#define NV_WKS_CAPS_OVERLAY_CAPABLE             0x00000020


//
// SUBESC_WKS_WINDOW_STEREOMODE
//
// Interface for workstation apps to enable/disable stereo toggle and 
// double pumping for a given window wiht a stereo-pfd
//
// Example code can be found in .\drivers\display\winnt4\displays\nv_disp\wksappsupport.c
//
// input structure
typedef struct _NV_WKS_WINDOW_STEREOMODE_IN
{
    NV_WKS_HEAD_IN hdr;

    PVOID          hwnd;   // handle of window to modify/query. If NULL current driver status is returned
    ULONG          ulMode; // NV_WKS_WINDOW_STEREOMODE_QUERY, NV_WKS_WINDOW_STEREOMODE_ENABLE, NV_WKS_WINDOW_STEREOMODE_DISABLE
} NV_WKS_WINDOW_STEREOMODE_IN;

// output structure
typedef struct _NV_WKS_WINDOW_STEREOMODE_OUT
{
    NV_WKS_HEAD_OUT hdr;

    PVOID           hwnd;   // handle of window queried, or NULL if not specified.
    ULONG           ulMode; // NV_WKS_WINDOW_STEREOMODE_ENABLED, NV_WKS_WINDOW_STEREOMODE_DISABLED, ...
} NV_WKS_WINDOW_STEREOMODE_OUT;

// IN.ulMode
#define NV_WKS_WINDOW_STEREOMODE_DISABLE           0 // disable stereo mode for IN.hwnd (must be valid stereo window)
#define NV_WKS_WINDOW_STEREOMODE_ENABLE            1 // enable stereo mode for IN.hwnd (must be valid stereo window)
#define NV_WKS_WINDOW_STEREOMODE_QUERY             2 // query status of IN.hwnd or system if IN.hwnd==NULL

// OUT.ulMode
#define NV_WKS_WINDOW_STEREOMODE_UNUSED            0 // value not set by driver, ignore value
#define NV_WKS_WINDOW_STEREOMODE_DISABLED          1 // stereo is disabled for OUT.hwnd
#define NV_WKS_WINDOW_STEREOMODE_ENABLED           3 // stereo is enabled for OUT.hwnd
#define NV_WKS_WINDOW_STEREOMODE_INVALID_HWND      4 // couldn't find hwnd in client list
#define NV_WKS_WINDOW_STEREOMODE_NO_STEREO_HWND    5 // hwnd isn't a stero window
#define NV_WKS_WINDOW_STEREOMODE_NO_STEREO         6 // currently no stereo active


// ESC_NV_WKS_APP_SUPPORT - END

#endif // _WIN32

#endif // NV_ESC_DEF_INCLUDED

